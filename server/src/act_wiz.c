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

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>


BAN_DATA *ban_free;
BAN_DATA *ban_list;

/* Comparison function for qsort, used to sort wizhelp alphabetically */
int compare_skills(const void *a, const void *b) {
    int sn1 = *(const int *)a;
    int sn2 = *(const int *)b;

    return strcmp(skill_table[sn1].name, skill_table[sn2].name);
}

void do_wizhelp(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *rch;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int sn;
    int col;
    int valid_skills[MAX_SKILL];
    int valid_skill_count = 0;

    rch = get_char(ch);

    if (!authorized(rch, gsn_wizhelp))
        return;

    buf1[0] = '\0';
    col = 0;

    sprintf(buf1, "=========================================================\n\r{WImm skills for level equivalent to %s (level %d){x\n\r=========================================================\n\r",
            extra_level_name(ch), get_trust(ch));

    /* Collect all valid skills */
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (!skill_table[sn].name)
            break;

        if (rch->pcdata->learned[sn] >= 100
            && wiz_do(ch, skill_table[sn].name)
            && skill_table[sn].prac_type == TYPE_WIZ)
        {
            valid_skills[valid_skill_count++] = sn;
        }
    }

    /* Sort the valid skills by their name */
    qsort(valid_skills, valid_skill_count, sizeof(int), compare_skills);

    /* Format the sorted skills for output */
    for (int i = 0; i < valid_skill_count; i++)
    {
        sn = valid_skills[i];
        sprintf(buf, "%-19s", skill_table[sn].name);
        strcat(buf1, buf);
        if (++col % 4 == 0)
            strcat(buf1, "\n\r");
    }

    if (col % 4 != 0)
        strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}


void do_bamfin( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_bamfin ) )
                return;

        if ( !IS_NPC( ch ) )
        {
                if ( longstring( ch, argument ) )
                        return;

                smash_tilde( argument );
                free_string( ch->pcdata->bamfin );
                ch->pcdata->bamfin = str_dup( argument );
                send_to_char( "Ok.\n\r", ch );
        }

        return;
}


void do_bamfout( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_bamfout ) )
                return;

        if ( !IS_NPC( ch ) )
        {
                if ( longstring( ch, argument ) )
                        return;

                smash_tilde( argument );
                free_string( ch->pcdata->bamfout );
                ch->pcdata->bamfout = str_dup( argument );
                send_to_char( "Ok.\n\r", ch );
        }

        return;
}

void do_resolve(CHAR_DATA *ch, char *argument) {
    char ip_address[MAX_INPUT_LENGTH];
    struct sockaddr_in sa;
    char host[NI_MAXHOST];

    if ( !authorized( ch, gsn_resolve ) )
        return;

    if ( IS_NPC( ch ) )
        return;

    // Check if an argument is provided
    if (argument[0] == '\0') {
        send_to_char("Syntax: resolve <<IP address>\n\r", ch);
        return;
    }

    // Get the IP address from the argument
    strncpy(ip_address, argument, MAX_INPUT_LENGTH - 1);
    ip_address[MAX_INPUT_LENGTH - 1] = '\0';

    // Populate the sockaddr_in structure
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    // Convert IP address to binary form
    if (inet_pton(AF_INET, ip_address, &sa.sin_addr) <= 0) {
        send_to_char("Invalid IP address.\n\r", ch);
        return;
    }

    // Resolve the hostname
    if (getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), NULL, 0, 0) != 0) {
        send_to_char("Unable to resolve hostname.\n\r", ch);
        return;
    }

    // Send the resolved hostname back to the IMM
    char buf[MAX_STRING_LENGTH];
    snprintf(buf, MAX_STRING_LENGTH, "The hostname for IP %s is: %s\n\r", ip_address, host);
    send_to_char(buf, ch);
}


void do_deny( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_deny ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Deny whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        SET_BIT( victim->act, PLR_DENY );
        send_to_char( "You are denied access.\n\r", victim );
        send_to_char( "OK.\n\r", ch );
        do_quit( victim, "" );

        return;
}


void do_disconnect( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        CHAR_DATA       *victim;
        DESCRIPTOR_DATA *d;
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_disconnect ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Disconnect whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( !victim->desc )
        {
                act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
                return;
        }

        for ( d = descriptor_list; d; d = d->next )
        {
                if ( d == victim->desc )
                {
                        close_socket( d );
                        send_to_char( "Ok.\n\r", ch );
                        return;
                }
        }

        bug( "Do_disconnect: desc not found.", 0 );
        send_to_char( "Descriptor not found!\n\r", ch );
        return;
}

void do_protocols( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        CHAR_DATA       *victim;
        DESCRIPTOR_DATA *d;
        char buf[MAX_STRING_LENGTH];
        char tmp[MAX_STRING_LENGTH];
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_disconnect ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Protocols for  whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( !victim->desc )
        {
                act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
                return;
        }

        buf[0] = '\0';

        for ( d = descriptor_list; d; d = d->next )
        {
                if ( d == victim->desc )
                {
                        sprintf( tmp, "Protocols Negotiated for %s\n\r bATCP: %5s\n\r bMSDP: %5s\n\r bMXP:  %5s\n\r bMSP:  %5s\n\r bGMCP: %5s\n\r Client: %s Version: %s\n\r",
                        d->character->name,
                        d->pProtocol->bATCP  ? "TRUE" : "false",
                        d->pProtocol->bMSDP  ? "TRUE" : "false",
                        d->pProtocol->bMXP ? "TRUE" : "false",
                        d->pProtocol->bMSP  ? "TRUE" : "false",
                        d->pProtocol->bGMCP  ? "TRUE" : "false",
                        d->pProtocol->pVariables[eMSDP_CLIENT_ID]->pValueString,
                        d->pProtocol->pVariables[eMSDP_CLIENT_VERSION]->pValueString);
                        strcat (buf, tmp);
                        send_to_char ( buf, ch );
                        return;
                }
        }


        bug( "Do_disconnect: desc not found.", 0 );
        send_to_char( "Descriptor not found!\n\r", ch );
        return;
}

void do_killsocket( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        DESCRIPTOR_DATA *d;
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_killsocket ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Kill which socket?\n\r", ch );
                return;
        }

        if ( !is_number(arg) )
        {
                send_to_char( "killsocket <<socket #>\n\r", ch);
                return;
        }

        for ( d = descriptor_list; d; d = d->next )
        {
                if ( d->descriptor == atoi(arg) )
                {
                        if ( d->connected != CON_PLAYING )
                        {
                                close_socket( d );
                                send_to_char( "Ok.\n\r", ch );
                                return;
                        }
                        else
                        {
                                send_to_char("That character is playing.  Use disconnect.\n\r" ,ch);
                                return;
                        }
                }
        }

        send_to_char( "Socket not found!\n\r", ch );
        return;
}


void do_pardon(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char arg1 [ MAX_INPUT_LENGTH ];
        char arg2 [ MAX_INPUT_LENGTH ];

        rch = get_char(ch);

        if (!authorized(rch, gsn_pardon ))
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char( "Syntax: pardon <<character> <<killer|thief|hunted|deserter>.\n\r", ch );
                return;
        }

        if (!( victim = get_char_world(ch, arg1)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim ))
        {
                send_to_char("Not on NPCs.\n\r", ch);
                return;
        }

        if (!str_cmp(arg2, "killer"))
        {
                if (IS_SET(victim->status, PLR_KILLER))
                {
                        REMOVE_BIT(victim->status, PLR_KILLER);
                        send_to_char("<15>Killer<0> flag removed.\n\r", ch);
                        send_to_char("You are no longer a <15>KILLER<0>.\n\r", victim);
                }
                return;
        }

        if (!str_cmp( arg2, "thief"))
        {
                if (IS_SET( victim->status, PLR_THIEF))
                {
                        REMOVE_BIT(victim->status, PLR_THIEF);
                        send_to_char("<12>Thief<0> flag removed.\n\r", ch);
                        send_to_char("You are no longer a <12>THIEF<0>.\n\r", victim);
                }
                return;
        }

        if (!str_cmp(arg2, "deserter"))
        {
                if (IS_SET( victim->status, PLR_LEFT_CLAN))
                {
                        REMOVE_BIT(victim->status, PLR_LEFT_CLAN);
                        send_to_char("Deserter flag removed.\n\r", ch);
                        send_to_char("You are no longer considered a clan deserter.\n\r", victim);
                }
                return;
        }

        if (!str_cmp(arg2, "hunted"))
        {
                if (IS_SET( victim->status, PLR_HUNTED))
                {
                        REMOVE_BIT(victim->status, PLR_HUNTED);
                        victim->pcdata->bounty = 0;
                        send_to_char("<11>Hunted<0> flag removed.\n\r", ch);
                        send_to_char("You are no longer <11>HUNTED<0>.\n\r", victim);
                }
                return;
        }

        send_to_char("Syntax: pardon <<character> <<killer|thief|hunted|deserter>.\n\r", ch);
        return;
}


void do_echo( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_echo ) )
                return;

        if ( argument[0] == '\0' )
        {
                send_to_char( "Echo what?\n\r", ch );
                return;
        }

        strcat( argument, "\n\r" );
        send_to_all_char( argument );

        return;
}


void do_recho( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        DESCRIPTOR_DATA *d;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_recho ) )
                return;

        if ( argument[0] == '\0' )
        {
                send_to_char( "Recho what?\n\r", ch );
                return;
        }

        for ( d = descriptor_list; d; d = d->next )
        {
                if ( d->connected == CON_PLAYING
                    && d->character->in_room == ch->in_room )
                {
                        send_to_char( argument, d->character );
                        send_to_char( "\n\r",   d->character );
                }
        }

        return;
}


ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;

        if ( is_number( arg ) )
                return get_room_index( atoi( arg ) );

        if ( ( victim = get_char_world( ch, arg ) ) )
                return victim->in_room;

        if ( ( obj = get_obj_world( ch, arg ) ) )
                return obj->in_room;

        return NULL;
}


ROOM_INDEX_DATA *find_qlocation( CHAR_DATA *ch, char *arg, int vnum )
{
        CHAR_DATA *victim;

        if ( ( victim = get_qchar_world( ch, arg, vnum ) ) )
                return victim->in_room;

        return NULL;
}


void do_transfer( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        CHAR_DATA       *victim;
        DESCRIPTOR_DATA *d;
        ROOM_INDEX_DATA *location;
        char             arg1 [ MAX_INPUT_LENGTH ];
        char             arg2 [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_transfer ) )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Transfer whom (and where)?\n\r", ch );
                return;
        }

        if ( !str_cmp( arg1, "all" ) )
        {
                for ( d = descriptor_list; d; d = d->next )
                {
                        if ( d->connected == CON_PLAYING
                            && d->character != ch
                            && d->character->in_room
                            && can_see( ch, d->character ) )
                        {
                                char buf [ MAX_STRING_LENGTH ];

                                sprintf( buf, "%s %s", d->character->name, arg2 );
                                do_transfer( ch, buf );
                        }
                }
                return;
        }

        /*
         * Thanks to Grodyn for the optional location parameter.
         */
        if ( arg2[0] == '\0' )
        {
                location = ch->in_room;
        }
        else
        {
                if ( !( location = find_location( ch, arg2 ) ) )
                {
                        send_to_char( "No such location.\n\r", ch );
                        return;
                }

                if ( room_is_private( location ) )
                {
                        send_to_char( "That room is private right now.\n\r", ch );
                        return;
                }
        }

        if ( !( victim = get_char_world( ch, arg1 ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if (!victim->in_room)
        {
                send_to_char( "They are in limbo.\n\r", ch );
                return;
        }

        if (victim->fighting)
                stop_fighting( victim, TRUE );

        if (victim->mount)
                strip_mount (victim);

        if (victim->rider)
                strip_mount (victim->rider);

        /* if they were swallowed, some text indicating escape + stripping effects */
        if (IS_AFFECTED(victim, AFF_SWALLOWED))
        {
                strip_swallow(victim);
        }

        act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );

        if (is_entered_in_tournament(victim) && tournament_status == TOURNAMENT_STATUS_RUNNING)
        {
                disqualify_tournament_entrant(victim, "left the Arena");
                check_for_tournament_winner();
        }

        char_from_room( victim );
        char_to_room( victim, location );
        act( "$n arrives in a puff of smoke.", victim, NULL, NULL, TO_ROOM );
        if ( ch != victim )
                act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
        do_look( victim, "auto" );
        send_to_char( "Ok.\n\r", ch );
}


void do_at( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        CHAR_DATA       *wch;
        ROOM_INDEX_DATA *location;
        ROOM_INDEX_DATA *original;
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_at ) )
                return;

        argument = one_argument( argument, arg );

        if ( arg[0] == '\0' || argument[0] == '\0' )
        {
                send_to_char( "At where what?\n\r", ch );
                return;
        }

        if ( !( location = find_location( ch, arg ) ) )
        {
                send_to_char( "No such location.\n\r", ch );
                return;
        }

        if ( room_is_private( location ) )
        {
                send_to_char( "That room is private right now.\n\r", ch );
                return;
        }

        original = ch->in_room;
        char_from_room( ch );
        char_to_room( ch, location );
        interpret( ch, argument );

        for ( wch = char_list; wch; wch = wch->next )
        {
                if ( wch == ch )
                {
                        char_from_room( ch );
                        char_to_room( ch, original );
                        break;
                }
        }
}


void do_goto( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        ROOM_INDEX_DATA *location;
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_goto ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Goto where?\n\r", ch );
                return;
        }

        if ( !( location = find_location( ch, arg ) ) )
        {
                send_to_char( "No such location.\n\r", ch );
                return;
        }

        if ( ch->fighting )
                stop_fighting( ch, TRUE );

        if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
        {
                act( "$n $T.", ch, NULL,
                    ( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
                    ? ch->pcdata->bamfout : "leaves in a swirling mist.",  TO_ROOM );
        }

        /*  Don't forget your mount!  Gezhp  */
        if (ch->mount)
        {
                act ("$n vanishes before your eyes.", ch->mount, NULL, ch, TO_NOTVICT );
                char_from_room (ch->mount);
                char_to_room (ch->mount, location );
        }

        char_from_room( ch );
        char_to_room( ch, location );

        if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
        {
                act( "$n $T.", ch, NULL,
                    ( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
                    ? ch->pcdata->bamfin : "appears in a swirling mist.", TO_ROOM );
        }

        if (ch->mount)
        {
                act ("$n appears out of thin air.", ch->mount, NULL, ch, TO_NOTVICT );
                char_from_room (ch->mount);
                char_to_room (ch->mount, location );
        }

        do_look( ch, "auto" );
        return;
}


void do_rstat( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA                *obj;
        CHAR_DATA               *rch;
        ROOM_INDEX_DATA         *location;
        char                    buf  [ MAX_STRING_LENGTH ];
        char                    buf1 [ MAX_STRING_LENGTH ];
        /*char                    buf2 [ MAX_STRING_LENGTH ];*/
        char                    arg  [ MAX_INPUT_LENGTH  ];
        int                     door;
        unsigned long int       next;
        int                     chroom_cnt      = 1;
        int                     objroom_cnt     = 1;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_rstat ) )
                return;

        one_argument( argument, arg );
        location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
        if ( !location )
        {
                send_to_char( "No such location.\n\r", ch );
                return;
        }

        if ( ch->in_room != location && room_is_private( location ) )
        {
                send_to_char( "That room is private right now.\n\r", ch );
                return;
        }

        buf1[0] = '\0';

        sprintf( buf, "Room: {W%s{x\n\rArea: {Y%s{x\n\r",
                location->name,
                location->area->name);
        strcat( buf1, buf );

        sprintf( buf, "Exp modifier: {W%d{x  ",
                location->area->exp_modifier);
        strcat( buf1, buf );

        sprintf( buf,
                "Vnum: {R%d{x  Sector: {G%s{x [{W%d{x]  Light: {W%d{x\n\r",
                location->vnum,
        sector_name(location->sector_type),
                location->sector_type,
                location->light );
        strcat( buf1, buf );

        if (location->area->area_flags)
        {
                sprintf( buf, "Area flags (num): {W");
                strcat( buf1, buf );
                bit_explode(ch, buf, location->area->area_flags);
                strcat( buf1, buf );

                strcat(buf1, "{x\n\r");

                sprintf( buf, "Area flags (txt):{R");

                strcat( buf1, buf );

                for (next = 1; next < BIT_MAX; next *= 2)
                {
                        if (IS_SET(location->area->area_flags, next))
                        {
                                strcat(buf1, " ");
                                strcat(buf1, area_flag_name(next));
                        }
                }

                strcat(buf1, "{x\n\r");

        }
        else {
                strcat(buf1, "Area flags: {Rnone {x[{W0{x]\n\r");
        }

        if (location->room_flags)
        {
                sprintf( buf, "Room flags (num): {W");
                strcat( buf1, buf );
                bit_explode(ch, buf, location->room_flags);
                strcat( buf1, buf );

                strcat(buf1, "{x\n\r");

                strcat(buf1, "Room flags (txt):{R");

                for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                {
                        /*sprintf(buf2,"looping: %lu \r\n", next);
                        log_string(buf2);*/

                        if (IS_SET(location->room_flags, next))
                        {
                                strcat(buf1, " ");
                                strcat(buf1, room_flag_name(next));
                        }
                }

                strcat(buf1, "{x\n\r");
        }
        else {
                strcat(buf1, "Room flags: {Rnone {x[{W0{x]\n\r");
        }

        sprintf( buf,
                "Description:\n\r  {C%s{x",
                location->description );
        strcat( buf1, buf );

        if ( location->extra_descr )
        {
                EXTRA_DESCR_DATA *ed;

                strcat( buf1, "Extra description keywords:\n\r  {W" );
                for ( ed = location->extra_descr; ed; ed = ed->next )
                {
                        strcat( buf1, ed->keyword );
                        if ( ed->next )
                                strcat( buf1, " " );
                }
                strcat( buf1, "{x\n\r" );
        }

        strcat( buf1, "Characters:{W" );

        /* Yes, we are reusing the variable rch.  - Kahn */
        for ( rch = location->people; rch; rch = rch->next_in_room )
        {
                chroom_cnt++;
                if ( chroom_cnt <= MAX_RSTAT_CHAR )
                {
                        if (!IS_NPC(rch))
                        {
                                strcat( buf1, "{Y " );
                                sprintf( buf, "%s{x,",
                                        rch->name );
                                strcat( buf1, buf );
                        }
                        else {
                                strcat( buf1, "{W " );
                                sprintf( buf, "%s{x",
                                        rch->short_descr );
                                strcat( buf1, buf );
                                sprintf(buf, "{x ({R%d{x){W,", rch->pIndexData->vnum);
                                strcat(buf1, buf);
                        }
                }
                else {
                        strcat( buf1, " ...[{WMORE{x] " );
                        break;
                }
        }

        buf1[strlen(buf1)-1] = 0; /* Deletes last char of buffer (a comma)... remember this one. */
        strcat( buf1, "{x\n\r" );

        if ((obj = location->contents))
        {
                strcat( buf1, "{xObjects:" );
        }

        for ( obj = location->contents; obj; obj = obj->next_content )
        {
                objroom_cnt++;

                if ( objroom_cnt <= MAX_RSTAT_OBJ )
                {
                        strcat( buf1, " {W" );
                        sprintf( buf, "%s",
                                obj->short_descr );
                        strcat( buf1, buf );
                        sprintf( buf, " {x({R%d{x),", obj->pIndexData->vnum );
                        strcat( buf1, buf );
                }
                else {
                        strcat( buf1, " ...[{WMORE{x] " );
                        break;
                }
        }
        buf1[strlen(buf1)-1] = 0;

        if ((obj = location->contents))
        {
                strcat( buf1, "\n\r" );
        }

        for ( door = 0; door <= 5; door++ )
        {
                EXIT_DATA *pexit;

                if ( ( pexit = location->exit[door] ))
                {
                        sprintf( buf,
                                "Door: {W%d{x [{G%-5s{x] To: {R%-5d{x Key: {Y%-5d{x Exit flags: {W%d{x\n\r",
                                door,
                                directions[door].name,
                                pexit->to_room ? pexit->to_room->vnum : 0,
                                pexit->key,
                                pexit->exit_info );

                        strcat( buf1, buf );

                        if ( pexit->keyword[0]    != '\0'
                        &&  pexit->description[0] != '\0')
                        {       sprintf( buf,
                                "Keyword: {W%s{x  \n\rDescription: \n\r{c%s{x",
                                pexit->keyword,
                                pexit->description[0] != '\0' ? pexit->description
                                : "{c(none){x\n\r");
                                strcat( buf1, buf );
                        }
                }
        }

        send_to_char( buf1, ch );
        return;
}


