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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
bool    check_blink          args((CHAR_DATA *ch, CHAR_DATA *victim));
bool    check_dodge          args((CHAR_DATA *ch, CHAR_DATA *victim));
bool    check_acrobatics     args((CHAR_DATA *ch, CHAR_DATA *victim));
bool    check_aura_of_fear   args((CHAR_DATA *ch, CHAR_DATA *victim));
void    check_killer         args((CHAR_DATA *ch, CHAR_DATA *victim));
bool    check_parry          args((CHAR_DATA *ch, CHAR_DATA *victim));
void    dam_message          args((CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool poison, bool crit));
bool    check_shield_block   args((CHAR_DATA *ch, CHAR_DATA *victim));
void    death_cry            args((CHAR_DATA *ch));
void    group_gain           args((CHAR_DATA *ch, CHAR_DATA *victim, bool mob_called));
int     xp_compute           args((CHAR_DATA *gch, CHAR_DATA *victim));
void    make_corpse          args((CHAR_DATA *ch));
void    disarm               args((CHAR_DATA *ch, CHAR_DATA *victim));
void    trip                 args((CHAR_DATA *ch, CHAR_DATA *victim));
void    use_magical_item     args((CHAR_DATA *ch));
bool    check_arrestor_unit  args((CHAR_DATA *ch, CHAR_DATA *victim, int dt));
bool    check_shield_unit    args((CHAR_DATA *ch, CHAR_DATA *victim, int dt));
bool    check_driver_unit    args((CHAR_DATA *ch, CHAR_DATA *victim));
bool    remove_bodypart      args((CHAR_DATA *ch, int iWear, bool fReplace));


/*
 * Death blurb
 */
const char* purgatory_message =
        "Your vision clouds, your senses become dulled.  Strange vapours\n\r"
        "gather about your twitching and bleeding body, and there is the\n\r"
        "sensation of flight.  Dazzling lights flicker within your mind\n\r"
        "and strange whispers are heard from all directions.\n\r\n\r"
        "After an age, your head clears.  You stand bewildered in the hazy\n\r"
        "gloom of the Underworld...\n\r\n\r";


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Slightly less efficient than Merc 2.2.  Takes 10% of
 * total CPU time.
 */

void state_update (void)
{
        CHAR_DATA *ch;
    /*    char      buf [ MAX_STRING_LENGTH ];

        sprintf(last_function, "entering status_update");
        sprintf(buf, "Entered state_update" );
        bug(buf, 0); */

        for (ch = char_list; ch; ch = ch->next)
        {
                if (!ch->in_room || ch->deleted)
                        continue;

                if ((IS_AFFECTED(ch, AFF_DAZED)) || (IS_AFFECTED(ch, AFF_PRONE)))
                {
                        AFFECT_DATA *paf;

                        for (paf = ch->affected; paf; paf = paf->next)
                        {
                                if ((paf->bitvector == AFF_DAZED) ||(paf->bitvector == AFF_PRONE) )
                                {
                                        if ( paf->deleted )
                                                continue;

                                        if (paf->duration > 0)
                                                paf->duration--;

                                        if (paf->duration <= 0)
                                        {
                                                send_to_char(skill_table[paf->type].msg_off, ch);
                                                send_to_char("\n\r", ch);
                                                act("<15>$n recovers, and re-focuses on the fight.<0>", ch, NULL, NULL, TO_ROOM);
                                                affect_remove(ch, paf);
                                        }

/*
                                        if (paf->bitvector == AFF_DAZED)
                                        {
                                                if (paf->duration <=0 )
                                                {
                                                        REMOVE_BIT(ch->affected_by, AFF_DAZED);
                                                        affect_strip(ch, paf->type);
                                                        char_update
                                                        act("$n recovers, and re-focuses on the fight.", ch, NULL, NULL, TO_ROOM);
                                                        act("You regain your senses, and re-focus on the fight at hand.", ch, NULL, NULL, TO_CHAR);
                                                }
                                                if (paf->duration > 0 )
                                                {
                                                        act("$n is DAZED, unable to attack or defend themselves.", ch, NULL, NULL, TO_ROOM);
                                                        return;
                                                }
                                        }
                                        else if (paf->bitvector == AFF_PRONE)
                                        {
                                                if (paf->duration <=0 )
                                                {
                                                        REMOVE_BIT(ch->affected_by, AFF_PRONE);
                                                        affect_strip(ch, paf->type);
                                                        act("$n is longer vulnerable to attack.", ch, NULL, NULL, TO_ROOM);
                                                        act("You clamber back to your feet.", ch, NULL, NULL, TO_CHAR);
                                                }
                                                if (paf->duration > 0 )
                                                {
                                                        act("$n is vulnerable, lying PRONE on the ground.", ch, NULL, NULL, TO_ROOM);
                                                }
                                        } */

                                }
                        }
                }
        }
        sprintf (last_function, "leaving state_update");
}

void violence_update (void)
{
        CHAR_DATA *ch;
        CHAR_DATA *victim;
        CHAR_DATA *rch;
        bool       mobfighting;
        bool       grmobfighting;
        int        tmp;
        /* char buf2 [ MAX_STRING_LENGTH ];*/

        sprintf(last_function, "entering violence_update");

        for (ch = char_list; ch; ch = ch->next)
        {
                if (!ch->in_room || ch->deleted)
                        continue;

                if ((victim = ch->fighting))
                {
                        if (IS_AWAKE(ch) && ch->in_room == victim->in_room)
                                multi_hit(ch, victim, TYPE_UNDEFINED);
                        else
                                stop_fighting(ch, FALSE);

                        if (IS_NPC(ch))
                        {
                                mprog_hitprcnt_trigger(ch, victim);
                                mprog_fight_trigger(ch, victim);
                        }

                        continue;
                }

                if (IS_AFFECTED(ch, AFF_BLIND)
                    || (IS_NPC(ch) && ch->pIndexData->pShop)
                    || (IS_NPC(ch) && ch->rider))
                        continue;

                /*
                 * Ok. So ch is not fighting anyone.
                 * Is there a fight going on?
                 */


                mobfighting = FALSE;
                grmobfighting = FALSE;

                for (rch = ch->in_room->people; rch; rch = rch->next_in_room)
                {
                        if (rch->deleted
                            || !IS_AWAKE(rch)
                            || (rch == ch)
                            || !(victim = rch->fighting))
                                continue;

                        if (IS_NPC(ch)
                            && ch->fighting == NULL
                            && IS_AWAKE(ch)
                            && ch->hunting != NULL)
                        {
                                hunt_victim(ch);
                                continue;
                        }

                        /* group member is fighting an NPC */
                        if ( !IS_NPC(ch)
                            && (!IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM))
                            && is_same_group(ch, rch)
                            && IS_NPC(victim))
                        {
                                break;
                        }

                        /* one of our grouped mobs is fighting */
                        if ( IS_NPC(rch)
                        &&   IS_NPC(victim)
                        &&   is_same_group(ch, rch)
                        &&   ( rch != victim ) )
                        {
                                mobfighting = TRUE;
                                grmobfighting = TRUE;
                                break;
                        }

                        /* another mob is fighting a player */
                        if (IS_NPC(ch)
                            && IS_NPC(rch)
                            && !IS_NPC(victim))
                        {
                                mobfighting = TRUE;
                                break;
                        }
                }

                if (!victim || !rch)
                        continue;

                /*
                 * Now that someone is fighting, consider fighting another pc
                 * or not at all.
                 */
                if (mobfighting)
                {
                        CHAR_DATA *vch;
                        int        number;
                        number = 0;
                        tmp = number_percent();

                        /*sprintf(buf2, "vch: %s rch: %s victim: %s\r\n",
                        vch->short_descr, rch->short_descr, victim->short_descr);*/
                        /* sprintf(buf2, "ch: %s victim: %s\r\n",
                                IS_NPC(ch) ? ch->short_descr : ch->name,
                                victim->short_descr );
                        log_string(buf2);*/

                        if ( grmobfighting)
                        {
                                /*It's a mob we're grouped with fighting, join in. */
                                if( CAN_SPEAK(ch) )
                                {
                                        if (tmp < 33)
                                        {
                                                act( "{Y$c shrieks '$C must be DESTROYED!'{x", ch, NULL, victim, TO_NOTVICT );
                                                act( "{Y$c shrieks 'You must be DESTROYED!'{x", ch, NULL, victim, TO_VICT );
                                        }
                                        else if (tmp < 66)
                                        {
                                                act( "{Y$c screeches 'I will feast on $N's BLOOD!'{x", ch, NULL, victim, TO_NOTVICT );
                                                act( "{Y$c screeches 'I will feast on your BLOOD!'{x", ch, NULL, victim, TO_VICT );
                                        }
                                        else
                                        {
                                                act( "{Y$c screams 'I demand that $N unhand my comrade!'{x", ch, NULL, victim, TO_NOTVICT );
                                                act( "{Y$c screams 'Unhand my comrade, fiend!'{x", ch, NULL, victim, TO_VICT );
                                        }
                                }
                                else
                                {
                                        if (tmp < 50)
                                        {
                                                act( "{Y$c suddenly lunges at $N!{x", ch, NULL, victim, TO_NOTVICT );
                                                act( "{Y$c suddenly lunges at you!{x", ch, NULL, victim, TO_VICT );
                                        }
                                        else
                                        {
                                                act( "{Y$c rears and leaps at $N!{x", ch, NULL, victim, TO_NOTVICT );
                                                act( "{Y$c rears and leaps at you!{x", ch, NULL, victim, TO_VICT );
                                        }
                                }

                                multi_hit(ch, victim, TYPE_UNDEFINED);
                                return;
                        }

                        /* mob attacks member of pc's group */
                        for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
                        {
                                if (can_see(ch, vch)
                                    && is_same_group(vch, victim)
                                    && number_range(0, number) == 0)
                                {
                                        victim = vch;
                                        number++;
                                }
                        }

                        if ((rch->pIndexData != ch->pIndexData && number_bits(3) != 0)
                            || (IS_GOOD(ch) && IS_GOOD(victim))
                            || victim->level - ch->level > 3
                            || victim->level - ch->level < -6)
                                continue;

                        if ( ch->master && (ch->master == victim) )
                                continue;

                        /* Don't attack someone in your own group */
                        if ( ch->master && ch->master == victim->master)
                                continue;

                        /* Don't attack yourself! */
                        if ( ch == victim)
                                continue;

                        if (!IS_AWAKE(ch) && number_bits(3))
                                continue;

                        if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_DETER))
                                continue;

                        if ((ch->mount && ch->mount == victim)
                            || (ch->rider && ch->rider == victim)
                            || is_group_members_mount (victim, ch))
                                continue;

                        if (!mob_interacts_players(ch))
                                continue;
                }

                tmp = number_percent();
                if( CAN_SPEAK(ch) )
                {
                        if (tmp < 33)
                        {
                                act( "{Y$c screams '$N must DIE!'{x", ch, NULL, victim, TO_NOTVICT );
                                act( "{Y$c screams 'You must DIE!'{x", ch, NULL, victim, TO_VICT );
                        }
                        else if (tmp < 66)
                        {
                                act( "{Y$c screeches 'I will taste $N's BLOOD!'{x", ch, NULL, victim, TO_NOTVICT );
                                act( "{Y$c screeches 'I will taste your BLOOD!'{x", ch, NULL, victim, TO_VICT );
                        }
                        else
                        {
                                act( "{Y$c yells '$C, prepare for DEATH!'{x", ch, NULL, victim, TO_NOTVICT );
                                act( "{Y$c yells 'Prepare for DEATH!'{x", ch, NULL, victim, TO_VICT );
                        }
                }
                else
                {
                        if (tmp < 50)
                        {
                                act( "{Y$c suddenly attacks $C!{x", ch, NULL, victim, TO_NOTVICT );
                                act( "{Y$c suddenly attacks you!{x", ch, NULL, victim, TO_VICT );
                        }
                        else
                        {
                                act( "{Y$c rears and strikes at $C!{x", ch, NULL, victim, TO_NOTVICT );
                                act( "{Y$c rears and strikes at you!{x", ch, NULL, victim, TO_VICT );
                        }
                }

                multi_hit(ch, victim, TYPE_UNDEFINED);
        }

        sprintf (last_function, "leaving violence_update");
}


/*
 * Do one group of attacks.
 */
void multi_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
        int chance;


        if (!IS_NPC(ch))
                ch->pcdata->rounds++;

        /* For ACT_NO_FIGHT mobs -- Owl 3/2/24 */

        if ( IS_NPC(ch)
        &&   IS_SET(ch->act, ACT_NO_FIGHT ) )
        {
                return;
        }


/*
 * These 2 functions are at the top, as I'm about to put a check for dazed things
 * If you are dazed you can't attack, BUT things you have in the room can still go off
 * DOTs and pulse objects - Brutus
 *
 */

       if (IS_AFFECTED(victim, AFF_DOT))
        {
                AFFECT_DATA *paf;
                for ( paf = victim->affected; paf; paf = paf->next )
                {
                        if ( paf->deleted )
                                continue;

                        if( paf->bitvector == AFF_DOT )
                        {
                                /* A bit brutal/good if it goes off every combat round 29/9/24 --Owl */

                                if (number_percent() < DOT_FREQ)
                                {
                                    damage(victim, victim, paf->modifier, paf->type, FALSE);
                                }
                        }
                }
        }

        /* Pulse objects go off every round (mainly runesmiths) */
        if ( !IS_NPC(ch) )
        {
                OBJ_DATA *pulse;
                for (pulse = ch->in_room->contents; pulse; pulse = pulse->next_content)
                {
                        if ( ( IS_OBJ_STAT( pulse, ITEM_RUNE ) ) && ( pulse->item_type == ITEM_COMBAT_PULSE || pulse->item_type == ITEM_DEFENSIVE_PULSE) )
                        {
                                if (skill_table[pulse->value[3]].target == TAR_CHAR_DEFENSIVE )
                                        victim = ch;

                                if (victim)
                                {
                                        if (ch == victim)
                                        {
                                                act("Your $p pulses.", ch, pulse, victim, TO_CHAR);
                                                act("$n's $p pulses.", ch, pulse, victim, TO_NOTVICT);
                                        }
                                        else
                                        {
                                                act("$p pulses and targets $N.", ch, pulse, victim, TO_CHAR);
                                                act("$n's $p pulses and targets you!", ch, pulse, victim, TO_VICT);
                                                act("$n's $p pulses and targets $p.", ch, pulse, victim, TO_NOTVICT);
                                        }
                                }
                                else
                                {
                                        act("You pulse with $p.", ch, pulse, NULL, TO_CHAR);
                                        act("$n pulse with $p.", ch, pulse, NULL, TO_ROOM);
                                }
                                obj_cast_spell(pulse->value[3], pulse->value[0], ch, victim, pulse);
                                if (--pulse->value[2] <= 0)
                                {
                                        act("Your $p explodes into fragments.", ch, pulse, NULL, TO_CHAR);
                                        act("$n's $p explodes into fragments.", ch, pulse, NULL, TO_ROOM);
                                        extract_obj(pulse);
                                }
                        }
                }
        }

/*
 * Everything below here is for players -- Brutus
 * If you're DAZED you can't attack.
 */
        if (IS_AFFECTED(ch, AFF_DAZED))
        {

                act("$n is DAZED, unable to attack or defend themselves.", ch, NULL, NULL, TO_ROOM);
                return;
        }

        /*
         * One attack (Even if PRONE, everything else is 50% less likely.)
         */
        one_hit(ch, victim, dt, FALSE);

        /*
         * Double backstab
         */
        if (dt == gsn_backstab)
        {
                if (IS_NPC(ch))
                {
                        /* Arena bots just a little too strong */
                        if (ch->pIndexData->vnum == BOT_VNUM)
                                return;
                        else
                                chance = ch->level;
                }
                else
                        chance = ch->pcdata->learned[gsn_double_backstab];

                /* If you're PRONE, you're 1/2 as likely to succeed in anything */
                if (IS_AFFECTED(ch, AFF_PRONE))
                        chance /= 2;

                if (number_percent() < chance)
                        one_hit(ch, victim, dt, FALSE);

                return;
        }

        /*
         * Restrict some attack types
         */
        if (ch->fighting != victim
            || dt == gsn_circle
            || dt == gsn_thrust
            || dt == gsn_constrict
            || dt == gsn_suck
            || dt == gsn_snap_shot)
                return;

        /*
         * Multiple attacks for Monks
         */
        if (!IS_NPC(ch) && ch->sub_class == SUB_CLASS_MONK)
        {
                int attacks;

                for (attacks = 2; attacks <= ch->level / 10; attacks++)
                {
                        chance = 3 * ch->level / attacks;

                        /* If you're PRONE, you're 1/2 as likely to succeed in anything */
                        if (IS_AFFECTED(ch, AFF_PRONE))
                                chance /= 2;

                        if (number_percent() < chance)
                        {
                                one_hit(ch, victim, dt, FALSE);

                                if (ch->fighting != victim)
                                        return;
                        }
                }

                return;
        }

        /*
         * Haste spell & haste
         */
        if ((is_affected(ch, gsn_haste)) && !IS_AFFECTED(ch, AFF_PRONE))
                one_hit(ch, victim, dt, FALSE);
        /*
         * Quicken skill
         */
        if ((is_affected(ch, gsn_quicken)) && !IS_AFFECTED(ch, AFF_PRONE))
                one_hit(ch, victim, dt, FALSE);

        /*
         * Bonus attack
         */
        if ((is_affected(ch, gsn_bonus_attack)) && !IS_AFFECTED(ch, AFF_PRONE))
                one_hit(ch, victim, dt, FALSE);

        /*
         * Swiftness check & calculations
         */
        if ( !IS_NPC(ch)
        &&   ch->pcdata->learned[gsn_enhanced_swiftness])
        {
            if ( number_percent() < ( ch->swiftness + ( ch->pcdata->learned[gsn_enhanced_swiftness] / 4 ) + dex_app[ get_curr_dex( ch ) ].toswift ) )
            {
                one_hit(ch, victim, dt, TRUE);
              /*   log_string("This fired for PC w/ES!");
                bug("Swiftness: %d", ( ch->swiftness + ( ch->pcdata->learned[gsn_enhanced_swiftness] / 4 ) )  + dex_app[ get_curr_dex( ch ) ].toswift ); */
            }
        }
        else if ( !IS_NPC(ch)
        &&        ( number_percent() < ( ch->swiftness + dex_app[ get_curr_dex( ch ) ].toswift ) ) )
        {
                one_hit(ch, victim, dt, TRUE);
                /* log_string("This fired for PC w/out ES!!");
                bug("Swiftness: %d", ( ch->swiftness + dex_app[ get_curr_dex( ch ) ].toswift ) ); */
        }
        else if ( IS_NPC(ch)
        &&        ( number_percent() < ch->swiftness ) )
        {
            one_hit(ch, victim, dt, TRUE);
            /* log_string("This fired for NPC!");
            bug("Swiftness: %d", ch->swiftness ); */
        }

        /* for counterbalance - adds another attack.*/

        if ( !IS_NPC(ch)
                && get_eq_char(ch, WEAR_WIELD)
                && ch->pcdata->learned[gsn_counterbalance] > 0
                && !IS_AFFECTED(ch, AFF_PRONE))
        {
                OBJ_DATA *wield;
                AFFECT_DATA *paf;
                wield = get_eq_char(ch, WEAR_WIELD);
                for ( paf = wield->affected; paf; paf = paf->next )
                {
                        if ( paf->location == APPLY_BALANCE )
                        {
                                if (number_percent() < paf->modifier)
                                        one_hit(ch, victim, gsn_counterbalance, FALSE);
                        }
                }
        }

        /*
         * Multiple attacks for shifter forms
         */
        if (ch->form == FORM_TIGER
            || ch->form == FORM_BEAR
            || ch->form == FORM_HYDRA
            || ch->form == FORM_DRAGON
            || ch->form == FORM_GRIFFIN)
        {
                if ((IS_AFFECTED(ch, AFF_PRONE)) && (number_percent() < 50) )
                        one_hit(ch,victim,dt, FALSE);
                else
                        one_hit(ch,victim,dt, FALSE);
        }

        if (ch->form == FORM_HYDRA
            || ch->form == FORM_DRAGON
            || ch->form == FORM_GRIFFIN)
        {
                if ((IS_AFFECTED(ch, AFF_PRONE)) && (number_percent() < 50) )
                        one_hit(ch,victim,dt, FALSE);
                else
                        one_hit(ch,victim,dt, FALSE);
        }

        /*
         * 2nd attack
         */
        chance = IS_NPC(ch) ? ((ch->level / 2) + ch->level)
                : ((45 + ch->pcdata->learned[gsn_second_attack] / 2)
                   * CAN_DO(ch, gsn_second_attack));

        if (IS_AFFECTED(ch, AFF_SLOW))
        {
                chance /= 2;
        }

        if (number_percent() < chance)
        {
                one_hit(ch, victim, dt, FALSE);

                if (ch->fighting != victim)
                        return;
        }
        else if (!IS_NPC(ch))
                return;

        /*
         * 3rd attack
         */
        chance = IS_NPC(ch) ? ch->level
                : ((35 + ch->pcdata->learned[gsn_third_attack] / 2)
                   * CAN_DO(ch, gsn_third_attack));

        if (IS_AFFECTED(ch, AFF_SLOW))
        {
                chance /= 2;
        }

        /* If you're PRONE, don't get this attack */
        if ((number_percent() < chance) && (!IS_AFFECTED(ch, AFF_PRONE)))
        {
                one_hit(ch, victim, dt, FALSE);

                if (ch->fighting != victim)
                        return;
        }
        else if (!IS_NPC(ch))
                return;

        /*
         * 4th attack
         */
        chance = IS_NPC(ch) ? ch->level
                : ((25 + ch->pcdata->learned[gsn_fourth_attack] / 2)
                   * CAN_DO(ch, gsn_fourth_attack));

        if (IS_AFFECTED(ch, AFF_SLOW))
        {
                chance /= 2;
        }

        /* If you're PRONE, don't get this attack */
       if ((number_percent() < chance) && (!IS_AFFECTED(ch, AFF_PRONE)))        {
                one_hit(ch, victim, dt, FALSE);

                if (ch->fighting != victim)
                        return;
        }

        if (!IS_NPC(ch))
                return;

        /*
         * 5th attack for mobiles
         */
        if ((ch->level < 20) || (IS_AFFECTED(ch, AFF_PRONE)) )
                return;

        chance = ch->level;

        if (IS_AFFECTED(ch, AFF_SLOW))
        {
                chance /= 2;
        }

        if (number_percent() < chance)
                one_hit(ch, victim, dt, FALSE);

        return;
}


/*
 *  Attack with primary and/or secondary weapon.
 */
bool one_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool haste)
{
        OBJ_DATA *wield;
        char      buf [ MAX_STRING_LENGTH ];
        int       victim_ac;
        int       thac0;
        int       thac0_00;
        int       thac0_47;
        int       dam;
        int       diceroll;
        int       chance;
        int       weapon_pos;
        int       count;
        bool      poison;
        bool      hit = FALSE;


        for (weapon_pos = WEAR_WIELD, count = 0;
             count < 2;
             weapon_pos = WEAR_DUAL, count++)
        {

                /* For ACT_NO_FIGHT mobs -- Owl 3/2/24 */

                if (IS_NPC(ch) && (IS_SET(ch->act, ACT_NO_FIGHT)))
                {
                        break;
                }
                /*
                 * Can't beat a dead char!
                 * Guard against weird room-leavings.
                 */
                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        break;

                if (ch->position == POS_DEAD)
                        break;

                if (haste && !ch->gag && !IS_AFFECTED(ch, AFF_ARM_TRAUMA))
                        send_to_char("<15>*SWIFT ATTACK*<0> ", ch);
                /*
                 * Check for secondary attack
                 */
                if (weapon_pos == WEAR_DUAL)
                {
                        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
                        {
                                if (!ch->gag)
                                {
                                        send_to_char("<6>Arm trauma prevents you from dual-wielding.<0>\n\r", ch);
                                }
                                break;
                        }
                        /* Not these dam types */
                        if (dt == gsn_circle
                            || dt == gsn_second_circle
                            || dt == gsn_suck
                            || dt == gsn_lunge
                            || dt == gsn_backstab
                            || dt == gsn_thrust
                            || dt == gsn_shoot
                            || dt == gsn_dive
                            || dt == gsn_joust
                            || dt == gsn_riposte
                            || dt == gsn_snap_shot)
                                break;

                        /* Do we get another attack? */
                        if (!get_eq_char(ch, WEAR_DUAL))
                        {
                                if (IS_NPC(ch))
                                        break;

                                if (!CAN_DO(ch, gsn_dual)
                                    && !CAN_DO(ch, gsn_pugalism))
                                        break;

                                if (get_eq_char(ch, WEAR_SHIELD))
                                        break;
                        }

                        /* Hack for shifters */
                        if (ch->form
                            && ch->form != FORM_TIGER
                            && ch->form != FORM_WOLF
                            && ch->form != FORM_DIREWOLF)
                                break;

                        /*
                         * Dual % should matter, so should dex - Shade Oct 02
                         */

                        chance = 90;

                        if (!IS_NPC(ch))
                        {
                                if (CAN_DO(ch, gsn_dual))
                                        chance *= ch->pcdata->learned[gsn_dual];
                                else if (CAN_DO(ch, gsn_pugalism))
                                        chance *= ch->pcdata->learned[gsn_pugalism];
                                else
                                        chance *= 100; /* shifters */
                        }

                        if (!IS_NPC(ch))
                        {
                                /* NPCs don't have dex and don't have %s boosted above -- Owl 23/8/22 */
                                chance /= 100;
                                chance += (get_curr_dex(ch) - 25);
                        }

                        if (chance < number_percent())
                        {
                                break;
                        }

                }

                /*
                 * Figure out the type of damage message.
                 */
                poison = FALSE;
                wield = get_eq_char(ch, weapon_pos);

                if (dt == TYPE_UNDEFINED || weapon_pos == WEAR_DUAL)
                {
                        dt = TYPE_HIT;

                        if (wield && wield->item_type == ITEM_WEAPON)
                                dt += wield->value[3];

                        if (wield && IS_SET(wield->extra_flags, ITEM_POISONED))
                                poison = TRUE;
                }

                /*
                 * Calculate to-hit-armor-class-0 versus armor.
                 */
                if (IS_NPC(ch))
                {
                        thac0_00 =  20;
                        thac0_47 = -10;
                }
                else
                {
                        thac0_00 = class_table[ch->class].thac0_00;
                        thac0_47 = class_table[ch->class].thac0_47;
                }

                thac0 = interpolate(ch->level, thac0_00, thac0_47)
                        - GET_HITROLL(ch);

                victim_ac = UMAX(-15, GET_AC(victim) / 10);

                if (GET_AC(victim) < -150)
                        victim_ac += ((GET_AC(victim) + 150) /30);

                if (!can_see(ch, victim))
                        victim_ac -= 4;

                /*
                 * The moment of excitement!
                 */
                while ((diceroll = number_bits(5)) >= 20) ;

                if (diceroll == 0
                    || (diceroll != 19
                        && diceroll < thac0 - victim_ac
                        && victim->position > POS_SLEEPING))
                {
                        /* Miss. */
                        damage(ch, victim, 0, dt, poison);
                        tail_chain();

                        if (!IS_NPC (ch) && CAN_DO(ch, gsn_second_attack))
                        {
                                chance = 100 - ch->pcdata->learned[gsn_second_attack];
                                if ((ch->pcdata->learned[gsn_second_attack] < 99)
                                    && (number_percent() < chance)
                                    && (number_percent() < 2))
                                {
                                        ch->pcdata->learned[gsn_second_attack] += 1;
                                        send_to_char("You sense your skill in second attack increasing...\n\r", ch);
                                }
                        }
                        continue;
                }

                /*
                 * Hit.
                 * Calc damage.
                 */
                hit = TRUE;

                if (IS_NPC(ch))
                {
                        dam = number_range(ch->level / 2, ch->level * 3 / 2);

                        if (ch->level < 30)
                                dam += ch->level / 4;

                        else if (ch->level < 65)
                                dam += ch->level / 2;

                        else
                                dam += ch->level * 3 / 4;

                        if (wield)
                                dam += dam / 2;
                }
                else
                {
                        if (wield)
                                dam = number_range(wield->value[1], wield->value[2]);
                        else
                        {
                                dam = number_range(1, 4);

                                /* Give low level Brawlers a boost, won't make any difference at high levels */
                                if (ch->class == CLASS_BRAWLER)
                                        dam += number_range(4, 8);
                        }

                        if (wield && dam > MAX_DAMAGE)
                        {
                                sprintf(buf, "One_hit dam range > %d from %d to %d",
                                        MAX_DAMAGE,
                                        wield->value[1],
                                        wield->value[2] );
                                bug(buf, 0);
                        }
                }

                /*
                 * Bonuses.
                 */
                dam += GET_DAMROLL(ch);

                if (wield
                    && IS_SET(wield->extra_flags, ITEM_POISONED)
                    && (IS_NPC(victim)
                        || number_percent() > victim->pcdata->learned[gsn_resist_toxin]
                        || is_affected(victim, gsn_bonus_exotic)))
                {
                        dam += dam / 4;
                }

                if (wield
                    && IS_SET(wield->ego_flags, EGO_ITEM_FIREBRAND))
                {
                        chance = 21;
                        if (chance > number_percent())
                        dam += dam / 3;

                }

                /* adds to the meter for smithys */
                if ( wield && (IS_SET(wield->ego_flags, EGO_ITEM_EMPOWERED)) && (ch->pcdata->meter < 100) )
                {
                        if ( (number_percent() < ch->pcdata->learned[gsn_empower]) && (victim->level +5 >= ch->level))
                        {
                                ch->pcdata->meter += (number_percent() /15 );
                                if (ch->pcdata->meter >= 100)
                                        send_to_char("<556><352><196>You are fully E M P O W E R E D.<0>\n\r",ch);
                        }

                }

                if ( wield && (IS_SET(wield->ego_flags, EGO_ITEM_ENGRAVED))
                        && (ch->pcdata->dam_meter < ch->damage_enhancement)
                        &&  (victim->level +5 >= ch->level) )
                {
                                ch->pcdata->dam_meter += (number_percent() /35 );
                                if (ch->pcdata->dam_meter >= ch->damage_enhancement)
                                        send_to_char("<556><352><196>Your E N G R A V I N G S are maxed.<0>\n\r",ch);
                }

                if (!IS_NPC(ch)
                    && ch->pcdata->learned[gsn_enhanced_damage] > 0
                    && ch->form != FORM_BEAR
                    && ch->form != FORM_DRAGON)
                        dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 200;

                /* adds the damage_enhancement for smithys */
                if (!IS_NPC(ch)
                    && ch->pcdata->learned[gsn_engrave] > 0 )
                        dam += dam * ch->damage_enhancement / 100;

                if (!IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_hit] > 0)
                        dam += dam * ch->pcdata->learned[gsn_enhanced_hit] / 400;

                if (!IS_AWAKE(victim))
                        dam *= 2;
                else if (IS_RESTING(victim))
                        dam += dam / 4;

                if (IS_AFFECTED(victim, AFF_PRONE))
                        dam += dam / 4;

                if (IS_AFFECTED(victim, AFF_DAZED))
                        dam += dam / 2;

                if (ch->form)
                {
                        if (ch->form == FORM_DRAGON)
                                dam *= 2;

                        else if ((ch->form == FORM_WOLF || ch->form == FORM_DIREWOLF)
                                  && IS_FULL_MOON)
                                dam += dam / 4;

                        else if (ch->form == FORM_BEAR)
                                dam += dam / 2;
                }

                /*
                 * Damage bonuses for certain damage types
                 */
                if (dt != TYPE_UNDEFINED && dt < TYPE_HIT)
                {
                        if (dt == gsn_backstab)
                                dam *= 3 + (ch->level / 15);

                        else if (dt == gsn_joust || dt == gsn_dive )
                                dam *= 2 + (ch->level / 20);

                        else if (dt == gsn_lunge)
                        {
                                dam += dam / 2;

                                /*
                                 * Shade 10.5.22 - make high / low blood have an impact
                                 */

                                if (ch->rage > (ch->max_rage * 3 / 4))
                                        dam += dam / 10;
                                else if (ch->rage < ch->max_rage / 4)
                                        dam -= dam / 10;
                        }

                        else if (dt == gsn_shoot)
                        {
                                dam *= 2;
                                if (IS_NPC(ch))
                                {
                                    dam += dam * ch->level / 300;
                                }
                                else {
                                    dam += dam * ch->pcdata->learned[gsn_accuracy] / 300;
                                }
                        }

                        else if (dt == gsn_circle || dt == gsn_constrict || dt == gsn_thrust)
                                dam += dam / 2;

                        else if (dt == gsn_snap_shot)
                        {
                                dam += dam / 4;
                                dam += dam * ch->pcdata->learned[gsn_accuracy] / 300;
                        }

                        else if (dt == gsn_suck)
                        {
                                dam += dam / 2;

                                /*
                                 * Shade 10.5.22 - make high / low blood have an impact
                                 */

                                if (ch->rage > (ch->max_rage * 3 / 4))
                                        dam += dam / 10;
                                else if (ch->rage < ch->max_rage / 4)
                                        dam -= dam / 10;
                        }

                        else if (dt == gsn_second_circle)
                                dam *= 2;

                        else if (dt == gsn_grapple || dt == gsn_smash)
                                dam += dam / 4;

                        else if (dt == gsn_riposte)
                                dam -= dam / 2;
                }

                if (dam <= 0)
                        dam = 1;

                damage(ch, victim, dam, dt, poison);
                tail_chain();
        }

        return hit;
}


