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



/*
 * Local functions.
 */
void say_spell (CHAR_DATA *ch, int sn);
bool is_safe (CHAR_DATA *ch, CHAR_DATA *victim);
bool skill_cannot_be_dispelled (int sn);


/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
        int sn;

        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
                if ( !skill_table[sn].name )
                        break;

                if ( LOWER( name[0] ) == LOWER( skill_table[sn].name[0] )
                    && !str_prefix( name, skill_table[sn].name ) )
                        return sn;
        }

        return -1;
}


void do_attack( CHAR_DATA *ch, char *argument )
{
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int value;

        if  ( IS_NPC( ch ))
                return;

        if (  ch->pcdata->learned[gsn_second_spell] < 10 )
        {
                send_to_char( "Huh?\n\r", ch );
                return;
        }

        argument = one_argument( argument, arg );

        if ( !is_number( arg ) )
        {
                send_to_char( "Please enter either 1, 2, 3 or 4 magic attacks per round.\n\r", ch );
                return;
        }

        value = atoi( arg );
        if ( value < 1 || value > 4 )
        {
                send_to_char(
                             "You may set your magic attacks to 1, 2, 3 or 4 per round.\n\r", ch);
                return;
        }

        if ( value == 3 && !CAN_DO(ch, gsn_third_spell))
        {
                send_to_char( "You don't know how to cast 3 spells per round.\n\r",ch);
                return;
        }

        if ( value == 4 && !CAN_DO(ch, gsn_fourth_spell))
        {
                send_to_char( "You don't know how to cast 4 spells per round.\n\r",ch);
                return;
        }

        ch->pcdata->spell_attacks = value;
        sprintf( buf, "Spells cast per round is set at %d.\n\r", value );
        send_to_char ( buf, ch );
        return;
}


/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
        CHAR_DATA *rch;
        char      *pName;
        char       buf       [ MAX_STRING_LENGTH ];
        char       buf2      [ MAX_STRING_LENGTH ];
        int        iSyl;
        int        length;

        struct syl_type
        {
                char *     old;
                char *     new;
        };

        static const struct syl_type syl_table [] =
        {
                { " ",          " "             },
                { "ar",         "abra"          },
                { "au",         "kada"          },
                { "bless",      "fido"          },
                { "blind",      "nose"          },
                { "bur",        "mosa"          },
                { "cu",         "judi"          },
                { "de",         "oculo"         },
                { "en",         "unso"          },
                { "light",      "dies"          },
                { "lo",         "hi"            },
                { "mor",        "zak"           },
                { "move",       "sido"          },
                { "ness",       "lacri"         },
                { "ning",       "illa"          },
                { "per",        "duda"          },
                { "ra",         "gru"           },
                { "re",         "candus"        },
                { "son",        "sabru"         },
                { "tect",       "infra"         },
                { "tri",        "cula"          },
                { "ven",        "nofo"          },
                { "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
                { "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
                { "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
                { "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
                { "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
                { "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
                { "y", "l" }, { "z", "k" },
                { "", "" }
        };

        buf[0]  = '\0';

        for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
        {
                for ( iSyl = 0;
                     ( length = strlen( syl_table[iSyl].old ) ) != 0;
                     iSyl++ )
                {

                        if ( !str_prefix( syl_table[iSyl].old, pName ) )
                        {
                                strcat( buf, syl_table[iSyl].new );
                                break;
                        }
                }

                if ( length == 0 )
                        length = 1;
        }

        sprintf( buf2, "$n utters the words, '%s'.", buf );
        sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );

        for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
        {
                if ( rch != ch )
                        act( ch->class == rch->class ? buf : buf2, ch, NULL, rch, TO_VICT );
        }
}


/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell (int level, CHAR_DATA *victim)
{
        int base = get_curr_wis(victim) + get_curr_int(victim);
        int save;

        if (IS_NPC(victim))
        {
                base *= 2;

                if (victim->level > 80)
                        base += 10;
                else if (victim->level > 50)
                        base += 5;
        }

        save = base + ((victim->level - level) * 4) - (victim->saving_throw / 4);
        save = URANGE(5, save, 95);

        return number_percent() < save;
}


/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

/*
 * uGlY!!
 */
int spell_attack_number;


void do_cast( CHAR_DATA *ch, char *argument )
{
        void      *vo;
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];
        char       buf [MAX_INPUT_LENGTH];
        int        mana;
        int        sn;

        if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
                return;

        if (!IS_NPC(ch) && (ch->pcdata->spell_attacks < 1 || ch->pcdata->spell_attacks > 4))
                ch->pcdata->spell_attacks = 1;

        target_name = one_argument( argument, arg1 );
        one_argument( target_name, arg2 );

        if ( arg1[0] == '\0' )
        {
                send_to_char( "Cast which what where?\n\r", ch );
                return;
        }

        sn = skill_lookup(arg1);

        /*
                Added below 23/09/18 to stop crashes caused by fake spellcasting
                --Owl
        */
        if (sn == -1)
        {
                send_to_char( "That spell doesn't exist.\n\r", ch );
                return;
        }
        /*
                Below code converts an int (e.g. 'sn') to a string and logs it.
                Which is sometimes useful.

                int length = snprintf( NULL, 0, "%d", sn );
                char* mystr = malloc( length + 1 );
                snprintf( mystr, length + 1, "%d", sn );
                log_string(mystr);

                -- Owl 23/09/18
        */

        if (!IS_NPC(ch) && !CAN_DO(ch, sn))
        {
                send_to_char("You fail miserably.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot cast in your current form.\n\r",ch);
                return;
        }

        if (  IS_SET(ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        {
                send_to_char( "An anti-magic field prevents any casting here.\n\r",ch);
                act("$n's attempt to cast fizzles out.",ch,NULL,NULL,TO_ROOM);
                return;
        }

        if ( ch->position < skill_table[sn].minimum_position )
        {
                send_to_char( "You can't concentrate enough.\n\r", ch );
                return;
        }

        /* mana calc */
        mana = 0;
        mana = mana_cost(ch, sn);

        if (ch->level >= LEVEL_IMMORTAL || IS_NPC(ch))
                mana = 0;

        /*
         * Locate targets.
         */
        victim  = NULL;
        obj     = NULL;
        vo      = NULL;

        switch ( skill_table[sn].target )
        {
            default:
                bug( "Do_cast: bad target for sn %d.", sn );
                return;

            case TAR_IGNORE:
                break;

            case TAR_CHAR_OFFENSIVE:
            case TAR_CHAR_OFFENSIVE_SINGLE:

                WAIT_STATE(ch, 2);  /* Not too much spam thanks */

                if ( arg2[0] == '\0' )
                {
                        if ( !( victim = ch->fighting ) )
                        {
                                send_to_char( "Cast the spell on whom?\n\r", ch );
                                return;
                        }
                }
                else
                {
                        if ( !( victim = get_char_room( ch, arg2 ) ) )
                        {
                                send_to_char( "They aren't here.\n\r", ch );
                                return;
                        }
                }

                if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
                {
                        send_to_char( "You can't do that on your own follower.\n\r", ch );
                        return;
                }

                if (ch == victim)
                {
                        send_to_char("You think about attacking yourself, but decide against it.\n\r", ch);
                        return;
                }

                if (is_safe(ch, victim))
                        return;

                if (IS_SET(ch->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        sprintf(buf, "$n casts %s at $N.", skill_table[sn].name);
                        arena_commentary(buf, ch, victim);
                }

                vo = (void *) victim;
                break;

            case TAR_CHAR_DEFENSIVE:
                if ( arg2[0] == '\0' )
                {
                        victim = ch;
                }
                else
                {
                        if ( !( victim = get_char_room( ch, arg2 ) ) )
                        {
                                send_to_char( "They aren't here.\n\r", ch );
                                return;
                        }

                        if (!IS_NPC(victim) && !IS_NPC(ch) && victim->level > 10
                            && (abs(ch->level - victim->level) > 10)
                            && (ch->level < LEVEL_HERO + 1))
                        {
                                send_to_char( "The gods prevent you casting a spell on them.\n\r", ch);
                                return;
                        }

                        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                        {
                                send_to_char("Not in their current form.\n\r", ch);
                                return;
                        }
                }

                if (ch == victim && is_affected(ch, sn))
                {
                        sprintf(buf, "You are already affected by '%s'.\n\r",
                                skill_table[sn].name);
                        send_to_char(buf, ch);
                        return;
                }

                vo = (void *) victim;
                break;

            case TAR_CHAR_SELF:
                if ( arg2[0] != '\0' && !is_name( arg2, ch->name ) )
                {
                        send_to_char( "You cannot cast this spell on another.\n\r", ch );
                        return;
                }

                if (is_affected(ch, sn))
                {
                        sprintf(buf, "You are already affected by '%s'.\n\r",
                                skill_table[sn].name);
                        send_to_char(buf, ch);
                        return;
                }

                vo = (void *) ch;
                break;

            case TAR_OBJ_INV:
                if ( arg2[0] == '\0' )
                {
                        send_to_char( "What should the spell be cast upon?\n\r", ch );
                        return;
                }

                if ( !( obj = get_obj_carry( ch, arg2 ) ) )
                {
                        send_to_char( "You are not carrying that.\n\r", ch );
                        return;
                }

                vo = (void *) obj;
                break;

            case TAR_OBJ_ROOM:
                if ( arg2[0] == '\0'  )
                {
                        send_to_char( "What should the spell be cast upon?\n\r", ch );
                        return;
                }

                obj = get_obj_list( ch, arg2, ch->in_room->contents );
                if ( !obj )
                {
                        act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
                        return;
                }

                vo = (void *) obj;
                break;
        }

        /*
        if  ( !skill_table[sn].name )
                return;

        Why is this so far into the function?  Also it doesn't work.
        --Owl 23/09/18
        */

        if ( ch->mana < mana )
        {
                send_to_char( "You don't have enough mana.\n\r", ch );
                return;
        }

        /*
        if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
        {
                say_spell( ch, sn );
        }
        Below probably fixes above?  --Owl 23/09/18
        */
        if ( (strcmp( skill_table[sn].name, "ventriloquate" )) == 0 )
        {
                say_spell( ch, sn );
        }

        WAIT_STATE( ch, skill_table[sn].beats );

        if ( !IS_NPC(ch)
            && ch->level <= LEVEL_HERO
            && number_percent() > (49 + (ch->pcdata->learned[sn] / 2))
            && ((sn = skill_lookup(arg1)) >= 0)
            && ch->pcdata->learned[sn] > 0)
        {
                if (skill_table[sn].target == TAR_CHAR_OFFENSIVE
                    || skill_table[sn].target == TAR_CHAR_OFFENSIVE_SINGLE)
                        damage(ch, victim, 0, sn, FALSE);
                else
                        send_to_char("You fail to correctly recite the spell!\n\r", ch);

                ch->mana -= mana / 2;
        }

        /* successfully cast */
        else
        {
                int chance;
                spell_attack_number = 1;
                ch->mana -= mana;
                (*skill_table[sn].spell_fun) (sn, URANGE(1, ch->level, MAX_LEVEL), ch, vo);

                if (skill_table[sn].target == TAR_CHAR_OFFENSIVE)
                {
                        if (victim->position == POS_DEAD
                            || ch->in_room != victim->in_room
                            || ch->position < POS_FIGHTING)
                                return;

                        /* 2nd spell */

                        if (IS_NPC(ch) || ch->pcdata->spell_attacks < 2)
                                return;

                        if (ch->mana < mana)
                        {
                                send_to_char ("You don't have enough mana for a second cast.\n\r", ch);
                                return;
                        }

                        chance = 45 + (ch->pcdata->learned[gsn_second_spell] / 2);

                        if ((number_percent() > (94 + ((ch->class == CLASS_CLERIC
                            ? get_curr_wis (ch) : get_curr_int (ch)) - 25)))
                            || (number_percent() > chance))
                                return;

                        ch->mana -= mana;
                        spell_attack_number = 2;

                        (*skill_table[sn].spell_fun)
                                (sn, URANGE(1, ch->level, MAX_LEVEL), ch, vo);

                        if (ch->fighting != victim)
                                return;

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room
                            || ch->position < POS_FIGHTING)
                                return;

                        /* 3rd spell  - must have succeeded in second spell to have a
                         * chance at a third spell - geoff */

                        if (ch->pcdata->spell_attacks < 3)
                                return;

                        if (ch->mana < mana)
                        {
                                send_to_char ("You don't have enough mana for a third cast.\n\r", ch);
                                return;
                        }

                        chance = 40 + (ch->pcdata->learned[gsn_third_spell] / 2);

                        if ((number_percent() > (94 + ((ch->class == CLASS_CLERIC
                            ? get_curr_wis (ch) : get_curr_int (ch)) - 25)))
                            || (number_percent() > chance))
                                return;

                        ch->mana -= mana;
                        spell_attack_number = 3;

                        (*skill_table[sn].spell_fun)
                                (sn, URANGE(1, ch->level, MAX_LEVEL), ch, vo);

                        if (ch->fighting != victim)
                                return;

                        if (victim->position == POS_DEAD || ch->in_room != victim->in_room
                            || ch->position < POS_FIGHTING)
                                return;

                        if (ch->pcdata->spell_attacks < 4)
                                return;

                        if (ch->mana < mana)
                        {
                                send_to_char ("You don't have enough mana for a fourth cast.\n\r", ch);
                                return;
                        }

                        chance = 30 + (ch->pcdata->learned[gsn_fourth_spell] / 2);

                        if ((number_percent() > (94 + ((ch->class == CLASS_CLERIC
                                                        ? get_curr_wis (ch) : get_curr_int (ch)) - 25)))
                            || (number_percent() > chance))
                                return;

                        ch->mana -= mana;
                        spell_attack_number = 4;

                        (*skill_table[sn].spell_fun)
                                (sn, URANGE(1, ch->level, MAX_LEVEL), ch, vo);

                        if (ch->fighting != victim)
                                return;
                }
        }  /* End successful cast */

        if (skill_table[sn].target == TAR_CHAR_OFFENSIVE
            && victim->master != ch
            && victim != ch
            && IS_AWAKE(victim)
            && !victim->fighting)
        {
                multi_hit(victim, ch, TYPE_UNDEFINED);
        }
}


/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
        void *vo;

        if ( sn <= 0 )
                return;

        if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
        {
                bug( "Obj_cast_spell: bad sn %d.", sn );
                return;
        }

        switch ( skill_table[sn].target )
        {
            default:
                bug( "Obj_cast_spell: bad target for sn %d.", sn );
                return;

            case TAR_IGNORE:
                vo = NULL;
                break;

            case TAR_CHAR_OFFENSIVE:
            case TAR_CHAR_OFFENSIVE_SINGLE:
                if ( !victim )
                        victim = ch->fighting;

                if ( !victim || ( !IS_NPC(victim) && !IS_NPC(ch) && ch != victim ) )
                {
                        send_to_char( "You can't do that.\n\r", ch );
                        return;
                }
                vo = (void *) victim;
                break;

            case TAR_CHAR_DEFENSIVE:
                if ( !victim )
                        victim = ch;

                if (!IS_NPC(ch)
                    && !IS_NPC(victim)
                    && victim->level > 10
                    && abs(ch->level - victim->level) > 10
                    && ch->level < LEVEL_HERO + 1)
                {
                        send_to_char( "The gods prevent your magic from affecting them.\n\r", ch);
                        return;
                }

                vo = (void *) victim;
                break;

            case TAR_CHAR_SELF:
                vo = (void *) ch;
                break;

            case TAR_OBJ_INV:
                if ( !obj )
                {
                        send_to_char( "You can't do that.\n\r", ch );
                        return;
                }
                vo = (void *) obj;
                break;
        }

        target_name = "";
        (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

        if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
            && victim->master != ch && ch != victim )
        {
                CHAR_DATA *vch;

                for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
                {
                        if ( vch->deleted )
                                continue;

                        if ( victim == vch && !victim->fighting )
                        {
                                multi_hit( victim, ch, TYPE_UNDEFINED );
                                break;
                        }
                }
        }

        return;
}


/*
 * Spell functions.
 */

void spell_acid_blast (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice( level, 8 );

        if (is_affected(victim, gsn_resist_acid) || saves_spell(level, victim))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("Hot acid streams from your fingertips towards $N!", ch, NULL, victim, TO_CHAR);
                act("Hot acid streams from $n's fingertips towards $N!", ch, NULL, victim, TO_NOTVICT);
        }

        damage( ch, victim, dam, sn, FALSE );
}


void spell_inner_fire (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int        dam;

        /* dam is 3-8 per level */

        dam = dice(level,6);
        dam += 2 * level;

        if (saves_spell(level,victim))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("$N's flesh begins to smoulder!", ch, NULL, victim, TO_CHAR);
                act("$N's flesh begins to smoulder!", ch, NULL, victim, TO_ROOM);
        }

        damage(ch,victim,dam,sn, FALSE);
}


void spell_synaptic_blast (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int        dam;

        /* dam is 5-12 per level */

        dam = dice(level,8);
        dam += 4 * level;

        if (saves_spell(level,victim))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("$N clutches $S head in agony!", ch, NULL, victim, TO_CHAR);
                act("$N clutches $S head in agony!", ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch,victim,dam,sn, FALSE);
        return;
}


void spell_prismatic_spray (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 8);
        dam += 3 * level;

        if (saves_spell(level, victim))
                dam /= 1.5;

        if (spell_attack_number == 1)
        {
                act("A spray of multi-coloured light shoots from $n's hand!", ch, NULL, NULL, TO_ROOM);
                act("You send forth a stream of {Yelemental{x death!", ch, NULL, NULL, TO_CHAR);
        }

        damage(ch, victim, dam, sn, FALSE);
}


void spell_holy_word (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice( level, 4 );
        dam += level;

        if ( saves_spell( level, victim ) )
                dam /= 2;

        if (IS_EVIL(ch))
                dam /= 2;

        if (IS_EVIL(victim))
                dam *= 2;

        if (spell_attack_number == 1)
        {
                act("You call upon the power of $D to smite $N!", ch, NULL, victim, TO_CHAR);
                act("$c calls upon the power of their god to smite $N!", ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch, victim, dam, sn, FALSE);
}


void spell_unholy_word (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice( level, 4 );
        dam += level;

        if ( saves_spell( level, victim ) )
                dam /= 2;

        if (IS_GOOD(ch))
                dam /= 2;

        if (IS_GOOD(victim))
                dam *= 2;

        if (spell_attack_number == 1)
        {
                act("You call upon the power of $D to smite $N!", ch, NULL, victim, TO_CHAR);
                act("$c calls upon the power of their god to smite $N!", ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch, victim, dam, sn, FALSE);
        return;
}


void spell_armor (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, sn))
                return;

        af.type      = sn;
        af.duration  = UMAX(24, ch->level);
        af.location  = APPLY_AC;
        af.modifier  = -20;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        if (ch != victim)
                send_to_char( "They are now protected.\n\r", ch );

        send_to_char("You feel someone protecting you.\n\r", victim);
}


void spell_astral_vortex( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim;

        /*
        return;
        Re-enabled 24/09/18.  And may God have mercy on my soul.
        --Owl
        */

        if ( ( !( victim = get_char_world (ch, target_name ) ) )
            || ( !IS_NPC ( ch ) && ch->fighting )
            || IS_SET (victim->in_room->room_flags, ROOM_PRIVATE )
            || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY )
            || IS_SET (victim->in_room->room_flags, ROOM_NO_RECALL )
            || IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL )
            || IS_SET (victim->in_room->room_flags, ROOM_SAFE )
            || IS_SET (victim->in_room->area->area_flags, AREA_FLAG_SAFE )
            || victim->level >= level +3
            || victim == ch )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        act( "$n creates a magical vortex and is suddenly sucked through it.", ch, NULL, victim, TO_ROOM );
        act( "You focus your energies on $N, a magical vortex appears into which you step.", ch, NULL, victim, TO_CHAR );
        char_from_room( ch );
        char_to_room( ch, victim->in_room );
        act( "A vortex appears overhead and $n is thrown from it.", ch, NULL, victim, TO_ROOM );
        do_look( ch, "auto" );
        return;
}


