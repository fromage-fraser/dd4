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

#define NUM_QUEST_OBJQUEST 10

/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 75
#define QUEST_OBJQUEST2 76
#define QUEST_OBJQUEST3 77
#define QUEST_OBJQUEST4 78
#define QUEST_OBJQUEST5 79
#define QUEST_OBJQUEST6 585
#define QUEST_OBJQUEST7 586
#define QUEST_OBJQUEST8 587
#define QUEST_OBJQUEST9 588
#define QUEST_OBJQUEST10 589

static const int quest_objquest_vnums[NUM_QUEST_OBJQUEST] = {
        QUEST_OBJQUEST1,
        QUEST_OBJQUEST2,
        QUEST_OBJQUEST3,
        QUEST_OBJQUEST4,
        QUEST_OBJQUEST5,
        QUEST_OBJQUEST6,
        QUEST_OBJQUEST7,
        QUEST_OBJQUEST8,
        QUEST_OBJQUEST9,
        QUEST_OBJQUEST10
};

#define MAX_QUEST_MOB_VNUM 32200
#define MAX_QUEST_OBJ_VNUM 32200

#define HOARD_HINT "\r\n<15>The ground here looks to have been recently disturbed.<0>\n\r"

/* rarity tiers */
#define RARITY_COMMON     0
#define RARITY_UNCOMMON   1
#define RARITY_RARE       2
#define RARITY_EPIC       3
#define RARITY_LEGENDARY  4

/* probability thresholds per 1000 roll:
   1–700: common (70%)
   701–900: uncommon (20%)
   901–980: rare (8%)
   981–995: epic (1.5%)
   996–1000: legendary (0.5%) */
#define THRESH_COMMON    700
#define THRESH_UNCOMMON  900
#define THRESH_RARE      980
#define THRESH_EPIC      995

/* which item types may drop */
static const int hoard_loot_whitelist[] = {
        ITEM_LIGHT,
        ITEM_SCROLL,
        ITEM_WAND,
        ITEM_STAFF,
        ITEM_WEAPON,
        ITEM_DIGGER,
        ITEM_TREASURE,
        ITEM_ARMOR,
        ITEM_POTION,
        ITEM_FURNITURE,
        ITEM_TRASH,
        ITEM_CONTAINER,
        ITEM_DRINK_CON,
        ITEM_KEY,
        ITEM_FOOD,
        ITEM_MONEY,
        ITEM_BOAT,
        ITEM_FOUNTAIN,
        ITEM_PILL,
        ITEM_CLIMBING_EQ,
        ITEM_PAINT,
        ITEM_ANVIL,
        ITEM_AUCTION_TICKET,
        ITEM_POISON_POWDER,
        ITEM_LOCK_PICK,
        ITEM_INSTRUMENT,
        ITEM_ARMOURERS_HAMMER,
        ITEM_MITHRIL,
        ITEM_WHETSTONE,
        ITEM_PIPE,
        ITEM_PIPE_CLEANER,
        ITEM_SMOKEABLE
};

static const int hoard_loot_whitelist_size =
    sizeof(hoard_loot_whitelist) / sizeof(hoard_loot_whitelist[0]);

/* Local functions */

