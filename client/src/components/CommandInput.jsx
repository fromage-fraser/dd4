import React, { useState } from 'react';
import './CommandInput.css';

/**
 * CommandInput component for entering MUD commands
 * Supports command history navigation with arrow keys
 */
function CommandInput({ onSubmit, connected }) {
  const [command, setCommand] = useState('');
  const [history, setHistory] = useState([]);
  const [historyIndex, setHistoryIndex] = useState(-1);

  const handleSubmit = (e) => {
    e.preventDefault();
    
    if (!command.trim()) return;
    
    // Add to history
    setHistory(prev => [...prev, command]);
    setHistoryIndex(-1);
    
    // Send command
    onSubmit(command);
    
    // Clear input
    setCommand('');
  };

  const handleKeyDown = (e) => {
    if (e.key === 'ArrowUp') {
      e.preventDefault();
      if (history.length > 0) {
        const newIndex = historyIndex === -1 
          ? history.length - 1 
          : Math.max(0, historyIndex - 1);
        setHistoryIndex(newIndex);
        setCommand(history[newIndex]);
      }
    } else if (e.key === 'ArrowDown') {
      e.preventDefault();
      if (historyIndex !== -1) {
        const newIndex = historyIndex + 1;
        if (newIndex >= history.length) {
          setHistoryIndex(-1);
          setCommand('');
        } else {
          setHistoryIndex(newIndex);
          setCommand(history[newIndex]);
        }
      }
    }
  };

  return (
    <form className="command-input" onSubmit={handleSubmit}>
      <input
        type="text"
        value={command}
        onChange={(e) => setCommand(e.target.value)}
        onKeyDown={handleKeyDown}
        placeholder={connected ? "Enter command..." : "Not connected"}
        disabled={!connected}
        autoFocus
      />
      <button type="submit" disabled={!connected || !command.trim()}>
        Send
      </button>
    </form>
  );
}

export default CommandInput;
