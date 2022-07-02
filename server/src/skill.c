#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void atemi              args((CHAR_DATA *ch, CHAR_DATA *victim));
void kansetsu           args((CHAR_DATA *ch, CHAR_DATA *victim));
void tetsui             args((CHAR_DATA *ch, CHAR_DATA *victim));
void shuto              args((CHAR_DATA *ch, CHAR_DATA *victim));
void yokogeri           args((CHAR_DATA *ch, CHAR_DATA *victim));
void mawasigeri         args((CHAR_DATA *ch, CHAR_DATA *victim));


/*
 * Fly/Flight for native flyers, Halfdragons Danath
 */
void do_fly (CHAR_DATA *ch, char* argument )
{
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_flight))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_FLYING))
        {
                send_to_char ("You feel the pull of gravity slowly return.\n\r", ch);
                act ("$c seems to be affected by gravity once more.", ch, NULL, NULL, TO_ROOM);
                affect_strip (ch, gsn_fly);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);
                return;
        }

        af.type      = gsn_fly;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_FLYING;
        affect_to_char( ch, &af );

        send_to_char( "The sensation of gravity leaves your body.\n\r", ch );
        act( "$n seems no longer to be affected by gravity.", ch, NULL, NULL, TO_ROOM );
        return;
}


void do_blink (CHAR_DATA *ch, char *argument)
{
        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_blink))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if ( ch->pcdata->blink )
        {
                ch->pcdata->blink = FALSE;
                send_to_char( "You relax your concentration and your form becomes stable.\n\r", ch );
        }
        else
        {
                ch->pcdata->blink = TRUE;
                send_to_char( "You concentrate and your body begins to shimmer.\n\r",ch);
        }
}


void do_swim (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_swim))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_swim))
        {
                send_to_char("You are already swimming.\n\r", ch);
                return;
        }

        if (number_percent() < ch->pcdata->learned[gsn_swim])
        {
                af.type      = gsn_swim;
                af.duration  = -1;
                af.modifier  = 0;
                af.location  = APPLY_NONE;
                af.bitvector = AFF_SWIM;
                affect_to_char(ch, &af);

                af.modifier = (get_curr_dex(ch) - 30);
                af.location = APPLY_HITROLL;
                affect_to_char(ch, &af);
        }

        send_to_char("You can now swim.\n\r", ch);
        return;
}


