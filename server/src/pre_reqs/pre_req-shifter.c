
/* Last modified 1/6 - Mat */

/* Total Pre_Reqs == 37  */

/* Starting Skills - have decided what worldly is going to be yet?*/
{&gsn_group_forms,              &gsn_shifter_base,      30,     PRE_SHIFTER},

{&gsn_morph,                    &gsn_shifter_base,      30,     PRE_SHIFTER},
{&gsn_form_chameleon,           &gsn_group_forms,       20,     PRE_SHIFTER},

{&gsn_form_hawk,                &gsn_form_chameleon,    40,     PRE_SHIFTER},

{&gsn_form_cat,                 &gsn_form_hawk,         32,     PRE_SHIFTER},

{&gsn_form_snake,               &gsn_group_forms,       40,     PRE_SHIFTER},
{&gsn_form_snake,               &gsn_form_cat,          40,     PRE_SHIFTER},
{&gsn_coil,                     &gsn_form_snake,        30,     PRE_SHIFTER},
{&gsn_constrict,                &gsn_form_snake,        45,     PRE_SHIFTER},
{&gsn_strangle,                 &gsn_form_snake,        53,     PRE_SHIFTER},
{&gsn_strangle,                 &gsn_constrict,         53,     PRE_SHIFTER},

{&gsn_form_scorpion,            &gsn_form_snake,        50,     PRE_SHIFTER},
{&gsn_form_scorpion,            &gsn_group_forms,       50,     PRE_SHIFTER},

{&gsn_form_spider,              &gsn_group_forms,       50,     PRE_SHIFTER},
{&gsn_venom,                    &gsn_form_spider,       20,     PRE_SHIFTER},
{&gsn_web,                      &gsn_form_spider,       40,     PRE_SHIFTER},

{&gsn_form_bear,                &gsn_group_forms,       60,     PRE_SHIFTER},
{&gsn_forage,                   &gsn_form_bear,         20,     PRE_SHIFTER},

{&gsn_form_tiger,               &gsn_group_forms,       70,     PRE_SHIFTER},
{&gsn_form_tiger,               &gsn_form_bear,         70,     PRE_SHIFTER},
{&gsn_bite,                     &gsn_form_tiger,        40,     PRE_SHIFTER},
{&gsn_maul,                     &gsn_form_tiger,        60,     PRE_SHIFTER},

/* due to popular demand this has been added for straight shifters */
{&gsn_dodge,                    &gsn_shifter_base,      60,     PRE_SHIFTER},
{&gsn_second_attack,            &gsn_shifter_base,      60,     PRE_SHIFTER},

{ &gsn_form_hydra,              &gsn_form_tiger,        70,     PRE_SHIFTER},
{ &gsn_form_hydra,              &gsn_shifter_base,      60,     PRE_SHIFTER},
{ &gsn_breathe,                 &gsn_form_hydra,        50,     PRE_SHIFTER},

{ &gsn_form_phoenix,            &gsn_form_hydra,        80,     PRE_SHIFTER},
{ &gsn_form_phoenix,            &gsn_group_forms,       80,     PRE_SHIFTER},

{ &gsn_form_fly,                &gsn_group_forms,       85,     PRE_SHIFTER},
{ &gsn_form_fly,                &gsn_shifter_base,      60,     PRE_SHIFTER},

{ &gsn_form_griffin,            &gsn_form_hydra,        90,     PRE_SHIFTER},
{ &gsn_form_griffin,            &gsn_group_forms,       90,     PRE_SHIFTER},

{ &gsn_form_demon,              &gsn_form_hydra,        90,     PRE_SHIFTER},
{ &gsn_form_demon,              &gsn_group_forms,       90,     PRE_SHIFTER},

{ &gsn_summon_demon,            &gsn_form_demon,        80,     PRE_SHIFTER},

{ &gsn_rescue,                  &gsn_form_phoenix,      65,     PRE_SHIFTER},

{ &gsn_form_dragon,             &gsn_form_phoenix,      95,     PRE_SHIFTER},
{ &gsn_form_dragon,             &gsn_group_forms,       95,     PRE_SHIFTER},

{ &gsn_dive,                    &gsn_form_dragon,       70,     PRE_SHIFTER},
{ &gsn_tailwhip,                &gsn_form_dragon,       70,     PRE_SHIFTER},

{ &gsn_group_riding,            &gsn_shifter_base,      59,     PRE_SHIFTER},
