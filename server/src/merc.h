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
#pragma once
#include <math.h>
#include <unistd.h>
#define _XOPEN_SOURCE

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
#define const
#define args ( list )                   ( )
#define DECLARE_DO_FUN( fun )           void fun( )
#define DECLARE_SPEC_FUN( fun )         bool fun( )
#define DECLARE_SPELL_FUN( fun )        void fun( )
#define DECLARE_CONSTRUCT_FUN( fun )    void fun( )
#else
#define args( list )                    list
#define DECLARE_DO_FUN( fun )           DO_FUN          fun
#define DECLARE_SPEC_FUN( fun )         SPEC_FUN        fun
#define DECLARE_SPELL_FUN( fun )        SPELL_FUN       fun
#define DECLARE_CONSTRUCT_FUN( fun )    CONSTRUCT_FUN      fun
#endif

/* System calls - for delete ( from ROM ) */
int     unlink();

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if !defined( FALSE )
#define FALSE    0
#endif

#if !defined( TRUE )
#define TRUE     1
#endif

#if defined( _AIX )
#if !defined( const )
#define const
#endif
typedef int bool;
#define unix
#else
typedef unsigned char bool;
#endif

typedef short int sh_int;


/*
 *  Bit values.  View online with 'HELP BITZ'.  Gezhp
 */
#define BIT_0   0x00000001      /*                          1 */
#define BIT_1   0x00000002      /*                          2 */
#define BIT_2   0x00000004      /*                          4 */
#define BIT_3   0x00000008      /*                          8 */
#define BIT_4   0x00000010      /*                         16 */
#define BIT_5   0x00000020      /*                         32 */
#define BIT_6   0x00000040      /*                         64 */
#define BIT_7   0x00000080      /*                        128 */
#define BIT_8   0x00000100      /*                        256 */
#define BIT_9   0x00000200      /*                        512 */
#define BIT_10  0x00000400      /*                       1024 */
#define BIT_11  0x00000800      /*                       2048 */
#define BIT_12  0x00001000      /*                       4096 */
#define BIT_13  0x00002000      /*                       8192 */
#define BIT_14  0x00004000      /*                      16384 */
#define BIT_15  0x00008000      /*                      32768 */
#define BIT_16  0x00010000      /*                      65536 */
#define BIT_17  0x00020000      /*                     131072 */
#define BIT_18  0x00040000      /*                     262144 */
#define BIT_19  0x00080000      /*                     524288 */
#define BIT_20  0x00100000      /*                    1048576 */
#define BIT_21  0x00200000      /*                    2097152 */
#define BIT_22  0x00400000      /*                    4194304 */
#define BIT_23  0x00800000      /*                    8388608 */
#define BIT_24  0x01000000      /*                   16777216 */
#define BIT_25  0x02000000      /*                   33554432 */
#define BIT_26  0x04000000      /*                   67108864 */
#define BIT_27  0x08000000      /*                  134217728 */
#define BIT_28  0x10000000      /*                  268435456 */
#define BIT_29  0x20000000      /*                  536870912 */
#define BIT_30  0x40000000      /*                 1073741824 */


/*
 * Structure types.
 */
typedef struct affect_data                      AFFECT_DATA;
typedef struct area_data                        AREA_DATA;
typedef struct ban_data                         BAN_DATA;
typedef struct char_data                        CHAR_DATA;
typedef struct descriptor_data                  DESCRIPTOR_DATA;
typedef struct exit_data                        EXIT_DATA;
typedef struct extra_descr_data                 EXTRA_DESCR_DATA;
typedef struct help_data                        HELP_DATA;
typedef struct kill_data                        KILL_DATA;
typedef struct learned_data                     LEARNED_DATA;
typedef struct mob_index_data                   MOB_INDEX_DATA;
typedef struct note_data                        NOTE_DATA;
typedef struct obj_data                         OBJ_DATA;
typedef struct obj_index_data                   OBJ_INDEX_DATA;
typedef struct pc_data                          PC_DATA;
typedef struct reset_data                       RESET_DATA;
typedef struct room_index_data                  ROOM_INDEX_DATA;
typedef struct shop_data                        SHOP_DATA;
typedef struct time_info_data                   TIME_INFO_DATA;
typedef struct weather_data                     WEATHER_DATA;
typedef struct mob_prog_data                    MPROG_DATA;
typedef struct mob_prog_act_list                MPROG_ACT_LIST;
typedef struct auction_data                     AUCTION_DATA;
typedef struct coin_data                        COIN_DATA;
typedef struct smelting_data                    SMELTING_DATA;
/* typedef struct raw_mats_data                    RAW_MATERIAL_DATA; */

/*
 * Tables - geoff
 */
typedef struct fame_data                        FAME_TABLE;
typedef struct infamy_data                      INFAMY_TABLE;
typedef struct clan_pkill_data                  CLAN_PKILL_TABLE;
typedef struct clan_vanquished_data             CLAN_VANQ_TABLE;
typedef struct clan_member_data                 CLAN_MEMBER_TABLE;

/*
 * Extra tables - Tavolir
 */
typedef struct hero_data                        HERO_DATA;
typedef struct legend_data                      LEGEND_DATA;
typedef struct pkscore_data                     PKSCORE_DATA;

/*
 * DD3 mapping and direction defs; Tavolir
 */
typedef struct dir_data                 DIR_DATA;
typedef struct map_data                 MAP_DATA;               /* This struct is for current pc map */
typedef struct mapbook_data             MAPBOOK_DATA;           /* This struct is whole pc map book */
#define MAP_FORWARD 0
#define MAP_REVERSE 1

/*
 * Function types.
 */
typedef void DO_FUN     args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN   args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN  args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );
typedef void CONSTRUCT_FUN args( ( int sn, int level, CHAR_DATA *ch ) );

/*
 * Herb info for ranger skill 'gather'
 */
struct HERB
{
        char    *name;
        int      type;
        char    *spell;
        int      min_skill;
        int      chance;
        char    *keywords;
        char    *short_desc;
        char    *long_desc;
        char    *action;
};

#define MAX_HERBS 11

struct imbue_types
{
        char    *name;
        char    *short_desc;
        char    apply_buff;
        int     base_gain;       
};

#define MAX_IMBUE 9

#define BLUEPRINTS_MAX  2

/* Blueprint structure : blueprint_name, blueprint_desc, blueprint_ref blueprint_cost steel,titanium,adamantite,elctrum,starmetal */
struct blueprint_type
{
        char    *blueprint_name;
        char    *blueprint_desc;
        int     blueprint_ref;
        int     blueprint_ego;
        int     blueprint_damage [2];
        int     blueprint_cost [5];

};

/*
 * Songs for bards
 */
struct song
{
        char    *name;
        char    *long_desc;
        char    *affect;
        int      apply;
};

#define MAX_SONGS 7
int     remove_songs    ( CHAR_DATA *ch );
bool    has_tranquility ( CHAR_DATA *ch );


/*
 * String and memory management parameters
 */
#define MAX_KEY_HASH             1024   /* 1024 */
#define MAX_STRING_LENGTH        32768   /* 8192  */
#define MAX_INPUT_LENGTH          256


/*
 * Game parameters.
 * Increase the maxes if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_CLASS                          9
#define MAX_SUB_CLASS                     19    /* thats 18 plus 'none' - Shade 8.7.22 */
#define MAX_RACE                          24    /* thats 23 races plus 'none' - Owl 2/4/22 */
#define MAX_STAT                          32    /* 0->31  - Shade */
#define MAX_CLAN                           7
#define MAX_CLAN_LEVEL                     3
#define MAX_FORM                          20
#define MAX_COLORS                        11
#define MAX_LEVEL                        106
#define MAX_TRADE                          5
#define MAX_DAMAGE                      6000    /* Increased from 3k->6k for chaos blast --Owl 2/3/22 */
#define FULL_AIR_SUPPLY                    2    /* ticks before drowning; Gez */

#define L_IMM                       MAX_LEVEL
#define L_DIR                     ( L_IMM - 1 )
#define L_SEN                     ( L_DIR - 1 )
#define L_JUN                     ( L_SEN - 1 )
#define L_APP                     ( L_JUN - 1 )
#define L_BUI                     ( L_APP - 1 )
#define L_HER                     ( L_BUI - 1 )
#define LEVEL_IMMORTAL              L_BUI
#define LEVEL_HERO                ( LEVEL_IMMORTAL - 1 )

#define MAX_SKILL               511     /* 511 for dart */
#define MAX_PRE_REQ             1349    /* Added for smithy*/
#define MAX_SPELL_GROUP         431   /* Added for smithy */
#define MAX_GROUPS              53
#define MAX_FORM_SKILL          73      /* for form skill table */
#define MAX_VAMPIRE_GAG         26      /* ugly vampire/werewolf hack */


/*
 * Channel recall, 'review' command; Gezhp 2001
 */
#define REVIEW_BUFFER_SIZE              300     /* bytes per line */
#define REVIEW_BUFFER_LENGTH             50     /* lines per channel */
#define REVIEW_CHAT                       0
#define REVIEW_MUSIC                      1
#define REVIEW_SHOUT                      2
#define REVIEW_INFO                       3
#define REVIEW_ARENA                      4
#define REVIEW_IMMTALK                    5
#define REVIEW_DIRTALK                    6
#define REVIEW_NEWBIE                     7
#define REVIEW_CHANNEL_COUNT              8

DECLARE_DO_FUN(do_review);

struct review_data
{
        char    buffer [ REVIEW_BUFFER_SIZE ];
        time_t  timestamp;
};

extern struct review_data       review_buffer_public [ REVIEW_CHANNEL_COUNT ] [ REVIEW_BUFFER_LENGTH ];
extern struct review_data       review_buffer_clan   [ MAX_CLAN ] [ REVIEW_BUFFER_LENGTH ];

void add_to_public_review_buffer ( CHAR_DATA *ch, int channel, char *text );
void add_to_clan_review_buffer   ( CHAR_DATA *ch, int clan, char *text );


/*
 * Includes for board system
 * This is version 2 of the board system, (c) 1995-96 erwin@pip.dknet.dk
 */
#define CON_NOTE_TO             16
#define CON_NOTE_SUBJECT        17
#define CON_NOTE_EXPIRE         18
#define CON_NOTE_TEXT           19
#define CON_NOTE_FINISH         20

#define NOTE_DIR "../notes/"    /* set it to something you like */

#define DEF_NORMAL               0      /* No forced change, but default (any string)   */
#define DEF_INCLUDE              1      /* 'names' MUST be included (only ONE name!)    */
#define DEF_EXCLUDE              2      /* 'names' must NOT be included (one name only) */

#define MAX_BOARD                5

#define DEFAULT_BOARD            0      /* default board is board #0 in the boards      */
                                        /* It should be readable by everyone!           */

#define MAX_LINE_LENGTH         80      /* enforce a max length of 80 on text lines, reject longer lines */
                                        /* This only applies in the Body of the note */

#define MAX_NOTE_TEXT ( 4 * MAX_STRING_LENGTH - 1000 )

#define BOARD_NOTFOUND          -1      /* Error code from board_lookup() and board_number */

/* Data about a board */
struct board_data
{
        char *short_name;               /* Max 8 chars */
        char *long_name;                /* Explanatory text, should be no more than 40 ? chars */

        int read_level;                 /* minimum level to see board */
        int write_level;                /* minimum level to post notes */

        char *names;                    /* Default recipient */
        int force_type;                 /* Default action (DEF_XXX) */

        int purge_days;                 /* Default expiration */

        NOTE_DATA *note_first;          /* pointer to board's first note */
        bool changed;                   /* currently unused */

        bool announce_post;
};

typedef struct board_data BOARD_DATA;
extern BOARD_DATA boards [ MAX_BOARD ]; /* Declare */

void  finish_note               ( BOARD_DATA *board, NOTE_DATA *note ); /* attach a note to a board */
void  free_note                 ( NOTE_DATA *note );                    /* deallocate memory used by a note */
void  load_boards               ( void );                               /* load all boards */
int   board_lookup              ( const char *name );                   /* Find a board with that name */
bool  is_note_to                ( CHAR_DATA *ch, NOTE_DATA *note );     /* is the note to ch? */
void  personal_message          ( const char *sender, const char *to, const char *subject, const int expire_days, const char *text );
void  make_note                 ( const char* board_name, const char *sender, const char *to, const char *subject, const int expire_days, const char *text );
void  save_notes                ( );

void handle_con_note_to         ( DESCRIPTOR_DATA *d, char * argument );
void handle_con_note_subject    ( DESCRIPTOR_DATA *d, char * argument );
void handle_con_note_expire     ( DESCRIPTOR_DATA *d, char * argument );
void handle_con_note_text       ( DESCRIPTOR_DATA *d, char * argument );
void handle_con_note_finish     ( DESCRIPTOR_DATA *d, char * argument );

DECLARE_DO_FUN ( do_note  );
DECLARE_DO_FUN ( do_board );

/* for multiple recalls - see also the #define DEFAULT_RECALL below */
#define MAX_RECALL_POINTS       16
#define CLAN_RECALL              1

/* Patterns - note they start from 1 */
#define MAX_PATTERN 9

/* Soar - as above.  Pattern-like ability for straight shifters, used in hawk form */
#define MAX_SOAR 10

/* Parameter limits for imm commands in act_wiz.c */
#define MAX_RSTAT_OBJ             100
#define MAX_RSTAT_CHAR            100
#define MAX_OLOAD_COPIES          500
#define MAX_ADDFAME               500
#define MIN_ADDFAME              -500
#define MAX_ADDQP               10000
#define MIN_MSET_AGE               17
#define MAX_MSET_AGE            10000
#define MIN_MSET_HP               -10
#define MAX_MSET_HP             30000
#define MIN_MSET_MANA               0
#define MAX_MSET_MANA           30000
#define MIN_MSET_MOVE               0
#define MAX_MSET_MOVE           30000
#define MIN_MSET_STR_PRAC           0
#define MAX_MSET_STR_PRAC        1000
#define MIN_MSET_INT_PRAC           0
#define MAX_MSET_INT_PRAC        1000
#define MIN_MSET_TOTALQP            0
#define MAX_MSET_TOTALQP        50000
#define MIN_MSET_QUESTPOINTS        0
#define MAX_MSET_QUESTPOINTS    50000
#define MIN_MSET_QUESTTIME          0
#define MAX_MSET_QUESTTIME         15
#define MIN_MSET_DEITYTIMER        -1
#define MAX_MSET_DEITYTIMER     10000

/*
 * Crafting 'effectiveness' bonuses for skills/spells when performed in ROOM_CRAFT or
 * ROOM_SPELLCRAFT.  '120' = +20%.  Bonus percentages are interpreted to mean different
 * things by different spells/skills.  Don't set them below 100 (+0%).
 */
#define CRAFT_BONUS_BARK_SKIN           120
#define CRAFT_BONUS_BLADETHIRST         120
#define CRAFT_BONUS_BLESS_WEAPON        120
#define CRAFT_BONUS_BREW                120
#define CRAFT_BONUS_CONTINUAL_LIGHT     120
#define CRAFT_BONUS_CREATE_FOOD         120
#define CRAFT_BONUS_CREATE_SPRING       120
#define CRAFT_BONUS_CREATE_WATER        120
#define CRAFT_BONUS_ENCHANT_WEAPON      120
#define CRAFT_BONUS_ENHANCE_ARMOR       120
#define CRAFT_BONUS_FLESH_ARMOR         120
#define CRAFT_BONUS_FORGE               120
#define CRAFT_BONUS_POISON_WEAPON       130
#define CRAFT_BONUS_RECHARGE_ITEM       150
#define CRAFT_BONUS_SCRIBE              120
#define CRAFT_BONUS_SHARPEN             120
#define CRAFT_BONUS_SUMMON_DEMON        120
#define CRAFT_BONUS_SUMMON_FAMILIAR     120
#define CRAFT_BONUS_STONE_SKIN          120

/* types for skills */
#define TYPE_INT                 1
#define TYPE_STR                 2
#define TYPE_WIZ                 3
#define TYPE_NULL                4
#define MAX_ITEM_TYPE           42
#define MAX_WEAR                22
#define MAX_COLOR_LIST          18

/* Timing */
#define PULSE_PER_SECOND           4
#define PULSE_VIOLENCE          (  3 * PULSE_PER_SECOND )
#define PULSE_MOBILE            (  4 * PULSE_PER_SECOND )
#define PULSE_TICK              ( 30 * PULSE_PER_SECOND )
#define PULSE_AREA              ( 60 * PULSE_PER_SECOND )
#define PULSE_AUCTION           ( 20 * PULSE_PER_SECOND )

/* character types et al. */
#define CLASS_MAGE                      0
#define CLASS_CLERIC                    1
#define CLASS_THIEF                     2
#define CLASS_WARRIOR                   3
#define CLASS_PSIONICIST                4
#define CLASS_SHAPE_SHIFTER             5
#define CLASS_BRAWLER                   6
#define CLASS_RANGER                    7
#define CLASS_SMITHY                    8


/* constants for fame quest lengths and stuff Umgook 2/10/98 */
#define QUEST_MAX_DELAY                 10
#define QUEST_UNAVAILABLE_DELAY          3
#define QUEST_ABORT_DELAY               15

/*  Quest recall point info struct;  Gezhp 2000  */
struct quest_recall
{
        int     slot;
        int     cost;
        int     room;
        char*   name;
};


/*
 * table structures - geoff
 *
 * changed lengths so tables don't do silly things and forget when people
 * go walkies off the end, added the print lengths so only a certain
 * number of things are shown rather than having squeaps dumped on screen
 * Umgook 7/5/98
 *
 * Also added extra non-printed pkscore table rows to accomodate new
 * pkscore system; Gezhp 2000
 */
#define CLAN_PKILL_TABLE_LENGTH                 50
#define CLAN_PKILL_TABLE_LENGTH_PRINT           20
#define FAME_TABLE_LENGTH                       30
#define FAME_TABLE_LENGTH_PRINT                 20
#define CLAN_VANQ_TABLE_LENGTH                  50
#define CLAN_VANQ_TABLE_LENGTH_PRINT            20
#define CLAN_MEMBER_TABLE_LENGTH                25
#define CLAN_MEMBER_TABLE_LENGTH_PRINT          25
#define PKSCORE_TABLE_LENGTH                    125
#define PKSCORE_TABLE_LENGTH_PRINT              25
#define LEGEND_TABLE_LENGTH                     20
#define INFAMY_TABLE_LENGTH                     200 /* Shade Apr 22 */
#define INFAMY_TABLE_LENGTH_PRINT               20

/*
 * Classes and subclasses
 */
#define SUB_CLASS_NONE                          0
#define SUB_CLASS_NECROMANCER                   1
#define SUB_CLASS_WARLOCK                       2
#define SUB_CLASS_TEMPLAR                       3
#define SUB_CLASS_DRUID                         4
#define SUB_CLASS_NINJA                         5
#define SUB_CLASS_BOUNTY                        6
#define SUB_CLASS_THUG                          7
#define SUB_CLASS_KNIGHT                        8
#define SUB_CLASS_SATANIST                      9
#define SUB_CLASS_WITCH                         10
#define SUB_CLASS_WEREWOLF                      11
#define SUB_CLASS_VAMPIRE                       12
#define SUB_CLASS_MONK                          13
#define SUB_CLASS_MARTIAL_ARTIST                14
#define SUB_CLASS_BARBARIAN                     15
#define SUB_CLASS_BARD                          16
#define SUB_CLASS_ENGINEER                      17
#define SUB_CLASS_ALCHEMIST                     18

#define CLASS_WARLOCK   SUB_CLASS_WARLOCK

#define PRE_MAGE                CLASS_MAGE              + 1
#define PRE_CLERIC              CLASS_CLERIC            + 1
#define PRE_THIEF               CLASS_THIEF             + 1
#define PRE_WARRIOR             CLASS_WARRIOR           + 1
#define PRE_PSIONIC             CLASS_PSIONICIST        + 1
#define PRE_SHIFTER             CLASS_SHAPE_SHIFTER     + 1
#define PRE_BRAWLER             CLASS_BRAWLER           + 1
#define PRE_RANGER              CLASS_RANGER            + 1
#define PRE_SMITHY              CLASS_SMITHY            + 1

#define PRE_NECRO               SUB_CLASS_NECROMANCER                   + MAX_CLASS
#define PRE_WARLOCK             SUB_CLASS_WARLOCK                       + MAX_CLASS
#define PRE_TEMPLAR             SUB_CLASS_TEMPLAR                       + MAX_CLASS
#define PRE_DRUID               SUB_CLASS_DRUID                         + MAX_CLASS
#define PRE_NINJA               SUB_CLASS_NINJA                         + MAX_CLASS
#define PRE_BOUNTY              SUB_CLASS_BOUNTY                        + MAX_CLASS
#define PRE_THUG                SUB_CLASS_THUG                          + MAX_CLASS
#define PRE_KNIGHT              SUB_CLASS_KNIGHT                        + MAX_CLASS
#define PRE_SATANIST            SUB_CLASS_SATANIST                      + MAX_CLASS
#define PRE_WITCH               SUB_CLASS_WITCH                         + MAX_CLASS
#define PRE_WEREWOLF            SUB_CLASS_WEREWOLF                      + MAX_CLASS
#define PRE_VAMPIRE             SUB_CLASS_VAMPIRE                       + MAX_CLASS
#define PRE_MONK                SUB_CLASS_MONK                          + MAX_CLASS
#define PRE_ARTIST              SUB_CLASS_MARTIAL_ARTIST                + MAX_CLASS
#define PRE_BARBARIAN           SUB_CLASS_BARBARIAN                     + MAX_CLASS
#define PRE_BARD                SUB_CLASS_MONK                          + MAX_CLASS
#define PRE_ENGINEER            SUB_CLASS_ENGINEER                      + MAX_CLASS
#define PRE_ALCHEMIST           SUB_CLASS_ALCHEMIST                     + MAX_CLASS

