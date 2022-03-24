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
 *  benefiting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


char * const where_name [] =
{
    "{d[{x{wused as light{d]{x     ",
    "{d<{x{wworn on finger{d>{x    ",
    "{d<{x{wworn on finger{d>{x    ",
    "{d<{x{wworn around neck{d>{x  ",
    "{d<{x{wworn around neck{d>{x  ",
    "{d<{x{wworn on body{d>{x      ",
    "{d<{x{wworn on head{d>{x      ",
    "{d<{x{wworn on legs{d>{x      ",
    "{d<{x{wworn on feet{d>{x      ",
    "{d<{x{wworn on hands{d>{x     ",
    "{d<{x{wworn on arms{d>{x      ",
    "{d[{x{wshield{d]{x            ",
    "{d<{x{wworn about body{d>{x   ",
    "{d<{x{wworn about waist{d>{x  ",
    "{d<{x{wworn around wrist{d>{x ",
    "{d<{x{wworn around wrist{d>{x ",
    "{d[{x{wweapon{d]{x            ",
    "{d[{x{wheld in hand{d]{x      ",
    "{d[{x{wsecond weapon{d]{x     ",
    "{d<{x{wfloating nearby{d>{x   ",
    "{d<{x{wsecured to belt{d>{x   ",
    "{d[{x{wranged weapon{d]{x     "
};


const int equipment_slot_order [MAX_WEAR] =
{
        WEAR_FLOAT,
        WEAR_HEAD,
        WEAR_NECK_1,
        WEAR_NECK_2,
        WEAR_ARMS,
        WEAR_WRIST_L,
        WEAR_WRIST_R,
        WEAR_HANDS,
        WEAR_FINGER_L,
        WEAR_FINGER_R,
        WEAR_BODY,
        WEAR_ABOUT,
        WEAR_WAIST,
        WEAR_POUCH,
        WEAR_LEGS,
        WEAR_FEET,
        WEAR_LIGHT,
        WEAR_HOLD,
        WEAR_SHIELD,
        WEAR_WIELD,
        WEAR_DUAL,
        WEAR_RANGED_WEAPON
};


/* This for the max count is who :) */
int max_on = 0;

/*
 * Local functions.
 */
char *  format_obj_to_char      args( ( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort ) );
void    show_char_to_char_0     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    prac_slist              (CHAR_DATA *ch);


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
        static char buf [ MAX_STRING_LENGTH ];

        buf[0] = '\0';

        if ( IS_OBJ_STAT( obj, ITEM_INVIS) )
                strcat( buf, "{c(Invis){x " );

        if (( IS_AFFECTED( ch, AFF_DETECT_EVIL  ) || is_affected(ch, gsn_song_of_revelation))
             && IS_OBJ_STAT( obj, ITEM_EVIL ) )
                strcat( buf, "{r(Red Aura){x " );

        if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC ) && IS_OBJ_STAT( obj, ITEM_MAGIC ) )
                strcat( buf, "{B(Magical){x " );

        /* Below is ugly, sorry -- Owl 4/3/22 */

        if ( ( ( IS_OBJ_STAT( obj, ITEM_NODROP )
              || IS_OBJ_STAT( obj, ITEM_NOREMOVE )
              || ( obj->value[1] == 33 )         /* curse, hex, divine curse */
              || ( obj->value[1] == 304 )
              || ( obj->value[1] == 458 )
              || ( obj->value[2] == 33 )
              || ( obj->value[2] == 304 )
              || ( obj->value[2] == 458 )
              || ( obj->value[3] == 33 )
              || ( obj->value[3] == 304 )
              || ( obj->value[3] == 458 ) )
            && IS_AFFECTED( ch, AFF_DETECT_CURSE ) ) )
                strcat( buf, "{b(Cursed){x " );

        if ( IS_OBJ_STAT( obj, ITEM_TRAP) && IS_AFFECTED( ch, AFF_DETECT_TRAPS) )
                strcat( buf, "(Trapped) " );

        if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
                strcat( buf, "{Y(Glowing){x " );

        if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
                strcat( buf, "{C(Humming){x " );

        if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
                strcat( buf, "{M(Poisoned){x " );

        if (IS_OBJ_STAT(obj, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_FIREBRAND))
                strcat( buf, "{R(Searing){x " );

        if ( IS_OBJ_STAT( obj, ITEM_SHARP ) )
                strcat( buf, "{W(Sharp){x " );

        if ( IS_OBJ_STAT( obj, ITEM_FORGED ) )
                strcat( buf, "{B(Forged){x " );

        if (IS_OBJ_STAT(obj, ITEM_BLADE_THIRST))
                strcat(buf, "{R(Thirsty){x ");

        if (obj->timer > 0
            && obj->timer < 21
            && ((obj->item_type == ITEM_WEAPON
                 && (IS_OBJ_STAT(obj, ITEM_POISONED)
                     || IS_OBJ_STAT(obj, ITEM_SHARP)
                     || IS_OBJ_STAT(obj, ITEM_BLADE_THIRST)))
                || (obj->item_type == ITEM_ARMOR
                    && IS_OBJ_STAT(obj, ITEM_FORGED))))
        {
                strcat(buf, "{R(Damaged){x ");
        }

        if ( fShort )
        {
                if ( obj->short_descr )
                        strcat( buf, obj->short_descr );
        }
        else
        {
                if ( obj->description )
                        strcat( buf, obj->description );
        }

        return buf;
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
        OBJ_DATA  *obj;
        char       buf [ MAX_STRING_LENGTH ];
        char     **prgpstrShow;
        char      *pstrShow;
        int       *prgnShow;
        int        nShow = 0;
        int        iShow;
        int        count = 0;
        bool       fCombine;

        if (!ch->desc)
                return;

        /*
         * Allocate space for output lines.
         */
        for (obj = list; obj; obj = obj->next_content)
        {
                if (!obj->deleted)
                        count++;
        }

        prgpstrShow = alloc_mem(count * sizeof(char *));
        prgnShow = alloc_mem(count * sizeof(int));

        /*
         * Format the list of objects.
         */
        for (obj = list; obj; obj = obj->next_content)
        {
                /* Objects with empty descriptions are intended to be hidden.
                   Suppress blanks lines that can appear in room descriptions: these can tip off players. */
                if (!fShort && !strcmp(obj->description, "")) {
                        continue;
                }

                if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
                {
                        pstrShow = format_obj_to_char(obj, ch, fShort);
                        fCombine = FALSE;

                        if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
                        {
                                /*
                                 * Look for duplicates, case sensitive.
                                 * Matches tend to be near end so run loop backwards.
                                 */
                                for (iShow = nShow - 1; iShow >= 0; iShow--)
                                {
                                        if (!strcmp(prgpstrShow[iShow], pstrShow))
                                        {
                                                prgnShow[iShow]++;
                                                fCombine = TRUE;
                                                break;
                                        }
                                }
                        }

                        /*
                         * Couldn't combine, or didn't want to.
                         */
                        if (!fCombine)
                        {
                                prgpstrShow [nShow] = str_dup(pstrShow);
                                prgnShow    [nShow] = 1;
                                nShow++;
                        }
                }
        }

        /*
         * Output the formatted list.
         */
        for (iShow = 0; iShow < nShow; iShow++)
        {
                if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
                {
                        if (prgnShow[iShow] != 1)
                        {
                                sprintf(buf, "{d({x{w%2d{d){x ", prgnShow[iShow]);
                                send_to_char(buf, ch);
                        }
                        else
                        {
                                send_to_char("     ", ch);
                        }
                }
                send_to_char(prgpstrShow[iShow], ch);
                send_to_char("\n\r", ch);
                free_string(prgpstrShow[iShow]);
        }

        if (fShowNothing && nShow == 0)
        {
                if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
                        send_to_char( "     ", ch );

                send_to_char("Nothing.\n\r", ch);
        }

        /*
         * Clean up.
         */
        free_mem(prgpstrShow, count * sizeof(char *));
        free_mem(prgnShow, count * sizeof(int));
}


void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
        char buf [ MAX_STRING_LENGTH ];
        char tmp [ MAX_STRING_LENGTH ];

        buf[0] = '\0';

        if (is_affected(victim,gsn_mist_walk) )
        {
                if ( CAN_SEE_MIST(ch) )
                        sprintf(buf, "[%s] ", victim->name);

                strcat(buf, "{G(Glowing){x A strange mist floats about.\n\r");
                send_to_char(buf, ch);
                return;
        }

        if (tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_entered_in_tournament(victim)
            && tournament_team_count > 1
            && victim->tournament_team >= 0
            && victim->tournament_team < TOURNAMENT_MAX_TEAMS)
        {
                sprintf(buf, "%s%s{x ",
                        tournament_teams[victim->tournament_team].colour_code,
                        tournament_teams[victim->tournament_team].flag);
        }

        if (IS_NPC(victim)
            && !IS_NPC(ch)
            && ch->pcdata->questmob > 0
            && victim->pIndexData->vnum == ch->pcdata->questmob)
                strcat(buf, "{R[TARGET]{x ");

        if (IS_AFFECTED(victim, AFF_MEDITATE))
                strcat(buf, "{W[Meditating]{x ");

        if (IS_AFFECTED(victim, AFF_HOLD))
                strcat(buf, "{g(Held){x ");

        if (IS_AFFECTED(victim, AFF_INVISIBLE))
                strcat(buf, "{c(Invis){x ");

        if (IS_AFFECTED(victim, AFF_HIDE))
                strcat(buf, "{m(Hidden){x ");

        if (IS_AFFECTED(victim, AFF_CHARM))
                strcat(buf, "{y(Charmed){x ");

        if (IS_SET(victim->act, ACT_MOUNTABLE))
        {
                if (victim->rider)
                {
                        sprintf (tmp, "<Mounted by %s> ", PERS(victim->rider, ch));
                        strcat (buf, tmp);
                }
                else strcat(buf, "<Mount> " );
        }

        if (IS_AFFECTED(victim, AFF_PASS_DOOR))
                strcat(buf, "(Translucent) ");

        if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
                strcat(buf, "{M(Pink Aura){x "  );

        if (IS_EVIL(victim )
            && (IS_AFFECTED(ch, AFF_DETECT_EVIL) || is_affected(ch, gsn_song_of_revelation)))
                strcat(buf, "{r(Red Aura){x "   );

        if (IS_AFFECTED(ch, AFF_DETECT_CURSE)
            && ( is_affected(victim, gsn_prayer_weaken)
              || is_affected(victim, gsn_hex)
              || is_affected(victim, gsn_curse)
              || IS_AFFECTED(victim, AFF_CURSE) ) )
                strcat(buf, "{b(Cursed){x "   );

        if (IS_GOOD(victim )
            && (IS_AFFECTED(ch, AFF_DETECT_GOOD) || is_affected(ch, gsn_song_of_revelation)))
                strcat(buf, "{y(Yellow Aura){x "   );

        if (IS_AFFECTED(victim, AFF_SANCTUARY )   )
                strcat(buf, "{W(White Aura){x " );

        if (IS_AFFECTED(victim, AFF_BATTLE_AURA) )
                strcat(buf, "{w(Silvery Glow){x " );

        if (IS_AFFECTED(victim, AFF_FLAMING )   )
                strcat(buf, "{R(Flaming){x "    );

        if (IS_AFFECTED (victim, AFF_GLOBE ))
                strcat(buf, "{Y(Globed){x " );

        if (!IS_NPC(victim ) && IS_SET(victim->status, PLR_KILLER )  )
                strcat(buf, "{W(KILLER){x " );

        if (!IS_NPC(victim ) && IS_SET(victim->status, PLR_THIEF  )  )
                strcat(buf, "{Y(THIEF){x " );

        if (!IS_NPC(victim ) && IS_SET(victim->status, PLR_RONIN  )  )
                strcat(buf, "{R(RONIN){x " );

        /*
        if (!IS_NPC(victim ) && IS_SET(victim->status, PLR_HUNTED  )  )
                strcat(buf, "{Y({W${Y){x " );
        */

        if (!IS_NPC(victim ) && IS_SET(victim->act, PLR_AFK )  )
                strcat(buf, "{g<{GAFK{x{g>{x ");


        if (victim->form != FORM_NORMAL)
        {
                strcat(buf, "The ");
                strcat(buf, extra_form_name(victim->form));
                strcat(buf, " form of ");
        }

        if (victim->position == POS_STANDING
            && victim->long_descr[0] != '\0'
            && (IS_NPC(victim) || !IS_SET(ch->act, PLR_BRIEF)))
        {
                strcat( buf, victim->long_descr );
                send_to_char( buf, ch );
                return;
        }

        strcat( buf, PERS( victim, ch ) );

        if ( !IS_NPC( victim ) && !IS_SET( ch->act, PLR_BRIEF ) )
                strcat( buf, victim->pcdata->title );

        switch ( victim->position )
        {
            case POS_DEAD:     strcat( buf, " is DEAD!!"              ); break;
            case POS_MORTAL:   strcat( buf, " is mortally wounded."   ); break;
            case POS_INCAP:    strcat( buf, " is incapacitated."      ); break;
            case POS_STUNNED:  strcat( buf, " is lying here, stunned." ); break;
            case POS_SLEEPING: strcat( buf, " is sleeping here."      ); break;
            case POS_RESTING:  strcat( buf, " is resting here."       ); break;
            case POS_STANDING: strcat( buf, " is here."               ); break;

            case POS_FIGHTING:
                strcat( buf, " is here, fighting " );
                if ( !victim->fighting )
                        strcat( buf, "thin air??" );
                else if ( victim->fighting == ch )
                        strcat( buf, "YOU!" );
                else if ( victim->in_room == victim->fighting->in_room )
                {
                        strcat( buf, PERS( victim->fighting, ch ) );
                        strcat( buf, "." );
                }
                else
                        strcat( buf, "someone who left??" );
                break;
        }

        strcat( buf, "\n\r" );
        buf[0] = UPPER( buf[0] );
        send_to_char( buf, ch );
        return;
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
        OBJ_DATA *obj;
        char      buf [ MAX_STRING_LENGTH ];
        int       iWear;
        int       percent;
        bool      found;

        if ( is_affected(victim, gsn_mist_walk) )
        {
                send_to_char("You see particles of fine mist.\n\r", ch);
                return;
        }

        if ( can_see( victim, ch ) )
        {
                act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
                act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
        }

        if ( victim->description[0] != '\0' )
                send_to_char( victim->description, ch );
        else
                act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );

        if ( victim->max_hit > 0 )
                percent = ( 100 * victim->hit ) / victim->max_hit;
        else
                percent = -1;

        strcpy(buf , PERS( victim, ch ) );

        if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
        else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
        else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
        else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
        else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
        else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
        else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
        else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
        else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
        else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
        else                       strcat( buf, " is DYING.\n\r"              );

        buf[0] = UPPER( buf[0] );
        send_to_char( buf, ch );

        found = FALSE;

        if (IS_NPC(victim) || victim->pcdata->allow_look || ch->level > LEVEL_HERO)
        {
                for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
                {
                    if ( ( obj = get_eq_char( victim, equipment_slot_order[iWear] ) )
                        && can_see_obj( ch, obj ) )
                    {
                            if ( !found )
                            {
                                    send_to_char( "\n\r", ch );
                                    act( "$N is using:", ch, NULL, victim, TO_CHAR );
                                    found = TRUE;
                            }
                            send_to_char( where_name[equipment_slot_order[iWear]], ch );
                            send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
                            send_to_char( "\n\r", ch );
                    }
                }

                if ( victim != ch
                    && !IS_NPC( ch )
                    && IS_NPC( victim )
                    && number_percent( ) < ch->pcdata->learned[gsn_peek] )
                {
                        send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
                        show_list_to_char( victim->carrying, ch, TRUE, TRUE );
                }
        }

        return;
}


