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

/* External function declarations */
void affect_modify      args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, OBJ_DATA *weapon ) );
int  total_coins_char   args( ( CHAR_DATA *ch ) );
void coins_from_char    args( ( int cost, CHAR_DATA *ch ) );

void atemi              args((CHAR_DATA *ch, CHAR_DATA *victim));
void kansetsu           args((CHAR_DATA *ch, CHAR_DATA *victim));
void tetsui             args((CHAR_DATA *ch, CHAR_DATA *victim));
void shuto              args((CHAR_DATA *ch, CHAR_DATA *victim));
void yokogeri           args((CHAR_DATA *ch, CHAR_DATA *victim));
void mawasigeri         args((CHAR_DATA *ch, CHAR_DATA *victim));
void tenketsu           args((CHAR_DATA *ch, CHAR_DATA *victim));

static inline void smelted_to_char_loss( const int cost[5],
                                         CHAR_DATA *ch,
                                         int coins_mode,
                                         int learned )
{
        int loss_pct;
        int lost[5];
        int j;

        if (learned < 1)   learned = 1;
        if (learned > 100) learned = 100;

        /* 1% -> 100% loss, 100% -> 10% loss (linear) */
        loss_pct = 100 - ((learned - 1) * 90) / 99;

        for (j = 0; j < 5; j++)
        {
                lost[j] = (cost[j] * loss_pct) / 100;

                if (lost[j] < 1 && cost[j] > 0)
                {
                        lost[j] = 1;
                }
        }

        smelted_to_char(lost[0], lost[1], lost[2], lost[3], lost[4], ch, coins_mode);
}

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

void do_bonus(CHAR_DATA *ch, char *argument)
{
        char       arg   [ MAX_INPUT_LENGTH  ];
        char       buf   [ MAX_STRING_LENGTH ];
        char       buf1  [ MAX_STRING_LENGTH ];
        long       current_lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
        long       current_lday = current_lhour / 24;

        long       last_lhour = (ch->pcdata->last_recharge - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
        long       last_lday = last_lhour / 24;
        int        must_sleep = 0;

        buf1[0] = '\0';

        one_argument(argument, arg);

        if (IS_NPC(ch))
        {
            return;
        }

        if (arg[0] == '\0')
        {
                /* report on bonus status here */
                if (ch->pcdata->max_bonus <= 0)
                {
                    send_to_char( "You have no access to bonus actions.\n\r", ch );
                    return;
                }

                sprintf(buf, "You have %d/%d bonus actions available. \n\r",
                        ch->pcdata->bonus,
                        ch->pcdata->max_bonus);

                strcat (buf1, buf);

                if ( ( ch->pcdata->max_bonus > 0 )
                &&   ( ch->pcdata->bonus < ch->pcdata->max_bonus ) )
                {
                    if (ch->pcdata->slept == 0)
                    {
                        must_sleep = 1;
                        sprintf(buf, "To regain your bonus actions you must sleep. \n\r");
                        strcat (buf1, buf);
                    }

                    if ((current_lday - last_lday) < 1)
                    {
                        if (must_sleep)
                        {
                            sprintf(buf, "You must also wait a full day until your bonus actions recharge.\r\n");
                            strcat (buf1, buf);
                        }
                        else {
                            sprintf(buf, "You must wait a full day until your bonus actions recharge.\r\n");
                            strcat (buf1, buf);
                        }
                    }
                }

                send_to_char(buf1,ch);
                return;

        }

        if ( !str_cmp( arg, "damage" ) )
        {
                if ( ch->pcdata->bonus < 1 )
                {
                        send_to_char("You don't have any bonus actions available.\n\r", ch);
                        return;
                }

                AFFECT_DATA af;

                if (IS_AFFECTED(ch, AFF_BONUS_DAMAGE))
                {
                        send_to_char("Your attacks are already doing extra damage.\n\r", ch);
                        return;
                }

                send_to_char("You temporarily increase the power of your attacks.\n\r", ch);
                act("$n seems suddenly more powerful.", ch, NULL, NULL, TO_ROOM);
                af.type         = gsn_bonus_damage;
                af.duration     = ch->pcdata->max_bonus;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_BONUS_DAMAGE;
                affect_join(ch, &af);
                ch->pcdata->bonus--;
                WAIT_STATE(ch, PULSE_PER_SECOND);
                return;
        }

        if ( !str_cmp( arg, "attack" ) )
        {
                if ( ch->pcdata->bonus < 1 )
                {
                        send_to_char("You don't have any bonus actions available.\n\r", ch);
                        return;
                }

                if ( ch->level < LEVEL_HERO )
                {
                        send_to_char("You don't have access to that bonus action yet.\n\r", ch);
                        return;
                }

                AFFECT_DATA af;

                if (IS_AFFECTED(ch, AFF_BONUS_ATTACK))
                {
                        send_to_char("You already receive a bonus attack.\n\r", ch);
                        return;
                }

                send_to_char("You temporarily gain an extra attack.\n\r", ch);
                act("$n starts moving faster.", ch, NULL, NULL, TO_ROOM);
                af.type         = gsn_bonus_attack;
                af.duration     = ch->pcdata->max_bonus;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_BONUS_ATTACK;
                affect_join(ch, &af);
                ch->pcdata->bonus--;
                WAIT_STATE(ch, PULSE_PER_SECOND);
                return;
        }

        if ( !str_cmp( arg, "resilience" ) )
        {
                if ( ch->pcdata->bonus < 1 )
                {
                        send_to_char("You don't have any bonus actions available.\n\r", ch);
                        return;
                }

                AFFECT_DATA af;

                if (IS_AFFECTED(ch, AFF_BONUS_RESILIENCE))
                {
                        send_to_char("You are already taking reduced damage.\n\r", ch);
                        return;
                }

                send_to_char("You are more resistant to damage.\n\r", ch);
                act("$n suddenly looks more durable.", ch, NULL, NULL, TO_ROOM);
                af.type         = gsn_bonus_resilience;
                af.duration     = ch->pcdata->max_bonus;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_BONUS_RESILIENCE;
                affect_join(ch, &af);
                ch->pcdata->bonus--;
                WAIT_STATE(ch, PULSE_PER_SECOND);
                return;
        }

        if ( !str_cmp( arg, "exotic" ) )
        {
                if ( ch->pcdata->bonus < 1 )
                {
                        send_to_char("You don't have any bonus actions available.\n\r", ch);
                        return;
                }

                if ( ch->level < 85 )
                {
                        send_to_char("You don't have access to that bonus action yet.\n\r", ch);
                        return;
                }

                AFFECT_DATA af;

                if (IS_AFFECTED(ch, AFF_BONUS_EXOTIC))
                {
                        send_to_char("You are already protected from exotic damage types.\n\r", ch);
                        return;
                }

                send_to_char("You feel more protected against exotic damage.\n\r", ch);
                act("$n grows sturdier.", ch, NULL, NULL, TO_ROOM);
                af.type         = gsn_bonus_exotic;
                af.duration     = ch->pcdata->max_bonus;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_BONUS_EXOTIC;
                affect_join(ch, &af);

                af.type      = gsn_bonus_exotic;
                af.duration  = ch->pcdata->max_bonus;
                af.modifier  = -ch->level / 6;
                af.location  = APPLY_SAVING_SPELL;
                af.bitvector = 0;
                affect_to_char(ch, &af );

                ch->pcdata->bonus--;
                WAIT_STATE(ch, PULSE_PER_SECOND);
                return;
        }

        if ( !str_cmp( arg, "initiate" ) )
        {
                if ( ch->pcdata->bonus < 1 )
                {
                        send_to_char("You don't have any bonus actions available.\n\r", ch);
                        return;
                }

                if ( ch->level < 95 )
                {
                        send_to_char("You don't have access to that bonus action yet.\n\r", ch);
                        return;
                }

                AFFECT_DATA af;

                if (IS_AFFECTED(ch, AFF_BONUS_INITIATE))
                {
                        send_to_char("Your attacks are already doing extra damage.\n\r", ch);
                        return;
                }

                send_to_char("You feel the power of your opening attacks increase.\n\r", ch);
                act("$n looks more deadly.", ch, NULL, NULL, TO_ROOM);
                af.type         = gsn_bonus_initiate;
                af.duration     = ch->pcdata->max_bonus;
                af.location     = APPLY_NONE;
                af.modifier     = 0;
                af.bitvector    = AFF_BONUS_INITIATE;
                affect_join(ch, &af);
                ch->pcdata->bonus--;
                WAIT_STATE(ch, PULSE_PER_SECOND);
                return;
        }

        send_to_char("That's not a valid bonus action.\n\r", ch);
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
        int dex_mod;

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

        if ( ( ch->form != FORM_SNAKE )
        && ( ch->in_room->sector_type != SECT_UNDERWATER )
        && ( ch->in_room->sector_type != SECT_UNDERWATER_GROUND )
        && ( ch->in_room->sector_type != SECT_WATER_SWIM ) )
        {
                send_to_char("You need to be in a suitable body of water to swim.\n\r", ch);
                return;
        }

        if ( ch->in_room->sector_type == SECT_WATER_NOSWIM )
        {
                send_to_char("You can't swim in this water.\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
        ||   IS_AFFECTED(ch, AFF_LEG_TRAUMA)
        ||   IS_AFFECTED(ch, AFF_TAIL_TRAUMA)
        ||   IS_AFFECTED(ch, AFF_TORSO_TRAUMA) )
        {
            dex_mod = (get_curr_dex(ch) / 5);
        }
        else {
            dex_mod = (get_curr_dex(ch) / 10);
        }

        if ( number_percent() < ch->pcdata->learned[gsn_swim] )
        {
                af.type      = gsn_swim;
                af.duration  = -1;
                af.modifier  = 0;
                af.location  = APPLY_NONE;
                af.bitvector = AFF_SWIM;
                affect_to_char(ch, &af);

                af.modifier = dex_mod;
                af.location = APPLY_HITROLL;
                affect_to_char(ch, &af);

                send_to_char("You start swimming.\n\r", ch);
                return;
        }
        else {
                send_to_char("You thrash about in the water ineffectively.\n\r", ch);
                return;
        }

        send_to_char("You start swimming.\n\r", ch);
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

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't gouge an object.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to rip your own eyes out?\n\r", ch);
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
                act ("You deftly lunge at $N's eyes, and brutally gouge them!", ch, NULL, victim, TO_CHAR);
                act ("$n scratches your eyes -- you see <124>red<0>!", ch, NULL, victim, TO_VICT);
                act ("$n gouges $N's eyes!", ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n gouges $N's eyes.", ch, victim);

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
                send_to_char("You want to choke yourself?\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't choke an object.\n\r", ch);
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

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
            chance = ( ch->pcdata->learned[gsn_choke] / 2 );
        }

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
                act ("$n begins to choke you... you feel dizzy... you black out.",
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
                send_to_char ("Your lack of intellect and concentration prevents this...\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_HEAD_TRAUMA) )
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
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
                send_to_char("You focus your mental energies, but your concentration is lacking...\n\r", ch);

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
                send_to_char("You want to knock yourself out?\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        if ( number_percent() < ch->pcdata->learned[gsn_punch] )
        {
                arena_commentary("$n punches $N.", ch, victim);
                damage(ch, victim, (ch->level / 2) + number_range(1, ch->level * 2), gsn_punch, FALSE);

                if (victim->position ==POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (!IS_AFFECTED(ch, AFF_ARM_TRAUMA))
                {
                    if (number_percent() < ch->pcdata->learned[gsn_second_punch])
                            damage(ch, victim, ch->level + number_range(ch->level, ch->level * 2),gsn_second_punch, FALSE);
                }
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
        act("$n skilfully dismounts $N.", ch, NULL, ch->mount, TO_NOTVICT);
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

void strip_swallow (CHAR_DATA* ch)
{
        char buf[MAX_STRING_LENGTH];

        if (!ch->inside)
        {
                /* bug("strip_swallow: 'ch->inside' is null", 0); */
                return;
        }

        REMOVE_BIT(ch->affected_by, AFF_SWALLOWED);
        affect_strip(ch, gsn_swallow);

        sprintf(buf, "You broke out from inside of %s!\n\r",
                IS_NPC(ch->inside) ? ch->inside->short_descr : ch->inside->name);
        send_to_char (buf, ch);

        sprintf(buf, "$c broke out from inside of %s!",
                IS_NPC(ch->inside) ? ch->inside->short_descr : ch->inside->name);
        act (buf, ch, NULL, NULL, TO_ROOM);

        ch->inside = NULL;
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
int combo_tenketsu;
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
        combo_tenketsu          = 0;
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
        const int sn_tenketsu   = skill_lookup("tenketsu");

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

                else if (is_name(arg, "tenketsu"))
                        sn = sn_tenketsu;

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
                                if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_atemi] / 2 ) ) )
                                {
                                        damage(ch , victim, 0 , gsn_atemi, FALSE);
                                        combo_last_punch = 0;
                                }
                                else {
                                    atemi(ch , victim);
                                    combo_last_punch = 1;

                                    if (!combo_atemi)
                                    {
                                            combo_count++;
                                            combo_atemi = 1;
                                    }
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
                                if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_kansetsu] / 2 ) ) )
                                {
                                        act ("$n attempts to break $N's arms, but fails.", ch, NULL, victim,TO_NOTVICT);
                                        damage(ch , victim, 0 , gsn_kansetsu, FALSE);
                                }
                                else {
                                    kansetsu(ch, victim);
                                }
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
                                if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_tetsui] / 2 ) ) )
                                {
                                        damage(ch, victim,0, gsn_tetsui, FALSE);
                                        combo_last_punch = 0;
                                }
                                else {
                                        tetsui(ch, victim);
                                        combo_last_punch = 1;

                                        if (!combo_tetsui)
                                        {
                                                combo_tetsui = 1;
                                                combo_count++;
                                        }
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
                                if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_shuto] / 2 ) ) )
                                {
                                        damage(ch, victim,0, gsn_shuto, FALSE);
                                        combo_last_punch = 0;
                                }
                                else {
                                        shuto(ch, victim);
                                        combo_last_punch = 1;

                                        if (!combo_shuto)
                                        {
                                                combo_shuto = 1;
                                                combo_count++;
                                        }
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
                                if ( IS_AFFECTED(ch, AFF_LEG_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_yokogeri] / 2 ) ) )
                                {
                                        damage(ch, victim,0, gsn_yokogeri, FALSE);
                                        combo_last_kick = 0;
                                }
                                else {
                                        yokogeri(ch, victim);
                                        combo_last_kick = 1;

                                        if (!combo_yokogeri)
                                        {
                                                combo_yokogeri = 1;
                                                combo_count++;
                                        }
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
                                if ( IS_AFFECTED(ch, AFF_LEG_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_mawasigeri] / 2 ) ) )
                                {
                                        damage(ch, victim,0, gsn_mawasigeri, FALSE);
                                        combo_last_kick = 0;
                                }
                                else {
                                        mawasigeri(ch, victim);
                                        combo_last_kick = 1;

                                        if (!combo_mawasigeri)
                                        {
                                                combo_mawasigeri = 1;
                                                combo_count++;
                                        }
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

                if (sn == sn_tenketsu)
                {
                        if (number_percent() < ch->pcdata->learned[gsn_tenketsu])
                        {
                                if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
                                && ( number_percent() < ( ch->pcdata->learned[gsn_tenketsu] / 2 ) ) )
                                {
                                        damage(ch, victim,0, gsn_tenketsu, FALSE);
                                        combo_last_punch = 0;
                                }
                                else {
                                        tenketsu(ch, victim);
                                        combo_last_punch = 1;

                                        if (!combo_tenketsu)
                                        {
                                                combo_tenketsu = 1;
                                                combo_count++;
                                        }
                                }
                        }
                        else
                        {
                                damage(ch, victim, 0, gsn_tenketsu, FALSE);
                                combo_last_punch = 0;
                        }

                        combo_last_kick = 0;
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

void do_tenketsu (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_tenketsu))
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

        WAIT_STATE(ch, skill_table[gsn_tenketsu].beats);
        if (number_percent() < ch->pcdata->learned[gsn_tenketsu])
        {
                reset_combo_flags();
                tenketsu(ch, victim);
        }
        else
                damage(ch, victim,0, gsn_tenketsu, FALSE);
}


