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


void do_morph_chameleon (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;

        if (to_form)
        {
                /* Remove any arm trauma, cat form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                if (ch->pcdata->learned[gsn_form_chameleon] > 60
                    || (ch->pcdata->learned[gsn_hide] > (80 - ch->pcdata->learned[gsn_form_chameleon])))
                {
                        affect_strip(ch, gsn_hide);
                        send_to_char("Your new form enables you to hide.\n\r", ch);
                        SET_BIT(ch->affected_by, AFF_HIDE);
                }

                if (ch->pcdata->learned[gsn_form_chameleon] > 30
                     || (ch->pcdata->learned[gsn_sneak] > (140 - ch->pcdata->learned[gsn_form_chameleon])))
                {
                        affect_strip(ch, gsn_sneak);
                        send_to_char("Your new form allows you to move amongst the shadows.\n\r", ch);

                        af.type      = gsn_sneak;
                        af.duration  = -1;
                        af.modifier  = APPLY_NONE;
                        af.location  = 0;
                        af.bitvector = AFF_SNEAK;
                        affect_to_char(ch, &af);
                }
        }
        else
        {
                affect_strip(ch, gsn_hide);
                REMOVE_BIT(ch->affected_by, AFF_HIDE);
                affect_strip(ch, gsn_sneak);
                REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        }
}


void do_spy (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *wch;
        CHAR_DATA       *victim;
        ROOM_INDEX_DATA *location;
        ROOM_INDEX_DATA *original;
        char             arg [ MAX_INPUT_LENGTH ];

        argument = one_argument( argument, arg );

        /* swap do_spy, gsn_spy etc  out for surveil once it's working */
        if ( IS_NPC( ch ) )
                return;

        if ( !IS_AWAKE( ch ) || !check_blind( ch ) )
                return;

        if ( !IS_NPC( ch ) && !CAN_DO( ch, gsn_spy ) )
        {
                send_to_char("You do not have that ability.\n\r", ch);
                return;
        }

        if (ch->form != FORM_HAWK)
        {
                send_to_char("You must be in hawk form to surveil someone.\n\r", ch);
                return;
        }

        if ( arg[0] == '\0' )
        {
                send_to_char( "Surveil whom?\n\r", ch );
                return;
        }

        if ( !( victim = get_char_world( ch, arg ) ) || !victim->desc )
        {
                send_to_char( "They are not currently in the domain.\n\r", ch );
                return;
        }

        if ( victim == ch )
        {
                send_to_char( "You don't need to surveil yourself, you're right here!\n\r", ch );
                return;
        }

        if ( IS_IMMORTAL( victim ) )
        {
                send_to_char("You may not surveil the gods.\n\r", ch);
                return;
        }

        if ( !IS_OUTSIDE( ch ) )
        {
                act( "You must be outdoors to attempt that.", ch, NULL, victim, TO_CHAR );
                return;
        }

        if ( !IS_OUTSIDE( victim ) )
        {
                act( "You can't surveil $N; they must be indoors.", ch, NULL, victim, TO_CHAR );
                return;
        }

        /*
           Do the spying. basically check victim is in same area and force look output
           You can also surveil mobs.
        */
        location = find_location( ch, arg );

        original = ch->in_room;
        char_from_room( ch );
        char_to_room( ch, location );
        act( "{YWith your keen hawk senses you track down $N--you see what they see!{x\n\r", ch, NULL, victim, TO_CHAR );
        interpret( ch, "look" );

        for ( wch = char_list; wch; wch = wch->next )
        {
                if ( wch == ch )
                {
                        char_from_room( ch );
                        char_to_room( ch, original );
                        break;
                }
        }
}

void do_soar(CHAR_DATA* ch, char* argument)
{
        char arg [ MAX_INPUT_LENGTH ];
        int place, i, target;
        ROOM_INDEX_DATA *location;
        char buf[MAX_STRING_LENGTH];
        char tmp[MAX_STRING_LENGTH];

        if( IS_NPC( ch ) )
                return;

        if( ch->class != CLASS_SHAPE_SHIFTER
        &&  !IS_IMMORTAL( ch ) )
        {
                send_to_char("You don't know how to do that.\n\r", ch);
                return;
        }

        if(ch->form != FORM_HAWK)
        {
                send_to_char("You need to be in hawk form to do that.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You can't think about long-distance travel while you're fighting!\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                sprintf(buf, "Soar locations available are:\n");

                for (i=1;i<MAX_SOAR;i++)
                {
                        if (is_valid_soar(ch, i))
                        {
                                place = soar_list[i].destination;
                                location = get_room_index(place);
                                sprintf(tmp, "%d. %s\n", i, location->name);
                                strcat(buf, tmp);
                        }
                }

                send_to_char(buf, ch);
                return;
        }

        if (!str_cmp(arg, "mem"))
        {
                if ( IS_SET( ch->in_room->room_flags, ROOM_INDOORS )
                ||   IS_SET( ch->in_room->room_flags, ROOM_PRIVATE )
                ||   IS_SET( ch->in_room->room_flags, ROOM_SOLITARY )
                ||   IS_SET( ch->in_room->room_flags, ROOM_NO_MOUNT )
                || ( ch->in_room->sector_type != SECT_FIELD
                &&   ch->in_room->sector_type != SECT_FOREST
                &&   ch->in_room->sector_type != SECT_HILLS
                &&   ch->in_room->sector_type != SECT_MOUNTAIN
                &&   ch->in_room->sector_type != SECT_DESERT
                &&   ch->in_room->sector_type != SECT_SWAMP
                &&   ch->in_room->sector_type != SECT_WATER_SWIM
                &&   ch->in_room->sector_type != SECT_WATER_NOSWIM ) )
                {
                        send_to_char("You can't land in this sort of terrain.\n\r", ch);
                        return;
                }

                if (ch->in_room->area->low_level == -4 && ch->in_room->area->high_level == -4)
                {
                        send_to_char("You can't memorise this location.\n\r", ch);
                        return;
                }

                if(ch->move < 50)
                {
                        send_to_char("You are too tired to memorise this location.\n\r",ch);
                        return;
                }

                ch->move -= 50;
                ch->pcdata->soar = ch->in_room->vnum;

                send_to_char("{WYou concentrate hard and memorise your surroundings.{x\n\r", ch);
                act("{W$c concentrates hard and studies $s surroundings.{x",

                        ch, NULL, NULL, TO_ROOM);
                WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                return;
        }

        if (!str_cmp(arg, "return"))
        {
                /*
                 * You CAN soar to and from a no_recall room if other criteria are met.  Feature not a bug.
                 */

                if ( IS_SET( ch->in_room->room_flags, ROOM_INDOORS )
                ||   IS_SET( ch->in_room->room_flags, ROOM_PRIVATE )
                ||   IS_SET( ch->in_room->room_flags, ROOM_SOLITARY )
                ||   IS_SET( ch->in_room->room_flags, ROOM_NO_MOUNT )
                || ( ch->in_room->sector_type != SECT_FIELD
                &&   ch->in_room->sector_type != SECT_FOREST
                &&   ch->in_room->sector_type != SECT_HILLS
                &&   ch->in_room->sector_type != SECT_MOUNTAIN
                &&   ch->in_room->sector_type != SECT_DESERT
                &&   ch->in_room->sector_type != SECT_SWAMP
                &&   ch->in_room->sector_type != SECT_WATER_SWIM
                &&   ch->in_room->sector_type != SECT_WATER_NOSWIM ) )

                {
                        send_to_char("You can't take off from this terrain.\n\r", ch);
                        return;
                }
                else
                {
                        if (ch->pcdata->soar)
                        {
                                location = get_room_index(ch->pcdata->soar);
                                ch->pcdata->soar = 0;
                        }
                        else
                        {
                                send_to_char("You have no location memorised.\n\r", ch);
                                return;
                        }
                }
        }
        else
        {
                target = atoi(arg);

                if ((target < 1) || (target > MAX_SOAR - 1))
                {
                        send_to_char("Syntax: soar # | soar mem | soar return\n\r", ch);
                        return;
                }

                if (!is_valid_soar(ch, target))
                {
                        send_to_char("You don't know of that destination.\n\r", ch);
                        return;
                }

                location = get_room_index(soar_list[target].destination);
        }

        if ( IS_SET( ch->in_room->room_flags, ROOM_INDOORS )
        ||   IS_SET( ch->in_room->room_flags, ROOM_PRIVATE )
        ||   IS_SET( ch->in_room->room_flags, ROOM_SOLITARY )
        ||   IS_SET( ch->in_room->room_flags, ROOM_NO_MOUNT )
        || ( ch->in_room->sector_type != SECT_FIELD
        &&   ch->in_room->sector_type != SECT_FOREST
        &&   ch->in_room->sector_type != SECT_HILLS
        &&   ch->in_room->sector_type != SECT_MOUNTAIN
        &&   ch->in_room->sector_type != SECT_DESERT
        &&   ch->in_room->sector_type != SECT_SWAMP
        &&   ch->in_room->sector_type != SECT_WATER_SWIM
        &&   ch->in_room->sector_type != SECT_WATER_NOSWIM ) )

        {
                send_to_char("You can't take off from this terrain.\n\r", ch);
                return;

        }

        if(ch->move < 25)
        {
                send_to_char("You are too exhausted to take off from this location.\n\r",ch);
                return;
        }

        if (get_room_index(ch->in_room->vnum) == location)
        {
                send_to_char("You're there right now!\n\r",ch);
                return;
        }

        if ( ch->mount )
        {
                act ("{yYou pick up $N in your talons.\n\r{x", ch, NULL, ch->mount, TO_CHAR);
                act ("{y$C picks up $n in $S talons.{x\n\r", ch->mount, NULL, ch, TO_NOTVICT);
        }

        act ("{y$c soars into the sky and quickly vanishes from your sight.{x", ch, NULL, NULL, TO_ROOM);
        send_to_char("{yYou fly into the air and soar through the skies...{x\n\r\n\r", ch);

        char_from_room(ch);
        char_to_room(ch, location);

        if (ch->mount)
        {
                char_from_room(ch->mount);
                char_to_room(ch->mount, location);
        }

        if ( ch->mount
        && ( ch->in_room->sector_type == SECT_WATER_SWIM
        ||   ch->in_room->sector_type == SECT_WATER_NOSWIM
        ||   ch->in_room->sector_type == SECT_SWAMP ) )
        {
                act ("{yYou release $N from your talons and $E splashes down into the water.\n\r{x", ch, NULL, ch->mount, TO_CHAR);
                act ("{y$N releases $n from $S talons and $e splashes down into the water.\n\r{x", ch->mount, NULL, ch, TO_NOTVICT);
        }

        if ( ch->mount
        &&   ch->in_room->sector_type != SECT_WATER_SWIM
        &&   ch->in_room->sector_type != SECT_WATER_NOSWIM
        &&   ch->in_room->sector_type != SECT_SWAMP )
        {
                act ("{yYou release $N from your talons and $E tumbles to the ground.\n\r{x", ch, NULL, ch->mount, TO_CHAR);
                act ("{y$N releases $n from $S talons and $e tumbles to the ground.\n\r{x", ch->mount, NULL, ch, TO_NOTVICT);
        }

        if ( ch->in_room->sector_type == SECT_WATER_SWIM
        ||   ch->in_room->sector_type == SECT_WATER_NOSWIM
        ||   ch->in_room->sector_type == SECT_SWAMP )
        {
                act ("{y$c swoops down from the sky and lands gracefully on the water.{x", ch, NULL, NULL, TO_ROOM);
        }
        else
        {
                act ("{y$c swoops down from the sky and alights nearby.{x", ch, NULL, NULL, TO_ROOM);
        }

        do_look(ch, "auto");

        ch->move -= 25;
        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        return;
}

bool is_valid_soar (CHAR_DATA *ch, int soar_index )
{
        /* Figures out which soar locations a character has access to. */

        if (soar_index > MAX_SOAR || soar_index < 1)
        {
                return 0;
        }

        if ( ch->pcdata->learned[gsn_form_hawk] >= soar_access[soar_index].hawkform_perc
        &&   ch->pcdata->learned[gsn_soar]      >= soar_access[soar_index].soar_perc
        &&   ch->level                          >= soar_access[soar_index].char_level )
        {
                return 1;
        }

        return 0;
}

void do_morph_hawk (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;

        if (to_form)
        {
                /* Remove any arm trauma, hawk form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                if (ch->pcdata->learned[gsn_form_hawk] > 40 || ch->pcdata->learned[gsn_fly])
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);
                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                }
        }
        else
        {
                affect_strip(ch, gsn_fly);
                affect_strip(ch, gsn_levitation);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);
        }
}


void do_morph_cat (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;

        if (to_form)
        {

                /* Remove any arm trauma, cat form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                if (ch->pcdata->learned[gsn_form_cat] > 30)
                {
                        affect_strip(ch, gsn_infravision);
                        send_to_char("Your new form causes your eyes to glow red.\n\r", ch);

                        af.type      = gsn_infravision;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_INFRARED;
                        affect_to_char(ch, &af);

                }

                if (ch->pcdata->learned[gsn_form_cat] > 50)
                {
                        affect_strip(ch, gsn_detect_hidden);
                        send_to_char("Your new form causes your awareness to improve.\n\r", ch);

                        af.type      = gsn_detect_hidden;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_DETECT_HIDDEN;
                        affect_to_char(ch, &af);
                }

                if (ch->pcdata->learned[gsn_form_cat] > 60)
                {
                        affect_strip(ch, gsn_detect_invis);
                        send_to_char("Your new form causes your senses to rocket.\n\r", ch);

                        af.type      = gsn_detect_invis;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_DETECT_INVIS;
                        affect_to_char(ch, &af);

                        af.type      = gsn_detect_sneak;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_DETECT_SNEAK;
                        affect_to_char(ch, &af);
                }

        }
        else
        {
                affect_strip(ch, gsn_infravision);
                REMOVE_BIT(ch->affected_by, AFF_INFRARED);
                affect_strip(ch, gsn_detect_hidden);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
                affect_strip(ch, gsn_detect_invis);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_INVIS);
                affect_strip(ch, gsn_detect_sneak);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_SNEAK);
                affect_strip(ch, gsn_heighten);
        }
}


void do_coil (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        AFFECT_DATA af;
        char arg[MAX_INPUT_LENGTH];

        if (!IS_NPC(ch) && !CAN_DO(ch, gsn_coil))
        {
                send_to_char("You tie yourself in knots foolishly trying to impersonate a snake.\n\r", ch);
                return;
        }

        if ( ch->form != FORM_SNAKE
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") )  )
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        victim = ch->fighting;
        one_argument(argument, arg);

        if (arg[0] == '\0' && !victim)
        {
                send_to_char("Coil around whom?\n\r", ch);
                return;
        }

        if (arg[0] != '\0' && !(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (is_safe (ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                send_to_char("Your tail is too damaged to wrap it around anything.\n\r", ch);
                return;
        }

        if (victim->position > POS_STUNNED)
        {
                if (!victim->fighting)
                        set_fighting(victim, ch);

                victim->position = POS_FIGHTING;

                if (!ch->fighting)
                        set_fighting(ch, victim);
        }

        if (is_affected(victim, gsn_coil))
        {
                send_to_char("Do that again and you will never untangle yourself...\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_coil].beats);

        if (IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_coil])
        {
                act ("You coil your body around $N.", ch, NULL, victim, TO_CHAR);
                act ("$n encoils you in $s flesh - you are trapped!", ch, NULL, victim, TO_VICT);
                act ("$n coils $s length about $N.", ch, NULL, victim, TO_NOTVICT);
                arena_commentary("$n coils about $N.", ch, victim);

                WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
                victim->position = POS_RESTING;
                WAIT_STATE(victim, 2 * PULSE_VIOLENCE);

                af.type      = gsn_coil;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_HOLD;
                affect_to_char(victim, &af);

                af.location = APPLY_HITROLL;
                af.bitvector = 0;
                af.modifier = get_curr_str(ch) / 2;
                affect_to_char(victim, &af);
        }
        else
                send_to_char("You fail to encoil your target.\n\r",ch);
}


void do_constrict (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;

        if ( IS_NPC( ch )
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") )  )
                return;

        if ( !IS_NPC( ch)
        && !CAN_DO(ch, gsn_constrict))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->form != FORM_SNAKE
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!is_affected(victim, gsn_coil))
        {
                send_to_char("You need to encoil them first!\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                send_to_char("Your tail is too damaged to constrict it around anything.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_constrict].beats);

        send_to_char("You {Gconstrict{x your opponent in your coils!\n\r", ch);
        damage(ch, victim, number_range(ch->level * 2, ch->level * 7/2), gsn_constrict, FALSE);
}



void do_strangle (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        int base_damage;

        if ( IS_NPC( ch )
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
                return;

        if ( !IS_NPC( ch)
        && !CAN_DO(ch, gsn_constrict))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->form != FORM_SNAKE
        && !( ch->spec_fun == spec_lookup("spec_laghathti") )
        && !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }

        if (!is_affected(victim, gsn_coil))
        {
                send_to_char("You need to encoil them first!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_strangle].beats);

        base_damage = number_range (2 * ch->level, 5 * ch->level);

        if ( ( victim->hit > (victim->max_hit / 2) )
        &&  !( ch->spec_fun == spec_lookup("spec_laghathti") )
        &&  !( ch->spec_fun == spec_lookup("spec_uzollru") ) )
        {
                send_to_char("You try to strangle your victim but they are currently too strong, and resist!\n\r",ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_TAIL_TRAUMA))
        {
                send_to_char("Your tail is too damaged to strangle anyone with it.\n\r", ch);
                return;
        }

        if (victim->hit < (victim->max_hit / 10))
        {
                damage(ch, victim, base_damage * 3, gsn_strangle, FALSE);
                return;
        }

        if (victim->hit < (victim->max_hit / 4))
        {
                damage(ch, victim, base_damage * 2, gsn_strangle, FALSE);
                return;
        }

        if (victim->hit < (victim->max_hit / 3))
        {
                damage(ch, victim, base_damage * 1.5, gsn_strangle, FALSE);
                return;
        }

        damage(ch, victim, base_damage, gsn_strangle, FALSE);
}


void do_morph_snake (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;
        OBJ_DATA *bite;

        if (to_form)
        {
                /* Remove any arm or leg trauma, snake form doesn't have arms or legs */
                affect_strip(ch, gsn_arm_trauma);
                affect_strip(ch, gsn_leg_trauma);

                bite = create_object(get_obj_index(OBJ_SNAKE_BITE), ch->level, "common", CREATED_NO_RANDOMISER);
                obj_to_char(bite, ch);
                form_equip_char(ch, bite, WEAR_WIELD);

                if (ch->pcdata->learned[gsn_form_snake] > 30)
                {
                        affect_strip(ch, gsn_swim);
                        send_to_char("Your new form enables you to swim.\n\r", ch);

                        af.type      = gsn_swim;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_SWIM;
                        affect_to_char(ch, &af);
                }

                if (ch->pcdata->learned[gsn_form_snake] > 50)
                {

                        send_to_char("Your new form enables you to breathe underwater.\n\r", ch);
                        affect_strip(ch, gsn_breathe_water);
                        af.type = gsn_breathe_water;
                        af.bitvector = 0;
                        affect_to_char(ch, &af);
                }
        }
        else
        {

                bite = get_obj_wear(ch, "bite");
                if (bite)
                {
                        unequip_char(ch, bite);
                        extract_obj(bite);
                }

                affect_strip(ch, gsn_swim);
                affect_strip(ch, gsn_breathe_water);
                REMOVE_BIT(ch->affected_by, AFF_SWIM);
        }
}


void do_morph_scorpion (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *sting;
        AFFECT_DATA *paf;

        if (to_form)
        {

                /* Remove any arm trauma, scorpion form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                sting = create_object(get_obj_index(OBJ_SCORPION_STING), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = sting->affected;
                sting->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = sting->affected;
                sting->affected = paf;

                obj_to_char(sting, ch);
                form_equip_char(ch, sting, WEAR_WIELD);

                send_to_char("You may backstab with your stinger...\n\r", ch);
        }
        else
        {
                sting = get_obj_wear(ch, "sting");
                if (sting)
                {
                        unequip_char(ch, sting);
                        extract_obj(sting);
                }
        }
}


void do_venom (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_venom))
        {
                send_to_char("What do you think you are, a spider?\n\r", ch);
                return;
        }

        if (ch->form != FORM_SPIDER)
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You are not fighting anyone.\n\r", ch);
                return;
        }

        if (IS_NPC(victim) && IS_SET(victim->act, ACT_OBJECT))
        {
                send_to_char("Objects are not affected by venom.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your mouthparts are too damaged to inflict venom.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_venom].beats);
        act ("You bite $N!",  ch, NULL, victim, TO_CHAR);
        act ("$n bites you!", ch, NULL, victim, TO_VICT);
        act ("$n bites $N!",  ch, NULL, victim, TO_NOTVICT);

        spell_poison (gsn_venom, ch->level, ch, victim);
}


void do_web (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];
        int chance;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_web))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->form != FORM_SPIDER)
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2);  /* Not too much spam thanks */

        one_argument (argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Whom do you want to trap in your web?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(victim, AFF_HOLD))
        {
                act ("$N is already stuck here by some means.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You want to spin a web around yourself?\n\r", ch);
                return;
        }

        if (victim->fighting)
        {
                send_to_char("You cannot web a fighting person.\n\r", ch);
                return;
        }

        if (is_safe(ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_TORSO_TRAUMA))
        {
                send_to_char("Your body is too damaged to produce webbing.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, PULSE_VIOLENCE);

        /*
         * Web gets better.
         */

        chance = 50 + (ch->pcdata->learned[gsn_web] / 2);
        chance += (ch->level - victim->level) * 3;

        chance = URANGE(5, chance, 95);

        if (number_percent() < chance)
        {
                AFFECT_DATA af;
                act ("You skilfully spin a sticky web around $N, trapping $M here!",
                     ch, NULL, victim, TO_CHAR);
                act ("$n skilfully spins a sticky web around $N, trapping $M here!",
                     ch, NULL, victim, TO_ROOM);
                arena_commentary("$n traps $N in a sticky web.", ch, victim);

                af.type = gsn_web;
                af.duration = 4 + (ch->level / 20);
                af.location = APPLY_HITROLL;
                af.modifier = -8;
                af.bitvector = AFF_HOLD;

                affect_to_char(victim, &af);

                WAIT_STATE(victim, PULSE_VIOLENCE);

                check_group_bonus(ch);

        }
        else
        {
                act ("$n attempts to spin a web around $N, but $N breaks free!",
                     ch, NULL, victim, TO_ROOM);
                act ("You fail to spin a web around your victim, $E breaks free!",
                     ch, NULL, victim, TO_CHAR);
                arena_commentary("$n tries to trap $N in a sticky web, but $N breaks free!", ch, victim);

                /*
                 * Web gets worse.
                 */

                if (!IS_NPC(victim))
                {
                        damage(ch, victim, 0, gsn_web, FALSE);
                        return;
                }

                if (victim->level > 60)
                {
                        damage(ch, victim, 0, gsn_web, FALSE);
                        return;
                }
                else
                {
                        if (victim->level > 20)
                        {
                                if (number_bits(1))
                                {
                                        damage(ch, victim, 0, gsn_web, FALSE);
                                        return;
                                }
                        }
                        else
                        {
                                if (!number_bits(2))
                                {
                                        damage(ch, victim, 0, gsn_web, FALSE);
                                        return;
                                }
                        }
                }

        }
}


void do_morph_spider (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *mandibles;

        if (to_form)
        {
                /* Remove any arm or tail trauma, spider form doesn't have arms or a tail */

                affect_strip(ch, gsn_arm_trauma);
                affect_strip(ch, gsn_tail_trauma);

                mandibles = create_object(get_obj_index(OBJ_SPIDER_MANDIBLES), ch->level, "common", CREATED_NO_RANDOMISER);
                if (mandibles)
                {
                        obj_to_char(mandibles, ch);
                        form_equip_char(ch, mandibles, WEAR_WIELD);
                }
        }
        else
        {
                mandibles = get_obj_wear(ch, "mandibles");
                if (mandibles)
                {
                        unequip_char(ch, mandibles);
                        extract_obj(mandibles);
                }
        }
}


void do_forage(CHAR_DATA *ch,  char *argument)
{
        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_forage) && ch->race != RACE_WILD_ELF)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->class == CLASS_SHAPE_SHIFTER && ch->form != FORM_BEAR && ch->race != RACE_WILD_ELF)
        {
                send_to_char("You are not in the correct form.\n\r",ch);
                return;
        }

        if (ch->in_room->sector_type != SECT_FIELD
            && ch->in_room->sector_type != SECT_FOREST
            && ch->in_room->sector_type != SECT_DESERT
            && ch->in_room->sector_type != SECT_MOUNTAIN
            && ch->in_room->sector_type != SECT_SWAMP
            && ch->in_room->sector_type != SECT_HILLS)
        {
                send_to_char("Not in this type of terrain.\n\r",ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_forage].beats);

        if (number_percent() > ch->pcdata->learned[gsn_forage])
        {
                send_to_char("You forage around for food but find nothing.\n\r", ch);
                return;
        }

        send_to_char("You forage about and find some food to eat.\n\r", ch);
        ch->pcdata->condition[COND_FULL] += UMIN(30, MAX_FOOD - ch->pcdata->condition[COND_FULL]);
}


void do_morph_bear (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *claws;

        if (to_form)
        {
                /* Remove any arm or tail trauma, bear form doesn't have arms or a tail */
                affect_strip(ch, gsn_arm_trauma);
                affect_strip(ch, gsn_tail_trauma);

                claws = create_object(get_obj_index(OBJ_BEAR_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                if (claws)
                {
                        claws->value[1] *= 1.5;
                        claws->value[2] *= 1.5;
                        obj_to_char(claws, ch);
                        form_equip_char(ch, claws, WEAR_WIELD);
                }
        }
        else
        {

                /*
                 * form_equipment_update has already been called before coming here; claws are probably removed
                 *
                 * if things aren't working, don't try and fix here
                 */

                claws = get_obj_wear(ch, "sftclaws");
                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }
        }
}


void do_bite (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
                return;

        if (!CAN_DO(ch, gsn_bite)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
        {
                send_to_char("What do you think you are, a tiger?\n\r", ch);
                return;
        }

        if (!(ch->form == FORM_TIGER)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You are not fighting anyone.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your teeth are too damaged to bite effectively with.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_bite].beats);

        send_to_char("You attempt to sink your fangs into your victim.\n\r", ch);

        if (number_percent() < ch->pcdata->learned[gsn_bite])
        {
                arena_commentary("$n bites $N.", ch, victim);
                damage(ch,victim,number_range(ch->level*3, ch->level*5), gsn_bite, FALSE);
        }
        else
                damage (ch, victim, 0, gsn_bite, FALSE);
}

void do_crush (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_crush))
        {
                send_to_char("What do you think you are, a bear?\n\r", ch);
                return;
        }

        if (!(ch->form == FORM_BEAR))
        {
                send_to_char("You are not in the correct form.\n\r", ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You are not fighting anyone.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
                send_to_char("Your legs are too damaged to crush anyone with.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_crush].beats);

        send_to_char("{WYou wrap your arms around your victim and attempt to crush them!{x\n\r", ch);

        if (number_percent() < ch->pcdata->learned[gsn_crush])
        {
                AFFECT_DATA af;
                arena_commentary("$c crushes $N in $s powerful arms.", ch, victim);
                act ("$c crushes $N in $s powerful arms!\n\r", ch, NULL, victim, TO_NOTVICT);
                damage(ch,victim,number_range(ch->level*3, ch->level*4), gsn_crush, FALSE);
                af.type         = gsn_crush;
                af.duration     = 1;
                af.location     = APPLY_HITROLL;
                af.modifier     = -4;
                af.bitvector    = AFF_HOLD;
                affect_to_char(victim, &af);
        }
        else
                damage (ch, victim, 0, gsn_crush, FALSE);
}

void do_wolfbite (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA       *victim;
        char            arg [MAX_INPUT_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_wolfbite))
        {
                send_to_char("What do you think you are, a wolf?\n\r", ch);
                return;
        }

        if (!(ch->form == FORM_WOLF || ch->form == FORM_DIREWOLF))
        {
                send_to_char("You are not in the correct form.\n\r",ch);
                return;
        }

        one_argument(argument, arg);

        if (!(victim = ch->fighting))
        {
                if (arg[0] == '\0')
                {
                        send_to_char("Wolfbite whom?\n\r", ch);
                        return;
                }

                if (!(victim = get_char_room(ch, arg)))
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }
        }

        if (is_safe (ch, victim))
                return;

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("Your fangs are too damaged to bite with.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_wolfbite].beats);

        send_to_char("You attempt to sink your fangs into your victim.\n\r", ch);

        if (number_percent() < ch->pcdata->learned[gsn_wolfbite])
        {
                arena_commentary("$n bites $N.", ch, victim);
                damage(ch, victim, number_range(ch->level*3, ch->level*5), gsn_wolfbite, FALSE);
        }
        else
                damage (ch, victim, 0, gsn_wolfbite, FALSE);
}


void do_maul (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        int percent;
        int count;

        if (IS_NPC(ch)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
                return;

        if (!CAN_DO(ch, gsn_maul)
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ))
        {
                send_to_char("What do you think you are, a tiger?\n\r", ch);
                return;
        }

        if ( (!(ch->form == FORM_TIGER || ch->form == FORM_GRIFFIN) )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_lieutenant") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_baron") )
        && !( ch->spec_fun == spec_lookup("spec_sahuagin_prince") ) )
        {
                send_to_char("You are not in the correct form.\n\r",ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You are not fighting anyone.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_LEG_TRAUMA))
        {
                send_to_char("Your claws are too damaged to maul effectively.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_maul].beats);
        arena_commentary("$n mauls $N.", ch, victim);
        percent = ch->pcdata->learned[gsn_maul];
        count = 0;

        while (!count || number_percent() < percent)
        {
                damage(ch, victim, number_range(ch->level * 2, ch->level * 3), gsn_maul, FALSE);

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (++count == 8)
                        return;

                percent -= 7;
        }
}


void do_ravage(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        int percent;
        int count;

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_ravage))
        {
                send_to_char("What do you think you are, a wolf?\n\r", ch);
                return;
        }

        if (!(ch->form == FORM_WOLF || ch->form == FORM_DIREWOLF))
        {
                send_to_char("You are not in the correct form.\n\r",ch);
                return;
        }

        if (!(victim = ch->fighting))
        {
                send_to_char("You are not fighting anyone.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_ravage].beats);
        arena_commentary("$n ravages $N.", ch, victim);

        percent = ch->pcdata->learned[gsn_ravage];
        count = 0;

        while (!count || number_percent() < percent)
        {
                damage(ch, victim, number_range(ch->level * 2, ch->level * 3), gsn_ravage, FALSE);

                if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                        return;

                if (++count == 8)
                        return;

                percent-= 7;
        }
}


void do_morph_tiger (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *fangs;
        OBJ_DATA *claws;
        AFFECT_DATA *paf;

        if (to_form)
        {
                /* Remove any arm trauma, tiger form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                claws = create_object(get_obj_index(OBJ_TIGER_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                fangs = create_object(get_obj_index(OBJ_TIGER_FANGS), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level/3;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                obj_to_char(claws, ch);
                form_equip_char(ch, claws, WEAR_DUAL);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = fangs->affected;
                fangs->affected = paf;

                obj_to_char(fangs, ch);
                form_equip_char(ch, fangs, WEAR_WIELD);
        }
        else
        {
                claws = get_obj_wear(ch, "sftclaws");
                fangs = get_obj_wear(ch, "sftfangs");

                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }
                if (fangs)
                {
                        unequip_char(ch, fangs);
                        extract_obj(fangs);
                }
        }
}


void do_morph_wolf (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *fangs;
        OBJ_DATA *claws;
        AFFECT_DATA *paf;
        AFFECT_DATA af;

        if (to_form)
        {
                /* Wolf form has no arms */
                affect_strip(ch, gsn_arm_trauma);

                claws = create_object(get_obj_index(OBJ_TIGER_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                fangs = create_object(get_obj_index(OBJ_TIGER_FANGS), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level/3;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                obj_to_char(claws, ch);
                form_equip_char(ch, claws, WEAR_DUAL);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = fangs->affected;
                fangs->affected = paf;

                obj_to_char(fangs, ch);
                form_equip_char(ch, fangs, WEAR_WIELD);

                affect_strip(ch, gsn_infravision);
                send_to_char("Your new form causes your eyes to glow red.\n\r", ch);

                af.type      = gsn_infravision;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_INFRARED;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_detect_hidden);
                send_to_char("Your new form causes your awareness to improve.\n\r", ch);

                af.type      = gsn_detect_hidden;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_DETECT_HIDDEN;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_detect_invis);
                send_to_char("Your new form causes your senses to rocket.\n\r", ch);

                af.type      = gsn_detect_invis;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_DETECT_INVIS;
                affect_to_char(ch, &af);

                af.type      = gsn_detect_sneak;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_DETECT_SNEAK;
                affect_to_char(ch, &af);
        }
        else
        {
                claws = get_obj_wear(ch, "sftclaws");
                fangs = get_obj_wear(ch, "sftfangs");

                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }
                if (fangs)
                {
                        unequip_char(ch, fangs);
                        extract_obj(fangs);
                }

                affect_strip(ch, gsn_infravision);
                affect_strip(ch, gsn_detect_hidden);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
                affect_strip(ch, gsn_detect_invis);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_INVIS);
                affect_strip(ch, gsn_detect_sneak);
                affect_strip(ch, gsn_heighten);
                affect_strip(ch, gsn_form_direwolf);
                affect_strip(ch, gsn_gaias_warning);
                REMOVE_BIT(ch->affected_by, AFF_BATTLE_AURA);
        }
}


void do_morph_direwolf (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *fangs;
        OBJ_DATA *claws;
        AFFECT_DATA *paf;
        AFFECT_DATA af;

        if (to_form)
        {
                /* Dire wolf form has no arms */
                affect_strip(ch, gsn_arm_trauma);

                claws = create_object(get_obj_index(OBJ_TIGER_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                fangs = create_object(get_obj_index(OBJ_TIGER_FANGS), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                obj_to_char(claws, ch);
                form_equip_char(ch, claws, WEAR_DUAL);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = fangs->affected;
                fangs->affected = paf;

                obj_to_char(fangs, ch);
                form_equip_char(ch, fangs, WEAR_WIELD);

                affect_strip(ch, gsn_infravision);
                send_to_char("Your new form causes your eyes to glow red.\n\r", ch);

                af.type      = gsn_infravision;
                af.duration  = -1;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_INFRARED;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_detect_hidden);
                send_to_char("Your new form causes your awareness to improve.\n\r", ch);

                af.type      = gsn_detect_hidden;
                af.bitvector = AFF_DETECT_HIDDEN;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_detect_invis);
                send_to_char("Your new form causes your senses to rocket.\n\r", ch);

                af.type      = gsn_detect_invis;
                af.bitvector = AFF_DETECT_INVIS;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_detect_sneak);
                af.type      = gsn_detect_sneak;
                af.bitvector = AFF_DETECT_SNEAK;
                affect_to_char(ch, &af);

                affect_strip(ch, gsn_form_direwolf);
                send_to_char("You are surrounded by a terrifying aura!\n\r", ch);
                af.type      = gsn_form_direwolf;
                af.bitvector = AFF_BATTLE_AURA;
                affect_to_char(ch, &af);

        }
        else
        {
                claws = get_obj_wear(ch, "sftclaws");
                fangs = get_obj_wear(ch, "sftfangs");

                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }
                if (fangs)
                {
                        unequip_char(ch, fangs);
                        extract_obj(fangs);
                }

                affect_strip(ch, gsn_infravision);
                affect_strip(ch, gsn_detect_hidden);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_HIDDEN);
                affect_strip(ch, gsn_detect_invis);
                REMOVE_BIT(ch->affected_by, AFF_DETECT_INVIS);
                affect_strip(ch, gsn_detect_sneak);
                affect_strip(ch, gsn_heighten);
                affect_strip(ch, gsn_form_direwolf);
                affect_strip(ch, gsn_gaias_warning);
                REMOVE_BIT(ch->affected_by, AFF_BATTLE_AURA);
        }
}


