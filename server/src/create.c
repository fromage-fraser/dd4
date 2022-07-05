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

void do_construct( CHAR_DATA *ch, char *argument )
{
        void           *vo;
        char            arg [ MAX_INPUT_LENGTH ];
        int             sn;
        char            buf[MAX_STRING_LENGTH];
        
        if (IS_NPC(ch))
                return;

        argument = one_argument( argument, arg );
       
        if ( arg[0] == '\0' )
        {
                send_to_char( "Syntax: Construct <object> <material>\n\r", ch );
                return;
        }
 
        sn = skill_lookup(arg);



        if (sn == -1)
        {
                send_to_char( "That skill doesn't exist.\n\r", ch );
                return;
        }

   /*    if (sn)
        {
                sprintf( buf, "Spells cast per round is set at %d. and %s\n\r", sn, arg );
                send_to_char ( buf, ch );
                return;
        }
*/

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

        sprintf( buf, "Spells cast per round is set at %d. and %s\n\r", sn, arg );
        send_to_char ( buf, ch );

        vo      = NULL;
        (*skill_table[sn].spell_fun) (sn, ch->level, ch, vo);
        /* successfully cast */
}


void construct_turret ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA        *creation;
        CHAR_DATA       *smelting_data = (CHAR_DATA *) vo;
        bool            in_sc_room;
        int             obj_spellcraft_bonus;
        int             mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_CREATE_FOOD + obj_spellcraft_bonus;
        
    /*    if ( blueprint_list[0].blue_print_cost[0] > smelting_data->smelted_steel 
                ||  blueprint_list[0].blue_print_cost[1] > smelting_data->smelted_mithral 
                ||  blueprint_list[0].blue_print_cost[2] > smelting_data->smelted_adamantite
                ||  blueprint_list[0].blue_print_cost[3] > smelting_data->smelted_electrum
                ||  blueprint_list[0].blue_print_cost[4] > smelting_data->smelted_starmetal)
        {
                send_to_char( "Not enough raw materials - 35 steel required.\n\r", ch );
                return; 
        }

    */

        creation = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
        creation->value[0] = (in_sc_room) ? 5 + ( level * mod_room_bonus ) / 100 : 5 + level;
        obj_to_room( creation, ch->in_room );
 
        act( "Behold your $p is formed.", ch, creation, NULL, TO_CHAR );
        act( "$n creates a $p.", ch, creation, NULL, TO_ROOM );
        return;
}