/*
 * Races
 */
#define RACE_NONE                       0
#define RACE_HUMAN                      1
#define RACE_ELF                        2
#define RACE_WILD_ELF                   3
#define RACE_ORC                        4
#define RACE_GIANT                      5
#define RACE_SATYR                      6
#define RACE_OGRE                       7
#define RACE_GOBLIN                     8
#define RACE_HALF_DRAGON                9
#define RACE_HALFLING                   10
#define RACE_DWARF                      11
#define RACE_CENTAUR                    12
#define RACE_DROW                       13
#define RACE_TROLL                      14
#define RACE_ALAGHI                     15
#define RACE_HOBGOBLIN                  16
#define RACE_YUAN_TI                    17
#define RACE_FAE                        18
#define RACE_SAHUAGIN                   19
#define RACE_TIEFLING                   20
#define RACE_JOTUN                      21
#define RACE_GENASI                     22
#define RACE_ILLITHID                   23


/*
 * Site ban structure
 */
struct ban_data
{
        BAN_DATA *      next;
        char *          name;
};


/*
 * Time and weather
 */
#define SUN_DARK                0
#define SUN_RISE                1
#define SUN_LIGHT               2
#define SUN_SET                 3

#define SKY_CLOUDLESS           0
#define SKY_CLOUDY              1
#define SKY_RAINING             2
#define SKY_LIGHTNING           3

#define MOON_FULL               0
#define MOON_WAXING             1
#define MOON_WANING             2
#define MOON_NONE               3

struct time_info_data
{
        int     hour;
        int     day;
        int     month;
        int     year;
};

struct weather_data
{
        int     mmhg;
        int     change;
        int     sky;
        int     sunlight;
        int     moonlight;
};

#define IS_FULL_MOON ( weather_info.moonlight == MOON_FULL \
                                        && weather_info.sunlight == SUN_DARK )


/* Auction data */
struct  auction_data
{
        OBJ_DATA  *     item;           /* a pointer to the item */
        CHAR_DATA *     seller;         /* a pointer to the seller - which may NOT quit */
        CHAR_DATA *     buyer;          /* a pointer to the buyer - which may NOT quit */
        int             bet;            /* last bet - or 0 if no one has bet anything */
        int             going;          /* 1,2, sold */
        int             pulse;          /* how many pulses (.25 sec) until another call-out ? */
        int             reserve;        /* Reserve price */
        bool            from_bank;      /* TRUE if bet was placed from bank funds (else from purse) */
};


/*
 * Connected state for a channel.
 */
#define CON_PLAYING                             0
#define CON_GET_NAME                            1
#define CON_GET_OLD_PASSWORD                    2
#define CON_CONFIRM_NEW_NAME                    3
#define CON_GET_NEW_PASSWORD                    4
#define CON_CONFIRM_NEW_PASSWORD                5
#define CON_DISPLAY_RACE                        6
#define CON_GET_NEW_RACE                        7
#define CON_CONFIRM_NEW_RACE                    8
#define CON_GET_NEW_SEX                         9
#define CON_DISPLAY_CLASS                       10
#define CON_GET_NEW_CLASS                       11
#define CON_CONFIRM_CLASS                       12
#define CON_READ_MOTD                           13
#define CON_GENERATE_STATS                      14
#define CON_CONFIRM_STATS                       15
/* notes 16-20 - Shade */
#define CON_WELCOME_RACE                        21
#define CON_GET_DISCONNECTION_PASSWORD          22
#define CON_CHECK_ANSI                          23

#define MAX_CONNECTIONS                         256   /* Maximum number of descriptors */
extern int connection_count;
extern DESCRIPTOR_DATA *initiative_list [ MAX_CONNECTIONS ];


/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
        DESCRIPTOR_DATA *               next;
        DESCRIPTOR_DATA *               snoop_by;
        CHAR_DATA *                     character;
        CHAR_DATA *                     original;
        char *                          host;
        int                             descriptor;
        int                             connected;
        bool                            fcommand;
        char                            inbuf  [ MAX_INPUT_LENGTH * 6 ];        /* was 4 */
        char                            incomm [ MAX_INPUT_LENGTH * 2 ];        /* was 1 */
        char                            inlast [ MAX_INPUT_LENGTH * 2 ];        /* was 1 */
        int                             repeat;
        char *                          showstr_head;
        char *                          showstr_point;
        char *                          outbuf;
        int                             outsize;
        int                             outtop;

        /* ident stuff */
        int             ifd;
        pid_t           ipid;
        char *          ident;
        int             port;
        int             ip;
};


/*
 *  Mobile body bits and macros.
 *  See online help for 'MOBILE BODY FORM'.  Gezhp 99
 */
#define BODY_NO_HEAD            BIT_0
#define BODY_NO_EYES            BIT_1
#define BODY_NO_ARMS            BIT_2
#define BODY_NO_LEGS            BIT_3
#define BODY_NO_HEART           BIT_4
#define BODY_NO_SPEECH          BIT_5
#define BODY_NO_CORPSE          BIT_6
#define BODY_HUGE               BIT_7
#define BODY_INORGANIC          BIT_8

#define HAS_HEAD( ch )          ( !(ch->body_form & BODY_NO_HEAD ) )
#define HAS_EYES( ch )          ( !(ch->body_form & BODY_NO_EYES ) )
#define HAS_ARMS( ch )          ( !(ch->body_form & BODY_NO_ARMS ) )
#define HAS_LEGS( ch )          ( !(ch->body_form & BODY_NO_LEGS ) )
#define HAS_HEART( ch )         ( !(ch->body_form & BODY_NO_HEART ) )
#define CAN_SPEAK( ch )         ( !(ch->body_form & BODY_NO_SPEECH ) )
#define MAKES_CORPSE( ch )      ( !(ch->body_form & BODY_NO_CORPSE ) )
#define IS_HUGE( ch )           ( ch->body_form & BODY_HUGE )
#define IS_INORGANIC( ch )      ( ch->body_form & BODY_INORGANIC )


/*
 * Attribute bonus structures.
 */
struct str_app_type
{
        int     tohit;
        int     todam;
        int     carry;
        int     wield;
};

struct dex_app_type
{
        int     defensive;
        int     initiative;
};

struct con_app_type
{
        int     hitp;
        int     shock;
};


/*
 * TO types for act.
 */
#define TO_ROOM         0
#define TO_NOTVICT      1
#define TO_VICT         2
#define TO_CHAR         3


/*
 * Help table types.
 */
struct help_data
{
        HELP_DATA *     next;
        int             level;
        char *          keyword;
        char *          text;
};


/*
 * Shop types.
 */
struct shop_data
{
        SHOP_DATA *     next;                           /* Next shop in list */
        int             keeper;                         /* Vnum of shop keeper mob */
        int             buy_type [ MAX_TRADE ];         /* Item types shop will buy */
        int             profit_buy;                     /* Cost multiplier for buying */
        int             profit_sell;                    /* Cost multiplier for selling */
        int             open_hour;                      /* First opening hour */
        int             close_hour;                     /* First closing hour */
};


/* COIN STRUCTURE Danath */
struct coin_data
{
        int     plat;
        int     gold;
        int     silver;
        int     copper;
};

/* Smelting MAterials - Brutus */
struct smelting_data
{
        int     smelted_iron;
        int     smelted_titanium;
        int     smelted_adamantite;
        int     smelted_electrum;
        int     smelted_starmetal;
};

/*
 * Per-class stuff.
 */
struct class_type
{
        char    who_name [ 4 ];         /* Three-letter name for 'who' */
        char    show_name [ 9 ];        /* Full name of class for 'who' */
        int     attr_prime;             /* Prime attribute */
        int     weapon;                 /* First weapon */
        int     guild;                  /* Vnum of guild room */
        int     skill_adept;            /* Maximum skill level */
        int     thac0_00;               /* Thac0 for level  0 */
        int     thac0_47;               /* Thac0 for level 47 */
        int     hp_min;                 /* Min hp gained on leveling */
        int     hp_max;                 /* Max hp gained on leveling */
        bool    fMana;                  /* Class gains mana on level */
        char    sub1 [ 9 ];             /* First sub-class-name */
        char    sub2 [ 9 ];             /* Second sub-class-name */
        char    sub_who1 [ 4 ];         /* Had to add for do_change     */
        char    sub_who2 [ 4 ];         /* Had to change for do_change */
        int     class_stats [ 5 ];      /* class stat modifier on startup */
};


struct clan_type
{
        char    *name;
        char    who_name [ 4 ];
        int     hall;
        int     guard_room;
        int     heal_room;
        bool    independent;            /* true for loners */
        char    colour_code;
};


struct clan_items
{
        char    clans_name [ 4 ];
        int     first_item;
        int     second_item;
        int     third_item;
        int     pouch_item;
};


struct pattern_points
{
        int location;
};


struct soar_points
{
        int destination;
};


struct sub_class_type
{
        char    who_name [ 4 ];         /* Three-letter name */
        char    show_name [ 9 ];        /* Full class name for who */
        int     attr_bonus;             /* Attribute bonus */
        bool    bMana;                  /* Mana bonus for new class? */
};


/*
 * wearing restrictions, body parts for form, types for wear
 * and skills for each form..... - geoff.
 */
struct form_skill_struct
{
        int *skill;
        int form;
};


struct vampire_gag
{
        int *skill;
};


struct pre_req_struct
{
        int *skill;
        int *pre_req;
        int min;
        int group;
};


struct spell_group_struct
{
        int *skill;
        int min;
};


/*
 * RACE STRUCTURE - Brutus
 */
#define RACE_NONE       0

struct race_struct
{
        char    *who_name;
        char    *race_name;
        int      str_bonus;
        int      int_bonus;
        int      wis_bonus;
        int      dex_bonus;
        int      con_bonus;
        int      hp_regen;
        int      mana_regen;
        int      move_regen;
        char    *spell_one;             /* First spell for that race */
        char    *spell_two;             /* Second spell for that race */
        int      size;                  /* Char sizes are Small (1), med (2) and Large (3) */
};


/* LEVEL STRUCTURE :) - brutus */
struct level_struct
{
        int     level;
        int     exp_level;
        int     exp_total;
};

/* soar struct to figure our what soar locations a char has access to. -- Owl 30/3/22 */
struct soar_struct
{
        int     hawkform_perc;
        int     soar_perc;
        int     char_level;
};


/*
 *  Data Un color type
 */
struct color_data
{
        char    code [ 10 ];
        char    act_code [ 5 ];
        char    name [ 15 ];
        int     number;
};


/*
 * Data structure for notes.
 */
struct note_data
{
        NOTE_DATA * next;
        char * sender;
        char * date;
        char * to_list;
        char * subject;
        char * text;
        time_t date_stamp;
        time_t expire;
};


/*
 * An affect.
 */
struct affect_data
{
        AFFECT_DATA *   next;
        int             type;
        int             duration;
        int             location;
        int             modifier;
        int             bitvector;
        bool            deleted;
};


/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
        int     number;
        int     killed;
};


/*
 * DD-3 Mapping system structure - Tavolir
 */
struct dir_data
{
        char    *name;
        int      reverse;
        char    *bamfin;
};

struct map_data
{
        int     current_vnum;
        char    *name;
        char    *desc;
        char    *steps;
        bool    direction;
        bool    is_mapping;
};

struct mapbook_data
{
        MAPBOOK_DATA *next;
        char *name;
        char *desc;
        char *steps;
        int vnum_start;
        int vnum_end;
};


/*
 * DD3 hero table - Tavolir
 */
struct hero_data
{
        HERO_DATA * next;
        char * name;
        char * date;
        int class;
        bool subclassed;
};

/*
 * DD3 pkill table -Tavolir
 */
struct pkscore_data
{
        char name [ 15 ];
        int pkills;
        int pkscore;
        int pdeaths;
        int class;
        bool subclassed;
        char clan [ 6 ];
};

/*
 * DD3 legends table - Tavolir
 */
struct legend_data
{
        char name [ 15 ];
        int fame;
        int class;
        bool subclassed;
};


/*
 * the long awaited fame table, geoff
 */
struct fame_data
{
        char name [ 15 ];
        char title [ 2 ];
        int fame;
};

/*
 * Mob infamy table
 */

struct infamy_data
{
        char name[40];
        char loc[40];
        int kills;
        int vnum;
};

/*
 * clan pkill table data entry
 */
struct clan_pkill_data
{
        char name [ 15 ];
        int  pkills;
};

/*
 * clan vanq table data entry
 */
struct clan_vanquished_data
{
        char name [ 15 ];
        int killed;
};

/*
 * clan member table data entry
 */
struct clan_member_data
{
        char name [ 15 ];
        bool leader;
};


/*
 *      Deity declarations and macros
 *
 *      Edit the function 'deities_active' to control whether
 *      deity system code being called during the game.
 *
 *      All code sits in 'deity.c'.
 *
 *      Gezhp and Shade, 2000-2001
 */

#define NUMBER_DEITIES          19

#define DEITY_TYPE_PEACEFUL     0
#define DEITY_TYPE_WARLIKE      1
#define DEITY_TYPE_DEATH        2
#define DEITY_TYPE_CHAOTIC      3
#define DEITY_TYPE_LAWFUL       4
#define DEITY_TYPE_NATURE       5
#define DEITY_NUMBER_TYPES      6

#define DEITY_PERSONALITY_HELPFUL               0
#define DEITY_PERSONALITY_CRUEL                 1
#define DEITY_PERSONALITY_JUDGEMENTAL           2
#define DEITY_PERSONALITY_MYSTERIOUS            3
#define DEITY_PERSONALITY_PROTECTIVE            4
#define DEITY_PERSONALITY_PROUD                 5
#define DEITY_PERSONALITY_MISCHIEVIOUS          6
#define DEITY_NUMBER_PERSONALITIES              7

#define DEITY_STATURE_UNKNOWN   0
#define DEITY_STATURE_WEAK      1
#define DEITY_STATURE_MODERATE  2
#define DEITY_STATURE_POWERFUL  3

#define DEITY_FAVOUR_UNKNOWN                    0
#define DEITY_FAVOUR_STRONG_DISFAVOUR           1
#define DEITY_FAVOUR_DISFAVOUR                  2
#define DEITY_FAVOUR_INDIFFERENCE               3
#define DEITY_FAVOUR_FAVOUR                     4
#define DEITY_FAVOUR_STRONG_FAVOUR              5

#define DEITY_PRAYER_DESTRUCTION                0
#define DEITY_PRAYER_COMBAT                     1
#define DEITY_PRAYER_LOCATE                     2
#define DEITY_PRAYER_REVEAL                     3
#define DEITY_PRAYER_WEAKEN                     4
#define DEITY_PRAYER_PROTECTION                 5
#define DEITY_PRAYER_RESCUE                     6
#define DEITY_PRAYER_HEALING                    7
#define DEITY_PRAYER_PLAGUE                     8
#define DEITY_PRAYER_DEATH                      9
#define DEITY_PRAYER_CHAOS                      10
#define DEITY_PRAYER_TRANSPORT                  11
#define DEITY_PRAYER_PASSAGE                    12
#define DEITY_NUMBER_PRAYERS                    13
#define DEITY_PRAYERS_PER_TYPE                  4

#define DEITY_PFLAG_PATRON_WARNING_1            BIT_0
#define DEITY_PFLAG_PATRON_WARNING_2            BIT_1
#define DEITY_PFLAG_PATRON_CHOSEN               BIT_2
#define DEITY_PFLAG_RELEASE_WARNING             BIT_3
#define DEITY_PFLAG_LEFT_GOOD                   BIT_4
#define DEITY_PFLAG_LEFT_NEUTRAL                BIT_5
#define DEITY_PFLAG_LEFT_EVIL                   BIT_6
#define DEITY_PFLAG_RELEASE_ATTEMPT             BIT_7
#define DEITY_PFLAG_GRANT_ACCESS                BIT_8   /* temporary for testing */

#define DEITY_DEFAULT_PLAYER_TIMER              360
#define DEITY_PRAYER_TIME_PENALTY               480
#define DEITY_MAX_FAVOUR                        1000
#define DEITY_MAX_POWER                         1000
#define DEITY_PANTHEON_UPDATE_TIMER             10

#define DEITY_TIMER_MAX                         480

#define SET_DEITY_TYPE_TIMER( ch, type, value ) \
        ( ( ch )->pcdata->deity_type_timer [ ( type ) ] \
        = URANGE( 0, ( ( ch )->pcdata->deity_type_timer [ ( type ) ] + ( value ) ), DEITY_TIMER_MAX ) )

#define SET_DEITY_PERSONALITY_TIMER( ch, type, value ) \
        ( ( ch )->pcdata->deity_personality_timer [ ( type ) ] \
        = URANGE( 0, ( ( ch )->pcdata->deity_personality_timer [ ( type ) ] + ( value ) ), DEITY_TIMER_MAX ) )

DECLARE_DO_FUN ( do_pantheoninfo );
DECLARE_DO_FUN ( do_pantheonrank );
DECLARE_DO_FUN ( do_pantheon );
DECLARE_DO_FUN ( do_pray );

struct deity_info
{
        char *name;
        char *title;
        char *description;
        int      sex;
        int      alignment;
        int      type;
        int      personality;
        int      extra_flags;
        int      class_favour     [ MAX_CLASS ];
        int      sub_class_favour [ MAX_SUB_CLASS ];
        int      race_favour      [ MAX_RACE ];
};

struct pantheon_status
{
        int     timer;
        int     atmosphere;
        int     ranking [ NUMBER_DEITIES ];
        int     power   [ NUMBER_DEITIES ];

        /* These variables for logging pantheon movements */
        double  average_rank    [ NUMBER_DEITIES ];
        long    total_power     [ NUMBER_DEITIES ];
        int     top_count       [ NUMBER_DEITIES ];
        int     update_count;
};

extern const struct deity_info deity_info_table [ NUMBER_DEITIES ];
extern struct pantheon_status pantheon_status_table;
extern bool deity_system_halted;

void log_deity                          ( char *text );
bool init_pantheon                      ( );
void update_pantheon                    ( );
void rank_pantheon                      ( );
bool deities_active                     ( );
bool allow_deity_command                ( CHAR_DATA *ch );
int  get_deity_stature                  ( int deity );
int  get_deity_favour                   ( CHAR_DATA *ch, int deity );
bool player_knows_deity_stature         ( CHAR_DATA *ch, int deity );
void update_player_favour               ( CHAR_DATA *ch, int deity );
void report_to_followers                ( int deity, char *message );
void patron_message                     ( char *message, CHAR_DATA *ch );
int  get_race_favour                    ( CHAR_DATA *ch, int deity );
int  get_class_favour                   ( CHAR_DATA *ch, int deity );
void pray_for_patronage                 ( CHAR_DATA *ch, int deity );
void pray_for_release                   ( CHAR_DATA *ch );
void deity_update                       ( );
bool effect_is_prayer                   ( AFFECT_DATA *af );

void prayer_destruction                         ( CHAR_DATA *ch );
void prayer_combat                              ( CHAR_DATA *ch );
void prayer_locate                              ( CHAR_DATA *ch, char *text );
void prayer_weaken                              ( CHAR_DATA *ch, char *text );
void prayer_reveal                              ( CHAR_DATA *ch, char *text );
void prayer_protection                          ( CHAR_DATA *ch );
void prayer_rescue                              ( CHAR_DATA *ch );
void prayer_healing                             ( CHAR_DATA *ch );
void prayer_plague                              ( CHAR_DATA *ch );
void prayer_chaos                               ( CHAR_DATA *ch );
void prayer_transport                           ( CHAR_DATA *ch, char *text );
void prayer_passage                             ( CHAR_DATA *ch );
void prayer_death                               ( CHAR_DATA *ch, char *text );


/*
 * Log last command before crash
 */
extern char     last_command    [ MAX_STRING_LENGTH ];
extern char     last_function   [ MAX_STRING_LENGTH ];
void            install_crash_handler ( );


/*
 * Tournament system; Gez 2000
 */

struct tournament_entrant
{
        CHAR_DATA *ch;
        char stats      [ 256 ];
        char killer     [ 256 ];
        int  status;
};

struct tournament_team
{
        char *keyword;
        char *name;
        char *flag;
        char *colour_code;
};

#define TOURNAMENT_STATUS_NONE                          0
#define TOURNAMENT_STATUS_POSTED                        1
#define TOURNAMENT_STATUS_CLOSED                        2
#define TOURNAMENT_STATUS_RUNNING                       3
#define TOURNAMENT_STATUS_FINISHED                      4

#define TOURNAMENT_OPTION_NOFLEE                BIT_0
#define TOURNAMENT_OPTION_NOWIMPY               BIT_1
#define TOURNAMENT_OPTION_SAMEROOM              BIT_2
#define TOURNAMENT_OPTION_NOQUAFF               BIT_3
#define TOURNAMENT_OPTION_NOPILL                BIT_4
#define TOURNAMENT_OPTION_NOBRANDISH            BIT_5
#define TOURNAMENT_OPTION_NOSUMMON              BIT_6

#define TOURNAMENT_ASSEMBLY_VNUM                3001
#define TOURNAMENT_ARENA_ENTRANCE_VNUM          14200

#define TOURNAMENT_MAX_ENTRANTS                 50
#define TOURNAMENT_MAX_BANNED                   50
#define TOURNAMENT_MAX_TEAMS                    4

