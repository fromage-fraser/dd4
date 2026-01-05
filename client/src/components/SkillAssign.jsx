import { useState } from 'react';
import './SkillAssign.css';

/*
 * Intent: Modal for assigning skills to hotbar slots and selecting opener skills.
 *
 * Allows players to:
 * - View all learned skills with filtering by type (spell/skill/opener)
 * - Drag or click skills to assign to 8 hotbar slots
 * - Select 2 opener skills for combat initiation (shown in NPC action menu)
 * - Clear assignments from slots
 * - Save configuration to localStorage
 *
 * Inputs:
 *   - skills: Array of all available skills from Char.Skills GMCP
 *   - assignedSkills: Current hotbar assignments [8 skill IDs or null]
 *   - openers: Current opener assignments [2 skill IDs or null]
 *   - onSave: Callback(assignedSkills, openers) when configuration saved
 *   - onClose: Callback() to close modal
 *
 * UI Organization:
 *   - Left: Available skills list (filterable, searchable)
 *   - Right: 8 hotbar slots + 2 opener slots
 *   - Bottom: Save/Cancel buttons
 */
function SkillAssign({ skills, assignedSkills, openers, onSave, onClose }) {
  const [localAssigned, setLocalAssigned] = useState([...assignedSkills]);
  const [localOpeners, setLocalOpeners] = useState([...openers]);
  const [filter, setFilter] = useState('all'); // all, spell, skill, opener
  const [search, setSearch] = useState('');

  // Filter skills based on type and search
  const filteredSkills = skills.filter(skill => {
    // Search filter
    if (search && !skill.name.toLowerCase().includes(search.toLowerCase())) {
      return false;
    }

    // Type filter
    if (filter === 'all') return true;
    if (filter === 'opener') return skill.opener;
    return skill.type === filter;
  });

  // Handle assigning skill to hotbar slot
  const handleAssignToSlot = (slotIndex, skillId) => {
    const newAssigned = [...localAssigned];
    newAssigned[slotIndex] = skillId;
    setLocalAssigned(newAssigned);
  };

  // Handle assigning skill to opener slot
  const handleAssignToOpener = (openerIndex, skillId) => {
    const skill = skills.find(s => s.id === skillId);
    if (!skill || !skill.opener) {
      return; // Only opener skills can be assigned to opener slots
    }

    const newOpeners = [...localOpeners];
    newOpeners[openerIndex] = skillId;
    setLocalOpeners(newOpeners);
  };

  // Clear a slot
  const handleClearSlot = (slotIndex) => {
    const newAssigned = [...localAssigned];
    newAssigned[slotIndex] = null;
    setLocalAssigned(newAssigned);
  };

  // Clear an opener slot
  const handleClearOpener = (openerIndex) => {
    const newOpeners = [...localOpeners];
    newOpeners[openerIndex] = null;
    setLocalOpeners(newOpeners);
  };

  // Save configuration
  const handleSave = () => {
    onSave(localAssigned, localOpeners);
    onClose();
  };

  // Check if skill is already assigned
  const isAssigned = (skillId) => {
    return localAssigned.includes(skillId) || localOpeners.includes(skillId);
  };

  return (
    <div className="skill-assign-overlay" onClick={onClose}>
      <div className="skill-assign-modal" onClick={(e) => e.stopPropagation()}>
        <div className="skill-assign-header">
          <h2>Assign Skills</h2>
          <button className="close-button" onClick={onClose}>×</button>
        </div>

        <div className="skill-assign-content">
          {/* Left panel: Available skills */}
          <div className="skills-list-panel">
            <div className="skills-filters">
              <input
                type="text"
                className="skill-search"
                placeholder="Search skills..."
                value={search}
                onChange={(e) => setSearch(e.target.value)}
              />
              <div className="filter-buttons">
                <button
                  className={filter === 'all' ? 'active' : ''}
                  onClick={() => setFilter('all')}
                >
                  All ({skills.length})
                </button>
                <button
                  className={filter === 'spell' ? 'active' : ''}
                  onClick={() => setFilter('spell')}
                >
                  Spells ({skills.filter(s => s.type === 'spell').length})
                </button>
                <button
                  className={filter === 'skill' ? 'active' : ''}
                  onClick={() => setFilter('skill')}
                >
                  Skills ({skills.filter(s => s.type === 'skill').length})
                </button>
                <button
                  className={filter === 'opener' ? 'active' : ''}
                  onClick={() => setFilter('opener')}
                >
                  Openers ({skills.filter(s => s.opener).length})
                </button>
              </div>
            </div>

            <div className="skills-list">
              {filteredSkills.map(skill => (
                <div
                  key={skill.id}
                  className={`skill-item ${isAssigned(skill.id) ? 'assigned' : ''}`}
                  draggable
                  onDragStart={(e) => e.dataTransfer.setData('skillId', skill.id)}
                >
                  <span className="skill-item-icon">
                    {skill.type === 'spell' ? '🔮' : '⚔️'}
                  </span>
                  <div className="skill-item-info">
                    <div className="skill-item-name">
                      {skill.name}
                      {skill.opener && <span className="opener-badge">⚡</span>}
                    </div>
                    <div className="skill-item-stats">
                      {skill.mana > 0 && <span className="mana">{skill.mana}m</span>}
                      <span className="cooldown">{skill.beats}s</span>
                      <span className="proficiency">{skill.learned}%</span>
                    </div>
                  </div>
                  {isAssigned(skill.id) && <span className="assigned-indicator">✓</span>}
                </div>
              ))}
            </div>
          </div>

          {/* Right panel: Assignment slots */}
          <div className="assignment-panel">
            <div className="opener-slots-section">
              <h3>Combat Openers</h3>
              <p className="section-help">Select 2 skills to initiate combat</p>
              <div className="opener-slots">
                {[0, 1].map(index => {
                  const skillId = localOpeners[index];
                  const skill = skillId ? skills.find(s => s.id === skillId) : null;

                  return (
                    <div
                      key={`opener-${index}`}
                      className={`opener-slot ${!skill ? 'empty' : ''}`}
                      onDragOver={(e) => e.preventDefault()}
                      onDrop={(e) => {
                        e.preventDefault();
                        const skillId = parseInt(e.dataTransfer.getData('skillId'));
                        handleAssignToOpener(index, skillId);
                      }}
                      onClick={() => skill && handleClearOpener(index)}
                      title={skill ? `${skill.name} - Click to remove` : 'Drop opener skill here'}
                    >
                      {skill ? (
                        <>
                          <span className="opener-icon">{skill.type === 'spell' ? '🔮' : '⚔️'}</span>
                          <span className="opener-name">{skill.name}</span>
                          <span className="opener-remove">×</span>
                        </>
                      ) : (
                        <span className="empty-text">Opener {index + 1}</span>
                      )}
                    </div>
                  );
                })}
              </div>
            </div>

            <div className="hotbar-slots-section">
              <h3>Hotbar (1-8)</h3>
              <p className="section-help">Assign skills to hotkeys 1-8</p>
              <div className="hotbar-assignment-slots">
                {localAssigned.map((skillId, index) => {
                  const skill = skillId ? skills.find(s => s.id === skillId) : null;

                  return (
                    <div
                      key={`slot-${index}`}
                      className={`hotbar-assignment-slot ${!skill ? 'empty' : ''}`}
                      onDragOver={(e) => e.preventDefault()}
                      onDrop={(e) => {
                        e.preventDefault();
                        const droppedSkillId = parseInt(e.dataTransfer.getData('skillId'));
                        handleAssignToSlot(index, droppedSkillId);
                      }}
                      onClick={() => skill && handleClearSlot(index)}
                      title={skill ? `${skill.name} - Click to remove` : 'Drop skill here'}
                    >
                      <div className="slot-hotkey">{index + 1}</div>
                      {skill ? (
                        <>
                          <span className="slot-icon">{skill.type === 'spell' ? '🔮' : '⚔️'}</span>
                          <span className="slot-name">{skill.name}</span>
                          <span className="slot-remove">×</span>
                        </>
                      ) : (
                        <span className="empty-text">+</span>
                      )}
                    </div>
                  );
                })}
              </div>
            </div>
          </div>
        </div>

        <div className="skill-assign-footer">
          <button className="cancel-button" onClick={onClose}>Cancel</button>
          <button className="save-button" onClick={handleSave}>Save Configuration</button>
        </div>
      </div>
    </div>
  );
}

export default SkillAssign;
