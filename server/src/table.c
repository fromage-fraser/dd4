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

#if !defined( macintosh )
extern  int     _filbuf         args( (FILE *) );
#endif

#if !defined( ultrix )
#include <memory.h>
#endif


/*
 * Fame table - Geoff
 */

void fame_table_swap (int one, int two) 
{
        char buf [MAX_STRING_LENGTH];
        int ibuf;
        
        strncpy(buf, fame_table[one].name, sizeof(buf));
        ibuf = fame_table[one].fame;
        
        strncpy(fame_table[one].name, fame_table[two].name, sizeof(fame_table[one].name));
        fame_table[one].fame = fame_table[two].fame;
        
        strncpy(fame_table[two].name, buf, sizeof(fame_table[two].name));
        fame_table[two].fame = ibuf;
}


/*
 * check the table for a change when PC's fame modified 
 */
void check_fame_table (CHAR_DATA *ch) 
{
        int prev = 0;
        int in = 0;
        int i = 0;
        char buf[ MAX_STRING_LENGTH ];
        char buf1[ MAX_STRING_LENGTH ];
        
        if (ch->level > LEVEL_HERO)
                return;

        for (i = 0; i < FAME_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp(fame_table[i].name, ch->name)) 
                {
                        prev = i;
                        in = 1;
                }
        }
        
        if (!in && (ch->pcdata->fame < fame_table[FAME_TABLE_LENGTH - 1].fame))
                return;

        if (!in) 
        {
                for (i = FAME_TABLE_LENGTH - 1; (fame_table[i - 1].fame <= ch->pcdata->fame); i--)
                {
                        strncpy(fame_table[i].name, fame_table[i - 1].name, sizeof(fame_table[i].name));
                        fame_table[i].fame = fame_table[i - 1].fame;

                        /*
                         * Shade 11.3.22
                         *
                         * Something about this has changed - the table appears to copy correctly but i gets decremented one time too many meaning players are promoted to the 0th position
                         * 
                         * Pretty sure it'll also mess up the fame table and character "1" will be kind of everywhere
                         */

                        if (i == 1) 
                        {
                                /* Might just log this for a bit; just in case */
                                sprintf(buf, "Promoting %s to 1st position in fame table", ch->name);
                                log_string(buf);  
                                i = 0;
                                break;
                        }
                }
                                
                strncpy(fame_table[i].name, ch->name, sizeof(fame_table[i].name));
                fame_table[i].fame = ch->pcdata->fame;
                
                /* 
                 * Umgook - added so messages only print if people added 
                 * to the printable length
                 */
                if (i < FAME_TABLE_LENGTH_PRINT) 
                {
                     
                        sprintf(buf, "You are now ranked %d%s amongst famous mortals.\n\r", 
                                i + 1,
                                number_suffix(i + 1));
                        send_to_char( buf, ch );
                        
                        sprintf(buf, "%s is now ranked number %d%s amongst famous mortals.",
                                ch->name, 
                                i + 1,
                                number_suffix(i + 1));
                        do_info(ch, buf);
                }
        }
        /* 
         * internal movement within table 
         */
        else
        {
                i = prev;

                if (!prev && ch->pcdata->fame > fame_table[1].fame)
                {
                        fame_table[prev].fame = ch->pcdata->fame;
                        save_fame_table();
                        update_legend_table(ch);
                        return;
                }
                else if (!prev)
                {
                        fame_table_swap(0, 1);
                        prev = 1;
                        i = 0;
                }
                
                if (prev == FAME_TABLE_LENGTH - 1
                    && ch->pcdata->fame < fame_table[FAME_TABLE_LENGTH - 2].fame)
                {
                        fame_table[prev].fame = ch->pcdata->fame;
                        save_fame_table();
                        update_legend_table(ch);
                        return;
                }
                else if (prev == FAME_TABLE_LENGTH - 1) 
                {
                        fame_table_swap(FAME_TABLE_LENGTH - 1, FAME_TABLE_LENGTH - 2);
                        prev = FAME_TABLE_LENGTH - 2;
                        i = FAME_TABLE_LENGTH;
                }
                
                while (prev && prev != FAME_TABLE_LENGTH - 1)
                {
                        if (ch->pcdata->fame > fame_table[prev - 1].fame) 
                        {
                                fame_table_swap(prev, prev - 1);
                                prev--;
                        }
                        else if (ch->pcdata->fame < fame_table[prev + 1].fame) 
                        {
                                fame_table_swap(prev, prev + 1);
                                prev++;
                        }
                        else 
                                break;
                }
                
                fame_table[prev].fame = ch->pcdata->fame;
                
                if (ch->desc->connected == CON_PLAYING
                    && prev < FAME_TABLE_LENGTH_PRINT && ((i && i != prev) || !i)) 
                {

                        sprintf(buf, "You are now ranked %d%s amongst famous mortals.\n\r", 
                                prev + 1,
                                number_suffix(prev+1));
                        send_to_char( buf, ch );
                        
                        sprintf(buf, "%s has been", ch->name);

                        if (prev < i) 
                                sprintf(buf1, " promoted ");
                        else
                                sprintf(buf1, " demoted ");

                        strcat(buf, buf1);
                        sprintf(buf1, "to number %d amongst famous mortals.\n\r", 
                                prev + 1);
                        strcat(buf, buf1);
                        
                        do_info(ch, buf);

                }
        }
        
        save_fame_table();
        update_legend_table(ch);
}       


void print_fame_table ( CHAR_DATA *ch, char *argument ) 
{
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        int i;
        char* colour;
        
        sprintf(buf, "Gezhp appears before you and speaks:\n\r"
                "\"The following are recognised as being\n\r"
                "amongst the most famous mortals...\"\n\r\n\r");
        
        for (i = 0; i < FAME_TABLE_LENGTH_PRINT; i++) 
        {
                if (i == 0)     colour = "{W";
                else if (i < 2) colour = "{Y";
                else if (i < 4) colour = "{G";
                else if (i < 7) colour = "{C";
                else if (i <12) colour = "{c";
                else            colour = "{w";
                
                if (fame_table[i].fame != 0) 
                {
                        sprintf(tmpbuf, "  %s%2d%s    %-12s %7d{x\n\r",
                                colour, 
                                i+1,
                                number_suffix (i+1),
                                fame_table[i].name, 
                                fame_table[i].fame);
                        strcat(buf, tmpbuf);
                }
        }
        
        send_to_char(buf, ch);
}