/*
 * 22.3.22 - Shade group support bonus
 */

void check_group_bonus(CHAR_DATA *ch)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        for (vch = ch->in_room->people; vch; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (ch == vch)
                        continue;

                if (vch->deleted)
                        continue;

                if (IS_NPC(vch))
                        continue;

                if (is_same_group(vch, ch) && !IS_NPC(ch))
                {
                        ch->pcdata->group_support_bonus += 1;
                        break;
                }

        }

}

/*
 * Inflict damage from a hit.
 */
void death_penalty (CHAR_DATA *ch, CHAR_DATA *victim)
{
        char buf[MAX_STRING_LENGTH];
        int loss;

        if (!IS_NPC(victim))
        {
                if (!IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        if (!IS_NPC(ch))
                                loss = (level_table[victim->level-1].exp_total - victim->exp) / 4;
                        else
                                loss = (level_table[victim->level-1].exp_total - victim->exp) / 2;

                        /*
                         *  Can only the amount of exp you need to level; Gez & Shade 2000
                         */
                        if (victim->pcdata->level_xp_loss
                            < (level_table[victim->level].exp_total
                               - level_table[victim->level-1].exp_total))
                        {
                                gain_exp(victim, loss);
                                victim->pcdata->level_xp_loss -= loss;
                        }
                }

                if (victim->pcdata->fame > 1 && !IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        victim->pcdata->fame /= 2;
                        check_fame_table(victim);
                }

                /*
                 * Shade Apr 2022
                 */

                if (IS_NPC(ch) && (!IS_SET(ch->act, ACT_WIZINVIS_MOB)))
                {
                        check_infamy_table(ch);
                }

                if (ch != victim)
                {
                        if (victim->pcdata->bounty
                            && ch->clan
                            && victim->clan
                            && victim->clan != ch->clan)
                        {
                                sprintf(buf, "{WYou receive {C%d{W platinum coins as reward for the death of %s!{x\n\r"
                                        "The money is placed in your bank account.\n\r",
                                        victim->pcdata->bounty/1000,
                                        victim->name);
                                send_to_char(buf,ch);

                                sprintf(buf, "%s collects %d platinum coins for the murder of %s.",
                                        ch->name,
                                        victim->pcdata->bounty/1000,
                                        victim->name);
                                do_info(ch, buf);

                                ch->pcdata->bank = UMIN(2000000000,
                                                        ch->pcdata->bank + victim->pcdata->bounty);
                                victim->pcdata->bounty = 0;

                                if (IS_SET( victim->status, PLR_HUNTED))
                                {
                                        REMOVE_BIT(victim->status, PLR_HUNTED);
                                        send_to_char("You are no longer HUNTED.\n\r", victim);
                                }

                                remove_from_wanted_table(victim->name);
                        }
                }
        }
}


/*
 * Damage!
 */
void damage (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool poison)
{
        CHAR_DATA *fighter;
        CHAR_DATA *opponent;
        int count;
        int no_defense = 0;
        OBJ_DATA *turret_unit;
        int reflected_dam = 0;
        bool crit = FALSE;


        if (victim->position == POS_DEAD)
                return;

        if ( IS_NPC(victim)
        &&   IS_SET(victim->act, ACT_INVULNERABLE) )
        {
            act ("$C is {WINVULNERABLE{x and cannot be physically harmed.", ch, NULL, victim, TO_CHAR );
            return;
        }

        if (!IS_NPC(ch))
                ch->pcdata->dam_per_fight += dam;


        if (!IS_NPC(ch)
        && ( ch->position == POS_STANDING )
        && IS_AFFECTED(ch, AFF_BONUS_INITIATE))
        {
            dam += dam / 3;
        }

        /*
         *  Damage inflicted to other
         */
        if (victim != ch)
        {
                check_killer(ch, victim);

                if (victim->position > POS_STUNNED)
                {
                        if (!victim->fighting)
                        {
                                if (victim->position < POS_FIGHTING)
                                        no_defense = 1;

                                set_fighting(victim, ch);
                        }

                        if (!ch->fighting)
                                set_fighting(ch, victim);

                        victim->position = POS_FIGHTING;

                        /*
                         * If victim is charmed, ch might attack victim's master
                         */
                        if (IS_NPC(ch)
                            && IS_NPC(victim)
                            && IS_AFFECTED(victim, AFF_CHARM)
                            && victim->master
                            && victim->master->in_room == ch->in_room
                            && number_bits(3) == 0
                            && !ch->mount)
                        {
                                stop_fighting(ch, FALSE);
                                set_fighting(ch, victim->master);
                                return;
                        }
                }

                /*
                if (ch == victim->master)
                        stop_follower(victim);

                if (IS_AFFECTED(ch, AFF_INVISIBLE))
                {
                        affect_strip(ch, gsn_invis);
                        affect_strip(ch, gsn_mass_invis);
                        REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
                        act ("$c fades into existence.", ch, NULL, NULL, TO_ROOM);
                }

                if (IS_AFFECTED(ch, AFF_HIDE)  && (ch->form != FORM_CHAMELEON))
                {
                        affect_strip(ch, gsn_hide);
                        REMOVE_BIT(ch->affected_by, AFF_HIDE);
                        act ("$c is revealed from a hidden location.",ch,NULL,NULL,TO_ROOM);
                }

                if (IS_AFFECTED(ch, AFF_MEDITATE))
                        REMOVE_BIT(ch->affected_by, AFF_MEDITATE);

                if (!IS_NPC(ch) && (ch->pcdata->tailing))
                        ch->pcdata->tailing = str_dup( "" );

                remove_songs(ch);

                if (IS_AFFECTED(ch, AFF_SNEAK) && ch->form != FORM_CHAMELEON)
                {
                        affect_strip(ch, gsn_sneak);
                        affect_strip(ch, gsn_shadow_form);
                        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
                        act ("$c emerges from the shadows.", ch, NULL, NULL, TO_ROOM);
                }
                 */

                if ( ( IS_AFFECTED (victim, AFF_GLOBE) )
                &&   ( victim->sub_class != SUB_CLASS_INFERNALIST) )
                {
                        if (((ch->alignment - victim->alignment) > 750)
                            || ((ch->alignment - victim->alignment) < -750))
                        {
                                dam -= victim->level;
                        }
                }

                /* Make dark globe work a little differently for Infernalists.  Basically better if
                   you're very evil fighting someone very good, and less good if you're very good fighting
                   someone very evil.  -- Owl 7/3/24 */

                if ( ( IS_AFFECTED (victim, AFF_GLOBE) )
                &&   ( victim->sub_class == SUB_CLASS_INFERNALIST) )
                {
                        if ((ch->alignment - victim->alignment) > 750)
                        {
                            dam -= (victim->level + (victim->level/4));
                        }

                        if ((ch->alignment - victim->alignment) < -750)
                        {
                            dam -= (victim->level - (victim->level/4));
                        }

                }

                if (IS_AFFECTED(victim, AFF_SANCTUARY))
                        dam /= 2;

                /* Figure out (painfully) critical hit stuff */
                if ( !IS_NPC(ch)
                &&    ch->pcdata->learned[gsn_enhanced_critical] )
                {
                        if ( number_percent() < ( ch->crit + ( ch->pcdata->learned[gsn_enhanced_critical] / 4 ) + int_app[ get_curr_int( ch ) ].tocrit ) )
                        {
                            dam *= 2;
                            crit = TRUE;
                            /* log_string("This fired for PC w/EC!");
                            bug("Crit: %d", ( ch->crit + ( ch->pcdata->learned[gsn_enhanced_critical] / 4 ) + int_app[ get_curr_int( ch ) ].tocrit ) ); */
                        }
                }
                else if ( !IS_NPC(ch)
                &&        ( number_percent() < ( ch->crit + int_app[ get_curr_int( ch ) ].tocrit  ) ) )
                {
                    dam *= 2;
                    crit = TRUE;
                    /* log_string("This fired for PC w/out EC!!");
                    bug("Crit: %d", ( ch->crit + int_app[ get_curr_int( ch ) ].tocrit ) ); */
                }
                else if ( IS_NPC(ch)
                &&        ( number_percent() < ch->crit ) )
                {
                    dam *= 2;
                    crit = TRUE;
                    /* log_string("Crit fired for NPC!");
                    bug("Crit: %d", ch->crit ); */
                }

                /* this is to support strengthen, but could be applied for other things - Brutus */
                if (!IS_NPC(victim))
                {
                        dam *=  (100 - victim->damage_mitigation);
                        dam /= 100;
                }
                if (IS_AFFECTED(victim, AFF_PROTECT))
                {
                        if (MOD(ch->alignment - victim->alignment) > 750)
                                dam -= dam / 4;
                }

                if (IS_AFFECTED(victim, AFF_BATTLE_AURA))
                        dam -= dam / 6;

                if (!IS_NPC(victim)
                &&   IS_AFFECTED(victim, AFF_BONUS_RESILIENCE))
                        dam -= dam / 3;

                if (dam > MAX_DAMAGE)
                {
                        char buf[MAX_STRING_LENGTH];

                        if (IS_NPC(ch) && ch->desc)
                                sprintf(buf, "Damage: %d from %s by %s: > %d points with %d dt!",
                                        dam,
                                        ch->name,
                                        ch->desc->original->name,
                                        MAX_DAMAGE,
                                        dt);
                        else
                                sprintf(buf, "Damage: %d from %s: > %d points with %d dt!",
                                        dam,
                                        ch->name,
                                        MAX_DAMAGE,
                                        dt);

                        log_string(buf);
                        dam = MAX_DAMAGE;
                }

                if (dam < 0)
                        dam = 0;

                /* check for disarm, trip, parry, dodge and shield block et al. */
                /* Cant disarm OR trip OR use magic items */
                if (dt >= TYPE_HIT
                    && victim->position > POS_SLEEPING
                    && ch->position > POS_SLEEPING
                    && !IS_AFFECTED(ch, AFF_PRONE)
                    && !IS_AFFECTED(victim, AFF_DAZED))
                {
                        int leveldiff = ch->level - victim->level;

                        /* mob: disarm */
                        if (IS_NPC(ch)
                            && get_eq_char(ch, WEAR_WIELD)
                            && number_percent() < (leveldiff < -5
                                                   ? ch->level / 2
                                                   : UMAX(10, leveldiff))
                            && dam == 0)
                        {
                            if (!IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
                            {
                                disarm(ch, victim);
                            }
                        }

                        /* mob: trip  */
                        if (IS_NPC(ch)
                            && !IS_AFFECTED(victim, AFF_FLYING)
                            && ( ch->level < 5 )
                            && !IS_AFFECTED(ch, AFF_PRONE)
                            && !is_affected(victim, gsn_haste)
                            && !is_affected(victim, gsn_quicken)
                            && !is_affected(victim, gsn_bonus_attack)
                            && !is_affected(victim, gsn_fly)
                            && !is_affected(victim, gsn_levitation)
                            && number_percent() < (leveldiff < -5
                                                   ? ch->level / 2
                                                   : UMAX(20, leveldiff))
                            && dam == 0)
                        {
                                trip(ch, victim);
                        }

                        /* mob: using magic item */
                        if (IS_NPC(ch)
                            && !IS_AFFECTED(ch, AFF_PRONE)
                            && number_percent() < 5
                            && !IS_NPC(victim))
                        {
                                use_magical_item(ch);

                                if (victim->in_room != ch->in_room)
                                        return;
                        }

                        if (dam && !no_defense)
                        {
                                /* Engineer's turrent deployable units come before any character defense */

                                if (check_arrestor_unit(ch, victim, dt))
                                        return;

                                if (check_shield_unit(ch, victim, dt))
                                        return;

                                if (check_shield_block(ch, victim))
                                        return;

                                if (check_parry(ch, victim))
                                {
                                        if (!IS_NPC(victim)
                                            && dt != gsn_riposte
                                            && number_percent() < victim->pcdata->learned[gsn_riposte] / 2)
                                        {
                                                if (victim->gag < 2)
                                                        act ("You strike quickly while $n recovers from $s attack.",
                                                             ch, NULL, victim, TO_VICT);

                                                act ("While recovering from your attack, $N makes a quick strike.",
                                                     ch, NULL, victim, TO_CHAR);

                                                one_hit (victim, ch, gsn_riposte, FALSE);
                                        }

                                        return;
                                }
                                /* cant dodge or use acrobatics while PRONE */
                                if (check_dodge(ch, victim))
                                        return;

                                if (!IS_NPC(victim))
                                {
                                        if (victim->form == FORM_BAT
                                        &&  number_percent() <= UMAX(1, (victim->pcdata->learned[gsn_form_bat] - 5)))
                                        {
                                            if (!IS_NPC(ch) && !ch->gag)
                                                act ("<180>$C flits away from your attack.<0>", ch, NULL, victim, TO_CHAR);

                                            if(!IS_NPC(victim) && !victim->gag)
                                                act ("<181>You flit away from $n's attack.<0>", ch, NULL, victim, TO_VICT);
                                            return;
                                        }

                                        if (check_acrobatics(ch, victim))
                                                return;

                                        if (check_blink(ch, victim))
                                                return;

                                        if (check_aura_of_fear(ch,victim))
                                                return;

                                        if (check_driver_unit(ch,victim))
                                                return;
                                }
                        }
                }
        }

        /* Reflector Unit (Smithy) */
        for (turret_unit = victim->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
        {
                if (turret_unit->item_type == ITEM_REFLECTOR_UNIT)
                {
                        if ((dam > 0) && ( dt < 1000 ) && (ch != victim) && (!IS_NPC(victim)))
                        {
                        reflected_dam = dam;
                        dam = 0;
                        }
                }
                break;
        }
/*
        if (!IS_NPC(victim))
        {
                char       buf [ MAX_STRING_LENGTH ];
                sprintf(buf, "damage incoming %d for %d damage", dt, dam );
                send_to_char(buf, victim);
        }
*/
        if (dt != TYPE_UNDEFINED)
                dam_message(ch, victim, dam, dt, poison, crit);

        /* Reflector Unit (Smithy) */
        if ( (turret_unit) && (reflected_dam > 0) && (IS_SPELL(dt)) && (ch != victim) && (!IS_NPC(victim)))
        {
                dam_message(victim, ch, reflected_dam, gsn_reflector_module, FALSE, FALSE);
                ch->hit -= reflected_dam;

                if (IS_NPC(ch) && IS_SET(ch->act, ACT_UNKILLABLE) && ch->hit < 1)
                        ch->hit = 1;

                if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL && ch->hit < 1)
                        ch->hit = 1;

                act("Your $p <316><102><562>V A P O U R I S E S<563><0> from the overload.<0>", ch, turret_unit, victim, TO_VICT);
                obj_from_room(turret_unit);
        }

        /* Fireshield */
        if (IS_AFFECTED(victim, AFF_FLAMING)
            && dam > 0
            && (dt >= TYPE_HIT || !IS_SPELL(dt))
            && dt != gsn_shoot
            && dt != gsn_kiai
            && dt != gsn_chant_of_pain
            && ch != victim)
        {
                int firedam = dam / 2;

                if (is_affected(ch, gsn_resist_heat))
                        firedam *= 0.8;

                dam_message(victim, ch, firedam, gsn_fireshield, FALSE, FALSE);
                ch->hit -= firedam;

                if (IS_NPC(ch) && IS_SET(ch->act, ACT_UNKILLABLE) && ch->hit < 1)
                        ch->hit = 1;

                if (!IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL && ch->hit < 1)
                        ch->hit = 1;
        }

        /* if we get here add a stack of serrate */
        if ( ( !IS_NPC(ch) && ch->pcdata->learned[gsn_serrate] > 0 ) && (get_eq_char(ch, WEAR_WIELD)) && (dt != gsn_serrate))
        {
                OBJ_DATA        *wield2;
                wield2 = get_eq_char(ch, WEAR_WIELD);

                if (IS_SET(wield2->ego_flags, EGO_ITEM_SERRATED))
                {
                        CHAR_DATA  *vic = (CHAR_DATA *) victim;
                        AFFECT_DATA af;
                        AFFECT_DATA *paf;



                        af.type      = gsn_serrate;
                        af.duration  = 2;

                        /* limiting serrate to 2 rounds of damage after fight */
                        for (paf = victim->affected; paf; paf = paf->next)
                        {
                                if (( paf->type == gsn_serrate ) && ( paf->duration > 2) )
                                {
                                        af.duration = 0;
                                        break;
                                }
                        }

                        af.location  = APPLY_NONE;
                        af.modifier  = 15;
                        af.bitvector = AFF_DOT;
                        affect_join(vic, &af);
                }

        }

        if (!IS_NPC(ch)
        &&   IS_AFFECTED(ch, AFF_BONUS_DAMAGE))
                dam += dam / 3;

        /* hurt the victim, and inform the victim of his new state */
        victim->hit -= dam;

        /* immortality.  imms can be nuked by higher imms :) */
        if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
                if (IS_NPC(ch) || (!IS_NPC(ch) && (ch->level <= victim->level)))
                        victim->hit = 1;

        /* this is for exp (damage bonus) */
        if ((!IS_NPC(ch) && (ch->level - victim->level < 6))
        &&  (!IS_SET(victim->act, ACT_UNKILLABLE)) )
                ch->pcdata->dam_bonus += dam;

        if (is_affected(victim, gsn_berserk) && (victim->position <= POS_STUNNED))
                affect_strip(victim, gsn_berserk);

        /* poisoned weapons */
        if (dam > 0
            && dt > TYPE_HIT
            && poison
            && !saves_spell(ch->level, victim))
        {
                AFFECT_DATA af, *paf;

                if (!IS_NPC(victim)
                    && ( number_percent() < victim->pcdata->learned[gsn_resist_toxin] || is_affected(victim, gsn_bonus_exotic) )
                    && victim->gag < 2)
                {
                        send_to_char("<46>Yo<47>u r<48>es<49>is<48>t t<47>he <46>po<47>is<48>on <49>su<48>rg<47>in<46>g t<47>hr<48>ou<49>gh <48>yo<47>ur <46>ve<47>in<48>s.<0>\n\r", victim);
                }
                else
                {
                        af.type = gsn_poison;
                        af.duration = 1;
                        af.location = APPLY_STR;
                        af.modifier = -2;
                        af.bitvector = AFF_POISON;

                        for (paf = victim->affected; paf; paf = paf->next)
                        {
                                if (paf->type == gsn_poison)
                                {
                                        if (paf->modifier <= -6)
                                                af.modifier = 0;

                                        af.duration = 0;
                                        break;
                                }
                        }
                        affect_join (victim, &af);
                }
        }

        /*
         *  Update the victim's position
         *
         *  Aiee: remember that `ch' can be dead too.  Use a loop to
         *  update both combatants properly, otherwise people killed by
         *  fireshield etc carrying on fighting until hit themselves
         *
         *  Gezhp 2000
         */

        update_pos(ch);
        update_pos(victim);

        for (fighter = ch, opponent = victim, count = 0;
             (!count) || (ch != victim && count == 1);
             fighter = victim, opponent = ch, count++)
        {
                if (!fighter)
                        bug ("damage: `fighter' null during position check", 0);
                if (!opponent)
                        bug ("damage: `opponent' null during position check", 0);
                if (!fighter || !opponent)
                        break;

                /* inform the victim */
                switch (fighter->position)
                {
                    case POS_MORTAL:
                        send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r",
                                     fighter);
                        act ("$c is mortally wounded, and will die soon, if not aided.",
                             fighter, NULL, NULL, TO_ROOM);
                        break;

                    case POS_INCAP:
                        send_to_char("You are incapacitated and will slowly die, if not aided.\n\r",
                                     fighter);
                        act ("$c is incapacitated and will slowly die, if not aided.",
                             fighter, NULL, NULL, TO_ROOM);
                        break;

                    case POS_STUNNED:
                        send_to_char("You are stunned, but will probably recover.\n\r", fighter);
                        act ("$c is stunned, but will probably recover.", fighter, NULL, NULL, TO_ROOM);
                        break;

                    case POS_DEAD:
                        send_to_char("<196>You have been KILLED!!<0>\n\r\n\r", fighter);
                        send_to_char(purgatory_message, fighter);
                        if (IS_NPC(fighter) && ( IS_INORGANIC(fighter) || IS_SET(fighter->act, ACT_OBJECT) ) )
                        {
                            act ("$c has been DESTROYED!!", fighter, NULL, NULL, TO_ROOM);
                        }
                        else {
                            act ("$c is DEAD!!", fighter, NULL, NULL, TO_ROOM);
                        }
                        break;

                    default:
                        if (!IS_NPC(fighter) && fighter->gag > 1)
                                break;
                        if ((dam > fighter->max_hit / 4) && fighter == victim)
                                send_to_char("<15>That really did HURT!<0>\n\r", fighter);
                        if ((fighter->hit < fighter->max_hit / 4) && fighter == victim)
                                send_to_char("<15>You sure are <0><160>BLEEDING!<0>\n\r", fighter);

                } /* Close switch */

                /* sleep spells and extremely wounded folks */
                if ((is_affected(fighter, gsn_sleep) || fighter->hit < 1)
                    && fighter->fighting)
                        stop_fighting(fighter, FALSE);

                /* payoff for killing things */
                if (fighter->position == POS_DEAD)
                {
                        /* If a mob you're grouped with killsteals for you, you get the reward */
                        if ( ( IS_NPC(opponent) && opponent->master)
                        && ( opponent->master->sub_class == SUB_CLASS_WITCH
                          || opponent->master->sub_class == SUB_CLASS_INFERNALIST
                          || opponent->master->sub_class == SUB_CLASS_NECROMANCER
                          || opponent->master->sub_class == SUB_CLASS_KNIGHT
                          || ( ( opponent->master->class == CLASS_SHAPE_SHIFTER )
                            && ( opponent->master->sub_class == 0 ) )
                          || opponent->master->sub_class == SUB_CLASS_WEREWOLF ) )
                        {
                                group_gain(opponent->master, fighter, TRUE);
                        }
                        else
                        {
                                group_gain(opponent, fighter, FALSE);
                        }

                        if (IS_NPC(fighter))
                        {
                                sprintf (log_buf, "damage: mob %d (%s) POS_DEAD",
                                         fighter->pIndexData->vnum, fighter->short_descr);
                                log_string (log_buf);
                        }

                        check_player_death (opponent, fighter);

                        /* Straight to the Reaper after Arena death; no penalties */
                        if (!IS_NPC(fighter)
                            && IS_SET(fighter->in_room->room_flags, ROOM_PLAYER_KILLER))
                        {
                                raw_kill(opponent, fighter, TRUE);
                                if ( fighter->fighting )
                                stop_fighting( fighter, TRUE );
                                char_from_room(fighter);
                                char_to_room(fighter, get_room_index(ROOM_VNUM_GRAVEYARD_A));
                        }

                        /* otherwise death penalty */
                        else
                        {
                                death_penalty (opponent, fighter);
                                death_cry(fighter);
                                raw_kill (opponent, fighter, TRUE);
                                check_autoloot (opponent, fighter);
                        }
                }
        }
        /* End fighter/opponent loop */

        /* return on a self-kill */
        if (victim == ch)
                return;

        /* Tournament stuff */
        if (tournament_action_illegal(victim, TOURNAMENT_OPTION_NOWIMPY))
                return;

        /* take care of link dead people */
        if (!IS_NPC(victim) && !victim->desc)
        {
                if (number_range(0, victim->wait) == 0)
                {
                        do_recall(victim, "");
                        return;
                }
        }

        /* wimp out? */

        if (has_ego_item_effect(victim, EGO_ITEM_BATTLE_TERROR)
            && !saves_spell(victim->level, victim)
            && (victim->level > LEVEL_HERO || !victim->wait)
            && !number_bits(3))
        {
                act("$c's eyes fill with terror!", victim, NULL, NULL, TO_ROOM);

                if (number_bits(1))
                {
                        send_to_char("<11>You are terrified!  You must flee!<0>\n\r", victim);
                        do_flee(victim, "");
                }
                else
                {
                        send_to_char("<11>You are terrified, you cannot fight!<0>\n\r", victim);
                        WAIT_STATE(victim, PULSE_VIOLENCE);
                }

                return;
        }

        if (IS_NPC(victim)
            && dam > 0
            && !victim->rider
            && victim->wait == 0
            && IS_SET(victim->act, ACT_WIMPY)
            && number_bits(1) == 0
            && (victim->hit < victim->max_hit / 2
                || (IS_AFFECTED(victim, AFF_CHARM) && victim->master
                    && victim->master->in_room != victim->in_room)))
        {
                do_flee(victim, "");
        }

        if (!IS_NPC(victim)
            && victim->hit > 0
            && victim->hit <= victim->wimpy
            && victim->wait == 0)
        {
                do_flee(victim, "Wimpy");
        }

        tail_chain();
        return;
}


/*
 *  Chat help when attacked by another player
 *  Shade & Gezhp Jan 2000
 */
