/*
  Module Intent: WebSocket gateway implementation for GMCP-to-JSON bridging.

  This module provides a WebSocket server that bridges GMCP protocol messages
  to JSON for web-based clients. It handles connection management, message
  serialization, and command translation to enable browser and mobile gameplay.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#include "merc.h"
#include "webgate.h"
#include "protocol.h"

/*
 * External function declarations from comm.c and interp.c
 */
void read_from_buffer(DESCRIPTOR_DATA *d);
void nanny(DESCRIPTOR_DATA *d, char *argument);

/*
 * Intent: Manage global state for web client connections and server socket.
 *
 * web_descriptor_list: Linked list of all active WebSocket connections
 * webgate_listen_fd: File descriptor for the listening WebSocket server socket
 *
 * These maintain the active connection pool and allow iteration during the
 * main game loop for I/O multiplexing.
 */
WEB_DESCRIPTOR_DATA *web_descriptor_list = NULL;
int webgate_listen_fd = -1;

/*
 * Forward declarations for internal helper functions
 */
static void webgate_read(WEB_DESCRIPTOR_DATA *web_desc);
static void webgate_write(WEB_DESCRIPTOR_DATA *web_desc);
static void webgate_serialize_extra_flags(OBJ_DATA *obj, bool identified, char *buf, int buf_size);
static void webgate_serialize_ego_flags(OBJ_DATA *obj, bool identified, char *buf, int buf_size);

static bool webgate_process_handshake(WEB_DESCRIPTOR_DATA *web_desc);
static bool webgate_parse_websocket_frames(WEB_DESCRIPTOR_DATA *web_desc);
static bool webgate_parse_json_message(WEB_DESCRIPTOR_DATA *web_desc, const char *json);
static void webgate_send_text_frame(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len);
static void webgate_send_raw(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len);
static WEB_DESCRIPTOR_DATA *webgate_new_descriptor(void);
static void webgate_free_descriptor(WEB_DESCRIPTOR_DATA *web_desc);
static DESCRIPTOR_DATA *webgate_create_mud_descriptor(WEB_DESCRIPTOR_DATA *web_desc);
static void webgate_send_prompt(WEB_DESCRIPTOR_DATA *web_desc, const char *prompt);
static void webgate_transfer_mud_output(WEB_DESCRIPTOR_DATA *web_desc);
static void webgate_parse_and_send_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *gmcp_msg);
static void webgate_send_text_as_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *text, int len);

/*
 * Intent: Initialize WebSocket gateway server and bind to specified port.
 *
 * Sets up a non-blocking TCP socket, binds it to the specified port, and begins
 * listening for incoming WebSocket connections. The socket is configured for
 * address reuse to support rapid server restarts during development.
 *
 * Inputs: port - TCP port number (typically 8080 for WebSocket)
 * Outputs: File descriptor of listening socket, or -1 on failure
 *
 * Preconditions: Port is available and caller has bind permissions
 * Postconditions: WebSocket server ready to accept connections
 *
 * Failure Behavior: Returns -1, logs detailed error. Server can continue
 *                   without web gateway functionality.
 */
int webgate_init(int port)
{
    struct sockaddr_in sa;
    int fd;
    int opt = 1;

    log_string("WebGate: Initializing WebSocket gateway...");

    /* Create TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("webgate_init: socket");
        return -1;
    }

    /* Set socket options: reuse address to allow quick restarts */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("webgate_init: setsockopt(SO_REUSEADDR)");
        close(fd);
        return -1;
    }

    /* Make socket non-blocking to integrate with game loop */
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("webgate_init: fcntl(O_NONBLOCK)");
        close(fd);
        return -1;
    }

    /* Bind to specified port on all interfaces */
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        perror("webgate_init: bind");
        close(fd);
        return -1;
    }

    /* Start listening for connections (backlog of 5) */
    if (listen(fd, 5) < 0)
    {
        perror("webgate_init: listen");
        close(fd);
        return -1;
    }

    webgate_listen_fd = fd;

    sprintf(log_buf, "WebGate: Listening on port %d", port);
    log_string(log_buf);

    return fd;
}

/*
 * Intent: Gracefully shut down the WebSocket gateway and clean up resources.
 *
 * Closes all active client connections, frees associated memory, and closes
 * the listening socket. Ensures no memory leaks and that all clients receive
 * proper disconnect notification.
 *
 * Preconditions: May be called even if webgate_init() failed
 * Postconditions: All WebSocket resources released; gateway inactive
 *
 * Failure Behavior: Best-effort cleanup; logs but continues on errors
 */
void webgate_shutdown(void)
{
    WEB_DESCRIPTOR_DATA *web_desc, *web_next;

    log_string("WebGate: Shutting down WebSocket gateway...");

    /* Close all active client connections */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_next)
    {
        web_next = web_desc->next;
        webgate_close(web_desc);
    }

    /* Close listening socket */
    if (webgate_listen_fd >= 0)
    {
        close(webgate_listen_fd);
        webgate_listen_fd = -1;
    }

    log_string("WebGate: Shutdown complete");
}

/*
 * Intent: Register WebSocket file descriptors with select() for I/O multiplexing.
 *
 * Adds the listening socket and all active client sockets to the appropriate
 * fd_sets so the main game loop can monitor them for readability/writability.
 * Updates maxfd to ensure select() checks all relevant descriptors.
 *
 * Inputs:
 *   - readfds: Set of file descriptors to monitor for reading
 *   - writefds: Set of file descriptors to monitor for writing
 *   - maxfd: Pointer to current maximum fd value (updated if needed)
 *
 * Outputs: Updated fd_sets and maxfd value
 *
 * Preconditions: fd_sets are initialized; called before select()
 * Postconditions: All WebSocket descriptors registered for monitoring
 */
void webgate_register_fds(fd_set *readfds, fd_set *writefds, int *maxfd)
{
    WEB_DESCRIPTOR_DATA *web_desc;

    /* Register listening socket for new connections */
    if (webgate_listen_fd >= 0)
    {
        FD_SET(webgate_listen_fd, readfds);
        if (webgate_listen_fd > *maxfd)
        {
            *maxfd = webgate_listen_fd;
        }
    }

    /* Register all active client sockets */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->fd < 0)
        {
            continue;
        }

        /* Always monitor for input */
        FD_SET(web_desc->fd, readfds);

        /* Monitor for output if we have data to send */
        if (web_desc->outbuf_size > 0)
        {
            FD_SET(web_desc->fd, writefds);
        }

        if (web_desc->fd > *maxfd)
        {
            *maxfd = web_desc->fd;
        }
    }
}

/*
 * Intent: Process WebSocket I/O for all ready descriptors without blocking.
 *
 * Checks the listening socket for new connections and processes reads/writes
 * on active client sockets. This function integrates the WebSocket gateway
 * into the main game loop's I/O multiplexing.
 *
 * Inputs:
 *   - readfds: File descriptors ready for reading (from select)
 *   - writefds: File descriptors ready for writing (from select)
 *
 * Outputs: None (side effects: connections accepted, messages processed)
 *
 * Preconditions: select() has returned; fd_sets contain ready descriptors
 * Postconditions: All ready I/O processed; connection states updated
 *
 * Failure Behavior: Individual connection errors handled gracefully;
 *                   bad connections closed and removed from list
 */
void webgate_process(fd_set *readfds, fd_set *writefds)
{
    WEB_DESCRIPTOR_DATA *web_desc, *web_next;

    /* Accept new connections */
    if (webgate_listen_fd >= 0 && FD_ISSET(webgate_listen_fd, readfds))
    {
        WEB_DESCRIPTOR_DATA *new_desc = webgate_accept(webgate_listen_fd);
        if (new_desc)
        {
            sprintf(log_buf, "WebGate: New connection from fd %d", new_desc->fd);
            log_string(log_buf);
        }
    }

    /* Process existing connections */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_next)
    {
        web_next = web_desc->next;

        if (web_desc->fd < 0)
        {
            continue;
        }

        /* Transfer output from MUD descriptor to web client */
        if (web_desc->mud_desc)
        {
            webgate_transfer_mud_output(web_desc);
        }

        /* Handle input */
        if (FD_ISSET(web_desc->fd, readfds))
        {
            webgate_read(web_desc);

            /* Connection may be closed after read */
            if (web_desc->state == WS_STATE_CLOSED)
            {
                webgate_close(web_desc);
                continue;
            }
        }

        /* Handle output */
        if (FD_ISSET(web_desc->fd, writefds))
        {
            webgate_write(web_desc);
        }

        /* TODO: Implement ping/pong timeouts */
        /* Check for idle connections and send keep-alive pings */
    }
}

/*
 * Intent: Accept a new WebSocket connection from the listening socket.
 *
 * Accepts the TCP connection, creates a new web descriptor to track state,
 * and adds it to the active connection list. The connection begins in
 * handshake state awaiting the WebSocket upgrade request.
 *
 * Inputs: listen_fd - The listening socket file descriptor
 * Outputs: New WEB_DESCRIPTOR_DATA structure, or NULL on error
 *
 * Preconditions: listen_fd is ready for accept (select indicated)
 * Postconditions: New client added to descriptor list; ready for handshake
 *
 * Failure Behavior: Returns NULL, logs error; no state corruption
 */
WEB_DESCRIPTOR_DATA *webgate_accept(int listen_fd)
{
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    int fd;
    WEB_DESCRIPTOR_DATA *web_desc;

    /* Accept the connection */
    fd = accept(listen_fd, (struct sockaddr *)&sa, &sa_len);
    if (fd < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("webgate_accept: accept");
        }
        return NULL;
    }

    /* Make socket non-blocking */
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        perror("webgate_accept: fcntl");
        close(fd);
        return NULL;
    }

    /* Allocate and initialize web descriptor */
    web_desc = webgate_new_descriptor();
    if (!web_desc)
    {
        close(fd);
        return NULL;
    }

    web_desc->fd = fd;
    web_desc->state = WS_STATE_HANDSHAKE;
    web_desc->connect_time = current_time;
    web_desc->last_ping = current_time;

    /* Add to list */
    web_desc->next = web_descriptor_list;
    web_descriptor_list = web_desc;

    return web_desc;
}

/*
 * Intent: Close a WebSocket connection and free associated resources.
 *
 * Sends proper WebSocket close frame (if connection is established),
 * closes the socket, frees allocated memory, and removes descriptor
 * from the active list. Ensures clean disconnect visible to client.
 *
 * Inputs: web_desc - Web descriptor to close
 * Outputs: None
 *
 * Preconditions: web_desc is in the active descriptor list
 * Postconditions: Connection closed; memory freed; removed from list
 *
 * Failure Behavior: Best-effort cleanup; ensures list integrity
 */
void webgate_close(WEB_DESCRIPTOR_DATA *web_desc)
{
    WEB_DESCRIPTOR_DATA *prev;

    if (!web_desc)
    {
        return;
    }

    sprintf(log_buf, "WebGate: Closing connection fd %d", web_desc->fd);
    log_string(log_buf);

    /* Close socket */
    if (web_desc->fd >= 0)
    {
        close(web_desc->fd);
    }

    /* Remove from list */
    if (web_desc == web_descriptor_list)
    {
        web_descriptor_list = web_desc->next;
    }
    else
    {
        for (prev = web_descriptor_list; prev; prev = prev->next)
        {
            if (prev->next == web_desc)
            {
                prev->next = web_desc->next;
                break;
            }
        }
    }

    /* Free resources */
    webgate_free_descriptor(web_desc);
}

/*
 * Intent: Send a GMCP message as JSON to a specific web client.
 *
 * Formats a WebSocket frame containing a JSON message in GMCP style
 * (module + data) and queues it for transmission. The actual send
 * happens asynchronously in webgate_process().
 *
 * Inputs:
 *   - web_desc: Target client connection
 *   - module: GMCP module name (e.g., "Char.Vitals", "Room.Info")
 *   - json_body: JSON object body as string
 *
 * Outputs: None (message queued in outbuf)
 *
 * Preconditions: web_desc is authenticated and in OPEN state
 * Postconditions: Message added to output buffer
 *
 * Failure Behavior: Silently drops if client disconnected or buffer full
 *
 * Example:
 *   webgate_send_gmcp(desc, "Char.Vitals", "{\"hp\":50,\"maxhp\":100}");
 */
void webgate_send_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *module, const char *json_body)
{
    char message[WEBGATE_MAX_MESSAGE];
    int len;

    if (!web_desc || web_desc->state != WS_STATE_OPEN)
    {
        return;
    }

    if (!module || !json_body)
    {
        return;
    }

    /* Format as GMCP-style JSON message */
    len = snprintf(message, sizeof(message),
                   "{\"type\":\"gmcp\",\"module\":\"%s\",\"data\":%s}\n",
                   module, json_body);

    if (len >= sizeof(message))
    {
        bug("webgate_send_gmcp: message truncated", 0);
        return;
    }

    /* Send as WebSocket text frame */
    webgate_send_text_frame(web_desc, message, len);
}

/*
 * Intent: Send room information to web client via GMCP Room.Info message.
 *
 * Formats and sends complete room data including name, description, exits,
 * and vnum to the web client. This enables the client to update the UI with
 * current room information and populate the navigation compass.
 *
 * Inputs:
 *   - web_desc: Target web client connection
 *   - room: Room index data containing room details
 *
 * Outputs: None (GMCP message queued for send)
 *
 * Preconditions: web_desc is authenticated; room is valid pointer
 * Postconditions: Room.Info GMCP message sent to client
 *
 * Failure Behavior: Silently returns if web_desc or room is NULL
 *
 * Notes: JSON strings are escaped to prevent syntax errors. Large descriptions
 *        may be truncated to fit WEBGATE_MAX_MESSAGE limit.
 */
