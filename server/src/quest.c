/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

/***************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com   *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this  *
*  code is allowed provided you add a credit line to the effect of:        *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest    *
*  of the standard diku/rom credits. If you use this or a modified version *
*  of this code, let me know via email: moongate@moongate.ams.com. Further *
*  updates will be posted to the rom mailing list. If you'd like to get    *
*  the latest version of quest.c, please send a request to the above add-  *
*  ress. Quest Code v2.00.                                                 *
***************************************************************************/


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

DECLARE_DO_FUN( do_say );

/* Object vnums for Quest Rewards */

#define QUEST_ITEM1 8322
#define QUEST_ITEM2 8323
#define QUEST_ITEM3 8324
#define QUEST_ITEM4 17543
#define QUEST_ITEM5 8325

/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 75
#define QUEST_OBJQUEST2 76
#define QUEST_OBJQUEST3 77
#define QUEST_OBJQUEST4 78
#define QUEST_OBJQUEST5 79

/* Local functions */

void generate_quest     args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void generate_special_quest (CHAR_DATA *ch, CHAR_DATA *questman);
void quest_update       args(( void ));
bool chance             args(( int num ));

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
        return number_range(1,100) <= num;
}

/* The main quest function */

void do_quest (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *questman;
        OBJ_DATA *obj=NULL, *obj_next;
        OBJ_INDEX_DATA *questinfoobj;
        MOB_INDEX_DATA *questinfo;
        char buf [MAX_STRING_LENGTH];
        char arg1 [MAX_INPUT_LENGTH];
        char arg2 [MAX_INPUT_LENGTH];
        int next;

        const int NUMBER_RECALL_POINTS = 12;
        const struct quest_recall quest_recall_info [] =
        {
            /*  Pfile slot, quest point cost, room vnum, name  */
                {  2,  1000,  28003,  "Draagdim"        },
                { 10,  1000,   3101,  "Midgaard"        },
                { 12,   900,  21500,  "Demondium"       },
                {  3,   800,  30050,  "Krondor"         },
                {  4,   500,  31000,  "Anon"            },
                {  5,   500,  29153,  "Freeport"        },
                {  6,   500,  18835,  "Dahij"           },
                { 13,   400,   1313,  "HighTower"       },
                {  7,   300,  30234,  "Kerofk"          },
                { 11,   250,  12167,  "Westreen"        },
                {  8,   200,  10201,  "Solace"          },
                {  9,   100,    601,  "Ofcol"           }
        };

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (is_name(arg1, "info"))
        {
                if (!IS_SET(ch->act, PLR_QUESTOR))
                {
                        send_to_char("You aren't currently on a quest.\n\r",ch);
                        return;
                }

                if (ch->pcdata->questmob == -1 && ch->pcdata->questgiver->short_descr)
                {
                        sprintf(buf, "Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->pcdata->questgiver->short_descr);
                        send_to_char(buf, ch);
                        return;
                }

                if (ch->pcdata->questobj > 0 && (questinfoobj = get_obj_index(ch->pcdata->questobj)))
                {
                        sprintf(buf, "You are on a quest to recover %s!\n\r", questinfoobj->short_descr);
                        send_to_char(buf, ch);
                        return;
                }

                if (ch->pcdata->questmob > 0 && (questinfo = get_mob_index(ch->pcdata->questmob)))
                {
                        sprintf(buf, "You are on a quest to slay %s!\n\r",questinfo->short_descr);
                        send_to_char(buf, ch);
                        return;
                }

                return;
        }

        if (is_name(arg1, "points"))
        {
                sprintf(buf, "You have earned {W%d{x quest points and may spend {W%d{x.\n\r",
                        ch->pcdata->totalqp,
                        ch->pcdata->questpoints);
                send_to_char(buf, ch);
                return;
        }

        if (is_name(arg1, "abort"))
        {
                if (!IS_SET(ch->act, PLR_QUESTOR))
                {
                        send_to_char ("You aren't currently on a quest.\n\r", ch);
                        return;
                }

                REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->pcdata->questgiver = NULL;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
                ch->pcdata->questobj = 0;
                ch->pcdata->nextquest = QUEST_ABORT_DELAY;
                send_to_char ("You abandon your quest.\n\r", ch);
                return;
        }

        if (is_name(arg1, "time"))
        {
                if (!IS_SET(ch->act, PLR_QUESTOR))
                {
                        send_to_char("You aren't currently on a quest.\n\r",ch);

                        if (ch->pcdata->nextquest > 1)
                        {
                                sprintf(buf, "There are %d minutes remaining until you can go on another quest.\n\r",ch->pcdata->nextquest);
                                send_to_char(buf, ch);
                        }
                        else if (ch->pcdata->nextquest == 1)
                        {
                                sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
                                send_to_char(buf, ch);
                        }
                }

                else if (ch->pcdata->countdown > 0)
                {
                        sprintf(buf, "Time left for current quest: %d minutes.\n\r",ch->pcdata->countdown);
                        send_to_char(buf, ch);
                }

                return;
        }

        /*
         * Checks for a character in the room with spec_questmaster set. This special
         * procedure must be defined in special.c. You could instead use an
         * ACT_QUESTMASTER flag instead of a special procedure.
         */

        for (questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room)
        {
                if (IS_NPC(questman) && IS_SET(questman->act, ACT_QUESTMASTER))
                        break;
        }

        if (!questman || questman->fighting)
        {
                send_to_char("You can't do that here.\n\r",ch);
                return;
        }

        ch->pcdata->questgiver = questman;

        if (is_name(arg1, "list"))
        {
                send_to_char ("Recall points available for purchase:\n\r\n\r", ch);

                for (next = 0; next < NUMBER_RECALL_POINTS; next++)
                {
                        sprintf (buf, "   {W%-12s{x  %4d qp    [%s]\n\r",
                                 quest_recall_info[next].name,
                                 quest_recall_info[next].cost,
                                 (ch->pcdata->recall_points[quest_recall_info[next].slot] >= 0)
                                 ? "{chave{x" : "{RNEED{x");
                        send_to_char (buf, ch);
                }

                sprintf (buf, "\n\rYou have {W%d{x quest point%s to spend.\n\r",
                         ch->pcdata->questpoints,
                         ch->pcdata->questpoints == 1 ? "" : "s");

                send_to_char (buf, ch);

                return;
        }


        if (is_name(arg1, "buy"))
        {
                if (arg2[0] == '\0')
                {
                        send_to_char("To buy an item, type 'QUEST BUY <recall point>'.\n\r",ch);
                        return;
                }

                if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                {
                        send_to_char("Not in your current form.\n\r", ch);
                        return;
                }

                for (next = 0; next < NUMBER_RECALL_POINTS; next++)
                {
                        if (is_name (arg2, quest_recall_info[next].name))
                        {
                                if (ch->pcdata->recall_points[quest_recall_info[next].slot] >= 0)
                                {
                                        send_to_char ("You already have that recall point.\n\r", ch);
                                        return;
                                }

                                if (ch->pcdata->questpoints < quest_recall_info[next].cost)
                                {
                                        send_to_char ("You don't have enough quest points to buy that recall point.\n\r", ch);
                                        return;
                                }

                                ch->pcdata->questpoints -= quest_recall_info[next].cost;
                                ch->pcdata->recall_points[quest_recall_info[next].slot] = quest_recall_info[next].room;

                                sprintf (buf, "Well done %s, you may now recall to %s.",
                                         ch->name,
                                         quest_recall_info[next].name);
                                do_say (questman, buf);

                                return;
                        }
                }

                sprintf (buf, "%s tells you 'I don't offer that reward.'\n\r",
                         questman->short_descr);
                send_to_char (buf, ch);
                return;
        }


        if (is_name(arg1, "request"))
        {
                if (ch->position < POS_RESTING)
                {
                        send_to_char ("Want to quest in your dreams, do you?\n\r", ch);
                        return;
                }

                if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                {
                        send_to_char("Not in your current form.\n\r", ch);
                        return;
                }

                act ("$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
                act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);

                if (IS_SET(ch->act, PLR_QUESTOR))
                {
                        sprintf(buf, "But you're already on a quest!");
                        do_say(questman, buf);
                        return;
                }

                if (ch->pcdata->nextquest > 0)
                {
                        sprintf(buf, "You're very brave, %s, but let someone else have a chance.", ch->name);
                        do_say(questman, buf);
                        return;
                }

                if (ch->pcdata->fame < 0)
                {
                        sprintf(buf, "You are not worthy of receiving a quest, %s.", ch->name);
                        do_say(questman, buf);
                        return;
                }

                sprintf(buf, "$N exclaims 'Thank you, brave %s!'",ch->name);
                act(buf, ch, NULL, questman, TO_CHAR);

                generate_quest(ch, questman);

                if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
                {
                        ch->pcdata->countdown = number_range(10,30);
                        SET_BIT(ch->act, PLR_QUESTOR);
                        sprintf(buf, "$N says 'You have %d minutes to complete this quest.'",ch->pcdata->countdown);
                        act(buf, ch, NULL, questman, TO_CHAR);
                }

                return;
        }

        if (is_name(arg1, "complete"))
        {
                if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                {
                        send_to_char("Not in your current form.\n\r", ch);
                        return;
                }

                act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
                act ("You inform $N you have completed your quest.", ch, NULL, questman, TO_CHAR);

                if (IS_SET(ch->act, PLR_QUESTOR))
                {
                        if (ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0)
                        {
                                int reward, pointreward, famereward, pracreward;

                                famereward = number_fuzzy(number_fuzzy(ch->level/10));
                                famereward = famereward > 0 ? famereward : 1;
                                reward = number_range(1, 10); /* gp */
                                pointreward = number_range(10,40);

                                if (questman->level > 70)
                                        pointreward += pointreward / 2;
                                else if (questman->level > 40)
                                        pointreward += pointreward / 4;


                                if (ch->sub_class == SUB_CLASS_KNIGHT || ch->sub_class == SUB_CLASS_TEMPLAR)
                                        pointreward += pointreward / 4;

                                sprintf(buf, "Congratulations on completing your quest!");
                                do_say(questman,buf);
                                sprintf(buf, "Your reward is %d quest points, %d fame point%s and %d gold coins.",
                                        pointreward,
                                        famereward,
                                        (famereward == 1) ? "" : "s",
                                        reward);
                                do_say(questman,buf);
                                send_to_char ("The money is placed into your bank account.\n\r", ch);

                                if (ch->pcdata->str_prac < 10 && chance(8))
                                {
                                        pracreward = number_range(3, 6);
                                        sprintf(buf, "You also gain %d physical practice points for your effort!\n\r", pracreward);
                                        send_to_char (buf, ch);
                                        ch->pcdata->str_prac += pracreward;
                                }
                                else
                                {
                                        if (ch->pcdata->int_prac < 10 && chance(8))
                                        {
                                                pracreward = number_range(3, 6);
                                                sprintf(buf, "You also gain %d intellectual practice points for your effort!\n\r", pracreward);
                                                send_to_char (buf, ch);
                                                ch->pcdata->int_prac += pracreward;
                                        }
                                }

                                ch->pcdata->fame += famereward;
                                check_fame_table(ch);
                                REMOVE_BIT(ch->act, PLR_QUESTOR);
                                ch->pcdata->questgiver = NULL;
                                ch->pcdata->countdown = 0;
                                ch->pcdata->questmob = 0;
                                ch->pcdata->questobj = 0;
                                ch->pcdata->nextquest = QUEST_MAX_DELAY;
                                ch->pcdata->bank += reward * 100;
                                ch->pcdata->questpoints += pointreward;
                                ch->pcdata->totalqp += pointreward;

                                return;

                        }
                        else if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
                        {
                                bool obj_found = FALSE;

                                for (obj = ch->carrying; obj != NULL; obj= obj_next)
                                {
                                        obj_next = obj->next_content;

                                        if (obj != NULL && obj->pIndexData->vnum == ch->pcdata->questobj)
                                        {
                                                obj_found = TRUE;
                                                break;
                                        }
                                }

                                if (obj_found)
                                {
                                        int reward, pointreward, pracreward;

                                        reward = number_range(1, 10) * 5; /* gp */
                                        pointreward = number_range(10,40);

                                        if (questman->level > 70)
                                                pointreward += pointreward / 2;
                                        else if (questman->level > 40)
                                                pointreward += pointreward / 4;

                                        act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
                                        act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                                        sprintf(buf, "Congratulations on completing your quest!");
                                        do_say(questman,buf);
                                        sprintf(buf,"As a reward, I am giving you %d quest points and %d gold coins.",
                                                pointreward,reward);
                                        do_say(questman,buf);
                                        send_to_char ("The money is placed into your bank account.\n\r", ch);

                                        if (ch->pcdata->str_prac < 10 && chance(8))
                                        {
                                                pracreward = number_range(3, 6);
                                                sprintf(buf, "You also gain %d physical practice points for your effort!\n\r",
                                                        pracreward);
                                                send_to_char (buf, ch);
                                                ch->pcdata->str_prac += pracreward;
                                        }
                                        else
                                        {
                                                if (ch->pcdata->int_prac < 10 && chance(8))
                                                {
                                                        pracreward = number_range(3, 6);
                                                        sprintf(buf, "You also gain %d intellectual practice points for your effort!\n\r", pracreward);
                                                        send_to_char (buf, ch);
                                                        ch->pcdata->int_prac += pracreward;
                                                }
                                        }                                        

                                        REMOVE_BIT(ch->act, PLR_QUESTOR);
                                        ch->pcdata->questgiver = NULL;
                                        ch->pcdata->countdown = 0;
                                        ch->pcdata->questmob = 0;
                                        ch->pcdata->questobj = 0;
                                        ch->pcdata->nextquest = QUEST_MAX_DELAY;
                                        ch->pcdata->bank += reward * 100;
                                        ch->pcdata->questpoints += pointreward;
                                        ch->pcdata->totalqp += pointreward;
                                        extract_obj(obj);
                                        return;
                                }
                                else
                                {
                                        sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                                        do_say(questman, buf);
                                        return;
                                }
                        }

                        sprintf(buf, "You haven't completed the quest yet, but there is still time!");
                        do_say(questman, buf);
                        return;
                }

                if (ch->pcdata->nextquest > 0)
                        sprintf(buf,"But you didn't complete your quest in time!");
                else
                        sprintf(buf, "You have to REQUEST a quest first, %s.",ch->name);

                do_say(questman, buf);
                return;
        }

        send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY ABORT.\n\r"
                     "For more information, type 'HELP QUEST'.\n\r",ch);
}