void chat_killer(CHAR_DATA *ch, CHAR_DATA *victim)
{
        DESCRIPTOR_DATA*        d;
        char                    buf [MAX_STRING_LENGTH];
        char                    chat_buf [MAX_STRING_LENGTH];
        CHAR_DATA*              och;
        CHAR_DATA*              vch;
        int                     position;

        if(ch == victim || victim->fighting == ch)
                return;

        sprintf(buf, "PKILL: %s (level %d) has attacked %s (level %d)",
                ch->name, ch->level, victim->name, victim->level);

        if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                strcat (buf, " [Arena]");
        else
                strcat (buf, " [Pkill]");

        log_string(buf);

        if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                return;

        sprintf(chat_buf, "%s: 'HELP!  I'm being attacked by %s!'", victim->name, ch->name);

        for (d = descriptor_list; d; d = d->next)
        {
                och = d->original ? d->original : d->character;
                vch = d->character;

                if (d->connected == CON_PLAYING
                    && vch != victim
                    && !IS_AFFECTED(och, AFF_NON_CORPOREAL)
                    && !och->silent_mode)
                {
                        position = vch->position;
                        vch->position = POS_STANDING;

                        sprintf(buf, "%s%s$R",
                                color_table_8bit[get_colour_index_by_code(vch->colors[COLOR_GOSSIP])].act_code,
                                chat_buf);

                        act(buf, ch, NULL, vch, TO_VICT);
                        vch->position = position;
                }
        }

        if (ch->pcdata->deity_patron > -1 && !IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_PEACEFUL, 120);
                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_LAWFUL, 60);
                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_WARLIKE, -10);
                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_DEATH, -10);

                if (deity_info_table[ch->pcdata->deity_patron].type == DEITY_TYPE_CHAOTIC)
                {
                        if (victim->pcdata->deity_patron == ch->pcdata->deity_patron)
                                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_CHAOTIC, -30);
                        else
                                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_CHAOTIC, -15);
                }

                /* if not in own hq test to be added */
                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_PROTECTIVE, 90);
        }
}



/*
 *  Check if a fight can legally start
 */
bool is_safe (CHAR_DATA *ch, CHAR_DATA *victim)
{
        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You can't attack them in your current form.\n\r", ch);
                return TRUE;
        }

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("You can't attack them in their current form.\n\r", ch);
                return TRUE;
        }

        if (IS_NPC(victim) && victim->rider)
        {
                if ( ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(victim->in_room->area->area_flags, AREA_FLAG_SAFE) )
                    && ( !IS_IMMORTAL( ch ) ) )
                {
                        send_to_char("Not in a place of sanctuary.\n\r", ch);
                        return TRUE;
                }

                if (!in_pkill_range(ch, victim->rider) && !IS_IMMORTAL( ch ))
                {
                        send_to_char("They are not in your pkilling range.\n\r", ch);
                        return TRUE;
                }
        }

        if (IS_NPC(ch) || IS_NPC(victim))
                return FALSE;

        /*
         * Fight is a pfight
         */

        if (IS_SET(victim->status, PLR_KILLER))
        {
                chat_killer(ch, victim);
                return FALSE;
        }

        if (IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                chat_killer(ch, victim);
                return FALSE;
        }

        if (victim->in_room->area->low_level == -4
            && victim->in_room->area->high_level == -4
            && victim->level > 50 && ch->level > 50)
        {
                chat_killer(ch, victim);
                return FALSE;
        }

        if ( ( IS_SET(victim->in_room->room_flags, ROOM_SAFE)
            || IS_SET(victim->in_room->area->area_flags, AREA_FLAG_SAFE) )
            && ( !IS_IMMORTAL( ch ) ) )
        {
                send_to_char("Not in a place of sanctuary.\n\r", ch);
                return TRUE;
        }

        if (!in_pkill_range(ch, victim) && !IS_IMMORTAL( ch ))
        {
                send_to_char("They are not in your pkilling range.\n\r",ch);
                return TRUE;
        }

        chat_killer(ch, victim);
        return FALSE;
}


/*
 *  See if an attack justifies a KILLER flag
 */
void check_killer (CHAR_DATA *ch, CHAR_DATA *victim)
{
        /*
         *  KILLER flag is set if:
         *  a) player murders another player, and
         *  b) victim is not a thief or a killer, and
         *  c) murder doesn not take place in the arena, and
         *  d) murderer is not already a killer, and
         *  e) victim is not clanned nor ronin, and
         *  f) murderer is not clanned nor ronin
         */

        if (!IS_NPC(victim)
            && !IS_NPC(ch)
            && ch != victim
            && !IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER)
            && !IS_SET(victim->status, PLR_KILLER)
            && !IS_SET(victim->status, PLR_THIEF)
            && !IS_SET(ch->status, PLR_KILLER)
            && (!(ch->clan || IS_SET(ch->status, PLR_RONIN))
                || !(victim->clan || IS_SET(victim->status, PLR_RONIN))))
        {
                SET_BIT(ch->status, PLR_KILLER);
                send_to_char("<88>*** <196>You are now a KILLER!!<0><88> ***<0>\n\r", ch);

                if (ch->pcdata->fame > 0)
                        ch->pcdata->fame = -ch->pcdata->fame / 2;
                else
                        ch->pcdata->fame *= 1.5;

                check_fame_table(ch);
                save_char_obj(ch);
        }
}


/*
 * Check for parry
 */
bool check_parry (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        if (IS_NPC(victim))
        {
                chance = UMIN(50, 2 * victim->level);

                if (!get_eq_char(victim, WEAR_WIELD))
                        chance = chance / 2;
        }
        else
        {
                if (victim->class == CLASS_BRAWLER)
                {
                        chance = victim->pcdata->learned[gsn_pre_empt] / 2;
                }
                else
                {
                        if (!get_eq_char(victim, WEAR_WIELD))
                                return FALSE;

                        chance = victim->pcdata->learned[gsn_parry] / 2;
                }
        }

        if (!IS_NPC(ch)
            && ch->pcdata->learned[gsn_feint] > 0
            && number_percent() <= ch->pcdata->learned[gsn_feint])
                chance -= 10;

        chance += (victim->level - ch->level) * 2;

        if (chance > 66)
                chance = 66;

        if (IS_AFFECTED(victim, AFF_BLIND))
                chance /= 2;

        if (IS_AFFECTED(victim, AFF_ARM_TRAUMA))
                chance /= 2;

        if (number_percent() >= chance)
        {
                if (IS_NPC(victim) || !CAN_DO(victim, gsn_dual_parry) || !get_eq_char(victim, WEAR_DUAL))
                        return FALSE;

                chance = victim->pcdata->learned[gsn_dual_parry] / 2;

                if (!IS_NPC(ch)
                    && ch->pcdata->learned[gsn_feint] > 0
                    && number_percent() <= ch->pcdata->learned[gsn_feint])
                        chance -= 10;

                chance += (victim->level - ch->level) * 2;

                if (chance > 66)
                        chance = 66;

                if (IS_AFFECTED(victim, AFF_BLIND))
                        chance /= 2;

                if (number_percent() >= chance)
                        return FALSE;
        }

        if (victim->class == CLASS_BRAWLER)
        {
                if (!IS_NPC(ch) && !ch->gag)
                        act ("<222>$C pre-empts your attack.<0>", ch, NULL, victim, TO_CHAR);

                if (!IS_NPC(victim) && !victim->gag)
                        act ("<220>You pre-empt $n's attack.<0>",  ch, NULL, victim, TO_VICT);
        }
        else
        {
                if (!IS_NPC(ch) && !ch->gag)
                        act ("<222>$C parries your attack.<0>", ch, NULL, victim, TO_CHAR);

                if (!IS_NPC(victim) && !victim->gag)
                        act ("<220>You parry $n's attack.<0>",  ch, NULL, victim, TO_VICT);
        }

        return TRUE;
}


/*
 * Check for shield block
 */
bool check_shield_block (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        if (IS_NPC(victim))
        {
                if (!get_eq_char(victim, WEAR_SHIELD))
                        return FALSE;

                chance  = UMIN(60, 2 * victim->level);
        }
        else
        {
                if (!get_eq_char(victim, WEAR_SHIELD))
                        return FALSE;

                chance  = victim->pcdata->learned[gsn_shield_block] / 2;
        }

        if (!IS_NPC(ch)
            && ch->pcdata->learned[gsn_feint] > 0
            && number_percent() <= ch->pcdata->learned[gsn_feint])
                chance -= 10;

        chance += (victim->level - ch->level) * 2;

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA) )
                chance /= 2;

        if (chance > 66)
                chance = 66;

        if (number_percent() >= chance)
                return FALSE;

        if(!IS_NPC(victim) && !victim->gag)
                act ("<51>You block $n's attack with your shield.<0>",  ch, NULL, victim, TO_VICT);

        if (!IS_NPC(ch) && !ch->gag)
                act ("<87>Your blow bounces off $N's shield.<0>", ch, NULL, victim, TO_CHAR);

        return TRUE;
}

/*
 * Check for arrestor unit
 */
bool check_arrestor_unit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
        OBJ_DATA *turret_unit;

        for (turret_unit = ch->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
        {
                if (turret_unit->item_type == ITEM_ARRESTOR_UNIT)
                break;
        }

        if (!turret_unit)
                return FALSE;

        if (IS_SPELL(dt) && (!IS_NPC(victim)))
        {
                if(!IS_NPC(victim) && !victim->gag)
                        act ("<51>$n's spell is grounded by your $p, which is destroyed.<0>",  ch, turret_unit, victim, TO_VICT);

                if (!IS_NPC(ch) && !ch->gag)
                        act ("<87>$N's spell is grounded by the $p.<0>", ch, turret_unit, victim, TO_CHAR);

                act("Your $p <316><102><562>M E L T S<563><0> from the overload.<0>", ch, turret_unit, victim, TO_VICT);
                obj_from_room(turret_unit);
                return TRUE;
        }
        return FALSE;
}

bool check_driver_unit (CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *turret_unit;

        for (turret_unit = ch->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
        {
                if (turret_unit->item_type == ITEM_DRIVER_UNIT)
                break;
        }

        if (!turret_unit)
                return FALSE;

        if ((!IS_NPC(victim)))
        {
                if(!IS_NPC(victim) && !victim->gag)
                        act ("<51>Your $p pounds the ground, unbalancing $n.<0>",  ch, turret_unit, victim, TO_VICT);

                if (!IS_NPC(ch) && !ch->gag)
                        act ("<87>$N's attacked is unbalanced by the $p.<0>", ch, turret_unit, victim, TO_CHAR);

                if (--turret_unit->value[0] <= 0)
                {
                        act("Your $p <316><102><562>C R U M P L E S<563><0> from the impacts.<0>", ch, turret_unit, victim, TO_VICT);
                        obj_from_room(turret_unit);
                }
                return TRUE;
        }
        return FALSE;
}

bool check_shield_unit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
        OBJ_DATA *turret_unit;

        for (turret_unit = ch->in_room->contents; turret_unit; turret_unit = turret_unit->next_content)
        {
                if (turret_unit->item_type == ITEM_SHIELD_UNIT)
                break;
        }

        if (!turret_unit)
        return FALSE;

        if ((number_percent() < 50) && (!IS_NPC(victim)))
        {
                if(!IS_NPC(victim) && !victim->gag)
                {
                        act ("<51>$n's attack is blocked by $p.<0>",  ch, turret_unit, victim, TO_VICT);
                        act ("<87>Your attack is blocked by $p.<0>", ch, turret_unit, victim, TO_CHAR);
                }

                if (--turret_unit->value[0] <= 0)
                {
                        act("Your $p <316><102><562>S H A T T E R S<563><0> under the prolonged attack.<0>", ch, turret_unit, victim, TO_VICT);
                        obj_from_room(turret_unit);
                }
                return TRUE;
        }
        return FALSE;
}

/*
 * Check for JasBlink enabled
 */
bool check_blink(CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        if (!victim->pcdata->blink)
                return FALSE;

        chance = victim->pcdata->learned[gsn_blink] / 2;

        if (!chance)
                return FALSE;

        chance += (victim->level - ch->level) * 2;

        if (chance > 66)
                chance = 66;

        if (number_percent() >= chance)
                return FALSE;

        if (victim->mana < 5)
        {
                send_to_char("<117>Your body returns to a more stable state.<0>\n\r", victim);
                victim->pcdata->blink = FALSE;
                return FALSE;
        }

        victim->mana -= 5;

        if (!IS_NPC(ch) && !ch->gag)
                act ("<81>$C shimmers and you miss your attack.<0>", ch, NULL, victim, TO_CHAR);

        if (!victim->gag)
                act ("<117>You fade away before $n's attack.<0>", ch, NULL, victim, TO_VICT);

        return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        if (IS_AFFECTED(victim, AFF_HOLD))
                return FALSE;

        if (IS_AFFECTED(victim, AFF_PRONE))
                return FALSE;

        if (!IS_NPC(victim) && (ch->form == FORM_WOLF || ch->form == FORM_DIREWOLF))
                return FALSE;

        if (IS_NPC(victim))
                chance  = UMIN(60, 2 * victim->level);
        else
                chance  = victim->pcdata->learned[gsn_dodge] / 2;

        if (!IS_NPC(ch)
            && ch->pcdata->learned[gsn_feint] > 0
            && number_percent() <= ch->pcdata->learned[gsn_feint])
                chance -= 10;

        chance += (victim->level - ch->level) * 2;

        if (chance > 66)
                chance = 66;

        if (IS_AFFECTED(victim, AFF_BLIND))
                chance /= 2;

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
                chance /= 2;

        if (number_percent() >= chance)
                return FALSE;

        if (!IS_NPC(ch) && !ch->gag)
                act ("<76>$C dodges your attack.<0>", ch, NULL, victim, TO_CHAR);

        if(!IS_NPC(victim) && !victim->gag)
                act ("<113>You dodge $n's attack.<0>", ch, NULL, victim, TO_VICT);

        return TRUE;
}


/*
 * Acrobatics
 */
bool check_acrobatics (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        if (IS_AFFECTED(victim, AFF_HOLD))
                return FALSE;

        if (IS_AFFECTED(victim, AFF_PRONE))
                return FALSE;

        if (IS_AFFECTED(victim, AFF_BLIND))
                return FALSE;

        chance = victim->pcdata->learned[gsn_acrobatics] / 3;

        if (!chance)
                return FALSE;

        chance += (victim->level - ch->level) * 2;

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA) )
            chance /= 2;

        if ( IS_AFFECTED(ch, AFF_LEG_TRAUMA) )
            chance /= 2;

        if (chance > 50)
                chance = 50;

        if (number_percent() >= chance)
                return FALSE;

        if (!IS_NPC(ch) && !ch->gag)
                act ("<123>$C evades your attack.<0>", ch, NULL, victim, TO_CHAR);

        if (!victim->gag)
                act ("<14>You flip away from $n's attack!<0>", ch, NULL, victim, TO_VICT);

        return TRUE;
}


/*
 * Check aura of fear
 */
bool check_aura_of_fear (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int chance;

        chance = victim->pcdata->learned[gsn_aura_of_fear] / 3;

        if (!chance)
                return FALSE;

        if (IS_NPC(ch) && IS_SET(ch->act, ACT_OBJECT))
        {
                return FALSE;
        }

        chance += (victim->level - ch->level) * 3;

        if (chance > 66)
                chance = 66;

        if (number_percent() >= chance)
                return FALSE;

        if (!IS_NPC(ch) && !ch->gag)
                act ("<129>You cower in fear at the sight of $C!<0>", ch, NULL, victim, TO_CHAR);

        if (!victim->gag)
                act ("<93>You terrify $n, preventing $m from attacking!<0>", ch, NULL, victim, TO_VICT);

        return TRUE;
}


void update_pos (CHAR_DATA *victim)
{

        if (IS_SET(victim->act, ACT_UNKILLABLE))
        {
                return;
        }

        if (victim->hit > 0)
        {
                if (victim->position <= POS_STUNNED)
                        victim->position = POS_STANDING;
                return;
        }

        /*
         * Check if mount has died
         */
        if (victim->rider)
        {
                send_to_char ("Your mount collapses, throwing you off it!\n\r",
                              victim->rider);
                act ("$c collapses, throwing $N off!", victim, NULL,
                     victim->rider, TO_NOTVICT);
                victim->rider->position = POS_RESTING;
                strip_mount (victim->rider);
        }

        /*
         * Check if rider has died
         */
        if (victim->mount)
        {
                act ("$c falls from $s mount.", victim, NULL, NULL, TO_ROOM);
                victim->position = POS_RESTING;
                strip_mount (victim);
        }

        if (IS_NPC(victim))
        {
                victim->position = POS_DEAD;
                return;
        }

        if (victim->hit < -10)
                victim->position = POS_DEAD;

        else if (victim->hit <= -6)
                victim->position = POS_MORTAL;

        else if (victim->hit <= -3)
                victim->position = POS_INCAP;

        else
                victim->position = POS_STUNNED;

        return;
}


/*
 * Start fights.
 */
void set_fighting (CHAR_DATA *ch, CHAR_DATA *victim)
{
        char buf [ MAX_STRING_LENGTH ];

        if (ch->fighting)
        {
                bug("Set_fighting: already fighting", 0);
                sprintf(buf, "...%s attacking %s at %d",
                        (IS_NPC(ch)    ? ch->short_descr     : ch->name),
                        (IS_NPC(victim) ? victim->short_descr : victim->name),
                        victim->in_room->vnum);
                bug(buf , 0);
                return;
        }

        if (IS_AFFECTED(ch, AFF_SLEEP))
                affect_strip(ch, gsn_sleep);

        if (IS_AFFECTED(ch, AFF_MEDITATE))
        {
                REMOVE_BIT(ch->affected_by, AFF_MEDITATE);
                affect_strip(ch, gsn_meditate);
        }

        /*
         * The case we're trying to protect below is having your summoned/charmed mob
         * that you're grouped with attack an NPC for you, which is permitted. Possibly
         * exploitable, revert if so.  --Owl 26/7/22
         */

        if ( ch == victim->master )
        {
                if ( ( !IS_NPC(ch)
                    && !IS_NPC(victim) )
                ||   ( !IS_NPC(ch)
                     && IS_NPC(victim) )
                ||   ( IS_NPC(ch)
                   && !IS_NPC(victim) ) )
                {
                        stop_follower(victim);
                }
        }

        if (IS_AFFECTED(ch, AFF_INVISIBLE))
        {
                affect_strip(ch, gsn_invis);
                affect_strip(ch, gsn_mass_invis);
                REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
                act ("$c fades into existence.", ch, NULL, NULL, TO_ROOM);
        }

        if (!IS_NPC(ch) && (ch->pcdata->tailing))
                ch->pcdata->tailing = str_dup( "" );

        remove_songs(ch);

        if (ch->form != FORM_CHAMELEON)
        {
                if (IS_AFFECTED(ch, AFF_SNEAK)
                    || is_affected(ch, gsn_sneak)
                    || is_affected(ch, gsn_shadow_form))
                {
                        affect_strip(ch, gsn_sneak);
                        affect_strip(ch, gsn_shadow_form);
                        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
                        act ("$c emerges from the shadows.", ch, NULL, NULL, TO_ROOM);
                }

                if (IS_AFFECTED(ch, AFF_HIDE)
                    || is_affected(ch, gsn_hide)
                    || is_affected(ch, gsn_chameleon_power))
                {
                        affect_strip(ch, gsn_hide);
                        affect_strip(ch, gsn_chameleon_power);
                        REMOVE_BIT(ch->affected_by, AFF_HIDE);
                        act ("$c is revealed from a hidden location.",ch, NULL, NULL, TO_ROOM);
                }
        }

        if (!IS_NPC(ch)
            && ch->sub_class == SUB_CLASS_WEREWOLF
            && (ch->rage > 80 || IS_FULL_MOON)
            && !is_affected(ch, gsn_berserk))
        {
                AFFECT_DATA af;
                send_to_char("\n\r{GYour LUST for <88>blood<0> overwhelms your senses... you go BERSERK!{x\n\r\r",ch);

                af.type      = gsn_berserk;
                af.duration  = -1;
                af.location  = APPLY_HITROLL;
                af.modifier  = (ch->level / 4);
                af.bitvector = 0;
                affect_to_char(ch, &af);

                af.location  = APPLY_DAMROLL;
                affect_to_char(ch, &af);

                af.location  = APPLY_AC;
                af.modifier  = ch->level;
                affect_to_char(ch, &af);

                send_to_char("You have gone BERSERK!\n\r", ch);
                act ("$c has gone BERSERK!", ch, NULL, NULL, TO_ROOM);
        }


        ch->fighting = victim;
        ch->position = POS_FIGHTING;
}


/*
 * Stop fights.
 */
void stop_fighting (CHAR_DATA *ch, bool fBoth)
{
        CHAR_DATA *fch;

        if (!IS_NPC(ch))
        {
                ch->pcdata->rounds = 0;
                ch->pcdata->dam_per_fight = 0;
        }

        for (fch = char_list; fch; fch = fch->next)
        {
                if (fch == ch || (fBoth && fch->fighting == ch))
                {
                        fch->fighting   = NULL;
                        fch->position   = POS_STANDING;

                        if (is_affected(fch, gsn_berserk))
                                affect_strip(fch, gsn_berserk);

                        if (is_affected(fch, gsn_coil))
                                affect_strip(fch, gsn_coil);

                        /*
                         * Shade - 15.6.22 belongs here
                         */

                        if (is_affected(fch, gsn_unarmed_combat))
                                affect_strip(fch, gsn_unarmed_combat);


                        update_pos(fch);
                }
        }

        return;
}


/*
 * Make a corpse out of a character.
 */
void make_corpse (CHAR_DATA *ch)
{
        OBJ_DATA        *corpse;
        OBJ_DATA        *obj;
        OBJ_DATA        *obj_next;
        OBJ_DATA        *coins;
        ROOM_INDEX_DATA *location;
        char            *name;
        char             buf [ MAX_STRING_LENGTH ];
        bool             report_eq_falling;

        if (IS_NPC(ch))
        {
                if (ch->pIndexData->vnum == BOT_VNUM)
                        return;

                /*
                 * This longwinded corpse creation routine comes about because
                 * we dont want anything created AFTER a corpse to be placed
                 * INSIDE a corpse.  This had caused crashes from obj_update()
                 * in extract_obj() when the updating list got shifted from
                 * object_list to obj_free.          --- Thelonius (Monk)
                 *
                 * Further altered so that mobs with the BODY_NO_CORPSE form bit set
                 * don't make corpses: all eq falls to the ground.  Gezhp 99.
                 */
                if (IS_SET(ch->act, ACT_OBJECT))
                {
                    corpse = create_object(get_obj_index( OBJ_VNUM_REMAINS ), 0, "common", CREATED_NO_RANDOMISER);
                }
                else {
                    corpse = create_object(get_obj_index( OBJ_VNUM_CORPSE_NPC ), 0, "common", CREATED_NO_RANDOMISER);
                }
                corpse->timer = number_range(10, 20);
                name = ch->short_descr;
                corpse->level = ch->level;

                if ((ch->plat + ch->gold + ch->silver + ch->copper) > 0)
                {
                        coins = create_money(ch->plat, ch->gold, ch->silver, ch->copper);

                        if (MAKES_CORPSE(ch))
                                obj_to_obj(coins, corpse);
                        else
                                obj_to_room(coins, ch->in_room);

                        ch->plat = ch->gold = ch->silver = ch->copper = 0;
                }
        }

        /* pc corpse */
        else
        {
                name            = ch->name;
                corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0, "common", CREATED_NO_RANDOMISER);
                corpse->timer   = number_range(90, 100);

                free_string(corpse->name);
                sprintf(buf, "corpse %s", ch->name);
                corpse->name = str_dup(buf);
        }

        sprintf(buf, corpse->short_descr, name);
        free_string(corpse->short_descr);
        corpse->short_descr = str_dup(buf);

        sprintf(buf, corpse->description, name);
        free_string(corpse->description);
        corpse->description = str_dup(buf);

        report_eq_falling = FALSE;

        for( obj = ch->carrying; obj; obj = obj_next )
        {
                obj_next = obj->next_content;
                if (obj->deleted)
                        continue;
                obj_from_char(obj);

                if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
                        extract_obj(obj);

                else if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
                        extract_obj(obj);

                else if (!IS_NPC(ch) || MAKES_CORPSE(ch))
                        obj_to_obj( obj, corpse );

                else
                {
                        if( report_eq_falling == FALSE )
                        {
                                /* act( "$n's belongings fall to the floor.\n\r", ch, NULL, NULL,
TO_ROOM );
                                report_eq_falling = TRUE; */
                        }
                        obj_to_room( obj, ch->in_room );
                }
        }

        if (IS_NPC(ch))
        {
                if (MAKES_CORPSE(ch))
                        obj_to_room(corpse, ch->in_room);
                else
                        extract_obj(corpse);
        }
        else
        {
                if (ch->level > LEVEL_HERO)
                        obj_to_room(corpse, ch->in_room);

                else if (!(location = get_room_index(ROOM_VNUM_GRAVEYARD_A)))
                {
                        bug("Graveyard A does not exist!", 0);
                        obj_to_room(corpse, ch->in_room);
                }
                else
                        obj_to_room(corpse, location);
        }
}


/*
 * Improved Death_cry contributed by Diavolo.
 * Modified to comply with mobile body_form value.  Gezhp 99.
 */
void death_cry (CHAR_DATA *ch)
{
        ROOM_INDEX_DATA *was_in_room;
        char             msg[MAX_STRING_LENGTH];
        int              body_part_vnum = 0;
        int              door;

        /* Default messages */
        if ((!IS_INORGANIC(ch) && !IS_SET(ch->act, ACT_OBJECT)) || (!IS_NPC(ch)))
        {
            if (!MAKES_CORPSE(ch))
                    strcpy(msg, "$c's form withers and dissolves into nothing.");
            else if (IS_HUGE(ch))
                    strcpy(msg, "$c's huge body collapses before you... DEAD.");
            else
                    strcpy(msg, "$c slumps before you... DEAD.");
        }

        /* Default messages */
        if (( IS_INORGANIC(ch) || IS_SET(ch->act, ACT_OBJECT) ) && (IS_NPC(ch)))
        {
            if (!MAKES_CORPSE(ch))
                    strcpy(msg, "$c withers and dissolves into nothing.");
            else if (IS_HUGE(ch))
                    strcpy(msg, "The huge structure of $n crashes down around you... SHATTERED.");
            else
                    strcpy(msg, "$c falls to pieces in front of you... RUINED.");
        }
        /* Random variations, including bits of bodies being dropped */
        switch (number_bits(4))
        {
            case 0:
                if (!IS_INORGANIC(ch) && MAKES_CORPSE(ch))
                        strcpy(msg, "$c collapses, blood leaking from many horrible wounds.");
                break;

            case 1:
                if (CAN_SPEAK(ch))
                        strcpy(msg, "You hear $n's horrible death cry.");
                break;

            case 2:
                if (!IS_INORGANIC(ch) && MAKES_CORPSE(ch))
                        strcpy(msg, "$c's hot blood splatters your body.");
                break;

            case 3:
                if (HAS_HEAD(ch) && MAKES_CORPSE(ch))
                {
                        strcpy(msg, "$c's head is separated from $s body.");
                        body_part_vnum = OBJ_VNUM_SEVERED_HEAD;
                }
                break;

            case 4:
                if (MAKES_CORPSE(ch) && HAS_HEART(ch))
                {
                        strcpy(msg, "$c's heart is torn from $s chest.");
                        body_part_vnum = OBJ_VNUM_TORN_HEART;
                }
                break;

            case 5:
                if (MAKES_CORPSE(ch) && HAS_ARMS(ch))
                {
                        strcpy(msg, "$c's arm is sliced from $s body.");
                        body_part_vnum = OBJ_VNUM_SLICED_ARM;
                }
                break;

            case 6:
                if (MAKES_CORPSE(ch) && HAS_LEGS(ch))
                {
                        strcpy(msg, "$c's leg is sliced from $s body.");
                        body_part_vnum = OBJ_VNUM_SLICED_LEG;
                }
                break;

            case 7:
                if (MAKES_CORPSE(ch) && HAS_TAIL(ch))
                {
                        strcpy(msg, "$c's tail is sliced from $s body.");
                        body_part_vnum = OBJ_VNUM_SLICED_TAIL;
                }
                break;

            case 8:
                if (MAKES_CORPSE(ch) && HAS_HEAD(ch) && !IS_INORGANIC(ch))
                        strcpy(msg, "$c's head splits apart, revealing $s brain.");
                break;
        }

        act(msg, ch, NULL, NULL, TO_ROOM);

        /* Body parts */
        if (body_part_vnum != 0)
        {
                OBJ_DATA *obj;
                char     *name;
                char      buf[MAX_STRING_LENGTH];

                name = IS_NPC(ch) ? ch->short_descr : ch->name;
                obj = create_object(get_obj_index(body_part_vnum), 0,"common", CREATED_NO_RANDOMISER);
                obj->timer = number_range(4, 7);
                obj->timermax = obj->timer;

                /* Set body_part flag so you can't locate magically */
                SET_BIT(obj->extra_flags, ITEM_BODY_PART);

                /*  Make body parts from inorganic mobs inedible.  */
                if (IS_INORGANIC(ch))
                        obj->item_type = ITEM_TRASH;

                sprintf(buf, obj->short_descr, name);
                free_string(obj->short_descr);
                obj->short_descr = str_dup(buf);

                sprintf(buf, obj->description, name);
                free_string(obj->description);
                obj->description = str_dup(buf);

                obj_to_room(obj, ch->in_room);
        }

        /* Death cry heard in neighbouring locations */
        if (!IS_NPC(ch) || CAN_SPEAK(ch))
        {
                if (IS_NPC(ch))
                        strcpy(msg, "You hear something's death cry.");
                else
                        strcpy(msg, "You hear someone's death cry.");

                was_in_room = ch->in_room;

                for (door = 0; door <= 5; door++)
                {
                        EXIT_DATA *pexit;

                        if ((pexit = was_in_room->exit[door])
                            && pexit->to_room
                            && pexit->to_room != was_in_room)
                        {
                                ch->in_room = pexit->to_room;
                                act(msg, ch, NULL, NULL, TO_ROOM);
                        }
                }

                ch->in_room = was_in_room;
        }
}


