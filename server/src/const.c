/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"


/*
 *  Class table.
 */
const struct class_type class_table [ MAX_CLASS ] =
{
        /*
         * who name, show name,
         * prime attribute, first weapon,
         * guild vnum, max skill level, thac0_0, thac0_47, hp_min, hp_max, gain mana,
         * sub-1 who name, sub-2 who name, sub-1 change name, sub-2 change name
         * stat modifiers (str, int, wis, dex, con)
         */

        {
                "Mag",  "Mage",
                APPLY_INT,  OBJ_VNUM_SCHOOL_DAGGER,
                3018,  95,  18,  6,  6,  9,  TRUE,
                "Necromncr",  "Warlock",  "Nec",  "Wlk",
                { -1, 3, 1, 1, -1 }
        },

        {
                "Cle",  "Cleric",
                APPLY_WIS, OBJ_VNUM_SCHOOL_MACE,
                3003,  95,  18,  9,  8,  11,  TRUE,
                "Templar",  "Druid",  "Tem",  "Dru",
                { 1, 1, 3, -1, -1 }
        },

        {
                "Thi",  "Thief",
                APPLY_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
                3028,  85,  18,  3,  9,  12,  FALSE,
                "Ninja",  "B. Hunter",  "Nin",  "Bou",
                { 1, 1, -1, 3, -1 }
        },

        {
                "War",  "Warrior",
                APPLY_STR,  OBJ_VNUM_SCHOOL_SWORD,
                3022,  85,  18,  0,  12,  15,  FALSE,
                "Thug",  "Knight",  "Thg",  "Kni",
                { 3, -1, -2, 1, 2 }
        },

        {
                "Psi",  "Psionic",
                APPLY_WIS,  OBJ_VNUM_SCHOOL_DAGGER,
                3151,  95,  18,  9,  7,  10,  TRUE,
                "Satanist",  "Witch",  "Sat",  "Wit",
                { -1, 2, 2, 0, 0 }
        },

        {
                "Sft",  "Shifter",
                APPLY_CON,  OBJ_VNUM_SCHOOL_SWORD,
                3220,  90,  18,  0,  13, 16,  TRUE,
                "Werewolf",  "Vampire ",  "Wer",  "Vam",
                { 1, 0, 0, -1, 3 }
        },

        {
                "Bra",  "Brawler",
                APPLY_DEX,  OBJ_VNUM_SCHOOL_NULL,
                3207,  85,  18,  0,  13, 16,  FALSE,
                "Monk",  "M. Artist",  "Mon",  "Mar",
                { 1, -2, -2, 3, 3 }
        },

        {
                "Rng",  "Ranger",
                APPLY_DEX,  OBJ_VNUM_SCHOOL_SWORD,
                2,  85, 18,  0,  10, 13,  FALSE,
                "Barbarian",  "Bard",  "Brb",  "Brd",
                { 1, -1, -1, 2, 2 }
        }
};


/*
 *  SUB_CLASS TABLE - Brutus
 */
const struct sub_class_type sub_class_table [ MAX_SUB_CLASS ] =
{
        /*  who name, show name, attribute bonus, mana bonus  */

        {       "Non",  "None",                 APPLY_STR,      FALSE   },
        {       "Nec",  "Necromncr",            APPLY_WIS,      TRUE    },
        {       "Wlk",  "Warlock",              APPLY_STR,      TRUE    },
        {       "Tem",  "Templar",              APPLY_STR,      TRUE    },
        {       "Dru",  "Druid",                APPLY_INT,      TRUE    },
        {       "Nin",  "Ninja",                APPLY_CON,      FALSE   },
        {       "Bou",  "B. Hunter",            APPLY_STR,      FALSE   },
        {       "Thg",  "Thug",                 APPLY_CON,      FALSE   },
        {       "Kni",  "Knight",               APPLY_WIS,      TRUE    },
        {       "Sat",  "Satanist",             APPLY_STR,      TRUE    },
        {       "Wit",  "Witch",                APPLY_INT,      TRUE    },
        {       "Wer",  "Werewolf",             APPLY_STR,      FALSE   },
        {       "Vam",  "Vampire",              APPLY_STR,      FALSE   },
        {       "Mon",  "Monk",                 APPLY_WIS,      TRUE    },
        {       "Mar",  "M. Artist",            APPLY_STR,      FALSE   },
        {       "Brb",  "Barbarian",            APPLY_STR,      FALSE   },
        {       "Brd",  "Bard",                 APPLY_DEX,      TRUE    }
};


/*
 *  Clan tables
 */
const struct clan_type clan_table [ MAX_CLAN ] =
{
        /*
         * name, who name, transfer room, guard_room, heal_room, independent,
         * colour code letter (for {X escapes)
         *
         * 'independent' should be FALSE for a normal clan
         */

        {       "No clan",                      "   ",  3001,   3001,   3001,   TRUE,   'x'     },
        {       "The Travellers",               "TRV",   534,    530,    533,   FALSE,  'G'     },
        {       "Clan Aesir",                   "AES",   963,    960,    964,   FALSE,  'Y'     },
        {       "Legion of Entropy",            "LOE",   452,    450,    451,   FALSE,  'R'     },
        {       "Mercenaries of Conquest",      "MOC",   362,    360,    365,   FALSE,  'd'     },
        {       "Shadows of Midnight",          "SOM",   472,    470,    474,   FALSE,  'M'     },
        {       "Coven of Bone",                "COB",   490,    480,    484,   FALSE,  'd'     }
};


const struct clan_items clan_item_list [ MAX_CLAN ] =
{
        /* clan, level 1, level 30, level 60, pouch */

        {       "No",     0,      0,      0,      0             },
        {       "TRV",  530,    531,    532,    533             },
        {       "AES",  974,    975,    976,    970             },
        {       "LOE",  458,    459,    461,    460             },
        {       "MOC",  368,    369,    370,    371             },
        {       "SOM",  476,    477,    479,    478             },
        {       "COB",  488,    489,    491,    490             }
};

const struct imbue_types imbue_list [ MAX_IMBUE ] = 
{
        /* short name, description, modifer, level obtainable */
        { "to_damage", "More damage", "APPLY_DAMROLL", 40 },
        { "to_hit", "More damage", "APPLY_HITROLL", 40 },
        { "to_mana", "More damage", "APPLY_MANA", 40 },
        { "to_hps", "More damage", "APPLY_HIT", 40 },
        { "to_move", "More damage", "APPLY_MOVE", 40 },
        { "to_str", "More damage", "APPLY_STR", 40 },
        { "to_dex", "More damage", "APPLY_DEX", 40 },
        { "to_damage", "More damage", "APPLY_DAMROLL", 40 },
        { "to_ac", "More damage", "APPLY_AC", 40 },
};


const struct HERB herb_table [ MAX_HERBS ] =
{
        /*
         * Herb table legend:
         * name, item type, spell name, minimum skill needed to find,
         * chance of finding, keywords, short_desc, long_descr,
         * action string\n\r (reported if herb found after gathering)
         */

        {
                "thurl", ITEM_POTION, "cure serious", 1, 60,
                "thurl leaf brew", "a brew of thurl leaf",
                "A brew of thurl leaf is here.",
                "You find some thurl leaf and prepare a brew.\n\r"
        },

        {
                "gariig", ITEM_PILL, "cure critical", 1, 45,
                "gariig cactus", "some gariig cactus",
                "Some gariig cactus is here.",
                "You locate a tiny gariig cactus in the rocks.\n\r"
        },

        {
                "gefnul", ITEM_PILL, "heal", 1, 30,
                "gefnul lichen", "some gefnul lichen",
                "Some gefnul lichen lies here.",
                "You find and harvest a clump of gefnul lichen.\n\r"  },

        {
                "draaf", ITEM_PILL, "power heal", 85, 10,
                "draaf leaf sprig", "a sprig of draaf",
                "You spot a sprig of draaf leaf here.",
                "You gather some leaves from a tall draaf plant.\n\r"
        },

        {
                "athelas", ITEM_POTION, "complete heal", 95, 3,
                "athelas brew", "a brew of athelas herb",
                "A brew of athelas herb is here.",
                "You find some athelas leaves, crush them and make a brew.\n\r"
        },

        {
                "arkasu", ITEM_PAINT, "refresh", 1, 60,
                "arkasu paste", "some arkasu paste",
                "Some arkasu herb paste lies here.",
                "You find some arkasu herbs and grind them into a thick paste.\n\r"
        },

        {
                "attanar", ITEM_PAINT, "cure poison", 1, 40,
                "attanar moss", "some attanar moss",
                "A clump of attanar moss sits here.",
                "You recover some attanar moss and mash it into a salve.\n\r"
        },

        {
                "karfar", ITEM_PAINT, "poison", 1, 10,
                "karfar leaf paste", "some karfar leaf paste",
                "Some karfar leaf paste is here.",
                "You gather some karfar leaves and grind them into a paste.\n\r"
        },

        {
                "dynallca", ITEM_PAINT, "weaken", 1, 10,
                "dynallca leaf paste", "a paste made from dynallca leaves",
                "A paste made from dynallca leaves lies here.",
                "You locate some dynallca leaves and mash them into a pulp.\n\r"
        },

        {
                "zur", ITEM_POTION, "cause serious", 1, 5,
                "zur fungus brew", "a brew made from zur fungus",
                "You spot a brew made from zur fungus.",
                "You find zur fungus growing on some trees, and make a strong brew.\n\r"
        },

        {
                "slota", ITEM_PILL, "sleep", 1, 5,
                "slota bark", "a piece of slota bark",
                "Some bark from the slota tree lies here.",
                "You find a bushy slota tree and harvest some of the chewy bark.\n\r"
        }
};


const struct song song_table [ MAX_SONGS ] =
{
        {       "revelation",   "Ode to Siope the All Seeing",          "song of revelation",   0               },
        {       "rejuvenation", "Hymn of Ealindel the Healer",          "song of rejuvenation", 0               },
        {       "tranquility",  "Lay of the Elfin Queen",               "song of tranquility",  AFF_DETER       },
        {       "shadows",      "Lament of Dracos-kar",                 "song of shadows",      0               },
        {       "divination",   "Song of Saint Brutus the Enlightened", "song of divination",   0               },
        {       "sustenance",   "Rime of the Wearie Adventurer",        "song of sustenance",   0               },
        {       "flight",       "Ballad of the Pegasus",                "song of flight",       AFF_FLYING      }
};


const struct pattern_points pattern_list [ MAX_PATTERN ] =
{
        {     0 },
        {  3017 },
        { 10300 },
        {   693 },
        { 30249 },
        {  1390 },
        { 31061 },
        { 30038 },
        { 28003 },
};

const struct soar_points soar_list [ MAX_SOAR ] =
{
        {     0 },
        {  6125 },
        {  5272 },
        {   305 },
        { 18410 },
        { 18530 },
        {   510 },
        { 18208 },
        { 20400 },
        {   753 },
};


/*
 * Clan titles
 * Remember to leave a space after the end of each clan title.
 * This is pretty hacky but it keeps who clean.
 * If anyone wants to clean the table below up, feel free - Shade
 */
char* const clan_title [ MAX_CLAN ] [ MAX_CLAN_LEVEL + 1 ] =
{
        { "", "",                       "",                             ""                      },
        { "", "[{WGypsy{x] ",           "[{WWanderer{x] ",              "[{WAdventurer{x] "     },
        { "", "[{WBerserker{x] ",       "[{WJarl{x] ",                  "[{WEinherjar{x] "      },
        { "", "[{WBushido{x] ",         "[{WSamurai{x] ",               "[{WKensai{x] "         },
        { "", "[{WAuxiliary{x] ",       "[{WLegionnaire{x] ",           "[{WMercenary{x] "      },
        { "", "[{WApprentice{x] ",      "[{WDragoon{x] ",               "[{WCouncillor{x] "     },
        { "", "[{WAcolyte{x] ",         "[{WScion{x] ",                 "[{WParagon{x] "        }
};


char * const color_list [MAX_COLOR_LIST] =
{
        "red",
        "blue",
        "black",
        "turquoise",
        "yellow",
        "faded indigo",
        "purple",
        "magenta",
        "lavender",
        "green",
        "white",
        "faded orange",
        "violet",
        "maroon",
        "mauve",
        "brown",
        "tan",
        "grey"
};


int size_index [ 3 ] =
{
        ITEM_SIZE_SMALL,
        ITEM_SIZE_MEDIUM,
        ITEM_SIZE_LARGE
};


char * const size_names [ 3 ] =
{
        "small",
        "man-sized",
        "large"
};


/*
 *  Race information
 */
const struct race_struct race_table [MAX_RACE] =
{
        /*
         * who name (padded), race name,
         * str int wis dex con adjustments,
         * hp regen, mana regen, move regen
         * race skill 1, race skill 2,
         * size
         */

        {
                "None   ", "None",
                0, 0, 0, 0, 0,
                0, 0, 0,
                "NULL", "NULL",
                0
        },

        {
                "Human  ", "Human",
                0, 1, 0, 0, 0,
                0, 0, 0,
                "Identify", "Detect Evil",
                CHAR_SIZE_MEDIUM
        },

        {
                "Elf    ", "Elf",
                -2, 2, 1, 1, -1,
                -20, 20, 10,
                "Infravision", "Refresh",
                CHAR_SIZE_MEDIUM
        },

        {
                "WildElf", "Wild-Elf",
                1, -1, -2, 1, 2,
                10, -10, 10,
                "Infravision", "Forage",
                CHAR_SIZE_MEDIUM
        },

        {
                "Orc    ", "Orc",
                2, -1, -1, -1, 2,
                20, -20, 10,
                "Trip", "Dirt Kick",
                CHAR_SIZE_MEDIUM
        },

        {
                "Giant  ", "Giant",
                3, -2, -2, -1, 3,
                30, -30, 0,
                "Giant Strength", "Bash Door",
                CHAR_SIZE_LARGE
        },

        {
                "Satyr  ", "Satyr",
                -1, -1, 2, 2, -1,
                -10, -10, 30,
                "Sleep", "Sneak",
                CHAR_SIZE_MEDIUM
        },

        {
                "Ogre   ", "Ogre",
                3, -2, -2, 0, 2,
                20, -30, 10,
                "Kick", "Stone Skin",
                CHAR_SIZE_LARGE
        },

        {
                "Goblin ", "Goblin",
                -2, 1, 3, 1, -2,
                -10, 0, 10,
                "Shield", "Hide",
                CHAR_SIZE_SMALL
        },

        {
                "HlfDrgn", "Half-Dragon",
                2, -2, 2, -1, 0,
                20, -20, 10,
                "Flight", "Dragon Shield",
                CHAR_SIZE_LARGE
        },

        {
                "Halflng", "Halfling",
                -2, 0, 1, 3, -1,
                -20, 10, 30,
                "Sneak", "Blindness",
                CHAR_SIZE_SMALL
        },

        {
                "Dwarf  ", "Dwarf",
                2, -2, 1, -1, 1,
                30, -20, -20,
                "Grip", "Armor",
                CHAR_SIZE_SMALL
        },

        {
                "Centaur", "Centaur",
                1, -1, -1, 2, 0,
                -10, 10, 20,
                "Kick", "Shield",
                CHAR_SIZE_LARGE
        },

        {
                "Drow   ", "Drow",
                -3, 3, 1, 2, -2,
                -30, 30, 0,
                "Sneak", "Heighten Senses",
                CHAR_SIZE_MEDIUM
        },

        {
                "Troll  ", "Troll",
                2, -1, -2, -1, 3,
                30,-30,-10,
                "Fast Healing", "Cell Adjustment",
                CHAR_SIZE_LARGE
        },

        {
                "Alaghi ", "Alaghi",
                1, -1, -1, -1, 3,
                30, -20, 10,
                "Berserk", "Bash Door",
                CHAR_SIZE_LARGE
        },

        {
                "Hobgbln", "Hobgoblin",
                1, -1, -1, 0, 2,
                10, 10, 0,
                "Resist Magic", "Teleport",
                CHAR_SIZE_MEDIUM
        },

        {
                "Yuan-ti", "Yuan-ti",
                1, 0, 0, 1, -1,
                -20, 0, 20,
                "Hide", "Resist Toxin",
                CHAR_SIZE_MEDIUM
        },

        {
                "Fae    ", "Fae",
                -3, 3, 3, 1, -3,
                -30, 30, 20,
                "Faerie Fire", "Fly",
                CHAR_SIZE_SMALL
        },

        {
                "Sahuagn", "Sahuagin",
                2, -1, -1, -1, 2,
                20, 0, 0,
                "Gouge", "Dowse",
                CHAR_SIZE_MEDIUM
        },

        {
                "Tieflng", "Tiefling",
                0, 1, 0, -2, 2,
                20, 10, -10,
                "Hand of Lucifer", "Resist Heat",
                CHAR_SIZE_MEDIUM
        },

        {
                "Jotun  ", "Jotun",
                3, -1, -2, -2, 3,
                30, -20, -10,
                "Berserk", "Resist Cold",
                CHAR_SIZE_LARGE
        },

        {
                "Genasi ", "Genasi",
                -1, 2, -1, -1, 2,
                10, 10, 0,
                "Infravision", "Resist Magic",
                CHAR_SIZE_MEDIUM
        },

        {
                "Illithd", "Illithid",
                -2, 2, 3, -2, 0,
                0, 30, -20,
                "Fear", "Energy Drain",
                CHAR_SIZE_MEDIUM
        },
};