void spell_bless (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = 6 + level;
        af.location  = APPLY_HITROLL;
        af.modifier  = level / 8;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location  = APPLY_SAVING_SPELL;
        af.modifier  = 0 - level / 8;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They are surrounded by righteousness.\n\r", ch );

        send_to_char( "You feel righteous.\n\r", victim );
        return;
}


void spell_blindness (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if( IS_NPC(victim) && !HAS_EYES( victim ) )
        {
                act( "You can't blind $N, $E doesn't have any eyes!", ch, NULL, victim, TO_CHAR );
                return;
        }

        if (IS_AFFECTED(victim, AFF_BLIND))
        {
                send_to_char("They are already blinded.\n\r", ch);
                return;
        }

        if (saves_spell(level, victim))
        {
                send_to_char( "They resist your spell!\n\r", ch );
                /* Shade 12.2.22 */
                /* Make a save initiate combat and do a minor amount of damage */
                damage(ch, victim, number_range(1, ch->level/2), gsn_blindness, FALSE);
                return;
        }

        af.type      = sn;
        af.duration  = 2 + (level / 10);
        af.location  = APPLY_HITROLL;
        af.modifier  = -4;
        af.bitvector = AFF_BLIND;
        affect_to_char( victim, &af );

        act( "{W$C is blinded!{x", ch, NULL, victim, TO_CHAR    );
        send_to_char( "You are blinded!\n\r", victim );
        act( "{W$C is blinded!{x", ch, NULL, victim, TO_NOTVICT );
        damage(ch, victim, number_range(1, ch->level/2) + 10, gsn_blindness, FALSE);
        return;
}


void spell_burning_hands (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = number_range(10, 20) + dice(UMIN(level, 30), 3);

        if (is_affected(victim, gsn_resist_heat) ||  saves_spell(level, victim))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_call_lightning (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *vch;
        int        dam;

        if ( !IS_OUTSIDE( ch ) )
        {
                send_to_char( "You must be outdoors.\n\r", ch );
                return;
        }

        if ( weather_info.sky < SKY_RAINING )
        {
                send_to_char( "You need bad weather.\n\r", ch );
                return;
        }

        send_to_char( "God's {Ylightning{x strikes your foes!\n\r", ch );
        act( "$n calls God's lightning to strike $s foes!", ch, NULL, NULL, TO_ROOM );

        for ( vch = char_list; vch; vch = vch->next )
        {
                if ( vch->deleted || !vch->in_room )
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if (vch->in_room == ch->in_room
                    && vch != ch
                    && (IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch)))
                {
                        dam = dice( level , 8 );

                        if (is_affected(vch, gsn_resist_lightning)
                            || saves_spell(level, vch))
                                dam /= 2;

                        damage(ch, vch, dam, sn, FALSE);
                        continue;
                }

                if ( vch->in_room->area == ch->in_room->area
                    && IS_OUTSIDE( vch )
                    && IS_AWAKE( vch ) )
                        send_to_char( "{YLightning{x flashes in the sky.\n\r", vch );
        }
}


void spell_cause_light (int sn, int level, CHAR_DATA *ch, void *vo)
{
        damage( ch, (CHAR_DATA *) vo, dice( 1, 8 ) + level / 3, sn, FALSE );
}


void spell_cause_critical (int sn, int level, CHAR_DATA *ch, void *vo)
{
        damage ( ch, (CHAR_DATA *) vo, dice( 3, 8 ) + level - 6, sn, FALSE );
}


void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
        damage ( ch, (CHAR_DATA *) vo, dice( 2, 8 ) + level / 2, sn, FALSE );
}


void spell_chain_lightning (int sn,int level,CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        CHAR_DATA *tmp_vict,*last_vict,*next_vict;
        bool found;
        int dam;

        /* first strike */

        act("A lightning bolt leaps from $n's hand and arcs to $N.",
            ch, NULL, victim, TO_ROOM);
        act("A lightning bolt leaps from your hand and arcs to $N.",
            ch, NULL, victim, TO_CHAR);
        act("A lightning bolt leaps from $n's hand and hits you!",
            ch, NULL, victim, TO_VICT);

        dam = dice(level,6);

        if (is_affected(victim, gsn_resist_lightning) || saves_spell(level, victim))
                dam /= 3;

        damage(ch, victim, dam, sn, FALSE);

        if (!victim->in_room || victim->in_room != ch->in_room)
                return;

        last_vict = victim;
        level -= 4;

        while (level > 0)
        {
                found = FALSE;

                for (tmp_vict = ch->in_room->people;
                     tmp_vict;
                     tmp_vict = next_vict)
                {
                        next_vict = tmp_vict->next_in_room;

                        if (!is_safe(ch, tmp_vict) && tmp_vict != last_vict)
                        {
                                found = TRUE;
                                last_vict = tmp_vict;
                                act("The bolt arcs to $n!", tmp_vict, NULL, NULL, TO_ROOM);
                                act("The bolt hits you!", tmp_vict, NULL, NULL, TO_CHAR);
                                dam = dice(level,6);

                                if (is_affected(tmp_vict, gsn_resist_lightning)
                                    || saves_spell(level,tmp_vict))
                                        dam /= 3;

                                damage(ch, tmp_vict, dam, sn, FALSE);
                                level -= 4;

                                if (!tmp_vict->in_room || tmp_vict->in_room != ch->in_room)
                                        return;
                        }
                }   /* end target searching loop */

                if (!found) /* no target found, hit the caster */
                {
                        act("The bolt arcs to $n... whoops!", ch, NULL, NULL, TO_ROOM);
                        send_to_char("You are struck by your own lightning!\n\r", ch);
                        dam = dice(level,6);

                        if (is_affected(ch, gsn_resist_lightning) || saves_spell(level,ch))
                                dam /= 3;

                        damage(ch, ch, dam, sn, FALSE);
                        return;
                }
        }
}


void spell_change_sex ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        af.type      = sn;
        af.duration  = 10 * level;
        af.location  = APPLY_SEX;

        do
        {
                af.modifier  = number_range( 0, 2 ) - victim->sex;
        }
        while ( af.modifier == 0 );

        af.bitvector = 0;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel different.\n\r", victim );
        return;
}


void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        /* return;  Re-enabled 22/09/2018.  I too like to live dangerously. --Owl */

        if ( victim == ch )
        {
                send_to_char( "You like yourself even better!\n\r", ch );
                return;
        }

        if ( !IS_NPC( victim ) )
        {
                send_to_char( "You cannot charm other players.\n\r", ch );
                return;
        }

        if (IS_AFFECTED(victim, AFF_CHARM)
            || IS_SET(victim->act, ACT_NOCHARM)
            || IS_AFFECTED(ch, AFF_CHARM)
            || level < victim->level
            || ch->mount == victim
            || victim->rider
            || IS_SET(victim->act, ACT_BANKER)
            || IS_SET(victim->act, ACT_CLAN_GUARD)
            || !mob_interacts_players(victim)
            || mob_is_quest_target(victim)
            || saves_spell(level, victim))
        {
                send_to_char("Your charm failed.\n\r", ch);
                return;
        }

        if ( victim->master )
                stop_follower( victim );

        add_follower( victim, ch );
        af.type      = sn;
        af.duration  = number_fuzzy( level / 4 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( victim, &af );

        send_to_char( "Ok.\n\r", ch );
        act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );

        SET_BIT(victim->act, ACT_NO_EXPERIENCE);
}


void spell_chill_touch (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;
        int dam;

        dam = number_range(10, 20) + level;

        if (!is_affected(victim, gsn_resist_cold)
            && !saves_spell(level, victim)
            && (!IS_NPC(victim) || (MAKES_CORPSE(victim) && !IS_INORGANIC(victim))))
        {
                af.type      = sn;
                af.duration  = 6;
                af.location  = APPLY_STR;
                af.modifier  = -2;
                af.bitvector = 0;
                affect_join(victim, &af);

                act("You drain strength from $N!", ch, NULL, victim, TO_CHAR);
                act("$c drains strength from $N!", ch, NULL, victim, TO_NOTVICT);
        }

        damage( ch, victim, dam, sn, FALSE );
}


void spell_colour_spray (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 4) + level;

        if (saves_spell(level, victim))
                dam /= 2;

        damage(ch, victim, dam, sn, FALSE);
}


void spell_continual_light (int sn, int level, CHAR_DATA *ch, void *vo)
{
        OBJ_DATA *light;

        light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );
        obj_to_room( light, ch->in_room );

        act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
        act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
}


void spell_control_weather ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        if (!str_cmp(target_name, "better"))
        {
                weather_info.change += dice(level / 3, 4);
                send_to_char("The weather starts to look a little better.\n\r", ch);
        }
        else if (!str_cmp(target_name, "worse"))
        {
                weather_info.change -= dice(level / 3, 4);
                send_to_char("The weather starts to look a little worse.\n\r", ch);
        }
        else
                send_to_char ("Do you want it to get better or worse?\n\r", ch );
}


void spell_create_food ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *mushroom;

        mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
        mushroom->value[0] = 5 + level;
        obj_to_room( mushroom, ch->in_room );

        act( "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
        act( "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
        return;
}


void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *spring;

        spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
        spring->timer = level;
        obj_to_room( spring, ch->in_room );

        act( "$p flows from the ground.", ch, spring, NULL, TO_CHAR );
        act( "$p flows from the ground.", ch, spring, NULL, TO_ROOM );
        return;
}


void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj   = (OBJ_DATA *) vo;
        int       water;

        if ( obj->item_type != ITEM_DRINK_CON )
        {
                send_to_char( "It is unable to hold water.\n\r", ch );
                return;
        }

        if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
        {
                send_to_char( "It contains some other liquid.\n\r", ch );
                return;
        }

        water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 ),
                     obj->value[0] - obj->value[1] );

        if ( water > 0 )
        {
                obj->value[2] = LIQ_WATER;
                obj->value[1] += water;

                if ( !is_name( "water", obj->name ) )
                {
                        char buf [ MAX_STRING_LENGTH ];

                        sprintf( buf, "%s water", obj->name );
                        free_string( obj->name );
                        obj->name = str_dup( buf );
                }
                act( "$p is filled.", ch, obj, NULL, TO_CHAR );
        }

        return;
}


void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (!IS_AFFECTED(victim, AFF_BLIND))
        {
                send_to_char( "They can see just fine.\n\r", ch );
                return;
        }

        affect_strip(victim, gsn_blindness);
        affect_strip(victim, gsn_gouge);
        affect_strip(victim, gsn_dirt);

        if (ch != victim)
                send_to_char( "You restore their vision.\n\r", ch );

        send_to_char( "Your vision returns!\n\r", victim );
}


void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char wound[100];
        char buf[MAX_STRING_LENGTH];
        int percent;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int heal;

        heal = dice( 3, 8 ) + level - 6;

        if( victim->hit > victim->max_hit )
                return;

        victim->hit = UMIN( victim->hit + heal, victim->max_hit - victim->aggro_dam );
        update_pos( victim );

        /* added for more info - Brutus */
        if ( ch != victim )
        {
                if (victim->max_hit > 0)
                        percent = victim->hit * 100 / victim->max_hit;
                else
                        percent = -1;

                if (percent >= 100)
                        sprintf(wound,"is in excellent condition.");
                else if (percent >= 90)
                        sprintf(wound,"has a few scratches.");
                else if (percent >= 75)
                        sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50)
                        sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30)
                        sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15)
                        sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)
                        sprintf(wound,"is in awful condition.");
                else
                        sprintf(wound,"is bleeding to death.");

                sprintf(buf,"%s %s \n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name,wound);
                send_to_char( buf, ch );
        }

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel better!\n\r", victim );
}


void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char wound[100];
        char buf[MAX_STRING_LENGTH];
        int percent;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int heal;

        heal = dice( 1, 8 ) + level / 3;

        if( victim->hit > victim->max_hit )
                return;

        victim->hit = UMIN( victim->hit + heal, victim->max_hit - victim->aggro_dam );
        update_pos( victim );

        if ( ch != victim )
        {
                if (victim->max_hit > 0)
                        percent = victim->hit * 100 / victim->max_hit;
                else
                        percent = -1;

                if (percent >= 100)
                        sprintf(wound,"is in excellent condition.");
                else if (percent >= 90)
                        sprintf(wound,"has a few scratches.");
                else if (percent >= 75)
                        sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50)
                        sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30)
                        sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15)
                        sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)
                        sprintf(wound,"is in awful condition.");
                else
                        sprintf(wound,"is bleeding to death.");

                sprintf(buf,"%s %s \n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name,wound);
                send_to_char( buf, ch );
        }

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel better!\n\r", victim );
}


void spell_cure_mana( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int restore;

        restore = dice( 4, 8 );

        if (victim->mana < victim->max_mana)
        {
                victim->mana = UMIN( victim->mana + restore, victim->max_mana );
                update_pos( victim );
        }

        send_to_char( "You feel a magical rush.\n\r", victim );
        return;
}


void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (!is_affected(victim, gsn_poison))
                return;

        affect_strip(victim, gsn_poison);

        if (ch != victim)
                send_to_char("You purge the poison from $M.\n\r", ch);

        send_to_char("A warm feeling runs through your body.\n\r", victim);
        act("$N looks better.", ch, NULL, victim, TO_NOTVICT);

}


void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char wound[100];
        char buf[MAX_STRING_LENGTH];
        int percent;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int heal;

        heal = dice( 2, 8 ) + level / 2 ;

        if( victim->hit > victim->max_hit )
                return;

        victim->hit = UMIN( victim->hit + heal, victim->max_hit - victim->aggro_dam);
        update_pos( victim );

        if ( ch != victim )
        {
                if (victim->max_hit > 0)
                        percent = victim->hit * 100 / victim->max_hit;
                else
                        percent = -1;

                if (percent >= 100)
                        sprintf(wound,"is in excellent condition.");
                else if (percent >= 90)
                        sprintf(wound,"has a few scratches.");
                else if (percent >= 75)
                        sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50)
                        sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30)
                        sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15)
                        sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)
                        sprintf(wound,"is in awful condition.");
                else
                        sprintf(wound,"is bleeding to death.");

                sprintf(buf,"%s %s \n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name,wound);
                send_to_char( buf, ch );
        }

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel better!\n\r", victim );
        return;
}


void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell( level, victim ) )
        {
                send_to_char( "You have failed.\n\r", ch );
                return;
        }

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_HITROLL;
        af.modifier  = -1;
        af.bitvector = AFF_CURSE;
        affect_to_char( victim, &af );

        af.location  = APPLY_SAVING_SPELL;
        af.modifier  = 1;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "You curse their existence.\n\r", ch );

        send_to_char( "You feel unclean.\n\r", victim );
        return;
}


void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_EVIL;
        affect_to_char( victim, &af );

        send_to_char( "You are now aware of the presence of evil.\n\r", victim );

        return;
}


void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_GOOD ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_GOOD;
        affect_to_char( victim, &af );

        send_to_char( "You now detect the presence of good.\n\r", victim );

        return;
}


void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
                return;

        af.type      = gsn_detect_hidden;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_HIDDEN;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They now detect hidden.\n\r", ch );

        send_to_char( "Your awareness improves.\n\r", victim );
        return;
}


void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
                return;

        af.type      = gsn_detect_invis;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_INVIS;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They can now see invisible things.\n\r", ch );

        send_to_char( "Your eyes tingle.\n\r", victim );
        return;
}


void spell_detect_sneak( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, gsn_detect_sneak ) )
                return;

        af.type      = gsn_detect_sneak;
        af.duration  = level / 2;
        af.location  = APPLY_NONE;
        af.modifier  = -1;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They now detect sneak.\n\r", ch );

        send_to_char( "You feel highly observant.\n\r", victim );
        return;
}


void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_MAGIC ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_MAGIC;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They are now sensitive to magic.\n\r", ch );

        send_to_char( "The magic around you arouses your senses.\n\r", victim);
        return;
}

void spell_detect_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_CURSE ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_CURSE;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "They now have an affinity for the accursed.\n\r", ch );

        send_to_char( "You feel an affinity for the accursed.\n\r", victim);
        return;
}


void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;

        if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
        {
                if ( obj->value[3] != 0 )
                        send_to_char( "You smell poisonous fumes.\n\r", ch );
                else
                        send_to_char( "It looks very delicious.\n\r", ch );
        }
        else
        {
                send_to_char( "It doesn't look poisoned.\n\r", ch );
        }

        return;
}


