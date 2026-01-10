import React, { useState, useRef, useEffect } from 'react';
import './Compass.css';

/**
 * Compass component for directional navigation
 * Displays available exits and allows click-to-move
 * Supports 6 directions: N, E, S, W, U, D
 * Handles doors (open/closed/locked) with interaction options
 * Shows map button when area map is available
 */
function Compass({ exits = [], onMove, onCommand, hasMap = false, onShowMap }) {
  // selectedExit: null or { exitData, left, top }
  const [selectedExit, setSelectedExit] = useState(null);
  const compassRef = useRef(null);
  const panelRef = useRef(null);

  // Close panel when clicking outside it. Use 'click' so button onClick runs first.
  useEffect(() => {
    if (!selectedExit) return undefined;
    const handler = (ev) => {
      if (!panelRef.current) return;
      if (!panelRef.current.contains(ev.target)) {
        setSelectedExit(null);
      }
    };
    document.addEventListener('click', handler);
    document.addEventListener('touchstart', handler);
    return () => {
      document.removeEventListener('click', handler);
      document.removeEventListener('touchstart', handler);
    };
  }, [selectedExit]);
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

  const handleDirectionClick = (direction, evt) => {
    const exitData = getExit(direction);
    if (exitData) {
      const doorStatus = getDoorStatus(exitData);
      if (doorStatus && doorStatus !== 'open') {
        // Door exists and is not open, show door options anchored to button
        if (compassRef.current && evt && evt.currentTarget) {
          const compassRect = compassRef.current.getBoundingClientRect();
          const btnRect = evt.currentTarget.getBoundingClientRect();
          const left = btnRect.left - compassRect.left + (btnRect.width / 2);
          const top = btnRect.top - compassRect.top + (btnRect.height / 2);
          setSelectedExit({ exitData, left, top });
        } else {
          setSelectedExit({ exitData, left: '50%', top: '50%' });
        }
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
    <div className="compass" ref={compassRef}>
      <div className="compass-header">
        <h3>Navigation</h3>
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
          onClick={(e) => handleDirectionClick('up', e)}
          disabled={!isExitAvailable('up')}
          title={getDoorStatus(getExit('up')) ? `Door: ${getDoorStatus(getExit('up'))}` : ''}
        >
          <span className="dir-label">U</span>
          <span className="door-icon">{getDoorIcon(getExit('up'))}</span>
        </button>
        <button 
          className={getDirectionClass('north')}
          onClick={(e) => handleDirectionClick('north', e)}
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
          onClick={(e) => handleDirectionClick('west', e)}
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
          onClick={(e) => handleDirectionClick('east', e)}
          disabled={!isExitAvailable('east')}
          title={getDoorStatus(getExit('east')) ? `Door: ${getDoorStatus(getExit('east'))}` : ''}
        >
          <span className="dir-label">E</span>
          <span className="door-icon">{getDoorIcon(getExit('east'))}</span>
        </button>
        
        {/* Bottom row: Down and South */}
        <button 
          className={getDirectionClass('down')}
          onClick={(e) => handleDirectionClick('down', e)}
          disabled={!isExitAvailable('down')}
          title={getDoorStatus(getExit('down')) ? `Door: ${getDoorStatus(getExit('down'))}` : ''}
        >
          <span className="dir-label">D</span>
          <span className="door-icon">{getDoorIcon(getExit('down'))}</span>
        </button>
        <button 
          className={getDirectionClass('south')}
          onClick={(e) => handleDirectionClick('south', e)}
          disabled={!isExitAvailable('south')}
          title={getDoorStatus(getExit('south')) ? `Door: ${getDoorStatus(getExit('south'))}` : ''}
        >
          <span className="dir-label">S</span>
          <span className="door-icon">{getDoorIcon(getExit('south'))}</span>
        </button>
        <div className="compass-spacer"></div>
      </div>
      
      {/* Door interaction panel */}
      {selectedExit && selectedExit.exitData && (
        (() => {
          const panelWidth = 260; // should match CSS min-width
          let left = selectedExit.left;
          let top = selectedExit.top;
          if (compassRef.current && typeof left === 'number') {
            const compassRect = compassRef.current.getBoundingClientRect();
            // constrain horizontally to keep panel inside compass
            if (left + panelWidth / 2 > compassRect.width) {
              left = compassRect.width - panelWidth / 2 - 8;
            } else if (left - panelWidth / 2 < 0) {
              left = panelWidth / 2 + 8;
            }
          }
          const style = {
            left: typeof left === 'number' ? `${left}px` : left,
            top: typeof top === 'number' ? `${top}px` : top,
            transform: 'translate(-50%, -110%)'
          };
          return (
            <div className="door-actions-panel" ref={panelRef} style={style}>
          <h4>Door: {selectedExit.exitData.keyword || 'door'}</h4>
          <div className="door-status">
            Status: <span className={`status-${getDoorStatus(selectedExit.exitData)}`}>
              {getDoorStatus(selectedExit.exitData).toUpperCase()}
            </span>
          </div>
          <div className="door-actions">
            {selectedExit.exitData.isClosed && !selectedExit.exitData.isLocked && (
              <button className="door-action-btn" onClick={() => handleDoorAction('open', selectedExit.exitData)}>
                🚪 Open
              </button>
            )}
            {!selectedExit.exitData.isClosed && (
              <button className="door-action-btn" onClick={() => handleDoorAction('close', selectedExit.exitData)}>
                🚪 Close
              </button>
            )}
            {selectedExit.exitData.isLocked && (
              <>
                <button className="door-action-btn" onClick={() => handleDoorAction('unlock', selectedExit.exitData)}>
                  🔓 Unlock
                </button>
                {!selectedExit.exitData.isPickproof && (
                  <button className="door-action-btn pick" onClick={() => handleDoorAction('pick', selectedExit.exitData)}>
                    🔧 Pick Lock
                  </button>
                )}
                <button className="door-action-btn bash" onClick={() => handleDoorAction('bash', selectedExit.exitData)}>
                  💥 Bash
                </button>
              </>
            )}
            <button onClick={() => setSelectedExit(null)} className="door-panel-close">
              Cancel
            </button>
          </div>
        </div>
      );
    })()
      )}
    </div>
  );
}

export default Compass;
