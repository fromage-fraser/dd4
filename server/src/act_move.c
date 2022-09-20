/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


DIR_DATA directions [MAX_DIR] =
{
        /* name,   reverse,   bamfin */
        { "north", DIR_SOUTH, "the north" },
        { "east",  DIR_WEST,  "the east"  },
        { "south", DIR_NORTH, "the south" },
        { "west",  DIR_EAST,  "the west"  },
        { "up",    DIR_DOWN,  "above"     },
        { "down",  DIR_UP,    "below"     }
};


const int movement_loss [SECT_MAX] =
{
        1, 2, 2, 3, 4, 5, 4, 1, 3, 10, 6, 4
};


/*
 * Local functions.
 */
int     find_door       args((CHAR_DATA *ch, char *arg));
bool    has_key         args((CHAR_DATA *ch, int key));
int     find_wall       args((CHAR_DATA *ch, char *arg));
void    scan            args((CHAR_DATA *ch, int door));


/*
 * Scan command.
 * (by Turtle 12-Jun-94)
 */
void do_scan (CHAR_DATA *ch, char *argument)
{
        int dir;
        bool found;
        EXIT_DATA *pexit;

        char * const dir_desc [] =
        {
                "to the north",
                "to the east",
                "to the south",
                "to the west",
                "upwards",
                "downwards"
        };

        if (!check_blind(ch))
                return;

        if (argument[0] == '\0')
        {
                act ("$n scans intently all around.", ch, NULL, NULL, TO_ROOM);

                found = FALSE;

                for (dir = 0; dir <= 5; dir++)
                {
                        if ((pexit = ch->in_room->exit[dir])
                            && pexit->to_room)
                        {
                                if (IS_SET(pexit->exit_info, EX_SECRET))
                                        continue;

                                /* Can't scan through walls, Clark Kent. --Owl 27/8/22 */
                                if (IS_SET(pexit->exit_info, EX_WALL))
                                        continue;

                                scan(ch, dir);
                                found = TRUE;
                        }
                }

                if(!found)
                        send_to_char("There are no exits here!\n\r", ch);
        }
        else
        {
                if(!str_prefix(argument, "north"))      dir = DIR_NORTH;
                else if(!str_prefix(argument, "east"))  dir = DIR_EAST;
                else if(!str_prefix(argument, "south")) dir = DIR_SOUTH;
                else if(!str_prefix(argument, "west"))  dir = DIR_WEST;
                else if(!str_prefix(argument, "up"))    dir = DIR_UP;
                else if(!str_prefix(argument, "down"))  dir = DIR_DOWN;

                else
                {
                        send_to_char("That's not a direction!\n\r", ch);
                        return;
                }

                act ("$n scans intensely $T.", ch, NULL, dir_desc[dir], TO_ROOM);

                if (!ch->in_room->exit[dir])
                {
                        send_to_char("There is no exit in that direction!\n\r", ch);
                        return;
                }

                scan(ch, dir);
        }
}



/*
 * Nagilum's scan function and related constants.
 * (contributed by VampLestat 17-Feb-94)
 * (modified by Turtle 12-Jun-94)
 */

char * const dist_name [] =
{
        "right here",
        "one room",
        "two rooms",
        "three rooms",
        "four rooms",
        "five rooms",
        "six rooms"
};

char * const dir_desc [] =
{
        "to the north",
        "to the east",
        "to the south",
        "to the west",
        "upwards",
        "downwards"
};


void scan (CHAR_DATA *ch, int door)
{
        char buf[MAX_STRING_LENGTH];
        char buf2[MAX_STRING_LENGTH];
        int distance, visibility;
        bool found;
        ROOM_INDEX_DATA *was_in_room;

        visibility = 6;

        if(!IS_SET(ch->act, PLR_HOLYLIGHT))
        {
                switch(weather_info.sunlight)
                {
                    case SUN_SET:   visibility = 4; break;
                    case SUN_DARK:
                        if (IS_AFFECTED(ch, AFF_INFRARED))
                                visibility = 4;
                        else
                                visibility = 2;
                        break;
                    case SUN_RISE:  visibility = 4; break;
                    case SUN_LIGHT: visibility = 6; break;
                }
                switch(weather_info.sky)
                {
                    case SKY_CLOUDLESS: break;
                    case SKY_CLOUDY:    visibility -= 1; break;
                    case SKY_RAINING:   visibility -= 2; break;
                    case SKY_LIGHTNING: visibility -= 3; break;
                }
        }

        was_in_room = ch->in_room;
        found = FALSE;

        for (distance = 1; distance <= 6; distance++)
        {
                EXIT_DATA *pexit;
                CHAR_DATA *list;
                CHAR_DATA *rch;

                if((pexit = ch->in_room->exit[door]) != NULL
                   && pexit->to_room != NULL
                   && pexit->to_room != was_in_room)
                {
                        if (IS_SET( pexit->exit_info, EX_SECRET ))
                                break;

                        /* Can't scan through walls --Owl 27/8/22 */
                        if (IS_SET( pexit->exit_info, EX_WALL ))
                                break;

                        if (IS_SET(pexit->exit_info, EX_CLOSED))
                        {
                                char door_name[80];
                                one_argument(pexit->keyword, door_name);
                                if(door_name[0] == '\0')
                                        strcat(door_name, "door");

                                sprintf(buf, "{CA closed %s{x %s %s.\n\r",
                                        door_name, dist_name[distance-1], dir_desc[door]);
                                send_to_char(buf, ch);
                                found = TRUE;
                                break;
                        }

                        ch->in_room = pexit->to_room;
                        if(IS_OUTSIDE(ch) ? distance > visibility : distance > 4)
                                break;

                        list = ch->in_room->people;
                        for(rch = list; rch; rch = rch->next_in_room)
                        {
                                if (can_see(ch, rch) && !IS_AFFECTED(rch, AFF_HIDE))
                                {
                                        found = TRUE;

                                        if (is_affected(rch,gsn_mist_walk))
                                        {
                                                sprintf(buf2, "[%s] ", rch->name);
                                                sprintf(buf, "{W%sA glowing mist{x who is %s %s.\n\r",
                                                        CAN_SEE_MIST(ch) ? buf2 : "",
                                                        dist_name[distance],
                                                        dir_desc[door]);
                                        }
                                        else if (rch->level <= 100)
                                        {
                                                sprintf(buf, "%s%s{x%s who is %s %s.\n\r",
                                                        IS_NPC(rch) ? "{Y" : "{W",
                                                        capitalize_initial(PERS(rch, ch)),
                                                        IS_NPC(rch) ? "" : " {g({GPLAYER{x{g){x",
                                                        dist_name[distance],
                                                        dir_desc[door]);
                                        }
                                        else
                                        {
                                                sprintf(buf, "%s%s{x%s who is %s %s.\n\r",
                                                        IS_NPC(rch) ? "{Y" : "{W",
                                                        capitalize_initial(PERS(rch, ch)),
                                                        IS_NPC(rch) ? "" : " {c({CIMM{x{c){x",
                                                        dist_name[distance],
                                                        dir_desc[door]);
                                        }

                                        buf[0] = UPPER(buf[0]);
                                        send_to_char(buf, ch);
                                }
                        }
                }
        }

        ch->in_room = was_in_room;

        if(!found)
        {
                sprintf(buf, "{cYou can't see anything %s.{x\n\r",
                        dir_desc[door]);
                send_to_char(buf, ch);
        }

        return;
}



/* Modified 23/10/99 to reduce branches and include mapping funcs - Tavolir */