void webgate_send_room_info(WEB_DESCRIPTOR_DATA *web_desc, ROOM_INDEX_DATA *room)
{
    char json[WEBGATE_MAX_MESSAGE];
    char exits_json[2048]; /* Increased from 512 to handle 6 exits with door info */
    char items_json[MAX_STRING_LENGTH];
    char npcs_json[MAX_STRING_LENGTH];
    char name_escaped[256];
    char desc_escaped[MAX_STRING_LENGTH];
    char temp_name[256];
    EXIT_DATA *pexit;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;
    bool first = true;
    int i, j, item_count = 0, npc_count = 0;
    extern DIR_DATA directions[];

    if (!web_desc || !room)
        return;

    /* Escape room name for JSON */
    for (i = 0, j = 0; room->name[i] != '\0' && j < sizeof(name_escaped) - 2; i++)
    {
        if (room->name[i] == '"' || room->name[i] == '\\')
        {
            name_escaped[j++] = '\\';
        }
        name_escaped[j++] = room->name[i];
    }
    name_escaped[j] = '\0';

    /* Escape room description for JSON (truncate if too long) */
    for (i = 0, j = 0; room->description[i] != '\0' && j < sizeof(desc_escaped) - 2 && i < 1000; i++)
    {
        if (room->description[i] == '"')
        {
            desc_escaped[j++] = '\\';
            desc_escaped[j++] = '"';
        }
        else if (room->description[i] == '\\')
        {
            desc_escaped[j++] = '\\';
            desc_escaped[j++] = '\\';
        }
        else if (room->description[i] == '\n')
        {
            desc_escaped[j++] = '\\';
            desc_escaped[j++] = 'n';
        }
        else if (room->description[i] == '\r')
        {
            continue; /* Skip carriage returns */
        }
        else if (room->description[i] >= 32)
        {
            desc_escaped[j++] = room->description[i];
        }
    }
    desc_escaped[j] = '\0';

    /* Build exits array JSON */
    sprintf(exits_json, "[");
    for (door = 0; door < MAX_DIR; door++)
    {
        pexit = room->exit[door];
        if (pexit && pexit->to_room)
        {
            char door_keyword[128];
            const char *src;
            char *dst;
            bool has_door = IS_SET(pexit->exit_info, EX_ISDOOR);
            bool is_closed = IS_SET(pexit->exit_info, EX_CLOSED);
            bool is_locked = IS_SET(pexit->exit_info, EX_LOCKED);
            bool is_pickproof = IS_SET(pexit->exit_info, EX_PICKPROOF);

            /* Escape door keyword if exists */
            if (has_door && pexit->keyword && pexit->keyword[0] != '\0')
            {
                src = pexit->keyword;
                dst = door_keyword;
                while (*src && (dst - door_keyword) < sizeof(door_keyword) - 2)
                {
                    if (*src == '"' || *src == '\\')
                        *dst++ = '\\';
                    if (*src >= 32 && *src != ' ')
                        *dst++ = *src++;
                    else if (*src == ' ')
                        break; /* Only use first word */
                    else
                        src++;
                }
                *dst = '\0';
            }
            else
            {
                strcpy(door_keyword, "door");
            }

            if (!first)
                strcat(exits_json, ",");

            /* Check buffer space before appending */
            if (strlen(exits_json) + 200 < sizeof(exits_json))
            {
                sprintf(exits_json + strlen(exits_json),
                        "{\"dir\":\"%s\",\"hasDoor\":%s,\"isClosed\":%s,\"isLocked\":%s,\"isPickproof\":%s,\"keyword\":\"%s\"}",
                        directions[door].name,
                        has_door ? "true" : "false",
                        is_closed ? "true" : "false",
                        is_locked ? "true" : "false",
                        is_pickproof ? "true" : "false",
                        door_keyword);
                first = false;
            }
            else
            {
                bug("webgate_send_room_info: exits_json buffer overflow", 0);
                break;
            }
        }
    }
    strcat(exits_json, "]");

    /* Build items array JSON - limit to 20 items */
    sprintf(items_json, "[");
    first = true;
    for (obj = room->contents; obj && item_count < 20; obj = obj->next_content)
    {
        char keywords_escaped[256];

        /* Escape item short description (for display) */
        for (i = 0, j = 0; obj->short_descr[i] != '\0' && j < sizeof(temp_name) - 2; i++)
        {
            if (obj->short_descr[i] == '"' || obj->short_descr[i] == '\\')
                temp_name[j++] = '\\';
            if (obj->short_descr[i] >= 32)
                temp_name[j++] = obj->short_descr[i];
        }
        temp_name[j] = '\0';

        /* Escape item keywords (for commands) - use first keyword only */
        for (i = 0, j = 0; obj->name[i] != '\0' && obj->name[i] != ' ' && j < sizeof(keywords_escaped) - 2; i++)
        {
            if (obj->name[i] == '"' || obj->name[i] == '\\')
                keywords_escaped[j++] = '\\';
            if (obj->name[i] >= 32)
                keywords_escaped[j++] = obj->name[i];
        }
        keywords_escaped[j] = '\0';

        /* Serialize item flags for visual effects */
        char extra_flags_json[512];
        char ego_flags_json[512];
        webgate_serialize_extra_flags(obj, obj->identified, extra_flags_json, sizeof(extra_flags_json));
        webgate_serialize_ego_flags(obj, obj->identified, ego_flags_json, sizeof(ego_flags_json));

        if (!first)
            strcat(items_json, ",");
        sprintf(items_json + strlen(items_json),
                "{\"id\":%d,\"name\":\"%s\",\"keywords\":\"%s\",\"vnum\":%d,\"type\":%d,"
                "\"identified\":%s,\"extraFlags\":%s,\"egoFlags\":%s}",
                obj->pIndexData ? obj->pIndexData->vnum : 0,
                temp_name,
                keywords_escaped,
                obj->pIndexData ? obj->pIndexData->vnum : 0,
                obj->item_type,
                obj->identified ? "true" : "false",
                extra_flags_json,
                ego_flags_json);
        first = false;
        item_count++;
    }
    strcat(items_json, "]");

    /* Build NPCs array JSON - limit to 20 NPCs, exclude the player's character */
    sprintf(npcs_json, "[");
    first = true;
    for (rch = room->people; rch && npc_count < 20; rch = rch->next_in_room)
    {
        char keywords_escaped[256];

        /* Skip the player's own character */
        if (!IS_NPC(rch) && web_desc->mud_desc && rch == web_desc->mud_desc->character)
            continue;

        /* Escape NPC short description (for display) */
        for (i = 0, j = 0; rch->short_descr[i] != '\0' && j < sizeof(temp_name) - 2; i++)
        {
            if (rch->short_descr[i] == '"' || rch->short_descr[i] == '\\')
                temp_name[j++] = '\\';
            if (rch->short_descr[i] >= 32)
                temp_name[j++] = rch->short_descr[i];
        }
        temp_name[j] = '\0';

        /* Escape NPC keywords (for commands) - use first keyword only */
        for (i = 0, j = 0; rch->name[i] != '\0' && rch->name[i] != ' ' && j < sizeof(keywords_escaped) - 2; i++)
        {
            if (rch->name[i] == '"' || rch->name[i] == '\\')
                keywords_escaped[j++] = '\\';
            if (rch->name[i] >= 32)
                keywords_escaped[j++] = rch->name[i];
        }
        keywords_escaped[j] = '\0';

        if (!first)
            strcat(npcs_json, ",");
        sprintf(npcs_json + strlen(npcs_json),
                "{\"id\":%d,\"name\":\"%s\",\"keywords\":\"%s\",\"vnum\":%d,\"hostile\":%s,\"isShopkeeper\":%s,\"isTrainer\":%s,\"isHealer\":%s}",
                IS_NPC(rch) && rch->pIndexData ? rch->pIndexData->vnum : 0,
                temp_name,
                keywords_escaped,
                IS_NPC(rch) && rch->pIndexData ? rch->pIndexData->vnum : 0,
                IS_NPC(rch) && IS_SET(rch->act, ACT_AGGRESSIVE) ? "true" : "false",
                IS_NPC(rch) && rch->pIndexData && rch->pIndexData->pShop ? "true" : "false",
                IS_NPC(rch) && IS_SET(rch->act, ACT_PRACTICE) ? "true" : "false",
                IS_NPC(rch) && IS_SET(rch->act, ACT_IS_HEALER) ? "true" : "false");
        first = false;
        npc_count++;
    }
    strcat(npcs_json, "]");

    /* Build extra descriptions array JSON */
    char extra_descr_json[MAX_STRING_LENGTH];
    EXTRA_DESCR_DATA *ed;
    sprintf(extra_descr_json, "[");
    first = true;
    for (ed = room->extra_descr; ed; ed = ed->next)
    {
        char keyword_escaped[256];
        char description_escaped[MAX_STRING_LENGTH];

        if (!ed->keyword || !ed->description)
            continue;

        /* Escape keyword for JSON */
        for (i = 0, j = 0; ed->keyword[i] != '\0' && j < sizeof(keyword_escaped) - 2; i++)
        {
            if (ed->keyword[i] == '"' || ed->keyword[i] == '\\')
                keyword_escaped[j++] = '\\';
            if (ed->keyword[i] >= 32)
                keyword_escaped[j++] = ed->keyword[i];
        }
        keyword_escaped[j] = '\0';

        /* Escape description for JSON */
        for (i = 0, j = 0; ed->description[i] != '\0' && j < sizeof(description_escaped) - 2 && i < 1000; i++)
        {
            if (ed->description[i] == '"')
            {
                description_escaped[j++] = '\\';
                description_escaped[j++] = '"';
            }
            else if (ed->description[i] == '\\')
            {
                description_escaped[j++] = '\\';
                description_escaped[j++] = '\\';
            }
            else if (ed->description[i] == '\n')
            {
                description_escaped[j++] = '\\';
                description_escaped[j++] = 'n';
            }
            else if (ed->description[i] == '\r')
            {
                continue; /* Skip carriage returns */
            }
            else if (ed->description[i] >= 32)
            {
                description_escaped[j++] = ed->description[i];
            }
        }
        description_escaped[j] = '\0';

        if (!first)
            strcat(extra_descr_json, ",");
        sprintf(extra_descr_json + strlen(extra_descr_json),
                "{\"keyword\":\"%s\",\"description\":\"%s\"}",
                keyword_escaped,
                description_escaped);
        first = false;
    }
    strcat(extra_descr_json, "]");

    /* Get area name for map display */
    char area_name_escaped[256];
    const char *area_name = room->area ? room->area->name : "Unknown";
    for (i = 0, j = 0; area_name[i] != '\0' && j < sizeof(area_name_escaped) - 2; i++)
    {
        if (area_name[i] == '"' || area_name[i] == '\\')
            area_name_escaped[j++] = '\\';
        if (area_name[i] >= 32)
            area_name_escaped[j++] = area_name[i];
    }
    area_name_escaped[j] = '\0';

    /* Build complete Room.Info JSON with items, NPCs, extra descriptions, and area name */
    snprintf(json, sizeof(json),
             "{\"name\":\"%s\",\"description\":\"%s\",\"exits\":%s,\"vnum\":%d,\"items\":%s,\"npcs\":%s,\"extraDescriptions\":%s,\"areaName\":\"%s\"}",
             name_escaped,
             desc_escaped,
             exits_json,
             room->vnum,
             items_json,
             npcs_json,
             extra_descr_json,
             area_name_escaped);

    sprintf(log_buf, "WebGate: Sending Room.Info (exits: %s, items: %d, npcs: %d, area: %s)",
            exits_json, item_count, npc_count, area_name_escaped);
    log_string(log_buf);

    webgate_send_gmcp(web_desc, "Room.Info", json);
}

/*
 * Intent: Notify all web clients in a room that room contents have changed.
 *
 * When NPCs move in/out of a room or items are added/removed, this function
 * sends updated Room.Info to all web clients present in the room so they
 * can refresh their displayed contents.
 *
 * Inputs:
 *   - room: The room that has changed
 *
 * Outputs: None (Room.Info messages sent to eligible clients)
 *
 * Preconditions: room must be valid
 * Postconditions: All web clients in room receive updated Room.Info
 *
 * Performance: O(n) where n is number of people in room
 */
void webgate_notify_room_update(ROOM_INDEX_DATA *room)
{
    CHAR_DATA *rch;
    WEB_DESCRIPTOR_DATA *web_desc;
    int player_count = 0;
    int web_desc_count = 0;

    if (!room)
        return;

    sprintf(log_buf, "WebGate: webgate_notify_room_update called for room vnum %d", room->vnum);
    log_string(log_buf);

    /* Iterate through all characters in the room */
    for (rch = room->people; rch; rch = rch->next_in_room)
    {
        /* Skip NPCs - they don't have web descriptors */
        if (IS_NPC(rch))
            continue;

        player_count++;
        sprintf(log_buf, "WebGate: Found player in room: %s", rch->name);
        log_string(log_buf);

        /* Find the web descriptor for this player */
        for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
        {
            web_desc_count++;

            /* Check if this web descriptor matches the player character */
            if (web_desc->mud_desc && web_desc->mud_desc->character == rch)
            {
                sprintf(log_buf, "WebGate: Sending room update to player %s", rch->name);
                log_string(log_buf);
                webgate_send_room_info(web_desc, room);
                break;
            }
        }
    }

    if (player_count > 0)
    {
        sprintf(log_buf, "WebGate: Checked %d players, %d web descriptors in room vnum %d",
                player_count, web_desc_count, room->vnum);
        log_string(log_buf);
    }
}

/*
 * Intent: Send shop inventory to web client via GMCP Shop.Inventory message.
 *
 * When a player uses the "list" command at a shopkeeper, this function
 * sends the shop's inventory to the web client so items can be displayed
 * in the GUI for purchase.
 *
 * Inputs:
 *   - ch: Player character viewing the shop
 *   - keeper: Shopkeeper NPC
 *
 * Outputs: None (Shop.Inventory GMCP message sent to client)
 *
 * Preconditions: ch and keeper must be valid; keeper must have inventory
 * Postconditions: Shop.Inventory sent to web client if authenticated
 *
 * Notes: Only sends items that are for sale (not worn, visible, and have valid cost)
 */
