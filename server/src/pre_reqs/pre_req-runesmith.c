/* Alchemist SubClass 30+

  22 Skills
 */

/* Rune Casting */

{&gsn_group_rune_casting,           &gsn_runesmith_base,            30,     PRE_RUNESMITH},

{&gsn_lightning_lunge,              &gsn_group_rune_casting,        70,     PRE_RUNESMITH},

{&gsn_fiery_strike,                 &gsn_group_rune_casting,        80,     PRE_RUNESMITH},
{&gsn_fiery_strike,                 &gsn_lightning_lunge,           80,     PRE_RUNESMITH},

{&gsn_death_strike,                 &gsn_group_rune_casting,        80,     PRE_RUNESMITH},
{&gsn_death_strike,                 &gsn_fiery_strike,              80,     PRE_RUNESMITH},

{&gsn_conducting_blast,             &gsn_group_rune_casting,        80,     PRE_RUNESMITH},
{&gsn_conducting_blast,             &gsn_death_strike,              80,     PRE_RUNESMITH},

/* Runic Arts: */

{&gsn_group_runic_arts,             &gsn_runesmith_base,            30,     PRE_RUNESMITH},

{&gsn_pyro_rune,                 	&gsn_group_runic_arts,          30,     PRE_RUNESMITH},
{&gsn_cryo_rune,                 	&gsn_group_runic_arts,          60,     PRE_RUNESMITH},
{&gsn_bolt_rune,                 	&gsn_group_runic_arts,          70,     PRE_RUNESMITH},
{&gsn_stab_rune,                 	&gsn_group_runic_arts,          80,     PRE_RUNESMITH},
{&gsn_rend_rune,           	        &gsn_group_runic_arts,          80,     PRE_RUNESMITH},

{&gsn_mend_rune,                    &gsn_group_runic_arts,          90,     PRE_RUNESMITH},
{&gsn_cure_rune,                 	&gsn_group_runic_arts,          95,     PRE_RUNESMITH},
{&gsn_ward_rune,                 	&gsn_group_inscription,         95,     PRE_RUNESMITH},


/* Inscription */
{&gsn_group_inscription, 	        &gsn_runesmith_base,	        30,	    PRE_RUNESMITH},
{&gsn_inscribe,                     &gsn_runesmith_base,            30,     PRE_RUNESMITH},
{&gsn_adamantite_runic_blade,       &gsn_group_inscription,         95,     PRE_RUNESMITH},
{&gsn_electrum_runic_blade,         &gsn_group_inscription,         95,     PRE_RUNESMITH},
{&gsn_starmetal_runic_blade,        &gsn_group_inscription,         95,     PRE_RUNESMITH},