/*
 * Infamy table - Shade Apr 2022, copied from Fame
 */

void infamy_table_swap (int one, int two) 
{
        char buf [MAX_STRING_LENGTH];
        char buf1 [MAX_STRING_LENGTH];
        int ibuf;
        int vbuf;
        
        strncpy(buf, infamy_table[one].name, sizeof(buf));
        strncpy(buf1,infamy_table[one].loc, sizeof(buf1));
        ibuf = infamy_table[one].kills;
        vbuf = infamy_table[one].vnum;

        strncpy(infamy_table[one].name, infamy_table[two].name, sizeof(infamy_table[one].name));
        strncpy(infamy_table[one].loc, infamy_table[two].loc, sizeof(infamy_table[one].loc));
        infamy_table[one].kills = infamy_table[two].kills;
        infamy_table[one].vnum = infamy_table[two].vnum;

        strncpy(infamy_table[two].name, buf, sizeof(infamy_table[two].name));
        strncpy(infamy_table[two].loc, buf1, sizeof(infamy_table[two].loc));
        infamy_table[two].kills = ibuf;
        infamy_table[two].vnum = vbuf;
}


/*
 * check the table for a change when a PC is killed
 */

void check_infamy_table (CHAR_DATA *mob) 
{
        int prev = 0;
        int in = 0;
        int i = 0;
        int kills = 0;
        char buf[ MAX_STRING_LENGTH ];
        char buf1[ MAX_STRING_LENGTH ];
        
        /*
         * Are they in the table
         */

        for (i = 0; i < INFAMY_TABLE_LENGTH; i++) 
        {
                if (infamy_table[i].vnum == mob->pIndexData->vnum ) 
                {
                        prev = i;
                        in = 1;
                        kills = infamy_table[i].kills + 1;
                }
        }
        
        if (!in && (1 < infamy_table[INFAMY_TABLE_LENGTH - 1].kills))
                return;

        /* if they aren't in the table, this is their first kill */
        if (!in) 
        {
                for (i = INFAMY_TABLE_LENGTH - 1; (infamy_table[i - 1].kills <= 1); i--)
                {
                        strncpy(infamy_table[i].name, infamy_table[i - 1].name, sizeof(infamy_table[i].name));
                        strncpy(infamy_table[i].loc, infamy_table[i - 1].loc, sizeof(infamy_table[i].loc));
                        infamy_table[i].kills = infamy_table[i - 1].kills;
                        infamy_table[i].vnum = infamy_table[i - 1].vnum;

                        /*
                         * Shade 11.3.22
                         *
                         * Something about this has changed - the table appears to copy correctly but i gets decremented one time too many meaning players are promoted to the 0th position
                         * 
                         * Pretty sure it'll also mess up the fame table and character "1" will be kind of everywhere
                         */

                        if (i == 1) 
                        {
                                /* Might just log this for a bit; just in case */
                                sprintf(buf, "Promoting %s to 1st position in infamy table", mob->name);
                                log_string(buf);  
                                i = 0;
                                break;
                        }
                }

                strncpy(infamy_table[i].name, mob->short_descr, sizeof(infamy_table[i].name));
                strncpy(infamy_table[i].name, capitalize_initial(infamy_table[i].name), sizeof(infamy_table[i].name));
                strncpy(infamy_table[i].loc, mob->in_room->area->name, sizeof(infamy_table[i].loc));
                infamy_table[i].kills = 1;
                infamy_table[i].vnum = mob->pIndexData->vnum;
                
                if (i < INFAMY_TABLE_LENGTH_PRINT) 
                {
                                             
                        sprintf(buf, "%s is now ranked %d%s amongst infamous monsters!",
                                capitalize_initial(mob->short_descr), 
                                i + 1,
                                number_suffix(i + 1));
                        do_info(mob, buf);
                }
        }
        /* 
         * internal movement within table 
         */
        else
        {
                i = prev;

                /*
                 * Already top of the table
                 */        

                if (!prev)
                {
                        infamy_table[prev].kills = kills;
                        save_infamy_table();
                        return;
                }
                
                /* 
                 * Already bottom of the table
                 */
                if (prev == INFAMY_TABLE_LENGTH - 1 && kills < infamy_table[INFAMY_TABLE_LENGTH - 2].kills)
                {
                        infamy_table[prev].kills = kills;
                        save_infamy_table();
                        return;
                }

                infamy_table[prev].kills = kills;

                if (prev == INFAMY_TABLE_LENGTH - 1) 
                {
                        infamy_table_swap(INFAMY_TABLE_LENGTH - 1, INFAMY_TABLE_LENGTH - 2);
                        prev = INFAMY_TABLE_LENGTH - 2;
                        i = INFAMY_TABLE_LENGTH;
                }
                
                while (prev && prev != INFAMY_TABLE_LENGTH - 1)
                {
                        if (kills > infamy_table[prev - 1].kills) 
                        {
                                infamy_table_swap(prev, prev - 1);
                                prev--;
                        }
                        else if (kills < infamy_table[prev + 1].kills) 
                        {
                                infamy_table_swap(prev, prev + 1);
                                prev++;
                        }
                        else 
                                break;
                }
                
                if (prev < INFAMY_TABLE_LENGTH_PRINT && ((i && i != prev) || !i)) 
                {
                        
                        sprintf(buf, "%s has been", capitalize_initial(mob->short_descr));
                        sprintf(buf1, " promoted "); /* Can't lose kills so this is strictly up */
                        strcat(buf, buf1);
                        sprintf(buf1, "to number %d amongst infamous monsters!\n\r", prev + 1);
                        strcat(buf, buf1);                        
                        do_info(mob, buf);
                }
        }
        
        save_infamy_table();
}       


