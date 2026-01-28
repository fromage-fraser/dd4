/**
 * Clean, single implementation of ANSI -> HTML parser with ROM token support
 */

const ANSI_COLOR_MAP = {
  '30': 'ansi-black', '31': 'ansi-red', '32': 'ansi-green', '33': 'ansi-yellow',
  '34': 'ansi-blue', '35': 'ansi-magenta', '36': 'ansi-cyan', '37': 'ansi-white',
  '90': 'ansi-bright-black', '91': 'ansi-bright-red', '92': 'ansi-bright-green', '93': 'ansi-bright-yellow',
  '94': 'ansi-bright-blue', '95': 'ansi-bright-magenta', '96': 'ansi-bright-cyan', '97': 'ansi-bright-white',
  '40': 'ansi-bg-black', '41': 'ansi-bg-red', '42': 'ansi-bg-green', '43': 'ansi-bg-yellow',
  '44': 'ansi-bg-blue', '45': 'ansi-bg-magenta', '46': 'ansi-bg-cyan', '47': 'ansi-bg-white',
  '1': 'ansi-bold', '3': 'ansi-italic', '4': 'ansi-underline', '0': 'ansi-reset'
};

export function parseAnsiToHtml(text) {
  if (!text) return '';
  text = translateRomToAnsi(text);

  const ansiRegex = /\x1B\[([0-9;]+)m/g;
  const parts = [];
  let lastIndex = 0;
  let currentClasses = new Set();
  let currentStyle = '';

  const buildSpanFor = (textPart) => {
    if (currentClasses.size > 0 || currentStyle) {
      const classAttr = currentClasses.size > 0 ? ` class="${Array.from(currentClasses).join(' ')}"` : '';
      const styleAttr = currentStyle ? ` style="${currentStyle}"` : '';
      return `<span${classAttr}${styleAttr}>${escapeHtml(textPart)}</span>`;
    }
    return escapeHtml(textPart);
  };

  let match;
  while ((match = ansiRegex.exec(text)) !== null) {
    if (match.index > lastIndex) parts.push(buildSpanFor(text.substring(lastIndex, match.index)));

    const codes = match[1].split(';');
    for (let i = 0; i < codes.length; i++) {
      const code = codes[i];
      if (code === '0') {
        currentClasses.clear(); currentStyle = '';
      } else if (ANSI_COLOR_MAP[code]) {
        const cssClass = ANSI_COLOR_MAP[code];
        if (cssClass !== 'ansi-reset') {
          if (code.startsWith('3') || code.startsWith('9')) {
            Array.from(currentClasses).forEach(cls => {
              if (cls.startsWith('ansi-') && !cls.startsWith('ansi-bg-') && !cls.startsWith('ansi-bold') && !cls.startsWith('ansi-italic') && !cls.startsWith('ansi-underline')) currentClasses.delete(cls);
            });
          } else if (code.startsWith('4')) {
            Array.from(currentClasses).forEach(cls => { if (cls.startsWith('ansi-bg-')) currentClasses.delete(cls); });
          }
          currentClasses.add(cssClass);
        }
      } else if (code === '38' && codes[i+1] === '5' && typeof codes[i+2] !== 'undefined') {
        const n = parseInt(codes[i+2], 10);
        if (!Number.isNaN(n)) currentStyle = appendStyle(currentStyle, `color: ${ansi256ToHex(n)}`);
        i += 2;
      } else if (code === '48' && codes[i+1] === '5' && typeof codes[i+2] !== 'undefined') {
        const n = parseInt(codes[i+2], 10);
        if (!Number.isNaN(n)) currentStyle = appendStyle(currentStyle, `background-color: ${ansi256ToHex(n)}`);
        i += 2;
      } else if (code === '38' && codes[i+1] === '2' && typeof codes[i+4] !== 'undefined') {
        const r = parseInt(codes[i+2],10), g = parseInt(codes[i+3],10), b = parseInt(codes[i+4],10);
        if (![r,g,b].some(v => Number.isNaN(v))) currentStyle = appendStyle(currentStyle, `color: rgb(${r},${g},${b})`);
        i += 4;
      } else if (code === '48' && codes[i+1] === '2' && typeof codes[i+4] !== 'undefined') {
        const r = parseInt(codes[i+2],10), g = parseInt(codes[i+3],10), b = parseInt(codes[i+4],10);
        if (![r,g,b].some(v => Number.isNaN(v))) currentStyle = appendStyle(currentStyle, `background-color: rgb(${r},${g},${b})`);
        i += 4;
      }
    }

    lastIndex = ansiRegex.lastIndex;
  }

  if (lastIndex < text.length) parts.push(buildSpanFor(text.substring(lastIndex)));
  return parts.join('');
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML.replace(/\n/g, '<br>');
}

export function stripAnsi(text) {
  if (!text) return '';
  return text.replace(/\x1B\[[0-9;]+m/g, '');
}

function translateRomToAnsi(text) {
  if (!text) return text;
  const map = {
    '0': '\x1B[0m', '1': '\x1B[31m', '2': '\x1B[32m', '3': '\x1B[33m', '4': '\x1B[34m',
    '5': '\x1B[35m', '6': '\x1B[36m', '7': '\x1B[37m', '8': '\x1B[90m', '9': '\x1B[91m'
  };
  const curly = {
    'k': '\x1B[30m', 'K': '\x1B[1m\x1B[30m', 'r': '\x1B[31m', 'R': '\x1B[1m\x1B[31m',
    'g': '\x1B[32m', 'G': '\x1B[1m\x1B[32m', 'y': '\x1B[33m', 'Y': '\x1B[1m\x1B[33m',
    'b': '\x1B[34m', 'B': '\x1B[1m\x1B[34m', 'm': '\x1B[35m', 'M': '\x1B[1m\x1B[35m',
    'c': '\x1B[36m', 'C': '\x1B[1m\x1B[36m', 'w': '\x1B[37m', 'W': '\x1B[1m\x1B[37m',
    'x': '\x1B[0m', 'X': '\x1B[0m'
  };

    text = text.replace(/\{([A-Za-z0-9])\}?/g, (m, p1) => {
    if (curly[p1]) return curly[p1];
    if (map[p1]) return map[p1];
    return m;
  });
   text = text.replace(/<([0-9]{1,3})>/g, (m, p1) => {
     // if it's a numeric 0-255 value, treat as 256-color SGR
     if (/^\d+$/.test(p1)) {
       const n = parseInt(p1, 10);
       if (!Number.isNaN(n) && n >= 0 && n <= 255) return `\x1B[38;5;${n}m`;
     }
     return map[p1] || m;
   });
  return text;
}

function appendStyle(existing, addition) {
  if (!existing) return addition;
  return `${existing}; ${addition}`;
}

function ansi256ToHex(n) {
  if (n >= 0 && n <= 15) {
    const basic = ['#000000','#800000','#008000','#808000','#000080','#800080','#008080','#c0c0c0','#808080','#ff0000','#00ff00','#ffff00','#0000ff','#ff00ff','#00ffff','#ffffff'];
    return basic[n] || '#000000';
  }
  if (n >= 16 && n <= 231) {
    const idx = n - 16;
    const r = Math.floor(idx / 36);
    const g = Math.floor((idx % 36) / 6);
    const b = idx % 6;
    const conv = v => v === 0 ? 0 : 55 + v * 40;
    const rr = conv(r), gg = conv(g), bb = conv(b);
    return `#${rr.toString(16).padStart(2,'0')}${gg.toString(16).padStart(2,'0')}${bb.toString(16).padStart(2,'0')}`;
  }
  if (n >= 232 && n <= 255) {
    const c = 8 + (n - 232) * 10;
    return `#${c.toString(16).padStart(2,'0')}${c.toString(16).padStart(2,'0')}${c.toString(16).padStart(2,'0')}`;
  }
  return '#000000';
}