void do_onstat( CHAR_DATA *ch, char *argument )
{
        OBJ_INDEX_DATA *pObjIndex;
        CHAR_DATA   *rch;
                AFFECT_DATA *paf;
        char         buf  [ MAX_STRING_LENGTH ];
        char         buf1 [ MAX_STRING_LENGTH ];
        char         arg  [ MAX_INPUT_LENGTH  ];


        rch = get_char( ch );

        buf[0] = '\0';
        buf1[0] = '\0';

        if ( !authorized( rch, gsn_mload ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' || !is_number( arg ) )
        {
                send_to_char( "Syntax: onstat <<vnum>.\n\r", ch );
                return;
        }

        if ( !( pObjIndex = get_obj_index( atoi( arg ) ) ) )
        {
                send_to_char( "No object has that vnum.\n\r", ch );
                return;
        }

        sprintf( buf, "Vnum: {R%d{x\n\r", pObjIndex->vnum);
        strcat( buf1, buf );

        sprintf( buf, "Short description: {W%s{x\n\rKeywords: {W%s{x\n\r",
                pObjIndex->short_descr,
                pObjIndex->name );
        strcat( buf1, buf );

        sprintf( buf, "Level: {W%d{x  Cost: {W%d{x \n\r",
                pObjIndex->level, pObjIndex->cost );
        strcat( buf1, buf );

        for ( paf = pObjIndex->affected; paf; paf = paf->next )
        {
                sprintf( buf, "Affects {Y%s{x by {Y%d{x\n\r",
                        affect_loc_name( paf->location ), paf->modifier );
                strcat( buf1, buf );
        }

        send_to_char( buf1, ch );
        return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA            *obj;
        OBJ_INDEX_DATA      *pObjIndex;
        OBJSET_INDEX_DATA   *pObjSetIndex;
        CHAR_DATA           *rch;
        AFFECT_DATA         *paf;
        extern char*        body_part_types[];
        char                buf  [ MAX_STRING_LENGTH ];
        char                buf1 [ MAX_STRING_LENGTH ];
        char                arg  [ MAX_INPUT_LENGTH  ];
        int                 has_bp_flag;
        unsigned long int   i;
        unsigned long int   next;

        has_bp_flag = 0;
        rch = get_char( ch );

        if ( !authorized( rch, gsn_ostat ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Ostat what?\n\r", ch );
                return;
        }

        buf1[0] = '\0';

        if ( !( obj = get_obj_world( ch, arg ) ) )
        {
                send_to_char( "Nothing like that in these realms.\n\r", ch);
                return;
        }

        pObjIndex = obj->pIndexData;

        sprintf( buf, "Vnum: {G%d{x\n\r",
                obj->pIndexData->vnum);
        strcat( buf1, buf );


        sprintf( buf, "Short description: {W%s{x\n\rKeywords: {W%s{x\n\rLong description: {W%s{x\n\r",
                obj->short_descr,
                obj->name,
                obj->description );
        strcat( buf1, buf );

        sprintf( buf, "Level: {W%d{x  Cost: {W%d{x  Timer: {W%d{x ({R%d{x) Number: {W%d{x/{W%d{x  Weight: {W%d{x/{W%d{x\n\r",
                obj->level, obj->cost, obj->timer, obj->timermax, 1, get_obj_number( obj ),
                obj->weight, get_obj_weight( obj ) );
        strcat( buf1, buf );

        sprintf( buf, "Type: {G%s{x [{W%d{x] ",item_type_name( obj ), obj->item_type);
        strcat( buf1, buf );

        /* Show liquid type if drink container or fountain. -- Owl 1/7/22 & 9/12/23 */
        if (obj->item_type == 17 || obj->item_type == 25)
        {
                sprintf( buf, "\n\rLiquid: {G%s{x [{W%d{x]  Appearance: {G%s{x  Poisoned: {G%s{x",
                        liq_table[obj->value[2]].liq_name,
                        obj->value[2],
                        liq_table[obj->value[2]].liq_color,
                        (obj->value[3] != 0 ? "{MYes{x" : "{GNo{x"));
                strcat( buf1, buf );
        }

        sprintf( buf, "\n\r");
        strcat( buf1, buf );

        if (obj->extra_flags)
        {
                strcat( buf1, "Item flags (num): {W");
                /*sprintf(buf, "extra flags for item are %lu\n\r", obj->extra_flags);
                log_string(buf);*/
                bit_explode( ch, buf, obj->extra_flags);
                strcat( buf1, buf );
                strcat( buf1, "{x\n\r");
                strcat( buf1, "Item flags (txt):{R");

                for (i = 1; i > 0 && i <= BIT_MAX; i *= 2)
                {
                        if (IS_SET(obj->extra_flags, i))
                        {
                                strcat(buf1, " ");
                                strcat(buf1, extra_bit_name(i));
                                if ( i == ITEM_BODY_PART)
                                {
                                        has_bp_flag = 1;
                                }
                        }
                }
                strcat(buf1, "{x\n\r");

                /* If there's a body part item that we might care about for combat,
                   particularly do_target, show details of type. 29/9/24 -- Owl */

                if ( ( has_bp_flag)
                &&   ( obj->item_type == ITEM_ARMOR
                    || obj->item_type == ITEM_WEAPON
                    || obj->item_type == ITEM_LIGHT ) )
                {
                        strcat( buf1, "Body part type: {G");

                        if (obj->item_type == ITEM_ARMOR)
                        {
                                sprintf( buf, "%s{x [{W%d{x] ",
                                        body_part_types[obj->value[1]],
                                        obj->value[1]);
                                strcat( buf1, buf );
                        }

                        if ( obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_LIGHT )
                        {
                                sprintf( buf, "%s{x [{W%d{x] ",
                                        body_part_types[obj->value[0]],
                                        obj->value[0]);
                                strcat( buf1, buf );
                        }

                        strcat(buf1, "{x\n\r");
                }
        }

        sprintf( buf, "Wear bits: {W" );
        strcat( buf1, buf );

        if (obj->wear_flags)
        {
                bit_explode(ch, buf, obj->wear_flags);
                strcat( buf1, buf );
                sprintf( buf, " {x[{G" );
                strcat( buf1, buf );

                for (next = 1; next <= BIT_17; next *= 2)
                {
                        if (IS_SET(obj->wear_flags, next))
                        {
                                strcat(buf1, " ");
                                strcat(buf1, wear_flag_name(next));
                        }
                }
                sprintf( buf, " {x]" );
                strcat( buf1, buf );
        }
        else {
                sprintf( buf, "none [0]." );
                strcat( buf1, buf );
        }

        sprintf( buf, "  Currently worn: {W%d{x [ {G", obj->wear_loc );
        strcat( buf1, buf );

        sprintf( buf, "%s {x]\n\r", wear_location_name(obj->wear_loc) );
        strcat( buf1, buf );

        if (!obj->carried_by)
        {
                sprintf( buf, "In room: {R%d{x  In object: {W%s{x\n\r",
                !obj->in_room    ?            0 : obj->in_room->vnum,
                !obj->in_obj     ? "{W(none){x" : obj->in_obj->short_descr);
                strcat( buf1, buf );

        }
        else if(IS_NPC(obj->carried_by))
        {
                sprintf( buf, "In room: {R%d{x  In object: {W%s{x  \n\rCarried by: {W%s{x ({G%s{x) [{R%d{x]  \n\r",
                        !obj->in_room    ?            0 : obj->in_room->vnum,
                        !obj->in_obj     ? "{W(none){x" : obj->in_obj->short_descr,
                        obj->carried_by->short_descr,
                        obj->carried_by->name,
                        obj->carried_by->pIndexData->vnum);
                strcat( buf1, buf );
        }
        else
        {
                sprintf( buf, "In room: {R%d{x  In object: {W%s{x  Carried by: {W%s{x\n\r",
                !obj->in_room    ?            0 : obj->in_room->vnum,
                !obj->in_obj     ? "{W(none){x" : obj->in_obj->short_descr,
                obj->carried_by->name);
                strcat( buf1, buf );
        }

        sprintf( buf, "Max instances: {W%d{x  Spawn count: {W%d{x\n\r", pObjIndex->max_instances, pObjIndex->spawn_count );
        strcat( buf1, buf );

        if (obj->item_type == 52)
        {
                sprintf( buf, "Benefit:       {W%3d%%{x ( value{W[{G0{W]{x ){x\n\r",
                        obj->value[0]);
                strcat( buf1, buf );
                sprintf( buf, "Max benefit:   {W%3d%%{x ( value{W[{G1{W]{x ){x\n\r",
                        obj->value[1]);
                strcat( buf1, buf );
                sprintf( buf, "Thirst mod:    {W%3d%%{x ( value{W[{G2{W]{x ){x\n\r",
                        obj->value[2]);
                strcat( buf1, buf );
                sprintf( buf, "Speed:         {W%3d{x  ( value{W[{G3{W]{x ){x\n\r",
                        obj->value[3]);
                strcat( buf1, buf );

        }
        else if (obj->item_type == 53)
        {
                sprintf( buf, "Uses remain:   {W%3d{x ( value{W[{G0{W]{x ){x\n\r",
                        obj->value[0]);
                strcat( buf1, buf );
                sprintf( buf, "Max uses:      {W%3d{x ( value{W[{G1{W]{x ){x\n\r",
                        obj->value[1]);
                strcat( buf1, buf );
                sprintf( buf, "Clean:         {W%3d{x ( value{W[{G2{W]{x ){x\n\r",
                        obj->value[2]);
                strcat( buf1, buf );
                sprintf( buf, "Clean max:     {W%3d{x ( value{W[{G3{W]{x ){x\n\r",
                        obj->value[3]);
                strcat( buf1, buf );

        }
        else {
                sprintf( buf, "Values: {W%d %d %d %d{x\n\r",
                        obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
                strcat( buf1, buf );
        }

        switch ( obj->item_type )
        {
            case ITEM_PILL:
            case ITEM_PAINT:
            case ITEM_SCROLL:
            case ITEM_POTION:
                sprintf( buf, "Level {G%d{x spells of:", obj->value[0] );
                strcat( buf1, buf );

                if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[1]].name);
                        strcat(buf1, "{x'");
                }

                if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[2]].name);
                        strcat(buf1, "{x'");
                }

                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[3]].name);
                        strcat(buf1, "{x'");
                }

                strcat(buf1, "\n\r");
                break;

            case ITEM_SMOKEABLE:

                sprintf( buf, "Uses remaining: {W%d{x\n\r",
                    obj->value[0] );
                strcat( buf1, buf );

                sprintf( buf, "Spells of: ");
                strcat( buf1, buf );

                if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[1]].name);
                        strcat(buf1, "{x'");
                }

                if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[2]].name);
                        strcat(buf1, "{x'");
                }

                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                        strcat(buf1, " '{G");
                        strcat(buf1, skill_table[obj->value[3]].name);
                        strcat(buf1, "{x'");
                }

                strcat(buf1, "\n\r");
                break;

            case ITEM_FOOD:
                if (obj->value[3] != 0)
                {
                    sprintf( buf, "This food is {Mpoisoned{x\n\r");
                    strcat( buf1, buf );
                }
                break;

            case ITEM_WAND:
            case ITEM_STAFF:
            case ITEM_COMBAT_PULSE:
            case ITEM_DEFENSIVE_PULSE:
                sprintf( buf, "Has {c%d{x/{C%d{x charges of level {G%d{x '{G",
                        obj->value[2], obj->value[1], obj->value[0] );
                strcat( buf1, buf );

                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                        strcat(buf1, skill_table[obj->value[3]].name);
                else
                        strcat(buf1, "?");

                strcat(buf1, "{x'\n\r");
                break;

            case ITEM_WEAPON:
                sprintf( buf, "Damage type is {G%s{x [{W%d{x]\n\r",
                        weapon_damage_type_name(obj->value[3]),
                        obj->value[3] );
                strcat( buf1, buf );

                sprintf( buf, "Damage is {G%d{x to {G%d{x (average {W%d{x)\n\r",
                        obj->value[1], obj->value[2],
                        ( obj->value[1] + obj->value[2] ) / 2 );
                strcat( buf1, buf );
                break;

            case ITEM_ARMOR:
                sprintf( buf, "Armor class is {Y%d{x\n\r", obj->value[0] );
                strcat( buf1, buf );
                break;
        }

        if ( obj->extra_descr || obj->pIndexData->extra_descr )
        {
                EXTRA_DESCR_DATA *ed;

                strcat( buf1, "Extra description keywords: '{W" );

                for ( ed = obj->extra_descr; ed; ed = ed->next )
                {
                        strcat( buf1, ed->keyword );
                        if ( ed->next )
                                strcat( buf1, " " );
                }

                for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
                {
                        strcat( buf1, ed->keyword );
                        if ( ed->next )
                                strcat( buf1, " " );
                }

                strcat( buf1, "{x'\n\r" );
        }

        sprintf( buf, "Creation method: {G%d{x [ {W%s{x ]\n\r",
            obj->how_created,
            created_name(obj->how_created)
        );
        strcat( buf1, buf );

        if (obj->how_created >= CREATED_NO_RANDOMISER )
        {
                for ( paf = obj->affected; paf; paf = paf->next )
                {
                        sprintf( buf, "Affects {Y%s{x by {Y%d{x (Read from Obj)\n\r",
                                affect_loc_name( paf->location ), paf->modifier );
                        strcat( buf1, buf );
                }
        }

        if (obj->how_created < CREATED_NO_RANDOMISER )
        {
                for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
                {
                        sprintf( buf, "Affects {Y%s{x by {Y%d{x (Read from Index)\n\r",
                                affect_loc_name( paf->location ), paf->modifier );
                        strcat( buf1, buf );
                }
        }

        if (obj->owner[0] != '\0')
        {
                sprintf(buf, "Owner: {W%s{x\n\r", get_obj_owner(obj));
                strcat(buf1, buf);
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO))
        {
                sprintf(buf, "Ego flags (num): {W%d{x\n\rEgo flags (txt):{R", obj->ego_flags);

                if (IS_SET(obj->ego_flags, EGO_ITEM_BLOODLUST))
                        strcat (buf, " bloodlust");

                if (IS_SET(obj->ego_flags, EGO_ITEM_SOUL_STEALER))
                        strcat (buf, " soul_stealer");

                if (IS_SET(obj->ego_flags, EGO_ITEM_FIREBRAND))
                        strcat (buf, " firebrand");

                if (IS_SET(obj->ego_flags, EGO_ITEM_IMBUED))
                        strcat (buf, " imbued");

                if (IS_SET(obj->ego_flags, EGO_ITEM_BALANCED))
                        strcat (buf, " counterbalanced");

                if (IS_SET(obj->ego_flags, EGO_ITEM_BATTLE_TERROR))
                        strcat (buf, " battle_terror");

                if (IS_SET(obj->ego_flags, EGO_ITEM_CHAINED))
                        strcat (buf, " chained");

                if (IS_SET(obj->ego_flags, EGO_ITEM_STRENGTHEN))
                        strcat (buf, " strengthened");

                if (IS_SET(obj->ego_flags, EGO_ITEM_CONSTRUCTED))
                        strcat (buf, " constructed");

                if (IS_SET(obj->ego_flags, EGO_ITEM_EMPOWERED))
                        strcat (buf, " empowered");

                if (IS_SET(obj->ego_flags, EGO_ITEM_SERRATED))
                        strcat (buf, " serrated");

                if (IS_SET(obj->ego_flags, EGO_ITEM_ENGRAVED))
                        strcat (buf, " engraved");

                strcat(buf, "{x\n\r");
                strcat(buf1, buf);
        }

        sprintf(buf, "Object score: {W%d{x  Identified: %s\n\r", calc_item_score(obj), obj->identified ? "{Gtrue{x" : "{Rfalse{x");
        strcat(buf1, buf);

        if ( (pObjSetIndex = objects_objset(obj->pIndexData->vnum) ) )
        {
                sprintf(buf, "This is part of a %s set.\n\rSet tags: %s. Vnum [%d]\n\r\n\r", objset_type(pObjSetIndex->vnum), pObjSetIndex->name, pObjSetIndex->vnum);
                strcat(buf1, buf);

        }
        send_to_char( buf1, ch );
}

void do_osstat( CHAR_DATA *ch, char *argument )
{
        OBJSET_INDEX_DATA *obj;
        CHAR_DATA   *rch;
        AFFECT_DATA *paf;
        char         buf  [ MAX_STRING_LENGTH ];
        char         buf1 [ MAX_STRING_LENGTH ];
        char         arg  [ MAX_INPUT_LENGTH  ];
        int count;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_osstat ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Osstat what?\n\r", ch );
                return;
        }

        buf1[0] = '\0';

        if ( !( obj = get_objset( arg ) ) )
        {
                send_to_char( "Nothing like that in these realms..\n\r", ch);
                return;
        }

        sprintf( buf, "This is a %s set. Vnum: {R%d{x\n\r", objset_type(obj->vnum),
                obj->vnum );
        strcat( buf1, buf );

        sprintf( buf, "Short description: {W%s{x\n\rLong description: {W%s{x\n\r",
                obj->name,
                obj->description );
        strcat( buf1, buf );

        sprintf ( buf, "Vnums of items within the set: {W%d{x {W%d{x {W%d{x {W%d{x {W%d{x\n\r\n\r",
                obj->objects[0], obj->objects[1], obj->objects[2], obj->objects[3], obj->objects[4] );
        strcat( buf1, buf );

/*         num_bonus = objset_bonus_num(obj->vnum);
        sprintf( buf, "There are %d set bonuses\n\r", objset_bonus_num(obj->vnum));
        strcat( buf1, buf );


        sprintf( buf, "you need {W%d{x for set bonus 1\n\r", objset_bonus_num_pos(obj->vnum, 1) );
        strcat( buf1, buf );
                sprintf( buf, "you need {W%d{x for set bonus 2\n\r", objset_bonus_num_pos(obj->vnum, 2) );
        strcat( buf1, buf );
                sprintf( buf, "you need {W%d{x for set bonus 3\n\r", objset_bonus_num_pos(obj->vnum, 3) );
        strcat( buf1, buf );
*/
        sprintf( buf, "{CEffects Given:{x\n\r");
                strcat( buf1, buf );

        count = 0;
        for ( paf = obj->affected; paf; paf = paf->next )
        {
                count++;
                sprintf( buf, "Equip {W%d{x items to provide {Y%s{x by {Y%d{x\n\r",
                        objset_bonus_num_pos(obj->vnum, count),
                        affect_loc_name( paf->location ),
                        paf->modifier );
                strcat( buf1, buf );
  /*              sprintf( buf, "you need {W%d{x for set bonus %d\n\r", objset_bonus_num_pos(obj->vnum, count), count );
                strcat( buf1, buf );
*/
        }

   /*     for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
        {
                sprintf( buf, "Affects {Y%s{x by {Y%d{x\n\r",
                        affect_loc_name( paf->location ), paf->modifier );
                strcat( buf1, buf );
        }
*/
        strcat(buf, "{x\n\r");

        send_to_char( buf1, ch );


}

