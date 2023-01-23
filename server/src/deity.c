/*
 *      deity.c
 *
 *      Routines and definitions used for the deity system on the
 *      Dragons Domain 4 Envy-style MUD.
 *
 *      To stop deity code being used during the game, alter the
 *      function 'deities_active' to return FALSE.
 *
 *      Gezhp and Shade, 2000-2001
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


/*
 *      Data
 */

const struct deity_info deity_info_table [NUMBER_DEITIES] =
{
        /*
         *  Name, title, description, sex, alignment, type, personality,
         *  extra flags, class favour, subclass favour, race favour.
         *
         *  Use the functions get_class_favour and get_race favour to
         *  determine a characters favour; get_class_favour handles
         *  sub-classes correctly.
         */

        { "Hurin",
                "the Hunter",
                "The vast forests of the Earth are the Hunter's domain; steadfastly he "
                        "protects the woodlands and the creatures that roam there.  "
                        "Hurin feels akin to the Elves, and despises the Goblinkind and "
                        "their allies, who destroy vast tracts of ancient forest as "
                        "their settlements expand.",
                SEX_MALE,
                350,
                DEITY_TYPE_NATURE,
                DEITY_PERSONALITY_PROTECTIVE,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  0,  0,  1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  1,  1, -1,  0,  1,  0, -1,  0,  0,  0,  1,  0, -1,  0, -1,  0,  1,  0,  0,  0,  0, -1,  1,  -1 }},

        { "Tulgor",
                "the Lord of Lightning",
                "Tulgor is the mightiest of the Titans, immensely powerful and concerned "
                        "only with physical power and the domination of others.  He seeks "
                        "to wipe out the lesser races by whatever means possible in order "
                        "to further his own strength.",
                SEX_MALE,
                -400,
                DEITY_TYPE_CHAOTIC,
                DEITY_PERSONALITY_JUDGEMENTAL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  {-1,  0,  0,  1,  0,  0,  1, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0, -1,  0,  0,  1, -1,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0, -1, -1,  0,  1,  0,  1,  0,  0, -1, -1,  0, -1,  1,  1,  0,  0, -1,  1,  0,  1,  0,  1,  0,  1 }},

        { "Lloth",
                "Queen of Darkness",
                "Spawned in impenetrable vaults sunken deep in the Earth, consuming all "
                        "that is light and goodness, the Spider Queen Lloth commands the "
                        "legion of Drow as they spread chaos throughout the Domain.  Her "
                        "mind is malignant and evil, and her cruelness is unparalleled; "
                        "even her most trusted minions live in terror of her wrath.",
                SEX_FEMALE,
                -750,
                DEITY_TYPE_CHAOTIC,
                DEITY_PERSONALITY_CRUEL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 1,  1,  0,  1,  0,  0,  0, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0, -1,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0, -1, -1,  1,  0,  0,  0,  1,  0, -1, -1,  0,  1,  0,  0,  0,  0, -1,  1,  0,  0,  0,  1,  -1,  1 }},

        { "Sephusia",
                "the Serpent",
                "Coldblooded and heartless, without fear or conscience, the Serpent "
                        "commands her reptillian armies and weaves powerful and terrible "
                        "magicks from her subterranian nest.  Hateful of surface-dwellers, "
                        "she fashions her snakeman warriors and plots their rampage throughout "
                        "the cities of the Domain.",
                SEX_FEMALE,
                -550,
                DEITY_TYPE_WARLIKE,
                DEITY_PERSONALITY_CRUEL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 1,  0,  0,  0,  0,  1,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0, -1, -1,  0,  0,  0,  0,  0,  1, -1, -1,  0,  0,  0,  0,  0,  1, -1, -1,  0,  0,  0,  1,  -1,  1 }},

        { "Torm",
                "Hand of Justice",
                "Chivalrous, honest and brave, Torm is the patron of Knights and Templars, "
                        "the protectors and crusaders of the Domain.  He seeks to bring "
                        "law, order and justice to the Earth's kingdoms, commanding a "
                        "righteous army to keep the wild hordes in check.",
                SEX_MALE,
                800,
                DEITY_TYPE_LAWFUL,
                DEITY_PERSONALITY_PROTECTIVE,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  1, -1,  0,  0,  0,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0, -1,  0,  1,  0, -1,  0, -1,  1, -1,  0, -1, -1,  1,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0,  0, -1,  1, -1,  0,  0,  0, -1,  -1,  -1 }},

        { "Ealindel",
                "the Elfin Maiden",
                "Her beauty and gentleness are unrivaled, her purity and kindness "
                        "absolute.  The princess Ealindel treads among the "
                        "sacred glades of the high elves, her gentle singing spreading "
                        "waves of rejuvenation and hope throughout the great forests.",
                SEX_FEMALE,
                1000,
                DEITY_TYPE_PEACEFUL,
                DEITY_PERSONALITY_PROTECTIVE,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  1,  0,  0,  0,  0,  0,  1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  1,  1,  1, -1,  0,  0,  0, -1,  0,  0,  0,  0, -1,  0,  0, -1, -1,  1,  0,  0,  0,  0, -1,  0,  -1 }},

        { "Thalin",
                "Hammer of the Dwarves",
                "Master of the dwarves, Thalin is a stout warrior and skilled craftsman.  "
                        "Deep he delved into the mountains of the Domain, carving out vast "
                        "cities and mines.  Fierce and proud, wielding his mighty hammer, "
                        "he musters his warriors to battle Goblinkind and their evil allies.",
                SEX_MALE,
                400,
                DEITY_TYPE_LAWFUL,
                DEITY_PERSONALITY_PROUD,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  {-1,  1,  0,  1, -1,  0,  1,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0, -1, -1,  0,  0,  0,  0,  0,  0, -1, -1,  0,  0,  0,  0,  1,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0, -1, -1,  0, -1, -1, -1,  0,  1,  0,  0, -1,  0,  0,  0,  0,  0,  0, -1,  0, -1,  0,  0 }},

        { "Televar",
                "Lord Morning",
                "Televar is the lord of the rising sun and the changing seasons.  He "
                        "is deeply in tune with the workings of the universe, the cycles "
                        "of nature and the creatures of the Earth.  The glory of the "
                        "sun's first morning rays fills him with power.  He deeply "
                        "distrusts unnatural creatures and sorceries, and any interference "
                        "with the natural progression of life in the Domain.",
                SEX_MALE,
                100,
                DEITY_TYPE_NATURE,
                DEITY_PERSONALITY_PROUD,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0, -1,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0, -1,  0,  0,  1,  0,  0,  0,  0, -1,  0, -1, -1,  1,  0,  0,  1,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0, -1, -1,  1, -1,  0, -1, -1,  1,  0 }},

        { "Gorthrond",
                "the Warmonger",
                "Lustful of war, Gorthrond is a masterful general and strategist.  To lead "
                        "armies into battle and obliterate his opponents is his strongest "
                        "desire.  Cool-headed and calculating, Gorthrond is a proud and "
                        "honourable warrior.",
                SEX_MALE,
                -150,
                DEITY_TYPE_WARLIKE,
                DEITY_PERSONALITY_PROUD,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0, -1,  1,  0, -1,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  1,  1,  0, -1, -1,  1,  1,  0,  0, -1, -1,  1,  1,  1,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Minerva",
                "the Shieldmaiden",
                "Young and beautiful, yet stern and terrible in anger, the Shieldmaiden "
                        "Minerva leads her righteous soldiers in the constant battle against "
                        "the evil hordes.  Her servants always seek to avoid conflict where "
                        "possible, but will attack furiously and fearlessly when confronted, "
                        "knowing that she watches and protects them.",
                SEX_FEMALE,
                900,
                DEITY_TYPE_WARLIKE,
                DEITY_PERSONALITY_PROTECTIVE,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0, -1,  1,  0,  0,  1,  1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  1,  1,  0, -1,  0,  0,  1,  0,  0, -1, -1,  1,  1,  1,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Xuul",
                "the Tyrant",
                "Xuul covets destruction and carnage.  Death, pain and suffering are a "
                        "delight to him, and he thirsts for the hot blood of the slain.  "
                        "No-one will be spared from Xuul's steel as he delivers terror, "
                        "despair and death to the innocent and guilty alike.",
                SEX_MALE,
                -900,
                DEITY_TYPE_WARLIKE,
                DEITY_PERSONALITY_CRUEL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  1,  0,  0,  1, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  1, -1,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0 }},

        { "Mira",
                "the White Goddess",
                "The night is Mira's cloak, the night's creatures her pets.  The White "
                        "Goddess weaves her subtle and devious enchantments as the "
                        "day-dwellers slumber in inky darkness.  Her followers live "
                        "sunken in the night, bathed by the gentle white heat of the moon, "
                        "guided by her soundless whispers.",
                SEX_FEMALE,
                -100,
                DEITY_TYPE_NATURE,
                DEITY_PERSONALITY_MYSTERIOUS,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  1,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0, -1,  0, -1,  0,  0,  0,  0,  0,  0, -1, -1,  0,  0, -1,  1,  0, -1,  0,  1,  0,  1}},

        { "Death",
                "Reaper of Souls",
                "Eternal, insubstantial, treading the shadows between the living and the "
                        "perished, Death waits to collect the souls of the unwary.  With "
                        "its mighty scythe, it reaps the dead, knowing their minds and "
                        "hearts, then judging their fate.",
                SEX_NEUTRAL,
                -300,
                DEITY_TYPE_DEATH,
                DEITY_PERSONALITY_JUDGEMENTAL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  0,  0, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  1,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Azazel",
                "Prince of Hell",
                "Stalker of the Wasteland, Overlord of Hell, Lord of the Flies, Prince of "
                        "Darkness; Azazel presides over the Abyss upon a throne of skulls "
                        "and fire.  The suffering of men, the torture and obliteration of "
                        "living things, misery, fear, loathing and terror fill his evil "
                        "mind and fuel his hideous body.",
                SEX_MALE,
                -1000,
                DEITY_TYPE_DEATH,
                DEITY_PERSONALITY_CRUEL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0, -1,  0,  0,  0,  0,  0, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0, -1,  0,  0,  0,  0, -1,  1,  1,  1,  1,  0,  0, -1, -1,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0 }},

        { "Swyft",
                "Prince of Thieves",
                "A master thief and scurrilous rogue, Swyft is the patron of thieves, "
                        "bandits, gamblers and fools.  He delights in mischief, "
                        "trickery and theft.  Devious, cunning and sneaky, Swyft ensures "
                        "that nothing stays safe for long.",
                SEX_MALE,
                -200,
                DEITY_TYPE_CHAOTIC,
                DEITY_PERSONALITY_MISCHIEVIOUS,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0, -1,  1,  0,  0,  0,  1, -1,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0, -1,  0,  1,  1,  1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Nystis",
                "the Enlightened",
                "Through deep meditation, mental discipline, contemplation and reflection, "
                        "Nystis perceives the inner workings of the universe.  He seeks "
                        "harmony and peace amongst the creatures of the Domain, so that "
                        "they may be attuned to this cosmic mechanism.  Nystis ignores what "
                        "form followers take or how they have behaved in the past, hoping "
                        "to enlighten them and bring them happiness.",
                SEX_MALE,
                700,
                DEITY_TYPE_PEACEFUL,
                DEITY_PERSONALITY_HELPFUL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Tractacus",
                "God of Wine and Music",
                "Musicians, actors, acrobats, dancers and drinkers worship the divine "
                        "satyr Tractacus, who revels all night long and sleeps "
                        "through the day.  He likes to drink and dance, play tricks "
                        "and cast meddling enchantments on the unsuspecting.",
                SEX_MALE,
                500,
                DEITY_TYPE_PEACEFUL,
                DEITY_PERSONALITY_MISCHIEVIOUS,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  0,  1,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0, -1,  0, -1,  0,  0,  0,  0, -1,  0,  0,  0, -1, -1,  0,  0,  1,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  1,  1,  1, -1,  0, -1,  0,  0,  1, -1,  1,  0,  0, -1,  0,  0 }},

        { "Brutus",
                "the Watcher",
                "The Watcher sees and knows all, surveying the Domain and its passage "
                        "through space and time.  From afar he observes, rarely "
                        "interfering with mortals as they pass their brief time on "
                        "Earth.",
                SEX_MALE,
                0,
                DEITY_TYPE_LAWFUL,
                DEITY_PERSONALITY_MYSTERIOUS,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }},

        { "Morfus",
                "the Sorcerer",
                "A mortal wizard of great learning and power, Morfus became divine after "
                        "gleaning the secrets of eternal life while experimenting with alchemy "
                        "and other sciences.  Now he roams the magical planes, always "
                        "seeking new lore and power, and protecting other magic-users.",
                SEX_MALE,
                200,
                DEITY_TYPE_LAWFUL,
                DEITY_PERSONALITY_HELPFUL,
                0,
                /* Mag Cle Thi War Psi Shi Bra Ran Smi */
                  { 1,  1,  0, -1,  1,  0, -1,  0,  0 },
                /* --- Nec Wlk Tem Dru Nin Bou Thg Kni Inf Wit Wer Vam Mon Mar Brb Brd Eng Alc */
                  { 0,  1,  1,  1,  1,  0,  0, -1,  0,  1,  1,  0,  0,  0, -1, -1,  1,  0,  0 },
                /* --- Hum Elf Hef Orc Gia Sat Ogr Gob Hdr Hfl Dwa Cen Dro Trl Ala Hob Yti Fae Sah Tfl Jtn Gen Ill Grg Dgr*/
                  { 0,  1,  1,  0, -1, -1,  0, -1,  0,  1,  0,  0,  0,  1, -1, -1,  0,  0,  1, -1,  0, -1,  1,  0,  -1,  0 }}
};


