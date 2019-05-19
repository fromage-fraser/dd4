/*
 * bot.c
 * 
 * Routines for the creation and activity of server-controlled
 * mobs (bots) used within tournaments.  See 'HELP TOURNAMENT'
 * and 'HELP BOT'.
 * 
 * Gezhp 2001
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


/*
 * Global data
 */

int 			bot_entry_count = 0;
struct bot_status	bot_status_table [BOT_MAX_ENTRANTS];


/*
 * Local data
 */

static int		target_list_count;

struct 
{
	CHAR_DATA	*ch;
	int		direction;
	int		distance;
}
target_list [BOT_MAX_TARGETS];


/*
 * Bot data
 */
struct bot_template bot_template_table [BOT_TEMPLATE_NUMBER] =
{
	/*
	 * name (30 chars max; below 16 best), short blurb,
	 * alignment, bot_type, 
	 * bot_flags, 
	 * speed, sight, 
	 * affect_flags (on top of defaults),
	 * body_form, sex,
	 * spec_fun name,
	 * opening_attack,
	 * eq array { vnum, wear_pos }, 15 pieces max; terminate with vnum -1)
	 */
	
	{
		"Vargo",  "Warrior; dual weapons",
		800,  BOT_TYPE_HUNTER,
		BOT_FLAG_IGNORE_EASY_TARGETS | BOT_FLAG_HITROLL_BONUS 
		    | BOT_FLAG_DAMROLL_BONUS,
		BOT_SPEED_FAST,  4,
		AFF_DETECT_HIDDEN,
		0,  SEX_MALE,
		"spec_warrior",
		BOT_OPENING_MURDER,
		
		{
			{ 11001, WEAR_WIELD	},
			{ 11001, WEAR_DUAL	},
			{ 11002, WEAR_HEAD	},
			{ 11003, WEAR_BODY	},
			{ 11004, WEAR_LEGS	},
			{ 11005, WEAR_FEET	},
			{ 11006, WEAR_HANDS	},
			{ -1, -1 		},
		}
	},
	
	{
		"Mongol",  "Barbarian; gouges",
		250,  BOT_TYPE_CAUTIOUS,
		BOT_FLAG_STEALS_KILLS | BOT_FLAG_DAMROLL_BONUS
		    | BOT_FLAG_RANDOM_TARGET,
		BOT_SPEED_MEDIUM,  4,
		AFF_DETECT_SNEAK,
		0,  SEX_MALE,
		"none",
		BOT_OPENING_GOUGE,
		
		{
			{ 11007, WEAR_WIELD	}, 
			{ 11008, WEAR_LEGS	}, 
			{ 11009, WEAR_HEAD	}, 
			{ 11010, WEAR_NECK_1	}, 
			{ 11011, WEAR_WRIST_L	}, 
			{ 11011, WEAR_WRIST_R	}, 
			{ -1, -1 }
		}
	},
	
	{
		"Shingozu",  "Ninja; backstabs, poison weapon",
		0,  BOT_TYPE_HUNTER,
		BOT_FLAG_HITROLL_BONUS | BOT_FLAG_IGNORE_EASY_TARGETS,
		BOT_SPEED_FAST,  6,
		AFF_DETECT_SNEAK | AFF_DETECT_HIDDEN | AFF_SNEAK | AFF_HIDE,
		0,  SEX_MALE,
		"spec_assassin",
		BOT_OPENING_TRAP_BACKSTAB,
		
		{
			{ 11000, WEAR_WIELD 	},
			{ 11012, WEAR_HEAD 	},
			{ 11013, WEAR_ABOUT 	},
			{ 11014, WEAR_FEET 	},
			{ 11015, WEAR_HANDS 	},
			{ 11016, WEAR_WAIST 	},
			{ -1, -1 }
		}
	},
	
	{
		"Azog",  "Archmage; globed",
		-800,  BOT_TYPE_CAUTIOUS,
		BOT_FLAG_WIMPY | BOT_FLAG_STEALS_KILLS | BOT_FLAG_OPENS_IMMEDIATELY,
		BOT_SPEED_MEDIUM,  4,
		AFF_GLOBE | AFF_SNEAK,
		0,  SEX_NEUTRAL,
		"spec_cast_archmage",
		BOT_OPENING_MURDER,
		
		{
			{ 11017, WEAR_WIELD 	},
			{ 11018, WEAR_HOLD 	},
			{ 11019, WEAR_NECK_1 	},
			{ 11020, WEAR_ABOUT 	},
			{ 11021, WEAR_FINGER_L 	},
			{ -1, -1 }
		}
	},
	