void move_char(CHAR_DATA *ch, int door)
{
        CHAR_DATA       *fch;
        CHAR_DATA       *fch_next;
        EXIT_DATA       *pexit;
        ROOM_INDEX_DATA *in_room;
        ROOM_INDEX_DATA *to_room;
        char             buf [ MAX_STRING_LENGTH ];

        if (door < 0 || door > 5)
        {
                bug("Do_move: bad door %d.", door);
                return;
        }

        in_room = ch->in_room;

        if (!(pexit = in_room->exit[door]) || !(to_room = pexit->to_room))
        {
                send_to_char("Alas, you cannot go that way.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HOLD))
        {
                send_to_char("You cannot move!\n\r", ch);
                return;
        }

        /*  Let Immortals pass through all doors  */
        if (IS_SET(pexit->exit_info, EX_CLOSED) && ch->level <= LEVEL_HERO)
        {
                if (!IS_AFFECTED(ch, AFF_PASS_DOOR)
                    && !IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                {
                        act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
                        return;
                }

                if (IS_SET(pexit->exit_info, EX_PASSPROOF))
                {
                        act ("You are unable to pass through the $d.",
                             ch, NULL, pexit->keyword, TO_CHAR);
                        return;
                }

                /* when mounts can be translucent, this will need to change */
                if (IS_AFFECTED(ch, AFF_PASS_DOOR) && ch->mount)
                {
                        act("You are unable to pass through the $d while mounted.",ch,NULL,pexit->keyword, TO_CHAR);
                        return;
                }
        }

        if (IS_SET(pexit->exit_info, EX_WALL)
            && !IS_AFFECTED(ch, AFF_FLYING))
        {
                act ("You cannot proceed, as a $d blocks your way.",
                     ch, NULL, pexit->keyword, TO_CHAR);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM)
            && ch->master
            && in_room == ch->master->in_room)
        {
                send_to_char("What?  And leave your beloved master?\n\r", ch);
                return;
        }

        /*
        if (to_room->vnum == TOURNAMENT_ARENA_ENTRANCE_VNUM
            && tournament_status != TOURNAMENT_STATUS_NONE)
        {
                send_to_char("You can't go there while a tournament has been posted.\n\r", ch);
                return;
        }
        */

        if (room_is_private(to_room))
        {
                send_to_char("That room is private right now.\n\r", ch);
                return;
        }

        if ( to_room->sector_type == SECT_UNDERWATER
          && is_affected(ch, gsn_mist_walk)
          && ( ( ch->race != RACE_SAHUAGIN )
            && ( ch->race != RACE_GRUNG ) ) )
        {
                send_to_char("You may not dive underwater in mist form.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch))
        {
                int move;

                /*
                int iClass;

                for (iClass = 0; iClass < MAX_CLASS; iClass++)
                {
                        if (iClass != ch->class
                            && to_room->vnum == class_table[iClass].guild)
                        {
                                send_to_char("You aren't allowed in there.\n\r", ch);
                                return;
                        }
                }
                */

                /* Immortals can fly, even when they can't -- Owl 22/2/22 */

                if (to_room->sector_type == SECT_AIR)
                {
                        if (!IS_AFFECTED(ch, AFF_FLYING)
                            && !IS_AFFECTED(ch, AFF_NON_CORPOREAL)
                            && !ch->mount
                            && !IS_IMMORTAL( ch ))
                        {
                                send_to_char("You can't fly.\n\r", ch);
                                return;
                        }
                }

                else if (to_room->sector_type == SECT_WATER_SWIM)
                {
                        OBJ_DATA *obj;
                        bool      found;

                        found = FALSE;

                        /*
                         * You can move into a SWIM sector from an UNDERWATER one... rather than drown.
                         * You can also move into one from any non-water room so you get a chance to turn
                         * your swim skill ON.
                         */

                        if ( IS_AFFECTED(ch, AFF_FLYING)
                            || IS_IMMORTAL( ch )
                            || ( ch->race == RACE_SAHUAGIN )
                            || ( ch->race == RACE_GRUNG )
                            || ch->mount
                            || IS_AFFECTED( ch, AFF_SWIM )
                            || ( ( ch->in_room->sector_type != SECT_UNDERWATER )
                              && ( ch->in_room->sector_type != SECT_WATER_SWIM )
                              && ( ch->in_room->sector_type != SECT_WATER_NOSWIM ) )
                            || is_affected(ch,gsn_mist_walk)
                            || is_affected(ch,gsn_astral_sidestep)
                            || ch->form == FORM_FLY)
                        {
                                found = TRUE;
                        }
                        else
                        {
                                for (obj = ch->carrying; obj; obj = obj->next_content)
                                {
                                        if (obj->item_type == ITEM_BOAT)
                                        {
                                                found = TRUE;
                                                break;
                                        }
                                }
                        }

                        if (!found)
                        {
                                send_to_char("You need to be able to swim, fly, or have a boat to go there.\n\r", ch);
                                return;
                        }
                }
                else if (to_room->sector_type == SECT_WATER_NOSWIM)
                {
                        OBJ_DATA *obj;
                        bool      found;

                        found = FALSE;

                        /* Immortals are boats -- Owl 22/2/22 */
                        /* You can move into a NOSWIM sector from an UNDERWATER one... rather than drown */

                        if (IS_AFFECTED(ch, AFF_FLYING)
                            || IS_IMMORTAL( ch )
                            || ( ch->race == RACE_SAHUAGIN )
                            || ( ch->race == RACE_GRUNG )
                            || ch->mount
                            || ( ch->in_room->sector_type == SECT_UNDERWATER )
                            || is_affected(ch,gsn_mist_walk)
                            || is_affected(ch,gsn_astral_sidestep)
                            || ch->form == FORM_FLY)
                        {
                                found = TRUE;
                        }
                        else
                        {
                                for (obj = ch->carrying; obj; obj = obj->next_content)
                                {
                                        if (obj->item_type == ITEM_BOAT)
                                        {
                                                found = TRUE;
                                                break;
                                        }
                                }
                        }

                        if (!found)
                        {
                                send_to_char("You need to be flying or have a boat to go there.\n\r", ch);
                                return;
                        }
                }

                /*
                 * Prevent access to some areas
                 */

                if ((ch->level < to_room->area->low_enforced
                     || ch->level > to_room->area->high_enforced)
                    && ch->level <= LEVEL_HERO)
                {
                        send_to_char("The Gods prevent you from proceeding there.\n\r", ch);
                        return;
                }

                if (to_room->area->low_level == -4
                    && to_room->area->high_level == -4
                    && !IS_NPC(ch)
                    && !ch->clan
                    && !IS_SET(ch->status, PLR_RONIN)
                    && ch->level <= LEVEL_HERO)
                {
                        send_to_char("The Gods prevent you from proceeding for your own safety.\n\r", ch);
                        return;
                }

                /*
                 *  Mounts
                 */
                if (ch->mount)
                {
                        OBJ_DATA *obj;
                        bool found_boat;
                        found_boat = FALSE;

                        if (ch->mount->position < POS_STANDING)
                        {
                                send_to_char ("Your mount isn't awake!\n\r", ch);
                                return;
                        }

                        if (IS_AFFECTED(ch->mount, AFF_HOLD))
                        {
                                send_to_char ("Your mount is unable to move.\n\r", ch);
                                return;
                        }

                        if (IS_SET(to_room->room_flags, ROOM_SOLITARY)
                        ||  IS_SET(to_room->room_flags, ROOM_PRIVATE)
                        ||  IS_SET(to_room->room_flags, ROOM_NO_MOUNT))
                        {
                                send_to_char ("You can't go there while mounted.\n\r", ch);
                                return;
                        }

                        if ( to_room->sector_type == SECT_WATER_NOSWIM
                        &&   !IS_AFFECTED( ch->mount, AFF_FLYING ) )
                        {
                                found_boat = FALSE;

                                for (obj = ch->carrying; obj; obj = obj->next_content)
                                {
                                        if (obj->item_type == ITEM_BOAT)
                                        {
                                                found_boat = TRUE;
                                                break;
                                        }
                                }

                                if (!found_boat)
                                {
                                        send_to_char ("Your mount refuses to enter the water.\n\r", ch);
                                        return;
                                }
                        }

                        if ( to_room->sector_type == SECT_WATER_SWIM
                        && ( ( !IS_AFFECTED( ch->mount, AFF_FLYING ) )
                          && ( !IS_AFFECTED( ch->mount, AFF_SWIM ) ) ) )
                        {
                                found_boat = FALSE;

                                for (obj = ch->carrying; obj; obj = obj->next_content)
                                {
                                        if (obj->item_type == ITEM_BOAT)
                                        {
                                                found_boat = TRUE;
                                                break;
                                        }
                                }

                                if (!found_boat)
                                {
                                        send_to_char ("Your mount refuses to enter the water.\n\r", ch);
                                        return;
                                }
                        }

                        if (to_room->sector_type == SECT_AIR
                            && !IS_AFFECTED(ch->mount, AFF_FLYING))
                        {
                                send_to_char ("Your mount can't fly!\n\r", ch);
                                return;
                        }
                }


                if (checkmovetrap(ch, door))
                        return;

                move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
                        + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)];


                if ((IS_AFFECTED(ch, AFF_NON_CORPOREAL)
                     || IS_AFFECTED(ch, AFF_FLYING))
                    && (to_room->sector_type != SECT_UNDERWATER
                        || ch->form == FORM_SNAKE))
                {
                        move /= 3;
                        move = UMAX(move, 1);
                }

                if ( ( ( to_room->sector_type == SECT_WATER_NOSWIM )
                ||     ( to_room->sector_type == SECT_WATER_SWIM )
                ||     ( to_room->sector_type == SECT_SWAMP )
                ||     ( to_room->sector_type == SECT_UNDERWATER ) )
                && ( ( ch->race == RACE_SAHUAGIN )
                  || ( ch->race == RACE_GRUNG )
                  || ( IS_AFFECTED(ch, AFF_SWIM) ) ) )
                {
                        move /= 3;
                        move = UMAX(move, 1);
                }

                /* After everything above, triple your movement cost if you're affected by AFF_SLOW */

                if (IS_AFFECTED(ch, AFF_SLOW))
                {
                        move *= 3;
                        move = UMAX(move, 1);
                }

                /* Don't put a wait state on or take movement points from imms -- Owl 22/2/22 */

                if (ch->move < move
                        && !IS_IMMORTAL( ch ))
                {
                        send_to_char("You are too exhausted.\n\r", ch);
                        return;
                }

                if (!IS_IMMORTAL( ch ))
                {
                        WAIT_STATE(ch, 1);

                        if (is_affected(ch, gsn_mount))
                                ch->move -= 1;
                        else
                                ch->move -= move;
                }
        } /* end PC checks */

        if (ch->in_room->area != to_room->area
            && ch->level < to_room->area->low_level)
        {
                sprintf(buf, "{GProceed with caution, you are entering %s!{x\n\r",
                        to_room->area->name);
                send_to_char(buf, ch);
        }

        /* for mist walk - Istari */
        if (is_affected(ch,gsn_mist_walk))
        {
                act_move ("A glowing mist drifts $T.",ch,NULL,directions[door].name,TO_ROOM);
        }
        else
        {
                if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
                {
                        if (ch->mount)
                        {
                                act_move ("$n rides $T.", ch, NULL, directions[door].name, TO_ROOM);
                        }
                        else if ( ( ( to_room->sector_type == SECT_WATER_NOSWIM )
                             ||     ( to_room->sector_type == SECT_WATER_SWIM )
                             ||     ( to_room->sector_type == SECT_UNDERWATER ) )
                             &&   (  ( !IS_AFFECTED(ch, AFF_FLYING) )
                                  || ( ch->race == RACE_SAHUAGIN )
                                  || ( ch->race == RACE_GRUNG ) ) )
                        {
                                act_move ("$n swims $T.", ch, NULL, directions[door].name, TO_ROOM);
                        }
                        else {
                                act_move ("$n leaves $T.", ch, NULL, directions[door].name, TO_ROOM);
                        }
                }
        }

        char_from_room(ch);
        char_to_room(ch, to_room);
        do_look(ch, "auto");

        if (is_affected(ch,gsn_mist_walk))
        {
                act_move ("A strange mist stirs up around you.",ch,NULL,NULL,TO_ROOM);
        }
        else if (ch->mount && !IS_NPC(ch) && !IS_SET(ch->act, PLR_WIZINVIS))
        {
                act_move ("$n rides in on $N.", ch, NULL, ch->mount, TO_ROOM);
        }
        else if ( ( ( to_room->sector_type == SECT_WATER_NOSWIM )
                ||  ( to_room->sector_type == SECT_WATER_SWIM )
                ||  ( to_room->sector_type == SECT_UNDERWATER ) )
                &&  ( !IS_AFFECTED(ch, AFF_FLYING)
                   || ch->race == RACE_SAHUAGIN
                   || ch->race == RACE_GRUNG ) )
        {
                act_move ("$n swims in.", ch, NULL, NULL, TO_ROOM);
        }
        else if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
        {
                act_move ("$n has arrived.", ch, NULL, NULL, TO_ROOM);
        }


        for (fch = in_room->people; fch; fch = fch_next)
        {
                fch_next = fch->next_in_room;

                if (fch->deleted)
                        continue;

                if (fch->rider == ch
                    && fch->position == POS_STANDING
                    && ch->in_room != fch->in_room)
                        move_char (fch, door);

                else if (fch->master == ch
                         && fch->position == POS_STANDING
                         && ch->in_room != fch->in_room)
                {
                        act ("You follow $N.\n\r", fch, NULL, ch, TO_CHAR);
                        move_char(fch, door);
                }
        }

        if (ch->mount && ch->in_room != ch->mount->in_room)
        {
                send_to_char ("\n\rYou seem to have lost your mount!\n\r", ch);
                sprintf (buf, "act_move: %s's mount missing after move; stripping effects",
                         ch->name );
                bug (buf, 0);
                strip_mount (ch);
                ch->position = POS_STANDING;
        }

         /* Strip swim if room we move to isn't wet.. AFTER moving. Imms can dispel themselves. */

        if ( ( ( ( IS_AFFECTED( ch, AFF_SWIM ) )
        ||       ( is_affected( ch, gsn_swim ) ) )
        &&       ( ch->form != FORM_SNAKE )
        &&       ( ch->level <= LEVEL_HERO ) )
            && ( ch->in_room->sector_type != SECT_UNDERWATER )
            && ( ch->in_room->sector_type != SECT_WATER_SWIM )
            && ( ch->in_room->sector_type != SECT_SWAMP )
            && ( ch->in_room->sector_type != SECT_WATER_NOSWIM ) )
        {
                affect_strip(ch, gsn_swim);
                REMOVE_BIT(ch->affected_by, AFF_SWIM);
                send_to_char("{cNo longer in the water, you stop swimming.{x\n\r", ch);
        }

        /* Lets check we're a pc BEFORE we call the trigger */
        /* CPU utilisation -= 50% */
        if (!IS_NPC(ch))
        {
                mprog_entry_trigger(ch);        /* for mob programs - Brutus */
                mprog_greet_trigger(ch);        /* for mob programs - Brutus */
        }
}


void do_north(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_NORTH);
}


void do_east(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_EAST);
}