#define TOURNAMENT_ENTRANT_ALIVE                0
#define TOURNAMENT_ENTRANT_DEAD                 1
#define TOURNAMENT_ENTRANT_DISQUALIFIED         2

#define TOURNAMENT_TYPE_OPEN                    0
#define TOURNAMENT_TYPE_TEAM                    1
#define TOURNAMENT_TYPE_TAG                     2

extern struct           tournament_entrant tournament_entrants [ TOURNAMENT_MAX_TEAMS ] [TOURNAMENT_MAX_ENTRANTS];
extern struct           tournament_team tournament_teams [ TOURNAMENT_MAX_TEAMS ];
extern                  CHAR_DATA *tournament_banned [ TOURNAMENT_MAX_BANNED ];
extern                  CHAR_DATA *tournament_host;
extern int              tournament_entrant_count [ TOURNAMENT_MAX_TEAMS ];
extern int              tournament_banned_count;
extern int              tournament_status;
extern int              tournament_lower_level;
extern int              tournament_upper_level;
extern int              tournament_options;
extern int              tournament_countdown;
extern int              tournament_type;
extern int              tournament_team_count;

DECLARE_DO_FUN( do_tournament );

void tournament_init                                    ( );
bool is_entered_in_tournament                           ( CHAR_DATA *ch );
void tournament_message                                 ( char *text, CHAR_DATA *ch );
void arena_commentary                                   ( char *text, CHAR_DATA *ch, CHAR_DATA *victim );
void remove_from_tournament                             ( CHAR_DATA *ch );
void do_tournament_countdown                            ( );
void disqualify_tournament_entrant                      ( CHAR_DATA *ch, char *reason );
void check_for_tournament_winner                        ( );
bool is_tournament_death                                ( CHAR_DATA *victim, CHAR_DATA *killer );
void tournament_death                                   ( CHAR_DATA *victim, CHAR_DATA *killer );
bool is_still_alive_in_tournament                       ( CHAR_DATA *ch );
bool tournament_action_illegal                          ( CHAR_DATA *ch, int flag );


/*
 * Tournament bots; Gezhp 2001
 */

#define BOT_TEMPLATE_NUMBER                     5
#define BOT_MAX_ENTRANTS                        20
#define BOT_MAX_EQ_SLOTS                        15
#define BOT_MAX_TARGETS                         25
#define BOT_VNUM                                11000   /* Reserved vnum for all bots */

#define BOT_TYPE_STATIC                         0
#define BOT_TYPE_CAUTIOUS                       1
#define BOT_TYPE_HUNTER                         2

#define BOT_FLAG_TEAMS_UP                       BIT_0
#define BOT_FLAG_ATTACKS_HIGH_TARGETS           BIT_1
#define BOT_FLAG_WIMPY                          BIT_2
#define BOT_FLAG_STEALS_KILLS                   BIT_3
#define BOT_FLAG_IGNORE_EASY_TARGETS            BIT_4
#define BOT_FLAG_HEALTH_BONUS                   BIT_5
#define BOT_FLAG_HITROLL_BONUS                  BIT_6
#define BOT_FLAG_DAMROLL_BONUS                  BIT_7
#define BOT_FLAG_RANDOM_TARGET                  BIT_8
#define BOT_FLAG_OPENS_IMMEDIATELY              BIT_9

#define BOT_STATUS_WAITING                      0
#define BOT_STATUS_ALIVE                        1
#define BOT_STATUS_DEAD                         2

#define BOT_SPEED_VERY_FAST                     1
#define BOT_SPEED_FAST                          2
#define BOT_SPEED_MEDIUM                        3
#define BOT_SPEED_SLOW                          4

#define BOT_OPENING_MURDER                      0
#define BOT_OPENING_GOUGE                       1
#define BOT_OPENING_BACKSTAB                    2
#define BOT_OPENING_TRAP                        3
#define BOT_OPENING_TRAP_BACKSTAB               4
#define BOT_OPENING_LUNGE                       5

struct bot_template
{
        char    *name;
        char    *blurb;
        int      alignment;
        int      bot_type;
        int      bot_flags;
        int      speed;
        int      sight;
        int      affect_flags;
        int      body_form;
        int      sex;
        char    *special;
        int      opening_attack;
        int      eq [ BOT_MAX_EQ_SLOTS ] [ 2 ];
};

struct bot_status
{
        int                     id;
        int                     status;
        int                     timer;
        int                     level;
        CHAR_DATA               *ch;
        char                    name [ 32 ];
        int                     desperation;
        char                    target [ 32 ];
        int                     move_direction;
        int                     move_distance;
        bool                    few_targets;
};

extern struct bot_template      bot_template_table      [ BOT_TEMPLATE_NUMBER ];
extern struct bot_status        bot_status_table        [ BOT_MAX_ENTRANTS ];
extern int    bot_entry_count;

void            do_bot                          ( CHAR_DATA *ch, char *argument );
void            do_bot_list                     ( CHAR_DATA *ch );
void            do_bot_status                   ( CHAR_DATA *ch );
void            do_bot_enter                    ( CHAR_DATA *ch, char *argument );
void            do_bot_remove                   ( CHAR_DATA *ch, char *argument );
void            log_bot                         ( int num, char *text );
CHAR_DATA*      create_bot                      ( int id, int level );
void            bot_update                      ( );
void            bot_think                       ( int num );
CHAR_DATA*      bot_get_room_target             ( int num );
void            bot_murder                      ( int num, CHAR_DATA *victim );
bool            bot_is_desperate                ( int num );
CHAR_DATA*      bot_scan_for_target             ( int num );
int             bot_can_see                     ( int num, char *argument );
bool            bot_consider_target             ( int num, CHAR_DATA *victim );
CHAR_DATA*      bot_choose_target               ( int num );



/*
 *  Wanted board; Gezhp 2001
 */

struct wanted_data
{
        struct wanted_data *next;
        char *date;
        char *name;
        char *class;
        char *clan;
        int reward;
};

DECLARE_DO_FUN( do_wanted );
typedef struct wanted_data WANTED_DATA;
void    remove_from_wanted_table ( char *name );
void    update_wanted_table ( CHAR_DATA *ch );
void    load_wanted_table ( );
void    save_wanted_table ( );
extern  WANTED_DATA *wanted_list_first;
extern  WANTED_DATA *wanted_list_last;




/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Ansi colors and VT100 codes
 * Used in #PLAYER
 *
 * On most machines that use ANSI, namely the IBM PCs, the decimal value for
 * the escape character is 27 (1B hex).     Change this value when needed.
 */

#define ESC             '\x1b'

#define BLACK           "\x1b[30m"                      /* These are foreground color codes */
#define RED             "\x1b[31m"
#define GREEN           "\x1b[32m"
#define YELLOW          "\x1b[33m"
#define BLUE            "\x1b[34m"
#define PURPLE          "\x1b[35m"
#define CYAN            "\x1b[36m"
#define GREY            "\x1b[37m"
#define D_GREY          "\x1b[1m\x1b[30m"

#define B_BLACK         "\x1b[40m"                      /* These are background color codes */
#define B_RED           "\x1b[41m"
#define B_GREEN         "\x1b[42m"
#define B_YELLOW        "\x1b[43m"
#define B_BLUE          "\x1b[44m"
#define B_PURPLE        "\x1b[45m"
#define B_CYAN          "\x1b[46m"
#define B_GREY          "\x1b[47m"
#define B_D_GREY        "\x1b[1;47m"

/* Below are VT100 and ANSI codes (above are ANSI exclusively) */

#define EEEE            "\x1b#8"                        /* Turns screen to EEEEs */
#define CLRSCR          "\x1b[2j"                       /* Clear screen */
#define CLREOL          "\x1b[K"                        /* Clear to end of line */

#define UPARR           "\x1b[A"                        /* Up one line */
#define DOWNARR         "\x1b[B"                        /* Down one line */
#define RIGHTARR        "\x1b[C"                        /* Right one column */
#define LEFTARR         "\x1b[D"                        /* Left one column */
#define HOMEPOS         "\x1b[H"                        /* Home (upper left) */

#define BOLD            "\x1b[1m"                       /* High intensity */
#define FLASH           "\x1b[5m"                       /* Flashing text */
#define INVERSE         "\x1b[7m"                       /* XORed back and fore */
#define NTEXT           "\x1b[0m"                       /* Normal text (grey) */

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define CLEAR           NTEXT                           /* Resets Colour */
#define C_RED           RED                                     /* Normal Colours */
#define C_GREEN         GREEN
#define C_YELLOW        YELLOW
#define C_BLUE          BLUE
#define C_MAGENTA       PURPLE
#define C_CYAN          CYAN
#define C_WHITE         "\x1b[37m"
#define C_D_GREY        GREY                            /* Light Colors */
#define C_B_RED         "\x1b[1m\x1b[31m"
#define C_B_GREEN       "\x1b[1m\x1b[32m"
#define C_B_YELLOW      "\x1b[1m\x1b[33m"
#define C_B_BLUE        "\x1b[1m\x1b[34m"
#define C_B_MAGENTA     "\x1b[1m\x1b[35m"
#define C_B_CYAN        "\x1b[1m\x1b[36m"
#define C_B_WHITE       "\x1b[1m\x1b[37m"
/*
 * Other codes of note for future ANSI development:
 * The <esc>[y;xH code works nicely for positioning the cursor.
 */

/*
 * Color codings for
 * channels+says
 */
#define COLOR_AUCTION                                   0
#define COLOR_GOSSIP                                    1
#define COLOR_SHOUT                                     2
#define COLOR_IMMTALK                                   3
#define COLOR_SAY                                       4
#define COLOR_TELL                                      5
#define COLOR_CLAN                                      6
#define COLOR_DIRTALK                                   7
#define COLOR_SERVER                                    8
#define COLOR_ARENA                                     9
#define COLOR_NEWBIE                                    10   /* --Owl 2/3/22 */


/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD                       3060
#define MOB_VNUM_VAMPIRE                         3404
#define MOB_VNUM_ULT                             3160

#define MOB_VNUM_AIR_ELEMENTAL                  8914
#define MOB_VNUM_EARTH_ELEMENTAL                8915
#define MOB_VNUM_WATER_ELEMENTAL                8916
#define MOB_VNUM_FIRE_ELEMENTAL                 8917
#define MOB_VNUM_DUST_ELEMENTAL                 8918
#define MOB_VNUM_PONY                           19900
#define MOB_VNUM_HORSE                          19901
#define MOB_VNUM_PEGASUS                        19902
#define MOB_VNUM_SILVER                         19903
#define MOB_VNUM_WOLF                           19904
#define MOB_VNUM_DEMON                          19905
#define MOB_VNUM_LESSER                         19906


/*
 * ACT bits for mobs.  Used in #MOBILES.
 * Bit values defined above; see 'HELP BITZ' online for a list.
 */
#define ACT_IS_NPC                      BIT_0   /* Auto set for mobs */
#define ACT_SENTINEL                    BIT_1   /* Stays in one room */
#define ACT_SCAVENGER                   BIT_2   /* Picks up objects */
#define ACT_QUESTMASTER                 BIT_3   /* For quests */
#define ACT_AGGRESSIVE                  BIT_5   /* Attacks PC's */
#define ACT_STAY_AREA                   BIT_6   /* Won't leave area */
#define ACT_WIMPY                       BIT_7   /* Flees when hurt */
#define ACT_PET                         BIT_8   /* Auto set for pets */
#define ACT_NO_QUEST                    BIT_9   /* Cannot be selected as quest target */
#define ACT_PRACTICE                    BIT_10  /* Can practice PC's */
#define ACT_GAMBLE                      BIT_11  /* Runs a gambling game */
#define ACT_NOCHARM                     BIT_12  /* Not charmable - Brutus */
#define ACT_IS_HEALER                   BIT_13  /* For healer spec */
#define ACT_IS_FAMOUS                   BIT_14  /* Award fame for kill */
#define ACT_LOSE_FAME                   BIT_15  /* Lose fame for kill */
#define ACT_WIZINVIS_MOB                BIT_16  /* Mob cant be seen by players */
#define ACT_MOUNTABLE                   BIT_17  /* Mob can be ridden */
#define ACT_TINKER                      BIT_18  /* Mob can repair equipment  - Owl */
#define ACT_BANKER                      BIT_19  /* Banker Mobs  - Brutus */
#define ACT_IDENTIFY                    BIT_20  /* Mob can ID things */
#define ACT_DIE_IF_MASTER_GONE          BIT_21  /* Destroy mob if master not in room; Gezhp */
#define ACT_CLAN_GUARD                  BIT_22  /* Protects from some combat skills; Gezhp */
#define ACT_NO_SUMMON                   BIT_23  /* Mob may not be summoned; Gezhp */
#define ACT_NO_EXPERIENCE               BIT_24  /* No exp rewarded for kill; Gezhp */


/*
 *  Bits for 'affected_by'.
 *  Used in #MOBILES.
 */
#define AFF_BLIND                       BIT_0
#define AFF_INVISIBLE                   BIT_1
#define AFF_DETECT_EVIL                 BIT_2
#define AFF_DETECT_INVIS                BIT_3
#define AFF_DETECT_MAGIC                BIT_4
#define AFF_DETECT_HIDDEN               BIT_5
#define AFF_HOLD                        BIT_6   /*  mob is trapped  */
#define AFF_SANCTUARY                   BIT_7
#define AFF_FAERIE_FIRE                 BIT_8
#define AFF_INFRARED                    BIT_9
#define AFF_CURSE                       BIT_10
#define AFF_FLAMING                     BIT_11
#define AFF_POISON                      BIT_12
#define AFF_PROTECT                     BIT_13
#define AFF_MEDITATE                    BIT_14
#define AFF_SNEAK                       BIT_15
#define AFF_HIDE                        BIT_16
#define AFF_SLEEP                       BIT_17
#define AFF_CHARM                       BIT_18
#define AFF_FLYING                      BIT_19
#define AFF_PASS_DOOR                   BIT_20
#define AFF_DETECT_TRAPS                BIT_21
#define AFF_BATTLE_AURA                 BIT_22
#define AFF_DETECT_SNEAK                BIT_23
#define AFF_GLOBE                       BIT_24
#define AFF_DETER                       BIT_25
#define AFF_SWIM                        BIT_26
#define AFF_PRAYER_PLAGUE               BIT_27
#define AFF_NON_CORPOREAL               BIT_28  /* Mist Walk, Astral Sidestep, Fly form */
#define AFF_DETECT_CURSE                BIT_29
#define AFF_DETECT_GOOD                 BIT_30	/* last */

/* forms - Brutus */
#define FORM_NORMAL                     0
#define FORM_CHAMELEON                  1
#define FORM_HAWK                       2
#define FORM_CAT                        3
#define FORM_SNAKE                      4
#define FORM_SCORPION                   5
#define FORM_SPIDER                     6
#define FORM_BEAR                       7
#define FORM_TIGER                      8

/* level 30 sft */
#define FORM_GHOST                      9
#define FORM_HYDRA                      10
#define FORM_PHOENIX                    11
#define FORM_DEMON                      12
#define FORM_DRAGON                     13

#define FORM_FLY                        17
#define FORM_GRIFFIN                    18
#define FORM_WOLF                       19
#define FORM_DIREWOLF                   14
#define FORM_NONE                       MAX_FORM + 1
#define FORM_ALL                        MAX_FORM + 2

/* form weapons */
#define OBJ_HAWK_TALONS            50
#define OBJ_SCORPION_STING         51
#define OBJ_SPIDER_MANDIBLES       52
#define OBJ_BEAR_CLAWS             53
#define OBJ_TIGER_FANGS            54
#define OBJ_TIGER_CLAWS            55
#define OBJ_DRAGON_FANGS           56
#define OBJ_DRAGON_CLAWS           57
#define OBJ_HYDRA_TEETH            58
#define OBJ_SNAKE_BITE             59
#define OBJ_PHOENIX_BEAK           500

/* this will be for the quest mobs */
#define QUEST_JUN1                              1
#define QUEST_JUN2                              2
#define QUEST_JUN3                              4
#define QUEST_MED1                              8
#define QUEST_MED2                             16
#define QUEST_MED3                             32
#define QUEST_SEN1                             64
#define QUEST_SEN2                            128
#define QUEST_SEN3                            256


/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL                             0
#define SEX_MALE                                1
#define SEX_FEMALE                              2


/*
 * Money is now defined
 */
#define COINS_ADD                       0       /* This is for function coins_to_char */
#define COINS_REPLACE                   1       /* This is for function coins_to_char */
#define COIN_PLAT                    1000
#define COIN_GOLD                     100
#define COIN_SILVER                    10
#define COIN_COPPER                     1

/* 
 * Smelted MAterials - Brutus
 */
#define SMELTED_STEEL                   1
#define SMELTED_TITANIUM                2
#define SMELTED_ADAMANTITE              3
#define SMELTED_ELECTRUM                4
#define SMELTED_STARMETAL               5

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE                  2
#define OBJ_VNUM_MONEY_SOME                 3

#define OBJ_VNUM_CORPSE_NPC                10
#define OBJ_VNUM_CORPSE_PC                 11
#define OBJ_VNUM_SEVERED_HEAD              12
#define OBJ_VNUM_TORN_HEART                13
#define OBJ_VNUM_SLICED_ARM                14
#define OBJ_VNUM_SLICED_LEG                15

#define OBJ_VNUM_MUSHROOM                  20
#define OBJ_VNUM_LIGHT_BALL                21
#define OBJ_VNUM_SPRING                    22
#define OBJ_VNUM_TURRET                    40
#define OBJ_VNUM_DART                      41
#define OBJ_VNUM_PORTAL                    26
#define OBJ_VNUM_LIGHT_BALL_CRAFT          64

#define OBJ_VNUM_SCHOOL_MACE             3700
#define OBJ_VNUM_SCHOOL_DAGGER           3701
#define OBJ_VNUM_SCHOOL_SWORD            3702
#define OBJ_VNUM_SCHOOL_VEST             3703
#define OBJ_VNUM_SCHOOL_SHIELD           3704
#define OBJ_VNUM_SCHOOL_BANNER           3716
#define OBJ_VNUM_SCHOOL_CLUB             3717
#define OBJ_VNUM_SCHOOL_NULL                2
#define OBJ_VNUM_POUCH                   3370
#define OBJ_VNUM_SCHOOL_BOW              3722

#define ITEM_VNUM_HERB                    100
#define ITEM_VNUM_WIZBREW_VIAL             99


/*
 * Traps
 */
#define TRAP_DAM_SLEEP          -1
#define TRAP_DAM_TELEPORT        0
#define TRAP_DAM_FIRE            1
#define TRAP_DAM_COLD            2
#define TRAP_DAM_ACID            3
#define TRAP_DAM_ENERGY          4
#define TRAP_DAM_BLUNT           5
#define TRAP_DAM_PIERCE          6
#define TRAP_DAM_SLASH           7
#define TRAP_DAM_POISON          8

#define TRAP_EFF_MOVE           BIT_0   /* trigger on movement */
#define TRAP_EFF_OBJECT         BIT_1   /* trigger on get or put */
#define TRAP_EFF_ROOM           BIT_2   /* affect all in room */
#define TRAP_EFF_NORTH          BIT_3   /* movement in this direction */
#define TRAP_EFF_EAST           BIT_4
#define TRAP_EFF_SOUTH          BIT_5
#define TRAP_EFF_WEST           BIT_6
#define TRAP_EFF_UP             BIT_7
#define TRAP_EFF_DOWN           BIT_8
#define TRAP_EFF_OPEN           BIT_9   /* trigger on open */


/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT                               1
#define ITEM_SCROLL                              2
#define ITEM_WAND                                3
#define ITEM_STAFF                               4
#define ITEM_WEAPON                              5
#define ITEM_TREASURE                            8
#define ITEM_ARMOR                               9
#define ITEM_POTION                             10
#define ITEM_FURNITURE                          12
#define ITEM_TRASH                              13
#define ITEM_CONTAINER                          15
#define ITEM_DRINK_CON                          17
#define ITEM_KEY                                18
#define ITEM_FOOD                               19
#define ITEM_MONEY                              20
#define ITEM_BOAT                               22
#define ITEM_CORPSE_NPC                         23
#define ITEM_CORPSE_PC                          24
#define ITEM_FOUNTAIN                           25
#define ITEM_PILL                               26
#define ITEM_CLIMBING_EQ                        27
#define ITEM_PAINT                              28
#define ITEM_MOB                                29
#define ITEM_ANVIL                              30
#define ITEM_AUCTION_TICKET                     31
#define ITEM_CLAN_OBJECT                        32
#define ITEM_PORTAL                             33
#define ITEM_POISON_POWDER                      34
#define ITEM_LOCK_PICK                          35
#define ITEM_INSTRUMENT                         36
#define ITEM_ARMOURERS_HAMMER                   37
#define ITEM_MITHRIL                            38
#define ITEM_WHETSTONE                          39
#define ITEM_CRAFT                              40 /* Increase bonus to crafting that takes place in ROOM_CRAFT */
#define ITEM_SPELLCRAFT                         41 /* Increase bonus to spellcrafting that takes place in ROOM_SPELLCRAFT */
#define ITEM_TURRET_MODULE                      42

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW                       BIT_0
#define ITEM_HUM                        BIT_1
#define ITEM_EGO                        BIT_2   /* Item has special effects and powers; was ITEM_DARK */
#define ITEM_ANTI_RANGER                BIT_3   /* Was ITEM_LOCK */
#define ITEM_EVIL                       BIT_4
#define ITEM_INVIS                      BIT_5
#define ITEM_MAGIC                      BIT_6
#define ITEM_NODROP                     BIT_7
#define ITEM_BLESS                      BIT_8
#define ITEM_ANTI_GOOD                  BIT_9
#define ITEM_ANTI_EVIL                  BIT_10
#define ITEM_ANTI_NEUTRAL               BIT_11
#define ITEM_NOREMOVE                   BIT_12
#define ITEM_INVENTORY                  BIT_13
#define ITEM_POISONED                   BIT_14
#define ITEM_ANTI_MAGE                  BIT_15
#define ITEM_ANTI_CLERIC                BIT_16
#define ITEM_ANTI_THIEF                 BIT_17
#define ITEM_ANTI_WARRIOR               BIT_18
#define ITEM_ANTI_PSIONIC               BIT_19
#define ITEM_VORPAL                     BIT_20
#define ITEM_TRAP                       BIT_21
#define ITEM_DONATED                    BIT_22
#define ITEM_BLADE_THIRST               BIT_23
#define ITEM_SHARP                      BIT_24
#define ITEM_FORGED                     BIT_25
#define ITEM_BODY_PART                  BIT_26
#define ITEM_LANCE                      BIT_27
#define ITEM_ANTI_BRAWLER               BIT_28
#define ITEM_ANTI_SHAPE_SHIFTER         BIT_29
#define ITEM_BOW                        BIT_30  /* last */