struct pantheon_status pantheon_status_table;
bool deity_system_halted = FALSE;


/*
 *      Functions
 */


bool deities_active ()
{
        if (deity_system_halted)
                return FALSE;

        /*  Edit return value below to turn system on (TRUE) or off (FALSE)  */
        return TRUE;
}


bool allow_deity_command (CHAR_DATA *ch)
{
        if (IS_NPC(ch))
        {
                send_to_char("NPCs may not use deity commands.\n\r", ch);
                return FALSE;
        }

        if (deities_active())
                return TRUE;

        send_to_char("The deity system is currently inactive.\n\r", ch);
        return FALSE;
}


void log_deity (char *text)
{
        char buf [MAX_STRING_LENGTH];
        sprintf(buf, "<<Deity>> %s", text);
        log_string(buf);
}


bool init_pantheon()
{
        int     ruler, best, target, i, j;
        char    buf [MAX_STRING_LENGTH];

        if (!deities_active())
        {
                log_deity("init_pantheon: deity code is inactive");
                return FALSE;
        }

        /*  Must be at least 3 Deities defined  */

        if (NUMBER_DEITIES < 3)
        {
                log_deity("init_pantheon: less than 3 deities defined");
                return FALSE;
        }

        /*
         *  Okay: set up the initial state of the Pantheon.  This is done
         *  by choosing a dominant deity at random, setting the 'atmosphere'
         *  of the Pantheon to that deity's alignment, and then ranking the
         *  rest of the Gods accordingly.
         *
         *  This is done roughly, then passed to update_pantheon_rankings()
         *  to refine the pantheon heirarchy.
         */

        ruler = number_range (0, NUMBER_DEITIES - 1);

        pantheon_status_table.atmosphere = deity_info_table[ruler].alignment;
        pantheon_status_table.ranking[0] = ruler;

        sprintf (buf, "init_pantheon: %s is dominant (alignment = %d)",
                deity_info_table[ruler].name,
                deity_info_table[ruler].alignment);
        log_deity (buf);

        for (i = 1; i < NUMBER_DEITIES; i++)
        {
                if (i == pantheon_status_table.ranking[0])
                        pantheon_status_table.ranking[i] = 0;
                else
                        pantheon_status_table.ranking[i] = i;
        }

        /*  Sort by alignment  */

        target = deity_info_table[pantheon_status_table.ranking[0]].alignment;

        for (i = 1; i < NUMBER_DEITIES - 1; i++)
        {
                best = i;

                for (j = i + 1; j < NUMBER_DEITIES; j++)
                {
                        if (abs (target - deity_info_table
                            [pantheon_status_table.ranking[j]].alignment) <
                            abs (target - deity_info_table
                            [pantheon_status_table.ranking[best]].alignment))
                        {
                                best = j;
                        }
                }

                j = pantheon_status_table.ranking[best];
                pantheon_status_table.ranking[best] = pantheon_status_table.ranking[i];
                pantheon_status_table.ranking[i] = j;
        }

        /*  Give deities semi-random power  */

        for (i = 0; i < NUMBER_DEITIES; i++)
        {
                pantheon_status_table.power[i]
                        = number_range (DEITY_MAX_POWER * 0.85, DEITY_MAX_POWER)
                        * (NUMBER_DEITIES - i) / NUMBER_DEITIES;

                pantheon_status_table.average_rank[i] = (double) (i + 1);
                pantheon_status_table.total_power[i] = 0;
                pantheon_status_table.top_count[i] = 0;
        }

        pantheon_status_table.update_count = 0;
        return TRUE;
}


void do_pantheon (CHAR_DATA *ch, char *argument)
{
        /*
         *  List Pantheon and knowledge of standing with each deity, if known.
         *  If a specific deity is indicated, then provide some information.
         */

        int             best, i, j, deity;
        char            buf [MAX_STRING_LENGTH];
        char            tmp [MAX_STRING_LENGTH];
        char            arg [MAX_INPUT_LENGTH];
        int             ranked [NUMBER_DEITIES];
        int             list [100];
        bool            flag;

        const char *favour [] = {"{wunknown", "{Rstrong disfavour", "{Ydisfavour",
                        "{Gindifference", "{Cfavour", "{Bstrong favour"};

        const char *his_her [] = {"its", "his", "her"};

        const char *he_she [] = {"it", "he", "she"};

        const char *races [MAX_RACE] = {"", "Human", "Elvish", "Wild-Elf",
                        "Orcish", "Giant", "Satyr", "Ogre", "Goblin",
                        "Half-Dragon", "Halfling", "Dwarvish", "Centaur",
                        "Drow", "Troll", "Alaghi", "Hobgoblin", "Yuan-ti",
                        "Fae", "Sahuagin", "Tiefling", "Jotun", "Genasi",
                        "Illithid", "Grung", "Duergar"};

        const char *classes [MAX_CLASS + MAX_SUB_CLASS -1] = {"Mage", "Cleric",
                        "Thief", "Warrior", "Psionic", "Shapeshifter",
                        "Brawler", "Ranger", "Smithy", "Necromancer", "Warlock",
                        "Templar", "Druid", "Ninja", "Bounty Hunter", "Thug",
                        "Knight", "Infernalist", "Witch", "Werewolf", "Vampire",
                        "Monk", "Martial Artist", "Barbarian", "Bard", "Engineer",
                        "Runesmith"};

        if (!allow_deity_command (ch))
                return;

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                /*  Show pantheon.  Sort from highest favour to lowest.  */

                for (i = 0; i < NUMBER_DEITIES; i++)
                        ranked[i] = i;

                for (i = 0; i < NUMBER_DEITIES - 1; i++)
                {
                        best = i;

                        for (j = i + 1; j < NUMBER_DEITIES; j++)
                        {
                                if (ch->pcdata->deity_favour[ranked[j]]
                                    > ch->pcdata->deity_favour[ranked[best]])
                                        best = j;
                        }

                        j = ranked[i];
                        ranked[i] = ranked[best];
                        ranked[best] = j;
                }

                send_to_char ("The Pantheon of Gods                      Favour\n\r"
                              "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r", ch);

                for (i = 0; i < NUMBER_DEITIES; i++)
                {
                        sprintf(buf, "{W%-16s{x %-24s %s{x\n\r",
                                 deity_info_table[ranked[i]].name,
                                 deity_info_table[ranked[i]].title,
                                 favour[get_deity_favour(ch, ranked[i])]);
                        send_to_char(buf, ch);
                }

                if (ch->pcdata->deity_patron < 0
                    || ch->pcdata->deity_patron >= NUMBER_DEITIES)
                        strcpy (buf, "\n\rYou have chosen no deity as your patron.\n\r");
                else
                {
                        sprintf (buf, "\n\rYour patron deity is %s.\n\r",
                                deity_info_table[ch->pcdata->deity_patron].name);

                        if (player_knows_deity_stature (ch, ch->pcdata->deity_patron))
                        {
                                i = deity_info_table[ch->pcdata->deity_patron].sex;

                                if (i < 0 || i > 2)
                                {
                                        sprintf (buf, "do_pantheon: deity %d has bad sex: %d",
                                                ch->pcdata->deity_patron, i);
                                        log_deity (buf);
                                        i = 0;
                                }

                                switch (get_deity_stature (ch->pcdata->deity_patron))
                                {
                                    case DEITY_STATURE_POWERFUL:
                                        send_to_char (buf, ch);
                                        sprintf (buf, "You feel %s power is strong and close.\n\r",
                                                his_her[i]);
                                        break;

                                    case DEITY_STATURE_WEAK:
                                        send_to_char (buf, ch);
                                        sprintf (buf, "You feel %s power is distant and weak.\n\r",
                                                his_her[i]);
                                        break;
                                }
                        }
                }

                send_to_char (buf, ch);
                return;
        }

        /*  Print specific info about a deity  */

        deity = -1;

        for (i = 0 ; i < NUMBER_DEITIES; i++)
        {
                if (is_name (arg, deity_info_table[i].name))
                {
                        deity = i;
                        break;
                }
        }

        if (deity < 0)
        {
                send_to_char ("There is no deity by that name in the Pantheon.\n\r", ch);
                return;
        }

        sprintf (buf, "\n\r{W%s, %s.{x\n\r\n\r{w%s{x\n\r\n\r%s ",
                deity_info_table[deity].name,
                deity_info_table[deity].title,
                deity_info_table[deity].description,
                deity_info_table[deity].name);

        switch (deity_info_table[deity].type)
        {
            case DEITY_TYPE_PEACEFUL:
                strcat (buf, "is peaceful, abhorring conflict and bloodshed");
                break;
            case DEITY_TYPE_WARLIKE:
                strcat (buf, "is violent and aggressive, revelling in combat and killing");
                break;
            case DEITY_TYPE_DEATH:
                strcat (buf, "controls the destiny of mortals and the passage of their souls");
                break;
            case DEITY_TYPE_CHAOTIC:
                strcat (buf, "is unprincipled and chaotic, desiring upheaval in the universe");
                break;
            case DEITY_TYPE_LAWFUL:
                strcat (buf, "is principled and righteous");
                break;
            case DEITY_TYPE_NATURE:
                strcat (buf, "seeks harmony, balance and natural order");
                break;
            default:
                strcat (buf, "has unknown motives");
        };

        sprintf (tmp, ".  %s ", capitalize (he_she[deity_info_table[deity].sex]));
        strcat (buf, tmp);

        switch (deity_info_table[deity].personality)
        {
            case DEITY_PERSONALITY_HELPFUL:
                strcat (buf, "is gentle, helpful and kind");
                break;
            case DEITY_PERSONALITY_CRUEL:
                strcat (buf, "is malicious, cruel and uncaring");
                break;
            case DEITY_PERSONALITY_JUDGEMENTAL:
                strcat (buf, "is stern, harsh and unforgiving");
                break;
            case DEITY_PERSONALITY_PROTECTIVE:
                strcat (buf, "is extremely protective of followers");
                break;
            case DEITY_PERSONALITY_PROUD:
                strcat (buf, "is majestic, boastful and proud");
                break;
            case DEITY_PERSONALITY_MISCHIEVIOUS:
                strcat (buf, "is mischievious and fun-loving");
                break;
            default:
                strcat (buf, "is mysterious and secretive");
        };

        sprintf (tmp, ".  %s ", deity_info_table[deity].name);
        strcat (buf, tmp);

        if (deity_info_table[deity].alignment < -749)
                strcat (buf, "is diabolically evil");
        else if (deity_info_table[deity].alignment < -349)
                strcat (buf, "is evil natured");
        else if (deity_info_table[deity].alignment < 350)
                strcat (buf, "is neutral");
        else if (deity_info_table[deity].alignment < 750)
                strcat (buf, "is good natured");
        else
                strcat (buf, "is very good natured");

        strcat (buf, ".{x\n\r");

        /*  Favour  */

        i = 0;
        flag = FALSE;

        for (j = 1; j < MAX_RACE; j++)
        {
                if (deity_info_table[deity].race_favour[j] > 0)
                        list[i++] = j;
        }

        if (i)
        {
                flag = TRUE;
                sprintf (tmp, "\n\r%s favours the ", deity_info_table[deity].name);

                for (j = 0; j < i; j++)
                {
                        if (j == i - 1 && j)
                                strcat (tmp, " and ");
                        else if (j)
                                strcat (tmp, ", ");
                        strcat (tmp, races[list[j]]);
                }

                if (i == 1)
                        strcat (tmp, " race");
                else
                        strcat (tmp, " races");
                strcat (buf, tmp);
        }

        i = 0;

        for (j = 0; j < MAX_CLASS; j++)
        {
                if (deity_info_table[deity].class_favour[j] > 0)
                        list[i++] = j;
        }

        for (j = 1; j < MAX_SUB_CLASS; j++)
                if (deity_info_table[deity].sub_class_favour[j] > 0)
                        list[i++] = j+7;
        if (i)
        {
                if (!flag)
                        sprintf (tmp, "\n\r%s favours those of ",
                                deity_info_table[deity].name);
                else
                        strcpy (tmp, ", and those of ");
                flag = TRUE;
                for (j = 0; j < i; j++)
                {
                        if (j == i - 1 && j)
                                strcat (tmp, " and ");
                        else if (j)
                                strcat (tmp, ", ");
                        strcat (tmp, classes[list[j]]);
                }
                if (i == 1)
                        strcat (tmp, " class");
                else
                        strcat (tmp, " classes");
                strcat (buf, tmp);
        }

        if (flag)
                strcat (buf, ".\n\r");

        /*  Disfavour  */

        i = 0;
        flag = FALSE;
        for (j = 1; j < MAX_RACE; j++)
                if (deity_info_table[deity].race_favour[j] < 0)
                        list[i++] = j;
        if (i)
        {
                flag = TRUE;
                sprintf (tmp, "\n\r%s disfavours the ",
                        capitalize (he_she[deity_info_table[deity].sex]));
                for (j = 0; j < i; j++)
                {
                        if (j == i - 1 && j)
                                strcat (tmp, " and ");
                        else if (j)
                                strcat (tmp, ", ");
                        strcat (tmp, races[list[j]]);
                }
                if (i == 1)
                        strcat (tmp, " race");
                else
                        strcat (tmp, " races");
                strcat (buf, tmp);
        }

        i = 0;
        for (j = 0; j < MAX_CLASS; j++)
                if (deity_info_table[deity].class_favour[j] < 0)
                        list[i++] = j;
        for (j = 1; j < MAX_SUB_CLASS; j++)
                if (deity_info_table[deity].sub_class_favour[j] < 0)
                        list[i++] = j+7;
        if (i)
        {
                if (!flag)
                        sprintf (tmp, "\n\r%s disfavours those of ",
                                capitalize (he_she[deity_info_table[deity].sex]));
                else
                        strcpy (tmp, ", and those of ");
                flag = TRUE;
                for (j = 0; j < i; j++)
                {
                        if (j == i - 1 && j)
                                strcat (tmp, " and ");
                        else if (j)
                                strcat (tmp, ", ");
                        strcat (tmp, classes[list[j]]);
                }
                if (i == 1)
                        strcat (tmp, " class");
                else
                        strcat (tmp, " classes");
                strcat (buf, tmp);
        }

        if (flag)
                strcat (buf, ".\n\r");

        send_paragraph_to_char(buf, ch, 0);
}


