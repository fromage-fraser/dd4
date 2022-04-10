/*
 * Lexicon: a collection of name and phrase generation methods.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "merc.h"
#include "lexicon.h"

/* We will only consider up to 2000 candidates when searching for words.
   Bump this limit if any one word list gets larger! */
#define MAX_CANDIDATES (2000)

const LEXICON_ENTRY lexicon_adjectives[] =
{
        { "red",        LEXICON_FILTER_GENERAL },
        { "scarlet",    LEXICON_FILTER_GENERAL },
        { "crimson",    LEXICON_FILTER_GENERAL },
        { "blue",       LEXICON_FILTER_GENERAL },
        { "green",      LEXICON_FILTER_GENERAL },
        { "yellow",     LEXICON_FILTER_GENERAL },
        { "orange",     LEXICON_FILTER_GENERAL },
        { "purple",     LEXICON_FILTER_GENERAL },
        { "indigo",     LEXICON_FILTER_GENERAL },
        { "violet",     LEXICON_FILTER_GENERAL },
        { "black",      LEXICON_FILTER_GENERAL },
        { "white",      LEXICON_FILTER_GENERAL },
        { "gray",       LEXICON_FILTER_GENERAL },

        { "golden",     LEXICON_FILTER_GENERAL },
        { "silver",     LEXICON_FILTER_GENERAL },
        { "steel",      LEXICON_FILTER_GENERAL },
        { "iron",       LEXICON_FILTER_GENERAL },
        { "copper",     LEXICON_FILTER_GENERAL },
        { "bronze",     LEXICON_FILTER_GENERAL },
        { "platinum",   LEXICON_FILTER_GENERAL },
        { "obsidian",   LEXICON_FILTER_GENERAL },

        { "ruby",       LEXICON_FILTER_GENERAL },
        { "diamond",    LEXICON_FILTER_GENERAL },
        { "sapphire",   LEXICON_FILTER_GENERAL },
        { "emerald",    LEXICON_FILTER_GENERAL },
        { "opal",       LEXICON_FILTER_GENERAL },

        { "silent",     LEXICON_FILTER_GOOD | LEXICON_FILTER_NEUTRAL },
        { "sublime",    LEXICON_FILTER_GOOD | LEXICON_FILTER_NEUTRAL },
        { "lonely",     LEXICON_FILTER_NEUTRAL },
        { "mysterious", LEXICON_FILTER_NEUTRAL },
        { "hidden",     LEXICON_FILTER_NEUTRAL },

        { "secret",     LEXICON_FILTER_GENERAL },
        { "ancient",    LEXICON_FILTER_GENERAL },
        { "new",        LEXICON_FILTER_GENERAL },
        { "old",        LEXICON_FILTER_GENERAL },
        { "elder",      LEXICON_FILTER_GENERAL },
        { "lost",       LEXICON_FILTER_GENERAL },
        { "jealous",    LEXICON_FILTER_GENERAL },

        { "holy",       LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "blessed",    LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "loyal",      LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "shining",    LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "golden",     LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "peaceful",   LEXICON_FILTER_GOOD | LEXICON_FILTER_NEUTRAL | LEXICON_FILTER_LAWFUL },

        { "gruesome",           LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "awful",              LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "deadly",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "bloody",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "bloodthirsty",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "unholy",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "sadistic",           LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "homicidal",          LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "cruel",              LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "withered",   LEXICON_FILTER_EVIL },
        { "baleful",    LEXICON_FILTER_EVIL },

        { "crazy",      LEXICON_FILTER_CHAOTIC },
        { "insane",     LEXICON_FILTER_CHAOTIC },
        { "manic",      LEXICON_FILTER_CHAOTIC },
};

