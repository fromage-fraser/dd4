import React, { useEffect, useState } from 'react';
import './RoomDisplay.css';

/**
 * RoomDisplay component shows the current room information
 * including name, description, and visible exits.
 */
function RoomDisplay({ room }) {
  const { 
    name = 'Unknown Location', 
    description = '', 
    exits = []
  } = room;

  const [minimized, setMinimized] = useState(() => {
    try {
      const v = localStorage.getItem('roomDisplayMinimized');
      return v === '1';
    } catch (e) {
      return false;
    }
  });

  useEffect(() => {
    try {
      localStorage.setItem('roomDisplayMinimized', minimized ? '1' : '0');
    } catch (e) {
      // ignore
    }
  }, [minimized]);

  // Extract direction names from exit objects or use strings directly
  const exitDirections = exits.map(exit => 
    typeof exit === 'object' ? exit.dir : exit
  );

  return (
    <div className={`room-display ${minimized ? 'minimized' : ''}`}>
      <div className="room-header">
        <h2 className="room-name">{name}</h2>
        <button
          className="minimize-toggle"
          aria-pressed={minimized}
          title={minimized ? 'Restore room display' : 'Minimize room display'}
          onClick={() => setMinimized(!minimized)}
        >
          {minimized ? '+' : '−'}
        </button>
      </div>

      {!minimized && (
        <>
          {description && <p className="room-description">{description}</p>}
          {exits.length > 0 && (
            <div className="room-exits">
              <span className="exits-label">Obvious exits:</span>
              <span className="exits-list">{exitDirections.join(', ')}</span>
            </div>
          )}
        </>
      )}
    </div>
  );
}

export default RoomDisplay;

