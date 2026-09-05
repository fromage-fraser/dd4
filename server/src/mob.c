/***************************************************************************
 *  mob.c
 *
 *  Mob Specs & Species types go here.
 *
 *
 *
 * Brutus 2022.
 *
 ***************************************************************************/
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
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

const struct species_type  species_table [ MAX_SPECIES ] =
{
        /* species, body parts, attack parts */

        {
                "reserved",
                0,
                0
        },

        {
                "humanoid",
                1024|4096|32768|BIT_15|BIT_18|BIT_19|BIT_20|BIT_21|BIT_22|BIT_23|BIT_24|BIT_25,
                0
        },

        {
                "elemental",
                4|8|16|256|1024, /* body parts */
                0                /* attack parts */
        },

        {
                "farm_mammal",
                BIT_10|BIT_15|BIT_16|BIT_18|BIT_19|BIT_20|BIT_24|BIT_25, /* body parts */
                BIT_41                  /* attack parts */
        }

};

/*
 *  Skill table
 */
const struct mob_type mob_table [MAX_MOB] =
{
  /*
    * name, species, male_icon, female_icon
    * resists, vulnerabilities, immunes,
    * hp %gain, dam % gain, crit % gain, haste % gain
    * height, weight, size
    * body_parts, attack_parts, language,
    * spec1, spec2, spec_boss
    *
    *
    *
    */
  {
          "reserved", "reserved", "icon1", "icon2",
          0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0,
          0, 0, 0,
          "NULL",
          "NULL",
          "NULL"
  },

  {
          "goat", "farm_mammal", "icon1", "icon2",
          0, 1|2|4|8, 0,        /*resists, vulnerabilites, immunes*/
          20, 20, -10, -2,      /*hp %gain, dam % gain, crit % gain, haste % gain*/
          1, 30, 100,           /*height, weight, size*/
          0, 0, 3,            /*body_parts, attack_parts, language,*/
          "spec_fido",
          "NULL",
          "NULL"
  },

  {
          "fire_elemental", "elemental", "icon1", "icon2",
          0, 0, RES_FIRE | RES_POISON | RES_PARALYSIS
                   | RES_HOLD | RES_DRAIN | RES_NONMAGIC,     /*resists, vulnerabilites, immunes*/
          0, 0, 0, 20,          /*hp %gain, dam % gain, crit % gain, haste % gain*/
          2, 20, 3,             /*height, weight, size*/
          0, 0, 1,              /*body_parts, attack_parts, language,*/
          "spec_breath_fire",
          "NULL",
          "NULL"
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
 * Validate body-species names and creature-archetype relationships.
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
         * Validate the body-species table itself.
         */
        for (species = 0; species < MAX_SPECIES; species++)
        {
                if (!species_table[species].species
                ||  species_table[species].species[0] == '\0')
                {
                        sprintf(
                            buf,
                            "[MOB TEMPLATE] Body-species entry %d has no name.",
                            species);
                        log_string(buf);
                        issues++;
                        continue;
                }

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
        }

        /*
         * Every creature archetype must refer to an existing body species.
         */
        for (mob_type = 0; mob_type < MAX_MOB; mob_type++)
        {
                if (!mob_table[mob_type].name
                ||  mob_table[mob_type].name[0] == '\0')
                {
                        /*
                         * The existing mob resistance validator reports
                         * unnamed archetype entries.
                         */
                        continue;
                }

                if (!mob_table[mob_type].species
                ||  mob_table[mob_type].species[0] == '\0')
                {
                        sprintf(
                            buf,
                            "[MOB TEMPLATE] Creature archetype '%s' has "
                            "no body-species name.",
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

/*
 * Validate mob_table resistance, vulnerability and immunity masks.
 *
 * Runtime precedence remains:
 *
 *   immunity overrides everything;
 *   resistance plus vulnerability resolves to normal;
 *   otherwise the single matching state applies.
 */
int validate_mob_resistance_table(void)
{
        unsigned long int unknown_bits;
        unsigned long int overlap;
        char buf[MAX_STRING_LENGTH];
        int mob_type;
        int other;
        int issues;

        issues = 0;

        for (mob_type = 0; mob_type < MAX_MOB; mob_type++)
        {
                if (!mob_table[mob_type].name
                ||  mob_table[mob_type].name[0] == '\0')
                {
                        sprintf(buf,
                                "[MOB TYPE] entry %d has no name.",
                                mob_type);
                        log_string(buf);
                        issues++;
                        continue;
                }

                /*
                 * Duplicate names make mob_type_sn() resolution ambiguous.
                 */
                for (other = mob_type + 1; other < MAX_MOB; other++)
                {
                        if (!mob_table[other].name
                        ||  mob_table[other].name[0] == '\0')
                                continue;

                        if (!str_cmp(mob_table[mob_type].name,
                                     mob_table[other].name))
                        {
                                sprintf(buf,
                                        "[MOB TYPE] duplicate name '%s' at entries %d and %d.",
                                        mob_table[mob_type].name,
                                        mob_type,
                                        other);
                                log_string(buf);
                                issues++;
                        }
                }

                unknown_bits =
                    mob_table[mob_type].resists & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "resists",
                    unknown_bits);

                unknown_bits =
                    mob_table[mob_type].vulnerabilities & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "vulnerabilities",
                    unknown_bits);

                unknown_bits =
                    mob_table[mob_type].immunes & ~RES_VALID_MASK;
                issues += log_unknown_resistance_bits(
                    mob_table[mob_type].name,
                    "immunes",
                    unknown_bits);

                overlap =
                    (mob_table[mob_type].resists
                     & mob_table[mob_type].vulnerabilities)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "both resistant and vulnerable to",
                    overlap);

                overlap =
                    (mob_table[mob_type].resists
                     & mob_table[mob_type].immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "both resistant and immune to",
                    overlap);

                overlap =
                    (mob_table[mob_type].vulnerabilities
                     & mob_table[mob_type].immunes)
                    & RES_VALID_MASK;
                issues += log_resistance_conflicts(
                    mob_table[mob_type].name,
                    "both vulnerable and immune to",
                    overlap);
        }

        if (issues == 0)
        {
                log_string(
                    "[MOB TYPE] Resistance validation complete: no issues found.");
        }
        else
        {
                sprintf(buf,
                        "[MOB TYPE] Resistance validation complete: %d issue%s found.",
                        issues,
                        issues == 1 ? "" : "s");
                log_string(buf);
        }

        return issues;
}