void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{

        /* Made some changes so imm dispel (hopefully) always works and on everything --Owl 19/2/22 */

        CHAR_DATA   *victim = (CHAR_DATA *) vo;
        AFFECT_DATA *paf;

        if (!IS_NPC(ch) && !IS_NPC(victim))
        {
                /* Defensive spell - remove ALL effects */

                if (victim == ch || ch->level > LEVEL_HERO)
                {
                        for ( paf = victim->affected; paf; paf = paf->next )
                        {
                                if (paf->duration < 0)
                                        continue;

                                if (effect_is_prayer(paf) && ch->level < L_IMM)
                                        continue;

                                if (paf->deleted)
                                        continue;

                                if (paf->type == gsn_mount && ch->mount)
                                        continue;

                                if (skill_cannot_be_dispelled(paf->type)
                                    && ch->level <= LEVEL_HERO)
                                        continue;

                                affect_remove( victim, paf );
                        }

                        if (ch == victim)
                        {
                                act ("You have removed all magic effects from yourself.",
                                     ch, NULL, NULL, TO_CHAR );
                                act ("$c has removed all magic effects from $mself.",
                                     ch, NULL, NULL, TO_ROOM );
                        }
                        else
                        {
                                act ("You have removed all magic effects from $N.",
                                     ch, NULL, victim, TO_CHAR);
                                act ("$c has removed all magic effects from you.",
                                     ch, NULL, victim, TO_VICT);
                                act ("$c has removed all magic effects from $N.",
                                     ch, NULL, victim, TO_NOTVICT);
                        }
                }
                else
                {
                        /* Find the first spell and remove it */

                        arena_commentary("$n casts dispel magic at $N.", ch, victim);

                        for ( paf = victim->affected; paf; paf = paf->next )
                        {
                                if (paf->duration < 0)
                                        continue;

                                if (effect_is_prayer(paf))
                                        continue;

                                if ( paf->type == gsn_mount && ch->mount)
                                        continue;

                                if (skill_cannot_be_dispelled(paf->type))
                                        continue;

                                if (paf->deleted)
                                        continue;
                                send_to_char("You attempt to dispel magical effects.\n\r", ch);
                                send_to_char("You feel a strange sensation.\n\r", victim);
                                send_to_char( skill_table[paf->type].msg_off, victim );
                                send_to_char( "\n\r", victim );

                                act( "$n is no longer affected by '$t'.",
                                    victim, skill_table[paf->type].name, NULL, TO_ROOM );

                                affect_strip( victim, paf->type );

                                break;
                        }
                }

                return;
        }
        else  /* either ch or victim is an NPC */
        {
                send_to_char("You attempt to dispel magical effects.\n\r", ch);
                send_to_char("You feel a strange sensation.\n\r", victim);

                for ( paf = victim->affected; paf; paf = paf->next )
                {
                        if (paf->duration < 0)
                                continue;

                        if (effect_is_prayer(paf) && !(IS_IMMORTAL( ch )))
                                continue;

                        if ( paf->type == gsn_mount && victim->mount)
                                continue;

                        if ( paf->deleted )
                                continue;

                        if (skill_cannot_be_dispelled(paf->type))
                                continue;

                        if ( !saves_spell( level, victim ) || (IS_IMMORTAL( ch )))
                        {
                                send_to_char( skill_table[paf->type].msg_off, victim );
                                send_to_char( "\n\r", victim );

                                act( "$n is no longer affected by '$t'.",
                                    victim, skill_table[paf->type].name, NULL, TO_ROOM );

                                affect_strip( victim, paf->type );
                                return;
                        }
                }

                /*
                 * Can't dispel sanctuary, fireshield or globe if player has
                 * prayer of protection; Gezhp
                 *
                 * Can if you're an imm - Owl 19/2/22)
                 */

                if (is_affected(victim, gsn_prayer_protection) && !(IS_IMMORTAL( ch )))
                        return;

                /* ALWAYS give a shot at removing sanctuary */
                if (IS_AFFECTED(victim, AFF_SANCTUARY) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))) )
                {
                        REMOVE_BIT(victim->affected_by, AFF_SANCTUARY);
                        send_to_char( "The white aura around your body fades.\n\r", victim );
                        act( "The white aura around $n's body fades.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                /* and globe - this is ugly */
                if (IS_AFFECTED(victim, AFF_GLOBE) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_GLOBE);
                        send_to_char( "The globe around your body dissipates.\n\r", victim );
                        act( "The globe around $n's body fades.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                /* and fireshield - this is ugly too */
                if (IS_AFFECTED(victim, AFF_FLAMING) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_FLAMING);
                        send_to_char( "The flames around your body fizzle out.\n\r", victim );
                        act( "The flames around $n's body fade.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                /*
                 * Continuing the ugliness for AFF flags on mobs we want players to be
                 * able to dispel (not set in stone, make imm-only if creates problems).
                 * - Owl 19/2/22
                 */

                if (IS_AFFECTED(victim, AFF_PROTECT) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_PROTECT);
                        send_to_char( "You feel less protected.\n\r", victim );
                        act( "$n looks more vulnerable.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_INVISIBLE) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_INVISIBLE);
                        send_to_char( "You are no longer invisible.\n\r", victim );
                        act( "$n fades back into existence.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_FAERIE_FIRE) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_FAERIE_FIRE);
                        send_to_char( "You are no longer surrounded by a pink outline.\n\r", victim );
                        act( "The pink outline around $n's body disappears.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_POISON) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_POISON);
                        send_to_char( "You are no longer sick.\n\r", victim );
                        act( "$n no longer looks sick.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_CURSE) && (!saves_spell(level, victim ) || (IS_IMMORTAL( ch ))))
                {
                        REMOVE_BIT(victim->affected_by, AFF_CURSE);
                        send_to_char( "You are no longer cursed.\n\r", victim );
                        act( "$n is no longer accursed.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                /* AFF bits that only imms can dispel, i.e. the rest of them. -- Owl 19/2/22 */

                if (IS_AFFECTED(victim, AFF_BATTLE_AURA) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_BATTLE_AURA);
                        send_to_char( "The silvery glow around you fades.\n\r", victim );
                        act( "$n seems less ready for battle.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_HOLD) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_HOLD);
                        send_to_char( "You are no longer entrapped.\n\r", victim );
                        act( "$n is released from $s confinement.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_BLIND) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_BLIND);
                        if (HAS_EYES(victim))
                        {
                                send_to_char( "You can see!\n\r", victim );
                                act( "$n's vision has returned.", victim, NULL, NULL, TO_ROOM );
                        }
                        else
                        {
                                send_to_char( "You are no longer affected by AFF_BLIND.\n\r", victim );
                                act( "The AFF_BLIND effect is stripped from $n.", victim, NULL, NULL, TO_ROOM );
                        }
                        return;
                }

                if (IS_AFFECTED(victim, AFF_CHARM) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_CHARM);
                        send_to_char( "You feel more self-confident.\n\r", victim );
                        act( "$n's mind is no longer under the dominion of another.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_HOLD) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_HOLD);
                        send_to_char( "You are no longer entrapped.\n\r", victim );
                        act( "$n is released from $s confinement.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_FLYING) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_FLYING);
                        send_to_char( "You are no longer flying.\n\r", victim );
                        act( "$n slowly floats to the ground.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_SLEEP) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_SLEEP);
                        send_to_char( "You wake up!\n\r", victim );
                        act( "$n wakes up!", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_HIDE) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_HIDE);
                        send_to_char( "You are no longer hiding.\n\r", victim );
                        act( "$n emerges from $s hiding place.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_PASS_DOOR) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_PASS_DOOR);
                        send_to_char( "You become tangible.\n\r", victim );
                        act( "$n solidifies.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_MEDITATE) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_MEDITATE);
                        send_to_char( "You awaken from your trance.\n\r", victim );
                        act( "$n awakens from $s trance.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETER) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETER);
                        send_to_char( "You feel less intimidating.\n\r", victim );
                        act( "$n seems less intimidating.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_SNEAK) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_SNEAK);
                        send_to_char( "You feel less stealthy.\n\r", victim );
                        act( "$n stops sneaking about.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_SWIM) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_SWIM);
                        send_to_char( "You suddenly forget how to swim!\n\r", victim );
                        act( "$n forgets how to swim!", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_PRAYER_PLAGUE) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_PRAYER_PLAGUE);
                        send_to_char( "You are no longer affected by the plague prayer.\n\r", victim );
                        act( "$n seems a LOT less sickly.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_NON_CORPOREAL) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_NON_CORPOREAL);
                        send_to_char( "You return to your corporeal form.\n\r", victim );
                        act( "$n returns to $s corporeal form.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_EVIL) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_EVIL);
                        send_to_char( "The red in your vision disappears.\n\r", victim );
                        act( "$n can no longer detect evil.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_INVIS) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_INVIS);
                        send_to_char( "You no longer see invisible objects.\n\r", victim );
                        act( "$n can no longer see invisible objects.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_GOOD) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_GOOD);
                        send_to_char( "The yellow in your vision disappears.\n\r", victim );
                        act( "$n can no longer detect goodness.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_MAGIC) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_MAGIC);
                        send_to_char( "The blue in your vision disappears.\n\r", victim );
                        act( "$n can no longer detect magic.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_HIDDEN) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_HIDDEN);
                        send_to_char( "You feel less aware of your surroundings.\n\r", victim );
                        act( "$n seems less aware of $s surroundings.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_TRAPS) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_TRAPS);
                        send_to_char( "You feel less perspicacious.\n\r", victim );
                        act( "$n seems less perspicacious.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_CURSE) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_CURSE);
                        send_to_char( "You no longer feel an affinity for the accursed.\n\r", victim );
                        act( "$n seems less attuned to the accursed.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_DETECT_SNEAK) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_DETECT_SNEAK);
                        send_to_char( "You feel less observant.\n\r", victim );
                        act( "$n seems less observant.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                if (IS_AFFECTED(victim, AFF_INFRARED) && IS_IMMORTAL( ch ))
                {
                        REMOVE_BIT(victim->affected_by, AFF_INFRARED);
                        send_to_char( "You no longer see in the dark.\n\r", victim );
                        act( "$n can no longer see in the dark.", victim, NULL, NULL, TO_ROOM );
                        return;
                }

                send_to_char( "They don't seem to be affected by anything.\n\r", ch );
                return;
        }
}


void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
        {
                send_to_char( "You are too EVIL to cast this.\n\r", ch );
                return;
        }

        if ( IS_GOOD( victim ) )
        {
                act( "God protects $N.", ch, NULL, victim, TO_ROOM );
                return;
        }

        if ( IS_NEUTRAL( victim ) )
        {
                act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
                return;
        }

        dam = dice(level, 4) + level;

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_earthquake (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *vch;

        send_to_char( "The earth trembles beneath your feet!\n\r", ch );
        act( "$c makes the earth tremble and shiver!", ch, NULL, NULL, TO_ROOM );

        for ( vch = char_list; vch; vch = vch->next )
        {
                if ( vch->deleted || !vch->in_room )
                        continue;

                if ( vch->in_room == ch->in_room )
                {
                        if (vch == ch->mount
                            || is_group_members_mount (vch, ch)
                            || is_same_group(vch, ch))
                                continue;

                        if (!mob_interacts_players(vch))
                                continue;

                        if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
                                damage( ch, vch, dice(level, 4) + level, sn, FALSE );

                        continue;
                }

                if ( vch->in_room->area == ch->in_room->area )
                {
                        send_to_char( "The earth trembles and shivers.\n\r", vch );
                        if ( !IS_NPC( vch ) && number_percent( ) < 50 )
                        {
                                send_to_char("You are knocked to the ground by the tremor.\n\r", vch );
                                vch->position = POS_RESTING;
                                update_pos( vch );
                        }
                }
        }
}


void spell_enchant_weapon(int sn, int level, CHAR_DATA *ch, void *vo)
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;
        AFFECT_DATA *paf;

        if (obj->item_type != ITEM_WEAPON
            || IS_OBJ_STAT(obj, ITEM_MAGIC)
            || obj->affected)
        {
                send_to_char("That item cannot be enchanted.\n\r", ch);
                return;
        }

        if (!affect_free)
        {
                paf = alloc_perm( sizeof( *paf ) );
        }
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type       = sn;
        paf->duration   = -1;
        paf->location   = APPLY_HITROLL;
        paf->modifier   = level / 5;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;

        if (!affect_free)
        {
                paf = alloc_perm( sizeof( *paf ) );
        }
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type       = sn;
        paf->duration   = -1;
        paf->location   = APPLY_DAMROLL;
        paf->modifier   = level / 10;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;
        obj->level      = UMAX(ch->level,  obj->level +1 );

        if ( IS_GOOD( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL);
                act( "$p glows blue.",   ch, obj, NULL, TO_CHAR );
        }
        else if ( IS_EVIL( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p glows red.",    ch, obj, NULL, TO_CHAR );
        }
        else
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p glows yellow.", ch, obj, NULL, TO_CHAR );
        }

        SET_BIT(obj->extra_flags, ITEM_MAGIC);
}


/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        if ( saves_spell( level, victim ) || victim->edrain)
        {
                damage(ch, victim, 0, sn, FALSE);
                return;
        }

        send_to_char("{RYou drain life force from your victim!\n\r{x",ch);

        ch->alignment = UMAX(-1000, ch->alignment - 200);
        if ( victim->level <= 2 )
        {
                dam              = 2 * ch->hit + 1;
        }
        else
        {
                gain_exp( victim, 0 - number_range( level / 2, 3 * level / 2 ) );
                victim->mana    /= 2;
                victim->hit     /= 2;
                dam              = level / 2;
                victim->edrain   = 5;
        }

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_entrapment( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = ( CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_safe(ch, victim))
                return;

        if (victim->fighting || (!IS_NPC(victim) && saves_spell(level, victim)))
        {
                send_to_char("Your entrapment fails.\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(victim, AFF_HOLD) )
        {
                send_to_char( "They are already held.\n\r", ch );
                return;
        }

        af.type = sn;
        af.duration = level / 20;
        af.location = APPLY_HITROLL;
        af.modifier = -8;
        af.bitvector= AFF_HOLD;
        affect_to_char(victim, &af);

        act( "You successfully immobilise $N in your stasis field.", ch, NULL, victim, TO_CHAR );
        act( "A stasis field surrounds you, preventing you from moving.",
            ch, NULL, victim, TO_VICT);
        act( "A stasis field forms around $N, preventing $m from moving.",
            ch, NULL, victim, TO_ROOM );
        return;
}


void spell_fireball (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 6);

        if (is_affected(victim, gsn_resist_heat) || saves_spell(level, victim))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("You conjure a fireball and hurl it towards $N!", ch, NULL, victim, TO_CHAR);
                act("$c conjures a fireball and hurls it towards $N!", ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch, victim, dam, sn, FALSE);
}


void spell_fireshield (int sn, int level, CHAR_DATA *ch, void *vo)
{
        AFFECT_DATA  af;
        CHAR_DATA  *victim = (CHAR_DATA *) vo;

        if (IS_AFFECTED( victim, AFF_FLAMING ) )
                return;

        af.type = sn;
        af.duration = number_fuzzy( level / 12 );
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector= AFF_FLAMING;
        affect_to_char( victim, &af );

        send_to_char( "A flaming aura forms around you!\n\r", victim );
        act("The air around $n's form bursts into flame!",victim,NULL,NULL,TO_ROOM);
        return;
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int        dam;

        dam = dice(level, 4 );
        dam += level * 2;

        if (is_affected(victim, gsn_resist_heat) || saves_spell(level, victim))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("You channel holy fire from $D!", ch, NULL, victim, TO_CHAR);
                act("$c channels holy fire from $s god!", ch, NULL, victim, TO_NOTVICT);
        }

        damage( ch, victim, dam, sn, FALSE );
}


void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
        {
                send_to_char("They are already surrounded by a pink outline.\n\r", ch);
                return;
        }

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_AC;
        af.modifier  = 2 * level;
        af.bitvector = AFF_FAERIE_FIRE;
        affect_to_char( victim, &af );

        send_to_char( "You are surrounded by a pink outline.\n\r", victim );
        act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *ich;

        send_to_char( "You conjure a cloud of {Mpurple smoke{x!\n\r", ch );
        act( "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );

        for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
        {
                if (!mob_interacts_players(ich))
                        continue;

                if ( ich == ch || saves_spell( level, ich ) )
                        continue;

                affect_strip ( ich, gsn_invis                   );
                affect_strip ( ich, gsn_mass_invis              );
                affect_strip ( ich, gsn_sneak                   );
                affect_strip ( ich, gsn_shadow_form             );
                REMOVE_BIT   ( ich->affected_by, AFF_HIDE       );
                REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE  );
                REMOVE_BIT   ( ich->affected_by, AFF_SNEAK      );
                act( "$n is revealed!", ich, NULL, NULL, TO_ROOM );
                send_to_char( "You are revealed!\n\r", ich );
        }

        return;
}


void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_FLYING ) )
                return;

        af.type      = gsn_fly;
        af.duration  = level + 3;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_FLYING;
        affect_to_char( victim, &af );

        send_to_char( "Your feet rise off the ground.\n\r", victim );
        act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *gch;
        CHAR_DATA *mob;
        int npccount  = 0;
        int pccount   = 0;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
                        npccount++;
                if ( !IS_NPC( gch ) ||
                    ( IS_NPC( gch ) && IS_AFFECTED( gch, AFF_CHARM ) ) )
                        pccount++;
        }

        do_say( ch, "{RCome brothers!  Join me in this glorious bloodbath!{x" );

        mob = create_mobile(get_mob_index(MOB_VNUM_VAMPIRE));
        /* Tweaked slightly to make  spell a little more dangerous. 9/4/15 --Owl */
        mob->level = ch->level - 7;
        mob->max_hit = mob->level * 8 + number_range(mob->level * mob->level / 4,
                                                     mob->level * mob->level);
        mob->hit = mob->max_hit;

        char_to_room(mob, ch->in_room);
        return;
}


/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 6);

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, gsn_giant_strength)
            || is_affected(victim, gsn_enhanced_strength))
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_STR;
        af.modifier  = 1 + (level >= 18) + (level >= 25);
        af.bitvector = 0;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "You strengthen them.\n\r", ch );
        send_to_char( "You feel stronger.\n\r", victim );
        return;
}


void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        if ( saves_spell( level, victim ) )
                dam = 50;
        else
                dam = 100;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char wound[100];
        char buf[MAX_STRING_LENGTH];
        int percent;

        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (victim == ch->fighting)
                return;

        if (victim->hit > victim->max_hit)
                return;

        victim->hit = UMIN( victim->hit + 100, victim->max_hit - victim->aggro_dam);
        update_pos( victim );

        if ( ch != victim )
        {
                if (victim->max_hit > 0)
                        percent = victim->hit * 100 / victim->max_hit;
                else
                        percent = -1;

                if (percent >= 100)
                        sprintf(wound,"is in excellent condition.");
                else if (percent >= 90)
                        sprintf(wound,"has a few scratches.");
                else if (percent >= 75)
                        sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50)
                        sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30)
                        sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15)
                        sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)
                        sprintf(wound,"is in awful condition.");
                else
                        sprintf(wound,"is bleeding to death.");

                sprintf(buf,"%s %s \n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name,wound);
                send_to_char( buf, ch );
                send_to_char( "Ok.\n\r", ch );
        }

        send_to_char( "A warm feeling fills your body.\n\r", victim );
        return;
}


void spell_power_heal (int sn, int level, CHAR_DATA *ch, void *vo)
{
        char wound[100];
        char buf[MAX_STRING_LENGTH];
        int percent;

        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (victim == ch->fighting)
                return;

        if (victim->hit > victim->max_hit)
                return;

        victim->hit = UMIN(victim->hit + 300, victim->max_hit - victim->aggro_dam );
        update_pos(victim);

        if (ch != victim)
        {
                if (victim->max_hit > 0)
                        percent = victim->hit * 100 / victim->max_hit;
                else
                        percent = -1;

                if (percent >= 100)
                        sprintf(wound,"is in excellent condition.");
                else if (percent >= 90)
                        sprintf(wound,"has a few scratches.");
                else if (percent >= 75)
                        sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50)
                        sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30)
                        sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15)
                        sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)
                        sprintf(wound,"is in awful condition.");
                else
                        sprintf(wound,"is bleeding to death.");

                sprintf(buf,"%s %s \n\r",
                        IS_NPC(victim) ? victim->short_descr : victim->name,wound);
                send_to_char( buf, ch );
        }

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );
        send_to_char( "A pulse of energy surges through your body.\n\r", victim );
        return;
}


/*
 * Ignores pickproofs, but can't unlock containers. -- Altrag 17/2/96
 */
void spell_knock( int sn, int level, CHAR_DATA *ch, void *vo )
{
        char arg [MAX_STRING_LENGTH];
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;
        ROOM_INDEX_DATA *to_room;
        int door;

        one_argument( target_name, arg);

        if ( arg[0] == '\0' )
        {
                send_to_char( "What do you want to try and unlock?\n\r", ch );
                return;
        }

        /*
         * shouldn't know why it didn't work, and shouldn't work on pickproof
         * exits.  -Thoric
         */

        if ( (door=find_door(ch, arg)) >= 0 )
        {
                pexit = ch->in_room->exit[door];

                if (!IS_SET(pexit->exit_info, EX_CLOSED)
                    || !IS_SET(pexit->exit_info, EX_LOCKED)
                    || IS_SET(pexit->exit_info, EX_BASHPROOF))
                {
                        send_to_char( "The door resists your magic.\n\r", ch );
                        return;
                }

                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                send_to_char( "Your magic bursts the lock open.\n\r", ch );

                if ( ( to_room   = pexit->to_room )
                    && ( pexit_rev = to_room->exit[directions[door].reverse] )
                    && pexit_rev->to_room == ch->in_room )
                        REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
                return;
        }
}


/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 6);

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_identify (int sn, int level, CHAR_DATA *ch, void *vo)
{
        /*
         *  Rewritten by Gezhp, 2000
         */

        OBJ_DATA    *obj = (OBJ_DATA *) vo;
        AFFECT_DATA *paf;
        int          i, j, k, list [50];
        char         buf [MAX_STRING_LENGTH];
        char         tmp [MAX_STRING_LENGTH];

        const char* type [MAX_ITEM_TYPE+1] =
        {
                "something strange",    "a light source",       "a scroll",
                "a wand",               "a staff",              "a weapon",
                "something strange",    "something strange",    "treasure",
                "a piece of armour",    "a potion",             "something strange",
                "a piece of furniture", "a piece of trash",     "something strange",
                "a container",          "something strange",    "a drink container",
                "a key",                "food",                 "money",
                "something strange",    "a boat",               "a corpse",
                "a corpse",             "a fountain",           "a pill",
                "some climbing equipment",                      "some paint",
                "something strange",    "an anvil",             "an auction ticket",
                "a special clan artefact",                      "a magical portal",
                "some poison powder",   "a lockpick",           "a musical instrument",
                "an armourer's hammer", "some mithril",         "a whetstone"
        };

        const char* extras [31] =
        {
                "glows faintly",        "emits a low humming noise",
                "?",                    "?",                    "is evil",
                "is invisible",         "has a magical aura",   "cannot be dropped",
                "has been blessed",     "?",                    "?",
                "?",
                "cannot be removed",    "?",                    "is coated in poison",
                "?",                    "?",                    "?",
                "?",                    "?",                    "is a vorpal weapon",
                "?",                    "?",                    "is bloodthirsty",
                "has been sharpened",   "has been forged",      "is a body part",
                "can be used as a lance",                       "?",
                "?",                    "can be used as a bow"
        };

        const int class_restrictions [MAX_CLASS] =
        {
                ITEM_ANTI_MAGE,         ITEM_ANTI_CLERIC,       ITEM_ANTI_THIEF,
                ITEM_ANTI_WARRIOR,      ITEM_ANTI_PSIONIC,      ITEM_ANTI_SHAPE_SHIFTER,
                ITEM_ANTI_BRAWLER,      ITEM_ANTI_RANGER
        };

        const int align_restriction_bits [3] =
        {
                ITEM_ANTI_GOOD,         ITEM_ANTI_NEUTRAL,      ITEM_ANTI_EVIL
        };

        const char* align_restriction_names [3] =
        {
                "good",                 "neutral",              "evil"
        };

        const char* wear_slots [18] =
        {
                "?",                            "is worn on the fingers",
                "is worn about the neck",       "is worn on the body",
                "is worn on the head",          "is worn on the legs",
                "is worn on the feet",          "is worn on the hands",
                "is worn on the arms",          "is used as a shield",
                "is worn about the body",       "is worn around the waist",
                "is worn on the wrist",         "?",
                "is held in the hands",         "floats about your head",
                "is worn as a belt pouch",      "?"
        };


        /*
         *  Name and type
         */
        sprintf (buf, "You determine that {W%s{x is %s.\n\r",
                 obj->short_descr,
                 (obj->item_type < 0 || obj->item_type >= MAX_ITEM_TYPE+1)
                 ? "something strange" : type[obj->item_type]);
        send_paragraph_to_char (buf, ch, 4);

        /*
         *  Wear position if sensible
         */
        for (j = 1, i = 2; j < 18; j++, i *= 2)
        {
                if ((obj->wear_flags & i) && strcmp (wear_slots[j], "?"))
                {
                        sprintf(buf, "It %s.\n\r", wear_slots[j]);
                        send_to_char(buf, ch);
                        break;
                }
        }

        /*
         *  Any extra flags
         */
        i = 0;
        for (j = 0, k = 1; j < 31; j++, k *= 2)
        {
                if ((k & obj->extra_flags) && strcmp (extras[j], "?"))
                        list[i++] = j;
        }
        if (i)
        {
                sprintf (buf, "{cIt");
                for (j = 0; j < i; j++)
                {
                        if (j == i-1 && j)
                                strcat (buf, " and");
                        else if (j)
                                strcat (buf, ",");
                        sprintf (tmp, " %s", extras[list[j]]);
                        strcat (buf, tmp);
                }
                strcat (buf, ".{x\n\r");
                send_paragraph_to_char (buf, ch, 4);
        }

        /*
         *  Any alignment restrictions
         */
        i = 0;
        for (j = 0; j < 3; j++)
        {
                if (align_restriction_bits[j] & obj->extra_flags)
                        list[i++] = j;
        }
        if (i)
        {
                sprintf (buf, "{cIt cannot be equipped by those of");
                for (j = 0; j < i; j++)
                {
                        if (j == i-1 && j)
                                strcat (buf, " or");
                        else if (j)
                                strcat (buf, ",");
                        sprintf (tmp, " %s", align_restriction_names[list[j]]);
                        strcat (buf, tmp);
                }
                strcat (buf, " alignment.{x\n\r");
                send_paragraph_to_char (buf, ch, 4);
        }

        /*
         *  Any class restrictions
         */
        i = 0;
        for (j = 0; j < MAX_CLASS; j++)
        {
                if (class_restrictions[j] & obj->extra_flags)
                        list[i++] = j;
        }
        if (i)
        {
                sprintf (buf, "{cIt cannot be used by those of the");
                for (j = 0; j < i; j++)
                {
                        if (j == i-1 && j)
                                strcat (buf, " or");
                        else if (j)
                                strcat (buf, ",");
                        strcat (buf, " ");
                        strcat (buf, full_class_name(list[j]));
                }
                sprintf (tmp, " class%s.{x\n\r",
                         (i == 1) ? "" : "es");
                strcat (buf, tmp);
                send_paragraph_to_char (buf, ch, 4);
        }

        if (IS_SET(obj->extra_flags, ITEM_EGO) && obj->ego_flags)
                send_to_char("There is something special about this item.\n\r", ch);

        sprintf( buf, "It weighs {W%d{x lbs, is worth {W%d{x copper coins and is level {W%d{x.\n\r",
                obj->weight,
                obj->cost,
                obj->level );
        send_paragraph_to_char (buf, ch, 4);

        /*
         *  Item type specific information
         */
        switch (obj->item_type)
        {
            case ITEM_PILL:
            case ITEM_PAINT:
            case ITEM_SCROLL:
            case ITEM_POTION:
                i = 0;
                for (j = 1; j < 4; j++)
                {
                        if ( obj->value[j] >= 0 && obj->value[j] < MAX_SKILL )
                                list[i++] = obj->value[j];
                }
                if (i)
                {
                        sprintf( buf, "It contains %slevel {W%d{X spell%s of",
                                (i == 1) ? "a " : "",
                                obj->value[0],
                                (i == 1) ? "" : "s");
                        for (j = 0; j < i; j++)
                        {
                                if (j == i-1 && j)
                                        strcat (buf, " and");
                                else if (j)
                                        strcat (buf, ",");
                                sprintf (tmp, " '{C%s{x'", skill_table[list[j]].name);
                                strcat (buf, tmp);
                        }
                        strcat (buf, ".\n\r");
                        send_paragraph_to_char (buf, ch, 4);
                }
                break;

            case ITEM_WAND:
            case ITEM_STAFF:
                if (obj->value[2] == 1 && obj->value[1] == 1)
                        sprintf(buf, "It contains {W1{x charge of level {W%d{x",
                                obj->value[0]);
                else
                        sprintf( buf, "It contains {W%d{x out of {W%d{x charges of level {W%d{x",
                                obj->value[2], obj->value[1], obj->value[0] );

                j = 0;
                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ) {
                        j++;
                        strcat( buf, " '{C" );
                        strcat( buf, skill_table[obj->value[3]].name );
                        strcat( buf, "{x'" );
                }

                strcat (buf, ".\n\r");
                if (j) send_paragraph_to_char( buf, ch, 4 );
                break;

            case ITEM_WEAPON:
                sprintf( buf, "This weapon does between {W%d{x and {W%d{x base points of damage.\n\r",
                        obj->value[1],
                        obj->value[2]);
                send_paragraph_to_char( buf, ch, 4 );
                break;

            case ITEM_ARMOR:
                sprintf( buf, "It has an armour class of {W%d{x.\n\r", obj->value[0] );
                send_to_char( buf, ch );
                break;
        }

        for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
        {
                if ( paf->location != APPLY_NONE && paf->modifier != 0
                    && strcmp (affect_loc_name (paf->location), "(unknown)"))
                {
                        if (paf->location < APPLY_SANCTUARY)
                                sprintf( buf, "It modifies {Y%s{x by {Y%d{x.\n\r",
                                        affect_loc_name( paf->location ), paf->modifier );
                        else
                                sprintf (buf, "It gives the wearer {Y%s{x.\n\r",
                                         affect_loc_name (paf->location));
                        send_to_char( buf, ch );
                }
        }

        for ( paf = obj->affected; paf; paf = paf->next )
        {
                if ( paf->location != APPLY_NONE
                    && paf->modifier != 0
                    && strcmp (affect_loc_name (paf->location), "(unknown)"))
                {
                        if (paf->location < APPLY_SANCTUARY)
                                sprintf( buf, "It modifies {Y%s{x by {Y%d{x.\n\r",
                                        affect_loc_name( paf->location ), paf->modifier );
                        else
                                sprintf (buf, "It gives the wearer {Y%s{x.\n\r",
                                         affect_loc_name (paf->location));
                        send_to_char( buf, ch );
                }
        }
}


void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (IS_AFFECTED(victim, AFF_INFRARED))
                return;

        af.type      = gsn_infravision;
        af.duration  = 2 + level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_INFRARED;
        affect_to_char(victim, &af);

        send_to_char( "Your eyes glow red.\n\r", victim );
        act( "$n's eyes glow red.\n\r", ch, NULL, NULL, TO_ROOM );
        return;
}


void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
                return;

        send_to_char( "You fade out of existence.\n\r", victim );
        act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );

        af.type      = sn;
        af.duration  = 2 + level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_INVISIBLE;
        affect_to_char( victim, &af );

        return;
}


