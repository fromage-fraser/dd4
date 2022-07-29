/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined( macintosh )
extern  int     _filbuf         args( (FILE *) );
#endif

#if !defined( ultrix )
#include <memory.h>
#endif


/*
 * Globals.
 */
HELP_DATA *             help_first;
HELP_DATA *             help_last;
SHOP_DATA *             shop_first;
SHOP_DATA *             shop_last;
GAME_DATA *		game_free;
GAME_DATA *		game_first;
GAME_DATA *		game_last;
CHAR_DATA *             char_free;
EXTRA_DESCR_DATA *      extra_descr_free;
NOTE_DATA *             note_free;
OBJ_DATA *              obj_free;
PC_DATA *               pcdata_free;
char                    bug_buf                 [ MAX_INPUT_LENGTH*2 ];
CHAR_DATA *             char_list;
char *                  help_greeting;
char                    log_buf                 [ MAX_INPUT_LENGTH*2 ];
KILL_DATA               kill_table              [ MAX_LEVEL          ];
OBJ_DATA *              object_list;
OBJSET_INDEX_DATA *     objset_list;
TIME_INFO_DATA          time_info;
WEATHER_DATA            weather_info;
char *                  down_time;
char *                  warning1;
char *                  warning2;
AUCTION_DATA *          auction;


/*
 * Tables - Geogg, Tavolir, Gezhp
 */
FAME_TABLE              fame_table[FAME_TABLE_LENGTH];
CLAN_PKILL_TABLE        clan_pkill_table[MAX_CLAN][CLAN_PKILL_TABLE_LENGTH];
CLAN_VANQ_TABLE         clan_vanq_table[MAX_CLAN][CLAN_VANQ_TABLE_LENGTH];
CLAN_MEMBER_TABLE       clan_member_table[MAX_CLAN][CLAN_MEMBER_TABLE_LENGTH];
HERO_DATA *             hero_first;
HERO_DATA *             hero_last;
LEGEND_DATA             legend_table[LEGEND_TABLE_LENGTH];
PKSCORE_DATA            pkscore_table[PKSCORE_TABLE_LENGTH];
WANTED_DATA             *wanted_list_first;
WANTED_DATA             *wanted_list_last;
INFAMY_TABLE            infamy_table[INFAMY_TABLE_LENGTH];

/*
 * Skill bases
 */
int     gsn_mage_base;
int     gsn_cleric_base;
int     gsn_thief_base;
int     gsn_warrior_base;
int     gsn_psionic_base;
int     gsn_shifter_base;
int     gsn_brawler_base;
int     gsn_ranger_base;
int     gsn_necro_base;
int     gsn_warlock_base;
int     gsn_templar_base;
int     gsn_druid_base;
int     gsn_ninja_base;
int     gsn_bounty_base;
int     gsn_thug_base;
int     gsn_knight_base;
int     gsn_satanist_base;
int     gsn_witch_base;
int     gsn_werewolf_base;
int     gsn_vampire_base;
int     gsn_monk_base;
int     gsn_martist_base;
int     gsn_barbarian_base;
int     gsn_bard_base;
int     gsn_smithy_base;
int     gsn_engineer_base;
int     gsn_alchemist_base;
int     gsn_teacher_base;


/*
 *  Regular skill gsns
 */
int     gsn_hunt;
int     gsn_transfix;
int     gsn_trap;
int     gsn_decapitate;
int     gsn_meditate;
int     gsn_knock;
int     gsn_flight;
int     gsn_assassinate;
int     gsn_backstab;
int     gsn_double_backstab;
int     gsn_circle;
int     gsn_second_circle;
int     gsn_constrict;
int     gsn_climb;
int     gsn_dirt;
int     gsn_blink;
int     gsn_dodge;
int     gsn_acrobatics;
int     gsn_dual;
int     gsn_hide;
int     gsn_peek;
int     gsn_pick_lock;
int     gsn_sneak;
int     gsn_tail;
int     gsn_stalk;
int     gsn_steal;
int     gsn_disable;
int     gsn_disarm;
int     gsn_poison_weapon;
int     gsn_stun;
int     gsn_trip;
int     gsn_advanced_consider;
int     gsn_bash;
int     gsn_berserk;
int     gsn_enhanced_damage;
int     gsn_enhanced_hit;
int     gsn_fast_healing;
int     gsn_headbutt;
int     gsn_second_headbutt;
int     gsn_kiai;
int     gsn_kick;
int     gsn_parry;
int     gsn_dual_parry;
int     gsn_pre_empt;
int     gsn_rescue;
int     gsn_second_attack;
int     gsn_third_attack;
int     gsn_fourth_attack;
int     gsn_feint;
int     gsn_find_traps;
int     gsn_second_spell;
int     gsn_third_spell;
int     gsn_fourth_spell;
int     gsn_unarmed_combat;
int     gsn_warcry;
int     gsn_whirlwind;
int     gsn_grip;
int     gsn_smash;
int     gsn_shield_block;
int     gsn_joust;
int     gsn_forge;
int     gsn_sharpen;
int     gsn_risposte;
int     gsn_destrier;
int     gsn_blindness;
int     gsn_brew;
int     gsn_charm_person;
int     gsn_curse;
int     gsn_detect_hidden;
int     gsn_detect_invis;
int     gsn_detect_sneak;
int     gsn_fireshield;
int     gsn_fly;
int     gsn_infravision;
int     gsn_intimidate;
int     gsn_extort;
int     gsn_invis;
int     gsn_entrapment;
int     gsn_mass_invis;
int     gsn_poison;
int     gsn_paralysis;
int     gsn_scribe;
int     gsn_sleep;
int     gsn_addfame;
int     gsn_addqp;
int     gsn_advance;
int     gsn_allow;
int     gsn_at;
int     gsn_bamfin;
int     gsn_bamfout;
int     gsn_ban;
int     gsn_cando;
int     gsn_deny;
int     gsn_disconnect;
int     gsn_echo;
int     gsn_force;
int     gsn_freeze;
int     gsn_goto;
int     gsn_holylight;
int     gsn_immtalk;
int     gsn_dirtalk;
int     gsn_killsocket;
int     gsn_leader;
int     gsn_guide;
int     gsn_log;
int     gsn_memory;
int     gsn_mfind;
int     gsn_mload;
int     gsn_mset;
int     gsn_oclanitem;
int     gsn_mstat;
int     gsn_mwhere;
int     gsn_newlock;
int     gsn_noemote;
int     gsn_notell;
int     gsn_numlock;
int     gsn_ofind;
int     gsn_osfind;
int     gsn_oload;
int     gsn_oset;
int     gsn_ostat;
int     gsn_osstat;
int     gsn_owhere;
int     gsn_pardon;
int     gsn_peace;
int     gsn_purge;
int     gsn_reboot;
int     gsn_recho;
int     gsn_rename;
int     gsn_reset;
int     gsn_restore;
int     gsn_return;
int     gsn_ronin;
int     gsn_rset;
int     gsn_rstat;
int     gsn_shutdown;
int     gsn_silence;
int     gsn_slay;
int     gsn_slookup;
int     gsn_snoop;
int     gsn_sset;
int     gsn_sstime;
int     gsn_switch;
int     gsn_transfer;
int     gsn_trust;
int     gsn_users;
int     gsn_wizhelp;
int     gsn_wizify;
int     gsn_wizinvis;
int     gsn_wizlock;
int     gsn_zones;
int     gsn_wizbrew;
int     gsn_chameleon_power;
int     gsn_domination;
int     gsn_heighten;
int     gsn_shadow_form;
int     gsn_satans_fury;
int     gsn_possession;
int     gsn_steal_strength;
int     gsn_hand_of_lucifer;
int     gsn_demon_flames;
int     gsn_steal_soul;
int     gsn_summon_demon;
int     gsn_gouge;
int     gsn_choke;
int     gsn_grapple;
int     gsn_flying_headbutt;
int     gsn_grab;
int     gsn_battle_aura;
int     gsn_punch;
int     gsn_second_punch;
int     gsn_push;
int     gsn_focus;
int     gsn_break_wrist;
int     gsn_snap_neck;
int     gsn_pugalism;
int     gsn_combo;
int     gsn_combo2;
int     gsn_combo3;
int     gsn_combo4;
int     gsn_atemi;
int     gsn_kansetsu;
int     gsn_tetsui;
int     gsn_shuto;
int     gsn_yokogeri;
int     gsn_mawasigeri;
int     gsn_fear;
int     gsn_deter;
int     gsn_astral_sidestep;
int     gsn_mist_walk;
int     gsn_gaias_warning;
int     gsn_resist_magic;
int     gsn_resist_toxin;
int     gsn_summon_avatar;
int     gsn_feed;
int     gsn_suck;
int     gsn_lunge;
int     gsn_aura_of_fear;
int     gsn_double_lunge;
int     gsn_rage;
int     gsn_howl;
int     gsn_mount;
int     gsn_dismount;
int     gsn_sunray;
int     gsn_prayer;
int     gsn_frenzy;
int     gsn_bless;
int     gsn_mass_sanctuary;
int     gsn_mass_heal;
int     gsn_mass_power_heal;
int     gsn_swim;
int     gsn_dowse;
int     gsn_thrust;
int     gsn_shoot;
int     gsn_second_shot;
int     gsn_third_shot;
int     gsn_accuracy;
int     gsn_snare;
int     gsn_classify;
int     gsn_gather_herbs;
int     gsn_group_archery;
int     gsn_group_music;
int     gsn_group_herb_lore;
int     gsn_song_of_revelation;
int     gsn_song_of_rejuvenation;
int     gsn_song_of_tranquility;
int     gsn_song_of_shadows;
int     gsn_song_of_sustenance;
int     gsn_song_of_flight;
int     gsn_chant_of_protection;
int     gsn_chant_of_battle;
int     gsn_chant_of_vigour;
int     gsn_chant_of_entrapment;
int     gsn_chant_of_enfeeblement;
int     gsn_chant_of_pain;
int     gsn_breathe_water;
int     gsn_morph;
int     gsn_spy;
int     gsn_coil;
int     gsn_strangle;
int     gsn_forage;
int     gsn_bite;
int     gsn_maul;
int     gsn_wolfbite;
int     gsn_ravage;
int     gsn_web;
int     gsn_venom;
int     gsn_breathe;
int     gsn_dive;
int     gsn_tailwhip;
int     gsn_dragon_aura;
int     gsn_gate;
int     gsn_levitation;
int     gsn_locate_object;
int     gsn_pass_door;
int     gsn_summon;
int     gsn_summon_familiar;
int     gsn_teleport;
int     gsn_sense_traps;
int     gsn_word_of_recall;
int     gsn_chill_touch;
int     gsn_dispel_evil;
int     gsn_dispel_magic;
int     gsn_energy_drain;
int     gsn_harm;
int     gsn_haste;
int     gsn_weaken;
int     gsn_feeblemind;
int     gsn_spiritwrack;
int     gsn_wither;
int     gsn_bladethirst;
int     gsn_hex;
int     gsn_death_field;
int     gsn_decay;
int     gsn_disintergrate;
int     gsn_imflict_pain;
int     gsn_psychic_drain;
int     gsn_acid_blast;
int     gsn_inner_fire;
int     gsn_synaptic_blast;
int     gsn_prismatic_spray;
int     gsn_holy_word;
int     gsn_unholy_word;
int     gsn_armor;
int     gsn_burning_hands;
int     gsn_dragon_shield;
int     gsn_call_lightning;
int     gsn_chain_lighhtning;
int     gsn_colour_spray;
int     gsn_continual_light;
int     gsn_control_weather;
int     gsn_earthquake;
int     gsn_fireball;
int     gsn_flamestrike;
int     gsn_giant_strength;
int     gsn_lightning_bolt;
int     gsn_magic_missile;
int     gsn_protection;
int     gsn_sanctuary;
int     gsn_shield;
int     gsn_shocking_grasp;
int     gsn_stone_skin;
int     gsn_acid_breath;
int     gsn_fire_breath;
int     gsn_frost_breath;
int     gsn_gas_breath;
int     gsn_lightning_breath;
int     gsn_general_purpose;
int     gsn_high_explosive;
int     gsn_moonray;
int     gsn_natures_fury;
int     gsn_dark_globe;
int     gsn_globe;
int     gsn_firestorm;
int     gsn_meteor_storm;
int     gsn_ultrablast;
int     gsn_control_flames;
int     gsn_create_sound;
int     gsn_detonate;
int     gsn_ballistic_attack;
int     gsn_ego_whip;
int     gsn_energy_containment;
int     gsn_intellect_fortress;
int     gsn_mental_barrier;
int     gsn_project_force;
int     gsn_psychic_crush;
int     gsn_thought_shield;
int     gsn_astral_vortex;
int     gsn_pattern;
int     gsn_cause_light;
int     gsn_cause_serious;
int     gsn_cause_critical;
int     gsn_change_sex;
int     gsn_cure_blindness;
int     gsn_cure_critical;
int     gsn_cure_light;
int     gsn_cure_poison;
int     gsn_cure_serious;
int     gsn_heal;
int     gsn_power_heal;
int     gsn_refresh;
int     gsn_weaken;
int     gsn_bark_skin;
int     gsn_harm;
int     gsn_adrenaline_control;
int     gsn_biofeedback;
int     gsn_cell_adjustment;
int     gsn_recharge_mana;
int     gsn_vitalize;
int     gsn_complete_healing;
int     gsn_ectoplasmic_form;
int     gsn_energy_containment;
int     gsn_enhanced_strength;
int     gsn_flesh_armor;
int     gsn_psychic_healing;
int     gsn_share_strength;
int     gsn_sense_traps;
int     gsn_call_lightning;
int     gsn_chain_lightning;
int     gsn_continual_light;
int     gsn_control_weather;
int     gsn_earthquake;
int     gsn_fireball;
int     gsn_flamestrike;
int     gsn_lightning_bolt;
int     gsn_moonray;
int     gsn_firestorm;
int     gsn_meteor_storm;
int     gsn_animate_dead;
int     gsn_create_food;
int     gsn_create_water;
int     gsn_create_spring;
int     gsn_agitation;
int     gsn_enhance_armor;
int     gsn_adrenaline_control;
int     gsn_agitation;
int     gsn_aura_sight;
int     gsn_awe;
int     gsn_ballistic_attack;
int     gsn_biofeedback;
int     gsn_cell_adjustment;
int     gsn_combat_mind;
int     gsn_recharge_mana;
int     gsn_complete_healing;
int     gsn_control_flames;
int     gsn_create_sound;
int     gsn_death_field;
int     gsn_decay;
int     gsn_detonate;
int     gsn_disintergrate;
int     gsn_displacement;
int     gsn_ectoplasmic_form;
int     gsn_ego_whip;
int     gsn_energy_containment;
int     gsn_enhance_armor;
int     gsn_ehanced_strength;
int     gsn_flesh_armor;
int     gsn_inertial_barrier;
int     gsn_inflict_pain;
int     gsn_intellect_fortress;
int     gsn_lend_health;
int     gsn_levitation;
int     gsn_mental_barrier;
int     gsn_mind_thrust;
int     gsn_project_force;
int     gsn_psionic_blast;
int     gsn_psychic_crush;
int     gsn_psychic_drain;
int     gsn_psychich_healing;
int     gsn_share_strength;
int     gsn_thought_shield;
int     gsn_ultrablast;
int     gsn_animate_weapon;
int     gsn_dark_ritual;
int     gsn_transport;
int     gsn_know_alignment;
int     gsn_ventriloquate;
int     gsn_feeblemind;
int     gsn_enchant_weapon;
int     gsn_detect_evil;
int     gsn_detect_good;
int     gsn_detect_magic;
int     gsn_detect_poison;
int     gsn_faerie_fire;
int     gsn_faerie_fog;
int     gsn_know_alignment;
int     gsn_identify;
int     gsn_wrath_of_god;
int     gsn_bless_weapon;
int     gsn_recharge_item;
int     gsn_remove_curse;
int     gsn_lore;
int     gsn_aura_sight;
int     gsn_enhance_armor;
int     gsn_word_of_recall;
int     gsn_pantheoninfo;
int     gsn_pantheonrank;
int     gsn_prayer_combat;
int     gsn_prayer_weaken;
int     gsn_prayer_protection;
int     gsn_prayer_passage;
int     gsn_prayer_destruction;
int     gsn_prayer_plague;
int     gsn_resist_heat;
int     gsn_resist_cold;
int     gsn_resist_lightning;
int     gsn_resist_acid;
int     gsn_hells_fire;
int     gsn_chaos_blast;
int     gsn_detect_curse;
int     gsn_knife_toss;
int     gsn_soar;
int     gsn_smoke_bomb;
int     gsn_snap_shot;
int     gsn_crush;
int     gsn_swoop;
int     gsn_smelt;
int     gsn_strengthen;
int     gsn_imbue;
int     gsn_empower;
int     gsn_uncommon_set;
int     gsn_rare_set;
int     gsn_epic_set;
int     gsn_legendary_set;
int     gsn_repelling;
int     gsn_craft_weapon;
int     gsn_counterbalance;
int     gsn_weaponchain;
int     gsn_shieldchain;
int     gsn_hurl;
int     gsn_serate;
int     gsn_engrave;
int     gsn_discharge;
int     gsn_trigger;
int     gsn_dart;
int     gsn_arrow;
int     gsn_turret;
int     gsn_launcher;
int     gsn_reflector;
int     gsn_arrestor;
int     gsn_driver;
int     gsn_emergency;
int     gsn_deploy;
int     gsn_forager;
int     gsn_spyglass;
int     gsn_base;
int     gsn_miner;
int     gsn_hurl;
int     gsn_fire_flask;
int     gsn_frost_flask;
int     gsn_stun_flask;
int     gsn_blind_flask;
int     gsn_lightning_flask;
int     gsn_acid_flask;
int     gsn_bmf_flask;
int     gsn_inscribe;
int     gsn_enhancement;
int     gsn_healing;
int     gsn_ward;

