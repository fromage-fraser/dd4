import React, { useState, useEffect } from 'react';
import './TargetSelector.css';
import { TAR_CHAR_OFFENSIVE, TAR_CHAR_OFFENSIVE_SINGLE, TAR_CHAR_DEFENSIVE } from '../utils/targetMemory';

/**
 * TargetSelector component - Reusable modal for selecting skill targets
 * 
 * Intent: Provide intuitive target selection UI that filters room NPCs
 * based on skill targetType. Supports single skill and mass targeting modes.
 * Persists selections using targetMemory utility.
 * 
 * Props:
 * - isOpen: Boolean controlling modal visibility
 * - onClose: Callback to close modal
 * - onSave: Callback receiving {skillId, targetName} or {skillIds, targetName} for mass mode
 * - skillName: Display name of skill being configured (optional)
 * - targetType: TAR_* constant determining which NPCs to show
 * - npcs: Array of NPCs from room.npcs
 * - characterName: String - character's own name to include as option for defensive skills
 * - isMassMode: Boolean - if true, expects multiple skills and shows skill count
 * - affectedSkills: Array of {id, name} for mass mode preview
 */
function TargetSelector({ 
  isOpen, 
  onClose, 
  onSave, 
  skillName = 'skill',
  targetType,
  npcs = [],
  characterName = null,
  isMassMode = false,
  affectedSkills = []
}) {
  const [selectedTarget, setSelectedTarget] = useState('');
  const [filterText, setFilterText] = useState('');

  // Reset state when modal opens
  useEffect(() => {
    if (isOpen) {
      setSelectedTarget('');
      setFilterText('');
    }
  }, [isOpen]);

  if (!isOpen) return null;

  // Filter NPCs based on targetType
  const isOffensive = targetType === TAR_CHAR_OFFENSIVE || targetType === TAR_CHAR_OFFENSIVE_SINGLE;
  const isDefensive = targetType === TAR_CHAR_DEFENSIVE;

  // For defensive skills, include character as first option
  const characterOption = (isDefensive && characterName) ? [{
    name: characterName,
    isPlayer: true,
    isSelf: true,
    keywords: 'self me'
  }] : [];

  // Filter NPCs:
  // - Offensive skills: show all NPCs (both hostile and friendly)
  // - Defensive skills: show only players (isPlayer=true)
  const filteredNpcs = npcs.filter(npc => {
    // Skip current player from room NPCs (we add them separately above for defensive skills)
    if (npc.isPlayer && npc.isSelf) return false;

    // Target type filtering
    if (isDefensive) {
      // Defensive skills can only target players
      if (!npc.isPlayer) return false;
    }
    // Offensive skills can target any NPC

    // Search filter
    if (filterText) {
      const searchLower = filterText.toLowerCase();
      const nameLower = (npc.name || '').toLowerCase();
      const keywordsLower = (npc.keywords || '').toLowerCase();
      return nameLower.includes(searchLower) || keywordsLower.includes(searchLower);
    }

    return true;
  });

  // Combine character option (if any) with filtered NPCs
  const allTargets = [...characterOption, ...filteredNpcs];

  const handleSave = () => {
    if (!selectedTarget) return;

    onSave(selectedTarget);
    onClose();
  };

  const handleCancel = () => {
    setSelectedTarget('');
    onClose();
  };

  const getModalTitle = () => {
    if (isMassMode) {
      const skillCount = affectedSkills.length;
      return `Set Target for ${skillCount} ${isOffensive ? 'Offensive' : 'Defensive'} Skill${skillCount !== 1 ? 's' : ''}`;
    }
    return `Select Target for ${skillName}`;
  };

  const getEmptyMessage = () => {
    if (isDefensive) {
      return 'No other players in room. Defensive skills target allies.';
    }
    if (allTargets.length === 0 && filterText) {
      return `No NPCs matching "${filterText}"`;
    }
    return 'No NPCs in room';
  };

  return (
    <div className="target-selector-overlay" onClick={handleCancel}>
      <div className="target-selector-modal" onClick={(e) => e.stopPropagation()}>
        <div className="target-selector-header">
          <h3>{getModalTitle()}</h3>
          <button className="close-button" onClick={handleCancel}>✕</button>
        </div>

        {isMassMode && affectedSkills.length > 0 && (
          <div className="affected-skills-preview">
            <h4>Affected Skills ({affectedSkills.length}):</h4>
            <div className="skills-list">
              {affectedSkills.map(skill => (
                <span key={skill.id} className="skill-chip">{skill.name}</span>
              ))}
            </div>
          </div>
        )}

        <div className="target-selector-body">
          <div className="search-bar">
            <input
              type="text"
              placeholder="Filter targets..."
              value={filterText}
              onChange={(e) => setFilterText(e.target.value)}
              autoFocus
            />
          </div>

          <div className="target-type-hint">
            {isOffensive && <span className="hint-offensive">⚔️ Offensive - Select enemy to attack</span>}
            {isDefensive && <span className="hint-defensive">🛡️ Defensive - Select ally to assist</span>}
          </div>

          <div className="targets-list">
            {allTargets.length === 0 ? (
              <div className="empty-state">{getEmptyMessage()}</div>
            ) : (
              allTargets.map((npc, index) => (
                <button
                  key={`npc-${index}-${npc.id || npc.vnum}`}
                  className={`target-item ${selectedTarget === npc.name ? 'selected' : ''} ${npc.hostile ? 'hostile' : 'friendly'} ${npc.isPlayer ? 'player' : 'npc'}`}
                  onClick={() => setSelectedTarget(npc.name)}
                >
                  <span className="target-icon">
                    {npc.isPlayer ? '👤' : (npc.hostile ? '⚔️' : '🧍')}
                  </span>
                  <span className="target-name">{npc.name}</span>
                  {npc.isPlayer && <span className="player-badge">Player</span>}
                  {!npc.isPlayer && npc.hostile && <span className="hostile-badge">Hostile</span>}
                </button>
              ))
            )}
          </div>
        </div>

        <div className="target-selector-footer">
          <button className="cancel-button" onClick={handleCancel}>Cancel</button>
          <button 
            className="save-button" 
            onClick={handleSave}
            disabled={!selectedTarget}
          >
            {isMassMode ? 'Set All' : 'Set Target'}
          </button>
        </div>
      </div>
    </div>
  );
}

export default TargetSelector;
