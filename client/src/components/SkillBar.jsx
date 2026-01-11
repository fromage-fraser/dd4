import { useState, useEffect, useCallback } from 'react';
import './SkillBar.css';
import {
  loadSkillTargets,
  getEffectiveTarget,
  getTargetTypeColor,
  shouldShowWarning,
  requiresTarget,
  saveSkillTarget
} from '../utils/targetMemory';
import TargetSelector from './TargetSelector';

/*
 * Intent: Display 8-slot skill bar with hotkey support (1-8) and cooldown tracking.
 *
 * Provides visual representation of assigned skills with:
 * - Skill name and mana cost display
 * - Keyboard hotkey support (keys 1-8)
 * - Client-side cooldown progress indicators
 * - Empty slot states for unassigned positions
 * - Click-to-use functionality
 * - Conditional action buttons (zap for wands, brandish for staves, flee when fighting)
 *
 * Inputs:
 *   - skills: Array of all available skills from Char.Skills GMCP
 *   - assignedSkills: Array of 8 skill IDs (or null for empty slots)
 *   - onUseSkill: Callback(skillId) when skill is activated
 *   - onAssignClick: Callback() to open skill assignment modal
 *   - connected: Boolean indicating WebSocket connection state
 *   - characterName: String - character name for target memory localStorage keys
 *   - isFighting: Boolean - whether character is in combat
 *   - opponent: String|null - current combat opponent name
 *   - room: Object - current room data with npcs array
 *   - equipment: Object - equipment object with wield/hold slots containing item type info
 *   - onCommand: Callback(command) to execute game commands (for zap/brandish/flee)
 *
 * Cooldown Tracking:
 *   - Client tracks cooldowns using setTimeout based on skill.beats
 *   - Visual progress indicator shows remaining cooldown time
 *   - Beats are converted to seconds (game ticks)
 */
