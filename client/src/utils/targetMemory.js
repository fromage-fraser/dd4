/**
 * Module Intent: Manage per-skill, per-character persistent target memory for the MUD client.
 * Responsibilities: 
 * - Store and retrieve saved skill targets in character-specific localStorage
 * - Provide auto-targeting logic based on combat state
 * - Support mass target updates for skill groups
 * 
 * Constraints:
 * - Targets persist indefinitely (no pruning)
 * - Targets preserved even when invalid (support chase mechanics)
 * - Separate memory per skill (no shared targets across skill types)
 */

// TAR_* constants matching server-side definitions (from merc.h)
export const TAR_IGNORE = 0;
export const TAR_CHAR_OFFENSIVE = 1;
export const TAR_CHAR_DEFENSIVE = 2;
export const TAR_CHAR_SELF = 3;
export const TAR_OBJ_INV = 4;
export const TAR_OBJ_ROOM = 5;
export const TAR_CHAR_OFFENSIVE_SINGLE = 6;

/**
 * Intent: Generate localStorage key for character-specific target memory.
 * Inputs: characterName (string) - Player character name
 * Outputs: Storage key string
 */
function getStorageKey(characterName) {
  return `dd4_skill_targets_${characterName}`;
}

/**
 * Intent: Load all saved skill targets for a character from localStorage.
 * Inputs: characterName (string) - Player character name
 * Outputs: Object mapping skillId to target name, or empty object if none exist
 * Failure Behavior: Returns empty object on parse error or missing data
 */
export function loadSkillTargets(characterName) {
  if (!characterName) return {};
  
  try {
    const key = getStorageKey(characterName);
    const data = localStorage.getItem(key);
    return data ? JSON.parse(data) : {};
  } catch (error) {
    console.error('Failed to load skill targets:', error);
    return {};
  }
}

/**
 * Intent: Save a single skill's target to persistent storage.
 * Inputs:
 *   - characterName (string) - Player character name
 *   - skillId (string) - Unique skill identifier
 *   - targetName (string) - Target NPC/player name (or null to clear)
 * Postconditions: localStorage updated with new target mapping
 * Failure Behavior: Logs error and continues on storage failure
 */
export function saveSkillTarget(characterName, skillId, targetName) {
  if (!characterName || !skillId) return;
  
  try {
    const key = getStorageKey(characterName);
    const targets = loadSkillTargets(characterName);
    
    if (targetName === null || targetName === undefined || targetName === '') {
      delete targets[skillId];
    } else {
      targets[skillId] = targetName;
    }
    
    localStorage.setItem(key, JSON.stringify(targets));
  } catch (error) {
    console.error('Failed to save skill target:', error);
  }
}

/**
 * Intent: Retrieve saved target for a specific skill.
 * Inputs:
 *   - characterName (string) - Player character name
 *   - skillId (string) - Unique skill identifier
 * Outputs: Target name (string) or null if not set
 */
export function getSkillTarget(characterName, skillId) {
  if (!characterName || !skillId) return null;
  
  const targets = loadSkillTargets(characterName);
  return targets[skillId] || null;
}

/**
 * Intent: Get effective target for a skill, including auto-targeting logic.
 * Inputs:
 *   - characterName (string) - Player character name
 *   - skillId (string) - Unique skill identifier
 *   - targetType (number) - TAR_* constant for skill
 *   - isFighting (boolean) - Whether character is in combat
 *   - opponent (string|null) - Current combat opponent name
 * Outputs: Object with {target: string|null, isAuto: boolean}
 * Notes:
 *   - Returns saved target if exists (isAuto: false)
 *   - Returns opponent for offensive skills in combat if no saved target (isAuto: true)
 *   - Returns null if no target available
 */
