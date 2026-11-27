/***************************************************************************
 *  mob.c
 *
 *  Mob Specs & Species types & related stuff go here.
 *
 *
 *
 * Brutus 2022 / Owl 2026.
 *
 ***************************************************************************/
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

const struct rank       rank_table [ MAX_RANK ] =
{
        /* name, rank_bonus, hp_bonus, who_format */
        { "none",       1,      1,      "{WCommon{x "},
        { "common",     1,      1,      "{WCommon{x "},
        { "rare",       3,      2,      "<39>[Rare]<0> "},
        { "elite",      3,      5,      "<93>[Elite]<0> "},
        { "boss",       4,      7,      "<514><556><16>[<560>BOSS<561>]<0><557> "},
        { "world",      5,     30,      "<81>[WO<75>RL<69>D B<75>OS<81>S]<0> "}
};

const struct species_type species_table[MAX_SPECIES] =
{
        /*
         * name
         * ACT defaults, AFF defaults
         * body-form defaults, natural attack-part defaults
         * resists, vulnerabilities, immunities
         * hp, damage, critical and haste modifier defaults
         * height, weight, size and language defaults
         * default special-function names
         */

        {
                "reserved",
                0, 0,
                0, 0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "humanoid",
                0, 0,
                PART_HEAD | PART_ARMS | PART_2_LEGS | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_HANDS | PART_FEET
                    | PART_FINGERS | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "elemental",
                0, 0,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_HEART
                    | BODY_INORGANIC | PART_HEAD,
                0,
                0, 0, RES_COLD,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "farm_mammal",
                0, 0,
                PART_HEAD | PART_2_LEGS | PART_4_LEGS | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_EAR | PART_EYE,
                PART_HOOVES,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "dragon",
                0, 0,

                BODY_NO_ARMS
                    | PART_HEAD | PART_4_LEGS | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_EAR | PART_EYE
                    | PART_WINGS | PART_TAIL | PART_SCALES
                    | PART_CLAWS | PART_FANGS,

                PART_CLAWS | PART_FANGS | PART_TAILATTACK,

                0, 0, 0,

                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "quadrupedal_reptile",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_4_LEGS | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FEET | PART_EAR | PART_EYE
                    | PART_TAIL | PART_SCALES,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "serpentine_reptile",
                0, 0,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH
                    | PART_HEAD | PART_HEART | PART_BRAINS | PART_GUTS
                    | PART_EYE | PART_TAIL | PART_SCALES,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "mysterious_being",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_INORGANIC,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "winged_mammal",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_2_LEGS | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FEET | PART_EAR | PART_EYE | PART_WINGS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "winged_reptile",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_2_LEGS | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FEET | PART_EAR | PART_EYE | PART_WINGS | PART_SCALES,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "bipedal_construct",
                0, 0,
                BODY_NO_HEART | BODY_INORGANIC
                    | PART_HEAD | PART_ARMS | PART_2_LEGS | PART_HANDS
                    | PART_FEET | PART_FINGERS | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "quadrupedal_mammal",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_4_LEGS | PART_FORELEGS | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_FEET | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "head_only",
                0, 0,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_HEART
                    | PART_HEAD | PART_EYE | PART_EAR,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "limbless_invertebrate",
                0, 0,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH
                    | PART_HEAD | PART_EYE | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "limbless_construct",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_INORGANIC,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },

        /* A humanlike upper body and a four-legged mammalian lower body. */
        {
                "centauroid_mammal",
                0, 0,
                PART_HEAD | PART_ARMS | PART_HANDS | PART_FINGERS
                    | PART_4_LEGS | PART_FORELEGS | PART_FEET
                    | PART_HEART | PART_BRAINS | PART_GUTS | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE, MOB_TEMPLATE_UNSET
        },
        /* The archetype supplies the avian and mammalian details of a hybrid. */
        {
                "quadrupedal_hybrid",
                0, 0,
                BODY_NO_ARMS | PART_HEAD | PART_4_LEGS | PART_FORELEGS
                    | PART_HEART | PART_BRAINS | PART_GUTS | PART_FEET
                    | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "elemental_being",
                0, 0,
                BODY_NO_HEART | BODY_INORGANIC,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "crustacean",
                0, 0,
                BODY_NO_SPEECH | PART_HEAD | PART_EYE | PART_EYESTALKS
                    | PART_ARMS | PART_MANY_LEGS | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_HUSK_SHELL,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "amorphous_organism",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_BLOODLESS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* Fins and a tail replace limbs; the ordinary organs are still there. */
        {
                "fish",
                0, AFF_SWIM,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH
                    | PART_HEAD | PART_EYE | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FINS | PART_TAIL,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* A four-legged amphibian has ordinary organs, without mammal extras. */
        {
                "quadrupedal_amphibian",
                0, AFF_SWIM,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_4_LEGS | PART_FORELEGS | PART_FEET
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* Flight is an archetype choice; the bird body just supplies anatomy. */
        {
                "bird",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_2_LEGS | PART_FEET | PART_WINGS | PART_FEATHERS
                    | PART_BEAK | PART_TAIL | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* Dolphins and whales are mammals with flippers and flukes, not fish. */
        {
                "cetacean",
                0, AFF_SWIM,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH
                    | PART_HEAD | PART_EYE | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FINS | PART_TAIL,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* This is a living cloud, without one limb or organ to target. */
        {
                "insect_swarm",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_NO_CORPSE
                    | BODY_BLOODLESS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* Use the front of the spider's body for head and eye targeting. */
        {
                "arachnid",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_MANY_LEGS | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "insect",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_MANY_LEGS | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* A humanoid torso sits above the spider's walking legs. */
        {
                "arachnoid_humanoid",
                0, 0,
                PART_HEAD | PART_ARMS | PART_HANDS | PART_FINGERS
                    | PART_EYE | PART_EAR | PART_MANY_LEGS
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* Living plants have sap, but no animal blood or pain sensation. */
        {
                "plant",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "environmental_hazard",
                0, AFF_MINDLESS,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_HEART | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_INORGANIC
                    | BODY_SOULLESS,
                0, 0, 0,
                RES_POISON | RES_DRAIN | RES_SLEEP | RES_CHARM | RES_HOLD
                    | RES_PARALYSIS | RES_PSYCHIC,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "myriapod",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_MANY_LEGS | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* These fungi have a thinking body, without animal organs or eyes. */
        {
                "fungoid",
                0, 0,
                BODY_NO_EYES | BODY_NO_HEART | BODY_NO_SPEECH | BODY_BLOODLESS
                    | PART_HEAD | PART_ARMS | PART_HANDS | PART_FINGERS
                    | PART_2_LEGS | PART_FEET,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* The arms fold around a central mouth; there is no separate head. */
        {
                "radial_invertebrate",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_LEGS | BODY_NO_HEART
                    | BODY_NO_SPEECH | BODY_BLOODLESS
                    | PART_ARMS | PART_MANY_ARMS | PART_GUTS,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "tripodal_organism",
                0, 0,
                BODY_NO_HEAD | BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_3_LEGS | PART_FEET | PART_EYE
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "hexapodal_mammal",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_MANY_LEGS | PART_FORELEGS | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_FEET | PART_EAR | PART_EYE,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        /* A cephalopod keeps its organs; a separately targeted limb does not. */
        {
                "cephalopod",
                0, AFF_SWIM,
                BODY_NO_LEGS | BODY_NO_SPEECH | PART_HEAD | PART_EYE
                    | PART_ARMS | PART_MANY_ARMS | PART_TENTACLES
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "living_appendage",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_LEGS | BODY_NO_HEART
                    | BODY_NO_SPEECH | BODY_NO_CORPSE | PART_ARMS | PART_TENTACLES,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT, GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "multipedal_reptile",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | PART_HEAD | PART_MANY_LEGS | PART_HEART | PART_BRAINS
                    | PART_GUTS | PART_FEET | PART_EAR | PART_EYE
                    | PART_TAIL | PART_SCALES,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "geometric_being",
                0, 0,
                BODY_NO_HEAD | BODY_NO_ARMS | BODY_NO_LEGS
                    | PART_EYE | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "bivalve", 0, AFF_SWIM,
                BODY_NO_HEAD | BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH | PART_HEART | PART_GUTS | PART_EYE | PART_HUSK_SHELL, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL, MOB_SPECIAL_CHANCES_INHERIT,
                GHOST_PHASE_NONE, MOB_TEMPLATE_UNSET
        }
};

/*
 * Specific creature archetypes.
 */
/* Both dwarf variants share these defaults; training lives in trait profiles. */
#define DWARF_ARCHETYPE(name) \
        { \
                name, "humanoid", "icon1", "icon2", \
                0, AFF_INFRARED, \
                0, 0, \
                RES_MAGIC | RES_POISON, 0, 0, \
                0, 0, 0, 0, \
                130, 70, \
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, \
                "", "", "", \
                10, \
                MOB_SPECIAL_CHANCES_AUTO, \
                GHOST_PHASE_NONE, \
                CREATURE_FAMILY_DWARF, \
                MOB_TEMPLATE_UNSET \
        }

/* Both forms use the same vampire physiology, defences and active ability. */
#define VAMPIRE_ARCHETYPE(name, body_species, senses, anatomy, attacks, body_size, body_height, body_weight, body_length) \
        { \
                name, body_species, "icon1", "icon2", \
                ACT_UNDEAD | ACT_REGENERATOR, senses, \
                anatomy, attacks, \
                RES_COLD | RES_ELECTRICITY | RES_DARK | RES_PSYCHIC, \
                RES_SILVER | RES_HOLY | RES_WATER, \
                RES_POISON | RES_SLEEP | RES_CHARM | RES_CURSE | RES_NONMAGIC, \
                0, 0, 0, 0, \
                body_height, body_weight, \
                body_size, MOB_TEMPLATE_UNSET, \
                "spec_vampire", "", "", \
                0, \
                MOB_SPECIAL_CHANCES_AUTO, \
                MOB_TEMPLATE_UNSET, \
                CREATURE_FAMILY_NONE, \
                body_length \
        }

/* These toys share their enchantment; their materials can differ. */
#define ANIMATED_TOY_ARCHETYPE(name, weakness, body_weight) \
        { \
                name, "bipedal_construct", "icon1", "icon2", \
                0, AFF_MINDLESS, \
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS \
                    | BODY_ANALGIC | BODY_SOULLESS, 0, \
                0, weakness, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN, \
                0, 0, 0, 0, \
                30, body_weight, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET, \
                "", "", "", \
                0, \
                MOB_SPECIAL_CHANCES_AUTO, \
                GHOST_PHASE_NONE, \
                CREATURE_FAMILY_NONE, \
                MOB_TEMPLATE_UNSET \
        }

/* Both sizes keep the same ordinary zombie physiology. */
#define ZOMBIE_ARCHETYPE(name, anatomy, body_height, body_weight, body_size, family) \
        { \
                name, "humanoid", "icon1", "icon2", \
                ACT_UNDEAD, AFF_MINDLESS, \
                BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS | anatomy, \
                0, \
                RES_DARK, RES_HOLY, \
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_COLD | RES_HOLD | RES_PARALYSIS, \
                0, 0, 0, 0, \
                body_height, body_weight, body_size, MOB_TEMPLATE_UNSET, \
                "", "", "", \
                0, MOB_SPECIAL_CHANCES_AUTO, MOB_TEMPLATE_UNSET, family, \
                MOB_TEMPLATE_UNSET \
        }

const struct mob_type mob_table[MAX_MOB] =
{
        /*
         * name, body species, male icon, female icon
         * ACT XOR mask, AFF XOR mask
         * body-form XOR mask, attack-part XOR mask
         * resistance XOR masks
         * hp, damage, critical and haste scalar overrides
         * height, weight, size and language scalar overrides
         * default special-function overrides
         */

        {
                "reserved", "reserved", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                NULL, NULL, NULL,
                0,                        /* XP adjustment: percentage points */
                MOB_SPECIAL_CHANCES_INHERIT,
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "goat", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_HORNS | PART_HOOVES | PART_TAIL, PART_HORNS | PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                70, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 110
        },

        {
                "fire_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_LEGS | BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC
                    | PART_HEAD | PART_EYE | PART_ARMS,
                0, 0, RES_COLD | RES_WATER,
                RES_FIRE | RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN
                    | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                300, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "skeleton", "humanoid", "icon1", "icon2",

                /* Add undead classification and explicit mindlessness. */
                ACT_UNDEAD, AFF_MINDLESS,

                /*
                 * XOR against the humanoid body.
                 *
                 * Existing positive organ bits are cancelled.
                 * Previously absent BODY restrictions are added.
                 * Do not repeat the retained head/arm/leg/hand/foot bits.
                 */
                BODY_NO_EYES | BODY_NO_HEART | BODY_NO_SPEECH
                    | BODY_INORGANIC | BODY_BLOODLESS
                    | BODY_ANALGIC | BODY_SOULLESS
                    | PART_HEART | PART_BRAINS | PART_GUTS
                    | PART_EAR | PART_EYE,

                /* No additional natural attack parts. */
                0,

                /* Resistance, vulnerability and immunity XOR masks. */
                RES_PIERCE | RES_SLASH | RES_DARK,
                RES_BLUNT | RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN
                    | RES_COLD | RES_HOLD | RES_PARALYSIS,

                /* Neutral HP, damage, critical and swiftness adjustments. */
                0, 0, 0, 0,

                /* Descriptive centimetres/kilograms; language stays inherited. */
                175, 12,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /* No compulsory inherited special in any slot. */
                "", "", "",

                /* No additional archetype XP adjustment. */
                0,

                /* Empty names resolve to a valid empty 0/0/0 set. */
                MOB_SPECIAL_CHANCES_AUTO,
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        ZOMBIE_ARCHETYPE("zombie", 0, 175, 65, CHAR_SIZE_MEDIUM, CREATURE_FAMILY_NONE),
        {
                "ghoul", "humanoid", "icon1", "icon2",

                /* Undead, but capable of thought and speech. */
                ACT_UNDEAD, 0,

                /* Keep the humanoid body and add its tongue, claws and fangs. */
                PART_LONG_TONGUE | PART_CLAWS | PART_FANGS,

                /* Existing natural claw and bite attacks. */
                PART_CLAWS | PART_FANGS,

                /* No additional resistance or vulnerability. */
                0,
                0,

                /* Explicit ghoul immunities. */
                RES_SLEEP | RES_CHARM,

                /* Keep DD4's ordinary level-scaled combat defaults. */
                0, 0, 0, 0,

                /* Descriptive centimetres/kilograms; language stays inherited. */
                175, 65,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /* Feeding and natural-contact paralysis. */
                "spec_ghoul", "", "",

                /* No separate archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "ghast", "humanoid", "icon1", "icon2",

                /* Undead, with thought and speech retained. */
                ACT_UNDEAD, AFF_STENCH,

                /* The ghast shares the ghoul's tongue, claws and fangs. */
                PART_LONG_TONGUE | PART_CLAWS | PART_FANGS,

                /* Existing natural claw and bite attacks. */
                PART_CLAWS | PART_FANGS,

                /* Resistance and vulnerability XOR masks. */
                0,
                RES_COLD_IRON,

                /* Shared ghoul-family defenses. */
                RES_SLEEP | RES_CHARM,

                /* Preserve ordinary DD4 level-scaled combat defaults. */
                0, 0, 0, 0,

                /* Descriptive centimetres/kilograms; language stays inherited. */
                175, 65,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                                /* Idle feeding and stronger contact paralysis. */
                "spec_ghast", "", "",

                /* No separate archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "ghost", "humanoid", "icon1", "icon2",

                /*
                 * Permanent undead classification.
                 *
                 * Ghosts retain a mind. Neither mindlessness nor
                 * the existing player non-corporeal affect is
                 * supplied by this template.
                 */
                ACT_UNDEAD, 0,

                /*
                 * XOR against the humanoid body.
                 *
                 * Add inorganic physiology, no heart and no corpse.
                 * Cancel the inherited physical heart, brain and
                 * guts. Retain humanoid limbs and sensory capability.
                 *
                 * Removing PART_BRAINS does not imply AFF_MINDLESS.
                 * Contact and materialisation rules are separate.
                 */
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS,

                /* No additional natural attack parts. */
                0,

                /*
                 * Ordinary mundane-weapon barrier.
                 *
                 * Supported phase-aware contacts supply narrowly scoped
                 * exceptions without changing this stored immunity.
                 * Individual XOR overrides may remove it.
                 */
                0,
                0,
                RES_NONMAGIC,

                /* Neutral HP, damage, critical and swiftness adjustments. */
                0, 0, 0, 0,

                /* Use a human-sized outline; spirit weight stays unspecified. */
                175, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /* No active ghost behaviour in this stage. */
                "", "", "",

                /* No additional archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "wight", "humanoid", "icon1", "icon2",

                /*
                 * Intelligent corporeal undead.
                 *
                 * Wights retain thought, speech and ordinary humanoid
                 * physical anatomy. Life-drain behaviour is added
                 * separately rather than encoded as a passive flag.
                 */
                ACT_UNDEAD, 0,

                /* The mummified hands have real claws. */
                PART_CLAWS,

                /* Use natural claws; life drain remains a separate special. */
                PART_CLAWS,

                /*
                 * Corporeal undead baseline:
                 * resistant to dark power, vulnerable to holy power.
                 */
                RES_DARK,
                RES_HOLY,

                /*
                 * Dead tissue is immune to poison and sleep, and a wight
                 * cannot itself be life-drained.
                 */
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_COLD | RES_CHARM
                    | RES_HOLD | RES_PARALYSIS | RES_NONMAGIC,

                /* Ordinary DD4 HP, damage, critical and swiftness values. */
                0, 0, 0, 0,

                /* A lean human-sized body; language stays inherited. */
                175, 60,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /*
                 * Dedicated wight life drain. This is deliberately narrower
                 * than the legacy general undead spellcasting special.
                 */
                "spec_wight", "", "",

                /* No additional archetype XP adjustment. */
                0,

                MOB_SPECIAL_CHANCES_AUTO,

                /* Corporeal; inherit the humanoid phase default of none. */
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "wraith", "humanoid", "icon1", "icon2",

                /*
                 * Intelligent supernatural undead.
                 *
                 * Wraiths retain a mind and the ability to communicate,
                 * but have no ordinary biological body.
                 */
                ACT_UNDEAD, 0,

                /*
                 * Spirit physiology.
                 *
                 * Add inorganic/no-heart/no-corpse properties and cancel
                 * the inherited physical heart, brain and guts. Retain
                 * humanoid limbs and sensory capability.
                 */
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS,

                /* No additional natural attack parts. */
                0,

                /*
                 * Supernatural undead defenses.
                 *
                 * RES_NONMAGIC supplies the ordinary mundane-contact
                 * barrier; the existing semi-material contact resolver
                 * provides the silver exception without changing masks.
                 */
                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_NONMAGIC
                    | RES_COLD | RES_CHARM | RES_HOLD | RES_PARALYSIS,

                /* Ordinary DD4 HP, damage, critical and swiftness values. */
                0, 0, 0, 0,

                /* A human-sized outline, with no assumed spirit weight. */
                175, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /*
                 * Dedicated life-drain and live phase behaviour.
                 * Do not use the broad legacy undead caster.
                 */
                "spec_wraith", "", "",

                /* No additional archetype XP adjustment. */
                0,

                MOB_SPECIAL_CHANCES_AUTO,

                /*
                 * Start semi-material so the existing magic/silver contact
                 * rules are usable before automatic phasing is introduced.
                 */
                GHOST_PHASE_SEMI_MATERIAL,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "spectre", "humanoid", "icon1", "icon2",

                /*
                 * Intelligent supernatural undead spirit.
                 *
                 * The passive spectre profile deliberately shares the
                 * established wraith spirit physiology. Stronger spectre
                 * behaviour is supplied separately by its active special.
                 */
                ACT_UNDEAD, 0,

                /*
                 * Non-biological spirit physiology.
                 *
                 * Add inorganic/no-heart/no-corpse properties and cancel
                 * the inherited physical heart, brain and guts while
                 * retaining humanoid limbs and sensory capability.
                 */
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS,

                /* No additional natural attack parts. */
                0,

                /*
                 * Shared supernatural-spirit defensive profile.
                 */
                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_NONMAGIC
                    | RES_COLD | RES_CHARM | RES_HOLD | RES_PARALYSIS,

                /* Ordinary DD4 HP, damage, critical and swiftness values. */
                0, 0, 0, 0,

                /* Human-sized spirit; its mass remains unspecified. */
                175, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /*
                 * Stronger life drain plus live spectral phasing.
                 * Do not use the broad legacy undead caster.
                 */
                "spec_spectre", "", "",

                /* No additional archetype XP adjustment. */
                0,

                MOB_SPECIAL_CHANCES_AUTO,

                /* Start with the existing semi-material contact rules. */
                GHOST_PHASE_SEMI_MATERIAL,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "mummy", "humanoid", "icon1", "icon2",
                /* A dried body still has organs, a mind and physical remains. */
                ACT_UNDEAD, 0, BODY_BLOODLESS | BODY_ANALGIC, 0,
                RES_DARK | RES_BLUNT | RES_PIERCE | RES_SLASH,
                RES_HOLY | RES_FIRE,
                RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN
                    | RES_COLD | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                175, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                /* The existing special supplies fear and contact rot. */
                "spec_mummy", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "banshee", "humanoid", "icon1", "icon2",

                ACT_UNDEAD, 0,

                /* Spirit body; the PART_* bits remove humanoid organs. */
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS,

                0,

                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_NONMAGIC
                    | RES_COLD | RES_ELECTRICITY | RES_CHARM | RES_HOLD,

                0, 0, 0, 0,

                175, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                "spec_banshee", "", "",

                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "death_knight", "humanoid", "icon1", "icon2",
                /* The knight keeps his mind and voice, without living circulation. */
                ACT_UNDEAD | ACT_FEAR_AURA, 0,
                BODY_NO_HEART | BODY_BLOODLESS | PART_HEART | PART_BRAINS | PART_GUTS,
                0, RES_DARK | RES_MAGIC, RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM | RES_HOLD,
                0, 0, 0, 0,
                185, 140, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_death_knight", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        VAMPIRE_ARCHETYPE("vampire", "humanoid", 0, PART_FANGS, 0,
            CHAR_SIZE_MEDIUM, 175, 75, MOB_TEMPLATE_UNSET),
        {
                "lich", "humanoid", "icon1", "icon2",

                ACT_UNDEAD | ACT_FEAR_AURA, AFF_INFRARED,

                /* Remove the humanoid organs; keep speech and limbs. */
                BODY_NO_HEART | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0,

                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM
                    | RES_COLD | RES_ELECTRICITY | RES_NONMAGIC
                    | RES_HOLD | RES_PARALYSIS,

                0, 0, 0, 0,

                175, 45,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,

                /* Casts at its current opponent. */
                "spec_lich", "", "",

                10,
                MOB_SPECIAL_CHANCES_AUTO,
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },
        {
                "dracolich", "dragon", "icon1", "icon2",

                ACT_UNDEAD | ACT_FEAR_AURA, AFF_FLYING,

                BODY_HUGE | BODY_NO_HEART | BODY_NO_EYES | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS | PART_EYE,
                0,

                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM
                    | RES_COLD | RES_ELECTRICITY | RES_HOLD
                    | RES_PARALYSIS | RES_NONMAGIC,

                0, 0, 0, 0,

                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,

                "spec_dracolich", "spec_breath_any", "",

                0,
                {70, 30, 0},
                MOB_TEMPLATE_UNSET,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        /* Reviewed creatures and the remaining target-family placeholders. */
        DWARF_ARCHETYPE("dwarf"),
        DWARF_ARCHETYPE("dwarf_untrained"),
        {
                "orc", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                175, 85,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_ORC,
                MOB_TEMPLATE_UNSET
        },
        {
                "half_orc", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                175, 80,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_HALF_ORC,
                MOB_TEMPLATE_UNSET
        },
        {
                "goblin", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                120, 30,
                CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_GOBLIN,
                MOB_TEMPLATE_UNSET
        },
        {
                "hobgoblin", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                200, 100,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_HOBGOBLIN,
                MOB_TEMPLATE_UNSET
        },
        {
                "ogre", "humanoid", "icon1", "icon2",
                0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                290, 160, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_OGRE, MOB_TEMPLATE_UNSET
        },
        /* Native healing is passive; the existing giant special throws rocks. */
        {
                "troll", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                275, 225, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_giant", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_TROLL, MOB_TEMPLATE_UNSET
        },
        {
                "ogre_mage", "humanoid", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_OGRE_MAGE,
                MOB_TEMPLATE_UNSET
        },
        {
                "giant", "humanoid", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_GIANT,
                MOB_TEMPLATE_UNSET
        },
        {
                "titan", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE, 0,
                RES_MAGIC, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                760, 9000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_TITAN, MOB_TEMPLATE_UNSET
        },
        {
                "human", "humanoid", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                175, 75,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_HUMAN,
                MOB_TEMPLATE_UNSET
        },

        {
                "giant_lizard", "quadrupedal_reptile", "icon1", "icon2",
                0, 0,
                PART_FANGS | PART_CLAWS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                457
        },

        {
                "giant_pit_viper", "serpentine_reptile", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                366
        },

        {
                "bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "animated_mist", "mysterious_being", "icon1", "icon2",
                0, AFF_SANCTUARY | AFF_SNEAK | AFF_PASS_DOOR | AFF_FLYING,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "mysterious_being", "mysterious_being", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "shadow", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, AFF_NON_CORPOREAL | AFF_INFRARED | AFF_SNEAK | AFF_PASS_DOOR | AFF_DETECT_HIDDEN,
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_NO_SPEECH | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS, 0,
                RES_DARK, RES_HOLY, RES_COLD | RES_POISON | RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_NONMAGIC,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        /* The large-bat form has a three-foot body; 10 kg is our estimate. */
        VAMPIRE_ARCHETYPE("vampire_bat_form", "winged_mammal", AFF_FLYING,
            PART_FANGS, PART_FANGS, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET, 10, 91),

        {
                "prism_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_HUGE, 0,
                0, 0, 0,
                0, 0, 0, 0,
                300, 3000, /* Shoulder height in cm and estimated weight in kg. */
                CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_any", "spec_prism_flash", "",
                0,
                {75, 25, 0},
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                1200 /* Snout to tail tip in cm. */
        },

        {
                "darkenbeast", "winged_reptile", "icon1", "icon2",
                0, AFF_FLYING,
                PART_FANGS | PART_CLAWS, PART_FANGS | PART_CLAWS,
                0, 0, RES_CHARM,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                120
        },

        ANIMATED_TOY_ARCHETYPE("animated_soft_toy", RES_FIRE, 1),
        ANIMATED_TOY_ARCHETYPE("animated_toy_soldier", 0, 2),

        {
                "wyvern", "winged_reptile", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_HUGE | PART_TAIL | PART_FANGS | PART_CLAWS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, /* Keep the old poison special's XP allowance. */
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                1070
        },

        ZOMBIE_ARCHETYPE("giant_zombie", BODY_HUGE, 450, 700,
            CHAR_SIZE_LARGE, CREATURE_FAMILY_GIANT),

        {
                "dog", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                50, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                90
        },


        {
                "elf", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                150, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_ELF,
                MOB_TEMPLATE_UNSET
        },

        /* The trophy can bite back, but has no living body to sustain. */
        {
                "preserved_head", "head_only", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_INORGANIC
                    | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS | PART_FANGS,
                PART_FANGS,
                0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                30, 5, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        /* Colour-specific tongue loot stays in the individual area resets. */
        {
                "lizard", "quadrupedal_reptile", "icon1", "icon2",
                0, 0,
                PART_FANGS | PART_CLAWS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                8, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                46
        },

        {
                "lizardman", "humanoid", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_SCALES | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                200, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_LIZARDMAN,
                MOB_TEMPLATE_UNSET
        },

        {
                "duergar", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED | AFF_SNEAK,
                0, 0,
                RES_MAGIC, 0, RES_POISON | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                120, 55, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_duergar", "", "",
                5, /* With the special's +10, the whole profile adds +15 XP. */
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_DWARF,
                MOB_TEMPLATE_UNSET
        },

        {
                "rock_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                150, 8000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_swallow", "", "",
                0, /* Swallow supplies the profile's +10 XP. */
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                800
        },

        /* The mirror gives the reflection a mind, but no living organs. */
        {
                "manifested_double", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_HEART | BODY_INORGANIC | BODY_NO_CORPSE
                    | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS
                    | PART_HEART | PART_BRAINS | PART_GUTS,
                0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "wooden_automaton", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS
                    | BODY_ANALGIC | BODY_SOULLESS,
                0,
                0, RES_FIRE, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                175, 50, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        /* This is an active guardian, rather than an undead monk. */
        {
                "animated_relic", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS
                    | BODY_ANALGIC | BODY_SOULLESS,
                0,
                0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                175, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                MOB_TEMPLATE_UNSET
        },

        {
                "black_dragon_hatchling", "dragon", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                0, PART_TAILATTACK,
                0, 0, RES_ACID,
                0, 0, 0, 0,
                60, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_acid", "", "",
                10,
                MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE,
                250
        },

        {
                "young_lightning_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                0, PART_TAILATTACK,
                0, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                120, 400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_lightning", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 550
        },

        {
                "animated_vase", "limbless_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0,
                0, RES_BLUNT, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                40, 3, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },

        {
                "animated_blade", "limbless_construct", "icon1", "icon2",
                0, AFF_MINDLESS | AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0,
                0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 50
        },

        {
                "animated_mirror", "limbless_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0,
                0, RES_BLUNT, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                100, 15, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },

        {
                "cave_beast", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE | BODY_NO_SPEECH | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                300, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },

        /* These living hybrids keep their own defaults, rather than stacking parents. */
        {
                "human_goblin", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                150, 50, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GOBLIN, MOB_TEMPLATE_UNSET
        },

        {
                "centaur", "centauroid_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_HOOVES, PART_HOOVES,
                0, 0, 0,
                0, 0, 0, 0,
                260, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 250
        },
        {
                "domestic_cat", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED | AFF_SNEAK,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                30, 4, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 50
        },
        /* This invisible actor runs room scripts rather than living in the world. */
        {
                "script_controller", "mysterious_being", "icon1", "icon2",
                ACT_NO_QUEST | ACT_NO_FIGHT, AFF_MINDLESS,
                BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* Keep the speaking head and four-legged body distinct from a lion. */
        {
                "manticore", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | BODY_HUGE | PART_WINGS | PART_TAIL
                    | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                183, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_manticore", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 457
        },
        /* This descriptive human variant adds wings, not celestial powers. */
        {
                "winged_human", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                175, 85, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_HUMAN, MOB_TEMPLATE_UNSET
        }

        ,
        /* This small bat can talk, but still has wings instead of arms. */
        {
                "talking_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | PART_FANGS | PART_CLAWS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 10
        },
        /* An unseen magical presence need not be a dead person's ghost. */
        {
                "magical_spirit", "mysterious_being", "icon1", "icon2",
                0, AFF_INVISIBLE,
                BODY_NO_SPEECH | BODY_ANALGIC, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        }

        ,
        /* An alhoon keeps its brain and psionics, without human-lich powers. */
        {
                "alhoon", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, AFF_INFRARED,
                BODY_BLOODLESS | PART_TENTACLES, 0,
                RES_MAGIC, 0, RES_SLEEP | RES_CHARM,
                0, 0, 0, 0,
                183, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_alhoon", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* Occupations and alignment still belong to the individual drow. */
        {
                "drow", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0, 0,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                150, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_ELF, MOB_TEMPLATE_UNSET
        },
        /* A giant worm can swallow prey without gaining a purple worm's venom. */
        {
                "giant_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                70, 1000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_swallow", "", "",
                0, /* The existing Swallow special supplies its XP adjustment. */
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 460
        },
        /* Rabbits have incisors; their attached teeth supply the ordinary bite. */
        {
                "rabbit", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_PAWS | PART_CLAWS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                25, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 40
        },
        /* Colour-specific powers and individual casting remain separate choices. */
        {
                "dragon_hatchling", "dragon", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                0, PART_TAILATTACK,
                0, 0, 0,
                0, 0, 0, 0,
                91, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 250
        },
        /* A faerie dragon keeps its bite, while its special handles its tricks. */
        {
                "faerie_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                0, PART_CLAWS | PART_TAILATTACK,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                20, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_faerie_dragon", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 61
        },
        /* A wild griffin has talons and a beak; training is an individual choice. */
        {
                "griffin", "quadrupedal_hybrid", "icon1", "icon2",
                0, AFF_FLYING | AFF_DETECT_HIDDEN,
                BODY_NO_SPEECH | PART_WINGS | PART_FEATHERS | PART_TAIL
                    | PART_PAWS | PART_CLAWS | PART_BEAK,
                PART_CLAWS | PART_BEAK,
                0, 0, 0,
                0, 0, 0, 0,
                152, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 275
        },
        /* Cattle and goats share ordinary mammal anatomy. */
        {
                "cattle", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_HORNS | PART_HOOVES, PART_HORNS | PART_HOOVES,
                0, 0, 0,
                0, 0, 0, 0,
                150, 650, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 250
        },
        /* Native rock supplies and catches belong to the reviewed giant types. */
        {
                "hill_giant", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE, 0,
                0, 0, 0,
                0, 0, 0, 0,
                490, 2050, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_giant", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GIANT, MOB_TEMPLATE_UNSET
        },
        {
                "stone_giant", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE, 0,
                0, 0, 0,
                0, 0, 0, 0,
                550, 4100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_giant", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GIANT, MOB_TEMPLATE_UNSET
        },
        /* An infant shares troll physiology but has no active rock special. */
        {
                "troll_infant", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                140, 80, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_TROLL, MOB_TEMPLATE_UNSET
        },
        /* These furred den animals have an ordinary bite, without new senses. */
        {
                "den_beast", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                90, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        /* XOR removes positive inherited head/eye bits as well as marking absence. */
        {
                "carrion_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0,
                PART_HEAD | PART_EYE | BODY_NO_HEAD | BODY_NO_EYES, 0,
                0, 0, 0,
                0, 0, 0, 0,
                10, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_fido", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        /* Gnome Village adds reviewed bodies; the last three rows are target identities only. */
        {
                "gnome", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0, 0,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                105, 40, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "gnome_untrained", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0, 0,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                105, 40, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "mouse", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                3, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 10
        },
        {
                "rat", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                8, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 20
        },
        {
                "fire_bat", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED,
                BODY_NO_ARMS | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_ANALGIC
                    | PART_HEAD | PART_EYE | PART_2_LEGS | PART_WINGS | PART_FANGS, PART_FANGS,
                0, 0, RES_FIRE,
                0, 0, 0, 0,
                30, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "",
                10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 61
        },
        {
                "giant_hermit_crab", "crustacean", "icon1", "icon2",
                0, 0,
                PART_CLAWS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                152, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 250
        },
        {
                "kobold", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_SCALES | PART_HORNS | PART_TAIL, 0,
                0, 0, 0,
                0, 0, 0, 0,
                91, 15, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_KOBOLD, MOB_TEMPLATE_UNSET
        },
        {
                "gnoll", "humanoid", "icon1", "icon2",
                0, 0, PART_TAIL | PART_FANGS, 0,
                0, 0, 0, 0, 0, 0, 0,
                230, 130, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GNOLL, MOB_TEMPLATE_UNSET
        },
        {
                "bugbear", "humanoid", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_BUGBEAR, MOB_TEMPLATE_UNSET
        },
        /* These profiles keep ordinary attacks separate from active powers. */
        {
                "snake", "serpentine_reptile", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 120
        },
        {
                "wererat", "humanoid", "icon1", "icon2",
                0, 0, PART_TAIL | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, RES_NONMAGIC, 0, 0, 0, 0,
                150, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "firedrake", "dragon", "icon1", "icon2",
                0, AFF_FLYING, BODY_NO_SPEECH, PART_CLAWS | PART_TAILATTACK,
                RES_FIRE, 0, 0, 0, 0, 0, 0,
                65, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_firedrake", "", "", 10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 122
        },
        {
                "green_ooze", "amorphous_organism", "icon1", "icon2",
                0, AFF_MINDLESS, 0, 0,
                0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                30, 80, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        /* The upper eye/mouth region uses ordinary head targeting. */
        {
                "yochlol", "amorphous_organism", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_SPEECH
                    | BODY_NO_CORPSE | PART_HEAD | PART_EYE | PART_ARMS
                    | PART_MANY_ARMS | PART_TENTACLES,
                0,
                RES_MAGIC | RES_COLD | RES_FIRE | RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0, 0, 0,
                180, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* Deer keep clean mammal anatomy; antlers use the existing horn part. */
        {
                "deer", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HOOVES, PART_HOOVES,
                0, 0, 0, 0, 0, 0, 0,
                100, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 160
        },
        {
                "stag", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HORNS | PART_HOOVES,
                PART_HORNS | PART_HOOVES,
                0, 0, 0, 0, 0, 0, 0,
                130, 180, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 210
        },
        {
                "eel", "fish", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 5, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        /* This is the temple's acid-trunked guardian, with ordinary living anatomy. */
        {
                "guardian_daemon", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_CLAWS | PART_TRUNK, PART_CLAWS,
                RES_MAGIC | RES_NONMAGIC, 0,
                RES_ACID | RES_CHARM | RES_HOLD | RES_SLEEP,
                0, 0, 0, 0,
                366, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_guardian_daemon", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* This ordinary pond frog keeps its tongue as anatomy, not a new attack. */
        {
                "frog", "quadrupedal_amphibian", "icon1", "icon2",
                0, 0, PART_LONG_TONGUE, 0,
                0, 0, 0, 0, 0, 0, 0,
                5, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 10
        },
        {
                "seagull", "bird", "icon1", "icon2",
                0, AFF_FLYING, 0, PART_BEAK,
                0, 0, 0, 0, 0, 0, 0,
                25, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 45
        },
        /* Teeth are anatomy here; the ordinary attack uses the tail. */
        {
                "dolphin", "cetacean", "icon1", "icon2",
                0, 0, PART_FANGS, PART_TAILATTACK,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_small_whale", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        /* DD4's blue whale keeps the existing swallowing special. */
        {
                "blue_whale", "cetacean", "icon1", "icon2",
                0, 0, BODY_HUGE, PART_TAILATTACK,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 100000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_large_whale", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 2400
        },
        {
                "hammerhead_shark", "fish", "icon1", "icon2",
                0, 0, PART_SCALES | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                "small_fish", "fish", "icon1", "icon2",
                0, 0, PART_SCALES, 0,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 20
        },
        {
                "brown_bear", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                150, 450, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_bear", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 280
        },
        {
                "mosquito_swarm", "insect_swarm", "icon1", "icon2",
                0, AFF_FLYING, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "wasp_swarm", "insect_swarm", "icon1", "icon2",
                0, AFF_FLYING, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "badger", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                30, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 70
        },
        {
                "robin", "bird", "icon1", "icon2",
                0, AFF_FLYING, 0, PART_BEAK,
                0, 0, 0, 0, 0, 0, 0,
                7, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 10
        },
        {
                "lynx", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED | AFF_SNEAK,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                55, 20, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 100
        },
        {
                "fox", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                40, 7, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 100
        },
        {
                "wolf", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                80, 50, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        /* Wargs have their own simple speech; being wild does not make a mount. */
        {
                "warg", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                BODY_NO_SPEECH | PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                100, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 210
        },
        /* Their sturdy saves and elusive movement use ordinary DD4 defences. */
        {
                "brownie", "humanoid", "icon1", "icon2",
                0, AFF_HIDE | AFF_INVISIBLE | AFF_DETECT_HIDDEN,
                0, 0, RES_MAGIC, 0, 0, 0, 0, 0, 0,
                61, 9, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_brownie", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* This adult profile leaves younger green dragons free to differ. */
        {
                "adult_green_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_SWIM | AFF_INFRARED
                    | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE | PART_HORNS, 0, RES_MAGIC, 0, RES_POISON,
                0, 0, 0, 0,
                400, 8000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_gas", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 3200
        },
        {
                "web_spider", "arachnid", "icon1", "icon2",
                0, 0,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                35, 15, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 70
        },
        {
                "hunting_spider", "arachnid", "icon1", "icon2",
                0, 0,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                45, 30, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 90
        },
        {
                "giant_spider", "arachnid", "icon1", "icon2",
                0, 0,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                90, 120, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        {
                "venomous_spider", "arachnid", "icon1", "icon2",
                0, 0,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                65, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "phase_spider", "arachnid", "icon1", "icon2",
                ACT_BLINK, 0,
                PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                70, 85, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 160
        },
        {
                "giant_wasp", "insect", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS, 0,
                0, 0, 0,
                0, 0, 0, 0,
                60, 35, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "spider_empress", "arachnoid_humanoid", "icon1", "icon2",
                0, 0,
                0, 0,
                0, 0, 0,
                0, 0, 0, 0,
                220, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_gas", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 280
        },
        {
                "quasit", "humanoid", "icon1", "icon2",
                ACT_BLINK | ACT_REGENERATOR, 0,
                PART_HORNS | PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                RES_MAGIC | RES_NONMAGIC, 0, RES_FIRE | RES_COLD | RES_ELECTRICITY,
                0, 0, 0, 0,
                61, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "ki_rin", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | BODY_HUGE | PART_TAIL | PART_SCALES
                    | PART_HOOVES | PART_HORNS, PART_HOOVES | PART_HORNS,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                235, 950, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 400
        },
        {
                "wormkin", "serpentine_reptile", "icon1", "icon2",
                0, AFF_FLYING,
                PART_FANGS, PART_FANGS | PART_TAILATTACK,
                0, 0, 0,
                0, 0, 0, 0,
                60, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_gas", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 230
        },
        {
                "harpy", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FEATHERS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                180, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* A moving cloud has no fixed head or limbs. */
        {
                "air_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING | AFF_PASS_DOOR,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_ANALGIC,
                0, RES_NONMAGIC, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "efreeti", "humanoid", "icon1", "icon2",
                0, AFF_FLYING, BODY_HUGE, 0, RES_FIRE, 0, 0,
                0, 0, 0, 0,
                366, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "roc", "bird", "icon1", "icon2",
                0, AFF_FLYING, BODY_HUGE | PART_CLAWS,
                PART_CLAWS | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                250, 650, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 450
        },
        {
                "green_hag", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED, PART_CLAWS, PART_CLAWS, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                165, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "black_bear", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                100, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_bear", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        {
                "hydra", "quadrupedal_reptile", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_MANY_HEAD | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                250, 3000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 900
        },
        {
                "imp", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_GOOD | AFF_DETECT_MAGIC,
                PART_HORNS | PART_WINGS | PART_FANGS | PART_TAIL, PART_FANGS,
                RES_MAGIC | RES_NONMAGIC, 0, RES_FIRE | RES_COLD | RES_ELECTRICITY,
                0, 0, 0, 0,
                61, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "valley_elf", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                175, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_ELF, MOB_TEMPLATE_UNSET
        },
        {
                "cooshee", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_SNEAK,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                80, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cooshee", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "antelope", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HORNS | PART_HOOVES,
                PART_HORNS | PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                100, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 160
        },
        {
                "bee", "insect", "icon1", "icon2",
                0, AFF_FLYING, PART_WINGS, 0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_poison", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 2
        },
        {
                "enchanted_oak", "plant", "icon1", "icon2",
                0, 0, BODY_NO_ARMS | PART_ARMS | BODY_HUGE, 0, 0, RES_FIRE, 0,
                0, 0, 0, 0,
                1500, 10000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "leprechaun", "humanoid", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN, 0, 0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                61, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "horse", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HOOVES, PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                150, 450, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 230
        },
        {
                "polar_bear", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, RES_COLD, 0, 0,
                0, 0, 0, 0,
                200, 900, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_bear", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 400
        },
        {
                "adult_white_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE, 0, RES_MAGIC, 0, RES_COLD,
                0, 0, 0, 0,
                350, 6000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_frost_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 3000
        },
        {
                "adult_blue_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE | PART_HORNS, 0, RES_MAGIC, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                500, 12000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_lightning_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 4000
        },
        {
                "frost_giant", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE, 0, 0, 0, RES_COLD,
                0, 0, 0, 0,
                640, 3600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_giant", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GIANT, MOB_TEMPLATE_UNSET
        },
        {
                "baaz_draconian", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | PART_SCALES | PART_WINGS | PART_TAIL
                    | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                168, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "barracuda", "fish", "icon1", "icon2",
                0, 0, PART_SCALES | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 120
        },
        /* The local mute turtle keeps its own NO_SPEECH override. */
        {
                "dragon_turtle", "quadrupedal_reptile", "icon1", "icon2",
                0, 0, BODY_NO_SPEECH | BODY_HUGE | PART_CLAWS
                    | PART_FANGS | PART_HUSK_SHELL,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                350, 25000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_steam_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1200
        },
        /* These wingless lair dwellers keep their own authored behaviour. */
        {
                "lair_gremlin", "humanoid", "icon1", "icon2",
                0, 0, PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                46, 6, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* Water and midnight transformations are still just local flavour. */
        {
                "mogwai", "humanoid", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                30, 3, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        /* A stuffed head has neither a living body nor sharp teeth. */
        {
                "animated_toy_head", "head_only", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_INORGANIC
                    | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS,
                0, 0, RES_FIRE, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                15, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "puma", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED | AFF_SNEAK,
                PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                75, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        /* Venom stays opt-in; the school's introductory cobra only bites. */
        {
                "cobra", "serpentine_reptile", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 3, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        {
                "wild_boar", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_TUSKS | PART_HOOVES, PART_TUSKS,
                0, 0, 0, 0, 0, 0, 0,
                90, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 160
        },
        {
                "halfling", "humanoid", "icon1", "icon2",
                0, AFF_SNEAK, 0, 0, RES_MAGIC | RES_POISON, 0, 0,
                0, 0, 0, 0,
                91, 30, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_HALFLING, MOB_TEMPLATE_UNSET
        },
        {
                "halfling_untrained", "humanoid", "icon1", "icon2",
                0, AFF_SNEAK, 0, 0, RES_MAGIC | RES_POISON, 0, 0,
                0, 0, 0, 0,
                91, 30, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_HALFLING, MOB_TEMPLATE_UNSET
        },
        {
                "pig", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HOOVES | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                75, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 130
        },
        /* Wings describe the bird; brief fluttering is not full aerial flight. */
        {
                "chicken", "bird", "icon1", "icon2",
                0, 0, 0, PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                40, 3, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 50
        },
        {
                "gold_dragon_kin", "humanoid", "icon1", "icon2",
                0, 0, PART_SCALES | PART_FANGS, PART_FANGS,
                RES_FIRE | RES_POISON, 0, 0,
                0, 0, 0, 0,
                190, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_gold_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "ancient_gold_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_SWIM | AFF_INFRARED
                    | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE, 0, RES_MAGIC, 0, RES_FIRE | RES_POISON,
                0, 0, 0, 0,
                700, 40000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_gold_anatomical", "spec_cast_mage", "", 0,
                { 70, 30, 0 }, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 6000
        },
        {
                "pigeon", "bird", "icon1", "icon2",
                0, AFF_FLYING, 0, PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                15, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "fire_hazard", "environmental_hazard", "icon1", "icon2",
                0, 0, 0, 0, 0, RES_COLD | RES_WATER, RES_FIRE,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM,
                MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "smoke_hazard", "environmental_hazard", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, RES_FIRE,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM,
                MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "lion", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                110, 190, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 280
        },
        {
                "elephant", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_TUSKS | PART_TRUNK,
                PART_TUSKS | PART_FORELEGS, 0, 0, 0,
                0, 0, 0, 0,
                330, 5500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_elephant", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 650
        },
        {
                "constrictor_snake", "serpentine_reptile", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 70, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_constrictor", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 600
        },
        {
                "centipede", "myriapod", "icon1", "icon2",
                0, 0, PART_FANGS | PART_HUSK_SHELL, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 15
        },
        {
                "lightning_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING, PART_FANGS, PART_FANGS, 0, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_lightning_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "fire_breathing_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING, PART_FANGS, PART_FANGS, 0, 0, RES_FIRE,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "gas_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING, PART_FANGS, PART_FANGS, 0, 0, RES_POISON,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_gas_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "frost_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING, PART_FANGS, PART_FANGS, 0, 0, RES_COLD,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_frost_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "acid_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING, PART_FANGS, PART_FANGS, 0, 0, RES_ACID,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_acid_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 30
        },
        {
                "giant_electric_eel", "fish", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS, 0, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 30, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_electric_eel", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                "giant_rat", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_SWIM, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                30, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 60
        },
        {
                "animated_armour", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS
                    | BODY_ANALGIC | BODY_SOULLESS,
                0, 0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                175, 35, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "shadow_fiend", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_INORGANIC | BODY_NO_HEART | BODY_NO_CORPSE
                    | PART_HEART | PART_BRAINS | PART_GUTS
                    | PART_CLAWS | PART_FANGS | PART_WINGS,
                PART_CLAWS | PART_FANGS, 0, 0,
                RES_FIRE | RES_COLD | RES_ELECTRICITY,
                0, 0, 0, 0,
                183, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "macaw", "bird", "icon1", "icon2",
                0, AFF_FLYING, PART_CLAWS, PART_BEAK,
                0, 0, 0, 0, 0, 0, 0,
                40, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 90
        },
        {
                "raven", "bird", "icon1", "icon2",
                0, AFF_FLYING, PART_CLAWS, PART_BEAK | PART_CLAWS,
                0, 0, 0, 0, 0, 0, 0,
                25, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 65
        },
        {
                "hawk", "bird", "icon1", "icon2",
                0, AFF_FLYING, PART_CLAWS, PART_BEAK | PART_CLAWS,
                0, 0, 0, 0, 0, 0, 0,
                30, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 60
        },
        {
                "giant_falcon", "bird", "icon1", "icon2",
                0, AFF_FLYING, PART_CLAWS, PART_BEAK | PART_CLAWS,
                0, 0, 0, 0, 0, 0, 0,
                80, 25, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "lemming_folk", "humanoid", "icon1", "icon2",
                0, 0, PART_TAIL | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                70, 15, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "bear_wolf", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                120, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 240
        },
        {
                "jakyra", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_PAWS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                100, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_claw", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        {
                "bile_demon", "humanoid", "icon1", "icon2",
                0, 0, PART_HORNS, PART_HORNS, 0, 0, 0, 0, 0, 0, 0,
                220, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "mastodon", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_TUSKS | PART_TRUNK,
                PART_TUSKS | PART_FORELEGS, 0, 0, 0, 0, 0, 0, 0,
                305, 6000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_elephant", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 700
        },
        {
                "small_scorpion", "arachnid", "icon1", "icon2",
                0, 0, PART_HUSK_SHELL | PART_TAIL | PART_CLAWS, PART_CLAWS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 5, /* The attached stinger uses ordinary Venom. */
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 20
        },
        {
                "purple_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 12000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_swallow", "", "", 5, /* Venom; Swallow adds its own XP once. */
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1500
        },
        {
                "adult_brass_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE, 0, RES_MAGIC, 0, RES_FIRE,
                0, 0, 0, 0,
                300, 6000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_brass", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 2400
        },
        {
                "drider", "arachnoid_humanoid", "icon1", "icon2",
                0, AFF_INFRARED, PART_FANGS, PART_FANGS,
                RES_MAGIC, 0, 0, 0, 0, 0, 0,
                200, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "spec_paralysing_bite", "", 5,
                {70, 30, 0}, GHOST_PHASE_NONE,
                CREATURE_FAMILY_ELF, 250
        },
        {
                "myconid", "fungoid", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                183, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_myconid_spores", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "dustdigger", "radial_invertebrate", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                40, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_engulf", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                "camel", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_FANGS, PART_FANGS | PART_FORELEGS,
                0, 0, 0, 0, 0, 0, 0,
                210, 550, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_camel_spit", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                "rolling_boulder", "limbless_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0,
                0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_DRAIN,
                0, 0, 0, 0,
                150, 4000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "carnivorous_grass", "plant", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                /* Its bulb has a mouth, and the plant walks on many little legs. */
                BODY_NO_HEAD | BODY_NO_LEGS | PART_HEAD | PART_MANY_LEGS | PART_FANGS,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                60, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_fido", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 80
        },
        {
                "giant_frog", "quadrupedal_amphibian", "icon1", "icon2",
                0, 0, PART_LONG_TONGUE, 0, 0, 0, 0,
                0, 0, 0, 0,
                60, 110, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        {
                "sheep", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_TAIL | PART_HOOVES, PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                80, 70, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 120
        },
        {
                "plesiosaur", "quadrupedal_reptile", "icon1", "icon2",
                0, AFF_SWIM,
                /* Its four swimming limbs have fins rather than ordinary feet. */
                PART_FEET | PART_FINS | PART_FANGS | BODY_HUGE,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 5000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1500
        },
        {
                "giant_leech", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_SWIM,
                /* Keep the area author's absence of one discrete, targetable heart. */
                BODY_NO_HEART | PART_HEART, 0, 0, 0, 0, 0, 0, 0, 0,
                10, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_bloodsucker", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 100
        },
        {
                "ancient_fire_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE, 0, RES_MAGIC, 0, RES_FIRE,
                0, 0, 0, 0,
                700, 30000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_any_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 4500
        },
        {
                "fiery_demon", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_TAIL | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS | PART_TAILATTACK,
                RES_MAGIC | RES_COLD, 0, RES_FIRE | RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                240, 180, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "hell_hound", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED, PART_PAWS | PART_FANGS | PART_TAIL,
                PART_FANGS, 0, 0, RES_FIRE, 0, 0, 0, 0,
                90, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 150
        },
        {
                "acid_hell_hound", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED, PART_HOOVES | PART_FANGS | PART_TAIL,
                PART_FANGS, 0, 0, RES_FIRE, 0, 0, 0, 0,
                100, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        {
                "baby_beholder", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING, PART_EYESTALKS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                45, 12, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 45
        },
        {
                "tame_lamia", "centauroid_mammal", "icon1", "icon2",
                0, 0, PART_PAWS | PART_CLAWS | PART_TAIL, 0,
                0, 0, 0, 0, 0, 0, 0,
                150, 140, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 210
        },
        {
                "stone_golem", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS,
                0, RES_MAGIC, 0,
                RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM
                    | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                290, 900, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_slow", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "beholder", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING, BODY_NO_SPEECH | PART_EYESTALKS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                180, 250, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_beholder", "", "", 15,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "lamia", "centauroid_mammal", "icon1", "icon2",
                0, 0, PART_PAWS | PART_CLAWS | PART_TAIL, 0,
                0, 0, 0, 0, 0, 0, 0,
                150, 140, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_lamia", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 210
        },
        {
                "mimic", "amorphous_organism", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_EYES | BODY_NO_HEART | BODY_NO_SPEECH | BODY_BLOODLESS
                    | PART_EYE | PART_HEART | PART_BRAINS | PART_GUTS | PART_TENTACLES,
                0, 0, 0, RES_ACID, 0, 0, 0, 0,
                90, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_mimic", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 240
        },
        {
                "awakened_golem", "bipedal_construct", "icon1", "icon2",
                0, 0, 0, 0,
                RES_MAGIC, 0, RES_POISON | RES_DRAIN,
                0, 0, 0, 0,
                240, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "four_armed_demon", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED, PART_MANY_ARMS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS,
                RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                244, 220, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "flesh_golem", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_INORGANIC | BODY_NO_SPEECH | BODY_BLOODLESS
                    | BODY_ANALGIC | BODY_SOULLESS | PART_BRAINS | PART_GUTS,
                0, RES_MAGIC, 0,
                RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM
                    | RES_HOLD | RES_PARALYSIS | RES_FIRE | RES_COLD | RES_ELECTRICITY,
                0, 0, 0, 0,
                230, 160, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 15,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "grell", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED,
                BODY_NO_ARMS | PART_MANY_ARMS | PART_TENTACLES | PART_BEAK, PART_BEAK,
                0, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                300, 150, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_grell", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 120
        },
        {
                "crocodile", "quadrupedal_reptile", "icon1", "icon2",
                0, AFF_SWIM, PART_CLAWS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                45, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                "flind", "humanoid", "icon1", "icon2",
                0, 0, PART_TAIL | PART_FANGS, 0,
                0, 0, 0, 0, 0, 0, 0,
                198, 140, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_flind", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GNOLL, MOB_TEMPLATE_UNSET
        },
        {
                "otyugh", "tripodal_organism", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_HEAD | BODY_NO_ARMS | PART_HEAD | PART_MANY_ARMS | PART_TENTACLES
                    | PART_EYESTALKS | PART_FANGS,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                140, 400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_otyugh", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 210
        },
        {
                "treant", "plant", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | BODY_NO_SPEECH | BODY_HUGE | PART_HEAD | PART_EYE | PART_EAR
                    | PART_ARMS | PART_2_LEGS | PART_HANDS | PART_FEET | PART_FINGERS,
                0, 0, RES_FIRE, 0,
                0, 0, 0, 0,
                450, 2000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "beaked_cave_beast", "humanoid", "icon1", "icon2",
                0, 0, BODY_NO_SPEECH | PART_BEAK | PART_CLAWS,
                PART_BEAK | PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                230, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_acid_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "mistling", "mysterious_being", "icon1", "icon2",
                0, AFF_FLYING, BODY_ANALGIC, 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                "yeti", "humanoid", "icon1", "icon2",
                0, 0, BODY_NO_SPEECH | PART_CLAWS | PART_FANGS,
                PART_CLAWS, RES_COLD, RES_FIRE, 0,
                0, 0, 0, 0,
                244, 136, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "ice_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_FANGS, PART_FANGS,
                RES_COLD, 0, 0,
                0, 0, 0, 0,
                150, 12000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_frost_anatomical", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1500
        },
        {
                "minotaur", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED, PART_HORNS | PART_HOOVES, PART_HORNS,
                0, 0, 0,
                0, 0, 0, 0,
                230, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_horn_gore", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "mist_hazard", "environmental_hazard", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "rock_hazard", "environmental_hazard", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "enchanted_chamber", "environmental_hazard", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "adult_crystal_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS, BODY_HUGE, 0, RES_COLD | RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                300, 5000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_crystal", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 2000
        },
        {
                "forest_giant", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE, 0, 0, 0, 0,
                0, 0, 0, 0,
                450, 1500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, 0
        },
        {
                "tiger", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_INFRARED | AFF_SNEAK | AFF_SWIM, PART_PAWS | PART_CLAWS | PART_FANGS | PART_TAIL, PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                100, 220, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "decaying_demon", "humanoid", "icon1", "icon2",
                0, 0, PART_HORNS | PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS, RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                240, 220, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "morepork", "bird", "icon1", "icon2",
                0, AFF_FLYING | AFF_INFRARED | AFF_SNEAK, PART_CLAWS, PART_CLAWS | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                20, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 29
        },
        {
                "giant_land_crab", "crustacean", "icon1", "icon2",
                0, 0, PART_CLAWS, 0, 0, 0, 0,
                0, 0, 0, 0,
                100, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "sasquatch", "humanoid", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                230, 180, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "boulder_beast", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0, RES_PIERCE | RES_SLASH, 0, RES_POISON | RES_DRAIN | RES_SLEEP | RES_CHARM | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                250, 1200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_elemental_burst", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "sprite", "humanoid", "icon1", "icon2",
                0, AFF_FLYING | AFF_DETECT_GOOD | AFF_DETECT_EVIL, PART_WINGS, 0, 0, 0, 0,
                0, 0, 0, 0,
                61, 4, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_sprite", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "swamp_thing", "plant", "icon1", "icon2",
                0, 0, BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS
                    | PART_HEAD | PART_EYE | PART_EAR | PART_ARMS | PART_2_LEGS
                    | PART_HANDS | PART_FEET | PART_FINGERS, 0, 0, 0, 0,
                0, 0, 0, 0,
                240, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "small_beast", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_PAWS | PART_CLAWS | PART_FANGS | PART_TAIL, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                30, 6, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 50
        },
        {
                "will_o_wisp", "mysterious_being", "icon1", "icon2",
                0, AFF_FLYING, BODY_NO_EYES | PART_EYE | BODY_ANALGIC, 0, RES_MAGIC, 0, RES_POISON,
                0, 0, 0, 0,
                61, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_innate_fade", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 61
        },
        {
                "death_slaad_humanoid", "humanoid", "icon1", "icon2",
                0, 0, 0, 0, RES_MAGIC, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                183, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "shambling_mound", "plant", "icon1", "icon2",
                0, AFF_SNEAK | AFF_SWIM, BODY_NO_ARMS | BODY_NO_LEGS | PART_ARMS | PART_2_LEGS, 0, RES_PIERCE | RES_SLASH | RES_COLD, 0, RES_BLUNT | RES_FIRE | RES_ELECTRICITY,
                0, 0, 0, 0,
                210, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_vine_grip", "", "", 15, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "marsh_giant", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE, 0, 0, 0, 0,
                0, 0, 0, 0,
                450, 1500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, 0
        },
        {
                "gargoyle", "bipedal_construct", "icon1", "icon2",
                0, AFF_FLYING, PART_WINGS | PART_CLAWS | PART_FANGS | PART_HORNS | BODY_ANALGIC, PART_CLAWS | PART_FANGS | PART_HORNS, 0, 0, RES_NONMAGIC | RES_POISON,
                0, 0, 0, 0,
                183, 180, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "killer_frog", "quadrupedal_amphibian", "icon1", "icon2",
                0, 0, PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                30, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 91
        },
        {
                "giant_flytrap", "plant", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_NO_HEAD | BODY_NO_ARMS | PART_HEAD | PART_ARMS | PART_FANGS, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                300, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_vine_grip", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "black_pudding", "amorphous_organism", "icon1", "icon2",
                0, AFF_MINDLESS | AFF_INFRARED, BODY_ANALGIC, 0, 0, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_HOLD | RES_PARALYSIS | RES_ACID | RES_COLD | RES_ELECTRICITY | RES_BLUNT | RES_PIERCE | RES_SLASH,
                0, 0, 0, 0,
                60, 450, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_acid_spray", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 244
        },
        {
                "clay_golem", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS, 0, RES_MAGIC, 0, RES_POISON | RES_SLEEP | RES_CHARM | RES_HOLD | RES_PARALYSIS | RES_DRAIN | RES_NONMAGIC | RES_SLASH | RES_PIERCE,
                0, 0, 0, 0,
                244, 272, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_golem_haste", "", "", 15, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "displacer_beast", "hexapodal_mammal", "icon1", "icon2",
                0, 0, PART_ARMS | PART_TENTACLES | PART_PAWS | PART_CLAWS | PART_FANGS | PART_TAIL, PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                100, 220, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                /* A conscious manifestation; each avatar keeps its own powers. */
                "apocalyptic_avatar", "humanoid", "icon1", "icon2",
                0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                190, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                /* These enormous horses share fiery feet, not one breath spell. */
                "apocalyptic_steed", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_HOOVES, PART_HOOVES,
                RES_FIRE, 0, 0,
                0, 0, 0, 0,
                230, 1400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 330
        },
        {
                /* The garden drake keeps its own magical flight and equipment. */
                "water_drake", "quadrupedal_reptile", "icon1", "icon2",
                0, AFF_SWIM, BODY_HUGE | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                250, 2500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1000
        },
        {
                /* River water can think and take a shape without animal organs. */
                "water_spirit", "elemental_being", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_LEGS | BODY_NO_SPEECH | BODY_NO_CORPSE
                    | BODY_BLOODLESS | BODY_ANALGIC
                    | PART_HEAD | PART_EYE | PART_EAR | PART_ARMS,
                0, RES_NONMAGIC | RES_WATER, 0, RES_POISON,
                0, 0, 0, 0,
                90, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Treat the grasping tendrils as arms for targeting and trauma. */
                "animated_vine", "plant", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_NO_ARMS | PART_ARMS | PART_TENTACLES,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                30, 40, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_vine_grip", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 600
        },
        {
                /* Their old invisibility remains; fading restores it while idle. */
                "pixie", "humanoid", "icon1", "icon2",
                0, AFF_FLYING, PART_WINGS, 0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                76, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_innate_fade", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Its antenna-bearing body has limbs and wings, but no head. */
                "mi_go", "fungoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_HEAD | PART_2_LEGS | PART_FEET | BODY_NO_HEAD
                    | PART_MANY_ARMS | PART_MANY_LEGS | PART_WINGS
                    | PART_HUSK_SHELL | PART_CLAWS,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                100, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 152
        },
        {
                /* Keep the area's heartless, faceless body and grasping paws. */
                "nightgaunt", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_EYE | PART_HEART | BODY_NO_EYES | BODY_NO_HEART
                    | BODY_NO_SPEECH | BODY_HUGE | PART_WINGS
                    | PART_HORNS | PART_PAWS | PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                250, 140, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* The winged, wormlike horror speaks without having hands. */
                "hunting_horror", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING, BODY_NO_SPEECH | BODY_HUGE | PART_WINGS | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 1220
        },
        {
                /* A living infernal servant keeps ordinary humanoid anatomy. */
                "infernal_servitor", "humanoid", "icon1", "icon2",
                0, 0, 0, 0, RES_FIRE | RES_COLD, 0, RES_POISON,
                0, 0, 0, 0,
                198, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* The bite selector does not give the slug anatomical fangs. */
                "rock_slug", "limbless_invertebrate", "icon1", "icon2",
                0, 0, PART_EYESTALKS, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                20, 40, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 120
        },
        {
                /* A djinni flies without wings; a local record can change its legs. */
                "djinni", "humanoid", "icon1", "icon2",
                0, AFF_FLYING, 0, 0, 0, 0, 0,
                0, 0, 0, 0,
                320, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_innate_fade", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* This blood-eating race is living, rather than undead. */
                "blood_reaper", "humanoid", "icon1", "icon2",
                0, 0, PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Ratlike people need no lycanthropic powers or weaknesses. */
                "ratfolk", "humanoid", "icon1", "icon2",
                0, 0, PART_TAIL | PART_CLAWS | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                152, 45, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Reuse the reviewed otyugh body and its grip special. */
                "neo_otyugh", "tripodal_organism", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_HEAD | BODY_NO_ARMS | PART_HEAD | PART_MANY_ARMS
                    | PART_TENTACLES | PART_EYESTALKS | PART_FANGS,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                160, 650, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_otyugh", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 244
        },
        {
                /* A conscious magical outline has limbs, but no living organs. */
                "ethereal_servitor", "humanoid", "icon1", "icon2",
                0, AFF_INVISIBLE | AFF_FLYING,
                PART_HEART | PART_BRAINS | PART_GUTS | BODY_NO_HEART
                    | BODY_INORGANIC | BODY_ANALGIC | BODY_NO_SPEECH | BODY_NO_CORPSE,
                0, 0, 0, 0, 0, 0, 0, 0,
                175, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* A small asp keeps its authored venom and ordinary snake organs. */
                "asp", "serpentine_reptile", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 60
        },
        {
                /* Hunting snakes does not make this animal immune to venom. */
                "mongoose", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, PART_PAWS | PART_CLAWS | PART_FANGS | PART_TAIL,
                PART_FANGS, 0, 0, 0, 0, 0, 0, 0,
                20, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 50
        },
        {
                /* Its five-centimetre height is the thickness of the carpet. */
                "animated_carpet", "limbless_construct", "icon1", "icon2",
                ACT_MOUNTABLE, AFF_FLYING | AFF_MINDLESS,
                BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS | BODY_NO_CORPSE,
                0, 0, 0, RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM,
                0, 0, 0, 0,
                5, 8, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        {
                /* This conscious sand spirit keeps a soul, but no living organs. */
                "sandman", "elemental_being", "icon1", "icon2",
                0, 0,
                PART_HEAD | PART_EYE | PART_EAR | PART_ARMS | PART_HANDS
                    | PART_FINGERS | PART_2_LEGS | PART_FEET
                    | BODY_ANALGIC | BODY_NO_SPEECH | BODY_NO_CORPSE,
                0, RES_FIRE | RES_NONMAGIC, 0, RES_POISON,
                0, 0, 0, 0,
                240, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Keep the stone remains and carried loot when it falls. */
                "caryatid_column", "bipedal_construct", "icon1", "icon2",
                0, AFF_MINDLESS,
                BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS | BODY_NO_SPEECH,
                0, RES_MAGIC | RES_NONMAGIC, 0,
                RES_POISON | RES_SLEEP | RES_DRAIN | RES_CHARM | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                213, 300, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                "hieracosphinx", "quadrupedal_hybrid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FEATHERS | PART_PAWS | PART_CLAWS | PART_TAIL | PART_BEAK,
                PART_CLAWS | PART_BEAK, 0, 0, 0, 0, 0, 0, 0,
                213, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 350
        },
        {
                /* An individual may replace its forelegs with a human upper body. */
                "gynosphinx", "quadrupedal_hybrid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FEATHERS | PART_PAWS | PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0, 0, 0, 0, 0,
                213, 260, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 330
        },
        {
                "criosphinx", "quadrupedal_hybrid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FEATHERS | PART_PAWS | PART_CLAWS | PART_TAIL | PART_HORNS,
                PART_CLAWS | PART_HORNS, 0, 0, 0, 0, 0, 0, 0,
                229, 400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_horn_gore", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 400
        },
        {
                "androsphinx", "quadrupedal_hybrid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FEATHERS | PART_PAWS | PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0, 0, 0, 0, 0,
                244, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "spec_terrifying_roar", "", 10,
                {70, 30, 0}, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 450
        },
        {
                /* It keeps its shell and mandibles, but no working organs. */
                "undead_riding_beetle", "insect", "icon1", "icon2",
                ACT_UNDEAD | ACT_MOUNTABLE, AFF_MINDLESS,
                PART_HEART | PART_BRAINS | PART_GUTS | PART_FANGS | PART_HUSK_SHELL
                    | BODY_NO_HEART | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS,
                PART_FANGS, RES_DARK, RES_HOLY,
                RES_COLD | RES_POISON | RES_DRAIN | RES_SLEEP | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                120, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 300
        },
        {
                /* Acidic contact replaces a punch; it does not add another hit. */
                "green_slime", "amorphous_organism", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_ANALGIC, 0, 0, 0,
                RES_ACID | RES_POISON | RES_SLEEP | RES_CHARM | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                20, 40, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 100
        },
        {
                /* Living crystal can have a mind and soul without animal organs. */
                "crystal_spider", "arachnid", "icon1", "icon2",
                0, 0,
                PART_HEART | PART_BRAINS | PART_GUTS | PART_FANGS | PART_HUSK_SHELL
                    | BODY_NO_HEART | BODY_INORGANIC | BODY_BLOODLESS | BODY_ANALGIC,
                PART_FANGS, RES_PIERCE | RES_SLASH, RES_SONIC, RES_POISON,
                0, 0, 0, 0,
                90, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        {
                "umber_hulk", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED, PART_SCALES | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                244, 750, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_confusing_gaze", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* Undigested treasure remains ordinary carried loot. */
                "ochre_jelly", "amorphous_organism", "icon1", "icon2",
                0, AFF_MINDLESS, BODY_ANALGIC, 0, 0, 0,
                RES_ELECTRICITY | RES_ACID | RES_POISON | RES_SLEEP
                    | RES_CHARM | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                40, 150, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_acid_spray", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 180
        },
        {
                /* These local dragonfolk do not share the Baaz stone death. */
                "draconian", "humanoid", "icon1", "icon2",
                0, 0, PART_SCALES | PART_WINGS | PART_TAIL | PART_HORNS
                    | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                190, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* The dead keep their scales and wings, but cannot fly. */
                "undead_draconian", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, AFF_MINDLESS,
                PART_SCALES | PART_WINGS | PART_TAIL | PART_HORNS
                    | PART_CLAWS | PART_FANGS | BODY_NO_SPEECH
                    | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS,
                PART_CLAWS | PART_FANGS, RES_DARK, RES_HOLY,
                RES_COLD | RES_POISON | RES_DRAIN | RES_SLEEP | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                190, 80, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                "adult_black_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_SWIM | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE | PART_HORNS, 0, RES_MAGIC, 0, RES_ACID,
                0, 0, 0, 0,
                350, 5000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_acid_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 2500
        },
        {
                "ancient_green_dragon", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_SWIM | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                BODY_HUGE | PART_HORNS, 0, RES_MAGIC, 0, RES_POISON,
                0, 0, 0, 0,
                600, 18000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_gas_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 4000
        },
        {
                /* Blink uses the existing defensive ability and its mana cost. */
                "phase_dragon", "dragon", "icon1", "icon2",
                ACT_BLINK,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                0, 0, 0, 0, 0,
                0, 0, 0, 0,
                25, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_breath_gas_anatomical", "", "", 0,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 100
        },
        {
                /* This is a living, killable avatar with one mixed breath. */
                "tiamat_avatar", "dragon", "icon1", "icon2",
                ACT_FEAR_AURA,
                AFF_FLYING | AFF_INFRARED | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
                PART_HEAD | PART_MANY_HEAD | PART_HORNS | BODY_HUGE,
                0, RES_MAGIC, 0,
                RES_FIRE | RES_COLD | RES_ELECTRICITY | RES_ACID | RES_POISON,
                0, 0, 0, 0,
                1200, 80000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_any_anatomical", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 6500
        },
        {
                /* The local acid-spitting beetle keeps its own special. */
                "rhinoceros_beetle", "insect", "icon1", "icon2",
                0, 0, PART_HUSK_SHELL | PART_HORNS | PART_FANGS,
                PART_HORNS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                120, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 366
        },
        {
                "bulette", "quadrupedal_hybrid", "icon1", "icon2",
                0, 0, BODY_NO_SPEECH | BODY_HUGE | PART_SCALES
                    | PART_HUSK_SHELL | PART_CLAWS | PART_FANGS | PART_TAIL,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                290, 1800, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 366
        },
        {
                "jaguar", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_HIDE | AFF_SNEAK | AFF_INFRARED | AFF_SWIM,
                PART_PAWS | PART_CLAWS | PART_FANGS | PART_TAIL,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                75, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 200
        },
        {
                /* Slow native regeneration replaces ordinary tick healing. */
                "humanoid_gargantua", "humanoid", "icon1", "icon2",
                0, 0, BODY_HUGE | BODY_NO_SPEECH, 0, 0, 0, 0,
                0, 0, 0, 0,
                3000, 300000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_GIANT, 0
        },
        {
                /* Its human disguise changes appearance, not its living body. */
                "rakshasa", "humanoid", "icon1", "icon2",
                0, 0, PART_CLAWS | PART_FANGS, PART_CLAWS | PART_FANGS,
                RES_MAGIC, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                183, 125, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 10,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                /* A conscious cloud has no jaws or solid organs. */
                "black_phasebeast", "mysterious_being", "icon1", "icon2",
                ACT_BLINK, AFF_FLYING, BODY_BLOODLESS | BODY_ANALGIC,
                0, 0, 0, RES_POISON,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_gas_cloud", "", "", 5,
                MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        {
                "great_ape", "humanoid", "icon1", "icon2",
                0, 0, BODY_NO_SPEECH | PART_FANGS, PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                180, 180, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 0
        },
        /* Keep each creature's body and role separate from its encounter rank. */
        {
                "pteranodon", "winged_reptile", "icon1", "icon2",
                0, AFF_FLYING,
                PART_SCALES | PART_BEAK,
                PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                120, 30, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "psi_beast", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_SPEECH | PART_TAIL | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                100, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_psionicist", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "fire_giant", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE,
                0, RES_FIRE, 0, 0,
                0, 0, 0, 0,
                549, 3400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_giant", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, 0
        },
        {
                "shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "water_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_LEGS | BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC
                    | PART_HEAD | PART_EYE | PART_ARMS,
                0, 0, 0, RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN
                    | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                300, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "earth_elemental", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC
                    | PART_HEAD | PART_EYE | PART_ARMS | PART_HANDS
                    | PART_FINGERS | PART_2_LEGS | PART_FEET,
                0, 0, 0, RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN
                    | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                300, 1500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "humanoid_avatar", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                180, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "giant_squid_tentacle", "living_appendage", "icon1", "icon2",
                0, 0,
                BODY_HUGE,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                0, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_tentacle_grip", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 600
        },
        {
                "giant_squid", "cephalopod", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_FINS | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 2000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_squid", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 1830
        },
        {
                "soul_gem", "limbless_construct", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC,
                0, 0, 0, 0,
                0, 0, 0, 0,
                20, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_cast_orb", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 20
        },
        /* Canyon material forms retain minds and souls; hosts stay living. */
        {
                "cyclops", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                610, 4000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, 0
        },
        {
                "mixed_elemental", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | PART_HEAD | PART_EYE | PART_ARMS | PART_HANDS | PART_FINGERS | PART_2_LEGS | PART_FEET,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                175, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "lightning_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_LEGS | PART_HEAD | PART_EYE | PART_ARMS,
                0, 0, 0, RES_ELECTRICITY | RES_NONMAGIC | RES_POISON | RES_SLEEP | RES_DRAIN | RES_HOLD | RES_PARALYSIS,
                0, 0, 0, 0,
                300, 0, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_lightning_discharge", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "element_bound_humanoid", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "water_mote", "elemental_being", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                5, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "fire_mote", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, RES_COLD | RES_WATER, RES_POISON | RES_SLEEP | RES_DRAIN | RES_FIRE,
                0, 0, 0, 0,
                50, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "ice_mote", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, RES_FIRE, RES_POISON | RES_SLEEP | RES_DRAIN | RES_COLD,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 2
        },
        {
                "stone_mote", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                0, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 10
        },
        {
                "lightning_mote", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN | RES_ELECTRICITY,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 10
        },
        {
                "acid_elemental", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN | RES_ACID,
                0, 0, 0, 0,
                30, 50, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_acid_spray", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "lava_elemental", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | PART_HEAD | PART_EYE | PART_ARMS,
                0, RES_NONMAGIC, RES_COLD | RES_WATER, RES_POISON | RES_SLEEP | RES_DRAIN | RES_FIRE,
                0, 0, 0, 0,
                100, 400, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "mud_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | PART_HEAD | PART_EYE | PART_ARMS | PART_HANDS | PART_FINGERS | PART_2_LEGS | PART_FEET,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                100, 200, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "mist_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                150, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 200
        },
        {
                "energy_mote", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                0, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_lightning_discharge", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "dust_elemental", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                100, 0, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_thief", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "air_eddy", "elemental_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS,
                0, RES_NONMAGIC, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                50, 0, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_thief", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        /* These living heads and the Modron keep their minds and souls. */
        {
                "chimera_goat_head", "head_only", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | PART_BRAINS | PART_HORNS,
                PART_HORNS, 0, 0, 0,
                0, 0, 0, 0,
                50, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 75
        },
        {
                "chimera_lion_head", "head_only", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | PART_BRAINS | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                60, 35, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 75
        },
        {
                "chimera_dragon_head", "head_only", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | PART_BRAINS | PART_SCALES | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                75, 50, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_any_anatomical", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "modron_secundus", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE,
                0, RES_MAGIC | RES_COLD | RES_FIRE | RES_ACID, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                300, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        /* Olympus constructs keep their minds; the winged beasts stay living. */
        {
                "golden_handmaid", "bipedal_construct", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC,
                0, 0, 0, RES_POISON | RES_SLEEP | RES_DRAIN,
                0, 0, 0, 0,
                170, 1250, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "chimera", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | PART_MANY_HEAD | PART_WINGS | PART_TAIL | PART_HORNS | PART_FANGS | PART_CLAWS | PART_PAWS | PART_HOOVES | PART_SCALES,
                PART_CLAWS | PART_FANGS | PART_HORNS, 0, 0, 0,
                0, 0, 0, 0,
                150, 400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "pegasus", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING | AFF_DETECT_GOOD | AFF_DETECT_EVIL,
                PART_HOOVES | PART_TAIL | PART_WINGS | PART_FEATHERS,
                PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                170, 700, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 260
        },
        /* Ghost Town gives these bodies their first proper profiles. */
        {
                "demon", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                180, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "revenant_spirit", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, 0,
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                175, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "haunt", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, 0,
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                175, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "crypt_thing", "humanoid", "icon1", "icon2",
                ACT_UNDEAD, 0,
                BODY_NO_HEART | BODY_INORGANIC | PART_HEART | PART_BRAINS | PART_GUTS | PART_CLAWS | BODY_BLOODLESS | BODY_ANALGIC,
                PART_CLAWS, 0, 0, RES_POISON | RES_DRAIN | RES_SLEEP | RES_CHARM | RES_HOLD | RES_NONMAGIC,
                0, 0, 0, 0,
                183, 12, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "lurker_above", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_HUGE | PART_HEAD | PART_EYE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                30, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_lurker_wrap", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 610
        },
        {
                "succubus", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_CLAWS | PART_FANGS,
                PART_CLAWS, RES_COLD | RES_NONMAGIC | RES_MAGIC, 0, RES_FIRE | RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                183, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_succubus_fascination", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "gray_ooze", "amorphous_organism", "icon1", "icon2",
                0, 0,
                BODY_ANALGIC,
                0, RES_MAGIC, 0, RES_FIRE | RES_COLD | RES_POISON,
                0, 0, 0, 0,
                20, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 244
        },
        {
                "nightmare", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_TAIL | PART_FANGS | PART_HOOVES,
                PART_HOOVES, 0, 0, 0,
                0, 0, 0, 0,
                183, 700, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 270
        },
        {
                "retriever", "arachnid", "icon1", "icon2",
                ACT_FEAR_AURA, 0,
                BODY_NO_ARMS | BODY_HUGE | PART_ARMS | PART_MANY_ARMS | PART_4_LEGS | PART_MANY_LEGS | PART_CLAWS | PART_FANGS,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                366, 2000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_retriever_rays", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 450
        },
        {
                "cambion", "humanoid", "icon1", "icon2",
                0, 0,
                PART_SCALES | PART_FANGS,
                0, RES_FIRE | RES_COLD | RES_MAGIC, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                198, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "orcus_avatar", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_HORNS | PART_HOOVES,
                0, 0, 0, 0,
                0, 0, 0, 0,
                610, 3000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "water_weird", "elemental_being", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH | BODY_BLOODLESS | BODY_ANALGIC,
                0, RES_FIRE | RES_PIERCE | RES_SLASH, 0, RES_POISON | RES_DRAIN,
                0, 0, 0, 0,
                91, 250, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_water_weird_wrap", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 305
        },
        {
                "kampflut", "plant", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | BODY_NO_ARMS | BODY_NO_LEGS | BODY_HUGE | PART_HEAD | PART_ARMS | PART_2_LEGS | PART_FEET | PART_TENTACLES | PART_FANGS,
                PART_TENTACLES | PART_FANGS, 0, RES_FIRE, 0,
                0, 0, 0, 0,
                600, 4000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_tentacle_grip", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "animated_evil_tree", "plant", "icon1", "icon2",
                0, 0,
                BODY_NO_ARMS | BODY_NO_LEGS | BODY_HUGE | PART_ARMS | PART_2_LEGS | PART_FEET,
                0, 0, RES_FIRE, 0,
                0, 0, 0, 0,
                450, 2000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "tree_spirit", "plant", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH | BODY_NO_CORPSE | PART_HEAD | PART_ARMS | PART_2_LEGS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR | PART_EYE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_adept", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "astral_manifestation", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_EYES | BODY_NO_HEART | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_INORGANIC | BODY_BLOODLESS | BODY_ANALGIC | PART_EAR | PART_EYE | PART_HEART | PART_BRAINS | PART_GUTS,
                0, 0, 0, 0,
                0, 0, 0, 0,
                180, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "githyanki", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_FANGS,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "githzerai", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                0,
                0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                175, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "night_hag", "humanoid", "icon1", "icon2",
                0, 0,
                PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, RES_MAGIC | RES_NONMAGIC, 0, RES_FIRE | RES_COLD | RES_CHARM | RES_SLEEP,
                0, 0, 0, 0,
                152, 50, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "invisible_stalker", "elemental_being", "icon1", "icon2",
                0, AFF_INVISIBLE | AFF_FLYING,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_NO_LEGS | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC,
                0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                244, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "mind_flayer", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                PART_TENTACLES,
                0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                183, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "small_red_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_HUGE,
                PART_TAILATTACK, 0, 0, RES_FIRE,
                0, 0, 0, 0,
                150, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 610
        },
        {
                "giant_ground_beetle", "insect", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                40, 15, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "giant_maggot", "limbless_invertebrate", "icon1", "icon2",
                0, 0,
                BODY_NO_EYES | PART_EYE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                25, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "homunculus", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | PART_WINGS | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                46, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_homunculus_sleep_bite", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "chreffn", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                150, 50, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "sea_hag", "humanoid", "icon1", "icon2",
                ACT_FEAR_AURA, AFF_SWIM,
                PART_CLAWS | PART_FANGS | PART_HORNS,
                PART_CLAWS, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                165, 55, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "guardian_naga", "serpentine_reptile", "icon1", "icon2",
                0, 0,
                BODY_NO_SPEECH | BODY_HUGE | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                100, 180, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "spec_poison_bite", "", 0, {50, 50, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 610
        },
        {
                "basilisk", "multipedal_reptile", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                45, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_basilisk_stiffening_gaze", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 213
        },
        {
                "lemure", "amorphous_organism", "icon1", "icon2",
                ACT_REGENERATOR, AFF_MINDLESS,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_ARMS | BODY_BLOODLESS | PART_HEAD | PART_EYE | PART_ARMS | PART_HANDS | PART_FINGERS | PART_CLAWS,
                PART_CLAWS, RES_COLD, 0, RES_FIRE | RES_POISON,
                0, 0, 0, 0,
                152, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "ettin", "humanoid", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_HUGE | PART_HEAD | PART_MANY_HEAD,
                0, 0, 0, 0,
                0, 0, 0, 0,
                396, 1100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, MOB_TEMPLATE_UNSET
        },
        {
                "modron_tridrone", "tripodal_organism", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_NO_ARMS | BODY_NO_SPEECH | PART_ARMS | PART_MANY_ARMS | PART_HANDS | PART_FINGERS,
                0, RES_COLD | RES_FIRE | RES_ACID, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN,
                0, 0, 0, 0,
                150, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_quadrone", "geometric_being", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_CORPSE | BODY_NO_ARMS | BODY_NO_LEGS | PART_ARMS | PART_MANY_ARMS | PART_HANDS | PART_FINGERS | PART_2_LEGS | PART_FEET,
                0, RES_COLD | RES_FIRE | RES_ACID, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN,
                0, 0, 0, 0,
                150, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_pentadrone", "geometric_being", "icon1", "icon2",
                0, AFF_INFRARED,
                BODY_NO_CORPSE | BODY_NO_ARMS | BODY_NO_LEGS | PART_ARMS | PART_MANY_ARMS | PART_MANY_LEGS | PART_FEET,
                0, RES_COLD | RES_FIRE | RES_ACID, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN,
                0, 0, 0, 0,
                180, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_paralysing_gas", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_decaton", "geometric_being", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_NO_ARMS | BODY_NO_LEGS | PART_ARMS | PART_MANY_ARMS | PART_TENTACLES | PART_2_LEGS | PART_FEET,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                160, 150, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_nonaton", "geometric_being", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_NO_ARMS | BODY_NO_LEGS | PART_ARMS | PART_MANY_ARMS | PART_TENTACLES | PART_2_LEGS | PART_FEET,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                240, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_octon", "geometric_being", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                BODY_NO_CORPSE | BODY_NO_ARMS | PART_ARMS | PART_MANY_ARMS | PART_TENTACLES,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                240, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_septon", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_CORPSE | PART_ARMS | PART_HANDS | PART_FINGERS | PART_MANY_ARMS | PART_TENTACLES,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                180, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_hexton", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | PART_MANY_ARMS | PART_TENTACLES | PART_CLAWS | PART_WINGS,
                PART_TENTACLES, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                270, 180, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_quinton", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | PART_MANY_ARMS | PART_TAIL | PART_WINGS,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                270, 220, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "modron_tertian", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_NO_CORPSE | BODY_HUGE | PART_HORNS | PART_TAIL,
                PART_TAILATTACK, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                366, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "primus_manifestation", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_HUGE,
                0, RES_COLD | RES_FIRE | RES_ACID | RES_MAGIC, 0, RES_SLEEP | RES_CHARM | RES_HOLD | RES_DRAIN | RES_PSYCHIC,
                0, 0, 0, 0,
                450, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        /* These planar visitors keep their living bodies, minds and souls. */
        {
                "hollyphant", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_TRUNK | PART_TUSKS | PART_TAIL,
                PART_TUSKS, RES_MAGIC, 0, RES_POISON,
                0, 0, 0, 0,
                45, 30, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "spec_holy_trumpet", "", 0, {70, 30, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 61
        },
        {
                "planetar", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR, AFF_FLYING,
                PART_WINGS | PART_FEATHERS,
                0, RES_MAGIC | RES_FIRE, 0, RES_COLD | RES_ELECTRICITY | RES_POISON | RES_DRAIN | RES_CHARM,
                0, 0, 0, 0,
                244, 120, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "lesser_shedu", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | PART_WINGS | PART_FEATHERS | PART_HOOVES | PART_TAIL,
                PART_HOOVES, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                160, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_psionicist", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "dretch", "humanoid", "icon1", "icon2",
                0, 0,
                PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, RES_MAGIC | RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                110, 45, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "chasme", "insect", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_ARMS | BODY_NO_SPEECH | PART_MANY_LEGS | PART_ARMS | PART_HANDS | PART_FINGERS | PART_4_LEGS | PART_FEET | PART_EAR | PART_WINGS | PART_CLAWS,
                PART_CLAWS, RES_MAGIC | RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                90, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "spec_lulling_drone", "", 0, {70, 30, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 213
        },
        {
                "babau", "humanoid", "icon1", "icon2",
                0, 0,
                PART_CLAWS | PART_HORNS,
                PART_CLAWS | PART_HORNS, RES_MAGIC | RES_FIRE | RES_COLD | RES_SLASH | RES_PIERCE, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                213, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "yagnoloth", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_SCALES,
                0, RES_MAGIC, RES_COLD, RES_ACID | RES_FIRE | RES_POISON,
                0, 0, 0, 0,
                366, 450, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "kostchtchie_avatar", "humanoid", "icon1", "icon2",
                0, 0,
                BODY_HUGE,
                0, RES_MAGIC | RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                853, 8000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_GIANT, 0
        },
        {
                "nabassu", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, RES_MAGIC | RES_FIRE | RES_COLD, 0, RES_ELECTRICITY | RES_POISON,
                0, 0, 0, 0,
                213, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_demon", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "cloud_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_HUGE,
                PART_TAILATTACK, 0, 0, RES_COLD,
                0, 0, 0, 0,
                300, 2500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_frost_anatomical", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 1200
        },
        {
                "solar", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR, AFF_FLYING | AFF_INFRARED,
                PART_WINGS | PART_FEATHERS,
                0, RES_MAGIC, 0, RES_COLD | RES_ELECTRICITY | RES_ACID | RES_POISON | RES_DRAIN | RES_CHARM | RES_HOLD,
                0, 0, 0, 0,
                274, 160, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_cleric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "wemic", "centauroid_mammal", "icon1", "icon2",
                0, 0,
                PART_PAWS | PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                213, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_thief", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 305
        },
        {
                "gorgimera", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_SPEECH | PART_MANY_HEAD | PART_WINGS | PART_TAIL | PART_SCALES | PART_PAWS | PART_HOOVES | PART_CLAWS | PART_FANGS | PART_HORNS,
                PART_CLAWS | PART_FANGS | PART_HORNS, 0, 0, 0,
                0, 0, 0, 0,
                150, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_fire_anatomical", "spec_paralysing_gas", "", 0, {50, 50, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 350
        },
        {
                "giant_squirrel", "quadrupedal_mammal", "icon1", "icon2",
                0, 0,
                PART_PAWS | PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                100, 80, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 240
        },
        /* Authored behaviour and venom roles remain individual area choices. */
        {
                "megalodon", "fish", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 50000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 2000
        },
        {
                "vampire_squid", "cephalopod", "icon1", "icon2",
                0, 0,
                PART_FINS | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 1, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 30
        },
        {
                "spider_crab", "crustacean", "icon1", "icon2",
                0, AFF_SWIM,
                PART_CLAWS,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                40, 15, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 60
        },
        {
                "sea_anemone", "radial_invertebrate", "icon1", "icon2",
                0, AFF_SWIM,
                PART_TENTACLES,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                50, 20, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "anglerfish", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS | PART_SCALES,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 80
        },
        {
                "sea_lion", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FINS | PART_FANGS | PART_TAIL,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                80, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 220
        },
        {
                "sea_snake", "serpentine_reptile", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "cuttlefish", "cephalopod", "icon1", "icon2",
                0, 0,
                PART_FINS | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 10, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 120
        },
        {
                "jellyfish", "radial_invertebrate", "icon1", "icon2",
                0, AFF_SWIM,
                PART_TENTACLES,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                30, 5, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "goblin_shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 350
        },
        {
                "porcupinefish", "fish", "icon1", "icon2",
                0, 0,
                PART_BEAK | PART_SCALES | PART_SHARPSCALES,
                PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 50
        },
        {
                "giant_lobster", "crustacean", "icon1", "icon2",
                0, AFF_SWIM,
                PART_CLAWS | PART_TAIL,
                PART_CLAWS, 0, 0, 0,
                0, 0, 0, 0,
                60, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "giant_octopus", "cephalopod", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 10000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 2000
        },
        {
                "sahuagin", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FINS | PART_TAIL | PART_SCALES | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                183, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        /* The Abyss visitors fly magically; marine bodies do not add Swim. */
        {
                "photophore", "amorphous_organism", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                80, 30, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 80
        },
        {
                "mesopelag", "fish", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "malacost", "crustacean", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_ARMS | PART_ARMS | PART_EYESTALKS,
                0, 0, 0, 0,
                0, 0, 0, 0,
                50, 30, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "pycnogonid", "arachnid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                65, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "anthozoa", "radial_invertebrate", "icon1", "icon2",
                0, AFF_FLYING,
                PART_TENTACLES,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                200, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "scyph", "limbless_invertebrate", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_LEGS | PART_HEAD | PART_EYE | PART_FEET | PART_HUSK_SHELL,
                0, 0, 0, 0,
                0, 0, 0, 0,
                80, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 200
        },
        {
                "siph", "plant", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                200, 20, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_fido", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "erenna", "radial_invertebrate", "icon1", "icon2",
                0, AFF_FLYING,
                BODY_NO_CORPSE | BODY_NO_HEART | PART_HEART | PART_TENTACLES,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                150, 200, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 600
        },
        {
                "copepod", "crustacean", "icon1", "icon2",
                0, AFF_FLYING,
                PART_TAIL,
                0, 0, 0, 0,
                0, 0, 0, 0,
                40, 25, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "abralia", "cephalopod", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                BODY_NO_CORPSE | PART_FINS | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 900
        },
        {
                "veranyi", "fish", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                BODY_HUGE | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 2000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_bloodsucker_bite", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 800
        },
        {
                "calimedusae", "cephalopod", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                PART_FINS | PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                0, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 600
        },
        {
                "cyclothone", "fish", "icon1", "icon2",
                0, AFF_FLYING | AFF_SWIM,
                PART_FANGS,
                PART_FANGS, 0, 0, RES_ELECTRICITY,
                0, 0, 0, 0,
                0, 300, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_cast_electric", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 700
        },
        /* Body size, arm count and venom remain separate choices. */
        {
                "sawfish", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_fido", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 700
        },
        {
                "cownose_ray", "fish", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 75, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "frilled_shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 20, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 200
        },
        {
                "lemon_shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 90, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 280
        },
        {
                "fiddler_ray", "fish", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 10, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 120
        },
        {
                "sahuagin_baron", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_ARMS | PART_MANY_ARMS | PART_FINS | PART_TAIL | PART_SCALES | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                244, 180, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_sahuagin_baron", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        /* These profiles keep each fish's anatomy separate from its role. */
        {
                "sheepshead_wrasse", "fish", "icon1", "icon2",
                0, 0,
                PART_SCALES | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 40, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "giant_blobfish", "fish", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "moonfish", "fish", "icon1", "icon2",
                0, 0,
                PART_SCALES,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "goliath_grouper", "fish", "icon1", "icon2",
                0, 0,
                PART_SCALES | BODY_HUGE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 3000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_large_whale", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 1000
        },
        {
                "stargazer", "fish", "icon1", "icon2",
                0, 0,
                PART_SCALES | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 5, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 60
        },
        /* Mount enchantments stay on the individual fish. */
        {
                "blue_shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "stingray", "fish", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "skate", "fish", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 35, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "great_white_shark", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS | BODY_HUGE,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 1500, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 600
        },
        {
                "training_dummy", "limbless_construct", "icon1", "icon2",
                ACT_OBJECT | ACT_NO_FIGHT | ACT_NO_EXPERIENCE | ACT_SENTINEL, AFF_MINDLESS,
                BODY_NO_HEAD | PART_HEAD | BODY_NO_CORPSE | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "triton", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FINS | PART_SCALES,
                0, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                213, 110, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "green_grung", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FANGS,
                PART_FANGS, RES_POISON, 0, 0,
                0, 0, 0, 0,
                91, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_green_grung", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "vodyanoi", "humanoid", "icon1", "icon2",
                0, AFF_SWIM | AFF_INFRARED,
                PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                275, 900, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 0
        },
        {
                "aboleth", "fish", "icon1", "icon2",
                0, 0,
                BODY_NO_ARMS | BODY_NO_SPEECH
                    | BODY_HUGE | PART_ARMS | PART_MANY_ARMS | PART_TENTACLES | PART_FANGS,
                PART_TENTACLES | PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 1600, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_aboleth", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 610
        },
        {
                "laghathti", "cephalopod", "icon1", "icon2",
                0, 0,
                BODY_NO_HEAD | PART_HEAD | BODY_NO_EYES | PART_EYE
                    | BODY_NO_HEART | PART_HEART | BODY_NO_SPEECH | PART_FINS,
                PART_TENTACLES, 0, 0, 0,
                0, 0, 0, 0,
                0, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_laghathti", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 244
        },
        {
                "giant_alligator_gar", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS | PART_SCALES,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                0, 150, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "sea_elf", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                190, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_ELF, 0
        },
        {
                "rolling_calf", "quadrupedal_mammal", "icon1", "icon2",
                ACT_UNDEAD, 0,
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS | PART_HOOVES | PART_TAIL | BODY_HUGE,
                0, 0, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                180, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_fire", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL, CREATURE_FAMILY_NONE, 300
        },
        {
                "three_legged_spirit_horse", "quadrupedal_mammal", "icon1", "icon2",
                ACT_UNDEAD, AFF_SWIM,
                BODY_NO_HEART | BODY_NO_CORPSE | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS | PART_HOOVES | PART_TAIL
                    | PART_4_LEGS | PART_3_LEGS,
                0, 0, 0, RES_NONMAGIC,
                0, 0, 0, 0,
                150, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_gas", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_SEMI_MATERIAL, CREATURE_FAMILY_NONE, 230
        },
        {
                "higue_fire_form", "mysterious_being", "icon1", "icon2",
                0, AFF_FLAMING | AFF_FLYING | AFF_SWIM,
                BODY_NO_SPEECH,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_bloodsucker", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 50
        },
        {
                "tokoloshe", "humanoid", "icon1", "icon2",
                0, AFF_SWIM | AFF_INVISIBLE,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                105, 32, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "water_spirit_avatar", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "selkie_seal_form", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_LEGS | BODY_NO_CORPSE | PART_4_LEGS | PART_FORELEGS
                    | PART_FEET | PART_FINS | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 170
        },
        {
                "selkie_human_form", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                175, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "sailing_ship", "limbless_construct", "icon1", "icon2",
                ACT_OBJECT, 0,
                BODY_HUGE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 3000
        },
        {
                "ghost_ship", "limbless_construct", "icon1", "icon2",
                ACT_OBJECT, AFF_INVISIBLE | AFF_SNEAK | AFF_FLYING,
                BODY_HUGE,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 3000
        },
        {
                "sperm_whale", "cetacean", "icon1", "icon2",
                0, 0,
                BODY_HUGE | PART_FANGS,
                PART_FANGS | PART_TAILATTACK, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 40000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_large_whale", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 1600
        },
        {
                "merfolk", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_LEGS | PART_2_LEGS | PART_FEET
                    | PART_TAIL | PART_FINS | PART_SCALES,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 175
        },
        {
                "typhoon", "environmental_hazard", "icon1", "icon2",
                ACT_OBJECT, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_lightning", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "kraken", "cephalopod", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_FINS | PART_BEAK, PART_TENTACLES | PART_BEAK,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 6000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_squid", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 2750
        },
        {
                "shore_crab", "crustacean", "icon1", "icon2",
                0, 0, PART_CLAWS, PART_CLAWS,
                0, 0, 0, 0, 0, 0, 0,
                91, 10, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 35
        },
        {
                "forged_lava_worm", "limbless_construct", "icon1", "icon2",
                0, AFF_PASS_DOOR, 0, 0,
                0, 0, RES_FIRE, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 90, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_fire", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "lava_lizard", "quadrupedal_reptile", "icon1", "icon2",
                0, AFF_SWIM, PART_CLAWS | PART_FANGS, PART_FANGS,
                RES_FIRE, 0, 0, 0, 0, 0, 0,
                45, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "fire_avatar", "elemental_being", "icon1", "icon2",
                0, 0, BODY_NO_LEGS | BODY_NO_SPEECH | BODY_NO_CORPSE | BODY_HUGE
                    | PART_HEAD | PART_ARMS | PART_EYE | BODY_BLOODLESS | BODY_ANALGIC, 0,
                0, 0, RES_FIRE, 0, 0, 0, 0,
                610, MOB_TEMPLATE_UNSET, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_fire", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "tiger_shark", "fish", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 450, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 400
        },
        {
                "mako_shark", "fish", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 90, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 270
        },
        {
                "giant_clam", "bivalve", "icon1", "icon2",
                0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 200, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 120
        },
        {
                "terra_kan", "bird", "icon1", "icon2",
                0, 0, PART_WINGS | PART_CLAWS, PART_CLAWS | PART_BEAK,
                0, 0, 0, 0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 160
        },
        {
                "orca", "cetacean", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_FANGS, PART_FANGS | PART_TAILATTACK,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 3000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_large_whale", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 600
        },
        {
                "cave_bear", "quadrupedal_mammal", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_TAIL | PART_CLAWS | PART_FANGS | PART_PAWS, PART_CLAWS
                    | PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                180, 800, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_bear", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "penguin", "bird", "icon1", "icon2",
                0, AFF_SWIM,
                0,
                PART_BEAK, RES_COLD, 0, 0,
                0, 0, 0, 0,
                100, 35, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 70
        },
        {
                "arctic_wolf", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_CLAWS | PART_FANGS | PART_PAWS,
                PART_FANGS, RES_COLD, 0, 0,
                0, 0, 0, 0,
                80, 50, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "ice_fish", "fish", "icon1", "icon2",
                0, 0,
                PART_SCALES,
                0, RES_COLD, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 2, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 60
        },
        {
                "leopard_seal", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_LEGS | PART_4_LEGS | PART_FEET | PART_FINS | PART_FANGS | PART_FORELEGS,
                PART_FANGS, RES_COLD, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 350, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "young_white_dragon", "dragon", "icon1", "icon2",
                0, AFF_DETECT_INVIS | AFF_DETECT_HIDDEN | AFF_INFRARED | AFF_FLYING,
                0,
                0, 0, 0, RES_COLD,
                0, 0, 0, 0,
                120, 400, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_frost_anatomical", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 550
        },
        {
                "frost_spirit_avatar", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                BODY_NO_HEART | BODY_NO_CORPSE | PART_HEART,
                0, 0, 0, RES_COLD,
                0, 0, 0, 0,
                175, 65, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "arctic_fox", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_CLAWS | PART_FANGS | PART_PAWS,
                PART_FANGS, RES_COLD, 0, 0,
                0, 0, 0, 0,
                45, 10, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 110
        },
        {
                "venomous_vampire", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR | ACT_UNDEAD, 0,
                PART_CLAWS | PART_FANGS,
                0, RES_COLD | RES_ELECTRICITY | RES_PSYCHIC | RES_DARK, RES_HOLY | RES_WATER | RES_SILVER, RES_POISON | RES_SLEEP | RES_CHARM | RES_NONMAGIC | RES_CURSE,
                0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_vampire", "spec_poison", "",
                0, {70, 30, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "gas_vampire", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR | ACT_UNDEAD, 0,
                PART_FANGS,
                0, RES_COLD | RES_ELECTRICITY | RES_PSYCHIC | RES_DARK, RES_HOLY | RES_WATER | RES_SILVER, RES_POISON | RES_SLEEP | RES_CHARM | RES_NONMAGIC | RES_CURSE,
                0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_vampire", "spec_breath_gas", "",
                0, {70, 30, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "vampire_mage", "humanoid", "icon1", "icon2",
                ACT_REGENERATOR | ACT_UNDEAD, 0,
                PART_FANGS,
                0, RES_COLD | RES_ELECTRICITY | RES_PSYCHIC | RES_DARK, RES_HOLY | RES_WATER | RES_SILVER, RES_POISON | RES_SLEEP | RES_CHARM | RES_NONMAGIC | RES_CURSE,
                0, 0, 0, 0,
                175, 75, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_vampire", "spec_cast_undead", "",
                0, {70, 30, 0},
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "vampire_leech", "limbless_invertebrate", "icon1", "icon2",
                ACT_REGENERATOR | ACT_UNDEAD, 0,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_HEART | PART_HEAD | PART_HEART
                    | PART_BRAINS | PART_GUTS | PART_EYE,
                0, RES_COLD | RES_ELECTRICITY | RES_PSYCHIC | RES_DARK, RES_HOLY | RES_WATER | RES_SILVER, RES_POISON | RES_SLEEP | RES_CHARM | RES_NONMAGIC | RES_CURSE,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 8, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 100
        },
        {
                "great_wolf", "quadrupedal_mammal", "icon1", "icon2",
                0, AFF_DETECT_HIDDEN,
                PART_TAIL | PART_CLAWS | PART_FANGS | PART_PAWS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                100, 85, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 180
        },
        {
                "giant_bat", "winged_mammal", "icon1", "icon2",
                0, AFF_FLYING,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 35, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 150
        },
        {
                "bat_humanoid", "humanoid", "icon1", "icon2",
                0, AFF_FLYING,
                PART_WINGS | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                185, 85, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "dreadguard", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                200, 120, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_breath_gas", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "poison_floor", "environmental_hazard", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_poison_contact", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 183
        },
        {
                "gordaron", "humanoid", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                190, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "grottobeast", "quadrupedal_hybrid", "icon1", "icon2",
                0, 0,
                BODY_NO_SPEECH | PART_TAIL | PART_CLAWS | PART_FANGS,
                PART_CLAWS | PART_FANGS | PART_TAILATTACK, 0, 0, 0,
                0, 0, 0, 0,
                180, 700, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 400
        },
        {
                "giant_millipede", "myriapod", "icon1", "icon2",
                0, 0,
                PART_HUSK_SHELL,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_poison_contact", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "ahrganite", "amorphous_organism", "icon1", "icon2",
                0, 0,
                0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_mage", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "bleeder", "quadrupedal_hybrid", "icon1", "icon2",
                0, 0,
                BODY_NO_SPEECH | PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                80, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_cast_psionicist", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 240
        },
        {
                "three_headed_dragon", "dragon", "icon1", "icon2",
                0, AFF_FLYING, PART_MANY_HEAD | BODY_HUGE, 0,
                0, 0, 0,
                0, 0, 0, 0,
                400, 6000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_any", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 2400
        },
        {
                "medusa", "humanoid", "icon1", "icon2",
                0, 0, PART_FANGS, PART_FANGS,
                0, 0, 0,
                0, 0, 0, 0,
                205, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_stiffening_gaze", "spec_medusa_snake_bite", "",
                5, {60, 40, 0}, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "acid_worm", "limbless_invertebrate", "icon1", "icon2",
                0, 0, BODY_HUGE | PART_FANGS, PART_FANGS,
                0, 0, RES_ACID,
                0, 0, 0, 0,
                100, 4000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_breath_acid_anatomical", "", "",
                5, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 800
        },
        {
                "infernal_wyrm", "dragon", "icon1", "icon2",
                0, AFF_FLYING, BODY_HUGE, 0,
                0, 0, 0,
                0, 0, 0, 0,
                700, 30000, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "", "", "",
                0, MOB_SPECIAL_CHANCES_AUTO, GHOST_PHASE_NONE,
                CREATURE_FAMILY_NONE, 4500
        },
        {
                "giant_sea_python", "serpentine_reptile", "icon1", "icon2",
                0, AFF_SWIM, BODY_HUGE | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 800, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_constrictor", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 1200
        },
        {
                "kuo_toa", "humanoid", "icon1", "icon2",
                0, AFF_SWIM | AFF_INFRARED | AFF_DETECT_INVIS, 0, 0,
                RES_ELECTRICITY, RES_FIRE, RES_POISON | RES_PARALYSIS,
                0, 0, 0, 0,
                150, 70, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "bullywug", "humanoid", "icon1", "icon2",
                0, AFF_SWIM, PART_LONG_TONGUE, 0, 0, 0, 0,
                0, 0, 0, 0,
                175, 80, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "kopru", "humanoid", "icon1", "icon2",
                0, AFF_SWIM | AFF_INFRARED,
                BODY_NO_LEGS | BODY_NO_SPEECH | PART_2_LEGS | PART_FEET
                    | PART_TAIL | PART_TENTACLES | PART_FANGS | PART_CLAWS,
                PART_CLAWS | PART_FANGS | PART_TAILATTACK, RES_MAGIC, 0, 0,
                0, 0, 0, 0,
                100, 100, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "spec_fascinating_gaze", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 200
        },
        {
                "vampiric_ixzan", "fish", "icon1", "icon2",
                ACT_REGENERATOR, AFF_INFRARED, PART_FANGS, PART_FANGS,
                RES_BLUNT, 0, 0, 0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_draining_bite", "", "", 10, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 250
        },
        {
                "water_naga", "serpentine_reptile", "icon1", "icon2",
                0, AFF_SWIM, BODY_NO_SPEECH | PART_FANGS, PART_FANGS,
                0, 0, 0, 0, 0, 0, 0,
                80, 100, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_poison_bite", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 305
        },
        {
                "uchuulon", "crustacean", "icon1", "icon2",
                0, AFF_SWIM | AFF_INFRARED,
                PART_MANY_LEGS | PART_4_LEGS | PART_CLAWS | PART_TENTACLES | PART_TAIL,
                PART_CLAWS, RES_MAGIC, 0, RES_POISON, 0, 0, 0, 0,
                100, 290, CHAR_SIZE_LARGE, MOB_TEMPLATE_UNSET,
                "spec_poison_contact", "", "", 5, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 244
        },
        {
                "orange_grung", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FANGS,
                PART_FANGS, RES_POISON, 0, 0,
                0, 0, 0, 0,
                110, 35, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_orange_grung", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "red_grung", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FANGS,
                PART_FANGS, RES_POISON, 0, 0,
                0, 0, 0, 0,
                91, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_red_grung", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "gold_grung", "humanoid", "icon1", "icon2",
                0, AFF_SWIM,
                PART_FANGS,
                PART_FANGS, RES_POISON, 0, 0,
                0, 0, 0, 0,
                91, 25, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "spec_gold_grung", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        },
        {
                "sharptooth_catfish", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 200
        },
        {
                "conger_eel", "fish", "icon1", "icon2",
                0, 0,
                PART_FANGS,
                PART_FANGS, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 60, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "octopus", "cephalopod", "icon1", "icon2",
                0, 0,
                PART_BEAK,
                PART_TENTACLES | PART_BEAK, 0, 0, 0,
                0, 0, 0, 0,
                MOB_TEMPLATE_UNSET, 15, CHAR_SIZE_MEDIUM, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, 300
        },
        {
                "headless_training_dummy", "bipedal_construct", "icon1", "icon2",
                ACT_OBJECT | ACT_NO_FIGHT | ACT_NO_EXPERIENCE | ACT_SENTINEL, AFF_MINDLESS,
                BODY_NO_HEAD | BODY_NO_EYES | BODY_NO_SPEECH | BODY_NO_CORPSE
                    | BODY_BLOODLESS | BODY_ANALGIC | BODY_SOULLESS
                    | PART_HEAD | PART_EYE | PART_EAR,
                0, 0, 0, 0,
                0, 0, 0, 0,
                100, 80, CHAR_SIZE_SMALL, MOB_TEMPLATE_UNSET,
                "", "", "", 0, MOB_SPECIAL_CHANCES_AUTO,
                GHOST_PHASE_NONE, CREATURE_FAMILY_NONE, MOB_TEMPLATE_UNSET
        }
};

#undef DWARF_ARCHETYPE
#undef VAMPIRE_ARCHETYPE
#undef ANIMATED_TOY_ARCHETYPE
#undef ZOMBIE_ARCHETYPE

/* Unconverted NPCs stay unclassified; names are not a species test. */
CREATURE_FAMILY creature_family(CHAR_DATA *ch)
{
        int archetype;

        if (!ch)
                return CREATURE_FAMILY_NONE;

        if (IS_NPC(ch))
        {
                if (!ch->mobspec || ch->mobspec[0] == '\0')
                        return CREATURE_FAMILY_NONE;
                archetype = mob_lookup(ch->mobspec);
                if (archetype < 0 || archetype >= MAX_MOB)
                        return CREATURE_FAMILY_NONE;
                return mob_table[archetype].family;
        }

        switch (ch->race)
        {
        case RACE_HUMAN:     return CREATURE_FAMILY_HUMAN;
        case RACE_DWARF:     return CREATURE_FAMILY_DWARF;
        case RACE_ORC:       return CREATURE_FAMILY_ORC;
        case RACE_GOBLIN:    return CREATURE_FAMILY_GOBLIN;
        case RACE_HOBGOBLIN: return CREATURE_FAMILY_HOBGOBLIN;
        case RACE_OGRE:      return CREATURE_FAMILY_OGRE;
        case RACE_TROLL:     return CREATURE_FAMILY_TROLL;
        case RACE_GIANT:     return CREATURE_FAMILY_GIANT;
        case RACE_HALFLING:  return CREATURE_FAMILY_HALFLING;
        default:            return CREATURE_FAMILY_NONE;
        }
}

const char *creature_family_name(CREATURE_FAMILY family)
{
        switch (family)
        {
        case CREATURE_FAMILY_NONE: return "none";
        case CREATURE_FAMILY_DWARF: return "dwarf";
        case CREATURE_FAMILY_ORC: return "orc";
        case CREATURE_FAMILY_HALF_ORC: return "half_orc";
        case CREATURE_FAMILY_GOBLIN: return "goblin";
        case CREATURE_FAMILY_HOBGOBLIN: return "hobgoblin";
        case CREATURE_FAMILY_OGRE: return "ogre";
        case CREATURE_FAMILY_TROLL: return "troll";
        case CREATURE_FAMILY_OGRE_MAGE: return "ogre_mage";
        case CREATURE_FAMILY_GIANT: return "giant";
        case CREATURE_FAMILY_TITAN: return "titan";
        case CREATURE_FAMILY_HUMAN: return "human";
        case CREATURE_FAMILY_ELF: return "elf";
        case CREATURE_FAMILY_LIZARDMAN: return "lizardman";
        case CREATURE_FAMILY_KOBOLD: return "kobold";
        case CREATURE_FAMILY_GNOLL: return "gnoll";
        case CREATURE_FAMILY_BUGBEAR: return "bugbear";
        case CREATURE_FAMILY_HALFLING: return "halfling";
        default: return "invalid";
        }
}

/* Passive creature traits live in code, alongside species and archetypes. */
typedef enum creature_trait_effect
{
        TRAIT_ATTACKER_MELEE,
        TRAIT_DEFENDER_MELEE,
        TRAIT_SUNLIGHT_ACCURACY,
        TRAIT_ECHOLOCATION,
        TRAIT_STRENGTH_DRAIN_TOUCH,
        TRAIT_NATURAL_ARMOUR,
        TRAIT_SILVER_CONTACT,
        TRAIT_STATUS_RESISTANCE_PERCENT,
        TRAIT_GHOUL_TOUCH_IMMUNITY,
        TRAIT_ILLUSION_IMMUNITY,
        TRAIT_LICH_CHILL_TOUCH,
        TRAIT_NATURAL_ATTACK_TYPE,
        TRAIT_SUNLIGHT_POWERLESS,
        TRAIT_COMBAT_VOLLEY_PERCENT,
        TRAIT_TURN_IMMUNITY,
        TRAIT_HOLY_WATER_IMMUNITY,
        TRAIT_COMBAT_CASTING_PERCENT,
        TRAIT_NATIVE_ROCK_PERCENT,
        TRAIT_NATIVE_ROCK_QUANTITY,
        TRAIT_NATIVE_REGEN_QUANTITY,
        TRAIT_NATIVE_BREATH_QUANTITY,
        TRAIT_BLOOD_REACTION_QUANTITY,
        TRAIT_FEAR_IMMUNITY,
        TRAIT_BODY_WEAPON_LOOT,
        TRAIT_NATIVE_HUG_DICE,
        TRAIT_STONE_DEATH,
        TRAIT_BOW_ACCURACY,
        TRAIT_NATURAL_DAMAGE_TYPE,
        TRAIT_LIGHT_RESPONSE,
        TRAIT_MAGICAL_NATURAL_ATTACKS,
        TRAIT_MELEE_ACCURACY,
        TRAIT_ARMED_MELEE_ACCURACY,
        TRAIT_ELEMENTAL_REACTION_QUANTITY,
        TRAIT_ARMED_MELEE_DAMAGE,
        TRAIT_DISPLACEMENT,
        TRAIT_SPELL_SAVE_BONUS,
        TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS,
        TRAIT_PARALYSING_GAS_CHANCE,
        TRAIT_PARALYSING_GAS_DAILY_USES,
        TRAIT_PARALYSING_GAS_COOLDOWN,
        TRAIT_PARALYSING_GAS_REQUIRES_HEAD,
        TRAIT_HOLY_TRUMPET_CHANCE,
        TRAIT_HOLY_TRUMPET_DAILY_USES,
        TRAIT_HOLY_TRUMPET_COOLDOWN,
        TRAIT_LULLING_DRONE_CHANCE,
        TRAIT_LULLING_DRONE_COOLDOWN,
        TRAIT_EFFECT_COUNT
} CREATURE_TRAIT_EFFECT;

typedef struct creature_trait_definition
{
        const char *name;
        CREATURE_TRAIT_EFFECT effect;
        const CREATURE_FAMILY *targets;
} CREATURE_TRAIT_DEFINITION;

typedef struct creature_trait_change
{
        CREATURE_TRAIT id;
        int value; /* Zero explicitly removes an inherited trait. */
} CREATURE_TRAIT_CHANGE;

typedef struct creature_trait_profile
{
        const char *name;
        const char *parent; /* Optional trait parent, within the same species. */
        const CREATURE_TRAIT_CHANGE *changes;
        size_t count;
} CREATURE_TRAIT_PROFILE;

typedef struct creature_trait_value
{
        CREATURE_TRAIT id;
        int value;
        const char *source;
        bool from_species;
} CREATURE_TRAIT_VALUE;

typedef struct creature_trait_set
{
        CREATURE_TRAIT_VALUE *entries;
        size_t count;
} CREATURE_TRAIT_SET;

#define TRAIT_ITEMS(list) (sizeof(list) / sizeof((list)[0]))

static const CREATURE_FAMILY trait_goblinoids[] =
{
        CREATURE_FAMILY_ORC, CREATURE_FAMILY_HALF_ORC,
        CREATURE_FAMILY_GOBLIN, CREATURE_FAMILY_HOBGOBLIN,
        CREATURE_FAMILY_NONE
};
static const CREATURE_FAMILY trait_giant_kin[] =
{
        CREATURE_FAMILY_OGRE, CREATURE_FAMILY_TROLL,
        CREATURE_FAMILY_OGRE_MAGE, CREATURE_FAMILY_GIANT,
        CREATURE_FAMILY_TITAN, CREATURE_FAMILY_NONE
};

static const CREATURE_FAMILY trait_gnome_enemies[] =
{
        CREATURE_FAMILY_GOBLIN, CREATURE_FAMILY_KOBOLD,
        CREATURE_FAMILY_NONE
};
static const CREATURE_FAMILY trait_gnome_giant_kin[] =
{
        CREATURE_FAMILY_GNOLL, CREATURE_FAMILY_BUGBEAR,
        CREATURE_FAMILY_OGRE, CREATURE_FAMILY_OGRE_MAGE,
        CREATURE_FAMILY_TROLL, CREATURE_FAMILY_GIANT, CREATURE_FAMILY_TITAN,
        CREATURE_FAMILY_NONE
};

/* IDs index this registry; they are never shifted into a flag word. */
static const CREATURE_TRAIT_DEFINITION creature_trait_definitions[] =
{
        {"goblinoid_training", TRAIT_ATTACKER_MELEE, trait_goblinoids},
        {"giant_defence", TRAIT_DEFENDER_MELEE, trait_giant_kin},
        {"sunlight_sensitive", TRAIT_SUNLIGHT_ACCURACY, NULL},
        {"echolocation", TRAIT_ECHOLOCATION, NULL},
        {"strength_drain_touch", TRAIT_STRENGTH_DRAIN_TOUCH, NULL},
        {"natural_armour", TRAIT_NATURAL_ARMOUR, NULL},
        {"silver_bypasses_nonmagic", TRAIT_SILVER_CONTACT, NULL},
        {"sleep_resistance_pct", TRAIT_STATUS_RESISTANCE_PERCENT, NULL},
        {"charm_resistance_pct", TRAIT_STATUS_RESISTANCE_PERCENT, NULL},
        {"ghoul_touch_immunity", TRAIT_GHOUL_TOUCH_IMMUNITY, NULL},
        {"illusion_immunity", TRAIT_ILLUSION_IMMUNITY, NULL},
        {"lich_chill_touch", TRAIT_LICH_CHILL_TOUCH, NULL},
        {"natural_attack_type", TRAIT_NATURAL_ATTACK_TYPE, NULL},
        {"sunlight_powerless", TRAIT_SUNLIGHT_POWERLESS, NULL},
        {"tail_spike_volley", TRAIT_COMBAT_VOLLEY_PERCENT, NULL},
        {"turn_immunity", TRAIT_TURN_IMMUNITY, NULL},
        {"holy_water_immunity", TRAIT_HOLY_WATER_IMMUNITY, NULL},
        {"psionic_casting_pct", TRAIT_COMBAT_CASTING_PERCENT, NULL},
        {"rock_throw_pct", TRAIT_NATIVE_ROCK_PERCENT, NULL},
        {"rock_damage_dice", TRAIT_NATIVE_ROCK_QUANTITY, NULL},
        {"rock_damage_sides", TRAIT_NATIVE_ROCK_QUANTITY, NULL},
        {"rock_daily_ammo", TRAIT_NATIVE_ROCK_QUANTITY, NULL},
        {"rock_catch_pct", TRAIT_NATIVE_ROCK_PERCENT, NULL},
        {"native_regen_min_hp", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"native_regen_level_divisor", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"native_regen_interval", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"native_regen_delay", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"native_regen_suppression", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"native_regen_trauma_interval", TRAIT_NATIVE_REGEN_QUANTITY, NULL},
        {"gnome_training", TRAIT_ATTACKER_MELEE, trait_gnome_enemies},
        {"gnome_giant_defence", TRAIT_DEFENDER_MELEE, trait_gnome_giant_kin},
        {"fire_breath_daily_casts", TRAIT_NATIVE_BREATH_QUANTITY, NULL},
        {"burning_blood_sides", TRAIT_BLOOD_REACTION_QUANTITY, NULL},
        {"fear_immunity", TRAIT_FEAR_IMMUNITY, NULL},
        {"acid_spit_daily_casts", TRAIT_NATIVE_BREATH_QUANTITY, NULL},
        {"body_weapon_loot", TRAIT_BODY_WEAPON_LOOT, NULL},
        {"bear_hug_dice", TRAIT_NATIVE_HUG_DICE, NULL},
        {"stone_death", TRAIT_STONE_DEATH, NULL},
        {"bow_accuracy", TRAIT_BOW_ACCURACY, NULL},
        {"natural_damage_type", TRAIT_NATURAL_DAMAGE_TYPE, NULL},
        {"shadow_light_response", TRAIT_LIGHT_RESPONSE, NULL},
        {"magical_natural_attacks", TRAIT_MAGICAL_NATURAL_ATTACKS, NULL},
        {"antimagic_gaze_pct", TRAIT_COMBAT_CASTING_PERCENT, NULL},
        {"melee_accuracy", TRAIT_MELEE_ACCURACY, NULL},
        {"armed_melee_accuracy", TRAIT_ARMED_MELEE_ACCURACY, NULL},
        {"electricity_healing_pct", TRAIT_ELEMENTAL_REACTION_QUANTITY, NULL},
        {"fire_cold_slow_ticks", TRAIT_ELEMENTAL_REACTION_QUANTITY, NULL},
        {"armed_melee_damage", TRAIT_ARMED_MELEE_DAMAGE, NULL},
        {"displacement_melee_penalty", TRAIT_DISPLACEMENT, NULL},
        {"spell_save_bonus_pct", TRAIT_SPELL_SAVE_BONUS, NULL},
        {"head_trauma_blocks_attacks", TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, NULL},
        {"paralysing_gas_chance", TRAIT_PARALYSING_GAS_CHANCE, NULL},
        {"paralysing_gas_daily_uses", TRAIT_PARALYSING_GAS_DAILY_USES, NULL},
        {"paralysing_gas_cooldown", TRAIT_PARALYSING_GAS_COOLDOWN, NULL},
        {"paralysing_gas_requires_head", TRAIT_PARALYSING_GAS_REQUIRES_HEAD, NULL},
        {"holy_trumpet_chance", TRAIT_HOLY_TRUMPET_CHANCE, NULL},
        {"holy_trumpet_daily_uses", TRAIT_HOLY_TRUMPET_DAILY_USES, NULL},
        {"holy_trumpet_cooldown", TRAIT_HOLY_TRUMPET_COOLDOWN, NULL},
        {"lulling_drone_chance", TRAIT_LULLING_DRONE_CHANCE, NULL},
        {"lulling_drone_cooldown", TRAIT_LULLING_DRONE_COOLDOWN, NULL}
};

static const CREATURE_TRAIT_CHANGE guardian_daemon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ACID_SPIT_DAILY_CASTS, 3}
};

static const CREATURE_TRAIT_CHANGE snake_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE wererat_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_SILVER_BYPASSES_NONMAGIC, 1}
};
static const CREATURE_TRAIT_CHANGE firedrake_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_FIRE_BREATH_DAILY_CASTS, 5},
        {CREATURE_TRAIT_BURNING_BLOOD_SIDES, 2}
};

static const CREATURE_TRAIT_CHANGE gnome_trait_changes[] =
{
        {CREATURE_TRAIT_GNOME_TRAINING, 1},
        {CREATURE_TRAIT_GNOME_GIANT_DEFENCE, -4}
};
static const CREATURE_TRAIT_CHANGE untrained_gnome_trait_changes[] =
{
        {CREATURE_TRAIT_GNOME_TRAINING, 0},
        {CREATURE_TRAIT_GNOME_GIANT_DEFENCE, 0}
};
static const CREATURE_TRAIT_CHANGE hermit_crab_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};

static const CREATURE_TRAIT_CHANGE dwarf_trait_changes[] =
{
        {CREATURE_TRAIT_GOBLINOID_TRAINING, 1},
        {CREATURE_TRAIT_GIANT_DEFENCE, -4}
};
static const CREATURE_TRAIT_CHANGE duergar_trait_changes[] =
{
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -2},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE untrained_dwarf_trait_changes[] =
{
        {CREATURE_TRAIT_GOBLINOID_TRAINING, 0},
        {CREATURE_TRAIT_GIANT_DEFENCE, 0}
};
static const CREATURE_TRAIT_CHANGE sunlight_trait_changes[] =
{
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -1}
};

static const CREATURE_TRAIT_CHANGE elf_trait_changes[] =
{
        {CREATURE_TRAIT_SLEEP_RESISTANCE_PERCENT, 90},
        {CREATURE_TRAIT_CHARM_RESISTANCE_PERCENT, 90},
        {CREATURE_TRAIT_GHOUL_TOUCH_IMMUNITY, 1}
};

static const CREATURE_TRAIT_CHANGE bat_trait_changes[] =
{
        {CREATURE_TRAIT_ECHOLOCATION, 1}
};
static const CREATURE_TRAIT_CHANGE spectre_trait_changes[] =
{
        {CREATURE_TRAIT_SUNLIGHT_POWERLESS, 1}
};
static const CREATURE_TRAIT_CHANGE shadow_trait_changes[] =
{
        {CREATURE_TRAIT_STRENGTH_DRAIN_TOUCH, 1}
};
static const CREATURE_TRAIT_CHANGE scaled_armour_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
static const CREATURE_TRAIT_CHANGE black_hatchling_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15}
};
static const CREATURE_TRAIT_CHANGE young_lightning_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
static const CREATURE_TRAIT_CHANGE animated_blade_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 3} /* slash with its own body */
};
static const CREATURE_TRAIT_CHANGE lich_trait_changes[] =
{
        {CREATURE_TRAIT_LICH_CHILL_TOUCH, 20}
};
static const CREATURE_TRAIT_CHANGE alhoon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_TURN_IMMUNITY, 1},
        {CREATURE_TRAIT_HOLY_WATER_IMMUNITY, 1},
        {CREATURE_TRAIT_PSIONIC_CASTING_PERCENT, 70}
};
static const CREATURE_TRAIT_CHANGE drow_trait_changes[] =
{
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -2}
};
static const CREATURE_TRAIT_CHANGE manticore_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_TAIL_SPIKE_VOLLEY, 25}
};
static const CREATURE_TRAIT_CHANGE cattle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE hill_giant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 15},
        {CREATURE_TRAIT_ROCK_DAMAGE_DICE, 2},
        {CREATURE_TRAIT_ROCK_DAMAGE_SIDES, 8},
        {CREATURE_TRAIT_ROCK_DAILY_AMMO, 4},
        {CREATURE_TRAIT_ROCK_CATCH_PERCENT, 30}
};
static const CREATURE_TRAIT_CHANGE stone_giant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -30},
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 15},
        {CREATURE_TRAIT_ROCK_DAMAGE_DICE, 3},
        {CREATURE_TRAIT_ROCK_DAMAGE_SIDES, 10},
        {CREATURE_TRAIT_ROCK_DAILY_AMMO, 8},
        {CREATURE_TRAIT_ROCK_CATCH_PERCENT, 90}
};
static const CREATURE_TRAIT_CHANGE troll_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 15},
        {CREATURE_TRAIT_ROCK_DAMAGE_DICE, 1},
        {CREATURE_TRAIT_ROCK_DAMAGE_SIDES, 8},
        {CREATURE_TRAIT_ROCK_DAILY_AMMO, 2},
        {CREATURE_TRAIT_ROCK_CATCH_PERCENT, 0},
        {CREATURE_TRAIT_NATIVE_REGEN_MIN_HP, 3},
        {CREATURE_TRAIT_NATIVE_REGEN_LEVEL_DIVISOR, 5},
        {CREATURE_TRAIT_NATIVE_REGEN_INTERVAL, 3},
        {CREATURE_TRAIT_NATIVE_REGEN_DELAY, 9},
        {CREATURE_TRAIT_NATIVE_REGEN_SUPPRESSION, 9},
        {CREATURE_TRAIT_NATIVE_REGEN_TRAUMA_INTERVAL, 30}
};
static const CREATURE_TRAIT_CHANGE infant_troll_trait_changes[] =
{
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 0}
};
static const CREATURE_TRAIT_CHANGE den_beast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE centaur_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE lizardman_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE darkenbeast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20},
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -1}
};

static const CREATURE_TRAIT_CHANGE wight_trait_changes[] =
{
        {CREATURE_TRAIT_SILVER_BYPASSES_NONMAGIC, 1}
};
static const CREATURE_TRAIT_CHANGE dracolich_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -40}
};

static const CREATURE_TRAIT_CHANGE dolphin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE blue_whale_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};
static const CREATURE_TRAIT_CHANGE hammerhead_shark_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE brown_bear_trait_changes[] =
{
        {CREATURE_TRAIT_BEAR_HUG_DICE, 2},
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_BODY_WEAPON_LOOT, 1}
};
static const CREATURE_TRAIT_CHANGE badger_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};
static const CREATURE_TRAIT_CHANGE lynx_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE mosquito_swarm_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 10}
};
static const CREATURE_TRAIT_CHANGE wasp_swarm_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 16}
};

static const CREATURE_TRAIT_CHANGE fox_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE wolf_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE warg_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE brownie_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE adult_green_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};

static const CREATURE_TRAIT_CHANGE web_spider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2}
};

static const CREATURE_TRAIT_CHANGE hunting_spider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE giant_spider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

static const CREATURE_TRAIT_CHANGE phase_spider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE giant_wasp_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 16}
};

static const CREATURE_TRAIT_CHANGE spider_empress_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};

static const CREATURE_TRAIT_CHANGE quasit_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};

static const CREATURE_TRAIT_CHANGE ki_rin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -25}
};

static const CREATURE_TRAIT_CHANGE wormkin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

static const CREATURE_TRAIT_CHANGE harpy_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE air_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};

static const CREATURE_TRAIT_CHANGE efreeti_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};

