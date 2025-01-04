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
#include <string.h>
#include <time.h>
#include "merc.h"
/*
 * Externals
 */
extern  bool            merc_down;

/*
 * Globals
 */
bool    delete_obj;
bool    delete_char;

/*
 * Local functions.
 */
int     hit_gain        args( ( CHAR_DATA *ch ) );
int     mana_gain       args( ( CHAR_DATA *ch ) );
int     move_gain       args( ( CHAR_DATA *ch ) );
int     rage_gain       args( ( CHAR_DATA *ch ) );
int     meter_gain      args( ( CHAR_DATA *ch ) );
int     engrave_gain    args( ( CHAR_DATA *ch ) );
void    mobile_update   args( ( void ) );
void    weather_update  args( ( void ) );
void    char_update     args( ( void ) );
void    obj_update      args( ( void ) );
void    aggr_update     args( ( void ) );
void    quest_update    args( ( void ) ); /* Vassago - quest.c */
void	msdp_update	    args( ( void ) ); /* <--- GMCP */
void	gmcp_update	    args( ( void ) ); /* <--- GMCP */


/*
 * Advancement stuff.
 */


int check_stat_advance (CHAR_DATA *ch, int stat)
{
        switch (stat)
        {
            case APPLY_STR:
                if (ch->pcdata->perm_str < (21 + race_table[ch->race].str_bonus +
                                            class_table[ch->class].class_stats[0]))
                        return 1;

                break;

            case APPLY_INT:
                if (ch->pcdata->perm_int < (21 + race_table[ch->race].int_bonus +
                                            class_table[ch->class].class_stats[1]))
                        return 1;

                break;

            case APPLY_WIS:
                if (ch->pcdata->perm_wis < (21 + race_table[ch->race].wis_bonus +
                                            class_table[ch->class].class_stats[2]))
                        return 1;

                break;

            case APPLY_DEX:
                if (ch->pcdata->perm_dex < (21 + race_table[ch->race].dex_bonus +
                                            class_table[ch->class].class_stats[3]))
                        return 1;

                break;

            case APPLY_CON:
                if (ch->pcdata->perm_con < (21 + race_table[ch->race].con_bonus +
                                            class_table[ch->class].class_stats[4]))
                            return 1;

                break;

        }

        return 0;
}


void advance_stat (CHAR_DATA *ch)
{
        int type = -1;

        if (ch->level % 2)
                return;

        if (check_stat_advance(ch, ch->pcdata->stat_train))
                type = ch->pcdata->stat_train;

        else if (check_stat_advance(ch, class_table[ch->class].attr_prime))
                type = class_table[ch->class].attr_prime;

        else if (check_stat_advance(ch, APPLY_WIS))
                type = APPLY_WIS;

        else if (check_stat_advance(ch, APPLY_CON))
                type = APPLY_CON;

        else if (check_stat_advance(ch, APPLY_STR))
                type = APPLY_STR;

        else if (check_stat_advance(ch, APPLY_INT))
                type = APPLY_INT;

        else if (check_stat_advance(ch, APPLY_DEX))
                type = APPLY_DEX;

        else
                return; /* max stats */

        switch (type)
        {
            case APPLY_STR:
                ch->pcdata->perm_str += 1;
                send_to_char("{WYou feel stronger", ch);

                if (!check_stat_advance(ch, APPLY_STR))
                        send_to_char("; your strength is at maximum", ch);

                send_to_char(".{x\n\r", ch);

                break;

            case APPLY_INT:
                ch->pcdata->perm_int += 1;
                send_to_char("{WYou feel smarter", ch);

                if (!check_stat_advance(ch, APPLY_INT))
                        send_to_char("; your intelligence is at maximum", ch);

                send_to_char(".{x\n\r", ch);

                break;

            case APPLY_WIS:
                ch->pcdata->perm_wis += 1;
                send_to_char("{WYou feel wiser", ch);

                if (!check_stat_advance(ch, APPLY_WIS))
                        send_to_char("; your wisdom is at maximum", ch);

                send_to_char(".{x\n\r", ch);

                break;

            case APPLY_DEX:
                ch->pcdata->perm_dex += 1;
                send_to_char("{WYou feel more dexterous", ch);

                if (!check_stat_advance(ch, APPLY_DEX))
                        send_to_char("; your dexterity is at maximum", ch);

                send_to_char(".{x\n\r", ch);

                break;

            case APPLY_CON:
                ch->pcdata->perm_con += 1;
                send_to_char("{WYou feel fitter", ch);

                if (!check_stat_advance(ch, APPLY_CON))
                        send_to_char("; your constitution is at maximum", ch);

                send_to_char(".{x\n\r", ch);

                break;
        }
}


void advance_base (CHAR_DATA *ch)
{
        if (ch->sub_class)
                ch->pcdata->learned[LAST_BASE_CLASS_INDEX + ch->sub_class]++;
        else
                ch->pcdata->learned[gsn_mage_base + ch->class]++;
}


void advance_level( CHAR_DATA *ch )
{
        char buf [ MAX_STRING_LENGTH ];
        int add_hp;
        int add_mana;
        int add_move;
        int add_rage;
        int add_phys_prac;
        int add_int_prac;
        int i;
        double tmp;

        advance_stat(ch);
        advance_base(ch);

        add_hp = con_app[get_curr_con( ch )].hitp
                + number_range(class_table[ch->class].hp_min, class_table[ch->class].hp_max );

        add_mana = number_range(1, 10) + ((2 * get_curr_int(ch) + get_curr_wis(ch)) / 3);

        if (!class_table[ch->class].fMana && !sub_class_table[ch->sub_class].bMana)
                add_mana /= 3;

        add_move = number_range(5, (get_curr_con(ch) + get_curr_dex(ch)) /4);

        /* add pracs - physical = str/dex/wis avg andbonus if prime >= 27 */
        /*           - intellect = pure int/ (wis * 2)                    */

        tmp = ((double)get_curr_wis(ch) + (double)get_curr_str(ch)
               + (double)get_curr_dex(ch)) / 3;

        add_phys_prac = 1;
        if (tmp > 15) { add_phys_prac++; }
        if (tmp > 20) { add_phys_prac++; }
        if (tmp > 25) { add_phys_prac++; }

        tmp = (2 * (double)get_curr_wis(ch) + (double)get_curr_int(ch)) / 3;

        add_int_prac = 1;
        if (class_table[ch->class].fMana) { add_int_prac++; }
        if (ch->sub_class == SUB_CLASS_MONK || ch->sub_class == SUB_CLASS_BARD) { add_int_prac++; }
        if (tmp > 15) { add_int_prac++; }
        if (tmp > 20) { add_int_prac++; }
        if (tmp > 25) { add_int_prac++; }

        switch ((ch->sub_class == 0)
                ? class_table[ch->class].attr_prime :
                sub_class_table[ch->sub_class].attr_bonus )
        {
            case APPLY_STR:
                if (get_curr_str(ch) > 26)
                        add_phys_prac++;
                break;

            case APPLY_INT:
                if (get_curr_int(ch) > 26)
                        add_int_prac++;
                break;

            case APPLY_WIS:
                if (get_curr_wis(ch) > 26)
                        add_int_prac++;
                break;

            case APPLY_DEX:
                if (get_curr_dex(ch) > 26)
                        add_phys_prac++;
                break;

            case APPLY_CON:
                if (get_curr_con(ch) > 26)
                        add_phys_prac++;
                break;
        }

        sprintf(buf, "PC levels: %s.  Level: %d.  Stats: %d %d %d %d %d.  Pracs: %d Phys, %d Int.",
                ch->name,
                ch->level,
                get_curr_str(ch),
                get_curr_int(ch),
                get_curr_wis(ch),
                get_curr_dex(ch),
                get_curr_con(ch),
                add_phys_prac,
                add_int_prac);
        log_string(buf);

        add_int_prac = UMIN (5, add_int_prac);

        add_hp               = UMAX(  1, add_hp   );
        add_mana             = UMAX(  1, add_mana );
        add_move             = UMAX( 10, add_move );
        add_rage             = number_range( 1, 3);

        ch->max_hit          += add_hp;
        ch->max_mana         += add_mana;
        ch->max_move         += add_move;

        if ( ch->sub_class == SUB_CLASS_VAMPIRE )
                ch->max_rage         += add_rage;

        ch->pcdata->level_xp_loss = 0;

        ch->max_hit += ch->pcdata->str_prac;
        ch->max_mana += ch->pcdata->int_prac;

        if (!IS_NPC(ch))
                REMOVE_BIT(ch->act, PLR_BOUGHT_PET);

        sprintf (buf, "You gain {G%d hitpoints{x, {C%d mana{x and {Y%d movement{x.\n\r"
                 "You gain {W%d{x physical and {W%d{x intellectual practice points.\n\r\n\r",
                 add_hp + ch->pcdata->str_prac,
                 add_mana + ch->pcdata->int_prac,
                 add_move,
                 add_phys_prac,
                 add_int_prac);

        send_to_char( buf, ch );

        ch->pcdata->str_prac = add_phys_prac;
        ch->pcdata->int_prac = add_int_prac;

        for (i = 0; i < DEITY_NUMBER_TYPES; i++)
                SET_DEITY_TYPE_TIMER(ch, i, UMIN(-240, 0 - (ch->pcdata->deity_type_timer[i] / 2)));

        for (i = 0; i < DEITY_NUMBER_PERSONALITIES; i++)
                SET_DEITY_PERSONALITY_TIMER(ch, i, UMIN(-120, 0 - (ch->pcdata->deity_personality_timer[i] / 2)));
}


void gain_exp( CHAR_DATA *ch, int gain )
{
        char buf [ MAX_STRING_LENGTH ];
        int tmp;

        /* This is the new quest points required for level - Shade Sept 99 */
        /* It's this hacky on purpose because we don't want the player to */
        /* get the xp until we've checked they can advance! */

        tmp = ch->exp + gain;
        if (tmp >= (level_table[ch->level].exp_total))
        {
                if (!check_questpoints_allow_level_gain(ch, TRUE))
                {
                        ch->exp = level_table[ch->level].exp_total - 1;
                        return;
                }

                /* Check to see if level lock is on */
                if (!( IS_SET ( ch->act, PLR_AUTOLEVEL ) ) )
                {
                        ch->exp = level_table[ch->level].exp_total - 10;
                        send_to_char("You cannot level while {WAUTOLEVEL{x is set to {RNO{x.\n\r", ch);
                        return;
                }

                if(ch->pcdata->fame < 0)
                {
                        ch->exp = level_table[ch->level].exp_total - 1;
                        send_to_char("You must have positive fame to level!\n\r", ch);
                        return;
                }
        }

        /*
         *  ch->exp = UMAX( 1000, ch->exp + gain );
         *
         *  Don't want people slipping back into previous level's exp range
         *  through fleeing, etc; Gezhp
         */

        ch->exp = UMAX ((ch->level < 2) ? 1000 : level_table[ch->level-1].exp_total,
                        ch->exp + gain);

        while ( ch->level < LEVEL_HERO && ch->exp >= (level_table[ch->level].exp_total))
        {
                send_to_char( "\n\rYou raise a level!!  ",ch );
                ch->level += 1;
                advance_level(ch);

                if (ch->level == LEVEL_HERO)
                {
                        sprintf(buf, "-=>>  A time of celebration arrives as %s reaches the ultimate level of HERO!  <<<<=-",ch->name);
                        do_info(ch,buf);
                        sprintf(buf, "-=>>  The name of %s has been recorded for all time in the Hall of Heroes!  <<<<=-\n\r",ch->name);
                        do_info(ch,buf);
                        ch->pcdata->condition[COND_FULL] = 0;
                        ch->pcdata->condition[COND_THIRST] = 0;
                        ch->pcdata->condition[COND_DRUNK] = 0;
                        add_hero(ch);
                }
                else
                {
                        sprintf(buf,"-=>>%s has made it to level %d<<<<=-\n\r",ch->name,ch->level);
                        do_info(ch,buf);
                }
        }
}