void do_gouge (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        AFFECT_DATA af;
        int chance;

        if (IS_NPC(ch))
        {
                if (ch->pIndexData->vnum != BOT_VNUM)
                        return;
        }
        else if (!CAN_DO(ch, gsn_gouge))
        {
                send_to_char("You are not skilled enough to gouge eyes.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char ("Whose eyes do you wish to gouge out?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)) || !can_see(ch, victim))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to rip your own eyes out???\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_BLIND))
        {
                send_to_char("They are blind already!\n\r",ch);
                damage(ch, victim, 0, gsn_gouge, FALSE);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("They are fighting.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && !HAS_EYES(victim) )
        {
                act("You can't gouge $N, $E doesn't have any eyes!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (IS_NPC(ch))
                chance = 40 + ch->level / 2;
        else
                chance = ch->pcdata->learned[gsn_gouge];

        chance += (ch->level - victim->level) * 3;
        chance = URANGE(5, chance, 95);

        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (number_percent() < chance)
        {
                act ("You deftly lunge at $N's eyes, and gouge them out!", ch, NULL, victim, TO_CHAR);
                act ("$n scratches your eyes -- you see red!", ch, NULL, victim, TO_VICT);
                act ("$n gouges $N's eyes out!", ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n gouges out $N's eyes.", ch, victim);

                damage(ch, victim, number_range(10, ch->level), gsn_gouge, FALSE);

                af.type      = gsn_gouge;
                af.duration  = ch->level / 20;
                af.location  = APPLY_HITROLL;
                af.modifier  = -4;
                af.bitvector = AFF_BLIND;
                affect_to_char(victim, &af);

                check_group_bonus(ch);

        }
        else
                damage(ch, victim, 0, gsn_gouge, FALSE);

}


void do_choke (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        AFFECT_DATA af;
        int chance;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_choke))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char ("Choke whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)) || !can_see(ch, victim))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to choke yourself??\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("Not while you are fighting!\n\r", ch);
                return;
        }

        if (is_affected( victim, gsn_choke ) )
        {
                send_to_char("They are already unconscious.\n\r", ch );
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot choke a fighting person.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        chance = ch->pcdata->learned[gsn_choke];
        chance += ((ch->level - victim->level) * 3);

        if (chance < 5)
                chance = 5;

        if (chance > 95)
                chance = 95;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (number_percent() < chance)
        {
                act ("You circle around $N and apply a choker hold on them until they turn blue.",
                     ch, NULL, victim, TO_CHAR);
                act ("$n begins to choke you.. you feel dizzy... you black out.",
                     ch, NULL, victim, TO_VICT);
                act ("$n applies a choker hold on $N until they collapse, unconscious!",
                     ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$N is choked unconscious by $n.", ch, victim);

                af.type      = gsn_choke;
                af.duration  = 0;
                af.location  = APPLY_HITROLL;
                af.modifier  = -4;
                af.bitvector = AFF_SLEEP;
                affect_to_char(victim, &af);

                do_sleep(victim, "");
                check_group_bonus(ch);

                WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
        }
        else
                damage(ch, victim, 0, gsn_choke, FALSE);
}


void do_battle_aura (CHAR_DATA *ch, char *argument)
{
        int temp;

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_battle_aura))

        {
                send_to_char("You are not skilled enough to do that.\n\r", ch);
                return;
        }

        if (is_affected (ch, gsn_battle_aura))
                return;

        if (get_curr_int (ch) < 19 && get_curr_wis(ch) <19)
        {
                send_to_char ("Your lack of intellect and concentration prevents this..\n\r", ch);
                return;
        }

        temp = ch->level;

        if (number_percent() < ch->pcdata->learned[gsn_battle_aura])
        {
                AFFECT_DATA af;

                af.type     = gsn_battle_aura;
                af.duration = temp / 10;
                af.modifier = - temp *2;
                af.location = APPLY_AC;
                af.bitvector = 0;
                affect_to_char(ch, &af);

                af.duration  = temp / 10;
                af.location  = APPLY_HITROLL;
                af.modifier  = temp/3;
                af.bitvector = AFF_BATTLE_AURA;
                affect_to_char(ch, &af);

                act ("$n closes his eyes, shakes violently, screams loudly and starts to emit a silvery glow.",
                     ch, NULL, NULL, TO_ROOM);
                send_to_char("Concentrating intently to the point of shaking, you feel your body stiffen for battle.\n\r", ch);
        }
        else
                send_to_char("You focus your mental energies but your concentration is lacking...\n\r", ch);

        return;
}


void do_punch (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_punch))
        {
                send_to_char("You'd better leave the martial arts to fighters.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }
        }

        if (victim == ch)
        {
                send_to_char("You want to knock yourself out??\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (number_percent() < ch->pcdata->learned[gsn_punch])
        {
                arena_commentary("$n punches $N.", ch, victim);
                damage(ch, victim, (ch->level / 2) + number_range(1, ch->level * 2), gsn_punch, FALSE);

                if (victim->position ==POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (number_percent() < ch->pcdata->learned[gsn_second_punch])
                        damage(ch, victim, ch->level + number_range(ch->level, ch->level * 2),gsn_second_punch, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_punch, FALSE);

        return;
}


void do_dismount (CHAR_DATA *ch, char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!ch->mount)
        {
                send_to_char("You're not mounted.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_mount].beats);

        act("You dismount $N.", ch, NULL, ch->mount, TO_CHAR);
        act("$n skillfully dismounts $N.", ch, NULL, ch->mount, TO_NOTVICT);
        act("$n dismounts you.  Whew!", ch, NULL, ch->mount, TO_VICT);

        strip_mount(ch);
        ch->position = POS_STANDING;
}


void strip_mount (CHAR_DATA* ch)
{
        if (!ch->mount)
        {
                bug("strip_mount: 'ch->mount' is null", 0);
                return;
        }

        affect_strip (ch, gsn_mount);
        ch->mount->rider = NULL;
        ch->mount = NULL;
}


void do_mount(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_mount))
        {
                send_to_char ("You aren't skilled enough to ride a mount.\n\r", ch);
                return;
        }

        if (ch->race == RACE_CENTAUR)
        {
                send_to_char ("Centaurs may not ride mounts.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What would you like to mount?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (!IS_NPC(victim))
                return;

        if (!IS_SET(victim->act, ACT_MOUNTABLE))
        {
                send_to_char("You must find a suitable mount!\n\r", ch);
                return;
        }

        if (victim->rider)
        {
                send_to_char("They are already mounted!\n\r",ch);
                return;
        }

        if (ch->mount)
        {
                send_to_char( "You are already mounted.\n\r",ch );
                return;
        }

        if ( victim->position <= POS_RESTING )
        {
                send_to_char( "They are not ready to be mounted.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You can't mount yourself.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char ("Not in your current form.\n\r", ch);
                return;
        }

        if ((victim->master && victim->master != ch) || ch->level < victim->level)
        {
                send_to_char("Your intended mount thinks you're unworthy and will not allow you to mount.\n\r", ch);
                return;
        }

        ch->mount = victim;
        victim->rider = ch;

        af.type     = gsn_mount;
        af.duration = -1;
        af.modifier = - ch->level /2;
        af.location = APPLY_AC;
        af.bitvector = 0;
        affect_to_char(ch, &af);

        af.duration  = -1;
        af.location  = APPLY_HITROLL;
        af.modifier  = ch->level / 10;
        affect_to_char(ch, &af);

        affect_strip(ch, gsn_sneak);
        affect_strip(ch, gsn_hide);
        affect_strip(ch, gsn_shadow_form);

        act ("You jump on $N's back, ready for travel.", ch, NULL, victim, TO_CHAR);
        act ("$n jumps on $N's back, ready for travel.", ch, NULL, victim, TO_ROOM);
}

/*
 * Combo flags - Shade & Gezhp June 2001
 */

/*
 * Attacks that follow a punch get a bonus.
 * Cannot kick twice in a row.
 */

int combo_last_punch;
int combo_last_kick;

/*
 * Record unique moves
 */

int combo_atemi;
int combo_tetsui;
int combo_shuto;
int combo_yokogeri;
int combo_mawasigeri;
int combo_count;

/*
 * Reset combo flags
 */

void reset_combo_flags()
{
        combo_last_punch        = 0;
        combo_last_kick         = 0;
        combo_atemi             = 0;
        combo_tetsui            = 0;
        combo_shuto             = 0;
        combo_yokogeri          = 0;
        combo_mawasigeri        = 0;
        combo_count             = 0;
}

/*
 * end combo flags
 */

void do_combo (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA               *victim;
        char                    arg [MAX_INPUT_LENGTH];
        int                     sn;
        int                     wait = 0;
        int                     number = 0;

        const int sn_atemi      = skill_lookup("atemi");
        const int sn_kansetsu   = skill_lookup("kansetsu");
        const int sn_tetsui     = skill_lookup("tetsui");
        const int sn_shuto      = skill_lookup("shuto");
        const int sn_yokogeri   = skill_lookup("yokogeri");
        const int sn_mawasigeri = skill_lookup("mawasigeri");

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_combo))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        reset_combo_flags();

        while (1)
        {
                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;

                argument = one_argument(argument, arg);

                if (arg[0] == '\0')
                        return;

                if (is_name(arg, "atemi"))
                        sn = sn_atemi;

                else if (is_name(arg, "kansetsu"))
                        sn = sn_kansetsu;

                else if (is_name(arg, "tetsui"))
                        sn = sn_tetsui;

                else if (is_name(arg, "shuto"))
                        sn = sn_shuto;

                else if (is_name(arg, "yokogeri"))
                        sn = sn_yokogeri;

                else if (is_name(arg, "mawasigeri"))
                        sn = sn_mawasigeri;

                else
                        continue;

                number++;

                if (number == 2 && number_percent() >= ch->pcdata->learned[gsn_combo2])
                        return;

                if (number == 3
                    && number_percent() >= (45 + ch->pcdata->learned[gsn_combo3] / 2))
                        return;

                if (number == 4
                    && number_percent() >= (40 + ch->pcdata->learned[gsn_combo4] / 2))
                        return;

                if (number > 4)
                        return;

                wait += 6;
                WAIT_STATE(ch, wait);

                if (sn == sn_atemi)
                {
                        if (number_percent() < ch->pcdata->learned[gsn_atemi])
                        {
                                atemi(ch , victim);
                                combo_last_punch = 1;

                                if (!combo_atemi)
                                {
                                        combo_count++;
                                        combo_atemi = 1;
                                }

                        }
                        else
                        {
                                damage(ch , victim, 0 , gsn_atemi, FALSE);
                                combo_last_punch = 0;
                        }

                        combo_last_kick = 0;
                        continue;
                }

                if (sn == sn_kansetsu)
                {
                        combo_last_punch = 0;
                        combo_last_kick = 0;

                        if (number_percent() < ch->pcdata->learned[gsn_kansetsu])
                        {
                                kansetsu(ch, victim);
                        }
                        else
                        {
                                act ("$n attempts to break $N's arms, but fails.", ch, NULL, victim,TO_NOTVICT);
                                damage(ch , victim, 0 , gsn_kansetsu, FALSE);
                        }

                        continue;
                }

                if (sn == sn_tetsui)
                {
                        if (number_percent() < ch->pcdata->learned[gsn_tetsui])
                        {
                                tetsui(ch, victim);
                                combo_last_punch = 1;

                                if (!combo_tetsui)
                                {
                                        combo_tetsui = 1;
                                        combo_count++;
                                }
                        }
                        else
                        {
                                damage(ch, victim,0, gsn_tetsui, FALSE);
                                combo_last_punch = 0;
                        }

                        combo_last_kick = 0;
                        continue;
                }

                if (sn == sn_shuto)
                {
                        if (number_percent() < ch->pcdata->learned[gsn_shuto])
                        {
                                shuto(ch, victim);
                                combo_last_punch = 1;

                                if (!combo_shuto)
                                {
                                        combo_shuto = 1;
                                        combo_count++;
                                }
                        }
                        else
                        {
                                damage(ch, victim,0, gsn_shuto, FALSE);
                                combo_last_punch = 0;
                        }

                        combo_last_kick = 0;
                        continue;
                }

                if (sn == sn_yokogeri)
                {
                        if (combo_last_kick)
                        {
                                send_to_char("You cannot kick twice in succession without regaining your balance.\n\r", ch);
                                combo_last_kick = 0;
                                combo_last_punch = 0;
                                continue;
                        }

                        if (number_percent() < ch->pcdata->learned[gsn_yokogeri])
                        {
                                yokogeri(ch, victim);
                                combo_last_kick = 1;

                                if (!combo_yokogeri)
                                {
                                        combo_yokogeri = 1;
                                        combo_count++;
                                }
                        }
                        else
                        {
                                damage(ch, victim,0, gsn_yokogeri, FALSE);
                                combo_last_kick = 0;
                        }

                        combo_last_punch = 0;
                        continue;
                }

                if (sn == sn_mawasigeri)
                {
                        if (combo_last_kick)
                        {
                                send_to_char("You cannot kick twice in succession without regaining your balance.\n\r", ch);
                                combo_last_kick = 0;
                                combo_last_punch = 0;
                                continue;
                        }

                        if ( number_percent() < ch->pcdata->learned[gsn_mawasigeri])
                        {
                                mawasigeri(ch, victim);
                                combo_last_kick = 1;

                                if (!combo_mawasigeri)
                                {
                                        combo_mawasigeri = 1;
                                        combo_count++;
                                }
                        }
                        else
                        {
                                damage(ch, victim,0, gsn_mawasigeri, FALSE);
                                combo_last_kick = 0;
                        }

                        combo_last_punch = 0;
                        continue;
                }
        }
}


void do_atemi (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_atemi))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Who is your target?\n\r", ch);
                        return;
                }
        }


        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_atemi].beats);

        if (number_percent() < ch->pcdata->learned[gsn_atemi])
        {
                reset_combo_flags();
                atemi(ch, victim);
        }
        else
                damage(ch , victim, 0 , gsn_atemi, FALSE);
}


void do_kansetsu (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_kansetsu))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Who is your target?\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_kansetsu].beats);
        if (number_percent() < ch->pcdata->learned[gsn_kansetsu])
                kansetsu(ch, victim);
        else
                damage(ch , victim, 0 , gsn_kansetsu, FALSE);
}


void do_tetsui (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_tetsui))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Who is your target?\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_tetsui].beats);
        if (number_percent() < ch->pcdata->learned[gsn_tetsui])
        {
                reset_combo_flags();
                tetsui(ch, victim);
        }
        else
                damage(ch, victim,0, gsn_tetsui, FALSE);
}


void do_shuto (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_shuto))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Who is your target?\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_shuto].beats);
        if (number_percent() < ch->pcdata->learned[gsn_shuto])
        {
                reset_combo_flags();
                shuto(ch, victim);
        }
        else
                damage(ch, victim,0, gsn_shuto, FALSE);
}

void do_yokogeri (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_yokogeri))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Who is your target?\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_yokogeri].beats);
        if (number_percent() < ch->pcdata->learned[gsn_yokogeri])
        {
                reset_combo_flags();
                yokogeri(ch , victim);
        }
        else
                damage(ch, victim,0, gsn_yokogeri, FALSE);
}