/*
 *  Form wear restrictions
 */
const struct loc_wear_struct form_wear_table [MAX_FORM] =
{
        /*
         * light, take, finger, neck, body, head, legs, feet, hands, arms,
         * shield, about, waist, wrist, wield, held, float, pouch, ranged
         */

        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /*  normal  */
        { { 1,1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,0,1,0 } }, /*  chameleon  */
        { { 1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0 } }, /*  hawk   */
        { { 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0 } }, /*  cat  */
        { { 1,1,0,0,0,1,0,0,0,0,0,1,1,0,0,1,1,1,0 } }, /*  snake  */
        { { 1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0 } }, /*  scorpion  */
        { { 1,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0 } }, /*  spider  */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /*  bear  */
        { { 1,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,1,1,0 } }, /*  tiger  */
        { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0 } }, /*  ghost  */
        { { 1,1,0,1,0,1,0,0,0,0,0,1,1,0,1,1,1,1,0 } }, /*  hydra  */
        { { 1,1,1,1,0,1,1,1,0,0,0,1,0,1,0,1,1,1,0 } }, /*  phoenix  */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /*  demon  */
        { { 1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,0 } }, /*  dragon  */
        { { 1,1,0,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,0 } }, /*  direwolf  */
        { { 0,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0 } }, /*  vampire  */
        { { 1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,0 } }, /*  werehuman  */
        { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } }, /*  fly  */
        { { 1,1,1,1,1,1,0,0,1,1,0,1,1,1,1,1,1,1,0 } }, /*  griffin  */
        { { 1,1,0,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,0 } }  /*  werewolf  */
};


/*
 *  Class wear location restrictions
 */
const struct loc_wear_struct loc_wear_table [MAX_CLASS + MAX_SUB_CLASS - 1] =
{
        /*
         * light, take, finger, neck, body, head, legs, feet, hands, arms,
         * shield, about, waist, wrist, wield, held, float, pouch, ranged
         */

        { { 1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0 } }, /* mag */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* cle */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0 } }, /* thi */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* war */
        { { 1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* psi */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* sft */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0 } }, /* bra */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } }, /* rng */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0 } }, /* nec */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* wlk */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* tem */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* dru */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0 } }, /* nin */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* bou */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* thg */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } }, /* kni */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* sat */
        { { 1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* wit */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* wer */
        { { 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* vam */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0 } }, /* mon */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0 } }, /* mar */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* brb */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0 } }  /* brd */
};


/*
 *  Class wear type restrictions
 */
