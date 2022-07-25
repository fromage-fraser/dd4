/*
 *  tournament.c
 * 
 *  Routines to allow Immortals to organise Pkilling tournaments
 *  in the Midgaard Arena.  See 'HELP TOURNAMENT'.
 * 
 *  Gezhp 2000
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


/*
 *  Globals
 */

int tournament_entrant_count [TOURNAMENT_MAX_TEAMS];
int tournament_banned_count;
int tournament_status;
int tournament_type;
int tournament_lower_level;
int tournament_upper_level;
int tournament_options;
int tournament_team_count;
int tournament_countdown;

struct tournament_entrant tournament_entrants [TOURNAMENT_MAX_TEAMS] [TOURNAMENT_MAX_ENTRANTS];
CHAR_DATA *tournament_banned [TOURNAMENT_MAX_BANNED];

struct tournament_team tournament_teams [TOURNAMENT_MAX_TEAMS] =
{
        /* keyword,  name,      look flag,        colour code */
        { "red",     "Red",     "[Red Team]",     "{R" },
        { "blue",    "Blue",    "[Blue Team]",    "{B" },
        { "yellow",  "Yellow",  "[Yellow Team]",  "{Y" },
        { "green",   "Green",   "[Green Team]",   "{G" }
};


/*
 *  Local functions and data
 */

void tournament_info (CHAR_DATA *ch);
void enter_tournament (CHAR_DATA *ch, char *arg);
void withdraw_from_tournament (CHAR_DATA *ch);
void cancel_tournament (CHAR_DATA *ch);
void close_tournament (CHAR_DATA *ch);
void begin_tournament (CHAR_DATA *ch);
void ban_from_tournament (CHAR_DATA *ch, char *argument);
void set_tournament (CHAR_DATA *ch, char *argument);
void print_entrant_info (CHAR_DATA *ch);
void transfer_entrants ();

bool tournament_team_alive [TOURNAMENT_MAX_TEAMS];
int tournament_tag_order [TOURNAMENT_MAX_TEAMS] [TOURNAMENT_MAX_ENTRANTS];
char option_string [MAX_STRING_LENGTH];
CHAR_DATA *tournament_host;
const int COUNTDOWN = 4;

const char *tournament_types [3] =
{
        "open: individuals against everyone else",
        "team: teams of players versus other teams",
        "tag team: teams fighting with one team member at a time"
};


/*
 *  The meat on the bones
 */

void do_tournament (CHAR_DATA *ch, char *argument)
{
        char arg [MAX_INPUT_LENGTH];
        
        if (IS_NPC(ch))
                return;
        
        argument = one_argument(argument, arg);
        
        if (arg[0] == '\0' || !str_cmp(arg, "info"))
                tournament_info(ch);
        
        else if (!str_cmp(arg, "enter"))
                enter_tournament(ch, argument);
        
        else if (!str_cmp(arg, "withdraw"))
                withdraw_from_tournament(ch);
        
        else if (!str_cmp(arg, "cancel") && ch->level > LEVEL_HERO)
                cancel_tournament(ch);
        
        else if (!str_cmp(arg, "close") && ch->level > LEVEL_HERO)
                close_tournament(ch);
        
        else if (!str_cmp(arg, "start") && ch->level > LEVEL_HERO)
                begin_tournament(ch);
        
        else if (!str_cmp(arg, "ban") && ch->level > LEVEL_HERO)
                ban_from_tournament(ch, argument);
        
        else if (!str_cmp(arg, "set") && ch->level > LEVEL_HERO)
                set_tournament(ch, argument);
        
        else if (ch->level > LEVEL_HERO)
                send_to_char("Syntax: tournament [set|cancel|close|start|ban]\n\r", ch);
        
        else
                send_to_char("Syntax: tournament [info|enter|withdraw]\n\r", ch);
}


void tournament_init ()
{
        DESCRIPTOR_DATA *d;
        int i;
        
        tournament_status = TOURNAMENT_STATUS_NONE;
        tournament_type = TOURNAMENT_TYPE_OPEN;
        tournament_team_count = 0;
        tournament_options = 0;
        tournament_banned_count = 0;
        tournament_countdown = 0;
        tournament_host = NULL;
        
        for (i = 0; i < TOURNAMENT_MAX_TEAMS; i++)
        {
                tournament_entrant_count[i] = 0;
                tournament_team_alive[i] = FALSE;
        }
        
        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected == CON_PLAYING && d->character)
                        d->character->tournament_team = -1;
        }
        
        bot_entry_count = 0;
}