void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        char      *msg;
        int        ap;

        ap = victim->alignment;

        if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
        else if ( ap >  350 ) msg = "$N is of excellent moral character.";
        else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
        else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
        else if ( ap > -350 ) msg = "$N lies to $S friends.";
        else if ( ap > -700 ) msg = "$N's slash tears out your heart!";
        else msg = "I'd rather just not say anything at all about $N.";

        act( msg, ch, NULL, victim, TO_CHAR );
        return;
}


void spell_dark_ritual( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;

        if (obj->item_type != ITEM_CORPSE_NPC)
        {
                send_to_char ("There do not appear to be any fresh corpses about...\n\r", ch );
                return;
        }

        extract_obj ( obj );
        ch->alignment = UMAX(-1000, ch->alignment - ch->level * 2);

        act("You give up an offering to $D and their blessing fills you with ecstasy!\n\r",
            ch, NULL, NULL, TO_CHAR);
        act("$n offers up a foul sacrifice... a darkness surrounds the corpse and it smolders and burns!",
            ch, NULL, NULL, TO_ROOM);
}


void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = dice(level, 4) + level;

        if (is_affected(victim, gsn_resist_lightning)
            || saves_spell(level, victim))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj;
        OBJ_DATA *in_obj;
        char buf [ MAX_INPUT_LENGTH * 15];
        bool found;
        char arg2 [ MAX_INPUT_LENGTH ];

        one_argument( target_name, arg2 );

        if ( arg2[0] == '\0'  )
        {
                send_to_char( "What are you attempting to locate??\n\r", ch );
                return;
        }

        if ( arg2[1] == '\0' )
        {
                send_to_char("Can you be a bit more specific?\n\r", ch);
                return;
        }

        found = FALSE;

        for ( obj = object_list; obj; obj = obj->next )
        {
                if (!multi_keyword_match( arg2, obj->name )
                    || !can_see_obj( ch, obj )
                    || obj->item_type == ITEM_CORPSE_NPC
                    || IS_SET(obj->extra_flags, ITEM_BODY_PART))
                        continue;

                found = TRUE;

                for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
                        ;

                if ( in_obj->carried_by )
                {
                        if (!IS_NPC(in_obj->carried_by) && IS_SET(in_obj->carried_by->act, PLR_WIZINVIS))
                                continue;

                        if (IS_NPC(in_obj->carried_by) && IS_SET(in_obj->carried_by->act, ACT_WIZINVIS_MOB))
                                continue;

                        sprintf( buf, "%s carried by %s.\n\r",
                                obj->short_descr, PERS( in_obj->carried_by, ch ) );
                }
                else
                {
                        sprintf( buf, "%s in %s.\n\r",
                                obj->short_descr, !in_obj->in_room
                                ? "somewhere" : in_obj->in_room->name );
                }

                buf[0] = UPPER( buf[0] );
                send_to_char( buf, ch );
        }

        if (!obj)
                return;

        if ( !found  )
                send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
}


void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA*      victim = (CHAR_DATA *) vo;
        int             dam;
        int             missiles;
        int             i;
        char            buf [MAX_STRING_LENGTH];

        missiles = UMIN((ch->level + 1) / 2, 10);
        dam = 0;

        for (i = 0; i < missiles; i++)
                dam += number_range(2, 5);

        if (missiles == 1)
        {
                act("You launch a magic missile at $N", ch, NULL, victim, TO_CHAR);
                act("$c launches a magic missile at $N", ch, NULL, victim, TO_NOTVICT);
        }
        else
        {
                sprintf(buf, "You launch a volley of %d magic missiles at $N", missiles);
                act(buf, ch, NULL, victim, TO_CHAR);

                sprintf(buf, "$c launches a volley of %d magic missiles at $N", missiles);
                act(buf, ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch, victim, dam, sn, FALSE);
}


void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;
        AFFECT_DATA af;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_INVISIBLE ) )
                        continue;

                send_to_char( "You slowly fade out of existence.\n\r", gch );
                act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );

                af.type      = sn;
                af.duration  = 24;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_INVISIBLE;
                affect_to_char( gch, &af );
        }

        send_to_char( "Ok.\n\r", ch );
        return;
}


void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
        send_to_char( "That's not a spell!\n\r", ch );
        return;
}


void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
                return;

        af.type      = sn;
        af.duration  = number_fuzzy( level / 4 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_PASS_DOOR;
        affect_to_char( victim, &af );

        send_to_char( "You turn translucent.\n\r", victim );
        act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af, *paf;

        if (is_affected(victim, gsn_prayer_plague))
                return;

        /* next bit for resist poison */
        if (!IS_NPC(victim) && number_percent() <
            victim->pcdata->learned[gsn_resist_toxin] && victim->gag < 2)
        {
            send_to_char( "You resist the poison surging through your veins.\n\r",victim );
            return;
        }

        /*
         * Tinkered with by Gezhp:
         * Let's limit the amount of strength that poison can reduce;
         * some of the modifiers and durations I've seen are rediculous
         */

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_STR;
        af.modifier  = -2;
        af.bitvector = AFF_POISON;

        for (paf = victim->affected; paf; paf = paf->next)
        {
                if (paf->type == sn)
                {
                        af.duration = level/10;
                        if (paf->modifier <= -20)
                        {
                                af.modifier = 0;
                                af.duration = 0;
                        }
                        break;
                }
        }

        affect_join( victim, &af );

        if ( ch != victim )
                send_to_char( "You successfully poison your victim.\n\r", ch );

        send_to_char( "You feel very sick.\n\r", victim );
        return;
}


void spell_paralysis( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        af.type      = sn;
        af.duration  = 1 + level / 10;
        af.location  = APPLY_DEX;
        af.modifier  = -5;
        af.bitvector = AFF_HOLD;
        affect_join( victim, &af );

        if ( ch != victim )
                send_to_char( "You successfully paralyze your victim.\n\r", ch );

        send_to_char( "You cannot move, you are paralyzed!\n\r", victim );
}


void spell_protection( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_PROTECT ) )
                return;

        af.type      = sn;
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_PROTECT;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel protected.\n\r", victim );
}


void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (victim->move > victim->max_move)
                return;

        victim->move = UMIN( victim->move + number_range(level * 3/2, level * 5/2),
                            victim->max_move );

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You feel less tired.\n\r", victim );
}


void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int iWear;
        int yesno  = 0;

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                return;

        if (victim->fighting)
                return;

        for ( iWear = 0; iWear < MAX_WEAR; iWear ++ )
        {
                if ( !( obj = get_eq_char( victim, iWear ) ) )
                        continue;

                if ( (IS_SET(obj->extra_flags, ITEM_NOREMOVE))
                    || (IS_SET(obj->extra_flags, ITEM_NODROP)))
                {
                        unequip_char( victim, obj );
                        obj_from_char( obj );
                        obj_to_room( obj, victim->in_room );
                        act( "You toss $p to the ground.",  victim, obj, NULL, TO_CHAR );
                        act( "$n tosses $p to the ground.", victim, obj, NULL, TO_ROOM );
                        yesno = 1;
                }
        }

        for (obj = victim->carrying; obj; obj = obj_next)
        {
                obj_next = obj->next_content;
                if ( (IS_SET(obj->extra_flags, ITEM_NOREMOVE))
                    || (IS_SET(obj->extra_flags, ITEM_NODROP)))
                {
                        obj_from_char(obj);
                        obj_to_room(obj, victim->in_room);
                        act("$p drops to the ground.", victim, obj, NULL, TO_CHAR);
                        act("$n drops $p.", victim, obj, NULL, TO_ROOM);
                }
        }

        if ( is_affected( victim, gsn_curse ) )
        {
                affect_strip( victim, gsn_curse );
                send_to_char( "Your curse has lifted.\n\r", victim );
                yesno = 1;
        }

        if ( ch != victim && yesno )
                send_to_char( "Ok.\n\r", ch );
}


