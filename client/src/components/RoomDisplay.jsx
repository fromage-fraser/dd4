import React, { useEffect, useState } from 'react';
import './RoomDisplay.css';
import { parseAnsiToHtml } from '../utils/ansiParser';
import { canSeeRoom, getVisibilityMessage } from '../utils/visibilityHelper';

/**
 * RoomDisplay component shows the current room information
 * including name, description, and visible exits.
 * 
 * Intent: Display room information with visibility restrictions for blind/asleep characters
 * Responsibilities: Show room name, description, exits when character can see; show appropriate
 *                   messages when character is blind or asleep
 * Constraints: Must check status before rendering room content; animate transitions
 */
function RoomDisplay({ room, status }) {
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

  // Check if character can see the room
  const canSee = canSeeRoom(status);
  const visibilityMsg = getVisibilityMessage(status);

  // If character cannot see, show visibility restriction message
  if (!canSee && visibilityMsg) {
    return (
      <div key="room-restricted" className={`room-display ${minimized ? 'minimized' : ''} visibility-restricted fade-in`}>
        <div className="room-header">
          <h2 className="room-name">???</h2>
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
          <p className="visibility-message">{visibilityMsg}</p>
        )}
      </div>
    );
  }

  return (
    <div key="room-visible" className={`room-display ${minimized ? 'minimized' : ''} fade-in`}>
      <div className="room-header">
        <h2 className="room-name" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(name) }} />
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
          {description && <p className="room-description" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(description) }} />}
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