static const CREATURE_TRAIT_CHANGE roc_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

static const CREATURE_TRAIT_CHANGE green_hag_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -12}
};

static const CREATURE_TRAIT_CHANGE black_bear_trait_changes[] =
{
        {CREATURE_TRAIT_BEAR_HUG_DICE, 2},
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE hydra_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};

static const CREATURE_TRAIT_CHANGE imp_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};

static const CREATURE_TRAIT_CHANGE cooshee_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE antelope_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE bee_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 16}
};
static const CREATURE_TRAIT_CHANGE enchanted_oak_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};
static const CREATURE_TRAIT_CHANGE leprechaun_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2}
};
static const CREATURE_TRAIT_CHANGE horse_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE polar_bear_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_BEAR_HUG_DICE, 3}
};
static const CREATURE_TRAIT_CHANGE adult_cold_lightning_dragon_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
static const CREATURE_TRAIT_CHANGE frost_giant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 15},
        {CREATURE_TRAIT_ROCK_DAMAGE_DICE, 2},
        {CREATURE_TRAIT_ROCK_DAMAGE_SIDES, 10},
        {CREATURE_TRAIT_ROCK_DAILY_AMMO, 4},
        {CREATURE_TRAIT_ROCK_CATCH_PERCENT, 40}
};

static const CREATURE_TRAIT_CHANGE baaz_draconian_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_STONE_DEATH, 1}
};
static const CREATURE_TRAIT_CHANGE barracuda_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE dragon_turtle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};