void print_infamy_table ( CHAR_DATA *ch, char *argument ) 
{
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        int i;
        char* colour;
        
        sprintf(buf, "Shade appears before you and speaks:\n\r"
                "\"The following are recognised as being\n\r"
                "amongst the most infamous monsters...\"\n\r\n\r");
        
        for (i = 0; i < INFAMY_TABLE_LENGTH_PRINT; i++) 
        {
                if (i == 0)     colour = "{W";
                else if (i < 2) colour = "{Y";
                else if (i < 4) colour = "{G";
                else if (i < 7) colour = "{C";
                else if (i <12) colour = "{c";
                else            colour = "{w";
                
                if (infamy_table[i].kills != 0) 
                {
                        sprintf(tmpbuf, "  %s%2d%s    %-30s %-30s %2d{x\n\r",
                                colour, 
                                i+1,
                                number_suffix (i+1),
                                infamy_table[i].name, 
                                infamy_table[i].loc,
                                infamy_table[i].kills);
                        strcat(buf, tmpbuf);
                }
        }
        
        send_to_char(buf, ch);
}


/*
 * Pkill table - Geoff
 */

void clan_pkill_table_swap (int clan, int one, int two) 
{
        char buf[15];
        int ibuf;
        
        strncpy(buf, clan_pkill_table[clan][one].name, sizeof(buf));
        ibuf = clan_pkill_table[clan][one].pkills;
        
        strncpy(clan_pkill_table[clan][one].name, clan_pkill_table[clan][two].name,
                sizeof(clan_pkill_table[clan][one].name));
        clan_pkill_table[clan][one].pkills = clan_pkill_table[clan][two].pkills;
        
        strncpy(clan_pkill_table[clan][two].name, buf, sizeof(clan_pkill_table[clan][two].name));
        clan_pkill_table[clan][two].pkills = ibuf;
}


void check_clan_pkill_table (CHAR_DATA *ch) 
{
        int prev = 0;
        int in = 0;
        int i;
        int clan = ch->clan;
        char buf[ MAX_STRING_LENGTH ];
        char buf1[ MAX_STRING_LENGTH ];

        if (!clan)
                return;
        
        for (i = 0; i < CLAN_PKILL_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp(clan_pkill_table[clan][i].name, ch->name)) 
                {
                        prev = i;
                        in = 1;
                }
        }
        
        if (!in && (clan_pkill_table[clan][CLAN_PKILL_TABLE_LENGTH - 1].pkills != 0))
                clan_pkill_table[clan][CLAN_PKILL_TABLE_LENGTH - 1].pkills = 0;
        
        if (!in) 
        {
                for (i = CLAN_PKILL_TABLE_LENGTH - 1;  
                     (clan_pkill_table[clan][i - 1].pkills <= 1 && i != 0);
                     i--)
                {
                        strncpy(clan_pkill_table[clan][i].name, clan_pkill_table[clan][i - 1].name,
                                sizeof(clan_pkill_table[clan][i].name));
                        clan_pkill_table[clan][i].pkills = clan_pkill_table[clan][i - 1].pkills;
                }
                
                strncpy(clan_pkill_table[clan][i].name, ch->name, sizeof(clan_pkill_table[clan][i].name));
                clan_pkill_table[clan][i].pkills = 1;
                
                strncpy(buf, "Congratulations, you have now entered the pkiller ranks for your clan.\n\r", sizeof(buf));
                send_to_char( buf, ch );
                sprintf(buf, "%s has made their first pkill for clan %s.\n\r",
                        ch->name, clan_table[clan].who_name);
                do_info(ch, buf);
        }
        else
        {
                i = prev;
                
                if (!prev)
                {
                        clan_pkill_table[clan][0].pkills++;
                        save_pkill_table(clan);
                        return;
                }
                
                if ((prev == CLAN_PKILL_TABLE_LENGTH - 1) 
                    && (clan_pkill_table[clan][prev].pkills 
                        < clan_pkill_table[clan][CLAN_PKILL_TABLE_LENGTH - 2].pkills)) 
                {
                        clan_pkill_table[clan][prev].pkills++;
                        save_pkill_table(clan);
                        return;
                }
                else if (prev == CLAN_PKILL_TABLE_LENGTH - 1) 
                {
                        clan_pkill_table_swap(clan, CLAN_PKILL_TABLE_LENGTH - 1, 
                                              CLAN_PKILL_TABLE_LENGTH - 2);
                        prev = CLAN_PKILL_TABLE_LENGTH - 2;
                        i = CLAN_PKILL_TABLE_LENGTH;
                }
                
                while (prev && prev != CLAN_PKILL_TABLE_LENGTH - 1)
                {
                        if (clan_pkill_table[clan][prev].pkills + 1 
                            > clan_pkill_table[clan][prev - 1].pkills) 
                        {
                                clan_pkill_table_swap(clan, prev, prev - 1);
                                prev--;
                        }
                        else if (clan_pkill_table[clan][prev].pkills + 1
                                 <= clan_pkill_table[clan][prev + 1].pkills) 
                        {
                                clan_pkill_table_swap(clan, prev, prev + 1);
                                prev++;
                        }
                        else 
                                break;
                }
                
                clan_pkill_table[clan][prev].pkills++;
                
                if (i != prev) 
                {
                        sprintf(buf, "Congratulations, you are now ranked %d%s amongst pkillers for your clan.\n\r",
                                prev + 1,
                                number_suffix(prev+1));
                        send_to_char( buf, ch );
                        
                        sprintf(buf, "%s has been", ch->name);
                        if (prev < i) 
                                sprintf(buf1, " promoted ");
                        else
                                sprintf(buf1, " demoted ");
                        strcat(buf, buf1);
                        sprintf(buf1, "to number %d amongst pkillers for clan %s.\n\r",
                                prev + 1, 
                                clan_table[clan].who_name);
                        strcat(buf, buf1);
                        do_info(ch, buf);
                }
        }

        save_pkill_table(clan);
}       