void do_mstat( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA               *rch;
        CHAR_DATA               *victim;
        AFFECT_DATA             *paf;
        MPROG_DATA              *mprg;
        char                    buf         [ MAX_STRING_LENGTH ];
        char                    buf1        [ MAX_STRING_LENGTH ];
        char                    spec_result [ MAX_STRING_LENGTH ];
        char                    arg         [ MAX_INPUT_LENGTH ];
        unsigned long int       next;
        int                     count;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_mstat ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Mstat whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        buf1[0] = '\0';

        /*
         *  Basically split this into 2 distinct routines based on whether it's a PC or
         *  an NPC, as the information you're interested in (and the ORDER you're likely
         *  to be interested in it) is sufficiently different between the two kinds of
         *  entities.  Should likely be different commands entirely, but here we are.
         *  --Owl 23/4/22
         */

        if (!IS_NPC( victim ))
        {
                /* All PLAYER mstat stuff goes here. */

                sprintf( buf, "Name: {W%s{x\n\r", victim->name );
                strcat( buf1, buf );

                if (get_trust(victim) != victim->level)
                {
                        sprintf(buf, "*** Trusted to level %d: %s ***\n\r", victim->trust,
                                extra_level_name(victim));
                        strcat (buf1, buf);
                }

                if ( victim->short_descr[0] != '\0'
                &&   victim->long_descr[0]  != '\0' )
                {
                        sprintf( buf, "Short description: {W%s{x\n\rLong description: \n\r  {W%s{x",
                                victim->short_descr,
                                victim->long_descr );
                        strcat( buf1, buf );
                }

                sprintf( buf, "Sex: {W%s{x  Race: {W%d{x ({G%s{x)  Size: {G%s{x  Level: {W%d{x  Room: {R%d{x\n\r",
                        victim->sex == SEX_MALE    ? "male"   :
                        victim->sex == SEX_FEMALE  ? "female" : "neutral",
                        victim->race,
                        race_name(victim->race),
                        race_size_name(race_table[victim->race].size),
                        victim->level,
                        !victim->in_room           ?        0 : victim->in_room->vnum );
                strcat( buf1, buf );

                sprintf( buf, "Str: {C%d{x  Int: {C%d{x  Wis: {C%d{x  Dex: {C%d{x  Con: {C%d{x\n\r",
                        get_curr_str( victim ),
                        get_curr_int( victim ),
                        get_curr_wis( victim ),
                        get_curr_dex( victim ),
                        get_curr_con( victim ) );
                strcat( buf1, buf );

                sprintf( buf, "Hp: {G%d{x/{g%d{x  Mana: {C%d{x/{c%d{x  Mv: {Y%d{x/{y%d{x  Wait: {C%d{x\n\r",
                        victim->hit,         victim->max_hit,
                        victim->mana,        victim->max_mana,
                        victim->move,        victim->max_move,
                        victim->wait);
                strcat( buf1, buf );

                sprintf( buf, "Hitroll: {R%d{x  Damroll: {R%d{x   AC: {W%d{x  Saving throw: {W%d{x Damage Mitigation: {W%d{x\n\r",
                        GET_HITROLL( victim ),
                        GET_DAMROLL( victim ),
                        GET_AC( victim ),
                        victim->saving_throw,
                        victim->damage_mitigation);
                strcat( buf1, buf );

                sprintf( buf, "Crit: {R%d{x  Swiftness: {R%d{x  Bonus: {R%d{x/{r%d{x  Slept: {R%d{x  Last recharge: {R%ld{x\n\r",
                        GET_CRIT( victim ),
                        GET_SWIFT( victim ),
                        victim->pcdata->bonus,
                        victim->pcdata->max_bonus,
                        victim->pcdata->slept,
                        victim->pcdata->last_recharge);
                strcat( buf1, buf );


                sprintf( buf, "Align: {W%d{x  Exp: {W%d{x  Class: {W%d{x ({G%s{x)  SubCl: {W%d{x ({G%s{x)\n\rAge: {W%d{x  Fame: {W%d{x  Form: {W%s{x  Aggro_dam: {R%d{x  Rage: {R%d{w/{r%d{x\n\r",
                        victim->alignment,
                        (level_table[victim->level].exp_total) - victim->exp,
                        victim->class,
                        full_class_name( victim->class ),
                        victim->sub_class,
                        full_sub_class_name( victim->sub_class ),
                        get_age( victim ),
                        !IS_NPC( victim ) ? victim->pcdata->fame : 0,
                        extra_form_name(victim->form),
                        victim->aggro_dam,
                        victim->rage,
                        victim->max_rage);
                strcat( buf1, buf );

                sprintf(buf, "Str pracs: {W%d{x  Int pracs: {W%d{x  Bank: {Y%d{x  Bounty: {Y%d{x\n\rCurrent quest points: {C%d{x  Total quest points: {C%d{x  Quest timer: {C%d{x\n\r",
                        victim->pcdata->str_prac,
                        victim->pcdata->int_prac,
                        victim->pcdata->questpoints,
                        victim->pcdata->totalqp,
                        victim->pcdata->nextquest,
                        victim->pcdata->bank,
                        victim->pcdata->bounty);
                strcat(buf1, buf);

                sprintf( buf, "Platinum: {W%d{x  Gold: {Y%d{x  Silver: %d  Copper: {y%d{x\n\r",
                        victim->plat,
                        victim->gold,
                        victim->silver,
                        victim->copper);
                strcat( buf1, buf );

                sprintf( buf, "Carry number: {C%d{x/{c%d{x  Carry weight: {C%d{x/{c%d{x  Coin weight: {W%d{x\n\r",
                        victim->carry_number,
                        can_carry_n( victim ),
                        (victim->carry_weight + victim->coin_weight),
                        can_carry_w( victim ),
                        victim->coin_weight );
                strcat( buf1, buf );

                sprintf( buf,
                        "Thirst: {W%d{x  Full: {W%d{x  Drunk: {W%d{x\n\rPosition: {G%d{x [{W%s{x]  Wimpy: {G%d{x  Page Lines: {G%d{x\n\r",
                        victim->pcdata->condition[COND_THIRST],
                        victim->pcdata->condition[COND_FULL  ],
                        victim->pcdata->condition[COND_DRUNK ],
                        victim->position,
                        position_name(victim->position),
                        victim->wimpy,
                        victim->pcdata->pagelen  );
                strcat( buf1, buf );

                sprintf( buf, "Master: {W%s{x  Leader: {W%s{x  Clan: {W%d{x ({W%s{x)\n\rFighting: {W%s{x  Mount: {W%s{x  Inside: {W%s{x\n\r",
                        victim->master      ? victim->master->name   : "(none)",
                        victim->leader      ? victim->leader->name   : "(none)",
                        !IS_NPC( victim ) ? victim->clan : 0,
                        !IS_NPC( victim ) ? clan_table[victim->clan].who_name : "none",
                        victim->fighting ? victim->fighting->name : "(none)",
                        victim->mount ? victim->mount->name : "(none)",
                        victim->inside ? victim->inside->name : "(none)");
                strcat( buf1, buf );

                sprintf( buf, "Play time: {W%d seconds{x ({G%d hours{x)  Timer: {W%d{x  Status: {W%d{x\n\r",
                        (int) victim->played,
                        ((get_age(victim)-17)*4),
                        victim->timer,
                        !IS_NPC( victim ) ? victim->status : 0);
                strcat( buf1, buf );

                if (victim->affected_by)
                {
                        sprintf(buf, "Affected by (num): {W");
                        strcat( buf1, buf );
                        bit_explode(ch, buf, victim->affected_by);
                        strcat(buf1, buf );
                        strcat(buf1, "{x\n\r");

                        strcat(buf1, "Affected by (txt):{R");

                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                        {
                                if (IS_AFFECTED(victim, next))
                                {
                                        strcat(buf1, " ");
                                        strcat(buf1, affect_bit_name(next));
                                }
                        }
                        strcat(buf1, "{x\n\r");
                }

                if (victim->act)
                {
                        sprintf(buf, "Act flags (num): {W");
                        strcat( buf1, buf );
                        bit_explode(ch, buf, victim->act);
                        strcat(buf1, buf );
                        strcat(buf1, "{x\n\r");
                        strcat(buf1, "Act flags (txt):{R");

                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                        {
                                if (IS_SET(victim->act, next))
                                {
                                        strcat(buf1, " ");
                                        strcat(buf1, pact_bit_name(next));
                                }
                        }

                        strcat(buf1, "{x\n\r");

                }

                if (deities_active() && ch->level == L_IMM)
                {
                        sprintf (buf, "Patron: {W%d{x ({W%s{x)  Deity timer: {W%d{x  "
                                 "Deity flags: {W%d{x\n\rFavour:\n\r",
                                 victim->pcdata->deity_patron,
                                 (victim->pcdata->deity_patron < 0
                                  || victim->pcdata->deity_patron >= NUMBER_DEITIES)
                                 ? "none" : deity_info_table[victim->pcdata->deity_patron].name,
                                 victim->pcdata->deity_timer,
                                 victim->pcdata->deity_flags);
                        strcat (buf1, buf);
                        count = 1;
                        for (next = 0; next < NUMBER_DEITIES; next++)
                        {
                                if (count != 5)
                                {
                                        sprintf (buf, "{W%-10s{x {G%-4d{x ",
                                                deity_info_table[next].name,
                                                victim->pcdata->deity_favour[next]);
                                        strcat (buf1, buf);
                                        ++count;
                                }
                                else {
                                        count = 1;
                                        sprintf (buf, "\n\r");
                                        strcat (buf1, buf);
                                }

                        }

                        strcat(buf1, "\n\rDeity type timers:       {W");

                        for (next = 0; next < DEITY_NUMBER_TYPES; next++)
                        {
                                sprintf(buf, " %d", victim->pcdata->deity_type_timer[next]);
                                strcat(buf1, buf);
                        }

                        strcat(buf1, "{x\n\rDeity personality timers:{W");

                        for (next = 0; next < DEITY_NUMBER_PERSONALITIES; next++)
                        {
                                sprintf(buf, " %d", victim->pcdata->deity_personality_timer[next]);
                                strcat(buf1, buf);
                        }

                        strcat(buf1, "{x\n\r");
                }

                if ((paf = victim->affected))
                {
                        sprintf(buf, "{W[{x{GSpell/s active{x{W]{x\n\r");
                        strcat(buf1, buf);
                }

                for ( paf = victim->affected; paf; paf = paf->next )
                {
                        if ( paf->deleted )
                                continue;

                        sprintf( buf, "{W%-25s{x modifies {G%s{x by {Y%d{x",
                                skill_table[(int) paf->type].name,
                                affect_loc_name( paf->location ),
                                paf->modifier);
                        strcat( buf1, buf );

                        if (paf->bitvector
                                && ( paf->bitvector == AFF_PRONE
                                || paf->bitvector == AFF_DAZED))
                        {
                                if( paf->duration > 1 )
                                {
                                        sprintf( buf, " for {G%d{x min", paf->duration );
                                        strcat( buf1, buf );
                                }
                                else
                                {
                                        sprintf( buf, " for {G%d{x min", paf->duration );
                                        strcat( buf1, buf );
                                }
                        }
                        else
                        {

                                if( paf->duration > 1 )
                                {
                                        sprintf( buf, " for {G%d{x hours",
                                                paf->duration );
                                        strcat( buf1, buf );
                                }
                                else if( paf->duration == 1 )
                                {
                                        sprintf( buf, " for {G%d{x hour",
                                                paf->duration );
                                        strcat( buf1, buf );
                                }
                                else if( paf->duration == 0 )
                                {
                                        sprintf( buf, " for less than an hour");
                                        strcat( buf1, buf );
                                }
                                else
                                {
                                        strcat( buf1, " indefinitely" );
                                }
                        }
                        sprintf( buf, " with bit {R%s{x\n\r",
                                affect_bit_name( paf->bitvector ) );
                        strcat( buf1, buf );
                }

        }
        else
        {
                /* All MOBILE mstat stuff goes here. */
                int temp = rank_sn(victim);
                sprintf( buf, "Vnum: {R%d{x  Rank: %s(%d)\n\r",
                        victim->pIndexData->vnum, rank_table[rank_sn(victim)].who_format, temp);
                strcat( buf1, buf );

                if ( victim->short_descr[0] != '\0'
                &&   victim->long_descr[0]  != '\0' )
                {
                        sprintf( buf, "Short description: {W%s{x\n\rKeywords: {W%s{x\n\rLong description: \n\r  {W%s{x",
                                victim->short_descr,
                                victim->name,
                                victim->long_descr );
                        strcat( buf1, buf );
                }

                strcpy( spec_result , spec_fun_name( victim ) );

                if ( str_cmp(spec_result, "none") )
                {
                        sprintf( buf, "Special function: {Y%s{x\n\r", spec_fun_name( victim ) );
                        strcat( buf1, buf );
                }

                if ( IS_NPC( victim )
	          && victim->pIndexData
	          && victim->pIndexData->pGame
	          && victim->pIndexData->pGame->game_fun != 0 )
                {
	                sprintf( buf, "Game function: {Y%s{x\n\r",
		        game_string( victim->pIndexData->pGame->game_fun ) );
                        strcat( buf1, buf );
                }

                sprintf( buf, "Lvl: {W%d{x  Room: {R%d{x  Align: {W%d{x  Sex: {W%s{x\n\r",
                        victim->level,
                        !victim->in_room           ?        0 : victim->in_room->vnum,
                        victim->alignment,
                        victim->sex == SEX_MALE    ? "male"   :
                        victim->sex == SEX_FEMALE  ? "female" : "neutral");
                strcat( buf1, buf );

                sprintf( buf, "Hp: {G%d{x/{g%d{x  Mana: {C%d{x/{c%d{x  Mv: {Y%d{x/{y%d{x  Wait: {C%d{x\n\r",
                        victim->hit,         victim->max_hit,
                        victim->mana,        victim->max_mana,
                        victim->move,        victim->max_move,
                        victim->wait);
                strcat( buf1, buf );

                sprintf( buf, "Hitroll: {R%d{x  Damroll: {R%d{x   AC: {W%d{x  Saving throw: {W%d{x\n\r",
                        GET_HITROLL( victim ),
                        GET_DAMROLL( victim ),
                        GET_AC( victim ),
                        victim->saving_throw);
                strcat( buf1, buf );

                sprintf( buf, "Crit: {R%d{x  Swiftness: {R%d{x\n\r",
                        victim->crit,
                        victim->swiftness);
                strcat( buf1, buf );

                sprintf( buf, "Position: {G%d{x [{W%s{x]  Wimpy: {W%d{x  Exp modifier: {W%d{x\n\r",
                        victim->position,
                        position_name(victim->position),
                        victim->wimpy,
                        victim->exp_modifier);
                strcat( buf1, buf );

                sprintf( buf, "Class: {W%d{x ({G%s{x)  SubCl: {W%d{x ({G%s{x)\n\rRace: {W%d{x ({G%s{x)  Age: {W%d{x  Form: {W%s{x\n\r",
                        victim->class,
                        full_class_name( victim->class ) ,
                        victim->sub_class,
                        full_sub_class_name( victim->sub_class ),
                        victim->race,
                        race_name(victim->race),
                        get_age( victim ),
                        extra_form_name(victim->form) );
                strcat( buf1, buf );

                sprintf( buf, "Platinum: {W%d{x  Gold: {Y%d{x  Silver: %d  Copper: {y%d{x\n\r",
                        victim->plat,
                        victim->gold,
                        victim->silver,
                        victim->copper);
                strcat( buf1, buf );

                sprintf( buf, "Carry number: {C%d{x/{c%d{x  Carry weight: {C%d{x/{c%d{x  Coin weight: {W%d{x\n\r",
                        victim->carry_number,
                        can_carry_n( victim ),
                        (victim->carry_weight + victim->coin_weight),
                        can_carry_w( victim ),
                        victim->coin_weight );
                strcat( buf1, buf );

                sprintf( buf, "Master: {W%s{x  Leader: {W%s{x  Inside: {W%s{x\n\rFighting: {W%s{x  Rider: {W%s{x\n\r",
                        victim->master      ? victim->master->name   : "(none)",
                        victim->leader      ? victim->leader->name   : "(none)",
                        victim->inside      ? victim->inside->name   : "(none)",
                        victim->fighting ? victim->fighting->name : "(none)",
                        victim->rider ? victim->rider->name : "(none)");
                strcat( buf1, buf );

                if (victim->act)
                {
                        sprintf(buf, "Act flags (num): {W");
                        strcat( buf1, buf );
                        bit_explode(ch, buf, victim->act);
                        strcat(buf1, buf );
                        strcat(buf1, "{x\n\r");
                        strcat(buf1, "Act flags (txt):{R");

                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                        {
                                if (IS_SET(victim->act, next))
                                {
                                        strcat(buf1, " ");
                                        strcat(buf1, act_bit_name(next));
                                }
                        }

                        strcat(buf1, "{x\n\r");

                }

                if (victim->affected_by)
                {
                        sprintf(buf, "Affected by (num): {W");
                        strcat( buf1, buf );
                        bit_explode(ch, buf, victim->affected_by);
                        strcat(buf1, buf );
                        strcat(buf1, "{x\n\r");

                        strcat(buf1, "Affected by (txt):{R");

                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                        {
                                if (IS_AFFECTED(victim, next))
                                {
                                        strcat(buf1, " ");
                                        strcat(buf1, affect_bit_name(next));
                                }
                        }
                        strcat(buf1, "{x\n\r");
                }

                if (victim->body_form)
                {
                        sprintf( buf, "Body form (num): {W");
                        strcat( buf1, buf );
                        bit_explode(ch, buf, victim->body_form);
                        strcat( buf1, buf );
                        strcat(buf1, "{x\n\r");
                }

                if (victim->body_form)
                {
                        strcat(buf1, "Body form (txt):{R");
                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                        {
                                if (IS_SET(victim->body_form, next))
                                {
                                        strcat(buf1, " ");
                                        strcat(buf1, body_form_name(next));

                                }
                        }
                        strcat(buf1, "{x\n\r");
                }



                if ((paf = victim->affected))
                {
                        sprintf(buf, "{W[{x{GSpell/s active{x{W]{x\n\r");
                        strcat(buf1, buf);
                }

                for ( paf = victim->affected; paf; paf = paf->next )
                {
                        if ( paf->deleted )
                                continue;

                        sprintf( buf, "{W%-25s{x modifies {G%s{x by {Y%d{x",
                                skill_table[(int) paf->type].name,
                                affect_loc_name( paf->location ),
                                paf->modifier);
                        strcat( buf1, buf );

                        if( paf->duration > 1 )
                        {
                                sprintf( buf, " for {G%d{x hours",
                                        paf->duration );
                                strcat( buf1, buf );
                        }
                        else if( paf->duration == 1 )
                        {
                                sprintf( buf, " for {G%d{x hour",
                                        paf->duration );
                                strcat( buf1, buf );
                        }
                        else if( paf->duration == 0 )
                        {
                                sprintf( buf, " for less than an hour");
                                strcat( buf1, buf );
                        }
                        else
                        {
                                strcat( buf1, " indefinitely" );
                        }

                        sprintf( buf, " with bit {R%s{x\n\r",
                                affect_bit_name( paf->bitvector ) );
                        strcat( buf1, buf );
                }

                if ( victim->pIndexData->progtypes )
                {
                        strcat(buf1, "{WMobprog:{x\n\r");
                        for ( mprg = victim->pIndexData->mobprogs; mprg != NULL;
                        mprg = mprg->next )
                        {
                                sprintf( buf, ">%s %s\n\r%s\n\r",
                                mprog_type_to_name( mprg->type ),
                                mprg->arglist,
                                mprg->comlist );
                                strcat( buf1, buf );
                        }
                }

                if ( IS_SET(victim->act, ACT_PRACTICE )
                &&  ( victim->pIndexData->skills ) )
                {
                        int sn;
                        sprintf( buf, "----------------------------------\r\nCan {Wteach{x the following:\n\r----------------------------------\r\n" );
                        strcat( buf1, buf );
                        for  ( sn = 0; sn < MAX_SKILL; sn++ )
                        {
                              if ( victim->pIndexData->skills->learned[sn] > 0 )
                              {
                                        sprintf( buf, "%-30s {G%3d{x \n\r",
                                                skill_table[sn].name,
                                                victim->pIndexData->skills->learned[sn]);
                                        strcat( buf1, buf );
                              }
                        }
                }

                /* Pull out the species specific info for this mob - Brutus Sept 2022 */
                if ( victim->mobspec )
                {
                        int sn;
                        int species;
                        species = species_sn(victim);

                        for ( sn = 0; sn < MAX_MOB; sn++ )
                        {
                                if ( !mob_table[sn].name )
                                        break;

                                if ( !str_cmp(victim->mobspec, mob_table[sn].name))
                                {
                                        strcat(buf, "\n\r{WThe Mobs Specification:{x\n\r");
                                        strcat( buf1, buf );

                                        sprintf( buf, "Name: %s Species: %s\n\r",
                                        mob_table[sn].name, mob_table[sn].species);
                                        strcat( buf1, buf );

                                        /* resists */
                                        sprintf( buf, "Resistant to (num): {W");
                                        strcat( buf1, buf );
                                        bit_explode(ch, buf, mob_table[sn].resists);
                                        strcat( buf1, buf );
                                        strcat(buf1, "{x\n\r");
                                        strcat(buf1, "Resistant To (txt):{R");
                                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                                        {
                                                if (IS_SET(mob_table[sn].resists, next))
                                                {
                                                        strcat(buf1, " ");
                                                        strcat(buf1, resist_name(next));

                                                }
                                        }
                                        strcat(buf1, "{x\n\r");

                                        /* Vulnerable */
                                        sprintf( buf, "Vulnerable to (num): {W");
                                        strcat( buf1, buf );
                                        bit_explode(ch, buf, mob_table[sn].vulnerabilities);
                                        strcat( buf1, buf );
                                        strcat(buf1, "{x\n\r");
                                        strcat(buf1, "Vulnerable To (txt):{R");
                                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                                        {
                                                if (IS_SET(mob_table[sn].vulnerabilities, next))
                                                {
                                                        strcat(buf1, " ");
                                                        strcat(buf1, resist_name(next));

                                                }
                                        }
                                        strcat(buf1, "{x\n\r");

                                        /*  Immune */
                                        sprintf( buf, "Immune to (num): {W");
                                        strcat( buf1, buf );
                                        bit_explode(ch, buf, mob_table[sn].immunes);
                                        strcat( buf1, buf );
                                        strcat(buf1, "{x\n\r");
                                        strcat(buf1, "Immune To (txt):{R");
                                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                                        {
                                                if (IS_SET(mob_table[sn].immunes, next))
                                                {
                                                        strcat(buf1, " ");
                                                        strcat(buf1, resist_name(next));

                                                }
                                        }
                                        strcat(buf1, "{x\n\r");

                                        sprintf( buf, "HP Mod: %d Dam Mod: %d Crit Mod: %d Swiftness Mod: %d\n\r",
                                        mob_table[sn].hp_mod, mob_table[sn].dam_mod, mob_table[sn].crit_mod, mob_table[sn].haste_mod);
                                        strcat( buf1, buf );
                                        sprintf( buf, "Height: %d Weight: %d Size: %d\n\r",
                                        mob_table[sn].height, mob_table[sn].weight, mob_table[sn].size);
                                        strcat( buf1, buf );

                                        /* body parts from Species Table */
                                        sprintf( buf, "Body Parts (num): {W");
                                        strcat( buf1, buf );
                                        bit_explode(ch, buf, species_table[species].body_parts);
                                        strcat( buf1, buf );
                                        strcat(buf1, "{x\n\r");
                                        strcat(buf1, "Body Parts (txt):{R");

                                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                                        {
                                                if (IS_SET(species_table[species].body_parts, next))
                                                {
                                                        strcat(buf1, " ");
                                                        strcat(buf1, body_form_name(next));

                                                }
                                        }
                                        strcat(buf1, "{x\n\r");

                                        /* Attack Parts  from Species Table*/
                                        sprintf( buf, "Attack Parts (num): {W");
                                        strcat( buf1, buf );
                                        bit_explode(ch, buf, species_table[species].attack_parts);
                                        strcat( buf1, buf );
                                        strcat(buf1, "{x\n\r");
                                        strcat(buf1, "Attack Parts (txt):{R");
                                        for (next = 1; next > 0 && next <= BIT_MAX; next *= 2)
                                        {
                                                if (IS_SET(species_table[species].attack_parts, next))
                                                {
                                                        strcat(buf1, " ");
                                                        strcat(buf1, body_form_name(next));

                                                }
                                        }
                                        strcat(buf1, "{x\n\r");

                                        sprintf( buf, "Lang: %d\n\r Spec_1: %s\n\r Spec_2: %s\n\r Spec_3: %s\n\r",
                                        mob_table[sn].language, mob_table[sn].spec_fun1, mob_table[sn].spec_fun2, mob_table[sn].spec_boss);
                                        strcat( buf1, buf );
                                }
                        }



                }


        }

        send_to_char( buf1, ch );
        return;
}


