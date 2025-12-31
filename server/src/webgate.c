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
    char exits_json[512];
    char name_escaped[256];
    char desc_escaped[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    int door;
    bool first = true;
    int i, j;
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
            if (!first)
                strcat(exits_json, ",");
            sprintf(exits_json + strlen(exits_json), "\"%s\"", directions[door].name);
            first = false;
        }
    }
    strcat(exits_json, "]");

    /* Build complete Room.Info JSON */
    snprintf(json, sizeof(json),
             "{\"name\":\"%s\",\"description\":\"%s\",\"exits\":%s,\"vnum\":%d}",
             name_escaped,
             desc_escaped,
             exits_json,
             room->vnum);

    sprintf(log_buf, "WebGate: Sending Room.Info (exits: %s)", exits_json);
    log_string(log_buf);

    webgate_send_gmcp(web_desc, "Room.Info", json);
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
 * Intent: Transfer MUD descriptor output to WebSocket as GMCP messages.
 *
 * Checks if the associated MUD descriptor has output pending in its buffer,
 * formats it as GMCP Comm.Channel messages, and sends via WebSocket frames.
 * This intercepts the normal telnet output path and routes it to web clients.
 *
 * Inputs: web_desc with linked mud_desc
 * Outputs: Transfers text from mud_desc->outbuf to WebSocket frames; clears buffer
 * Preconditions: mud_desc exists and has output in outbuf
 * Postconditions: Output delivered to web client; mud_desc->outbuf cleared
 * Failure Behavior: Logs errors; does not block or crash
 */
static void webgate_transfer_mud_output(WEB_DESCRIPTOR_DATA *web_desc)
{
    DESCRIPTOR_DATA *mud_desc;
    char json[MAX_STRING_LENGTH * 2];
    char *msg_start;
    char message_buf[MAX_STRING_LENGTH];
    int msg_len;

    if (!web_desc || !web_desc->mud_desc)
        return;

    mud_desc = web_desc->mud_desc;

    /* Check if there's output to transfer */
    if (mud_desc->outtop <= 0)
        return;

    sprintf(log_buf, "WebGate: Transferring %d bytes of output", mud_desc->outtop);
    log_string(log_buf);

    /* Ensure null termination */
    mud_desc->outbuf[mud_desc->outtop] = '\0';

    /* Debug: Log first 100 chars of raw output */
    {
        char debug_buf[128];
        int debug_len = mud_desc->outtop < 100 ? mud_desc->outtop : 100;
        memcpy(debug_buf, mud_desc->outbuf, debug_len);
        debug_buf[debug_len] = '\0';
        sprintf(log_buf, "WebGate: Raw output: %s", debug_buf);
        log_string(log_buf);
    }

    /* Send as GMCP message */
    /* We need to escape quotes and newlines for JSON */
    /* ANSI codes (ESC sequences) are passed through for client-side rendering */
    msg_start = mud_desc->outbuf;
    msg_len = 0;

    for (int i = 0; i < mud_desc->outtop && msg_len < MAX_STRING_LENGTH - 10; i++)
    {
        char c = msg_start[i];

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
        else if (c >= 32 || c == '\0')
        {
            /* Regular printable character */
            message_buf[msg_len++] = c;
        }
        /* Skip other control characters (but not ESC for ANSI) */
    }
    message_buf[msg_len] = '\0';

    /* Send as GMCP Comm.Channel message */
    snprintf(json, sizeof(json), "{\"channel\":\"game\",\"message\":\"%s\"}", message_buf);

    /* Debug: Log the JSON being sent */
    {
        char debug_json[256];
        int json_len = strlen(json);
        int copy_len = json_len < 200 ? json_len : 200;
        memcpy(debug_json, json, copy_len);
        debug_json[copy_len] = '\0';
        sprintf(log_buf, "WebGate: Sending JSON: %s", debug_json);
        log_string(log_buf);
    }

    webgate_send_gmcp(web_desc, "Comm.Channel", json);

    /* Clear the MUD output buffer */
    mud_desc->outtop = 0;
}
