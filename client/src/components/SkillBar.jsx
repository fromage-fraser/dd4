import { useState, useEffect, useCallback } from 'react';
import './SkillBar.css';

/*
 * Intent: Display 8-slot skill bar with hotkey support (1-8) and cooldown tracking.
 *
 * Provides visual representation of assigned skills with:
 * - Skill name and mana cost display
 * - Keyboard hotkey support (keys 1-8)
 * - Client-side cooldown progress indicators
 * - Empty slot states for unassigned positions
 * - Click-to-use functionality
 *
 * Inputs:
 *   - skills: Array of all available skills from Char.Skills GMCP
 *   - assignedSkills: Array of 8 skill IDs (or null for empty slots)
 *   - onUseSkill: Callback(skillId) when skill is activated
 *   - onAssignClick: Callback() to open skill assignment modal
 *   - connected: Boolean indicating WebSocket connection state
 *
 * Cooldown Tracking:
 *   - Client tracks cooldowns using setTimeout based on skill.beats
 *   - Visual progress indicator shows remaining cooldown time
 *   - Beats are converted to seconds (game ticks)
 */
function SkillBar({ skills, assignedSkills, onUseSkill, onAssignClick, connected }) {
  const [cooldowns, setCooldowns] = useState({}); // { slotIndex: { endTime, duration } }

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

  return (
    <div className="skill-bar">
      <div className="skill-bar-header">
        <span className="skill-bar-title">Skills</span>
        <button 
          className="skill-assign-button"
          onClick={onAssignClick}
          disabled={!connected}
          title="Assign skills to hotbar"
        >
          ⚙️
        </button>
      </div>
      <div className="skill-slots">
        {assignedSkills.map((skillId, index) => {
          const skill = skillId ? skills.find(s => s.id === skillId) : null;
          const cooldown = cooldowns[index];
          const isOnCooldown = cooldown && Date.now() < cooldown.endTime;
          const progress = getCooldownProgress(index);

          return (
            <div
              key={index}
              className={`skill-slot ${!skill ? 'empty' : ''} ${isOnCooldown ? 'on-cooldown' : ''} ${!connected ? 'disabled' : ''}`}
              onClick={() => handleSkillUse(index, skillId)}
              title={skill ? `${skill.name}\nMana: ${skill.mana}\nCooldown: ${skill.beats}s\nHotkey: ${index + 1}` : 'Empty slot - click ⚙️ to assign'}
            >
              <div className="skill-hotkey">{index + 1}</div>
              
              {skill ? (
                <>
                  <div className="skill-icon">{skill.type === 'spell' ? '🔮' : '⚔️'}</div>
                  <div className="skill-name">{skill.name}</div>
                  <div className="skill-mana">{skill.mana > 0 ? `${skill.mana}m` : ''}</div>
                  
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
    </div>
  );
}

export default SkillBar;