void do_mfind( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        MOB_INDEX_DATA *pMobIndex;
        char            buf  [ MAX_STRING_LENGTH   ];
        char            buf1 [ MAX_STRING_LENGTH*2 ];
        char            arg  [ MAX_INPUT_LENGTH    ];
        extern int      top_mob_index;
        int             vnum;
        int             nMatch;
        int             listnum = 1;
        bool            fAll;
        bool            found;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_mfind ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Mfind whom?\n\r", ch );
                return;
        }

        if (strlen (arg) < 3)
        {
                send_to_char ("Argument must be at least three letters long.\n\r", ch);
                return;
        }

        buf1[0] = '\0';
        fAll    = FALSE;
        found   = FALSE;
        nMatch  = 0;

        /*
         * Yeah, so iterating over all vnum's takes 10,000 loops.
         * Get_mob_index is fast, and I don't feel like threading another link.
         * Do you?
         * -- Furey
         */
        for ( vnum = 0; nMatch < top_mob_index; vnum++ )
        {
                if ( ( pMobIndex = get_mob_index( vnum ) ) )
                {
                        nMatch++;
                        if ( fAll || multi_keyword_match( arg, pMobIndex->player_name ) )
                        {
                                found = TRUE;
                                sprintf( buf, "{Y%3d{x. [{W%5d{x] {c%s{x\n\r",
                                        listnum, pMobIndex->vnum, pMobIndex->short_descr );
                                listnum++;
                                if ( !fAll )
                                        strcat( buf1, buf );
                                else
                                        send_to_char( buf, ch );
                        }
                }
        }

        if ( !found )
        {
                send_to_char( "Nothing like that in the domain.\n\r", ch);
                return;
        }

        if ( !fAll )
                send_to_char( buf1, ch );
        return;
}

/* Find teachers based on the skills they teach and beyond what % level -- Owl 9/12/23 */

void do_tfind( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        MOB_INDEX_DATA *pMobIndex;
        char            *target_skill;
        char            buf     [ MAX_STRING_LENGTH   ];
        char            buf1    [ MAX_STRING_LENGTH*2 ];
        char            headbuf [ MAX_STRING_LENGTH   ];
        char            arg1    [ MAX_INPUT_LENGTH    ];
        char            arg2    [ MAX_INPUT_LENGTH    ];
        extern int      top_mob_index;
        int             vnum;
        int             nMatch;
        int             listnum = 1;
        int             sn;
        int             skill_value;
        bool            fAll;
        bool            found;
        bool            first_result;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_tfind ) )
                return;

        target_skill = one_argument( argument, arg1 );
        one_argument( target_skill, arg2 );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Tfind for which skill/spell?\n\r", ch );
                return;
        }

        sn = skill_lookup(arg1);

        if (sn == -1)
        {
                send_to_char( "No such skill/spell.\n\r", ch );
                return;
        }

        skill_value = atoi(arg2);

        if (skill_value == 0)
        {
            skill_value = 1;
        }

        buf1[0]         = '\0';
        fAll            = FALSE;
        found           = FALSE;
        first_result    = FALSE;
        nMatch          = 0;

        for ( vnum = 0; nMatch < top_mob_index; vnum++ )
        {
                if ( ( pMobIndex = get_mob_index( vnum ) ) )
                {
                        nMatch++;

                        if ( fAll
                        ||   ( IS_SET(pMobIndex->act, ACT_PRACTICE ) && ( pMobIndex->skills ) ) )
                        {
                                if ( (pMobIndex->skills->learned[sn] >= skill_value ) )
                                {
                                    found = TRUE;
                                    if (first_result == FALSE)
                                    {
                                        /* print stuff, set it to false */
                                        sprintf (headbuf, "{W================================================================================{x\r\n{WThe following teachers know '{G%s{x'{W at or above {G%d{W%%\r\n{W================================================================================{x\r\n",
                                            skill_table[sn].name,
                                            skill_value
                                        );
                                        send_to_char( headbuf, ch );
                                        first_result = TRUE;
                                    }
                                    sprintf( buf, "{Y%3d{x. [{W%5d{x] ({G%3d{x) {c%s{x {w({W%s{w){x\n\r",
                                            listnum,
                                            pMobIndex->vnum,
                                            pMobIndex->skills->learned[sn],
                                            pMobIndex->short_descr,
                                            pMobIndex->player_name );
                                    listnum++;
                                    if ( !fAll )
                                        strcat( buf1, buf );
                                    else
                                        send_to_char( buf, ch );
                                }
                        }
                }
        }

        if ( !found )
        {
                send_to_char( "No one teaches that specifically. Try the spell or skill group it belongs to.\n\r", ch);
                return;
        }

        if ( !fAll )
                send_to_char( buf1, ch );
        return;
}

/* Find room vnums by searching room title strings.  --Owl 13/3/24 */

void do_rfind(CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        ROOM_INDEX_DATA *pRoomIndex;
        char            buf  [ MAX_STRING_LENGTH   ];
        char            buf1 [ MAX_STRING_LENGTH*2 ];
        char            arg  [ MAX_INPUT_LENGTH    ];
        extern int      top_room;
        int             vnum;
        int             nMatch;
        int             listnum = 1;
        int             listmax = 400; /* Increase at your peril, will probably cause segfault --Owl */
        bool            fAll;
        bool            found;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_rfind ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Rfind which room?\n\r", ch );
                return;
        }

        if (strlen (arg) < 3)
        {
                send_to_char ("Search string must be at least three letters long.\n\r", ch);
                return;
        }

        buf1[0] = '\0';
        fAll    = FALSE;
        found   = FALSE;
        nMatch  = 0;

        for ( vnum = 0; nMatch < top_room && listnum <= listmax; vnum++ )
        {
            if ( ( pRoomIndex = get_room_index( vnum ) ) )
            {
                nMatch++;
                if ( fAll || multi_keyword_match( arg, pRoomIndex->name ) )
                {
                    found = TRUE;
                    sprintf( buf, "{Y%3d{x. [{W%5d{x] {c%s{x\n\r",
                            listnum, pRoomIndex->vnum, pRoomIndex->name );

                    if (listnum <= listmax)
                    {
                        if (!fAll)
                        {
                            if (strlen(buf1) + strlen(buf) < sizeof(buf1) - 1)
                            {
                                strcat(buf1, buf);
                            }
                            else
                            {
                                // Buffer full
                                send_to_char("Too many results.  Try a more targetted search.\n\r", ch);
                                return;
                            }
                        }
                        else
                        {
                            send_to_char(buf, ch);
                        }
                        listnum++;
                    }

                    if (listnum > listmax) // Check after increment if we should break
                    {
                        break; // Exit the loop if we've reached the limit
                    }
                }
            }
        }

        if ( !found )
        {
                send_to_char( "Nowhere like that in the domain.\n\r", ch);
                return;
        }

        if ( !fAll )
                send_to_char( buf1, ch );
        return;
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        OBJ_INDEX_DATA *pObjIndex;
        char            buf  [ MAX_STRING_LENGTH   ];
        char            buf1 [ MAX_STRING_LENGTH*2 ];
        char            arg  [ MAX_INPUT_LENGTH    ];
        extern int      top_obj_index;
        int             vnum;
        int             listnum = 1;
        int             nMatch;
        bool            fAll;
        bool            found;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_ofind ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "ofind what?\n\r", ch );
                return;
        }

        if (strlen (arg) < 3)
        {
                send_to_char ("Argument must be at least three letters long.\n\r", ch);
                return;
        }

        buf1[0] = '\0';
        fAll    = FALSE;
        found   = FALSE;
        nMatch  = 0;

        for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        {
                if ( ( pObjIndex = get_obj_index( vnum ) ) )
                {
                        nMatch++;
                        if ( fAll || multi_keyword_match( arg, pObjIndex->name ) )
                        {
                                found = TRUE;
                                sprintf( buf, "{Y%3d{x. [{W%5d{x] {g%s{w\n\r",
                                        listnum, pObjIndex->vnum, pObjIndex->short_descr );
                                listnum++;
                                if ( !fAll )
                                        strcat( buf1, buf );
                                else
                                        send_to_char( buf, ch );
                        }
                }
        }

        if ( !found )
        {
                send_to_char( "Nothing like that in the domain.\n\r", ch);
                return;
        }

        if ( !fAll )
                send_to_char( buf1, ch );
        return;
}

void do_oscore( CHAR_DATA *ch, char *argument )
{

        int             uncommon, rare, epic, legendary;
        char tmp [MAX_STRING_LENGTH];
        OBJ_DATA  *obj;
        OBJ_DATA  *in_obj;
        CHAR_DATA *rch;
        char       buf  [ MAX_STRING_LENGTH ];
        char       arg  [ MAX_INPUT_LENGTH  ];
        int        obj_counter = 1;

        uncommon = 0;
        rare = 0;
        epic = 0;
        legendary = 0;
        buf[0] = '\0';

        rch = get_char( ch );

        if ( !authorized( rch, gsn_owhere ) )
                return;

        one_argument( argument, arg );

        if( arg[0] == '\0' )
        {
                for ( obj = object_list; obj; obj = obj->next )
                {
                        if ( !can_see_obj( ch, obj ) )
                                continue;

                        for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
                                ;

                        if (calc_item_score(obj) > ITEM_SCORE_LEGENDARY)
                        {
                                legendary++;
                                if ( in_obj->carried_by )
                                {
                                        if ( !can_see( ch, in_obj->carried_by ) )
                                                continue;

                                        sprintf( tmp, "[LEGENDARY] {g%s{x carried by {c%s{x at [%5d]\n\r",
                                                obj->short_descr,
                                                PERS( in_obj->carried_by, ch ),
                                                in_obj->carried_by->in_room->vnum );
                                }
                                else
                                {
                                        sprintf( tmp, "[LEGENDARY] {g%s{x in {c%s{x at [%5d]\n\r",
                                                obj->short_descr,
                                                ( !in_obj->in_room ) ?
                                                "somewhere" : in_obj->in_room->name,
                                                ( !in_obj->in_room ) ?
                                                0 : in_obj->in_room->vnum );
                                }
                                strcat( buf, tmp);
                        }
                        else if (calc_item_score(obj) > ITEM_SCORE_EPIC)
                                epic++;
                        else if (calc_item_score(obj) > ITEM_SCORE_RARE)
                                rare++;
                        else if (calc_item_score(obj) > ITEM_SCORE_UNCOMMON)
                                uncommon++;

                        obj_counter++;

                }
                sprintf( tmp, "Total[%5d] Uncommon[%5d] Rare[%5d] Epic[%5d] Legendary[%5d]\n\r", obj_counter, uncommon, rare, epic, legendary );
                                                strcat( buf, tmp);
                /* buf[0] = UPPER( buf[0] ); */
                send_to_char (buf, ch);
                return;
       }
 /*       else
        {
                if (strlen (arg) < 3)
                {
                        send_to_char ("Argument must be at least three letters long.\n\r", ch);
                        return;
                }

                for ( obj = object_list; obj; obj = obj->next )
                {
                        if ( !can_see_obj( ch, obj ) || !multi_keyword_match( arg, obj->name ) )
                                continue;


                        for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
                                ;

                        if ( in_obj->carried_by )
                        {
                                if ( !can_see( ch, in_obj->carried_by ) )
                                        continue;

                                sprintf( buf, "[%-3d] {g%s{x carried by {c%s{x at [%5d]\n\r",
                                        obj_counter,
                                        obj->short_descr,
                                        PERS( in_obj->carried_by, ch ),
                                        in_obj->carried_by->in_room->vnum );
                        }
                        else
                        {
                                sprintf( buf, "[%-3d] {g%s{x in {c%s{x at [%5d]\n\r",
                                        obj_counter,
                                        obj->short_descr,
                                        ( !in_obj->in_room ) ?
                                        "somewhere" : in_obj->in_room->name,
                                        ( !in_obj->in_room ) ?
                                        0 : in_obj->in_room->vnum );
                        }

                        obj_counter++;
                        buf[0] = UPPER( buf[0] );
                        send_to_char (buf, ch);
                }
        }
        return;
*/
}


void do_osfind( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        OBJSET_INDEX_DATA *pObjSetIndex;
        char            buf  [ MAX_STRING_LENGTH   ];
        char            buf1 [ MAX_STRING_LENGTH*2 ];
        char            arg  [ MAX_INPUT_LENGTH    ];
        extern int      top_objset_index;
        int             vnum;
        int             nMatch;
        bool            fAll;
        bool            found;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_osfind ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Osfind what?\n\r", ch );
                return;
        }

        if (strlen (arg) < 3)
        {
                send_to_char ("Argument must be at least three letters long.\n\r", ch);
                return;
        }

        buf1[0] = '\0';
        fAll    = FALSE;
        found   = FALSE;
        nMatch  = 0;

        for ( vnum = 0; nMatch < top_objset_index; vnum++ )
        {
                if ( ( pObjSetIndex = get_objset_index( vnum ) ) )
                {
                        nMatch++;
                        if ( fAll || multi_keyword_match( arg, pObjSetIndex->name ) )
                        {
                                found = TRUE;
                                sprintf( buf, "[%5d] %s\n\r",
                                        pObjSetIndex->vnum, pObjSetIndex->name );
                                if ( !fAll )
                                        strcat( buf1, buf );
                                else
                                        send_to_char( buf, ch );
                        }
                }
        }

        if ( !found )
        {
                send_to_char( "Nothing like that in the domain.\n\r", ch);
                return;
        }

        if ( !fAll )
                send_to_char( buf1, ch );
        return;
}

void do_mwhere( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       buf  [ MAX_STRING_LENGTH   ];
        char       arg  [ MAX_INPUT_LENGTH    ];
        bool       found;
        int        count = 0;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_mwhere ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Mwhere whom?\n\r", ch );
                return;
        }

        if (strlen (arg) < 3)
        {
                send_to_char ("Argument must be at least three letters long.\n\r", ch);
                return;
        }

        found = FALSE;
        for ( victim = char_list; victim; victim = victim->next )
        {
                if ( IS_NPC( victim )
                    && victim->in_room
                    && multi_keyword_match( arg, victim->name ) )
                {
                        found = TRUE;
                        sprintf( buf, "{Y%3d{x. {c%s{x [{W%d{x] at {m%s{x [{W%d{x] \n\r",
                                ++count,
                                victim->short_descr,
                                victim->pIndexData->vnum,
                                victim->in_room->name,
                                victim->in_room->vnum );
                        send_to_char (buf, ch);
                }
        }

        if ( !found )
        {
                act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
                return;
        }

        return;
}


void do_reboo( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_reboot ) )
                return;

        send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
        return;
}


void do_reboot( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        char buf [ MAX_STRING_LENGTH ];
        extern bool merc_down;

        rch = get_char( ch );

        if (!authorized(rch, gsn_reboot))
                return;

        sprintf( buf, "Reboot by %s.", ch->name );
        do_echo( ch, buf );

        end_of_game( );

        merc_down = TRUE;
        return;
}


