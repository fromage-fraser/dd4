/* **** THUG SKILLS **** */

/* **** ARMED COMBAT **** */

{&gsn_fourth_attack,            &gsn_group_armed,       85,     PRE_THUG},
{&gsn_fourth_attack,            &gsn_third_attack,      85,     PRE_THUG},
{&gsn_fourth_attack,            &gsn_second_attack,     85,     PRE_THUG},
{&gsn_fourth_attack,            &gsn_thug_base,         30,     PRE_THUG},

/* **** DEFENSE KNOWLEDGE **** */

{&gsn_smash,                    &gsn_group_defense,     75,     PRE_THUG},
{&gsn_smash,                    &gsn_shield_block,      75,     PRE_THUG},
{&gsn_smash,                    &gsn_thug_base,         30,     PRE_THUG},

/* **** ADVANCED COMBAT **** */

/* At this stage they only get 1 skill in this group. Not ideal but hey */

{&gsn_group_advcombat,          &gsn_group_armed,       90,     PRE_THUG},
{&gsn_group_advcombat,          &gsn_thug_base,         30,     PRE_THUG},

{&gsn_poison_weapon,            &gsn_group_advcombat,   50,     PRE_THUG},
{&gsn_poison_weapon,            &gsn_thug_base,         30,     PRE_THUG},

{&gsn_enhanced_hit,             &gsn_group_advcombat,   90,     PRE_THUG},
{&gsn_enhanced_hit,             &gsn_enhanced_damage,   90,     PRE_THUG},
{&gsn_enhanced_hit,             &gsn_thug_base,         30,     PRE_THUG},

/* **** UNARMED COMBAT **** */

{&gsn_gouge,                    &gsn_group_unarmed,     80,     PRE_THUG},
{&gsn_gouge,                    &gsn_thug_base,         30,     PRE_THUG},

{&gsn_second_headbutt,          &gsn_group_unarmed,     85,     PRE_THUG},
{&gsn_second_headbutt,          &gsn_headbutt,          85,     PRE_THUG},
{&gsn_second_headbutt,          &gsn_thug_base,         30,     PRE_THUG},

/* **** THIEVERY **** */

{&gsn_group_thievery,           &gsn_thug_base,         31,     PRE_THUG},

{&gsn_peek,                     &gsn_group_thievery,    50,     PRE_THUG},
{&gsn_peek,                     &gsn_thug_base,         30,     PRE_THUG},

{&gsn_steal,                    &gsn_group_thievery,    85,     PRE_THUG},
{&gsn_steal,                    &gsn_peek,              85,     PRE_THUG},
{&gsn_steal,                    &gsn_thug_base,         30,     PRE_THUG},

{&gsn_extort,                   &gsn_group_thievery,    90,     PRE_THUG},
{&gsn_extort,                   &gsn_thug_base,         30,     PRE_THUG},

{&gsn_intimidate,               &gsn_group_thievery,    90,     PRE_THUG},
{&gsn_intimidate,               &gsn_extort,            95,     PRE_THUG},
{&gsn_intimidate,               &gsn_thug_base,         30,     PRE_THUG},



