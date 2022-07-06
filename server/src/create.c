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

void do_construct( CHAR_DATA *ch, char *arg )
{
        OBJ_DATA        *creation;
        int             found, i;
        AFFECT_DATA     af;
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA        *obj;

        if (IS_NPC(ch))
                return;

        if (ch->class != CLASS_SMITHY)
        {
                send_to_char("You are unble to build anything you clumst oath.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You can't construct while fighting.\n\r", ch);
                return;
        }

        if( arg[0] == '\0' )
        {
               
                send_to_char("What do you want to construct?\n\r", ch);
                return;
        }

        found = -1;
        for (i = 0; i < BLUEPRINTS_MAX; i++)
        {
                if (is_name(arg, blueprint_list[i].blueprint_name))
                {
                        found = i;
                        break;
                }
        }

        if (found == -1)
        {
                send_to_char("You are studing the following blueprints:\n\r", ch);
                for (i = 0; i < BLUEPRINTS_MAX; i++)
                {
                        if( ch->pcdata->learned[skill_lookup(blueprint_list[i].blueprint_name)] > 0)
                        {
                                sprintf(buf, "%s: %d percent.", blueprint_list[i].blueprint_name,ch->pcdata->learned[skill_lookup(blueprint_list[i].blueprint_name)] );
                                act(buf, ch, NULL, NULL, TO_CHAR);
                        }
                        
                }
                return;
        }

        if( !ch->pcdata->learned[skill_lookup(blueprint_list[i].blueprint_name)] )
        {
                send_to_char( "You don't know how to construct that.\n\r", ch );
                return;
        }

        if ( blueprint_list[i].blueprint_cost[0] > ch->smelted_steel 
                ||  blueprint_list[i].blueprint_cost[1] > ch->smelted_mithral 
                ||  blueprint_list[i].blueprint_cost[2] > ch->smelted_adamantite
                ||  blueprint_list[i].blueprint_cost[3] > ch->smelted_electrum
                ||  blueprint_list[i].blueprint_cost[4] > ch->smelted_starmetal)
        {
                sprintf(buf, "Blueprint cost: %d Steel %d Mithral %d Adamantite %d Electrum %d Starmetal", 
                blueprint_list[i].blueprint_cost[0], 
                blueprint_list[i].blueprint_cost[1],
                blueprint_list[i].blueprint_cost[2], 
                blueprint_list[i].blueprint_cost[3], 
                blueprint_list[i].blueprint_cost[4]);
                act(buf, ch, NULL, NULL, TO_CHAR);
                return; 
        }

        creation = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
        obj_to_room( creation, ch->in_room );

        sprintf(buf, "You start to construct '%s'", blueprint_list[i].blueprint_desc);
        act(buf, ch, NULL, NULL, TO_CHAR);

        sprintf(buf, "$n constructs a '%s'.", blueprint_list[i].blueprint_desc);
        act(buf, ch, NULL, NULL, TO_ROOM);

        return;
}


void construct_turret ( int sn, int level, CHAR_DATA *ch, void *vo )
{


        send_to_char( "Howd you get here....\n\r", ch );
        return;

}