void do_south(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_SOUTH);
}


void do_west(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_WEST);
}


void do_up(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_UP);
}


void do_down(CHAR_DATA *ch, char *argument)
{
    move_char(ch, DIR_DOWN);
}


int find_door(CHAR_DATA *ch, char *arg)
{
        EXIT_DATA *pexit;
        int        door;

        if (!str_prefix(arg, "north")) door = 0;
        else if (!str_prefix(arg, "east")) door = 1;
        else if (!str_prefix(arg, "south")) door = 2;
        else if (!str_prefix(arg, "west")) door = 3;
        else if (!str_prefix(arg, "up")) door = 4;
        else if (!str_prefix(arg, "down")) door = 5;
        else
        {
                for (door = 0; door <= 5; door++)
                {
                        if ((pexit = ch->in_room->exit[door])
                            && IS_SET(pexit->exit_info, EX_ISDOOR)
                            && pexit->keyword
                            && is_name(arg, pexit->keyword))
                                return door;
                }
                act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
                return -1;
        }

        if (!(pexit = ch->in_room->exit[door]))
        {
                act ("I see no door $T here.", ch, NULL, arg, TO_CHAR);
                return -1;
        }

        if (!IS_SET(pexit->exit_info, EX_ISDOOR))
        {
                send_to_char("You can't do that.\n\r", ch);
                return -1;
        }

        return door;
}