	{
		"Balisarious", "Vampire; transfix, lunges",
		-500, BOT_TYPE_HUNTER,
		BOT_FLAG_IGNORE_EASY_TARGETS | BOT_FLAG_HEALTH_BONUS,
		BOT_SPEED_MEDIUM,  5,
		AFF_SNEAK | AFF_HIDE | AFF_DETECT_HIDDEN,
		0, SEX_MALE,
		"spec_vampire",
		BOT_OPENING_LUNGE,

		{
			{ 11022, WEAR_ABOUT	},
			{ 11023, WEAR_WIELD	},
			{ 11024, WEAR_BODY	},
			{ 11005, WEAR_FEET	},
			{ -1, -1 }
		}
	}
};
	
	
/*
 * Functions
 */

void do_bot (CHAR_DATA *ch, char *argument)
{
	char arg [MAX_INPUT_LENGTH];
	
	if (ch->level <= LEVEL_HERO)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}
	
	argument = one_argument(argument, arg);
	
	if (!str_cmp(arg, "list"))
		do_bot_list(ch);
	
	else if (!str_cmp(arg, "status"))
	    	do_bot_status(ch);
	
	else if (!str_cmp(arg, "enter"))
	    	do_bot_enter(ch, argument);
	
	else if (!str_cmp(arg, "remove"))
	    	do_bot_remove(ch, argument);

	else
	    	send_to_char("Usage: bot list\n\r"
			     "       bot status\n\r"
			     "       bot enter <name> <level> [team]\n\r"
			     "       bot remove <name>\n\r", ch);
}


void do_bot_list (CHAR_DATA *ch)
{
	/*
	 * List all bot definitions
	 */
	
	char buf [MAX_STRING_LENGTH];
	int i;
	
	if (BOT_TEMPLATE_NUMBER < 1)
	{
		send_to_char("There are no bots defined.\n\r", ch);
		return;
	}
	
	send_to_char("{WId Name         Align Type Flags Speed Sight Description{x\n\r", ch);
	
	for (i = 0; i < BOT_TEMPLATE_NUMBER; i++)
	{
		sprintf(buf, "%2d %-12s %5d %4d %5d %5d %5d %s\n\r",
			i,
			bot_template_table[i].name,
			bot_template_table[i].alignment,
			bot_template_table[i].bot_type,
			bot_template_table[i].bot_flags,
			bot_template_table[i].speed,
			bot_template_table[i].sight,
			bot_template_table[i].blurb);
		send_to_char(buf, ch);
	}
}


void do_bot_status (CHAR_DATA *ch)
{
	/*
	 * List details of loaded bots (dead or alive)
	 */
	
	char buf [MAX_STRING_LENGTH];
	int i;
	int id;
	bool dead;
	
	if (bot_entry_count < 1)
	{
		send_to_char("There are no bots active.\n\r", ch);
		return;
	}
	
	send_to_char("{WNum Name         Id Where Timer Desp Health Status{x\n\r", ch);
	
	for (i = 0; i < bot_entry_count; i++)
	{
		id = bot_status_table[i].id;
		dead = FALSE;
		
		if (bot_status_table[i].status == BOT_STATUS_DEAD)
			dead = TRUE;
		
		sprintf(buf, "%3d %-12s %2d %5d %5d %4d %6d ",
			i,
			bot_template_table[id].name,
			id,
			dead ? 0 : bot_status_table[i].ch->in_room->vnum,
			dead ? 0 : bot_status_table[i].timer,
			dead ? 0 : bot_status_table[i].desperation,
			dead ? 0 : bot_status_table[i].ch->hit);
		
		if (bot_status_table[i].status == BOT_STATUS_WAITING)
		    	strcat(buf, "waiting");
		
		else if (bot_status_table[i].status == BOT_STATUS_ALIVE)
		    	strcat(buf, "active");

		else if (bot_status_table[i].status == BOT_STATUS_DEAD)
		    	strcat(buf, "dead");

		else 
		    	strcat(buf, "(unknown)");

		strcat(buf, "\n\r");
		send_to_char(buf, ch);
	}
}


