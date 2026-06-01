/* Any sound-related header information that doesn't HAVE to be in merc.h should go here */

#include <math.h>
#include <stdint.h>
#include <unistd.h>
#define _XOPEN_SOURCE
#include "protocol.h"
#include "merc.h"

#define SND_DEF_ENABLED         TRUE   /* sound on by default */
#define SND_LOG_ENABLED         FALSE   /* sound logging default */
#define SND_DEF_MASTER          100     /* master 0..100 */
#define SND_DEF_ENV             100    /* environment (area/room/sector) scale 0..100 */
#define SND_DEF_MUSIC           100    /* music (boss/combat/etc.) scale 0..100 */
#define SND_DEF_FOLEY           100    /* mob/player foley scale 0..100 */
#define SND_DEF_SFX             100    /* combat/item/UI SFX scale 0..100 */
#define SND_DEF_NOTIFY          100    /* notifications scale 0..100 */
#define SND_DEF_UI              100    /* UI scale 0..100 */
#define DOOR_LABEL              "door"  /* human category */
#define DOOR_LANE_FMT           "dd.sfx.door.%d.%s"   /* roomVnum + dir name */
#define SOUND_MAX_FILES         64  /* 63 + NULL terminator */
#define TYPE_BOW_HIT            ( TYPE_HIT + 100 ) /* For making shoot sounds different */

/* --- Sound events (one-shot SFX / notifications) ------------------------ */
typedef struct sound_event_def {
    const char *key;          /* e.g., "notify.levelup", "fx.footstep.leather" */
    const char *files[SOUND_MAX_FILES ];     /* up to 32 variants; NULL-terminated */
    int         weights[SOUND_MAX_FILES];   /* same length as files; 0/NULL => equal weight */
    int         default_volume; /* 1..100 (will be scaled by player master vol) */
    const char *tag;          /* GMCP tag, e.g., "notify", "fx", "ui" */
    int         loops;        /* usually 1 for SFX */
} sound_event_def;

/* sector type default sounds */
typedef struct sector_ambience_t {
        const char *name;   /* relative media path or full URL; NULL/"" = none */
        int         volume; /* 1..100; <=0 means disabled */
} sector_ambience_t;

typedef enum {
    DOOR_ACT_OPEN,
    DOOR_ACT_CLOSE,
    DOOR_ACT_LOCK,
    DOOR_ACT_UNLOCK
} door_action_t;

typedef enum consume_action_t {
    CONSUME_ACT_EAT,
    CONSUME_ACT_DRINK,
    CONSUME_ACT_PILL,
    CONSUME_ACT_QUAFF,
    CONSUME_ACT_SMOKE,
    CONSUME_ACT_SMEAR
} consume_action_t;

/* Registry accessor */
const sound_event_def *sound_event_lookup(const char *key);

/* Sound-related function decs */
void            media_env_refresh               ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool force );
void            sound_emit_char                 ( CHAR_DATA *ch, const char *event_key, int vol_override ); /* -1 to use default */
void            sound_emit_room                 ( ROOM_INDEX_DATA *room, const char *event_key, int vol_override /* -1 */, CHAR_DATA *except );
int             media_apply_master_volume       ( int base_vol, CHAR_DATA *ch ); /* scales 1..100 by player setting (or returns base) */
void            media_notify_levelup            ( CHAR_DATA *who );
void            media_notify_channel            ( CHAR_DATA *to, int channel );
int             media_apply_volume              ( int base_vol, CHAR_DATA *ch, const char *tag, const char *type );
void            media_play_door_sfx_room        ( ROOM_INDEX_DATA *room, int door, door_action_t act );
void            sound_play_room_file            ( ROOM_INDEX_DATA *room, const char *file, int base_vol, const char *tag, CHAR_DATA *except , const char *id);
void            media_play_consume_sfx_room     ( ROOM_INDEX_DATA *room, consume_action_t act, CHAR_DATA *actor );
void            update_weather_for_char         ( CHAR_DATA *ch );
void            sfx_enqueue                     ( DESCRIPTOR_DATA *d, const char *file, int vol, const char *tag, int delay_ticks);
int             sound_combat_enqueue_hit_tier   ( struct char_data *attacker, struct char_data *victim, int damage_value, int organic_only );
void            sound_combat_hit_sfx            ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt );
void            sound_sfx_update                args( ( void ) );
void            sound_mobdeath_room             ( ROOM_INDEX_DATA *room, int base_vol );
void            sfx_queue_clear                 ( DESCRIPTOR_DATA *d );
void            sound_mobdeath_room_key         ( ROOM_INDEX_DATA *room, const char *key, int base_vol );
void            sound_combat_shield_block_sfx   ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_miss_sfx           ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_dodge_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_parry_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_acrobatics_sfx     ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_transfix_sfx       ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_blink_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_spell_sfx                 ( CHAR_DATA *ch, int sn, const char *phase );
void            sound_combat_resist_toxin_sfx   ( CHAR_DATA *victim );
void            sound_footstep_sfx              ( CHAR_DATA *actor, ROOM_INDEX_DATA *from_room, ROOM_INDEX_DATA *to_room, ROOM_INDEX_DATA *hear_room, CHAR_DATA *except );
void            sound_combat_maul_sfx           ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_web_sfx            ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_venom_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_crush_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_skill_sfx          ( CHAR_DATA *ch, CHAR_DATA *victim, const char *key );
void            sound_combat_backstab_sfx       ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_combat_assassinate_sfx    ( CHAR_DATA *ch, CHAR_DATA *victim );
void            sound_morph_sfx                 ( CHAR_DATA *ch );
void            sound_condition_sfx             ( CHAR_DATA *ch, const char *key );

extern const sector_ambience_t sector_ambience_defaults[SECT_MAX];
const        sector_ambience_t *sector_ambience_for(int sector);