/*
 *  Spell groups
 */
int     gsn_group_evocation;
int     gsn_group_dark;
int     gsn_group_death;
int     gsn_group_lycanthropy;
int     gsn_group_vampyre;
int     gsn_group_breath;
int     gsn_group_destruction;
int     gsn_group_majorp;
int     gsn_group_craft;
int     gsn_group_divination;
int     gsn_group_alteration;
int     gsn_group_summoning;
int     gsn_group_illusion;
int     gsn_group_enchant;
int     gsn_group_conjuration;
int     gsn_group_protection;
int     gsn_group_divine;
int     gsn_group_disease;
int     gsn_group_harmful;
int     gsn_group_healing;
int     gsn_group_advanced_heal;
int     gsn_group_curative;
int     gsn_group_mana;
int     gsn_group_mentald;
int     gsn_group_matter;
int     gsn_group_energy;
int     gsn_group_telepathy;
int     gsn_group_advtele;
int     gsn_group_body;
int     gsn_group_forms;
int     gsn_group_stealth;
int     gsn_group_hunting;
int     gsn_group_arts;
int     gsn_group_combos;
int     gsn_group_riding;
int     gsn_group_poison;
int     gsn_group_combat;
int     gsn_group_thievery;
int     gsn_group_armed;
int     gsn_group_unarmed;
int     gsn_group_inner;
int     gsn_group_knowledge;
int     gsn_group_nature;
int     gsn_group_defense;
int     gsn_group_pugalism;
int     gsn_group_metal;
int     gsn_group_advcombat;
int     gsn_spellcraft;
int     gsn_group_morph;
int     gsn_group_resistance;
int     gsn_group_last;
int     gsn_group_weaponsmith;
int     gsn_group_armoursmith;
int     gsn_group_inscription;
int     gsn_group_alchemy;
int     gsn_group_turret_tech;
int     gsn_group_mech_tech;


/*
 *  Form knowledge gsns
 */
int     gsn_form_chameleon;
int     gsn_form_hawk;
int     gsn_form_cat;
int     gsn_form_snake;
int     gsn_form_bat;
int     gsn_form_scorpion;
int     gsn_form_spider;
int     gsn_form_bear;
int     gsn_form_tiger;
int     gsn_form_dragon;
int     gsn_form_demon;
int     gsn_form_phoenix;
int     gsn_form_hydra;
int     gsn_form_wolf;
int     gsn_form_direwolf;
int     gsn_form_elemental_air;
int     gsn_form_elemental_water;
int     gsn_form_elemental_fire;
int     gsn_form_elemental_earth;
int     gsn_form_fly;
int     gsn_form_griffin;


/*
 * Locals.
 */
MOB_INDEX_DATA *        mob_index_hash          [ MAX_KEY_HASH ];
OBJ_INDEX_DATA *        obj_index_hash          [ MAX_KEY_HASH ];
OBJSET_INDEX_DATA *     objset_index_hash       [ MAX_KEY_HASH ];
ROOM_INDEX_DATA *       room_index_hash         [ MAX_KEY_HASH ];
char *                  string_hash             [ MAX_KEY_HASH ];

AREA_DATA *             area_first;
AREA_DATA *             area_last;

char *                  string_space;
char *                  top_string;
char                    str_empty               [ 1 ];

int                     top_affect;
int                     top_area;
int                     top_ed;
int                     top_exit;
int                     top_help;
int                     top_mob_index;
int                     top_obj_index;
int                     top_objset_index;
int                     top_reset;
int                     top_room;
int                     top_shop;
int			top_game;


int             mprog_name_to_type      (char* name);
MPROG_DATA *    mprog_file_read         (char* f, MPROG_DATA* mprg, MOB_INDEX_DATA *pMobIndex);
void            load_mobprogs           (FILE *fp);
void            mprog_read_programs     (FILE *fp, MOB_INDEX_DATA *pMobIndex);


/*
 * Memory management.
 * Increase MAX_STRING if you have to.
 * Tune the others only if you understand what you're doing.
 */
#define                 MAX_STRING      5500000     /* 5000000 */

#if defined( machintosh )
# define                        MAX_PERM_BLOCK  131072
# define                        MAX_MEM_LIST    11

void *                  rgFreeList              [ MAX_MEM_LIST       ];
const int               rgSizeList              [ MAX_MEM_LIST       ]  =
{
        16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};

#else
# define                        MAX_PERM_BLOCK  131072
# define                        MAX_MEM_LIST    12

void *                  rgFreeList              [ MAX_MEM_LIST       ];
const int               rgSizeList              [ MAX_MEM_LIST       ]  =
{
        16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
};
#endif

int                     nAllocString;
int                     sAllocString;
int                     nAllocPerm;
int                     sAllocPerm;


/*
 * Semi-locals.
 */
bool                    fBootDb;
FILE *                  fpArea;
char                    strArea                 [ MAX_INPUT_LENGTH   ];


/*
 * Local booting procedures.
 */
void    init_mm                 args( ( void ) );
void    load_area               args( ( FILE *fp ) );
void    load_area_special       args( ( FILE *fp ) );
void    load_helps              args( ( FILE *fp ) );
void    load_recall             args( ( FILE *fp ) );
void    load_mobiles            args( ( FILE *fp ) );
void    load_objects            args( ( FILE *fp ) );
void    load_resets             args( ( FILE *fp ) );
void    load_object_sets        args( ( FILE *fp ) );
void    load_rooms              args( ( FILE *fp ) );
void    load_shops              args( ( FILE *fp ) );
void    load_specials           args( ( FILE *fp ) );
void	load_games	        args( ( FILE *fp ) );
void    load_notes              args( ( void ) );
void    load_down_time          args( ( void ) );
void    fix_exits               args( ( void ) );


/*
 * Big mama top level function.
 */
void boot_db( void )
{
        /*
         * Init some data space stuff.
         */
        {
                if ( !( string_space = calloc( 1, MAX_STRING ) ) )
                {
                        bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
                        exit( 1 );
                }
                top_string      = string_space;
                fBootDb         = TRUE;
        }

        /*
         * Init random number generator.
         */
        {
                init_mm( );
        }

        /*
         * Set time and weather.
         */
        {
                long lhour, lday, lmonth;

                lhour = ( current_time - 650336715 ) / ( PULSE_TICK / PULSE_PER_SECOND );
                time_info.hour  = lhour  % 24;
                lday            = lhour  / 24;
                time_info.day   = lday   % 35;
                lmonth          = lday   / 35;
                time_info.month = lmonth % 17;
                time_info.year  = lmonth / 17;

                if ( time_info.hour <  5 )
                        weather_info.sunlight = SUN_DARK;
                else if ( time_info.hour <  6 )
                        weather_info.sunlight = SUN_RISE;
                else if ( time_info.hour < 19 )
                        weather_info.sunlight = SUN_LIGHT;
                else if ( time_info.hour < 20 )
                        weather_info.sunlight = SUN_SET;
                else
                        weather_info.sunlight = SUN_DARK;

                if ( time_info.day <  8 )
                        weather_info.moonlight = MOON_NONE;
                else if ( time_info.day < 17 )
                        weather_info.moonlight = MOON_WAXING;
                else if ( time_info.day < 26 )
                        weather_info.moonlight = MOON_FULL;
                else
                        weather_info.moonlight  = MOON_WANING;

                weather_info.change     = 0;
                weather_info.mmhg       = 960;

                if ( time_info.month >= 7 && time_info.month <=12 )
                        weather_info.mmhg += number_range( 1, 50 );
                else
                        weather_info.mmhg += number_range( 1, 80 );

                if ( weather_info.mmhg <=  980 )
                        weather_info.sky = SKY_LIGHTNING;
                else if ( weather_info.mmhg <= 1000 )
                        weather_info.sky = SKY_RAINING;
                else if ( weather_info.mmhg <= 1020 )
                        weather_info.sky = SKY_CLOUDY;
                else
                        weather_info.sky = SKY_CLOUDLESS;

        }

        /*
         *  Auctioning
         */
        {
                auction = (AUCTION_DATA *) malloc (sizeof(AUCTION_DATA));
                if (!auction)
                {
                        bug ("malloc'ing AUCTION_DATA didn't give %d bytes", sizeof(AUCTION_DATA));
                        exit (1);
                }
                auction->item = NULL;
        }

        /*
         * Assign gsns for skills which have them.
         */
        {
                int sn;

                for ( sn = 0; sn < MAX_SKILL; sn++ )
                {
                        if ( skill_table[sn].pgsn )
                                *skill_table[sn].pgsn = sn;
                }
        }

        /*
         * Read in all the area files.
         */
        {
                FILE *fpList;

                if ( !( fpList = fopen( AREA_LIST, "r" ) ) )
                {
                        perror( AREA_LIST );
                        exit( 1 );
                }

                for ( ; ; )
                {
                        strcpy( strArea, fread_word( fpList ) );
                        if ( strArea[0] == '$' )
                                break;

                        if ( strArea[0] == '-' )
                        {
                                fpArea = stdin;
                        }
                        else
                        {
                                if ( !( fpArea = fopen( strArea, "r" ) ) )
                                {
                                        perror( strArea );
                                        exit( 1 );
                                }
                        }

                        for ( ; ; )
                        {
                                char *word;

                                if ( fread_letter( fpArea ) != '#' )
                                {
                                        bug( "Boot_db: # not found.", 0 );
                                        exit( 1 );
                                }

                                word = fread_word( fpArea );

                                if ( word[0] == '$' )
                                        break;
                                else if ( !str_cmp( word, "AREA"     ) )
                                        load_area    ( fpArea );
                                else if ( !str_cmp( word, "AREA_SPECIAL" ) )
                                        load_area_special ( fpArea );
                                else if ( !str_cmp( word, "HELPS"    ) )
                                        load_helps   ( fpArea );
                                else if ( !str_cmp( word, "RECALL"   ) )
                                        load_recall  ( fpArea );
                                else if ( !str_cmp( word, "MOBILES"  ) )
                                        load_mobiles ( fpArea );
                                else if ( !str_cmp( word, "MOBPROGS" ) )
                                        load_mobprogs( fpArea );
                                else if ( !str_cmp( word, "OBJECTS"  ) )
                                        load_objects ( fpArea );
                                else if ( !str_cmp( word, "RESETS"   ) )
                                        load_resets  ( fpArea );
                                else if ( !str_cmp( word, "OBJECT_SETS"   ) )
                                        load_object_sets  ( fpArea );
                                else if ( !str_cmp( word, "ROOMS"    ) )
                                        load_rooms   ( fpArea );
                                else if ( !str_cmp( word, "SHOPS"    ) )
                                        load_shops   ( fpArea );
                                else if ( !str_cmp( word, "SPECIALS" ) )
                                        load_specials( fpArea );
                                else if ( !str_cmp( word, "GAMES"    ) )
	                                load_games( fpArea );
                                else
                                {
                                        bug( "Boot_db: bad section name.", 0 );
                                        exit( 1 );
                                }
                        }

                        if ( fpArea != stdin )
                                fclose( fpArea );
                        fpArea = NULL;
                }
                fclose( fpList );
        }

        /*
         * Init fame table
         */
        {
                int i;

                for (i = 0; i < FAME_TABLE_LENGTH; i++)
                {
                        fame_table[i].fame = 0;
                        sprintf(fame_table[i].name, "*");
                }
        }

        /*
         * Init legend table
         */
        {
                int i;

                for (i = 0; i < LEGEND_TABLE_LENGTH; i++)
                {
                        sprintf(legend_table[i].name, "*");
                        legend_table[i].fame = 0;
                        legend_table[i].class = 0;
                        legend_table[i].subclassed = 0;
                }
        }

        /*
         * Init pkscore table
         */
        {
                int i;

                for (i = 0; i < PKSCORE_TABLE_LENGTH; i++)
                {
                        sprintf(pkscore_table[i].name, "*");
                        pkscore_table[i].pkills = 0;
                        pkscore_table[i].pkscore = 0;
                        pkscore_table[i].class = 0;
                        pkscore_table[i].subclassed = 0;
                }
        }

        /*
         * Init clan pkill table
         */
        {
                int i;
                int j;

                for (i = 0; i < MAX_CLAN; i++)
                {
                        for (j = 0; j < CLAN_PKILL_TABLE_LENGTH; j++)
                        {
                                clan_pkill_table[i][j].pkills = 0;
                                sprintf(clan_pkill_table[i][j].name, "*");
                        }
                }
        }

        /*
         * Init clan vanq table
         */
        {
                int i;
                int j;

                for (i = 0; i < MAX_CLAN; i++)
                {
                        for (j = 0; j < CLAN_VANQ_TABLE_LENGTH; j++)
                        {
                                clan_vanq_table[i][j].killed = 0;
                                sprintf(clan_vanq_table[i][j].name, "*");
                        }
                }
        }

        /*
         * Init infamy table
         */
        {
                int i;

                for (i = 0; i < INFAMY_TABLE_LENGTH; i++)
                {
                        infamy_table[i].kills = 0;
                        infamy_table[i].vnum = 0;
                        sprintf(infamy_table[i].name, "*");
                        sprintf(infamy_table[i].loc, "*");
                }
        }


        /*
         * Load all tables
         */
        {
                load_fame_table();
                load_hero_table();
                load_legend_table();
                load_pkscore_table();
                load_all_clan_tables();
                load_wanted_table();
                load_infamy_table();
        }

        /*
         * Initialise Deity system
         */
        {
                if (!init_pantheon() && deities_active())
                {
                        log_deity ("failed to init pantheon: deactivating Deity system");
                        deity_system_halted = TRUE;
                }
        }

        /*
         *  Tournament system
         */
        {
                tournament_init();
        }

        /*
         *  Init review buffers
         */
        {
                int i;

                for (i = 0; i < REVIEW_CHANNEL_COUNT; i++)
                        review_buffer_public[i][0].buffer[0] = '\0';

                for (i = 0; i < MAX_CLAN; i++)
                        review_buffer_clan[i][0].buffer[0] = '\0';
        }

        /*
         * Fix up exits.
         * Declare db booting over.
         * Reset all areas once.
         * Load up the notes file.
         */
        {
                fix_exits();
                fBootDb = FALSE;
                area_update();
                load_boards();
                save_notes();
                MOBtrigger = TRUE;
                load_down_time();
        }

        return;
}