void tournament_info (CHAR_DATA *ch)
{
        char buf [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;
        
        if (tournament_status != TOURNAMENT_STATUS_NONE)
        {
                if (tournament_lower_level != tournament_upper_level)
                {
                        sprintf(buf, "%s has posted a tournament for those level %d-%d.\n\r"
                                "Options: %s.\n\rType: %s.\n\r\n\r",
                                tournament_host->name,
                                tournament_lower_level,
                                tournament_upper_level,
                                option_string,
                                tournament_types[tournament_type]);
                }
                else
                {
                        sprintf(buf, "%s has posted a tournament for those at level %d.\n\r"
                                "Options: %s.\n\rType: %s.\n\r\n\r",
                                tournament_host->name,
                                tournament_lower_level,
                                option_string,
                                tournament_types[tournament_type]);
                }
                        
                send_to_char(buf, ch);
                print_entrant_info(ch);
                        
                if (tournament_status == TOURNAMENT_STATUS_POSTED)
                {
                        if (tournament_type == TOURNAMENT_TYPE_TEAM)
                                send_to_char("\n\rType 'TOURNAMENT ENTER <team>' to enter the combat!\n\r", ch);
                        else
                                send_to_char("\n\rType 'TOURNAMENT ENTER' to enter the combat!\n\r", ch);
                }
                
                else if (tournament_status == TOURNAMENT_STATUS_CLOSED)
                        send_to_char("\n\rThis tournament is closed to further entries.\n\r", ch);
                
                else if (tournament_status == TOURNAMENT_STATUS_RUNNING)
                        send_to_char("\n\rThe tournament is currently underway!\n\r", ch);

                else
                        send_to_char("\n\rThe tournament has finished.\n\r", ch);
        }
                
        else
                send_to_char("No tournament has been posted.\n\r", ch);
}


void print_entrant_info (CHAR_DATA *ch)
{
        char buf [MAX_STRING_LENGTH];
        struct tournament_entrant next;
        int i, j;
        
        const char *status [] = 
        {
                "{GAlive{x",
                "{RDead{x",
                "{YDisqualified{x"      
        };

        if (tournament_team_count == 1)
        {
                if (!tournament_entrant_count[0]) 
                        send_to_char("No one has entered this tournament.\n\r", ch);

                else
                {
                        for (i = 0; i < tournament_entrant_count[0]; i++)
                        {
                                next = tournament_entrants[0][i];
                                sprintf(buf, " %2d   %s   %s\n\r",
                                        i+1,
                                        next.stats,
                                        (next.killer[0] != '\0') ? next.killer : status[next.status]);
                                send_to_char(buf, ch);
                        }
                }
        }
        else
        {
                for (i = 0; i < tournament_team_count; i++)
                {
                        if (tournament_status != TOURNAMENT_STATUS_POSTED
                            && !tournament_entrant_count[i])
                                continue;
                        
                        sprintf(buf, "%s %s%s team{x\n\r",
                                i ? "\n\r" : "",
                                tournament_teams[i].colour_code,
                                tournament_teams[i].name);
                        send_to_char(buf, ch);
                                                
                        if (!tournament_entrant_count[i])
                                send_to_char(" No one has entered this team\n\r", ch);
                        else
                        {
                                for (j = 0; j < tournament_entrant_count[i]; j++)
                                {
                                        next = tournament_entrants[i][j];
                                        sprintf(buf, " %2d   %s   %s\n\r",
                                                j+1,
                                                next.stats,
                                                (next.killer[0] != '\0') ? next.killer : status[next.status]);
                                        send_to_char(buf, ch);
                                }
                        }
                }
        }
}


void enter_tournament (CHAR_DATA *ch, char *arg)
{
        char buf [MAX_STRING_LENGTH];
        char team_name [MAX_INPUT_LENGTH];
        int i;
        int team;
        
        if (IS_NPC(ch))
                return;
        
        one_argument(arg, team_name);
        
        if (tournament_status == TOURNAMENT_STATUS_NONE)
        {
                send_to_char("No tournament has been posted!\n\r", ch);
                return; 
        }
        
        if (tournament_status != TOURNAMENT_STATUS_POSTED)
        {
                send_to_char("Sorry, you may not enter the tournament now.\n\r", ch);
                return;
        }
        
        if (ch->level < tournament_lower_level || ch->level > tournament_upper_level)
        {
                send_to_char("Sorry, the tournament is not open to your level.\n\r", ch);
                return;
        }
        
        if (is_entered_in_tournament(ch))
        {
                send_to_char("You are already entered!\n\r", ch);
                return;
        }

        for (i = 0; i < tournament_banned_count; i++)
        {
                if (ch == tournament_banned[i])
                {
                        send_to_char("Sorry, you are banned from this tournament.\n\r", ch);
                        return;
                }
        }
        
        if (tournament_type == TOURNAMENT_TYPE_TEAM
            || tournament_type == TOURNAMENT_TYPE_TAG)
        {
                if (team_name[0] == '\0')
                {
                        send_to_char("What team do you wish to enter?\n\r", ch);
                        return;
                }
                
                team = -1;
                
                for (i = 0; i < tournament_team_count; i++)
                {
                        if (!str_cmp(team_name, tournament_teams[i].keyword))
                        {
                                team = i;
                                break;
                        }
                }
                
                if (team < 0)
                {
                        strcpy(buf, "Valid teams are:");
                        
                        for (i = 0; i < tournament_team_count; i++)
                        {
                                strcat(buf, " ");
                                strcat(buf, tournament_teams[i].keyword);
                                
                                if (i < tournament_team_count - 1)
                                        strcat(buf, ",");
                        }
                        
                        strcat(buf, ".\n\r");
                        send_to_char(buf, ch);
                        return;
                }
        }
        else
                team = 0;
        
        if (tournament_entrant_count[team] == TOURNAMENT_MAX_ENTRANTS)
        {
                if (tournament_team_count == 1)
                        send_to_char("Sorry, no more people may enter this tournament.\n\r", ch);
                else
                        send_to_char("Sorry, no more people may enter that team.\n\r", ch);
                return;
        }
        
        sprintf(tournament_entrants[team][tournament_entrant_count[team]].stats,
                "{W%-15s{x [%s %3d]",
                ch->name,
                ch->sub_class ? sub_class_table[ch->sub_class].who_name
                    : class_table[ch->class].who_name,
                ch->level);
        
        tournament_entrants[team][tournament_entrant_count[team]].killer[0] = '\0';
        tournament_entrants[team][tournament_entrant_count[team]].status = TOURNAMENT_ENTRANT_ALIVE;
        tournament_entrants[team][tournament_entrant_count[team]].ch = ch;
        tournament_entrant_count[team]++;
        ch->tournament_team = team;
        
        send_to_char("You enter the tournament!\n\r", ch);
        
        if (tournament_team_count == 1)
                sprintf(buf, "%s has entered the tournament!", ch->name);
        else
                sprintf(buf, "%s has entered the tournament for the %s team!",
                        ch->name,
                        tournament_teams[team].name);
                
        tournament_message(buf, ch);
}


bool is_entered_in_tournament (CHAR_DATA *ch)
{
        int i, j;
        
        if (IS_NPC(ch) && ch->pIndexData->vnum != BOT_VNUM)
                return FALSE;
        
        for (i = 0; i < tournament_team_count; i++) 
        {
                for (j = 0; j < tournament_entrant_count[i]; j++)
                {
                        if (ch == tournament_entrants[i][j].ch)
                                return TRUE;
                }
        }

        return FALSE;
}



void withdraw_from_tournament (CHAR_DATA *ch)
{
        char buf [MAX_STRING_LENGTH];
        
        if (IS_NPC(ch))
                return;
        
        if (tournament_status == TOURNAMENT_STATUS_NONE) 
                send_to_char("But a tournament hasn't been posted!\n\r", ch);

        else if (tournament_status == TOURNAMENT_STATUS_FINISHED) 
                send_to_char("But the tournament has finished!\n\r", ch);

        else if (tournament_status == TOURNAMENT_STATUS_RUNNING) 
                send_to_char("Not while the tournament is running!\n\r", ch);

        else 
        {
                if (!is_entered_in_tournament(ch))
                {
                        send_to_char("But you're not entered in the first place!\n\r", ch);
                        return;
                }

                remove_from_tournament(ch);
                send_to_char("You withdraw from the tournament.\n\r", ch);
                sprintf(buf, "%s has withdrawn from the tournament.", ch->name);
                tournament_message(buf, ch);
        }
}


void cancel_tournament (CHAR_DATA *ch)
{
        if (IS_NPC(ch) || ch->level <= LEVEL_HERO)
                return;
        
        if (tournament_status == TOURNAMENT_STATUS_NONE) 
                send_to_char("But a tournament hasn't been posted!\n\r", ch);
        
        else if (ch->level < tournament_host->level)
                send_to_char("You aren't authorised to cancel this tournament.\n\r", ch);

        else 
        {
                if (tournament_status != TOURNAMENT_STATUS_FINISHED)
                        tournament_message("The tournament has been cancelled!", ch);
                send_to_char("Cancelling tournament.\n\r", ch);
                tournament_init();
        }
}


void close_tournament (CHAR_DATA *ch)
{
        if (IS_NPC(ch) || ch->level <= LEVEL_HERO)
                return;
        
        if (tournament_status == TOURNAMENT_STATUS_NONE)
                send_to_char("There is no tournament to close.\n\r", ch);
        
        else if (tournament_status == TOURNAMENT_STATUS_CLOSED)
                send_to_char("The tournament is already closed.\n\r", ch);
        
        else if (tournament_status == TOURNAMENT_STATUS_RUNNING)
                send_to_char("Too late now: the tournament is running.\n\r", ch);
        
        else if (tournament_status == TOURNAMENT_STATUS_FINISHED)
                send_to_char("Too late now: the tournament is finished.\n\r", ch);
        
        else 
        {
                send_to_char("Closing tournament to further entries.\n\r", ch);
                tournament_message("The tournament is now closed to further entries.", ch);
                tournament_status = TOURNAMENT_STATUS_CLOSED;
        }
}


void begin_tournament (CHAR_DATA *ch)
{
        int i, teams;
        
        if (IS_NPC(ch) || ch->level <= LEVEL_HERO)
                return;
        
        if (tournament_status == TOURNAMENT_STATUS_NONE)
        {
                send_to_char("No tournament has been set.\n\r", ch);
                return;
        }
        
        if (tournament_status == TOURNAMENT_STATUS_RUNNING)
        {
                send_to_char("The tournament is already running.\n\r", ch);
                return;
        }
        
        if (ch != tournament_host) 
        {
                send_to_char("You may not start the tournament.\n\r", ch);
                return;
        }

        if (tournament_team_count > 1)
        {
                teams = 0;
                
                for (i = 0; i < tournament_team_count; i++)
                {
                        if (tournament_entrant_count[i])
                        {
                                tournament_team_alive[i] = TRUE;
                                teams++;
                        }
                        else
                                tournament_team_alive[i] = FALSE;
                }
                                
                if (teams < 2)
                {
                        send_to_char("You need at least two teams before you can start.\n\r", ch);
                        return;
                }
        }
        else if (tournament_entrant_count[0] < 2)
        {
                send_to_char("You need at least two entrants before you can start.\n\r", ch);
                return;
        }
        
        send_to_char("Starting countdown...\n\r", ch);
        tournament_message("The tournament is about to begin!", ch);
        tournament_countdown = COUNTDOWN;
        tournament_status = TOURNAMENT_STATUS_RUNNING;
}


void ban_from_tournament (CHAR_DATA *ch, char *argument)
{
        char buf [MAX_STRING_LENGTH];
        char arg [MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        
        if (IS_NPC(ch) || ch->level <= LEVEL_HERO)
                return;

        if (tournament_status == TOURNAMENT_STATUS_NONE)
                send_to_char("There is no tournament to ban anyone from.\n\r", ch);
        
        else if (tournament_status != TOURNAMENT_STATUS_POSTED)
                send_to_char("Too late now!\n\r", ch);
        
        else 
        {
                one_argument(argument, arg);

                if (arg[0] == '\0')
                {
                        send_to_char("Ban who from this tournament?\n\r", ch);
                        return;
                }
                
                victim = get_char_world(ch, arg);
                
                if (!victim || !is_entered_in_tournament(victim))
                {
                        send_to_char("No one by that name in the tournament.\n\r", ch);
                        return;
                }
                
                if (tournament_banned_count == TOURNAMENT_MAX_BANNED)
                {
                        send_to_char("Can't ban anymore characters.\n\r", ch);
                        return;
                }
                
                send_to_char("Banning player from the tournament.\n\r", ch);
                send_to_char("You've been banned from this tournament!\n\r", victim);
                sprintf(buf, "%s has been removed from the tournament!", victim->name);
                tournament_message(buf, victim);

                remove_from_tournament(victim);
                tournament_banned[tournament_banned_count++] = victim;
        }
}


void set_tournament (CHAR_DATA *ch, char *argument)
{
        char arg_type [MAX_INPUT_LENGTH];
        char arg_teams [MAX_INPUT_LENGTH];
        char arg_lower [MAX_INPUT_LENGTH];
        char arg_upper [MAX_INPUT_LENGTH];
        char arg_option [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];
        int options = 0;
        int lower;
        int upper;
        int type;
        int team_count = 1;
        
        const char *syntax = 
                "Syntax: tournament set <open|team|tag> [number teams] <lower level> \\\n\r"
                "                       <upper level> [sameroom] [noflee] [nowimpy] \\\n\r"
                "                       [noquaff] [nopill] [nobrandish] [nosummon]\n\r";
        
        if (IS_NPC(ch) || ch->level <= LEVEL_HERO)
                return;
        
        if (tournament_status != TOURNAMENT_STATUS_NONE
            && tournament_status != TOURNAMENT_STATUS_FINISHED)
        {
                send_to_char("A tournament has already been posted!\n\r", ch);
                return;
        }
        
        if (argument[0] == '\0')
        {
                send_to_char(syntax, ch);
                return;
        }
        
        argument = one_argument(argument, arg_type);
        
        if (!str_cmp(arg_type, "open"))
                type = TOURNAMENT_TYPE_OPEN;

        else if (!str_cmp(arg_type, "team"))
                type = TOURNAMENT_TYPE_TEAM;
        
        else if (!str_cmp(arg_type, "tag"))
        {
                type = TOURNAMENT_TYPE_TAG;
                send_to_char("Tag team tournaments not quite ready yet!\n\r", ch);
                return;
        }
        else
        {
                send_to_char(syntax, ch);
                send_to_char("* Invalid tournament type.\n\r", ch);
                return;
        }
        
        if (type == TOURNAMENT_TYPE_TEAM
            || type == TOURNAMENT_TYPE_TAG)
        {
                argument = one_argument(argument, arg_teams);
                team_count = is_number(arg_teams) ? atoi(arg_teams) : -1;
                
                if (team_count < 2 || team_count > TOURNAMENT_MAX_TEAMS)
                {
                        send_to_char(syntax, ch);
                        sprintf(buf, "* Number of teams must be from 2-%d.\n\r",
                                TOURNAMENT_MAX_TEAMS);
                        send_to_char(buf, ch);
                        return;
                }
        }
                                
        argument = one_argument(argument, arg_lower);
        argument = one_argument(argument, arg_upper);
        lower = is_number(arg_lower) ? atoi(arg_lower) : -1;
        upper = is_number(arg_lower) ? atoi(arg_upper) : -1;
        
        if (lower < 10 || lower > 100) 
        {
                send_to_char(syntax, ch);
                send_to_char("* Lower level limit must be from 10-100.\n\r", ch);
                return;
        }
        
        if (upper < 10 || upper > 100) 
        {
                send_to_char(syntax, ch);
                send_to_char("* Upper level limit must be from 10-100.\n\r", ch);
                return;
        }
        
        if (lower > upper) 
        {
                int temp = upper;
                upper = lower;
                lower = temp;
        }
        
        while (1)
        {
                argument = one_argument(argument, arg_option);
                
                if (arg_option[0] == '\0')
                        break;
                
                else if (!str_cmp(arg_option, "noflee"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOFLEE);
                
                else if (!str_cmp(arg_option, "nowimpy"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOWIMPY);
                
                else if (!str_cmp(arg_option, "sameroom"))
                        SET_BIT(options, TOURNAMENT_OPTION_SAMEROOM);
                
                else if (!str_cmp(arg_option, "noquaff"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOQUAFF);
                
                else if (!str_cmp(arg_option, "nopill"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOPILL);
                
                else if (!str_cmp(arg_option, "nobrandish"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOBRANDISH);
                
                else if (!str_cmp(arg_option, "nosummon"))
                        SET_BIT(options, TOURNAMENT_OPTION_NOSUMMON);
                
                else 
                {
                        send_to_char(syntax, ch);
                        sprintf(buf, "* Unknown option: %s.\n\r", arg_option);
                        send_to_char(buf, ch);
                        return;
                }
        }
        
        sprintf(option_string,
                "entry:%s flee:%s wimpy:%s potions:%s pills:%s staves:%s summon:%s",
                (options & TOURNAMENT_OPTION_SAMEROOM) ? "same" : "random",
                (options & TOURNAMENT_OPTION_NOFLEE) ? "NO" : "yes",
                (options & TOURNAMENT_OPTION_NOWIMPY) ? "NO" : "yes",
                (options & TOURNAMENT_OPTION_NOQUAFF) ? "NO" : "yes",
                (options & TOURNAMENT_OPTION_NOPILL) ? "NO" : "yes",
                (options & TOURNAMENT_OPTION_NOBRANDISH) ? "NO" : "yes",
                (options & TOURNAMENT_OPTION_NOSUMMON) ? "NO" : "yes");
        
        tournament_init();
        tournament_type = type;
        tournament_team_count = team_count;
        tournament_options = options;
        tournament_host = ch;
        tournament_lower_level = lower;
        tournament_upper_level = upper;
        tournament_status = TOURNAMENT_STATUS_POSTED;
        
        sprintf(buf, "Tournament posted for levels %d-%d!\n\rOptions: %s.\n\rType: %s\n\r",
                lower, 
                upper,
                option_string,
                tournament_types[type]);
        send_to_char(buf, ch);
        
        sprintf(buf, "%s has posted a TOURNAMENT for those level %d-%d!",
                ch->name, lower, upper);
        do_info(ch, buf);
        
        sprintf(buf, "Options: %s", option_string);
        tournament_message(buf, ch);

        sprintf(buf, "Type: %s", tournament_types[type]);
        tournament_message(buf, ch);
}


void remove_from_tournament (CHAR_DATA *ch)
{
        int h, i, j;

        if (tournament_status == TOURNAMENT_STATUS_FINISHED)
                return;
        
        for (h = 0; h < tournament_team_count; h++)
        {
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (ch == tournament_entrants[h][i].ch) 
                        {
                                for (j = i; j < tournament_entrant_count[h]-1; j++) 
                                        tournament_entrants[h][j] = tournament_entrants[h][j+1];

                                tournament_entrant_count[h]--;
                                ch->tournament_team = -1;
                                return;
                        }
                }
        }
}


void do_tournament_countdown ()
{
        char buf [64];
        
        if (tournament_countdown < 1)
                return;
        
        if (tournament_countdown == 1)
        {
                tournament_message("The tournament has begun!", tournament_host);
                send_to_char("The tournament has begun!\n\r", tournament_host);
                tournament_countdown = 0;
                transfer_entrants();
        }
        
        else if (tournament_countdown-- <= COUNTDOWN)
        {
                sprintf(buf, ">> %d <<<<", tournament_countdown);
                tournament_message(buf, tournament_host);
                return;
        }
        
        check_for_tournament_winner();
}


void transfer_entrants ()
{
        CHAR_DATA *ch;
        ROOM_INDEX_DATA *assembly_room;
        ROOM_INDEX_DATA *start_room = NULL;
        int h, i, j;
        
        assembly_room = get_room_index(TOURNAMENT_ASSEMBLY_VNUM);
        
        for (i = 0; i < 1000; i++) 
        {
                start_room = get_room_index(ROOM_VNUM_ARENA + number_range(5, 35));
                if (start_room)
                        break;
        }
        
        if (!start_room) 
        {
                send_to_char("Couldn't find a transfer room!\n\r", tournament_host);
                tournament_init();
                return;
        }
        
        if (!assembly_room) 
        {
                tournament_init();
                send_to_char("Start room vnum invalid!\n\r", tournament_host);
        }

        for (h = 0; h < tournament_team_count; h++)
        {
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (tournament_type == TOURNAMENT_TYPE_TAG)
                                i = number_range(0, tournament_entrant_count[h]-1);
                        
                        ch = tournament_entrants[h][i].ch;
                        
                        if (!IS_NPC(ch))
                        {
                                if (!ch->desc || ch->desc->connected != CON_PLAYING)
                                {
                                        disqualify_tournament_entrant(ch, "not ready");
                                        continue;
                                }
                        
                                if (ch->in_room != assembly_room)
                                {
                                        disqualify_tournament_entrant(ch, "not at assembly point");
                                        continue;
                                }
                        
                                if (ch->fighting || ch->position != POS_STANDING)
                                {
                                        disqualify_tournament_entrant(ch, "not standing ready");
                                        continue;
                                }
                        
                                if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                                {
                                        disqualify_tournament_entrant(ch, "not in correct form");
                                        continue;
                                }
                        
                                ch->pcdata->tailing = 0;
                        }
                        
                        else if (ch->pIndexData->vnum != BOT_VNUM)
                        {
                                disqualify_tournament_entrant(ch, "bad bot");
                                continue;
                        }
                                                            
                        act("$n disappears into thin air!", ch, NULL, NULL, TO_ROOM);
                        char_from_room(ch);
                        char_to_room(ch, start_room);
                        act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
                        
                        if (ch->mount)
                        {
                                char_from_room(ch->mount);
                                char_to_room(ch->mount, start_room);
                        }
                
                        send_to_char("\n\r", ch);
                        do_look(ch, "");
                        
                        if (!IS_SET(tournament_options, TOURNAMENT_OPTION_SAMEROOM))
                        {
                                for (j = 0; j < 1000; j++)
                                {
                                        start_room = get_room_index(ROOM_VNUM_ARENA + number_range(5, 35));
                                        
                                        if (start_room)
                                                break;
                                }
                                
                                if (!start_room)
                                {
                                        send_to_char("Couldn't find a transfer room!\n\r", tournament_host);
                                        tournament_init();
                                        return;
                                }
                        }
                        
                        if (tournament_type == TOURNAMENT_TYPE_TAG)
                                break;
                }
        }
}


void disqualify_tournament_entrant (CHAR_DATA *ch, char *reason)
{
        char buf [MAX_STRING_LENGTH];
        int h, i, j;
        int alive = 0;
        
        for (h = 0; h < tournament_team_count; h++)
        {
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (tournament_entrants[h][i].ch == ch)
                        {
                                tournament_entrants[h][i].status = TOURNAMENT_ENTRANT_DISQUALIFIED;
                                
                                sprintf(buf, "%s has been disqualified: %s.",
                                        ch->name, reason ? reason : "(no reason)");
                                tournament_message(buf, ch);
                                
                                sprintf(buf, "{RYou have been disqualified from the tournament: %s.{x\n\r",
                                        reason ? reason : "(no reason)");
                                send_to_char(buf, ch);
                                
                                for (j = 0; j < tournament_entrant_count[h]; j++)
                                {
                                        if (tournament_entrants[h][j].status == TOURNAMENT_ENTRANT_ALIVE)
                                                alive++;
                                }
                                
                                if (!alive)
                                        tournament_team_alive[h] = FALSE;
                                
                                return;
                        }
                }
                
        }
}

        
void check_for_tournament_winner ()
{
        char buf [MAX_STRING_LENGTH];
        int i, j;
        int alive = 0;
        int teams_alive = 0;
        int winning_team = -1;
        CHAR_DATA *ch = NULL;
        
        if (tournament_status != TOURNAMENT_STATUS_RUNNING)
                return;

        if (tournament_team_count == 1)
        {
                for (i = 0; i < tournament_entrant_count[0]; i++)
                {
                        if (tournament_entrants[0][i].status == TOURNAMENT_ENTRANT_ALIVE) 
                        {
                                alive++;
                                ch = tournament_entrants[0][i].ch;
                        }
                }       
                
                if (!alive) 
                {
                        tournament_message("Ack!  NO ONE has won this tournament!", tournament_host);
                        send_to_char("No one wins the tournament.\n\r", tournament_host);
                        tournament_status = TOURNAMENT_STATUS_FINISHED;
                        bot_entry_count = 0;
                        return;
                }
                
                if (alive == 1)
                {
                        sprintf(buf, ">>> The tournament has been won...  %s is victorious! <<<\n\r", 
                                ch->name);
                        tournament_message(buf, ch);
                        send_to_char("\n\r{Y>>> {WCongratulations!  {CYou have won the tournament! {Y<<<{x\n\r\n\r", ch);
                        tournament_status = TOURNAMENT_STATUS_FINISHED;
                        bot_entry_count = 0;
                        ch->tournament_team = -1;
                }
        }
        else
        {
                for (i = 0; i < tournament_team_count; i++)
                {
                        for (j = 0; j < tournament_entrant_count[i]; j++)
                        {
                                if (tournament_entrants[i][j].status == TOURNAMENT_ENTRANT_ALIVE)
                                {
                                        teams_alive++;
                                        winning_team = i;
                                        break;
                                }
                        }
                }
                
                if (!teams_alive)
                {
                        tournament_message("Ack!  NO ONE has won this tournament!", tournament_host);
                        send_to_char("No one wins the tournament.\n\r", tournament_host);
                        tournament_status = TOURNAMENT_STATUS_FINISHED;
                        bot_entry_count = 0;
                        return;
                }
                
                if (teams_alive == 1)
                {
                        sprintf(buf, ">>> The tournament has been won...  The %s team wins! <<<", 
                                tournament_teams[winning_team].name);
                        tournament_message(buf, tournament_host);
                        
                        sprintf(buf, "The tournament has been won by the %s team!\n\r",
                                tournament_teams[winning_team].name);
                        send_to_char(buf, tournament_host);
                        
                        for (i = 0; i < tournament_entrant_count[winning_team]; i++)
                        {
                                send_to_char("\n\r{Y>>> {WCongratulations!  {CYour team has won the tournament! {Y<<<{x\n\r\n\r", 
                                             tournament_entrants[winning_team][i].ch);
                                tournament_entrants[winning_team][i].ch->tournament_team = -1;
                        }
                        
                        tournament_status = TOURNAMENT_STATUS_FINISHED;
                        bot_entry_count = 0;
                }
        }
}


bool is_tournament_death (CHAR_DATA *victim, CHAR_DATA *killer)
{
        int h, i;
        
        if (tournament_status != TOURNAMENT_STATUS_RUNNING)
                return FALSE;
        
        if ((IS_NPC(victim) && victim->pIndexData->vnum != BOT_VNUM)
             || (IS_NPC(killer) && killer->pIndexData->vnum != BOT_VNUM))
                return FALSE;
        
        for (h = 0; h < tournament_team_count; h++)
        {
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (victim == tournament_entrants[h][i].ch
                            && tournament_entrants[h][i].status == TOURNAMENT_ENTRANT_ALIVE)
                            /* && IS_SET(killer->in_room->room_flags, ROOM_PLAYER_KILLER)) */
                        {
                                return TRUE;
                        }
                }
        }
        
        return FALSE;
}


void tournament_death (CHAR_DATA *victim, CHAR_DATA *killer)
{
        char buf [MAX_STRING_LENGTH];
        int h, i;
        int alive [TOURNAMENT_MAX_TEAMS];
        
        for (h = 0; h < tournament_team_count; h++)
        {
                alive[h] = 0;
                
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (tournament_entrants[h][i].ch == victim)
                        {
                                tournament_entrants[h][i].status = TOURNAMENT_ENTRANT_DEAD;
                                sprintf(buf, "{RKilled by %s{x", killer->name);
                                strcpy(tournament_entrants[h][i].killer, buf);
                        }
                        
                        else if (tournament_entrants[h][i].status == TOURNAMENT_ENTRANT_ALIVE)
                        {
                                alive[h]++;
                        }
                }
                
                if (!alive[h] 
                    && tournament_team_count > 1
                    && tournament_team_alive[h])
                {
                        sprintf(buf, "The %s team has been knocked out of the tournament!",
                                tournament_teams[h].name);
                        tournament_message(buf, victim);
                        tournament_team_alive[h] = FALSE;
                }
        }
        
        if (tournament_team_count == 1)
        {
                if (alive[0] > 1)
                {
                        sprintf(buf, "%d contestants remain in the tournament!", alive[0]);
                        tournament_message(buf, victim);
                }
        }
                
        check_for_tournament_winner();
}


bool is_still_alive_in_tournament (CHAR_DATA *ch)
{
        int h, i;
        
        if (IS_NPC(ch) && ch->pIndexData->vnum != BOT_VNUM)
                return FALSE;
        
        for (h = 0; h < tournament_team_count; h++)
        {
                for (i = 0; i < tournament_entrant_count[h]; i++)
                {
                        if (ch == tournament_entrants[h][i].ch)
                        {
                                if (tournament_entrants[h][i].status == TOURNAMENT_ENTRANT_ALIVE)
                                        return TRUE;
                                else
                                        return FALSE;
                        }
                }
        }
        
        return FALSE;
}

        
bool tournament_action_illegal (CHAR_DATA *ch, int flag)
{
        if (tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_still_alive_in_tournament(ch)
            && IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER)
            && IS_SET(tournament_options, flag))
                return TRUE;
        
        return FALSE;
}


void arena_commentary (char *text, CHAR_DATA *ch, CHAR_DATA *victim)
{
        DESCRIPTOR_DATA *d;
        char buf [MAX_STRING_LENGTH];
        char buf2 [MAX_STRING_LENGTH];
        char *write;
        const char *read, *token;
        
        if (!ch
            || !IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER)
            || text[0] == '\0')
                return;
        
        /*
         *  Modified version of act to include '$n' and '$N' tokens
         */
        write = buf;
        read = text;
        
        while (*read != '\0')
        {
                if (*read != '$')
                {
                        *write++ = *read++;
                        continue;
                }
                
                ++read;
                
                if (*read == 'n')
                {
                        if (IS_NPC(ch))
                                token = ch->short_descr;
                        else
                                token = ch->name;
                }
                
                else if (*read == 'N' && victim)
                {
                        if (IS_NPC(victim))
                                token = victim->short_descr;
                        else
                                token = victim->name;
                }
                
                else
                        continue;
                
                ++read;
                
                while ((*write = *token) != '\0')
                        ++write, ++token;
        }

        *write = '\0';

        add_to_public_review_buffer(NULL, REVIEW_ARENA, buf);
        
        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected != CON_PLAYING
                    || !d->character
                    || d->character->silent_mode
                    || IS_SET(d->character->deaf, CHANNEL_ARENA)
                    || IS_SET(d->character->in_room->room_flags, ROOM_PLAYER_KILLER)
                    || d->character == ch
                    || d->character == victim)
                {
                        continue;
                }

                sprintf(buf2, "%s<Arena> %s$R",
                        color_table_8bit[get_colour_index_by_code(d->character->colors[COLOR_ARENA])].act_code,
                        buf);
                act(buf2, d->character, NULL, NULL, TO_CHAR);
        }
}

                              
                    
