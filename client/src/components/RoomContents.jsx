import React, { useState } from 'react';
import './RoomContents.css';
import ItemDetailModal from './ItemDetailModal';

function RoomContents({ items, npcs, onCommand, connected, skills, openers, onPracticeClick, extraDescriptions, itemDetails }) {
    const [selectedItem, setSelectedItem] = useState(null);
    const [selectedNpc, setSelectedNpc] = useState(null);
    const [detailModalItem, setDetailModalItem] = useState(null);
    const [selectedExtraDesc, setSelectedExtraDesc] = useState(null);

    const ITEM_PORTAL = 33; // Portal item type from server

    const itemActions = [
        { label: '🔍 Inspect', command: null, action: 'inspect' },
        { label: '👁️ Examine', command: 'examine' },
        { label: '📦 Get', command: 'get' },
        { label: '🍺 Drink', command: 'drink' },
        { label: '🍖 Eat', command: 'eat' },
    ];

    const portalActions = [
        { label: '🌀 Enter', command: 'enter' },
    ];

    // Get appropriate actions for an item based on its type
    const getItemActions = (item) => {
        if (item.type === ITEM_PORTAL) {
            return portalActions;
        }
        return itemActions;
    };

    // Build NPC actions dynamically including opener skills
    const getNpcActions = (npc) => {
        const baseActions = [
            { label: '👁️ Look', command: 'look' },
            { label: '⚖️ Consider', command: 'consider' },
        ];

        // Add List action for shopkeepers
        if (npc && npc.isShopkeeper) {
            baseActions.push({ label: '🛒 List', command: 'list' });
        }

        // Add Practice action for trainers
        if (npc && npc.isTrainer) {
            baseActions.push({ label: '📚 Practice', command: 'practice', isTrainer: true });
        }

        // Add opener skills if available
        if (openers && skills) {
            openers.forEach((openerId, idx) => {
                if (openerId) {
                    const skill = skills.find(s => s.id === openerId);
                    if (skill) {
                        // Format the label: "⚔️ Attack with [Skill Name]"
                        const label = `⚔️ Attack with ${skill.name}`;
                        const command = skill.type === 'spell' 
                            ? `cast '${skill.name}'` 
                            : skill.name.toLowerCase();
                        baseActions.push({ label, command, isOpener: true, skill });
                    }
                }
            });
        }

        // Add generic kill as fallback
        baseActions.push({ label: '⚔️ Kill', command: 'kill' });

        return baseActions;
    };

    const handleItemClick = (item) => {
        setSelectedItem(selectedItem?.id === item.id ? null : item);
        setSelectedNpc(null);
        setSelectedExtraDesc(null);
    };

    const handleNpcClick = (npc) => {
        setSelectedNpc(selectedNpc?.id === npc.id ? null : npc);
        setSelectedItem(null);
        setSelectedExtraDesc(null);
    };

    const handleExtraDescClick = (extraDesc) => {
        setSelectedExtraDesc(selectedExtraDesc?.keyword === extraDesc.keyword ? null : extraDesc);
        setSelectedItem(null);
        setSelectedNpc(null);
    };

    const executeAction = (action, target) => {
        if (!connected) return;
        
        // Handle inspect action (opens modal)
        if (action.action === 'inspect') {
            // Send examine command to get detailed info
            onCommand(`examine ${target.keywords}`);
            // Wait for examine response, then show modal with merged data
            setTimeout(() => {
                const itemNameWithoutArticle = target.name.replace(/^(a|an|the)\s+/i, '');
                const enrichedItem = {
                    ...target,
                    ...(itemDetails && itemDetails[itemNameWithoutArticle] ? itemDetails[itemNameWithoutArticle] : {})
                };
                setDetailModalItem(enrichedItem);
            }, 800);
            setSelectedItem(null);
            return;
        }
        
        // Handle list action (no target needed)
        if (action.command === 'list') {
            onCommand('list');
            setSelectedNpc(null);
            return;
        }

        // Handle practice action (should open practice modal)
        if (action.isTrainer) {
            // Open the practice modal
            if (onPracticeClick) {
                onPracticeClick();
            }
            setSelectedNpc(null);
            return;
        }
        
        const targetRef = target.keywords || target.name;
        
        // For opener skills, include the target in the command
        if (action.isOpener) {
            const command = `${action.command} ${targetRef}`;
            onCommand(command);
        } else {
            const command = `${action.command} ${targetRef}`;
            onCommand(command);
        }
        
        setSelectedItem(null);
        setSelectedNpc(null);
    };

    const getItemVisualIndicators = (item) => {
        const indicators = [];
        
        // Show visual flags for items (cosmetic flags always visible)
        if (item.extraFlags && item.extraFlags.includes('GLOW')) {
            indicators.push({ icon: '✨', label: 'Glowing', color: 'glow' });
        }
        if (item.extraFlags && item.extraFlags.includes('HUM')) {
            indicators.push({ icon: '🎵', label: 'Humming', color: 'hum' });
        }
        if (item.extraFlags && item.extraFlags.includes('MAGIC')) {
            indicators.push({ icon: '🔮', label: 'Magical', color: 'magic' });
        }
        
        // Show identification status
        if (item.identified === false) {
            indicators.push({ icon: '❓', label: 'Unidentified', color: 'unidentified' });
        } else if (item.identified === true) {
            indicators.push({ icon: '✓', label: 'Identified', color: 'identified' });
        }
        
        return indicators;
    };

    return (
        <div className="room-contents">
            {detailModalItem && (
                <ItemDetailModal 
                    item={detailModalItem} 
                    onClose={() => setDetailModalItem(null)} 
                />
            )}
            
            {items && items.length > 0 && (
                <div className="content-section">
                    <h4 className="content-header">Items Here</h4>
                    <div className="content-list">
                        {items.map((item, index) => {
                            const indicators = getItemVisualIndicators(item);
                            return (
                                <div key={`item-${index}-${item.id}-${item.vnum}`} className="content-item">
                                    <button
                                        className={`item-button ${selectedItem?.id === item.id ? 'selected' : ''}`}
                                        onClick={() => handleItemClick(item)}
                                        disabled={!connected}
                                        title={`Click for actions: ${item.name}`}
                                    >
                                        <span className="item-icon">📦</span>
                                        <span className="item-name">{item.name}</span>
                                        {indicators.length > 0 && (
                                            <span className="item-indicators">
                                                {indicators.map((ind, idx) => (
                                                    <span 
                                                        key={idx} 
                                                        className={`indicator ${ind.color}`}
                                                        title={ind.label}
                                                    >
                                                        {ind.icon}
                                                    </span>
                                                ))}
                                            </span>
                                        )}
                                    </button>
                                    {selectedItem?.id === item.id && (
                                        <div className="action-menu">
                                            {getItemActions(item).map((action, idx) => (
                                                <button
                                                    key={idx}
                                                    className="action-button"
                                                    onClick={() => executeAction(action, item)}
                                                    disabled={!connected}
                                                >
                                                    {action.label}
                                                </button>
                                            ))}
                                        </div>
                                    )}
                                </div>
                            );
                        })}
                    </div>
                </div>
            )}
            {npcs && npcs.length > 0 && (
                <div className="content-section">
                    <h4 className="content-header">Characters Here</h4>
                    <div className="content-list">
                        {npcs.map((npc, index) => (
                            <div key={`npc-${index}-${npc.id}-${npc.vnum}`} className="content-item">
                                <button
                                    className={`npc-button ${selectedNpc?.id === npc.id ? 'selected' : ''} ${npc.hostile ? 'hostile' : 'friendly'}`}
                                    onClick={() => handleNpcClick(npc)}
                                    disabled={!connected}
                                    title={`Click for actions: ${npc.name}`}
                                >
                                    <span className="npc-name">{npc.name}</span>
                                    {npc.isShopkeeper && (
                                        <span className="npc-indicator shopkeeper" title="Shopkeeper">
                                            🛒
                                        </span>
                                    )}
                                    {npc.isTrainer && (
                                        <span className="npc-indicator trainer" title="Trainer">
                                            📚
                                        </span>
                                    )}
                                </button>
                                {selectedNpc?.id === npc.id && (
                                    <div className="action-menu">
                                        {getNpcActions(npc).map((action, idx) => (
                                            <button
                                                key={idx}
                                                className="action-button"
                                                onClick={() => executeAction(action, npc)}
                                                disabled={!connected}
                                            >
                                                {action.label}
                                            </button>
                                        ))}
                                    </div>
                                )}
                            </div>
                        ))}
                    </div>
                </div>
            )}
            {(!items || items.length === 0) && (!npcs || npcs.length === 0) && (!extraDescriptions || extraDescriptions.length === 0) && (
                <div className="empty-state">
                    <p>Nothing of note here.</p>
                </div>
            )}

            {extraDescriptions && extraDescriptions.length > 0 && (
                <div className="content-section">
                    <h4 className="content-header">You Notice</h4>
                    <div className="content-list">
                        {extraDescriptions.map((extraDesc, index) => (
                            <div key={`extra-${index}-${extraDesc.keyword}`} className="content-item">
                                <button
                                    className={`extra-desc-button ${selectedExtraDesc?.keyword === extraDesc.keyword ? 'selected' : ''}`}
                                    onClick={() => handleExtraDescClick(extraDesc)}
                                    title={`Click to view details about ${extraDesc.keyword}`}
                                >
                                    <span className="extra-desc-icon">👁️</span>
                                    <span className="extra-desc-keyword">{extraDesc.keyword}</span>
                                </button>
                                {selectedExtraDesc?.keyword === extraDesc.keyword && (
                                    <div className="extra-desc-details">
                                        <p>{extraDesc.description}</p>
                                    </div>
                                )}
                            </div>
                        ))}
                    </div>
                </div>
            )}
        </div>
    );
}

export default RoomContents;