static const CREATURE_TRAIT_CHANGE lair_gremlin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

static const CREATURE_TRAIT_CHANGE puma_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE cobra_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE wild_boar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE halfling_trait_changes[] =
{
        {CREATURE_TRAIT_BOW_ACCURACY, 3}
};
static const CREATURE_TRAIT_CHANGE untrained_halfling_trait_changes[] =
{
        {CREATURE_TRAIT_BOW_ACCURACY, 0}
};

static const CREATURE_TRAIT_CHANGE gold_dragon_kin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE ancient_gold_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -40}
};

static const CREATURE_TRAIT_CHANGE fire_hazard_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 6}, /* blast */
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_FIRE}
};

static const CREATURE_TRAIT_CHANGE lion_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE elephant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE constrictor_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE centipede_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -1}
};

static const CREATURE_TRAIT_CHANGE shadow_fiend_trait_changes[] =
{
        {CREATURE_TRAIT_SHADOW_LIGHT_RESPONSE, 1}
};

static const CREATURE_TRAIT_CHANGE purple_worm_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE adult_brass_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
/* A drider keeps these drow defences on its different body species. */
static const CREATURE_TRAIT_CHANGE drider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -2},
        {CREATURE_TRAIT_SLEEP_RESISTANCE_PERCENT, 90},
        {CREATURE_TRAIT_CHARM_RESISTANCE_PERCENT, 90},
        {CREATURE_TRAIT_GHOUL_TOUCH_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE dustdigger_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 10}
};
static const CREATURE_TRAIT_CHANGE camel_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE rolling_boulder_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8} /* The boulder crushes with its own body. */
};