void webgate_send_shop_inventory(CHAR_DATA *ch, CHAR_DATA *keeper)
{
    char json[WEBGATE_MAX_MESSAGE];
    char items_json[MAX_STRING_LENGTH];
    char temp_name[256];
    WEB_DESCRIPTOR_DATA *web_desc;
    OBJ_DATA *obj;
    int cost, i, j;
    bool first = true;
    int item_count = 0;
    extern int get_cost(CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy);

    if (!ch || !keeper)
        return;

    /* Find web descriptor for this character */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc && web_desc->mud_desc->character == ch)
            break;
    }

    if (!web_desc)
        return;

    /* Build items array JSON */
    sprintf(items_json, "[");

    for (obj = keeper->carrying; obj && item_count < 100; obj = obj->next_content)
    {
        char keywords_escaped[256];

        if (obj->wear_loc != WEAR_NONE)
            continue;

        cost = get_cost(keeper, obj, TRUE);
        if (cost < 0)
            continue;

        if (!can_see_obj(ch, obj))
            continue;

        /* Escape item name for JSON */
        for (i = 0, j = 0; obj->short_descr[i] != '\0' && j < sizeof(temp_name) - 2; i++)
        {
            if (obj->short_descr[i] == '"' || obj->short_descr[i] == '\\')
                temp_name[j++] = '\\';
            if (obj->short_descr[i] >= 32)
                temp_name[j++] = obj->short_descr[i];
        }
        temp_name[j] = '\0';

        /* Escape item keywords (for commands) - use first keyword only */
        for (i = 0, j = 0; obj->name[i] != '\0' && obj->name[i] != ' ' && j < sizeof(keywords_escaped) - 2; i++)
        {
            if (obj->name[i] == '"' || obj->name[i] == '\\')
                keywords_escaped[j++] = '\\';
            if (obj->name[i] >= 32)
                keywords_escaped[j++] = obj->name[i];
        }
        keywords_escaped[j] = '\0';

        if (!first)
            strcat(items_json, ",");

        sprintf(items_json + strlen(items_json),
                "{\"name\":\"%s\",\"keywords\":\"%s\",\"level\":%d,\"cost\":%d,\"vnum\":%d}",
                temp_name,
                keywords_escaped,
                obj->level,
                cost,
                obj->pIndexData ? obj->pIndexData->vnum : 0);

        first = false;
        item_count++;
    }

    strcat(items_json, "]");

    /* Build complete Shop.Inventory JSON */
    snprintf(json, sizeof(json),
             "{\"shopkeeper\":\"%s\",\"items\":%s}",
             keeper->short_descr,
             items_json);

    webgate_send_gmcp(web_desc, "Shop.Inventory", json);
}

/*
 * Intent: Send healer services list to web client via GMCP for healer modal display.
 *
 * Inputs: ch - Player requesting healer services; healer - NPC mob with ACT_IS_HEALER flag.
 * Outputs: Sends "Healer.Services" GMCP message with JSON structure:
 *          {healer: name, services: [{keyword, name, description, cost, category}]}
 * Preconditions: ch must have web descriptor; healer must be valid NPC.
 * Postconditions: Client receives healer service list and opens healer modal UI.
 * Failure Behavior: Returns silently if no web descriptor.
 * Notes: Service descriptions pulled from skill_table with generic fallbacks.
 *        Categories: healing, curing, utility, buff. Based on spell_list from healer.c.
 */
void webgate_send_healer_services(CHAR_DATA *ch, CHAR_DATA *healer)
{
    WEB_DESCRIPTOR_DATA *web_desc;
    char json[MAX_STRING_LENGTH * 2];
    char services_json[MAX_STRING_LENGTH * 2];
    char healer_name_escaped[256];
    char temp_name[256];
    char temp_keyword[256];
    char temp_description[512];
    const int NUMBER_SPELLS = 14;
    int i, j, sn;
    bool first = true;

    /* Healer spell list from healer.c */
    const struct healer_spell
    {
        char *keyword;
        char *spell_name;
        int price;
        char *category; /* healing, curing, utility, buff */
    } spell_list[14] = {
        {"light", "cure light wounds", 2, "healing"},
        {"serious", "cure serious wounds", 5, "healing"},
        {"critical", "cure critical wounds", 10, "healing"},
        {"heal", "heal", 15, "healing"},
        {"power", "power heal", 35, "healing"},
        {"blindness", "cure blindness", 5, "curing"},
        {"poison", "cure poison", 5, "curing"},
        {"stabilise", "stabilise", 5, "healing"},
        {"regenerate", "regenerate", 35, "buff"},
        {"curse", "remove curse", 5, "curing"},
        {"refresh", "refresh", 1, "utility"},
        {"bless", "bless", 10, "buff"},
        {"mana", "restore mana", 40, "utility"},
        {"vitalize", "vitalize", 120, "buff"}};

    if (!ch || !healer)
        return;

    /* Find web descriptor for this character */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc && web_desc->mud_desc->character == ch)
            break;
    }

    if (!web_desc)
        return;

    /* Escape healer name for JSON */
    for (i = 0, j = 0; healer->short_descr[i] != '\0' && j < sizeof(healer_name_escaped) - 2; i++)
    {
        if (healer->short_descr[i] == '"' || healer->short_descr[i] == '\\')
            healer_name_escaped[j++] = '\\';
        if (healer->short_descr[i] >= 32)
            healer_name_escaped[j++] = healer->short_descr[i];
    }
    healer_name_escaped[j] = '\0';

    /* Build services JSON array */
    sprintf(services_json, "[");

    for (i = 0; i < NUMBER_SPELLS; i++)
    {
        char description[512];
        int k;

        /* Escape keyword */
        for (j = 0, k = 0; spell_list[i].keyword[j] != '\0' && k < sizeof(temp_keyword) - 2; j++)
        {
            if (spell_list[i].keyword[j] == '"' || spell_list[i].keyword[j] == '\\')
                temp_keyword[k++] = '\\';
            if (spell_list[i].keyword[j] >= 32)
                temp_keyword[k++] = spell_list[i].keyword[j];
        }
        temp_keyword[k] = '\0';

        /* Escape spell name */
        for (j = 0, k = 0; spell_list[i].spell_name[j] != '\0' && k < sizeof(temp_name) - 2; j++)
        {
            if (spell_list[i].spell_name[j] == '"' || spell_list[i].spell_name[j] == '\\')
                temp_name[k++] = '\\';
            if (spell_list[i].spell_name[j] >= 32)
                temp_name[k++] = spell_list[i].spell_name[j];
        }
        temp_name[k] = '\0';

        /* Get spell description from skill_table or use fallback */
        sn = skill_lookup(spell_list[i].spell_name);
        if (sn >= 0 && sn < MAX_SKILL && skill_table[sn].noun_damage && skill_table[sn].noun_damage[0] != '\0')
        {
            /* Use skill table description */
            sprintf(description, "%s", skill_table[sn].noun_damage);
        }
        else
        {
            /* Generic fallback descriptions based on spell name */
            if (!str_cmp(spell_list[i].keyword, "light"))
                sprintf(description, "Heals minor wounds");
            else if (!str_cmp(spell_list[i].keyword, "serious"))
                sprintf(description, "Heals serious wounds");
            else if (!str_cmp(spell_list[i].keyword, "critical"))
                sprintf(description, "Heals critical wounds");
            else if (!str_cmp(spell_list[i].keyword, "heal"))
                sprintf(description, "Restores a large amount of health");
            else if (!str_cmp(spell_list[i].keyword, "power"))
                sprintf(description, "Restores a massive amount of health");
            else if (!str_cmp(spell_list[i].keyword, "blindness"))
                sprintf(description, "Removes blindness");
            else if (!str_cmp(spell_list[i].keyword, "poison"))
                sprintf(description, "Cures poison");
            else if (!str_cmp(spell_list[i].keyword, "stabilise"))
                sprintf(description, "Stabilizes dying character");
            else if (!str_cmp(spell_list[i].keyword, "regenerate"))
                sprintf(description, "Grants regeneration over time");
            else if (!str_cmp(spell_list[i].keyword, "curse"))
                sprintf(description, "Removes curses");
            else if (!str_cmp(spell_list[i].keyword, "refresh"))
                sprintf(description, "Restores movement energy");
            else if (!str_cmp(spell_list[i].keyword, "bless"))
                sprintf(description, "Grants divine blessing");
            else if (!str_cmp(spell_list[i].keyword, "mana"))
                sprintf(description, "Restores 100 mana");
            else if (!str_cmp(spell_list[i].keyword, "vitalize"))
                sprintf(description, "Greatly enhances vitality");
            else
                sprintf(description, "Provides healing service");
        }

        /* Escape description for JSON */
        for (j = 0, k = 0; description[j] != '\0' && k < sizeof(temp_description) - 2; j++)
        {
            if (description[j] == '"' || description[j] == '\\')
                temp_description[k++] = '\\';
            if (description[j] >= 32)
                temp_description[k++] = description[j];
        }
        temp_description[k] = '\0';

        /* Add service to array */
        if (!first)
            strcat(services_json, ",");

        sprintf(services_json + strlen(services_json),
                "{\"keyword\":\"%s\",\"name\":\"%s\",\"description\":\"%s\",\"cost\":%d,\"category\":\"%s\"}",
                temp_keyword,
                temp_name,
                temp_description,
                spell_list[i].price,
                spell_list[i].category);

        first = false;
    }

    strcat(services_json, "]");

    /* Build final GMCP message */
    sprintf(json, "{\"healer\":\"%s\",\"services\":%s}",
            healer_name_escaped,
            services_json);

    webgate_send_gmcp(web_desc, "Healer.Services", json);
}

/*
 * Intent: Check if character is using web client (has active web descriptor).
 *
 * Inputs: ch - Character to check
 * Outputs: true if character has web descriptor, false otherwise
 * Preconditions: ch must be valid
 * Notes: Used to conditionally skip text output when web client handles via GMCP.
 */
bool webgate_is_web_client(CHAR_DATA *ch)
{
    WEB_DESCRIPTOR_DATA *web_desc;

    if (!ch)
        return false;

    /* Find web descriptor for this character */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc && web_desc->mud_desc->character == ch)
            return true;
    }

    return false;
}

/*
 * Intent: Broadcast a GMCP message to all authenticated web clients.
 *
 * Iterates through all active web connections and sends the message
 * to those that are authenticated and in the OPEN state. Use for
 * server-wide events or room-wide broadcasts.
 *
 * Inputs:
 *   - module: GMCP module name
 *   - json_body: JSON data as string
 *
 * Outputs: None (messages queued for all eligible clients)
 *
 * Preconditions: None (handles empty list gracefully)
 * Postconditions: Message queued for all authenticated clients
 *
 * Performance: O(n) where n is number of connections; avoid in hot paths
 */
void webgate_broadcast(const char *module, const char *json_body)
{
    WEB_DESCRIPTOR_DATA *web_desc;

    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->authenticated)
        {
            webgate_send_gmcp(web_desc, module, json_body);
        }
    }
}

/*
 * Intent: Send character skills/spells to web client via GMCP.
 *
 * Provides list of learned active skills and spells for skill bar assignment
 * and combat opener selection. Excludes passive abilities that don't require
 * manual activation. Includes skill proficiency, mana cost, and metadata for
 * client-side cooldown tracking and UI display.
 *
 * Inputs:
 *   - web_desc: Web client descriptor
 *   - ch: Character whose skills to send
 *
 * Outputs: Char.Skills GMCP message with JSON array of learned skills
 *
 * Preconditions: ch must be valid player character with pcdata, web_desc must be open
 * Postconditions: Client receives JSON array of active skills with metadata
 *
 * Failure Behavior: Returns silently if web_desc or ch is NULL or NPC
 *
 * Performance: O(MAX_SKILL) linear scan of skill table
 *
 * Notes:
 *   - Only sends skills with learned > 0
 *   - Filters out passive skills (spell_null with no combat application)
 *   - Marks "opener" skills that can initiate combat from POS_STANDING
 *   - Client tracks cooldowns locally using beats value
 */
void webgate_send_char_skills(WEB_DESCRIPTOR_DATA *web_desc, CHAR_DATA *ch)
{
    char json[MAX_STRING_LENGTH * 4];
    char skill_entry[MAX_STRING_LENGTH];
    char skill_name_escaped[MAX_INPUT_LENGTH];
    const char *src;
    char *dst;
    int sn;
    int count = 0;
    bool is_opener;
    const char *skill_type;

    if (!web_desc || !ch || IS_NPC(ch) || web_desc->state != WS_STATE_OPEN)
        return;

    strcpy(json, "{\"skills\":[");

    /* Iterate through all skills in skill_table */
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        /* Skip if no skill name or if it's a wizard/null skill */
        if (!skill_table[sn].name ||
            skill_table[sn].prac_type == TYPE_WIZ ||
            skill_table[sn].prac_type == TYPE_NULL)
            continue;

        /* Include skills the character has learned (CAN_DO checks learned > 0) */
        /* OR skills they can learn (learned == 0, has pre-req, and sn < gsn_mage_base) */
        if (!CAN_DO(ch, sn))
        {
            /* For unlearned skills, check if they can be learned */
            if (ch->pcdata->learned[sn] != 0 || sn >= gsn_mage_base || !has_pre_req(ch, sn))
                continue;
        }

        /* Determine if this skill can initiate combat (opener) */
        /* Openers can be used from POS_STANDING and target enemies */
        is_opener = (skill_table[sn].minimum_position <= POS_STANDING &&
                     (skill_table[sn].target == TAR_CHAR_OFFENSIVE ||
                      skill_table[sn].target == TAR_CHAR_OFFENSIVE_SINGLE));

        /* Determine skill type: spell (uses mana) or skill (physical/special ability) */
        skill_type = (skill_table[sn].min_mana > 0) ? "spell" : "skill";

        /* Simple JSON escape for skill name (most skill names don't have special chars) */
        src = skill_table[sn].name;
        dst = skill_name_escaped;
        while (*src && (dst - skill_name_escaped) < (MAX_INPUT_LENGTH - 2))
        {
            if (*src == '"' || *src == '\\')
            {
                *dst++ = '\\';
            }
            *dst++ = *src++;
        }
        *dst = '\0';

        /* Build JSON entry for this skill */
        sprintf(skill_entry, "%s{\"id\":%d,\"name\":\"%s\",\"learned\":%d,\"mana\":%d,\"beats\":%d,\"type\":\"%s\",\"opener\":%s,\"pracType\":%d}",
                (count > 0 ? "," : ""),
                sn,
                skill_name_escaped,
                ch->pcdata->learned[sn],
                skill_table[sn].min_mana,
                skill_table[sn].beats,
                skill_type,
                is_opener ? "true" : "false",
                skill_table[sn].prac_type);

        /* Check if adding this entry would overflow the buffer */
        if (strlen(json) + strlen(skill_entry) + 10 >= sizeof(json))
        {
            sprintf(log_buf, "WebGate: Skill list too large for %s, truncating", ch->name);
            log_string(log_buf);
            break;
        }

        strcat(json, skill_entry);
        count++;
    }

    strcat(json, "]}");

    sprintf(log_buf, "WebGate: Sending %d skills to %s", count, ch->name);
    log_string(log_buf);

    webgate_send_gmcp(web_desc, "Char.Skills", json);
}