void raw_kill (CHAR_DATA *ch, CHAR_DATA *victim, bool corpse)
{
        AFFECT_DATA prayers [100];
        AFFECT_DATA *paf;
        int prayer_count = 0;

        stop_fighting(victim, TRUE);

        /* if the victim has been swallowed and the ch makes no corpse, strip
         victim swallow effects */

        if ( ( IS_AFFECTED(victim, AFF_SWALLOWED) )
        &&   ( !MAKES_CORPSE(ch) ) )
        {
                strip_swallow(victim);
        }

        if (ch != victim)
                mprog_death_trigger(victim);

        if (corpse)
                make_corpse(victim);

        /*
         * Need to remember prayer effects because we want to
         * call reset_char_stats, wipe the slate clean and add them back;
         * reseting stats helps fix occasional stat weirdness (this is a
         * good thing :)
         */
        for (paf = victim->affected; paf; paf = paf->next)
        {
                if (!IS_NPC(victim)
                    && !paf->deleted
                    && effect_is_prayer(paf))
                {
                        prayers[prayer_count++] = *paf;
                }

                affect_remove(victim, paf);
        }

        victim->affected_by = 0;

        if (IS_NPC(victim))
        {
                if (ch->level - victim->level < 11)
                {
                        victim->pIndexData->killed++;
                        kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
                }

                extract_char(victim, TRUE);
                return;
        }

        extract_char(victim, FALSE);
        reset_char_stats(victim);

        while (prayer_count)
                affect_to_char(victim, &prayers[--prayer_count]);

        victim->pcdata->killed += 1;
        save_char_obj(victim);
}

/*
 * GG_MARK
 */

void group_gain (CHAR_DATA *ch, CHAR_DATA *victim, bool mob_called)
{
        CHAR_DATA *gch;
        char       buf[ MAX_STRING_LENGTH ];
        int        members;
        int        npc_members;
        int        pc_members;
        int        grxp;
        int        xp;
        int        tlevel;
        int        level_dif;
        int        fame;
        int        total_xp;
        int        total_message;

        /*
         * Monsters don't get kill xp's or alignment changes.
         * P-killing doesn't help either.
         * Dying of mortal wounds or poison doesn't give xp to anyone!
         * Summoned mobs will not reward xp either.
         */
        if (IS_NPC(ch)
            || !IS_NPC(victim)
            || IS_SET(victim->act, ACT_DIE_IF_MASTER_GONE)
            || IS_SET(victim->act, ACT_NO_EXPERIENCE))
        {
                return;
        }

        tlevel = 0;
        members = 0;
        npc_members = 0;
        pc_members = 0;

        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                if (is_same_group(gch, ch))
                {
                        tlevel += gch->level;
                        members++;

                        if (IS_NPC(gch))
                        {
                                npc_members++;
                        }
                        else {
                                pc_members++;
                        }

                }
        }

        /*
         * If function called by a mob on behalf of ch and there are no npc group members in room,
         * return, as this implies that the player's mobs have killed the victim mob after the player
         * has fled.  --Owl 31/7/22
         */

        if (mob_called)
        {
                if (npc_members == 0)
                {
                        return;
                }
        }

        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                OBJ_DATA *obj;
                OBJ_DATA *obj_next;
                total_xp = 0;
                total_message = 0;

                if (!is_same_group(gch, ch))
                        continue;

                if (IS_NPC(gch))
                        continue;

                /* No fancy grouping tricks thanks! - Shade */

                level_dif = ch->level - gch->level;

                if (level_dif > 11 || level_dif < -11)
                        continue;

                level_dif = victim->level - gch->level;
                fame = 0;
                if (gch->pcdata->rounds > 1)
                        gch->pcdata->rounds = 1;

                /* print out damage per round - Brutus
                sprintf(buf, "Damage Per Round (DPR): %s", get_dpr(gch->pcdata->dam_per_fight/gch->pcdata->rounds));
                send_to_char(buf, gch);
                if (gch->level > 50)
                {
                sprintf(buf, "%d\n\r", gch->pcdata->dam_per_fight/gch->pcdata->rounds);
                send_to_char(buf, gch);
                }
                else {
                sprintf(buf, "\n\r");
                send_to_char(buf, gch);
                }
                gch->pcdata->rounds = 0;
                gch->pcdata->dam_per_fight = 0; */

                /*
                  Test below to stop people exploiting flee when fighting unkillable mobs.
                   -- Owl 1/10/23
                */

                if ( (level_dif > -6 && gch->pcdata->dam_bonus)
                &&    (!IS_SET(victim->act, ACT_UNKILLABLE)) )
                {
                        sprintf(buf, "The damage you caused gains you %d experience.\n\r",
                                gch->pcdata->dam_bonus);
                        send_to_char(buf, gch);
                        gain_exp(gch, gch->pcdata->dam_bonus);
                        total_xp += gch->pcdata->dam_bonus;
                        total_message = 1;
                        gch->pcdata->dam_bonus = 0;
                }

                xp = xp_compute(gch, victim) * gch->level / tlevel;

                if (level_dif < -5)
                        xp = victim->level * 10;

                log_string(ch->name);
                log_string(gch->name);

                if (IS_SET(victim->act, ACT_OBJECT))
                {
                    sprintf(buf, "You receive %d experience points for its destruction.\n\r", xp);
                }
                else {
                    sprintf(buf, "You receive %d experience points for the kill.\n\r", xp);
                }
                send_to_char(buf, gch);
                gch->pcdata->kills++;
                gain_exp(gch, xp);
                total_xp += xp;

                if (!IS_SET(gch->status, PLR_KILLER)
                    && ((IS_SET(victim->act, ACT_IS_FAMOUS) && level_dif > -10)
                        || level_dif > 5)
                    && !IS_SET(victim->act, ACT_LOSE_FAME))
                {
                        fame = UMAX(1, victim->level / (5 * members));

                        if (victim->exp_modifier < 100)
                                fame /= 2;

                        if (!(gch->exp == (level_table[gch->level].exp_total - 1)
                              && !check_questpoints_allow_level_gain(gch, FALSE)))
                        {
                                sprintf(buf, "{YFor your heroic actions, you gain %d fame!{x\n\r", fame);
                                send_to_char(buf, gch);
                                gch->pcdata->fame += fame;
                                check_fame_table(gch);
                        }
                }

                /*
                 * Shouldn't get grouping xp bonus if the number of npcs you're grouped
                 * with are >= the number of PCs in your group.
                 */

                if ( members > 1
                &&   pc_members > npc_members)
                {
                        grxp = (xp / 4) * members;

                        if (fame)
                                grxp *= 1.5;

                        sprintf(buf, "You get a grouping bonus of %d experience points.\n\r", grxp);
                        send_to_char(buf, gch);
                        gain_exp(gch, grxp);
                        total_xp += grxp;

                        /*
                         * Shade 22.3.22
                         *
                         * Grouping support bonus
                         *
                         * The aim is to give a little boost to casters doing defensive and otherwise useful spells but not damage which gives XP.  Help lower level
                         * casters who won't do a lot of damage relative to someone tanking for them.
                         *
                         * group_support_bonus is incremented in most defensive/support spells and some non damaging skills / spells eg Trap, Web, Faerie Fire
                         *
                         */

                        if (gch->pcdata->group_support_bonus > 0)
                        {
                                /*
                                 * Starting point - let's try 33% xp bonus per helpful action
                                 */

                                grxp = (xp / 3) * gch->pcdata->group_support_bonus;

                                /*
                                 * So players will figure out how to abuse this I'm sure so let's cap it
                                 */

                                if (grxp > (2 * xp))
                                {
                                        sprintf(buf, "Capping max group bonus for %s", gch->name);
                                        log_string(buf);
                                        grxp = 2 * xp;
                                }

                                sprintf(buf, "For supporting your group, you gain %d bonus experience points.\n\r", grxp);
                                send_to_char(buf, gch);
                                gain_exp(gch, grxp);
                                total_xp += grxp;

                                gch->pcdata->group_support_bonus = 0;
                        }

                }

                /*
                 * Shade 10.5.22 - add total XP
                 */

                if (total_message)
                {

                        sprintf(buf, "{WYou gained a total of %d experience points!{x\n\r", total_xp);
                        send_to_char(buf, gch);
                        total_xp = 0;
                        total_message = 0;
                }

                if (IS_SET(victim->act, ACT_LOSE_FAME))
                {
                        fame = victim->level / 2;
                        sprintf(buf, "{RFor your cowardly actions, you lose %d fame!{x\n\r", fame);
                        send_to_char(buf, gch);
                        gch->pcdata->fame -= fame;
                        check_fame_table(gch);
                }

                if (gch->sub_class == SUB_CLASS_WEREWOLF)
                {
                        int gain = number_range(2, 5);
                        gch->rage -= UMIN(gain, gch->rage);
                }

                /*
                 * Shade 15.6.22
                 *
                 * This should be in stop_fighting
                 *        affect_strip(gch, gsn_unarmed_combat);
                 * */

                if (gch->pcdata->questmob == victim->pIndexData->vnum)
                {
                        send_to_char("\n\rYou have almost completed your QUEST!\n\r"
                                     "Return to the questmaster before your time runs out!\n\r\n\r", gch);
                        gch->pcdata->questmob = -1;
                }

                if (!IS_SET(gch->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(gch->in_room->area->area_flags, AREA_FLAG_SAFE))
                {
                        if (gch->pcdata->deity_patron > -1)
                        {
                                int reward;

                                if (level_dif < -9)
                                        reward = 0;
                                else if (level_dif < -4)
                                        reward = 2;
                                else if (level_dif < 5)
                                        reward = 4;
                                else if (level_dif < 10)
                                        reward = 6;
                                else
                                        reward = 8;

                                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_PEACEFUL, 5);
                                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_CHAOTIC, -2);
                                SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_WARLIKE, -1 - reward);
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_JUDGEMENTAL, 4 - reward);
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_PROUD, 4 - reward);
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_PROTECTIVE, 4 - reward);
                        }
                }
                else
                {
                        SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_LAWFUL, 40);
                }

                /*
                 *  Check whether incompatibly aligned or poisoned items should be dropped
                 */
                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if (obj->deleted)
                                continue;

                        if (obj->wear_loc == WEAR_NONE)
                                continue;

                        if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
                            || (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
                            || (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch))
                            || (IS_SET(obj->extra_flags, ITEM_POISONED)
                                && (!can_use_poison_weapon(ch))))
                        {
                                act ("{YYou are zapped by $p and drop it.{x", ch, obj, NULL, TO_CHAR);
                                act ("{Y$c is zapped by $p and drops it.{x",   ch, obj, NULL, TO_ROOM);
                                obj_from_char(obj);
                                obj_to_room(obj, ch->in_room);
                        }
                }
        }
}


/*
 * XPC_MARK
 *
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute (CHAR_DATA *gch, CHAR_DATA *victim)
{
        int xp;
        int align_delta;
        int align_mod = 0;
        int extra;
        int level;
        int number;

        xp = (victim->level + 10) * (victim->level + 10);
        xp -= URANGE(-4, gch->level - victim->level, 11) * 3 * (victim->level + 10);
        /* xp *= victim->exp_modifier / 100; */
        xp *= victim->exp_modifier;
        xp /= 100;

        align_delta = gch->alignment - victim->alignment;

        if (align_delta >  500 || align_delta < -500)
        {
                if (IS_NEUTRAL(victim) || IS_NEUTRAL(gch))
                        align_mod = align_delta / 20;
                else
                        align_mod = align_delta / 10;

                if ((IS_GOOD(gch) && IS_EVIL(victim))
                    || (IS_EVIL(gch) && IS_GOOD(victim))
                    || (IS_NEUTRAL(gch) && !IS_NEUTRAL(victim)))
                {
                        xp *= 5 / 4;
                }
        }
        else {
                align_mod = - gch->alignment / 8;
        }
        gch->alignment = URANGE(-1000, gch->alignment + align_mod, 1000);

        /*
         * Adjust for popularity of target:  unless pc
         *   -1/8 for each target over  'par' (down to - 50%)
         *   +1/8 for each target under 'par' (up to + 12.5%)
         */

        if (!IS_NPC(victim))
                return xp / 2;

        level  = URANGE(0, victim->level, MAX_LEVEL - 1);
        number = UMAX(1, kill_table[level].number);
        extra  = victim->pIndexData->killed - kill_table[level].killed / number;

        xp    -= xp * URANGE(-1, extra, 4) / 8;
        xp     = number_range(xp * 0.9, xp * 1.1);
        xp     = UMAX(10 * victim->level, xp);

        return xp;
}



void dam_message (CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool poison, bool crit)
{
        static char * const attack_table [] =
        {
                "hit",          /* 0 */
                "slice",
                "stab",
                "slash",
                "whip",
                "claw",         /* 5 */
                "blast",
                "pound",
                "crush",
                "grep",
                "bite",         /* 10 */
                "pierce",
                "suction",
                "chop",
                "rake",
                "swipe",        /* 15 */
                "sting",
                "scoop",
                "mash",
                "hack"
        };

        const  char         *vs;
        const  char         *vp;
        const  char         *attack;
        char          buf          [ MAX_STRING_LENGTH ];
        char          buf1         [ 256 ];
        char          buf2         [ 256 ];
        char          buf3         [ 256 ];
        char          buf4         [ 256 ];
        char          buf5         [ 256 ];
        char          buf9         [ 256 ];
        char          punct;

        vs = get_damage_string(dam, TRUE);
        vp = get_damage_string(dam, FALSE);

        punct = (dam <= 24) ? '.' : '!';

        if ((crit) && (dam > 0))
                sprintf(buf9, " {W*CRITICAL HIT*{x");
        else {
                *buf9 = '\0';
        }

        if (dt == TYPE_HIT)
        {
                /* Combat gagging level 2 now gags 'misses' -- Owl */

                if ( ( ( ch->gag == 2 )
                    && ( dam > 0 ) ) )
                {
                        sprintf(buf1, "You %s $N%c",       vs, punct);
                }
                else if ( dam > 0 )
                {
                        sprintf(buf1, "You %s $N%c",       vs, punct);
                }
                else if ( ch->gag < 2 )
                {
                        sprintf(buf1, "You %s $N%c",       vs, punct);
                }
                else {
                       *buf1 = '\0';
                }

                strcat( buf1, buf9 );

                /*
                 * Shade 10.5.2022 - make you getting hit stand out more, help when a lot of room spam
                 */

                if (dam > 0)
                        sprintf(buf2, "{W$c %s {Wyou%c{x",       vp, punct);
                else
                        sprintf(buf2, "$n %s you%c",       vp, punct);

                sprintf(buf3, "$n %s $N%c",        vp, punct);
                sprintf(buf4, "You %s yourself%c", vs, punct);
                sprintf(buf5, "$n %s $mself%c",    vp, punct);
        }
        else
        {
                if (dt >= 0 && dt < MAX_SKILL)
                        attack  = skill_table[dt].noun_damage;

                else if (dt >= TYPE_HIT
                         && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]))
                        attack  = attack_table[dt - TYPE_HIT];
                else
                {
                        sprintf(buf, "Dam_message: bad dt %d caused by %s.", dt,
                                ch->name);
                        bug(buf, 0);
                        dt      = TYPE_HIT;
                        attack  = attack_table[0];
                }

                /*
                 * Shade 10.5.2022 - make you getting hit stand out more, help when a lot of room spam
                 */


                if (dt > TYPE_HIT && poison)
                {
                        sprintf(buf1, "Your poisoned %s %s $N%c",  attack, vp, punct);

                        if (dam > 0)
                                sprintf(buf2, "{W$c's poisoned %s %s {x{Wyou%c{x", attack, vp, punct);
                        else
                                sprintf(buf2, "$c's poisoned %s %s you%c", attack, vp, punct);

                        sprintf(buf3, "$c's poisoned %s %s $N%c",  attack, vp, punct);
                        sprintf(buf4, "Your poisoned %s %s you%c", attack, vp, punct);
                        sprintf(buf5, "$c's poisoned %s %s $n%c",  attack, vp, punct);
                }

                else
                {
                        sprintf(buf1, "Your %s %s $N%c",  attack, vp, punct);
                        strcat( buf1, buf9 );

                        if (dam > 0)
                        {
                                sprintf(buf2, "$c's %s %s you%c", attack, vp, punct);
                                                strcat( buf2, buf9 );
                        }
                        else
                        {
                                sprintf(buf2, "$c's %s %s you%c", attack, vp, punct);
                                                strcat( buf2, buf9 );
                        }
                        sprintf(buf3, "$c's %s %s $N%c",  attack, vp, punct);
                        sprintf(buf4, "Your %s %s you%c", attack, vp, punct);
                        sprintf(buf5, "$c's %s %s $n%c",  attack, vp, punct);
                        strcat( buf3, buf9 );
                        strcat( buf4, buf9 );
                        strcat( buf5, buf9 );
                }
        }

        if (victim != ch)
        {
                act (buf1, ch, NULL, victim, TO_CHAR);
                act (buf2, ch, NULL, victim, TO_VICT);
                act (buf3, ch, NULL, victim, TO_NOTVICT);
        }
        else
        {
                act (buf4, ch, NULL, victim, TO_CHAR);
                act (buf5, ch, NULL, victim, TO_ROOM);
        }

        return;
}


/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm (CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *obj;
        int chance;

        if (!(obj = get_eq_char(victim, WEAR_WIELD)))
                return;

        if (!check_blind(ch))
                return;

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from disarming anyone here.<0>\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
        {
                act ("That is part of $N's body!", ch, NULL, victim, TO_CHAR);
                if (!victim->gag)
                        act ("$n attempts to disarm you but your body parts decline to be removed!",
                             ch, NULL, victim, TO_VICT);
                return;
        }

        if (!get_eq_char(ch, WEAR_WIELD) && number_bits(1) == 0)
                return;

        if (!IS_NPC(victim) && CAN_DO(victim, gsn_grip))
        {
                chance = (IS_NPC(victim) ? victim->level : victim->pcdata->learned[gsn_grip]);

                if (number_percent() < chance)
                {
                        act ("$N holds the weapon tight!", ch, NULL, victim, TO_CHAR);
                        act ("$n attempts to disarm you but you grip your weapon tight!",
                             ch, NULL, victim, TO_VICT);
                        return;
                }
        }

        if (!IS_NPC(victim) && IS_SET(obj->ego_flags, EGO_ITEM_CHAINED))
        {
                chance = (IS_NPC(victim) ? victim->level : victim->pcdata->learned[gsn_weaponchain]);

                if (number_percent() < chance)
                {
                        act ("$N holds the weapon tight!", ch, NULL, victim, TO_CHAR);
                        act ("$n attempts to disarm you but your weaponchain snaps your weapon back to your hand!",
                             ch, NULL, victim, TO_VICT);
                        return;
                }
        }

        act ("You {Gdisarm{x $N!",  ch, NULL, victim, TO_CHAR);
        act ("$n {GDISARMS{x you!", ch, NULL, victim, TO_VICT);
        act ("$n {GDISARMS{x $N!",  ch, NULL, victim, TO_NOTVICT);
        arena_commentary("$n disarms $N.", ch, victim);

        if (get_eq_char(victim, WEAR_DUAL))
                obj = get_eq_char(victim, WEAR_DUAL);
        obj_from_char(obj);

        if (IS_NPC(victim))
                obj_to_char(obj, victim);
        else
                obj_to_room(obj, victim->in_room);

        check_group_bonus(ch);

}


/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip (CHAR_DATA *ch, CHAR_DATA *victim)
{
        if (victim->wait == 0)
        {
                act ("You trip $N and $E goes down!", ch, NULL, victim, TO_CHAR);
                act ("$n trips you and you go down!", ch, NULL, victim, TO_VICT);
                act ("$n trips $N and $E goes down!", ch, NULL, victim, TO_NOTVICT);

                WAIT_STATE(ch,     2 * PULSE_VIOLENCE);
                WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
                victim->position = POS_RESTING;
        }

        return;
}

void prone (CHAR_DATA *ch, CHAR_DATA *victim, int sn, int rounds)
{
        if (!IS_AFFECTED(victim, AFF_PRONE))
        {
                AFFECT_DATA af;

                af.type      = sn;
                af.duration  = rounds;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_PRONE;
                affect_to_char(victim,&af);
        }
        return;
}

void daze (CHAR_DATA *ch, CHAR_DATA *victim, int sn, int rounds)
{
        if (!IS_AFFECTED(victim, AFF_DAZED))
        {
                AFFECT_DATA af;

                af.type      = sn;
                af.duration  = rounds;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_DAZED;
                affect_to_char(victim,&af);
                ch->dazed += 1;
        }
        return;
}

