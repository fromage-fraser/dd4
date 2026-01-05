import React from 'react';
import './PracticeModal.css';

/**
 * PracticeModal component displays available skills to practice
 * Shows skills filtered by practice type (physical/intellectual)
 * Displays current skill percentage and allows training
 */
function PracticeModal({ skills, pracPhysical, pracIntellectual, onClose, onPractice, connected }) {
  const TYPE_INT = 1;
  const TYPE_STR = 2;
  const TYPE_WIZ = 3;
  const TYPE_NULL = 4;

  // Filter skills that can be practiced (not TYPE_WIZ or TYPE_NULL)
  const practiceableSkills = skills.filter(skill => 
    skill.pracType === TYPE_INT || skill.pracType === TYPE_STR
  );

  // Separate into physical and intellectual
  const physicalSkills = practiceableSkills.filter(s => s.pracType === TYPE_STR);
  const intellectualSkills = practiceableSkills.filter(s => s.pracType === TYPE_INT);

  const handlePractice = (skill) => {
    if (!connected) return;
    onPractice(`practice ${skill.name}`);
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
              <span className="skill-name">{skill.name}</span>
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
                    : `Practice ${skill.name}`
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
