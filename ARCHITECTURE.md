# GMCP WebSocket Gateway Architecture

## Overview

This document describes the architecture of the GMCP-to-WebSocket gateway that enables web-based clients to connect to the Dragons Domain IV MUD server.

## Architecture Diagram

```
┌─────────────────┐         WebSocket          ┌──────────────────┐
│  Web Browser    │◄───────(port 8080)────────►│  WebGate Server  │
│  (React App)    │         JSON/GMCP          │  (webgate.c)     │
└─────────────────┘                             └──────────────────┘
                                                         │
                                                         │ Internal
                                                         │ Bridge
                                                         │
                                                         ▼
                                                ┌──────────────────┐
                                                │   MUD Server     │
                                                │   (comm.c)       │
                                                └──────────────────┘
                                                         │
                                                         │ Telnet
                                                         │ GMCP
                                                         ▼
                                                ┌──────────────────┐
                                                │ Traditional      │
                                                │ Telnet Clients   │
                                                └──────────────────┘
```

## Components

### 1. WebSocket Gateway (webgate.c / webgate.h)

**Purpose**: Provides a WebSocket server that bridges between web clients and the MUD server.

**Key Functions**:
- `webgate_init(port)` - Initialize WebSocket server on specified port
- `webgate_accept(fd)` - Accept new WebSocket connections
- `webgate_process(readfds, writefds)` - Process WebSocket I/O
- `webgate_send_gmcp(desc, module, json)` - Send GMCP message as JSON
- `webgate_json_to_command(json)` - Convert JSON action to MUD command

**Connection States**:
- `WS_STATE_CONNECTING` - TCP connection established
- `WS_STATE_HANDSHAKE` - WebSocket handshake in progress
- `WS_STATE_OPEN` - Connection ready for data exchange
- `WS_STATE_CLOSING` - Graceful close initiated
- `WS_STATE_CLOSED` - Connection closed

**Data Structures**:
```c
typedef struct web_descriptor_data {
    WEB_DESCRIPTOR_DATA *next;         // Linked list
    DESCRIPTOR_DATA     *mud_desc;     // Associated game character
    int                  fd;           // Socket descriptor
    ws_state_t           state;        // Connection state
    char                *inbuf;        // Input buffer
    char                *outbuf;       // Output buffer
    char                *session_id;   // Session identifier
    bool                 authenticated; // Has completed authentication
} WEB_DESCRIPTOR_DATA;
```

### 2. React Frontend (/client)

**Purpose**: Provides a modern, user-friendly interface for playing the MUD in a web browser.

**Key Components**:

#### App.jsx
- Main application component
- Manages WebSocket connection
- Routes GMCP messages to appropriate UI components
- Maintains global game state (vitals, room, messages)

#### VitalsBar.jsx
- Displays character health, mana, and movement
- Animated progress bars with color-coding
- Updates in real-time via GMCP `Char.Vitals` messages

#### Compass.jsx
- 10-direction navigation (N, NE, E, SE, S, SW, W, NW, Up, Down)
- Visual indication of available exits
- Click-to-move functionality
- Highlights available directions based on room exits

#### RoomDisplay.jsx
- Shows current room name and description
- Lists visible exits
- Updates via GMCP `Room.Info` messages

#### CombatLog.jsx
- Scrollable message history
- Color-coded message types (system, error, command, combat)
- Auto-scrolls to latest messages

#### CommandInput.jsx
- Command entry with history
- Arrow-key navigation through command history
- Disabled when disconnected

### 3. Integration with Main Server (comm.c)

The WebSocket gateway is integrated into the main game loop:

```c
// In main():
webgate_init(WEBGATE_DEFAULT_PORT);  // Initialize on port 8080

// In game_loop_unix():
webgate_register_fds(&in_set, &out_set, &maxdesc);  // Register for select()
// ... select() call ...
webgate_process(&in_set, &out_set);  // Process WebSocket I/O

// On shutdown:
webgate_shutdown();  // Clean up
```

## Message Format

### Client to Server

Commands from the web client are sent as JSON:

```json
{
  "action": "command",
  "command": "north"
}
```

The gateway converts this to the appropriate MUD command string.

### Server to Client

GMCP messages are sent to web clients as JSON:

```json
{
  "type": "gmcp",
  "module": "Char.Vitals",
  "data": {
    "hp": 100,
    "maxhp": 100,
    "mana": 50,
    "maxmana": 100,
    "move": 75,
    "maxmove": 100
  }
}
```

### Supported GMCP Modules

| Module | Purpose | Data Format |
|--------|---------|-------------|
| `Char.Vitals` | Character health/mana/movement | `{hp, maxhp, mana, maxmana, move, maxmove}` |
| `Room.Info` | Current room information | `{name, description, exits[], vnum}` |
| `Char.Stats` | Character statistics | `{str, int, wis, dex, con, hitroll, damroll, ...}` |
| `Char.Items` | Inventory updates | `{inventory: [...], equipment: [...]}` |
| `Char.Enemies` | Combat opponents | `{enemies: [{name, hp, maxhp, level}, ...]}` |