void do_bot_enter (CHAR_DATA *ch, char *argument)
{
	/*
	 * Create a bot and enter it into a running tournament
	 */
	
	char buf [MAX_STRING_LENGTH];
	char arg_name [MAX_INPUT_LENGTH];
	char arg_team [MAX_INPUT_LENGTH];
	char arg_level [MAX_INPUT_LENGTH];
	int i, vnum, slot;
	int team = -1;
	int bot_id = -1;
	int level = -1;
	CHAR_DATA *bot;
	OBJ_INDEX_DATA *obj_index;
	OBJ_DATA *obj;
	AFFECT_DATA *paf;
	
	const char *usage = "Syntax: bot enter <bot name> <bot level> [team]\n\r";
	
	if (IS_NPC(ch))
		return;
	
	if (tournament_status == TOURNAMENT_STATUS_NONE)
	{
		send_to_char("No tournament has been posted!\n\r", ch);
		return;	
	}
	
	if (ch != tournament_host)
	{
		send_to_char("You may not enter bots into this tournament.\n\r", ch);
		return;
	}
		
	if (tournament_status != TOURNAMENT_STATUS_POSTED)
	{
		send_to_char("You may not enter any bots at the moment.\n\r", ch);
		return;
	}
	
	argument = one_argument(argument, arg_name);
	argument = one_argument(argument, arg_level);
	one_argument(argument, arg_team);

	/*
	 * Get bot type
	 */
	if (arg_name[0] == '\0')
	{
		send_to_char(usage, ch);
		send_to_char("Enter which bot?\n\r", ch);
		return;
	}
	
	for (i = 0; i < BOT_TEMPLATE_NUMBER; i++)
	{
		if (is_name(arg_name, bot_template_table[i].name))
		{
			bot_id = i;
			break;
		}
	}
	
	if (bot_id < 0)
	{
		send_to_char("There is no such bot.  Enter 'BOT LIST' for a list.\n\r", ch);
		return;
	}
	
	for (i = 0; i < bot_entry_count; i++)
	{
		if (bot_status_table[i].id == bot_id)
		{
			send_to_char("That bot is already entered.\n\r", ch);
			return;
		}
	}
	
	/*
	 * Get bot level
	 */
	if (arg_level[0] == '\0')
	{
		send_to_char(usage, ch);
		send_to_char("What level is the bot?\n\r", ch);
		return;
	}
		
	if (is_number(arg_level))
	    	level = atoi(arg_level);
	
	if (level < 1 || level > 500)
	{
		send_to_char(usage, ch);
		send_to_char("Level must be between 1 and 500.\n\r", ch);
		return;
	}
	
	/*
	 * Get team if required
	 */
	if (tournament_type == TOURNAMENT_TYPE_TEAM
	    || tournament_type == TOURNAMENT_TYPE_TAG)
	{
		if (arg_team[0] == '\0')
		{
			send_to_char(usage, ch);
			send_to_char("What team should the bot enter?\n\r", ch);
			return;
		}
		
		for (i = 0; i < tournament_team_count; i++)
		{
			if (!str_cmp(arg_team, tournament_teams[i].keyword))
			{
				team = i;
				break;
			}
		}
		
		if (team < 0)
		{
			strcpy(buf, "Valid teams are:");
			
			for (i = 0; i < tournament_team_count; i++)
			{
				strcat(buf, " ");
				strcat(buf, tournament_teams[i].keyword);
				
				if (i < tournament_team_count - 1)
					strcat(buf, ",");
			}
			
			strcat(buf, ".\n\r");
			send_to_char(buf, ch);
			return;
		}
	}
	else
		team = 0;
	
	if (tournament_entrant_count[team] == TOURNAMENT_MAX_ENTRANTS)
	{
		send_to_char("There is no room for this bot.\n\r", ch);
		return;
	}
	
	/*
	 * Everything looks good; next create and init the bot object
	 */
	if (!(bot = create_bot(bot_id, level)))
	{
	    	send_to_char("Ack! Couldn't create the bot! (See log for details.)\n\r", ch);
		return;		
	}

	bot_status_table[bot_entry_count].id = bot_id;
	bot_status_table[bot_entry_count].ch = bot;
	bot_status_table[bot_entry_count].status = BOT_STATUS_WAITING;
	bot_status_table[bot_entry_count].timer = 0;
	bot_status_table[bot_entry_count].desperation = 0;
	bot_status_table[bot_entry_count].move_direction = -1;
	bot_status_table[bot_entry_count].move_distance = 0;
	bot_status_table[bot_entry_count].few_targets = FALSE;
	bot_status_table[bot_entry_count].target[0] = '\0';

	strncpy(bot_status_table[bot_entry_count].name, bot->name, 30);

       /*
	* Load any equipment
	*/
	for (i = 0; i < BOT_MAX_EQ_SLOTS; i++)
	{
		vnum = bot_template_table[bot_id].eq[i][0];
		slot = bot_template_table[bot_id].eq[i][1];
		
		if (vnum < 0)
		    	break;
		
		if (!(obj_index = get_obj_index(vnum)))
		{
			sprintf(buf, "bot has bad eq vnum: %d", vnum);
		    	log_bot(bot_entry_count, buf);
			continue;
		}
		
		if (slot < WEAR_LIGHT || slot > WEAR_RANGED_WEAPON)
		{
			sprintf(buf, "bot has bad eq slot: %d (vnum %d)", slot, vnum);
		    	log_bot(bot_entry_count, buf);
			continue;
		}
		
		if (!(obj = create_object(obj_index, bot->level)))
		{
			sprintf(buf, "unable to create object (vnum %d)", vnum);
		    	log_bot(bot_entry_count, buf);
			continue;
		}
		
		/* Fiddle with stats */
		set_obj_owner(obj, bot->name);
		REMOVE_BIT(obj->wear_flags, ITEM_TAKE);

		for (paf = obj->pIndexData->affected; paf; paf = paf->next)
		{
			if (paf->location == APPLY_HITROLL
			    || paf->location == APPLY_DAMROLL)
			{
				paf->modifier = bot->level / 4;
			}
		}
		
		if (obj->item_type == ITEM_WAND || obj->item_type == ITEM_STAFF)
		    	obj->value[0] = bot->level;
		
		obj_to_char(obj, bot);
		equip_char(bot, obj, slot);
		sprintf(buf, "equipping object (vnum %d, slot %d)", vnum, slot);
	    	log_bot(bot_entry_count, buf);
	}
		    
	log_bot(bot_entry_count, "entered in tournament");
	bot_entry_count++;
		
	/* 
	 * Update tournament stats 
	 */
	sprintf(tournament_entrants[team][tournament_entrant_count[team]].stats,
		"{W%-15s{x <%s %3d>",
		bot->name,
		"Bot",
		bot->level);
	
	tournament_entrants[team][tournament_entrant_count[team]].killer[0] = '\0';
	tournament_entrants[team][tournament_entrant_count[team]].status = TOURNAMENT_ENTRANT_ALIVE;
	tournament_entrants[team][tournament_entrant_count[team]].ch = bot;
	tournament_entrant_count[team]++;
	bot->tournament_team = team;
	
	if (tournament_team_count == 1)
		sprintf(buf, "%s has been entered in the tournament!", bot->name);
	else
		sprintf(buf, "%s has been entered in the tournament for the %s team!",
			bot->name,
			tournament_teams[team].name);
		
	tournament_message(buf, tournament_host);
	
	sprintf(buf, "Bot '%s' entered.\n\r", bot->name);
	send_to_char(buf, ch);
}