void do_kill (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Kill whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("Kill yourself?  Try committing SUICIDE.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("You can't attack them in their current form.\n\r", ch);
                return;
        }

        if (!IS_NPC(victim))
        {
                if (!IS_SET(victim->status, PLR_KILLER)
                    && !IS_SET(victim->status, PLR_THIEF))
                {
                        send_to_char("You must MURDER a player.\n\r", ch);
                        return;
                }
        }
        else
        {
                if (IS_AFFECTED(victim, AFF_CHARM) && victim->master)
                {
                        send_to_char("You must MURDER a charmed creature.\n\r", ch);
                        return;
                }
        }

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
        {
                act ("$N is your beloved master!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char("You do the best you can!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
        check_killer(ch, victim);
        multi_hit(ch, victim, TYPE_UNDEFINED);

        return;
}


void do_murde (CHAR_DATA *ch, char *argument)
{
        send_to_char("If you want to MURDER, spell it out.\n\r", ch);
        return;
}



void do_murder (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH  ];

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Murder whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("Suicide is a mortal sin.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
        {
                act ("$N is your beloved master!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char("You do the best you can!\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
        check_killer(ch, victim);
        arena_commentary("$n has attacked $N!", ch, victim);
        multi_hit(ch, victim, TYPE_UNDEFINED);
}


void do_circle (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        CHAR_DATA *victim;
        char arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_circle))
        {
                send_to_char("You better leave the assassin trade to thieves.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if ( ch->in_room->sector_type == SECT_UNDERWATER
        && ( ch->race != RACE_SAHUAGIN && ch->race != RACE_GRUNG ) )
        {
                send_to_char("You cannot circle underwater without firm ground underneath.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (IS_AFFECTED(ch, AFF_BLIND))
        {
                send_to_char("You can't see anything to circle!\n\r", ch);
                return;
        }

        if (arg[0] != '\0')
        {
                if (!(victim = get_char_room  (ch, arg)))
                {
                        send_to_char("Circle whom?\n\r", ch);
                        return;
                }
        }

        if (victim == ch)
        {
                send_to_char("You can't circle yourself!\n\r", ch);
                return;
        }

        if (!(obj = get_eq_char(ch, WEAR_WIELD)) || !is_piercing_weapon(obj))
        {
                send_to_char("You need to wield a piecing weapon to successfully circle.\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        send_to_char("You attempt to circle around your opponent.\n\r", ch);

        if ((!IS_AWAKE(victim) || !IS_NPC (ch))
            && number_percent () < ch->pcdata->learned[gsn_circle])
        {
                arena_commentary("$n circles and backstabs $N.", ch, victim);

                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                one_hit(ch, victim, gsn_circle, FALSE);

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (number_percent () < ch->pcdata->learned[gsn_second_circle])
                        one_hit(ch, victim, gsn_second_circle, FALSE);
        }
        else
                damage (ch, victim, 0, gsn_circle, FALSE);
}

/* Absolute monster function that is poorly coded and should definitely
   be refactored, but here we are. 28/9/24 --Owl */

void do_target (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [ MAX_INPUT_LENGTH ];
        AFFECT_DATA af;
        int vnum;
        int dot_dam;
        char msg [ MAX_STRING_LENGTH ];

        one_argument (argument, arg);
        victim = ch->fighting;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_target))
        {
                send_to_char("You don't know how to do that.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You must be fighting to do that!\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Target what?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_BLIND))
        {
                send_to_char("You need to be able to see to <10>target<0> something!\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("No targeting yourself, emo.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't <10>target<0> an object!\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_INORGANIC( victim ))
        {
                send_to_char("<10>target<0> is only effective against flesh-and-blood opponents.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot <10>target<0> the non-corporeal.\n\r", ch);
                return;
        }

        if (!IS_NPC(victim))
        {
                /*
                    Has to function somewhat differently vs player, as players don't have
                    BODY_FORM values and can't have body parts that are "disarmed". Should
                    check player race for relevant effects instead.

                    Doing it this way, while pissing hard on DRY, makes it easy to make
                    more subtle differences between vs-PC and vs-NPC later.

                    29/9/24 --Owl
                */

            /* PLAYER VERSUS PLAYER or MOB VERSUS PLAYER STARTS HERE */

            if ( ( !str_cmp( arg, "eye"  ) )
            ||   ( !str_cmp( arg, "eyes" ) ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);

                act ("You <10>target<0> $N's <14>EYES<0> with a strike!", ch, NULL, victim, TO_CHAR);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <14>EYES<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_EYE_TRAUMA))
                        {
                                act ("You successfully attack $N's eyes, and <14>seriously damage them<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the eyes -- they are <14>grievously damaged<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the eyes!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N flush in the eyes!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_eye_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_HITROLL;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = (AFF_BLIND + AFF_EYE_TRAUMA);

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted horrific eye trauma on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "eye" ITEM_BODY_PARTs ch is wearing, whether eye trauma has
                           already been inflicted or not -- fails silently if ch has none. */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 2);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if (  ( !str_cmp( arg, "head"  ) )
            ||         ( !str_cmp( arg, "heads" ) ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <194>HEAD<0> with a strike!\n\r", ch);
                if ( (!IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target]) ) ) ) )
                ||   ( IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <194>HEAD<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_HEAD_TRAUMA))
                        {
                                act ("You successfully strike $N's head, and cause <194>serious brain injury<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the head -- you take <194>critical brain damage<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the head!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the head!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_head_trauma, FALSE);
                                daze(ch, victim, gsn_head_trauma, 3);

                                af.type      = gsn_head_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_WIS;
                                af.modifier  = -(ch->level / 10);
                                af.bitvector = AFF_HEAD_TRAUMA;
                                affect_to_char(victim, &af);

                                af.location  = APPLY_INT;
                                af.modifier  = -(ch->level / 10);
                                af.bitvector = AFF_HEAD_TRAUMA;
                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted major head trauma on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* Small chance of decapitation */

                        if ( !( IS_NPC(victim) && IS_SET(victim->act, ACT_UNKILLABLE) )
                        && ( victim->hit <= ( victim->max_hit / 5 ) )
                        &&      ((!IS_NPC(ch) && ( number_percent () < ( ( ch->pcdata->learned[gsn_target] / 50 ) + 1 ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 50 ) ) ) ) )
                        {
                                act ("{BYou strike $N's head perfectly and it DETACHES!!{x", ch, NULL, victim, TO_CHAR);
                                act ("{B$c's vicious head strike causes $N's head to separate from $S body and fly away!{x",
                                    ch, NULL, victim, TO_NOTVICT);
                                act ("{B$c DECAPITATES you!!{x", ch, NULL, victim, TO_VICT);

                                if (IS_NPC(victim))
                                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                                victim->short_descr);
                                else
                                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                                victim->name);

                                act (msg, ch, NULL, NULL, TO_ROOM);
                                arena_commentary("$c decapitates $N!", ch, victim);

                                vnum = OBJ_VNUM_SEVERED_HEAD;

                                if (vnum != 0)
                                {
                                        OBJ_DATA *obj;
                                        char      *name;
                                        char      buf [ MAX_STRING_LENGTH ];

                                        name = IS_NPC(victim) ? victim->short_descr : victim->name;
                                        obj = create_object(get_obj_index(vnum), 0, "common", CREATED_NO_RANDOMISER);
                                        obj->timer = number_range(4, 7);
                                        obj->timermax = obj->timer;

                                        sprintf(buf, obj->short_descr, name);
                                        free_string(obj->short_descr);
                                        obj->short_descr = str_dup(buf);

                                        sprintf(buf, obj->description, name);
                                        free_string(obj->description);
                                        obj->description = str_dup(buf);

                                        obj_to_room(obj, ch->in_room);
                                }

                                damage(ch,victim,victim->max_hit/2,gsn_decapitate, FALSE);
                                return;
                        }

                        /* chance of removing any "head" ITEM_BODY_PARTs ch is wearing, whether head trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 1);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }

            else if (  ( !str_cmp( arg, "arm"  ) )
            ||         ( !str_cmp( arg, "arms" ) ) )
            {
                if ( ( ( victim->class == CLASS_SHAPE_SHIFTER )
                &&   ( ( victim->form != FORM_NORMAL)
                    && ( victim->form != FORM_DEMON) )  )
                ||   ( ( victim->sub_class == SUB_CLASS_WEREWOLF )
                     &&  ( ( victim->form == FORM_WOLF )
                        || ( victim->form == FORM_DIREWOLF ) ) ) )
                {
                        WAIT_STATE(ch, PULSE_VIOLENCE);
                        act("{W$C doesn't (currently!) have that body part!{x", ch, NULL, victim, TO_CHAR);
                        return;
                }

                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <174>ARM<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <174>ARM<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_ARM_TRAUMA))
                        {
                                act ("You successfully attack $N's arm, and <174>mutilate it beyond recognition<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the arm -- it is <174>mutilated beyond recognition<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the arm!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N solidly in the arm!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_arm_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_ARM_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted critical arm damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 3);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "leg"  ) )
            ||          ( !str_cmp( arg, "legs" ) ) )
                &&    ( victim->race != RACE_YUAN_TI ) )
            {
                if ( ( victim->class == CLASS_SHAPE_SHIFTER )
                &&   ( victim->form == FORM_SNAKE ) )
                {
                        WAIT_STATE(ch, PULSE_VIOLENCE);
                        act("{W$C doesn't (currently!) have that body part!{x", ch, NULL, victim, TO_CHAR);
                        return;
                }

                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <186>LEG<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <186>LEG<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_LEG_TRAUMA))
                        {
                                act ("You successfully attack $N's leg, and <186>brutally cripple it<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the leg -- it is <186>maimed<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the leg!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the leg!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_leg_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_LEG_TRAUMA;

                                affect_to_char(victim, &af);

                                af.location  = APPLY_MOVE;
                                af.modifier  = ( ch->level * -2 );

                                af.bitvector = AFF_LEG_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted severe leg damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "leg" ITEM_BODY_PARTs ch is wearing, whether leg trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 4);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( !str_cmp( arg, "heart"  ) )
            ||        ( !str_cmp( arg, "hearts" ) ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <9>HEART<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <9>HEART<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_HEART_TRAUMA))
                        {
                                act ("You successfully attack $N's heart, and <9>wound it grievously<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the heart -- you go into <9>cardiac arrest<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N directly in the heart!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the heart!", ch, victim);

                                damage(ch, victim, number_range(30, (ch->level *3)), gsn_heart_trauma, FALSE);
                                daze(ch, victim, gsn_heart_trauma,2);

                                dot_dam = number_range((victim->level * 5),(victim->level * 6));
                                if (dot_dam > MAX_DAMAGE)
                                    dot_dam = MAX_DAMAGE;

                                af.type      = gsn_heart_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_NONE;
                                af.modifier  = dot_dam;
                                af.bitvector = AFF_DOT;
                                affect_to_char( victim, &af );

                                af.location  = APPLY_CON;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_HEART_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already damaged $N's heart grievously.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "heart" ITEM_BODY_PARTs ch is wearing, whether heart trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 5);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "tail"  ) )
            ||          ( !str_cmp( arg, "tails" ) ) )
                && ( ( victim->race == RACE_YUAN_TI )
                  || ( victim->race == RACE_SATYR )
                  || ( victim->race == RACE_CENTAUR )
                  || ( victim->race == RACE_SAHUAGIN )
                  || ( victim->race == RACE_TIEFLING )
                  || ( victim->race == RACE_HALF_DRAGON )
                  || ( victim->class == CLASS_SHAPE_SHIFTER )
                  || ( victim->sub_class == SUB_CLASS_WEREWOLF ) ) )
            {
                if ( ( ( ( victim->class == CLASS_SHAPE_SHIFTER )
                      && ( victim->sub_class != SUB_CLASS_WEREWOLF )
                      && ( victim->sub_class != SUB_CLASS_VAMPIRE ) )
                &&       ( ( victim->form != FORM_CHAMELEON )
                        &&  ( victim->form != FORM_CAT )
                        &&  ( victim->form != FORM_HAWK )
                        &&  ( victim->form != FORM_SNAKE )
                        &&  ( victim->form != FORM_SCORPION )
                        &&  ( victim->form != FORM_TIGER )
                        &&  ( victim->form != FORM_HYDRA )
                        &&  ( victim->form != FORM_TIGER )
                        &&  ( victim->form != FORM_GRIFFIN )
                        &&  ( victim->form != FORM_DRAGON ) ) )
                ||   ( ( victim->sub_class == SUB_CLASS_WEREWOLF )
                        &&  ( ( victim->form != FORM_WOLF ) && ( victim->form != FORM_DIREWOLF ) ) ) )
                {
                        WAIT_STATE(ch, PULSE_VIOLENCE);
                        act("{W$C doesn't (currently!) have that body part!{x", ch, NULL, victim, TO_CHAR);
                        return;
                }

                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <154>TAIL<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <154>TAIL<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_TAIL_TRAUMA))
                        {
                                act ("You successfully attack $N's tail, and <154>viciously maim it<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the tail -- it is <154>severely damaged<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the tail!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly on their tail!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_tail_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 12);
                                af.bitvector = AFF_TAIL_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted ruinous tail damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* higher-than usual (big target!) chance of removing any "tail" ITEM_BODY_PARTs ch is wearing, whether tail trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 6);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( !str_cmp( arg, "torso"  ) )
            ||        ( !str_cmp( arg, "body"   ) ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);

                send_to_char("You <10>target<0> your opponent's <173>TORSO<0> with a strike!\n\r", ch);
                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <173>TORSO<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_TORSO_TRAUMA))
                        {
                                act ("You successfully attack $N's body, and inflict a <173>terrible gut wound<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the torso -- your guts start <173>leaking out<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N directly in the torso!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n hit $N straight in the gut!", ch, victim);

                                damage(ch, victim, number_range(20, (ch->level *2)), gsn_target, FALSE);

                                dot_dam = number_range((victim->level),(victim->level * 2));
                                if (dot_dam > MAX_DAMAGE)
                                    dot_dam = MAX_DAMAGE;

                                af.type      = gsn_torso_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_NONE;
                                af.modifier  = dot_dam;
                                af.bitvector = AFF_DOT;
                                affect_to_char( victim, &af );

                                af.location  = APPLY_STR;
                                af.modifier  = -(ch->level / 15);
                                af.bitvector = AFF_TORSO_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already grievously wounded $N's body.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "torso" ITEM_BODY_PARTs ch is wearing, whether torso trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 7);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                act("{W$C doesn't have that body part!{x", ch, NULL, victim, TO_CHAR);
                return;
            }
        }
        else {

            /* PLAYER VERSUS MOB or MOB VERSUS MOB STARTS HERE */

            if ( ( ( !str_cmp( arg, "eye"  ) )
            ||     ( !str_cmp( arg, "eyes" ) ) )
                && HAS_EYES( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);

                act ("You <10>target<0> $N's <14>EYES<0> with a strike!", ch, NULL, victim, TO_CHAR);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <14>EYES<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_EYE_TRAUMA))
                        {
                                act ("You successfully attack $N's eyes, and <14>seriously damage them<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the eyes -- they are <14>grievously damaged<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the eyes!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N flush in the eyes!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_eye_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_HITROLL;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = (AFF_BLIND + AFF_EYE_TRAUMA);

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted horrific eye trauma on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "eye" ITEM_BODY_PARTs ch is wearing, whether eye trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 2);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "head"  ) )
            ||          ( !str_cmp( arg, "heads" ) ) )
                && HAS_HEAD( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <194>HEAD<0> with a strike!\n\r", ch);
                if ( (!IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target]) ) ) ) )
                ||   ( IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {

                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <194>HEAD<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_HEAD_TRAUMA))
                        {
                                act ("You successfully strike $N's head, and cause <194>serious brain injury<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the head -- you take <194>critical brain damage<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the head!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the head!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_head_trauma, FALSE);
                                daze(ch, victim, gsn_head_trauma, 3);

                                af.type      = gsn_head_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_WIS;
                                af.modifier  = -(ch->level / 10);
                                af.bitvector = AFF_HEAD_TRAUMA;
                                affect_to_char(victim, &af);

                                af.location  = APPLY_INT;
                                af.modifier  = -(ch->level / 10);
                                af.bitvector = AFF_HEAD_TRAUMA;
                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted major head trauma on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* Small chance of decapitation */

                        if ( !(IS_NPC(victim) && IS_SET(victim->act, ACT_UNKILLABLE) )
                        && ( victim->hit <= ( victim->max_hit / 5 ) )
                        &&      ((!IS_NPC(ch) && ( number_percent () < ( ( ch->pcdata->learned[gsn_target] / 50 ) + 1 ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 50 ) ) ) ) )
                        {
                                act ("{BYou strike $N's head perfectly and it DETACHES!!{x", ch, NULL, victim, TO_CHAR);
                                act ("{B$c's vicious head strike causes $N's head to separate from $S body and fly away!{x",
                                    ch, NULL, victim, TO_NOTVICT);
                                act ("{B$c DECAPITATES you!!{x", ch, NULL, victim, TO_VICT);

                                if (IS_NPC(victim))
                                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                                victim->short_descr);
                                else
                                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                                victim->name);

                                act (msg, ch, NULL, NULL, TO_ROOM);
                                arena_commentary("$c decapitates $N!", ch, victim);

                                vnum = OBJ_VNUM_SEVERED_HEAD;

                                if (vnum != 0)
                                {
                                        OBJ_DATA *obj;
                                        char      *name;
                                        char      buf [ MAX_STRING_LENGTH ];

                                        name = IS_NPC(victim) ? victim->short_descr : victim->name;
                                        obj = create_object(get_obj_index(vnum), 0, "common", CREATED_NO_RANDOMISER);
                                        obj->timer = number_range(4, 7);
                                        obj->timermax = obj->timer;

                                        sprintf(buf, obj->short_descr, name);
                                        free_string(obj->short_descr);
                                        obj->short_descr = str_dup(buf);

                                        sprintf(buf, obj->description, name);
                                        free_string(obj->description);
                                        obj->description = str_dup(buf);

                                        obj_to_room(obj, ch->in_room);
                                }

                                damage(ch,victim,victim->max_hit/2,gsn_decapitate, FALSE);
                                return;
                        }

                        /* chance of removing any "head" ITEM_BODY_PARTs ch is wearing, whether head trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 1);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "arm"  ) )
            ||          ( !str_cmp( arg, "arms" ) ) )
                && HAS_ARMS( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <174>ARM<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <174>ARM<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_ARM_TRAUMA))
                        {
                                act ("You successfully attack $N's arm, and <174>mutilate it beyond recognition<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the arm -- it is <174>mutilated beyond recognition<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the arm!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N solidly in the arm!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_arm_trauma;
                                af.duration  = -1;
                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_ARM_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted critical arm damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "arm" ITEM_BODY_PARTs ch is wearing, whether arm trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 3);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "leg"  ) )
            ||          ( !str_cmp( arg, "legs" ) ) )
                && HAS_LEGS( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <186>LEG<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <186>LEG<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_LEG_TRAUMA))
                        {
                                act ("You successfully attack $N's leg, and <186>brutally cripple it<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the leg -- it is <186>maimed<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the leg!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the leg!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_leg_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_LEG_TRAUMA;

                                affect_to_char(victim, &af);

                                af.location  = APPLY_MOVE;
                                af.modifier  = ( ch->level * -2 );

                                af.bitvector = AFF_LEG_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted severe leg damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "leg" ITEM_BODY_PARTs ch is wearing, whether leg trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 4);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "heart"  ) )
            ||          ( !str_cmp( arg, "hearts" ) ) )
                && HAS_HEART( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <9>HEART<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <9>HEART<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_HEART_TRAUMA))
                        {
                                act ("You successfully attack $N's heart, and <9>wound it grievously<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the heart -- you go into <9>cardiac arrest<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N directly in the heart!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly in the heart!", ch, victim);

                                damage(ch, victim, number_range(30, (ch->level *3)), gsn_target, FALSE);
                                daze(ch, victim, gsn_heart_trauma,2);

                                dot_dam = (victim->hit / 20);
                                if (dot_dam > MAX_DAMAGE)
                                    dot_dam = MAX_DAMAGE;

                                af.type      = gsn_heart_trauma;
                                af.duration  = -1;

                                af.bitvector = AFF_DOT;
                                af.location  = APPLY_NONE;
                                af.modifier  = dot_dam;
                                affect_to_char( victim, &af );

                                af.location  = APPLY_CON;
                                af.modifier  = -(ch->level / 8);
                                af.bitvector = AFF_HEART_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already damaged $N's heart grievously.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "heart" ITEM_BODY_PARTs ch is wearing, whether heart trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 5);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( ( !str_cmp( arg, "tail"  ) )
            ||          ( !str_cmp( arg, "tails" ) ) )
                && HAS_TAIL( victim ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <154>TAIL<0> with a strike!\n\r", ch);

                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <154>TAIL<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_TAIL_TRAUMA))
                        {
                                act ("You successfully attack $N's tail, and <154>viciously maim it<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the tail -- it is <154>severely damaged<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N in the tail!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n strikes $N directly on their tail!", ch, victim);

                                damage(ch, victim, number_range(10, ch->level), gsn_target, FALSE);

                                af.type      = gsn_tail_trauma;
                                af.duration  = -1;

                                af.location  = APPLY_DEX;
                                af.modifier  = -(ch->level / 12);
                                af.bitvector = AFF_TAIL_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already inflicted ruinous tail damage on $N.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* higher-than usual (big target!) chance of removing any "tail" ITEM_BODY_PARTs ch is wearing, whether tail trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 6);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else if ( ( !str_cmp( arg, "torso"  ) )
            ||        ( !str_cmp( arg, "body"   ) ) )
            {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                send_to_char("You <10>target<0> your opponent's <173>TORSO<0> with a strike!\n\r", ch);
                if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] ) - ( victim->level - ch->level ) ) ) ) )
                ||   (  IS_NPC(ch) && ( number_percent () < ch->level ) ) )
                {
                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                        arena_commentary("$n targets $N's <173>TORSO<0> with a strike!", ch, victim);

                        if (!IS_AFFECTED(victim, AFF_TORSO_TRAUMA))
                        {
                                act ("You successfully attack $N's body, and inflict a <173>terrible gut wound<0>!", ch, NULL, victim, TO_CHAR);
                                act ("$n strikes you in the torso -- your guts start <173>leaking out<0>!", ch, NULL, victim, TO_VICT);
                                act ("$n strikes $N directly in the torso!", ch, NULL, victim, TO_NOTVICT);
                                arena_commentary("$n hit $N straight in the gut!", ch, victim);

                                damage(ch, victim, number_range(20, (ch->level *2)), gsn_target, FALSE);

                                dot_dam = (victim->hit / 50);
                                if (dot_dam > MAX_DAMAGE)
                                    dot_dam = MAX_DAMAGE;

                                af.type      = gsn_torso_trauma;
                                af.duration  = -1;

                                af.bitvector = AFF_DOT;
                                af.location  = APPLY_NONE;
                                af.modifier  = dot_dam;
                                affect_to_char( victim, &af );

                                af.location  = APPLY_STR;
                                af.modifier  = -(ch->level / 15);
                                af.bitvector = AFF_TORSO_TRAUMA;

                                affect_to_char(victim, &af);
                        }
                        else {
                                act ("<6>You have already grievously wounded $N's body.<0>", ch, NULL, victim, TO_CHAR);
                        }

                        /* chance of removing any "torso" ITEM_BODY_PARTs ch is wearing, whether torso trauma has
                           already been inflicted or not -- fails silently if ch has none  */

                        if ( ( !IS_NPC(ch) && ( number_percent () < UMAX(5, ( ( ch->pcdata->learned[gsn_target] / 2 ) - ( victim->level - ch->level ) ) ) ) )
                            ||   ( IS_NPC(ch) && ( number_percent () < ( ch->level / 2 ) ) ) )
                        {
                            disarm_bodypart(ch, victim, 7);
                        }

                        check_group_bonus(ch);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else {
                        damage (ch, victim, 0, gsn_target, FALSE);
                }
                return;
            }
            else {
                WAIT_STATE(ch, PULSE_VIOLENCE);
                act("{W$C doesn't have that body part!{x", ch, NULL, victim, TO_CHAR);
                return;
            }

        }
}