void atemi (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int dam = ch->level * 1.5;

        if (combo_last_punch)
                dam += dam / 4;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        if (IS_NPC(victim))
        {
                if (IS_SET(victim->act, ACT_OBJECT))
                {
                    arena_commentary("$n strikes $N in its centre mass.", ch, victim);
                }
                else {
                    arena_commentary("$n strikes $N in the torso.", ch, victim);
                }
        }
        else {
                arena_commentary("$n strikes $N in the torso.", ch, victim);
        }
        damage(ch, victim, dam, gsn_atemi, FALSE);
}


void kansetsu (CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *obj;

        if (!HAS_ARMS (victim))
        {
                act ("<105>Alas, $N has no wrists for you to break.<0>", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't break the wrists of an object.\n\r", ch);
                return;
        }


        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from disarming anyone here.<0>\n\r", ch);
                return;
        }

        obj = get_eq_char(victim, WEAR_WIELD);
        if (!obj)
                obj = get_eq_char(victim, WEAR_DUAL);

        if (!obj)
        {
                act ("<99>Alas, $N has no weapon.<0>", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_SET (obj->extra_flags, ITEM_BODY_PART))
        {
                send_to_char ("<111>You can't disarm your opponent's body parts!<0>\n\r", ch);
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

void tenketsu (CHAR_DATA *ch, CHAR_DATA *victim)
{
        AFFECT_DATA af;
        int dam = ch->level * 1.9;

        if (combo_last_punch)
                dam += dam / 5;

        if (combo_count > 2 )
                dam *= -1.5 + combo_count;

        arena_commentary("$n attacks $N with a pressure-point strike.", ch, victim);

        if ( !IS_AFFECTED( victim, AFF_HOLD ) )
        {
                af.type      = gsn_tenketsu;
                af.duration  = 1 + ch->level / 15;
                af.location  = APPLY_DEX;
                af.modifier  = -5;
                af.bitvector = AFF_HOLD;
                affect_join( victim, &af );

                af.location     = APPLY_MOVE;
                af.modifier     = ( ch->level * -10 );
                af.bitvector    = AFF_NO_RECALL;
                affect_to_char(victim, &af);

                if ( ch != victim )
                        send_to_char( "<201>Your pressure-point strike successfully paralyses your victim.<0>\n\r", ch );

                send_to_char( "<14>You cannot move, you are paralysed!<0>\n\r", victim );
        }

        damage(ch, victim, dam, gsn_tenketsu, FALSE);
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

        if ( IS_NPC(ch) )
                return;

        if ( !IS_NPC(ch)
        &&   !CAN_DO(ch, gsn_push) )
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
                send_to_char("Your target cannot be pushed into that area.\n\r", ch);
                victim->position = POS_STANDING;
                return;
        }

        if (is_safe(ch, victim))
                return;

        chance = 50;
        chance += (get_curr_str(ch) - 15) * 3;
        chance += ch->level - victim->level;
        race_bonus = 0;

        if ( !IS_NPC(ch) )
        {
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
                case 24: race_bonus =  -1; break;
                case 25: race_bonus =   2; break;
                default: break;
                }
        }

        chance += race_bonus;

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
        ||   IS_AFFECTED(ch, AFF_LEG_TRAUMA
        ||   IS_AFFECTED(ch, AFF_TORSO_TRAUMA)))
        {
                chance = ( chance / 2 );
        }

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

        send_to_char("<196>You feed on the fresh corpse!<0>\n\r", ch);
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

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
                return;

        if ( !IS_NPC(ch)
        &&   !CAN_DO(ch, gsn_suck) )
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

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't suck blood from an object.\n\r", ch);
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
                one_hit(ch, victim, gsn_suck, FALSE);
                ch->rage += 2;
                ch->rage = UMIN(ch->rage, ch->max_rage);

                if (ch->rage > (ch->max_rage * 3 / 4))
                        send_to_char("You feel the power of {Rfresh blood{x surge through your body!\n\r", ch);

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

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from disarming anyone here.<0>\n\r", ch);
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

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't break the wrists of an object.\n\r", ch);
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
                        send_to_char("<15>Your opponent is not wielding a weapon.<0>\n\r", ch);
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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
            if (number_percent() > (ch->pcdata->learned[gsn_sharpen]/2))
            {
                    send_to_char("You clumsily slip while sharpening your blade! You ruin the stone and cut yourself!!\n\r", ch);
                    damage(ch, ch, ch->level, gsn_sharpen, FALSE);
                    act ("$n cuts $mself while sharpening $m blade!", ch, NULL, NULL, TO_ROOM);
                    extract_obj(wobj);
                    return;
            }
        }
        else {
            if (number_percent() > ch->pcdata->learned[gsn_sharpen])
            {
                    send_to_char("You slip while sharpening your blade! You ruin the stone and cut yourself!!\n\r", ch);
                    damage(ch, ch, ch->level, gsn_sharpen, FALSE);
                    act ("$n cuts $mself while sharpening $m blade!", ch, NULL, NULL, TO_ROOM);
                    extract_obj(wobj);
                    return;
            }
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
                send_to_char("You need an armourer's hammer to forge your armour.\n\r", ch);
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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        if (!IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
            if (number_percent() > ch->pcdata->learned[gsn_forge])
            {
                    send_to_char("You slip while hammering your armour and pound yourself!\n\r", ch);
                    damage(ch, ch, ch->level, gsn_forge, FALSE);
                    act ("$n pounds $mself while forging $s armour!", ch, NULL, NULL, TO_ROOM);
                    return;
            }
        }
        else {
            if (number_percent() > (ch->pcdata->learned[gsn_forge]/2))
            {
                    send_to_char("You clumsily slip while hammering your armour and pound yourself!\n\r", ch);
                    damage(ch, ch, ch->level, gsn_forge, FALSE);
                    act ("$n pounds $mself while forging $s armour!", ch, NULL, NULL, TO_ROOM);
                    return;
            }
        }

        if (in_c_room)
                send_to_char("{CYour use of specialised crafting tools improves your forging!\n\r{x", ch);

        act ("You skilfully enhance $P with $p!", ch, wobj, obj, TO_CHAR);
        act ("$n skilfully enhances $P using $p!", ch, wobj, obj, TO_ROOM);

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

void do_strengthen (CHAR_DATA *ch, char *argument)
{
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *anvil;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool found;
        int cost_st;
        int cost_ti;
        int cost_ad;
        int cost_el;
        int cost_sm;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_strengthen))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What are you trying to strengthen?\n\r", ch);
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

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_STRENGTHEN))
        {
                send_to_char("That is already strengthened.\n\r", ch);
                return;
        }

        if (obj->wear_flags)
        {
                int next;
                 bit_explode(ch, buf, obj->wear_flags);

                for (next = 1; next <= BIT_17; next *= 2)
                {
                        if (IS_SET(obj->wear_flags, next))
                        {
                                if ( !str_cmp( wear_flag_name(next), "finger")
                                || !str_cmp( wear_flag_name(next), "float")
                                || !str_cmp( wear_flag_name(next), "hold" )
                                || !str_cmp( wear_flag_name(next), "neck")
                                || !str_cmp( wear_flag_name(next), "about" ) )
                                {
                                        send_to_char("You cannot strengthen that type of armour.\n\r", ch);
                                        return;
                                }
                        }
                }
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

        cost_sm =0;
        cost_ad = 0;
        cost_el = 0;
        cost_st = 0;
        cost_ti = 0;

        if ( (obj->level > 90) )
                cost_sm = 1;

        if (obj->level > 70 )
                cost_el = 10;

        if (obj->level > 50 )
                cost_ad = 15;
        if (obj->level > 25 )
                cost_ti = 20;
        if (obj->level > 15 )
                cost_st = 10;
        if (obj->level >= 1 )
                cost_st += 12;

        if ( cost_st > ch->smelted_steel
        ||  cost_ti > ch->smelted_titanium
        ||  cost_ad > ch->smelted_adamantite
        ||  cost_el > ch->smelted_electrum
        ||  cost_sm > ch->smelted_starmetal)
        {
                send_to_char( "You don't have enough raw materials, you need:\n\r", ch );
                sprintf(buf, "%d Steel %d Titanium %d Adamantite %d Electrum %d Starmetal",
                cost_st, cost_ti, cost_ad, cost_el, cost_sm);
                act(buf, ch, NULL, NULL, TO_CHAR);
                return;
        }


        if (number_percent() > ch->pcdata->learned[gsn_strengthen])
        {
                send_to_char("You slip while hammering your armour and pound yourself!\n\r", ch);
                damage(ch, ch, ch->level, gsn_forge, FALSE);
                act ("$n pounds $mself while forging $s armour!", ch, NULL, NULL, TO_ROOM);
                smelted_to_char( cost_st, cost_ti, cost_ad, cost_el, cost_sm, ch, COINS_REPLACE);
                return;
        }

        act ("You skilfully strengthen $P!", ch, NULL, obj, TO_CHAR);
        act ("$n skilfully strengthens $P!", ch, NULL, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_STRENGTHEN);
        smelted_to_char( cost_st, cost_ti, cost_ad, cost_el, cost_sm, ch, COINS_REPLACE);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_strengthen;
        paf->duration       = -1;
        paf->location       = APPLY_STRENGTHEN;
        paf->modifier       = 1 + ( ch->level / 50 );
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        set_obj_owner(obj, ch->name);
}