function SkillBar({ skills, assignedSkills, onUseSkill, onAssignClick, connected, characterName, isFighting, opponent, room, equipment, onCommand }) {
  const [cooldowns, setCooldowns] = useState({}); // { slotIndex: { endTime, duration } }
  const [skillTargets, setSkillTargets] = useState({}); // Loaded from localStorage
  const [targetSelectorOpen, setTargetSelectorOpen] = useState(false);
  const [selectedSkillForTarget, setSelectedSkillForTarget] = useState(null);

  // Load saved targets from localStorage when character name becomes available
  useEffect(() => {
    if (characterName) {
      const targets = loadSkillTargets(characterName);
      setSkillTargets(targets);
      console.log('Loaded skill targets for', characterName, ':', targets);
    }
  }, [characterName]);

  // Start cooldown timer for a slot
  const startCooldown = (slotIndex, beats) => {
    if (beats <= 0) return;

    const duration = beats * 1000; // Convert beats to milliseconds (1 beat = 1 second approx)
    const endTime = Date.now() + duration;

    setCooldowns(prev => ({
      ...prev,
      [slotIndex]: { endTime, duration }
    }));

    // Clear cooldown after duration
    setTimeout(() => {
      setCooldowns(prev => {
        const newCooldowns = { ...prev };
        delete newCooldowns[slotIndex];
        return newCooldowns;
      });
    }, duration);
  };

  // Handle skill use
  const handleSkillUse = useCallback((slotIndex, skillId) => {
    if (!connected || !skillId) return;
    
    const cooldown = cooldowns[slotIndex];
    if (cooldown && Date.now() < cooldown.endTime) {
      return; // Still on cooldown
    }

    const skill = skills.find(s => s.id === skillId);
    if (skill) {
      onUseSkill(skill);
      startCooldown(slotIndex, skill.beats);
    }
  }, [connected, cooldowns, skills, onUseSkill]);

  // Keyboard hotkey support (1-8)
  useEffect(() => {
    const handleKeyPress = (e) => {
      // Only handle if not in input field
      if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') {
        return;
      }

      const key = e.key;
      if (key >= '1' && key <= '8') {
        const slotIndex = parseInt(key) - 1;
        const skillId = assignedSkills[slotIndex];
        if (skillId) {
          e.preventDefault();
          handleSkillUse(slotIndex, skillId);
        }
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, [assignedSkills, handleSkillUse]);

  // Calculate cooldown progress (0-100%)
  const getCooldownProgress = (slotIndex) => {
    const cooldown = cooldowns[slotIndex];
    if (!cooldown) return 0;

    const remaining = cooldown.endTime - Date.now();
    if (remaining <= 0) return 0;

    return ((cooldown.duration - remaining) / cooldown.duration) * 100;
  };

  // Force re-render for cooldown animation
  useEffect(() => {
    if (Object.keys(cooldowns).length === 0) return;

    const interval = setInterval(() => {
      setCooldowns(prev => ({ ...prev })); // Trigger re-render
    }, 100);

    return () => clearInterval(interval);
  }, [cooldowns]);

  // Handle right-click to open target selector
  const handleContextMenu = (e, skill) => {
    if (!skill || !characterName) return;
    
    // Only show target selector for skills that require targeting
    if (!requiresTarget(skill.target)) return;

    e.preventDefault();
    setSelectedSkillForTarget(skill);
    setTargetSelectorOpen(true);
  };

  // Handle target selection from modal
  const handleTargetSave = (targetName) => {
    if (!selectedSkillForTarget || !characterName) return;

    // Save to localStorage
    saveSkillTarget(characterName, selectedSkillForTarget.id, targetName);
    
    // Update local state
    setSkillTargets(prev => ({
      ...prev,
      [selectedSkillForTarget.id]: targetName
    }));

    console.log(`Target set for ${selectedSkillForTarget.name}: ${targetName}`);
  };

  // Helper functions to detect wielded item types
  const isWieldingWand = () => {
    return equipment?.wield?.type === 3 || equipment?.hold?.type === 3;
  };

  const isWieldingStaff = () => {
    return equipment?.wield?.type === 4 || equipment?.hold?.type === 4;
  };

  return (
    <div className="skill-bar">
      <div className="skill-bar-container">
        <span className="skill-bar-title">Skills</span>
        
        {/* Conditional action buttons - left side */}
        {isWieldingWand() && (
          <button 
            className="skill-action-button"
            onClick={() => onCommand('zap')}
            disabled={!connected}
            title="Zap wand"
            style={{ borderColor: '#ffc107' }}
          >
            ⚡
          </button>
        )}
        {isWieldingStaff() && (
          <button 
            className="skill-action-button"
            onClick={() => onCommand('brandish')}
            disabled={!connected}
            title="Brandish staff"
            style={{ borderColor: '#9c27b0' }}
          >
            🔮
          </button>
        )}
        
        <div className="skill-slots">
        {assignedSkills.map((skillId, index) => {
          const skill = skillId ? skills.find(s => s.id === skillId) : null;
          const cooldown = cooldowns[index];
          const isOnCooldown = cooldown && Date.now() < cooldown.endTime;
          const progress = getCooldownProgress(index);

          // Get target information
          const targetInfo = skill && characterName
            ? getEffectiveTarget(characterName, skill.id, skill.target, isFighting, opponent)
            : { target: null, isAuto: false };
          const showWarning = skill && shouldShowWarning(
            skill.target,
            !!targetInfo.target,
            isFighting
          );
          const slotColor = skill ? getTargetTypeColor(skill.target) : null;

          return (
            <div
              key={index}
              className={`skill-slot ${!skill ? 'empty' : ''} ${isOnCooldown ? 'on-cooldown' : ''} ${!connected ? 'disabled' : ''}`}
              onClick={() => handleSkillUse(index, skillId)}
              onContextMenu={(e) => handleContextMenu(e, skill)}
              title={skill ? `${skill.name}\nMana: ${skill.mana}\nCooldown: ${skill.beats}s\nHotkey: ${index + 1}${targetInfo.target ? `\nTarget: ${targetInfo.target}${targetInfo.isAuto ? ' (auto)' : ''}` : ''}\n\nRight-click to change target` : 'Empty slot - click ⚙️ to assign'}
              style={slotColor ? { borderColor: slotColor } : {}}
            >
              <div className="skill-hotkey">{index + 1}</div>
              
              {skill ? (
                <>
                  <div className="skill-icon">{skill.type === 'spell' ? '🔮' : '⚔️'}</div>
                  <div className="skill-name">{skill.name}</div>
                  <div className="skill-mana">{skill.mana > 0 ? `${skill.mana}m` : ''}</div>
                  
                  {/* Target display */}
                  {targetInfo.target && (
                    <div className="skill-target" style={{ color: slotColor }}>
                      {targetInfo.target}
                      {targetInfo.isAuto && <span className="auto-tag">(auto)</span>}
                    </div>
                  )}
                  
                  {/* Warning icon for missing target */}
                  {showWarning && (
                    <div className="skill-warning" title="No target set">⚠️</div>
                  )}
                  
                  {isOnCooldown && (
                    <div className="cooldown-overlay">
                      <div 
                        className="cooldown-progress" 
                        style={{ height: `${100 - progress}%` }}
                      />
                      <div className="cooldown-text">
                        {Math.ceil((cooldown.endTime - Date.now()) / 1000)}s
                      </div>
                    </div>
                  )}
                </>
              ) : (
                <div className="empty-slot-text">+</div>
              )}
            </div>
          );
        })}
        </div>
        <button 
          className="skill-assign-button"
          onClick={onAssignClick}
          disabled={!connected}
          title="Assign skills to hotbar"
        >
          ⚙️
        </button>
        
        {/* Flee action button - right side */}
        {isFighting && (
          <button 
            className="skill-action-button skill-flee-button"
            onClick={() => onCommand('flee')}
            disabled={!connected}
            title="Flee from combat"
            style={{ borderColor: '#f44336' }}
          >
            🏃
          </button>
        )}
      </div>

      {/* Target Selector Modal */}
      <TargetSelector
        isOpen={targetSelectorOpen}
        onClose={() => {
          setTargetSelectorOpen(false);
          setSelectedSkillForTarget(null);
        }}
        onSave={handleTargetSave}
        skillName={selectedSkillForTarget?.name}
        targetType={selectedSkillForTarget?.target}
        npcs={room?.npcs || []}
        characterName={characterName}
        isMassMode={false}
        affectedSkills={[]}
      />
    </div>
  );
}

export default SkillBar;
