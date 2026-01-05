import React from 'react';
import './CharacterInfo.css';

/**
 * Character information display component
 * 
 * Shows real-time character vitals (HP/Mana/Move) with progress bars,
 * character metadata (name, level, class, race), and active affects/buffs.
 * Updates via GMCP Char.Vitals and Char.Status messages from server.
 */
function CharacterInfo({ vitals, status }) {
    if (!vitals || !status) {
        return (
            <div className="character-info">
                <div className="char-loading">Waiting for character data...</div>
            </div>
        );
    }
    
    const hpPercent = vitals.maxhp > 0 ? (vitals.hp / vitals.maxhp) * 100 : 0;
    const manaPercent = vitals.maxmana > 0 ? (vitals.mana / vitals.maxmana) * 100 : 0;
    const movePercent = vitals.maxmove > 0 ? (vitals.move / vitals.maxmove) * 100 : 0;
    
    // Color-code HP based on percentage
    const getHpColor = (percent) => {
        if (percent > 75) return '#4caf50'; // Green
        if (percent > 50) return '#ffeb3b'; // Yellow
        if (percent > 25) return '#ff9800'; // Orange
        return '#f44336'; // Red
    };
    
    return (
        <div className="character-info">
            <div className="char-header">
                <h3 className="char-name">{status.name}</h3>
                <div className="char-meta">
                    <span className="char-level">Lvl {vitals.level}</span>
                    <span className="char-race">{status.race}</span>
                    <span className="char-class">{status.class}</span>
                </div>
                {vitals.tnl > 0 && (
                    <div className="exp-bar-container">
                        <div 
                            className="exp-bar-fill" 
                            style={{
                                width: `${Math.max(0, Math.min(100, ((vitals.exp - (vitals.expLevelStart || 0)) / ((vitals.exp + vitals.tnl) - (vitals.expLevelStart || 0))) * 100))}%`
                            }} 
                        />
                        <span className="exp-bar-text">{vitals.tnl.toLocaleString()} TNL</span>
                    </div>
                )}
                {status.position && status.position !== 'standing' && (
                    <div className="char-position">{status.position}</div>
                )}
            </div>
            
            <div className="vitals-bars">
                <div className="vital-bar hp">
                    <label>HP</label>
                    <div className="bar-container">
                        <div 
                            className="bar-fill" 
                            style={{
                                width: `${Math.max(0, Math.min(100, hpPercent))}%`,
                                backgroundColor: getHpColor(hpPercent)
                            }} 
                        />
                        <span className="bar-text">{vitals.hp}/{vitals.maxhp}</span>
                    </div>
                </div>
                
                <div className="vital-bar mana">
                    <label>Mana</label>
                    <div className="bar-container">
                        <div 
                            className="bar-fill" 
                            style={{width: `${Math.max(0, Math.min(100, manaPercent))}%`}} 
                        />
                        <span className="bar-text">{vitals.mana}/{vitals.maxmana}</span>
                    </div>
                </div>
                
                <div className="vital-bar move">
                    <label>Move</label>
                    <div className="bar-container">
                        <div 
                            className="bar-fill" 
                            style={{width: `${Math.max(0, Math.min(100, movePercent))}%`}} 
                        />
                        <span className="bar-text">{vitals.move}/{vitals.maxmove}</span>
                    </div>
                </div>
            </div>
            
            <div className="char-stats">
                <div className="stat-item">
                    <span className="stat-label">💪</span>
                    <span className="stat-value">{vitals.pracPhysical || 0}</span>
                </div>
                <div className="stat-item">
                    <span className="stat-label">🧠</span>
                    <span className="stat-value">{vitals.pracIntellectual || 0}</span>
                </div>
            </div>
            
            {status.affects && status.affects.length > 0 && (
                <div className="active-affects">
                    <h4>Active Effects</h4>
                    <div className="affect-list">
                        {status.affects.map((aff, idx) => {
                            const isExpiring = aff.duration <= 5;
                            return (
                                <span 
                                    key={idx} 
                                    className={`affect-badge ${isExpiring ? 'expiring' : ''}`}
                                    title={`${aff.name} (${aff.duration} rounds remaining)`}
                                >
                                    {aff.name}
                                    <span className="affect-duration">({aff.duration})</span>
                                </span>
                            );
                        })}
                    </div>
                </div>
            )}
        </div>
    );
}

export default CharacterInfo;
