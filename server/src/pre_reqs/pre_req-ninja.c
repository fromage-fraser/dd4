/* Ninja Skills */
/* 20 pre-reqs last count 23:00 27/3 - Shade */

/* **** HUNTING **** */

{&gsn_group_hunting,		&gsn_group_thievery,	85,	PRE_NINJA},
{&gsn_group_hunting,		&gsn_ninja_base,	30,	PRE_NINJA},

{&gsn_trap,			&gsn_group_hunting,	50,	PRE_NINJA},
{&gsn_trap,			&gsn_ninja_base,	31,	PRE_NINJA},

{&gsn_group_poison,		&gsn_ninja_base,	31,	PRE_NINJA},

/* **** Poison Skills **** */

{&gsn_poison,			&gsn_group_poison,	30,	PRE_NINJA},

{&gsn_poison_weapon,		&gsn_group_poison,	60,	PRE_NINJA},
{&gsn_poison_weapon,		&gsn_poison,		50,	PRE_NINJA},

{&gsn_cure_poison,		&gsn_group_poison,	80,	PRE_NINJA},
{&gsn_cure_poison,		&gsn_poison,		75,	PRE_NINJA},

/* **** Armed Combat **** */

{&gsn_dual,			&gsn_group_armed,	90,	PRE_NINJA},
{&gsn_dual,			&gsn_third_attack,	92,	PRE_NINJA},
{&gsn_dual,			&gsn_second_attack,	92,	PRE_NINJA},
{&gsn_dual,			&gsn_ninja_base,	50,	PRE_NINJA},

/* **** Advanced Combat Skills **** */

{&gsn_group_advcombat,		&gsn_group_armed,	85,	PRE_NINJA},
{&gsn_group_advcombat,		&gsn_ninja_base,	30,	PRE_NINJA},

{&gsn_fourth_attack,		&gsn_group_advcombat,	65,	PRE_NINJA},
{&gsn_fourth_attack,		&gsn_ninja_base,	31,	PRE_NINJA},
{&gsn_fourth_attack,		&gsn_second_attack,	90,	PRE_NINJA},
{&gsn_fourth_attack,		&gsn_third_attack,	90,	PRE_NINJA},

{&gsn_decapitate,		&gsn_group_advcombat,	90,	PRE_NINJA},
{&gsn_decapitate,		&gsn_fourth_attack,	95,	PRE_NINJA},
{&gsn_decapitate,		&gsn_ninja_base,	31,	PRE_NINJA},

{&gsn_enhanced_hit,		&gsn_group_advcombat,	95,	PRE_NINJA},
{&gsn_enhanced_hit,		&gsn_ninja_base,	31,	PRE_NINJA},