CHAR_DATA* create_bot (int id, int level)
{
	/*
	 * Create bot mobile object and set stats.  All bots have same vnum; level 
	 * and stats set directly here, based on data in bot_template_table.
	 */
	
       	char buf [MAX_STRING_LENGTH];
	CHAR_DATA *bot;
	MOB_INDEX_DATA *bot_index;
	
	if (id < 0 || id >= BOT_TEMPLATE_NUMBER)
	{
		sprintf(buf, "create_bot: invalid bot id: %d", id);
		log_string(buf);
		return NULL;
	}
	
	if (level < 1 || level > 500)
	{
		sprintf(buf, "create_bot: invalid bot level: %d", level);
		log_string(buf);
		return NULL;
	}
	
	if (!(bot_index = get_mob_index(BOT_VNUM)))
	{
		log_string("create_bot: couldn't create mob of vnum BOT_VNUM");
		return NULL;
	}
	
	bot = create_mobile(bot_index);
		
	bot->level 		= level;
	bot->alignment 		= bot_template_table[id].alignment;
	bot->affected_by	= bot_template_table[id].affect_flags;
	bot->body_form		= bot_template_table[id].body_form;
	bot->sex		= bot_template_table[id].sex;
	
	sprintf(buf, "%s is here.\n\r", bot_template_table[id].name);
	
	bot->long_descr		= str_dup(buf);
	bot->name 		= str_dup(bot_template_table[id].name);
	bot->short_descr	= str_dup(bot_template_table[id].name);
	bot->description 	= str_dup("Ever dance with the Devil?\n\r");
	
	bot->max_hit = level * 8
	    	       + number_range(level * level * 0.75,
				      level * level * 1.25);
	
	if (IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_HEALTH_BONUS))
	    	bot->max_hit *= 1.33;
	
	bot->hit = bot->max_hit;
	bot->hitroll = level * 1.2;
	bot->damroll = level * 1.2;

	if (IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_HITROLL_BONUS))
	    	bot->hitroll *= 1.5;

	if (IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_DAMROLL_BONUS))
	    	bot->damroll *= 1.5;

	bot->spec_fun = spec_lookup(bot_template_table[id].special);

	bot->act = (ACT_IS_NPC
		    | ACT_SENTINEL
		    | ACT_NO_QUEST
		    | ACT_NOCHARM
		    | ACT_NO_EXPERIENCE);

	SET_BIT(bot->affected_by, AFF_INFRARED);
	SET_BIT(bot->affected_by, AFF_DETECT_INVIS);
	SET_BIT(bot->affected_by, AFF_SANCTUARY);
	
	char_to_room(bot, get_room_index(ROOM_VNUM_ARENA));
	
	return bot;
}