void generate_quest         args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void populate_hoard_loot    args((CHAR_DATA *ch, OBJ_DATA *hoard));
void generate_special_quest (CHAR_DATA *ch, CHAR_DATA *questman);
void quest_update           args(( void ));
bool chance                 args(( int num ));

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

        const int NUMBER_RECALL_POINTS = 15;
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
                { 16,   500,  28503,  "Omu"             },
                { 13,   400,   1313,  "HighTower"       },
                { 14,   400,  27347,  "Ota'ar Dar"      },
                { 15,   400,  16084,  "Underdark"       },
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
                        sprintf(buf, "{cYou are on a quest to recover {C%s{x{c, last seen in {C%s{x{c,\n\rwhich is in the general area of {C%s{x{c!{x\n\r",
                            questinfoobj->short_descr,
                            ch->pcdata->questroom->name,
                            ch->pcdata->questarea->name
                        );
                        send_to_char(buf, ch);
                        return;
                }

                if (ch->pcdata->questmob > 0 && (questinfo = get_mob_index(ch->pcdata->questmob)))
                {
                        sprintf(buf, "{cYou are on a quest to slay {C%s{x{c, last seen in {C%s{x{c,\n\rin the general area of {C%s{x{c!{x\n\r",
                            questinfo->short_descr,
                            ch->pcdata->questroom->name,
                            ch->pcdata->questarea->name);
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
                ch->pcdata->questroom = NULL;
                ch->pcdata->questarea = NULL;
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
                                 ? "{cHAVE{x" : "{RNEED{x");
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
                        send_to_char("To buy an item, type 'QUEST BUY <<recall point>'.\n\r",ch);
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
                        send_to_char ("Dream quests are not materially rewarded.\n\r", ch);
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
                                ch->pcdata->questroom = NULL;
                                ch->pcdata->questarea = NULL;
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
                                        ch->pcdata->questroom = NULL;
                                        ch->pcdata->questarea = NULL;
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
                        mob_vnum = number_range(100, MAX_QUEST_MOB_VNUM);
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

                        /* Shade 31.3.22 - If we want to check area levels before continuing, need to move the room lookup to here */
                {
                        /* Shade 3.5.22 - moved test here */
                        victim = get_qchar_world(ch, vsearch->player_name, vsearch->vnum);
                        if (victim)
                        {
                                room = find_qlocation(ch, victim->name, victim->pIndexData->vnum);
                                ch->pcdata->questroom = room;
                                ch->pcdata->questarea = room->area;

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

        if (number_percent() < 10)
        {
                sprintf(buf, "$N says 'I'm sorry, but I don't have any quests for you at this time.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                sprintf(buf, "$N says 'Try again later.'");
                act(buf, ch, NULL, questman, TO_CHAR);
                ch->pcdata->nextquest = QUEST_UNAVAILABLE_DELAY;
                return;
        }

       /* Pick quest type ~1/3 each: 0=hoard, 1=object, 2=kill */
        switch ( number_range(0, 2) )
        {
            case 0: /* HOARD QUEST */
            {
                ROOM_INDEX_DATA *room;
                int tries = 0;
                do
                {
                    int random_room_vnum = number_range(0, 32200);
                    /* bug("Number of room is %d", random_room_vnum); */

                    /* never allow a room in purgatory */
                    if (random_room_vnum >= 401 && random_room_vnum <= 427)
                        room = NULL;
                    else
                        room = get_room_index(random_room_vnum);

                    if ( ++tries > 400 )
                    {
                        sprintf(buf, "$N says 'I'm sorry, but I don't have any quests for you at this time.'");
                        act(buf, ch, NULL, questman, TO_CHAR);
                        sprintf(buf, "$N says 'Try again later.'");
                        act(buf, ch, NULL, questman, TO_CHAR);
                        ch->pcdata->nextquest = QUEST_UNAVAILABLE_DELAY;
                        return;
                    }
                }
                while( !room
                    ||  room->area == ch->in_room->area
                    ||  room->sector_type == SECT_UNDERWATER
                    ||  room->sector_type == SECT_WATER_SWIM
                    ||  room->sector_type == SECT_WATER_NOSWIM
                    ||  room->sector_type == SECT_AIR );

                /* create the hoard container */
                OBJ_DATA *hoard = create_object(
                    get_obj_index( QUEST_HOARD ),
                    ch->level, "common", CREATED_NO_RANDOMISER );

                /* mark it buried and at full “hitpoints” */
                hoard->value[1] = 1;
                hoard->value[2] = hoard->value[3];
                hoard->timer    = 240;

                /* (optional) arm the hoard with a trap */
                maybe_arm_hoard_trap( hoard, ch->level );

                /* put some coinage in it */
                OBJ_DATA *coins = create_money( number_range( 1, ((ch->level/18)+2) ),
                                                number_range( 5, ((ch->level)+5)  ),
                                                number_range( 20, ((ch->level)+20) ),
                                                number_range( 50, ((ch->level)+50)) );
                obj_to_obj( coins, hoard );

                /* put the quest token in it as well */
                int idx = number_range(0, NUM_QUEST_OBJQUEST - 1);
                int vnum = quest_objquest_vnums[idx];
                OBJ_DATA *questitem = create_object(
                    get_obj_index(vnum),
                    ch->level, "common", CREATED_NO_RANDOMISER );
                set_obj_owner( questitem, ch->name );
                questitem->timer = hoard->timer;
                obj_to_obj( questitem, hoard );

                /* drop the hoard into the chosen room */
                obj_to_room( hoard, room );

                /* remember where we put it so do_ostat/mpoload can find it if needed */
                ch->pcdata->questobj  = questitem->pIndexData->vnum;
                ch->pcdata->questroom = room;
                ch->pcdata->questarea = room->area;

                populate_hoard_loot( ch, hoard );

                add_hoard_hint(room);

                sprintf( buf,
                    "\n\r{c%s intones in a voice like distant thunder, \n\r"
                    "'Legends tell of a lost hoard, buried deep beneath the earth near {C%s{x{c, \n\r"
                    "at the edge of the ancient realm of {C%s{x{c.  Hie thee swiftly, brave soul, \n\r"
                    "for the earth’s secret will stir but for a heartbeat...'{x\n\r\n\r",
                    capitalize( questman->short_descr ),
                    room->name,
                    room->area->name );
                send_to_char( buf, ch );
                return;
            }

            case 1: /* OBJECT RETRIEVAL QUEST */
            {
                int objvnum = 0;

                switch ( number_range(0, 9) )
                {
                    case 0: objvnum = QUEST_OBJQUEST1;  break;
                    case 1: objvnum = QUEST_OBJQUEST2;  break;
                    case 2: objvnum = QUEST_OBJQUEST3;  break;
                    case 3: objvnum = QUEST_OBJQUEST4;  break;
                    case 4: objvnum = QUEST_OBJQUEST5;  break;
                    case 5: objvnum = QUEST_OBJQUEST6;  break;
                    case 6: objvnum = QUEST_OBJQUEST7;  break;
                    case 7: objvnum = QUEST_OBJQUEST8;  break;
                    case 8: objvnum = QUEST_OBJQUEST9;  break;
                    case 9: objvnum = QUEST_OBJQUEST10; break;
                }

                questitem = create_object(get_obj_index(objvnum), ch->level, "common", CREATED_NO_RANDOMISER);
                questitem->timer = 240;
                set_obj_owner(questitem, ch->name);

                /* NB: uses the earlier-found 'room' tied to the chosen victim/area */
                obj_to_room(questitem, room);
                ch->pcdata->questobj  = questitem->pIndexData->vnum;
                ch->pcdata->questroom = room;
                ch->pcdata->questarea = room->area;

                sprintf (buf, "\n\r{c%s says, 'Vile pilferers have stolen {C%s{x{c from the royal "
                        "treasury!  My court wizardess, with her magic mirror, has pinpointed its "
                        "location.  I ask that you try your utmost to recover this artefact.  "
                        "Look in the general area of {C%s{x{c for {C%s{x{c!'{x\n\r\n\r",
                        capitalize (questman->short_descr),
                        questitem->short_descr,
                        ch->pcdata->questarea->name,
                        ch->pcdata->questroom->name);

                send_paragraph_to_char(buf, ch, 0);
                return;
            }

            default: /* KILL QUEST */
            {
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
                                ch->pcdata->questroom->name,
                                ch->pcdata->questarea->name);
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
                                ch->pcdata->questarea->name,
                                ch->pcdata->questroom->name);
                        break;
                }

                ch->pcdata->questmob = victim->pIndexData->vnum;
                send_paragraph_to_char(buf, ch, 0);
                break;
            }
        }
}