void do_pantheoninfo (CHAR_DATA *ch, char *argument)
{
        /*
         *  Display qualities of the various deities.
         */

        char            buf [MAX_STRING_LENGTH];
        char            tmp [MAX_STRING_LENGTH];
        int             next;

        const char      *type [DEITY_NUMBER_TYPES] = { "Peaceful", "Warlike", "Death",
                                "Chaotic", "Lawful", "Nature" };
        const char      *personality [DEITY_NUMBER_PERSONALITIES] = { "Helpful", "Cruel",
                                "Judgemental", "Mysterious", "Protective", "Proud",
                                "Mischievous" };

        if (!authorized (ch, gsn_pantheoninfo))
                return;

        sprintf (buf, "\n\r[Pantheon information]  Deity system is %s.\n\r",
                deities_active() ? "ACTIVE" : "inactive");

        if (NUMBER_DEITIES < 1)
        {
                strcat (buf, "No deities have been defined.\n\r");
                send_to_char (buf, ch);
                return;
        }

        strcat (buf, "\n\r"
                "ID, Name and Title                  Align  Type         Personality   Flags\n\r"
                "---------------------------------------------------------------------------\n\r");
        send_to_char (buf, ch);

        for (next = 0; next < NUMBER_DEITIES; next++)
        {
                sprintf (tmp, "%2d %s, %s",
                        next,
                        deity_info_table[next].name,
                        deity_info_table[next].title);
                sprintf (buf, "%-35s %5d  %-12s %-12s %6d\n\r",
                        tmp,
                        deity_info_table[next].alignment,

                        (deity_info_table[next].type < 0
                            || deity_info_table[next].type >= DEITY_NUMBER_TYPES)
                                ? "(unknown)"
                                : type[deity_info_table[next].type],

                        (deity_info_table[next].personality < 0
                            || deity_info_table[next].personality
                            >= DEITY_NUMBER_PERSONALITIES)
                                ? "(unknown)"
                                : personality[deity_info_table[next].personality],

                        deity_info_table[next].extra_flags);

                send_to_char (buf, ch);
        }
}


void do_pantheonrank (CHAR_DATA *ch, char *argument)
{
        /*
         *  Display the current ranking of the Pantheon
         */

        char            buf [MAX_STRING_LENGTH];
        char            tmp [MAX_STRING_LENGTH];
        int             next;

        if (!authorized (ch, gsn_pantheoninfo))
                return;

        if (!deities_active())
        {
                send_to_char ("\n\rDeity system is inactive.\n\r", ch);
                return;
        }

        sprintf (buf, "\n\r[Pantheon ranking]\n\r"
                 "Pantheon timer: %d.  Pantheon atmosphere: %d.  Update count: %d.\n\r\n\r"
                 "Rank  Deity           Align  Power  Stature   Avg rank  Tot power  First\n\r"
                 "------------------------------------------------------------------------\n\r",
                 pantheon_status_table.timer,
                 pantheon_status_table.atmosphere,
                 pantheon_status_table.update_count);

        for (next = 0; next < NUMBER_DEITIES; next++)
        {
                sprintf (tmp, "%3d   %-15s %5d  %5d  ",
                         next+1,
                         deity_info_table[pantheon_status_table.ranking[next]].name,
                         deity_info_table[pantheon_status_table.ranking[next]].alignment,
                         pantheon_status_table.power[next]);
                strcat (buf, tmp);

                switch (get_deity_stature (pantheon_status_table.ranking[next]))
                {
                    default:
                        strcat (buf, "Invalid   ");
                        break;

                    case DEITY_STATURE_UNKNOWN:
                        strcat (buf, "Unknown   ");
                        break;

                    case DEITY_STATURE_WEAK:
                        strcat (buf, "Weak      ");
                        break;

                    case DEITY_STATURE_MODERATE:
                        strcat (buf, "Moderate  ");
                        break;

                    case DEITY_STATURE_POWERFUL:
                        strcat (buf, "Powerful  ");
                        break;
                };

                sprintf(tmp, " %6.2f %10ld   %4d\n\r",
                        pantheon_status_table.average_rank[next],
                        pantheon_status_table.total_power[next],
                        pantheon_status_table.top_count[pantheon_status_table.ranking[next]]);
                strcat(buf, tmp);
        }

        send_to_char (buf, ch);
}


void update_pantheon ()
{
        /*
         *  Adjust the ranking of the deities within the Pantheon.
         */

        char            buf [MAX_STRING_LENGTH];
        int             stature [NUMBER_DEITIES];
        int             adj, next;
        const int       step = DEITY_MAX_POWER / 10;

        if (!deities_active())
                return;

        /*  Remember initial stature of each deity  */

        for (next = 0; next < NUMBER_DEITIES; next++)
                stature[next] = get_deity_stature (next);

        /*
         *  Adjust the atmosphere of the Pantheon.  Change is mostly
         *  small, sometimes very large.
         */

        if (number_percent() < 95)
                adj = number_range (-50, 50);
        else
        {
                if ((pantheon_status_table.atmosphere < -750
                     || pantheon_status_table.atmosphere > 750)
                    && number_bits(1))
                        adj = pantheon_status_table.atmosphere * -2;

                else
                        adj = number_range (-500, 500);
        }

        pantheon_status_table.atmosphere += adj;
        pantheon_status_table.atmosphere
                = URANGE (-1100, pantheon_status_table.atmosphere, 1100);

        sprintf (buf, "update_pantheon: atmosphere moves %d to %d",
                 adj, pantheon_status_table.atmosphere);

        server_message (buf);

        /*  Adjust power of deities by alignment.  */

        for (next = 0; next < NUMBER_DEITIES; next++)
        {
                adj = step / 3
                        - ((double) abs ((double) pantheon_status_table.atmosphere - (double)
                                         deity_info_table[pantheon_status_table.ranking[next]].alignment)
                           / (double) 900 * (double) step );

                adj = URANGE(-step, adj, step);
                pantheon_status_table.power[next]
                        = URANGE(0, pantheon_status_table.power[next] + adj, DEITY_MAX_POWER);

                /*
                sprintf (log, "-> deity %d (%s) align adj %d",
                        next,
                        deity_info_table[pantheon_status_table.ranking[next]].name,
                        adj);

                sprintf (buf, " final %d", pantheon_status_table.power[next]);
                strcat (log, buf);
                log_deity (log);
                */
        }

        rank_pantheon();

        /*  Update pantheon statistics  */

        for (next = 0; next < NUMBER_DEITIES; next++)
        {
                pantheon_status_table.total_power[next]
                        += pantheon_status_table.power[next];

                pantheon_status_table.average_rank[next]
                        = (pantheon_status_table.average_rank[next]
                           * pantheon_status_table.update_count + next + 1)
                                / (pantheon_status_table.update_count + 1);
        }

        pantheon_status_table.update_count++;
        pantheon_status_table.top_count[pantheon_status_table.ranking[0]]++;

        /*  Report any change in deity stature to any followers  */

        for (next = 0; next < NUMBER_DEITIES; next++)
        {
                if (get_deity_stature(next) == DEITY_STATURE_UNKNOWN
                    || get_deity_stature(next) == stature[next])
                        continue;
                if (get_deity_stature(next) > stature[next])
                        report_to_followers (next,
                            "{CYou feel the strength of your patron deity increasing...{x\n\r");
                else
                        report_to_followers (next,
                            "{CYou sense the power of your patron deity fading...{x\n\r");
        }
}


void report_to_followers (int deity, char *message)
{
        /*
         *  Send a message to the followers of a deity.
         */

        char buf [MAX_STRING_LENGTH];
        DESCRIPTOR_DATA *d;

        if (!deities_active())
                return;

        if (!message)
        {
                log_deity ("report_to_followers: message is null");
                return;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "report_to_followers: bad deity id: %d", deity);
                log_deity (buf);
                return;
        }

        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected == CON_PLAYING
                    && d->character
                    && d->character->pcdata->deity_patron == deity)
                        send_to_char (message, d->character);
        }

        return;
}


void rank_pantheon ()
{
        /*
         *  Rank deities within the Pantheon by power.
         */

        int best, i, j;
        double k;

        if (!deities_active())
                return;

        for (i = 0; i < NUMBER_DEITIES-1; i++)
        {
                best = i;

                for (j = i + 1; j < NUMBER_DEITIES; j++)
                {
                        if (pantheon_status_table.power[j]
                            > pantheon_status_table.power[i])
                                best = j;
                }

                j = pantheon_status_table.ranking[best];
                pantheon_status_table.ranking[best] = pantheon_status_table.ranking[i];
                pantheon_status_table.ranking[i] = j;

                j = pantheon_status_table.power[best];
                pantheon_status_table.power[best] = pantheon_status_table.power[i];
                pantheon_status_table.power[i] = j;

                j = pantheon_status_table.total_power[best];
                pantheon_status_table.total_power[best] = pantheon_status_table.total_power[i];
                pantheon_status_table.total_power[i] = j;

                k = (double) pantheon_status_table.average_rank[best];
                pantheon_status_table.average_rank[best] = pantheon_status_table.average_rank[i];
                pantheon_status_table.average_rank[i] = k;
        }
}


