/* MAGE PRE REQS */

/* **** STARTING SKILLS **** */

{&gsn_detect_evil,              &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_detect_good,              &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_magic_missile,            &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_group_illusion,           &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_continual_light,          &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_summon_familiar,          &gsn_mage_base,         30,     PRE_MAGE},
{&gsn_group_protection,         &gsn_mage_base,         30,     PRE_MAGE},

/* **** GROUPS..... **** */

{&gsn_group_evocation,          &gsn_magic_missile,     10,     PRE_MAGE},
{&gsn_group_alteration,         &gsn_continual_light,   10,     PRE_MAGE},
{&gsn_group_summoning,          &gsn_summon_familiar,   10,     PRE_MAGE},

/* **** LEVEL DEPENDENT SKILLS **** */
{&gsn_group_enchant,            &gsn_mage_base,         39,     PRE_MAGE},
{&gsn_group_mana,               &gsn_mage_base,         34,     PRE_MAGE},

/* **** GROUP EVOCATION **** */
{&gsn_chill_touch,              &gsn_group_evocation,   30,     PRE_MAGE},
{&gsn_burning_hands,            &gsn_group_evocation,   45,     PRE_MAGE},
{&gsn_shocking_grasp,           &gsn_group_evocation,   60,     PRE_MAGE},

{&gsn_lightning_bolt,           &gsn_group_evocation,   65,     PRE_MAGE},
{&gsn_lightning_bolt,           &gsn_shocking_grasp,    50,     PRE_MAGE},

{&gsn_colour_spray,             &gsn_group_evocation,   70,     PRE_MAGE},
{&gsn_colour_spray,             &gsn_group_illusion,    40,     PRE_MAGE},

{&gsn_fireball,                 &gsn_group_evocation,   75,     PRE_MAGE},
{&gsn_acid_blast,               &gsn_group_evocation,   80,     PRE_MAGE},

{&gsn_chain_lightning,          &gsn_group_evocation,   85,     PRE_MAGE},
{&gsn_chain_lightning,          &gsn_lightning_bolt,    80,     PRE_MAGE},

{&gsn_prismatic_spray,          &gsn_group_evocation,   100,    PRE_MAGE},
{&gsn_prismatic_spray,          &gsn_mage_base,         60,     PRE_MAGE},

/* **** GROUP DIVINATION **** */

{&gsn_group_divination,         &gsn_mage_base,         30,     PRE_MAGE},

/* **** GROUP PROTECTION **** */

{&gsn_armor,                    &gsn_group_protection,  30,     PRE_MAGE},

{&gsn_shield,                   &gsn_armor,             60,     PRE_MAGE},
{&gsn_shield,                   &gsn_group_protection,  60,     PRE_MAGE},
{&gsn_shield,                   &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_stone_skin,               &gsn_shield,            50,     PRE_MAGE},
{&gsn_stone_skin,               &gsn_group_protection,  75,     PRE_MAGE},
{&gsn_stone_skin,               &gsn_mage_base,         30,     PRE_MAGE},

/* **** GROUP ILLUSION * ****/
{&gsn_ventriloquate,            &gsn_group_illusion,    15,     PRE_MAGE},
{&gsn_invis,                    &gsn_group_illusion,    30,     PRE_MAGE},
{&gsn_faerie_fog,               &gsn_group_illusion,    50,     PRE_MAGE},

{&gsn_mass_invis,               &gsn_group_illusion,    80,     PRE_MAGE},
{&gsn_mass_invis,               &gsn_invis,             80,     PRE_MAGE},


/* **** SUMMONING **** */
{&gsn_summon,                   &gsn_summon_familiar,   50,     PRE_MAGE},