void strip_clan_pkill_table (CHAR_DATA *ch) 
{
        return;
        
        /*
         * This method of stripping fouls up the table cause the ex-clan-member
         * is not stripped: kills are just set to 0.  This ruins the formatting of
         * the new 'pkillers <clan>' table.
         * 
         * Gezhp, Dec 2000
         */
        
        /*
        int prev = 0;
        int in = 0;
        int clan = ch->clan;
        int i;
        
        if (!clan)
                return;

        for (i = 0; i < CLAN_PKILL_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp(clan_pkill_table[clan][i].name, ch->name)) 
                {
                        prev = i;
                        in = 1;
                }
        }
        
        if (in) 
        {
                for (i = prev; i < (CLAN_PKILL_TABLE_LENGTH - 1); i++) 
                {
                        sprintf(clan_pkill_table[clan][i].name,
                                clan_pkill_table[clan][i + 1].name);
                        clan_pkill_table[clan][i].pkills = clan_pkill_table[clan][i + 1].pkills;
                }
                clan_pkill_table[clan][i + 1].pkills = 0;
        }
        
        save_pkill_table(clan);
        */
}


/*
 * Vanquished table - Geoff
 */

void clan_vanq_table_swap (int clan, int one, int two) 
{
        char buf[15];
        int ibuf;
        
        strncpy(buf, clan_vanq_table[clan][one].name, sizeof(buf));
        ibuf = clan_vanq_table[clan][one].killed;
        
        strncpy(clan_vanq_table[clan][one].name, clan_vanq_table[clan][two].name, sizeof(clan_vanq_table[clan][one].name));
        clan_vanq_table[clan][one].killed = clan_vanq_table[clan][two].killed;
        
        strncpy(clan_vanq_table[clan][two].name, buf, sizeof(clan_vanq_table[clan][two].name));
        clan_vanq_table[clan][two].killed = ibuf;
}


void check_clan_vanq_table ( CHAR_DATA *ch, CHAR_DATA *victim ) 
{
        int prev = 0;
        int in = 0;
        int clan = ch->clan;
        int i;
        char buf[ MAX_STRING_LENGTH ];
        char buf1[ MAX_STRING_LENGTH ];
        
        if (!clan)
                return;
        
        for (i = 0; i < CLAN_VANQ_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp(clan_vanq_table[clan][i].name, victim->name)) 
                {
                        prev = i;
                        in = 1;
                }
        }

        if (!in && (clan_vanq_table[clan][CLAN_VANQ_TABLE_LENGTH - 1].killed != 0))
                clan_vanq_table[clan][CLAN_VANQ_TABLE_LENGTH - 1].killed = 0;
        
        if (!in) 
        {
                for (i = CLAN_VANQ_TABLE_LENGTH - 1;  
                     (clan_vanq_table[clan][i - 1].killed <= 1 && i != 0);
                     i--)
                {
                        strncpy(clan_vanq_table[clan][i].name, clan_vanq_table[clan][i - 1].name, sizeof(clan_vanq_table[clan][i].name));
                        clan_vanq_table[clan][i].killed = clan_vanq_table[clan][i - 1].killed;
                }
                strncpy(clan_vanq_table[clan][i].name, victim->name, sizeof(clan_vanq_table[clan][i].name));
                
                clan_vanq_table[clan][i].killed = 1;
                
                sprintf(buf, "Congratulations, you have now entered the ranks of the vanquished for clan %s.\n\r", clan_table[clan].who_name);
                send_to_char( buf, victim );
                sprintf(buf, "%s has been added to the ranks of the vanquished.", victim->name);
                do_clantalk(ch, buf);
        }
        else
        {
                i = prev;

                if (!prev)
                {
                        clan_vanq_table[clan][0].killed++;
                        save_vanq_table(clan);
                        return;
                }
      
                if (prev == CLAN_VANQ_TABLE_LENGTH - 1
                    && (clan_vanq_table[clan][prev].killed 
                        < clan_vanq_table[clan][CLAN_VANQ_TABLE_LENGTH - 2].killed)) 
                {
                        clan_vanq_table[clan][prev].killed++;
                        save_vanq_table(clan);
                        return;
                }
                else if (prev == CLAN_VANQ_TABLE_LENGTH - 1) 
                {
                        clan_vanq_table_swap(clan, CLAN_VANQ_TABLE_LENGTH - 1, 
                                             CLAN_VANQ_TABLE_LENGTH - 2);
                        prev = CLAN_VANQ_TABLE_LENGTH - 2;
                        i = CLAN_VANQ_TABLE_LENGTH;
                }
                
                while (prev && prev != CLAN_VANQ_TABLE_LENGTH - 1)
                {
                        if (clan_vanq_table[clan][prev].killed + 1 
                            > clan_vanq_table[clan][prev - 1].killed) 
                        {
                                clan_vanq_table_swap(clan, prev, prev - 1);
                                prev--;
                        }
                        else if (clan_vanq_table[clan][prev].killed + 1
                                 <= clan_vanq_table[clan][prev + 1].killed) 
                        {
                                clan_vanq_table_swap(clan, prev, prev + 1);
                                prev++;
                        }
                        else 
                                break;
                }
                
                clan_vanq_table[clan][prev].killed++;
                
                if (i != prev) 
                {
                        sprintf(buf, "Congratulations, you are now ranked %d%s amongst vanquished for clan %s.\n\r",
                                prev + 1,
                                number_suffix(prev+1),
                                clan_table[clan].who_name);
                        send_to_char( buf, victim );
                        
                        sprintf(buf, "%s has been", victim->name);
                        if (prev < i) 
                                sprintf(buf1, " promoted ");
                        else
                                sprintf(buf1, " demoted ");
                        strcat(buf, buf1);
                        sprintf(buf1, "to number %d amongst the vanquished.", prev + 1);
                        strcat(buf, buf1);
                        
                        do_clantalk(ch, buf);
                }
        }
        
        save_vanq_table(clan);
}