void do_shutdow( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_shutdown ) )
                return;

        send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
        return;
}


void do_shutdown( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        char buf [ MAX_STRING_LENGTH ];
        extern bool merc_down;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_shutdown ) )
                return;

        sprintf( buf, "Shutdown by %s.", ch->name );
        append_file( ch, SHUTDOWN_FILE, buf );
        strcat( buf, "\n\r" );
        do_echo( ch, buf );

        end_of_game( );

        merc_down = TRUE;
        return;
}


void do_snoop( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA       *rch;
        CHAR_DATA       *victim;
        DESCRIPTOR_DATA *d;
        char             arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_snoop ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Snoop whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( !victim->desc )
        {
                send_to_char( "No descriptor to snoop.\n\r", ch );
                return;
        }

        if ( victim == ch )
        {
                send_to_char( "Cancelling all snoops.\n\r", ch );
                for ( d = descriptor_list; d; d = d->next )
                {
                        if ( d->snoop_by == ch->desc )
                                d->snoop_by = NULL;
                }
                return;
        }

        if ( victim->desc->snoop_by )
        {
                send_to_char( "Busy already.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( ch->desc )
        {
                for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
                {
                        if ( d->character == victim || d->original == victim )
                        {
                                send_to_char( "No snoop loops.\n\r", ch );
                                return;
                        }
                }
        }

        victim->desc->snoop_by = ch->desc;
        send_to_char( "Ok.\n\r", ch );
        return;
}


void do_switch( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_switch ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Switch into whom?\n\r", ch );
                return;
        }

        if ( !ch->desc )
                return;

        if ( ch->desc->original )
        {
                send_to_char( "You are already switched.\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( victim == ch )
        {
                send_to_char( "Ok.\n\r", ch );
                return;
        }

        if ( !IS_NPC( victim ) )
        {
                send_to_char( "You cannot switch into a player.\n\r", ch );
                return;
        }

        if ( victim->desc )
        {
                send_to_char( "Character in use.\n\r", ch );
                return;
        }

        ch->pcdata->switched  = TRUE;
        ch->desc->character   = victim;
        ch->desc->original    = ch;
        victim->desc          = ch->desc;
        ch->desc              = NULL;
        send_to_char( "Ok.\n\r", victim );
        return;
}


void do_return( CHAR_DATA *ch, char *argument )
{
        if ( !ch->desc )
                return;

        if ( !ch->desc->original )
        {
                send_to_char( "You aren't switched.\n\r", ch );
                return;
        }

        send_to_char( "You return to your original body.\n\r", ch );
        ch->desc->original->pcdata->switched = FALSE;
        ch->desc->character                  = ch->desc->original;
        ch->desc->original                   = NULL;
        ch->desc->character->desc            = ch->desc;
        ch->desc                             = NULL;
        return;
}

void do_mnstat( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        MOB_INDEX_DATA *pMobIndex;
        char            buf  [ MAX_STRING_LENGTH ];
        char            buf1 [ MAX_STRING_LENGTH ];
        char            arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        buf[0] = '\0';
        buf1[0] = '\0';

        if ( !authorized( rch, gsn_mload ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' || !is_number( arg ) )
        {
                send_to_char( "Syntax: mnstat <<vnum>.\n\r", ch );
                return;
        }

        if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
        {
                send_to_char( "No mob has that vnum.\n\r", ch );
                return;
        }

        sprintf( buf, "Vnum: {R%d{x\n\r", pMobIndex->vnum);
        strcat( buf1, buf );

                if ( pMobIndex->short_descr[0] != '\0'
                &&   pMobIndex->long_descr[0]  != '\0' )
                {
                        sprintf( buf, "Short description: {W%s{x\n\rMobspec: {W%s{x\n\rLong description: \n\r  {W%s{x",
                                pMobIndex->short_descr,
                                pMobIndex->mobspec,
                                pMobIndex->long_descr );
                        strcat( buf1, buf );
                }
        send_to_char( buf1, ch );
        return;
}

void do_mload( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA      *rch;
        CHAR_DATA      *victim;
        MOB_INDEX_DATA *pMobIndex;
        char            arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_mload ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' || !is_number( arg ) )
        {
                send_to_char( "Syntax: mload <<vnum>.\n\r", ch );
                return;
        }

        if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
        {
                send_to_char( "No mob has that vnum.\n\r", ch );
                return;
        }

        victim = create_mobile( pMobIndex );
        char_to_room( victim, ch->in_room );
        send_to_char( "Ok.\n\r", ch );
        act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
        return;
}


void do_oload( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA       *obj;
        CHAR_DATA      *rch;
        OBJ_INDEX_DATA *pObjIndex;
        char            arg1 [ MAX_INPUT_LENGTH ];
        char            arg2 [ MAX_INPUT_LENGTH ];
        char            arg3 [ MAX_INPUT_LENGTH ];
        int             level;
        int             copies;
        int             cc_def;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_oload ) )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

        if ( arg1[0] == '\0' || !is_number( arg1 ) )
        {
                send_to_char( "Syntax: oload <<vnum> <<level> <<copies>.\n\r", ch );
                return;
        }

        if ( arg2[0] == '\0' )
        {
                level   = get_trust( ch );
                copies  = 1;
        }

        if ( arg2[0] != '\0' && arg3[0] == '\0' )
        {
                if ( !is_number( arg2 ) )
                {
                        send_to_char( "Syntax: oload <<vnum> <<level> <<copies>.\n\r", ch );
                        return;
                }
                level = atoi( arg2 );
                copies  = 1;
        }

        if ( arg2[0] != '\0' && arg3[0] != '\0' )
        {
                if ( !is_number( arg2 ) || !is_number( arg3 ) )
                {
                        send_to_char( "Syntax: oload <<vnum> <<level> <<copies>.\n\r", ch );
                        return;
                }
                level  = atoi( arg2 );
                copies = atoi( arg3 );

                if ( copies > MAX_OLOAD_COPIES )
                {
                        char buf [ MAX_STRING_LENGTH ];
                        sprintf( buf, "You cannot oload more than %d of the same item at a time.\n\r", MAX_OLOAD_COPIES );
                        send_to_char( buf, ch );
                        return;
                }
        }

        if (copies < 1)
        {
                send_to_char( "Number of copies must be >= 1 if argument is supplied.\n\r", ch );
                return;

        }

        if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
        {
                send_to_char( "No object has that vnum.\n\r", ch );
                return;
        }

        /* testing random_qnd function, Owl 1/1/23
        rand_value_test = random_qnd ( -3, "elite", APPLY_STR );
        sprintf( buf, "Rand value returned for rand item = %d.\n\r", rand_value_test );
        send_to_char( buf, ch );
        rand_value_test = random_qnd ( -10, "boss", APPLY_AC );
        sprintf( buf, "Rand value returned for rand item = %d.\n\r", rand_value_test );
        send_to_char( buf, ch );
        */
        for ( cc_def = 1; cc_def <= copies; cc_def++ )
        {
                /*
                        At present, this will create an object that is not randomised (flag set to FALSE)
                        Changing to TRUE will randomise the object created.  May want to add options
                        to turn randomisation on/off and to be able to create EPIC, LEGENDARY etc items
                        -- Owl 2/1/23
                */
                obj = create_object( pObjIndex, level,"common", CREATED_NO_RANDOMISER );

                /* Check if object creation failed due to spawn cap */
                if (obj == NULL || obj->deleted == TRUE)
                {
                    char buf[MAX_STRING_LENGTH];
                    sprintf(buf,
                        "{WWarning: object vnum %d has hit its max_instances limit (%d). No object loaded.{x\n\r",
                        pObjIndex->vnum,
                        pObjIndex->max_instances);
                    send_to_char(buf, ch);

                    if (obj != NULL)
                    {
                        obj_to_char(obj, ch);
                    }

                    return;
                }

                if ( IS_SET(obj->wear_flags, ITEM_TAKE) )
                {
                        if ( (ch->carry_number + copies) > can_carry_n( ch ))
                        {
                                char buf [ MAX_STRING_LENGTH ];
                                if (copies == 1)
                                {
                                        sprintf( buf, "You can carry up to {W%d{x items.  oloading {W1{x more item will exceed that limit.\n\r",
                                                can_carry_n( ch ));
                                        send_to_char( buf, ch );
                                }
                                else {
                                        sprintf( buf, "You can carry up to {W%d{x items.  oloading {W%d{x more items will exceed that limit.\n\r",
                                                can_carry_n( ch ),
                                                copies );
                                        send_to_char( buf, ch );
                                }
                                return;
                        }
                        obj_to_char( obj, ch );
                }
                else
                {
                        obj_to_room( obj, ch->in_room );
                        if (cc_def == 1)
                        {
                                act( "$n creates $p!", ch, obj, NULL, TO_ROOM );
                        }
                        if (cc_def == 2)
                        {
                                act( "$n creates more of '$p'!", ch, obj, NULL, TO_ROOM );
                        }
                }
        }
        send_to_char( "Ok.\n\r", ch );
        return;
}


void do_purge( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA  *obj;
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_purge ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                /* 'purge' */
                OBJ_DATA  *obj_next;
                CHAR_DATA *vnext;

                for ( victim = ch->in_room->people; victim; victim = vnext )
                {
                        vnext = victim->next_in_room;
                        if ( victim->deleted )
                                continue;

                        if ( IS_NPC( victim ) && victim != ch )
                                extract_char( victim, TRUE );
                }

                for ( obj = ch->in_room->contents; obj; obj = obj_next )
                {
                        obj_next = obj->next_content;
                        if ( obj->deleted )
                                continue;
                        extract_obj( obj );
                }

                send_to_char( "Ok.\n\r", ch );
                act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( !IS_NPC( victim ) )
        {
                send_to_char( "Not on PCs.\n\r", ch );
                return;
        }

        act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
        extract_char( victim, TRUE );
        return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        int        level;
        int        iLevel;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_advance ) )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
        {
                send_to_char( "Syntax: advance <<char> <<level>.\n\r", ch );
                return;
        }

        if ( !( victim = get_char_room( ch, arg1 ) ) )
        {
                send_to_char( "That player is not here.\n\r", ch);
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        level = atoi( arg2 );

        if ( level < 1 || level > MAX_LEVEL )
        {
                char buf [ MAX_STRING_LENGTH ];

                sprintf( buf, "Advance within range 1 to %d.\n\r", MAX_LEVEL );
                send_to_char( buf, ch );
                return;
        }

        if ( level > get_trust( ch ) )
        {
                send_to_char( "Limited to your trust level.\n\r", ch );
                return;
        }

        if ( level <= victim->level )
        {
                int sn;

                send_to_char( "Lowering a player's level!\n\r", ch );
                send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r",    victim );

                victim->level    = 1;
                victim->exp      = 1000;
                victim->max_hit  = 10;
                victim->max_mana = 100;
                victim->max_move = 100;
                victim->pcdata->str_prac = 0;
                victim->pcdata->int_prac = 0;
                victim->hit      = victim->max_hit;
                victim->mana     = victim->max_mana;
                victim->move     = victim->max_move;

                ch->pcdata->learned[gsn_mage_base + ch->class] = 29;

                for ( sn = 0; sn < MAX_SKILL; sn++ )
                        victim->pcdata->learned[sn] = 0;

                advance_level( victim );
        }
        else
        {
                send_to_char( "Raising a player's level!\n\r", ch );
                send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
        }

        for ( iLevel = victim->level ; iLevel < level; iLevel++ )
        {
                send_to_char( "You raise a level!!  ", victim );
                victim->level += 1;
                advance_level( victim );
        }

        victim->exp = level_table[victim->level-1].exp_total;
        victim->trust = 0;
        return;
}


void do_addfame( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        char       buf  [ MAX_STRING_LENGTH ];
        int        level;

        if ( IS_NPC( ch ) && !IS_SET(ch->act, ACT_NOCHARM )  )
                return;

        if  (!IS_NPC(ch) )
        {
                if (!authorized( ch, gsn_addfame ) )
                        return;
        }

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( !( victim = get_char_room( ch, arg1 ) ) )
        {
                send_to_char( "That player is not here.\n\r", ch);
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        level = atoi( arg2 );

        if ( level < MIN_ADDFAME || level > MAX_ADDFAME  || level == 0)
        {
                sprintf( buf, "The amount of fame added must be between %d and %d, and not 0.\n\r",
                        MIN_ADDFAME,
                        MAX_ADDFAME );
                send_to_char( buf, ch );
                return;
        }

        if (level < 0 )
        {
                send_to_char( "Lowering a players fame!\n\r", ch );
                victim->pcdata->fame += level;
                sprintf( buf, "You lose %d fame for your cowardly acts!\n\r", -level );
                send_to_char( buf, victim );
                check_fame_table(victim);
        }
        else
        {
                send_to_char( "Raising a player's fame!\n\r", ch );
                victim->pcdata->fame += level;
                sprintf( buf, "You gain %d fame for your heroic deeds!\n\r", level );
                send_to_char( buf, victim );
                act( "$n rewards $N for $s heroic actions.",ch,NULL,victim,TO_ROOM);
                check_fame_table(victim);
        }
        return;
}


void do_addqp( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        int        points;
        char       buf [ MAX_STRING_LENGTH ];

        if ( IS_NPC( ch ) && !IS_SET(ch->act, ACT_NOCHARM )  )
                return;

        if  (!IS_NPC(ch) && !IS_IMMORTAL(ch)) /* So imms can use this --Owl 2/3/22 */
        {
                if (!authorized( ch, gsn_addqp ) )
                        return;
        }

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
        {
                send_to_char( "Usage: addqp <<character> <<quest points>\n\r", ch );
                return;
        }
                if ( !( victim = get_char_room( ch, arg1 ) ) )
        {
                send_to_char( "That player is not here.\n\r", ch);
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        points = atoi( arg2 );

        if ( points < 1 || points > MAX_ADDQP )
        {
                sprintf( buf, "The number of quest points added must be between 1 and %d.\n\r", MAX_ADDQP );
                send_to_char( buf, ch );
                return;
        }

        send_to_char( "Increasing player's quest points!\n\r", ch );
        victim->pcdata->questpoints += points;
        victim->pcdata->totalqp += points;
        sprintf( buf, "You gain %d quest point%s!\n\r", points, (points > 1) ? "s" : "");
        send_to_char( buf, victim );
        check_fame_table(victim);
}


void do_trust( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        int        level;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_trust ) )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
        {
                send_to_char( "Syntax: trust <<char> <<level>.\n\r", ch );
                return;
        }

        if ( !( victim = get_char_room( ch, arg1 ) ) )
        {
                send_to_char( "That player is not here.\n\r", ch );
                return;
        }

        level = atoi( arg2 );

        if ( level < 0 || level > MAX_LEVEL )
        {
                char buf [ MAX_STRING_LENGTH ];

                sprintf( buf, "Trust within range 0 to %d.\n\r", MAX_LEVEL );
                send_to_char( buf, ch );
                return;
        }

        if ( level > get_trust( ch ) )
        {
                send_to_char( "Limited to your trust.\n\r", ch );
                return;
        }

        if (get_trust(victim) > get_trust(ch))
        {
                send_to_char( "Get bent.\n\r", ch );
                return;
        }

        victim->trust = level;
        return;
}


void do_restore( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_restore ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Restore whom?\n\r", ch );
                return;
        }

        /* Restore All feature coded by Katrina */
        if ( !str_cmp( arg, "all" ) )
        {
                for ( victim = char_list; victim; victim = victim->next )
                {
                        if ( victim->deleted )
                                continue;

                        if ( IS_NPC(victim) )
                                continue;

                        victim->aggro_dam = 0;

                        if( victim->hit < victim->max_hit )
                                victim->hit = victim->max_hit;

                        if( victim->mana < victim->max_mana )
                                victim->mana = victim->max_mana;

                        victim->move = victim->max_move;
                        victim->pcdata->condition[COND_FULL] = ( MAX_FOOD - 10);
                        victim->pcdata->condition[COND_THIRST] = MAX_DRINK;
                        victim->rage = victim->max_rage;
                        update_pos( victim );
                        act( "$n has restored you.", ch, NULL, victim, TO_VICT );
                }

                send_to_char( "Aww... how sweet.\n\r", ch );
        }

        /*  Tournament restore  */
        else if (!str_cmp(arg, "tournament"))
        {
                if (tournament_status == TOURNAMENT_STATUS_NONE)
                {
                        send_to_char("There is no tournament set.\n\r", ch);
                        return;
                }

                for (victim = char_list; victim; victim = victim->next)
                {
                        if (victim->deleted)
                                continue;

                        if (IS_NPC(victim))
                                continue;

                        if (!is_entered_in_tournament(victim))
                                continue;

                        victim->aggro_dam = 0;

                        if( victim->hit < victim->max_hit )
                                victim->hit = victim->max_hit;

                        if( victim->mana < victim->max_mana )
                                victim->mana = victim->max_mana;

                        victim->move = victim->max_move;
                        victim->pcdata->condition[COND_FULL] = MAX_FOOD;
                        victim->pcdata->condition[COND_THIRST] = MAX_DRINK;
                        victim->rage = victim->max_rage;
                        update_pos( victim );
                        act( "$n has restored you.", ch, NULL, victim, TO_VICT );
                }

                send_to_char( "Restored all tournament entrants.\n\r", ch );
        }
        else
        {
                if ( !( victim = get_char_world( ch, arg ) ) )
                {
                        send_to_char( "They aren't here.\n\r", ch );
                        return;
                }

                if( victim->hit < victim->max_hit )
                        victim->hit = victim->max_hit;

                if( victim->mana < victim->max_mana )
                        victim->mana = victim->max_mana;

                victim->aggro_dam = 0;
                victim->move = victim->max_move;
                victim->rage = victim->max_rage;

                update_pos( victim );
                act( "$n has restored you.", ch, NULL, victim, TO_VICT );
                send_to_char( "Ok.\n\r", ch );
        }

        return;
}


void do_freeze( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_freeze ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Freeze whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( IS_SET( victim->act, PLR_FREEZE ) )
        {
                REMOVE_BIT( victim->act, PLR_FREEZE );
                send_to_char( "FREEZE removed.\n\r",     ch     );
                send_to_char( "You can play again.\n\r", victim );
        }
        else
        {
                SET_BIT(    victim->act, PLR_FREEZE );
                send_to_char( "FREEZE set.\n\r",            ch     );
                send_to_char( "You can't do anything.\n\r", victim );
        }

        save_char_obj( victim );
        return;
}


void do_promote( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        char       buf [ MAX_STRING_LENGTH ];
        int        level;
        int        iLevel;

        rch = get_char( ch );

        if ( IS_NPC( rch ))
                return;

        if ( !IS_SET( rch->act, PLR_LEADER ) && rch->level <= LEVEL_HERO )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
        {
                send_to_char( "Syntax: promote <<char> <<level>.\n\r", ch );
                return;
        }

        if ( !( victim = get_char_room( ch, arg1 ) ) )
        {
                send_to_char( "That player is not here.\n\r", ch);
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( ch->clan != victim->clan && ch->level <= LEVEL_HERO)
        {
                send_to_char( "You cannot promote people from other clans.\n\r",ch);
                return;
        }

        level = atoi( arg2 );

        if ( level < 0 || level > MAX_CLAN_LEVEL )
        {
                char buf [ MAX_STRING_LENGTH ];

                sprintf( buf, "Advance within range 0 to %d.\n\r", MAX_CLAN_LEVEL );
                send_to_char( buf, ch );
                return;
        }

        if ( level <= victim->clan_level )
        {
                send_to_char( "Lowering a player's clan level!\n\r", ch );
                send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r",    victim );
                victim->clan_level    = 0;
                victim->clan_level    = level;

                sprintf( buf, "You are DEMOTED to %s\n\r",
                        victim->clan_level ?
                        clan_title [victim->clan] [victim->clan_level]
                        : "level zero");
                send_to_char( buf, victim );
        }
        else
        {
                send_to_char( "Raising a player's clan level!\n\r", ch );
                send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
                victim->clan_level    = 0;
                victim->clan_level    = level;
                sprintf( buf, "You are PROMOTED to %s\n\r",
                        clan_title [victim->clan] [victim->clan_level] );
                send_to_char( buf, victim );
        }

        for ( iLevel = victim->clan_level ; iLevel < level; iLevel++ )
        {
                send_to_char( "You are promoted a level!  ", victim );
                victim->clan_level += 1;
        }
}



void do_leader( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_leader ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Make whom a Clan Leader?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) > get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( IS_SET( victim->act, PLR_LEADER ) )
        {
                REMOVE_BIT( victim->act, PLR_LEADER );
                send_to_char( "Clan Leader flag removed.\n\r",     ch     );
                send_to_char( "You are no longer a Clan Leader!!\n\r", victim );
        }
        else
        {
                SET_BIT(    victim->act, PLR_LEADER );
                send_to_char( "Clan Leader flag set.\n\r",            ch     );
                send_to_char( "You are now a CLAN LEADER!!\n\r", victim );
                send_to_char("{YPlease read 'HELP CLAN COMMANDS' and 'HELP CLAN ITEMS'.{x\n\r", victim);
        }

        save_char_obj( victim );
}


/*
 *  Toggle RONIN status bit.
 *  Ronin now independent pkillers that receive pk points.
 *  Gezhp & Shade 2000
 */
void do_ronin (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        rch = get_char(ch);

        if (!authorized(rch, gsn_ronin))
                return;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Make whom Ronin?\n\r", ch);
                return;
        }

        if (!(victim = get_char_world(ch, arg)))
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("Not on NPCs.\n\r", ch);
                return;
        }

        if (get_trust(victim) > get_trust(ch))
        {
                send_to_char("You failed.\n\r", ch);
                return;
        }

        if (IS_SET(victim->status, PLR_RONIN))
        {
                REMOVE_BIT(victim->status, PLR_RONIN);
                send_to_char("Ronin flag removed.\n\r", ch);
                send_to_char("You are no longer a ronin.\n\r", victim );
                sprintf(buf, "%s is no longer a RONIN.\n\r", victim->name);
                do_info(victim, buf);
                remove_from_wanted_table(victim->name);
        }
        else
        {
                if (victim->clan)
                {
                        send_to_char("Only unclanned players may become ronin.\n\r", ch);
                        return;
                }

                if (victim->level < 15)
                {
                        send_to_char("Players below level 15 may not become ronin.\n\r", ch);
                        return;
                }

                SET_BIT(victim->status, PLR_RONIN);
                send_to_char("Ronin flag set.\n\r", ch);
                send_to_char("You are now a RONIN!\n\r", victim);
                sprintf(buf, "%s is now a RONIN!\n\r", victim->name);
                do_info(victim, buf);
        }

        save_char_obj(victim);
        return;
}


