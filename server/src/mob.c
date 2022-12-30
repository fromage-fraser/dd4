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
        { "none",       1,      "{WCommon.{x "},
        { "common",     1,      "{WCommon.{x "},
        { "rare",       2,      "<39>[Rare]<0> "},
        { "elite",      3,      "<93>[Elite]<0> "},
        { "boss",       5,      "<514><556><16>[<560>BOSS<561>]<0><557> "},
        { "world",      30,     "<81>[WO<75>RL<69>D B<75>OS<81>S]<0> "}
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
    * name, species, male_icon, femail_icon
    * resists, vulnerabilites, immunes,
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
          16|32|64, 2, 1,       /*resists, vulnerabilites, immunes*/
          0, 0, 0, 20,          /*hp %gain, dam % gain, crit % gain, haste % gain*/
          2, 20, 3,             /*height, weight, size*/
          0, 0, 1,              /*body_parts, attack_parts, language,*/
          "spec_breath_fire", 
          "NULL", 
          "NULL"
  }

};
  