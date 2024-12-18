/* **** TEMPLAR **** */

/* **** ARMED COMBAT **** */

{&gsn_third_attack,             &gsn_second_attack,     75,     PRE_TEMPLAR},
{&gsn_third_attack,             &gsn_group_armed,       85,     PRE_TEMPLAR},
{&gsn_third_attack,             &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_dual,                     &gsn_group_armed,       95,     PRE_TEMPLAR},
{&gsn_dual,                     &gsn_third_attack,      95,     PRE_TEMPLAR},
{&gsn_dual,                     &gsn_templar_base,      30,     PRE_TEMPLAR},

/* Chuck Enhanced hit into Armed Combat for lack of better place */

{&gsn_enhanced_hit,             &gsn_group_armed,       95,     PRE_TEMPLAR},
{&gsn_enhanced_hit,             &gsn_templar_base,      30,     PRE_TEMPLAR},

/* **** INNER FORCE **** */

{&gsn_group_inner,              &gsn_templar_base,      31,     PRE_TEMPLAR},

{&gsn_bash,                     &gsn_group_inner,       40,     PRE_TEMPLAR},
{&gsn_bash,                     &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_fast_healing,             &gsn_group_inner,       70,     PRE_TEMPLAR},
{&gsn_fast_healing,             &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_warcry,                   &gsn_group_inner,       90,     PRE_TEMPLAR},
{&gsn_warcry,                   &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_frenzy,                   &gsn_group_divine,      90,     PRE_TEMPLAR},
{&gsn_frenzy,                   &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_prayer,                   &gsn_templar_base,      49,     PRE_TEMPLAR},

/* **** DEFENSE **** */

{&gsn_swim,                     &gsn_group_defense,     20,     PRE_TEMPLAR},
{&gsn_swim,                     &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_rescue,                   &gsn_group_defense,     75,     PRE_TEMPLAR},
{&gsn_rescue,                   &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_parry,                    &gsn_group_defense,     90,     PRE_TEMPLAR},
{&gsn_parry,                    &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_grip,                     &gsn_group_defense,     95,     PRE_TEMPLAR},
{&gsn_grip,                     &gsn_parry,             90,     PRE_TEMPLAR},
{&gsn_grip,                     &gsn_templar_base,      30,     PRE_TEMPLAR},

/* **** FORGE **** */

/* Get it at 55th level - this is hacky and done under protest! - Shade */
{&gsn_forge,                    &gsn_templar_base,      54,     PRE_TEMPLAR},

/* **** MANA CRAFT **** */

{&gsn_group_craft,              &gsn_group_mana,        90,     PRE_TEMPLAR},
{&gsn_group_craft,              &gsn_templar_base,      30,     PRE_TEMPLAR},

{&gsn_recharge_item,            &gsn_group_craft,       50,     PRE_TEMPLAR},
{&gsn_recharge_item,            &gsn_templar_base,      30,     PRE_TEMPLAR},


/* **** POST-70 SKILLS **** */

{&gsn_target,                   &gsn_enhanced_hit,     100,     PRE_TEMPLAR},
{&gsn_target,                   &gsn_templar_base,      70,     PRE_TEMPLAR},

{&gsn_enhanced_swiftness,       &gsn_third_attack,     100,     PRE_TEMPLAR},
{&gsn_enhanced_swiftness,       &gsn_templar_base,      70,     PRE_TEMPLAR},

{&gsn_resist_lightning,         &gsn_frenzy,           100,     PRE_TEMPLAR},
{&gsn_resist_lightning,         &gsn_templar_base,      80,     PRE_TEMPLAR},

{&gsn_resist_cold,              &gsn_resist_lightning, 100,     PRE_TEMPLAR},
{&gsn_resist_cold,              &gsn_templar_base,      80,     PRE_TEMPLAR},