void print_clan_vanq_table (CHAR_DATA *ch, char *argument) 
{
        char buf [MAX_STRING_LENGTH];
        char tmpbuf [MAX_STRING_LENGTH];
        int i;
        char* colour;
        int clan = ch->clan;
        
        if (IS_NPC(ch) || !clan)
        {
                send_to_char("You are not in a clan.\n\r", ch);
                return;
        }
        
        strcpy(buf, "Those most frequently vanquished:\n\r\n\r");
        
        for (i = 0; i < FAME_TABLE_LENGTH_PRINT; i++) 
        {
                if (i == 0)     colour = "{W";
                else if (i < 2) colour = "{Y";
                else if (i < 4) colour = "{G";
                else if (i < 7) colour = "{C";
                else if (i <12) colour = "{c";
                else            colour = "{w";
                
                if (clan_vanq_table[clan][i].killed < 1)
                        break;

                sprintf(tmpbuf, "  %s%2d%s    %-12s %4d{x\n\r",
                        colour, 
                        i+1,
                        number_suffix (i+1),
                        clan_vanq_table[clan][i].name, 
                        clan_vanq_table[clan][i].killed);
                strcat(buf, tmpbuf);
        }
        
        send_to_char(buf, ch);
}


void OLD_print_clan_vanq_table ( CHAR_DATA *ch, char *argument ) 
{
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        int i;
        int clan = ch->clan;
        
        if (!clan)
                return;
        
        sprintf(buf,"\n\rShade appears before you and speaks:\n\r\"The following are "
                "recognised as those most frequently vanquished by %s...\"\n\r",
                clan_table[clan].who_name);
        
        for (i = 0; i < CLAN_VANQ_TABLE_LENGTH_PRINT; i++) 
        {
                if (clan_vanq_table[clan][i].killed != 0) 
                {
                        sprintf(tmpbuf, "%15s", clan_vanq_table[clan][i].name);
                        strcat(buf, tmpbuf);
                        sprintf(tmpbuf, "   %5d\n\r", clan_vanq_table[clan][i].killed);
                        strcat(buf, tmpbuf);
                }
        }
        send_to_char(buf, ch);
}


/*
 * Hero table
 */
void print_hero_table ( CHAR_DATA *ch, char *argument ) 
{
        HERO_DATA * hero;
        int count;
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        
        const char* class_colours[] = 
        {
                "{Y", "{G", "{B", "{M", "{W", "{R", "{C", "{w" 
        };
        const char* sub_class_colours[] = 
        {
                "{x", "{Y", "{Y", "{G", "{G", "{B", "{B", "{M", "{M",
                "{W", "{W", "{R", "{R", "{C", "{C", "{w", "{w" 
        }; 
        
        if (!hero_first)
                sprintf( buf, "\n\rNobody has reached the level of hero yet!\n\r");
        else
        {
                sprintf(buf, "\n\rThe following table is a tribute to those players who "
                        "have dedicated\n\rthemselves to attaining the highest honour "
                        "possible within the Dragons\n\rDomain -- that of becoming a hero.\n\r\n\r"
                        "  Rank   Hero's name      Class          Date Attained\n\r"
                        "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");
                
                for ( hero = hero_first, count = 1; hero; hero = hero->next, count++ )
                {
                        sprintf (tmpbuf, "%4d%2s   {W%-16s{x %s%-14s{x {w%s{x\n\r",
                                 count,
                                 number_suffix (count),
                                 hero->name,
                                 hero->subclassed
                                 ? sub_class_colours[hero->class]
                                 : class_colours[hero->class],
                                 hero->subclassed 
                                 ? sub_class_table[hero->class].show_name
                                 : class_table[hero->class].show_name,
                                 hero->date);
                        strcat(buf, tmpbuf);
                }
        }
        send_to_char(buf, ch);
}


/*
 *  This function adds a new hero to the table in date sorted order
 *  - passed argument is char name
 */
void add_hero ( CHAR_DATA *ch ) 
{
        HERO_DATA * hero;
        char *strtime;
        hero = alloc_perm( sizeof( HERO_DATA ) );
        
        strtime = ctime (&current_time);
        strtime[strlen(strtime)-1] = '\0';
        
        if (ch->pcdata->choose_subclass && ch->sub_class != 0 ) 
        {
                hero->subclassed = TRUE;
                hero->class = ch->sub_class;
        } 
        else 
        {
                hero->subclassed = FALSE;
                hero->class = ch->class;
        }

        hero->name = str_dup( ch->name );
        hero->date = str_dup( strtime );
        
        if ( !hero_first )
                hero_first = hero;
        
        if (  hero_last  )
                hero_last->next = hero;
        
        hero_last = hero;
        hero->next = NULL;
        
        save_hero_table();
}


/*
 * Legend table stuff - Tavolir 
 */

void print_legend_table ( CHAR_DATA *ch, char *argument )
{
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        int i;
        char* colour;
        
        sprintf (buf, "Owl appears from out of thin air and proclaims: \"The\n\r"
                 "following Adventurers are the most legendary within\n\r"
                 "the Domain...\"\n\r"); 
        strcat( buf, "\n\r  Rank    Name            Class       Greatest fame");
        strcat( buf, "\n\r  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");
        
        for ( i = 0; i < LEGEND_TABLE_LENGTH; i++ )
        {
                if (i == 0)     colour = "{W";
                else if (i < 2) colour = "{Y";
                else if (i < 4) colour = "{G";
                else if (i < 7) colour = "{C";
                else if (i <12) colour = "{c";
                else            colour = "{w";
                
                if(legend_table[i].name[0] != '*') 
                {
                        sprintf (tmpbuf, "  %s%2d%2s    %-15s %-11s %9d{x\n\r",
                                 colour,
                                 i+1,
                                 number_suffix (i+1), 
                                 legend_table[i].name,
                                 legend_table[i].subclassed 
                                 ? sub_class_table[legend_table[i].class].show_name
                                 : class_table[legend_table[i].class].show_name,
                                 legend_table[i].fame );
                        strcat(buf, tmpbuf);
                }
        }
        
        send_to_char(buf, ch);
}


