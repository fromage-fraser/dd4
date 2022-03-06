
/* **** GROUP DIVINATION **** */
{&gsn_detect_invis,             &gsn_group_divination,  20,     11},
{&gsn_detect_magic,             &gsn_group_divination,  30,     11},
{&gsn_detect_hidden,            &gsn_group_divination,  40,     11},

{&gsn_infravision,              &gsn_group_divination,  36,     11},

{&gsn_identify,                 &gsn_group_divination,  30,     11},
{&gsn_locate_object,            &gsn_group_divination,  40,     11},
{&gsn_locate_object,            &gsn_identify,          40,     11},

{&gsn_sense_traps,              &gsn_group_divination,  50,     11},
{&gsn_sense_traps,              &gsn_detect_invis,      50,     11},
{&gsn_sense_traps,              &gsn_detect_magic,      50,     11},

{&gsn_detect_curse,             &gsn_group_divination,  60,     11},
{&gsn_detect_curse,             &gsn_detect_magic,      60,     11},

{&gsn_know_alignment,           &gsn_detect_evil,       50,     11},
{&gsn_know_alignment,           &gsn_group_divination,  50,     11},


/* **** GROUP ALTERATION **** */

{&gsn_fly,                      &gsn_group_alteration,  30,     0},
{&gsn_giant_strength,           &gsn_group_alteration,  40,     0},
{&gsn_weaken,                   &gsn_giant_strength,    20,     0},
{&gsn_weaken,                   &gsn_chill_touch,       20,     0},
{&gsn_faerie_fog,               &gsn_group_alteration,  60,     0},
{&gsn_faerie_fog,               &gsn_faerie_fire,       60,     0},

/* **** GROUP ENCHANTMENT **** */

{&gsn_sleep,                    &gsn_group_enchant,     40,     0},
{&gsn_enchant_weapon,           &gsn_group_enchant,     60,     0},

/* **** SUMMONING **** */

{&gsn_call_lightning,           &gsn_group_summoning,   20,     0},
{&gsn_control_weather,          &gsn_group_summoning,   40,     0},
{&gsn_control_weather,          &gsn_call_lightning,    50,     0},

{&gsn_summon,                   &gsn_group_summoning,   80,     0},
{&gsn_pattern,                  &gsn_group_summoning,   95,     0},
{&gsn_pattern,                  &gsn_summon,            95,     0},

/* **** MANA CONTROL **** */

{&gsn_meditate,                 &gsn_group_mana,        30,     0},
{&gsn_second_spell,             &gsn_group_mana,        65,     0},
{&gsn_blink,                    &gsn_group_mana,        85,     0},

/* **** RIDING **** */

{&gsn_mount,                    &gsn_group_riding,      50,     0},