void do_log( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_log ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Log whom?\n\r", ch );
                return;
        }

        if ( !str_cmp( arg, "all" ) )
        {
                if ( fLogAll )
                {
                        fLogAll = FALSE;
                        send_to_char( "Log ALL off.\n\r", ch );
                }
                else
                {
                        fLogAll = TRUE;
                        send_to_char( "Log ALL on.\n\r",  ch );
                }
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        /*
         * No level check, gods can log anyone.
         */
        if ( IS_SET( victim->act, PLR_LOG ) )
        {
                REMOVE_BIT( victim->act, PLR_LOG );
                send_to_char( "LOG removed.\n\r", ch );
        }
        else
        {
                SET_BIT(    victim->act, PLR_LOG );
                send_to_char( "LOG set.\n\r",     ch );
        }

        return;
}


void do_noemote( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_noemote ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Noemote whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
        {
                REMOVE_BIT( victim->act, PLR_NO_EMOTE );
                send_to_char( "NO_EMOTE removed.\n\r",    ch     );
                send_to_char( "You can emote again.\n\r", victim );
        }
        else
        {
                SET_BIT(    victim->act, PLR_NO_EMOTE );
                send_to_char( "You can't emote!\n\r",    victim );
                send_to_char( "NO_EMOTE set.\n\r",       ch     );
        }

        return;
}


void do_notell( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_notell ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Notell whom?", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( IS_SET( victim->act, PLR_NO_TELL ) )
        {
                REMOVE_BIT( victim->act, PLR_NO_TELL );
                send_to_char( "NO_TELL removed.\n\r",    ch );
                send_to_char( "You can tell again.\n\r", victim );
        }
        else
        {
                SET_BIT(    victim->act, PLR_NO_TELL );
                send_to_char( "NO_TELL set.\n\r",        ch     );
                send_to_char( "You can't tell!\n\r",     victim );
        }

        return;
}


void do_silence( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_silence ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Silence whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if ( IS_SET( victim->act, PLR_SILENCE ) )
        {
                REMOVE_BIT( victim->act, PLR_SILENCE );
                send_to_char( "You can use channels again.\n\r", victim );
                send_to_char( "SILENCE removed.\n\r",            ch     );
        }
        else
        {
                SET_BIT(    victim->act, PLR_SILENCE );
                send_to_char( "You can't use channels!\n\r",     victim );
                send_to_char( "SILENCE set.\n\r",                ch     );
        }

        return;
}


void do_peace( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_peace )
        && !( ch->spec_fun == spec_lookup("spec_gold_grung") ) )
                return;

        /* Yes, we are reusing rch.  -Kahn */
        for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
        {
                if ( rch->fighting )
                        stop_fighting( rch, TRUE );
        }

        send_to_char( "All hostilities cease.\n\r", ch );
        return;
}


void do_ban( CHAR_DATA *ch, char *argument )
{
        BAN_DATA  *pban;
        CHAR_DATA *rch;
        char       buf [ MAX_STRING_LENGTH ];
        char       arg [ MAX_INPUT_LENGTH  ];

        if ( IS_NPC( ch ) )
                return;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_ban ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                strcpy( buf, "Banned sites:\n\r" );
                for ( pban = ban_list; pban; pban = pban->next )
                {
                        strcat( buf, pban->name );
                        strcat( buf, "\n\r" );
                }
                send_to_char( buf, ch );
                return;
        }

        for ( pban = ban_list; pban; pban = pban->next )
        {
                if ( !str_cmp( arg, pban->name ) )
                {
                        send_to_char( "That site is already banned!\n\r", ch );
                        return;
                }
        }

        if ( !ban_free )
        {
                pban            = alloc_perm( sizeof( *pban ) );
        }
        else
        {
                pban            = ban_free;
                ban_free        = ban_free->next;
        }

        pban->name      = str_dup( arg );
        pban->next      = ban_list;
        ban_list        = pban;
        send_to_char( "Ok.\n\r", ch );
        return;
}


void do_allow( CHAR_DATA *ch, char *argument )
{
        BAN_DATA  *prev;
        BAN_DATA  *curr;
        CHAR_DATA *rch;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_allow ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Remove which site from the ban list?\n\r", ch );
                return;
        }

        prev = NULL;
        for ( curr = ban_list; curr; prev = curr, curr = curr->next )
        {
                if ( !str_cmp( arg, curr->name ) )
                {
                        if ( !prev )
                                ban_list   = ban_list->next;
                        else
                                prev->next = curr->next;

                        free_string( curr->name );
                        curr->next      = ban_free;
                        ban_free        = curr;
                        send_to_char( "Ok.\n\r", ch );
                        return;
                }
        }

        send_to_char( "Site is not banned.\n\r", ch );
        return;
}


void do_wizlock( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        extern bool       wizlock;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_wizlock ) )
                return;

        wizlock = !wizlock;

        if ( wizlock )
                send_to_char( "Game wizlocked.\n\r", ch );
        else
                send_to_char( "Game wizlock removed.\n\r", ch );

        return;
}


void do_slookup( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        char       buf  [ MAX_STRING_LENGTH ];
        char       buf1 [ MAX_STRING_LENGTH*3];
        char       arg  [ MAX_INPUT_LENGTH ];
        int        sn;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_slookup ) )
                return;

        one_argument( argument, arg );
        if ( arg[0] == '\0' )
        {
                send_to_char( "Slookup what?\n\r", ch );
                return;
        }

        if ( !str_cmp( arg, "all" ) )
        {
                buf1[0] = '\0';
                for ( sn = 0; sn < MAX_SKILL; sn++ )
                {
                        if ( !skill_table[sn].name )
                                break;
                        sprintf( buf, "Sn: %4d Skill/spell: '%s'\n\r",
                                sn, skill_table[sn].name );
                        strcat( buf1, buf );
                }
                send_to_char( buf1, ch );
        }
        else
        {
                if ( ( sn = skill_lookup( arg ) ) < 0 )
                {
                        send_to_char( "No such skill or spell.\n\r", ch );
                        return;
                }

                sprintf( buf, "Sn: %4d Skill/spell: '%s'\n\r",
                        sn, skill_table[sn].name );
                send_to_char( buf, ch );
        }

        return;
}