LEGEND_DATA swap_legend_table (LEGEND_DATA temp, int position)
{
        LEGEND_DATA swap;
        
        swap.fame = legend_table[position].fame;
        swap.class = legend_table[position].class;
        swap.subclassed = legend_table[position].subclassed;
        strcpy(swap.name, legend_table[position].name);
        
        legend_table[position].fame = temp.fame;
        legend_table[position].class = temp.class;
        legend_table[position].subclassed = temp.subclassed;
        strcpy(legend_table[position].name, temp.name);
        
        return swap;
}


void update_legend_table ( CHAR_DATA *ch)
{
        LEGEND_DATA tmplegend;
        char buf [ MAX_STRING_LENGTH ];
        int i;
        bool alreadylegend, legendok;
        
        if ( IS_NPC( ch ) || ch->level > LEVEL_HERO)
                return; 
        
        if ( ch->pcdata->fame < legend_table[LEGEND_TABLE_LENGTH - 1].fame)
                return;
        
        alreadylegend = FALSE;
        legendok = FALSE;
        
        for (i = 0; i < LEGEND_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp(legend_table[i].name, ch->name) ) 
                {
                        alreadylegend = TRUE;
                        if (ch->pcdata->fame > legend_table[i].fame) 
                        {
                                sprintf(legend_table[i].name, "*");
                                legend_table[i].fame = 0;
                        } 
                        else
                                return;
                } 
        }
        
        if (ch->pcdata->choose_subclass && ch->sub_class != 0 ) 
        {
                tmplegend.subclassed = TRUE;
                tmplegend.class = ch->sub_class;
        } 
        else 
        {
                tmplegend.subclassed = FALSE;
                tmplegend.class = ch->class;
        }
        tmplegend.fame = ch->pcdata->fame;
        sprintf(tmplegend.name, "%s", ch->name);
        
        for (i = 0; i < LEGEND_TABLE_LENGTH; i++) 
        {
                if( tmplegend.fame > legend_table[i].fame ) 
                {
                        tmplegend = swap_legend_table(tmplegend, i);
                        legendok = TRUE;
                }
        }
        
        if (legendok && !alreadylegend) 
        {
                sprintf( buf, "{YCongratulations!  You have become famous enough to be a legend of the Dragons Domain!{x\n\r");
                send_to_char(buf, ch);
        }
        
        save_legend_table();
}


/*
 * pkscore table stuff - Tavolir
 * Extended by Gezhp and Shade 2000
 */
void print_pkscore_table (CHAR_DATA *ch, int start)
{
        char buf  [ MAX_STRING_LENGTH ];
        char tmpbuf  [ MAX_STRING_LENGTH ];
        int i;
        double ratio;
        int rating;     
        bool found;
        
        const char* ratings[8] = 
        {
                "{w  :Victim:  ",
                "{w   Coward   ",
                "{B  Brigand   ", 
                "{B  Stalker   ",
                "{M  -Hunter-  ",
                "{M <Assassin> ",
                "{R-=Predator=-", 
                "{R <<Slayer>> " 
        };

        const char* class_colours[] =
        {
                "{Y", "{G", "{B", "{M", "{W", "{R", "{C", "{w", "{G"
        };
        
        const char* sub_class_colours[] =
        {
                "{x", "{Y", "{Y", "{G", "{G", "{B", "{B", "{M", "{M",
                "{W", "{W", "{R", "{R", "{C", "{C", "{w", "{w", "{G", "{G" 
        }; 
        
        sprintf( buf, "                                                 Raw    Final\n\r"   
                "Rank   Name         Class      Kills   Deaths   Score   Score     Rating\n\r"
                "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");

        found = FALSE;

        for ( i = start; i < start + PKSCORE_TABLE_LENGTH_PRINT; i++ )
        {
                if (pkscore_table[i].name[0] != '*' && pkscore_table[i].name[0] != '\0')
                {
                        found = TRUE;
                        
                        if (!pkscore_table[i].pkills)
                                rating = 0;
                        else 
                        {
                                ratio = (double)pkscore_table[i].pkscore
                                        / (double)pkscore_table[i].pkills;
                                
                                if      (ratio <= 0  ) rating = 0;
                                else if (ratio <  1.8) rating = 1;
                                else if (ratio <  2.5) rating = 2;
                                else if (ratio <= 3  ) rating = 3;
                                else if (ratio <  3.5) rating = 4;
                                else if (ratio <  4.0) rating = 5;
                                else if (ratio <  4.5) rating = 6;
                                else                   rating = 7;
                        }
                        
                        sprintf (tmpbuf, "%2d%2s   {W%-12s{x %s%-9s{x  {C%4d    %4d  %7d{x  {Y%6d{x   %s{x\n\r",
                                 i+1,
                                 number_suffix (i+1),
                                 pkscore_table[i].name,
                                 pkscore_table[i].subclassed
                                 ? sub_class_colours[pkscore_table[i].class]
                                 : class_colours[pkscore_table[i].class],
                                 pkscore_table[i].subclassed
                                 ? sub_class_table[pkscore_table[i].class].show_name
                                 : class_table[pkscore_table[i].class].show_name,
                                 pkscore_table[i].pkills,
                                 pkscore_table[i].pdeaths,
                                 pkscore_table[i].pkscore,
                                 pkscore_table[i].pkscore - 2 * pkscore_table[i].pdeaths,
                                 ratings[rating]);
                        strcat(buf, tmpbuf);
                } 
        }
        
        strcat(buf, "\n\rType 'PKILLERS' to see the overall pkillers table.\n\r"
               "For a list of pkill statistics by clan, type 'PKILLERS <clan name>'.\n\r");
        
        if (found)
                send_to_char(buf, ch);
}


PKSCORE_DATA swap_pkscore_table (PKSCORE_DATA temp, int position)
{
        PKSCORE_DATA swap;
        
        swap.pkills = pkscore_table[position].pkills;
        swap.pkscore = pkscore_table[position].pkscore;
        swap.pdeaths = pkscore_table[position].pdeaths;
        swap.class = pkscore_table[position].class;
        swap.subclassed = pkscore_table[position].subclassed;
        strcpy(swap.name, pkscore_table[position].name);
        
        pkscore_table[position].pkills = temp.pkills;
        pkscore_table[position].pkscore = temp.pkscore;
        pkscore_table[position].pdeaths = temp.pdeaths;
        pkscore_table[position].class = temp.class;
        pkscore_table[position].subclassed = temp.subclassed;
        strcpy(pkscore_table[position].name, temp.name);
        
        return swap;
}


