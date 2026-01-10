import React from 'react';
import { parseAnsiToHtml } from '../utils/ansiParser';
import './EnemyStatus.css';

/**
 * EnemyStatus component displays real-time enemy combat status
 * Shows all opponents with health bars, level, and highlights primary target
 * Updates in real-time via GMCP Char.Enemies messages
 */
function EnemyStatus({ enemies }) {
  // Don't render if no enemies
  if (!enemies || enemies.length === 0) {
    return null;
  }

  const getPercentage = (current, max) => {
    return max > 0 ? Math.round((current / max) * 100) : 0;
  };

  const getHealthColor = (percentage) => {
    if (percentage > 60) return '#4caf50';
    if (percentage > 30) return '#ff9800';
    return '#f44336';
  };

  return (
    <div className="enemy-status">
      <h3>Enemies</h3>
      
      {enemies.map((enemy) => {
        const hpPercentage = getPercentage(enemy.hp, enemy.maxhp);
        const isPrimary = enemy.isPrimary === true;
        
        return (
          <div 
            key={enemy.id} 
            className={`enemy-item ${isPrimary ? 'primary-target' : ''}`}
          >
            <div className="enemy-header">
              <span 
                className="enemy-name"
                dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(enemy.name) }}
              />
              <span className="enemy-level">Lvl {enemy.level}</span>
            </div>
            
            <div className="enemy-hp">
              <div className="hp-label">
                <span>{hpPercentage}%</span>
              </div>
              <div className="progress-bar">
                <div 
                  className="progress-fill"
                  style={{
                    width: `${hpPercentage}%`,
                    backgroundColor: getHealthColor(hpPercentage)
                  }}
                />
              </div>
            </div>
          </div>
        );
      })}
    </div>
  );
}

export default EnemyStatus;