void show_char_to_char (CHAR_DATA *list, CHAR_DATA *ch)
{
        CHAR_DATA *rch;

        for (rch = list; rch; rch = rch->next_in_room)
        {
                if (rch->deleted || rch == ch)
                        continue;

                if (!IS_NPC(rch)
                    && IS_SET(rch->act, PLR_WIZINVIS)
                    && get_trust(ch) < get_trust(rch))
                        continue;

                if (rch->position == POS_DEAD)
                        continue;

                if (can_see(ch, rch))
                        show_char_to_char_0(rch, ch);

                else if (room_is_dark(ch->in_room)
                         && IS_AFFECTED(rch, AFF_INFRARED)
                         && (!IS_NPC(rch) || !IS_SET(rch->act, ACT_WIZINVIS_MOB)))
                        send_to_char("You see glowing red eyes watching YOU!\n\r", ch);
        }
}


bool check_blind( CHAR_DATA *ch )
{
        if ( IS_AFFECTED( ch, AFF_BLIND ) )
        {
                send_to_char( "You can't see a thing!\n\r", ch );
                return FALSE;
        }

        return TRUE;
}


void do_look( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        EXIT_DATA *pexit;
        char       buf  [ MAX_STRING_LENGTH ];
        char       arg1 [ MAX_INPUT_LENGTH  ];
        char       arg2 [ MAX_INPUT_LENGTH  ];
        char      *pdesc;
        int        door;

        if ( !IS_NPC( ch ) && !ch->desc )
                return;

        if ( ch->position < POS_SLEEPING )
        {
                send_to_char( "You can't see anything but stars!\n\r", ch );
                return;
        }

        if ( ch->position == POS_SLEEPING )
        {
                send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
                return;
        }

        if ( !check_blind( ch ) )
                return;

        if ( !IS_NPC( ch )
            && !IS_SET( ch->act, PLR_HOLYLIGHT )
            && ch->sub_class != SUB_CLASS_VAMPIRE
            && ch->form != FORM_FLY
            && room_is_dark( ch->in_room ) )
        {
                send_to_char( "It is pitch black ... \n\r", ch );
                show_char_to_char( ch->in_room->people, ch );
                return;
        }

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
        {
                /* 'look' or 'look auto' */
                ansi_color( GREY, ch );
                ansi_color( BOLD, ch );

                send_to_char( ch->in_room->name, ch );
                send_to_char( "\n\r", ch );

                ansi_color( NTEXT, ch );
                ansi_color( GREEN, ch );

                if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
                        do_exits( ch, "auto" );

                ansi_color( NTEXT, ch );
                ansi_color( GREY, ch );

                if ( arg1[0] == '\0'
                    || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
                        send_to_char( ch->in_room->description, ch );

                ansi_color( NTEXT, ch );

                show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
                show_char_to_char( ch->in_room->people,   ch );

                ansi_color( NTEXT, ch );

                return;
        }

        if ( !str_prefix( arg1, "in" ) )
        {
                /* 'look in' */
                if ( arg2[0] == '\0' )
                {
                        send_to_char( "Look in what?\n\r", ch );
                        return;
                }

                if ( !( obj = get_obj_here( ch, arg2 ) ) )
                {
                        send_to_char( "You do not see that here.\n\r", ch );
                        return;
                }

                switch ( obj->item_type )
                {
                    default:
                        send_to_char( "That is not a container.\n\r", ch );
                        break;

                    case ITEM_DRINK_CON:
                        if ( obj->value[1] <= 0 )
                        {
                                send_to_char( "It is empty.\n\r", ch );
                                break;
                        }

                        sprintf( buf, "It's %s full of a %s liquid.\n\r",
                                obj->value[1] <     obj->value[0] / 4
                                ? "less than" :
                                obj->value[1] < 3 * obj->value[0] / 4
                                ? "about"     : "more than",
                                liq_table[obj->value[2]].liq_color);

                        send_to_char( buf, ch );
                        break;

                    case ITEM_CONTAINER:
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                        if ( IS_SET( obj->value[1], CONT_CLOSED ) )
                        {
                                send_to_char( "It is closed.\n\r", ch );
                                break;
                        }

                        act( "$p contains:", ch, obj, NULL, TO_CHAR );
                        show_list_to_char( obj->contains, ch, TRUE, TRUE );
                        break;
                }
                return;
        }

        if ( ( victim = get_char_room( ch, arg1 ) ) )
        {
                show_char_to_char_1( victim, ch );
                return;
        }

        for ( obj = ch->carrying; obj; obj = obj->next_content )
        {
                if ( can_see_obj( ch, obj ) )
                {
                        pdesc = get_extra_descr( arg1, obj->extra_descr );
                        if ( pdesc )
                        {
                                send_to_char( pdesc, ch );
                                return;
                        }

                        pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
                        if ( pdesc )
                        {
                                send_to_char( pdesc, ch );
                                return;
                        }
                }

                if ( is_name( arg1, obj->name ) )
                {
                        send_to_char( obj->description, ch );
                        send_to_char( "\n\r", ch );
                        return;
                }
        }

        for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
        {
                if ( can_see_obj( ch, obj ) )
                {
                        pdesc = get_extra_descr( arg1, obj->extra_descr );
                        if ( pdesc )
                        {
                                send_to_char( pdesc, ch );
                                return;
                        }

                        pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
                        if ( pdesc )
                        {
                                send_to_char( pdesc, ch );
                                return;
                        }
                }

                if ( is_name( arg1, obj->name ) )
                {
                        send_to_char( obj->description, ch );
                        send_to_char( "\n\r", ch );
                        return;
                }
        }

        pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
        if ( pdesc )
        {
                send_to_char( pdesc, ch );
                return;
        }

        if ( !str_prefix( arg1, "north" ) ) door = 0;
        else if ( !str_prefix( arg1, "east"  ) ) door = 1;
        else if ( !str_prefix( arg1, "south" ) ) door = 2;
        else if ( !str_prefix( arg1, "west"  ) ) door = 3;
        else if ( !str_prefix( arg1, "up"    ) ) door = 4;
        else if ( !str_prefix( arg1, "down"  ) ) door = 5;
        else
        {
                send_to_char( "You do not see that here.\n\r", ch );
                return;
        }

        /* 'look direction' */
        if ( !( pexit = ch->in_room->exit[door] ) )
        {
                send_to_char( "Nothing special there.\n\r", ch );
                return;
        }

        if ( pexit->description && pexit->description[0] != '\0' )
                send_to_char( pexit->description, ch );
        else
                send_to_char( "Nothing special there.\n\r", ch );

        if (   pexit->keyword
            && pexit->keyword[0] != '\0'
            && pexit->keyword[0] != ' ' )
        {
                if ( IS_SET( pexit->exit_info, EX_BASHED ) )
                        act( "The $d has been bashed from its hinges.",
                            ch, NULL, pexit->keyword, TO_CHAR );
                else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
                        act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
                else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
                        act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
        }

        return;
}


void do_examine (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char buf [MAX_STRING_LENGTH];
        char tmp [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];
        int i;
        bool found;

        const char* coin [] =
        {
                "copper",
                "silver",
                "gold",
                "platinum"
        };

        one_argument( argument, arg );

        if (arg[0] == '\0')
        {
                send_to_char ("Examine what?\n\r", ch);
                return;
        }

        do_look (ch, arg);

        if ((obj = get_obj_here (ch, arg)))
        {
                if (!IS_NPC(ch) && IS_SET(obj->extra_flags, ITEM_TRAP)
                    && number_percent() < ch->pcdata->learned[gsn_find_traps])
                {
                        send_to_char( "You believe that it is trapped!!\n\r", ch );
                        return;
                }

                switch (obj->item_type)
                {
                    case ITEM_DRINK_CON:
                    case ITEM_CONTAINER:
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                        send_to_char ("When you look inside, you see:\n\r", ch);
                        sprintf (buf, "in %s", arg);
                        do_look (ch, buf);
                        break;

                    case ITEM_MONEY:
                        found = FALSE;
                        strcpy (buf, "You see");

                        for (i = 3; i >= 0; i--)
                        {
                                if (obj->value[i] > 0)
                                {
                                        if (found)
                                                strcat (buf, ",");
                                        found = TRUE;
                                        sprintf (tmp, " %d %s coin%s",
                                                 obj->value[i],
                                                 coin[i],
                                                 (obj->value[i] == 1) ? "" : "s");
                                        strcat (buf, tmp);
                                }
                        }

                        if (found)
                        {
                                strcat (buf, ".\n\r");
                                send_to_char (buf, ch);
                        }

                    case ITEM_WEAPON:
                        if (obj->timer > 0 && obj->timer < 11)
                        {
                                if (IS_SET(obj->extra_flags, ITEM_POISONED))
                                        send_to_char("Poison is beginning to seriously corrode the blade.\n\r", ch);

                                else if (IS_SET(obj->extra_flags, ITEM_SHARP)
                                         || IS_SET(obj->extra_flags, ITEM_BLADE_THIRST))
                                        send_to_char("The blade's condition is deteriorating badly.\n\r", ch);
                        }
                        break;

                    case ITEM_ARMOR:
                        if (obj->timer > 0 && obj->timer < 11
                            && IS_SET(obj->extra_flags, ITEM_FORGED))
                                send_to_char("The forged metal looks to be in poor condition.\n\r", ch);
                        break;
                }
        }
}


/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
        EXIT_DATA       *pexit;
        char             buf      [ MAX_STRING_LENGTH ];
        int              door;
        bool             found;
        bool             fAuto;

        buf[0] = '\0';
        fAuto  = !str_cmp( argument, "auto" );

        if ( !check_blind( ch ) )
                return;

        strcpy(buf,  fAuto ? "[Exits:" : "Obvious exits:\n\r" );

        found = FALSE;
        for ( door = 0; door <= 5; door++ )
        {
                if ( ( pexit = ch->in_room->exit[door] )
                    && pexit->to_room
                    && !IS_SET( pexit->exit_info, EX_CLOSED )
                    &&  !IS_SET( pexit->exit_info, EX_SECRET ))
                {
                        found = TRUE;
                        if ( fAuto )
                        {
                                strcat( buf, " " );
                                strcat( buf, directions[door].name );
                        }
                        else
                        {
                                sprintf( buf + strlen( buf ), "%-5s - %s\n\r",
                                        capitalize( directions[door].name ),
                                        room_is_dark( pexit->to_room )
                                        ?  "Too dark to tell"
                                        : pexit->to_room->name);
                        }
                }
        }

        if ( !found )
                strcat( buf, fAuto ? " none" : "None.\n\r" );

        if ( fAuto )
                strcat( buf, "]{x\n\r" );

        send_to_char( buf, ch );
        return;
}


void do_cscore( CHAR_DATA *ch, char *argument )
{
        char         buf  [ MAX_STRING_LENGTH ];
        char         buf1 [ MAX_STRING_LENGTH ];
        int          age_hours;
        
        age_hours = (get_age( ch ) - 17) * 4;

        ansi_color( NTEXT, ch );

        if (IS_NPC(ch))
                return;

        if (ch->clan == 0 )
        {
                send_to_char("You are not in a clan.\n\r", ch );
                return;
        }

        buf1[0] = '\0';

        sprintf( buf, "You are %s%s, level %d, %d years old (%d hours).\n\r",
                ch->name,
                IS_NPC( ch ) ? "" : ch->pcdata->title,
                ch->level,
                get_age( ch ),
                age_hours);
        strcat( buf1, buf );
        
        sprintf( buf, "You belong to clan %s (%s).\n\r",
                clan_table[ch->clan].who_name,
                clan_table[ch->clan].name );
        strcat( buf1, buf );

        if (ch->clan_level)
        {
                sprintf( buf, "You are ranked %s(level %d).\n\r",
                        (clan_title [ch->clan] [ch->clan_level] ),
                        ch->clan_level );
                strcat( buf1, buf );
        }

        sprintf (buf, "You have killed %d other player%s for a total of %d point%s.\n\r",
                 ch->pcdata->pkills,
                 (ch->pcdata->pkills == 1) ? "" : "s",
                 ch->pcdata->pkscore,
                 (ch->pcdata->pkscore == 1) ? "" : "s");
        strcat (buf1, buf);

        sprintf (buf, "You have been killed %d time%s by other players.\n\r",
                 ch->pcdata->pdeaths,
                 (ch->pcdata->pdeaths == 1) ? "" : "s");
        strcat (buf1, buf);

        sprintf(buf, "You have a final pkill score of %d point%s.\n\r",
                ch->pcdata->pkscore - 2 * ch->pcdata->pdeaths,
                (ch->pcdata->pkscore - 2 * ch->pcdata->pdeaths == 1) ? "" : "s");
        strcat(buf1, buf);

        send_to_char( buf1, ch );
}


/*
 *  New score command - hopefully :) - Brutus - idea by Rufus
 *  Cleaned up Dec 99 by Gezhp
 */
