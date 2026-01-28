/*
 * Intent: Centralize visibility and interaction logic for character states (blind, asleep)
 * Responsibilities: Determine what the character can see and what actions they can perform
 * Constraints: Must check status object structure for position and affects arrays
 */

/**
 * Intent: Check if character is affected by blindness
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if character is blind
 */
export function isBlind(status) {
  if (!status || !status.affects) return false;
  
  return status.affects.some(affect => {
    const name = affect.name ? affect.name.toLowerCase() : '';
    const duration = Number(affect.duration) || 0;
    return name.includes('blind') && duration > 0;
  });
}

/**
 * Intent: Check if character is sleeping
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if character is sleeping
 */
export function isAsleep(status) {
  if (!status || !status.position) return false;
  
  const position = String(status.position).toLowerCase();
  return position.includes('sleep');
}

/**
 * Intent: Determine if character can see their inventory
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if inventory should be visible
 */
export function canSeeInventory(status) {
  // Inventory hidden only when asleep
  return !isAsleep(status);
}

/**
 * Intent: Determine if character can see room contents
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if room should be visible
 */
export function canSeeRoom(status) {
  // Room hidden when blind or asleep
  return !isAsleep(status) && !isBlind(status);
}

/**
 * Intent: Determine if character can equip items
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if character can equip items
 */
export function canEquipItem(status) {
  // Cannot equip when blind or asleep
  return !isBlind(status) && !isAsleep(status);
}

/**
 * Intent: Determine if character can remove equipped items
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if character can remove items
 */
export function canRemoveItem(status) {
  // Can remove when awake (even if blind)
  return !isAsleep(status);
}

/**
 * Intent: Determine if character can examine items
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {boolean} true if character can examine items
 */
export function canExamineItem(status) {
  // Cannot examine when blind or asleep
  return !isBlind(status) && !isAsleep(status);
}

/**
 * Intent: Get appropriate visibility restriction message for current state
 * @param {Object} status - Character status from Char.Status GMCP message
 * @returns {string} Message explaining why character can't see
 */
export function getVisibilityMessage(status) {
  if (isAsleep(status)) {
    return "You can't see anything, you're sleeping!";
  }
  
  if (isBlind(status)) {
    return "You can't see a thing!";
  }
  
  return "";
}

/**
 * Intent: Get tooltip message for disabled equipment action
 * @param {Object} status - Character status from Char.Status GMCP message
 * @param {string} action - Action type ('equip', 'remove', or 'examine')
 * @returns {string} Tooltip message explaining why action is disabled
 */
export function getEquipmentActionTooltip(status, action) {
  if (isAsleep(status)) {
    return "Cannot interact while sleeping";
  }
  
  if ((action === 'equip' || action === 'examine') && isBlind(status)) {
    return action === 'equip' ? "Cannot equip while blind" : "Cannot examine while blind";
  }
  
  return "";
}