void update_pkscore_table (CHAR_DATA *ch)
{
        /*
         *  Update rewritten to reflect new scoring system; Gezhp 2000
         */

        PKSCORE_DATA    tmp_pkscore;
        int             i, j, k, pos;

        if (IS_NPC(ch) || ch->level > LEVEL_HERO)
                return; 

        if (ch->pcdata->choose_subclass && ch->sub_class != 0) 
        {
                tmp_pkscore.subclassed = TRUE;
                tmp_pkscore.class = ch->sub_class;
        } 
        else 
        {
                tmp_pkscore.subclassed = FALSE;
                tmp_pkscore.class = ch->class;
        }
        
        tmp_pkscore.pkscore = ch->pcdata->pkscore;
        tmp_pkscore.pkills = ch->pcdata->pkills;
        tmp_pkscore.pdeaths = ch->pcdata->pdeaths;
        sprintf (tmp_pkscore.name, "%s", ch->name);
        
        if (ch->clan)
                sprintf (tmp_pkscore.clan, "{%c%s",
                         clan_table[ch->clan].colour_code,
                         clan_table[ch->clan].who_name);
        else 
                strcpy (tmp_pkscore.clan, "{wRon");
                         
        pos = -1;
        
        for (i = 0; i < PKSCORE_TABLE_LENGTH; i++) 
        {
                if (!strcasecmp (pkscore_table[i].name, ch->name)) 
                {
                        pos = i;
                        for (j = i; j < PKSCORE_TABLE_LENGTH - 1; j++)
                                pkscore_table[j] = pkscore_table[j+1];
                        sprintf (pkscore_table[PKSCORE_TABLE_LENGTH-1].name, "*");
                        break;
                }
        }
        
        j = PKSCORE_TABLE_LENGTH;
        
        for (i = 0; i < PKSCORE_TABLE_LENGTH; i++) 
        {
                if (   (tmp_pkscore.pkscore - 2 * tmp_pkscore.pdeaths >
                        pkscore_table[i].pkscore - 2 * pkscore_table[i].pdeaths)
                    || ((tmp_pkscore.pkscore - 2 * tmp_pkscore.pdeaths ==
                         pkscore_table[i].pkscore - 2 * pkscore_table[i].pdeaths)
                        && (((double)tmp_pkscore.pkscore / (double)tmp_pkscore.pkills)
                            > (double)pkscore_table[i].pkscore / (double)pkscore_table[i].pkills))
                    || !strcmp (pkscore_table[i].name, "*")) 
                {
                        for (k = PKSCORE_TABLE_LENGTH - 1; k > i; k--)
                                pkscore_table[k] = pkscore_table[k-1];

                        pkscore_table[i] = tmp_pkscore;
                        /* if (j == PKSCORE_TABLE_LENGTH) */
                                j = i;
                        break;
                }
        }
        
        if (j < PKSCORE_TABLE_LENGTH_PRINT && tmp_pkscore.pkscore > 0) 
        {
                if (pos < 0) 
                        send_to_char ("{YCongratulations!  You have one of the highest pkill scores in the Dragons Domain!{x\n\r", ch);
                else if (j < pos) 
                        send_to_char ("{YCongratulations!  You have improved your standing as one of the most\n\r"
                                      "fearsome pkillers within the Dragons Domain!{x\n\r", ch);
                else if (j > pos)
                        send_to_char ("{YCondolances!  Your reputation as a fearsome pkiller has been reduced!{x\n\r", ch);
        }
        
        save_pkscore_table();
}


/*
 * New combined pkill and vanq tables; Gezhp 2000
 */
void print_clan_pkill_table ( CHAR_DATA *ch, char *argument ) 
{
        char buf [MAX_STRING_LENGTH];
        char tmp [MAX_STRING_LENGTH];
        int i;
        int clan;
        char *colour;
        bool found_pkill;
        bool found_vanq;
        
        argument = one_argument(argument, buf);
        
        if (buf[0] == '\0' || !strcmp(buf, "1"))
        {
                print_pkscore_table(ch, 0);
                return;
        }

        /*
        else if (!strcmp(buf, "2"))
        {
                print_pkscore_table(ch, PKSCORE_TABLE_LENGTH_PRINT);
                return;
        }
        */

        /*
        else if (!strcmp(buf, "3"))
        {
                print_pkscore_table(ch, 2 * PKSCORE_TABLE_LENGTH_PRINT);
                return;
        }
        */
        
        if ((clan = clan_lookup(buf)) == -1)
        {
                send_to_char("There is no such clan.\n\r",ch);
                return;
        }

        sprintf(buf, "\n\r{WPkill statistics for clan %s (%s):{x\n\r\n\r"
                "   Most frequent pkillers                Most frequently vanquished\n\r"
                "~~~~~~~~~~~~~~~~~~~~~~~~~~~~            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r",
                clan_table[clan].who_name,
                clan_table[clan].name);
        
        for (i = 0; i < CLAN_PKILL_TABLE_LENGTH_PRINT; i++) 
        {
                found_pkill = FALSE;
                found_vanq = FALSE;
                
                /*
                 * pkillers
                 */
                if (i == 0)     colour = "{W";
                else if (i < 2) colour = "{Y";
                else if (i < 4) colour = "{G";
                else if (i < 7) colour = "{C";
                else if (i <12) colour = "{c";
                else            colour = "{w";
                
                if (clan_pkill_table[clan][i].pkills) 
                {
                        found_pkill = TRUE;
                        sprintf(tmp, "%s%2d%s     %-14s%4d             ",
                                colour,
                                i+1,
                                number_suffix(i+1),
                                clan_pkill_table[clan][i].name,
                                clan_pkill_table[clan][i].pkills);
                        strcat(buf, tmp);
                }
                else if (!i)
                {
                        strcat(buf, "{x  No pkills have been made{W              ");
                        found_pkill = TRUE;
                }
                
                /*
                 * vanquished
                 */
                
                if (i < CLAN_VANQ_TABLE_LENGTH_PRINT
                    && clan_vanq_table[clan][i].killed != 0)
                {
                        if (!found_pkill)
                                strcat(buf, "                                        ");

                        found_vanq = TRUE;
                        sprintf(tmp, "%s%2d%s     %-14s%4d",
                                colour,
                                i+1,
                                number_suffix(i+1),
                                clan_vanq_table[clan][i].name,
                                clan_vanq_table[clan][i].killed);
                        strcat(buf, tmp);
                }
                else if (!i)
                {
                        strcat(buf, "{x Nobody has been vanquished");
                        found_vanq = TRUE;
                }
                
                if (!found_pkill && !found_vanq)
                        break;
                
                strcat(buf, "{x\n\r");
        }
        
        send_to_char(buf, ch);
}


