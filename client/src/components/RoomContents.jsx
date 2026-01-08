import React, { useState } from 'react';
import './RoomContents.css';
import ItemDetailModal from './ItemDetailModal';

function RoomContents({ items, npcs, onCommand, connected, skills, openers, onPracticeClick, extraDescriptions, itemDetails, currentPlayerName }) {
    const [selectedItem, setSelectedItem] = useState(null);
    const [selectedNpc, setSelectedNpc] = useState(null);
    const [selectedPlayer, setSelectedPlayer] = useState(null);
    const [detailModalItem, setDetailModalItem] = useState(null);
    const [selectedExtraDesc, setSelectedExtraDesc] = useState(null);

    // Filter players from NPCs and exclude current player
    const players = npcs ? npcs.filter(npc => npc.isPlayer && npc.name !== currentPlayerName) : [];
    const actualNpcs = npcs ? npcs.filter(npc => !npc.isPlayer) : [];

    const ITEM_PORTAL = 33; // Portal item type from server
    const ITEM_FOUNTAIN = 25; // Fountain item type from server

    const baseItemActions = [
        { label: '👁️ Examine', command: 'examine' },
        { label: '📦 Get', command: 'get' },
    ];

    const portalActions = [
        { label: '🌀 Enter', command: 'enter' },
    ];

    // Get appropriate actions for an item based on its type
    const getItemActions = (item) => {
        if (item.type === ITEM_PORTAL) {
            return portalActions;
        }
        
        const actions = [...baseItemActions];
        
        // Add drink action only for fountains
        if (item.type === ITEM_FOUNTAIN) {
            actions.push({ label: '🍺 Drink', command: 'drink' });
        }
        
        return actions;
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

        // Add Heal action for healers
        if (npc && npc.isHealer) {
            baseActions.push({ label: '⚕️ Heal', command: 'heal' });
        }

        // Add Identify action for identifiers
        if (npc && npc.isIdentifier) {
            baseActions.push({ label: '🔮 Identify', command: 'identify' });
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

    // Player-specific actions (social/grouping)
    const getPlayerActions = (player) => {
        return [
            { label: '👁️ Look', command: 'look' },
            { label: '👥 Follow', command: 'follow' },
            { label: '🤝 Group', command: 'group' },
        ];
    };

    const handleItemClick = (item) => {
        setSelectedItem(selectedItem?.id === item.id ? null : item);
        setSelectedNpc(null);
        setSelectedPlayer(null);
        setSelectedExtraDesc(null);
    };

    const handleNpcClick = (npc) => {
        setSelectedNpc(selectedNpc?.id === npc.id ? null : npc);
        setSelectedItem(null);
        setSelectedPlayer(null);
        setSelectedExtraDesc(null);
    };

    const handlePlayerClick = (player) => {
        setSelectedPlayer(selectedPlayer?.keywords === player.keywords ? null : player);
        setSelectedItem(null);
        setSelectedNpc(null);
        setSelectedExtraDesc(null);
    };

    const handleExtraDescClick = (extraDesc) => {
        setSelectedExtraDesc(selectedExtraDesc?.keyword === extraDesc.keyword ? null : extraDesc);
        setSelectedItem(null);
        setSelectedNpc(null);
        setSelectedPlayer(null);
    };

    const executeAction = (action, target) => {
        if (!connected) return;
        
        // Handle list action (no target needed)
        if (action.command === 'list') {
            onCommand('list');
            setSelectedNpc(null);
            return;
        }

        // Handle heal action (no target needed)
        if (action.command === 'heal') {
            onCommand('heal');
            setSelectedNpc(null);
            return;
        }

        // Handle identify action (opens identifier modal)
        if (action.command === 'identify') {
            onCommand('identify');
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
        
        // Cosmetic flags (always visible)
        if (item.extraFlags && item.extraFlags.includes('glow')) {
            indicators.push({ icon: '✨', label: 'Glowing', color: 'glow' });
        }
        if (item.extraFlags && item.extraFlags.includes('hum')) {
            indicators.push({ icon: '🎵', label: 'Humming', color: 'hum' });
        }
        
        // Identified-only flags and effects
        if (item.identified === true) {
            if (item.extraFlags && item.extraFlags.includes('magic')) {
                indicators.push({ icon: '🔮', label: 'Magical', color: 'magic' });
            }
            if (item.extraFlags && item.extraFlags.includes('cursed')) {
                indicators.push({ icon: '💀', label: 'Cursed', color: 'cursed' });
            }
            
            // Ego enchantment effects
            if (item.egoFlags) {
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
            // Show identification status for unidentified items
            indicators.push({ icon: '❓', label: 'Unidentified', color: 'unidentified' });
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
            
            {players && players.length > 0 && (
                <div className="content-section">
                    <h4 className="content-header">Players Here</h4>
                    <div className="content-list">
                        {players.map((player, index) => (
                            <div key={`player-${index}-${player.keywords}`} className="content-item">
                                <button
                                    className={`player-button ${selectedPlayer?.keywords === player.keywords ? 'selected' : ''}`}
                                    onClick={() => handlePlayerClick(player)}
                                    disabled={!connected}
                                    title={`Click for actions: ${player.name}`}
                                >
                                    <span className="player-name">{player.name}</span>
                                    {player.level && (
                                        <span className="player-level-badge" title={`Level ${player.level}`}>
                                            Lv{player.level}
                                        </span>
                                    )}
                                </button>
                                {selectedPlayer?.keywords === player.keywords && (
                                    <div className="action-menu">
                                        {getPlayerActions(player).map((action, idx) => (
                                            <button
                                                key={idx}
                                                className="action-button"
                                                onClick={() => executeAction(action, player)}
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
            {actualNpcs && actualNpcs.length > 0 && (
                <div className="content-section">
                    <h4 className="content-header">Characters Here</h4>
                    <div className="content-list">
                        {actualNpcs.map((npc, index) => (
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
                                    {npc.isIdentifier && (
                                        <span className="npc-indicator identifier" title="Identifier">
                                            🔮
                                        </span>
                                    )}
                                    {npc.isHealer && (
                                        <span className="npc-indicator healer" title="Healer">
                                            ⚕️
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
            {(!items || items.length === 0) && (!players || players.length === 0) && (!actualNpcs || actualNpcs.length === 0) && (!extraDescriptions || extraDescriptions.length === 0) && (
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
