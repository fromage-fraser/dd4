import React, { useState } from 'react';
import './CharacterSheet.css';
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';

function CharacterSheet({ inventory, equipment, onCommand, onClose, connected, onRefresh }) {
    const [selectedInventoryItem, setSelectedInventoryItem] = useState(null);
    const [selectedEquipmentSlot, setSelectedEquipmentSlot] = useState(null);
    const [inspectedItem, setInspectedItem] = useState(null);
    const [comparisonItems, setComparisonItems] = useState([]);

    const equipmentSlots = [
        { key: 'light', label: 'Light', icon: '🔦' },
        { key: 'head', label: 'Head', icon: '⛑️' },
        { key: 'neck_1', label: 'Neck 1', icon: '📿' },
        { key: 'neck_2', label: 'Neck 2', icon: '📿' },
        { key: 'body', label: 'Body', icon: '👕' },
        { key: 'about', label: 'About', icon: '🧥' },
        { key: 'waist', label: 'Waist', icon: '🥋' },
        { key: 'arms', label: 'Arms', icon: '🦾' },
        { key: 'wrist_l', label: 'L Wrist', icon: '⌚' },
        { key: 'wrist_r', label: 'R Wrist', icon: '⌚' },
        { key: 'hands', label: 'Hands', icon: '🧤' },
        { key: 'finger_l', label: 'L Finger', icon: '💍' },
        { key: 'finger_r', label: 'R Finger', icon: '💍' },
        { key: 'legs', label: 'Legs', icon: '👖' },
        { key: 'feet', label: 'Feet', icon: '👢' },
        { key: 'shield', label: 'Shield', icon: '🛡️' },
        { key: 'wield', label: 'Wield', icon: '⚔️' },
        { key: 'dual', label: 'Dual', icon: '🗡️' },
        { key: 'hold', label: 'Hold', icon: '🤚' },
        { key: 'ranged_weapon', label: 'Ranged', icon: '🏹' },
        { key: 'float', label: 'Float', icon: '✨' },
        { key: 'pouch', label: 'Pouch', icon: '👝' },
    ];

    const inventoryActions = [
        { label: '🔍 Inspect', action: 'inspect' },

        { label: '�📦 Drop', command: 'drop' },
        { label: '👕 Wear', command: 'wear', requiresWearable: true },
        { label: '🍽️ Eat', command: 'eat', requiresType: 'food' },
        { label: '🍺 Drink', command: 'drink', requiresType: 'drink' },
        { label: '📜 Read', command: 'read', requiresType: 'scroll' },
        { label: '🧪 Quaff', command: 'quaff', requiresType: 'potion' },
    ];

    const getItemIcon = (type) => {
        switch (type) {
            case 'weapon': return '⚔️';
            case 'armor': return '🛡️';
            case 'potion': return '🧪';
            case 'scroll': return '📜';
            case 'food': return '🍖';
            case 'drink': return '🍺';
            case 'container': return '🎒';
            case 'light': return '🔦';
            case 'key': return '🔑';
            case 'treasure': return '💎';
            default: return '📦';
        }
    };

    const getAvailableInventoryActions = (item) => {
        return inventoryActions.filter(action => {
            if (action.action === 'inspect') return true;
            if (action.requiresWearable && !item.canWear) return false;
            if (action.requiresType && item.type !== action.requiresType) return false;
            return true;
        });
    };

    const getItemVisualIndicators = (item) => {
        if (!item) return [];
        
        const indicators = [];
        
        // Cosmetic flags (always visible)
        if (item.extraFlags && item.extraFlags.includes('glow')) {
            indicators.push({ icon: '✨', label: 'Glowing', color: 'glow' });
        }
        if (item.extraFlags && item.extraFlags.includes('hum')) {
            indicators.push({ icon: '🎵', label: 'Humming', color: 'hum' });
        }
        
        // Identified-only flags
        if (item.identified === true) {
            if (item.extraFlags && item.extraFlags.includes('magic')) {
                indicators.push({ icon: '🔮', label: 'Magical', color: 'magic' });
            }
            if (item.extraFlags && item.extraFlags.includes('cursed')) {
                indicators.push({ icon: '💀', label: 'Cursed', color: 'cursed' });
            }
            
            // Ego enchantment effects
            if (item.egoFlags && item.egoFlags.length > 0) {
                if (item.egoFlags.includes('firebrand')) {
                    indicators.push({ icon: '🔥', label: 'Firebrand', color: 'firebrand' });
                }
                if (item.egoFlags.includes('empowered')) {
                    indicators.push({ icon: '⚡', label: 'Empowered', color: 'empowered' });
                }
                if (item.egoFlags.includes('soul_stealer')) {
                    indicators.push({ icon: '👻', label: 'Soul Stealer', color: 'soul-stealer' });
                }
                if (item.egoFlags.includes('bloodlust')) {
                    indicators.push({ icon: '🩸', label: 'Bloodlust', color: 'bloodlust' });
                }
                if (item.egoFlags.includes('serrated')) {
                    indicators.push({ icon: '⚔️', label: 'Serrated', color: 'serrated' });
                }
                if (item.egoFlags.includes('balanced')) {
                    indicators.push({ icon: '⚖️', label: 'Balanced', color: 'balanced' });
                }
            }
        } else if (item.identified === false) {
            indicators.push({ icon: '❓', label: 'Unidentified', color: 'unidentified' });
        }
        
        if (item.setName && item.setName !== '') {
            indicators.push({ icon: '🛡️', label: `Set: ${item.setName}`, color: 'set' });
        }
        
        return indicators;
    };

    const getRarityClass = (rarity) => {
        if (!rarity) return '';
        switch (rarity.toLowerCase()) {
            case 'uncommon': return 'rarity-uncommon';
            case 'rare': return 'rarity-rare';
            case 'epic': return 'rarity-epic';
            case 'legendary': return 'rarity-legendary';
            default: return '';
        }
    };

    // Map wearFlags to equipment slot keys
    const mapWearFlagsToSlots = (wearFlags) => {
        if (!wearFlags || !Array.isArray(wearFlags)) return [];
        
        const slotMapping = {
            'finger': ['finger_l', 'finger_r'],
            'neck': ['neck_1', 'neck_2'],
            'wrist': ['wrist_l', 'wrist_r'],
            'body': ['body'],
            'head': ['head'],
            'legs': ['legs'],
            'feet': ['feet'],
            'hands': ['hands'],
            'arms': ['arms'],
            'shield': ['shield'],
            'about': ['about'],
            'waist': ['waist'],
            'wield': ['wield'],
            'hold': ['hold'],
            'float': ['float'],
            'pouch': ['pouch'],
            'ranged_weapon': ['ranged_weapon']
        };
        
        const slots = [];
        wearFlags.forEach(flag => {
            if (slotMapping[flag]) {
                slots.push(...slotMapping[flag]);
            }
        });
        
        return slots;
    };

    // Get currently equipped items for comparison
    const getEquippedItemsForSlots = (slots) => {
        if (!equipment || !slots || slots.length === 0) return [];
        
        return slots.map(slotKey => ({
            slot: slotKey,
            item: equipment[slotKey] || null
        }));
    };

    const handleInventoryClick = (item, index) => {
        setSelectedInventoryItem(selectedInventoryItem === index ? null : index);
        setSelectedEquipmentSlot(null);
    };

    const handleEquipmentClick = (slot, item) => {
        if (item) {
            setSelectedEquipmentSlot(selectedEquipmentSlot === slot.key ? null : slot.key);
            setSelectedInventoryItem(null);
        }
    };

    const executeInventoryAction = (action, item) => {
        if (!connected) return;
        
        if (action.action === 'inspect') {
            // Determine if item is wearable and get compatible slots
            const isWearable = item.wearFlags && item.wearFlags.some(flag => flag !== 'take');
            let comparison = [];
            
            if (isWearable) {
                const compatibleSlots = mapWearFlagsToSlots(item.wearFlags);
                comparison = getEquippedItemsForSlots(compatibleSlots);
            }
            
            setInspectedItem(item);
            setComparisonItems(comparison);
            setSelectedInventoryItem(null);
            return;
        }
        
        if (action.requiresWearable && !item.canWear) return;
        if (action.requiresType && item.type !== action.requiresType) return;
        
        const command = `${action.command} ${item.keywords}`;
        onCommand(command);
        setSelectedInventoryItem(null);
        
        if (onRefresh && (action.command === 'drop' || action.command === 'wear')) {
            setTimeout(() => onRefresh(), 500);
        }
    };

    const executeEquipmentAction = (action, item) => {
        if (!connected || !item) return;
        
        if (action === 'remove') {
            onCommand(`remove ${item.keywords}`);
            if (onRefresh) {
                setTimeout(() => onRefresh(), 500);
            }
            setInspectedItem(null);
        } else if (action === 'examine') {
            // Equipment items don't need comparison since they're already equipped
            setInspectedItem(item);
            setComparisonItems([]);
        }
        setSelectedEquipmentSlot(null);
    };

    return (
        <div className="character-sheet-overlay" onClick={onClose}>
            <div className="character-sheet-modal" onClick={(e) => e.stopPropagation()}>
                <div className="character-sheet-header">
                    <h2>📋 Character Sheet</h2>
                    <div className="character-sheet-header-actions">
                        <button
                            className="header-action-btn"
                            onClick={() => onCommand('wear all')}
                            disabled={!connected}
                            title="Wear all wearable items"
                        >
                            🧥 Wear All
                        </button>
                        <button
                            className="header-action-btn"
                            onClick={() => onCommand('remove all')}
                            disabled={!connected}
                            title="Remove all equipped items"
                        >
                            🚫 Remove All
                        </button>
                    </div>
                    <button className="close-button" onClick={onClose}>✕</button>
                </div>

                <div className="character-sheet-content">
                    {inspectedItem && comparisonItems.length > 0 && (
                        <div className="comparison-panel">
                            <div className="comparison-header">
                                <h3>Equipment Comparison</h3>
                            </div>
                            <div className="comparison-body">
                                {comparisonItems.map((comp, idx) => {
                                    const slotLabel = equipmentSlots.find(s => s.key === comp.slot)?.label || comp.slot;
                                    const currentItem = comp.item;
                                    const newAvgDamage = inspectedItem.damageMin !== undefined && inspectedItem.damageMax !== undefined 
                                        ? (inspectedItem.damageMin + inspectedItem.damageMax) / 2 
                                        : null;
                                    const currentAvgDamage = currentItem?.damageMin !== undefined && currentItem?.damageMax !== undefined
                                        ? (currentItem.damageMin + currentItem.damageMax) / 2
                                        : null;
                                    
                                    return (
                                        <div key={idx} className="comparison-row">
                                            <div className="comparison-slot-header">{slotLabel}</div>
                                            <div className="comparison-items-container">
                                                {/* Currently Equipped Item */}
                                                <div className="comparison-item-side">
                                                    <div className="comparison-side-label">Equipped</div>
                                                    {currentItem ? (
                                                        <div className="comparison-item-card">
                                                            <div className="comparison-item-name">
                                                                <span dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(currentItem.name) }} />
                                                            </div>
                                                            <div className="comparison-stats">
                                                                {/* Weapon average damage */}
                                                                {currentAvgDamage !== null && newAvgDamage !== null && (
                                                                    <div className="stat-row">
                                                                        <span className="stat-label">Avg Damage:</span>
                                                                        <span className="stat-value">{currentAvgDamage.toFixed(1)}</span>
                                                                    </div>
                                                                )}
                                                                
                                                                {/* Armor AC */}
                                                                {currentItem.armorClass !== undefined && inspectedItem.armorClass !== undefined && (
                                                                    <div className="stat-row">
                                                                        <span className="stat-label">AC:</span>
                                                                        <span className="stat-value">{currentItem.armorClass}</span>
                                                                    </div>
                                                                )}
                                                                
                                                                {/* Level */}
                                                                <div className="stat-row">
                                                                    <span className="stat-label">Level:</span>
                                                                    <span className="stat-value">{currentItem.level}</span>
                                                                </div>
                                                                
                                                                {/* Affects */}
                                                                {currentItem.affects && currentItem.affects.length > 0 && (
                                                                    <div className="stat-affects">
                                                                        <div className="stat-label">Affects:</div>
                                                                        {currentItem.affects.map((affect, affIdx) => (
                                                                            <div key={affIdx} className="stat-affect-item">
                                                                                {affect.type}: {affect.modifier > 0 ? '+' : ''}{affect.modifier}
                                                                            </div>
                                                                        ))}
                                                                    </div>
                                                                )}
                                                            </div>
                                                        </div>
                                                    ) : (
                                                        <div className="comparison-item-empty">
                                                            <div className="empty-slot-icon">∅</div>
                                                            <div className="empty-slot-text">Empty Slot</div>
                                                        </div>
                                                    )}
                                                </div>
                                                
                                                <div className="comparison-arrow">→</div>
                                                
                                                {/* New Item */}
                                                <div className="comparison-item-side">
                                                    <div className="comparison-side-label">New Item</div>
                                                    <div className="comparison-item-card">
                                                        <div className="comparison-item-name">
                                                            <span dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(inspectedItem.name) }} />
                                                        </div>
                                                        <div className="comparison-stats">
                                                            {/* Weapon average damage with comparison */}
                                                            {newAvgDamage !== null && (
                                                                <div className={`stat-row ${currentAvgDamage !== null ? (newAvgDamage > currentAvgDamage ? 'stat-better' : newAvgDamage < currentAvgDamage ? 'stat-worse' : '') : ''}`}>
                                                                    <span className="stat-label">Avg Damage:</span>
                                                                    <span className="stat-value">{newAvgDamage.toFixed(1)}</span>
                                                                </div>
                                                            )}
                                                            
                                                            {/* Armor AC with comparison */}
                                                            {inspectedItem.armorClass !== undefined && (
                                                                <div className={`stat-row ${currentItem?.armorClass !== undefined ? (inspectedItem.armorClass > currentItem.armorClass ? 'stat-better' : inspectedItem.armorClass < currentItem.armorClass ? 'stat-worse' : '') : ''}`}>
                                                                    <span className="stat-label">AC:</span>
                                                                    <span className="stat-value">{inspectedItem.armorClass}</span>
                                                                </div>
                                                            )}
                                                            
                                                            {/* Level without color coding */}
                                                            <div className="stat-row">
                                                                <span className="stat-label">Level:</span>
                                                                <span className="stat-value">{inspectedItem.level}</span>
                                                            </div>
                                                            
                                                            {/* Affects */}
                                                            {inspectedItem.affects && inspectedItem.affects.length > 0 && (
                                                                <div className="stat-affects">
                                                                    <div className="stat-label">Affects:</div>
                                                                    {inspectedItem.affects.map((affect, affIdx) => (
                                                                        <div key={affIdx} className="stat-affect-item">
                                                                            {affect.type}: {affect.modifier > 0 ? '+' : ''}{affect.modifier}
                                                                        </div>
                                                                    ))}
                                                                </div>
                                                            )}
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    );
                                })}
                            </div>
                        </div>
                    )}
                    
                    {inspectedItem && (
                        <div className="item-detail-panel">
                            <div className="detail-header">
                                <h3 className={getRarityClass(inspectedItem.rarity)}>
                                    <span dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(inspectedItem.name) }} />
                                </h3>
                                <button className="detail-close-btn" onClick={() => setInspectedItem(null)}>✕</button>
                            </div>
                            <div className="detail-body">
                                <div className={`id-badge ${inspectedItem.identified ? 'identified' : 'unidentified'}`}>
                                    {inspectedItem.identified ? '✓ Identified' : '? Unidentified'}
                                </div>
                                
                                <div className="detail-section">
                                    <h4>Basic Info</h4>
                                    <p><strong>Level:</strong> {inspectedItem.level}</p>
                                    <p><strong>Weight:</strong> {inspectedItem.weight} lbs</p>
                                    {inspectedItem.identified && inspectedItem.material && (
                                        <p><strong>Material:</strong> {inspectedItem.material}</p>
                                    )}
                                    {inspectedItem.itemType && (
                                        <p><strong>Type:</strong> {inspectedItem.itemType}</p>
                                    )}
                                    {inspectedItem.wearLocation && (
                                        <p><strong>Worn:</strong> {inspectedItem.wearLocation}</p>
                                    )}
                                    {inspectedItem.itemValue !== undefined && (
                                        <p><strong>Value:</strong> {inspectedItem.itemValue} {inspectedItem.valueCurrency || 'copper'}</p>
                                    )}
                                    {inspectedItem.damageMin !== undefined && inspectedItem.damageMax !== undefined && (
                                        <p><strong>Damage:</strong> {inspectedItem.damageMin}-{inspectedItem.damageMax}</p>
                                    )}
                                    {inspectedItem.armorClass !== undefined && (
                                        <p><strong>Armor Class:</strong> {inspectedItem.armorClass}</p>
                                    )}
                                </div>

                                {inspectedItem.longDescription && (
                                    <div className="detail-section">
                                        <h4>Description</h4>
                                        <div style={{whiteSpace: 'pre-wrap'}} dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(inspectedItem.longDescription) }} />
                                    </div>
                                )}

                                {inspectedItem.extraFlags && inspectedItem.extraFlags.length > 0 && (
                                    <div className="detail-section">
                                        <h4>Properties</h4>
                                        <div className="flags-list">
                                            {inspectedItem.extraFlags.map((flag, idx) => (
                                                <span key={idx} className="flag-item">{flag.replace(/_/g, ' ')}</span>
                                            ))}
                                        </div>
                                    </div>
                                )}

                                {inspectedItem.identified && inspectedItem.egoFlags && inspectedItem.egoFlags.length > 0 && (
                                    <div className="detail-section">
                                        <h4>⭐ Enhancements</h4>
                                        <div className="flags-list">
                                            {inspectedItem.egoFlags.map((flag, idx) => (
                                                <span key={idx} className="ego-item">{flag.replace(/_/g, ' ')}</span>
                                            ))}
                                        </div>
                                    </div>
                                )}

                                {inspectedItem.identified && inspectedItem.affects && inspectedItem.affects.length > 0 && (
                                    <div className="detail-section">
                                        <h4>Stat Modifiers</h4>
                                        {inspectedItem.affects.map((affect, idx) => {
                                            const isPercentage = affect.type.toLowerCase().includes('chance') || 
                                                               affect.type.toLowerCase().includes('swiftness');
                                            const displayValue = isPercentage ? `${affect.modifier}%` : affect.modifier;
                                            
                                            return (
                                                <div key={idx} className={`affect-row ${affect.modifier > 0 ? 'positive' : 'negative'}`}>
                                                    <span className="stat-name">{affect.type}</span>
                                                    <span className="modifier-value">
                                                        {affect.modifier > 0 && !isPercentage ? '+' : ''}{displayValue}
                                                    </span>
                                                </div>
                                            );
                                        })}
                                    </div>
                                )}

                                {inspectedItem.identified && inspectedItem.classRestrictions && inspectedItem.classRestrictions.length > 0 && (
                                    <div className="detail-section">
                                        <h4>⛔ Class Restrictions</h4>
                                        <p className="restriction-text">Cannot be used by: {inspectedItem.classRestrictions.join(', ')}</p>
                                    </div>
                                )}

                                {inspectedItem.identified && inspectedItem.alignmentRestrictions && inspectedItem.alignmentRestrictions.length > 0 && (
                                    <div className="detail-section">
                                        <h4>⛔ Alignment Restrictions</h4>
                                        <p className="restriction-text">Cannot be used by: {inspectedItem.alignmentRestrictions.join(', ')}</p>
                                    </div>
                                )}

                                {inspectedItem.setRarity && (
                                    <div className="detail-section set-info">
                                        <h4>📦 Set Item</h4>
                                        <div className="set-rarity">
                                            <span className={`rarity-badge ${inspectedItem.setRarity.toLowerCase()}`}>{inspectedItem.setRarity}</span> Set
                                        </div>
                                        {inspectedItem.setLore && (
                                            <div className="set-lore">{inspectedItem.setLore}</div>
                                        )}
                                        {inspectedItem.setBonus && (
                                            <div className="set-bonus">
                                                Equip {inspectedItem.setBonus.itemCount} items to provide {inspectedItem.setBonus.stat} by +{inspectedItem.setBonus.value}
                                            </div>
                                        )}
                                    </div>
                                )}

                                {!inspectedItem.identified && (
                                    <div className="detail-section unidentified-hint">
                                        <p>💡 Cast <strong>identify</strong> to reveal hidden properties</p>
                                    </div>
                                )}
                            </div>
                        </div>
                    )}

                    <div className="equipment-section">
                        <h3 className="section-title">⚔️ Equipment</h3>
                        <div className="equipment-grid-modal">
                            {equipmentSlots.map((slot) => {
                                const item = equipment ? equipment[slot.key] : null;
                                const isSelected = selectedEquipmentSlot === slot.key;
                                const indicators = getItemVisualIndicators(item);
                                
                                return (
                                    <div key={slot.key} className="equipment-slot-wrapper">
                                        <button
                                            className={`equipment-slot-modal ${item ? 'equipped' : 'empty'} ${isSelected ? 'selected' : ''}`}
                                            onClick={() => handleEquipmentClick(slot, item)}
                                            disabled={!connected || !item}
                                            title={item ? `${stripAnsi(item.name || '')} (Lv ${item.level})` : `Empty ${slot.label}`}
                                        >
                                            <div className="slot-icon">{slot.icon}</div>
                                            <div className="slot-label">{slot.label}</div>
                                            {item && (
                                                <>
                                                    <div className={`slot-item-name ${getRarityClass(item.rarity)}`} dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.name) }} />
                                                    {indicators.length > 0 && (
                                                        <div className="item-indicators-inline">
                                                            {indicators.map((ind, idx) => (
                                                                <span key={idx} className={`indicator-mini ${ind.color}`} title={ind.label}>
                                                                    {ind.icon}
                                                                </span>
                                                            ))}
                                                        </div>
                                                    )}
                                                </>
                                            )}
                                        </button>
                                        {isSelected && item && (
                                            <div className="equipment-actions-modal">
                                                <button
                                                    className="action-btn"
                                                    onClick={() => executeEquipmentAction('remove', item)}
                                                    disabled={!connected}
                                                >
                                                    🚫 Remove
                                                </button>
                                                <button
                                                    className="action-btn"
                                                    onClick={() => executeEquipmentAction('examine', item)}
                                                    disabled={!connected}
                                                >
                                                    🔍 Examine
                                                </button>
                                            </div>
                                        )}
                                    </div>
                                );
                            })}
                        </div>
                    </div>

                    <div className="inventory-section">
                        <h3 className="section-title">🎒 Inventory ({inventory ? inventory.length : 0} items)</h3>
                        <div className="inventory-list-modal">
                            {inventory && inventory.length > 0 ? (
                                inventory.map((item, index) => {
                                    const isSelected = selectedInventoryItem === index;
                                    const indicators = getItemVisualIndicators(item);
                                    return (
                                        <div key={`inv-${item.id}-${index}`} className="inventory-item-wrapper">
                                            <button
                                                className={`inventory-item-modal ${isSelected ? 'selected' : ''}`}
                                                onClick={() => handleInventoryClick(item, index)}
                                                disabled={!connected}
                                                title={`${stripAnsi(item.name || '')} (Lv ${item.level}, ${item.weight}lbs)`}
                                            >
                                                <span className="item-icon">{getItemIcon(item.type)}</span>
                                                <span className={`item-name ${getRarityClass(item.rarity)}`} dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.name) }} />
                                                <span className="item-level">Lv{item.level}</span>
                                                {indicators.length > 0 && (
                                                    <span className="item-indicators-inline">
                                                        {indicators.map((ind, idx) => (
                                                            <span key={idx} className={`indicator-mini ${ind.color}`} title={ind.label}>
                                                                {ind.icon}
                                                            </span>
                                                        ))}
                                                    </span>
                                                )}
                                            </button>
                                            {isSelected && (
                                                <div className="inventory-actions-modal">
                                                    {getAvailableInventoryActions(item).map((action, idx) => (
                                                        <button
                                                            key={idx}
                                                            className="action-btn"
                                                            onClick={() => executeInventoryAction(action, item)}
                                                            disabled={!connected}
                                                        >
                                                            {action.label}
                                                        </button>
                                                    ))}
                                                </div>
                                            )}
                                        </div>
                                    );
                                })
                            ) : (
                                <div className="empty-message">
                                    <p>Your backpack is empty</p>
                                </div>
                            )}
                        </div>
                    </div>
                </div>

                <div className="character-sheet-footer">
                    <button className="close-footer-button" onClick={onClose}>Close</button>
                </div>
            </div>
        </div>
    );
}

export default CharacterSheet;
