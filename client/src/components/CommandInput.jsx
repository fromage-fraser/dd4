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
  const [lastSentSelected, setLastSentSelected] = useState(false);
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
    // If input is empty, repeat last non-empty command from history (if any)
    let toSend = command;

    if (!toSend.trim()) {
      if (history.length > 0) {
        toSend = history[history.length - 1];
        // show the repeated command in the input so the user can see/edit it
        setCommand(toSend);
      } else {
        // nothing in history, send an empty string (useful for login prompts)
        toSend = '';
      }
    } else {
      // Non-empty: store in history
      setHistory((prev) => [...prev, toSend]);
      setHistoryIndex(-1);
    }

    // Send the command (may be empty string or repeated history entry)
    onSubmit(toSend);

    // Keep the last command visible in the input and select it so
    // typing replaces it and Enter will resend it. On mobile, we
    // still blur to dismiss the keyboard instead of selecting.
    if (window.innerWidth <= 768) {
      inputRef.current?.blur();
    } else {
      // Focus and select the text so it's highlighted
      inputRef.current?.focus();
      try {
        inputRef.current?.select();
        setLastSentSelected(true);
      } catch (err) {
        // ignore if selection fails
      }
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
        onChange={(e) => {
          // If the last-sent command was selected, typing should replace it
          if (lastSentSelected) setLastSentSelected(false);
          setCommand(e.target.value);
        }}
        onKeyDown={handleKeyDown}
        placeholder={connected ? "Enter command..." : "Not connected"}
        disabled={!connected}
        autoComplete="off"
        autoCorrect="off"
        autoCapitalize="off"
        spellCheck="false"
      />
      <button type="submit" disabled={!connected}>
        Send
      </button>
    </form>
  );
}

export default CommandInput;
