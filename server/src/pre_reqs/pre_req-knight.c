/* **** KNIGHT PRE-REQS **** */

/* About 38 so far :-) - Shade */

/* **** INITIAL SKILLS **** */

{&gsn_armor,			&gsn_knight_base,	31,	PRE_KNIGHT},
{&gsn_group_conjuration,	&gsn_knight_base,	31,	PRE_KNIGHT},
{&gsn_group_healing,		&gsn_knight_base,	31,	PRE_KNIGHT},

{&gsn_flamestrike,		&gsn_knight_base,	31,	PRE_KNIGHT},

/* **** HEALING **** */

{&gsn_cure_light,		&gsn_group_healing,	30,	PRE_KNIGHT},

{&gsn_cure_serious,		&gsn_group_healing,	50,	PRE_KNIGHT},
{&gsn_cure_serious,		&gsn_cure_light,	50,	PRE_KNIGHT},
{&gsn_cure_serious,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_cure_critical,		&gsn_group_healing,	75,	PRE_KNIGHT},
{&gsn_cure_critical,		&gsn_cure_serious,	75,	PRE_KNIGHT},
{&gsn_cure_critical,		&gsn_cure_light,	75,	PRE_KNIGHT},
{&gsn_cure_critical,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_heal,			&gsn_cure_critical,	75,	PRE_KNIGHT},
{&gsn_heal,			&gsn_group_healing,	85,	PRE_KNIGHT},
{&gsn_heal,			&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_cure_poison,              &gsn_group_healing,     90,     PRE_KNIGHT},
{&gsn_cure_poison,              &gsn_knight_base,       30,     PRE_KNIGHT},

/* **** CONJURATION **** */	

{&gsn_create_water,		&gsn_group_conjuration,	30,	PRE_KNIGHT},
{&gsn_create_water,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_faerie_fire,		&gsn_group_conjuration,	50,	PRE_KNIGHT},
{&gsn_faerie_fire,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_create_food,		&gsn_group_conjuration,	70,	PRE_KNIGHT},
{&gsn_create_food,		&gsn_knight_base,	30,	PRE_KNIGHT},

/* **** KNOWLEDGE **** */

{&gsn_group_knowledge,		&gsn_knight_base,	30,	PRE_KNIGHT},
{&gsn_group_knowledge,		&gsn_group_healing,	80,	PRE_KNIGHT},
{&gsn_group_knowledge,		&gsn_group_conjuration,	80,	PRE_KNIGHT},

{&gsn_detect_hidden,		&gsn_group_knowledge,	30,	PRE_KNIGHT},
{&gsn_detect_hidden,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_detect_invis,		&gsn_group_knowledge,	50,	PRE_KNIGHT},
{&gsn_detect_invis,		&gsn_detect_hidden,	75,	PRE_KNIGHT},
{&gsn_detect_invis,		&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_advanced_consider,	&gsn_group_knowledge,	90,	PRE_KNIGHT},
{&gsn_advanced_consider,	&gsn_knight_base,	30,	PRE_KNIGHT},

/* **** MANA CONTROL **** */

{&gsn_group_mana,		&gsn_group_knowledge,	90,	PRE_KNIGHT},
{&gsn_group_mana,		&gsn_knight_base,	30,	PRE_KNIGHT},

/* **** ARMED COMBAT **** */

{&gsn_dual,			&gsn_group_armed,	90,	PRE_KNIGHT},
{&gsn_dual,			&gsn_third_attack,	93,	PRE_KNIGHT},
{&gsn_dual,			&gsn_second_attack,	93,	PRE_KNIGHT},
{&gsn_dual,			&gsn_knight_base,	50,	PRE_KNIGHT},

/* **** RIDING **** */

{&gsn_joust,			&gsn_group_riding,	70,	PRE_KNIGHT},
{&gsn_joust,			&gsn_mount,		75,	PRE_KNIGHT},
{&gsn_joust,			&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_destrier,                 &gsn_group_riding,      85, PRE_KNIGHT},
{&gsn_destrier,                 &gsn_mount,             85, PRE_KNIGHT},
{&gsn_destrier,                 &gsn_knight_base,       30, PRE_KNIGHT},

/* **** METAL WORKING **** */

{&gsn_group_metal,		&gsn_group_knowledge,	95,	PRE_KNIGHT},

{&gsn_sharpen,			&gsn_group_metal,	50,	PRE_KNIGHT},
{&gsn_sharpen,			&gsn_knight_base,	30,	PRE_KNIGHT},

{&gsn_forge,			&gsn_group_metal,	75,	PRE_KNIGHT},
{&gsn_forge,			&gsn_sharpen,		50,	PRE_KNIGHT},
{&gsn_forge,			&gsn_knight_base,	30,	PRE_KNIGHT},