void do_destrier (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        int chance;

        if (IS_NPC(ch))
                return;

        if ( !ch->mount && ch->race != RACE_CENTAUR)
        {
                send_to_char("You must have a mount to trample someone with!\n\r",ch);
                return;
        }

        if (!CAN_DO(ch, gsn_destrier))
        {
                send_to_char("You do not have the skill of a mounted Knight.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if ( ch->in_room->sector_type == SECT_UNDERWATER
        && ( ch->race != RACE_SAHUAGIN && ch->race != RACE_GRUNG ) )
        {
                send_to_char("You can't do that underwater without firm ground beneath you.\n\r", ch);
                return;
        }

        if (ch->mount)
        {
                if (!can_see(ch->mount, victim))
                {
                        send_to_char("Your mount cannot see them!\n\r", ch);
                        return;
                }
        }
        else
        {
                if (!can_see(ch, victim))
                {
                        send_to_char("You can't see them!\n\r", ch);
                        return;
                }
        }

        if (is_safe (ch, victim))
                return;

        WAIT_STATE (ch, 2 * PULSE_VIOLENCE);

        chance = ch->pcdata->learned[gsn_destrier] + ((ch->level - victim->level) * 3);

        if (number_percent () < chance)
        {
                act ("$n's mount {Gtramples{x you!", ch, NULL, victim,TO_VICT);
                act ("$n's mount {Gtramples{x $N!", ch, NULL,victim,TO_NOTVICT);
                act ("Your mount {Gtramples{x $N!", ch, NULL,victim,TO_CHAR);

                arena_commentary("$n's mount tramples $N to the ground.", ch, victim);

                WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
                damage(ch, victim, number_range(ch->level * 2, ch->level * 4), gsn_destrier, FALSE);
        }
        else
                damage (ch, victim, 0, gsn_destrier, FALSE);
}


void do_dive (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_dive) )
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->form != FORM_DRAGON)
        {
                send_to_char("Not in this form.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Dive upon whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("How can you do that?\n\r", ch);
                return;
        }

        if ( ( ch->in_room->sector_type == SECT_UNDERWATER )
          || ( ch->in_room->sector_type == SECT_UNDERWATER_GROUND ) )
        {
                send_to_char("You can't dive quickly enough through the water.\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You can't dive upon a combatant.\n\r", ch);
                return;
        }

        if ((victim->hit < victim->max_hit) && IS_NPC(victim) && !IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("They seem too alert for you to successfully surprise them.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (is_affected(victim, gsn_gaias_warning))
        {
                act ("You attempt to dive at $N but they deftly step aside.",
                     ch, NULL, victim, TO_CHAR);
                act ("Sensing impending danger you step to one side, just as $n flashes by.",
                     ch, NULL, victim, TO_VICT);
                act ("$n's attempted dive is thwarted by $N's acute senses.",
                     ch, NULL, victim, TO_NOTVICT);
                damage(ch, victim, 0, gsn_dive, FALSE);
                return;
        }

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        if ((!IS_AWAKE(victim)
             || IS_NPC(ch)
             || number_percent() < ch->pcdata->learned[gsn_dive])
            && victim->backstab == 0 )
        {
                arena_commentary("$n dives on $N.", ch, victim);
                victim->backstab = 5;
                one_hit(ch, victim, gsn_dive, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_dive, FALSE);
}


void do_backstab (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];
        int        assas_chance;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_backstab) && ch->form != FORM_SCORPION)
        {
                send_to_char("You better leave the assassin trade to thieves.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Backstab whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if ( ch->mount && ch->mount == victim )
        {
                send_to_char("Not while you're riding it!\n\r",ch);
                return;
        }

        if (IS_HUGE (victim))
        {
                act ("$C is too large for you to backstab!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_NPC(victim)
            && IS_NPC(ch)
            && ch->pIndexData->vnum != BOT_VNUM
            && victim->pIndexData->vnum != BOT_VNUM)
                return;

        if (victim == ch)
        {
                send_to_char("How can you sneak up on yourself?\n\r", ch);
                return;
        }

        if (!(obj = get_eq_char(ch, WEAR_WIELD)) || !is_piercing_weapon(obj))
        {
                send_to_char("You need to wield a piercing or stabbing weapon.\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You can't backstab a combatant.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (victim->hit < victim->max_hit
            && IS_NPC(victim)
            && !IS_SET(victim->act, ACT_OBJECT)
            && victim->pIndexData->vnum != BOT_VNUM)
        {
                act ("$C is hurt and suspicious... you can't sneak up on $m.",
                     ch, NULL, victim, TO_CHAR);
                return;
        }

        if (is_affected(victim, gsn_gaias_warning))
        {
                act ("You attempt to backstab $N, but $e deftly steps aside!",
                     ch, NULL, victim, TO_CHAR);
                act ("Sensing impending danger, you step to one side, just as $n's blade flashes by.",
                     ch, NULL, victim, TO_VICT);
                act ("$c's attempted backstab is thwarted by $N!",
                     ch, NULL, victim, TO_NOTVICT);
                damage(ch, victim, 0, gsn_backstab, FALSE);
                return;
        }

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                if (ch->form == FORM_SCORPION)
                {
                        send_to_char("Your tail is too damaged to backstab anyone with.\n\r", ch);
                        return;
                }
        }

        check_killer(ch, victim);
        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (ch->level > victim->level)
                assas_chance = (((ch->level - victim->level) / 5 ) * 10);
        else
                assas_chance = 10;

        if (((!IS_NPC(ch) && number_percent() < ch->pcdata->learned[gsn_backstab])
             || !IS_AWAKE(victim)
             || IS_AFFECTED(victim, AFF_HOLD)
             || IS_NPC(ch)
             || ch->form == FORM_SCORPION)
             && victim->backstab == 0)
        {
                victim->backstab = 5;

                if (!IS_NPC(ch)
                    && number_percent() < ch->pcdata->learned[gsn_assassinate]
                    && number_percent() < assas_chance
                    && HAS_HEAD(victim)
                    && !IS_SET(victim->act, ACT_OBJECT)
                    && !IS_INORGANIC(victim)
                    && !(is_entered_in_tournament(victim)
                         && is_still_alive_in_tournament(victim)
                         && tournament_status == TOURNAMENT_STATUS_RUNNING))
                {
                        arena_commentary("$n assassinates $N!", ch, victim);
                        act("You spin around $N and quickly slit $S throat!",
                            ch, NULL, victim, TO_CHAR);
                        act("$c spins around $N and quickly slits $S throat!",
                            ch, NULL, victim, TO_NOTVICT);
                        act("$c spins around you and quickly slits your throat!\n\r",
                            ch, NULL, victim, TO_VICT);
                        send_to_char(purgatory_message, victim);

                        /*
                         *  No penalties for arena death; straight to corpse
                         */
                        if (!IS_NPC(victim)
                            && IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                        {
                                raw_kill(ch, victim, TRUE);
                                char_from_room(victim);
                                char_to_room(victim, get_room_index(ROOM_VNUM_GRAVEYARD_A));
                        }
                        else
                        {
                                /* If a mob you're grouped with killsteals, you get the reward */
                                if ( ( IS_NPC(ch) && ch->master )
                                && ( ch->master->sub_class == SUB_CLASS_WITCH
                                  || ch->master->sub_class == SUB_CLASS_INFERNALIST
                                  || ch->master->sub_class == SUB_CLASS_NECROMANCER
                                  || ch->master->sub_class == SUB_CLASS_KNIGHT
                                  || ( ( ch->master->class == CLASS_SHAPE_SHIFTER )
                                    && ( ch->master->sub_class == 0 ) )
                                  || ch->master->sub_class == SUB_CLASS_WEREWOLF ) )
                                {
                                        group_gain(ch->master, victim, TRUE);
                                }
                                else
                                {
                                        group_gain(ch, victim, FALSE);
                                }

                                check_player_death(ch, victim);
                                death_penalty(ch, victim);
                                raw_kill(ch, victim, TRUE);
                                check_autoloot(ch, victim);
                        }

                        return;
                }

                arena_commentary("$n backstabs $N.", ch, victim);
                WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
                multi_hit(ch, victim, gsn_backstab);
        }
        else
                damage(ch, victim, 0, gsn_backstab, FALSE);
}


void do_lunge (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        int chance;

        if (IS_NPC(ch) && !(ch->spec_fun == spec_lookup("spec_mast_vampire")
                            || ch->pIndexData->vnum == BOT_VNUM))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_lunge))
        {
                send_to_char("Thou are not a Vampire.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Lunge at whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("How can you sneak up on yourself?\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You can't lunge at a fighting person.\n\r", ch);
                return;
        }

        if ((victim->hit < victim->max_hit) && (IS_NPC(victim)))
        {
                act ("$N is hurt and suspicious... you can't sneak up on $m.",
                     ch, NULL, victim, TO_CHAR);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (is_affected(victim, gsn_gaias_warning))
        {
                act ("You attempt to lunge at $N but $e deftly steps aside.",
                     ch, NULL, victim, TO_CHAR);
                act ("Sensing impending danger you step to one side, just as $n's fangs flash by.",
                     ch, NULL, victim, TO_VICT);
                act ("$n's attempt to sink their fangs into $N is thwarted by $N's acute awareness.",
                     ch, NULL, victim, TO_NOTVICT);
                damage(ch, victim, 0, gsn_lunge, FALSE);
                return;
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
                send_to_char("Your legs are too injured to lunge effectively.\n\r", ch);
                return;
        }

        check_killer(ch, victim);
        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (IS_NPC(ch))
                chance = UMIN(50 + ch->level * 2, 95);
        else
                chance = ch->pcdata->learned[gsn_lunge];

        if ((!IS_AWAKE(victim)
             || IS_NPC(ch)
             || number_percent() < chance)
            && victim->backstab == 0 )
        {
                arena_commentary("$n lunges at $N.", ch, victim);
                victim->backstab = 5;
                WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
                multi_hit(ch, victim, gsn_lunge);

                if (!IS_NPC(ch) && number_percent() < ch->pcdata->learned[gsn_double_lunge])
                        multi_hit(ch,victim, gsn_lunge);
        }
        else
                damage(ch, victim, 0, gsn_lunge, FALSE);
}


void do_joust (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *wobj, *first_weapon, *second_weapon;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_joust))
        {
                send_to_char("You better leave that to a knight.\n\r", ch);
                return;
        }

        wobj = get_eq_char( ch, WEAR_RANGED_WEAPON );

        if( !wobj || !IS_OBJ_STAT( wobj, ITEM_LANCE ) )
        {
                send_to_char("You must have a lance readied to joust!\n\r",ch);
                return;
        }

        if ( !ch->mount && ch->race != RACE_CENTAUR )
        {
                send_to_char("You must have a mount to charge someone on!\n\r",ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Joust at whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (ch->mount && ch->mount == victim)
        {
                send_to_char ("You can't charge your own mount!\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("How can you do that?\n\r", ch);
                return;
        }

        if (ch->mount && ch->mount->fighting)
        {
                send_to_char("Your mount isn't ready to joust right now!\n\r", ch);
                return;
        }

        if ((victim->hit < victim->max_hit) && IS_NPC(victim) && !IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("They won't fall for that twice!\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You can't joust at a combatant.\n\r", ch);
                return;
        }

        if (victim->position < POS_STANDING)
        {
                send_to_char("They're too low down to be jousted.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (is_affected(victim, gsn_gaias_warning))
        {
                act ("You attempt to joust at $N but $e deftly moves aside.",
                     ch, NULL, victim, TO_CHAR);
                act ("Sensing impending danger you move to one side, just as $n's blade flashes by.",
                     ch, NULL, victim, TO_VICT);
                act ("$n's jousting is thwarted by $N's acute awareness.",
                     ch, NULL, victim, TO_NOTVICT);
                damage(ch, victim, 0, gsn_joust, FALSE);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA) )
        {
                send_to_char("Your arm is too damaged to joust effectively.\n\r", ch);
                return;
        }

        check_killer(ch, victim);
        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        first_weapon = get_eq_char( ch, WEAR_WIELD );
        if( first_weapon )
                unequip_char( ch, first_weapon );

        second_weapon = get_eq_char( ch, WEAR_DUAL );
        if( second_weapon )
                unequip_char( ch, second_weapon );

        equip_char( ch, wobj, WEAR_WIELD );

        if ((!IS_AWAKE(victim)
             || IS_NPC(ch)
             || number_percent() < ch->pcdata->learned[gsn_joust])
            && victim->backstab == 0 )
        {
                arena_commentary("$n hits $N with a joust.", ch, victim);
                victim->backstab = 5;
                one_hit(ch, victim, gsn_joust, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_joust, FALSE);

        /*  Double check lance is still equipped  */
        if (get_eq_char (ch, WEAR_WIELD))
        {
                unequip_char( ch, wobj );
                equip_char( ch, wobj, WEAR_RANGED_WEAPON );
        }

        if( first_weapon )
                equip_char( ch, first_weapon, WEAR_WIELD );

        if( second_weapon )
                equip_char( ch, second_weapon, WEAR_DUAL );
}


void do_berserk (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA af;

        if ((!IS_NPC(ch) && !CAN_DO(ch, gsn_berserk))
            || ch->sub_class == SUB_CLASS_WEREWOLF)
        {
                send_to_char("You're not enough of a warrior to go berserk.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_berserk))
                return;

        send_to_char("<88>A red mist comes over your vision!<0>\n\r", ch);
        send_to_char("The thrill of combat begins to drive you crazy!\n\r", ch);
        arena_commentary("$n has gone berserk.", ch, ch);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_berserk])
        {
                af.type      = gsn_berserk;
                af.duration  = -1;
                af.location  = APPLY_HITROLL;
                af.modifier  = (ch->level / 4);
                af.bitvector = 0;
                affect_to_char(ch, &af);

                af.location  = APPLY_DAMROLL;
                affect_to_char(ch, &af);

                af.location  = APPLY_AC;
                af.modifier  = ch->level;
                affect_to_char(ch, &af);

                send_to_char("You have gone {RBERSERK{x!\n\r", ch);
                act ("$n has gone {RBERSERK{x!", ch, NULL, NULL, TO_ROOM);

                return;
        }

        send_to_char("You shake off the madness.\n\r", ch);
}


void do_flee (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA        *victim;
        ROOM_INDEX_DATA  *was_in;
        ROOM_INDEX_DATA  *now_in;
        int              attempt;
        bool             lose_xp;

        if (!(victim = ch->fighting))
        {
                if (ch->position == POS_FIGHTING)
                        ch->position = POS_STANDING;

                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (tournament_action_illegal(ch, TOURNAMENT_OPTION_NOFLEE))
        {
                send_to_char("You can't flee during this tournament fight!\n\r", ch);
                return;
        }

        if (has_ego_item_effect(ch, EGO_ITEM_BLOODLUST)
            && !has_ego_item_effect(ch, EGO_ITEM_BATTLE_TERROR))
        {
                send_to_char("You must not flee: your enemy must die!\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if ( IS_NPC(ch) && IS_SET(ch->act, ACT_OBJECT) )
        {
                send_to_char( "An object cannot be made to flee.\n\r", ch );
                return;
        }

        if (IS_AFFECTED(ch, AFF_HOLD) && number_percent() < 80)
        {
                send_to_char("You fail to break free from your prison!\n\r", ch);
                act ("$n attemps to break free from $s prison, but fails!!",
                     ch, NULL, victim, TO_ROOM);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected(ch, gsn_coil))
        {
                send_to_char("You cannot flee from the coils!\n\r", ch);
                act ("$n attemps in vain to break free from $s coils!!",
                     ch, NULL, victim, TO_ROOM);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected(ch, gsn_berserk) || is_affected(ch, gsn_rage))
        {
                send_to_char("The battle madness consumes you, you cannot flee!\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected( ch, gsn_frenzy))
        {
                send_to_char("The divine rage flowing through your body overrides your desire to flee...\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
                send_to_char("Your legs are too damaged to have any hope of escape!\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        was_in = ch->in_room;
        lose_xp = TRUE;

        if (IS_SET(was_in->room_flags, ROOM_PLAYER_KILLER))
                lose_xp = FALSE;

        for (attempt = 0; attempt < 6; attempt++)
        {
                EXIT_DATA *pexit;
                int        door;

                door = number_door();

                if ((pexit = was_in->exit[door]) == 0
                    || !pexit->to_room
                    || (IS_SET(pexit->exit_info, EX_CLOSED)
                        && (IS_SET(pexit->exit_info, EX_PASSPROOF)
                            || !IS_AFFECTED(ch, AFF_PASS_DOOR)))
                    || (IS_NPC(ch)
                        && (IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
                            || (IS_SET(ch->act, ACT_STAY_AREA)
                                && pexit->to_room->area != ch->in_room->area))))
                {
                        continue;
                }

                /* Make it harder for mobs to flee; Gez 2000 */
                if (IS_NPC(ch) && number_bits(1) && str_cmp("Fear", argument))
                        break;

                stop_fighting(ch, TRUE);

                if (IS_AFFECTED(ch, AFF_HOLD))
                {
                        act ("$n breaks free from $s prison!",ch,NULL,victim,TO_ROOM);
                        affect_strip(ch, gsn_trap);
                        REMOVE_BIT(ch->affected_by, AFF_HOLD);
                }

                move_char(ch, door);
                if ((now_in = ch->in_room) == was_in)
                        continue;

                if (ch->mount)
                {
                        char_from_room (ch->mount);
                        char_to_room (ch->mount, ch->in_room);
                        stop_fighting (ch->mount, TRUE);
                }

                if (ch->rider)
                {
                        char_from_room (ch->rider);
                        char_to_room (ch->rider, ch->in_room);
                        stop_fighting (ch->rider, TRUE);
                }

                ch->in_room = was_in;
                act ("$n has fled!", ch, NULL, NULL, TO_ROOM);
                arena_commentary("$n has fled!", ch, ch);
                ch->in_room = now_in;
                WAIT_STATE(ch, PULSE_VIOLENCE);

                if (!IS_NPC(ch))
                {
                        char buf [MAX_INPUT_LENGTH];

                        /*  No xp or fame penalties if in the arena; Gezhp 2000  */
                        if (!lose_xp)
                        {
                                send_to_char("You flee from combat!\n\r", ch);
                                return;
                        }

                        /* if they were swallowed, some text indicating escape + stripping effects */
                        if (IS_AFFECTED(ch, AFF_SWALLOWED))
                        {
                                strip_swallow(ch);
                        }

                        if (ch->form == FORM_GRIFFIN || ch->form == FORM_BAT)
                        {
                                sprintf( buf, "You escape from your opponent without futher harm.\n\r");
                                send_to_char(buf, ch);
                        }
                        else
                        {
                                sprintf(buf, "You flee from combat! You lose %d exp.\n\r",
                                        level_table[ch->level].exp_level/100);
                                send_to_char(buf, ch);
                                gain_exp(ch, -level_table[ch->level].exp_level/100);
                        }

                        if ( ch->pcdata->fame > 500
                        &&   ch->form != FORM_BAT )
                        {
                                if (str_cmp("Wimpy", argument))
                                {
                                        sprintf(buf, "Your cowardly actions cover you in shame!  You lose %d fame.\n\r", ch->pcdata->fame/40);
                                        send_to_char(buf, ch);
                                        ch->pcdata->fame = ch->pcdata->fame - (ch->pcdata->fame/40);
                                }
                                else
                                {
                                        sprintf(buf, "Your cowardly actions cover you in shame!  You lose %d fame.\n\r", ch->pcdata->fame/20);
                                        send_to_char(buf, ch);
                                        ch->pcdata->fame = ch->pcdata->fame - (ch->pcdata->fame/20);
                                }
                                check_fame_table(ch);
                        }

                        if (ch->pcdata->dam_bonus > 0)
                        {
                                sprintf(buf, "However, you damaged your opponent sufficiently for %d experience.\n\r", ch->pcdata->dam_bonus);
                                send_to_char(buf, ch);
                                gain_exp (ch, ch->pcdata->dam_bonus);
                                ch->pcdata->dam_bonus = 0;
                        }
                }

                stop_fighting(ch, TRUE);
                return;
        }

        send_to_char("You failed!\n\r", ch);
        WAIT_STATE(ch, PULSE_VIOLENCE);
}

void do_bomb (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *victim;
        ROOM_INDEX_DATA *was_in;
        ROOM_INDEX_DATA *now_in;
        int              attempt;
        EXIT_DATA       *pexit;
        int             door;
        AFFECT_DATA     af;


        if ( (IS_NPC(ch)
        &&   !(ch->spec_fun == spec_lookup("spec_superwimpy")) ))
                return;

        if (!(victim = ch->fighting))
        {
                if (ch->position == POS_FIGHTING)
                        ch->position = POS_STANDING;

                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HOLD) && number_percent() < 80)
        {
                send_to_char("You fail to break free from your prison!\n\r", ch);
                act ("$n attemps to break free from $s prison, but fails!!",
                     ch, NULL, victim, TO_ROOM);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected(ch, gsn_coil))
        {
                send_to_char("You cannot flee from the coils!\n\r", ch);
                act ("$n attemps in vain to break free from $s coils!!",
                     ch, NULL, victim, TO_ROOM);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected(ch, gsn_berserk) || is_affected(ch, gsn_rage))
        {
                send_to_char("The battle madness consumes you, you cannot flee!\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if (is_affected( ch, gsn_frenzy))
        {
                send_to_char("The divine rage flowing through your body overrides your desire to flee...\n\r", ch);
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        if ( !IS_NPC(ch)
        && (number_percent() > (50 + (ch->pcdata->learned[gsn_bomb] / 2))))
        {
                act ("You throw a smoke bomb but your enemies aren't confused!", ch, NULL, NULL, TO_CHAR);
                act ("$n drops a smoke bomb but fails to escape!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if ( IS_NPC(ch)
        && (number_percent() > 85))
        {
                /* Put this in here for spec_superwimpy -- Owl 22/8/22 */
                act ("You throw a smoke bomb but your enemies aren't confused!", ch, NULL, NULL, TO_CHAR);
                act ("$n drops a smoke bomb but fails to escape!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        was_in = ch->in_room;

        /*
         * Have 30 goes at randomly finding an exit to escape too
         */

        for (attempt = 0; attempt < 30; attempt++)
        {
                /* Randomly find an exit */
                door = number_range(0, 5);

                if ((pexit = was_in->exit[door]) == 0
                    || !pexit->to_room
                    || (IS_SET(pexit->exit_info, EX_CLOSED)
                        && (IS_SET(pexit->exit_info, EX_PASSPROOF)
                            || !IS_AFFECTED(ch, AFF_PASS_DOOR)))
                    )
                {
                        continue;
                }

                stop_fighting(ch, TRUE);
                send_to_char ("{WYou drop a smoke bomb and escape!{x\n\r\n\r", ch);
                act ("{W$c drops a smoke bomb and escapes!{x", ch, NULL, NULL, TO_ROOM);
                arena_commentary("$c drops a smoke bomb and escapes!", ch, ch);

                /*
                 * Make the character sneak so characters in the room don't see which way they go
                 */

                af.type = gsn_sneak;
                af.duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_SNEAK;

                affect_to_char(ch, &af);

                /*
                 * I think this handles random rooms looping back on itself
                 */

                move_char(ch, door);
                if ((now_in = ch->in_room) == was_in)
                        continue;

                /*
                 * Valid exit - we escape
                 */


                if (IS_AFFECTED(ch, AFF_HOLD))
                {
                        act ("$n breaks free from $s prison!",ch,NULL,victim,TO_ROOM);
                        affect_strip(ch, gsn_trap);
                        REMOVE_BIT(ch->affected_by, AFF_HOLD);
                }

                if (ch->mount)
                {
                        char_from_room (ch->mount);
                        char_to_room (ch->mount, ch->in_room);
                        stop_fighting (ch->mount, TRUE);
                }

                if (ch->rider)
                {
                        char_from_room (ch->rider);
                        char_to_room (ch->rider, ch->in_room);
                        stop_fighting (ch->rider, TRUE);
                }

                ch->in_room = now_in;
                WAIT_STATE(ch, PULSE_VIOLENCE);
                return;
        }

        act ("You throw a smoke bomb but find no way to escape!", ch, NULL, NULL, TO_CHAR);
        act ("$n drops a smoke bomb but finds no way to escape!", ch, NULL, NULL, TO_ROOM);
        WAIT_STATE(ch, PULSE_VIOLENCE);
}



void do_rescue (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        char arg [ MAX_INPUT_LENGTH ];

        if ( IS_NPC(ch)
        &&  ( ch->master) )
        {
                /*
                 * Ordering an NPC you're grouped with to rescue you disconnects you and often crashes the MUD.
                 * be warned if you decide you want charmed/grouped mobs to be able to rescue players you will
                 * need to fix this issue, not just delete this check. Note the issue does not exist if the charmed
                 * NPC is not grouped with you. -- Owl 24/7/22
                 */

                return;
        }

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_rescue))
        {
                send_to_char("You'd better leave the heroic acts to warriors.\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_berserk))
        {
                send_to_char("You can't rescue anyone, you're BERSERK!\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Rescue whom?\n\r", ch);
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

        if (!IS_NPC(ch) && IS_NPC(victim))
        {
                send_to_char("They don't need your help!\n\r", ch);
                return;
        }

        if (ch->fighting == victim)
        {
                send_to_char("Too late.\n\r", ch);
                return;
        }

        if (!(victim->fighting))
        {
                send_to_char("That person is not fighting right now.\n\r", ch);
                return;
        }

        if (!is_same_group(ch, victim))
        {
                send_to_char("Why would you want to?\n\r", ch);
                return;
        }

        if (!check_blind (ch))
                return;

        /*
         * Shade 30.3.22
         *
         * We want to rescue only if a player is being attacked; find the first mob that is attacking
         * the victim and rescue them
         */

        for (vch = ch->in_room->people; vch; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (ch == vch)
                        continue;

                if (vch->deleted)
                        continue;

                if (vch->fighting == victim)
                        break;

        }

        if (!vch)
        {
                send_to_char("No one is attacking them!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_rescue].beats);

        if (!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_rescue])
        {
                send_to_char("You fail the rescue.\n\r", ch);
                return;
        }

        act ("You rescue $N!",  ch, NULL, victim, TO_CHAR);
        act ("$n rescues you!", ch, NULL, victim, TO_VICT);
        act ("$n rescues $N!",  ch, NULL, victim, TO_NOTVICT);

        stop_fighting(vch, FALSE);
        set_fighting(vch, ch);

        ch->pcdata->group_support_bonus += 1;

}


void do_dirt_kick (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char    arg [MAX_INPUT_LENGTH ];
        int     chance;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_dirt))
        {
                send_to_char("You don't know how to do that.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        if (!ch->fighting)
        {
                send_to_char("You're not in combat.\n\r", ch);
                return;
        }

        one_argument(argument,arg);

        if (arg[0] == '\0')
        {
                victim = ch->fighting;
                if (!victim)
                {
                        send_to_char("But you aren't in combat!\n\r",ch);
                        return;
                }
        }

        else if (!(victim = get_char_room(ch,arg)))
        {
                send_to_char("They aren't here.\n\r",ch);
                return;
        }

        if( IS_NPC(victim) && !HAS_EYES(victim) )
        {
                act( "You can't blind $N, $E doesn't have any eyes!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (IS_AFFECTED(victim, AFF_BLIND))
        {
                act ("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("Very funny.\n\r",ch);
                return;
        }

        if ( victim->fighting && !is_same_group( ch,victim->fighting ) )
        {
                send_to_char("Kill stealing is not permitted.\n\r",ch);
                return;
        }

        if ( IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim )
        {
                act ("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
                return;
        }

        if ( ch->in_room->sector_type == SECT_WATER_SWIM
        ||   ch->in_room->sector_type == SECT_WATER_NOSWIM
        ||   ch->in_room->sector_type == SECT_UNDERWATER
        ||   ch->in_room->sector_type == SECT_AIR )
        {
                send_to_char("There is no dirt to kick here.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch))
                chance = ch->pcdata->learned[gsn_dirt];
        else
                chance = 90;

        chance += (get_curr_dex(ch) - get_curr_dex(victim)) * 2;
        chance += (ch->level - victim->level) * 2;

        switch(ch->in_room->sector_type)
        {
            case(SECT_INSIDE):              chance -= 20;   break;
            case(SECT_CITY):                chance -= 10;   break;
            case(SECT_FIELD):               chance +=  5;   break;
            case(SECT_MOUNTAIN):            chance -= 10;   break;
            case(SECT_SWAMP):               chance -=  5;   break;
            case(SECT_UNDERWATER_GROUND):   chance += 15;   break;
            case(SECT_DESERT):              chance += 10;   break;
        }

        chance = URANGE(5, chance, 95);

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
            chance /= 2;

        if (is_safe(ch, victim))
                return;

        if (number_percent() < chance)
        {
                AFFECT_DATA af;

                arena_commentary("$n blinds $N with a dirt kick.", ch, victim);
                act ("$n is blinded by the dirt in $s eyes!", victim, NULL, NULL, TO_ROOM);
                damage(ch, victim, number_range(5,10), gsn_dirt, FALSE);
                send_to_char("You can't see a thing!\n\r", victim);

                af.type         = gsn_dirt;
                af.duration     = 0;
                af.location     = APPLY_HITROLL;
                af.modifier     = -4;
                af.bitvector    = AFF_BLIND;

                affect_to_char(victim,&af);

                check_group_bonus(ch);

        }
        else
                damage(ch, victim, 0, gsn_dirt, FALSE);

        WAIT_STATE(ch, PULSE_VIOLENCE);
}


void do_kick (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_kick))
        {
                send_to_char("You'd better leave the martial arts to fighters.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_kick].beats);

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
            send_to_char("You try to kick, but your leg is too damaged.\n\r", ch);
            return;
        }

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_kick])
        {
                arena_commentary("$n kicks $N.", ch, victim);

                if (!IS_NPC(ch))
                        damage(ch, victim, (ch->level / 2) + number_range(1, ch->level), gsn_kick, FALSE);
                else
                        damage(ch, victim, (ch->level) + number_range(1, ch->level), gsn_kick, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_kick, FALSE);

        return;
}


void do_knife_toss (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *victim;
        char            arg [ MAX_INPUT_LENGTH ];
        int             dam;
        int             chance;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_knife_toss))
        {
                send_to_char("You'd better leave the street fighting to thieves.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_knife_toss].beats);

        chance = number_percent();

        if (IS_NPC(ch) || chance < ch->pcdata->learned[gsn_knife_toss])
        {
                arena_commentary("$n tosses a knife at $N.", ch, victim);

                if (!IS_NPC(ch))
                {
                        dam = (ch->level / 2) + number_range(1, ch->level);

                        if( (IS_NPC(victim) && HAS_EYES(victim)) || !IS_NPC(victim) )
                        {
                                if (chance <= 10 )
                                {
                                        /*sprintf( buf, "{WYour knife catches the %s right in the face!{x\n\r", victim->name );
                                        send_to_char(buf, ch); */

                                        act("{WYour knife catches $N in the face!{x", ch, NULL, victim, TO_CHAR);

                                        if (!IS_NPC(victim))
                                                act("{W$c's knife catches you right in the face!{x", ch, NULL, victim, TO_VICT);

                                        act("{W$c's knife catches $N in the face!{x", ch, NULL, victim, TO_NOTVICT);

                                        dam *= 2;
                                }
                        }

                        damage(ch, victim, dam, gsn_knife_toss, FALSE);
                }
                else
                        damage(ch, victim, (ch->level) + number_range(1, ch->level), gsn_knife_toss, FALSE);
        }
        else
                damage(ch, victim, 0, gsn_knife_toss, FALSE);

        return;
}

void do_trap (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char    arg[MAX_INPUT_LENGTH];
        int     chance;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
        {
                if (ch->pIndexData->vnum != BOT_VNUM)
                    return;
        }
        else if (!CAN_DO(ch, gsn_trap))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Who do you want to trap?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_HOLD))
        {
                act ("$N is already trapped.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to trap yourself?\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot trap a fighting person.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_trap].beats);

        if (IS_NPC(ch))
                chance = 40 + ch->level / 2;
        else
                chance = ch->pcdata->learned[gsn_trap];

        chance += (ch->level - victim->level) * 5;

        if (chance < 5)
                chance = 5;

        if (chance > 95)
                chance = 95;

        if (number_percent() < chance)
        {
                AFFECT_DATA af;

                arena_commentary("$n traps $N.", ch, victim);
                act ("You skilfully snare $N in your trap!", ch, NULL, victim, TO_CHAR);
                act ("$n skilfully snares $N in $s trap!", ch, NULL, victim, TO_ROOM);

                af.type         = gsn_trap;
                af.duration     = 5;
                af.location     = APPLY_HITROLL;
                af.modifier     = -8;
                af.bitvector    = AFF_HOLD;

                affect_to_char(victim, &af);
                WAIT_STATE (victim, 2 * PULSE_VIOLENCE);

                if (!IS_NPC(ch) && ch->pcdata->tailing)
                        ch->pcdata->tailing = str_dup( "" );

                check_group_bonus(ch);
        }
        else
        {
                act ("$n attempts to trap $N, but steps into $s own trap!",
                     ch, NULL, victim, TO_ROOM);
                send_to_char("You unwittingly step into your own trap!!\n\r", ch);
                damage(ch, victim, 0, gsn_trap, FALSE);
        }

        WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
}

void do_snare (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char    arg[MAX_INPUT_LENGTH];
        int     chance;

        if (IS_NPC(ch))
        {
                if (ch->pIndexData->vnum != BOT_VNUM)
                    return;
        }
        else if (!CAN_DO(ch, gsn_snare))
        {
                send_to_char("You don't know how to do that.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Who do you want to ensnare?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_HOLD))
        {
                act ("$N is already ensnared.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to ensnare yourself?\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot ensnare a fighting person.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if ( IS_SET( ch->in_room->room_flags, ROOM_INDOORS )
        || ( ch->in_room->sector_type == SECT_INSIDE ) )
        {
                send_to_char("You can't set a snare indoors.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_snare].beats);

        if (IS_NPC(ch))
                chance = 40 + ch->level / 2;
        else
                chance = ch->pcdata->learned[gsn_snare];

        chance += (ch->level - victim->level) * 5;

        if (chance < 3)
                chance = 3;

        if (chance > 97)
                chance = 97;

        if (number_percent() < chance)
        {
                AFFECT_DATA af;

                arena_commentary("$n snares $N.", ch, victim);
                act ("$N becomes entangled in your snare!", ch, NULL, victim, TO_CHAR);
                act ("$n skilfully entangles $N in $s snare!", ch, NULL, victim, TO_ROOM);

                af.type         = gsn_snare;
                af.duration     = 6;
                af.location     = APPLY_HITROLL;
                af.modifier     = -10;
                af.bitvector    = AFF_HOLD;

                affect_to_char(victim, &af);
                WAIT_STATE (victim, 3 * PULSE_VIOLENCE);

                if (!IS_NPC(ch) && ch->pcdata->tailing)
                        ch->pcdata->tailing = str_dup( "" );

                check_group_bonus(ch);
        }
        else
        {
                act ("$n attempts to ensnare $N, but ensnares $mself instead!",
                     ch, NULL, victim, TO_ROOM);
                send_to_char("You unwittingly step into your own snare!!\n\r", ch);
                damage(ch, victim, 0, gsn_trap, FALSE);
        }

        WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
}



void do_transfix (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA     af;
        CHAR_DATA       *victim;
        char            arg[MAX_INPUT_LENGTH];
        int             chance;

        if ( IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_laghathti") ) )
                return;

        if ( !IS_NPC(ch)
        &&   !CAN_DO(ch, gsn_transfix))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_mist_walk))
        {
                send_to_char("You cannot transfix while in mist form.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Who do you want to transfix?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't transfix an object.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_BLIND))
        {
                send_to_char("You must be able to see to transfix someone!\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_HOLD))
        {
                act ("$N is already held.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You'll need a mirror to transfix yourself.\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot catch their eye.\n\r", ch);
                return;
        }

        if( IS_NPC(victim) && !HAS_EYES(victim) )
        {
                act( "You gaze at $N, but $E doesn't have any eyes!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_transfix].beats);

        if ( IS_NPC(ch) )
        {
            chance = 100;
        }
        else {
            chance = ch->pcdata->learned[gsn_transfix];
        }

        chance += (ch->level - victim->level) * 2;

        if (chance < 5)
                chance = 5;

        if (chance > 95)
                chance = 95;

        if (number_percent() < chance)
        {
                act ("You {Mtransfix{x $N with the power of your gaze!",
                     ch, NULL, victim, TO_CHAR);
                act ("$n {Mtransfixes{x $N with $s gaze!", ch, NULL, victim, TO_NOTVICT);
                act ("$n {Mtransfixes{x you with $s gaze!", ch, NULL, victim, TO_VICT);
                arena_commentary("$n transfixes $N.", ch, victim);

                af.type         = gsn_transfix;
                af.duration     = 3;
                af.location     = APPLY_HITROLL;
                af.modifier     = -8;
                af.bitvector    = AFF_HOLD;

                affect_to_char(victim, &af);
                WAIT_STATE (victim,    2 * PULSE_VIOLENCE);

                check_group_bonus(ch);


                if (!IS_NPC(ch) && ch->pcdata->tailing)
                        ch->pcdata->tailing = str_dup( "" );
        }
        else
        {
                act ("$n attempts to {Mtransfix{x $N, but they resist!",
                     ch, NULL, victim, TO_ROOM);
                send_to_char("They resist the power of your gaze!\n\r", ch);
                damage(ch, victim, 0, gsn_transfix, FALSE);
        }

        WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
}


void do_howl (CHAR_DATA *ch, char *argument)
{
        AFFECT_DATA     af;
        CHAR_DATA       *victim;
        char            arg[MAX_INPUT_LENGTH];
        int             chance;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_howl))
        {
                send_to_char("Yeah. Very intimidating.\n\r", ch);
                return;
        }

        if (ch->form != FORM_WOLF && ch->form != FORM_DIREWOLF)
        {
                send_to_char("Not in this form.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("You bay at the {Wmoon{x!\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_HOLD))
        {
                act ("$N is already held.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You howl scarily at yourself. Something isn't working...\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("Not while they are fighting.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("Objects are not affected by your howl.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        WAIT_STATE(ch, skill_table[gsn_howl].beats);

        chance = ch->pcdata->learned[gsn_howl];
        chance += (ch->level - victim->level) * 2;

        if (chance < 5)
                chance = 5;

        if (chance > 95)
                chance = 95;

        act( "{W::HoooOOOOOooowwwwlll::{x",ch, NULL, NULL, TO_ROOM);

        if (number_percent() < chance)
        {
                act ("You freeze $N with a {Wbone-chilling{x howl!",
                     ch, NULL, victim, TO_CHAR);
                act ("$n freezes $N with a {Wbone-chilling{x howl!",
                     ch, NULL, victim, TO_NOTVICT);
                act ("You freeze at the sound of $n's {Wbone-chilling{x howl!",
                     ch, NULL, victim, TO_VICT);
                arena_commentary("$n's howl freezes $N.", ch, victim);

                af.type         = gsn_howl;
                af.duration     = 3;
                af.location     = APPLY_HITROLL;
                af.modifier     = -8;
                af.bitvector    = AFF_HOLD;

                affect_to_char(victim, &af);
                WAIT_STATE(victim,    2 * PULSE_VIOLENCE);

        }
        else
        {
                send_to_char("They do not seem to fear your howl!\n\r", ch);
                damage(ch, victim, 0, gsn_howl, FALSE);
        }

        WAIT_STATE (ch, PULSE_VIOLENCE);
}


void do_headbutt (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;

        /* if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_warrior")
           || ch->spec_fun == spec_lookup("spec_sahuagin_infantry")
           || ch->spec_fun == spec_lookup("spec_sahuagin_cavalry")
           || ch->spec_fun == spec_lookup("spec_grail")
           || ch->spec_fun == spec_lookup("spec_kappa")
           || ch->spec_fun == spec_lookup("spec_sahuagin_guard")
           || ch->spec_fun == spec_lookup("spec_guard") ) )
                return;
        */

        if ( IS_NPC(ch)
        &&  ( ch->master ) )
        {
            return;
        }

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_headbutt))
        {
                send_to_char ("You cannot headbutt.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char ("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind (ch))
                return;

        if (ch->mount)
        {
                send_to_char("Not while mounted.\n\n", ch);
                return;
        }

        if (!HAS_HEAD (victim))
        {
                act ("$C has no head for you to butt!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_HUGE (victim))
        {
                act ("$C is too large for you to headbutt!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your head is far too damaged to be smashing it into things!\n\n", ch);
                return;
        }

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_headbutt])
        {
                if (IS_NPC(victim))
                {
                    if(!IS_SET(victim->act, ACT_OBJECT))
                    {
                        act ("Your {Cheadbutt{x causes $N to lose $S balance!", ch, NULL, victim, TO_CHAR);
                    }
                    else{
                        act ("{CYou smash your head into $N!{x", ch, NULL, victim, TO_CHAR);
                    }
                }
                else {
                        act ("Your {Cheadbutt{x causes $N to lose $S balance!", ch, NULL, victim, TO_CHAR);
                }

                act ("$n {Cheadbutts{x you - you see red!", ch, NULL, victim, TO_VICT);
                act ("$n {Cheadbutts{x $N!", ch, NULL, victim, TO_NOTVICT);

                arena_commentary("$n headbutts $N to the ground.", ch, victim);

                WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
                WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
                damage(ch, victim, (ch->level / 2) + number_range(1, ch->level * 3), gsn_headbutt, FALSE);

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (!IS_NPC(ch) && number_percent() < ch->pcdata->learned[gsn_second_headbutt])
                        damage(ch, victim, (ch->level / 2) + number_range (10, ch->level * 3),
                               gsn_second_headbutt, FALSE);
        }
        else
        {
                damage(ch, victim, 0, gsn_headbutt, FALSE);
                WAIT_STATE( ch, PULSE_VIOLENCE );
        }
}


void do_decapitate (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        OBJ_DATA *wobj;
        char arg [ MAX_INPUT_LENGTH ];
        int chance;
        char msg [ MAX_STRING_LENGTH ];
        int vnum;
        int ok;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_decapitate))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        ok = 0;
        wobj = get_eq_char(ch, WEAR_WIELD);

        if (wobj && IS_OBJ_STAT(wobj,ITEM_VORPAL))
                ok = 1;

        wobj = get_eq_char(ch, WEAR_DUAL);

        if (wobj && IS_OBJ_STAT(wobj,ITEM_VORPAL))
                ok = 1;

        if (!ok)
        {
                send_to_char("You must wield a vorpal to decapitate!\n\r",ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You must be engaged in combat.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't decapitate an object.\n\r", ch);
                return;
        }

        if( IS_NPC(victim) && !HAS_HEAD(victim) )
        {
                act( "$N doesn't have a head to chop off!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_UNKILLABLE))
        {
                send_to_char("That won't work on them.\n\r", ch);
                return;
        }

        if (victim->hit > victim->max_hit / 5)
        {
                act ("$N's reflexes are still too good.", ch, NULL, victim, TO_CHAR);
                return;
        }

        chance = 0;

        if (!IS_NPC (ch))
        {
                chance = ch->pcdata->learned[gsn_decapitate];
                chance += (ch->level - victim->level) * 2;
        }

        if (chance < 5)
                chance = 5;

        if (chance > 95)
                chance = 95;

        act ("{WYou swing your vorpal blade at $N...{x", ch, NULL, victim, TO_CHAR);
        WAIT_STATE(ch, skill_table[gsn_decapitate].beats);

        if (number_percent() < chance)
        {
                act ("{B$C's head is detached from $S neck!!{x", ch, NULL, victim, TO_CHAR);
                act ("{B$c swings $s blade at $N, causing $N's head to separate from $S body and fly away!!{x",
                     ch, NULL, victim, TO_NOTVICT);
                act ("{B$c DECAPITATES you!!{x", ch, NULL, victim, TO_VICT);

                if (IS_NPC(victim))
                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                victim->short_descr);
                else
                        sprintf(msg, "%s's severed head rolls around in front of you.",
                                victim->name);

                act (msg, ch, NULL, NULL, TO_ROOM);
                arena_commentary("$c decapitates $N!", ch, victim);

                vnum = OBJ_VNUM_SEVERED_HEAD;

                if (vnum != 0)
                {
                        OBJ_DATA *obj;
                        char      *name;
                        char      buf [ MAX_STRING_LENGTH ];

                        name = IS_NPC(victim) ? victim->short_descr : victim->name;
                        obj = create_object(get_obj_index(vnum), 0, "common", CREATED_NO_RANDOMISER);
                        obj->timer = number_range(4, 7);
                        obj->timermax = obj->timer;

                        sprintf(buf, obj->short_descr, name);
                        free_string(obj->short_descr);
                        obj->short_descr = str_dup(buf);

                        sprintf(buf, obj->description, name);
                        free_string(obj->description);
                        obj->description = str_dup(buf);

                        obj_to_room(obj, ch->in_room);
                }

                damage(ch,victim,victim->max_hit/2,gsn_decapitate, FALSE);
                return;
        }
        else
        {
                send_to_char("You failed.\n\r",ch);
                return;
        }
}


void do_snap_neck (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA   *victim;
        char        arg [ MAX_INPUT_LENGTH ];
        int         chance;

        if (!IS_NPC(ch)  && !CAN_DO(ch, gsn_snap_neck))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You must be engaged in combat.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument, arg);

        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't snap the neck of an object.\n\r", ch);
                return;
        }

        if( IS_NPC(victim) && !HAS_HEAD(victim) )
        {
                act( "$N has no neck for you to break!\n\r", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (victim->hit > victim->max_hit / 5)
        {
                act ("$N's reflexes are still too good.", ch, NULL, victim, TO_CHAR);
                return;
        }

        chance = 0;

        if (!IS_NPC (ch))
        {
                chance = ch->pcdata->learned[gsn_snap_neck]/2;
                chance += get_curr_dex(ch);
                chance += (ch->level - victim->level) *2;
        }

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
                chance = ( chance /2 );
        }

        act ("You attempt to snap $N's spinal column at the neck...",
             ch, NULL, victim, TO_CHAR);
        WAIT_STATE(ch, skill_table[gsn_snap_neck].beats);

        if (number_percent() < chance)
        {
                act ("You grab $N's head and violently wrench it around.... *CRACK*!!",
                     ch, NULL, victim, TO_CHAR);
                act ("$n grabs $N's head and twists violently... *CRACK*!!",
                     ch, NULL, victim, TO_NOTVICT);
                act ("$n violently twists your head!!", ch, NULL, victim, TO_VICT);
                arena_commentary("$n snaps $N's neck.", ch, victim);

                damage(ch, victim, victim->max_hit/2, gsn_snap_neck, FALSE);
                return;
        }
        else
        {
                send_to_char("You missed.\n\r",ch);
                act ("$n attempts to break $N's neck, but fails.", ch, NULL, victim, TO_ROOM);
                return;
        }
}


void do_stun (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        CHAR_DATA *victim;
        AFFECT_DATA af;
        char arg [MAX_INPUT_LENGTH];
        int chance = 0;

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_stun))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You can't try to stun anything while fighting!\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_mount))
        {
                send_to_char("Not while mounted.\n\r", ch);
                return;
        }

        if ((!(obj = get_eq_char(ch, WEAR_WIELD)) || !is_blunt_weapon(obj))
            && (!(obj = get_eq_char(ch, WEAR_DUAL)) || !is_blunt_weapon(obj)))
        {
                send_to_char("You need a weapon that pounds, blasts or crushes in order to stun.\n\r", ch);
                return;
        }

        if (!check_blind (ch))
                return;

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Who do you want to try and knock out?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("How can you knock yourself out?\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't stun an object.\n\r", ch);
                return;
        }

        if (!HAS_HEAD (victim))
        {
                act ("$C has no head for you to whack!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_HUGE (victim))
        {
                act ("$C is too large for you to stun!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (is_affected(victim, gsn_stun))
        {
                send_to_char("They are already stunned.\n\r",ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot stun a fighting person.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (is_affected(victim, gsn_gaias_warning))
        {
                act ("You attempt to stun $N, but they deftly step aside.",
                     ch, NULL, victim, TO_CHAR);
                act ("Sensing impending danger, you step to one side, just as $n's bludgeon flashes by.",
                     ch, NULL, victim, TO_VICT);
                act ("$n's attempted stun is thwarted by $N's acute awareness.",
                     ch, NULL, victim, TO_NOTVICT);
                damage(ch, victim, 0, gsn_stun, FALSE);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_stun].beats);

        if (!IS_NPC(ch))
        {
                chance = ch->pcdata->learned[gsn_stun];
                chance += (ch->level - victim->level) * 5;

                if (chance < 5)
                        chance = 5;

                if (chance > 95)
                        chance = 95;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_CLAN_GUARD))
        {
                damage(ch, victim, 0, gsn_stun, FALSE);
                return;
        }

        WAIT_STATE (ch, PULSE_VIOLENCE);

        if (IS_NPC(ch) || number_percent() < chance)
        {
                act ("You viciously pound $N, causing $M to buckle and collapse.",
                     ch, NULL, victim, TO_CHAR);
                act ("You legs buckles and you collapse, unable to arise.", ch, NULL, victim, TO_VICT);
                act ("$n walks up and pounds $N on the head causing $N to collapse.",
                     ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n stuns $N.", ch, victim);

                af.type = gsn_stun;
                af.modifier = 0;
                af.location = APPLY_NONE;
                af.duration = 0;
                af.bitvector = AFF_SLEEP;
                affect_to_char(victim,&af);

                do_sleep(victim,"");

                check_group_bonus(ch);

                WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
        }
        else
                damage(ch, victim, 0, gsn_stun, FALSE);
}


void do_disarm (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];
        int        percent;


        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from disarming anyone here.<0>\n\r", ch);
                return;
        }

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_disarm))
        {
                send_to_char("You don't know how to disarm opponents.\n\r", ch);
                return;
        }

        if (!get_eq_char(ch, WEAR_WIELD))
        {
                send_to_char("You must wield a weapon to disarm.\n\r", ch);
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

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim->fighting != ch && ch->fighting != victim)
        {
                act ("$E is not fighting you!", ch, NULL, victim, TO_CHAR);
                return;
        }

        obj = get_eq_char(victim, WEAR_WIELD);

        if (!obj)
        {
                send_to_char("{WYour opponent is not wielding a weapon.{x\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
        {
                send_to_char("You cannot disarm your opponent's body parts.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, PULSE_VIOLENCE);
        percent = number_percent() + victim->level - ch->level;

        if (IS_NPC(ch)|| percent < ch->pcdata->learned[gsn_disarm] * 2 / 3)
                disarm(ch, victim);

        else
                send_to_char("<51>Your disarm attempt failed.<0>\n\r", ch);
        return;
}


void do_trip (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        int chance;

        one_argument(argument,arg);

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_trip))
        {
                send_to_char("Tripping?  You'd be better off tying their shoelaces together!\n\r",ch);
                return;
        }

        if (IS_NPC(ch) && !HAS_LEGS(ch))
                return;

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument,arg);

        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r",ch);
                return;
        }

        if (is_safe(ch,victim))
                return;


        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("You can't trip up an OBJECT, weirdo.\n\r", ch);
                return;
        }

        if( IS_NPC(victim) && !HAS_LEGS(victim) )
        {
                act( "$N has no legs for you to trip up!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (victim->mount)
        {
                send_to_char ("You can't trip a mounted player.\n\r", ch);
                return;
        }

        if (ch->mount)
        {
                send_to_char ("You can't trip while mounted.\n\r", ch);
                return;
        }

        if ( ch->in_room->sector_type == SECT_UNDERWATER
        ||   ch->in_room->sector_type == SECT_WATER_SWIM
        ||   ch->in_room->sector_type == SECT_WATER_NOSWIM )
        {
                send_to_char ("You can't trip in water without firm ground beneath you.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim,AFF_FLYING))
        {
                act ("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim->position < POS_FIGHTING)
        {
                act ("$N is already down.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You fall flat on your face!\n\r", ch);
                WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
                act ("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
        {
                act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
                return;
        }

        chance = 0;

        if (!IS_NPC(ch))
        {
                chance = ch->pcdata->learned[gsn_trip]/2;
                chance += get_curr_dex(ch);
                chance += (ch->level - victim->level) * 2;
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
                chance /= 2;

        WAIT_STATE (ch, 2 * PULSE_VIOLENCE );

        if (IS_NPC(ch) || number_percent() < chance)
        {
                act ("<15>You trip $N and $E goes down!<0>", ch, NULL, victim, TO_CHAR);
                act ("$n trips you and you go down!", ch, NULL, victim, TO_VICT);
                act ("$n trips $N and $E goes down!", ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n trips up $N.", ch, victim);
               /* trip(ch, victim); */
                prone(ch,victim, gsn_trip, 1);
        }
        else
                send_to_char("You failed.\n\r", ch);
        return;
}


void do_grapple (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        int chance;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_kappa") )
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_green_grung") )
        && !( ch->spec_fun == spec_lookup("spec_purple_grung") )
        && !( ch->spec_fun == spec_lookup("spec_orange_grung") )
        && !( ch->spec_fun == spec_lookup("spec_gold_grung") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_infantry") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_cavalry") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ))
                return;

        one_argument(argument,arg);

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_grapple))
        {
                send_to_char("Huh?\n\r",ch);
                return;
        }

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument,arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r",ch);
                return;
        }

        if (is_safe(ch,victim))
                return;

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("Wrestling moves don't work on objects.\n\r", ch);
                return;
        }

        if (IS_HUGE (victim))
        {
                act ("$C is too large to grapple!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_AFFECTED(victim,AFF_FLYING))
        {
                act ("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim->position < POS_FIGHTING)
        {
                act ("$N is already down.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You fall flat on your face!\n\r", ch);
                WAIT_STATE(ch, 2 * skill_table[gsn_grapple].beats);
                act ("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
        {
                act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
                return;
        }

        chance = 0;

        if (!IS_NPC(ch))
        {
                chance = ch->pcdata->learned[gsn_grapple];
                chance += (ch->level - victim->level) * 2;
        }

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
                chance = ( ( chance / 3 ) * 2 );
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
                chance = ( ( chance / 3 ) * 2 );
        }

        WAIT_STATE(ch,skill_table[gsn_grapple].beats);

        if ((IS_NPC(ch) || number_percent() < chance)
            && (victim->position == POS_FIGHTING) && (ch->dazed < 3))
        {
                act ("You {Wgrapple{x $N down, winding $M.", ch, NULL, victim, TO_CHAR);
                act ("$n {Wgrapples{x you down!", ch, NULL, victim, TO_VICT);
                act ("$n {Wgrapples{x $N down!", ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n grapples $N to the ground.", ch, victim);

                WAIT_STATE (ch,        2 * PULSE_VIOLENCE);
                WAIT_STATE (victim,    2 * PULSE_VIOLENCE);
              /*  one_hit (ch, victim, gsn_grapple); */
                daze(ch,victim, gsn_grapple, 1);
        }
        else if (ch->dazed >= 3)
                send_to_char("Your opponent has learnt to avoid your grapple.\n\r",ch);
        else
                send_to_char("Your grapple fails.\n\r",ch);
}


/*
 * Flying Headbutt.  Grapple that does damage and hits flying people.
 * Shade - April 99
 */
void do_flying_headbutt (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
        int chance;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_kappa") )
        && !( ch->spec_fun == spec_lookup("spec_green_grung") )
        && !( ch->spec_fun == spec_lookup("spec_purple_grung") )
        && !( ch->spec_fun == spec_lookup("spec_orange_grung") )
        && !( ch->spec_fun == spec_lookup("spec_gold_grung") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_infantry") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_cavalry") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_flying_headbutt))
        {
                send_to_char("Huh?\n\r",ch);
                return;
        }

        if (! ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
                return;

        one_argument(argument,arg);
        victim = ch->fighting;

        /*
         * Moved to before the head/huge checks
         */

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r",ch);
                return;
        }

        if (!HAS_HEAD (victim))
        {
                act ("$C has no head for you to butt!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (IS_HUGE (victim))
        {
                act ("$C is too large for you to headbutt!", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim->position < POS_FIGHTING)
        {
                act ("$N is already down.",ch,NULL,victim,TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You leap up and fall face-first!\n\r", ch);
                WAIT_STATE(ch, 2 * skill_table[gsn_flying_headbutt].beats);
                act ("$n leaps up and crashes down in a pile!", ch, NULL, NULL, TO_ROOM);
                return;
        }

        chance = 0;

        if (!IS_NPC(ch))
                chance = ch->pcdata->learned[gsn_flying_headbutt];

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your head is far too damaged to be smashing it into things!\n\r", ch);
                return;
        }

        WAIT_STATE(ch,skill_table[gsn_flying_headbutt].beats);

        if ((IS_NPC(ch) || number_percent() < chance )
            && victim->position == POS_FIGHTING)
        {
                act ("You leap upand {Cheadbutt{x $N down!",
                     ch, NULL, victim, TO_CHAR);
                act ("$n leaps at you and {Cheadbutts{x you down!",
                     ch, NULL, victim, TO_VICT);
                act ("$n leaps up and {Cheadbutts{x $N down!",
                     ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n's flying headbutt knocks $N to the ground.", ch, victim);

                WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
                WAIT_STATE (victim, 2 * PULSE_VIOLENCE);

                damage(ch, victim, number_range(ch->level, ch->level * 2), gsn_flying_headbutt,
                       FALSE);

                if (!ch->fighting)
                    return;

                one_hit(ch, victim, gsn_grapple, FALSE); /* grapple so that msg is extra attack not flying headbutt */
        }
        else
        {
                send_to_char("Your flying headbutt misses and you crash in a heap.\n\r", ch);
                act ("$n flies through the air and crashes in a heap!", ch, NULL, victim,
                     TO_NOTVICT);
        }
}


/*
 * Mobs using magical items by Spitfire from merc mailing list .
 * Modified to give every magical item an equal chance of being used plus
 * eating pills -Kahn
 *
 * Made smarter by Gezhp 2000
 */
void use_magical_item(CHAR_DATA *ch)
{
        OBJ_DATA *obj;
        OBJ_DATA *cobj = NULL;
        int number = 0;
        char buf [MAX_INPUT_LENGTH];

        const int sn_dispel_magic = skill_lookup("dispel magic");

        for (obj = ch->carrying; obj; obj = obj->next_content)
        {
                if ((obj->item_type == ITEM_SCROLL
                     || obj->item_type == ITEM_WAND
                     || obj->item_type == ITEM_STAFF
                     || obj->item_type == ITEM_POTION
                     || obj->item_type == ITEM_PAINT
                     || obj->item_type == ITEM_PILL)
                    && number_range(0, number) == 0)
                {
                        cobj = obj;
                        number++;
                }
        }

        if (!cobj || !ch->fighting)
                return;

        switch (cobj->item_type)
        {
            case ITEM_SCROLL:
                if (cobj->wear_loc == WEAR_HOLD)
                        do_recite(ch, "scroll");
                return;

            case ITEM_POTION:
                sprintf(buf, "%s", cobj->name);
                do_quaff(ch, buf);
                return;

            case ITEM_PAINT:
                sprintf(buf, "%s", cobj->name);
                do_smear(ch, buf);
                return;

            case ITEM_PILL:
                sprintf(buf, "%s", cobj->name);
                do_eat(ch, buf);
                return;

            case ITEM_WAND:
                if (cobj->wear_loc == WEAR_HOLD)
                {
                        if (cobj->value[3] < 0
                            || cobj->value[3] >= MAX_SKILL)
                                break;

                        /*
                         * Hack: dispel_magic is not defined as an offensive spell,
                         * so mob targets itself.  We want it to target its
                         * opponent.  Gezhp
                         */
                        if (cobj->value[3] == sn_dispel_magic)
                        {
                                sprintf(buf, "%s", ch->fighting->name);
                                do_zap(ch, buf);
                                return;
                        }

                        switch (skill_table[cobj->value[3]].target)
                        {
                            case TAR_CHAR_OFFENSIVE:
                            case TAR_IGNORE:
                                sprintf(buf, "%s", ch->fighting->name);
                                do_zap(ch, buf);
                                break;

                            case TAR_CHAR_DEFENSIVE:
                            case TAR_CHAR_SELF:
                                sprintf(buf, "%s", ch->name);
                                do_zap(ch, buf);
                                break;
                        }
                }
                break;

            case ITEM_STAFF:
                if (cobj->wear_loc == WEAR_HOLD)
                {
                        if (cobj->value[3] < 0
                            || cobj->value[3] >= MAX_SKILL)
                                break;
                        switch (skill_table[cobj->value[3]].target)
                        {
                            case TAR_CHAR_OFFENSIVE:
                            case TAR_IGNORE:
                                do_brandish(ch, "");
                                break;
                        }
                }
                break;

        }
}


void do_sla (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *rch;

        rch = get_char(ch);

        if (!authorized(rch, gsn_slay))
                return;

        send_to_char("If you want to SLAY, spell it out.\n\r", ch);
        return;
}


void do_slay (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        CHAR_DATA *rch;
        char arg [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        rch = get_char(ch);

        if (!authorized(rch, gsn_slay))
                return;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Slay whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (ch == victim)
        {
                send_to_char("Suicide is a mortal sin.\n\r", ch);
                return;
        }

        if (!IS_NPC(victim) && victim->level >= ch->level)
        {
                send_to_char("You failed.\n\r", ch);
                return;
        }

        act ("You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR);
        act ("$n slays you in cold blood!", ch, NULL, victim, TO_VICT);
        act ("$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT);

        if (victim->rider)
        {
                send_to_char ("Your mount collapses, throwing you off it!\n\r",
                              victim->rider);
                act ("$c collapses, throwing $N off!", victim, NULL,
                     victim->rider, TO_NOTVICT);
                victim->rider->position = POS_RESTING;
                strip_mount (victim->rider);
        }

        if (victim->mount)
        {
                act ("$c falls from $s mount.", victim, NULL, NULL, TO_ROOM);
                victim->position = POS_RESTING;
                strip_mount (victim);
        }

        raw_kill (ch, victim, TRUE);

        if (!IS_NPC(victim))
        {
                sprintf(buf, "OH NO!!  %s has been SLAIN by %s!\n\r",
                        victim->name, ch->name);
                do_info(victim, buf);
        }

        return;
}

void do_swoop (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if(ch->form != FORM_PHOENIX)
        {
                send_to_char("You need to be in phoenix form to swoop.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_swoop))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't in combat with anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room  (ch, arg)))
        {
                send_to_char("Swoop whom?\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You can't swoop yourself!\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        WAIT_STATE(ch, PULSE_VIOLENCE);

        send_to_char("{RYou spread your flaming wings and swoop at your opponent!{x\n\r", ch);
        act ("$c swoops towards $N with $s flaming wings!", ch, NULL, victim, TO_ROOM);

        if (number_percent () < ch->pcdata->learned[gsn_swoop])
        {
                arena_commentary("$n swoops at $N in their phoenix form!", ch, victim);

                WAIT_STATE(ch, 1.5 * PULSE_VIOLENCE);
                one_hit(ch,victim,gsn_swoop, FALSE);

                if (number_percent () < ( ch->pcdata->learned[gsn_swoop] / 3))
                {
                        send_to_char("{WYou circle back to swoop your opponent a second time!{x\n\r", ch);
                        act ("$c attacks $N a second time with $s flaming wings!", ch, NULL, victim, TO_ROOM);
                        one_hit(ch,victim,gsn_swoop, FALSE);
                }

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;
        }
        else
                damage (ch, victim, 0, gsn_swoop, FALSE);
}


void do_whirlwind (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int count = 0;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_orange_grung") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_whirlwind) && ch->form != FORM_HYDRA)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if ( is_affected(ch,gsn_mount) )
        {
                send_to_char("You cannot do that while mounted.\n\r", ch);
                return;
        }

        if (!check_blind(ch))
        {
                send_to_char("You cannot see!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        for (vch = ch->in_room->people; vch; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (ch == vch)
                        continue;

                if (vch->deleted)
                        continue;

                if (!IS_NPC(vch) && ch->fighting != vch)
                        continue;

                if (is_same_group(vch, ch))
                        continue;

                if (is_group_members_mount(vch, ch))
                        continue;

                if (!can_see(ch, vch))
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if (count == 0)
                {
                        act("$n flails around in a fury!", ch, NULL, NULL, TO_ROOM);
                        arena_commentary("$n whirlwinds.", ch, ch);
                        count = 1;
                }

                act ("You attack $N in a frenzy of bloodlust!", ch, NULL, vch, TO_CHAR);
                WAIT_STATE(ch, skill_table[gsn_whirlwind].beats);
                one_hit(ch, vch, gsn_whirlwind, FALSE);
        }
}


void do_hurl (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        CHAR_DATA *victim;
        char arg1 [ MAX_INPUT_LENGTH ];
        char buf[MAX_STRING_LENGTH];
        char buf2[MAX_STRING_LENGTH];


        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_hurl))
        {
                send_to_char("You better leave that to a smithy.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg1);
        victim = ch->fighting;

        if (IS_AFFECTED(ch, AFF_BLIND))
        {
                send_to_char("You can't see anything!\n\r", ch);
                return;
        }

        if (arg1[0] == '\0')
        {
                send_to_char("Hurl what?\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You can't hurl anything at yourself!\n\r", ch);
                return;
        }

        if (!(obj = get_obj_wear(ch, arg1)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        if (! IS_SET(obj->ego_flags, EGO_ITEM_CHAINED))
        {
                send_to_char("You're not going to throw away something thats not chained.\n\r", ch);
                return;
        }

        if ( obj->item_type != ITEM_WEAPON) /*do this if hurling a shield */
        {
                int chance;
                if (is_affected(victim, gsn_stun))
                {
                        sprintf(buf2,"It looks like %s is already stunned.\n\r",
                                (IS_NPC(victim)) ? victim->short_descr : victim->name);
                        send_to_char(buf2,ch);
                        return;
                }

                if (is_safe(ch, victim))
                        return;

                WAIT_STATE(ch, skill_table[gsn_hurl].beats);

                if (!IS_NPC(ch))
                {
                        chance = ch->pcdata->learned[gsn_hurl];
                        chance += (ch->level - victim->level) * 5;

                        if (chance < 5)
                                chance = 5;

                        if (chance > 95)
                                chance = 95;
                }

                WAIT_STATE (ch, PULSE_VIOLENCE);

                if (IS_NPC(ch) || number_percent() < chance)
                {
                        act ("You hurl your shield. It slams into the side of $S head.. THUNK!!",
                        ch, NULL, victim, TO_CHAR);
                        act ("You're knocked out cold by a shield to the head.", ch, NULL, victim, TO_VICT);
                        act ("$n hurls their shield at $N. It slams into the side of $S head... THUNK!",
                        ch, NULL, victim, TO_NOTVICT);
                        arena_commentary("$n knocks $N out cold.", ch, victim);
                        daze(ch,victim, gsn_hurl, 1);
                }
                else
                {
                        damage(ch, victim, 0, gsn_stun, FALSE);
                }
        }
        else if ( obj->item_type == ITEM_WEAPON) /* do this is hurling a weapon */
        {

                WAIT_STATE(ch, PULSE_VIOLENCE);
                sprintf (buf, "You hurl {W%s{x at %s.\n\r", obj->short_descr, victim->short_descr);
                send_to_char(buf, ch);

                if ((!IS_AWAKE(victim) || !IS_NPC (ch))
                && number_percent () < ch->pcdata->learned[gsn_hurl])
                {
                        arena_commentary("$n hurls their equipment at $N.", ch, victim);

                        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                        damage(ch, victim, (ch->level / 2) + number_range(ch->level, ch->level * 2), gsn_hurl, FALSE);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                return;
                }
                else
                {
                        damage (ch, victim, 0, gsn_hurl, FALSE);
                }
        }
}


void do_shoot (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj, *objWield, *objShield, *objDual;
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];
        int num, i;

        /* Made this available to NPCs --Owl 29/3/24 */

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_shoot))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You're too close to shoot at anyone!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Shoot whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("Shoot whom?\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You can't shoot at yourself!\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You can't shoot a fighting person.\n\r", ch);
                return;
        }

        obj = get_eq_char(ch, WEAR_RANGED_WEAPON);

        if (!obj || !IS_OBJ_STAT(obj, ITEM_BOW))
        {
                send_to_char("You must have a bow equipped to shoot.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
                send_to_char("Your arm is too damaged to operate a weapon like that.\n\r", ch);
                return;
        }

        /* Remove weapons and shield, equip bow */
        objWield = get_eq_char(ch, WEAR_WIELD);
        if (objWield)
                unequip_char(ch, objWield);

        objShield = get_eq_char(ch, WEAR_SHIELD);
        if (objShield)
                unequip_char(ch, objShield);

        objDual = get_eq_char(ch, WEAR_DUAL);
        if (objDual)
                unequip_char(ch, objDual);

        equip_char(ch, obj, WEAR_WIELD);
        WAIT_STATE(ch, PULSE_VIOLENCE);
        send_to_char("{WYou take aim and let loose!{x\n\r", ch);

        if (IS_NPC(ch))
        {
            for (i = 0; i < (ch->level / 30); i++)
                {
                        one_hit(ch, victim, gsn_shoot, FALSE);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                break;
                }

        }
        else if ( ( number_percent () < ch->pcdata->learned[gsn_shoot] )
        ||   ( IS_AFFECTED(victim, AFF_HOLD) ) )
        {
                /* Shot check won't fail if victim is trapped/snared - Owl 11/6/22 */

                arena_commentary("$n shoots a volley of missiles at $N.", ch, victim);

                num = 1;

                if ( ( number_percent() < ch->pcdata->learned[gsn_second_shot] )
                ||    ( ( IS_AFFECTED(victim, AFF_HOLD) )
                     && ( ch->pcdata->learned[gsn_second_shot]) ) )
                {
                        num++;
                }

                if ( ( number_percent() < ch->pcdata->learned[gsn_third_shot] )
                ||   ( ( IS_AFFECTED(victim, AFF_HOLD) )
                     && ( ch->pcdata->learned[gsn_third_shot]) ) )
                {
                        num++;
                }

                for (i = 0; i < num; i++)
                {
                        one_hit(ch, victim, gsn_shoot, FALSE);

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                                break;
                }
        }
        else
        {
                damage (ch, victim, 0, gsn_shoot, FALSE);
        }

        /*  Double check bow is still being held  */
        if (get_eq_char (ch, WEAR_WIELD))
        {
                unequip_char(ch, obj);
                equip_char(ch, obj, WEAR_RANGED_WEAPON);
        }

        if(objWield)
                equip_char(ch, objWield, WEAR_WIELD);

        if(objShield)
                equip_char(ch, objShield, WEAR_SHIELD);

        if(objDual)
                equip_char(ch, objDual, WEAR_DUAL);
}

void do_snap_shot (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj, *objWield, *objShield, *objDual;
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_snap_shot))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You're not fighting anyone!\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
        {
                send_to_char("Your arm is too damaged to operate that weapon.\n\r", ch);
                return;
        }

        victim = ch->fighting;

        obj = get_eq_char(ch, WEAR_RANGED_WEAPON);

        if (!obj || !IS_OBJ_STAT(obj, ITEM_BOW))
        {
                send_to_char("With what?\n\r", ch);
                return;
        }

        /* Remove weapons and shield, equip bow */
        objWield = get_eq_char(ch, WEAR_WIELD);
        if (objWield)
                unequip_char(ch, objWield);

        objShield = get_eq_char(ch, WEAR_SHIELD);
        if (objShield)
                unequip_char(ch, objShield);

        objDual = get_eq_char(ch, WEAR_DUAL);
        if (objDual)
                unequip_char(ch, objDual);

        equip_char(ch, obj, WEAR_WIELD);
        WAIT_STATE(ch, 2 * PULSE_VIOLENCE); /* change of equipment, we'll make the wait state count regardless of hit */

        send_to_char("{WYou quickly grab your bow and fire off a shot!{x\n\r", ch);

        if (number_percent () < 50 + (ch->pcdata->learned[gsn_snap_shot] / 2))
        {
                arena_commentary("$n shoots a quick snap shot at $N.", ch, victim);
                one_hit(ch, victim, gsn_snap_shot, FALSE);
        }
        else
        {
                damage (ch, victim, 0, gsn_snap_shot, FALSE);
        }

        /*  Double check bow is still being held  */
        if (get_eq_char (ch, WEAR_WIELD))
        {
                unequip_char(ch, obj);
                equip_char(ch, obj, WEAR_RANGED_WEAPON);
        }

        if(objWield)
                equip_char(ch, objWield, WEAR_WIELD);

        if(objShield)
                equip_char(ch, objShield, WEAR_SHIELD);

        if(objDual)
                equip_char(ch, objDual, WEAR_DUAL);
}


void do_thrust (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_thrust))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room  (ch, arg)))
        {
                send_to_char("Thrust whom?\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You cant thrust at yourself!\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        if (!(obj = get_eq_char(ch, WEAR_WIELD)) || !is_piercing_weapon(obj))
        {
                send_to_char("You can't use that weapon to thrust.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, PULSE_VIOLENCE);

        send_to_char("You lunge forward at your opponent.\n\r", ch);

        if (number_percent () < ch->pcdata->learned[gsn_thrust])
        {
                arena_commentary("$n thrusts a blade into $N.", ch, victim);

                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                one_hit(ch,victim,gsn_thrust, FALSE);

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;
        }
        else
                damage (ch, victim, 0, gsn_thrust, FALSE);
}


void do_tailwhip (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        int chance;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_tailwhip))
        {
                send_to_char("You cannot tailwhip.\n\r", ch);
                return;
        }

        if (ch->form != FORM_DRAGON)
        {
                send_to_char ("You aren't in the correct form.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char ("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room (ch, arg)))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                send_to_char("Your tail is too damaged to whip.\n\r", ch);
                return;
        }

        chance = ch->pcdata->learned[gsn_tailwhip];
        chance = UMIN (chance, 95);

        WAIT_STATE (ch, PULSE_VIOLENCE);

        if (number_percent() > chance)
        {
                act ("You whip your tail at $N but miss $M.", ch, NULL, victim, TO_CHAR);
                act ("$c whips $s tail at you but misses!", ch, NULL, victim, TO_VICT);
                act ("$c whips $s tail at $N but misses $M.", ch, NULL, victim, TO_NOTVICT);
                return;
        }

        act ("You whip your tail at $N and smash $M down!", ch, NULL, victim, TO_CHAR);
        act ("$c whips $s tail at you and smashes you down!",  ch, NULL, victim, TO_VICT);
        act ("$c whips $s tail at $N and smashes $M down!", ch, NULL, victim, TO_NOTVICT);
        arena_commentary("$n tailwhips $N down.", ch, victim);

        WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
        WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
        damage(ch, victim, number_range (20, ch->level * 2), gsn_tailwhip, FALSE);

        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                return;

        victim->position = POS_RESTING;
}

void do_flukeslap (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        int chance;

        if (!IS_NPC(ch))
        {
                send_to_char("Are you a whale? No. So, no.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char ("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (IS_AFFECTED(victim, AFF_SWALLOWED))
        {
                send_to_char ("You can't use your flukes to slap someone inside your body!\n\r", ch);
                return;
        }

        if (arg[0] != '\0' && !(victim = get_char_room (ch, arg)))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                send_to_char("Your flukes are too damaged to do that.\n\r", ch);
                return;
        }

        chance = 95;

        WAIT_STATE (ch, PULSE_VIOLENCE);

        if (number_percent() > chance)
        {
                act ("You slap your flukes at $N but miss $M.", ch, NULL, victim, TO_CHAR);
                act ("$c slaps $s flukes at you but misses!", ch, NULL, victim, TO_VICT);
                act ("$c slaps $s flukes at $N but misses $M.", ch, NULL, victim, TO_NOTVICT);
                return;
        }

        act ("You slap your flukes at $N and send $M reeling!", ch, NULL, victim, TO_CHAR);
        act ("$c slaps $s flukes at you and sends you reeling!",  ch, NULL, victim, TO_VICT);
        act ("$c slaps $s flukes at $N and sends $M reeling!", ch, NULL, victim, TO_NOTVICT);
        arena_commentary("$c flukeslaps $N and sends $M reeling.", ch, victim);

        WAIT_STATE (victim, 3 * PULSE_VIOLENCE);
        WAIT_STATE (ch, 3 * PULSE_VIOLENCE);
        damage(ch, victim, number_range (30, ch->level * 2), gsn_flukeslap, FALSE);

        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                return;

        victim->position = POS_RESTING;
}

void do_spit_mucus (CHAR_DATA *ch, char *argument)
{
        /* For Aboleth or similar creatures.  Basically a heavy 'stun' (waitstate) attack.  Does
        very little physical damage. */

        CHAR_DATA *victim;
        char arg [MAX_INPUT_LENGTH];
        int chance;

        if (!IS_NPC(ch))
        {
                send_to_char("You're the wrong kind of creature to be trying that.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char ("You aren't fighting anyone.\n\r", ch);
                return;
        }

        one_argument (argument, arg);
        victim = ch->fighting;

        if (arg[0] != '\0' && !(victim = get_char_room (ch, arg)))
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        chance = 95;

        WAIT_STATE (ch, PULSE_VIOLENCE);

        if (number_percent() > chance)
        {
                act ("You spit grey mucus at $N but miss $M.", ch, NULL, victim, TO_CHAR);
                act ("$c spits grey mucus at you but misses!", ch, NULL, victim, TO_VICT);
                act ("$c spits grey mucus at $N but misses $M.", ch, NULL, victim, TO_NOTVICT);
                return;
        }

        act ("You spit grey mucus at $N and $E is stunned!", ch, NULL, victim, TO_CHAR);
        act ("$c spits grey mucus at you and your mind FREEZES UP!",  ch, NULL, victim, TO_VICT);
        act ("$c spits grey mucus at $N and $E is stunned!", ch, NULL, victim, TO_NOTVICT);
        arena_commentary("$c spits grey mucus at $N and $E is STUNNED!", ch, victim);

        WAIT_STATE (victim, ((ch->level > 50 ) ? 50 : ch->level) );
        WAIT_STATE (ch, (ch->level/2));
        damage(ch, victim, number_range (0, (ch->level/2)), gsn_spit_mucus, FALSE);

        if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                return;

        victim->position = POS_RESTING;
}


void do_swallow (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char    arg [MAX_INPUT_LENGTH ];
        char    buf [MAX_INPUT_LENGTH ];
        int     chance;

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_swallow))
        {
                send_to_char("You don't know how to do that.\n\r", ch);
                return;
        }

        if (!ch->fighting)
        {
                send_to_char("You're not in combat.\n\r", ch);
                return;
        }

        one_argument(argument,arg);

        if (arg[0] == '\0')
        {
                victim = ch->fighting;
                if (!victim)
                {
                        send_to_char("But you aren't in combat!\n\r",ch);
                        return;
                }
        }

        else if (!(victim = get_char_room(ch,arg)))
        {
                send_to_char("They aren't here.\n\r",ch);
                return;
        }

        if( IS_NPC(victim) && IS_HUGE(victim) )
        {
                act( "You can't swallow $N, $E is much too large!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if( !IS_NPC(victim) && ( race_table[ch->race].size == 3 ) )
        {
                act( "You can't swallow $N, $E is much too large!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (IS_AFFECTED(victim, AFF_SWALLOWED))
        {
                act ("$E has already been swallowed!",ch,NULL,victim,TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("This universe does have SOME physical limitations.\n\r",ch);
                return;
        }

        if ( victim->fighting && !is_same_group( ch,victim->fighting ) )
        {
                send_to_char("Kill stealing is not permitted.\n\r",ch);
                return;
        }

        if ( IS_AFFECTED(victim,AFF_CHARM) && victim->master == ch )
        {
                act ("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
                return;
        }

        if (!IS_NPC(ch))
                chance = ch->pcdata->learned[gsn_swallow];
        else
                chance = 90;

        chance += (ch->level - victim->level) * 2;

        chance = URANGE(5, chance, 95);

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your mouth is too damaged to swallow.\n\r", ch);
                return;
        }

        if (number_percent() < chance)
        {
                AFFECT_DATA af;

                arena_commentary("$n swallows $N whole!", ch, victim);
                sprintf(buf, "<15>You swallow %s whole!<0>\n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name);
                send_to_char(buf, ch);
                act( "<15>$c swallows $N whole!<0>", ch, NULL, victim, TO_NOTVICT );
                send_to_char("You have been <132>SWALLOWED<0> and can't see a thing!\n\r", victim);

                victim->inside = ch;

                af.type      = gsn_swallow;
                af.duration  = ( ch->level / 75 ); /* 0 until lvl 75, 1 thereafter */

                af.location     = APPLY_HITROLL;
                af.modifier     = ( ch->level / -5 );
                af.bitvector    = AFF_BLIND;
                affect_to_char(victim,&af);

                af.location     = APPLY_DAMROLL;
                af.modifier     = ( ch->level / -5 );
                af.bitvector    = AFF_HOLD;
                affect_to_char(victim, &af);

                af.location     = APPLY_DEX;
                af.modifier     = ( ch->level / -10 );
                af.bitvector    = AFF_SWALLOWED;
                affect_to_char(victim, &af);

                af.location     = APPLY_MOVE;
                af.modifier     = ((ch->level * -10) + (ch->level * 2));
                af.bitvector    = AFF_NO_RECALL;

                affect_to_char(victim, &af);

                check_group_bonus(ch);

        }
        else{
                damage(ch, victim, 0, gsn_swallow, FALSE);
                return;
        }
        WAIT_STATE(ch, (PULSE_VIOLENCE * 2));
}


/*
 *  This code moved from damage() so that it can be reused by
 *  assassinate and disintegrate.  Gezhp 2000.
 *
 *  This function called when `opponent' kills `victim'.
 */
void check_player_death (CHAR_DATA *opponent, CHAR_DATA *victim)
{
        char buf [MAX_STRING_LENGTH];
        int i;

        if (IS_NPC(victim))
        {
                /*
                 * Check for bot deaths
                 */
                if (victim->pIndexData->vnum == BOT_VNUM
                    && IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        if (opponent != victim)
                        {
                                sprintf(buf, "%s has been MURDERED by %s at %s!",
                                        victim->name,
                                        IS_NPC(opponent) ? opponent->short_descr : opponent->name,
                                        victim->in_room->name);
                        }
                        else
                        {
                                sprintf(buf, "%s has died at %s!",
                                        victim->name,
                                        victim->in_room->name);
                        }

                        for (i = 0; i < bot_entry_count; i++)
                        {
                                if (bot_status_table[i].ch == victim)
                                {
                                        log_bot(i, "killed");
                                        bot_status_table[i].status = BOT_STATUS_DEAD;
                                        break;
                                }
                        }

                        tournament_message(buf, victim);

                        if (is_tournament_death(victim, opponent)
                            && is_still_alive_in_tournament(victim))
                        {
                                tournament_death(victim, opponent);
                        }

                }

                return;
        }

        stop_fighting(victim, FALSE);

        sprintf (log_buf, "%s (level %d) killed by %s (level %d) at %d%s",
                 victim->name,
                 victim->level,
                 IS_NPC(opponent) ? opponent->short_descr : opponent->name,
                 opponent->level,
                 victim->in_room->vnum,
                 IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
                 ? " [Arena]" : (!IS_NPC(victim) && !IS_NPC(opponent))
                 ? " [Pkill]" : "");
        log_string (log_buf);

        if (IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
        {
                if (opponent != victim)
                {
                        sprintf(buf, "%s has been MURDERED by %s at %s!",
                                victim->name,
                                IS_NPC(opponent) ? opponent->short_descr : opponent->name,
                                victim->in_room->name);
                }
                else
                {
                        sprintf(buf, "%s has died at %s!",
                                victim->name,
                                victim->in_room->name);
                }
                tournament_message(buf, victim);

                if (is_tournament_death(victim, opponent)
                    && is_still_alive_in_tournament(victim))
                {
                        tournament_death(victim, opponent);
                        return;
                }
        }
        else
        {
                if (opponent != victim)
                {
                        sprintf (buf, "ACK! %s has been killed by %s at %s!",
                                 victim->name,
                                 IS_NPC(opponent) ? opponent->short_descr : opponent->name,
                                 victim->in_room->name);
                }
                else
                {
                        sprintf (buf, "ACK! %s has died at %s!",
                                 victim->name,
                                 victim->in_room->name);
                }
                do_info (victim, buf);
        }

        if (!IS_NPC(opponent) && !IS_NPC(victim) && opponent != victim)
        {
                if (!IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        sprintf (buf, "OH NO!!  %s has been MURDERED by %s!!\n\r",
                                 victim->name,
                                 opponent->name);
                        do_info (victim, buf);
                        sprintf(buf, "You have just MURDERED %s!\n\r", opponent->name);
                }

                /*
                 *  Pkill points awarded if:
                 *  a) kill made outside arena, and
                 *  b) killer is in a clan or is ronin, and
                 *  c) victim is in a clan or is ronin, and
                 *  d) killer and victim are not in the same clan.
                 */
                if (!IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
                    && (opponent->clan || IS_SET(opponent->status, PLR_RONIN))
                    && (victim->clan || IS_SET(victim->status, PLR_RONIN)))
                {
                        if (victim->clan && victim->clan == opponent->clan)
                                log_string ("Clanmate has been murdered; no points awarded");
                        else
                        {
                                sprintf (log_buf, "Incrementing pkills...");
                                log_string (log_buf);
                                opponent->pcdata->pkills += 1;
                                victim->pcdata->pdeaths += 1;

                                /*  Calculate pkscore value for killer and update pkscore_table  */

                                if (opponent->level > victim->level)
                                {
                                        if (opponent->level - victim->level < 3)
                                                opponent->pcdata->pkscore += 3;
                                        else if (opponent->level - victim->level < 6)
                                                opponent->pcdata->pkscore += 2;
                                        else
                                                opponent->pcdata->pkscore +=1;
                                }
                                else opponent->pcdata->pkscore
                                        += ((((victim->level - opponent->level) / 5) + 1) * 2 + 1);

                                update_pkscore_table (opponent);
                                update_pkscore_table (victim);
                                check_clan_pkill_table (opponent);
                                check_clan_vanq_table (opponent, victim);
                        }
                }
        }
}


/*
 *  Check whether a character is within range for a pkill.
 *  Pkill ranges are:
 *  < 31 :  5 levels
 *  < 51 : 10 levels
 *  < 99 : 25 levels
 *   100 :  5 levels
 *
 *  Shade and Gezhp Nov 2000
 */

bool in_pkill_range (CHAR_DATA *ch, CHAR_DATA *victim)
{
        int range;

        if (IS_NPC(ch) || IS_NPC(victim) || ch->level < 20 || victim->level < 20)
                return FALSE;

        range = ch->level - victim->level;

        if (ch->in_room->area->low_level == -4
            && ch->in_room->area->high_level == -4
            && ch->level > 50
            && victim->level > 50)
        {
                return TRUE;
        }


        if (victim->level < 31 || ch->level < 31)
        {
                if (range >= -5 && range <= 5)
                        return TRUE;

                return FALSE;
        }

        if (victim->level < 51 || ch->level < 51)
        {
                if (range >= -10 && range <= 10)
                        return TRUE;

                return FALSE;
        }

        if (victim->level == LEVEL_HERO || ch->level == LEVEL_HERO)
        {
                if (range >= -5 && range <= 5)
                        return TRUE;

                return FALSE;
        }

        if (range >= -25 && range <= 25)
                return TRUE;

        return FALSE;
}


void reset_char_stats (CHAR_DATA *ch)
{
        if (IS_NPC(ch))
                return;

        ch->armor        = 100;
        ch->position     = POS_STANDING; /* Changed from POS_RESTING 7/4/22 -- Owl 7/4/22 */
        ch->hit          = 1;
        ch->mana         = 1;
        ch->move         = UMAX(1, ch->move);
        ch->aggro_dam    = 0;
        ch->hitroll      = 0;
        ch->damroll      = 0;
        ch->form         = 0;
        ch->saving_throw = 0;
        ch->carry_weight = 0;
        ch->carry_number = 0;
        ch->crit         = 5;
        ch->swiftness    = 5;
        ch->pcdata->mod_str = 0;
        ch->pcdata->mod_int = 0;
        ch->pcdata->mod_wis = 0;
        ch->pcdata->mod_dex = 0;
        ch->pcdata->mod_con = 0;

        if (ch->sub_class == SUB_CLASS_WEREWOLF)
                ch->rage = 0;

        if (ch->sub_class == SUB_CLASS_VAMPIRE)
                ch->rage = ch->max_rage;
}


/*
 *  Inflict identical amounts of regular and aggravated damage on ch;
 *  update position and check for death.  Return TRUE on death.
 *  Gezhp 2000
 */
bool aggro_damage (CHAR_DATA *ch, CHAR_DATA *victim, int damage)
{
        if (damage < 1)
                return FALSE;

        victim->hit -= damage;
        victim->aggro_dam += damage;

        if (!IS_NPC(victim) && victim->level > LEVEL_HERO)
        {
                if (victim->hit < 1)
                        victim->hit = 1;

                if (victim->max_hit - victim->aggro_dam < 1)
                        victim->aggro_dam = victim->max_hit - 1;
        }

        if (victim->hit < 1)
        {
                if ( IS_NPC(victim) && IS_SET(victim->act, ACT_UNKILLABLE))
                {
                        victim->hit = 1;
                        return FALSE;
                }

                /*
                 * If a mob you're grouped with killsteals, and you're a fights-with-mobs class,
                 * you get the reward. -- Owl 26/7/22
                 */
                if ( ( IS_NPC(ch) && ch->master )
                &&  ( ch->master->sub_class == SUB_CLASS_WITCH
                   || ch->master->sub_class == SUB_CLASS_INFERNALIST
                   || ch->master->sub_class == SUB_CLASS_NECROMANCER
                   || ch->master->sub_class == SUB_CLASS_KNIGHT
                   || ( ( ch->master->class == CLASS_SHAPE_SHIFTER )
                     && ( ch->master->sub_class == 0 ) )
                   || ch->master->sub_class == SUB_CLASS_WEREWOLF) )
                {
                        group_gain(ch->master, victim, TRUE);
                }
                else
                {
                        group_gain(ch, victim, FALSE);
                }

                check_player_death(ch, victim);
                death_penalty(ch, victim);
                raw_kill(ch, victim, TRUE);
                send_to_char("{RYou have been KILLED!!{x\n\r\n\r", victim);
                send_to_char(purgatory_message, victim);
                if ( IS_NPC(victim)
                    && ( IS_INORGANIC(victim) || IS_SET(victim->act, ACT_OBJECT) ) )
                {
                    act ("$c has been DESTROYED!!", victim, NULL, NULL, TO_ROOM);
                }
                else {
                    act ("$c is DEAD!!", victim, NULL, NULL, TO_ROOM);
                }
                check_autoloot(ch, victim);

                return TRUE;
        }

        return FALSE;
}


void check_autoloot (CHAR_DATA *ch, CHAR_DATA *victim)
{
        if (!IS_NPC(ch)
            && IS_NPC(victim)
            && MAKES_CORPSE(victim))
        {
                if (IS_SET(ch->act, PLR_AUTOLOOT))
                {
                    if (IS_SET(victim->act, ACT_OBJECT))
                    {
                        do_get (ch, "all remains");
                    }
                    else {
                        do_get (ch, "all corpse");
                    }
                }
                else
                {
                    if (IS_SET(ch->act, PLR_AUTOCOIN))
                    {
                        if (IS_SET(victim->act, ACT_OBJECT))
                        {
                            do_get_coins (ch, "remains");
                        }
                        else {
                            do_get_coins (ch, "corpse");
                        }
                        if (IS_SET(victim->act, ACT_OBJECT))
                        {
                            do_look (ch, "in remains");
                        }
                        else {
                            do_look (ch, "in corpse");
                        }
                    }
                }

                if (IS_SET(ch->act, PLR_AUTOSAC))
                {
                    if (IS_SET(victim->act, ACT_OBJECT))
                    {
                        do_sacrifice (ch, "remains");
                    }
                    else {
                        do_sacrifice (ch, "corpse");
                    }
                }
        }
}


char *get_damage_string( int damage_value, bool is_singular )
{
        /*
        * Get the damage string for the amount of damage you're doing.
        * takes damage amount and TRUE/FALSE depending on if you want
        * a singular or plural string returned.
        * --Owl 12/7/22
        */

        char *vs;
        char *vp;

        if (damage_value == 0)

        {
                vs = "<0><240>miss<0>";
                vp = "<0><240>misses<0>";
        }
        else if (damage_value <= 8)
        {
                vs = "<0><241>scratch<0>";
                vp = "<0><241>scratches<0>";
        }
        else if (damage_value <= 16)
        {
                vs = "<0><242>graze<0>";
                vp = "<0><242>grazes<0>";
        }
        else if (damage_value <= 24)
        {
                vs = "<0><243>hit<0>";
                vp = "<0><243>hits<0>";
        }
        else if (damage_value <= 32)
        {
                vs = "<0><244>injure<0>";
                vp = "<0><244>injures<0>";
        }
        else if (damage_value <= 40)
        {
                vs = "<0><245>wound<0>";
                vp = "<0><245>wounds<0>";
        }
        else if (damage_value <= 48)
        {
                vs = "<0><246>maul<0>";
                vp = "<0><246>mauls<0>";
        }
        else if (damage_value <= 56)
        {
                vs = "<0><247>decimate<0>";
                vp = "<0><247>decimates<0>";
        }
        else if (damage_value <= 64)
        {
                vs = "<0><248>mangle<0>";
                vp = "<0><248>mangles<0>";
        }
        else if (damage_value <= 72)
        {
                vs = "<0><249>maim<0>";
                vp = "<0><249>maims<0>";
        }
        else if (damage_value <= 80)
        {
                vs = "<0><250>MUTILATE<0>";
                vp = "<0><250>MUTILATES<0>";
        }
        else if (damage_value <= 88)
        {
                vs = "<0><251>DISEMBOWEL<0>";
                vp = "<0><251>DISEMBOWELS<0>";
        }
        else if (damage_value <= 96)
        {
                vs = "<0><253>EVISCERATE<0>";
                vp = "<0><253>EVISCERATES<0>";
        }
        else if (damage_value <= 104)
        {
                vs = "<0><255>MASSACRE<0>";
                vp = "<0><255>MASSACRES<0>";
        }
        else if (damage_value <= 120)
        {
                vs = "<0><196>*** DEMOLISH ***<0>";
                vp = "<0><196>*** DEMOLISHES ***<0>";
        }
        else if (damage_value <= 140)
        {
                vs = "<0><160>***<0> <196>DEVASTATE<0> <160>***<0>";
                vp = "<0><160>***<0> <196>DEVASTATES<0> <160>***<0>";
        }
        else if (damage_value <= 180)
        {
                vs = "<0><124>***<0> <196>ANNIHILATE<0> <124>***<0>";
                vp = "<0><124>***<0> <196>ANNIHILATES<0> <124>***<0>";
        }
        else if (damage_value <= 250)
        {
                vs = "<0><124>-=<<**<160><558>OBLITERATE<0><124>**>=-<0>";
                vp = "<0><124>-=<<**<160><558>OBLITERATES<0><124>**>=-<0>";
        }
        else if (damage_value <= 400)
        {
                vs = "<0><88>-=<<||<160><558>RAVAGE<0><88>||>=-<0>";
                vp = "<0><88>-=<<||<160><558>RAVAGES<0><88>||>=-<0>";
        }
        else if (damage_value <= 600)
        {
                vs = "<0><52>-=*>|<160><558>CRIPPLE<559><0><52>|<<*=-<0>";
                vp = "<0><52>-=*>|<160><558>CRIPPLES<559><0><52>|<<*=-<0>";
        }
        else if (damage_value <= 900)
        {
                vs = "<0><124>-=**>><124><558>BRUTALISE<559><0><124><<<<**=-<0>";
                vp = "<0><124>-=**>><124><558>BRUTALISES<559><0><124><<<<**=-<0>";
        }
        else if (damage_value <= 1200)
        {
                vs = "<0><88>-=**>><124><558>VAPOURISE<559><0><88><<<<**=-<0>";
                vp = "<0><88>-=**>><124><558>VAPOURISES<559><0><88><<<<**=-<0>";
        }
        else if (damage_value <= 1500)
        {
                vs = "<0><52>-+*>>><124><558>ATOMIZE<559><0><52><<<<<<*+-<0>";
                vp = "<0><52>-+*>>><124><558>ATOMIZES<559><0><52><<<<<<*+-<0>";
        }
        else if (damage_value <= 2000)
        {
                vs = "<0><88>-+*>>##<558>ELIMINATE<559><0><88>##<<<<*+-<0>";
                vp = "<0><88>-+*>>##<558>ELIMINATES<559><0><88>##<<<<*+-<0>";
        }
        else if (damage_value <= 2500)
        {
                vs = "<0><52>-+*###<88><558>EXTERMINATE<559><0><52>###*+-<0>";
                vp = "<0><52>-+*###<88><558>EXTERMINATES<559><0><52>###*+-<0>";
        }
        else if (damage_value <= 3000)
        {
                vs = "<0><52>--=##>><558>UTTERLY DESTROY<559><52><<<<##=--<0>";
                vp = "<0><52>--=##>><558>UTTERLY DESTROYS<559><52><<<<##=--<0>";
        }
        else if (damage_value <= 3500)
        {
                vs = "<0><52>-=*<<|<556>[NULLIFY<52>]<557>|>*=-<0>";
                vp = "<0><52>-=*<<|<556>[NULLIFIES<52>]<557>|>*=-<0>";
        }
        else if (damage_value <= 4000)
        {
                vs = "<0><52>-=**[|<<<<<556>B U T C H E R<0><52>>>|]**=-<0>";
                vp = "<0><52>-=**[|<<<<<556>B U T C H E R S<0><52>>>|]**=-<0>";
        }
        else if (damage_value <= 4500)
        {
                vs = "<0><52>--=<<#[|<556><460><52>L I Q U I D A T E<0><52>|]#>=--<0>";
                vp = "<0><52>--=<<#[|<556><460><52>L I Q U I D A T E S<0><52>|]#>=--<0>";
        }
        else if (damage_value <= 5000)
        {
                vs = "<0><52>-=+<<##<556><424><52>S L A U G H T E R<0><52>##>+=-<0>";
                vp = "<0><52>-=+<<##<556><424><52>S L A U G H T E R S<0><52>##>+=-<0>";
        }
        else if (damage_value <= 5500)
        {
                vs = "<0><52>-=+*<<(|[ <556><388><196>E X T I R P A T E<0><52> ]|)>*+=-<0>";
                vp = "<0><52>-=+*<<(|[ <556><388><196>E X T I R P A T E S<0><52> ]|)>*+=-<0>";
        }
        else
        {
                vs = "<0><52>-+<<<<[[ <556><352><196>P A R T I C L I Z E<0><52> ]]>>+-<0>";
                vp = "<0><52>-+<<<<[[ <556><352><196>P A R T I C L I Z E S<0><52> ]]>>+-<0>";
        }

        if (is_singular == TRUE)
        {
                return vs;
        }
        else {
                return vp;
        }
}


char *get_dpr( int dam ) {
  int i;
  for (i = 0; i < MAX_DPR; i++) {
    if (dam < dprs[i].dam ) {
        return dprs[i].dpr_verb;
        }
  }
    return "nothing";

}

/* Helper function for do_target.  Given a ch pointer , an int representing the
   kind of body part to look for, and a percentage value, it will remove the
   first body part of bp_kind that CHAR is wearing with percent probability
   --Owl 25/9/24 */

void disarm_bodypart (CHAR_DATA *ch, CHAR_DATA *victim, int bp_kind)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for (obj = victim->carrying; obj; obj = obj_next)
    {
            obj_next = obj->next_content;

            if ( ( obj->wear_loc != WEAR_NONE )
            &&   ( IS_SET(obj->extra_flags, ITEM_BODY_PART ) )
            &&   ( ( obj->item_type == ITEM_LIGHT ) || ( obj->item_type == ITEM_WEAPON ) )
            &&   ( obj->value[0] == bp_kind ) )
            {
                    remove_bodypart(victim, obj->wear_loc, TRUE);
            }

            if ( ( obj->wear_loc != WEAR_NONE )
            &&   ( IS_SET(obj->extra_flags, ITEM_BODY_PART ) )
            &&   ( obj->item_type == ITEM_ARMOR )
            &&   ( obj->value[1] == bp_kind ) )
            {
                    remove_bodypart(victim, obj->wear_loc, TRUE);
            }
    }

}

bool remove_bodypart (CHAR_DATA *ch, int iWear, bool fReplace)
{
        OBJ_DATA *obj;
        int plural = 0;

        if (!(obj = get_eq_char(ch, iWear)))
                return TRUE;

        if (!fReplace)
                return FALSE;

        if( obj->item_type == ITEM_INSTRUMENT && obj->wear_loc == WEAR_HOLD )
                remove_songs( ch );

        size_t len = strlen(obj->name);

        if (len > 0 && (obj->name[len - 1] == 's' || obj->name[len - 1] == 'S'))
        {
            plural = 1;
        }

        if (!IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
	    {
                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);

                if (plural)
                {
                        act("$p are torn from $n's body!", ch, obj, NULL, TO_ROOM);
                        act("$p are torn from your body!", ch, obj, NULL, TO_CHAR);
                }
                else
                {
                        act("$p is torn from $n's body!", ch, obj, NULL, TO_ROOM);
                        act("$p is torn from your body!", ch, obj, NULL, TO_CHAR);
                }
                return TRUE;
	    }
        else {
                if (plural)
                {
                        act("$p refuse to separate from $n's body!", ch, obj, NULL, TO_ROOM);
                        act("$p refuse to separate from your body!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$p refuses to separate from $n's body!", ch, obj, NULL, TO_ROOM);
                        act("$p refuses to separate from your body!", ch, obj, NULL, TO_CHAR);
                }
                return FALSE;
        }
}


/* EOF fight.c */