/* **** GROUP ALTERATION **** */
{&gsn_faerie_fire,              &gsn_group_alteration,  20,     PRE_MAGE},
{&gsn_faerie_fire,              &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_giant_strength,           &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_pass_door,                &gsn_group_alteration,  70,     PRE_MAGE},
{&gsn_pass_door,                &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_energy_drain,             &gsn_group_alteration,  80,     PRE_MAGE},
{&gsn_energy_drain,             &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_breathe_water,            &gsn_group_alteration,  45,     PRE_MAGE},
{&gsn_breathe_water,            &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_blindness,                &gsn_group_alteration,  50,     PRE_MAGE},
{&gsn_blindness,                &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_curse,                    &gsn_group_alteration,  70,     PRE_MAGE},
{&gsn_curse,                    &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_haste,                    &gsn_group_alteration,  100,    PRE_MAGE},
{&gsn_haste,                    &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_dispel_magic,             &gsn_group_alteration,  80,     PRE_MAGE},
{&gsn_dispel_magic,             &gsn_mage_base,         30,     PRE_MAGE},

/* **** MANA CONTROL **** */

{&gsn_blink,                    &gsn_mage_base,         30,     PRE_MAGE},

{&gsn_third_spell,              &gsn_group_mana,        95,     PRE_MAGE},
{&gsn_third_spell,              &gsn_second_spell,      95,     PRE_MAGE},
{&gsn_third_spell,              &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_fourth_spell,             &gsn_group_mana,        100,    PRE_MAGE},
{&gsn_fourth_spell,             &gsn_third_spell,       100,    PRE_MAGE},
{&gsn_fourth_spell,             &gsn_mage_base,         60,     PRE_MAGE},

/* **** BREATH SPELLS **** */

{&gsn_group_breath,             &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_frost_breath,             &gsn_group_breath,      30,     PRE_MAGE},
{&gsn_fire_breath,              &gsn_group_breath,      60,     PRE_MAGE},
{&gsn_steam_breath,             &gsn_group_breath,      80,     PRE_MAGE},
{&gsn_lightning_breath,         &gsn_group_breath,      85,     PRE_MAGE},
{&gsn_acid_breath,              &gsn_group_breath,      90,     PRE_MAGE},
{&gsn_gas_breath,               &gsn_group_breath,      95,     PRE_MAGE},

/* **** MAJOR PROTECTIVE **** */

{&gsn_group_majorp,             &gsn_mage_base,         60,     PRE_MAGE},
{&gsn_group_majorp,             &gsn_group_protection,  85,     PRE_MAGE},

{&gsn_fireshield,               &gsn_group_majorp,      30,     PRE_MAGE},
{&gsn_fireshield,               &gsn_fireball,          80,     PRE_MAGE},
{&gsn_fireshield,               &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_mass_invis,               &gsn_mage_base,         50,     PRE_MAGE},
{&gsn_mass_invis,               &gsn_invis,             90,     PRE_MAGE},
{&gsn_mass_invis,               &gsn_group_majorp,      60,     PRE_MAGE},

{&gsn_globe,                    &gsn_group_majorp,      95,     PRE_MAGE},
{&gsn_globe,                    &gsn_mage_base,         60,     PRE_MAGE},

/* **** MANA CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_MAGE},
{&gsn_group_craft,              &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_scribe,                   &gsn_group_craft,       50,     PRE_MAGE},
{&gsn_scribe,                   &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_recharge_item,            &gsn_group_craft,       90,     PRE_MAGE},
{&gsn_recharge_item,            &gsn_mage_base,         60,     PRE_MAGE},

/* **** DESTRUCTION **** */

{&gsn_group_destruction,        &gsn_group_evocation,   95,     PRE_MAGE},
{&gsn_group_destruction,        &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_firestorm,                &gsn_group_destruction, 50,     PRE_MAGE},
{&gsn_firestorm,                &gsn_fireball,          90,     PRE_MAGE},
{&gsn_firestorm,                &gsn_mage_base,         60,     PRE_MAGE},

{&gsn_meteor_storm,             &gsn_group_destruction, 95,     PRE_MAGE},
{&gsn_meteor_storm,             &gsn_firestorm,         90,     PRE_MAGE},
{&gsn_meteor_storm,             &gsn_mage_base,         60,     PRE_MAGE},

/* **** RIDING **** */

{&gsn_group_riding,             &gsn_mage_base,         59,     PRE_MAGE},
