import React, { useState, useEffect } from 'react';
import './SkillTreeModal.css';

/**
 * Intent: Display skill prerequisite tree showing available and locked skills.
 * 
 * Shows the complete skill learning progression for the character including:
 * - Available skills (learned or learnable)
 * - Locked skills with their prerequisite chains
 * 
 * Inputs:
 * - skillTree: object with { available: [], locked: [] } from Char.SkillTree GMCP
 * - onClose: callback to close the modal
 * - onCommand: callback to execute MUD commands
 * 
 * Outputs: Rendered skill tree visualization in modal
 * 
 * Preconditions: skillTree data structure from server
 * Postconditions: User understands skill progression and prerequisites
 * 
 * Notes: Prerequisites shown with → arrows; OR groups show alternatives
 */
function SkillTreeModal({ skillTree, onClose, onCommand }) {
  const [filter, setFilter] = useState('all'); // 'all', 'available', 'locked'
  const [searchTerm, setSearchTerm] = useState('');

  console.log('SkillTreeModal rendered with skillTree:', skillTree);

  if (!skillTree) {
    console.log('SkillTreeModal: No skillTree data');
    return null;
  }

  const { available = [], locked = [] } = skillTree;
  console.log('SkillTreeModal: available:', available.length, 'locked:', locked.length);

  // Group skills by their primary spell group
  const groupSkills = (skills) => {
    const groups = {};
    const subclassByName = {}; // Organize subclass skills by subclass name
    const subclassGroups = {}; // Legacy: single subclass view
    const ungrouped = [];

    skills.forEach(skill => {
      // Separate subclass-specific skills
      if (skill.isSubclass) {
        // If we have subclassName, organize by that
        if (skill.subclassName) {
          if (!subclassByName[skill.subclassName]) {
            subclassByName[skill.subclassName] = {};
          }
          
          // Group within subclass by spell group
          const prereqs = skill.prerequisites || '';
          const groupMatch = prereqs.match(/([a-z]+(?:\s+[a-z]+)*)\s+magiks/i);
          
          if (groupMatch) {
            const groupName = groupMatch[1].charAt(0).toUpperCase() + groupMatch[1].slice(1);
            if (!subclassByName[skill.subclassName][groupName]) {
              subclassByName[skill.subclassName][groupName] = [];
            }
            subclassByName[skill.subclassName][groupName].push(skill);
          } else {
            if (!subclassByName[skill.subclassName]['Other']) {
              subclassByName[skill.subclassName]['Other'] = [];
            }
            subclassByName[skill.subclassName]['Other'].push(skill);
          }
        } else {
          // Legacy: no subclassName (level 30+)
          const prereqs = skill.prerequisites || '';
          const groupMatch = prereqs.match(/([a-z]+(?:\s+[a-z]+)*)\s+magiks/i);
          
          if (groupMatch) {
            const groupName = groupMatch[1].charAt(0).toUpperCase() + groupMatch[1].slice(1);
            if (!subclassGroups[groupName]) {
              subclassGroups[groupName] = [];
            }
            subclassGroups[groupName].push(skill);
          } else {
            if (!subclassGroups['Other']) {
              subclassGroups['Other'] = [];
            }
            subclassGroups['Other'].push(skill);
          }
        }
        return;
      }
      
      // Extract primary spell group from prerequisites
      // Look for patterns like "evocation magiks", "alteration magiks", etc.
      const prereqs = skill.prerequisites || '';
      const groupMatch = prereqs.match(/([a-z]+(?:\s+[a-z]+)*)\s+magiks/i);
      
      if (groupMatch) {
        const groupName = groupMatch[1].charAt(0).toUpperCase() + groupMatch[1].slice(1);
        if (!groups[groupName]) {
          groups[groupName] = [];
        }
        groups[groupName].push(skill);
      } else if (prereqs.includes('mage base')) {
        if (!groups['Mage Base']) {
          groups['Mage Base'] = [];
        }
        groups['Mage Base'].push(skill);
      } else {
        ungrouped.push(skill);
      }
    });

    return { groups, subclassGroups, subclassByName, ungrouped };
  };

  const allSkills = [...available, ...locked];
  const { groups: skillGroups, subclassGroups, subclassByName, ungrouped } = groupSkills(allSkills);
  
  // Sort group names alphabetically
  const sortedGroupNames = Object.keys(skillGroups).sort();

  // Filter skills based on selected filter and search term
  const getFilteredSkills = () => {
    let filteredAllSkills = allSkills;

    // Apply search filter
    if (searchTerm) {
      const term = searchTerm.toLowerCase();
      filteredAllSkills = filteredAllSkills.filter(s => 
        s.name.toLowerCase().includes(term) || 
        (s.prerequisites && s.prerequisites.toLowerCase().includes(term))
      );
    }

    // Apply category filter
    switch (filter) {
      case 'available':
        filteredAllSkills = filteredAllSkills.filter(s => s.learned > 0);
        break;
      case 'locked':
        filteredAllSkills = filteredAllSkills.filter(s => s.learned === 0);
        break;
      default:
        // all
        break;
    }

    // Re-group the filtered skills
    return groupSkills(filteredAllSkills);
  };

  const { groups: filteredGroups, subclassGroups: filteredSubclassGroups, ungrouped: filteredUngrouped } = getFilteredSkills();
  const filteredGroupNames = Object.keys(filteredGroups).sort();

  const handlePractice = (skillName) => {
    onCommand(`practice ${skillName}`);
  };

  const handleBackdropClick = (e) => {
    if (e.target === e.currentTarget) {
      onClose();
    }
  };

  return (
    <div className="skill-tree-modal-backdrop" onClick={handleBackdropClick}>
      <div className="skill-tree-modal">
        <div className="skill-tree-header">
          <h2>🌳 Skill Tree</h2>
          <button className="skill-tree-close" onClick={onClose}>✕</button>
        </div>

        <div className="skill-tree-controls">
          <input
            type="text"
            className="skill-tree-search"
            placeholder="Search skills..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
          />
          <div className="skill-tree-filters">
            <button
              className={`filter-btn ${filter === 'all' ? 'active' : ''}`}
              onClick={() => setFilter('all')}
            >
              All ({available.length + locked.length})
            </button>
            <button
              className={`filter-btn ${filter === 'available' ? 'active' : ''}`}
              onClick={() => setFilter('available')}
            >
              Available ({available.length})
            </button>
            <button
              className={`filter-btn ${filter === 'locked' ? 'active' : ''}`}
              onClick={() => setFilter('locked')}
            >
              Locked ({locked.length})
            </button>
          </div>
        </div>

        <div className="skill-tree-content">
          {/* Render each spell group as a collapsible section */}
          {filteredGroupNames.map(groupName => {
            const groupSkills = filteredGroups[groupName];
            const availableCount = groupSkills.filter(s => s.learned > 0).length;
            const lockedCount = groupSkills.filter(s => s.learned === 0).length;

            return (
              <div key={groupName} className="skill-group">
                <h3 className="group-title">
                  📚 {groupName} ({availableCount} / {groupSkills.length})
                </h3>
                <div className="skill-list">
                  {groupSkills.map((skill, idx) => (
                    <div 
                      key={`${groupName}-${idx}`} 
                      className={`skill-card ${skill.learned > 0 ? 'available' : 'locked'}`}
                    >
                      <div className="skill-card-header">
                        <span className="skill-name">
                          {skill.learned > 0 ? '✓ ' : '🔒 '}
                          {skill.name}
                        </span>
                        {skill.learned > 0 && (
                          <span className="skill-proficiency">{skill.learned}%</span>
                        )}
                      </div>
                      {skill.prerequisites && (
                        <div className="skill-prerequisites">
                          <span className="prereq-label">Requires:</span> {skill.prerequisites}
                        </div>
                      )}
                      {skill.learned > 0 && skill.learned < 100 && (
                        <button 
                          className="practice-btn"
                          onClick={() => handlePractice(skill.name)}
                        >
                          Practice
                        </button>
                      )}
                    </div>
                  ))}
                </div>
              </div>
            );
          })}

          {/* Ungrouped skills */}
          {filteredUngrouped.length > 0 && (
            <div className="skill-group">
              <h3 className="group-title">🔧 Other Skills ({filteredUngrouped.length})</h3>
              <div className="skill-list">
                {filteredUngrouped.map((skill, idx) => (
                  <div 
                    key={`ungrouped-${idx}`} 
                    className={`skill-card ${skill.learned > 0 ? 'available' : 'locked'}`}
                  >
                    <div className="skill-card-header">
                      <span className="skill-name">
                        {skill.learned > 0 ? '✓ ' : '🔒 '}
                        {skill.name}
                      </span>
                      {skill.learned > 0 && (
                        <span className="skill-proficiency">{skill.learned}%</span>
                      )}
                    </div>
                    {skill.prerequisites && (
                      <div className="skill-prerequisites">
                        <span className="prereq-label">Requires:</span> {skill.prerequisites}
                      </div>
                    )}
                    {skill.learned > 0 && skill.learned < 100 && (
                      <button 
                        className="practice-btn"
                        onClick={() => handlePractice(skill.name)}
                      >
                        Practice
                      </button>
                    )}
                  </div>
                ))}
              </div>
            </div>
          )}

          {/* Subclass skills - 3-column view when under level 30 */}
          {Object.keys(subclassByName).length > 0 && (
            <div className="skill-group subclass-section">
              <h3 className="group-title">⭐ Subclass Options (Choose at Level 30)</h3>
              <p className="subclass-note">Preview the three paths available for specialization at level 30. Each offers unique spell groups and abilities.</p>
              
              <div className="subclass-columns">
                {Object.keys(subclassByName).sort().map(subclassName => {
                  const subclassData = subclassByName[subclassName];
                  const totalSkills = Object.values(subclassData).flat().length;
                  
                  return (
                    <div key={subclassName} className="subclass-column">
                      <h4 className="subclass-column-title">{subclassName} ({totalSkills})</h4>
                      
                      {Object.keys(subclassData).sort().map(groupName => {
                        const groupSkills = subclassData[groupName];
                        return (
                          <div key={`${subclassName}-${groupName}`} className="subclass-spell-group">
                            <h5 className="subclass-group-title">📚 {groupName} ({groupSkills.length})</h5>
                            <div className="skill-list-compact">
                              {groupSkills.map((skill, idx) => (
                                <div 
                                  key={`${subclassName}-${groupName}-${idx}`} 
                                  className={`skill-card compact ${skill.learned > 0 ? 'available' : 'locked'}`}
                                >
                                  <div className="skill-card-header">
                                    <span className="skill-name">
                                      {skill.learned > 0 ? '✓ ' : '🔒 '}
                                      {skill.name}
                                    </span>
                                  </div>
                                  {skill.prerequisites && (
                                    <div className="skill-prerequisites">
                                      <span className="prereq-label">Req:</span> {skill.prerequisites}
                                    </div>
                                  )}
                                </div>
                              ))}
                            </div>
                          </div>
                        );
                      })}
                    </div>
                  );
                })}
              </div>
            </div>
          )}
          
          {/* Legacy subclass view for level 30+ (single subclass) */}
          {Object.keys(subclassByName).length === 0 && Object.keys(filteredSubclassGroups).length > 0 && (
            <div className="skill-group subclass-section">
              <h3 className="group-title">⭐ Subclass Skills (Level 30+)</h3>
              <p className="subclass-note">These skills are unlocked should you choose to focus on this class at level 30.</p>
              
              {Object.keys(filteredSubclassGroups).sort().map(groupName => {
                const groupSkills = filteredSubclassGroups[groupName];
                return (
                  <div key={`subclass-${groupName}`} className="subclass-spell-group">
                    <h4 className="subclass-group-title">📚 {groupName} ({groupSkills.length})</h4>
                    <div className="skill-list">
                      {groupSkills.map((skill, idx) => (
                        <div 
                          key={`subclass-${groupName}-${idx}`} 
                          className={`skill-card ${skill.learned > 0 ? 'available' : 'locked'}`}
                        >
                          <div className="skill-card-header">
                            <span className="skill-name">
                              {skill.learned > 0 ? '✓ ' : '🔒 '}
                              {skill.name}
                            </span>
                            {skill.learned > 0 && (
                              <span className="skill-proficiency">{skill.learned}%</span>
                            )}
                          </div>
                          {skill.prerequisites && (
                            <div className="skill-prerequisites">
                              <span className="prereq-label">Requires:</span> {skill.prerequisites}
                            </div>
                          )}
                          {skill.learned > 0 && skill.learned < 100 && (
                            <button 
                              className="practice-btn"
                              onClick={() => handlePractice(skill.name)}
                            >
                              Practice
                            </button>
                            )}
                        </div>
                      ))}
                    </div>
                  </div>
                );
              })}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}

export default SkillTreeModal;
