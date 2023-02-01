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
                "Infernlst",  "Witch",  "Inf",  "Wit",
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
        },

        {
                "Smi",  "Smithy",
                APPLY_STR,  OBJ_VNUM_SCHOOL_SWORD,
                0,  85, 18,  0,  10, 13,  FALSE,
                "Engineer",  "Runesmith",  "Eng",  "Run",
                { 2, 2, 0, -2, 0 }
        }

};

const struct dpr dprs [ MAX_DPR ] =
{
        /* description, minimum dpr*/
        { "<0><241>Trifling<0>",             10     },
        { "<0><242>Inconsequential<0>",      20},
        { "<0><243>Paltry<0>",               30},
        { "<0><244>Meager<0>",               50},
        { "<0><245>Light<0>",                75},
        { "<0><246>Moderate<0>",             100},
        { "<0><247>Heavy<0>",                150},
        { "<0><248>Serious<0>",              300},
        { "<0><249>Critical<0>",             500},
        { "<0><251>GRIEVOUS<0>",             750},
        { "<0><253>DISASTEROUS<0>",          1000},
        { "<0><255>* DEMOLISHING *<0>",      1250},
        { "<0><255>** ANNIHILATING **<0>",   1500},
        { "<0><124>-<**<160><558>CRUSHING<0><0><124>**-<0>",     2000},
        { "<0><88>-+*>>##<558>Destructive<559><0><88>##<<<<*+-<0>",          3000},
        { "<0><52>-+*###<88><558>Devastating<559><0><52>###*+-<0>",          4000},
        { "<0><52>-=*<<|<556>[Wrecking<52>]<557>|>*=-<0>",             5000},
        { "<0><52>--=<<#[|<556><460><52>Pulverizing<0><52>|]#>=--<0>",          7500},
        { "<0><52>-=+*<<(|[ <556><388><196>Decimating<0><52> ]|)>*+=-<0>",           10000},
        { "<0><52>-+<<<<[[ <556><352><196>O B L I T E R A T I N G<0><52> ]]>>+-<0>",          20000}
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
        {       "Inf",  "Infernlst",            APPLY_STR,      TRUE    },
        {       "Wit",  "Witch",                APPLY_INT,      TRUE    },
        {       "Wer",  "Werewolf",             APPLY_STR,      FALSE   },
        {       "Vam",  "Vampire",              APPLY_STR,      FALSE   },
        {       "Mon",  "Monk",                 APPLY_WIS,      TRUE    },
        {       "Mar",  "M. Artist",            APPLY_STR,      FALSE   },
        {       "Brb",  "Barbarian",            APPLY_STR,      FALSE   },
        {       "Brd",  "Bard",                 APPLY_DEX,      TRUE    },
        {       "Eng",  "Engineer",             APPLY_INT,      FALSE   },
        {       "Run",  "Runesmith",            APPLY_INT,      FALSE   }
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

const struct random_types random_list [ MAX_RANDOMS ] =
{
        { APPLY_DAMROLL, SCORE_HIT_DAM},
        { APPLY_HITROLL, SCORE_HIT_DAM},
        { APPLY_AC, SCORE_AC},
        { APPLY_STR, SCORE_STATS},
        { APPLY_DEX, SCORE_STATS},
        { APPLY_WIS, SCORE_STATS },
        { APPLY_CON, SCORE_STATS },
        { APPLY_CRIT, SCORE_STATS },
        { APPLY_MANA, SCORE_HP_MANA},
        { APPLY_HIT, SCORE_HP_MANA},
        { APPLY_RESIST_ACID, SCORE_RESISTS},
        { APPLY_RESIST_LIGHTNING, SCORE_RESISTS},
        { APPLY_RESIST_HEAT, SCORE_RESISTS},
        { APPLY_RESIST_COLD, SCORE_RESISTS},
        { APPLY_SWIFTNESS, SCORE_STATS},
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


/*
 * Blueprint structure : name, description, blueprint_ref, blueprint_ego, blueprint_damage,
 * cost (steel, titanium, adamantite, electrum, starmetal), skill_name
 */

const struct blueprint_type blueprint_list [ BLUEPRINTS_MAX ] =
{
        { "turret",                 "a turret",                        OBJ_VNUM_TURRET,        EGO_ITEM_TURRET,        { 0, 0, 0},        { 30, 1, 0, 0, 0 },    "turret" },
        { "dart",                   "a dart module",                   OBJ_VNUM_DART,          EGO_ITEM_TURRET_MODULE, { 10, 20, 0 },     { 1, 0, 0, 0, 0 },     "dart" },
        { "weaponchain",            "a weaponchain",                   -1,                     EGO_ITEM_CHAINED,       { 0, 0, 0 },       { 12, 0, 0, 0, 0 },    "weaponchain" },
        { "shieldchain",            "a shieldchain",                   -1,                     EGO_ITEM_CHAINED,       { 0, 0, 0 },       { 100, 10, 0, 0, 0 },  "shieldchain" },
        { "arrow",                  "an arrow module",                 OBJ_VNUM_ARROW,         EGO_ITEM_TURRET_MODULE, { 25, 35, 0 },     { 1, 1, 0, 0, 0 },     "arrow" },
        { "wrench of the crow",     "Enchanted Wrench of the Crow",    OBJ_VNUM_UNCOMMON1,     -1,                     { 0, 0, 0 },       { 10, 1, 0, 0, 0 },    "uncommon set" },
        { "wings of the crow",      "Splayed Wings of the Crow",       OBJ_VNUM_UNCOMMON2,     -1,                     { 0, 0, 0 },       { 10, 1, 0, 0, 0 },    "uncommon set" },
        { "huntsmith's gloves",     "Huntsmith's gloves of the flame", OBJ_VNUM_RARE1,         -1,                     { 0, 0, 0 },       { 50, 10, 0, 0, 0 },   "rare set" },
        { "huntsmith's belt",       "Huntsmith's belt of the flame",   OBJ_VNUM_RARE2,         -1,                     { 0, 0, 0 },       { 70, 20, 0, 0, 0 },   "rare set" },
        { "huntsmith's boots",      "Huntsmith's boots of the flame",  OBJ_VNUM_RARE3,         -1,                     { 0, 0, 0 },       { 100, 30, 5, 0, 0 },  "rare set" },
        { "steel broadsword",       "Fierce steel broadsword",         OBJ_VNUM_ST_SWORD1,     -1,                     { 8, 18, 0 },      { 25, 0, 0, 0, 0 },    "steel broadsword" },
        { "titanium rapier",        "Titanium rapier of torment",      OBJ_VNUM_TI_SWORD1,     -1,                     { 14, 23, 0 },     { 40, 15, 0, 0, 0 },   "titanium rapier" },
        { "bloodforged helm",       "Bloodforged battlehelm",          OBJ_VNUM_BF_SET1,       -1,                     { 0, 0, 0 },       { 40, 20, 10, 5, 0 },  "bloodforged set" },
        { "bloodforged boots",      "Bloodforged swiftboots",          OBJ_VNUM_BF_SET2,       -1,                     { 0, 0, 0 },       { 50, 20, 12, 7, 0 },  "bloodforged set" },
        { "bloodforged leggings",   "Bloodforged spiked leggings",     OBJ_VNUM_BF_SET3,       -1,                     { 0, 0, 0 },       { 70, 30, 15, 9, 0 },  "bloodforged set" },
        { "bloodforged grips",      "Bloodforged grips",               OBJ_VNUM_BF_SET4,       -1,                     { 0, 0, 0 },       { 80, 40, 21, 12, 0 }, "bloodforged set" },
        { "astral horizon",         "Astral horizon of ascendance",    OBJ_VNUM_AS_SET1,       -1,                     { 0, 0, 0 },       { 50, 20, 2, 20, 3 },  "astral set" },
        { "astral wrap",            "Astral wrap of insight",          OBJ_VNUM_AS_SET2,       -1,                     { 0, 0, 0 },       { 70, 30, 5, 20, 5 },  "astral set" },
        { "astral anomaly",         "Astral anomaly of knowing",       OBJ_VNUM_AS_SET3,       -1,                     { 0, 0, 0 },       { 40, 10, 1, 20, 6 },  "astral set" },
        { "astral plane",           "Astral plane of travel",          OBJ_VNUM_AS_SET4,       -1,                     { 0, 0, 0 },       { 50, 20, 2, 30, 8 },  "astral set" },
        { "astral transcendence",   "Astral transcendence",            OBJ_VNUM_AS_SET5,       -1,                     { 0, 0, 0 },       { 70, 30, 5, 40, 10 }, "astral set" },
        { "steel cache",            "Smithys steel cache",             OBJ_VNUM_STEEL_CACHE,   -1,                     { 0, 0, 0 },       { 6, 0, 0, 0, 0 },     "steel cache" },
        { "blade module",           "a blade module",                  OBJ_VNUM_BLADE,         EGO_ITEM_TURRET_MODULE, { 60, 120, 0 },    { 10, 1, 1, 0, 0 },    "blade module" },
        { "shuriken module",        "a shuriken module",               OBJ_VNUM_SHURIKEN,      EGO_ITEM_TURRET_MODULE, { 80, 200, 0 },    { 10, 10, 1, 1, 0 },   "shuriken module" },
        { "spear module",           "a spear module",                  OBJ_VNUM_SPEAR,         EGO_ITEM_TURRET_MODULE, { 400, 700, 0 },   { 10, 10, 1, 1, 1 },   "spear module" },
        { "arrestor module",        "an arrestor module",              OBJ_VNUM_ARRESTOR,      EGO_ITEM_TURRET_MODULE, { 0, 0, OBJ_VNUM_ARRESTOR_UNIT },  { 10, 1, 0, 0, 0 }, "arrestor module" },
        { "driver module",          "a driver module",                 OBJ_VNUM_DRIVER,        EGO_ITEM_TURRET_MODULE, { 0, 0, OBJ_VNUM_DRIVER_UNIT },    { 10, 1, 0, 0, 0 }, "driver module" },
        { "reflector module",       "a reflector module",              OBJ_VNUM_REFLECTOR,     EGO_ITEM_TURRET_MODULE, { 0, 0, OBJ_VNUM_REFLECTOR_UNIT }, { 10, 1, 0, 0, 0 }, "reflector module" },
        { "shield module",          "a shield module",                 OBJ_VNUM_SHIELD,        EGO_ITEM_TURRET_MODULE, { 0, 0, OBJ_VNUM_SHIELD_UNIT },    { 10, 1, 0, 0, 0 }, "shield module" },
        { "adamantite katana",      "Adamantite Katana of souls",      OBJ_VNUM_AD_SWORD1,     -1,                     { 30, 45, 0 },       { 25, 0, 0, 0, 0 },   "adamantite katana" },
        { "electrum sword",         "HeartForged electrum sword",      OBJ_VNUM_EL_SWORD1,     -1,                     { 50, 67, 0 },       { 25, 0, 0, 0, 0 },   "electrum sword" },
        { "starmetal dual blade",   "Starmetal Dual-Vibro Blade",      OBJ_VNUM_SM_SWORD1,     -1,                     { 69, 169, 0 },      { 25, 0, 0, 0, 0 },   "starmetal dual blade" },
        { "adamantite runic blade", "Adamantite Runic Blade",          OBJ_VNUM_AD_RB,         -1,                     { 69, 169, 0 },      { 25, 0, 0, 0, 0 },   "adamantite runic blade" },
        { "electrum runic blade",   "Electrum Runic Blade",            OBJ_VNUM_EL_RB,         -1,                     { 69, 169, 0 },      { 25, 0, 0, 0, 0 },   "electrum runic blade" },
        { "starmetal runic blade",  "Starmetal Runic Blade",           OBJ_VNUM_SM_RB,         -1,                     { 69, 169, 0 },      { 25, 0, 0, 0, 0 },   "starmetal runic blade" },
        { "pyro rune",              "Pyro Rune",                       OBJ_VNUM_PYRO_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "pyro rune" },
        { "cryo rune",              "Cryo Rune",                       OBJ_VNUM_CRYO_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "cryo rune" },
        { "bolt rune",              "Bolt Rune",                       OBJ_VNUM_BOLT_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "bolt rune" },
        { "stab rune",              "Stab Rune",                       OBJ_VNUM_STAB_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "stab rune" },
        { "rend rune",              "Rend Rune",                       OBJ_VNUM_REND_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "rend rune" },
        { "mending rune",           "Mending Rune",                    OBJ_VNUM_MEND_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "mending rune" },
        { "cure rune",              "Cure Rune",                       OBJ_VNUM_CURE_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "cure rune" },
        { "ward rune",              "Ward Rune",                       OBJ_VNUM_WARD_RUNE,     -1,                     { 0, 0, 0 },         { 0, 0, 0, 0, 0 },    "ward rune" },

};

/* set_name, set_desc, set_ego, set_bonus1, set_bonus2, set_bonus3 */
const struct set_type   set_list [MAX_SETS] =
{
        { "Smithy's Set",       "Desc for the Smithy set", { FALSE, TRUE, TRUE, FALSE},
        {0, &gsn_second_attack, &gsn_enhanced_hit, 0}, {AFF_FLYING, 0, 0, AFF_SANCTUARY},
        {APPLY_FLY, 0, 0, APPLY_SANCTUARY} },

        { "Smithy's Set",       "Desc for uncommon set", { FALSE, TRUE, TRUE, FALSE},
        {0, &gsn_second_attack, &gsn_enhanced_hit, 0}, {AFF_FLYING, 0, 0, AFF_SANCTUARY},
        {APPLY_FLY, 0, 0, APPLY_SANCTUARY} }
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
                "Abyssal Hand", "Resist Heat",
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

        {
                "Grung  ", "Grung",
                0, -2, -1, 3, 1,
                20, -10, 10,
                "Resist Toxin", "Poison",
                CHAR_SIZE_SMALL
        },

        {
                "Duergar", "Duergar",
                2, 2, -2, -2, 1,
                30, -20, -20,
                "Infravision", "Invis",
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
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* smi */
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
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } }, /* brb */
        { { 1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1 } }, /* brd */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }, /* eng */
        { { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 } }  /* alc */
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
        {{ 0,1,0,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* smi */
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
        {{ 0,1,1,0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,1,1,1,1,1 }}, /* brd */
        {{ 0,1,0,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}, /* eng */
        {{ 0,1,0,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,0,1, 0,1,1,1,0,0,1,1,1 }}  /* alc */
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
        {       &gsn_swallow,                   FORM_DRAGON             },
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
        {       &gsn_swallow                    },

        /*
         * Skills below gagged by vamps but shown by werewolves
         * Need to edit has_pre_req() in act_info.c if the number
         * of wolf-only skills is changed
         */
        {       &gsn_morph                      },
        {       &gsn_wolfbite                   },
        {       &gsn_ravage                     },
        {       &gsn_swim                       },
};

char * const channel_names [ MAX_CHANNELS ] =
{
        "AUCTION",
        "CHAT",
        "SHOUT",
        "IMMTALK",
        "SAY",
        "TELL",
        "CLAN",
        "DIRTALK",
        "SERVER",
        "ARENA",
        "NEWBIE"
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
        { "\x1b[3m",            "$F",           "italics",              17 },
        { "\x1b[7m",            "$I",           "inverse",              18 },
        { "\x1b[0m",            "$R",           "normal",               19 }
};


/*
* Below adds 8-bit colour support, see
* http://strasis.com/documentation/limelight-xe/reference/ecma-48-sgr-codes
*
* --Owl 18/7/22 Increase MAX_8BIT_COLORS if you add any more codes to this table
*/

const struct color_data_8bit color_table_8bit [] =
{
        { "\x1b[0m",              "<0>",       "Normal",                 0  },
        { "\x1b[38;5;1m",         "<1>",       "Red",                    1  },
        { "\x1b[38;5;2m",         "<2>",       "Green",                  2  },
        { "\x1b[38;5;3m",         "<3>",       "Yellow",                 3  },
        { "\x1b[38;5;4m",         "<4>",       "Blue",                   4  },
        { "\x1b[38;5;5m",         "<5>",       "Magenta",                5  },
        { "\x1b[38;5;6m",         "<6>",       "Cyan",                   6  },
        { "\x1b[38;5;7m",         "<7>",       "White",                  7  },
        { "\x1b[38;5;8m",         "<8>",       "BrightBlack",            8  },
        { "\x1b[38;5;9m",         "<9>",       "BrightRed",              9  },
        { "\x1b[38;5;10m",       "<10>",       "BrightGreen",           10  },
        { "\x1b[38;5;11m",       "<11>",       "BrightYellow",          11  },
        { "\x1b[38;5;12m",       "<12>",       "BrightBlue",            12  },
        { "\x1b[38;5;13m",       "<13>",       "BrightMagenta",         13  },
        { "\x1b[38;5;14m",       "<14>",       "BrightCyan",            14  },
        { "\x1b[38;5;15m",       "<15>",       "BrightWhite",           15  },
        { "\x1b[38;5;16m",       "<16>",       "Black",                 16  },
        { "\x1b[38;5;17m",       "<17>",       "NavyBlue",              17  },
        { "\x1b[38;5;18m",       "<18>",       "DarkBlue",              18  },
        { "\x1b[38;5;19m",       "<19>",       "Blue4",                 19  },
        { "\x1b[38;5;20m",       "<20>",       "Blue3",                 20  },
        { "\x1b[38;5;21m",       "<21>",       "Blue2",                 21  },
        { "\x1b[38;5;22m",       "<22>",       "DarkGreen",             22  },
        { "\x1b[38;5;23m",       "<23>",       "DeepSkyBlue7",          23  },
        { "\x1b[38;5;24m",       "<24>",       "DeepSkyBlue6",          24  },
        { "\x1b[38;5;25m",       "<25>",       "DeepSkyBlue5",          25  },
        { "\x1b[38;5;26m",       "<26>",       "DodgerBlue3",           26  },
        { "\x1b[38;5;27m",       "<27>",       "DodgerBlue2",           27  },
        { "\x1b[38;5;28m",       "<28>",       "Green5",                28  },
        { "\x1b[38;5;29m",       "<29>",       "SpringGreen6",          29  },
        { "\x1b[38;5;30m",       "<30>",       "Turquoise2",            30  },
        { "\x1b[38;5;31m",       "<31>",       "DeepSkyBlue4",          31  },
        { "\x1b[38;5;32m",       "<32>",       "DeepSkyBlue3",          32  },
        { "\x1b[38;5;33m",       "<33>",       "DodgerBlue1",           33  },
        { "\x1b[38;5;34m",       "<34>",       "Green4",                34  },
        { "\x1b[38;5;35m",       "<35>",       "SpringGreen5",          35  },
        { "\x1b[38;5;36m",       "<36>",       "DarkCyan",              36  },
        { "\x1b[38;5;37m",       "<37>",       "LightSeaGreen",         37  },
        { "\x1b[38;5;38m",       "<38>",       "DeepSkyBlue2",          38  },
        { "\x1b[38;5;39m",       "<39>",       "DeepSkyBlue1",          39  },
        { "\x1b[38;5;40m",       "<40>",       "Green3",                40  },
        { "\x1b[38;5;41m",       "<41>",       "SpringGreen4",          41  },
        { "\x1b[38;5;42m",       "<42>",       "SpringGreen3",          42  },
        { "\x1b[38;5;43m",       "<43>",       "Cyan4",                 43  },
        { "\x1b[38;5;44m",       "<44>",       "DarkTurquoise",         44  },
        { "\x1b[38;5;45m",       "<45>",       "Turquoise1",            45  },
        { "\x1b[38;5;46m",       "<46>",       "Green2",                46  },
        { "\x1b[38;5;47m",       "<47>",       "SpringGreen2",          47  },
        { "\x1b[38;5;48m",       "<48>",       "SpringGreen1",          48  },
        { "\x1b[38;5;49m",       "<49>",       "MediumSpringGreen",     49  },
        { "\x1b[38;5;50m",       "<50>",       "Cyan3",                 50  },
	{ "\x1b[38;5;51m",       "<51>",       "Cyan2",                 51  },
        { "\x1b[38;5;52m",       "<52>",       "DarkRed2",        	52  },
        { "\x1b[38;5;53m",       "<53>",       "DeepPink8",             53  },
        { "\x1b[38;5;54m",       "<54>",       "Purple5",        	54  },
        { "\x1b[38;5;55m",       "<55>",       "Purple4",        	55  },
        { "\x1b[38;5;56m",       "<56>",       "Purple3",        	56  },
	{ "\x1b[38;5;57m",       "<57>",       "BlueViolet",            57  },
        { "\x1b[38;5;58m",       "<58>",       "Orange4",        	58  },
        { "\x1b[38;5;59m",       "<59>",       "Grey29",        	59  },
        { "\x1b[38;5;60m",       "<60>",       "MediumPurple7",         60  },
	{ "\x1b[38;5;61m",       "<61>",       "SlateBlue3",            61  },
	{ "\x1b[38;5;62m",       "<62>",       "SlateBlue2",            62  },
        { "\x1b[38;5;63m",       "<63>",       "RoyalBlue",             63  },
        { "\x1b[38;5;64m",       "<64>",       "Chartreuse6",           64  },
        { "\x1b[38;5;65m",       "<65>",       "DarkSeaGreen9",         65  },
        { "\x1b[38;5;66m",       "<66>",       "PaleTurquoise2",        66  },
        { "\x1b[38;5;67m",       "<67>",       "SteelBlue4",            67  },
        { "\x1b[38;5;68m",       "<68>",       "SteelBlue3",            68  },
        { "\x1b[38;5;69m",       "<69>",       "CornflowerBlue",        69  },
        { "\x1b[38;5;70m",       "<70>",       "Chartreuse5",           70  },
        { "\x1b[38;5;71m",       "<71>",       "DarkSeaGreen8",         71  },
	{ "\x1b[38;5;72m",       "<72>",       "CadetBlue2",            72  },
        { "\x1b[38;5;73m",       "<73>",       "CadetBlue1",            73  },
        { "\x1b[38;5;74m",       "<74>",       "SkyBlue3",              74  },
        { "\x1b[38;5;75m",       "<75>",       "SteelBlue2",            75  },
        { "\x1b[38;5;76m",       "<76>",       "Chartreuse4",           76  },
	{ "\x1b[38;5;77m",       "<77>",       "PaleGreen4",            77  },
        { "\x1b[38;5;78m",       "<78>",       "SeaGreen4",             78  },
        { "\x1b[38;5;79m",       "<79>",       "Aquamarine3",           79  },
        { "\x1b[38;5;80m",       "<80>",       "MediumTurquoise",       80  },
        { "\x1b[38;5;81m",       "<81>",       "SteelBlue1",            81  },
        { "\x1b[38;5;82m",       "<82>",       "Chartreuse3",           82  },
        { "\x1b[38;5;83m",       "<83>",       "SeaGreen3",             83  },
        { "\x1b[38;5;84m",       "<84>",       "SeaGreen2",             84  },
        { "\x1b[38;5;85m",       "<85>",       "SeaGreen1",             85  },
        { "\x1b[38;5;86m",       "<86>",       "Aquamarine2",           86  },
	{ "\x1b[38;5;87m",       "<87>",       "DarkSlateGray2",        87  },
        { "\x1b[38;5;88m",       "<88>",       "DarkRed1",              88  },
        { "\x1b[38;5;89m",       "<89>",       "DeepPink7",             89  },
        { "\x1b[38;5;90m",       "<90>",       "DarkMagenta2",          90  },
        { "\x1b[38;5;91m",       "<91>",       "DarkMagenta1",          91  },
	{ "\x1b[38;5;92m",       "<92>",       "DarkViolet2",           92  },
        { "\x1b[38;5;93m",       "<93>",       "Purple2",               93  },
        { "\x1b[38;5;94m",       "<94>",       "Orange3",               94  },
        { "\x1b[38;5;95m",       "<95>",       "LightPink3",            95  },
        { "\x1b[38;5;96m",       "<96>",       "Plum4",                 96  },
        { "\x1b[38;5;97m",       "<97>",       "MediumPurple6",         97  },
	{ "\x1b[38;5;98m",       "<98>",       "MediumPurple5",         98  },
        { "\x1b[38;5;99m",       "<99>",       "SlateBlue1",            99  },
        { "\x1b[38;5;100m",     "<100>",       "Yellow7",              100  },
        { "\x1b[38;5;101m",     "<101>",       "Wheat2",               101  },
        { "\x1b[38;5;102m",     "<102>",       "Grey28",               102  },
        { "\x1b[38;5;103m",     "<103>",       "LightSlateGrey",       103  },
        { "\x1b[38;5;104m",     "<104>",       "MediumPurple4",        104  },
        { "\x1b[38;5;105m",     "<105>",       "LightSlateBlue",       105  },
        { "\x1b[38;5;106m",     "<106>",       "Yellow6",              106  },
        { "\x1b[38;5;107m",     "<107>",       "DarkOliveGreen6",      107  },
	{ "\x1b[38;5;108m",     "<108>",       "DarkSeaGreen7",        108  },
        { "\x1b[38;5;109m",     "<109>",       "LightSkyBlue3",        109  },
        { "\x1b[38;5;110m",     "<110>",       "LightSkyBlue2",        110  },
        { "\x1b[38;5;111m",     "<111>",       "SkyBlue2",             111  },
        { "\x1b[38;5;112m",     "<112>",       "Chartreuse2",          112  },
	{ "\x1b[38;5;113m",     "<113>",       "DarkOliveGreen5",      113  },
        { "\x1b[38;5;114m",     "<114>",       "PaleGreen3",           114  },
        { "\x1b[38;5;115m",     "<115>",       "DarkSeaGreen6",        115  },
        { "\x1b[38;5;116m",     "<116>",       "DarkSlateGray3",       116  },
        { "\x1b[38;5;117m",     "<117>",       "SkyBlue1",             117  },
        { "\x1b[38;5;118m",     "<118>",       "Chartreuse1",          118  },
        { "\x1b[38;5;119m",     "<119>",       "LightGreen2",          119  },
        { "\x1b[38;5;120m",     "<120>",       "LightGreen1",          120  },
        { "\x1b[38;5;121m",     "<121>",       "PaleGreen2",           121  },
        { "\x1b[38;5;122m",     "<122>",       "Aquamarine1",          122  },
	{ "\x1b[38;5;123m",     "<123>",       "DarkSlateGray1",       123  },
        { "\x1b[38;5;124m",     "<124>",       "Red4",                 124  },
        { "\x1b[38;5;125m",     "<125>",       "DeepPink6",            125  },
        { "\x1b[38;5;126m",     "<126>",       "MediumVioletRed",      126  },
        { "\x1b[38;5;127m",     "<127>",       "Magenta7",             127  },
	{ "\x1b[38;5;128m",     "<128>",       "DarkViolet1",          128  },
        { "\x1b[38;5;129m",     "<129>",       "Purple1",              129  },
        { "\x1b[38;5;130m",     "<130>",       "DarkOrange3",          130  },
        { "\x1b[38;5;131m",     "<131>",       "IndianRed4",           131  },
        { "\x1b[38;5;132m",     "<132>",       "HotPink5",             132  },
        { "\x1b[38;5;133m",     "<133>",       "MediumOrchid4",        133  },
	{ "\x1b[38;5;134m",     "<134>",       "MediumOrchid3",        134  },
        { "\x1b[38;5;135m",     "<135>",       "MediumPurple3",        135  },
        { "\x1b[38;5;136m",     "<136>",       "DarkGoldenrod",        136  },
        { "\x1b[38;5;137m",     "<137>",       "LightSalmon3",         137  },
        { "\x1b[38;5;138m",     "<138>",       "RosyBrown",            138  },
        { "\x1b[38;5;139m",     "<139>",       "Grey27",               139  },
        { "\x1b[38;5;140m",     "<140>",       "MediumPurple2",        140  },
        { "\x1b[38;5;141m",     "<141>",       "MediumPurple1",        141  },
        { "\x1b[38;5;142m",     "<142>",       "Gold3",                142  },
        { "\x1b[38;5;143m",     "<143>",       "DarkKhaki",            143  },
	{ "\x1b[38;5;144m",     "<144>",       "NavajoWhite2",         144  },
        { "\x1b[38;5;145m",     "<145>",       "Grey26",               145  },
        { "\x1b[38;5;146m",     "<146>",       "LightSteelBlue3",      146  },
        { "\x1b[38;5;147m",     "<147>",       "LightSteelBlue2",      147  },
        { "\x1b[38;5;148m",     "<148>",       "Yellow5",              148  },
	{ "\x1b[38;5;149m",     "<149>",       "DarkOliveGreen4",      149  },
        { "\x1b[38;5;150m",     "<150>",       "DarkSeaGreen5",        150  },
        { "\x1b[38;5;151m",     "<151>",       "DarkSeaGreen4",        151  },
        { "\x1b[38;5;152m",     "<152>",       "LightCyan2",           152  },
        { "\x1b[38;5;153m",     "<153>",       "LightSkyBlue1",        153  },
        { "\x1b[38;5;154m",     "<154>",       "GreenYellow",          154  },
        { "\x1b[38;5;155m",     "<155>",       "DarkOliveGreen3",      155  },
        { "\x1b[38;5;156m",     "<156>",       "PaleGreen1",           156  },
        { "\x1b[38;5;157m",     "<157>",       "DarkSeaGreen3",        157  },
        { "\x1b[38;5;158m",     "<158>",       "DarkSeaGreen2",        158  },
	{ "\x1b[38;5;159m",     "<159>",       "PaleTurquoise1",       159  },
        { "\x1b[38;5;160m",     "<160>",       "Red3",                 160  },
        { "\x1b[38;5;161m",     "<161>",       "DeepPink5",            161  },
        { "\x1b[38;5;162m",     "<162>",       "DeepPink4",            162  },
        { "\x1b[38;5;163m",     "<163>",       "Magenta6",             163  },
	{ "\x1b[38;5;164m",     "<164>",       "Magenta5",             164  },
        { "\x1b[38;5;165m",     "<165>",       "Magenta4",             165  },
        { "\x1b[38;5;166m",     "<166>",       "DarkOrange2",          166  },
        { "\x1b[38;5;167m",     "<167>",       "IndianRed3",           167  },
        { "\x1b[38;5;168m",     "<168>",       "HotPink4",             168  },
        { "\x1b[38;5;169m",     "<169>",       "HotPink3",             169  },
        { "\x1b[38;5;170m",     "<170>",       "Orchid3",              170  },
        { "\x1b[38;5;171m",     "<171>",       "MediumOrchid2",        171  },
        { "\x1b[38;5;172m",     "<172>",       "Orange2",              172  },
        { "\x1b[38;5;173m",     "<173>",       "LightSalmon2",         173  },
        { "\x1b[38;5;174m",     "<174>",       "LightPink2",           174  },
        { "\x1b[38;5;175m",     "<175>",       "Pink2",                175  },
        { "\x1b[38;5;176m",     "<176>",       "Plum3",                176  },
	{ "\x1b[38;5;177m",     "<177>",       "Violet",               177  },
        { "\x1b[38;5;178m",     "<178>",       "Gold2",                178  },
        { "\x1b[38;5;179m",     "<179>",       "LightGoldenrod5",      179  },
        { "\x1b[38;5;180m",     "<180>",       "Tan",                  180  },
        { "\x1b[38;5;181m",     "<181>",       "MistyRose2",           181  },
	{ "\x1b[38;5;182m",     "<182>",       "Thistle2",             182  },
        { "\x1b[38;5;183m",     "<183>",       "Plum2",                183  },
        { "\x1b[38;5;184m",     "<184>",       "Yellow4",              184  },
        { "\x1b[38;5;185m",     "<185>",       "Khaki2",               185  },
        { "\x1b[38;5;186m",     "<186>",       "LightGoldenrod4",      186  },
        { "\x1b[38;5;187m",     "<187>",       "LightYellow",          187  },
        { "\x1b[38;5;188m",     "<188>",       "Grey25",               188  },
        { "\x1b[38;5;189m",     "<189>",       "LightSteelBlue1",      189  },
        { "\x1b[38;5;190m",     "<190>",       "Yellow3",              190  },
        { "\x1b[38;5;191m",     "<191>",       "DarkOliveGreen2",      191  },
	{ "\x1b[38;5;192m",     "<192>",       "DarkOliveGreen1",      192  },
        { "\x1b[38;5;193m",     "<193>",       "DarkSeaGreen1",        193  },
        { "\x1b[38;5;194m",     "<194>",       "Honeydew",             194  },
        { "\x1b[38;5;195m",     "<195>",       "LightCyan1",           195  },
        { "\x1b[38;5;196m",     "<196>",       "Red2",                 196  },
	{ "\x1b[38;5;197m",     "<197>",       "DeepPink3",            197  },
        { "\x1b[38;5;198m",     "<198>",       "DeepPink2",            198  },
        { "\x1b[38;5;199m",     "<199>",       "DeepPink1",            199  },
        { "\x1b[38;5;200m",     "<200>",       "Magenta3",             200  },
        { "\x1b[38;5;201m",     "<201>",       "Magenta2",             201  },
        { "\x1b[38;5;202m",     "<202>",       "OrangeRed1",           202  },
	{ "\x1b[38;5;203m",     "<203>",       "IndianRed2",           203  },
        { "\x1b[38;5;204m",     "<204>",       "IndianRed1",           204  },
        { "\x1b[38;5;205m",     "<205>",       "HotPink2",             205  },
        { "\x1b[38;5;206m",     "<206>",       "HotPink1",             206  },
        { "\x1b[38;5;207m",     "<207>",       "MediumOrchid1",        207  },
        { "\x1b[38;5;208m",     "<208>",       "DarkOrange1",          208  },
        { "\x1b[38;5;209m",     "<209>",       "Salmon",               209  },
        { "\x1b[38;5;210m",     "<210>",       "LightCoral",           210  },
        { "\x1b[38;5;211m",     "<211>",       "PaleVioletRed1",       211  },
        { "\x1b[38;5;212m",     "<212>",       "Orchid2",              212  },
	{ "\x1b[38;5;213m",     "<213>",       "Orchid1",              213  },
        { "\x1b[38;5;214m",     "<214>",       "Orange1",              214  },
        { "\x1b[38;5;215m",     "<215>",       "SandyBrown",           215  },
        { "\x1b[38;5;216m",     "<216>",       "LightSalmon1",         216  },
        { "\x1b[38;5;217m",     "<217>",       "LightPink1",           217  },
	{ "\x1b[38;5;218m",     "<218>",       "Pink1",                218  },
        { "\x1b[38;5;219m",     "<219>",       "Plum1",                219  },
        { "\x1b[38;5;220m",     "<220>",       "Gold1",                220  },
        { "\x1b[38;5;221m",     "<221>",       "LightGoldenrod3",      221  },
        { "\x1b[38;5;222m",     "<222>",       "LightGoldenrod2",      222  },
        { "\x1b[38;5;223m",     "<223>",       "NavajoWhite1",         223  },
        { "\x1b[38;5;224m",     "<224>",       "MistyRose1",           224  },
        { "\x1b[38;5;225m",     "<225>",       "Thistle1",             225  },
        { "\x1b[38;5;226m",     "<226>",       "Yellow2",              226  },
        { "\x1b[38;5;227m",     "<227>",       "LightGoldenrod1",      227  },
	{ "\x1b[38;5;228m",     "<228>",       "Khaki1",               228  },
        { "\x1b[38;5;229m",     "<229>",       "Wheat1",               229  },
        { "\x1b[38;5;230m",     "<230>",       "Cornsilk",             230  },
        { "\x1b[38;5;231m",     "<231>",       "White2",               231  },
        { "\x1b[38;5;232m",     "<232>",       "Grey24",               232  },
	{ "\x1b[38;5;233m",     "<233>",       "Grey23",               233  },
        { "\x1b[38;5;234m",     "<234>",       "Grey22",               234  },
        { "\x1b[38;5;235m",     "<235>",       "Grey21",               235  },
        { "\x1b[38;5;236m",     "<236>",       "Grey20",               236  },
        { "\x1b[38;5;237m",     "<237>",       "Grey19",               237  },
        { "\x1b[38;5;238m",     "<238>",       "Grey18",               238  },
        { "\x1b[38;5;239m",     "<239>",       "Grey17",               239  },
        { "\x1b[38;5;240m",     "<240>",       "Grey16",               240  },
        { "\x1b[38;5;241m",     "<241>",       "Grey15",               241  },
        { "\x1b[38;5;242m",     "<242>",       "Grey14",               242  },
        { "\x1b[38;5;243m",     "<243>",       "Grey13",               243  },
        { "\x1b[38;5;244m",     "<244>",       "Grey12",               244  },
        { "\x1b[38;5;245m",     "<245>",       "Grey11",               245  },
	{ "\x1b[38;5;246m",     "<246>",       "Grey10",               246  },
        { "\x1b[38;5;247m",     "<247>",       "Grey9",                247  },
        { "\x1b[38;5;248m",     "<248>",       "Grey8",                248  },
        { "\x1b[38;5;249m",     "<249>",       "Grey7",                249  },
        { "\x1b[38;5;250m",     "<250>",       "Grey6",                250  },
	{ "\x1b[38;5;251m",     "<251>",       "Grey5",                251  },
        { "\x1b[38;5;252m",     "<252>",       "Grey4",                252  },
        { "\x1b[38;5;253m",     "<253>",       "Grey3",                253  },
        { "\x1b[38;5;254m",     "<254>",       "Grey2",                254  },
        { "\x1b[38;5;255m",     "<255>",       "Grey1",                255  },
        { "\x1b[48;5;1m",       "<301>",       "RedB",                 301  },
        { "\x1b[48;5;2m",       "<302>",       "GreenB",               302  },
        { "\x1b[48;5;3m",       "<303>",       "YellowB",              303  },
        { "\x1b[48;5;4m",       "<304>",       "BlueB",                304  },
        { "\x1b[48;5;5m",       "<305>",       "MagentaB",             305  },
        { "\x1b[48;5;6m",       "<306>",       "CyanB",                306  },
        { "\x1b[48;5;7m",       "<307>",       "WhiteB",               307  },
        { "\x1b[48;5;8m",       "<308>",       "BrightBlackB",         308  },
        { "\x1b[48;5;9m",       "<309>",       "BrightRedB",           309  },
        { "\x1b[48;5;10m",      "<310>",       "BrightGreenB",         310  },
        { "\x1b[48;5;11m",      "<311>",       "BrightYellowB",        311  },
        { "\x1b[48;5;12m",      "<312>",       "BrightBlueB",          312  },
        { "\x1b[48;5;13m",      "<313>",       "BrightMagentaB",       313  },
        { "\x1b[48;5;14m",      "<314>",       "BrightCyanB",          314  },
        { "\x1b[48;5;15m",      "<315>",       "BrightWhiteB",         315  },
        { "\x1b[48;5;15m",      "<316>",       "BlackB",               316  },
        { "\x1b[48;5;17m",      "<317>",       "NavyBlueB",            317  },
        { "\x1b[48;5;18m",      "<318>",       "DarkBlueB",            318  },
        { "\x1b[48;5;19m",      "<319>",       "Blue4B",               319  },
        { "\x1b[48;5;20m",      "<320>",       "Blue3B",               320  },
        { "\x1b[48;5;21m",      "<321>",       "Blue2B",               321  },
        { "\x1b[48;5;22m",      "<322>",       "DarkGreenB",           322  },
        { "\x1b[48;5;23m",      "<323>",       "DeepSkyBlue7B",        323  },
        { "\x1b[48;5;24m",      "<324>",       "DeepSkyBlue6B",        324  },
        { "\x1b[48;5;25m",      "<325>",       "DeepSkyBlue5B",        325  },
        { "\x1b[48;5;26m",      "<326>",       "DodgerBlue3B",         326  },
        { "\x1b[48;5;27m",      "<327>",       "DodgerBlue2B",         327  },
        { "\x1b[48;5;28m",      "<328>",       "Green5B",              328  },
        { "\x1b[48;5;29m",      "<329>",       "SpringGreen6B",        329  },
        { "\x1b[48;5;30m",      "<330>",       "Turquoise2B",          330  },
        { "\x1b[48;5;31m",      "<331>",       "DeepSkyBlue4B",        331  },
        { "\x1b[48;5;32m",      "<332>",       "DeepSkyBlue3B",        332  },
        { "\x1b[48;5;33m",      "<333>",       "DodgerBlue1B",         333  },
        { "\x1b[48;5;34m",      "<334>",       "Green4B",              334  },
        { "\x1b[48;5;35m",      "<335>",       "SpringGreen5B",        335  },
        { "\x1b[48;5;36m",      "<336>",       "DarkCyanB",            336  },
        { "\x1b[48;5;37m",      "<337>",       "LightSeaGreenB",       337  },
        { "\x1b[48;5;38m",      "<338>",       "DeepSkyBlue2B",        338  },
        { "\x1b[48;5;39m",      "<339>",       "DeepSkyBlue1B",        339  },
        { "\x1b[48;5;40m",      "<340>",       "Green3B",              340  },
        { "\x1b[48;5;41m",      "<341>",       "SpringGreen4B",        341  },
        { "\x1b[48;5;42m",      "<342>",       "SpringGreen3B",        342  },
        { "\x1b[48;5;43m",      "<343>",       "Cyan4B",               343  },
        { "\x1b[48;5;44m",      "<344>",       "DarkTurquoiseB",       344  },
        { "\x1b[48;5;45m",      "<345>",       "Turquoise1B",          345  },
        { "\x1b[48;5;46m",      "<346>",       "Green2B",              346  },
        { "\x1b[48;5;47m",      "<347>",       "SpringGreen2B",        347  },
        { "\x1b[48;5;48m",      "<348>",       "SpringGreen1B",        348  },
        { "\x1b[48;5;49m",      "<349>",       "MediumSpringGreenB",   349  },
        { "\x1b[48;5;50m",      "<350>",       "Cyan3B",               350  },
	{ "\x1b[48;5;51m",      "<351>",       "Cyan2B",               351  },
        { "\x1b[48;5;52m",      "<352>",       "DarkRed2B",            352  },
        { "\x1b[48;5;53m",      "<353>",       "DeepPink8B",           353  },
        { "\x1b[48;5;54m",      "<354>",       "Purple5B",             354  },
        { "\x1b[48;5;55m",      "<355>",       "Purple4B",             355  },
        { "\x1b[48;5;56m",      "<356>",       "Purple3B",             356  },
	{ "\x1b[48;5;57m",      "<357>",       "BlueVioletB",          357  },
        { "\x1b[48;5;58m",      "<358>",       "Orange4B",             358  },
        { "\x1b[48;5;59m",      "<359>",       "Grey29B",              359  },
        { "\x1b[48;5;60m",      "<360>",       "MediumPurple7B",       360  },
	{ "\x1b[48;5;61m",      "<361>",       "SlateBlue3B",          361  },
	{ "\x1b[48;5;62m",      "<362>",       "SlateBlue2B",          362  },
        { "\x1b[48;5;63m",      "<363>",       "RoyalBlueB",           363  },
        { "\x1b[48;5;64m",      "<364>",       "Chartreuse6B",         364  },
        { "\x1b[48;5;65m",      "<365>",       "DarkSeaGreen9B",       365  },
        { "\x1b[48;5;66m",      "<366>",       "PaleTurquoise2B",      366  },
        { "\x1b[48;5;67m",      "<367>",       "SteelBlue4B",          367  },
        { "\x1b[48;5;68m",      "<368>",       "SteelBlue3B",          368  },
        { "\x1b[48;5;69m",      "<369>",       "CornflowerBlueB",      369  },
        { "\x1b[48;5;70m",      "<370>",       "Chartreuse5B",         370  },
        { "\x1b[48;5;71m",      "<371>",       "DarkSeaGreen8B",       371  },
	{ "\x1b[48;5;72m",      "<372>",       "CadetBlue2B",          372  },
        { "\x1b[48;5;73m",      "<373>",       "CadetBlue1B",          373  },
        { "\x1b[48;5;74m",      "<374>",       "SkyBlue3B",            374  },
        { "\x1b[48;5;75m",      "<375>",       "SteelBlue2B",          375  },
        { "\x1b[48;5;76m",      "<376>",       "Chartreuse4B",         376  },
	{ "\x1b[48;5;77m",      "<377>",       "PaleGreen4B",          377  },
        { "\x1b[48;5;78m",      "<378>",       "SeaGreen4B",           378  },
        { "\x1b[48;5;79m",      "<379>",       "Aquamarine3B",         379  },
        { "\x1b[48;5;80m",      "<380>",       "MediumTurquoiseB",     380  },
        { "\x1b[48;5;81m",      "<381>",       "SteelBlue1B",          381  },
        { "\x1b[48;5;82m",      "<382>",       "Chartreuse3B",         382  },
        { "\x1b[48;5;83m",      "<383>",       "SeaGreen3B",           383  },
        { "\x1b[48;5;84m",      "<384>",       "SeaGreen2B",           384  },
        { "\x1b[48;5;85m",      "<385>",       "SeaGreen1B",           385  },
        { "\x1b[48;5;86m",      "<386>",       "Aquamarine2B",         386  },
	{ "\x1b[48;5;87m",      "<387>",       "DarkSlateGray2B",      387  },
        { "\x1b[48;5;88m",      "<388>",       "DarkRed1B",            388  },
        { "\x1b[48;5;89m",      "<389>",       "DeepPink7B",           389  },
        { "\x1b[48;5;90m",      "<390>",       "DarkMagenta2B",        390  },
        { "\x1b[48;5;91m",      "<391>",       "DarkMagenta1B",        391  },
	{ "\x1b[48;5;92m",      "<392>",       "DarkViolet2B",         392  },
        { "\x1b[48;5;93m",      "<393>",       "Purple2B",             393  },
        { "\x1b[48;5;94m",      "<394>",       "Orange3B",             394  },
        { "\x1b[48;5;95m",      "<395>",       "LightPink3B",          395  },
        { "\x1b[48;5;96m",      "<396>",       "Plum4B",               396  },
        { "\x1b[48;5;97m",      "<397>",       "MediumPurple6B",       397  },
	{ "\x1b[48;5;98m",      "<398>",       "MediumPurple5B",       398  },
        { "\x1b[48;5;99m",      "<399>",       "SlateBlue1B",          399  },
        { "\x1b[48;5;100m",     "<400>",       "Yellow7B",             400  },
        { "\x1b[48;5;101m",     "<401>",       "Wheat2B",              401  },
        { "\x1b[48;5;102m",     "<402>",       "Grey28B",              402  },
        { "\x1b[48;5;103m",     "<403>",       "LightSlateGreyB",      403  },
        { "\x1b[48;5;104m",     "<404>",       "MediumPurple4B",       404  },
        { "\x1b[48;5;105m",     "<405>",       "LightSlateBlueB",      405  },
        { "\x1b[48;5;106m",     "<406>",       "Yellow6B",             406  },
        { "\x1b[48;5;107m",     "<407>",       "DarkOliveGreen6B",     407  },
	{ "\x1b[48;5;108m",     "<408>",       "DarkSeaGreen7B",       408  },
        { "\x1b[48;5;109m",     "<409>",       "LightSkyBlue3B",       409  },
        { "\x1b[48;5;110m",     "<410>",       "LightSkyBlue2B",       410  },
        { "\x1b[48;5;111m",     "<411>",       "SkyBlue2B",            411  },
        { "\x1b[48;5;112m",     "<412>",       "Chartreuse2B",         412  },
	{ "\x1b[48;5;113m",     "<413>",       "DarkOliveGreen5B",     413  },
        { "\x1b[48;5;114m",     "<414>",       "PaleGreen3B",          414  },
        { "\x1b[48;5;115m",     "<415>",       "DarkSeaGreen6B",       415  },
        { "\x1b[48;5;116m",     "<416>",       "DarkSlateGray3B",      416  },
        { "\x1b[48;5;117m",     "<417>",       "SkyBlue1B",            417  },
        { "\x1b[48;5;118m",     "<418>",       "Chartreuse1B",         418  },
        { "\x1b[48;5;119m",     "<419>",       "LightGreen2B",         419  },
        { "\x1b[48;5;120m",     "<420>",       "LightGreen1B",         420  },
        { "\x1b[48;5;121m",     "<421>",       "PaleGreen2B",          421  },
        { "\x1b[48;5;122m",     "<422>",       "Aquamarine1B",         422  },
	{ "\x1b[48;5;123m",     "<423>",       "DarkSlateGray1B",      423  },
        { "\x1b[48;5;124m",     "<424>",       "Red4B",                424  },
        { "\x1b[48;5;125m",     "<425>",       "DeepPink6B",           425  },
        { "\x1b[48;5;126m",     "<426>",       "MediumVioletRedB",     426  },
        { "\x1b[48;5;127m",     "<427>",       "Magenta7B",            427  },
	{ "\x1b[48;5;128m",     "<428>",       "DarkViolet1B",         428  },
        { "\x1b[48;5;129m",     "<429>",       "Purple1B",             429  },
        { "\x1b[48;5;130m",     "<430>",       "DarkOrange3B",         430  },
        { "\x1b[48;5;131m",     "<431>",       "IndianRed4B",          431  },
        { "\x1b[48;5;132m",     "<432>",       "HotPink5B",            432  },
        { "\x1b[48;5;133m",     "<433>",       "MediumOrchid4B",       433  },
	{ "\x1b[48;5;134m",     "<434>",       "MediumOrchid3B",       434  },
        { "\x1b[48;5;135m",     "<435>",       "MediumPurple3B",       435  },
        { "\x1b[48;5;136m",     "<436>",       "DarkGoldenrodB",       436  },
        { "\x1b[48;5;137m",     "<437>",       "LightSalmon3B",        437  },
        { "\x1b[48;5;138m",     "<438>",       "RosyBrownB",           438  },
        { "\x1b[48;5;139m",     "<439>",       "Grey27B",              439  },
        { "\x1b[48;5;140m",     "<440>",       "MediumPurple2B",       440  },
        { "\x1b[48;5;141m",     "<441>",       "MediumPurple1B",       441  },
        { "\x1b[48;5;142m",     "<442>",       "Gold3B",               442  },
        { "\x1b[48;5;143m",     "<443>",       "DarkKhakiB",           443  },
	{ "\x1b[48;5;144m",     "<444>",       "NavajoWhite2B",        444  },
        { "\x1b[48;5;145m",     "<445>",       "Grey26B",              445  },
        { "\x1b[48;5;146m",     "<446>",       "LightSteelBlue3B",     446  },
        { "\x1b[48;5;147m",     "<447>",       "LightSteelBlue2B",     447  },
        { "\x1b[48;5;148m",     "<448>",       "Yellow5B",             448  },
	{ "\x1b[48;5;149m",     "<449>",       "DarkOliveGreen4B",     449  },
        { "\x1b[48;5;150m",     "<450>",       "DarkSeaGreen5B",       450  },
        { "\x1b[48;5;151m",     "<451>",       "DarkSeaGreen4B",       451  },
        { "\x1b[48;5;152m",     "<452>",       "LightCyan2B",          452  },
        { "\x1b[48;5;153m",     "<453>",       "LightSkyBlue1B",       453  },
        { "\x1b[48;5;154m",     "<454>",       "GreenYellowB",         454  },
        { "\x1b[48;5;155m",     "<455>",       "DarkOliveGreen3B",     455  },
        { "\x1b[48;5;156m",     "<456>",       "PaleGreen1B",          456  },
        { "\x1b[48;5;157m",     "<457>",       "DarkSeaGreen3B",       457  },
        { "\x1b[48;5;158m",     "<458>",       "DarkSeaGreen2B",       458  },
	{ "\x1b[48;5;159m",     "<459>",       "PaleTurquoise1B",      459  },
        { "\x1b[48;5;160m",     "<460>",       "Red3B",                460  },
        { "\x1b[48;5;161m",     "<461>",       "DeepPink5B",           461  },
        { "\x1b[48;5;162m",     "<462>",       "DeepPink4B",           462  },
        { "\x1b[48;5;163m",     "<463>",       "Magenta6B",            463  },
	{ "\x1b[48;5;164m",     "<464>",       "Magenta5B",            464  },
        { "\x1b[48;5;165m",     "<465>",       "Magenta4B",            465  },
        { "\x1b[48;5;166m",     "<466>",       "DarkOrange2B",         466  },
        { "\x1b[48;5;167m",     "<467>",       "IndianRed3B",          467  },
        { "\x1b[48;5;168m",     "<468>",       "HotPink4B",            468  },
        { "\x1b[48;5;169m",     "<469>",       "HotPink3B",            469  },
        { "\x1b[48;5;170m",     "<470>",       "Orchid3B",             470  },
        { "\x1b[48;5;171m",     "<471>",       "MediumOrchid2B",       471  },
        { "\x1b[48;5;172m",     "<472>",       "Orange2B",             472  },
        { "\x1b[48;5;173m",     "<473>",       "LightSalmon2B",        473  },
        { "\x1b[48;5;174m",     "<474>",       "LightPink2B",          474  },
        { "\x1b[48;5;175m",     "<475>",       "Pink2B",               475  },
        { "\x1b[48;5;176m",     "<476>",       "Plum3B",               476  },
	{ "\x1b[48;5;177m",     "<477>",       "VioletB",              477  },
        { "\x1b[48;5;178m",     "<478>",       "Gold2B",               478  },
        { "\x1b[48;5;179m",     "<479>",       "LightGoldenrod5B",     479  },
        { "\x1b[48;5;180m",     "<480>",       "TanB",                 480  },
        { "\x1b[48;5;181m",     "<481>",       "MistyRose2B",          481  },
	{ "\x1b[48;5;182m",     "<482>",       "Thistle2B",            482  },
        { "\x1b[48;5;183m",     "<483>",       "Plum2B",               483  },
        { "\x1b[48;5;184m",     "<484>",       "Yellow4B",             484  },
        { "\x1b[48;5;185m",     "<485>",       "Khaki2B",              485  },
        { "\x1b[48;5;186m",     "<486>",       "LightGoldenrod4B",     486  },
        { "\x1b[48;5;187m",     "<487>",       "LightYellowB",         487  },
        { "\x1b[48;5;188m",     "<488>",       "Grey25B",              488  },
        { "\x1b[48;5;189m",     "<489>",       "LightSteelBlue1B",     489  },
        { "\x1b[48;5;190m",     "<490>",       "Yellow3B",             490  },
        { "\x1b[48;5;191m",     "<491>",       "DarkOliveGreen2B",     491  },
	{ "\x1b[48;5;192m",     "<492>",       "DarkOliveGreen1B",     492  },
        { "\x1b[48;5;193m",     "<493>",       "DarkSeaGreen1B",       493  },
        { "\x1b[48;5;194m",     "<494>",       "HoneydewB",            494  },
        { "\x1b[48;5;195m",     "<495>",       "LightCyan1B",          495  },
        { "\x1b[48;5;196m",     "<496>",       "Red2B",                496  },
	{ "\x1b[48;5;197m",     "<497>",       "DeepPink3B",           497  },
        { "\x1b[48;5;198m",     "<498>",       "DeepPink2B",           498  },
        { "\x1b[48;5;199m",     "<499>",       "DeepPink1B",           499  },
        { "\x1b[48;5;200m",     "<500>",       "Magenta3B",            500  },
        { "\x1b[48;5;201m",     "<501>",       "Magenta2B",            501  },
        { "\x1b[48;5;202m",     "<502>",       "OrangeRed1B",          502  },
	{ "\x1b[48;5;203m",     "<503>",       "IndianRed2B",          503  },
        { "\x1b[48;5;204m",     "<504>",       "IndianRed1B",          504  },
        { "\x1b[48;5;205m",     "<505>",       "HotPink2B",            505  },
        { "\x1b[48;5;206m",     "<506>",       "HotPink1B",            506  },
        { "\x1b[48;5;207m",     "<507>",       "MediumOrchid1B",       507  },
        { "\x1b[48;5;208m",     "<508>",       "DarkOrange1B",         508  },
        { "\x1b[48;5;209m",     "<509>",       "SalmonB",              509  },
        { "\x1b[48;5;210m",     "<510>",       "LightCoralB",          510  },
        { "\x1b[48;5;211m",     "<511>",       "PaleVioletRed1B",      511  },
        { "\x1b[48;5;212m",     "<512>",       "Orchid2B",             512  },
	{ "\x1b[48;5;213m",     "<513>",       "Orchid1B",             513  },
        { "\x1b[48;5;214m",     "<514>",       "Orange1B",             514  },
        { "\x1b[48;5;215m",     "<515>",       "SandyBrownB",          515  },
        { "\x1b[48;5;216m",     "<516>",       "LightSalmon1B",        516  },
        { "\x1b[48;5;217m",     "<517>",       "LightPink1B",          517  },
	{ "\x1b[48;5;218m",     "<518>",       "Pink1B",               518  },
        { "\x1b[48;5;219m",     "<519>",       "Plum1B",               519  },
        { "\x1b[48;5;220m",     "<520>",       "Gold1B",               520  },
        { "\x1b[48;5;221m",     "<521>",       "LightGoldenrod3B",     521  },
        { "\x1b[48;5;222m",     "<522>",       "LightGoldenrod2B",     522  },
        { "\x1b[48;5;223m",     "<523>",       "NavajoWhite1B",        523  },
        { "\x1b[48;5;224m",     "<524>",       "MistyRose1B",          524  },
        { "\x1b[48;5;225m",     "<525>",       "Thistle1B",            525  },
        { "\x1b[48;5;226m",     "<526>",       "Yellow2B",             526  },
        { "\x1b[48;5;227m",     "<527>",       "LightGoldenrod1B",     527  },
	{ "\x1b[48;5;228m",     "<528>",       "Khaki1B",              528  },
        { "\x1b[48;5;229m",     "<529>",       "Wheat1B",              529  },
        { "\x1b[48;5;230m",     "<530>",       "CornsilkB",            530  },
        { "\x1b[48;5;231m",     "<531>",       "White2B",              531  },
        { "\x1b[48;5;232m",     "<532>",       "Grey24B",              532  },
	{ "\x1b[48;5;233m",     "<533>",       "Grey23B",              533  },
        { "\x1b[48;5;234m",     "<534>",       "Grey22B",              534  },
        { "\x1b[48;5;235m",     "<535>",       "Grey21B",              535  },
        { "\x1b[48;5;236m",     "<536>",       "Grey20B",              536  },
        { "\x1b[48;5;237m",     "<537>",       "Grey19B",              537  },
        { "\x1b[48;5;238m",     "<538>",       "Grey18B",              538  },
        { "\x1b[48;5;239m",     "<539>",       "Grey17B",              539  },
        { "\x1b[48;5;240m",     "<540>",       "Grey16B",              540  },
        { "\x1b[48;5;241m",     "<541>",       "Grey15B",              541  },
        { "\x1b[48;5;242m",     "<542>",       "Grey14B",              542  },
        { "\x1b[48;5;243m",     "<543>",       "Grey13B",              543  },
        { "\x1b[48;5;244m",     "<544>",       "Grey12B",              544  },
        { "\x1b[48;5;245m",     "<545>",       "Grey11B",              545  },
	{ "\x1b[48;5;246m",     "<546>",       "Grey10B",              546  },
        { "\x1b[48;5;247m",     "<547>",       "Grey9B",               547  },
        { "\x1b[48;5;248m",     "<548>",       "Grey8B",               548  },
        { "\x1b[48;5;249m",     "<549>",       "Grey7B",               549  },
        { "\x1b[48;5;250m",     "<550>",       "Grey6B",               550  },
	{ "\x1b[48;5;251m",     "<551>",       "Grey5B",               551  },
        { "\x1b[48;5;252m",     "<552>",       "Grey4B",               552  },
        { "\x1b[48;5;253m",     "<553>",       "Grey3B",               553  },
        { "\x1b[48;5;254m",     "<554>",       "Grey2B",               554  },
        { "\x1b[48;5;255m",     "<555>",       "Grey1B",               555  },
        { "\x1b[1m",            "<556>",       "BoldOn",               556  },
        { "\x1b[2m",            "<557>",       "BoldOff",              557  },
        { "\x1b[3m",            "<558>",       "ItalicsOn",            558  },
        { "\x1b[23m",           "<559>",       "ItalicsOff",           559  },
        { "\x1b[4m",            "<560>",       "UnderlineOn",          560  },
        { "\x1b[24m",           "<561>",       "UnderlineOff",         561  },
        { "\x1b[9m",            "<562>",       "StrikeThroughOn",      562  },
        { "\x1b[29m",           "<563>",       "StrikeThroughOff",     563  },
        { "\x1b[39m",           "<564>",       "DefaultForeground",    564  },
        { "\x1b[49m",           "<565>",       "DefaultBackground",    565  }
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
        /* defensive adjustment, initiative bonus,  */
        {   60, 0, -5 },  /* 0 */
        {   50, 0, -4  },
        {   50, 0, -4  },
        {   40, 0, -3  },
        {   30, 0, -3  },
        {   20, 0, -2  },  /* 5 */
        {   10, 0, -2  },
        {    0, 0, -1  },
        {    0, 0, -1  },
        {    0, 0, -1  },
        {    0, 0, -1  },  /* 10 */
        {    0, 0, 0   },
        {    0, 0, 0   },
        {    0, 0, 0   },
        {    0, 0, 0   },
        { - 10, 0, 0   },  /* 15 */
        { - 15, 0, 0   },
        { - 20, 0, 0   },
        { - 30, 0, 1   },
        { - 40, 0, 1   },
        { - 50, 0, 2   },  /* 20 */
        { - 65, 0, 2   },
        { - 75, 0, 2   },
        { - 90, 0, 3   },
        { -105, 0, 3   },
        { -120, 0, 3   },  /* 25 */
        { -130, 1, 4   },
        { -140, 1, 4   },
        { -150, 2, 4   },
        { -160, 2, 5   },
        { -175, 3, 5   },
        { -200, 4, 6   }
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

const struct int_app_type int_app [ MAX_STAT ] =
{
        /*  tocrit */

        { -5 },  /* 0 */
        { -4 },
        { -4 },
        { -3 },
        { -3 },
        { -2 },  /* 5 */
        { -2 },
        { -1 },
        { -1 },
        { -1 },
        { -1 },  /* 10 */
        {  0 },
        {  0 },
        {  0 },
        {  0 },
        {  0 },  /* 15 */
        {  0 },
        {  0 },
        {  1 },
        {  1 },
        {  2 },  /* 20 */
        {  2 },
        {  2 },
        {  3 },
        {  3 },
        {  3 },  /* 25 */
        {  4 },
        {  4 },
        {  4 },
        {  5 },
        {  5 },  /* 30 */
        {  6 }
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
#include "pre_reqs/pre_req-infernal.c"
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
#include "pre_reqs/pre_req-runesmith.c"
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
        &gsn_group_death,
        &gsn_group_dark,
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
        &gsn_group_weaponsmith,
        &gsn_group_turret_tech,
        &gsn_group_mech_tech,
        &gsn_group_runic_arts,
        &gsn_group_inscription,
        &gsn_group_adv_smith,
        &gsn_group_weaponlore,
        &gsn_group_rune_casting,
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
        { &gsn_swim,                                   0 },
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
        { &gsn_slow,                                    0 },

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
        { &gsn_swallow,                                 0 },
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


        { &gsn_group_death,                             0 },
        { &gsn_feeblemind,                              0 },
        { &gsn_spiritwrack,                             0 },
        { &gsn_animate_dead,                            0 },
        { &gsn_bladethirst,                             0 },
        { &gsn_dark_ritual,                             0 },

        { &gsn_group_dark,                              0 },
        { &gsn_possession,                              0 },
        { &gsn_demon_flames,                            0 },
        { &gsn_steal_strength,                          0 },
        { &gsn_abyssal_hand,                            0 },
        { &gsn_infernal_fury,                           0 },
        { &gsn_steal_soul,                              0 },
        { &gsn_summon_demon,                            0 },
        { &gsn_hells_fire,                              0 },
        { &gsn_chaos_blast,                             0 },

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
        { &gsn_steam_breath,                            0 },

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
        { &gsn_stabilise,                               0 },

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
        { &gsn_awe,                                     0 },
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
        { &gsn_uncommon_set,	                        0 },
        { &gsn_rare_set,		                0 },
        { &gsn_steel_cache,                             0 },

        { &gsn_group_weaponsmith,	                0 },
        { &gsn_counterbalance,		                0 },
        { &gsn_weaponchain,		                0 },
        { &gsn_hurl,		                        0 },
        { &gsn_imbue,			                0 },
        { &gsn_shieldchain,		                0 },
        { &gsn_steel_broadsword,                        0 },
        { &gsn_titanium_rapier,                         0 },
        { &gsn_adamantite_katana,                       0 },
        { &gsn_electrum_sword,                          0 },
        { &gsn_starmetal_dual_blade,	                0 },
        { &gsn_reforge,                                 0 },

        { &gsn_group_turret_tech,	                0 },
        { &gsn_trigger,			                0 },
        { &gsn_dart,			                0 },
        { &gsn_arrow,                                   0 },
        { &gsn_turret,                                  0 },
        { &gsn_launcher,	                        0 },
        { &gsn_reflector_module,		        0 },
        { &gsn_shield_module,		                0 },
        { &gsn_arrestor_module,		                0 },
        { &gsn_driver_module,			        0 },
        { &gsn_emergency,		                0 },

        { &gsn_group_mech_tech,                         0 },
        { &gsn_deploy,			                0 },
        { &gsn_forager,        		                0 },
        { &gsn_spyglass,       		                0 },
        { &gsn_base,          		                0 },
        { &gsn_miner,          		                0 },

        { &gsn_group_runic_arts,          		        0 },
        { &gsn_pyro_rune,            		        0 },
        { &gsn_cryo_rune,           		        0 },
        { &gsn_bolt_rune,                              0 },
        { &gsn_stab_rune,                   	        0 },
        { &gsn_rend_rune,           		        0 },
        { &gsn_mend_rune,                              0 },
        { &gsn_cure_rune,                               0 },
        { &gsn_ward_rune,                               0 },

        { &gsn_group_inscription, 	       	        0 },
        { &gsn_inscribe,                  	        0 },
        { &gsn_protection,                   	        0 },
        { &gsn_adamantite_runic_blade,                  0 },
        { &gsn_electrum_runic_blade,              	0 },
        { &gsn_starmetal_runic_blade,                   0 },


        { &gsn_group_adv_smith,                         0 },
        { &gsn_bloodforged_set,                         0 },
        { &gsn_astral_set,                              0 },
        { &gsn_repelling,                               0 },

        { &gsn_group_weaponlore,                        0 },
        { &gsn_innate_knowledge,                        0 },
        { &gsn_serrate,                                 0 },
        { &gsn_engrave,                                 0 },
        { &gsn_discharge,                               0 },
        { &gsn_empower,                                 0 },

        { &gsn_group_rune_casting,                      0 },
        { &gsn_fiery_strike,                            0 },
        { &gsn_lightning_lunge,                         0 },

        {&gsn_group_last,                              0 }
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
         * index and results will be unpredictable (BUT BAD)
         * if the current order gets screwed up.
         */
        {
                "reserved", NULL,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                0, 0, 0,
                "", ""
        },

        {
                "acid blast", &gsn_acid_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 128|16384,
                spell_acid_blast, 20, 12,
                "<28>a<106>c<178>i<229>d <15>b<229>l<178>a<106>s<28>t<0>", "!Acid Blast!"
        },

        {
                "inner fire", &gsn_inner_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_inner_fire, 20, 12,
                "<124>i<196>n<11>n<229>e<15>r <229>f<11>i<196>r<124>e<0>", "!Inner Fire!"

        },

        {
                 "synaptic blast", &gsn_synaptic_blast,
                 TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                 spell_synaptic_blast, 30, 12,
                 "<12>s<27>y<14>n<123>a<159>p<195>t<231>i<195>c <159>b<123>l<14>a<27>s<12>t<0>", "!Synaptic Blast!"
        },

        {
                "prismatic spray", &gsn_prismatic_spray,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_prismatic_spray, 25, 12,
                "<196>p<160>r<124>i<226>s<154>m<10>a<118>t<51>i<87>c <21>s<20>p<201>r<165>a<129>y<0>", "!Prismatic Spray!"
        },

        {
                "holy word", &gsn_holy_word,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_holy_word, 20, 12,
                "<556><196>+*(<15>holy word<196>)*+<557>", "!Holy Word!"
        },

        {
                "unholy word", &gsn_unholy_word,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 512|262144,
                spell_unholy_word, 20, 12,
                "<556><55>un<56>ho<57>ly <56>wo<55>rd<0>", "!Unholy Word!"
        },

        {
                "armor", &gsn_armor,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_armor, 5, 12,
                "", "Your magical armour dissipates."
        },

        {
                "astral vortex", &gsn_astral_vortex,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_astral_vortex, 200, 24,
                "", "!Astral Vortex!"
        },

        {
                "portal", 0,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 400, 24,
                "", "!Portal!"
        },

        {
                "pattern", &gsn_pattern,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!Pattern!"
        },

        {
                "bless", &gsn_bless,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_bless, 5, 12,
                "", "You feel less righteous."
        },

        {
                "blindness", &gsn_blindness,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_blindness, 5, 12,
                "spell", "You can see again."
        },


        {
                "burning hands", &gsn_burning_hands,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_burning_hands, 15, 12,
                "<196>bu<202>r<208>n<202>i<196>ng<0> hand", "!Burning Hands!"
        },

        {
                "call lightning", &gsn_call_lightning,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 4|16384,
                spell_call_lightning, 15, 12,
                "<27>l<123>i<51>g<15>h<51>t<15>nin<51>g <15>b<51>o<123>l<27>t<0>", "!Call Lightning!"
        },

        {
                "cause critical", &gsn_cause_critical,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cause_critical, 20, 12,
                "spell", "!Cause Critical!"
        },

        {
                "cause light", &gsn_cause_light,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cause_light, 15, 12,
                "spell", "!Cause Light!"
        },

        {
                "cause serious", &gsn_cause_serious,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cause_serious, 17, 12,
                "spell", "!Cause Serious!"
        },

        {
                "change sex", &gsn_change_sex,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_change_sex, 15, 12,
                "", "Your body feels familiar again."
        },

        {
                "chain lightning", &gsn_chain_lightning,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 4|16384,
                spell_chain_lightning, 15, 24,
                "<27>l<123>i<51>g<15>h<51>t<15>nin<51>g <15>b<51>o<123>l<27>t<0>", "!Chain Lightning!"
        },

        {
                "charm person", &gsn_charm_person,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 2048|16384,
                spell_charm_person, 5, 12,
                "attempted charm", "You feel more self-confident."
        },

        {
                "chill touch", &gsn_chill_touch,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 2|16384,
                spell_chill_touch, 10, 12,
                "<27>c<32>h<14>i<87>l<123>l<159>i<15>n<159>g <123>t<87>o<14>u<32>c<27>h<0>", "You feel less cold."
        },

        {
                "colour spray", &gsn_colour_spray,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_colour_spray, 15, 12,
                "<198>c<201>o<12>l<33>o<14>u<49>r <34>s<106>p<11>r<202>a<196>y<0>", "!Colour Spray!"
        },

        {
                "continual light", &gsn_continual_light,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 131072,
                spell_continual_light, 7, 12,
                "", "!Continual Light!"
        },

        {
                "control weather", &gsn_control_weather,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_control_weather, 25, 12,
                "", "!Control Weather!"
        },

        {
                "create food", &gsn_create_food,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_create_food, 5, 12,
                "", "!Create Food!"
        },

        {
                "create spring", &gsn_create_spring,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_create_spring, 20, 12,
                "", "!Create Spring!"
        },

        {
                "create water", &gsn_create_water,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_create_water, 5, 12,
                "", "!Create Water!"
        },

        {
                "cure blindness", &gsn_cure_blindness,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cure_blindness, 5, 12,
                "", "!Cure Blindness!"
        },

        {
                "cure critical", &gsn_cure_critical,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cure_critical, 20, 12,
                "", "!Cure Critical!"
        },

        {
                "cure light", &gsn_cure_light,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cure_light, 10, 12,
                "", "!Cure Light!"
        },

        {
                "cure poison", &gsn_cure_poison,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384|131072,
                spell_cure_poison, 5, 12,
                "", "!Cure Poison!"
        },

        {
                "cure serious", &gsn_cure_serious,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_cure_serious, 15, 12,
                "", "!Cure Serious!"
        },

        {
                "curse", &gsn_curse,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 524288,
                spell_curse, 20, 12,
                "<199>c<200>u<201>r<200>s<199>e<0>", "The curse wears off."
        },

        {
                "detect good", &gsn_detect_good,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384|131072,
                spell_detect_good, 5, 12,
                "", "<228>The yellow in your vision disappears.<0>"
        },

        {
                "detect evil", &gsn_detect_evil,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384|131072,
                spell_detect_evil, 5, 12,
                "", "<124>The red in your vision disappears.<0>"
        },

        {
                "detect hidden", &gsn_detect_hidden,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_detect_hidden, 5, 12,
                "", "You feel less aware of your surroundings."
        },

        {
                "detect invis", &gsn_detect_invis,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_detect_invis, 5, 12,
                "", "You no longer see invisible objects."
        },

        {
                "detect sneak", &gsn_detect_sneak,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_detect_sneak, 5, 20,
                "", "You feel less observant."
        },

        {
                "detect magic", &gsn_detect_magic,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_detect_magic, 5, 12,
                "", "<27>The blue in your vision disappears.<0>"
        },

        {
                "detect poison", &gsn_detect_poison,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_detect_poison, 5, 12,
                "", "!Detect Poison!"
        },

        {
                "dispel evil", &gsn_dispel_evil,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_dispel_evil, 15, 12,
                "dispel evil", "!Dispel Evil!"
        },

        {
                "dispel magic", &gsn_dispel_magic,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_dispel_magic, 15, 16,
                "", "!Dispel Magic!"
        },

        {
                "earthquake", &gsn_earthquake,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_earthquake, 15, 12,
                "earthquake", "!Earthquake!"
        },

        {
                "enchant weapon", &gsn_enchant_weapon,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_enchant_weapon, 100, 24,
                "", "!Enchant Weapon!"
        },

        {
                "energy drain", &gsn_energy_drain,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 8|16384,
                spell_energy_drain, 35, 12,
                "<20>e<21>n<14>e<11>r<229>g<15>y <229>d<11>r<14>a<21>i<20>n<0>", "!Energy Drain!"
        },

        {
                "entrapment", &gsn_entrapment,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 4096|16384,
                spell_entrapment, 100, 12,
                "entrapment", "You break free from the stasis field."
        },

        {
                "faerie fire", &gsn_faerie_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384,
                spell_faerie_fire, 5, 12,
                "<54>f<90>a<126>e<162>r<198>ie <162>f<126>i<90>r<54>e<0>","The pink aura around you fades away."
        },

        {
                "faerie fog", &gsn_faerie_fog,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_faerie_fog, 12, 12,
                "faerie fog", "!Faerie Fog!"
        },

        {
                "fireball", &gsn_fireball,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_fireball, 15, 12,
                "<556><196>fi<11>r<15>e<11>b<15>a<196>ll<0>", "!Fireball!"
        },

        {
                "fireshield", &gsn_fireshield,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 1|16384,
                spell_fireshield, 75, 18,
                "<196>fi<202>r<11>e<15>sh<11>i<202>e<196>ld<0>", "<88>The flames around your body fizzle out.<0>"
        },

        {
                "flamestrike", &gsn_flamestrike,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_flamestrike, 20, 12,
                "<196>fl<202>a<11>me<15>s<11>tr<202>i<196>ke<0>", "!Flamestrike!"
        },

        {
                "fly", &gsn_fly,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_fly, 10, 18,
                "", "You feel the pull of gravity slowly return."
        },

        {
                "__EMPTY__", 0, 0, 0, 0, 0, NULL, 0, 0, "", ""
        },

        {
                "__EMPTY__", 0, 0, 0, 0, 0, NULL, 0, 0, "", ""
        },

        {
                "gate", &gsn_gate,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_gate, 50, 12,
                "", "!Gate!"
        },

        {
                "giant strength", &gsn_giant_strength,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_giant_strength, 20, 12,
                "", "You feel weaker."
        },

        {
                "harm", &gsn_harm,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|131072,
                spell_harm, 35, 12,
                "harm spell", "!Harm!"
        },

        {
                "haste", &gsn_haste,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_haste, 300, 12,
                "", "You slow down."
        },

        {
                "heal", &gsn_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_heal, 50, 12,
                "", "!Heal!"
        },

        {
                "knock", &gsn_knock,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_knock, 50, 12,
                "", "!Knock!"
        },

        {
                "power heal", &gsn_power_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|131072,
                spell_power_heal, 75, 24,
                "", "!Power Heal!"
        },

        {
                "identify", &gsn_identify,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_identify, 12, 12,
                "", "!Identify!"
        },

        {
                "infravision", &gsn_infravision,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_infravision, 5, 18,
                "", "You no longer see in the dark."
        },

        {
                "intimidate", &gsn_intimidate,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING, 0,
                spell_null, 0, 0,
                "", "You no longer feel invincible."
        },

        {
                "extort", &gsn_extort,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Extort!"
        },

        {
                "invis", &gsn_invis,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_invis, 5, 12,
                "", "You are no longer invisible."
        },

        {
                "know alignment", &gsn_know_alignment,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_know_alignment, 9, 12,
                "", "!Know Alignment!"
        },

        {
                "lightning bolt", &gsn_lightning_bolt,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 4|16384,
                spell_lightning_bolt, 15, 12,
                "<27>l<123>i<51>g<15>h<51>t<15>nin<51>g <15>b<51>o<123>l<27>t<0>", "!Lightning Bolt!"
        },

        {
                "locate object", &gsn_locate_object,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_locate_object, 20, 18,
                "", "!Locate Object!"
        },

        {
                "magic missile", &gsn_magic_missile,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 8|16384,
                spell_magic_missile, 5, 12,
                "spell", "!Magic Missile!"
        },

        {
                "mass invis", &gsn_mass_invis,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_mass_invis, 20, 24,
                "", "Your party fades into existence."
        },

        {
                "pass door", &gsn_pass_door,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_pass_door, 20, 12,
                "", "You feel solid again."
        },

        {
                "poison", &gsn_poison,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 256|16384,
                spell_poison, 10, 12,
                "<201>p<200>o<199>i<198>s<197>o<198>n<199>e<200>d <201>b<200>l<199>o<198>o<197>d<0>", "<122>You feel less sick.<0>"
        },

        {
                "paralysis", &gsn_paralysis,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 4096|16384,
                spell_paralysis, 10, 12,
                "", "You can move again!"
        },

        {
                "project", 0,
                TYPE_INT, TAR_IGNORE, POS_RESTING, 16384,
                spell_null, 0, 12,
                "", "You return to your body."
        },

        {
                "protection", &gsn_protection,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_protection, 5, 12,
                "", "You feel less protected."
        },

        {
                "refresh", &gsn_refresh,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_refresh, 12, 18,
                "refresh", "!Refresh!"
        },

        {
                "remove curse", &gsn_remove_curse,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_remove_curse, 5, 12,
                "", "!Remove Curse!"
        },

        {
                "sanctuary", &gsn_sanctuary,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_sanctuary, 75, 12,
                "", "<250>The white aura around your body fades.<0>"
        },

        {
                "sense traps", &gsn_sense_traps,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_sense_traps, 15, 12,
                "", "You feel less perspicacious."
        },

        {
                "shield", &gsn_shield,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_shield, 12, 18,
                "", "Your force shield shimmers then fades away."
        },

        {
                "dragon shield", &gsn_dragon_shield,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_dragon_shield, 12, 18,
                "", "You feel vulnerable once again."
        },

        {
                "shocking grasp", &gsn_shocking_grasp,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_shocking_grasp, 15, 12,
                "<21>s<27>h<14>o<226>c<228>k<15>ing <228>g<226>r<14>a<27>s<21>p<0>", "!Shocking Grasp!"
        },

        {
                "sleep", &gsn_sleep,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 1024|16384,
                spell_sleep, 15, 12,
                "spell", "You feel less tired."
        },

        {
                "stone skin", &gsn_stone_skin,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_stone_skin, 12, 18,
                "", "Your skin feels soft again."
        },

        {
                "summon", &gsn_summon,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_summon, 50, 12,
                "", "!Summon!"
        },

        {
                "summon familiar", &gsn_summon_familiar,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_summon_familiar, 100, 12,
                "", "!Summon Familiar!"
        },

        {
                "teleport", &gsn_teleport,
                TYPE_INT, TAR_CHAR_SELF, POS_FIGHTING, 16384,
                spell_teleport, 35, 12,
                "", "!Teleport!"
        },

        {
                "ventriloquate", &gsn_ventriloquate,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_ventriloquate, 5, 12,
                "", "!Ventriloquate!"
        },

        {
                "weaken", &gsn_weaken,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384,
                spell_weaken, 20, 12,
                "spell", "You feel stronger."
        },

        {
                "word of recall", &gsn_word_of_recall,
                TYPE_INT, TAR_CHAR_SELF, POS_RESTING, 16384,
                spell_word_of_recall, 5, 12,
                "", "!Word of Recall!"
        },

        {
                "brew", &gsn_brew,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 12,
                "", "!Brew!"
        },

        {
                "scribe", &gsn_scribe,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 12,
                "", "!Scribe!"
        },

        {
                "acid breath", &gsn_acid_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 128|16384,
                spell_acid_breath, 50, 12,
                "<112>bl<76>as<40>t of <76>ac<112>id<0>", "!Acid Breath!"
        },

        {
                "fire breath", &gsn_fire_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_fire_breath, 50, 12,
                "<202>b<196>l<160>a<124>s<88>t<52> of <88>f<124>l<160>a<196>m<202>e<0>", "!Fire Breath!"
        },

        {
                "frost breath", &gsn_frost_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 2|16384,
                spell_frost_breath, 50, 12,
                "<14>bl<87>a<159>s<195>t <15>o<195>f<159> f<87>ro<14>st<0>", "!Frost Breath!"
        },

        {
                "gas breath", &gsn_gas_breath,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 256|16384,
                spell_gas_breath, 50, 12,
                "<201>b<165>l<129>a<93>s<57>t o<93>f <129>g<165>a<201>s<0>", "!Gas Breath!"
        },

        {
                "lightning breath", &gsn_lightning_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 4|16384,
                spell_lightning_breath, 50, 12,
                "<21>b<27>l<51>a<159>s<51>t o<159>f l<51>i<195>g<15>htn<51>i<27>n<21>g<0>", "!Lightning Breath!"
        },

        {
                "advance", &gsn_advance,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Advance!"
        },

        {
                "at", &gsn_at,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!At!"
        },

        {
                "advanced consider", &gsn_advanced_consider,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Advanced Consider!"
        },

        {
                "assassinate", &gsn_assassinate,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 24,
                "assassination attempt", "!Assassinate!"
        },

        {
                "backstab", &gsn_backstab,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 24,
                "backstab", "!Backstab!"
        },

        {
                "joust", &gsn_joust,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 18,
                "joust", "!Joust!"
        },

        {
                "double backstab", &gsn_double_backstab,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "second backstab", "!Double Backstab!"
        },

        {
                "circle", &gsn_circle,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "circle", "!Circle!"
        },

        {
                "second circle", &gsn_second_circle,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "second circle", "!Second Circle!"
        },

        {
                "climb", &gsn_climb,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "slip up", "!Climb!"
        },

        {
                "bash door", &gsn_bash,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 24,
                "bash", "!Bash Door!"
        },

        {
                "berserk", &gsn_berserk,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 12,
                "", "!Berserk!"
        },

        {
                "decapitate", &gsn_decapitate,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 12,
                "decapitation", "!Decapitate!"
        },

        {
                "dirt kick", &gsn_dirt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "dirt kick", "You wipe the dirt from your eyes."
        },

        {
                "disable", &gsn_disable,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "slip up", "!Disable!"
        },

        {
                "disarm", &gsn_disarm,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "", "!Disarm!"
        },

        {
                "blink", &gsn_blink,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 10, 0,
                "", "!Blink!"
        },

        {
                "dodge", &gsn_dodge,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Dodge!"
        },

        {
                "acrobatics", &gsn_acrobatics,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Acrobatics!"
        },

        {
                "dual", &gsn_dual,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Dual!"
        },

        {
                "enhanced damage", &gsn_enhanced_damage,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Enhanced Damage!"
        },

        {
                "enhanced hit", &gsn_enhanced_hit,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Enhanced Hit!"
        },

        {
                "fast healing", &gsn_fast_healing,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Fast Healing!"
        },

        {
                "find traps", &gsn_find_traps,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Find Traps!"
        },

        {
                "fourth attack", &gsn_fourth_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Fourth Attack!"
        },

        {
                "feint", &gsn_feint,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Feint!"
        },

        {
                "focus", &gsn_focus,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 0, 12,
                "", "!Focus!"
        },

        {
                "second spell", &gsn_second_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 0, 0,
                "", "!Second Spell!"
        },

        {
                "third spell", &gsn_third_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 0, 0,
                "", "!Third Spell!"
        },

        {
                "fourth spell", &gsn_fourth_spell,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 0, 0,
                "", "!Fourth Spell!"
        },

        {
                "animate weapon", &gsn_animate_weapon,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384,
                spell_animate_weapon, 20, 12,
                "", "!Animate Weapon!"
        },

        {
                "mass teleport", 0,
                TYPE_WIZ, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 50, 0,
                "", "!Mass Teleport!"
        },

        {
                "transport", &gsn_transport,
                TYPE_INT, TAR_OBJ_INV, POS_FIGHTING, 16384,
                spell_transport, 85, 0,
                "", "!Transport!"
        },

        {
                "grip", &gsn_grip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Grip!"
        },

        {
                "riposte", &gsn_risposte,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "riposte", "!Riposte!"
        },

        {
                "destrier", &gsn_destrier,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "destrier", "!Destrier!"
        },

        {
                "combo", &gsn_combo,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 4,
                "combo of moves", "!Combo!"
        },

        {
                "combo2", &gsn_combo2,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 4,
                "combo of moves", "!Combo2!"
        },

        {
                "combo3", &gsn_combo3,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 4,
                "combo of moves", "!Combo3!"
        },

        {
                "combo4", &gsn_combo4,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 4,
                "combo of moves", "!Combo4!"
        },

        {
                "atemi", &gsn_atemi,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<15><556>strike<0>", "!Atemi!"
        },

        {
                "kansetsu", &gsn_kansetsu,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<15><556>arm lock<0>", "!Kansetsu!"
        },

        {
                "tetsui", &gsn_tetsui,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<15><556>hammer fist<0>", "!Tetsui!"
        },

        {
                "shuto", &gsn_shuto,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<15><556>knife hand strike<0>", "!Shuto!"
        },

        {
                "yokogeri", &gsn_yokogeri,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<11><556>side kick<0>", "!Yokogeri!"
        },

        {
                "mawasigeri", &gsn_mawasigeri,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 8,
                "<11><556>round house<0>", "!Mawasigeri!"
        },

        {
                "headbutt", &gsn_headbutt,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 24,
                "headbutt", "!Headbutt!"
        },

        {
                "second headbutt", &gsn_second_headbutt,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 24,
                "second headbutt", "!Headbutt!"
        },

        {
                "hide", &gsn_hide,
                TYPE_STR, TAR_IGNORE, POS_RESTING, 16384,
                spell_null, 0, 12,
                "", "!Hide!"
        },

        {
                "hunt", &gsn_hunt,
                TYPE_STR, TAR_IGNORE, POS_RESTING, 16384,
                spell_null, 0, 12,
                "", "!Hunt!"
        },

        {
                "kick", &gsn_kick,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 8,
                "kick", "!Kick!"

        },

        {
                "punch", &gsn_punch,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 8,
                "punch", "!Punch!"
        },

        {
                "push", &gsn_push,
                TYPE_STR, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 8,
                "push", "!Push!"
        },

        {
                "choke", &gsn_choke,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 8,
                "choker hold", "You regain consciousness."
        },

        {
                "break wrist", &gsn_break_wrist,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 16,
                "wrist break", "!Break Wrist!"
        },

        {
                "snap_neck", &gsn_snap_neck,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 16,
                "wrench", "!Snap Neck!"
        },

        {
                "grapple", &gsn_grapple,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 8,
                "extra attack", "{WYou recover from the grapple, and get back to your feet.{x"
        },

        {
                "fly", &gsn_flight,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 8,
                "fly", "!Fly!"
        },

        {
                "flying headbutt", &gsn_flying_headbutt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 8,
                "flying headbutt", "!Flying Headbutt!"
        },

        {
                "gouge", &gsn_gouge,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 12,
                "gouge", "Your vision returns."
        },

        {
                "pugilism", &gsn_pugalism,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "pugilism", "!Pugilism!"
        },

        {
                "grab", &gsn_grab,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 8,
                "", "!Grab!"
        },

        {
                "mount", &gsn_mount,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "mount", "!Mount!"
        },

        {
                "dismount", &gsn_dismount,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "dismount", "!Dismount!"
        },

        {
                "battle aura", &gsn_battle_aura,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_null, 0, 8,
                "", "You feel less sure of yourself."
        },

        {
                "second punch", &gsn_second_punch,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 8,
                "second punch", "!Second Punch!"
        },

        {
                "meditate", &gsn_meditate,
                TYPE_STR, TAR_IGNORE, POS_RESTING, 16384,
                spell_null, 0, 12,
                "", "You awaken from your trance."
        },

        {
                "parry", &gsn_parry,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Parry!"
        },

        {
                "pre-empt", &gsn_pre_empt,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Pre-empt!"
        },

        {
                "peek", &gsn_peek,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Peek!"
        },

        {
                "pick lock", &gsn_pick_lock,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", "!Pick!"
        },

        {
                "bladethirst", &gsn_bladethirst,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_null, 0, 12,
                "concoction", "!Bladethirst!"
        },

        {
                "poison weapon", &gsn_poison_weapon,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_null, 0, 12,
                "poisonous concoction", "!Poison Weapon!"
        },

        {
                "forge", &gsn_forge,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_null, 0, 12,
                "slip", "!Forge!"
        },

        {
                "sharpen", &gsn_sharpen,
                TYPE_STR, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_null, 0, 12,
                "sharp blade", "!Sharpen!"
        },

        {
                "rescue", &gsn_rescue,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 12,
                "", "!Rescue!"
        },

        {
                "second attack", &gsn_second_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Second Attack!"
        },

        {
                "shield block", &gsn_shield_block,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Shield Block!"
        },

        {
                "smash", &gsn_smash,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "extra attack", "!Smash!"
        },

        {
                "sneak", &gsn_sneak,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "tail", &gsn_tail,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "stalk", &gsn_stalk,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", NULL
        },

        {
                "steal", &gsn_steal,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 24,
                "", "!Steal!"
        },

        {
                "stun", &gsn_stun,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "attempted stun", "Your head stops ringing."
        },

        {
                "suck", &gsn_suck,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "suck", "!Suck!"
        },

        {
                "third attack", &gsn_third_attack,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Third Attack!"
        },

        {
                "transfix", &gsn_transfix,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "terrifying gaze", "You shake free from the trance."
        },

        {
                "trap", &gsn_trap,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "slip up", "You finally break free from the trap!"
        },

        {
                "trip", &gsn_trip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 18,
                "", "<15>You get back on your feet.<0>"
        },

        {
                "unarmed combat", &gsn_unarmed_combat,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "", "The kill satisfies your lust for combat."
        },

        {
                "warcry", &gsn_warcry,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "You feel less enraged."
        },

        {
                "rage", &gsn_rage,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "You feel less furious."
        },

        {
                "howl", &gsn_howl,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "howl", "!Howl!"
        },

        {
                "whirlwind", &gsn_whirlwind,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 18, 18,
                "whirlwind", "You feel less angry."
        },

        {
                "flight", &gsn_flight,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 18, 18,
                "flight", "You fold your wings and settle on the ground."
        },

        {
                "general purpose", NULL,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_general_purpose, 0, 12,
                "general purpose ammo", "!General Purpose Ammo!"
        },

        {
                "high explosive", NULL,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_high_explosive, 0, 12,
                "high explosive ammo", "!High Explosive Ammo!"
        },

        {
                "addfame", &gsn_addfame,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Addfame!"
        },

        {
                "allow", &gsn_allow,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Allow!"
        },

        {
                "bamfin", &gsn_bamfin,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Bamfin!"
        },

        {
                "bamfout", &gsn_bamfout,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Bamfout!"
        },

        {
                "ban", &gsn_ban,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Ban!"
        },

        {
                "cando", &gsn_cando,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Cando!"
        },

        {
                "deny", &gsn_deny,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Deny!"
        },

        {
                "disconnect", &gsn_disconnect,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Disconnect!"
        },

        {
                "echo", &gsn_echo,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Echo!"
        },

        {
                "force", &gsn_force,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Force!"
        },

        {
                "freeze", &gsn_freeze,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Freeze!"
        },

        {
                "goto", &gsn_goto,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Goto!"
        },

        {
                "holylight", &gsn_holylight,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Holylight!"
        },

        {
                "immtalk", &gsn_immtalk,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Immtalk!"
        },

        {
                "dirtalk", &gsn_dirtalk,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Dirtalk!"
        },

        {
                "wizinvis", &gsn_wizinvis,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                    "", "!Wizinvis!"
        },

        {
                "killsocket", &gsn_killsocket,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Killsocket!"
        },

        {
                "log", &gsn_log,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Log!"
        },

        {
                "memory", &gsn_memory,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Memory!"
        },

        {
                "mfind", &gsn_mfind,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Mfind!"
        },

        {
                "mload", &gsn_mload,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Mload!"
        },

        {
                "oclanitem", &gsn_oclanitem,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!OclanItem!"
        },

        {
                "mset", &gsn_mset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Mset!"
        },

        {
                "mstat", &gsn_mstat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                    "", "!Mstat!"
        },

        {
                "mwhere", &gsn_mwhere,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Mwhere!"
        },

        {
                "newlock", &gsn_newlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Newlock!"
        },

        {
                "noemote", &gsn_noemote,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Noemote!"
        },

        {
                "notell", &gsn_notell,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Notell!"
        },

        {
                "numlock", &gsn_numlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Numlock!"
        },

        {
                "ofind", &gsn_ofind,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Ofind!"
        },

        {
                "oload", &gsn_oload,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Oload!"
        },

        {
                "oset", &gsn_oset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Oset!"
        },

        {
                "ostat", &gsn_ostat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Ostat!"
        },

        {
                "owhere", &gsn_owhere,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Owhere!"
        },

        {
                "pardon", &gsn_pardon,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Pardon!"
        },

        {
                "peace", &gsn_peace,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Peace!"
        },

        {
                "purge", &gsn_purge,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Purge!"
        },

        {
                "reboot", &gsn_reboot,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Reboot!"
        },

        {
                "recho", &gsn_recho,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Recho!"
        },

        {
                "rename", &gsn_rename,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Rename!"
        },

        {
                "reset", &gsn_reset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Reset!"
        },

        {
                "restore", &gsn_restore,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Restore!"
        },

        {
                "return", &gsn_return,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Return!"
        },

        {
                "rset", &gsn_rset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Rset!"
        },

        {
                "rstat", &gsn_rstat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Rstat!"
        },

        {
                "shutdown", &gsn_shutdown,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Shutdown!"
        },

        {
                "silence", &gsn_silence,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Silence!"
        },

        {
                "slay", &gsn_slay,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Slay!"
        },

        {
                "slookup", &gsn_slookup,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Slookup!"
        },

        {
                "snoop", &gsn_snoop,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Snoop!"
        },

        {
                "sset", &gsn_sset,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Sset!"
        },

        {
                "sstime", &gsn_sstime,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Sstime!"
        },

        {
                "switch", &gsn_switch,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Switch!"
        },

        {
                "transfer", &gsn_transfer,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Transfer!"
        },

        {
                "trust", &gsn_trust,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Trust!"
        },

        {
                "users", &gsn_users,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Users!"
        },

        {
                "wizhelp", &gsn_wizhelp,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Wizhelp!"
        },

        {
                "wizify", &gsn_wizify,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Wizify!"
        },

        {
                "wizlock", &gsn_wizlock,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Wizlock!"
        },

        {
                "zones", &gsn_zones,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Zones!"
        },

        {
                "adrenaline control", &gsn_adrenaline_control,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_adrenaline_control, 6, 12,
                "", "The adrenaline rush wears off."
        },

        {

                "agitation", &gsn_agitation,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_agitation, 10, 12,
                "agitation", "!Agitation!"
        },

        {
                "aura sight", &gsn_aura_sight,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_aura_sight, 9, 12,
                "", "!Aura Sight!"
        },

        {
                "awe", &gsn_awe,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_awe, 35, 12,
                "", "!Awe!"
        },

        {
                "ballistic attack", &gsn_ballistic_attack,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_ballistic_attack, 5, 12,
                "ballistic attack", "!Ballistic Attack!"
        },

        {
                "biofeedback", &gsn_biofeedback,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_biofeedback, 75, 12,
                "", "<250>Your biofeedback is no longer effective.<0>"
        },

        {
                "cell adjustment", &gsn_cell_adjustment,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_cell_adjustment, 8, 12,
                "", "!Cell Adjustment!"
        },

        {
                "chameleon power", &gsn_chameleon_power,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", "!Chameleon Power!"
        },

        {
                "combat mind", &gsn_combat_mind,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_combat_mind, 15, 12,
                "", "Your battle sense has faded."
        },

        {
                "recharge mana", &gsn_recharge_mana,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_recharge_mana, 100, 12,
                "", "!Recharge mana!"
        },

        {
                "vitalize", &gsn_vitalize,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_vitalize, 100, 12,
                "", "!Vitalize!"
        },

        {
                "complete healing", &gsn_complete_healing,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_complete_healing, 100, 12,
                "", "!Complete Healing!"
        },

        {
                "control flames", &gsn_control_flames,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_control_flames, 15, 12,
                "tongue of flame", "!Control Flames!"
        },

        {
                "create sound", &gsn_create_sound,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_create_sound, 5, 12,
                "", "!Create Sound!"
        },

        {
                "death field", &gsn_death_field,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384|262144,
                spell_death_field, 200, 18,
                "field of death", "!Death Field!"
        },

        {
                "decay", &gsn_decay,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384|262144,
                spell_decay, 100, 12,
                "slip up", "!Decay!"
        },

        {
                "detonate", &gsn_detonate,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 65536,
                spell_detonate, 25, 24,
                "detonation", "!Detonate!"
        },

        {
                "disintegrate", &gsn_disintergrate,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 65536,
                spell_disintegrate, 150, 18,
                "disintegration", "!Disintegrate!"
        },

        {
                "displacement", &gsn_displacement,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_displacement, 10, 12,
                "", "You are no longer displaced."
        },

        {
                "domination", &gsn_domination,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 16384,
                spell_domination, 5, 12,
                "", "You regain control of your body."
        },

        {
                "ectoplasmic form", &gsn_ectoplasmic_form,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_ectoplasmic_form, 20, 12,
                "", "You feel solid again."
        },

        {
                "ego whip", &gsn_ego_whip,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384|65536,
                spell_ego_whip, 20, 12,
                "", "You feel more confident."
        },

        {
                "energy containment", &gsn_energy_containment,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_energy_containment, 10, 12,
                "", "You no longer absorb energy."
        },

        {
                "enhance armor", &gsn_enhance_armor,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_enhance_armor, 100, 24,
                "", "!Enhance Armor!"
        },

        {
                "enhanced strength", &gsn_enhanced_strength,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_enhanced_strength, 20, 12,
                "", "Your strength is no longer enhanced."
        },

        {
                "flesh armor", &gsn_flesh_armor,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_flesh_armor, 15, 12,
                "", "Your skin returns to normal."
        },

        {
                "heighten senses", &gsn_heighten,
                TYPE_STR, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "Your senses return to normal."
        },

        {
                "inertial barrier", &gsn_inertial_barrier,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_inertial_barrier, 40, 24,
                "", "Your inertial barrier dissipates."
        },

        {
                "inflict pain", &gsn_inflict_pain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_inflict_pain, 10, 12,
                "mindpower", "!Inflict Pain!"
        },

        {
                "intellect fortress", &gsn_intellect_fortress,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_intellect_fortress, 25, 24,
                "", "Your intellectual fortress crumbles."
        },

        {
                "lend health", &gsn_lend_health,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_lend_health, 10, 12,
                "", "!Lend Health!"
        },

        {
                "levitation", &gsn_levitation,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_levitation, 10, 18,
                "", "The sensation of gravity gently returns to your body."
        },

        {
                "mental barrier", &gsn_mental_barrier,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_mental_barrier, 8, 12,
                "", "Your mental barrier breaks down."
        },

        {
                "mind thrust", &gsn_mind_thrust,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_mind_thrust, 8, 12,
                "mind thrust", "!Mind Thrust!"
        },

        {
                "project force", &gsn_project_force,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_project_force, 18, 12,
                "projected force", "!Project Force!"
        },

        {
                "psionic blast", &gsn_psionic_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_psionic_blast, 15, 12,
                "psionic blast", "!Psionic Blast!"
        },

        {
                "psychic crush", &gsn_psychic_crush,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_psychic_crush, 15, 12,
                "psychic crush", "!Psychic Crush!"
        },

        {
                "psychic drain", &gsn_psychic_drain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|65536,
                spell_psychic_drain, 20, 12,
                "", "You no longer feel drained."
        },

        {
                "psychic healing", &gsn_psychic_healing,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_psychic_healing, 20, 12,
                "", "!Psychic Healing!"
        },

        {
                "shadow form", &gsn_shadow_form,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "", "You no longer move in the shadows."
        },

        {
                "share strength", &gsn_share_strength,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_share_strength, 8, 12,
                "", "You no longer share strength with another."
        },

        {
                "thought shield", &gsn_thought_shield,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_thought_shield, 5, 12,
                "", "You no longer feel so protected."
        },

        {
                "ultrablast", &gsn_ultrablast,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384|65536,
                spell_ultrablast, 25, 12,
                "<14>u<121>l<191>t<226>r<15>ab<226>l<191>a<121>s<14>t<0>", "!Ultrablast!"
        },

        {
                "wrath of god", &gsn_wrath_of_god,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384|131072,
                spell_wrath_of_god, 25, 12,
                "god", "!Wrath Of God!"
        },

        {
                "feeblemind", &gsn_feeblemind,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384|65536,
                spell_feeblemind, 25, 12,
                "", "You feel less dizzy."
        },

        {
                "spiritwrack", &gsn_spiritwrack,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384|262144,
                spell_spiritwrack, 25, 12,
                "", "You feel more able."
        },

        {
                "wither", &gsn_wither,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|262144,
                spell_wither, 20, 12,
                "<112>w<76>i<40>t<11>h<227>e<254>r<15>in<254>g <227>g<11>r<40>a<76>s<112>p<0>", "You feel less brittle."
        },

        {
                "lore", &gsn_lore,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_lore, 10, 24,
                "", "!Lore!"
        },

        {
                "hex", &gsn_hex,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384|524288,
                spell_hex, 30, 12,
                "hex", "You are no longer hexed."
        },

        {
                "animate dead", &gsn_animate_dead,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING, 16384,
                spell_animate_dead, 100, 12,
                "", "!Animate Dead!"
        },

        {
                "dark ritual", &gsn_dark_ritual,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING, 16384|262144,
                spell_dark_ritual, 20, 10,
                "", "!dark ritual!"
        },

        {
                "bark skin", &gsn_bark_skin,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_bark_skin, 10, 12,
                "", "You no longer feel like a tree."
        },

        {
                "moonray", &gsn_moonray,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 8|16384,
                spell_moonray, 20, 12,
                "<230>m<229>o<228>on be<229>a<230>m<0>", "!moonray!"
        },

        {
                "sunray", &gsn_sunray,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 1|8|16384,
                spell_sunray, 25, 12,
                "<214>s<220>u<11>n r<220>a<214>y<0>", "!sunray!"
        },

        {
                "prayer", &gsn_prayer,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_prayer, 50, 12,
                "", "You feel your prayer cease."
        },

        {
                "frenzy", &gsn_frenzy,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_frenzy, 100, 24,
                "", "Slowly you release the rage as it threatens to overwhelm you."
        },

        {
                "mass heal", &gsn_mass_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_mass_heal, 150, 20,
                "", "!Mass Heal!"
        },

        {
                "mass power heal", &gsn_mass_power_heal,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_mass_power_heal, 300, 32,
                "", "!Mass Power Heal!"
        },

        {
                "mass sanctuary", &gsn_mass_sanctuary,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_mass_sanctuary, 200, 36,
                "", "<250>The white aura around your body fades.<0>"
        },

        {
                "globe of invulnerability", &gsn_globe,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_globe, 120, 12,
                "", "<208>The globe about you implodes.<0>"
        },

        {
                "dark globe of invulnerability", &gsn_dark_globe,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_dark_globe, 120, 12,
                "", "<208>The dark globe about you implodes.<0>"
        },

        {
                "firestorm", &gsn_firestorm,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 1|16384,
                spell_firestorm, 20, 6,
                "{Rfi{Yre{Ws{Yto{Rrm{X", "!Firestorm!"
        },

        {
                "meteor storm", &gsn_meteor_storm,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 1|16384,
                spell_meteor_storm, 25, 18,
                "<196>me<202>te<11>o<214>r <196>st<202>or<11>m<0>", "!Meteor storm!"
        },

        {
                "bless weapon", &gsn_bless_weapon,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_bless_weapon, 100, 24,
                "", "!Bless Weapon!"
        },

        {
                "recharge item", &gsn_recharge_item,
                TYPE_INT, TAR_OBJ_INV, POS_STANDING, 16384,
                spell_recharge_item, 100, 24,
                "", "!Recharge Item!"
        },

        {
                "kiai", &gsn_kiai,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 20, 12,
                "<11>f<227>o<229>r<230>c<15>e-<230>b<229>a<227>l<11>l<0>", "!Kiai!"
        },

        {
                "possession", &gsn_possession,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_possession, 25, 12,
                "attempted possession", "You return to your own body."
        },

        {
                "demon flames", &gsn_demon_flames,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 1|16384|262144,
                spell_demon_flames, 25, 12,
                "horde of demons", "You feel less tormented."
        },

        {
                "steal strength", &gsn_steal_strength,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384,
                spell_steal_strength, 25, 12,
                "spell", "You feel less strong."
        },

        {
                "infernal fury", &gsn_infernal_fury,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_infernal_fury, 25, 12,
                "summoned <124>d<160>e<196>v<160>i<124>l<0>", "!Infernal_Fury!"
        },

        {
                "abyssal hand", &gsn_abyssal_hand,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 4096|16384,
                spell_abyssal_hand, 100, 12,
                "abyssal hand", "You break free from the black hand that holds you."
        },

        {
                "steal soul", &gsn_steal_soul,
                TYPE_INT, TAR_OBJ_ROOM, POS_STANDING, 16384|262144,
                spell_steal_soul, 25, 12,
                "", "!Steal Soul!"
        },

        {
                "summon demon", &gsn_summon_demon,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_summon_demon, 100, 12,
                "", "!Summon Demon!"
        },

        {
                "evocation magiks", &gsn_group_evocation,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Evocation!"
        },

        {
                "death magiks", &gsn_group_death,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Death!"
        },

        {
                "dark magiks", &gsn_group_dark,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Dark!"
        },

        {
                "lycanthropy skills", &gsn_group_lycanthropy,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Lycanthropy!"
        },

        {
                "vampyre skills", &gsn_group_vampyre,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Vampyre!"
        },

        {
                "draconian magiks", &gsn_group_breath,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Breath!"
        },

        {
                "destructive magiks", &gsn_group_destruction,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Destruction!"
        },

        {
                "major protective magiks", &gsn_group_majorp,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Major Protection!"
        },

        {
                "mana craft", &gsn_group_craft,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Mana Craft!"
        },

        {
                "divination magiks", &gsn_group_divination,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Divination!"
        },

        {
                "alteration magiks", &gsn_group_alteration,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Alteration!"
        },

        {
                "summoning magiks", &gsn_group_summoning,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Summoning!"
        },

        {
                "illusion magiks", &gsn_group_illusion,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Illusion!"
        },

        {
                "enchantment magiks", &gsn_group_enchant,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Enchantment!"
        },

        {
                "conjuration magiks", &gsn_group_conjuration,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Conjuration!"
        },

        {
                "protective magiks", &gsn_group_protection,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Protection!"
        },

        {
                "divine magiks", &gsn_group_divine,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Divine!"
        },

        {
                "harmful magiks", &gsn_group_harmful,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Harmful!"
        },

        {
                "disease magiks", &gsn_group_disease,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Disase!"
        },

        {
                "curative magiks", &gsn_group_curative,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Curative!"
        },

        {
                "healing magiks", &gsn_group_healing,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Healing!"
        },

        {
                "advanced healing magiks", &gsn_group_advanced_heal,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Advanced Healing!"
        },

        {
                "mental defense disciplines", &gsn_group_mentald,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Mental Defense!"
        },

        {
                "matter control disciplines", &gsn_group_matter,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Matter Control!"
        },

        {
                "energy control disciplines", &gsn_group_energy,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Energy Control!"
        },

        {
                "telepathy disciplines", &gsn_group_telepathy,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Telepathy!"
        },

        {
                "advanced telepathy disciplines", &gsn_group_advtele,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Advanced Telepathy!"
        },

        {
                "mana control disciplines", &gsn_group_mana,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Mana Control!"
        },

        {
                "combat knowledge", &gsn_group_combat  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Combat!"
        },

        {
                "poison arts", &gsn_group_poison  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Poison!"
        },

        {
                "stealth techniques", &gsn_group_stealth  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Stealth!"
        },

        {
                "hunting arts", &gsn_group_hunting  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Hunting!"
        },

        {
                "martial arts", &gsn_group_arts,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Arts!"
        },

        {
                "combination moves", &gsn_group_combos  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Combos!"
        },

        {
                "riding techniques", &gsn_group_riding  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Riding!"
        },

        {
                "thievery skills", &gsn_group_thievery  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Thievery!"
        },

        {
                "armed combat knowledge", &gsn_group_armed  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Armed Combat!"
        },

        {
                "body control disciplines", &gsn_group_body  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Body Control!"
        },

        {
                "morph", &gsn_morph,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 20, 12,
                "", "!Morph!"
        },

        {
                "inner force", &gsn_group_inner  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Inner Force!"
        },

        {
                "knowledge", &gsn_group_knowledge  ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Knowledge!"
        },

        {
                "nature skills", &gsn_group_nature,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group NAture!"
        },

        {
                "unarmed combat knowledge", &gsn_group_unarmed,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Unarmed Combat!"
        },

        {
                "defense knowledge", &gsn_group_defense ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Defense!"
        },

        {
                "advanced combat knowledge", &gsn_group_advcombat ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Advanced Combat!"
        },

        {
                "metal working techniques", &gsn_group_metal ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Metal!"
        },

        {
                "morphing knowledge", &gsn_group_forms   ,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Forms!"
        },

        {
                "morphing abilities", &gsn_group_morph,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Morphing Abilities"
        },

        {
                "pugilism knowledge", &gsn_group_pugalism,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Pugilism!"
        },

        {
                "fear", &gsn_fear,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_FIGHTING, 16384|32768,
                spell_fear, 5, 12,
                "spell", "You feel less afraid."
        },

        {
                "deter", &gsn_deter,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_STANDING, 16384,
                spell_deter, 5, 12,
                "", "Monsters seem less intimidated by you..."
        },

        {
                "astral sidestep", &gsn_astral_sidestep,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_astral_sidestep, 5, 12,
                "", "You rematerialise in the physical plane."
        },

        {
                "mist walk", &gsn_mist_walk,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 5, 12,
                "", "Your body reverts to its normal state."
        },

        {
                "gaias warning", &gsn_gaias_warning,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_gaias_warning, 5, 12,
                "", "You feel less astute."
        },

        {
                "resist toxin", &gsn_resist_toxin,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_null, 0, 0,
                "", "!Resist Toxin!"
        },

        {
                "resist magic", &gsn_resist_magic,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_resist_magic, 5, 12,
                "", "You feel more vulnerable to magic."
        },

        {
                "summon avatar", &gsn_summon_avatar,
                TYPE_INT, TAR_IGNORE, POS_FIGHTING, 16384,
                spell_summon_avatar, 100, 18,
                "", "!Summon Avatar!"
        },

        {
                "feed", &gsn_feed,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 18,
                "", "!Feed!"
        },

        {
                "lunge", &gsn_lunge,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 18,
                "blood-thirsty fangs", "!lunge!"
        },

        {
                "aura of fear", &gsn_aura_of_fear,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 18,
                "aura of fear", "!Aura Of Fear!"
        },

        {
                "double lunge", &gsn_double_lunge,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 18,
                "blood-thirsty fangs", "!Feed!"
        },

        {
                "chameleon form", &gsn_form_chameleon,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 10, 0,
                "", "!Form Chameleon!"
        },

        {
                "hawk form", &gsn_form_hawk,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 15, 0,
                "", "!Form Hawk!"
        },

        {
                "cat form", &gsn_form_cat,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 15, 0,
                "", "!Form Cat!"
        },

        {
                "bat form", &gsn_form_bat,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 50, 0,
                "", "!Form Bat!"
        },

        {
                "snake form", &gsn_form_snake,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 20, 0,
                "", "!Form Snake!"
        },

        {
                "scorpion form", &gsn_form_scorpion,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 25, 0,
                "", "!Form Scorpion"
        },

        {
                "spider form", &gsn_form_spider,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 25, 0,
                "", "!Form Spider!"
        },

        {
                "bear form", &gsn_form_bear,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 30, 0,
                "", "!Form Bear!"
        },

        {
                "tiger form", &gsn_form_tiger,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 35, 0,
                "", "!Form Tiger!"
        },

        {
                "dragon form", &gsn_form_dragon,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Dragon!"
        },

        {
                "demon form", &gsn_form_demon,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 75, 0,
                "", "!Form Demon!"
        },

        {
                "phoenix form", &gsn_form_phoenix,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 120, 0,
                "", "!Form Phoenix!"
        },

        {
                "hydra form", &gsn_form_hydra,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Hydra!"
        },

        {
                "wolf form", &gsn_form_wolf,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Wolf!"
        },

        {
                "fly form", &gsn_form_fly,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 200, 0,
                "", "!Form Phoenix!"
        },

        {
                "griffin form", &gsn_form_griffin,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Griffin!"
        },

        {
                "wolf aura", &gsn_form_direwolf,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 150, 0,
                "", "!Form Direwolf!"
        },

        {
                "elemental air form", &gsn_form_elemental_air,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Elemental Air!"
        },

        {
                "elemental fire form", &gsn_form_elemental_fire,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 1,
                spell_null, 100, 0,
                "", "!Form Elemental Fire!"
        },

        {
                "elemental water form", &gsn_form_elemental_water,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Elemental Water!"
        },

        {
                "elemental earth form", &gsn_form_elemental_earth,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 100, 0,
                "", "!Form Elemental Earth!"
        },

        {
                "coil", &gsn_coil,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 24,
                "coil", "!Coil!"
        },

        {
                "constrict", &gsn_constrict,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 10,
                "constriction", "!Constrict!"
        },

        {
                "strangle", &gsn_strangle,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 30,
                "strangle", "!Strangle!"
        },

        {
                "swim", &gsn_swim,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Swim!"
        },

        {
                "spy", &gsn_spy,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "", "!Spy!"
        },

        {
                "forage", &gsn_forage,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 10,
                "", "!Forage!"
        },

        {
                "bite", &gsn_bite,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 10,
                "bite", "!Bite!"
        },

        {
                "maul", &gsn_maul,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 18,
                "frenzied attack", "!Maul!"
        },

        {
                "wolfbite", &gsn_wolfbite,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 10,
                "wolfbite", "!Wolfbite!"
        },

        {
                "ravage", &gsn_ravage,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 18,
                "frenzied attack", "!Ravage!"
        },

        {
                "web", &gsn_web,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 12,
                "slip up", "You break free from the sticky webbing."
        },

        {
                "venom", &gsn_venom,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 12,
                "poisoned bite", "!Venom!"
        },

        {
                "breathe", &gsn_breathe,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 50, 12,
                "breath", "!Breathe!"
        },

        {
                "dive", &gsn_dive,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 18,
                "airborne assault", "!Dive!"
        },

        {
                "tailwhip", &gsn_tailwhip,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "tailwhip", "!Tailwhip!"
        },

        {
                "dragon aura", &gsn_dragon_aura,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "", "Your dragon aura fades."
        },

        {
                "spellcraft", &gsn_spellcraft,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!Spy!"
        },

        {
                "dowse", &gsn_dowse,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "dowse", "!Dowse!"
        },

        {
                "thrust", &gsn_thrust,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 0,
                "thrust", "!Thrust!"
        },

        {
                "shoot", &gsn_shoot,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "shot", "!Shoot!"
        },

        {
                "second shot", &gsn_second_shot,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "second shot", "!Second shot!"
        },

        {
                "third shot", &gsn_third_shot,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "third shot", "!Third shot!"
        },

        {
                "accuracy", &gsn_accuracy,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "accuracy", "!Accuracy!"
        },

        {
                "snare", &gsn_snare,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "slip up", "You finally break free from the snare!"
        },

        {
                "classify", &gsn_classify,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "classify", "!classify!"
        },

        {
                "gather", &gsn_gather_herbs,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "gather", "!Gather!"
        },

        {
                "archery knowledge", &gsn_group_archery,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "archery knowledge", "!Archery Knowledge!"
        },

        {
                "musicianship", &gsn_group_music,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "musicianship", "!Musicianship!"
        },

        {
                "herb lore", &gsn_group_herb_lore,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "herb lore", "!Herb Lore!"
        },

        {
                "song of revelation", &gsn_song_of_revelation,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of revelation", "!Song of Revelation!"
        },

        {
                "song of rejuvenation", &gsn_song_of_rejuvenation,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of rejuvenation", "!Song of Rejuvenation!"
        },

        {
                "song of tranquility", &gsn_song_of_tranquility,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of tranquility", "!Song of Tranquility!"
        },

        {
                "song of shadows", &gsn_song_of_shadows,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of shadows", "!Song of Shadows!"
        },

        {
                "song of sustenance", &gsn_song_of_sustenance,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of sustenance", "!Song of Sustenance!"
        },

        {
                "song of flight", &gsn_song_of_flight,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "song of flight", "!Song of Flight!"
        },

        {
                "chant of protection", &gsn_chant_of_protection,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "chant of protection", "Your chant of protection ceases."
        },

        {
                "chant of battle", &gsn_chant_of_battle,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "chant of battle", "Your battle chant ends."
        },

          {
                  "chant of vigour", &gsn_chant_of_vigour,
                  TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                  spell_null, 0, 0,
                  "chant of vigour", "Your chant of vigour ends."
          },

          {
                  "chant of enfeeblement", &gsn_chant_of_enfeeblement,
                  TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384|32768,
                  spell_null, 0, 0,
                  "chant of enfeeblement", "!Chant of Enfeeblement!"
          },

        {
                "chant of pain", &gsn_chant_of_pain,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 8|16384,
                spell_null, 0, 0,
                "chant of pain", "!Chant of Pain!"
        },

        {
                "wizbrew", &gsn_wizbrew,
                TYPE_WIZ, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "wizbrew", "!wizbrew!"
        },

        {
                "breathe water", &gsn_breathe_water,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_breathe_water, 10, 12,
                "", "Your lungs revert to normal."
        },

        {
                "pantheoninfo", &gsn_pantheoninfo,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "pantheoninfo", "!pantheoninfo!"
        },

        {
                "pantheonrank", &gsn_pantheonrank,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "pantheonrank", "!pantheonrank!"
        },

        {
                "holy prayer of combat", &gsn_prayer_combat,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "holy prayer of combat", "Your holy prayer's power wavers and ends."
        },

        {
                "holy prayer of protection", &gsn_prayer_protection,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "holy prayer of protection", "Your holy prayer's power wavers and ends."
        },

        {
                "holy prayer of free passage", &gsn_prayer_passage,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "holy prayer of free passage", "Your holy prayer's power wavers and ends."
        },

        {
                "divine curse", &gsn_prayer_weaken,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 524288,
                spell_null, 0, 0,
                "divine curse", "You are no longer cursed by the gods."
        },

        {
                "resist heat", &gsn_resist_heat,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_resist_heat, 15, 12,
                "resist heat", "You feel more vulnerable to heat and flame."
        },

        {
                "resist cold", &gsn_resist_cold,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_resist_cold, 15, 12,
                "resist cold", "You feel more vulnerable to cold and ice."
        },

        {
                "resist lightning", &gsn_resist_lightning,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 4,
                spell_resist_lightning, 15, 12,
                "resist lightning", "You feel more vulnerable to electricity."
        },

        {
                "resist acid", &gsn_resist_acid,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 128,
                spell_resist_acid, 15, 12,
                "resist acid", "You feel more vulnerable to acid."
        },

        {
                "hellfire", &gsn_hells_fire,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_hells_fire, 20, 12,
                "<160>h<124>e<88>l<124>l<160>f<124>i<88>r<124>e<0>", "!Hellfire!"
        },

        {
                "holy prayer of destruction", &gsn_prayer_destruction,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "<556><196>+*(<15>deity<196>)*+<557>", "!holy prayer of destruction"
        },

        {
                "plague", &gsn_prayer_plague,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 524288,
                spell_null, 0, 0,
                "plague", "You have recovered from your illness."
        },

        {
                "resistance magiks", &gsn_group_resistance,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "resistance magiks", "!Group Resistance!"
        },

        {
                "fury of nature", &gsn_natures_fury,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_natures_fury, 25, 12,
                "magical assault", ""
        },

        {
                "dual parry", &gsn_dual_parry,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Parry!"
        },

        {
                "addqp", &gsn_addqp,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "", "!Addqp!"
        },

        {
                "chaos blast", &gsn_chaos_blast,
                TYPE_INT, TAR_CHAR_OFFENSIVE_SINGLE, POS_STANDING, 8|16384,
                spell_chaos_blast, 5, 12,
                "<242>c<251>h<242>a<251>o<242>s<0> <242>b<251>l<242>a<251>s<242>t<0>", "!Chaos Blast!"
        },

        {
                "detect curse", &gsn_detect_curse,
                TYPE_INT, TAR_CHAR_SELF, POS_STANDING, 16384,
                spell_detect_curse, 5, 12,
                "", "You no longer feel an affinity for the accursed."
        },

        {
                "knife toss", &gsn_knife_toss,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 8,
                "knife toss", "!Knife Toss!"
        },

        {
                "soar", &gsn_soar,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 24,
                "", "!Soar!"
        },

        {
                "smoke bomb", &gsn_smoke_bomb,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 8,
                "smoke bomb", "!Smoke Bomb!"
        },

        {
                "snap shot", &gsn_snap_shot,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 24,
                "snap shot", "!Snap Shot!"
        },

        {
                "crush", &gsn_crush,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 18,
                "crush", "!Crush!"
        },

        {
                "swoop", &gsn_swoop,
                TYPE_STR, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 16384,
                spell_null, 0, 18,
                "swoop", "!Swoop!"
        },

        {
                "smelt", &gsn_smelt,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "smelt", "!Smelt!"

        },

        {
                "imbue", &gsn_imbue,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "imbue", "!Imbue!"

        },

        {
                "counterbalance", &gsn_counterbalance,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "counterbalanced attack", "!Counterbalance!"
        },

        {
                "trigger", &gsn_trigger,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "turret module", "!Trigger!"
        },

        {
                "dart", &gsn_dart,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "dart module", "!Dart!"
        },

        {
                "empower", &gsn_empower,
                TYPE_INT, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "empower", "!Empower!"

        },

        {
                "osstat", &gsn_osstat,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Osstat!"
        },

        {
                "osfind", &gsn_osfind,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Ostat!"
        },

        {
                "hurl", &gsn_hurl,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "hurled weapon", "!Hurl!"
                /* is used for weapons and shields but 'hurled weapon' is not called by shield attack, so. */
        },

        {
                "arrow module", &gsn_arrow,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "arrow module", "!Arrow!"
        },

        {
                "turret", &gsn_turret,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "turret base", "!Turret!"
        },

        {
                "uncommon set", &gsn_uncommon_set,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "uncommon set", "!Uncommon_set!"
        },

        {
                "rare set", &gsn_rare_set,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "rare set", "!Rare_set!"
        },

        {
                "bloodforged set", &gsn_bloodforged_set,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "bloodforged set", "!Bloodforged_set!"
        },

        {
                "astral set", &gsn_astral_set,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "astral set", "!Astral_set!"
        },

        {
                "weaponsmithing", &gsn_group_weaponsmith,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Weaponsmith!"
        },

        {
                "armoursmithing", &gsn_group_armoursmith,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Armoursmith!"
        },

        {
                "weaponchain", &gsn_weaponchain,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "chained weapon", "!Weaponchain!"
        },

        {
                "shieldchain", &gsn_shieldchain,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "chained shield", "!Shieldchain!"
        },

        {
                "strengthen", &gsn_strengthen,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "strengthen", "!Strengthen!"
        },

        {
                "steel broadsword", &gsn_steel_broadsword,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "steel broadsword", "!Steel_Broadsword!"
        },

        {
                "titanium rapier", &gsn_titanium_rapier,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "titanium rapier", "!Titanium_rapier!"
        },

        {
                "slow", &gsn_slow,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384|32768,
                spell_slow, 200, 18,
                "spell", "You feel yourself speed up."
        },

        {
                "stabilise", &gsn_stabilise,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_stabilise, 50, 12,
                "spell", "Your body returns to its normal state."
        },

        {
                "engrave", &gsn_engrave,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "engrave", "!Engrave!"
        },

        {
                "discharge", &gsn_discharge,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "discharge", "!Discharge!"
        },

        {
                "steel cache", &gsn_steel_cache,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "steel cache", "!Steel_cache!"
        },

        {
                "flukeslap", &gsn_flukeslap,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "flukeslap", "!Flukeslap!"
        },

        {
                "swallow", &gsn_swallow,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "<132>s<168>w<204>a<204>l<203>l<168>o<132>w<0>", "You escape from the insides of the creature."
        },

        {
                "serrate", &gsn_serrate,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "<231>bl<225>ee<219>d e<213>ff<207>ec<201>t<0>", "!Serrate!"
        },

        {
                "innate knowledge", &gsn_innate_knowledge,
                TYPE_INT, TAR_IGNORE, POS_RESTING, 16384,
                spell_null, 0, 0,
                "innate knowledge", "!Innate Knowledge!"
        },

        {
                "spit mucus", &gsn_spit_mucus,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "<236>e<237>x<238>p<239>e<240>c<241>t<242>o<243>r<245>a<246>t<247>e<248>d <249>m<250>u<251>c<252>u<253>s<0>", "You regain autonomy over your body."
        },

        {
                "steam breath", &gsn_steam_breath,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1|16384,
                spell_steam_breath, 50, 12,
                "<51>j<87>e<123>t<159> o<195>f<253> s<254>t<255>e<15>a<556>m<0>", "!Steam Breath!"
        },

        {
                "mech tech", &gsn_group_mech_tech,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "", "!Group MechTech!"
        },

        {
                "turret tech", &gsn_group_turret_tech,
                TYPE_STR, TAR_IGNORE, POS_DEAD, 512,
                spell_null, 0, 0,
                "", "!Group TurretTech!"
        },

        {
                "runic arts", &gsn_group_runic_arts,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Runic Arts!"
        },

        {
                "inscription", &gsn_group_inscription,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Inscription!"
        },

        {
                "advanced smithing", &gsn_group_adv_smith,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Advanced Smithing!"
        },

        {
                "weapon lore", &gsn_group_weaponlore,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Weapon Lore!"
        },

        {
                "blade module", &gsn_blade_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "razor blade", "!Blade Module!"
        },

        {
                "shuriken module", &gsn_shuriken_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "shuriken", "!Shuriken Module!"
        },

        {
                "spear module", &gsn_spear_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "sharpened spear", "!Spear Module!"
        },

        {
                "arrestor module", &gsn_arrestor_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Arrestor Module!"
        },

        {
                "driver module", &gsn_driver_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!DRiver Module!"
        },

        {
                "reflector module", &gsn_reflector_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "reflected spell", "!DRiver Module!"
        },

        {
                "shield module", &gsn_shield_module,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Shield Module!"
        },

        {
                "adamantite katana", &gsn_adamantite_katana,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Titanium_rapier!"
        },

        {
                "electrum sword", &gsn_electrum_sword,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Electrum sword!"
        },

        {
                "starmetal dual blade", &gsn_starmetal_dual_blade,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "", "!Starmetal dual blade!"
        },

        {
                "inscribe", &gsn_inscribe,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 24,
                "inscribe", "!Inscribe!"
        },

        {
                "adamantite runic blade", &gsn_adamantite_runic_blade,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "runic blade", "!Adamantite runic blade!"
        },

        {
                "electrum runic blade", &gsn_electrum_runic_blade,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "runic blade", "!Electrum runic blade!"
        },

        {
                "starmetal runic blade", &gsn_starmetal_runic_blade,
                TYPE_STR, TAR_IGNORE, POS_FIGHTING, 8192,
                spell_null, 0, 0,
                "runic blade", "!Starmetal runic blade!"
        },

        {
                "pyro rune", &gsn_pyro_rune,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 1,
                spell_runic_flames, 0, 0,
                "runic flames", "!Pyro rune!"
        },

        {
                "cryo rune", &gsn_cryo_rune,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 2,
                spell_runic_frost, 0, 0,
                "runic frost", "!Cryo rune!"
        },

        {
                "bolt rune", &gsn_bolt_rune,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 4,
                spell_runic_bolts, 0, 0,
                "runic bolts", "!Bolt rune!"
        },

        {
                "stab rune", &gsn_stab_rune,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 32,
                spell_runic_stab, 0, 0,
                "runic stab", "!Pyro rune!"
        },

        {
                "rend rune", &gsn_rend_rune,
                TYPE_INT, TAR_CHAR_OFFENSIVE, POS_FIGHTING, 64,
                spell_runic_rend, 0, 0,
                "runic rend", "!Rend rune!"
        },

        {
                "mending rune", &gsn_mend_rune,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_runic_mend, 0, 0,
                "runic mend", "!Mending rune!"
        },

        {
                "cure rune", &gsn_cure_rune,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_runic_cure, 0, 0,
                "runic cure", "!Cure rune!"
        },

        {
                "ward rune", &gsn_ward_rune,
                TYPE_INT, TAR_CHAR_DEFENSIVE, POS_FIGHTING, 16384,
                spell_runic_ward, 0, 0,
                "runic cure", "!Ward rune!"
        },

        {
                "rune casting", &gsn_group_rune_casting,
                TYPE_INT, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!Group Rune Casting!"
        },

        {
                "oscore", &gsn_oscore,
                TYPE_WIZ, TAR_IGNORE, POS_DEAD, 0,
                spell_null, 0, 0,
                "", "!OScore!"
        },

        {
                "reforge", &gsn_reforge,
                TYPE_STR, TAR_IGNORE, POS_STANDING, 8192,
                spell_null, 0, 0,
                "reforge", "!Reforge!"
        },
        /*
         *  Add new spells/skills at the end of the section just above.  NOWHERE ELSE.
         */

        /*
         *  Base skills for classes :)  ALWAYS LEAVE BASE SKILLS AT END OF THE TABLE !!!
         */

        {
                "mage base", &gsn_mage_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-mage base-!"
        },

        {
                "cleric base", &gsn_cleric_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-cleric base-!"
        },

        {
                "thief base", &gsn_thief_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-thief base-!"
        },

        {
                "warrior base", &gsn_warrior_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-warrior base-!"
        },

        {
                "psionic base", &gsn_psionic_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-psionic base-!"
        },

        {
                "shifter base", &gsn_shifter_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-shifter base-!"
        },

        {
                "brawler base", &gsn_brawler_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-brawler base-!"
        },

        {
                "ranger base", &gsn_ranger_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-ranger base-!"
        },

        {
                "smithy base", &gsn_smithy_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-smithy base-!"
        },

        /*
         * Put the base class for any new classes added just above here,
         * and set LAST_BASE_CLASS_INDEX in merc.h to its value.
         */

        {
                "necro base", &gsn_necro_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-necro base-!"
        },

        {
                "warlock base", &gsn_warlock_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-warlock base-!"
        },

        {
                "templar base", &gsn_templar_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-templar base-!"
        },

        {
                "druid base", &gsn_druid_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-druid base-!"
        },

        {
                "ninja base", &gsn_ninja_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-ninja base-!"
        },

        {
                "bounty base", &gsn_bounty_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-bounty base-!"
        },

        {
                "thug base", &gsn_thug_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-thug base-!"
        },

        {
                "knight base", &gsn_knight_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-knight base-!"
        },

        {
                "infernalist base", &gsn_infernalist_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-infernalist base-!"
        },
        {
                "witch base", &gsn_witch_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-witch base-!"
        },


        {
                "werewolf base", &gsn_werewolf_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-werewolf base-!"
        },

        {
                "vampire base", &gsn_vampire_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-vampire base-!"
        },

        {
                "monk base", &gsn_monk_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-monk base-!"
        },

        {
                "artist base", &gsn_martist_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-martial artist base-!"
        },

        {
                "barbarian base", &gsn_barbarian_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-barbarian base-!"
        },

        {
                "bard base", &gsn_bard_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-bard base-!"
        },

        {
                "engineer base", &gsn_engineer_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-engineer base-!"
        },

        {
                "runesmith base", &gsn_runesmith_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-runesmith base-!"
        },

        /*
         *  When adding base skills for new classes, put the base class base after the last base class above
         *  (smithy base as at 12/8/22) the order is critical.  Also set LAST_BASE_CLASS_INDEX in merc.h to its
         *  value.  Order of base skills and subclasses should follow order defined in merc.h.  See also 'help
         *  SUBZ' online.
         */

        /*
         *  The very last skill - ESSENTIAL - after the class bases
         */

        {
                "teacher base", &gsn_teacher_base,
                TYPE_NULL, TAR_IGNORE, POS_STANDING, 16384,
                spell_null, 0, 0,
                "", "!-teacher base-!"
        }
};


/* EOF const.c */
