/* **** NECROMANCER **** */

/* **** MANA CONTROL **** */

{&gsn_third_spell,	        &gsn_group_mana,	95,	PRE_NECRO},
{&gsn_third_spell,		&gsn_second_spell,	95,	PRE_NECRO},
{&gsn_third_spell,		&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_second_attack,		&gsn_necro_base,	40,	PRE_NECRO},

/* **** DEATH MAGIKS **** */

{&gsn_group_death,		&gsn_necro_base,	31,	PRE_NECRO},
{&gsn_harm,			&gsn_group_death,	50,	PRE_NECRO},
{&gsn_harm,			&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_animate_dead,		&gsn_group_death,	60,	PRE_NECRO},
{&gsn_animate_dead,		&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_feeblemind,		&gsn_group_death,	75,	PRE_NECRO},
{&gsn_feeblemind,               &gsn_necro_base,        30,     PRE_NECRO},

{&gsn_fear,			&gsn_group_death,	80,	PRE_NECRO},
{&gsn_fear,			&gsn_feeblemind,	75,	PRE_NECRO},

{&gsn_deter,			&gsn_group_death,	85,	PRE_NECRO},
{&gsn_deter,			&gsn_fear,		85,	PRE_NECRO},
{&gsn_deter,                    &gsn_necro_base,        30,     PRE_NECRO},


{&gsn_spiritwrack,		&gsn_group_death,	90,	PRE_NECRO},
{&gsn_spiritwrack,		&gsn_fear,		80,	PRE_NECRO},
{&gsn_spiritwrack,		&gsn_deter,		80,	PRE_NECRO},

{&gsn_hex,			&gsn_fear,		95,	PRE_NECRO},
{&gsn_hex,			&gsn_deter,		95,	PRE_NECRO},
{&gsn_hex,			&gsn_group_disease,	95,	PRE_NECRO},

/* **** DISEASE MAGIKS **** */
{&gsn_group_disease, 		&gsn_group_death,	80,	PRE_NECRO},
{&gsn_group_disease, 		&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_wither,			&gsn_group_disease,	80,	PRE_NECRO},
{&gsn_wither,			&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_poison,			&gsn_group_disease,	50,	PRE_NECRO},
{&gsn_poison,			&gsn_necro_base,	30,	PRE_NECRO},

{&gsn_entrapment,               &gsn_group_alteration,  90,     PRE_NECRO},
{&gsn_entrapment,               &gsn_faerie_fire,       95,     PRE_NECRO},
{&gsn_entrapment,               &gsn_necro_base,        50,     PRE_NECRO},

/* **** MANA CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_NECRO},
{&gsn_group_craft,              &gsn_necro_base,        30,     PRE_NECRO},

{&gsn_recharge_item,            &gsn_group_craft,       50,     PRE_NECRO},
{&gsn_recharge_item,            &gsn_necro_base,        30,     PRE_NECRO},

{&gsn_bladethirst,		&gsn_necro_base,	50,	PRE_NECRO},
{&gsn_bladethirst,		&gsn_group_craft,	90,	PRE_NECRO},

/* **** PROTECTIVE **** */

{&gsn_fireshield,		&gsn_group_protection,	90,	PRE_NECRO},
{&gsn_fireshield,		&gsn_necro_base,	30,	PRE_NECRO},