void do_score (CHAR_DATA *ch, char *argument)
{
        char    buf  [ MAX_STRING_LENGTH ];
        char    buf1 [ MAX_STRING_LENGTH ];
        extern char* size_names[];

        /*
         *  Experience bar.  `full_length' is length of bar in chars;
         *  string `bar' has each printed symbol preceded by ANSI colour
         *  escape sequence; string `empty' is colour and symbol for
         *  unfilled length of the bar.
         */

        int             bar_length, bar_left, i, full_length = 30;
        const char      bar [] = "{m|{m|{m|{m|{m|{B|{B|{B|{B|{B|{C|{C|{C|{C|{C|"
                                 "{G|{G|{G|{G|{G|{Y|{Y|{Y|{Y|{Y|{R|{R|{R|{R|{R|";
        const char      empty [] = "{w:";


        if( IS_NPC(ch) )
                return;

        ansi_color( NTEXT, ch );
        buf1[0] = '\0'; /* buf1 sent to char at end */

        sprintf( buf, "You are {W%s{x%s, of the %s profession.\n\r",
                ch->name, ch->pcdata->title,
                ch->sub_class ? full_sub_class_name( ch->sub_class )
                : full_class_name( ch->class ) );
        strcat( buf1, buf );

        if( get_trust(ch) != ch->level )
        {
                sprintf( buf, "You are trusted to level %d (%s).\n\r",
                        get_trust(ch), extra_level_name(ch) );
                strcat( buf1, buf );
        }

        sprintf( buf, "You are a %s %s.  ",
                size_names[race_table[ch->race].size],
                race_table[ch->race].race_name );
        strcat( buf1, buf );

        if ( !ch->clan )
                sprintf( buf, "You don't belong to any clan.\n\r" );
        else
                sprintf( buf, "You belong to clan {%c%s{x (%s).\n\r",
                        clan_table[ch->clan].colour_code,
                        clan_table[ch->clan].who_name,
                        clan_table[ch->clan].name );
        strcat( buf1, buf );

        if (deities_active())
        {
                if (ch->pcdata->deity_patron < 0
                    || ch->pcdata->deity_patron >= NUMBER_DEITIES)
                        strcpy (buf, "You have not chosen a patron deity.\n\r");
                else
                        sprintf (buf, "You worship the mighty god {W%s{x as your patron.\n\r",
                                deity_info_table[ch->pcdata->deity_patron].name);
                strcat (buf1, buf);
        }

        if(ch->pcdata->bounty > 0)
        {
                sprintf( buf, "You have a bounty of {R%d{x platinum coins on your head.\n\r",
                        ch->pcdata->bounty/1000 );
                strcat( buf1, buf );
        }

        sprintf( buf, "You have slaughtered {R%d{x creature%s, and {R%d{x player%s scoring "
                "{R%d{x point%s.\n\rYou've been killed {R%d{x time%s by other players and "
                "{R%d{x time%s in total.\n\r",
                ch->pcdata->kills,
                ch->pcdata->kills == 1 ? "" : "s",
                ch->pcdata->pkills,
                ch->pcdata->pkills == 1 ? "" : "s",
                ch->pcdata->pkscore,
                ch->pcdata->pkscore == 1 ? "" : "s",
                ch->pcdata->pdeaths,
                ch->pcdata->pdeaths == 1 ? "" : "s",
                ch->pcdata->killed,
                ch->pcdata->killed == 1 ? "" : "s" );
        strcat( buf1, buf );

        sprintf( buf, "You are {W%d{x years old, after {W%d{x hours of playing time.\n\r",
                get_age(ch), (get_age(ch)-17)*4 );
        strcat( buf1, buf );

        sprintf( buf, "Autoexit: %s  Autoloot: %s  Autocoin: %s\n\r",
                IS_SET(ch->act, PLR_AUTOEXIT)   ? "{GYES {x" : "{RNO  {x",
                IS_SET(ch->act, PLR_AUTOLOOT)   ? "{GYES {x" : "{RNO  {x",
                IS_SET(ch->act, PLR_AUTOCOIN)   ? "{GYES{x" : "{RNO {x");
        strcat( buf1, buf );

        sprintf( buf, "Autosac: %s  Autowield: %s  Autolevel: %s\n\r\n\r",
                IS_SET(ch->act, PLR_AUTOSAC)    ? "{G YES{x" : "{R NO {x",
                IS_SET(ch->act, PLR_AUTOWIELD)  ? "{GYES{x" : "{RNO {x",
                IS_SET(ch->act, PLR_AUTOLEVEL)  ? "{GYES{x" : "{RNO {x" );
        strcat( buf1, buf );

        print_player_status (ch, buf);
        strcat (buf1, buf);

        sprintf( buf, "\n\rPhysical pracs: {W%d{x.  Intellectual pracs: {W%d{x.\n\r"
                "You have earned {W%d{x quest points and can spend {W%d{x.\n\r",
                ch->pcdata->str_prac,
                ch->pcdata->int_prac,
                ch->pcdata->totalqp,
                ch->pcdata->questpoints );
        strcat( buf1, buf );

        if( ch->level >= LEVEL_HERO )
        {
                sprintf( buf, "You are level {W%d{x.  You can no longer gain "
                        "experience.\n\r", ch->level );
                strcat( buf1, buf );
        }
        else
        {
                sprintf( buf, "You are level {W%d{x, have {W%d{x experience "
                        "and need {W%d{x to level.\n\r",
                        ch->level, ch->exp,
                        (level_table[ch->level].exp_total)-ch->exp );
                strcat( buf1, buf );

                /*
                 *  Experience progress bar
                 *  Originally included by Brutus?
                 *  Rewritten by Gezhp 99
                 */

                bar_length = (full_length * (ch->exp - (level_table[ch->level-1].exp_total)))
                        / ((level_table[ch->level].exp_total) - (level_table[ch->level-1].exp_total) );

                if( bar_length >= full_length )
                {
                        bar_length = full_length;
                        bar_left = 0;
                }
                else if( bar_length < 1 )
                        bar_left = full_length;
                else
                        bar_left = full_length - bar_length;

                strcat( buf1, "Experience gained this level: {x{w[" );
                strncat( buf1, bar, bar_length*3 );
                strcat( buf1, "{x" );
                for( i=0; i < bar_left; i++ )
                        strcat( buf1, empty );
                strcat( buf1, "{x{w]{x\n\r" );
        }

        sprintf(buf, "Coin:  Platinum: {W%d{x  Gold: {Y%d{x  Silver: {w%d{x  Copper: {y%d{x\n\r",
                ch->plat, ch->gold, ch->silver, ch->copper);
        strcat(buf1, buf);

        sprintf( buf, "You're carrying {C%d{x/{c%d{x items, or {C%d{x/{c%d{x lbs in weight.\n\r",
                ch->carry_number, can_carry_n( ch ), (ch->carry_weight +
                ch->coin_weight), can_carry_w( ch ) );
        strcat( buf1, buf );

        if( ch->level >= 20 )
        {
                sprintf( buf, "Save vs magic: {W%d{x.  ", ch->saving_throw );
                strcat( buf1, buf );
        }

        strcat( buf1, "You ");
        if( ch->saving_throw >= 5 )
                strcat( buf1, "attract magic!\n\r" );
        else if( ch->saving_throw >= 0 )
                strcat( buf1, "feel vulnerable to magic.\n\r" );
        else if( ch->saving_throw >= -10 )
                strcat( buf1, "feel somewhat protected against magic attacks.\n\r" );
        else if( ch->saving_throw >= -20 )
                strcat( buf1, "have an inherent magical protection.\n\r" );
        else if ( ch->saving_throw >= -40 )
                strcat( buf1, "have a strong magical resistance\n\r" );
        else strcat( buf1, "are a magic TANK!!\n\r" );

        if( ch->level >= 20 )
        {
                sprintf( buf, "Armour class: {W%d{x.  ", GET_AC( ch ) );
                strcat( buf1, buf );
        }

        strcat( buf1, "You are " );
        if( GET_AC( ch ) >= 101 ) strcat( buf1, "WORSE than naked!\n\r" );
        else if( GET_AC( ch ) >= 75 ) strcat( buf1, "barely clothed.\n\r"   );
        else if( GET_AC( ch ) >= 50 ) strcat( buf1, "wearing clothes.\n\r"  );
        else if( GET_AC( ch ) >= 25 ) strcat( buf1, "slightly armored.\n\r" );
        else if( GET_AC( ch ) >= 0 ) strcat( buf1, "somewhat armored.\n\r" );
        else if( GET_AC( ch ) >= - 25) strcat( buf1, "armored.\n\r" );
        else if( GET_AC( ch ) >= - 50 ) strcat( buf1, "well armored.\n\r"     );
        else if( GET_AC( ch ) >= - 75 ) strcat( buf1, "strongly armored.\n\r" );
        else if ( GET_AC( ch ) >= - 100 ) strcat( buf1, "heavily armored.\n\r"  );
        else if ( GET_AC( ch ) >= - 150 ) strcat( buf1, "superbly armored.\n\r" );
        else if ( GET_AC( ch ) >= - 200 ) strcat( buf1, "divinely armored.\n\r" );
        else if ( GET_AC( ch ) >= - 250 ) strcat( buf1, "invincible\n\r" );
        else if ( GET_AC( ch ) >= - 300 ) strcat( buf1, "untouchable!\n\r" );
        else if ( GET_AC( ch ) >= - 400 ) strcat( buf1, "invulnerable!!\n\r" );
        else if ( GET_AC( ch ) >= - 500 ) strcat( buf1, "protected by the GODS!\n\r" );
        else strcat( buf1, "armoured beyond words...\n\r" );

        sprintf( buf, "Your fame: {W%d{x.  ", ch->pcdata->fame );
        strcat( buf1, buf );

        strcat( buf1, "You are " );
        if ( ch->pcdata->fame < 10 )          strcat( buf1, "nobody.\n\r" );
        else if ( ch->pcdata->fame < 30 )     strcat( buf1, "unknown.\n\r" );
        else if ( ch->pcdata->fame < 50 )     strcat( buf1, "a prince among bums.\n\r");
        else if ( ch->pcdata->fame < 100 )    strcat( buf1, "recognisable.\n\r" );
        else if ( ch->pcdata->fame < 250 )    strcat( buf1, "familiar.\n\r" );
        else if ( ch->pcdata->fame < 500 )    strcat( buf1, "well known.\n\r" );
        else if ( ch->pcdata->fame < 1000 )   strcat ( buf1, "mildly famous.\n\r" );
        else if ( ch->pcdata->fame < 1500 )   strcat ( buf1, "famous.\n\r" );
        else if ( ch->pcdata->fame < 2500 )   strcat ( buf1, "popular.\n\r" );
        else if ( ch->pcdata->fame < 3500 )   strcat ( buf1, "very famous.\n\r" );
        else if ( ch->pcdata->fame < 5000 )   strcat ( buf1, "a minor legend.\n\r" );
        else if ( ch->pcdata->fame < 6500 )   strcat ( buf1, "infamous.\n\r" );
        else if ( ch->pcdata->fame < 8000 )   strcat ( buf1, "a legend.\n\r" );
        else if ( ch->pcdata->fame < 10000 )  strcat ( buf1, "a major legend.\n\r");
        else strcat ( buf1, "a GOD amongst mortals.\n\r" );

        if( ch->level >= 10 )
        {
                sprintf( buf, "Alignment: {W%d{x.  ", ch->alignment );
                strcat( buf1, buf );
        }

        strcat( buf1, "You are " );
        if ( ch->alignment >  900 ) strcat( buf1, "angelic.\n\r" );
        else if ( ch->alignment >  700 ) strcat( buf1, "saintly.\n\r" );
        else if ( ch->alignment >  350 ) strcat( buf1, "good.\n\r"    );
        else if ( ch->alignment >  100 ) strcat( buf1, "kind.\n\r"    );
        else if ( ch->alignment > -100 ) strcat( buf1, "neutral.\n\r" );
        else if ( ch->alignment > -350 ) strcat( buf1, "mean.\n\r"    );
        else if ( ch->alignment > -700 ) strcat( buf1, "evil.\n\r"    );
        else if ( ch->alignment > -900 ) strcat( buf1, "demonic.\n\r" );
        else strcat( buf1, "satanic.\n\r" );

        if (ch->pcdata->condition[COND_DRUNK] > 1)
                strcat(buf1, "You are drunk.\n\r");

        if (ch->pcdata->condition[COND_THIRST] < 1 && ch->level < LEVEL_HERO)
                strcat(buf1, "You are thirsty.\n\r");

        if (ch->pcdata->condition[COND_FULL] < 1 && ch->level < LEVEL_HERO)
                strcat(buf1, "You are hungry.\n\r");

        /*
         *  Some special messages
         */
        if ( ch->level == 30 && !ch->pcdata->choose_subclass )
        {
                sprintf( buf, "\n\r{W--*>> You are ready to choose a {Y*NEW*{W class! <<*--{x\n\r" );
                strcat( buf1, buf );
                sprintf(buf, "You may specialise in the ways of the %s, %s or %s.\n\r",
                        full_sub_class_name(ch->class * 2 + 1),
                        full_class_name(ch->class),
                        full_sub_class_name(ch->class * 2 + 2));
                strcat( buf1, buf );
        }

        if (ch->level == 40
            && !ch->clan
            && !IS_SET(ch->status, PLR_RONIN)
            && !IS_SET(ch->status, PLR_LEFT_CLAN))
                strcat(buf1, "\n\r{WThis is the last level that you can join a clan.{x\n\r");

        if (ch->level == 40 && ch->clan)
                strcat(buf1, "\n\r{WThis is the last level that you can use the default Midgaard recall.{x\n\r");

        /* Quest point message hack */
        if (ch->level == 49 && ch->pcdata->totalqp < 1)
                strcat(buf1, "\n\r{RYou need at least 1 quest point before you can reach level 50.{x\n\r");

        if (ch->level == 69 && ch->pcdata->totalqp < 200)
                strcat(buf1, "\n\r{RYou need at least 200 total quest points before you can reach level 70.{x\n\r");

        if (ch->level == 99 && ch->pcdata->totalqp < 500)
                strcat(buf1, "\n\r{RYou need at least 500 total quest points before you can reach level 100.{x\n\r");

        send_to_char(buf1, ch);
}


void do_fixmoney( CHAR_DATA *ch, char *argument )
{
        return;
}


void do_fixexp( CHAR_DATA *ch, char *argument )
{
        int     level_percent;
        int     extra_gain;
        int     modified_gain;
        int     extra_xp;
        int     needed_xp;

        if (ch->level < 2 || ch->level == 99)
                return;

        if ( ( ch->exp >= level_table[ch->level-1].exp_total ) &&
            ( ch->exp <= level_table[ch->level].exp_total ) )
        {
                send_to_char ("Your exp is correct.\n\r", ch );
                return;
        }

        if ( ch->level >= LEVEL_IMMORTAL )
        {
                send_to_char("Immortals don't need to fix exp!\n\r",ch);
                return;
        }


        /*find out what % of xp towards their next level the char has */

        needed_xp = (old_level_table[ch->level].exp_level);
        extra_xp = (ch->exp - old_level_table[ch->level].exp_total);
        level_percent = ( (extra_xp * 100) / needed_xp );
        extra_gain = ( (level_percent) * ( level_table[ch->level].exp_level/100 ) );
        modified_gain = extra_gain;

        if ( extra_gain <= 0 )
                modified_gain = 1;

        if ( (level_table[ch->level].exp_total + extra_gain)
            > (level_table[ch->level+1].exp_total) )
                modified_gain = (level_table[ch->level].exp_level - 1);

        ch->exp = (level_table[ch->level-1].exp_total );
        ch->exp = ( ch->exp + modified_gain );

        send_to_char("You feel better experienced.\n\r",ch);

        return;
}


void do_fixnote( CHAR_DATA *ch, char *argument )
{
        int i;
        send_to_char( "Note list has been reset due to corruptions.\n\r",ch);
        for (i=0;i<MAX_BOARD;i++)
                ch->pcdata->last_note[i] = 0;
}


void do_affects( CHAR_DATA *ch, char *argument )
{
        AFFECT_DATA *paf;
        char buf [MAX_STRING_LENGTH ];
        char buf1 [ MAX_STRING_LENGTH ];
        bool printed = FALSE;

        buf1[0] = '\0';

        if ( ch->affected )
        {
                for ( paf = ch->affected; paf; paf = paf->next )
                {
                        if ( paf->deleted )
                                continue;

                        if ( !printed )
                        {
                                strcat( buf1, "You are affected by:\n\r" );
                                printed = TRUE;
                        }

                        sprintf( buf, "{W%-25s{x", skill_table[paf->type].name );
                        strcat( buf1, buf );

                        if ( ch->level >= 20 )
                        {
                                if( paf->location != APPLY_NONE )
                                {
                                        sprintf( buf, " modifies {G%s{x by {Y%d{x",
                                                affect_loc_name( paf->location ),
                                                paf->modifier );
                                        strcat( buf1, buf );
                                }
                                else if (paf->bitvector
                                         && paf->bitvector != AFF_HIDE
                                         && paf->bitvector != AFF_SNEAK)
                                {
                                        sprintf( buf, " gives you {G%s{x",
                                                affect_bit_name_nice( paf->bitvector ));
                                        strcat( buf1, buf );
                                }

                                if( paf->duration > 1 )
                                {
                                        sprintf( buf, " for {G%d{x hours", paf->duration );
                                        strcat( buf1, buf );
                                }
                                else if( paf->duration == 1 )
                                {
                                        sprintf( buf, " for {G%d{x hour", paf->duration );
                                        strcat( buf1, buf );
                                }
                                else if( paf->duration == 0 )
                                {
                                        sprintf( buf, " for less than an hour");
                                        strcat( buf1, buf );
                                }
                                else strcat( buf1, " indefinitely" );
                        }
                        strcat( buf1, "\n\r" );
                }
        }

        if (ch->form == FORM_CHAMELEON)
        {
                if (!printed)
                {
                        strcat( buf1, "You are affected by:\n\r" );
                        printed = TRUE;
                }

                sprintf( buf, "{Whide{x" );
                strcat( buf1, buf );

                if (ch->level >= 20)
                        strcat (buf1, "               until you change form");

                strcat( buf1, "\n\r" );
        }

        if( printed )
                send_to_char( buf1, ch );
        else
                send_to_char( "You are not affected in any way.\n\r", ch );

        return;
}


