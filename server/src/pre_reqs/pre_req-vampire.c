 
{ &gsn_group_vampyre,           &gsn_vampire_base,       	30, 	PRE_VAMPIRE},

{ &gsn_fast_healing,		&gsn_vampire_base,		50,	PRE_VAMPIRE},

{ &gsn_climb,			&gsn_vampire_base,		35,	PRE_VAMPIRE},
{ &gsn_hunt,			&gsn_vampire_base,		50,	PRE_VAMPIRE},
{ &gsn_advanced_consider,	&gsn_hunt,			70,	PRE_VAMPIRE},

{ &gsn_fear,			&gsn_group_vampyre,		70,	PRE_VAMPIRE},
{ &gsn_fear,			&gsn_vampire_base,		40,	PRE_VAMPIRE},

{ &gsn_resist_toxin,		&gsn_group_vampyre,		85,	PRE_VAMPIRE},
{ &gsn_resist_toxin,		&gsn_fear,			80,	PRE_VAMPIRE},
{ &gsn_resist_magic,		&gsn_group_vampyre,		85,	PRE_VAMPIRE},
{ &gsn_resist_magic,		&gsn_resist_toxin,		80,	PRE_VAMPIRE},

{ &gsn_mist_walk,		&gsn_group_vampyre,		85,	PRE_VAMPIRE},

{ &gsn_stalk,			&gsn_shadow_form,		80,	PRE_VAMPIRE},
{ &gsn_stalk,			&gsn_group_vampyre,		80,	PRE_VAMPIRE},
{ &gsn_stalk,			&gsn_vampire_base,		30,	PRE_VAMPIRE},

{ &gsn_transfix,		&gsn_group_vampyre,		30,	PRE_VAMPIRE},

{ &gsn_suck,			&gsn_group_vampyre,		50,	PRE_VAMPIRE},

{ &gsn_lunge,			&gsn_suck,			70,	PRE_VAMPIRE},
{ &gsn_lunge,			&gsn_group_vampyre,		70,	PRE_VAMPIRE},

{ &gsn_aura_of_fear,		&gsn_group_vampyre,		85,	PRE_VAMPIRE},

{ &gsn_double_lunge,		&gsn_lunge,			95,	PRE_VAMPIRE},
{ &gsn_double_lunge,		&gsn_group_vampyre,		90,	PRE_VAMPIRE},

{ &gsn_shadow_form,		&gsn_group_vampyre,		85,	PRE_VAMPIRE},
{ &gsn_shadow_form,		&gsn_vampire_base,		30,	PRE_VAMPIRE},

{ &gsn_deter,			&gsn_fear,      		90,	PRE_VAMPIRE},
{ &gsn_deter,			&gsn_vampire_base,		35,	PRE_VAMPIRE},

{&gsn_group_armed,              &gsn_vampire_base,      30,     PRE_VAMPIRE},
{&gsn_second_attack,            &gsn_group_armed,       50,     PRE_VAMPIRE},
{&gsn_enhanced_hit,      	&gsn_group_armed,       95,	PRE_VAMPIRE},
{&gsn_enhanced_hit,  	        &gsn_vampire_base,      30,     PRE_VAMPIRE},
 
{&gsn_disarm,			&gsn_group_armed,	75,	PRE_VAMPIRE},
{&gsn_disarm,			&gsn_vampire_base,	30,	PRE_VAMPIRE},

{&gsn_third_attack,             &gsn_group_armed,       70,     PRE_VAMPIRE},
{&gsn_third_attack,             &gsn_second_attack,     70,     PRE_VAMPIRE},
{&gsn_third_attack,             &gsn_vampire_base,      30,     PRE_VAMPIRE},
 
{&gsn_fourth_attack,            &gsn_group_armed,       85,	PRE_VAMPIRE},
{&gsn_fourth_attack,            &gsn_second_attack,     85,	PRE_VAMPIRE},
{&gsn_fourth_attack,            &gsn_third_attack,      85,	PRE_VAMPIRE},
{&gsn_fourth_attack,            &gsn_vampire_base,      30,     PRE_VAMPIRE},
 
{&gsn_dual,                     &gsn_group_armed,       90,	PRE_VAMPIRE},
{&gsn_dual,                     &gsn_third_attack,      90,	PRE_VAMPIRE},
{&gsn_dual,                     &gsn_fourth_attack,     90,	PRE_VAMPIRE},
{&gsn_dual,                     &gsn_vampire_base,      50,     PRE_VAMPIRE},