void do_reforge (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *anvil;
        char arg [ MAX_INPUT_LENGTH ];
        bool found;


        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_reforge))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What are you trying to reforge?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that on your person.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That item is not a weapon.\n\r", ch);
                return;
        }

        if (!IS_SET(obj->ego_flags, EGO_ITEM_CONSTRUCTED))
        {
                send_to_char("That is not something that is constructed.\n\r", ch);
                return;
        }

        if (!is_obj_owner (obj,ch))
        {
                send_to_char("That is not something you own.\n\r", ch);
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

        if (number_percent() > ch->pcdata->learned[gsn_strengthen])
        {
                send_to_char("You slip while reforging your weapon and pound yourself!\n\r", ch);
                damage(ch, ch, ch->level, gsn_forge, FALSE);
                act ("$n pounds $mself while forging $s armour!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        act ("You skilfully reforge $P!", ch, NULL, obj, TO_CHAR);
        act ("$n skilfully reforge $P!", ch, NULL, obj, TO_ROOM);

                obj->value[1]   = 2* (number_fuzzy( number_fuzzy( 1 * ch->level / 4 + 2 ) ));
                obj->value[2]   = 2* (number_fuzzy( number_fuzzy( 3 * ch->level / 4 + 6 ) ));
        set_obj_owner(obj, ch->name);
        obj->level = ch->level;
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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your head is too damaged to use breath attacks.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (ch->form == FORM_DRAGON && (arg[0] == '\0'))
        {
                send_to_char("Breathe fire, lightning, acid, steam, or gas?\n\r", ch);
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

                if ( !str_cmp( arg, "steam" ) )
                {
                        int sn;
                        if ( ( sn = skill_lookup( "steam breath" ) ) < 0 )
                                return;
                        arena_commentary("$n breathes steam on $N.", ch, victim);
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

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't extort an object.\n\r", extortionist);
                return;
        }

        store = victim->pIndexData->pShop;

        if( store
           || IS_SET( victim->pIndexData->act, ACT_BANKER)
           || IS_SET( victim->pIndexData->act, ACT_IS_HEALER)
           || ( victim->pIndexData->pGame->game_fun != 0 )
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
                           &&  ( guardian_mob->spec_fun == spec_lookup("spec_guard") ||  guardian_mob->spec_fun == spec_lookup("spec_sahuagin_guard") ) )
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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember the words!\n\r", ch);
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
                {
                        send_to_char( "You are already affected by a chant of protection.\n\r", ch );
                        return;
                }

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
                {
                        send_to_char( "You are already affected by a chant of battle.\n\r", ch );
                        return;
                }

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

        else if( is_name( arg, "dragonsbane" ) )
        {
                if( ch->fighting )
                {
                        send_to_char( "You can't chant that while you're fighting!\n\r", ch );
                        return;
                }

                act( "$n begins to chant loudly.", ch, NULL, NULL, TO_ROOM );

                if( number_percent() > ch->pcdata->learned[gsn_chant_of_dragonsbane] )
                {
                        send_to_char( "Your dragonsbane chant fails miserably.\n\r", ch );
                        return;
                }

                if( is_affected( ch, gsn_dragon_shield ) )
                {
                        send_to_char( "The chant of dragonsbane confers no benefit on you.\n\r", ch );
                        return;
                }

                send_to_char( "You complete your dragonsbane chant.\n\r", ch );

                af.type         = gsn_dragon_shield;
                af.duration     = 3 + ch->level/10;
                af.modifier     = 0;
                af.bitvector    = 0;
                af.location     = APPLY_NONE;
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
                {
                        send_to_char( "You are already affected by a chant of vigour.\n\r", ch );
                        return;
                }

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

                if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
                {
                        send_to_char("Your chant can't affect objects.\n\r", ch);
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

                if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
                {
                        send_to_char("Your chant can't affect objects.\n\r", ch);
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

/*
 *  checks if character has cursed item on them or is
 *  afflicted by curse, hex, etc.
 */
bool is_cursed(CHAR_DATA *ch)
{
        OBJ_DATA *pobj;
        OBJ_DATA *pobj_next;

        if ( is_affected( ch, gsn_curse )
        ||   is_affected( ch, gsn_hex )
        ||   is_affected( ch, gsn_prayer_weaken )
        ||   IS_AFFECTED( ch, AFF_CURSE ) )
        {
                return TRUE;
        }

        for (pobj = ch->carrying; pobj; pobj = pobj_next)
        {
                pobj_next = pobj->next_content;

                if (pobj->deleted)
                        continue;

                if ( IS_SET(pobj->extra_flags, ITEM_CURSED) )
                {
                        return TRUE;
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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember the words!\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
                send_to_char("Your arms are too damaged--you can't play your instrument.\n\r", ch);
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
                send_to_char( "You wouldn't know how to dowse for fresh water.\n\r", ch );
                return;
        }

        if( ch->in_room->sector_type != SECT_FOREST
           && ch->in_room->sector_type != SECT_FIELD
           && ch->in_room->sector_type != SECT_HILLS
           && ch->in_room->sector_type != SECT_SWAMP
           && ch->in_room->sector_type != SECT_MOUNTAIN )
        {
                send_to_char( "You wouldn't find a fresh water source in this terrain.\n\r", ch );
                return;
        }

        if( number_percent() > ch->pcdata->learned[gsn_dowse] )
        {
                send_to_char( "You're unable to locate a fresh water source.\n\r", ch );
                act( "$n searches for a fresh water source, but doesn't find one.", ch, NULL,
                        NULL, TO_ROOM );
        }
        else
        {
                spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0,"common", CREATED_NO_RANDOMISER );
                spring->timer    = ch->level;
                spring->timermax = spring->timer;
                obj_to_room( spring, ch->in_room );
                send_to_char( "You search for a fresh water source and find a small spring.\n\r", ch);
                act( "$n searches for a fresh water source, and locates a small spring.",
                    ch, NULL, NULL, TO_ROOM );
        }

        WAIT_STATE( ch, PULSE_VIOLENCE );
}

static bool is_negative_herb(int idx)
{
    const char *n = herb_table[idx].name;
    return !str_cmp(n, "slota")
        || !str_cmp(n, "zur")
        || !str_cmp(n, "dynallca")
        || !str_cmp(n, "karfar");
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
           && ch->in_room->sector_type != SECT_SWAMP
           && ch->in_room->sector_type != SECT_MOUNTAIN
           && ch->in_room->sector_type != SECT_UNDERWATER_GROUND )
        {
                send_to_char( "You wouldn't find any herbs in this terrain.\n\r", ch );
                return;
        }

        {
            int base_low   = 50;
            int base_high  = 75;
            int skill_pct  = UMAX(0, UMIN(100, ch->pcdata->learned[ gsn_gather_herbs ]));
            int level_steps = ch->level / 10;
            int reduction   = (level_steps * skill_pct) / 100;

            int low  = UMAX(1, base_low  - reduction);
            int high = UMAX(low, base_high - reduction);

            move_cost = number_range(low, high);
        }

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

        int base_pct = herb_table[herb_type].chance;         /* from table, e.g., 60 */
        int eff_pct  = base_pct;

        if (is_negative_herb(herb_type)) {

            int skill_pct  = UMAX(0, UMIN(100, ch->pcdata->learned[ gsn_gather_herbs ]));
            int level_pct  = UMAX(0, UMIN(100, ch->level));

            const int MAX_REDUCTION_PCT = 50;

            int reduction_from_level = (MAX_REDUCTION_PCT/2) * level_pct / 100;
            int reduction_from_skill = (MAX_REDUCTION_PCT/2) * skill_pct / 100;
            int total_reduction      = reduction_from_level + reduction_from_skill;

            eff_pct = base_pct * (100 - total_reduction) / 100;

            eff_pct = URANGE(1, eff_pct, 95);
        }

        i = eff_pct * 10;

        if ( number_range(0, 999) >= i )
        {
            if( random )
                send_to_char( "You are unable to find any useful herbs.\n\r", ch );
            else
            {
                sprintf( buf, "You search the area but cannot find any %s.\n\r",
                    herb_table[ herb_type ].name );
                send_to_char( buf, ch );
            }
            act( "$n searches for healing herbs, but comes up empty handed.", ch, NULL, NULL, TO_ROOM );
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
                herb = create_object( get_obj_index( ITEM_VNUM_HERB ), 0, "common", CREATED_NO_RANDOMISER );
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

/* Harvest and carve for druids and rangers, harvest similar to gather but
   for smokeables.  Carve allows you to make pipes for yourself or others.
--Owl 4/10/24 */

void do_harvest (CHAR_DATA *ch, char *arg)
{
        OBJ_DATA        *smokeable;
        int             smokeable_type;
        int             random_smokeable;
        int             move_cost;
        bool            random;
        int             i;
        char            buf[ MAX_STRING_LENGTH ];

        if( IS_NPC(ch) )
                return;

        if( !CAN_DO( ch, gsn_harvest ) )
        {
                send_to_char( "You don't know what's smokeable and what isn't.\n\r", ch );
                return;
        }

        if (ch->sub_class == SUB_CLASS_DRUID)
        {
                move_cost = number_range( 75, 100 );
        }
        else {
                move_cost = number_range( 150, 200 );
        }

        if( ch->move - move_cost < 1 )
        {
                send_to_char( "You're too tired to look for smokeables to harvest.\n\r", ch );
                return;
        }

        ch->move -= move_cost;
        WAIT_STATE( ch, PULSE_VIOLENCE );

        smokeable_type = -1;
        random_smokeable = -1;
        random = FALSE;

        if( arg[0] == '\0' )
        {
                random = TRUE;
                for( i=0; i < MAX_SMOKEABLES; i++ )
                {
                        random_smokeable = number_range( 0, MAX_SMOKEABLES-1 );
                        if( (ch->pcdata->learned[ gsn_harvest ] >= smokeable_table[ random_smokeable ].min_skill)
                           && ( ch->level >= smokeable_table[ random_smokeable ].min_level )
                           && ( ch->in_room->sector_type == smokeable_table[ random_smokeable ].sect_type ) )
                        {
                                smokeable_type = random_smokeable;
                                break;
                        }
                }
        }
        else for( i=0; i<MAX_SMOKEABLES; i++ )
        {
                if( is_full_name( arg, smokeable_table[i].name ) )
                {
                        if( ch->in_room->sector_type != smokeable_table[i].sect_type )
                        {
                            send_to_char( "You wouldn't find any of that in this sort of terrain.\n\r", ch );
                            return;
                        }

                        if( ch->pcdata->learned[ gsn_harvest ] < smokeable_table[i].min_skill )
                        {
                            send_to_char( "You don't know enough about how to find it.\n\r", ch );
                            return;
                        }

                        if( ch->level < smokeable_table[i].min_level)
                        {
                            send_to_char( "You aren't experienced enough to know how to do that.\n\r", ch );
                            return;
                        }

                        smokeable_type = i;
                        break;
                }
        }

        if( smokeable_type < 0 )
        {
                if( random )
                {
                        send_to_char( "You are unable to find any smokeable substances here.\n\r", ch );
                }
                else {
                        send_to_char( "You search intensely but cannot find that smokeable substance.\n\r", ch );
                }

                act( "$n searches for smokeable substances, but comes up empty-handed.", ch, NULL,NULL, TO_ROOM );

                return;
        }

        i = smokeable_table[ smokeable_type ].chance * 10;

        if ( ch->sub_class == SUB_CLASS_DRUID )
        {
            i = i + ( ( 999 - i ) / 8 );
        }

        if( number_range( 0, 999 ) >= i )
        {
                if( random )
                        send_to_char( "You are unable to find any smokeable substances here.\n\r", ch );
                else
                {
                        sprintf( buf, "You search the area intensely, but cannot find any %s.\n\r",
                                smokeable_table[ smokeable_type ].name );
                        send_to_char( buf, ch );
                }
                act( "$n searches for smokeable substances to harvest, but comes up empty handed.", ch, NULL,
                    NULL, TO_ROOM );
                return;
        }

        send_to_char( smokeable_table[ smokeable_type ].action, ch );

        sprintf( buf, "$n searches the area for smokeable substances and finds some %s.",
                smokeable_table[ smokeable_type ].name );
        act( buf, ch, NULL, NULL, TO_ROOM );

        smokeable = create_object( get_obj_index( ITEM_VNUM_SMOKEABLE ), 0, "common", CREATED_NO_RANDOMISER );
        free_string(smokeable->name);
        free_string(smokeable->short_descr);
        free_string(smokeable->description);
        smokeable->name              = str_dup( smokeable_table[ smokeable_type ].keywords );
        smokeable->short_descr       = str_dup( smokeable_table[ smokeable_type ].short_desc );
        smokeable->description       = str_dup( smokeable_table[ smokeable_type ].long_desc );
        smokeable->item_type         = ITEM_SMOKEABLE;
        smokeable->level             = ch->level;
        smokeable->value[0]          = ch->sub_class == SUB_CLASS_DRUID ? ( number_fuzzy( smokeable_table[ smokeable_type ].uses ) + ( number_range (0, 7 ) ) ) : number_fuzzy( smokeable_table[ smokeable_type ].uses );
        smokeable->value[1]          = skill_lookup( smokeable_table[ smokeable_type ].spell1 );
        smokeable->value[2]          = skill_lookup( smokeable_table[ smokeable_type ].spell2 );
        smokeable->value[3]          = skill_lookup( smokeable_table[ smokeable_type ].spell3 );

        if( ch->carry_number >= can_carry_n( ch ) )
                obj_to_room( smokeable, ch->in_room );
        else obj_to_char( smokeable, ch );

        return;
}

void do_carve (CHAR_DATA *ch, char *arg)
{
        OBJ_DATA        *pipe;
        OBJ_DATA        *obj;
        int             pipe_type;
        int             random_pipe;
        bool            random;
        int             i;
        int             move_cost;
        int             created_level;
        int             load_level;
        char            arg1 [ MAX_INPUT_LENGTH ];
        char            arg2 [ MAX_INPUT_LENGTH ];
        char            buf [ MAX_STRING_LENGTH ];

        created_level = 0;

        if( IS_NPC(ch) )
                return;

        if( !CAN_DO( ch, gsn_carve ) )
        {
                send_to_char( "You don't know how to do that.\n\r", ch );
                return;
        }

        smash_tilde( arg );
        arg = one_argument( arg, arg1 );
        arg = one_argument( arg, arg2 );

        if ( ( arg2[0] != '\0' ) && !is_number( arg2 ) )
        {
                send_to_char( "Huh?\n\r", ch );
                return;
        }

        if (!(obj = get_eq_char(ch, WEAR_WIELD)) || !is_carving_weapon(obj))
        {
                send_to_char("You need to wield an appropriate blade to carve.\n\r", ch);
                return;
        }

        if (ch->sub_class == SUB_CLASS_DRUID)
        {
                move_cost = number_range( 50, 75 );
        }
        else {
                move_cost = number_range( 100, 150 );
        }

        if( ch->move - move_cost < 1 )
        {
                send_to_char( "You're too tired to look for pipe construction materials.\n\r", ch );
                return;
        }

        ch->move -= move_cost;
        WAIT_STATE( ch, PULSE_VIOLENCE );

        pipe_type = -1;
        random_pipe = -1;
        random = FALSE;

        if( arg1[0] == '\0' )
        {
                random = TRUE;
                for( i=0; i < 5; i++ )
                {
                        random_pipe = number_range( 0, MAX_PIPES-1 );
                        if( ( ch->pcdata->learned[ gsn_carve ] >= pipe_table[ random_pipe ].min_skill )
                        &&  ( ch->level >= pipe_table[ random_pipe ].min_level )
                        &&  ( ch->in_room->sector_type == pipe_table[ random_pipe ].sect_type ) )
                        {
                                pipe_type = random_pipe;
                                break;
                        }
                }
        }
        else for( i=0; i < MAX_PIPES; i++ )
        {

                if( is_full_name( arg1, pipe_table[i].name ) )
                {
                        if( ch->in_room->sector_type != pipe_table[i].sect_type )
                        {
                            send_to_char( "You won't find materials for that pipe in this terrain.\n\r", ch );
                            return;
                        }

                        if( ch->pcdata->learned[ gsn_carve ] < pipe_table[i].min_skill )
                        {
                            send_to_char( "You don't know enough about how to find it.\n\r", ch );
                            return;
                        }

                        if( ch->level < pipe_table[i].min_level)
                        {
                            send_to_char( "You aren't experienced enough to know how to do that.\n\r", ch );
                            return;
                        }

                        if (arg2[0] != '\0')
                        {
                                created_level = atoi( arg2 );

                                if ( created_level > ch->level )
                                {
                                        send_to_char( "You aren't skilled enough to make a pipe of that quality.\n\r", ch );
                                        return;
                                }
                        }

                        pipe_type = i;
                        break;
                }
        }

        if( pipe_type < 0 )
        {
                if( random )
                        send_to_char( "You are unable to find any pipe construction materials.\n\r", ch );
                else
                        send_to_char( "You search the area but cannot find the materials you need.\n\r", ch );
                act( "$n searches for pipe construction materials, but comes up empty-handed.", ch, NULL,
                        NULL, TO_ROOM );
                return;
        }

        i = pipe_table[ pipe_type ].chance * 10;

        if ( ch->sub_class == SUB_CLASS_DRUID )
        {
            i = i + ( ( 999 - i ) / 8 );
        }

        if( number_range( 0, 999 ) >= i )
        {
                if( random )
                        send_to_char( "You are unable to find any pipe construction materials.\n\r", ch );
                else
                {
                        sprintf( buf, "You search the area but can't find what you need to make %s.\n\r",
                                pipe_table[ pipe_type ].short_desc );
                        send_to_char( buf, ch );
                }
                act( "$n searches for pipe construction materials, but comes up empty-handed.", ch, NULL,
                    NULL, TO_ROOM );
                return;
        }

        send_to_char( pipe_table[ pipe_type ].action, ch );

        sprintf( buf, "$n finds appropriate construction materials and carves %s.",
                pipe_table[ pipe_type ].short_desc );
        act( buf, ch, NULL, NULL, TO_ROOM );

        load_level = (random) ? ch->level : ( created_level ? created_level : ch->level);

        pipe = create_object( get_obj_index( ITEM_VNUM_PIPE ), 0, "common", CREATED_NO_RANDOMISER );
        free_string(pipe->name);
        free_string(pipe->short_descr);
        free_string(pipe->description);
        pipe->name              = str_dup( pipe_table[ pipe_type ].keywords );
        pipe->short_descr       = str_dup( pipe_table[ pipe_type ].short_desc );
        pipe->description       = str_dup( pipe_table[ pipe_type ].long_desc );
        pipe->item_type         = ITEM_PIPE;
        pipe->level             = load_level;
        pipe->value[0]          = scale_pipe( pipe_table[ pipe_type ].min_level, load_level, pipe_table[ pipe_type ].c_benefit, FALSE );
        pipe->value[1]          = scale_pipe( pipe_table[ pipe_type ].min_level, load_level, pipe_table[ pipe_type ].m_benefit, FALSE );
        pipe->value[2]          = scale_pipe( pipe_table[ pipe_type ].min_level, load_level, pipe_table[ pipe_type ].thirst_cost, TRUE );
        pipe->value[3]          = scale_pipe( pipe_table[ pipe_type ].min_level, load_level, pipe_table[ pipe_type ].speed, TRUE );

        set_obj_owner(pipe, ch->name);

        if( ch->carry_number >= can_carry_n( ch ) )
                obj_to_room( pipe, ch->in_room );
        else obj_to_char( pipe, ch );

        return;
}


/* New Smelt Code - Brutus */


void do_smelt (CHAR_DATA *ch, char *argument)
{

        char            arg[MAX_INPUT_LENGTH];
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA        *obj;
        OBJ_DATA        *forge;
        int             starmetal=0;
        int             electrum=0;
        int             adamantite=0;
        int             titanium=0;
        int             steel=0;
        bool found;
        int next;

        argument = one_argument(argument, arg);
        if (IS_NPC(ch))
        return;

        if (!CAN_DO(ch, gsn_smelt))
        {
                send_to_char("You don't know how to smelt anything.\n\r", ch);
                return;
        }

        for (forge = ch->in_room->contents; forge; forge = forge->next_content)
        {
                if (forge->item_type == ITEM_FORGE)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no forge here!\n\r", ch);
                return;
        }

         if (arg[0] == '\0')
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
                send_to_char("You can't see anything.\n\r", ch);
                return;
        }


        if (!str_cmp(arg, ch->name))
        {
                send_to_char("You're unlikely to contain any elements of value.\n\r", ch);
                act("$n considers smelting $mself, but determines they hold no value.",
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

        if ((obj->item_type == ITEM_WEAPON) && !is_bladed_weapon(obj))
        {
                send_to_char("You cant smelt that type of weapon. Find something with a blade.\n\r", ch);
                return;
        }

        if (obj->wear_flags)
        {
                bit_explode(ch, buf, obj->wear_flags);
                for (next = 1; next <= BIT_17; next *= 2)
                {
                        if (IS_SET(obj->wear_flags, next))
                        {
                               if ( !str_cmp( wear_flag_name(next), "float")
                               || !str_cmp( wear_flag_name(next), "neck")
                               || !str_cmp( wear_flag_name(next), "about")
                               || !str_cmp( wear_flag_name(next), "hold") )
                                {
                                        send_to_char("You cant smelt that type of armour.\n\r", ch);
                                        return;
                                }
                        }
                }
        }

        if (obj->level > ch->level)
        {
                send_to_char("You don't know how to smelt that yet.\n\r", ch);
                return;
        }

        if (number_percent() <= ch->pcdata->learned[gsn_smelt])
        {
                if (number_percent() >= 98)
                        starmetal = (obj->level/60);
                if (number_percent() >= 94)
                        electrum = (obj->level/40);
                if (number_percent() >= 75)
                        adamantite = (obj->level/24);
                if (number_percent() >= 50)
                        titanium = (obj->level/10);
                if (number_percent() >= 1)
                        steel = (1 + obj->level/6);

                /* If a magic item rare and over double the resources obtained*/
                if (calc_item_score (obj)> ITEM_SCORE_UNCOMMON)
                        titanium *=2;
                if (calc_item_score (obj)> ITEM_SCORE_RARE)
                        adamantite *=2;
                if (calc_item_score (obj)> ITEM_SCORE_EPIC)
                        electrum *= 2;
                if (calc_item_score (obj)> ITEM_SCORE_LEGENDARY)
                        starmetal *= 2;

                act("$n smelts $p into its raw materials.", ch, obj, NULL, TO_ROOM);
                act("{WYou place $p into the forge.{x", ch, obj, NULL, TO_CHAR);
                smelted_to_char( steel, titanium, adamantite, electrum, starmetal, ch, COINS_ADD);
                sprintf(buf, "You recover the following raw materials:\n");
                send_to_char (buf, ch);
                if (steel > 0)
                {
                        steel = number_fuzzy (steel);
                         sprintf(buf,"    {wSteel {W[{C%d{W]{x\n",steel);
                        send_to_char (buf, ch);
                }
                if (titanium > 0)
                {
                        titanium = number_fuzzy (titanium);
                        sprintf(buf,"    {yTitanium {W[{C%d{W]{x\n", titanium);
                        send_to_char (buf, ch);
                }
                if (adamantite > 0)
                {
                        adamantite = number_fuzzy (adamantite);
                        sprintf(buf,"    {YAdamantite {W[{C%d{W]{x\n", adamantite);
                        send_to_char (buf, ch);
                }
                if (electrum > 0)
                {
                        electrum = number_fuzzy (electrum);
                        sprintf(buf,"    {WElectrum {W[{C%d{W]{x\n", electrum);
                        send_to_char (buf, ch);
                }
                if (starmetal > 0)
                {
                        starmetal = number_fuzzy (starmetal);
                        sprintf(buf,"    {RStarmetal {W[{C%d{W]{x\n", starmetal);
                        send_to_char (buf, ch);
                }
                ch->smelted_steel += steel;
                ch->smelted_titanium += titanium;
                ch->smelted_adamantite += adamantite;
                ch->smelted_electrum += electrum;
                ch->smelted_starmetal += starmetal;
                extract_obj(obj);
                return;
        }
        else
        {
                send_to_char("Your skill in smelting lets you down. You fail to recover anything.\n\r", ch);
                act("You watch as $p slowly dissolves into the belly of the forge.",ch, obj, NULL, TO_CHAR);
                act("$n fails to recover any materials from $p.",ch, obj, NULL, TO_ROOM);
                extract_obj(obj);
                return;
        }
}

/* Construct Code - Brutus Jul 2023 */
void do_construct( CHAR_DATA *ch, char *arg )
{
        OBJ_DATA        *creation;
        OBJ_DATA        *anvil;
        int             found, i;
        char            buf[MAX_STRING_LENGTH];
        const char* bar = "_____________________________________________________________________________\n\r\n\r";
        char            arg1[MAX_INPUT_LENGTH];
        char            arg2[MAX_INPUT_LENGTH];


        arg = one_argument(arg, arg1);
        arg = one_argument(arg, arg2);

        if (IS_NPC(ch))
                return;

        if (ch->class != CLASS_SMITHY)
        {
                send_to_char("You are unable to construct anything. Try asking a smithy.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You can't construct while fighting.\n\r", ch);
                return;
        }

        if ( ( ch->position < POS_STANDING ) &&
             ( arg1[0] != '\0' ) )
        {
            send_to_char("You're in no position to construct that right now.\n\r", ch);
            return;
        }

        if( arg1[0] == '\0' )
        {
                send_to_char( "            {WBlueprints{x  {GLearned{x       {CDamage{x         {YMetal Cost{x\n\r", ch);
                send_to_char(bar, ch);

                for (i = 0; i < BLUEPRINTS_MAX; i++)
                {

                        if ((ch->pcdata->learned[skill_lookup(blueprint_list[i].skill_name)]  > 0) && (blueprint_list[i].blueprint_cost[0] > 0))
                        {
                                sprintf(buf, "{W%22s{x {G%7d{x%% {c%6d{x {C%6d{x {w%6d{x {y%3d{x {Y%3d{x {W%3d{x {R%3d{x\n\r",
                                blueprint_list[i].blueprint_name,
                                ch->pcdata->learned[skill_lookup(blueprint_list[i].skill_name)],
                                blueprint_list[i].blueprint_damage[0],
                                blueprint_list[i].blueprint_damage[1],
                                blueprint_list[i].blueprint_cost[0],
                                blueprint_list[i].blueprint_cost[1],
                                blueprint_list[i].blueprint_cost[2],
                                blueprint_list[i].blueprint_cost[3],
                                blueprint_list[i].blueprint_cost[4]
                                );
                                send_to_char(buf, ch);
                        }

                }
                return;

        }

        found = -1;
        for (i = 0; i < BLUEPRINTS_MAX; i++)
        {
                if (!strcmp( arg1, blueprint_list[i].blueprint_name))
                {
                        found = i;
                        break;
                }
        }

        if (found == -1)
        {
                send_to_char( "Unknown Blueprint\n\rTry wrapping your blueprint in single quotes if it's more than one word.\n\r", ch);
                send_to_char( "            Blueprints         Learned      Damage\n\r", ch);
                send_to_char(bar, ch);

                for (i = 0; i < BLUEPRINTS_MAX; i++)
                {
                        if ((ch->pcdata->learned[skill_lookup(blueprint_list[i].skill_name)]  > 0) && (blueprint_list[i].blueprint_cost[0] > 0))
                        {
                                 sprintf(buf, "{W%22s{x {G%7d{x%% {c%6d{x {C%6d{x {w%6d{x {y%3d{x {Y%3d{x {W%3d{x {R%3d{x\n\r",
                                blueprint_list[i].blueprint_name,
                                ch->pcdata->learned[skill_lookup(blueprint_list[i].skill_name)],
                                blueprint_list[i].blueprint_damage[0],
                                blueprint_list[i].blueprint_damage[1],
                                blueprint_list[i].blueprint_cost[0],
                                blueprint_list[i].blueprint_cost[1],
                                blueprint_list[i].blueprint_cost[2],
                                blueprint_list[i].blueprint_cost[3],
                                blueprint_list[i].blueprint_cost[4]
                                );
                                send_to_char(buf, ch);
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
                ||  blueprint_list[i].blueprint_cost[1] > ch->smelted_titanium
                ||  blueprint_list[i].blueprint_cost[2] > ch->smelted_adamantite
                ||  blueprint_list[i].blueprint_cost[3] > ch->smelted_electrum
                ||  blueprint_list[i].blueprint_cost[4] > ch->smelted_starmetal)
        {
                send_to_char( "You don't have enough raw materials. You need:\n\r", ch );
                sprintf(buf, "%d Steel %d Titanium %d Adamantite %d Electrum %d Starmetal",
                blueprint_list[i].blueprint_cost[0],
                blueprint_list[i].blueprint_cost[1],
                blueprint_list[i].blueprint_cost[2],
                blueprint_list[i].blueprint_cost[3],
                blueprint_list[i].blueprint_cost[4]);

                act(buf, ch, NULL, NULL, TO_CHAR);
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
                send_to_char("You will need to find an anvil.\n\r", ch);
                return;
        }

        /* go through a loop if you're adding a chain */
        if  ( !strcmp( blueprint_list[i].blueprint_name, "weaponchain")
                ||  !strcmp(  blueprint_list[i].blueprint_name, "shieldchain") )
        {
                OBJ_DATA *obj;
                int eff_class;
                eff_class = ch->class;

                if( arg2[0] == '\0' )
                {
                        send_to_char("What object would you like to add the chain to?\n\r", ch);
                        return;
                }

                if (!(obj = get_obj_carry(ch, arg2)))
                {
                        send_to_char("You do not have that in your inventory.\n\r", ch);
                        return;
                }

                if ((!strcmp(blueprint_list[i].blueprint_name, "weaponchain") && (obj->item_type != ITEM_WEAPON))
                ||  (!strcmp(blueprint_list[i].blueprint_name, "shieldchain") && ( !CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_SHIELD, BIT_WEAR_SHIELD)))  )
                {
                        send_to_char("You cannot add a chain to that.\n\r", ch);
                        return;
                }

                if ((!strcmp(blueprint_list[i].blueprint_name, "weaponchain") && number_percent() > ch->pcdata->learned[gsn_weaponchain]) )
                {
                        send_to_char("You heat the forge and ready your materials.\n\r", ch);
                        send_to_char("You fumble... some of your materials slip into the forge and are lost to the flames.\n\r", ch);
                        act ("$n pounds $mself while creating $s armour!", ch, NULL, NULL, TO_ROOM);
                        smelted_to_char_loss(blueprint_list[i].blueprint_cost, ch, COINS_REPLACE, ch->pcdata->learned[gsn_weaponchain]);
                        return;
                }
                if ((!strcmp(blueprint_list[i].blueprint_name, "shieldchain") && number_percent() > ch->pcdata->learned[gsn_shieldchain]) )
                {
                        send_to_char("You heat the forge and ready your materials.\n\r", ch);
                        send_to_char("You fumble... some of your materials slip into the forge and are lost to the flames.\n\r", ch);
                        act ("$n pounds $mself while creating $s armour!", ch, NULL, NULL, TO_ROOM);
                        smelted_to_char_loss(blueprint_list[i].blueprint_cost, ch, COINS_REPLACE, ch->pcdata->learned[gsn_shieldchain]);
                        return;
                }


                if (blueprint_list[i].blueprint_ego)
                {
                        SET_BIT(obj->extra_flags, ITEM_EGO);
                        SET_BIT(obj->ego_flags, blueprint_list[i].blueprint_ego);
                        send_to_char( "You heat the forge and ready your materials.\n\r", ch );
                        sprintf(buf, "Expertly you attach your {W%s{x to {W%s{x.", blueprint_list[i].blueprint_name, obj->name);
                        act(buf, ch, NULL, NULL, TO_CHAR);

                        sprintf(buf, "$n constructs {W%s{x.", blueprint_list[i].blueprint_desc);
                        act(buf, ch, NULL, NULL, TO_ROOM);
                        smelted_to_char( blueprint_list[i].blueprint_cost[0],
                                blueprint_list[i].blueprint_cost[1],
                                blueprint_list[i].blueprint_cost[2],
                                blueprint_list[i].blueprint_cost[3],
                                blueprint_list[i].blueprint_cost[4], ch, COINS_REPLACE);
                }
                set_obj_owner(obj, ch->name);
                return;
        }

        if ( number_percent() > ch->pcdata->learned[skill_lookup(blueprint_list[i].blueprint_name)] )
        {
                send_to_char("You heat the forge and ready your materials.\n\r", ch);
                send_to_char("You fumble... some of your materials slip into the forge and are lost to the flames.\n\r", ch);
                act ("$n pounds $mself while creating $s armour!", ch, NULL, NULL, TO_ROOM);
                smelted_to_char_loss(blueprint_list[i].blueprint_cost, ch, COINS_REPLACE, ch->pcdata->learned[skill_lookup(blueprint_list[i].skill_name)]);
                return;
        }

        creation = create_object( get_obj_index( blueprint_list[i].blueprint_ref ), ch->level, "common", CREATED_NO_RANDOMISER );
        obj_to_char( creation, ch );
        if (blueprint_list[i].blueprint_ego >= 1 )
        {
                SET_BIT(creation->extra_flags, ITEM_EGO);
                SET_BIT(creation->ego_flags, blueprint_list[i].blueprint_ego);
        }
        set_obj_owner(creation, ch->name);
        creation->how_created    = CREATED_SKILL;

        send_to_char( "You heat the forge and ready your materials.\n\r", ch );
        sprintf(buf, "Expertly you assemble your components to create {W%s{x.", blueprint_list[i].blueprint_desc);
        act(buf, ch, NULL, NULL, TO_CHAR);

        sprintf(buf, "$n constructs {W%s{x.", blueprint_list[i].blueprint_desc);
        act(buf, ch, NULL, NULL, TO_ROOM);
        smelted_to_char( blueprint_list[i].blueprint_cost[0],
        blueprint_list[i].blueprint_cost[1],
        blueprint_list[i].blueprint_cost[2],
        blueprint_list[i].blueprint_cost[3],
        blueprint_list[i].blueprint_cost[4], ch, COINS_REPLACE);

        return;

}

void do_empower (CHAR_DATA *ch, char *argument)
{

        char            arg [ MAX_INPUT_LENGTH ];
        OBJ_DATA        *obj;
        OBJ_DATA        *forge;
        bool found;
        argument = one_argument(argument, arg);

        if( !CAN_DO( ch, gsn_empower ) )
        {
                send_to_char( "You don't know how to do that.\n\r", ch );
                return;
        }

        for (forge = ch->in_room->contents; forge; forge = forge->next_content)
        {
                if (forge->item_type == ITEM_FORGE)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no forge here!\n\r", ch);
                return;
        }


         if (arg[0] == '\0')
        {
                send_to_char("Empower what?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("Not while fighting.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
        {
                send_to_char("You can't see anything.\n\r", ch);
                return;
        }

        if (!str_cmp(arg, ch->name))
        {
                send_to_char("You're going to hurt yourself someday.\n\r", ch);
                act("$n considers empowering themselves.",
                    ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That item is not a weapon.\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_EMPOWERED))
        {
                send_to_char("That is already empowered.\n\r", ch);
                return;
        }

        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_EMPOWERED);
        set_obj_owner(obj, ch->name);

        act ("You empower $p!", ch, obj, NULL, TO_CHAR);
        act ( "{WNOTE: $p has a power meter! The meter has been added to the prompt (see help PROMPT for %G)", ch, obj, NULL, TO_CHAR);
        act ("$n empowers $p!", ch, obj, NULL, TO_ROOM);
        ch->prompt = str_dup("<<{G%h/%H{x hits {C%m/%M{x mana {Y%v/%V{x move [{W%z{x] %G> ");
}

void do_discharge (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        OBJ_DATA *obj;
        char       arg [ MAX_INPUT_LENGTH ];

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_discharge))
        {
                send_to_char("You do not know that.\n\r", ch);
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
                send_to_char("You are going to hurt yourself one day.\n\r", ch);
                return;
        }

        if (!(obj = get_eq_char(ch, WEAR_WIELD)))
        {
                if (!IS_SET(obj->ego_flags, EGO_ITEM_EMPOWERED))

                send_to_char("You will need to empower that weapon first.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (ch->pcdata->meter < 100)
        {
                send_to_char("Not at full charge yet.\n\r", ch);
                return;
        }

        if (number_percent() < ch->pcdata->learned[gsn_discharge])
        {
                arena_commentary("<531>$n's weapon unleashes a blinding light against $N.<0>", ch, victim);
                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                damage(ch, victim, number_range( ch->pcdata->meter * ch->level/3, ch->pcdata->meter * ch->level/2), gsn_discharge, FALSE);
                ch->pcdata->meter = 0;
                if (victim->position ==POS_DEAD || ch->in_room != victim->in_room)
                        return;
        }
        else
        {
                damage(ch, victim, 0, gsn_discharge, FALSE);
                ch->pcdata->meter = 0;
        }
        return;
}


void do_engrave (CHAR_DATA *ch, char *argument)
{
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *anvil;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool found;
        int cost_st;
        int cost_ti;
        int cost_ad;
        int cost_el;
        int cost_sm;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_engrave))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What are you trying to engrave?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that.\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_ENGRAVED))
        {
                send_to_char("That is already engraved.\n\r", ch);
                return;
        }

        if (obj->wear_flags)
        {
                int next;
                 bit_explode(ch, buf, obj->wear_flags);

                for (next = 1; next <= BIT_17; next *= 2)
                {
                        if (IS_SET(obj->wear_flags, next))
                        {
                                if (
                                !str_cmp( wear_flag_name(next), "float")
                                || !str_cmp( wear_flag_name(next), "neck")
                                || !str_cmp( wear_flag_name(next), "about" ) )
                                {
                                        send_to_char("You cannot engrave that type of armour.\n\r", ch);
                                        return;
                                }
                        }
                }
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

        cost_sm =0;
        cost_ad = 0;
        cost_el = 0;
        cost_st = 0;
        cost_ti = 0;

        if ( (obj->level > 90) )
                cost_sm = 1;

        if (obj->level > 70 )
                cost_el = 10;

        if (obj->level > 50 )
                cost_ad = 15;
        if (obj->level > 25 )
                cost_ti = 20;
        if (obj->level > 15 )
                cost_st = 10;
        if (obj->level >= 1 )
                cost_st += 12;

        if ( cost_st > ch->smelted_steel
        ||  cost_ti > ch->smelted_titanium
        ||  cost_ad > ch->smelted_adamantite
        ||  cost_el > ch->smelted_electrum
        ||  cost_sm > ch->smelted_starmetal)
        {
                send_to_char( "You don't have enough raw materials. You need:\n\r", ch );
                sprintf(buf, "%d Steel %d Titanium %d Adamantite %d Electrum %d Starmetal",
                cost_st, cost_ti, cost_ad, cost_el, cost_sm);
                act(buf, ch, NULL, NULL, TO_CHAR);
                return;
        }

        if (number_percent() > ch->pcdata->learned[gsn_engrave])
        {
                send_to_char("You slip while enrgaving!\n\r", ch);
                act ("$n slips while engraving $s armour!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        act ("$P glows softly as the runes form in the forge.", ch, NULL, obj, TO_CHAR);
        act ("$n skilfully engraves $P!", ch, NULL, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_ENGRAVED);
        set_obj_owner(obj, ch->name);
        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_engrave;
        paf->duration       = -1;
        paf->location       = APPLY_ENGRAVED;
        paf->modifier       = 1 + ( ch->level / 15 );
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

}

void do_inscribe (CHAR_DATA *ch, char *argument)
{
        char            buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *anvil;
        char            arg[MAX_INPUT_LENGTH];

        AFFECT_DATA *paf;
        bool found_rune;
        bool found_obj;
        bool found_anvil;
        int cost_st;
        int cost_ti;
        int cost_ad;
        int cost_el;
        int cost_sm;
        int i;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

  /*      if (ch->sub_class != SUB_CLASS_RUNESMITH)
        {
                send_to_char( "You'd better find a Runesmith.\n\r", ch );
                return;
        }
*/
        if (arg[0] == '\0')
        {
                send_to_char("Either inscribe equipment/weapon or inscribe your rune.\n\r", ch);
                return;
        }


/* --------------------------------- */
        found_rune = FALSE;
        for (i = 0; i < BLUEPRINTS_MAX; i++)
        {
                if (!strcmp( arg, blueprint_list[i].blueprint_name))
                {
                        found_rune = TRUE;
                        break;
                }
        }


        found_obj = FALSE;
        if ((obj = get_obj_carry(ch, arg)))
        {
                found_obj = TRUE;
        }



        if (!found_rune && (!found_obj))
        {
                send_to_char( "Cannot find the object or rune to inscribe within the room.\n\r", ch);
                return;
        }

/*        if( !ch->pcdata->learned[skill_lookup(blueprint_list[i].blueprint_name)] )
        {
                send_to_char( "You don't know how to construct that.\n\r", ch );
                return;
        }
*/
        /* prefer finding runes over objects */
        if (found_rune)
        {
                OBJ_DATA        *creation;
                creation = create_object( get_obj_index( blueprint_list[i].blueprint_ref ), ch->level, "common", CREATED_NO_RANDOMISER );
                obj_to_room( creation, ch->in_room );
                if (blueprint_list[i].blueprint_ego >= 1 )
                {
                        SET_BIT(creation->extra_flags, ITEM_EGO);
                        SET_BIT(creation->ego_flags, blueprint_list[i].blueprint_ego);
                }
                set_obj_owner(creation, ch->name);

                sprintf(buf, "You inscribe {W%s{x into the ground.", blueprint_list[i].blueprint_desc);
                act(buf, ch, NULL, NULL, TO_CHAR);

                sprintf(buf, "$n inscribes {W%s{x into the ground.", blueprint_list[i].blueprint_desc);
                act(buf, ch, NULL, NULL, TO_ROOM);
                return;
        }
 /*        ---------------------------  */
        if (found_obj)
        {
                if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_INSCRIBED))
                {
                        send_to_char("That is already inscribed.\n\r", ch);
                        return;
                }

                if (obj->wear_flags)
                {
                        int next;
                        bit_explode(ch, buf, obj->wear_flags);

                        for (next = 1; next <= BIT_17; next *= 2)
                        {
                                if (IS_SET(obj->wear_flags, next))
                                {
                                        if (
                                        !str_cmp( wear_flag_name(next), "float")
                                        || !str_cmp( wear_flag_name(next), "neck")
                                        || !str_cmp( wear_flag_name(next), "about" ) )
                                        {
                                                send_to_char("You cannot inscribe that type of armour.\n\r", ch);
                                                return;
                                        }
                                }
                        }
                }

                found_anvil = FALSE;
                for (anvil = ch->in_room->contents; anvil; anvil = anvil->next_content)
                {
                        if (anvil->item_type == ITEM_ANVIL)
                        {
                                found_anvil = TRUE;
                                break;
                        }
                }

                if ((!found_anvil) && (found_obj))
                {
                        send_to_char("You will need to find an anvil.\n\r", ch);
                        return;
                }


                cost_sm =0;
                cost_ad = 0;
                cost_el = 0;
                cost_st = 0;
                cost_ti = 0;

                if ( (obj->level > 90) )
                        cost_sm = 1;

                if (obj->level > 70 )
                        cost_el = 10;

                if (obj->level > 50 )
                        cost_ad = 15;
                if (obj->level > 25 )
                        cost_ti = 20;
                if (obj->level > 15 )
                        cost_st = 10;
                if (obj->level >= 1 )
                        cost_st += 12;

                if ( cost_st > ch->smelted_steel
                ||  cost_ti > ch->smelted_titanium
                ||  cost_ad > ch->smelted_adamantite
                ||  cost_el > ch->smelted_electrum
                ||  cost_sm > ch->smelted_starmetal)
                {
                        send_to_char( "You don't have enough raw materials. You need:\n\r", ch );
                        sprintf(buf, "%d Steel %d Titanium %d Adamantite %d Electrum %d Starmetal",
                        cost_st, cost_ti, cost_ad, cost_el, cost_sm);
                        act(buf, ch, NULL, NULL, TO_CHAR);
                        return;
                }

                if (number_percent() > ch->pcdata->learned[gsn_inscribe])
                {
                        send_to_char("You slip while inscribing!\n\r", ch);
                        act ("$n slips while inscribing $s armour!", ch, NULL, NULL, TO_ROOM);
                        smelted_to_char( cost_st, cost_ti, cost_ad, cost_el, cost_sm, ch, COINS_REPLACE);
                        return;
                }

                act ("$P glows softly as the inscriptions form in the forge.", ch, NULL, obj, TO_CHAR);
                act ("$n skilfully inscribes $P!", ch, NULL, obj, TO_ROOM);
                smelted_to_char( cost_st, cost_ti, cost_ad, cost_el, cost_sm, ch, COINS_REPLACE);

                SET_BIT(obj->extra_flags, ITEM_EGO);
                SET_BIT(obj->ego_flags, EGO_ITEM_INSCRIBED);
                set_obj_owner(obj, ch->name);
                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type           = gsn_inscribe;
                paf->duration       = -1;
                paf->location       = APPLY_INSCRIBED;
                paf->modifier       = 1 + ( ch->level / 15 );
                paf->bitvector      = 0;
                paf->next           = obj->affected;
                obj->affected       = paf;
        }
}

void do_serrate (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *forge;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool found;

        if ( IS_NPC( ch ) )
                return;

        if (!CAN_DO(ch, gsn_serrate))
        {
                send_to_char("What do you think you are, a smithy?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What do you want to serrate?\n\r", ch);
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

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_SERRATED))
        {
                send_to_char("That is already serrated.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_POISONED))
        {
                send_to_char("That weapon is poisoned and cannot be serrated!\n\r", ch);
                return;
        }

        if (!is_bladed_weapon(obj))
        {
                send_to_char("You can only sharpen bladed weapons.\n\r",ch);
                return;
        }

        for (forge = ch->in_room->contents; forge; forge = forge->next_content)
        {
                if (forge->item_type == ITEM_FORGE)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no forge here!\n\r", ch);
                return;
        }


        if (number_percent() > ch->pcdata->learned[gsn_serrate])
        {
                send_to_char("You slip while serrating your blade! You ruin the stone and cut yourself!!\n\r", ch);
                damage(ch, ch, ch->level, gsn_serrate, FALSE);
                act ("$n cuts $mself while serrating $m blade!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        act ("You staggers the edges of the blade of $P, creating a deadly weapon!",
             ch, NULL, obj, TO_CHAR);
        act ("$n staggers the blade of $P, creating a deadly blade!",
             ch, NULL, obj, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_SERRATED);
        set_obj_owner(obj, ch->name);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_serrate;
        paf->duration       = -1;
        paf->location       = APPLY_SERRATED;
        paf->modifier       = (ch->pcdata->learned[gsn_serrate]);
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;
}

void do_imbue (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *forge;
        char            arg[MAX_INPUT_LENGTH];
        char            modifier;
        int             random_buff;
        int value = 1;
        AFFECT_DATA     *paf;
        bool found;

        random_buff = -1;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_imbue))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        for (forge = ch->in_room->contents; forge; forge = forge->next_content)
        {
                if (forge->item_type == ITEM_FORGE)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no forge here!\n\r", ch);
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

        if (IS_SET(obj->extra_flags, ITEM_EGO) && IS_SET(obj->ego_flags, EGO_ITEM_IMBUED))
        {
                send_to_char("That is already imbued.\n\r", ch);
                return;
        }

        /* Now using the random table for alignment with randomiser*/
        random_buff = number_range( 0, MAX_RANDOMS-1);
        modifier = random_list[random_buff].apply_buff;
        value = 250 / (calc_aff_score(modifier,ch->level));
        value = ((modifier == APPLY_AC) ? (-value) : value);

        if (!affect_free)
                paf = alloc_perm(sizeof(*paf));
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type           = gsn_imbue;
        paf->duration       = -1;
        paf->location       = modifier;
       /* paf->modifier       = ch->level * imbue_list[random_buff].base_gain * ch->pcdata->learned[gsn_imbue] / 1000; */
        paf->modifier       = value;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;


        if (ch->pcdata->learned[gsn_imbue] > 75)
        {
                random_buff = number_range( 0, MAX_RANDOMS-1);
                modifier = random_list[random_buff].apply_buff;
                value = 250 / (calc_aff_score(modifier,ch->level));
                value = ((modifier == APPLY_AC) ? (-value) : value);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }
                paf->type           = gsn_imbue;
                paf->duration       = -1;
                paf->location       = modifier;
                paf->modifier       = value;
                paf->bitvector      = 0;
                paf->next           = obj->affected;
                obj->affected       = paf;

        }

        if (ch->pcdata->learned[gsn_imbue] > 97)
        {
                random_buff = number_range( 0, MAX_RANDOMS-1);
                modifier = random_list[random_buff].apply_buff;
                value = 250 / (calc_aff_score(modifier,ch->level));
                value = ((modifier == APPLY_AC) ? (-value) : value);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }
                paf->type           = gsn_imbue;
                paf->duration       = -1;
                paf->location       = modifier;
                paf->modifier       = value;
                paf->next           = obj->affected;
                obj->affected       = paf;
                random_buff = -1;
        }

        obj->how_created    = CREATED_SKILL;
        SET_BIT(obj->extra_flags, ITEM_EGO);
        SET_BIT(obj->ego_flags, EGO_ITEM_IMBUED);
        set_obj_owner(obj, ch->name);

        act ("You immerse the $p within the flames of the forge, imbuing it with power!", ch, obj, NULL, TO_CHAR);
        act ("$n immerses the $p within the flames of the forge, imbuing it with power!", ch, obj, NULL, TO_ROOM);
        return;
}


void do_counterbalance (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *anvil;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA *paf;
        bool found;
        char            buf[MAX_STRING_LENGTH];

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

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        /*
                always succeed - but percentage based on skill      if (number_percent() > ch->pcdata->learned[gsn_counterbalance])
                {
                        send_to_char("You slip while balancing your weapon!!\n\r", ch);
                        act ("$n cuts $mself while balancing $m weapon!", ch, NULL, NULL, TO_ROOM);
                        return;
                }
        */

        sprintf( buf, "You counterbalance %s.\n\r", obj->short_descr );
        send_to_char( buf, ch );
        sprintf( buf, "It's a %d/%d weighting split.\n\r", ch->pcdata->learned[gsn_counterbalance]/2, (100 - (ch->pcdata->learned[gsn_counterbalance]/2)) );
        send_to_char( buf, ch );


        act ("$n expertly balances $p!",
             ch, obj, NULL, TO_ROOM);

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
        paf->modifier       = (ch->pcdata->learned[gsn_counterbalance]);
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

        set_obj_owner(obj, ch->name);
}

void do_trigger (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        OBJ_DATA *turret;
        OBJ_DATA *module;
        OBJ_DATA *turret_unit;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        int     glookup;
        int     unit;
        int     i;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
        {
                return;
        }

        else if (!CAN_DO(ch, gsn_trigger))
        {
                send_to_char("You can't trigger mechanisms.\n\r", ch);
                return;
        }

        if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
        {
                send_to_char("You can't do that.\n\r", ch);
                return;
        }

        if (  (arg1[0] == '\0' ) )
        {
                send_to_char( "Trigger what against who?\n\r", ch );
                return;
        }

        for (turret = ch->in_room->contents; turret; turret = turret->next_content)
        {
                if (turret->item_type == ITEM_TURRET)
                        break;
        }

        if (!turret)
        {
                send_to_char("Your turret is not deployed!\n\r", ch);
                return;
        }


        if (!str_cmp(arg1, "all"))
        {
                send_to_char("You cant trigger everything at once.\n\r", ch);
                return;
        }
        /* trigger 1 object in the turret' */

        /*if (str_cmp(arg1, "all") && str_prefix("all.", arg1)) */
        if (str_prefix("all.", arg1))
        {
                if (!(module = get_obj_list(ch, arg1, turret->contains )) && (arg2[0] == '\0' ))
                {
                        send_to_char("That module is not in your turret.\n\r", ch);
                        return;
                }

                if (module->level > ch->level)
                {
                        act("$p is too high level for you.", ch, module, NULL, TO_CHAR);
                        return;
                }

                /* find some module details - this is for the defensive modules that produce units */
                for (i = 0; i < BLUEPRINTS_MAX; i++)
                {
                        if (module->pIndexData->vnum == blueprint_list[i].blueprint_ref)
                        {
                                glookup = skill_lookup(blueprint_list[i].skill_name);
                                unit = blueprint_list[i].blueprint_damage[2];
                                break;
                        }
                        glookup = -1;
                        unit = -1;
                }

                /* check if a unit already exists */
                for (turret_unit = ch->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
                {
                        if (turret_unit->pIndexData->vnum == unit)
                        {
                                send_to_char("That module unit already exists here.\n\r", ch);
                                return;
                        }
                }

                /* Deploy a defensive module unit */
                if ((module->item_type == ITEM_DEFENSIVE_TURRET_MODULE) && (unit >= 1))
                {

                        if ( number_range(0,100) < ch->pcdata->learned[gsn_trigger])
                        {
                                OBJ_DATA *deployed;

                                deployed = create_object( get_obj_index( unit ), ch->level, "common", CREATED_NO_RANDOMISER );
                                obj_to_room(deployed, ch->in_room);
                                act("Your turret deploys $p.", ch, deployed, NULL ,TO_CHAR);
                                act("$n deploys $p from their turret.", ch, deployed, NULL, TO_ROOM);
                                set_obj_owner(deployed, ch->name);
                                if (--module->value[2] <= 0)
                                {
                                        act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                        act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                        extract_obj(module);
                                }
                                return;
                        }
                        else
                        {
                                act("The triggering mechanism fails for the $p. Nothing happens.", ch, turret, NULL ,TO_CHAR);
                                act("$n triggers $m $p, but nothing happens.", ch, turret, NULL, TO_ROOM);
                                if (--module->value[2] <= 0)
                                {
                                        act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                        act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                        extract_obj(module);
                                }
                                return;
                        }
                }

                if ((module->item_type == ITEM_DEFENSIVE_TURRET_MODULE) && (unit <= 0))
                {
                        send_to_char("BUG: cant lookup unit from module.\n\r", ch);
                        return;
                }

                /* If you have an offensive module */
                if (arg2[0] == '\0')
                {
                        send_to_char ("Who would you trigger an attack on?\n\r", ch);
                        return;
                }

                if (!( victim = get_char_room( ch, arg2) ) )
                {

                        send_to_char ("They aren't here.\n\r", ch);
                        return;
                }

                if (victim == ch)
                {
                        send_to_char("You cant target yourself.\n\r", ch);
                        return;
                }

                if (is_safe(ch, victim))
                        return;

                if (!IS_SET(module->ego_flags, EGO_ITEM_TURRET_MODULE) )
                {
                        send_to_char("How did that get in there - it's not a turret module! (report bug).\n\r", ch);
                        return;
                }

                if (( number_percent() < ch->pcdata->learned[gsn_trigger]) && (glookup > 1))
                {
                        /* moght do a case here depending on type of module      */
                        act("You trigger your $p.", ch, turret, NULL ,TO_CHAR);
                        act("$n triggers $m $p.", ch, turret, NULL, TO_ROOM);
                        damage(ch, victim, number_range(module->value[0],module->value[1]), glookup, FALSE);
                        if (--module->value[2] <= 0)
                        {
                                act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                extract_obj(module);
                        }
                }
                else
                {
                        act("The triggering mechanism fails for the $p. Nothing happens.", ch, turret, NULL ,TO_CHAR);
                        act("$n triggers $m $p, but nothing happens.", ch, turret, NULL, TO_ROOM);
                        if (--module->value[2] <= 0)
                        {
                                act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                extract_obj(module);
                        }
                }
                return;
        }
        else
        {
                /* trigger  ' ALL.modules' */
                OBJ_DATA *module_next;

                for (module = turret->contains; module; module = module_next)
                {
                        module_next = module->next_content;

                        if ((arg1[3] == '\0' || is_name(&arg1[4], module->name)))
                        {
                                if (!(module = get_obj_list(ch, &arg1[4], turret->contains )))
                                {
                                        send_to_char("That module is not in your turret.\n\r", ch);
                                        return;
                                }

                                if (module->item_type == ITEM_DEFENSIVE_TURRET_MODULE)
                                        return;

                                if (module->level > ch->level)
                                {
                                        act("$p is too high level for you.", ch, module, NULL, TO_CHAR);
                                        return;
                                }

                                /* find some module details - this is for the defensive modules that produce units */
                                for (i = 0; i < BLUEPRINTS_MAX; i++)
                                {
                                        if (module->pIndexData->vnum == blueprint_list[i].blueprint_ref)
                                        {
                                                glookup = skill_lookup(blueprint_list[i].skill_name);
                                                unit = blueprint_list[i].blueprint_damage[2];
                                                break;
                                        }
                                        glookup = -1;
                                        unit = -1;
                                }

                                /* check if a unit already exists */
                                for (turret_unit = ch->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
                                {
                                        if (turret_unit->pIndexData->vnum == unit)
                                        {
                                                send_to_char("That module unit already exists here.\n\r", ch);
                                                return;
                                        }
                                }

                                /* Deploy a defensive module unit */
                                if ((module->item_type == ITEM_DEFENSIVE_TURRET_MODULE) && (unit >= 1))
                                {

                                        if ( number_range(0,100) < ch->pcdata->learned[gsn_trigger])
                                        {
                                                OBJ_DATA *deployed;

                                                deployed = create_object( get_obj_index( unit ), ch->level, "common", CREATED_NO_RANDOMISER );
                                                obj_to_room(deployed, ch->in_room);
                                                act("Your turret deploys $p.", ch, deployed, NULL ,TO_CHAR);
                                                act("$n deploys $p from their turret.", ch, deployed, NULL, TO_ROOM);
                                                set_obj_owner(deployed, ch->name);
                                                if (--module->value[2] <= 0)
                                                {
                                                        act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                                        act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                                        extract_obj(module);
                                                }
                                                return;
                                        }
                                        else
                                        {
                                                act("The triggering mechanism fails for the $p. Nothing happens.", ch, turret, NULL ,TO_CHAR);
                                                act("$n triggers $m $p, but nothing happens.", ch, turret, NULL, TO_ROOM);
                                                if (--module->value[2] <= 0)
                                                {
                                                        act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                                        act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                                        extract_obj(module);
                                                }
                                                return;
                                        }
                                }

                                if ((module->item_type == ITEM_DEFENSIVE_TURRET_MODULE) && (unit <= 0))
                                {
                                        send_to_char("BUG: cant lookup unit from module.\n\r", ch);
                                        return;
                                }

                                /* If you have an offensive module */
                                if (arg2[0] == '\0')
                                {
                                        send_to_char ("Who would you trigger an attack on?\n\r", ch);
                                        return;
                                }

                                if (!( victim = get_char_room( ch, arg2) ) )
                                {

                                        send_to_char ("They aren't here.\n\r", ch);
                                        return;
                                }

                                if (victim == ch)
                                {
                                        send_to_char("You cant target yourself.\n\r", ch);
                                        return;
                                }

                                if (is_safe(ch, victim))
                                        return;

                                if (!IS_SET(module->ego_flags, EGO_ITEM_TURRET_MODULE) )
                                {
                                        send_to_char("How did that get in there - it's not a turret module! (report bug).\n\r", ch);
                                        return;
                                }

                                if (( number_percent() < ch->pcdata->learned[gsn_trigger]) && (glookup > 1))
                                {
                                        damage(ch, victim, number_range(module->value[0],module->value[1]), glookup, FALSE);
                                        if (--module->value[2] <= 0)
                                        {
                                                act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                                act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                                extract_obj(module);
                                        }
                                }
                                else
                                {
                                        act("The triggering mechanism fails for the $p. Nothing happens.", ch, module, NULL ,TO_CHAR);
                                        act("$n triggers $m $p, but nothing happens.", ch, turret, NULL, TO_ROOM);
                                        if (--module->value[2] <= 0)
                                        {
                                                act("Your $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_CHAR);
                                                act("$n's $p is empty. It ejects itself from the turret.", ch, module, NULL, TO_ROOM);
                                                extract_obj(module);
                                        }
                                }
                        }
                }
        }
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
            && obj->item_type != ITEM_SMOKEABLE
            && obj->item_type != ITEM_POTION )
        {
                send_to_char( "Your herb lore won't help you describe that type of object.\n\r", ch );
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
                sprintf( buf, "%s looks like <15>food<0>", capitalize( obj->short_descr ) );
                if( obj->value[3] != 0 )
                        strcat( buf, ".  It's probably not safe to consume." );
        }
        else
        {
                sprintf( buf, "%s looks like a ", capitalize( obj->short_descr ) );
                if( obj->item_type == ITEM_PILL )
                        strcat( buf, "<15>pill<0>" );
                else if( obj->item_type == ITEM_POTION )
                        strcat( buf, "<15>potion<0>" );
                else if( obj->item_type == ITEM_SMOKEABLE )
                        strcat( buf, "<15>smokeable substance<0>" );
                else strcat( buf, "<15>paint<0>" );
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
            || obj->value[3] == 16
            || obj->value[3] == 19)
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

bool is_carving_weapon (OBJ_DATA *obj)
{
        if (!obj || obj->item_type != ITEM_WEAPON)
                return FALSE;

        if ( obj->value[3] == 1
        ||   obj->value[3] == 2
        ||   obj->value[3] == 3
        ||   obj->value[3] == 11
        ||   obj->value[3] == 13
        ||   obj->value[3] == 19)
                return TRUE;

        return FALSE;
}

int scale_pipe(int limit_level, int load_level, int base_value, bool higher_bad)
{
    float scaled_value;
    float percentage_diff;

    if (load_level == 0)
    {
            return base_value;
    }

    percentage_diff = ((float)(limit_level - load_level) / load_level) * 100;

    if (higher_bad)
    {
            scaled_value = base_value + (base_value * (percentage_diff / 100));
    }
    else {
            scaled_value = base_value - (base_value * (percentage_diff / 100));
    }

    if (scaled_value > 230)
    {
        scaled_value = 230;
    }
    else if (scaled_value < 4)
    {
        scaled_value = 4;
    }

    return (int)scaled_value;
}


/*
 * Gem and Socket System Commands
 *
 * Intent: Allow Smithy class players to manipulate gems and sockets.
 * - gem_set: Place a gem in an empty socket
 * - gem_unset: Remove a gem from a socket
 * - gem_socket: Add a new socket to an item (requires tinker NPC)
 * - gem_combine: Combine gems to upgrade quality
 */

/*
 * do_gem_set: Place a gem into an item's socket.
 *
 * Syntax: set <gem> <item>
 *
 * Requirements:
 * - Player must have the gem_set skill (Smithy class)
 * - Item must have an empty socket
 * - Gem must be in player's inventory
 */
void do_gem_set( CHAR_DATA *ch, char *argument )
{
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *gem;
        OBJ_DATA *item;
        int i;
        int empty_socket = -1;

        if ( IS_NPC(ch) )
                return;

        if ( !CAN_DO(ch, gsn_gem_set) )
        {
                send_to_char( "You don't know how to set gems into items.\n\r", ch );
                return;
        }

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
                send_to_char( "Syntax: set <<gem> <<item>\n\r", ch );
                return;
        }

        /* Find the gem */
        if ( ( gem = get_obj_carry( ch, arg1 ) ) == NULL )
        {
                send_to_char( "You don't have that gem.\n\r", ch );
                return;
        }

        if ( gem->item_type != ITEM_GEM )
        {
                send_to_char( "That's not a gem.\n\r", ch );
                return;
        }

        /* Find the item */
        if ( ( item = get_obj_carry( ch, arg2 ) ) == NULL )
        {
                /* Check equipped items too */
                item = get_obj_wear( ch, arg2 );
                if ( item == NULL )
                {
                        send_to_char( "You don't have that item.\n\r", ch );
                        return;
                }
        }

        if ( !can_socket_item( item ) )
        {
                send_to_char( "That item cannot have gems socketed into it.\n\r", ch );
                return;
        }

        if ( item->socket_count <= 0 )
        {
                send_to_char( "That item has no sockets.\n\r", ch );
                return;
        }

        /* Find an empty socket */
        for ( i = 0; i < item->socket_count && i < MAX_SOCKETS; i++ )
        {
                if ( item->socket_gem_type[i] < 0 )
                {
                        empty_socket = i;
                        break;
                }
        }

        if ( empty_socket < 0 )
        {
                send_to_char( "All sockets on that item are already filled.\n\r", ch );
                return;
        }

        /* Check skill success */
        if ( number_percent() > ch->pcdata->learned[gsn_gem_set] )
        {
                send_to_char( "You fumble and fail to set the gem properly.\n\r", ch );
                
                WAIT_STATE( ch, skill_table[gsn_gem_set].beats );
                return;
        }

        /* Set the gem into the socket */
        item->socket_gem_type[empty_socket] = gem->value[0];     /* gem type stored in value[0] */
        item->socket_gem_quality[empty_socket] = gem->value[1];  /* quality stored in value[1] */

        /* If item is equipped, apply the gem bonus immediately */
        if ( item->wear_loc != WEAR_NONE )
        {
                int bonus = get_gem_bonus( gem->value[0], gem->value[1] );
                AFFECT_DATA af;

                af.type      = 0;
                af.duration  = -1;
                af.location  = gem_table[gem->value[0]].apply_type;
                af.modifier  = bonus;
                af.bitvector = 0;

                affect_modify( ch, &af, TRUE, item );
        }

        sprintf( buf, "You carefully set %s into %s.\n\r",
                gem->short_descr, item->short_descr );
        send_to_char( buf, ch );

        act( "$n sets $p into $P.", ch, gem, item, TO_ROOM );

        
        WAIT_STATE( ch, skill_table[gsn_gem_set].beats );

        /* Remove and destroy the gem object */
        extract_obj( gem );
}


/*
 * do_gem_unset: Remove a gem from an item's socket.
 *
 * Syntax: unset <item> [socket#]
 *
 * Requirements:
 * - Player must have the gem_unset skill (Smithy class)
 * - Item must have a gem in a socket
 */
void do_gem_unset( CHAR_DATA *ch, char *argument )
{
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *item;
        OBJ_DATA *gem;
        OBJ_INDEX_DATA *gem_index;
        int socket_num = 0;
        int gem_type;
        int gem_quality;
        int i;

        if ( IS_NPC(ch) )
                return;

        if ( !CAN_DO(ch, gsn_gem_unset) )
        {
                send_to_char( "You don't know how to remove gems from items.\n\r", ch );
                return;
        }

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Syntax: unset <<item> [socket#]\n\r", ch );
                return;
        }

        /* Find the item */
        if ( ( item = get_obj_carry( ch, arg1 ) ) == NULL )
        {
                item = get_obj_wear( ch, arg1 );
                if ( item == NULL )
                {
                        send_to_char( "You don't have that item.\n\r", ch );
                        return;
                }
        }

        if ( item->socket_count <= 0 )
        {
                send_to_char( "That item has no sockets.\n\r", ch );
                return;
        }

        /* If socket number specified, use it; otherwise find first filled socket */
        if ( arg2[0] != '\0' )
        {
                socket_num = atoi( arg2 ) - 1;  /* User specifies 1-based */
                if ( socket_num < 0 || socket_num >= item->socket_count )
                {
                        send_to_char( "Invalid socket number.\n\r", ch );
                        return;
                }
        }
        else
        {
                /* Find first filled socket */
                socket_num = -1;
                for ( i = 0; i < item->socket_count && i < MAX_SOCKETS; i++ )
                {
                        if ( item->socket_gem_type[i] >= 0 )
                        {
                                socket_num = i;
                                break;
                        }
                }
        }

        if ( socket_num < 0 || item->socket_gem_type[socket_num] < 0 )
        {
                send_to_char( "That socket is empty.\n\r", ch );
                return;
        }

        gem_type = item->socket_gem_type[socket_num];
        gem_quality = item->socket_gem_quality[socket_num];

        /* Check skill success - failure may destroy gem */
        if ( number_percent() > ch->pcdata->learned[gsn_gem_unset] )
        {
                send_to_char( "You fumble and the gem shatters!\n\r", ch );
                item->socket_gem_type[socket_num] = -1;
                item->socket_gem_quality[socket_num] = 0;

                /* Remove the bonus if equipped */
                if ( item->wear_loc != WEAR_NONE )
                {
                        int bonus = get_gem_bonus( gem_type, gem_quality );
                        AFFECT_DATA af;

                        af.type      = 0;
                        af.duration  = -1;
                        af.location  = gem_table[gem_type].apply_type;
                        af.modifier  = -bonus;
                        af.bitvector = 0;

                        affect_modify( ch, &af, TRUE, item );
                }

                
                WAIT_STATE( ch, skill_table[gsn_gem_unset].beats );
                return;
        }

        /* Create a gem object to give back to the player */
        /* We need a template gem object - use a generic one or create dynamically */
        gem_index = get_obj_index( OBJ_VNUM_GEM_TEMPLATE );  /* Use a generic object vnum as template */
        if ( gem_index != NULL )
        {
                gem = create_object( gem_index, 0, "common", CREATED_SKILL );
                gem->item_type = ITEM_GEM;
                gem->value[0] = gem_type;
                gem->value[1] = gem_quality;

                free_string( gem->name );
                sprintf( buf, "gem %s %s",
                        gem_quality_name( gem_quality ),
                        gem_type_name( gem_type ) );
                gem->name = str_dup( buf );

                free_string( gem->short_descr );
                sprintf( buf, "a %s %s (%s %+d)",
                        gem_quality_name( gem_quality ),
                        gem_type_name( gem_type ),
                        affect_loc_name( gem_table[gem_type].apply_type ),
                        get_gem_bonus( gem_type, gem_quality ) );
                gem->short_descr = str_dup( buf );

                free_string( gem->description );
                sprintf( buf, "A %s %s lies here.",
                        gem_quality_name( gem_quality ),
                        gem_type_name( gem_type ) );
                gem->description = str_dup( buf );

                obj_to_char( gem, ch );
        }

        /* Clear the socket */
        item->socket_gem_type[socket_num] = -1;
        item->socket_gem_quality[socket_num] = 0;

        /* Remove the bonus if equipped */
        if ( item->wear_loc != WEAR_NONE )
        {
                int bonus = get_gem_bonus( gem_type, gem_quality );
                AFFECT_DATA af;

                af.type      = 0;
                af.duration  = -1;
                af.location  = gem_table[gem_type].apply_type;
                af.modifier  = -bonus;
                af.bitvector = 0;

                affect_modify( ch, &af, TRUE, item );
        }

        sprintf( buf, "You carefully remove the %s from %s.\n\r",
                gem_type_name( gem_type ), item->short_descr );
        send_to_char( buf, ch );

        act( "$n removes a gem from $P.", ch, NULL, item, TO_ROOM );

        
        WAIT_STATE( ch, skill_table[gsn_gem_unset].beats );
}


/*
 * do_gem_socket: Add a socket to an item.
 *
 * Syntax: socket <item>
 *
 * Requirements:
 * - Player must have the gem_socket skill (Smithy class)
 * - Must be at a tinker NPC
 * - Item must be able to have sockets
 * - Item must not be at max sockets for its slot type
 * - Costs increase with each additional socket
 */
void do_gem_socket( CHAR_DATA *ch, char *argument )
{
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *item;
        CHAR_DATA *tinker = NULL;
        int max_slots;
        int cost;
        int current_sockets;

        if ( IS_NPC(ch) )
                return;

        if ( !CAN_DO(ch, gsn_gem_socket) )
        {
                send_to_char( "You don't know how to add sockets to items.\n\r", ch );
                return;
        }

        /* Check for tinker NPC in room */
        for ( tinker = ch->in_room->people; tinker; tinker = tinker->next_in_room )
        {
                if ( IS_NPC(tinker) && IS_SET(tinker->pIndexData->act, ACT_TINKER) )
                        break;
        }

        if ( !tinker )
        {
                send_to_char( "You need to be at a tinker's shop to add sockets.\n\r", ch );
                return;
        }

        argument = one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Syntax: socket <<item>\n\r", ch );
                return;
        }

        /* Find the item */
        if ( ( item = get_obj_carry( ch, arg ) ) == NULL )
        {
                send_to_char( "You don't have that item.\n\r", ch );
                return;
        }

        if ( !can_socket_item( item ) )
        {
                send_to_char( "That item cannot have sockets added to it.\n\r", ch );
                return;
        }

        /* Check max sockets based on item's potential wear location */
        max_slots = MAX_SOCKETS_LARGE;  /* Default to large for items in inventory */

        /* If armor, check its wear flags */
        if ( item->item_type == ITEM_ARMOR )
        {
                if ( IS_SET(item->wear_flags, ITEM_WEAR_FINGER) )
                        max_slots = MAX_SOCKETS_SMALL;
                else if ( IS_SET(item->wear_flags, ITEM_WEAR_NECK) )
                        max_slots = MAX_SOCKETS_SMALL;
                else if ( IS_SET(item->wear_flags, ITEM_WEAR_WAIST) )
                        max_slots = MAX_SOCKETS_MEDIUM;
                else if ( IS_SET(item->wear_flags, ITEM_WEAR_HANDS) )
                        max_slots = MAX_SOCKETS_MEDIUM;
                else if ( IS_SET(item->wear_flags, ITEM_WEAR_FEET) )
                        max_slots = MAX_SOCKETS_MEDIUM;
                else if ( IS_SET(item->wear_flags, ITEM_WEAR_WRIST) )
                        max_slots = MAX_SOCKETS_MEDIUM;
        }

        current_sockets = item->socket_count;

        if ( current_sockets >= max_slots )
        {
                send_to_char( "That item already has the maximum number of sockets.\n\r", ch );
                return;
        }

        /* Cost increases with each socket: 1st=1000, 2nd=3000, 3rd=8000, 4th=21000 (Fibonacci-ish) */
        switch ( current_sockets )
        {
            case 0:  cost = 1000 * COIN_PLAT; break;
            case 1:  cost = 3000 * COIN_PLAT; break;
            case 2:  cost = 8000 * COIN_PLAT; break;
            case 3:  cost = 21000 * COIN_PLAT; break;
            default: cost = 50000 * COIN_PLAT; break;
        }

        if ( total_coins_char(ch) < cost )
        {
                sprintf( buf, "Adding a socket costs %d platinum. You don't have enough.\n\r",
                        cost / COIN_PLAT );
                send_to_char( buf, ch );
                return;
        }

        /* Check skill success */
        if ( number_percent() > ch->pcdata->learned[gsn_gem_socket] )
        {
                /* Failure - lose half the cost */
                coins_from_char( cost / 2, ch );
                send_to_char( "The tinker's attempt to add a socket fails, damaging the item slightly.\n\r", ch );
                
                WAIT_STATE( ch, skill_table[gsn_gem_socket].beats * 2 );
                return;
        }

        /* Success - pay and add socket */
        coins_from_char( cost, ch );
        item->socket_count++;

        /* Initialize the new socket as empty */
        item->socket_gem_type[item->socket_count - 1] = -1;
        item->socket_gem_quality[item->socket_count - 1] = 0;

        sprintf( buf, "The tinker carefully adds a socket to %s.\n\r", item->short_descr );
        send_to_char( buf, ch );
        sprintf( buf, "It now has %d socket%s.\n\r",
                item->socket_count, item->socket_count == 1 ? "" : "s" );
        send_to_char( buf, ch );

        act( "$n has a socket added to $p.", ch, item, NULL, TO_ROOM );

        
        WAIT_STATE( ch, skill_table[gsn_gem_socket].beats );
}


/*
 * do_gem_combine: Combine gems to create a higher quality gem.
 *
 * Syntax: gemcombine <gem type>
 *
 * Requirements:
 * - Player must have the gem_combine skill (Smithy class)
 * - Must have enough gems of the same type and quality
 * - Combining thresholds: 2 Dull->Cloudy, 3 Cloudy->Clear, 5 Clear->Brilliant, 8 Brilliant->Flawless
 */
void do_gem_combine( CHAR_DATA *ch, char *argument )
{
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *gem;
        OBJ_DATA *gem_next;
        OBJ_DATA *new_gem;
        OBJ_INDEX_DATA *gem_index;
        int gem_type = -1;
        int gem_quality = -1;
        int count = 0;
        int needed = 0;
        int gems_used = 0;

        if ( IS_NPC(ch) )
                return;

        if ( !CAN_DO(ch, gsn_gem_combine) )
        {
                send_to_char( "You don't know how to combine gems.\n\r", ch );
                return;
        }

        argument = one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
                send_to_char( "Syntax: gemcombine <<gem type>\n\r", ch );
                send_to_char( "This will combine your lowest quality gems of that type.\n\r", ch );
                return;
        }

        /* Look up the gem type */
        gem_type = gem_type_lookup( arg );
        if ( gem_type < 0 )
        {
                send_to_char( "That's not a valid gem type.\n\r", ch );
                send_to_char( "Valid types: garnet, chrysoberyl, sapphire, amethyst, jade, bloodstone,\n\r", ch );
                send_to_char( "            onyx, jasper, ruby, lapis, opal, aquamarine, amber, peridot,\n\r", ch );
                send_to_char( "            diamond, sunstone\n\r", ch );
                return;
        }

        /* Find the lowest quality gems of this type and count them */
        for ( gem_quality = GEM_QUALITY_DULL; gem_quality < GEM_QUALITY_FLAWLESS; gem_quality++ )
        {
                count = 0;
                for ( gem = ch->carrying; gem; gem = gem->next_content )
                {
                        if ( gem->item_type == ITEM_GEM
                             && gem->value[0] == gem_type
                             && gem->value[1] == gem_quality )
                        {
                                count++;
                        }
                }

                if ( count > 0 )
                        break;  /* Found lowest quality with gems */
        }

        if ( gem_quality >= GEM_QUALITY_FLAWLESS )
        {
                send_to_char( "You don't have any gems of that type to combine, or they are already flawless.\n\r", ch );
                return;
        }

        /* Determine how many are needed to combine */
        switch ( gem_quality )
        {
            case GEM_QUALITY_DULL:      needed = GEM_COMBINE_DULL; break;
            case GEM_QUALITY_CLOUDY:    needed = GEM_COMBINE_CLOUDY; break;
            case GEM_QUALITY_CLEAR:     needed = GEM_COMBINE_CLEAR; break;
            case GEM_QUALITY_BRILLIANT: needed = GEM_COMBINE_BRILLIANT; break;
            default: needed = 99; break;
        }

        if ( count < needed )
        {
                sprintf( buf, "You need %d %s %ss to combine into a %s one.\n\r",
                        needed,
                        gem_quality_name( gem_quality ),
                        gem_type_name( gem_type ),
                        gem_quality_name( gem_quality + 1 ) );
                send_to_char( buf, ch );
                sprintf( buf, "You have %d.\n\r", count );
                send_to_char( buf, ch );
                return;
        }

        /* Check skill success */
        if ( number_percent() > ch->pcdata->learned[gsn_gem_combine] )
        {
                /* Failure - lose one gem */
                for ( gem = ch->carrying; gem; gem = gem_next )
                {
                        gem_next = gem->next_content;
                        if ( gem->item_type == ITEM_GEM
                             && gem->value[0] == gem_type
                             && gem->value[1] == gem_quality )
                        {
                                extract_obj( gem );
                                break;
                        }
                }
                send_to_char( "You fumble the combination and a gem shatters!\n\r", ch );
                
                WAIT_STATE( ch, skill_table[gsn_gem_combine].beats );
                return;
        }

        /* Remove the required gems */
        gems_used = 0;
        for ( gem = ch->carrying; gem && gems_used < needed; gem = gem_next )
        {
                gem_next = gem->next_content;
                if ( gem->item_type == ITEM_GEM
                     && gem->value[0] == gem_type
                     && gem->value[1] == gem_quality )
                {
                        extract_obj( gem );
                        gems_used++;
                }
        }

        /* Create the new higher quality gem */
        gem_index = get_obj_index( OBJ_VNUM_GEM_TEMPLATE );  /* Generic object template */
        if ( gem_index != NULL )
        {
                new_gem = create_object( gem_index, 0, "common", CREATED_SKILL );
                new_gem->item_type = ITEM_GEM;
                new_gem->value[0] = gem_type;
                new_gem->value[1] = gem_quality + 1;

                free_string( new_gem->name );
                sprintf( buf, "gem %s %s",
                        gem_quality_name( gem_quality + 1 ),
                        gem_type_name( gem_type ) );
                new_gem->name = str_dup( buf );

                free_string( new_gem->short_descr );
                sprintf( buf, "a %s %s (%s %+d)",
                        gem_quality_name( gem_quality + 1 ),
                        gem_type_name( gem_type ),
                        affect_loc_name( gem_table[gem_type].apply_type ),
                        get_gem_bonus( gem_type, gem_quality + 1 ) );
                new_gem->short_descr = str_dup( buf );

                free_string( new_gem->description );
                sprintf( buf, "A %s %s lies here.",
                        gem_quality_name( gem_quality + 1 ),
                        gem_type_name( gem_type ) );
                new_gem->description = str_dup( buf );

                obj_to_char( new_gem, ch );

                sprintf( buf, "You combine %d %s %ss into a %s %s!\n\r",
                        needed,
                        gem_quality_name( gem_quality ),
                        gem_type_name( gem_type ),
                        gem_quality_name( gem_quality + 1 ),
                        gem_type_name( gem_type ) );
                send_to_char( buf, ch );

                act( "$n combines several gems into a more brilliant one.", ch, NULL, NULL, TO_ROOM );
        }

        
        WAIT_STATE( ch, skill_table[gsn_gem_combine].beats );
}


