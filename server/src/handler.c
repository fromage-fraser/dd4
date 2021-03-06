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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


AFFECT_DATA *affect_free;
void affect_modify (CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, OBJ_DATA *weapon);


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
	if ( ch->desc && ch->desc->original )
		ch = ch->desc->original;

	if ( ch->trust != 0 )
		return ch->trust;

	if ( IS_NPC( ch ) && ch->level >= LEVEL_HERO )
		return LEVEL_HERO - 1;
	else
		return ch->level;
}


/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
	return 17 + ( ch->played + (int) ( current_time - ch->logon ) ) / 14400;
	/* 14400 assumes 30 second hours, 24 hours a day, 20 day - Kahn */
}


/*
 * Retrieve character's current stats........ - geoff.
 */

int get_curr_str(CHAR_DATA *ch)
{
	int max;

	if (IS_NPC(ch))
		return 13;

	max = 25 + race_table[ch->race].str_bonus +  class_table[ch->class].class_stats[0];
	return URANGE(3, ch->pcdata->perm_str + ch->pcdata->mod_str, max);
}


int get_curr_int(CHAR_DATA *ch)
{
	int max;

	if (IS_NPC(ch))
		return 13;

	max = 25 + race_table[ch->race].int_bonus +  class_table[ch->class].class_stats[1];
	return URANGE(3, ch->pcdata->perm_int + ch->pcdata->mod_int, max);
}


int get_curr_wis(CHAR_DATA *ch)
{
	int max;

	if (IS_NPC(ch))
		return 13;

	max = 25 + race_table[ch->race].wis_bonus +  class_table[ch->class].class_stats[2];
	return URANGE(3, ch->pcdata->perm_wis + ch->pcdata->mod_wis, max);
}


int get_curr_dex(CHAR_DATA *ch)
{
	int max;

	if (IS_NPC(ch))
		return 13;

	max = 25 + race_table[ch->race].dex_bonus + class_table[ch->class].class_stats[3];
	return URANGE(3, ch->pcdata->perm_dex + ch->pcdata->mod_dex, max);
}


int get_curr_con(CHAR_DATA *ch)
{
	int max;

	if (IS_NPC(ch))
		return 13;

	max = 25 + race_table[ch->race].con_bonus + class_table[ch->class].class_stats[4];
	return URANGE(3, ch->pcdata->perm_con + ch->pcdata->mod_con, max);
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
	if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
		return 1000;

	if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
		return 0;

	return MAX_WEAR + 2 * get_curr_dex( ch ) / 2;
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
	if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
		return 1000000;

	if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
		return 0;

	return str_app[get_curr_str( ch )].carry;
}


/*
 * See if a string is one of the names of an object.
 * New is_name sent in by Alander.
 */
bool is_name( const char *str, char *namelist )
{
	char name [ MAX_INPUT_LENGTH ];

	for ( ; ; )
	{
		namelist = one_argument( namelist, name );
		if ( name[0] == '\0' )
			return FALSE;
		if ( !str_prefix( str, name ) )
			return TRUE;
	}
}


/*
 * Match multiple keywords against a name list
 * Gezhp 2000
 */
bool multi_keyword_match (char *keys, char *namelist)
{
	char buf [MAX_INPUT_LENGTH];
	bool match = FALSE;

	for (;;)
	{
		keys = one_argument (keys, buf);
		if (buf[0] == '\0')
			break;
		if (!is_name (buf, namelist))
			return FALSE;
		match = TRUE;
	}

	return match;
}


