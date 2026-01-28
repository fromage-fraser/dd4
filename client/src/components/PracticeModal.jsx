import React, { useState } from 'react';
import './PracticeModal.css';
import { parseAnsiToHtml, stripAnsi } from '../utils/ansiParser';
import { categorizeByPracticeType } from '../utils/skillFilters';

/**
 * PracticeModal component displays available skills to practice
 * Shows skills filtered by practice type (physical/intellectual)
 * Includes group skills (e.g., "dark magiks") that unlock prerequisites
 * Displays current skill percentage and allows training
 */
function PracticeModal({ skills, pracPhysical, pracIntellectual, onClose, onPractice, onRefresh, connected, onHelp }) {
  // Get skills categorized by practice type (includes group skills)
  const { physical: physicalSkills, intellectual: intellectualSkills } = categorizeByPracticeType(skills);

  // Track per-skill help debounce timers
  const [helpTimers, setHelpTimers] = useState({});

  const handlePractice = (skill) => {
    if (!connected) return;
    onPractice(`practice ${skill.name}`);
    // Request a fresh skill list after practicing so the modal updates
    // Small delay gives the server time to generate Char.Skills GMCP
    if (onRefresh) {
      setTimeout(() => onRefresh(), 300);
    }
  };

  const handleHelpClick = (skill) => {
    if (!connected || !onHelp) return;

    // If already debounced, ignore
    if (helpTimers[skill.id]) return;

    // Set a short debounce so repeated clicks don't spam the server
    const tid = setTimeout(() => {
      onHelp(`help ${skill.name}`);
      setHelpTimers(prev => {
        const copy = { ...prev };
        delete copy[skill.id];
        return copy;
      });
    }, 500);

    setHelpTimers(prev => ({ ...prev, [skill.id]: tid }));
  };


  const renderSkillList = (skillList, type, practicePoints) => {
    if (skillList.length === 0) {
      return <div className="no-skills">No {type} skills available</div>;
    }

    return (
      <div className="skill-list">
        {skillList.map((skill) => (
          <div key={`practice-${skill.id}`} className="practice-skill">
            <div className="skill-info">
              <div style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
                <span className="skill-name" dangerouslySetInnerHTML={{ __html: parseAnsiToHtml(skill.name) }} />
                <button
                  className="help-button"
                  onClick={() => handleHelpClick(skill)}
                  disabled={!connected || Boolean(helpTimers[skill.id])}
                  title={`Show help for ${skill.name}`}
                  aria-label={`Show help for ${skill.name}`}
                >
                  ?
                </button>
              </div>
              <span className="skill-learned">{skill.learned}%</span>
            </div>
            <button 
              className="practice-button"
              onClick={() => handlePractice(skill)}
              disabled={!connected || practicePoints <= 0 || skill.learned >= 100}
              title={
                practicePoints <= 0 
                  ? `No ${type} practice points remaining` 
                  : skill.learned >= 100 
                    ? 'Skill mastered' 
                    : `Practice ${stripAnsi(skill.name || '')}`
              }
            >
              {skill.learned >= 100 ? '✓ Mastered' : 'Practice'}
            </button>
          </div>
        ))}
      </div>
    );
  };

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="practice-modal" onClick={(e) => e.stopPropagation()}>
        <div className="practice-header">
          <h2>📚 Practice Skills</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>

        <div className="practice-content">
          <div className="practice-points">
            <div className="practice-point-box physical">
              <span className="point-icon">💪</span>
              <span className="point-label">Physical</span>
              <span className="point-value">{pracPhysical}</span>
            </div>
            <div className="practice-point-box intellectual">
              <span className="point-icon">🧠</span>
              <span className="point-label">Intellectual</span>
              <span className="point-value">{pracIntellectual}</span>
            </div>
          </div>

          <div className="practice-sections">
            <div className="practice-section physical-section">
              <h3><span className="section-icon">💪</span> Physical Skills</h3>
              {renderSkillList(physicalSkills, 'physical', pracPhysical)}
            </div>

            <div className="practice-section intellectual-section">
              <h3><span className="section-icon">🧠</span> Intellectual Skills</h3>
              {renderSkillList(intellectualSkills, 'intellectual', pracIntellectual)}
            </div>
          </div>
        </div>

        <div className="practice-footer">
          <button className="close-footer-button" onClick={onClose}>
            Close
          </button>
        </div>
      </div>
    </div>
  );
}

export default PracticeModal;
