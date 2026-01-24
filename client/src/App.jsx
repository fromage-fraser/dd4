import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import Compass from './components/Compass';
import RoomDisplay from './components/RoomDisplay';
import CharacterInfo from './components/CharacterInfo';
import CombatLog from './components/CombatLog';
import CommandInput from './components/CommandInput';
import QuickActions from './components/QuickActions';
import RoomContents from './components/RoomContents';
import EnemyStatus from './components/EnemyStatus';
import SkillBar from './components/SkillBar';
import SkillAssign from './components/SkillAssign';
import AliasBar from './components/AliasBar';
import AliasAssign from './components/AliasAssign';
import CharacterSheet from './components/CharacterSheet';
import ShopModal from './components/ShopModal';
import IdentifyModal from './components/IdentifyModal';
import HealerModal from './components/HealerModal';
import PracticeModal from './components/PracticeModal';
import MapModal from './components/MapModal';
import SpellBookModal from './components/SpellBookModal';
import HelpModal from './components/HelpModal';
import SkillTreeModal from './components/SkillTreeModal';

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
  const [characterName, setCharacterName] = useState(null); // Character name for localStorage keys
  const [isFighting, setIsFighting] = useState(false); // Combat state
  const [opponent, setOpponent] = useState(null); // Current combat opponent name
  const [enemies, setEnemies] = useState([]); // Full array of combat enemies with HP/level
  const [isDisarmed, setIsDisarmed] = useState(false); // Disarmed state during combat
  const [disarmedWeapons, setDisarmedWeapons] = useState([]); // Array of weapon objects that were disarmed
  const previousEquipment = useRef(null); // Track previous equipment state for disarm detection
  const [room, setRoom] = useState({ name: 'Unknown', description: '', exits: [], items: [], npcs: [] });
  const [messages, setMessages] = useState([]);
  const [skills, setSkills] = useState([]);
  const [assignedSkills, setAssignedSkills] = useState(Array(8).fill(null));
  const [openers, setOpeners] = useState([null, null]);
  const [showSkillAssign, setShowSkillAssign] = useState(false);
  const [assignedAliases, setAssignedAliases] = useState(Array(8).fill(null));
  const [showAliasModal, setShowAliasModal] = useState(false);
  const [showCharacterSheet, setShowCharacterSheet] = useState(false);
  const [inventory, setInventory] = useState([]);
  const [equipment, setEquipment] = useState(null);
  const [shopData, setShopData] = useState(null); // { shopkeeper, items }
  const [identifierData, setIdentifierData] = useState(null); // { identifier }
  const [shopMessage, setShopMessage] = useState(null);
  const [lastUserCommandTs, setLastUserCommandTs] = useState(0);
  const shopOpenRef = useRef(false);

  // Ensure identical consecutive shop messages still surface in the UI
  // Pushes a message object with a timestamp so identical text always updates
  const pushShopMessage = (msg) => {
    if (!msg) return;
    setShopMessage({ text: msg, ts: Date.now() });
  };
  const [healerData, setHealerData] = useState(null); // { healer, services }
  const [showPracticeModal, setShowPracticeModal] = useState(false);
  const [showSpellBook, setShowSpellBook] = useState(false);
  const [showSkillTree, setShowSkillTree] = useState(false);
  const [skillTree, setSkillTree] = useState(null);
  const [itemDetails, setItemDetails] = useState({}); // Store detailed item info keyed by item name
  const [mapsData, setMapsData] = useState(null); // All available maps from maps.json
  const [currentMap, setCurrentMap] = useState(null); // Current area's map data
  const [showMapModal, setShowMapModal] = useState(false); // Store detailed item info keyed by item name
  const [helpContent, setHelpContent] = useState(null);
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

  // Load character-specific aliases when character name is available
  useEffect(() => {
    if (!characterName) return;
    
    const aliasKey = `dd4_aliases_${characterName}`;
    const savedAliases = localStorage.getItem(aliasKey);
    
    if (savedAliases) {
      try {
        setAssignedAliases(JSON.parse(savedAliases));
      } catch (e) {
        console.error('Failed to parse saved aliases:', e);
      }
    } else {
      // Reset to empty when switching characters
      setAssignedAliases(Array(8).fill(null));
    }
  }, [characterName]);

  // Load maps.json on mount
  useEffect(() => {
    fetch('/maps.json')
      .then(response => response.json())
      .then(data => {
        console.log('Loaded maps.json:', Object.keys(data).length, 'maps');
        setMapsData(data);
      })
      .catch(error => {
        console.error('Failed to load maps.json:', error);
      });
  }, []);

  // Match current room to map when mapsData loads
  useEffect(() => {
    if (mapsData && room.areaName) {
      console.log('Checking map for current area:', room.areaName);
      const areaNameLower = room.areaName.toLowerCase().trim();
      let matchedMap = null;
      
      for (const [filename, mapInfo] of Object.entries(mapsData)) {
        const mapNameLower = mapInfo.name.toLowerCase().trim();
        if (mapNameLower === areaNameLower) {
          console.log('Found matching map:', filename, mapInfo);
          matchedMap = mapInfo;
          break;
        }
      }
      
      setCurrentMap(matchedMap);
      
      if (!matchedMap) {
        console.log('No map found for area:', room.areaName);
      }
    }
  }, [mapsData, room.areaName]);

  // Keep a ref indicating whether the shop modal is currently open
  useEffect(() => {
    shopOpenRef.current = !!shopData;
  }, [shopData]);

  /*
    Intent: Detect when player is disarmed during combat by tracking equipment changes.
    Compares current equipment state with previous state to identify weapon removal.
    Inputs: equipment - current equipment GMCP data, isFighting - combat state
    Outputs: Updates isDisarmed state and stores disarmed weapon info
    Preconditions: Equipment data available from GMCP Char.Equipment messages
    Postconditions: isDisarmed triggers recovery button display, disarmedWeapons stores weapon keywords
  */
  useEffect(() => {
    if (!equipment) {
      previousEquipment.current = equipment;
      return;
    }

    // Only track disarms during combat
    if (previousEquipment.current && isFighting) {
      const weapons = [];
      
      // Check wield slot
      if (previousEquipment.current.wield && !equipment.wield) {
        weapons.push(previousEquipment.current.wield);
      }
      
      // Check dual wield slot
      if (previousEquipment.current.dual && !equipment.dual) {
        weapons.push(previousEquipment.current.dual);
      }
      
      // If weapons were lost during combat, mark as disarmed
      if (weapons.length > 0) {
        console.log('Disarmed! Lost weapons:', weapons);
        setIsDisarmed(true);
        setDisarmedWeapons(weapons);
      }
    }
    
    // Clear disarm state when weapons are re-equipped
    if (isDisarmed && equipment.wield) {
      console.log('Weapon re-equipped, clearing disarm state');
      setIsDisarmed(false);
      setDisarmedWeapons([]);
    }
    
    previousEquipment.current = equipment;
  }, [equipment, isFighting, isDisarmed]);

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
        console.log('Char.Status received:', data);
        setStatus(data);
        // Extract character name for target memory localStorage keys
        if (data.name && data.name !== characterName) {
          console.log('Character name set:', data.name);
          setCharacterName(data.name);
        }
        break;
      
      case 'Char.Enemies':
        // Handle combat state tracking with full enemy array
        console.log('Char.Enemies received:', data);
        // data is directly the enemies array from server
        if (Array.isArray(data) && data.length > 0) {
          // In combat - store full enemies array
          setEnemies(data);
          setIsFighting(true);
          setOpponent(data[0].name);
          console.log('Combat active with enemies:', data);
        } else {
          // Combat ended - clear enemies
          setEnemies([]);
          setIsFighting(false);
          setOpponent(null);
          console.log('Combat ended');
        }
        break;
      
      case 'Room.Info':
        setRoom(data);
        addMessage(`You are in: ${data.name}`, 'room');
        
        // Match area name to map data
        console.log('Room.Info received, areaName:', data.areaName, 'mapsData loaded:', mapsData !== null);
        if (data.areaName && mapsData) {
          console.log('Area name from server:', data.areaName);
          
          // Find matching map by comparing area names (case-insensitive, trimmed)
          const areaNameLower = data.areaName.toLowerCase().trim();
          let matchedMap = null;
          
          for (const [filename, mapInfo] of Object.entries(mapsData)) {
            const mapNameLower = mapInfo.name.toLowerCase().trim();
            if (mapNameLower === areaNameLower) {
              console.log('Found matching map:', filename, mapInfo);
              matchedMap = mapInfo;
              break;
            }
          }
          
          setCurrentMap(matchedMap);
          
          if (!matchedMap) {
            console.log('No map found for area:', data.areaName);
          }
        }
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
      
      case 'Char.SkillTree':
        // Handle skill tree update (prerequisite visualization)
        // New simplified format: {skills:[{id,name,learned,prerequisites}]}
        console.log('Char.SkillTree received:', data);
        console.log('Raw skills array:', data.skills);
        console.log('Total skills for class:', data.skills?.length);
        
        // Split into available (learned > 0) and locked (learned == 0)
        if (data.skills && Array.isArray(data.skills)) {
          const available = data.skills.filter(s => s.learned > 0);
          const locked = data.skills.filter(s => s.learned === 0);
          console.log('Available skills:', available.length, available.slice(0, 3));
          console.log('Locked skills:', locked.length, locked.slice(0, 3));
          setSkillTree({available, locked});
          console.log('SkillTree state updated');
        } else {
          console.error('Invalid skills data structure:', data);
        }
        break;
      
      case 'Char.Inventory':
        // Handle inventory update
        console.log('Char.Inventory received:', data);
        console.log('Inventory count:', data.items?.length);
        if (data.items) {
          setInventory(data.items);
        }
        break;
      
      case 'Char.Equipment':
        // Handle equipment update
        console.log('Char.Equipment received:', data);
        if (data.equipment) {
          setEquipment(data.equipment);
        }
        break;
      
      case 'Shop.Inventory':
        // Handle shop inventory
        console.log('Shop.Inventory received:', data);
        // Always set shop data so the modal appears when the server sends it.
        // Surface the optional message if the user acted recently or modal already open.
        setShopData(data);
        if (data.message) {
          if (shopOpenRef.current || (Date.now() - lastUserCommandTs) < 5000) {
            pushShopMessage(data.message);
          } else {
            console.log('Received Shop.Inventory with message, but suppressing notification (no recent user command)');
          }
        }
        break;
      
      case 'Identifier.Inventory':
        // Handle identifier inventory
        console.log('Identifier.Inventory received:', data);
        setIdentifierData(data);
        break;
      
      case 'Healer.Services':
        // Handle healer services
        console.log('Healer.Services received:', data);
        setHealerData(data);
        break;
      
      case 'Comm.Channel':
        // Handle channel messages (system, chat, game output, etc)
        console.log('Comm.Channel received:', data);
        console.log('Message text:', data.message);
        console.log('Message text (escaped):', JSON.stringify(data.message));
        
        // Check for shopkeeper messages that should be shown in GUI
        if (data.channel === 'game' && data.message) {
          const msg = data.message.trim();
          const msgLower = msg.toLowerCase();
          
          // Hybrid disarm detection: parse combat messages for instant button feedback
          // Patterns: "DISARMS you", "weapon slips from your hands", "breaks your wrist"
          if (isFighting && (msgLower.includes('disarm') && msgLower.includes('you')) ||
              msgLower.includes('weapon slips from your hands') ||
              msgLower.includes('weapon to fall') ||
              msgLower.includes('breaks your wrist')) {
            console.log('Disarm detected via message parsing:', msg);
            // Mark as disarmed immediately; equipment update will provide weapon details
            // Store current weapons before equipment update arrives
            const weapons = [];
            if (equipment?.wield) weapons.push(equipment.wield);
            if (equipment?.dual) weapons.push(equipment.dual);
            if (weapons.length > 0) {
              setIsDisarmed(true);
              setDisarmedWeapons(weapons);
            }
          }

          // Detect help output and surface it in the Practice modal
          if (msg.match(/help for/i) || msg.includes('Syntax:') || msg.match(/^Help for/i)) {
            const clean = msg.replace(/\u001b\[[0-9;]*m/g, '').replace(/<[0-9]+>/g, '');
            console.log('Help text received:', clean);
            setHelpContent(clean);
          }
          
          // Handle "uninterested" messages for items that can't be sold
          if (msg.includes('looks uninterested in') || msg.includes("won't buy that") || 
              msg.includes("can't sell") || msg.includes("don't have that")) {
            // Show the message in the shop modal only if modal open or user acted recently
            if (shopData || (Date.now() - lastUserCommandTs) < 5000) {
              pushShopMessage(msg);
            } else {
              console.log('Ignoring unsolicited shopkeeper message (no recent user activity)');
            }
            // Also show in the output window as error
            addMessage(msg, 'error');
          } else if (data.channel === 'system') {
            addMessage(data.message, 'system');
          } else if (data.channel === 'game') {
            addMessage(data.message, 'normal');
          } else {
            addMessage(`[${data.channel}] ${data.message}`, 'info');
          }
        } else if (data.channel === 'system') {
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
    setLastUserCommandTs(Date.now());
  };

  // Wrapper used for help requests so we can surface help output in the UI
  const requestHelp = (command) => {
    // clear previous help
    setHelpContent(null);
    // Send the actual help command
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      addMessage('Not connected to server', 'error');
      return;
    }

    const message = JSON.stringify({ command });
    ws.current.send(message);
    addMessage(`> ${command}`, 'command');
    setLastUserCommandTs(Date.now());
  };

  /**
   * Request fresh inventory and equipment data from server
   * Note: Server automatically sends GMCP updates after identify and other actions
   * This function is kept as a placeholder for explicit refresh needs
   */
  const refreshInventoryEquipment = () => {
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      return;
    }
    
    // Server automatically sends Char.Inventory and Char.Equipment GMCP
    // updates after identify, transactions, and equipment changes
    // No action needed here - GMCP updates happen automatically
    console.log('Inventory/equipment will auto-refresh via GMCP');
  };

  /**
   * Request fresh skills data from server via GMCP
   * Ensures skill list is current when opening modals
   */
  const refreshSkills = () => {
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      return;
    }
    
    // Send 'practice' command to trigger webgate_send_char_skills()
    // Don't add to message log (silent refresh)
    const message = JSON.stringify({
      command: 'practice'
    });
    ws.current.send(message);
    console.log('Requesting Char.Skills GMCP update via skills command');
    setLastUserCommandTs(Date.now());
  };

  /**
   * Request fresh skill tree data from server via GMCP
   * Ensures skill tree is current when opening skill tree modal
   */
  const refreshSkillTree = () => {
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      return;
    }
    
    // Send 'skilltree' command to trigger webgate_send_char_skill_tree()
    const message = JSON.stringify({
      command: 'skilltree'
    });
    ws.current.send(message);
    console.log('Requesting Char.SkillTree GMCP update via skilltree command');
    setLastUserCommandTs(Date.now());
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

  /*
    Intent: Save configured aliases to state and persist to character-specific localStorage.
    Inputs: newAliases - array of 8 alias objects or nulls from AliasAssign modal.
    Outputs: Updates assignedAliases state and saves to localStorage with character-specific key.
    Preconditions: newAliases should be properly formatted array, characterName should be set.
    Postconditions: Aliases are available for AliasBar to render and localStorage is updated per character.
  */
  const handleSaveAliases = (newAliases) => {
    setAssignedAliases(newAliases);
    if (characterName) {
      const aliasKey = `dd4_aliases_${characterName}`;
      localStorage.setItem(aliasKey, JSON.stringify(newAliases));
      console.log('Alias configuration saved for', characterName, ':', newAliases);
    }
  };

  /*
    Intent: Recover disarmed weapons by sending get/wield commands for each weapon.
    Executes recovery sequence for all disarmed weapons (handles dual-wielding).
    Inputs: disarmedWeapons - array of weapon objects with keywords
    Outputs: Sends commands to server, hides recovery button immediately
    Preconditions: disarmedWeapons array populated with weapon data from GMCP
    Postconditions: Commands sent to server, isDisarmed set to false (button fades)
    Notes: Button hides immediately to handle edge case where mob picks up weapon.
           Uses first keyword from weapon's keywords string for targeting.
  */
  const handleWeaponRecovery = () => {
    if (!disarmedWeapons || disarmedWeapons.length === 0) {
      console.warn('handleWeaponRecovery called with no disarmed weapons');
      return;
    }
    
    console.log('Recovering weapons:', disarmedWeapons);
    
    // Send get and wield commands for each disarmed weapon
    disarmedWeapons.forEach(weapon => {
      // Extract first keyword from keywords string
      const keyword = weapon.keywords ? weapon.keywords.split(' ')[0] : weapon.name;
      console.log(`Recovering weapon: ${weapon.name} using keyword: ${keyword}`);
      sendCommand(`get ${keyword}`);
      sendCommand(`wield ${keyword}`);
    });
    
    // Hide button immediately after sending commands
    // Handles edge case where another mob picks up weapon
    setIsDisarmed(false);
    setDisarmedWeapons([]);
  };

  return (
    <div className="app">
      <header className="app-header">
        <div className="header-left">
          <h1>Dragons Domain IV</h1>
        </div>
        <div className="header-center">
          <QuickActions
            onCommand={sendCommand}
            connected={connected}
            status={status}
            room={room}
            onOpenCharacterSheet={() => setShowCharacterSheet(true)}
            onOpenSpellBook={() => {
              refreshSkills();
              setShowSpellBook(true);
            }}
            onOpenSkillTree={() => {
              refreshSkillTree();
              setShowSkillTree(true);
            }}
          />
        </div>
        <div className="header-right">
          <div className={`status ${connected ? 'connected' : reconnecting ? 'reconnecting' : 'disconnected'}`}>
            {connected ? '● Connected' : reconnecting ? '○ Reconnecting...' : '○ Disconnected'}
          </div>
        </div>
      </header>

      <div className="app-content">
        <div className="left-panel">
          <CharacterInfo vitals={vitals} status={status} onCommand={sendCommand} />
          <Compass 
            exits={room.exits} 
            onMove={handleMove} 
            onCommand={sendCommand}
            hasMap={currentMap !== null}
            onShowMap={() => setShowMapModal(true)}
          />
        </div>

        <div className="main-panel">
          <RoomDisplay room={room} status={status} />
          <CombatLog messages={messages} />
          <CommandInput onSubmit={sendCommand} connected={connected} />
        </div>

        <div className="right-panel">
          <EnemyStatus 
            enemies={enemies} 
            isDisarmed={isDisarmed}
            disarmedWeapons={disarmedWeapons}
            onRecoverWeapon={handleWeaponRecovery}
            connected={connected}
          />
          <RoomContents 
            items={room.items} 
            npcs={room.npcs} 
            extraDescriptions={room.extraDescriptions || []}
            onCommand={sendCommand}
            connected={connected}
            skills={skills}
            openers={openers}
            onPracticeClick={() => setShowPracticeModal(true)}
            itemDetails={itemDetails}
            currentPlayerName={status?.name}
            status={status}
          />
        </div>
      </div>

      <SkillBar
        skills={skills}
        assignedSkills={assignedSkills}
        onUseSkill={handleUseSkill}
        onAssignClick={() => {
          refreshSkills();
          setShowSkillAssign(true);
        }}
        connected={connected}
        characterName={characterName}
        isFighting={isFighting}
        opponent={opponent}
        room={room}
        equipment={equipment}
        onCommand={sendCommand}
      />

      <AliasBar
        aliases={assignedAliases}
        onOpenSettings={() => setShowAliasModal(true)}
        onExecuteCommand={sendCommand}
      />

      {showSkillAssign && (
        <SkillAssign
          skills={skills}
          assignedSkills={assignedSkills}
          openers={openers}
          onSave={handleSaveSkillConfig}
          onClose={() => setShowSkillAssign(false)}
          characterName={characterName}
          room={room}
        />
      )}

      {showAliasModal && (
        <AliasAssign
          currentAliases={assignedAliases}
          onSave={handleSaveAliases}
          onClose={() => setShowAliasModal(false)}
        />
      )}

      {showCharacterSheet && (
        <CharacterSheet
          inventory={inventory}
          equipment={equipment}
          onCommand={sendCommand}
          onClose={() => setShowCharacterSheet(false)}
          connected={connected}
          onRefresh={refreshInventoryEquipment}
          status={status}
        />
      )}

      {shopData && (
        <ShopModal
          shopkeeper={shopData.shopkeeper}
          items={shopData.items}
          inventory={inventory}
          onClose={() => {
            setShopData(null);
            setShopMessage(null);
          }}
          onBuy={sendCommand}
          connected={connected}
          onRefresh={refreshInventoryEquipment}
          shopMessage={shopMessage}
        />
      )}

      {identifierData && (
        <IdentifyModal
          identifier={identifierData.identifier}
          inventory={inventory}
          onClose={() => {
            setIdentifierData(null);
          }}
          onIdentify={(keywords) => sendCommand(`identify ${keywords}`)}
          connected={connected}
        />
      )}

      {showPracticeModal && (
        <PracticeModal
          skills={skills}
          pracPhysical={vitals.pracPhysical || 0}
          pracIntellectual={vitals.pracIntellectual || 0}
          onClose={() => setShowPracticeModal(false)}
          onPractice={sendCommand}
          onRefresh={refreshSkills}
          connected={connected}
          onHelp={requestHelp}
        />
      )}

      {helpContent && (
        <HelpModal content={helpContent} onClose={() => setHelpContent(null)} />
      )}

      {showSpellBook && (
        <SpellBookModal
          skills={skills}
          room={room}
          inventory={inventory}
          onCommand={sendCommand}
          onClose={() => setShowSpellBook(false)}
        />
      )}

      {showSkillTree && skillTree && (
        <SkillTreeModal
          key={`st-${skillTree.available?.length || 0}-${skillTree.locked?.length || 0}`}
          skillTree={skillTree}
          onCommand={sendCommand}
          onClose={() => setShowSkillTree(false)}
        />
      )}

      {showMapModal && currentMap && (
        <MapModal
          mapData={currentMap}
          playerLevel={vitals.level || 1}
          onClose={() => setShowMapModal(false)}
        />
      )}

      {healerData && (
        <HealerModal
          healer={healerData.healer}
          services={healerData.services}
          onClose={() => setHealerData(null)}
          onBuyService={sendCommand}
          connected={connected}
        />
      )}
    </div>
  );
}

export default App;