bool is_full_name( const char *str, char *namelist )
{
	char name [ MAX_INPUT_LENGTH ];

	for ( ; ; )
	{
		namelist = one_argument( namelist, name );
		if ( name[0] == '\0' )
			return FALSE;
		if ( !str_cmp( str, name ) )
			return TRUE;
	}
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, OBJ_DATA *weapon )
{
	OBJ_DATA *wield;
	char buf [MAX_STRING_LENGTH];
	int mod;
	AFFECT_DATA af;

	mod = paf->modifier;

	if ( fAdd )
	{
		SET_BIT   ( ch->affected_by, paf->bitvector );
	}
	else
	{
		REMOVE_BIT( ch->affected_by, paf->bitvector );
		mod = 0 - mod;
	}

	switch ( paf->location )
	{
	    default:
		sprintf( buf, "Affect_modify: unknown location %d on %s.",
			paf->location, ch->name );
		bug ( buf, 0 );
		return;

	    case APPLY_NONE:
		break;

	    case APPLY_STR:
		if ( !IS_NPC( ch ) )
			ch->pcdata->mod_str += mod;
		break;

	    case APPLY_DEX:
		if ( !IS_NPC( ch ) )
			ch->pcdata->mod_dex += mod;
		break;

	    case APPLY_INT:
		if ( !IS_NPC( ch ) )
			ch->pcdata->mod_int += mod;
		break;

	    case APPLY_WIS:
		if ( !IS_NPC( ch ) )
			ch->pcdata->mod_wis += mod;
		break;

	    case APPLY_CON:
		if ( !IS_NPC( ch ) )
			ch->pcdata->mod_con += mod;
		break;

	    case APPLY_SEX:
		ch->sex += mod;
		break;

	    case APPLY_CLASS:
	    case APPLY_LEVEL:
	    case APPLY_AGE:
	    case APPLY_HEIGHT:
	    case APPLY_WEIGHT:
	    case APPLY_GOLD:
	    case APPLY_EXP:
		break;

	    case APPLY_MANA:
		ch->max_mana += mod;
		break;

	    case APPLY_HIT:
		ch->max_hit += mod;
		break;

	    case APPLY_MOVE:
		ch->max_move += mod;
		break;

	    case APPLY_AC:
		ch->armor += mod;
		break;

	    case APPLY_HITROLL:
		ch->hitroll += mod;
		break;

	    case APPLY_DAMROLL:
		ch->damroll += mod;
		break;

	    case APPLY_SAVING_PARA:
	    case APPLY_SAVING_ROD:
	    case APPLY_SAVING_PETRI:
	    case APPLY_SAVING_BREATH:
	    case APPLY_SAVING_SPELL:
		ch->saving_throw += mod;
		break;

	    case APPLY_SANCTUARY:
		af.type = skill_lookup( "sanctuary" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already in sanctuary.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_SANCTUARY;
			affect_to_char( ch, &af );
			send_to_char( "You are surrounded by a white aura.\n\r", ch );
			act( "$n is surrounded by a white aura.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "The white aura around your body vanishes.\n\r", ch );
			act( "The white aura around $n's body vanishes.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_SNEAK:
		af.type = skill_lookup( "sneak" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already sneaking.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_SNEAK;
			affect_to_char( ch, &af );
			send_to_char( "You can now move amongst the shadows.\n\r", ch );
			act( "$n can now move amongst the shadows.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You emerge from the shadows.\n\r", ch );
			act( "$n emerges from the shadows.", ch, NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_INVIS:
		af.type = skill_lookup( "invis" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already invisible.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_INVISIBLE;
			affect_to_char( ch, &af );
			send_to_char( "You fade out of existence.\n\r", ch );
			act( "$n fades out of existence.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You fade back into existance.\n\r", ch );
			act( "$n fades back into existance body vanishes.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_DETECT_INVIS:
		af.type = skill_lookup( "detect invis" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You can already see the invisible.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_DETECT_INVIS;
			affect_to_char( ch, &af );
			send_to_char( "Your eyes tingle.\n\r", ch );
			act( "$n's eyes tingle.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You no longer see invisible objects.\n\r", ch );
			act( "$n's eyes stop tingling.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_DETECT_HIDDEN:
		af.type = skill_lookup( "detect hidden" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You can already see the hidden.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_DETECT_HIDDEN;
			affect_to_char( ch, &af );
			send_to_char( "Your awareness improves.\n\r", ch );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You feel less aware of your surroundings.\n\r", ch );
			break;
		}

	    case APPLY_FLAMING:
		af.type = skill_lookup( "fireshield" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already surrounded by flames.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_FLAMING;
			affect_to_char( ch, &af );
			send_to_char( "A flaming aura surrounds you!\n\r", ch );
			act( "The air around $n's form bursts into flame.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "The flames around your body fizzle out.\n\r", ch );
			act( "The flames around $n's body fizzle out.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_PROTECT:
		af.type = skill_lookup( "protection" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already protected.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_PROTECT;
			affect_to_char( ch, &af );
			send_to_char( "You feel protected.\n\r", ch );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You feel less protected.\n\r", ch );
			break;
		}

	    case APPLY_FLY:
		af.type = skill_lookup( "fly" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already flying.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_FLYING;
			affect_to_char( ch, &af );
			send_to_char( "Your feet rise off the ground.\n\r", ch );
			act( "$n's feet rise off the ground.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You fall back to earth.\n\r", ch );
			act( "$n slowly floats to the ground.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_PASS_DOOR:
		af.type = skill_lookup( "pass door" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already translucent.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_PASS_DOOR;
			affect_to_char( ch, &af );
			send_to_char( "You fade out of existence.\n\r", ch );
			act( "$n turns translucent.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "You feel solid again.\n\r", ch );
			act( "$n looks solid once more.", ch, NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_GLOBE:
		af.type = skill_lookup( "globe" );
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
			{
				send_to_char( "You are already globed.\n\r", ch );
				break;
			}
			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_GLOBE;
			affect_to_char( ch, &af );
			send_to_char( "You are surrounded by an invulnerable globe.\n\r", ch );
			act( "$n is surrounded by a scintillating globe.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char( "The globe around you implodes.\n\r", ch );
			act( "The globe around $n implodes.", ch,
			    NULL, NULL, TO_ROOM);
			break;
		}

	    case APPLY_RESIST_HEAT:
		af.type = skill_lookup("resist heat");
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
				break;

			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = 0;
			affect_to_char( ch, &af );

			send_to_char( "You feel resistant to heat and flame.\n\r", ch );
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char("You feel vulnerable to heat and flame.\n\r", ch);
		}
		break;

	    case APPLY_RESIST_COLD:
		af.type = skill_lookup("resist cold");
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
				break;

			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = 0;
			affect_to_char( ch, &af );

			send_to_char( "You feel resistant to cold and ice.\n\r", ch );
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char("You feel vulnerable to cold and ice.\n\r", ch);
		}
		break;

	    case APPLY_RESIST_LIGHTNING:
		af.type = skill_lookup("resist lightning");
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
				break;

			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = 0;
			affect_to_char( ch, &af );

			send_to_char( "You feel resistant to electricity.\n\r", ch );
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char("You feel vulnerable to electricity.\n\r", ch);
		}
		break;

	    case APPLY_RESIST_ACID:
		af.type = skill_lookup("resist acid");
		if( fAdd )
		{
			if( is_affected( ch, af.type ) )
				break;

			af.duration = -1;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = 0;
			affect_to_char( ch, &af );

			send_to_char( "You feel resistant to acid.\n\r", ch );
		}
		else
		{
			affect_strip( ch, af.type );
			send_to_char("You feel vulnerable to acid.\n\r", ch);
		}
		break;
	}

	if (IS_NPC(ch))
		return;

	/*
	 * Check for weapon wielding.
	 * Guard against recursion (for weapons with affects).
	 *
	 * OBJ_DATA *weapon is the item (if any) whose effects are being destroyed in
	 * this whole function.  We do NOT want to drop any wielded weapon with the code
	 * below if that weapon is the same as OBJ_DATA *weapon: otherwise effects and
	 * objects are removed TWICE and WE ALL BURN IN HELL.  Gezhp 2001
	 */

	if (((wield = get_eq_char(ch, WEAR_WIELD)) || (wield = get_eq_char(ch, WEAR_DUAL)))
	    && wield != weapon
	    && get_obj_weight(wield) > str_app[get_curr_str(ch)].wield)
	{
		static int depth;

		if (!depth)
		{
			depth++;
			unequip_char(ch, wield);
			obj_from_char(wield);
			act("$p is too heavy for you to hold!\n\rYour weapon slips from your hand.",
			    ch, wield, NULL, TO_CHAR);
			act("$n drops $p.", ch, wield, NULL, TO_ROOM);
			obj_to_room(wield, ch->in_room);
			depth--;
		}
	}
}


/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
	AFFECT_DATA *paf_new;

	if ( !affect_free )
	{
		paf_new		= alloc_perm( sizeof( *paf_new ) );
	}
	else
	{
		paf_new		= affect_free;
		affect_free	= affect_free->next;
	}

	*paf_new = *paf;
	paf_new->deleted = FALSE;
	paf_new->next = ch->affected;
	ch->affected = paf_new;

	affect_modify( ch, paf_new, TRUE, NULL );
	return;
}


/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
	if ( !ch->affected )
	{
		bug( "Affect_remove: no affect.", 0 );
		return;
	}

	affect_modify( ch, paf, FALSE, NULL );

	paf->deleted = TRUE;

	return;
}


/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
	AFFECT_DATA *paf;

	for ( paf = ch->affected; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;
		if ( paf->type == sn )
			affect_remove( ch, paf );
	}

	return;
}


/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
	AFFECT_DATA *paf;

	for ( paf = ch->affected; paf; paf = paf->next )
	{
		if ( paf->deleted )
			continue;

		if ( paf->type == sn )
			return TRUE;
	}

	return FALSE;
}


/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
	AFFECT_DATA *paf_old;

	for ( paf_old = ch->affected; paf_old; paf_old = paf_old->next )
	{
		if ( paf_old->deleted )
			continue;

		if ( paf_old->type == paf->type )
		{
			paf->duration += paf_old->duration;
			paf->modifier += paf_old->modifier;
			affect_remove( ch, paf_old );
			break;
		}
	}

	affect_to_char( ch, paf );
	return;
}


/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	if ( !ch->in_room )
	{
		bug( "Char_from_room: NULL.", 0 );
		return;
	}

	if ( !IS_NPC( ch ) )
		--ch->in_room->area->nplayer;

	if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	    && obj->item_type == ITEM_LIGHT
	    && obj->value[2] != 0
	    && (!IS_NPC(ch) && ch->desc && ch->desc->connected != CON_GET_OLD_PASSWORD)
	    && ch->in_room->light > 0 )
	{
		--ch->in_room->light;
	}

	if ( ch == ch->in_room->people )
	{
		ch->in_room->people = ch->next_in_room;
	}
	else
	{
		CHAR_DATA *prev;

		for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
		{
			if ( prev->next_in_room == ch )
			{
				prev->next_in_room = ch->next_in_room;
				break;
			}
		}

		if ( !prev )
			bug( "Char_from_room: ch not found.", 0 );
	}

	ch->in_room      = NULL;
	ch->next_in_room = NULL;
	return;
}


/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
	OBJ_DATA *obj;

	if ( !pRoomIndex )
	{
		bug( "Char_to_room: NULL.", 0 );
		return;
	}

	ch->in_room		= pRoomIndex;
	ch->next_in_room	= pRoomIndex->people;
	pRoomIndex->people	= ch;

	if (!IS_NPC(ch))
	{
		++ch->in_room->area->nplayer;

		if (ch->in_room->sector_type != SECT_UNDERWATER)
			ch->pcdata->air_supply = FULL_AIR_SUPPLY;
	}

	if ((obj = get_eq_char(ch, WEAR_LIGHT))
	    && obj->item_type == ITEM_LIGHT
	    && obj->value[2] != 0 )
		++ch->in_room->light;

	return;
}


/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
	obj->next_content = ch->carrying;
	ch->carrying = obj;
	obj->carried_by = ch;
	obj->in_room = NULL;
	obj->in_obj = NULL;
	ch->carry_number += get_obj_number( obj );
	ch->carry_weight += get_obj_weight( obj );
}


/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
	CHAR_DATA *ch;
	int i;

	if ( !( ch = obj->carried_by ) )
	{
		bug( "Obj_from_char: null ch.", 0 );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
		unequip_char( ch, obj );

	if ( ch->carrying == obj )
	{
		ch->carrying = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for ( prev = ch->carrying; prev; prev = prev->next_content )
		{
			if ( prev->next_content == obj )
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( !prev )
			bug( "Obj_from_char: obj not in list.", 0 );
	}

	/* Clear morph list for shifters - Shade */
	if (ch->class == CLASS_SHAPE_SHIFTER && !IS_NPC(ch))
	{
		for (i=0;i<MAX_WEAR;i++)
		{
			if (ch->pcdata->morph_list[i] == obj)
			{
				ch->pcdata->morph_list[i] = NULL;
				break;
			}
		}
	}

	obj->carried_by = NULL;
	obj->next_content = NULL;
	ch->carry_number -= get_obj_number( obj );
	ch->carry_weight -= get_obj_weight( obj );
}


/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
	if ( obj->item_type != ITEM_ARMOR )
		return 0;

	switch ( iWear )
	{
	    case WEAR_BODY:     return 3 * obj->value[0];
	    case WEAR_HEAD:	return 2 * obj->value[0];
	    case WEAR_LEGS:	return 2 * obj->value[0];
	    case WEAR_FEET:	return     obj->value[0];
	    case WEAR_HANDS:	return     obj->value[0];
	    case WEAR_ARMS:	return     obj->value[0];
	    case WEAR_SHIELD:	return     obj->value[0];
	    case WEAR_FINGER_L:	return     obj->value[0];
	    case WEAR_FINGER_R: return     obj->value[0];
	    case WEAR_NECK_1:	return     obj->value[0];
	    case WEAR_NECK_2:	return     obj->value[0];
	    case WEAR_ABOUT:	return 2 * obj->value[0];
	    case WEAR_WAIST:	return     obj->value[0];
	    case WEAR_WRIST_L:	return     obj->value[0];
	    case WEAR_WRIST_R:	return     obj->value[0];
	    case WEAR_HOLD:	return     obj->value[0];
	}

	return 0;
}


/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
	OBJ_DATA *obj;

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( obj->deleted )
			continue;

		if ( obj->wear_loc == iWear )
			return obj;
	}

	return NULL;
}


/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
	AFFECT_DATA *paf;
	char         buf [ MAX_STRING_LENGTH ];

	if ( get_eq_char( ch, iWear ) )
	{
		sprintf( buf, "Equip_char: %s already equipped at %d.",
			ch->name, iWear );
		bug( buf, 0 );
		return;
	}

	if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
	    || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
	    || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) ) )
	{
		/*
		 * Thanks to Morgenes for the bug fix here!
		 */
		act( "You are {Yzapped{x by $p and drop it.", ch, obj, NULL, TO_CHAR );
		act( "$n is {Yzapped{x by $p and drops it.",  ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		return;
	}

	if( iWear != WEAR_RANGED_WEAPON )
	{
		ch->armor -= apply_ac( obj, iWear );

		for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
			affect_modify( ch, paf, TRUE, obj );

		for ( paf = obj->affected; paf; paf = paf->next )
			affect_modify( ch, paf, TRUE, obj );

		if ( obj->item_type == ITEM_LIGHT
		    && iWear == WEAR_LIGHT
		    && obj->value[2] != 0
		    && ch->in_room )
			++ch->in_room->light;
	}

	obj->wear_loc = iWear;
	update_pos( ch );
	return;
}