void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;
        char    buf     [MAX_STRING_LENGTH];

        if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
        {
                sprintf( buf, "%s is already affected by that spell.\n\r", victim->name);
                send_to_char( buf, ch );
                return;
        }

        af.type      = sn;
        af.duration  = number_fuzzy( level / 8 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SANCTUARY;
        affect_to_char( victim, &af );

        send_to_char( "You are surrounded by a white aura.\n\r", victim );
        act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
}


void spell_sense_traps( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_DETECT_TRAPS ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_TRAPS;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );

        send_to_char( "You become more astute in your observations.\n\r", victim );
        return;
}


void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = 8 + level;
        af.location  = APPLY_AC;
        af.modifier  = -20;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You are surrounded by a force shield.\n\r", victim );
        act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_shocking_grasp (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = number_range(10, 20) + 2 * level;

        if (is_affected(victim, gsn_resist_lightning))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_SLEEP )
            || level < victim->level
            || saves_spell( level, victim ) )
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        af.type      = sn;
        af.duration  = 4 + number_range(level/6, level / 4);
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SLEEP;
        affect_to_char( victim, &af );

        if ( IS_AWAKE( victim ) )
        {
                send_to_char( "You feel very sleepy... zzzZZZzzz\n\r", victim );

                if ( victim->position == POS_FIGHTING )
                        stop_fighting( victim, TRUE );

                do_sleep( victim, "" );
        }
}


void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( ch, sn ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_AC;
        af.modifier  = -40;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "Your skin turns to stone.\n\r", victim );
        act( "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_summon (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim;

        if (!(victim = get_char_world(ch, target_name))
            || victim == ch
            || !victim->in_room
            || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
            || IS_SET(ch->in_room->room_flags, ROOM_SAFE)
            || IS_SET(victim->in_room->area->area_flags, AREA_FLAG_SAFE)
            || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE)
            || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
            || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
            || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
            || IS_AFFECTED(victim, AFF_CURSE)
            || victim->level >= level + 3
            || victim->fighting
            || victim->in_room->area != ch->in_room->area
            || victim->rider
            || mob_is_quest_target(victim)
            || (victim->in_room->area->low_level == -4
                && victim->in_room->area->high_level == -4)
            || (IS_NPC(victim)
                && (IS_SET(victim->act, ACT_MOUNTABLE)
                    || IS_SET(victim->act, ACT_NO_SUMMON)
                    || saves_spell(level, victim))))
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        if (tournament_action_illegal(ch, TOURNAMENT_OPTION_NOSUMMON))
        {
                send_to_char("You may not summon during this tournament.\n\r", ch);
                return;
        }

        act( "$c disappears suddenly.", victim, NULL, NULL, TO_ROOM );
        act( "$c has summoned you!", ch, NULL, victim, TO_VICT );
        char_from_room( victim );
        char_to_room( victim, ch->in_room );
        act( "$c arrives suddenly.", victim, NULL, NULL, TO_ROOM );
        arena_commentary("$N is summoned by $n.", ch, victim);

        if (victim->mount)
        {
                act ("$c disappears suddenly.", victim->mount, NULL, NULL, TO_ROOM);
                char_from_room (victim->mount);
                char_to_room (victim->mount, victim->in_room);
        }

        do_look(victim, "auto");
        return;
}


void spell_summon_familiar( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim= (CHAR_DATA *) vo;
        char    buf     [MAX_STRING_LENGTH];

        if ( !IS_OUTSIDE( ch ) )
        {
                send_to_char( "You must be out doors.\n\r", ch);
                return;
        }

        if (ch->level < 25)
                victim=(create_mobile( get_mob_index( MOB_VNUM_PONY) ) );

        else if (ch->level < 50 )
                victim = (create_mobile( get_mob_index( MOB_VNUM_HORSE) ) );

        else if( ch->level < 75 )
                victim = (create_mobile( get_mob_index(MOB_VNUM_PEGASUS)) );

        else
                victim = (create_mobile( get_mob_index( MOB_VNUM_SILVER) ) );

        sprintf( buf, "You raise your hands and the form of %s appears before you.\n\r",
                victim->short_descr);
        send_to_char( buf, ch );
        act("The form of $N slowly solidifies before $n",ch,NULL,victim,TO_ROOM);

        char_to_room( victim, ch->in_room);
        add_follower( victim, ch );
        SET_BIT(victim->act, ACT_NO_EXPERIENCE);
}


void spell_summon_demon( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *mob;
        char buf [MAX_STRING_LENGTH];
        int count;

        if (ch->class == CLASS_SHAPE_SHIFTER && ch->form != FORM_DEMON)
        {
                send_to_char("What do you think you are, a demon?\n\r", ch);
                return;
        }

        /*  Check for exisiting demons a'la 'summon avatar'.  Gezhp  */
        count = 0;

        for (mob = char_list; mob; mob = mob->next)
        {
                if (mob->deleted)
                        continue;

                if (IS_NPC (mob) && mob->master == ch
                    && (mob->pIndexData->vnum == MOB_VNUM_LESSER
                        || mob->pIndexData->vnum == MOB_VNUM_DEMON))
                        count++;
        }

        if (count > 5)
        {
                send_to_char ("You draw a pentagram and invoke your spell, but no demon appears.\n\r", ch);
                return;
        }

        if (ch->class == CLASS_SHAPE_SHIFTER)
        {
                mob = create_mobile(get_mob_index(MOB_VNUM_LESSER));
                mob->level = ch->level - 10;

                mob->max_hit = mob->level * 10
                        + number_range(mob->level * mob->level/4, mob->level * mob->level);
                mob->hit = mob->max_hit;
        }
        else
        {
                mob = create_mobile(get_mob_index(MOB_VNUM_DEMON));
                mob->level = ch->level - 10;
                mob->max_hit = mob->level * 8 + number_range(mob->level * mob->level/4, mob->level * mob->level);
                mob->hit = mob->max_hit;
        }

        sprintf( buf, "You draw a pentagram on the ground and the form of %s rises "
                "from its centre!\n\r", mob->short_descr);
        send_to_char( buf, ch );
        act("The form of $N slowly rises from the ground before $n!", ch, NULL, mob, TO_ROOM);

        char_to_room(mob, ch->in_room);
        add_follower(mob, ch );

        SET_BIT(mob->act, ACT_NO_EXPERIENCE);
}


void spell_summon_avatar( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        CHAR_DATA *target = (CHAR_DATA *) vo;
        CHAR_DATA *rch;
        int count;
        char buf [MAX_STRING_LENGTH];

        if (!ch->fighting)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        /*
         * Modified by Gezhp to limit number of avatars summoned.
         * Avatars are flagged ACT_DIE_IF_MASTER_GONE, so disappear
         * when master is no longer in room.
         */

        count = 0;

        for (rch = char_list; rch; rch = rch->next)
        {
                if (rch->deleted)
                        continue;

                if (IS_NPC (rch) && rch->master == ch
                    && rch->pIndexData->vnum == MOB_VNUM_WOLF)
                        count++;
        }

        if (count > 5)
        {
                send_to_char ("You call to your brethren, but none answer.\n\r", ch);
                return;
        }

        target = ch->fighting;
        victim = (create_mobile( get_mob_index( MOB_VNUM_WOLF) ) );
        char_to_room( victim, ch->in_room);

        sprintf( buf, "You call to your brethren to aid you in battle, a %s enters the fray.\n\r",
                victim->name);
        send_to_char( buf, ch );

        act("The form of a $d emerges from the shadows to aid $n",
            ch, NULL, victim->name, TO_ROOM);

        add_follower( victim, ch );
        stop_fighting( target, FALSE );
        set_fighting( target, victim );
        do_rescue( victim, ch->name );

        SET_BIT(victim->act, ACT_NO_EXPERIENCE);
}


void spell_teleport (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        ROOM_INDEX_DATA *pRoomIndex;

        if (!victim->in_room
            || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
            || IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
            || (!IS_NPC(ch) && victim->fighting)
            || (victim != ch
                && (saves_spell(level, victim) || saves_spell(level, victim))))
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        for ( ; ; )
        {
                pRoomIndex = get_room_index(number_range(0, 65535));
                if (pRoomIndex
                    && !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
                    && !IS_SET(pRoomIndex->room_flags, ROOM_PLAYER_KILLER)
                    && !IS_SET(pRoomIndex->room_flags, ROOM_NO_RECALL)
                    && !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY))
                        break;
        }

        act( "$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM );
        arena_commentary("$n teleports away.", ch, NULL);

        stop_fighting(victim, TRUE);
        char_from_room( victim );

        if (is_entered_in_tournament(ch)
            && tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_still_alive_in_tournament(ch))
        {
                disqualify_tournament_entrant(ch, "left the Arena");
                check_for_tournament_winner();
        }

        char_to_room( victim, pRoomIndex );
        act( "$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM );

        if (is_affected(ch, gsn_mount) )
        {
                CHAR_DATA *mount;
                mount = ch->mount;
                char_from_room( mount );
                char_to_room( mount, pRoomIndex );
                act_move( "$n fades into existence..", mount, NULL, NULL, TO_ROOM );
        }

        do_look( victim, "auto" );
        return;
}


void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        char       buf1    [ MAX_STRING_LENGTH ];
        char       buf2    [ MAX_STRING_LENGTH ];
        char       speaker [ MAX_INPUT_LENGTH  ];

        target_name = one_argument( target_name, speaker );

        sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
        sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
        buf1[0] = UPPER( buf1[0] );

        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        {
                if ( !is_name( speaker, vch->name ) )
                        send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
        }

        return;
}


void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        {
                send_to_char( "You fail to weaken them.\n\r", ch );
                return;
        }

        af.type      = sn;
        af.duration  = level / 2;
        af.location  = APPLY_STR;
        af.modifier  = -4;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "You drain their strength.\n\r", ch );

        send_to_char( "You feel weaker.\n\r", victim );
}


void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
        if (!ch->fighting)
                do_recall( (CHAR_DATA *) vo, "" );
}


/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA  *obj_lose;
        OBJ_DATA  *obj_next;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;
        int hpch;
        bool resist = FALSE;

        if (is_affected(victim, gsn_resist_acid))
                resist = TRUE;

        if (number_percent() < 2 * level
            && !saves_spell(level, victim)
            && !IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
            && !is_affected(victim,gsn_dragon_shield))
        {

                for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
                {
                        int iWear;
                        obj_next = obj_lose->next_content;

                        if ( obj_lose->deleted )
                                continue;

                        if ( number_bits( 2 ) != 0 )
                                continue;

                        if (resist && number_bits(1))
                                continue;

                        switch ( obj_lose->item_type )
                        {
                            case ITEM_ARMOR:
                                if ( obj_lose->value[0] > 0 )
                                {
                                        act( "{Y$p is pitted and etched!{x",
                                            victim, obj_lose, NULL, TO_CHAR );
                                        if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
                                                victim->armor -= apply_ac( obj_lose, iWear );
                                        obj_lose->value[0] -= 1;
                                        obj_lose->cost      = 0;

                                        if ( iWear != WEAR_NONE )
                                                victim->armor += apply_ac( obj_lose, iWear );
                                }
                                break;

                            case ITEM_CONTAINER:
                                act( "{Y$p fumes and dissolves!{x",
                                    victim, obj_lose, NULL, TO_CHAR );
                                extract_obj( obj_lose );
                                break;
                        }
                }
        }

        hpch = UMAX( 10, ch->hit );
        dam  = number_range( hpch / 8 + 1, hpch / 4 );

        if (resist || saves_spell(level, victim))
                dam /= 2;

        if (is_affected(victim, gsn_dragon_shield))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA  *obj_lose;
        OBJ_DATA  *obj_next;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;
        int hpch;
        bool resist = FALSE;

        if (is_affected(victim, gsn_resist_heat))
                resist = TRUE;

        if (number_percent() < 2 * level
            && !saves_spell(level, victim)
            && !IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
            && !is_affected(victim,gsn_dragon_shield))
        {
                for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
                {
                        char *msg;
                        obj_next = obj_lose->next_content;

                        if ( obj_lose->deleted )
                                continue;

                        if ( number_bits( 2 ) != 0 )
                                continue;

                        if (resist && number_bits(1))
                                continue;

                        switch ( obj_lose->item_type )
                        {
                            default:             continue;
                            case ITEM_CONTAINER: msg = "{Y$p ignites and burns!{x";   break;
                            case ITEM_POTION:    msg = "{Y$p bubbles and boils!{x";   break;
                            case ITEM_SCROLL:    msg = "{Y$p crackles and burns!{x";  break;
                            case ITEM_STAFF:     msg = "{Y$p smokes and chars!{x";    break;
                            case ITEM_WAND:      msg = "{Y$p sparks and sputters!{x"; break;
                            case ITEM_FOOD:      msg = "{Y$p blackens and crisps!{x"; break;
                            case ITEM_PILL:      msg = "{Y$p melts and drips!{x";     break;
                        }

                        act( msg, victim, obj_lose, NULL, TO_CHAR );
                        extract_obj( obj_lose );
                }
        }

        hpch = UMAX( 10, ch->hit );
        dam  = number_range( hpch / 8 + 1, hpch / 4 );

        if (resist || saves_spell(level, victim))
                dam /= 2;

        if (is_affected(victim, gsn_dragon_shield))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA  *obj_lose;
        OBJ_DATA  *obj_next;
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;
        int hpch;
        bool resist = FALSE;

        if (is_affected(victim, gsn_resist_cold))
                resist = TRUE;

        if (number_percent() < 2 * level
            && !saves_spell( level, victim )
            && !IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)
            && !is_affected(victim,gsn_dragon_shield))
        {
                for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
                {
                        char *msg;
                        obj_next = obj_lose->next_content;

                        if ( obj_lose->deleted )
                                continue;

                        if ( number_bits( 2 ) != 0 )
                                continue;

                        if (resist && number_bits(1))
                                continue;

                        switch ( obj_lose->item_type )
                        {
                            default:
                                continue;

                            case ITEM_CONTAINER:
                            case ITEM_DRINK_CON:
                            case ITEM_POTION:
                                msg = "{Y$p freezes and shatters!{x";
                                break;
                        }

                        act( msg, victim, obj_lose, NULL, TO_CHAR );
                        extract_obj( obj_lose );
                }
        }

        hpch = UMAX( 10, ch->hit );
        dam  = number_range( hpch / 8 + 1, hpch / 4 );

        if (resist || saves_spell(level, victim))
                dam /= 2;

        if (is_affected(victim, gsn_dragon_shield))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;
        int hpch;

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if ( vch->deleted )
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group (vch, ch))
                        continue;

                if ( IS_NPC( ch ) ? !IS_NPC( vch ) : ( IS_NPC( vch ) || ch->fighting == vch ))
                {
                        hpch = UMAX( 10, ch->hit );
                        dam  = number_range( hpch / 8 + 1, hpch / 4 );

                        if ( saves_spell( level, vch ) )
                                dam /= 2;

                        if (is_affected(vch,gsn_dragon_shield))
                                dam /= 2;
                        else
                                spell_poison( gsn_poison, level, ch, vch );

                        damage( ch, vch, dam, sn, FALSE );
                }
        }

        return;
}


void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int hpch = UMAX( 10, ch->hit );
        int dam = number_range( hpch / 8 + 1, hpch / 4 );

        if (is_affected(victim, gsn_resist_lightning)
            || saves_spell(level, victim))
                dam /= 2;

        if (is_affected(victim, gsn_dragon_shield))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


/*
 * Code for Psionicist spells/skills by Thelonius
 */

void spell_adrenaline_control ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type  = sn;
        af.duration      = level;
        af.location      = APPLY_DEX;
        af.modifier      = 2;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location     = APPLY_CON;
        affect_to_char( victim, &af );

        send_to_char( "You have given yourself an adrenaline rush!\n\r", ch );
        act( "$n has given $mself an adrenaline rush!", ch, NULL, NULL, TO_ROOM );
}


void spell_agitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        static const int dam_each [ ] =
        {
                0,
                0,  0,  3,  6,  9,       12, 15, 18, 21, 24,
                24, 24, 25, 25, 26,      26, 26, 27, 27, 27,
                28, 28, 28, 29, 29,      29, 30, 30, 30, 31,
                31, 31, 32, 32, 32,      33, 33, 33, 34, 34,
                34, 35, 35, 35, 36,      36, 36, 37, 37, 37
        };

        level = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
        level = UMAX( 0, level );
        dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_aura_sight ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        char  *msg;
        int ap = victim->alignment;

        if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
        else if ( ap >  350 ) msg = "$N is of excellent moral character.";
        else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
        else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
        else if ( ap > -350 ) msg = "$N lies to $S friends.";
        else if ( ap > -700 ) msg = "Don't bring $N home to meet your family.";
        else msg = "Uh, check please!";

        act( msg, ch, NULL, victim, TO_CHAR );
}


void spell_awe ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if ( victim->fighting == ch && !saves_spell( level, victim ) )
        {
                stop_fighting ( victim, TRUE);
                act( "$N is in AWE of you!", ch, NULL, victim, TO_CHAR    );
                act( "You are in AWE of $n!",ch, NULL, victim, TO_VICT    );
                act( "$N is in AWE of $n!",  ch, NULL, victim, TO_NOTVICT );
        }
        return;
}


void spell_ballistic_attack ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        static const int dam_each [ ] =
        {
                0,
                3,  4,  4,  5,  6,       6,  6,  7,  7,  7,
                7,  7,  8,  8,  8,       9,  9,  9, 10, 10,
                10, 11, 11, 11, 12,      12, 12, 13, 13, 13,
                14, 14, 14, 15, 15,      15, 16, 16, 16, 17,
                17, 17, 18, 18, 18,      19, 19, 19, 20, 20
        };

        level = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
        level = UMAX( 0, level );
        dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );

        if ( saves_spell( level, victim ) )
                dam /= 2;

        act( "You chuckle as a stone strikes $N.", ch, NULL, victim, TO_CHAR );
        damage( ch, victim, dam, sn, FALSE);
        return;
}


void spell_biofeedback ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
        {
                send_to_char( "You are already protected.\n\r", victim );
                return;
        }

        af.type      = sn;
        af.duration  = number_fuzzy( level / 8 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SANCTUARY;
        affect_to_char( victim, &af );

        send_to_char( "A white aura surrounds you.\n\r", victim );
        act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_cell_adjustment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if ( is_affected( victim, gsn_poison ) )
        {
                affect_strip( victim, gsn_poison );
                send_to_char( "A warm feeling runs through your body.\n\r", victim );
                act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );
        }

        if ( is_affected( victim, gsn_curse  ) )
        {
                affect_strip( victim, gsn_curse  );
                send_to_char( "Your curse is lifted.\n\r", victim );
        }
}


void spell_combat_mind ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
        {
                if ( victim == ch )
                        send_to_char( "You already understand battle tactics.\n\r", victim );
                else
                        act( "$N already understands battle tactics.",
                            ch, NULL, victim, TO_CHAR );
                return;
        }

        af.type = sn;
        af.duration = level + 3;
        af.location = APPLY_HITROLL;
        af.modifier = level / 6;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location = APPLY_AC;
        af.modifier = - level/2 - 5;
        affect_to_char( victim, &af );

        if ( victim != ch )
                send_to_char( "You heighten their combat skills.\n\r", ch );

        send_to_char( "You gain a keen understanding of battle tactics.\n\r", victim );
        return;
}