export function getEffectiveTarget(characterName, skillId, targetType, isFighting, opponent) {
  // Check for saved target first
  const savedTarget = getSkillTarget(characterName, skillId);
  if (savedTarget) {
    return { target: savedTarget, isAuto: false };
  }
  
  // Auto-target for offensive skills in combat
  if (isFighting && opponent) {
    const isOffensive = targetType === TAR_CHAR_OFFENSIVE || 
                        targetType === TAR_CHAR_OFFENSIVE_SINGLE;
    if (isOffensive) {
      return { target: opponent, isAuto: true };
    }
  }
  
  return { target: null, isAuto: false };
}

/**
 * Intent: Set the same target for multiple skills (mass update).
 * Inputs:
 *   - characterName (string) - Player character name
 *   - skillIds (array of strings) - Skills to update
 *   - targetName (string) - Target to assign to all skills
 * Postconditions: All specified skills have target updated in localStorage
 * Failure Behavior: Logs error and continues on storage failure
 */
export function setMassTarget(characterName, skillIds, targetName) {
  if (!characterName || !skillIds || skillIds.length === 0) return;
  
  try {
    const key = getStorageKey(characterName);
    const targets = loadSkillTargets(characterName);
    
    skillIds.forEach(skillId => {
      if (targetName === null || targetName === undefined || targetName === '') {
        delete targets[skillId];
      } else {
        targets[skillId] = targetName;
      }
    });
    
    localStorage.setItem(key, JSON.stringify(targets));
  } catch (error) {
    console.error('Failed to set mass targets:', error);
  }
}

/**
 * Intent: Determine color coding for skill based on target type.
 * Inputs: targetType (number) - TAR_* constant
 * Outputs: Color string (CSS color name or hex)
 * Notes:
 *   - Red for offensive (TAR_CHAR_OFFENSIVE, TAR_CHAR_OFFENSIVE_SINGLE)
 *   - Blue for defensive (TAR_CHAR_DEFENSIVE)
 *   - Green for self (TAR_CHAR_SELF)
 *   - Gray for area/items (TAR_IGNORE, TAR_OBJ_*)
 */
export function getTargetTypeColor(targetType) {
  switch (targetType) {
    case TAR_CHAR_OFFENSIVE:
    case TAR_CHAR_OFFENSIVE_SINGLE:
      return '#dc2626'; // red
    case TAR_CHAR_DEFENSIVE:
      return '#2563eb'; // blue
    case TAR_CHAR_SELF:
      return '#16a34a'; // green
    case TAR_IGNORE:
    case TAR_OBJ_INV:
    case TAR_OBJ_ROOM:
    default:
      return '#6b7280'; // gray
  }
}

/**
 * Intent: Check if a skill requires a target (needs targeting UI).
 * Inputs: targetType (number) - TAR_* constant
 * Outputs: Boolean - true if skill needs a target selection
 */
export function requiresTarget(targetType) {
  return targetType === TAR_CHAR_OFFENSIVE ||
         targetType === TAR_CHAR_OFFENSIVE_SINGLE ||
         targetType === TAR_CHAR_DEFENSIVE ||
         targetType === TAR_CHAR_SELF;
}

/**
 * Intent: Determine if skill should show warning icon.
 * Inputs:
 *   - targetType (number) - TAR_* constant
 *   - hasTarget (boolean) - Whether skill has effective target
 *   - isFighting (boolean) - Whether in combat
 * Outputs: Boolean - true if warning should be shown
 * Notes:
 *   - Show warning if target required AND:
 *     (a) No target AND not in combat, OR
 *     (b) No target AND in combat but no auto-target available
 */
export function shouldShowWarning(targetType, hasTarget, isFighting) {
  if (!requiresTarget(targetType)) return false;
  if (hasTarget) return false;
  
  // For offensive skills, only warn if not in combat (auto-target unavailable)
  const isOffensive = targetType === TAR_CHAR_OFFENSIVE || 
                      targetType === TAR_CHAR_OFFENSIVE_SINGLE;
  if (isOffensive && isFighting) return false;
  
  // For defensive/self skills, always warn if no target
  return true;
}
