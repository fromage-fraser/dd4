import React, { useEffect, useRef } from 'react';
import './CombatLog.css';

/**
 * CombatLog component displays scrolling message history
 * with color-coded message types
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
        <div key={index} className={getMessageClass(msg.type)}>
          {msg.text}
        </div>
      ))}
    </div>
  );
}

export default CombatLog;