void do_morph_hydra (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *teeth;

        if (to_form)
        {
                /* Remove any arm trauma, hydra form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                teeth = create_object(get_obj_index(OBJ_HYDRA_TEETH), ch->level, "common", CREATED_NO_RANDOMISER);
                teeth->value[1] *= 2;
                teeth->value[2] *= 2;

                obj_to_char(teeth, ch);
                form_equip_char(ch, teeth, WEAR_WIELD);

                if (ch->pcdata->learned[gsn_form_hydra] > 50)
                        send_to_char("You can now breathe colour spray!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_hydra] > 75)
                        send_to_char("You can now breathe fireballs!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_hydra] > 85)
                        send_to_char("You can now breathe acid blasts!\n\r",ch);
        }
        else
        {
                teeth = get_obj_wear(ch, "teeth");
                if (teeth)
                {
                        unequip_char(ch, teeth);
                        extract_obj(teeth);
                }
        }
}


void do_morph_dragon (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;
        OBJ_DATA *fangs;
        OBJ_DATA *claws;
        AFFECT_DATA *paf;

        if (to_form)
        {
                /* Remove any arm trauma, dragon form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                if (ch->pcdata->learned[gsn_form_dragon] > 60
                    || ch->pcdata->learned[gsn_fly])
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);
                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                }

                if (ch->pcdata->learned[gsn_form_dragon] > 75)
                        send_to_char("You can now breathe lightning!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 80)
                        send_to_char("You can now breathe frost!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 83)
                        send_to_char("You can now breathe steam!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 85)
                        send_to_char("You can now breathe fire!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 90)
                        send_to_char("You can now breathe acid!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 95)
                        send_to_char("You can now breathe gas!\n\r",ch);

                if (ch->pcdata->learned[gsn_form_dragon] > 90)
                {
                        send_to_char ("You are surrounded by a terrifying aura!\n\r", ch);
                        affect_strip (ch, gsn_dragon_aura);
                        affect_strip (ch, gsn_dragon_shield);

                        af.type         = gsn_dragon_aura;
                        af.duration     = -1;
                        af.modifier     = - ch->level;
                        af.bitvector    = 0;
                        af.location     = APPLY_AC;
                        affect_to_char (ch, &af);

                        af.type         = gsn_dragon_aura;
                        af.duration     = -1;
                        af.modifier     = ch->level / 8;
                        af.bitvector    = 0;
                        af.location     = APPLY_HITROLL;
                        affect_to_char (ch, &af);

                        af.type         = gsn_dragon_aura;
                        af.duration     = -1;
                        af.modifier     = 0;
                        af.bitvector    = AFF_BATTLE_AURA;
                        af.location     = APPLY_NONE;
                        affect_to_char (ch, &af);

                        af.type         = gsn_dragon_shield;
                        af.duration     = -1;
                        af.modifier     = 0;
                        af.bitvector    = 0;
                        af.location     = APPLY_NONE;
                        affect_to_char( ch, &af );
                }

                claws = create_object(get_obj_index(OBJ_DRAGON_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                fangs = create_object(get_obj_index(OBJ_DRAGON_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level / 3;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 3;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                obj_to_char(claws, ch);
                form_equip_char(ch, claws, WEAR_WIELD);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = fangs->affected;
                fangs->affected = paf;

                obj_to_char(fangs, ch);
                form_equip_char(ch, fangs, WEAR_DUAL);
        }
        else
        {
                affect_strip(ch, gsn_fly);
                affect_strip(ch, gsn_levitation);
                affect_strip(ch, gsn_dragon_aura);
                affect_strip (ch, gsn_dragon_shield);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);

                claws = get_obj_wear(ch, "sftclaws");
                fangs = get_obj_wear(ch, "sftfangs");

                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }

                if (fangs)
                {
                        unequip_char(ch, fangs);
                        extract_obj(fangs);
                }
        }
}


void do_morph_phoenix (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;
        OBJ_DATA *beak;

        if (to_form)
        {
                /* Remove any arm trauma, phoenix form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                beak = create_object(get_obj_index(OBJ_PHOENIX_BEAK), ch->level, "common", CREATED_NO_RANDOMISER);
                if (beak)
                {
                        beak->value[1] *= 1.5;
                        beak->value[2] *= 1.5;
                        obj_to_char(beak, ch);
                        form_equip_char(ch, beak, WEAR_WIELD);
                }

                if ((ch->pcdata->learned[gsn_form_phoenix] > 40)
                    || (ch->pcdata->learned[gsn_fly]))
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                }

                if (ch->pcdata->learned[gsn_form_phoenix] > 80)
                {
                        affect_strip(ch, gsn_fireshield);
                        send_to_char("The air around your form bursts into flame!\n\r", ch);

                        af.type      = gsn_fireshield;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLAMING;
                        affect_to_char(ch, &af);

                        affect_strip(ch, gsn_resist_heat);
                        send_to_char("You resist heat and flame!\n\r", ch);

                        af.type  = gsn_resist_heat;
                        af.bitvector = 0;
                        affect_to_char(ch, &af);
                }

                if (ch->pcdata->learned[gsn_form_phoenix] > 95)
                {
                        affect_strip(ch, gsn_globe);
                        send_to_char("A scintillating globe forms around you!\n\r", ch);

                        af.type      = gsn_globe;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_GLOBE;
                        affect_to_char(ch, &af);
                }
        }
        else
        {
                affect_strip(ch, gsn_fly);
                affect_strip(ch, gsn_levitation);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);
                affect_strip(ch,gsn_fireshield);
                REMOVE_BIT(ch->affected_by, AFF_FLAMING);
                affect_strip(ch,gsn_globe);
                REMOVE_BIT(ch->affected_by, AFF_GLOBE);
                affect_strip(ch, gsn_resist_heat);

                beak = get_obj_wear(ch, "sftbeak");
                if (beak)
                {
                        unequip_char(ch, beak);
                        extract_obj(beak);
                }
        }
}


void do_morph_fly (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;
        AFFECT_DATA *paf;

        if (to_form)
        {

                /* Remove any arm or tail trauma, fly form doesn't have arms or a tail */
                affect_strip(ch, gsn_arm_trauma);
                affect_strip(ch, gsn_tail_trauma);

                for ( paf = ch->affected; paf; paf = paf->next )
                {

                        if ( paf->deleted )
                                continue;

                        if (paf->duration < 0 )
                                continue;

                        if (effect_is_prayer(paf))
                                continue;

                        affect_remove( ch, paf );
                }

                if ((ch->pcdata->learned[gsn_form_fly] > 10) || (ch->pcdata->learned[gsn_fly]))
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);

                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_form_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);

                        af.bitvector = AFF_NON_CORPOREAL;
                        affect_to_char(ch, &af);
                }
        }
        else
        {
                affect_strip(ch, gsn_form_fly);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);
                REMOVE_BIT(ch->affected_by, AFF_NON_CORPOREAL);
        }
}

