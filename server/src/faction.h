/*
 * Factions: random allegiances of mobiles with their own personalities, abilities and buffs.
 *
 * See server/doc/factions.md
 */

#pragma once

/* Number of factions to generate. At least six should be generated. */
#define MAX_FACTIONS (25)

/* Alignment: restricts which mobiles can be members. There can be some overlapping of actual alignment value ranges.
   E.g. some "evil-ish" mobiles could belong to evil or neutral factions. */
typedef enum {
        FACTION_ALIGNMENT_GOOD,
        FACTION_ALIGNMENT_NEUTRAL,
        FACTION_ALIGNMENT_EVIL
} faction_alignment;

/* Demeanour: attitude, behaviour */
typedef enum {
        FACTION_DEMEANOUR_PEACEFUL,
        FACTION_DEMEANOUR_LAWFUL,
        FACTION_DEMEANOUR_VIOLENT,
        FACTION_DEMEANOUR_CHAOTIC
} faction_demeanour;

/* A faction */
typedef struct {
        int id;
        char *name;
        faction_alignment alignment;
        faction_demeanour demeanour;
} FACTION;

/* All generated factions */
extern FACTION *factions[];

/* Check whether the factions feature is enabled */
bool faction_feature_enabled();

/* Initialise faction data */
void faction_init();

/* Logging helper */
void faction_log(const char *text);

/* Wiz command: show faction details */
DECLARE_DO_FUN(do_factions);