/*
 * Intent: Serialize item extra flags to JSON array format.
 *
 * Only includes flags that are visible to player. Unidentified items only
 * show cosmetic flags (glow, hum), while identified items show all flags.
 *
 * Inputs:
 *   - obj: Item to serialize
 *   - identified: Whether item stats are visible
 *   - buf: Output buffer for JSON array
 *   - buf_size: Size of output buffer
 *
 * Outputs: buf filled with JSON array string like ["glow","hum","magic"]
 *
 * Preconditions: obj is valid, buf has sufficient space
 * Postconditions: buf contains properly formatted JSON array
 */
static void webgate_serialize_extra_flags(OBJ_DATA *obj, bool identified, char *buf, int buf_size)
{
    unsigned long int flags = obj->extra_flags;
    bool first = true;
    int len = 0;

    buf[0] = '\0';
    strcat(buf, "[");
    len++;

    /* Always show cosmetic flags */
    if (flags & ITEM_GLOW)
    {
        strcat(buf, first ? "\"glow\"" : ",\"glow\"");
        first = false;
    }
    if (flags & ITEM_HUM)
    {
        strcat(buf, first ? "\"hum\"" : ",\"hum\"");
        first = false;
    }

    /* Show all other flags only if identified */
    if (identified)
    {
        if (flags & ITEM_MAGIC)
        {
            strcat(buf, first ? "\"magic\"" : ",\"magic\"");
            first = false;
        }
        if (flags & ITEM_BLESS)
        {
            strcat(buf, first ? "\"blessed\"" : ",\"blessed\"");
            first = false;
        }
        if (flags & ITEM_CURSED)
        {
            strcat(buf, first ? "\"cursed\"" : ",\"cursed\"");
            first = false;
        }
        if (flags & ITEM_INVIS)
        {
            strcat(buf, first ? "\"invis\"" : ",\"invis\"");
            first = false;
        }
        if (flags & ITEM_EVIL)
        {
            strcat(buf, first ? "\"evil\"" : ",\"evil\"");
            first = false;
        }
        if (flags & ITEM_NODROP)
        {
            strcat(buf, first ? "\"no_drop\"" : ",\"no_drop\"");
            first = false;
        }
        if (flags & ITEM_NOREMOVE)
        {
            strcat(buf, first ? "\"no_remove\"" : ",\"no_remove\"");
            first = false;
        }
        if (flags & ITEM_VORPAL)
        {
            strcat(buf, first ? "\"vorpal\"" : ",\"vorpal\"");
            first = false;
        }
        if (flags & ITEM_TRAP)
        {
            strcat(buf, first ? "\"trapped\"" : ",\"trapped\"");
            first = false;
        }
        if (flags & ITEM_POISONED)
        {
            strcat(buf, first ? "\"poisoned\"" : ",\"poisoned\"");
            first = false;
        }
        if (flags & ITEM_SHARP)
        {
            strcat(buf, first ? "\"sharp\"" : ",\"sharp\"");
            first = false;
        }
        if (flags & ITEM_FORGED)
        {
            strcat(buf, first ? "\"forged\"" : ",\"forged\"");
            first = false;
        }
    }

    strcat(buf, "]");
}

/*
 * Intent: Serialize item ego flags to JSON array format.
 *
 * Only included if item is identified and has ITEM_EGO flag set.
 *
 * Inputs:
 *   - obj: Item to serialize
 *   - identified: Whether item stats are visible
 *   - buf: Output buffer
 *   - buf_size: Size of output buffer
 *
 * Outputs: buf filled with JSON array like ["firebrand","balanced"]
 */
static void webgate_serialize_ego_flags(OBJ_DATA *obj, bool identified, char *buf, int buf_size)
{
    int flags;
    bool first = true;

    buf[0] = '\0';
    strcat(buf, "[");

    if (!identified || !IS_OBJ_STAT(obj, ITEM_EGO))
    {
        strcat(buf, "]");
        return;
    }

    flags = obj->ego_flags;

    if (flags & EGO_ITEM_BLOODLUST)
    {
        strcat(buf, first ? "\"bloodlust\"" : ",\"bloodlust\"");
        first = false;
    }
    if (flags & EGO_ITEM_SOUL_STEALER)
    {
        strcat(buf, first ? "\"soul_stealer\"" : ",\"soul_stealer\"");
        first = false;
    }
    if (flags & EGO_ITEM_FIREBRAND)
    {
        strcat(buf, first ? "\"firebrand\"" : ",\"firebrand\"");
        first = false;
    }
    if (flags & EGO_ITEM_IMBUED)
    {
        strcat(buf, first ? "\"imbued\"" : ",\"imbued\"");
        first = false;
    }
    if (flags & EGO_ITEM_BALANCED)
    {
        strcat(buf, first ? "\"balanced\"" : ",\"balanced\"");
        first = false;
    }
    if (flags & EGO_ITEM_BATTLE_TERROR)
    {
        strcat(buf, first ? "\"battle_terror\"" : ",\"battle_terror\"");
        first = false;
    }
    if (flags & EGO_ITEM_STRENGTHEN)
    {
        strcat(buf, first ? "\"strengthened\"" : ",\"strengthened\"");
        first = false;
    }
    if (flags & EGO_ITEM_EMPOWERED)
    {
        strcat(buf, first ? "\"empowered\"" : ",\"empowered\"");
        first = false;
    }
    if (flags & EGO_ITEM_SERRATED)
    {
        strcat(buf, first ? "\"serrated\"" : ",\"serrated\"");
        first = false;
    }
    if (flags & EGO_ITEM_ENGRAVED)
    {
        strcat(buf, first ? "\"engraved\"" : ",\"engraved\"");
        first = false;
    }
    if (flags & EGO_ITEM_INSCRIBED)
    {
        strcat(buf, first ? "\"inscribed\"" : ",\"inscribed\"");
        first = false;
    }

    strcat(buf, "]");
}

/*
 * Intent: Serialize item affect modifiers to JSON array format.
 *
 * Converts item's AFFECT_DATA linked list to JSON array of stat modifiers.
 * Only included if item is identified.
 *
 * Inputs:
 *   - obj: Item to serialize
 *   - identified: Whether item stats are visible
 *   - buf: Output buffer
 *   - buf_size: Size of output buffer
 *
 * Outputs: buf filled with JSON array like [{"type":"hitroll","modifier":5}]
 */
static void webgate_serialize_affects(OBJ_DATA *obj, bool identified, char *buf, int buf_size)
{
    AFFECT_DATA *paf;
    bool first = true;
    char temp[256];

    buf[0] = '\0';
    strcat(buf, "[");

    if (!identified)
    {
        strcat(buf, "]");
        return;
    }

    /* Iterate through both index and object-specific affects */
    if (obj->pIndexData && (!obj->how_created || obj->how_created == CREATED_PRE_DD5))
    {
        for (paf = obj->pIndexData->affected; paf; paf = paf->next)
        {
            if (paf->location != APPLY_NONE && paf->modifier != 0)
            {
                sprintf(temp, "%s{\"type\":\"%s\",\"modifier\":%d}",
                        first ? "" : ",",
                        affect_loc_name(paf->location),
                        paf->modifier);
                if (strlen(buf) + strlen(temp) < buf_size - 10)
                {
                    strcat(buf, temp);
                    first = false;
                }
            }
        }
    }

    for (paf = obj->affected; paf; paf = paf->next)
    {
        if (paf->location != APPLY_NONE && paf->modifier != 0)
        {
            sprintf(temp, "%s{\"type\":\"%s\",\"modifier\":%d}",
                    first ? "" : ",",
                    affect_loc_name(paf->location),
                    paf->modifier);
            if (strlen(buf) + strlen(temp) < buf_size - 10)
            {
                strcat(buf, temp);
                first = false;
            }
        }
    }

    strcat(buf, "]");
}

/*
 * Intent: Serialize item class restrictions to JSON array format.
 *
 * Only included if item is identified and has class restriction flags.
 *
 * Inputs:
 *   - obj: Item to serialize
 *   - identified: Whether item stats are visible
 *   - buf: Output buffer
 *   - buf_size: Size of output buffer
 *
 * Outputs: buf filled with JSON array like ["mage","cleric"]
 */
static void webgate_serialize_class_restrictions(OBJ_DATA *obj, bool identified, char *buf, int buf_size)
{
    unsigned long int flags = obj->extra_flags;
    bool first = true;

    buf[0] = '\0';
    strcat(buf, "[");

    if (!identified)
    {
        strcat(buf, "]");
        return;
    }

    if (flags & ITEM_ANTI_MAGE)
    {
        strcat(buf, first ? "\"mage\"" : ",\"mage\"");
        first = false;
    }
    if (flags & ITEM_ANTI_CLERIC)
    {
        strcat(buf, first ? "\"cleric\"" : ",\"cleric\"");
        first = false;
    }
    if (flags & ITEM_ANTI_THIEF)
    {
        strcat(buf, first ? "\"thief\"" : ",\"thief\"");
        first = false;
    }
    if (flags & ITEM_ANTI_WARRIOR)
    {
        strcat(buf, first ? "\"warrior\"" : ",\"warrior\"");
        first = false;
    }
    if (flags & ITEM_ANTI_PSIONIC)
    {
        strcat(buf, first ? "\"psionic\"" : ",\"psionic\"");
        first = false;
    }
    if (flags & ITEM_ANTI_BRAWLER)
    {
        strcat(buf, first ? "\"brawler\"" : ",\"brawler\"");
        first = false;
    }
    if (flags & ITEM_ANTI_SHAPE_SHIFTER)
    {
        strcat(buf, first ? "\"shape_shifter\"" : ",\"shape_shifter\"");
        first = false;
    }
    if (flags & ITEM_ANTI_RANGER)
    {
        strcat(buf, first ? "\"ranger\"" : ",\"ranger\"");
        first = false;
    }
    if (flags & ITEM_ANTI_SMITHY)
    {
        strcat(buf, first ? "\"smithy\"" : ",\"smithy\"");
        first = false;
    }

    strcat(buf, "]");
}

/*
 * Intent: Serialize item alignment restrictions to JSON array format.
 *
 * Only included if item is identified and has alignment restriction flags.
 *
 * Inputs:
 *   - obj: Item to serialize
 *   - identified: Whether item stats are visible
 *   - buf: Output buffer
 *   - buf_size: Size of output buffer
 *
 * Outputs: buf filled with JSON array like ["evil","good"]
 */
static void webgate_serialize_alignment_restrictions(OBJ_DATA *obj, bool identified, char *buf, int buf_size)
{
    unsigned long int flags = obj->extra_flags;
    bool first = true;

    buf[0] = '\0';
    strcat(buf, "[");

    if (!identified)
    {
        strcat(buf, "]");
        return;
    }

    if (flags & ITEM_ANTI_GOOD)
    {
        strcat(buf, first ? "\"good\"" : ",\"good\"");
        first = false;
    }
    if (flags & ITEM_ANTI_NEUTRAL)
    {
        strcat(buf, first ? "\"neutral\"" : ",\"neutral\"");
        first = false;
    }
    if (flags & ITEM_ANTI_EVIL)
    {
        strcat(buf, first ? "\"evil\"" : ",\"evil\"");
        first = false;
    }

    strcat(buf, "]");
}

/*
 * Intent: Determine item rarity based on item score.
 *
 * Returns rarity string for color coding item names in GUI.
 *
 * Inputs:
 *   - obj: Item to check
 *
 * Outputs: Returns rarity string: "common", "uncommon", "rare", "epic", "legendary"
 */
static const char *webgate_get_item_rarity(OBJ_DATA *obj)
{
    int score;

    if (!obj || !obj->pIndexData)
        return "common";

    score = calc_item_score(obj);

    if (score >= ITEM_SCORE_LEGENDARY)
        return "legendary";
    if (score >= ITEM_SCORE_EPIC)
        return "epic";
    if (score >= ITEM_SCORE_RARE)
        return "rare";
    if (score >= ITEM_SCORE_UNCOMMON)
        return "uncommon";

    return "common";
}

/*
 * Intent: Check if item is part of an object set.
 *
 * Returns set name if item is part of a set, otherwise returns empty string.
 *
 * Inputs:
 *   - obj: Item to check
 *
 * Outputs: Returns set type name or empty string
 */
static const char *webgate_get_item_set(OBJ_DATA *obj)
{
    OBJSET_INDEX_DATA *pObjSetIndex;

    if (!obj || !obj->pIndexData)
        return "";

    pObjSetIndex = objects_objset(obj->pIndexData->vnum);
    if (pObjSetIndex)
        return objset_type(pObjSetIndex->vnum);

    return "";
}

/*
 * Intent: Send character inventory to web client via GMCP.
 *
 * Provides list of items in character's inventory for UI display and
 * interaction. Includes item metadata for building context menus with
 * actions like drop, wear, use, etc.
 *
 * Inputs:
 *   - web_desc: Web client descriptor
 *   - ch: Character whose inventory to send
 *
 * Outputs: Char.Inventory GMCP message with JSON array of inventory items
 *
 * Preconditions: ch must be valid player character, web_desc must be open
 * Postconditions: Client receives JSON array of carried items
 *
 * Failure Behavior: Returns silently if web_desc or ch is NULL
 *
 * Performance: O(n) where n is number of items carried
 *
 * Notes: Limits to 100 items to prevent buffer overflow
 */