/* Append the hoard hint once, freeing the old description to avoid leaks. */
void add_hoard_hint(ROOM_INDEX_DATA *room)
{
    if (!room || !room->description)
        return;

    if (strstr(room->description, HOARD_HINT)) /* already there */
        return;

    size_t oldlen = strlen(room->description);
    size_t addlen = strlen(HOARD_HINT);
    char *newdesc = malloc(oldlen + addlen + 1);
    if (!newdesc) return; /* fail quietly */

    memcpy(newdesc, room->description, oldlen);
    memcpy(newdesc + oldlen, HOARD_HINT, addlen + 1);

    free_string(room->description);            /* ROM/Envy string free */
    room->description = str_dup(newdesc);      /* ROM/Envy string dup */
    free(newdesc);
}

/* Remove the *last* occurrence of the hoard hint from the room description. */
void remove_hoard_hint(ROOM_INDEX_DATA *room)
{
    if (!room || !room->description) return;

    const char *hint = HOARD_HINT;
    size_t      hlen = strlen(hint);

    /* find last occurrence */
    char *desc = room->description;
    char *last = NULL, *scan = strstr(desc, hint);
    while (scan) { last = scan; scan = strstr(scan + 1, hint); }
    if (!last) return;

    size_t pre  = (size_t)(last - desc);
    size_t post = strlen(last + hlen);

    char *newdesc = malloc(pre + post + 1);
    if (!newdesc) return;

    memcpy(newdesc, desc, pre);
    memcpy(newdesc + pre, last + hlen, post);
    newdesc[pre + post] = '\0';

    free_string(room->description);
    room->description = str_dup(newdesc);
    free(newdesc);
}