static const CREATURE_TRAIT_CHANGE giant_frog_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE plesiosaur_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE giant_leech_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 10} /* A mouth bite, without invented fangs. */
};
static const CREATURE_TRAIT_CHANGE fire_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_FIRE},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE ancient_fire_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -40},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE fiery_demon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE hell_hound_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};
static const CREATURE_TRAIT_CHANGE acid_hell_hound_trait_changes[] =
{
        /* The saliva changes the bite's contact, without adding a second hit. */
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID}
};

static const CREATURE_TRAIT_CHANGE baby_beholder_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2}
};
static const CREATURE_TRAIT_CHANGE tame_lamia_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE stone_golem_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE beholder_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_ANTIMAGIC_GAZE_PERCENT, 25}
};
static const CREATURE_TRAIT_CHANGE lamia_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE mimic_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -1}
};

/* These golems are people; their construction does not remove mind or soul. */
static const CREATURE_TRAIT_CHANGE awakened_golem_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE four_armed_demon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE flesh_golem_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -1},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1},
        {CREATURE_TRAIT_ELECTRICITY_HEALING_PERCENT, 20},
        {CREATURE_TRAIT_FIRE_COLD_SLOW_TICKS, 1}
};
static const CREATURE_TRAIT_CHANGE grell_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE crocodile_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE flind_trait_changes[] =
{
        {CREATURE_TRAIT_MELEE_ACCURACY, 1}
};
static const CREATURE_TRAIT_CHANGE otyugh_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE ogre_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_ARMED_MELEE_ACCURACY, 2}
};

static const CREATURE_TRAIT_CHANGE treant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};
static const CREATURE_TRAIT_CHANGE yeti_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE minotaur_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_ARMED_MELEE_DAMAGE, 2}
};

static const CREATURE_TRAIT_CHANGE adult_crystal_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
static const CREATURE_TRAIT_CHANGE forest_giant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE tiger_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE decaying_demon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE morepork_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};

static const CREATURE_TRAIT_CHANGE goat_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE giant_land_crab_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8}
};
static const CREATURE_TRAIT_CHANGE sasquatch_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE boulder_beast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};
static const CREATURE_TRAIT_CHANGE sprite_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE swamp_thing_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};
static const CREATURE_TRAIT_CHANGE small_beast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -1}
};
static const CREATURE_TRAIT_CHANGE will_o_wisp_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -18},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 6},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ELECTRICITY}
};
static const CREATURE_TRAIT_CHANGE death_slaad_humanoid_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -14}
};
static const CREATURE_TRAIT_CHANGE shambling_mound_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8},
        {CREATURE_TRAIT_ELECTRICITY_HEALING_PERCENT, 20}
};
static const CREATURE_TRAIT_CHANGE gargoyle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE killer_frog_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2}
};
static const CREATURE_TRAIT_CHANGE giant_flytrap_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE black_pudding_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 12},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID}
};
static const CREATURE_TRAIT_CHANGE clay_golem_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE displacer_beast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_DISPLACEMENT_MELEE_PENALTY, -2},
        {CREATURE_TRAIT_SPELL_SAVE_BONUS_PERCENT, 10}
};

static const CREATURE_TRAIT_CHANGE water_spirit_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_WATER}
};
static const CREATURE_TRAIT_CHANGE pixie_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};

static const CREATURE_TRAIT_CHANGE mi_go_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE djinni_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

static const CREATURE_TRAIT_CHANGE mummy_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE neo_otyugh_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};

static const CREATURE_TRAIT_CHANGE animated_carpet_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7}
};
static const CREATURE_TRAIT_CHANGE caryatid_column_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE hieracosphinx_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -9}
};
static const CREATURE_TRAIT_CHANGE gynosphinx_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -11}
};
static const CREATURE_TRAIT_CHANGE criosphinx_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE androsphinx_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -12}
};

static const CREATURE_TRAIT_CHANGE undead_riding_beetle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};
static const CREATURE_TRAIT_CHANGE green_slime_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID}
};
static const CREATURE_TRAIT_CHANGE crystal_spider_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE umber_hulk_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};
static const CREATURE_TRAIT_CHANGE ochre_jelly_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID}
};

static const CREATURE_TRAIT_CHANGE draconian_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE undead_draconian_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE adult_black_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -16}
};
static const CREATURE_TRAIT_CHANGE ancient_green_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -25},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE phase_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE tiamat_avatar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -40},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE rhinoceros_beetle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};
static const CREATURE_TRAIT_CHANGE bulette_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -12}
};
static const CREATURE_TRAIT_CHANGE jaguar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE humanoid_gargantua_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_NATIVE_REGEN_MIN_HP, 4},
        {CREATURE_TRAIT_NATIVE_REGEN_LEVEL_DIVISOR, 25},
        {CREATURE_TRAIT_NATIVE_REGEN_INTERVAL, 3},
        {CREATURE_TRAIT_NATIVE_REGEN_DELAY, 3},
        {CREATURE_TRAIT_NATIVE_REGEN_SUPPRESSION, 3},
        {CREATURE_TRAIT_NATIVE_REGEN_TRAUMA_INTERVAL, 0}
};
static const CREATURE_TRAIT_CHANGE rakshasa_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -14},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE great_ape_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

static const CREATURE_TRAIT_CHANGE pteranodon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE psi_beast_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE fire_giant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_ROCK_THROW_PERCENT, 15},
        {CREATURE_TRAIT_ROCK_DAMAGE_DICE, 2},
        {CREATURE_TRAIT_ROCK_DAMAGE_SIDES, 10},
        {CREATURE_TRAIT_ROCK_DAILY_AMMO, 4},
        {CREATURE_TRAIT_ROCK_CATCH_PERCENT, 50}
};
static const CREATURE_TRAIT_CHANGE shark_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE water_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE earth_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE giant_squid_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};