void do_mawasigeri (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_mawasigeri))
        {
                send_to_char("You are not skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_mawasigeri].beats);
        if (number_percent() < ch->pcdata->learned[gsn_mawasigeri])
        {
                reset_combo_flags();
                mawasigeri(ch, victim);
        }
        else
                damage(ch, victim,0, gsn_mawasigeri, FALSE);
}


void atemi (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = ch->level * 1.5;

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        arena_commentary("$n strikes $N in the torso.", ch, victim);
        damage(ch, victim, dam, gsn_atemi, FALSE);
}


void kansetsu (CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *obj;

        if (!HAS_ARMS (victim))
        {
                act ("$N has no wrists for you to break.", ch, NULL, victim, TO_CHAR);
                return;
        }

        obj = get_eq_char(victim, WEAR_WIELD);
        if (!obj)
                obj = get_eq_char(victim, WEAR_DUAL);

        if (!obj)
        {
                act ("$N has no weapon.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_SET (obj->extra_flags, ITEM_BODY_PART))
        {
                send_to_char ("You can't disarm your opponent's body parts!\n\r", ch);
                return;
        }

        if (number_percent() < ch->pcdata->learned[gsn_kansetsu]*2/3)
        {
                act ("{Y*SNAP*{x You break $N's wrist, causing $S weapon to fall from $M!",
                     ch, NULL, victim, TO_CHAR);
                act ("{Y*SNAP*{x, $n breaks your wrist, causing your weapon to fall!",
                     ch, NULL, victim, TO_VICT);
                act ("{Y*SNAP*{x, $n deftly breaks $N's wrist, causing $M to drop $S weapon!",
                     ch, NULL, victim,TO_NOTVICT);
                arena_commentary("$n snaps $N's wrist.", ch, victim);

                obj_from_char(obj);
                obj_to_room(obj, victim->in_room);
                damage(ch, victim, number_range (20, ch->level*2), gsn_kansetsu, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_kansetsu, FALSE);
}


void tetsui (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = ch->level * 2;

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        arena_commentary("$n strikes $N with a hammer fist.", ch, victim);
        damage(ch, victim, dam, gsn_tetsui, FALSE);
}


void shuto (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = dice(ch->level, 4);

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2)
                dam *= -1.5 + combo_count;

        arena_commentary("$N is hit by $n's knife hand strike.", ch, victim);
        damage(ch, victim, dam, gsn_shuto, FALSE);
}


void yokogeri (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = dice(ch->level, 5);

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        arena_commentary("$n side kicks $N.", ch, victim);
        damage(ch, victim, dam, gsn_yokogeri, FALSE);
}


void mawasigeri (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = dice(ch->level, 6);

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        arena_commentary("$n roundhouse kicks $N.", ch, victim);
        damage(ch, victim, dam, gsn_mawasigeri, FALSE);
}


void do_push (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        int exit_dir;
        EXIT_DATA *pexit;
        CHAR_DATA *victim;
        ROOM_INDEX_DATA *to_room;
        ROOM_INDEX_DATA *in_room;
        int chance;
        int race_bonus;

        argument = one_argument(argument, arg);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_push))
        {
                send_to_char("Huh?\n\r",ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Shove whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You shove yourself around, to no avail.\n\r", ch);
                return;
        }

        if ( ( ( ch->level - victim->level > 5 )
        ||     ( victim->level - ch->level > 5 ) )
        &&   !IS_IMMORTAL( ch ) )
        {
                send_to_char("There is too great an experience difference for you to even bother.\n\r", ch);
                return;
        }

        if (victim->position != POS_STANDING)
        {
                act ( "$N isn't upright.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (arg2[0] == '\0')
        {
                send_to_char("Shove them in which direction?\n\r", ch);
                return;
        }

        exit_dir = get_dir(arg2);
        if (exit_dir < 0 || exit_dir > 5)
        {
                bug("Do_move: bad push direction %d.", exit_dir);
                return;
        }

        in_room = ch->in_room;

        if (!(pexit = in_room->exit[exit_dir])
            || !(to_room = pexit->to_room)
            || (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(victim, AFF_PASS_DOOR)))
        {
                send_to_char("There's no exit in that direction.\n\r", ch);
                victim->position = POS_STANDING;
                return;
        }

        if (ch->in_room->area != to_room->area)
        {
                send_to_char("That character cannot enter that area.\n\r", ch);
                victim->position = POS_STANDING;
                return;
        }

        if (is_safe(ch, victim))
                return;

        chance = 50;
        chance += (get_curr_str(ch) - 15) * 3;
        chance += ch->level - victim->level;
        race_bonus = 0;

        switch (ch->race)
        {
            case 2:  race_bonus =  -4; break;
            case 3:  race_bonus =   2; break;
            case 4:  race_bonus =   4; break;
            case 5:  race_bonus =  10; break;
            case 6:  race_bonus =  -2; break;
            case 7:  race_bonus =  10; break;
            case 8:  race_bonus =  -4; break;
            case 9:  race_bonus =   7; break;
            case 10: race_bonus =  -5; break;
            case 11: race_bonus =   6; break;
            case 12: race_bonus =   4; break;
            case 13: race_bonus =  -4; break;
            case 14: race_bonus =   8; break;
            case 15: race_bonus =   5; break;
            case 16: race_bonus =   3; break;
            case 17: race_bonus =   4; break;
            case 18: race_bonus = -10; break;
            case 19: race_bonus =   6; break;
            case 21: race_bonus =  10; break;
            case 22: race_bonus =  -3; break;
            case 23: race_bonus =  -6; break;
            default: break;
        }

        chance += race_bonus;

        if (chance < number_percent())
        {
                send_to_char("You failed.\n\r", ch);
                victim->position = POS_STANDING;
                return;
        }

        act ("You shove $N.", ch, NULL, victim, TO_CHAR);
        act ("$n shoves you.", ch, NULL, victim, TO_VICT);
        move_char(victim, exit_dir);

        if (!(victim->position == POS_DEAD || ch->in_room != victim->in_room))
                victim->position = POS_STANDING;
        WAIT_STATE(ch, 12);
}


void do_feed (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA   *corpse;
        char        arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (ch->sub_class != SUB_CLASS_VAMPIRE)
        {
                send_to_char("Better leave feeding on corpses to the vampires.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("You can only feed on fresh corpses.\n\r", ch);
                return;
        }

        corpse = get_obj_list(ch, arg, ch->in_room->contents);
        if (!corpse)
        {
                send_to_char("You can't find it.\n\r", ch);
                return;
        }

        if (corpse->item_type != ITEM_CORPSE_NPC)
        {
                send_to_char ("You can only feed on fresh corpses.\n\r", ch);
                return;
        }

        send_to_char("You feed on the fresh corpse!\n\r", ch);
        ch->rage = UMIN(ch->rage +5, ch->max_rage);

        if (ch->rage > (ch->max_rage * 3 / 4))
                send_to_char("You feel the power of {Rblood{x surge through your fangs!\n\r", ch);

        update_pos(ch);
        act ("$n feeds on $p.", ch, corpse, NULL, TO_ROOM);
        extract_obj(corpse);
        return;
}


void do_focus (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *fch;
        char arg [ MAX_INPUT_LENGTH ];

        if ( IS_NPC( ch ) && !(ch->spec_fun == spec_lookup("spec_grail")))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_focus))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_berserk))
        {
                send_to_char("You cant focus on others, you're BERSERK!\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Change your focus of attack to whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("What about fleeing instead?\n\r", ch);
                return;
        }

        if (!(fch = victim->fighting))
        {
                send_to_char("That person is not fighting right now.\n\r", ch);
                return;
        }

        if (fch != ch)
        {
                send_to_char("They aren't fighting you.\n\r", ch);
                return;
        }

        if (ch->fighting == victim)
        {
                send_to_char("You are already fighting them.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_focus].beats);

        if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_focus])
        {
                send_to_char("You fail.\n\r", ch);
                return;
        }

        act ("You shift the focus of your attacks to $N!",  ch, NULL, victim, TO_CHAR);
        act ("$n shifts his attacks toward you!", ch, NULL, victim, TO_VICT);
        act ("$n shifts the focus of his attacks toward $N!",  ch, NULL, victim, TO_NOTVICT);

        stop_fighting(ch, FALSE);
        set_fighting(ch, victim);

        return;
}


void do_suck (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_suck))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char("Suck Whom?\n\r", ch);
                        return;
                }
        }

        if (victim == ch)
        {
                send_to_char("Sucking oneself is an unwholesome activity.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_INORGANIC(victim))
        {
                send_to_char("They have no blood for you to suck!\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        send_to_char("You attempt to suck some {Rblood{x from your opponent.\n\r", ch);

        if (number_percent () < ch->pcdata->learned[gsn_suck])
        {
                arena_commentary("$n sucks blood from $N's neck.", ch, victim);
                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                one_hit(ch, victim, gsn_suck);
                ch->rage += 2;
                ch->rage = UMIN(ch->rage, ch->max_rage);

                if (ch->rage > (ch->max_rage * 3 / 4))
                        send_to_char("You feel the power of {Rblood{x surge through your fangs!\n\r", ch);

        }
        else
        {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                damage (ch, victim, 0, gsn_suck, FALSE);
        }
}


void do_break_wrist (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_break_wrist))
        {
                send_to_char("You aren't skilled enough.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
        {
                send_to_char("You grope around blindly for their arms...\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }
        }

        if (victim->fighting != ch && ch->fighting != victim)
        {
                act ("$E is not fighting you!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_NPC(victim) && !HAS_ARMS(victim))
        {
                act("$N has no arms for you to break!", ch, NULL, victim, TO_CHAR);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_break_wrist].beats);

        if (number_percent() < (2 * (ch->pcdata->learned[gsn_break_wrist] / 3)))
        {
                OBJ_DATA *obj;
                obj = get_eq_char(victim, WEAR_WIELD);

                if (!obj)
                        obj = get_eq_char(victim, WEAR_DUAL);

                if (!obj)
                {
                        send_to_char("Your opponent is not wielding a weapon.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
                {
                        send_to_char("You cannot sever your opponent's body parts.\n\r", ch);
                        return;
                }

                act ("{Y*SNAP*{x You break $N's wrist, causing $S weapon to fall from $M!",
                     ch, NULL, victim, TO_CHAR);
                act ("{Y*SNAP*{x $n breaks your wrist, causing your weapon to fall!",
                     ch, NULL, victim, TO_VICT);
                act ("{Y*SNAP*{x, $n deftly breaks $N's wrist, causing $M to drop $S weapon!",
                     ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n breaks $N's wrist.", ch, victim);

                /* object to room */
                obj_from_char(obj);
                obj_to_room(obj, victim->in_room);
                damage(ch, victim, number_range (20, ch->level), gsn_break_wrist, FALSE);

                check_group_bonus(ch);

        }
        else
                damage(ch , victim, 0 , gsn_break_wrist, FALSE);
}


/*
 *  sharpen weapon skill _ Brutus
 */

void do_sharpen (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool in_c_room;
        int obj_craft_bonus;
        int mod_room_bonus;

        obj_craft_bonus = get_craft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_SHARPEN + obj_craft_bonus;

        in_c_room = FALSE;


        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_c_room = TRUE;
        }

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_sharpen))
        {
                send_to_char("What do you think you are, a smith?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What do you want to sharpen?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that weapon.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That item is not a weapon.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_SHARP))
        {
                send_to_char("That weapon has already been sharpened.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_POISONED))
        {
                send_to_char("That weapon is poisoned and cannot be sharpened!\n\r", ch);
                return;
        }

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_WHETSTONE)
                        break;
        }
        if (!wobj)
        {
                send_to_char("You need a whetstone to sharpen your blade.\n\r", ch);
                return;
        }

        if (!is_bladed_weapon(obj))
        {
                send_to_char("You can only sharpen bladed weapons.\n\r",ch);
                return;
        }

        if (get_curr_dex(ch) < 20 || ch->pcdata->condition[COND_DRUNK] > 0)
        {
                send_to_char("Your hands aren't steady enough to safely sharpen your blade.\n\r", ch);
                return;
        }

        if (number_percent() > ch->pcdata->learned[gsn_sharpen])
        {
                send_to_char("You slip while sharpening your blade! You ruin the stone and cut yourself!!\n\r", ch);
                damage(ch, ch, ch->level, gsn_sharpen, FALSE);
                act ("$n cuts $mself while sharpening $m blade!", ch, NULL, NULL, TO_ROOM);
                extract_obj(wobj);
                return;
        }

        if (in_c_room)
                send_to_char("{CThe use of specialised tools improves the quality of your smithing!\n\r{x", ch);

        act ("You run $p down the blade of $P, creating a deadly weapon!",
             ch, wobj, obj, TO_CHAR);
        act ("$n runs $p down the blade of $P, creating a deadly blade!",
             ch, wobj, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_SHARP);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_sharpen;
        paf->duration       = -1;
        paf->location       = APPLY_DAMROLL;
        paf->modifier       = ( in_c_room ) ? 2 + ( ch->level / ( ( 5 * 100 ) / mod_room_bonus ) ) : 2 + ch->level / 5;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        obj->timer          = (in_c_room) ? ( ( 30  * mod_room_bonus ) / 100 ) * (ch->level / 15) + 60 : 30 * (ch->level / 15) + 60;  /* 1-2 real hours */
        obj->timermax       = obj->timer;
        set_obj_owner(obj, ch->name);
}

/*
 *  improve armour skill _ Brutus
 */
void do_forge (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        OBJ_DATA *anvil;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool found;
        bool in_c_room;
        int obj_craft_bonus;
        int mod_room_bonus;

        in_c_room = FALSE;

        obj_craft_bonus = get_craft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_FORGE + obj_craft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_c_room = TRUE;
        }

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_forge))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What are you trying to forge?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that piece of armour.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_ARMOR)
        {
                send_to_char("That item is not armour.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_MAGIC))
        {
                send_to_char("The enchanted armour cannot be enhanced.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_FORGED))
        {
                send_to_char("That piece of armour has already been forged.\n\r", ch);
                return;
        }

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_ARMOURERS_HAMMER)
                        break;
        }
        if (!wobj)
        {
                send_to_char("You need a armourer's hammer to forge your armour.\n\r", ch);
                return;
        }

        found = FALSE;

        for (anvil = ch->in_room->contents; anvil; anvil = anvil->next_content)
        {
                if (anvil->item_type == ITEM_ANVIL)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no anvil here!\n\r", ch);
                return;
        }

        if (get_curr_dex(ch) < 20 || ch->pcdata->condition[COND_DRUNK] > 0)
        {
                send_to_char("Your hands aren't steady enough to safely forge your armour.\n\r", ch);
                return;
        }

        if (number_percent() > ch->pcdata->learned[gsn_forge])
        {
                send_to_char("You slip while hammering your armour and pound yourself!\n\r", ch);
                damage(ch, ch, ch->level, gsn_forge, FALSE);
                act ("$n pounds $mself while forging $s armour!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (in_c_room)
                send_to_char("{CYour use of specialised crafting tools improves your forging!\n\r{x", ch);

        act ("You skillfully enhance $P with $p!", ch, wobj, obj, TO_CHAR);
        act ("$n skillfully enhances $P using $p!", ch, wobj, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_FORGED);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }


        paf->type           = gsn_forge;
        paf->duration       = -1;
        paf->location       = APPLY_AC;
        paf->modifier       = (in_c_room) ? 0 - ( ch->level / ( ( 5 * 100 ) / mod_room_bonus ) ) : 0 - ch->level / 5;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        obj->timer    = (in_c_room) ? ( ( 30  * mod_room_bonus ) / 100 ) * (ch->level / 15) + 60 : 30 * (ch->level / 15) + 60;  /* 1-2 real hours */
        obj->timermax = obj->timer; 
        set_obj_owner(obj, ch->name);
}


void do_breathe (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_breathe))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->form != FORM_HYDRA && ch->form != FORM_DRAGON)
        {
                send_to_char("You are not in the correct form.\n\r",ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (ch->form == FORM_DRAGON && (arg[0] == '\0'))
        {
                send_to_char("Breathe fire, lightning, acid or gas?\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You breathe on yourself - phew bad breath!\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (ch->form == FORM_DRAGON)
        {
                if (! str_cmp( arg, "fire" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "fire breath" ) ) < 0 )
                                return;
                        WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                        arena_commentary("$n breathes fire on $N.", ch, victim);
                        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                        return;
                }

                if ( !str_cmp( arg, "lightning" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "lightning breath" ) ) < 0 )
                                return;
                        WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                        arena_commentary("$n breathes lightning on $N.", ch, victim);
                        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                        return;
                }

                if ( !str_cmp( arg, "frost" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "frost breath" ) ) < 0 )
                                return;
                        arena_commentary("$n breathes frost on $N.", ch, victim);
                        WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                        return;
                }

                if ( !str_cmp( arg, "gas" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
                                return;
                        WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                        arena_commentary("$n breathes gas.", ch, ch);
                        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                        return;
                }

                if ( !str_cmp( arg, "acid" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "acid breath" ) ) < 0 )
                                return;
                        arena_commentary("$n breathes acid on $N.", ch, victim);
                        WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                        return;
                }
        }

        if (number_percent() > ch->pcdata->learned[gsn_breathe])
        {
                send_to_char("You failed.\n\r",ch);
                return;
        }

        if (ch->form == FORM_HYDRA)
        {
                WAIT_STATE(ch, skill_table[gsn_breathe].beats);
                act ("You breathe your fury on $N!", ch, NULL, victim, TO_CHAR);
                act ("$n breathes at you!", ch, NULL, victim, TO_VICT);
                act ("$n breathes at $N!",  ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n breathes on $N.", ch, victim);

                if (ch->pcdata->learned[gsn_form_hydra] > 50)
                        spell_colour_spray(gsn_breathe, ch->level, ch, victim);
                if (ch->pcdata->learned[gsn_form_hydra] > 75)
                        spell_fireball(gsn_breathe, ch->level, ch, victim);
                if (ch->pcdata->learned[gsn_form_hydra] > 85)
                        spell_acid_blast(gsn_breathe, ch->level, ch, victim);
                return;
        }
}


void do_tail (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];
        char       buf [ MAX_STRING_LENGTH];
        int        chance;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_tail ))
        {
                send_to_char("You are not skilled enough to shadow someone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);

        if (arg[0] =='\0')
        {
                if (ch->pcdata->tailing)
                {
                        sprintf( buf, "You stop tailing %s.\n\r", ch->pcdata->tailing);
                        send_to_char( buf, ch);
                        ch->pcdata->tailing = 0;
                }
                else
                        send_to_char("Tail whom?\n\r", ch);
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                sprintf(buf, "You are unable to locate %s.\n\r", arg );
                send_to_char( buf, ch );
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("You can only tail players.\n\r", ch);
                return;
        }

        if (is_entered_in_tournament(ch)
            && tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_still_alive_in_tournament(ch))
        {
                send_to_char("Not during the tournament!\n\r", ch);
                return;
        }


        sprintf(buf, "You attempt to tail %s.\n\r", victim->name);
        send_to_char(buf,ch);

        ch->pcdata->tailing = 0;

        chance = ch->pcdata->learned[gsn_tail] + ((ch->level - victim->level) * 5);
        if (chance > 95)
                chance = 95;

        if ( number_percent() < chance )
                ch->pcdata->tailing = str_dup(victim->name);

        WAIT_STATE(ch, PULSE_VIOLENCE);
}


void do_stalk (CHAR_DATA *ch, char *argument)
{
        /*  Identical to tail but with aesthetics changed for vampires  */

        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];
        char       buf [ MAX_STRING_LENGTH];
        int        chance;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_stalk ))
        {
                send_to_char("You are not skilled enough to stalk someone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);

        if (arg[0] =='\0')
        {
                if (ch->pcdata->tailing)
                {
                        sprintf( buf, "You stop stalking %s.\n\r", ch->pcdata->tailing);
                        send_to_char( buf, ch);
                        ch->pcdata->tailing = 0;
                }
                else
                        send_to_char("Stalk whom?\n\r", ch);
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) )
        {
                sprintf(buf, "You are unable to locate %s.\n\r", arg );
                send_to_char( buf, ch );
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("You can only stalk players.\n\r", ch);
                return;
        }

        if (is_entered_in_tournament(ch)
            && tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_still_alive_in_tournament(ch))
        {
                send_to_char("Not during the tournament!\n\r", ch);
                return;
        }

        sprintf(buf, "You attempt to stalk %s.\n\r", victim->name);
        send_to_char(buf,ch);

        ch->pcdata->tailing = 0;

        chance = ch->pcdata->learned[gsn_stalk] + ((ch->level - victim->level) * 5);
        if (chance > 95)
                chance = 95;

        if ( number_percent() < chance )
                ch->pcdata->tailing = str_dup(victim->name);

        WAIT_STATE(ch, PULSE_VIOLENCE);
}