/*
 *  Wanted list: show current bounties on offer; Gezhp 2001
 */

void do_wanted (CHAR_DATA *ch, char *argument)
{
        char buf [MAX_STRING_LENGTH];
        WANTED_DATA *wanted;
        
        if (!wanted_list_first)
        {
                send_to_char("There is nobody currently wanted in the Domain.\n\r", ch);
                return;
        }
        
        send_to_char("\n\r    The following scoundrels and dogs are to be hunted down and killed!\n\r"
                     "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r", ch);
        
        for (wanted = wanted_list_first; wanted; wanted = wanted->next)
        {
                if (wanted->name[0] == '\0')
                        continue;
                
                sprintf(buf, "  {W%-12s{x  {C%7d{x plat   [%-9s %3s]   %s\n\r",
                        wanted->name,
                        wanted->reward/1000,
                        wanted->class,
                        wanted->clan,
                        wanted->date);
                send_to_char(buf, ch);
        }
}


void load_wanted_table ()
{
        WANTED_DATA *next;
        char path [MAX_STRING_LENGTH];
        char buf [MAX_STRING_LENGTH];
        FILE *file;
        char *word;
        int   stat;
        
        wanted_list_first = NULL;
        next = NULL;
        
        sprintf(path, "%s%s", PLAYER_DIR, "TABLES/wanted.table");
        
        if (!(file = fopen(path, "r")))
        {
                log_string("load_wanted_table: unable to open 'wanted.table'");
                return;
        }
        
        while (1)
        {
                if (fread_letter(file) != '#')
                {
                        log_string("load_wanted_table: expected '#'");
                        break;
                }
                
                word = fread_word(file);
                
                if (!str_cmp(word, "WANTED"))
                {
                        next = alloc_perm(sizeof(WANTED_DATA));
                        
                        next->date = fread_string(file);
                        next->name = fread_string(file);
                        next->class = fread_string(file);
                        next->clan = fread_string(file);
                        next->reward = fread_number(file, &stat);
                        next->next = NULL;
                        
                        if (!wanted_list_first)
                                wanted_list_first = next;
                        
                        if (wanted_list_last)
                                wanted_list_last->next = next;
                        
                        wanted_list_last = next;
                }
                
                else if (!str_cmp(word, "END"))
                        break;
                
                else
                {
                        sprintf(buf, "load_wanted_list: section: '%s'", word);
                        log_string(buf);
                        break;
                }
        }
        
        fclose(file);
}


void update_wanted_table (CHAR_DATA *ch)
{
        WANTED_DATA *wanted;
        char *date;
        char *class;
        
        if (IS_NPC(ch)
            || !ch->clan
            || !ch->pcdata->bounty)
        {
                return;
        }
        
        if (ch->sub_class)
                class = (char *)sub_class_table[ch->sub_class].show_name;
        else
                class = (char *)class_table[ch->class].show_name;
        
        for (wanted = wanted_list_first; wanted; wanted = wanted->next)
        {
                if (!str_cmp(wanted->name, ch->name))
                        break;
        }
        
        if (!wanted)
        {
                date = ctime(&current_time);
                date[strlen(date)-1] = '\0';
                
                wanted = alloc_perm(sizeof(WANTED_DATA));
                
                wanted->name = str_dup(ch->name);
                wanted->date = str_dup(date);
                wanted->class = str_dup(class);
                wanted->clan = str_dup(clan_table[ch->clan].who_name);
                wanted->reward = ch->pcdata->bounty;
                wanted->next = NULL;
                
                if (!wanted_list_first)
                        wanted_list_first = wanted;
                
                if (wanted_list_last)
                        wanted_list_last->next = wanted;
                
                wanted_list_last = wanted;
        }
        else 
        {
                free_string(wanted->class);
                free_string(wanted->clan);
                wanted->class = str_dup(class);
                wanted->clan = str_dup(clan_table[ch->clan].who_name);
                wanted->reward = ch->pcdata->bounty;
        }
        
        save_wanted_table();
}


void remove_from_wanted_table (char *name)
{
        /*
         *  Hacky but who wants to do pointer swapping: not me 
         */
        
        WANTED_DATA *wanted;
        
        for (wanted = wanted_list_first; wanted; wanted = wanted->next)
        {
                if (!str_cmp(name, wanted->name))
                {
                        wanted->name = str_dup("");
                        save_wanted_table();
                        return;
                }
        }
}


void save_wanted_table ()
{
        FILE *file;
        WANTED_DATA *wanted;
        char path [MAX_STRING_LENGTH];
        
        sprintf(path, "%s%s", PLAYER_DIR, "TABLES/wanted.table");

        if (!(file = fopen(path, "w")))
        {
                log_string("save_wanted_table: couldn't open file 'wanted.table' for writing");
                return;
        }
        
        for (wanted = wanted_list_first; wanted; wanted = wanted->next)
        {
                if (wanted->name[0] == '\0')
                        continue;

                fprintf(file, "#WANTED\n%s~\n%s~\n%s~\n%s~\n%d\n\n",
                        wanted->date,
                        wanted->name,
                        wanted->class,
                        wanted->clan,
                        wanted->reward);
        }
        
        fprintf(file, "#END\n");
        fclose(file);
}


/* EOF table.c */