int hit_gain( CHAR_DATA *ch )
{
        CHAR_DATA *gch;
        OBJ_DATA *obj;
        int gain;
        int amt;
        int count;
        int regen_mult = 1;

        if (IS_NPC(ch))
        {
                gain = ch->level * 3 / 2;

                if (IS_SET(ch->act, ACT_REGENERATOR))
                {
                      regen_mult = (ch->level / 10) + 2;
                      gain *= regen_mult;
                }

                if (IS_AFFECTED(ch, AFF_POISON)
                    || IS_AFFECTED(ch, AFF_PRAYER_PLAGUE))
                        gain /= 4;

                if (IS_AFFECTED(ch, AFF_SLOW))
                        gain /= 2;

                if (IS_SET(ch->act, ACT_NO_HEAL))
                {
                      gain = 0;
                }

                return UMIN( gain, ch->max_hit - ch->hit );
        }

        gain = UMAX(5, ch->level / 2);

        switch (ch->position)
        {
            case POS_SLEEPING:
                gain += 2 * get_curr_con(ch);
                break;

            case POS_RESTING:
                gain += get_curr_con(ch) ;
                break;
        }

        gain += gain * ch->pcdata->learned[gsn_fast_healing] / 100;

        if (ch->form == FORM_DEMON)
                gain += gain * ch->pcdata->learned[gsn_form_demon] / 100;

        if (ch->level < LEVEL_HERO)
        {
                if (!ch->pcdata->condition[COND_FULL])
                        gain /= 2;

                if (!ch->pcdata->condition[COND_THIRST])
                        gain /= 2;
        }

        if (IS_AFFECTED(ch, AFF_MEDITATE))
                gain += get_curr_int(ch);

        if (IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_PRAYER_PLAGUE))
                gain /= 4;

        if (IS_AFFECTED(ch, AFF_SLOW))
                gain /= 2;

        if ((ch->sub_class == SUB_CLASS_VAMPIRE && ch->rage < ch->max_rage / 10)
            || (ch->sub_class == SUB_CLASS_WEREWOLF && ch->rage > 90 ))
                gain -= gain/3;

        gain += (gain * race_table[ch->race].hp_regen/100);

        if ( IS_SET(ch->in_room->room_flags, ROOM_HEALING) )
                gain *= 2;

        if ( IS_SET(ch->in_room->room_flags, ROOM_TOXIC) )
        {
                int random;
                random = number_range(0,100);

                if (random <= 4)
                {
                    send_to_char("<165>This environment is toxic. You should not linger here.<0>\n\r", ch);

                    if (number_percent() < ch->pcdata->learned[gsn_resist_toxin] || is_affected(ch, gsn_bonus_exotic) )
                    {
                        send_to_char( "<46>Yo<47>u r<48>es<49>is<48>t t<47>he <46>po<47>is<48>on <49>su<48>rg<47>in<46>g t<47>hr<48>ou<49>gh <48>yo<47>ur <46>ve<47>in<48>s.<0>\n\r",ch );
                    }
                    else {
                        spell_nausea( gsn_nausea, ch->level, ch, ch );
                    }
                }
                gain /= 2;
        }

        if ( ch->in_room->vnum == clan_table[ch->clan].heal_room )
        {
                count = 1;

                for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                        if (obj->item_type == ITEM_CLAN_OBJECT )
                                count++;

                gain *= count;
        }

        /* Small terrain healing bonuses for various classes and races --Owl 20/9/22 */

        if ( ch->sub_class == SUB_CLASS_WITCH
        &&   ( ch->in_room->sector_type == SECT_SWAMP
          ||   ch->in_room->sector_type == SECT_FOREST ) )
        {
                gain += ( get_curr_con(ch) / 3 );
        }

        if ( ch->sub_class == CLASS_RANGER
        &&   ( ch->in_room->sector_type == SECT_HILLS
          ||   ch->in_room->sector_type == SECT_FOREST ) )
        {
                gain += ( get_curr_con(ch) / 3 );
        }

        if ( ( ch->race == RACE_HUMAN           && ch->in_room->sector_type == SECT_CITY )
          || ( ch->race == RACE_FAE             && ch->in_room->sector_type == SECT_AIR )
          || ( ch->race == RACE_WILD_ELF        && ch->in_room->sector_type == SECT_FOREST )
          || ( ch->race == RACE_SATYR           && ch->in_room->sector_type == SECT_FOREST )
          || ( ch->race == RACE_HALF_DRAGON     && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_DWARF           && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_OGRE            && ch->in_room->sector_type == SECT_HILLS )
          || ( ch->race == RACE_HALFLING        && ch->in_room->sector_type == SECT_FIELD )
          || ( ch->race == RACE_DWARF           && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_TROLL           && ch->in_room->sector_type == SECT_SWAMP )
          || ( ch->race == RACE_YUAN_TI         && ch->in_room->sector_type == SECT_DESERT )
          || ( ch->race == RACE_DUERGAR         && ch->in_room->sector_type == SECT_MOUNTAIN ) )
        {
                gain += ( get_curr_con(ch) / 4 );
        }



        if (ch->sub_class == SUB_CLASS_VAMPIRE
            && IS_OUTSIDE(ch)
            && !IS_SET(ch->in_room->room_flags, ROOM_DARK)
            && ch->in_room->sector_type != SECT_UNDERWATER
            && ch->in_room->sector_type != SECT_UNDERWATER_GROUND
            && weather_info.sky < SKY_RAINING
            && (time_info.hour >= 6 && time_info.hour <= 18)
            && !is_affected(ch, gsn_mist_walk))
        {
                send_to_char("Your skin burns as the {Ysunlight{x hits it!\n\r", ch);
                act("$n's skin starts to burn!", ch, NULL, NULL, TO_ROOM);
                gain = - number_range( 30, 60);

                if (ch->hit + gain <= 0)
                        gain = ch->hit -1;
        }

        /* Being in watery rooms rehydrates and somewhat speeds healing for Sahuagin & Grung */

        if ( ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
        && (  ( ch->in_room->sector_type == SECT_UNDERWATER )
           || ( ch->in_room->sector_type == SECT_UNDERWATER_GROUND )
           || ( ch->in_room->sector_type == SECT_WATER_SWIM )
           || ( ch->in_room->sector_type == SECT_SWAMP )
           || ( ch->in_room->sector_type == SECT_WATER_NOSWIM ) ) )
        {
                ch->pcdata->condition[COND_THIRST] = MAX_DRINK;
                gain += ( get_curr_con(ch) / 4 );
        }

        /*
         *  Burning and freezing rooms; Gez 2000
         */

        if (!IS_NPC(ch)
            && ch->level <= LEVEL_HERO
            && !IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                if (IS_SET(ch->in_room->room_flags, ROOM_BURNING)
                    && !is_affected(ch, gsn_resist_heat))
                {
                        send_to_char("{RThe intense heat is too much to bear!{x\n\r", ch);
                        damage(ch, ch, number_range(5, 15), TYPE_UNDEFINED, FALSE);
                        return 0;
                }

                if (IS_SET(ch->in_room->room_flags, ROOM_FREEZING)
                    && !is_affected(ch, gsn_resist_cold))
                {
                        send_to_char("{WThe freezing temperature drains your energy!{x\n\r", ch);
                        damage(ch, ch, number_range(5, 15), TYPE_UNDEFINED, FALSE);
                        return 0;
                }

                /*
                 *  Anti-swim (?); Owl 14/7/22
                 *
                 *  Strip the swim skill and affect for PCs when they're not in 'deep water' rooms.  Leaves
                 *  shifter snake form unaffected. Imms can dispel their own swimming effect--better for
                 *  testing etc.
                 */

                if ( ( ( ( IS_AFFECTED( ch, AFF_SWIM ) )
                ||       ( is_affected( ch, gsn_swim ) ) )
                &&     ( ch->form != FORM_SNAKE ) )
                   && ( ch->in_room->sector_type != SECT_UNDERWATER )
                   && ( ch->in_room->sector_type != SECT_UNDERWATER_GROUND )
                   && ( ch->in_room->sector_type != SECT_WATER_SWIM )
                   && ( ch->in_room->sector_type != SECT_WATER_NOSWIM ) )
                {
                        affect_strip(ch, gsn_swim);
                        REMOVE_BIT(ch->affected_by, AFF_SWIM);
                        send_to_char("{cNo longer in the water, you stop swimming.{w\n\r", ch);
                }

                /* Gravity code was here */

        }

        if (!IS_NPC(ch)
        && !ch->fighting
        && is_affected(ch, gsn_coil))
        {
                affect_strip(ch, gsn_coil);

                if (IS_AFFECTED(ch, AFF_HOLD))
                {
                        REMOVE_BIT(ch->affected_by, AFF_HOLD);
                }
        }

        if (is_affected(ch, gsn_song_of_rejuvenation))
        {
                amt = (ch->pcdata->learned[gsn_song_of_rejuvenation]) * ch->level / 100;
                gain += amt;
                send_to_char("{YYour song fills you with warmth; you feel rejuvenated.\n\r{x", ch);

                for (gch = char_list;gch;gch=gch->next)
                {
                        if (gch->deleted)
                                continue;

                        if (ch == gch)
                                continue;

                        if ((is_same_group(gch, ch)) && (ch->in_room == gch->in_room))
                        {
                                if (gch->hit < gch->max_hit)
                                {
                                        act("{Y$N's song fills you with warmth; you feel rejuvenated.{x", gch, NULL, ch,
                                            TO_CHAR);
                                        gch->hit += amt;
                                        if (gch->hit > gch->max_hit)
                                                gch->hit = gch->max_hit;
                                }
                        }
                }
        }

        if (!IS_NPC(ch))
        {
            recharge_bonuses(ch);
        }

        if (IS_AFFECTED(ch, AFF_HEART_TRAUMA))
                gain /= 2;

        if (ch->hit > ch->max_hit)
        {
                if (gain >= 0)
                        return 0;
                else
                        return gain;
        }

        return UMIN( gain, ch->max_hit - ch->hit );
}



int mana_gain( CHAR_DATA *ch )
{
        CHAR_DATA *gch;
        OBJ_DATA *obj;
        int gain;
        int amt;
        int count;

        if (IS_NPC(ch))
        {
                gain = ch->level * 3 / 2;
                return UMIN( gain, ch->max_mana - ch->mana );
        }

        gain = UMAX(5, ch->level);

        switch (ch->position)
        {
            case POS_SLEEPING:
                gain += get_curr_int(ch) * 2;
                break;

            case POS_RESTING:
                gain += get_curr_int(ch);
                break;
        }

        if (ch->level < LEVEL_HERO)
        {
                if (!ch->pcdata->condition[COND_FULL])
                        gain /= 2;

                if (!ch->pcdata->condition[COND_THIRST])
                        gain /= 2;
        }

        if (IS_AFFECTED(ch, AFF_MEDITATE))
                gain *= 2;

        /* To help pure shifters with all the mana-draining stuff they have to do */

        if ( ( ch->class == CLASS_SHAPE_SHIFTER )
        &&   ( ch->sub_class == SUB_CLASS_NONE ) )
                gain *= 2;

        if (IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_PRAYER_PLAGUE))
                gain /= 4;

        if (IS_AFFECTED(ch, AFF_SLOW))
                gain /= 2;

        gain += gain * race_table[ch->race].mana_regen / 100;

        if (IS_SET(ch->in_room->room_flags, ROOM_HEALING))
                gain *= 2;

        if (IS_SET(ch->in_room->room_flags, ROOM_TOXIC))
                gain /= 2;

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
                gain /= 2;

        if (ch->in_room->vnum == clan_table[ch->clan].heal_room)
        {
                count = 1;

                for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                        if (obj->item_type == ITEM_CLAN_OBJECT )
                                count++;

                gain = gain  * count;
        }

        /* Small terrain mana healing bonuses for various classes and races --Owl 20/9/22 */

        if ( ch->sub_class == SUB_CLASS_WITCH
        &&   ( ch->in_room->sector_type == SECT_SWAMP
          ||   ch->in_room->sector_type == SECT_FOREST ) )
        {
                gain += ( ( get_curr_int(ch) / 6 )  +  ( get_curr_wis(ch) / 6 ));
        }

        if ( ( (ch->class == CLASS_RANGER ) && ( ch->sub_class == SUB_CLASS_NONE ) )
        &&   ( ch->in_room->sector_type == SECT_HILLS
          ||   ch->in_room->sector_type == SECT_FOREST ) )
        {
                gain += ( ( get_curr_int(ch) / 6 )  +  ( get_curr_wis(ch) / 6 ));
        }

        if ( ( ch->race == RACE_HUMAN           && ch->in_room->sector_type == SECT_CITY )
          || ( ch->race == RACE_FAE             && ch->in_room->sector_type == SECT_AIR )
          || ( ch->race == RACE_WILD_ELF        && ch->in_room->sector_type == SECT_FOREST )
          || ( ch->race == RACE_SATYR           && ch->in_room->sector_type == SECT_FOREST )
          || ( ch->race == RACE_HALF_DRAGON     && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_DWARF           && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_OGRE            && ch->in_room->sector_type == SECT_HILLS )
          || ( ch->race == RACE_HALFLING        && ch->in_room->sector_type == SECT_FIELD )
          || ( ch->race == RACE_DWARF           && ch->in_room->sector_type == SECT_MOUNTAIN )
          || ( ch->race == RACE_TROLL           && ch->in_room->sector_type == SECT_SWAMP )
          || ( ch->race == RACE_YUAN_TI         && ch->in_room->sector_type == SECT_DESERT )
          || ( ch->race == RACE_DUERGAR         && ch->in_room->sector_type == SECT_MOUNTAIN ) )
        {
                gain += ( ( get_curr_int(ch) / 8 )  +  ( get_curr_wis(ch) / 8 ));;
        }


        if (is_affected(ch, gsn_song_of_rejuvenation))
        {
                amt = ch->pcdata->learned[gsn_song_of_rejuvenation] * ch->level;
                gain += amt;

                for (gch = char_list; gch; gch = gch->next)
                {
                        if (gch->deleted)
                                continue;

                        if (ch == gch)
                                continue;

                        if (is_same_group(gch, ch) && ch->in_room == gch->in_room)
                        {
                                if (gch->mana < gch->max_mana)
                                {
                                        gch->mana += amt;
                                        if (gch->mana > gch->max_mana)
                                                gch->mana = gch->max_mana;
                                }
                        }
                }
        }

        if (ch->mana > ch->max_mana)
                return 0;

        return UMIN(gain, ch->max_mana - ch->mana);
}


int move_gain( CHAR_DATA *ch )
{
        int gain;
        int count;
        OBJ_DATA *obj;

        if (IS_NPC(ch))
        {
                gain = ch->level;
                return UMIN( gain, ch->max_move - ch->move );
        }

        gain = UMAX(15, 2 * ch->level);

        switch (ch->position)
        {
            case POS_SLEEPING:
                gain += get_curr_dex(ch);
                break;

            case POS_RESTING:
                gain += get_curr_dex(ch) / 2;
                break;
        }

        if (ch->level < LEVEL_HERO)
        {
                if (!ch->pcdata->condition[COND_FULL])
                        gain /= 2;

                if (!ch->pcdata->condition[COND_THIRST])
                        gain /= 2;
        }

        if (IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_PRAYER_PLAGUE))
                gain /= 4;

        if (IS_AFFECTED(ch, AFF_SLOW))
                gain /= 2;

        gain += gain * race_table[ch->race].move_regen / 100;

        if (IS_SET(ch->in_room->room_flags, ROOM_HEALING))
                gain *= 2;

        if (IS_SET(ch->in_room->room_flags, ROOM_TOXIC))
                gain /= 2;

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
                gain /= 2;

        if (ch->in_room->vnum == clan_table[ch->clan].heal_room)
        {
                count = 1;

                for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                        if (obj->item_type == ITEM_CLAN_OBJECT)
                                count++;

                gain = gain * count;
        }

        return UMIN(gain, ch->max_move - ch->move);
}