void do_morph_bat (CHAR_DATA *ch, bool to_form)
{
        OBJ_DATA *fangs;
        AFFECT_DATA af;
        AFFECT_DATA *paf;

        if (to_form)
        {
                /* Remove any arm trauma, bat form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                fangs = create_object(get_obj_index(OBJ_BAT_FANGS), ch->level, "common", CREATED_NO_RANDOMISER);
                obj_to_char(fangs, ch);
                form_equip_char(ch, fangs, WEAR_WIELD);

                for ( paf = ch->affected; paf; paf = paf->next )
                {

                        if ( paf->deleted )
                                continue;

                        if (paf->duration < 0 )
                                continue;

                        if (effect_is_prayer(paf))
                                continue;

                        affect_remove( ch, paf );
                }

                if ((ch->pcdata->learned[gsn_form_bat]) || (ch->pcdata->learned[gsn_fly]))
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);
                        affect_strip(ch, gsn_mist_walk);
                        REMOVE_BIT(ch->affected_by, AFF_NON_CORPOREAL);

                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_form_bat;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                        ch->rage = ( ch->rage - ( ch->level / 10 ) );
                }
        }
        else
        {
                affect_strip(ch, gsn_form_bat);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);

                fangs = get_obj_wear(ch, "fangs");
                if (fangs)
                {
                        unequip_char(ch, fangs);
                        extract_obj(fangs);
                }
        }
}


void do_morph_demon (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;

        if (to_form)
        {
                /* Remove any tail trauma, demon form does not have a tail */
                affect_strip(ch, gsn_tail_trauma);

                if (ch->pcdata->learned[gsn_form_demon] > 10 || ch->pcdata->learned[gsn_fly])
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);

                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                }
        }
        else
        {
                affect_strip(ch, gsn_fly);
                affect_strip(ch, gsn_levitation);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);
        }
}


