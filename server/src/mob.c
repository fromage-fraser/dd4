/***************************************************************************
 *  mob.c
 *
 *  Mapping system for Dragons Domain III
 *
 *  
 *
 * 
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


/*#define RES_FIRE		  BIT_0
#define RES_COLD		  BIT_1
#define RES_ELECTRICITY		  BIT_2
#define RES_ENERGY		  BIT_3
#define RES_BLUNT		  BIT_4
#define RES_PIERCE		  BIT_5
#define RES_SLASH		  BIT_6
#define RES_ACID		  BIT_7
#define RES_POISON		  BIT_8
#define RES_DRAIN		  BIT_9
#define RES_SLEEP		  BIT_10
#define RES_CHARM		  BIT_11
#define RES_HOLD		  BIT_12
#define RES_NONMAGIC		  BIT_13
#define RES_MAGIC		  BIT_14
#define RES_PARALYSIS		  BIT_15
*/


const struct species_type  species_table [ MAX_SPECIES ] =
{
        /* species, body parts, attack parts */
      
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
  