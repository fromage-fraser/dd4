import React from 'react';
import './Compass.css';

/**
 * Compass component for directional navigation
 * Displays available exits and allows click-to-move
 */
function Compass({ exits = [], onMove }) {
  const directions = ['north', 'northeast', 'east', 'southeast', 'south', 'southwest', 'west', 'northwest', 'up', 'down'];
  
  const isExitAvailable = (direction) => {
    return exits.some(exit => exit.toLowerCase() === direction.toLowerCase());
  };

  const getDirectionClass = (direction) => {
    const available = isExitAvailable(direction);
    return `compass-direction ${direction} ${available ? 'available' : 'unavailable'}`;
  };

  const handleDirectionClick = (direction) => {
    if (isExitAvailable(direction)) {
      onMove(direction);
    }
  };

  return (
    <div className="compass">
      <h3>Navigation</h3>
      <div className="compass-grid">
        <button 
          className={getDirectionClass('northwest')}
          onClick={() => handleDirectionClick('northwest')}
          disabled={!isExitAvailable('northwest')}
        >
          NW
        </button>
        <button 
          className={getDirectionClass('north')}
          onClick={() => handleDirectionClick('north')}
          disabled={!isExitAvailable('north')}
        >
          N
        </button>
        <button 
          className={getDirectionClass('northeast')}
          onClick={() => handleDirectionClick('northeast')}
          disabled={!isExitAvailable('northeast')}
        >
          NE
        </button>
        
        <button 
          className={getDirectionClass('west')}
          onClick={() => handleDirectionClick('west')}
          disabled={!isExitAvailable('west')}
        >
          W
        </button>
        <button 
          className={getDirectionClass('up')}
          onClick={() => handleDirectionClick('up')}
          disabled={!isExitAvailable('up')}
        >
          U
        </button>
        <button 
          className={getDirectionClass('east')}
          onClick={() => handleDirectionClick('east')}
          disabled={!isExitAvailable('east')}
        >
          E
        </button>
        
        <button 
          className={getDirectionClass('southwest')}
          onClick={() => handleDirectionClick('southwest')}
          disabled={!isExitAvailable('southwest')}
        >
          SW
        </button>
        <button 
          className={getDirectionClass('down')}
          onClick={() => handleDirectionClick('down')}
          disabled={!isExitAvailable('down')}
        >
          D
        </button>
        <button 
          className={getDirectionClass('southeast')}
          onClick={() => handleDirectionClick('southeast')}
          disabled={!isExitAvailable('southeast')}
        >
          SE
        </button>
      </div>
    </div>
  );
}

export default Compass;