/*
 *  Mobprogs
 */

int mprog_name_to_type ( char *name )
{
        if ( !str_cmp( name, "in_file_prog"   ) )       return IN_FILE_PROG;
        if ( !str_cmp( name, "act_prog"       ) )   return ACT_PROG;
        if ( !str_cmp( name, "speech_prog"    ) )       return SPEECH_PROG;
        if ( !str_cmp( name, "rand_prog"      ) )       return RAND_PROG;
        if ( !str_cmp( name, "fight_prog"     ) )       return FIGHT_PROG;
        if ( !str_cmp( name, "hitprcnt_prog"  ) )       return HITPRCNT_PROG;
        if ( !str_cmp( name, "death_prog"     ) )       return DEATH_PROG;
        if ( !str_cmp( name, "entry_prog"     ) )       return ENTRY_PROG;
        if ( !str_cmp( name, "greet_prog"     ) )       return GREET_PROG;
        if ( !str_cmp( name, "all_greet_prog" ) )       return ALL_GREET_PROG;
        if ( !str_cmp( name, "give_prog"      ) )       return GIVE_PROG;
        if ( !str_cmp( name, "bribe_prog"     ) )       return BRIBE_PROG;

        return( ERROR_PROG );
}


MPROG_DATA* mprog_file_read( char *f, MPROG_DATA *mprg, MOB_INDEX_DATA *pMobIndex )
{
        char        MOBProgfile[ MAX_INPUT_LENGTH ];
        MPROG_DATA *mprg2;
        FILE       *progfile;
        char        letter;
        bool        done = FALSE;

        sprintf( MOBProgfile, "%s%s", MOB_DIR, f );

        progfile = fopen( MOBProgfile, "r" );
        if ( !progfile )
        {
                bug( "Mob: %d couldnt open mobprog file", pMobIndex->vnum );
                exit( 1 );
        }

        mprg2 = mprg;

        switch ( letter = fread_letter( progfile ) )
        {
            case '>':
                break;

            case '|':
                bug( "empty mobprog file.", 0 );
                exit( 1 );
                break;

            default:
                bug( "in mobprog file syntax error.", 0 );
                exit( 1 );
                break;
        }

        while ( !done )
        {
                mprg2->type = mprog_name_to_type( fread_word( progfile ) );

                switch ( mprg2->type )
                {
                    case ERROR_PROG:
                        bug( "mobprog file type error", 0 );
                        exit( 1 );
                        break;

                    case IN_FILE_PROG:
                        bug( "mprog file contains a call to file.", 0 );
                        exit( 1 );
                        break;

                    default:
                        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
                        mprg2->arglist       = fread_string( progfile );
                        mprg2->comlist       = fread_string( progfile );

                        switch ( letter = fread_letter( progfile ) )
                        {
                            case '>':
                                mprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                                mprg2       = mprg2->next;
                                mprg2->next = NULL;
                                break;

                            case '|':
                                done = TRUE;
                                break;

                            default:
                                bug( "in mobprog file syntax error.", 0 );
                                exit( 1 );
                                break;
                        }
                        break;
                }
        }

        fclose( progfile );
        return mprg2;
}


/*
 * Snarf a MOBprogram section from an area file.
 */
void load_mobprogs( FILE *fp )
{
        MOB_INDEX_DATA *iMob;
        MPROG_DATA     *original;
        MPROG_DATA     *working;
        char            letter;
        int             value;
        int             stat;

        for (;;)
        {
                switch ( letter = fread_letter( fp ) )
                {
                    default:
                        bug( "Load_mobprogs: bad command '%c'.",letter);
                        exit(1);
                        break;

                    case 'S':
                    case 's':
                        fread_to_eol( fp );
                        return;

                    case '*':
                        fread_to_eol( fp );
                        break;

                    case 'M':
                    case 'm':
                        value = fread_number( fp, &stat );
                        if ( ( iMob = get_mob_index( value ) ) == NULL )
                        {
                                bug( "Load_mobprogs: vnum %d doesnt exist", value );
                                exit( 1 );
                        }

                        if ( ( original = iMob->mobprogs ) )
                                for ( ; original->next != NULL; original = original->next );

                        working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );

                        if ( original )
                                original->next = working;
                        else
                                iMob->mobprogs = working;

                        working = mprog_file_read( fread_word( fp ), working, iMob );
                        working->next = NULL;
                        fread_to_eol( fp );
                        break;
                }
        }

        return;
}


void mprog_read_programs( FILE *fp, MOB_INDEX_DATA *pMobIndex)
{
        MPROG_DATA *mprg;
        char        letter;
        bool        done = FALSE;

        if ( ( letter = fread_letter( fp ) ) != '>' )
        {
                bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
                exit( 1 );
        }

        pMobIndex->mobprogs = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
        mprg = pMobIndex->mobprogs;

        while ( !done )
        {
                mprg->type = mprog_name_to_type( fread_word( fp ) );

                switch ( mprg->type )
                {
                    case ERROR_PROG:
                        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
                        exit( 1 );
                        break;

                    case IN_FILE_PROG:
                        mprg = mprog_file_read( fread_string( fp ), mprg,pMobIndex );
                        fread_to_eol( fp );

                        switch ( letter = fread_letter( fp ) )
                        {
                            case '>':
                                mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                                mprg       = mprg->next;
                                mprg->next = NULL;
                                break;

                            case '|':
                                mprg->next = NULL;
                                fread_to_eol( fp );
                                done = TRUE;
                                break;

                            default:
                                bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
                                exit( 1 );
                                break;
                        }
                        break;

                    default:
                        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
                        mprg->arglist = fread_string( fp );
                        fread_to_eol( fp );
                        mprg->comlist = fread_string( fp );
                        fread_to_eol( fp );

                        switch ( letter = fread_letter( fp ) )
                        {
                            case '>':
                                mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                                mprg       = mprg->next;
                                mprg->next = NULL;
                                break;

                            case '|':
                                mprg->next = NULL;
                                fread_to_eol( fp );
                                done = TRUE;
                                break;

                            default:
                                bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
                                exit( 1 );
                                break;
                        }
                        break;
                }
        }

        return;
}


/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
        AREA_DATA *pArea;
        int stat;

        pArea               = alloc_perm( sizeof( *pArea ) );
        pArea->reset_first  = NULL;
        pArea->reset_last   = NULL;
        pArea->author       = fread_string( fp );
        pArea->name         = fread_string( fp );
        pArea->recall       = DEFAULT_RECALL;
        pArea->low_level    = fread_number(fp, &stat);
        pArea->high_level   = fread_number(fp, &stat);
        pArea->low_enforced = fread_number(fp, &stat);
        pArea->high_enforced = fread_number(fp, &stat);
        pArea->age          = 15;
        pArea->nplayer      = 0;
        pArea->low_r_vnum   = 0;
        pArea->low_o_vnum   = 0;
        pArea->low_m_vnum   = 0;
        pArea->hi_r_vnum    = 0;
        pArea->hi_o_vnum    = 0;
        pArea->hi_m_vnum    = 0;

        pArea->container_reset_timer = number_range (1, 10);
        pArea->area_flags = 0;
        pArea->exp_modifier = 100;

        if ( !area_first )
                area_first = pArea
                ;
        if (  area_last  )
                area_last->next = pArea;

        area_last       = pArea;
        pArea->next     = NULL;

        top_area++;
        return;
}


/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
        HELP_DATA *pHelp;
        int stat;

        for ( ; ; )
        {
                pHelp           = alloc_perm( sizeof( *pHelp ) );
                pHelp->level    = fread_number( fp, &stat );
                pHelp->keyword  = fread_string( fp );

                if ( pHelp->keyword[0] == '$' )
                        break;

                pHelp->text     = fread_string( fp );

                if ( !str_cmp( pHelp->keyword, "greeting" ) )
                        help_greeting = pHelp->text;
                if ( !help_first )
                        help_first = pHelp;
                if (  help_last  )
                        help_last->next = pHelp;

                help_last       = pHelp;
                pHelp->next     = NULL;
                top_help++;
        }

        return;
}


/*
 * Snarf a recall point.
 */
void load_recall( FILE *fp )
{
        AREA_DATA *pArea;
        char       buf [ MAX_STRING_LENGTH ];
        int        stat;

        pArea         = area_last;
        pArea->recall = fread_number( fp, &stat );

        if ( pArea->recall < 1 )
        {
                sprintf( buf, "Load_recall:  %s invalid recall point", pArea->name );
                bug( buf, 0 );
                pArea->recall = DEFAULT_RECALL;
        }

        return;
}


/*
 * Area specials:
 * Area-wide flags and exp modifier.
 * Gez 2001
 */
void load_area_special (FILE *fp)
{
        char buf [MAX_STRING_LENGTH];
        char *next;
        int num;
        int stat;

        while (1)
        {
                next = fread_word(fp);

                if (next[0] == '\0' || !str_cmp(next, "$"))
                        break;

                /* Keywords much better than numbers, aren't they? */

                if (!str_cmp(next, "school"))
                        SET_BIT(area_last->area_flags, AREA_FLAG_SCHOOL);

                else if (!str_cmp(next, "no_quest"))
                        SET_BIT(area_last->area_flags, AREA_FLAG_NO_QUEST);

                else if (!str_cmp(next, "hidden"))
                        SET_BIT(area_last->area_flags, AREA_FLAG_HIDDEN);

                else if (!str_cmp(next, "safe"))
                        SET_BIT(area_last->area_flags, AREA_FLAG_SAFE);

                else if (!str_cmp(next, "no_teleport"))
                        SET_BIT(area_last->area_flags, AREA_FLAG_NO_TELEPORT);

                else if (!str_cmp(next, "exp_mod"))
                {
                        num = fread_number(fp, &stat);

                        if (num < 0)
                        {
                                sprintf(buf, "load_area_special: area '%s' has bad exp_mod '%d' (ignoring)",
                                        area_last->name, num);
                                log_string(buf);
                        }
                        else
                                area_last->exp_modifier = num;
                }

                else
                {
                        sprintf(buf, "load_area_special: area '%s' has unknown flag '%s' (ignoring)",
                                area_last->name, next);
                        log_string(buf);
                }
        }
}


/*
 *  Load the teachers skills
 */
