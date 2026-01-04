/* Any sound-related header information that doesn't HAVE to be in merc.h should go here */

#include <math.h>
#include <stdint.h>
#include <unistd.h>
#define _XOPEN_SOURCE
#include "protocol.h"
#include "merc.h"

#define SND_DEF_ENABLED TRUE              /* sound on by default */
#define SND_DEF_MASTER 70                 /* master 0..100 */
#define SND_DEF_ENV 100                   /* environment (area/room/sector) scale 0..100 */
#define SND_DEF_MUSIC 100                 /* music (boss/combat/etc.) scale 0..100 */
#define SND_DEF_FOLEY 100                 /* mob/player foley scale 0..100 */
#define SND_DEF_SFX 100                   /* combat/item/UI SFX scale 0..100 */
#define SND_DEF_NOTIFY 100                /* notifications scale 0..100 */
#define SND_DEF_UI 100                    /* UI scale 0..100 */
#define DOOR_LABEL "door"                 /* human category */
#define DOOR_LANE_FMT "dd.sfx.door.%d.%s" /* roomVnum + dir name */

/* --- Sound events (one-shot SFX / notifications) ------------------------ */
typedef struct sound_event_def
{
    const char *key;      /* e.g., "notify.levelup", "fx.footstep.leather" */
    const char *files[6]; /* up to 6 variants; NULL-terminated */
    int weights[6];       /* same length as files; 0/NULL => equal weight */
    int default_volume;   /* 1..100 (will be scaled by player master vol) */
    const char *tag;      /* GMCP tag, e.g., "notify", "fx", "ui" */
    int loops;            /* usually 1 for SFX */
} sound_event_def;

/* sector type default sounds */
typedef struct sector_ambience_t
{
    const char *name; /* relative media path or full URL; NULL/"" = none */
    int volume;       /* 1..100; <=0 means disabled */
} sector_ambience_t;

typedef enum
{
    DOOR_ACT_OPEN,
    DOOR_ACT_CLOSE,
    DOOR_ACT_LOCK,
    DOOR_ACT_UNLOCK
} door_action_t;

typedef enum consume_action_t
{
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
void media_env_refresh(CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool force);
void sound_emit_char(CHAR_DATA *ch, const char *event_key, int vol_override); /* -1 to use default */
void sound_emit_room(ROOM_INDEX_DATA *room, const char *event_key, int vol_override /* -1 */, CHAR_DATA *except);
int media_apply_master_volume(int base_vol, CHAR_DATA *ch); /* scales 1..100 by player setting (or returns base) */
void media_notify_levelup(CHAR_DATA *who);
void media_notify_channel(CHAR_DATA *to, int channel);
int media_apply_volume(int base_vol, CHAR_DATA *ch, const char *tag, const char *type);
void media_play_door_sfx_room(ROOM_INDEX_DATA *room, int door, door_action_t act);
void sound_play_room_file(ROOM_INDEX_DATA *room, const char *file, int base_vol, const char *tag, CHAR_DATA *except, const char *id);
void media_play_consume_sfx_room(ROOM_INDEX_DATA *room, consume_action_t act, CHAR_DATA *actor);
void update_weather_for_char(CHAR_DATA *ch);
void sfx_enqueue(DESCRIPTOR_DATA *d, const char *file, int vol, const char *tag, int delay_ticks);
void sound_sfx_update(void);

extern const sector_ambience_t sector_ambience_defaults[SECT_MAX];
const sector_ambience_t *sector_ambience_for(int sector);