/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE                       BIT_0
#define ITEM_WEAR_FINGER                BIT_1
#define ITEM_WEAR_NECK                  BIT_2
#define ITEM_WEAR_BODY                  BIT_3
#define ITEM_WEAR_HEAD                  BIT_4
#define ITEM_WEAR_LEGS                  BIT_5
#define ITEM_WEAR_FEET                  BIT_6
#define ITEM_WEAR_HANDS                 BIT_7
#define ITEM_WEAR_ARMS                  BIT_8
#define ITEM_WEAR_SHIELD                BIT_9
#define ITEM_WEAR_ABOUT                 BIT_10
#define ITEM_WEAR_WAIST                 BIT_11
#define ITEM_WEAR_WRIST                 BIT_12
#define ITEM_WIELD                      BIT_13
#define ITEM_HOLD                       BIT_14
#define ITEM_FLOAT                      BIT_15
#define ITEM_WEAR_POUCH                 BIT_16
#define ITEM_RANGED_WEAPON              BIT_17
#define ITEM_LAST_WEAR_BIT              BIT_17

/* Size bits are not really wear bits, so don't include under last_wear */
#define ITEM_SIZE_ALL                   BIT_27
#define ITEM_SIZE_SMALL                 BIT_28
#define ITEM_SIZE_MEDIUM                BIT_29
#define ITEM_SIZE_LARGE                 BIT_30

#define CHAR_SIZE_SMALL                 0
#define CHAR_SIZE_MEDIUM                1
#define CHAR_SIZE_LARGE                 2

#define BIT_LIGHT                       0
#define BIT_TAKE                        1
#define BIT_WEAR_FINGER                 2
#define BIT_WEAR_NECK                   3
#define BIT_WEAR_BODY                   4
#define BIT_WEAR_HEAD                   5
#define BIT_WEAR_LEGS                   6
#define BIT_WEAR_FEET                   7
#define BIT_WEAR_HANDS                  8
#define BIT_WEAR_ARMS                   9
#define BIT_WEAR_SHIELD                 10
#define BIT_WEAR_ABOUT                  11
#define BIT_WEAR_WAIST                  12
#define BIT_WEAR_WRIST                  13
#define BIT_WIELD                       14
#define BIT_HOLD                        15
#define BIT_FLOAT                       16
#define BIT_WEAR_POUCH                  17
#define BIT_RANGED_WEAPON               18
#define ITEM_BIT_COUNT                  19


/*
 * Ego item specials; Gezhp 2001
 * Note: not all ego effects completed :P
 */
#define EGO_ITEM_BLOODLUST              BIT_0   /* Wearer may not flee; wimpy ignored; attacks creatures at random */
#define EGO_ITEM_SOUL_STEALER           BIT_1   /* Weapon: drains hits, mana and move after some successful hits */
#define EGO_ITEM_FIREBRAND              BIT_2   /* Weapon: inflicts fire damage after some successful hits */
#define EGO_ITEM_BATTLE_TERROR          BIT_3   /* Wearer may automatically try to flee after being hit; may become terrified and unable to act in combat */
#define EGO_ITEM_IMBUED                 BIT_4   /* Used for Imbue */                                                 
#define EGO_ITEM_BALANCED               BIT_5   /* counterbalanced weapon */
#define EGO_ITEM_TURRET                 BIT_6
#define EGO_ITEM_TURRET_MODULE          BIT_7

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE                               0
#define APPLY_STR                                1
#define APPLY_DEX                                2
#define APPLY_INT                                3
#define APPLY_WIS                                4
#define APPLY_CON                                5
#define APPLY_SEX                                6
#define APPLY_CLASS                              7
#define APPLY_LEVEL                              8
#define APPLY_AGE                                9
#define APPLY_HEIGHT                            10
#define APPLY_WEIGHT                            11
#define APPLY_MANA                              12
#define APPLY_HIT                               13
#define APPLY_MOVE                              14
#define APPLY_GOLD                              15
#define APPLY_EXP                               16
#define APPLY_AC                                17
#define APPLY_HITROLL                           18
#define APPLY_DAMROLL                           19
#define APPLY_SAVING_PARA                       20
#define APPLY_SAVING_ROD                        21
#define APPLY_SAVING_PETRI                      22
#define APPLY_SAVING_BREATH                     23
#define APPLY_SAVING_SPELL                      24
#define APPLY_SANCTUARY                         25
#define APPLY_SNEAK                             26
#define APPLY_FLY                               27
#define APPLY_INVIS                             28
#define APPLY_DETECT_INVIS                      29
#define APPLY_DETECT_HIDDEN                     30
#define APPLY_FLAMING                           31
#define APPLY_PROTECT                           32
#define APPLY_PASS_DOOR                         33
#define APPLY_GLOBE                             34
#define APPLY_DRAGON_AURA                       35
#define APPLY_RESIST_HEAT                       36
#define APPLY_RESIST_COLD                       37
#define APPLY_RESIST_LIGHTNING                  38
#define APPLY_RESIST_ACID                       39
#define APPLY_BREATHE_WATER                     40      /* So we can sell aqualungs -- Owl 11/4/22 */
#define APPLY_BALANCE                    41      /* for Balance Skill - Brutus Jul 2022 */

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE                          1
#define CONT_PICKPROOF                          2
#define CONT_CLOSED                             4
#define CONT_LOCKED                             8


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO                         2
#define ROOM_VNUM_TEMPLE                        3001
#define ROOM_VNUM_ALTAR                         3054
#define ROOM_VNUM_SCHOOL                        3700
#define ROOM_VNUM_GRAVEYARD_A                   427
#define ROOM_VNUM_PURGATORY_A                   401
#define ROOM_VNUM_ARENA                         14200

#define OBJECT_VNUM_PURGATORY_PORTAL            404
#define DEFAULT_RECALL                          ROOM_VNUM_TEMPLE

/* used in area files for level limits */
#define ROOM_LABEL_PLUS                         -1
#define ROOM_LABEL_ALL                          -2
#define ROOM_LABEL_NONE                         -3
#define ROOM_LABEL_CLAN                         -4
#define ROOM_LABEL_NOTELEPORT                   -5

#define ROOM_VNUM_DONATION        3216
#define ROOM_VNUM_DONATION_1      3208
#define ROOM_VNUM_DONATION_2      3209
#define ROOM_VNUM_DONATION_3      3210
#define ROOM_VNUM_DONATION_4      3211
#define ROOM_VNUM_DONATION_5      3212


/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK                       BIT_0
#define ROOM_NO_MOB                     BIT_2
#define ROOM_INDOORS                    BIT_3
#define ROOM_CRAFT                      BIT_7 /* Improve outcomes of crafting skills and spells */
#define ROOM_SPELLCRAFT                 BIT_8 /* --Owl 2/5/22 */
#define ROOM_PRIVATE                    BIT_9
#define ROOM_SAFE                       BIT_10
#define ROOM_SOLITARY                   BIT_11
#define ROOM_PET_SHOP                   BIT_12
#define ROOM_NO_RECALL                  BIT_13
#define ROOM_CONE_OF_SILENCE            BIT_14
#define ROOM_PLAYER_KILLER              BIT_15
#define ROOM_HEALING                    BIT_16
#define ROOM_FREEZING                   BIT_17
#define ROOM_BURNING                    BIT_18
#define ROOM_NO_MOUNT                   BIT_19


/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH               0
#define DIR_EAST                1
#define DIR_SOUTH               2
#define DIR_WEST                3
#define DIR_UP                  4
#define DIR_DOWN                5
#define MAX_DIR                 6

extern DIR_DATA directions [ MAX_DIR ];


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR               1
#define EX_CLOSED               2
#define EX_LOCKED               4
#define EX_BASHED               8
#define EX_BASHPROOF            16
#define EX_PICKPROOF            32
#define EX_PASSPROOF            64
#define EX_WALL                 128
#define EX_SECRET               256


/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE                     0
#define SECT_CITY                       1
#define SECT_FIELD                      2
#define SECT_FOREST                     3
#define SECT_HILLS                      4
#define SECT_MOUNTAIN                   5
#define SECT_WATER_SWIM                 6
#define SECT_WATER_NOSWIM               7
#define SECT_UNDERWATER                 8
#define SECT_AIR                        9
#define SECT_DESERT                     10
#define SECT_MAX                        11


/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE                       -1
#define WEAR_LIGHT                       0
#define WEAR_FINGER_L                    1
#define WEAR_FINGER_R                    2
#define WEAR_NECK_1                      3
#define WEAR_NECK_2                      4
#define WEAR_BODY                        5
#define WEAR_HEAD                        6
#define WEAR_LEGS                        7
#define WEAR_FEET                        8
#define WEAR_HANDS                       9
#define WEAR_ARMS                       10
#define WEAR_SHIELD                     11
#define WEAR_ABOUT                      12
#define WEAR_WAIST                      13
#define WEAR_WRIST_L                    14
#define WEAR_WRIST_R                    15
#define WEAR_WIELD                      16
#define WEAR_HOLD                       17
#define WEAR_DUAL                       18
#define WEAR_FLOAT                      19
#define WEAR_POUCH                      20
#define WEAR_RANGED_WEAPON              21


/*
 * Area flags; set in header of each area.
 * Flags set in #AREA_SPECIAL header within .are file.
 * Gezhp 2001
 */
#define AREA_FLAG_SCHOOL        BIT_0   /* Mud School */
#define AREA_FLAG_NO_QUEST      BIT_1   /* No mobs defined in area used for quests */
#define AREA_FLAG_HIDDEN        BIT_2   /* Area not seen on 'AREA' output */
#define AREA_FLAG_SAFE          BIT_3   /* All rooms SAFE */
#define AREA_FLAG_NO_TELEPORT   BIT_4   /* No rooms in area can be teleported into */


/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/


/*
 * Conditions.
 */
#define COND_DRUNK                      0
#define COND_FULL                       1
#define COND_THIRST                     2


/*
 * Positions.
 */
#define POS_DEAD                        0
#define POS_MORTAL                      1
#define POS_INCAP                       2
#define POS_STUNNED                     3
#define POS_SLEEPING                    4
#define POS_RESTING                     5
#define POS_FIGHTING                    6
#define POS_STANDING                    7


/*
 * ACT bits for players.
 */
#define PLR_IS_NPC                      BIT_0   /* Don't EVER set. */
#define PLR_BOUGHT_PET                  BIT_1
#define PLR_QUESTOR                     BIT_2
#define PLR_AUTOEXIT                    BIT_3
#define PLR_AUTOLOOT                    BIT_4
#define PLR_AUTOSAC                     BIT_5
#define PLR_BLANK                       BIT_6
#define PLR_BRIEF                       BIT_7
#define PLR_LEADER                      BIT_8
#define PLR_COMBINE                     BIT_9
#define PLR_PROMPT                      BIT_10
#define PLR_TELNET_GA                   BIT_11
#define PLR_HOLYLIGHT                   BIT_12
#define PLR_WIZINVIS                    BIT_13
#define PLR_FALLING                     BIT_14 /* Owl 20/3/22, 'gravity' code */
#define PLR_SILENCE                     BIT_15
#define PLR_NO_EMOTE                    BIT_16
#define PLR_NO_TELL                     BIT_18
#define PLR_LOG                         BIT_19
#define PLR_DENY                        BIT_20
#define PLR_FREEZE                      BIT_21
#define PLR_GUIDE                       BIT_22  /* was PLR_THIEF */
#define PLR_AUTOLEVEL                   BIT_23  /* Owl 16/3/22 */
#define PLR_ANSI                        BIT_24
#define PLR_VT100                       BIT_25
#define PLR_AFK                         BIT_26
#define PLR_AUTOWIELD                   BIT_27
#define PLR_AUTOCOIN                    BIT_28  /* Automatically loot coins; Gez */


/*
 * Status bits
 */
#define PLR_KILLER                       1
#define PLR_THIEF                        2
#define PLR_RONIN                        4
#define PLR_HUNTED                       8
#define PLR_LEFT_CLAN                   16


/*
 * Channel bits.
 */
#define CHANNEL_AUCTION         BIT_0
#define CHANNEL_CHAT            BIT_1
#define CHANNEL_SERVER          BIT_2
#define CHANNEL_IMMTALK         BIT_3
#define CHANNEL_MUSIC           BIT_4
#define CHANNEL_QUESTION        BIT_5
#define CHANNEL_SHOUT           BIT_6
#define CHANNEL_YELL            BIT_7
#define CHANNEL_INFO            BIT_8
#define CHANNEL_CLAN            BIT_9
#define CHANNEL_DIRTALK         BIT_10
#define CHANNEL_ARENA           BIT_11
#define CHANNEL_NEWBIE          BIT_12


/*
 * Wear restrictions
 */
struct wear_struct
{
        int can_wear [ MAX_ITEM_TYPE + 1 ];
};

struct loc_wear_struct
{
        int can_wear [ ITEM_BIT_COUNT ];
};

const int eq_slot_to_wear_bit [ MAX_WEAR ];


/* learned struct for practicers - geoff */
struct  learned_data
{
        int learned [ MAX_SKILL ];
};


/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */

struct  mob_index_data
{
        MOB_INDEX_DATA *        next;
        SPEC_FUN *              spec_fun;
        SHOP_DATA *             pShop;
        MPROG_DATA *            mobprogs;
        LEARNED_DATA *          skills;         /* used by practicers only */

        char *          player_name;
        char *          short_descr;
        char *          long_descr;
        char *          description;
        int             vnum;
        int             count;
        int             killed;
        int             sex;
        int             level;
        int             act;
        int             affected_by;
        int             alignment;
        int             progtypes;
        int             body_form;
        int             spec_fun_exp_modifier;
};


/*
 * One character (PC or NPC).
 */
struct char_data
{
        CHAR_DATA *                     next;
        CHAR_DATA *                     next_in_room;
        CHAR_DATA *                     master;
        CHAR_DATA *                     leader;
        CHAR_DATA *                     fighting;
        CHAR_DATA *                     hunting;
        CHAR_DATA *                     reply;
        CHAR_DATA *                     mount;
        CHAR_DATA *                     rider;
        SPEC_FUN *                      spec_fun;
        MOB_INDEX_DATA *                pIndexData;
        DESCRIPTOR_DATA *               desc;
        AFFECT_DATA *                   affected;
        OBJ_DATA *                      carrying;
        ROOM_INDEX_DATA *               in_room;
        ROOM_INDEX_DATA *               was_in_room;
        PC_DATA *                       pcdata;
        MPROG_ACT_LIST *                mpact;

        char *          name;
        char *          short_descr;
        char *          long_descr;
        char *          description;
        char *          prompt;
        int             sex;
        int             class;
        int             sub_class;
        int             race;
        int             clan;
        int             clan_level;
        int             level;
        int             trust;
        bool            wizbit;
        int             played;
        time_t          logon;
        time_t          save_time;
        time_t          last_note;
        int             timer;
        int             wait;
        int             hit;
        int             max_hit;
        int             aggro_dam;
        int             mana;
        int             max_mana;
        int             move;
        int             max_move;
        int             rage;                           /* rage and blood */
        int             max_rage;
        int             plat;
        int             gold;
        int             silver;
        int             copper;
        int             coin_weight;
        int             smelted_steel;
        int             smelted_titanium;
        int             smelted_adamantite;
        int             smelted_electrum;
        int             smelted_starmetal;
        int             exp;
        int             act;
        int             status;
        int             affected_by;
        int             position;
        int             carry_weight;
        int             carry_number;
        int             saving_throw;
        int             alignment;
        int             hitroll;
        int             damroll;
        int             armor;
        int             wimpy;
        int             deaf;
        int             silent_mode;
        int             colors [ MAX_COLORS ];
        int             mpactnum;
        bool            deleted;
        int             form;
        int             gag;
        int             backstab;
        int             edrain;
        int             body_form;
        int             tournament_team;        /* mobs can use this too */
        int             exp_modifier;
        /*
        *  Does the variable you're about to add belong here or in 'pcdata'?
        */
};


/* FOR mob progs */
struct mob_prog_act_list
{
        MPROG_ACT_LIST *        next;
        char *                  buf;
        CHAR_DATA *             ch;
        OBJ_DATA *              obj;
        void *                  vo;
};

struct mob_prog_data
{
        MPROG_DATA *            next;
        int                     type;
        char *                  arglist;
        char *                  comlist;
};

bool    MOBtrigger;
#define ERROR_PROG                -1
#define IN_FILE_PROG               0
#define ACT_PROG                   1
#define SPEECH_PROG                2
#define RAND_PROG                  4
#define FIGHT_PROG                 8
#define DEATH_PROG                16
#define HITPRCNT_PROG             32
#define ENTRY_PROG                64
#define GREET_PROG               128
#define ALL_GREET_PROG           256
#define GIVE_PROG                512
#define BRIBE_PROG              1024


/*
 * Data which only PCs have.
 */

struct  pc_data
{
        PC_DATA *                       next;
        char *                          pwd;
        char *                          bamfin;
        char *                          bamfout;
        char *                          title;
        char *                          tailing;
        CHAR_DATA *                     group_leader;
        int                             perm_str;
        int                             perm_int;
        int                             perm_wis;
        int                             perm_dex;
        int                             perm_con;
        int                             mod_str;
        int                             mod_int;
        int                             mod_wis;
        int                             mod_dex;
        int                             mod_con;
        int                             stat_train;
        int                             condition [ 3 ];
        int                             pagelen;
        int                             learned [ MAX_SKILL ];
        bool                            switched;
        bool                            confirm_delete;
        bool                            confirm_leave;
        bool                            choose_subclass;
        int                             spell_attacks;
        int                             dam_bonus;
        int                             fame;
        int                             pkscore;                        /* Used to track pkill points for individuals */
        BOARD_DATA *                    board;                          /* The current board */
        time_t                          last_note [ MAX_BOARD ];        /* last note for the boards */
        NOTE_DATA *                     in_progress;
        int                             current_recall;
        int                             recall_points [ MAX_RECALL_POINTS ];
        OBJ_DATA *                      morph_list [ MAX_WEAR ];
        int                             blink;
        int                             deity_patron;
        int                             deity_favour [ NUMBER_DEITIES ];
        int                             deity_type_timer [ DEITY_NUMBER_TYPES ];
        int                             deity_personality_timer [ DEITY_NUMBER_PERSONALITIES ];
        int                             deity_timer;
        int                             deity_flags;
        int                             suicide_code;
        int                             kills;
        int                             killed;
        int                             pdeaths;
        int                             pkills;
        int                             str_prac;
        int                             int_prac;
        int                             air_supply;
        int                             level_xp_loss;
        CHAR_DATA *                     questgiver;
        int                             questpoints;
        int                             nextquest;
        int                             countdown;
        int                             questobj;
        int                             questmob;
        int                             totalqp;
        int                             bank;
        int                             bounty;
        int                             allow_look;
        time_t                          review_stamp;
        int                             pattern;
        int                             soar;
        int                             group_support_bonus;
};


/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX         16

struct liq_type
{
        char    *liq_name;
        char    *liq_color;
        int      liq_effect [ 3 ];
};


/* Raw MAterials - name, desc, max, weight, cost, spare  - Brutus Jul 2022
#define RAW_MATS_MAX        5

struct raw_mats_data
{
        char    *mat_name;
        char    *mat_desc;
        int     mat_properties [ 4 ];
};
*/

/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
        EXTRA_DESCR_DATA        *next;  /* Next in list                     */

        char    *keyword;               /* Keyword in look/examine          */
        char    *description;           /* What to see                      */
        bool    deleted;
};


/*
 * Prototype for an object.
 */
struct obj_index_data
{
        OBJ_INDEX_DATA *        next;
        EXTRA_DESCR_DATA *      extra_descr;
        AFFECT_DATA *           affected;

        char *          name;
        char *          short_descr;
        char *          description;
        int             vnum;
        int             item_type;
        int             extra_flags;
        int             wear_flags;
        int             count;
        int             weight;
        int             level;
        int             cost;
        int             value   [ 4 ];
        int             trap_eff;
        int             trap_dam;
        int             trap_charge;
        int             ego_flags;
};

/*
 * Timer-remaining ticks at which an object either becomes visibly DAMAGED or appears nearly destroyed (ALERT)
 */
#define TIMER_DAMAGED        20
#define TIMER_ALERT          10

/*
 * One object.
 */
