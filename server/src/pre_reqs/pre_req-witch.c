/* **** WITCH PRE_REQS **** */
{&gsn_second_attack,            &gsn_witch_base,        40,     PRE_WITCH},

{&gsn_group_summoning,          &gsn_witch_base,        31,     PRE_WITCH},

{&gsn_summon_familiar,          &gsn_group_summoning,   50,     PRE_WITCH},
{&gsn_summon_familiar,          &gsn_witch_base,        30,     PRE_WITCH},

/* **** MANA CONTROL **** */

{&gsn_third_spell,              &gsn_group_mana,        95,     PRE_WITCH},
{&gsn_third_spell,              &gsn_witch_base,        30,     PRE_WITCH},
{&gsn_third_spell,              &gsn_second_spell,      95,     PRE_WITCH},

/* **** MANA CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_WITCH},
{&gsn_group_craft,              &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_brew,                     &gsn_group_craft,       50,     PRE_WITCH},
{&gsn_brew,                     &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_scribe,                   &gsn_group_craft,       85,     PRE_WITCH},
{&gsn_scribe,                   &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_recharge_item,            &gsn_group_craft,       95,     PRE_WITCH},
{&gsn_recharge_item,            &gsn_witch_base,        30,     PRE_WITCH},

/* **** HUNTING **** */

{&gsn_group_hunting,            &gsn_witch_base,        40,     PRE_WITCH},

{&gsn_hunt,                     &gsn_group_hunting,     50,     PRE_WITCH},
{&gsn_hunt,                     &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_entrapment,               &gsn_group_hunting,     95,     PRE_WITCH},
{&gsn_entrapment,               &gsn_witch_base,        30,     PRE_WITCH},

/* **** MENTAL DEFENSE **** */

{&gsn_globe,                    &gsn_witch_base,        30,     PRE_WITCH},
{&gsn_globe,                    &gsn_group_mentald,     100,    PRE_WITCH},

{&gsn_fireshield,               &gsn_witch_base,        30,     PRE_WITCH},
{&gsn_fireshield,               &gsn_group_mentald,     90,     PRE_WITCH},

/* **** DEATH **** */

{&gsn_group_death,              &gsn_witch_base,        31,     PRE_WITCH},

{&gsn_animate_dead,             &gsn_group_death,       90,     PRE_WITCH},
{&gsn_animate_dead,             &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_fear,                     &gsn_group_death,       90,     PRE_WITCH},
{&gsn_fear,                     &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_steal_soul,               &gsn_group_death,       100,    PRE_WITCH},
{&gsn_steal_soul,               &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_hex,                      &gsn_group_death,       95,     PRE_WITCH},
{&gsn_hex,                      &gsn_witch_base,        30,     PRE_WITCH},

{&gsn_wither,                   &gsn_group_death,       90,     PRE_WITCH},
{&gsn_wither,                   &gsn_witch_base,        30,     PRE_WITCH},

/* **** POST-70 SKILLS **** */
{&gsn_resist_acid,              &gsn_globe,             100,    PRE_WITCH},
{&gsn_resist_acid,              &gsn_witch_base,         70,    PRE_WITCH},

{&gsn_resist_cold,              &gsn_resist_acid,       100,    PRE_WITCH},
{&gsn_resist_cold,              &gsn_witch_base,         70,    PRE_WITCH},

{&gsn_nausea,                   &gsn_fear,              100,    PRE_WITCH},
{&gsn_nausea,                   &gsn_witch_base,         70,    PRE_WITCH},

{&gsn_resist_lightning,         &gsn_resist_cold,       100,    PRE_WITCH},
{&gsn_resist_lightning,         &gsn_witch_base,         80,    PRE_WITCH},

{&gsn_confusion,                &gsn_nausea,            100,    PRE_WITCH},
{&gsn_confusion,                &gsn_witch_base,         80,    PRE_WITCH},

{&gsn_decay,                    &gsn_wither,            100,    PRE_WITCH},
{&gsn_decay,                    &gsn_witch_base,         80,    PRE_WITCH},

{&gsn_psychometry,              &gsn_scribe,            100,    PRE_WITCH},
{&gsn_psychometry,              &gsn_witch_base,         90,    PRE_WITCH},

{&gsn_fleshrot,                 &gsn_decay,             100,    PRE_WITCH},
{&gsn_fleshrot,                 &gsn_witch_base,         90,    PRE_WITCH},