static const CREATURE_TRAIT_CHANGE cyclops_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};
static const CREATURE_TRAIT_CHANGE mixed_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ENERGY},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE lightning_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ELECTRICITY},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE water_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_WATER},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE fire_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_FIRE},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE ice_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_COLD},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE stone_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE lightning_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ELECTRICITY},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE acid_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE lava_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_FIRE},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE mud_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE mist_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE energy_mote_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ENERGY},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE dust_elemental_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE air_eddy_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE chimera_goat_head_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE chimera_lion_head_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE chimera_dragon_head_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE modron_secundus_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -16},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};

static const CREATURE_TRAIT_CHANGE golden_handmaid_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE chimera_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE pegasus_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

static const CREATURE_TRAIT_CHANGE revenant_spirit_trait_changes[] =
{
        {CREATURE_TRAIT_TURN_IMMUNITY, 1},
        {CREATURE_TRAIT_HOLY_WATER_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE haunt_trait_changes[] =
{
        {CREATURE_TRAIT_TURN_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE crypt_thing_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_TURN_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE lurker_above_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT}
};
static const CREATURE_TRAIT_CHANGE succubus_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE gray_ooze_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_ACID}
};
static const CREATURE_TRAIT_CHANGE nightmare_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -14},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_FIRE},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE retriever_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8}
};
static const CREATURE_TRAIT_CHANGE cambion_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE water_weird_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_WATER},
        {CREATURE_TRAIT_MAGICAL_NATURAL_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE kampflut_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE animated_evil_tree_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT}
};
static const CREATURE_TRAIT_CHANGE death_knight_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_TURN_IMMUNITY, 1}
};

/* These reviewed profiles use existing traits; no shared defaults change. */
static const CREATURE_TRAIT_CHANGE night_hag_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE invisible_stalker_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 7},
        {CREATURE_TRAIT_NATURAL_DAMAGE_TYPE, NATURAL_DAMAGE_BLUNT}
};
static const CREATURE_TRAIT_CHANGE mind_flayer_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};
static const CREATURE_TRAIT_CHANGE small_red_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1},
        {CREATURE_TRAIT_BODY_WEAPON_LOOT, 1}
};

/* These Sewer profiles use the existing internal traits. */
static const CREATURE_TRAIT_CHANGE giant_ground_beetle_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE homunculus_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE chreffn_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2}
};
static const CREATURE_TRAIT_CHANGE sea_hag_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE guardian_naga_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE basilisk_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE lemure_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};
static const CREATURE_TRAIT_CHANGE ettin_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7}
};

/* These planar bodies keep their minds, souls and living inner organs. */
static const CREATURE_TRAIT_CHANGE modron_tridrone_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_quadrone_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_pentadrone_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1},
        {CREATURE_TRAIT_PARALYSING_GAS_CHANCE, 25},
        {CREATURE_TRAIT_PARALYSING_GAS_DAILY_USES, 5},
        {CREATURE_TRAIT_PARALYSING_GAS_COOLDOWN, 5}
};
static const CREATURE_TRAIT_CHANGE modron_decaton_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -8},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_nonaton_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_octon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -9},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_septon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -11},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_hexton_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -12},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_quinton_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -13},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE modron_tertian_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};
static const CREATURE_TRAIT_CHANGE primus_manifestation_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20},
        {CREATURE_TRAIT_FEAR_IMMUNITY, 1},
        {CREATURE_TRAIT_ILLUSION_IMMUNITY, 1}
};

static const CREATURE_TRAIT_CHANGE hollyphant_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -14},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1},
        {CREATURE_TRAIT_HOLY_TRUMPET_CHANCE, 25},
        {CREATURE_TRAIT_HOLY_TRUMPET_DAILY_USES, 3},
        {CREATURE_TRAIT_HOLY_TRUMPET_COOLDOWN, 5}
};
static const CREATURE_TRAIT_CHANGE planetar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -17}
};
static const CREATURE_TRAIT_CHANGE lesser_shedu_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};
static const CREATURE_TRAIT_CHANGE dretch_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE chasme_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -15},
        {CREATURE_TRAIT_LULLING_DRONE_CHANCE, 25},
        {CREATURE_TRAIT_LULLING_DRONE_COOLDOWN, 5}
};
static const CREATURE_TRAIT_CHANGE babau_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -13},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE yagnoloth_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -14}
};
static const CREATURE_TRAIT_CHANGE kostchtchie_avatar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};
static const CREATURE_TRAIT_CHANGE nabassu_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE cloud_dragon_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE solar_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -20}
};
static const CREATURE_TRAIT_CHANGE wemic_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE gorgimera_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1},
        {CREATURE_TRAIT_PARALYSING_GAS_CHANCE, 25},
        {CREATURE_TRAIT_PARALYSING_GAS_DAILY_USES, 2},
        {CREATURE_TRAIT_PARALYSING_GAS_COOLDOWN, 5},
        {CREATURE_TRAIT_PARALYSING_GAS_REQUIRES_HEAD, 1}
};
static const CREATURE_TRAIT_CHANGE giant_squirrel_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};

/* These living marine bodies share passive values, not active roles. */
static const CREATURE_TRAIT_CHANGE marine_head_traits[] =
{
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE marine_sting_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 16}
};
static const CREATURE_TRAIT_CHANGE marine_armour_2_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE marine_armour_3_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE marine_armour_4_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE marine_armour_5_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE marine_armour_6_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6}
};

/* Shared values leave these airborne species with their own anatomy. */
static const CREATURE_TRAIT_CHANGE abyss_armour_6_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE abyss_armour_7_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -7},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE abyss_armour_1_head_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -1},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE abyss_armour_3_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3}
};

/* These ray tooth plates use ordinary crushing contact, without venom. */
static const CREATURE_TRAIT_CHANGE ray_crushing_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -3},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8}
};

/* Soft fish and large tooth plates use ordinary crushing contact. */
static const CREATURE_TRAIT_CHANGE blobfish_crushing_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -1},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8}
};
static const CREATURE_TRAIT_CHANGE grouper_crushing_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8}
};

/* Tritons use the existing natural-armour trait. */
static const CREATURE_TRAIT_CHANGE triton_trait_changes[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5}
};

/* Add body defaults here when a reviewed species needs them. */
static const CREATURE_TRAIT_PROFILE species_trait_profiles[] =
{
        {NULL, NULL, NULL, 0}
};
static const CREATURE_TRAIT_CHANGE frozen_bird_head_traits[] =
{
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE fanged_beast_trait_changes[] =
{
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};

static const CREATURE_TRAIT_CHANGE titan_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -10}
};

static const CREATURE_TRAIT_CHANGE infernal_wyrm_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -40}
};

static const CREATURE_TRAIT_CHANGE kuo_toa_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_SUNLIGHT_SENSITIVE, -1}
};
static const CREATURE_TRAIT_CHANGE bullywug_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -4}
};
static const CREATURE_TRAIT_CHANGE kopru_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -2},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE vampiric_ixzan_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE water_naga_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -5},
        {CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS, 1}
};
static const CREATURE_TRAIT_CHANGE uchuulon_traits[] =
{
        {CREATURE_TRAIT_NATURAL_ARMOUR, -6},
        {CREATURE_TRAIT_NATURAL_ATTACK_TYPE, 8}
};

