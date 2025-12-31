/*
  Module Intent: WebSocket gateway for GMCP-to-JSON bridging to web clients.

  This module implements a WebSocket server that bridges the existing GMCP protocol
  to JSON for modern web-based clients. It enables browser-based and mobile app
  gameplay by providing a REST/WebSocket API layer on top of the traditional
  telnet-based MUD server.

  Responsibilities:
  - WebSocket server lifecycle management (listen, accept, close)
  - Connection state tracking for web clients
  - Bi-directional message conversion (JSON ↔ GMCP ↔ MUD commands)
  - Session management and authentication
  - Real-time event streaming to connected clients

  Interactions:
  - Integrates with protocol.c/h for GMCP message handling
  - Uses comm.c patterns for network I/O and descriptor management
  - Interfaces with main game loop for multiplexed I/O

  Constraints:
  - Must not block the main game loop
  - WebSocket connections treated as special descriptor types
  - Must handle partial JSON message assembly
  - Thread-safety not required (single-threaded event loop model)

  Extensibility:
  - WebSocket library can be swapped by implementing the interface below
  - JSON format is version-controlled via protocol version negotiation
  - Additional message types can be added to the routing table
*/

#ifndef WEBGATE_H
#define WEBGATE_H

#include "merc.h"
#include "protocol.h"

/*
 * WebSocket gateway configuration
 */
#define WEBGATE_DEFAULT_PORT 8080
#define WEBGATE_MAX_MESSAGE (32 * 1024) /* 32KB max WebSocket message */
#define WEBGATE_PROTOCOL_VERSION "1.0"

/*
 * WebSocket connection states
 */
typedef enum
{
  WS_STATE_CONNECTING,
  WS_STATE_HANDSHAKE,
  WS_STATE_OPEN,
  WS_STATE_CLOSING,
  WS_STATE_CLOSED
} ws_state_t;

/*
 * Web client descriptor - extends the standard descriptor concept
 * for WebSocket-based clients.
 */
typedef struct web_descriptor_data WEB_DESCRIPTOR_DATA;
struct web_descriptor_data
{
  WEB_DESCRIPTOR_DATA *next; /* Next in linked list */
  DESCRIPTOR_DATA *mud_desc; /* Associated MUD descriptor (if authenticated) */
  int fd;                    /* Socket file descriptor */
  ws_state_t state;          /* WebSocket connection state */
  char *inbuf;               /* Incoming message buffer */
  int inbuf_size;            /* Current size of inbuf */
  char *outbuf;              /* Outgoing message buffer */
  int outbuf_size;           /* Current size of outbuf */
  char *session_id;          /* Session identifier */
  time_t connect_time;       /* Connection timestamp */
  time_t last_ping;          /* Last ping/pong timestamp */
  bool authenticated;        /* Has client authenticated? */
  void *ws_context;          /* WebSocket library-specific context */
};

/*
 * Intent: Initialize the WebSocket gateway server.
 *
 * Inputs: port - TCP port to listen on (typically 8080)
 * Outputs: File descriptor for the listening socket, or -1 on error
 *
 * Preconditions: Called once during server initialization
 * Postconditions: WebSocket server is listening and ready to accept connections
 *
 * Failure Behavior: Returns -1 and logs error; server continues without web gateway
 *
 * Performance: Fast initialization; non-blocking socket setup
 */
int webgate_init(int port);

/*
 * Intent: Shut down the WebSocket gateway cleanly.
 *
 * Inputs: None
 * Outputs: None
 *
 * Preconditions: webgate_init() was called successfully
 * Postconditions: All web clients disconnected; listening socket closed
 *
 * Failure Behavior: Best-effort cleanup; logs any errors
 */
void webgate_shutdown(void);

/*
 * Intent: Process pending WebSocket events (new connections, messages, disconnects).
 *
 * Inputs:
 *   - fd_set *readfds  - Set of file descriptors ready for reading
 *   - fd_set *writefds - Set of file descriptors ready for writing
 *
 * Outputs: None (side effect: messages processed, state updated)
 *
 * Preconditions: Called from main game loop on each iteration
 * Postconditions: All pending WebSocket I/O processed without blocking
 *
 * Failure Behavior: Individual connection errors handled gracefully;
 *                   bad connections are closed and cleaned up
 *
 * Performance: Non-blocking; processes only ready descriptors
 */
void webgate_process(fd_set *readfds, fd_set *writefds);

/*
 * Intent: Accept a new WebSocket connection.
 *
 * Inputs: listen_fd - Listening socket file descriptor
 * Outputs: New WEB_DESCRIPTOR_DATA or NULL on error
 *
 * Preconditions: listen_fd has a pending connection (select indicated readiness)
 * Postconditions: New web client added to descriptor list; handshake pending
 *
 * Failure Behavior: Returns NULL; logs error; no state corruption
 */
WEB_DESCRIPTOR_DATA *webgate_accept(int listen_fd);

