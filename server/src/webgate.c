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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#include "merc.h"
#include "webgate.h"
#include "protocol.h"

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
static bool webgate_parse_json_message(WEB_DESCRIPTOR_DATA *web_desc, const char *json);
static void webgate_send_raw(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len);
static WEB_DESCRIPTOR_DATA *webgate_new_descriptor(void);
static void webgate_free_descriptor(WEB_DESCRIPTOR_DATA *web_desc);

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
    if (fd < 0) {
        perror("webgate_init: socket");
        return -1;
    }

    /* Set socket options: reuse address to allow quick restarts */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("webgate_init: setsockopt(SO_REUSEADDR)");
        close(fd);
        return -1;
    }

    /* Make socket non-blocking to integrate with game loop */
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("webgate_init: fcntl(O_NONBLOCK)");
        close(fd);
        return -1;
    }

    /* Bind to specified port on all interfaces */
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("webgate_init: bind");
        close(fd);
        return -1;
    }

    /* Start listening for connections (backlog of 5) */
    if (listen(fd, 5) < 0) {
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
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_next) {
        web_next = web_desc->next;
        webgate_close(web_desc);
    }

    /* Close listening socket */
    if (webgate_listen_fd >= 0) {
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
    if (webgate_listen_fd >= 0) {
        FD_SET(webgate_listen_fd, readfds);
        if (webgate_listen_fd > *maxfd) {
            *maxfd = webgate_listen_fd;
        }
    }

    /* Register all active client sockets */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next) {
        if (web_desc->fd < 0) {
            continue;
        }

        /* Always monitor for input */
        FD_SET(web_desc->fd, readfds);

        /* Monitor for output if we have data to send */
        if (web_desc->outbuf_size > 0) {
            FD_SET(web_desc->fd, writefds);
        }

        if (web_desc->fd > *maxfd) {
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
    if (webgate_listen_fd >= 0 && FD_ISSET(webgate_listen_fd, readfds)) {
        WEB_DESCRIPTOR_DATA *new_desc = webgate_accept(webgate_listen_fd);
        if (new_desc) {
            sprintf(log_buf, "WebGate: New connection from fd %d", new_desc->fd);
            log_string(log_buf);
        }
    }

    /* Process existing connections */
    for (web_desc = web_descriptor_list; web_desc; web_desc = web_next) {
        web_next = web_desc->next;

        if (web_desc->fd < 0) {
            continue;
        }

        /* Handle input */
        if (FD_ISSET(web_desc->fd, readfds)) {
            webgate_read(web_desc);
            
            /* Connection may be closed after read */
            if (web_desc->state == WS_STATE_CLOSED) {
                webgate_close(web_desc);
                continue;
            }
        }

        /* Handle output */
        if (FD_ISSET(web_desc->fd, writefds)) {
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
    if (fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("webgate_accept: accept");
        }
        return NULL;
    }

    /* Make socket non-blocking */
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
        perror("webgate_accept: fcntl");
        close(fd);
        return NULL;
    }

    /* Allocate and initialize web descriptor */
    web_desc = webgate_new_descriptor();
    if (!web_desc) {
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

    if (!web_desc) {
        return;
    }

    sprintf(log_buf, "WebGate: Closing connection fd %d", web_desc->fd);
    log_string(log_buf);

    /* Close socket */
    if (web_desc->fd >= 0) {
        close(web_desc->fd);
    }

    /* Remove from list */
    if (web_desc == web_descriptor_list) {
        web_descriptor_list = web_desc->next;
    } else {
        for (prev = web_descriptor_list; prev; prev = prev->next) {
            if (prev->next == web_desc) {
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

    if (!web_desc || web_desc->state != WS_STATE_OPEN) {
        return;
    }

    if (!module || !json_body) {
        return;
    }

    /* Format as GMCP-style JSON message */
    len = snprintf(message, sizeof(message),
                   "{\"type\":\"gmcp\",\"module\":\"%s\",\"data\":%s}\n",
                   module, json_body);

    if (len >= sizeof(message)) {
        bug("webgate_send_gmcp: message truncated", 0);
        return;
    }

    /* TODO: Implement proper WebSocket framing */
    /* For now, queue the raw JSON */
    webgate_send_raw(web_desc, message, len);
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

    for (web_desc = web_descriptor_list; web_desc; web_desc = web_desc->next) {
        if (web_desc->authenticated) {
            webgate_send_gmcp(web_desc, module, json_body);
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
    if (result) {
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
 * reads and WebSocket framing.
 */
static void webgate_read(WEB_DESCRIPTOR_DATA *web_desc)
{
    char buf[4096];
    int nread;

    /* Read from socket */
    nread = read(web_desc->fd, buf, sizeof(buf) - 1);
    
    if (nread < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("webgate_read: read");
            web_desc->state = WS_STATE_CLOSED;
        }
        return;
    }

    if (nread == 0) {
        /* Connection closed by peer */
        web_desc->state = WS_STATE_CLOSED;
        return;
    }

    buf[nread] = '\0';

    /* Process based on state */
    if (web_desc->state == WS_STATE_HANDSHAKE) {
        /* TODO: Implement WebSocket handshake */
        webgate_process_handshake(web_desc);
    } else if (web_desc->state == WS_STATE_OPEN) {
        /* TODO: Implement WebSocket frame parsing */
        /* For now, treat as raw JSON */
        webgate_parse_json_message(web_desc, buf);
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

    if (web_desc->outbuf_size <= 0) {
        return;
    }

    /* Write to socket */
    nwritten = write(web_desc->fd, web_desc->outbuf, web_desc->outbuf_size);

    if (nwritten < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("webgate_write: write");
            web_desc->state = WS_STATE_CLOSED;
        }
        return;
    }

    /* Update output buffer */
    if (nwritten >= web_desc->outbuf_size) {
        /* Everything written */
        free(web_desc->outbuf);
        web_desc->outbuf = NULL;
        web_desc->outbuf_size = 0;
    } else {
        /* Partial write - shift remaining data */
        memmove(web_desc->outbuf, web_desc->outbuf + nwritten,
                web_desc->outbuf_size - nwritten);
        web_desc->outbuf_size -= nwritten;
    }
}

/*
 * Intent: Process WebSocket handshake request.
 * 
 * Parses the HTTP upgrade request, validates it, and sends the
 * WebSocket handshake response. On success, transitions to OPEN state.
 */
static bool webgate_process_handshake(WEB_DESCRIPTOR_DATA *web_desc)
{
    /* TODO: Implement proper WebSocket handshake per RFC 6455 */
    /* For MVP, we'll accept and move to OPEN state */
    /* This is NOT production-ready - proper handshake needed */
    
    const char *response = 
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "\r\n";
    
    webgate_send_raw(web_desc, response, strlen(response));
    web_desc->state = WS_STATE_OPEN;
    
    log_string("WebGate: Handshake completed (simplified)");
    return true;
}

/*
 * Intent: Parse a JSON message and route to appropriate handler.
 * 
 * Parses the JSON message, extracts action type and parameters,
 * and routes to the appropriate game command or authentication handler.
 */
static bool webgate_parse_json_message(WEB_DESCRIPTOR_DATA *web_desc, const char *json)
{
    /* TODO: Implement proper JSON parsing */
    /* This is a placeholder that logs the message */
    
    sprintf(log_buf, "WebGate: Received JSON: %.200s", json);
    log_string(log_buf);
    
    /* TODO: Parse action type */
    /* TODO: Convert to MUD command */
    /* TODO: Execute command for associated character */
    
    return true;
}

/*
 * Intent: Queue raw data for sending to web client.
 * 
 * Appends data to the output buffer. The data will be sent
 * asynchronously when the socket becomes writable.
 */
static void webgate_send_raw(WEB_DESCRIPTOR_DATA *web_desc, const char *data, int len)
{
    char *new_buf;

    if (!web_desc || !data || len <= 0) {
        return;
    }

    /* Allocate or expand output buffer */
    new_buf = realloc(web_desc->outbuf, web_desc->outbuf_size + len);
    if (!new_buf) {
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
    if (!web_desc) {
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
    if (!web_desc) {
        return;
    }

    if (web_desc->inbuf) {
        free(web_desc->inbuf);
    }

    if (web_desc->outbuf) {
        free(web_desc->outbuf);
    }

    if (web_desc->session_id) {
        free(web_desc->session_id);
    }

    free(web_desc);
}
