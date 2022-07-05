/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  DD Code - Brutus (C) 2022                                              *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 * The kludgy global is for skills that want more stuff from command line.
*/
char *target_name;


void do_onstruct( CHAR_DATA *ch, char *argument )
{
        void           *vo;
        OBJ_DATA       *obj;
        CHAR_DATA      *rch;
        OBJ_INDEX_DATA *pObjIndex;
        char            arg1 [ MAX_INPUT_LENGTH ];
        char            arg2 [ MAX_INPUT_LENGTH ];
        char            buf [MAX_INPUT_LENGTH];
        int             sn;
        int             mats;
        char         buf [MAX_STRING_LENGTH];
        RAW_MATERIAL_DATA * material;


     
        if (IS_NPC(ch))
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
     
        if ( arg1[0] == '\0' )
        {
                send_to_char( "Syntax: create <object> <material>\n\r", ch );
                return;
        }

       if ( arg2[0] == '\0'  )
                {
                        send_to_char( "Syntax: create <object> <material>\n\r", ch );
                        return;
                }
        
        sn = skill_lookup(arg2);
        if (sn == -1)
        {
                send_to_char( "That skill doesn't exist.\n\r", ch );
                return;
        }
        /*
                Below code converts an int (e.g. 'sn') to a string and logs it.
                Which is sometimes useful.

                int length = snprintf( NULL, 0, "%d", sn );
                char* mystr = malloc( length + 1 );
                snprintf( mystr, length + 1, "%d", sn );
                log_string(mystr);

                -- Owl 23/09/18
        */

        if (!IS_NPC(ch) && !CAN_DO(ch, sn))
        {
                send_to_char("You fail miserably.\n\r", ch);
                return;
        }
 
        if ( ch->position < skill_table[sn].minimum_position )
        {
                send_to_char( "You can't concentrate enough.\n\r", ch );
                return;
        }

        /* mats calc */
        mats = 0;
        mats = mana_cost(ch, sn);

         /*
         * Locate targets.
         */
        obj     = NULL;
        vo      = NULL;

/*
        if ( ch-> < mana )
        {
                send_to_char( "You don't have enough mana.\n\r", ch );
                return;
        }

*/
          if ( arg1[0] == '\0'  )
                {
                        send_to_char( "What Metal should be used?\n\r", ch );
                        return;
                }
       /* SET OBJ to the METAL and set VO to that. */

 if( !str_prefix(arg1, "steel") )
        {
            
              /*     liq_table[obj->value[2]].liq_color); */
             /*   material = raw_mats_table[0]; */
                vo = (void *) arg1; 
        }

        (*skill_table[sn].spell_fun) (sn, ch->level, ch, arg2);
        /* successfully cast */
}


void construct_turret ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char            arg [MAX_STRING_LENGTH];
        OBJ_DATA        *creation;
        CHAR_DATA   *smelting_data = (CHAR_DATA *) vo;
        bool            in_sc_room;
        int             obj_spellcraft_bonus;
        int             mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_CREATE_FOOD + obj_spellcraft_bonus;

  /*      one_argument( target_name, arg);

        if ( !arg )
        {
                send_to_char( "What base material should be used? 35 units required.\n\r", ch );
                send_to_char( "Steel, Mithral, Adamantite, Electrum, Starmetal.\n\r", ch );
                return;
        }
*/
        if  ( ( !str_cmp( arg, "steel")  && ch->smelted_steel <35 ) )
      /*   ||
        (!str_cmp( arg, "mithral") && ch->smelted_mithral < 35) ||
        (!str_cmp( arg, "adamantite") && ch->smelted_adamantite < 35) ||
        (!str_cmp( arg, "electrum") && ch->smelted_electrum < 35) ||
        (!str_cmp( arg, "starmetal") && ch->smelted_starmetal < 35) ) */
        {
              send_to_char( "Not enough raw materials - 35 steel required.\n\r", ch );
                return;  
        }
        
        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_sc_room = TRUE;
        }

        if ( in_sc_room )
                send_to_char( "{MYou summon a turret in teh form of a mushroom!{x\n\r", ch);


        creation = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
        creation->value[0] = (in_sc_room) ? 5 + ( level * mod_room_bonus ) / 100 : 5 + level;
        obj_to_room( creation, ch->in_room );
 
        act( "Behold your $p is formed.", ch, creation, NULL, TO_CHAR );
        act( "$n creates a $p.", ch, creation, NULL, TO_ROOM );
        return;
}

