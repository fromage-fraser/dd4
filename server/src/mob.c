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
                MOB_SPECIAL_CHANCES_INHERIT
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
                MOB_SPECIAL_CHANCES_INHERIT
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
                MOB_SPECIAL_CHANCES_INHERIT
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
                MOB_SPECIAL_CHANCES_INHERIT
        }
};

/*
 * Specific creature archetypes.
 */
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
                MOB_SPECIAL_CHANCES_INHERIT
        },

        {
                "goat", "farm_mammal", "icon1", "icon2",
                0, 0,
                0, 0,
                0,
                RES_FIRE | RES_COLD | RES_ELECTRICITY | RES_ENERGY,
                0,
                20, 20, -10, -2,
                1, 30, 100, 3,
                "spec_fido", "spec_poison", NULL,
                0,
                MOB_SPECIAL_CHANCES_AUTO
        },

        {
                "fire_elemental", "elemental", "icon1", "icon2",
                ACT_UNDEAD, AFF_DETECT_MAGIC,
                0, PART_CLAWS,
                0, 0,
                RES_FIRE | RES_COLD | RES_POISON | RES_PARALYSIS
                    | RES_HOLD | RES_DRAIN | RES_NONMAGIC,
                50, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, 20,
                2, 20, 3, 1,
                "spec_breath_fire", "spec_breath_frost", "spec_poison",
                5,
                { 60, 30, 10 }
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
                    | BODY_INORGANIC
                    | PART_HEART | PART_BRAINS | PART_GUTS
                    | PART_EAR | PART_EYE,

                /* No additional natural attack parts. */
                0,

                /* Resistance, vulnerability and immunity XOR masks. */
                RES_PIERCE | RES_SLASH | RES_DARK,
                RES_BLUNT | RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN,

                /* Neutral HP, damage, critical and swiftness adjustments. */
                0, 0, 0, 0,

                /* Preserve unspecified humanoid dimension/language data. */
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                /* No compulsory inherited special in any slot. */
                "", "", "",

                /* No additional archetype XP adjustment. */
                0,

                                /* Empty names resolve to a valid empty 0/0/0 set. */
                MOB_SPECIAL_CHANCES_AUTO
        },

        {
                "zombie", "humanoid", "icon1", "icon2",

                /* Undead classification and explicit mindlessness. */
                ACT_UNDEAD, AFF_MINDLESS,
                BODY_NO_SPEECH,

                /* No additional natural attack parts. */
                0,

                /* Resistance, vulnerability and immunity XOR masks. */
                RES_DARK,
                RES_HOLY,
                RES_POISON | RES_SLEEP | RES_DRAIN,

                /* Neutral HP, damage, critical and swiftness adjustments. */
                0, 0, 0, 0,

                /* Inherit dimensions and language. */
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                /* No compulsory special functions or extra XP. */
                "", "", "",
                0,
                MOB_SPECIAL_CHANCES_AUTO
        },
        {
                "ghoul", "humanoid", "icon1", "icon2",

                /* Undead, but capable of thought and speech. */
                ACT_UNDEAD, 0,

                /* Retain humanoid anatomy and add the elongated tongue. */
                PART_LONG_TONGUE,

                /* Existing natural claw and bite attacks. */
                PART_CLAWS | PART_FANGS,

                /* No additional resistance or vulnerability. */
                0,
                0,

                /* Explicit ghoul immunities. */
                RES_SLEEP | RES_CHARM,

                /* Keep DD4's ordinary level-scaled combat defaults. */
                0, 0, 0, 0,

                /* Inherit dimensions and language metadata. */
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                /* Feeding and natural-contact paralysis. */
                "spec_ghoul", "", "",

                /* No separate archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO
        },

        {
                "ghast", "humanoid", "icon1", "icon2",

                /* Undead, with thought and speech retained. */
                ACT_UNDEAD, AFF_STENCH,

                /* Organic humanoid anatomy with an elongated tongue. */
                PART_LONG_TONGUE,

                /* Existing natural claw and bite attacks. */
                PART_CLAWS | PART_FANGS,

                /* Resistance and vulnerability XOR masks. */
                0,
                RES_COLD_IRON,

                /* Shared ghoul-family defenses. */
                RES_SLEEP | RES_CHARM,

                /* Preserve ordinary DD4 level-scaled combat defaults. */
                0, 0, 0, 0,

                /* Inherit dimension and language metadata. */
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                                /* Idle feeding and stronger contact paralysis. */
                "spec_ghast", "", "",

                /* No separate archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO
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
                 * No resistance, vulnerability or immunity defaults
                 * are introduced by this physiology stage.
                 *
                 * Phase-dependent weapon defences belong in the
                 * later ghost contact-resolution work.
                 */
                0,
                0,
                0,

                /* Neutral HP, damage, critical and swiftness adjustments. */
                0, 0, 0, 0,

                /* Inherit dimensions and language metadata. */
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,

                /* No active ghost behaviour in this stage. */
                "", "", "",

                /* No additional archetype XP adjustment. */
                0,
                MOB_SPECIAL_CHANCES_AUTO
        }

};

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
 * One validated live-state change. This foundation does not yet apply
 * contact rules or edit flags, resistance masks, followers or combat.
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

        if (!HAS_HEAD(mob))
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
                { PART_FORELEGS,    TYPE_HIT + 7  }  /* pound: blunt */
        };
        unsigned long int parts;
        size_t i;
        int count;
        int choice;

        parts = mob_usable_attack_parts(mob);
        count = 0;

        for (i = 0; i < sizeof(attacks) / sizeof(attacks[0]); i++)
        {
                if (parts & attacks[i].part)
                        count++;
        }

        if (count == 0)
                return TYPE_HIT;

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
        handled = (*selected)(mob);
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
                { "spec_fido", 0 },
                { "spec_thief", 0 },
                { "spec_janitor", 0 },
                { "spec_repairman", 0 },
                { "spec_celestial_repairman", 0 },
                { "spec_cast_adept", 0 },
                { "spec_cast_hooker", 0 },
                { "spec_clan_guard", 0 },
                { "spec_bounty", 0 },
                { "spec_executioner", 0 },
                { "spec_cast_orb", 0 },

                { "spec_poison", 5 },
                { "spec_bloodsucker", 5 },
                { "spec_superwimpy", 5 },
                { "spec_spectral_minion", 5 },
                { "spec_kungfu_poison", 5 },
                { "spec_guard", 5 },
                { "spec_sahuagin_guard", 5 },
                { "spec_cast_judge", 5 },

                { "spec_small_whale", 10 },
                { "spec_large_whale", 10 },
                { "spec_kappa", 10 },
                { "spec_laghathti", 10 },
                { "spec_uzollru", 10 },
                { "spec_warrior", 10 },
                { "spec_sahuagin_infantry", 10 },
                { "spec_sahuagin_cavalry", 10 },
                { "spec_sahuagin_cleric", 10 },
                { "spec_green_grung", 10 },
                { "spec_blue_grung", 10 },

                { "spec_cast_druid", 15 },
                { "spec_demon", 15 },
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