void spell_recharge_mana ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        victim->mana = victim->max_mana - victim->aggro_dam;
        update_pos( victim );

        if ( ch != victim )
                send_to_char( "Ok.\n\r", ch );
        send_to_char( "Ahhhhhh... your mana surges as it flows through your body!\n\r", victim );
        return;
}


void spell_vitalize ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;

        if (!IS_NPC(ch))
                return;

        victim->hit = victim->max_hit - victim->aggro_dam;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;

        send_to_char( "You feel a massive rush as raw magical energy courses through your body.\n\r", victim);
        return;
}


void spell_complete_healing (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;

        if (victim == ch->fighting)
                return;

        if (victim->hit > victim->max_hit)
                return;

        victim->hit = victim->max_hit - victim->aggro_dam;
        update_pos(victim);

        if (ch != victim)
                send_to_char("Ok.\n\r", ch);

        send_to_char( "Ahhhhhh... you are completely healed!\n\r", victim );
        arena_commentary("$n is completely healed.", ch, ch);

        ch->edrain = 0;
        ch->backstab = 0;
}


void spell_control_flames ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        static const int dam_each [ ] =
        {
                0,
                0,  0,  0,  2,  4,       8,  12, 16, 20, 24,
                28, 32, 35, 38, 40,      42, 44, 45, 45, 45,
                46, 46, 46, 47, 47,      47, 48, 48, 48, 49,
                49, 49, 50, 50, 50,      51, 51, 51, 52, 52,
                52, 53, 53, 53, 54,      54, 54, 55, 55, 55
        };

        if ( !get_eq_char( ch, WEAR_LIGHT ) )
        {
                send_to_char( "You must be carrying a light source.\n\r", ch );
                return;
        }

        level = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
        level = UMAX( 0, level );
        dam = number_range( dam_each[level] / 2, dam_each[level] * 2 );

        if (is_affected(victim, gsn_resist_heat) || saves_spell(level, victim))
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_create_sound ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        char buf1    [ MAX_STRING_LENGTH ];
        char buf2    [ MAX_STRING_LENGTH ];
        char speaker [ MAX_INPUT_LENGTH  ];

        target_name = one_argument( target_name, speaker );

        sprintf( buf1, "%s says '%s'.\n\r", speaker, target_name );
        sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
        buf1[0] = UPPER( buf1[0] );

        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        {
                if ( !is_name( speaker, vch->name ) )
                        send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
        }
        return;
}


void spell_death_field ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;
        int hpch;

        if ( !IS_EVIL( ch ) )
        {
                send_to_char( "You are not evil enough to do that!\n\r", ch);
                return;
        }

        send_to_char( "A black fog emanates from you!\n\r", ch );
        act ( "A black fog emanates from $n!", ch, NULL, ch, TO_ROOM );

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if ( !IS_NPC( ch ) ? IS_NPC( vch ) : IS_NPC( vch ) )
                {
                        hpch = URANGE( 10, ch->hit, 999 );
                        if ( !saves_spell( level, vch )
                            && (   level <= vch->level + 5
                                && level >= vch->level - 5 ) )
                        {
                                dam = 10; /* Enough to compensate for sanct. and prot. */
                                vch->hit = 1;
                                update_pos( vch );
                                send_to_char( "The black fog envelops you!\n\r", vch );
                                act( "The black fog envelops $N!", ch, NULL, vch, TO_ROOM );
                        }
                        else
                                dam = number_range( hpch / 16 + 1, hpch / 8 );

                        damage( ch, vch, dam, sn, FALSE );
                }
        }
}


void spell_decay ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        /* Who, what, where? */
        return;
}


void spell_detonate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        static const int dam_each [ ] =
        {
                0,
                0,   0,   0,   0,   0,         5,  10,  15,  20,  25,
                30,  35,  40,  45,  50,       60,  65,  65,  70,  75,
                80,  85,  90,  95, 100,      102, 104, 106, 108, 110,
                113, 114, 116, 118, 120,     122, 124, 126, 128, 130,
                134, 138, 142, 146, 150,     160, 170, 180, 190, 200
        };

        level = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
        level = UMAX( 0, level );

        dam = dice(level,6);

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
        return;
}


void spell_deter (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (IS_AFFECTED(ch, AFF_DETER))
                return;

        af.type      = sn;
        af.duration  = number_fuzzy(level / 20);
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETER;
        affect_to_char( victim, &af );

        if (ch != victim)
                send_to_char( "Ok.\n\r", ch );

        act( "$n is protected from hostile creatures.", ch, NULL, victim, TO_ROOM );
        act( "Hostile creatures no longer attack you.", ch, NULL, victim, TO_CHAR );
}


void spell_disintegrate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        OBJ_DATA  *obj_lose;
        OBJ_DATA  *obj_next;

        if ( !IS_NPC( victim )
            && (victim->form == FORM_PHOENIX
                || IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER)))
                return;

        if ( !saves_spell( level, victim ) )
        {
                for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
                {
                        obj_next = obj_lose->next_content;

                        if ( obj_lose->deleted )
                                continue;

                        if ( number_bits( 2 ) != 0 )
                                continue;

                        act( "$p disintegrates!",      victim, obj_lose, NULL, TO_CHAR );
                        act( "$n's $p disintegrates!", victim, obj_lose, NULL, TO_ROOM );
                        extract_obj( obj_lose ) ;
                }
                damage (ch, victim, UMIN(level, victim->hit - 1), sn, FALSE);
        }
        else
        {
                damage (ch, victim, 0, sn, FALSE);
                return;
        }

        if ( (IS_NPC(ch) || IS_NPC(victim)) && !saves_spell( level, victim ) )
        {
                act( "You have DISINTEGRATED $N!",         ch, NULL, victim, TO_CHAR );
                act( "You have been DISINTEGRATED by $n!", ch, NULL, victim, TO_VICT );
                act( "$n's spell DISINTEGRATES $N!",       ch, NULL, victim, TO_NOTVICT );

                if (!IS_NPC(victim))
                {
                        char buf [MAX_STRING_LENGTH];
                        sprintf (buf, "%s has been DISINTEGRATED by %s at %s!",
                                 victim->name,
                                 IS_NPC(ch) ? ch->short_descr : ch->name,
                                 victim->in_room->name);
                        do_info (victim, buf);
                }

                stop_fighting(victim, TRUE);

                if (IS_NPC(victim))
                        mprog_death_trigger(victim);

                if (victim->mount)
                        strip_mount(victim);

                if (victim->rider)
                        strip_mount(victim->rider);

                if (IS_NPC(victim))
                        extract_char(victim, TRUE);
                else
                        raw_kill(ch, victim, FALSE);
        }
}



void spell_displacement ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type = sn;
        af.duration = level - 4;
        af.location = APPLY_AC;
        af.modifier = 4 - level;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "Your form shimmers, and you appear displaced.\n\r", victim );
        act( "$N shimmers and appears in a different location.",
            ch, NULL, victim, TO_NOTVICT );
        return;
}


void spell_domination ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        return;

        if ( victim == ch )
        {
                send_to_char( "Dominate yourself?  You're weird.\n\r", ch );
                return;
        }

        if (!IS_NPC(victim))
        {
                send_to_char ("You cannot dominate other players.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_CHARM)
            || IS_SET(victim->act, ACT_NOCHARM)
            || IS_AFFECTED(ch, AFF_CHARM)
            || level < victim->level
            || ch->mount == victim
            || victim->rider
            || IS_SET(victim->act, ACT_BANKER)
            || IS_SET(victim->act, ACT_CLAN_GUARD)
            || !mob_interacts_players(victim)
            || mob_is_quest_target(victim)
            || saves_spell(level, victim))
        {
                send_to_char("Your domination failed.\n\r", ch);
                return;
        }

        if (victim->mount)
                return;

        if ( victim->master )
                stop_follower( victim );

        add_follower( victim, ch );

        af.type = sn;
        af.duration = number_fuzzy( level / 4 );
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( victim, &af );

        act( "Your will dominates $N!", ch, NULL, victim, TO_CHAR );
        act( "Your will is dominated by $n!", ch, NULL, victim, TO_VICT );
        return;
}


void spell_ectoplasmic_form ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
                return;

        af.type = sn;
        af.duration = number_fuzzy( level / 4 );
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_PASS_DOOR;
        affect_to_char( victim, &af );

        send_to_char( "You turn translucent.\n\r", victim );
        act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_ego_whip ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
                return;

        af.type = sn;
        af.duration = level;
        af.location = APPLY_HITROLL;
        af.modifier = -2;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location = APPLY_SAVING_SPELL;
        af.modifier = 2;
        affect_to_char( victim, &af );

        af.location = APPLY_AC;
        af.modifier = level / 2;
        affect_to_char( victim, &af );

        act( "You ridicule $N about $S childhood.", ch, NULL, victim, TO_CHAR    );
        send_to_char( "Your ego takes a beating.\n\r", victim );
        act( "$N's ego is crushed by $n!", ch, NULL, victim, TO_NOTVICT );

        return;
}


void spell_energy_containment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type = sn;
        af.duration = level / 2 + 7;
        af.modifier = -level / 4;
        af.location  = APPLY_SAVING_SPELL;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You can now absorb some forms of energy.\n\r", ch );
        return;
}


void spell_enhance_armor (int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA    *obj = (OBJ_DATA *) vo;
        AFFECT_DATA *paf;

        if ( obj->item_type != ITEM_ARMOR
            || IS_OBJ_STAT( obj, ITEM_MAGIC )
            || obj->affected )
        {
                send_to_char( "That item cannot be enhanced.\n\r", ch );
                return;
        }

        if ( !affect_free )
        {
                paf         = alloc_perm( sizeof( *paf ) );
        }
        else
        {
                paf         = affect_free;
                affect_free = affect_free->next;
        }

        paf->type = sn;
        paf->duration = -1;
        paf->location = APPLY_AC;
        paf->bitvector = 0;
        paf->next = obj->affected;
        paf->modifier   = -level / 8;

        obj->affected = paf;

        if ( IS_GOOD( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
                act( "$p glows blue.",   ch, obj, NULL, TO_CHAR );
        }
        else if ( IS_EVIL( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p glows red.",    ch, obj, NULL, TO_CHAR );
        }
        else
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p glows yellow.", ch, obj, NULL, TO_CHAR );
        }

        send_to_char( "Ok.\n\r", ch );
        return;
}


void spell_enhanced_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, gsn_giant_strength)
            || is_affected(victim, gsn_enhanced_strength))
                return;

        af.type = sn;
        af.duration = level;
        af.location = APPLY_STR;
        af.modifier = 1 + ( level >= 15 ) + ( level >= 25 );
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "Your strength is enhanced!\n\r", victim );
        return;
}


void spell_flesh_armor ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type = sn;
        af.duration = level;
        af.location = APPLY_AC;
        af.modifier = -40;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "Your flesh turns to steel.\n\r", victim );
        act( "$N's flesh turns to steel.", ch, NULL, victim, TO_NOTVICT);
        return;
}


void spell_inertial_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;
        AFFECT_DATA af;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_PROTECT ) )
                        continue;

                act( "An inertial barrier forms around $n.", gch, NULL, NULL, TO_ROOM );
                send_to_char( "An inertial barrier forms around you.\n\r", gch );

                af.type      = sn;
                af.duration  = 24;
                af.modifier  = 0;
                af.location  = APPLY_NONE;
                af.bitvector = AFF_PROTECT;
                affect_to_char( gch, &af );
        }
        return;
}


void spell_inflict_pain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        damage( ch, (CHAR_DATA *) vo, dice( 2, 10 ) + level / 2, sn, FALSE );
        return;
}


void spell_intellect_fortress ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;
        AFFECT_DATA af;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ) || is_affected( gch, sn ) )
                        continue;

                send_to_char( "A virtual fortress forms around you.\n\r", gch );
                act( "A virtual fortress forms around $N.", gch, NULL, gch, TO_ROOM );

                af.type      = sn;
                af.duration  = 24;
                af.location  = APPLY_AC;
                af.modifier  = -40;
                af.bitvector = 0;
                affect_to_char( gch, &af );
        }
        return;
}


void spell_lend_health ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int hpch;

        if ( ch == victim )
        {
                send_to_char( "Lend health to yourself?  Weird idea.\n\r", ch );
                return;
        }

        hpch = UMIN( 100, victim->max_hit - victim->hit );

        if ( hpch < 1 )
        {
                act( "Nice thought, but $N doesn't need healing.", ch, NULL, victim, TO_CHAR );
                return;
        }

        if ( ch->hit-hpch < 100 )
        {
                send_to_char( "You aren't healthy enough yourself!\n\r", ch );
                return;
        }

        victim->hit += hpch;
        ch->hit -= hpch;
        update_pos( victim );
        update_pos( ch );

        act( "You lend some of your health to $N.", ch, NULL, victim, TO_CHAR );
        act( "$n lends you some of $s health.",     ch, NULL, victim, TO_VICT );
}


void spell_levitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_FLYING ) )
        {
                send_to_char( "You are already floating in mid-air!\n\r", victim );
                return;
        }
        af.type = sn;
        af.duration = level + 3;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_FLYING;
        affect_to_char( victim, &af );

        send_to_char( "You rise up into the air.\n\r", victim );
        act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_mental_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type = sn;
        af.duration = 24;
        af.location = APPLY_AC;
        af.modifier = -20;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You erect a mental barrier around yourself.\n\r", victim );
}


void spell_mind_thrust ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        damage( ch, (CHAR_DATA *) vo, dice( 1, 10 ) + level / 2, sn, FALSE );
}


void spell_project_force ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        damage( ch, (CHAR_DATA *) vo, dice( 4, 6 ) + level, sn, FALSE );
}


void spell_psionic_blast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        dam = number_range(20, 30) + dice(level, 4);

        if ( saves_spell( level, victim ) )
                dam /= 2;

        damage( ch, victim, dam, sn, FALSE );
}


void spell_psychic_crush ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        damage( ch, (CHAR_DATA *) vo, dice( 3, 5 ) + level, sn, FALSE );
}


void spell_psychic_drain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
                return;

        af.type = sn;
        af.duration = level / 2;
        af.location = APPLY_STR;
        af.modifier = -1 - ( level >= 10 ) - ( level >= 20 ) - ( level >= 30 );
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You feel drained.\n\r", victim );
        act( "$n appears drained of strength.", victim, NULL, NULL, TO_ROOM );
        return;
}


void spell_psychic_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int heal;

        heal = dice( 3, 6 ) + 2 * level / 3 ;

        if( victim->hit > victim->max_hit )
                return;

        victim->hit = UMIN( victim->hit + heal, victim->max_hit - victim->aggro_dam);
        update_pos( victim );

        send_to_char( "You feel better!\n\r", victim );
}


void spell_share_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( victim == ch )
        {
                send_to_char( "You can't share strength with yourself.\n\r", ch );
                return;
        }

        if ( is_affected( victim, sn ) )
        {
                act( "$N already shares someone's strength.", ch, NULL, victim, TO_CHAR );
                return;
        }

        if ( get_curr_str( ch ) <= 5 )
        {
                send_to_char( "You are too weak to share your strength.\n\r", ch );
                return;
        }

        af.type = sn;
        af.duration = level;
        af.location = APPLY_STR;
        af.modifier =  1 + ( level >= 20 ) + ( level >= 30 );
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.modifier = -1 - ( level >= 20 ) - ( level >= 30 );
        affect_to_char( ch, &af );

        act( "You share your strength with $N.", ch, NULL, victim, TO_CHAR );
        act( "$n shares $s strength with you.",  ch, NULL, victim, TO_VICT );
}


void spell_thought_shield ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type = sn;
        af.duration = level;
        af.location = APPLY_AC;
        af.modifier = -20;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You have created a shield around yourself.\n\r", ch );
}


void spell_ultrablast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;
        int hpch;

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
                {
                        hpch = UMAX( 10, ch->hit );
                        dam  = number_range( hpch / 8+1, hpch / 4 );

                        if ( saves_spell( level, vch ) )
                                dam /= 1.5;

                        damage( ch, vch, dam, sn, FALSE );
                }
        }
}


void spell_wrath_of_god (int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;
        int hpch;

        act("You call upon the wrath of $D!", ch, NULL, NULL, TO_CHAR);
        act( "$n calls upon $s god to bring death to $s enemies!", ch, NULL, NULL, TO_ROOM );

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (!mob_interacts_players(vch))
                        continue;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if ( IS_NPC ( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
                {
                        hpch = UMAX( 10, ch->hit);
                        dam = number_range( hpch / 8+1, hpch / 4 );

                        if ( saves_spell ( level, vch ) )
                                dam /= 1.5;

                        damage ( ch, vch, dam, sn, FALSE );
                }
        }
}


void spell_spiritwrack ( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        if ( saves_spell( level, victim ) )
        {
                send_to_char("Your victim resists your spirits!\n\r", ch);
                return;
        }

        af.type      = sn;
        af.duration  = level/5;
        af.location  = APPLY_AC;
        af.modifier  = level*2/3;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location  = APPLY_HITROLL;
        af.modifier  = - (level/2 - 5);
        affect_to_char( victim, &af );

        af.location  = APPLY_DAMROLL;
        af.modifier  = - (level/3 - 5);
        affect_to_char( victim, &af );

        send_to_char( "You summon a horde of spirits to torment your victim.\n\r", ch );
        send_to_char( "You are disoriented by the spiritual torment inflicted on you.\n\r", victim );

        return;
}


void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = level/4;
        af.location  = APPLY_WIS;
        af.modifier  =-4;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location  = APPLY_INT;
        af.modifier  = - 4;
        affect_to_char( victim, &af );

        af.location  = APPLY_MANA;
        af.modifier  = - level;
        affect_to_char( victim, &af );

        send_to_char( "Your victim turns into a simpleton..\n\r", ch );
        send_to_char( "Huh... What?... Where?... You are dazed!\n\r", victim);
        return;
}


void spell_wither( int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int        dam;

        /* 2-9 level - Shade */
        dam = dice( level, 8 );
        dam += level;

        if ( saves_spell( level, victim ) )
                dam /= 1.5;

        if (spell_attack_number == 1)
        {
                act("You drain the life essence from your victim!", ch, NULL, NULL, TO_CHAR);
                act("$c drains $N's life essence away!", ch, NULL, victim, TO_NOTVICT);
        }

        damage( ch, victim, dam, sn, FALSE );
}


