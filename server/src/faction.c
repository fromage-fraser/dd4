/*
 * Factions: random allegiances of mobiles with their own personalities, abilities and buffs.
 *
 * See server/doc/factions.md
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "merc.h"
#include "faction.h"
#include "lexicon.h"

#define MAX_NAME_LENGTH (50)

/*
 * Factions table.
 */
FACTION *factions[MAX_FACTIONS];

/*
 * Whether the factions feature is enabled.
 * Set _feature_enabled to FALSE to disable at startup.
 * Use faction_feature_enabled() to test the value.
 */
bool _feature_enabled = TRUE;

bool faction_feature_enabled()
{
        return _feature_enabled;
}

void faction_log(const char * const text)
{
        char buf[MAX_STRING_LENGTH + 1];
        snprintf(buf, MAX_STRING_LENGTH, "[faction] %s", text);
        log_string(buf);
}

const char *faction_alignment_name(const faction_alignment alignment)
{
        switch (alignment) {
        case FACTION_ALIGNMENT_GOOD:
                return "good";
        case FACTION_ALIGNMENT_NEUTRAL:
                return "neutral";
        case FACTION_ALIGNMENT_EVIL:
                return "evil";
        default:
                return "unknown";
        }
}

const char *faction_demeanour_name(const faction_demeanour demeanour)
{
        switch (demeanour) {
        case FACTION_DEMEANOUR_PEACEFUL:
                return "peaceful";
        case FACTION_DEMEANOUR_LAWFUL:
                return "lawful";
        case FACTION_DEMEANOUR_VIOLENT:
                return "violent";
        case FACTION_DEMEANOUR_CHAOTIC:
                return "chaotic";
        default:
                return "unknown";
        }
}

/*
 * 'factions' wiz command.
 */
void do_factions(CHAR_DATA *ch, char *argument)
{
        const FACTION *faction;
        char buf[MAX_STRING_LENGTH];
        int i;

        if (!authorized(ch, gsn_factions)) return;

        if (!faction_feature_enabled()) {
                send_to_char("Factions system is disabled.\n\r", ch);
                return;
        }

        send_to_char("Factions\n\r~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r",
                ch);

        for (i = 0; i < MAX_FACTIONS; i++) {
                faction = factions[i];
                sprintf(buf, "%-2d  %-50s  %s, %s\n\r",
                        faction->id,
                        faction->name,
                        faction_alignment_name(faction->alignment),
                        faction_demeanour_name(faction->demeanour));
                if ((i + 1) % 5 == 0) strcat(buf, "\n\r");
                send_to_char(buf, ch);
        }
}

/*
 * Name generator: (the) X Ys - the Crazy Rogues
 */
void faction_generate_name_variation_1(char * const name_buf, const int max_len, const unsigned int filter)
{
        char adj[MAX_STRING_LENGTH + 1];
        char collective_noun[MAX_STRING_LENGTH + 1];

        strncpy(adj, capitalize_words(lexicon_random_adjective(filter)), MAX_STRING_LENGTH);
        strncpy(collective_noun, capitalize_words(lexicon_random_collective_noun(filter)), MAX_STRING_LENGTH);

        snprintf(name_buf, max_len, "%s %s", adj, collective_noun);
}

/*
 * Name generator: (the) Xs of Y - the Legion of Doom
 */
void faction_generate_name_variation_2(char * const name_buf, const int max_len, const unsigned int filter)
{
        char collective_noun[MAX_STRING_LENGTH + 1];
        char abstract_noun[MAX_STRING_LENGTH + 1];

        strncpy(collective_noun, capitalize_words(lexicon_random_collective_noun(filter)), MAX_STRING_LENGTH);
        strncpy(abstract_noun, capitalize_words(lexicon_random_abstract_noun(filter)), MAX_STRING_LENGTH);

        snprintf(name_buf, max_len, "%s of %s", collective_noun, abstract_noun);
}

/*
 * Name generator: (the) X Zs of Y - The Bloody Legion of Doom
 */
void faction_generate_name_variation_3(char * const name_buf, const int max_len, const unsigned int filter)
{
        char adj[MAX_STRING_LENGTH + 1];
        char collective_noun[MAX_STRING_LENGTH + 1];
        char abstract_noun[MAX_STRING_LENGTH + 1];

        strncpy(adj, capitalize_words(lexicon_random_adjective(filter)), MAX_STRING_LENGTH);
        strncpy(collective_noun, capitalize_words(lexicon_random_collective_noun(filter)), MAX_STRING_LENGTH);
        strncpy(abstract_noun, capitalize_words(lexicon_random_abstract_noun(filter)), MAX_STRING_LENGTH);

        snprintf(name_buf, max_len, "%s %s of %s", adj, collective_noun, abstract_noun);
}

/*
 * Name generator: (the) Xs of the Y Z - The Army of the Bloody Claw
 */