void do_levels( CHAR_DATA *ch, char *argument )
{
        int clevel = 0;
        char buf [ MAX_STRING_LENGTH ];
        char buf1 [ MAX_STRING_LENGTH ];

        buf1[0] = '\0';

        if (ch->level >= LEVEL_HERO)
        {
                send_to_char("You have no use for experience now, you are immortal!\n\r",ch);
                return;
        }

        for (clevel = UMAX (0, ch->level - 1);
             clevel < UMIN (LEVEL_HERO, ch->level + 10);
             clevel++)
        {
                sprintf( buf, "Level %3d:    Total exp needed: %8d", level_table[clevel].level, level_table[clevel].exp_total );
                strcat( buf1, buf );

                if (clevel+1 == ch->level)
                {
                        sprintf( buf, "\n\r{WYour current experience ------> %8d{x", ch->exp);
                        strcat( buf1, buf );
                }

                strcat ( buf1, "\n\r" );
        }

        send_to_char( buf1, ch );
}


char *  const   day_name [] =
{
        "the Moon",
        "the Bull",
        "Deception",
        "Thunder",
        "Freedom",
        "the Great Gods",
        "the Sun"
};


char *  const   month_name [] =
{
        "Winter",
        "the Winter Wolf",
        "the Frost Giant",
        "the Old Forces",
        "the Grand Struggle",
        "the Spring",
        "Nature",
        "Futility",
        "the Dragon",
        "the Sun",
        "the Heat",
        "the Battle",
        "the Dark Shades",
        "the Shadows",
        "the Long Shadows",
        "the Ancient Darkness",
        "the Great Evil"
};


void do_time( CHAR_DATA *ch, char *argument )
{
        char  buf  [ MAX_STRING_LENGTH ];
        extern char  str_boot_time[];
        char *suf;
        int   day;
        char *moon;
        int   days, hours, minutes, seconds;

        day = time_info.day + 1;

        if ( day > 4 && day <  20 )
                suf = "th";
        else if ( day % 10 ==  1 )
                suf = "st";
        else if ( day % 10 ==  2 )
                suf = "nd";
        else if ( day % 10 ==  3 )
                suf = "rd";
        else
                suf = "th";

        if ( weather_info.moonlight == MOON_NONE )
                moon = "new";
        else if ( weather_info.moonlight == MOON_WAXING )
                moon = "waxing";
        else if ( weather_info.moonlight == MOON_FULL )
                moon = "full";
        else
                moon = "waning";

        sprintf( buf, "It is %d o'clock %s, Day of %s, %d%s of the Month of %s.\n\r",
                ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
                time_info.hour >= 12 ? "pm" : "am",
                day_name[day % 7],
                day, suf,
                month_name[time_info.month] );
        send_to_char( buf, ch );

        sprintf( buf, "The moon is currently %s.\n\r", moon );
        send_to_char( buf, ch );

        sprintf( buf, "DD4 was started at %s\rThe system time is %s\r",
                str_boot_time,
                (char *) ctime( &current_time ) );
        send_to_char( buf, ch );


        /*  Calculate uptime; Gezhp 2000  */

        seconds = (current_time - boot_time) / 60;
        days = 0;
        hours = 0;
        minutes = 0;

        if (seconds)
        {
                minutes = seconds % 60;
                if (seconds >= 60)
                {
                        seconds /= 60;
                        hours = seconds % 24;
                        if (seconds >= 24)
                                days = seconds / 24;
                }
        }

        if (days)
        {
                sprintf (buf, "DD has been running for %d day%s and %d hour%s.\n\r",
                         days,
                         days == 1 ? "" : "s",
                         hours,
                         hours == 1 ? "" : "s");
        }
        else if (hours)
        {
                sprintf (buf, "DD has been running for %d hour%s and %d minute%s.\n\r",
                         hours,
                         hours == 1 ? "" : "s",
                         minutes,
                         minutes == 1 ? "" : "s");
        }
        else if (minutes)
        {
                sprintf (buf, "DD has been running for %d minute%s.\n\r",
                         minutes,
                         minutes == 1 ? "" : "s");
        }
        else
                sprintf (buf, "DD has been running for less than a minute.\n\r");

        send_to_char (buf, ch);

        return;
}


void do_weather( CHAR_DATA *ch, char *argument )
{
        char buf [ MAX_STRING_LENGTH ];

        static char * const sky_look[ 4 ] =
        {
                "cloudless",
                "cloudy",
                "rainy",
                "lit by flashes of lightning"
        };

        if ( !IS_OUTSIDE( ch ) )
        {
                send_to_char( "You can't see the weather indoors.\n\r", ch );
                return;
        }

        sprintf( buf, "The sky is %s and %s.\n\r",
                sky_look[weather_info.sky],
                weather_info.change >= 0
                ? "a warm southerly breeze blows"
                : "a cold northern gust blows" );
        send_to_char( buf, ch );
        return;
}


void do_help( CHAR_DATA *ch, char *argument )
{
        HELP_DATA *pHelp;
        char buf [MAX_STRING_LENGTH];

        if (argument[0] == '\0')
                argument = "summary";

        for (pHelp = help_first; pHelp; pHelp = pHelp->next)
        {
                if (pHelp->level > get_trust(ch))
                        continue;

                if (is_name(argument, pHelp->keyword))
                {
                        if (pHelp->level >= 0 && str_cmp(argument, "imotd"))
                        {
                                sprintf(buf, "{c[Help for: %s]{x\n\r\n\r",
                                        pHelp->keyword);
                                send_paragraph_to_char(buf, ch, 11);
                        }

                        /* Strip leading '.' to allow initial blanks */
                        if ( pHelp->text[0] == '.' )
                                send_to_char( pHelp->text+1, ch );
                        else
                                send_to_char( pHelp->text  , ch );

                        return;
                }
        }

        sprintf(buf, "There is no help entry for '%s'.\n\r", argument);
        send_to_char(buf, ch);
}


/*
 *  Routines for printing 'who' information for individual characters
 *  moved to print_who_data() for reuse by 'whois'; Gezhp 2000.
 */
void do_who (CHAR_DATA *ch, char *argument)
{
        DESCRIPTOR_DATA *d;
        char buf [MAX_STRING_LENGTH * 3];
        char buf2 [MAX_STRING_LENGTH];
        char buf_imm [MAX_STRING_LENGTH];
        char buf_hero [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];
        int iClass = -1;
        int iClan = 0;
        int iLevelLower = 0;
        int iLevelUpper = L_IMM;
        int count = 0;
        int nNumber = 0;
        int nMatch = 0;
        int nImm = 0;
        int nHero = 0;
        int nMort = 0;
        bool fImmortal = FALSE;
        bool fTarget = FALSE;
        bool fClan = FALSE;
        bool fClass = FALSE;

        while (1)
        {
                argument = one_argument(argument, arg);

                if (arg[0] == '\0')
                        break;

                if (!str_cmp(arg, "clan"))
                {
                        fClan = TRUE;
                        one_argument(argument, arg);

                        if (arg[0] != '\0')
                        {
                                iClan = clan_lookup(arg);

                                if (iClan == -1)
                                {
                                        send_to_char("There is no such clan.\n\r", ch);
                                        return;
                                }
                        }

                        break;
                }

                if (!str_cmp(arg, "target"))
                {
                        fTarget = TRUE;
                        break;
                }

                if (is_name(arg, "immortal"))
                {
                        fImmortal = TRUE;
                        break;
                }

                if (is_number(arg))
                {
                        switch (++nNumber)
                        {
                            case 1:
                                iLevelLower = atoi(arg);
                                break;

                            case 2:
                                iLevelUpper = atoi(arg);
                                break;

                            default:
                                send_to_char ("Only two level numbers allowed.\n\r", ch);
                                return;
                        }
                }

                else
                {
                        int i;

                        for (i = 0; i < MAX_CLASS; i++)
                        {
                                if (!str_cmp(arg, class_table[i].show_name)
                                    || !str_cmp(arg, class_table[i].who_name))
                                {
                                        iClass = i;
                                        fClass = TRUE;
                                        break;
                                }
                        }

                        if (iClass < 0)
                        {
                                send_to_char("That's not a valid class.\n\r", ch);
                                return;
                        }
                }
        }


        strcpy(buf, "Mortals in the Dragons Domain\n\r-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r");
        strcpy(buf_hero, "Heroes of the Dragons Domain\n\r-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r");
        strcpy(buf_imm, "\n\rImmortals of the Dragons Domain\n\r-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r");

        for (d = descriptor_list; d; d = d->next)
        {
                CHAR_DATA *wch;

                wch = (d->original) ? d->original : d->character;

                if ((d->connected != CON_PLAYING
                     && (d->connected < CON_NOTE_TO || d->connected > CON_NOTE_FINISH))
                    || !wch
                    || !can_see(ch, wch))
                        continue;

                if (wch->level < iLevelLower
                    || wch->level > iLevelUpper
                    || (fImmortal && wch->level < LEVEL_HERO)
                    || (fClass && wch->class != iClass)
                    || (fClan && (!wch->clan || wch->level > LEVEL_HERO))
                    || (fClan && iClan && iClan != wch->clan))
                {
                        continue;
                }

                if (fTarget
                    && (ch == wch
                        || wch->level > LEVEL_HERO
                        || !(ch->clan || IS_SET(ch->status, PLR_RONIN))
                        || !(wch->clan || IS_SET(wch->status, PLR_RONIN))
                        || (ch->clan && ch->clan == wch->clan)
                        || !in_pkill_range(ch, wch)))
                {
                        continue;
                }

                nMatch++;
                print_who_data(wch, buf2);

                if (wch->level < LEVEL_HERO)
                {
                        strcat(buf, buf2);
                        nMort++;
                }
                else if (wch->level == LEVEL_HERO)
                {
                        strcat(buf_hero, buf2);
                        nHero++;
                }
                else
                {
                        strcat(buf_imm, buf2);
                        nImm++;
                }
        }

        /* The following is for the maximum players - taken from the ROM code */
        count = 0;

        for (d = descriptor_list; d; d = d->next)
        {
                if ((d->connected == CON_PLAYING
                     || (d->connected >= CON_NOTE_TO && d->connected <= CON_NOTE_FINISH))
                     && can_see (ch, d->character))
                        count ++;
        }

        max_on = UMAX (count, max_on);

        if (nMatch)
        {
                sprintf (buf2, "You see %d player%s in the game. The most this session was %d player%s.\n\r",
                         nMatch, nMatch == 1 ? "" : "s",
                         max_on, max_on == 1 ? "" : "s");
        }
        else
        {
                send_to_char("Nobody matches.\n\r", ch);
                return;
        }

        if (!nImm)
                buf_imm[0] = '\0';
        else
                strcat(buf_imm, "\n\r");

        if (!nHero)
                buf_hero[0] = '\0';
        else
                strcat(buf_hero, "\n\r");

        if (!nMort)
                buf[0] = '\0';

        strcat(buf_imm, buf_hero);
        strcat(buf_imm, buf);
        strcat(buf_imm, buf2);
        send_to_char(buf_imm, ch);
}


/*
 * Contributed by Kaneda
 */
void do_whois (CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
   char             buf  [ MAX_STRING_LENGTH  ];
   char             name [ MAX_INPUT_LENGTH   ];

   argument = one_argument( argument, name );

   if( name[0] == '\0' )
   {
           send_to_char( "Usage: whois <name>\n\r", ch );
           return;
   }

   name[0] = UPPER( name[0] );
   buf[0] = '\0';

   for( d = descriptor_list ; d ; d = d->next )
   {
           CHAR_DATA       *wch;

           wch = ( d->original ) ? d->original : d->character;

           if( (d->connected != CON_PLAYING
                && (d->connected < CON_NOTE_TO || d->connected > CON_NOTE_FINISH))
              || !can_see( ch, wch ) )
                   continue;

           if( str_prefix( name, wch->name ) )
                   continue;

           print_who_data(wch, buf);

           if ( wch->level < LEVEL_IMMORTAL )
                   sprintf( buf + strlen( buf ),
                           "Has %d fame, slaughtered %d mob%s, trounced %d player%s scoring %d point%s.\n\r"
                           "Has been nailed %d time%s by other players and perished %d time%s.\n\r"
                           "Has a final pkill score of %d point%s.\n\r",
                           wch->pcdata->fame,
                           wch->pcdata->kills,
                           wch->pcdata->kills == 1 ? "" : "s",
                           wch->pcdata->pkills,
                           wch->pcdata->pkills == 1 ? "" : "s",
                           wch->pcdata->pkscore,
                           wch->pcdata->pkscore == 1 ? "" : "s",
                           wch->pcdata->pdeaths,
                           wch->pcdata->pdeaths == 1 ? "" : "s",
                           wch->pcdata->killed,
                           wch->pcdata->killed == 1 ? "" : "s",
                           wch->pcdata->pkscore - 2 * wch->pcdata->pdeaths,
                           (wch->pcdata->pkscore - 2 * wch->pcdata->pdeaths) == 1 ? "" : "s");

           if (wch->pcdata->bounty)
           {
                   send_to_char (buf, ch);
                   sprintf (buf, "%s has a bounty of %d platinum coins on %s head!\n\r",
                            wch->name,
                            wch->pcdata->bounty/1000,
                            wch->sex == SEX_MALE ? "his"
                            : (wch->sex == SEX_FEMALE ? "her" : "its"));
           }

           send_to_char (buf, ch);

           if (wch->silent_mode
               || (IS_SET(wch->deaf, CHANNEL_CHAT) && IS_SET(wch->act, PLR_NO_TELL)))
                   send_to_char("Does not receive CHATs or TELLs.\n\r", ch);
           else if (IS_SET(wch->deaf, CHANNEL_CHAT))
                   send_to_char("Does not receive CHATs.\n\r", ch);
           else if (IS_SET(wch->act, PLR_NO_TELL))
                   send_to_char("Does not receive TELLs.\n\r", ch);

           return;
   }

   send_to_char("There is nobody logged on by that name.\n\r", ch);
}