void form_equip_char (CHAR_DATA *ch, OBJ_DATA *obj, int iWear)
{
	OBJ_DATA *removed = NULL;

	/* force removal of old weapon */
	if ((removed = get_eq_char(ch, iWear)))
		unequip_char(ch, removed);

	/* do the equipping */
	equip_char(ch, obj, iWear);
}


/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
	AFFECT_DATA *paf;
	char         buf [ MAX_STRING_LENGTH ];

	if ( obj->wear_loc == WEAR_NONE )
	{
		sprintf( buf, "Unequip_char: %s already unequipped with %d.",
			ch->name, obj->pIndexData->vnum );
		bug( buf, 0 );
		return;
	}

	if( obj->wear_loc != WEAR_RANGED_WEAPON )
	{
		ch->armor += apply_ac( obj, obj->wear_loc );

		for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
			affect_modify( ch, paf, FALSE, obj );

		for ( paf = obj->affected; paf; paf = paf->next )
			affect_modify( ch, paf, FALSE, obj );

		if ( obj->item_type == ITEM_LIGHT
		    && obj->wear_loc == WEAR_LIGHT
		    && obj->value[2] != 0
		    && ch->in_room
		    && (!IS_NPC(ch) && ch->desc && ch->desc->connected != CON_GET_OLD_PASSWORD)
		    && ch->in_room->light > 0 )
			--ch->in_room->light;
	}

	obj->wear_loc = -1;
	return;
}


/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
	OBJ_DATA *obj;
	int       nMatch;

	nMatch = 0;
	for ( obj = list; obj; obj = obj->next_content )
	{
		if ( obj->deleted )
			continue;

		if ( obj->pIndexData == pObjIndex )
			nMatch++;
	}

	return nMatch;
}


/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
	ROOM_INDEX_DATA *in_room;

	if ( !( in_room = obj->in_room ) )
	{
		bug( "obj_from_room: NULL.", 0 );
		return;
	}

	if ( obj == in_room->contents )
	{
		in_room->contents = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for ( prev = in_room->contents; prev; prev = prev->next_content )
		{
			if ( prev->next_content == obj )
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( !prev )
		{
			bug( "Obj_from_room: obj not found.", 0 );
			return;
		}
	}

	obj->in_room      = NULL;
	obj->next_content = NULL;
	return;
}


/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
	obj->next_content = pRoomIndex->contents;
	pRoomIndex->contents = obj;
	obj->in_room = pRoomIndex;
	obj->carried_by = NULL;
	obj->in_obj = NULL;
	return;
}


/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
	if ( obj_to->deleted )
	{
		bug( "Obj_to_obj:  Obj_to already deleted", 0 );
		return;
	}

	obj->next_content = obj_to->contains;
	obj_to->contains = obj;
	obj->in_obj = obj_to;
	obj->in_room = NULL;
	obj->carried_by	= NULL;

	for ( ; obj_to; obj_to = obj_to->in_obj )
	{
		if ( obj_to->deleted )
			continue;

		if ( obj_to->carried_by )
		{
			/* Strider - Comment out line below - Container code fix  */
			/* obj_to->carried_by->carry_number += get_obj_number( obj ); */
			obj_to->carried_by->carry_weight += get_obj_weight( obj );
		}
	}

	return;
}


/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
	OBJ_DATA *obj_from;

	if ( !( obj_from = obj->in_obj ) )
	{
		bug( "Obj_from_obj: null obj_from.", 0 );
		return;
	}

	if ( obj == obj_from->contains )
	{
		obj_from->contains = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for ( prev = obj_from->contains; prev; prev = prev->next_content )
		{
			if ( prev->next_content == obj )
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if ( !prev )
		{
			bug( "Obj_from_obj: obj not found.", 0 );
			return;
		}
	}

	obj->next_content = NULL;
	obj->in_obj       = NULL;

	for ( ; obj_from; obj_from = obj_from->in_obj )
	{
		if ( obj_from->deleted )
			continue;
		if ( obj_from->carried_by )
		{
			/* Strider - Comment out line below - Container code fix  */
			/*obj_from->carried_by->carry_number -= get_obj_number( obj ); */
			obj_from->carried_by->carry_weight -= get_obj_weight( obj );
		}
	}

	return;
}


