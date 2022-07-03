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
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
        int sn;

        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
                if ( !skill_table[sn].name )
                        break;

                if ( LOWER( name[0] ) == LOWER( skill_table[sn].name[0] )
                    && !str_prefix( name, skill_table[sn].name ) )
                        return sn;
        }

        return -1;
}

/*
 * The kludgy global is for skills that want more stuff from command line.

char *target_name; */
char *mat_base;


void do_create( CHAR_DATA *ch, char *argument )
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
     
        if (IS_NPC(ch))
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
     
        if ( arg1[0] == '\0' || !is_number( arg1 ) )
        {
                send_to_char( "Syntax: create <object> (optional: material).\n\r", ch );
                return;
        }

        sn = skill_lookup(arg1);
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

        switch ( skill_table[sn].target )
        {
            default:
                bug( "Do_cast: bad target for sn %d.", sn );
                return;

            case TAR_IGNORE:
                break;

            case TAR_OBJ_INV:
                if ( arg2[0] == '\0' )
                {
                        send_to_char( "What should the spell be cast upon?\n\r", ch );
                        return;
                }

                if ( !( obj = get_obj_carry( ch, arg2 ) ) )
                {
                        send_to_char( "You are not carrying that.\n\r", ch );
                        return;
                }

                vo = (void *) obj;
                break;
        }

/*
        if ( ch-> < mana )
        {
                send_to_char( "You don't have enough mana.\n\r", ch );
                return;
        }

*/
 
        WAIT_STATE( ch, skill_table[sn].beats );

        if ( !IS_NPC(ch)
            && ch->level <= LEVEL_HERO
            && ((sn = skill_lookup(arg1)) >= 0)
            && ch->pcdata->learned[sn] > 0)
        {
                send_to_char("You fail to correctly recite the spell!\n\r", ch);

               /* ch->mana -= mana / 2; */
        }

        /* successfully cast */
   

}



void create_turret ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *mushroom;
        bool in_sc_room;
        int obj_spellcraft_bonus;
        int mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_CREATE_FOOD + obj_spellcraft_bonus;


        if (IS_SET( ch->in_room->room_flags, ROOM_SPELLCRAFT ))
        {
             in_sc_room = TRUE;
        }

        if ( in_sc_room )
                send_to_char( "{MYou summon a turret in teh form of a mushroom!{x\n\r", ch);


        mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
        mushroom->value[0] = (in_sc_room) ? 5 + ( level * mod_room_bonus ) / 100 : 5 + level;
        obj_to_room( mushroom, ch->in_room );

        act( "$p a secret turret appears.", ch, mushroom, NULL, TO_CHAR );
        act( "$p a secret turret appears.", ch, mushroom, NULL, TO_ROOM );
        return;
}

