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




/*
 *  Skill table
 */
const struct mob_type mob_table [MAX_MOB] =
{
  /*
    * name, pointer to gsn
    * practice type, target, minimum position,
    * pointer to spell function, minimum mana, beats
    * damage noun, wear off message
    *
    * When adding new spells add them near the end, just
    * before the base skills.  Elements are accessed by
    * index and results will be unpredictable (BUT BAD)
    * if the current order gets screwed up.
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
          "goat", "mammal", "icon1", "icon2",
          1, 1, 1, 
          20, 20, -10, -2, 
          1, 30, 100,
          22, 12, 3,
          "spec_warrior", 
          "NULL", 
          "spec_warrior"
  },

  {
          "fire_elemental", "elemental", "icon1", "icon2",
          1, 1, 1, 
          20, 100, 2, 20, 
          1, 30, 100,
          22, 12, 3,
          "spec_default", 
          "NULL", 
          "spec_default"
  }

};
  