/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
	OBJ_DATA *obj_content;
	OBJ_DATA *obj_next;
	extern bool delete_obj;

	if ( obj->deleted )
	{
		bug( "Extract_obj:  Obj already deleted", 0 );
		return;
	}

	if ( obj->in_room )
		obj_from_room( obj );

	else if ( obj->carried_by )
		obj_from_char( obj );

	else if ( obj->in_obj )
		obj_from_obj( obj );

	for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
	{
		obj_next = obj_content->next_content;
		if( obj_content->deleted )
			continue;
		extract_obj( obj_content );
	}

	obj->deleted = TRUE;
	delete_obj   = TRUE;
	return;
}


/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
	CHAR_DATA *wch;
	OBJ_DATA  *obj;
	OBJ_DATA  *obj_next;
	extern bool delete_char;

	if ( !ch->in_room )
	{
		bug( "Extract_char: NULL.", 0 );
		return;
	}

	if ( fPull )
	{
	    char* name;

		if ( IS_NPC ( ch ) )
			name = ch->short_descr;
		else
			name = ch->name;

		die_follower( ch, name );
	}

	stop_fighting( ch, TRUE );

	if (ch->rider)
	{
		act( "With your mount no longer here, you fall off.", ch->rider, NULL, NULL, TO_CHAR);
		act( "$c falls from $s slain mount.", ch->rider, NULL, NULL, TO_ROOM);
		strip_mount (ch->rider);
	}

	if (ch->mount)
	{
		act ("$c falls from $s mount.", ch, NULL, NULL, TO_ROOM);
		strip_mount (ch);
	}

	/* Prevent reporting of removal of perm affect items to active players */
	char_from_room (ch);
	char_to_room (ch, get_room_index(ROOM_VNUM_LIMBO));

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( obj->deleted )
			continue;
		extract_obj( obj );
	}

	char_from_room (ch);

	if ( !fPull )
	{
		ROOM_INDEX_DATA *location;

		if ( !( location = get_room_index( ROOM_VNUM_PURGATORY_A ) ) )
		{
			bug( "Purgatory A does not exist!", 0 );
			char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
		}
		else
			char_to_room( ch, location );
		return;
	}

	if ( IS_NPC( ch ) )
		--ch->pIndexData->count;

	if ( ch->desc && ch->desc->original )
		do_return( ch, "" );

	for ( wch = char_list; wch; wch = wch->next )
	{
		if ( wch->reply == ch )
			wch->reply = NULL;
	}

	ch->deleted = TRUE;

	if ( ch->desc )
		ch->desc->character = NULL;

	delete_char = TRUE;
	return;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *rch;
	char       arg [ MAX_INPUT_LENGTH ];
	int        number;
	int        count;

	number = number_argument( argument, arg );
	count  = 0;

	if ( !str_cmp( arg, "self" ) )
		return ch;

	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
		if ( !can_see( ch, rch ) || !is_name( arg, rch->name ) )
			continue;

		if ( ++count == number )
			return rch;
	}

	return NULL;
}


/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *wch;
	char       arg [ MAX_INPUT_LENGTH ];
	int        number;
	int        count;

	if ( ( wch = get_char_room( ch, argument ) ) )
		return wch;

	number = number_argument( argument, arg );
	count  = 0;

	for ( wch = char_list; wch ; wch = wch->next )
	{
		if ( !can_see( ch, wch ) || !multi_keyword_match( arg, wch->name ) )
			continue;

		if ( ++count == number )
			return wch;
	}

	return NULL;
}


/*
 * Get quest mob from world
 */
CHAR_DATA *get_qchar_world( CHAR_DATA *ch, char *argument, int vnum )
{
	CHAR_DATA *wch;

	for ( wch = char_list; wch ; wch = wch->next )
	{
		if ( !can_see( ch, wch ) || strcmp( argument, wch->name )
		    || !IS_NPC(wch) || vnum != wch->pIndexData->vnum )
			continue;

		return wch;
	}

	return NULL;
}


/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
	OBJ_DATA *obj;

	for ( obj = object_list; obj; obj = obj->next )
	{
		if ( obj->deleted )
			continue;

		if ( obj->pIndexData == pObjIndex )
			return obj;
	}

	return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       number;
	int       count;

	number = number_argument( argument, arg );
	count  = 0;

	for ( obj = list; obj; obj = obj->next_content )
	{
		if ( (can_see_obj( ch, obj ) || (obj->in_obj && IS_SET(obj->in_obj->wear_flags, ITEM_WEAR_POUCH))) && is_name( arg, obj->name ) )
		{
			if ( ++count == number )
				return obj;
		}
	}

	return NULL;
}


/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       number;
	int       count;

	number = number_argument( argument, arg );
	count  = 0;

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE
		    && can_see_obj( ch, obj )
		    && is_name( arg, obj->name ) )
		{
			if ( ++count == number )
				return obj;
		}
	}

	return NULL;
}


/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       number;
	int       count;

	number = number_argument( argument, arg );
	count  = 0;

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( obj->wear_loc != WEAR_NONE
		    && can_see_obj( ch, obj )
		    && is_name( arg, obj->name ) )
		{
			if ( ++count == number )
				return obj;
		}
	}

	return NULL;
}


/*
 * returns race number
 */
int race_lookup (const char *name)
{
	int race;

	for ( race = 0; race_table[race].race_name != NULL; race++)
	{
		if (LOWER(name[0]) == LOWER(race_table[race].race_name[0])
		    &&  !str_prefix( name,race_table[race].race_name))
			return race;
	}

	return 0;
}

/*
 * returns class number
 */
int class_lookup (const char *name)
{
	int class;

	for ( class = 0; class < MAX_CLASS; class++)
	{
		if (LOWER(name[0]) == LOWER(class_table[class].who_name[0])
		    &&  !str_prefix( name,class_table[class].who_name))
			return class;
	}

	return -1;
}


bool is_clan (CHAR_DATA *ch)
{
    return ch->clan;
}


bool is_same_clan (CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (clan_table[ch->clan].independent)
		return FALSE;
	else
		return (ch->clan == victim->clan);
}


int clan_lookup (const char *name)
{
	int clan;
	char buf [MAX_INPUT_LENGTH];

	sprintf(buf, "Looking up clan %s", name);
	log_string(buf);

	for (clan = 0; clan < MAX_CLAN; clan++)
	{
		if (LOWER(name[0]) == LOWER(clan_table[clan].name[0])
		    &&  !str_prefix(name,clan_table[clan].name))
		{
			sprintf(buf, "clan lookup clanname %d", clan);
			log_string(buf);
			return clan;
		}
	}

	/* &&  !str_prefix(name,clan_table[clan].who_name)) */

	for (clan = 0; clan < MAX_CLAN; clan++)
	{
		if (LOWER(name[0]) == LOWER(clan_table[clan].who_name[0])
		    && LOWER(name[1]) == LOWER(clan_table[clan].who_name[1])
		    && LOWER(name[2]) == LOWER(clan_table[clan].who_name[2]))
		{
			sprintf(buf, "clan lookup clanwho %d", clan);
			log_string(buf);
			return clan;
		}
	}

	return -1;
}


/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;

	obj = get_obj_list( ch, argument, ch->in_room->contents );

	if ( obj )
		return obj;

	if ( ( obj = get_obj_carry( ch, argument ) ) )
		return obj;

	if ( ( obj = get_obj_wear( ch, argument ) ) )
		return obj;

	return NULL;
}