/*
 * Intent: Close and clean up a web client connection.
 *
 * Inputs: web_desc - Web descriptor to close
 * Outputs: None
 *
 * Preconditions: web_desc is in the active descriptor list
 * Postconditions: Connection closed; memory freed; removed from list
 *
 * Failure Behavior: Best-effort cleanup; ensures list integrity
 */
void webgate_close(WEB_DESCRIPTOR_DATA *web_desc);

/*
 * Intent: Send a GMCP-style message as JSON to a web client.
 *
 * Inputs:
 *   - web_desc - Target web client
 *   - module - GMCP module name (e.g., "Char.Vitals")
 *   - json_body - JSON object body (already formatted)
 *
 * Outputs: None (message queued for transmission)
 *
 * Preconditions: web_desc is authenticated and in OPEN state
 * Postconditions: Message added to outgoing queue
 *
 * Failure Behavior: Silently drops message if client disconnected;
 *                   logs error if buffer overflow
 *
 * Performance: Fast enqueue; actual send happens in webgate_process()
 *
 * Example:
 *   webgate_send_gmcp(web_desc, "Char.Vitals", "{\"hp\":100,\"maxhp\":100}");
 */
void webgate_send_gmcp(WEB_DESCRIPTOR_DATA *web_desc, const char *module, const char *json_body);

/*
 * Intent: Send room information to a web client via GMCP Room.Info.
 *
 * Inputs:
 *   - web_desc - Target web client connection
 *   - room - ROOM_INDEX_DATA pointer containing room details
 *
 * Outputs: None (GMCP message queued)
 *
 * Preconditions: web_desc is authenticated; room is valid
 * Postconditions: Room.Info GMCP message sent with name, description, exits, vnum
 *
 * Failure Behavior: Silently returns if arguments are NULL
 *
 * Notes: Enables client-side compass and room display updates
 */
void webgate_send_room_info(WEB_DESCRIPTOR_DATA *web_desc, ROOM_INDEX_DATA *room);

/*
 * Intent: Broadcast a GMCP message to all authenticated web clients.
 *
 * Inputs:
 *   - module - GMCP module name
 *   - json_body - JSON object body
 *
 * Outputs: None (messages queued for all clients)
 *
 * Preconditions: None (handles empty client list gracefully)
 * Postconditions: Message queued for all authenticated web clients
 *
 * Failure Behavior: Per-client errors handled individually
 *
 * Notes: Use sparingly; prefer targeted updates when possible
 */
void webgate_broadcast(const char *module, const char *json_body);

/*
 * Intent: Send WebSocket PING frame to keep connection alive.
 *
 * Inputs: web_desc - Web client to send ping to
 * Outputs: None (PING frame queued)
 *
 * Preconditions: web_desc is in WS_STATE_OPEN
 * Postconditions: PING frame sent; last_ping timestamp updated
 *
 * Failure Behavior: Silently returns if connection not open
 *
 * Notes: RFC-6455 §5.5.2 - Used for keep-alive and connection health checks
 */
void webgate_send_ping(WEB_DESCRIPTOR_DATA *web_desc);

/*
 * Intent: Perform periodic WebSocket connection maintenance.
 *
 * Inputs: None
 * Outputs: None (may close unresponsive connections)
 *
 * Preconditions: Called from main game loop heartbeat
 * Postconditions: PINGs sent to idle connections; timeouts closed
 *
 * Failure Behavior: Per-connection errors handled individually
 *
 * Notes: Should be called once per game loop iteration (every 1-2 seconds)
 */
void webgate_pulse(void);

/*
 * Intent: Register file descriptors with select() for WebSocket I/O.
 *
 * Inputs:
 *   - fd_set *readfds - Set to add readable descriptors to
 *   - fd_set *writefds - Set to add writable descriptors to
 *   - int *maxfd - Current maximum file descriptor (updated if necessary)
 *
 * Outputs: Updated fd_sets and maxfd
 *
 * Preconditions: Called before select() in main loop
 * Postconditions: All active WebSocket descriptors registered
 *
 * Failure Behavior: N/A (no failure modes)
 */
void webgate_register_fds(fd_set *readfds, fd_set *writefds, int *maxfd);

/*
 * Intent: Convert incoming JSON action to MUD command string.
 *
 * Inputs:
 *   - json_msg - Incoming JSON message from web client
 *
 * Outputs: Dynamically allocated command string, or NULL on parse error
 *
 * Preconditions: json_msg is well-formed JSON
 * Postconditions: Caller must free returned string
 *
 * Failure Behavior: Returns NULL and logs parse error
 *
 * Example input:  {"action":"move","direction":"north"}
 * Example output: "north"
 *
 * Example input:  {"action":"get","target":"sword"}
 * Example output: "get sword"
 */
char *webgate_json_to_command(const char *json_msg);

/*
 * Global state
 */
extern WEB_DESCRIPTOR_DATA *web_descriptor_list;
extern int webgate_listen_fd;

#endif /* WEBGATE_H */