void do_morph_griffin (CHAR_DATA *ch, bool to_form)
{
        AFFECT_DATA af;
        OBJ_DATA *claw;
        OBJ_DATA *claws;
        AFFECT_DATA *paf;

        if (to_form)
        {

                /* Remove any arm trauma, griffin form doesn't have arms */
                affect_strip(ch, gsn_arm_trauma);

                if (ch->pcdata->learned[gsn_form_fly] > 10 || ch->pcdata->learned[gsn_fly])
                {
                        affect_strip(ch, gsn_fly);
                        affect_strip(ch, gsn_levitation);
                        REMOVE_BIT(ch->affected_by, AFF_FLYING);

                        send_to_char("Your new form enables you to fly.\n\r", ch);

                        af.type      = gsn_fly;
                        af.duration  = -1;
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_FLYING;
                        affect_to_char(ch, &af);
                }

                claws = create_object(get_obj_index(OBJ_TIGER_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);
                claw = create_object(get_obj_index(OBJ_TIGER_CLAWS), ch->level, "common", CREATED_NO_RANDOMISER);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_HITROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level/2;
                paf->bitvector = 0;
                paf->next = claws->affected;
                claws->affected = paf;

                obj_to_char(claws, ch);
                form_equip_char(ch, claws, WEAR_DUAL);

                if (!affect_free)
                        paf = alloc_perm(sizeof(*paf));
                else
                {
                        paf = affect_free;
                        affect_free = affect_free->next;
                }

                paf->type = gsn_morph;
                paf->duration = -1;
                paf->location = APPLY_DAMROLL;
                paf->modifier = ch->level / 2;
                paf->bitvector = 0;
                paf->next = claw->affected;
                claw->affected = paf;

                obj_to_char(claw, ch);
                form_equip_char(ch, claw, WEAR_WIELD);
        }
        else
        {
                affect_strip(ch, gsn_fly);
                affect_strip(ch, gsn_levitation);
                REMOVE_BIT(ch->affected_by, AFF_FLYING);

                claws = get_obj_wear(ch, "sftclaws");
                claw = get_obj_wear(ch, "sftclaw");
                if (claws)
                {
                        unequip_char(ch, claws);
                        extract_obj(claws);
                }

                if (claw)
                {
                        unequip_char(ch, claw);
                        extract_obj(claw);
                }
        }
}


void do_morph (CHAR_DATA *ch, char *argument)
{
        int sn;
        int form;
        int cost;
        int old_form;
        char buf[MAX_STRING_LENGTH];
        char buf1[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_morph))
        {
                send_to_char("Huh?  What?\n\r", ch);
                return;
        }

        if (is_affected(ch, gsn_astral_sidestep))
        {
                send_to_char("You cannot do that in your current form.\n\r",ch);
                return;
        }

        if (argument[0] == '\0')
        {
                argument = "normal";
                /*
                 * send_to_char("Morph to which form?\n\r", ch);
                 * return;
                 *
                 * Made it so no argument->normal form. Uncomment if
                 * original behaviour desired.  --Owl 14/8/22
                 */

        }

        old_form = ch->form;
        form = extra_form_int(argument);

        if (form == -1)
        {
                send_to_char("Which form was that?\n\r", ch);
                return;
        }

        if( ch->sub_class == SUB_CLASS_VAMPIRE
        &&  form != FORM_NORMAL
        &&  form != FORM_BAT )
        {
                send_to_char( "You can no longer change into that form.\n\r", ch );
                return;
        }

        if (ch->sub_class == SUB_CLASS_WEREWOLF)
        {
                if (form != FORM_NORMAL && form != FORM_WOLF)
                {
                        send_to_char ("You may only morph between wolf and normal forms.\n\r", ch);
                        return;
                }

                if (IS_FULL_MOON && form == FORM_NORMAL)
                {
                        send_to_char("Not while the moon is full.\n\r", ch);
                        return;
                }
        }

        if (form == ch->form && !is_affected(ch, gsn_mist_walk))
        {
                send_to_char("You are already in that form.\n\r", ch);
                return;
        }

        strcpy(buf, extra_form_name(form));
        strcat(buf, " form");

        sn = skill_lookup(buf);

        if ((sn == -1) && (form != 0))
        {
                send_to_char("That form is not possible.\n\r", ch);
                return;
        }

        if ((ch->pcdata->learned[sn] < 20) && (form != 0))
        {
                send_to_char("You don't know that form well enough to morph into it.\n\r", ch);
                return;
        }

        if (form != FORM_NORMAL)
                cost =  UMAX (skill_table[sn].min_mana, 60 - ch->pcdata->learned[sn]);
        else
                cost = 10;

        if (ch->mana - cost < 0)
        {
                send_to_char("You do not have enough mana.\n\r",ch);
                return;
        }

        if ( ch->sub_class == SUB_CLASS_VAMPIRE
        &&   form == FORM_BAT
        &&   ch->rage < (ch->level / 10) )
        {
                send_to_char("<88><556><558>You are too blood-starved to morph into bat form.<559><557><0>\n\r", ch);
                return;
        }

        if (form == FORM_FLY)
        {
                if (ch->pcdata->group_leader == ch)
                {
                        send_to_char("You can't lead a group in a non-corporeal form.\n\r", ch);
                        return;
                }

                if (ch->fighting)
                {
                        send_to_char("You may not morph into a fly while fighting.\n\r", ch);
                        return;
                }

                if (is_entered_in_tournament(ch)
                    && tournament_status == TOURNAMENT_STATUS_RUNNING
                    && is_still_alive_in_tournament(ch))
                {
                        send_to_char("Not during the tournament!\n\r", ch);
                        return;
                }
        }

        ch->mana -= cost;
        WAIT_STATE(ch, skill_table[gsn_morph].beats);

        if (form == FORM_WOLF
            && !IS_NPC(ch)
            && ch->pcdata->learned[gsn_form_wolf] > 95
            && ch->level > 59)
        {
                form = FORM_DIREWOLF;
        }

        /* Fix Shade - did in prod direct might not work */
        ch->form = form;

        form_equipment_update(ch);

        if (is_affected(ch, gsn_mist_walk))
            sprintf(buf, "\n\rYou morph from mist form");
        else
            sprintf(buf, "\n\rYou morph from %s", extra_form_name(old_form));
        sprintf(buf1, " to %s form.\n\r", extra_form_name(form));
        strcat(buf, buf1);

        send_to_char(buf, ch);

        if (form != FORM_NORMAL)
        {
                sprintf(buf, "$n shimmers and morphs into a %s.\n\r",
                        extra_form_name(form));
                act (buf, ch, NULL, NULL, TO_ROOM);
        }
        else {
                if (is_affected(ch, gsn_mist_walk))
                {
                    send_to_char("Your body reverts to its normal state.\n\r",ch);
                    act("A cloud of glowing mist withdraws to unveil $n!",ch,NULL,NULL,TO_ROOM);
                    affect_strip(ch,gsn_mist_walk);
                    WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

                }
                else {
                    act ("$n shimmers and returns to normal.\n\r", ch, NULL, NULL, TO_ROOM);
                }
        }

        switch (old_form)
        {
            case FORM_CHAMELEON:
                do_morph_chameleon(ch, FALSE);
                break;

            case FORM_HAWK:
                do_morph_hawk(ch, FALSE);
                break;

            case FORM_CAT:
                do_morph_cat(ch, FALSE);
                break;

            case FORM_SNAKE:
                do_morph_snake(ch, FALSE);
                break;

            case FORM_SCORPION:
                do_morph_scorpion(ch, FALSE);
                break;

            case FORM_SPIDER:
                do_morph_spider(ch, FALSE);
                break;

            case FORM_BEAR:
                do_morph_bear(ch, FALSE);
                break;

            case FORM_TIGER:
                do_morph_tiger(ch, FALSE);
                break;

            case FORM_WOLF:
                do_morph_wolf(ch, FALSE);
                break;

            case FORM_DIREWOLF:
                do_morph_direwolf(ch, FALSE);
                break;

            case FORM_HYDRA:
                do_morph_hydra(ch, FALSE);
                break;

            case FORM_DRAGON:
                do_morph_dragon(ch, FALSE);
                break;

            case FORM_PHOENIX:
                do_morph_phoenix(ch, FALSE);
                break;

            case FORM_FLY:
                do_morph_fly(ch, FALSE);
                break;

            case FORM_GRIFFIN:
                do_morph_griffin(ch, FALSE);
                break;

            case FORM_DEMON:
                do_morph_demon(ch, FALSE);
                break;

            case FORM_BAT:
                do_morph_bat(ch, FALSE);
                break;
        }

        switch (form)
        {
            case FORM_CHAMELEON:
                do_morph_chameleon(ch, TRUE);
                break;

            case FORM_HAWK:
                do_morph_hawk(ch, TRUE);
                break;

            case FORM_CAT:
                do_morph_cat(ch, TRUE);
                break;

            case FORM_SNAKE:
                do_morph_snake(ch, TRUE);
                break;

            case FORM_SCORPION:
                do_morph_scorpion(ch, TRUE);
                break;

            case FORM_SPIDER:
                do_morph_spider(ch, TRUE);
                break;

            case FORM_BEAR:
                do_morph_bear(ch, TRUE);
                break;

            case FORM_TIGER:
                do_morph_tiger(ch, TRUE);
                break;

            case FORM_WOLF:
                do_morph_wolf(ch, TRUE);
                break;

            case FORM_DIREWOLF:
                do_morph_direwolf(ch, TRUE);
                break;

            case FORM_HYDRA:
                do_morph_hydra(ch, TRUE);
                break;

            case FORM_DRAGON:
                do_morph_dragon(ch, TRUE);
                break;

            case FORM_PHOENIX:
                do_morph_phoenix(ch, TRUE);
                break;

            case FORM_FLY:
                do_morph_fly(ch, TRUE);
                break;

            case FORM_GRIFFIN:
                do_morph_griffin(ch, TRUE);
                break;

            case FORM_DEMON:
                do_morph_demon(ch, TRUE);
                break;

            case FORM_BAT:
                do_morph_bat(ch, TRUE);
                break;

        }
}

/* EOF sft.c */