/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	char      arg [ MAX_INPUT_LENGTH ];
	int       number;
	int       count;

	if ( ( obj = get_obj_here( ch, argument ) ) )
		return obj;

	number = number_argument( argument, arg );
	count  = 0;

	for ( obj = object_list; obj; obj = obj->next )
	{
		if ( can_see_obj( ch, obj ) && multi_keyword_match( arg, obj->name ) )
		{
			if ( ++count == number )
				return obj;
		}
	}

	return NULL;
}


/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int plat, int gold, int silver, int copper )
{
	OBJ_DATA *obj;

	if ( (plat + gold + silver + copper) <= 0 )
	{
		bug( "Create_money: zero or negative money %d.", (plat + gold + silver + copper) );
		copper = 1;
	}

	if ( (plat + gold + silver + copper) == 1 )
		obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
	else
		obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );

	obj->value[0]		= copper;
	obj->value[1]		= silver;
	obj->value[2]		= gold;
	obj->value[3]		= plat;
	obj->weight = plat + gold + silver + copper;

	return obj;
}


/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
	return 1;

	/*
	 * Strider - Comment out entire function, replace with ->  return 1;
	 * This fixes the container code
	 *

	int number = 0;

	if ( obj->item_type == ITEM_CONTAINER )
	{
		for ( obj = obj->contains; obj; obj = obj->next_content )
		{
			if ( obj->deleted )
				continue;
			number += get_obj_number( obj );
		}
		else
			number = 1;
	}

	return number;
	*/
}


/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
	int weight;

	weight = obj->weight;
	for ( obj = obj->contains; obj; obj = obj->next_content )
	{
		if ( obj->deleted )
			continue;
		weight += get_obj_weight( obj );
	}

	return weight;
}


/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
	OBJ_DATA *obj;

	if ( pRoomIndex->light > 0 )
		return FALSE;

	for ( obj = pRoomIndex->contents; obj; obj = obj->next_content )
	{
		if ( obj->deleted )
			continue;

		if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
			return FALSE;
	}

	if ( IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
		return TRUE;

	if ( pRoomIndex->sector_type == SECT_INSIDE )
		return FALSE;

	if ( weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK )
		return TRUE;

	return FALSE;
}

/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
	CHAR_DATA *rch;
	int count = 0;

	for ( rch = pRoomIndex->people; rch; rch = rch->next_in_room )
	{
		if ( rch->deleted )
			continue;
		count++;
	}

	if ( IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE  ) && count >= 2 )
		return TRUE;

	if ( IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY ) && count >= 1 )
		return TRUE;

	return FALSE;
}


/*
 * True if char can see through mist form - Istari
 */
bool can_see_mist (CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (victim->deleted )
		return FALSE;

	if (ch->level > LEVEL_HERO || ch->sub_class == SUB_CLASS_VAMPIRE)
		return TRUE;

	return FALSE;
}


/*
 * True if char can see victim.
 */
bool can_see (CHAR_DATA *ch, CHAR_DATA *victim)
{
	int chance;

	if (victim->deleted)
		return FALSE;

	if (ch == victim)
		return TRUE;

	if (!IS_NPC(victim)
	    && IS_SET(victim->act, PLR_WIZINVIS)
	    && get_trust(ch) < get_trust(victim))
		return FALSE;

	if (ch->level > LEVEL_HERO)
		return TRUE;

	if (!IS_NPC(ch)
	    && IS_AFFECTED(ch, AFF_NON_CORPOREAL)
	    && ch->in_room != victim->in_room)
		return FALSE;

	if (IS_AFFECTED(ch, AFF_BLIND))
		return FALSE;

	if (IS_NPC(victim) && IS_SET(victim->act, ACT_WIZINVIS_MOB))
		return FALSE;

	if (!IS_NPC(ch) && is_affected(ch, gsn_song_of_revelation))
	{
		chance = ch->pcdata->learned[gsn_song_of_revelation];
		chance += (ch->level - victim->level) * 2;

		if (chance > 95)
			chance = 95;

		if (chance < 5)
			chance = 5;

		if (number_percent() < chance)
			return TRUE;
	}

	if (!IS_NPC(victim) && victim->pcdata->tailing)
	{
		if (!IS_NPC(victim) && !str_cmp(ch->name, victim->pcdata->tailing))
		{
			chance = 10;
			chance += (ch->level - victim->level);

                	if (chance > 95)
                        	chance = 95;

                	if (chance < 5)
                        	chance = 5;

			if (number_percent() < chance )
				victim->pcdata->tailing[0] = '\0';
			else
				return FALSE;
		}
	}

	if (!IS_NPC(victim) && is_affected(victim, gsn_song_of_shadows))
	{
		chance = victim->pcdata->learned[gsn_song_of_shadows];
		chance += (victim->level - ch->level) * 3;

		if (chance > 95)
			chance = 95;

		if (chance < 5)
			chance = 5;

		if (number_percent() < chance)
			return FALSE;
	}

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return TRUE;

	if (IS_NPC(ch)
	    && !IS_NPC(victim)
	    && IS_AFFECTED(victim, AFF_SNEAK)
	    && !IS_AFFECTED(ch, AFF_DETECT_SNEAK)
	    && number_percent() < (50 + (victim->level - ch->level) * 5))
		return FALSE;

	if (IS_AFFECTED(victim, AFF_INVISIBLE) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
		return FALSE;

	if (IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
		return FALSE;

	if (room_is_dark(ch->in_room)
	    && (ch->sub_class == SUB_CLASS_VAMPIRE || ch->form == FORM_FLY))
		return TRUE;

	if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
		return FALSE;

	if (victim->position == POS_DEAD)
		return TRUE;

	return TRUE;
}


/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
	if ( obj->deleted )
		return FALSE;

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
		return TRUE;

	if ( IS_AFFECTED( ch, AFF_BLIND ) )
		return FALSE;

	if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
		return TRUE;

	if ( room_is_dark( ch->in_room ) && ch->sub_class == SUB_CLASS_VAMPIRE)
		return TRUE;

	if (!IS_NPC(ch) && is_affected(ch, gsn_song_of_revelation))
		return TRUE;

	if ( room_is_dark( ch->in_room ) && !IS_AFFECTED( ch, AFF_INFRARED ) )
		return FALSE;

	if ( IS_SET( obj->extra_flags, ITEM_INVIS )
	    && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
		return FALSE;

	return TRUE;
}


/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
	if ( !IS_SET( obj->extra_flags, ITEM_NODROP ) )
		return TRUE;

	if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
		return TRUE;

	return FALSE;
}


/*
 * Return ascii name of an item type.
 */
char *item_type_name( OBJ_DATA *obj  )
{
	OBJ_DATA *in_obj;
	char buf [ MAX_STRING_LENGTH ];

	switch ( obj->item_type )
	{
	    case ITEM_LIGHT:		return "light";
	    case ITEM_SCROLL:		return "scroll";
	    case ITEM_WAND:		return "wand";
	    case ITEM_STAFF:		return "staff";
	    case ITEM_WEAPON:		return "weapon";
	    case ITEM_TREASURE:		return "treasure";
	    case ITEM_ARMOR:		return "armor";
	    case ITEM_POTION:		return "potion";
	    case ITEM_FURNITURE:	return "furniture";
	    case ITEM_TRASH:		return "trash";
	    case ITEM_CONTAINER:	return "container";
	    case ITEM_DRINK_CON:	return "drink container";
	    case ITEM_KEY:		return "key";
	    case ITEM_FOOD:		return "food";
	    case ITEM_MONEY:		return "money";
	    case ITEM_BOAT:		return "boat";
	    case ITEM_CORPSE_NPC:	return "npc corpse";
	    case ITEM_CORPSE_PC:	return "pc corpse";
	    case ITEM_FOUNTAIN:		return "fountain";
	    case ITEM_PILL:		return "pill";
	    case ITEM_PAINT:            return "paint pigment";
	    case ITEM_CLAN_OBJECT:	return "clan item";
	    case ITEM_LOCK_PICK:	return "lockpick";
	    case ITEM_INSTRUMENT:	return "musical instrument";
	    case ITEM_PORTAL:           return "portal";
	    case ITEM_POISON_POWDER:	return "poison powder";
	    case ITEM_ARMOURERS_HAMMER:	return "armourer's hammer";
	    case ITEM_MITHRIL:		return "mithril";
	    case ITEM_WHETSTONE:	return "whetstone";
	    case ITEM_ANVIL:		return "anvil";
	}

	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
		;

	if ( in_obj->carried_by )
		sprintf( buf, "Item_type_name: unknown type %d from %s owned by %s.",
			obj->item_type, obj->name, obj->carried_by->name );
	else
		sprintf( buf,
			"Item_type_name: unknown type %d from %s owned by (unknown).",
			obj->item_type, obj->name );

	bug( buf, 0 );
	return "(unknown)";
}


/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
	switch ( location )
	{
	    case APPLY_NONE:		return "none";
	    case APPLY_STR:		return "strength";
	    case APPLY_DEX:		return "dexterity";
	    case APPLY_INT:		return "intelligence";
	    case APPLY_WIS:		return "wisdom";
	    case APPLY_CON:		return "constitution";
	    case APPLY_SEX:		return "sex";
	    case APPLY_CLASS:		return "class";
	    case APPLY_LEVEL:		return "level";
	    case APPLY_AGE:		return "age";
	    case APPLY_HEIGHT:          return "height";
	    case APPLY_WEIGHT:          return "weight";
	    case APPLY_MANA:		return "mana";
	    case APPLY_HIT:		return "hit points";
	    case APPLY_MOVE:		return "moves";
	    case APPLY_GOLD:		return "gold";
	    case APPLY_EXP:		return "experience";
	    case APPLY_AC:		return "armor class";
	    case APPLY_HITROLL:		return "hit roll";
	    case APPLY_DAMROLL:		return "damage roll";
	    case APPLY_SAVING_PARA:	return "save vs paralysis";
	    case APPLY_SAVING_ROD:	return "save vs rod";
	    case APPLY_SAVING_PETRI:	return "save vs petrification";
	    case APPLY_SAVING_BREATH:	return "save vs breath";
	    case APPLY_SAVING_SPELL:	return "save vs spell";
	    case APPLY_SANCTUARY:	return "sanctuary";
	    case APPLY_SNEAK:		return "sneak";
	    case APPLY_FLY:		return "fly";
	    case APPLY_INVIS:		return "invisibility";
	    case APPLY_DETECT_INVIS:	return "detect invisibility";
	    case APPLY_DETECT_HIDDEN:	return "detect hidden";
	    case APPLY_PROTECT:		return "protection";
	    case APPLY_PASS_DOOR:	return "pass door";
	    case APPLY_GLOBE:		return "globe";
	    case APPLY_FLAMING:		return "fireshield";
	    case APPLY_RESIST_HEAT:     return "heat resistance";
	    case APPLY_RESIST_COLD:     return "cold resistance";
	    case APPLY_RESIST_LIGHTNING: return "lightning resistance";
	    case APPLY_RESIST_ACID:     return "acid resistance";
	}

	bug( "Affect_location_name: unknown location %d.", location );
	return "(unknown)";
}