void OLD_spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;
        int sp_slot, i, mana;
        char buf[ MAX_STRING_LENGTH ];

        if (skill_table[sn].spell_fun == spell_null )
        {
                send_to_char("That is not a spell.\n\r",ch);
                return;
        }

        /* counting the number of spells contained within */
        for (sp_slot = i = 1; i < 4; i++)
        {
                if (obj->value[i] != -1)
                        sp_slot++;
        }

        if (sp_slot > 3)
        {
                act ("$p cannot contain any more spells.", ch, obj, NULL, TO_CHAR);
                return;
        }

        /* scribe/brew costs 4 times the normal mana required to cast the spell */

        mana = 4 * mana_cost(ch, sn);

        if ( !IS_NPC(ch) && ch->mana < mana )
        {
                send_to_char( "You don't have enough mana.\n\r", ch );
                return;
        }

        if ( number_percent( ) > ch->pcdata->learned[sn] )
        {
                send_to_char( "You lost your concentration.\n\r", ch );
                ch->mana -= mana / 2;
                return;
        }

        /* executing the imprinting process */
        ch->mana -= mana;
        obj->value[sp_slot] = sn;

        /* Making it successively harder to pack more spells into potions or
         scrolls - JH */

        switch( sp_slot )
        {
            default:
                bug( "sp_slot has more than %d spells.", sp_slot );
                return;

            case 1:
                if ( number_percent() > 80 )
                {
                        sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name(obj) );
                        send_to_char( buf, ch );
                        extract_obj( obj );
                        return;
                }
                break;

            case 2:
                if ( number_percent() > 25 )
                {
                        sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name( obj ) );
                        send_to_char( buf, ch );
                        extract_obj( obj );
                        return;
                }
                break;

            case 3:
                if ( number_percent() > 10 )
                {
                        sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name( obj ) );
                        send_to_char( buf, ch );
                        extract_obj( obj );
                        return;
                }
                break;
        }

        /* labeling the item */

        free_string (obj->short_descr);
        sprintf ( buf, "a %s of ", item_type_name(obj) );

        for (i = 1; i <= sp_slot ; i++)
        {
                if (obj->value[i] != -1)
                {
                        strcat (buf, skill_table[obj->value[i]].name);
                        (i != sp_slot ) ? strcat (buf, ", ") : strcat (buf, "") ;
                }
        }

        obj->short_descr = str_dup(buf);

        sprintf( buf, "%s %s", obj->name, item_type_name(obj) );
        free_string( obj->name );
        obj->name = str_dup( buf );

        sprintf(buf, "You have imbued a new spell to the %s.\n\r", item_type_name(obj) );
        send_to_char( buf, ch );

        return;
}


void spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;
        int sp_slot, i, mana;
        char buf [MAX_STRING_LENGTH];

        if (skill_table[sn].spell_fun == spell_null )
        {
                send_to_char("That is not a spell.\n\r", ch);
                return;
        }

        for (sp_slot = i = 1; i < 4; i++)
        {
                if (obj->value[i] != -1)
                        sp_slot++;
        }

        if (sp_slot > 1)
        {
                act ("$p cannot contain any more spells.", ch, obj, NULL, TO_CHAR);
                return;
        }

        mana = 2 * mana_cost(ch, sn);

        if (ch->mana < mana)
        {
                send_to_char( "You don't have enough mana.\n\r", ch );
                return;
        }

        if (number_percent() > ch->pcdata->learned[sn])
        {
                send_to_char( "You lose your concentration.\n\r", ch );
                ch->mana -= mana / 2;
                return;
        }

        ch->mana -= mana;
        obj->value[sp_slot] = sn;

        if (number_percent() > (50 + ch->pcdata->learned[sn] / 2))
        {
                sprintf(buf, "The magic enchantment has failed. The %s vanishes!\n\r", item_type_name(obj));
                send_to_char(buf, ch);
                extract_obj(obj);
                return;
        }

        sprintf(buf, "a %s of %s", item_type_name(obj), skill_table[obj->value[1]].name);
        obj->short_descr = str_dup(buf);

        sprintf( buf, "%s %s", obj->name, item_type_name(obj) );
        obj->name = str_dup(buf);

        sprintf(buf, "You see a %s of %s here.", item_type_name(obj), skill_table[obj->value[1]].name);
        obj->description = str_dup(buf);

        sprintf(buf, "You have imbued the %s with a new spell.\n\r", item_type_name(obj) );
        send_to_char( buf, ch );
}


void spell_prayer( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;
        AFFECT_DATA af;

        for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
        {
                if ( is_affected ( gch, gsn_prayer )  )
                        continue;

                if ( is_same_group ( gch, ch ) )
                {
                        af.duration  = level / 6;
                        af.bitvector = 0;

                        if ( ! is_affected  ( gch, gsn_bless ) )
                        {
                                af.type = gsn_bless;

                                af.location  = APPLY_HITROLL;
                                af.modifier  = level / 6;
                                affect_to_char( gch, &af );

                                af.location  = APPLY_SAVING_SPELL;
                                af.modifier  = 0 - level / 6;
                                affect_to_char( gch, &af );
                        }

                        af.type      = gsn_prayer;

                        af.location  = APPLY_DAMROLL;
                        af.modifier  = level / 6;
                        affect_to_char( gch, &af );

                        af.location  = APPLY_AC;
                        af.modifier  = - level / 2;
                        affect_to_char ( gch, &af );

                        af.location  = APPLY_SAVING_BREATH;
                        af.modifier  = - level / 6;
                        affect_to_char ( gch, &af );

                        send_to_char( "You feel _very_ righteous.\n\r", gch );
                }
        }
}


/*
 * Frenzy - Shade, Erazmus
 */
void spell_frenzy (int sn, int level, CHAR_DATA *caster, void *vo)
{
        AFFECT_DATA affects;
        CHAR_DATA  *target = ( CHAR_DATA *) vo;

        if( IS_NPC( target))
        {
                send_to_char("They are not capable of containing the rage.\n\r", caster);
                return;
        }

        if( is_affected( target, gsn_frenzy))
        {
                if (target == caster)
                        send_to_char("You are already a vessel for your god's rage.\n\r", caster);
                else
                        send_to_char("They are already a vessel for divine rage.\n\r", caster);
                return;
        }

        if( target->fighting)
        {
                send_to_char("You try to get a word in edgewise, but they are busy fighting.\n\r", caster);
                return;
        }

        if( caster->fighting)
        {
                send_to_char("You try in vain to kneel in prayer whilst warding off attacks.\n\r", caster);
                return;
        }

        affects.type      = sn;
        affects.bitvector = 0;
        affects.duration  = level / 2;

        affects.location  = APPLY_HITROLL;
        affects.modifier  = ( level / 5);
        affect_to_char( target, &affects);

        affects.location  = APPLY_DAMROLL;
        affects.modifier  = (( level / 8) + 3);
        affect_to_char( target, &affects);

        if (caster->pcdata->learned[gsn_frenzy] > 94 && !is_affected(target, gsn_haste))
        {
                affects.location  = APPLY_AC;
                affects.modifier  = level + 50;
                affect_to_char( target, &affects);

                affects.type = gsn_haste;
                affects.duration = 24;
                affects.modifier = 0;
                affects.location = APPLY_NONE;
                affect_to_char( target, &affects);
        }

        act("$n kneels deep in prayer.  Fires of divine rage burn brightly in $s eyes.",
            caster, NULL, target, TO_ROOM);

        if (target == caster)
                act("Mind and body, you are now a vessel for $D's rage!", caster, NULL, NULL, TO_CHAR);
        else
        {
                act("Mind and body, you are now a vessel for the rage of $n's god.",
                    caster, NULL, target, TO_VICT);
                act("$D is pleased with this one!", caster, NULL, NULL, TO_CHAR);
        }
}


void spell_mass_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;
        AFFECT_DATA af;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_SANCTUARY ) )
                        continue;

                af.type      = sn;
                af.duration  = number_fuzzy( level / 10 );
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_SANCTUARY;
                affect_to_char( gch, &af );

                send_to_char( "You are surrounded by a white aura.\n\r", gch );
                act( "$n is surrounded by a white aura.", gch, NULL, NULL, TO_ROOM );
        }

        send_to_char( "Ok.\n\r", ch );
}


void spell_mass_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ))
                        continue;

                gch->hit = UMIN( gch->hit + 100, gch->max_hit - gch->aggro_dam);
                update_pos( gch );
                send_to_char( "A warm feeling fills your body.\n\r", gch );
        }

        send_to_char( "Ok.\n\r", ch );
}


void spell_mass_power_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *gch;

        for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
        {
                if ( !is_same_group( gch, ch ))
                        continue;

                gch->hit = UMIN( gch->hit + 300, gch->max_hit - gch->aggro_dam);
                update_pos( gch );
                send_to_char( "A pulse of energy surges through your body.\n\r", gch );
        }

        send_to_char( "Ok.\n\r", ch );
}


void spell_bless_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA    *obj = (OBJ_DATA *) vo;
        AFFECT_DATA *paf;

        if ( obj->item_type != ITEM_WEAPON
            || IS_OBJ_STAT( obj, ITEM_BLESS )
            || obj->affected )
        {
                send_to_char( "That item cannot be blessed.\n\r", ch );
                return;
        }

        if ( !affect_free )
        {
                paf             = alloc_perm( sizeof( *paf ) );
        }
        else
        {
                paf             = affect_free;
                affect_free     = affect_free->next;
        }

        paf->type       = sn;
        paf->duration   = -1;
        paf->location   = APPLY_MANA;
        paf->modifier   = level * 2;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;

        if ( !affect_free )
        {
                paf = alloc_perm( sizeof( *paf ) );
        }
        else
        {
                paf = affect_free;
                affect_free = affect_free->next;
        }

        paf->type       = sn;
        paf->duration   = -1;
        paf->location   = APPLY_SAVING_SPELL;
        paf->modifier   = - level / 5;
        paf->bitvector  = 0;
        paf->next       = obj->affected;
        obj->affected   = paf;
        obj->level      = number_fuzzy( obj->level - 2 );

        if ( IS_GOOD( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL);
                act( "$p pulses blue.",   ch, obj, NULL, TO_CHAR );
        }
        else if ( IS_EVIL( ch ) )
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p pulses red.",    ch, obj, NULL, TO_CHAR );
        }
        else
        {
                SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
                SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
                act( "$p pulses yellow.", ch, obj, NULL, TO_CHAR );
        }

        SET_BIT( obj->extra_flags, ITEM_BLESS );

        send_to_char( "Ok.\n\r", ch );
        return;
}


void spell_bark_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = 2 * level;
        af.location  = APPLY_AC;
        af.modifier  = -30;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "Your skin turns to bark.\n\r", victim );
        act( "$c's skin is transformed into tough tree bark!", victim, NULL, NULL, TO_ROOM );
}


void spell_moonray( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        int        dam;

        if ( !IS_OUTSIDE( ch ) )
        {
                send_to_char( "You must be outdoors.\n\r", ch );
                return;
        }

        if ( weather_info.sky > SKY_CLOUDY )
        {
                send_to_char( "You need clear weather.\n\r", ch );
                return;
        }

        if ( weather_info.sunlight == SUN_LIGHT )
        {
                send_to_char( "You cannot see the moon.\n\r", ch );
                return;
        }

        dam = dice( level, 9 );

        if ( weather_info.sunlight == SUN_DARK )
                dam *= 1.5;

        if ( weather_info.sky == SKY_CLOUDY )
                dam /= 2;


        send_to_char( "A powerful moonbeam strikes your foes!\n\r", ch );
        act( "$n calls a moonbeam to strike $s foes!", ch, NULL, NULL, TO_ROOM );

        for ( vch = char_list; vch; vch = vch->next )
        {
                if ( vch->deleted || !vch->in_room )
                        continue;

                if ( vch->in_room == ch->in_room )
                {
                        if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
                                damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam, sn, FALSE );
                        continue;
                }

                if ( vch->in_room->area == ch->in_room->area
                    && IS_OUTSIDE( vch )
                    && IS_AWAKE( vch ) )
                        send_to_char( "The moon pulses in the sky.\n\r", vch );
        }
}


void spell_sunray( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        int        dam;
        AFFECT_DATA af;

        if ( !IS_OUTSIDE( ch ) )
        {
                send_to_char( "You must be outdoors.\n\r", ch );
                return;
        }

        if ( weather_info.sky > SKY_CLOUDY )
        {
                send_to_char( "You need clear weather.\n\r", ch );
                return;
        }

        if ( weather_info.sunlight == SUN_DARK )
        {
                send_to_char( "It is too dark to cast this spell.\n\r", ch );
                return;
        }

        dam = dice( level, 9 );

        if ( weather_info.sunlight == SUN_LIGHT )
                dam *= 1.5;

        if ( weather_info.sky == SKY_CLOUDY )
                dam /= 2;

        send_to_char( "A sunray fries your foes!\n\r", ch );
        act( "$n calls a sunray to toast $s foes!", ch, NULL, NULL, TO_ROOM );

        for ( vch = char_list; vch; vch = vch->next )
        {
                if ( vch->deleted || !vch->in_room )
                        continue;

                if ( vch->in_room == ch->in_room )
                {
                        if ( ! ( vch == ch || IS_AFFECTED( vch, AFF_BLIND )
                                || saves_spell( level, vch ) ) )
                        {
                                af.type      = gsn_blindness;
                                af.duration  = 1 + level;
                                af.location  = APPLY_HITROLL;
                                af.modifier  = -5;
                                af.bitvector = AFF_BLIND;
                                affect_to_char( vch, &af );

                                act( "$N is blinded!", ch, NULL, vch, TO_CHAR    );
                                act( "$N is blinded!", ch, NULL, vch, TO_NOTVICT );
                        }

                        if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
                                damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam, sn, FALSE );
                        continue;
                }

                if ( vch->in_room->area == ch->in_room->area
                    && IS_OUTSIDE( vch )
                    && IS_AWAKE( vch ) )
                        send_to_char( "The sun pulses violently in the sky.\n\r", vch );
        }
}


void spell_natures_fury( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        char* msg;
        int dam;

        dam = dice(level, 8);
        dam += 3 * level;

        if (saves_spell(level, victim))
                dam /= 2;

        switch (ch->in_room->sector_type)
        {
            case SECT_INSIDE:
            case SECT_CITY:
                dam /= 2;
                msg = "{yThe earth shudders violently below $N!{x";
                break;

            case SECT_WATER_SWIM:
            case SECT_WATER_NOSWIM:
            case SECT_UNDERWATER:
                msg = "{BA huge water spout erupts beneath $N!{x";
                break;

            case SECT_MOUNTAIN:
                msg = "{wHuge boulders crash down the mountainside into $N!{x";
                break;

            case SECT_DESERT:
                msg = "{YA violent sandstorm swirls about $N!{x";
                break;

            default:
                msg = "{WPowerful winds blow $N violently to the ground!{x";
                break;
        }

        if (spell_attack_number == 1)
        {
                act(msg, ch, NULL, victim, TO_CHAR);
                act(msg, ch, NULL, victim, TO_NOTVICT);
        }

        damage(ch, victim, dam, sn, FALSE);
}


void spell_recharge_item( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;

        if ( obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF )
        {
                send_to_char( "Only staves and wands may be recharged.\n\r", ch );
                return;
        }

        obj->value[2]++;

        /*
         * increase charge by one, and level and another charge if spell in item
         * is less than half of the caster's level.
         */

        if ( obj->level < ( level / 2 ) )
        {
                obj->level++;
                obj->value[2]++;

                if ( obj->value[0] < obj->level )
                        obj->value[0] = obj->level;

                if ( obj->item_type == ITEM_WAND )
                        send_to_char( "The wand shudders, then hums softly.\n\r", ch );
                else
                        send_to_char( "The staff vibrates, then hums softly.\n\r", ch );
        }
        else
        {
                if ( obj->item_type == ITEM_WAND )
                        send_to_char( "The wand glows softly.\n\r", ch );
                else
                        send_to_char( "The staff glows softly.\n\r", ch );
        }

        if ( obj->value[2] > obj->value[1] )
        {
                act ("It then shudders violently and implodes!", ch, obj, NULL, TO_CHAR );
                extract_obj ( obj );
        }
}


void spell_globe( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_GLOBE ) )
                return;

        if (ch->sub_class == SUB_CLASS_SATANIST)
                af.type = skill_lookup("dark globe");
        else
                af.type = sn;

        af.duration  = number_fuzzy( level / 6 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_GLOBE;
        affect_to_char( victim, &af );

        if (ch->sub_class == SUB_CLASS_SATANIST)
        {
                send_to_char( "You are surrounded by a globe of dark energy.\n\r",victim );
                act( "$n is surrounded by a ominous-looking globe.", victim, NULL, NULL, TO_ROOM );
        }
        else
        {
                send_to_char( "You are surrounded by an invulnerable globe.\n\r",victim );
                act( "$n is surrounded by a scintillating globe.", victim, NULL, NULL, TO_ROOM );
        }
}


void spell_dark_globe( int sn, int level, CHAR_DATA *ch, void *vo )
{
    return;
}


void spell_firestorm( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (!mob_interacts_players(vch))
                        continue;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC (vch ) )
                {
                        dam = dice (level, 6);

                        if (is_affected(vch, gsn_resist_heat)
                            || saves_spell(level, vch))
                                dam /= 1.5;

                        damage( ch, vch, dam, sn, FALSE );
                }
        }
}


void spell_meteor_storm( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (!mob_interacts_players(vch))
                        continue;

                if (vch == ch->mount
                    || is_group_members_mount (vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC (vch ) )
                {
                        dam = dice (level, 8);

                        if ( saves_spell( level, vch ) )
                                dam /= 1.5;

                        damage( ch, vch, dam, sn, FALSE );
                }
        }

        return;
}


void spell_hex( int sn, int level, CHAR_DATA *ch, void *vo )
{
        /*
         * Improved curse spell for witches.  cannot curse a victim already
         * hex'ed, hence the bitvector application :)
         */

        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
        {
                send_to_char( "They are already defiled.\n\r", ch );
                return;
        }

        if ( saves_spell( level, victim ) )
        {
                send_to_char( "Your curse is resisted.\n\r", ch );
                return;
        }

        af.type      = sn;
        af.duration  = level;

        /* stops recall as curse does :) */
        af.bitvector = AFF_CURSE;

        af.location  = APPLY_HITROLL;
        af.modifier  = - level / 2;
        affect_to_char( victim, &af );

        af.location  = APPLY_DAMROLL;
        af.modifier  = - level / 2;
        affect_to_char ( victim, &af );

        af.location  = APPLY_STR;
        af.modifier  = - level / 4;
        affect_to_char ( victim, &af );

        af.location  = APPLY_INT;
        af.modifier  = - level / 4;
        affect_to_char (victim, &af );

        af.location  = APPLY_WIS;
        af.modifier  = - level / 4;
        affect_to_char (victim, &af );

        af.location  = APPLY_DEX;
        af.modifier  = - level / 4;
        affect_to_char (victim, &af );

        af.location  = APPLY_CON;
        af.modifier  = - level / 4;
        affect_to_char (victim, &af );

        af.location  = APPLY_SAVING_SPELL;
        af.modifier  =  level / 2;
        affect_to_char( victim, &af );

        if ( ch != victim )
                send_to_char( "You defile their soul.\n\r", ch );

        send_to_char( "You feel unbelievably dirty.\n\r", victim );
}