void load_teacher (FILE *fp, MOB_INDEX_DATA *pMobIndex)
{
        int sn;
        int value;
        int stat;
        char letter;
        char *buf;
        char buf2[100];

        letter = fread_letter( fp );

        /* init all skills to zero :) */
        for  ( sn = 0; sn < MAX_SKILL; sn++ )
                pMobIndex->skills->learned[sn] = 0;

        while (letter == '&')
        {
                value = fread_number( fp, &stat );
                buf = fread_word(fp);
                sn = skill_lookup(buf);

                if (sn < 0)
                {
                        sprintf(buf2, "[*****] BUG: Fread_teacher: unknown skill: mob vnum %d: %s (%d)",
                                pMobIndex->vnum, buf, value);
                        log_string (buf2);
                }
                else
                        pMobIndex->skills->learned[sn] = value;

                letter = fread_letter( fp );
        }

        ungetc( letter, fp );
}


/*
 * Snarf a mob section.
 */
void load_mobiles( FILE *fp )
{
        MOB_INDEX_DATA *pMobIndex;
        char buf[MAX_STRING_LENGTH];

        for ( ; ; )
        {
                char letter;
                int  vnum;
                int  iHash;
                int  stat;

                letter = fread_letter( fp );
                if ( letter != '#' )
                {
                        bug( "Load_mobiles: # not found.", 0 );
                        exit( 1 );
                }

                vnum = fread_number( fp, &stat );
                if ( vnum == 0 )
                        break;

                fBootDb = FALSE;
                if ( get_mob_index( vnum ) )
                {
                        bug( "Load_mobiles: vnum %d duplicated.", vnum );
                        exit( 1 );
                }
                fBootDb = TRUE;

                pMobIndex = alloc_perm( sizeof( *pMobIndex ) );
                pMobIndex->vnum = vnum;

                if ( fBootDb )
                {
                        if ( !area_last->low_m_vnum )
                                area_last->low_m_vnum       = vnum;
                        if ( vnum > area_last->hi_m_vnum )
                                area_last->hi_m_vnum        = vnum;
                }

                pMobIndex->player_name          = fread_string( fp );
                pMobIndex->short_descr          = fread_string( fp );
                pMobIndex->long_descr           = fread_string( fp );
                pMobIndex->description          = fread_string( fp );

                pMobIndex->long_descr[0]        = UPPER( pMobIndex->long_descr[0]  );
                pMobIndex->description[0]       = UPPER( pMobIndex->description[0] );

                pMobIndex->act                  = fread_number( fp, &stat ) | ACT_IS_NPC;

                pMobIndex->affected_by          = fread_number( fp, &stat );
                REMOVE_BIT (pMobIndex->affected_by, AFF_CHARM);

                pMobIndex->pShop                = NULL;
                pMobIndex->alignment            = fread_number( fp, &stat );
                letter                          = fread_letter( fp );
                pMobIndex->level                = number_fuzzy( fread_number( fp, &stat ) );

                fread_number( fp, &stat );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_letter( fp );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_letter( fp );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_letter( fp );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_letter( fp );   /* Unused */
                fread_number( fp, &stat );   /* Unused */

                pMobIndex->body_form = fread_number( fp, &stat );   /* Gezhp 99 */

                fread_number( fp, &stat );   /* Unused */
                fread_number( fp, &stat );   /* Unused */
                fread_number( fp, &stat );   /* Unused */

                pMobIndex->sex = fread_number( fp, &stat );

                if ( letter != 'S' )
                {
                        bug( "Load_mobiles: vnum %d non-S.", vnum );
                        exit( 1 );
                }

                /* This is for mob-programs - Brutus */

                letter = fread_letter( fp );
                if ( letter == '>' )
                {
                        ungetc( letter, fp );
                        mprog_read_programs( fp, pMobIndex );
                }
                else
                        ungetc( letter, fp );

                /* this is for teachers...... :)  geoff */

                pMobIndex->skills = NULL;

                letter = fread_letter( fp );
                if ( letter == '&' )
                {
                        pMobIndex->skills = alloc_perm(sizeof(LEARNED_DATA));
                        ungetc (letter, fp);
                        sprintf(buf, "[teacher] %s (vnum %d).",
                                pMobIndex->short_descr, pMobIndex->vnum);
                        log_string (buf);
                        load_teacher(fp, pMobIndex);
                }
                else
                        ungetc( letter, fp );

                iHash                   = vnum % MAX_KEY_HASH;
                pMobIndex->next         = mob_index_hash[iHash];
                mob_index_hash[iHash]   = pMobIndex;
                top_mob_index++;
                kill_table[URANGE( 0, pMobIndex->level, MAX_LEVEL-1 )].number++;

                if (pMobIndex->player_name[0] == '\0')
                {
                        char buf[MAX_STRING_LENGTH];

                        sprintf(buf, "Mob #%d (%s) has empty name string (ignores players)",
                                vnum,
                                pMobIndex->short_descr);
                        log_string(buf);
                }
        }
}


/*
 * Snarf an obj section.
 */
void load_objects( FILE *fp )
{
        OBJ_INDEX_DATA *pObjIndex;
        char buf [MAX_STRING_LENGTH];
        int stat;

        for ( ; ; )
        {
                char *value [ 4 ];
                char  letter;
                int   vnum;
                int   iHash;

                letter = fread_letter( fp );
                if ( letter != '#' )
                {
                        bug( "Load_objects: # not found.", 0 );
                        exit( 1 );
                }

                vnum = fread_number( fp, &stat );
                if ( vnum == 0 )
                        break;

                fBootDb = FALSE;
                if ( get_obj_index( vnum ) )
                {
                        bug( "Load_objects: vnum %d duplicated.", vnum );
                        exit( 1 );
                }

                fBootDb = TRUE;
                pObjIndex = alloc_perm( sizeof( *pObjIndex ) );
                pObjIndex->vnum = vnum;

                if ( fBootDb )
                {
                        if ( !area_last->low_o_vnum )
                                area_last->low_o_vnum = vnum;
                        if ( vnum > area_last->hi_o_vnum )
                                area_last->hi_o_vnum = vnum;
                }

                pObjIndex->name                 = fread_string( fp );
                pObjIndex->short_descr          = fread_string( fp );
                pObjIndex->description          = fread_string( fp );
                /* Action description */          fread_string( fp );
                pObjIndex->description[0]       = UPPER( pObjIndex->description[0] );
                pObjIndex->item_type            = fread_number( fp, &stat );
                pObjIndex->extra_flags          = fread_number( fp, &stat );


                if (IS_SET(pObjIndex->extra_flags, ITEM_TRAP) )
                {
                        pObjIndex->trap_eff             = fread_number( fp, &stat );
                        pObjIndex->trap_dam             = fread_number( fp, &stat );
                        pObjIndex->trap_charge          = fread_number( fp, &stat );
                }
                else
                {
                        pObjIndex->trap_dam            = 0;
                        pObjIndex->trap_eff            = 0;
                        pObjIndex->trap_charge         = 0;
                }

                if (IS_SET(pObjIndex->extra_flags, ITEM_EGO))
                {
                        sprintf(buf, "Found ego item: %s [%d]",
                                pObjIndex->short_descr,
                                pObjIndex->vnum);
                        log_string(buf);
                        pObjIndex->ego_flags = fread_number( fp, &stat );
                }
                else
                        pObjIndex->ego_flags = 0;

                pObjIndex->wear_flags           = fread_number( fp, &stat );
                value[0]                        = fread_string( fp );
                value[1]                        = fread_string( fp );
                value[2]                        = fread_string( fp );
                value[3]                        = fread_string( fp );
                pObjIndex->weight               = fread_number( fp, &stat );
                pObjIndex->cost                 = fread_number( fp, &stat );
                pObjIndex->level                = fread_number( fp, &stat );

                /*
                 * Check here for the redundancy of invisible light sources - Kahn
                 */
                if ( pObjIndex->item_type == ITEM_LIGHT
                    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
                {
                        bug( "Vnum %d : light source with ITEM_INVIS set", vnum );
                        REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
                }

                for ( ; ; )
                {
                        char letter;
                        letter = fread_letter( fp );

                        if ( letter == 'A' )
                        {
                                AFFECT_DATA *paf;

                                paf                     = alloc_perm( sizeof( *paf ) );
                                paf->type               = -1;
                                paf->duration           = -1;
                                paf->location           = fread_number( fp, &stat );
                                paf->modifier           = fread_number( fp, &stat );
                                paf->bitvector          = 0;
                                paf->next               = pObjIndex->affected;
                                pObjIndex->affected     = paf;
                                top_affect++;
                        }
                        else if ( letter == 'E' )
                        {
                                EXTRA_DESCR_DATA *ed;

                                ed                      = alloc_perm( sizeof( *ed ) );
                                ed->keyword             = fread_string( fp );
                                ed->description         = fread_string( fp );
                                ed->next                = pObjIndex->extra_descr;
                                pObjIndex->extra_descr  = ed;
                                top_ed++;
                        }
                        else
                        {
                                ungetc( letter, fp );
                                break;
                        }
                }

                /*
                 * Translate character strings *value[] into integers:  sn's for
                 * items with spells, or straight conversion for other items.
                 * - Thelonius
                 */
                switch ( pObjIndex->item_type )
                {
                    default:
                        pObjIndex->value[0] = atoi( value[0] );
                        pObjIndex->value[1] = atoi( value[1] );
                        pObjIndex->value[2] = atoi( value[2] );
                        pObjIndex->value[3] = atoi( value[3] );
                        break;

                    case ITEM_PILL:
                    case ITEM_PAINT:
                    case ITEM_POTION:
                    case ITEM_SCROLL:
                        pObjIndex->weight = 2;
                        pObjIndex->value[0] = atoi( value[0] );
                        pObjIndex->value[1] = skill_lookup( value[1] );
                        pObjIndex->value[2] = skill_lookup( value[2] );
                        pObjIndex->value[3] = skill_lookup( value[3] );
                        break;

                    case ITEM_STAFF:
                    case ITEM_WAND:
                        pObjIndex->value[0] = atoi( value[0] );
                        pObjIndex->value[1] = atoi( value[1] );
                        pObjIndex->value[2] = atoi( value[2] );
                        pObjIndex->value[3] = skill_lookup( value[3] );
                        break;
                }

                iHash                   = vnum % MAX_KEY_HASH;
                pObjIndex->next         = obj_index_hash[iHash];
                obj_index_hash[iHash]   = pObjIndex;
                top_obj_index++;
        }

        return;
}


/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
        RESET_DATA *pReset;
        int stat;

        if ( !area_last )
        {
                bug( "Load_resets: no #AREA seen yet.", 0 );
                exit( 1 );
        }

        for ( ; ; )
        {
                EXIT_DATA       *pexit;
                ROOM_INDEX_DATA *pRoomIndex;
                char             letter;

                letter = fread_letter( fp );

                if ( letter == 'S' )
                        break;

                if ( letter == '*' )
                {
                        fread_to_eol( fp );
                        continue;
                }

                pReset          = alloc_perm( sizeof( *pReset ) );
                pReset->command = letter;

                /*
                 * if_flag
                 * fread_number( fp, &stat );
                 * (replaced by arg0 assignment by Gezhp 2000
                 */

                pReset->arg0    = fread_number( fp, &stat );
                pReset->arg1    = fread_number( fp, &stat );
                pReset->arg2    = fread_number( fp, &stat );
                pReset->arg3    = ( letter == 'G' || letter == 'R' || letter == 'F' )
                        ? 0 : fread_number( fp, &stat );
                fread_to_eol( fp );

                /*
                 * Validate parameters.
                 * We're calling the index functions for the side effect.
                 */
                switch ( letter )
                {
                    default:
                        bug( "Load_resets: bad command '%c'.", letter );
                        exit( 1 );
                        break;

                    case 'M':
                        get_mob_index  ( pReset->arg1 );
                        get_room_index ( pReset->arg3 );
                        break;

                    case 'O':
                        get_obj_index  ( pReset->arg1 );
                        get_room_index ( pReset->arg3 );
                        break;

                    case 'I':
                        get_obj_index  ( pReset->arg0 );
                        get_room_index ( pReset->arg2 );

                        if (pReset->arg1 < 0)
                        {
                                bug("Load resets: 'I': item level invalid: %d",
                                    pReset->arg1);
                                exit(1);
                        }

                        if (pReset->arg3 < 1)
                        {
                                bug("Load resets: 'I': max count invalid: %d",
                                    pReset->arg3);
                                exit(1);
                        }

                        break;

                    case 'P':
                        get_obj_index  ( pReset->arg1 );
                        get_obj_index  ( pReset->arg3 );
                        break;

                    case 'F':
                        get_obj_index ( pReset->arg1 );
                        get_obj_index ( pReset->arg3 );
                    case 'G':
                    case 'E':
                        get_obj_index  ( pReset->arg1 );
                        break;

                    case 'D':
                        pRoomIndex = get_room_index( pReset->arg1 );

                        if (   pReset->arg2 < 0
                            || pReset->arg2 > 5
                            || !( pexit = pRoomIndex->exit[pReset->arg2] )
                            || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
                        {
                                bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
                                exit( 1 );
                        }

                        if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
                        {
                                bug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
                                exit( 1 );
                        }

                        break;

                    case 'R':
                        pRoomIndex = get_room_index( pReset->arg1 );

                        if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
                        {
                                bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
                                exit( 1 );
                        }

                        break;
                }

                if ( !area_last->reset_first )
                        area_last->reset_first      = pReset;
                if (  area_last->reset_last  )
                        area_last->reset_last->next = pReset;

                area_last->reset_last   = pReset;
                pReset->next            = NULL;
                top_reset++;
        }

        return;
}


void load_object_sets( FILE *fp )
{
        OBJSET_INDEX_DATA *pObjSetIndex;
 
        if ( !area_last )
        {
                bug( "Load_Object_sets: no #AREA seen yet.", 0 );
                exit( 1 );
        }

        for ( ; ; )
        {
                char  letter;
                int   vnum;
                int   stat;
                int   iHash;

                letter = fread_letter( fp );
                if ( letter != '#' )
                {
                        bug( "Load_objects: # not found.", 0 );
                        exit( 1 );
                }

                vnum = fread_number( fp, &stat );
                if ( vnum == 0 )
                        break;

                fBootDb = FALSE;
                if ( get_objset_index( vnum ) )
                {
                        bug( "Load_objects: vnum %d duplicated.", vnum );
                        exit( 1 );
                }

                fBootDb = TRUE;
                
                pObjSetIndex = alloc_perm( sizeof( *pObjSetIndex ) );
                pObjSetIndex->vnum = vnum;
                
                pObjSetIndex->name                 = fread_string( fp );
                pObjSetIndex->description          = fread_string( fp );
                pObjSetIndex->objects[0]             = fread_number( fp, &stat );
                pObjSetIndex->objects[1]             = fread_number( fp, &stat );
                pObjSetIndex->objects[2]             = fread_number( fp, &stat );
                pObjSetIndex->objects[3]             = fread_number( fp, &stat );
                pObjSetIndex->objects[4]             = fread_number( fp, &stat );
                pObjSetIndex->bonus_num[0]           = fread_number( fp, &stat );
                pObjSetIndex->bonus_num[1]           = fread_number( fp, &stat );
                pObjSetIndex->bonus_num[2]           = fread_number( fp, &stat );
                pObjSetIndex->bonus_num[3]           = fread_number( fp, &stat );
                pObjSetIndex->bonus_num[4]           = fread_number( fp, &stat );

                
                /*
                 * Validate parameters.
                 * We're calling the index functions for the side effect.
                 */
               for ( ; ; )
                {
                        char letter;
                        letter = fread_letter( fp );

                        if ( letter == 'A' )
                        {
                                AFFECT_DATA *paf;

                                paf                     = alloc_perm( sizeof( *paf ) );
                                paf->type               = -1;
                                paf->duration           = -1;
                                paf->location           = fread_number( fp, &stat );
                                paf->modifier           = fread_number( fp, &stat );
                                paf->bitvector          = 0;
                                paf->next               = pObjSetIndex->affected;
                                pObjSetIndex->affected     = paf;
                                top_affect++;
                        }
                        else
                        {
                                ungetc( letter, fp );
                                break;
                        }
                }
                iHash                   = vnum % MAX_KEY_HASH;
                pObjSetIndex->next         = objset_index_hash[iHash];
                objset_index_hash[iHash]   = pObjSetIndex;
                top_objset_index++;
        }
        return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
        ROOM_INDEX_DATA *pRoomIndex;
        int stat;

        if ( !area_last )
        {
                bug( "Load_resets: no #AREA seen yet.", 0 );
                exit( 1 );
        }

        for ( ; ; )
        {
                char letter;
                int  vnum;
                int  door;
                int  iHash;

                letter = fread_letter( fp );
                if ( letter != '#' )
                {
                        bug( "Load_rooms: # not found.", 0 );
                        exit( 1 );
                }

                vnum = fread_number( fp, &stat );
                if ( vnum == 0 )
                        break;

                fBootDb = FALSE;
                if ( get_room_index( vnum ) )
                {
                        bug( "Load_rooms: vnum %d duplicated.", vnum );
                        exit( 1 );
                }
                fBootDb = TRUE;

                pRoomIndex                      = alloc_perm( sizeof( *pRoomIndex ) );
                pRoomIndex->people              = NULL;
                pRoomIndex->contents            = NULL;
                pRoomIndex->extra_descr         = NULL;
                pRoomIndex->area                = area_last;
                pRoomIndex->vnum                = vnum;

                if ( fBootDb )
                {
                        if ( !area_last->low_r_vnum )
                                area_last->low_r_vnum           = vnum;
                        if ( vnum > area_last->hi_r_vnum )
                                area_last->hi_r_vnum            = vnum;
                }

                pRoomIndex->name                = fread_string( fp );
                pRoomIndex->description         = fread_string( fp );
                /* Area number */                 fread_number( fp, &stat );   /* Unused */
                pRoomIndex->room_flags          = fread_number( fp, &stat );
                pRoomIndex->sector_type         = fread_number( fp, &stat );
                pRoomIndex->light               = 0;

                for ( door = 0; door <= 5; door++ )
                        pRoomIndex->exit[door] = NULL;

                for ( ; ; )
                {
                        letter = fread_letter( fp );

                        if ( letter == 'S' || letter == 's' )
                        {
                                if ( letter == 's' )
                                        bug( "Load_rooms: vnum %d has lowercase 's'", vnum );
                                break;
                        }

                        if ( letter == 'D' )
                        {
                                EXIT_DATA *pexit;
                                int        locks;

                                door = fread_number( fp, &stat );
                                if ( door < 0 || door > 5 )
                                {
                                        bug( "Fread_rooms: vnum %d has bad door number.", vnum );
                                        exit( 1 );
                                }

                                pexit                   = alloc_perm( sizeof( *pexit ) );
                                pexit->description      = fread_string( fp );
                                pexit->keyword          = fread_string( fp );
                                pexit->exit_info        = 0;
                                locks                   = fread_number( fp, &stat );
                                pexit->key              = fread_number( fp, &stat );
                                pexit->vnum             = fread_number( fp, &stat );

                                switch ( locks )
                                {
                                    case 1:
                                        pexit->exit_info = EX_ISDOOR;
                                        break;

                                    case 2:
                                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;
                                        break;

                                    case 3:
                                        pexit->exit_info = EX_ISDOOR | EX_BASHPROOF;
                                        break;

                                    case 4:
                                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
                                                | EX_BASHPROOF;
                                        break;

                                    case 5:
                                        pexit->exit_info = EX_ISDOOR | EX_PASSPROOF;
                                        break;

                                    case 6:
                                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
                                                | EX_PASSPROOF;
                                        break;

                                    case 7:
                                        pexit->exit_info = EX_ISDOOR | EX_BASHPROOF
                                                | EX_PASSPROOF;
                                        break;

                                    case 8:
                                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
                                                | EX_BASHPROOF | EX_PASSPROOF;
                                        break;

                                    case 9:
                                        pexit->exit_info = EX_WALL;
                                        break;

                                    case 10:
                                        pexit->exit_info = EX_ISDOOR | EX_SECRET;
                                        break;

                                    case 11:
                                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
                                                | EX_BASHPROOF | EX_PASSPROOF | EX_SECRET;
                                        break;

                                    case 12:
                                        pexit->exit_info = EX_SECRET;
                                        break;
                                }

                                pRoomIndex->exit[door]   = pexit;
                                top_exit++;
                        }
                        else if ( letter == 'E' )
                        {
                                EXTRA_DESCR_DATA *ed;

                                ed                      = alloc_perm( sizeof( *ed ) );
                                ed->keyword             = fread_string( fp );
                                ed->description         = fread_string( fp );
                                ed->next                = pRoomIndex->extra_descr;
                                pRoomIndex->extra_descr = ed;
                                top_ed++;
                        }
                        else
                        {
                                bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
                                exit( 1 );
                        }
                }

                iHash                   = vnum % MAX_KEY_HASH;
                pRoomIndex->next        = room_index_hash[iHash];
                room_index_hash[iHash]  = pRoomIndex;
                top_room++;
        }

        return;
}


/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
        SHOP_DATA *pShop;
        int stat;

        for ( ; ; )
        {
                MOB_INDEX_DATA *pMobIndex;
                int iTrade;

                pShop                   = alloc_perm( sizeof( *pShop ) );
                pShop->keeper           = fread_number( fp, &stat );
                if ( pShop->keeper == 0 )
                        break;

                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                        pShop->buy_type[iTrade] = fread_number( fp, &stat );

                pShop->profit_buy       = fread_number( fp, &stat );
                pShop->profit_sell      = fread_number( fp, &stat );
                pShop->open_hour        = fread_number( fp, &stat );
                pShop->close_hour       = fread_number( fp, &stat );
                fread_to_eol( fp );
                pMobIndex               = get_mob_index( pShop->keeper );
                pMobIndex->pShop        = pShop;

                if ( !shop_first )
                        shop_first = pShop;
                if (  shop_last  )
                        shop_last->next = pShop;

                shop_last       = pShop;
                pShop->next     = NULL;
                top_shop++;
        }

        return;
}


