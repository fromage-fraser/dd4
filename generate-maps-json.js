const fs = require('fs');
const path = require('path');

// Read the index.html file
const html = fs.readFileSync('maps/area/index.html', 'utf-8');

// Parse map entries - extract href, name, and level info
const regex = /<a href="([^"]+)">([^<]+)<\/a><\/h2><div class="details">([^<]+)/g;
const maps = {};
let match;

while ((match = regex.exec(html)) !== null) {
  const filename = match[1];
  const name = match[2]
    .replace(/&#39;/g, "'")
    .replace(/&amp;/g, "&")
    .replace(/&mdash;/g, "-");
  
  // Extract just the level range (before the author name)
  const detailsText = match[3]
    .replace(/&mdash;/g, "-")
    .replace(/&#39;/g, "'")
    .replace(/&amp;/g, "&");
  
  const levels = detailsText.split(' -')[0].trim();
  
  maps[filename] = {
    filename: filename,
    name: name,
    levels: levels
  };
}

// Create client/public directory if it doesn't exist
const publicDir = path.join(__dirname, 'client', 'public');
if (!fs.existsSync(publicDir)) {
  fs.mkdirSync(publicDir, { recursive: true });
}

// Write maps.json
const outputPath = path.join(publicDir, 'maps.json');
fs.writeFileSync(outputPath, JSON.stringify(maps, null, 2));
console.log(`Generated ${outputPath} with ${Object.keys(maps).length} maps`);
