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
 * 
 * Outputs: Rendered quick action button bar
 * 
 * Notes: Icons use emoji for universal compatibility. Can be replaced with
 *        icon library (FontAwesome, Material Icons) for more polished look.
 */
function QuickActions({ onCommand, connected, onOpenCharacterSheet, onOpenSpellBook }) {
  const quickCommands = [
    { label: '👁️', text: 'Look', command: 'look', color: '#2196f3' },
    { label: '📋', text: 'Sheet', action: 'sheet', color: '#9c27b0' },
    { label: '📖', text: 'Spells', action: 'spellbook', color: '#673ab7' },
    { label: '📊', text: 'Score', command: 'score', color: '#4caf50' },
    { label: '👥', text: 'Who', command: 'who', color: '#00bcd4' },
  ];

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
    
    onCommand(cmd.command);
  };

  return (
    <div className="quick-actions">
      <h4>Quick Actions</h4>
      <div className="quick-actions-grid">
        {quickCommands.map((cmd, idx) => (
          <button
            key={idx}
            className="quick-action-btn"
            style={{ borderColor: cmd.color }}
            onClick={() => handleClick(cmd)}
            disabled={!connected}
            title={cmd.text}
          >
            <span className="quick-action-icon">{cmd.label}</span>
            <span className="quick-action-text">{cmd.text}</span>
          </button>
        ))}
      </div>
    </div>
  );
}

export default QuickActions;