/*
 * Snarf spec proc declarations.
 *
 * Modified to give xp bonuses April 2001 Shade / Gezhp
 */

void load_specials( FILE *fp )
{
        int bonus;
        int stat;

        for ( ; ; )
        {
                MOB_INDEX_DATA *pMobIndex;
                char letter;

                switch ( letter = fread_letter( fp ) )
                {
                    default:
                        bug( "Load_specials: letter '%c' not *MS.", letter );
                        exit( 1 );

                    case 'S':
                        return;

                    case '*':
                        break;

                    case 'M':
                        pMobIndex           = get_mob_index ( fread_number ( fp, &stat ) );
                        pMobIndex->spec_fun = spec_lookup   ( fread_word   ( fp ) );

                        if ( pMobIndex->spec_fun == 0 )
                        {
                                bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
                                exit( 1 );
                        }

                        /*
                         * average combat specials
                         */

                        bonus = 10;

                        /*
                         * non combat specials
                         */

                        if (pMobIndex->spec_fun == spec_lookup("spec_fido")
                            || pMobIndex->spec_fun == spec_lookup("spec_thief")
                            || pMobIndex->spec_fun == spec_lookup("spec_janitor")
                            || pMobIndex->spec_fun == spec_lookup("spec_repairman")
                            || pMobIndex->spec_fun == spec_lookup("spec_celestial_repairman")
                            || pMobIndex->spec_fun == spec_lookup("spec_cast_adept")
                            || pMobIndex->spec_fun == spec_lookup("spec_cast_hooker")
                            || pMobIndex->spec_fun == spec_lookup("spec_clan_guard")
                            || pMobIndex->spec_fun == spec_lookup("spec_bounty")
                            || pMobIndex->spec_fun == spec_lookup("spec_executioner")
                            || pMobIndex->spec_fun == spec_lookup("spec_cast_orb"))
                                bonus = 0;

                        /*
                         * weak combat specials
                         */

                        if (pMobIndex->spec_fun == spec_lookup("spec_poison")
                            || pMobIndex->spec_fun == spec_lookup("spec_bloodsucker")
                            || pMobIndex->spec_fun == spec_lookup("spec_spectral_minion")
                            || pMobIndex->spec_fun == spec_lookup("spec_kungfu_poison")
                            || pMobIndex->spec_fun == spec_lookup("spec_guard")
                            || pMobIndex->spec_fun == spec_lookup("spec_judge"))
                                bonus = 5;

                        /*
                         * strong combat specials
                         */

                        else if (pMobIndex->spec_fun == spec_lookup("spec_warrior")
                                 || pMobIndex->spec_fun == spec_lookup("spec_cast_druid")
                                 || pMobIndex->spec_fun == spec_lookup("spec_demon")
                                 || pMobIndex->spec_fun == spec_lookup("spec_cast_electric")
                                 || pMobIndex->spec_fun == spec_lookup("spec_assassin"))
                                bonus = 15;

                        /*
                         * bad ass combat specials
                         */

                        else if (pMobIndex->spec_fun == spec_lookup("spec_priestess")
                                 || pMobIndex->spec_fun == spec_lookup("spec_mast_vampire")
                                 || pMobIndex->spec_fun == spec_lookup("spec_evil_evil_gezhp")
                                 || pMobIndex->spec_fun == spec_lookup("spec_grail")
                                 || pMobIndex->spec_fun == spec_lookup("spec_cast_archmage"))
                                bonus = 20;

                        pMobIndex->spec_fun_exp_modifier = bonus;

                        break;
                }

                fread_to_eol( fp );
        }
}


void load_down_time( void )
{
        FILE *fp;

        down_time = str_dup ( "*" );
        warning1  = str_dup ( "*" );
        warning2  = str_dup ( "*" );

        if ( !( fp = fopen( DOWN_TIME_FILE, "r" ) ) )
                return;

        for ( ; ; )
        {
                char *word;
                char  letter;

                do
                {
                        letter = getc( fp );
                        if ( feof( fp ) )
                        {
                                fclose( fp );
                                return;
                        }
                }
                while ( isspace( letter ) );

                ungetc( letter, fp );

                word = fread_word( fp );

                if ( !str_cmp( word, "DOWNTIME" ) )
                {
                        free_string( down_time );
                        down_time = fread_string( fp );
                }

                if ( !str_cmp( word, "WARNINGA" ) )
                {
                        free_string( warning1 );
                        warning1 = fread_string( fp );
                }

                if ( !str_cmp( word, "WARNINGB" ) )
                {
                        free_string( warning2 );
                        warning2 = fread_string( fp );
                }
        }
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad or suspicious reverse exits.
 */
void fix_exits( void )
{
        EXIT_DATA       *pexit;
        ROOM_INDEX_DATA *pRoomIndex;
        int              iHash;
        int              door;

        for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        {
                for ( pRoomIndex  = room_index_hash[iHash];
                     pRoomIndex;
                     pRoomIndex  = pRoomIndex->next )
                {
                        bool fexit;

                        fexit = FALSE;
                        for ( door = 0; door <= 5; door++ )
                        {
                                if ( ( pexit = pRoomIndex->exit[door] ) )
                                {
                                        fexit = TRUE;
                                        if ( pexit->vnum <= 0 )
                                                pexit->to_room = NULL;
                                        else
                                                pexit->to_room = get_room_index( pexit->vnum );
                                }
                        }

                        if ( !fexit )
                                SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
                }
        }
        return;
}


/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
        AREA_DATA *pArea;

        sprintf (last_function, "entering area_update");

        for ( pArea = area_first; pArea; pArea = pArea->next )
        {
                CHAR_DATA *pch;

                if ( ++pArea->age < 8 )
                        continue;

                /*
                 * Check for PC's.
                 */
                if ( pArea->nplayer > 0 && pArea->age == 14 )
                {
                        for ( pch = char_list; pch; pch = pch->next )
                        {
                                if ( !IS_NPC( pch )
                                    && IS_AWAKE( pch )
                                    && pch->in_room
                                    && pch->in_room->area == pArea )
                                {
                                        send_to_char( "You hear the patter of little feet.\n\r", pch );
                                }
                        }
                }

                /*
                 * Check age and reset.
                 * Note: Mud School resets every 3 minutes (not 15).
                 */
                if ( !pArea->nplayer || pArea->age > 14 )
                {
                        ROOM_INDEX_DATA *pRoomIndex;

                        reset_area( pArea );
                        pArea->age = number_range( 0, 3 );
                        pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );

                        if ( pRoomIndex && pArea == pRoomIndex->area )
                                pArea->age = 12;
                }
        }

        sprintf (last_function, "leaving area_update");
        return;
}