struct obj_data
{
        OBJ_DATA *                      next;
        OBJ_DATA *                      next_content;
        OBJ_DATA *                      contains;
        OBJ_DATA *                      in_obj;
        CHAR_DATA *                     carried_by;
        EXTRA_DESCR_DATA *              extra_descr;
        AFFECT_DATA *                   affected;
        OBJ_INDEX_DATA *                pIndexData;
        ROOM_INDEX_DATA *               in_room;

        char *          name;
        char *          short_descr;
        char *          description;
        int             item_type;
        int             extra_flags;
        int             wear_flags;
        int             wear_loc;
        int             weight;
        int             cost;
        int             level;
        int             timer;
        int             timermax;
        int             value [ 4 ];
        bool            deleted;
        int             trap_eff;
        int             trap_dam;
        int             trap_charge;
        char            owner [ 32 ];
        int             ego_flags;
};


/*
 * Exit data.
 */
struct exit_data
{
        ROOM_INDEX_DATA *       to_room;

        int     vnum;
        int     exit_info;
        int     key;
        char *  keyword;
        char *  description;
};


/*
 * Area-reset definition.
 */
struct reset_data
{
        RESET_DATA *    next;

        char    command;
        int     arg0;   /* Gezhp 2000 */
        int     arg1;
        int     arg2;
        int     arg3;
};


/*
 * Area definition.
 */
struct area_data
{
        AREA_DATA *             next;
        AREA_DATA *             next_sort;
        RESET_DATA *            reset_first;
        RESET_DATA *            reset_last;

        char *          author;
        char *          name;
        int             low_level;
        int             high_level;
        int             low_enforced;
        int             high_enforced;

        int             low_r_vnum;
        int             hi_r_vnum;
        int             low_o_vnum;
        int             hi_o_vnum;
        int             low_m_vnum;
        int             hi_m_vnum;

        int             recall;
        int             age;
        int             nplayer;

        int             container_reset_timer;  /* Poor little cpu; Gezhp */
        int             area_flags;
        int             exp_modifier;
};


/*
 * Room type.
 */
struct room_index_data
{
        ROOM_INDEX_DATA *       next;
        CHAR_DATA *             people;
        OBJ_DATA *              contents;
        EXTRA_DESCR_DATA *      extra_descr;
        AREA_DATA *             area;
        EXIT_DATA *             exit [ 6 ];

        char *  name;
        char *  description;
        int     vnum;
        int     room_flags;
        int     light;
        int     sector_type;
};


/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED                    -1
#define TYPE_HIT                        1000


/*
 *  Target types.
 */
#define TAR_IGNORE                      0
#define TAR_CHAR_OFFENSIVE              1
#define TAR_CHAR_DEFENSIVE              2
#define TAR_CHAR_SELF                   3
#define TAR_OBJ_INV                     4
#define TAR_OBJ_ROOM                    5
#define TAR_CHAR_OFFENSIVE_SINGLE       6


/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
        char *          name;                           /* Name of skill */
        int *           pgsn;                           /* Pointer to associated gsn */
        int             prac_type;                      /* for praccing */
        int             target;                         /* Legal targets */
        int             minimum_position;               /* Position for caster / user */
        SPELL_FUN *     spell_fun;                      /* Spell pointer (for spells) */
        int             min_mana;                       /* Minimum mana used */
        int             beats;                          /* Waiting time after use */
        char *          noun_damage;                    /* Damage message */
        char *          msg_off;                        /* Wear off message */
};


/* class basic gsn's */
extern int gsn_mage_base;
extern int gsn_cleric_base;
extern int gsn_thief_base;
extern int gsn_warrior_base;
extern int gsn_psionic_base;
extern int gsn_shifter_base;
extern int gsn_brawler_base;
extern int gsn_ranger_base;
extern int gsn_smithy_base;
extern int gsn_engineer_base;
extern int gsn_alchemist_base;
extern int gsn_necro_base;
extern int gsn_warlock_base;
extern int gsn_templar_base;
extern int gsn_druid_base;
extern int gsn_ninja_base;
extern int gsn_bounty_base;
extern int gsn_thug_base;
extern int gsn_knight_base;
extern int gsn_satanist_base;
extern int gsn_witch_base;
extern int gsn_werewolf_base;
extern int gsn_vampire_base;
extern int gsn_monk_base;
extern int gsn_martist_base;
extern int gsn_barbarian_base;
extern int gsn_bard_base;
extern int gsn_teacher_base;


/*
 * These are skill_lookup return values for common skills and spells.
 */
extern int gsn_flight;
extern int gsn_hunt;
extern int gsn_dirt;
extern int gsn_decapitate;
extern int gsn_meditate;
extern int gsn_assassinate;
extern int gsn_backstab;
extern int gsn_double_backstab;
extern int gsn_circle;
extern int gsn_second_circle;
extern int gsn_climb;
extern int gsn_coil;
extern int gsn_blink;
extern int gsn_dodge;
extern int gsn_acrobatics;
extern int gsn_hide;
extern int gsn_peek;
extern int gsn_pick_lock;
extern int gsn_sneak;
extern int gsn_stalk;
extern int gsn_tail;
extern int gsn_steal;
extern int gsn_disable;
extern int gsn_disarm;
extern int gsn_poison_weapon;
extern int gsn_bladethirst;
extern int gsn_dual;
extern int gsn_stun;
extern int gsn_transfix;
extern int gsn_trap;
extern int gsn_trip;
extern int gsn_advanced_consider;
extern int gsn_bash;
extern int gsn_berserk;
extern int gsn_enhanced_hit;
extern int gsn_enhanced_damage;
extern int gsn_fast_healing;
extern int gsn_headbutt;
extern int gsn_second_headbutt;
extern int gsn_kiai;
extern int gsn_kick;
extern int gsn_parry;
extern int gsn_dual_parry;
extern int gsn_pre_empt;
extern int gsn_rescue;
extern int gsn_second_attack;
extern int gsn_third_attack;
extern int gsn_fourth_attack;
extern int gsn_feint;
extern int gsn_find_traps;              /* for finding traps - Brutus */
extern int gsn_second_spell;
extern int gsn_third_spell;
extern int gsn_fourth_spell;
extern int gsn_animate_weapon;          /* magical disarm -  Istari */
extern int gsn_dark_ritual;
extern int gsn_transport;               /* transport object - Istari */
extern int gsn_shield_block;
extern int gsn_smash;
extern int gsn_unarmed_combat;
extern int gsn_warcry;
extern int gsn_whirlwind;
extern int gsn_grip;
extern int gsn_joust;
extern int gsn_sharpen;
extern int gsn_forge;
extern int gsn_risposte;
extern int gsn_destrier;
extern int gsn_gouge;
extern int gsn_choke;
extern int gsn_grapple;
extern int gsn_flying_headbutt;
extern int gsn_grab;
extern int gsn_battle_aura;
extern int gsn_punch;
extern int gsn_second_punch;
extern int gsn_push;
extern int gsn_focus;
extern int gsn_break_wrist;
extern int gsn_snap_neck;
extern int gsn_pugalism;
extern int gsn_combo;
extern int gsn_combo2;
extern int gsn_combo3;
extern int gsn_combo4;
extern int gsn_atemi;
extern int gsn_kansetsu;
extern int gsn_tetsui;
extern int gsn_shuto;
extern int gsn_yokogeri;
extern int gsn_mawasigeri;
extern int gsn_fear;
extern int gsn_deter;
extern int gsn_mist_walk;
extern int gsn_astral_sidestep;
extern int gsn_gaias_warning;
extern int gsn_resist_magic;
extern int gsn_resist_toxin;
extern int gsn_summon_avatar;
extern int gsn_feed;
extern int gsn_suck;
extern int gsn_lunge;
extern int gsn_aura_of_fear;
extern int gsn_double_lunge;
extern int gsn_open_seal;
extern int gsn_rage;
extern int gsn_howl;
extern int gsn_mount;
extern int gsn_dismount;
extern int gsn_blindness;
extern int gsn_brew;
extern int gsn_charm_person;
extern int gsn_curse;
extern int gsn_detect_hidden;
extern int gsn_detect_invis;
extern int gsn_detect_sneak;
extern int gsn_entrapment;
extern int gsn_fireshield;
extern int gsn_fly;
extern int gsn_infravision;
extern int gsn_intimidate;
extern int gsn_extort;
extern int gsn_invis;
extern int gsn_mass_invis;
extern int gsn_poison;
extern int gsn_paralysis;
extern int gsn_scribe;
extern int gsn_sleep;
extern int gsn_sunray;
extern int gsn_addfame;
extern int gsn_addqp;
extern int gsn_advance;
extern int gsn_allow;
extern int gsn_at;
extern int gsn_bamfin;
extern int gsn_bamfout;
extern int gsn_ban;
extern int gsn_cando;
extern int gsn_deny;
extern int gsn_disconnect;
extern int gsn_echo;
extern int gsn_force;
extern int gsn_freeze;
extern int gsn_goto;
extern int gsn_guide;
extern int gsn_holylight;
extern int gsn_immtalk;
extern int gsn_dirtalk;
extern int gsn_killsocket;
extern int gsn_leader;                  /* for the clan leader flag */
extern int gsn_log;
extern int gsn_memory;
extern int gsn_mfind;
extern int gsn_mload;
extern int gsn_oclanitem;
extern int gsn_mset;
extern int gsn_mstat;
extern int gsn_mwhere;
extern int gsn_newlock;
extern int gsn_noemote;
extern int gsn_notell;
extern int gsn_numlock;
extern int gsn_ofind;
extern int gsn_oload;
extern int gsn_oset;
extern int gsn_ostat;
extern int gsn_owhere;
extern int gsn_pardon;
extern int gsn_peace;
extern int gsn_purge;
extern int gsn_reboot;
extern int gsn_recho;
extern int gsn_rename;
extern int gsn_reset;
extern int gsn_restore;
extern int gsn_return;
extern int gsn_ronin;
extern int gsn_rset;
extern int gsn_rstat;
extern int gsn_shutdown;
extern int gsn_silence;
extern int gsn_slay;
extern int gsn_slookup;
extern int gsn_snoop;
extern int gsn_sset;
extern int gsn_sstime;
extern int gsn_switch;
extern int gsn_transfer;
extern int gsn_trust;
extern int gsn_users;
extern int gsn_wizbrew;
extern int gsn_wizhelp;
extern int gsn_wizify;
extern int gsn_wizinvis;
extern int gsn_wizlock;
extern int gsn_zones;
extern int gsn_chameleon_power;
extern int gsn_domination;
extern int gsn_heighten;
extern int gsn_shadow_form;

extern int gsn_group_evocation;
extern int gsn_group_dark;
extern int gsn_group_death;
extern int gsn_group_lycanthropy;
extern int gsn_group_vampyre;
extern int gsn_group_breath;
extern int gsn_group_destruction;
extern int gsn_group_majorp;
extern int gsn_group_craft;
extern int gsn_group_divination;
extern int gsn_group_alteration;
extern int gsn_group_summoning;
extern int gsn_group_illusion;
extern int gsn_group_enchant;
extern int gsn_group_conjuration;
extern int gsn_group_protection;
extern int gsn_group_divine;
extern int gsn_group_disease;
extern int gsn_group_harmful;
extern int gsn_group_healing;
extern int gsn_group_advanced_heal;
extern int gsn_group_curative;
extern int gsn_group_mana;
extern int gsn_group_mentald;
extern int gsn_group_matter;
extern int gsn_group_energy;
extern int gsn_group_telepathy;
extern int gsn_group_advtele;
extern int gsn_group_forms;
extern int gsn_group_combat;
extern int gsn_group_stealth;
extern int gsn_group_hunting;
extern int gsn_group_arts;
extern int gsn_group_combos;
extern int gsn_group_riding;
extern int gsn_group_unarmed;
extern int gsn_group_metal;
extern int gsn_group_advcombat;
extern int gsn_group_body;
extern int gsn_group_poison;
extern int gsn_group_armed;
extern int gsn_group_knowledge;
extern int gsn_group_nature;
extern int gsn_group_inner;
extern int gsn_group_thievery;
extern int gsn_group_defense;
extern int gsn_group_pugalism;
extern int gsn_group_morph;
extern int gsn_group_archery;
extern int gsn_group_music;
extern int gsn_group_herb_lore;
extern int gsn_spellcraft;
extern int gsn_group_resistance;
extern int gsn_group_last;
extern int gsn_group_weaponsmith;
extern int gsn_group_armoursmith;
extern int gsn_group_inscription;
extern int gsn_group_alchemy;
extern int gsn_group_turret_tech;
extern int gsn_group_mech_tech;

extern int gsn_form_chameleon;
extern int gsn_form_hawk;
extern int gsn_form_cat;
extern int gsn_form_bat;
extern int gsn_form_snake;
extern int gsn_form_scorpion;
extern int gsn_form_spider;
extern int gsn_form_bear;
extern int gsn_form_tiger;
extern int gsn_form_dragon;
extern int gsn_form_demon;
extern int gsn_form_phoenix;
extern int gsn_form_hydra;
extern int gsn_form_wolf;
extern int gsn_form_direwolf;
extern int gsn_form_elemental_air;
extern int gsn_form_elemental_water;
extern int gsn_form_elemental_fire;
extern int gsn_form_elemental_earth;
extern int gsn_form_fly;
extern int gsn_form_griffin;

extern int gsn_morph;
extern int gsn_spy;
extern int gsn_swim;
extern int gsn_constrict;
extern int gsn_strangle;
extern int gsn_forage;
extern int gsn_bite;
extern int gsn_maul;
extern int gsn_wolfbite;
extern int gsn_ravage;
extern int gsn_venom;
extern int gsn_web;
extern int gsn_breathe;
extern int gsn_dive;
extern int gsn_tailwhip;
extern int gsn_dragon_aura;
extern int gsn_possession;
extern int gsn_demon_flames;
extern int gsn_steal_strength;
extern int gsn_hand_of_lucifer;
extern int gsn_satans_fury;
extern int gsn_steal_soul;
extern int gsn_summon_demon;

extern int gsn_gate;
extern int gsn_levitation;
extern int gsn_locate_object;
extern int gsn_pass_door;
extern int gsn_summon;
extern int gsn_summon_familiar;
extern int gsn_teleport;
extern int gsn_sense_traps;
extern int gsn_word_of_recall;
extern int gsn_cause_light;
extern int gsn_cause_serious;
extern int gsn_cause_critical;
extern int gsn_chill_touch;
extern int gsn_dispel_evil;
extern int gsn_dispel_magic;
extern int gsn_energy_drain;
extern int gsn_harm;
extern int gsn_haste;
extern int gsn_weaken;
extern int gsn_feeblemind;
extern int gsn_spiritwrack;
extern int gsn_wither;
extern int gsn_hex;
extern int gsn_death_field;
extern int gsn_decay;
extern int gsn_disintergrate;
extern int gsn_inflict_pain;
extern int gsn_psychic_drain;
extern int gsn_acid_blast;
extern int gsn_inner_fire;
extern int gsn_synaptic_blast;
extern int gsn_prismatic_spray;
extern int gsn_holy_word;
extern int gsn_unholy_word;
extern int gsn_armor;
extern int gsn_burning_hands;
extern int gsn_dragon_shield;
extern int gsn_call_lightning;
extern int gsn_chain_lighhtning;
extern int gsn_colour_spray;
extern int gsn_continual_light;
extern int gsn_control_weather;
extern int gsn_earthquake;
extern int gsn_fireball;
extern int gsn_flamestrike;
extern int gsn_giant_strength;
extern int gsn_lightning_bolt;
extern int gsn_magic_missile;
extern int gsn_protection;
extern int gsn_sanctuary;
extern int gsn_shield;
extern int gsn_shocking_grasp;
extern int gsn_stone_skin;
extern int gsn_acid_breath;
extern int gsn_fire_breath;
extern int gsn_frost_breath;
extern int gsn_gas_breath;
extern int gsn_lightning_breath;
extern int gsn_general_purpose;
extern int gsn_high_explosive;
extern int gsn_moonray;
extern int gsn_natures_fury;
extern int gsn_dark_globe;
extern int gsn_globe;
extern int gsn_firestorm;
extern int gsn_meteor_storm;
extern int gsn_ultrablast;
extern int gsn_control_flames;
extern int gsn_create_sound;
extern int gsn_detonate;
extern int gsn_ballistic_attack;
extern int gsn_ego_whip;
extern int gsn_energy_containment;
extern int gsn_intellect_fortress;
extern int gsn_mental_barrier;
extern int gsn_mind_thrust;
extern int gsn_project_force;
extern int gsn_psionic_blast;
extern int gsn_psychic_crush;
extern int gsn_thought_shield;
extern int gsn_astral_vortex;
extern int gsn_pattern;
extern int gsn_cause_light;
extern int gsn_cause_serious;
extern int gsn_cause_critical;
extern int gsn_change_sex;
extern int gsn_cure_blindness;
extern int gsn_cure_critical;
extern int gsn_cure_light;
extern int gsn_cure_poison;
extern int gsn_cure_serious;
extern int gsn_heal;
extern int gsn_power_heal;
extern int gsn_refresh;
extern int gsn_weaken;
extern int gsn_bark_skin;
extern int gsn_harm;
extern int gsn_adrenaline_control;
extern int gsn_biofeedback;
extern int gsn_cell_adjustment;
extern int gsn_recharge_mana;
extern int gsn_vitalize;
extern int gsn_complete_healing;
extern int gsn_ectoplasmic_form;
extern int gsn_energy_containment;
extern int gsn_enhanced_strength;
extern int gsn_flesh_armor;
extern int gsn_psychic_healing;
extern int gsn_share_strength;
extern int gsn_sense_traps;
extern int gsn_call_lightning;
extern int gsn_chain_lightning;
extern int gsn_continual_light;
extern int gsn_control_weather;
extern int gsn_earthquake;
extern int gsn_fireball;
extern int gsn_flamestrike;
extern int gsn_lightning_bolt;
extern int gsn_firestorm;
extern int gsn_meteor_storm;
extern int gsn_animate_dead;
extern int gsn_create_food;
extern int gsn_create_water;
extern int gsn_create_spring;
extern int gsn_agitation;
extern int gsn_enhance_armor;
extern int gsn_adrenaline_control;
extern int gsn_agitation;
extern int gsn_aura_sight;
extern int gsn_awe;
extern int gsn_ballistic_attack;
extern int gsn_biofeedback;
extern int gsn_cell_adjustment;
extern int gsn_combat_mind;
extern int gsn_recharge_mana;
extern int gsn_vitalize;
extern int gsn_complete_healing;
extern int gsn_control_flames;
extern int gsn_create_sound;
extern int gsn_death_field;
extern int gsn_decay;
extern int gsn_detonate;
extern int gsn_displacement;
extern int gsn_ectoplasmic_form;
extern int gsn_ego_whip;
extern int gsn_energy_containment;
extern int gsn_enhance_armor;
extern int gsn_ehanced_strength;
extern int gsn_flesh_armor;
extern int gsn_inertial_barrier;
extern int gsn_inflict_pain;
extern int gsn_intellect_fortress;
extern int gsn_lend_health;
extern int gsn_levitation;
extern int gsn_mental_barrier;
extern int gsn_mind_thrust;
extern int gsn_project_force;
extern int gsn_psionic_blast;
extern int gsn_psychic_crush;
extern int gsn_psychic_drain;
extern int gsn_psychich_healing;
extern int gsn_share_strength;
extern int gsn_thought_shield;
extern int gsn_ultrablast;
extern int gsn_know_alignment;
extern int gsn_ventriloquate;
extern int gsn_feeblemind;
extern int gsn_enchant_weapon;
extern int gsn_detect_evil;
extern int gsn_detect_good;
extern int gsn_detect_magic;
extern int gsn_detect_poison;
extern int gsn_faerie_fire;
extern int gsn_faerie_fog;
extern int gsn_know_alignment;
extern int gsn_identify;
extern int gsn_wrath_of_god;
extern int gsn_bless_weapon;
extern int gsn_recharge_item;
extern int gsn_remove_curse;
extern int gsn_lore;
extern int gsn_aura_sight;
extern int gsn_enhance_armor;
extern int gsn_word_of_recall;
extern int gsn_knock;
extern int gsn_dowse;
extern int gsn_thrust;
extern int gsn_shoot;
extern int gsn_second_shot;
extern int gsn_third_shot;
extern int gsn_accuracy;
extern int gsn_snare;
extern int gsn_classify;
extern int gsn_gather_herbs;
extern int gsn_song_of_revelation;
extern int gsn_song_of_rejuvenation;
extern int gsn_song_of_tranquility;
extern int gsn_song_of_shadows;
extern int gsn_song_of_sustenance;
extern int gsn_song_of_flight;
extern int gsn_chant_of_protection;
extern int gsn_chant_of_battle;
extern int gsn_chant_of_vigour;
extern int gsn_chant_of_entrapment;
extern int gsn_chant_of_enfeeblement;
extern int gsn_chant_of_pain;
extern int gsn_breathe_water;
extern int gsn_resist_heat;
extern int gsn_resist_cold;
extern int gsn_resist_lightning;
extern int gsn_resist_acid;
extern int gsn_hells_fire;
extern int gsn_prayer;
extern int gsn_frenzy;
extern int gsn_bless;
extern int gsn_mass_sanctuary;
extern int gsn_mass_heal;
extern int gsn_mass_power_heal;
extern int gsn_chaos_blast;
extern int gsn_detect_curse;
extern int gsn_knife_toss;
extern int gsn_soar;
extern int gsn_smoke_bomb;
extern int gsn_snap_shot;
extern int gsn_crush;
extern int gsn_swoop;
extern int gsn_smelt;
extern int gsn_strengthen;
extern int gsn_imbue;
extern int gsn_uncommon_set;
extern int gsn_rare_set;
extern int gsn_epic_set;
extern int gsn_legendary_set;
extern int gsn_repelling;
extern int gsn_group_weaponsmith;
extern int gsn_craft_weapon;
extern int gsn_counterbalance;
extern int gsn_weaponchain;
extern int gsn_shieldchain;
extern int gsn_hurl;
extern int gsn_serate;
extern int gsn_engrave;
extern int gsn_discharge;
extern int gsn_trigger;
extern int gsn_dart;
extern int gsn_launcher;
extern int gsn_reflector;
extern int gsn_shield;
extern int gsn_arrestor;
extern int gsn_driver;
extern int gsn_emergency;
extern int gsn_deploy;
extern int gsn_forager;
extern int gsn_spyglass;
extern int gsn_base;
extern int gsn_miner;
extern int gsn_hurl;
extern int gsn_fire_flask;
extern int gsn_frost_flask;
extern int gsn_stun_flask;
extern int gsn_blind_flask;
extern int gsn_lightning_flask;
extern int gsn_acid_flask;
extern int gsn_bmf_flask;
extern int gsn_group_inscription;
extern int gsn_inscribe;
extern int gsn_protection;
extern int gsn_enhancement;
extern int gsn_healing;
extern int gsn_ward;