int get_deity_stature (int deity)
{
        /*
         *  Calculate how powerful a deity is considered to be within
         *  the Pantheon.
         *
         *  Currently, just a function of how close a deity is from
         *  the top of the Pantheon ranking list.
         */

        char    buf [MAX_STRING_LENGTH];
        int     rank, i;
        double  j;

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "get_deity_stature: bad deity id: %d", deity);
                log_deity (buf);
                return -1;
        }

        rank = -1;

        for (i = 0; i < NUMBER_DEITIES; i++)
        {
                if (deity == pantheon_status_table.ranking[i])
                {
                        rank = i;
                        break;
                }
        }

        if (rank < 0)
                return -1;

        j = (double) (rank+1) / (double) NUMBER_DEITIES;

        if (j < 0.34)
                return DEITY_STATURE_POWERFUL;
        else if (j < 0.67)
                return DEITY_STATURE_MODERATE;
        else
                return DEITY_STATURE_WEAK;
}


int get_deity_favour (CHAR_DATA *ch, int deity)
{
        /*
         *  Determine the attitude of a Deity towards a player.
         */

        char    buf [MAX_STRING_LENGTH];
        int     base, ratio;

        if (IS_NPC (ch))
        {
                log_deity ("get_deity_favour: ch is NPC");
                return DEITY_FAVOUR_UNKNOWN;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "get_deity_favour: bad deity id: %d", deity);
                log_deity (buf);
                return DEITY_FAVOUR_UNKNOWN;
        }

        base = ch->pcdata->deity_favour[deity];

        if (base < 0)
                return DEITY_FAVOUR_UNKNOWN;
        else
        {
                ratio = 100 * base / DEITY_MAX_FAVOUR;

                if (ratio <= 25)
                        return DEITY_FAVOUR_STRONG_DISFAVOUR;
                else if (ratio < 45)
                        return DEITY_FAVOUR_DISFAVOUR;
                else if (ratio < 65)
                        return DEITY_FAVOUR_INDIFFERENCE;
                else if (ratio < 80)
                        return DEITY_FAVOUR_FAVOUR;
                else
                        return DEITY_FAVOUR_STRONG_FAVOUR;
        }
}


bool player_knows_deity_stature (CHAR_DATA *ch, int deity)
{
        /*
         *  Test whether player knows how powerful a deity is within
         *  the Pantheon.
         */

        char    buf [MAX_STRING_LENGTH];

        if (IS_NPC (ch))
        {
                log_deity ("player_knows_deity_stature: ch is NPC");
                return FALSE;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "player_knows_deity_stature: bad deity id: %d", deity);
                log_deity (buf);
                return FALSE;
        }

        /*  Pretty bare at the moment: return true if deity is
            character's patron  */

        if (ch->pcdata->deity_patron == deity)
                return TRUE;

        return FALSE;
}


void update_player_favour (CHAR_DATA *ch, int deity)
{
        /*
         * Calculate the favour by a particular deity
         */

        char    buf [MAX_STRING_LENGTH];
        int     favour, step, tmp;

        if (IS_NPC (ch))
        {
                log_deity ("update_player_favour: ch is NPC");
                return;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "update_player_favour: bad deity id: %d", deity);
                log_deity (buf);
                return;
        }

        step = DEITY_MAX_FAVOUR / 10;
        favour = DEITY_MAX_FAVOUR / 2;


        /*
         *  Adjustments made for those with patrons
         */

        if (ch->pcdata->deity_patron >= 0)
        {
                /*  Another step added below in switch; 1.5 steps for patron  */
                if (ch->pcdata->deity_patron == deity)
                {
                        favour += step / 2;

                        if (ch->level == LEVEL_HERO)
                                favour += step;
                }

                /*
                 *  Relative deity/patron types;
                 *  Type-specific favour adjustments
                 */
                switch (deity_info_table[deity].type)
                {
                    case DEITY_TYPE_PEACEFUL:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_PEACEFUL)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_WARLIKE)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_PEACEFUL];

                        if (tmp > 400)
                                favour -= 2 * step;
                        else if (tmp > 200)
                                favour -= 1.5 * step;
                        else if (tmp > 100)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;

                    case DEITY_TYPE_WARLIKE:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_WARLIKE)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_PEACEFUL)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_WARLIKE];

                        if (tmp > 359)
                                favour -= 3 * step;
                        else if (tmp > 179)
                                favour -= 2 * step;
                        else if (tmp > 59)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;

                    case DEITY_TYPE_DEATH:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_DEATH)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_NATURE)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_DEATH];

                        if (tmp > 359)
                                favour -= 3 * step;
                        else if (tmp > 179)
                                favour -= 2 * step;
                        else if (tmp > 59)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;

                    case DEITY_TYPE_CHAOTIC:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_CHAOTIC)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_LAWFUL)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_CHAOTIC];

                        if (tmp > 359)
                                favour -= 3 * step;
                        else if (tmp > 179)
                                favour -= 2 * step;
                        else if (tmp > 119)
                                favour -= 3 / 2 * step;
                        else if (tmp > 59)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;

                    case DEITY_TYPE_LAWFUL:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_LAWFUL)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_CHAOTIC)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_LAWFUL];

                        if (tmp > 400)
                                favour -= 2 * step;
                        else if (tmp > 200)
                                favour -= 1.5 * step;
                        else if (tmp > 100)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;

                    case DEITY_TYPE_NATURE:
                        if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_NATURE)
                                favour += step;
                        else if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_DEATH)
                                favour -= step;

                        /* timer range is 0 - 480 */
                        tmp = ch->pcdata->deity_type_timer[DEITY_TYPE_NATURE];

                        if (tmp > 400)
                                favour -= 2 * step;
                        else if (tmp > 200)
                                favour -= 1.5 * step;
                        else if (tmp > 100)
                                favour -= step;
                        else if (tmp > 19)
                                favour -= step / 2;

                        break;
                }

                /*
                 *  Personality timers
                 */
                tmp = ch->pcdata->deity_personality_timer[deity_info_table[deity].personality];

                if (tmp > 119)
                        favour -= 2 * step;
                else if (tmp > 89)
                        favour -= 1.5 * step;
                else if (tmp > 29)
                        favour -= step;
                else if (tmp > 9)
                        favour -= step / 2;
        }


        /*  Adjust by alignment  */

        if (deity_info_table[deity].alignment > 349)
        {
                if (ch->alignment > 349)
                        favour += step;
                else if (ch->alignment > -350)
                        favour += -step;
                else
                        favour += -2 * step;
        }
        else if (deity_info_table[deity].alignment < -349)
        {
                if (ch->alignment < -349)
                        favour += step;
                else if (ch->alignment < 350)
                        favour += -step;
                else
                        favour += -2 * step;
        }
        else
        {
                if (ch->alignment < -349 || ch->alignment > 349)
                        favour += -step;
                else
                        favour += step;
        }


        /*  Adjust by class and race  */

        favour += step / 2 * get_class_favour (ch, deity);
        favour += step / 2 * get_race_favour (ch, deity);


        /*  Check for attempted release  */

        if (ch->pcdata->deity_patron == deity
            && IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_ATTEMPT))
        {
                favour -= 2 * step;
        }


        /*  Check whoever offended by releases  */

        if ((deity_info_table[deity].alignment < -349
             && IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_LEFT_EVIL))

            || ((deity_info_table[deity].alignment > -350
                && deity_info_table[deity].alignment < 350)
                && IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_LEFT_NEUTRAL))

            || (deity_info_table[deity].alignment > 349
                && IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_LEFT_GOOD)))
        {
                favour -= step;
        }


        /*  Tidy up  */

        favour = URANGE(0, favour, DEITY_MAX_FAVOUR);
        ch->pcdata->deity_favour[deity] = favour;
}