void do_bot_remove (CHAR_DATA *ch, char *argument)
{
	/*
	 * Remove a bot from a tournament
	 */
	
	char buf [MAX_STRING_LENGTH];
	char bot_name [MAX_INPUT_LENGTH];
	int i, j;
	int bot_id = -1;
	
	if (IS_NPC(ch))
		return;
	
	if (tournament_status == TOURNAMENT_STATUS_NONE)
	{
		send_to_char("No tournament has been posted!\n\r", ch);
		return;	
	}
	
	if (ch != tournament_host)
	{
		send_to_char("You may not remove bots into this tournament.\n\r", ch);
		return;
	}
		
	if (tournament_status != TOURNAMENT_STATUS_POSTED)
	{
		send_to_char("You may not remove any bots at the moment.\n\r", ch);
		return;
	}
	
	argument = one_argument(argument, bot_name);
	
	if (bot_name[0] == '\0')
	{
		send_to_char("Remove which bot?\n\r", ch);
		return;
	}
	
	for (i = 0; i < BOT_TEMPLATE_NUMBER; i++)
	{
		if (is_name(bot_name, bot_template_table[i].name))
		{
			bot_id = i;
			break;
		}
	}
	
	if (bot_id < 0)
	{
		send_to_char("There is no such bot.  Enter 'BOT LIST' for a list.\n\r", ch);
		return;
	}
	
	for (i = 0; i < bot_entry_count; i++)
	{
		if (bot_status_table[i].id == bot_id)
		{
			log_bot(i, "removed from tournament");
			
			remove_from_tournament(bot_status_table[i].ch);
			
			for (j = i; j < bot_entry_count-1; j++)
				bot_status_table[j] = bot_status_table[j+1];
			
			extract_char(bot_status_table[j].ch, TRUE);
			bot_entry_count--;
			
			sprintf(buf, "%s has been removed from the tournament.",
				bot_template_table[bot_id].name);
			tournament_message(buf, tournament_host);
			
			send_to_char("Bot removed.\n\r", ch);
			return;
		}
	}
	
	send_to_char("That bot is not entered.\n\r", ch);
}


void log_bot (int num, char *text)
{
	char buf [MAX_STRING_LENGTH];
	
	if (num < 0 || num >= BOT_MAX_ENTRANTS)
	    	return;
	
	sprintf(buf, "[Bot] (%d '%s') %s",
		num,
		bot_status_table[num].name,
		text);
	log_string (buf);
}


void bot_update ()
{
	/*
	 * Control bot activity
	 */
	
	int i, j, k, l;
	int order [BOT_MAX_ENTRANTS];
	
	if (tournament_status != TOURNAMENT_STATUS_RUNNING
	    || tournament_countdown > 0
	    || bot_entry_count < 1)
		return;
	
	/* Randomise order of action with quick and dirty shuffle */
	for (i = 0; i < bot_entry_count; i++)
	    	order[i] = i;

	if (bot_entry_count > 1)
	{
		for (i = 0; i < bot_entry_count * 3; i++)
		{
			do
			{
				j = number_range(0, bot_entry_count-1);
				k = number_range(0, bot_entry_count-1);
			}
			while (k == j);

			l = order[j];
			order[j] = order[k];
			order[k] = l;
		}
	}
	
	/* Update bots */
	for (i = 0; i < bot_entry_count; i++)
	{
		/* check for weirdness */
		if (!bot_status_table[order[i]].ch
		    || bot_status_table[order[i]].ch->deleted
		    || !bot_status_table[order[i]].ch->in_room)
		{
			continue;
		}
		
		switch (bot_status_table[order[i]].status)
		{
		    case BOT_STATUS_WAITING:
			bot_status_table[order[i]].status = BOT_STATUS_ALIVE;
			break;
			
		    case BOT_STATUS_ALIVE:
			if (bot_status_table[order[i]].timer-- < 1)
			    	bot_think(order[i]);
			break;
		}
	}
}