/* empower meter loss */
int meter_gain( CHAR_DATA *ch )
{
        int gain = -20;
        if (IS_NPC(ch))
                return 0;

        if (ch->pcdata->meter < 20)
        {
                gain = ch->pcdata->meter;
                return -gain;
        }
        else
                return gain;
}
/* engrave loss */
int engrave_gain( CHAR_DATA *ch )
{
        int gain = -20;
        if (IS_NPC(ch))
                return 0;

        if (ch->pcdata->dam_meter < 20)
        {
                gain = ch->pcdata->dam_meter;
                return -gain;
        }
        else
                return gain;
}

/*
 *  Update blood and rage
 */
int rage_gain( CHAR_DATA *ch )
{
        int gain = -2;

        if (IS_NPC(ch)
            || !(ch->sub_class == SUB_CLASS_VAMPIRE || ch->sub_class == SUB_CLASS_WEREWOLF)
            || IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                return 0;

        if (ch->sub_class == SUB_CLASS_VAMPIRE)
        {
                if (ch->rage < ch->max_rage / 10)
                        send_to_char("You feel weak - you crave {Rblood!{x\n\r", ch);
                else if (ch->rage < ch->max_rage / 4)
                        send_to_char("You feel weakened - the power of your {Rlunge{x is reduced!\n\r", ch);

                if ( ch->rage < (ch->level / 10)
                &&   is_affected(ch, gsn_quicken) )
                {
                        affect_strip(ch, gsn_quicken);
                        send_to_char("You are too blood-starved to maintain your speed--you slow down.", ch);
                }

                if ( ch->rage < (ch->level / 10)
                &&   ch->form == FORM_BAT)
                {
                        send_to_char("<88><556><558>You are too blood-starved to remain in bat form.<559><557><0>", ch);
                        do_morph(ch, "normal");
                }

                if (!ch->pcdata->condition[COND_THIRST] && ch->level < LEVEL_HERO)
                        gain *= 2;

                if (ch->position == POS_SLEEPING)
                        gain /= 2;

                return UMAX(gain, -ch->rage);
        }

        /* else werewolf */

        if (ch->rage > 90)
                send_to_char("You feel enraged - you must {Rkill!{x\n\r", ch);

        if ( ch->rage < (ch->level / 10)
        &&   is_affected(ch, gsn_quicken) )
        {
                affect_strip(ch, gsn_quicken);
                send_to_char("You lack the rage to maintain your speed--you slow down.", ch);
        }

        if (ch->form == FORM_WOLF || ch->form == FORM_DIREWOLF)
                return UMIN(2, ch->max_rage - ch->rage);
        else if (ch->rage > 0)
                return -1;
        else
                return 0;
}


void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
        int condition;
        int gain;

        if (IS_NPC(ch)
            || value == -10
            || ch->level >= LEVEL_HERO
            || ch->level < 2
            || ch->sub_class == SUB_CLASS_VAMPIRE
            || IS_AFFECTED(ch, AFF_NON_CORPOREAL) )
                return;

        condition = ch->pcdata->condition[iCond];
        ch->pcdata->condition[iCond] = URANGE(-10, condition + value, MAX_FOOD);

        if (ch->pcdata->condition[iCond] <= -8)
        {
                switch (iCond)
                {
                    case COND_FULL:
                        send_to_char("You're dying of hunger!\n\r", ch);
                        gain = - number_range(2, 5) * ch->level;

                        if (ch->hit + gain <= 0)
                                gain = 0;

                        ch->hit += gain;
                        update_pos(ch);
                        break;

                    case COND_THIRST:
                        if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                        {
                             send_to_char("You're dying of dehydration!  You NEED water!\n\r", ch);
                        }
                        else {
                                send_to_char( "You're dying of thirst!\n\r", ch);
                        }
                        gain = - number_range( 15, 30);

                        if ( ch->hit + gain <= 0)
                                gain = 0;

                        ch->hit += gain;
                        update_pos(ch);
                        break;
                }
                return;
        }

        if (ch->pcdata->condition[iCond] <= -4)
        {
                switch (iCond)
                {
                    case COND_FULL:
                        send_to_char("You begin to feel weak due to lack of food.\n\r", ch);
                        gain = - number_range(5, 10);

                        if (ch->hit + gain <= 0)
                                gain = 0;

                        ch->hit += gain;
                        update_pos(ch);
                        break;

                    case COND_THIRST:
                        if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                        {
                             send_to_char("You're drying out; you badly need water.\n\r", ch);
                        }
                        else {
                                send_to_char("Your throat is parched; you cry out for liquid.\n\r", ch);
                        }
                        gain = - number_range(1, 2) * ch->level;

                        if (ch->hit + gain <= 0)
                                gain = 0;

                        ch->hit += gain;
                        update_pos(ch);
                        break;
                }
                return;
        }

        if (!ch->pcdata->condition[iCond])
        {
                switch (iCond)
                {
                    case COND_FULL:
                        send_to_char("You are hungry.\n\r", ch);
                        break;

                    case COND_THIRST:
                        if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                        {
                                send_to_char("Your body is starting to lose its moisture.\n\r", ch);
                        }
                        else {
                                send_to_char("You are thirsty.\n\r", ch);
                        }
                        break;
                }
        }
}


/*
 * Mob autonomous action.
 * This function takes 25% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
        char buf [MAX_STRING_LENGTH];
        CHAR_DATA *ch;
        EXIT_DATA *pexit;
        int door;

        for (ch = char_list; ch; ch = ch->next)
        {
                if (ch->deleted)
                        continue;

                if (!IS_NPC(ch)
                    || !ch->in_room
                    || ch->mount
                    || ch->rider
                    || IS_AFFECTED(ch, AFF_CHARM))
                        continue;

                /*
                 * Some summoned mobs should disappear when their master is absent.
                 * Also take care of tournament Bots.
                 * Gezhp
                 */
                if (IS_NPC(ch))
                {
                        if (IS_SET(ch->act, ACT_DIE_IF_MASTER_GONE)
                            && (!ch->master || ch->master->in_room != ch->in_room)
                            && !ch->fighting)
                        {
                                send_to_char ("With your master gone, you go too.\n\r", ch);
                                act ("$c shimmers and disappears from view.", ch, NULL, NULL, TO_ROOM);
                                extract_char (ch, TRUE);
                                continue;
                        }

                        if (IS_AFFECTED(ch, AFF_SWALLOWED)
                            && (!ch->inside || ch->inside->in_room != ch->in_room))
                        {
                                strip_swallow(ch);
                                continue;
                        }

                        if (ch->pIndexData->vnum == BOT_VNUM)
                        {
                                if (tournament_status == TOURNAMENT_STATUS_NONE
                                    || tournament_status == TOURNAMENT_STATUS_FINISHED
                                    || !IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                                {
                                        if (is_still_alive_in_tournament(ch)
                                            && tournament_status == TOURNAMENT_STATUS_RUNNING)
                                        {
                                                disqualify_tournament_entrant(ch, "left the Arena");
                                                check_for_tournament_winner();
                                        }

                                        sprintf(buf, "[Bot] destroying bot '%s' (location %d)",
                                                ch->short_descr,
                                                ch->in_room->vnum);
                                        log_string(buf);

                                        act("$c shimmers and disappears from view.",
                                            ch, NULL, NULL, TO_ROOM);

                                        extract_char(ch, TRUE);
                                }

                                else if (ch->spec_fun)
                                        (*ch->spec_fun)(ch);

                                continue;
                        }
                }

                /* Return after special function returns success */
                if (ch->spec_fun && (*ch->spec_fun)(ch))
                        continue;

                if (ch->in_room->area->nplayer > 0)
                        mprog_random_trigger(ch);

                if (ch->position < POS_STANDING)
                        continue;

                if (ch->rider && ch->in_room != ch->rider->in_room)
                        strip_mount(ch->rider);

                if (IS_SET(ch->act, ACT_SCAVENGER)
                    && ch->in_room->contents
                    && !number_bits(2))
                {
                        OBJ_DATA *obj;
                        OBJ_DATA *obj_best = NULL;
                        int max = 1;

                        for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                        {
                                if (IS_SET(obj->wear_flags, ITEM_TAKE)
                                    && obj->item_type != ITEM_FURNITURE
                                    && obj->item_type != ITEM_CORPSE_PC
                                    && obj->item_type != ITEM_CORPSE_NPC
                                    && obj->item_type != ITEM_REMAINS
                                    && obj->item_type != ITEM_FOUNTAIN
                                    && obj->item_type != ITEM_MOB
                                    && obj->item_type != ITEM_ANVIL
                                    && obj->item_type != ITEM_CLAN_OBJECT
                                    && obj->item_type != ITEM_PORTAL
                                    && obj->item_type != ITEM_TURRET
                                    && obj->cost > max
                                    && obj->owner[0] == '\0'
                                    && can_see_obj(ch, obj))
                                {
                                        obj_best = obj;
                                        max = obj->cost;
                                }
                        }

                        if (obj_best)
                        {
                                obj_from_room(obj_best);
                                obj_to_char(obj_best, ch);
                                act("$n gets $p.", ch, obj_best, NULL, TO_ROOM);
                        }
                }

                /*
                 * Wander
                 */
                if (!IS_SET(ch->act, ACT_SENTINEL)
                    && (door = number_bits(5)) <= 5
                    && (pexit = ch->in_room->exit[door])
                    && pexit->to_room
                    && !IS_SET(pexit->exit_info, EX_CLOSED)
                    && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
                    && !IS_SET(ch->act, ACT_DIE_IF_MASTER_GONE)
                    && (!IS_SET(ch->act, ACT_STAY_AREA)
                        || pexit->to_room->area == ch->in_room->area) )
                {
                        move_char(ch, door);

                        if (ch->position < POS_STANDING)
                                continue;
                }

                /*
                 * Flee
                 */
                if (ch->hit < (ch->max_hit / 2)
                    && !IS_SET(ch->act, ACT_SENTINEL)
                    && (door = number_bits(3)) <= 5
                    && (pexit = ch->in_room->exit[door])
                    && !IS_SET(ch->act, ACT_DIE_IF_MASTER_GONE)
                    && (!IS_SET(ch->act, ACT_STAY_AREA)
                        || pexit->to_room->area == ch->in_room->area)
                    && pexit->to_room
                    && !IS_SET(pexit->exit_info, EX_CLOSED)
                    && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
                {
                        move_char( ch, door );
                }
        }
}


/*
 * Update the weather.
 */
void day_weather_update()
{
        DESCRIPTOR_DATA *d;
        char buf [MAX_STRING_LENGTH];

        switch (time_info.day)
        {
            case 5:
                weather_info.moonlight = MOON_WAXING_CRESCENT;
                strcpy( buf, "The waxing crescent {Wmoon{x appears in the sky.\n\r" );
                break;

            case  9:
                weather_info.moonlight = MOON_FIRST_QUARTER;
                strcpy( buf, "The {Wmoon{x enters its first quarter.\n\r" );
                break;

            case 12:
                weather_info.moonlight = MOON_WAXING_GIBBOUS;
                strcpy( buf, "The {Wmoon{x enters its waxing gibbous phase.\n\r" );
                break;

            case 16:
                weather_info.moonlight = MOON_FULL;
                strcpy( buf, "A {Wfull moon{x rises in the sky.\n\r" );
                break;

            case  21:
                weather_info.moonlight = MOON_WANING_GIBBOUS;
                strcpy( buf, "The {Wmoon{x enters its waning gibbous phase.\n\r" );
                break;

            case 25:
                weather_info.moonlight = MOON_LAST_QUARTER;
                strcpy( buf, "The {Wmoon{x enters its last quarter.\n\r" );
                break;

            case 30:
                weather_info.moonlight = MOON_WANING_CRESCENT;
                strcpy( buf, "The waning crescent {Wmoon{x rises in the sky.\n\r" );
                break;

            case 35:
                weather_info.moonlight = MOON_NEW;
                strcpy( buf, "The {Wmoon{x has disappeared from view.\n\r" );
                time_info.day = 0;
                time_info.month++;

                if ( time_info.month >= 17 )
                {
                        time_info.month = 0;
                        time_info.year++;
                }
                break;

            default:
                return;
        }

        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected == CON_PLAYING
                    && IS_OUTSIDE(d->character)
                    && ( d->character->in_room->sector_type != SECT_UNDERWATER )
                    && ( d->character->in_room->sector_type != SECT_UNDERWATER_GROUND )
                    && IS_AWAKE(d->character))
                        send_to_char(buf, d->character);
        }
}