/* Utility: does this room have any *other* buried hoard? */
bool room_has_other_buried_hoard(ROOM_INDEX_DATA *room, OBJ_DATA *exclude)
{
    for (OBJ_DATA *o = room ? room->contents : NULL; o; o = o->next_content)
        if (o != exclude && o->item_type == ITEM_HOARD && o->value[1] == 1) /* 1 = buried */
            return TRUE;
    return FALSE;
}

/* Choose a damage type for hoard traps.
   Weighted towards curse/hex/spirit, with a mix of classics. */
int roll_hoard_trap_type(int for_level)
{
    int r = number_range(1, 100);

    /*  1–20  : direct damage (fire/cold/acid/energy)
        21–35 : poison/snare
        36–55 : blunt/pierce/slash
        56–75 : curse
        76–88 : hex
        89–100: spirit guardian
    */
    if (r <= 20) {
        switch (number_range(0,3)) {
            case 0: return TRAP_DAM_FIRE;
            case 1: return TRAP_DAM_COLD;
            case 2: return TRAP_DAM_ACID;
            default:return TRAP_DAM_ENERGY;
        }
    }
    else if (r <= 35) {
        return (number_bits(1) ? TRAP_DAM_POISON : TRAP_DAM_SNARE);
    }
    else if (r <= 55) {
        switch (number_range(0,2)) {
            case 0: return TRAP_DAM_BLUNT;
            case 1: return TRAP_DAM_PIERCE;
            default:return TRAP_DAM_SLASH;
        }
    }
    else if (r <= 75) {
        return TRAP_DAM_CURSE;
    }
    else if (r <= 88) {
        return TRAP_DAM_HEX;
    }
    else {
        return TRAP_DAM_SPIRIT;
    }
}

/* Arm a hoard with a random trap that triggers on OPEN (i.e. when unearthed). */

void maybe_arm_hoard_trap(OBJ_DATA *hoard, int level)
{
    if (!hoard)
        return;

    /* Only trap quest hoards. Bail if this isn’t the right prototype. */
    if (!hoard->pIndexData || hoard->pIndexData->vnum != QUEST_HOARD)
        return;

    /* Small chance overall */
    if (number_percent() > HOARD_TRAP_CHANCE)
        return;

    /* Clean slate (in case the prototype ever carries trap bits) */
    REMOVE_BIT(hoard->extra_flags, ITEM_TRAP);
    hoard->trap_dam    = 0;
    hoard->trap_eff    = 0;
    hoard->trap_charge = 0;

    /* Arm it */
    SET_BIT(hoard->extra_flags, ITEM_TRAP);
    hoard->trap_dam    = roll_hoard_trap_type(level);
    hoard->trap_charge = number_range(1, 3);      /* a few pops */
    hoard->level       = URANGE(1, level, LEVEL_HERO - 1);

    /* Must trigger on “open” to work with do_dig -> checkopen() */
    SET_BIT(hoard->trap_eff, TRAP_EFF_OPEN);

    /* Sometimes make it room-wide (AoE) for spice */
    if (number_percent() <= HOARD_TRAP_ROOM_AOE)
        SET_BIT(hoard->trap_eff, TRAP_EFF_ROOM);
}

/**
 * sample 0–4 with a discrete normal(2,1) approximation:
 *   0 → 2.3%   1 → 24.2%   2 → 47.0%
 *   3 → 24.2%  4 → 2.3%
 */
static int sample_loot_count(void) {
    int r = number_range(1, 1000);
    if      (r <=  23) return 0;
    else if (r <= 265) return 1;
    else if (r <= 735) return 2;
    else if (r <= 977) return 3;
    else               return 4;
}

/* decide which tier to attempt this drop */
static int sample_drop_tier(void) {
    int r = number_range(1, 1000);
    if      (r <= THRESH_COMMON)   return RARITY_COMMON;
    else if (r <= THRESH_UNCOMMON) return RARITY_UNCOMMON;
    else if (r <= THRESH_RARE)     return RARITY_RARE;
    else if (r <= THRESH_EPIC)     return RARITY_EPIC;
    else                            return RARITY_LEGENDARY;
}

/* map an item’s score into a tier */
static int item_score_to_tier(int score) {
    if      (score < ITEM_SCORE_UNCOMMON)   return RARITY_COMMON;
    else if (score < ITEM_SCORE_RARE)       return RARITY_UNCOMMON;
    else if (score < ITEM_SCORE_EPIC)       return RARITY_RARE;
    else if (score < ITEM_SCORE_LEGENDARY)  return RARITY_EPIC;
    else                                     return RARITY_LEGENDARY;
}