void bot_think (int num)
{
	/*
	 * Bot 'AI' :P
	 */

	char buf [MAX_STRING_LENGTH];
	CHAR_DATA *bot;
	CHAR_DATA *victim;
	ROOM_INDEX_DATA *old_room;
	int id, type, i, j;
	int total_targets = 0;
	
	if (num < 0 || num >= bot_entry_count)
	    	return;
	
	bot = bot_status_table[num].ch;
	id = bot_status_table[num].id;
	bot_status_table[num].timer = bot_template_table[id].speed;
	type = bot_template_table[id].bot_type;
	
	/* Prepare for weirdness */
	if (!bot)
	    	return;

	/* Are we awake? */
	if (!IS_AWAKE(bot))
		return;
	
	/* Are we fighting? */
	if (bot->fighting)
	{
		/* Do we want to flee? */
		if (!IS_SET(tournament_options, TOURNAMENT_OPTION_NOFLEE)
		    && IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_WIMPY)
		    && bot->hit < bot->max_hit * 0.25
		    && !number_bits(2))
		{
			old_room = bot->in_room;
			do_flee(bot, "");
			
			if (bot->in_room != old_room)
			{
				bot_status_table[num].desperation = 0;
				log_bot(num, "fled");
			}
			
			return;
		}
		
		/* Otherwise keep fighting */
	    	return;
	}
		
	/* Do we need to resneak, rehide? */
	if (IS_SET(bot_template_table[id].affect_flags, AFF_SNEAK))
	    	SET_BIT(bot->affected_by, AFF_SNEAK);
	
	if (IS_SET(bot_template_table[id].affect_flags, AFF_HIDE))
	    	SET_BIT(bot->affected_by, AFF_HIDE);
	
	/* Check if we're desperate for targets */
	if (!bot_status_table[num].few_targets)
	{
		for (j = 0; j < tournament_team_count; j++)
		{
			for (i = 0; i < tournament_entrant_count[0]; i++)
			{
				if (tournament_team_count > 1
				    && j == bot->tournament_team)
				    continue;

				if (tournament_entrants[j][i].status == TOURNAMENT_ENTRANT_ALIVE
				    && tournament_entrants[j][i].ch != bot)
				    total_targets++;
			}
		}

		if (total_targets < 3) 
		{
			log_bot(num, "desperate for targets");
			bot_status_table[num].few_targets = TRUE;
		}
	}
		
	/* See if there's someone in the room we can kill */
	if ((victim = bot_get_room_target(num)))
	{
		bot_murder(num, victim);
		return;
	}
	
	/* Okay, are we hunting someone already? */
	
	/* Scan for potential targets to kill */
	if ((victim = bot_scan_for_target(num)))
	{	
		i = bot_status_table[num].move_direction;
		
		if (i < 0 || i > 5)
		{
			sprintf(buf, "bad move direction: %d", i);
			log_bot(num, buf);
			return;
		}
		
		/* Head after target */
		sprintf(buf, "heading after target '%s' (direction %d, distance %d)",
			victim->name,
			i,
			bot_status_table[num].move_distance);
		log_bot(num, buf);
		
		move_char(bot, i);
		
		/* If we've reached the target, shall we attack immediately? */
		if (bot->in_room == victim->in_room 
		    && (IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_OPENS_IMMEDIATELY)
			|| number_bits(1)))
		{
			bot_murder(num, victim);
		}
				
		return;
	}
	
	/* No target; do we want to move?  */
	if (type != BOT_TYPE_STATIC || bot_is_desperate(num))
	{
		i = bot_status_table[num].move_direction;
		j = bot_status_table[num].move_distance;
		old_room = bot->in_room;
		
		/* Are we moving already? */
		if (i >= 0 && i <= 5 && j > 0)
		{
			/* Attempt move */
			move_char(bot, i);
			
			if (bot->in_room != old_room)
			{
				bot_status_table[num].move_distance--;
				/* sprintf(buf, "continuing move (direction %d)", i);
				log_bot(num, buf); */
				return;
			}
		}
		
		/*
		 * Pick a direction to move in.
		 * 
		 * Hack: bots sometimes get stuck in the pit; head straight back up
		 * if we've just popped down into it.  We would have scanned for targets
		 * earlier in the function and headed for them.
		 */
		if (bot->in_room->vnum == 14239) /* Centre room in 3x3 pit */
		{
			i = 4;
			j = 1;
		}
		else
		{
			for (j = 100, i = -1; j; j--)
			{
				i = number_range(0, 5);

				if (bot->in_room->exit[i]
				    && bot->in_room->exit[i]->to_room)
				{
					j = number_range(1, 5);
					break;
				}
			}
		}
		
		if (i < 0)
		{
			log_bot(num, "ack, couldn't find a room to move into");
			return;
		}
		

		/* Attempt move */
		sprintf(buf, "moving (direction %d, distance %d)", i, j);
		log_bot(num, buf);

		move_char(bot, i);
		
		if (bot->in_room == old_room)
			i = -1;				
		
		bot_status_table[num].move_direction = i;
		bot_status_table[num].move_distance = j;
	}
	
	/* Become more desperate */
	if (bot_status_table[num].desperation < 20)
		bot_status_table[num].desperation++;
}

	    
CHAR_DATA* bot_get_room_target (int num)
{
	CHAR_DATA *bot;
	CHAR_DATA *vch;
	
	if (num < 0 || num >= bot_entry_count)
	    	return NULL;
	
	bot = bot_status_table[num].ch;

	/* Prepare for weirdness */
	if (!bot || !bot->in_room)
		return NULL;
	
	target_list_count = 0;
	
	/* Get potential targets in room */
	for (vch = bot->in_room->people; vch; vch = vch->next_in_room)
	{
		if (bot_consider_target(num, vch))
		{
			target_list[target_list_count].ch = vch;
			target_list[target_list_count].direction = -1;
			target_list[target_list_count].distance = 0;

			if (++target_list_count == BOT_MAX_TARGETS)
			    	break;
		}
	}
	
	/* No target */
	if (!target_list_count)
	    	return NULL;
	
	/* Choose a target from final list */
	return bot_choose_target(num);
}


