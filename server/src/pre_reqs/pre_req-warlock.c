/* **** WARLOCK PRE-REQS **** */

{&gsn_forge,                    &gsn_warlock_base,      64,     PRE_WARLOCK},

/* **** CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_WARLOCK},
{&gsn_group_craft,              &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_recharge_item,            &gsn_group_craft,       50,     PRE_WARLOCK},
{&gsn_recharge_item,            &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_transport,                &gsn_group_craft,       90,     PRE_WARLOCK},
{&gsn_transport,                &gsn_warlock_base,      30,     PRE_WARLOCK},

/* **** MAJOR PROTECTION **** */

{&gsn_group_majorp,             &gsn_group_protection,  95,     PRE_WARLOCK},
{&gsn_group_majorp,             &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_dragon_shield,            &gsn_group_majorp,      50,     PRE_WARLOCK},
{&gsn_dragon_shield,            &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_fireshield,               &gsn_group_majorp,      90,     PRE_WARLOCK},
{&gsn_fireshield,               &gsn_dragon_shield,     90,     PRE_WARLOCK},
{&gsn_fireshield,               &gsn_warlock_base,      30,     PRE_WARLOCK},

/* **** LEVEL 31 **** */

{&gsn_group_defense,            &gsn_warlock_base,      31,     PRE_WARLOCK},

/* **** DEFENSE **** */

{&gsn_swim,                     &gsn_group_defense,     20,     PRE_WARLOCK},
{&gsn_swim,                     &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_rescue,                   &gsn_group_defense,     50,     PRE_WARLOCK},
{&gsn_rescue,                   &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_shield_block,             &gsn_group_defense,     70,     PRE_WARLOCK},
{&gsn_shield_block,             &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_parry,                    &gsn_group_defense,     90,     PRE_WARLOCK},
{&gsn_parry,                    &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_grip,                     &gsn_group_defense,     95,     PRE_WARLOCK},
{&gsn_grip,                     &gsn_parry,             95,     PRE_WARLOCK},
{&gsn_grip,                     &gsn_warlock_base,      30,     PRE_WARLOCK},

/* **** ARMED COMBAT **** */

{&gsn_group_armed,              &gsn_group_defense,     85,     PRE_WARLOCK},
{&gsn_group_armed,              &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_second_attack,            &gsn_group_armed,       40,     PRE_WARLOCK},
{&gsn_second_attack,            &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_third_attack,             &gsn_group_armed,       80,     PRE_WARLOCK},
{&gsn_third_attack,             &gsn_second_attack,     80,     PRE_WARLOCK},
{&gsn_third_attack,             &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_enhanced_hit,             &gsn_group_armed,       95,     PRE_WARLOCK},
{&gsn_enhanced_hit,             &gsn_warlock_base,      30,     PRE_WARLOCK},

/* **** ADVANCED COMBAT **** */

{&gsn_group_advcombat,          &gsn_group_armed,       95,     PRE_WARLOCK},
{&gsn_group_advcombat,          &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_dual,                     &gsn_group_advcombat,   50,     PRE_WARLOCK},
{&gsn_dual,                     &gsn_third_attack,      90,     PRE_WARLOCK},
{&gsn_dual,                     &gsn_warlock_base,      30,     PRE_WARLOCK},

/* **** INNER FORCE **** */

{&gsn_group_inner,              &gsn_group_defense,     90,     PRE_WARLOCK},
{&gsn_group_inner,              &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_fast_healing,             &gsn_group_inner,       50,     PRE_WARLOCK},
{&gsn_fast_healing,             &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_warcry,                   &gsn_group_inner,       95,     PRE_WARLOCK},
{&gsn_warcry,                   &gsn_warlock_base,      30,     PRE_WARLOCK},

{&gsn_whirlwind,                &gsn_dual,              95,     PRE_WARLOCK},
{&gsn_whirlwind,                &gsn_warlock_base,      60,     PRE_WARLOCK},