void faction_generate_name_variation_4(char * const name_buf, const int max_len, const unsigned int filter)
{
        char adj[MAX_STRING_LENGTH + 1];
        char collective_noun[MAX_STRING_LENGTH + 1];
        char concrete_noun[MAX_STRING_LENGTH + 1];

        strncpy(adj, capitalize_words(lexicon_random_adjective(filter)), MAX_STRING_LENGTH);
        strncpy(collective_noun, capitalize_words(lexicon_random_collective_noun(filter)), MAX_STRING_LENGTH);
        strncpy(concrete_noun, capitalize_words(lexicon_random_concrete_noun(filter)), MAX_STRING_LENGTH);

        snprintf(name_buf, max_len, "%s of the %s %s", collective_noun, adj, concrete_noun);
}

/*
 * Generate a random name, applying a name filter.
 */
void faction_generate_name(char * const name_buf, const int max_len, const unsigned int filter)
{
        switch (number_range(1, 6)) {
        default:
        case 1:
        case 2:
                /* the Loyal Knights */
                faction_generate_name_variation_1(name_buf, max_len, filter);
                break;
        case 3:
        case 4:
                /* the Children of Bedlam */
                faction_generate_name_variation_2(name_buf, max_len, filter);
                break;
        case 5:
                /* the Baleful Brigade of Woe */
                faction_generate_name_variation_3(name_buf, max_len, filter);
                break;
        case 6:
                /* the Horde of the Red Citadel */
                faction_generate_name_variation_4(name_buf, max_len, filter);
                break;
        }
}

/*
 * Find a registered faction by name.
 */
FACTION *faction_find_by_name(const char * const name)
{
        FACTION *faction;
        int i;

        if (strlen(name) == 0) return NULL;

        for (i = 0; i < MAX_FACTIONS; i++) {
                faction = factions[i];
                if (faction != NULL && !str_cmp(name, faction->name)) return faction;
        }

        return NULL;
}

/*
 * Randomly select a faction alignment.
 */
faction_alignment faction_select_alignment()
{
        const int num = number_percent();

        if (num <= 35)
                return FACTION_ALIGNMENT_EVIL;
        else if (num <= 70)
                return FACTION_ALIGNMENT_GOOD;
        else
                return FACTION_ALIGNMENT_NEUTRAL;
}

/*
 * Randomly select a faction demeanour for the given alignment.
 */
faction_demeanour faction_select_demeanour(const faction_alignment alignment)
{
        const int num = number_percent();

        switch (alignment) {
        case FACTION_ALIGNMENT_GOOD:
                if (num <= 65)
                        return FACTION_DEMEANOUR_LAWFUL;
                else
                        return FACTION_DEMEANOUR_PEACEFUL;

        case FACTION_ALIGNMENT_NEUTRAL:
                if (num <= 60)
                        return FACTION_DEMEANOUR_PEACEFUL;
                else if (num <= 80)
                        return FACTION_DEMEANOUR_LAWFUL;
                else
                        return FACTION_DEMEANOUR_CHAOTIC;

        case FACTION_ALIGNMENT_EVIL:
                if (num <= 10)
                        return FACTION_DEMEANOUR_PEACEFUL;
                else if (num <= 20)
                        return FACTION_DEMEANOUR_CHAOTIC;
                else if (num <= 40)
                        return FACTION_DEMEANOUR_LAWFUL;
                else
                        return FACTION_DEMEANOUR_VIOLENT;

        default:
                faction_log("BUG: unhandled alignment in faction_select_demeanour");
                abort();
        }
}

/*
 * Randomly select a name filter for factions of good alignment and the given demeanour.
 */
unsigned int faction_select_name_filter_good_alignment(const faction_demeanour demeanour)
{
        const int num = number_percent();

        switch (demeanour) {
        case FACTION_DEMEANOUR_LAWFUL:
                if (num <= 25)
                        return LEXICON_FILTER_GOOD | LEXICON_FILTER_GENERAL;
                else if (num <= 50)
                        return LEXICON_FILTER_GOOD | LEXICON_FILTER_LAWFUL;
                else if (num <= 75)
                        return LEXICON_FILTER_LAWFUL | LEXICON_FILTER_GENERAL;
                else
                        return LEXICON_FILTER_GOOD;

        default:
                if (num <= 50)
                        return LEXICON_FILTER_GOOD | LEXICON_FILTER_GENERAL;
                else
                        return LEXICON_FILTER_GOOD;
        }
}

/*
 * Randomly select a name filter for factions neutral evil alignment and the given demeanour.
 */
unsigned int faction_select_name_filter_neutral_alignment(const faction_demeanour demeanour)
{
        const int num = number_percent();

        switch (demeanour) {
        case FACTION_DEMEANOUR_LAWFUL:
                if (num <= 75)
                        return LEXICON_FILTER_LAWFUL | LEXICON_FILTER_NEUTRAL;
                else
                        return LEXICON_FILTER_NEUTRAL | LEXICON_FILTER_GENERAL;

        case FACTION_DEMEANOUR_CHAOTIC:
                return LEXICON_FILTER_NEUTRAL| LEXICON_FILTER_GENERAL | LEXICON_FILTER_CHAOTIC;

        case FACTION_DEMEANOUR_VIOLENT:
                return LEXICON_FILTER_NEUTRAL| LEXICON_FILTER_GENERAL | LEXICON_FILTER_VIOLENT;

        default:
                return LEXICON_FILTER_NEUTRAL | LEXICON_FILTER_GENERAL;
        }
}

