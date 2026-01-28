import React from 'react';
import './VitalsBar.css';

/**
 * VitalsBar component displays character health, mana, and movement
 * with animated progress bars that update in real-time via GMCP
 */
function VitalsBar({ vitals }) {
  const { hp = 0, maxhp = 100, mana = 0, maxmana = 100, move = 0, maxmove = 100 } = vitals;

  const getPercentage = (current, max) => {
    return max > 0 ? (current / max) * 100 : 0;
  };

  const getHealthColor = (percentage) => {
    if (percentage > 60) return '#4caf50';
    if (percentage > 30) return '#ff9800';
    return '#f44336';
  };

  return (
    <div className="vitals-bar">
      <h3>Character Vitals</h3>
      
      <div className="vital-item">
        <div className="vital-label">
          <span>Health</span>
          <span>{hp} / {maxhp}</span>
        </div>
        <div className="progress-bar">
          <div 
            className="progress-fill"
            style={{
              width: `${getPercentage(hp, maxhp)}%`,
              backgroundColor: getHealthColor(getPercentage(hp, maxhp))
            }}
          />
        </div>
      </div>

      <div className="vital-item">
        <div className="vital-label">
          <span>Mana</span>
          <span>{mana} / {maxmana}</span>
        </div>
        <div className="progress-bar">
          <div 
            className="progress-fill"
            style={{
              width: `${getPercentage(mana, maxmana)}%`,
              backgroundColor: '#2196f3'
            }}
          />
        </div>
      </div>

      <div className="vital-item">
        <div className="vital-label">
          <span>Movement</span>
          <span>{move} / {maxmove}</span>
        </div>
        <div className="progress-bar">
          <div 
            className="progress-fill"
            style={{
              width: `${getPercentage(move, maxmove)}%`,
              backgroundColor: '#ffc107'
            }}
          />
        </div>
      </div>
    </div>
  );
}

export default VitalsBar;