void do_open(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        int       door;

        one_argument(argument, arg);

        if(IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot do that in your current form.\n\r",ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Open what?\n\r", ch);
                return;
        }

        if ((obj = get_obj_here(ch, arg)))
        {
                if (obj->item_type != ITEM_CONTAINER)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSED))
                {
                        send_to_char("It's already open.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
                {
                        send_to_char("You can't do that.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->value[1], CONT_LOCKED))
                {
                        send_to_char("It's locked.\n\r", ch);
                        return;
                }

                /* for traps */
                if (checkopen(ch, obj))
                        return;

                REMOVE_BIT(obj->value[1], CONT_CLOSED);
                send_to_char("Ok.\n\r", ch);
                act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
                return;
        }

        if ((door = find_door(ch, arg)) >= 0)
        {
                /* 'open door' */
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;

                pexit = ch->in_room->exit[door];

                if (!IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("It's already open.\n\r", ch);
                        return;
                }

                if (IS_SET(pexit->exit_info, EX_LOCKED))
                {
                        send_to_char("It's locked.\n\r", ch);
                        return;
                }

                REMOVE_BIT(pexit->exit_info, EX_CLOSED);
                act ("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);
                send_to_char("Ok.\n\r", ch);

                /* open the other side */
                if ((to_room = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[door].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        CHAR_DATA *rch;

                        REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
                        for (rch = to_room->people; rch; rch = rch->next_in_room)
                        {
                                if (rch->deleted)
                                        continue;
                                act ("The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR);
                        }
                }
        }

        return;
}


int find_wall(CHAR_DATA *ch, char *arg)
{
        EXIT_DATA *pexit;
        int wall;

        if (!str_prefix(arg, "north")) wall = 0;
        else if (!str_prefix(arg, "east")) wall = 1;
        else if (!str_prefix(arg, "south")) wall = 2;
        else if (!str_prefix(arg, "west")) wall = 3;
        else if (!str_prefix(arg, "up")) wall = 4;
        else if (!str_prefix(arg, "down")) wall = 5;
        else
        {
                for (wall = 0; wall <= 5; wall++)
                {
                        if ((pexit = ch->in_room->exit[wall])
                            && IS_SET(pexit->exit_info, EX_WALL)
                            && pexit->keyword
                            && is_name(arg, pexit->keyword))
                                return wall;
                }
                act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
                return -1;
        }

        if (!(pexit = ch->in_room->exit[wall]))
        {
                act ("I see no wall $T here.", ch, NULL, arg, TO_CHAR);
                return -1;
        }

        if (!IS_SET(pexit->exit_info, EX_WALL))
        {
                send_to_char("You can't do that.\n\r", ch);
                return -1;
        }

        return wall;
}


void do_climb(CHAR_DATA *ch, char *argument)
{
        char      arg [ MAX_INPUT_LENGTH ];
        int       wall;

        one_argument(argument, arg);

        if (IS_NPC (ch))
                return;

        if (arg[0] == '\0')
        {
                send_to_char("Climb what?\n\r", ch);
                return;
        }

        if ((wall = find_wall(ch, arg)) >= 0)
        {
                /* 'climb wall' */
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;
                OBJ_DATA        *cobj;

                for (cobj = ch->carrying; cobj; cobj = cobj->next_content)
                {
                        if (cobj->item_type == ITEM_CLIMBING_EQ)
                                break;
                }

                if ((!cobj) && (ch->pcdata->learned[gsn_climb] < 1))
                {
                        send_to_char("You have no climbing equipment.\n\r", ch);
                        return;
                }

                pexit = ch->in_room->exit[wall];
                act ("You examine the $d, and decide how best to tackle it.",
                     ch, NULL, pexit->keyword, TO_CHAR);

                if (cobj)
                {
                        if (number_percent () > 50)
                        {
                                act ("You misjudge completely the $d, and tumble down.",
                                     ch, NULL, pexit->keyword, TO_CHAR);
                                ch->position = POS_RESTING;
                                damage(ch, ch, ch->level/2, gsn_climb, FALSE);
                                update_pos (ch);
                                return;
                        }
                        act ("Utilising your climbing equipment, you scale the $d.",
                             ch, NULL, pexit->keyword, TO_CHAR);
                }
                else if (number_percent() > ch->pcdata->learned[gsn_climb])
                {
                        act ("You begin to climb the $d but lose your footing and fall!", ch, NULL, pexit->keyword, TO_CHAR);
                        act ("$n tries to scale the $d, but looses his footing.", ch,
                             NULL, pexit->keyword, TO_ROOM);
                        damage(ch, ch, ch->level/2, gsn_climb, FALSE);
                        ch->position = POS_RESTING;
                        update_pos (ch);
                        return;
                }

                act ("You use your climbing expertise to scale the $d.",
                     ch,NULL,pexit->keyword, TO_CHAR);
                char_from_room (ch);
                char_to_room (ch, pexit->to_room);
                do_look(ch, "auto");

                /* climb to other side */
                if ((to_room   = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[wall].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        CHAR_DATA *rch;

                        for (rch = to_room->people; rch; rch = rch->next_in_room)
                        {
                                if (rch->deleted)
                                        continue;
                                act ("$n scales the $d to join you.",
                                     rch, NULL, pexit_rev->keyword, TO_CHAR);
                        }
                }
        }

        return;
}


void do_close(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        int       door;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Close what?\n\r", ch);
                return;
        }

        if ((obj = get_obj_here(ch, arg)))
        {
                /* 'close object' */
                if (obj->item_type != ITEM_CONTAINER)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->value[1], CONT_CLOSED))
                {
                        send_to_char("It's already closed.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
                {
                        send_to_char("You can't do that.\n\r", ch);
                        return;
                }

                SET_BIT(obj->value[1], CONT_CLOSED);
                send_to_char("Ok.\n\r", ch);
                act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
                return;
        }

        if ((door = find_door(ch, arg)) >= 0)
        {
                /* 'close door' */
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;

                pexit   = ch->in_room->exit[door];
                if (IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("It's already closed.\n\r",    ch);
                        return;
                }

                if (IS_SET(pexit->exit_info, EX_BASHED))
                {
                        act ("The $d has been bashed open and cannot be closed.",
                             ch, NULL, pexit->keyword, TO_CHAR);
                        return;
                }

                SET_BIT(pexit->exit_info, EX_CLOSED);
                act ("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM);
                send_to_char("Ok.\n\r", ch);

                /* close the other side */
                if ((to_room   = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[door].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        CHAR_DATA *rch;

                        SET_BIT(pexit_rev->exit_info, EX_CLOSED);
                        for (rch = to_room->people; rch; rch = rch->next_in_room)
                        {
                                if (rch->deleted)
                                        continue;
                                act ("The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR);
                        }
                }
        }

        return;
}


bool has_key(CHAR_DATA *ch, int key)
{
        OBJ_DATA *obj;

        for (obj = ch->carrying; obj; obj = obj->next_content)
        {
                if (obj->pIndexData->vnum == key)
                        return TRUE;
        }

        return FALSE;
}


void do_lock(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        int       door;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Lock what?\n\r", ch);
                return;
        }

        if ((obj = get_obj_here(ch, arg)))
        {
                /* 'lock object' */
                if (obj->item_type != ITEM_CONTAINER)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (obj->value[2] < 0 || !has_key(ch, obj->value[2]))
                {
                        send_to_char("You lack the key.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->value[1], CONT_LOCKED))
                {
                        send_to_char("It's already locked.\n\r", ch);
                        return;
                }

                SET_BIT(obj->value[1], CONT_LOCKED);
                send_to_char("*Click*\n\r", ch);
                act ("$n locks $p.", ch, obj, NULL, TO_ROOM);
                return;
        }

        if ((door = find_door(ch, arg)) >= 0)
        {
                /* 'lock door' */
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;

                pexit   = ch->in_room->exit[door];

                if (!IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (pexit->key < 0 || !has_key(ch, pexit->key))
                {
                        send_to_char("You lack the key.\n\r", ch);
                        return;
                }

                if (IS_SET(pexit->exit_info, EX_LOCKED))
                {
                        send_to_char("It's already locked.\n\r", ch);
                        return;
                }

                SET_BIT(pexit->exit_info, EX_LOCKED);
                send_to_char("*Click*\n\r", ch);
                act ("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

                /* lock the other side */
                if ((to_room   = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[door].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        SET_BIT(pexit_rev->exit_info, EX_LOCKED);
                }
        }

        return;
}


void do_unlock(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        int       door;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Unlock what?\n\r", ch);
                return;
        }

        if ((obj = get_obj_here(ch, arg)))
        {
                /* 'unlock object' */
                if (obj->item_type != ITEM_CONTAINER)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (obj->value[2] < 0  || !has_key(ch, obj->value[2]))
                {
                        send_to_char("You lack the key.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_LOCKED))
                {
                        send_to_char("It's already unlocked.\n\r", ch);
                        return;
                }

                REMOVE_BIT(obj->value[1], CONT_LOCKED);
                send_to_char("*Click*\n\r", ch);
                act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
                return;
        }

        if ((door = find_door(ch, arg)) >= 0)
        {
                /* 'unlock door' */
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;

                pexit = ch->in_room->exit[door];

                if (!IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (pexit->key < 0 || !has_key(ch, pexit->key))
                {
                        send_to_char("You lack the key.\n\r", ch);
                        return;
                }

                if (!IS_SET(pexit->exit_info, EX_LOCKED))
                {
                        send_to_char("It's already unlocked.\n\r", ch);
                        return;
                }

                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                send_to_char("*Click*\n\r", ch);
                act ("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

                /* unlock the other side */
                if ((to_room   = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[door].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
                }
        }

        return;
}


void do_pick(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        CHAR_DATA *gch;
        char arg [ MAX_INPUT_LENGTH ];
        int door;
        char sLockpickBroken [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Pick what?\n\r", ch);
                return;
        }

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_LOCK_PICK )
                        break;
        }

        if ((obj = get_obj_here(ch, arg)))
        {
                if (obj->item_type != ITEM_CONTAINER)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (!IS_SET(obj->value[1], CONT_LOCKED))
                {
                        send_to_char("It's already unlocked.\n\r", ch);
                        return;
                }

                if (!wobj)
                {
                        send_to_char("Without the right tools you fail totally.\n\r", ch);
                        return;
                }

                for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
                {
                        if (gch->deleted)
                                continue;

                        if (IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level)
                        {
                                act ("$N is too close to the lock.", ch, NULL, gch, TO_CHAR);
                                return;
                        }
                }

                if (number_percent() < 5)
                {
                        wobj->item_type  = ITEM_TRASH;
                        sprintf(sLockpickBroken, "a broken lockpick");
                        free_string(wobj->short_descr);
                        wobj->short_descr = str_dup(sLockpickBroken);
                        send_to_char("You hear a tortured metallic sound. You have rendered your lockpick useless.\n\r", ch);
                        return;
                }

                WAIT_STATE(ch, skill_table[gsn_pick_lock].beats);

                if (!IS_NPC(ch) && number_percent() > (5 + ch->pcdata->learned[gsn_pick_lock] / 2))
                {
                        send_to_char("You failed.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->value[1], CONT_PICKPROOF))
                {
                        send_to_char("You don't think you'll be able to pick that lock.\n\r", ch);
                        return;
                }

                REMOVE_BIT(obj->value[1], CONT_LOCKED);
                send_to_char("*Click*  The lock mechanism releases.\n\r", ch);
                act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
                return;
        }

        /* picking a door */
        if ((door = find_door(ch, arg)) >= 0)
        {
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                ROOM_INDEX_DATA *to_room;

                pexit = ch->in_room->exit[door];
                if (!IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("It's not closed.\n\r", ch);
                        return;
                }

                if (!IS_SET(pexit->exit_info, EX_LOCKED))
                {
                        send_to_char("It's already unlocked.\n\r", ch);
                        return;
                }

                if (!wobj)
                {
                        send_to_char("Without the right tools you fail totally.\n\r", ch);
                        return;
                }

                for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
                {
                        if (gch->deleted)
                                continue;

                        if (IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level)
                        {
                                act ("$N is too close to the lock.", ch, NULL, gch, TO_CHAR);
                                return;
                        }
                }

                if (number_percent() < 5)
                {
                        wobj->item_type  = ITEM_TRASH;
                        sprintf(sLockpickBroken, "a broken lockpick");
                        free_string(wobj->short_descr);
                        wobj->short_descr = str_dup(sLockpickBroken);
                        send_to_char("You hear a tortured metallic sound. You have rendered your lockpick useless.\n\r", ch);
                        return;
                }

                WAIT_STATE(ch, skill_table[gsn_pick_lock].beats);

                if (!IS_NPC(ch) && number_percent() > (5 + ch->pcdata->learned[gsn_pick_lock] / 2))
                {
                        send_to_char("You failed.\n\r", ch);
                        return;
                }

                if (IS_SET(pexit->exit_info, EX_PICKPROOF))
                {
                        send_to_char("You fail to pick the lock and realise it's tamper-proof.\n\r", ch);
                        return;
                }

                /* pick the other side */
                if ((to_room = pexit->to_room)
                    && (pexit_rev = to_room->exit[directions[door].reverse])
                    && pexit_rev->to_room == ch->in_room)
                {
                        REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
                }

                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                send_to_char("*Click*  The lock mechanism releases.\n\r", ch);
                act ("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);
        }
}


void do_stand(CHAR_DATA *ch, char *argument)
{
        switch (ch->position)
        {
            case POS_SLEEPING:
                if ( IS_AFFECTED(ch, AFF_SLEEP)
                && ( (ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG ) )
                && ( ch->in_room->sector_type == SECT_UNDERWATER) )
                {
                        send_to_char("You can't wake up!\n\r", ch);
                        return;
                }

                if ( IS_AFFECTED( ch, AFF_SLEEP )
                && ( ch->in_room->sector_type == SECT_UNDERWATER)
                && !is_affected( ch, gsn_breathe_water ) )
                {
                        REMOVE_BIT(ch->affected_by, AFF_SLEEP);
                         affect_strip(ch, gsn_sleep);
                }

                /* Let's not have imms hosed by sleep traps etc -- Owl 8/7/22 */


                if ( IS_IMMORTAL(ch) )
                {
                        REMOVE_BIT(ch->affected_by, AFF_SLEEP);
                        affect_strip(ch, gsn_sleep);
                        affect_strip(ch, gsn_trap);
                }

                if ( IS_AFFECTED(ch, AFF_SLEEP ) )
                {
                        send_to_char("You can't wake up!\n\r", ch);
                        return;
                }

                if (IS_AFFECTED(ch, AFF_MEDITATE))
                        REMOVE_BIT(ch->affected_by, AFF_MEDITATE);

                send_to_char("You wake and ready yourself for action.\n\r", ch);
                act ("$n wakes and readies $mself for action.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_STANDING;
                break;

            case POS_RESTING:
                send_to_char("You ready yourself for action.\n\r", ch);
                act ("$n readies $mself for action.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_STANDING;
                break;

            case POS_FIGHTING:
                send_to_char("You are already fighting!\n\r",  ch);
                break;

            case POS_STANDING:
                send_to_char("You are already conscious and alert.\n\r",  ch);
                break;
        }
}


void do_rest(CHAR_DATA *ch, char *argument)
{
        if (ch->rider || ch->mount)
        {
                send_to_char("Not while you're mounted.\n\r",ch);
                return;
        }

        switch (ch->position)
        {
            case POS_SLEEPING:
                send_to_char("You are already sleeping.\n\r",  ch);
                break;

            case POS_RESTING:
                send_to_char("You are already resting.\n\r",   ch);
                break;

            case POS_FIGHTING:
                send_to_char("Not while you're fighting!\n\r", ch);
                break;

            case POS_STANDING:
                send_to_char("You rest.\n\r", ch);
                act ("$n rests.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_RESTING;
                break;
        }
}


void do_sleep (CHAR_DATA *ch, char *argument)
{
        if (ch->rider)
        {
                send_to_char("Not while you're mounted.\n\r",ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char ("Not while you are fighting!\n\r", ch);
                return;
        }

        if (ch->mount)
        {
                if (get_curr_int(ch) > 20)
                {
                        send_to_char("Do that and you are likely to fall off your mount.\n\r", ch);
                        return;
                }
                else
                {
                        send_to_char ("Asleep, you fall from your mount.\n\r", ch);
                        act ("$n dozes in the saddle of $N and hits the deck.",
                             ch, NULL, ch->mount, TO_NOTVICT);
                        act ("$n falls asleep - and off your back.",
                             ch, NULL, ch->mount, TO_VICT);
                        damage(ch, ch, number_range(10, ch->mount->level), TYPE_UNDEFINED, FALSE);
                        strip_mount (ch);
                        ch->position = POS_RESTING;
                        return;
                }
        }

        switch (ch->position)
        {
            case POS_SLEEPING:
                send_to_char("You are already sleeping.\n\r",  ch);
                break;

            case POS_RESTING:
            case POS_STANDING:
                remove_songs(ch);
                send_to_char("You sleep.\n\r", ch);
                act ("$n sleeps.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_SLEEPING;
                break;
        }
}


void do_wake(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                do_stand(ch, argument);
                return;
        }

        if (!IS_AWAKE(ch))
        {
                send_to_char("You are asleep yourself!\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AWAKE(victim))
        {
                act ("$N is already awake.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_AFFECTED(victim, AFF_SLEEP) || IS_AFFECTED(victim,AFF_MEDITATE))
        {
                act ("You can't wake $M!", ch, NULL, victim, TO_CHAR);
                return;
        }

        victim->position = POS_STANDING;
        act ("You wake $M.",  ch, NULL, victim, TO_CHAR);
        act ("$n wakes you.", ch, NULL, victim, TO_VICT);
        return;
}


/*
 * Mist walk for vamps - 9/98 Istari
 */
void do_mist_walk(CHAR_DATA *ch, char *argument )
{
        AFFECT_DATA af;
        AFFECT_DATA *paf;

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_mist_walk))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->pcdata->group_leader == ch)
        {
                send_to_char("You can't lead a group in a non-corporeal form.\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_mist_walk))
                return;

        if (ch->mount)
        {
                send_to_char("You cannot whilst riding.\n\r",ch);
                return;
        }

        if ( ch->in_room->sector_type == SECT_UNDERWATER
        && ( ch->race != RACE_SAHUAGIN
          && ch->race != RACE_GRUNG ) )
        {
                send_to_char("Not while you're underwater.\n\r", ch);
                return;
        }

        if (tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_entered_in_tournament(ch)
            && is_still_alive_in_tournament(ch))
        {
                send_to_char("Not during the tournament!\n\r", ch);
                return;
        }

        for ( paf = ch->affected; paf; paf = paf->next )
        {
                if ( paf->deleted )
                        continue;

                if (paf->duration < 0 )
                        continue;

                if (effect_is_prayer(paf))
                        continue;

                affect_remove( ch, paf );
        }

        send_to_char("Your magical effects disperse.\n\r",ch);

        af.type = gsn_mist_walk;
        af.duration = -1;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_NON_CORPOREAL;
        affect_to_char(ch,&af);

        send_to_char("Your body dissolves into a cloud of mist.\n\r",ch);
        act("$n dissolves into a glowing cloud of mist.", ch, NULL, NULL, TO_ROOM);

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

}


void do_sneak (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_sneak))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->mount)
        {
                send_to_char("You can't sneak while mounted.\n\r", ch);
                return;
        }

        send_to_char("You attempt to move amongst the shadows.\n\r", ch);
        affect_strip(ch, gsn_sneak);
        affect_strip(ch, gsn_shadow_form);

        af.type      = gsn_sneak;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(ch, &af);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_sneak])
        {
                af.bitvector = AFF_SNEAK;
                affect_join(ch, &af);
        }
}


void do_hide (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_hide))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        send_to_char("You attempt to hide.\n\r", ch);
        affect_strip(ch, gsn_hide);
        affect_strip(ch, gsn_chameleon_power);

        af.type      = gsn_hide;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(ch, &af);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_hide])
        {
                af.bitvector = AFF_HIDE;
                affect_join(ch, &af);
        }
}


void do_meditate (CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_meditate))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_mount))
        {
                send_to_char("Your mount is a little unstable for proper meditation.\n\r", ch);
                return;
        }

        if ( (!IS_NPC(ch))
        && ch->in_room->sector_type == SECT_UNDERWATER
        && ( ( ch->race != RACE_SAHUAGIN && ch->race != RACE_GRUNG )
          && ( !is_affected(ch, gsn_breathe_water) ) ) )
        {
                send_to_char("You can't meditate underwater if you can't breathe underwater.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_MEDITATE))
                REMOVE_BIT(ch->affected_by, AFF_MEDITATE);

        if (IS_NPC(ch) || number_percent() < (49 + (ch->pcdata->learned[gsn_meditate]/2)))
        {
                SET_BIT(ch->affected_by, AFF_MEDITATE);
                send_to_char("You fall into a deep trance.\n\r", ch);
                act ("$n sits in a lotus position and begins to meditate.",
                     ch, NULL, NULL, TO_ROOM);
                ch->position = POS_SLEEPING;
                return;
        }

        send_to_char("You try to fall into a deep trance, but fail miserably.\n\r",ch);
        WAIT_STATE(ch, 12);  /* Shade 20.2.22 encourage more practice */
        return;
}


/*
 * Contributed by Alander.
 */
void do_visible (CHAR_DATA *ch, char *argument)
{
        affect_strip (ch, gsn_astral_sidestep);
        affect_strip (ch, gsn_invis);
        affect_strip (ch, gsn_mass_invis);
        affect_strip (ch, gsn_chameleon_power);
        affect_strip (ch, gsn_sneak);
        affect_strip (ch, gsn_shadow_form);
        REMOVE_BIT (ch->affected_by, AFF_INVISIBLE);
        REMOVE_BIT (ch->affected_by, AFF_SNEAK);

        /* Owl 26/3/22, to produce expected behaviour */
        if (ch->form != FORM_CHAMELEON)
        {
                affect_strip (ch, gsn_hide);
                REMOVE_BIT (ch->affected_by, AFF_HIDE);
        }
        else {
                send_to_char("You will continue to hide as as long as you are in chameleon form.\n\r",ch);
        }

        if (is_affected(ch,gsn_mist_walk))
        {
                send_to_char("Your body reverts to its normal state.\n\r",ch);
                act("A cloud of glowing mist withdraws to unveil $n!",ch,NULL,NULL,TO_ROOM);
                affect_strip(ch,gsn_mist_walk);
                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                return;
        }

        send_to_char("Ok.\n\r", ch);
}


void do_kiai (CHAR_DATA *ch, char *argument)
{
        char            arg [MAX_INPUT_LENGTH];
        CHAR_DATA*      victim;
        int             mana;

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_kiai))
        {
                send_to_char("You are not mentally prepared.\n\r", ch);
                return;
        }

        mana = 20;

        if (ch->mana < 20)
        {
                send_to_char("You don't have enough mana.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        argument = one_argument(argument, arg);

        if (ch->fighting && arg[0] == '\0')
                victim = ch->fighting;
        else
        {
                if (arg[0] == '\0')
                {
                        send_to_char("Who do you want to focus on?\n\r", ch);
                        return;
                }

                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char ("Who do you want to focus your energies on?\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_kiai].beats);
        ch->mana -= mana;

        if (number_percent() < ch->pcdata->learned[gsn_kiai])
        {
                damage(ch, victim, number_range(5 * ch->level, 8 * ch->level),
                       gsn_kiai, FALSE);
                arena_commentary("$n hits $N with a force ball.", ch, victim);
        }
        else
                damage(ch, victim, 0, gsn_kiai, FALSE);
}


void do_recall (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA        *victim;
        CHAR_DATA        *mob;
        CHAR_DATA        *vnext;
        ROOM_INDEX_DATA  *location;
        OBJ_DATA         *pobj;
        OBJ_DATA         *pobj_next;
        char             buf [ MAX_STRING_LENGTH ];
        int              place;
        int              followers_found;

        char arg[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg);
        argument = one_argument(argument, arg2);

        followers_found = 0;

        if (IS_NPC(ch))
                return;

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                place = ch->in_room->area->recall;

                if (place == DEFAULT_RECALL
                    && ch->level > 40
                    && ch->clan
                    && !ch->pcdata->current_recall)
                {
                        send_to_char("You are too powerful to rely on the gods for help.\n\r", ch);
                        return;
                }

                act ("$n prays for transportation!", ch, NULL, NULL, TO_ROOM);

                /*
                 * multiple recalls.  if pc and current recall set to > 0 then get that
                 * recall point.  check validity and set recall there instead
                 */
                if (ch->pcdata->current_recall
                    && ch->pcdata->recall_points[ch->pcdata->current_recall] != -1)
                {
                        place = ch->pcdata->recall_points[ch->pcdata->current_recall];
                }

                if (!(location = get_room_index(place)))
                {
                        send_to_char("You are completely lost.\n\r", ch);
                        return;
                }

                if (ch->in_room == location)
                        return;

                if ( ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
                   ||  IS_AFFECTED( ch, AFF_CURSE )
                   ||  IS_AFFECTED( ch, AFF_NO_RECALL ) )
                && !IS_SET( ch->in_room->room_flags, ROOM_PLAYER_KILLER ) )
                {
                        send_to_char("God has forsaken you.\n\r", ch);
                        return;
                }

                if ((victim = ch->fighting))
                {
                        int lose;

                        if (has_ego_item_effect(ch, EGO_ITEM_BLOODLUST)
                            && !has_ego_item_effect(ch, EGO_ITEM_BATTLE_TERROR))
                        {
                                send_to_char("You must not recall: your enemy must die!\n\r", ch);
                                return;
                        }

                        if (number_bits(1) || (IS_AFFECTED(ch, AFF_HOLD) && number_percent() < 80))
                        {
                                WAIT_STATE(ch, PULSE_VIOLENCE);
                                lose = (ch->desc) ? 50 : 100;
                                gain_exp(ch, 0 - lose);
                                sprintf(buf, "You failed!  You lose %d experience points.\n\r", lose);
                                send_to_char(buf, ch);
                                return;
                        }

                        lose = level_table[ch->level].exp_level / 100;
                        gain_exp(ch, 0 - lose);
                        sprintf(buf, "You recall from combat!  You lose %d experience points.\n\r", lose);
                        send_to_char(buf, ch);
                        stop_fighting(ch, TRUE);

                        if (ch->pcdata->fame > 500)
                        {
                                sprintf(buf, "Your cowardly actions cover you in shame! You lose %d fame.\n\r", ch->pcdata->fame/40);
                                send_to_char(buf, ch);
                                ch->pcdata->fame = ch->pcdata->fame - (ch->pcdata->fame/40);
                                check_fame_table(ch);
                        }
                }

                /* Check to see if the character is carrying any ITEM_CURSED items.  They
                 * will prevent recall as well as other forms of magical travel.
                 */

                for (pobj = ch->carrying; pobj; pobj = pobj_next)
                {
                        pobj_next = pobj->next_content;

                        if (pobj->deleted)
                                continue;

                        if (IS_SET(pobj->extra_flags, ITEM_CURSED))
                        {
                                sprintf(buf, "The gods will not assist carriers of cursed items.\n\r");
                                send_to_char(buf, ch);
                                return;
                        }
                }

                /*
                 * Check the room for mobs where victim->master == ch.  If you find one,
                 * Send them to the room ch will recall to. Put in to make life a little
                 * less painful for Infernalists -- Owl 24/7/22
                 */

                for(mob = ch->in_room->people; mob; mob = vnext)
                {
                        vnext = mob->next_in_room;

                        if ( ( mob != ch )
                        &&   ( mob->master == ch )
                        &&   ( IS_NPC( mob ) ) )
                        {
                                followers_found++;
                                char_from_room(mob);
                                char_to_room(mob, location);
                                act_move("$n appears suddenly.", mob, NULL, NULL, TO_ROOM);
                                mob = mob->next_in_room;
                        }

                }

                if (tournament_status == TOURNAMENT_STATUS_RUNNING
                    && is_entered_in_tournament(ch)
                    && IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER)
                    && is_still_alive_in_tournament(ch))
                {
                        disqualify_tournament_entrant(ch, "left the Arena");
                        check_for_tournament_winner();
                }

                if (IS_AFFECTED(ch, AFF_SWALLOWED))
                {
                        strip_swallow(ch);
                }

                arena_commentary("$n has recalled to safety.", ch, NULL);

                ch->move /= 2;
                act_move ("$n disappears.", ch, NULL, NULL, TO_ROOM);
                char_from_room(ch);
                char_to_room(ch, location);

                if (ch->mount)
                {
                        char_from_room(ch->mount);
                        char_to_room(ch->mount, location);
                        act_move("$n appears suddenly.", ch->mount, NULL, NULL, TO_ROOM);
                }

                if (is_affected(ch,gsn_mist_walk))
                        act("A strange mist stirs up around you.", ch, NULL, NULL,TO_ROOM);
                else
                        act_move ("$n appears in the room.", ch, NULL, NULL, TO_ROOM);

                do_look(ch, "auto");

                if (followers_found == 1)
                {
                        sprintf(buf, "<15>Your follower has joined you in safety.<0>\n\r");
                        send_to_char(buf, ch);
                }
                else if (followers_found > 1)
                {
                        sprintf(buf, "<15>Your followers have joined you in safety.<0>\n\r");
                        send_to_char(buf, ch);
                }

                return;
        }

        /* list the recall points of the character */
        if (!str_prefix(arg, "list"))
        {
                int iter;
                char tmp[MAX_STRING_LENGTH];
                const char *template = "%2d %c %-35s (%s)";

                sprintf(buf, "Recall points currently available (indexed by number):\n");
                sprintf(tmp, template,
                        0,
                        ((!ch->pcdata->current_recall) ? '*' : ' '),
                        "Default recall",
                        get_room_index(DEFAULT_RECALL)->name);
                strcat(buf, tmp);

                if (IS_IMMORTAL(ch))
                        sprintf(tmp, " (%d)\n\r", DEFAULT_RECALL);
                else
                        sprintf(tmp, "\n\r");
                strcat(buf, tmp);

                for (iter = 1; iter < MAX_RECALL_POINTS; iter++)
                {
                        place = ch->pcdata->recall_points[iter];

                        if (place != -1)
                        {
                                location = get_room_index(place);
                                sprintf(tmp, template, iter,
                                        ((ch->pcdata->current_recall == iter) ? '*' : ' '),
                                        location ? location->name : "Unknown",
                                        location ? location->area->name : "Unknown");
                                strcat(buf, tmp);

                                if (IS_IMMORTAL(ch))
                                        sprintf(tmp, " (%d)\n\r", place);
                                else
                                        sprintf(tmp, "\n\r");
                                strcat(buf, tmp);
                        }
                }

                strcat(buf, "\n\r");
                send_to_char(buf, ch);
                return;
        }

        /* set a new recall point */
        if (!str_prefix(arg, "set"))
        {
                int tmp;

                if (arg2[0] == '\0')
                        tmp = 0;
                else
                        tmp = atoi(arg2);

                tmp = URANGE(0, tmp, (MAX_RECALL_POINTS - 1));
                place = ch->pcdata->recall_points[tmp];

                if (get_room_index(place))
                {
                        ch->pcdata->current_recall = tmp;
                        sprintf(buf, "Setting recall to point %d (%s)\n\r", tmp,
                                get_room_index(place)->name);
                }
                else
                {
                        ch->pcdata->current_recall = 0;
                        sprintf(buf, "Cannot access point %d, defaulting to 0 (%s)\n\r", tmp,
                                get_room_index(DEFAULT_RECALL)->name);
                }

                send_to_char (buf, ch);
                return;
        }

        send_to_char("Syntax: recall [list|set <#>]\n\r", ch);
        return;
}


void do_home (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *victim;
        ROOM_INDEX_DATA *location;
        char             buf [ MAX_STRING_LENGTH ];
        int              place;

        if (!ch->clan)
        {
                send_to_char("You're not in a clan.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        act_move ("$n prays for the safety of their clan!", ch, NULL, NULL, TO_ROOM);

        place = clan_table[ch->clan].hall;

        if (!(location = get_room_index(place)))
        {
                send_to_char("You are completely lost.\n\r", ch);
                return;
        }

        if (ch->in_room == location)
                return;

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
            || IS_AFFECTED(ch, AFF_CURSE)
            || IS_AFFECTED(ch, AFF_NO_RECALL) )
        {
                send_to_char("God has forsaken you.\n\r", ch);
                return;
        }

        if ((victim = ch->fighting))
        {
                int lose;

                if (has_ego_item_effect(ch, EGO_ITEM_BLOODLUST)
                    && !has_ego_item_effect(ch, EGO_ITEM_BATTLE_TERROR))
                {
                        send_to_char("You must not recall: your enemy must die!\n\r", ch);
                        return;
                }

                if (number_bits(1) || (IS_AFFECTED(ch, AFF_HOLD) && number_percent() < 80))
                {
                        WAIT_STATE(ch, PULSE_VIOLENCE);
                        lose = (ch->desc) ? 50 : 100;
                        gain_exp(ch, 0 - lose);
                        sprintf(buf, "You failed!  You lose %d exps.\n\r", lose);
                        send_to_char(buf, ch);
                        return;
                }

                lose = (ch->desc) ? 100 : 200;
                gain_exp(ch, 0 - lose);
                sprintf(buf, "You recall from combat!  You lose %d exps.\n\r", lose);
                send_to_char(buf, ch);
                stop_fighting(ch, TRUE);

                arena_commentary("$n has recalled to safety.", ch, NULL);

                if (ch->pcdata->fame > 500)
                {
                        sprintf(buf, "Your cowardly actions cover you in shame! You lose %d fame.\n\r", ch->pcdata->fame/20);
                        send_to_char(buf, ch);
                        ch->pcdata->fame = ch->pcdata->fame - (ch->pcdata->fame/20);
                }
        }

        ch->move /= 2;
        act_move ("$n disappears.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, location);

        if (ch->mount)
        {
                char_from_room(ch->mount);
                char_to_room(ch->mount, location);
                act_move ("$n appears suddenly.", ch->mount, NULL, NULL, TO_ROOM);
        }

        if (tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_entered_in_tournament(ch)
            && is_still_alive_in_tournament(ch))
        {
                disqualify_tournament_entrant(ch, "left the Arena");
                check_for_tournament_winner();
        }

        act_move ("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
        do_look(ch, "auto");
}


void do_arena (CHAR_DATA *ch, char *argument)
{
        ROOM_INDEX_DATA *location;
        char buf [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (tournament_status != TOURNAMENT_STATUS_NONE && ch->level <= LEVEL_HERO)
        {
                send_to_char("You can't do that while a tournament has been posted.\n\r", ch);
                return;
        }

        act ("$n screams out to be portalled to the Arena!", ch, NULL, NULL, TO_ROOM);
        location = get_room_index(ROOM_VNUM_ARENA + number_range(5,35));

        if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                send_to_char("You are already in the Arena.\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
            || is_affected(ch, AFF_CURSE)
            || is_affected(ch, AFF_NO_RECALL))
        {
                send_to_char("Your cries fall upon deaf ears.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You cannot be transported while fighting.\n\r", ch);
                return;
        }

        act ("A giant hand reaches from out of the heavens and drags $n away!",
             ch, NULL, NULL, TO_ROOM);
        send_to_char("\n\r{RA giant hand grabs you and places you in the Arena!{x\n\r\n\r",ch);

        sprintf(buf, "*** %s has entered the ARENA! ***", ch->name);
        tournament_message(buf, ch);
        char_from_room(ch);
        char_to_room(ch, location);

        if (ch->mount)
        {
                char_from_room(ch->mount);
                char_to_room(ch->mount, location);
                act_move ("$n appears suddenly.", ch->mount, NULL, NULL, TO_ROOM);
        }

        act ("A giant hand places $n in the room!", ch, NULL, NULL, TO_ROOM);
        do_look (ch, "auto");
        return;
}


void do_change (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *mob;
        OBJ_DATA *obj, *obj_next;
        bool remove_eq;
        int class;
        char buf [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        buf[0] = '\0';
        one_argument(argument, arg);

        /* Check for master to change classes */
        for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
        {
                if (mob->deleted)
                        continue;
                if (IS_NPC(mob) && mob->pIndexData->skills)
                        break;
        }

        if (!mob)
        {
                send_to_char("Who is going to change your profession?\n\r", ch);
                return;
        }

        if (ch->level != 30)
        {
                send_to_char("You can only change classes at level 30!\n\r", ch);
                return;
        }

        if (ch->pcdata->choose_subclass)
        {
                send_to_char("You have already chosen a sub-class.\n\r", ch);
                return;
        }

        if (ch->class == CLASS_SHAPE_SHIFTER && ch->form != FORM_NORMAL)
        {
                send_to_char ("You must be in normal form to change to a sub-class.\n\r", ch);
                return;
        }

        /* Fix grammar below, 'a' may be incorrect for some classes -- Owl*/

        if (arg[0] == '\0')
        {
                sprintf(buf, "You can specialise as a %s, a %s or a %s.\n\r",
                        full_sub_class_name(ch->class * 2 + 1),
                        full_class_name(ch->class),
                        full_sub_class_name(ch->class * 2 + 2));
                send_to_char(buf, ch);
                return;
        }

        if (!str_prefix(arg, class_table[ch->class].show_name))
        {
                sprintf(buf, "You have chosen to remain true to the path of the %s.\n\rYou have chosen wisely...\n\r",
                        full_class_name(ch->class));

                send_to_char(buf, ch);
                ch->sub_class = 0;
                ch->pcdata->choose_subclass = TRUE;
                return;
        }
        else if (!str_prefix(arg, sub_class_table[2 * ch->class + 1].who_name))
        {
                ch->sub_class =  2 * ch->class + 1;
        }
        else if (!str_prefix(arg, sub_class_table[2 * ch->class + 2].who_name))
        {
                ch->sub_class =  2 * ch->class + 2;
        }
        else
        {
                sprintf(buf, "To change to a %s, enter 'Change %s'.\n\r"
                        "To change to a %s, enter 'Change %s'.\n\r",
                        full_sub_class_name(ch->class * 2 + 1),
                        sub_class_table[2 * ch->class + 1].who_name,
                        full_sub_class_name(ch->class * 2 + 2),
                        sub_class_table[2 * ch->class + 2].who_name);
                send_to_char(buf, ch);
                return;
        }

        /* changing class - check for teacher then change */
        sprintf(buf, "%d", LAST_BASE_CLASS_INDEX);
        log_string(buf);
        sprintf(buf, "%d", ch->sub_class);
        log_string(buf);

        sprintf(buf, "%d", mob->pIndexData->skills->learned[LAST_BASE_CLASS_INDEX + ch->sub_class]);
        log_string(buf);

        if (!mob->pIndexData->skills->learned[LAST_BASE_CLASS_INDEX + ch->sub_class])
        {
                sprintf(buf, "I'm sorry %s, but I am not familiar with the ways of the %s.",
                        ch->name, full_sub_class_name(ch->sub_class));
                do_say(mob, buf);
                ch->sub_class = 0;
                return;
        }

        /* do the change */

        if ( ch->sub_class == SUB_CLASS_INFERNALIST
        ||   ch->sub_class == SUB_CLASS_ENGINEER
        ||   ch->sub_class == SUB_CLASS_RUNESMITH )
        {
                /* Grammar! -- Owl 29/7/22 */
                sprintf(buf, "A wise choice %s... may you fare well as an %s.", ch->name, full_sub_class_name(ch->sub_class));
                do_say(mob, buf);
        }
        else {
                sprintf(buf, "A wise choice %s... may you fare well as a %s.", ch->name, full_sub_class_name(ch->sub_class));
                do_say(mob, buf);
        }

        ch->pcdata->choose_subclass = TRUE;
        ch->pcdata->learned[LAST_BASE_CLASS_INDEX + ch->sub_class ] = 30;

        if (ch->sub_class == SUB_CLASS_VAMPIRE || ch->sub_class == SUB_CLASS_WEREWOLF)
        {
                sprintf(buf, "%ss are a challenging class %s, I shall grant you some additional practices to help.", full_sub_class_name(ch->sub_class), ch->name);
                do_say(mob, buf);

                ch->pcdata->int_prac += 5;
                ch->pcdata->str_prac += 5;
        }

        /* remove any eq the new class can't wear; Gezhp */

        remove_eq = FALSE;

        class = ch->class;
        if (ch->sub_class)
                class = MAX_CLASS + ch->sub_class - 1;

        for (obj = ch->carrying; obj; obj = obj_next)
        {
                obj_next = obj->next_content;

                if (obj->wear_loc != WEAR_NONE
                    && (!wear_table[class].can_wear[obj->item_type]
                        || !loc_wear_table[class].can_wear[eq_slot_to_wear_bit[obj->wear_loc]]))
                {
                        if (!remove_eq)
                        {
                                remove_eq = TRUE;
                                send_to_char ("Your new subclass does not permit the wearing of some of your equipment.\n\r", ch);
                        }

                        unequip_char (ch, obj);
                        act ("$n stops using $p.", ch, obj, NULL, TO_ROOM);
                        act ("You stop using $p.", ch, obj, NULL, TO_CHAR);
                }
        }
}


void do_chameleon (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_chameleon_power))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        send_to_char("You attempt to blend in with your surroundings.\n\r", ch);
        affect_strip(ch, gsn_hide);
        affect_strip(ch, gsn_chameleon_power);

        af.type      = gsn_chameleon_power;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(ch, &af);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_chameleon_power])
        {
                af.bitvector = AFF_HIDE;
                affect_join(ch, &af);
        }
}


void do_heighten (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_heighten))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        };

        if (is_affected(ch, gsn_heighten))
        {
                send_to_char("Your senses are already enhanced.\n\r", ch);
                return;
        }

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_heighten])
        {
                af.type      = gsn_heighten;
                af.duration  = 24;
                af.modifier  = 0;
                af.location  = APPLY_NONE;
                af.bitvector = AFF_DETECT_INVIS;
                affect_to_char(ch, &af);

                af.bitvector = AFF_DETECT_HIDDEN;
                affect_to_char(ch, &af);

                af.bitvector = AFF_INFRARED;
                affect_to_char(ch, &af);

                send_to_char("Your senses are heightened.\n\r", ch);
                return;
        }
        else
        {
                send_to_char("You fail to enhance your senses.\n\r", ch);
                return;
        }
}


void do_smash (CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch) && !(ch->spec_fun == spec_lookup("spec_warrior") || ch->spec_fun == spec_lookup("spec_guard")))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_smash))
        {
                send_to_char("You'd better leave the smashing to trained warriors.\n\r", ch);
                return;
        }

        if ((!IS_NPC(ch) || !IS_HUGE(ch)) && !get_eq_char(ch, WEAR_SHIELD) )
        {
                send_to_char("You must be wearing a shield to smash.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (IS_HUGE(ch->fighting))
        {
                send_to_char("They are too big for you to knock over!\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_smash])
        {
                act("Your powerful {Bsmash{x stuns $N!", ch, NULL, ch->fighting, TO_CHAR);
                act("$n's {Bsmash{x stuns you!  You see nothing but stars.", ch, NULL, ch->fighting, TO_VICT);
                act ("$n {Bsmashes{x $N with $s shield!", ch, NULL, ch->fighting, TO_NOTVICT);
                arena_commentary("$n smashes $N to the ground.", ch, ch->fighting);

                WAIT_STATE(ch->fighting, 2 * PULSE_VIOLENCE);
                one_hit(ch, ch->fighting, gsn_smash);
        }
        else
        {
                act ("Your {Wsmash{x misses $N.", ch, NULL, ch->fighting, TO_CHAR);
        }

        return;
}


/* unarmed combat - geoff */

void do_unarmed_combat (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af, *paf;
        int temp;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_unarmed_combat))
        {
                send_to_char("Huh!?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("Not while you're fighting.\n\r", ch);
                return;
        }

        if (is_affected (ch, gsn_unarmed_combat))
        {
                for (paf = ch->affected; paf; paf = paf->next)
                {
                        if (paf->deleted)
                                continue;

                        if (paf->type == gsn_unarmed_combat)
                                affect_remove(ch, paf);
                }

                send_to_char("Your mind wanders from the arts of unarmed combat.\n\r", ch);
                return;
        }

        if (get_curr_int (ch) < 16)
        {
                send_to_char ("You are unable to remember the details of the art...\n\r", ch);
                return;
        }

        temp = ch->level;

        if (number_percent() < ch->pcdata->learned[gsn_unarmed_combat])
        {
                af.type     = gsn_unarmed_combat;
                af.duration = -1;
                af.modifier = temp / 4;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char(ch, &af);

                af.location  = APPLY_DAMROLL;
                af.modifier  = temp / 4;
                affect_to_char(ch, &af);

                send_to_char("You are prepared for unarmed combat.\n\r", ch);
                return;
        }

        send_to_char("You prepare for unarmed combat but cannot focus...\n\r",ch);
        return;
}


void do_warcry (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;
        int temp;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_warcry))
        {
                send_to_char("Huh!?\n\r", ch);
                return;
        }

        if (is_affected (ch, gsn_warcry))
                return;

        if (get_curr_int (ch) < 16)
        {
                send_to_char ("You are unable to remember the complex warchant...\n\r", ch);
                return;
        }

        temp = ch->level;

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_warcry])
        {
                af.type     = gsn_warcry;
                af.duration = temp / 2;
                af.modifier = temp /8;
                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                affect_to_char(ch, &af);

                af.location  = APPLY_DAMROLL;
                af.modifier  = temp /8;
                affect_to_char(ch, &af);
                act ("$n cries an ancient chant in preparation for battle.",
                     ch, NULL, NULL, TO_ROOM);
                send_to_char("You cry an ancient warchant in preparation for battle.\n\r", ch);

                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        send_to_char("This time the warchant doesn't inspire you for battle...\n\r",ch);
        WAIT_STATE(ch, PULSE_VIOLENCE / 2);
        return;
}


void do_rage (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;
        int temp;

        if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_rage))
        {
                send_to_char("Huh!?\n\r", ch);
                return;
        }

        if (is_affected (ch, gsn_rage))
                return;

        if (ch->rage > 50)
        {
                send_to_char ("Your rage is too high.\n\r", ch);
                return;
        }

        temp = ch->level;

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_rage])
        {
                af.type     = gsn_rage;
                af.duration = temp / 4;
                af.modifier = temp / 6;
                af.location = APPLY_DAMROLL;
                af.bitvector = 0;
                affect_to_char(ch, &af);

                act ("$n starts to growl horribly... I'd stand back if I were you.\n\r",ch,NULL,NULL,TO_ROOM);
                send_to_char("You begin to growl softly, focusing on your next target.\n\r", ch);

                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        send_to_char("Your need for combat is not as great as you thought.\n\r",ch);
        WAIT_STATE(ch, PULSE_VIOLENCE / 2);
        return;
}


void do_shadow_form (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_shadow_form))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                return;

        send_to_char("You attempt to move in the shadows.\n\r", ch);
        affect_strip(ch, gsn_sneak);
        affect_strip(ch, gsn_shadow_form);

        af.type      = gsn_shadow_form;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(ch, &af);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_shadow_form])
        {
                af.bitvector = AFF_SNEAK;
                affect_join(ch, &af);
        }
}


