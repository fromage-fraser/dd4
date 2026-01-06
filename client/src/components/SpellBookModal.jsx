import React, { useState, useMemo, useEffect } from 'react';
import './SpellBookModal.css';

// TAR_* constants matching server-side (from merc.h)
const TAR_IGNORE = 0;                  // Area effect
const TAR_CHAR_OFFENSIVE = 1;          // Offensive (multi-target)
const TAR_CHAR_DEFENSIVE = 2;          // Defensive (buff others)
const TAR_CHAR_SELF = 3;              // Self-cast
const TAR_OBJ_INV = 4;                // Item in inventory
const TAR_OBJ_ROOM = 5;               // Item in room
const TAR_CHAR_OFFENSIVE_SINGLE = 6;  // Offensive (single-target)

const LOCALSTORAGE_KEYS = {
  SORT_MODE: 'spellbook_sort_mode',
  RECENT_SPELLS: 'spellbook_recent_spells',
  CATEGORY_PREFIX: 'spellbook_category_',
};

const CATEGORIES = [
  { id: 'offensive', name: 'Offensive', icon: '🗡️', targets: [TAR_CHAR_OFFENSIVE, TAR_CHAR_OFFENSIVE_SINGLE] },
  { id: 'defensive', name: 'Defensive', icon: '🛡️', targets: [TAR_CHAR_DEFENSIVE] },
  { id: 'self', name: 'Self', icon: '✨', targets: [TAR_CHAR_SELF] },
  { id: 'item', name: 'Item', icon: '🔮', targets: [TAR_OBJ_INV] },
  { id: 'roomitem', name: 'Room Items', icon: '🌍', targets: [TAR_OBJ_ROOM] },
  { id: 'area', name: 'Area', icon: '💥', targets: [TAR_IGNORE] },
];