int item_name_type( char *name )
{
	if ( !str_cmp( name, "light"     ) ) return ITEM_LIGHT;
	if ( !str_cmp( name, "scroll"    ) ) return ITEM_SCROLL;
	if ( !str_cmp( name, "wand"      ) ) return ITEM_WAND;
	if ( !str_cmp( name, "staff"     ) ) return ITEM_STAFF;
	if ( !str_cmp( name, "weapon"    ) ) return ITEM_WEAPON;
	if ( !str_cmp( name, "treasure"  ) ) return ITEM_TREASURE;
	if ( !str_cmp( name, "armor"     ) ) return ITEM_ARMOR;
	if ( !str_cmp( name, "potion"    ) ) return ITEM_POTION;
	if ( !str_cmp( name, "furniture" ) ) return ITEM_FURNITURE;
	if ( !str_cmp( name, "trash"     ) ) return ITEM_TRASH;
	if ( !str_cmp( name, "container" ) ) return ITEM_CONTAINER;
	if ( !str_cmp( name, "drink"     ) ) return ITEM_DRINK_CON;
	if ( !str_cmp( name, "key"       ) ) return ITEM_KEY;
	if ( !str_cmp( name, "food"      ) ) return ITEM_FOOD;
	if ( !str_cmp( name, "money"     ) ) return ITEM_MONEY;
	if ( !str_cmp( name, "boat"      ) ) return ITEM_BOAT;
	if ( !str_cmp( name, "corpse"    ) ) return ITEM_CORPSE_NPC;
	if ( !str_cmp( name, "fountain"  ) ) return ITEM_FOUNTAIN;
	if ( !str_cmp( name, "pill"      ) ) return ITEM_PILL;

	if ( !str_cmp( name, "poison powder"      ) ) return ITEM_POISON_POWDER;
	if ( !str_cmp( name, "armourer's hammer"  ) ) return ITEM_ARMOURERS_HAMMER;
	if ( !str_cmp( name, "mithril"            ) ) return ITEM_MITHRIL;
	if ( !str_cmp( name, "whetstone"          ) ) return ITEM_WHETSTONE;

	return 0;
}


/*
 * Return ascii name of an affect bit vector.
 * Remember to change the function directly below too.
 */
char *affect_bit_name (int vector)
{
	if ( vector & AFF_BLIND         ) return "blind";
	if ( vector & AFF_INVISIBLE     ) return "invisible";
	if ( vector & AFF_DETECT_EVIL   ) return "detect_evil";
	if ( vector & AFF_DETECT_INVIS  ) return "detect_invis";
	if ( vector & AFF_DETECT_MAGIC  ) return "detect_magic";
	if ( vector & AFF_DETECT_HIDDEN ) return "detect_hidden";
	if ( vector & AFF_HOLD          ) return "hold";
	if ( vector & AFF_SANCTUARY     ) return "sanctuary";
	if ( vector & AFF_FAERIE_FIRE   ) return "faerie_fire";
	if ( vector & AFF_INFRARED      ) return "infrared";
	if ( vector & AFF_CURSE         ) return "curse";
	if ( vector & AFF_FLAMING       ) return "flaming";
	if ( vector & AFF_POISON        ) return "poison";
	if ( vector & AFF_PROTECT       ) return "protect";
	if ( vector & AFF_SLEEP         ) return "sleep";
	if ( vector & AFF_SNEAK         ) return "sneak";
	if ( vector & AFF_HIDE          ) return "hide";
	if ( vector & AFF_CHARM         ) return "charm";
	if ( vector & AFF_FLYING        ) return "flying";
	if ( vector & AFF_PASS_DOOR     ) return "pass_door";
	if ( vector & AFF_MEDITATE	) return "meditating";
	if ( vector & AFF_DETECT_GOOD   ) return "detect_good";
	if ( vector & AFF_GLOBE         ) return "globed";
	if ( vector & AFF_DETECT_TRAPS  ) return "detect_traps";
	if ( vector & AFF_DETECT_SNEAK  ) return "detect_sneak";
	if ( vector & AFF_DETER         ) return "deter";
	if ( vector & AFF_BATTLE_AURA   ) return "battle_aura";
	if ( vector & AFF_SWIM          ) return "swim";
	if ( vector & AFF_PRAYER_PLAGUE ) return "plague";

	return "none";
}