void do_inventory( CHAR_DATA *ch, char *argument )
{
        char buf        [ MAX_STRING_LENGTH ];

        if (is_affected(ch,gsn_mist_walk))
        {
                send_to_char("You see particles of the finest mist...\n\r",ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You're a bit busy to look in your backpack!\n\r", ch);
                return;
        }

        send_to_char( "Your backpack contains:\n\r", ch );
        show_list_to_char( ch->carrying, ch, TRUE, TRUE );
        sprintf( buf, "You are carrying %d/%d items.\n\r",
                ch->carry_number, can_carry_n( ch ) );
        send_to_char( buf, ch );

        return;
}


void do_equipment( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA *obj;
        int i, eff_class;
        bool show_empty = FALSE;
        bool found = FALSE;

        if (ch->sub_class)
                eff_class = ch->sub_class + MAX_CLASS - 1;
        else
                eff_class = ch->class;

        if (argument[0] != '\0')
                show_empty = TRUE;

        if (is_affected(ch, gsn_mist_walk))
        {
                send_to_char("You see particles of the finest mist...\n\r",ch);
                return;
        }

        for (i = 0; i < MAX_WEAR; i++)
        {
                if (!(obj = get_eq_char(ch, equipment_slot_order[i])))
                {
                        if (IS_NPC(ch)
                            || !show_empty
                            || (equipment_slot_order[i] == WEAR_RANGED_WEAPON
                                && ch->sub_class != SUB_CLASS_KNIGHT
                                && !(ch->class == CLASS_RANGER && !ch->sub_class))
                            || (equipment_slot_order[i] == WEAR_DUAL
                                && !ch->pcdata->learned[gsn_dual])
                            || !loc_wear_table[eff_class].can_wear[eq_slot_to_wear_bit[equipment_slot_order[i]]])
                        {
                                continue;
                        }
                        send_to_char(where_name[equipment_slot_order[i]], ch);
                        send_to_char("-\n\r", ch);
                }
                else
                {
                        send_to_char(where_name[equipment_slot_order[i]], ch);
                        found = TRUE;

                        if (can_see_obj(ch, obj))
                        {
                                send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
                                send_to_char("\n\r", ch);
                        }
                        else
                                send_to_char("something\n\r", ch);
                }
        }

        if (!found && !show_empty)
                send_to_char("You are not using any equipment.\n\r", ch);
}


void do_compare( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA *obj1;
        OBJ_DATA *obj2;
        char     *msg;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        int       value1;
        int       value2;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Compare what to what?\n\r", ch );
                return;
        }

        if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
        {
                send_to_char( "You do not have that item.\n\r", ch );
                return;
        }

        if ( arg2[0] == '\0' )
        {
                for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
                {
                        if ( obj2->wear_loc != WEAR_NONE
                            && can_see_obj( ch, obj2 )
                            && obj1->item_type == obj2->item_type
                            && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
                                break;
                }

                if ( !obj2 )
                {
                        send_to_char( "You aren't wearing anything comparable.\n\r", ch );
                        return;
                }
        }
        else
        {
                if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
                {
                        send_to_char( "You do not have that item.\n\r", ch );
                        return;
                }
        }

        msg     = NULL;
        value1  = 0;
        value2  = 0;

        if ( obj1 == obj2 )
                msg = "You compare $p to itself.  It looks about the same.";

        else if ( obj1->item_type != obj2->item_type )
                msg = "You can't compare $p and $P.";

        else
        {
                switch ( obj1->item_type )
                {
                    default:
                        msg = "You can't compare $p and $P.";
                        break;

                    case ITEM_ARMOR:
                        value1 = obj1->value[0];
                        value2 = obj2->value[0];
                        break;

                    case ITEM_WEAPON:
                        value1 = obj1->value[1] + obj1->value[2];
                        value2 = obj2->value[1] + obj2->value[2];
                        break;
                }
        }

        if ( !msg )
        {
                if ( value1 == value2 ) msg = "$p and $P look about the same.";
                else if ( value1  > value2 ) msg = "$p looks better than $P.";
                else                         msg = "$p looks worse than $P.";
        }

        act( msg, ch, obj1, obj2, TO_CHAR );
        return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
        do_help( ch, "diku" );
        return;
}


void do_where( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *victim;
        DESCRIPTOR_DATA *d;
        char             buf [ MAX_STRING_LENGTH ];
        char             arg [ MAX_INPUT_LENGTH  ];
        bool             found;

        if (!IS_AWAKE(ch) || !check_blind(ch))
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                found = FALSE;

                for ( d = descriptor_list; d; d = d->next )
                {
                        if ( d->connected == CON_PLAYING
                            && ( victim = d->character )
                            && ( victim != ch )
                            && (!IS_NPC( victim ))
                            && victim->in_room
                            && victim->in_room->area == ch->in_room->area
                            && can_see( ch, victim ))
                        {
                                if (IS_AFFECTED(victim, AFF_HIDE) && victim->in_room != ch->in_room)
                                        continue;

                                if( !found )
                                {
                                        found = TRUE;
                                        send_to_char( "You detect the presence of:\n\r", ch );
                                }
                                sprintf( buf, "%-28s %s\n\r",
                                        victim->name, victim->in_room->name );
                                send_to_char( buf, ch );
                        }
                }

                if ( !found )
                        send_to_char( "You find no other players.\n\r", ch );
        }
        else
        {
                found = FALSE;

                for ( victim = char_list; victim; victim = victim->next )
                {
                        if (!victim->deleted
                            && victim->in_room
                            && victim->in_room->area == ch->in_room->area
                            && can_see(ch, victim)
                            && multi_keyword_match(arg, victim->name))
                        {
                                if (IS_AFFECTED(victim, AFF_HIDE) && victim->in_room != ch->in_room)
                                        continue;

                                if( !found )
                                {
                                        found = TRUE;
                                        send_to_char( "You detect the presence of:\n\r", ch );
                                }
                                sprintf( buf, "%-28s %s\n\r",
                                        capitalize_initial(PERS( victim, ch )),
                                        victim->in_room->name );
                                send_to_char( buf, ch );
                                continue;
                        }
                }

                if ( !found )
                        send_to_char ("You fail to find anyone by that name.\n\r", ch);
        }

        WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
        return;
}


void do_consider( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *victim;
        char      *msg = '\0';
        char      *buf = '\0';
        char       arg [ MAX_INPUT_LENGTH ];
        int        diff;
        int        hpdiff;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Consider killing whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_room( ch, arg ) ) )
        {
                send_to_char( "They're not here.\n\r", ch );
                return;
        }

        if ( !IS_NPC( victim ) )
        {
                send_to_char( "The gods do not accept this type of sacrifice.\n\r", ch );
                return;
        }

        act("$n starts to consider $N as a possible target.", ch, NULL, victim, TO_ROOM);
        diff = victim->level - ch->level;

        if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
        else if ( diff <=  -5 ) msg = "$N is no match for you.";
        else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
        else if ( diff <=   1 ) msg = "The perfect match!";
        else if ( diff <=   5 ) msg = "$N says 'Do you feel lucky, punk?'.";
        else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
        else if ( diff <=  14 ) msg = "Death will thank you for your gift.";
        else if ( diff <=  25 ) msg = "$N says 'I could crush you with my little finger!'";
        else if ( diff <=  99 ) msg = "$N says 'You puny insect... I will destroy you!'";
        else msg = "$N is unimaginably more powerful than you.";

        act( msg, ch, NULL, victim, TO_CHAR );

        if ( !IS_NPC(ch) && number_percent( ) < ch->pcdata->learned[gsn_advanced_consider] )
        {
                char buf2 [MAX_STRING_LENGTH];

                if ( diff < 0 )
                        sprintf( buf2, "%s is %d levels below yours.\n\r",
                                capitalize(victim->short_descr),( 0 - diff) );

                else if ( diff > 0 )
                        sprintf( buf2, "You surmise that %s is %d levels higher than you!\n\r",
                                victim->short_descr, diff );

                else
                        sprintf( buf2, "%s is the same level as you!\n\r",
                                capitalize(victim->short_descr) );

                send_to_char( buf2, ch );
        }

        hpdiff = ( ch->hit - victim->hit );

        if ( ( ( diff >= 0) && ( hpdiff <= 0 ) )
            || ( ( diff <= 0 ) && ( hpdiff >= 0 ) ) )
        {
                send_to_char( "Also,", ch );
        }
        else
        {
                send_to_char( "However,", ch );
        }

        if ( hpdiff >= 101 )
                buf = " you are currently much healthier than $E.";
        if ( hpdiff <= 100 )
                buf = " you are currently healthier than $E.";
        if ( hpdiff <= 50 )
                buf = " you are currently slightly healthier than $E.";
        if ( hpdiff <= 25 )
                buf = " you are a teensy bit healthier than $E.";
        if ( hpdiff <= 0 )
                buf = " $E is a teensy bit healthier than you.";
        if ( hpdiff <= -25 )
                buf = " $E is slightly healthier than you.";
        if ( hpdiff <= -50 )
                buf = " $E is healthier than you.";
        if ( hpdiff <= -100 )
                buf = " $E is much healthier than you.";
        if ( hpdiff <= -500 )
                buf = " $E is built like a tank!";

        act( buf, ch, NULL, victim, TO_CHAR );

        if ( !IS_NPC( ch ) && number_percent( ) < ch->pcdata->learned[gsn_advanced_consider] )
        {
                int hps;
                char buf1 [MAX_STRING_LENGTH];

                hps = ( victim->hit /100 ) * 100;
                sprintf ( buf1, "You estimate %s to have %d hit points.\n\r",
                         victim->short_descr, hps );
                send_to_char ( buf1, ch );
        }

        /*  A "sizing up" ability for thugs?  Re-enabled and edited for sanity 5/3/22 -- Owl */
        if ( IS_NPC( victim ) && ch->sub_class == 7 && victim->spec_fun != 0 )
                act ("You suspect $N of having unusual capabilities.", ch,
                     NULL, victim, TO_CHAR );
}