## Security Considerations

### Current State (MVP)
⚠️ **WARNING**: The current implementation is an MVP/proof-of-concept with simplified security:

1. **WebSocket Handshake**: Simplified version, not RFC 6455 compliant
2. **Authentication**: Not yet implemented
3. **Input Validation**: Minimal validation on incoming commands
4. **Session Management**: Basic session IDs without cryptographic security

### Recommended Improvements

1. **Proper WebSocket Handshake**:
   - Implement full RFC 6455 WebSocket protocol
   - Validate Sec-WebSocket-Key and generate proper accept hash
   - Support WebSocket protocol versioning

2. **Authentication**:
   - Implement login flow via WebSocket
   - Use session tokens (JWT or similar)
   - Link web descriptors to authenticated MUD characters

3. **Input Validation**:
   - Sanitize all incoming commands
   - Rate limiting on command submission
   - Command injection prevention

4. **Transport Security**:
   - Enable WSS (WebSocket Secure) for production
   - Certificate management
   - TLS configuration

5. **Session Management**:
   - Cryptographically secure session IDs
   - Session expiration and renewal
   - Session hijacking prevention

## Performance Considerations

### Non-Blocking I/O
All WebSocket operations are non-blocking and integrate with the main game loop's select() multiplexing. This ensures:
- No blocking of the game loop
- Efficient handling of multiple connections
- Low latency for all clients (telnet and web)

### Buffer Management
- Input and output buffers dynamically allocated
- Automatic buffer expansion when needed
- Maximum message size: 32KB (configurable)

### Connection Limits
Currently no explicit limit on WebSocket connections. For production:
- Set reasonable per-IP connection limits
- Global connection limit based on server capacity
- Memory usage monitoring

## Development Workflow

### Starting the Development Environment

1. **Start the MUD Server** (with WebSocket gateway):
```bash
cd /home/runner/work/dd4/dd4/server/src
make clean && make
cd ../area
../src/dd4
```

2. **Start the React Development Server**:
```bash
cd /home/runner/work/dd4/dd4/client
npm install
npm run dev
```

3. **Access the Web Client**:
Open browser to `http://localhost:3000`

### Testing WebSocket Connection

You can test the WebSocket connection with a simple client:

```javascript
const ws = new WebSocket('ws://localhost:8080');

ws.onopen = () => {
  console.log('Connected');
  ws.send(JSON.stringify({
    action: 'command',
    command: 'look'
  }));
};

ws.onmessage = (event) => {
  console.log('Received:', event.data);
};
```

## Build and Deployment

### Building the React Frontend

```bash
cd client
npm run build
```

This creates an optimized production build in `client/dist/`.

### Docker Integration

The Dockerfile needs to be updated to:
1. Build the React frontend
2. Copy static files to a web-accessible location
3. Serve static files via the WebSocket gateway or nginx

### Production Deployment

For production deployment:
1. Build React app for production
2. Configure WebSocket gateway to serve static files
3. Set up reverse proxy (nginx) for WSS and HTTPS
4. Configure firewall rules (port 8080 for WebSocket)
5. Set up SSL certificates
6. Configure environment-specific settings

## Future Enhancements

### Phase 1 Improvements
- [ ] Implement RFC 6455 compliant WebSocket handshake
- [ ] Add JSON parsing library (cJSON or jsmn)
- [ ] Implement authentication flow
- [ ] Link web descriptors to MUD characters

### Phase 2 Features
- [ ] Inventory management UI
- [ ] Character stats panel
- [ ] Equipment display
- [ ] Map visualization
- [ ] Sound/music integration (Client.Media)

### Phase 3 Advanced Features
- [ ] Mobile-responsive layout
- [ ] Touch controls
- [ ] Offline mode with sync
- [ ] Push notifications
- [ ] Progressive Web App (PWA)

### Phase 4 Administration
- [ ] Admin panel for web client management
- [ ] Analytics and monitoring
- [ ] A/B testing framework
- [ ] Feature flags

## Known Issues

1. **Build Error**: Multiple definition errors in `merc.h` (pre-existing issue)
   - Workaround: Use `LINK=-static` flag in Docker build
   - Does not affect WebSocket gateway code

2. **WebSocket Handshake**: Simplified for MVP
   - Full RFC 6455 implementation needed for production

3. **Authentication**: Not yet implemented
   - Web clients cannot authenticate as game characters

4. **Command Parsing**: Basic implementation
   - Needs proper JSON parsing library integration

## Contributing

When adding new features:
1. Follow existing code style and commenting conventions
2. Add comprehensive intent comments (see project instructions)
3. Test with both telnet and web clients
4. Update this documentation
5. Add tests if applicable

## References

- [RFC 6455 - The WebSocket Protocol](https://tools.ietf.org/html/rfc6455)
- [GMCP Specification](https://www.gammon.com.au/gmcp)
- [React Documentation](https://react.dev/)
- [Vite Documentation](https://vitejs.dev/)
