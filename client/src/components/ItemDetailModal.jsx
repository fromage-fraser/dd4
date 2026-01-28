import React from 'react';
import './ItemDetailModal.css';
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';

/**
 * Modal component for displaying detailed item identification information
 * 
 * Shows all item properties revealed by the identify spell including:
 * - Base stats (level, weight, material)
 * - Extra flags (glow, hum, magic, cursed, etc.)
 * - Ego enhancements (bloodlust, firebrand, balanced)
 * - Stat modifiers (affects)
 * - Class and alignment restrictions
 * 
 * Respects the identified status - unidentified items only show cosmetic flags
 */
function ItemDetailModal({ item, onClose }) {
    if (!item) return null;

    const getItemTypeIcon = (type) => {
        const typeMap = {
            1: '🔦', // Light
            2: '📜', // Scroll
            3: '🪄', // Wand
            4: '🪵', // Staff
            5: '⚔️', // Weapon
            8: '💎', // Treasure
            9: '🛡️', // Armor
            10: '🧪', // Potion
            15: '🎒', // Container
            19: '🍖', // Food
            20: '💰', // Money
            23: '🍺', // Drink container
            24: '🔑', // Key
            25: '🍞', // Food
        };
        return typeMap[type] || '📦';
    };

    const getFlagIcon = (flag) => {
        const flagIcons = {
            'GLOW': '✨',
            'HUM': '🎵',
            'DARK': '🌑',
            'EVIL': '😈',
            'INVIS': '👻',
            'MAGIC': '🔮',
            'NO_DROP': '🔒',
            'BLESSED': '🙏',
            'ANTI_GOOD': '⛔',
            'ANTI_EVIL': '⛔',
            'ANTI_NEUTRAL': '⛔',
            'VORPAL': '⚡',
            'SHARP': '🔪',
            'FLAMING': '🔥',
            'FROST': '❄️',
            'POISONED': '☠️',
            'CURSED': '💀',
        };
        return flagIcons[flag] || '🏷️';
    };

    const getAffectIcon = (location) => {
        const affectIcons = {
            'STR': '💪',
            'DEX': '🎯',
            'INT': '🧠',
            'WIS': '📖',
            'CON': '❤️',
            'HIT': '⚔️',
            'DAM': '💥',
            'AC': '🛡️',
            'HITROLL': '🎲',
            'DAMROLL': '🎲',
            'SAVES': '🍀',
            'MANA': '✨',
            'HP': '❤️',
            'MOVE': '👟',
        };
        return affectIcons[location] || '📊';
    };

    const getModifierColor = (modifier) => {
        if (modifier > 0) return 'positive';
        if (modifier < 0) return 'negative';
        return '';
    };

    return (
        <div className="modal-overlay" onClick={onClose}>
            <div className="modal-content" onClick={(e) => e.stopPropagation()}>
                <div className="modal-header">
                    <h3>
                        <span className="item-type-icon">{getItemTypeIcon(item.type)}</span>
                        <span dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.name) }} />
                    </h3>
                    <button className="close-button" onClick={onClose}>✕</button>
                </div>
                
                <div className="modal-body">
                    {/* Identification Status */}
                    <div className="detail-section">
                        <div className={`identification-badge ${item.identified ? 'identified' : 'unidentified'}`}>
                            {item.identified ? '✓ Identified' : '? Unidentified'}
                        </div>
                    </div>

                    {/* Base Item Info */}
                    <div className="detail-section">
                        <h4>Basic Information</h4>
                        <div className="info-grid">
                            <div className="info-item">
                                <span className="info-label">Level:</span>
                                <span className="info-value">{item.level}</span>
                            </div>
                            <div className="info-item">
                                <span className="info-label">Weight:</span>
                                <span className="info-value">{item.weight} lbs</span>
                            </div>
                            {item.identified && item.material && (
                                <div className="info-item">
                                    <span className="info-label">Material:</span>
                                    <span className="info-value">{item.material}</span>
                                </div>
                            )}
                            {item.itemType && (
                                <div className="info-item">
                                    <span className="info-label">Type:</span>
                                    <span className="info-value">{item.itemType}</span>
                                </div>
                            )}
                            {item.wearLocation && (
                                <div className="info-item">
                                    <span className="info-label">Worn:</span>
                                    <span className="info-value">{item.wearLocation}</span>
                                </div>
                            )}
                            {item.value !== undefined && (
                                <div className="info-item">
                                    <span className="info-label">Value:</span>
                                    <span className="info-value">{item.value} {item.valueCurrency || 'copper'}</span>
                                </div>
                            )}
                            {item.armorClass !== undefined && (
                                <div className="info-item">
                                    <span className="info-label">Armor Class:</span>
                                    <span className="info-value">{item.armorClass}</span>
                                </div>
                            )}
                            {item.damageDice && (
                                <div className="info-item">
                                    <span className="info-label">Damage:</span>
                                    <span className="info-value">{item.damageDice}</span>
                                </div>
                            )}
                        </div>
                    </div>

                    {/* Extra Flags */}
                    {item.extraFlags && item.extraFlags.length > 0 && (
                        <div className="detail-section">
                            <h4>Properties</h4>
                            <div className="flags-container">
                                {item.extraFlags.map((flag, idx) => (
                                    <span key={idx} className="flag-badge">
                                        {getFlagIcon(flag)} {flag.replace('_', ' ')}
                                    </span>
                                ))}
                            </div>
                        </div>
                    )}

                    {/* Ego Flags - Only shown if identified */}
                    {item.identified && item.egoFlags && item.egoFlags.length > 0 && (
                        <div className="detail-section">
                            <h4>Special Enhancements</h4>
                            <div className="flags-container">
                                {item.egoFlags.map((flag, idx) => (
                                    <span key={idx} className="ego-badge">
                                        ⭐ {flag.replace('_', ' ')}
                                    </span>
                                ))}
                            </div>
                        </div>
                    )}

                    {/* Stat Modifiers - Only shown if identified */}
                    {item.identified && item.affects && item.affects.length > 0 && (
                        <div className="detail-section">
                            <h4>Stat Modifiers</h4>
                            <div className="affects-list">
                                {item.affects.map((affect, idx) => (
                                    <div key={idx} className={`affect-item ${getModifierColor(affect.modifier)}`}>
                                        <span className="affect-icon">{getAffectIcon(affect.location)}</span>
                                        <span className="affect-location">{affect.location}</span>
                                        <span className="affect-modifier">
                                            {affect.modifier > 0 ? '+' : ''}{affect.modifier}
                                        </span>
                                    </div>
                                ))}
                            </div>
                        </div>
                    )}

                    {/* Class Restrictions - Only shown if identified */}
                    {item.identified && item.classRestrictions && item.classRestrictions.length > 0 && (
                        <div className="detail-section">
                            <h4>Class Restrictions</h4>
                            <div className="restrictions-container">
                                <p className="restriction-note">Cannot be used by:</p>
                                <div className="flags-container">
                                    {item.classRestrictions.map((cls, idx) => (
                                        <span key={idx} className="restriction-badge">
                                            🚫 {cls}
                                        </span>
                                    ))}
                                </div>
                            </div>
                        </div>
                    )}

                    {/* Alignment Restrictions - Only shown if identified */}
                    {item.identified && item.alignmentRestrictions && item.alignmentRestrictions.length > 0 && (
                        <div className="detail-section">
                            <h4>Alignment Restrictions</h4>
                            <div className="restrictions-container">
                                <p className="restriction-note">Cannot be used by:</p>
                                <div className="flags-container">
                                    {item.alignmentRestrictions.map((align, idx) => (
                                        <span key={idx} className="restriction-badge">
                                            🚫 {align}
                                        </span>
                                    ))}
                                </div>
                            </div>
                        </div>
                    )}

                    {/* Set Information */}
                    {item.setRarity && (
                        <div className="detail-section set-info">
                            <h4>📦 Set Item</h4>
                            <div className="set-rarity">
                                <span className={`rarity-badge ${item.setRarity.toLowerCase()}`}>{item.setRarity}</span> Set
                            </div>
                            {item.setName && (
                                <div className="set-name" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.setName) }} />
                            )}
                            {item.setLore && (
                                <div className="set-lore" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.setLore) }} />
                            )}
                            {item.setBonus && (
                                <div className="set-bonus">
                                    <strong>Set Bonus:</strong> Equip {item.setBonus.itemCount} items to gain <span className="positive">+{item.setBonus.value}</span> {item.setBonus.stat}
                                </div>
                            )}
                        </div>
                    )}

                    {/* Unidentified hint */}
                    {!item.identified && (
                        <div className="detail-section unidentified-hint">
                            <p>💡 Cast <strong>identify</strong> to reveal this item's full properties</p>
                        </div>
                    )}
                </div>

                <div className="modal-footer">
                    <button className="modal-button" onClick={onClose}>Close</button>
                </div>
            </div>
        </div>
    );
}

export default ItemDetailModal;