void spell_animate_dead( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *obj = (OBJ_DATA *) vo;
        CHAR_DATA *victim= (CHAR_DATA *) vo;
        char buf [ MAX_STRING_LENGTH ];
        AFFECT_DATA af;

        if (obj->item_type != ITEM_CORPSE_NPC)
        {
                send_to_char ("There do not appear to be any fresh corpses about.\n\r", ch );
                return;
        }

        extract_obj ( obj );

        if ( ch->level < 35 )
                victim = ( create_mobile (get_mob_index ( MOB_VNUM_SKELETON ) ) );

        else if (ch->level < 45 )
                victim = ( create_mobile (get_mob_index ( MOB_VNUM_GHOUL ) ) );

        else
                victim = ( create_mobile (get_mob_index ( MOB_VNUM_GHOST ) ) );

        victim->level = UMAX(ch->level - 10, 1);
        victim->max_hit = victim->level * 8
                + number_range(victim->level * victim->level / 4, victim->level * victim->level);
        victim->hit = victim->max_hit;

        char_to_room ( victim, ch->in_room );

        sprintf ( buf, "You manipulate the corpse and it morphs into a %s.\n\r", victim->name );
        send_to_char ( buf, ch );

        act ( "The corpse in the room decays and a $d materialises to serve $n",
             ch, NULL, victim->name, TO_ROOM );

        if ( victim->master )
                stop_follower( victim );

        add_follower( victim, ch );

        af.type = gsn_charm_person;
        af.duration = level;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( victim, &af );
}


void spell_lore( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA    *obj = (OBJ_DATA *) vo;

        if (obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON)
        {
                send_to_char ( "You can't determine this item's properties.\n\r", ch );
                return;
        }

        spell_identify (sn, level, ch, vo);
}


void spell_possession( int sn, int level, CHAR_DATA *ch, void *vo  )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;

        return;

        if ( !ch->desc )
                return;

        if ( ch->desc->original )
        {
                send_to_char( "You already possess another.\n\r", ch );
                return;
        }

        if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch )
        {
                send_to_char( "You must charm your victim first!!\n\r", ch );
                return;
        }

        if ( victim == ch )
        {
                send_to_char( "Ok.\n\r", ch );
                return;
        }

        /*
         * Pointed out by Da Pub (What Mud)
         */
        if ( !IS_NPC( victim ) || ( ch->level - victim->level < 5 ) )
        {
                send_to_char( "Their will is too strong!\n\r", ch );
                return;
        }

        if ( victim->desc )
        {
                send_to_char( "Character is already possessed.\n\r", ch );
                return;
        }

        ch->pcdata->switched  = TRUE;
        ch->desc->character   = victim;
        ch->desc->original    = ch;
        victim->desc          = ch->desc;
        ch->desc              = NULL;

        send_to_char( "You concentrate on your victim's mind--suddenly you see what they see!\n\r", victim );
        return;
}


void spell_demon_flames( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
                return;

        af.type      = sn;
        af.duration  = level;
        af.location  = APPLY_AC;
        af.modifier  = 2 * level;
        af.bitvector = AFF_FAERIE_FIRE;
        affect_to_char( victim, &af );

        af.location     = APPLY_HITROLL;
        af.modifier     = -level/3;
        af.bitvector = AFF_FAERIE_FIRE;
        affect_to_char( victim, &af ),

        send_to_char( "Summoned demons breathe on you; you are surrounded by a fiery aura.\n\r", victim );
        act( "$c is surrounded by the fiery breath of summoned demons.", victim, NULL, NULL, TO_ROOM );
}


void spell_steal_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        {
                send_to_char("Your spell has no effect.\n\r",ch);
                return;
        }

        af.type      = sn;
        af.duration  = level / 2;
        af.location  = APPLY_STR;
        af.modifier  = -5;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.location  = APPLY_STR;
        af.modifier  = 5;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        if ( ch != victim )
                send_to_char( "You leech the strength from your enemy; you feel stronger.\n\r", ch );

        send_to_char( "You feel weaker.\n\r", victim );
}


void spell_steal_soul( int sn, int level, CHAR_DATA *ch, void *vo )
{
        OBJ_DATA *corpse =(OBJ_DATA *) vo;

        if (corpse->item_type != ITEM_CORPSE_NPC)
        {
                send_to_char ("You can only do that on fresh corpses.\n\r", ch );
                return;
        }

        send_to_char("You leech the soul from the corpse; you feel revitalised!\n\r", ch);
        act("$n sucks the soul from $p.", ch, corpse, NULL, TO_ROOM);

        if (ch->hit < ch->max_hit - ch->aggro_dam)
        {
                ch->hit = UMIN(ch->hit + 100, ch->max_hit - ch->aggro_dam);
                update_pos(ch);
        }

        extract_obj(corpse);
}


void spell_satans_fury (int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int dam;
        int hpch;

        if (IS_NPC(ch))
                return;

        if ( !IS_EVIL( ch ) )
        {
                send_to_char( "You are not evil enough to do that!\n\r", ch);
                return;
        }

        send_to_char( "You summon {RSatan{x to aid in your battle!\n\r", ch );
        act( "$n calls upon Satan to wreak havoc on $s enemies!", ch, NULL, NULL, TO_ROOM );

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if (vch == ch->mount
                    || is_group_members_mount(vch, ch)
                    || is_same_group(vch, ch))
                        continue;

                if (!mob_interacts_players(vch))
                        continue;

                if ( IS_NPC ( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
                {
                        hpch = UMAX( 10, ch->hit);
                        dam = number_range( hpch / 4 , hpch / 2 );

                        if ( saves_spell ( level, vch ) )
                                dam /= 1.5;

                        damage ( ch, vch, dam, sn, FALSE );
                }
        }
}


void spell_hand_of_lucifer( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *victim   = ( CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( IS_AFFECTED(victim, AFF_HOLD) )
        {
                send_to_char( "They are already held.\n\r", ch );
                return;
        }

        if (is_safe(ch, victim))
                return;

        af.type     = sn;
        af.duration = level/2;
        af.location = APPLY_HITROLL;
        af.modifier = -8;
        af.bitvector= AFF_HOLD;
        affect_to_char( victim, &af );

        act( "A dark hand materialises to grab $N, immobilising them.", ch, NULL, victim, TO_CHAR );
        act( "A giant hand grabs you, preventing any chance of escape!",  ch, NULL, victim, TO_VICT);
        act( "Dark mist forms into a hand which grabs $N, preventing their escape.",
            ch, NULL, victim, TO_ROOM );
}


void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;

        if (ch == victim)
        {
                send_to_char("You flee in fear from yourself!\n\r", ch);
                do_flee(ch, "");
                return;
        }

        if (saves_spell(ch->level, victim))
        {
                send_to_char( "You failed.\n\r", ch );
                return;
        }

        send_to_char("Your victim is overcome by fear and attempts to flee!\n\r", ch);

        /* newline added to following for nice formatting after do_look */
        act ("$c fills you with abject terror!\n\r", ch, NULL, victim, TO_VICT);

        act ("$C is overcome with fear!", ch, NULL, victim, TO_NOTVICT);
        do_flee( victim, "Fear" );
}


void spell_astral_sidestep( int sn, int level, CHAR_DATA *ch, void *vo )
{
        AFFECT_DATA af;

        if ( IS_NPC(ch) )
                return;

        if (is_entered_in_tournament(ch)
            && tournament_status == TOURNAMENT_STATUS_RUNNING
            && is_still_alive_in_tournament(ch))
        {
                send_to_char("Not during the tournament!\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_astral_sidestep))
                return;

        af.type      = sn;
        af.duration  = number_fuzzy( level / 10 );
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_NON_CORPOREAL;
        affect_to_char( ch, &af );

        act( "$n dissolves into the ether.", ch, NULL, NULL, TO_ROOM );
        act( "You dematerialise and enter a parallel dimension.", ch, NULL, NULL, TO_CHAR );

}


void spell_gaias_warning (int sn, int level, CHAR_DATA *ch, void *vo)
{
        AFFECT_DATA af;

        if (IS_NPC(ch))
                return;

        if (ch->form != FORM_WOLF && ch->form != FORM_DIREWOLF)
        {
                send_to_char("You must be in wolf form to cast this spell.\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_gaias_warning))
                return;

        af.type      = gsn_gaias_warning;
        af.duration  = number_fuzzy( level / 10 );
        af.location  = APPLY_NONE;
        af.bitvector = 0;
        af.modifier  = 0;
        affect_to_char( ch, &af );

        act( "You become more aware of the dangers that surround you.", ch, NULL, NULL, TO_CHAR );
}


void spell_resist_magic(  int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = level / 2 + 7;
        af.modifier  = -level / 5;
        af.location  = APPLY_SAVING_SPELL;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You feel resistant to magical attacks.\n\r", ch );
}


void spell_haste(  int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
        {
                send_to_char("They are already hastened.\n\r",ch);
                return;
        }

        af.type      = sn;
        af.duration  = level / 2 + 7;
        af.modifier  = 0;
        af.location  = APPLY_NONE;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        act ( "$n speeds up.", victim, NULL, NULL, TO_ROOM);
        send_to_char( "You feel quick.\n\r", victim );
}


void spell_animate_weapon (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        OBJ_DATA  *obj;

        if ((obj = get_eq_char(victim,WEAR_DUAL))
            && !IS_SET(obj->extra_flags, ITEM_BODY_PART))
        {
                act("You focus on $N's other weapon.", ch, NULL, victim, TO_CHAR);

                if ( saves_spell( level, victim ) )
                {
                        send_to_char("They moved too quickly.\n\r",ch);
                        return;
                }

                unequip_char(victim,obj);
                obj_from_char(obj);

                if ( IS_NPC (victim) )
                        obj_to_char(obj,victim);
                else
                        obj_to_room(obj,victim->in_room);

                act("Your weapon slips from your hands!", ch, NULL, victim, TO_VICT);
                act("$N's weapon slips from $s hands.", ch, NULL, victim, TO_NOTVICT);

                if (ch != victim)
                        send_to_char("Success!\n\r",ch);

                return;
        }

        if ((obj = get_eq_char(victim,WEAR_WIELD) ) != NULL)
        {
                if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
                {
                        act ("You cannot disarm $p.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                act("You focus on $N's weapon.", ch, NULL, victim, TO_CHAR);

                if ( saves_spell( level, victim ) )
                {
                        send_to_char("They moved too quickly.\n\r",ch);
                        return;
                }

                unequip_char(victim, obj);
                obj_from_char( obj);

                if ( IS_NPC( victim ) )
                        obj_to_char( obj, victim);
                else
                        obj_to_room( obj, victim->in_room);

                act("Your weapon slips from your hands!", ch, NULL, victim, TO_VICT);
                act("$N's weapon slips from $s hands.", ch, NULL, victim, TO_NOTVICT);
                if (ch != victim)
                        send_to_char("Success!\n\r",ch);
                return;
        }

        send_to_char("They don't have a weapon.\n\r",ch);
        return;
}


/*
 * Magical transport an item from one player's inventory to another
 * player somewhere in the mud.  Ripped from Smaug,  by Istari
 */
void spell_transport( int sn, int level, CHAR_DATA *ch, void *vo)
{
        OBJ_DATA *obj;
        CHAR_DATA *victim;
        char arg3[MAX_INPUT_LENGTH];

        target_name = one_argument(target_name, arg3);

        if ( ( !( victim = get_char_world (ch, target_name ) ) )
            || ( !IS_NPC ( ch ) && ch->fighting )
            || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY )
            || victim->level >= level +3
            || victim == ch )
        {
                send_to_char("Nothing happens.\n\r",ch);
                return;
        }

        if (arg3[0] == '\0')
        {
                send_to_char("What do you wish to transport?\n\r",ch);
                return;
        }

        if (  !(obj = get_obj_carry(ch,arg3) ) )
        {
                send_to_char( "You are not carrying that. \n\r",ch);
                return;
        }

        if (!can_drop_obj(ch,obj))
        {
                act("You can't let go of it!", ch, obj, NULL, TO_CHAR);
                return;
        }

        if (victim->carry_number + get_obj_number(obj) > can_carry_n(victim)
            || victim->carry_weight + victim->coin_weight + get_obj_weight(obj)
               > can_carry_w(victim))
        {
                act("$p begins to fade... then becomes soild once more.", ch, obj, NULL, TO_CHAR);
                return;
        }

        act("$p slowly dematerialises...", ch, obj, NULL, TO_CHAR);
        act("$p slowly dematerialises from $n's hands..", ch, obj, NULL, TO_ROOM);

        obj_from_char( obj );
        obj_to_char( obj, victim );

        act( "$p from $n appears in your hands!", ch, obj, victim,TO_VICT);
        return;
}


void spell_mass_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        ROOM_INDEX_DATA *pRoomIndex;

        for ( ; ; )
        {
                pRoomIndex = get_room_index( number_range( 0, 65535 ) );
                if ( pRoomIndex
                    && !IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE )
                    && !IS_SET( pRoomIndex->room_flags, ROOM_NO_RECALL )
                    && !IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY )
                    && pRoomIndex->area == ch->in_room->area )
                        break;
        }

        for ( vch = ch->in_room->people; vch; vch = vch_next )
        {
                vch_next = vch->next_in_room;

                if ( vch->deleted )
                        continue;

                if (ch == vch)
                        continue;

                if (IS_NPC(vch))
                        continue;

                act( "$n slowly dissolves into thin air.", vch, NULL, NULL, TO_ROOM);
                char_from_room( vch );
                char_to_room( vch, pRoomIndex );
                act( "$n slowly fades into existence.", vch, NULL, NULL, TO_ROOM);
                do_look( vch, "auto" );
        }

        char_from_room(ch);
        char_to_room(ch,pRoomIndex);
        do_look(ch,"auto");
        return;
}


void spell_dragon_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
        CHAR_DATA  *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if ( is_affected( victim, sn ) )
                return;

        af.type      = sn;
        af.duration  = ch->level / 3;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        send_to_char( "You feel less vulnerable.\n\r",victim );
}


/*
 * Check to see if a spell / skill can hit a mob.
 * Shade / Gezhp
 */
bool mob_interacts_players (CHAR_DATA *mob)
{
        if (IS_AFFECTED(mob, AFF_NON_CORPOREAL))
                return FALSE;

        if (!IS_NPC(mob))
                return TRUE;

        if (IS_SET(mob->act, ACT_WIZINVIS_MOB))
                return FALSE;

        if (mob->name[0] == '\0')
                return FALSE;

        if (mob->rider && (IS_SET(mob->in_room->room_flags, ROOM_SAFE)
                           || IS_SET(mob->in_room->area->area_flags, AREA_FLAG_SAFE)))
                return FALSE;

        return TRUE;
}


void spell_breathe_water (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (IS_NPC(victim)
            || victim->position == POS_FIGHTING
            || is_affected(victim, sn))
                return;

        af.type = sn;
        af.duration = 5 + level / 2;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);

        victim->pcdata->air_supply = FULL_AIR_SUPPLY;

        send_to_char("Your lungs begin to tingle and pulse.\n\r", victim);

        if (victim != ch)
                send_to_char("Ok.\n\r", ch);
}


void spell_resist_heat (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, sn))
                return;

        af.type = sn;
        af.duration = 5 + level / 4;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);

        send_to_char("You feel more resistant to heat and flame.\n\r", victim);

        if (victim != ch)
                send_to_char("Ok.\n\r", ch);
}


void spell_resist_cold (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, sn))
                return;

        af.type = sn;
        af.duration = 5 + level / 4;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);

        send_to_char("You feel more resistant to cold and ice.\n\r", victim);

        if (victim != ch)
                send_to_char("Ok.\n\r", ch);
}


void spell_resist_lightning (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, sn))
                return;

        af.type = sn;
        af.duration = 5 + level / 4;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);

        send_to_char("You feel more resistant to electricity.\n\r", victim);

        if (victim != ch)
                send_to_char("Ok.\n\r", ch);
}


void spell_resist_acid (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        AFFECT_DATA af;

        if (is_affected(victim, sn))
                return;

        af.type = sn;
        af.duration = 5 + level / 4;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);

        send_to_char("You feel more resistant to acid.\n\r", victim);

        if (victim != ch)
                send_to_char("Ok.\n\r", ch);
}


void spell_hells_fire (int sn, int level, CHAR_DATA *ch, void *vo)
{
        CHAR_DATA *victim = (CHAR_DATA *) vo;
        int dam;

        /* 3-10 */
        dam = dice(level, 8);
        dam += level * 2;

        if (saves_spell(level, victim) || is_affected(victim, gsn_resist_heat))
                dam /= 2;

        if (spell_attack_number == 1)
        {
                act("{RFlames{x burst forth from the ground around $N!", ch, NULL, victim, TO_NOTVICT);
                act("You summon {Rflames{x from the pits of Hell!", ch, NULL, NULL, TO_CHAR);
        }

        damage(ch, victim, dam, sn, FALSE);
}

void spell_chaos_blast (int sn, int level, CHAR_DATA *ch, void *vo)
{
    /*
     Does factorial damage, somewhat comical opener spell for Satanists --Owl 1/3/22
    */

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int    dam;
    double factorials[7]    = { 1.0, 2.0, 6.0, 24.0, 120.0, 720.0, 5040.0 };
    int    rnd_idx          = rand() % 7;
    double rnd_factorial    = factorials[rnd_idx];
    double fuzz_value;
    int    flip             = rand() % 2;
    double range            = 1.0;
    double div              = RAND_MAX / range;

    if (victim->hit < victim->max_hit
        && IS_NPC(victim)
        && victim->pIndexData->vnum != BOT_VNUM)
    {
        act ("{W$C is too alert for you to unleash chaos energy on $m!{x",
                ch, NULL, victim, TO_CHAR);
        return;
    }

    if ( !IS_EVIL( ch ) )
    {
        act ("{WYou are not evil enough to summon chaos energy.{x",
             ch, NULL, victim, TO_CHAR);
        return;
    }

    /* better chance of good damage if victim is held, i.e. by hand of lucifer */
    if (IS_AFFECTED(victim, AFF_HOLD) &&
        (rnd_idx != 6))
    {
        rnd_idx++;
    }

    fuzz_value = (rnd_factorial/20 * (rand()/div));

    dam = (flip) ? (int)(((rnd_factorial/100) * level) + fuzz_value) :
                   (int)(((rnd_factorial/100) * level) - fuzz_value);

    /*fprintf(stderr, "rnd_idx = %d | rnd_factorial = %f | dam = %d | flip = %d | fuzz = %f\r\n", rnd_idx, rnd_factorial, dam, flip, fuzz_value  );*/

    act("Tendrils of chaos energy snake from $n's hand!", ch, NULL, NULL, TO_ROOM);
    act("You unleash a blast of pure {Wc{Bh{Wa{Bo{Ws{x energy!", ch, NULL, NULL, TO_CHAR);

    damage(ch, victim, dam, sn, FALSE);

}


/*
 * Some affect types cannot be dispelled
 */
bool skill_cannot_be_dispelled (int sn)
{
        if (sn == gsn_dirt
            || sn == gsn_coil
            || sn == gsn_stun
            || sn == gsn_transfix
            || sn == gsn_trap
            || sn == gsn_gouge
            || sn == gsn_choke
            || sn == gsn_howl
            || sn == gsn_web
            || sn == gsn_mount
            || sn == gsn_battle_aura
            || sn == gsn_berserk
            || sn == gsn_warcry
            || sn == gsn_swim)
                return TRUE;

        return FALSE;
}


/* EOF magic.c */