void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
        CHAR_DATA *victim;
        MOB_INDEX_DATA *vsearch;
        ROOM_INDEX_DATA *room;
        OBJ_DATA *questitem;
        char buf [MAX_STRING_LENGTH];
        long mcounter;
        int level_diff, mob_vnum, upper_limit;

        for (mcounter = 0; mcounter < 100; mcounter ++) 
        {
                do
                        mob_vnum = number_range(100, 32200);
                while
                        (!get_mob_index(mob_vnum)
                         || mob_vnum == BOT_VNUM);

                room = NULL;
                victim = NULL;

                vsearch = get_mob_index(mob_vnum);
                level_diff = vsearch->level - ch->level;

                if (ch->level < 20)
                        upper_limit = 5;
                else if (ch->level < 50)
                        upper_limit = 10;
                else if (ch->level < 80)
                        upper_limit = 15;
                else
                        upper_limit = 20;

                if ((level_diff < upper_limit && level_diff > -15)
                    && !vsearch->pShop
                    && !IS_SET(vsearch->act, ACT_NO_QUEST)
                    && !IS_SET(vsearch->act, ACT_PRACTICE)
                    && !IS_SET(vsearch->act, ACT_PET)
                    && !IS_SET(vsearch->act, ACT_QUESTMASTER)
                    && !IS_SET(vsearch->act, ACT_MOUNTABLE)
                    && !IS_SET(vsearch->act, ACT_WIZINVIS_MOB)
                    && !IS_SET(vsearch->act, ACT_BANKER)
                    && !IS_SET(vsearch->act, ACT_LOSE_FAME)
                    && !IS_SET(vsearch->act, ACT_NO_EXPERIENCE)
                    && !IS_SET(vsearch->act, ACT_IS_HEALER))
                
                        /* Shade 31.3.22 - If we want to check area levels before continueing, need to move the room lookup to here */                    
                {
                        /* Shade 3.5.22 - moved test here */
                        victim = get_qchar_world(ch, vsearch->player_name, vsearch->vnum);
                        if (victim) 
                        {
                                room = find_qlocation(ch, victim->name, victim->pIndexData->vnum);

                                if (room && room->area->low_level <= ch->level && room->area->high_level >= ch->level)
                                        break;
                        
                                else
                                        vsearch = NULL;
                        }
                        else
                                vsearch = NULL;
                }
                else
                        vsearch = NULL;
        }

        if (!vsearch)
        {
                sprintf(buf, "$N says 'I'm sorry, but I can't find any suitable quests for you at this time.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                sprintf(buf, "$N says 'Try again later.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                ch->pcdata->nextquest = QUEST_UNAVAILABLE_DELAY;
                return;
        }

        /* 
         * Shade 31.3.22
         *
         * Changed from enforced levels to low/high levels, most areas don't enforce they just pop up the warning.
         * 
         * Test was && not || - have adjusted
         * 
         * And now moved test up as that was resulting in not enough quests
         *          
         */

        if (number_percent() < 15)
        {
                sprintf(buf, "$N says 'I'm sorry, but I don't have any quests for you at this time.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                sprintf(buf, "$N says 'Try again later.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                ch->pcdata->nextquest = QUEST_UNAVAILABLE_DELAY;
                return;
        }

        if (chance(40))
        {
                int objvnum = 0;

                switch (number_range(0,4))
                {
                    case 0:
                        objvnum = QUEST_OBJQUEST1;
                        break;

                    case 1:
                        objvnum = QUEST_OBJQUEST2;
                        break;

                    case 2:
                        objvnum = QUEST_OBJQUEST3;
                        break;

                    case 3:
                        objvnum = QUEST_OBJQUEST4;
                        break;

                    case 4:
                        objvnum = QUEST_OBJQUEST5;
                        break;
                }

                questitem = create_object(get_obj_index(objvnum), ch->level);
                questitem->timer = 240;
                set_obj_owner(questitem, ch->name);
                obj_to_room(questitem, room);
                ch->pcdata->questobj = questitem->pIndexData->vnum;

                sprintf (buf, "\n\r{c%s says, 'Vile pilferers have stolen {C%s{x{c from the royal "
                         "treasury!  My court wizardess, with her magic mirror, has pinpointed its "
                         "location.  I ask that you try your utmost to recover this artefact.  "
                         "Look in the general area of {C%s{x{c for {C%s{x{c!'{x\n\r\n\r",
                         capitalize (questman->short_descr),
                         questitem->short_descr,
                         room->area->name, room->name);

                send_paragraph_to_char(buf, ch, 0);
                return;
        }

        switch(number_range(0,1))
        {
            case 0:
                sprintf(buf, "\n\r{c%s says, 'A bitter enemy of mine, {C%s{x{c, is making vile threats "
                        "against the Domain.  You are to silence this opposition... permanently.'  "
                        "%s grins evilly.  'Seek %s in the vicinity of {C%s{x{c.  That location is in the "
                        "general area of {C%s{x{c.  Good luck!'{x\n\r\n\r",
                        capitalize (questman->short_descr),
                        victim->short_descr,
                        capitalize (questman->short_descr),
                        victim->short_descr,
                        room->name,
                        room->area->name);
                break;

            case 1:
                sprintf (buf, "{c\n\r%s exclaims, 'One of the Domain's most heinous criminals, {C%s{x{c, "
                         "has escaped from captivity!  Since the escape, %s has murdered %d "
                         "defenseless civilians!'  %s looks grave.  'The penalty for these "
                         "crimes is death, and you are to deliver the sentence!  Begin your search "
                         "in the general area of {C%s{x{c; your target has been spotted in the vicinity "
                         "of {C%s{x{c.'{x\n\r\n\r",
                         capitalize (questman->short_descr),
                         victim->short_descr,
                         victim->short_descr,
                         number_range (2, 20),
                         capitalize (questman->short_descr),
                         room->area->name,
                         room->name);
                break;
        }

        ch->pcdata->questmob = victim->pIndexData->vnum;
        send_paragraph_to_char(buf, ch, 0);
}


/*
 * Called from update_handler() by pulse_area
 */
void quest_update ()
{
        CHAR_DATA       *ch, *ch_next;
        char            buf [MAX_STRING_LENGTH];

        sprintf (last_function, "entering quest_update");

        for (ch = char_list; ch != NULL; ch = ch_next)
        {
                ch_next = ch->next;

                if (IS_NPC(ch) || ch->deleted)
                        continue;

                if (ch->pcdata->nextquest > 0)
                {
                        ch->pcdata->nextquest--;

                        if (ch->pcdata->nextquest == 0)
                        {
                                send_to_char ("You may now quest again.\n\r", ch);
                                continue;
                        }
                }
                else if (IS_SET(ch->act,PLR_QUESTOR))
                {
                        if (--ch->pcdata->countdown <= 0)
                        {
                                ch->pcdata->nextquest = QUEST_MAX_DELAY;
                                sprintf (buf, "You have run out of time for your quest!\n\r"
                                        "You may quest again in %d minutes.\n\r",
                                        ch->pcdata->nextquest);
                                send_to_char (buf, ch);
                                REMOVE_BIT (ch->act, PLR_QUESTOR);
                                ch->pcdata->questgiver = NULL;
                                ch->pcdata->countdown = 0;
                                ch->pcdata->questmob = 0;
                        }

                        if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6)
                        {
                                send_to_char("Better hurry, you're almost out of time for your quest!\n\r", ch);
                                continue;
                        }
                }

        }
        sprintf (last_function, "leaving quest_update");
}


void generate_special_quest (CHAR_DATA *ch, CHAR_DATA *questman)
{
        return;
}


bool mob_is_quest_target (CHAR_DATA *ch)
{
        DESCRIPTOR_DATA *d;

        if (!IS_NPC(ch))
                return FALSE;

        for (d = descriptor_list; d; d = d->next)
        {
                if (d->character && d->character->pcdata->questmob == ch->pIndexData->vnum)
                        return TRUE;
        }

        return FALSE;
}



/* EOF quest.c */
