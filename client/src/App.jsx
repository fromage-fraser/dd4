import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import Compass from './components/Compass';
import RoomDisplay from './components/RoomDisplay';
import CharacterInfo from './components/CharacterInfo';
import CombatLog from './components/CombatLog';
import CommandInput from './components/CommandInput';
import QuickActions from './components/QuickActions';
import RoomContents from './components/RoomContents';
import SkillBar from './components/SkillBar';
import SkillAssign from './components/SkillAssign';
import CharacterSheet from './components/CharacterSheet';
import ShopModal from './components/ShopModal';
import HealerModal from './components/HealerModal';
import PracticeModal from './components/PracticeModal';
import MapModal from './components/MapModal';
import SpellBookModal from './components/SpellBookModal';

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
  const [showCharacterSheet, setShowCharacterSheet] = useState(false);
  const [inventory, setInventory] = useState([]);
  const [equipment, setEquipment] = useState(null);
  const [shopData, setShopData] = useState(null); // { shopkeeper, items }
  const [shopMessage, setShopMessage] = useState(null);
  const [healerData, setHealerData] = useState(null); // { healer, services }
  const [showPracticeModal, setShowPracticeModal] = useState(false);
  const [showSpellBook, setShowSpellBook] = useState(false);
  const [itemDetails, setItemDetails] = useState({}); // Store detailed item info keyed by item name
  const [mapsData, setMapsData] = useState(null); // All available maps from maps.json
  const [currentMap, setCurrentMap] = useState(null); // Current area's map data
  const [showMapModal, setShowMapModal] = useState(false); // Store detailed item info keyed by item name
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
   * Parse item details from identify/examine text
   * Extracts type, wear location, value, AC, set info, etc.
   */
  const parseItemDetails = (text, itemName) => {
    const details = {};
    
    // Remove ANSI color codes for parsing (both \u001b format and <##> format)
    const cleanText = text.replace(/\u001b\[[0-9;]*m/g, '').replace(/<[0-9]+>/g, '');
    
    // Extract long description - everything between the separator lines before set info
    const descMatch = cleanText.match(/={10,}\s*\n([\s\S]+?)\n(?:={10,}|-{10,}|$)/);
    if (descMatch) {
      details.longDescription = descMatch[1].trim();
    }
    
    // Extract item type - handle "piece of" prefix
    const typeMatch = cleanText.match(/You determine that .+ is a (?:piece of )?(\w+)\./);
    if (typeMatch) {
      details.itemType = typeMatch[1].charAt(0).toUpperCase() + typeMatch[1].slice(1);
    }
    
    // Extract wear location
    const wearMatch = cleanText.match(/It is worn on the ([\w\s]+)\./);
    if (wearMatch) {
      details.wearLocation = wearMatch[1].trim();
    }
    
    // Extract value and currency
    const valueMatch = cleanText.match(/is worth ([0-9,]+) (\w+) coins/);
    if (valueMatch) {
      details.value = parseInt(valueMatch[1].replace(/,/g, ''));
      details.valueCurrency = valueMatch[2];
    }
    
    // Extract armor class
    const acMatch = cleanText.match(/It has an armou?r class of ([0-9]+)\./);
    if (acMatch) {
      details.armorClass = parseInt(acMatch[1]);
    }
    
    // Extract damage dice for weapons
    const damageMatch = cleanText.match(/It inflicts ([0-9]+)d([0-9]+) damage/);
    if (damageMatch) {
      details.damageDice = `${damageMatch[1]}d${damageMatch[2]}`;
    }
    
    // Extract set information - look for the separator line and "This is part of"
    const setMatch = cleanText.match(/This is part of (?:a |an )?(\w+) set\./);
    if (setMatch) {
      details.setRarity = setMatch[1];
    }
    
    // Extract set name - line after the separator, before the blank line
    const setNameMatch = cleanText.match(/-{10,}\s*\n(?:This is part of[^\n]+\n)?([^\n:]+):/);
    if (setNameMatch) {
      details.setName = setNameMatch[1].trim();
    }
    
    // Extract set lore - everything between the set name line and "Equip X items"
    const loreMatch = cleanText.match(/:\s*\n\s*\n([\s\S]+?)\n(?:Equip \d)/);
    if (loreMatch) {
      details.setLore = loreMatch[1].trim().replace(/\n\n/g, '\n');
    }
    
    // Extract set bonus - "Equip X items to provide..."
    const setBonusMatch = cleanText.match(/Equip ([0-9]+) items? to provide ([\w\s]+) by ([0-9]+)/);
    if (setBonusMatch) {
      details.setBonus = {
        itemCount: parseInt(setBonusMatch[1]),
        stat: setBonusMatch[2].trim(),
        value: parseInt(setBonusMatch[3])
      };
    }
    
    console.log('Parsed item details:', details);
    return details;
  };

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
        setShopData(data);
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
          
          // Parse item identification/examination details
          if (msg.includes('You determine that') && msg.includes('is a')) {
            const itemNameMatch = msg.match(/You determine that ([^\n]+) is a/);
            if (itemNameMatch) {
              // Extract clean item name (remove ANSI codes)
              const rawName = itemNameMatch[1];
              const cleanName = rawName.replace(/\u001b\[[0-9;]*m/g, '').trim();
              
              // Strip articles (a/an/the) for better matching
              const nameWithoutArticle = cleanName.replace(/^(a|an|the)\s+/i, '');
              
              console.log('Parsing item details for:', cleanName);
              console.log('Storing as (without article):', nameWithoutArticle);
              console.log('Full examine text:', msg);
              
              const details = parseItemDetails(msg, cleanName);
              
              console.log('Parsed details:', details);
              
              // Store under the name without article for consistent lookup
              setItemDetails(prev => ({
                ...prev,
                [nameWithoutArticle]: details
              }));
            }
          }
          
          // Handle "uninterested" messages for items that can't be sold
          if (msg.includes('looks uninterested in') || msg.includes("won't buy that") || 
              msg.includes("can't sell") || msg.includes("don't have that")) {
            // If shop modal is open, show the message there
            if (shopData) {
              setShopMessage(msg);
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
  };

  /**
   * Request fresh inventory and equipment data from server
   * Note: With the server now sending GMCP updates automatically after 
   * buy/sell/drop/wear actions, this function is mainly a fallback
   */
  const refreshInventoryEquipment = () => {
    if (!ws.current || ws.current.readyState !== WebSocket.OPEN) {
      return;
    }
    
    // Server automatically sends Char.Inventory and Char.Equipment GMCP
    // updates after transactions, so we don't need to send text commands
    // This function is kept for potential future explicit refresh needs
    console.log('Inventory/equipment will auto-refresh via GMCP');
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
          <CharacterInfo vitals={vitals} status={status} onCommand={sendCommand} />
          <QuickActions 
            onCommand={sendCommand} 
            connected={connected}
            onOpenCharacterSheet={() => setShowCharacterSheet(true)}
            onOpenSpellBook={() => setShowSpellBook(true)}
          />
          <Compass 
            exits={room.exits} 
            onMove={handleMove} 
            onCommand={sendCommand}
            hasMap={currentMap !== null}
            onShowMap={() => setShowMapModal(true)}
          />
        </div>

        <div className="main-panel">
          <RoomDisplay room={room} />
          <CombatLog messages={messages} />
          <CommandInput onSubmit={sendCommand} connected={connected} />
        </div>

        <div className="right-panel">
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
          />
        </div>
      </div>

      <SkillBar
        skills={skills}
        assignedSkills={assignedSkills}
        onUseSkill={handleUseSkill}
        onAssignClick={() => setShowSkillAssign(true)}
        connected={connected}
      />

      {showSkillAssign && (
        <SkillAssign
          skills={skills}
          assignedSkills={assignedSkills}
          openers={openers}
          onSave={handleSaveSkillConfig}
          onClose={() => setShowSkillAssign(false)}
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
          itemDetails={itemDetails}
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

      {showPracticeModal && (
        <PracticeModal
          skills={skills}
          pracPhysical={vitals.pracPhysical || 0}
          pracIntellectual={vitals.pracIntellectual || 0}
          onClose={() => setShowPracticeModal(false)}
          onPractice={sendCommand}
          connected={connected}
        />
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
