import React, { useState } from 'react';
import './ShopModal.css';

/**
 * ShopModal component displays shop inventory and player inventory side-by-side
 * Shows items for sale with buy options and player items with sell options
 * Allows players to buy from shop and sell to shopkeeper
 * Intent: Stays open during transactions and refreshes both shop and inventory
 */
function ShopModal({ shopkeeper, items, inventory, onClose, onBuy, connected, onRefresh, shopMessage }) {
  const [selectedInventoryItem, setSelectedInventoryItem] = useState(null);
  const [isTransacting, setIsTransacting] = useState(false);
  const [notification, setNotification] = useState(null);

  const handleBuy = (item, quantity) => {
    if (!connected || isTransacting) return;
    setIsTransacting(true);
    
    // Use quantity and item keywords for the buy command
    onBuy(`buy ${quantity} ${item.keywords}`);
    
    // Server now automatically sends Char.Inventory GMCP after buy
    // We just need to re-request the shop listing to keep modal open
    setTimeout(() => {
      onBuy('list');
      setIsTransacting(false);
    }, 600);
  };

  const handleSell = (item) => {
    if (!connected || isTransacting) return;
    setIsTransacting(true);
    
    // Clear any previous notification
    setNotification(null);
    
    // Sell command with item keywords
    onBuy(`sell ${item.keywords}`);
    
    // Server now automatically sends Char.Inventory GMCP after sell
    // We just need to re-request the shop listing to keep modal open
    setTimeout(() => {
      onBuy('list');
      setIsTransacting(false);
    }, 600);
  };

  // Auto-dismiss notification after 5 seconds
  React.useEffect(() => {
    if (notification) {
      const timer = setTimeout(() => setNotification(null), 5000);
      return () => clearTimeout(timer);
    }
  }, [notification]);

  // Listen for shop messages from parent
  React.useEffect(() => {
    if (shopMessage) {
      setNotification(shopMessage);
      // Clear any previous notification timer
      const timer = setTimeout(() => {
        setNotification(null);
      }, 5000);
      return () => clearTimeout(timer);
    }
  }, [shopMessage]);

  // Determine if an item can be sold to this shopkeeper
  // Quest items, keys, and cursed items typically can't be sold
  const canSellItem = (item) => {
    if (!item) return false;
    
    // Check for no-sell flags
    if (item.extraFlags) {
      if (item.extraFlags.includes('NO_SELL') || 
          item.extraFlags.includes('QUEST') ||
          item.extraFlags.includes('NO_DROP')) {
        return false;
      }
    }
    
    // Quest items usually have quest in their name or type
    if (item.type === 'quest' || (item.name && item.name.toLowerCase().includes('quest'))) {
      return false;
    }
    
    return true;
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
          <h2>🛒 {shopkeeper}'s Shop</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>

        {notification && (
          <div className="shop-notification">
            <span className="notification-text">{notification}</span>
            <button className="notification-close" onClick={() => setNotification(null)}>✕</button>
          </div>
        )}

        <div className="shop-content">
          <div className="shop-dual-panel">
            {/* Shop Goods Section */}
            <div className="shop-goods-section">
              <h3 className="panel-title">🏪 Shop Goods</h3>
              {items && items.length > 0 ? (
                <div className="shop-items">
                  <div className="shop-items-header">
                    <span className="col-level">Lvl</span>
                    <span className="col-price">Price</span>
                    <span className="col-item">Item</span>
                    <span className="col-actions">Actions</span>
                  </div>
                  {items.map((item, index) => (
                    <div key={`shop-item-${index}-${item.vnum}`} className="shop-item">
                      <span className="col-level">{item.level}</span>
                      <span className="col-price">{item.cost}gp</span>
                      <span className="col-item">{item.name}</span>
                      <div className="col-actions">
                        <button 
                          className="buy-button buy-1"
                          onClick={() => handleBuy(item, 1)}
                          disabled={!connected || isTransacting}
                          title="Buy 1"
                        >
                          Buy 1
                        </button>
                        <button 
                          className="buy-button buy-3"
                          onClick={() => handleBuy(item, 3)}
                          disabled={!connected || isTransacting}
                          title="Buy 3"
                        >
                          Buy 3
                        </button>
                        <button 
                          className="buy-button buy-10"
                          onClick={() => handleBuy(item, 10)}
                          disabled={!connected || isTransacting}
                          title="Buy 10"
                        >
                          Buy 10
                        </button>
                      </div>
                    </div>
                  ))}
                </div>
              ) : (
                <div className="empty-shop">
                  <p>No items for sale</p>
                </div>
              )}
            </div>

            {/* Player Inventory Section */}
            <div className="shop-inventory-section">
              <h3 className="panel-title">🎒 Your Inventory ({inventory ? inventory.length : 0} items)</h3>
              {inventory && inventory.length > 0 ? (
                <div className="shop-player-inventory">
                  {inventory.map((item, index) => {
                    const isSelected = selectedInventoryItem === index;
                    const isSellable = canSellItem(item);
                    return (
                      <div 
                        key={`inv-${item.id}-${index}`} 
                        className={`shop-inventory-item ${isSelected ? 'selected' : ''} ${!isSellable ? 'not-sellable' : ''}`}
                        onClick={() => setSelectedInventoryItem(isSelected ? null : index)}
                      >
                        <span className="item-icon">{getItemIcon(item.type)}</span>
                        <div className="item-info">
                          <span className={`item-name ${getRarityClass(item.rarity)}`}>{item.name}</span>
                          <span className="item-details">Lv{item.level} • {item.weight}lbs</span>
                          {!isSellable && <span className="item-warning">⚠️ Cannot sell</span>}
                        </div>
                        <button
                          className="sell-button"
                          onClick={(e) => {
                            e.stopPropagation();
                            handleSell(item);
                          }}
                          disabled={!connected || !isSellable || isTransacting}
                          title={isSellable ? "Sell this item" : "Shopkeeper won't buy this"}
                        >
                          💰 Sell
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

export default ShopModal;