void do_throw (CHAR_DATA *ch, char *argument)
{
        /*
         *  Why, when, what next?
         */
        return;
}


/*
 *  Intimidate - Shade Sept 99
 */
void do_intimidate (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_intimidate))
        {
                send_to_char("You try to look intimidating... but no one's fooled.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_DETER))
                return;

        af.type      = gsn_intimidate;
        af.duration  = number_range(1, ch->level / 20 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETER;
        affect_to_char(ch, &af );

        send_to_char("You are a god, no mortal can touch you!\n\r", ch);
        return;
}


void do_extort (CHAR_DATA *extortionist, char *targetList)
{
        CHAR_DATA *victim;
        CHAR_DATA *guardian_mob;
        SHOP_DATA *store;
        COIN_DATA  coins;
        char       buf [ MAX_INPUT_LENGTH ];
        char       target [ MAX_INPUT_LENGTH  ];
        int        percent;
        int        chance;
        int        amount_gained;
        int        victims_coins;
        int        special_event;
        int        index;
        int        number_of_guards;

        if (IS_NPC(extortionist))
                return;

        store = NULL;
        one_argument( targetList, target);

        if( !CAN_DO( extortionist, gsn_extort))
        {
                send_to_char("A budding extortionist eh?  Well your scam needs some refining.\n",extortionist);
                return;
        }

        if( target[0] == '\0')
        {
                send_to_char("Who did you want to extort money from?\n\r", extortionist);
                return;
        }

        victim = get_char_room( extortionist, target);

        if( !victim)
        {
                send_to_char("You muscle on up to, hrmm... now where did they go?\n\r", extortionist);
                return;
        }

        if( !can_see( extortionist, victim))
        {
                act("There must be something wrong with your eyes, you can't see $n here.\n\r",
                    victim, NULL, extortionist, TO_VICT);
                return;
        }

        if( !IS_NPC( victim))
        {
                act("You muscle on up to $n; they dont seem amused at your act.\n",
                    victim, NULL, extortionist, TO_VICT);
                act("$n tries some bully tactics on you in an attempt to get money.  Isn't it sad?\n",
                    extortionist, NULL, victim, TO_VICT);
                return;
        }

        store = victim->pIndexData->pShop;

        if( store
           || IS_SET( victim->pIndexData->act, ACT_BANKER)
           || IS_SET( victim->pIndexData->act, ACT_GAMBLE)
           || IS_SET( victim->pIndexData->act, ACT_IS_HEALER)
           || victim->spec_fun == spec_lookup("spec_cast_hooker")
           || victim->spec_fun == spec_lookup("spec_thief"))
        {
                /* is there a guard in the room? */
                for (guardian_mob = extortionist->in_room->people;
                     guardian_mob;
                     guardian_mob = guardian_mob->next_in_room)
                {
                        if( guardian_mob->deleted)
                                continue;
                        if( IS_NPC( guardian_mob)
                           && IS_AWAKE( guardian_mob)
                           && guardian_mob->spec_fun == spec_lookup("spec_guard"))
                        {
                                act("You commend $N on their staunch vigil against crime, and decide to return when they are not around.\n\r ", extortionist, NULL, guardian_mob, TO_CHAR);
                                return;
                        }
                }

                /* No guard?, well lets get some money ... */
                WAIT_STATE( extortionist, skill_table[ gsn_extort].beats);
                percent = number_percent();
                chance = (( extortionist->level + extortionist->pcdata->learned[ gsn_extort]) / 4 )
                        - (( victim->level - extortionist->level) / 2);
                special_event = 10;

                if( percent <= chance)
                {
                        act("You muscle on up to $N, wondering what monetary value they put on their safety.",
                            extortionist, NULL, victim, TO_CHAR);
                        act("$n muscle's on up to $N, you wonder what they could be doing.",
                            extortionist, NULL, victim, TO_NOTVICT);
                        amount_gained = 1 + (( chance + number_range( 1, 400)) * 5);
                        sprintf(buf, "They grudgingly part with %d copper coins for their continued peace of mind.\n\r", amount_gained);
                        send_to_char(buf, extortionist);
                        act("$N gives $n some copper in exchange for their continued peace of mind.",
                            extortionist, NULL, victim, TO_NOTVICT);
                        extortionist->copper += amount_gained;

                        if( amount_gained > 2000)
                        {
                                act("With a BANZAI!!! and a mad glint in their eyes $n goes for your throat!",
                                    victim, NULL, extortionist, TO_VICT);
                                act("With a BANZAI!!! and a mad glint in their eyes $n goes for $N's throat!",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                send_to_char("Guess you shouldn't have taken their last copper huh?\n\r", extortionist);
                                special_event = number_range(1, 3);
                        }

                        if( amount_gained <= 2000 && amount_gained > 1500)
                        {
                                act("$n pleads you to leave them enough coin to feed their family.",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("You make them grovel piteously before tossing them a copper back.", extortionist);
                                send_to_char("Sensitivity is your middle name, isn't it?", extortionist);
                                act("$N makes $n grovel piteously for one lousy copper piece.",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                special_event = number_range(3, 7);
                        }

                        if( amount_gained <= 1500 && amount_gained > 1000)
                        {
                                act("$n mutters an oath, cursing their lack of security.",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("You're sure you just heard them threaten to hire some real guards, what a cheek!", extortionist);
                                act("Muttering and cursing, $n vows one day they will have to hire some guards.",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                special_event = number_range(2, 10);
                        }

                        if(( amount_gained <= 1000) && ( amount_gained > 750))
                        {
                                act("You grind your teeth, as $n whines about how unfair life is.",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("Give them a slap in the head to hurry them along, time is money after all!", extortionist);
                                act("$n goes on and on to $N about how unfair life is.",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                special_event = number_range(5, 10);
                        }

                        if(( amount_gained <= 750) && ( amount_gained > 500))
                        {
                                act("You hide your surprise as $n scowls at you fearsomely.",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("Have they found some backbone after all? Is it time to find a new source of income?", extortionist);
                                act("$n levels a fearsome scowl at $N.  Now might be a good time to leave.",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                special_event = number_range(4, 10);
                        }

                        if(( amount_gained <= 500) && ( amount_gained > 100))
                        {
                                act("You sense quite a bit of hostility from $n, better make this quick!",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("They're just lucky that you are very busy today, otherwise things\n could have gotten a whole lot uglier!\n\r",
                                             extortionist);
                                act("The hostility emanating from $n is frightening, wonder what $N did?",
                                    victim, NULL, extortionist, TO_NOTVICT);
                                special_event = number_range(3, 10);
                        }

                        if( amount_gained <= 100)
                        {
                                act("$n happily hands over the money, $m is very pleased with your service so far.",
                                    victim, NULL, extortionist, TO_VICT);
                                send_to_char("How you gonna build a reputation as a hard-ass when they smile at ya?\n",
                                             extortionist);
                                act("$n smiles happily at $N as $m slips $N a small parcel. Wonder what it was?",
                                    victim, NULL, extortionist, TO_NOTVICT);
                        }
                }
                else
                {
                        act("Your attempt to increase $n's awareness of social issues such as personal", victim, NULL, extortionist, TO_VICT);
                        act("safety through sound protection plans and suddenly realise you, yourself", victim, NULL, extortionist, TO_VICT);
                        act("are a small player in the game of life... scary ain't it?", victim, NULL, extortionist, TO_VICT);
                        act("$n convinces you that your best interest lies in paying $m a small amount", victim, NULL, extortionist, TO_VICT);
                        act("of money to remain safe.", victim, NULL, extortionist, TO_VICT);

                        amount_gained = (75 + percent + chance) * 5;
                        victims_coins = extortionist->copper + extortionist->silver * 10
                                + extortionist->gold * 100 + extortionist->plat * 1000;

                        if( amount_gained > victims_coins)
                        {
                                send_to_char("You part with all your coins!!!\n", extortionist);
                                extortionist->copper = 0;
                                extortionist->silver = 0;
                                extortionist->gold   = 0;
                                extortionist->plat   = 0;
                        }
                        else
                        {
                                /* subtract the amount lost from the characters total, then convert back to coins */
                                victims_coins -= amount_gained;
                                coins.plat   = 0 + div( victims_coins, 1000).quot;
                                victims_coins -= coins.plat * 1000;
                                coins.gold   = 0 + div( victims_coins, 100).quot;
                                victims_coins -= coins.gold * 100;
                                coins.silver = 0 + div( victims_coins, 10).quot;
                                coins.copper = 0 + div( victims_coins, 10).rem;
                                sprintf(buf, "You part with %d coins.\n", amount_gained);
                                send_to_char(buf, extortionist);

                                send_to_char("A small price to pay for your peace of mind, wouldn't you agree?\n", extortionist);
                                extortionist->copper = coins.copper;
                                extortionist->silver = coins.silver;
                                extortionist->gold   = coins.gold;
                                extortionist->plat   = coins.plat;
                        }
                }

                number_of_guards = 0;

                switch( special_event)
                {
                    case 1 :
                    case 2 :
                        multi_hit(victim, extortionist, TYPE_UNDEFINED);
                        break;

                    case 3 :
                        number_of_guards = number_range(6, 10);

                    case 4 :
                        if( number_of_guards == 0)
                                number_of_guards = number_range( 2, 5);

                    case 5 :
                        if( number_of_guards == 0)
                                number_of_guards = number_range( 1, 3);
                        act("\n\rRAID!!\n\r", extortionist, NULL, victim, TO_ROOM);
                        send_to_char("You've been set up!\n\r", extortionist);
                        multi_hit( victim, extortionist, TYPE_UNDEFINED);
                        for( index = 1; index <= number_of_guards; index++ )
                        {
                                char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
                                             extortionist->in_room );
                        }
                        break;

                    case 6 :
                        send_to_char("Damn, here comes the law, better look innocent.\n\r", extortionist);
                        char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
                                     extortionist->in_room );
                        break;

                default :
                        break;
                }
                return;
        }
        else
        {
                send_to_char("They turn and walk away before you can catch up with them.\n\r", extortionist);
                return;
        }
}


/*
 *  Bard skills : Shade, Geshp, Owl Jan 2000
 */

void do_chant(CHAR_DATA *ch, char *arg)
{
        AFFECT_DATA     af;
        CHAR_DATA       *victim;
        int             chance, dam;

        if (IS_NPC(ch))
                return;

        if (ch->sub_class != SUB_CLASS_BARD)
        {
                send_to_char( "You'd better leave the chanting up to the Bards.\n\r", ch );
                return;
        }

        if( arg[0] == '\0' )
        {
                send_to_char( "What do you want to chant?\n\r", ch );
                return;
        }

        if( is_name( arg, "protection" ) )
        {
                if( ch->fighting )
                {
                        send_to_char( "You can't chant that while you're fighting!\n\r", ch );
                        return;
                }

                act( "$n begins to chant softly.", ch, NULL, NULL, TO_ROOM );

                if( number_percent() > ch->pcdata->learned[gsn_chant_of_protection] )
                {
                        send_to_char( "Your chant of protection fails miserably.\n\r", ch );
                        return;
                }

                if (is_affected(ch, gsn_chant_of_protection ))
                        return;

                send_to_char( "You complete your chant of protection.\n\r", ch );
                affect_strip(ch, gsn_sanctuary);

                af.type         = gsn_sanctuary;
                af.duration     = 2 + ch->level/10;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_SANCTUARY;
                affect_to_char( ch, &af );

                af.type         = gsn_chant_of_protection;
                af.location     = APPLY_AC;
                af.modifier     = 0 - ch->level/2;
                af.bitvector    = 0;
                affect_to_char( ch, &af );

                af.location     = APPLY_SAVING_SPELL;
                af.modifier     = 0 - ch->level/10;
                affect_to_char( ch, &af );
        }

        else if( is_name( arg, "battle" ) )
        {
                if( ch->fighting )
                {
                        send_to_char( "You can't chant that while you're fighting!\n\r", ch );
                        return;
                }

                act( "$n begins to chant loudly.", ch, NULL, NULL, TO_ROOM );

                if( number_percent() > ch->pcdata->learned[gsn_chant_of_battle] )
                {
                        send_to_char( "Your battle chant fails miserably.\n\r", ch );
                        return;
                }

                if( is_affected( ch, gsn_chant_of_battle ) )
                        return;

                send_to_char( "You complete your battle chant.\n\r", ch );

                af.type         = gsn_chant_of_battle;
                af.duration     = 3 + ch->level/15;
                af.location     = APPLY_HITROLL;
                af.modifier     = ch->level/5;
                af.bitvector    = 0;
                affect_to_char( ch, &af );

                af.location     = APPLY_DAMROLL;
                affect_to_char( ch, &af );
        }

        else if( is_name( arg, "vigour" ) )
        {
                if( ch->fighting )
                {
                        send_to_char( "You can't chant that while you're fighting!\n\r", ch );
                        return;
                }

                act( "$n begins to chant loudly.", ch, NULL, NULL, TO_ROOM );

                if( number_percent() > ch->pcdata->learned[gsn_chant_of_vigour] )
                {
                        send_to_char( "Your chant of vigour fails miserably.\n\r", ch );
                        return;
                }

                if( is_affected( ch, gsn_chant_of_vigour ) )
                        return;

                send_to_char( "You complete your chant of vigour.\n\r", ch );
                affect_strip(ch, gsn_haste);

                af.type         = gsn_haste;
                af.duration     = 2 + ch->level/15;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = 0;
                affect_to_char( ch, &af );

                af.type         = gsn_chant_of_vigour;
                af.location     = APPLY_DEX;
                af.modifier     = ch->level/30 + 1;
                affect_to_char( ch, &af );
        }

        else if( is_name( arg, "enfeeblement" ) )
        {
                victim = ch->fighting;

                if( !victim )
                {
                        send_to_char( "You're not fighting anyone!\n\r", ch );
                        return;
                }

                act( "$n recites a vicious-sounding chant.", ch, NULL, NULL, TO_ROOM );
                WAIT_STATE( ch, PULSE_VIOLENCE );

                chance = URANGE( 5, ch->pcdata->learned[gsn_chant_of_enfeeblement],95 );

                if( is_affected( victim, gsn_chant_of_enfeeblement )
                   || saves_spell( ch->level, victim ) || number_percent() > chance )
                {
                        send_to_char( "Your chant of enfeeblement has no effect.\n\r", ch );
                        return;
                }

                act( "Your chant of enfeeblement weakens $N!", ch, NULL, victim, TO_CHAR );
                act( "$n's chant of enfeeblement weakens $N!", ch, NULL, victim, TO_ROOM );
                arena_commentary("$N is enfeebled by $n's chant.", ch, victim);

                af.type         = gsn_chant_of_enfeeblement;
                af.duration     = ch->level/12;
                af.location     = APPLY_HITROLL;
                af.modifier     = - ch->level / 4;
                af.bitvector    = 0;
                affect_to_char( victim, &af );

                af.location     = APPLY_DAMROLL;
                affect_to_char( victim, &af );

                af.location     = APPLY_STR;
                af.modifier     = - ch->level / 8;
                affect_to_char( victim, &af );
        }

        else if( is_name( arg, "pain" ) )
        {
                victim = ch->fighting;

                if( !victim )
                {
                        send_to_char( "You're not fighting anyone!\n\r", ch );
                        return;
                }

                act( "$n recites a vicious-sounding chant.", ch, NULL, NULL, TO_ROOM );
                WAIT_STATE( ch, PULSE_VIOLENCE );

                chance = URANGE( 5, ch->pcdata->learned[gsn_chant_of_pain], 95 );

                if( number_percent() > chance )
                {
                        send_to_char( "Your chant of pain has no effect.\n\r", ch );
                        return;
                }

                arena_commentary("$n's chant inflicts pain on $N.", ch, victim);
                dam = dice( ch->level, 6 ) + ch->level;

                if( saves_spell( ch->level, victim ) )
                        dam /= 2;

                damage( ch, victim, dam, gsn_chant_of_pain, FALSE );
        }

        else send_to_char( "You don't know that chant.\n\r", ch );
}


/*
 *  checks character or group member has song_of_tranquility
 */
bool has_tranquility(CHAR_DATA *ch)
{
        CHAR_DATA *gch;

        for (gch = ch->in_room->people;gch;gch = gch->next_in_room)
        {
                if (!IS_NPC(gch)
                    && is_affected(gch, gsn_song_of_tranquility)
                    && is_same_group(ch, gch))
                {
                        return number_percent() < gch->pcdata->learned[gsn_song_of_tranquility];
                }
        }

        return FALSE;
}


int remove_songs(CHAR_DATA *ch)
{
        AFFECT_DATA     *paf;
        char            buf[MAX_STRING_LENGTH];
        int             i;
        int             removed = 0;

        for (paf = ch->affected ; paf; paf = paf->next)
        {
                if (paf->deleted)
                        continue;

                for (i = 0; i < MAX_SONGS; i++)
                {
                        if (paf->type == skill_lookup(song_table[i].affect))
                        {
                                sprintf(buf, "You stop singing '%s'.", song_table[i].long_desc);
                                act(buf, ch, NULL, NULL, TO_CHAR);

                                sprintf(buf, "$n stops playing '%s'.", song_table[i].long_desc);
                                act(buf, ch, NULL, NULL, TO_ROOM);

                                removed = 1;
                                affect_remove(ch, paf);
                        }
                }
        }

        return removed;
}


void do_sing(CHAR_DATA *ch, char* arg)
{
        int             found, i;
        AFFECT_DATA     af;
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA        *obj;

        if (IS_NPC(ch))
                return;

        if (ch->sub_class != SUB_CLASS_BARD)
        {
                send_to_char("You scream and screech for a bit, but nothing seems to happen.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You can't sing while fighting.\n\r", ch);
                return;
        }

        if( arg[0] == '\0' )
        {
                if (!remove_songs(ch))
                {
                        send_to_char("What do you want to sing?\n\r", ch);
                        return;
                }
                return;
        }

        found = -1;
        for (i = 0; i < MAX_SONGS; i++)
        {
                if (is_name(arg, song_table[i].name))
                {
                        found = i;
                        break;
                }
        }

        if (found == -1)
        {
                send_to_char("You're not familiar with that song.\n\r", ch);
                return;
        }

        obj = get_eq_char(ch, WEAR_HOLD);
        if (!obj || obj->item_type != ITEM_INSTRUMENT)
        {
                send_to_char("You must hold an instrument to sing songs.\n\r", ch);
                return;
        }

        if( !ch->pcdata->learned[skill_lookup(song_table[i].affect)] )
        {
                send_to_char( "You don't know how to sing that song.\n\r", ch );
                return;
        }

        remove_songs(ch);

        sprintf(buf, "You start to play '%s' on %s.", song_table[i].long_desc,obj->short_descr);
        act(buf, ch, NULL, NULL, TO_CHAR);

        sprintf(buf, "$n starts to play '%s' on %s.", song_table[i].long_desc,obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);

        af.type      = skill_lookup(song_table[i].affect);
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = song_table[i].apply;

        affect_to_char( ch, &af );

        return;
}


/*
 *  Ranger skills : Shade, Geshp, Owl Jan 2000
 */

void do_dowse(CHAR_DATA *ch, char *arg)
{
        OBJ_DATA *spring;

        if( IS_NPC(ch) )
                return;

        if( !CAN_DO( ch, gsn_dowse ) )
        {
                send_to_char( "You wouldn't know how to dowse for water.\n\r", ch );
                return;
        }

        if( ch->in_room->sector_type != SECT_FOREST
           && ch->in_room->sector_type != SECT_FIELD
           && ch->in_room->sector_type != SECT_HILLS
           && ch->in_room->sector_type != SECT_MOUNTAIN )
        {
                send_to_char( "You wouldn't find a water source in this terrain.\n\r", ch );
                return;
        }

        if( number_percent() > ch->pcdata->learned[gsn_dowse] )
        {
                send_to_char( "You're unable to locate a water source.\n\r", ch );
                act( "$n searches for a water source, but finds none.", ch, NULL,
                        NULL, TO_ROOM );
        }
        else
        {
                spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
                spring->timer    = ch->level;
                spring->timermax = spring->timer;
                obj_to_room( spring, ch->in_room );
                send_to_char( "You search for a water source and find a small spring.\n\r", ch);
                act( "$n searches for a water source, and locates a small spring.",
                    ch, NULL, NULL, TO_ROOM );
        }

        WAIT_STATE( ch, PULSE_VIOLENCE );
}


void do_gather (CHAR_DATA *ch, char *arg)
{
        OBJ_DATA        *herb;
        int             herb_type;
        int             herb_dose;
        int             random_herb;
        int             move_cost;
        bool            random;
        int             i;
        char            buf[ MAX_STRING_LENGTH ];

        if( IS_NPC(ch) )
                return;

        if( !CAN_DO( ch, gsn_gather_herbs ) )
        {
                send_to_char( "You wouldn't know which herbs to gather.\n\r", ch );
                return;
        }

        if( ch->in_room->sector_type != SECT_FOREST
           && ch->in_room->sector_type != SECT_FIELD
           && ch->in_room->sector_type != SECT_HILLS
           && ch->in_room->sector_type != SECT_MOUNTAIN )
        {
                send_to_char( "You wouldn't find any herbs in this terrain.\n\r", ch );
                return;
        }

        move_cost = number_range( 50, 75 );
        if( ch->move - move_cost < 1 )
        {
                send_to_char( "You're too tired to search for healing herbs.\n\r", ch );
                return;
        }
        ch->move -= move_cost;
        WAIT_STATE( ch, PULSE_VIOLENCE );

        herb_type = -1;
        random_herb = -1;
        random = FALSE;

        if( arg[0] == '\0' )
        {
                random = TRUE;
                for( i=0; i < 10; i++ )
                {
                        random_herb = number_range( 0, MAX_HERBS-1 );
                        if( ch->pcdata->learned[ gsn_gather_herbs ]
                           >= herb_table[ random_herb ].min_skill )
                        {
                                herb_type = random_herb;
                                break;
                        }
                }
        }
        else for( i=0; i<MAX_HERBS; i++ )
        {
                if( is_full_name( arg, herb_table[i].name ) )
                {
                        herb_type = i;
                        break;
                }
        }

        if( herb_type < 0 )
        {
                if( random )
                        send_to_char( "You are unable to find any useful herbs.\n\r", ch );
                else
                        send_to_char( "You search the area but cannot find that herb.\n\r", ch );
                act( "$n searches for healing herbs, but comes up empty handed.", ch, NULL,
                        NULL, TO_ROOM );
                return;
        }

        i = herb_table[ herb_type ].chance * 10;
        /* if( random ) i /= 2;  removed for now */

        if( number_range( 0, 999 ) >= i )
        {
                if( random )
                        send_to_char( "You are unable to find any useful herbs.\n\r", ch );
                else
                {
                        sprintf( buf, "You search the area but cannot find any %s.\n\r",
                                herb_table[ herb_type ].name );
                        send_to_char( buf, ch );
                }
                act( "$n searches for healing herbs, but comes up empty handed.", ch, NULL,
                    NULL, TO_ROOM );
                return;
        }

        i = number_percent();
        if( i < 55 )
                herb_dose = 1;
        else if ( i < 90 )
                herb_dose = 2;
        else
                herb_dose = 3;

        send_to_char( herb_table[ herb_type ].action, ch );

        if( herb_dose > 1 )
        {
                sprintf( buf, "You find enough %s to prepare %d doses.\n\r",
                        herb_table[ herb_type ].name, herb_dose );
                send_to_char( buf, ch );
        }
        sprintf( buf, "$n searches the area for healing herbs and finds some %s.",
                herb_table[ herb_type ].name );
        act( buf, ch, NULL, NULL, TO_ROOM );

        for( i=0; i<herb_dose; i++ )
        {
                herb = create_object( get_obj_index( ITEM_VNUM_HERB ), 0 );
                free_string(herb->name);
                free_string(herb->short_descr);
                free_string(herb->description);
                herb->name              = str_dup( herb_table[ herb_type ].keywords );
                herb->short_descr       = str_dup( herb_table[ herb_type ].short_desc );
                herb->description       = str_dup( herb_table[ herb_type ].long_desc );
                herb->item_type         = herb_table[ herb_type ].type;
                herb->timer             = number_range( 200, 250 );
                herb->timermax          = herb->timer;
                herb->level             = ch->level;
                herb->value[0]          = 10;
                herb->value[1]          = skill_lookup( herb_table[ herb_type ].spell );
                herb->value[2]          = -1;
                herb->value[3]          = -1;

                if( ch->carry_number >= can_carry_n( ch ) )
                        obj_to_room( herb, ch->in_room );
                else obj_to_char( herb, ch );
        }

        return;
}

/* New Smelt Code - Brutus */


void do_smelt (CHAR_DATA *ch, char *argument)
{
       
        /*  ROOM_INDEX_DATA *pRoomIndex; */
        char            arg[MAX_INPUT_LENGTH];
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA        *obj;
        /*   int             percent; */
        int             starmetal=0;
        int             electrum=0;
        int             adamantite=0;
        int             mithral=0;
        int             steel=0;

        argument = one_argument(argument, arg);
        if (IS_NPC(ch))
        return;

        if (!CAN_DO(ch, gsn_smelt))
        {
                send_to_char("You don't know how to smelt anything.\n\r", ch);
                return;
        }

         if (arg[0] == '0')
        {
                send_to_char("Smelt what?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("Not while fighting.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
        {
                send_to_char("you can't see anything.\n\r", ch);
                return;
        }

        
        if (arg[0] == '\0' || !str_cmp(arg, ch->name))
        {
                send_to_char("You're unlikely to contain any elements of value.\n\r", ch);
                act("$n considers smelting $mself, but determins they hold no value.",
                    ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_NODROP))
        {
                send_to_char("You can't let go of it!\n\r", ch);
                return;
        }

        if ((obj->item_type == ITEM_WEAPON) && (number_percent() <= ch->pcdata->learned[gsn_smelt]))
        {
                if (number_percent() >= 98)
                        starmetal = (obj->level/60);
                if (number_percent() >= 94)
                        electrum = (obj->level/45);
                if (number_percent() >= 75)
                        adamantite = (obj->level/30);
                if (number_percent() >= 50)
                        mithral = (obj->level/10);
                if (number_percent() >= 30)
                        steel = (obj->level/3);

                ch->smelted_steel = (ch->smelted_steel + steel);
                ch->smelted_mithral = (ch->smelted_mithral + mithral);
                ch->smelted_adamantite = (ch->smelted_adamantite + adamantite);
                ch->smelted_electrum = (ch->smelted_electrum + electrum);
                ch->smelted_starmetal = (ch->smelted_starmetal + starmetal);
        }
        else if (obj->item_type == ITEM_ARMOR && (number_percent() <= ch->pcdata->learned[gsn_smelt]))
        {
                if (number_percent() >= 98)
                        starmetal = (obj->level/60);
                if (number_percent() >= 94)
                        electrum = (obj->level/45);
                if (number_percent() >= 75)
                        adamantite = (obj->level/30);
                if (number_percent() >= 50)
                        mithral = (obj->level/10);
                if (number_percent() >= 30)
                        steel = (obj->level/3);

                ch->smelted_steel = (ch->smelted_steel + steel);
                ch->smelted_mithral = (ch->smelted_mithral + mithral);
                ch->smelted_adamantite = (ch->smelted_adamantite + adamantite);
                ch->smelted_electrum = (ch->smelted_electrum + electrum);
                ch->smelted_starmetal = (ch->smelted_starmetal + starmetal);
        }
        else
        {
                send_to_char("You can't Smelt that!\n\r", ch);
                return;
        }

        act("$n Smelts $p into its raw materials.", ch, obj, NULL, TO_ROOM);
        act("You place $p into the Forge.", ch, obj, NULL, TO_CHAR);        
        sprintf(buf, "You recover the following raw materials: \nSteel: %d\nMithral: %d\nAdamantite: %d\nElectrum: %d\nStarmetal: %d\n\r", steel, mithral, adamantite, electrum, starmetal);
        send_to_char (buf, ch);
        extract_obj(obj);        
}


void do_imbue (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA        *imbue_types;
        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        OBJ_DATA *anvil;
        char            arg[MAX_INPUT_LENGTH];
        char            buf[ MAX_STRING_LENGTH ];
 
        int             modifier;
        int             random_buff;
        int             i;
        int obj_craft_bonus;
        int mod_room_bonus;
        AFFECT_DATA *paf;
        bool found;
        bool in_c_room;
        bool            random;

        in_c_room = FALSE;
        modifier = -1;
        random_buff = -1;
        random = FALSE;

        obj_craft_bonus = get_craft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_FORGE + obj_craft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_c_room = TRUE;
        }

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_imbue))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);
 
        if( arg[0] == '\0' )
        {
                send_to_char("Imbue what?\n\r", ch);
                return;
        }
       
        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that weapon.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That item is not a weapon.\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO) && (obj->ego_flags, EGO_ITEM_IMBUED))
        {
                send_to_char("That is already embued.\n\r", ch);
                return;
        }

        random_buff = number_range( 1, MAX_IMBUE);
        /* random_buff = (const char *rand_string = imbue_list[rand() % MAX_IMBUE]);
*/
       SET_BIT(obj->extra_flags, ITEM_EGO);
       SET_BIT(obj->ego_flags, EGO_ITEM_IMBUED);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_imbue;
        paf->duration       = -1;
        if (random_buff = 1) 
                paf->location       = APPLY_DAMROLL;
        if (random_buff  = 2)
                paf->location   = APPLY_HITROLL;
        if (random_buff  = 3)
                paf->location   = APPLY_AC;
        if (random_buff  = 4)
                 paf->location   = APPLY_STR;
        if (random_buff  = 5)
                paf->location   = APPLY_DEX;
        if (random_buff  = 6)
                paf->location   = APPLY_MANA;
        if (random_buff  = 7)
                paf->location   = APPLY_MOVE;
        else paf->location   = APPLY_AC;

        paf->modifier       = ( in_c_room ) ? 2 + ( ch->level / ( ( 5 * 100 ) / mod_room_bonus ) ) : 2 + ch->level / 5;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }
        paf->location           = APPLY_AC;
        paf->modifier       = ( in_c_room ) ? 2 + ( ch->level / ( ( 5 * 100 ) / mod_room_bonus ) ) : 2 + ch->level / 5;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;


        set_obj_owner(obj, ch->name);

        act ("You immerse the $p within the flames of the Forge, Imbuing it with power!", ch, obj, NULL, TO_CHAR);
        act ("$n immerses the $p within the flames of the Forge, Imbuing it with power!", ch, obj, NULL, TO_ROOM);
        
        return;
}


void do_counterbalance (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool in_c_room;
        int obj_craft_bonus;
        int mod_room_bonus;

        obj_craft_bonus = get_craft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_SHARPEN + obj_craft_bonus;

        in_c_room = FALSE;


        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_c_room = TRUE;
        }

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_counterbalance))
        {
                send_to_char("What do you think you are, a smith?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What do you want to counterbalance?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that weapon.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That item is not a weapon.\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_BALANCED))
        {
                send_to_char("That is already counterbalanced.\n\r", ch);
                return;
        }

        if (ch->pcdata->condition[COND_DRUNK] > 0)
        {
                send_to_char("Your hands aren't steady enough to safely sharpen your blade.\n\r", ch);
                return;
        }

        if (number_percent() > ch->pcdata->learned[gsn_counterbalance])
        {
                send_to_char("You slip while balancing your weapon!!\n\r", ch);
                act ("$n cuts $mself while balancing $m weapon!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (in_c_room)
                send_to_char("{CThe use of specialised tools improves the quality of your smithing!\n\r{x", ch);

        act ("You run $p down the blade of $P, creating a deadly weapon!",
             ch, wobj, obj, TO_CHAR);
        act ("$n runs $p down the blade of $P, creating a deadly blade!",
             ch, wobj, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_BALANCED);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_counterbalance;
        paf->duration       = -1;
        paf->location       = APPLY_BALANCE;
        paf->modifier       = (ch->pcdata->learned[gsn_counterbalance]) + (number_range(0,10));
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        set_obj_owner(obj, ch->name);
}

void do_classify( CHAR_DATA *ch, char *arg )
{
        OBJ_DATA *obj;
        char buf[MAX_STRING_LENGTH];
        char tmp[MAX_STRING_LENGTH];
        int i;
        bool found;

        if( IS_NPC(ch) )
                return;

        if( !CAN_DO( ch, gsn_classify ) )
        {
                send_to_char( "You don't know how to classify medicines.\n\r", ch );
                return;
        }

        if( arg[0] == '\0' )
        {
                send_to_char( "Examine what object?\n\r", ch );
                return;
        }

        obj = get_obj_carry( ch, arg );
        if( !obj )
        {
                send_to_char( "You aren't carrying that.\n\r", ch );
                return;
        }

        if( obj->item_type != ITEM_PILL
            && obj->item_type != ITEM_FOOD
            && obj->item_type != ITEM_PAINT
            && obj->item_type != ITEM_POTION )
        {
                send_to_char( "Your herb lore doesn't describe that type of object.\n\r", ch );
                return;
        }

        if( number_percent() > ch->pcdata->learned[gsn_classify] )
        {
                sprintf( buf, "You fail to discern %s's properties.\n\r", obj->short_descr );
                send_to_char( buf, ch );
                return;
        }

        if( obj->item_type == ITEM_FOOD )
        {
                sprintf( buf, "%s looks like food", capitalize( obj->short_descr ) );
                if( obj->value[3] != 0 )
                        strcat( buf, ".  It's probably not safe to consume." );
        }
        else
        {
                sprintf( buf, "%s looks like a ", capitalize( obj->short_descr ) );
                if( obj->item_type == ITEM_PILL )
                        strcat( buf, "pill" );
                else if( obj->item_type == ITEM_POTION )
                        strcat( buf, "potion" );
                else strcat( buf, "paint" );
                strcat( buf, ".\n\rEffects:" );
                found = FALSE;
                for( i=1; i < 4; i++ )
                {
                        if( obj->value[i] >= 0 && obj->value[i] < MAX_SKILL )
                        {
                                if( found )
                                        strcat( buf, "," );
                                found = TRUE;
                                sprintf( tmp, " %s", skill_table[obj->value[i]].name );
                                strcat( buf, tmp );
                        }
                }
                if( !found )
                        strcat( buf, " none" );
        }

        strcat( buf, ".\n\r" );
        send_to_char( buf, ch );
        WAIT_STATE( ch, PULSE_VIOLENCE );
}

bool is_bladed_weapon (OBJ_DATA *obj)
{
        if (!obj || obj->item_type != ITEM_WEAPON)
                return FALSE;

        if (obj->value[3] == 1
            || obj->value[3] == 2
            || obj->value[3] == 3
            || obj->value[3] == 11
            || obj->value[3] == 13
            || obj->value[3] == 16)
                return TRUE;

        return FALSE;
}


bool is_blunt_weapon (OBJ_DATA *obj)
{
        if (!obj || obj->item_type != ITEM_WEAPON)
                return FALSE;

        if (obj->value[3] == 6
            || obj->value[3] == 7
            || obj->value[3] == 8)
                return TRUE;

        return FALSE;
}


bool is_piercing_weapon (OBJ_DATA *obj)
{
        if (!obj || obj->item_type != ITEM_WEAPON)
                return FALSE;

        if (obj->value[3] == 2
            || obj->value[3] == 11)
                return TRUE;

        return FALSE;
}



