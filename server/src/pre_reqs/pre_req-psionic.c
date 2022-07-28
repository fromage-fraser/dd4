/* **** PSIONIC PRE_REQS **** */

/* **** STARTING SKILLS **** */

{&gsn_thought_shield,           &gsn_psionic_base,         30,     PRE_PSIONIC},
{&gsn_ballistic_attack,         &gsn_psionic_base,         30,     PRE_PSIONIC},
{&gsn_mind_thrust,              &gsn_psionic_base,         30,     PRE_PSIONIC},
{&gsn_levitation,               &gsn_psionic_base,         30,     PRE_PSIONIC},
{&gsn_group_mana,               &gsn_psionic_base,         30,     PRE_PSIONIC},
{&gsn_group_summoning,          &gsn_psionic_base,         30,     PRE_PSIONIC},
   
/* **** GROUPS **** */   
{&gsn_group_mentald,            &gsn_thought_shield,       10,     PRE_PSIONIC},
{&gsn_group_matter,             &gsn_ballistic_attack,     10,     PRE_PSIONIC},
{&gsn_group_telepathy,          &gsn_mind_thrust,          10,     PRE_PSIONIC},
{&gsn_group_energy,             &gsn_group_matter,         30,     PRE_PSIONIC},
{&gsn_group_body,               &gsn_levitation,           20,     PRE_PSIONIC},


/* Skills to Add : Decay. Also gotta do some of the high level skills, in fact 
most of them ? */

/* **** PROTECTION **** */

{&gsn_group_protection,         &gsn_psionic_base,         60,     PRE_PSIONIC},
   
{&gsn_fireshield,               &gsn_group_protection,     85,     PRE_PSIONIC},
{&gsn_fireshield,               &gsn_group_mentald,        80,     PRE_PSIONIC},
{&gsn_fireshield,               &gsn_control_flames,       75,     PRE_PSIONIC},
{&gsn_fireshield,               &gsn_psionic_base,         60,     PRE_PSIONIC},
   
{&gsn_globe,                    &gsn_group_protection,     95,     PRE_PSIONIC},
{&gsn_globe,                    &gsn_psionic_base,         60,     PRE_PSIONIC},
   
/* **** MENTAL DEFENSE **** */   
   
{&gsn_mental_barrier,           &gsn_group_mentald,        20,     PRE_PSIONIC},
{&gsn_displacement,             &gsn_group_mentald,        30,     PRE_PSIONIC},
{&gsn_flesh_armor,              &gsn_group_mentald,        40,     PRE_PSIONIC},
   
{&gsn_animate_weapon,           &gsn_group_mentald,        90,     PRE_PSIONIC},
{&gsn_animate_weapon,           &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_intellect_fortress,       &gsn_group_mentald,        50,     PRE_PSIONIC},
{&gsn_intellect_fortress,       &gsn_mental_barrier,       50,     PRE_PSIONIC},
   
{&gsn_biofeedback,              &gsn_group_mentald,        65,     PRE_PSIONIC},
{&gsn_biofeedback,              &gsn_flesh_armor,          50,     PRE_PSIONIC},
   
{&gsn_inertial_barrier,         &gsn_group_mentald,        70,     PRE_PSIONIC},
{&gsn_inertial_barrier,         &gsn_intellect_fortress,   60,     PRE_PSIONIC},
   
/* **** SUMMONING **** */   
   
{&gsn_teleport,                 &gsn_group_summoning,      30,     PRE_PSIONIC},
{&gsn_summon,                   &gsn_psionic_base,         30,     PRE_PSIONIC},
   
/* **** MATTER CONTROL **** */   
   
{&gsn_agitation,                &gsn_group_matter,         30,     PRE_PSIONIC},
{&gsn_control_flames,           &gsn_group_matter,         40,     PRE_PSIONIC},
{&gsn_project_force,            &gsn_group_matter,         55,     PRE_PSIONIC},
{&gsn_enhance_armor,            &gsn_group_matter,         62,     PRE_PSIONIC},
   
{&gsn_detonate,                 &gsn_group_matter,         70,     PRE_PSIONIC},
{&gsn_detonate,                 &gsn_control_flames,       50,     PRE_PSIONIC},
   
/* **** DIVINATION **** */   
   
{&gsn_group_divination,         &gsn_psionic_base,         30,     PRE_PSIONIC},
   
/* **** ENERGY CONTROL **** */   
   
{&gsn_create_sound,             &gsn_group_energy,         20,     PRE_PSIONIC},
{&gsn_shadow_form,              &gsn_group_energy,         20,     PRE_PSIONIC},
   
