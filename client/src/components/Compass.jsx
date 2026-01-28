import React, { useState } from 'react';
import './Compass.css';

/**
 * Compass component for directional navigation
 * Displays available exits and allows click-to-move
 * Supports 6 directions: N, E, S, W, U, D
 * Handles doors (open/closed/locked) with interaction options
 * Shows map button when area map is available
 */
function Compass({ exits = [], onMove, onCommand, hasMap = false, onShowMap }) {
  const [selectedExit, setSelectedExit] = useState(null);
  const directions = ['north', 'east', 'south', 'west', 'up', 'down'];
  
  const getExit = (direction) => {
    if (Array.isArray(exits) && exits.length > 0 && typeof exits[0] === 'object') {
      // New format: array of objects with door info
      return exits.find(exit => exit.dir && exit.dir.toLowerCase() === direction.toLowerCase());
    } else if (Array.isArray(exits)) {
      // Old format: array of strings
      return exits.includes(direction) || exits.includes(direction.toLowerCase()) 
        ? { dir: direction, hasDoor: false } 
        : null;
    }
    return null;
  };

  const isExitAvailable = (direction) => {
    return getExit(direction) != null; // Checks for both null and undefined
  };

  const getDoorStatus = (exitData) => {
    if (!exitData || !exitData.hasDoor) return null;
    if (exitData.isLocked) return 'locked';
    if (exitData.isClosed) return 'closed';
    return 'open';
  };

  const getDoorIcon = (exitData) => {
    const status = getDoorStatus(exitData);
    if (!status) return '';
    switch (status) {
      case 'locked': return '🔒';
      case 'closed': return '🚪';
      case 'open': return '🚪';
      default: return '';
    }
  };

  const getDirectionClass = (direction) => {
    const exitData = getExit(direction);
    const available = exitData != null; // Checks for both null and undefined
    if (!available) {
      return `compass-direction ${direction}`;
    }
    const doorStatus = getDoorStatus(exitData);
    let statusClass = '';
    if (doorStatus === 'locked') statusClass = ' door-locked';
    else if (doorStatus === 'closed') statusClass = ' door-closed';
    else if (doorStatus === 'open') statusClass = ' door-open';
    return `compass-direction ${direction} available${statusClass}`;
  };

  const handleDirectionClick = (direction) => {
    const exitData = getExit(direction);
    if (exitData) {
      const doorStatus = getDoorStatus(exitData);
      if (doorStatus && doorStatus !== 'open') {
        // Door exists and is not open, show door options
        setSelectedExit(exitData);
      } else {
        // No door or door is open, move normally
        onMove(direction);
        setSelectedExit(null);
      }
    }
  };

  const handleDoorAction = (action, exitData) => {
    if (!onCommand || !exitData) return;
    
    const direction = exitData.dir;
    let command;
    
    switch (action) {
      case 'open':
        command = `open ${direction}`;
        break;
      case 'close':
        command = `close ${direction}`;
        break;
      case 'unlock':
        command = `unlock ${direction}`;
        break;
      case 'pick':
        command = `pick ${direction}`;
        break;
      case 'bash':
        command = `bash ${direction}`;
        break;
      default:
        return;
    }
    
    onCommand(command);
    setSelectedExit(null);
  };

  return (
    <div className="compass">
      <div className="compass-header">
        <h3>Navigation</h3>
        {onCommand && (
          <button
            className="scan-button"
            onClick={() => onCommand('scan')}
            title="Scan surroundings"
          >
            🔎
          </button>
        )}
        {hasMap && (
          <button 
            className="map-button" 
            onClick={onShowMap}
            title="View area map"
          >
            🗺️
          </button>
        )}
      </div>
      <div className="compass-grid">
        {/* Top row: Up and North */}
        <button 
          className={getDirectionClass('up')}
          onClick={() => handleDirectionClick('up')}
          disabled={!isExitAvailable('up')}
          title={getDoorStatus(getExit('up')) ? `Door: ${getDoorStatus(getExit('up'))}` : ''}
        >
          <span className="dir-label">U</span>
          <span className="door-icon">{getDoorIcon(getExit('up'))}</span>
        </button>
        <button 
          className={getDirectionClass('north')}
          onClick={() => handleDirectionClick('north')}
          disabled={!isExitAvailable('north')}
          title={getDoorStatus(getExit('north')) ? `Door: ${getDoorStatus(getExit('north'))}` : ''}
        >
          <span className="dir-label">N</span>
          <span className="door-icon">{getDoorIcon(getExit('north'))}</span>
        </button>
        <div className="compass-spacer"></div>
        
        {/* Middle row: West, Center, East */}
        <button 
          className={getDirectionClass('west')}
          onClick={() => handleDirectionClick('west')}
          disabled={!isExitAvailable('west')}
          title={getDoorStatus(getExit('west')) ? `Door: ${getDoorStatus(getExit('west'))}` : ''}
        >
          <span className="dir-label">W</span>
          <span className="door-icon">{getDoorIcon(getExit('west'))}</span>
        </button>
        <button 
          className="compass-center recall-btn"
          onClick={() => onCommand && onCommand('recall')}
          title="Recall to your base of operations"
        >
          <span className="recall-icon">🏠</span>
        </button>
        <button 
          className={getDirectionClass('east')}
          onClick={() => handleDirectionClick('east')}
          disabled={!isExitAvailable('east')}
          title={getDoorStatus(getExit('east')) ? `Door: ${getDoorStatus(getExit('east'))}` : ''}
        >
          <span className="dir-label">E</span>
          <span className="door-icon">{getDoorIcon(getExit('east'))}</span>
        </button>
        
        {/* Bottom row: Down and South */}
        <button 
          className={getDirectionClass('down')}
          onClick={() => handleDirectionClick('down')}
          disabled={!isExitAvailable('down')}
          title={getDoorStatus(getExit('down')) ? `Door: ${getDoorStatus(getExit('down'))}` : ''}
        >
          <span className="dir-label">D</span>
          <span className="door-icon">{getDoorIcon(getExit('down'))}</span>
        </button>
        <button 
          className={getDirectionClass('south')}
          onClick={() => handleDirectionClick('south')}
          disabled={!isExitAvailable('south')}
          title={getDoorStatus(getExit('south')) ? `Door: ${getDoorStatus(getExit('south'))}` : ''}
        >
          <span className="dir-label">S</span>
          <span className="door-icon">{getDoorIcon(getExit('south'))}</span>
        </button>
        <div className="compass-spacer"></div>
      </div>
      
      {/* Door interaction panel */}
      {selectedExit && selectedExit.hasDoor && (
        <div className="door-actions-panel">
          <h4>Door: {selectedExit.keyword || 'door'}</h4>
          <div className="door-status">
            Status: <span className={`status-${getDoorStatus(selectedExit)}`}>
              {getDoorStatus(selectedExit).toUpperCase()}
            </span>
          </div>
          <div className="door-actions">
            {selectedExit.isClosed && !selectedExit.isLocked && (
              <button className="door-action-btn" onClick={() => handleDoorAction('open', selectedExit)}>
                🚪 Open
              </button>
            )}
            {!selectedExit.isClosed && (
              <button className="door-action-btn" onClick={() => handleDoorAction('close', selectedExit)}>
                🚪 Close
              </button>
            )}
            {selectedExit.isLocked && (
              <>
                <button className="door-action-btn" onClick={() => handleDoorAction('unlock', selectedExit)}>
                  🔓 Unlock
                </button>
                {!selectedExit.isPickproof && (
                  <button className="door-action-btn pick" onClick={() => handleDoorAction('pick', selectedExit)}>
                    🔧 Pick Lock
                  </button>
                )}
                <button className="door-action-btn bash" onClick={() => handleDoorAction('bash', selectedExit)}>
                  💥 Bash
                </button>
              </>
            )}
            <button onClick={() => setSelectedExit(null)} className="door-panel-close">
              Cancel
            </button>
          </div>
        </div>
      )}
    </div>
  );
}

export default Compass;
