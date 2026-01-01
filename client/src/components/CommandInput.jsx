import React, { useState, useRef, useEffect } from 'react';
import './CommandInput.css';

/**
 * CommandInput component for entering MUD commands
 * 
 * Intent: Provide touch-optimized command entry for mobile and desktop
 * 
 * Features:
 * - Command history navigation with arrow keys
 * - Mobile-friendly: 16px minimum font size prevents iOS auto-zoom
 * - Sticky positioning on mobile for keyboard accessibility
 * - Auto-blur after submit to dismiss mobile keyboard
 * - Touch-optimized Send button (min 48px tap target)
 * 
 * Preconditions: Must receive onSubmit callback and connected status
 * Postconditions: Sends commands to server, manages input history
 */
function CommandInput({ onSubmit, connected }) {
  const [command, setCommand] = useState('');
  const [history, setHistory] = useState([]);
  const [historyIndex, setHistoryIndex] = useState(-1);
  const inputRef = useRef(null);

  // On mobile, prevent viewport scroll when keyboard appears
  useEffect(() => {
    const handleResize = () => {
      // Scroll input into view when keyboard appears on mobile
      if (document.activeElement === inputRef.current) {
        setTimeout(() => {
          inputRef.current?.scrollIntoView({ behavior: 'smooth', block: 'center' });
        }, 100);
      }
    };

    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, []);

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
    
    // On mobile, blur to dismiss keyboard after sending
    // User can tap input again to type next command
    if (window.innerWidth <= 768) {
      inputRef.current?.blur();
    }
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
        ref={inputRef}
        type="text"
        value={command}
        onChange={(e) => setCommand(e.target.value)}
        onKeyDown={handleKeyDown}
        placeholder={connected ? "Enter command..." : "Not connected"}
        disabled={!connected}
        autoComplete="off"
        autoCorrect="off"
        autoCapitalize="off"
        spellCheck="false"
      />
      <button type="submit" disabled={!connected || !command.trim()}>
        Send
      </button>
    </form>
  );
}

export default CommandInput;