void weather_update ()
{
        DESCRIPTOR_DATA *d;
        char buf [MAX_STRING_LENGTH];
        int diff;

        buf[0] = '\0';

        switch (++time_info.hour)
        {
            case  5:
                weather_info.sunlight = SUN_LIGHT;
                strcat( buf, "<229>The day has begun.<0>\n\r" );
                break;

            case  6:
                weather_info.sunlight = SUN_RISE;
                strcat( buf, "The <226>sun<0> rises in the east.\n\r" );
                break;

            case 19:
                weather_info.sunlight = SUN_SET;
                strcat( buf, "The <202>sun<0> slowly disappears in the west.\n\r" );
                break;

            case 20:
                weather_info.sunlight = SUN_DARK;
                strcat( buf, "<20>The night has begun.<0>\n\r" );
                break;

            case 24:
                time_info.hour = 0;
                time_info.day++;
                day_weather_update();
                break;
        }

        /*
         * Weather change.
         */
        if (time_info.month >= 9 && time_info.month <= 16)
                diff = weather_info.mmhg >  985 ? -2 : 2;
        else
                diff = weather_info.mmhg > 1015 ? -2 : 2;

        weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
        weather_info.change = URANGE(-12, weather_info.change,  12 );

        weather_info.mmhg += weather_info.change;
        weather_info.mmhg = URANGE(960, weather_info.mmhg, 1040 );

        switch (weather_info.sky)
        {
            default:
                bug( "Weather_update: bad sky %d.", weather_info.sky );
                weather_info.sky = SKY_CLOUDLESS;
                break;

            case SKY_CLOUDLESS:
                if (weather_info.mmhg < 990
                    || (weather_info.mmhg < 1010 && !number_bits(2)))
                {
                        strcat( buf, "<240>The sky is getting cloudy.<0>\n\r" );
                        weather_info.sky = SKY_CLOUDY;
                }
                break;

            case SKY_CLOUDY:
                if (weather_info.mmhg < 970
                    || ( weather_info.mmhg < 990 && !number_bits(2)))
                {
                        strcat( buf, "<67>It starts to rain.<0>\n\r" );
                        weather_info.sky = SKY_RAINING;
                }

                if (weather_info.mmhg > 1030 && !number_bits(2))
                {
                        strcat( buf, "<87>The clouds disappear.<0>\n\r" );
                        weather_info.sky = SKY_CLOUDLESS;
                }
                break;

            case SKY_RAINING:
                if (weather_info.mmhg < 970 && !number_bits(2))
                {
                        strcat( buf, "<15>Lightning flashes in the sky.<0>\n\r" );
                        weather_info.sky = SKY_LIGHTNING;
                }

                if (weather_info.mmhg > 1030
                    || (weather_info.mmhg > 1010 && !number_bits(2)))
                {
                        strcat( buf, "<189>The rain stopped.<0>\n\r" );
                        weather_info.sky = SKY_CLOUDY;
                }
                break;

            case SKY_LIGHTNING:
                if (weather_info.mmhg > 1010
                    || (weather_info.mmhg > 990 && !number_bits(2)))
                {
                        strcat( buf, "<241>The lightning has stopped.<0>\n\r" );
                        weather_info.sky = SKY_RAINING;
                }
                break;
        }

        for (d = descriptor_list; d; d = d->next)
        {
                if (buf[0] != '\0'
                    && d->connected == CON_PLAYING
                    && IS_OUTSIDE(d->character)
                    && ( d->character->in_room->sector_type != SECT_UNDERWATER )
                    && ( d->character->in_room->sector_type != SECT_UNDERWATER_GROUND )
                    && IS_AWAKE(d->character))
                        send_to_char(buf, d->character);

                if (time_info.hour == 20
                    && d->character
                    && d->character->sub_class == SUB_CLASS_WEREWOLF
                    && weather_info.moonlight == MOON_FULL)
                {
                        if (d->character->position != POS_FIGHTING)
                                d->character->position = POS_STANDING;

                        send_to_char("<230>The full moon inspires a glorious <88>bloodlust<0><230> in you!<0>\n\r",
                                     d->character);

                        if (d->character->form != FORM_WOLF && d->character->form != FORM_DIREWOLF)
                                do_morph(d->character, "wolf");

                        act("$n bays at the <230>moon<0>!", d->character, NULL, NULL, TO_ROOM);
                }

                if (time_info.hour == 5
                    && d->character
                    && (d->character->sub_class == SUB_CLASS_VAMPIRE
                        || d->character->sub_class == SUB_CLASS_WEREWOLF))
                {
                        send_to_char("You sense the <226>sun<0> rising...\n\r", d->character);
                }

                if (time_info.hour == 19
                    && d->character
                    && (d->character->sub_class == SUB_CLASS_VAMPIRE
                        || d->character->sub_class == SUB_CLASS_WEREWOLF))
                {
                        send_to_char("You sense the <202>sun<0> setting...\n\r", d->character);
                }
        }
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{
        CHAR_DATA *gch;
        CHAR_DATA *ch;
        CHAR_DATA *ch_save = NULL;
        CHAR_DATA *ch_quit = NULL;
        EXIT_DATA *pexit;
        time_t save_time = current_time;
        int pre_gain_drunk;
        int door;

        for (ch = char_list; ch; ch = ch->next)
        {
                AFFECT_DATA *paf;

                if (ch->deleted)
                        continue;

                if (ch->backstab > 0)
                        ch->backstab--;

                if (ch->edrain > 0)
                        ch->edrain--;

                /*
                 * Find dude with oldest save time.
                 */
                if (!IS_NPC(ch)
                    && (!ch->desc || ch->desc->connected == CON_PLAYING)
                    && ch->level >= 2
                    && ch->save_time < save_time)
                {
                        ch_save = ch;
                        save_time = ch->save_time;
                }

                if (ch->position >= POS_STUNNED)
                {
                        ch->hit  += hit_gain(ch);

                        if (ch->aggro_dam > 0)
                                ch->aggro_dam -= UMIN(ch->level/8, ch->aggro_dam);

                        ch->mana += mana_gain(ch);

                        if (ch->move < ch->max_move)
                                ch->move += move_gain(ch);

                        ch->rage += rage_gain(ch);

                        if (!IS_NPC(ch))
                                ch->pcdata->meter += meter_gain(ch);
                        if (!IS_NPC(ch))
                                ch->pcdata->dam_meter += engrave_gain(ch);
                }

                if (ch->position == POS_STUNNED)
                        update_pos(ch);

                if (!IS_NPC(ch) && (ch->level < LEVEL_IMMORTAL
                                       || (!ch->desc && !IS_SWITCHED(ch))))
                {
                        OBJ_DATA *obj;

                        if ((obj = get_eq_char(ch, WEAR_LIGHT))
                            && obj->item_type == ITEM_LIGHT
                            && obj->value[2] > 0)
                        {
                                if (--obj->value[2] == 0 && ch->in_room)
                                {
                                        --ch->in_room->light;
                                        act("$p goes out.", ch, obj, NULL, TO_ROOM);
                                        act("$p goes out.", ch, obj, NULL, TO_CHAR);
                                        extract_obj(obj);
                                }
                        }

                        if (++ch->timer >= 10)
                        {
                                if (!ch->was_in_room && ch->in_room)
                                {
                                        ch->was_in_room = ch->in_room;

                                        if (ch->fighting)
                                                stop_fighting(ch, TRUE);

                                        send_to_char("You disappear into the void.\n\r", ch);
                                        act("$n disappears into the void.", ch, NULL, NULL, TO_ROOM);

                                        save_char_obj(ch);
                                        char_from_room(ch);
                                        char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
                                }
                        }

                        if (ch->timer > 20 && !IS_SWITCHED(ch))
                                ch_quit = ch;

                        pre_gain_drunk = ch->pcdata->condition[COND_DRUNK];
                        gain_condition(ch, COND_DRUNK, -1);

                        if ((ch->pcdata->condition[COND_DRUNK] == 0) && (pre_gain_drunk > 0 ))
                            send_to_char("You sober up.\n\r", ch);

                        if (ch->pcdata->condition[COND_DRUNK] == 13)
                            send_to_char("You are no longer inebriated.\n\r", ch);

                        if (ch->pcdata->condition[COND_DRUNK] == 26)
                            send_to_char("You are no longer dangerously drunk.\n\r", ch);

                        if (ch->pcdata->condition[COND_DRUNK] == 39)
                            send_to_char("You are no longer completely blind drunk.\n\r", ch);


                        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
                        {
                                if (!IS_NPC(gch)
                                    && is_same_group(gch,ch)
                                    && is_affected(gch, gsn_song_of_sustenance))
                                {
                                        send_to_char("{BThe song of sustenance restores you.\n\r{x", ch);
                                        ch->pcdata->condition[COND_FULL] =
                                                UMIN(ch->pcdata->condition[COND_FULL] + 4, MAX_FOOD);
                                        ch->pcdata->condition[COND_THIRST] =
                                                UMIN(ch->pcdata->condition[COND_THIRST] + 4, MAX_DRINK);
                                }
                        }

                        gain_condition(ch, COND_FULL,   -1);
                        gain_condition(ch, COND_THIRST, -1);
                }

                for (paf = ch->affected; paf; paf = paf->next)
                {
                        if (paf->deleted)
                                continue;

                        if (paf->duration < 0)
                                continue;

                        if (paf->duration > 0)
                                paf->duration--;
                        else
                        {
                                if (!paf->next
                                    || paf->next->type != paf->type
                                    || paf->next->duration > 0)
                                {
                                        if (paf->type > 0 && skill_table[paf->type].msg_off)
                                        {
                                                send_to_char(skill_table[paf->type].msg_off, ch);
                                                send_to_char("\n\r", ch);
                                        }
                                }

                                affect_remove(ch, paf);
                        }
                }

                /*
                 *   Careful with the damages here,
                 *   MUST NOT refer to ch after damage taken,
                 *   as it may be lethal damage (on NPC).
                 */
                if (ch->position == POS_MORTAL)
                {
                        damage(ch, ch, 2, TYPE_UNDEFINED, FALSE);
                }
                else if (ch->position == POS_INCAP)
                {
                        damage(ch, ch, 1, TYPE_UNDEFINED, FALSE);
                }
                else
                {
                        if ( (IS_AFFECTED(ch, AFF_CONFUSION))
                        && (!is_affected(ch, gsn_confusion)))
                        {
                            REMOVE_BIT(ch->affected_by, AFF_CONFUSION);
                        }

                        /*
                            Allows for permanently "slowed" mobiles.  Tortoises, slugs, who knows?
                           --Owl 18/8/24
                        */

                        if (!IS_NPC(ch)) {
                            if ( (IS_AFFECTED(ch, AFF_SLOW))
                            && (!is_affected(ch, gsn_slow)))
                            {
                                REMOVE_BIT(ch->affected_by, AFF_SLOW);
                            }
                        }

                        if ( IS_AFFECTED(ch, AFF_CONFUSION)
                        && ( door = number_bits(5)) <= 5
                        && ( pexit = ch->in_room->exit[door])
                        &&   pexit->to_room
                        &&  !IS_SET(pexit->exit_info, EX_CLOSED ) )
                        {
                            if ( ch->position == POS_SLEEPING )
                            {
                                    send_to_char("Your mind swirls in a fog of confusion.\n\r", ch);
                                    do_wake( ch, "" );
                            }
                            send_to_char("You stumble about clumsily.\n\r\n\r", ch);
                            act("$c staggers about in confusion.", ch, NULL, NULL, TO_ROOM);
                            move_char(ch, door);
                            act("$c looks very confused and uncertain.", ch, NULL, NULL, TO_ROOM);

                            /*if (ch->position < POS_STANDING)
                                    continue;*/
                        }

                        if (is_affected(ch, gsn_nausea) && IS_AFFECTED(ch, AFF_POISON))
                        {
                                /* dehydrating and hunger-making effect */

                                if (!IS_NPC(ch)
                                && ch->level < LEVEL_HERO
                                && ch->level > 2
                                && ch->sub_class != SUB_CLASS_VAMPIRE
                                && !IS_AFFECTED(ch, AFF_NON_CORPOREAL) )
                                {
                                    if (ch->pcdata->condition[COND_FULL] > 0)
                                    {
                                        ch->pcdata->condition[COND_FULL]--;
                                    }
                                    if (ch->pcdata->condition[COND_THIRST] > 0)
                                    {
                                        ch->pcdata->condition[COND_THIRST]--;
                                    }
                                }

                                if (IS_AFFECTED(ch, AFF_MEDITATE))
                                        REMOVE_BIT(ch->affected_by, AFF_MEDITATE);

                                send_to_char("You sweat and retch.\n\r", ch);
                                act("$n sweats and retches.", ch, NULL, NULL, TO_ROOM);
                                damage(ch, ch, number_range(3, 5), gsn_nausea, FALSE);

                        }

                        if ( (is_affected(ch, gsn_poison) && IS_AFFECTED(ch, AFF_POISON) )
                        || IS_AFFECTED(ch, AFF_PRAYER_PLAGUE))
                        {
                                if (IS_AFFECTED(ch, AFF_MEDITATE))
                                        REMOVE_BIT(ch->affected_by, AFF_MEDITATE);

                                send_to_char("You shiver and suffer.\n\r", ch);
                                act("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
                                damage(ch, ch, number_range(2, 4), gsn_poison, FALSE);

                        }

                        if ( ( ( IS_AFFECTED(ch, AFF_DOT) )
                           &&  ( !IS_AFFECTED(ch, AFF_NON_CORPOREAL) ) )
                        || ( IS_AFFECTED(ch, AFF_DOT) && ch->form == FORM_FLY ) )
                        {
                                for (paf = ch->affected; paf; paf = paf->next)
                                {
                                        if ( paf->deleted )
                                            continue;

                                        if( paf->bitvector && paf->bitvector == AFF_DOT )
                                        {
                                            if ( number_percent() < DOT_FREQ )
                                            {
                                                damage(ch, ch, paf->modifier, paf->type, FALSE);
                                            }
                                        }
                                }
                        }

                        /* Drowning? */
                        if (!IS_NPC(ch)
                            && ch->level <= LEVEL_HERO
                            && ch->position > POS_INCAP
                            && ch->form != FORM_SNAKE
                            && ch->form != FORM_HYDRA
                            && ch->form != FORM_DEMON
                            && ch->race != RACE_SAHUAGIN
                            && ch->race != RACE_GRUNG
                            && ch->sub_class != SUB_CLASS_VAMPIRE
                            && !is_affected(ch,gsn_mist_walk)
                            && !is_affected(ch,gsn_astral_sidestep)
                            && ( ( ch->in_room->sector_type == SECT_UNDERWATER )
                              || ( ch->in_room->sector_type == SECT_UNDERWATER_GROUND ) )
                            && !is_affected(ch, gsn_breathe_water))
                        {
                                if (--ch->pcdata->air_supply > 0)
                                {
                                        send_to_char("{BYou hold your breath under the water...{x\n\r", ch);
                                        return;
                                }
                                else
                                {
                                        send_to_char("{BYou gasp desperately for breath: you're drowning!{x\n\r", ch);
                                        act("$n is drowning!", ch, NULL, NULL, TO_ROOM);
                                        damage(ch, ch, number_range(ch->level * 3, ch->level * 5),
                                               TYPE_UNDEFINED, FALSE);

                                        if ( ch->position == POS_SLEEPING )
                                        {
                                                send_to_char("You decide that drowning is no fun and try to wake yourself up.\n\r", ch);
                                                do_wake( ch, "" );
                                        }

                                }
                        }
                }
        }

        /*
         * Autosave and autoquit.
         * Check that these chars still exist.
         */
        if (ch_save || ch_quit)
        {
                for (ch = char_list; ch; ch = ch->next)
                {
                        if (ch->deleted)
                                continue;

                        if (ch == ch_save)
                                save_char_obj(ch);

                        if (ch == ch_quit)
                                do_quit(ch, "");
                }
        }

        return;
}


/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update()
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        CHAR_DATA *rch;
        char *message;

        for (obj = object_list; obj; obj = obj_next)
        {
                obj_next = obj->next;

                if (obj->deleted)
                        continue;

                if (obj->timer < -1)
                        obj->timer = -1;

                if (--obj->timer == 0)
                {
                        switch (obj->item_type)
                        {
                            default:
                                message = "$p vanishes.";
                                break;

                            case ITEM_FOUNTAIN:
                                message = "$p dries up.";
                                break;

                            case ITEM_PORTAL:
                                message = "$p shimmers out of existence.";
                                break;
                            case ITEM_CORPSE_NPC:
                            case ITEM_CORPSE_PC:
                                message = "$p decays into dust.";
                                break;
                            case ITEM_REMAINS:
                                message = "$p crumble into dust.";
                                break;

                            case ITEM_FOOD:
                                message = "$p decomposes.";
                                break;
                        }

                        if (obj->carried_by)
                                act(message, obj->carried_by, obj, NULL, TO_CHAR);

                        else if (obj->in_room && (rch = obj->in_room->people))
                        {
                                act(message, rch, obj, NULL, TO_ROOM);
                                act(message, rch, obj, NULL, TO_CHAR);
                        }

                        extract_obj(obj);

                        if (obj == object_list)
                        {
                                obj_next = object_list;
                        }
                        else
                        {
                                OBJ_DATA *previous;

                                for (previous = object_list; previous; previous = previous->next)
                                        if (previous->next == obj)
                                                break;

                                obj_next = previous->next;
                        }
                }

                if (obj->timer == TIMER_DAMAGED && obj->carried_by)
                {
                        if (obj->item_type == ITEM_WEAPON
                            && IS_OBJ_STAT(obj, ITEM_POISONED))
                        {
                                act("Poison is beginning to seriously damage $p.",
                                    obj->carried_by, obj, NULL, TO_CHAR);
                        }

                        else if (obj->item_type == ITEM_WEAPON
                                 && (IS_OBJ_STAT(obj, ITEM_SHARP) || IS_OBJ_STAT(obj, ITEM_BLADE_THIRST)))
                        {
                                act("$p is beginning to deteriorate.",
                                    obj->carried_by, obj, NULL, TO_CHAR);
                        }

                        else if (obj->item_type == ITEM_ARMOR
                                 && IS_OBJ_STAT(obj, ITEM_FORGED))
                        {
                                act("$p's metal looks to be in poor condition.",
                                    obj->carried_by, obj, NULL, TO_CHAR);
                        }
                }
        }
}


/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 */
void aggr_update()
{
        CHAR_DATA *ch;
        CHAR_DATA *mch;
        CHAR_DATA *vch;
        CHAR_DATA *victim;
        DESCRIPTOR_DATA *d;
        ACT_PROG_DATA   *apdtmp;

        /* Gravity */
        CHAR_DATA *fch;
        CHAR_DATA *fch_next;
        ROOM_INDEX_DATA *in_room;
        ROOM_INDEX_DATA *to_room;
        static int gravity_count = 0;
        /* End gravity */

        static int bloodlust_count = 0;
        int cursed_utterance = 0;
        int tmp;

        /* char buf [MAX_STRING_LENGTH]; */

        while ( ( apdtmp = mob_act_list ) )
        {
            mch = mob_act_list->vo;
            if ( !mch->deleted && mch->mpactnum > 0 )
            {
                MPROG_ACT_LIST *tmp_act;

                while ( ( tmp_act = mch->mpact ) )
                    {
                if ( tmp_act->obj && tmp_act->obj->deleted )
                tmp_act->obj = NULL;
                if ( tmp_act->ch && !tmp_act->ch->deleted )
                mprog_wordlist_check( tmp_act->buf, mch, tmp_act->ch,
                            tmp_act->obj, tmp_act->vo, ACT_PROG );
                mch->mpact = tmp_act->next;
                free_string( tmp_act->buf );
                tmp_act->buf = NULL;
                free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
                }
                mch->mpactnum = 0;
                mch->mpact    = NULL;
            }
            mob_act_list = apdtmp->next;
            free_mem( apdtmp, sizeof( ACT_PROG_DATA ) );
        }

        gravity_count++;

        for (d = descriptor_list; d; d = d->next)
        {
            ch = d->character;

            if (d->connected != CON_PLAYING || !ch->in_room)
                    continue;

            /*
            * Bloodlust ego items might cause players to attack any NPC
            * target; check once per 20 pulses (5 secs); Gezhp 2001
            */
            if (++bloodlust_count > 19
                && has_ego_item_effect(ch, EGO_ITEM_BLOODLUST)
                && IS_AWAKE(ch)
                && !ch->fighting
                && !(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                        || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                && !IS_AFFECTED(ch, AFF_NON_CORPOREAL)
                && (ch->level > LEVEL_HERO || !ch->wait))
            {
                bloodlust_count = 0;

                for (mch = ch->in_room->people; mch; mch = mch->next_in_room)
                {
                    if (IS_NPC(mch)
                        && !mch->rider
                        && !IS_SET(mch->act, ACT_CLAN_GUARD)
                        && !number_bits(2))
                    {
                        act("{YLust for battle overwhelms you!  $C must DIE!{x",
                            ch, NULL, mch, TO_CHAR);
                        act("A strange gleam appears in $n's eyes...",
                            ch, NULL, NULL, TO_ROOM);
                        multi_hit(ch, mch, TYPE_UNDEFINED);
                        break;
                    }
                }
            }

            for (mch = ch->in_room->people; mch; mch = mch->next_in_room)
            {
                int count;

                if (gravity_count > 14)
                {
                    gravity_count = 0;

                    /*
                    *  Gravity; Owl 20/3/22
                    */

                    /*
                    * Below check goes here in case an imm transfers a player while they're falling, or they teleport or whatever.
                    * Makes sure the PLR_FALLING gets stripped and doesn't do crash damage.
                    */

                    if ( IS_SET( ch->act, PLR_FALLING )
                    &&   ch->in_room->sector_type != SECT_AIR )
                    {
                            REMOVE_BIT( ch->act, PLR_FALLING );
                    }

                    if ( ( ch->in_room->sector_type == SECT_AIR)
                    && ( !IS_AFFECTED(ch, AFF_FLYING) ) )
                    {
                        /* Don't set below until we're sure there's an open exit down */

                        in_room = ch->in_room;

                        if ( ch->in_room->exit[5] )
                        {
                            to_room = in_room->exit[5]->to_room;
                            /* Can we fall into it tho? headache coming */

                            if  ( room_is_private(to_room) )
                            {
                                return;
                            }

                            /* You -can- fall into a SECT_WATER_NOSWIM room you need a boat to enter, just bad luck. */

                            /* Don't fall into level-restricted rooms. */
                            if ( ( ch->level < to_room->area->low_enforced
                                || ch->level > to_room->area->high_enforced )
                                && ch->level <= LEVEL_HERO )
                            {
                                return;
                            }

                            if ( to_room->area->low_level == -4
                            && to_room->area->high_level == -4
                            && !IS_NPC(ch)
                            && !ch->clan
                            && !IS_SET( ch->status, PLR_RONIN )
                            && ch->level <= LEVEL_HERO )
                            {
                                return;
                            }

                            /* Mount checks */

                            if (ch->mount)
                            {
                                if ( IS_SET( to_room->room_flags, ROOM_SOLITARY )
                                ||   IS_SET( to_room->room_flags, ROOM_PRIVATE ) )
                                {
                                    return;
                                }

                                if ( IS_AFFECTED( ch->mount, AFF_FLYING ) )
                                {
                                    /* Don't fall if mount is flying */
                                    return;
                                }

                                if ( IS_SET( to_room->room_flags, ROOM_NO_MOUNT ) )
                                {
                                    act ("$c falls from $s mount.", ch, NULL, NULL, TO_ROOM);
                                    strip_mount (ch);
                                }
                            }

                            /* If we passed above mount checks you can both fall.  Check to damage mount. */
                            /* Push char into room and add PLR_FALLING bit */

                            SET_BIT(ch->act, PLR_FALLING);

                            if (!IS_IMMORTAL( ch ))
                            {
                                WAIT_STATE(ch, 1);
                            }

                            send_to_char("<14>Unable to stay aloft, you fall through the air!<0>\n\r\n\r", ch);

                            if (ch->mount && !IS_NPC(ch) && !IS_SET(ch->act, PLR_WIZINVIS))
                            {
                                act_move ("$n and $N freefall downwards, quickly vanishing from sight.", ch, NULL, ch->mount, TO_ROOM);
                            }
                            else if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
                            {
                                act ("$n freefalls downwards, quickly vanishing from sight.", ch, NULL, NULL, TO_ROOM);
                            }

                            char_from_room(ch);
                            char_to_room(ch, to_room);

                            if (ch->position == POS_STANDING )
                            {
                                do_look(ch, "auto");
                            }

                            if (ch->mount && !IS_NPC(ch) && !IS_SET(ch->act, PLR_WIZINVIS))
                            {
                                act_move ("$n and $N hurtle down from above.", ch, NULL, ch->mount, TO_ROOM);
                            }
                            else if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
                            {
                                act_move ("$n hurtles down from above.", ch, NULL, NULL, TO_ROOM);
                            }

                            for (fch = in_room->people; fch; fch = fch_next)
                            {
                                fch_next = fch->next_in_room;

                                if (fch->deleted)
                                    continue;

                                if (fch->rider == ch
                                && fch->position == POS_STANDING
                                && ch->in_room != fch->in_room)
                                    move_char (fch, 5);

                                else if (fch->master == ch
                                && fch->position == POS_STANDING
                                && ch->in_room != fch->in_room)
                                {
                                    act ("You follow $N.\n\r", fch, NULL, ch, TO_CHAR);
                                    move_char(fch, 5);
                                }
                            }

                            if (ch->mount && ch->in_room != ch->mount->in_room)
                            {
                                send_to_char ("\n\rYou seem to have lost your mount!\n\r", ch);
                                strip_mount (ch);
                                ch->position = POS_STANDING;
                            }

                            if ( IS_SET( ch->act, PLR_FALLING )
                            &&   ch->in_room->sector_type != SECT_AIR )
                            {
                                if ( ch->in_room->sector_type == SECT_WATER_NOSWIM
                                ||   ch->in_room->sector_type == SECT_WATER_SWIM
                                ||   ch->in_room->sector_type == SECT_SWAMP
                                ||   ch->in_room->sector_type == SECT_UNDERWATER_GROUND
                                ||   ch->in_room->sector_type == SECT_UNDERWATER )
                                {
                                    send_to_char("<12>You splashdown into water!<0>\n\r", ch);

                                    if (ch->mount && !IS_NPC(ch) && !IS_SET(ch->act, PLR_WIZINVIS))
                                    {
                                        act_move ("$n and $N splashdown into the water!", ch, NULL, ch->mount, TO_ROOM);
                                    }
                                    else if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
                                    {
                                        act_move ("$n splashes down into the water.", ch, NULL, NULL, TO_ROOM);
                                    }
                                    damage(ch, ch, (ch->hit / 4), TYPE_UNDEFINED, FALSE);
                                }
                                else {
                                    send_to_char("<3>You crash down onto the ground! OOOF!<0>\n\r", ch);

                                    if (ch->mount && !IS_NPC(ch) && !IS_SET(ch->act, PLR_WIZINVIS))
                                    {
                                        act_move ("$n and $N crash into the ground!", ch, NULL, ch->mount, TO_ROOM);
                                    }
                                    else if (!ch->rider && (IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
                                    {
                                        act_move ("$n crashes into the ground!", ch, NULL, NULL, TO_ROOM);
                                    }
                                    damage(ch, ch, (ch->hit / 3), TYPE_UNDEFINED, FALSE);
                                }

                                REMOVE_BIT( ch->act, PLR_FALLING );

                                if (!IS_NPC(ch))
                                {
                                    mprog_entry_trigger(ch);
                                    mprog_greet_trigger(ch);
                                }
                                return;
                            }

                            if (!IS_NPC(ch))
                            {
                                mprog_entry_trigger(ch);
                                mprog_greet_trigger(ch);
                            }
                        }
                        else {
                            /* No down exit */
                            return;
                        }
                    }
                }
                /* End gravity */

                /* Trauma stuff */
                if ( IS_AFFECTED(mch, AFF_EYE_TRAUMA) )
                {
                    SET_BIT(mch->affected_by, AFF_BLIND);
                }

                if ( IS_AFFECTED(ch, AFF_EYE_TRAUMA) )
                {
                    SET_BIT(ch->affected_by, AFF_BLIND);
                }

                /*
                * Mobs with DETECT_CURSE will aggro PCs who are cursed by a spell or who are carrying a cursed
                * object. Check is_cursed() to see exactly what qualifies them.  Speaking NPCs will also yell out to
                * explain why they are attacking if their HP is at max, which produces usually-correct behaviour.
                * -- Owl 9/8/22
                */

                if ( ( IS_AFFECTED(mch, AFF_DETECT_CURSE) && is_cursed(ch) )
                &&   ( mch != ch )
                &&   ( IS_NPC(mch) && !IS_NPC(ch) )
                &&   ( ch->level < LEVEL_IMMORTAL ) )
                {
                    cursed_utterance = 1;
                    goto fightloop;
                }

                if (!IS_NPC(mch)
                    || !IS_SET(mch->act, ACT_AGGRESSIVE)
                    || !can_see(mch, ch)
                    || !IS_AWAKE(mch)
                    || mch->fighting
                    || (IS_AFFECTED(ch, AFF_DETER) && !IS_SET(mch->act, ACT_CLAN_GUARD))
                    || IS_SET(mch->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(mch->in_room->area->area_flags, AREA_FLAG_SAFE)
                    || IS_AFFECTED(mch, AFF_CHARM)
                    || IS_AFFECTED(ch, AFF_NON_CORPOREAL)
                    || (IS_SET(mch->act, ACT_WIMPY) && IS_AWAKE(ch))
                    || has_tranquility(ch)
                    || ch->level > LEVEL_HERO
                    || mch->rider
                    || mch->deleted
                    || (ch->level > mch->level + 10))
                        continue;

                fightloop:

                /* If in a clan headquarters, don't attack anyone */
                if (mch->in_room->area->low_level == -4
                    && mch->in_room->area->high_level == -4)
                        continue;

                /*
                 * Ok we have a 'ch' player character and a 'mch' npc aggressor.
                 * Now make the aggressor fight a RANDOM pc victim in the room,
                 * giving each 'vch' an equal chance of selection.
                 */


                count  = 0;
                victim = NULL;

                for (vch = mch->in_room->people; vch; vch = vch->next_in_room)
                {
                    if (IS_NPC(vch)
                        || vch->deleted
                        || IS_AFFECTED(vch, AFF_NON_CORPOREAL)
                        || vch->level >= LEVEL_IMMORTAL)
                            continue;

                    if ( ( !IS_SET( mch->act, ACT_WIMPY )
                        || !IS_AWAKE( vch ) )
                        && can_see( mch, vch ) )
                    {
                        if ( !number_range( 0, count ) )
                        {
                            if ( vch->pcdata->group_leader
                            &&   !(IS_AFFECTED(vch->pcdata->group_leader, AFF_NON_CORPOREAL))
                            &&   can_see(mch, vch->pcdata->group_leader)
                            &&   vch->pcdata->group_leader->in_room == mch->in_room)
                            {
                                victim = vch->pcdata->group_leader;
                            }
                            else
                            {
                                victim = vch;
                            }
                        }
                        count++;
                    }
                }

                if (!victim)
                        continue;

                if ( CAN_SPEAK(mch)
                && ( cursed_utterance == 1 )
                && ( mch->hit == mch->max_hit) )
                {
                    cursed_utterance = 0;
                    tmp = number_percent();
                    if (tmp < 25)
                    {
                        act( "{R$c screams 'The cursed must be DESTROYED!'{x", mch, NULL, victim, TO_NOTVICT );
                        act( "{R$c screams 'The cursed must be DESTROYED!'{x", mch, NULL, victim, TO_VICT );
                    }
                    else if (tmp < 50)
                    {
                        act( "{R$c screams 'DIE, foul cursed creature!'{x", mch, NULL, victim, TO_NOTVICT );
                        act( "{R$c screams 'DIE, foul cursed creature!'{x", mch, NULL, victim, TO_VICT );
                    }
                    else if (tmp < 75)
                    {
                        act( "{R$c screeches 'I must eliminate every cursed being!'{x", mch, NULL, victim, TO_NOTVICT );
                        act( "{R$c screeches 'I will eliminate every cursed being!'{x", mch, NULL, victim, TO_VICT );
                    }
                    else
                    {
                        act( "{R$c yells 'Prepare for DEATH, accursed one!'{x", mch, NULL, victim, TO_NOTVICT );
                        act( "{R$c yells 'Prepare for DEATH, accursed one!'{x", mch, NULL, victim, TO_VICT );
                    }
                }
                multi_hit(mch, victim, TYPE_UNDEFINED);
            } /* mch loop */
        } /* descriptor loop */
}


/* Update the check on time for autoshutdown */
void time_update()
{
        FILE *fp;
        char *curr_time;
        char buf [ MAX_STRING_LENGTH ];

        if (down_time[0] == '*')
                return;

        curr_time = ctime(&current_time);
        if (!str_infix(warning1, curr_time))
        {
                sprintf(buf, "First Warning!\n\rShutdown at %s system time\n\r", down_time);
                send_to_all_char(buf);
                free_string(warning1);
                warning1 = str_dup("*");
        }

        if (!str_infix(warning2, curr_time))
        {
                sprintf(buf, "Second Warning!\n\rShutdown at %s system time\n\r", down_time);
                send_to_all_char(buf);
                free_string(warning2);
                warning2 = str_dup("*");
        }

        if (!str_infix(down_time, curr_time))
        {
                send_to_all_char("Shutdown by system.\n\r");
                log_string("Shutdown by system.");
                end_of_game();
                fclose(fpReserve);

                if (!(fp = fopen(SHUTDOWN_FILE, "a")))
                {
                        perror(SHUTDOWN_FILE);
                        bug("Could not open the Shutdown file!", 0);
                }
                else
                {
                        fprintf(fp, "Shutdown by System\n");
                        fclose (fp);
                }
                fpReserve = fopen (NULL_FILE, "r");
                merc_down = TRUE;
        }
}


/*
 * Remove deleted EXTRA_DESCR_DATA from objects.
 * Remove deleted AFFECT_DATA from chars and objects.
 * Remove deleted CHAR_DATA and OBJ_DATA from char_list and object_list.
 */
void list_update ()
{
        CHAR_DATA *ch;
        CHAR_DATA *ch_next;
        CHAR_DATA *ch_prev;
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        OBJ_DATA *obj_prev;
        AFFECT_DATA *paf;
        AFFECT_DATA *paf_next;
        AFFECT_DATA *paf_prev;
        EXTRA_DESCR_DATA *ed;
        EXTRA_DESCR_DATA *ed_next;
        extern bool delete_obj;
        extern bool delete_char;
        char buf [MAX_STRING_LENGTH];

        sprintf (last_function, "entering list_update");

        if (delete_char)
        {
                sprintf (last_function, "list_update: char_list");

                for (ch = char_list; ch; ch = ch_next)
                {
                        ch_next = ch->next;

                        for (paf = ch->affected; paf; paf = paf_next)
                        {
                                paf_next = paf->next;

                                if (paf->deleted || ch->deleted)
                                {
                                        if (ch->affected == paf)
                                                ch->affected = paf->next;
                                        else
                                        {
                                                for (paf_prev = ch->affected; paf_prev; paf_prev = paf_prev->next)
                                                {
                                                        if (paf_prev->next == paf)
                                                        {
                                                                paf_prev->next = paf->next;
                                                                break;
                                                        }
                                                }

                                                if (!paf_prev)
                                                {
                                                        bug("List_update: cannot find paf on ch.", 0);
                                                        continue;
                                                }
                                        }

                                        paf->next   = affect_free;
                                        affect_free = paf;
                                }
                        }

                        if (ch->deleted)
                        {
                                if (ch == char_list)
                                        char_list = ch->next;
                                else
                                {
                                        for (ch_prev = char_list; ch_prev; ch_prev = ch_prev->next)
                                        {
                                                if (ch_prev->next == ch)
                                                {
                                                        ch_prev->next = ch->next;
                                                        break;
                                                }
                                        }

                                        if (!ch_prev)
                                        {
                                                sprintf(buf, "List_update: char %s not found.",
                                                        ch->name);
                                                bug(buf, 0);
                                                continue;
                                        }
                                }

                                free_char(ch);
                        }
                }
        }

        sprintf (last_function, "list_update: obj_list");

        if (delete_obj)
        {
                for (obj = object_list; obj; obj = obj_next)
                {
                        obj_next = obj->next;

                        if (!obj->deleted)
                                continue;

                        sprintf (last_function, "list_update: delete_obj: extra_descr");

                        for (ed = obj->extra_descr; ed; ed = ed_next)
                        {
                                ed_next = ed->next;

                                if (obj->deleted)
                                {
                                        free_string(ed->description);
                                        free_string(ed->keyword);
                                        ed->next = extra_descr_free;
                                        extra_descr_free = ed;
                                }
                        }

                        sprintf (last_function, "list_update: delete_obj: effects");

                        for (paf = obj->affected; paf; paf = paf_next)
                        {
                                paf_next = paf->next;

                                if (obj->deleted)
                                {
                                        if (obj->affected == paf)
                                                obj->affected = paf->next;
                                        else
                                        {
                                                for (paf_prev = obj->affected; paf_prev; paf_prev = paf_prev->next)
                                                {
                                                        if (paf_prev->next == paf)
                                                        {
                                                                paf_prev->next = paf->next;
                                                                break;
                                                        }
                                                }

                                                if (!paf_prev)
                                                {
                                                        bug("List_update: cannot find paf on obj.", 0);
                                                        continue;
                                                }
                                        }

                                        paf->next = affect_free;
                                        affect_free = paf;
                                }
                        }

                        sprintf (last_function, "list_update: delete_obj: list");

                        if (obj->deleted)
                        {
                                if (obj == object_list)
                                        object_list = obj->next;
                                else
                                {
                                        for (obj_prev = object_list; obj_prev; obj_prev = obj_prev->next)
                                        {
                                                if (obj_prev->next == obj)
                                                {
                                                        obj_prev->next = obj->next;
                                                        break;
                                                }
                                        }

                                        if (!obj_prev)
                                        {
                                                bug("List_update: obj %d not found.",
                                                    obj->pIndexData->vnum);
                                                continue;
                                        }
                                }

                                sprintf (last_function, "list_update: delete_obj: free_string, "
                                         "object name is '%s'", obj->name);

                                free_string(obj->name);
                                free_string(obj->description);
                                free_string(obj->short_descr);
                                --obj->pIndexData->count;
                                obj->next = obj_free;
                                obj_free = obj;
                        }
                }
        }

        delete_obj  = FALSE;
        delete_char = FALSE;

        sprintf (last_function, "leaving list_update");
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler ()
{
        static int pulse_area;
        static int pulse_mobile;
        static int pulse_violence;
        static int pulse_point;
        static int pulse_state; /* for prone and dazed state initially - Brutus */
        static int pulse_msdp; /* <--- GCMP */

        sprintf (last_function, "entering update_hander");

        if (--pulse_area <= 0)
        {
                pulse_area = number_range(PULSE_AREA / 2, 3 * PULSE_AREA / 2);
                sprintf (last_function, "update_hander: calling area_update");
                area_update();
                sprintf (last_function, "update_hander: calling quest_update");
                quest_update();
        }

        if (--pulse_violence <= 0)
        {
                pulse_violence = PULSE_VIOLENCE;
                sprintf (last_function, "update_hander: calling violence_update");
                violence_update();
        }

        if (--pulse_state <= 0)
        {
                pulse_state = PULSE_STATE;
                sprintf (last_function, "update_hander: calling state_update");
                state_update();
        }

        if (--pulse_mobile <= 0)
        {
                pulse_mobile = PULSE_MOBILE;
                sprintf (last_function, "update_hander: calling mobile_update");
                mobile_update();
                do_tournament_countdown();
        }

        if (--pulse_point <= 0)
        {
                pulse_point = number_range(PULSE_TICK / 2, 3 * PULSE_TICK / 2);
                sprintf (last_function, "update_hander: calling weather_update");
                weather_update();
                sprintf (last_function, "update_hander: calling char_update");
                char_update();
                sprintf (last_function, "update_hander: calling obj_update");
                obj_update();
                sprintf (last_function, "update_hander: calling list_update");
                list_update();
                sprintf (last_function, "update_hander: calling deity_update");
                deity_update();
        }

        if ( --pulse_msdp <= 0 )
        {
                pulse_msdp      = PULSE_PER_SECOND;
                msdp_update();
        }

        sprintf (last_function, "calling gmcp_update");
        gmcp_update();   /* Comment this out to disable for troubleshooting */

        /* <---- GMCP */

        sprintf (last_function, "calling time_update");
        time_update();

        sprintf (last_function, "calling auction_update");
        auction_update();

        sprintf (last_function, "calling bot_update");
        bot_update();

        sprintf (last_function, "calling aggr_update");
        aggr_update();

        sprintf (last_function, "leaving update_handler");
        tail_chain();
        return;
}


/*
 * save morphed items - Shade
 */
void add_morph_list(CHAR_DATA *ch, int iWear)
{
        OBJ_DATA *theObj;

        if (!(theObj = get_eq_char(ch, iWear)))
                return;

        unequip_char(ch, theObj);
        ch->pcdata->morph_list[iWear] = theObj;

}


void rem_morph_list(CHAR_DATA *ch, int iWear)
{
        OBJ_DATA *theObj = ch->pcdata->morph_list[iWear];

        if (!theObj)
                return;

        equip_char(ch, theObj, iWear);
        ch->pcdata->morph_list[iWear] = NULL;
}


bool remove_obj (CHAR_DATA *ch, int iWear, bool fReplace);


void form_equipment_update (CHAR_DATA *ch)
{
        OBJ_DATA *obj;
        int iter;
        int jter;

        for (iter = 0; iter < MAX_WEAR; iter++)
        {
                jter = eq_slot_to_wear_bit[iter];

                obj = get_eq_char(ch, iter);

                /*
                 * no object currently worn - reapply what was there
                 */

                if (!obj)
                {
                        if (form_wear_table[ch->form].can_wear[jter])
                                rem_morph_list(ch, iter);
                }

                else
                {
                        /*
                         * there is an object there
                         */

                        if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
                        {
                                unequip_char(ch, obj);
                                extract_obj(obj);
                                rem_morph_list(ch, iter); /* Shade added April 2022 */
                        }

                        else if (!form_wear_table[ch->form].can_wear[jter])
                                add_morph_list(ch, iter);

                }
        }
}


/*
 * the auction update - another very important part
 */
void auction_update ()
{
        char buf[MAX_STRING_LENGTH];

        if (auction->item && --auction->pulse <= 0)
        {
                auction->pulse = PULSE_AUCTION;

                switch (++auction->going)
                {
                    case 1 :
                    case 2 :
                        if (auction->bet > 0)
                        {
                                sprintf (buf, "%s: going %s for %d platinum...",
                                         auction->item->short_descr,
                                         ((auction->going == 1) ? "once" : "twice"), auction->bet);
                        }
                        else
                        {
                                sprintf (buf, "%s: going %s (no bid received yet).",
                                         auction->item->short_descr,
                                         ((auction->going == 1) ? "once" : "twice"));
                        }

                        talk_auction (buf);
                        break;

                    case 3 :
                        if (auction->bet > 0)
                        {
                                sprintf (buf, "%s has been sold to %s for %d platinum.",
                                         auction->item->short_descr,
                                         auction->buyer->name,
                                         auction->bet);
                                talk_auction(buf);

                                obj_to_char(auction->item, auction->buyer);

                                act ("The auctioneer appears before you in a puff of smoke and hands you $p.",
                                     auction->buyer, auction->item, NULL, TO_CHAR);
                                act ("The auctioneer appears before $n, and hands $m $p.",
                                     auction->buyer, auction->item, NULL, TO_ROOM);

                                auction->seller->pcdata->bank += auction->bet * COIN_PLAT;
                                sprintf (buf, "%d platinum has been placed in your bank account.\n\r",
                                         auction->bet);
                                send_to_char(buf, auction->seller);

                                auction->item = NULL;
                        }
                        else
                        {
                                sprintf (buf, "No bids were received for %s: this item has been removed from auction.",
                                         auction->item->short_descr);
                                talk_auction(buf);

                                act ("The auctioneer appears before you to return $p to you.",
                                     auction->seller, auction->item, NULL, TO_CHAR);
                                act ("The auctioneer appears before $n to return $p to $m.",
                                     auction->seller, auction->item, NULL, TO_ROOM);

                                obj_to_char (auction->item, auction->seller);
                                auction->item = NULL;
                        }
                }
        }

        sprintf (last_function, "leaving auction_update");
}

void recharge_bonuses(CHAR_DATA *ch) {
    if (IS_NPC(ch)) return;

    PC_DATA *pcdata = ch->pcdata;
    if (!pcdata) return;

    long current_lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
    long current_lday = current_lhour / 24;

    long last_lhour = (pcdata->last_recharge - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
    long last_lday = last_lhour / 24;

    if ((current_lday - last_lday) < 1) {
        /* send_to_char("Not enough time has passed to recharge your bonuses.\r\n", ch); */
        return;
    }

    if (!pcdata->slept) {
        /* send_to_char("You have not slept since your last recharge.\r\n", ch); */
        return;
    }

    pcdata->bonus = pcdata->max_bonus;
    pcdata->slept = false;
    pcdata->last_recharge = current_time;

    send_to_char("<45>You feel refreshed and your bonus actions are fully recharged.\r\n<0>", ch);
}

bool check_questpoints_allow_level_gain (CHAR_DATA* ch, bool verbose)
{
        int qp;
        char buf [MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return FALSE;

        switch (ch->level)
        {
                /* Old
                 case 30:  qp =    1;  break;
                 case 49:  qp =  200;  break;
                 case 59:  qp =  400;  break;
                 case 69:  qp =  600;  break;
                 case 79:  qp =  900;  break;
                 case 89:  qp = 1200;  break;
                 case 99:  qp = 1500;  break;
                 */

            /* You'll also need to change the messages in do_score if you change these values. */

            case 29:
                qp = 1;
                break;

            case 49:
                qp = 200;
                break;

            case 79:
                qp = 500;
                break;

            case 99:
                qp = 1000;
                break;

            default:
                return TRUE;
        }

        if (ch->pcdata->totalqp < qp)
        {
                if (verbose)
                {
                        sprintf(buf,
                                "You need at least %d quest point%s to advance to level %d.\n\r",
                                qp,
                                (qp == 1) ? "" : "s",
                                ch->level + 1);
                        send_to_char(buf, ch);
                }
                return FALSE;
        }

        return TRUE;
}

void msdp_update( void )
{
    DESCRIPTOR_DATA *d;
    int PlayerCount = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->character && d->connected == CON_PLAYING && !IS_NPC(d->character) )
        {
            char buf[MAX_STRING_LENGTH];
            /*char buf2[MAX_STRING_LENGTH];*/
            CHAR_DATA *pOpponent = d->character->fighting;
            ROOM_INDEX_DATA *pRoom = d->character->in_room;
            AFFECT_DATA *paf;
            /*int temphitroll;*/

            ++PlayerCount;

            MSDPSetString( d, eMSDP_CHARACTER_NAME, d->character->name );
            MSDPSetNumber( d, eMSDP_ALIGNMENT, d->character->alignment );
            MSDPSetNumber( d, eMSDP_EXPERIENCE, d->character->exp );
/*
            MSDPSetNumber( d, eMSDP_EXPERIENCE_MAX, TBD );
            MSDPSetNumber( d, eMSDP_EXPERIENCE_TNL, TBD );
*/
            MSDPSetNumber( d, eMSDP_HEALTH, d->character->hit );
            MSDPSetNumber( d, eMSDP_HEALTH_MAX, d->character->max_hit );
            MSDPSetNumber( d, eMSDP_LEVEL, d->character->level );
/*
            MSDPSetString( d, eMSDP_RACE, TBD );
*/
            MSDPSetString( d, eMSDP_CLASS, class_table[d->character->class].who_name );
            MSDPSetString( d, eMSDP_SUBCLASS, sub_class_table[d->character->sub_class].who_name );
            MSDPSetNumber( d, eMSDP_MANA, d->character->mana );
            MSDPSetNumber( d, eMSDP_MANA_MAX, d->character->max_mana );
            MSDPSetNumber( d, eMSDP_WIMPY, d->character->wimpy );
            /* MSDPSetNumber( d, eMSDP_PRACTICE, d->character->practice ); */
            MSDPSetNumber( d, eMSDP_MONEY, d->character->gold );
            MSDPSetNumber( d, eMSDP_MOVEMENT, d->character->move );
            MSDPSetNumber( d, eMSDP_MOVEMENT_MAX, d->character->max_move );
            MSDPSetNumber( d, eMSDP_HITROLL, GET_HITROLL(d->character) );
            /*temphitroll =  GET_HITROLL(d->character);
            sprintf(buf2, "Hitroll in update.c is %d \r\n", temphitroll);
            log_string(buf2);*/
            MSDPSetNumber( d, eMSDP_DAMROLL, GET_DAMROLL(d->character) );
            MSDPSetNumber( d, eMSDP_AC, GET_AC(d->character) );
            MSDPSetNumber( d, eMSDP_STR, get_curr_str(d->character) );
            MSDPSetNumber( d, eMSDP_INT, get_curr_int(d->character) );
            MSDPSetNumber( d, eMSDP_WIS, get_curr_wis(d->character) );
            MSDPSetNumber( d, eMSDP_DEX, get_curr_dex(d->character) );
            MSDPSetNumber( d, eMSDP_CON, get_curr_con(d->character) );
            MSDPSetNumber( d, eMSDP_STR_PERM, d->character->pcdata->perm_str );
            MSDPSetNumber( d, eMSDP_INT_PERM, d->character->pcdata->perm_int );
            MSDPSetNumber( d, eMSDP_WIS_PERM, d->character->pcdata->perm_wis );
            MSDPSetNumber( d, eMSDP_DEX_PERM, d->character->pcdata->perm_dex );
            MSDPSetNumber( d, eMSDP_CON_PERM, d->character->pcdata->perm_con );

            /* This would be better moved elsewhere */
            if ( pOpponent != NULL )
            {
                int hit_points = (pOpponent->hit * 100) / pOpponent->max_hit;
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH, hit_points );
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH_MAX, 100 );
                MSDPSetNumber( d, eMSDP_OPPONENT_LEVEL, pOpponent->level );
                MSDPSetString( d, eMSDP_OPPONENT_NAME, pOpponent->name );
            }
            else /* Clear the values */
            {
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH, 0 );
                MSDPSetNumber( d, eMSDP_OPPONENT_LEVEL, 0 );
                MSDPSetString( d, eMSDP_OPPONENT_NAME, "" );
            }

            /* Only update room stuff if they've changed room */
            if ( pRoom && pRoom->vnum != d->pProtocol->pVariables[eMSDP_ROOM_VNUM]->ValueInt )
            {
                int i; /* Loop counter */
                buf[0] = '\0';

                for ( i = DIR_NORTH; i <= DIR_DOWN; ++i )
                {
                    if ( pRoom->exit[i] != NULL )
                    {
                        const char MsdpVar[] = { (char)MSDP_VAR, '\0' };
                        const char MsdpVal[] = { (char)MSDP_VAL, '\0' };
                   /*     extern char *const dir_name[]; */

                        strcat( buf, MsdpVar );
                        /*strcat( buf, dir_name[i] ); */
                        strcat( buf, MsdpVal );

                        if ( IS_SET(pRoom->exit[i]->exit_info, EX_CLOSED) )
                            strcat( buf, "C" );
                        else /* The exit is open */
                            strcat( buf, "O" );
                    }
                }

                if ( pRoom->area != NULL )
                    MSDPSetString( d, eMSDP_AREA_NAME, pRoom->area->name );

                MSDPSetString( d, eMSDP_ROOM_NAME, pRoom->name );
                MSDPSetTable( d, eMSDP_ROOM_EXITS, buf );
                MSDPSetNumber( d, eMSDP_ROOM_VNUM, pRoom->vnum );
            }
/*
            MSDPSetNumber( d, eMSDP_WORLD_TIME, d->character-> );
*/

            buf[0] = '\0';
            for ( paf = d->character->affected; paf; paf = paf->next )
            {
                char skill_buf[MAX_STRING_LENGTH];
                sprintf( skill_buf, "%c%s%c%d",
                    (char)MSDP_VAR, skill_table[paf->type].name,
                    (char)MSDP_VAL, paf->duration );
                strcat( buf, skill_buf );
            }
            MSDPSetTable( d, eMSDP_AFFECTS, buf );

            MSDPUpdate( d );
        }
    }

    /* Ideally this should be called once at startup, and again whenever
     * someone leaves or joins the mud.  But this works, and it keeps the
     * snippet simple.  Optimise as you see fit.
     */
    MSSPSetPlayers( PlayerCount );
}

/***************************************************************************
 * GMCP ADDON
 * File: update.c
 *
 * Add a new gmcp_update() function.
 ***************************************************************************/
void gmcp_update( void )
{
        DESCRIPTOR_DATA *d;

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
                if ( d->character && d->connected == CON_PLAYING && !IS_NPC(d->character) )
                {
                        ROOM_INDEX_DATA *room = d->character->in_room;
                        CHAR_DATA       *enemy = d->character->fighting;
                        AFFECT_DATA     *paf;
                        OBJ_DATA        *obj;
                        char            buf[MAX_STRING_LENGTH];
                        char            buf2[MAX_STRING_LENGTH];
                        char            buf3[MAX_STRING_LENGTH];
                        char            buf4[MAX_STRING_LENGTH];
                        char            **prgpstrShow;
                        char            *pstrShow;
                        int             *prgnShow;
                        int             obj_count;
                        int             nShow;
                        int             iShow;
                        int             rNext;
                        int             rSetcount;
                        bool            fShort;
                        bool            fShowNothing;
                        bool            fCombine;

                        obj_count       = 0;
                        nShow           = 0;
                        rNext           = 0;
                        rSetcount       = 0;
                        fShort          = TRUE;
                        fShowNothing    = TRUE;

                        UpdateGMCPString( d, GMCP_NAME, d->character->name );
                        UpdateGMCPString( d, GMCP_RACE, race_table[d->character->race].race_name );
                        UpdateGMCPString( d, GMCP_CLASS, full_class_name( d->character->class ) );
                        UpdateGMCPString( d, GMCP_SUBCLASS, full_sub_class_name( d->character->sub_class ) );
                        UpdateGMCPNumber( d, GMCP_SEX, d->character->sex );

                        UpdateGMCPNumber( d, GMCP_HP, d->character->hit );
                        UpdateGMCPNumber( d, GMCP_MANA, d->character->mana );
                        UpdateGMCPNumber( d, GMCP_MOVE, d->character->move );
                        UpdateGMCPNumber( d, GMCP_MAX_HP, d->character->max_hit );
                        UpdateGMCPNumber( d, GMCP_MAX_MANA, d->character->max_mana );
                        UpdateGMCPNumber( d, GMCP_MAX_MOVE, d->character->max_move );
                        UpdateGMCPNumber( d, GMCP_POSITION, d->character->position );
                        UpdateGMCPString( d, GMCP_FORM, extra_form_name(d->character->form) );
                        UpdateGMCPNumber( d, GMCP_RAGE, d->character->rage );
                        UpdateGMCPNumber( d, GMCP_MAX_RAGE, d->character->max_rage );

                        UpdateGMCPNumber( d, GMCP_STR, d->character->pcdata->perm_str );
                        UpdateGMCPNumber( d, GMCP_INT, d->character->pcdata->perm_int );
                        UpdateGMCPNumber( d, GMCP_WIS, d->character->pcdata->perm_wis );
                        UpdateGMCPNumber( d, GMCP_DEX, d->character->pcdata->perm_dex );
                        UpdateGMCPNumber( d, GMCP_CON, d->character->pcdata->perm_con);

                        UpdateGMCPNumber( d, GMCP_STR_MOD, (get_curr_str(d->character)) );
                        UpdateGMCPNumber( d, GMCP_INT_MOD, (get_curr_int(d->character)) );
                        UpdateGMCPNumber( d, GMCP_WIS_MOD, (get_curr_wis(d->character)) );
                        UpdateGMCPNumber( d, GMCP_DEX_MOD, (get_curr_dex(d->character)) );
                        UpdateGMCPNumber( d, GMCP_CON_MOD, (get_curr_con(d->character)) );

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_HITROLL, GET_HITROLL( d->character ) );
                        }
                        else {
                                /* Send a ridiculous false number so GUI knows to conceal it, same # for other lvl-restricted stats */
                                UpdateGMCPNumber( d, GMCP_HITROLL, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_DAMROLL, GET_DAMROLL( d->character ) );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_DAMROLL, 50000 );
                        }

                        UpdateGMCPNumber( d, GMCP_WIMPY, d->character->wimpy );
                        UpdateGMCPNumber( d, GMCP_CARRY_NUMBER, d->character->carry_number );
                        UpdateGMCPNumber( d, GMCP_CARRY_MAXNUM, ( can_carry_n( d->character ) ) );
                        UpdateGMCPNumber( d, GMCP_CARRY_WEIGHT, ( d->character->carry_weight + d->character->coin_weight ) );
                        UpdateGMCPNumber( d, GMCP_CARRY_MAXWEIGHT, ( can_carry_w( d->character ) ) );

                        if (d->character->level >= 20)
                        {
                                UpdateGMCPNumber( d, GMCP_AC, GET_AC( d->character) );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_AC, 50000 );
                        }

                        UpdateGMCPNumber( d, GMCP_FAME, d->character->pcdata->fame );

                        if (d->character->level >= 20)
                        {
                                UpdateGMCPNumber( d, GMCP_SAVE_VS, d->character->saving_throw );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_SAVE_VS, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_CRITICAL, GET_CRIT( d->character ) );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_CRITICAL, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_SWIFTNESS, GET_SWIFT( d->character ) );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_SWIFTNESS, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_RESIST_ACID, d->character->resist_acid );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_RESIST_ACID, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_RESIST_LIGHTNING, d->character->resist_lightning );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_RESIST_LIGHTNING, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_RESIST_HEAT, d->character->resist_heat );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_RESIST_HEAT, 50000 );
                        }

                        if (d->character->level >= 15)
                        {
                                UpdateGMCPNumber( d, GMCP_RESIST_COLD, d->character->resist_cold );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_RESIST_COLD, 50000 );
                        }

                        if (d->character->level >= 10)
                        {
                                UpdateGMCPNumber( d, GMCP_ALIGNMENT, d->character->alignment );
                        }
                        else {
                                UpdateGMCPNumber( d, GMCP_ALIGNMENT, 50000 );
                        }

                        UpdateGMCPNumber( d, GMCP_LEVEL, d->character->level );
                        UpdateGMCPNumber( d, GMCP_XP, d->character->exp );
                        UpdateGMCPNumber( d, GMCP_XP_MAX, (level_table[ d->character->level].exp_total) );
                        UpdateGMCPNumber( d, GMCP_XP_TNL, ( level_table[ d->character->level].exp_total) - d->character->exp );
                        UpdateGMCPNumber( d, GMCP_XP_CURLEVEL, ( level_table[ d->character->level].exp_level ) );
                        UpdateGMCPNumber( d, GMCP_PRACTICE, d->character->pcdata->str_prac );
                        UpdateGMCPNumber( d, GMCP_PLATINUM, d->character->plat );
                        UpdateGMCPNumber( d, GMCP_GOLD, d->character->gold );
                        UpdateGMCPNumber( d, GMCP_SILVER, d->character->silver );
                        UpdateGMCPNumber( d, GMCP_COPPER, d->character->copper );
                        UpdateGMCPNumber( d, GMCP_STEEL, d->character->smelted_steel );
                        UpdateGMCPNumber( d, GMCP_TITANIUM, d->character->smelted_titanium );
                        UpdateGMCPNumber( d, GMCP_ADAMANTITE, d->character->smelted_adamantite );
                        UpdateGMCPNumber( d, GMCP_ELECTRUM, d->character->smelted_electrum );
                        UpdateGMCPNumber( d, GMCP_STARMETAL, d->character->smelted_starmetal );

                        UpdateGMCPString( d, GMCP_AREA, d->character->in_room->area->name );
                        UpdateGMCPString( d, GMCP_ROOM_NAME, d->character->in_room->name );
                        UpdateGMCPNumber( d, GMCP_ROOM_SECT, d->character->in_room->sector_type );
                        UpdateGMCPNumber( d, GMCP_ROOM_FLAGS, d->character->in_room->room_flags );
                        UpdateGMCPNumber( d, GMCP_ROOM_VNUM, d->character->in_room->vnum );

                        buf4[0] = '\0';
                        if (d->character->in_room->room_flags)
                        {
                                for (rNext = 1; rNext > 0 && rNext <= BIT_25; rNext *= 2)
                                {
                                        if (IS_SET(d->character->in_room->room_flags, rNext))
                                        {
                                                rSetcount++;

                                                if (rSetcount != 1)
                                                        strcat(buf4, " ");

                                                strcat(buf4, room_flag_name(rNext));
                                        }
                                }
                        }
                        rSetcount = 0;

                        UpdateGMCPString( d, GMCP_ROOM_FLAGS, buf4 );

                        sprintf( buf, "%d", d->character->in_room->vnum );

                        if ( room && !strcmp( buf, d->pProtocol->GMCPVariable[GMCP_ROOM_VNUM] ) )
                        {
                                static const char *exit[] = { "n", "e", "s", "w", "u", "d" };
                                int i;
                                /*UpdateGMCPString( d, GMCP_AREA, d->character->in_room->area->name );
                                UpdateGMCPString( d, GMCP_ROOM_NAME, d->character->in_room->name );
                                UpdateGMCPNumber( d, GMCP_ROOM_VNUM, d->character->in_room->vnum );*/
                                buf[0] = '\0';
                                buf2[0] = '\0';

                                for ( i = DIR_NORTH; i <= DIR_DOWN; i++ )
                                {
                                        if ( !room->exit[i] )
                                                continue;

                                        if ( !room->exit[i]->to_room )
                                                continue;

                                        if ( buf[0] == '\0' )
                                        {
                                                #ifndef COLOR_CODE_FIX

                                                sprintf( buf, "\"%s\": \"%d\"", exit[i], room->exit[i]->to_room->vnum);
                                                #else
                                                sprintf( buf, "\"%s\": \"%d\"", exit[i], room->exit[i]->to_room->vnum);
                                                #endif
                                        }
                                        else
                                        {
                                                sprintf( buf2, ", \"%s\": \"%d\"", exit[i], room->exit[i]->to_room->vnum);
                                                strcat( buf, buf2 );
                                        }
                                }

                                UpdateGMCPString( d, GMCP_ROOM_EXITS, buf );

                        }

                        if ( enemy )
                        {
                                CHAR_DATA *ch;
                                buf[0] = '\0';
                                buf2[0] = '\0';

                                for ( ch = room->people; ch; ch = ch->next_in_room )
                                {
                                        /* Don't check current ch as this will double up enemies. */
                                        if ( ch == d->character )
                                                continue;

                                        /* isnpc value is 0 if enemy is a PC, otherwise it is the VNUM of the mobile. */
                                        if ( enemy == ch->fighting || ch->fighting == d->character )
                                        {
                                                #ifndef COLOR_CODE_FIX
                                                if ( buf[0] == '\0' )
                                                {
                                                        sprintf( buf, "[ { \"name\": \"%s\", \"level\": \"%d\", \"hp\": \"%d\", \"maxhp\": \"%d\", \"isnpc\": \"%d\" }",
                                                                (IS_NPC(enemy) ? enemy->short_descr : enemy->name),
                                                                enemy->level,
                                                                enemy->hit,
                                                                enemy->max_hit,
                                                                (IS_NPC(enemy) ? enemy->pIndexData->vnum : 0 )
                                                        );
                                                }
                                                else
                                                {
                                                        sprintf( buf2, ", { \"name\": \"%s\", \"level\": \"%d\", \"hp\": \"%d\", \"maxhp\": \"%d\", \"isnpc\": \"%d\" }",
                                                                (IS_NPC(enemy) ? enemy->short_descr : enemy->name),
                                                                enemy->level,
                                                                enemy->hit,
                                                                enemy->max_hit,
                                                                (IS_NPC(enemy) ? enemy->pIndexData->vnum : 0 )
                                                        );
                                                        strcat( buf, buf2 );
                                                }
                                                #else
                                                if ( buf[0] == '\0' ) sprintf( buf, "[ {{ \"name\": \"%s\", \"level\": \"%d\", \"hp\": \"%d\", \"maxhp\": \"%d\" }", enemy->name, enemy->level, enemy->hit, enemy->max_hit );
                                                else
                                                {
                                                        sprintf( buf2, ", {{ \"name\": \"%s\", \"level\": \"%d\", \"hp\": \"%d\", \"maxhp\": \"%d\" }", enemy->name, enemy->level, enemy->hit, enemy->max_hit );
                                                        strcat( buf, buf2 );
                                                }
                                                #endif
                                        }
                                }

                                strcat( buf, " ]" );
                                UpdateGMCPString( d, GMCP_ENEMY, buf );
                        }
                        else
                        {
                                UpdateGMCPString( d, GMCP_ENEMY, "" );
                        }

                        buf4[0] = '\0';

                        buf[0] = '\0';
                        buf2[0] = '\0';
                        for ( paf = d->character->affected; paf; paf = paf->next )
                        {

                                #ifndef COLOR_CODE_FIX

                                if ( paf->deleted != 1 )
                                {
                                        if ( buf[0] == '\0' )
                                        {
                                                sprintf( buf, "[ { \"name\": \"%s\", \"gives\": \"%s\", \"modifies\": \"%s\", \"mod_amount\": \"%d\", \"duration\": \"%d\" }",
                                                        skill_table[paf->type].name,
                                                        affect_bit_name_nice( paf->bitvector ),
                                                        affect_loc_name( paf->location ),
                                                        paf->modifier,
                                                        paf->duration );
                                        }
                                        else
                                        {
                                                sprintf( buf2, ", { \"name\": \"%s\", \"gives\": \"%s\", \"modifies\": \"%s\", \"mod_amount\": \"%d\", \"duration\": \"%d\" }",
                                                        skill_table[paf->type].name,
                                                        affect_bit_name_nice( paf->bitvector ),
                                                        affect_loc_name( paf->location ),
                                                        paf->modifier,
                                                        paf->duration );

                                                strcat( buf, buf2 );
                                        }
                                }
                                #else
                                if ( buf[0] == '\0' ) sprintf( buf, "[ {{ \"name\": \"%s\", \"duration\": \"%d\" }", skill_table[paf->type].name, paf->duration );
                                else
                                {
                                        sprintf( buf2, ", {{ \"name\": \"%s\", \"duration\": \"%d\" }", skill_table[paf->type].name, paf->duration );
                                        strcat( buf, buf2 );
                                }
                                #endif
                        }

                        if ( buf[0] == '\0' )
                                sprintf( buf, "[]" );
                        else
                                strcat( buf, " ]" );

                        UpdateGMCPString( d, GMCP_AFFECT, buf );

                        /* Inventory */

                        buf[0] = '\0';
                        buf2[0] = '\0';
                        buf3[0] = '\0';

                        /*
                         * Allocate space for output lines.
                         */

                        for (obj = d->character->carrying; obj; obj = obj->next_content)
                        {
                                if (!obj->deleted)
                                obj_count++;
                        }

                        prgpstrShow     = alloc_mem( obj_count * sizeof( char * ) );
                        prgnShow        = alloc_mem( obj_count * sizeof( int ) );

                        for (obj = d->character->carrying; obj; obj = obj->next_content)
                        {
                                /*
                                 * Objects with empty descriptions are intended to be hidden.
                                 * Suppress blanks lines that can appear in room descriptions: these can tip off players.
                                 */

                                if (!fShort && !strcmp(obj->description, ""))
                                {
                                        continue;
                                }

                                if (obj->wear_loc == WEAR_NONE && can_see_obj(d->character, obj))
                                {
                                        pstrShow = format_obj_to_char(obj, d->character, fShort);
                                        fCombine = FALSE;

                                        if (IS_SET(d->character->act, PLR_COMBINE))
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
                         * Create the JSON
                         */

                        if ((fShowNothing == 1) && (nShow == 0))
                        {
                               sprintf( buf, "[ []");
                               UpdateGMCPString( d, GMCP_INVENTORY, buf );
                               goto updated;

                        }

                        for (iShow = 0; iShow < nShow; iShow++)
                        {

                                if (IS_SET(d->character->act, PLR_COMBINE))
                                {
                                        if (prgnShow[iShow] != 1)
                                        {
                                                colourconv_8bit(buf3, prgpstrShow[iShow], d->character);

                                                if ( buf[0] == '\0' )
                                                {
                                                        sprintf( buf, "[ { \"quan\": \"%d\", \"short_desc\": \"%s\" }",
                                                                prgnShow[iShow],
                                                                buf3 );
                                                }
                                                else
                                                {
                                                        sprintf( buf2, ", { \"quan\": \"%d\", \"short_desc\": \"%s\" }",
                                                                prgnShow[iShow],
                                                                buf3 );

                                                        strcat( buf, buf2 );
                                                }
                                                buf3[0] = '\0';
                                        }
                                }
                                if ( !IS_SET(d->character->act, PLR_COMBINE)
                                     ||   prgnShow[iShow] == 1 )
                                {
                                        if ( buf[0] == '\0' )
                                        {
                                                colourconv_8bit(buf3, prgpstrShow[iShow], d->character);
                                                sprintf( buf, "[ { \"quan\": \"%d\", \"short_desc\": \"%s\" }",
                                                        1,
                                                        buf3 );
                                                buf3[0] = '\0';
                                        }
                                        else
                                        {
                                                colourconv_8bit(buf3, prgpstrShow[iShow], d->character);
                                                sprintf( buf2, ", { \"quan\": \"%d\", \"short_desc\": \"%s\" }",
                                                        1,
                                                        buf3 );
                                                strcat( buf, buf2 );
                                                buf3[0] = '\0';
                                        }
                                }
                                free_string(prgpstrShow[iShow]);
                        }

                        /*
                        * Clean up.
                        */
                        updated:
                        free_mem(prgpstrShow,   obj_count * sizeof( char * ) );
                        free_mem(prgnShow,      obj_count * sizeof( int ) );

                        if ( buf[0] == '\0' )
                                sprintf( buf, "[]" );
                        else
                                strcat( buf, " ]" );

                        UpdateGMCPString( d, GMCP_INVENTORY, buf );

                }
                SendUpdatedGMCP( d );
        }
        return;
}

/* EOF update.c */