{&gsn_chameleon_power,          &gsn_group_energy,         30,     PRE_PSIONIC},
{&gsn_chameleon_power,          &gsn_shadow_form,          30,     PRE_PSIONIC},
{&gsn_chameleon_power,          &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_energy_containment,       &gsn_group_energy,         58,     PRE_PSIONIC},
{&gsn_energy_drain,             &gsn_group_energy,         62,     PRE_PSIONIC},
{&gsn_energy_drain,             &gsn_energy_containment,   50,     PRE_PSIONIC},
{&gsn_energy_drain,             &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_invis,                    &gsn_group_energy,         70,     PRE_PSIONIC},
{&gsn_invis,                    &gsn_chameleon_power,      70,     PRE_PSIONIC},
   
{&gsn_dispel_magic,             &gsn_group_energy,         80,     PRE_PSIONIC},
{&gsn_dispel_magic,             &gsn_psionic_base,         30,     PRE_PSIONIC},
   
/* **** TELEPATHY **** */   
   
{&gsn_psychic_drain,            &gsn_group_telepathy,      20,     PRE_PSIONIC},
{&gsn_inflict_pain,             &gsn_group_telepathy,      30,     PRE_PSIONIC},
   
{&gsn_psychic_crush,            &gsn_mind_thrust,          30,     PRE_PSIONIC},
{&gsn_psychic_crush,            &gsn_group_telepathy,      45,     PRE_PSIONIC},
   
{&gsn_ego_whip,                 &gsn_group_telepathy,      60,     PRE_PSIONIC},
{&gsn_ego_whip,                 &gsn_inflict_pain,         40,     PRE_PSIONIC},
   
{&gsn_combat_mind,              &gsn_group_telepathy,      63,     PRE_PSIONIC},
{&gsn_combat_mind,              &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_psionic_blast,            &gsn_group_telepathy,      65,     PRE_PSIONIC},
{&gsn_psionic_blast,            &gsn_psychic_crush,        50,     PRE_PSIONIC},
{&gsn_psionic_blast,            &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_inner_fire,               &gsn_group_telepathy,      90,     PRE_PSIONIC},
{&gsn_inner_fire,               &gsn_psionic_base,         30,     PRE_PSIONIC},
   
/* **** BODY CONTROL **** */   
   
{&gsn_psychic_healing,          &gsn_group_body,           30,     PRE_PSIONIC},
{&gsn_enhanced_strength,        &gsn_group_body,           40,     PRE_PSIONIC},
   
{&gsn_adrenaline_control,       &gsn_group_body,           40,     PRE_PSIONIC},
{&gsn_adrenaline_control,       &gsn_psychic_healing,      30,     PRE_PSIONIC},
   
{&gsn_lend_health,              &gsn_group_body,           50,     PRE_PSIONIC},
   
{&gsn_cell_adjustment,          &gsn_group_body,           50,     PRE_PSIONIC},
{&gsn_cell_adjustment,          &gsn_psychic_healing,      50,     PRE_PSIONIC},
   
{&gsn_share_strength,           &gsn_group_body,           60,     PRE_PSIONIC},
{&gsn_share_strength,           &gsn_lend_health,          40,     PRE_PSIONIC},
   
{&gsn_heighten,                 &gsn_group_body,           75,     PRE_PSIONIC},
   
{&gsn_ectoplasmic_form,         &gsn_group_body,           85,     PRE_PSIONIC},
   
/* **** RIDING **** */   
   
{&gsn_group_riding,             &gsn_psionic_base,         59,     PRE_PSIONIC},
   
/* **** ADVANCED TELEPATHY **** */   
   
{&gsn_group_advtele,            &gsn_group_telepathy,      90,     PRE_PSIONIC},
{&gsn_group_advtele,            &gsn_psionic_base,         60,     PRE_PSIONIC},
   
{&gsn_ultrablast,               &gsn_group_advtele,        50,     PRE_PSIONIC},

{&gsn_awe,                      &gsn_group_advtele,        80,     PRE_PSIONIC},

{&gsn_death_field,              &gsn_group_advtele,        90,     PRE_PSIONIC},

{&gsn_disintergrate,            &gsn_group_advtele,        100,    PRE_PSIONIC},
   
{&gsn_synaptic_blast,           &gsn_group_advtele,        95,     PRE_PSIONIC},
{&gsn_synaptic_blast,           &gsn_inner_fire,           95,     PRE_PSIONIC},
   
{&gsn_blink,                    &gsn_psionic_base,         30,     PRE_PSIONIC},
   
{&gsn_third_spell,              &gsn_group_mana,           100,    PRE_PSIONIC},
{&gsn_third_spell,              &gsn_second_spell,         100,    PRE_PSIONIC},
{&gsn_third_spell,              &gsn_psionic_base,         60,     PRE_PSIONIC},