/*
 *  Used for the 'affect' command output
 */
char* affect_bit_name_nice (int vector)
{
	if ( vector & AFF_BLIND         ) return "blindness";
	if ( vector & AFF_INVISIBLE     ) return "invisibility";
	if ( vector & AFF_DETECT_EVIL   ) return "detect evil";
	if ( vector & AFF_DETECT_INVIS  ) return "detect invisibility";
	if ( vector & AFF_DETECT_MAGIC  ) return "detect magic";
	if ( vector & AFF_DETECT_HIDDEN ) return "detect hidden";
	if ( vector & AFF_HOLD          ) return "hold";
	if ( vector & AFF_SANCTUARY     ) return "sanctuary";
	if ( vector & AFF_FAERIE_FIRE   ) return "faerie fire";
	if ( vector & AFF_INFRARED      ) return "infrared vision";
	if ( vector & AFF_CURSE         ) return "curse";
	if ( vector & AFF_FLAMING       ) return "fireshield";
	if ( vector & AFF_POISON        ) return "poison";
	if ( vector & AFF_PROTECT       ) return "protection";
	if ( vector & AFF_SLEEP         ) return "sleep";
	if ( vector & AFF_SNEAK         ) return "sneak";
	if ( vector & AFF_HIDE          ) return "hide";
	if ( vector & AFF_CHARM         ) return "charm";
	if ( vector & AFF_FLYING        ) return "flight";
	if ( vector & AFF_PASS_DOOR     ) return "pass door";
	if ( vector & AFF_MEDITATE	) return "meditation";
	if ( vector & AFF_DETECT_GOOD   ) return "detect good";
	if ( vector & AFF_GLOBE         ) return "globe";
	if ( vector & AFF_DETECT_TRAPS  ) return "detect traps";
	if ( vector & AFF_DETECT_SNEAK  ) return "detect sneak";
	if ( vector & AFF_DETER         ) return "deter aggression";
	if ( vector & AFF_BATTLE_AURA   ) return "battle aura";
	if ( vector & AFF_SWIM          ) return "swim";
	if ( vector & AFF_NON_CORPOREAL ) return "non-corporeal form";
	if ( vector & AFF_PRAYER_PLAGUE ) return "plague";

	return "some unknown effect";
}


/*
 * Return ascii name of anti-class vector - brutus
 */
char *extra_class_name (int extra_flags)
{
	if ( extra_flags & ITEM_ANTI_MAGE) return "Mage";
	if ( extra_flags & ITEM_ANTI_CLERIC) return "Cleric";
	if ( extra_flags & ITEM_ANTI_THIEF) return "Thief";
	if ( extra_flags & ITEM_ANTI_WARRIOR) return "Warrior";
	if ( extra_flags & ITEM_ANTI_PSIONIC) return "Psionic";
	if ( extra_flags & ITEM_ANTI_BRAWLER) return "Brawler";
	if ( extra_flags & ITEM_ANTI_SHAPE_SHIFTER) return "Shape Shifter";
	if ( extra_flags & ITEM_ANTI_RANGER) return "Ranger";
	return "none";
}


/* return class and subclass names  - geoff */
char *full_class_name (int class)
{
	if (class == CLASS_MAGE) return "Mage";
	if (class == CLASS_CLERIC) return "Cleric";
	if (class == CLASS_THIEF) return "Thief";
	if (class == CLASS_WARRIOR) return "Warrior";
	if (class == CLASS_PSIONICIST) return "Psionic";
	if (class == CLASS_BRAWLER) return "Brawler";
	if (class == CLASS_SHAPE_SHIFTER) return "Shape Shifter";
	if (class == CLASS_RANGER) return "Ranger";

	return "NO CLASS";
}


char *full_sub_class_name (int sub_class)
{
	if (sub_class == SUB_CLASS_NECROMANCER) return "Necromancer";
	if (sub_class == SUB_CLASS_WARLOCK) return "Warlock";
	if (sub_class == SUB_CLASS_TEMPLAR) return "Templar";
	if (sub_class == SUB_CLASS_DRUID) return "Druid";
	if (sub_class == SUB_CLASS_NINJA) return "Ninja";
	if (sub_class == SUB_CLASS_BOUNTY) return "Bounty Hunter";
	if (sub_class == SUB_CLASS_THUG) return "Thug";
	if (sub_class == SUB_CLASS_KNIGHT) return "Knight";
	if (sub_class == SUB_CLASS_SATANIST) return "Satanist";
	if (sub_class == SUB_CLASS_WITCH) return "Witch";
	if (sub_class == SUB_CLASS_WEREWOLF) return "Werewolf";
	if (sub_class == SUB_CLASS_VAMPIRE) return "Vampire";
	if (sub_class == SUB_CLASS_MONK) return "Monk";
	if (sub_class == SUB_CLASS_MARTIAL_ARTIST) return "Martial Artist";
	if (sub_class == SUB_CLASS_BARBARIAN) return "Barbarian";
	if (sub_class == SUB_CLASS_BARD) return "Bard";

	return "NO SUB CLASS";
}


/* Return assci name for character levels. */
char *extra_level_name (CHAR_DATA *ch)
{
	switch (get_trust(ch))
	{
	    case L_HER: return "Hero";
	    case L_BUI: return "Builder";
	    case L_APP: return "Apprentice";
	    case L_JUN: return "Junior";
	    case L_SEN: return "Senior";
	    case L_DIR: return "God's Gopher";
	    case L_IMM: return "The Boss";

	    default: return "nothing special";
	}
}


char *extra_form_name (int form)
{
	if ( form == FORM_NORMAL          ) return "normal";
	if ( form == FORM_CHAMELEON       ) return "chameleon";
	if ( form == FORM_HAWK            ) return "hawk";
	if ( form == FORM_CAT             ) return "cat";
	if ( form == FORM_SNAKE           ) return "snake";
	if ( form == FORM_SCORPION        ) return "scorpion";
	if ( form == FORM_SPIDER          ) return "spider";
	if ( form == FORM_BEAR            ) return "bear";
	if ( form == FORM_TIGER           ) return "tiger";
	if ( form == FORM_HYDRA           ) return "hydra";
	if ( form == FORM_WOLF	          ) return "wolf";
	if ( form == FORM_DIREWOLF	  ) return "direwolf";
	if ( form == FORM_PHOENIX         ) return "phoenix";
	if ( form == FORM_DEMON           ) return "demon";
	if ( form == FORM_DRAGON          ) return "dragon";
	if ( form == FORM_FLY		  ) return "fly";
	if ( form == FORM_GRIFFIN	  ) return "griffin";

	return "none";
}


int extra_form_int (char *name)
{
	int form = -1;

	if (!strncmp(name, "normal", strlen(name))) 	form = FORM_NORMAL;
	if (!strncmp(name, "chameleon", strlen(name)))	form = FORM_CHAMELEON;
	if (!strncmp(name, "hawk", strlen(name))) 	form = FORM_HAWK;
	if (!strncmp(name, "cat", strlen(name))) 	form = FORM_CAT;
	if (!strncmp(name, "snake", strlen(name))) 	form = FORM_SNAKE;
	if (!strncmp(name, "scorpion", strlen(name))) 	form = FORM_SCORPION;
	if (!strncmp(name, "spider", strlen(name))) 	form = FORM_SPIDER;
	if (!strncmp(name, "bear", strlen(name))) 	form = FORM_BEAR;
	if (!strncmp(name, "tiger", strlen(name))) 	form = FORM_TIGER;
	if (!strncmp(name, "hydra", strlen(name))) 	form = FORM_HYDRA;
	if (!strncmp(name, "wolf", strlen(name))) 	form = FORM_WOLF;
	if (!strncmp(name, "direwolf", strlen(name))) 	form = FORM_DIREWOLF;
	if (!strncmp(name, "phoenix", strlen(name))) 	form = FORM_PHOENIX;
	if (!strncmp(name, "demon", strlen(name))) 	form = FORM_DEMON;
	if (!strncmp(name, "dragon", strlen(name))) 	form = FORM_DRAGON;
	if (!strncmp(name, "fly", strlen(name))) 	form = FORM_FLY;
	if (!strncmp(name, "griffin", strlen(name))) 	form = FORM_GRIFFIN;

	return form;
}