const struct wear_struct wear_table [ MAX_CLASS + MAX_SUB_CLASS - 1 ] =
{
        /*
         * null, light, scroll, wand, staff, weapon, -, -, treasure, armour, potion,
         * -, furniture, trash, -, container, -, drink con, key, food, money,
         * -, boat, NPC corpse, PC corpse, fountain, pill, climbing eq, paint, mob, anvil,
         * ticket, clan object, portal, poison powder, lock pick, instrument
         * armourer's hammer, mithril, whetstone
         */

        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 1,0,1,1,0,0,1,1,1 }}, /* mag */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* cle */
        {{ 0,1,1,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,1,0,1,1,1 }}, /* thi */
        {{ 0,1,0,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* war */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* psi */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* sft */
        {{ 0,1,0,0,0,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 0,1,1,1,0,0,1,1,1 }}, /* bra */
        {{ 0,1,1,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* rng */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* nec */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* wlk */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* tem */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* dru */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,1,0,1,1,1 }}, /* nin */
        {{ 0,1,1,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,1,0,1,1,1 }}, /* bou */
        {{ 0,1,1,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* thg */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* kni */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* sat */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* wit */
        {{ 0,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* wer */
        {{ 0,0,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,0,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* vam */
        {{ 0,1,1,1,1,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* mon */
        {{ 0,1,0,0,0,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* mar */
        {{ 0,1,1,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* brb */
        {{ 0,1,1,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,1,1,1,1,1 }}  /* brd */
};


/*
 *  Equipment slot to wear bit conversion table; Gezhp 2000
 */
const int eq_slot_to_wear_bit [ MAX_WEAR ] =
{
        BIT_LIGHT,
        BIT_WEAR_FINGER,
        BIT_WEAR_FINGER,
        BIT_WEAR_NECK,
        BIT_WEAR_NECK,
        BIT_WEAR_BODY,  /* 5 */
        BIT_WEAR_HEAD,
        BIT_WEAR_LEGS,
        BIT_WEAR_FEET,
        BIT_WEAR_HANDS,
        BIT_WEAR_ARMS,  /* 10 */
        BIT_WEAR_SHIELD,
        BIT_WEAR_ABOUT,
        BIT_WEAR_WAIST,
        BIT_WEAR_WRIST,
        BIT_WEAR_WRIST, /* 15 */
        BIT_WIELD,
        BIT_HOLD,
        BIT_WIELD,
        BIT_FLOAT,
        BIT_WEAR_POUCH, /* 20 */
        BIT_RANGED_WEAPON
};


/*
 *  Form allowed skills
 */
struct form_skill_struct form_skill_table [ MAX_FORM_SKILL ] =
{
        {       &gsn_hide,                      FORM_NONE               },
        {       &gsn_sneak,                     FORM_NONE               },
        {       &gsn_morph,                     FORM_ALL                },
        {       &gsn_venom,                     FORM_SPIDER             },
        {       &gsn_soar,                      FORM_HAWK               },
        {       &gsn_swim,                      FORM_SNAKE              },
        {       &gsn_constrict,                 FORM_SNAKE              },
        {       &gsn_coil,                      FORM_SNAKE              },
        {       &gsn_strangle,                  FORM_SNAKE              },
        {       &gsn_backstab,                  FORM_SCORPION           },
        {       &gsn_second_attack,             FORM_BEAR               },
        {       &gsn_third_attack,              FORM_BEAR               },
        {       &gsn_enhanced_damage,           FORM_BEAR               },
        {       &gsn_second_attack,             FORM_TIGER              },
        {       &gsn_dual,                      FORM_TIGER              },
        {       &gsn_berserk,                   FORM_TIGER              },
        {       &gsn_forage,                    FORM_BEAR               },
        {       &gsn_crush,                     FORM_BEAR               },
        {       &gsn_bite,                      FORM_TIGER              },
        {       &gsn_maul,                      FORM_TIGER              },
        {       &gsn_web,                       FORM_SPIDER             },
        {       &gsn_venom,                     FORM_SPIDER             },
        {       &gsn_breathe,                   FORM_HYDRA              },
        {       &gsn_breathe,                   FORM_DRAGON             },
        {       &gsn_whirlwind,                 FORM_HYDRA              },
        {       &gsn_second_attack,             FORM_HYDRA              },
        {       &gsn_third_attack,              FORM_HYDRA              },
        {       &gsn_second_attack,             FORM_PHOENIX            },
        {       &gsn_rescue,                    FORM_PHOENIX            },
        {       &gsn_swoop,                     FORM_PHOENIX            },
        {       &gsn_second_attack,             FORM_DRAGON             },
        {       &gsn_enhanced_damage,           FORM_DRAGON             },
        {       &gsn_dual,                      FORM_DRAGON             },
        {       &gsn_dive,                      FORM_DRAGON             },
        {       &gsn_second_attack,             FORM_GRIFFIN            },
        {       &gsn_maul,                      FORM_GRIFFIN            },
        {       &gsn_third_attack,              FORM_GRIFFIN            },
        {       &gsn_enhanced_damage,           FORM_GRIFFIN            },
        {       &gsn_dual,                      FORM_GRIFFIN            },
        {       &gsn_tailwhip,                  FORM_DRAGON             },
        {       &gsn_wolfbite,                  FORM_WOLF               },
        {       &gsn_ravage,                    FORM_WOLF               },
        {       &gsn_dual,                      FORM_WOLF               },
        {       &gsn_berserk,                   FORM_WOLF               },
        {       &gsn_howl,                      FORM_WOLF               },
        {       &gsn_second_attack,             FORM_WOLF               },
        {       &gsn_third_attack,              FORM_WOLF               },
        {       &gsn_fourth_attack,             FORM_WOLF               },
        {       &gsn_rage,                      FORM_WOLF               },
        {       &gsn_dodge,                     FORM_WOLF               },
        {       &gsn_fast_healing,              FORM_WOLF               },
        {       &gsn_enhanced_hit,              FORM_WOLF               },
        {       &gsn_resist_magic,              FORM_WOLF               },
        {       &gsn_resist_toxin,              FORM_WOLF               },
        {       &gsn_hunt,                      FORM_WOLF               },
        {       &gsn_gaias_warning,             FORM_WOLF               },
        {       &gsn_wolfbite,                  FORM_DIREWOLF           },
        {       &gsn_ravage,                    FORM_DIREWOLF           },
        {       &gsn_dual,                      FORM_DIREWOLF           },
        {       &gsn_berserk,                   FORM_DIREWOLF           },
        {       &gsn_howl,                      FORM_DIREWOLF           },
        {       &gsn_second_attack,             FORM_DIREWOLF           },
        {       &gsn_third_attack,              FORM_DIREWOLF           },
        {       &gsn_fourth_attack,             FORM_DIREWOLF           },
        {       &gsn_rage,                      FORM_DIREWOLF           },
        {       &gsn_dodge,                     FORM_DIREWOLF           },
        {       &gsn_fast_healing,              FORM_DIREWOLF           },
        {       &gsn_enhanced_hit,              FORM_DIREWOLF           },
        {       &gsn_resist_magic,              FORM_DIREWOLF           },
        {       &gsn_resist_toxin,              FORM_DIREWOLF           },
        {       &gsn_hunt,                      FORM_DIREWOLF           },
        {       &gsn_gaias_warning,             FORM_DIREWOLF           },
        {       &gsn_summon_demon,              FORM_DEMON              }
};


/*
 * Vampire disallow table
 * Ugly hack to hide some skills from vampires and werewolves
 */
struct vampire_gag vampire_gag_table [ MAX_VAMPIRE_GAG ] =
{
        {       &gsn_group_forms                },
        {       &gsn_form_chameleon             },
        {       &gsn_form_hawk                  },
        {       &gsn_form_cat                   },
        {       &gsn_form_bat                   },
        {       &gsn_form_snake                 },
        {       &gsn_form_scorpion              },
        {       &gsn_form_spider                },
        {       &gsn_form_bear                  },
        {       &gsn_form_tiger                 },
        {       &gsn_venom                      },
        {       &gsn_soar                       },
        {       &gsn_swim                       },
        {       &gsn_constrict                  },
        {       &gsn_coil                       },
        {       &gsn_web                        },
        {       &gsn_strangle,                  },
        {       &gsn_forage                     },
        {       &gsn_tailwhip                   },
        {       &gsn_bite                       },
        {       &gsn_maul                       },
        {       &gsn_crush                      },
        {       &gsn_swoop                      },

        /*
         * Skills below gagged by vamps but shown by werewolves
         * Need to edit prac_slist() if the number of wolf-only
         * skills is changed
         */
        {       &gsn_morph                      },
        {       &gsn_wolfbite                   },
        {       &gsn_ravage                     },
};


/*
 * A single set of colors for use with the COLOR command. :)
 */
const struct color_data color_table [] =
{
        { "\x1b[30m",           "$R$0",         "black",                 0 },
        { "\x1b[34m",           "$R$1",         "blue",                  1 },
        { "\x1b[32m",           "$R$2",         "green",                 2 },
        { "\x1b[36m",           "$R$3",         "cyan",                  3 },
        { "\x1b[31m",           "$R$4",         "red",                   4 },
        { "\x1b[35m",           "$R$5",         "purple",                5 },
        { "\x1b[33m",           "$R$6",         "brown",                 6 },
        { "\x1b[37m",           "$R$7",         "grey",                  7 },
        { "\x1b[30;1m",         "$B$0",         "dark_grey",             8 },
        { "\x1b[34;1m",         "$B$1",         "light_blue",            9 },
        { "\x1b[32;1m",         "$B$2",         "light_green",          10 },
        { "\x1b[36;1m",         "$B$3",         "light_cyan",           11 },
        { "\x1b[31;1m",         "$B$4",         "light_red",            12 },
        { "\x1b[35;1m",         "$B$5",         "magenta",              13 },
        { "\x1b[33;1m",         "$B$6",         "yellow",               14 },
        { "\x1b[37;1m",         "$B$7",         "white",                15 },
        { "\x1b[1m",            "$B",           "bold",                 16 },
        { "\x1b[5m",            "$F",           "flashing",             17 },
        { "\x1b[7m",            "$I",           "inverse",              18 },
        { "\x1b[0m",            "$R",           "normal",               19 }
};

/*
 * To validate ch access to do_soar locations -- Owl 30/3/22
 * min gsn_hawk %, min gsn_soar %, min char level
 */

const struct soar_struct soar_access [ MAX_SOAR ] =
{
        {       0,      0,      0 },
        {       5,      5,      1 },
        {      30,     25,      5 },
        {      40,     45,     10 },
        {      50,     55,     15 },
        {      60,     65,     20 },
        {      70,     75,     30 },
        {      80,     85,     40 },
        {      90,     95,     50 },
        {      99,     99,     60 }
};


/*
 *  Old experience table
 */
const struct level_struct old_level_table [ LEVEL_HERO ] =
{
        {   1,             1000,            1000 },
        {   2,             1311,            2311 },
        {   3,             1728,            4039 },
        {   4,             2744,            6783 },
        {   5,             3375,           10158 },
        {   6,             4096,           14254 },
        {   7,             4913,           19167 },
        {   8,             5832,           24999 },
        {   9,             6859,           31858 },
        {  10,             8000,           39858 },
        {  11,             9261,           49119 },
        {  12,            10468,           59767 },
        {  13,            12167,           71934 },
        {  14,            13824,           85758 },
        {  15,            15625,          101383 },
        {  16,            17576,          118959 },
        {  17,            19683,          138642 },
        {  18,            21952,          160594 },
        {  19,            24389,          184983 },
        {  20,            27000,          211983 },
        {  21,            29991,          241974 },
        {  22,            32768,          274742 },
        {  23,            35937,          310679 },
        {  24,            39304,          349983 },
        {  25,            42875,          392858 },
        {  26,            46656,          439514 },
        {  27,            50653,          490167 },
        {  28,            54872,          545039 },
        {  29,            59319,          604358 },
        {  30,            64000,          668358 },
        {  31,            68921,          737279 },
        {  32,            74088,          811367 },
        {  33,            79507,          890874 },
        {  34,            85184,          976058 },
        {  35,            91125,         1067183 },
        {  36,            97336,         1164519 },
        {  37,           103823,         1268342 },
        {  38,           110592,         1378934 },
        {  39,           117649,         1496583 },
        {  40,           125000,         1621583 },
        {  41,           132651,         1754234 },
        {  42,           140608,         1894842 },
        {  43,           148877,         2043719 },
        {  44,           157464,         2201183 },
        {  45,           166375,         2367558 },
        {  46,           175616,         2543174 },
        {  47,           185193,         2728367 },
        {  48,           195112,         2923479 },
        {  49,           205379,         3128858 },
        {  50,           216000,         3344858 },
        {  51,           226981,         3571839 },
        {  52,           238328,         3810167 },
        {  53,           250047,         4060214 },
        {  54,           262144,         4322358 },
        {  55,           274625,         4596983 },
        {  56,           287496,         4884479 },
        {  57,           300763,         5185242 },
        {  58,           314432,         5499674 },
        {  59,           328509,         5828183 },
        {  60,           343000,         6171183 },
        {  61,           357911,         6205483 },
        {  62,           373248,         6578731 },
        {  63,           389017,         6967748 },
        {  64,           405224,         7372972 },
        {  65,           421875,         7794847 },
        {  66,           438976,         8233823 },
        {  67,           456533,         8690356 },
        {  68,           474552,         9164908 },
        {  69,           493039,         9657947 },
        {  70,           512000,        10169947 },
        {  71,           531441,        10701388 },
        {  72,           551368,        11252756 },
        {  73,           571787,        11824543 },
        {  74,           592704,        12417247 },
        {  75,           614125,        13031372 },
        {  76,           636056,        13667428 },
        {  77,           658503,        14325931 },
        {  78,           681472,        15007403 },
        {  79,           704969,        15712372 },
        {  80,           729000,        16441372 },
        {  81,           753571,        17194943 },
        {  82,           778688,        17973631 },
        {  83,           804357,        18777988 },
        {  84,           830584,        19608572 },
        {  85,           857375,        20465947 },
        {  86,           884736,        21350683 },
        {  87,           912673,        22263356 },
        {  88,           941192,        23204548 },
        {  89,           970299,        24114847 },
        {  90,           885153,        25000000 },
        {  91,          1000000,        26000000 },
        {  92,          1000000,        27000000 },
        {  93,          1000000,        28000000 },
        {  94,          1000000,        29000000 },
        {  95,          1000000,        30000000 },
        {  96,          1000000,        31000000 },
        {  97,          1000000,        32000000 },
        {  98,          1000000,        33000000 },
        {  99,          1000000,        34000000 },
        { 100,          6000000,        40000000 }
};


/*
 *  New level table
 */
const struct level_struct level_table [ LEVEL_HERO ] =
{
        {   1,             1000,            1000 },
        {   2,             1300,            2300 },
        {   3,             1700,            4000 },
        {   4,             2700,            6700 },
        {   5,             3350,           10050 },
        {   6,             4100,           14150 },
        {   7,             4900,           19050 },
        {   8,             5800,           24850 },
        {   9,             6850,           31700 },
        {  10,             8000,           39700 },
        {  11,             8800,           48500 },
        {  12,             9950,           58450 },
        {  13,            11600,           70050 },
        {  14,            13200,           83250 },
        {  15,            14850,           98100 },
        {  16,            16700,          114800 },
        {  17,            18800,          133600 },
        {  18,            20850,          154450 },
        {  19,            23200,          177650 },
        {  20,            25650,          203300 },
        {  21,            27000,          230300 },
        {  22,            29500,          259800 },
        {  23,            32350,          292150 },
        {  24,            35400,          327550 },
        {  25,            38550,          366100 },
        {  26,            41950,          408050 },
        {  27,            45550,          453600 },
        {  28,            49350,          502950 },
        {  29,            53350,          556300 },
        {  30,            57600,          613900 },
        {  31,            55150,          669050 },
        {  32,            59250,          728300 },
        {  33,            63600,          791900 },
        {  34,            68150,          860050 },
        {  35,            72900,          932950 },
        {  36,            77850,         1010800 },
        {  37,            83050,         1093850 },
        {  38,            88500,         1182350 },
        {  39,            94100,         1276450 },
        {  40,           100000,         1376450 },
        {  41,           106000,         1482450 },
        {  42,           112500,         1594950 },
        {  43,           119050,         1714000 },
        {  44,           125500,         1839500 },
        {  45,           133000,         1972500 },
        {  46,           140500,         2113000 },
        {  47,           148000,         2261000 },
        {  48,           156000,         2417000 },
        {  49,           164500,         2581500 },
        {  50,           172500,         2754000 },
        {  51,           170000,         2924000 },
        {  52,           178500,         3102500 },
        {  53,           187500,         3290000 },
        {  54,           196500,         3486500 },
        {  55,           205500,         3692000 },
        {  56,           216000,         3908000 },
        {  57,           226000,         4134000 },
        {  58,           236000,         4370000 },
        {  59,           247000,         4617000 },
        {  60,           257000,         4874000 },
        {  61,           269000,         5143000 },
        {  62,           278000,         5421000 },
        {  63,           292000,         5713000 },
        {  64,           304000,         6017000 },
        {  65,           317000,         6334000 },
        {  66,           329000,         6663000 },
        {  67,           342000,         7005000 },
        {  68,           355000,         7360000 },
        {  69,           369000,         7729000 },
        {  70,           384000,         8113000 },
        {  71,           371000,         8484000 },
        {  72,           385000,         8869000 },
        {  73,           400000,         9269000 },
        {  74,           414000,         9683000 },
        {  75,           430000,        10113000 },
        {  76,           445000,        10558000 },
        {  77,           460000,        11018000 },
        {  78,           477000,        11495000 },
        {  79,           493000,        11988000 },
        {  80,           510000,        12498000 },
        {  81,           452000,        12950000 },
        {  82,           467000,        13417000 },
        {  83,           482000,        13899000 },
        {  84,           498000,        14397000 },
        {  85,           514000,        14911000 },
        {  86,           530000,        15441000 },
        {  87,           547000,        15988000 },
        {  88,           564000,        16552000 },
        {  89,           582000,        17134000 },
        {  90,           466000,        17600000 },
        {  91,           600000,        18200000 },
        {  92,           600000,        18800000 },
        {  93,           600000,        19400000 },
        {  94,           600000,        20000000 },
        {  95,           600000,        20600000 },
        {  96,           700000,        21300000 },
        {  97,           800000,        22100000 },
        {  98,           900000,        23000000 },
        {  99,          1000000,        24000000 },
        { 100,          3000000,        27000000 }
};


/*
 * Attribute bonus tables
 */
const struct str_app_type str_app [ MAX_STAT ] =
{
        /*  to-hit, to-dam, carry weight, wield weight  */

        { -5, -4,    0,  0 },  /* 0 */
        { -5, -4,    3,  1 },
        { -3, -2,    3,  2 },
        { -3, -1,   10,  3 },
        { -2, -1,   25,  4 },
        { -2, -1,   55,  5 },  /* 5 */
        { -1,  0,   80,  6 },
        { -1,  0,   90,  7 },
        {  0,  0,  100,  8 },
        {  0,  0,  100,  9 },
        {  0,  0,  115, 10 },  /* 10 */
        {  0,  0,  115, 11 },
        {  0,  0,  140, 12 },
        {  0,  0,  140, 13 },
        {  0,  1,  170, 14 },
        {  1,  1,  200, 15 },  /* 15 */
        {  1,  2,  250, 16 },
        {  2,  3,  300, 22 },
        {  2,  4,  350, 25 },
        {  3,  5,  400, 30 },
        {  3,  6,  500, 35 },  /* 20 */
        {  4,  7,  600, 40 },
        {  5,  7,  700, 45 },
        {  6,  8,  800, 50 },
        {  8, 10,  900, 55 },
        { 10, 12,  950, 60 },  /* 25 */
        { 12, 13, 1000, 70 },
        { 13, 14, 1050, 80 },
        { 16, 16, 1100, 85 },
        { 18, 18, 1125, 90 },
        { 20, 20, 1150, 99 },  /* 30 */
        { 22, 22, 1200, 99 }
};


const struct dex_app_type dex_app [ MAX_STAT ] =
{
        /* defensive adjustment, initiative bonus */
        {   60, 0 },  /* 0 */
        {   50, 0 },
        {   50, 0 },
        {   40, 0 },
        {   30, 0 },
        {   20, 0 },  /* 5 */
        {   10, 0 },
        {    0, 0 },
        {    0, 0 },
        {    0, 0 },
        {    0, 0 },  /* 10 */
        {    0, 0 },
        {    0, 0 },
        {    0, 0 },
        {    0, 0 },
        { - 10, 0 },  /* 15 */
        { - 15, 0 },
        { - 20, 0 },
        { - 30, 0 },
        { - 40, 0 },
        { - 50, 0 },  /* 20 */
        { - 65, 0 },
        { - 75, 0 },
        { - 90, 0 },
        { -105, 0 },
        { -120, 0 },  /* 25 */
        { -130, 1 },
        { -140, 1 },
        { -150, 2 },
        { -160, 2 },
        { -175, 3 },
        { -200, 4 }
};


const struct con_app_type con_app [ MAX_STAT ] =
{
        /*  hp, shock */

        { -4, 20 },  /* 0 */
        { -3, 25 },
        { -2, 30 },
        { -2, 35 },
        { -1, 40 },
        { -1, 45 },  /* 5 */
        { -1, 50 },
        {  0, 55 },
        {  0, 60 },
        {  0, 65 },
        {  0, 70 },  /* 10 */
        {  0, 75 },
        {  0, 80 },
        {  0, 85 },
        {  0, 88 },
        {  1, 90 },  /* 15 */
        {  2, 95 },
        {  3, 97 },
        {  4, 99 },
        {  5, 99 },
        {  6, 99 },  /* 20 */
        {  7, 99 },
        {  8, 99 },
        {  9, 99 },
        { 10, 99 },
        { 11, 99 },  /* 25 */
        { 12, 99 },
        { 13, 99 },
        { 15, 99 },
        { 16, 99 },
        { 18, 99 },  /* 30 */
        { 20, 99 }
};


/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table [ LIQ_MAX ] =
{
        { "water",                      "clear",                {  0, 0, 10 } },  /* 0 */
        { "beer",                       "amber",                {  3, 2,  5 } },
        { "wine",                       "rose",                 {  5, 2,  5 } },
        { "ale",                        "brown",                {  2, 2,  5 } },
        { "dark ale",                   "dark",                 {  1, 2,  5 } },
        { "whisky",                     "golden",               {  6, 1,  4 } },  /* 5 */
        { "lemonade",                   "pink",                 {  0, 1,  8 } },
        { "firebreather",               "boiling",              { 10, 0,  0 } },
        { "local speciality",           "everclear",            {  3, 3,  3 } },
        { "slime mould juice",          "green",                {  0, 4, -8 } },
        { "milk",                       "white",                {  0, 3,  6 } },  /* 10 */
        { "tea",                        "tan",                  {  0, 1,  6 } },
        { "coffee",                     "black",                {  0, 1,  6 } },
        { "blood",                      "red",                  {  0, 2, -1 } },
        { "salt water",                 "clear",                {  0, 1, -2 } },
        { "cola",                       "cherry",               {  0, 1,  5 } }   /* 15 */
};


/*
 *  Skill pre-reqs
 */
struct pre_req_struct pre_req_table [ MAX_PRE_REQ ] =
{
#include "pre_reqs/pre_req-common.c"
#include "pre_reqs/pre_req-thief.c"
#include "pre_reqs/pre_req-warrior.c"
#include "pre_reqs/pre_req-mage.c"
#include "pre_reqs/pre_req-knight.c"
#include "pre_reqs/pre_req-thug.c"
#include "pre_reqs/pre_req-ninja.c"
#include "pre_reqs/pre_req-brawler.c"
#include "pre_reqs/pre_req-cleric.c"
#include "pre_reqs/pre_req-templar.c"
#include "pre_reqs/pre_req-bounty.c"
#include "pre_reqs/pre_req-warlock.c"
#include "pre_reqs/pre_req-psionic.c"
#include "pre_reqs/pre_req-shifter.c"
#include "pre_reqs/pre_req-artist.c"
#include "pre_reqs/pre_req-satan.c"
#include "pre_reqs/pre_req-druid.c"
#include "pre_reqs/pre_req-necro.c"
#include "pre_reqs/pre_req-werewolf.c"
#include "pre_reqs/pre_req-witch.c"
#include "pre_reqs/pre_req-vampire.c"
#include "pre_reqs/pre_req-monk.c"
#include "pre_reqs/pre_req-bard.c"
#include "pre_reqs/pre_req-ranger.c"
#include "pre_reqs/pre_req-barbarian.c"
#include "pre_reqs/pre_req-smithy.c"
#include "pre_reqs/pre_req-engineer.c"
#include "pre_reqs/pre_req-alchemist.c"
};


/*
 *  Skill practice groups
 */
const int *spell_groups [ MAX_GROUPS ] =
{
        &gsn_group_thievery,
        &gsn_group_stealth,
        &gsn_group_hunting,
        &gsn_group_riding,
        &gsn_group_unarmed,
        &gsn_group_armed,
        &gsn_group_defense,
        &gsn_group_inner,
        &gsn_group_knowledge,
        &gsn_group_nature,
        &gsn_group_advcombat,
        &gsn_group_metal,
        &gsn_group_combos,
        &gsn_group_arts,
        &gsn_group_combat,
        &gsn_group_poison,
        &gsn_group_pugalism,
        &gsn_group_forms,
        &gsn_group_healing,
        &gsn_group_harmful,
        &gsn_group_curative,
        &gsn_group_disease,
        &gsn_group_conjuration,
        &gsn_group_protection,
        &gsn_group_advanced_heal,
        &gsn_group_divine,
        &gsn_group_alteration,
        &gsn_group_enchant,
        &gsn_group_summoning,
        &gsn_group_mana,
        &gsn_group_divination,
        &gsn_group_dark,
        &gsn_group_death,
        &gsn_group_lycanthropy,
        &gsn_group_vampyre,
        &gsn_group_evocation,
        &gsn_group_breath,
        &gsn_group_destruction,
        &gsn_group_majorp,
        &gsn_group_craft,
        &gsn_group_mentald,
        &gsn_group_matter,
        &gsn_group_energy,
        &gsn_group_telepathy,
        &gsn_group_advtele,
        &gsn_group_body,
        &gsn_group_archery,
        &gsn_group_music,
        &gsn_group_herb_lore,
        &gsn_group_morph,
        &gsn_group_resistance,
        &gsn_group_armoursmith,
        &gsn_group_last
        
};


/*
 *  Skill group members
 *  Groups MUST be in the same order as the gsn's above...
 */
struct spell_group_struct spell_group_table [MAX_SPELL_GROUP] =
{
        { &gsn_group_thievery,                          0 },
        { &gsn_pick_lock,                               0 },
        { &gsn_find_traps,                              0 },
        { &gsn_disable,                                 0 },
        { &gsn_climb,                                   0 },
        { &gsn_intimidate,                              0 },
        { &gsn_extort,                                  0 },
        { &gsn_knife_toss,                              0 },

        { &gsn_group_stealth,                           0 },
        { &gsn_hide,                                    0 },
        { &gsn_sneak,                                   0 },
        { &gsn_tail,                                    0 },
        { &gsn_backstab,                                0 },
        { &gsn_circle,                                  0 },
        { &gsn_second_circle,                           0 },
        { &gsn_peek,                                    0 },
        { &gsn_steal,                                   0 },
        { &gsn_assassinate,                             0 },
        { &gsn_double_backstab,                         0 },
        { &gsn_smoke_bomb,                              0 },

        { &gsn_group_hunting,                           0 },
        { &gsn_trap,                                    0 },
        { &gsn_hunt,                                    0 },
        { &gsn_snare,                                   0 },

        { &gsn_group_riding,                            0 },
        { &gsn_mount,                                   0 },
        { &gsn_dismount,                                0 },
        { &gsn_joust,                                   0 },
        { &gsn_destrier,                                0 },

        { &gsn_group_unarmed,                           0 },
        { &gsn_kick,                                    0 },
        { &gsn_headbutt,                                0 },
        { &gsn_second_headbutt,                         0 },
        { &gsn_stun,                                    0 },
        { &gsn_trip,                                    0 },
        { &gsn_dirt,                                    0 },
        { &gsn_unarmed_combat,                          0 },

        { &gsn_group_armed,                             0 },
        { &gsn_second_attack,                           0 },
        { &gsn_enhanced_damage,                         0 },
        { &gsn_third_attack,                            0 },
        { &gsn_fourth_attack,                           0 },
        { &gsn_feint,                                   0 },
        { &gsn_dual,                                    0 },
        { &gsn_thrust,                                  0 },

        { &gsn_group_defense,                           0 },
        { &gsn_rescue,                                  0 },
        { &gsn_dodge,                                   0 },
        { &gsn_parry,                                   0 },
        { &gsn_pre_empt,                                0 },
        { &gsn_disarm,                                  0 },
        { &gsn_grip,                                    0 },
        { &gsn_shield_block,                            0 },
        { &gsn_smash,                                   0 },
        { &gsn_acrobatics,                              0 },
        { &gsn_dual_parry,                              0 },

        { &gsn_group_inner,                             0 },
        { &gsn_bash,                                    0 },
        { &gsn_warcry,                                  0 },
        { &gsn_battle_aura,                             0 },
        { &gsn_fast_healing,                            0 },
        { &gsn_berserk,                                 0 },

        { &gsn_group_knowledge,                         0 },
        { &gsn_lore,                                    0 },
        { &gsn_advanced_consider,                       0 },
        { &gsn_enhanced_hit,                            0 },

        { &gsn_group_nature,                            0 },
        { &gsn_bark_skin,                               0 },
        { &gsn_sunray,                                  0 },
        { &gsn_moonray,                                 0 },
        { &gsn_wither,                                  0 },
        { &gsn_natures_fury,                            0 },

        { &gsn_group_advcombat,                         0 },
        { &gsn_risposte,                                0 },
        { &gsn_whirlwind,                               0 },
        { &gsn_decapitate,                              0 },
        { &gsn_focus,                                   0 },

        { &gsn_group_metal,                             0 },
        { &gsn_sharpen,                                 0 },
        { &gsn_forge,                                   0 },

        { &gsn_group_combos,                            0 },
        { &gsn_combo,                                   0 },
        { &gsn_combo2,                                  0 },
        { &gsn_combo3,                                  0 },
        { &gsn_combo4,                                  0 },

        { &gsn_group_arts,                              0 },
        { &gsn_atemi,                                   0 },
        { &gsn_kansetsu,                                0 },
        { &gsn_tetsui,                                  0 },
        { &gsn_shuto,                                   0 },
        { &gsn_yokogeri,                                0 },
        { &gsn_mawasigeri,                              0 },

        { &gsn_group_combat,                            0 },
        { &gsn_kiai,                                    0 },

        { &gsn_group_poison,                            0 },
        { &gsn_poison_weapon,                           0 },

        { &gsn_group_pugalism,                          0 },
        { &gsn_gouge,                                   0 },
        { &gsn_punch,                                   0 },
        { &gsn_grapple,                                 0 },
        { &gsn_flying_headbutt,                         0 },
        { &gsn_second_punch,                            0 },
        { &gsn_grab,                                    0 },
        { &gsn_choke,                                   0 },
        { &gsn_break_wrist,                             0 },
        { &gsn_snap_neck,                               0 },
        { &gsn_push,                                    0 },
        { &gsn_pugalism,                                0 },

        { &gsn_group_forms,                             0 },
        { &gsn_morph,                                   0 },
        { &gsn_form_chameleon,                          0 },
        { &gsn_form_hawk,                               0 },
        { &gsn_soar,                                    0 },
        { &gsn_form_cat,                                0 },
        { &gsn_form_snake,                              0 },
        { &gsn_coil,                                    0 },
        { &gsn_constrict,                               0 },
        { &gsn_strangle,                                0 },
        { &gsn_form_scorpion,                           0 },
        { &gsn_form_spider,                             0 },
        { &gsn_venom,                                   0 },
        { &gsn_web,                                     0 },
        { &gsn_form_bear,                               0 },
        { &gsn_forage,                                  0 },
        { &gsn_crush,                                   0 },
        { &gsn_form_tiger,                              0 },
        { &gsn_bite,                                    0 },
        { &gsn_maul,                                    0 },
        { &gsn_wolfbite,                                0 },
        { &gsn_ravage,                                  0 },
        { &gsn_form_dragon,                             0 },
        { &gsn_dive,                                    0 },
        { &gsn_tailwhip,                                0 },
        { &gsn_form_demon,                              0 },
        { &gsn_form_phoenix,                            0 },
        { &gsn_swoop,                                   0 },
        { &gsn_form_hydra,                              0 },
        { &gsn_breathe,                                 0 },
        { &gsn_form_direwolf,                           0 },
        { &gsn_form_elemental_air,                      0 },
        { &gsn_form_elemental_water,                    0 },
        { &gsn_form_elemental_fire,                     0 },
        { &gsn_form_elemental_earth,                    0 },
        { &gsn_form_fly,                                0 },
        { &gsn_form_griffin,                            0 },
        { &gsn_form_wolf,                               0 },

        { &gsn_group_healing,                           0 },
        { &gsn_cure_light,                              0 },
        { &gsn_cure_serious,                            0 },
        { &gsn_cure_critical,                           0 },
        { &gsn_heal,                                    0 },

        { &gsn_group_harmful,                           0 },
        { &gsn_cause_light,                             0 },
        { &gsn_cause_serious,                           0 },
        { &gsn_cause_critical,                          0 },
        { &gsn_harm,                                    0 },

        { &gsn_group_curative,                          0 },
        { &gsn_refresh,                                 0 },
        { &gsn_cure_blindness,                          0 },
        { &gsn_cure_poison,                             0 },
        { &gsn_remove_curse,                            0 },

        { &gsn_group_disease,                           0 },
        { &gsn_blindness,                               0 },
        { &gsn_poison,                                  0 },
        { &gsn_curse,                                   0 },
        { &gsn_hex,                                     0 },

        { &gsn_group_conjuration,                       0 },
        { &gsn_create_water,                            0 },
        { &gsn_create_spring,                           0 },
        { &gsn_create_food,                             0 },
        { &gsn_brew,                                    0 },

        { &gsn_group_protection,                        0 },
        { &gsn_protection,                              0 },
        { &gsn_armor,                                   0 },
        { &gsn_sanctuary,                               0 },
        { &gsn_shield,                                  0 },
        { &gsn_stone_skin,                              0 },
        { &gsn_blink,                                   0 },
        { &gsn_dispel_magic,                            0 },
        { &gsn_mass_sanctuary,                          0 },

        { &gsn_group_advanced_heal,                     0 },
        { &gsn_mass_heal,                               0 },
        { &gsn_power_heal,                              0 },
        { &gsn_mass_power_heal,                         0 },

        { &gsn_group_divine,                            0 },
        { &gsn_bless,                                   0 },
        { &gsn_earthquake,                              0 },
        { &gsn_dispel_evil,                             0 },
        { &gsn_flamestrike,                             0 },
        { &gsn_holy_word,                               0 },
        { &gsn_unholy_word,                             0 },
        { &gsn_bless_weapon,                            0 },
        { &gsn_wrath_of_god,                            0 },
        { &gsn_prayer,                                  0 },
        { &gsn_frenzy,                                  0 },

        { &gsn_group_alteration,                        0 },
        { &gsn_continual_light,                         0 },
        { &gsn_invis,                                   0 },
        { &gsn_mass_invis,                              0 },
        { &gsn_faerie_fire,                             0 },
        { &gsn_fly,                                     0 },
        { &gsn_giant_strength,                          0 },
        { &gsn_weaken,                                  0 },
        { &gsn_faerie_fog,                              0 },
        { &gsn_knock,                                   0 },
        { &gsn_pass_door,                               0 },
        { &gsn_haste,                                   0 },
        { &gsn_entrapment,                              0 },
        { &gsn_breathe_water,                           0 },

        { &gsn_group_illusion,                          0 },
        { &gsn_ventriloquate,                           0 },

        { &gsn_group_enchant,                           0 },
        { &gsn_sleep,                                   0 },
        { &gsn_charm_person,                            0 },
        { &gsn_enchant_weapon,                          0 },
        { &gsn_enhance_armor,                           0 },

        { &gsn_group_summoning,                         0 },
        { &gsn_summon_familiar,                         0 },
        { &gsn_call_lightning,                          0 },
        { &gsn_control_weather,                         0 },
        { &gsn_teleport,                                0 },
        { &gsn_summon,                                  0 },
        { &gsn_pattern,                                 0 },

        { &gsn_group_mana,                              0 },
        { &gsn_meditate,                                0 },
        { &gsn_second_spell,                            0 },
        { &gsn_third_spell,                             0 },
        { &gsn_fourth_spell,                            0 },

        { &gsn_group_divination,                        0 },
        { &gsn_detect_evil,                             0 },
        { &gsn_detect_poison,                           0 },
        { &gsn_detect_good,                             0 },
        { &gsn_detect_invis,                            0 },
        { &gsn_detect_magic,                            0 },
        { &gsn_detect_hidden,                           0 },
        { &gsn_detect_curse,                            0 },
        { &gsn_detect_sneak,                            0 },
        { &gsn_infravision,                             0 },
        { &gsn_identify,                                0 },
        { &gsn_locate_object,                           0 },
        { &gsn_sense_traps,                             0 },
        { &gsn_know_alignment,                          0 },

        { &gsn_group_dark,                              0 },
        { &gsn_possession,                              0 },
        { &gsn_demon_flames,                            0 },
        { &gsn_steal_strength,                          0 },
        { &gsn_hand_of_lucifer,                         0 },
        { &gsn_satans_fury,                             0 },
        { &gsn_steal_soul,                              0 },
        { &gsn_summon_demon,                            0 },
        { &gsn_hells_fire,                              0 },
        { &gsn_chaos_blast,                             0 },

        { &gsn_group_death,                             0 },
        { &gsn_feeblemind,                              0 },
        { &gsn_spiritwrack,                             0 },
        { &gsn_animate_dead,                            0 },
        { &gsn_bladethirst,                             0 },
        { &gsn_dark_ritual,                             0 },

        { &gsn_group_lycanthropy,                       0 },
        { &gsn_resist_toxin,                            0 },
        { &gsn_resist_magic,                            0 },
        { &gsn_astral_sidestep,                         0 },
        { &gsn_gaias_warning,                           0 },
        { &gsn_summon_avatar,                           0 },
        { &gsn_rage,                                    0 },
        { &gsn_howl,                                    0 },

        { &gsn_group_vampyre,                           0 },
        { &gsn_feed,                                    0 },
        { &gsn_suck,                                    0 },
        { &gsn_lunge,                                   0 },
        { &gsn_double_lunge,                            0 },
        { &gsn_mist_walk,                               0 },
        { &gsn_stalk,                                   0 },
        { &gsn_transfix,                                0 },
        { &gsn_aura_of_fear,                            0 },

        { &gsn_group_evocation,                         0 },
        { &gsn_magic_missile,                           0 },
        { &gsn_chill_touch,                             0 },
        { &gsn_burning_hands,                           0 },
        { &gsn_shocking_grasp,                          0 },
        { &gsn_lightning_bolt,                          0 },
        { &gsn_colour_spray,                            0 },
        { &gsn_fireball,                                0 },
        { &gsn_acid_blast,                              0 },
        { &gsn_chain_lightning,                         0 },
        { &gsn_energy_drain,                            0 },
        { &gsn_prismatic_spray,                         0 },

        { &gsn_group_breath,                            0 },
        { &gsn_frost_breath,                            0 },
        { &gsn_acid_breath,                             0 },
        { &gsn_fire_breath,                             0 },
        { &gsn_gas_breath,                              0 },
        { &gsn_lightning_breath,                        0 },

        { &gsn_group_destruction,                       0 },
        { &gsn_firestorm,                               0 },
        { &gsn_meteor_storm,                            0 },

        { &gsn_group_majorp,                            0 },
        { &gsn_globe,                                   0 },
        { &gsn_fireshield,                              0 },
        { &gsn_dragon_shield,                           0 },

        { &gsn_group_craft,                             0 },
        { &gsn_scribe,                                  0 },
        { &gsn_recharge_item,                           0 },
        { &gsn_transport,                               0 },

        { &gsn_group_mentald,                           0 },
        { &gsn_thought_shield,                          0 },
        { &gsn_mental_barrier,                          0 },
        { &gsn_displacement,                            0 },
        { &gsn_flesh_armor,                             0 },
        { &gsn_intellect_fortress,                      0 },
        { &gsn_biofeedback,                             0 },
        { &gsn_inertial_barrier,                        0 },
        { &gsn_animate_weapon,                          0 },

        { &gsn_group_matter,                            0 },
        { &gsn_ballistic_attack,                        0 },
        { &gsn_agitation,                               0 },
        { &gsn_control_flames,                          0 },
        { &gsn_project_force,                           0 },
        { &gsn_detonate,                                0 },

        { &gsn_group_energy,                            0 },
        { &gsn_create_sound,                            0 },
        { &gsn_shadow_form,                             0 },
        { &gsn_chameleon_power,                         0 },
        { &gsn_energy_containment,                      0 },

        { &gsn_group_telepathy,                         0 },
        { &gsn_mind_thrust,                             0 },
        { &gsn_psychic_drain,                           0 },
        { &gsn_inflict_pain,                            0 },
        { &gsn_psychic_crush,                           0 },
        { &gsn_ego_whip,                                0 },
        { &gsn_combat_mind,                             0 },
        { &gsn_domination,                              0 },
        { &gsn_psionic_blast,                           0 },
        { &gsn_fear,                                    0 },
        { &gsn_deter,                                   0 },

        { &gsn_group_advtele,                           0 },
        { &gsn_inner_fire,                              0 },
        { &gsn_synaptic_blast,                          0 },
        { &gsn_ultrablast,                              0 },
        { &gsn_death_field,                             0 },
        { &gsn_disintergrate,                           0 },

        { &gsn_group_body,                              0 },
        { &gsn_levitation,                              0 },
        { &gsn_psychic_healing,                         0 },
        { &gsn_enhanced_strength,                       0 },
        { &gsn_adrenaline_control,                      0 },
        { &gsn_lend_health,                             0 },
        { &gsn_cell_adjustment,                         0 },
        { &gsn_share_strength,                          0 },
        { &gsn_recharge_mana,                           0 },
        { &gsn_vitalize,                                0 },
        { &gsn_complete_healing,                        0 },
        { &gsn_heighten,                                0 },
        { &gsn_ectoplasmic_form,                        0 },

        { &gsn_group_archery,                           0 },
        { &gsn_shoot,                                   0 },
        { &gsn_second_shot,                             0 },
        { &gsn_third_shot,                              0 },
        { &gsn_accuracy,                                0 },
        { &gsn_snap_shot,                               0 },

        { &gsn_group_music,                             0 },
        { &gsn_song_of_shadows,                         0 },
        { &gsn_song_of_revelation,                      0 },
        { &gsn_song_of_sustenance,                      0 },
        { &gsn_song_of_rejuvenation,                    0 },
        { &gsn_song_of_tranquility,                     0 },
        { &gsn_song_of_flight,                          0 },
        { &gsn_chant_of_pain,                           0 },
        { &gsn_chant_of_vigour,                         0 },
        { &gsn_chant_of_enfeeblement,                   0 },
        { &gsn_chant_of_battle,                         0 },
        { &gsn_chant_of_protection,                     0 },

        { &gsn_group_herb_lore,                         0 },
        { &gsn_gather_herbs,                            0 },
        { &gsn_classify,                                0 },

        { &gsn_group_morph,                             0 },
        { &gsn_flight,                                  0 },

        { &gsn_group_resistance,                        0 },
        { &gsn_resist_heat,                             0 },
        { &gsn_resist_cold,                             0 },
        { &gsn_resist_acid,                             0 },
        { &gsn_resist_lightning,                        0 },

        { &gsn_group_armoursmith,                       0 },
        { &gsn_smelt,                                   0 },
        { &gsn_strengthen,		                0 },
        { &gsn_imbue,			                0 },
        { &gsn_uncommon_set,	                        0 },
        { &gsn_rare_set,		                0 },
        { &gsn_epic_set,		                0 },
        { &gsn_legendary_set,	                        0 },
        { &gsn_repelling,		                0 },

        {&gsn_group_weaponsmith,	                0 },
        {&gsn_craft_weapon,		                0 },
        {&gsn_counterbalance,		                0 },
        {&gsn_weaponchain,		                0 },
        {&gsn_shieldchain,		                0 },
        {&gsn_hurl,		                        0 },
        {&gsn_serate,		                        0 },
        {&gsn_engrave,		                        0 },
        {&gsn_discharge,		                0 },

        {&gsn_group_turret_tech,	                0 },
        {&gsn_turret,			                0 },
        {&gsn_trigger,			                0 },
        {&gsn_dart,			                0 },
        {&gsn_launcher,			                0 },
        {&gsn_reflector,		                0 },
        {&gsn_shield,			                0 },
        {&gsn_arrestor,		  	                0 },
        {&gsn_driver,			                0 },
        {&gsn_emergency,		                0 },

        {&gsn_group_mech_tech,                          0 },
        {&gsn_deploy,			                0 },
        {&gsn_forager,        		                0 },
        {&gsn_spyglass,       		                0 },
        {&gsn_base,          		                0 },
        {&gsn_miner,          		                0 },

        {&gsn_group_alchemy,          		        0 },
        {&gsn_hurl,              		        0 },
        {&gsn_fire_flask,            		        0 },
        {&gsn_frost_flask,           		        0 },
        {&gsn_stun_flask,              		        0 },
        {&gsn_blind_flask,                   	        0 },
        {&gsn_lightning_flask,           		0 },
        {&gsn_acid_flask,                               0 },
        {&gsn_bmf_flask,                                0 },

        {&gsn_group_inscription, 	       	        0 },
        {&gsn_inscribe,                  	        0 },
        {&gsn_protection,                   	        0 },
        {&gsn_enhancement,                   	        0 },
        {&gsn_healing,                                  0 },
        {&gsn_ward,                                     0 },


        { &gsn_group_last,                              0 }
};


/*
 *  Skill table
 */
const struct skill_type skill_table [MAX_SKILL] =
{
        /*
         * name, pointer to gsn
         * practice type, target, minimum position,
         * pointer to spell function, minimum mana, beats
         * damage noun, wear off message
         *
         * When adding new spells add them near the end, just
         * before the base skills.  Elements are accessed by
         * index and results will be unpredictable if the current
         * order gets screwed up.
         */
        {
                "reserved", NULL,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                0, 0, 0,
                "", ""
        },

        {
                "acid blast", &gsn_acid_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_acid_blast, 20, 12,
                "{Ga{Yci{Wd bl{Yas{Gt{x", "!Acid Blast!"
        },

        {
                "inner fire", &gsn_inner_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_inner_fire, 20, 12,
                "{Rin{Yne{Wr {Yfi{Rre{x", "!Inner Fire!"
        },

        {
                 "synaptic blast", &gsn_synaptic_blast,
                 TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                 spell_synaptic_blast, 30, 12,
                 "{Bsy{Cnap{Wtic {Cbla{Bst{x", "!Synaptic Blast!"
        },

        {
                "prismatic spray", &gsn_prismatic_spray,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_prismatic_spray, 25, 12,
                "{Rpri{Ysm{Gat{Cic {Bsp{Mray{x", "!Prismatic Spray!"
        },

        {
                "holy word", &gsn_holy_word,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_holy_word, 20, 12,
                "{Wholy word{x", "!Holy Word!"
        },

        {
                "unholy word", &gsn_unholy_word,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_unholy_word, 20, 12,
                "{Munholy word{x", "!Unholy Word!"
        },

        {
                "armor", &gsn_armor,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_armor, 5, 12,
                "", "Your magical armour dissipates."
        },

        {
                "astral vortex", &gsn_astral_vortex,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_astral_vortex, 200, 24,
                "", "!Astral Vortex!"
        },

        {
                "portal", 0,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 400, 24,
                "", "!Portal!"
        },

        {
                "pattern", &gsn_pattern,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Pattern!"
        },

        {
                "bless", &gsn_bless,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_bless, 5, 12,
                "", "You feel less righteous."
        },

        {
                "blindness", &gsn_blindness,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_blindness, 5, 12,
                "spell", "You can see again."
        },

        {
                "burning hands", &gsn_burning_hands,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_burning_hands, 15, 12,
                "{Rburning{x hands", "!Burning Hands!"
        },

        {
                "call lightning", &gsn_call_lightning,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_call_lightning, 15, 12,
                "{Bl{Cig{Wh{Ct{Wnin{Cg {Wb{Col{Bt{x", "!Call Lightning!"
        },

        {
                "cause critical", &gsn_cause_critical,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_cause_critical, 20, 12,
                "spell", "!Cause Critical!"
        },

        {
                "cause light", &gsn_cause_light,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_cause_light, 15, 12,
                "spell", "!Cause Light!"
        },

        {
                "cause serious", &gsn_cause_serious,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_cause_serious, 17, 12,
                "spell", "!Cause Serious!"
        },

        {
                "change sex", &gsn_change_sex,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_change_sex, 15, 12,
                "", "Your body feels familiar again."
        },

        {
                "chain lightning", &gsn_chain_lightning,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_chain_lightning, 15, 24,
                "{Bl{Cig{Wh{Ct{Wnin{Cg {Wb{Col{Bt{x", "!Chain Lightning!"
        },

        {
                "charm person", &gsn_charm_person,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_charm_person, 5, 12,
                "attempted charm", "You feel more self-confident."
        },

        {
                "chill touch", &gsn_chill_touch,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_chill_touch, 10, 12,
                "{Bch{Ci{Wlling to{Cu{Bch{x", "You feel less cold."
        },

        {
                "colour spray", &gsn_colour_spray,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_colour_spray, 15, 12,
                "{Mco{Blo{Cur {Gs{Ypr{Ray{x", "!Colour Spray!"
        },

        {
                "continual light", &gsn_continual_light,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_continual_light, 7, 12,
                "", "!Continual Light!"
        },

        {
                "control weather", &gsn_control_weather,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_control_weather, 25, 12,
                "", "!Control Weather!"
        },

        {
                "create food", &gsn_create_food,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_create_food, 5, 12,
                "", "!Create Food!"
        },

        {
                "create spring", &gsn_create_spring,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_create_spring, 20, 12,
                "", "!Create Spring!"
        },

        {
                "create water", &gsn_create_water,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_create_water, 5, 12,
                "", "!Create Water!"
        },

        {
                "cure blindness", &gsn_cure_blindness,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_cure_blindness, 5, 12,
                "", "!Cure Blindness!"
        },

        {
                "cure critical", &gsn_cure_critical,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_cure_critical, 20, 12,
                "", "!Cure Critical!"
        },

        {
                "cure light", &gsn_cure_light,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_cure_light, 10, 12,
                "", "!Cure Light!"
        },

        {
                "cure poison", &gsn_cure_poison,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_cure_poison, 5, 12,
                "", "!Cure Poison!"
        },

        {
                "cure serious", &gsn_cure_serious,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_cure_serious, 15, 12,
                "", "!Cure Serious!"
        },

        {
                "curse", &gsn_curse,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_curse, 20, 12,
                "{Mcurse{x", "The curse wears off."
        },

        {
                "detect good", &gsn_detect_good,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_good, 5, 12,
                "", "The yellow in your vision disappears."
        },

        {
                "detect evil", &gsn_detect_evil,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_evil, 5, 12,
                "", "The red in your vision disappears."
        },

        {
                "detect hidden", &gsn_detect_hidden,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_hidden, 5, 12,
                "", "You feel less aware of your surroundings."
        },

        {
                "detect invis", &gsn_detect_invis,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_invis, 5, 12,
                "", "You no longer see invisible objects."
        },

        {
                "detect sneak", &gsn_detect_sneak,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_sneak, 5, 20,
                "", "You feel less observant."
        },

        {
                "detect magic", &gsn_detect_magic,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_magic, 5, 12,
                "", "The blue in your vision disappears."
        },

        {
                "detect poison", &gsn_detect_poison,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_detect_poison, 5, 12,
                "", "!Detect Poison!"
        },

        {
                "dispel evil", &gsn_dispel_evil,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_dispel_evil, 15, 12,
                "dispel evil", "!Dispel Evil!"
        },

        {
                "dispel magic", &gsn_dispel_magic,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_dispel_magic, 15, 16,
                "", "!Dispel Magic!"
        },

        {
                "earthquake", &gsn_earthquake,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_earthquake, 15, 12,
                "earthquake", "!Earthquake!"
        },

        {
                "enchant weapon", &gsn_enchant_weapon,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_enchant_weapon, 100, 24,
                "", "!Enchant Weapon!"
        },

        {
                "energy drain", &gsn_energy_drain,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_energy_drain, 35, 12,
                "{Ben{Ce{Yr{Wgy d{Yr{Ca{Bin{x", "!Energy Drain!"
        },

        {
                "entrapment", &gsn_entrapment,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_entrapment, 100, 12,
                "entrapment", "You break free from the stasis field."
        },

        {
                "faerie fire", &gsn_faerie_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_faerie_fire, 5, 12,
                "{mfa{Merie fi{x{mre{x","The pink aura around you fades away."
        },

        {
                "faerie fog", &gsn_faerie_fog,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_faerie_fog, 12, 12,
                "faerie fog", "!Faerie Fog!"
        },

        {
                "fireball", &gsn_fireball,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_fireball, 15, 12,
                "{Rfi{Yr{Web{Ya{Rll{x", "!Fireball!"
        },

        {
                "fireshield", &gsn_fireshield,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_fireshield, 75, 18,
                "{Rfi{Yre{Wsh{Yie{Rld{x", "The flames around your body fizzle out."
        },

        {
                "flamestrike", &gsn_flamestrike,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_flamestrike, 20, 12,
                "{Rfla{Yme{Ws{Ytr{Rike{x", "!Flamestrike!"
        },

        {
                "fly", &gsn_fly,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_fly, 10, 18,
                "", "You feel the pull of gravity slowly return."
        },

        {
                "__EMPTY__", 0, 0, 0, 0, NULL, 0, 0, "", ""
        },

        {
                "__EMPTY__", 0, 0, 0, 0, NULL, 0, 0, "", ""
        },

        {
                "gate", &gsn_gate,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_gate, 50, 12,
                "", "!Gate!"
        },

        {
                "giant strength", &gsn_giant_strength,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_giant_strength, 20, 12,
                "", "You feel weaker."
        },

        {
                "harm", &gsn_harm,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_harm, 35, 12,
                "harm spell", "!Harm!"
        },

        {
                "haste", &gsn_haste,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_haste, 300, 12,
                "", "You slow down."
        },

        {
                "heal", &gsn_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_heal, 50, 12,
                "", "!Heal!"
        },

        {
                "knock", &gsn_knock,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_knock, 50, 12,
                "", "!Knock!"
        },

        {
                "power heal", &gsn_power_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_power_heal, 75, 24,
                "", "!Power Heal!"
        },

        {
                "identify", &gsn_identify,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_identify, 12, 12,
                "", "!Identify!"
        },

        {
                "infravision", &gsn_infravision,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_infravision, 5, 18,
                "", "You no longer see in the dark."
        },

        {
                "intimidate", &gsn_intimidate,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING,
                spell_null, 0, 0,
                "", "You no longer feel invincible."
        },

        {
                "extort", &gsn_extort,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Extort!"
        },

        {
                "invis", &gsn_invis,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_invis, 5, 12,
                "", "You are no longer invisible."
        },

        {
                "know alignment", &gsn_know_alignment,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_know_alignment, 9, 12,
                "", "!Know Alignment!"
        },

        {
                "lightning bolt", &gsn_lightning_bolt,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_lightning_bolt, 15, 12,
                "{Bl{Cig{Wh{Ct{Wnin{Cg {Wb{Col{Bt{x", "!Lightning Bolt!"
        },

        {
                "locate object", &gsn_locate_object,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_locate_object, 20, 18,
                "", "!Locate Object!"
        },

        {
                "magic missile", &gsn_magic_missile,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_magic_missile, 5, 12,
                "spell", "!Magic Missile!"
        },

        {
                "mass invis", &gsn_mass_invis,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_mass_invis, 20, 24,
                "", "Your party fades into existence."
        },

        {
                "pass door", &gsn_pass_door,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_pass_door, 20, 12,
                "", "You feel solid again."
        },

        {
                "poison", &gsn_poison,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_poison, 10, 12,
                "{Mpoisoned blood{x", "You feel less sick."
        },

        {
                "paralysis", &gsn_paralysis,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_paralysis, 10, 12,
                "", "You can move again!"
        },

        {
                "project", 0,
                TYPE_INT, TAR_IGNORE, POS_RESTING,
                spell_null, 0, 12,
                "", "You return to your body."
        },

        {
                "protection", &gsn_protection,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_protection, 5, 12,
                "", "You feel less protected."
        },

        {
                "refresh", &gsn_refresh,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_refresh, 12, 18,
                "refresh", "!Refresh!"
        },

        {
                "remove curse", &gsn_remove_curse,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_remove_curse, 5, 12,
                "", "!Remove Curse!"
        },

        {
                "sanctuary", &gsn_sanctuary,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_sanctuary, 75, 12,
                "", "The white aura around your body fades."
        },

        {
                "sense traps", &gsn_sense_traps,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_sense_traps, 15, 12,
                "", "You feel less perspicacious."
        },

        {
                "shield", &gsn_shield,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_shield, 12, 18,
                "", "Your force shield shimmers then fades away."
        },

        {
                "dragon shield", &gsn_dragon_shield,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_dragon_shield, 12, 18,
                "", "You feel vulnerable once again."
        },

        {
                "shocking grasp", &gsn_shocking_grasp,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_shocking_grasp, 15, 12,
                "{Bsh{Co{Yc{Wking g{Yr{Ca{Bsp{x", "!Shocking Grasp!"
        },

        {
                "sleep", &gsn_sleep,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_sleep, 15, 12,
                "", "You feel less tired."
        },

        {
                "stone skin", &gsn_stone_skin,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_stone_skin, 12, 18,
                "", "Your skin feels soft again."
        },

        {
                "summon", &gsn_summon,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_summon, 50, 12,
                "", "!Summon!"
        },

        {
                "summon familiar", &gsn_summon_familiar,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_summon_familiar, 100, 12,
                "", "!Summon Familiar!"
        },

        {
                "teleport", &gsn_teleport,
                TYPE_INT, TAR_CHAR_SELF, POS_FIGHTING,
                spell_teleport, 35, 12,
                "", "!Teleport!"
        },

        {
                "ventriloquate", &gsn_ventriloquate,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_ventriloquate, 5, 12,
                    "", "!Ventriloquate!"
        },

        {
                "weaken", &gsn_weaken,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_weaken, 20, 12,
                "spell", "You feel stronger."
        },

        {
                "word of recall", &gsn_word_of_recall,
                TYPE_INT, TAR_CHAR_SELF, POS_RESTING,
                spell_word_of_recall, 5, 12,
                "", "!Word of Recall!"
        },

        {
                "brew", &gsn_brew,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", "!Brew!"
        },

        {
                "scribe", &gsn_scribe,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", "!Scribe!"
        },

        {
                "acid breath", &gsn_acid_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_acid_breath, 50, 12,
                "{Gblast of acid{x", "!Acid Breath!"
        },

        {
                "fire breath", &gsn_fire_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_fire_breath, 50, 12,
                "{Rbla{Yst {Wof {Yfl{Rame{x", "!Fire Breath!"
        },

        {
                "frost breath", &gsn_frost_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_frost_breath, 50, 12,
                "{Bbl{Cast {Wof {Cfr{Bost{x", "!Frost Breath!"
        },

        {
                "gas breath", &gsn_gas_breath,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_gas_breath, 50, 12,
                "{Mblast of gas{x", "!Gas Breath!"
        },

        {
                "lightning breath", &gsn_lightning_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_lightning_breath, 50, 12,
                "{Bb{Cla{Ws{Ct o{Wf l{Ci{Wghtni{Cn{Bg{x", "!Lightning Breath!"
        },

        {
                "advance", &gsn_advance,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Advance!"
        },

        {
                "at", &gsn_at,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!At!"
        },

        {
                "advanced consider", &gsn_advanced_consider,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Advanced Consider!"
        },

        {
                "assassinate", &gsn_assassinate,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 24,
                "assassination attempt", "!Assassinate!"
        },

        {
                "backstab", &gsn_backstab,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 24,
                "backstab", "!Backstab!"
        },

          {
                  "joust", &gsn_joust,
                  TYPE_STR, TAR_IGNORE, POS_STANDING,
                  spell_null, 0, 18,
                  "joust", "!Joust!"
          },

        {
                "double backstab", &gsn_double_backstab,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "second backstab", "!Double Backstab!"
        },

        {
                "circle", &gsn_circle,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "circle", "!Circle!"
        },

        {
                "second circle", &gsn_second_circle,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "second circle", "!Second Circle!"
        },

        {
                "climb", &gsn_climb,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "slip up", "!Climb!"
        },

        {
                "bash door", &gsn_bash,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 24,
                "bash", "!Bash Door!"
        },

        {
                "berserk", &gsn_berserk,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 12,
                "", "!Berserk!"
        },

        {
                "decapitate", &gsn_decapitate,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 12,
                "decapitation", "!Decapitate!"
        },

        {
                "dirt kick", &gsn_dirt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "dirt kick", "You wipe the dirt from your eyes."
        },

        {
                "disable", &gsn_disable,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "slip up", "!Disable!"
        },

        {
                "disarm", &gsn_disarm,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "", "!Disarm!"
        },

        {
                "blink", &gsn_blink,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 10, 0,
                "", "!Blink!"
        },

        {
                "dodge", &gsn_dodge,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Dodge!"
        },

        {
                "acrobatics", &gsn_acrobatics,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Acrobatics!"
        },

        {
                "dual", &gsn_dual,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Dual!"
        },

        {
                "enhanced damage", &gsn_enhanced_damage,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Enhanced Damage!"
        },

        {
                "enhanced hit", &gsn_enhanced_hit,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Enhanced Hit!"
        },

        {
                "fast healing", &gsn_fast_healing,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Fast Healing!"
        },

        {
                "find traps", &gsn_find_traps,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Find Traps!"
        },

        {
                "fourth attack", &gsn_fourth_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Fourth Attack!"
        },

        {
                "feint", &gsn_feint,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Feint!"
        },

        {
                "focus", &gsn_focus,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 12,
                "", "!Focus!"
        },

        {
                "second spell", &gsn_second_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Second Spell!"
        },

        {
                "third spell", &gsn_third_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Third Spell!"
        },

        {
                "fourth spell", &gsn_fourth_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Fourth Spell!"
        },

        {
                "animate weapon", &gsn_animate_weapon,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_animate_weapon, 20, 12,
                "", "!Animate Weapon!"
        },

        {
                "mass teleport", 0,
                TYPE_WIZ, TAR_IGNORE, POS_FIGHTING,
                spell_null, 50, 0,
                "", "!Mass Teleport!"
        },

        {
                "transport", &gsn_transport,
                TYPE_INT, TAR_OBJ_INV, POS_FIGHTING,
                spell_transport, 85, 0,
                "", "!Transport!"
        },

        {
                "grip", &gsn_grip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Grip!"
        },

        {
                "riposte", &gsn_risposte,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "riposte", "!Riposte!"
        },

        {
                "destrier", &gsn_destrier,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "destrier", "!Destrier!"
        },

        {
                "combo", &gsn_combo,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 4,
                "combo of moves", "!Combo!"
        },

        {
                "combo2", &gsn_combo2,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 4,
                "combo of moves", "!Combo2!"
        },

        {
                "combo3", &gsn_combo3,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 4,
                "combo of moves", "!Combo3!"
        },

        {
                "combo4", &gsn_combo4,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 4,
                "combo of moves", "!Combo4!"
        },

        {
                "atemi", &gsn_atemi,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Wstrike{x", "!Atemi!"
        },

        {
                "kansetsu", &gsn_kansetsu,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Warm lock{x", "!Kansetsu!"
        },

        {
                "tetsui", &gsn_tetsui,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Whammer fist{x", "!Tetsui!"
        },

        {
                "shuto", &gsn_shuto,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Wknife hand strike{x", "!Shuto!"
        },

        {
                "yokogeri", &gsn_yokogeri,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Yside kick{x", "!Yokogeri!"
        },

        {
                "mawasigeri", &gsn_mawasigeri,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 8,
                "{Yround house{x", "!Mawasigeri!"
        },

        {
                "headbutt", &gsn_headbutt,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 24,
                "headbutt", "!Headbutt!"
        },

        {
                "second headbutt", &gsn_second_headbutt,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 24,
                "second headbutt", "!Headbutt!"
        },

        {
                "hide", &gsn_hide,
                TYPE_STR, TAR_IGNORE, POS_RESTING,
                spell_null, 0, 12,
                "", "!Hide!"
        },

        {
                "hunt", &gsn_hunt,
                TYPE_STR, TAR_IGNORE, POS_RESTING,
                spell_null, 0, 12,
                "", "!Hunt!"
        },

        {
                "kick", &gsn_kick,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 8,
                "kick", "!Kick!"
        },

        {
                "punch", &gsn_punch,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 8,
                "punch", "!Punch!"
        },

        {
                "push", &gsn_push,
                TYPE_STR, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_null, 0, 8,
                "push", "!Push!"
        },

        {
                "choke", &gsn_choke,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING,
                spell_null, 0, 8,
                "choker hold", "You regain consciousness."
        },

        {
                "break wrist", &gsn_break_wrist,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING,
                spell_null, 0, 16,
                "wrist break", "!Break Wrist!"
        },

        {
                "snap_neck", &gsn_snap_neck,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 16,
                "wrench", "!Snap Neck!"
        },

        {
                "grapple", &gsn_grapple,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 8,
                "extra attack", "!Grapple!"
        },

        {
                "fly", &gsn_flight,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 8,
                "fly", "!Fly!"
        },

        {
                "flying headbutt", &gsn_flying_headbutt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 8,
                "flying headbutt", "!Flying Headbutt!"
        },

        {
                "gouge", &gsn_gouge,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING,
                spell_null, 0, 12,
                "gouge", "Your vision returns."
        },

        {
                "pugilism", &gsn_pugalism,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "pugilism", "!Pugilism!"
        },

        {
                "grab", &gsn_grab,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING,
                spell_null, 0, 8,
                "", "!Grab!"
        },

        {
                "mount", &gsn_mount,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "mount", "!Mount!"
        },

        {
                "dismount", &gsn_dismount,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "dismount", "!Dismount!"
        },

        {
                "battle aura", &gsn_battle_aura,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING,
                spell_null, 0, 8,
                "", "You feel less sure of yourself."
        },

        {
                "second punch", &gsn_second_punch,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 8,
                "second punch", "!Second Punch!"
        },

        {
                "meditate", &gsn_meditate,
                TYPE_STR, TAR_IGNORE, POS_RESTING,
                spell_null, 0, 12,
                "", "You awaken from your trance."
        },

        {
                "parry", &gsn_parry,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Parry!"
        },

        {
                "pre-empt", &gsn_pre_empt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Pre-empt!"
        },

        {
                "peek", &gsn_peek,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Peek!"
        },

        {
                "pick lock", &gsn_pick_lock,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", "!Pick!"
        },

        {
                "bladethirst", &gsn_bladethirst,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING,
                spell_null, 0, 12,
                "concoction", "!Bladethirst!"
        },

        {
                "poison weapon", &gsn_poison_weapon,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING,
                spell_null, 0, 12,
                "poisonous concoction", "!Poison Weapon!"
        },

        {
                "forge", &gsn_forge,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING,
                spell_null, 0, 12,
                "slip", "!Forge!"
        },

        {
                "sharpen", &gsn_sharpen,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING,
                spell_null, 0, 12,
                "sharp blade", "!Sharpen!"
        },

        {
                "rescue", &gsn_rescue,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 12,
                "", "!Rescue!"
        },

        {
                "second attack", &gsn_second_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Second Attack!"
        },

        {
                "shield block", &gsn_shield_block,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Shield Block!"
        },

        {
                "smash", &gsn_smash,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "extra attack", "!Smash!"
        },

        {
                "sneak", &gsn_sneak,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "tail", &gsn_tail,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "stalk", &gsn_stalk,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "steal", &gsn_steal,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 24,
                "", "!Steal!"
        },

        {
                "stun", &gsn_stun,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "attempted stun", "Your head stops ringing."
        },

        {
                "suck", &gsn_suck,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "suck", "!Suck!"
        },

        {
                "third attack", &gsn_third_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Third Attack!"
        },

        {
                "transfix", &gsn_transfix,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "terrifying gaze", "You shake free from the trance."
        },

        {
                "trap", &gsn_trap,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "slip up", "You finally break free from the trap!"
        },

        {
                "trip", &gsn_trip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 18,
                "", "!Trip!"
        },

        {
                "unarmed combat", &gsn_unarmed_combat,
                TYPE_STR, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "The kill satisfies your lust for combat."
        },

        {
                "warcry", &gsn_warcry,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_null, 0, 0,
                "", "You feel less enraged."
        },

        {
                "rage", &gsn_rage,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_null, 0, 0,
                "", "You feel less furious."
        },

        {
                "howl", &gsn_howl,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "howl", "!Howl!"
        },

        {
                "whirlwind", &gsn_whirlwind,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 18, 18,
                "whirlwind", "You feel less angry."
        },

        {
                "flight", &gsn_flight,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 18, 18,
                "flight", "You fold your wings and settle on the ground."
        },

        {
                "general purpose", NULL,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_general_purpose, 0, 12,
                "general purpose ammo", "!General Purpose Ammo!"
        },

        {
                "high explosive", NULL,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_high_explosive, 0, 12,
                "high explosive ammo", "!High Explosive Ammo!"
        },

        {
                "addfame", &gsn_addfame,
                TYPE_STR, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Addfame!"
        },

        {
                "allow", &gsn_allow,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Allow!"
        },

        {
                "bamfin", &gsn_bamfin,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Bamfin!"
        },

        {
                "bamfout", &gsn_bamfout,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Bamfout!"
        },

        {
                "ban", &gsn_ban,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Ban!"
        },

        {
                "cando", &gsn_cando,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Cando!"
        },

        {
                "deny", &gsn_deny,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Deny!"
        },

        {
                "disconnect", &gsn_disconnect,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Disconnect!"
        },

        {
                "echo", &gsn_echo,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Echo!"
        },

        {
                "force", &gsn_force,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Force!"
        },

        {
                "freeze", &gsn_freeze,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Freeze!"
        },

        {
                "goto", &gsn_goto,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Goto!"
        },

        {
                "holylight", &gsn_holylight,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Holylight!"
        },

        {
                "immtalk", &gsn_immtalk,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Immtalk!"
        },

        {
                "dirtalk", &gsn_dirtalk,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Dirtalk!"
        },

        {
                "wizinvis", &gsn_wizinvis,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                    "", "!Wizinvis!"
        },

        {
                "killsocket", &gsn_killsocket,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Killsocket!"
        },

        {
                "log", &gsn_log,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Log!"
        },

        {
                "memory", &gsn_memory,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Memory!"
        },

        {
                "mfind", &gsn_mfind,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Mfind!"
        },

        {
                "mload", &gsn_mload,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Mload!"
        },

        {
                "oclanitem", &gsn_oclanitem,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!OclanItem!"
        },

        {
                "mset", &gsn_mset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Mset!"
        },

        {
                "mstat", &gsn_mstat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                    "", "!Mstat!"
        },

        {
                "mwhere", &gsn_mwhere,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Mwhere!"
        },

        {
                "newlock", &gsn_newlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Newlock!"
        },

        {
                "noemote", &gsn_noemote,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Noemote!"
        },

        {
                "notell", &gsn_notell,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Notell!"
        },

        {
                "numlock", &gsn_numlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Numlock!"
        },

        {
                "ofind", &gsn_ofind,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Ofind!"
        },

        {
                "oload", &gsn_oload,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Oload!"
        },

        {
                "oset", &gsn_oset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Oset!"
        },

        {
                "ostat", &gsn_ostat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Ostat!"
        },

        {
                "owhere", &gsn_owhere,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Owhere!"
        },

        {
                "pardon", &gsn_pardon,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Pardon!"
        },

        {
                "peace", &gsn_peace,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Peace!"
        },

        {
                "purge", &gsn_purge,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Purge!"
        },

        {
                "reboot", &gsn_reboot,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Reboot!"
        },

        {
                "recho", &gsn_recho,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Recho!"
        },

        {
                "rename", &gsn_rename,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Rename!"
        },

        {
                "reset", &gsn_reset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Reset!"
        },

        {
                "restore", &gsn_restore,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Restore!"
        },

        {
                "return", &gsn_return,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Return!"
        },

        {
                "rset", &gsn_rset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Rset!"
        },

        {
                "rstat", &gsn_rstat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Rstat!"
        },

        {
                "shutdown", &gsn_shutdown,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Shutdown!"
        },

        {
                "silence", &gsn_silence,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Silence!"
        },

        {
                "slay", &gsn_slay,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Slay!"
        },

        {
                "slookup", &gsn_slookup,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Slookup!"
        },

        {
                "snoop", &gsn_snoop,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Snoop!"
        },

        {
                "sset", &gsn_sset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Sset!"
        },

        {
                "sstime", &gsn_sstime,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Sstime!"
        },

        {
                "switch", &gsn_switch,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Switch!"
        },

        {
                "transfer", &gsn_transfer,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Transfer!"
        },

        {
                "trust", &gsn_trust,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Trust!"
        },

        {
                "users", &gsn_users,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Users!"
        },

        {
                "wizhelp", &gsn_wizhelp,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Wizhelp!"
        },

        {
                "wizify", &gsn_wizify,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Wizify!"
        },

        {
                "wizlock", &gsn_wizlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Wizlock!"
        },

        {
                "zones", &gsn_zones,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Zones!"
        },

        {
                "adrenaline control", &gsn_adrenaline_control,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_adrenaline_control, 6, 12,
                "", "The adrenaline rush wears off."
        },

        {

                "agitation", &gsn_agitation,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_agitation, 10, 12,
                "agitation", "!Agitation!"
        },

        {
                "aura sight", &gsn_aura_sight,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_aura_sight, 9, 12,
                "", "!Aura Sight!"
        },

        {
                "awe", &gsn_awe,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
                spell_awe, 35, 12,
                "", "!Awe!"
        },

        {
                "ballistic attack", &gsn_ballistic_attack,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_ballistic_attack, 5, 12,
                "ballistic attack", "!Ballistic Attack!"
        },

        {
                "biofeedback", &gsn_biofeedback,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_biofeedback, 75, 12,
                "", "Your biofeedback is no longer effective."
        },

        {
                "cell adjustment", &gsn_cell_adjustment,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_cell_adjustment, 8, 12,
                "", "!Cell Adjustment!"
        },

        {
                "chameleon power", &gsn_chameleon_power,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", "!Chameleon Power!"
        },

        {
                "combat mind", &gsn_combat_mind,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_combat_mind, 15, 12,
                "", "Your battle sense has faded."
        },

        {
                "recharge mana", &gsn_recharge_mana,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_recharge_mana, 100, 12,
                "", "!Recharge mana!"
        },

        {
                "vitalize", &gsn_vitalize,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_vitalize, 100, 12,
                "", "!Vitalize!"
        },

        {
                "complete healing", &gsn_complete_healing,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_complete_healing, 100, 12,
                "", "!Complete Healing!"
        },

        {
                "control flames", &gsn_control_flames,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_control_flames, 15, 12,
                "tongue of flame", "!Control Flames!"
        },

        {
                "create sound", &gsn_create_sound,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_create_sound, 5, 12,
                "", "!Create Sound!"
        },

        {
                "death field", &gsn_death_field,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_death_field, 200, 18,
                "field of death", "!Death Field!"
        },

        {
                "decay", &gsn_decay,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_decay, 100, 12,
                "slip up", "!Decay!"
        },

        {
                "detonate", &gsn_detonate,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_detonate, 25, 24,
                "detonation", "!Detonate!"
        },

        {
                "disintegrate", &gsn_disintergrate,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_disintegrate, 150, 18,
                "disintegration", "!Disintegrate!"
        },

        {
                "displacement", &gsn_displacement,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_displacement, 10, 12,
                "", "You are no longer displaced."
        },

        {
                "domination", &gsn_domination,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_domination, 5, 12,
                "", "You regain control of your body."
        },

        {
                "ectoplasmic form", &gsn_ectoplasmic_form,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_ectoplasmic_form, 20, 12,
                "", "You feel solid again."
        },

        {
                "ego whip", &gsn_ego_whip,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_ego_whip, 20, 12,
                "", "You feel more confident."
        },

        {
                "energy containment", &gsn_energy_containment,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_energy_containment, 10, 12,
                "", "You no longer absorb energy."
        },

        {
                "enhance armor", &gsn_enhance_armor,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_enhance_armor, 100, 24,
                "", "!Enhance Armor!"
        },

        {
                "enhanced strength", &gsn_enhanced_strength,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_enhanced_strength, 20, 12,
                "", "Your strength is no longer enhanced."
        },

        {
                "flesh armor", &gsn_flesh_armor,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_flesh_armor, 15, 12,
                "", "Your skin returns to normal."
        },

        {
                "heighten senses", &gsn_heighten,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING,
                spell_null, 0, 0,
                "", "Your senses return to normal."
        },

        {
                "inertial barrier", &gsn_inertial_barrier,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_inertial_barrier, 40, 24,
                "", "Your inertial barrier dissipates."
        },

        {
                "inflict pain", &gsn_inflict_pain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_inflict_pain, 10, 12,
                "mindpower", "!Inflict Pain!"
        },

        {
                "intellect fortress", &gsn_intellect_fortress,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_intellect_fortress, 25, 24,
                "", "Your intellectual fortress crumbles."
        },

        {
                "lend health", &gsn_lend_health,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_lend_health, 10, 12,
                "", "!Lend Health!"
        },

        {
                "levitation", &gsn_levitation,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_levitation, 10, 18,
                "", "The sensation of gravity gently returns to your body."
        },

        {
                "mental barrier", &gsn_mental_barrier,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_mental_barrier, 8, 12,
                "", "Your mental barrier breaks down."
        },

        {
                "mind thrust", &gsn_mind_thrust,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_mind_thrust, 8, 12,
                "mind thrust", "!Mind Thrust!"
        },

        {
                "project force", &gsn_project_force,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_project_force, 18, 12,
                "projected force", "!Project Force!"
        },

        {
                "psionic blast", &gsn_psionic_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_psionic_blast, 15, 12,
                "psionic blast", "!Psionic Blast!"
        },

        {
                "psychic crush", &gsn_psychic_crush,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_psychic_crush, 15, 12,
                "psychic crush", "!Psychic Crush!"
        },

        {
                "psychic drain", &gsn_psychic_drain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_psychic_drain, 20, 12,
                "", "You no longer feel drained."
        },

        {
                "psychic healing", &gsn_psychic_healing,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_psychic_healing, 20, 12,
                "", "!Psychic Healing!"
        },

        {
                "shadow form", &gsn_shadow_form,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "", "You no longer move in the shadows."
        },

        {
                "share strength", &gsn_share_strength,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_share_strength, 8, 12,
                "", "You no longer share strength with another."
        },

        {
                "thought shield", &gsn_thought_shield,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_thought_shield, 5, 12,
                "", "You no longer feel so protected."
        },

        {
                "ultrablast", &gsn_ultrablast,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_ultrablast, 25, 12,
                "{Cu{Gl{Ytr{Wab{Yla{Gs{Ct{x", "!Ultrablast!"
        },

        {
                "wrath of god", &gsn_wrath_of_god,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_wrath_of_god, 25, 12,
                "god", "!Wrath Of God!"
        },

        {
                "feeblemind", &gsn_feeblemind,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_feeblemind, 25, 12,
                "", "You feel less dizzy."
        },

        {
                "spiritwrack", &gsn_spiritwrack,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_spiritwrack, 25, 12,
                "", "You feel more able."
        },

        {
                "wither", &gsn_wither,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_wither, 20, 12,
                "{Gwit{Yhe{Wring {Ygr{Gasp{x", "You feel less brittle."
        },

        {
                "lore", &gsn_lore,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_lore, 10, 24,
                "", "!Lore!"
        },

        {
                "hex", &gsn_hex,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_hex, 30, 12,
                "hex", "You are no longer hexed."
        },

        {
                "animate dead", &gsn_animate_dead,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING,
                spell_animate_dead, 100, 12,
                "", "!Animate Dead!"
        },

        {
                "dark ritual", &gsn_dark_ritual,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING,
                spell_dark_ritual, 20, 10,
                "", "!dark ritual!"
        },

        {
                "bark skin", &gsn_bark_skin,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_bark_skin, 10, 12,
                "", "You no longer feel like a tree."
        },

        {
                "moonray", &gsn_moonray,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_moonray, 20, 12,
                "{Wmoon beam{x", "!moonray!"
        },

        {
                "sunray", &gsn_sunray,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_sunray, 25, 12,
                "{Ysun ray{x", "!sunray!"
        },

        {
                "prayer", &gsn_prayer,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_prayer, 50, 12,
                "", "You feel your prayer cease."
        },

        {
                "frenzy", &gsn_frenzy,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_frenzy, 100, 24,
                "", "Slowly you release the rage as it threatens to overwhelm you."
        },

        {
                "mass heal", &gsn_mass_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_mass_heal, 150, 20,
                "", "!Mass Heal!"
        },

        {
                "mass power heal", &gsn_mass_power_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_mass_power_heal, 300, 32,
                "", "!Mass Power Heal!"
        },

        {
                "mass sanctuary", &gsn_mass_sanctuary,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_mass_sanctuary, 200, 36,
                "", "The white aura around your body fades."
        },

        {
                "globe of invulnerability", &gsn_globe,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_globe, 120, 12,
                "", "The globe about you implodes."
        },

        {
                "dark globe of invulnerability", &gsn_dark_globe,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_dark_globe, 120, 12,
                "", "The dark globe about you implodes."
        },

        {
                "firestorm", &gsn_firestorm,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_firestorm, 20, 6,
                "{Rfi{Yre{Ws{Yto{Rrm{X", "!Firestorm!"
        },

        {
                "meteor storm", &gsn_meteor_storm,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_meteor_storm, 25, 18,
                "{Rmeteor storm{x", "!Meteor storm!"
        },

        {
                "bless weapon", &gsn_bless_weapon,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_bless_weapon, 100, 24,
                "", "!Bless Weapon!"
        },

        {
                "recharge item", &gsn_recharge_item,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING,
                spell_recharge_item, 100, 24,
                "", "!Recharge Item!"
        },

        {
                "kiai", &gsn_kiai,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 20, 12,
                "{Bf{Co{Wrce-ba{Cl{Bl{x", "!Kiai!"
        },

        {
                "possession", &gsn_possession,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_possession, 25, 12,
                "attempted possession", "You return to your own body."
        },

        {
                "demon flames", &gsn_demon_flames,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_demon_flames, 25, 12,
                "Horde of Demons", "You feel less tormented."
        },

        {
                "steal strength", &gsn_steal_strength,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_steal_strength, 25, 12,
                "spell", "You feel less strong."
        },

        {
                "satanic fury", &gsn_satans_fury,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_satans_fury, 25, 12,
                "summoned {Rdevil{x", "!Satans_Fury!"
        },

        {
                "hand of lucifer", &gsn_hand_of_lucifer,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_hand_of_lucifer, 100, 12,
                "hand of lucifer", "You break free from the black hand that holds you."
        },

        {
                "steal soul", &gsn_steal_soul,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING,
                spell_steal_soul, 25, 12,
                "", "!Steal Soul!"
        },

        {
                "summon demon", &gsn_summon_demon,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_summon_demon, 100, 12,
                "", "!Summon Demon!"
        },

        {
                "evocation magiks", &gsn_group_evocation,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Evocation!"
        },

        {
                "dark magiks", &gsn_group_dark,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Dark!"
        },

        {
                "death magiks", &gsn_group_death,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Dark!"
        },

        {
                "lycanthropy skills", &gsn_group_lycanthropy,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Lycanthropy!"
        },

        {
                "vampyre skills", &gsn_group_vampyre,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Vampyre!"
        },

        {
                "draconian magiks", &gsn_group_breath,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Breath!"
        },

        {
                "destructive magiks", &gsn_group_destruction,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Destruction!"
        },

        {
                "major protective magiks", &gsn_group_majorp,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Major Protection!"
        },

        {
                "mana craft", &gsn_group_craft,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Mana Craft!"
        },

        {
                "divination magiks", &gsn_group_divination,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Divination!"
        },

        {
                "alteration magiks", &gsn_group_alteration,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Alteration!"
        },

        {
                "summoning magiks", &gsn_group_summoning,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Summoning!"
        },

        {
                "illusion magiks", &gsn_group_illusion,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Illusion!"
        },

        {
                "enchantment magiks", &gsn_group_enchant,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Enchantment!"
        },

        {
                "conjuration magiks", &gsn_group_conjuration,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Conjuration!"
        },

        {
                "protective magiks", &gsn_group_protection,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Protection!"
        },

        {
                "divine magiks", &gsn_group_divine,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Divine!"
        },

        {
                "harmful magiks", &gsn_group_harmful,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Harmful!"
        },

        {
                "disease magiks", &gsn_group_disease,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Disase!"
        },

        {
                "curative magiks", &gsn_group_curative,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Curative!"
        },

        {
                "healing magiks", &gsn_group_healing,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Healing!"
        },

        {
                "advanced healing magiks", &gsn_group_advanced_heal,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Advanced Healing!"
        },

        {
                "mental defense disciplines", &gsn_group_mentald,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Mental Defense!"
        },

        {
                "matter control disciplines", &gsn_group_matter,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Matter Control!"
        },

        {
                "energy control disciplines", &gsn_group_energy,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Energy Control!"
        },

        {
                "telepathy disciplines", &gsn_group_telepathy,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Telepathy!"
        },

        {
                "advanced telepathy disciplines", &gsn_group_advtele,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Advanced Telepathy!"
        },

        {
                "mana control disciplines", &gsn_group_mana,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Mana Control!"
        },

        {
                "combat knowledge", &gsn_group_combat  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Combat!"
        },

        {
                "poison arts", &gsn_group_poison  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Poison!"
        },

        {
                "stealth techniques", &gsn_group_stealth  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Stealth!"
        },

        {
                "hunting arts", &gsn_group_hunting  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Hunting!"
        },

        {
                "martial arts", &gsn_group_arts,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Arts!"
        },

        {
                "combination moves", &gsn_group_combos  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Combos!"
        },

        {
                "riding techniques", &gsn_group_riding  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Riding!"
        },

        {
                "thievery skills", &gsn_group_thievery  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Thievery!"
        },

        {
                "armed combat knowledge", &gsn_group_armed  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Armed Combat!"
        },

        {
                "body control disciplines", &gsn_group_body  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Body Control!"
        },

        {
                "morph", &gsn_morph,
                TYPE_STR, TAR_IGNORE, POS_DEAD,
                spell_null, 20, 12,
                "", "!Morph!"
        },

        {
                "inner force", &gsn_group_inner  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Inner Force!"
        },

        {
                "knowledge", &gsn_group_knowledge  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Knowledge!"
        },

        {
                "nature skills", &gsn_group_nature,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group NAture!"
        },

        {
                "unarmed combat knowledge", &gsn_group_unarmed,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Unarmed Combat!"
        },

        {
                "defense knowledge", &gsn_group_defense ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Defense!"
        },

        {
                "advanced combat knowledge", &gsn_group_advcombat ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Advanced Combat!"
        },

        {
                "metal working techniques", &gsn_group_metal ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Metal!"
        },

        {
                "morphing knowledge", &gsn_group_forms   ,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Forms!"
        },

        {
                "morphing abilities", &gsn_group_morph,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Morphing Abilities"
        },

        {
                "pugilism knowledge", &gsn_group_pugalism,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Group Pugilism!"
        },

        {
                "fear", &gsn_fear,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING,
                spell_fear, 5, 12,
                "spell", "You feel less afraid."
        },

        {
                "deter", &gsn_deter,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING,
                spell_deter, 5, 12,
                "", "Monsters seem less intimidated by you..."
        },

        {
                "astral sidestep", &gsn_astral_sidestep,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_astral_sidestep, 5, 12,
                "", "You rematerialise in the physical plane."
        },

        {
                "mist walk", &gsn_mist_walk,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 5, 12,
                "", "Your body reverts to its normal state."
        },

        {
                "gaias warning", &gsn_gaias_warning,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_gaias_warning, 5, 12,
                "", "You feel less astute."
        },

        {
                "resist toxin", &gsn_resist_toxin,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Resist Toxin!"
        },

        {
                "resist magic", &gsn_resist_magic,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_resist_magic, 5, 12,
                "", "You feel more vulnerable to magic."
        },

        {
                "summon avatar", &gsn_summon_avatar,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING,
                spell_summon_avatar, 100, 18,
                "", "!Summon Avatar!"
        },

        {
                "feed", &gsn_feed,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 18,
                "", "!Feed!"
        },

        {
                "lunge", &gsn_lunge,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 18,
                "blood-thirsty fangs", "!lunge!"
        },

        {
                "aura of fear", &gsn_aura_of_fear,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 18,
                "aura of fear", "!Aura Of Fear!"
        },

        {
                "double lunge", &gsn_double_lunge,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 18,
                "blood-thirsty fangs", "!Feed!"
        },

        {
                "chameleon form", &gsn_form_chameleon,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 10, 0,
                "", "!Form Chameleon!"
        },

        {
                "hawk form", &gsn_form_hawk,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 15, 0,
                "", "!Form Hawk!"
        },

        {
                "cat form", &gsn_form_cat,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 15, 0,
                "", "!Form Cat!"
        },

        {
                "bat form", &gsn_form_bat,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 50, 0,
                "", "!Form Bat!"
        },

        {
                "snake form", &gsn_form_snake,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 20, 0,
                "", "!Form Snake!"
        },

        {
                "scorpion form", &gsn_form_scorpion,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 25, 0,
                "", "!Form Scorpion"
        },

        {
                "spider form", &gsn_form_spider,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 25, 0,
                "", "!Form Spider!"
        },

        {
                "bear form", &gsn_form_bear,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 30, 0,
                "", "!Form Bear!"
        },

        {
                "tiger form", &gsn_form_tiger,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 35, 0,
                "", "!Form Tiger!"
        },

        {
                "dragon form", &gsn_form_dragon,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Dragon!"
        },

        {
                "demon form", &gsn_form_demon,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 75, 0,
                "", "!Form Demon!"
        },

        {
                "phoenix form", &gsn_form_phoenix,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 120, 0,
                "", "!Form Phoenix!"
        },

        {
                "hydra form", &gsn_form_hydra,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Hydra!"
        },

        {
                "wolf form", &gsn_form_wolf,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Wolf!"
        },

        {
                "fly form", &gsn_form_fly,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 200, 0,
                "", "!Form Phoenix!"
        },

        {
                "griffin form", &gsn_form_griffin,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Griffin!"
        },

        {
                "wolf aura", &gsn_form_direwolf,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 150, 0,
                "", "!Form Direwolf!"
        },

        {
                "elemental air form", &gsn_form_elemental_air,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Elemental Air!"
        },

        {
                "elemental fire form", &gsn_form_elemental_fire,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Elemental Fire!"
        },

        {
                "elemental water form", &gsn_form_elemental_water,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Elemental Water!"
        },

        {
                "elemental earth form", &gsn_form_elemental_earth,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 100, 0,
                "", "!Form Elemental Earth!"
        },

        {
                "coil", &gsn_coil,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 24,
                "coil", "!Coil!"
        },

        {
                "constrict", &gsn_constrict,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 10,
                "constriction", "!Constrict!"
        },

        {
                "strangle", &gsn_strangle,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 30,
                "strangle", "!Strangle!"
        },

        {
                "swim", &gsn_swim,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Swim!"
        },

        {
                "spy", &gsn_spy,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Spy!"
        },

        {
                "forage", &gsn_forage,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 10,
                "", "!Forage!"
        },

        {
                "bite", &gsn_bite,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 10,
                "bite", "!Bite!"
        },

        {
                "maul", &gsn_maul,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 18,
                "frenzied attack", "!Maul!"
        },

        {
                "wolfbite", &gsn_wolfbite,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 10,
                "wolfbite", "!Wolfbite!"
        },

        {
                "ravage", &gsn_ravage,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 18,
                "frenzied attack", "!Ravage!"
        },

        {
                "web", &gsn_web,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 12,
                "slip up", "You break free from the sticky webbing."
        },

        {
                "venom", &gsn_venom,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 12,
                "poisoned bite", "!Venom!"
        },

        {
                "breathe", &gsn_breathe,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 50, 12,
                "breath", "!Breathe!"
        },

        {
                "dive", &gsn_dive,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 18,
                "airborne assault", "!Dive!"
        },

        {
                "tailwhip", &gsn_tailwhip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "tailwhip", "!Tailwhip!"
        },

        {
                "dragon aura", &gsn_dragon_aura,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 24,
                "", "Your dragon aura fades."
        },

        {
                "spellcraft", &gsn_spellcraft,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!Spy!"
        },

        {
                "dowse", &gsn_dowse,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "dowse", "!Dowse!"
        },

        {
                "thrust", &gsn_thrust,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 0,
                "thrust", "!Thrust!"
        },

        {
                "shoot", &gsn_shoot,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING,
                spell_null, 0, 0,
                "shot", "!Shoot!"
        },

        {
                "second shot", &gsn_second_shot,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "second shot", "!Second shot!"
        },

        {
                "third shot", &gsn_third_shot,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "third shot", "!Third shot!"
        },

        {
                "accuracy", &gsn_accuracy,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "accuracy", "!Accuracy!"
        },

        {
                "snare", &gsn_snare,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "slip up", "You finally break free from the snare!"
        },

        {
                "classify", &gsn_classify,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "classify", "!classify!"
        },

        {
                "gather", &gsn_gather_herbs,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "gather", "!Gather!"
        },

        {
                "archery knowledge", &gsn_group_archery,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "archery knowledge", "!Archery Knowledge!"
        },

        {
                "musicianship", &gsn_group_music,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "musicianship", "!Musicianship!"
        },

        {
                "herb lore", &gsn_group_herb_lore,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "herb lore", "!Herb Lore!"
        },

        {
                "song of revelation", &gsn_song_of_revelation,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of revelation", "!Song of Revelation!"
        },

        {
                "song of rejuvenation", &gsn_song_of_rejuvenation,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of rejuvenation", "!Song of Rejuvenation!"
        },

        {
                "song of tranquility", &gsn_song_of_tranquility,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of tranquility", "!Song of Tranquility!"
        },

        {
                "song of shadows", &gsn_song_of_shadows,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of shadows", "!Song of Shadows!"
        },

        {
                "song of sustenance", &gsn_song_of_sustenance,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of sustenance", "!Song of Sustenance!"
        },

        {
                "song of flight", &gsn_song_of_flight,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "song of flight", "!Song of Flight!"
        },

        {
                "chant of protection", &gsn_chant_of_protection,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "chant of protection", "Your chant of protection ceases."
        },

        {
                "chant of battle", &gsn_chant_of_battle,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "chant of battle", "Your battle chant ends."
        },

          {
                  "chant of vigour", &gsn_chant_of_vigour,
                  TYPE_INT, TAR_IGNORE, POS_STANDING,
                  spell_null, 0, 0,
                  "chant of vigour", "Your chant of vigour ends."
          },

          {
                  "chant of enfeeblement", &gsn_chant_of_enfeeblement,
                  TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                  spell_null, 0, 0,
                  "chant of enfeeblement", "!Chant of Enfeeblement!"
          },

        {
                "chant of pain", &gsn_chant_of_pain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 0,
                "chant of pain", "!Chant of Pain!"
        },

        {
                "wizbrew", &gsn_wizbrew,
                TYPE_WIZ, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "wizbrew", "!wizbrew!"
        },

        {
                "breathe water", &gsn_breathe_water,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_breathe_water, 10, 12,
                "", "Your lungs revert to normal."
        },

        {
                "pantheoninfo", &gsn_pantheoninfo,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "pantheoninfo", "!pantheoninfo!"
        },

        {
                "pantheonrank", &gsn_pantheonrank,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "pantheonrank", "!pantheonrank!"
        },

        {
                "holy prayer of combat", &gsn_prayer_combat,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "holy prayer of combat", "Your holy prayer's power wavers and ends."
        },

        {
                "holy prayer of protection", &gsn_prayer_protection,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "holy prayer of protection", "Your holy prayer's power wavers and ends."
        },

        {
                "holy prayer of free passage", &gsn_prayer_passage,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "holy prayer of free passage", "Your holy prayer's power wavers and ends."
        },

        {
                "divine curse", &gsn_prayer_weaken,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "divine curse", "You are no longer cursed by the gods."
        },

        {
                "resist heat", &gsn_resist_heat,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_resist_heat, 15, 12,
                "resist heat", "You feel more vulnerable to heat and flame."
        },

        {
                "resist cold", &gsn_resist_cold,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_resist_cold, 15, 12,
                "resist cold", "You feel more vulnerable to cold and ice."
        },

        {
                "resist lightning", &gsn_resist_lightning,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_resist_lightning, 15, 12,
                "resist lightning", "You feel more vulnerable to electricity."
        },

        {
                "resist acid", &gsn_resist_acid,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_resist_acid, 15, 12,
                "resist acid", "You feel more vulnerable to acid."
        },

        {
                "hellfire", &gsn_hells_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_hells_fire, 20, 12,
                "{Whellfire{x", "!Hellfire!"
        },

        {
                "holy prayer of destruction", &gsn_prayer_destruction,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "{Wdeity{x", "!holy prayer of destruction"
        },

        {
                "plague", &gsn_prayer_plague,
                TYPE_INT, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "plague", "You have recovered from your illness."
        },

        {
                "resistance magiks", &gsn_group_resistance,
                TYPE_INT, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "resistance magiks", "!Group Resistance!"
        },

        {
                "fury of nature", &gsn_natures_fury,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_natures_fury, 25, 12,
                "magical assault", ""
        },

        {
                "dual parry", &gsn_dual_parry,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING,
                spell_null, 0, 0,
                "", "!Parry!"
        },

        {
                "addqp", &gsn_addqp,
                TYPE_STR, TAR_IGNORE, POS_DEAD,
                spell_null, 0, 0,
                "", "!Addqp!"
        },

        {
                "chaos blast", &gsn_chaos_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING,
                spell_chaos_blast, 5, 12,
                "{Wc{Bh{Wa{Bo{Ws {Bb{Wl{Ba{Ws{Bt{x", "!Chaos Blast!"
        },

        {
                "detect curse", &gsn_detect_curse,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING,
                spell_detect_curse, 5, 12,
                "", "You no longer feel an affinity for the accursed."
        },

        {
                "knife toss", &gsn_knife_toss,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 8,
                "knife toss", "!Knife Toss!"
        },

        {
                "soar", &gsn_soar,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 24,
                "", "!Soar!"
        },

        {
                "smoke bomb", &gsn_smoke_bomb,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 8,
                "smoke bomb", "!Smoke Bomb!"
        },

        {
                "snap shot", &gsn_snap_shot,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 24,
                "snap shot", "!Snap Shot!"
        },

        {
                "crush", &gsn_crush,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 18,
                "crush", "!Crush!"
        },

        {
                "swoop", &gsn_swoop,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
                spell_null, 0, 18,
                "swoop", "!Swoop!"
        },

        {
                "smelt", &gsn_smelt,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "smelt", "!Smelt!"

        },

        {
                "imbue", &gsn_imbue,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "imbue", "!Imbue!"

        },

        {
                "counterbalance", &gsn_counterbalance,
                TYPE_STR, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "counterbalance", "!Counterbalance!"
        },

        /*
         *  Add new spells/skills at the end of the section just above.  NOWHERE ELSE.
         */

        /*
         *  Base skills for classes :)  ALWAYS LEAVE BASE SKILLS AT END OF THE TABLE !!!
         */

        {
                "mage base", &gsn_mage_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-mage base-!"
        },

        {
                "cleric base", &gsn_cleric_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-cleric base-!"
        },

        {
                "thief base", &gsn_thief_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-thief base-!"
        },

        {
                "warrior base", &gsn_warrior_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-warrior base-!"
        },

        {
                "psionic base", &gsn_psionic_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-psionic base-!"
        },

        {
                "shifter base", &gsn_shifter_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-shifter base-!"
        },

        {
                "brawler base", &gsn_brawler_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-brawler base-!"
        },

        {
                "ranger base", &gsn_ranger_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-ranger base-!"
        },

        {
                "necro base", &gsn_necro_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-necro base-!"
        },

        {
                "warlock base", &gsn_warlock_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-warlock base-!"
        },

        {
                "templar base", &gsn_templar_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-templar base-!"
        },

        {
                "druid base", &gsn_druid_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-druid base-!"
        },

        {
                "ninja base", &gsn_ninja_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-ninja base-!"
        },

        {
                "bounty base", &gsn_bounty_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-bounty base-!"
        },

        {
                "thug base", &gsn_thug_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-thug base-!"
        },

        {
                "knight base", &gsn_knight_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-knight base-!"
        },

        {
                "satanist base", &gsn_satanist_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-satanist base-!"
        },

        {
                "witch base", &gsn_witch_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-witch base-!"
        },

        {
                "werewolf base", &gsn_werewolf_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-werewolf base-!"
        },

        {
                "vampire base", &gsn_vampire_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-vampire base-!"
        },

        {
                "monk base", &gsn_monk_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-monk base-!"
        },

        {
                "artist base", &gsn_martist_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-martial artist base-!"
        },

        {
                "barbarian base", &gsn_barbarian_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-barbarian base-!"
        },

        {
                "bard base", &gsn_bard_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-bard base-!"
        },

        {
                "smithy base", &gsn_smithy_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-smithy base-!"
        },

        {
                "engineer base", &gsn_engineer_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-engineer base-!"
        },

        {
                "alchemist base", &gsn_alchemist_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-alchemist base-!"
        },

        /*
         *  The very last skill - ESSENTIAL - after the class bases
         */

        {
                "teacher base", &gsn_teacher_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING,
                spell_null, 0, 0,
                "", "!-teacher base-!"
        }
};


/* EOF const.c */