void bot_murder (int num, CHAR_DATA *victim)
{
	char buf [MAX_STRING_LENGTH];
	CHAR_DATA *bot;
	int id;
	
	if (num < 0 || num >= bot_entry_count)
	    	return;
	
	bot = bot_status_table[num].ch;
	id = bot_status_table[num].id;

	/* Prepare for weirdness */
	if (!bot 
	    || !bot->in_room 
	    || !victim 
	    || !victim->in_room 
	    || bot->in_room != victim->in_room
	    || !IS_SET(bot->in_room->room_flags, ROOM_PLAYER_KILLER))
	    	return;
	
	sprintf(buf, "attacking target '%s'", victim->name);
	log_bot(num, buf);

	/* Become less desperate */
	bot_status_table[num].desperation -= 5;
	
	/* Launch opening attack */
	switch (bot_template_table[id].opening_attack)
	{
	    case BOT_OPENING_GOUGE:
		if (!victim->fighting && !IS_AFFECTED(victim, AFF_BLIND))
		{
			do_gouge(bot, victim->name);
			return;
		}
		break;
		
	    case BOT_OPENING_BACKSTAB:
		if (!victim->fighting && !victim->backstab)
		{
			do_backstab(bot, victim->name);
			return;
		}
		break;
		
	    case BOT_OPENING_LUNGE:
		if (!victim->fighting && !victim->backstab)
		{
			do_lunge(bot, victim->name);
			return;
		}
		break;
		
	    case BOT_OPENING_TRAP:
		if (!victim->fighting && !IS_AFFECTED(victim, AFF_HOLD))
		{
			do_trap(bot, victim->name);
			return;
		}
		break;

	    case BOT_OPENING_TRAP_BACKSTAB:
		if (!victim->fighting && !IS_AFFECTED(victim, AFF_HOLD))
		{
			do_trap(bot, victim->name);
			return;
		}

		if (!victim->fighting && !victim->backstab)
		{
			do_backstab(bot, victim->name);
			return;
		}
		
		break;
	}
	
	/* Plain murder if not fighting yet */
	if (bot->fighting != victim)
	    	do_murder(bot, victim->name);
}


bool bot_is_desperate (int num)
{
	if (num < 0 || num >= bot_entry_count)
	    	return FALSE;

	/* Desperate if desperation_timer too high */
	if (bot_status_table[num].desperation > 15)
	    	return TRUE;
	 
	/* Desperate if there are few targets */
	if (bot_status_table[num].few_targets)
	    	return TRUE;

	/* Desperate if we're trapped */
	if (IS_SET(bot_status_table[num].ch->affected_by, AFF_HOLD))
	    	return TRUE;

	/* Not desperate yet */
	return FALSE;
}