void webgate_send_char_inventory(WEB_DESCRIPTOR_DATA *web_desc, CHAR_DATA *ch)
{
    char json[MAX_STRING_LENGTH * 4];
    char item_entry[MAX_STRING_LENGTH * 2];
    char item_name_escaped[MAX_INPUT_LENGTH];
    char keywords_escaped[MAX_INPUT_LENGTH];
    char material_escaped[MAX_INPUT_LENGTH];
    char extra_flags_json[512];
    char ego_flags_json[512];
    char affects_json[1024];
    char class_restrictions_json[256];
    char alignment_restrictions_json[128];
    const char *src;
    char *dst;
    OBJ_DATA *obj;
    int count = 0;
    const char *item_type_name;

    if (!web_desc || !ch || web_desc->state != WS_STATE_OPEN)
        return;

    strcpy(json, "{\"items\":[");

    /* Iterate through carried items */
    for (obj = ch->carrying; obj && count < 100; obj = obj->next_content)
    {
        /* Skip worn items - they're in equipment */
        if (obj->wear_loc != WEAR_NONE)
            continue;

        /* Escape item short description */
        src = obj->short_descr;
        dst = item_name_escaped;
        while (*src && (dst - item_name_escaped) < (MAX_INPUT_LENGTH - 2))
        {
            if (*src == '"' || *src == '\\')
                *dst++ = '\\';
            *dst++ = *src++;
        }
        *dst = '\0';

        /* Escape item keywords (first keyword only) */
        src = obj->name;
        dst = keywords_escaped;
        while (*src && *src != ' ' && (dst - keywords_escaped) < (MAX_INPUT_LENGTH - 2))
        {
            if (*src == '"' || *src == '\\')
                *dst++ = '\\';
            *dst++ = *src++;
        }
        *dst = '\0';

        /* Escape material name */
        if (obj->material && obj->material[0] != '\0')
        {
            src = obj->material;
            dst = material_escaped;
            while (*src && (dst - material_escaped) < (MAX_INPUT_LENGTH - 2))
            {
                if (*src == '"' || *src == '\\')
                    *dst++ = '\\';
                *dst++ = *src++;
            }
            *dst = '\0';
        }
        else
        {
            strcpy(material_escaped, "unknown");
        }

        /* Determine item type name for client */
        switch (obj->item_type)
        {
        case ITEM_WEAPON:
            item_type_name = "weapon";
            break;
        case ITEM_ARMOR:
            item_type_name = "armor";
            break;
        case ITEM_POTION:
            item_type_name = "potion";
            break;
        case ITEM_SCROLL:
            item_type_name = "scroll";
            break;
        case ITEM_FOOD:
            item_type_name = "food";
            break;
        case ITEM_DRINK_CON:
            item_type_name = "drink";
            break;
        case ITEM_CONTAINER:
            item_type_name = "container";
            break;
        case ITEM_LIGHT:
            item_type_name = "light";
            break;
        case ITEM_KEY:
            item_type_name = "key";
            break;
        case ITEM_TREASURE:
            item_type_name = "treasure";
            break;
        default:
            item_type_name = "item";
            break;
        }

        /* Serialize identification details */
        webgate_serialize_extra_flags(obj, obj->identified, extra_flags_json, sizeof(extra_flags_json));
        webgate_serialize_ego_flags(obj, obj->identified, ego_flags_json, sizeof(ego_flags_json));
        webgate_serialize_affects(obj, obj->identified, affects_json, sizeof(affects_json));
        webgate_serialize_class_restrictions(obj, obj->identified, class_restrictions_json, sizeof(class_restrictions_json));
        webgate_serialize_alignment_restrictions(obj, obj->identified, alignment_restrictions_json, sizeof(alignment_restrictions_json));

        /* Get rarity and set information */
        {
            const char *rarity = webgate_get_item_rarity(obj);
            const char *set_name = webgate_get_item_set(obj);
            char set_escaped[256];

            /* Escape set name */
            src = set_name;
            dst = set_escaped;
            while (*src && (dst - set_escaped) < sizeof(set_escaped) - 2)
            {
                if (*src == '"' || *src == '\\')
                    *dst++ = '\\';
                *dst++ = *src++;
            }
            *dst = '\0';

            /* Build JSON entry for this item with identification details */
            sprintf(item_entry, "%s{\"id\":%d,\"name\":\"%s\",\"keywords\":\"%s\",\"type\":\"%s\",\"level\":%d,\"weight\":%d,\"canWear\":%s,\"identified\":%s,\"material\":\"%s\",\"rarity\":\"%s\",\"setName\":\"%s\",\"extraFlags\":%s,\"egoFlags\":%s,\"affects\":%s,\"classRestrictions\":%s,\"alignmentRestrictions\":%s}",
                    (count > 0 ? "," : ""),
                    obj->pIndexData ? obj->pIndexData->vnum : 0,
                    item_name_escaped,
                    keywords_escaped,
                    item_type_name,
                    obj->level,
                    obj->weight,
                    (obj->wear_flags > 0 && obj->wear_flags != ITEM_TAKE) ? "true" : "false",
                    obj->identified ? "true" : "false",
                    material_escaped,
                    rarity,
                    set_escaped,
                    extra_flags_json,
                    ego_flags_json,
                    affects_json,
                    class_restrictions_json,
                    alignment_restrictions_json);
        }

        /* Check buffer overflow */
        if (strlen(json) + strlen(item_entry) + 10 >= sizeof(json))
        {
            sprintf(log_buf, "WebGate: Inventory list too large for %s, truncating", ch->name);
            log_string(log_buf);
            break;
        }

        strcat(json, item_entry);
        count++;
    }

    strcat(json, "]}");

    sprintf(log_buf, "WebGate: Sending %d inventory items to %s", count, ch->name);
    log_string(log_buf);

    webgate_send_gmcp(web_desc, "Char.Inventory", json);
}

/*
 * Intent: Send character equipment to web client via GMCP.
 *
 * Provides list of equipped items organized by wear location for paperdoll-style
 * UI display. Includes all 22 equipment slots even if empty so client can show
 * equipment slots consistently.
 *
 * Inputs:
 *   - web_desc: Web client descriptor
 *   - ch: Character whose equipment to send
 *
 * Outputs: Char.Equipment GMCP message with JSON object of equipment slots
 *
 * Preconditions: ch must be valid character, web_desc must be open
 * Postconditions: Client receives JSON with all equipment slots
 *
 * Failure Behavior: Returns silently if web_desc or ch is NULL
 *
 * Performance: O(n) where n is MAX_WEAR (22 slots)
 *
 * Notes: Sends null for empty slots so client knows all possible slots
 */
void webgate_send_char_equipment(WEB_DESCRIPTOR_DATA *web_desc, CHAR_DATA *ch)
{
    char json[MAX_STRING_LENGTH * 4];
    char item_entry[MAX_STRING_LENGTH * 2];
    char item_name_escaped[MAX_INPUT_LENGTH];
    char keywords_escaped[MAX_INPUT_LENGTH];
    char material_escaped[MAX_INPUT_LENGTH];
    char extra_flags_buf[MAX_STRING_LENGTH];
    char ego_flags_buf[MAX_STRING_LENGTH];
    char affects_buf[MAX_STRING_LENGTH];
    char class_restrict_buf[MAX_STRING_LENGTH];
    char align_restrict_buf[MAX_STRING_LENGTH];
    const char *src;
    char *dst;
    OBJ_DATA *obj;
    int wear_loc;
    bool first = true;
    const char *slot_names[] = {
        "light", "finger_l", "finger_r", "neck_1", "neck_2",
        "body", "head", "legs", "feet", "hands",
        "arms", "shield", "about", "waist", "wrist_l",
        "wrist_r", "wield", "hold", "dual", "float",
        "pouch", "ranged_weapon"};

    if (!web_desc || !ch || web_desc->state != WS_STATE_OPEN)
        return;

    strcpy(json, "{\"equipment\":{");

    /* Iterate through all equipment slots */
    for (wear_loc = WEAR_LIGHT; wear_loc < MAX_WEAR; wear_loc++)
    {
        if (!first)
            strcat(json, ",");

        obj = get_eq_char(ch, wear_loc);

        if (obj)
        {
            /* Escape item short description */
            src = obj->short_descr;
            dst = item_name_escaped;
            while (*src && (dst - item_name_escaped) < (MAX_INPUT_LENGTH - 2))
            {
                if (*src == '"' || *src == '\\')
                    *dst++ = '\\';
                *dst++ = *src++;
            }
            *dst = '\0';

            /* Escape item keywords (first keyword only) */
            src = obj->name;
            dst = keywords_escaped;
            while (*src && *src != ' ' && (dst - keywords_escaped) < (MAX_INPUT_LENGTH - 2))
            {
                if (*src == '"' || *src == '\\')
                    *dst++ = '\\';
                *dst++ = *src++;
            }
            *dst = '\0';

            /* Escape material name */
            src = obj->material ? obj->material : "unknown";
            dst = material_escaped;
            while (*src && (dst - material_escaped) < (MAX_INPUT_LENGTH - 2))
            {
                if (*src == '"' || *src == '\\')
                    *dst++ = '\\';
                *dst++ = *src++;
            }
            *dst = '\0';

            /* Serialize identification details using helper functions */
            webgate_serialize_extra_flags(obj, obj->identified, extra_flags_buf, sizeof(extra_flags_buf));
            webgate_serialize_ego_flags(obj, obj->identified, ego_flags_buf, sizeof(ego_flags_buf));
            webgate_serialize_affects(obj, obj->identified, affects_buf, sizeof(affects_buf));
            webgate_serialize_class_restrictions(obj, obj->identified, class_restrict_buf, sizeof(class_restrict_buf));
            webgate_serialize_alignment_restrictions(obj, obj->identified, align_restrict_buf, sizeof(align_restrict_buf));

            /* Get rarity and set information */
            {
                const char *rarity = webgate_get_item_rarity(obj);
                const char *set_name = webgate_get_item_set(obj);
                char set_escaped[256];
                const char *src2;
                char *dst2;

                /* Escape set name */
                src2 = set_name;
                dst2 = set_escaped;
                while (*src2 && (dst2 - set_escaped) < sizeof(set_escaped) - 2)
                {
                    if (*src2 == '"' || *src2 == '\\')
                        *dst2++ = '\\';
                    *dst2++ = *src2++;
                }
                *dst2 = '\0';

                /* Build item entry with all identification details */
                sprintf(item_entry,
                        "\"%s\":{\"id\":%d,\"name\":\"%s\",\"keywords\":\"%s\","
                        "\"level\":%d,\"type\":%d,\"weight\":%d,"
                        "\"identified\":%s,\"material\":\"%s\",\"rarity\":\"%s\",\"setName\":\"%s\","
                        "\"extraFlags\":%s,\"egoFlags\":%s,\"affects\":%s,"
                        "\"classRestrictions\":%s,\"alignmentRestrictions\":%s}",
                        slot_names[wear_loc],
                        obj->pIndexData ? obj->pIndexData->vnum : 0,
                        item_name_escaped,
                        keywords_escaped,
                        obj->level,
                        obj->item_type,
                        obj->weight,
                        obj->identified ? "true" : "false",
                        material_escaped,
                        rarity,
                        set_escaped,
                        extra_flags_buf,
                        ego_flags_buf,
                        affects_buf,
                        class_restrict_buf,
                        align_restrict_buf);
            }
        }
        else
        {
            /* Empty slot */
            sprintf(item_entry, "\"%s\":null", slot_names[wear_loc]);
        }

        /* Check buffer overflow */
        if (strlen(json) + strlen(item_entry) + 10 >= sizeof(json))
        {
            sprintf(log_buf, "WebGate: Equipment list too large for %s, truncating", ch->name);
            log_string(log_buf);
            break;
        }

        strcat(json, item_entry);
        first = false;
    }

    strcat(json, "}}");

    sprintf(log_buf, "WebGate: Sending equipment to %s", ch->name);
    log_string(log_buf);

    webgate_send_gmcp(web_desc, "Char.Equipment", json);
}

/*
 * Intent: Send character vitals to web client via GMCP.
 *
 * Provides real-time updates of character health, mana, movement, experience,
 * and level information. This enables web clients to display progress bars
 * and numeric indicators without parsing game text output.
 *
 * Inputs:
 *   - web_desc: Web client descriptor
 *   - ch: Character whose vitals to send
 *
 * Outputs: Char.Vitals GMCP message sent to client
 *
 * Preconditions: ch must be valid player character, web_desc must be open
 * Postconditions: Client receives JSON with current vitals
 *
 * Failure Behavior: Returns silently if web_desc or ch is NULL or connection not open
 *
 * Notes: Should be called after combat, healing, regen ticks, level gains
 */
void webgate_send_char_vitals(WEB_DESCRIPTOR_DATA *web_desc, CHAR_DATA *ch)
{
    char json[MAX_STRING_LENGTH];
    int exp_tnl;
    int exp_level_start;
    int str_prac = 0;
    int int_prac = 0;

    if (!web_desc || !ch || web_desc->state != WS_STATE_OPEN)
        return;

    /* Calculate experience to next level and exp at start of current level */
    exp_tnl = 0;
    exp_level_start = 0;
    if (ch->level < LEVEL_HERO)
    {
        exp_tnl = level_table[ch->level].exp_total - ch->exp;
        if (exp_tnl < 0)
            exp_tnl = 0;

        /* Get exp total at start of current level (from previous level) */
        if (ch->level > 1)
            exp_level_start = level_table[ch->level - 1].exp_total;
    }

    /* Get practice points for player characters */
    if (ch->pcdata)
    {
        str_prac = ch->pcdata->str_prac;
        int_prac = ch->pcdata->int_prac;
    }

    snprintf(json, sizeof(json),
             "{\"hp\":%d,\"maxhp\":%d,\"mana\":%d,\"maxmana\":%d,"
             "\"move\":%d,\"maxmove\":%d,\"level\":%d,\"align\":%d,"
             "\"exp\":%d,\"tnl\":%d,\"expLevelStart\":%d,\"pracPhysical\":%d,\"pracIntellectual\":%d}",
             ch->hit, ch->max_hit,
             ch->mana, ch->max_mana,
             ch->move, ch->max_move,
             ch->level, ch->alignment,
             ch->exp, exp_tnl, exp_level_start,
             str_prac, int_prac);

    webgate_send_gmcp(web_desc, "Char.Vitals", json);

    sprintf(log_buf, "WebGate: Sent Char.Vitals (hp:%d/%d mana:%d/%d move:%d/%d)",
            ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);
    log_string(log_buf);
}

/*
 * Intent: Send character status information to web client via GMCP.
 *
 * Provides character metadata including name, class, race, position, and
 * active affects. This enables web clients to display character sheets and
 * buff/debuff indicators without parsing descriptive text.
 *
 * Inputs:
 *   - web_desc: Web client descriptor
 *   - ch: Character whose status to send
 *
 * Outputs: Char.Status GMCP message sent to client
 *
 * Preconditions: ch must be valid player character, web_desc must be open
 * Postconditions: Client receives JSON with character status and affects
 *
 * Failure Behavior: Returns silently if web_desc or ch is NULL or connection not open
 *
 * Notes: Should be called when affects change, position changes, or on login
 */