/*
 *  Deity gsns
 */
extern int gsn_pantheoninfo;
extern int gsn_pantheonrank;
extern int gsn_prayer_combat;
extern int gsn_prayer_weaken;
extern int gsn_prayer_protection;
extern int gsn_prayer_passage;
extern int gsn_prayer_destruction;
extern int gsn_prayer_plague;


/*
 * Utility macros.
 */
#define UMIN( a, b )                    ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )                    ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define URANGE( a, b, c )               ( ( b ) < ( a ) ? ( a ) : ( ( b ) > ( c ) ? ( c ) : ( b ) ) )
#define LOWER( c )                      ( ( c ) >= 'A' && ( c ) <= 'Z' ? ( c ) + 'a' - 'A' : ( c ) )
#define UPPER( c )                      ( ( c ) >= 'a' && ( c ) <= 'z' ? ( c ) + 'A' - 'a' : ( c ) )
#define IS_SET( flag, bit )             ( ( flag ) &   ( bit ) )
#define SET_BIT( var, bit )             ( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )          ( ( var )  &= ~( bit ) )
#define MOD(a)                          ( a < 0 ? (a * -1) : (a) )

#define replace_string( pstr, nstr ){ free_string( (pstr) ); pstr=str_dup( (nstr) ); }

#define IS_NULLSTR( str )               ( ( str ) == NULL || ( str ) [ 0 ] == '\0' )
#define CH(d)                           ( ( d )->original ? ( d )->original : ( d )->character )


/*
 * Character macros.
 */
#define IS_QUESTOR( ch )                ( IS_SET( ( ch )->act, PLR_QUESTOR ) )
#define IS_NPC( ch )                    ( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_TRUE_IMMORTAL( ch )          ( ch->level >= L_SEN )
#define IS_IMMORTAL( ch )               ( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )                   ( get_trust( ch ) >= LEVEL_HERO )
#define IS_AFFECTED( ch, sn )           ( IS_SET( ( ch )->affected_by, ( sn ) ) )
#define CAN_SEE_MIST(ch)                ( ch->level > LEVEL_HERO || ch->sub_class == SUB_CLASS_VAMPIRE )
#define IS_RACE_ONE( ch )               ( skill_lookup( race_table [ ch->race ].spell_one ) )
#define IS_RACE_TWO( ch )               ( skill_lookup( race_table [ ch->race ].spell_two ) )
#define IS_RACE_SKILL( ch )             ( IS_RACE_ONE( ch ) && IS_RACE_TWO( ch ) )
#define IS_GOOD( ch )                   ( ch->alignment >=  350 )
#define IS_EVIL( ch )                   ( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )                ( !IS_GOOD( ch ) && !IS_EVIL( ch ) )
#define IS_GOODandNEUTRAL( ch )         ( ch->alignment >= 0 )
#define IS_EVILandNEUTRAL( ch )         ( ch->alignment <  0 )
#define IS_RESTING( ch )                ( ch->position == POS_RESTING )
#define IS_AWAKE( ch )                  ( ch->position > POS_SLEEPING )
#define GET_AC( ch )                    ( ( ch )->armor  + ( IS_AWAKE( ch )     ? dex_app[ get_curr_dex( ch )].defensive : 0 ) )
#define GET_HITROLL( ch )               ( ( ch )->hitroll + str_app[ get_curr_str( ch ) ].tohit )
#define GET_DAMROLL( ch )               ( ( ch )->damroll + str_app[ get_curr_str( ch ) ].todam )
#define IS_OUTSIDE( ch )                ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS) && ch->in_room->sector_type != SECT_INSIDE )
#define WAIT_STATE( ch, pulse )         ( ( ch )->wait = UMAX( ( ch )->wait, ( pulse ) ) )
#define MANA_COST( ch, sn )             ( IS_NPC( ch ) ? 0 : UMAX ( skill_table [ sn ].min_mana, 60 - ch->pcdata->learned [ sn ]) )
#define IS_SWITCHED( ch )               ( ch->pcdata->switched )


/*
 * Object macros.
 */
#define CAN_WEAR( class, cur_form, obj, part, loc ) ( IS_SET( obj->wear_flags, part ) \
                && form_wear_table      [ cur_form ].can_wear   [ loc ] \
                && wear_table           [ class ].can_wear              [ obj->item_type ] \
                && loc_wear_table       [ class ].can_wear              [ loc ] )

#define IS_OBJ_STAT( obj, stat )( IS_SET( ( obj )->extra_flags, ( stat ) ) )
#define IS_SPELL( skill )               ( skill_table [ skill ].spell_fun != spell_null )
#define CAN_DO( ch, sn )                ( (ch->pcdata->learned [ sn ] > 0) && ( form_skill_allow( ch, sn ) ) )


/*
 * Description macros.
 */
#define PERS( ch, looker )      ( can_see( looker, ( ch ) ) ? ( IS_NPC( ch ) ? ( ch )->short_descr : ( ch )->name ) : "someone" )


/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
        char * const            name;
        DO_FUN *                do_fun;
        int                     position;
        int                     level;
        int                     log;
};


/*
 * Structure for a social in the socials table.
 */
struct social_type
{
        char * const name;
        char * const char_no_arg;
        char * const others_no_arg;
        char * const char_found;
        char * const others_found;
        char * const vict_found;
        char * const char_auto;
        char * const others_auto;
};


/*
 * Global constants.
 */
extern const    struct str_app_type             str_app                         [ MAX_STAT ];
extern const    struct dex_app_type             dex_app                         [ MAX_STAT ];
extern const    struct con_app_type             con_app                         [ MAX_STAT ];
extern const    struct class_type               class_table                     [ MAX_CLASS ];
extern const    struct sub_class_type           sub_class_table                 [ MAX_SUB_CLASS ];
extern const    struct clan_items               clan_item_list                  [ MAX_CLAN ];
extern const    struct clan_type                clan_table                      [ MAX_CLAN ];
extern const    struct color_data               color_table                     [ ];
extern const    struct cmd_type                 cmd_table                       [ ];
extern const    struct liq_type                 liq_table                       [ LIQ_MAX  ];
extern const    struct blueprint_type           blueprint_list                  [ BLUEPRINTS_MAX ];
/* extern const    struct raw_mats_data            raw_mats_table                  [ RAW_MATS_MAX ]; */
extern const    struct skill_type               skill_table                     [ MAX_SKILL ];
extern const    struct social_type              social_table                    [ ];
extern const    struct pattern_points           pattern_list                    [ MAX_PATTERN ];
extern const    struct soar_points              soar_list                       [ MAX_SOAR ];
extern const    struct HERB                     herb_table                      [ MAX_HERBS ];
extern const    struct imbue_types              imbue_list                      [ MAX_IMBUE ];
extern const    struct song                     song_table                      [ MAX_SONGS ];
extern char *   const  color_list               [ MAX_COLOR_LIST ];
extern char *   const  clan_title               [ MAX_CLAN ]                    [ MAX_CLAN_LEVEL + 1 ];
extern const    struct race_struct              race_table                      [ MAX_RACE ];
extern const    struct level_struct             level_table                     [ MAX_LEVEL - 6 ];
extern const    struct level_struct             old_level_table                 [ MAX_LEVEL - 6 ];
extern const    struct soar_struct              soar_access                     [ MAX_SOAR ];
extern const    struct loc_wear_struct          form_wear_table                 [ MAX_FORM ];
extern const    struct wear_struct              wear_table                      [ MAX_CLASS + MAX_SUB_CLASS - 1 ];
extern const    struct loc_wear_struct          loc_wear_table                  [ MAX_CLASS + MAX_SUB_CLASS - 1 ];

extern struct           pre_req_struct          pre_req_table                   [ MAX_PRE_REQ ];
extern struct           spell_group_struct      spell_group_table               [ MAX_SPELL_GROUP ];
extern const int        *spell_groups                                           [ MAX_GROUPS ];
extern struct           form_skill_struct       form_skill_table                [ MAX_FORM_SKILL ];
extern struct           vampire_gag             vampire_gag_table               [ MAX_VAMPIRE_GAG ];



/*
 * Global variables.
 */
extern HELP_DATA                * help_first;
extern SHOP_DATA                * shop_first;
extern BAN_DATA                 * ban_list;
extern CHAR_DATA                * char_list;
extern DESCRIPTOR_DATA          * descriptor_list;
extern NOTE_DATA                * note_list;
extern OBJ_DATA                 * object_list;
extern AFFECT_DATA              * affect_free;
extern BAN_DATA                 * ban_free;
extern CHAR_DATA                * char_free;
extern DESCRIPTOR_DATA          * descriptor_free;
extern EXTRA_DESCR_DATA         * extra_descr_free;
extern NOTE_DATA                * note_free;
extern OBJ_DATA                 * obj_free;
extern PC_DATA                  * pcdata_free;
extern char     bug_buf         [ ];
extern time_t                   current_time;
extern time_t                   boot_time;
extern bool                     fLogAll;
extern FILE *                   fpReserve;
extern KILL_DATA                kill_table      [ ];
extern char                     log_buf         [ ];
extern TIME_INFO_DATA           time_info;
extern WEATHER_DATA             weather_info;
extern char                     * down_time;
extern char                     * warning1;
extern char                     * warning2;
extern AUCTION_DATA             * auction;

/* geoff's tables */
extern FAME_TABLE               fame_table              [ FAME_TABLE_LENGTH ];
extern CLAN_PKILL_TABLE         clan_pkill_table        [ MAX_CLAN ][ CLAN_PKILL_TABLE_LENGTH ];
extern CLAN_VANQ_TABLE          clan_vanq_table         [ MAX_CLAN ][ CLAN_VANQ_TABLE_LENGTH ];
extern CLAN_MEMBER_TABLE        clan_member_table       [ MAX_CLAN ][ CLAN_MEMBER_TABLE_LENGTH ];

/* Extra table stuff - Tavolir */
extern HERO_DATA                * hero_first;
extern HERO_DATA                * hero_last;
extern LEGEND_DATA              legend_table  [ LEGEND_TABLE_LENGTH ];
extern PKSCORE_DATA             pkscore_table [ PKSCORE_TABLE_LENGTH ];
extern INFAMY_TABLE             infamy_table [ INFAMY_TABLE_LENGTH ];


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_ansi                         );      /* ansi color */
DECLARE_DO_FUN( do_color                        );      /* toggles color on and off */
DECLARE_DO_FUN( do_addfame                      );      /* for adding fame */
DECLARE_DO_FUN( do_addqp                        );
DECLARE_DO_FUN( do_advance                      );
DECLARE_DO_FUN( do_advice                       );
DECLARE_DO_FUN( do_affects                      );      /* moved affects out of score - Brutus */
DECLARE_DO_FUN( do_afk                          );
DECLARE_DO_FUN( do_allow                        );
DECLARE_DO_FUN( do_allow_look                   );      /* toggle for viewing equipment */
DECLARE_DO_FUN( do_answer                       );
DECLARE_DO_FUN( do_areas                        );
DECLARE_DO_FUN( do_arena                        );      /* for arena ( like recall ) */
DECLARE_DO_FUN( do_assassinate                  );      /* assassinate skill */
DECLARE_DO_FUN( do_at                           );
DECLARE_DO_FUN( do_atemi                        );      /* martial artist - Brutus */
DECLARE_DO_FUN( do_attack                       );      /* sets number of magic attacks */
DECLARE_DO_FUN( do_auction                      );
DECLARE_DO_FUN( do_auto                         );
DECLARE_DO_FUN( do_autocoin                     );      /* autoloot coin from corpses; Gez */
DECLARE_DO_FUN( do_autoexit                     );
DECLARE_DO_FUN( do_autowield                    );
DECLARE_DO_FUN( do_autoloot                     );
DECLARE_DO_FUN( do_autolevel                    );      /* automatically level when you have enough experience; Owl */
DECLARE_DO_FUN( do_autosac                      );
DECLARE_DO_FUN( do_backstab                     );
DECLARE_DO_FUN( do_balance                      );      /* for bank - Brutus */
DECLARE_DO_FUN( do_borrow                       );      /* ditto */
DECLARE_DO_FUN( do_bamfin                       );
DECLARE_DO_FUN( do_bamfout                      );
DECLARE_DO_FUN( do_ban                          );
DECLARE_DO_FUN( do_bash                         );
DECLARE_DO_FUN( do_battle_aura                  );      /* brawler skill - Brutus */
DECLARE_DO_FUN( do_berserk                      );      /* Berserk from ENVY2.0 */
DECLARE_DO_FUN( do_bite                         );      /* tiger skill */
DECLARE_DO_FUN( do_wolfbite                     );      /* bite for werewolves */
DECLARE_DO_FUN( do_bladethirst                  );      /* nec skill - like poison weapon */
DECLARE_DO_FUN( do_blank                        );
DECLARE_DO_FUN( do_blink                        );      /* umgook added 11/10/98 */
DECLARE_DO_FUN( do_brandish                     );
DECLARE_DO_FUN( do_breathe                      );      /* hydr skill */
DECLARE_DO_FUN( do_break_wrist                  );      /* brawler skill - Brutus */
DECLARE_DO_FUN( do_brew                         );      /* Brew potions */
DECLARE_DO_FUN( do_brief                        );
DECLARE_DO_FUN( do_bug                          );
DECLARE_DO_FUN( do_buy                          );
DECLARE_DO_FUN( do_chameleon                    );
DECLARE_DO_FUN( do_cando                        );
DECLARE_DO_FUN( do_cast                         );
DECLARE_DO_FUN( do_circle                       );      /* New thief skill - Brutus */
DECLARE_DO_FUN( do_change                       );      /* sub-class changing shit */
DECLARE_DO_FUN( do_channels                     );
DECLARE_DO_FUN( do_chat                         );
DECLARE_DO_FUN( do_choke                        );      /* choke for brawlers - Brutus */
DECLARE_DO_FUN( do_clantalk                     );      /* clan talking - brutus */
DECLARE_DO_FUN( do_climb                        );      /* climb skill - Brutus */
DECLARE_DO_FUN( do_close                        );
DECLARE_DO_FUN( do_coil                         );      /* coil - for snakes */
DECLARE_DO_FUN( do_constrict                    );      /* constrict - for snakes */
DECLARE_DO_FUN( do_combine                      );
DECLARE_DO_FUN( do_combo                        );      /* martial artist combos */
DECLARE_DO_FUN( do_commands                     );
DECLARE_DO_FUN( do_compare                      );
DECLARE_DO_FUN( do_config                       );
DECLARE_DO_FUN( do_consider                     );
DECLARE_DO_FUN( do_counterbalance               );      /* foe wmithys JUl 2022 - Brutus */
DECLARE_DO_FUN( do_construct                    );
DECLARE_DO_FUN( do_credits                      );
DECLARE_DO_FUN( do_crush                        );      /* crush for shifter bear form - Owl */
DECLARE_DO_FUN( do_cscore                       );      /* clan score - Brutus */
DECLARE_DO_FUN( do_decapitate                   );      /* New ninja skill - Brutus */
DECLARE_DO_FUN( do_delet                        );
DECLARE_DO_FUN( do_delete                       );      /* For self Deletion ( ROM code ) */
DECLARE_DO_FUN( do_deny                         );
DECLARE_DO_FUN( do_deposit                      );      /* deposit money */
DECLARE_DO_FUN( do_description                  );
DECLARE_DO_FUN( do_dirt_kick                    );      /* New ninja skill - Brutus */
DECLARE_DO_FUN( do_disable                      );      /* for disarming traps */
DECLARE_DO_FUN( do_disarm                       );
DECLARE_DO_FUN( do_dive                         );      /* dragons */
DECLARE_DO_FUN( do_disconnect                   );
DECLARE_DO_FUN( do_donate                       );      /* Donate command */
DECLARE_DO_FUN( do_down                         );
DECLARE_DO_FUN( do_drink                        );
DECLARE_DO_FUN( do_drop                         );
DECLARE_DO_FUN( do_east                         );
DECLARE_DO_FUN( do_eat                          );
DECLARE_DO_FUN( do_echo                         );
DECLARE_DO_FUN( do_emote                        );
DECLARE_DO_FUN( do_enter                        );      /* enter for portal.. - Brutus */
DECLARE_DO_FUN( do_equipment                    );
DECLARE_DO_FUN( do_examine                      );
DECLARE_DO_FUN( do_exits                        );
DECLARE_DO_FUN( do_fame                         );      /* fame table - geoff */
DECLARE_DO_FUN( do_feed                         );      /* for vamps - Matt */
DECLARE_DO_FUN( do_fill                         );
DECLARE_DO_FUN( do_fixmoney                     );
DECLARE_DO_FUN( do_fixexp                       );
DECLARE_DO_FUN( do_fixnote                      );
DECLARE_DO_FUN( do_flee                         );
DECLARE_DO_FUN( do_focus                        );      /* brawler skill */
DECLARE_DO_FUN( do_follow                       );
DECLARE_DO_FUN( do_force                        );
DECLARE_DO_FUN( do_forage                       );      /* bear skill */
DECLARE_DO_FUN( do_forge                        );      /* forge skill - warriors */
DECLARE_DO_FUN( do_freeze                       );
DECLARE_DO_FUN( do_gag                          );      /* gag parry etc... */
DECLARE_DO_FUN( do_get                          );
DECLARE_DO_FUN( do_give                         );
DECLARE_DO_FUN( do_gouge                        );      /* brawler skill */
DECLARE_DO_FUN( do_grapple                      );      /* ditto */
DECLARE_DO_FUN( do_fly                          );      /* Fly for native flyers */
DECLARE_DO_FUN( do_flying_headbutt              );
DECLARE_DO_FUN( do_goto                         );
DECLARE_DO_FUN( do_group                        );
DECLARE_DO_FUN( do_group_order                  );      /* new group order - Shade */
DECLARE_DO_FUN( do_group_leader                 );
DECLARE_DO_FUN( do_gtell                        );
DECLARE_DO_FUN( do_guide                        );
DECLARE_DO_FUN( do_guild                        );      /* clan command for adding people */
DECLARE_DO_FUN( do_headbutt                     );      /* New warrior/ninja skill - Brutus */
DECLARE_DO_FUN( do_heal                         );      /* for the new heal thingee - Brutus */
DECLARE_DO_FUN( do_howl                         );
DECLARE_DO_FUN( do_heighten                     );
DECLARE_DO_FUN( do_help                         );
DECLARE_DO_FUN( do_hero                         );      /* hero table - Tavolir */
DECLARE_DO_FUN( do_hide                         );
DECLARE_DO_FUN( do_holylight                    );
DECLARE_DO_FUN( do_home                         );      /* clan recall - Brutus */
DECLARE_DO_FUN( do_hunt                         );      /* Ranger skill */
DECLARE_DO_FUN( do_idea                         );
DECLARE_DO_FUN( do_immtalk                      );
DECLARE_DO_FUN( do_dirtalk                      );
DECLARE_DO_FUN( do_identify                     );      /* new identify code */
DECLARE_DO_FUN( do_imbue                        );      /* Smithy Imbue - Brutus Jun 2022 */
DECLARE_DO_FUN( do_imprint                      );      /* For brew/scribe skills */
DECLARE_DO_FUN( do_info                         );      /* new info channel */
DECLARE_DO_FUN( do_intimidate                   );      /* intimidate - thugs */
DECLARE_DO_FUN( do_extort                       );
DECLARE_DO_FUN( do_inventory                    );
DECLARE_DO_FUN( do_invis                        );
DECLARE_DO_FUN( do_joust                        );      /* joust for knight */
DECLARE_DO_FUN( do_kansetsu                     );      /* Martial artist - brutus */
DECLARE_DO_FUN( do_kiai                         );      /* monk skill */
DECLARE_DO_FUN( do_kick                         );
DECLARE_DO_FUN( do_kill                         );
DECLARE_DO_FUN( do_killsocket                   );
DECLARE_DO_FUN( do_leader                       );      /* for the clan leaders */
DECLARE_DO_FUN( do_leav                         );
DECLARE_DO_FUN( do_leave                        );      /* for leaving a clan */
DECLARE_DO_FUN( do_legend                       );      /* Legend table - Tavolir */
DECLARE_DO_FUN( do_bounty                       );      /* Bounty setup - Danath */
DECLARE_DO_FUN( do_levels                       );      /* levels command for new exp system */
DECLARE_DO_FUN( do_look_coins                   );
DECLARE_DO_FUN( do_list                         );
DECLARE_DO_FUN( do_lock                         );
DECLARE_DO_FUN( do_log                          );
DECLARE_DO_FUN( do_look                         );
DECLARE_DO_FUN( do_lunge                        );      /* for VAMPS - Brutus */
DECLARE_DO_FUN( do_map                          );      /* Main mapping func - Tavolir */
DECLARE_DO_FUN( do_maul                         );      /* tiger skill */
DECLARE_DO_FUN( do_mawasigeri                   );      /* Martial artist - brutus */
DECLARE_DO_FUN( do_meditate                     );
DECLARE_DO_FUN( do_memory                       );
DECLARE_DO_FUN( do_mfind                        );
DECLARE_DO_FUN( do_mload                        );
DECLARE_DO_FUN( do_morph                        );      /* for changing forms - geoff */
DECLARE_DO_FUN( do_mount                        );      /* mounting mobs for riding */
DECLARE_DO_FUN( do_dismount                     );
DECLARE_DO_FUN( do_destrier                     );
DECLARE_DO_FUN( do_pattern                      );
DECLARE_DO_FUN( do_soar                         );
DECLARE_DO_FUN( do_infamy                       );       /* Shade Apr 22 */
DECLARE_DO_FUN( do_repair                       );      /* Owl 16/6/22 */