void set_title (CHAR_DATA *ch, char *title)
{
        char buf [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        strcpy(buf, title);
        free_string(ch->pcdata->title);
        ch->pcdata->title = str_dup(buf);
        return;
}


void do_title (CHAR_DATA *ch, char *argument)
{
        int len, i, k;
        char buf [MAX_STRING_LENGTH];
        char tidy [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (IS_SET(ch->act, PLR_SILENCE))
        {
                send_to_char("Not while you are silenced.\n\r", ch);
                set_title(ch, "");
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char( "Your title is now blank.\n\r", ch );
                set_title( ch, "" );
                return;
        }


        /*
         * Destroy unwanted colour escapes
         */
        len = strlen(argument);

        for (i = 0, k = 0; i < len && k < 50; i++, k++)
        {
                if (argument[i] == '~')
                        continue;

                if (argument[i] == '{'
                    && (argument[i+1] == '/'
                        || argument[i+1] == '*'
                        || argument[i+1] == '{'
                        || argument[i+1] == '\0'
                        || argument[i+1] == '\\'))
                {
                        tidy[k++] = '{';
                }

                else if (argument[i] == '}')
                        tidy[k++] = '}';

                tidy[k] = argument[i];
        }

        tidy[k] = '\0';

        sprintf(buf, "{x%s%s{x",
                (tidy[0] == ',' || tidy[0] == ':' || tidy[0] == ';' || tidy[0] == '.') ? "" : " ",
                tidy);

        set_title(ch, buf);
        send_to_char("Ok.\n\r", ch);
}


void do_description( CHAR_DATA *ch, char *argument )
{
        char buf [ MAX_STRING_LENGTH ];

        if ( argument[0] != '\0' )
        {
                buf[0] = '\0';
                smash_tilde( argument );

                if ( argument[0] == '+' )
                {
                        if ( ch->description )
                                strcat( buf, ch->description );
                        argument++;
                        while ( isspace( *argument ) )
                                argument++;
                }

                if ( strlen( buf ) + strlen( argument ) >=  MAX_STRING_LENGTH  - 2 )
                {
                        send_to_char( "Description too long.\n\r", ch );
                        return;
        }

                strcat( buf, argument );
                strcat( buf, "\n\r" );
                free_string( ch->description );
                ch->description = str_dup( buf );
        }

        send_to_char( "Your description is:\n\r", ch );
        send_to_char( ch->description ? ch->description : "(None).\n\r", ch );

        return;
}


void do_report( CHAR_DATA *ch, char *argument )
{
        /*
         *  Redone by Gezhp 2000
         */

        char buf [MAX_INPUT_LENGTH], stats [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        sprintf (stats, "{%c%d{x/%d hits, {%c%d{x/%d mana, {%c%d{x/%d move",
                 (ch->hit < (ch->max_hit - ch->aggro_dam)/10) ? 'R' : 'G',
                 ch->hit, ch->max_hit - ch->aggro_dam,
                 (ch->mana < ch->max_mana/10) ? 'R' : 'C',
                 ch->mana, ch->max_mana,
                 (ch->move < ch->max_move/10) ? 'R' : 'Y',
                 ch->move, ch->max_move);

        if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
        {
                sprintf (buf, ", %d exp needed to level",
                         level_table[ch->level].exp_total - ch->exp);
                strcat (stats, buf);
        }

        sprintf (buf, "You report: %s.\n\r", stats);
        send_to_char (buf, ch);

        sprintf (buf, "$c reports: %s.", stats);
        act (buf, ch, NULL, NULL, TO_ROOM);
}


/*
 * Geoff - pre_req spell system
 */
int has_groups(CHAR_DATA *ch, int sn)
{
        int learned [MAX_SKILL];
        int iter;
        int jter;
        int min;

        /* take care of either pc or npc types :) */
        for (iter = 0; iter < MAX_SKILL; iter++)
        {
                if (IS_NPC(ch))
                        learned[iter] = ch->pIndexData->skills->learned[iter];
                else
                        learned[iter] = ch->pcdata->learned[iter];
        }

        min = 797;
        jter = 0;

        for (iter = 0; iter < MAX_SPELL_GROUP; iter++)
        {
                /* hit a new group in the table */
                if (*spell_group_table[iter].skill == *spell_groups[jter])
                {
                        if (jter < MAX_GROUPS - 1)
                                jter++;
                }
                else
                {
                        /* if find the required spell */
                        if (*spell_group_table[iter].skill == sn)
                        {
                                /* if the person has the correct amount of knowledge */
                                if (learned[*spell_groups[jter - 1]] >= spell_group_table[iter].min)
                                {
                                        /* assign the minimum of the teacher across all groups */
                                        if (min > learned[*spell_groups[jter - 1]])
                                                min = learned[*spell_groups[jter - 1]];
                                }
                                /* knowledge not present -> cannot teach :)  */
                                else
                                {
                                        min = 0;
                                }
                        }
                }
        }

        if (min == 797)
                min = 0;

        return min;
}


int has_pre_req(CHAR_DATA *ch, int sn)
{
#define SUB_GROUPS 25

        int iter;
        bool found[SUB_GROUPS];
        bool ok[SUB_GROUPS];
        bool sub = FALSE;
        bool gotit = FALSE;

        /* Ugly Vampire and Werewolf hacks - Anonymous */
        if (ch->sub_class == SUB_CLASS_VAMPIRE)
                for (iter = 0; iter < MAX_VAMPIRE_GAG; iter++)
                        if (*vampire_gag_table[iter].skill == sn)
                                return 0;

        if (ch->sub_class == SUB_CLASS_WEREWOLF)
                for (iter = 0; iter < MAX_VAMPIRE_GAG-3; iter++)
                        if (*vampire_gag_table[iter].skill == sn)
                                return 0;

        /* initiate sub group test */
        for (iter = 0; iter < SUB_GROUPS; iter++)
        {
                found[iter] = FALSE;
                ok[iter] = TRUE;
        }

        /* the scan of the list (sigh) */
        for (iter = 0; iter < MAX_PRE_REQ; iter++)
        {
                if (*pre_req_table[iter].skill == sn)
                {
                        gotit = TRUE;

                        /* if a sub group found (not group 0) */
                        if (pre_req_table[iter].group)
                        {
                                found[pre_req_table[iter].group] = TRUE;
                                sub = TRUE;
                        }

                        if (ch->pcdata->learned[*pre_req_table[iter].pre_req]
                            < pre_req_table[iter].min )
                        {
                                if (pre_req_table[iter].group == 0)
                                        return 0;
                                else
                                        ok[pre_req_table[iter].group] = FALSE;
                        }
                }
        }

        for (iter = 0; iter < SUB_GROUPS; iter++)
        {
                if (found[iter] == TRUE && ok[iter] == TRUE)
                        return 1;
        }

        if (sub)
                return 0;
        else
                return gotit;
}


void do_advice (CHAR_DATA *ch, char *argument)
{
        char buf [MAX_STRING_LENGTH];
        int sn;
        int iter;
        int pre_group;
        bool result;
        CHAR_DATA *mob;

        if (IS_NPC(ch))
                return;

        if ( argument[0] == '\0' )
        {
                send_to_char("What did you want advice on?\n\r", ch);
                return;
        }

        for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
        {
                if ( mob->deleted )
                        continue;

                if (IS_NPC(mob) && mob->pIndexData->skills)
                        break;
        }

        if ( !mob )
        {
                send_to_char( "Who is going to advise you?\n\r", ch );
                return;
        }

        sn = skill_lookup(argument);

        if (sn < 0)
        {
                sprintf(buf, "I'm sorry %s, but I have never heard of that ability.",
                        ch->name);
                do_say(mob, buf);
                return;
        }

        if (skill_table[sn].prac_type == TYPE_WIZ)
        {
                do_say (mob, "I'm am not versed in the ways of the gods.");
                return;
        }

        if (skill_table[sn].prac_type == TYPE_NULL)
        {
                do_say (mob, "Of course levelling will help you in the long run.");
                return;
        }

        if (!ch->pcdata->learned[sn])
        {
                sprintf(buf, "I'm sorry %s, but my advice would be meaningless until you know something about %s.", ch->name, skill_table[sn].name);
                do_say(mob, buf);
                return;
        }

        /* check the pc is of appropriate level */
        if (ch->level < mob->pIndexData->skills->learned[gsn_teacher_base])
        {
                sprintf(buf, "I'm sorry %s, but you are not yet of the right calibre for my advice.", ch->name);
                do_say(mob, buf);
                return;
        }

        result = FALSE;

        /* What sort of pre-reqs do we need to search? */

        pre_group = ch->class + 1;

        if (ch->sub_class)
                pre_group = ch->sub_class + MAX_CLASS;

        for (iter = 0; iter < gsn_mage_base; iter++)
        {
                if (*pre_req_table[iter].pre_req == sn)
                {
                        /* Check the group is the correct one or a common pre-req */

                        if ((pre_req_table[iter].group == pre_group)
                            || (!pre_req_table[iter].group))
                        {
                                if (pre_req_table[iter].min > ch->pcdata->learned[sn])
                                {
                                        int effective_teach = 0;
                                        effective_teach = mob->pIndexData->skills->learned[sn];

                                        if (!effective_teach)
                                                effective_teach = has_groups(mob, sn);

                                        if (skill_table[sn].prac_type == TYPE_INT)
                                                effective_teach -= get_int_penalty(ch);
                                        else
                                                effective_teach -= get_phys_penalty(ch);

                                        if (pre_req_table[iter].min <= effective_teach)
                                        {
                                                result = TRUE;
                                                break;
                                        }
                                }
                        }
                }
        }

        if (result)
        {
                sprintf (buf, "If you were more proficient in %s %s, then I may be able to teach you some new knowledge.", skill_table[sn].name, ch->name);
                do_say (mob, buf);
        }
        else
        {
                sprintf (buf, "Learning %s from me would not have any long term benefits, %s.", skill_table[sn].name, ch->name);
                do_say (mob, buf);
        }
}


void do_practice (CHAR_DATA *ch, char *argument)
{
        char buf  [ MAX_STRING_LENGTH   ];
        int penalty;
        int sn;
        int teach;
        CHAR_DATA *mob;

        if (IS_NPC(ch))
                return;

        if ( argument[0] == '\0' )
        {
                prac_slist(ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
        {
                if ( mob->deleted )
                        continue;

                if (IS_NPC(mob) && mob->pIndexData->skills)
                        break;
        }

        if (!mob)
        {
                send_to_char( "Who is going to teach you?\n\r", ch );
                return;
        }

        sn = skill_lookup(argument);

        if (sn < 0)
        {
                sprintf(buf, "I'm sorry %s, but I have never heard of that ability.",
                        ch->name);
                do_say(mob, buf);
                return;
        }

        if (skill_table[sn].prac_type == TYPE_WIZ || skill_table[sn].prac_type == TYPE_NULL)
                return;

        if (sn != -1)
                teach = mob->pIndexData->skills->learned[sn];
        else
                teach = 0;

        /* check the pc is of appropriate level */
        if (ch->level < mob->pIndexData->skills->learned[gsn_teacher_base])
        {
                sprintf(buf, "I'm sorry %s, but you are not yet of the right calibre for my knowledge.", ch->name);
                do_say(mob, buf);
                return;
        }

        /* reduce the appropriate practices of the char */
        if (skill_table[sn].prac_type == TYPE_INT)
        {
                if (!ch->pcdata->int_prac)
                {
                        sprintf(buf, "I'm sorry %s, you haven't the potential to obtain further knowledge.",
                                ch->name);
                        do_say(mob, buf);
                        return;
                }
        }
        else
        {
                if (!ch->pcdata->str_prac)
                {
                        sprintf(buf, "I'm sorry %s, you haven't the ability to learn more skills.",
                                ch->name);
                        do_say(mob, buf);
                        return;
                }
        }

        /* assign the int / wis penalty */
        if (skill_table[sn].prac_type == TYPE_STR)
                penalty = get_phys_penalty(ch);
        else
                penalty = get_int_penalty(ch);

        /* check the ability of the teacher - first skill itself */
        if (teach != 0)
        {
                /* if the skill <> 0 then go by that ..... */
                if (((teach - 1) <= (ch->pcdata->learned[sn] + penalty))
                    || (ch->pcdata->learned[sn] >= 100))
                {
                        sprintf(buf, "I'm sorry %s, but I have insufficient knowledge to help you.",
                                ch->name);
                        do_say(mob, buf);
                        return;
                }
        }

        /* else if the individual skill is 0, check the groups ... */
        else
        {
                teach = has_groups(mob, sn);

                if (((teach - 1) <= (ch->pcdata->learned[sn] + penalty))
                    || (ch->pcdata->learned[sn] >= 100))
                {
                        sprintf(buf, "I'm sorry %s, but I have insufficient knowledge to help you.",
                                ch->name);
                        do_say(mob, buf);
                        return;
                }
        }

        /* check the PC's pre_req */
        if (has_pre_req(ch, sn))
        {
                /* practice */
                if (skill_table[sn].prac_type == TYPE_INT)
                        ch->pcdata->int_prac--;
                else
                        ch->pcdata->str_prac--;

                ch->pcdata->learned[sn] = (ch->pcdata->learned[sn] - penalty + teach) / 2;
                if (ch->pcdata->learned[sn] > 100)
                        ch->pcdata->learned[sn] = 100;

                sprintf(buf, "I hope my knowledge helps you, %s.", ch->name);
                do_say(mob, buf);
                return;
        }
        else
        {
                sprintf(buf, "I'm sorry %s, but you are not ready for that knowledge.",
                        ch->name);
                do_say(mob, buf);
                return;
        }
}


void do_train (CHAR_DATA *ch, char *argument)
{
        if ( IS_NPC( ch ) )
                return;

        if (!str_prefix("str", argument))
        {
                ch->pcdata->stat_train = APPLY_STR;
                send_to_char( "You commit yourself to strength training.\n\r", ch );
        }
        else if (!str_prefix("int", argument))
        {       
                ch->pcdata->stat_train = APPLY_INT;
                send_to_char( "You will labour to sharpen your intellect.\n\r", ch );
        }
        else if (!str_prefix("wis", argument))
        {       
                ch->pcdata->stat_train = APPLY_WIS;
                send_to_char( "You dedicate yourself to the pursuit of wisdom.\n\r", ch );
        }
        else if (!str_prefix("dex", argument))
        {
                ch->pcdata->stat_train = APPLY_DEX;
                send_to_char( "You will focus on improving your dexterity.\n\r", ch );
        }
        else if (!str_prefix("con", argument))
        {
                ch->pcdata->stat_train = APPLY_CON;
                send_to_char( "You intend to cultivate a hardier constitution.\n\r", ch );
        }
        else
        {
                send_to_char("Syntax: train <str|int|wis|dex|con>\n\r", ch);

                switch (ch->pcdata->stat_train)
                {
                    case APPLY_STR:
                        send_to_char("You are currently training strength.\n\r",ch);
                        return;

                    case APPLY_INT:
                        send_to_char("You are currently training intelligence.\n\r",ch);
                        return;

                    case APPLY_WIS:
                        send_to_char("You are currently training wisdom.\n\r",ch);
                        return;

                    case APPLY_DEX:
                        send_to_char("You are currently training dexterity.\n\r",ch);
                        return;

                    case APPLY_CON:
                        send_to_char("You are currently training constitution.\n\r",ch);
                        return;
                }
        }

}



/*
 * helper function for slist
 */
void show_slist (CHAR_DATA *ch, int number)
{
        /* base class */
        if (number < MAX_CLASS)
        {
                switch (number)
                {
                    case CLASS_WARRIOR: do_help(ch, "swarrior"); break;
                    case CLASS_CLERIC: do_help(ch, "scleric"); break;
                    case CLASS_MAGE: do_help(ch, "smage"); break;
                    case CLASS_THIEF: do_help(ch, "sthief"); break;
                    case CLASS_BRAWLER: do_help(ch, "sbrawler"); break;
                    case CLASS_PSIONICIST: do_help(ch, "spsi"); break;
                    case CLASS_SHAPE_SHIFTER: do_help(ch, "sshape"); break;
                    case CLASS_RANGER: do_help(ch, "sranger"); break;

                    default: break;
                }
        }

        /* sub-class */
        switch (number - MAX_CLASS)
        {
            case SUB_CLASS_NECROMANCER: do_help(ch, "snecromancer"); break;
            case SUB_CLASS_WARLOCK: do_help(ch, "swarlock"); break;
            case SUB_CLASS_TEMPLAR: do_help(ch, "stemplar"); break;
            case SUB_CLASS_DRUID: do_help(ch, "sdruid"); break;
            case SUB_CLASS_NINJA: do_help(ch, "sninja"); break;
            case SUB_CLASS_BOUNTY: do_help(ch, "sbounty"); break;
            case SUB_CLASS_THUG: do_help(ch, "sthug"); break;
            case SUB_CLASS_KNIGHT: do_help(ch, "sknight"); break;
            case SUB_CLASS_SATANIST: do_help(ch, "ssatanist"); break;
            case SUB_CLASS_WITCH: do_help(ch, "switch"); break;
            case SUB_CLASS_WEREWOLF: do_help(ch, "swerewolf"); break;
            case SUB_CLASS_VAMPIRE: do_help(ch, "svampire"); break;
            case SUB_CLASS_MONK: do_help(ch, "smonk"); break;
            case SUB_CLASS_MARTIAL_ARTIST: do_help(ch, "smartial"); break;
            case SUB_CLASS_BARBARIAN: do_help(ch, "sbarbarian"); break;
            case SUB_CLASS_BARD: do_help(ch, "sbard"); break;

            default: break;
        }
}


void do_slist (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        one_argument(argument, arg);

        /* natural slist */
        if (arg[0] == '\0')
        {
                /* show slist relevant to the PC's class / sub-class */
                if (!ch->sub_class)
                        show_slist(ch, ch->class);
                else
                        show_slist(ch, ch->sub_class + MAX_CLASS);

                return;
        }

        send_to_char( "To view the skills/spells for another class type 'HELP S<class>'.\n\r", ch );
        send_to_char( "E.g. for a warlock, 'HELP SWARLOCK'; for a warrior: 'HELP SWARRIOR'.\n\r",ch);
}


void prac_slist (CHAR_DATA *ch)
{
        char foo [MAX_STRING_LENGTH], str_col = 'Y', int_col = 'C';
        int sn, page, new;
        const char* bar = "_____________________________________________________________________________\n\r\n\r";

        if (IS_NPC(ch))
                return;

        /* race skills:- */
        sprintf( foo, " Race skills:\n\r%30s: {W%3d{x%%    %30s: {W%3d{x%%\n\r",
                race_table[ch->race].spell_one,
                ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_one)],
                race_table[ch->race].spell_two,
                ch->pcdata->learned[skill_lookup(race_table[ch->race].spell_two)]);
        send_to_char(foo, ch);
        send_to_char(bar, ch);

        page = 0;
        send_to_char(" Skills known:\n\r", ch);

        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                if (skill_table[sn].prac_type == TYPE_WIZ
                    || skill_table[sn].prac_type == TYPE_NULL)
                        continue;

                if (CAN_DO(ch, sn))
                {
                        if (!page++)
                        {
                                sprintf(foo, "{%c%30s{x: {W%3d{x%%",
                                        skill_table[sn].prac_type == TYPE_STR ? str_col : int_col,
                                        skill_table[sn].name,
                                        ch->pcdata->learned[sn]);
                        }
                        else
                        {
                                sprintf(foo, "    {%c%30s{x: {W%3d{x%%\n",
                                        skill_table[sn].prac_type == TYPE_STR ? str_col : int_col,
                                        skill_table[sn].name,
                                        ch->pcdata->learned[sn]);
                                page = 0;
                        }

                        send_to_char(foo, ch);
                }
        }

        if (page)
                send_to_char ("\n\r", ch);
        page = 0;
        new = 0;

        for (sn = 0; sn < gsn_mage_base; sn++)
        {
                if (ch->pcdata->learned[sn] == 0)
                {
                        if (has_pre_req(ch, sn))
                        {
                                if (!new)
                                {
                                        send_to_char(bar, ch);
                                        send_to_char(" Skills which may be learned:\n\r", ch);
                                        new++;
                                }

                                if (!page++)
                                {
                                        sprintf(foo, "{%c%30s{x: {W%3d{x%%",
                                                skill_table[sn].prac_type == TYPE_STR ? str_col : int_col,
                                                skill_table[sn].name,
                                                ch->pcdata->learned[sn]);
                                }
                                else
                                {
                                        sprintf(foo, "    {%c%30s{x: {W%3d{x%%\n",
                                                skill_table[sn].prac_type == TYPE_STR ? str_col : int_col,
                                                skill_table[sn].name,
                                                ch->pcdata->learned[sn]);
                                        page = 0;
                                }

                                send_to_char(foo, ch);
                        }
                }
        }

        if (page)
                send_to_char("\n\r", ch);
        send_to_char(bar, ch);

        sprintf(foo, "You have {W%d{x {%cphysical{x and {W%d{x {%cintellectual{x practices remaining.\n\r",
                ch->pcdata->str_prac, str_col, ch->pcdata->int_prac, int_col );
        send_to_char(foo, ch );

        return;
}


/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
        char buf [ MAX_STRING_LENGTH ];
        char arg [ MAX_INPUT_LENGTH  ];
        int  wimpy;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
                wimpy = ch->max_hit / 5;
        else
                wimpy = atoi( arg );

        if ( wimpy < 0 )
        {
                send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
                return;
        }

        if ( wimpy > ch->max_hit / 5 )
        {
                send_to_char( "Such cowardice ill becomes you.\n\r", ch );
                return;
        }

        ch->wimpy = wimpy;
        sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
        send_to_char( buf, ch );
        return;
}


void do_password( CHAR_DATA *ch, char *argument )
{
        char *pArg;
        char *pwdnew;
        char *p;
        char  arg1 [ MAX_INPUT_LENGTH ];
        char  arg2 [ MAX_INPUT_LENGTH ];
        char  cEnd;

        if ( IS_NPC( ch ) )
                return;

        /*
         * Can't use one_argument here because it smashes case.
         * So we just steal all its code.  Bleagh.
         */
        pArg = arg1;
        while ( isspace( *argument ) )
                argument++;

        cEnd = ' ';
        if ( *argument == '\'' || *argument == '"' )
                cEnd = *argument++;

        while ( *argument != '\0' )
        {
                if ( *argument == cEnd )
                {
                        argument++;
                        break;
                }
                *pArg++ = *argument++;
        }
        *pArg = '\0';

        pArg = arg2;
        while ( isspace( *argument ) )
                argument++;

        cEnd = ' ';
        if ( *argument == '\'' || *argument == '"' )
                cEnd = *argument++;

        while ( *argument != '\0' )
        {
                if ( *argument == cEnd )
                {
                        argument++;
                        break;
                }
                *pArg++ = *argument++;
        }
        *pArg = '\0';

        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
                send_to_char( "Syntax: password <old> <new>.\n\r", ch );
                return;
        }

        if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
        {
                WAIT_STATE( ch, 40 );
                send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
                return;
        }

        if ( strlen( arg2 ) < 5 )
        {
                send_to_char(
                             "New password must be at least five characters long.\n\r", ch );
                return;
        }

        /*
         * No tilde allowed because of player file format.
         */
        pwdnew = crypt( arg2, ch->name );
        for ( p = pwdnew; *p != '\0'; p++ )
        {
                if ( *p == '~' )
                {
                        send_to_char("New password not acceptable, try again.\n\r", ch );
                        return;
                }
        }

        free_string( ch->pcdata->pwd );
        ch->pcdata->pwd = str_dup( pwdnew );
        save_char_obj( ch );
        send_to_char( "Ok.\n\r", ch );
        return;
}


