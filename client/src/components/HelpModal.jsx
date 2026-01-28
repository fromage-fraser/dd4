import React from 'react';
import './HelpModal.css';

/**
 * Simple pop-out help modal for displaying server help text
 */
function HelpModal({ content, onClose }) {
  if (!content) return null;

  // Clean ANSI-ish codes for display
  const clean = content.replace(/\u001b\[[0-9;]*m/g, '').replace(/<[0-9]+>/g, '');

  return (
    <div className="help-overlay" onClick={onClose}>
      <div className="help-modal" onClick={(e) => e.stopPropagation()}>
        <div className="help-header">
          <h3>Help</h3>
          <button className="help-close" onClick={onClose}>✕</button>
        </div>
        <div className="help-body">
          <pre className="help-pre">{clean}</pre>
        </div>
      </div>
    </div>
  );
}

export default HelpModal;
