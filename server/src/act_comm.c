/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
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

#if defined(macintosh)
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

/*
 * Local functions.
 */
bool is_note_to args((CHAR_DATA * ch, NOTE_DATA *pnote));
void note_attach args((CHAR_DATA * ch));
void note_remove args((CHAR_DATA * ch, NOTE_DATA *pnote));
void talk_channel args((CHAR_DATA * ch, char *argument, int channel, const char *verb));

/*
 * 'Review' command channel buffers; Gez 2001
 */
struct review_data review_buffer_public[REVIEW_CHANNEL_COUNT][REVIEW_BUFFER_LENGTH];
struct review_data review_buffer_clan[MAX_CLAN][REVIEW_BUFFER_LENGTH];

int chan(int channel)
{
        int c;

        switch (channel)
        {
        case CHANNEL_DIRTALK:
                c = COLOR_DIRTALK;
                break;
        case CHANNEL_IMMTALK:
                c = COLOR_IMMTALK;
                break;
        case CHANNEL_CHAT:
                c = COLOR_GOSSIP;
                break;
        case CHANNEL_AUCTION:
                c = COLOR_AUCTION;
                break;
        case CHANNEL_INFO:
                c = COLOR_AUCTION;
                break;
        case CHANNEL_CLAN:
                c = COLOR_CLAN;
                break;
        case CHANNEL_SHOUT:
                c = COLOR_SHOUT;
                break;
        case CHANNEL_MUSIC:
                c = COLOR_AUCTION;
                break;
        case CHANNEL_QUESTION:
                c = COLOR_AUCTION;
                break;
        case CHANNEL_YELL:
                c = COLOR_SHOUT;
                break;
        case CHANNEL_SERVER:
                c = COLOR_SERVER;
                break;
        case CHANNEL_ARENA:
                c = COLOR_ARENA;
                break;
        case CHANNEL_NEWBIE:
                c = COLOR_NEWBIE;
                break;

        default:
                bug("Chan: invalid code %d", channel);
                c = 0;
                break;
        }

        return c;
}

/*
 * Generic channel function.
 */
void talk_channel(CHAR_DATA *ch, char *argument, int channel, const char *verb)
{
        DESCRIPTOR_DATA *d;
        char buf[MAX_STRING_LENGTH];
        char buf2[MAX_STRING_LENGTH];
        int position;
        char *verb2;

        if (argument[0] == '\0')
        {
                sprintf(buf, "%s what?\n\r", verb);
                buf[0] = UPPER(buf[0]);
                return;
        }

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) && channel != CHANNEL_INFO)
        {
                sprintf(buf, "You can't %s.\n\r", verb);
                send_to_char(buf, ch);
                return;
        }

        if (IS_NPC(ch) && ch->master && !IS_NPC(ch->master) && IS_SET(ch->master->act, PLR_SILENCE))
        {
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL) && channel != CHANNEL_INFO)
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        REMOVE_BIT(ch->deaf, channel);

        if (channel != CHANNEL_INFO)
                ch->silent_mode = 0;

        if (channel == CHANNEL_INFO)
                sprintf(buf, "<<INFO>: $t");

        else if (channel == CHANNEL_ARENA)
                sprintf(buf, "<<Arena> $t");

        else if (channel == CHANNEL_IMMTALK)
        {
                sprintf(buf, "%s$c: $t$R",
                        color_table_8bit[get_colour_index_by_code(ch->colors[chan(channel)])].act_code);

                position = ch->position;
                ch->position = POS_STANDING;
                act(buf, ch, argument, NULL, TO_CHAR);
                ch->position = position;
                sprintf(buf, "$c: $t$R");
        }

        else if (channel == CHANNEL_DIRTALK)
        {
                sprintf(buf, "%s$c-| $t$R",
                        color_table_8bit[get_colour_index_by_code(ch->colors[chan(channel)])].act_code);
                position = ch->position;
                ch->position = POS_STANDING;
                act(buf, ch, argument, NULL, TO_CHAR);
                ch->position = position;
                sprintf(buf, "$c-| $t$R");
        }

        else
        {
                switch (argument[strlen(argument) - 1])
                {
                default:
                        verb2 = "";
                        break;
                case '!':
                        verb2 = " exclaiming";
                        break;
                case '?':
                        verb2 = " asking";
                        break;
                }
                sprintf(buf, "%sYou %s%s '$t'$R",
                        color_table_8bit[get_colour_index_by_code(ch->colors[chan(channel)])].act_code, verb, verb2);
                position = ch->position;
                ch->position = POS_STANDING;
                act(buf, ch, argument, NULL, TO_CHAR);
                ch->position = position;

                sprintf(buf, "$c %ss%s '$t'$R", verb, verb2);
        }

        for (d = descriptor_list; d; d = d->next)
        {
                CHAR_DATA *och;
                CHAR_DATA *vch;

                och = d->original ? d->original : d->character;
                vch = d->character;

                if (d->connected == CON_PLAYING && vch != ch && !IS_SET(och->deaf, channel) && !och->silent_mode)
                {
                        if (channel == CHANNEL_IMMTALK && !IS_HERO(och))
                                continue;
                        if (channel == CHANNEL_DIRTALK && get_trust(och) < L_SEN)
                                continue;
                        if (channel == CHANNEL_CLAN && !is_same_clan(vch, ch))
                                continue;
                        if (channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area)
                                continue;
                        if (channel == CHANNEL_NEWBIE && och->level > 4 && och->level < LEVEL_HERO && !IS_SET(och->act, PLR_GUIDE))
                                continue;

                        position = vch->position;
                        if (channel != CHANNEL_SHOUT && channel != CHANNEL_YELL)
                                vch->position = POS_STANDING;

                        sprintf(buf2, "%s%s$R", color_table_8bit[get_colour_index_by_code(vch->colors[chan(channel)])].act_code, buf);

                        act(buf2, ch, argument, vch, TO_VICT);
                        /* Play a channel notification sound for the recipient (if their settings allow). */
                        media_notify_channel(vch, channel);
                        vch->position = position;
                }
        }

        return;
}

void do_chat(CHAR_DATA *ch, char *argument)
{

        /*        if (ch->level < 5)
                {
                        send_to_char("You must be level 5 to chat, use NEWBIE instead.\n\r", ch);
                        return;
                }
        */

        talk_channel(ch, argument, CHANNEL_CHAT, "chat");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_public_review_buffer(ch, REVIEW_CHAT, argument);
}