/* fill the hoard with 1–3 extra loot items */
void populate_hoard_loot(CHAR_DATA *ch, OBJ_DATA *hoard) {
    int num_items = sample_loot_count();
    int capacity  = hoard->value[0];
    int used_wt   = 0;
    /* bug("Num items is %d", num_items); */

    for (int i = 0; i < num_items; i++) {
        int target_tier = sample_drop_tier();
        OBJ_INDEX_DATA *idx = NULL;
        OBJ_DATA       *tmp = NULL;
        int attempt = 0;

        /* try up to 1000 times for a matching item */
        while (++attempt < 1000) {
            int vnum = number_range(1, MAX_QUEST_OBJ_VNUM);
            idx = get_obj_index(vnum);
            if (!idx) continue;

            /* 1) whitelisted type? */
            bool ok = FALSE;
            for (int w = 0; w < hoard_loot_whitelist_size; w++)
                if (idx->item_type == hoard_loot_whitelist[w]) {
                    ok = TRUE; break;
                }
            if (!ok) continue;

            /* 1a) must be takeable */
            if (!IS_SET(idx->wear_flags, ITEM_TAKE))
                continue;

            /* 1b) exclude body parts */
            if (IS_SET(idx->extra_flags, ITEM_BODY_PART))
                continue;

            /* 2) level within [ch->level-5 .. ch->level] */
            if (idx->level > ch->level || idx->level < ch->level - 5)
                continue;

            /* 3) create a temp object to calc its score */
            tmp = create_object(idx, ch->level, "common", CREATED_NO_RANDOMISER);
            int score = calc_item_score(tmp);
            int tier  = item_score_to_tier(score);

            /* discard if not our sampled tier */
            if (tier != target_tier) {
                extract_obj(tmp);
                continue;
            }

            /* 4) weight check */
            int wgt = get_obj_weight(tmp);
            if (used_wt + wgt > capacity) {
                extract_obj(tmp);
                continue;
            }

            /* found it */
            used_wt += wgt;
            obj_to_obj(tmp, hoard);
            break;
        }
        /* if no idx found in 1000 tries, stop adding more */
        if (!idx) break;
    }

    /*
     * Gem Generation for Hoards
     *
     * Quality scales with character level using Fibonacci-style brackets:
     * 1-20: Dull, 21-40: Cloudy, 41-60: Clear, 61-80: Brilliant, 81+: Flawless
     * Chance for a gem to drop: ~25% per quest, scaling slightly with level.
     */
    if (number_percent() <= 25 + (ch->level / 10))
    {
        OBJ_INDEX_DATA *gem_idx;
        OBJ_DATA *gem;
        int gem_type;
        int gem_quality;
        char buf[MAX_STRING_LENGTH];

        /* Determine gem quality based on level */
        if (ch->level >= 81)
            gem_quality = GEM_QUALITY_FLAWLESS;
        else if (ch->level >= 61)
            gem_quality = GEM_QUALITY_BRILLIANT;
        else if (ch->level >= 41)
            gem_quality = GEM_QUALITY_CLEAR;
        else if (ch->level >= 21)
            gem_quality = GEM_QUALITY_CLOUDY;
        else
            gem_quality = GEM_QUALITY_DULL;

        /* Small chance for higher quality: roll again */
        if (gem_quality < GEM_QUALITY_FLAWLESS && number_percent() <= 15)
            gem_quality++;

        /* Random gem type */
        gem_type = number_range(0, GEM_TYPE_MAX - 1);

        /* Create the gem object from generic template */
        gem_idx = get_obj_index(2);
        if (gem_idx != NULL)
        {
            gem = create_object(gem_idx, ch->level, "common", CREATED_SKILL);
            gem->item_type = ITEM_GEM;
            gem->value[0] = gem_type;
            gem->value[1] = gem_quality;
            gem->weight = 1;
            gem->cost = (gem_quality + 1) * 100;

            free_string(gem->name);
            sprintf(buf, "gem %s %s",
                gem_quality_name(gem_quality),
                gem_type_name(gem_type));
            gem->name = str_dup(buf);

            free_string(gem->short_descr);
            sprintf(buf, "a %s %s (%s %+d)",
                gem_quality_name(gem_quality),
                gem_type_name(gem_type),
                affect_loc_name(gem_table[gem_type].apply_type),
                get_gem_bonus(gem_type, gem_quality));
            gem->short_descr = str_dup(buf);

            free_string(gem->description);
            sprintf(buf, "A %s %s lies here, glinting softly.",
                gem_quality_name(gem_quality),
                gem_type_name(gem_type));
            gem->description = str_dup(buf);

            obj_to_obj(gem, hoard);
        }
    }
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
                                send_to_char ("{CYou may now quest again.{x\n\r", ch);
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