void do_pray (CHAR_DATA *ch, char *argument)
{
        /*
         *  Pray for patronage, release or for divine assistance!
         *  This function is a bit of a monster.
         */

        char buf [MAX_STRING_LENGTH];
        char prayer [MAX_INPUT_LENGTH];
        int next, deity, patron, prayer_id;

        const char* prayer_info [DEITY_NUMBER_PRAYERS] [2] =
        {
                { "Destruction","Call upon your patron's furious wrath"         },
                { "Combat",     "Enhance your fighting skills"                  },
                { "Locate",     "Divine somebody's location"                    },
                { "Reveal",     "Divine somebody's condition and power"         },
                { "Weaken",     "Cripple an opponent"                           },
                { "Protection", "Ask your patron for protection"                },
                { "Rescue",     "Pray to be transported to safety"              },
                { "Healing",    "Restore your health and well-being"            },
                { "Plague",     "Unleash a terrible plague upon the Infidels"   },
                { "Death",      "Bring death to the living"                     },
                { "Chaos",      "Release the forces of chaos and upheaval"      },
                { "Transport",  "Pray for transportation to somebody"           },
                { "Passage",    "Move unhindered throughout the world"          },
        };

        const int prayer_menu [DEITY_NUMBER_TYPES] [DEITY_PRAYERS_PER_TYPE] =
        {
                /*  Peaceful  */
                {
                        DEITY_PRAYER_HEALING,
                        DEITY_PRAYER_RESCUE,
                        DEITY_PRAYER_PROTECTION,
                        DEITY_PRAYER_PASSAGE
                },
                /*  Warlike  */
                {
                        DEITY_PRAYER_DESTRUCTION,
                        DEITY_PRAYER_REVEAL,
                        DEITY_PRAYER_COMBAT,
                        DEITY_PRAYER_WEAKEN
                },
                /*  Death  */
                {
                        DEITY_PRAYER_REVEAL,
                        DEITY_PRAYER_LOCATE,
                        DEITY_PRAYER_DEATH,
                        DEITY_PRAYER_PLAGUE
                },
                /*  Chaotic  */
                {
                        DEITY_PRAYER_DESTRUCTION,
                        DEITY_PRAYER_LOCATE,
                        DEITY_PRAYER_TRANSPORT,
                        DEITY_PRAYER_CHAOS
                },
                /*  Lawful  */
                {
                        DEITY_PRAYER_HEALING,
                        DEITY_PRAYER_RESCUE,
                        DEITY_PRAYER_TRANSPORT,
                        DEITY_PRAYER_PROTECTION
                },
                /*  Nature  */
                {
                        DEITY_PRAYER_HEALING,
                        DEITY_PRAYER_DESTRUCTION,
                        DEITY_PRAYER_PASSAGE,
                        DEITY_PRAYER_TRANSPORT
                }
        };

        if (!allow_deity_command(ch))
                return;

        /*
         * Temporary
         */
        if (IS_SET(ch->pcdata->deity_flags, DEITY_PFLAG_GRANT_ACCESS))
        {
                send_to_char("Okay Tester: all of your deity statistics have been reset.\n\r"
                             "Your favour will be automatically updated shortly.\n\r"
                             "You may pray for patronage whenever (and if) you want.\n\r"
                             "Thanks for helping out!  Shade and Gez\n\r", ch);

                ch->pcdata->deity_patron = -1;
                ch->pcdata->deity_timer = DEITY_DEFAULT_PLAYER_TIMER;
                ch->pcdata->deity_flags = 0;

                for (next = 0; next < DEITY_NUMBER_TYPES; next++)
                        ch->pcdata->deity_type_timer[next] = 0;

                for (next = 0; next < DEITY_NUMBER_PERSONALITIES; next++)
                        ch->pcdata->deity_personality_timer[next] = 0;

                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                send_to_char("You may not pray inside the Arena.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (ch->level < 15)
        {
                send_to_char ("You have no patron.  You may not seek a patron until you "
                              "reach level 15.\n\r", ch);
                return;
        }

        patron = ch->pcdata->deity_patron;


        /*
         *  No argument
         */

        if (argument[0] == '\0')
        {
                if (patron < 0 || patron >= NUMBER_DEITIES)
                {
                        send_to_char ("You have no patron.  To seek patronage, enter "
                                      "'PRAY PATRONAGE <deity>'.\n\r", ch);
                        return;
                }

                /*
                 *  Send player a list of prayers if any are available.
                 */

                sprintf (buf, "%s grants the following prayers:\n\r\n\r",
                         deity_info_table[patron].name);
                send_to_char (buf, ch);

                deity = deity_info_table[patron].type;

                for (next = 0; next < DEITY_PRAYERS_PER_TYPE; next++)
                {
                        if (prayer_menu[deity][next] < 0
                            || prayer_menu[deity][next] >= DEITY_NUMBER_PRAYERS)
                                continue;

                        sprintf (buf, "{W%-20s{x%s\n\r",
                                 prayer_info[prayer_menu[deity][next]][0],
                                 prayer_info[prayer_menu[deity][next]][1]);
                        send_to_char (buf, ch);
                }

                send_to_char("\n\rEnter 'HELP PRAYER <prayer name>' for further information.\n\r", ch);
                return;
        }

        argument = one_argument(argument, prayer);


        /*
         *  Patronage, release from patronage
         */

        if (is_name(prayer, "patronage"))
        {
                if (argument[0] == '\0')
                {
                        send_to_char("Which deity do you wish to be your patron?\n\r", ch);
                        return;
                }

                if (is_name(argument, "release"))
                {
                        pray_for_release(ch);
                        return;
                }

                for (next = 0; next < NUMBER_DEITIES; next++)
                {
                        if (is_name(argument, deity_info_table[next].name))
                        {
                                pray_for_patronage(ch, next);
                                return;
                        }
                }

                send_to_char ("There is no deity by that name.\n\r", ch);
                return;
        }

        if (patron < 0 || patron >= NUMBER_DEITIES)
        {
                send_to_char ("You have no patron.  To seek patronage, enter "
                              "'PRAY PATRONAGE <<deity>'.\n\r", ch);
                return;
        }


        /*
         *  Pray for assistance
         */

        prayer_id = -1;

        for (next = 0; next < DEITY_PRAYERS_PER_TYPE; next++)
        {
                if (prayer_menu[deity_info_table[patron].type][next] < 0)
                        continue;

                if (is_name(prayer, (char*) prayer_info
                             [prayer_menu[deity_info_table[patron].type][next]][0]))
                {
                        prayer_id = prayer_menu[deity_info_table[patron].type][next];
                        break;
                }
        }

        if (prayer_id < 0)
        {
                send_to_char ("Your patron doesn't offer that prayer.\n\r", ch);
                return;
        }

        send_to_char ("You pray to your patron and beg for divine assistance!\n\r", ch);
        act ("$c calls upon the power of $s patron deity!", ch, NULL, NULL, TO_ROOM);

        if (ch->pcdata->deity_timer < 1)
        {
                ch->pcdata->deity_timer = number_range(DEITY_PRAYER_TIME_PENALTY * 3 / 4,
                                                       DEITY_PRAYER_TIME_PENALTY * 5 / 4);
        }
        else
                return;  /* Timer needs to run out before prayer considered */


        if (get_deity_favour(ch, patron) < DEITY_FAVOUR_FAVOUR)
        {
                /*  Cruel/evil Deities might punish followers here  */
                /*  Divine curse maybe */
                ch->pcdata->deity_timer /= 2;
                return;
        }

        sprintf (buf, "prayer granted to %s: %s (favour %d, arg '%s')",
                 ch->name,
                 prayer_info[prayer_id][0],
                 ch->pcdata->deity_favour[patron],
                 argument);
        log_deity (buf);
        server_message (buf);

        switch (prayer_id)
        {
            case DEITY_PRAYER_DESTRUCTION:
                prayer_destruction (ch);
                break;

            case DEITY_PRAYER_COMBAT:
                prayer_combat (ch);
                break;

            case DEITY_PRAYER_LOCATE:
                prayer_locate (ch, argument);
                break;

            case DEITY_PRAYER_REVEAL:
                prayer_reveal (ch, argument);
                break;

            case DEITY_PRAYER_WEAKEN:
                prayer_weaken (ch, argument);
                break;

            case DEITY_PRAYER_PROTECTION:
                prayer_protection (ch);
                break;

            case DEITY_PRAYER_RESCUE:
                prayer_rescue (ch);
                break;

            case DEITY_PRAYER_HEALING:
                prayer_healing (ch);
                break;

            case DEITY_PRAYER_PLAGUE:
                prayer_plague (ch);
                break;

            case DEITY_PRAYER_CHAOS:
                prayer_chaos (ch);
                break;

            case DEITY_PRAYER_TRANSPORT:
                prayer_transport (ch, argument);
                break;

            case DEITY_PRAYER_PASSAGE:
                prayer_passage (ch);
                break;

            case DEITY_PRAYER_DEATH:
                prayer_death (ch, argument);
                break;

            default:
                sprintf (buf, "do_pray: unknown prayer: %d", prayer_id);
                log_deity (buf);
        }

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
}


void pray_for_release (CHAR_DATA* ch)
{
        int     patron, next;
        char    buf [MAX_STRING_LENGTH];

        if (!allow_deity_command (ch))
                return;

        /*
         *  Player prays for release from service
         */

        patron = ch->pcdata->deity_patron;

        if (patron < 0 || patron >= NUMBER_DEITIES)
        {
                send_to_char ("You have no patron.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char ("You may not pray for release while fighting.\n\r", ch);
                return;
        }

        sprintf (buf, "You beg your patron {W%s{x to release you from service.\n\r",
                 deity_info_table[patron].name);
        send_to_char (buf, ch);

        if (!IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_WARNING))
        {
                send_paragraph_to_char ("\n\r{RWarning!{x  You are about to abandon "
                        "your patron, who may or may not release you.  If you are not "
                        "released, you will lose favour with your patron.  If you are "
                        "released, you will lose favour with your patron and possibly "
                        "your patron's allies within the Pantheon!\n\r\n\rRepeat "
                        "this command to ask for release.\n\r", ch, 0);
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_WARNING);
                return;
        }

        /*
         *  Decide whether to release follower.
         *
         *  Evil, cruel and proud deities will not release players.
         *  Good deities will release players in strong favour without penalty,
         *  although release attempt penalty still applies.
         */

        if (deity_info_table[patron].alignment < -349
            || deity_info_table[patron].personality == DEITY_PERSONALITY_CRUEL)
        {
                sprintf (buf, "\n\r%s's rage fills your mind: {CSlave! You are still "
                         "my plaything...  You will never be free!{x\n\r",
                         deity_info_table[patron].name);
                send_paragraph_to_char(buf, ch, 0);
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_ATTEMPT);
                return;
        }

        if (deity_info_table[patron].personality == DEITY_PERSONALITY_PROUD)
        {
                sprintf (buf, "\n\r%s's voice rings firmly in your mind: {CYou may "
                         "not desert me: you will worship me to the end!{x\n\r",
                         deity_info_table[patron].name);
                send_paragraph_to_char(buf, ch, 0);
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_ATTEMPT);
                return;
        }

        if (deity_info_table[patron].alignment > 349)
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_LEFT_GOOD);
        else
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_LEFT_NEUTRAL);

        send_to_char ("{WYour patron releases you from service!\n\r{x", ch);

        ch->pcdata->deity_patron = -1;
        REMOVE_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_RELEASE_ATTEMPT);

        for (next = 0; next < NUMBER_DEITIES; next++)
                update_player_favour (ch, patron);

        sprintf (buf, "Deity %d (%s) has released %s from service",
                 patron,
                 deity_info_table[patron].name,
                 ch->name);
        log_deity (buf);
        server_message (buf);
}


void pray_for_patronage (CHAR_DATA* ch, int deity)
{
        char            buf [MAX_STRING_LENGTH];
        int             patron;
        const char      *his_her [] = {"its", "his", "her"};

        if (!allow_deity_command (ch))
                return;

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "Invalid deity passed to pray_for_patronage: %d", deity);
                log_deity (buf);
                return;
        }


        /*
         *  Player seeks a patron
         */

        patron = ch->pcdata->deity_patron;

        if (ch->fighting)
        {
                send_to_char ("You cannot pray for patronage while fighting.\n\r", ch);
                return;
        }

        if (deity == patron)
        {
                sprintf (buf, "%s is already your patron.\n\r",
                         deity_info_table[patron].name);
                send_to_char (buf, ch);
                return;
        }

        if (patron > -1 && patron < NUMBER_DEITIES)
        {
                send_to_char ("You already worship another deity!\n\r", ch);
                return;
        }

        /*
         *  Send player a few warnings
         */

        sprintf (buf, "You beg the mighty God {W%s{x to accept you as %s servant...\n\r",
                 deity_info_table[deity].name,
                 his_her[deity_info_table[deity].sex]);
        send_to_char (buf, ch);

        if (!IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_PATRON_WARNING_1))
        {
                send_paragraph_to_char ("\n\r{RWarning!{x  You are about to select a "
                      "patron deity!  Once you have chosen a patron, you may not "
                      "choose another under normal circumstances, and some gods "
                      "may not release you from service.\n\r\n\rAlso {Rbeware{x "
                      "that by choosing a patron, you are subject to the temper "
                      "of other deities within the Pantheon, many of whom will "
                      "not think favourably of you.  Good things will happen to "
                      "you, and bad things too!\n\r\n\rMake sure you have read "
                      "the online help entries for DEITIES, PANTHEON and PRAY.  "
                      "Also check the FAQ for any information.\n\r\n\rRepeat "
                      "this command to select your patron.\n\r", ch, 0);
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_PATRON_WARNING_1);
                return;
        }

        if (!IS_SET (ch->pcdata->deity_flags, DEITY_PFLAG_PATRON_WARNING_2))
        {
                send_paragraph_to_char ("\n\r{RFinal Warning!{x  The deity system is "
                      "currently experimental.  If you continue, you accept that "
                      "TERRIBLE things may happen to you!\n\r\n\rRepeat this "
                      "command one last time to select a patron.\n\r", ch, 0);
                SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_PATRON_WARNING_2);
                return;
        }

        /*
         *  Alright: player thinks he knows what he's in for!
         *  See if the deity is interested next.
         */

        if (ch->pcdata->deity_favour[deity] < DEITY_MAX_FAVOUR / 2)
        {
                sprintf (buf, "\n\r%s speaks to your mind: {C",
                         deity_info_table[deity].name);

                switch (deity_info_table[deity].personality)
                {
                    case DEITY_PERSONALITY_HELPFUL:
                    case DEITY_PERSONALITY_PROUD:
                        strcat (buf, "I don't consider you true enough of heart to follow me.");
                        break;

                    case DEITY_PERSONALITY_CRUEL:
                        strcat (buf, "Insect! You will burn in the pits of Hell before you march with me!");
                        break;

                    case DEITY_PERSONALITY_JUDGEMENTAL:
                    case DEITY_PERSONALITY_PROTECTIVE:
                        strcat (buf, "You must improve before you may follow my banner.");
                        break;

                    case DEITY_PERSONALITY_MISCHIEVIOUS:
                        strcat (buf, "Worship me would you?  Find some other fool to follow!");
                        break;

                    default:
                        strcat (buf, "Do you really think you are ready?  Return when you know the answer.");
                }

                strcat (buf, "{x\n\r");
                send_paragraph_to_char(buf, ch, 0);
                return;
        }

        /*
         *  Set the player's patron and inform.
         */

        ch->pcdata->deity_patron = deity;
        SET_BIT (ch->pcdata->deity_flags, DEITY_PFLAG_PATRON_CHOSEN);
        strcpy (buf, "\n\rA voice fills your mind: {C");

        switch (deity_info_table[deity].personality)
        {
            case DEITY_PERSONALITY_HELPFUL:
            case DEITY_PERSONALITY_PROTECTIVE:
            case DEITY_PERSONALITY_MISCHIEVIOUS:
                strcat (buf, "Welcome! I shall guide you as best I can.");
                break;

            case DEITY_PERSONALITY_CRUEL:
                strcat (buf, "Poor fool... fail me and you shall suffer.");
                break;

            case DEITY_PERSONALITY_JUDGEMENTAL:
            case DEITY_PERSONALITY_PROUD:
                strcat (buf, "Serve me well, and I shall protect you.");
                break;

            default:
                strcat (buf, "Heed my warning: tread carefully along this path.");
        }

        strcat (buf, "{x\n\r");
        send_paragraph_to_char(buf, ch, 0);

        update_player_favour (ch, deity);

        sprintf (buf, "\n\r%s has accepted you as %s follower!\n\r",
                 deity_info_table[deity].name,
                 his_her[deity_info_table[deity].sex]);
        send_to_char (buf, ch);

        sprintf (buf, "Deity %d (%s) has accepted %s as a follower",
                 deity,
                 deity_info_table[deity].name,
                 ch->name);
        log_deity (buf);
        server_message (buf);
}