void webgate_send_char_status(WEB_DESCRIPTOR_DATA *web_desc, CHAR_DATA *ch)
{
    char json[MAX_STRING_LENGTH * 2];
    char affect_list[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    bool first = true;
    int affect_count = 0;
    const char *pos_name;

    if (!web_desc || !ch || web_desc->state != WS_STATE_OPEN)
        return;

    /* Build affects array */
    sprintf(affect_list, "[");
    for (paf = ch->affected; paf; paf = paf->next)
    {
        if (paf->type < 0 || paf->type >= MAX_SKILL)
            continue;

        if (!skill_table[paf->type].name || !skill_table[paf->type].name[0])
            continue;

        if (!first)
            strcat(affect_list, ",");

        sprintf(affect_list + strlen(affect_list),
                "{\"name\":\"%s\",\"duration\":%d}",
                skill_table[paf->type].name,
                paf->duration);
        first = false;
        affect_count++;
    }
    strcat(affect_list, "]");

    /* Get position name */
    switch (ch->position)
    {
    case POS_DEAD:
        pos_name = "dead";
        break;
    case POS_MORTAL:
        pos_name = "mortally wounded";
        break;
    case POS_INCAP:
        pos_name = "incapacitated";
        break;
    case POS_STUNNED:
        pos_name = "stunned";
        break;
    case POS_SLEEPING:
        pos_name = "sleeping";
        break;
    case POS_RESTING:
        pos_name = "resting";
        break;
    case POS_FIGHTING:
        pos_name = "fighting";
        break;
    case POS_STANDING:
        pos_name = "standing";
        break;
    default:
        pos_name = "unknown";
        break;
    }

    snprintf(json, sizeof(json),
             "{\"name\":\"%s\",\"class\":\"%s\",\"race\":\"%s\","
             "\"position\":\"%s\",\"affects\":%s}",
             ch->name,
             class_table[ch->class].show_name,
             race_table[ch->race].race_name,
             pos_name,
             affect_list);

    webgate_send_gmcp(web_desc, "Char.Status", json);

    sprintf(log_buf, "WebGate: Sent Char.Status (%s - %d affects)",
            ch->name, affect_count);
    log_string(log_buf);
}

/*
 * Intent: Send WebSocket ping frame to keep connection alive.
 *
 * Sends a WebSocket PING frame (opcode 0x9) to the client. Client must
 * respond with a PONG frame to indicate connection is still alive. This
 * prevents idle timeout disconnections and detects broken connections.
 *
 * Inputs: web_desc - Web client to ping
 * Outputs: None (PING frame queued for send)
 *
 * Preconditions: web_desc is in WS_STATE_OPEN
 * Postconditions: PING frame sent; last_ping timestamp updated
 *
 * Failure Behavior: Silently returns if connection not open
 *
 * Notes: RFC-6455 §5.5.2 - PING frames may include application data
 */
void webgate_send_ping(WEB_DESCRIPTOR_DATA *web_desc)
{
    unsigned char frame[2];

    if (!web_desc || web_desc->state != WS_STATE_OPEN)
        return;

    /* WebSocket PING frame: FIN=1, opcode=0x9, no payload */
    frame[0] = 0x89; /* 10001001: FIN + PING opcode */
    frame[1] = 0x00; /* No payload, no mask */

    webgate_send_raw(web_desc, (char *)frame, 2);
    web_desc->last_ping = time(NULL);

    sprintf(log_buf, "WebGate: Sent PING to fd %d", web_desc->fd);
    log_string(log_buf);
}

/*
 * Intent: Perform periodic maintenance on WebSocket connections.
 *
 * Called once per game loop iteration (heartbeat). Sends PING frames to
 * idle connections and closes connections that haven't responded to PING
 * within the timeout period.
 *
 * Inputs: None
 * Outputs: None (may close unresponsive connections)
 *
 * Preconditions: Called from main game loop
 * Postconditions: Stale connections cleaned up; PINGs sent to idle connections
 *
 * Failure Behavior: Per-connection errors handled individually
 *
 * Timing:
 * - Send PING every 30 seconds of idle time
 * - Close connection if no PONG received within 60 seconds
 */
void webgate_pulse(void)
{
    WEB_DESCRIPTOR_DATA *web_desc, *web_next;
    time_t now = time(NULL);

    for (web_desc = web_descriptor_list; web_desc; web_desc = web_next)
    {
        web_next = web_desc->next;

        if (web_desc->state != WS_STATE_OPEN)
            continue;

        /* Send PING every 30 seconds */
        if (now - web_desc->last_ping >= 30)
        {
            webgate_send_ping(web_desc);
        }

        /* Close connection if no response for 90 seconds */
        if (now - web_desc->last_ping >= 90)
        {
            sprintf(log_buf, "WebGate: Connection timeout (no pong) for fd %d", web_desc->fd);
            log_string(log_buf);
            webgate_close(web_desc);
        }
    }
}

/*
 * Intent: Notify all web clients for a character of GMCP updates.
 *
 * Called from protocol.c when GMCP packages are updated. Finds all web
 * descriptors for the given MUD descriptor and triggers vitals/status sends.
 * This bridges the GMCP protocol (telnet-based) to WebSocket clients.
 *
 * Inputs: mud_desc - MUD descriptor that has GMCP updates
 * Outputs: None (updates sent to all associated web clients)
 *
 * Preconditions: mud_desc has character attached
 * Postconditions: Web clients receive Char.Vitals and/or Char.Status
 *
 * Failure Behavior: Silently returns if no character or no web clients
 *
 * Notes: Called from SendUpdatedGMCP() in protocol.c
 */
void webgate_notify_gmcp_update(DESCRIPTOR_DATA *mud_desc)
{
    WEB_DESCRIPTOR_DATA *web_desc;
    CHAR_DATA *ch;

    if (!mud_desc || !mud_desc->character)
        return;

    ch = mud_desc->character;

    /* Find all web descriptors for this character and update them */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc == mud_desc && web_desc->state == WS_STATE_OPEN)
        {
            /* Send updated vitals - these change frequently */
            webgate_send_char_vitals(web_desc, ch);

            /* Send updated status - these change less frequently */
            webgate_send_char_status(web_desc, ch);
        }
    }
}

/*
 * Intent: Send inventory update to all web clients for a MUD descriptor.
 *
 * Helper function for command handlers (get, drop, wear, remove) to trigger
 * inventory updates. Finds all web descriptors associated with the MUD
 * descriptor and sends updated inventory data.
 *
 * Inputs: mud_desc - MUD descriptor whose character's inventory changed
 * Outputs: None (Char.Inventory GMCP sent to all associated web clients)
 *
 * Preconditions: mud_desc has character attached
 * Postconditions: Web clients receive updated inventory
 */
void webgate_send_char_inventory_for_desc(DESCRIPTOR_DATA *mud_desc)
{
    WEB_DESCRIPTOR_DATA *web_desc;
    CHAR_DATA *ch;

    if (!mud_desc || !mud_desc->character)
        return;

    ch = mud_desc->character;

    /* Find all web descriptors for this character and update inventory */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc == mud_desc && web_desc->state == WS_STATE_OPEN)
        {
            webgate_send_char_inventory(web_desc, ch);
        }
    }
}

/*
 * Intent: Send equipment update to all web clients for a MUD descriptor.
 *
 * Helper function for command handlers (wear, remove) to trigger
 * equipment updates. Finds all web descriptors associated with the MUD
 * descriptor and sends updated equipment data.
 *
 * Inputs: mud_desc - MUD descriptor whose character's equipment changed
 * Outputs: None (Char.Equipment GMCP sent to all associated web clients)
 *
 * Preconditions: mud_desc has character attached
 * Postconditions: Web clients receive updated equipment
 */
void webgate_send_char_equipment_for_desc(DESCRIPTOR_DATA *mud_desc)
{
    WEB_DESCRIPTOR_DATA *web_desc;
    CHAR_DATA *ch;

    if (!mud_desc || !mud_desc->character)
        return;

    ch = mud_desc->character;

    /* Find all web descriptors for this character and update equipment */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next)
    {
        if (web_desc->mud_desc == mud_desc && web_desc->state == WS_STATE_OPEN)
        {
            webgate_send_char_equipment(web_desc, ch);
        }
    }
}

/*
 * Intent: Parse incoming JSON message and convert to MUD command.
 *
 * Parses a JSON action message from the web client and converts it
 * to an equivalent MUD command string. Supports common actions like
 * movement, combat, inventory management.
 *
 * Inputs: json_msg - JSON string from client
 * Outputs: Allocated command string or NULL on parse error
 *
 * Preconditions: json_msg is null-terminated
 * Postconditions: Caller must free() returned string
 *
 * Failure Behavior: Returns NULL, logs parse error
 *
 * Example transformations:
 *   {"action":"move","dir":"north"}  → "north"
 *   {"action":"get","item":"sword"}  → "get sword"
 *   {"action":"say","text":"hello"}  → "say hello"
 */
char *webgate_json_to_command(const char *json_msg)
{
    /* TODO: Implement JSON parsing and command conversion */
    /* For MVP, we'll implement a simple parser */
    /* In production, use a JSON library like cJSON or jsmn */

    char *result;

    /* Placeholder: return a copy of the input for now */
    result = malloc(strlen(json_msg) + 1);
    if (result)
    {
        strcpy(result, json_msg);
    }

    return result;
}

/* ================================================================
 * Internal helper functions
 * ================================================================ */

/*
 * Intent: Read incoming data from WebSocket client without blocking.
 *
 * Reads available data into the input buffer and processes it based
 * on connection state (handshake vs established). Handles partial
 * reads and accumulates data in inbuf for proper handshake processing.
 */
static void webgate_read(WEB_DESCRIPTOR_DATA *web_desc)
{
    char buf[4096];
    int nread;

    /* Read from socket */
    nread = read(web_desc->fd, buf, sizeof(buf) - 1);

    if (nread < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("webgate_read: read");
            web_desc->state = WS_STATE_CLOSED;
        }
        return;
    }

    if (nread == 0)
    {
        /* Connection closed by peer */
        web_desc->state = WS_STATE_CLOSED;
        return;
    }

    /* Append into descriptor input buffer (preserve previous partial data) */
    buf[nread] = '\0';
    if (!web_desc->inbuf)
    {
        web_desc->inbuf = malloc(nread + 1);
        if (!web_desc->inbuf)
        {
            bug("webgate_read: malloc failed", 0);
            web_desc->state = WS_STATE_CLOSED;
            return;
        }
        memcpy(web_desc->inbuf, buf, nread + 1);
        web_desc->inbuf_size = nread;
    }
    else
    {
        char *nb = realloc(web_desc->inbuf, web_desc->inbuf_size + nread + 1);
        if (!nb)
        {
            bug("webgate_read: realloc failed", 0);
            web_desc->state = WS_STATE_CLOSED;
            return;
        }
        memcpy(nb + web_desc->inbuf_size, buf, nread + 1);
        web_desc->inbuf = nb;
        web_desc->inbuf_size += nread;
    }

    /* Process based on state */
    if (web_desc->state == WS_STATE_HANDSHAKE)
    {
        webgate_process_handshake(web_desc);
    }
    else if (web_desc->state == WS_STATE_OPEN)
    {
        /* Parse WebSocket frames from buffer */
        webgate_parse_websocket_frames(web_desc);
    }
}

/*
 * Intent: Write queued data to WebSocket client without blocking.
 *
 * Attempts to send any queued output to the client. Handles partial
 * writes and maintains the output buffer correctly.
 */
static void webgate_write(WEB_DESCRIPTOR_DATA *web_desc)
{
    int nwritten;

    if (web_desc->outbuf_size <= 0)
    {
        return;
    }

    /* Write to socket */
    nwritten = write(web_desc->fd, web_desc->outbuf, web_desc->outbuf_size);

    if (nwritten < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("webgate_write: write");
            web_desc->state = WS_STATE_CLOSED;
        }
        return;
    }

    /* Update output buffer */
    if (nwritten >= web_desc->outbuf_size)
    {
        /* Everything written */
        free(web_desc->outbuf);
        web_desc->outbuf = NULL;
        web_desc->outbuf_size = 0;
    }
    else
    {
        /* Partial write - shift remaining data */
        memmove(web_desc->outbuf, web_desc->outbuf + nwritten,
                web_desc->outbuf_size - nwritten);
        web_desc->outbuf_size -= nwritten;
    }
}

/*
 * Intent: Parse incoming WebSocket frames from client per RFC-6455.
 *
 * Decodes WebSocket frames from inbuf, extracts payload, unmasks data
 * (client-to-server frames are always masked), and passes text frames
 * to the JSON message parser. Handles fragmentation and control frames.
 *
 * Inputs: web_desc with accumulated frame data in inbuf
 * Outputs: Parsed messages sent to webgate_parse_json_message
 * Preconditions: web_desc in OPEN state; inbuf contains raw frame data
 * Postconditions: Consumed frames removed from inbuf
 * Failure Behavior: Closes connection on protocol violations
 */
