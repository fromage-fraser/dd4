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
#include <stdio.h>
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
                0, 0,
                0, 0,
                0, 0,
                RES_FIRE | RES_COLD | RES_POISON | RES_PARALYSIS
                    | RES_HOLD | RES_DRAIN | RES_NONMAGIC,
                MOB_TEMPLATE_UNSET, MOB_TEMPLATE_UNSET,
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