/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name (int extra_flags)
{
	switch (extra_flags)
	{
	    case ITEM_GLOW:            return "glow";
	    case ITEM_HUM:             return "hum";
	    case ITEM_EGO:             return "ego_item";
	    case ITEM_ANTI_RANGER:     return "anti_ranger";
	    case ITEM_EVIL:            return "evil";
	    case ITEM_INVIS:           return "invis";
	    case ITEM_MAGIC:           return "magic";
	    case ITEM_NODROP:          return "no_drop";
	    case ITEM_BLESS:           return "blessed";
	    case ITEM_ANTI_GOOD:       return "anti_good";
	    case ITEM_ANTI_EVIL:       return "anti_evil";
	    case ITEM_ANTI_NEUTRAL:    return "anti_neutral";
	    case ITEM_NOREMOVE:        return "no_remove";
	    case ITEM_POISONED:        return "poisoned";
	    case ITEM_ANTI_MAGE:       return "anti_mage";
	    case ITEM_ANTI_CLERIC:     return "anti_cleric";
	    case ITEM_ANTI_THIEF:      return "anti_thief";
	    case ITEM_ANTI_WARRIOR:    return "anti_warrior";
	    case ITEM_VORPAL:          return "vorpal";
	    case ITEM_TRAP:            return "trapped";
	    case ITEM_DONATED:         return "donated";
	    case ITEM_BLADE_THIRST:    return "blade_thirst";
	    case ITEM_SHARP:           return "sharpened";
	    case ITEM_FORGED:          return "forged";
	    case ITEM_BODY_PART:       return "body_part";
	    case ITEM_LANCE:           return "lance";
	    case ITEM_ANTI_BRAWLER:    return "anti_brawler";
	    case ITEM_ANTI_SHAPE_SHIFTER: return "anti_shifter";
	    case ITEM_BOW:             return "bow";

	    default: return "(unknown)";
	}
}


CHAR_DATA *get_char( CHAR_DATA *ch )
{
	if ( !ch->pcdata )
		return ch->desc->original;
	else
		return ch;
}


bool longstring( CHAR_DATA *ch, char *argument )
{
	if ( strlen( argument) > 60 )
	{
		send_to_char( "No more than 60 characters in this field.\n\r", ch );
		return TRUE;
	}
	else
		return FALSE;
}


bool authorized (CHAR_DATA *ch, int gsn)
{
	if (IS_NPC(ch))
	{
		send_to_char( "You are not authorized to use this command.\n\r", ch );
		return FALSE;
	}
	else if (ch->pcdata->learned[gsn] < 100)
	{
		send_to_char( "You are not authorized to use this command.\n\r", ch );
		return FALSE;
	}
	else
		return TRUE;
}


void end_of_game( void )
{
	DESCRIPTOR_DATA *d;
	DESCRIPTOR_DATA *d_next;

	for ( d = descriptor_list; d; d = d_next )
	{
		d_next = d->next;
		if ( d->connected == CON_PLAYING )
		{
			if ( d->character->position == POS_FIGHTING )
				interpret( d->character, "save" );
			else
				interpret( d->character, "quit" );
		}
	}

	save_fame_table();
	save_all_clan_tables();
	save_wanted_table();
}


int form_skill_allow (CHAR_DATA *ch, int sn)
{
	int iter;

	/* Ugly vampire and werewolf skill hacks - Anonymous */
	if (ch->sub_class == SUB_CLASS_VAMPIRE)
	{
		for (iter = 0; iter < MAX_VAMPIRE_GAG; iter++)
			if (*vampire_gag_table[iter].skill == sn)
				return 0;
	}

	if (ch->sub_class == SUB_CLASS_WEREWOLF)
	{
		for (iter = 0; iter < MAX_VAMPIRE_GAG-3; iter++)
			if (*vampire_gag_table[iter].skill == sn)
				return 0;
	}

	/* if in normal form return  */
	if (ch->form == FORM_NORMAL)
		return 1;

	if (IS_RACE_ONE(ch) == sn || IS_RACE_TWO(ch) == sn)
		return 1;

	/* for each skill in table, if current form can do then return true */
	for (iter = 0; iter < MAX_FORM_SKILL; iter++)
	{
		if (*form_skill_table[iter].skill == sn)
		{
			if (form_skill_table[iter].form == FORM_ALL)
				return 1;

			if (ch->form == form_skill_table[iter].form)
				return 1;
		}
	}

	/* cannot do in current form */
	return 0;
}


void generate_stats (CHAR_DATA *ch)
{
	/* initial random number 11 -> 15 */
	ch->pcdata->perm_str = number_range(11, 15);
	ch->pcdata->perm_int = number_range(11, 15);
	ch->pcdata->perm_wis = number_range(11, 15);
	ch->pcdata->perm_dex = number_range(11, 15);
	ch->pcdata->perm_con = number_range(11, 15);

	/* racial modification */
	ch->pcdata->perm_str += race_table[ch->race].str_bonus;
	ch->pcdata->perm_int += race_table[ch->race].int_bonus;
	ch->pcdata->perm_wis += race_table[ch->race].wis_bonus;
	ch->pcdata->perm_dex += race_table[ch->race].dex_bonus;
	ch->pcdata->perm_con += race_table[ch->race].con_bonus;

	/* class stats modification */
	ch->pcdata->perm_str += class_table[ch->class].class_stats[0];
	ch->pcdata->perm_int += class_table[ch->class].class_stats[1];
	ch->pcdata->perm_wis += class_table[ch->class].class_stats[2];
	ch->pcdata->perm_dex += class_table[ch->class].class_stats[3];
	ch->pcdata->perm_con += class_table[ch->class].class_stats[4];
}


/*
 * Returns value 0 - 9 based on directional text.
 */
int get_dir( char *txt )
{
	int edir;
	char c1;

	if ( !str_cmp( txt, "somewhere" ) )
		return 10;

	c1 = txt[0];

	if ( c1 == '\0' )
		return 0;

	edir = 0;

	switch ( c1 )
	{
	    case 'n':
	    case '0':
		edir = 0;
		break;

	    case 'e':
	    case '1':
		edir = 1;
		break;

	    case 's':
	    case '2':
		edir = 2;
		break;

	    case 'w':
	    case '3':
		edir = 3;
		break;

	    case 'u':
	    case '4':
		edir = 4;
		break;

	    case 'd':
	    case '5':
		edir = 5;
		break;

	    case '?':
		edir = 10;
		break;
	}

	return edir;
}


int mana_cost (CHAR_DATA *ch, int sn)
{
	if (IS_NPC(ch))
		return 0;

	if ((IS_RACE_ONE(ch) == sn || IS_RACE_TWO(ch) == sn)
	    && !has_pre_req(ch, sn))
		return 50;

	return UMAX (skill_table[sn].min_mana, 60 - ch->pcdata->learned[sn]);
}


int get_phys_penalty (CHAR_DATA *ch)
{
	return (30 - ((get_curr_str(ch) + get_curr_dex(ch) + (get_curr_int(ch) * 2)) / 4));
}


int get_int_penalty (CHAR_DATA *ch)
{
	return (30 - ((get_curr_wis(ch) + (get_curr_int(ch)) * 2) / 3));
}


/* EOF handler.c */