CHAR_DATA* bot_scan_for_target (int num)
{
	CHAR_DATA *bot;
	CHAR_DATA *vch;
	ROOM_INDEX_DATA *room;
	int id;
	unsigned int sight, i, j;

	if (num < 0 || num >= bot_entry_count)
	    	return NULL;
	
	bot = bot_status_table[num].ch;
	id = bot_status_table[num].id;
	
	/* Bots can scan at least one room deep */
	sight = UMAX(bot_template_table[id].sight, 1);

	/* Prepare for weirdness */
	if (!bot || !bot->in_room)
		return NULL;
	
	target_list_count = 0;
	
	/* 
	 * Scan in all directions (i) up to 'sight' rooms deep (j).
	 * Doors currently not handled (no doors in the Arena at the moment).
	 */
	for (i = 0; i < 6; i++)
	{
		room = bot->in_room;
		
		for (j = 0; j < sight; j++)
		{
			if (!room->exit[i]
			    || !(room = room->exit[i]->to_room))
			    	break;
	
			for (vch = room->people; vch; vch = vch->next_in_room)
			{
				if (bot_consider_target(num, vch))
				{
					target_list[target_list_count].ch = vch;
					target_list[target_list_count].distance = j+1;
					target_list[target_list_count].direction = i;

					if (++target_list_count == BOT_MAX_TARGETS)
					{
						j = sight;
						i = 6;
						break;
					}
				}
			}
		}
	}
	
	/* No target */
	if (target_list_count == 0)
	    	return NULL;
	
	/* Choose a target from final list */
	return bot_choose_target(num);
}


bool bot_consider_target (int num, CHAR_DATA *victim)
{
	CHAR_DATA *bot;
	int id;
	char buf [MAX_STRING_LENGTH];
	
	if (num < 0 || num >= bot_entry_count)
	    	return FALSE;
	
	bot = bot_status_table[num].ch;
	id = bot_status_table[num].id;

	/* Prepare for weirdness */
	if (!bot 
	    || !bot->in_room 
	    || !victim 
	    || !victim->in_room 
	    || !IS_SET(bot->in_room->room_flags, ROOM_PLAYER_KILLER))
	{
		return FALSE;
	}
		
	/* Ignore silly targets */
	if (victim->deleted
	    || bot == victim
	    || !can_see(bot, victim)
	    || (tournament_team_count > 1
		&& bot->tournament_team == victim->tournament_team)
	    || (!IS_NPC(victim) && victim->level > LEVEL_HERO)
	    || (IS_NPC(victim) && victim->pIndexData->vnum != BOT_VNUM))
	{
		return FALSE;
	}
	
	/* Include targetting restrictions unless we're desperate */
	if (!bot_is_desperate(num))
	{
		/* Check whether we'll attack over 7 levels up */
		if (victim->level > bot->level + 7
		    && !IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_ATTACKS_HIGH_TARGETS))
		    return FALSE;

		/* Check whether we're too wimpy to want to attack */
		if (victim->level >= bot->level
		    && bot->hit < bot->max_hit / 2
		    && IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_WIMPY))
		    return FALSE;

		/* Check whether we'll attack over 7 levels down */
		if (victim->level < bot->level - 7
		    && IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_IGNORE_EASY_TARGETS))
		    return FALSE;

		/* Check whether we'll attack fighting people */
		if (victim->fighting
		    && IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_STEALS_KILLS))
		    return FALSE;
	}
				
	/* Target looks good */
	sprintf(buf, "can see target '%s'", victim->name);
	log_bot(num, buf);
	
	return TRUE;
}

		
		
CHAR_DATA* bot_choose_target (int num)
{
	/*
	 * Determine who we attack from our final list
	 */
	CHAR_DATA *target;
	int id, i, score;
	int best = -1;
	int best_score = -1;
	
	if (num < 0 || num >= bot_entry_count)
	    	return NULL;
	
	id = bot_status_table[num].id;

	if (target_list_count < 1)
	    	return NULL;
	
	/* Attack a random target? */
	if (IS_SET(bot_template_table[id].bot_flags, BOT_FLAG_RANDOM_TARGET))
	{
		best = number_range(0, target_list_count - 1);
	}
	else
	{
		/* Score each target */
		for (i = 0; i < target_list_count; i++)
		{
			target = target_list[i].ch;
			score = 0;
		
			/* Nice if target is hurt */
			if (target->hit < target->max_hit * 0.1)
			    	score += 4;

			else if (target->hit < target->max_hit * 0.25)
			    	score += 2;
			
			else if (target->hit < target->max_hit * 0.5)
			    	score += 1;
			
			/* Good if victim isn't fighting */
			if (!target->fighting)
			    	score += 1;
			
			/* Check score */
			if (score > best_score || (score == best && number_bits(1)))
			{
				best_score = score;
				best = i;
			}
		}
	}
	
	/* We found our target */
	bot_status_table[num].move_direction = target_list[best].direction;
	bot_status_table[num].move_distance = target_list[best].distance;
	strncpy(bot_status_table[num].target, target_list[best].ch->name, 30);
	
	return target_list[best].ch;
}


/* EOF bot.c */
