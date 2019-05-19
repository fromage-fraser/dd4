/* CLERIC PRE REQS */

/* 86 PRE_REQS (excluding Alt @ 15:50 22/2 - Simon */

/* STARTING SKILLS */

{&gsn_cure_light,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_cause_light,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_detect_evil,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_detect_good,              &gsn_cleric_base,       30,     PRE_CLERIC},
{&gsn_detect_poison,		&gsn_cleric_base,	30,     PRE_CLERIC},
{&gsn_armor,			&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_continual_light,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_healing,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_disease,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_conjuration,	&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_divine,		&gsn_cleric_base,	30,	PRE_CLERIC},

/* **** GROUPS **** */
{&gsn_group_summoning,		&gsn_group_conjuration,	50,	PRE_CLERIC},
{&gsn_group_summoning,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_alteration,		&gsn_continual_light,	10,	PRE_CLERIC},
{&gsn_group_harmful,		&gsn_group_healing,	30,	PRE_CLERIC},
{&gsn_group_harmful,            &gsn_cleric_base,  	30,     PRE_CLERIC},
{&gsn_group_protection,		&gsn_armor,		10,	PRE_CLERIC},
{&gsn_group_protection,		&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_group_curative,		&gsn_group_disease,	30,	PRE_CLERIC},


/* **** LEVEL DEPENDENT SKILLS **** */

{&gsn_group_mana,		&gsn_cleric_base,	39,	PRE_CLERIC},
{&gsn_group_armed,		&gsn_cleric_base,	39,	PRE_CLERIC},
{&gsn_group_defense,		&gsn_cleric_base,	44,	PRE_CLERIC},

  /* **** ARMED COMBAT **** */

{&gsn_second_attack,		&gsn_group_armed,	50,	PRE_CLERIC},
{&gsn_second_attack,		&gsn_cleric_base,	30,	PRE_CLERIC},

/* **** DEFENSE **** */
{&gsn_shield_block,		&gsn_group_defense,	30,	PRE_CLERIC},
{&gsn_shield_block,		&gsn_cleric_base,	30,	PRE_CLERIC},

/* **** GROUP HEALING **** */

{&gsn_cure_serious,		&gsn_group_healing,	30,	PRE_CLERIC},
{&gsn_cure_serious,		&gsn_cure_light,	30,	PRE_CLERIC},
{&gsn_cure_serious,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_cure_critical,		&gsn_group_healing,	45,	PRE_CLERIC},
{&gsn_cure_critical,		&gsn_cure_serious,	30,	PRE_CLERIC},
{&gsn_cure_critical,		&gsn_cure_light,	50,	PRE_CLERIC},
{&gsn_cure_critical,     	&gsn_cleric_base,       30,     PRE_CLERIC},

{&gsn_heal,			&gsn_group_healing,	60,	PRE_CLERIC},
{&gsn_heal,			&gsn_cure_serious,	50,	PRE_CLERIC},
{&gsn_heal,			&gsn_cure_critical,	50,	PRE_CLERIC},
{&gsn_heal,             	&gsn_cleric_base,       30,     PRE_CLERIC},

/* **** GROUP MANA **** */
{&gsn_blink,                    &gsn_cleric_base,       30,     PRE_CLERIC},

/* **** GROUP HARMFUL **** */


{&gsn_cause_serious,		&gsn_group_harmful,	30,	PRE_CLERIC},
{&gsn_cause_serious,		&gsn_cause_light,	30,	PRE_CLERIC},

{&gsn_cause_critical,		&gsn_group_harmful,	45,	PRE_CLERIC},
{&gsn_cause_critical,		&gsn_cause_serious,	30,	PRE_CLERIC},
{&gsn_cause_critical,		&gsn_cause_light,	50,	PRE_CLERIC},

{&gsn_harm,			&gsn_group_harmful,	60,	PRE_CLERIC},
{&gsn_harm,			&gsn_cause_critical,	30,	PRE_CLERIC},
{&gsn_harm,			&gsn_cause_serious,	50,	PRE_CLERIC},

/* **** CURATIVE AND DISEASE GROUPS **** */

{&gsn_refresh,			&gsn_group_curative,	20,	PRE_CLERIC},

{&gsn_cure_blindness,		&gsn_group_curative,	30,	PRE_CLERIC},
{&gsn_cure_blindness,		&gsn_blindness,		30,	PRE_CLERIC},

{&gsn_cure_poison,		&gsn_group_curative,	40,	PRE_CLERIC},
{&gsn_cure_poison,		&gsn_poison,		30,	PRE_CLERIC},

{&gsn_remove_curse,		&gsn_group_curative,	50,	PRE_CLERIC},
{&gsn_remove_curse,		&gsn_curse,		30,	PRE_CLERIC},

{&gsn_blindness,		&gsn_group_disease,	20,	PRE_CLERIC},
{&gsn_poison,			&gsn_group_disease,	35,	PRE_CLERIC},
{&gsn_curse,			&gsn_group_disease,	50,	PRE_CLERIC},



 /* **** GROUP DIVINATION **** */

{&gsn_group_divination,		&gsn_cleric_base,      	30,	PRE_CLERIC},

/* **** CONJURATION **** */

{&gsn_create_water,		&gsn_group_conjuration,	20,	PRE_CLERIC},
{&gsn_create_water,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_create_spring,		&gsn_create_water,	50,	PRE_CLERIC},
{&gsn_create_spring,		&gsn_group_conjuration, 50,	PRE_CLERIC},