static const CREATURE_TRAIT_PROFILE archetype_trait_profiles[] =
{
        {"orange_grung", "green_grung", NULL, 0},
        {"red_grung", "green_grung", NULL, 0},
        {"gold_grung", "green_grung", NULL, 0},
        {"sharptooth_catfish", NULL, fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"conger_eel", NULL, fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"octopus", "giant_octopus", NULL, 0},
        {"giant_sea_python", "constrictor_snake", fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"kuo_toa", NULL, kuo_toa_traits, TRAIT_ITEMS(kuo_toa_traits)},
        {"bullywug", NULL, bullywug_traits, TRAIT_ITEMS(bullywug_traits)},
        {"kopru", NULL, kopru_traits, TRAIT_ITEMS(kopru_traits)},
        {"vampiric_ixzan", NULL, vampiric_ixzan_traits, TRAIT_ITEMS(vampiric_ixzan_traits)},
        {"water_naga", NULL, water_naga_traits, TRAIT_ITEMS(water_naga_traits)},
        {"uchuulon", NULL, uchuulon_traits, TRAIT_ITEMS(uchuulon_traits)},
        {"infernal_wyrm", NULL, infernal_wyrm_traits, TRAIT_ITEMS(infernal_wyrm_traits)},
        {"titan", NULL, titan_traits, TRAIT_ITEMS(titan_traits)},
        {"three_headed_dragon", NULL, scaled_armour_trait_changes, TRAIT_ITEMS(scaled_armour_trait_changes)},
        {"medusa", NULL, marine_armour_5_head_traits, TRAIT_ITEMS(marine_armour_5_head_traits)},
        {"acid_worm", NULL, fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"grottobeast", NULL, fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"bleeder", NULL, fanged_beast_trait_changes, TRAIT_ITEMS(fanged_beast_trait_changes)},
        {"great_wolf", "wolf", NULL, 0},
        {"giant_bat", "bat", NULL, 0},
        {"bat_humanoid", NULL, frozen_bird_head_traits, TRAIT_ITEMS(frozen_bird_head_traits)},
        {"penguin", NULL, frozen_bird_head_traits, TRAIT_ITEMS(frozen_bird_head_traits)},
        {"arctic_wolf", "wolf", NULL, 0},
        {"leopard_seal", "selkie_seal_form", NULL, 0},
        {"young_white_dragon", NULL, abyss_armour_7_head_traits, TRAIT_ITEMS(abyss_armour_7_head_traits)},
        {"arctic_fox", "fox", NULL, 0},
        {"kraken", "giant_squid", NULL, 0},
        {"shore_crab", "giant_land_crab", NULL, 0},
        {"lava_lizard", "lizard", NULL, 0},
        {"tiger_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"mako_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"terra_kan", "hawk", NULL, 0},
        {"orca", "sperm_whale", NULL, 0},
        {"cave_bear", "brown_bear", NULL, 0},
        {"sperm_whale", NULL, abyss_armour_6_head_traits, TRAIT_ITEMS(abyss_armour_6_head_traits)},
        {"merfolk", NULL, abyss_armour_3_traits, TRAIT_ITEMS(abyss_armour_3_traits)},
        {"selkie_seal_form", NULL, marine_armour_5_head_traits, TRAIT_ITEMS(marine_armour_5_head_traits)},
        {"tokoloshe", NULL, marine_head_traits, TRAIT_ITEMS(marine_head_traits)},
        {"vodyanoi", "umber_hulk", NULL, 0},
        {"aboleth", NULL, marine_armour_6_traits, TRAIT_ITEMS(marine_armour_6_traits)},
        {"giant_alligator_gar", NULL, abyss_armour_6_head_traits, TRAIT_ITEMS(abyss_armour_6_head_traits)},
        {"sea_elf", "elf", NULL, 0},
        {"blue_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"stingray", NULL, ray_crushing_trait_changes, TRAIT_ITEMS(ray_crushing_trait_changes)},
        {"skate", NULL, ray_crushing_trait_changes, TRAIT_ITEMS(ray_crushing_trait_changes)},
        {"great_white_shark", NULL, marine_armour_5_head_traits, TRAIT_ITEMS(marine_armour_5_head_traits)},
        {"triton", NULL, triton_trait_changes, TRAIT_ITEMS(triton_trait_changes)},
        {"green_grung", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"sheepshead_wrasse", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"giant_blobfish", NULL, blobfish_crushing_traits, TRAIT_ITEMS(blobfish_crushing_traits)},
        {"moonfish", NULL, ray_crushing_trait_changes, TRAIT_ITEMS(ray_crushing_trait_changes)},
        {"goliath_grouper", NULL, grouper_crushing_traits, TRAIT_ITEMS(grouper_crushing_traits)},
        {"stargazer", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"sawfish", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"cownose_ray", NULL, ray_crushing_trait_changes, TRAIT_ITEMS(ray_crushing_trait_changes)},
        {"frilled_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"lemon_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"fiddler_ray", NULL, ray_crushing_trait_changes, TRAIT_ITEMS(ray_crushing_trait_changes)},
        {"sahuagin_baron", "sahuagin", NULL, 0},
        {"mesopelag", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"pycnogonid", NULL, abyss_armour_6_head_traits, TRAIT_ITEMS(abyss_armour_6_head_traits)},
        {"copepod", NULL, abyss_armour_3_traits, TRAIT_ITEMS(abyss_armour_3_traits)},
        {"abralia", NULL, abyss_armour_7_head_traits, TRAIT_ITEMS(abyss_armour_7_head_traits)},
        {"veranyi", NULL, marine_armour_2_head_traits, TRAIT_ITEMS(marine_armour_2_head_traits)},
        {"calimedusae", NULL, marine_armour_2_head_traits, TRAIT_ITEMS(marine_armour_2_head_traits)},
        {"cyclothone", NULL, abyss_armour_1_head_traits, TRAIT_ITEMS(abyss_armour_1_head_traits)},
        {"megalodon", NULL, marine_armour_5_head_traits, TRAIT_ITEMS(marine_armour_5_head_traits)},
        {"vampire_squid", NULL, marine_head_traits, TRAIT_ITEMS(marine_head_traits)},
        {"sea_anemone", NULL, marine_sting_traits, TRAIT_ITEMS(marine_sting_traits)},
        {"anglerfish", NULL, marine_armour_2_head_traits, TRAIT_ITEMS(marine_armour_2_head_traits)},
        {"sea_lion", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"sea_snake", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"cuttlefish", NULL, marine_armour_2_head_traits, TRAIT_ITEMS(marine_armour_2_head_traits)},
        {"jellyfish", NULL, marine_sting_traits, TRAIT_ITEMS(marine_sting_traits)},
        {"goblin_shark", NULL, marine_armour_4_head_traits, TRAIT_ITEMS(marine_armour_4_head_traits)},
        {"porcupinefish", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"giant_lobster", NULL, marine_armour_6_traits, TRAIT_ITEMS(marine_armour_6_traits)},
        {"giant_octopus", NULL, marine_armour_3_head_traits, TRAIT_ITEMS(marine_armour_3_head_traits)},
        {"sahuagin", NULL, marine_armour_5_head_traits, TRAIT_ITEMS(marine_armour_5_head_traits)},
        {"hollyphant", NULL, hollyphant_trait_changes, TRAIT_ITEMS(hollyphant_trait_changes)},
        {"planetar", NULL, planetar_trait_changes, TRAIT_ITEMS(planetar_trait_changes)},
        {"lesser_shedu", NULL, lesser_shedu_trait_changes, TRAIT_ITEMS(lesser_shedu_trait_changes)},
        {"dretch", NULL, dretch_trait_changes, TRAIT_ITEMS(dretch_trait_changes)},
        {"chasme", NULL, chasme_trait_changes, TRAIT_ITEMS(chasme_trait_changes)},
        {"babau", NULL, babau_trait_changes, TRAIT_ITEMS(babau_trait_changes)},
        {"yagnoloth", NULL, yagnoloth_trait_changes, TRAIT_ITEMS(yagnoloth_trait_changes)},
        {"kostchtchie_avatar", NULL, kostchtchie_avatar_trait_changes, TRAIT_ITEMS(kostchtchie_avatar_trait_changes)},
        {"nabassu", NULL, nabassu_trait_changes, TRAIT_ITEMS(nabassu_trait_changes)},
        {"cloud_dragon", NULL, cloud_dragon_trait_changes, TRAIT_ITEMS(cloud_dragon_trait_changes)},
        {"solar", NULL, solar_trait_changes, TRAIT_ITEMS(solar_trait_changes)},
        {"wemic", NULL, wemic_trait_changes, TRAIT_ITEMS(wemic_trait_changes)},
        {"gorgimera", NULL, gorgimera_trait_changes, TRAIT_ITEMS(gorgimera_trait_changes)},
        {"giant_squirrel", NULL, giant_squirrel_trait_changes, TRAIT_ITEMS(giant_squirrel_trait_changes)},
        {"modron_tridrone", NULL, modron_tridrone_trait_changes, TRAIT_ITEMS(modron_tridrone_trait_changes)},
        {"modron_quadrone", NULL, modron_quadrone_trait_changes, TRAIT_ITEMS(modron_quadrone_trait_changes)},
        {"modron_pentadrone", NULL, modron_pentadrone_trait_changes, TRAIT_ITEMS(modron_pentadrone_trait_changes)},
        {"modron_decaton", NULL, modron_decaton_trait_changes, TRAIT_ITEMS(modron_decaton_trait_changes)},
        {"modron_nonaton", NULL, modron_nonaton_trait_changes, TRAIT_ITEMS(modron_nonaton_trait_changes)},
        {"modron_octon", NULL, modron_octon_trait_changes, TRAIT_ITEMS(modron_octon_trait_changes)},
        {"modron_septon", NULL, modron_septon_trait_changes, TRAIT_ITEMS(modron_septon_trait_changes)},
        {"modron_hexton", NULL, modron_hexton_trait_changes, TRAIT_ITEMS(modron_hexton_trait_changes)},
        {"modron_quinton", NULL, modron_quinton_trait_changes, TRAIT_ITEMS(modron_quinton_trait_changes)},
        {"modron_tertian", NULL, modron_tertian_trait_changes, TRAIT_ITEMS(modron_tertian_trait_changes)},
        {"primus_manifestation", NULL, primus_manifestation_trait_changes, TRAIT_ITEMS(primus_manifestation_trait_changes)},
        {"giant_ground_beetle", NULL, giant_ground_beetle_trait_changes, TRAIT_ITEMS(giant_ground_beetle_trait_changes)},
        {"homunculus", NULL, homunculus_trait_changes, TRAIT_ITEMS(homunculus_trait_changes)},
        {"chreffn", NULL, chreffn_trait_changes, TRAIT_ITEMS(chreffn_trait_changes)},
        {"sea_hag", NULL, sea_hag_trait_changes, TRAIT_ITEMS(sea_hag_trait_changes)},
        {"guardian_naga", NULL, guardian_naga_trait_changes, TRAIT_ITEMS(guardian_naga_trait_changes)},
        {"basilisk", NULL, basilisk_trait_changes, TRAIT_ITEMS(basilisk_trait_changes)},
        {"lemure", NULL, lemure_trait_changes, TRAIT_ITEMS(lemure_trait_changes)},
        {"ettin", NULL, ettin_trait_changes, TRAIT_ITEMS(ettin_trait_changes)},
        {"night_hag", NULL, night_hag_trait_changes, TRAIT_ITEMS(night_hag_trait_changes)},
        {"invisible_stalker", NULL, invisible_stalker_trait_changes, TRAIT_ITEMS(invisible_stalker_trait_changes)},
        {"mind_flayer", NULL, mind_flayer_trait_changes, TRAIT_ITEMS(mind_flayer_trait_changes)},
        {"small_red_dragon", NULL, small_red_dragon_trait_changes, TRAIT_ITEMS(small_red_dragon_trait_changes)},
        {"revenant_spirit", NULL, revenant_spirit_trait_changes, TRAIT_ITEMS(revenant_spirit_trait_changes)},
        {"haunt", NULL, haunt_trait_changes, TRAIT_ITEMS(haunt_trait_changes)},
        {"crypt_thing", NULL, crypt_thing_trait_changes, TRAIT_ITEMS(crypt_thing_trait_changes)},
        {"lurker_above", NULL, lurker_above_trait_changes, TRAIT_ITEMS(lurker_above_trait_changes)},
        {"succubus", NULL, succubus_trait_changes, TRAIT_ITEMS(succubus_trait_changes)},
        {"gray_ooze", NULL, gray_ooze_trait_changes, TRAIT_ITEMS(gray_ooze_trait_changes)},
        {"nightmare", NULL, nightmare_trait_changes, TRAIT_ITEMS(nightmare_trait_changes)},
        {"retriever", NULL, retriever_trait_changes, TRAIT_ITEMS(retriever_trait_changes)},
        {"cambion", NULL, cambion_trait_changes, TRAIT_ITEMS(cambion_trait_changes)},
        {"water_weird", NULL, water_weird_trait_changes, TRAIT_ITEMS(water_weird_trait_changes)},
        {"kampflut", NULL, kampflut_trait_changes, TRAIT_ITEMS(kampflut_trait_changes)},
        {"animated_evil_tree", NULL, animated_evil_tree_trait_changes, TRAIT_ITEMS(animated_evil_tree_trait_changes)},
        {"death_knight", NULL, death_knight_trait_changes, TRAIT_ITEMS(death_knight_trait_changes)},
        {"golden_handmaid", NULL, golden_handmaid_trait_changes, TRAIT_ITEMS(golden_handmaid_trait_changes)},
        {"chimera", NULL, chimera_trait_changes, TRAIT_ITEMS(chimera_trait_changes)},
        {"pegasus", NULL, pegasus_trait_changes, TRAIT_ITEMS(pegasus_trait_changes)},
        {"chimera_goat_head", NULL, chimera_goat_head_trait_changes, TRAIT_ITEMS(chimera_goat_head_trait_changes)},
        {"chimera_lion_head", NULL, chimera_lion_head_trait_changes, TRAIT_ITEMS(chimera_lion_head_trait_changes)},
        {"chimera_dragon_head", NULL, chimera_dragon_head_trait_changes, TRAIT_ITEMS(chimera_dragon_head_trait_changes)},
        {"modron_secundus", NULL, modron_secundus_trait_changes, TRAIT_ITEMS(modron_secundus_trait_changes)},
        {"cyclops", NULL, cyclops_trait_changes, TRAIT_ITEMS(cyclops_trait_changes)},
        {"mixed_elemental", NULL, mixed_elemental_trait_changes, TRAIT_ITEMS(mixed_elemental_trait_changes)},
        {"lightning_elemental", NULL, lightning_elemental_trait_changes, TRAIT_ITEMS(lightning_elemental_trait_changes)},
        {"water_mote", NULL, water_mote_trait_changes, TRAIT_ITEMS(water_mote_trait_changes)},
        {"fire_mote", NULL, fire_mote_trait_changes, TRAIT_ITEMS(fire_mote_trait_changes)},
        {"ice_mote", NULL, ice_mote_trait_changes, TRAIT_ITEMS(ice_mote_trait_changes)},
        {"stone_mote", NULL, stone_mote_trait_changes, TRAIT_ITEMS(stone_mote_trait_changes)},
        {"lightning_mote", NULL, lightning_mote_trait_changes, TRAIT_ITEMS(lightning_mote_trait_changes)},
        {"acid_elemental", NULL, acid_elemental_trait_changes, TRAIT_ITEMS(acid_elemental_trait_changes)},
        {"lava_elemental", NULL, lava_elemental_trait_changes, TRAIT_ITEMS(lava_elemental_trait_changes)},
        {"mud_elemental", NULL, mud_elemental_trait_changes, TRAIT_ITEMS(mud_elemental_trait_changes)},
        {"mist_elemental", NULL, mist_elemental_trait_changes, TRAIT_ITEMS(mist_elemental_trait_changes)},
        {"energy_mote", NULL, energy_mote_trait_changes, TRAIT_ITEMS(energy_mote_trait_changes)},
        {"dust_elemental", NULL, dust_elemental_trait_changes, TRAIT_ITEMS(dust_elemental_trait_changes)},
        {"air_eddy", NULL, air_eddy_trait_changes, TRAIT_ITEMS(air_eddy_trait_changes)},
        {"pteranodon", NULL, pteranodon_trait_changes, TRAIT_ITEMS(pteranodon_trait_changes)},
        {"psi_beast", NULL, psi_beast_trait_changes, TRAIT_ITEMS(psi_beast_trait_changes)},
        {"fire_giant", NULL, fire_giant_trait_changes, TRAIT_ITEMS(fire_giant_trait_changes)},
        {"shark", NULL, shark_trait_changes, TRAIT_ITEMS(shark_trait_changes)},
        {"water_elemental", NULL, water_elemental_trait_changes, TRAIT_ITEMS(water_elemental_trait_changes)},
        {"earth_elemental", NULL, earth_elemental_trait_changes, TRAIT_ITEMS(earth_elemental_trait_changes)},
        {"giant_squid", NULL, giant_squid_trait_changes, TRAIT_ITEMS(giant_squid_trait_changes)},
        {"rhinoceros_beetle", NULL, rhinoceros_beetle_trait_changes, TRAIT_ITEMS(rhinoceros_beetle_trait_changes)},
        {"bulette", NULL, bulette_trait_changes, TRAIT_ITEMS(bulette_trait_changes)},
        {"jaguar", NULL, jaguar_trait_changes, TRAIT_ITEMS(jaguar_trait_changes)},
        {"humanoid_gargantua", NULL, humanoid_gargantua_trait_changes, TRAIT_ITEMS(humanoid_gargantua_trait_changes)},
        {"rakshasa", NULL, rakshasa_trait_changes, TRAIT_ITEMS(rakshasa_trait_changes)},
        {"great_ape", NULL, great_ape_trait_changes, TRAIT_ITEMS(great_ape_trait_changes)},
        {"draconian", NULL, draconian_trait_changes, TRAIT_ITEMS(draconian_trait_changes)},
        {"undead_draconian", NULL, undead_draconian_trait_changes, TRAIT_ITEMS(undead_draconian_trait_changes)},
        {"adult_black_dragon", NULL, adult_black_dragon_trait_changes, TRAIT_ITEMS(adult_black_dragon_trait_changes)},
        {"ancient_green_dragon", NULL, ancient_green_dragon_trait_changes, TRAIT_ITEMS(ancient_green_dragon_trait_changes)},
        {"phase_dragon", NULL, phase_dragon_trait_changes, TRAIT_ITEMS(phase_dragon_trait_changes)},
        {"tiamat_avatar", NULL, tiamat_avatar_trait_changes, TRAIT_ITEMS(tiamat_avatar_trait_changes)},
        {"undead_riding_beetle", NULL, undead_riding_beetle_trait_changes, TRAIT_ITEMS(undead_riding_beetle_trait_changes)},
        {"green_slime", NULL, green_slime_trait_changes, TRAIT_ITEMS(green_slime_trait_changes)},
        {"crystal_spider", NULL, crystal_spider_trait_changes, TRAIT_ITEMS(crystal_spider_trait_changes)},
        {"umber_hulk", NULL, umber_hulk_trait_changes, TRAIT_ITEMS(umber_hulk_trait_changes)},
        {"ochre_jelly", NULL, ochre_jelly_trait_changes, TRAIT_ITEMS(ochre_jelly_trait_changes)},
        {"animated_carpet", NULL, animated_carpet_trait_changes, TRAIT_ITEMS(animated_carpet_trait_changes)},
        {"caryatid_column", NULL, caryatid_column_trait_changes, TRAIT_ITEMS(caryatid_column_trait_changes)},
        {"hieracosphinx", NULL, hieracosphinx_trait_changes, TRAIT_ITEMS(hieracosphinx_trait_changes)},
        {"gynosphinx", NULL, gynosphinx_trait_changes, TRAIT_ITEMS(gynosphinx_trait_changes)},
        {"criosphinx", NULL, criosphinx_trait_changes, TRAIT_ITEMS(criosphinx_trait_changes)},
        {"androsphinx", NULL, androsphinx_trait_changes, TRAIT_ITEMS(androsphinx_trait_changes)},
        {"mummy", NULL, mummy_trait_changes, TRAIT_ITEMS(mummy_trait_changes)},
        {"neo_otyugh", "otyugh", neo_otyugh_trait_changes, TRAIT_ITEMS(neo_otyugh_trait_changes)},
        {"mi_go", NULL, mi_go_trait_changes, TRAIT_ITEMS(mi_go_trait_changes)},
        {"djinni", NULL, djinni_trait_changes, TRAIT_ITEMS(djinni_trait_changes)},
        {"water_spirit", NULL, water_spirit_trait_changes,
            TRAIT_ITEMS(water_spirit_trait_changes)},
        {"pixie", NULL, pixie_trait_changes, TRAIT_ITEMS(pixie_trait_changes)},
        {"killer_frog", NULL, killer_frog_trait_changes,
            TRAIT_ITEMS(killer_frog_trait_changes)},
        {"giant_flytrap", NULL, giant_flytrap_trait_changes,
            TRAIT_ITEMS(giant_flytrap_trait_changes)},
        {"black_pudding", NULL, black_pudding_trait_changes,
            TRAIT_ITEMS(black_pudding_trait_changes)},
        {"clay_golem", NULL, clay_golem_trait_changes,
            TRAIT_ITEMS(clay_golem_trait_changes)},
        {"displacer_beast", NULL, displacer_beast_trait_changes,
            TRAIT_ITEMS(displacer_beast_trait_changes)},
        {"swamp_thing", NULL, swamp_thing_trait_changes,
            TRAIT_ITEMS(swamp_thing_trait_changes)},
        {"small_beast", NULL, small_beast_trait_changes,
            TRAIT_ITEMS(small_beast_trait_changes)},
        {"will_o_wisp", NULL, will_o_wisp_trait_changes,
            TRAIT_ITEMS(will_o_wisp_trait_changes)},
        {"death_slaad_humanoid", NULL, death_slaad_humanoid_trait_changes,
            TRAIT_ITEMS(death_slaad_humanoid_trait_changes)},
        {"shambling_mound", NULL, shambling_mound_trait_changes,
            TRAIT_ITEMS(shambling_mound_trait_changes)},
        {"gargoyle", NULL, gargoyle_trait_changes,
            TRAIT_ITEMS(gargoyle_trait_changes)},
        {"marsh_giant", "forest_giant", NULL, 0},
        {"goat", NULL, goat_trait_changes,
            TRAIT_ITEMS(goat_trait_changes)},
        {"giant_land_crab", NULL, giant_land_crab_trait_changes,
            TRAIT_ITEMS(giant_land_crab_trait_changes)},
        {"sasquatch", NULL, sasquatch_trait_changes,
            TRAIT_ITEMS(sasquatch_trait_changes)},
        {"boulder_beast", NULL, boulder_beast_trait_changes,
            TRAIT_ITEMS(boulder_beast_trait_changes)},
        {"sprite", NULL, sprite_trait_changes,
            TRAIT_ITEMS(sprite_trait_changes)},
        {"adult_crystal_dragon", NULL, adult_crystal_dragon_trait_changes,
            TRAIT_ITEMS(adult_crystal_dragon_trait_changes)},
        {"forest_giant", NULL, forest_giant_trait_changes,
            TRAIT_ITEMS(forest_giant_trait_changes)},
        {"tiger", NULL, tiger_trait_changes,
            TRAIT_ITEMS(tiger_trait_changes)},
        {"decaying_demon", NULL, decaying_demon_trait_changes,
            TRAIT_ITEMS(decaying_demon_trait_changes)},
        {"morepork", NULL, morepork_trait_changes,
            TRAIT_ITEMS(morepork_trait_changes)},
        {"treant", NULL, treant_trait_changes,
            TRAIT_ITEMS(treant_trait_changes)},
        {"yeti", NULL, yeti_trait_changes,
            TRAIT_ITEMS(yeti_trait_changes)},
        {"minotaur", NULL, minotaur_trait_changes,
            TRAIT_ITEMS(minotaur_trait_changes)},
        {"four_armed_demon", NULL, four_armed_demon_trait_changes,
            TRAIT_ITEMS(four_armed_demon_trait_changes)},
        {"flesh_golem", NULL, flesh_golem_trait_changes,
            TRAIT_ITEMS(flesh_golem_trait_changes)},
        {"grell", NULL, grell_trait_changes,
            TRAIT_ITEMS(grell_trait_changes)},
        {"crocodile", NULL, crocodile_trait_changes,
            TRAIT_ITEMS(crocodile_trait_changes)},
        {"flind", NULL, flind_trait_changes,
            TRAIT_ITEMS(flind_trait_changes)},
        {"otyugh", NULL, otyugh_trait_changes,
            TRAIT_ITEMS(otyugh_trait_changes)},
        {"ogre", NULL, ogre_trait_changes,
            TRAIT_ITEMS(ogre_trait_changes)},
        {"awakened_golem", NULL, awakened_golem_trait_changes,
            TRAIT_ITEMS(awakened_golem_trait_changes)},
        {"beholder", NULL, beholder_trait_changes,
            TRAIT_ITEMS(beholder_trait_changes)},
        {"lamia", NULL, lamia_trait_changes,
            TRAIT_ITEMS(lamia_trait_changes)},
        {"mimic", NULL, mimic_trait_changes,
            TRAIT_ITEMS(mimic_trait_changes)},
        {"baby_beholder", NULL, baby_beholder_trait_changes,
            TRAIT_ITEMS(baby_beholder_trait_changes)},
        {"tame_lamia", NULL, tame_lamia_trait_changes,
            TRAIT_ITEMS(tame_lamia_trait_changes)},
        {"stone_golem", NULL, stone_golem_trait_changes,
            TRAIT_ITEMS(stone_golem_trait_changes)},
        {"ancient_fire_dragon", NULL, ancient_fire_dragon_trait_changes,
            TRAIT_ITEMS(ancient_fire_dragon_trait_changes)},
        {"fiery_demon", NULL, fiery_demon_trait_changes,
            TRAIT_ITEMS(fiery_demon_trait_changes)},
        {"hell_hound", NULL, hell_hound_trait_changes,
            TRAIT_ITEMS(hell_hound_trait_changes)},
        {"acid_hell_hound", "hell_hound", acid_hell_hound_trait_changes,
            TRAIT_ITEMS(acid_hell_hound_trait_changes)},
        {"giant_frog", NULL, giant_frog_trait_changes,
            TRAIT_ITEMS(giant_frog_trait_changes)},
        {"plesiosaur", NULL, plesiosaur_trait_changes,
            TRAIT_ITEMS(plesiosaur_trait_changes)},
        {"giant_leech", NULL, giant_leech_trait_changes,
            TRAIT_ITEMS(giant_leech_trait_changes)},
        {"fire_elemental", NULL, fire_elemental_trait_changes,
            TRAIT_ITEMS(fire_elemental_trait_changes)},
        {"rolling_boulder", NULL, rolling_boulder_trait_changes,
            TRAIT_ITEMS(rolling_boulder_trait_changes)},
        {"purple_worm", NULL, purple_worm_trait_changes,
            TRAIT_ITEMS(purple_worm_trait_changes)},
        {"adult_brass_dragon", NULL, adult_brass_dragon_trait_changes,
            TRAIT_ITEMS(adult_brass_dragon_trait_changes)},
        {"drider", NULL, drider_trait_changes,
            TRAIT_ITEMS(drider_trait_changes)},
        {"dustdigger", NULL, dustdigger_trait_changes,
            TRAIT_ITEMS(dustdigger_trait_changes)},
        {"camel", NULL, camel_trait_changes,
            TRAIT_ITEMS(camel_trait_changes)},
        {"guardian_daemon", NULL, guardian_daemon_trait_changes,
            TRAIT_ITEMS(guardian_daemon_trait_changes)},
        {"snake", NULL, snake_trait_changes, TRAIT_ITEMS(snake_trait_changes)},
        {"wererat", NULL, wererat_trait_changes, TRAIT_ITEMS(wererat_trait_changes)},
        {"firedrake", NULL, firedrake_trait_changes, TRAIT_ITEMS(firedrake_trait_changes)},
        {"gnome", NULL, gnome_trait_changes, TRAIT_ITEMS(gnome_trait_changes)},
        {"gnome_untrained", "gnome", untrained_gnome_trait_changes,
            TRAIT_ITEMS(untrained_gnome_trait_changes)},
        {"giant_hermit_crab", NULL, hermit_crab_trait_changes,
            TRAIT_ITEMS(hermit_crab_trait_changes)},
        {"dwarf", NULL, dwarf_trait_changes, TRAIT_ITEMS(dwarf_trait_changes)},
        {"duergar", "dwarf", duergar_trait_changes, TRAIT_ITEMS(duergar_trait_changes)},
        {"dwarf_untrained", "dwarf", untrained_dwarf_trait_changes,
            TRAIT_ITEMS(untrained_dwarf_trait_changes)},
        {"goblin", NULL, sunlight_trait_changes, TRAIT_ITEMS(sunlight_trait_changes)},
        {"kobold", NULL, sunlight_trait_changes, TRAIT_ITEMS(sunlight_trait_changes)},
        {"shadow_fiend", NULL, shadow_fiend_trait_changes, TRAIT_ITEMS(shadow_fiend_trait_changes)},
        {"constrictor_snake", NULL, constrictor_trait_changes, TRAIT_ITEMS(constrictor_trait_changes)},
        {"centipede", NULL, centipede_trait_changes, TRAIT_ITEMS(centipede_trait_changes)},
        {"lightning_bat", "bat", NULL, 0},
        {"fire_breathing_bat", "bat", NULL, 0},
        {"gas_bat", "bat", NULL, 0},
        {"frost_bat", "bat", NULL, 0},
        {"acid_bat", "bat", NULL, 0},
        {"lion", NULL, lion_trait_changes, TRAIT_ITEMS(lion_trait_changes)},
        {"elephant", NULL, elephant_trait_changes, TRAIT_ITEMS(elephant_trait_changes)},
        {"mastodon", "elephant", NULL, 0},
        {"fire_hazard", NULL, fire_hazard_trait_changes,
            TRAIT_ITEMS(fire_hazard_trait_changes)},
        {"gold_dragon_kin", NULL, gold_dragon_kin_trait_changes,
            TRAIT_ITEMS(gold_dragon_kin_trait_changes)},
        {"ancient_gold_dragon", NULL, ancient_gold_dragon_trait_changes,
            TRAIT_ITEMS(ancient_gold_dragon_trait_changes)},
        {"halfling", NULL, halfling_trait_changes, TRAIT_ITEMS(halfling_trait_changes)},
        {"halfling_untrained", "halfling", untrained_halfling_trait_changes,
            TRAIT_ITEMS(untrained_halfling_trait_changes)},
        {"orc", NULL, sunlight_trait_changes, TRAIT_ITEMS(sunlight_trait_changes)},
        {"elf", NULL, elf_trait_changes, TRAIT_ITEMS(elf_trait_changes)},
        {"valley_elf", "elf", NULL, 0},
        {"drow", "elf", drow_trait_changes, TRAIT_ITEMS(drow_trait_changes)},
        {"alhoon", NULL, alhoon_trait_changes, TRAIT_ITEMS(alhoon_trait_changes)},
        {"bat", NULL, bat_trait_changes, TRAIT_ITEMS(bat_trait_changes)},
        {"talking_bat", "bat", NULL, 0},
        {"vampire_bat_form", NULL, bat_trait_changes, TRAIT_ITEMS(bat_trait_changes)},
        {"shadow", NULL, shadow_trait_changes, TRAIT_ITEMS(shadow_trait_changes)},
        {"spectre", NULL, spectre_trait_changes, TRAIT_ITEMS(spectre_trait_changes)},
        {"prism_dragon", NULL, scaled_armour_trait_changes, TRAIT_ITEMS(scaled_armour_trait_changes)},
        {"darkenbeast", NULL, darkenbeast_trait_changes, TRAIT_ITEMS(darkenbeast_trait_changes)},
        {"wyvern", NULL, scaled_armour_trait_changes, TRAIT_ITEMS(scaled_armour_trait_changes)},
        {"lizardman", NULL, lizardman_trait_changes, TRAIT_ITEMS(lizardman_trait_changes)},
        {"centaur", NULL, centaur_trait_changes, TRAIT_ITEMS(centaur_trait_changes)},
        {"manticore", NULL, manticore_trait_changes, TRAIT_ITEMS(manticore_trait_changes)},
        {"faerie_dragon", NULL, black_hatchling_trait_changes,
            TRAIT_ITEMS(black_hatchling_trait_changes)},
        {"griffin", NULL, scaled_armour_trait_changes,
            TRAIT_ITEMS(scaled_armour_trait_changes)},
        {"dragon_hatchling", NULL, black_hatchling_trait_changes,
            TRAIT_ITEMS(black_hatchling_trait_changes)},
        {"black_dragon_hatchling", NULL, black_hatchling_trait_changes,
            TRAIT_ITEMS(black_hatchling_trait_changes)},
        {"lich", NULL, lich_trait_changes, TRAIT_ITEMS(lich_trait_changes)},
        {"young_lightning_dragon", NULL, young_lightning_trait_changes,
            TRAIT_ITEMS(young_lightning_trait_changes)},
        {"animated_blade", NULL, animated_blade_trait_changes,
            TRAIT_ITEMS(animated_blade_trait_changes)},
        {"wight", NULL, wight_trait_changes, TRAIT_ITEMS(wight_trait_changes)},
        {"dracolich", NULL, dracolich_trait_changes, TRAIT_ITEMS(dracolich_trait_changes)},
        {"cattle", NULL, cattle_trait_changes, TRAIT_ITEMS(cattle_trait_changes)},
        {"hill_giant", NULL, hill_giant_trait_changes, TRAIT_ITEMS(hill_giant_trait_changes)},
        {"stone_giant", NULL, stone_giant_trait_changes, TRAIT_ITEMS(stone_giant_trait_changes)},
        {"troll", NULL, troll_trait_changes, TRAIT_ITEMS(troll_trait_changes)},
        {"troll_infant", "troll", infant_troll_trait_changes,
            TRAIT_ITEMS(infant_troll_trait_changes)},
        {"den_beast", NULL, den_beast_trait_changes, TRAIT_ITEMS(den_beast_trait_changes)},
        {"dolphin", NULL, dolphin_trait_changes, TRAIT_ITEMS(dolphin_trait_changes)},
        {"blue_whale", NULL, blue_whale_trait_changes, TRAIT_ITEMS(blue_whale_trait_changes)},
        {"hammerhead_shark", NULL, hammerhead_shark_trait_changes,
            TRAIT_ITEMS(hammerhead_shark_trait_changes)},
        {"brown_bear", NULL, brown_bear_trait_changes,
            TRAIT_ITEMS(brown_bear_trait_changes)},
        {"badger", NULL, badger_trait_changes,
            TRAIT_ITEMS(badger_trait_changes)},
        {"lynx", NULL, lynx_trait_changes,
            TRAIT_ITEMS(lynx_trait_changes)},
        {"fox", NULL, fox_trait_changes,
            TRAIT_ITEMS(fox_trait_changes)},
        {"wolf", NULL, wolf_trait_changes,
            TRAIT_ITEMS(wolf_trait_changes)},
        {"warg", "wolf", warg_trait_changes,
            TRAIT_ITEMS(warg_trait_changes)},
        {"brownie", NULL, brownie_trait_changes,
            TRAIT_ITEMS(brownie_trait_changes)},
        {"adult_green_dragon", NULL, adult_green_dragon_trait_changes,
            TRAIT_ITEMS(adult_green_dragon_trait_changes)},
        {"web_spider", NULL, web_spider_trait_changes,
            TRAIT_ITEMS(web_spider_trait_changes)},
        {"hunting_spider", NULL, hunting_spider_trait_changes,
            TRAIT_ITEMS(hunting_spider_trait_changes)},
        {"giant_spider", NULL, giant_spider_trait_changes,
            TRAIT_ITEMS(giant_spider_trait_changes)},
        {"phase_spider", NULL, phase_spider_trait_changes,
            TRAIT_ITEMS(phase_spider_trait_changes)},
        {"giant_wasp", NULL, giant_wasp_trait_changes,
            TRAIT_ITEMS(giant_wasp_trait_changes)},
        {"spider_empress", NULL, spider_empress_trait_changes,
            TRAIT_ITEMS(spider_empress_trait_changes)},
        {"quasit", NULL, quasit_trait_changes,
            TRAIT_ITEMS(quasit_trait_changes)},
        {"ki_rin", NULL, ki_rin_trait_changes,
            TRAIT_ITEMS(ki_rin_trait_changes)},
        {"wormkin", NULL, wormkin_trait_changes,
            TRAIT_ITEMS(wormkin_trait_changes)},
        {"venomous_spider", "giant_spider", NULL, 0},
        {"baaz_draconian", NULL, baaz_draconian_trait_changes,
            TRAIT_ITEMS(baaz_draconian_trait_changes)},
        {"barracuda", NULL, barracuda_trait_changes,
            TRAIT_ITEMS(barracuda_trait_changes)},
        {"dragon_turtle", NULL, dragon_turtle_trait_changes,
            TRAIT_ITEMS(dragon_turtle_trait_changes)},
        {"lair_gremlin", NULL, lair_gremlin_trait_changes,
            TRAIT_ITEMS(lair_gremlin_trait_changes)},
        {"puma", NULL, puma_trait_changes,
            TRAIT_ITEMS(puma_trait_changes)},
        {"cobra", NULL, cobra_trait_changes,
            TRAIT_ITEMS(cobra_trait_changes)},
        {"wild_boar", NULL, wild_boar_trait_changes,
            TRAIT_ITEMS(wild_boar_trait_changes)},
        {"polar_bear", NULL, polar_bear_trait_changes,
            TRAIT_ITEMS(polar_bear_trait_changes)},
        {"adult_white_dragon", NULL, adult_cold_lightning_dragon_traits,
            TRAIT_ITEMS(adult_cold_lightning_dragon_traits)},
        {"adult_blue_dragon", NULL, adult_cold_lightning_dragon_traits,
            TRAIT_ITEMS(adult_cold_lightning_dragon_traits)},
        {"frost_giant", NULL, frost_giant_trait_changes,
            TRAIT_ITEMS(frost_giant_trait_changes)},
        {"bee", NULL, bee_trait_changes,
            TRAIT_ITEMS(bee_trait_changes)},
        {"enchanted_oak", NULL, enchanted_oak_trait_changes,
            TRAIT_ITEMS(enchanted_oak_trait_changes)},
        {"leprechaun", NULL, leprechaun_trait_changes,
            TRAIT_ITEMS(leprechaun_trait_changes)},
        {"horse", NULL, horse_trait_changes,
            TRAIT_ITEMS(horse_trait_changes)},
        {"cooshee", NULL, cooshee_trait_changes,
            TRAIT_ITEMS(cooshee_trait_changes)},
        {"antelope", NULL, antelope_trait_changes,
            TRAIT_ITEMS(antelope_trait_changes)},
        {"black_bear", NULL, black_bear_trait_changes,
            TRAIT_ITEMS(black_bear_trait_changes)},
        {"hydra", NULL, hydra_trait_changes,
            TRAIT_ITEMS(hydra_trait_changes)},
        {"imp", NULL, imp_trait_changes,
            TRAIT_ITEMS(imp_trait_changes)},
        {"harpy", NULL, harpy_trait_changes,
            TRAIT_ITEMS(harpy_trait_changes)},
        {"air_elemental", NULL, air_elemental_trait_changes,
            TRAIT_ITEMS(air_elemental_trait_changes)},
        {"efreeti", NULL, efreeti_trait_changes,
            TRAIT_ITEMS(efreeti_trait_changes)},
        {"roc", NULL, roc_trait_changes,
            TRAIT_ITEMS(roc_trait_changes)},
        {"green_hag", NULL, green_hag_trait_changes,
            TRAIT_ITEMS(green_hag_trait_changes)},
        {"mosquito_swarm", NULL, mosquito_swarm_trait_changes,
            TRAIT_ITEMS(mosquito_swarm_trait_changes)},
        {"wasp_swarm", NULL, wasp_swarm_trait_changes,
            TRAIT_ITEMS(wasp_swarm_trait_changes)},
        {NULL, NULL, NULL, 0}
};

/* Player races opt into particular traits, not whole NPC packages. */
static const struct player_creature_trait
{
        int race;
        const char *archetype;
        CREATURE_TRAIT id;
} player_creature_traits[] =
{
        {RACE_DWARF, "dwarf", CREATURE_TRAIT_GOBLINOID_TRAINING},
        {RACE_DWARF, "dwarf", CREATURE_TRAIT_GIANT_DEFENCE}
};

static CREATURE_TRAIT_SET resolved_creature_traits[MAX_MOB];
static int creature_trait_build_state[MAX_MOB];
static bool creature_traits_ready;

static const CREATURE_TRAIT_PROFILE *find_trait_profile(
    const CREATURE_TRAIT_PROFILE *profiles, const char *name)
{
        for (; profiles->name; profiles++)
                if (!str_cmp(profiles->name, name))
                        return profiles;
        return NULL;
}

static const CREATURE_TRAIT_VALUE *find_trait_value(
    const CREATURE_TRAIT_SET *set, CREATURE_TRAIT id)
{
        size_t i;
        for (i = 0; i < set->count; i++)
                if (set->entries[i].id == id)
                        return &set->entries[i];
        return NULL;
}

static bool apply_trait_profile(CREATURE_TRAIT_SET *set,
    const CREATURE_TRAIT_PROFILE *profile, bool from_species)
{
        size_t i, slot;
        CREATURE_TRAIT_VALUE *grown;
        if (!profile)
                return TRUE;
        for (i = 0; i < profile->count; i++)
        {
                for (slot = 0; slot < set->count; slot++)
                        if (set->entries[slot].id == profile->changes[i].id)
                                break;
                if (slot == set->count)
                {
                        grown = realloc(set->entries,
                            (set->count + 1) * sizeof(*grown));
                        if (!grown)
                                return FALSE;
                        set->entries = grown;
                        set->count++;
                }
                set->entries[slot].id = profile->changes[i].id;
                set->entries[slot].value = profile->changes[i].value;
                set->entries[slot].source = profile->name;
                set->entries[slot].from_species = from_species;
        }
        return TRUE;
}

static bool build_creature_traits(int sn)
{
        const CREATURE_TRAIT_PROFILE *profile;
        CREATURE_TRAIT_SET *set = &resolved_creature_traits[sn];
        const CREATURE_TRAIT_SET *parent_set;
        int parent;
        char buf[MAX_STRING_LENGTH];
        if (creature_trait_build_state[sn] == 2)
                return TRUE;
        if (creature_trait_build_state[sn] == 3)
                return FALSE;
        if (creature_trait_build_state[sn] == 1)
        {
                snprintf(buf, sizeof(buf),
                    "[CREATURE TRAITS] Cyclic trait inheritance at archetype '%s'.",
                    mob_table[sn].name);
                log_string(buf);
                creature_trait_build_state[sn] = 3;
                return FALSE;
        }
        creature_trait_build_state[sn] = 1;
        profile = find_trait_profile(archetype_trait_profiles, mob_table[sn].name);
        if (profile && profile->parent)
        {
                parent = mob_lookup(profile->parent);
                if (parent < 0 || parent >= MAX_MOB
                || str_cmp(mob_table[parent].species, mob_table[sn].species))
                {
                        snprintf(buf, sizeof(buf),
                            "[CREATURE TRAITS] Archetype '%s' needs trait parent '%s' "
                            "in the same body species.", mob_table[sn].name, profile->parent);
                        log_string(buf);
                        creature_trait_build_state[sn] = 3;
                        return FALSE;
                }
                if (!build_creature_traits(parent))
                {
                        creature_trait_build_state[sn] = 3;
                        return FALSE;
                }
                parent_set = &resolved_creature_traits[parent];
                if (parent_set->count)
                {
                        set->entries = malloc(parent_set->count * sizeof(*set->entries));
                        if (!set->entries)
                        {
                                creature_trait_build_state[sn] = 3;
                                return FALSE;
                        }
                        memcpy(set->entries, parent_set->entries,
                            parent_set->count * sizeof(*set->entries));
                        set->count = parent_set->count;
                }
        }
        else if (!apply_trait_profile(set,
            find_trait_profile(species_trait_profiles, mob_table[sn].species), TRUE))
        {
                creature_trait_build_state[sn] = 3;
                return FALSE;
        }
        if (!apply_trait_profile(set, profile, FALSE))
        {
                creature_trait_build_state[sn] = 3;
                return FALSE;
        }
        creature_trait_build_state[sn] = 2;
        return TRUE;
}

static const struct
{
        const char *name;
        unsigned long int types;
} natural_damage_kinds[NATURAL_DAMAGE_COUNT] =
{
        {"none", 0},
        {"fire", RES_FIRE},
        {"cold", RES_COLD},
        {"electricity", RES_ELECTRICITY},
        {"energy", RES_ENERGY},
        {"blunt", RES_BLUNT},
        {"pierce", RES_PIERCE},
        {"slash", RES_SLASH},
        {"acid", RES_ACID},
        {"poison", RES_POISON},
        {"drain", RES_DRAIN},
        {"psychic", RES_PSYCHIC},
        {"holy", RES_HOLY},
        {"dark", RES_DARK},
        {"sonic", RES_SONIC},
        {"water", RES_WATER},
};

static int validate_trait_profiles(const CREATURE_TRAIT_PROFILE *profiles,
    bool species)
{
        const CREATURE_TRAIT_PROFILE *profile, *other;
        size_t i, j;
        int index, issues = 0;
        char buf[MAX_STRING_LENGTH];
        for (profile = profiles; profile->name; profile++)
        {
                index = species ? species_lookup(profile->name)
                                : mob_lookup(profile->name);
                if (index < 0 || (species ? index >= MAX_SPECIES : index >= MAX_MOB))
                        issues++;
                if (species && profile->parent)
                        issues++;
                if (profile->count && !profile->changes)
                {
                        issues++;
                        continue;
                }
                for (other = profiles; other < profile; other++)
                        if (!str_cmp(other->name, profile->name))
                                issues++;
                for (i = 0; i < profile->count; i++)
                {
                        if (profile->changes[i].id < 0
                        || profile->changes[i].id >= CREATURE_TRAIT_COUNT
                        || profile->changes[i].value < -100
                        || profile->changes[i].value > 100)
                                issues++;
                        if (profile->changes[i].id >= 0
                        && profile->changes[i].id < CREATURE_TRAIT_COUNT)
                        {
                                CREATURE_TRAIT_EFFECT effect =
                                    creature_trait_definitions[profile->changes[i].id].effect;
                                if ((effect == TRAIT_STATUS_RESISTANCE_PERCENT
                                    || effect == TRAIT_COMBAT_VOLLEY_PERCENT
                                    || effect == TRAIT_COMBAT_CASTING_PERCENT
                                    || effect == TRAIT_NATIVE_ROCK_PERCENT
                                    || effect == TRAIT_NATIVE_ROCK_QUANTITY
                                    || effect == TRAIT_NATIVE_HUG_DICE
                                    || effect == TRAIT_NATIVE_REGEN_QUANTITY
                                    || effect == TRAIT_NATIVE_BREATH_QUANTITY
                                    || effect == TRAIT_BLOOD_REACTION_QUANTITY
                                    || effect == TRAIT_LICH_CHILL_TOUCH
                                    || effect == TRAIT_BOW_ACCURACY
                                    || effect == TRAIT_ELEMENTAL_REACTION_QUANTITY
                                    || effect == TRAIT_PARALYSING_GAS_CHANCE
                                    || effect == TRAIT_PARALYSING_GAS_DAILY_USES
                                    || effect == TRAIT_PARALYSING_GAS_COOLDOWN
                                    || effect == TRAIT_PARALYSING_GAS_REQUIRES_HEAD
                                    || effect == TRAIT_HOLY_TRUMPET_CHANCE
                                    || effect == TRAIT_HOLY_TRUMPET_DAILY_USES
                                    || effect == TRAIT_HOLY_TRUMPET_COOLDOWN
                                    || effect == TRAIT_LULLING_DRONE_CHANCE
                                    || effect == TRAIT_LULLING_DRONE_COOLDOWN)
                                && profile->changes[i].value < 0)
                                        issues++;
                                if (effect == TRAIT_NATURAL_DAMAGE_TYPE
                                && (profile->changes[i].value < NATURAL_DAMAGE_NONE
                                    || profile->changes[i].value >= NATURAL_DAMAGE_COUNT))
                                        issues++;
                                /* Zero removes the override; 1-19 are ordinary attack verbs. */
                                if (effect == TRAIT_NATURAL_ATTACK_TYPE
                                && (profile->changes[i].value < 0
                                    || profile->changes[i].value > 19))
                                        issues++;
                                if ((effect == TRAIT_GHOUL_TOUCH_IMMUNITY
                                    || effect == TRAIT_ILLUSION_IMMUNITY
                                    || effect == TRAIT_SUNLIGHT_POWERLESS
                                    || effect == TRAIT_TURN_IMMUNITY
                                    || effect == TRAIT_HOLY_WATER_IMMUNITY
                                    || effect == TRAIT_FEAR_IMMUNITY
                                    || effect == TRAIT_STONE_DEATH
                                    || effect == TRAIT_LIGHT_RESPONSE
                                    || effect == TRAIT_MAGICAL_NATURAL_ATTACKS
                                    || effect == TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS
                                    || effect == TRAIT_PARALYSING_GAS_REQUIRES_HEAD)
                                && (profile->changes[i].value < 0
                                    || profile->changes[i].value > 1))
                                        issues++;
                        }
                        for (j = 0; j < i; j++)
                                if (profile->changes[j].id == profile->changes[i].id)
                                        issues++;
                }
        }
        if (issues)
        {
                snprintf(buf, sizeof(buf),
                    "[CREATURE TRAITS] Invalid %s profiles: %d issue(s).",
                    species ? "species" : "archetype", issues);
                log_string(buf);
        }
        return issues;
}

int validate_creature_traits(void)
{
        size_t i, j;
        int sn, issues = 0;
        const CREATURE_FAMILY *target;
        creature_traits_ready = FALSE;
        for (sn = 0; sn < MAX_MOB; sn++)
        {
                free(resolved_creature_traits[sn].entries);
                memset(&resolved_creature_traits[sn], 0,
                    sizeof(resolved_creature_traits[sn]));
                creature_trait_build_state[sn] = 0;
        }
        if (TRAIT_ITEMS(creature_trait_definitions) != CREATURE_TRAIT_COUNT)
                issues++;
        for (i = 0; i < TRAIT_ITEMS(creature_trait_definitions); i++)
        {
                if (!creature_trait_definitions[i].name
                || !creature_trait_definitions[i].name[0])
                {
                        issues++;
                        continue;
                }
                if (creature_trait_definitions[i].effect < TRAIT_ATTACKER_MELEE
                || creature_trait_definitions[i].effect >= TRAIT_EFFECT_COUNT)
                        issues++;
                for (j = 0; j < i; j++)
                        if (creature_trait_definitions[j].name
                        && !str_cmp(creature_trait_definitions[i].name,
                                     creature_trait_definitions[j].name))
                                issues++;
                target = creature_trait_definitions[i].targets;
                if (!target && (creature_trait_definitions[i].effect == TRAIT_ATTACKER_MELEE
                            || creature_trait_definitions[i].effect == TRAIT_DEFENDER_MELEE))
                        issues++;
                if (target)
                        for (; *target != CREATURE_FAMILY_NONE; target++)
                                if (*target < CREATURE_FAMILY_NONE || *target >= CREATURE_FAMILY_MAX)
                                        issues++;
        }
        issues += validate_trait_profiles(species_trait_profiles, TRUE);
        issues += validate_trait_profiles(archetype_trait_profiles, FALSE);
        for (i = 0; i < TRAIT_ITEMS(player_creature_traits); i++)
        {
                sn = mob_lookup(player_creature_traits[i].archetype);
                if (sn < 0 || sn >= MAX_MOB
                || player_creature_traits[i].race < 0
                || player_creature_traits[i].race >= MAX_RACE
                || player_creature_traits[i].id < 0
                || player_creature_traits[i].id >= CREATURE_TRAIT_COUNT)
                        issues++;
                for (j = 0; j < i; j++)
                        if (player_creature_traits[i].race == player_creature_traits[j].race
                        && player_creature_traits[i].id == player_creature_traits[j].id)
                                issues++;
        }
        if (!issues)
                for (sn = 0; sn < MAX_MOB; sn++)
                        if (!build_creature_traits(sn))
                                issues++;
        if (issues)
                bug("[CREATURE TRAITS] Validation failed: %d issue(s).", issues);
        else
        {
                creature_traits_ready = TRUE;
                log_string("[CREATURE TRAITS] Validation complete: no issues found.");
        }
        return issues;
}

static const CREATURE_TRAIT_VALUE *creature_trait_entry(CHAR_DATA *ch,
    CREATURE_TRAIT id)
{
        size_t i;
        int sn = -1;
        if (!ch || id < 0 || id >= CREATURE_TRAIT_COUNT)
                return NULL;
        if (!creature_traits_ready && validate_creature_traits() != 0)
                return NULL;
        if (IS_NPC(ch))
        {
                if (ch->mobspec && ch->mobspec[0])
                        sn = mob_lookup(ch->mobspec);
        }
        else
        {
                for (i = 0; i < TRAIT_ITEMS(player_creature_traits); i++)
                        if (ch->race == player_creature_traits[i].race
                        && id == player_creature_traits[i].id)
                        {
                                sn = mob_lookup(player_creature_traits[i].archetype);
                                break;
                        }
        }
        if (sn < 0 || sn >= MAX_MOB)
                return NULL;
        return find_trait_value(&resolved_creature_traits[sn], id);
}

int creature_trait_value(CHAR_DATA *ch, CREATURE_TRAIT id)
{
        const CREATURE_TRAIT_VALUE *entry = creature_trait_entry(ch, id);
        return entry ? entry->value : 0;
}

/* A mixed sleep/charm effect gets one roll at the stronger percentage. */
int creature_status_resistance_percent(CHAR_DATA *ch, unsigned long int res_types)
{
        int chance = 0;
        if (res_types & RES_SLEEP)
                chance = creature_trait_value(ch, CREATURE_TRAIT_SLEEP_RESISTANCE_PERCENT);
        if (res_types & RES_CHARM)
                chance = UMAX(chance,
                    creature_trait_value(ch, CREATURE_TRAIT_CHARM_RESISTANCE_PERCENT));
        return URANGE(0, chance, 100);
}

bool creature_resists_status(CHAR_DATA *ch, unsigned long int res_types)
{
        int chance = creature_status_resistance_percent(ch, res_types);
        return chance > 0 && number_percent() <= chance;
}

static bool trait_targets_family(const CREATURE_TRAIT_DEFINITION *trait,
    CREATURE_FAMILY family)
{
        const CREATURE_FAMILY *target;
        if (family == CREATURE_FAMILY_NONE || !trait->targets)
                return FALSE;
        for (target = trait->targets; *target != CREATURE_FAMILY_NONE; target++)
                if (*target == family)
                        return TRUE;
        return FALSE;
}

int creature_melee_hit_bonus(CHAR_DATA *attacker, CHAR_DATA *victim)
{
        int id, bonus = 0;
        OBJ_DATA *weapon;
        CREATURE_FAMILY attacker_family, victim_family;
        if (!attacker || !victim)
                return 0;
        /* The spell already supplies displacement while its condition is present. */
        if (!is_affected(victim, gsn_displacement)
        && (!victim->in_room || !victim->in_room->area
            || !IS_SET(victim->in_room->area->area_flags, AREA_FLAG_NO_MAGIC)))
                bonus += creature_trait_value(victim,
                    CREATURE_TRAIT_DISPLACEMENT_MELEE_PENALTY);
        bonus += creature_trait_value(attacker, CREATURE_TRAIT_MELEE_ACCURACY);
        weapon = get_attack_weapon(attacker, WEAR_WIELD);
        if (weapon && weapon->item_type == ITEM_WEAPON)
                bonus += creature_trait_value(attacker, CREATURE_TRAIT_ARMED_MELEE_ACCURACY);
        attacker_family = creature_family(attacker);
        victim_family = creature_family(victim);
        for (id = 0; id < CREATURE_TRAIT_COUNT; id++)
        {
                if (creature_trait_definitions[id].effect == TRAIT_ATTACKER_MELEE
                && trait_targets_family(&creature_trait_definitions[id], victim_family))
                        bonus += creature_trait_value(attacker, id);
                if (creature_trait_definitions[id].effect == TRAIT_DEFENDER_MELEE
                && trait_targets_family(&creature_trait_definitions[id], attacker_family))
                        bonus += creature_trait_value(victim, id);
        }
        return bonus + creature_light_melee_bonus(attacker);
}

/* This bonus follows the actual melee weapon used for this hit. */
int creature_armed_melee_damage_bonus(CHAR_DATA *ch, OBJ_DATA *weapon, int dt)
{
        if (!ch || !weapon || weapon->deleted || weapon->item_type != ITEM_WEAPON
        || weapon->carried_by != ch
        || (weapon->wear_loc != WEAR_WIELD && weapon->wear_loc != WEAR_DUAL)
        || dt < TYPE_HIT || IS_OBJ_STAT(weapon, ITEM_BOW | ITEM_BODY_PART)
        || !bodypart_weapon_usable(ch, weapon))
                return 0;
        return URANGE(-100,
            creature_trait_value(ch, CREATURE_TRAIT_ARMED_MELEE_DAMAGE), 100);
}

/* These optional reactions belong to the creature, not to every immune target. */
void creature_elemental_reaction(CHAR_DATA *victim, int contact_damage,
                                unsigned long int types)
{
        int percent, heal, limit, ticks;
        AFFECT_DATA af;

        if (!victim || !IS_NPC(victim) || victim->deleted || !victim->in_room
        || victim->hit <= 0 || victim->position == POS_DEAD || contact_damage <= 0
        || victim->ghost_phase != GHOST_PHASE_NONE
        || IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                return;

        percent = creature_trait_value(victim, CREATURE_TRAIT_ELECTRICITY_HEALING_PERCENT);
        if ((types & RES_ELECTRICITY) && percent > 0
        && !IS_SET(victim->act, ACT_NO_HEAL))
        {
                limit = UMIN(effective_max_hit(victim), aggravated_healing_limit(victim));
                if (victim->hit < limit)
                {
                        heal = (int)UMIN((long long)limit - victim->hit,
                            UMAX(1LL, (long long)contact_damage * URANGE(0, percent, 100) / 100));
                        victim->hit += heal;
                        update_pos(victim);
                        act("Electricity surges through $n, restoring $s vitality.", victim, NULL, NULL, TO_ROOM);
                }
        }

        ticks = creature_trait_value(victim, CREATURE_TRAIT_FIRE_COLD_SLOW_TICKS);
        if (!(types & (RES_FIRE | RES_COLD)) || ticks <= 0
        || gsn_slow < 0 || gsn_slow >= MAX_SKILL
        || IS_AFFECTED(victim, AFF_SLOW) || is_affected(victim, gsn_slow))
                return;

        /* As with ordinary Slow, an existing speed spell is cancelled first. */
        if (is_affected(victim, gsn_haste))
        {
                affect_strip(victim, gsn_haste);
                return;
        }
        if (is_affected(victim, gsn_quicken))
        {
                affect_strip(victim, gsn_quicken);
                return;
        }
        memset(&af, 0, sizeof(af));
        af.type = gsn_slow;
        af.duration = ticks - 1;
        af.location = APPLY_NONE;
        af.bitvector = AFF_SLOW;
        affect_to_char(victim, &af);
        act("$n's stitched limbs stiffen and slow down.", victim, NULL, NULL, TO_ROOM);
}

bool creature_is_sunlight_sensitive(CHAR_DATA *ch)
{
        return creature_trait_value(ch, CREATURE_TRAIT_SUNLIGHT_SENSITIVE) != 0;
}

/* Both sunlight traits use the same natural light and shelter rules. */
static bool creature_in_direct_sunlight(CHAR_DATA *ch)
{
        ROOM_INDEX_DATA *room;
        if (!ch || !ch->in_room)
                return FALSE;
        room = ch->in_room;
        return IS_OUTSIDE(ch)
            && room->sector_type != SECT_UNDERWATER
            && room->sector_type != SECT_UNDERWATER_GROUND
            && !IS_SET(room->room_flags, ROOM_DARK | ROOM_NO_WEATHER)
            && weather_info.sunlight == SUN_LIGHT
            && weather_info.sky == SKY_CLOUDLESS;
}

bool creature_sunlight_powerless(CHAR_DATA *ch)
{
        return creature_trait_value(ch, CREATURE_TRAIT_SUNLIGHT_POWERLESS) > 0
            && creature_in_direct_sunlight(ch);
}

/* An active spell orb is bright whether it rests here or is worn as a light. */
static bool active_continual_light(OBJ_DATA *obj)
{
        return obj && !obj->deleted && obj->pIndexData
            && obj->item_type == ITEM_LIGHT && obj->value[2] != 0
            && (obj->pIndexData->vnum == OBJ_VNUM_LIGHT_BALL
                || obj->pIndexData->vnum == OBJ_VNUM_LIGHT_BALL_CRAFT);
}

CREATURE_LIGHT_STATE creature_light_state(CHAR_DATA *ch)
{
        OBJ_DATA *obj;
        CHAR_DATA *other;
        ROOM_INDEX_DATA *room;

        if (!ch || ch->deleted || !(room = ch->in_room))
                return CREATURE_LIGHT_ORDINARY;
        if (creature_in_direct_sunlight(ch))
                return CREATURE_LIGHT_BRIGHT;
        for (obj = room->contents; obj; obj = obj->next_content)
                if (active_continual_light(obj))
                        return CREATURE_LIGHT_BRIGHT;
        for (other = room->people; other; other = other->next_in_room)
                if (!other->deleted
                && active_continual_light(get_eq_char(other, WEAR_LIGHT)))
                        return CREATURE_LIGHT_BRIGHT;
        return room_is_dark(room) ? CREATURE_LIGHT_DARK : CREATURE_LIGHT_ORDINARY;
}

/* A trait opts an NPC in; there is no creature-name or vnum exception. */
static bool creature_has_light_response(CHAR_DATA *ch)
{
        return ch && !ch->deleted && IS_NPC(ch)
            && creature_trait_value(ch, CREATURE_TRAIT_SHADOW_LIGHT_RESPONSE) > 0;
}

int creature_light_melee_bonus(CHAR_DATA *ch)
{
        if (!creature_has_light_response(ch))
                return 0;
        switch (creature_light_state(ch))
        {
        case CREATURE_LIGHT_DARK: return 2;
        case CREATURE_LIGHT_ORDINARY: return 1;
        default: return 0;
        }
}

int creature_light_damage(CHAR_DATA *victim, int damage)
{
        if (damage <= 0 || !creature_has_light_response(victim))
                return damage;
        switch (creature_light_state(victim))
        {
        case CREATURE_LIGHT_DARK: return damage / 2;
        case CREATURE_LIGHT_BRIGHT: return (int)UMIN((long long)INT_MAX, 2LL * damage);
        default: return damage;
        }
}

/* Sunlight exposes a phased spirit without changing an explicit NONE. */
void update_creature_sunlight(CHAR_DATA *ch)
{
        if (creature_sunlight_powerless(ch)
        && ch->ghost_phase == GHOST_PHASE_ETHEREAL
        && set_ghost_phase(ch, GHOST_PHASE_SEMI_MATERIAL))
                act("{dSunlight draws $n back into a half-material form.{x",
                    ch, NULL, NULL, TO_ROOM);
}

/* The existing melee callers still decide whether this penalty applies. */
int creature_sunlight_hit_penalty(CHAR_DATA *ch)
{
        int id, penalty = 0;
        if (!creature_in_direct_sunlight(ch))
                return 0;
        for (id = 0; id < CREATURE_TRAIT_COUNT; id++)
                if (creature_trait_definitions[id].effect == TRAIT_SUNLIGHT_ACCURACY)
                        penalty -= creature_trait_value(ch, id);
        return penalty;
}

void show_creature_traits(CHAR_DATA *viewer, CHAR_DATA *victim)
{
        int id;
        bool any = FALSE;
        const CREATURE_TRAIT_VALUE *entry;
        char buf[MAX_STRING_LENGTH];
        send_to_char("Creature traits (species/archetype definitions):\n\r", viewer);
        for (id = 0; id < CREATURE_TRAIT_COUNT; id++)
        {
                entry = creature_trait_entry(victim, id);
                if (!entry)
                        continue;
                any = TRUE;
                if (creature_trait_definitions[id].effect == TRAIT_NATURAL_DAMAGE_TYPE)
                        snprintf(buf, sizeof(buf), "  %s: %s%s; %s %s%s.\n\r",
                            creature_trait_definitions[id].name,
                            entry->value >= NATURAL_DAMAGE_NONE
                                && entry->value < NATURAL_DAMAGE_COUNT
                                ? natural_damage_kinds[entry->value].name : "invalid",
                            entry->value == 0 ? " (disabled)" : "",
                            entry->from_species ? "species" : "archetype", entry->source,
                            IS_NPC(victim) ? "" : "; approved player racial trait");
                else if (creature_trait_definitions[id].effect == TRAIT_STATUS_RESISTANCE_PERCENT
                ||  creature_trait_definitions[id].effect == TRAIT_COMBAT_VOLLEY_PERCENT
                ||  creature_trait_definitions[id].effect == TRAIT_COMBAT_CASTING_PERCENT
                ||  creature_trait_definitions[id].effect == TRAIT_NATIVE_ROCK_PERCENT)
                        snprintf(buf, sizeof(buf), "  %s: %d%%%s; %s %s%s.\n\r",
                            creature_trait_definitions[id].name, entry->value,
                            entry->value == 0 ? " (disabled)" : "",
                            entry->from_species ? "species" : "archetype", entry->source,
                            IS_NPC(victim) ? "" : "; approved player racial trait");
                else
                        snprintf(buf, sizeof(buf), "  %s: %+d%s; %s %s%s.\n\r",
                            creature_trait_definitions[id].name, entry->value,
                            entry->value == 0 ? " (disabled)" : "",
                            entry->from_species ? "species" : "archetype", entry->source,
                            IS_NPC(victim) ? "" : "; approved player racial trait");
                send_to_char(buf, viewer);
        }
        if (creature_has_light_response(victim))
        {
                CREATURE_LIGHT_STATE light = creature_light_state(victim);
                snprintf(buf, sizeof(buf),
                    "  Current light response: %s; melee +%d, armour %+d, incoming damage %d%%.\n\r",
                    light == CREATURE_LIGHT_DARK ? "darkness"
                        : light == CREATURE_LIGHT_BRIGHT ? "bright light" : "ordinary light",
                    creature_light_melee_bonus(victim),
                    light == CREATURE_LIGHT_DARK ? -8 : light == CREATURE_LIGHT_ORDINARY ? -4 : 0,
                    light == CREATURE_LIGHT_DARK ? 50 : light == CREATURE_LIGHT_BRIGHT ? 200 : 100);
                send_to_char(buf, viewer);
        }
        if (!any)
                send_to_char("  None.\n\r", viewer);
}

/* Bow training improves a real shot, without changing general hitroll. */
int creature_bow_hit_bonus(CHAR_DATA *ch, OBJ_DATA *weapon, int dt)
{
        if (!ch || ch->deleted || !IS_NPC(ch)
        ||  !weapon || weapon->deleted || weapon->item_type != ITEM_WEAPON
        ||  !IS_OBJ_STAT(weapon, ITEM_BOW)
        ||  (dt != gsn_shoot && dt != gsn_snap_shot))
                return 0;

        return UMAX(0, creature_trait_value(ch, CREATURE_TRAIT_BOW_ACCURACY));
}

/* An ordinary unarmed NPC attack can have an elemental damage kind. */
unsigned long int creature_natural_damage_type(CHAR_DATA *ch)
{
        int kind;

        if (!ch || ch->deleted || !IS_NPC(ch))
                return 0;
        kind = creature_trait_value(ch, CREATURE_TRAIT_NATURAL_DAMAGE_TYPE);
        return kind > NATURAL_DAMAGE_NONE && kind < NATURAL_DAMAGE_COUNT
            ? natural_damage_kinds[kind].types : 0;
}

int creature_natural_armour(CHAR_DATA *ch)
{
        int armour = creature_trait_value(ch, CREATURE_TRAIT_NATURAL_ARMOUR);
        if (creature_has_light_response(ch))
        {
                switch (creature_light_state(ch))
                {
                case CREATURE_LIGHT_DARK: armour -= 8; break;
                case CREATURE_LIGHT_ORDINARY: armour -= 4; break;
                default: break;
                }
        }
        return armour;
}

/* Sonar locates a nearby physical creature; it does not provide distant sight. */
bool creature_echo_locates(CHAR_DATA *ch, CHAR_DATA *victim)
{
        if (!ch || !victim || ch->deleted || victim->deleted
        || !ch->in_room || ch->in_room != victim->in_room || !IS_AWAKE(ch)
        || !HAS_HEAD(ch) || !(ch->body_form & PART_EAR)
        || creature_trait_value(ch, CREATURE_TRAIT_ECHOLOCATION) <= 0)
                return FALSE;
        if ((IS_NPC(victim) && IS_SET(victim->act, ACT_WIZINVIS_MOB))
        || (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS)
            && get_trust(ch) < get_trust(victim)))
                return FALSE;
        if (is_noncorporeal_player(victim)
        || IS_AFFECTED(victim, AFF_NON_CORPOREAL)
        || (IS_NPC(victim) && victim->ghost_phase != GHOST_PHASE_NONE))
                return FALSE;
        /* The existing deaf/channel and silence settings are communication controls. */
        return TRUE;
}

bool clear_strength_drain(CHAR_DATA *ch)
{
        if (!ch || gsn_strength_drain < 0 || gsn_strength_drain >= MAX_SKILL
        || !is_affected(ch, gsn_strength_drain))
                return FALSE;
        affect_strip(ch, gsn_strength_drain);
        send_to_char("Your stolen strength returns.\n\r", ch);
        return TRUE;
}

/* Called only after a weaponless NPC has actually removed HP by melee contact. */
void strength_drain_after_hit(CHAR_DATA *ch, CHAR_DATA *victim,
    int dt, int actual_damage)
{
        AFFECT_DATA af, *paf;
        int strength, penalty = 0;
        if (!ch || !victim || ch == victim || !IS_NPC(ch)
        || ch->deleted || victim->deleted || !ch->in_room
        || ch->in_room != victim->in_room || ch->hit <= 0 || victim->hit <= 0
        || ch->position == POS_DEAD || victim->position == POS_DEAD
        || actual_damage <= 0 || dt < TYPE_HIT
        || gsn_strength_drain < 0 || gsn_strength_drain >= MAX_SKILL
        || IS_UNDEAD(victim) || IS_INORGANIC(victim)
        || is_noncorporeal_player(victim)
        || (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT)))
                return;
        strength = creature_trait_value(ch, CREATURE_TRAIT_STRENGTH_DRAIN_TOUCH);
        if (strength <= 0 || number_percent() > 25
        || saves_resistance_effect(ch, ch->level, victim, RES_DRAIN))
                return;
        for (paf = victim->affected; paf; paf = paf->next)
                if (!paf->deleted && paf->type == gsn_strength_drain
                && paf->location == APPLY_STR)
                        penalty = UMAX(penalty, -paf->modifier);
        penalty = UMIN(3, penalty + strength);
        affect_strip(victim, gsn_strength_drain);
        memset(&af, 0, sizeof(af));
        af.type = gsn_strength_drain;
        af.duration = 4;
        af.location = APPLY_STR;
        af.modifier = -penalty;
        affect_to_char(victim, &af);
        act("$n's touch steals some of your strength!", ch, NULL, victim, TO_VICT);
        act("Your touch steals some of $N's strength.", ch, NULL, victim, TO_CHAR);
}

