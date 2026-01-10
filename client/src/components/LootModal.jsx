import React from 'react';
import './LootModal.css';
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';

/**
 * Modal for displaying the contents of a container/corpse and allowing
 * the player to take individual items. Calls `onTake` with the knob
 * command string (e.g. "get <keyword> from <container>").
 */
function LootModal({ container, contents = [], onClose, onTake }) {
    if (!container) return null;

    return (
        <div className="modal-overlay" onClick={onClose}>
            <div className="modal-content" onClick={(e) => e.stopPropagation()}>
                <div className="modal-header">
                    <h3 dangerouslySetInnerHTML={{ __html: `Loot: ${parseAnsiToHtml(container.name)}` }} />
                    <button className="close-button" onClick={onClose}>✕</button>
                </div>

                <div className="modal-body">
                    {contents.length === 0 && (
                        <p className="empty">Nothing to loot.</p>
                    )}

                    {contents.length > 0 && (
                        <div className="loot-list">
                            {contents.map((it, idx) => (
                                <div key={`loot-${idx}-${it.id}-${it.vnum}`} className="loot-item">
                                    <div className="loot-name" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(it.name) }} />
                                    <div className="loot-actions">
                                        <button
                                            className="action-button"
                                            onClick={() => onTake(it)}
                                            title={stripAnsi(it.name || '')}
                                        >
                                            Get
                                        </button>
                                    </div>
                                </div>
                            ))}
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

export default LootModal;
