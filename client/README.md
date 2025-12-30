# DD4 Web Client

Modern web-based client for Dragons Domain IV MUD using React and GMCP over WebSocket.

## Features

- **Real-time GMCP Integration**: Receives structured game data via WebSocket
- **Visual Navigation**: Click-to-move compass with available exits
- **Live Character Vitals**: Animated HP/Mana/Movement bars
- **Room Display**: Shows current location and description
- **Combat Log**: Scrollable message history with color-coded output
- **Command Input**: Full command entry with history navigation (arrow keys)

## Prerequisites

- Node.js 16+ and npm
- DD4 MUD server with WebSocket gateway running on port 8080

## Installation

```bash
cd client
npm install
```

## Development

Start the development server with hot reload:

```bash
npm run dev
```

The client will be available at `http://localhost:3000`

The dev server proxies WebSocket connections to `ws://localhost:8080`

## Building for Production

Create an optimized production build:

```bash
npm run build
```

The built files will be in the `dist/` directory.

Preview the production build:

```bash
npm run preview
```

## Project Structure

```
client/
├── public/          # Static assets
├── src/
│   ├── components/  # React components
│   │   ├── Compass.jsx         # Directional navigation
│   │   ├── RoomDisplay.jsx     # Current room info
│   │   ├── VitalsBar.jsx       # HP/Mana/Move bars
│   │   ├── CombatLog.jsx       # Message history
│   │   └── CommandInput.jsx    # Command entry
│   ├── App.jsx      # Main application
│   ├── App.css      # Application styles
│   ├── main.jsx     # Entry point
│   └── index.css    # Global styles
├── index.html       # HTML template
├── vite.config.js   # Vite configuration
└── package.json     # Dependencies
```

## GMCP Message Format

The client expects JSON messages in this format:

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

Supported GMCP modules:
- `Char.Vitals` - Character health/mana/movement
- `Room.Info` - Current room information
- `Char.Stats` - Character statistics
- `Char.Items` - Inventory updates

## Command Format

Commands are sent to the server as JSON:

```json
{
  "action": "command",
  "command": "north"
}
```

## Configuration

WebSocket connection URL can be configured in `src/App.jsx`:

```javascript
const wsUrl = 'ws://localhost:8080';
```

For production, update this to your server's WebSocket endpoint.

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 14+

WebSocket support is required.

## Development Notes

- The client uses React 18 with functional components and hooks
- Vite provides fast HMR for development
- All components are documented with JSDoc comments
- CSS is scoped per-component for maintainability

## Future Enhancements

- Character stats panel integration
- Inventory management UI
- Equipment display
- Map visualization
- Sound/music player (Client.Media)
- Mobile-responsive layout
- Touch controls for mobile
- Authentication flow