{&gsn_faerie_fire,		&gsn_group_conjuration,	30,	PRE_CLERIC},
{&gsn_faerie_fire,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_create_food,		&gsn_group_conjuration,	40,	PRE_CLERIC},
{&gsn_create_food,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_faerie_fog,		&gsn_group_conjuration,	50,	PRE_CLERIC},
{&gsn_faerie_fog,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_brew,			&gsn_group_conjuration,	75,	PRE_CLERIC},
{&gsn_brew,			&gsn_cleric_base,	60,	PRE_CLERIC},


/* **** SUMMONING **** */
{&gsn_summon,			&gsn_cleric_base,	30,	PRE_CLERIC},
{&gsn_call_lightning,		&gsn_cleric_base,	30,	PRE_CLERIC},

/* **** PROTECTION **** */

{&gsn_protection,		&gsn_group_protection,	30,	PRE_CLERIC},
{&gsn_protection,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_sanctuary,		&gsn_group_protection,	45,	PRE_CLERIC},
{&gsn_sanctuary,		&gsn_protection,	40,	PRE_CLERIC},

{&gsn_dispel_magic,		&gsn_group_protection, 	75,	PRE_CLERIC},
{&gsn_dispel_magic,		&gsn_cleric_base,	60,	PRE_CLERIC},

{&gsn_mass_sanctuary,		&gsn_group_protection, 	95, 	PRE_CLERIC},
{&gsn_mass_sanctuary,		&gsn_cleric_base,	60,	PRE_CLERIC},
{&gsn_mass_sanctuary,		&gsn_sanctuary, 	90,	PRE_CLERIC},

{&gsn_fireshield,		&gsn_group_protection,	90,	PRE_CLERIC},
{&gsn_fireshield,		&gsn_cleric_base,	60,	PRE_CLERIC},
{&gsn_fireshield,		&gsn_flamestrike,	75,	PRE_CLERIC},

{&gsn_globe,			&gsn_group_protection,	100,	PRE_CLERIC},
{&gsn_globe,			&gsn_cleric_base,	60,	PRE_CLERIC},

/* **** ADVANCED HEALING **** */

{&gsn_group_advanced_heal,	&gsn_cleric_base,		60,	PRE_CLERIC},

{&gsn_mass_heal,		&gsn_group_advanced_heal,	30,	PRE_CLERIC},
{&gsn_mass_heal,		&gsn_heal,			75,	PRE_CLERIC},

{&gsn_power_heal,		&gsn_group_advanced_heal,	60,	PRE_CLERIC},
{&gsn_power_heal,		&gsn_heal,			90,	PRE_CLERIC},

{&gsn_mass_power_heal,		&gsn_group_advanced_heal,	95,	PRE_CLERIC},
{&gsn_mass_power_heal,		&gsn_power_heal,		95,	PRE_CLERIC},
{&gsn_mass_power_heal,		&gsn_mass_heal,			95,	PRE_CLERIC},

{&gsn_complete_healing,		&gsn_power_heal,		100,	PRE_CLERIC},
{&gsn_complete_healing,		&gsn_group_advanced_heal,	100,	PRE_CLERIC},
{&gsn_complete_healing,		&gsn_cleric_base,		60,	PRE_CLERIC},

/* **** ALTERATION **** */

{&gsn_invis,			&gsn_group_alteration,	40,	PRE_CLERIC},

{&gsn_mass_invis,		&gsn_group_alteration,	70,	PRE_CLERIC},
{&gsn_mass_invis,		&gsn_invis,		60,	PRE_CLERIC},
{&gsn_mass_invis,		&gsn_cleric_base,	30,	PRE_CLERIC},

{&gsn_knock,			&gsn_group_alteration,	65,	PRE_CLERIC},
{&gsn_knock,			&gsn_cleric_base,	30,	PRE_CLERIC},

/* **** DIVINE POWER **** */

{&gsn_bless,			&gsn_group_divine,	 20,	PRE_CLERIC},
{&gsn_earthquake,		&gsn_group_divine,	 80,	PRE_CLERIC},
{&gsn_dispel_evil,		&gsn_group_divine,	 40,	PRE_CLERIC},
{&gsn_flamestrike,		&gsn_group_divine,	 50,	PRE_CLERIC},

{&gsn_holy_word,		&gsn_group_divine,	 60,	PRE_CLERIC},
{&gsn_holy_word,		&gsn_dispel_evil,	 60,	PRE_CLERIC},

{&gsn_unholy_word,		&gsn_group_divine,	 75,    PRE_CLERIC},

{&gsn_bless_weapon,		&gsn_group_divine,	 60,	PRE_CLERIC},
{&gsn_bless_weapon,		&gsn_bless,		 60,	PRE_CLERIC},
{&gsn_bless_weapon,		&gsn_cleric_base,	 60,	PRE_CLERIC},

{&gsn_wrath_of_god,		&gsn_group_divine,	 75,	PRE_CLERIC},
{&gsn_wrath_of_god,		&gsn_dispel_evil,	 60,	PRE_CLERIC},
{&gsn_wrath_of_god,		&gsn_cleric_base,	 60,	PRE_CLERIC},

{&gsn_prayer,			&gsn_group_divine,	 90,	PRE_CLERIC},
{&gsn_prayer,			&gsn_cleric_base,	 60,	PRE_CLERIC},
{&gsn_prayer,			&gsn_bless,		 90,	PRE_CLERIC},

/* **** Riding **** */

{&gsn_group_riding,		&gsn_cleric_base,	59,	PRE_CLERIC},