void do_sset( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg1  [ MAX_INPUT_LENGTH ];
        char       arg2  [ MAX_INPUT_LENGTH ];
        char       arg3  [ MAX_INPUT_LENGTH ];
        int        value;
        int        sn;
        bool       fAll;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_sset ) )
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
        {
                send_to_char( "Syntax: sset <<victim> <<skill> <<value>\n\r"
                             "or:     sset <<victim> all     <<value>\n\r"
                             "Skill being any skill or spell.\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg1 ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        fAll = !str_cmp( arg2, "all" );
        sn   = 0;
        if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
        {
                send_to_char( "No such skill or spell.\n\r", ch );
                return;
        }

        /*
         * Snarf the value.
         */
        if ( !is_number( arg3 ) )
        {
                send_to_char( "Value must be numeric.\n\r", ch );
                return;
        }

        value = atoi( arg3 );
        if ( value < 0 || value > 100 )
        {
                if (victim->level < L_SEN) {
                        send_to_char( "Value range is 0 to 100.\n\r", ch );
                        return;
                }
        }

        if ( fAll )
        {
                if ( get_trust( ch ) < L_DIR )
                {
                        send_to_char( "Only Directors may sset all.\n\r", ch );
                        return;
                }
                for ( sn = 0; sn < MAX_SKILL; sn++ )
                {
                        if ( skill_table[sn].name != NULL )
                                victim->pcdata->learned[sn]     = value;
                }
        }
        else
        {
                victim->pcdata->learned[sn] = value;
        }

        return;
}


/*
 * loads clan items - Istari
 * Redone by Gezhp 2000
 */
void do_oclanitem (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg1 [ MAX_INPUT_LENGTH  ];
        char arg2 [ MAX_INPUT_LENGTH  ];
        char buf  [ MAX_STRING_LENGTH ];
        OBJ_DATA *clanobj;
        int clan;
        int level;
        int itemvnum;

        if (IS_NPC(ch))
                return;

        if (!IS_SET(ch->act, PLR_LEADER) && ch->level <= LEVEL_HERO)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char("Syntax: oclanitem <<character> <<low|mid|high|pouch>\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg1)))
        {
                send_to_char("That player is not here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char( "They have no need for such things.\n\r", ch );
                return;
        }

        clan = victim->clan;
        level = victim->level;
        itemvnum = -1;

        if (!clan)
        {
                send_to_char("They are not in a clan.\n\r", ch);
                return;
        }

        if (ch->clan != clan && ch->level <= LEVEL_HERO)
        {
                send_to_char("They are not in your clan.\n\r",ch);
                return;
        }

        if (!str_cmp(arg2, "low"))
        {
                if (level > 14)
                        itemvnum = clan_item_list[clan].first_item;
        }
        else if (!str_cmp(arg2, "mid"))
        {
                if (level > 29)
                        itemvnum = clan_item_list[clan].second_item;
        }
        else if (!str_cmp(arg2, "high"))
        {
                if (level > 59)
                        itemvnum = clan_item_list[clan].third_item;
        }
        else if (!str_cmp(arg2, "pouch"))
        {
                if (level > 49)
                        itemvnum = clan_item_list[clan].pouch_item;
        }
        else
        {
                send_to_char("Syntax: oclanitem <<character> <<low|mid|high|pouch>\n\r", ch);
                return;
        }

        if (itemvnum < 0)
        {
                send_to_char("They cannot use that item yet.\n\r", ch);
                return;
        }

        if (!(get_obj_index(itemvnum)))
        {
                send_to_char( "Ack! That item is undefined: please inform an Immortal.\n\r", ch);
                return;
        }

        clanobj = create_object(get_obj_index(itemvnum), level,"common", CREATED_NO_RANDOMISER);
        set_obj_owner(clanobj, victim->name);
        obj_to_char(clanobj, ch);

        sprintf(buf, "You create %s for %s.\n\r",
                clanobj->short_descr,
                victim->name);
        send_to_char(buf,ch);
}


void do_mset( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA               *rch;
        CHAR_DATA               *victim;
        char                    buf  [ MAX_STRING_LENGTH ];
        char                    arg1 [ MAX_INPUT_LENGTH  ];
        char                    arg2 [ MAX_INPUT_LENGTH  ];
        char                    arg3 [ MAX_INPUT_LENGTH  ];
        unsigned long int       value;
        unsigned long int       bvalue;
        int                     ivalue;
        char                    *bptr;
        int                     max;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_mset ) )
                return;

        smash_tilde( argument );
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
        {
                send_to_char("Syntax: mset <<victim> <<field>  <<value>\n\r"
                             "or:     mset <<victim> <<string> <<value>\n\r\n\r"
                             "Field being one of:\n\r"
                             "  str int wis dex con class level body_form\n\r"
                             "  hp mana move str_prac int_prac align\n\r"
                             "  thirst drunk full sub_class form race \n\r"
                             "  bounty fame questpoints totalqp questtime\n\r"
                             "  patron deity_timer deity_flags affected_by\n\r"
                             "  bank plat gold silver copper age rage spec\n\r"
                             "  act crit swiftness bonus max_bonus slept\n\r"
                             "String being one of:\n\r"
                             "  name short long title spec\n\r", ch);
                return;
        }

        if ( !( victim = get_char_world( ch, arg1 ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        /*
         * Snarf the mset value (which need not be numeric) and create necessary types for
         * different applications (value for most, bvalue for bitflags, ivalue for applications
         * that allow negative values like 'align' )
         */

        value = is_number( arg3 ) ? atoi( arg3 ) : -1;
        bvalue = strtoul(arg3, &bptr, 10);
        ivalue = is_number( arg3 ) ? atoi( arg3 ) : -1;

        /*
         * Set something.
         */
        if ( !str_cmp( arg2, "str" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                max = 21 + race_table[ch->race].str_bonus
                        + class_table[ch->class].class_stats[0];

                if ( value < 3 || value > max )
                {
                        sprintf( buf, "Strength range is 3 to %d.\n\r", max );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->perm_str = value;
                return;
        }

        if ( !str_cmp( arg2, "int" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                max = 21 + race_table[ch->race].int_bonus
                        + class_table[ch->class].class_stats[1];

                if ( value < 3 || value > max )
                {
                        sprintf( buf, "Intelligence range is 3 to %d.\n\r", max );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->perm_int = value;
                return;
        }

        if ( !str_cmp( arg2, "wis" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                max = 21 + race_table[ch->race].wis_bonus
                        + class_table[ch->class].class_stats[2];

                if ( value < 3 || value > max )
                {
                        sprintf( buf, "Wisdom range is 3 to %d.\n\r", max );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->perm_wis = value;
                return;
        }

        if ( !str_cmp( arg2, "dex" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                max = 21 + race_table[ch->race].dex_bonus
                        + class_table[ch->class].class_stats[3];

                if ( value < 3 || value > max )
                {
                        sprintf( buf, "Dexterity range is 3 to %d.\n\r", max );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->perm_dex = value;
                return;
        }

        if ( !str_cmp( arg2, "con" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                max = 21 + race_table[ch->race].con_bonus
                        + class_table[ch->class].class_stats[4];

                if ( value < 3 || value > max )
                {
                        sprintf( buf, "Constitution range is 3 to %d.\n\r", max );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->perm_con = value;
                return;
        }

        if ( !str_cmp( arg2, "class" ) )
        {
                if ( value < 0 || value >= MAX_CLASS )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
                        send_to_char( buf, ch );
                        return;
                }
                victim->class = value;
                return;
        }

        if ( !str_cmp( arg2, "crit" ) )
        {
                if ( value < 0 || value >= 100 )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "Crit range is 0 to 100.\n" );
                        send_to_char( buf, ch );
                        return;
                }
                victim->crit = value;
                return;
        }

        if ( !str_cmp( arg2, "swiftness" ) )
        {
                if ( value < 0 || value >= 100 )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "Swiftness range is 0 to 100.\n" );
                        send_to_char( buf, ch );
                        return;
                }
                victim->swiftness = value;
                return;
        }

        if ( !str_cmp( arg2, "sub_class" ) )
        {
                if ( value < 0 || value >= MAX_SUB_CLASS )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "Sub Class range is 0 to %d.\n", MAX_SUB_CLASS-1 );
                        send_to_char( buf, ch );
                        return;
                }
                victim->sub_class = value;
                return;
        }

        if ( !str_cmp( arg2, "form" ) )
        {
                if ( value < 0 || value >= MAX_FORM )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "The FORM range is 0 to %d.\n", MAX_FORM-1 );
                        send_to_char( buf, ch );
                        return;
                }
                victim->form = value;
                return;
        }

        if ( !str_cmp( arg2, "race" ) )
        {
                if (value < 1 || value >= MAX_RACE )
                {
                        char buf [ MAX_STRING_LENGTH ];
                        sprintf( buf, "The range is 1 to %d (see help RACE).\n\r", ( MAX_RACE - 1) );
                        send_to_char( buf, ch );
                        return;
                }

                victim->race = value;
                return;
        }

        if ( !str_cmp( arg2, "bonus" ) )
        {
                if (value < 0 || value > MAX_BONUS )
                {
                        char buf [ MAX_STRING_LENGTH ];
                        sprintf( buf, "The range is 0 to %d (see help BONUS).\n\r", ( MAX_BONUS ) );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->bonus = value;
                return;
        }

        if ( !str_cmp( arg2, "max_bonus" ) )
        {
                if (value < 0 || value > MAX_BONUS )
                {
                        char buf [ MAX_STRING_LENGTH ];
                        sprintf( buf, "The range is 0 to %d (see help BONUS).\n\r", ( MAX_BONUS ) );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->max_bonus = value;
                return;
        }

        if ( !str_cmp( arg2, "slept" ) )
        {
                if (value < 0 || value > 1 )
                {
                        char buf [ MAX_STRING_LENGTH ];
                        sprintf( buf, "Value can be 0 or 1.\n\r");
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->slept = value;
                return;
        }

        if ( !str_cmp( arg2, "level" ) )
        {
                if ( !IS_NPC( victim ) )
                {
                        send_to_char( "Not on PCs.\n\r", ch );
                        return;
                }

                if ( value < 0 || value > LEVEL_HERO )
                {
                        sprintf( buf, "Level range is 0 to %d.\n\r", LEVEL_HERO );
                        send_to_char( buf, ch );
                        return;
                }

                victim->level = value;
                return;
        }

        if ( !str_cmp( arg2, "bounty" ) )
        {
                if ( IS_NPC( victim ))
                {
                        send_to_char("Not on NPCs.\n\r", ch);
                        return;
                }

                victim->pcdata->bounty = value;
                return;
        }

        if ( !str_cmp( arg2, "bank" ) )
        {
                if ( IS_NPC( victim ))
                {
                        send_to_char("Not on NPCs.\n\r", ch);
                        return;
                }

                victim->pcdata->bank = value;
                return;
        }

        if( !str_cmp( arg2, "body_form" ) )
        {
                if( !IS_NPC( victim ) )
                        send_to_char( "You can't set body form on PCs.\n\r", ch );
                else
                        victim->body_form = bvalue;
                return;
        }

        if( !str_cmp( arg2, "affected_by" ) )
        {
                victim->affected_by = bvalue;
                return;
        }

        if( !str_cmp( arg2, "act" ) )
        {
                /* Below to ensure NPC bit is not removed from mobs. */

                if (IS_NPC(victim))
                {
                        if ( (ACT_IS_NPC & (1 << bvalue )) != 0 )
                        {
                                victim->act = ( bvalue + ACT_IS_NPC );

                        }
                        else {
                                victim->act = bvalue;
                        }
                }
                else {
                        victim->act = bvalue;
                }
                return;
        }


        if ( !str_cmp( arg2, "age" ) )
        {
                if ( ( value < MIN_MSET_AGE )
                ||   ( value > MAX_MSET_AGE ) )
                {
                        sprintf( buf, "Age must be between %d and %d years.\n\r",
                                MIN_MSET_AGE,
                                MAX_MSET_AGE );
                        send_to_char( buf, ch );
                        return;
                }

                value = (value * 14400) - (17 * 14400);
                victim->played = value;
                return;
        }

        if ( !str_cmp( arg2, "fame" ) )
        {
                if ( IS_NPC( victim ))
                {
                        send_to_char("Not on NPCs.\n\r", ch);
                        return;
                }

                victim->pcdata->fame = value;
                return;
        }

        if ( !str_cmp( arg2, "plat" ) )
        {
                victim->plat = value;
                return;
        }

        if ( !str_cmp( arg2, "gold" ) )
        {
                victim->gold = value;
                return;
        }

        if ( !str_cmp( arg2, "silver" ) )
        {
                victim->silver = value;
                return;
        }

        if ( !str_cmp( arg2, "copper" ) )
        {
                victim->copper = value;
                return;
        }

        if ( !str_cmp( arg2, "hp" ) )
        {
                if ( ivalue < MIN_MSET_HP || ivalue > MAX_MSET_HP )
                {
                        sprintf( buf, "Hp range is %d to %d hit points.\n\r",
                                MIN_MSET_HP,
                                MAX_MSET_HP );
                        send_to_char( buf, ch );
                        return;
                }

                if ( victim->fighting && ivalue < 0 )
                {
                        send_to_char( "You cannot set a fighting person's hp below 0.\n\r", ch );
                        return;
                }
                victim->max_hit = ivalue;
                return;
        }

        if (!str_cmp(arg2, "rage"))
        {
                if (ivalue < 0)
                        return;

                victim->rage = ivalue;
                return;
        }

        if ( !str_cmp( arg2, "mana" ) )
        {
                if ( value < MIN_MSET_MANA || value > MAX_MSET_MANA )
                {
                        sprintf( buf, "Mana range is %d to %d mana points.\n\r",
                                MIN_MSET_MANA,
                                MAX_MSET_MANA );
                        send_to_char( buf, ch );
                        return;
                }

                victim->max_mana = value;
                return;
        }

        if ( !str_cmp( arg2, "move" ) )
        {
                if ( value < MIN_MSET_MOVE || value > MAX_MSET_MOVE )
                {
                        sprintf( buf, "Move range is %d to %d move points.\n\r",
                                MIN_MSET_MOVE,
                                MAX_MSET_MOVE );
                        send_to_char( buf, ch );
                        return;
                }

                victim->max_move = value;
                return;
        }

        if ( !str_cmp( arg2, "str_prac" ) )
        {
                if (IS_NPC(victim))
                {
                        send_to_char("Not on NPCs.\n\r", ch);
                        return;
                }

                if ( value < MIN_MSET_STR_PRAC || value > MAX_MSET_STR_PRAC )
                {
                        sprintf( buf, "Physical practices range is %d to %d sessions.\n\r",
                                MIN_MSET_STR_PRAC,
                                MAX_MSET_STR_PRAC );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->str_prac = value;
                return;
        }

        if ( !str_cmp( arg2, "int_prac" ) )
        {
                if (IS_NPC(victim))
                {
                        send_to_char("Not on NPCs.\n\r", ch);
                        return;
                }

                if ( value < MIN_MSET_INT_PRAC || value > MAX_MSET_INT_PRAC )
                {
                        sprintf( buf, "Int practices range is %d to %d sessions.\n\r",
                                MIN_MSET_INT_PRAC,
                                MAX_MSET_INT_PRAC );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->int_prac = value;
                return;
        }

        if ( !str_cmp( arg2, "totalqp" ) )
        {

                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( value < MIN_MSET_TOTALQP || value > MAX_MSET_TOTALQP )
                {
                        sprintf( buf, "Total QP range is %d to %d.\n\r",
                                MIN_MSET_TOTALQP,
                                MAX_MSET_TOTALQP );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->totalqp = value;
                return;
        }

        if ( !str_cmp( arg2, "questpoints" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( value < MIN_MSET_QUESTPOINTS || value > MAX_MSET_QUESTPOINTS )
                {
                        sprintf( buf, "Questpoint range is %d to %d.\n\r",
                                MIN_MSET_QUESTPOINTS,
                                MAX_MSET_QUESTPOINTS );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->questpoints = value;
                return;
        }

        if ( !str_cmp( arg2, "questtime" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( value < MIN_MSET_QUESTTIME || value > MAX_MSET_QUESTTIME )
                {
                        sprintf( buf, "Questtime range is %d to %d.\n\r",
                                MIN_MSET_QUESTTIME,
                                MAX_MSET_QUESTTIME );
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->nextquest = value;
                return;
        }

        if ( !str_cmp( arg2, "align" ) )
        {
                if ( ivalue < -1000 || ivalue > 1000 )
                {

                        send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
                        return;
                }

                victim->alignment = ivalue;
                return;
        }

        if ( !str_cmp( arg2, "thirst" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( ( ivalue < 0 || ivalue > MAX_DRINK )
                    && get_trust( victim ) < LEVEL_IMMORTAL )
                {
                        sprintf( buf, "Thirst range is 0 to %d.\n\r", MAX_DRINK);
                        send_to_char( buf, ch );
                        return;
                }
                else
                        if ( ivalue < -1 || ivalue > MAX_DRINK )
                        {
                                sprintf( buf, "Thirst range is -1 to %d.\n\r", MAX_DRINK);
                                send_to_char( buf, ch );
                                return;
                        }

                victim->pcdata->condition[COND_THIRST] = ivalue;
                return;
        }

        if ( !str_cmp( arg2, "drunk" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( value < 0 || value > MAX_DRUNK )
                {
                        sprintf( buf, "Drunk range is 0 to %d.\n\r", MAX_DRUNK);
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->condition[COND_DRUNK] = value;
                return;
        }

        if ( !str_cmp( arg2, "full" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                if ( ( ivalue < 0 || ivalue > MAX_FOOD )
                    && get_trust( victim ) < LEVEL_IMMORTAL )
                {
                        sprintf( buf, "Full range is 0 to %d.\n\r", MAX_FOOD);
                        send_to_char( buf, ch );
                        return;
                }
                else if ( ivalue < -1 || ivalue > MAX_FOOD )
                {
                        sprintf( buf, "Full range is -1 to %d.\n\r", MAX_FOOD);
                        send_to_char( buf, ch );
                        return;
                }

                victim->pcdata->condition[COND_FULL] = ivalue;
                return;
        }

        if ( !str_cmp( arg2, "name" ) )
        {
                if ( !IS_NPC( victim ) )
                {
                        send_to_char( "Not on PCs.\n\r", ch );
                        return;
                }

                if ( longstring( ch, arg3 ) )
                        return;

                free_string( victim->name );
                victim->name = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "short" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( victim->short_descr );
                victim->short_descr = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "long" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( victim->long_descr );
                strcat( arg3, "\n\r" );
                victim->long_descr = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "title" ) )
        {
                if ( IS_NPC( victim ) )
                {
                        send_to_char( "Not on NPCs.\n\r", ch );
                        return;
                }

                set_title( victim, arg3 );
                return;
        }

        if ( !str_cmp( arg2, "spec" ) )
        {
                if ( !IS_NPC( victim ) )
                {
                        send_to_char( "Not on PCs.\n\r", ch );
                        return;
                }

                if ( !( victim->spec_fun = spec_lookup( arg3 ) ) )
                {
                        send_to_char( "No such spec fun.\n\r", ch );
                        return;
                }

                return;
        }

        if (!strcmp (arg2, "patron"))
        {
                if (IS_NPC (victim))
                {
                        send_to_char ("Not on an NPC.\n\r", ch);
                        return;
                }

                if (ch->level < L_IMM)
                {
                        send_to_char("Not authorised.\n\r", ch);
                        return;
                }

                if (ivalue < 0 || ivalue >= NUMBER_DEITIES)
                {
                        victim->pcdata->deity_patron = -1;
                        send_to_char ("Character now has no patron.\n\r", ch);
                }
                else
                {
                        victim->pcdata->deity_patron = ivalue;
                        send_to_char ("Ok.\n\r", ch);
                }
                return;
        }

        if (!strcmp (arg2, "deity_timer"))
        {
                if (IS_NPC (victim))
                {
                        send_to_char ("Not on an NPC.\n\r", ch);
                        return;
                }

                if (ch->level < L_IMM)
                {
                        send_to_char("Not authorised.\n\r", ch);
                        return;
                }

                if ( value < MIN_MSET_DEITYTIMER || value > MAX_MSET_DEITYTIMER )
                {
                        sprintf( buf, "You may set the timer from %d to %d ticks.\n\r",
                                MIN_MSET_DEITYTIMER,
                                MAX_MSET_DEITYTIMER );
                        send_to_char( buf, ch );
                        return;
                }
                else
                {
                        victim->pcdata->deity_timer = value;
                        send_to_char ("Ok.\n\r", ch);
                }
                return;
        }

        if (!strcmp (arg2, "deity_flags"))
        {
                if (IS_NPC (victim))
                {
                        send_to_char ("Not on an NPC.\n\r", ch);
                        return;
                }

                if (ch->level < L_IMM)
                {
                        send_to_char("Not authorised.\n\r", ch);
                        return;
                }

                if (ivalue < 0)
                {
                        send_to_char ("Flag value must not be negative.\n\r", ch);
                }
                else
                {
                        victim->pcdata->deity_flags = ivalue;
                        send_to_char ("Ok.\n\r", ch);
                }
                return;
        }

        /*
         * Generate usage message.
         */
        do_mset( ch, "" );
        return;
}


void do_oset( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA                *obj;
        CHAR_DATA               *rch;
        char                    arg1 [ MAX_INPUT_LENGTH ];
        char                    arg2 [ MAX_INPUT_LENGTH ];
        char                    arg3 [ MAX_INPUT_LENGTH ];
        char                    arg4 [ MAX_INPUT_LENGTH ];
        int                     value;
        unsigned long int       bvalue;
        char                    *bptr;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_oset ) )
                return;

        smash_tilde( argument );
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
        {
                send_to_char( "Syntax: oset <<object> <<field>  <<value>\n\r"
                             "or:     oset <<object> <<string> <<value>\n\r\n\r"
                             "Field being one of:\n\r"
                             "  value0 value1 value2 value3 owner ego\n\r"
                             "  extra wear level weight cost timer timermax\n\r"
                             "  spawn instances\n\r\n\r"
                             "String being one of:\n\r"
                             "  name short long ed\n\r", ch );
                return;
        }

        if ( !( obj = get_obj_world( ch, arg1 ) ) )
        {
                send_to_char( "Nothing like that in the domain.\n\r", ch );
                return;
        }

        /*
         * Snarf the value (which need not be numeric).
         */
        value = atoi(arg3);
        bvalue = strtoul(arg3, &bptr, 10);
        /*
         * Set something.
         */
        if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
        {
                obj->value[0] = value;
                return;
        }

        if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
        {
                obj->value[1] = value;
                return;
        }

        if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
        {
                obj->value[2] = value;
                return;
        }

        if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
        {
                obj->value[3] = value;
                return;
        }

        if ( !str_cmp( arg2, "extra" ) )
        {
                obj->extra_flags = bvalue;
                return;
        }

        if ( !str_cmp( arg2, "wear" ) )
        {
                obj->wear_flags = value;
                return;
        }

        if ( !str_cmp( arg2, "level" ) )
        {
            obj->level = value;
            return;
        }

        if ( !str_cmp( arg2, "weight" ) )
        {
                obj->weight = value;
                return;
        }

        if ( !str_cmp( arg2, "cost" ) )
        {
                obj->cost = value;
                return;
        }

        if ( !str_cmp( arg2, "type" ) )
        {
                obj->item_type  = value;
                return;
        }

        if ( !str_cmp( arg2, "timer" ) )
        {
                obj->timer = value;
                return;
        }

        if ( !str_cmp( arg2, "timermax" ) )
        {
                obj->timermax = value;
                return;
        }

        if (!str_cmp(arg2, "ego"))
        {
                obj->ego_flags = value;
                return;
        }

        if (!str_cmp(arg2, "owner"))
        {
                if (!str_cmp(arg3, "none"))
                        set_obj_owner(obj, "");
                else
                        set_obj_owner(obj, arg3);
                return;
        }

        if ( !str_cmp( arg2, "spawn" ) )
        {
                obj->pIndexData->spawn_count = value;
                return;
        }

        if ( !str_cmp( arg2, "instances" ) )
        {
                obj->pIndexData->max_instances = value;
                return;
        }

        if ( !str_cmp( arg2, "name" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->name );
                obj->name = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "short" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->short_descr );
                obj->short_descr = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "long" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->description );
                obj->description = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "ed" ) )
        {
                EXTRA_DESCR_DATA *ed;

                argument = one_argument( argument, arg3 );
                argument = one_argument( argument, arg4 );

                if ( arg4[0] == '\0' )
                {
                        send_to_char( "Syntax: oset <<object> ed <<keyword> <<string>\n\r", ch );
                        return;
                }

                if ( !extra_descr_free )
                {
                        ed                      = alloc_perm( sizeof( *ed ) );
                }
                else
                {
                        ed                      = extra_descr_free;
                        extra_descr_free        = extra_descr_free->next;
                }

                ed->keyword             = str_dup( arg3     );
                ed->description         = str_dup( argument );
                ed->deleted             = FALSE;
                ed->next                = obj->extra_descr;
                obj->extra_descr        = ed;
                return;
        }

        /*
         * Generate usage message.
         */
        do_oset( ch, "" );
        return;
}


void do_rename( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA  *obj;
        CHAR_DATA *rch;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        char       arg3 [ MAX_INPUT_LENGTH ];
        char       arg4 [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_rename ) )
                return;

        smash_tilde( argument );
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
        {
                send_to_char( "Syntax: rename <<object> <<string> <<value>\n\r",     ch );
                send_to_char( "\n\r",                                           ch );
                send_to_char( "String being one of:\n\r",                       ch );
                send_to_char( "  name short long ed\n\r",                       ch );
                return;
        }

        if ( !(obj = get_obj_carry( ch, arg1 ) ) )
        {
                send_to_char( "You dont have that item.\n\r", ch );
                return;
        }

        if ( !( obj = get_obj_world( ch, arg1 ) ) )
        {
                send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
                return;
        }

        /*
         * Place a few restrictions
         */

        if ( ( obj->item_type == ITEM_CLAN_OBJECT
        ||    !is_obj_owner( obj, ch ) )
        &&    !IS_IMMORTAL( ch ) )
        {
                send_to_char("You may not rename that item.\n\r", ch);
                return;
        }

        /*
         * Set something.
         */

        if ( !str_cmp( arg2, "name" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->name );
                obj->name = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "short" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->short_descr );
                obj->short_descr = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "long" ) )
        {
                if ( longstring( ch, arg3 ) )
                        return;

                free_string( obj->description );
                obj->description = str_dup( arg3 );
                return;
        }

        if ( !str_cmp( arg2, "ed" ) )
        {
                EXTRA_DESCR_DATA *ed;

                argument = one_argument( argument, arg3 );
                argument = one_argument( argument, arg4 );

                if ( arg4[0] == '\0' )
                {
                        send_to_char( "Syntax: rename <<object> ed <<keyword> <<value>\n\r", ch );
                        return;
                }

                if ( !extra_descr_free )
                {
                        ed                      = alloc_perm( sizeof( *ed ) );
                }
                else
                {
                        ed                      = extra_descr_free;
                        extra_descr_free        = extra_descr_free->next;
                }

                ed->keyword             = str_dup( arg3     );
                ed->description         = str_dup( argument );
                ed->deleted             = FALSE;
                ed->next                = obj->extra_descr;
                obj->extra_descr        = ed;
                return;
        }

        /*
         * Generate usage message.
         */
        do_rename( ch, "" );
        return;
}


void do_rset( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA               *rch;
        CHAR_DATA               *person;
        ROOM_INDEX_DATA         *location;
        char                    arg1 [ MAX_INPUT_LENGTH ];
        char                    arg2 [ MAX_INPUT_LENGTH ];
        char                    arg3 [ MAX_INPUT_LENGTH ];
        int                     value;
        unsigned long int       bvalue;
        char                    *bptr;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_rset ) )
                return;

        smash_tilde( argument );
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
        {
                send_to_char( "Syntax: rset <<location> <<field> value\n\r",      ch );
                send_to_char( "\n\r",                                           ch );
                send_to_char( "Field being one of:\n\r",                        ch );
                send_to_char( "  flags sector\n\r",                             ch );
                return;
        }

        if ( !( location = find_location( ch, arg1 ) ) )
        {
                send_to_char( "No such location.\n\r", ch );
                return;
        }

        /*
         * Snarf the value.
         */
        if ( !is_number( arg3 ) )
        {
                send_to_char( "Value must be numeric.\n\r", ch );
                return;
        }
        value = atoi( arg3 );
        bvalue = strtoul(arg3, &bptr, 10);

        for ( person = location->people; person; person = person->next_in_room )
        {
                if ( person != ch && person->level >= ch->level && IS_IMMORTAL(person))
                {
                        send_to_char("Your superior is in this room, no rsetting now.\n\r", ch );
                        return;
                }
        }

        /*
         * Set something.
         */
        if ( !str_cmp( arg2, "flags" ) )
        {
                location->room_flags    = bvalue;
                return;
        }

        if ( !str_cmp( arg2, "sector" ) )
        {
                location->sector_type   = value;
                return;
        }

        /*
         * Generate usage message.
         */
        do_rset( ch, "" );
        return;
}


void do_users (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *rch;
        DESCRIPTOR_DATA *d;
        char            buf  [ MAX_STRING_LENGTH ];
        char            buf2 [ MAX_STRING_LENGTH ];
        int             count;

        const char *connect_desc [24] =
        {
                "playing",              "get name",             "get old pwd",
                "confirm pwd",          "get new pwd",          "confirm pwd",
                "display race",         "choose race",          "confirm race",
                "choose sex",           "confirm sex",          "choose class",
                "confirm class",        "read motd",            "roll stats",
                "confirm stats",        "note to",              "note subject",
                "note expiry",          "note text",            "note finish",
                "welcome note",         "reconnect pwd",        "confirm ansi"
        };

        rch = get_char( ch );

        if ( !authorized( rch, gsn_users ) )
                return;

        count = 0;
        buf[0] = '\0';
        buf2[0] = '\0';

        for ( d = descriptor_list; d; d = d->next )
        {
                if ( d->character && can_see( ch, d->character ) )
                {
                        count++;
                        sprintf( buf + strlen( buf ), "\n\r[{G%2d {W%-13s{x] {W%-12s{x {C%s{x\n\r",
                                d->descriptor,
                                (d->connected < 0 || d->connected > 23)
                                        ? "?" : connect_desc[d->connected],
                                d->original  ? d->original->name  :
                                d->character ? d->character->name : "(none)",
                                d->host );
                }
        }

        sprintf( buf2, "\n\r{W%d{x users    connection_count = {C%d{x/{c%d{x\n\r",
                count,
                connection_count,
                MAX_CONNECTIONS );
        strcat( buf, buf2 );
        send_to_char( buf, ch );
        return;
}


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        char       arg [ MAX_INPUT_LENGTH ];
        int        trust;
        int        cmd;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_force ) )
                return;

        argument = one_argument( argument, arg );

        if ( arg[0] == '\0' || argument[0] == '\0' )
        {
                send_to_char( "Force whom to do what?\n\r", ch );
                return;
        }

        /*
         * Look for command in command table.
         */
        trust = get_trust( ch );

        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
                if ( argument[0] == cmd_table[cmd].name[0]
                    && !str_prefix( argument, cmd_table[cmd].name )
                    && ( cmd_table[cmd].level > trust ) )
                {
                        send_to_char( "You can't even do that yourself!\n\r", ch );
                        return;
                }
        }

        if ( !str_cmp( arg, "all" ) )
        {
                CHAR_DATA *vch;

                for ( vch = char_list; vch; vch = vch->next )
                {
                        if ( vch->deleted )
                                continue;

                        if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
                        {
                                act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
                                interpret( vch, argument );
                        }
                }
        }
        else
        {
                CHAR_DATA *victim;

                if ( !( victim = get_char_world( ch, arg ) ) )
                {
                        send_to_char( "They aren't here.\n\r", ch );
                        return;
                }

                if ( victim == ch )
                {
                        send_to_char( "Aye aye, right away!\n\r", ch );
                        return;
                }

                if ( get_trust( victim ) >= get_trust( ch ) )
                {
                        send_to_char( "Do it yourself.\n\r", ch );
                        return;
                }

                act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
                interpret( victim, argument );
        }

        send_to_char( "Ok.\n\r", ch );
        return;
}


/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
        if ( IS_NPC( ch ) )
                return;

        if ( !authorized( ch, gsn_wizinvis ) )
                return;

        if ( IS_SET( ch->act, PLR_WIZINVIS ) )
        {
                REMOVE_BIT( ch->act, PLR_WIZINVIS );
                send_to_char( "You slowly fade back into existence.\n\r", ch    );
                act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
                send_to_char( "You slowly vanish into thin air.\n\r",     ch    );
                act( "$n slowly fades into thin air.",  ch, NULL, NULL, TO_ROOM );
                SET_BIT(    ch->act, PLR_WIZINVIS );
        }

        return;
}


void do_holylight( CHAR_DATA *ch, char *argument )
{
        if ( IS_NPC( ch ) )
                return;

        if ( !authorized( ch, gsn_holylight ) )
                return;

        if ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
        {
                REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
                send_to_char( "Holy light mode off.\n\r", ch );
        }
        else
        {
                SET_BIT( ch->act, PLR_HOLYLIGHT );
                send_to_char( "Holy light mode on.\n\r", ch );
        }

        return;
}


/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_wizify ) )
                return;

        argument = one_argument( argument, arg1  );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Syntax: wizify <<name>\n\r" , ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg1 ) ) )
        {
                send_to_char( "They aren't here.\n\r" , ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on mobs.\n\r", ch );
                return;
        }

        victim->wizbit = !victim->wizbit;

        if ( victim->wizbit )
        {
                act( "$N wizified.",         ch, NULL, victim, TO_CHAR );
                act( "$n has wizified you!", ch, NULL, victim, TO_VICT );
        }
        else
        {
                act( "$N dewizzed.",         ch, NULL, victim, TO_CHAR );
                act( "$n has dewizzed you!", ch, NULL, victim, TO_VICT );
        }

        do_save( victim, "");
        return;
}


/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
        OBJ_DATA  *obj;
        OBJ_DATA  *in_obj;
        CHAR_DATA *rch;
        char       buf  [ MAX_STRING_LENGTH ];
        char       arg  [ MAX_INPUT_LENGTH  ];
        int        obj_counter = 1;
        bool       found = FALSE;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_owhere ) )
                return;

        one_argument( argument, arg );

        if( arg[0] == '\0' )
        {
                send_to_char( "Syntax:  owhere <<object>.\n\r", ch );
                return;
        }
        else
        {
                if (strlen (arg) < 3)
                {
                        send_to_char ("Argument must be at least three letters long.\n\r", ch);
                        return;
                }

                for ( obj = object_list; obj; obj = obj->next )
                {
                        if ( !can_see_obj( ch, obj ) || !multi_keyword_match( arg, obj->name ) )
                                continue;

                        found = TRUE;

                        for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
                                ;

                        if ( in_obj->carried_by )
                        {
                                if ( !can_see( ch, in_obj->carried_by ) )
                                        continue;

                                sprintf( buf, "{Y%3d{x. {g%s{x [{W%d{x] carried by {c%s{x at [{W%d{x]\n\r",
                                        obj_counter,
                                        obj->short_descr,
                                        obj->pIndexData->vnum,
                                        PERS( in_obj->carried_by, ch ),
                                        in_obj->carried_by->in_room->vnum );
                        }
                        else
                        {
                                sprintf( buf, "{Y%3d{x. {g%s{x [{W%d{x] in {m%s{x at [{W%d{x]\n\r",
                                        obj_counter,
                                        obj->short_descr,
                                        obj->pIndexData->vnum,
                                        ( !in_obj->in_room ) ?
                                        "somewhere" : in_obj->in_room->name,
                                        ( !in_obj->in_room ) ?
                                        0 : in_obj->in_room->vnum );
                        }

                        obj_counter++;
                        buf[0] = UPPER( buf[0] );
                        send_to_char (buf, ch);
                }
        }

        if ( !found )
                send_to_char("Nothing like that in hell, earth, or heaven.\n\r" , ch);

        return;
}