#undef TRAIT_ITEMS


/*
 * Names for the live ghost-phase state. Unknown values remain visible
 * in diagnostics rather than being mistaken for an ordinary default.
 */
const char *ghost_phase_name(GHOST_PHASE phase)
{
        switch (phase)
        {
        case GHOST_PHASE_NONE:
                return "none";
        case GHOST_PHASE_ETHEREAL:
                return "ethereal";
        case GHOST_PHASE_SEMI_MATERIAL:
                return "semi_material";
        default:
                return "invalid";
        }
}

/* Parse one complete named value; leave the output unchanged on failure. */
bool parse_ghost_phase(const char *text, GHOST_PHASE *phase)
{
        GHOST_PHASE parsed;

        if (!text || !phase)
                return FALSE;

        if (!str_cmp(text, "none"))
                parsed = GHOST_PHASE_NONE;
        else if (!str_cmp(text, "ethereal"))
                parsed = GHOST_PHASE_ETHEREAL;
        else if (!str_cmp(text, "semi_material"))
                parsed = GHOST_PHASE_SEMI_MATERIAL;
        else
                return FALSE;

        *phase = parsed;
        return TRUE;
}

/*
 * One validated live-state change. Supported contacts read the live value.
 * This setter does not rewrite flags, resistance masks or prototype data.
 */
bool set_ghost_phase(CHAR_DATA *ch, GHOST_PHASE phase)
{
        if (!ch || ch->deleted || !IS_NPC(ch)
        ||  ch->position == POS_DEAD)
                return FALSE;

        switch (phase)
        {
        case GHOST_PHASE_NONE:
        case GHOST_PHASE_ETHEREAL:
        case GHOST_PHASE_SEMI_MATERIAL:
                break;
        default:
                return FALSE;
        }

        ch->ghost_phase = phase;
        return TRUE;
}

/* Name a raw inherited setting without confusing inherit with none. */
const char *mob_ghost_phase_setting_name(int phase)
{
        if (phase == MOB_TEMPLATE_UNSET)
                return "inherit";

        return ghost_phase_name((GHOST_PHASE)phase);
}

/* Area/template settings may inherit; live mset values still may not. */
bool parse_mob_ghost_phase(const char *text, int *phase)
{
        GHOST_PHASE parsed;

        if (!text || !phase)
                return FALSE;

        if (!str_cmp(text, "inherit"))
        {
                *phase = MOB_TEMPLATE_UNSET;
                return TRUE;
        }

        if (!parse_ghost_phase(text, &parsed))
                return FALSE;

        *phase = (int)parsed;
        return TRUE;
}

/* Validate both raw table layers, including overridden values. */
int validate_mob_ghost_phases(void)
{
        char buf[MAX_STRING_LENGTH];
        const char *kind;
        const char *name;
        int i;
        int value;
        int issues = 0;

        for (i = 0; i < MAX_SPECIES + MAX_MOB; i++)
        {
                if (i < MAX_SPECIES)
                {
                        kind = "Body species";
                        name = species_table[i].species;
                        value = species_table[i].initial_ghost_phase;
                }
                else
                {
                        kind = "Creature archetype";
                        name = mob_table[i - MAX_SPECIES].name;
                        value = mob_table[i - MAX_SPECIES].initial_ghost_phase;
                }

                if (value == MOB_TEMPLATE_UNSET
                ||  value == GHOST_PHASE_NONE
                ||  value == GHOST_PHASE_ETHEREAL
                ||  value == GHOST_PHASE_SEMI_MATERIAL)
                        continue;

                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] %s '%s': invalid initial ghost phase %d. "
                    "Use MOB_TEMPLATE_UNSET or a defined GHOST_PHASE value.",
                    kind, name ? name : "unnamed", value);
                log_string(buf);
                issues++;
        }

        if (issues == 0)
                log_string("[MOB TEMPLATE] Initial ghost phases: no issues found.");

        return issues;
}

/*
 * Return the body-species table index for a species name.
 */
int species_lookup(const char *name)
{
        int sn;

        if (!name || name[0] == '\0')
                return -1;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                if (!species_table[sn].species)
                        continue;

                if (!str_cmp(name, species_table[sn].species))
                        return sn;
        }

        return -1;
}

/*
 * Resolve one scalar through the body-species and archetype layers.
 */
static int resolve_template_scalar(int species_value,
                                   int archetype_value)
{
        if (archetype_value != MOB_TEMPLATE_UNSET)
                return archetype_value;

        if (species_value != MOB_TEMPLATE_UNSET)
                return species_value;

        return 0;
}

/*
 * Resolve one default special-function name.
 *
 * NULL means inherit.
 * An empty string means explicitly clear the inherited value.
 * A non-empty string replaces the inherited value.
 */
static const char *resolve_template_special(
    const char *species_special,
    const char *archetype_special)
{
        if (archetype_special)
        {
                if (archetype_special[0] == '\0')
                        return NULL;

                return archetype_special;
        }

        if (species_special && species_special[0] != '\0')
                return species_special;

        return NULL;
}

/*
 * Resolve body species followed by creature archetype.
 *
 * Bitfields use XOR so that an archetype can both add defaults and cancel
 * inappropriate body-species defaults. Scalar values use explicit inheritance.
 */
bool resolve_mob_template(int mob_type,
                          MOB_TEMPLATE_DATA *resolved)
{
        const struct mob_type *archetype;
        const struct species_type *body_species;
        int species;
        int special_slot;

        if (!resolved)
                return FALSE;

        memset(resolved, 0, sizeof(*resolved));

        if (mob_type < 0 || mob_type >= MAX_MOB)
                return FALSE;

        archetype = &mob_table[mob_type];

        if (!archetype->name
        ||  archetype->name[0] == '\0'
        ||  !archetype->species
        ||  archetype->species[0] == '\0')
        {
                return FALSE;
        }

        species = species_lookup(archetype->species);

        if (species < 0 || species >= MAX_SPECIES)
                return FALSE;

        body_species = &species_table[species];

        resolved->act =
            body_species->act ^ archetype->act;

        resolved->affected_by =
            body_species->affected_by ^ archetype->affected_by;

        resolved->body_form =
            body_species->body_form ^ archetype->body_form;

        resolved->attack_parts =
            body_species->attack_parts ^ archetype->attack_parts;

        resolved->resists =
            body_species->resists ^ archetype->resists;

        resolved->vulnerabilities =
            body_species->vulnerabilities
            ^ archetype->vulnerabilities;

        resolved->immunes =
            body_species->immunes ^ archetype->immunes;

        resolved->hp_mod =
            resolve_template_scalar(
                body_species->hp_mod,
                archetype->hp_mod);

        resolved->dam_mod =
            resolve_template_scalar(
                body_species->dam_mod,
                archetype->dam_mod);

        resolved->crit_mod =
            resolve_template_scalar(
                body_species->crit_mod,
                archetype->crit_mod);

        resolved->haste_mod =
            resolve_template_scalar(
                body_species->haste_mod,
                archetype->haste_mod);

        resolved->height =
            resolve_template_scalar(
                body_species->height,
                archetype->height);

        resolved->length =
            resolve_template_scalar(
                body_species->length,
                archetype->length);

        resolved->weight =
            resolve_template_scalar(
                body_species->weight,
                archetype->weight);

        resolved->size =
            resolve_template_scalar(
                body_species->size,
                archetype->size);

        resolved->language =
            resolve_template_scalar(
                body_species->language,
                archetype->language);

        resolved->initial_ghost_phase =
            resolve_template_scalar(
                body_species->initial_ghost_phase,
                archetype->initial_ghost_phase);

        resolved->spec_fun1 =
            resolve_template_special(
                body_species->spec_fun1,
                archetype->spec_fun1);

        resolved->spec_fun2 =
            resolve_template_special(
                body_species->spec_fun2,
                archetype->spec_fun2);

        resolved->spec_boss =
            resolve_template_special(
                body_species->spec_boss,
                archetype->spec_boss);

        /*
         * XP adjustment belongs to the creature archetype.
         * It is a signed scalar, not an XOR mask.
         */
        resolved->xp_mod = archetype->xp_mod;

        /*
         * Probability policies inherit as one vector.
         * Never inherit individual percentage components separately.
         */
        for (special_slot = 0;
             special_slot < MOB_SPECIAL_SLOTS;
             special_slot++)
        {
                if (archetype->spec_chance[0] != MOB_TEMPLATE_UNSET)
                {
                        resolved->spec_chance[special_slot] =
                            archetype->spec_chance[special_slot];
                }
                else if (body_species->spec_chance[0] != MOB_TEMPLATE_UNSET)
                {
                        resolved->spec_chance[special_slot] =
                            body_species->spec_chance[special_slot];
                }
                else
                {
                        resolved->spec_chance[special_slot] =
                            MOB_SPECIAL_AUTO;
                }
        }

        return TRUE;
}

/*
 * Parse a signed scalar or inherit. Output is unchanged on failure.
 * The caller supplies the lower bound for its particular modifier.
 */
static bool parse_mob_percent_modifier(const char *text,
                                       int minimum,
                                       int *modifier)
{
        const char *p;
        char *end;
        long value;

        if (!text || !modifier || text[0] == '\0')
                return FALSE;

        if (!str_cmp(text, "inherit"))
        {
                *modifier = MOB_TEMPLATE_UNSET;
                return TRUE;
        }

        p = text;
        if (*p == '+' || *p == '-')
                p++;

        if (*p < '0' || *p > '9')
                return FALSE;

        while (*p >= '0' && *p <= '9')
                p++;

        if (*p != '\0')
                return FALSE;

        errno = 0;
        value = strtol(text, &end, 10);

        if (errno == ERANGE || end == text || *end != '\0'
        ||  value < minimum || value > INT_MAX)
        {
                return FALSE;
        }

        *modifier = (int)value;
        return TRUE;
}

/*
 * Preserve the existing HP parser's interface and accepted values.
 */
bool parse_mob_hp_modifier(const char *text, int *modifier)
{
        return parse_mob_percent_modifier(
            text, MOB_HP_MOD_MIN, modifier);
}

/*
 * Parse an individual attack-damage adjustment.
 */
bool parse_mob_damage_modifier(const char *text, int *modifier)
{
        return parse_mob_percent_modifier(
            text, MOB_DAMAGE_MOD_MIN, modifier);
}

/*
 * Parse a critical/swiftness score adjustment, preserving explicit zero.
 * The output remains unchanged on failure.
 */
bool parse_mob_combat_modifier(const char *text, int *modifier)
{
        int value;

        if (!modifier)
                return FALSE;

        if (!parse_mob_percent_modifier(text, MOB_COMBAT_MOD_MIN, &value))
                return FALSE;

        if (value != MOB_TEMPLATE_UNSET && value > MOB_COMBAT_MOD_MAX)
                return FALSE;

        *modifier = value;
        return TRUE;
}

/*
 * Check one raw table value. An unset value means inherit.
 */
static int validate_mob_combat_modifier_value(
    const char *owner,
    const char *name,
    const char *field,
    int value)
{
        char buf[MAX_STRING_LENGTH];

        if (value == MOB_TEMPLATE_UNSET
        ||  (value >= MOB_COMBAT_MOD_MIN && value <= MOB_COMBAT_MOD_MAX))
        {
                return 0;
        }

        snprintf(
            buf, sizeof(buf),
            "[MOB TEMPLATE] %s '%s' has invalid %s %d; "
            "use MOB_TEMPLATE_UNSET or a value from %d to %d.",
            owner, name ? name : "unnamed", field, value,
            MOB_COMBAT_MOD_MIN, MOB_COMBAT_MOD_MAX);
        log_string(buf);
        return 1;
}

/*
 * Validate both raw layers, including values hidden by later overrides.
 */
int validate_mob_combat_modifiers(void)
{
        char buf[MAX_STRING_LENGTH];
        int sn;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                issues += validate_mob_combat_modifier_value(
                    "Body species", species_table[sn].species,
                    "crit_mod", species_table[sn].crit_mod);

                issues += validate_mob_combat_modifier_value(
                    "Body species", species_table[sn].species,
                    "haste_mod", species_table[sn].haste_mod);
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                issues += validate_mob_combat_modifier_value(
                    "Creature archetype", mob_table[sn].name,
                    "crit_mod", mob_table[sn].crit_mod);

                issues += validate_mob_combat_modifier_value(
                    "Creature archetype", mob_table[sn].name,
                    "haste_mod", mob_table[sn].haste_mod);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Combat-score validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Combat-score validation complete: "
                    "%d issue%s found.",
                    issues, issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Parse one non-negative dimension value or inherit.
 * This reuses integer parsing only; no percentage arithmetic is involved.
 */
bool parse_mob_dimension(const char *text, int *value)
{
        return parse_mob_percent_modifier(text, 0, value);
}

/* Validate a raw template value even when a later layer overrides it. */
static int validate_mob_dimension_value(
    const char *owner,
    const char *name,
    const char *field,
    int value)
{
        char buf[MAX_STRING_LENGTH];

        if (value == MOB_TEMPLATE_UNSET || value >= 0)
                return 0;

        snprintf(
            buf, sizeof(buf),
            "[MOB TEMPLATE] %s '%s' has invalid %s %d; "
            "use MOB_TEMPLATE_UNSET or a value from 0 to %d.",
            owner, name ? name : "unnamed", field, value, INT_MAX);
        log_string(buf);
        return 1;
}

int validate_mob_dimensions(void)
{
        char buf[MAX_STRING_LENGTH];
        int sn;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                issues += validate_mob_dimension_value(
                    "Body species", species_table[sn].species,
                    "height", species_table[sn].height);
                issues += validate_mob_dimension_value(
                    "Body species", species_table[sn].species,
                    "length", species_table[sn].length);
                issues += validate_mob_dimension_value(
                    "Body species", species_table[sn].species,
                    "weight", species_table[sn].weight);
                issues += validate_mob_dimension_value(
                    "Body species", species_table[sn].species,
                    "size", species_table[sn].size);
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                issues += validate_mob_dimension_value(
                    "Creature archetype", mob_table[sn].name,
                    "height", mob_table[sn].height);
                issues += validate_mob_dimension_value(
                    "Creature archetype", mob_table[sn].name,
                    "length", mob_table[sn].length);
                issues += validate_mob_dimension_value(
                    "Creature archetype", mob_table[sn].name,
                    "weight", mob_table[sn].weight);
                issues += validate_mob_dimension_value(
                    "Creature archetype", mob_table[sn].name,
                    "size", mob_table[sn].size);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Dimension validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Dimension validation complete: "
                    "%d issue%s found.",
                    issues, issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Parse one raw language code or inherit.
 * Reuse integer syntax only; this is not a percentage adjustment.
 */
bool parse_mob_language(const char *text, int *value)
{
        return parse_mob_percent_modifier(text, 0, value);
}

/* Validate stored codes, not membership of a language-name dictionary. */
static int validate_mob_language_value(
    const char *owner,
    const char *name,
    int value)
{
        char buf[MAX_STRING_LENGTH];

        if (value == MOB_TEMPLATE_UNSET || value >= 0)
                return 0;

        snprintf(
            buf, sizeof(buf),
            "[MOB TEMPLATE] %s '%s' has invalid language code %d; "
            "use MOB_TEMPLATE_UNSET or a value from 0 to %d.",
            owner, name ? name : "unnamed", value, INT_MAX);
        log_string(buf);
        return 1;
}

int validate_mob_languages(void)
{
        char buf[MAX_STRING_LENGTH];
        int sn;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                issues += validate_mob_language_value(
                    "Body species", species_table[sn].species,
                    species_table[sn].language);
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                issues += validate_mob_language_value(
                    "Creature archetype", mob_table[sn].name,
                    mob_table[sn].language);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Language-code validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Language-code validation complete: "
                    "%d issue%s found.",
                    issues, issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Check raw template HP adjustments before any mobile is instantiated.
 * An unset scalar is valid; an adjustment below -99 is not.
 */
int validate_mob_hp_modifiers(void)
{
        char buf[MAX_STRING_LENGTH];
        int sn;
        int value;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                value = species_table[sn].hp_mod;

                if (value == MOB_TEMPLATE_UNSET || value >= MOB_HP_MOD_MIN)
                        continue;

                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Body species '%s' has invalid hp_mod %d; "
                    "use MOB_TEMPLATE_UNSET or a value from %d to %d.",
                    species_table[sn].species
                        ? species_table[sn].species : "unnamed",
                    value, MOB_HP_MOD_MIN, INT_MAX);
                log_string(buf);
                issues++;
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                value = mob_table[sn].hp_mod;

                if (value == MOB_TEMPLATE_UNSET || value >= MOB_HP_MOD_MIN)
                        continue;

                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Creature archetype '%s' has invalid "
                    "hp_mod %d; use MOB_TEMPLATE_UNSET or a value "
                    "from %d to %d.",
                    mob_table[sn].name ? mob_table[sn].name : "unnamed",
                    value, MOB_HP_MOD_MIN, INT_MAX);
                log_string(buf);
                issues++;
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] HP-modifier validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] HP-modifier validation complete: "
                    "%d issue%s found.",
                    issues, issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Apply a resolved HP adjustment to the normal level/rank-derived spawn HP.
 * Call once during creation, before equipment or later scripted adjustments.
 */
int apply_mob_hp_modifier(int base_hp, int modifier)
{
        int64_t scaled;

        /* Defensive handling for callers outside the validated loader. */
        if (modifier == MOB_TEMPLATE_UNSET)
                modifier = 0;

        if (modifier < MOB_HP_MOD_MIN)
                modifier = MOB_HP_MOD_MIN;

        if (base_hp < 1)
                base_hp = 1;

        scaled = (int64_t)base_hp * ((int64_t)100 + modifier) / 100;

        if (scaled < 1)
                return 1;

        if (scaled > MOB_SPAWN_HP_LIMIT)
                return MOB_SPAWN_HP_LIMIT;

        return (int)scaled;
}

/*
 * Validate both raw table layers before loading individual mobiles.
 */
int validate_mob_damage_modifiers(void)
{
        char buf[MAX_STRING_LENGTH];
        int sn;
        int value;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                value = species_table[sn].dam_mod;

                if (value == MOB_TEMPLATE_UNSET
                ||  value >= MOB_DAMAGE_MOD_MIN)
                {
                        continue;
                }

                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Body species '%s' has invalid dam_mod %d; "
                    "use MOB_TEMPLATE_UNSET or a value from %d to %d.",
                    species_table[sn].species
                        ? species_table[sn].species : "unnamed",
                    value, MOB_DAMAGE_MOD_MIN, INT_MAX);
                log_string(buf);
                issues++;
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                value = mob_table[sn].dam_mod;

                if (value == MOB_TEMPLATE_UNSET
                ||  value >= MOB_DAMAGE_MOD_MIN)
                {
                        continue;
                }

                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Creature archetype '%s' has invalid "
                    "dam_mod %d; use MOB_TEMPLATE_UNSET or a value "
                    "from %d to %d.",
                    mob_table[sn].name ? mob_table[sn].name : "unnamed",
                    value, MOB_DAMAGE_MOD_MIN, INT_MAX);
                log_string(buf);
                issues++;
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Damage-modifier validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf, sizeof(buf),
                    "[MOB TEMPLATE] Damage-modifier validation complete: "
                    "%d issue%s found.",
                    issues, issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Scale one NPC attack resolved through one_hit(). Call once, before the
 * resistance/defence pipeline. This does not alter the stored modifier.
 */
int apply_mob_damage_modifier(CHAR_DATA *mob, int damage)
{
        int modifier;
        int64_t scaled;

        if (!mob || !IS_NPC(mob) || damage <= 0)
                return damage;

        modifier = mob->dam_mod;

        if (modifier == MOB_TEMPLATE_UNSET)
                modifier = 0;

        if (modifier < MOB_DAMAGE_MOD_MIN)
                modifier = MOB_DAMAGE_MOD_MIN;

        scaled =
            (int64_t)damage * ((int64_t)100 + modifier) / 100;

        if (scaled < 1)
                return 1;

        if (scaled > MOB_ATTACK_DAMAGE_LIMIT)
                return MOB_ATTACK_DAMAGE_LIMIT;

        return (int)scaled;
}

/*
 * Parse one decimal mask or a compact '|' expression.
 *
 * valid_mask determines which flag namespace is accepted.
 * On failure, the caller's output value remains unchanged.
 */
static bool parse_mob_flag_mask(const char *text,
                                unsigned long int valid_mask,
                                unsigned long int *mask)
{
        const char *p;
        unsigned long int result;
        unsigned long int value;
        unsigned long int digit;

        if (!text || !mask || text[0] == '\0')
                return FALSE;

        p = text;
        result = 0;

        for (;;)
        {
                if (*p < '0' || *p > '9')
                        return FALSE;

                value = 0;

                do
                {
                        digit = (unsigned long int)(*p - '0');

                        if (value > (ULONG_MAX - digit) / 10)
                                return FALSE;

                        value = value * 10 + digit;
                        p++;
                }
                while (*p >= '0' && *p <= '9');

                if (value & ~valid_mask)
                        return FALSE;

                result |= value;

                if (*p == '\0')
                        break;

                if (*p != '|')
                        return FALSE;

                p++;
        }

        *mask = result;
        return TRUE;
}

/*
 * Preserve the existing resistance-parser interface.
 */
bool parse_mob_resistance_mask(const char *text,
                                unsigned long int *mask)
{
        return parse_mob_flag_mask(text, RES_VALID_MASK, mask);
}

/*
 * Parse the separate natural attack-part namespace.
 */
bool parse_mob_attack_parts_mask(const char *text,
                                 unsigned long int *mask)
{
        return parse_mob_flag_mask(text, MOB_ATTACK_PARTS_VALID_MASK, mask);
}

/*
 * Validate raw body-species and archetype attack-part masks.
 *
 * Validate both raw layers: identical undefined bits could otherwise
 * cancel during XOR and disappear from the resolved result.
 */
