import React, { useState, useEffect } from 'react';
import './ShopModal.css';
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';

/**
 * Intent: Display identify service modal for unidentified items
 * Shows player inventory with identify options for each item
 * Allows players to identify items for a gold cost (1gp per item)
 * Reuses ShopModal.css for consistent styling with shop interface
 */
function IdentifyModal({ identifier, inventory, onClose, onIdentify, connected }) {
  const [notification, setNotification] = useState(null);
  const [isTransacting, setIsTransacting] = useState(false);

  // Auto-dismiss notification after 5 seconds
  useEffect(() => {
    if (notification) {
      const timer = setTimeout(() => setNotification(null), 5000);
      return () => clearTimeout(timer);
    }
  }, [notification]);

  const handleIdentify = (item) => {
    if (!connected || isTransacting) return;
    setIsTransacting(true);
    
    // Clear any previous notification
    setNotification(null);
    
    // Call identify with item keywords
    onIdentify(item.keywords);
    
    // Re-enable after a short delay
    setTimeout(() => {
      setIsTransacting(false);
    }, 600);
  };

  const isIdentified = (item) => {
    // Check if item has been identified
    if (item.identified === true) return true;
    if (item.extraFlags && item.extraFlags.includes('IDENTIFIED')) return true;
    return false;
  };

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

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="shop-modal" onClick={(e) => e.stopPropagation()}>
        <div className="shop-header">
          <h2 dangerouslySetInnerHTML={{ __html: `🔮 ${parseAnsiToHtml(identifier)}'s Identify Service` }} />
          <button className="close-button" onClick={onClose}>✕</button>
        </div>

        {notification && (
          <div className="shop-notification">
            <span className="notification-text">{notification}</span>
            <button className="notification-close" onClick={() => setNotification(null)}>✕</button>
          </div>
        )}

        <div className="shop-content">
          <div className="shop-inventory-section" style={{ maxWidth: '800px', margin: '0 auto' }}>
            <h3 className="panel-title">🎒 Your Inventory ({inventory ? inventory.length : 0} items)</h3>
            {inventory && inventory.length > 0 ? (
              <div className="shop-player-inventory">
                {inventory.map((item, index) => {
                  const identified = isIdentified(item);
                  return (
                    <div 
                      key={`inv-${item.id}-${index}`} 
                      className={`shop-inventory-item ${identified ? 'identified' : ''}`}
                    >
                      <span className="item-icon">{getItemIcon(item.type)}</span>
                      <div className="item-info">
                        <span className={`item-name ${getRarityClass(item.rarity)}`} dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(item.name) }} />
                        {identified && <span className="identified-checkmark"> ✓</span>}
                        <span className="item-details">
                          Lv{item.level} • {item.weight ? `${item.weight}lbs` : 'Light'}
                        </span>
                      </div>
                      <button
                        className={`sell-button ${identified ? 'identified-button' : ''}`}
                        onClick={(e) => {
                          e.stopPropagation();
                          handleIdentify(item);
                        }}
                        disabled={!connected || identified || isTransacting}
                        title={identified ? "Already identified" : "Identify this item for 1 gold"}
                      >
                        {identified ? '✓ Identified' : '🔮 Identify (1G)'}
                      </button>
                    </div>
                  );
                })}
              </div>
            ) : (
              <div className="empty-shop">
                <p>Your inventory is empty</p>
              </div>
            )}
          </div>
        </div>

        <div className="shop-footer">
          <button className="close-footer-button" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
}

export default IdentifyModal;
