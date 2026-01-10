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
const TAR_IGNORE = 0;

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
  
  return skills.filter(skill => isPracticeableSkill(skill));
}

/**
 * Filter skills that can be actively used/cast
 * Excludes group skills (like "dark magiks") as they are just prerequisites
 * and cannot be assigned to skill bars or cast
 * Also excludes skills with 0% learned - no use in battle
 * 
 * @param {Array} skills - All skills from GMCP
 * @returns {Array} Skills that can be used/cast (not group skills, learned > 0%)
 */
export function getUsableSkills(skills) {
  if (!skills) return [];
  
  return skills.filter(skill => isActionableSkill(skill) && skill.learned > 0);
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

/**
 * Server/client helper: is this skill practiceable at a trainer?
 * Includes group/knowledge skills (they are marked `isGroup` on the server)
 */
export function isPracticeableSkill(skill) {
  if (!skill) return false;

  if (skill.isGroup) return true;

  return skill.pracType === TYPE_INT || skill.pracType === TYPE_STR;
}

/**
 * Server/client helper: is this skill actionable (assignable/usable)?
 * Excludes group skills and wizard/null placeholders. Also excludes
 * entries with no mana/beats/target which are generally non-actionable.
 */
export function isActionableSkill(skill) {
  if (!skill) return false;

  if (skill.isGroup) return false;
  if (skill.pracType === TYPE_WIZ || skill.pracType === TYPE_NULL) return false;

  // If the skill has no mana, no beats and an ignore target it's likely not actionable
  const mana = typeof skill.mana === 'number' ? skill.mana : 0;
  const beats = typeof skill.beats === 'number' ? skill.beats : 0;
  const target = typeof skill.target === 'number' ? skill.target : TAR_IGNORE;

  if (mana === 0 && beats === 0 && target === TAR_IGNORE) return false;

  return true;
}
