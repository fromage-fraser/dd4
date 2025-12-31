import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import Compass from './components/Compass';
import RoomDisplay from './components/RoomDisplay';
import VitalsBar from './components/VitalsBar';
import CombatLog from './components/CombatLog';
import CommandInput from './components/CommandInput';

/**
 * Main application component for DD4 Web Client
 * 
 * Manages WebSocket connection to the GMCP gateway and routes
 * messages to appropriate UI components. Handles character state
 * and real-time updates from the MUD server.
 */
function App() {
  const [connected, setConnected] = useState(false);
  const [vitals, setVitals] = useState({ hp: 100, maxhp: 100, mana: 100, maxmana: 100, move: 100, maxmove: 100 });
  const [room, setRoom] = useState({ name: 'Unknown', description: '', exits: [] });
  const [messages, setMessages] = useState([]);
  const ws = useRef(null);

  // Connect to WebSocket gateway
  useEffect(() => {
    const connectWebSocket = () => {
      // Connect to the WebSocket gateway
      // In production, this should be configurable
      const wsUrl = 'ws://localhost:8080';
      
      try {
        ws.current = new WebSocket(wsUrl);

        ws.current.onopen = () => {
          console.log('Connected to DD4 WebSocket Gateway');
          setConnected(true);
          addMessage('Connected to Dragons Domain IV', 'system');
        };

        ws.current.onmessage = (event) => {
          try {
            const message = JSON.parse(event.data);
            handleGMCPMessage(message);
          } catch (err) {
            console.error('Failed to parse message:', err);
            addMessage('Error: ' + event.data, 'error');
          }
        };

        ws.current.onerror = (error) => {
          console.error('WebSocket error:', error);
          addMessage('Connection error occurred', 'error');
        };

        ws.current.onclose = () => {
          console.log('Disconnected from WebSocket');
          setConnected(false);
          addMessage('Disconnected from server', 'system');
          
          // Attempt to reconnect after 5 seconds
          setTimeout(connectWebSocket, 5000);
        };
      } catch (err) {
        console.error('Failed to create WebSocket:', err);
        setTimeout(connectWebSocket, 5000);
      }
    };

    connectWebSocket();

    // Cleanup on unmount
    return () => {
      if (ws.current) {
        ws.current.close();
      }
    };
  }, []);

  /**
   * Handle incoming GMCP messages from the server
   * Routes messages to appropriate state handlers based on module type
   */
  const handleGMCPMessage = (message) => {
    if (message.type !== 'gmcp') {
      addMessage(message.toString(), 'info');
      return;
    }

    const { module, data } = message;

    switch (module) {
      case 'Char.Vitals':
        setVitals(data);
        break;
      
      case 'Room.Info':
        setRoom(data);
        addMessage(`You are in: ${data.name}`, 'room');
        break;
      
      case 'Comm.Channel':
        // Handle channel messages (system, chat, game output, etc)
        if (data.channel === 'system') {
          addMessage(data.message, 'system');
        } else if (data.channel === 'game') {
          addMessage(data.message, 'normal');
        } else {
          addMessage(`[${data.channel}] ${data.message}`, 'info');
        }
        break;
      
      case 'Comm.Prompt':
        // Handle prompts (Name:, Password:, etc)
        addMessage(data.prompt, 'prompt');
        break;
      
      case 'Char.Stats':
        // Handle character stats update
        console.log('Stats update:', data);
        break;
      
      case 'Char.Items':
        // Handle inventory update
        console.log('Inventory update:', data);
        break;
      
      default:
        console.log('Unknown GMCP module:', module, data);
    }
  };

  /**
   * Add a message to the combat/message log
   */
  const addMessage = (text, type = 'normal') => {
    setMessages(prev => [...prev, { text, type, timestamp: Date.now() }]);
  };

  /**
   * Send a command to the MUD server via WebSocket
   */
  const sendCommand = (command) => {
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      addMessage('Not connected to server', 'error');
      return;
    }

    // Format command as JSON for the gateway
    const message = JSON.stringify({
      command: command
    });

    ws.current.send(message);
    addMessage(`> ${command}`, 'command');
  };

  /**
   * Handle movement via compass
   */
  const handleMove = (direction) => {
    sendCommand(direction);
  };

  return (
    <div className="app">
      <header className="app-header">
        <h1>Dragons Domain IV</h1>
        <div className={`status ${connected ? 'connected' : 'disconnected'}`}>
          {connected ? '● Connected' : '○ Disconnected'}
        </div>
      </header>

      <div className="app-content">
        <div className="left-panel">
          <VitalsBar vitals={vitals} />
          <Compass exits={room.exits} onMove={handleMove} />
        </div>

        <div className="main-panel">
          <RoomDisplay room={room} />
          <CombatLog messages={messages} />
          <CommandInput onSubmit={sendCommand} connected={connected} />
        </div>

        <div className="right-panel">
          <div className="stats-panel">
            <h3>Character Stats</h3>
            <p>Stats display coming soon...</p>
          </div>
          <div className="inventory-panel">
            <h3>Inventory</h3>
            <p>Inventory display coming soon...</p>
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
