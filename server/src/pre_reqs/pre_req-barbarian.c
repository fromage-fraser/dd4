/* **** BARBARIAN SKILLS **** */
{&gsn_fast_healing,		&gsn_barbarian_base,	40,	PRE_BARBARIAN},

{&gsn_group_knowledge,		&gsn_barbarian_base,	31,	PRE_BARBARIAN},

{&gsn_lore,			&gsn_group_knowledge,	40,	PRE_BARBARIAN},
{&gsn_lore,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_bash,			&gsn_group_knowledge,	70,	PRE_BARBARIAN},
{&gsn_bash,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_berserk,			&gsn_group_knowledge,	75,	PRE_BARBARIAN},
{&gsn_berserk,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_hunt,			&gsn_group_knowledge,	80,	PRE_BARBARIAN},
{&gsn_hunt,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_advanced_consider,	&gsn_group_knowledge,	90,	PRE_BARBARIAN},
{&gsn_advanced_consider,	&gsn_barbarian_base,	30,	PRE_BARBARIAN},	

/* **** METAL **** */

{&gsn_group_metal,		&gsn_group_knowledge,	85,	PRE_BARBARIAN},
{&gsn_group_metal,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_forge,			&gsn_group_metal,	60,	PRE_BARBARIAN},
{&gsn_forge,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_sharpen,			&gsn_group_metal,	90,	PRE_BARBARIAN},
{&gsn_sharpen,			&gsn_forge,		90,	PRE_BARBARIAN},
{&gsn_sharpen,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

/* **** ARMED COMBAT **** */

{&gsn_enhanced_damage,		&gsn_group_armed,	80,	PRE_BARBARIAN},
{&gsn_enhanced_damage,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_fourth_attack,		&gsn_group_armed,	85,	PRE_BARBARIAN},
{&gsn_fourth_attack,		&gsn_third_attack,	85,	PRE_BARBARIAN},
{&gsn_fourth_attack,		&gsn_second_attack,	85,	PRE_BARBARIAN},
{&gsn_fourth_attack,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_dual,			&gsn_group_armed,	92,	PRE_BARBARIAN},
{&gsn_dual,			&gsn_fourth_attack,	90,	PRE_BARBARIAN},
{&gsn_dual,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

/* **** ADVANCED COMBAT **** */

{&gsn_group_advcombat,		&gsn_group_armed,	95,	PRE_BARBARIAN},
{&gsn_group_advcombat,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_focus,			&gsn_group_advcombat,	50,	PRE_BARBARIAN},
{&gsn_focus,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_whirlwind,		&gsn_group_advcombat,	60,	PRE_BARBARIAN},
{&gsn_whirlwind,		&gsn_dual,		95,	PRE_BARBARIAN},
{&gsn_whirlwind,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

/* **** UNARMED COMBAT **** */
{&gsn_group_unarmed,		&gsn_barbarian_base,	31,	PRE_BARBARIAN},

{&gsn_headbutt,			&gsn_group_unarmed,	50,	PRE_BARBARIAN},
{&gsn_headbutt,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_gouge,			&gsn_group_unarmed,	80,	PRE_BARBARIAN},
{&gsn_gouge,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_push,			&gsn_group_unarmed,	85,	PRE_BARBARIAN},
{&gsn_push,			&gsn_barbarian_base,	30,	PRE_BARBARIAN},

{&gsn_second_headbutt,		&gsn_group_unarmed,	85,	PRE_BARBARIAN},
{&gsn_second_headbutt,		&gsn_headbutt,		85,	PRE_BARBARIAN},
{&gsn_second_headbutt,		&gsn_barbarian_base,	30,	PRE_BARBARIAN},

