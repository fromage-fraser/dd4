/**
 * Intent: Parse ANSI color codes and convert them to HTML with CSS classes
 * 
 * Converts MUD color codes (ANSI escape sequences) to HTML spans with
 * appropriate CSS classes for styling. Handles foreground colors, background
 * colors, bold, underline, and other text attributes.
 * 
 * Supported ANSI codes:
 * - \x1B[0m - Reset all attributes
 * - \x1B[1m - Bold
 * - \x1B[3m - Italic
 * - \x1B[4m - Underline
 * - \x1B[30m-37m - Foreground colors
 * - \x1B[40m-47m - Background colors
 * - \x1B[90m-97m - Bright foreground colors
 */

/**
 * ANSI to CSS class mapping
 */
const ANSI_COLOR_MAP = {
  // Foreground colors
  '30': 'ansi-black',
  '31': 'ansi-red',
  '32': 'ansi-green',
  '33': 'ansi-yellow',
  '34': 'ansi-blue',
  '35': 'ansi-magenta',
  '36': 'ansi-cyan',
  '37': 'ansi-white',
  
  // Bright foreground colors
  '90': 'ansi-bright-black',
  '91': 'ansi-bright-red',
  '92': 'ansi-bright-green',
  '93': 'ansi-bright-yellow',
  '94': 'ansi-bright-blue',
  '95': 'ansi-bright-magenta',
  '96': 'ansi-bright-cyan',
  '97': 'ansi-bright-white',
  
  // Background colors
  '40': 'ansi-bg-black',
  '41': 'ansi-bg-red',
  '42': 'ansi-bg-green',
  '43': 'ansi-bg-yellow',
  '44': 'ansi-bg-blue',
  '45': 'ansi-bg-magenta',
  '46': 'ansi-bg-cyan',
  '47': 'ansi-bg-white',
  
  // Text attributes
  '1': 'ansi-bold',
  '3': 'ansi-italic',
  '4': 'ansi-underline',
  '0': 'ansi-reset'
};

/**
 * Parse ANSI escape sequences and convert to HTML
 * 
 * @param {string} text - Text containing ANSI codes
 * @returns {string} HTML string with styled spans
 */
export function parseAnsiToHtml(text) {
  if (!text) return '';
  
  // Replace ANSI escape sequences with HTML spans
  const ansiRegex = /\x1B\[([0-9;]+)m/g;
  const parts = [];
  let lastIndex = 0;
  let currentClasses = new Set();
  
  let match;
  while ((match = ansiRegex.exec(text)) !== null) {
    // Add text before the ANSI code
    if (match.index > lastIndex) {
      const textPart = text.substring(lastIndex, match.index);
      if (currentClasses.size > 0) {
        parts.push(`<span class="${Array.from(currentClasses).join(' ')}">${escapeHtml(textPart)}</span>`);
      } else {
        parts.push(escapeHtml(textPart));
      }
    }
    
    // Parse ANSI code
    const codes = match[1].split(';');
    codes.forEach(code => {
      if (code === '0') {
        // Reset all attributes
        currentClasses.clear();
      } else if (ANSI_COLOR_MAP[code]) {
        const cssClass = ANSI_COLOR_MAP[code];
        if (cssClass !== 'ansi-reset') {
          // Remove conflicting color classes before adding new one
          if (code.startsWith('3') || code.startsWith('9')) {
            // Foreground color - remove existing foreground colors
            Array.from(currentClasses).forEach(cls => {
              if (cls.startsWith('ansi-') && !cls.startsWith('ansi-bg-') && 
                  !cls.startsWith('ansi-bold') && !cls.startsWith('ansi-italic') && 
                  !cls.startsWith('ansi-underline')) {
                currentClasses.delete(cls);
              }
            });
          } else if (code.startsWith('4')) {
            // Background color - remove existing background colors
            Array.from(currentClasses).forEach(cls => {
              if (cls.startsWith('ansi-bg-')) {
                currentClasses.delete(cls);
              }
            });
          }
          currentClasses.add(cssClass);
        }
      }
    });
    
    lastIndex = ansiRegex.lastIndex;
  }
  
  // Add remaining text
  if (lastIndex < text.length) {
    const textPart = text.substring(lastIndex);
    if (currentClasses.size > 0) {
      parts.push(`<span class="${Array.from(currentClasses).join(' ')}">${escapeHtml(textPart)}</span>`);
    } else {
      parts.push(escapeHtml(textPart));
    }
  }
  
  return parts.join('');
}

/**
 * Escape HTML special characters to prevent XSS and convert newlines to <br>
 */
function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  // Convert newlines to <br> tags for proper HTML rendering
  return div.innerHTML.replace(/\n/g, '<br>');
}

/**
 * Strip ANSI codes from text (for plain text output)
 */
export function stripAnsi(text) {
  if (!text) return '';
  return text.replace(/\x1B\[[0-9;]+m/g, '');
}
