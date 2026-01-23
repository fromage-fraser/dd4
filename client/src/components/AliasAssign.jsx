import React, { useState, useEffect } from 'react';
import './AliasAssign.css';

/*
  Intent: Provide a modal interface for configuring alias slots with commands and emoji icons.
  Responsibilities: Display 8 alias configuration slots, handle command text input, provide curated emoji selection.
  Interactions: Receives current aliases from parent, calls onSave with updated configuration, onClose to dismiss modal.
  Constraints: Exactly 8 configurable slots, curated emoji set for game relevance.
  Extensibility: Emoji set can be expanded, validation rules can be added for command syntax.
*/

// Curated emoji set for game-relevant commands (~30 icons)
const AVAILABLE_EMOJIS = [
  '💬', '📝', '⚔️', '🏃', '🍖', '🧪', '🗡️', '🛡️', 
  '🔮', '⚡', '🎯', '🏹', '🔥', '💰', '🗝️', '📿', 
  '🍺', '🎒', '🪓', '⚙️', '🌟', '💎', '🏺', '📜',
  '🧙', '🐉', '🏰', '⚗️', '🍞', '🎲'
];

const AliasAssign = ({ currentAliases = [], onSave, onClose }) => {
  /*
    Intent: Maintain local state for alias editing before saving.
    Inputs: currentAliases - array of 8 alias objects or nulls from parent.
    Outputs: Local state updated as user modifies commands and icons.
    Preconditions: currentAliases should be array of length up to 8.
    Postconditions: Local edits don't affect parent until onSave is called.
  */
  const [aliases, setAliases] = useState(() => {
    // Initialize 8 slots from current aliases
    const slots = Array(8).fill(null);
    currentAliases.forEach((alias, index) => {
      if (index < 8 && alias) {
        slots[index] = { ...alias };
      }
    });
    return slots;
  });

  const [selectedSlot, setSelectedSlot] = useState(null);

  /*
    Intent: Update the command text for a specific alias slot.
    Inputs: index - slot number (0-7), command - text string for the command.
    Outputs: Updates aliases state with new command for specified slot.
    Preconditions: index must be valid (0-7).
    Postconditions: Slot is created with default icon if it was null.
  */
  const handleCommandChange = (index, command) => {
    setAliases(prev => {
      const updated = [...prev];
      if (!updated[index]) {
        updated[index] = { command: '', icon: '💬' };
      }
      updated[index] = { ...updated[index], command };
      return updated;
    });
  };

  /*
    Intent: Update the icon for a specific alias slot.
    Inputs: index - slot number (0-7), icon - emoji character string.
    Outputs: Updates aliases state with new icon for specified slot.
    Preconditions: index must be valid (0-7), icon should be valid emoji.
    Postconditions: Slot is created with empty command if it was null.
  */
  const handleIconSelect = (index, icon) => {
    setAliases(prev => {
      const updated = [...prev];
      if (!updated[index]) {
        updated[index] = { command: '', icon };
      } else {
        updated[index] = { ...updated[index], icon };
      }
      return updated;
    });
    setSelectedSlot(null); // Close emoji picker after selection
  };

  /*
    Intent: Clear a specific alias slot, removing both command and icon.
    Inputs: index - slot number (0-7) to clear.
    Outputs: Sets slot to null in aliases state.
  */
  const handleClearSlot = (index) => {
    setAliases(prev => {
      const updated = [...prev];
      updated[index] = null;
      return updated;
    });
  };

  /*
    Intent: Save the configured aliases and close the modal.
    Outputs: Calls onSave callback with filtered non-empty aliases, then calls onClose.
    Preconditions: Aliases must be in valid format.
    Postconditions: Parent receives updated aliases array, modal closes.
  */
  const handleSave = () => {
    // Filter out empty aliases but preserve array positions
    const aliasesToSave = aliases.map(alias => {
      if (alias && alias.command && alias.command.trim()) {
        return {
          command: alias.command.trim(),
          icon: alias.icon || '💬'
        };
      }
      return null;
    });
    onSave(aliasesToSave);
    onClose();
  };

  /*
    Intent: Prevent modal close when clicking inside modal content.
    Inputs: e - click event from inside modal.
    Outputs: Stops event propagation to prevent overlay click handler.
  */
  const handleModalClick = (e) => {
    e.stopPropagation();
  };

  return (
    <div className="alias-assign-overlay" onClick={onClose}>
      <div className="alias-assign-modal" onClick={handleModalClick}>
        <div className="alias-assign-header">
          <h2>Configure Aliases</h2>
          <button className="close-button" onClick={onClose}>×</button>
        </div>
        <div className="alias-assign-info">
          Multiple commands can be configured, separate subsequent commands with a ;
        </div>

        <div className="alias-assign-content">
          <div className="alias-slots-config">
            {aliases.map((alias, index) => (
              <div key={index} className="alias-config-slot">
                <div className="slot-header">
                  <span className="slot-number">Slot {index + 1}</span>
                  {alias && (
                    <button 
                      className="clear-slot-button"
                      onClick={() => handleClearSlot(index)}
                      title="Clear this slot"
                    >
                      ×
                    </button>
                  )}
                </div>

                <div className="slot-config-row">
                  <div className="icon-selector">
                    <button
                      className="current-icon-button"
                      onClick={() => setSelectedSlot(selectedSlot === index ? null : index)}
                      title="Click to change icon"
                    >
                      <span className="icon-display">
                        {alias?.icon || '💬'}
                      </span>
                    </button>
                    {selectedSlot === index && (
                      <div className="emoji-picker">
                        {AVAILABLE_EMOJIS.map(emoji => (
                          <button
                            key={emoji}
                            className="emoji-option"
                            onClick={() => handleIconSelect(index, emoji)}
                            title={`Select ${emoji}`}
                          >
                            {emoji}
                          </button>
                        ))}
                      </div>
                    )}
                  </div>

                  <input
                    type="text"
                    className="command-input"
                    placeholder="Enter command (e.g., 'eat pie')"
                    value={alias?.command || ''}
                    onChange={(e) => handleCommandChange(index, e.target.value)}
                    maxLength={100}
                  />
                </div>
              </div>
            ))}
          </div>
        </div>

        <div className="alias-assign-footer">
          <button className="cancel-button" onClick={onClose}>
            Cancel
          </button>
          <button className="save-button" onClick={handleSave}>
            Save Aliases
          </button>
        </div>
      </div>
    </div>
  );
};

export default AliasAssign;