int get_race_favour (CHAR_DATA* ch, int deity)
{
        /*
         *  Return -1 if race is not favoured, 1 if favoured, else 0.
         */

        char buf [MAX_STRING_LENGTH];

        if (!deities_active())
                return 0;

        if (IS_NPC(ch))
        {
                log_deity ("get_race_favour: 'ch' is an NPC");
                return 0;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "get_race_favour: invalid deity: %d",
                        deity);
                log_deity (buf);
                return 0;
        }

        return deity_info_table[deity].race_favour[ch->race];
}


int get_class_favour (CHAR_DATA* ch, int deity)
{
        /*
         *  Return -1 if class is not favoured, 1 if favoured, else 0.
         */

        char buf [MAX_STRING_LENGTH];

        if (!deities_active())
                return 0;

        if (IS_NPC(ch))
        {
                log_deity ("get_class_favour: 'ch' is an NPC");
                return 0;
        }

        if (deity < 0 || deity >= NUMBER_DEITIES)
        {
                sprintf (buf, "get_class_favour: invalid deity: %d",
                        deity);
                log_deity (buf);
                return 0;
        }

        if (ch->sub_class)
                return deity_info_table[deity].sub_class_favour[ch->sub_class];
        else
                return deity_info_table[deity].class_favour[ch->class];
}


void deity_update ()
{
        DESCRIPTOR_DATA *d;
        int i;

        if (!deities_active())
                return;

        /*
         *  Update pantheon and players
         */

        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected == CON_PLAYING
                    && d->character
                    && d->character->pcdata)
                {
                        /*
                         *  Those with patrons have more sensitive favour
                         */
                        if (d->character->pcdata->deity_patron >= 0)
                        {
                                /*
                                 * Prayer timer decreases when in favour, increases
                                 * when out of favour
                                 */
                                switch (get_deity_favour(d->character, d->character->pcdata->deity_patron))
                                {
                                    case DEITY_FAVOUR_STRONG_FAVOUR:
                                        d->character->pcdata->deity_timer -= 2;
                                        break;

                                    case DEITY_FAVOUR_FAVOUR:
                                        d->character->pcdata->deity_timer--;
                                        break;

                                    case DEITY_FAVOUR_DISFAVOUR:
                                        d->character->pcdata->deity_timer++;
                                        break;

                                    case DEITY_FAVOUR_STRONG_DISFAVOUR:
                                        d->character->pcdata->deity_timer += 2;
                                        break;
                                }

                                d->character->pcdata->deity_timer
                                        = URANGE(0, d->character->pcdata->deity_timer, DEITY_TIMER_MAX);

                                /*
                                 * Type specific effects;
                                 * Set a number that is used in update_player_favour for types
                                 */
                                if (d->character->alignment < -349)
                                        SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_NATURE, 15);

                                switch(d->character->in_room->sector_type)
                                {
                                    case SECT_FIELD:
                                    case SECT_FOREST:
                                    case SECT_HILLS:
                                    case SECT_MOUNTAIN:
                                    case SECT_SWAMP:
                                        SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_NATURE, -2);
                                        break;

                                    case SECT_INSIDE:
                                    case SECT_CITY:
                                        SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_NATURE, 1);
                                        break;
                                }

                                SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_DEATH, 1);
                                SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_WARLIKE, 1);
                                SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_CHAOTIC, 1);
                                SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_PEACEFUL, -1);
                                SET_DEITY_TYPE_TIMER(d->character, DEITY_TYPE_LAWFUL, -1);

                                for (i = 0; i < DEITY_NUMBER_PERSONALITIES; i++)
                                        SET_DEITY_PERSONALITY_TIMER(d->character, i, -1);
                        }

                        /*
                         *  Update favour for everyone
                         */
                        for (i = 0; i < NUMBER_DEITIES; i++)
                                update_player_favour(d->character, i);
                }
        }

        if (--pantheon_status_table.timer < 1)
        {
                pantheon_status_table.timer = DEITY_PANTHEON_UPDATE_TIMER;
                update_pantheon();
        }
}


bool effect_is_prayer (AFFECT_DATA *af)
{
        /*
         *  Return true if effect is a divine prayer
         */

        if (af->type == gsn_prayer_combat
            || af->type == gsn_prayer_weaken
            || af->type == gsn_prayer_protection
            || af->type == gsn_prayer_passage
            || af->type == gsn_prayer_plague)
                return TRUE;

        return FALSE;
}



/*
 *  Prayers
 */

void prayer_destruction (CHAR_DATA *ch)
{
        bool report = FALSE;
        int i;
        char buf [MAX_STRING_LENGTH];
        EXIT_DATA *door, *door_rev;
        ROOM_INDEX_DATA *room;
        CHAR_DATA *rch, *rch_next;
        OBJ_DATA *obj, *obj_next;

        if (!allow_deity_command (ch))
                return;

        if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
        {
                send_to_char("Not in a place of sanctuary.\n\r", ch);
                ch->pcdata->deity_timer = 10;
                return;
        }

        sprintf(buf, "{CThe fury of %s is released in a cataclysmic explosion!{x\n\r",
                deity_info_table[ch->pcdata->deity_patron].name);
        send_to_char(buf, ch);


        /*
         * Doors
         */

        for (i = 0; i < 6; i++)
        {
                if (!(door = ch->in_room->exit[i])
                    || !IS_SET(door->exit_info, EX_CLOSED))
                        continue;

                REMOVE_BIT(door->exit_info, EX_CLOSED);
                SET_BIT(door->exit_info, EX_BASHED);

                if (IS_SET(door->exit_info, EX_LOCKED))
                        REMOVE_BIT(door->exit_info, EX_LOCKED);

                if ((room = door->to_room)
                    && (door_rev = room->exit[directions[i].reverse])
                    && door_rev->to_room == ch->in_room)
                {
                        REMOVE_BIT(door_rev->exit_info, EX_CLOSED);
                        SET_BIT(door_rev->exit_info, EX_BASHED);

                        if (IS_SET(door_rev->exit_info, EX_LOCKED))
                                REMOVE_BIT(door_rev->exit_info, EX_LOCKED);

                        for (rch = room->people; rch; rch = rch->next_in_room)
                        {
                                if (rch->deleted)
                                        continue;

                                act("The $d crashes open!",
                                     rch, NULL, door_rev->keyword, TO_CHAR);
                        }
                }

                if (!report)
                {
                        report = TRUE;
                        act("A tremendous shockwave blows open the doors!",
                            ch, NULL, NULL, TO_CHAR);
                        act("A tremendous shockwave blows open the doors!",
                            ch, NULL, NULL, TO_ROOM);
                }
        }


        /*
         * Items on ground
         */

        for (obj = ch->in_room->contents; obj; obj = obj_next)
        {
                obj_next = obj->next_content;

                if (obj->deleted
                    || obj->item_type == ITEM_CORPSE_PC
                    || obj->item_type == ITEM_CLAN_OBJECT
                    || obj->item_type == ITEM_PORTAL)
                        continue;

                act("$p disintegrates!", ch, obj, NULL, TO_CHAR);
                act("$p disintegrates!", ch, obj, NULL, TO_ROOM);
                extract_obj(obj);
        }


        /*
         * Mobiles
         */

        for (rch = ch->in_room->people; rch; rch = rch_next)
        {
                rch_next = rch->next_in_room;

                if (rch->deleted
                    || !IS_NPC(rch)
                    || !mob_interacts_players(rch)
                    || rch == ch->mount
                    || is_group_members_mount(rch, ch)
                    || is_same_group(rch, ch))
                        continue;

                i = number_range(ch->level * ch->level * 3 / 8,
                                 ch->level * ch->level * 5 / 8);

                if (get_deity_stature(ch->pcdata->deity_patron)
                    == DEITY_STATURE_POWERFUL)
                        i *= 2;

                else if (get_deity_stature(ch->pcdata->deity_patron)
                         == DEITY_STATURE_WEAK)
                        i /= 2;

                damage(ch, rch, i, gsn_prayer_destruction, FALSE);
        }

        ch->pcdata->deity_timer += 60;
}


void prayer_combat (CHAR_DATA *ch)
{
        AFFECT_DATA af;
        char buf[MAX_STRING_LENGTH];

        if (!allow_deity_command (ch))
                return;

        sprintf(buf, "{CYou feel %s's hands guide your own in melee!{x\n\r",
                deity_info_table[ch->pcdata->deity_patron].name);
        send_to_char(buf, ch);

        affect_strip(ch, gsn_prayer_combat);
        af.type = gsn_prayer_combat;

        switch (get_deity_stature (ch->pcdata->deity_patron))
        {
            case DEITY_STATURE_POWERFUL:

                /*  hit bonus  */
                af.modifier = ch->level / 2;
                af.duration = ch->level / 3;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*  dam bonus  */
                af.location = APPLY_DAMROLL;
                affect_to_char (ch, &af);

                /* str bonus */
                af.location = APPLY_STR;
                af.modifier = 5 + ch->level / 20;
                affect_to_char(ch, &af);

                /* dex bonus */
                af.location = APPLY_DEX;
                affect_to_char(ch, &af);

                break;

            case DEITY_STATURE_MODERATE:

                /*  hit bonus  */
                af.modifier = ch->level / 3;
                af.duration = ch->level / 4;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*  dam bonus  */
                af.location = APPLY_DAMROLL;
                affect_to_char (ch, &af);

                /* str bonus */
                af.location = APPLY_STR;
                af.modifier = 5 + ch->level / 20;
                affect_to_char(ch, &af);

                /* dex bonus */
                af.location = APPLY_DEX;
                affect_to_char(ch, &af);

                break;

            case DEITY_STATURE_WEAK:

                /*  hit bonus  */
                af.modifier = ch->level / 4;
                af.duration = ch->level / 4;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*  dam bonus  */
                af.location = APPLY_DAMROLL;
                affect_to_char (ch, &af);

                /* str bonus */
                af.location = APPLY_STR;
                af.modifier = 2 + ch->level / 20;
                affect_to_char(ch, &af);

                /* dex bonus */
                af.location = APPLY_DEX;
                affect_to_char(ch, &af);

                break;
        }
}


