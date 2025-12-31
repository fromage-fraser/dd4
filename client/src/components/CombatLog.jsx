import React, { useEffect, useRef } from 'react';
import './CombatLog.css';
import './ansi.css';
import { parseAnsiToHtml } from '../utils/ansiParser';

/**
 * Intent: Display scrolling message history with ANSI color codes converted to HTML/CSS
 * 
 * Responsibilities:
 * - Parse ANSI escape sequences from MUD output
 * - Convert color codes to styled HTML spans
 * - Auto-scroll to show latest messages
 * - Apply message type classes for additional styling
 * 
 * Inputs: messages array with {text, type, timestamp}
 * Outputs: Rendered HTML with ANSI colors and formatting
 */
function CombatLog({ messages }) {
  const logRef = useRef(null);

  // Auto-scroll to bottom when new messages arrive
  useEffect(() => {
    if (logRef.current) {
      logRef.current.scrollTop = logRef.current.scrollHeight;
    }
  }, [messages]);

  const getMessageClass = (type) => {
    return `log-message ${type}`;
  };

  return (
    <div className="combat-log" ref={logRef}>
      {messages.length === 0 && (
        <div className="log-message system">Waiting for connection...</div>
      )}
      {messages.map((msg, index) => (
        <div 
          key={index} 
          className={getMessageClass(msg.type)}
          dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(msg.text) }}
        />
      ))}
    </div>
  );
}

export default CombatLog;
