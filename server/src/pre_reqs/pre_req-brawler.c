/* Brawler Pre_Req.c */

/* Starting Skills */
{&gsn_group_combat,             &gsn_brawler_base,      30,     PRE_BRAWLER},
{&gsn_group_unarmed,            &gsn_brawler_base,      30,     PRE_BRAWLER},
{&gsn_group_defense,            &gsn_brawler_base,      30,     PRE_BRAWLER},

/* Level dependent skills */
{&gsn_group_pugalism,           &gsn_brawler_base,      34,     PRE_BRAWLER},
{&gsn_group_inner,              &gsn_brawler_base,      39,     PRE_BRAWLER},

/* **** DEFENSE **** */

{&gsn_swim,                     &gsn_brawler_base,      20,     PRE_BRAWLER},
{&gsn_swim,                     &gsn_group_defense,     20,     PRE_BRAWLER},

{&gsn_dodge,                    &gsn_group_defense,     20,     PRE_BRAWLER},
{&gsn_dodge,                    &gsn_brawler_base,      20,     PRE_BRAWLER},

{&gsn_rescue,                   &gsn_group_defense,     40,     PRE_BRAWLER},
{&gsn_rescue,                   &gsn_brawler_base,      30,     PRE_BRAWLER},

{&gsn_pre_empt,                 &gsn_group_defense,     60,     PRE_BRAWLER},
{&gsn_pre_empt,                 &gsn_brawler_base,      30,     PRE_BRAWLER},

/* **** GROUP COMBAT **** */

{&gsn_second_attack,            &gsn_group_combat,      20,     PRE_BRAWLER},
{&gsn_enhanced_damage,          &gsn_group_combat,      35,     PRE_BRAWLER},

{&gsn_third_attack,             &gsn_second_attack,     50,     PRE_BRAWLER},
{&gsn_third_attack,             &gsn_group_combat,      50,     PRE_BRAWLER},

{&gsn_feint,                    &gsn_group_combat,      85,     PRE_BRAWLER},

{&gsn_fourth_attack,            &gsn_third_attack,      85,     PRE_BRAWLER},
{&gsn_fourth_attack,            &gsn_second_attack,     85,     PRE_BRAWLER},
{&gsn_fourth_attack,            &gsn_group_combat,      85,     PRE_BRAWLER},
{&gsn_fourth_attack,            &gsn_brawler_base,      60,     PRE_BRAWLER},

{&gsn_whirlwind,                &gsn_third_attack,      85,     PRE_BRAWLER},
{&gsn_whirlwind,                &gsn_fourth_attack,     85,     PRE_BRAWLER},
{&gsn_whirlwind,                &gsn_group_combat,      90,     PRE_BRAWLER},
{&gsn_whirlwind,                &gsn_brawler_base,      60,     PRE_BRAWLER},

/* group pugalism */
{&gsn_pugalism,                 &gsn_group_pugalism,    90,     PRE_BRAWLER},
{&gsn_pugalism,                 &gsn_brawler_base,      60,     PRE_BRAWLER},
{&gsn_gouge,                    &gsn_group_pugalism,    38,     PRE_BRAWLER},
{&gsn_punch,                    &gsn_group_pugalism,    40,     PRE_BRAWLER},

{&gsn_grapple,                  &gsn_punch,             60,     PRE_BRAWLER},
{&gsn_grapple,                  &gsn_group_pugalism,    60,     PRE_BRAWLER},

{&gsn_second_punch,              &gsn_punch,            51,     PRE_BRAWLER},
{&gsn_second_punch,              &gsn_group_pugalism,   51,     PRE_BRAWLER},

/* above level 30 */
{&gsn_choke,                    &gsn_group_pugalism,    75,     PRE_BRAWLER},
{&gsn_choke,                    &gsn_brawler_base,      60,     PRE_BRAWLER},

{&gsn_break_wrist,              &gsn_brawler_base,      60,     PRE_BRAWLER},
{&gsn_break_wrist,              &gsn_choke,             75,     PRE_BRAWLER},

{&gsn_flying_headbutt,          &gsn_headbutt,          85,     PRE_BRAWLER},
{&gsn_flying_headbutt,          &gsn_group_pugalism,    85,     PRE_BRAWLER},
{&gsn_flying_headbutt,          &gsn_brawler_base,      60,     PRE_BRAWLER},

{&gsn_snap_neck,                &gsn_choke,             85,     PRE_BRAWLER},
{&gsn_snap_neck,                &gsn_break_wrist,       95,     PRE_BRAWLER},
{&gsn_snap_neck,                &gsn_group_pugalism,    95,     PRE_BRAWLER},

{&gsn_push,                     &gsn_brawler_base,      60,     PRE_BRAWLER},
{&gsn_push,                     &gsn_grapple,           95,     PRE_BRAWLER},
{&gsn_push,                     &gsn_snap_neck,         95,     PRE_BRAWLER},

{&gsn_enhanced_hit,             &gsn_group_pugalism,    95,     PRE_BRAWLER},

/* **** UNARMED COMBAT **** */

{&gsn_headbutt,                 &gsn_group_unarmed,     30,     PRE_BRAWLER},
{&gsn_headbutt,                 &gsn_brawler_base,      30,     PRE_BRAWLER},

{&gsn_warcry,                   &gsn_group_inner,       40,     PRE_BRAWLER},
{&gsn_warcry,                   &gsn_brawler_base,      30,     PRE_BRAWLER},

{&gsn_bash,                     &gsn_group_inner,       50,     PRE_BRAWLER},
{&gsn_bash,                     &gsn_brawler_base,      30,     PRE_BRAWLER},

{&gsn_battle_aura,              &gsn_group_inner,       85,     PRE_BRAWLER},
{&gsn_battle_aura,              &gsn_warcry,            85,     PRE_BRAWLER},
{&gsn_battle_aura,              &gsn_brawler_base,      60,     PRE_BRAWLER},

{&gsn_unarmed_combat,           &gsn_group_unarmed,     50,     PRE_BRAWLER},
{&gsn_unarmed_combat,           &gsn_warcry,            45,     PRE_BRAWLER},
{&gsn_unarmed_combat,           &gsn_brawler_base,      30,     PRE_BRAWLER},

{&gsn_advanced_consider,        &gsn_group_inner,       70,     PRE_BRAWLER},
{&gsn_advanced_consider,        &gsn_brawler_base,      70,     PRE_BRAWLER},

{&gsn_focus,                    &gsn_brawler_base,      60,     PRE_BRAWLER},
{&gsn_focus,                    &gsn_group_inner,       90,     PRE_BRAWLER},
{&gsn_focus,                    &gsn_advanced_consider, 90,     PRE_BRAWLER},

{&gsn_fast_healing,             &gsn_group_inner,       85,     PRE_BRAWLER},
{&gsn_fast_healing,             &gsn_brawler_base,      30,     PRE_BRAWLER},