/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
        CHAR_DATA  *mob;
        RESET_DATA *pReset;
        int         level;
        bool        last;

        OBJ_DATA        *obj;
        OBJ_INDEX_DATA  *pObjIndex;
        ROOM_INDEX_DATA *pRoomIndex;

        mob     = NULL;
        last    = TRUE;
        level   = 0;

        for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
        {
                OBJ_DATA        *obj_to;
                EXIT_DATA       *pexit;
                MOB_INDEX_DATA  *pMobIndex;
                OBJ_INDEX_DATA  *pObjToIndex;

                switch ( pReset->command )
                {
                    default:
                        bug( "Reset_area: bad command %c.", pReset->command );
                        break;

                    case 'M':
                        if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
                        {
                                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                                continue;
                        }

                        level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

                        if ( pMobIndex->count >= pReset->arg2 )
                        {
                                last = FALSE;
                                break;
                        }

                        if (IS_SET(pArea->area_flags, AREA_FLAG_NO_QUEST))
                                SET_BIT(pMobIndex->act, ACT_NO_QUEST);

                        mob = create_mobile( pMobIndex );
                        mob->exp_modifier = pArea->exp_modifier + pMobIndex->spec_fun_exp_modifier;

                        /*
                         * Check for pet shop.
                         */
                        {
                                ROOM_INDEX_DATA *pRoomIndexPrev;

                                pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
                                if ( pRoomIndexPrev
                                    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                                        SET_BIT( mob->act, ACT_PET);
                        }

                        if ( room_is_dark( pRoomIndex ) )
                                SET_BIT( mob->affected_by, AFF_INFRARED );

                        char_to_room( mob, pRoomIndex );
                        level = URANGE( 0, mob->level - 2, LEVEL_HERO );
                        last  = TRUE;
                        break;

                    case 'O':
                        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
                        {
                                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                                continue;
                        }

                        if ( pArea->nplayer > 0
                            || count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
                        {
                                last = FALSE;
                                break;
                        }

                        obj       = create_object( pObjIndex, number_fuzzy( level ) );
                        obj->cost = 0;
                        obj_to_room( obj, pRoomIndex );
                        last = TRUE;
                        break;

                    case 'I':
                        /*
                         * New item to room reset by Gezhp 2000
                         * I <item vnum> <item level> <room vnum> <max count in room>
                         */
                        if (!(pObjIndex = get_obj_index(pReset->arg0)))
                        {
                                bug("Reset_area: 'I': bad item vnum: %d", pReset->arg0);
                                continue;
                        }

                        if (pReset->arg1 < 0)
                        {
                                bug("Reset_area: 'I': bad item level: %d", pReset->arg1);
                                continue;
                        }

                        if (!(pRoomIndex = get_room_index(pReset->arg2)))
                        {
                                bug("Reset_area: 'R': bad room vnum: %d", pReset->arg2);
                                continue;
                        }

                        if (pReset->arg3 < 0)
                        {
                                bug("Reset_area: 'I': bad max count: %d", pReset->arg3);
                                continue;
                        }

                        if (pArea->nplayer > 0
                            || count_obj_list(pObjIndex, pRoomIndex->contents) >= pReset->arg3)
                        {
                                last = FALSE;
                                break;
                        }

                        obj = create_object(pObjIndex, number_fuzzy(pReset->arg1));
                        obj->cost = 0;
                        obj_to_room(obj, pRoomIndex);
                        last = TRUE;
                        break;

                    case 'P':
                        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
                        {
                                bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
                                continue;
                        }

                        if ( pObjToIndex == pObjIndex )
                        {
                                bug( "Reset_area: 'P': object loaded in itself %d.", pReset->arg3);
                                continue;
                        }

                        if ( pArea->nplayer > 0
                            || !( obj_to = get_obj_type( pObjToIndex ) )
                            || count_obj_list( pObjIndex, obj_to->contains ) > 0 )
                        {
                                last = FALSE;
                                break;
                        }

                        obj = create_object( pObjIndex, number_fuzzy( obj_to->level ) );
                        obj_to_obj( obj, obj_to );
                        last = TRUE;
                        break;

                    case 'G':
                    case 'E':
                        if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        if ( !last )
                                break;

                        if ( !mob )
                        {
                                bug( "Reset_area: 'E' or 'G': null mob for vnum %d.",
                                    pReset->arg1 );
                                last = FALSE;
                                break;
                        }

                        if ( mob->pIndexData->pShop )
                        {
                                int olevel;

                                /*
                                 * Additions to allow items levels to be set in area files
                                 * Tavolir
                                 */
                                if(pObjIndex->level == 0)
                                {
                                        switch ( pObjIndex->item_type )
                                        {
                                            default:
                                                olevel = 0;
                                                break;

                                            case ITEM_PILL:
                                                olevel = number_range(  0, 10 );
                                                break;

                                            case ITEM_PAINT:
                                                olevel = number_range(  0, 10 );
                                                break;

                                            case ITEM_POTION:
                                                olevel = number_range(  0, 10 );
                                                break;

                                            case ITEM_SCROLL:
                                                olevel = number_range(  5, 15 );
                                                break;

                                            case ITEM_WAND:
                                                olevel = number_range( 10, 20 );
                                                break;

                                            case ITEM_STAFF:
                                                olevel = number_range( 15, 25 );
                                                break;

                                            case ITEM_ARMOR:
                                                olevel = number_range( 5, 15 );
                                                break;

                                            case ITEM_WEAPON:
                                                if ( pReset->command == 'G' )
                                                        olevel = number_range( 5, 15 );
                                                else
                                                        olevel = number_fuzzy( level );
                                                break;
                                        }
                                }
                                else
                                {
                                        if( pObjIndex->level >= LEVEL_HERO)
                                                olevel = LEVEL_HERO - 1;
                                        else
                                                olevel = pObjIndex->level;
                                }
                                /* End of item level additions */

                                obj = create_object( pObjIndex, olevel );

                                if ( pReset->command == 'G' )
                                        SET_BIT( obj->extra_flags, ITEM_INVENTORY );
                        }
                        else
                        {
                                /* Make sure MUD School loot is level 1 */
                                if (IS_SET(pArea->area_flags, AREA_FLAG_SCHOOL) && level <= 5) {
                                        obj = create_object(pObjIndex, 1);
                                }
                                else {
                                        obj = create_object(pObjIndex, number_fuzzy(level));
                                }

                                if (obj->level > LEVEL_HERO) {
                                        obj->level = LEVEL_HERO;
                                }
                        }

                        obj_to_char( obj, mob );

                        if (pReset->command == 'E')
                        {
                                equip_char(mob, obj, pReset->arg3);

                                switch (pReset->arg3)
                                {
                                    case WEAR_WIELD:
                                        mob->exp_modifier += 10;
                                        break;

                                    case WEAR_DUAL:
                                    case WEAR_SHIELD:
                                        mob->exp_modifier += 5;
                                        break;

                                    case WEAR_HOLD:
                                        if (obj->item_type == ITEM_WAND
                                            || obj->item_type == ITEM_STAFF
                                            || obj->item_type == ITEM_POTION
                                            || obj->item_type == ITEM_PILL
                                            || obj->item_type == ITEM_SCROLL)
                                        {
                                                mob->exp_modifier += 5;
                                        }
                                        break;
                                }
                        }

                        last = TRUE;
                        break;

                    case 'D':
                        if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        if ( !( pexit = pRoomIndex->exit[pReset->arg2] ) )
                                break;

                        /* Bashed doors persist across resets */
                        if ( !IS_SET( pexit->exit_info, EX_BASHED ) )
                                switch ( pReset->arg3 )
                                {
                                    case 0:
                                        REMOVE_BIT( pexit->exit_info, EX_CLOSED );
                                        REMOVE_BIT( pexit->exit_info, EX_LOCKED );
                                        break;

                                    case 1:
                                        SET_BIT(    pexit->exit_info, EX_CLOSED );
                                        REMOVE_BIT( pexit->exit_info, EX_LOCKED );
                                        break;

                                    case 2:
                                        SET_BIT(    pexit->exit_info, EX_CLOSED );
                                        SET_BIT(    pexit->exit_info, EX_LOCKED );
                                        break;
                                }

                        last = TRUE;
                        break;

                    case 'R':
                        if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                        {
                                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
                                continue;
                        }

                        {
                                int d0;
                                int d1;

                                for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
                                {
                                        d1                   = number_range( d0, pReset->arg2-1 );
                                        pexit                = pRoomIndex->exit[d0];
                                        pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
                                        pRoomIndex->exit[d1] = pexit;
                                }
                        }
                        break;
                }
        }

        /*
         *  Reset container lids, traps with exhausted charges.
         *  This is a bit of a cpu hog so do it only once every 10 resets.
         *  Gezhp 2000
         */
        if (--pArea->container_reset_timer < 1)
        {
                for (obj = object_list; obj; obj = obj->next)
                {
                        if (!obj->deleted && obj->in_room && obj->in_room->area == pArea)
                        {
                                if (obj->item_type == ITEM_CONTAINER)
                                        obj->value[1] = obj->pIndexData->value[1];

                                if (IS_SET (obj->extra_flags, ITEM_TRAP) && !obj->trap_charge)
                                        obj->trap_charge = obj->pIndexData->trap_charge;
                        }
                }

                pArea->container_reset_timer = 10;
        }
}


/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
        CHAR_DATA *mob;

        if ( !pMobIndex )
        {
                bug( "Create_mobile: NULL pMobIndex.", 0 );
                exit( 1 );
        }

        if ( !char_free )
        {
                mob             = alloc_perm( sizeof( *mob ) );
        }
        else
        {
                mob             = char_free;
                char_free       = char_free->next;
        }

        clear_char( mob );
        mob->pIndexData         = pMobIndex;

        mob->name               = pMobIndex->player_name;
        mob->short_descr        = pMobIndex->short_descr;
        mob->long_descr         = pMobIndex->long_descr;
        mob->description        = pMobIndex->description;
        mob->spec_fun           = pMobIndex->spec_fun;
        mob->prompt             = str_dup( "<%hhp %mm %vmv> " );

        mob->level              = number_fuzzy( pMobIndex->level );
        mob->act                = pMobIndex->act;
        mob->affected_by        = pMobIndex->affected_by;
        mob->alignment          = pMobIndex->alignment;
        mob->sex                = pMobIndex->sex;
        mob->body_form          = pMobIndex->body_form;

        mob->armor              = interpolate( mob->level, 100, -100 );

        mob->max_hit = mob->level * 8
                + number_range( mob->level * mob->level / 4, mob->level * mob->level );

        mob->hit                = mob->max_hit;

        if (IS_SET(mob->act, ACT_CLAN_GUARD))
                REMOVE_BIT(mob->affected_by, AFF_HIDE);

        mob->next               = char_list;
        char_list               = mob;
        pMobIndex->count++;
        return mob;
}


/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object (OBJ_INDEX_DATA *pObjIndex, int level)
{
        static OBJ_DATA obj_zero;
        OBJ_DATA*       obj;
        int             i;
        const int       complete_heal_sn = skill_lookup("complete heal");

        if ( !pObjIndex )
        {
                bug( "Create_object: NULL pObjIndex.", 0 );
                exit( 1 );
        }

        if ( !obj_free )
        {
                obj             = alloc_perm( sizeof( *obj ) );
        }
        else
        {
                obj             = obj_free;
                obj_free        = obj_free->next;
        }

        *obj            = obj_zero;
        obj->pIndexData = pObjIndex;
        obj->in_room    = NULL;
        obj->level      = level;
        obj->wear_loc   = -1;

        obj->name               = pObjIndex->name;
        obj->short_descr        = pObjIndex->short_descr;
        obj->description        = pObjIndex->description;
        obj->item_type          = pObjIndex->item_type;
        obj->extra_flags        = pObjIndex->extra_flags;
        obj->wear_flags         = pObjIndex->wear_flags;
        obj->value[0]           = pObjIndex->value[0];
        obj->value[1]           = pObjIndex->value[1];
        obj->value[2]           = pObjIndex->value[2];
        obj->value[3]           = pObjIndex->value[3];
        obj->weight             = pObjIndex->weight;

        obj->cost = number_fuzzy(10) * number_fuzzy(level) + number_range(1, 20);

        obj->trap_eff               = pObjIndex->trap_eff;
        obj->trap_dam               = pObjIndex->trap_dam;
        obj->trap_charge            = pObjIndex->trap_charge;
        obj->ego_flags              = pObjIndex->ego_flags;
        obj->deleted                = FALSE;

        /*
         * Mess with object properties.
         */
        switch ( obj->item_type )
        {
            default:
                bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
                break;

            case ITEM_CONTAINER:
            case ITEM_LIGHT:
            case ITEM_FURNITURE:
            case ITEM_TRASH:
            case ITEM_DRINK_CON:
            case ITEM_KEY:
            case ITEM_FOOD:
            case ITEM_BOAT:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
            case ITEM_FOUNTAIN:
            case ITEM_CLIMBING_EQ:
            case ITEM_MOB:
            case ITEM_ANVIL:
            case ITEM_CLAN_OBJECT:
            case ITEM_PORTAL:
            case ITEM_POISON_POWDER:
            case ITEM_INSTRUMENT:
            case ITEM_LOCK_PICK:
            case ITEM_WHETSTONE:
            case ITEM_MITHRIL:
            case ITEM_CRAFT:
            case ITEM_SPELLCRAFT:
                break;

            case ITEM_TREASURE:
                obj->cost *= (number_range(3, 6) / 2);
                break;

            case ITEM_SCROLL:
                obj->value[0]   = number_fuzzy( obj->value[0] );
                break;

            case ITEM_WAND:
                obj->value[0]   = number_fuzzy( obj->value[0] );
                obj->value[1]   = number_fuzzy( obj->value[1] );
                obj->value[2]   = obj->value[1];
                break;

            case ITEM_STAFF:

                /*
                 * Complete heal staff charge hack; Gezhp 2001
                 *
                 * 1 charge    ->  1 charge
                 * 2 charges   ->  1-2 charges
                 * 3+ charges  ->  number_fuzzy - 1 (3 -> 1-3, 4 -> 2-4, ...)
                 */
                if (complete_heal_sn > -1 && complete_heal_sn == obj->value[3])
                {
                        if (obj->value[1] == 2)
                                obj->value[1] = number_range(1, 2);

                        else if (obj->value[1] > 2)
                                obj->value[1] = number_fuzzy(obj->value[1]) - 1;
                }
                else
                        obj->value[1]   = number_fuzzy(obj->value[1]);

                obj->value[0]   = number_fuzzy( obj->value[0] );
                obj->value[2]   = obj->value[1];
                break;

            case ITEM_WEAPON:
                obj->value[1]   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
                obj->value[2]   = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
                break;

            /*
             * Although we can't wield armourer's hammers, area resets may
             * equip them as weapons; let's prepare for wierdness; Gezhp 2001
             */
            case ITEM_ARMOURERS_HAMMER:
                obj->value[1]   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
                obj->value[2]   = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
                obj->value[3]   = 7;
                break;

            case ITEM_ARMOR:
                obj->value[0]   = number_fuzzy( level / 5 + 2 );
                break;

            case ITEM_POTION:
            case ITEM_PILL:
            case ITEM_PAINT:
                obj->value[0]   = number_fuzzy( number_fuzzy( obj->value[0] ) );
                break;

            case ITEM_MONEY:
                /* Make coin amounts fuzzy; Gezhp */
                for (i = 0; i < 4; i++)
                {
                        if (obj->value[i] > 10)
                        {
                                obj->value[i] = number_range (obj->value[i] * 0.9,
                                                              obj->value[i] * 1.1);
                        }

                        if (obj->value[i] < 0)
                                obj->value[i] = 0;
                }
                break;

            case ITEM_AUCTION_TICKET:
                obj->value[0]   = obj->cost;
                break;
        }

        obj->next               = object_list;
        object_list             = obj;
        pObjIndex->count++;

        return obj;
}


