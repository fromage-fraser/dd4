/* Martial Artist Pre_Req.c */

/* Last modified 1/4/24 - Owl */

/* Total Pre_Reqs == 40  */

/* Starting Skills - have decided what worldly is going to be yet?*/
{&gsn_group_arts,               &gsn_martist_base,      31,     PRE_ARTIST},

{&gsn_group_combos,             &gsn_group_arts,        60,     PRE_ARTIST},

{&gsn_group_stealth,            &gsn_group_arts,        85,     PRE_ARTIST},
{&gsn_group_knowledge,          &gsn_group_arts,        90,     PRE_ARTIST},

/* Level dependent skills */
{&gsn_fast_healing,             &gsn_martist_base,      49,     PRE_ARTIST},

/* DEFENSE */
{&gsn_acrobatics,               &gsn_group_defense,     90,     PRE_ARTIST},
{&gsn_acrobatics,               &gsn_martist_base,      30,     PRE_ARTIST},
{&gsn_acrobatics,               &gsn_group_arts,        90,     PRE_ARTIST},


/* Martial Arts group. */
{&gsn_atemi,            &gsn_group_arts,                40,     PRE_ARTIST},

{&gsn_kansetsu,         &gsn_group_arts,                60,     PRE_ARTIST},
{&gsn_kansetsu,         &gsn_atemi,                     60,     PRE_ARTIST},

{&gsn_tetsui,           &gsn_group_arts,                70,     PRE_ARTIST},
{&gsn_tetsui,           &gsn_kansetsu,                  75,     PRE_ARTIST},

{&gsn_shuto,            &gsn_tetsui,                    80,     PRE_ARTIST},

{&gsn_yokogeri,         &gsn_group_arts,                90,     PRE_ARTIST},
{&gsn_yokogeri,         &gsn_shuto,                     90,     PRE_ARTIST},

{&gsn_mawasigeri,       &gsn_group_arts,                95,     PRE_ARTIST},

{&gsn_push,             &gsn_mawasigeri,                90,     PRE_ARTIST},
{&gsn_push,             &gsn_martist_base,              30,     PRE_ARTIST},

/* **** combos group **** */
{&gsn_combo,            &gsn_group_combos,              65,     PRE_ARTIST},
{&gsn_combo2,           &gsn_group_combos,              65,     PRE_ARTIST},
{&gsn_combo3,           &gsn_group_combos,              85,     PRE_ARTIST},
{&gsn_combo3,           &gsn_combo2,                    85,     PRE_ARTIST},
{&gsn_combo4,           &gsn_group_combos,              95,     PRE_ARTIST},
{&gsn_combo4,           &gsn_combo3,                    95,     PRE_ARTIST},

{&gsn_whirlwind,        &gsn_combo4,                    95,     PRE_ARTIST},
{&gsn_whirlwind,        &gsn_group_combos,              95,     PRE_ARTIST},
{&gsn_whirlwind,        &gsn_martist_base,              30,     PRE_ARTIST},

/* stealth group */
{&gsn_hide,             &gsn_group_stealth,             60,     PRE_ARTIST},
{&gsn_hide,             &gsn_martist_base,              30,     PRE_ARTIST},

{&gsn_sneak,            &gsn_group_stealth,             95,     PRE_ARTIST},
{&gsn_sneak,            &gsn_hide,                      90,     PRE_ARTIST},
{&gsn_sneak,            &gsn_martist_base,              30,     PRE_ARTIST},

/* worldy group */
{&gsn_lore,                     &gsn_group_knowledge,   50,     PRE_ARTIST},

{&gsn_advanced_consider,        &gsn_group_knowledge,   80,     PRE_ARTIST},
{&gsn_advanced_consider,        &gsn_lore,              80,     PRE_ARTIST},
{&gsn_advanced_consider,        &gsn_martist_base,      30,     PRE_ARTIST},

{&gsn_enhanced_hit,             &gsn_group_knowledge,   90,     PRE_ARTIST},
{&gsn_enhanced_hit,             &gsn_advanced_consider, 90,     PRE_ARTIST},
{&gsn_enhanced_hit,             &gsn_martist_base,      30,     PRE_ARTIST},

/* **** POST-70 SKILLS **** */
{&gsn_target,                   &gsn_enhanced_hit,      95,     PRE_ARTIST},
{&gsn_target,                   &gsn_martist_base,      70,     PRE_ARTIST},

{&gsn_enhanced_swiftness,       &gsn_combo4,            95,     PRE_ARTIST},
{&gsn_enhanced_swiftness,       &gsn_martist_base,      70,     PRE_ARTIST},

{&gsn_enhanced_critical,        &gsn_enhanced_swiftness,95,     PRE_ARTIST},
{&gsn_enhanced_critical,        &gsn_martist_base,      80,     PRE_ARTIST},

{&gsn_tenketsu,                 &gsn_group_arts,        97,     PRE_ARTIST},
{&gsn_tenketsu,                 &gsn_martist_base,      80,     PRE_ARTIST},