void do_socials( CHAR_DATA *ch, char *argument )
{
        char buf  [ MAX_STRING_LENGTH ];
        char buf1 [ MAX_STRING_LENGTH ];
        int  iSocial;
        int  col;

        buf1[0] = '\0';
        col = 0;
        for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
        {
                sprintf( buf, "%-12s", social_table[iSocial].name );
                strcat( buf1, buf );
                if ( ++col % 6 == 0 )
                        strcat( buf1, "\n\r" );
        }

        if ( col % 6 != 0 )
                strcat( buf1, "\n\r" );

        send_to_char( buf1, ch );
        return;
}


/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
        char buf  [ MAX_STRING_LENGTH ];
        char buf1 [ MAX_STRING_LENGTH ];
        int  cmd;
        int  col;

        buf1[0] = '\0';
        col = 0;

        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
                if (   cmd_table[cmd].level <  LEVEL_HERO
                    && cmd_table[cmd].level <= get_trust( ch ) )
                {
                        sprintf( buf, "%-16s", cmd_table[cmd].name );
                        strcat( buf1, buf );

                        if ( ++col % 5 == 0 )
                                strcat( buf1, "\n\r" );
                }
        }

        if ( col % 5 != 0 )
                strcat( buf1, "\n\r" );

        send_to_char( buf1, ch );
        return;
}


void do_channels( CHAR_DATA *ch, char *argument )
{
        char arg [ MAX_INPUT_LENGTH  ];

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
                {
                        send_to_char( "You are silenced.\n\r", ch );
                        return;
                }

                send_to_char( "Channels:", ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_AUCTION  )
                             ? " +AUCTION"
                             : " -auction",
                             ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_CHAT     )
                             ? " +CHAT"
                             : " -chat",
                             ch );

                if ( IS_HERO( ch ) )
                {
                        send_to_char( !IS_SET( ch->deaf, CHANNEL_IMMTALK )
                                     ? " +IMMTALK"
                                     : " -immtalk",
                                     ch );
                }

                send_to_char( !IS_SET( ch->deaf, CHANNEL_MUSIC    )
                             ? " +MUSIC"
                             : " -music",
                             ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_QUESTION )
                             ? " +QUESTION"
                             : " -question",
                             ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_SHOUT    )
                             ? " +SHOUT"
                             : " -shout",
                             ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_YELL     )
                             ? " +YELL"
                             : " -yell",
                             ch );

                send_to_char( !IS_SET( ch->deaf, CHANNEL_INFO   )
                             ? " +INFO"
                             : " -info",
                             ch );

                send_to_char( !IS_SET(ch->deaf,CHANNEL_CLAN     )
                             ? " +CLAN"
                             : " -clan",
                             ch );

                send_to_char( !IS_SET(ch->deaf,CHANNEL_ARENA    )
                             ? " +ARENA"
                             : " -arena",
                             ch );

                send_to_char( !IS_SET(ch->act, PLR_NO_TELL      )
                             ? " +TELL"
                             : " -tell",
                             ch );

                send_to_char( ".\n\r", ch );
        }
        else
        {
                int  bit;
                bool fClear;

                if ( arg[0] == '+' )
                        fClear = TRUE;
                else if ( arg[0] == '-' )
                        fClear = FALSE;
                else
                {
                        send_to_char( "Channels -channel or +channel?\n\r", ch );
                        return;
                }

                if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
                else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
                else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
                else if ( !str_cmp( arg+1, "dirtalk"  ) ) bit = CHANNEL_DIRTALK;
                else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
                else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
                else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
                else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
                else if ( !str_cmp( arg+1, "info"     ) ) bit = CHANNEL_INFO;
                else if ( !str_cmp( arg+1, "clan"     ) ) bit = CHANNEL_CLAN;
                else if ( !str_cmp( arg+1, "server"   ) ) bit = CHANNEL_SERVER;
                else if ( !str_cmp( arg+1, "arena"    ) ) bit = CHANNEL_ARENA;
                else if ( !str_cmp( arg+1, "newbie"   ) ) bit = CHANNEL_NEWBIE;
                else
                {
                        send_to_char( "Set or clear which channel?\n\r", ch );
                        return;
                }

                if ( fClear )
                        REMOVE_BIT ( ch->deaf, bit );
                else
                        SET_BIT    ( ch->deaf, bit );

                send_to_char( "Ok.\n\r", ch );
        }

        return;
}


char * const color_name [] =
{
        "AUCTION",
        "CHAT",
        "SHOUT",
        "IMMTALK",
        "SAY",
        "TELL",
        "CLAN",
        "DIRTALK",
        "SERVER",
        "ARENA",
        "NEWBIE"
};


void do_ansi( CHAR_DATA *ch, char *argument )
{
        char arg[MAX_INPUT_LENGTH];
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int c;
        int c2;
        bool found;

        if ( IS_NPC(ch) )
                return;

        argument = one_argument( argument, arg );
        strcpy(arg1, argument );

        if ( arg[0] == '\0' || arg1[0] == '\0' )
        {
                if ( IS_SET( ch->act, PLR_ANSI ) )
                        for ( c = 1; c < 10; c++ )
                        {
                                sprintf( buf, "%s%10ss will appear in %s%s%s\n\r",
                                        color_table[19].code,
                                        color_name[c],
                                        color_table[ch->colors[c]].code,
                                        color_table[ch->colors[c]].name,
                                        color_table[19].code );
                                send_to_char( buf, ch );
                        }
                send_to_char( "\n\rTo toggle ANSI-color, type 'COLOR'.\n\r"
                             "Type 'HELP ANSI' for more information.\n\r", ch );
                return;
        }

        found = FALSE;

        for ( c = 0; c < 10; c++ )
        {
                if ( !str_cmp( arg, color_name[c] ) )
                        found = TRUE;

                if (found)
                        break;
        }

        if (( c > MAX_COLORS) || (c <= 0) || (!found) )
        {
                send_to_char( "Not valid.\n\r", ch );
                return;
        }

        if ( !is_number( arg1 ) )
        {
                for ( c2 = 0; c2 < 20; c2++ )
                {
                        if ( !str_cmp( arg1, color_table[c2].name ) )
                                break;
                }
        }
        else c2 = atoi( arg1 );

        sprintf( buf, "Invalid color index.\n\r" );

        if ((c < MAX_COLORS) && (c >= 0) && (c2 < 20) && (c2 >= 0))
        {
                ch->colors[c] = c2;
                sprintf( buf, "Set %s color to %s%s{x.\n\r", color_name[c],
                        color_table[c2].code,
                        color_table[c2].name);
        }

        send_to_char( buf, ch );
        send_to_char( NTEXT, ch );
        return;
}


/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
        char arg [ MAX_INPUT_LENGTH ];

        if ( IS_NPC( ch ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "[ Keyword  ] Option\n\r", ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOWIELD )
                             ? "[+AUTOWIELD] You automatically wield picked up weapons in combat.\n\r"
                             : "[-autowield] You don't automatically wield picked up weapons in combat.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOEXIT  )
                             ? "[+AUTOEXIT ] You automatically see exits.\n\r"
                             : "[-autoexit ] You don't automatically see exits.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOLOOT  )
                             ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
                             : "[-autoloot ] You don't automatically loot corpses.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOCOIN  )
                             ? "[+AUTOCOIN ] You automatically take coin from corpses.\n\r"
                             : "[-autocoin ] You don't automatically take coin from corpses.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOSAC   )
                             ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
                             : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_AUTOLEVEL  )
                             ? "[+AUTOLEVEL] You automatically level if you have enough experience.\n\r"
                             : "[-autolevel] You don't automatically level if you have enough experience.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_BLANK     )
                             ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
                             : "[-blank    ] You have no blank line before your prompt.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_BRIEF     )
                             ? "[+BRIEF    ] You see brief descriptions.\n\r"
                             : "[-brief    ] You see long descriptions.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_COMBINE   )
                             ? "[+COMBINE  ] You see object lists in combined format.\n\r"
                             : "[-combine  ] You see object lists in single format.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_PROMPT    )
                             ? "[+PROMPT   ] You have a prompt.\n\r"
                             : "[-prompt   ] You don't have a prompt.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_ANSI     )
                             ? "[+ANSI     ] You have color.\n\r"
                             : "[-ANSI     ] You don't have color.\n\r"
                             , ch );

                send_to_char( (ch->gag != 0)
                             ? ( (ch->gag == 1)
                                ? "[+GAG light] You are gagging some combat messages.\n\r"
                                : "[+GAG heavy] You are gagging many combat messages.\n\r")
                             : "[-gag      ] You receive all combat messages.\n\r"
                             , ch );

                send_to_char( (ch->silent_mode)
                             ? "[+QUIET    ] You're in quiet mode.\n\r"
                             : "[-quiet    ] Quiet mode is off.\n\r"
                             , ch );

                send_to_char( (ch->pcdata->allow_look)
                             ? "[+ALLOWEQ  ] Players see your description and equipment.\n\r"
                             : "[-alloweq  ] Players see your description only.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_TELNET_GA )
                             ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
                             : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
                             , ch );

                send_to_char(  IS_SET( ch->act, PLR_SILENCE   )
                             ? "[+SILENCE  ] You are silenced.\n\r"
                             : ""
                             , ch );

                send_to_char( !IS_SET( ch->act, PLR_NO_EMOTE  )
                             ? ""
                             : "[-emote    ] You can't emote.\n\r"
                             , ch );

                send_to_char( !IS_SET( ch->act, PLR_NO_TELL   )
                             ? "[+TELL     ] You receive tells.\n\r"
                             : "[-tell     ] You don't hear tells.\n\r"
                             , ch );
        }
        else
        {
                char buf [ MAX_STRING_LENGTH ];
                int  bit;
                bool fSet;

                if ( arg[0] == '+' )
                        fSet = TRUE;
                else if ( arg[0] == '-' )
                        fSet = FALSE;
                else
                {
                        send_to_char( "Config -option or +option?\n\r", ch );
                        return;
                }

                if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
                else if ( !str_cmp( arg+1, "autowield") ) bit = PLR_AUTOWIELD;
                else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
                else if ( !str_cmp( arg+1, "autocoin" ) ) bit = PLR_AUTOCOIN;
                else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
                else if ( !str_cmp( arg+1, "autolevel") ) bit = PLR_AUTOLEVEL;
                else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
                else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
                else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
                else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
                else if ( !str_cmp( arg+1, "ansi"     ) ) bit = PLR_ANSI;
                else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
                else
                {
                        send_to_char( "Config which option?\n\r", ch );
                        return;
                }

                if ( fSet )
                {
                        SET_BIT    ( ch->act, bit );
                        sprintf( buf, "%s is now ON.\n\r", arg+1 );
                        buf[0] = UPPER( buf[0] );
                        send_to_char( buf, ch );
                }
                else
                {
                        REMOVE_BIT ( ch->act, bit );
                        sprintf( buf, "%s is now OFF.\n\r", arg+1 );
                        buf[0] = UPPER( buf[0] );
                        send_to_char( buf, ch );
                }
        }

        return;
}


void do_wizlist ( CHAR_DATA *ch, char *argument )
{
        do_help ( ch, "wizlist" );
        return;
}


void do_spells ( CHAR_DATA *ch, char *argument )
{
        char foo  [ MAX_STRING_LENGTH ];
        int  sn;
        int  page;

        if (IS_NPC(ch))
                return;

        page = 0;

        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                if (CAN_DO(ch, sn) && IS_SPELL(sn))
                {
                        if (!page++)
                        {
                                sprintf(foo, "%25s: %3d mana", skill_table[sn].name,
                                        mana_cost(ch, sn));
                        }
                        else
                        {
                                sprintf(foo, "    %25s: %3d mana\n\r", skill_table[sn].name,
                                        mana_cost(ch, sn));
                                page = 0;
                        }

                        send_to_char(foo, ch);
                }
        }

        send_to_char("\n\r", ch);
        if (!page)
                send_to_char("\n\r", ch);

        return;
}


/* bypassing the config command - Kahn */

void do_autowield ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        if (ch->class == CLASS_BRAWLER)
                send_to_char("Brawlers cannot autowield weapons!\n\r",ch);
        else
        {
                ( IS_SET ( ch->act, PLR_AUTOWIELD )
                 ? sprintf( buf, "-autowield" )
                 : sprintf( buf, "+autowield" ) );

                do_config( ch, buf );
        }

        return;
}


void do_autoexit ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_AUTOEXIT )
         ? sprintf( buf, "-autoexit" )
         : sprintf( buf, "+autoexit" ) );

        do_config( ch, buf );

        return;
}


void do_autoloot ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_AUTOLOOT )
         ? sprintf( buf, "-autoloot" )
         : sprintf( buf, "+autoloot" ) );

        do_config( ch, buf );

        return;
}


void do_autocoin (CHAR_DATA *ch, char *argument)
{
        char buf [16];

        (IS_SET(ch->act, PLR_AUTOCOIN))
                ? sprintf(buf, "-autocoin")
                        : sprintf(buf, "+autocoin");
        do_config(ch, buf);
}


void do_autolevel (CHAR_DATA *ch, char *argument)
{
        char buf [16];

        (IS_SET(ch->act, PLR_AUTOLEVEL))
                ? sprintf(buf, "-autolevel")
                        : sprintf(buf, "+autolevel");
        do_config(ch, buf);
}


void do_color ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_ANSI )
         ? sprintf( buf, "-ansi" )
         : sprintf( buf, "+ansi" ) );

        do_config( ch, buf );
        return;
}


void do_autosac ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_AUTOSAC )
         ? sprintf( buf, "-autosac" )
         : sprintf( buf, "+autosac" ) );

        do_config( ch, buf );

        return;
}


void do_blank ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_BLANK )
         ? sprintf( buf, "-blank" )
         : sprintf( buf, "+blank" ) );

        do_config( ch, buf );

        return;
}


void do_brief ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_BRIEF )
         ? sprintf( buf, "-brief" )
         : sprintf( buf, "+brief" ) ) ;

        do_config( ch, buf );

        return;
}


void do_combine ( CHAR_DATA *ch, char *argument )
{
        char buf[ MAX_STRING_LENGTH ];

        ( IS_SET ( ch->act, PLR_COMBINE )
         ? sprintf( buf, "-combine" )
         : sprintf( buf, "+combine" ) );

        do_config( ch, buf );

        return;
}