void do_numlock( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        char buf [ MAX_STRING_LENGTH ];
        char arg1 [ MAX_INPUT_LENGTH  ];
        extern int numlock;
        int temp;

        rch = get_char( ch );

        if ( !authorized( rch, gsn_numlock ) )
                return;

        argument = one_argument( argument, arg1 );
        temp = atoi( arg1 );

        if ( arg1[0] == '\0' )
        {
                sprintf( buf, "Current numlock setting is:  %d.\n\r", numlock );
                send_to_char( buf, ch );
                return;
        }

        if ( ( temp < 0 ) || ( temp > LEVEL_HERO ) )
        {
                sprintf( buf, "Level must be between 0 and %d.\n\r", LEVEL_HERO );
                send_to_char( buf, ch );
                return;
        }

        numlock = temp;

        if ( numlock != 0 )
        {
                sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
                send_to_char( buf, ch );
        }
        else
                send_to_char( "Game now open to all levels.\n\r", ch );

        return;
}


void do_newlock( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        extern int numlock;
        char buf [ MAX_STRING_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_newlock ) )
                return;

        if ( numlock != 0 && get_trust( ch ) < L_SEN )
        {
                send_to_char( "You may not change the current numlock setting\n\r", ch );
                sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
                send_to_char( buf, ch );
                return;
        }

        if ( numlock != 0 )
        {
                sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
                send_to_char( buf, ch );
                send_to_char( "Changing to: ", ch );
        }

        numlock = 1;
        send_to_char( "Game locked to new characters.\n\r", ch );
        return;
}


void do_sstime( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        extern char *down_time;
        extern char *warning1;
        extern char *warning2;
        char buf  [ MAX_STRING_LENGTH ];
        char arg1 [ MAX_INPUT_LENGTH  ];
        char arg2 [ MAX_INPUT_LENGTH  ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_sstime ) )
                return;

        smash_tilde( argument );
        argument = one_argument( argument, arg1 );
        strcpy ( arg2, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0'
            || ( strlen( arg2 ) != 8 && ( arg2[0] != '*' && strlen( arg2 ) != 1 ) ) )
        {
                send_to_char( "Syntax: sstime <<field> <<value>\n\r",               ch );
                send_to_char( "\n\r",                                             ch );
                send_to_char( "Field being one of:\n\r",                          ch );
                send_to_char( "  downtime  1warning  2warning\n\r",               ch );
                send_to_char( "\n\r",                                             ch );
                send_to_char( "Value being format of:\n\r",                       ch );
                send_to_char( "  hh:mm:ss  (military time) or  *  (for off)\n\r", ch );
                send_to_char( "\n\r",                                             ch );

                sprintf( buf, "1st warning:  %s\n\r2nd warning:  %s\n\r   Downtime:  %s\n\r",
                        warning1, warning2, down_time );
                send_to_char( buf, ch );
                return;
        }

        /* Set something */

        if ( !str_infix( arg1, "downtime" ) )
        {
                free_string( down_time );
                down_time = str_dup( arg2 );
                sprintf( buf, "Downtime is now set to:  %s\n\r", down_time );
                send_to_char( buf, ch );
                return;
        }

        if ( !str_infix( arg1, "1warning" ) )
        {
                free_string( warning1 );
                warning1 = str_dup( arg2 );
                sprintf( buf, "First warning will be given at:  %s\n\r", warning1 );
                send_to_char( buf, ch );
                return;
        }

        if ( !str_infix( arg1, "2warning" ) )
        {
                free_string( warning2 );
                warning2 = str_dup( arg2 );
                sprintf( buf, "Second warning will be given at:  %s\n\r", warning2 );
                send_to_char( buf, ch );
                return;
        }

        /* Generate usage mesage */

        do_sstime( ch, "" );
        return;
}


void other_prac (CHAR_DATA *ch, CHAR_DATA *victim)
{
        char foo  [ MAX_STRING_LENGTH   ];
        int learned;
        int sn;
        int page = 0;
        int new;

        /* those skills learnt */
        sprintf(foo, "\n\rSkills known by %s%s:\n\r", victim->name,
                IS_NPC(victim) ? " (mob)" : "");
        send_to_char(foo, ch);

        /* set to gsn_mage_base when finished */
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                learned = !IS_NPC(victim) ? victim->pcdata->learned[sn] :
                victim->pIndexData->skills->learned[sn];

                if (learned)
                {
                        if (!page++)
                                sprintf(foo, "%30s: %3d%%", skill_table[sn].name, learned);
                        else {
                                sprintf(foo, "    %30s: %3d%%\n", skill_table[sn].name, learned);
                                page = 0;
                        }

                        send_to_char(foo, ch);
                }
        }

        if (!page)
                send_to_char("\n\r", ch);
        else
                send_to_char("\n\r\n\r", ch);

        page = 0;
        new = 0;

        /* those which can now be learnt */
        for (sn = 0; sn < gsn_mage_base; sn++)
        {
                if (!IS_NPC(victim))
                {
                        if (has_pre_req(victim, sn) && (victim->pcdata->learned[sn] == 0))
                        {
                                if (!new)
                                {
                                        send_to_char("-----------------------------------------------------------------------------\n\rSkills which have pre-reqs:\n\r", ch);
                                        new ++;
                                }

                                if (!page++)
                                {
                                        sprintf(foo, "%30s: %3d%%", skill_table[sn].name,
                                                victim->pcdata->learned[sn]);
                                }
                                else
                                {
                                        sprintf(foo, "    %30s: %3d%%\n", skill_table[sn].name,
                                                victim->pcdata->learned[sn]);
                                        page = 0;
                                }

                                send_to_char(foo, ch);
                        }
                }
        }

        if (!page)
                send_to_char("\n\r", ch);
        else
                send_to_char("\n\r\n\r", ch);

        sprintf(foo, "-----------------------------------------------------------------------------\n\r         Physical Prac Penalty: %d\n\r     Intellectual Prac Penalty: %d\n\r\n\r", get_phys_penalty(victim), get_int_penalty(victim));
        send_to_char(foo, ch);

        return;
}


void do_cando (CHAR_DATA *ch, char *argument)
{
        char buf [MAX_STRING_LENGTH];
        char arg1 [MAX_INPUT_LENGTH];
        char arg2 [MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        int sn;

        if (!authorized(ch, gsn_cando))
                return;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if (arg1[0] == '\0')
        {
                send_to_char("Syntax: cando <<victim> <<skill>\n\r", ch);
                send_to_char("No skill argument will list all.\n\r", ch);
                return;
        }

        if (!( victim = get_char_world(ch, arg1)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim))
        {
                if (!victim->pIndexData->skills)
                {
                        send_to_char("That would crash the MUD. Choose a PC.\n\r", ch);
                        return;
                }
        }

        /* snooped prac for target */
        if (arg2[0] == '\0')
        {
                other_prac(ch, victim);
                return;
        }

        sn = skill_lookup(arg2);

        if (sn < 0)
        {
                send_to_char( "No such skill or spell.\n\r", ch );
                return;
        }

        /* get results */
        if ((!IS_NPC(victim) && CAN_DO(victim, sn))
            || (IS_NPC(victim) && victim->pIndexData->skills->learned[sn]))
        {
                sprintf(buf, "\n%s can do %s (%d) at %d%%.\n\r", victim->name,
                        skill_table[sn].name, sn,
                        !IS_NPC(victim) ? victim->pcdata->learned[sn] :
                        victim->pIndexData->skills->learned[sn]);
        }
        else
        {
                sprintf(buf, "%s cannot do %s (%d), but that skill is at %d%%.\n\r",
                        victim->name,
                        skill_table[sn].name, sn,
                        !IS_NPC(victim) ? victim->pcdata->learned[sn] :
                        victim->pIndexData->skills->learned[sn]);
        }

        send_to_char(buf, ch);
}


void do_reset (CHAR_DATA *ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *pch;

        if (!authorized(ch, gsn_reset))
                return;

        reset_area(ch->in_room->area);

        for (pch = char_list; pch; pch = pch->next)
        {
                if (!IS_NPC(pch) && IS_AWAKE(pch) && pch->in_room
                    && pch->in_room->area == ch->in_room->area)
                {
                        send_to_char( "You hear the patter of little feet.\n\r", pch );
                }
        }

        sprintf(buf, "Area: %s reset.\n\r", ch->in_room->area->name);
        send_to_char(buf, ch);
}


void do_wizbrew (CHAR_DATA *ch, char *argument)
{
        /*
         *      Wizbrew a potion, out of thin air, 'cause you're an imm,
         *      and you're worth it.  Nice if you want to quickly make
         *      a quest reward, and that sort of thing.
         *      [Gezhp 2000 deluxe]
         */

        int     item_level;
        int     spell_level;
        char    spell1 [MAX_STRING_LENGTH];
        char    spell2 [MAX_STRING_LENGTH];
        char    spell3 [MAX_STRING_LENGTH];
        char    buf [MAX_STRING_LENGTH];
        char    tmp [MAX_INPUT_LENGTH];
        int     okay;
        int     sn;
        OBJ_DATA* potion;

        if (!authorized (ch, gsn_wizbrew))
                return;

        /*
         *      Let's make sure all the input is kosher first
         */

        okay = 0;
        spell_level = 1;
        item_level = 1;
        buf[0] = '\0';

        do {
                if (!strcmp (argument, ""))
                        break;

                argument = one_argument (argument, tmp);

                if (!is_number (tmp))
                {
                        strcpy (buf, "* The item level needs to be a number, for starters.\n\r");
                        break;
                }

                item_level = atoi (tmp);

                if (item_level < 1 || item_level > MAX_LEVEL)
                {
                        strcpy (buf, "* The item level needs to be between 1 and the maximum immortal level.\n\r");
                        break;
                }

                argument = one_argument (argument, tmp);

                if (!strcmp (tmp, ""))
                {
                        strcpy (buf, "* What about a level for the spells, and one or more spells?\n\r");
                        break;
                }

                if (!is_number (tmp))
                {
                        strcpy (buf, "* The spell level needs to be a number.\n\r");
                        break;
                }

                spell_level = atoi (tmp);

                if (spell_level < 0 || spell_level > 1000)
                {
                        strcpy (buf, "* The spell level must be between 1 and 1000.\n\r");
                        break;
                }

                argument = one_argument (argument, spell1);

                if (!strcmp (spell1, ""))
                {
                        strcpy (buf, "* You'll be wanting a spell in this potion.\n\r");
                        break;
                }

                if ((sn = skill_lookup (spell1)) < 0)
                {
                        strcpy (buf, "* That doesn't look like a valid spell.\n\r");
                        break;
                }

                argument = one_argument (argument, spell2);

                if ((sn = skill_lookup (spell2)) < 0  &&  strcmp (spell2, ""))
                {
                        strcpy (buf, "* That second spell just doesn't sound right.\n\r");
                        break;
                }

                argument = one_argument (argument, spell3);

                if ((sn = skill_lookup (spell3)) < 0  &&  strcmp (spell3, ""))
                {
                        strcpy (buf, "* That third spell looks plain wrong.\n\r");
                        break;
                }

                okay = 1;
        }
        while (!okay);

        if (!okay)
        {
                send_to_char ("To wizbrew: wizbrew <<item level> <<spell level> <<spell 1> [spell 2] [spell 3]\n\r", ch);
                if (buf[0] != '\0')
                        send_to_char (buf, ch);
                else
                        send_to_char ("Example:    wizbrew 10 1 heal\n\rExample:    wizbrew 20 50 'cure poison' sanctuary 'inertial barrier'\n\r", ch);
                return;
        }

        /*
         *      Okedoke, command's okay so let's make a potion
         */

        potion = create_object (get_obj_index (ITEM_VNUM_WIZBREW_VIAL), 0, "common", CREATED_NO_RANDOMISER);
        if (!potion)
        {
                send_to_char ("Oops, couldn't create the potion object: abort!\n\r", ch);
                return;
        }

        strcpy (buf, "a potion of ");
        strcat (buf, spell1);

        if (strcmp (spell2, ""))
        {
                if (strcmp (spell3, ""))
                {
                        strcat (buf, ", ");
                        strcat (buf, spell3);
                }
                strcat (buf, " and ");
                strcat (buf, spell2);
        }

        free_string (potion->short_descr);
        free_string (potion->description);

        potion->short_descr = str_dup (buf);
        strcat (buf, " lies here.");
        buf[0] = UPPER (buf[0]);
        potion->description = str_dup (buf);

        potion->level           = item_level;
        potion->value[0]        = spell_level;
        potion->value[1]        = skill_lookup(spell1);
        potion->value[2]        = skill_lookup(spell2);
        potion->value[3]        = skill_lookup(spell3);

        send_to_char ("You wizbrew yourself a lovely potion.\n\r", ch);
        if (ch->carry_number >=  can_carry_n (ch))
        {
                obj_to_room (potion, ch->in_room);
                send_to_char ("Couldn't fit the thing in your inventory, so it's on the ground.\n\r", ch);
        }
        else obj_to_char (potion, ch);
}

void do_guide( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA *rch;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        rch = get_char( ch );

        if ( !authorized( rch, gsn_guide ) )
                return;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Make whom a Guide?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                send_to_char( "They aren't here.\n\r", ch );
                return;
        }

        if ( IS_NPC( victim ) )
        {
                send_to_char( "Not on NPCs.\n\r", ch );
                return;
        }

        if (victim->clan)
        {
                send_to_char( "Clanned characters may not be guides.\n\r", ch);
                return;
        }

        if ( IS_SET( victim->act, PLR_GUIDE ) )
        {
                REMOVE_BIT( victim->act, PLR_GUIDE );
                send_to_char( "Guide flag removed.\n\r",     ch     );
                send_to_char( "You are no longer a Guide\n\r", victim );
        }
        else
        {
                SET_BIT(    victim->act, PLR_GUIDE );
                send_to_char( "Guide flag set.\n\r",            ch     );
                send_to_char( "You are now a Guide!!\n\r", victim );
        }

        save_char_obj( victim );
}

void do_bitsum(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    int num;
    unsigned long long bitvalue_sum = 0;

    // Loop through each argument
    while (*argument != '\0') {
        argument = one_argument(argument, arg);

        if (is_number(arg))
        {
            num = atoi(arg);

            if (num >= 0 && num < 64)
            {
                bitvalue_sum += (1ULL << num);
            }
            else {
                send_to_char("Numbers must be between 0 and 63.\n\r", ch);
                return;
            }
        } else {
            send_to_char("Invalid input. Please enter numbers only.\n\r", ch);
            return;
        }
    }

    char buf[MAX_STRING_LENGTH];
    snprintf(buf, sizeof(buf), "Bitvalue sum: %llu\n\r", bitvalue_sum);
    send_to_char(buf, ch);
}

/* EOF act_wiz.c */
