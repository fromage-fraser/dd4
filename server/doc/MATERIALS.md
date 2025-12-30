# Materials System Documentation

## Overview

The materials system allows area builders to specify the material composition of weapons and armour items in the game. This information is visible to players via the `identify` skill and to immortals via the `ostat` command.

## Purpose

Materials provide:
- Immersive world-building by describing what items are made from
- Potential for future gameplay mechanics (durability, elemental interactions, crafting)
- Rich flavor text that enhances item descriptions

## Area File Format

### Material Tag: T

The `T` tag is used in area files to specify an item's material composition. It should be placed after the item's basic properties (weight, cost, level) and before any affects (`A`), extra descriptions (`E`), or max instances (`M`) tags.

### Format

```
#<VNUM>
<name>~
<short_description>~
<long_description>~
~
<item_type> <extra_flags> <wear_flags>
<value0>~ <value1>~ <value2>~ <value3>~
<weight> <cost> <level>
T
<material>~
[A/E/M tags follow...]
```

### Example: Short Sword

```
#7211
sword short~
a short sword~
There lies a short sword.~
~
5 0 8193
0~ 2~ 4~ 11~
4 60 20
T
steel/wood/leather~
A
18 1
```

### Example: Dragon Armour

```
#7304
red torso~
a red dragon torso~
There is a pile of red dragonscale on the floor.~
~
9 64|512 9
9~ 0~ 0~ 0~
30 50000 12000
T
dragonscale~
A
2 -1
A
19 2
```

## Material Types

Common material types used in fantasy settings:

### Metals
- `steel` - Common weapon and armour material
- `iron` - Basic metal, less refined than steel
- `silver` - Precious metal, often used for jewelry and fine armour
- `platinum` - Rare precious metal
- `mithril` - Magical lightweight metal (if exists in your world)
- `adamantite` - Extremely hard magical metal
- `titanium` - Strong, lightweight metal

### Organic Materials
- `leather` - Tanned animal hide
- `wood` - For handles, shields, staves
- `bone` - From creatures, often dragon parts
- `cloth` - For cloaks, robes, padding
- `silk` - Fine fabric
- `fur` - Animal pelts
- `organic` - Generic biological material (e.g., tentacles)

### Special Materials
- `dragonscale` - Scales from dragons, highly protective
- `exotic` - Mysterious or magical materials
- `obsidian` - Volcanic glass, sharp and dark
- `crystal` - Crystalline materials
- `stone` - Rock-based items

## Multiple Materials

Items can be composed of multiple materials, separated by forward slashes (`/`). List materials in order of prominence:

```
steel/wood/leather  - Steel blade with wooden handle wrapped in leather
silver/steel        - Silver plating over steel base
bone/dragonscale    - Bone reinforced with dragonscale
```

## Implementation Details

### Code Structure

Materials are stored as strings in both:
- `OBJ_INDEX_DATA->material` - The prototype/template
- `OBJ_DATA->material` - Runtime object instances

### Loading

The material is loaded from the area file by `load_objects()` in `db.c`:
```c
else if ( letter == 'T' )
{
    pObjIndex->material = fread_string( fp );
}
```

### Display

Materials are shown to players in two contexts:

1. **Identify Spell** (`spell_identify` in `magic.c`):
   ```
   It is made of steel/wood/leather.
   ```

2. **Ostat Command** (`do_ostat` in `act_wiz.c`):
   ```
   Material: steel/wood/leather
   ```

## Examples from sewer.are

### Weapons
- Devil Rod: `exotic/obsidian` - Magical item with black obsidian base
- Large Mace: `iron/organic` - Iron head with organic (tentacle) components
- Short Sword: `steel/wood/leather` - Traditional construction
- Dragon Claw: `bone/dragonscale` - Dragon body part

### Armour
- Silver Ring: `silver` - Pure silver jewelry
- Purple Cloak: `cloth` - Fabric garment
- White Skull: `bone` - Bone helmet
- Muddy Boots: `leather` - Leather footwear
- Silvery Breast Plate: `silver/steel` - Silver-plated steel armour
- Silvery Gloves: `silver` - Silver thread construction
- Platinum Helmet: `platinum` - Precious metal helm
- Shield of the Rose: `steel/wood` - Steel-faced wooden shield
- Dragon Torso: `dragonscale` - Pure dragonscale armour
- Dragonscale Leggings: `dragonscale` - Dragon scale construction
- Red Dragonhelm: `bone/dragonscale` - Dragon skull with scales
- Red Dragon Shield: `dragonscale/bone` - Scale-covered bone shield

## Guidelines for Area Builders

1. **Be descriptive but concise**: Use 1-3 materials maximum
2. **Match item descriptions**: Ensure materials align with the item's description text
3. **Order by prominence**: List the most significant material first
4. **Use standard terms**: Stick to the common material types listed above for consistency
5. **Consider the item type**:
   - Weapons: Often have blades (metal) and handles (wood/leather)
   - Armour: Usually metal, leather, or cloth
   - Jewelry: Precious metals and gems
   - Shields: Often wood with metal reinforcement

## Future Enhancements

The materials system is designed to support future features such as:
- Durability and repair mechanics based on material type
- Elemental resistances/weaknesses
- Crafting and smithing systems
- Material-based special abilities
- Weight and cost calculations
- Environmental effects (rust, decay, etc.)

## Backward Compatibility

The `T` tag is optional. Items without materials will function normally:
- `obj->material` will be NULL
- Identify and ostat will simply not display material information
- All existing area files continue to work without modification

## Version History

- Initial implementation: Added `T` tag support and material fields to object structures
- Applied to sewer.are: 24 weapons and armour items updated with appropriate materials