/* The following are for mob programs - Brutus */
DECLARE_DO_FUN( do_mpasound                     );
DECLARE_DO_FUN( do_mpat                         );
DECLARE_DO_FUN( do_mpecho                       );
DECLARE_DO_FUN( do_mpechoaround                 );
DECLARE_DO_FUN( do_mpechoat                     );
DECLARE_DO_FUN( do_mpforce                      );
DECLARE_DO_FUN( do_mpgoto                       );
DECLARE_DO_FUN( do_mpjunk                       );
DECLARE_DO_FUN( do_mpkill                       );
DECLARE_DO_FUN( do_mpmload                      );
DECLARE_DO_FUN( do_mpoload                      );
DECLARE_DO_FUN( do_mppurge                      );
DECLARE_DO_FUN( do_mpstat                       );
DECLARE_DO_FUN( do_mptransfer                   );

DECLARE_DO_FUN( do_mset                         );
DECLARE_DO_FUN( do_oclanitem                    );
DECLARE_DO_FUN( do_mist_walk                    );
DECLARE_DO_FUN( do_mstat                        );
DECLARE_DO_FUN( do_mwhere                       );
DECLARE_DO_FUN( do_murde                        );
DECLARE_DO_FUN( do_murder                       );
DECLARE_DO_FUN( do_music                        );
DECLARE_DO_FUN( do_newbie                       );
DECLARE_DO_FUN( do_newlock                      );
DECLARE_DO_FUN( do_noemote                      );
DECLARE_DO_FUN( do_north                        );
DECLARE_DO_FUN( do_note                         );
DECLARE_DO_FUN( do_notell                       );
DECLARE_DO_FUN( do_numlock                      );
DECLARE_DO_FUN( do_ofind                        );
DECLARE_DO_FUN( do_oload                        );
DECLARE_DO_FUN( do_open                         );
DECLARE_DO_FUN( do_open_seal                    );      /*for werewolfs*/
DECLARE_DO_FUN( do_order                        );
DECLARE_DO_FUN( do_oset                         );
DECLARE_DO_FUN( do_ostat                        );
DECLARE_DO_FUN( do_owhere                       );
DECLARE_DO_FUN( do_pagelen                      );
DECLARE_DO_FUN( do_pardon                       );
DECLARE_DO_FUN( do_password                     );
DECLARE_DO_FUN( do_peace                        );
DECLARE_DO_FUN( do_pick                         );
DECLARE_DO_FUN( do_pkillers                     );
DECLARE_DO_FUN( do_venom                        );      /* spider skill - BBrutus */
DECLARE_DO_FUN( do_poison_weapon                );
DECLARE_DO_FUN( do_pose                         );
DECLARE_DO_FUN( do_practice                     );
DECLARE_DO_FUN( do_pugalism                     );      /* for brawlers */
DECLARE_DO_FUN( do_shuto                        );      /* Martial artist - brutus */
DECLARE_DO_FUN( do_swim                         );      /* hmmmm i wonder ?? */
DECLARE_DO_FUN( do_spy                          ); /* for the sfter */
DECLARE_DO_FUN( do_strangle                     ); /* for the sfter */
DECLARE_DO_FUN( do_prompt                       );
DECLARE_DO_FUN( do_promote                      ); /* promote command for leaders */
DECLARE_DO_FUN( do_punch                        ); /* brawler skill _ brutus */
DECLARE_DO_FUN( do_push                         ); /* ditto*/
DECLARE_DO_FUN( do_purge                        );
DECLARE_DO_FUN( do_put                          );
DECLARE_DO_FUN( do_quaff                        );
DECLARE_DO_FUN( do_quest                        );
DECLARE_DO_FUN( do_question                     );
DECLARE_DO_FUN( do_qui                          );
DECLARE_DO_FUN( do_quiet                        );      /* Silent play mode */
DECLARE_DO_FUN( do_quit                         );
DECLARE_DO_FUN( do_rage                         );      /* werewolves */
DECLARE_DO_FUN( do_ravage                       );      /* maul for werewolves */
DECLARE_DO_FUN( do_reboo                        );
DECLARE_DO_FUN( do_reboot                       );
DECLARE_DO_FUN( do_recall                       );
DECLARE_DO_FUN( do_recho                        );
DECLARE_DO_FUN( do_recite                       );
DECLARE_DO_FUN( do_remove                       );
DECLARE_DO_FUN( do_rename                       );      /* New hero wiz-command - Brutus */
DECLARE_DO_FUN( do_rent                         );
DECLARE_DO_FUN( do_reply                        );
DECLARE_DO_FUN( do_report                       );
DECLARE_DO_FUN( do_rescue                       );
DECLARE_DO_FUN( do_reset                        );
DECLARE_DO_FUN( do_rest                         );
DECLARE_DO_FUN( do_restore                      );
DECLARE_DO_FUN( do_return                       );
DECLARE_DO_FUN( do_ronin                        );      /* Toggle RONIN flag; Gez & Shade */
DECLARE_DO_FUN( do_rset                         );
DECLARE_DO_FUN( do_rstat                        );
DECLARE_DO_FUN( do_sacrifice                    );
DECLARE_DO_FUN( do_safe                         );      /* Test if room is safe; Gez */
DECLARE_DO_FUN( do_save                         );
DECLARE_DO_FUN( do_say                          );
DECLARE_DO_FUN( do_scan                         );
DECLARE_DO_FUN( do_score                        );
DECLARE_DO_FUN( do_scribe                       );      /* scribe scrolls */
DECLARE_DO_FUN( do_sell                         );
DECLARE_DO_FUN( do_shadow_form                  );
DECLARE_DO_FUN( do_sharpen                      );
DECLARE_DO_FUN( do_shout                        );
DECLARE_DO_FUN( do_shutdow                      );
DECLARE_DO_FUN( do_shutdown                     );
DECLARE_DO_FUN( do_silence                      );
DECLARE_DO_FUN( do_sla                          );
DECLARE_DO_FUN( do_slay                         );
DECLARE_DO_FUN( do_sleep                        );
DECLARE_DO_FUN( do_slist                        );
DECLARE_DO_FUN( do_slookup                      );
DECLARE_DO_FUN( do_smash                        );      /* Smash skill (WS) */
DECLARE_DO_FUN( do_smear                        );
DECLARE_DO_FUN( do_smelt                        );      /* Smithy - Brutus */
DECLARE_DO_FUN( do_snap_neck                    );      /* brawler goodie - brutus */
DECLARE_DO_FUN( do_sneak                        );
DECLARE_DO_FUN( do_tail                         );
DECLARE_DO_FUN( do_stalk                        );
DECLARE_DO_FUN( do_snoop                        );
DECLARE_DO_FUN( do_socials                      );
DECLARE_DO_FUN( do_south                        );
DECLARE_DO_FUN( do_spells                       );
DECLARE_DO_FUN( do_split                        );
DECLARE_DO_FUN( do_sset                         );
DECLARE_DO_FUN( do_sstime                       );
DECLARE_DO_FUN( do_status                       );
DECLARE_DO_FUN( do_stand                        );
DECLARE_DO_FUN( do_steal                        );
DECLARE_DO_FUN( do_stun                         );      /* Ninja/thief/ranger skill :) */
DECLARE_DO_FUN( do_suck                         );      /* Vampire thing - Brutus */
DECLARE_DO_FUN( do_switch                       );
DECLARE_DO_FUN( do_tailwhip                     );      /* Dragon form */
DECLARE_DO_FUN( do_tell                         );
DECLARE_DO_FUN( do_tellmode                     );      /* No tells */
DECLARE_DO_FUN( do_tetsui                       );      /* Martial artist - Brutus */
DECLARE_DO_FUN( do_time                         );
DECLARE_DO_FUN( do_title                        );
DECLARE_DO_FUN( do_train                        );
DECLARE_DO_FUN( do_transfer                     );
DECLARE_DO_FUN( do_transfix                     );
DECLARE_DO_FUN( do_trap                         );      /* Ninja skill again - Brutus */
DECLARE_DO_FUN( do_trapset                      );
DECLARE_DO_FUN( do_trapremove                   );
DECLARE_DO_FUN( do_trapstat                     );
DECLARE_DO_FUN( do_traplist                     );      /* new trap skills */
DECLARE_DO_FUN( do_trigger                      );      /* Trigger skill */
DECLARE_DO_FUN( do_trip                         );      /* New trip skill (from ROM) */
DECLARE_DO_FUN( do_trust                        );
DECLARE_DO_FUN( do_typo                         );
DECLARE_DO_FUN( do_throw                        );
DECLARE_DO_FUN( do_unlock                       );
DECLARE_DO_FUN( do_up                           );
DECLARE_DO_FUN( do_unarmed_combat               );      /* monks - geoff */
DECLARE_DO_FUN( do_users                        );
DECLARE_DO_FUN( do_value                        );
DECLARE_DO_FUN( do_vanquished                   );
DECLARE_DO_FUN( do_visible                      );
DECLARE_DO_FUN( do_wake                         );
DECLARE_DO_FUN( do_warcry                       );
DECLARE_DO_FUN( do_wear                         );
DECLARE_DO_FUN( do_weather                      );
DECLARE_DO_FUN( do_web                          );      /* spider skill - Brutus */
DECLARE_DO_FUN( do_west                         );
DECLARE_DO_FUN( do_where                        );
DECLARE_DO_FUN( do_whirlwind                    );      /* hit everyone */
DECLARE_DO_FUN( do_who                          );
DECLARE_DO_FUN( do_whois                        );
DECLARE_DO_FUN( do_wimpy                        );
DECLARE_DO_FUN( do_withdraw                     );      /* withdraw from bank */
DECLARE_DO_FUN( do_wizhelp                      );
DECLARE_DO_FUN( do_wizify                       );
DECLARE_DO_FUN( do_wizlist                      );
DECLARE_DO_FUN( do_wizlock                      );
DECLARE_DO_FUN( do_yell                         );
DECLARE_DO_FUN( do_yokogeri                     );      /* Martial artist - brutus */
DECLARE_DO_FUN( do_class                        );      /* new class channel */
DECLARE_DO_FUN( do_zap                          );
DECLARE_DO_FUN( do_zones                        );
DECLARE_DO_FUN( do_dowse                        );
DECLARE_DO_FUN( do_thrust                       );
DECLARE_DO_FUN( do_shoot                        );
DECLARE_DO_FUN( do_snare                        );
DECLARE_DO_FUN( do_classify                     );
DECLARE_DO_FUN( do_gather                       );
DECLARE_DO_FUN( do_sing                         );
DECLARE_DO_FUN( do_chant                        );
DECLARE_DO_FUN( do_wizbrew                      );      /* Gezhp goes crazy with power */
DECLARE_DO_FUN( do_suicid                       );
DECLARE_DO_FUN( do_suicide                      );
DECLARE_DO_FUN( do_knife_toss                   );
DECLARE_DO_FUN( do_smoke_bomb                   );
DECLARE_DO_FUN( do_snap_shot                    );
DECLARE_DO_FUN( do_swoop                        );      /* swoop for shifter phoenix form - Owl */

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN( spell_null                   );
DECLARE_SPELL_FUN( spell_acid_blast             );
DECLARE_SPELL_FUN( spell_inner_fire             );
DECLARE_SPELL_FUN( spell_synaptic_blast         );
DECLARE_SPELL_FUN( spell_prismatic_spray        );
DECLARE_SPELL_FUN( spell_holy_word              );
DECLARE_SPELL_FUN( spell_unholy_word            );
DECLARE_SPELL_FUN( spell_animate_weapon         );      /* for psi's - Istari*/
DECLARE_SPELL_FUN( spell_dark_ritual            );      /* for satanists - Simon */
DECLARE_SPELL_FUN( spell_transport              );
DECLARE_SPELL_FUN( spell_armor                  );
DECLARE_SPELL_FUN( spell_astral_sidestep        );      /* werewolf - Brutus */
DECLARE_SPELL_FUN( spell_astral_vortex          );
DECLARE_SPELL_FUN( spell_bless                  );
DECLARE_SPELL_FUN( spell_breathe_water          );      /* Gezhp 2000 */
DECLARE_SPELL_FUN( spell_blindness              );
DECLARE_SPELL_FUN( spell_burning_hands          );
DECLARE_SPELL_FUN( spell_dragon_shield          );
DECLARE_SPELL_FUN( spell_call_lightning         );
DECLARE_SPELL_FUN( spell_cause_critical         );
DECLARE_SPELL_FUN( spell_cause_light            );
DECLARE_SPELL_FUN( spell_cause_serious          );
DECLARE_SPELL_FUN( spell_chain_lightning        );
DECLARE_SPELL_FUN( spell_change_sex             );
DECLARE_SPELL_FUN( spell_charm_person           );
DECLARE_SPELL_FUN( spell_chill_touch            );
DECLARE_SPELL_FUN( spell_colour_spray           );
DECLARE_SPELL_FUN( spell_continual_light        );
DECLARE_SPELL_FUN( spell_control_weather        );
DECLARE_SPELL_FUN( spell_create_food            );
DECLARE_SPELL_FUN( spell_create_spring          );
DECLARE_SPELL_FUN( spell_create_water           );
DECLARE_SPELL_FUN( spell_cure_blindness         );
DECLARE_SPELL_FUN( spell_cure_critical          );
DECLARE_SPELL_FUN( spell_cure_light             );
DECLARE_SPELL_FUN( spell_cure_mana              );
DECLARE_SPELL_FUN( spell_cure_poison            );
DECLARE_SPELL_FUN( spell_cure_serious           );
DECLARE_SPELL_FUN( spell_curse                  );
DECLARE_SPELL_FUN( spell_detect_evil            );
DECLARE_SPELL_FUN( spell_detect_good            );
DECLARE_SPELL_FUN( spell_detect_hidden          );
DECLARE_SPELL_FUN( spell_detect_invis           );
DECLARE_SPELL_FUN( spell_detect_sneak           );
DECLARE_SPELL_FUN( spell_detect_magic           );
DECLARE_SPELL_FUN( spell_detect_poison          );
DECLARE_SPELL_FUN( spell_deter                  );
DECLARE_SPELL_FUN( spell_dispel_evil            );
DECLARE_SPELL_FUN( spell_dispel_magic           );
DECLARE_SPELL_FUN( spell_earthquake             );
DECLARE_SPELL_FUN( spell_enchant_weapon         );
DECLARE_SPELL_FUN( spell_energy_drain           );
DECLARE_SPELL_FUN( spell_entrapment             );
DECLARE_SPELL_FUN( spell_faerie_fire            );
DECLARE_SPELL_FUN( spell_faerie_fog             );
DECLARE_SPELL_FUN( spell_fear                   );
DECLARE_SPELL_FUN( spell_fireball               );
DECLARE_SPELL_FUN( spell_fireshield             );
DECLARE_SPELL_FUN( spell_flamestrike            );
DECLARE_SPELL_FUN( spell_fly                    );
DECLARE_SPELL_FUN( spell_gaias_warning          );
DECLARE_SPELL_FUN( spell_gate                   );
DECLARE_SPELL_FUN( spell_general_purpose        );
DECLARE_SPELL_FUN( spell_giant_strength         );
DECLARE_SPELL_FUN( spell_harm                   );
DECLARE_SPELL_FUN( spell_haste                  );
DECLARE_SPELL_FUN( spell_heal                   );
DECLARE_SPELL_FUN( spell_high_explosive         );
DECLARE_SPELL_FUN( spell_identify               );
DECLARE_SPELL_FUN( spell_infravision            );
DECLARE_SPELL_FUN( spell_invis                  );
DECLARE_SPELL_FUN( spell_know_alignment         );
DECLARE_SPELL_FUN( spell_lightning_bolt         );
DECLARE_SPELL_FUN( spell_locate_object          );
DECLARE_SPELL_FUN( spell_magic_missile          );
DECLARE_SPELL_FUN( spell_mass_invis             );
DECLARE_SPELL_FUN( spell_pass_door              );
DECLARE_SPELL_FUN( spell_poison                 );
DECLARE_SPELL_FUN( spell_power_heal             );
DECLARE_SPELL_FUN( spell_paralysis              );
DECLARE_SPELL_FUN( spell_protection             );
DECLARE_SPELL_FUN( spell_refresh                );
DECLARE_SPELL_FUN( spell_remove_curse           );
DECLARE_SPELL_FUN( spell_resist_magic           );      /* werewolf - Brutus */
DECLARE_SPELL_FUN( spell_sanctuary              );
DECLARE_SPELL_FUN( spell_sense_traps            );
DECLARE_SPELL_FUN( spell_shocking_grasp         );
DECLARE_SPELL_FUN( spell_shield                 );
DECLARE_SPELL_FUN( spell_sleep                  );
DECLARE_SPELL_FUN( spell_stone_skin             );
DECLARE_SPELL_FUN( spell_summon                 );
DECLARE_SPELL_FUN( spell_summon_avatar          );      /* werewolf */
DECLARE_SPELL_FUN( spell_summon_familiar        );
DECLARE_SPELL_FUN( spell_teleport               );
DECLARE_SPELL_FUN( spell_ventriloquate          );
DECLARE_SPELL_FUN( spell_weaken                 );
DECLARE_SPELL_FUN( spell_word_of_recall         );
DECLARE_SPELL_FUN( spell_acid_breath            );
DECLARE_SPELL_FUN( spell_fire_breath            );
DECLARE_SPELL_FUN( spell_frost_breath           );
DECLARE_SPELL_FUN( spell_gas_breath             );
DECLARE_SPELL_FUN( spell_lightning_breath       );
DECLARE_SPELL_FUN( spell_adrenaline_control     );
DECLARE_SPELL_FUN( spell_agitation              );
DECLARE_SPELL_FUN( spell_aura_sight             );
DECLARE_SPELL_FUN( spell_awe                    );
DECLARE_SPELL_FUN( spell_ballistic_attack       );
DECLARE_SPELL_FUN( spell_biofeedback            );
DECLARE_SPELL_FUN( spell_cell_adjustment        );
DECLARE_SPELL_FUN( spell_combat_mind            );
DECLARE_SPELL_FUN( spell_recharge_mana          );
DECLARE_SPELL_FUN( spell_complete_healing       );
DECLARE_SPELL_FUN( spell_control_flames         );
DECLARE_SPELL_FUN( spell_create_sound           );
DECLARE_SPELL_FUN( spell_death_field            );
DECLARE_SPELL_FUN( spell_decay                  );      /* for traps */
DECLARE_SPELL_FUN( spell_detonate               );
DECLARE_SPELL_FUN( spell_disintegrate           );
DECLARE_SPELL_FUN( spell_displacement           );
DECLARE_SPELL_FUN( spell_domination             );
DECLARE_SPELL_FUN( spell_ectoplasmic_form       );
DECLARE_SPELL_FUN( spell_ego_whip               );
DECLARE_SPELL_FUN( spell_energy_containment     );
DECLARE_SPELL_FUN( spell_enhance_armor          );
DECLARE_SPELL_FUN( spell_enhanced_strength      );
DECLARE_SPELL_FUN( spell_flesh_armor            );
DECLARE_SPELL_FUN( spell_inertial_barrier       );
DECLARE_SPELL_FUN( spell_inflict_pain           );
DECLARE_SPELL_FUN( spell_intellect_fortress     );
DECLARE_SPELL_FUN( spell_lend_health            );
DECLARE_SPELL_FUN( spell_levitation             );
DECLARE_SPELL_FUN( spell_mental_barrier         );
DECLARE_SPELL_FUN( spell_mind_thrust            );
DECLARE_SPELL_FUN( spell_project                );
DECLARE_SPELL_FUN( spell_project_force          );
DECLARE_SPELL_FUN( spell_psionic_blast          );
DECLARE_SPELL_FUN( spell_psychic_crush          );
DECLARE_SPELL_FUN( spell_psychic_drain          );
DECLARE_SPELL_FUN( spell_psychic_healing        );
DECLARE_SPELL_FUN( spell_share_strength         );
DECLARE_SPELL_FUN( spell_thought_shield         );
DECLARE_SPELL_FUN( spell_ultrablast             );
DECLARE_SPELL_FUN( spell_wrath_of_god           );
DECLARE_SPELL_FUN( spell_feeblemind             );
DECLARE_SPELL_FUN( spell_spiritwrack            );
DECLARE_SPELL_FUN( spell_wither                 );
DECLARE_SPELL_FUN( spell_mass_sanctuary         );
DECLARE_SPELL_FUN( spell_mass_heal              );
DECLARE_SPELL_FUN( spell_mass_power_heal        );
DECLARE_SPELL_FUN( spell_bark_skin              );
DECLARE_SPELL_FUN( spell_firestorm              );
DECLARE_SPELL_FUN( spell_meteor_storm           );
DECLARE_SPELL_FUN( spell_sunray                 );
DECLARE_SPELL_FUN( spell_moonray                );
DECLARE_SPELL_FUN( spell_natures_fury           );
DECLARE_SPELL_FUN( spell_bless_weapon           );
DECLARE_SPELL_FUN( spell_prayer                 );
DECLARE_SPELL_FUN( spell_frenzy                 );
DECLARE_SPELL_FUN( spell_recharge_item          );
DECLARE_SPELL_FUN( spell_hex                    );
DECLARE_SPELL_FUN( spell_dark_globe             );
DECLARE_SPELL_FUN( spell_globe                  );
DECLARE_SPELL_FUN( spell_animate_dead           );
DECLARE_SPELL_FUN( spell_lore                   );
DECLARE_SPELL_FUN( spell_possession             );
DECLARE_SPELL_FUN( spell_demon_flames           );
DECLARE_SPELL_FUN( spell_steal_strength         );
DECLARE_SPELL_FUN( spell_steal_soul             );
DECLARE_SPELL_FUN( spell_satans_fury            );
DECLARE_SPELL_FUN( spell_hand_of_lucifer        );
DECLARE_SPELL_FUN( spell_summon_demon           );
DECLARE_SPELL_FUN( spell_knock                  );
DECLARE_SPELL_FUN( spell_vitalize               );
DECLARE_SPELL_FUN( spell_resist_heat            );
DECLARE_SPELL_FUN( spell_resist_cold            );
DECLARE_SPELL_FUN( spell_resist_lightning       );
DECLARE_SPELL_FUN( spell_resist_acid            );
DECLARE_SPELL_FUN( spell_hells_fire             );
DECLARE_SPELL_FUN( spell_chaos_blast            );
DECLARE_SPELL_FUN( spell_detect_curse           );
DECLARE_SPELL_FUN( spell_imprint                );


