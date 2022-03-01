
/* Groups

Thieves have four different groups: Armed Combat, Defense, Stealth and
Thievery. More will be added when/if we think of them - Simon 

Last modified 29/7/97 @ 15:45 - Mat

*/

  {&gsn_group_armed,            &gsn_thief_base,        30,     PRE_THIEF},
  {&gsn_group_defense,          &gsn_thief_base,        30,     PRE_THIEF},
  {&gsn_group_stealth,          &gsn_thief_base,        30,     PRE_THIEF},
  {&gsn_group_thievery,         &gsn_thief_base,        30,     PRE_THIEF},
  
/* **** GROUPS **** */

  {&gsn_group_unarmed,          &gsn_thief_base,        39,     PRE_THIEF},

  /* Base Skills */

  {&gsn_peek,                   &gsn_thief_base,        30,     PRE_THIEF},
  {&gsn_hide,                   &gsn_thief_base,        30,     PRE_THIEF},
  {&gsn_pick_lock,              &gsn_thief_base,        30,     PRE_THIEF},  

  /* Set level dependent skills */

  {&gsn_fast_healing,           &gsn_thief_base,        49,     PRE_THIEF},

/* SpellCarft is the group that thieves get at 31 */

  {&gsn_spellcraft,             &gsn_thief_base,        60,     PRE_THIEF},
  
/* Spell Skills */

  {&gsn_detect_hidden,          &gsn_peek,              20,     PRE_THIEF},
  {&gsn_detect_hidden,          &gsn_spellcraft,        30,     PRE_THIEF},

  {&gsn_detect_invis,           &gsn_detect_hidden,     60,     PRE_THIEF},
  {&gsn_detect_invis,           &gsn_spellcraft,        50,     PRE_THIEF},

  {&gsn_detect_sneak,           &gsn_detect_invis,      90,     PRE_THIEF},
  {&gsn_detect_sneak,           &gsn_sneak,             90,     PRE_THIEF},
  {&gsn_detect_sneak,           &gsn_spellcraft,        90,     PRE_THIEF},

  {&gsn_detect_magic,           &gsn_detect_invis,      80,     PRE_THIEF},
  {&gsn_detect_magic,           &gsn_spellcraft,        65,     PRE_THIEF},
        
  {&gsn_invis,                  &gsn_detect_invis,      80,     PRE_THIEF},
  {&gsn_invis,                  &gsn_spellcraft,        80,     PRE_THIEF},

/* Stealth skills.. */

  {&gsn_sneak,                  &gsn_hide,              20,     PRE_THIEF},
  {&gsn_sneak,                  &gsn_group_stealth,     20,     PRE_THIEF},
  {&gsn_sneak,                  &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_steal,                  &gsn_peek,              20,     PRE_THIEF},
  {&gsn_steal,                  &gsn_sneak,             40,     PRE_THIEF},
  {&gsn_steal,                  &gsn_group_stealth,     40,     PRE_THIEF},

  {&gsn_backstab,               &gsn_sneak,             40,     PRE_THIEF},
  {&gsn_backstab,               &gsn_group_stealth,     60,     PRE_THIEF},
  {&gsn_backstab,               &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_double_backstab,        &gsn_thief_base,        60,     PRE_THIEF},
  {&gsn_double_backstab,        &gsn_backstab,          85,     PRE_THIEF},
  {&gsn_double_backstab,        &gsn_group_armed,       85,     PRE_THIEF},
  {&gsn_double_backstab,        &gsn_group_stealth,     85,     PRE_THIEF},

  {&gsn_tail,                   &gsn_sneak,             80,     PRE_THIEF},
  {&gsn_tail,                   &gsn_group_stealth,     80,     PRE_THIEF},
  {&gsn_tail,                   &gsn_thief_base,        30,     PRE_THIEF},


/* Thief Skills */

  {&gsn_climb,                  &gsn_group_thievery,    20,     PRE_THIEF},
  {&gsn_climb,                  &gsn_thief_base,        30,     PRE_THIEF},

/* Doubt these work at this stage...*/

  {&gsn_find_traps,             &gsn_pick_lock,         20,     PRE_THIEF},
  {&gsn_find_traps,             &gsn_group_thievery,    40,     PRE_THIEF},     
  {&gsn_find_traps,             &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_disable,                &gsn_find_traps,        40,     PRE_THIEF},
  {&gsn_disable,                &gsn_group_thievery,    60,     PRE_THIEF},

/* Defense Skills */

  {&gsn_dodge,                  &gsn_group_defense,     20,     PRE_THIEF},
  {&gsn_dodge,                  &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_parry,                  &gsn_dodge,             40,     PRE_THIEF},
  {&gsn_parry,                  &gsn_group_defense,     40,     PRE_THIEF},
  {&gsn_parry,                  &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_disarm,                 &gsn_dodge,             40,     PRE_THIEF},
  {&gsn_disarm,                 &gsn_parry,             40,     PRE_THIEF},
  {&gsn_disarm,                 &gsn_group_defense,     60,     PRE_THIEF},
  {&gsn_disarm,                 &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_acrobatics,             &gsn_dodge,             90,     PRE_THIEF},
  {&gsn_acrobatics,             &gsn_parry,             90,     PRE_THIEF},
  {&gsn_acrobatics,             &gsn_group_defense,     85,     PRE_THIEF},
  {&gsn_acrobatics,             &gsn_thief_base,        60,     PRE_THIEF},

/* Combat Skills */

  {&gsn_second_attack,          &gsn_group_armed,       20,     PRE_THIEF},

  {&gsn_trip,                   &gsn_group_unarmed,     20,     PRE_THIEF},
  {&gsn_trip,                   &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_dirt,                   &gsn_trip,              60,     PRE_THIEF},
  {&gsn_dirt,                   &gsn_group_unarmed,     60,     PRE_THIEF},     
  {&gsn_dirt,                   &gsn_thief_base,        30,     PRE_THIEF},
        
  {&gsn_circle,                 &gsn_trip,              70,     PRE_THIEF},
  {&gsn_circle,                 &gsn_group_armed,       70,     PRE_THIEF},
  {&gsn_circle,                 &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_third_attack,           &gsn_second_attack,     60,     PRE_THIEF},     
  {&gsn_third_attack,           &gsn_group_armed,       60,     PRE_THIEF},
  {&gsn_third_attack,           &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_feint,                  &gsn_group_armed,       80,     PRE_THIEF},
  {&gsn_feint,                  &gsn_thief_base,        30,     PRE_THIEF},

  {&gsn_dual,                   &gsn_third_attack,      95,     PRE_THIEF},
  {&gsn_dual,                   &gsn_second_attack,     95,     PRE_THIEF},
  {&gsn_dual,                   &gsn_group_armed,       90,     PRE_THIEF},
  {&gsn_dual,                   &gsn_thief_base,        60,     PRE_THIEF},

/* **** HIGH LEVEL **** */

  {&gsn_poison_weapon,          &gsn_thief_base,        74,     PRE_THIEF},

  {&gsn_second_circle,          &gsn_group_armed,       85,     PRE_THIEF},
  {&gsn_second_circle,          &gsn_group_stealth,     85,     PRE_THIEF},
  {&gsn_second_circle,          &gsn_circle,            85,     PRE_THIEF},
  {&gsn_second_circle,          &gsn_thief_base,        60,     PRE_THIEF},

