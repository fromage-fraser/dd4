import React, { useState, useEffect } from 'react';
import './ConfigModal.css';

/**
 * ConfigModal component - Settings modal for toggling character config options
 * 
 * Displays all 16 config flags (autoexit, autoloot, etc.) as toggles.
 * Sends config +/-<option> commands to server and enforces 1-second cooldown
 * between toggles. State syncs via GMCP Char.Config messages.
 */
function ConfigModal({ configOptions, onCommand, onClose, connected }) {
  const [lastToggleTime, setLastToggleTime] = useState(0);
  const [pendingOption, setPendingOption] = useState(null);

  // Config options with labels and descriptions
  const configItems = [
    { key: 'autoexit', label: 'Auto Exit', description: 'Automatically see exits' },
    { key: 'autoloot', label: 'Auto Loot', description: 'Automatically loot corpses' },
    { key: 'autosac', label: 'Auto Sacrifice', description: 'Automatically sacrifice corpses' },
    { key: 'autocoin', label: 'Auto Coin', description: 'Automatically take coin from corpses' },
    { key: 'autowield', label: 'Auto Wield', description: 'Automatically wield weapons in combat' },
    { key: 'autolevel', label: 'Auto Level', description: 'Automatically level with enough XP' },
    { key: 'blank', label: 'Blank Line', description: 'Blank line before prompt' },
    { key: 'brief', label: 'Brief Mode', description: 'See brief descriptions only' },
    { key: 'combine', label: 'Combine Items', description: 'See object lists in combined format' },
    { key: 'prompt', label: 'Prompt', description: 'Display command prompt' },
    { key: 'ansi', label: 'ANSI Color', description: 'Receive color codes' },
    { key: 'gag', label: 'Gag Combat', description: 'Gag some combat messages' },
    { key: 'quiet', label: 'Quiet Mode', description: 'Don\'t receive broadcast messages' },
    { key: 'alloweq', label: 'Allow Equipment', description: 'Players can see your equipment' },
    { key: 'telnetga', label: 'Telnet GA', description: 'Receive telnet GA sequence' },
    { key: 'tell', label: 'Receive Tells', description: 'Receive tell messages' }
  ];

  // Clear cooldown when GMCP confirms toggle
  useEffect(() => {
    if (pendingOption && configOptions) {
      // Clear pending if server has responded
      setPendingOption(null);
    }
  }, [configOptions, pendingOption]);

  const handleToggle = (option) => {
    if (!connected) return;

    const now = Date.now();
    const timeSinceLastToggle = now - lastToggleTime;

    // Enforce 1-second cooldown
    if (timeSinceLastToggle < 1000) {
      return;
    }

    // Get current value (default to 0 if not set)
    const currentValue = configOptions?.[option] || 0;
    const newValue = currentValue ? 0 : 1;

    // Send config command
    const command = `config ${newValue ? '+' : '-'}${option}`;
    onCommand(command);

    // Set cooldown
    setLastToggleTime(now);
    setPendingOption(option);
  };

  const isOnCooldown = () => {
    const now = Date.now();
    return (now - lastToggleTime) < 1000;
  };

  const getCooldownRemaining = () => {
    const now = Date.now();
    const remaining = 1000 - (now - lastToggleTime);
    return Math.max(0, Math.ceil(remaining / 1000));
  };

  if (!configOptions) {
    return (
      <div className="modal-overlay" onClick={onClose}>
        <div className="config-modal-content" onClick={(e) => e.stopPropagation()}>
          <div className="config-modal-header">
            <h2>Settings</h2>
            <button className="close-button" onClick={onClose}>✕</button>
          </div>
          <div className="config-modal-body">
            <p className="config-loading">Loading configuration...</p>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="config-modal-content" onClick={(e) => e.stopPropagation()}>
        <div className="config-modal-header">
          <h2>Settings</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>
        <div className="config-modal-body">
          {isOnCooldown() && (
            <div className="config-cooldown-notice">
              Please wait {getCooldownRemaining()}s between changes...
            </div>
          )}
          <div className="config-grid">
            {configItems.map(item => {
              const isEnabled = configOptions[item.key] === 1;
              const isPending = pendingOption === item.key;
              const disabled = !connected || isOnCooldown();

              return (
                <div key={item.key} className="config-item">
                  <div className="config-item-header">
                    <label className="config-label">{item.label}</label>
                    <p className="config-description">{item.description}</p>
                  </div>
                  <button
                    className={`config-toggle-switch ${isEnabled ? 'on' : 'off'} ${isPending ? 'pending' : ''}`}
                    onClick={() => handleToggle(item.key)}
                    disabled={disabled}
                    title={disabled ? 'Cooldown active' : (isEnabled ? 'Click to turn OFF' : 'Click to turn ON')}
                  >
                    <span className="toggle-slider"></span>
                  </button>
                </div>
              );
            })}
          </div>
        </div>
        <div className="config-modal-footer">
          <button className="config-close-btn" onClick={onClose}>Close</button>
        </div>
      </div>
    </div>
  );
}

export default ConfigModal;