/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
        static CHAR_DATA ch_zero;

        *ch                     = ch_zero;
        ch->name                = &str_empty[0];
        ch->short_descr         = &str_empty[0];
        ch->long_descr          = &str_empty[0];
        ch->description         = &str_empty[0];
        ch->prompt              = str_dup("> ");
        ch->last_note           = 0;
        ch->logon               = current_time;
        ch->armor               = 100;
        ch->position            = POS_STANDING;
        ch->level               = 0;
        ch->status              = 0;
        ch->hit                 = 20;
        ch->max_hit             = 20;
        ch->mana                = 100;
        ch->max_mana            = 100;
        ch->move                = 100;
        ch->max_move            = 100;
        ch->rage                = 50;
        ch->max_rage            = 100;
        ch->leader              = NULL;
        ch->master              = NULL;
        ch->mount               = NULL;
        /* ch->mapbook          = NULL; */
        ch->deleted             = FALSE;
        ch->exp_modifier        = 100;

        ch->colors[COLOR_AUCTION]      = 13;
        ch->colors[COLOR_GOSSIP]       = 11;
        ch->colors[COLOR_SHOUT]        = 9;
        ch->colors[COLOR_IMMTALK]      = 12;
        ch->colors[COLOR_SAY]          = 15;
        ch->colors[COLOR_TELL]         = 10;
        ch->colors[COLOR_CLAN]         = 7;
        ch->colors[COLOR_DIRTALK]      = 25;
        ch->colors[COLOR_SERVER]       = 6;
        ch->colors[COLOR_ARENA]        = 13;
        ch->colors[COLOR_NEWBIE]       = 14; /* --Owl 2/3/22, didn't want it same colour as 'say' */
   
}


/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
        OBJ_DATA    *obj;
        OBJ_DATA    *obj_next;
        AFFECT_DATA *paf;

        for ( obj = ch->carrying; obj; obj = obj_next )
        {
                obj_next = obj->next_content;
                if ( obj->deleted )
                        continue;
                extract_obj( obj );
        }

        for ( paf = ch->affected; paf; paf = paf->next )
        {
                if ( paf->deleted )
                        continue;
                affect_remove( ch, paf );
        }

        free_string( ch->name );
        free_string( ch->short_descr );
        free_string( ch->long_descr );
        free_string( ch->description );
        free_string( ch->prompt );

        if ( ch->pcdata )
        {
                free_string( ch->pcdata->pwd            );
                free_string( ch->pcdata->bamfin         );
                free_string( ch->pcdata->bamfout        );
                free_string( ch->pcdata->title          );
                ch->pcdata->next = pcdata_free;
                pcdata_free      = ch->pcdata;
        }

        ch->next             = char_free;
        char_free            = ch;

        return;
}


/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
        for ( ; ed; ed = ed->next )
        {
                if ( is_name( name, ed->keyword ) )
                        return ed->description;
        }
        return NULL;
}


/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
        MOB_INDEX_DATA *pMobIndex;

        for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
             pMobIndex;
             pMobIndex  = pMobIndex->next )
        {
                if ( pMobIndex->vnum == vnum )
                        return pMobIndex;
        }

        if ( fBootDb )
        {
                bug( "Get_mob_index: bad vnum %d.", vnum );
                exit( 1 );
        }

        return NULL;
}


/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
        OBJ_INDEX_DATA *pObjIndex;

        for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
             pObjIndex;
             pObjIndex  = pObjIndex->next )
        {
                if ( pObjIndex->vnum == vnum )
                        return pObjIndex;
        }

        if ( fBootDb )
        {
                bug( "Get_obj_index: bad vnum %d.", vnum );
                exit( 1 );
        }

        return NULL;
}


OBJSET_INDEX_DATA *get_objset_index( int vnum )
{
        OBJSET_INDEX_DATA *pObjSetIndex;

        for ( pObjSetIndex  = objset_index_hash[vnum % MAX_KEY_HASH];
             pObjSetIndex;
             pObjSetIndex  = pObjSetIndex->next )
        {
                if ( pObjSetIndex->vnum == vnum )
                        return pObjSetIndex;
        }

        if ( fBootDb )
        {
                bug( "Get_objset_index: bad vnum %d.", vnum );
                exit( 1 );
        }

        return NULL;
}

/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
        ROOM_INDEX_DATA *pRoomIndex;

        if (vnum < 0)
                return NULL;

        for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
             pRoomIndex;
             pRoomIndex  = pRoomIndex->next )
        {
                if ( pRoomIndex->vnum == vnum )
                        return pRoomIndex;
        }

        if ( fBootDb )
        {
                bug( "Get_room_index: bad vnum %d.", vnum );
                exit( 1 );
        }

        return NULL;
}


/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
        char c;

        do
        {
                c = getc( fp );
        }
        while ( isspace( c ) );

        return c;
}


/*
 * Read a number from a file.
 */
int fread_number( FILE *fp, int *status )
{
    int  c;
    bool sign;
    int  number;
    int  stat;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit( c ) )
    {
        *status = 1;
	bug( "Fread_number: bad format.", 0 );
	bug( "   If bad object, check for missing '~' in value[] fields.", 0 );
	return 0;
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp, &stat );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}


/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
        char *plast;
        char  c;

        plast = top_string + sizeof( char * );
        if ( plast > &string_space [ MAX_STRING - MAX_STRING_LENGTH ] )
        {
                bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
                exit( 1 );
        }

        /*
         * Skip blanks.
         * Read first char.
         */
        do
        {
                c = getc( fp );
        }
        while ( isspace( c ) );

        if ( ( *plast++ = c ) == '~' )
                return &str_empty[0];

        for ( ;; )
        {
                /*
                 * Back off the char type lookup,
                 *   it was too dirty for portability.
                 *   -- Furey
                 */
                switch ( *plast = getc( fp ) )
                {
                    default:
                        plast++;
                        break;

                    case EOF:
                        bug( "Fread_string: EOF", 0 );
                        exit( 1 );
                        break;

                    case '\n':
                        plast++;
                        *plast++ = '\r';
                        break;

                    case '\r':
                        break;

                    case '~':
                        plast++;
                        {
                                union
                                {
                                        char *  pc;
                                        char    rgc[sizeof( char * )];
                                } u1;
                                int ic;
                                int iHash;
                                char *pHash;
                                char *pHashPrev;
                                char *pString;

                                plast[-1] = '\0';
                                iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
                                for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
                                {
                                        for ( ic = 0; ic < sizeof( char * ); ic++ )
                                                u1.rgc[ic] = pHash[ic];
                                        pHashPrev = u1.pc;
                                        pHash    += sizeof(char *);

                                        if ( top_string[sizeof( char * )] == pHash[0]
                                            && !strcmp( top_string+sizeof( char * )+1, pHash+1 ) )
                                                return pHash;
                                }

                                if ( fBootDb )
                                {
                                        pString         = top_string;
                                        top_string      = plast;
                                        u1.pc           = string_hash[iHash];
                                        for ( ic = 0; ic < sizeof( char * ); ic++ )
                                                pString[ic] = u1.rgc[ic];
                                        string_hash[iHash]  = pString;

                                        nAllocString += 1;
                                        sAllocString += top_string - pString;
                                        return pString + sizeof( char * );
                                }
                                else
                                {
                                        return str_dup( top_string + sizeof( char * ) );
                                }
                        }
                }
        }
}


/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
        char c;

        do
        {
                c = getc( fp );
        }
        while ( c != '\n' && c != '\r' );

        do
        {
                c = getc( fp );
        }
        while ( c == '\n' || c == '\r' );

        ungetc( c, fp );
        return;
}


/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
        static char  word [ MAX_INPUT_LENGTH ];
        char *pword;
        char  cEnd;

        do
        {
                cEnd = getc( fp );
        }
        while ( isspace( cEnd ) );

        if ( cEnd == '\'' || cEnd == '"' )
        {
                pword   = word;
        }
        else
        {
                word[0] = cEnd;
                pword   = word+1;
                cEnd    = ' ';
        }

        for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
        {
                *pword = getc( fp );
                if ( cEnd == ' ' ? isspace( *pword ) : *pword == cEnd )
                {
                        if ( cEnd == ' ' )
                                ungetc( *pword, fp );
                        *pword = '\0';
                        return word;
                }
        }

        bug( "Fread_word: word too long.", 0 );
        bug(word, 0);
        exit( 1 );
        return NULL;
}


/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
        void *pMem;
        int iList;

        for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
        {
                if ( sMem <= rgSizeList[iList] )
                        break;
        }

        if ( iList == MAX_MEM_LIST )
        {
                bug( "Alloc_mem: size %d too large.", sMem );
                abort();
        }

        if ( !rgFreeList[iList] )
        {
                pMem = alloc_perm( rgSizeList[iList] );
        }
        else
        {
                pMem              = rgFreeList[iList];
                rgFreeList[iList] = * ( (void **) rgFreeList[iList] );
        }

        return pMem;
}


/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
        int iList;

        for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
        {
                if ( sMem <= rgSizeList[iList] )
                        break;
        }

        if ( iList == MAX_MEM_LIST )
        {
                bug( "Free_mem: size %d too large.", sMem );
                exit( 1 );
        }

        * ( (void **) pMem ) = rgFreeList[iList];
        rgFreeList[iList]  = pMem;

        return;
}


/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
        void *pMem;
        static char *pMemPerm;
        static int   iMemPerm;

        while ( sMem % sizeof( long ) != 0 )
                sMem++;
        if ( sMem > MAX_PERM_BLOCK )
        {
                bug( "Alloc_perm: %d too large.", sMem );
                exit( 1 );
        }

        if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
        {
                iMemPerm = 0;
                if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
                {
                        perror( "Alloc_perm" );
                        exit( 1 );
                }
        }

        pMem        = pMemPerm + iMemPerm;
        iMemPerm   += sMem;
        nAllocPerm += 1;
        sAllocPerm += sMem;
        return pMem;
}


/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
        char *str_new;

        if ( str[0] == '\0' )
                return &str_empty[0];

        if ( str >= string_space && str < top_string )
                return (char *) str;

        str_new = alloc_mem( strlen(str) + 1 );
        strcpy( str_new, str );
        return str_new;
}


/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
        if (  !pstr
            || pstr == &str_empty[0]
            || ( pstr >= string_space && pstr < top_string ) )
                return;

        free_mem( pstr, strlen( pstr ) + 1 );
        return;
}


void do_areas (CHAR_DATA *ch, char *argument)
{
        AREA_DATA *pArea;
        char buf  [MAX_STRING_LENGTH * 2];
        char buf1 [MAX_STRING_LENGTH];
        char buf_all [MAX_STRING_LENGTH];
        int level = 0;

        if ( argument[0] != '\0' && is_number(argument))
                level = atoi(argument);

        pArea = area_first;

        if (level)
        {
                sprintf(buf, "----------------------------------------------------\n\r   {CAreas Around Level %d{x\n\r----------------------------------------------------\n\r", level);
        }
        else
        {
                sprintf(buf, "----------------------------------------------------\n\r   {CComplete Area List{x\n\r----------------------------------------------------\n\r");
        }

        sprintf(buf_all, "\n\r");

        for (pArea = area_first; pArea; pArea = pArea->next)
        {
                if (!level
                    || (pArea->low_level <= level
                        && (pArea->high_level >= level || pArea->high_level == -1 || pArea->low_level == -2)))
                {
                        /*
                         * added last || evaluation to if_check above so you see 'all' areas if you supply
                         * an area number argument to 'area' --Owl 13/2/22
                        */

                        /*fprintf(stderr, "var: %d\r\n", pArea->low_level);*/

                        if (IS_SET(pArea->area_flags, AREA_FLAG_HIDDEN))
                                continue;

                        if (pArea->low_level == ROOM_LABEL_CLAN)
                                sprintf(buf1, "{W({x  clan {W){x{x   ");

                        else if (pArea->low_level == ROOM_LABEL_NONE)
                                sprintf(buf1, "{W({x  none {W){x   ");

                        else if (pArea->low_level == ROOM_LABEL_ALL)
                                sprintf(buf1, "{W({x  all  {W){x   ");

                        else if (pArea->low_level == ROOM_LABEL_NOTELEPORT )
                                sprintf(buf1, "{W({xnot finished{W){x ");

                        else if (pArea->high_level == ROOM_LABEL_PLUS)
                                sprintf(buf1, "{W({x%3d  + {W){x   ", pArea->low_level);

                        else
                                sprintf(buf1, "{W({x%3d %3d{W){x   ", pArea->low_level, pArea->high_level);

                        if (!(pArea->low_level == -4 && level))
                        {
                                if (pArea->low_level == ROOM_LABEL_ALL || pArea->low_level == ROOM_LABEL_CLAN)
                                {
                                        strcat(buf_all, buf1);
                                        sprintf(buf1, "{W%-30s{x {G%-20s{x\n\r", pArea->name, pArea->author);
                                        strcat(buf_all, buf1);
                                }
                                else
                                {
                                        strcat(buf, buf1);
                                        sprintf(buf1, "{W%-30s{x {G%-20s{x\n\r", pArea->name, pArea->author);
                                        strcat(buf, buf1);
                                }
                        }
                }
        }

        strcat(buf, buf_all);
        send_to_char(buf, ch);


        return;
}