const SpellBookModal = ({ skills = [], room = {}, inventory = [], onCommand, onClose }) => {
  const [searchTerm, setSearchTerm] = useState('');
  const [sortMode, setSortMode] = useState(() => 
    localStorage.getItem(LOCALSTORAGE_KEYS.SORT_MODE) || 'proficiency'
  );
  const [collapsedCategories, setCollapsedCategories] = useState(() => {
    const collapsed = {};
    CATEGORIES.forEach(cat => {
      const key = `${LOCALSTORAGE_KEYS.CATEGORY_PREFIX}${cat.id}_collapsed`;
      collapsed[cat.id] = localStorage.getItem(key) === 'true';
    });
    return collapsed;
  });
  const [recentSpells, setRecentSpells] = useState(() => {
    const stored = localStorage.getItem(LOCALSTORAGE_KEYS.RECENT_SPELLS);
    return stored ? JSON.parse(stored) : [];
  });
  const [selectedSpell, setSelectedSpell] = useState(null);
  const [selectedTarget, setSelectedTarget] = useState('');

  // Debug logging
  useEffect(() => {
    console.log('SpellBook Debug:');
    console.log('- Total skills:', skills.length);
    console.log('- Skills sample:', skills.slice(0, 5));
    console.log('- Learned abilities:', skills.filter(s => s.learned > 0).length);
  }, [skills]);

  // Filter to show all learned abilities (both spells and skills)
  const spells = useMemo(() => 
    skills.filter(s => s.learned > 0),
    [skills]
  );

  // Apply search filter
  const filteredSpells = useMemo(() => {
    if (!searchTerm) return spells;
    const term = searchTerm.toLowerCase();
    return spells.filter(s => s.name.toLowerCase().includes(term));
  }, [spells, searchTerm]);

  // Sort spells
  const sortedSpells = useMemo(() => {
    const sorted = [...filteredSpells];
    if (sortMode === 'proficiency') {
      sorted.sort((a, b) => b.learned - a.learned);
    } else {
      sorted.sort((a, b) => a.name.localeCompare(b.name));
    }
    return sorted;
  }, [filteredSpells, sortMode]);

  // Categorize spells (spells can appear in multiple categories)
  const categorizedSpells = useMemo(() => {
    const result = {};
    CATEGORIES.forEach(category => {
      result[category.id] = sortedSpells.filter(spell => {
        // Default target to -1 if undefined (means server hasn't sent target field yet)
        const spellTarget = spell.target !== undefined ? spell.target : -1;
        return category.targets.includes(spellTarget);
      });
    });
    console.log('Categorized spells:', result);
    return result;
  }, [sortedSpells]);

  // Get proficiency color
  const getProficiencyColor = (learned) => {
    if (learned < 50) return '#ef4444'; // red
    if (learned < 80) return '#f59e0b'; // orange
    return '#10b981'; // green
  };

  // Toggle sort mode
  const toggleSortMode = () => {
    const newMode = sortMode === 'proficiency' ? 'alphabetical' : 'proficiency';
    setSortMode(newMode);
    localStorage.setItem(LOCALSTORAGE_KEYS.SORT_MODE, newMode);
  };

  // Toggle category collapse
  const toggleCategory = (categoryId) => {
    const newCollapsed = !collapsedCategories[categoryId];
    setCollapsedCategories(prev => ({
      ...prev,
      [categoryId]: newCollapsed
    }));
    localStorage.setItem(`${LOCALSTORAGE_KEYS.CATEGORY_PREFIX}${categoryId}_collapsed`, newCollapsed);
  };

  // Add spell to recent history
  const addToRecentSpells = (spell, target = '') => {
    const entry = {
      id: spell.id,
      name: spell.name,
      target: target,
      timestamp: Date.now()
    };
    
    const updated = [entry, ...recentSpells.filter(s => s.id !== spell.id)].slice(0, 5);
    setRecentSpells(updated);
    localStorage.setItem(LOCALSTORAGE_KEYS.RECENT_SPELLS, JSON.stringify(updated));
  };

  // Cast spell or use skill
  const handleCastSpell = (spell, target = '') => {
    let command;
    
    if (spell.type === 'spell') {
      // Spells use "cast 'spellname' target" format
      if (target) {
        command = `cast '${spell.name}' ${target}`;
      } else {
        command = `cast '${spell.name}'`;
      }
    } else {
      // Skills: use first word of skill name as command
      // e.g., "bash door" -> "bash", "shield block" -> "shield"
      const skillCommand = spell.name.split(' ')[0];
      if (target) {
        command = `${skillCommand} ${target}`;
      } else {
        command = skillCommand;
      }
    }
    
    addToRecentSpells(spell, target);
    onCommand(command);
    onClose();
  };

  // Handle spell click - determine if targeting UI is needed
  const handleSpellClick = (spell) => {
    // Area effect spells - cast immediately
    if (spell.target === TAR_IGNORE) {
      handleCastSpell(spell);
      return;
    }
    
    // Self spells - cast immediately
    if (spell.target === TAR_CHAR_SELF) {
      handleCastSpell(spell);
      return;
    }
    
    // Spells needing target selection
    setSelectedSpell(spell);
    setSelectedTarget('');
  };

  // Get available targets for a spell
  const getAvailableTargets = (spell) => {
    if (!spell) return [];
    
    const targets = [];
    
    // Item spells - show inventory
    if (spell.target === TAR_OBJ_INV) {
      return inventory.map(item => ({
        type: 'item',
        keyword: item.keywords || item.name,
        display: item.name
      }));
    }
    
    // Room item spells - show room items
    if (spell.target === TAR_OBJ_ROOM) {
      return (room.items || []).map(item => ({
        type: 'roomitem',
        keyword: item.keywords || item.name,
        display: item.name
      }));
    }
    
    // Character-targeting spells
    if ([TAR_CHAR_OFFENSIVE, TAR_CHAR_OFFENSIVE_SINGLE, TAR_CHAR_DEFENSIVE].includes(spell.target)) {
      // Add self
      targets.push({ type: 'self', keyword: 'self', display: 'Self' });
      
      // Add NPCs
      (room.npcs || []).forEach(npc => {
        if (!npc.isPlayer) {
          targets.push({ type: 'npc', keyword: npc.keywords, display: npc.name });
        }
      });
      
      // Add players (for defensive spells)
      (room.npcs || []).forEach(char => {
        if (char.isPlayer) {
          targets.push({ type: 'player', keyword: char.keywords, display: char.name });
        }
      });
    }
    
    return targets;
  };

  // Cancel target selection
  const cancelTargeting = () => {
    setSelectedSpell(null);
    setSelectedTarget('');
  };

  // Confirm target and cast
  const confirmCast = () => {
    if (selectedSpell && selectedTarget) {
      handleCastSpell(selectedSpell, selectedTarget);
    }
  };

  // Render spell card
  const renderSpellCard = (spell) => {
    const isAreaEffect = spell.target === TAR_IGNORE;
    const profColor = getProficiencyColor(spell.learned);
    
    return (
      <div 
        key={`${spell.id}-${spell.name}`} 
        className="spell-card"
        onClick={() => handleSpellClick(spell)}
        style={{ borderLeftColor: profColor }}
      >
        <div className="spell-header">
          <span className="spell-name">{spell.name}</span>
          {isAreaEffect && <span className="area-effect-badge">Area Effect</span>}
        </div>
        
        <div className="spell-proficiency-bar" style={{ backgroundColor: `${profColor}33` }}>
          <div 
            className="spell-proficiency-fill"
            style={{ width: `${spell.learned}%`, backgroundColor: profColor }}
          />
        </div>
        
        <div className="spell-stats">
          <span className="spell-stat mana">{spell.mana}m</span>
          <span className="spell-stat cooldown">{spell.beats}s</span>
          <span 
            className="spell-stat proficiency"
            style={{ color: profColor }}
          >
            {spell.learned}%
          </span>
        </div>
      </div>
    );
  };

  // Render recent spell
  const renderRecentSpell = (recent) => {
    const spell = spells.find(s => s.id === recent.id);
    if (!spell) return null;
    
    return (
      <div 
        key={`recent-${recent.id}-${recent.timestamp}`}
        className="recent-spell-card"
        onClick={() => {
          if (recent.target) {
            handleCastSpell(spell, recent.target);
          } else {
            handleSpellClick(spell);
          }
        }}
      >
        <span className="recent-spell-name">{spell.name}</span>
        {recent.target && <span className="recent-spell-target">→ {recent.target}</span>}
      </div>
    );
  };

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="spellbook-modal" onClick={(e) => e.stopPropagation()}>
        <div className="spellbook-header">
          <h2>📖 SpellBook</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>

        {/* Recent Spells */}
        {recentSpells.length > 0 && (
          <div className="recent-spells-section">
            <h3>Recently Cast</h3>
            <div className="recent-spells-container">
              {recentSpells.map(renderRecentSpell)}
            </div>
          </div>
        )}

        {/* Search and Sort */}
        <div className="spellbook-controls">
          <div className="search-container">
            <span className="search-icon">🔍</span>
            <input
              type="text"
              placeholder="Search spells..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="search-input"
            />
            {searchTerm && (
              <button 
                className="clear-search"
                onClick={() => setSearchTerm('')}
              >
                ✕
              </button>
            )}
          </div>
          
          <button className="sort-toggle" onClick={toggleSortMode}>
            Sort: {sortMode === 'proficiency' ? 'Proficiency ▼' : 'Name A-Z ▲'}
          </button>
        </div>

        {/* Categories */}
        <div className="spellbook-content">
          {CATEGORIES.map(category => {
            const categorySpells = categorizedSpells[category.id];
            
            // Hide empty categories
            if (!categorySpells || categorySpells.length === 0) return null;
            
            const isCollapsed = collapsedCategories[category.id];
            
            return (
              <div key={category.id} className="spell-category">
                <div 
                  className="category-header"
                  onClick={() => toggleCategory(category.id)}
                >
                  <span className="category-icon">{category.icon}</span>
                  <span className="category-name">{category.name}</span>
                  <span className="category-count">({categorySpells.length})</span>
                  <span className={`category-chevron ${isCollapsed ? 'collapsed' : ''}`}>
                    ▼
                  </span>
                </div>
                
                {!isCollapsed && (
                  <div className="category-spells">
                    {categorySpells.map(renderSpellCard)}
                  </div>
                )}
              </div>
            );
          })}
        </div>

        {/* Targeting UI */}
        {selectedSpell && (
          <div className="targeting-overlay" onClick={cancelTargeting}>
            <div className="targeting-panel" onClick={(e) => e.stopPropagation()}>
              <h3>Cast {selectedSpell.name}</h3>
              
              <div className="target-selection">
                <label>Select Target:</label>
                <select 
                  value={selectedTarget}
                  onChange={(e) => setSelectedTarget(e.target.value)}
                  className="target-dropdown"
                >
                  <option value="">-- Choose Target --</option>
                  {getAvailableTargets(selectedSpell).map((target, idx) => (
                    <option key={`${target.keyword}-${idx}`} value={target.keyword}>
                      {target.display}
                    </option>
                  ))}
                </select>
              </div>
              
              <div className="targeting-actions">
                <button 
                  className="cast-button"
                  onClick={confirmCast}
                  disabled={!selectedTarget}
                >
                  Cast
                </button>
                <button className="cancel-button" onClick={cancelTargeting}>
                  Cancel
                </button>
              </div>
            </div>
          </div>
        )}

        <div className="spellbook-footer">
          <button className="footer-close-button" onClick={onClose}>Close</button>
        </div>
      </div>
    </div>
  );
};

export default SpellBookModal;
