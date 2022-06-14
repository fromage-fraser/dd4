# Builders

Information for area builders.

> This document is work in progress!


## Area file schema

Area files are stored in the _server/area/_ directory and use with file extension `.are`.

Area files that are loaded by the server are defined in the file _server/area/area.lst_.
They are processed in the order they appear in that file.
Areas can safely refer to entities defined in previously loaded area files, but not to entities in later files.


### Entities and vnums

Key entities in the MUD world are mobiles, objects (items) and rooms.
These entities are identified by _vnums_: numeric IDs that are scoped by entity type.
This means there is no globally unique vnum: room #123 and mobile #123 can both exist as separate entities.
What type of entity a vnum refers to must be provided by some additional context.


### Section `#OBJECTS`

Object (item) definitions.

Objects have a type and up to four _values_ associated with them.
What each value represents depends on the item type.
Some values are no longer used by the server: they would have been used by earlier versions of the codebase.
Old area files can have these redundant values still set.


#### Item types and values

> The four values are counted from zero to make it easier to cross-reference with code.

| ID    | Type          | Value | Value purpose
| ----- | ------------- | ----- | -------------
| 2     | scroll        | 0     | Spell level
|       |               | 1     | Spell one name
|       |               | 2     | Spell two name (optional)
|       |               | 3     | Spell three name (optional)
|       |               |       |
| 3     | wand          | 0     | Spell level
|       |               | 1     | Max charges
|       |               | 2     | Current charges
|       |               | 3     | Spell name
|       |               |       |
| 4     | staff         | 0     | Spell level
|       |               | 1     | Max charges
|       |               | 2     | Current charges
|       |               | 3     | Spell name
|       |               |       |
| 5     | weapon        | 0     | -
|       |               | 1     | -
|       |               | 2     | -
|       |               | 3     | Attack type (see below)
|       |               |       |
| 10    | potion        | 0     | Spell level
|       |               | 1     | Spell one name
|       |               | 2     | Spell two name (optional)
|       |               | 3     | Spell three name (optional)
|       |               |       |
| 15    | container     | 0     | Capacity
|       |               | 1     | Flags (see below)
|       |               | 2     | Key vnum
|       |               | 3     | -
|       |               |       |
| 26    | pill          | 0     | Spell level
|       |               | 1     | Spell one name
|       |               | 2     | Spell two name (optional)
|       |               | 3     | Spell three name (optional)
|       |               |       |
| 28    | paint         | 0     | Spell level
|       |               | 1     | Spell one name
|       |               | 2     | Spell two name (optional)
|       |               | 3     | Spell three name (optional)


#### Attack types

| Value | Description   | Class
| ----- | -----------   | -----
| 0     | hit           |
| 1     | slice         | Bladed
| 2     | stab          | Bladed, piercing
| 3     | slash         | Bladed
| 4     | whip          |
| 5     | claw          |
| 6     | blast         | Blunt
| 7     | pound         | Blunt
| 8     | crush         | Blunt
| 9     | grep          |
| 10    | bite          |
| 11    | pierce        | Bladed, piercing
| 12    | suction       |
| 13    | chop          | Bladed
| 14    | rake          |
| 15    | swipe         |
| 16    | sting         | Bladed


#### Container flags

| Flag  | Description
| ----- | -----------
| 1     | Closeable
| 2     | Pick-proof
| 4     | Closed
| 8     | Locked

