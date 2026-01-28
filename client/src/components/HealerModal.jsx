import React, { useState, useEffect } from 'react';
import './ShopModal.css'; // Reuse ShopModal CSS classes
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';

/**
 * HealerModal component displays healer services grouped by category
 * Shows healing, curing, utility, and buff services with descriptions and costs
 * Intent: Allows players to purchase healer services; stays open for multiple purchases
 */
function HealerModal({ healer, services, onClose, onBuyService, connected }) {
  const [isTransacting, setIsTransacting] = useState(false);
  const [notification, setNotification] = useState(null);

  /**
   * Intent: Handle service purchase without closing modal (players often need multiple services).
   * Inputs: service - Service object with keyword, name, description, cost, category.
   * Postconditions: Sends 'heal <keyword>' command to server; modal remains open.
   * Failure Behavior: Does nothing if not connected or already transacting.
   */
  const handleBuyService = (service) => {
    if (!connected || isTransacting) return;
    setIsTransacting(true);
    
    // Send heal command with service keyword
    onBuyService(`heal ${service.keyword}`);
    
    // Brief delay for server processing before allowing next transaction
    setTimeout(() => {
      setIsTransacting(false);
    }, 600);
  };

  // Auto-dismiss notification after 5 seconds
  useEffect(() => {
    if (notification) {
      const timer = setTimeout(() => setNotification(null), 5000);
      return () => clearTimeout(timer);
    }
  }, [notification]);

  /**
   * Intent: Get emoji icon representing service category for visual grouping.
   * Inputs: category - String: 'healing', 'curing', 'utility', 'buff'.
   * Outputs: Emoji string for UI display.
   */
  const getCategoryIcon = (category) => {
    switch (category) {
      case 'healing': return '❤️';
      case 'curing': return '💊';
      case 'utility': return '🔮';
      case 'buff': return '✨';
      default: return '⚕️';
    }
  };

  /**
   * Intent: Get display name for service category with proper capitalization.
   */
  const getCategoryName = (category) => {
    switch (category) {
      case 'healing': return 'Healing';
      case 'curing': return 'Curing';
      case 'utility': return 'Utility';
      case 'buff': return 'Buffs';
      default: return 'Services';
    }
  };

  /**
   * Intent: Group services by category for organized display in modal.
   * Outputs: Object with category keys and arrays of service objects.
   */
  const groupedServices = services.reduce((groups, service) => {
    const category = service.category || 'healing';
    if (!groups[category]) {
      groups[category] = [];
    }
    groups[category].push(service);
    return groups;
  }, {});

  // Order categories: healing, curing, utility, buff
  const categoryOrder = ['healing', 'curing', 'utility', 'buff'];
  const orderedCategories = categoryOrder.filter(cat => groupedServices[cat]);

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="shop-modal" onClick={(e) => e.stopPropagation()}>
        <div className="shop-header">
          <h2 dangerouslySetInnerHTML={{ __html: `⚕️ ${parseAnsiToHtml(healer)}'s Services` }} />
          <button className="close-button" onClick={onClose}>✕</button>
        </div>

        {notification && (
          <div className="shop-notification">
            <span className="notification-text" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(notification) }} />
            <button className="notification-close" onClick={() => setNotification(null)}>✕</button>
          </div>
        )}

        <div className="shop-content">
          <div className="healer-services">
            {orderedCategories.map((category) => (
              <div key={category} className="healer-category">
                <h3 className="healer-category-title">
                  {getCategoryIcon(category)} {getCategoryName(category)}
                </h3>
                <div className="healer-services-list">
                  {groupedServices[category].map((service, index) => (
                    <div key={`service-${category}-${index}`} className="healer-service-card">
                      <div className="healer-service-info">
                        <div className="healer-service-header">
                          <span className="healer-service-name" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(service.name) }} />
                          <span className="healer-service-cost">{service.cost} gold</span>
                        </div>
                        <p className="healer-service-description" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(service.description) }} />
                      </div>
                      <button
                        className="buy-button buy-service"
                        onClick={() => handleBuyService(service)}
                        disabled={!connected || isTransacting}
                        title={`Purchase ${stripAnsi(service.name || '')} for ${service.cost} gold`}
                      >
                        Buy
                      </button>
                    </div>
                  ))}
                </div>
              </div>
            ))}
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

export default HealerModal;
