 
/* Last modified 1/6 - Mat */
 
/* Total Pre_Reqs == 33  */
 
/* Starting Skills - have decided what worldly is going to be yet?*/
{ &gsn_group_lycanthropy,       &gsn_werewolf_base,	30,    	PRE_WEREWOLF},

{ &gsn_howl,			&gsn_form_wolf,		50,	PRE_WEREWOLF},

{ &gsn_form_wolf,		&gsn_werewolf_base,	30,	PRE_WEREWOLF},

{ &gsn_fast_healing,		&gsn_werewolf_base,	35,	PRE_WEREWOLF},

{ &gsn_climb,			&gsn_werewolf_base,	31,	PRE_WEREWOLF},
{ &gsn_hunt,			&gsn_werewolf_base,	30,	PRE_WEREWOLF},
{ &gsn_hunt,			&gsn_climb,		50,	PRE_WEREWOLF},
{ &gsn_advanced_consider,	&gsn_hunt,		70,	PRE_WEREWOLF},

{ &gsn_fear,			&gsn_group_lycanthropy,	70,	PRE_WEREWOLF},
{ &gsn_fear,			&gsn_werewolf_base,	40,	PRE_WEREWOLF},

{ &gsn_resist_toxin,		&gsn_group_lycanthropy,	75,	PRE_WEREWOLF},
{ &gsn_resist_magic,		&gsn_group_lycanthropy,	85,	PRE_WEREWOLF},
{ &gsn_resist_magic,		&gsn_resist_toxin,	80,	PRE_WEREWOLF},

{ &gsn_astral_sidestep,		&gsn_group_lycanthropy,	90,	PRE_WEREWOLF},
{ &gsn_astral_sidestep,		&gsn_hunt,		85,	PRE_WEREWOLF},

{ &gsn_gaias_warning,		&gsn_group_lycanthropy,	90,	PRE_WEREWOLF},
{ &gsn_gaias_warning,		&gsn_astral_sidestep,	90,	PRE_WEREWOLF},

{ &gsn_summon_avatar,		&gsn_group_lycanthropy,	95,	PRE_WEREWOLF},
{ &gsn_summon_avatar,		&gsn_astral_sidestep,	95,	PRE_WEREWOLF},
{ &gsn_summon_avatar,		&gsn_hunt,		95,	PRE_WEREWOLF},

{ &gsn_rage,			&gsn_group_lycanthropy, 80,	PRE_WEREWOLF},

{&gsn_group_armed,		&gsn_werewolf_base,	30,	PRE_WEREWOLF},
{&gsn_second_attack,            &gsn_group_armed,       50,     PRE_WEREWOLF},
{&gsn_enhanced_hit,          &gsn_group_armed,       	80,	PRE_WEREWOLF},
{&gsn_enhanced_hit,          &gsn_werewolf_base,      	30,	PRE_WEREWOLF},
 
{&gsn_third_attack,             &gsn_group_armed,       70,     PRE_WEREWOLF},
{&gsn_third_attack,             &gsn_second_attack,     70,     PRE_WEREWOLF},
{&gsn_third_attack,             &gsn_werewolf_base,      30,     PRE_WEREWOLF},
 
{&gsn_fourth_attack,            &gsn_group_armed,       90,     PRE_WEREWOLF},
{&gsn_fourth_attack,            &gsn_second_attack,     85,	PRE_WEREWOLF},
{&gsn_fourth_attack,            &gsn_third_attack,      85,	PRE_WEREWOLF},
{&gsn_fourth_attack,            &gsn_werewolf_base,     30,	PRE_WEREWOLF},
 
{&gsn_dual,                     &gsn_group_armed,       92,	PRE_WEREWOLF},
{&gsn_dual,                     &gsn_fourth_attack,     90,	PRE_WEREWOLF},
{&gsn_dual,                     &gsn_werewolf_base,      30,     PRE_WEREWOLF},

{&gsn_group_defense,		&gsn_werewolf_base,	45,	PRE_WEREWOLF},

{&gsn_dodge,			&gsn_group_defense,	80,	PRE_WEREWOLF},
{&gsn_dodge,			&gsn_werewolf_base,	30,	PRE_WEREWOLF},

{&gsn_parry,			&gsn_group_defense,	80,	PRE_WEREWOLF},
{&gsn_parry,			&gsn_werewolf_base,	30,	PRE_WEREWOLF},

{&gsn_wolfbite,                 &gsn_form_wolf,         50,     PRE_WEREWOLF},

{&gsn_ravage,                   &gsn_form_wolf,         70,     PRE_WEREWOLF},
{&gsn_ravage,                   &gsn_wolfbite,          50,     PRE_WEREWOLF},