/*
 * Bash code by Thelonius for EnvyMud (originally bash_door)
 * Damage modified using Morpheus's code
 */
void do_bash (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *gch;
        char       arg [ MAX_INPUT_LENGTH ];
        int        door;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_bash))
        {
                send_to_char("You're not skilled enough to bash down doors.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (ch->fighting)
        {
                send_to_char("You can't break off your fight.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Bash what?\n\r", ch);
                return;
        }

        if ((door = find_door(ch, arg)) >= 0)
        {
                ROOM_INDEX_DATA *to_room;
                EXIT_DATA       *pexit;
                EXIT_DATA       *pexit_rev;
                int              chance;

                pexit = ch->in_room->exit[door];

                if (!IS_SET(pexit->exit_info, EX_CLOSED))
                {
                        send_to_char("Calm down.  It is already open.\n\r", ch);
                        return;
                }

                if (!IS_SET(pexit->exit_info, EX_LOCKED))
                {
                        send_to_char("Calm down.  It is not locked.\n\r", ch);
                        return;
                }

                WAIT_STATE(ch, skill_table[gsn_bash].beats);

                chance = ch->pcdata->learned[gsn_bash]/2;

                if (IS_SET(pexit->exit_info, EX_LOCKED))
                        chance /= 2;

                if (IS_SET(pexit->exit_info, EX_BASHPROOF))
                {
                        act ("WHAAAAM!!!  You bash against the $d, but it doesn't budge.",
                             ch, NULL, pexit->keyword, TO_CHAR);
                        send_to_char("You don't think that you'll ever be able to break through.\n\r", ch );
                        act ("WHAAAAM!!!  $n bashes against the $d, but it holds strong.",
                             ch, NULL, pexit->keyword, TO_ROOM);
                        damage(ch, ch, (ch->max_hit / 5), gsn_bash, FALSE);
                        return;
                }

                if ((get_curr_str(ch) >= 20)
                    && number_percent() < (chance + 4 * (get_curr_str(ch) - 20)))
                {
                        /* Success */
                        REMOVE_BIT(pexit->exit_info, EX_CLOSED);
                        if (IS_SET(pexit->exit_info, EX_LOCKED))
                                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                        SET_BIT(pexit->exit_info, EX_BASHED);

                        act ("Crash!  You bashed open the $d!", ch, NULL, pexit->keyword, TO_CHAR);
                        act ("$n bashes open the $d!", ch, NULL, pexit->keyword, TO_ROOM);

                        damage(ch, ch, (ch->max_hit / 20), gsn_bash, FALSE);

                        /* Bash through the other side */
                        if ((to_room = pexit->to_room)
                            && (pexit_rev = to_room->exit[directions[door].reverse])
                            && pexit_rev->to_room == ch->in_room)
                        {
                                CHAR_DATA *rch;

                                REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
                                if (IS_SET(pexit_rev->exit_info, EX_LOCKED))
                                        REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
                                SET_BIT(pexit_rev->exit_info, EX_BASHED);

                                for (rch = to_room->people; rch; rch = rch->next_in_room)
                                {
                                        if (rch->deleted)
                                                continue;
                                        act ("The $d crashes open!",
                                             rch, NULL, pexit_rev->keyword, TO_CHAR);
                                }

                        }
                }
                else
                {
                        /* Failure */
                        act ("OW!  You bash against the $d, but it doesn't budge.",
                             ch, NULL, pexit->keyword, TO_CHAR);
                        act ("$n bashes against the $d, but it holds strong.",
                             ch, NULL, pexit->keyword, TO_ROOM);
                        damage(ch, ch, (ch->max_hit / 10), gsn_bash, FALSE);
                }
        }

        /*
         * Check for "guards"... anyone bashing a door is considered as
         * a potential aggressor, and there's a 25% chance that mobs
         * will do unto before being done unto.
         */
        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                if (gch->deleted)
                        continue;
                if (IS_AWAKE(gch)
                    && !gch->fighting
                    && (IS_NPC(gch) && !IS_AFFECTED(gch, AFF_CHARM))
                    && (ch->level - gch->level <= 4)
                    && number_bits(2) == 0)
                        multi_hit(gch, ch, TYPE_UNDEFINED);
        }
}


