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
                NULL, NULL, NULL
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
                NULL, NULL, NULL
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
                NULL, NULL, NULL
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
                NULL, NULL, NULL
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
                0                       /* XP adjustment: percentage points */
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
                "spec_fido", NULL, NULL,
                0                       /* XP adjustment: percentage points */
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
                "spec_breath_fire", NULL, NULL,
                5                       /* XP adjustment: percentage points */
        }
};

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