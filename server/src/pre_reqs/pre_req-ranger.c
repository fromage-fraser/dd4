/* Ranger Pre_Req.c */

/* Last modified 04:00 16/1/00 - Owl */

/* Total Pre_Reqs == 66 */

/* Starting Skills */

{&gsn_group_armed,              &gsn_ranger_base,       30,     PRE_RANGER},
{&gsn_group_defense,            &gsn_ranger_base,       30,     PRE_RANGER},
{&gsn_group_thievery,           &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_forage,                   &gsn_ranger_base,       39,     PRE_RANGER},
{&gsn_dowse,                    &gsn_ranger_base,       39,     PRE_RANGER},

/* **** GROUPS **** */

/* Defense Skills */

{&gsn_dodge,                    &gsn_group_defense,     40,     PRE_RANGER},
{&gsn_dodge,                    &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_parry,                    &gsn_group_defense,     60,     PRE_RANGER},
{&gsn_parry,                    &gsn_dodge,             40,     PRE_RANGER},
{&gsn_parry,                    &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_grip,                     &gsn_group_defense,     75,     PRE_RANGER},
{&gsn_grip,                     &gsn_parry,             60,     PRE_RANGER},
{&gsn_grip,                     &gsn_dodge,             60,     PRE_RANGER},
{&gsn_grip,                     &gsn_ranger_base,       30,     PRE_RANGER},

/* Armed Combat Skills */

{&gsn_second_attack,            &gsn_group_armed,       20,     PRE_RANGER},
{&gsn_second_attack,            &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_third_attack,             &gsn_group_armed,       40,     PRE_RANGER},
{&gsn_third_attack,             &gsn_second_attack,     60,     PRE_RANGER},
{&gsn_third_attack,             &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_feint,                    &gsn_group_armed,       80,     PRE_RANGER},
{&gsn_feint,                    &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_disarm,                   &gsn_group_defense,     75,     PRE_RANGER},
{&gsn_disarm,                   &gsn_grip,              75,     PRE_RANGER},
{&gsn_disarm,                   &gsn_ranger_base,       30,     PRE_RANGER},

/* Thievery Skills */

{&gsn_hide,                     &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_sneak,                    &gsn_hide,              50,     PRE_RANGER},
{&gsn_sneak,                    &gsn_group_thievery,    50,     PRE_RANGER},
{&gsn_sneak,                    &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_find_traps,               &gsn_group_thievery,    60,     PRE_RANGER},
{&gsn_find_traps,               &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_disable,                  &gsn_find_traps,        40,     PRE_RANGER},
{&gsn_disable,                  &gsn_group_thievery,    70,     PRE_RANGER},

/* Knowledge */

{&gsn_group_knowledge,          &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_lore,                     &gsn_group_knowledge,   85,     PRE_RANGER},
{&gsn_lore,                     &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_advanced_consider,        &gsn_group_knowledge,   70,     PRE_RANGER},
{&gsn_advanced_consider,        &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_fast_healing,             &gsn_group_knowledge,   85,     PRE_RANGER},
{&gsn_fast_healing,             &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_bark_skin,                &gsn_group_knowledge,   75,     PRE_RANGER},
{&gsn_bark_skin,                &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_faerie_fire,              &gsn_group_knowledge,   80,     PRE_RANGER},
{&gsn_faerie_fire,              &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_detect_hidden,            &gsn_group_knowledge,   85,     PRE_RANGER},
{&gsn_detect_hidden,            &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_infravision,              &gsn_detect_hidden,     70,     PRE_RANGER},
{&gsn_infravision,              &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_enhanced_hit,             &gsn_group_knowledge,   95,     PRE_RANGER},
{&gsn_enhanced_hit,             &gsn_ranger_base,       60,     PRE_RANGER},

/* Advanced Combat Knowledge Skills */

{&gsn_group_advcombat,          &gsn_group_armed,       92,     PRE_RANGER},
{&gsn_group_advcombat,          &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_thrust,                   &gsn_feint,             85,     PRE_RANGER},
{&gsn_thrust,                   &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_dual,                     &gsn_group_advcombat,   50,     PRE_RANGER},
{&gsn_dual,                     &gsn_second_attack,     90,     PRE_RANGER},
{&gsn_dual,                     &gsn_third_attack,      90,     PRE_RANGER},
{&gsn_dual,                     &gsn_ranger_base,       60,     PRE_RANGER},

/* Archery Skills */

{&gsn_group_archery,            &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_shoot,                    &gsn_group_archery,     20,     PRE_RANGER},
{&gsn_shoot,                    &gsn_ranger_base,       30,     PRE_RANGER},

{&gsn_snap_shot,                &gsn_group_archery,     30,     PRE_RANGER},

/*
 * add some kind of between rounds quick shot to archery at about 60%
 */

{&gsn_second_shot,              &gsn_group_archery,     80,     PRE_RANGER},
{&gsn_second_shot,              &gsn_shoot,             80,     PRE_RANGER},
{&gsn_second_shot,              &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_third_shot,               &gsn_second_shot,       90,     PRE_RANGER},
{&gsn_third_shot,               &gsn_group_archery,     90,     PRE_RANGER},
{&gsn_third_shot,               &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_accuracy,                 &gsn_third_shot,        95,     PRE_RANGER},
{&gsn_accuracy,                 &gsn_group_archery,     95,     PRE_RANGER},
{&gsn_accuracy,                 &gsn_ranger_base,       60,     PRE_RANGER},

/* Hunting Skills */

{&gsn_group_hunting,            &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_snare,                    &gsn_group_hunting,     50,     PRE_RANGER},
{&gsn_snare,                    &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_hunt,                     &gsn_group_hunting,     80,     PRE_RANGER},
{&gsn_hunt,                     &gsn_ranger_base,       60,     PRE_RANGER},

/* Herb Lore */

{&gsn_group_herb_lore,          &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_classify,                 &gsn_group_herb_lore,   70,     PRE_RANGER},
{&gsn_classify,                 &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_gather_herbs,             &gsn_group_herb_lore,   80,     PRE_RANGER},
{&gsn_gather_herbs,             &gsn_classify,          80,     PRE_RANGER},
{&gsn_gather_herbs,             &gsn_ranger_base,       60,     PRE_RANGER},


/* **** RIDING **** */

{&gsn_group_riding,             &gsn_ranger_base,       59,     PRE_RANGER},

/* **** Metal Group **** */

{&gsn_group_metal,              &gsn_group_knowledge,   80,     PRE_RANGER},
{&gsn_group_metal,              &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_forge,                    &gsn_group_metal,       40,     PRE_RANGER},
{&gsn_forge,                    &gsn_ranger_base,       60,     PRE_RANGER},

{&gsn_sharpen,                  &gsn_group_metal,       70,     PRE_RANGER},
{&gsn_sharpen,                  &gsn_forge,             50,     PRE_RANGER},
{&gsn_sharpen,                  &gsn_ranger_base,       60,     PRE_RANGER},

