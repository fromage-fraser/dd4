/* **** DRUID **** */

/* Druid pre-reqs - edited 15/6 - matt - total numer :-   36  */

{&gsn_group_nature,             &gsn_druid_base,        30,     PRE_DRUID},

/* **** NATURE **** */

{&gsn_bark_skin,                &gsn_group_nature,      40,     PRE_DRUID},

{&gsn_breathe_water,            &gsn_group_nature,      40,     PRE_DRUID},

{&gsn_wither,                   &gsn_group_nature,      55,     PRE_DRUID},
{&gsn_wither,                   &gsn_bark_skin,         55,     PRE_DRUID},

{&gsn_moonray,                  &gsn_group_nature,      80,     PRE_DRUID},
{&gsn_sunray,                   &gsn_group_nature,      85,     PRE_DRUID},
{&gsn_sunray,                   &gsn_moonray,           80,     PRE_DRUID},

{&gsn_natures_fury,             &gsn_group_nature,      100,    PRE_DRUID},
{&gsn_natures_fury,             &gsn_sunray,            95,     PRE_DRUID},

/* **** MANA CONTROL **** */

{&gsn_third_spell,              &gsn_group_mana,        95,     PRE_DRUID},
{&gsn_third_spell,              &gsn_second_spell,      95,     PRE_DRUID},
{&gsn_third_spell,              &gsn_druid_base,        30,     PRE_DRUID},

/* **** MANA CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_DRUID},
{&gsn_group_craft,              &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_scribe,                   &gsn_group_craft,       50,     PRE_DRUID},
{&gsn_scribe,                   &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_brew,                     &gsn_group_craft,       90,     PRE_DRUID},
{&gsn_brew,                     &gsn_druid_base,        30,     PRE_DRUID},

/* **** DESTRUCTION **** */

{&gsn_group_destruction,        &gsn_group_divine,      90,     PRE_DRUID},
{&gsn_group_destruction,        &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_firestorm,                &gsn_group_destruction, 50,     PRE_DRUID},
{&gsn_firestorm,                &gsn_flamestrike,       90,     PRE_DRUID},
{&gsn_firestorm,                &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_meteor_storm,             &gsn_group_destruction, 95,     PRE_DRUID},
{&gsn_meteor_storm,             &gsn_firestorm,         90,     PRE_DRUID},
{&gsn_meteor_storm,             &gsn_druid_base,        30,     PRE_DRUID},

/* **** MAJOR PROTECTION */

{&gsn_group_majorp,             &gsn_druid_base,        30,     PRE_DRUID},
{&gsn_group_majorp,             &gsn_group_protection,  80,     PRE_DRUID},

{&gsn_fireshield,               &gsn_group_majorp,      30,     PRE_DRUID},
{&gsn_fireshield,               &gsn_flamestrike,       80,     PRE_DRUID},
{&gsn_fireshield,               &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_mass_invis,               &gsn_druid_base,        60,     PRE_DRUID},
{&gsn_mass_invis,               &gsn_invis,             90,     PRE_DRUID},
{&gsn_mass_invis,               &gsn_group_majorp,      30,     PRE_DRUID},

/* **** ALTERATION **** */

{&gsn_entrapment,               &gsn_group_alteration,  90,     PRE_DRUID},
{&gsn_entrapment,               &gsn_faerie_fire,       95,     PRE_DRUID},
{&gsn_entrapment,               &gsn_druid_base,        30,     PRE_DRUID},

/* **** HEALING **** */

{&gsn_power_heal,               &gsn_group_healing,     90,     PRE_DRUID},
{&gsn_power_heal,               &gsn_druid_base,        30,     PRE_DRUID},

/* Resistances */

{&gsn_group_resistance,         &gsn_druid_base,        30,     PRE_DRUID},

{&gsn_resist_acid,              &gsn_group_resistance,  40,     PRE_DRUID},
{&gsn_resist_lightning,         &gsn_group_resistance,  65,     PRE_DRUID},
{&gsn_resist_cold,              &gsn_group_resistance,  90,     PRE_DRUID},
{&gsn_resist_heat,              &gsn_group_resistance,  95,     PRE_DRUID},




