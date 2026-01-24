import React from 'react';
import './QuickActions.css';

/**
 * Intent: Provide touch-friendly quick action buttons for common MUD commands.
 * 
 * Mobile-first component that gives players one-tap access to frequently used
 * commands and UI elements. Designed for easy thumb access on mobile devices.
 * 
 * Inputs: 
 * - onCommand: callback function to execute a command
 * - connected: boolean indicating server connection status
 * - onOpenCharacterSheet: callback to open the character sheet modal
 * - onOpenSpellBook: callback to open the spell book modal
 * - onOpenSkillTree: callback to open the skill tree modal
 * - onOpenConfigModal: callback to open the config/settings modal
 * 
 * Outputs: Rendered quick action button bar
 * 
 * Notes: Icons use emoji for universal compatibility. Can be replaced with
 *        icon library (FontAwesome, Material Icons) for more polished look.
 */
function QuickActions({ onCommand, connected, onOpenCharacterSheet, onOpenSpellBook, onOpenSkillTree, onOpenConfigModal, status, room }) {
  const quickCommands = [
    { label: '⚙️', text: 'Settings', action: 'config', color: '#607d8b' },
    { label: '👁️', text: 'Look', command: 'look', color: '#2196f3' },
    { label: '📋', text: 'Sheet', action: 'sheet', color: '#9c27b0' },
    { label: '📖', text: 'Spells', action: 'spellbook', color: '#673ab7' },
    { label: '📊', text: 'Score', command: 'score', color: '#4caf50' },
    { label: '👥', text: 'Who', command: 'who', color: '#00bcd4' },
  ];

  const isSleeping = status && status.position && String(status.position).toLowerCase().includes('sleep');
  const roomFlags = room && room.roomFlags ? room.roomFlags : [];
  const hasNoMob = roomFlags.includes('ROOM_NO_MOB');
  const isHealing = roomFlags.includes('ROOM_HEALING');

  const handleClick = (cmd) => {
    if (!connected) return;

    if (cmd.action === 'sheet') {
      onOpenCharacterSheet();
      return;
    }

    if (cmd.action === 'spellbook') {
      onOpenSpellBook();
      return;
    }

    if (cmd.action === 'skilltree') {
      onOpenSkillTree();
      return;
    }

    if (cmd.action === 'config') {
      onOpenConfigModal();
      return;
    }

    onCommand(cmd.command);
  };

  const handleSleepToggle = () => {
    if (!connected) return;
    if (isSleeping) {
      onCommand('wake');
    } else {
      onCommand('sleep');
    }
  };

  return (
    <div className="quick-actions">
      <div className="quick-actions-grid">
        {quickCommands.map((cmd, idx) => (
          <button
            key={idx}
            className={`quick-action-btn ${cmd.action === 'config' ? 'settings-btn' : ''}`}
            style={{ borderColor: cmd.color }}
            onClick={() => handleClick(cmd)}
            disabled={!connected}
            title={cmd.text}
          >
            <span className="quick-action-icon">{cmd.label}</span>
            <span className="quick-action-text">{cmd.text}</span>
          </button>
        ))}

        {/* Sleep/Wake quick action */}
        <button
          key="sleep"
          className={`quick-action-btn ${hasNoMob ? 'no-mob' : ''}`}
          style={{ borderColor: '#ff9800' }}
          onClick={handleSleepToggle}
          disabled={!connected}
          title={isSleeping ? 'Wake' : 'Sleep'}
        >
          <span className="quick-action-icon">{isSleeping ? '🛌' : '😴'}</span>
          <span className="quick-action-text">{isSleeping ? 'Wake' : 'Sleep'}</span>
          {isHealing && <span className="healing-overlay">↑</span>}
        </button>

        {/* Skill Tree quick action */}
        <button
          key="skilltree"
          className="quick-action-btn"
          style={{ borderColor: '#8bc34a' }}
          onClick={() => handleClick({ action: 'skilltree' })}
          disabled={!connected}
          title="Skill Tree"
        >
          <span className="quick-action-icon">🌳</span>
          <span className="quick-action-text">Skills</span>
        </button>
      </div>
    </div>
  );
}

export default QuickActions;
