import React from 'react';
import './Compass.css';

/**
 * Compass component for directional navigation
 * Displays available exits and allows click-to-move
 * Supports 6 directions: N, E, S, W, U, D
 */
function Compass({ exits = [], onMove }) {
  const directions = ['north', 'east', 'south', 'west', 'up', 'down'];
  
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
        {/* Top row: Up and North */}
        <button 
          className={getDirectionClass('up')}
          onClick={() => handleDirectionClick('up')}
          disabled={!isExitAvailable('up')}
        >
          U
        </button>
        <button 
          className={getDirectionClass('north')}
          onClick={() => handleDirectionClick('north')}
          disabled={!isExitAvailable('north')}
        >
          N
        </button>
        <div className="compass-spacer"></div>
        
        {/* Middle row: West, Center, East */}
        <button 
          className={getDirectionClass('west')}
          onClick={() => handleDirectionClick('west')}
          disabled={!isExitAvailable('west')}
        >
          W
        </button>
        <div className="compass-center">
          <span>⊕</span>
        </div>
        <button 
          className={getDirectionClass('east')}
          onClick={() => handleDirectionClick('east')}
          disabled={!isExitAvailable('east')}
        >
          E
        </button>
        
        {/* Bottom row: Down and South */}
        <button 
          className={getDirectionClass('down')}
          onClick={() => handleDirectionClick('down')}
          disabled={!isExitAvailable('down')}
        >
          D
        </button>
        <button 
          className={getDirectionClass('south')}
          onClick={() => handleDirectionClick('south')}
          disabled={!isExitAvailable('south')}
        >
          S
        </button>
        <div className="compass-spacer"></div>
      </div>
    </div>
  );
}

export default Compass;