void do_memory( CHAR_DATA *ch, char *argument )
{
        CHAR_DATA*      rch;
        char            buf [ MAX_STRING_LENGTH ];
        int             mob_count = 0, obj_count = 0;
        static int      mob_max = 0, obj_max = 0;
        OBJ_DATA*       obj;
        CHAR_DATA*      mob;
        int             desc_count;
        DESCRIPTOR_DATA *d;


        rch = get_char(ch);

        if (!authorized(rch, gsn_memory))
                return;

        sprintf(buf,
                "Affects {W%5d{x      Areas   {W%5d{x      ExDes   {W%5d{x\n\r"
                "Exits   {W%5d{x      Helps   {W%5d{x      Mobs    {W%5d{x\n\r"
                "Objs    {W%5d{x      Resets  {W%5d{x      Rooms   {W%5d{x\n\r"
                "Shops   {W%5d{x      ObjSets {W%5d{x\n\r",
                top_affect, top_area, top_ed,
                top_exit, top_help, top_mob_index,
                top_obj_index, top_reset, top_room,
                top_shop,top_objset_index);
        send_to_char(buf, ch);

        sprintf( buf, "\n\rStrings {C%7d{x strings of {c%8d{x bytes (max {W%d{x)\n\r",
                nAllocString, sAllocString, MAX_STRING );
        send_to_char( buf, ch );

        sprintf( buf, "Perms   {C%7d{x  blocks of {c%8d{x bytes\n\r",
                nAllocPerm, sAllocPerm );
        send_to_char( buf, ch );

        for (mob = char_list; mob; mob = mob->next)
                mob_count++;

        if (mob_count > mob_max)
                mob_max = mob_count;

        for (obj = object_list; obj; obj = obj->next)
                obj_count++;

        if (obj_count > obj_max)
                obj_max = obj_count;

        sprintf (buf, "\n\rMobile list: {W%5d{x  (highest {C%5d{x)\n\r"
                 "Object list: {W%5d{x  (highest {C%5d{x)\n\r",
                 mob_count, mob_max,
                 obj_count, obj_max);
        send_to_char (buf, ch);

        send_to_char("\n\rDescriptor information:\n\r", ch);

        for (d = descriptor_list, desc_count = 1; d; d = d->next, desc_count++)
        {
                sprintf(buf, "{G%2d{x  char={W%-12s{x  desc={W%-3d{x  connected={W%-2d{x  host={C%-s{x\n\r",
                        desc_count,
                        d->original ? d->original->name :
                        d->character ? d->character->name : "{c(none){x",
                        d->descriptor,
                        d->connected,
                        d->host ? d->host : "{c(none){x");
                send_to_char(buf, ch);
        }
}


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
        switch ( number_bits( 2 ) )
        {
            case 0:  number -= 1; break;
            case 3:  number += 1; break;
        }

        return UMAX( 1, number );
}


/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
        int power;
        int number;

        if ( ( to = to - from + 1 ) <= 1 )
                return from;

        for ( power = 2; power < to; power <<= 1 )
                ;

        while ( ( number = number_mm( ) & ( power - 1 ) ) >= to )
                ;

        return from + number;
}


/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
        int percent;

        while ( ( percent = number_mm( ) & ( 128-1 ) ) > 99 )
                ;

        return 1 + percent;
}


/*
 * Generate a random door.
 */
int number_door( void )
{
        int door;

        while ( ( door = number_mm( ) & ( 8-1 ) ) > 5 )
                ;

        return door;
}


int number_bits( int width )
{
        return number_mm( ) & ( ( 1 << width ) - 1 );
}


/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static int rgiState[2+55];

void init_mm( )
{
        int *piState;
        int  iState;

        piState = &rgiState[2];

        piState[-2]     = 55 - 55;
        piState[-1]     = 55 - 24;

        piState[0]      = ( (int) current_time ) & ( ( 1 << 30 ) - 1 );
        piState[1]      = 1;
        for ( iState = 2; iState < 55; iState++ )
        {
                piState[iState] = ( piState[iState-1] + piState[iState-2] )
                        & ( ( 1 << 30 ) - 1 );
        }
        return;
}


int number_mm( void )
{
        int *piState;
        int  iState1;
        int  iState2;
        int  iRand;

        piState         = &rgiState[2];
        iState1         = piState[-2];
        iState2         = piState[-1];
        iRand           = ( piState[iState1] + piState[iState2] ) & ( ( 1 << 30 ) - 1 );
        piState[iState1]        = iRand;
        if ( ++iState1 == 55 )
                iState1 = 0;
        if ( ++iState2 == 55 )
                iState2 = 0;
        piState[-2]             = iState1;
        piState[-1]             = iState2;
        return iRand >> 6;
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
        int idice;
        int sum;

        switch ( size )
        {
            case 0: return 0;
            case 1: return number;
        }

        for ( idice = 0, sum = 0; idice < number; idice++ )
                sum += number_range( 1, size );

        return sum;
}

/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_47 )
{
        return value_00 + level * ( value_47 - value_00 ) / 47;
}


/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
        for ( ; *str != '\0'; str++ )
        {
                if ( *str == '~' )
                        *str = '-';
        }

        return;
}


/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
        if ( !astr )
        {
                bug( "Str_cmp: null astr.", 0 );
                return TRUE;
        }

        if ( !bstr )
        {
                bug( "Str_cmp: null bstr.", 0 );
                return TRUE;
        }

        for ( ; *astr || *bstr; astr++, bstr++ )
        {
                if ( LOWER( *astr ) != LOWER( *bstr ) )
                        return TRUE;
        }

        return FALSE;
}


/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
        if ( !astr )
        {
                bug( "Str_prefix: null astr.", 0 );
                return TRUE;
        }

        if ( !bstr )
        {
                bug( "Str_prefix: null bstr.", 0 );
                return TRUE;
        }

        for ( ; *astr; astr++, bstr++ )
        {
                if ( LOWER( *astr ) != LOWER( *bstr ) )
                        return TRUE;
        }

        return FALSE;
}


/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
        char c0;
        int  sstr1;
        int  sstr2;
        int  ichar;

        if ( ( c0 = LOWER( astr[0] ) ) == '\0' )
                return FALSE;

        sstr1 = strlen( astr );
        sstr2 = strlen( bstr );

        for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
        {
                if ( c0 == LOWER( bstr[ichar] ) && !str_prefix( astr, bstr + ichar ) )
                        return FALSE;
        }

        return TRUE;
}


/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
        int sstr1;
        int sstr2;

        sstr1 = strlen( astr );
        sstr2 = strlen( bstr );

        if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
                return FALSE;
        else
                return TRUE;
}


/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
        static char strcap [ MAX_STRING_LENGTH ];
        int  i;

        for ( i = 0; str[i] != '\0'; i++ )
                strcap[i] = LOWER( str[i] );
        strcap[i] = '\0';
        strcap[0] = UPPER( strcap[0] );
        return strcap;
}


/*
 * Capitalizes initial of string only
 */
char *capitalize_initial( const char *str )
{
        static char strcap [ MAX_STRING_LENGTH ];
        int  i;

        for ( i = 0; str[i] != '\0'; i++ )
                strcap[i] = str[i];
        strcap[i] = '\0';
        strcap[0] = UPPER( strcap[0] );
        return strcap;
}

/*
 * Initial letter of a string, lower-cased.
 */
char *initial (const char *str)
{
        static char initial [1];
        initial[0] = LOWER(str[0]);
        return initial;
}


/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
        FILE *fp;

        if ( IS_NPC( ch ) || str[0] == '\0' )
                return;

        fclose( fpReserve );
        if ( !( fp = fopen( file, "a" ) ) )
        {
                perror( file );
                send_to_char( "Could not open the file!\n\r", ch );
        }
        else
        {
                fprintf( fp, "[%5d] %s: %s\n",
                        ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
                fclose( fp );
        }

        fpReserve = fopen( NULL_FILE, "r" );
        return;
}


/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
        FILE *fp;
        char  buf [ MAX_STRING_LENGTH ];

        if ( fpArea )
        {
                int iLine;
                int iChar;

                if ( fpArea == stdin )
                {
                        iLine = 0;
                }
                else
                {
                        iChar = ftell( fpArea );
                        fseek( fpArea, 0, 0 );
                        for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
                        {
                                while ( getc( fpArea ) != '\n' )
                                        ;
                        }
                        fseek( fpArea, iChar, 0 );
                }

                sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
                log_string( buf );
                if ( ( fp = fopen( "../src/shutdown.txt", "a" ) ))
                {
                        fprintf( fp, "[*****] %s\n", buf );
                        fclose( fp );
                }
        }

        strcpy( buf, "[*****] BUG: " );
        sprintf( buf + strlen( buf ), str, param );
        log_string( buf );

        /* We used to close the fpReserve file handle here then reopen it after writing to the bug file,
           but this can cause the wrong error to be logged (and a hard crash to occur) under some conditions.
           We probably don't need to ever reserve a file handle on a modern server.
           It is better to just assume we will not hit open file limits rather than risk exploding... */
        if ( ( fp = fopen( BUG_FILE, "a" ) ) )
        {
                fprintf( fp, "%s\n", buf );
                fclose( fp );
        }
}


/*
 * Writes a string to the log.
 */
void log_string (const char *str)
{
        char *strtime;

        strtime = ctime(&current_time);
        strtime[strlen(strtime)-6] = '\0';
        fprintf(stderr, "%s :: %s\n", &strtime[4], str);
}


/*
 * Display vnums currently assigned to areas            -Altrag & Thoric
 * Sorted, and flagged if loaded.
 */
void show_vnums(CHAR_DATA *ch, int low, int high,  bool shownl,
                char *loadst, char *notloadst)
{
        char buf[MAX_STRING_LENGTH];
        AREA_DATA *pArea, *first_sort;
        int count;

        first_sort = area_first;
        count = 0;

        for ( pArea = first_sort; pArea; pArea = pArea->next )
        {
                if ( pArea->low_r_vnum < low )
                        continue;

                if ( pArea->hi_r_vnum > high )
                        continue;

                else if ( !shownl )
                        continue;

                sprintf(buf, "%-24s| Rooms: %5d-%-5d"
                        " Objs: %5d-%-5d Mobs: %5d-%-5d%s\n\r",
                        (pArea->name ? pArea->name : "(invalid)"),
                        pArea->low_r_vnum, pArea->hi_r_vnum,
                        pArea->low_o_vnum, pArea->hi_o_vnum,
                        pArea->low_m_vnum, pArea->hi_m_vnum,
                        loadst  );

                send_to_char( buf, ch );

                /*
                 * Used by Gezhp to extract area vnum info to file
                 * printf ("\t'%s',\t\t%d,\t%d,\n",
                 * pArea->name ? pArea->name : "(invalid)",
                 * pArea->low_m_vnum,
                 * pArea->hi_m_vnum);
                 */

                count++;
        }

        sprintf( buf, "Areas listed: %d\n\r", count );
        send_to_char( buf, ch );
        return;
}


/*
 * Shows installed areas, sorted.  Mark unloaded areas with an X
 */
void do_zones( CHAR_DATA *ch, char *argument )
{
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        int low = 0;
        int high = 60000;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if ( arg1[0] != '\0' )
        {
                low = atoi(arg1);

                if ( arg2[0] != '\0' )
                        high = atoi(arg2);
        }

        show_vnums( ch, low, high, TRUE, "", " X" );
}

GAME_DATA *new_game( void )
{
    GAME_DATA *pGame;

    if ( !game_free )
    {
	pGame = alloc_perm( sizeof( *pGame ) );
	top_game++;
    }
    else
    {
	pGame		= game_free;
	game_free	= game_free->next;
    }

    pGame->next		= NULL;
    pGame->croupier	= 0;
    pGame->game_fun	= NULL;
    pGame->bankroll	= 5000;
    pGame->max_wait	= 100;
    pGame->cheat	= 0;

    return pGame;
}

void free_game( GAME_DATA * pGame )
{
    pGame->next	= game_free;
    game_free	= pGame;
    return;
}

/*
 * Snarf games proc declarations.
 */
void load_games( FILE *fp )
{
    int        croupier = 0;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	GAME_DATA      *pGame;
        char            letter;
	int             stat;
        char            buf[MAX_STRING_LENGTH];

	pGame = alloc_perm( sizeof( GAME_DATA ) );

        switch ( letter = fread_letter( fp ) )
        {
        default:
            bug( "Load_games: letter '%c' not *MS.", letter );
            exit( 1 );

        case 'S':
            return;

        case '*':
            break;

        case 'M':
	    croupier			= fread_number( fp, &stat );
	    if ( croupier == 0 )
	    {
		free_mem( pGame, sizeof( GAME_DATA ) );
		return;
	    }
	    pMobIndex			= get_mob_index( croupier );
	    pMobIndex->pGame		= pGame;
	    pGame->croupier		= croupier;
	    pGame->game_fun		= game_lookup( fread_word ( fp ) );
	    pGame->bankroll		= fread_number( fp, &stat );
	    pGame->max_wait		= fread_number( fp, &stat );
	    pGame->cheat		= fread_number( fp, &stat );

            sprintf(buf, "[croupier] %s (vnum %d).",
                        pMobIndex->short_descr,
                        pMobIndex->vnum
            );
            log_string(buf);

	    if ( pGame->game_fun == 0 )
            {
                bug( "Load_games: 'M': vnum %d.", pMobIndex->vnum );
                exit( 1 );
            }
					  fread_to_eol( fp );
            break;
        }

	if ( !game_first )
	    game_first = pGame;
	if (  game_last  )
	    game_last->next = pGame;

	game_last	= pGame;
	pGame->next	= NULL;
	top_game++;
    }

    return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */

void tail_chain( void )
{
        return;
}


/* EFO db.c */