void do_pattern(CHAR_DATA* ch, char* argument)
{
        char            arg [ MAX_INPUT_LENGTH ];
        int             place, i, target;
        ROOM_INDEX_DATA *location;
        OBJ_DATA        *pobj;
        OBJ_DATA        *pobj_next;
        char            buf[MAX_STRING_LENGTH];
        char            tmp[MAX_STRING_LENGTH];

        if(IS_NPC(ch))
                return;

        if(!CAN_DO(ch, gsn_pattern))
        {
                send_to_char("You create a mystical-looking pattern before you, but nothing seems to happen...\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You cannot focus your energies while fighting.\n\r", ch);
                return;
        }

        for (pobj = ch->carrying; pobj; pobj = pobj_next)
        {
                pobj_next = pobj->next_content;

                if (pobj->deleted)
                        continue;

                if (IS_SET(pobj->extra_flags, ITEM_CURSED))
                {
                        send_to_char("You are carrying a cursed item that prevents magical travel.\n\r", ch);
                        return;
                }
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                sprintf(buf, "Patterns available are:\n");

                for (i=1;i<MAX_PATTERN;i++)
                {
                        place = pattern_list[i].location;
                        location = get_room_index(place);
                        sprintf(tmp, "%d. %s\n", i, location->name);
                        strcat(buf, tmp);
                }

                send_to_char(buf, ch);
                return;
        }

        if (!str_cmp(arg, "set"))
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
                {
                        send_to_char("You cannot create a mystic pattern here.\n\r", ch);
                        return;
                }

                if (ch->in_room->area->low_level == -4 && ch->in_room->area->high_level == -4)
                {
                        send_to_char("You cannot create a mystic pattern here.\n\r", ch);
                        return;
                }

                if(ch->mana < 200)
                {
                        send_to_char("You cannot gather enough mana to create a pattern.\n\r",ch);
                        return;
                }

                ch->mana -= 200;
                ch->pcdata->pattern = ch->in_room->vnum;
                send_to_char("{CYou create a mystical pattern before you and imbue it with magical energy.{x\n\r", ch);
                act("{Y$c creates a mystical pattern before them and imbues it with magical energy.{x",
                        ch, NULL, NULL, TO_ROOM);
                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                return;
        }

        if (!str_cmp(arg, "return"))
        {
                if (ch->pcdata->pattern)
                {
                        location = get_room_index(ch->pcdata->pattern);
                        ch->pcdata->pattern = 0;
                }
                else
                {
                        send_to_char("You have no pattern to return to.\n\r", ch);
                        return;
                }
        }
        else
        {
                target = atoi(arg);

                if ((target < 1) || (target > MAX_PATTERN - 1))
                {
                        send_to_char("Syntax: pattern # | pattern set | pattern return\n\r", ch);
                        return;
                }

                location = get_room_index(pattern_list[target].location);
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
        {
                send_to_char("You cannot find a link to a mystic pattern here.\n\r", ch);
                return;
        }

        if(ch->mana < 100)
        {
                send_to_char("You cannot gather enough mana to create a pattern.\n\r",ch);
                return;
        }

        act ("{Y$c steps into a mystic pattern and is gone.{x", ch, NULL, NULL, TO_ROOM);
        send_to_char("{YYou create a mystic pattern and step into it...{x\n\r\n\r", ch);

        char_from_room(ch);
        char_to_room(ch, location);

        if (ch->mount)
        {
                act ("$n steps into a mystic pattern and is gone.", ch->mount, NULL, NULL, TO_ROOM);
                char_from_room(ch->mount);
                char_to_room(ch->mount, location);
        }

        do_look(ch, "auto");
        act ("A small chime sounds and $n appears.", ch, NULL, NULL, TO_ROOM);
        ch->mana -= 100;
        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        return;
}


/* EOF act_move.c */
