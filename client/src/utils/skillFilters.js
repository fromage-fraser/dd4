/**
 * Skill filtering utilities for different UI contexts
 * 
 * Two main use cases:
 * 1. Practiceable skills - includes group skills (e.g., "dark magiks") that can be practiced
 * 2. Usable skills - excludes group skills that cannot be cast/used in combat
 */

const TYPE_INT = 1;
const TYPE_STR = 2;
const TYPE_WIZ = 3;
const TYPE_NULL = 4;

/**
 * Filter skills that can be practiced at a trainer
 * Includes group skills (like "dark magiks") as they can be practiced
 * to unlock prerequisites for other skills
 * 
 * @param {Array} skills - All skills from GMCP
 * @returns {Array} Skills that can be practiced (TYPE_INT or TYPE_STR)
 */
export function getPracticeableSkills(skills) {
  if (!skills) return [];
  
  return skills.filter(skill => 
    skill.pracType === TYPE_INT || skill.pracType === TYPE_STR
  );
}

/**
 * Filter skills that can be actively used/cast
 * Excludes group skills (like "dark magiks") as they are just prerequisites
 * and cannot be assigned to skill bars or cast
 * 
 * @param {Array} skills - All skills from GMCP
 * @returns {Array} Skills that can be used/cast (not group skills)
 */
export function getUsableSkills(skills) {
  if (!skills) return [];
  
  return skills.filter(skill => !skill.isGroup);
}

/**
 * Separate practiceable skills into physical and intellectual categories
 * 
 * @param {Array} skills - All skills from GMCP
 * @returns {Object} { physical: Array, intellectual: Array }
 */
export function categorizeByPracticeType(skills) {
  const practiceable = getPracticeableSkills(skills);
  
  return {
    physical: practiceable.filter(s => s.pracType === TYPE_STR),
    intellectual: practiceable.filter(s => s.pracType === TYPE_INT)
  };
}
