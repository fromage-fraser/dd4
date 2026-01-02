import React from 'react';
import './RoomDisplay.css';

/**
 * RoomDisplay component shows the current room information
 * including name, description, and visible exits
 */
function RoomDisplay({ room }) {
  const { name = 'Unknown Location', description = '', exits = [] } = room;

  // Extract direction names from exit objects or use strings directly
  const exitDirections = exits.map(exit => 
    typeof exit === 'object' ? exit.dir : exit
  );

  return (
    <div className="room-display">
      <h2 className="room-name">{name}</h2>
      {description && <p className="room-description">{description}</p>}
      {exits.length > 0 && (
        <div className="room-exits">
          <span className="exits-label">Obvious exits:</span>
          <span className="exits-list">{exitDirections.join(', ')}</span>
        </div>
      )}
    </div>
  );
}

export default RoomDisplay;