void prayer_locate (CHAR_DATA *ch, char *text)
{
        CHAR_DATA *victim;
        ROOM_INDEX_DATA *room;
        char buf [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];

        if (!allow_deity_command (ch))
                return;

        if (ch->fighting)
        {
                send_to_char("Not while fighting.\n\r", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        one_argument(text, arg);

        if (arg[0] == '\0')
        {
                patron_message("Who do you want me to look for?", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!(victim = get_char_world(ch, arg)) || !victim->in_room)
        {
                patron_message("I cannot find anyone by that name.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        sprintf(buf, "{C%s reveals the location of %s.{x\n\r\n\r",
                deity_info_table[ch->pcdata->deity_patron].name,
                IS_NPC(victim) ? victim->short_descr : victim->name);
        send_to_char(buf, ch);

        room = ch->in_room;
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        do_look(ch, "");
        char_from_room(ch);
        char_to_room(ch, room);
}


void prayer_weaken (CHAR_DATA *ch, char *text)
{
        CHAR_DATA *victim;
        AFFECT_DATA af;
        char buf [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];

        if (!allow_deity_command(ch))
                return;

        if (ch->fighting)
        {
                send_to_char("Not while fighting.\n\r", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        one_argument(text, arg);

        if (arg[0] == '\0')
        {
                patron_message("Who do you want me to punish?", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!(victim = get_char_world(ch, arg)) || !victim->in_room)
        {
                patron_message("I cannot find anyone by that name.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!IS_NPC(victim))
        {
                if (!in_pkill_range(ch, victim))
                {
                        patron_message("I will not punish that mortal for you.", ch);
                        ch->pcdata->deity_timer = 20;
                        return;
                }

                if (get_deity_favour(victim, ch->pcdata->deity_patron)
                    > DEITY_FAVOUR_INDIFFERENCE)
                {
                        patron_message("I will not punish my favoured subjects!", ch);

                        switch (deity_info_table[ch->pcdata->deity_patron].personality)
                        {
                            case DEITY_PERSONALITY_JUDGEMENTAL:
                            case DEITY_PERSONALITY_PROUD:

                                ch->pcdata->deity_timer /= 2;
                                return;

                            case DEITY_PERSONALITY_CRUEL:
                                victim = ch;
                                break;

                            default:
                                ch->pcdata->deity_timer = 20;
                                return;
                        }
                }
        }

        affect_strip(victim, gsn_prayer_weaken);
        af.type = gsn_prayer_weaken;

        switch (get_deity_stature(ch->pcdata->deity_patron))
        {

            case DEITY_STATURE_POWERFUL:

                /*  hit penalty  */
                af.modifier = - ch->level / 2;
                af.duration = ch->level / 3;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (victim, &af);

                /*  dam penalty  */
                af.location = APPLY_DAMROLL;
                affect_to_char (victim, &af);

                /* str penalty */
                af.location = APPLY_STR;
                af.modifier = - 3 - ch->level / 25;
                affect_to_char(victim, &af);

                /* poison */
                af.location = APPLY_NONE;
                af.bitvector = AFF_POISON;
                affect_to_char(victim, &af);

                break;

            case DEITY_STATURE_MODERATE:

                /*  hit penalty  */
                af.modifier = - ch->level / 3;
                af.duration = ch->level / 4;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (victim, &af);

                /*  dam penalty  */
                af.location = APPLY_DAMROLL;
                affect_to_char (victim, &af);

                /* str penalty */
                af.location = APPLY_STR;
                af.modifier = - 3 - ch->level / 25;
                affect_to_char(victim, &af);

                /* poison */
                af.location = APPLY_NONE;
                af.bitvector = AFF_POISON;
                affect_to_char(victim, &af);

                break;

            case DEITY_STATURE_WEAK:

                /*  hit penalty  */
                af.modifier = - ch->level / 4;
                af.duration = ch->level / 4;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char (victim, &af);

                /*  dam penalty  */
                af.location = APPLY_DAMROLL;
                affect_to_char (victim, &af);

                /* str penalty */
                af.location = APPLY_STR;
                af.modifier = - 3 - ch->level / 25;
                affect_to_char(victim, &af);

                /* poison */
                af.location = APPLY_NONE;
                af.bitvector = AFF_POISON;
                affect_to_char(victim, &af);

                break;
        }

        send_to_char("{CYou feel horribly weak.{x\n\r", victim);

        if (ch != victim)
        {
                sprintf(buf, "%s now suffers by my hand.",
                        IS_NPC(victim) ? victim->short_descr : victim->name);
                patron_message(buf, ch);
        }
}


void prayer_reveal (CHAR_DATA *ch, char *text)
{
        char buf [MAX_STRING_LENGTH];
        char tmp [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];
        double ratio;
        CHAR_DATA *victim;

        if (!allow_deity_command (ch))
                return;

        one_argument(text, arg);
        strcpy(buf, "\n\r");

        if (arg[0] == '\0')
        {
                patron_message("Who do you want me to reveal to you?", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!(victim = get_char_world(ch, arg)) || !victim->in_room)
        {
                patron_message("I cannot find anyone by that name.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        /*
         * PCs
         */

        if (!IS_NPC(victim))
        {
                if (!in_pkill_range(ch, victim))
                {
                        patron_message("I will not reveal that mortal to you.", ch);
                        ch->pcdata->deity_timer = 20;
                        return;
                }

                if (get_deity_favour(victim, ch->pcdata->deity_patron)
                    > DEITY_FAVOUR_INDIFFERENCE)
                {
                        patron_message("I will not reveal my favoured subjects.", ch);
                        ch->pcdata->deity_timer = 20;
                        return;
                }

                if (victim->pcdata->deity_patron < 0)
                {
                        strcat(buf, "Has not chosen a patron deity.\n\r");
                }
                else
                {
                        sprintf(tmp, "Worships the deity %s.\n\r",
                                deity_info_table[victim->pcdata->deity_patron].name);
                        strcat(buf, tmp);
                }
        }

        /*
         * Hit points
         */

        ratio = (double) victim->hit / (double) ch->hit;

        if (ratio >= 1.25)
                strcat(buf, "Is currently much healthier than you.\n\r");
        else if (ratio >= 1.1)
                strcat(buf, "Is currently healthier than you.\n\r");
        else if (ratio > 0.9)
                strcat(buf, "Is currently about as healthy as you.\n\r");
        else if (ratio > 0.75)
                strcat(buf, "Is currently less healthy than you.\n\r");
        else
                strcat(buf, "Is currently a lot less healthy than you.\n\r");

        /*
         * Hit roll
         */

        if (GET_HITROLL(ch) < 1)
                ratio = 0.0;
        else
                ratio = (double) GET_HITROLL(victim) / (double) GET_HITROLL(ch);

        if (ratio >= 1.25)
                strcat(buf, "Hits a lot more often than you during combat.\n\r");
        else if (ratio >= 1.1)
                strcat(buf, "Hits more often than you during combat.\n\r");
        else if (ratio > 0.9)
                strcat(buf, "Hits about as often as you during combat.\n\r");
        else if (ratio > 0.75)
                strcat(buf, "Hits less often than you during combat.\n\r");
        else
                strcat(buf, "Hits a lot less often than you during combat.\n\r");

        /*
         * Dam roll
         */

        if (GET_DAMROLL(ch) < 1)
                ratio = 0.0;
        else
                ratio = (double) GET_DAMROLL(victim) / (double) GET_DAMROLL(ch);

        if (ratio >= 1.25)
                strcat(buf, "Inflicts much more damage than you in combat.\n\r");
        else if (ratio >= 1.1)
                strcat(buf, "Inflicts more damage than you in combat.\n\r");
        else if (ratio > 0.9)
                strcat(buf, "Inflicts about the same amount of damage as you in combat.\n\r");
        else if (ratio > 0.75)
                strcat(buf, "Inflicts less damage than you in combat.\n\r");
        else
                strcat(buf, "Inflicts much less damage than you in combat.\n\r");

        /*
         * Mobs
         */

        if (IS_NPC(victim))
        {
                if (victim->spec_fun)
                        strcat(buf, "Has special powers.\n\r");

                if (IS_SET(victim->act, ACT_AGGRESSIVE))
                        strcat(buf, "Is very aggressive.\n\r");

                if (IS_SET(victim->act, ACT_IS_FAMOUS))
                        strcat(buf, "Is known throughout the Domain.\n\r");

                ch->pcdata->deity_timer /= 2;
        }

        sprintf(tmp, "This is what I have divined of %s:",
                IS_NPC(victim) ? victim->short_descr : victim->name);
        patron_message(tmp, ch);
        send_to_char(buf, ch);
}


void prayer_protection (CHAR_DATA *ch)
{
        AFFECT_DATA af;
        char buf[MAX_STRING_LENGTH];

        if (!allow_deity_command(ch))
                return;

        sprintf(buf, "{C%s surrounds you with protective magiks!{x\n\r",
                deity_info_table[ch->pcdata->deity_patron].name);
        send_to_char(buf, ch);

        affect_strip(ch, gsn_prayer_protection);
        affect_strip(ch, gsn_inertial_barrier);
        af.type = gsn_prayer_protection;

        switch (get_deity_stature (ch->pcdata->deity_patron))
        {
            case DEITY_STATURE_POWERFUL:

                /*
                 * AC
                 */

                af.modifier = - (50 + ch->level * 2);
                af.duration = 5 + ch->level / 2;
                af.location = APPLY_AC;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*
                 * Sanctuary
                 */

                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_SANCTUARY;
                affect_strip (ch, gsn_sanctuary);
                affect_to_char (ch, &af);

                /*
                 * Protection
                 */

                af.bitvector = AFF_PROTECT;
                affect_strip (ch, gsn_protection);
                affect_to_char (ch, &af);

                /*
                 * Save vs spell
                 */

                af.bitvector = 0;
                af.location = APPLY_SAVING_SPELL;
                af.modifier = 0 - (5 + ch->level / 5);
                affect_to_char (ch, &af);

                break;

            case DEITY_STATURE_MODERATE:

                /*
                 * AC
                 */

                af.modifier = - (50 + ch->level * 2);
                af.duration = 5 + ch->level / 3;
                af.location = APPLY_AC;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*
                 * Sanctuary
                 */

                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_SANCTUARY;
                affect_strip (ch, gsn_sanctuary);
                affect_to_char (ch, &af);

                /*
                 * Protection
                 */

                af.bitvector = AFF_PROTECT;
                affect_strip (ch, gsn_protection);
                affect_to_char (ch, &af);

                /*
                 * Save vs spell
                 */

                af.bitvector = 0;
                af.location = APPLY_SAVING_SPELL;
                af.modifier = 0 - (5 + ch->level / 7);
                affect_to_char (ch, &af);

                break;

            case DEITY_STATURE_WEAK:

                /*
                 * AC
                 */

                af.modifier = - (50 + ch->level * 2);
                af.duration = 5 + ch->level / 4;
                af.location = APPLY_AC;
                af.bitvector = 0;
                affect_to_char (ch, &af);

                /*
                 * Sanctuary
                 */

                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_SANCTUARY;
                affect_strip (ch, gsn_sanctuary);
                affect_to_char (ch, &af);

                /*
                 * Save vs spell
                 */

                af.bitvector = 0;
                af.location = APPLY_SAVING_SPELL;
                af.modifier = 0 - (5 + ch->level / 10);
                affect_to_char (ch, &af);

                break;
        }
}


void prayer_rescue (CHAR_DATA *ch)
{
        if (!allow_deity_command (ch))
                return;

        /*
         * Fighting
         */

        if (ch->fighting
            && get_deity_stature(ch->pcdata->deity_patron) == DEITY_STATURE_WEAK
            && !number_bits(2))
        {
                patron_message("I'm too weak to rescue you!", ch);
                ch->pcdata->deity_timer = 20;
                return;
        }

        /*
         * Escape
         */

        if (ch->fighting)
                stop_fighting(ch, TRUE);

        if (ch->mount)
                strip_mount(ch);

        act("{W$c is surrounded by holy light and disappears!{x", ch, NULL, NULL, TO_ROOM);
        send_to_char("{WYou are surrounded by holy light!{x\n\r\n\r", ch);

        char_from_room(ch);
        char_to_room(ch, get_room_index(ROOM_VNUM_TEMPLE));
        act("{W$c appears in a flash of holy light!{x", ch, NULL, NULL, TO_ROOM);

        do_look(ch, "");

        send_to_char("\n\r", ch);
        patron_message("You will find sanctuary here.", ch);
}


void prayer_healing (CHAR_DATA *ch)
{
        char buf [MAX_STRING_LENGTH];

        if (!allow_deity_command (ch))
                return;

        act("{Y$c is briefly surrounded by a warm golden fire!{x", ch, NULL, NULL, TO_ROOM);
        sprintf(buf, "{C%s revives your battered and weary body!{x\n\r",
                deity_info_table[ch->pcdata->deity_patron].name);
        send_to_char(buf, ch);

        /*
         * Full restore
         */

        ch->aggro_dam = 0;
        ch->hit = UMAX(ch->hit, ch->max_hit);
        ch->mana = UMAX(ch->mana, ch->max_mana);
        ch->move = UMAX(ch->move, ch->max_move);
        ch->edrain = 0;
        ch->backstab = 0;

        /*
         * Powerful deities remove some nasty effects
         */

        if (get_deity_stature(ch->pcdata->deity_patron) > DEITY_STATURE_WEAK)
        {
                affect_strip(ch, gsn_blindness);
                affect_strip(ch, gsn_gouge);
                affect_strip(ch, gsn_dirt);
                affect_strip(ch, gsn_poison);

                if (get_deity_stature(ch->pcdata->deity_patron) > DEITY_STATURE_MODERATE)
                {
                        affect_strip(ch, gsn_curse);
                        affect_strip(ch, gsn_hex);
                        affect_strip(ch, gsn_prayer_plague);
                        affect_strip(ch, gsn_prayer_weaken);
                }
        }
}


void prayer_plague (CHAR_DATA *ch)
{
        DESCRIPTOR_DATA *d;
        AFFECT_DATA af;

        if (!allow_deity_command (ch))
                return;

        af.type = gsn_prayer_plague;

        switch (get_deity_stature(ch->pcdata->deity_patron))
        {
            case DEITY_STATURE_POWERFUL:
                af.duration = ch->level;
                break;

            case DEITY_STATURE_MODERATE:
                af.duration = ch->level / 3 * 2;
                break;

            default:
                af.duration = ch->level / 2;
        }

        for (d = descriptor_list; d; d = d->next)
        {
                if (!d->character
                    || d->connected != CON_PLAYING)
                {
                        continue;
                }

                send_to_char("{yA terrible plague afflicts the Domain...{x\n\r",
                             d->character);

                if (ch == d->character
                    || d->character->level > LEVEL_HERO
                    || d->character->pcdata->deity_patron < 0
                    || get_deity_favour(d->character, ch->pcdata->deity_patron)
                       > DEITY_FAVOUR_INDIFFERENCE)
                {
                        continue;
                }

                /*
                 * Please ... kill ... me
                 */

                send_to_char("{gSickness rots your bones and poisons your blood!{x\n\r",
                             d->character);

                affect_strip(d->character, gsn_prayer_plague);

                /*
                 * Plague (equivalent to poison during updates)
                 */

                af.bitvector = AFF_PRAYER_PLAGUE;
                af.location = APPLY_NONE;
                af.modifier = 0;
                affect_to_char(d->character, &af);

                /*
                 * Strength
                 */

                af.bitvector = 0;
                af.location = APPLY_STR;
                af.modifier = 0 - (ch->level / 20 + 3);
                affect_to_char(d->character, &af);

                /*
                 * Constitution
                 */

                af.location = APPLY_CON;
                affect_to_char(d->character, &af);
        }
}


void prayer_chaos_ruin_food_drink (OBJ_DATA *obj)
{
        if (obj->next_content)
                prayer_chaos_ruin_food_drink(obj->next_content);

        else if (obj->contains)
                prayer_chaos_ruin_food_drink(obj->contains);

        if (obj->item_type == ITEM_FOOD)
                obj->value[3] = 1;

        else if (obj->item_type == ITEM_DRINK_CON)
                obj->value[2] = 14;
}

void prayer_chaos_ruin_potions_pills (OBJ_DATA *obj)
{
        if (obj->next_content)
                prayer_chaos_ruin_potions_pills(obj->next_content);

        else if (obj->contains)
                prayer_chaos_ruin_potions_pills(obj->contains);

        if (obj->item_type == ITEM_POTION)
        {
                obj->value[1] = skill_lookup("change sex");
                obj->value[2] = -1;
                obj->value[3] = -1;
        }

        else if (obj->item_type == ITEM_PILL)
        {
                obj->value[1] = skill_lookup("poison");
                obj->value[2] = -1;
                obj->value[3] = -1;
        }
}


void prayer_chaos (CHAR_DATA *ch)
{
        int type = number_bits(2);
        CHAR_DATA *victim;
        DESCRIPTOR_DATA *d;
        ROOM_INDEX_DATA *room;

        if (!allow_deity_command (ch))
                return;

        for (d = descriptor_list; d; d = d->next)
        {
                if (!(victim = d->character)
                    || d->connected != CON_PLAYING)
                {
                        continue;
                }


                send_to_char("{CChaos reigns!{x\n\r", victim);

                if (ch == victim
                    || victim->level > LEVEL_HERO
                    || victim->pcdata->deity_patron < 0
                    || get_deity_favour(victim, ch->pcdata->deity_patron)
                       > DEITY_FAVOUR_INDIFFERENCE)
                {
                        continue;
                }

                switch (type)
                {
                    case 0:

                        /*
                         * Food becomes poisoned; liquid becomes brine
                         */

                        prayer_chaos_ruin_food_drink(victim->carrying);
                        break;

                    case 1:

                        /*
                         * Teleport
                         */


                        while (1)
                        {
                                room = get_room_index(number_range(0, 65535));

                                if (room
                                    && !IS_SET(room->room_flags, ROOM_PRIVATE)
                                    && !IS_SET(room->room_flags, ROOM_NO_RECALL)
                                    && !IS_SET(room->room_flags, ROOM_PLAYER_KILLER)
                                    && !IS_SET(room->room_flags, ROOM_SOLITARY))
                                        break;
                        }

                        if (victim->fighting)
                                stop_fighting(victim, TRUE);

                        act("$n disappears in a flash of light!",
                            victim, NULL, NULL, TO_ROOM);

                        char_from_room(victim);
                        char_to_room(victim, room);

                        if (victim->mount)
                        {
                                char_from_room(victim->mount);
                                char_to_room(victim->mount, room);
                        }

                        send_to_char("\n\r", victim);
                        do_look(victim, "");

                        break;

                    case 2:

                        /*
                         * Coins -> copper
                         */

                        victim->copper += victim->silver + victim->gold + victim->plat;
                        victim->plat = 0;
                        victim->gold = 0;
                        victim->silver = 0;
                        break;

                    case 3:

                        /*
                         * Potions becomes change sex; Pills becomes poisoned
                         */

                        prayer_chaos_ruin_potions_pills(victim->carrying);
                        break;
                }
        }
}


void prayer_transport (CHAR_DATA *ch, char *text)
{
        CHAR_DATA *victim;
        ROOM_INDEX_DATA *room;
        char arg[MAX_INPUT_LENGTH];

        if (!allow_deity_command (ch))
                return;

        one_argument(text, arg);

        if (arg[0] == '\0')
        {

                patron_message("Who do you want me to transport you to?", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (ch->fighting)
        {
                patron_message("I cannot transport you while fighting.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!(victim = get_char_world(ch, arg)) || !victim->in_room)
        {

                patron_message("I cannot find anyone by that name.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!IS_NPC(victim))
        {
                patron_message("I cannot transport you to them.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        room = victim->in_room;

        if (IS_SET(room->room_flags, ROOM_NO_RECALL)
            || IS_SET(room->room_flags, ROOM_PLAYER_KILLER)
            || IS_SET(room->room_flags, ROOM_SOLITARY))
        {
                patron_message("I am unable to transport you to them.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        patron_message("As you wish.", ch);

        act("{C$c disappears in a flash of holy light!{x",
            ch, NULL, NULL, TO_ROOM);

        char_from_room(ch);
        char_to_room(ch, victim->in_room);

        if (ch->mount)
        {
                act ("{W$c disappears in a flash of holy light!{x", ch->mount, NULL, NULL, TO_ROOM);
                char_from_room(ch->mount);
                char_to_room(ch->mount, victim->in_room);
        }

        act("{C$c appears in a flash of holy light!{x", ch, NULL, NULL, TO_ROOM);

        if (ch->mount)
                act("{C$c appears in a flash of holy light!{x", ch->mount, NULL, NULL, TO_ROOM);

        send_to_char("\n\r", ch);
        do_look(ch, "");
}


void prayer_passage (CHAR_DATA *ch)
{
        AFFECT_DATA af;

        if (!allow_deity_command(ch))
                return;

        if (ch->fighting)
        {
                patron_message("I cannot help you while fighting.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        patron_message("My power now protects and aids you while travelling.", ch);

        affect_strip(ch, gsn_prayer_passage);
        affect_strip(ch, gsn_deter);
        affect_strip(ch, gsn_intimidate);
        affect_strip(ch, gsn_fly);
        affect_strip(ch, gsn_levitation);
        affect_strip(ch, gsn_pass_door);
        affect_strip(ch, gsn_ectoplasmic_form);

        af.type = gsn_prayer_passage;

        switch(get_deity_stature(ch->pcdata->deity_patron))
        {
            case DEITY_STATURE_POWERFUL:
                af.duration = 5 + ch->level / 2;
                break;

            case DEITY_STATURE_MODERATE:
                af.duration = 5 + ch->level / 3;
                break;

            case DEITY_STATURE_WEAK:
                af.duration = 5 + ch->level / 4;
                break;
        }

        /*
         * Deter
         */

        af.modifier = 0;
        af.location = APPLY_NONE;
        af.bitvector = AFF_DETER;
        affect_to_char (ch, &af);

        /*
         * Fly
         */

        af.bitvector = AFF_FLYING;
        affect_to_char (ch, &af);

        /*
         * Pass door
         */

        af.bitvector = AFF_PASS_DOOR;
        affect_to_char(ch, &af);
}


void prayer_death (CHAR_DATA *ch, char *text)
{
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];

        if (!allow_deity_command (ch))
                return;

        one_argument(text, arg);

        if (arg[0] == '\0')
        {

                patron_message("Who do you want me to slay for you?", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {

                patron_message("I cannot see them here.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (!IS_NPC(victim))
        {
                patron_message("I cannot slay them for you.", ch);
                ch->pcdata->deity_timer = 0;
                return;
        }

        if (victim->rider)
        {
                send_to_char("Your mount collapses beneath you!", victim->rider);
                act ("$c collapses, throwing $N to the ground!", victim, NULL,
                     victim->rider, TO_NOTVICT);
                victim->rider->position = POS_RESTING;
                strip_mount (victim->rider);
        }

        patron_message("Watch me and learn!", ch);
        act("{Y$c is struck down by bolts of lightning!{x",
            victim, NULL, NULL, TO_ROOM);

        raw_kill(ch, victim, TRUE);
}


void patron_message (char *message, CHAR_DATA *ch)
{
        char buf [MAX_STRING_LENGTH];
        int patron;

        if (!allow_deity_command(ch))
                return;

        patron = ch->pcdata->deity_patron;

        if (patron < 0 || patron >= NUMBER_DEITIES)
        {
                sprintf(buf, "patron_message: %s has invalid patron: %d",
                        ch->name,
                        patron);
                log_deity(buf);
                return;
        }

        sprintf(buf, "%s%s: {C%s{x\n\r",
                deity_info_table[patron].name,
                number_bits(1) ? " speaks to your mind" : "'s voice rings in your head",
                message);
        send_to_char(buf, ch);
}



/*
 *  Something special...  Gez
 */
void write_snark_database ()
{
        FILE *db;
        OBJ_DATA *obj, *in_obj;
        AFFECT_DATA *af;

        if (! (db = fopen ("snark.raw", "w+")))
                return;

        for (obj = object_list; obj; obj = obj->next)
        {
                for (in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj)
                        ;

                /*
                fprintf (db, "%d\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
                         obj->pIndexData->vnum,
                         obj->short_descr,
                         obj->item_type,
                         obj->extra_flags,
                         obj->wear_flags,
                         obj->value[0],
                         obj->value[1],
                         obj->value[2],
                         obj->value[3],
                         obj->level);
                */

                fprintf (db, "%d\n%s\n%d\n%lu\n%d\n",
                         obj->pIndexData->vnum,
                         obj->short_descr,
                         obj->item_type,
                         obj->extra_flags,
                         obj->wear_flags);

                switch (obj->item_type)
                {
                case ITEM_WAND:
                case ITEM_STAFF:
                        fprintf (db, "%d\n%d\n%d\n%s\n",
                                obj->value[0],
                                obj->value[1],
                                obj->value[2],
                                obj->value[3] > 0
                                        ? skill_table[obj->value[3]].name : "");
                        break;

                case ITEM_SCROLL:
                case ITEM_POTION:
                case ITEM_PAINT:
                case ITEM_PILL:
                        fprintf (db, "%d\n%s\n%s\n%s\n",
                                obj->value[0],
                                obj->value[1] > 0
                                        ? skill_table[obj->value[1]].name : "",
                                obj->value[2] > 0
                                        ? skill_table[obj->value[2]].name : "",
                                obj->value[3] > 0
                                        ? skill_table[obj->value[3]].name : "");
                        break;

                default:
                        fprintf (db, "%d\n%d\n%d\n%d\n",
                                obj->value[0],
                                obj->value[1],
                                obj->value[2],
                                obj->value[3]);
                        break;
                }

                fprintf (db, "%d\n", obj->level);

                for (af = obj->pIndexData->affected; af; af = af->next)
                {
                        fprintf (db, "Apply\n%d %d\n",
                                 af->location,
                                 af->modifier);
                }

                if (in_obj->carried_by)
                {
                        fprintf (db, "Mob\n%s\n%d\n",
                                 in_obj->carried_by->short_descr ?
                                 in_obj->carried_by->short_descr : "unknown",
                                 in_obj->carried_by->pIndexData ?
                                 in_obj->carried_by->pIndexData->vnum : -1);
                }
                else if (in_obj->in_room)
                {
                        fprintf (db, "Room\n%s\n%d\n",
                                 in_obj->in_room->name,
                                 in_obj->in_room->vnum);
                }

                fprintf (db, "End\n\n");
        }

        fclose (db);
}

/* EOF deity.c */