void do_newbie(CHAR_DATA *ch, char *argument)
{
        if (ch->level > 4 && ch->level < LEVEL_HERO && !IS_SET(ch->act, PLR_GUIDE))
        {
                send_to_char("You're not a newbie!\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_NEWBIE, "newbie");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_public_review_buffer(ch, REVIEW_NEWBIE, argument);
}

void do_music(CHAR_DATA *ch, char *argument)
{
        if (ch->level < 5)
        {
                send_to_char("You must be level 5 to music, use NEWBIE instead.\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_MUSIC, "music");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_public_review_buffer(ch, REVIEW_MUSIC, argument);
}

void do_question(CHAR_DATA *ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_QUESTION, "question");
        return;
}

void do_answer(CHAR_DATA *ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_QUESTION, "answer");
        return;
}

void do_shout(CHAR_DATA *ch, char *argument)
{
        if (ch->level < 5)
        {
                send_to_char("You must be level 5 to shout, use NEWBIE instead.\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_SHOUT, "shout");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_public_review_buffer(ch, REVIEW_SHOUT, argument);
}

void do_yell(CHAR_DATA *ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_YELL, "yell");
        return;
}

void do_clantalk(CHAR_DATA *ch, char *argument)
{
        if (!is_clan(ch) || clan_table[ch->clan].independent)
        {
                send_to_char("You aren't in a clan.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char("Tell your Clan what?\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_CLAN, "clan-talk");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_clan_review_buffer(ch, ch->clan, argument);
}

void do_immtalk(CHAR_DATA *ch, char *argument)
{
        if (!IS_NPC(ch) && ch->pcdata->learned[gsn_immtalk] < 100)
        {
                send_to_char("You are not authorized to use this command.\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_IMMTALK, "immtalk");
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                return;
        }
        add_to_public_review_buffer(ch, REVIEW_IMMTALK, argument);
}

void do_dirtalk(CHAR_DATA *ch, char *argument)
{
        if (!IS_NPC(ch) && ch->pcdata->learned[gsn_dirtalk] < 100)
        {
                send_to_char("You are not authorized to use this command.\n\r", ch);
                return;
        }

        talk_channel(ch, argument, CHANNEL_DIRTALK, "dirtalk");
        add_to_public_review_buffer(ch, REVIEW_DIRTALK, argument);
}

void do_info(CHAR_DATA *ch, char *argument)
{
        talk_channel(ch, argument, CHANNEL_INFO, NULL);
        add_to_public_review_buffer(NULL, REVIEW_INFO, argument);

        if (argument && *argument)
                json_write_events(ch, argument);
}

void tournament_message(char *text, CHAR_DATA *ch)
{
        talk_channel(ch, text, CHANNEL_ARENA, NULL);
        add_to_public_review_buffer(NULL, REVIEW_ARENA, text);

        if (text && *text)
                json_write_events(ch, text);
}

void do_say(CHAR_DATA *ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *mob;
        char *verb;

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                send_to_char("You are mute. Sssh.\n\r", ch);
                return;
        }

        if (IS_NPC(ch) && !CAN_SPEAK(ch))
        {
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char_bw("Say what?\n\r", ch);
                return;
        }

        switch (argument[strlen(argument) - 1])
        {
        default:
                verb = "say";
                break;
        case '!':
                verb = "exclaim";
                break;
        case '?':
                verb = "ask";
                break;
        }

        sprintf(buf, "%sYou %s '$t'$R", color_table_8bit[get_colour_index_by_code(ch->colors[COLOR_SAY])].act_code, verb);

        if (ch->desc)
        {
                act(buf, ch, argument, NULL, TO_CHAR);
                MOBtrigger = FALSE;
        }

        for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
        {
                sprintf(buf, "%s%s %ss '$t'$R",
                        color_table_8bit[get_colour_index_by_code(mob->colors[COLOR_SAY])].act_code,
                        capitalize_initial(PERS(ch, mob)), verb);

                if (mob != ch && mob->desc != NULL)
                {
                        act(buf, mob, argument, NULL, TO_CHAR);
                        MOBtrigger = FALSE;
                }
                if (IS_NPC(ch) && IS_NPC(mob) && mob->pIndexData->vnum == ch->pIndexData->vnum)
                        continue;
        }

        mprog_speech_trigger(argument, ch);
        return;
}

void do_tell(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int position;

        if (!IS_NPC(ch) && (IS_SET(ch->act, PLR_SILENCE) || ch->silent_mode || IS_SET(ch->act, PLR_NO_TELL)))
        {
                send_to_char("Your message didn't get through.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0' || argument[0] == '\0')
        {
                send_to_char("Tell whom what?\n\r", ch);
                return;
        }

        if (!(victim = get_char_world(ch, arg)) || (IS_NPC(victim) && victim->in_room != ch->in_room))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (!IS_TRUE_IMMORTAL(ch) && (victim->silent_mode || IS_SET(victim->act, PLR_NO_TELL)))
        {
                act("$N's mobile is off or outside the coverage area.", ch, 0, victim, TO_CHAR);
                return;
        }

        if (!IS_TRUE_IMMORTAL(ch) && victim->fighting)
        {
                act("$N's fighting right now, try again later.", ch, 0, victim, TO_CHAR);
                return;
        }

        sprintf(buf, "%sYou tell $N '$t'$R",
                color_table_8bit[get_colour_index_by_code(ch->colors[COLOR_TELL])].act_code);
        act(buf, ch, argument, victim, TO_CHAR);

        sprintf(buf, "%s$C tells you '$t'$R",
                color_table_8bit[get_colour_index_by_code(victim->colors[COLOR_TELL])].act_code);

        position = victim->position;
        victim->position = POS_STANDING;
        act(buf, victim, argument, ch, TO_CHAR);
        victim->position = position;
        victim->reply = ch;

        if (IS_SET(victim->act, PLR_AFK))
                act("Just so you know, $E is AFK.", ch, NULL, victim, TO_CHAR);

        return;
}

void do_reply(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char buf[MAX_STRING_LENGTH];
        int position;

        if (!IS_NPC(ch) && (IS_SET(ch->act, PLR_SILENCE)))
        {
                send_to_char("Your message didn't get through.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (!(victim = ch->reply))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char("Reply what?\n\r", ch);
                return;
        }

        if (!IS_IMMORTAL(ch) && !IS_AWAKE(victim))
        {
                act("$E can't hear you.", ch, 0, victim, TO_CHAR);
                return;
        }

        sprintf(buf, "%sYou tell $N '$t'$R",
                color_table_8bit[get_colour_index_by_code(ch->colors[COLOR_TELL])].act_code);
        act(buf, ch, argument, victim, TO_CHAR);
        position = victim->position;
        victim->position = POS_STANDING;

        sprintf(buf, "%s$c tells you '$t'$R",
                color_table_8bit[get_colour_index_by_code(ch->colors[COLOR_TELL])].act_code);
        act(buf, ch, argument, victim, TO_VICT);
        victim->position = position;
        victim->reply = ch;

        return;
}

void do_emote(CHAR_DATA *ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];
        char *plast;

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                send_to_char("You are mute. Sssh.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch))
        {
                if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                {
                        send_to_char("Not in your current form.\n\r", ch);
                        return;
                }

                if (IS_SET(ch->act, PLR_NO_EMOTE))
                {
                        send_to_char("You can't show your emotions.\n\r", ch);
                        return;
                }
        }

        if (argument[0] == '\0')
        {
                send_to_char("Emote what?\n\r", ch);
                return;
        }

        for (plast = argument; *plast != '\0'; plast++)
                ;

        strcpy(buf, argument);
        if (isalpha(plast[-1]))
                strcat(buf, ".");

        act("$n $T", ch, NULL, buf, TO_ROOM);
        MOBtrigger = FALSE;
        act("$n $T", ch, NULL, buf, TO_CHAR);

        return;
}

/*
 * All the posing stuff.
 */
struct pose_table_type
{
        char *message[2 * MAX_CLASS];
};

const struct pose_table_type pose_table[] =
    {
        {{"You sizzle with energy.",
          "$n sizzles with energy.",
          "You feel very holy.",
          "$n looks very holy.",
          "You perform a small card trick.",
          "$n performs a small card trick.",
          "You show your bulging muscles.",
          "$n shows $s bulging muscles.",
          "Stop it with the Ouija board, will ya?",
          "Great, $n is playing with $s Ouija board again.",
          "You show your bulging muscles.",
          "$n shows $s bulging muscles.",
          "You slip into the shadows.",
          "$n slips into the shadows."}},

        {{"You turn into a butterfly, then return to your normal shape.",
          "$n turns into a butterfly, then returns to $s normal shape.",
          "You nonchalantly turn wine into water.",
          "$n nonchalantly turns wine into water.",
          "You wiggle your ears alternately.",
          "$n wiggles $s ears alternately.",
          "You crack nuts between your fingers.",
          "$n cracks nuts between $s fingers.",
          "You read everyone's mind....and shudder with disgust.",
          "$n reads your mind...eww, you pervert!",
          "You wiggle your ears alternately.",
          "$n wiggles $s ears alternately.",
          "You swallow your sword.",
          "$n swallows $s sword."

        }},

        {{"Blue sparks fly from your fingers.",
          "Blue sparks fly from $n's fingers.",
          "A halo appears over your head.",
          "A halo appears over $n's head.",
          "You nimbly tie yourself into a knot.",
          "$n nimbly ties $mself into a knot.",
          "You grizzle your teeth and look mean.",
          "$n grizzles $s teeth and looks mean.",
          "You show everyone your awards for perfect school attendance",
          "You aren't impressed by $n's school attendance awards.  Geek.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You juggle knives",
          "$n juggles 5 knives"}},

        {{"Little red lights dance in your eyes.",
          "Little red lights dance in $n's eyes.",
          "You recite words of wisdom.",
          "$n recites words of wisdom.",
          "You juggle with daggers, apples, and eyeballs.",
          "$n juggles with daggers, apples, and eyeballs.",
          "You hit your head, and your eyes roll.",
          "$n hits $s head, and $s eyes roll.",
          "A will-o-the-wisp arrives with your slippers.",
          "A will-o-the-wisp arrives with $n's slippers.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You juggle with knives.",
          "$n juggles with $n knives."}},

        {{"A slimy green monster appears before you and bows.",
          "A slimy green monster appears before $n and bows.",
          "Deep in prayer, you levitate.",
          "Deep in prayer, $n levitates.",
          "You steal the underwear off every person in the room.",
          "Your underwear is gone!  $n stole it!",
          "Crunch, crunch -- you munch a bottle.",
          "Crunch, crunch -- $n munches a bottle.",
          "What's with the extra leg?",
          "Why did $n sprout an extra leg just now?",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You steal the underwear off every person in the room.",
          "Your underwear is gone!  $n stole it!"}},

        {{"You turn everybody into a little pink elephant.",
          "You are turned into a little pink elephant by $n.",
          "An angel consults you.",
          "An angel consults $n.",
          "The dice roll ... and you win again.",
          "The dice roll ... and $n wins again.",
          "... 98, 99, 100 ... you do push-ups.",
          "... 98, 99, 100 ... $n does push-ups.",
          "The spoons flee as you begin to concentrate.",
          "The spoons flee as $n begins to concentrate.",
          "You pull a rabbit out of your pants.",
          "$n pulls a rabbit from $s pants.",
          "You swiftly move around the room and sweep them off their feet",
          "$n knocks you off your feet!"}},

        {{"A small ball of light dances on your fingertips.",
          "A small ball of light dances on $n's fingertips.",
          "Your body glows with an unearthly light.",
          "$n's body glows with an unearthly light.",
          "You count the money in everyone's pockets.",
          "Check your money, $n is counting it.",
          "Arnold Schwarzenegger admires your physique.",
          "Arnold Schwarzenegger admires $n's physique.",
          "Stop wiggling your brain at people.",
          "Make $n stop wiggling $s brain at you!",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s weapon at you, splitting your hair!"}},

        {{"Smoke and fumes leak from your nostrils.",
          "Smoke and fumes leak from $n's nostrils.",
          "A spotlight hits you.",
          "A spotlight hits $n.",
          "You balance a pocket knife on your tongue.",
          "$n balances a pocket knife on your tongue.",
          "Watch your feet, you are juggling granite boulders.",
          "Watch your feet, $n is juggling granite boulders.",
          "MENSA called...they want your opinion on something.",
          "MENSA just called $n for consultation.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s weapon at you, splitting your hair!"}},

        {{"The light flickers as you rap in magical languages.",
          "The light flickers as $n raps in magical languages.",
          "Everyone levitates as you pray.",
          "You levitate as $n prays.",
          "You produce a coin from everyone's ear.",
          "$n produces a coin from your ear.",
          "Oomph!  You squeeze water out of a granite boulder.",
          "Oomph!  $n squeezes water out of a granite boulder.",
          "Chairs fly around the room at your slightest whim.",
          "Chairs fly around the room at $n's slightest whim.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s weapon at you, splitting your hair!"}},

        {{"Your head disappears.",
          "$n's head disappears.",
          "A cool breeze refreshes you.",
          "A cool breeze refreshes $n.",
          "You step behind your shadow.",
          "$n steps behind $s shadow.",
          "You pick your teeth with a spear.",
          "$n picks $s teeth with a spear.",
          "Oof...maybe you shouldn't summon any more hippopotamuses.",
          "Oof!  Guess $n won't be summoning any more hippos for a while.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"A fire elemental singes your hair.",
          "A fire elemental singes $n's hair.",
          "The sun pierces through the clouds to illuminate you.",
          "The sun pierces through the clouds to illuminate $n.",
          "Your eyes dance with greed.",
          "$n's eyes dance with greed.",
          "Everyone is swept off their foot by your hug.",
          "You are swept off your feet by $n's hug.",
          "Oops...your hair is sizzling from thinking too hard.",
          "Oops...$n's hair is sizzling from thinking too hard.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"The sky changes color to match your eyes.",
          "The sky changes color to match $n's eyes.",
          "The ocean parts before you.",
          "The ocean parts before $n.",
          "You deftly steal everyone's weapon.",
          "$n deftly steals your weapon.",
          "Your karate chop splits a tree.",
          "$n's karate chop splits a tree.",
          "What?  You were too busy concentrating.",
          "What?  Oh, $n was lost in thought...again.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"The stones dance to your command.",
          "The stones dance to $n's command.",
          "A thunder cloud kneels to you.",
          "A thunder cloud kneels to $n.",
          "The Grey Mouser buys you a beer.",
          "The Grey Mouser buys $n a beer.",
          "A strap of your armor breaks over your mighty thews.",
          "A strap of $n's armor breaks over $s mighty thews.",
          "Will you get down here before you get hurt?",
          "Quick, get a stick, $n is doing $s pinata impression again.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"The heavens and grass change colour as you smile.",
          "The heavens and grass change colour as $n smiles.",
          "The Burning Man speaks to you.",
          "The Burning Man speaks to $n.",
          "Everyone's pocket explodes with your fireworks.",
          "Your pocket explodes with $n's fireworks.",
          "A boulder cracks at your frown.",
          "A boulder cracks at $n's frown.",
          "Careful...don't want to disintegrate anyone!",
          "LOOK OUT!  $n is trying to disintegrate something!",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"Everyone's clothes are transparent, and you are laughing.",
          "Your clothes are transparent, and $n is laughing.",
          "An eye in a pyramid winks at you.",
          "An eye in a pyramid winks at $n.",
          "Everyone discovers your dagger a centimetre from their eye.",
          "You discover $n's dagger a centimetre from your eye.",
          "Mercenaries arrive to do your bidding.",
          "Mercenaries arrive to do $n's bidding.",
          "You run off at the mouth about 'mind over matter'.",
          "Yeah, yeah, mind over matter.  Shut up, $n.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"A black hole swallows you.",
          "A black hole swallows $n.",
          "Valentine Michael Smith offers you a glass of water.",
          "Valentine Michael Smith offers $n a glass of water.",
          "Where did you go?",
          "Where did $n go?",
          "Four matched Percherons bring in your chariot.",
          "Four matched Percherons bring in $n's chariot.",
          "Thud.",
          "Thud.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}},

        {{"The world shimmers in time with your whistling.",
          "The world shimmers in time with $n's whistling.",
          "The great god Mota gives you a staff.",
          "The great god Mota gives $n a staff.",
          "Click.",
          "Click.",
          "Atlas asks you to relieve him.",
          "Atlas asks $n to relieve him.",
          "You charm the pants off everyone...and refuse to give them back.",
          "Your pants are charmed off by $n, and $e won't give them back.",
          "You pull a rabbit out of you pants.",
          "$n pulls a rabbit from $s pants.",
          "You throw your katana at everyone.",
          "$n throws $s at you, splitting your hair!"}}};

void do_pose(CHAR_DATA *ch, char *argument)
{
        int level;
        int pose;

        if (IS_NPC(ch))
                return;

        level = UMIN(ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1);
        pose = number_range(0, level);

        act(pose_table[pose].message[2 * ch->class + 0], ch, NULL, NULL, TO_CHAR);
        act(pose_table[pose].message[2 * ch->class + 1], ch, NULL, NULL, TO_ROOM);

        return;
}

void do_bug(CHAR_DATA *ch, char *argument)
{
        append_file(ch, BUG_FILE, argument);
        send_to_char("Ok.  Thanks.\n\r", ch);
        return;
}

void do_idea(CHAR_DATA *ch, char *argument)
{
        append_file(ch, IDEA_FILE, argument);
        send_to_char("Ok.  Thanks.\n\r", ch);
        return;
}

void do_typo(CHAR_DATA *ch, char *argument)
{
        append_file(ch, TYPO_FILE, argument);
        send_to_char("Ok.  Thanks.\n\r", ch);
        return;
}

/* RT code to delete yourself */

void do_delet(CHAR_DATA *ch, char *argument)
{
        send_to_char("You must type the full command to delete yourself.\n\r", ch);
        return;
}

void do_delete(CHAR_DATA *ch, char *argument)
{
        char strsave[MAX_INPUT_LENGTH];
        char buf[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
        {
                send_to_char("Mobs can't delete themselves.\n\r", ch);
                return;
        }

        if (ch->pcdata->confirm_delete)
        {
                if (argument[0] != '\0')
                {
                        send_to_char("Delete status removed.\n\r", ch);
                        ch->pcdata->confirm_delete = FALSE;
                        return;
                }
                else
                {
#if !defined(macintosh) && !defined(MSDOS)
                        sprintf(strsave, "%s%s%s%s", PLAYER_DIR, initial(ch->name),
                                "/", capitalize(ch->name));
#else
                        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
#endif
                        do_quit(ch, "");
                        sprintf(buf, "rm %s", strsave);
                        sprintf(buf, "rm %s.vault", strsave);
                        system(buf);
                        unlink(strsave);
                        return;
                }
        }

        if (argument[0] != '\0')
        {
                send_to_char("Just type delete. No argument.\n\r", ch);
                return;
        }

        send_to_char("Type delete again to confirm this command.\n\r", ch);
        send_to_char("WARNING: this command is irreversible.\n\r", ch);
        send_to_char("Typing delete with an argument will undo delete status.\n\r", ch);
        ch->pcdata->confirm_delete = TRUE;
}

void do_rent(CHAR_DATA *ch, char *argument)
{
        send_to_char("There is no rent here.  Just save and quit.\n\r", ch);
        return;
}

void do_qui(CHAR_DATA *ch, char *argument)
{
        send_to_char("If you want to QUIT, you have to spell it out.\n\r", ch);
        return;
}

void do_quit(CHAR_DATA *ch, char *argument)
{
        DESCRIPTOR_DATA *d;
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *pobj;
        OBJ_DATA *pobj_next;

        if (IS_NPC(ch))
        {
                send_to_char("Wimping out eh?  Mobs can't quit!\n\r", ch);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char("No way! You are fighting.\n\r", ch);
                return;
        }

        if (ch->position < POS_STUNNED)
        {
                send_to_char("You're not DEAD yet... lie still!\n\r", ch);
                return;
        }

        /* Tournaments */
        if (is_entered_in_tournament(ch))
        {
                if (tournament_status == TOURNAMENT_STATUS_RUNNING && is_still_alive_in_tournament(ch))
                {
                        send_to_char("You can't quit from a tournament!  Recall to leave.\n\r", ch);
                        return;
                }
                else if (tournament_status != TOURNAMENT_STATUS_FINISHED)
                        remove_from_tournament(ch);
        }

        if (ch == tournament_host && tournament_status != TOURNAMENT_STATUS_NONE)
        {
                if (tournament_status != TOURNAMENT_STATUS_FINISHED)
                        tournament_message("The tournament has been cancelled!", ch);
                tournament_init();
        }

        /* check for auctioning items.... */
        if (auction->item)
        {
                if (ch == auction->buyer)
                {
                        send_to_char("Wait until you have bought the item on auction.\n\r", ch);
                        return;
                }
                else if (ch == auction->seller)
                {
                        send_to_char("Wait until you have sold the item on auction.\n\r", ch);
                        return;
                }
        }

        /* drop items above ch level or clan items */
        for (pobj = ch->carrying; pobj; pobj = pobj_next)
        {
                pobj_next = pobj->next_content;

                if (pobj->deleted)
                        continue;

                if (pobj->item_type == ITEM_CLAN_OBJECT || (ch->level < pobj->level && pobj->item_type != ITEM_KEY))
                {
                        obj_from_char(pobj);
                        obj_to_room(pobj, get_room_index(ROOM_VNUM_DONATION_4));
                }
        }

        /* check for mounted player */
        if (ch->mount)
                do_dismount(ch, "");

        send_to_char("You bid farewell to the Dragons Domain.\n\r", ch);
        send_to_char("{RDon't worry though, they'll be here, plotting their revenge...\n\r\n\r{x", ch);

        ch->pcdata->has_quit = TRUE; /* To suppress eq aff wearoffs */

        /* modified by geoff so wizinvis gods are not seen departing :)  */
        if (!IS_SET(ch->act, PLR_WIZINVIS))
        {
                act("$n has left the game.", ch, NULL, NULL, TO_ROOM);
                sprintf(buf, "%s has left the Domain of Dragons.", ch->name);
                do_info(ch, buf);
        }

        sprintf(log_buf, "%s has quit.", ch->name);
        log_string(log_buf);

        /* after extract_char the ch is no longer valid! */
        save_char_obj(ch);

        /* Free note that might be there somehow */
        if (ch->pcdata->in_progress)
                free_note(ch->pcdata->in_progress);

        d = ch->desc;

        /* Nullify descriptor's character pointer before extract_char to prevent
         * webgate from accessing freed memory after interpret() returns */
        if (d)
                d->character = NULL;

        extract_char(ch, TRUE);

        if (d)
                close_socket(d);

        return;
}

void do_save(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (ch->level < 2)
        {
                send_to_char("You must be at least second level to save.\n\r", ch);
                return;
        }

        save_char_obj(ch);
        send_to_char("Ok.\n\r", ch);
        return;
}

void do_follow(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Follow whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM) && ch->master)
        {
                act("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                if (!ch->master)
                {
                        send_to_char("You already follow yourself.\n\r", ch);
                        return;
                }
                stop_follower(ch);
                return;
        }

        /* Immortals can follow and be followed by whomsoever. -Owl 2/2/08 */

        if ((ch->level - victim->level < -11 || ch->level - victim->level > 11) && (!(ch->level > LEVEL_HERO)) && (!(victim->level > LEVEL_HERO)))
        {
                send_to_char("You are not of the right calibre to follow.\n\r", ch);
                return;
        }

        if (ch->master)
                stop_follower(ch);

        add_follower(ch, victim);
        return;
}

void add_follower(CHAR_DATA *ch, CHAR_DATA *master)
{
        if (ch->master)
        {
                bug("Add_follower: non-null master.", 0);
                return;
        }

        ch->master = master;
        ch->leader = NULL;

        if (can_see(master, ch) && master->mount != ch)
                act("$n now follows you.", ch, NULL, master, TO_VICT);

        act("You now follow $N.", ch, NULL, master, TO_CHAR);

        return;
}

void stop_follower(CHAR_DATA *ch)
{
        if (!ch->master)
        {
                bug("Stop_follower: null master.", 0);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM))
        {
                REMOVE_BIT(ch->affected_by, AFF_CHARM);
                affect_strip(ch, gsn_charm_person);
                affect_strip(ch, gsn_domination);
        }

        if (IS_NPC(ch) && ch->rider)
                strip_mount(ch->rider);

        if (can_see(ch->master, ch))
                act("$n stops following you.", ch, NULL, ch->master, TO_VICT);
        act("You stop following $N.", ch, NULL, ch->master, TO_CHAR);

        ch->master = NULL;
        ch->leader = NULL;

        if (!IS_NPC(ch))
                ch->pcdata->group_leader = NULL;

        return;
}

void die_follower(CHAR_DATA *ch, char *name)
{
        CHAR_DATA *fch;

        if (ch->master)
                stop_follower(ch);

        ch->leader = NULL;

        for (fch = char_list; fch; fch = fch->next)
        {
                if (fch->deleted)
                        continue;
                if (fch->master == ch)
                        stop_follower(fch);
                if (fch->leader == ch)
                        fch->leader = NULL;
                if (!IS_NPC(fch) && fch->pcdata->group_leader == ch)
                        fch->pcdata->group_leader = NULL;
        }

        return;
}

void do_order(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *och;
        CHAR_DATA *och_next;
        char arg[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        bool found;
        bool fAll;

        argument = one_argument(argument, arg);
        one_argument(argument, arg2);

        if (!str_prefix(arg2, "mpoload") || !str_prefix(arg2, "mptransfer") || !str_prefix(arg2, "mpmload") || !str_prefix(arg2, "mptransfergroup") || !str_prefix(arg2, "mpforce") || !str_prefix(arg2, "mpecho") || !str_prefix(arg2, "mpechoat") || !str_prefix(arg2, "mpechoaround") || !str_prefix(arg2, "mpkill") || !str_prefix(arg2, "mppurge") || !str_prefix(arg2, "mppeace") || !str_prefix(arg2, "oclanitem") || !str_prefix(arg2, "cast") || !str_prefix(arg2, "murder") || !str_prefix(arg2, "dirt") || !str_prefix(arg2, "flukeslap") || !str_prefix(arg2, "kick"))
        {
                return;
        }

        if (arg[0] == '\0' || argument[0] == '\0')
        {
                send_to_char("Order whom to do what?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM))
        {
                send_to_char("You feel like taking, not giving, orders.\n\r", ch);
                return;
        }

        if (!str_cmp(arg, "all"))
        {
                fAll = TRUE;
                victim = NULL;
        }
        else
        {
                fAll = FALSE;
                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }

                if (victim == ch)
                {
                        send_to_char("Aye aye, right away!\n\r", ch);
                        return;
                }

                if (!IS_NPC(victim) || victim->master != ch)
                {
                        send_to_char("Do it yourself!\n\r", ch);
                        return;
                }
        }

        found = FALSE;

        for (och = ch->in_room->people; och; och = och_next)
        {
                och_next = och->next_in_room;

                if (och->deleted)
                        continue;

                if (!IS_NPC(och))
                        continue;

                if (och->master == ch && (fAll || och == victim))
                {
                        found = TRUE;
                        act("$n orders you to '$t'.", ch, argument, och, TO_VICT);
                        if (!och->rider)
                                interpret(och, argument);
                }
        }

        if (found)
                send_to_char("Ok.\n\r", ch);
        else
                send_to_char("You have no followers here.\n\r", ch);

        return;
}

void do_group(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *gch;
        char buf[MAX_STRING_LENGTH];
        char buf1[MAX_INPUT_LENGTH];
        char arg[MAX_INPUT_LENGTH];
        int npc_count;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                CHAR_DATA *leader;

                leader = (ch->leader) ? ch->leader : ch;
                sprintf(buf1, "%s's group:", leader->name);
                sprintf(buf, "{W%-30s{x       Hits       Mana       Move  Exp needed\n\r"
                             "---------------------------------------------------------------------------\n\r",
                        buf1);
                send_to_char(buf, ch);

                for (gch = char_list; gch; gch = gch->next)
                {
                        if (gch->deleted)
                                continue;

                        if (is_same_group(gch, ch))
                        {
                                if (IS_NPC(gch))
                                {
                                        sprintf(buf, "[{W%3d Mob{x] {B%-21s{x {%c%4d{x/%4d  {%c%4d{x/%4d  {%c%4d{x/%4d\n\r",
                                                gch->level,
                                                capitalize_initial(PERS(gch, ch)),
                                                gch->hit < gch->max_hit / 10 ? 'R' : 'G',
                                                gch->hit, gch->max_hit,
                                                gch->mana < gch->max_mana / 10 ? 'R' : 'C',
                                                gch->mana, gch->max_mana,
                                                gch->move < gch->max_move / 10 ? 'R' : 'Y',
                                                gch->move, gch->max_move);
                                }
                                else
                                {
                                        sprintf(buf, "[{W%3d %s{x] {C%-12s{x{c%8s{x  {%c%4d{x/%4d  {%c%4d{x/%4d  {%c%4d{x/%4d  %10d\n\r",
                                                gch->level,
                                                IS_NPC(gch) ? "Mob" : gch->sub_class ? sub_class_table[gch->sub_class].who_name
                                                                                     : class_table[gch->class].who_name,
                                                capitalize_initial(PERS(gch, ch)),
                                                (gch->pcdata->group_leader == gch) ? "(leads)" : "",
                                                gch->hit < (gch->max_hit - gch->aggro_dam) / 10 ? 'R' : 'G',
                                                gch->hit, gch->max_hit - gch->aggro_dam,
                                                gch->mana < gch->max_mana / 10 ? 'R' : 'C',
                                                gch->mana, gch->max_mana,
                                                gch->move < gch->max_move / 10 ? 'R' : 'Y',
                                                gch->move, gch->max_move,
                                                (gch->level >= LEVEL_HERO)
                                                    ? 0
                                                    : level_table[gch->level].exp_total - gch->exp);
                                }
                                send_to_char(buf, ch);
                        }
                }
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (ch->master || (ch->leader && ch->leader != ch))
        {
                send_to_char("But you are following someone else!\n\r", ch);
                return;
        }

        if (victim->master != ch && ch != victim)
        {
                act("$N isn't following you.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (is_same_group(victim, ch) && ch != victim)
        {
                victim->leader = NULL;
                /* group leader removal */
                if (!IS_NPC(victim))
                        victim->pcdata->group_leader = NULL;

                for (gch = char_list; gch; gch = gch->next)
                {
                        if (!IS_NPC(gch) && gch->pcdata->group_leader == ch)
                                gch->pcdata->group_leader = NULL;
                }

                act("You remove $N from your group.", ch, NULL, victim, TO_CHAR);
                act("$n removes you from $s group.", ch, NULL, victim, TO_VICT);
                act("$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT);
                return;
        }

        if (((ch->level - victim->level < -11) || (ch->level - victim->level > 11)) && !(ch->level > LEVEL_HERO) && !(victim->level > LEVEL_HERO))
        {
                act("$N cannot join your group.", ch, NULL, victim, TO_CHAR);
                act("You cannot join $n's group.", ch, NULL, victim, TO_VICT);
                act("$N cannot join $n's group.", ch, NULL, victim, TO_NOTVICT);
                return;
        }

        /*  Limit number of NPCs in group; Gezhp  */
        npc_count = 0;
        for (gch = char_list; gch; gch = gch->next)
        {
                if (gch->deleted)
                        continue;
                if (is_same_group(gch, ch) && IS_NPC(gch))
                        npc_count++;
        }
        if (npc_count > 5)
        {
                send_to_char("You can't have any more NPCs in your group.\n\r", ch);
                return;
        }

        victim->leader = ch;

        if (!IS_NPC(victim) && !IS_NPC(ch))
                victim->pcdata->group_leader = ch->pcdata->group_leader;

        act("$N joins your group.", ch, NULL, victim, TO_CHAR);
        act("You join $n's group.", ch, NULL, victim, TO_VICT);
        act("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT);
        return;
}

/*
 * do_group_order
 * Shade August 99
 *
 * This function moves the named person to the front
 * of the group.  Currently there will be only one
 * person at the front and everyone else in the group
 * will be equal.
 */

void do_group_order(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *gch;
        char arg[MAX_INPUT_LENGTH];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Who should take the lead?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They're not here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("Mobs cannot be group leaders.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("Groups may not be led by the non-corporeal.\n\r", ch);
                return;
        }

        if (!is_same_group(ch, victim))
        {
                send_to_char("They're not in your group.\n\r", ch);
                return;
        }

        for (gch = char_list; gch; gch = gch->next)
        {
                if (gch->deleted)
                        continue;

                if (is_same_group(gch, ch) && !IS_NPC(gch))
                {
                        gch->pcdata->group_leader = victim;
                        if (gch != victim)
                                act("$N takes the lead.", gch, NULL, victim, TO_CHAR);
                        else
                                send_to_char("You take the lead.\n\r", gch);
                }
        }

        return;
}

/*
 * do_group_leader
 * Returns current group leader
 */
void do_group_leader(CHAR_DATA *ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (ch->pcdata->group_leader)
        {
                if (ch->pcdata->group_leader == ch)
                        send_to_char("You are, genius!\n\r", ch);
                else
                {
                        sprintf(buf, "%s is leading the group.\n\r",
                                ch->pcdata->group_leader->name);
                        send_to_char(buf, ch);
                }
        }
        else
                send_to_char("You have no group leader.\n\r", ch);
}

void do_tellmode(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (IS_SET(ch->act, PLR_NO_TELL))
        {
                REMOVE_BIT(ch->act, PLR_NO_TELL);
                send_to_char("You will now hear tells again.\n\r", ch);
        }
        else
        {
                SET_BIT(ch->act, PLR_NO_TELL);
                send_to_char("You no longer hear tells.\n\r", ch);
        }
}

void do_gag(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!ch->gag)
        {
                send_to_char("Normal gagging: You're now gagging some combat messages.\n\r", ch);
                ch->gag = 1;
        }
        else if (ch->gag == 1)
        {
                send_to_char("Heavy gagging: You're now gagging many combat messages.\n\r", ch);
                ch->gag = 2;
        }
        else
        {
                send_to_char("Combat message gagging is now off.\n\r", ch);
                ch->gag = 0;
        }
}

/*
 * do_quiet Shade / Gezhp
 */
void do_quiet(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (ch->silent_mode)
        {
                send_to_char("Silent mode off.\n\r", ch);
                ch->silent_mode = 0;
        }
        else
        {
                send_to_char("Silent mode on.\n\r", ch);
                ch->silent_mode = 1;
        }
}

/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *gch;
        char buf[MAX_STRING_LENGTH];
        char arg[MAX_INPUT_LENGTH];
        int members;
        int amount;
        int share;
        int extra;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Split how much?\n\r", ch);
                return;
        }

        amount = atoi(arg);

        if (amount < 0)
        {
                send_to_char("Your group wouldn't like that.\n\r", ch);
                return;
        }

        if (amount == 0)
        {
                send_to_char("You hand out zero coins, but no one notices.\n\r", ch);
                return;
        }

        if (total_coins_char(ch) < amount)
        {
                send_to_char("You don't have that many coins.\n\r", ch);
                return;
        }

        members = 0;

        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                if (gch->deleted)
                        continue;
                if (is_same_group(gch, ch))
                        members++;
        }

        if (members < 2)
        {
                send_to_char("Just keep it all.\n\r", ch);
                return;
        }

        share = amount / members;
        extra = amount % members;

        if (share == 0)
        {
                send_to_char("Don't even bother, cheapskate.\n\r", ch);
                return;
        }

        coins_from_char(amount, ch);
        coins_to_char(share + extra, ch, COINS_ADD);

        sprintf(buf, "You split %d coppers worth of coins.  Your share is %d coins.\n\r",
                amount, share + extra);
        send_to_char(buf, ch);

        sprintf(buf, "$n splits %d coppers worth of coins.  Your share is %d coins.",
                amount, share);

        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                if (gch->deleted)
                        continue;
                if (gch != ch && is_same_group(gch, ch))
                {
                        act(buf, ch, NULL, gch, TO_VICT);
                        coins_to_char(share, gch, COINS_ADD);
                }
        }

        return;
}

void do_gtell(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *gch;
        char buf[MAX_STRING_LENGTH];
        int position;

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE))
        {
                send_to_char("You are mute. Sssh.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char("Tell your group what?\n\r", ch);
                return;
        }

        /* No dollar signs thanks: problems with act */
        for (position = 0; position < strlen(argument); position++)
                if (argument[position] == '$')
                        argument[position] = '-';

        /* Note use of send_to_char, so gtell works on sleepers */
        for (gch = char_list; gch; gch = gch->next)
        {
                sprintf(buf, "%s%s tells the group '%s'$R",
                        color_table_8bit[get_colour_index_by_code(gch->colors[COLOR_TELL])].act_code, ch->name, argument);
                position = gch->position;
                gch->position = POS_STANDING;
                if (is_same_group(gch, ch))
                        act(buf, gch, NULL, NULL, TO_CHAR);
                gch->position = position;
        }

        return;
}

bool is_same_group(CHAR_DATA *ach, CHAR_DATA *bch)
{
        if (ach->leader)
                ach = ach->leader;
        if (bch->leader)
                bch = bch->leader;
        return ach == bch;
}

bool is_group_members_mount(CHAR_DATA *mount, CHAR_DATA *ch)
{
        /*
         *  Check if a mob in the room is being mounted by a
         *  member of a character's group.  Gezhp 2000.
         */

        CHAR_DATA *victim;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (is_same_group(ch, victim) && !IS_NPC(victim) && victim->mount == mount)
                        return TRUE;
        }

        return FALSE;
}

void talk_auction(char *argument)
{
        DESCRIPTOR_DATA *d;
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *original;

        for (d = descriptor_list; d != NULL; d = d->next)
        {
                original = d->original ? d->original : d->character;

                if (d->connected == CON_PLAYING && !original->silent_mode && !IS_SET(original->deaf, CHANNEL_AUCTION))
                {
                        sprintf(buf, "%sAUCTION: $t$R",
                                color_table_8bit[original->colors[COLOR_AUCTION]].act_code);
                        act(buf, original, argument, NULL, TO_CHAR);
                        media_notify_channel(original, CHANNEL_AUCTION);
                }
        }
}

void server_message(const char *text)
{
        char buf[MAX_STRING_LENGTH];
        DESCRIPTOR_DATA *d;
        CHAR_DATA *ch;

        if (!text)
                return;

        for (d = descriptor_list; d; d = d->next)
        {
                ch = d->original ? d->original : d->character;

                if (d->connected == CON_PLAYING && !IS_SET(ch->deaf, CHANNEL_SERVER) && ch->level > L_DIR)
                {
                        sprintf(buf, "%s[Server] $t$R",
                                color_table_8bit[get_colour_index_by_code(ch->colors[COLOR_SERVER])].act_code);
                        act(buf, ch, text, NULL, TO_CHAR);
                }
        }
}

/*
 * 'Review' channel recall command and related functions; Gezhp 2001
 */

void do_review(CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_INPUT_LENGTH];
        int i, j;
        char *when;
        bool found = FALSE;

        const char *usage = "Syntax: review <<chat|music|shout|info|arena|immtalk|dirtalk|clan|newbie>\n\r";

        const char *channels[] =
            {
                "chat",
                "music",
                "shout",
                "info",
                "arena",
                "immtalk",
                "dirtalk",
                "newbie"};

        if (IS_NPC(ch))
                return;

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (argument[0] == '\0')
        {
                send_to_char(usage, ch);
                return;
        }

        when = ctime(&current_time);
        when[strlen(when) - 9] = '\0';
        one_argument(argument, arg);

        for (i = 0; i < REVIEW_CHANNEL_COUNT; i++)
        {
                if (!is_name(arg, (char *)channels[i]))
                        continue;

                if ((i == REVIEW_IMMTALK && ch->pcdata->learned[gsn_immtalk] < 100) || (i == REVIEW_DIRTALK && ch->pcdata->learned[gsn_dirtalk] < 100) || (i == REVIEW_NEWBIE && ch->level > 4 && ch->level < 100 && !IS_SET(ch->act, PLR_GUIDE)))
                {
                        send_to_char("You're not authorized to review that channel.\n\r", ch);
                        return;
                }

                for (j = REVIEW_BUFFER_LENGTH - 1; j >= 0; j--)
                {
                        if (review_buffer_public[i][j].buffer[0] == '\0' || review_buffer_public[i][j].timestamp < ch->pcdata->review_stamp)
                                continue;

                        if (!found)
                        {
                                sprintf(buf, "---- Reviewing the %s channel ----\n\r", channels[i]);
                                send_to_char(buf, ch);
                                found = TRUE;
                        }

                        send_paragraph_to_char(review_buffer_public[i][j].buffer, ch, 16);
                }

                if (!found)
                {
                        send_to_char("There is nothing to review for that channel.\n\r", ch);
                        return;
                }

                sprintf(buf, "%s  is the current time\n\r---- Review ends ----\n\r",
                        &when[4]);
                send_to_char(buf, ch);
                return;
        }

        if (is_name(arg, "clan"))
        {
                if (!ch->clan)
                {
                        send_to_char("You are not in a clan.\n\r", ch);
                        return;
                }

                if (review_buffer_clan[ch->clan][0].buffer[0] == '\0')
                {
                        send_to_char("There is nothing to review for that channel.\n\r", ch);
                        return;
                }

                for (j = REVIEW_BUFFER_LENGTH - 1; j >= 0; j--)
                {
                        if (review_buffer_clan[ch->clan][j].buffer[0] == '\0' || review_buffer_clan[ch->clan][j].timestamp < ch->pcdata->review_stamp)
                                continue;

                        if (!found)
                        {
                                sprintf(buf, "---- Reviewing the %s clan channel ----\n\r",
                                        clan_table[ch->clan].who_name);
                                send_to_char(buf, ch);
                                found = TRUE;
                        }

                        send_paragraph_to_char(review_buffer_clan[ch->clan][j].buffer, ch, 16);
                }

                sprintf(buf, "%s  is the current time\n\r---- Review ends ----\n\r",
                        &when[4]);
                send_to_char(buf, ch);
                return;
        }

        send_to_char(usage, ch);
}

void add_to_public_review_buffer(CHAR_DATA *ch, int channel, char *text)
{
        int i;
        char buf[MAX_STRING_LENGTH];
        char *when;

        if (channel < 0 || channel >= REVIEW_CHANNEL_COUNT || text[0] == '\0')
                return;

        when = ctime(&current_time);
        when[strlen(when) - 9] = '\0';

        if (ch)
                sprintf(buf, "%s  {W%s{x: %s{x\n\r",
                        &when[4],
                        IS_NPC(ch)
                            ? capitalize_initial(ch->short_descr)
                        : IS_SET(ch->act, PLR_WIZINVIS)
                            ? "Someone"
                            : ch->name,
                        text);
        else
                sprintf(buf, "%s  %s{x\n\r",
                        &when[4],
                        text);

        for (i = REVIEW_BUFFER_LENGTH - 1; i; i--)
        {
                strncpy(review_buffer_public[channel][i].buffer,
                        review_buffer_public[channel][i - 1].buffer,
                        REVIEW_BUFFER_SIZE);

                review_buffer_public[channel][i].timestamp = review_buffer_public[channel][i - 1].timestamp;
        }

        strncpy(review_buffer_public[channel][0].buffer, buf, REVIEW_BUFFER_SIZE);
        review_buffer_public[channel][0].timestamp = current_time;
}

void add_to_clan_review_buffer(CHAR_DATA *ch, int clan, char *text)
{
        int i;
        char buf[MAX_STRING_LENGTH];
        char *when;

        if (IS_NPC(ch) || clan < 1 || clan >= MAX_CLAN || text[0] == '\0')
                return;

        when = ctime(&current_time);
        when[strlen(when) - 9] = '\0';

        sprintf(buf, "%s  {W%s{x: %s{x\n\r",
                &when[4],
                ch->name,
                text);

        for (i = REVIEW_BUFFER_LENGTH - 1; i; i--)
        {
                strncpy(review_buffer_clan[clan][i].buffer,
                        review_buffer_clan[clan][i - 1].buffer,
                        REVIEW_BUFFER_SIZE);

                review_buffer_clan[clan][i].timestamp = review_buffer_clan[clan][i - 1].timestamp;
        }

        strncpy(review_buffer_clan[clan][0].buffer, buf, REVIEW_BUFFER_SIZE);
        review_buffer_clan[clan][0].timestamp = current_time;
}

/*
 * Suicide; Gezhp 2001
 */

void do_suicid(CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        send_to_char("You must spell it out if you want to commit suicide.\n\r", ch);
}

void do_suicide(CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                send_to_char("You may not commit suicide in the Arena.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0' || !ch->pcdata->suicide_code)
        {
                ch->pcdata->suicide_code = number_range(10000, 99999);

                sprintf(buf, "{RYou contemplate suicide.{x\n\r"
                             "To end your life, you must type 'SUICIDE %d'.\n\r",
                        ch->pcdata->suicide_code);
                send_to_char(buf, ch);
                return;
        }

        if (!is_number(arg) || atoi(arg) != ch->pcdata->suicide_code)
        {
                sprintf(buf, "To end your life, you must type 'SUICIDE %d'.\n\r",
                        ch->pcdata->suicide_code);
                send_to_char(buf, ch);
                return;
        }

        do_chat(ch, "Goodbye cruel world!");

        act("$n offers his body to the gods of death and they greedily accept!",
            ch, NULL, NULL, TO_ROOM);
        send_to_char("You offer your body to the gods of death and they greedily accept!\n\r", ch);

        sprintf(buf, "%s has committed SUICIDE!", ch->name);
        do_info(ch, buf);

        ch->pcdata->suicide_code = 0;
        check_player_death(ch, ch);
        death_penalty(ch, ch);
        raw_kill(ch, ch, TRUE);
        send_to_char("\n\r", ch);
        do_look(ch, "");
}

/* EOF act_comm.c */