/*
 * Randomly select a name filter for factions of evil alignment and the given demeanour.
 */
unsigned int faction_select_name_filter_evil_alignment(const faction_demeanour demeanour)
{
        const int num = number_percent();

        switch (demeanour) {
        case FACTION_DEMEANOUR_LAWFUL:
                if (num <= 75)
                        return LEXICON_FILTER_LAWFUL | LEXICON_FILTER_EVIL;
                else
                        return LEXICON_FILTER_EVIL | LEXICON_FILTER_GENERAL;

        case FACTION_DEMEANOUR_CHAOTIC:
                if (num <= 10)
                        return LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT | LEXICON_FILTER_CHAOTIC;
                if (num <= 75)
                        return LEXICON_FILTER_EVIL | LEXICON_FILTER_CHAOTIC;
                else
                        return LEXICON_FILTER_EVIL | LEXICON_FILTER_GENERAL;

        case FACTION_DEMEANOUR_VIOLENT:
                return LEXICON_FILTER_EVIL | LEXICON_FILTER_VIOLENT;

        default:
                return LEXICON_FILTER_EVIL | LEXICON_FILTER_GENERAL;
        }
}

/*
 * Randomly select a name filter for the given alignment and demeanour.
 */
unsigned int faction_select_name_filter(const faction_alignment alignment, const faction_demeanour demeanour)
{
        switch (alignment) {
        case FACTION_ALIGNMENT_GOOD:
                return faction_select_name_filter_good_alignment(demeanour);
        case FACTION_ALIGNMENT_NEUTRAL:
                return faction_select_name_filter_neutral_alignment(demeanour);
        case FACTION_ALIGNMENT_EVIL:
                return faction_select_name_filter_evil_alignment(demeanour);
        default:
                faction_log("BUG: unhandled alignment in faction_select_name_filter");
                abort();
        }
}

/*
 * Randomly generate a faction with the given unique ID and alignment.
 */
FACTION *faction_generate_faction(const int id, const faction_alignment alignment)
{
        FACTION *faction;
        char name[MAX_NAME_LENGTH + 1];
        int attempt;
        bool ok = FALSE;

        const faction_demeanour demeanour = faction_select_demeanour(alignment);
        const unsigned int name_filter = faction_select_name_filter(alignment, demeanour);

        /* Don't try forever to generate unique names */
        for (attempt = 0; attempt < 100; attempt++) {
                faction_generate_name(name, MAX_NAME_LENGTH, name_filter);
                if (!faction_find_by_name(name)) {
                        ok = TRUE;
                        break;
                }
        }

        if (!ok) return NULL;

        faction = (FACTION *) alloc_perm(sizeof(FACTION));
        faction->id = id;
        faction->name = str_dup(name);
        faction->alignment = alignment;
        faction->demeanour = demeanour;

        return faction;
}

/*
 * Main initialization function. This should be called once before the first area reset.
 */
void faction_init()
{
        /* Ensure we have a certain number of each alignment represented */
        const int min_per_alignment = MAX_FACTIONS / 6;

        FACTION *faction;
        char buf[MAX_STRING_LENGTH + 1];
        int good_count = 0;
        int neutral_count = 0;
        int evil_count = 0;
        faction_alignment alignment;
        int i;

        if (!faction_feature_enabled()) {
                faction_log("Factions disabled");
                return;
        }

        /* Zero everything in the factions table as we will be checking for previously used names */
        for (i = 0; i < MAX_FACTIONS; i++) factions[i] = NULL;

        faction_log("Generating factions...");

        for (i = 0; i < MAX_FACTIONS; i++) {
                /* Fill alignment quotas first, then select randomly */
                if (good_count < min_per_alignment) {
                        good_count++;
                        alignment = FACTION_ALIGNMENT_GOOD;
                } else if (neutral_count < min_per_alignment) {
                        neutral_count++;
                        alignment = FACTION_ALIGNMENT_NEUTRAL;
                } else if (evil_count < min_per_alignment) {
                        evil_count++;
                        alignment = FACTION_ALIGNMENT_EVIL;
                } else {
                        alignment = faction_select_alignment();
                }

                /* ID scheme: integer sequence starting from 1 */
                faction = faction_generate_faction(i + 1, alignment);

                if (!faction) {
                        faction_log("Unable to generate unique faction details: disabling factions");
                        _feature_enabled = FALSE;
                        return;
                }

                factions[i] = faction;
                snprintf(buf, MAX_STRING_LENGTH, "Faction %d: %s (%s, %s)",
                        faction->id,
                        faction->name,
                        faction_alignment_name(faction->alignment),
                        faction_demeanour_name(faction->demeanour));
                faction_log(buf);
        }
}

