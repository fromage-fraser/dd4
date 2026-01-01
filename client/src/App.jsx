import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import Compass from './components/Compass';
import RoomDisplay from './components/RoomDisplay';
import VitalsBar from './components/VitalsBar';
import CombatLog from './components/CombatLog';
import CommandInput from './components/CommandInput';
import QuickActions from './components/QuickActions';
import CharacterInfo from './components/CharacterInfo';
import RoomContents from './components/RoomContents';
import SkillBar from './components/SkillBar';
import SkillAssign from './components/SkillAssign';

/**
 * Main application component for DD4 Web Client
 * 
 * Manages WebSocket connection to the GMCP gateway and routes
 * messages to appropriate UI components. Handles character state
 * and real-time updates from the MUD server.
 */
function App() {
  const [connected, setConnected] = useState(false);
  const [reconnecting, setReconnecting] = useState(false);
  const [reconnectAttempt, setReconnectAttempt] = useState(0);
  const [vitals, setVitals] = useState({ hp: 100, maxhp: 100, mana: 100, maxmana: 100, move: 100, maxmove: 100 });
  const [status, setStatus] = useState(null);
  const [room, setRoom] = useState({ name: 'Unknown', description: '', exits: [], items: [], npcs: [] });
  const [messages, setMessages] = useState([]);
  const [skills, setSkills] = useState([]);
  const [assignedSkills, setAssignedSkills] = useState(Array(8).fill(null));
  const [openers, setOpeners] = useState([null, null]);
  const [showSkillAssign, setShowSkillAssign] = useState(false);
  const ws = useRef(null);
  const reconnectTimeoutRef = useRef(null);
  const reconnectAttemptRef = useRef(0); // Use ref to track attempts for closure issues

  // Load saved skill configuration from localStorage
  useEffect(() => {
    const savedAssigned = localStorage.getItem('dd4_assigned_skills');
    const savedOpeners = localStorage.getItem('dd4_openers');
    
    if (savedAssigned) {
      try {
        setAssignedSkills(JSON.parse(savedAssigned));
      } catch (e) {
        console.error('Failed to parse saved skill assignments:', e);
      }
    }
    
    if (savedOpeners) {
      try {
        setOpeners(JSON.parse(savedOpeners));
      } catch (e) {
        console.error('Failed to parse saved openers:', e);
      }
    }
  }, []);

  // Connect to WebSocket gateway with reconnection logic
  useEffect(() => {
    const connectWebSocket = () => {
      // Connect to the WebSocket gateway
      // In production, this should be configurable
      const wsUrl = 'ws://localhost:8080';
      
      try {
        console.log('Connecting to WebSocket...');
        ws.current = new WebSocket(wsUrl);

        ws.current.onopen = () => {
          console.log('Connected to DD4 WebSocket Gateway');
          setConnected(true);
          setReconnecting(false);
          setReconnectAttempt(0);
          reconnectAttemptRef.current = 0; // Reset ref
          addMessage('Connected to Dragons Domain IV', 'system');
          
          // Note: Client doesn't need to send PINGs
          // Browser automatically responds to server PINGs with PONGs per RFC-6455
          // Server sends PING every 30s and closes connection if no PONG after 90s
        };

        ws.current.onmessage = (event) => {
          try {
            // Handle binary frames (pong responses)
            if (event.data instanceof Blob) {
              console.log('Received binary frame (likely pong)');
              return;
            }
            
            console.log('Raw WebSocket message:', event.data);
            const message = JSON.parse(event.data);
            console.log('Parsed GMCP message:', message);
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

        ws.current.onclose = (event) => {
          console.log('Disconnected from WebSocket', event);
          setConnected(false);
          
          // Attempt to reconnect with exponential backoff
          // Delay: 1s, 2s, 4s, 8s, 16s, max 30s
          const currentAttempt = reconnectAttemptRef.current;
          const delay = Math.min(1000 * Math.pow(2, currentAttempt), 30000);
          setReconnecting(true);
          setReconnectAttempt(currentAttempt + 1);
          reconnectAttemptRef.current = currentAttempt + 1; // Update ref
          
          const delaySeconds = (delay / 1000).toFixed(0);
          addMessage(`Disconnected. Reconnecting in ${delaySeconds}s... (attempt ${currentAttempt + 1})`, 'system');
          
          if (reconnectTimeoutRef.current) {
            clearTimeout(reconnectTimeoutRef.current);
          }
          
          reconnectTimeoutRef.current = setTimeout(() => {
            console.log(`Reconnection attempt ${reconnectAttemptRef.current}`);
            connectWebSocket();
          }, delay);
        };
      } catch (err) {
        console.error('Failed to create WebSocket:', err);
        setReconnecting(true);
        
        const currentAttempt = reconnectAttemptRef.current;
        const delay = Math.min(1000 * Math.pow(2, currentAttempt), 30000);
        reconnectAttemptRef.current = currentAttempt + 1;
        setReconnectAttempt(currentAttempt + 1);
        
        reconnectTimeoutRef.current = setTimeout(() => {
          connectWebSocket();
        }, delay);
      }
    };

    connectWebSocket();

    // Cleanup on unmount
    return () => {
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current);
      }
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
      
      case 'Char.Status':
        setStatus(data);
        break;
      
      case 'Room.Info':
        setRoom(data);
        addMessage(`You are in: ${data.name}`, 'room');
        break;
      
      case 'Char.Skills':
        // Handle skills update
        console.log('Char.Skills received:', data);
        console.log('Skills count:', data.skills?.length);
        if (data.skills && data.skills.length > 0) {
          console.log('First 5 skills:', data.skills.slice(0, 5));
          console.log('Opener skills:', data.skills.filter(s => s.opener));
          setSkills(data.skills);
        }
        break;
      
      case 'Comm.Channel':
        // Handle channel messages (system, chat, game output, etc)
        console.log('Comm.Channel received:', data);
        console.log('Message text:', data.message);
        console.log('Message text (escaped):', JSON.stringify(data.message));
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

  /**
   * Handle using a skill from the skill bar
   */
  const handleUseSkill = (skill) => {
    console.log('Using skill:', skill.name);
    // Skills with mana are typically spells, use 'cast'
    // Skills without mana are physical abilities, use skill name directly
    if (skill.type === 'spell') {
      sendCommand(`cast '${skill.name}'`);
    } else {
      sendCommand(skill.name);
    }
  };

  /**
   * Save skill configuration to localStorage
   */
  const handleSaveSkillConfig = (newAssigned, newOpeners) => {
    setAssignedSkills(newAssigned);
    setOpeners(newOpeners);
    
    localStorage.setItem('dd4_assigned_skills', JSON.stringify(newAssigned));
    localStorage.setItem('dd4_openers', JSON.stringify(newOpeners));
    
    console.log('Skill configuration saved:', { assigned: newAssigned, openers: newOpeners });
  };

  return (
    <div className="app">
      <header className="app-header">
        <h1>Dragons Domain IV</h1>
        <div className={`status ${connected ? 'connected' : reconnecting ? 'reconnecting' : 'disconnected'}`}>
          {connected ? '● Connected' : reconnecting ? '○ Reconnecting...' : '○ Disconnected'}
        </div>
      </header>

      <div className="app-content">
        <div className="left-panel">
          <VitalsBar vitals={vitals} />
          <QuickActions onCommand={sendCommand} connected={connected} />
          <Compass exits={room.exits} onMove={handleMove} />
        </div>

        <div className="main-panel">
          <RoomDisplay room={room} />
          <CombatLog messages={messages} />
          <CommandInput onSubmit={sendCommand} connected={connected} />
        </div>

        <div className="right-panel">
          <CharacterInfo vitals={vitals} status={status} />
          <RoomContents 
            items={room.items} 
            npcs={room.npcs} 
            onCommand={sendCommand} 
            connected={connected}
            skills={skills}
            openers={openers}
          />
          <SkillBar
            skills={skills}
            assignedSkills={assignedSkills}
            onUseSkill={handleUseSkill}
            onAssignClick={() => setShowSkillAssign(true)}
            connected={connected}
          />
        </div>
      </div>

      {showSkillAssign && (
        <SkillAssign
          skills={skills}
          assignedSkills={assignedSkills}
          openers={openers}
          onSave={handleSaveSkillConfig}
          onClose={() => setShowSkillAssign(false)}
        />
      )}
    </div>
  );
}

export default App;