void do_pagelen ( CHAR_DATA *ch, char *argument )
{
        char buf [ MAX_STRING_LENGTH ];
        char arg [ MAX_INPUT_LENGTH  ];
        int  lines;
        const int default_len = 25;
        const int max_len = 1000;

        if (IS_NPC(ch))
                return;

        one_argument( argument, arg );

        if (arg[0] == '\0')
        {
                if (ch->pcdata->pagelen)
                        lines = ch->pcdata->pagelen;
                else
                        lines = default_len;
        }
        else
        {
                lines = atoi( arg );
        }

        if (lines < 1)
        {
                send_to_char("Negative or zero values for a page pause are not legal.\n\r", ch );
                return;
        }

        if (lines > max_len)
        {
                sprintf(buf, "I don't know of a screen that is larger than %d lines!\n\r", max_len);
                send_to_char(buf, ch);
                lines = max_len;
        }

        ch->pcdata->pagelen = lines;
        sprintf(buf, "Page pause set to %d lines.\n\r", lines);
        send_to_char(buf, ch);
        return;
}


/*
 * Do_prompt from Morgenes from Aldara Mud
 */
void do_prompt( CHAR_DATA *ch, char *argument )
{
        char buf [ MAX_STRING_LENGTH ];

        if (IS_NPC(ch))
                return;

        buf[0] = '\0';

        ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

        if ( argument[0] == '\0' )
        {
                ( IS_SET ( ch->act, PLR_PROMPT )
                 ? sprintf( buf, "-prompt" )
                 : sprintf( buf, "+prompt" ) );

                do_config( ch, buf );
                return;
        }

        if ( !strcmp(argument, "current"))
        {
                send_to_char("Your current prompt:\n\r", ch);
                strcat(buf, ch->prompt);
                send_to_char(buf, ch);
                send_to_char("\n\r", ch);

                return;
        }

        if( !strcmp( argument, "all" ) || !strcmp (argument, "default") )
                strcat (buf, "<{G%h/%H{x hits {C%m/%M{x mana {Y%v/%V{x move> ");
        else
        {
                if ( strlen( argument ) > 100 )
                        argument[100] = '\0';
                smash_tilde( argument );
                strcat( buf, argument );
        }

        free_string( ch->prompt );
        ch->prompt = str_dup( buf );
        send_to_char( "Ok.\n\r", ch );
        return;
}


void do_auto( CHAR_DATA *ch, char *argument )
{
        do_config( ch, "" );
        return;
}


void do_afk( CHAR_DATA *ch, char *argument )
{
        if ( IS_NPC( ch ) )
                return;

        if ( IS_SET( ch->act, PLR_AFK ) )
        {
                REMOVE_BIT( ch->act, PLR_AFK );
                send_to_char( "You are back at your keyboard.\n\r", ch        );
                act( "$n has returned to $s keyboard.", ch, NULL, ch, TO_ROOM );
        }
        else
        {
                SET_BIT( ch->act, PLR_AFK );
                send_to_char( "You are now away from keyboard.\n\r", ch       );
                act( "$n has left $s keyboard.", ch, NULL, ch, TO_ROOM        );
        }

        return;
}


void do_hero ( CHAR_DATA *ch, char *argument )
{
        print_hero_table(ch, argument);
}


void do_legend ( CHAR_DATA *ch, char *argument )
{
        print_legend_table(ch, argument);
}


void do_fame( CHAR_DATA *ch, char *argument )
{
        print_fame_table(ch, argument);
}


void do_pkillers( CHAR_DATA *ch, char *argument )
{
        print_clan_pkill_table(ch, argument);
}


void do_vanquished( CHAR_DATA *ch, char *argument )
{
        print_clan_vanq_table(ch, argument);
}


/*
 *  Identify object from special mobs
 */
void do_identify( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA *obj;
        CHAR_DATA *rch;
        int cost;

        for (rch = ch->in_room->people; rch; rch = rch->next_in_room)
        {
                if (IS_NPC(rch) && IS_SET(rch->act, ACT_IDENTIFY))
                        break;
        }

        if (!rch)
        {
                send_to_char("No one here seems to know much about identifying anything.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                act("$C asks you, 'Which object would you like to identify?'",
                    ch, NULL, rch, TO_CHAR);
                return;
        }

        if (!(obj = get_obj_carry(ch, argument)))
        {
                send_to_char("You don't have that item.\n\r", ch);
                return;
        }

        if (total_coins_char(ch) < obj->level * 50)
        {
                act( "$C decides you do not have enough money for $s services.",
                    ch, obj, rch, TO_CHAR );
                return;
        }

        cost = obj->level * 50;
        send_to_char("Your purse feels lighter.\n\r", ch);
        coins_from_char(cost, ch);
        act("$C examines $p and identifies its properties.\n\r", ch, obj, rch, TO_CHAR);
        spell_identify(0, 0, ch, obj);
}


char* number_suffix (int num)
{
        if (num > 3 && num < 21)
                return "th";

        else if (num % 10 == 1)
                return "st";

        else if (num % 10 == 2)
                return "nd";

        else if (num % 10 == 3)
                return "rd";
        else
                return "th";
}


void do_status (CHAR_DATA *ch, char *argument)
{
        char buf [MAX_STRING_LENGTH];
        char tmp [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        print_player_status(ch, tmp);

        sprintf(buf,
                "\n\r%s\n\r"
                "Align: {W%6d{x     Str practices: {W%5d{x      Carrying: {C%4d{x/{c%4d{x\n\r"
                "Fame: {W%7d{x     Int practices: {W%5d{x      Burden: {C%6d{x/{c%4d{x\n\r",
                tmp,
                ch->alignment,
                ch->pcdata->str_prac,
                ch->carry_number,
                can_carry_n(ch),
                ch->pcdata->fame,
                ch->pcdata->int_prac,
                ch->carry_weight + ch->coin_weight,
                can_carry_w(ch));

        send_to_char (buf, ch);
}

void print_player_status (CHAR_DATA *ch, char* buf)
{
        char tmp [MAX_STRING_LENGTH];

        sprintf( tmp, "Str: {C%2d{x ({c%2d{x)%c     Hits:      {%c%4d{x/{G%4d{x",
                get_curr_str(ch),
                ch->pcdata->perm_str,
                check_stat_advance (ch, APPLY_STR) ? ' ' : '+',
                ch->hit < (ch->max_hit - ch->aggro_dam)/10 ? 'R' : 'G',
                ch->hit,
                ch->max_hit - ch->aggro_dam );
        strcpy( buf, tmp );

        if( ch->level >= 15 )
        {
                sprintf( tmp, "      Hit roll:     {R%5d{x",
                        GET_HITROLL(ch) );
                strcat( buf, tmp );
        }
        strcat( buf, "\n\r" );

        sprintf( tmp, "Int: {C%2d{x ({c%2d{x)%c     Aggro dam:      {%c%4d{x",
                get_curr_int(ch),
                ch->pcdata->perm_int,
                check_stat_advance (ch, APPLY_INT) ? ' ' : '+',
                ch->aggro_dam ? 'R' : 'G',
                ch->aggro_dam );
        strcat( buf, tmp );

        if( ch->level >= 15 )
        {
                sprintf( tmp, "      Dam roll:     {R%5d{x",
                        GET_DAMROLL(ch) );
                strcat( buf, tmp );
        }
        strcat( buf, "\n\r" );

        sprintf( tmp, "Wis: {C%2d{x ({c%2d{x)%c     Wimpy:          {G%4d{x",
                get_curr_wis(ch),
                ch->pcdata->perm_wis,
                check_stat_advance (ch, APPLY_WIS) ? ' ' : '+',
                ch->wimpy );
        strcat( buf, tmp );

        if( ch->level >= 20 )
        {
                sprintf( tmp, "      Armour class: {R%5d{x", GET_AC(ch) );
                strcat( buf, tmp );
        }
        strcat( buf, "\n\r" );

        sprintf( tmp, "Dex: {C%2d{x ({c%2d{x)%c     Mana:      {%c%4d{x/{C%4d{x",
                get_curr_dex(ch), ch->pcdata->perm_dex,
                check_stat_advance (ch, APPLY_DEX) ? ' ' : '+',
                ch->mana < ch->max_mana/10 ? 'R' : 'C',
                ch->mana, ch->max_mana );
        strcat( buf, tmp );

        if( ch->class == CLASS_SHAPE_SHIFTER )
        {
                if (ch->sub_class == SUB_CLASS_VAMPIRE)
                        sprintf( tmp, "      Form: %13s",
                                is_affected (ch, gsn_mist_walk) ? "mist" : "normal");
                else
                        sprintf( tmp, "      Form: %13s", extra_form_name(ch->form) );
                strcat( buf, tmp );
        }
        strcat( buf, "\n\r" );

        sprintf( tmp, "Con: {C%2d{x ({c%2d{x)%c     Move:      {%c%4d{x/{Y%4d{x",
                get_curr_con(ch),
                ch->pcdata->perm_con,
                check_stat_advance (ch, APPLY_CON) ? ' ' : '+',
                ch->move < ch->max_move/10 ? 'R' : 'Y',
                ch->move,
                ch->max_move );
        strcat( buf, tmp );

        if( ch->sub_class == SUB_CLASS_WEREWOLF )
        {
                sprintf( tmp, "      Rage:       {R%3d{x/{R%3d{x",
                        ch->rage,
                        ch->max_rage );
                strcat( buf, tmp );
        }
        else if ( ch->sub_class == SUB_CLASS_VAMPIRE )
        {
                sprintf( tmp, "      Blood:      {R%3d{x/{R%3d{x",
                        ch->rage,
                        ch->max_rage );
                strcat( buf, tmp );
        }
        strcat (buf, "\n\r");
}


/*
 *  Check if room if safe, arena or unsafe for pkill
 *  Gez 2000
 */
void do_safe(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch) || !ch->in_room)
                return;

        else if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                send_to_char("You are in the Arena and may be attacked by anyone.\n\r", ch);

        else if (!ch->clan && !IS_SET(ch->status, PLR_RONIN))
        {
                send_to_char("You are not ronin nor in a clan: you are always safe.\n\r", ch);

                if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                        send_to_char("This location is always safe to everyone.\n\r", ch);
                else
                        send_to_char("However, this location is not normally safe for pkillers.\n\r", ch);
        }

        else if (ch->level < 15)
        {
                send_to_char("You are below level 15: you are always safe.\n\r", ch);

                if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                        send_to_char("This location is always safe to everyone.\n\r", ch);
                else
                        send_to_char("However, this location is not normally safe for pkillers.\n\r", ch);
        }

        else if (IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                 || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                 send_to_char("You are safe here.\n\r", ch);

        else
                send_to_char("You are UNSAFE here.\n\r", ch);
}


/*
 *  Print 'who' or 'whois' character information; Gez 2000
 */
void print_who_data (CHAR_DATA *ch, char *buf)
{
        char const *class;
        bool idle = FALSE;
        bool note = FALSE;

        const char *class_colours[]
                = { "{Y", "{G", "{B", "{M", "{W", "{R", "{C", "{w" };

        const char *idle_flag = "{r<{RIdle{x{r>{x ";
        const char *note_flag = "{g<{GNote{x{g>{x ";
        const char *quiet_flag = "{g<{GQuiet{x{g>{x ";
        const char *afk_flag = "{g<{GAFK{x{g>{x ";
        const char *leader_flag = "{c[{CLeader{x{c]{x ";
        const char *guide_flag = "{c[{CGuide{x{c]{x ";
        const char *killer_flag = "{w({WKILLER{x{w){x ";
        const char *thief_flag = "{b({BTHIEF{x{b){x ";
        const char *ronin_flag = "{r({RRONIN{x{r){x ";
        const char *hunted_flag = "{Y({W${Y){x ";
        const char *wizinvis_flag = "{c(Wizinvis){x ";

        if (IS_NPC(ch))
        {
                buf[0] = '\0';
                return;
        }

        class = class_table[ch->class].show_name;

        if (ch->level > LEVEL_HERO)
        {
           switch (ch->level)
           {
               default: break;
               case L_IMM: class = "   GOD    "; break;
               case L_DIR: class = " DIRECTOR "; break;
               case L_SEN: class = "  SENIOR  "; break;
               case L_JUN: class = "  JUNIOR  "; break;
               case L_APP: class = "APPRENTICE"; break;
               case L_BUI: class = " BUILDER  "; break;
               case L_HER: class = "   HERO   "; break;
           }
        }

        if (ch->timer > 5)
                idle = TRUE;

        if (ch->desc && ch->desc->connected >= CON_NOTE_TO
            && ch->desc->connected <= CON_NOTE_FINISH)
                note = TRUE;

        if (ch->level < LEVEL_HERO)
        {
                sprintf (buf, "{x[{w%s{x {W%2d{x %s%-9s{x {%c%s{x] %s%s%s%s%s%s%s%s%s%s%s%s%s\n\r",
                         race_table[ch->race].who_name,
                         ch->level,
                         class_colours[ch->class],
                         ch->sub_class ? sub_class_table[ch->sub_class].show_name : class,
                         clan_table[ch->clan].colour_code,
                         IS_SET(ch->status, PLR_RONIN) ? "Ron" : clan_table[ch->clan].who_name,
                         idle ? idle_flag : "",
                         note ? note_flag : "",
                         ch->silent_mode ? quiet_flag : "",
                         IS_SET(ch->act, PLR_AFK) ? afk_flag : "",
                         IS_SET(ch->status, PLR_KILLER) ? killer_flag : "",
                         IS_SET(ch->status, PLR_THIEF) ? thief_flag : "",
                         IS_SET(ch->status, PLR_RONIN) ? ronin_flag : "",
                         IS_SET(ch->status, PLR_HUNTED) ? hunted_flag : "",
                         IS_SET(ch->act, PLR_LEADER) ? leader_flag : "",
                         IS_SET(ch->act, PLR_GUIDE) ? guide_flag : "",
                         clan_title[ch->clan][ch->clan_level],
                         ch->name,
                         ch->pcdata->title);
        }

        else if (ch->level == LEVEL_HERO)
        {
                sprintf (buf, "[{w%s{x %s%-9s{x {%c%s{x] %s%s%s%s%s%s%s%s%s%s%s%s\n\r",
                         race_table[ch->race].who_name,
                         class_colours[ch->class],
                         ch->sub_class ? sub_class_table[ch->sub_class].show_name : class,
                         clan_table[ch->clan].colour_code,
                         IS_SET(ch->status, PLR_RONIN) ? "Ron" : clan_table[ch->clan].who_name,
                         note ? note_flag : "",
                         ch->silent_mode ? quiet_flag : "",
                         IS_SET(ch->act, PLR_AFK) ? afk_flag : "",
                         IS_SET(ch->status, PLR_KILLER) ? killer_flag : "",
                         IS_SET(ch->status, PLR_THIEF) ? thief_flag : "",
                         IS_SET(ch->status, PLR_RONIN) ? ronin_flag : "",
                         IS_SET(ch->status, PLR_HUNTED) ? hunted_flag : "",
                         IS_SET(ch->act, PLR_LEADER) ? leader_flag : "",
                         IS_SET(ch->act, PLR_GUIDE) ? guide_flag : "",
                         clan_title[ch->clan][ch->clan_level],
                         ch->name,
                         ch->pcdata->title);
        }

        else
        {
                sprintf (buf, "[{W%s{x] %s%s%s%s{Y%s{x%s\n\r",
                         class,
                         note ? note_flag : "",
                         IS_SET(ch->act, PLR_AFK) ? afk_flag : "",
                         ch->silent_mode ? quiet_flag : "",
                         IS_SET(ch->act, PLR_WIZINVIS) ? wizinvis_flag : "",
                         ch->name,
                         ch->pcdata->title);
        }
}

void do_allow_look(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (ch->pcdata->allow_look)
        {
                send_to_char("Players will no longer be able to view your equipment.\n\r", ch);
                ch->pcdata->allow_look = 0;
        }
        else
        {
                send_to_char("Players can now view your equipment.\n\r", ch);
                ch->pcdata->allow_look = 1;
        }
}


/* EOF act_info.c */
