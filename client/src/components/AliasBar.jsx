import React from 'react';
import './AliasBar.css';

/*
  Intent: Provide a customizable command alias bar that displays up to 8 user-configured command shortcuts.
  Responsibilities: Render alias slots with icons, handle alias execution, provide settings access.
  Interactions: Receives alias configurations from App.jsx, sends commands via onExecuteCommand callback.
  Constraints: Exactly 8 slots, positioned below skills bar with half the height.
  Extensibility: Icons and commands are user-configurable through the assignment modal.
*/

const AliasBar = ({ aliases = [], onOpenSettings, onExecuteCommand }) => {
  /*
    Intent: Handle click on an alias slot to execute command or open settings if empty.
    Inputs: index - the slot number (0-7) that was clicked.
    Outputs: Calls onExecuteCommand with the alias command if populated, or onOpenSettings if empty.
    Preconditions: aliases array must be defined.
    Failure Behavior: Opens settings if slot is empty or command is falsy.
  */
  const handleAliasClick = (index) => {
    const alias = aliases[index];
    if (alias && alias.command) {
      onExecuteCommand(alias.command);
    } else {
      // Open settings when clicking empty slot
      onOpenSettings();
    }
  };

  // Initialize 8 slots with null for empty slots
  const aliasSlots = Array(8).fill(null).map((_, i) => aliases[i] || null);

  return (
    <div className="alias-bar">
      <div className="alias-bar-container">
        <span className="alias-bar-title">Aliases</span>
        <div className="alias-slots">
          {aliasSlots.map((alias, index) => (
            <button
              key={index}
              className={`alias-slot ${!alias ? 'empty' : ''}`}
              onClick={() => handleAliasClick(index)}
              title={alias ? alias.command : 'Empty alias slot'}
            >
              <span className="alias-icon">
                {alias ? alias.icon : '+'}
              </span>
            </button>
          ))}
        </div>
        <button 
          className="alias-settings-button" 
          onClick={onOpenSettings}
          title="Configure aliases"
        >
          ⚙️
        </button>
      </div>
    </div>
  );
};

export default AliasBar;