int validate_mob_attack_part_tables(void)
{
        unsigned long int invalid;
        char buf[MAX_STRING_LENGTH];
        int sn;
        int issues;

        issues = 0;

        for (sn = 0; sn < MAX_SPECIES; sn++)
        {
                invalid =
                    species_table[sn].attack_parts
                    & ~MOB_ATTACK_PARTS_VALID_MASK;

                if (invalid == 0)
                        continue;

                snprintf(
                    buf,
                    sizeof(buf),
                    "[MOB TEMPLATE] Body species '%s' has undefined "
                    "attack-part bits: %lu.",
                    species_table[sn].species
                        ? species_table[sn].species
                        : "unnamed",
                    invalid);
                log_string(buf);
                issues++;
        }

        for (sn = 0; sn < MAX_MOB; sn++)
        {
                invalid =
                    mob_table[sn].attack_parts
                    & ~MOB_ATTACK_PARTS_VALID_MASK;

                if (invalid == 0)
                        continue;

                snprintf(
                    buf,
                    sizeof(buf),
                    "[MOB TEMPLATE] Creature archetype '%s' has undefined "
                    "attack-part bits: %lu.",
                    mob_table[sn].name
                        ? mob_table[sn].name
                        : "unnamed",
                    invalid);
                log_string(buf);
                issues++;
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Attack-part validation complete: "
                    "no issues found.");
        }
        else
        {
                snprintf(
                    buf,
                    sizeof(buf),
                    "[MOB TEMPLATE] Attack-part validation complete: "
                    "%d issue%s found.",
                    issues,
                    issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Return the configured natural attacks which the NPC can currently use.
 * This is a read-only anatomical filter; do not alter the stored live mask.
 */
unsigned long int mob_usable_attack_parts(CHAR_DATA *mob)
{
        unsigned long int parts;
        bool arms_usable;
        bool legs_usable;

        if (!mob || !IS_NPC(mob))
                return 0;

        parts = mob->attack_parts & MOB_ATTACK_PARTS_VALID_MASK;

        /* These features have no ordinary offensive action in this chunk. */
        parts &= ~(PART_FEATHERS | PART_HUSK_SHELL);

        if (!HAS_HEAD(mob)
        ||  (IS_AFFECTED(mob, AFF_HEAD_TRAUMA)
             && creature_trait_value(mob,
                 CREATURE_TRAIT_HEAD_TRAUMA_BLOCKS_ATTACKS) > 0))
                parts &= ~(PART_FANGS | PART_HORNS | PART_TUSKS | PART_BEAK);

        arms_usable =
            HAS_ARMS(mob) && !IS_AFFECTED(mob, AFF_ARM_TRAUMA);

        legs_usable =
            HAS_LEGS(mob) && !IS_AFFECTED(mob, AFF_LEG_TRAUMA);

        /* Claws and paws may belong to either usable limb group. */
        if (!arms_usable && !legs_usable)
                parts &= ~(PART_CLAWS | PART_PAWS);

        if (!legs_usable)
                parts &= ~(PART_HAUNCH | PART_HOOVES | PART_FORELEGS);

        if (!arms_usable)
                parts &= ~PART_TENTACLES;

        if (!HAS_TAIL(mob) || IS_AFFECTED(mob, AFF_TAIL_TRAUMA))
                parts &= ~PART_TAILATTACK;

        return parts;
}

/*
 * Choose one ordinary damage type from the usable natural attack parts.
 * One configured part is one candidate; this never creates an extra attack.
 * The caller remains responsible for weapon precedence and attack timing.
 */
int mob_natural_attack_type(CHAR_DATA *mob)
{
        static const struct
        {
                unsigned long int part;
                int damage_type;
        } attacks[] =
        {
                { PART_CLAWS,       TYPE_HIT + 5  }, /* claw: slash */
                { PART_FANGS,       TYPE_HIT + 10 }, /* bite: pierce */
                { PART_HORNS,       TYPE_HIT + 11 }, /* pierce */
                { PART_TUSKS,       TYPE_HIT + 11 }, /* pierce */
                { PART_TAILATTACK,  TYPE_HIT + 7  }, /* pound: blunt */
                { PART_SHARPSCALES, TYPE_HIT + 14 }, /* rake: slash */
                { PART_BEAK,        TYPE_HIT + 11 }, /* pierce */
                { PART_HAUNCH,      TYPE_HIT + 8  }, /* crush: blunt */
                { PART_HOOVES,      TYPE_HIT + 7  }, /* pound: blunt */
                { PART_PAWS,        TYPE_HIT + 7  }, /* pound: blunt */
                { PART_FORELEGS,    TYPE_HIT + 7  }, /* pound: blunt */
                { PART_TENTACLES,   TYPE_HIT + 4  }  /* whip: slash */
        };
        unsigned long int parts;
        size_t i;
        int count;
        int choice;
        int native_type;

        parts = mob_usable_attack_parts(mob);
        count = 0;

        for (i = 0; i < sizeof(attacks) / sizeof(attacks[0]); i++)
        {
                if (parts & attacks[i].part)
                        count++;
        }

        if (count == 0)
        {
                /* An animated blade is its own weapon, without invented limbs. */
                native_type = mob && IS_NPC(mob)
                    ? creature_trait_value(mob, CREATURE_TRAIT_NATURAL_ATTACK_TYPE)
                    : 0;
                return native_type > 0 && native_type <= 19
                    ? TYPE_HIT + native_type : TYPE_HIT;
        }

        choice = count == 1 ? 1 : number_range(1, count);

        for (i = 0; i < sizeof(attacks) / sizeof(attacks[0]); i++)
        {
                if (!(parts & attacks[i].part))
                        continue;

                if (--choice == 0)
                        return attacks[i].damage_type;
        }

        return TYPE_HIT;
}

/*
 * Validate an effective resistance state, not raw XOR input masks.
 */
bool mob_resistance_masks_valid(unsigned long int resists,
                                unsigned long int vulnerabilities,
                                unsigned long int immunes)
{
        return (((resists | vulnerabilities | immunes)
                 & ~RES_VALID_MASK) == 0
             && (resists & vulnerabilities) == 0
             && (resists & immunes) == 0
             && (vulnerabilities & immunes) == 0);
}

/*
 * Report inherited bits explicitly cancelled by the individual mobile.
 * Protected engine bits are removed from the supplied masks by the caller.
 */
static void log_mob_flag_cancellations(
    MOB_INDEX_DATA *index,
    const char *field,
    unsigned long int inherited,
    unsigned long int individual,
    char *(*flag_name)(unsigned long int))
{
        unsigned long int cancelled;
        unsigned long int bit;
        char buf[MAX_STRING_LENGTH];

        cancelled = inherited & individual;

        for (bit = 1; bit != 0; bit <<= 1)
        {
                if (!(cancelled & bit))
                        continue;

                snprintf(
                    buf,
                    sizeof(buf),
                    "[MOB TEMPLATE] vnum %d (%s): %s flag '%s' "
                    "(bit %lu), inherited via '%s', overridden OFF "
                    "by #MOBILES.",
                    index->vnum,
                    index->short_descr ? index->short_descr : "unnamed",
                    field,
                    flag_name(bit),
                    bit,
                    index->mobspec ? index->mobspec : "none");
                log_string(buf);
        }
}

/*
 * Initialise prototype flags from the resolved template and original area
 * masks. Call once at the end of load_mobiles() for each complete entry.
 * Never XOR an already-resolved prototype or live character in place.
 */
void initialise_mob_index_flags(MOB_INDEX_DATA *index)
{
        MOB_TEMPLATE_DATA inherited;
        unsigned long int invalid;
        unsigned long int bit;
        char buf[MAX_STRING_LENGTH];

        if (!index)
                return;

        memset(&inherited, 0, sizeof(inherited));

        if (index->mobspec && index->mobspec[0] != '\0')
        {
                if (!resolve_mob_template(
                        mob_lookup(index->mobspec),
                        &inherited))
                {
                        memset(&inherited, 0, sizeof(inherited));

                        snprintf(
                            buf,
                            sizeof(buf),
                            "[MOB TEMPLATE] vnum %d: cannot resolve '%s'; "
                            "using individual area flags only.",
                            index->vnum,
                            index->mobspec);
                        log_string(buf);
                }
        }

        index->act =
            (inherited.act ^ index->area_act) | ACT_IS_NPC;

        index->affected_by =
            (inherited.affected_by ^ index->area_affected_by)
            & ~(unsigned long int)AFF_CHARM;

        index->body_form =
            inherited.body_form ^ index->area_body_form;

        index->attack_parts =
            inherited.attack_parts ^ index->area_attack_parts;

        /*
         * Scalars replace inherited values; they are not XOR masks.
         * The existing scalar resolver preserves explicit zero.
         */
        index->hp_mod =
            resolve_template_scalar(
                inherited.hp_mod,
                index->area_hp_mod);

        index->dam_mod =
            resolve_template_scalar(
                inherited.dam_mod,
                index->area_dam_mod);

        index->crit_mod =
            resolve_template_scalar(
                inherited.crit_mod,
                index->area_crit_mod);

                index->haste_mod =
            resolve_template_scalar(
                inherited.haste_mod,
                index->area_haste_mod);

        index->height =
            resolve_template_scalar(
                inherited.height,
                index->area_height);

        index->length =
            resolve_template_scalar(
                inherited.length,
                index->area_length);

        index->weight =
            resolve_template_scalar(
                inherited.weight,
                index->area_weight);

        index->size =
            resolve_template_scalar(
                inherited.size,
                index->area_size);

        index->language =
            resolve_template_scalar(
                inherited.language,
                index->area_language);

        index->initial_ghost_phase =
            resolve_template_scalar(
                inherited.initial_ghost_phase,
                index->area_ghost_phase);

        index->resists =
            inherited.resists ^ index->area_resists;

        index->vulnerabilities =
            inherited.vulnerabilities ^ index->area_vulnerabilities;

        index->immunes =
            inherited.immunes ^ index->area_immunes;

        log_mob_flag_cancellations(
            index,
            "ACT",
            inherited.act & ~(unsigned long int)ACT_IS_NPC,
            index->area_act,
            act_bit_name);

        log_mob_flag_cancellations(
            index,
            "AFF",
            inherited.affected_by & ~(unsigned long int)AFF_CHARM,
            index->area_affected_by,
            affect_bit_name);

        log_mob_flag_cancellations(
            index,
            "BODY",
            inherited.body_form,
            index->area_body_form,
            body_form_name);

        log_mob_flag_cancellations(
            index,
            "ATTACK PART",
            inherited.attack_parts,
            index->area_attack_parts,
            body_form_name);

        log_mob_flag_cancellations(
            index,
            "RESIST",
            inherited.resists,
            index->area_resists,
            resist_name);

        log_mob_flag_cancellations(
            index,
            "VULNERABILITY",
            inherited.vulnerabilities,
            index->area_vulnerabilities,
            resist_name);

        log_mob_flag_cancellations(
            index,
            "IMMUNITY",
            inherited.immunes,
            index->area_immunes,
            resist_name);

        /*
         * Input masks may overlap legitimately. The final effective state
         * must not contain unknown bits or contradictory categories.
         */
        invalid =
            (index->resists & index->vulnerabilities)
            | (index->resists & index->immunes)
            | (index->vulnerabilities & index->immunes)
            | ((index->resists | index->vulnerabilities | index->immunes)
               & ~RES_VALID_MASK);

        for (bit = 1; bit != 0; bit <<= 1)
        {
                if (!(invalid & bit))
                        continue;

                snprintf(
                    buf,
                    sizeof(buf),
                    "[MOB TEMPLATE] vnum %d: invalid effective resistance "
                    "'%s' (bit %lu): %s. See mstat resistance layers.",
                    index->vnum,
                    resist_name(bit),
                    bit,
                    (bit & RES_VALID_MASK)
                        ? "present in multiple categories"
                        : "undefined resistance bit");
                log_string(buf);
        }
}

/*
 * Return the effective XP percentage for a character.
 *
 * exp_modifier already contains the existing XP contributions assigned by
 * the normal creation/reset machinery. Add the archetype adjustment once,
 * without changing the stored value.
 *
 * Keeping this calculation separate prevents repeated queries from adding
 * the archetype bonus repeatedly, and prevents reset assignments from
 * accidentally overwriting it.
 */
int get_mob_exp_modifier(CHAR_DATA *mob)
{
        MOB_TEMPLATE_DATA resolved;
        int64_t modifier;

        if (!mob)
                return 0;

        /*
         * This extension does not change player XP-modifier behaviour.
         */
        if (!IS_NPC(mob))
                return mob->exp_modifier;

        modifier = mob->exp_modifier;

        if (resolve_mob_template(
                mob_type_sn(mob),
                &resolved))
        {
                modifier += resolved.xp_mod;
        }

        /*
         * Negative percentages must not turn a kill reward into an
         * XP deduction. Existing minimum-reward rules remain separate.
         */
        if (modifier < 0)
                return 0;

        if (modifier > INT_MAX)
                return INT_MAX;

        return (int)modifier;
}



/*
 * Validate one non-empty default special-function name.
 *
 * NULL means inherit.
 * An empty string means explicitly clear an inherited special.
 */
static int validate_template_special(
    const char *owner_type,
    const char *owner_name,
    const char *slot_name,
    const char *special_name)
{
        char buf[MAX_STRING_LENGTH];

        if (!special_name || special_name[0] == '\0')
                return 0;

        if (spec_lookup(special_name))
                return 0;

        sprintf(
            buf,
            "[MOB TEMPLATE] %s '%s' references unknown %s '%s'.",
            owner_type,
            owner_name,
            slot_name,
            special_name);
        log_string(buf);

        return 1;
}

/*
 * Validate body-species names, creature-archetype relationships,
 * and default special-function names.
 *
 * Resistance masks are validated separately by
 * validate_mob_resistance_table().
 */
int validate_mob_template_tables(void)
{
        char buf[MAX_STRING_LENGTH];
        int issues;
        int mob_type;
        int other;
        int species;

        issues = 0;

        /*
         * Validate the body-species table.
         */
        for (species = 0; species < MAX_SPECIES; species++)
        {
                if (!species_table[species].species
                ||  species_table[species].species[0] == '\0')
                {
                        sprintf(
                            buf,
                            "[MOB TEMPLATE] Body-species entry %d "
                            "has no name.",
                            species);
                        log_string(buf);
                        issues++;
                        continue;
                }

                /*
                 * Body-species names must be unique.
                 */
                for (other = species + 1;
                     other < MAX_SPECIES;
                     other++)
                {
                        if (!species_table[other].species
                        ||  species_table[other].species[0] == '\0')
                        {
                                continue;
                        }

                        if (!str_cmp(
                                species_table[species].species,
                                species_table[other].species))
                        {
                                sprintf(
                                    buf,
                                    "[MOB TEMPLATE] Duplicate body-species "
                                    "name '%s' at entries %d and %d.",
                                    species_table[species].species,
                                    species,
                                    other);
                                log_string(buf);
                                issues++;
                        }
                }

                /*
                 * Validate all three default special-function slots
                 * for this body species.
                 */
                issues += validate_template_special(
                    "Body species",
                    species_table[species].species,
                    "spec_fun1",
                    species_table[species].spec_fun1);

                issues += validate_template_special(
                    "Body species",
                    species_table[species].species,
                    "spec_fun2",
                    species_table[species].spec_fun2);

                issues += validate_template_special(
                    "Body species",
                    species_table[species].species,
                    "spec_boss",
                    species_table[species].spec_boss);
        }

        /*
         * Validate creature-archetype relationships and default specials.
         */
        for (mob_type = 0; mob_type < MAX_MOB; mob_type++)
        {
                if (!mob_table[mob_type].name
                ||  mob_table[mob_type].name[0] == '\0')
                {
                        /*
                         * The mob resistance validator already reports
                         * unnamed and duplicate archetype entries.
                         */
                        continue;
                }

                if (!mob_table[mob_type].species
                ||  mob_table[mob_type].species[0] == '\0')
                {
                        sprintf(
                            buf,
                            "[MOB TEMPLATE] Creature archetype '%s' "
                            "has no body-species name.",
                            mob_table[mob_type].name);
                        log_string(buf);
                        issues++;
                        continue;
                }

                if (species_lookup(mob_table[mob_type].species) < 0)
                {
                        sprintf(
                            buf,
                            "[MOB TEMPLATE] Creature archetype '%s' "
                            "references unknown body species '%s'.",
                            mob_table[mob_type].name,
                            mob_table[mob_type].species);
                        log_string(buf);
                        issues++;
                }

                if ((unsigned int)mob_table[mob_type].family >= CREATURE_FAMILY_MAX)
                {
                        snprintf(buf, sizeof(buf),
                            "[MOB TEMPLATE] Archetype '%s' has an invalid creature family.",
                            mob_table[mob_type].name);
                        log_string(buf);
                        issues++;
                }

                /*
                 * Validate all three default special-function slots
                 * for this creature archetype.
                 */
                issues += validate_template_special(
                    "Creature archetype",
                    mob_table[mob_type].name,
                    "spec_fun1",
                    mob_table[mob_type].spec_fun1);

                issues += validate_template_special(
                    "Creature archetype",
                    mob_table[mob_type].name,
                    "spec_fun2",
                    mob_table[mob_type].spec_fun2);

                issues += validate_template_special(
                    "Creature archetype",
                    mob_table[mob_type].name,
                    "spec_boss",
                    mob_table[mob_type].spec_boss);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TEMPLATE] Relationship validation complete: "
                    "no issues found.");
        }
        else
        {
                sprintf(
                    buf,
                    "[MOB TEMPLATE] Relationship validation complete: "
                    "%d issue%s found.",
                    issues,
                    issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Log unknown bits from one mob resistance field.
 */
static int log_unknown_resistance_bits(const char *mob_name,
                                       const char *field_name,
                                       unsigned long int bits)
{
        unsigned long int bit;
        char buf[MAX_STRING_LENGTH];
        int issues;

        issues = 0;

        for (bit = 1; bit > 0 && bit <= BIT_MAX; bit *= 2)
        {
                if (!IS_SET(bits, bit))
                        continue;

                sprintf(buf,
                        "[MOB TYPE] '%s': unknown resistance bit %lu in %s.",
                        mob_name,
                        bit,
                        field_name);
                log_string(buf);
                issues++;
        }

        return issues;
}

/*
 * Log bits which appear in contradictory resistance fields.
 */
static int log_resistance_conflicts(const char *mob_name,
                                    const char *description,
                                    unsigned long int bits)
{
        unsigned long int bit;
        char buf[MAX_STRING_LENGTH];
        int issues;

        issues = 0;

        for (bit = 1; bit > 0 && bit <= BIT_MAX; bit *= 2)
        {
                if (!IS_SET(bits, bit))
                        continue;

                sprintf(buf,
                        "[MOB TYPE] '%s': %s %s.",
                        mob_name,
                        description,
                        resist_name(bit));
                log_string(buf);
                issues++;
        }

        return issues;
}

int validate_mob_resistance_table(void)
{
        MOB_TEMPLATE_DATA resolved;
        unsigned long int unknown_bits;
        unsigned long int overlap;
        char buf[MAX_STRING_LENGTH];
        int mob_type;
        int other;
        int species;
        int issues;

        issues = 0;

        /*
         * Body-species resistance data is a base state, so contradictions
         * within this layer are always invalid.
         */
        for (species = 0; species < MAX_SPECIES; species++)
        {
                if (!species_table[species].species
                ||  species_table[species].species[0] == '\0')
                {
                        continue;
                }

                unknown_bits =
                    species_table[species].resists
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    species_table[species].species,
                    "body-species resists",
                    unknown_bits);

                unknown_bits =
                    species_table[species].vulnerabilities
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    species_table[species].species,
                    "body-species vulnerabilities",
                    unknown_bits);

                unknown_bits =
                    species_table[species].immunes
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    species_table[species].species,
                    "body-species immunes",
                    unknown_bits);

                overlap =
                    (species_table[species].resists
                     & species_table[species].vulnerabilities)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    species_table[species].species,
                    "body species is both resistant and vulnerable to",
                    overlap);

                overlap =
                    (species_table[species].resists
                     & species_table[species].immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    species_table[species].species,
                    "body species is both resistant and immune to",
                    overlap);

                overlap =
                    (species_table[species].vulnerabilities
                     & species_table[species].immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    species_table[species].species,
                    "body species is both vulnerable and immune to",
                    overlap);
        }

        for (mob_type = 0; mob_type < MAX_MOB; mob_type++)
        {
                if (!mob_table[mob_type].name
                ||  mob_table[mob_type].name[0] == '\0')
                {
                        sprintf(
                            buf,
                            "[MOB TYPE] entry %d has no name.",
                            mob_type);
                        log_string(buf);
                        issues++;
                        continue;
                }

                /*
                 * Duplicate names make archetype lookup ambiguous.
                 */
                for (other = mob_type + 1;
                     other < MAX_MOB;
                     other++)
                {
                        if (!mob_table[other].name
                        ||  mob_table[other].name[0] == '\0')
                        {
                                continue;
                        }

                        if (!str_cmp(
                                mob_table[mob_type].name,
                                mob_table[other].name))
                        {
                                sprintf(
                                    buf,
                                    "[MOB TYPE] duplicate name '%s' at "
                                    "entries %d and %d.",
                                    mob_table[mob_type].name,
                                    mob_type,
                                    other);
                                log_string(buf);
                                issues++;
                        }
                }

                /*
                 * Raw archetype masks are XOR masks. They may legitimately
                 * contain a bit in two fields when moving that bit from one
                 * inherited category to another. Validate unknown bits here,
                 * then validate contradictions only after full resolution.
                 */
                unknown_bits =
                    mob_table[mob_type].resists
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "archetype resists",
                    unknown_bits);

                unknown_bits =
                    mob_table[mob_type].vulnerabilities
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "archetype vulnerabilities",
                    unknown_bits);

                unknown_bits =
                    mob_table[mob_type].immunes
                    & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "archetype immunes",
                    unknown_bits);

                /*
                 * Structural relationship errors were already reported by
                 * validate_mob_template_tables().
                 */
                if (!resolve_mob_template(
                        mob_type,
                        &resolved))
                {
                        continue;
                }

                overlap =
                    (resolved.resists
                     & resolved.vulnerabilities)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "resolved template is both resistant and vulnerable to",
                    overlap);

                overlap =
                    (resolved.resists
                     & resolved.immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "resolved template is both resistant and immune to",
                    overlap);

                overlap =
                    (resolved.vulnerabilities
                     & resolved.immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "resolved template is both vulnerable and immune to",
                    overlap);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TYPE] Resistance validation complete: "
                    "no issues found.");
        }
        else
        {
                sprintf(
                    buf,
                    "[MOB TYPE] Resistance validation complete: "
                    "%d issue%s found.",
                    issues,
                    issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}

/*
 * Three-slot mobile specials.
 *
 * Percentages choose one routine per existing special opportunity.
 * They do not bypass that routine's own conditions or cause retries
 * when it returns FALSE.
 */

/*
 * A whole-vector inheritance or automatic marker must occupy
 * all three components.
 */
static bool special_policy_is(const int *chance, int value)
{
        int i;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (chance[i] != value)
                        return FALSE;
        }

        return TRUE;
}

bool mob_special_policy_valid(const int *chance)
{
        int i;
        int total;

        if (!chance)
                return FALSE;

        if (special_policy_is(chance, MOB_TEMPLATE_UNSET)
        ||  special_policy_is(chance, MOB_SPECIAL_AUTO))
        {
                return TRUE;
        }

        total = 0;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (chance[i] < 0 || chance[i] > 100)
                        return FALSE;

                total += chance[i];
        }

        /*
         * An all-zero vector is valid only for an empty final set.
         * build_mob_specials() checks that relationship.
         */
        return total == 100 || total == 0;
}

bool parse_mob_special_percent(const char *text, int *value)
{
        const char *p;
        int result;

        if (!text || !value || text[0] == '\0')
                return FALSE;

        result = 0;

        for (p = text; *p; p++)
        {
                if (*p < '0' || *p > '9')
                        return FALSE;

                result = result * 10 + (*p - '0');

                if (result > 100)
                        return FALSE;
        }

        *value = result;
        return TRUE;
}

/*
 * Resolve names and a complete probability policy into executable data.
 *
 * NULL or empty names mean an empty final slot. Name inheritance must
 * already have been resolved by the caller.
 *
 * The caller's result is unchanged on failure.
 */
bool build_mob_specials(const char *const *names,
                        const int *chance,
                        MOB_SPECIAL_DATA *result,
                        char *error,
                        size_t error_size)
{
        MOB_SPECIAL_DATA next;
        int i;
        int count;
        int remaining;
        int total;
        bool automatic;

        if (!names || !chance || !result || !error || error_size == 0)
                return FALSE;

        error[0] = '\0';
        memset(&next, 0, sizeof(next));
        count = 0;

        if (!mob_special_policy_valid(chance))
        {
                snprintf(
                    error, error_size,
                    "percentages must be 0..100 and total 100; "
                    "use a complete inherit/auto policy, not mixed markers");
                return FALSE;
        }

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (!names[i] || names[i][0] == '\0')
                        continue;

                next.fun[i] = spec_lookup(names[i]);

                if (!next.fun[i])
                {
                        snprintf(
                            error, error_size,
                            "unknown function in slot %d: '%s'",
                            i + 1, names[i]);
                        return FALSE;
                }

                count++;
        }

        automatic =
            special_policy_is(chance, MOB_SPECIAL_AUTO)
            || special_policy_is(chance, MOB_TEMPLATE_UNSET);

        remaining = count ? 100 % count : 0;
        total = 0;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (automatic)
                {
                        next.chance[i] =
                            next.fun[i] ? 100 / count : 0;

                        if (next.fun[i] && remaining > 0)
                        {
                                next.chance[i]++;
                                remaining--;
                        }
                }
                else
                {
                        next.chance[i] = chance[i];
                }

                if (!next.fun[i] && next.chance[i] != 0)
                {
                        snprintf(
                            error, error_size,
                            "empty slot %d must have 0 percent",
                            i + 1);
                        return FALSE;
                }

                total += next.chance[i];
        }

        if ((count > 0 && total != 100)
        ||  (count == 0 && total != 0))
        {
                snprintf(
                    error, error_size,
                    "configured functions require total 100; "
                    "an entirely empty set requires 0/0/0 (got %d)",
                    total);
                return FALSE;
        }

        *result = next;
        return TRUE;
}

/*
 * Validate an effective executable configuration.
 */
bool mob_specials_valid(const MOB_SPECIAL_DATA *set)
{
        int i;
        int count;
        int total;

        if (!set)
                return FALSE;

        count = 0;
        total = 0;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (set->chance[i] < 0 || set->chance[i] > 100
                ||  (!set->fun[i] && set->chance[i] != 0))
                {
                        return FALSE;
                }

                if (set->fun[i])
                        count++;

                total += set->chance[i];
        }

        return count ? total == 100 : total == 0;
}

/*
 * Disabled slots do not grant special-gated capabilities.
 */
bool mob_has_special(CHAR_DATA *mob, SPEC_FUN *special)
{
        int i;

        if (!mob || !IS_NPC(mob) || !special)
                return FALSE;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (mob->specials.chance[i] > 0
                &&  mob->specials.fun[i] == special)
                {
                        return TRUE;
                }
        }

        return FALSE;
}

bool mob_has_specials(CHAR_DATA *mob)
{
        int i;

        if (!mob || !IS_NPC(mob))
                return FALSE;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (mob->specials.fun[i]
                &&  mob->specials.chance[i] > 0)
                {
                        return TRUE;
                }
        }

        return FALSE;
}

/*
 * Replace a complete live configuration atomically.
 *
 * Do not release special_running here: a callback may replace its own
 * configuration while its current invocation is still running.
 */
bool set_mob_specials(CHAR_DATA *mob, const MOB_SPECIAL_DATA *set)
{
        if (!mob || !mob_specials_valid(set))
                return FALSE;

        mob->specials = *set;
        memset(mob->special_calls, 0, sizeof(mob->special_calls));

        return TRUE;
}

/*
 * Preserve existing code that deliberately assigns one runtime special.
 * A singleton occupies slot 1 at 100 percent; other slots are cleared.
 */
void set_mob_single_special(CHAR_DATA *mob, SPEC_FUN *special)
{
        MOB_SPECIAL_DATA next;

        if (!mob)
                return;

        memset(&next, 0, sizeof(next));
        next.fun[0] = special;
        next.chance[0] = special ? 100 : 0;

        set_mob_specials(mob, &next);
}

/*
 * Invoke at most one routine.
 *
 * Capability checks inspect all enabled slots. Do not temporarily replace
 * the mobile's identity with whichever function was selected.
 */
bool run_mob_special(CHAR_DATA *mob)
{
        ROOM_INDEX_DATA *room;
        SPEC_FUN *selected;
        int i;
        int slot;
        int draw;
        bool handled;
        MAGIC_ACTIVATION_DATA activation;

        if (!mob || !IS_NPC(mob))
                return FALSE;

        if (mob->deleted
        ||  !mob->in_room
        ||  mob->position == POS_DEAD)
        {
                return TRUE;
        }

        if (mob->special_running)
                return TRUE;

        if (!mob_specials_valid(&mob->specials))
        {
                bug(
                    "Run_mob_special: invalid live distribution for vnum %d.",
                    mob->pIndexData ? mob->pIndexData->vnum : 0);
                return FALSE;
        }

        if (!mob_has_specials(mob))
                return FALSE;

        slot = -1;

        /*
         * A single 100-percent choice needs no additional random draw.
         * This preserves the old random-number cadence for singleton sets.
         */
        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (mob->specials.chance[i] == 100)
                        slot = i;
        }

        if (slot < 0)
        {
                draw = number_range(1, 100);

                for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
                {
                        if (draw <= mob->specials.chance[i])
                        {
                                slot = i;
                                break;
                        }

                        draw -= mob->specials.chance[i];
                }
        }

        if (slot < 0 || !mob->specials.fun[slot])
                return FALSE;

        selected = mob->specials.fun[slot];
        room = mob->in_room;

        if (mob->special_calls[slot] < ULONG_MAX)
                mob->special_calls[slot]++;

        mob->special_running = TRUE;
        begin_magic_activation(&activation, mob);
        handled = (*selected)(mob);
        end_magic_activation(&activation);
        mob->special_running = FALSE;

        /*
         * DD4 defers reclaiming extracted characters. Stop this update
         * after a callback deletes, kills, or relocates its mobile.
         */
        return handled
            || mob->deleted
            || mob->position == POS_DEAD
            || mob->in_room != room;
}

/*
 * Map the existing template names onto the three ordinary slots.
 * The historical spec_boss name does not impose a rank restriction.
 */
void mob_template_special_names(const MOB_TEMPLATE_DATA *data,
                                 const char **names)
{
        names[0] = data->spec_fun1;
        names[1] = data->spec_fun2;
        names[2] = data->spec_boss;
}

/*
 * Resolve individual names and the complete probability policy.
 * Called after all #MOBILES and #SPECIALS sections have loaded.
 */
bool resolve_mob_index_specials(MOB_INDEX_DATA *index,
                                 char *error,
                                 size_t error_size)
{
        MOB_TEMPLATE_DATA data;
        const char *names[MOB_SPECIAL_SLOTS];
        const int *chance;
        MOB_SPECIAL_DATA result;
        int i;

        if (!index || !error || error_size == 0)
                return FALSE;

        memset(&data, 0, sizeof(data));

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
                data.spec_chance[i] = MOB_SPECIAL_AUTO;

        if (index->mobspec && index->mobspec[0]
        &&  !resolve_mob_template(mob_lookup(index->mobspec), &data))
        {
                snprintf(
                    error, error_size,
                    "invalid archetype '%s'",
                    index->mobspec);
                return FALSE;
        }

        mob_template_special_names(&data, names);

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                if (index->area_special_name[i])
                        names[i] = index->area_special_name[i];
        }

        chance =
            special_policy_is(
                index->area_spec_chance,
                MOB_TEMPLATE_UNSET)
            ? data.spec_chance
            : index->area_spec_chance;

        if (!build_mob_specials(
                names, chance, &result, error, error_size))
        {
                return FALSE;
        }

        index->specials = result;
        return TRUE;
}

/*
 * Validate names even when their slot has zero probability or a later
 * inheritance layer would replace them.
 */
static int validate_raw_special_name(
    const char *owner,
    const char *name,
    const char *special)
{
        char buf[MAX_STRING_LENGTH];

        if (!special || !special[0] || spec_lookup(special))
                return 0;

        snprintf(
            buf, sizeof(buf),
            "[MOB SPECIALS] %s '%s': unknown function '%s'.",
            owner, name ? name : "unnamed", special);
        log_string(buf);

        return 1;
}

/*
 * Validate raw policies and fully resolved archetype configurations.
 */
int validate_mob_special_templates(void)
{
        MOB_TEMPLATE_DATA data;
        MOB_SPECIAL_DATA set;
        const char *names[MOB_SPECIAL_SLOTS];
        char error[MAX_STRING_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int i;
        int issues;

        issues = 0;

        for (i = 0; i < MAX_SPECIES; i++)
        {
                issues += validate_raw_special_name(
                    "Species",
                    species_table[i].species,
                    species_table[i].spec_fun1);

                issues += validate_raw_special_name(
                    "Species",
                    species_table[i].species,
                    species_table[i].spec_fun2);

                issues += validate_raw_special_name(
                    "Species",
                    species_table[i].species,
                    species_table[i].spec_boss);

                if (!mob_special_policy_valid(
                        species_table[i].spec_chance))
                {
                        snprintf(
                            buf, sizeof(buf),
                            "[MOB SPECIALS] Invalid probability policy "
                            "on species '%s'.",
                            species_table[i].species
                                ? species_table[i].species
                                : "unnamed");
                        log_string(buf);
                        issues++;
                }
        }

        for (i = 0; i < MAX_MOB; i++)
        {
                issues += validate_raw_special_name(
                    "Archetype",
                    mob_table[i].name,
                    mob_table[i].spec_fun1);

                issues += validate_raw_special_name(
                    "Archetype",
                    mob_table[i].name,
                    mob_table[i].spec_fun2);

                issues += validate_raw_special_name(
                    "Archetype",
                    mob_table[i].name,
                    mob_table[i].spec_boss);

                if (!mob_special_policy_valid(
                        mob_table[i].spec_chance))
                {
                        snprintf(
                            buf, sizeof(buf),
                            "[MOB SPECIALS] Invalid probability policy "
                            "on archetype '%s'.",
                            mob_table[i].name
                                ? mob_table[i].name
                                : "unnamed");
                        log_string(buf);
                        issues++;
                        continue;
                }

                if (!resolve_mob_template(i, &data))
                {
                        snprintf(
                            buf, sizeof(buf),
                            "[MOB SPECIALS] Cannot resolve "
                            "archetype entry %d.",
                            i);
                        log_string(buf);
                        issues++;
                        continue;
                }

                mob_template_special_names(&data, names);

                if (!build_mob_specials(
                        names,
                        data.spec_chance,
                        &set,
                        error,
                        sizeof(error)))
                {
                        snprintf(
                            buf, sizeof(buf),
                            "[MOB SPECIALS] Archetype '%s': %.700s.",
                            mob_table[i].name,
                            error);
                        log_string(buf);
                        issues++;
                }
        }

        snprintf(
            buf, sizeof(buf),
            "[MOB SPECIALS] Template validation complete: %d issue%s.",
            issues,
            issues == 1 ? "" : "s");
        log_string(buf);

        return issues;
}

/*
 * Preserve the existing per-special XP schedule.
 * Only the method of combining multiple specials is new.
 */
static int mob_special_exp_bonus(SPEC_FUN *special)
{
        static const struct
        {
                const char *name;
                int bonus;
        } exceptions[] =
        {
                /* These short status actions have no new XP premium. */
                { "spec_homunculus_sleep_bite", 0 },
                { "spec_basilisk_stiffening_gaze", 0 },
                { "spec_paralysing_gas", 0 }, /* No new XP premium for the short gas Hold. */
                { "spec_holy_trumpet", 0 },
                { "spec_lulling_drone", 0 },
                { "spec_fido", 0 },
                { "spec_thief", 0 },
                { "spec_janitor", 0 },
                { "spec_repairman", 0 },
                { "spec_celestial_repairman", 0 },
                { "spec_cast_adept", 0 },
                { "spec_cast_hooker", 0 },
                { "spec_clan_guard", 0 },
                { "spec_manticore", 0 }, /* Its archetype already adds +5 XP. */
                { "spec_alhoon", 0 }, /* Its archetype already adds +10 XP. */
                { "spec_bounty", 0 },
                { "spec_executioner", 0 },
                { "spec_cast_orb", 0 },

                { "spec_poison", 5 },
                { "spec_bear", 5 },
                { "spec_constrictor", 5 },
                { "spec_fascinating_gaze", 5 },
                { "spec_draining_bite", 10 },
                { "spec_electric_eel", 10 },
                { "spec_cooshee", 5 },
                { "spec_brownie", 5 },
                { "spec_poison_palm", 5 },
                { "spec_poison_claw", 5 },
                { "spec_breath_brass", 10 },
                { "spec_paralysing_bite", 10 },
                { "spec_myconid_spores", 5 },
                { "spec_engulf", 10 },
                { "spec_camel_spit", 5 },
                { "spec_poison_bite", 5 },
                { "spec_bloodsucker", 5 },
                { "spec_bloodsucker_bite", 5 },
                { "spec_superwimpy", 5 },
                { "spec_spectral_minion", 5 },
                { "spec_kungfu_poison", 5 },
                { "spec_guard", 5 },
                { "spec_sahuagin_guard", 5 },
                { "spec_cast_judge", 5 },

                { "spec_small_whale", 10 },
                { "spec_large_whale", 10 },
                { "spec_faerie_dragon", 10 },
                { "spec_giant", 10 },
                { "spec_cast_cleric_helper", 10 },
                { "spec_executioner_local", 0 },
                { "spec_grell", 5 },
                { "spec_flind", 5 },
                { "spec_horn_gore", 5 },
                { "spec_terrifying_roar", 5 },
                { "spec_confusing_gaze", 5 },
                { "spec_breath_crystal", 10 },
                { "spec_elemental_burst", 10 },
                { "spec_sprite", 5 },
                { "spec_innate_fade", 5 },
                { "spec_wisp_lightning", 10 },
                { "spec_lightning_discharge", 10 },
                { "spec_acid_spray", 10 },
                { "spec_gas_cloud", 10 },
                { "spec_fire_surge", 10 },
                { "spec_tentacle_grip", 5 },
                { "spec_squid", 5 },
                { "spec_golem_haste", 5 },
                { "spec_vine_grip", 5 },
                { "spec_otyugh", 5 },
                { "spec_cast_slow", 10 },
                { "spec_cast_construct_adept", 10 },
                { "spec_beholder", 15 },
                { "spec_lamia", 10 },
                { "spec_mimic", 5 },
                { "spec_duergar", 10 },
                { "spec_swallow", 10 },
                { "spec_kappa", 10 },
                { "spec_laghathti", 10 },
                { "spec_uzollru", 10 },
                { "spec_warrior", 10 },
                { "spec_psionic_thief", 10 },
                { "spec_breath_gas_anatomical", 10 },
                { "spec_breath_frost_anatomical", 10 },
                { "spec_breath_lightning_anatomical", 10 },
                { "spec_breath_steam_anatomical", 10 },
                { "spec_cleric_adept", 10 },
                { "spec_wight", 10 },
                { "spec_wraith", 10 },
                { "spec_prism_flash", 10 },
                { "spec_sahuagin_infantry", 10 },
                { "spec_sahuagin_cavalry", 10 },
                { "spec_sahuagin_cleric", 10 },
                { "spec_green_grung", 10 },
                { "spec_blue_grung", 10 },

                { "spec_cast_druid", 15 },
                { "spec_spectre", 15 },
                { "spec_mummy", 15 },
                { "spec_banshee", 15 },
                { "spec_death_knight", 15 },
                { "spec_lich", 15 },
                { "spec_dracolich", 15 },
                { "spec_demon", 15 },
                { "spec_guardian_daemon", 15 },
                { "spec_cast_electric", 15 },
                { "spec_assassin", 15 },
                { "spec_aboleth", 15 },
                { "spec_sahuagin_baron", 15 },
                { "spec_sahuagin_lieutenant", 15 },
                { "spec_red_grung", 15 },
                { "spec_purple_grung", 15 },
                { "spec_orange_grung", 15 },
                { "spec_cast_water_sprite", 15 },

                /*
                 * Preserve the legacy spelling/lookup here rather than
                 * silently changing the existing XP balance.
                 */
                { "spec_priestess", 20 },
                { "spec_sahuagin_high_cleric", 20 },
                { "spec_mast_vampire", 20 },
                { "spec_evil_evil_gezhp", 20 },
                { "spec_grail", 20 },
                { "spec_gold_grung", 20 },
                { "spec_cast_archmage", 20 },
                { "spec_sahuagin_prince", 20 }
        };
        size_t i;
        int bonus;

        if (!special)
                return 0;

        bonus = 10;

        for (i = 0;
             i < sizeof(exceptions) / sizeof(exceptions[0]);
             i++)
        {
                if (special == spec_lookup(exceptions[i].name))
                        bonus = exceptions[i].bonus;
        }

        return bonus;
}

int mob_specials_exp_bonus(const MOB_SPECIAL_DATA *set)
{
        int i;
        int weighted;

        if (!mob_specials_valid(set))
                return 0;

        weighted = 0;

        for (i = 0; i < MOB_SPECIAL_SLOTS; i++)
        {
                weighted +=
                    set->chance[i]
                    * mob_special_exp_bonus(set->fun[i]);
        }

        /*
         * Percentage-point adjustment rounded to the nearest integer.
         * Do not sum three full special bonuses.
         */
        return (weighted + 50) / 100;
}
