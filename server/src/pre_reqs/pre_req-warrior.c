/* Warrior Pre_Req.c */

/* Last modified 22:20 18/2 - Simon */

/* Total Pre_Reqs == 48 */

/* Starting Skills */

{&gsn_group_armed,              &gsn_warrior_base,      30,     PRE_WARRIOR},
{&gsn_group_unarmed,            &gsn_warrior_base,      30,     PRE_WARRIOR},
{&gsn_group_defense,            &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_shield_block,             &gsn_warrior_base,      30,     PRE_WARRIOR},

/* **** GROUPS **** */

{&gsn_group_inner,              &gsn_warrior_base,      44,     PRE_WARRIOR},/* 15th */

/* Inner Force Skills */

{&gsn_bash,                     &gsn_group_inner,       20,     PRE_WARRIOR},
{&gsn_bash,                     &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_fast_healing,             &gsn_group_inner,       40,     PRE_WARRIOR},
{&gsn_fast_healing,             &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_warcry,                   &gsn_group_inner,       60,     PRE_WARRIOR},
{&gsn_warcry,                   &gsn_warrior_base,      30,     PRE_WARRIOR},

/* Unarmed Combat Skills - 1 to be added */

{&gsn_kick,                     &gsn_group_unarmed,     20,     PRE_WARRIOR},
{&gsn_kick,                     &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_headbutt,                 &gsn_group_unarmed,     40,     PRE_WARRIOR},
{&gsn_headbutt,                 &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_stun,                     &gsn_group_unarmed,     60,     PRE_WARRIOR},
{&gsn_stun,                     &gsn_warrior_base,      30,     PRE_WARRIOR},

/* Defense Skills */

{&gsn_rescue,                   &gsn_group_defense,     20,     PRE_WARRIOR},
{&gsn_rescue,                   &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_swim,                     &gsn_group_defense,     20,     PRE_WARRIOR},
{&gsn_swim,                     &gsn_warrior_base,      20,     PRE_WARRIOR},

{&gsn_dodge,                    &gsn_group_defense,     40,     PRE_WARRIOR},
{&gsn_dodge,                    &gsn_shield_block,      20,     PRE_WARRIOR},
{&gsn_dodge,                    &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_parry,                    &gsn_group_defense,     60,     PRE_WARRIOR},
{&gsn_parry,                    &gsn_dodge,             40,     PRE_WARRIOR},
{&gsn_parry,                    &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_grip,                     &gsn_group_defense,     75,     PRE_WARRIOR},
{&gsn_grip,                     &gsn_parry,             60,     PRE_WARRIOR},
{&gsn_grip,                     &gsn_dodge,             60,     PRE_WARRIOR},
{&gsn_grip,                     &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_disarm,                   &gsn_group_defense,     85,     PRE_WARRIOR},
{&gsn_disarm,                   &gsn_grip,              85,     PRE_WARRIOR},
{&gsn_disarm,                   &gsn_warrior_base,      30,     PRE_WARRIOR},

/* Armed Combat Skills */

{&gsn_second_attack,            &gsn_group_armed,       20,     PRE_WARRIOR},
{&gsn_enhanced_damage,          &gsn_group_armed,       40,     PRE_WARRIOR},
{&gsn_enhanced_damage,          &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_third_attack,             &gsn_group_armed,       40,     PRE_WARRIOR},
{&gsn_third_attack,             &gsn_second_attack,     60,     PRE_WARRIOR},
{&gsn_third_attack,             &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_feint,                    &gsn_group_armed,       85,     PRE_WARRIOR},
{&gsn_feint,                    &gsn_warrior_base,      30,     PRE_WARRIOR},

{&gsn_fourth_attack,            &gsn_group_armed,       85,     PRE_WARRIOR},
{&gsn_fourth_attack,            &gsn_second_attack,     80,     PRE_WARRIOR},
{&gsn_fourth_attack,            &gsn_third_attack,      80,     PRE_WARRIOR},
{&gsn_fourth_attack,            &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_dual,                     &gsn_group_armed,       90,     PRE_WARRIOR},
{&gsn_dual,                     &gsn_third_attack,      92,     PRE_WARRIOR},
{&gsn_dual,                     &gsn_fourth_attack,     92,     PRE_WARRIOR},
{&gsn_dual,                     &gsn_warrior_base,      60,     PRE_WARRIOR},

/* **** HIGH LEVEL SKILLS **** */

/* These skills are those a Warrior only gets starting with the knowledge
sphere at 31st level */

{&gsn_group_knowledge,          &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_lore,                     &gsn_group_knowledge,   30,     PRE_WARRIOR},

{&gsn_advanced_consider,        &gsn_group_knowledge,   70,     PRE_WARRIOR},
{&gsn_advanced_consider,        &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_enhanced_hit,             &gsn_group_knowledge,   75,     PRE_WARRIOR},
{&gsn_enhanced_hit,             &gsn_advanced_consider, 75,     PRE_WARRIOR},
{&gsn_enhanced_hit,             &gsn_warrior_base,      69,     PRE_WARRIOR},

/* **** Advanced Combat **** */

{&gsn_group_advcombat,          &gsn_group_armed,       90,     PRE_WARRIOR},
{&gsn_group_advcombat,          &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_focus,                    &gsn_group_advcombat,   90,     PRE_WARRIOR},
{&gsn_focus,                    &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_risposte,                 &gsn_group_advcombat,   50,     PRE_WARRIOR},
{&gsn_risposte,                 &gsn_parry,             90,     PRE_WARRIOR},
{&gsn_risposte,                 &gsn_group_defense,     90,     PRE_WARRIOR},
{&gsn_risposte,                 &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_whirlwind,                &gsn_group_advcombat,   95,     PRE_WARRIOR},
{&gsn_whirlwind,                &gsn_dual,              90,     PRE_WARRIOR},
{&gsn_whirlwind,                &gsn_warrior_base,      60,     PRE_WARRIOR},

/* **** Metal Group **** */

{&gsn_group_metal,              &gsn_group_knowledge,   80,     PRE_WARRIOR},
{&gsn_group_metal,              &gsn_warrior_base,      60,     PRE_WARRIOR},

{&gsn_forge,                    &gsn_group_metal,       40,     PRE_WARRIOR},
{&gsn_sharpen,                  &gsn_group_metal,       70,     PRE_WARRIOR},
{&gsn_sharpen,                  &gsn_forge,             50,     PRE_WARRIOR},

/* **** Riding **** */

{&gsn_group_riding,             &gsn_warrior_base,      59,     PRE_WARRIOR},