static bool webgate_parse_websocket_frames(WEB_DESCRIPTOR_DATA *web_desc)
{
    unsigned char *buf;
    int buf_len;
    int pos = 0;

    if (!web_desc->inbuf || web_desc->inbuf_size < 2)
    {
        /* Need at least 2 bytes for minimal frame header */
        return false;
    }

    buf = (unsigned char *)web_desc->inbuf;
    buf_len = web_desc->inbuf_size;

    while (pos + 2 <= buf_len)
    {
        unsigned char byte1 = buf[pos];
        unsigned char byte2 = buf[pos + 1];
        bool fin = (byte1 & 0x80) != 0;
        unsigned char opcode = byte1 & 0x0F;
        bool masked = (byte2 & 0x80) != 0;
        uint64_t payload_len = byte2 & 0x7F;
        int header_len = 2;
        unsigned char mask_key[4];
        int i;

        /* Client-to-server frames MUST be masked per RFC-6455 */
        if (!masked)
        {
            sprintf(log_buf, "WebGate: Received unmasked frame from client (protocol violation)");
            log_string(log_buf);
            web_desc->state = WS_STATE_CLOSED;
            return false;
        }

        /* Decode extended payload length */
        if (payload_len == 126)
        {
            if (pos + 4 > buf_len)
                break; /* Need more data */
            payload_len = ((uint64_t)buf[pos + 2] << 8) | buf[pos + 3];
            header_len = 4;
        }
        else if (payload_len == 127)
        {
            if (pos + 10 > buf_len)
                break; /* Need more data */
            payload_len = ((uint64_t)buf[pos + 2] << 56) |
                          ((uint64_t)buf[pos + 3] << 48) |
                          ((uint64_t)buf[pos + 4] << 40) |
                          ((uint64_t)buf[pos + 5] << 32) |
                          ((uint64_t)buf[pos + 6] << 24) |
                          ((uint64_t)buf[pos + 7] << 16) |
                          ((uint64_t)buf[pos + 8] << 8) |
                          (uint64_t)buf[pos + 9];
            header_len = 10;
        }

        /* Extract masking key */
        if (pos + header_len + 4 > buf_len)
            break; /* Need more data */
        memcpy(mask_key, buf + pos + header_len, 4);
        header_len += 4;

        /* Check if we have complete frame */
        if (pos + header_len + payload_len > buf_len)
            break; /* Need more data */

        /* Unmask payload */
        unsigned char *payload = malloc(payload_len + 1);
        if (!payload)
        {
            bug("webgate_parse_websocket_frames: malloc failed", 0);
            web_desc->state = WS_STATE_CLOSED;
            return false;
        }

        for (i = 0; i < payload_len; i++)
        {
            payload[i] = buf[pos + header_len + i] ^ mask_key[i % 4];
        }
        payload[payload_len] = '\0';

        /* Handle frame based on opcode */
        switch (opcode)
        {
        case 0x01: /* Text frame */
            if (fin)
            {
                webgate_parse_json_message(web_desc, (char *)payload);
            }
            else
            {
                /* TODO: Handle fragmented frames */
                sprintf(log_buf, "WebGate: Fragmented frames not yet supported");
                log_string(log_buf);
            }
            break;

        case 0x02: /* Binary frame */
            /* Client attempted to send binary data (possibly malformed ping) */
            /* Ignore silently or log at debug level */
            break;

        case 0x08: /* Close frame */
            sprintf(log_buf, "WebGate: Client sent close frame");
            log_string(log_buf);
            web_desc->state = WS_STATE_CLOSING;
            free(payload);
            return false;

        case 0x09: /* Ping frame */
            /* Client sent ping - respond with pong */
            /* Note: Most browsers auto-respond to server pings, so client shouldn't send these */
            sprintf(log_buf, "WebGate: Received ping from client (unusual)");
            log_string(log_buf);
            /* TODO: Send pong response if needed */
            break;

        case 0x0A: /* Pong frame */
            /* Update last_ping timestamp - this is the normal response to our pings */
            web_desc->last_ping = current_time;
            break;

        default:
            sprintf(log_buf, "WebGate: Unknown opcode 0x%02X", opcode);
            log_string(log_buf);
            break;
        }

        free(payload);

        /* Move to next frame */
        pos += header_len + payload_len;
    }

    /* Remove consumed data from buffer */
    if (pos > 0)
    {
        if (pos >= buf_len)
        {
            /* All data consumed */
            free(web_desc->inbuf);
            web_desc->inbuf = NULL;
            web_desc->inbuf_size = 0;
        }
        else
        {
            /* Partial consumption - shift remaining data */
            int remaining = buf_len - pos;
            memmove(web_desc->inbuf, web_desc->inbuf + pos, remaining);
            web_desc->inbuf_size = remaining;
            web_desc->inbuf[remaining] = '\0';
        }
    }

    return true;
}

/*
 * Intent: Process WebSocket handshake request according to RFC-6455.
 *
 * Parses the HTTP Upgrade request, extracts Sec-WebSocket-Key, computes
 * the Sec-WebSocket-Accept response per RFC-6455 (SHA1 + base64), and
 * sends a valid 101 Switching Protocols response. On success, transitions
 * descriptor to WS_STATE_OPEN.
 *
 * Inputs: web_desc with incoming HTTP Upgrade request in web_desc->inbuf
 * Outputs: Sends HTTP/1.1 101 response with Sec-WebSocket-Accept header
 * Preconditions: web_desc->inbuf contains at least partial HTTP request
 * Postconditions: On success, connection in OPEN state; on failure, CLOSED
 * Failure Behavior: On malformed or missing Sec-WebSocket-Key, logs and closes
 */
static bool webgate_process_handshake(WEB_DESCRIPTOR_DATA *web_desc)
{
    const char *key_hdr = "Sec-WebSocket-Key:";
    const char *guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char *p, *line_end;
    char key[256];
    char accept_src[512];
    unsigned char sha1_digest[20];
    char accept_b64[64];
    int accept_len;
    char response[1024];
    int response_len;

    if (!web_desc->inbuf)
    {
        return false;
    }

    /* Ensure we have received the full HTTP headers (look for double-CRLF) */
    if (!strstr(web_desc->inbuf, "\r\n\r\n"))
    {
        /* Headers not complete yet */
        return false;
    }

    /* Find Sec-WebSocket-Key header */
    p = strstr(web_desc->inbuf, key_hdr);
    if (!p)
    {
        /* No key found — invalid handshake */
        sprintf(log_buf, "WebGate: Handshake missing %s", key_hdr);
        log_string(log_buf);
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }

    /* Move pointer to header value and trim whitespace */
    p += strlen(key_hdr);
    while (*p == ' ' || *p == '\t')
        p++;
    line_end = strstr(p, "\r\n");
    if (!line_end)
    {
        log_string("WebGate: Malformed Sec-WebSocket-Key header");
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }

    /* Copy key (safe-guard length) */
    int klen = (int)(line_end - p);
    if (klen <= 0 || klen >= (int)sizeof(key))
    {
        log_string("WebGate: Invalid Sec-WebSocket-Key length");
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }
    memcpy(key, p, klen);
    key[klen] = '\0';

    /* Compute SHA1(key + GUID) using OpenSSL */
    snprintf(accept_src, sizeof(accept_src), "%s%s", key, guid);
    SHA1((unsigned char *)accept_src, strlen(accept_src), sha1_digest);

    /* Base64-encode the SHA1 digest using OpenSSL EVP_EncodeBlock */
    accept_len = EVP_EncodeBlock((unsigned char *)accept_b64, sha1_digest, sizeof(sha1_digest));
    if (accept_len <= 0 || accept_len >= (int)sizeof(accept_b64))
    {
        bug("webgate_process_handshake: base64 encode failed", 0);
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }
    accept_b64[accept_len] = '\0';

    /* Send proper RFC-6455 handshake response */
    response_len = snprintf(response, sizeof(response),
                            "HTTP/1.1 101 Switching Protocols\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Accept: %s\r\n"
                            "\r\n",
                            accept_b64);

    if (response_len >= (int)sizeof(response))
    {
        bug("webgate_process_handshake: response buffer overflow", 0);
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }

    webgate_send_raw(web_desc, response, response_len);

    /* Transition to OPEN state and clear consumed handshake data */
    web_desc->state = WS_STATE_OPEN;
    free(web_desc->inbuf);
    web_desc->inbuf = NULL;
    web_desc->inbuf_size = 0;

    log_string("WebGate: Handshake completed (RFC-6455)");

    /* Create MUD descriptor for authentication flow */
    if (!webgate_create_mud_descriptor(web_desc))
    {
        log_string("WebGate: Failed to create MUD descriptor");
        web_desc->state = WS_STATE_CLOSED;
        return false;
    }

    /* Send welcome message */
    webgate_send_gmcp(web_desc, "Comm.Channel",
                      "{\"channel\":\"system\",\"message\":\"Welcome to Dragons Domain IV\"}");

    /* Send login prompt */
    webgate_send_prompt(web_desc, "Name: ");

    return true;
}

/*
 * Intent: Parse a JSON message and route to appropriate handler.
 *
 * Parses the JSON message, extracts command or action, and routes to
 * the MUD descriptor's nanny() state machine for authentication or
 * interpret_command() for game commands based on connection state.
 *
 * Inputs: web_desc with linked mud_desc, json string containing command
 * Outputs: Routes command text to MUD command processor
 * Preconditions: mud_desc must be created and linked
 * Postconditions: Command queued to MUD descriptor's input buffer
 * Failure Behavior: Logs parse errors, ignores malformed messages
 */
static bool webgate_parse_json_message(WEB_DESCRIPTOR_DATA *web_desc, const char *json)
{
    char *cmd_start, *cmd_end;
    char command[MAX_INPUT_LENGTH];
    int cmd_len;

    if (!web_desc->mud_desc)
    {
        bug("webgate_parse_json_message: no mud descriptor", 0);
        return false;
    }

    sprintf(log_buf, "WebGate: Received JSON: %.200s", json);
    log_string(log_buf);

    /* Simple JSON parsing: look for "command":"<text>" */
    cmd_start = strstr(json, "\"command\"");
    if (!cmd_start)
    {
        /* No command field - ignore */
        return true;
    }

    /* Find the value after "command": */
    cmd_start = strchr(cmd_start, ':');
    if (!cmd_start)
        return true;
    cmd_start++;

    /* Skip whitespace and opening quote */
    while (*cmd_start == ' ' || *cmd_start == '\t')
        cmd_start++;
    if (*cmd_start != '"')
        return true;
    cmd_start++;

    /* Find closing quote */
    cmd_end = strchr(cmd_start, '"');
    if (!cmd_end)
        return true;

    /* Extract command */
    cmd_len = (int)(cmd_end - cmd_start);
    if (cmd_len <= 0 || cmd_len >= MAX_INPUT_LENGTH)
    {
        sprintf(log_buf, "WebGate: Command length invalid: %d", cmd_len);
        log_string(log_buf);
        return true;
    }

    memcpy(command, cmd_start, cmd_len);
    command[cmd_len] = '\0';

    sprintf(log_buf, "WebGate: Parsed command: %.100s", command);
    log_string(log_buf);

    /* Write command directly to input buffer (not output buffer!) */
    snprintf(web_desc->mud_desc->inbuf, sizeof(web_desc->mud_desc->inbuf),
             "%s\n", command);

    /* Process the input immediately since web descriptors aren't in select() */
    read_from_buffer(web_desc->mud_desc);

    sprintf(log_buf, "WebGate: After read_from_buffer, incomm='%.50s'",
            web_desc->mud_desc->incomm);
    log_string(log_buf);

    /* Call nanny or interpret based on connection state */
    if (web_desc->mud_desc->incomm[0] != '\0')
    {
        sprintf(log_buf, "WebGate: Calling nanny/interpret (connected=%d)",
                web_desc->mud_desc->connected);
        log_string(log_buf);

        if (web_desc->mud_desc->connected == CON_PLAYING)
        {
            if (web_desc->mud_desc->character)
            {
                /* Reset idle timer when processing web commands */
                web_desc->mud_desc->character->timer = 0;

                interpret(web_desc->mud_desc->character, web_desc->mud_desc->incomm);

                /* Send updated room info after command execution */
                if (web_desc->mud_desc->character->in_room)
                {
                    webgate_send_room_info(web_desc, web_desc->mud_desc->character->in_room);
                }
            }
        }
        else
        {
            int old_state = web_desc->mud_desc->connected;
            nanny(web_desc->mud_desc, web_desc->mud_desc->incomm);

            /* Check if player just entered the game */
            if (old_state != CON_PLAYING && web_desc->mud_desc->connected == CON_PLAYING)
            {
                if (web_desc->mud_desc->character && web_desc->mud_desc->character->in_room)
                {
                    webgate_send_room_info(web_desc, web_desc->mud_desc->character->in_room);
                    webgate_send_char_vitals(web_desc, web_desc->mud_desc->character);
                    webgate_send_char_status(web_desc, web_desc->mud_desc->character);
                    webgate_send_char_skills(web_desc, web_desc->mud_desc->character);
                    webgate_send_char_inventory(web_desc, web_desc->mud_desc->character);
                    webgate_send_char_equipment(web_desc, web_desc->mud_desc->character);
                }
            }
        }
        web_desc->mud_desc->incomm[0] = '\0';

        sprintf(log_buf, "WebGate: After nanny/interpret, outtop=%d",
                web_desc->mud_desc->outtop);
        log_string(log_buf);
    }

    return true;
}

/*
 * Intent: Send WebSocket-framed text message to client per RFC-6455.
 *
 * Encodes the data as a WebSocket text frame (opcode 0x01) with proper
 * framing header and appends to output buffer. Server-to-client frames
 * are never masked per RFC-6455 section 5.1.
 *
 * Inputs: web_desc - target client; data - payload; len - payload length
 * Outputs: None (appends framed message to outbuf)
 * Preconditions: web_desc in OPEN state; data is valid UTF-8 text
 * Postconditions: WebSocket frame queued for transmission
 * Failure Behavior: Logs and drops message on allocation failure
 */
static void webgate_send_text_frame(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len)
{
    unsigned char frame[14]; /* Max header size for any payload */
    int frame_len = 0;
    char *new_buf;
    int total_len;

    if (!web_desc || !data || len <= 0)
    {
        return;
    }

    /* Build WebSocket frame header (RFC-6455 section 5.2) */
    frame[0] = 0x81; /* FIN=1, opcode=1 (text frame) */

    /* Encode payload length */
    if (len < 126)
    {
        frame[1] = (unsigned char)len; /* mask=0, len=0-125 */
        frame_len = 2;
    }
    else if (len < 65536)
    {
        frame[1] = 126; /* mask=0, len=126 means next 2 bytes are length */
        frame[2] = (unsigned char)((len >> 8) & 0xFF);
        frame[3] = (unsigned char)(len & 0xFF);
        frame_len = 4;
    }
    else
    {
        frame[1] = 127; /* mask=0, len=127 means next 8 bytes are length */
        /* For simplicity, we only use the lower 32 bits */
        frame[2] = frame[3] = frame[4] = frame[5] = 0;
        frame[6] = (unsigned char)((len >> 24) & 0xFF);
        frame[7] = (unsigned char)((len >> 16) & 0xFF);
        frame[8] = (unsigned char)((len >> 8) & 0xFF);
        frame[9] = (unsigned char)(len & 0xFF);
        frame_len = 10;
    }

    total_len = frame_len + len;

    /* Allocate or expand output buffer */
    new_buf = realloc(web_desc->outbuf, web_desc->outbuf_size + total_len);
    if (!new_buf)
    {
        bug("webgate_send_text_frame: realloc failed", 0);
        return;
    }

    /* Append frame header and payload */
    memcpy(new_buf + web_desc->outbuf_size, frame, frame_len);
    memcpy(new_buf + web_desc->outbuf_size + frame_len, data, len);
    web_desc->outbuf = new_buf;
    web_desc->outbuf_size += total_len;
}