#define MOB_VNUM_SKELETON  3404
#define MOB_VNUM_GHOUL     3404
#define MOB_VNUM_GHOST     3404


/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 * but some systems have incomplete or non-ansi header files.
 */

#if     defined( linux )
char * crypt args( ( const char *key, const char *salt ) );
#endif


/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */

#if defined( unix )
#define PLAYER_DIR              "../player/"                    /* Player files */
#define MOB_DIR                 "MOBProgs/"                             /* For mob progs */
#define NULL_FILE               "/dev/null"                             /* To reserve one stream */
#endif

#if defined( linux )
#define PLAYER_DIR              "../player/"                            /* Player files */
#define NULL_FILE               "/dev/null"                             /* To reserve one stream */
#endif

#define AREA_LIST               "area.lst"                              /* List of areas */

#define BUG_FILE                "bugs.txt"                              /* For 'bug' and bug( ) */
#define IDEA_FILE               "ideas.txt"                             /* For 'idea' */
#define TYPO_FILE               "typos.txt"                             /* For 'typo' */
#define NOTE_FILE               "notes.txt"                             /* For 'notes' */
#define SHUTDOWN_FILE           "../src/shutdown.txt"                   /* For 'shutdown' */
#define DOWN_TIME_FILE          "time.txt"                              /* For automatic shutdown */


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD      CHAR_DATA
#define MID     MOB_INDEX_DATA
#define OD      OBJ_DATA
#define OID     OBJ_INDEX_DATA
#define RID     ROOM_INDEX_DATA
#define SF      SPEC_FUN
#define ED      EXIT_DATA


/* act_comm.c */
void add_follower                       args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void stop_follower                      args( ( CHAR_DATA *ch ) );
void die_follower                       args( ( CHAR_DATA *ch, char *name ) );
bool is_same_group                      args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool is_note_to                         args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void talk_auction                       args( ( char *argument ) );
void do_quit                            args( ( CHAR_DATA *ch, char *argument ) );
bool is_group_members_mount               ( CHAR_DATA *mount, CHAR_DATA *ch );
void server_message                       ( const char *text );


/*
 * Colour stuff by Lope of Loping Through The MUD
 */
int     colour          args( ( char type, CHAR_DATA *ch, char *string ) );
int     bgcolour        args( ( char type, CHAR_DATA *ch, char *string ) );
void    colourconv      args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void    send_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );

/* act_info.c */
void  show_list_to_char         args( ( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing ) );
void  show_char_to_char         args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
void  set_title                 args( ( CHAR_DATA *ch, char *title ) );
bool  check_blind               args( ( CHAR_DATA *ch ) );
int   has_pre_req                     ( CHAR_DATA *ch, int sn );
char* number_suffix                   ( int num );
void  print_player_status             ( CHAR_DATA *ch, char *buf );
void  print_who_data                  ( CHAR_DATA *ch, char *buf );

/* act_move.c */
void move_char          args( ( CHAR_DATA *ch, int door ) );
int  find_door          args( ( CHAR_DATA *ch, char *arg ) );
ED * get_exit           args( ( ROOM_INDEX_DATA *room, int dir ) );

/* act_obj.c */
void  do_auction                        args( ( CHAR_DATA *ch, char *argument ) );
void  calc_coin_weight                  args( ( CHAR_DATA *ch ) );
void  coins_to_char                     args( ( int numcoins, CHAR_DATA *ch, int mode ) );
void  coins_from_char                   args( ( int numcoins, CHAR_DATA *ch ) );
void  coin_crunch                       args( ( int numcoins , COIN_DATA *coins ) );
int   total_coins_char                  args( ( CHAR_DATA *ch ) );
void  do_get_coins                      args( ( CHAR_DATA *ch, char *argument ) );
bool  can_use_poison_weapon                   ( CHAR_DATA *ch );
void  set_obj_owner                           ( OBJ_DATA *obj, char *owner );
char* get_obj_owner                           ( OBJ_DATA *obj );
bool  is_obj_owner                            ( OBJ_DATA *obj, CHAR_DATA *ch );
void  destroy_clan_items                      ( CHAR_DATA *ch );
bool  has_ego_item_effect                     ( CHAR_DATA *ch, int flag );
void  player_leaves_clan                      ( CHAR_DATA *ch );
int   get_craft_obj_bonus                     ( CHAR_DATA *ch );
int   get_spellcraft_obj_bonus                ( CHAR_DATA *ch );

/* act_wiz.c */
ROOM_INDEX_DATA *       find_location   args( ( CHAR_DATA *ch, char *arg ) );
ROOM_INDEX_DATA *       find_qlocation  args( ( CHAR_DATA *ch, char *arg, int vnum ) );

/* comm.c */
void close_socket                       args( ( DESCRIPTOR_DATA *dclose ) );
void write_to_buffer                    args( ( DESCRIPTOR_DATA *d, const char *txt, int length ) );
void send_to_all_char                   args( ( const char *text ) );
void send_to_char                       args( ( const char *txt, CHAR_DATA *ch ) );
void send_paragraph_to_char                   ( char* text, CHAR_DATA* ch, unsigned int indent );
void ansi_color                         args( ( const char *txt, CHAR_DATA *ch ) );
void show_string                        args( ( DESCRIPTOR_DATA *d, char *input ) );
void act                                args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type ) );
void act_move                                 ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type );
void bit_explode                              ( CHAR_DATA *ch, char *buf, unsigned long int n);

/* db.c */
void    boot_db                         args( ( void ) );
void    area_update                     args( ( void ) );
CD *    create_mobile                   args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *    create_object                   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void    clear_char                      args( ( CHAR_DATA *ch ) );
void    free_char                       args( ( CHAR_DATA *ch ) );
char *  get_extra_descr                 args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *   get_mob_index                   args( ( int vnum ) );
OID *   get_obj_index                   args( ( int vnum ) );
RID *   get_room_index                  args( ( int vnum ) );
void    obj_strings                     args( ( OBJ_DATA *obj ) );
char    fread_letter                    args( ( FILE *fp ) );
int     fread_number                    args( ( FILE *fp ) );
char *  fread_string                    args( ( FILE *fp ) );
void    fread_to_eol                    args( ( FILE *fp ) );
char *  fread_word                      args( ( FILE *fp ) );
void *  alloc_mem                       args( ( int sMem ) );
void *  alloc_perm                      args( ( int sMem ) );
void    free_mem                        args( ( void *pMem, int sMem ) );
char *  str_dup                         args( ( const char *str ) );
void    free_string                     args( ( char *pstr ) );
int     number_fuzzy                    args( ( int number ) );
int     number_range                    args( ( int from, int to ) );
int     number_percent                  args( ( void ) );
int     number_door                     args( ( void ) );
int     number_bits                     args( ( int width ) );
int     number_mm                       args( ( void ) );
int     dice                            args( ( int number, int size ) );
int     interpolate                     args( ( int level, int value_00, int value_32 ) );
void    smash_tilde                     args( ( char *str ) );
bool    str_cmp                         args( ( const char *astr, const char *bstr ) );
bool    str_prefix                      args( ( const char *astr, const char *bstr ) );
bool    str_infix                       args( ( const char *astr, const char *bstr ) );
bool    str_suffix                      args( ( const char *astr, const char *bstr ) );
char *  capitalize                      args( ( const char *str ) );
char *  capitalize_initial              args( ( const char *str ) );
char *  initial                         args( ( const char *str ) );
void    append_file                     args( ( CHAR_DATA *ch, char *file, char *str ) );
void    bug                             args( ( const char *str, int param ) );
void    log_string                      args( ( const char *str ) );
void    tail_chain                      args( ( void ) );
void    reset_area                      args( ( AREA_DATA * pArea ) );

/* fight.c */
void    violence_update                 args( ( void ) );
bool    is_safe                         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    multi_hit                       args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    damage                          args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool poison ) );
void    update_pos                      args( ( CHAR_DATA *victim ) );
void    set_fighting                    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    stop_fighting                   args( ( CHAR_DATA *ch, bool fBoth ) );
void    raw_kill                              ( CHAR_DATA *ch, CHAR_DATA *victim, bool corpse );
void    death_cry                       args( ( CHAR_DATA *ch ) );
bool    one_hit                         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    death_penalty                   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    check_player_death              args( ( CHAR_DATA *opponent, CHAR_DATA *victim ) );
bool    in_pkill_range                  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    chat_killer                     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    reset_char_stats                      ( CHAR_DATA *ch);
bool    aggro_damage                          ( CHAR_DATA *ch, CHAR_DATA *victim, int damage );
void    check_autoloot                        ( CHAR_DATA *ch, CHAR_DATA *victim );
void    check_group_bonus                     (CHAR_DATA *ch) ;

/* handler.c */
int     get_dir                         args( ( char *txt  ) );
int     get_trust                       args( ( CHAR_DATA *ch ) );
int     get_age                         args( ( CHAR_DATA *ch ) );
int     get_curr_str                    args( ( CHAR_DATA *ch ) );
int     get_curr_int                    args( ( CHAR_DATA *ch ) );
int     get_curr_wis                    args( ( CHAR_DATA *ch ) );
int     get_curr_dex                    args( ( CHAR_DATA *ch ) );
int     get_curr_con                    args( ( CHAR_DATA *ch ) );
int     can_carry_n                     args( ( CHAR_DATA *ch ) );
int     can_carry_w                     args( ( CHAR_DATA *ch ) );
bool    is_name                         args( ( const char *str, char *namelist ) );
bool    multi_keyword_match             args( ( char *keys, char *namelist ) );
bool    is_full_name                    args( ( const char *str, char *namelist ) );
void    affect_to_char                  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove                   args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_strip                    args( ( CHAR_DATA *ch, int sn ) );
bool    is_affected                     args( ( CHAR_DATA *ch, int sn ) );
void    affect_join                     args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    char_from_room                  args( ( CHAR_DATA *ch ) );
void    char_to_room                    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_char                     args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char                   args( ( OBJ_DATA *obj ) );
int     apply_ac                        args( ( OBJ_DATA *obj, int iWear ) );
OD *    get_eq_char                     args( ( CHAR_DATA *ch, int iWear ) );
void    equip_char                      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    form_equip_char                       ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear );
void    unequip_char                    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int     count_obj_list                  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room                   args( ( OBJ_DATA *obj ) );
void    obj_to_room                     args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj                      args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj                    args( ( OBJ_DATA *obj ) );
void    extract_obj                     args( ( OBJ_DATA *obj ) );
void    extract_char                    args( ( CHAR_DATA *ch, bool fPull ) );
CD *    get_char_room                   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world                  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_qchar_world                 args( ( CHAR_DATA *ch, char *argument, int vnum ) );
OD *    get_obj_type                    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list                    args( ( CHAR_DATA *ch, char *argument, OBJ_DATA *list ) );
OD *    get_obj_carry                   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_wear                    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_here                    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world                   args( ( CHAR_DATA *ch, char *argument ) );
OD *    create_money                    args( ( int plat, int gold, int silver, int copper ) );
OD *    create_smelted_materials        args( ( int smelted_steel, int smelted_titanium, int smelted_adamantite, int smelted_electrum, int smelted_starmetal ) );
int     get_obj_number                  args( ( OBJ_DATA *obj ) );
int     get_inv_number                  args( ( OBJ_DATA *obj ) );
int     get_obj_weight                  args( ( OBJ_DATA *obj ) );
bool    is_clan                         args( ( CHAR_DATA *ch ) );
bool    is_same_clan                    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int     clan_lookup                     args( ( const char *name ) );
int     race_lookup                     args( ( const char *name ) );
int     class_lookup                    args( ( const char *name ) );
bool    room_is_dark                    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    room_is_private                 args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see                         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj                     args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_drop_obj                    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_craft                       args( ( CHAR_DATA *ch ) );
bool    can_spellcraft                  args( ( CHAR_DATA *ch ) );
char *  item_type_name                  args( ( OBJ_DATA *obj ) );
int     item_name_type                  args( ( char *name ) );
char *  affect_loc_name                 args( ( int location ) );
char *  affect_loc_name                 args( ( int location ) );
char *  affect_bit_name                 args( ( int vector ) );
char *  affect_bit_name_nice            args( ( int vector ) );
char *  act_bit_name                    args( ( int vector ) );
char *  pact_bit_name                   args( ( int vector ) );
char *  extra_form_name                 args( ( int form ) );
int     extra_form_int                        ( char *name );
char *  extra_bit_name                  args( ( int extra_flags ) );
char *  body_form_name                  args( ( int vector ) );
char *  room_flag_name                  args( ( int vector ) );
char *  area_flag_name                  args( ( int vector ) );
char *  wear_flag_name                  args( ( int vector ) );
char *  wear_location_name              args( ( int wearloc_num ) );
char *  weapon_damage_type_name         args( ( int dt_num) );
char *  sector_name                     args( ( int sector_num ) );
char *  race_name                       args( ( int race_num ) );
char *  spec_fun_name                         ( CHAR_DATA *ch );
char *  extra_class_name                args( ( int extra_flags ) );
char *  position_name                   args( ( int position) );
char *  full_class_name                       ( int class );
char *  full_sub_class_name                   ( int sub_class );
char *  extra_level_name                      ( CHAR_DATA *ch );
CD   *  get_char                        args( ( CHAR_DATA *ch ) );
bool    longstring                      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized                      args( ( CHAR_DATA *ch, int gsn ) );
void    end_of_game                     args( ( void ) );
int     form_skill_allow                args( ( CHAR_DATA *ch, int sn ) );
void    generate_stats                        ( CHAR_DATA *ch );
int     mana_cost                             ( CHAR_DATA *ch, int sn );
int     get_phys_penalty                      ( CHAR_DATA *ch );
int     get_int_penalty                       ( CHAR_DATA *ch );

/* hunt.c   */
void hunt_victim                args( ( CHAR_DATA *ch ));

/* interp.c */
void   interpret                args( ( CHAR_DATA *ch, char *argument ) );
bool   is_number                args( ( char *arg ) );
int    number_argument          args( ( char *argument, char *arg ) );
char * one_argument             args( ( char *argument, char *arg_first ) );
char * first_arg                      ( char *argument, char *arg_first, bool fCase );
bool   IS_SWITCHED              args( ( CHAR_DATA *ch ) );
bool   wiz_do                   args( ( CHAR_DATA *ch, char *command ) );

/* magic.c */
int  skill_lookup               args( ( const char *name ) );
bool saves_spell                args( ( int level, CHAR_DATA *victim ) );
void obj_cast_spell             args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) );
bool mob_interacts_players            ( CHAR_DATA *mob );

/* mob_commands.c */
char*   mprog_type_to_name      args( ( int type ) );

/* mob_prog.c */
#ifdef DUNNO_STRSTR
char * strstr                   args( ( const char *s1, const char *s2 ) );
#endif
void mprog_wordlist_check       args( ( char * arg, CHAR_DATA *mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void mprog_percent_check        args( ( CHAR_DATA *mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type ) );
void mprog_act_trigger          args( ( char* buf, CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj, void* vo ) );
void mprog_bribe_trigger        args( ( CHAR_DATA* mob, CHAR_DATA* ch, int amount ) );
void mprog_entry_trigger        args( ( CHAR_DATA* mob ) );
void mprog_give_trigger         args( ( CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj ) );
void mprog_greet_trigger        args( ( CHAR_DATA* mob ) );
void mprog_fight_trigger        args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void mprog_hitprcnt_trigger     args( ( CHAR_DATA* mob, CHAR_DATA* ch  ));
void mprog_death_trigger        args( ( CHAR_DATA* mob ) );
void mprog_random_trigger       args( ( CHAR_DATA* mob ) );
void mprog_speech_trigger       args( ( char* txt, CHAR_DATA* mob ) );

/* save.c */
void save_char_obj              args( ( CHAR_DATA *ch ) );
bool load_char_obj              args( ( DESCRIPTOR_DATA *d, char *name ) );
void save_fame_table            ( );
void load_fame_table            ( );
void save_all_clan_tables       ( );
void load_all_clan_tables       ( );
void save_vanq_table            ( int clan );
void load_vanq_table            ( int clan );
void save_pkill_table           ( int clan );
void load_pkill_table           ( int clan );
bool fread_hero                 args( ( HERO_DATA *hero, FILE *fp ) );
void load_hero_table            ( );
void save_hero_table            ( );
void load_legend_table          ( );
void save_legend_table          ( );
void load_pkscore_table         ( );
void save_pkscore_table         ( );
void load_infamy_table          ( );
void save_infamy_table          ( );


/* special.c */
SF * spec_lookup        args( ( const char *name ) );

/* update.c */
void advance_level                                      args( ( CHAR_DATA *ch ) );
void gain_exp                                           args( ( CHAR_DATA *ch, int gain ) );
void gain_condition                                     args( ( CHAR_DATA *ch, int iCond, int value ) );
void update_handler                                     args( ( void ) );
void auction_update                                     args( ( void ) );
void form_equipment_update                                    ( CHAR_DATA *ch );
int  check_stat_advance                                       ( CHAR_DATA *ch, int stat );
bool check_questpoints_allow_level_gain                       ( CHAR_DATA *ch, bool verbose );

/* trap.c */
bool checkmovetrap      args( ( CHAR_DATA *ch, int dir) );
bool checkgetput        args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
bool checkopen          args( ( CHAR_DATA *ch, OBJ_DATA *obj) );

/* bet.h */
int parsebet    args( ( const int currentbet, const char *argument ) );

/* table.c */
void                    check_fame_table                ( CHAR_DATA *ch );
void                    print_fame_table                ( CHAR_DATA *ch, char *argument );
void                    check_clan_pkill_table          ( CHAR_DATA *ch );
void                    print_clan_pkill_table          ( CHAR_DATA *ch, char *argument );
void                    strip_clan_pkill_table          ( CHAR_DATA *ch );
void                    check_clan_vanq_table           ( CHAR_DATA *ch, CHAR_DATA *victim );
void                    print_clan_vanq_table           ( CHAR_DATA *ch, char *argument );
void                    check_clan_member_table         ( CHAR_DATA *ch );
void                    print_clan_member_table         ( CHAR_DATA *ch, char *argument );
void                    strip_clan_member_table         ( CHAR_DATA *ch );
void                    print_hero_table                ( CHAR_DATA *ch, char *argument );
void                    add_hero                        ( CHAR_DATA *ch );
void                    print_legend_table              ( CHAR_DATA *ch, char *argument );
LEGEND_DATA             swap_legend_table               args( ( LEGEND_DATA temp, int position ) );
void                    update_legend_table             args( ( CHAR_DATA *ch ) );
void                    print_pkscore_table             ( CHAR_DATA *ch, int start);
PKSCORE_DATA            swap_pkscore_table              args( ( PKSCORE_DATA temp, int position ) );
void                    update_pkscore_table            args( ( CHAR_DATA *ch ) );
void                    check_infamy_table              ( CHAR_DATA *ch);
void                    print_infamy_table              ( CHAR_DATA *ch, char *argument);

/* sft.c */
void do_morph_wolf              ( CHAR_DATA *ch, bool to_form );
void do_morph_direwolf          ( CHAR_DATA *ch, bool to_form );
bool is_valid_soar              args( ( CHAR_DATA *ch, int soar_index ) );

/* skill.c */
void strip_mount        ( CHAR_DATA *ch );
bool is_bladed_weapon   ( OBJ_DATA *obj );
bool is_blunt_weapon    ( OBJ_DATA *obj );
bool is_piercing_weapon ( OBJ_DATA *obj );

/* quest.c */
bool mob_is_quest_target (CHAR_DATA *ch);


#undef  CD
#undef  MID
#undef  OD
#undef  OID
#undef  RID
#undef  SF


/* EOF merc.h */