const LEXICON_ENTRY lexicon_collective_nouns[] =
{
        { "squadron",   LEXICON_FILTER_GENERAL },
        { "squad",      LEXICON_FILTER_GENERAL },
        { "band",       LEXICON_FILTER_GENERAL },
        { "legion",     LEXICON_FILTER_GENERAL },
        { "crew",       LEXICON_FILTER_GENERAL },
        { "society",    LEXICON_FILTER_GENERAL },
        { "brothers",   LEXICON_FILTER_GENERAL },
        { "sisters",    LEXICON_FILTER_GENERAL },
        { "children",   LEXICON_FILTER_GENERAL },

        { "horde",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT | LEXICON_FILTER_CHAOTIC },
        { "cult",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT | LEXICON_FILTER_CHAOTIC },
        { "gang",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT | LEXICON_FILTER_CHAOTIC },

        { "army",               LEXICON_FILTER_GOOD | LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "soldiers",           LEXICON_FILTER_GOOD | LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "brigade",            LEXICON_FILTER_GOOD | LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "company",            LEXICON_FILTER_GOOD | LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "foot soldiers",      LEXICON_FILTER_GOOD | LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "killers",            LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "thieves",            LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "ghosts",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "devils",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "demons",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "thugs",              LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "assassins",          LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "hunters",            LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "slayers",            LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "executioners",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "butchers",           LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "cannibals",          LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "raiders",            LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "berserkers",         LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "death eaters",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "head smashers",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "blood drinkers",     LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "wild men",           LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "slaves",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "crusaders",  LEXICON_FILTER_GOOD | LEXICON_FILTER_VIOLENT },
        { "knights",    LEXICON_FILTER_GOOD | LEXICON_FILTER_VIOLENT },
        { "garrison",   LEXICON_FILTER_GOOD | LEXICON_FILTER_VIOLENT },

        { "battalion",  LEXICON_FILTER_GENERAL | LEXICON_FILTER_VIOLENT },
        { "warriors",   LEXICON_FILTER_GENERAL | LEXICON_FILTER_VIOLENT },

        { "judges",     LEXICON_FILTER_LAWFUL },

        { "patrol",     LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "patrollers", LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "guards",     LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "guardians",  LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "lords",      LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
};

const LEXICON_ENTRY lexicon_abstract_nouns[] =
{
        { "time",       LEXICON_FILTER_GENERAL },
        { "eternity",   LEXICON_FILTER_GENERAL },
        { "winter",     LEXICON_FILTER_GENERAL },
        { "summer",     LEXICON_FILTER_GENERAL },
        { "autumn",     LEXICON_FILTER_GENERAL },
        { "spring",     LEXICON_FILTER_GENERAL },

        { "beauty",             LEXICON_FILTER_GOOD },
        { "truth",              LEXICON_FILTER_GOOD },
        { "light",              LEXICON_FILTER_GOOD },
        { "heaven",             LEXICON_FILTER_GOOD },
        { "true destiny",       LEXICON_FILTER_GOOD },
        { "love",               LEXICON_FILTER_GOOD },
        { "zeal",               LEXICON_FILTER_GOOD },

        { "blood",              LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "certain doom",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "doom",               LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "death",              LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "pain",               LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "famine",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "bedlam",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "destruction",        LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "armageddon",         LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "hell",               LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "sorrow",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "terror",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "shadow",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "horror",             LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "chaos",      LEXICON_FILTER_CHAOTIC },
        { "turmoil",    LEXICON_FILTER_CHAOTIC },
        { "anarchy",    LEXICON_FILTER_CHAOTIC },

        { "fate",       LEXICON_FILTER_LAWFUL },
        { "woe",        LEXICON_FILTER_LAWFUL | LEXICON_FILTER_EVIL },
        { "destiny",    LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "zeal",       LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
};

const LEXICON_ENTRY lexicon_concrete_nouns[] =
{
        { "sun",        LEXICON_FILTER_GENERAL },
        { "moon",       LEXICON_FILTER_GENERAL },
        { "sea",        LEXICON_FILTER_GENERAL },
        { "ocean",      LEXICON_FILTER_GENERAL },
        { "stars",      LEXICON_FILTER_GENERAL },
        { "river",      LEXICON_FILTER_GENERAL },
        { "waves",      LEXICON_FILTER_GENERAL },
        { "forest",     LEXICON_FILTER_GENERAL },
        { "mountain",   LEXICON_FILTER_GENERAL },
        { "mountains",  LEXICON_FILTER_GENERAL },
        { "valley",     LEXICON_FILTER_GENERAL },
        { "hill",       LEXICON_FILTER_GENERAL },
        { "hills",      LEXICON_FILTER_GENERAL },
        { "tree",       LEXICON_FILTER_GENERAL },
        { "cavern",     LEXICON_FILTER_GENERAL },
        { "crystal",    LEXICON_FILTER_GENERAL },

        { "dragon",     LEXICON_FILTER_GENERAL },
        { "griffon",    LEXICON_FILTER_GENERAL },
        { "serpent",    LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "eye",        LEXICON_FILTER_GENERAL },
        { "hand",       LEXICON_FILTER_GENERAL },
        { "god",        LEXICON_FILTER_GENERAL },
        { "gods",       LEXICON_FILTER_GENERAL },

        { "banner",     LEXICON_FILTER_GENERAL },
        { "flag",       LEXICON_FILTER_GENERAL },
        { "standard",   LEXICON_FILTER_GENERAL },

        { "tower",      LEXICON_FILTER_GENERAL },
        { "castle",     LEXICON_FILTER_GENERAL },
        { "keep",       LEXICON_FILTER_GENERAL },
        { "city",       LEXICON_FILTER_GENERAL },
        { "bastion",    LEXICON_FILTER_GENERAL },

        { "dungeon",    LEXICON_FILTER_EVIL },
        { "crypt",      LEXICON_FILTER_EVIL },
        { "coffin",     LEXICON_FILTER_EVIL },
        { "pit",        LEXICON_FILTER_EVIL },
        { "plague pit", LEXICON_FILTER_EVIL },
        { "wastes",     LEXICON_FILTER_EVIL },
        { "scum",       LEXICON_FILTER_EVIL },

        { "hammer",     LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "sword",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "spear",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "axe",        LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "arrow",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "claw",       LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "tooth",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "hatchet",    LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "scimitar",   LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "skull",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "skulls",     LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },
        { "bones",      LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT },

        { "citadel",    LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "shield",     LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },
        { "sanctum",    LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL },

        { "law",        LEXICON_FILTER_LAWFUL },
        { "book",       LEXICON_FILTER_GENERAL | LEXICON_FILTER_LAWFUL },
        { "grimoire",   LEXICON_FILTER_EVIL | LEXICON_FILTER_LAWFUL },
};

/*
 * Main function for selecting a random entry from an entry list, with optional filtering.
 * Supply a filter value of zero to consider all entries.
 * Callers of the library should use the list-specific wrapper functions.
 */
const char *_lexicon_random_entry_text(
        const LEXICON_ENTRY all_entries[],
        const int all_entries_count,
        const unsigned int filter,
        const char * const entry_list_debug
) {
        const LEXICON_ENTRY *candidate_entries[MAX_CANDIDATES];
        const LEXICON_ENTRY *candidate_entry;
        int candidate_count = 0;
        int i;
        char buf[1024];

        for (i = 0; i < all_entries_count && i < MAX_CANDIDATES; i++) {
                candidate_entry = &all_entries[i];
                if (candidate_entry->filter & filter) {
                        candidate_entries[candidate_count++] = candidate_entry;
                }
        }

        if (candidate_count == 0) {
                sprintf(buf, "Lexicon: Couldn't find any candidates in %s for filter value %d",
                        entry_list_debug, filter);
                log_string(buf);
                return "?";
        }

        return candidate_entries[number_range(0, candidate_count - 1)]->text;
}

const char *lexicon_random_adjective(const unsigned int filter)
{
        static const int count = sizeof(lexicon_adjectives) / sizeof(LEXICON_ENTRY);
        return _lexicon_random_entry_text(lexicon_adjectives, count, filter, "lexicon_adjectives");
}

const char *lexicon_random_collective_noun(const unsigned int filter)
{
        static const int count = sizeof(lexicon_collective_nouns) / sizeof(LEXICON_ENTRY);
        return _lexicon_random_entry_text(lexicon_collective_nouns, count, filter, "lexicon_collective_nouns");
}

const char *lexicon_random_abstract_noun(const unsigned int filter)
{
        static const int count = sizeof(lexicon_abstract_nouns) / sizeof(LEXICON_ENTRY);
        return _lexicon_random_entry_text(lexicon_abstract_nouns, count, filter, "lexicon_abstract_nouns");
}

const char *lexicon_random_concrete_noun(const unsigned int filter)
{
        static const int count = sizeof(lexicon_concrete_nouns) / sizeof(LEXICON_ENTRY);
        return _lexicon_random_entry_text(lexicon_concrete_nouns, count, filter, "lexicon_concrete_nouns");
}