/*
 * Intent: Queue raw data for sending to web client (unframed).
 *
 * Appends data to the output buffer without WebSocket framing.
 * Use only for handshake responses; all post-handshake messages
 * must use webgate_send_text_frame.
 */
static void webgate_send_raw(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len)
{
    char *new_buf;

    if (!web_desc || !data || len <= 0)
    {
        return;
    }

    /* Allocate or expand output buffer */
    new_buf = realloc(web_desc->outbuf, web_desc->outbuf_size + len);
    if (!new_buf)
    {
        bug("webgate_send_raw: realloc failed", 0);
        return;
    }

    /* Append new data */
    memcpy(new_buf + web_desc->outbuf_size, data, len);
    web_desc->outbuf = new_buf;
    web_desc->outbuf_size += len;
}

/*
 * Intent: Allocate and initialize a new web descriptor.
 *
 * Creates a clean descriptor structure with all fields zeroed
 * and ready for use with a new client connection.
 */
static WEB_DESCRIPTOR_DATA *webgate_new_descriptor(void)
{
    WEB_DESCRIPTOR_DATA *web_desc;

    web_desc = calloc(1, sizeof(WEB_DESCRIPTOR_DATA));
    if (!web_desc)
    {
        bug("webgate_new_descriptor: calloc failed", 0);
        return NULL;
    }

    web_desc->fd = -1;
    web_desc->state = WS_STATE_CONNECTING;

    return web_desc;
}

/*
 * Intent: Free all memory associated with a web descriptor.
 *
 * Cleans up dynamically allocated buffers and the descriptor itself.
 * Does not close the socket or modify the list - caller's responsibility.
 */
static void webgate_free_descriptor(WEB_DESCRIPTOR_DATA *web_desc)
{
    if (!web_desc)
    {
        return;
    }

    if (web_desc->inbuf)
    {
        free(web_desc->inbuf);
    }

    if (web_desc->outbuf)
    {
        free(web_desc->outbuf);
    }

    if (web_desc->session_id)
    {
        free(web_desc->session_id);
    }

    free(web_desc);
}

/*
 * Intent: Create and link a MUD descriptor to handle authentication and gameplay.
 *
 * Allocates a DESCRIPTOR_DATA structure, links it to the web descriptor,
 * adds it to the global descriptor_list, sets initial connection state to
 * CON_GET_NAME for authentication, and sets up protocol handlers.
 *
 * Inputs: web_desc with established WebSocket connection
 * Outputs: Returns newly created mud_desc; sets web_desc->mud_desc pointer
 * Preconditions: web_desc in WS_STATE_OPEN with valid socket
 * Postconditions: mud_desc added to descriptor_list, ready for nanny() flow
 * Failure Behavior: Returns NULL on allocation failure; logs error
 */
static DESCRIPTOR_DATA *webgate_create_mud_descriptor(WEB_DESCRIPTOR_DATA *web_desc)
{
    DESCRIPTOR_DATA *mud_desc;

    if (!web_desc)
    {
        bug("webgate_create_mud_descriptor: null web_desc", 0);
        return NULL;
    }

    /* Allocate MUD descriptor */
    mud_desc = calloc(1, sizeof(DESCRIPTOR_DATA));
    if (!mud_desc)
    {
        bug("webgate_create_mud_descriptor: calloc failed", 0);
        return NULL;
    }

    /* Initialize descriptor fields to match new_descriptor() from comm.c */
    mud_desc->descriptor = -web_desc->fd; /* Negative fd to avoid select() conflicts */
    mud_desc->connected = CON_GET_NAME;   /* Start at name prompt */
    mud_desc->showstr_head = NULL;
    mud_desc->showstr_point = NULL;
    mud_desc->outsize = 2000;
    mud_desc->outbuf = alloc_mem(mud_desc->outsize);
    mud_desc->outtop = 0;
    mud_desc->character = NULL;
    mud_desc->original = NULL;
    mud_desc->next = NULL;
    mud_desc->snoop_by = NULL;
    mud_desc->host = str_dup("web-client");
    mud_desc->fcommand = FALSE;
    mud_desc->inbuf[0] = '\0';
    mud_desc->incomm[0] = '\0';
    mud_desc->inlast[0] = '\0';
    mud_desc->repeat = 0;
    mud_desc->pProtocol = NULL;

    /* Add to global descriptor list */
    mud_desc->next = descriptor_list;
    descriptor_list = mud_desc;

    /* Link to web descriptor */
    web_desc->mud_desc = mud_desc;

    sprintf(log_buf, "WebGate: Created MUD descriptor (fd %d, state CON_GET_NAME)", web_desc->fd);
    log_string(log_buf);

    /* Initialize protocol for GMCP support */
    mud_desc->pProtocol = ProtocolCreate();

    /* Send initial Name: prompt */
    webgate_send_prompt(web_desc, "Name: ");

    return mud_desc;
}

/*
 * Intent: Send a prompt message to the web client via GMCP.
 *
 * Encodes the prompt as a GMCP message and sends it as a WebSocket frame.
 * Used during authentication (e.g., "Name: ", "Password: ") and gameplay.
 *
 * Inputs: web_desc with open connection, prompt string
 * Outputs: Sends GMCP Comm.Prompt message
 * Preconditions: WebSocket in OPEN state
 * Postconditions: Prompt visible in web client UI
 */
static void webgate_send_prompt(WEB_DESCRIPTOR_DATA *web_desc, const char *prompt)
{
    char json[MAX_STRING_LENGTH];

    if (!web_desc || !prompt)
        return;

    /* Format as GMCP prompt message */
    snprintf(json, sizeof(json), "{\"prompt\":\"%s\"}", prompt);
    webgate_send_gmcp(web_desc, "Comm.Prompt", json);
}

/*
 * Intent: Parse GMCP message from telnet format and send to web client.
 *
 * Extracts the module name (e.g., "Char.Vitals") and JSON data from a
 * telnet GMCP message, then forwards it to the web client in WebSocket format.
 *
 * Inputs: web_desc, gmcp_msg in format "Module.Message { json }"
 * Outputs: Sends GMCP message to web client
 * Preconditions: gmcp_msg is null-terminated GMCP message content
 * Postconditions: Web client receives {"type":"gmcp","module":"...","data":{...}}
 *
 * Example Input: "Char.Vitals { "hp": 100, "maxhp": 150 }"
 * Example Output: {"type":"gmcp","module":"Char.Vitals","data":{"hp":100,"maxhp":150}}
 */
static void webgate_parse_and_send_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *gmcp_msg)
{
    char module[256];
    char *json_start;
    char *space_pos;
    int module_len;

    if (!web_desc || !gmcp_msg)
        return;

    /* Find the space between "Module.Message" and "{ json }" */
    space_pos = strchr(gmcp_msg, ' ');
    if (!space_pos)
    {
        sprintf(log_buf, "WebGate: Malformed GMCP message (no space): %s", gmcp_msg);
        log_string(log_buf);
        return;
    }

    /* Extract module name */
    module_len = space_pos - gmcp_msg;
    if (module_len >= sizeof(module))
        module_len = sizeof(module) - 1;

    memcpy(module, gmcp_msg, module_len);
    module[module_len] = '\0';

    /* Find JSON start (skip spaces, find '{' or '[') */
    json_start = space_pos + 1;
    while (*json_start == ' ')
        json_start++;

    sprintf(log_buf, "WebGate: Parsed GMCP - Module: %s, JSON: %.100s", module, json_start);
    log_string(log_buf);

    /* Send to web client */
    webgate_send_gmcp(web_desc, module, json_start);
}

/*
 * Intent: Send regular text as Comm.Channel GMCP message.
 *
 * Escapes special JSON characters in text output and sends it as a
 * Comm.Channel message. Handles ANSI color codes by converting them
 * to JSON-safe Unicode escapes.
 *
 * Inputs: web_desc, text buffer, length
 * Outputs: Sends Comm.Channel GMCP message
 * Preconditions: text is valid buffer of specified length
 * Postconditions: Text delivered to web client as game output
 */
static void webgate_send_text_as_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *text, int len)
{
    char json[MAX_STRING_LENGTH * 2];
    char message_buf[MAX_STRING_LENGTH];
    int msg_len = 0;
    int i;

    if (!web_desc || !text || len <= 0)
        return;

    /* Escape special JSON characters */
    for (i = 0; i < len && msg_len < MAX_STRING_LENGTH - 10; i++)
    {
        unsigned char c = (unsigned char)text[i];

        /* Escape special JSON characters */
        if (c == '"')
        {
            message_buf[msg_len++] = '\\';
            message_buf[msg_len++] = '"';
        }
        else if (c == '\\')
        {
            message_buf[msg_len++] = '\\';
            message_buf[msg_len++] = '\\';
        }
        else if (c == '\n')
        {
            message_buf[msg_len++] = '\\';
            message_buf[msg_len++] = 'n';
        }
        else if (c == '\r')
        {
            /* Skip carriage returns */
            continue;
        }
        else if (c == '\t')
        {
            message_buf[msg_len++] = '\\';
            message_buf[msg_len++] = 't';
        }
        else if (c == 27) /* ESC - start of ANSI sequence */
        {
            /* Pass through ANSI escape sequences for client-side color rendering */
            /* Escape as \u001B for JSON */
            message_buf[msg_len++] = '\\';
            message_buf[msg_len++] = 'u';
            message_buf[msg_len++] = '0';
            message_buf[msg_len++] = '0';
            message_buf[msg_len++] = '1';
            message_buf[msg_len++] = 'B';
        }
        else if (c >= 32 && c < 127)
        {
            /* Regular printable ASCII character */
            message_buf[msg_len++] = c;
        }
        else if (c == 255)
        {
            /* IAC character that wasn't part of GMCP - skip it */
            sprintf(log_buf, "WebGate: Stray IAC (255) at position %d", i);
            log_string(log_buf);
            continue;
        }
        /* Skip other control characters */
    }
    message_buf[msg_len] = '\0';

    /* Send as GMCP Comm.Channel message */
    snprintf(json, sizeof(json), "{\"channel\":\"game\",\"message\":\"%s\"}", message_buf);
    webgate_send_gmcp(web_desc, "Comm.Channel", json);
}

/*
 * Intent: Transfer MUD descriptor output to WebSocket as GMCP messages.
 *
 * Parses the MUD descriptor output buffer for telnet IAC GMCP sequences and
 * regular text. GMCP messages are extracted and sent as proper JSON GMCP,
 * while regular text is sent as Comm.Channel messages. This bridges the
 * telnet-based GMCP protocol to WebSocket clients.
 *
 * Inputs: web_desc with linked mud_desc
 * Outputs: Transfers GMCP and text from mud_desc->outbuf to WebSocket; clears buffer
 * Preconditions: mud_desc exists and has output in outbuf
 * Postconditions: Output delivered to web client; mud_desc->outbuf cleared
 * Failure Behavior: Logs errors; does not block or crash
 *
 * GMCP Format: IAC SB TELOPT_GMCP Module.Message { json } IAC SE
 * Example: \xFF\xFA\xC9Char.Vitals { "hp": 100, "maxhp": 150 }\xFF\xF0
 */
static void webgate_transfer_mud_output(WEB_DESCRIPTOR_DATA *web_desc)
{
    DESCRIPTOR_DATA *mud_desc;
    unsigned char *buf;
    int buf_len;
    int i;
    char text_buf[MAX_STRING_LENGTH];
    int text_len;

    if (!web_desc || !web_desc->mud_desc)
        return;

    mud_desc = web_desc->mud_desc;

    /* Check if there's output to transfer */
    if (mud_desc->outtop <= 0)
        return;

    buf = (unsigned char *)mud_desc->outbuf;
    buf_len = mud_desc->outtop;
    text_len = 0;

    sprintf(log_buf, "WebGate: Transferring %d bytes of output", buf_len);
    log_string(log_buf);

    /* Parse buffer for IAC GMCP sequences and regular text */
    for (i = 0; i < buf_len; i++)
    {
        /* Check for IAC SB TELOPT_GMCP (255 250 201) */
        if (i + 2 < buf_len && buf[i] == 255 && buf[i + 1] == 250 && buf[i + 2] == 201)
        {
            /* Flush any pending text before processing GMCP */
            if (text_len > 0)
            {
                text_buf[text_len] = '\0';
                webgate_send_text_as_gmcp(web_desc, text_buf, text_len);
                text_len = 0;
            }

            /* Found GMCP sequence, extract it */
            int gmcp_start = i + 3; /* Skip IAC SB TELOPT_GMCP */
            int gmcp_end = gmcp_start;

            /* Find IAC SE (255 240) */
            while (gmcp_end + 1 < buf_len)
            {
                if (buf[gmcp_end] == 255 && buf[gmcp_end + 1] == 240)
                    break;
                gmcp_end++;
            }

            if (gmcp_end + 1 < buf_len && buf[gmcp_end] == 255 && buf[gmcp_end + 1] == 240)
            {
                /* Extract GMCP message */
                int gmcp_len = gmcp_end - gmcp_start;
                char gmcp_msg[MAX_STRING_LENGTH];

                if (gmcp_len < MAX_STRING_LENGTH)
                {
                    memcpy(gmcp_msg, buf + gmcp_start, gmcp_len);
                    gmcp_msg[gmcp_len] = '\0';

                    /* Parse and send GMCP message */
                    webgate_parse_and_send_gmcp(web_desc, gmcp_msg);
                }

                /* Skip past IAC SE */
                i = gmcp_end + 1;
            }
            else
            {
                /* Malformed GMCP, treat as regular text */
                if (text_len < MAX_STRING_LENGTH - 1)
                    text_buf[text_len++] = buf[i];
            }
        }
        else
        {
            /* Regular character, add to text buffer */
            if (text_len < MAX_STRING_LENGTH - 1)
                text_buf[text_len++] = buf[i];
        }
    }

    /* Send any remaining text */
    if (text_len > 0)
    {
        text_buf[text_len] = '\0';
        webgate_send_text_as_gmcp(web_desc, text_buf, text_len);
    }

    /* Clear the MUD output buffer */
    mud_desc->outtop = 0;
}
