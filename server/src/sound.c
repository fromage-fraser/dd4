#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "sound.h"
#include "protocol.h"

static bool receives_channel_info(CHAR_DATA *to)
{
    if (!to || IS_NPC(to))
        return FALSE;
    if (to->silent_mode)
        return FALSE;
    if (IS_SET(to->deaf, CHANNEL_INFO))
        return FALSE;
    return TRUE;
}

/* Update weather ambience for a single character */
void update_weather_for_char(CHAR_DATA *ch)
{
    if (!ch || !ch->in_room || !ch->desc || !ch->desc->pProtocol)
        return;
    if (IS_NPC(ch))
        return;

    protocol_t *p = ch->desc->pProtocol;

    /* Must have GMCP Client.Media and not be suppressed */
    if (!p->bGMCP || !p->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
        return;
    if (p->MediaSuppress)
        return;

    /* Always assert base for relative paths */
    GMCP_Media_Default(ch->desc, "https://www.dragons-domain.org/main/gui/custom/audio/");

/* Stable key for weather lane */
#define KEY_WEATHER "dd.ambient.weather"

    /* If indoors or underwater, suppress weather sounds entirely */
    if ((!IS_OUTSIDE(ch)) || (ch->in_room->sector_type == SECT_UNDERWATER) || (ch->in_room->sector_type == SECT_UNDERWATER_GROUND))
    {
        if (p->MediaWeatherActive)
        {
            GMCP_Media_Stop(ch->desc,
                            "\"key\":\"" KEY_WEATHER "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");

            if (p->MediaWeatherName)
                free_string(p->MediaWeatherName);
            p->MediaWeatherName = NULL;
            p->MediaWeatherVol = 0;
            p->MediaWeatherActive = FALSE;

            log_stringf("WeatherSFX: stop (indoors) for %s", ch->name);
        }
        return;
    }

    /* Determine desired weather event and volume */
    const sound_event_def *ev = NULL;
    int vol = 0;

    if (weather_info.sky == SKY_RAINING)
    {
        ev = sound_event_lookup("ambient.weather.rain");
        vol = ev ? ev->default_volume : 15;
    }
    else if (weather_info.sky == SKY_LIGHTNING)
    {
        ev = sound_event_lookup("ambient.weather.lightning");
        vol = ev ? ev->default_volume : 15;
    }

    /* No relevant weather => stop current lane */
    if (!ev || !ev->files[0] || !*ev->files[0])
    {
        if (p->MediaWeatherActive)
        {
            GMCP_Media_Stop(ch->desc,
                            "\"key\":\"" KEY_WEATHER "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
            if (p->MediaWeatherName)
                free_string(p->MediaWeatherName);
            p->MediaWeatherName = NULL;
            p->MediaWeatherVol = 0;
            p->MediaWeatherActive = FALSE;

            log_stringf("WeatherSFX: stop (clear sky) for %s", ch->name);
        }
        return;
    }

    /* Apply player's ambient slider */
    vol = media_apply_volume(vol, ch, "environment", "ambient");
    if (vol <= 0)
        return;

    /* If changed or inactive, (re)play */
    if (!p->MediaWeatherActive || !p->MediaWeatherName || str_cmp(p->MediaWeatherName, ev->files[0]) || p->MediaWeatherVol != vol)
    {
        /* If something might still be playing under this lane key, fade it */
        if (p->MediaWeatherActive)
        {
            GMCP_Media_Stop(ch->desc,
                            "\"key\":\"" KEY_WEATHER "\","
                            "\"type\":\"music\",\"fadeaway\":true,\"fadeout\":600");
            log_stringf("WeatherSFX: stop previous (switch) for %s", ch->name);
        }

        /* Update our cache BEFORE play so do_rstat can show it immediately */
        if (p->MediaWeatherName)
            free_string(p->MediaWeatherName);
        p->MediaWeatherName = str_dup(ev->files[0]);
        p->MediaWeatherVol = vol;

        /* Play loop on the weather lane */
        {
            char opts[256];
            snprintf(opts, sizeof(opts),
                     "\"type\":\"music\",\"tag\":\"environment\",\"key\":\"" KEY_WEATHER "\","
                     "\"volume\":%d,\"loops\":-1,\"continue\":true,\"fadein\":1200",
                     vol);
            GMCP_Media_Play(ch->desc, ev->files[0], opts);
        }

        p->MediaWeatherActive = TRUE;

        log_stringf("WeatherSFX: start %s vol=%d for %s",
                    ev->key, vol, ch->name);
    }

#undef KEY_WEATHER
}

/* Scale a base 1..100 volume by the player's master volume (also 1..100).
   If you don't yet have per-player volume fields, just return base. */
int media_apply_volume(int base_vol, CHAR_DATA *ch, const char *tag, const char *type)
{
    int v = URANGE(1, base_vol, 100);
    if (!ch || IS_NPC(ch) || !ch->pcdata)
        return v;

    /* global mute */
    if (!ch->pcdata->snd_enabled)
        return 0;

    /* master */
    int master = URANGE(0, ch->pcdata->snd_master, 100);
    v = (v * master + 50) / 100;

    /* pick category slider */
    int cat = 100;

    /* music lane (area/room/sector ambience uses type:"music") */
    if (type && !str_cmp(type, "music"))
        cat = ch->pcdata->snd_music;
    else if (tag && !str_cmp(tag, "environment"))
        cat = ch->pcdata->snd_env;
    else if (tag && !str_cmp(tag, "foley"))
        cat = ch->pcdata->snd_foley;
    else if (tag && !str_cmp(tag, "ui"))
        cat = ch->pcdata->snd_ui;
    else if (tag && !str_cmp(tag, "notify"))
        cat = ch->pcdata->snd_notify;
    else
        cat = ch->pcdata->snd_sfx; /* default bucket for generic SFX */

    cat = URANGE(0, cat, 100);
    v = (v * cat + 50) / 100;

    /* 0 => muted (don’t send Play), otherwise clamp to 1..100 for GMCP */
    return v <= 0 ? 0 : URANGE(1, v, 100);
}

int media_apply_master_volume(int base_vol, CHAR_DATA *ch)
{
    /* Defensive defaults */
    if (base_vol <= 0)
        return 0;
    if (!ch || IS_NPC(ch) || !ch->pcdata)
        return base_vol;

    /* If the player has sound disabled, force silence */
    if (!ch->pcdata->snd_enabled)
        return 0;

    /* Clamp master and apply scaling (rounded), then clamp final 0..100 */
    int master = ch->pcdata->snd_master;
    master = URANGE(0, master, 100);

    /* Scale and round to nearest int */
    int scaled = (base_vol * master + 50) / 100;

    return URANGE(0, scaled, 100);
}

void media_notify_channel(CHAR_DATA *to, int channel)
{
    if (!to || IS_NPC(to) || !to->desc || !to->desc->pProtocol)
        return;

    /* Fast opt-out: if the player has notifications/UI volume off, skip. */
    if (!to->pcdata || !to->pcdata->snd_enabled || to->pcdata->snd_notify <= 0)
        return;

    /* Respect channel mutes here as a safety net (talk_channel also filters). */
    if (IS_SET(to->deaf, channel) || to->silent_mode)
        return;

    const char *key = NULL;
    switch (channel)
    {
    case CHANNEL_DIRTALK:
        key = "notify.channel.dirtalk";
        break;
    case CHANNEL_IMMTALK:
        key = "notify.channel.immtalk";
        break;
    case CHANNEL_CHAT:
        key = "notify.channel.chat";
        break;
    case CHANNEL_AUCTION:
        key = "notify.channel.auction";
        break;
    case CHANNEL_INFO:
        key = "notify.channel.info";
        break;
    case CHANNEL_CLAN:
        key = "notify.channel.clan";
        break;
    case CHANNEL_SHOUT:
        key = "notify.channel.shout";
        break;
    case CHANNEL_MUSIC:
        key = "notify.channel.music";
        break;
    case CHANNEL_QUESTION:
        key = "notify.channel.question";
        break;
    case CHANNEL_YELL:
        key = "notify.channel.yell";
        break;
    case CHANNEL_SERVER:
        key = "notify.channel.server";
        break;
    case CHANNEL_ARENA:
        key = "notify.channel.arena";
        break;
    case CHANNEL_NEWBIE:
        key = "notify.channel.newbie";
        break;
    default:
        return; /* unknown channel code */
    }

    /* One-shot to this player; registry supplies default volume + your scaling. */
    sound_emit_char(to, key, -1);
}

/* sound_play_room_file: enqueue instead of play */
void sfx_enqueue(DESCRIPTOR_DATA *d,
                 const char *file,
                 int vol,
                 const char *tag,
                 int delay_ticks)
{
    static unsigned long seq = 0;
    int next;
    SFX_EVENT *e;

    if (!d || !file || !*file)
        return;

    next = (d->sfx_tail + 1) % MAX_SFX_QUEUE;
    if (next == d->sfx_head)
    {
        log_stringf("SFXQ: DROP d=%p head=%d tail=%d cd=%d tag=%s vol=%d file=%s",
                    d,
                    d->sfx_head,
                    d->sfx_tail,
                    d->sfx_cooldown,
                    (tag && *tag) ? tag : "sfx",
                    vol,
                    file);
        return;
    }

    e = &d->sfx_q[d->sfx_tail];

    memset(e, 0, sizeof(*e));

    strncpy(e->file, file, sizeof(e->file) - 1);
    e->file[sizeof(e->file) - 1] = '\0';

    strncpy(e->tag, (tag && *tag) ? tag : "sfx", sizeof(e->tag) - 1);
    e->tag[sizeof(e->tag) - 1] = '\0';

    snprintf(e->id, sizeof(e->id),
             "sfx.%lu.%ld.%d",
             ++seq, current_time, number_range(0, 9999));

    e->volume = URANGE(1, vol, 100);

    /* Caller decides the gap; enforce a minimum of 1 tick. */
    e->delay_ticks = UMAX(1, delay_ticks);

    log_stringf("SFXQ: ENQ d=%p head=%d tail=%d next=%d cd=%d delay=%d tag=%s vol=%d id=%s file=%s",
                d,
                d->sfx_head,
                d->sfx_tail,
                next,
                d->sfx_cooldown,
                e->delay_ticks,
                e->tag,
                e->volume,
                e->id,
                e->file);

    d->sfx_tail = next;
}

/* Plays a one-shot file to everyone in a room (SFX category), scaled per player. */
void sound_play_room_file(ROOM_INDEX_DATA *room,
                          const char *file,
                          int base_vol,
                          const char *tag,
                          CHAR_DATA *except,
                          const char *id)
{
    CHAR_DATA *vch;

    if (!room || !file || !*file)
        return;

    for (vch = room->people; vch; vch = vch->next_in_room)
    {
        if (!vch->desc || !vch->desc->pProtocol)
            continue;
        if (vch == except)
            continue;
        if (IS_NPC(vch))
            continue;

        protocol_t *p = vch->desc->pProtocol;
        if (!p->bGMCP || !p->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
            continue;
        if (!vch->pcdata || !vch->pcdata->snd_enabled)
            continue;

        /* Category = SFX: apply master and sfx sliders (0 => effectively off) */
        int master = URANGE(0, vch->pcdata->snd_master, 100);
        int sfx = URANGE(0, vch->pcdata->snd_sfx, 100);
        if (master == 0 || sfx == 0)
            continue;

        /* Scale and clamp, but keep at least 1 so it’s audible if requested. */
        long scaled = (long)base_vol * master / 100;
        scaled = (long)scaled * sfx / 100;
        int vol = URANGE(1, (int)scaled, 100);

        /* Ensure relative paths resolve */
        GMCP_Media_Default(vch->desc, "https://www.dragons-domain.org/main/gui/custom/audio/");

        char unique_id[64];
        snprintf(unique_id, sizeof(unique_id),
                 "sfx.%ld.%d",
                 current_time,
                 number_range(0, 9999));

        char opts[256];
        snprintf(opts, sizeof(opts),
                 "\"id\":\"%s\",\"type\":\"sound\",\"tag\":\"%s\","
                 "\"volume\":%d,\"loops\":1,\"priority\":50,\"replace\":true",
                 unique_id,
                 (tag && *tag) ? tag : "sfx",
                 vol);

        GMCP_Media_Play(vch->desc, file, opts);
    }
}

/* Helper: enqueue a one-shot door sound so SFX play consecutively per player */
static void door_play_event_room(ROOM_INDEX_DATA *room,
                                 const char *file,
                                 int volume,
                                 const char *base_tag,
                                 const char *dname,
                                 const char *act_str,
                                 int vnum)
{
    CHAR_DATA *vch;

    if (!room || !file || !*file)
        return;

    for (vch = room->people; vch; vch = vch->next_in_room)
    {
        protocol_t *p;
        int vol_adj;
        int delay_ticks;

        if (!vch->desc || !vch->desc->pProtocol)
            continue;

        if (IS_NPC(vch))
            continue;

        p = vch->desc->pProtocol;

        if (!p->bGMCP || !p->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
            continue;

        if (!vch->pcdata || !vch->pcdata->snd_enabled)
            continue;

        vol_adj = media_apply_volume(volume, vch, "sfx", "sfx");
        if (vol_adj <= 0)
            continue;

        delay_ticks = 0;

        sfx_enqueue(vch->desc,
                    file,
                    vol_adj,
                    (base_tag && *base_tag) ? base_tag : "sfx",
                    delay_ticks);
    }
}

/* Helper: play a one-shot door sound that can overlap with others
static void door_play_event_room( ROOM_INDEX_DATA *room,
                                  const char *file,
                                  int volume,
                                  const char *base_tag,
                                  const char *dname,
                                  const char *act_str,
                                  int vnum )
{
    if (!room || !file || !*file)
        return;

    static unsigned long global_seq = 0;
    ++global_seq;

    for (CHAR_DATA *vch = room->people; vch; vch = vch->next_in_room)
    {
        if (!vch->desc || !vch->desc->pProtocol) continue;
        protocol_t *p = vch->desc->pProtocol;
        if (!p->bGMCP || !p->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA]) continue;
        if (IS_NPC(vch)) continue;

        unsigned long local_seq = ++global_seq;
        char keybuf[128];
        snprintf(keybuf, sizeof(keybuf),
                 "door.%d.%s.%s.%lu.%ld",
                 vnum, dname, act_str, local_seq, (long)(uintptr_t)vch);

        int vol_adj = media_apply_volume(volume, vch, "sfx", "sfx");
        if (vol_adj <= 0)
            continue;

        char opts[256];
        snprintf(opts, sizeof(opts),
                 "\"type\":\"sound\",\"tag\":\"%s\",\"key\":\"%s\","
                 "\"volume\":%d,\"loops\":1,\"priority\":80",
                 base_tag ? base_tag : "sfx", keybuf, vol_adj);

        GMCP_Media_Play(vch->desc, file, opts);

        log_stringf("DoorSFX: play %s key=%s vol=%d file=%s for %s",
                    act_str, keybuf, vol_adj, file, vch->name);
    }
}
*/

void media_play_door_sfx_room(ROOM_INDEX_DATA *room, int door, door_action_t act)
{
    EXIT_DATA *pexit;
    const char *act_str;
    const char *rk;

    const int DEF_OPEN_VOL = 60;
    const int DEF_CLOSE_VOL = 55;
    const int DEF_LOCK_VOL = 55;
    const int DEF_UNLOCK_VOL = 55;

    if (!room || door < 0 || door > 5)
        return;

    pexit = room->exit[door];

    act_str = (act == DOOR_ACT_OPEN) ? "OPEN" : (act == DOOR_ACT_CLOSE) ? "CLOSE"
                                            : (act == DOOR_ACT_LOCK)    ? "LOCK"
                                            : (act == DOOR_ACT_UNLOCK)  ? "UNLOCK"
                                                                        : "???";

    rk = (act == DOOR_ACT_OPEN) ? "sfx.door.open.generic" : (act == DOOR_ACT_CLOSE) ? "sfx.door.close.generic"
                                                        : (act == DOOR_ACT_LOCK)    ? "sfx.door.lock.generic"
                                                        : (act == DOOR_ACT_UNLOCK)  ? "sfx.door.unlock.generic"
                                                                                    : NULL;

    /* ----- THIS SIDE ----- */
    {
        const char *dname = directions[door].name ? directions[door].name : "dir";

        log_stringf("DoorSFX: room=%d dir=%s action=%s",
                    room->vnum, dname, act_str);

        /* 1) Per-exit override */
        if (pexit != NULL)
        {
            const char *file = NULL;
            int vol = 0;

            switch (act)
            {
            case DOOR_ACT_OPEN:
                file = pexit->sfx_open;
                vol = (pexit->sfx_open_vol > 0) ? pexit->sfx_open_vol : DEF_OPEN_VOL;
                break;
            case DOOR_ACT_CLOSE:
                file = pexit->sfx_close;
                vol = (pexit->sfx_close_vol > 0) ? pexit->sfx_close_vol : DEF_CLOSE_VOL;
                break;
            case DOOR_ACT_LOCK:
                file = pexit->sfx_lock;
                vol = (pexit->sfx_lock_vol > 0) ? pexit->sfx_lock_vol : DEF_LOCK_VOL;
                break;
            case DOOR_ACT_UNLOCK:
                file = pexit->sfx_unlock;
                vol = (pexit->sfx_unlock_vol > 0) ? pexit->sfx_unlock_vol : DEF_UNLOCK_VOL;
                break;
            default:
                break;
            }

            if (file && *file)
            {
                log_stringf("DoorSFX: OVERRIDE %s file=%s vol=%d room=%d",
                            act_str, file, vol, room->vnum);
                door_play_event_room(room, file, vol, "sfx", dname, act_str, room->vnum);
                goto mirror_side;
            }
        }

        /* 2) Registry fallback */
        if (rk != NULL)
        {
            const sound_event_def *ev = sound_event_lookup(rk);
            int defv = (act == DOOR_ACT_OPEN) ? DEF_OPEN_VOL : (act == DOOR_ACT_CLOSE) ? DEF_CLOSE_VOL
                                                           : (act == DOOR_ACT_LOCK)    ? DEF_LOCK_VOL
                                                           : (act == DOOR_ACT_UNLOCK)  ? DEF_UNLOCK_VOL
                                                                                       : 50;
            int vol = (ev && ev->default_volume > 0) ? ev->default_volume : defv;
            const char *file = (ev && ev->files[0] && *ev->files[0]) ? ev->files[0] : NULL;
            const char *tag = (ev && ev->tag && *ev->tag) ? ev->tag : "sfx";

            if (file != NULL)
            {
                log_stringf("DoorSFX: FALLBACK key=%s file=%s vol=%d room=%d",
                            rk, file, vol, room->vnum);
                door_play_event_room(room, file, vol, tag, dname, act_str, room->vnum);
                goto mirror_side;
            }

            log_stringf("DoorSFX: FALLBACK MISSING key=%s vol=%d room=%d",
                        rk, vol, room->vnum);
        }
    }

mirror_side:
    /* ----- FAR SIDE (mirror) ----- */
    if (pexit && pexit->to_room)
    {
        ROOM_INDEX_DATA *other = pexit->to_room;
        const int rev = directions[door].reverse;

        if (rev >= 0 && rev <= 5)
        {
            EXIT_DATA *prev = other->exit[rev];
            const char *dname_rev = directions[rev].name ? directions[rev].name : "dir";

            log_stringf("DoorSFX: mirror room=%d dir=%s action=%s",
                        other->vnum, dname_rev, act_str);

            /* 1) Far-side override */
            if (prev)
            {
                const char *file = NULL;
                int vol = 0;

                switch (act)
                {
                case DOOR_ACT_OPEN:
                    file = prev->sfx_open;
                    vol = (prev->sfx_open_vol > 0) ? prev->sfx_open_vol : DEF_OPEN_VOL;
                    break;
                case DOOR_ACT_CLOSE:
                    file = prev->sfx_close;
                    vol = (prev->sfx_close_vol > 0) ? prev->sfx_close_vol : DEF_CLOSE_VOL;
                    break;
                case DOOR_ACT_LOCK:
                    file = prev->sfx_lock;
                    vol = (prev->sfx_lock_vol > 0) ? prev->sfx_lock_vol : DEF_LOCK_VOL;
                    break;
                case DOOR_ACT_UNLOCK:
                    file = prev->sfx_unlock;
                    vol = (prev->sfx_unlock_vol > 0) ? prev->sfx_unlock_vol : DEF_UNLOCK_VOL;
                    break;
                default:
                    break;
                }

                if (file && *file)
                {
                    log_stringf("DoorSFX: mirror OVERRIDE %s file=%s vol=%d room=%d",
                                act_str, file, vol, other->vnum);
                    door_play_event_room(other, file, vol, "sfx", dname_rev, act_str, other->vnum);
                    return;
                }
            }

            /* 2) Far-side fallback */
            if (rk != NULL)
            {
                const sound_event_def *ev = sound_event_lookup(rk);
                int defv = (act == DOOR_ACT_OPEN) ? DEF_OPEN_VOL : (act == DOOR_ACT_CLOSE) ? DEF_CLOSE_VOL
                                                               : (act == DOOR_ACT_LOCK)    ? DEF_LOCK_VOL
                                                               : (act == DOOR_ACT_UNLOCK)  ? DEF_UNLOCK_VOL
                                                                                           : 50;
                int vol = (ev && ev->default_volume > 0) ? ev->default_volume : defv;
                const char *file = (ev && ev->files[0] && *ev->files[0]) ? ev->files[0] : NULL;
                const char *tag = (ev && ev->tag && *ev->tag) ? ev->tag : "sfx";

                if (file != NULL)
                {
                    log_stringf("DoorSFX: mirror FALLBACK key=%s file=%s vol=%d room=%d",
                                rk, file, vol, other->vnum);
                    door_play_event_room(other, file, vol, tag, dname_rev, act_str, other->vnum);
                    return;
                }

                log_stringf("DoorSFX: mirror FALLBACK MISSING key=%s vol=%d room=%d",
                            rk, vol, other->vnum);
            }
        }
    }
}

/* Play a consumption-related SFX (eat, drink, pill, quaff, smoke, smear). */
void media_play_consume_sfx_room(ROOM_INDEX_DATA *room, consume_action_t act, CHAR_DATA *actor)
{
    if (!room)
        return;

    const char *rk = NULL;
    const char *act_str = NULL;

    switch (act)
    {
    case CONSUME_ACT_EAT:
        rk = "sfx.consume.eat";
        act_str = "EAT";
        break;
    case CONSUME_ACT_DRINK:
        rk = "sfx.consume.drink";
        act_str = "DRINK";
        break;
    case CONSUME_ACT_PILL:
        rk = "sfx.consume.pill";
        act_str = "PILL";
        break;
    case CONSUME_ACT_QUAFF:
        rk = "sfx.consume.quaff";
        act_str = "QUAFF";
        break;
    case CONSUME_ACT_SMOKE:
        rk = "sfx.consume.smoke";
        act_str = "SMOKE";
        break;
    case CONSUME_ACT_SMEAR:
        rk = "sfx.consume.smear";
        act_str = "SMEAR";
        break;
    default:
        return;
    }

    const sound_event_def *ev = sound_event_lookup(rk);
    if (!ev || !ev->files[0] || !*ev->files[0])
    {
        log_stringf("ConsumeSFX: missing registry key=%s", rk);
        return;
    }

    int vol = (ev->default_volume > 0) ? ev->default_volume : 60;
    const char *file = ev->files[0];

    log_stringf("ConsumeSFX: action=%s file=%s vol=%d room=%d",
                act_str, file, vol, room->vnum);

    /* broadcast to everyone in the room */
    sound_play_room_file(room, file, vol, (ev->tag ? ev->tag : "sfx"), NULL, rk);
}

/* Internal: pick a file variant by weights (0 => equal weight). */
static const char *sound_pick_file(const sound_event_def *def)
{
    if (!def || !def->files[0])
        return NULL;

    /* Count options */
    int n = 0;
    for (; n < 6 && def->files[n]; ++n)
        ;

    /* Sum weights (if all zero, treat as 1 each) */
    int total = 0;
    bool any = FALSE;
    for (int i = 0; i < n; ++i)
    {
        total += def->weights[i];
        if (def->weights[i] > 0)
            any = TRUE;
    }
    if (!any)
        total = n; /* equal weights */

    int roll = number_range(1, UMAX(1, total));
    int acc = 0;

    for (int i = 0; i < n; ++i)
    {
        int w = any ? def->weights[i] : 1;
        acc += w;
        if (roll <= acc)
            return def->files[i];
    }
    return def->files[n - 1];
}

/* Ensure default base URL is set for the profile (harmless if repeated). */
static void media_default_ensure(DESCRIPTOR_DATA *d)
{
    if (!d)
        return;
    GMCP_Media_Default(d, "https://www.dragons-domain.org/main/gui/custom/audio/");
}

/* Play a one-shot sound to a single descriptor, honoring master volume. */
static void sound_play_to_desc(DESCRIPTOR_DATA *d, const sound_event_def *def, int vol_override)
{
    if (!d || !d->pProtocol || !d->pProtocol->bGMCP || !d->pProtocol->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
        return;

    const char *name = sound_pick_file(def);
    if (!name)
        return;

    media_default_ensure(d);

    int vol = (vol_override > 0) ? vol_override : def->default_volume;
    /* We can’t scale without a CHAR_DATA; leave raw for pure descriptors. */
    vol = URANGE(1, vol, 100);

    char opts[256];
    /* One-shots: type:"sound", tag from registry, loops from registry (usually 1).
       We do NOT set a key by default so overlapping repeats are allowed;
       for mutually-exclusive effects you can add a key later per event. */
    snprintf(opts, sizeof(opts),
             "\"type\":\"sound\",\"tag\":\"%s\",\"volume\":%d,\"loops\":%d",
             def->tag ? def->tag : "fx", vol, (def->loops > 0 ? def->loops : 1));

    GMCP_Media_Play(d, name, opts);
}

/* Public: play to one player */
void sound_emit_char(CHAR_DATA *ch, const char *event_key, int vol_override)
{
    if (!ch || !ch->desc)
        return;

    const sound_event_def *def = sound_event_lookup(event_key);
    if (!def)
        return;

    int vol = (vol_override > 0) ? vol_override : def->default_volume;
    vol = media_apply_volume(vol, ch, def->tag, "sound");
    if (vol <= 0)
        return;

    /* Clone def for this call so we can pass the scaled volume cleanly */
    sound_event_def tmp = *def;
    tmp.default_volume = vol;

    sound_play_to_desc(ch->desc, &tmp, -1);
}

/* Public: play to everyone in a room (except optional 'except') */
void sound_emit_room(ROOM_INDEX_DATA *room, const char *event_key, int vol_override, CHAR_DATA *except)
{
    if (!room)
        return;

    const sound_event_def *def = sound_event_lookup(event_key);
    if (!def)
        return;

    for (CHAR_DATA *to = room->people; to; to = to->next_in_room)
    {
        if (!to->desc || to == except)
            continue;

        int vol = (vol_override > 0) ? vol_override : def->default_volume;
        vol = media_apply_volume(vol, to, def->tag, "sound");
        if (vol <= 0)
            continue;

        sound_event_def tmp = *def;
        tmp.default_volume = vol;

        sound_play_to_desc(to->desc, &tmp, -1);
    }
}

/* Re-assert environmental media for a player where they are now.
 * If force == TRUE, treat as changed and (re)send the correct lanes
 * even if our cached proto state looks identical.
 */
void media_env_refresh(CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool force)
{
    if (!ch || !room || !ch->desc || !ch->desc->pProtocol)
        return;

    protocol_t *proto = ch->desc->pProtocol;

    /* Must have GMCP Client.Media and not be suppressed */
    if (!proto->bGMCP || !proto->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
        return;
    if (proto->MediaSuppress)
        return;

    /* All relative media names resolve under this base */
    GMCP_Media_Default(ch->desc, "https://www.dragons-domain.org/main/gui/custom/audio/");

/* Stable keys for our lanes */
#define KEY_AREA "dd.ambient.area"
#define KEY_ROOM "dd.ambient.room"
#define KEY_SECT_A "dd.ambient.sector.A"
#define KEY_SECT_B "dd.ambient.sector.B"

    /* --- Desired names/volumes (room > area > sector) ---------------------- */
    const char *area_name = NULL;
    int area_vol = 0;
    if (room->area && room->area->ambient_sound && *room->area->ambient_sound && room->area->ambient_volume > 0)
    {
        area_name = room->area->ambient_sound;
        area_vol = URANGE(1, room->area->ambient_volume, 100);

        /* APPLY PLAYER SETTINGS */
        area_vol = media_apply_volume(area_vol, ch, "environment", "music");
        if (area_vol <= 0)
        {
            area_name = NULL;
        } /* treat as no area track */
    }

    const char *room_name = NULL;
    int room_vol = 0;
    if (room->ambient_sound && *room->ambient_sound && room->ambient_volume > 0)
    {
        room_name = room->ambient_sound;
        room_vol = URANGE(1, room->ambient_volume, 100);

        /* APPLY PLAYER SETTINGS */
        room_vol = media_apply_volume(room_vol, ch, "environment", "music");
        if (room_vol <= 0)
        {
            room_name = NULL;
        } /* treat as no room track */
    }

    /* Sector fallback only if no room and no area */
    const char *sect_name = NULL;
    int sect_vol = 0;
    if (!room_name && !area_name)
    {
        const sector_ambience_t *sa = sector_ambience_for(room->sector_type);
        if (sa && sa->name && *sa->name && sa->volume > 0)
        {
            sect_name = sa->name;
            sect_vol = URANGE(1, sa->volume, 100);

            /* APPLY PLAYER SETTINGS */
            sect_vol = media_apply_volume(sect_vol, ch, "environment", "music");
            if (sect_vol <= 0)
            {
                sect_name = NULL;
            } /* treat as no sector track */
        }
    }

    /* ---------------- Sector lane (A/B crossfade) -------------------------- */
    if (sect_name)
    {
        const char *newKey = (proto->MediaSectorFlip ? KEY_SECT_B : KEY_SECT_A);
        const char *oldKey = (proto->MediaSectorFlip ? KEY_SECT_A : KEY_SECT_B);

        bool changed = force || !proto->MediaSectorActive || !proto->MediaSectorName || str_cmp(sect_name, proto->MediaSectorName) || proto->MediaSectorVol != sect_vol;

        if (changed)
        {
            char play_opts[256];
            snprintf(play_opts, sizeof(play_opts),
                     "\"type\":\"music\",\"tag\":\"environment\",\"key\":\"%s\","
                     "\"volume\":%d,\"loops\":-1,\"continue\":true,\"fadein\":1200",
                     newKey, sect_vol);
            GMCP_Media_Play(ch->desc, sect_name, play_opts);

            /* Always ask previous sector key to fade away if we were active */
            if (proto->MediaSectorActive)
            {
                char stop_opts[192];
                snprintf(stop_opts, sizeof(stop_opts),
                         "\"key\":\"%s\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200",
                         oldKey);
                GMCP_Media_Stop(ch->desc, stop_opts);
            }

            if (proto->MediaSectorName)
            {
                free_string((char *)proto->MediaSectorName);
            }
            proto->MediaSectorName = str_dup(sect_name);
            proto->MediaSectorVol = sect_vol;
            proto->MediaSectorActive = TRUE;
            proto->MediaSectorFlip = !proto->MediaSectorFlip;
        }
    }
    else
    {
        /* Room or Area present, or nothing at all: silence the sector lane */
        GMCP_Media_Stop(ch->desc, "\"key\":\"" KEY_SECT_A "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
        GMCP_Media_Stop(ch->desc, "\"key\":\"" KEY_SECT_B "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
        if (proto->MediaSectorName)
        {
            free_string((char *)proto->MediaSectorName);
            proto->MediaSectorName = NULL;
        }
        proto->MediaSectorVol = 0;
        proto->MediaSectorActive = FALSE;
        /* keep flip as-is */
    }

    /* ---------------- Room lane (room OR sector fallback) ------------------ */
    if (room_name || sect_name)
    {
        const char *want_name = room_name ? room_name : sect_name;
        const int want_vol = room_name ? room_vol : sect_vol;

        bool changed = force || !proto->MediaRoomActive || !proto->MediaRoomName || str_cmp(want_name, proto->MediaRoomName) || proto->MediaRoomVol != want_vol;

        if (changed)
        {
            /* Fade away previous room lane if name changed, to avoid a pop */
            if (proto->MediaRoomActive && proto->MediaRoomName && str_cmp(want_name, proto->MediaRoomName))
            {
                GMCP_Media_Stop(ch->desc,
                                "\"key\":\"" KEY_ROOM "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
                if (proto->MediaRoomName)
                {
                    free_string((char *)proto->MediaRoomName);
                    proto->MediaRoomName = NULL;
                }
                proto->MediaRoomActive = FALSE;
                proto->MediaRoomVol = 0;
            }

            char opts_room[256];
            snprintf(opts_room, sizeof(opts_room),
                     "\"type\":\"music\",\"tag\":\"environment\",\"key\":\"" KEY_ROOM "\","
                     "\"volume\":%d,\"loops\":-1,\"continue\":true,\"fadein\":1200",
                     want_vol);
            GMCP_Media_Play(ch->desc, want_name, opts_room);

            if (proto->MediaRoomName)
                free_string((char *)proto->MediaRoomName);
            proto->MediaRoomName = str_dup(want_name);
            proto->MediaRoomVol = want_vol;
            proto->MediaRoomActive = TRUE;
        }

        /* Room/sector overrides area — unconditionally silence area lane */
        GMCP_Media_Stop(ch->desc,
                        "\"key\":\"" KEY_AREA "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
        if (proto->MediaAreaName)
        {
            free_string((char *)proto->MediaAreaName);
            proto->MediaAreaName = NULL;
        }
        proto->MediaAreaVol = 0;
        proto->MediaAreaActive = FALSE;
    }
    else
    {
        /* No room/sector — unconditionally silence the room lane */
        GMCP_Media_Stop(ch->desc,
                        "\"key\":\"" KEY_ROOM "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
        if (proto->MediaRoomName)
        {
            free_string((char *)proto->MediaRoomName);
            proto->MediaRoomName = NULL;
        }
        proto->MediaRoomVol = 0;
        proto->MediaRoomActive = FALSE;
    }

    /* ---------------- Area lane (only when no room/sector override) -------- */
    if (!room_name && !sect_name)
    {
        if (area_name)
        {
            bool changed = force || !proto->MediaAreaActive || !proto->MediaAreaName || str_cmp(area_name, proto->MediaAreaName) || proto->MediaAreaVol != area_vol;

            if (changed)
            {
                char opts_area[256];
                snprintf(opts_area, sizeof(opts_area),
                         "\"type\":\"music\",\"tag\":\"environment\",\"key\":\"" KEY_AREA "\","
                         "\"volume\":%d,\"loops\":-1,\"continue\":true,\"fadein\":1200",
                         area_vol);
                GMCP_Media_Play(ch->desc, area_name, opts_area);

                if (proto->MediaAreaName)
                    free_string((char *)proto->MediaAreaName);
                proto->MediaAreaName = str_dup(area_name);
                proto->MediaAreaVol = area_vol;
                proto->MediaAreaActive = TRUE;
            }
        }
        else
        {
            /* No area ambience defined: ensure any area track is off */
            GMCP_Media_Stop(ch->desc,
                            "\"key\":\"" KEY_AREA "\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":1200");
            if (proto->MediaAreaName)
            {
                free_string((char *)proto->MediaAreaName);
                proto->MediaAreaName = NULL;
            }
            proto->MediaAreaVol = 0;
            proto->MediaAreaActive = FALSE;
        }
    }

#undef KEY_AREA
#undef KEY_ROOM
#undef KEY_SECT_A
#undef KEY_SECT_B

    /* also refresh the weather ambience layer */
    if (ch->pcdata && ch->pcdata->snd_enabled && ch->pcdata->snd_env > 0)
    {
        /* Re-assert current weather ambience immediately */
        update_weather_for_char(ch);
    }
}

/* ---- Level-up sound broadcast via registry -------------------------------- */

static const char *snd_pick_from_event(const sound_event_def *ev)
{
    int i, n = 0, total = 0;
    if (!ev)
        return NULL;

    /* Count valid files and sum positive weights */
    for (i = 0; i < 6 && ev->files[i]; ++i)
    {
        ++n;
        if (ev->weights[i] > 0)
            total += ev->weights[i];
    }
    if (n == 0)
        return NULL;

    if (total <= 0)
    {
        /* No weights -> uniform choice */
        int idx = number_range(0, n - 1);
        return ev->files[idx];
    }
    else
    {
        /* Weighted choice */
        int roll = number_range(1, total), acc = 0;
        for (i = 0; i < n; ++i)
        {
            int w = (ev->weights[i] > 0 ? ev->weights[i] : 0);
            acc += w;
            if (roll <= acc)
                return ev->files[i];
        }
        return ev->files[0]; /* fallback */
    }
}

void media_notify_levelup(CHAR_DATA *who)
{
    const sound_event_def *ev = sound_event_lookup("notify.levelup");
    const char *name = ev ? snd_pick_from_event(ev) : NULL;
    if (!name)
        return;

    /* 1) Optional: play to the levelling player as well */
    if (who && who->desc && who->desc->pProtocol && who->desc->pProtocol->bGMCP && who->desc->pProtocol->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA] && !who->desc->pProtocol->MediaSuppress && who->pcdata && who->pcdata->snd_enabled)
    {
        int vol = media_apply_master_volume(ev->default_volume, who);
        if (vol > 0)
        {
            GMCP_Media_Default(who->desc, "https://www.dragons-domain.org/main/gui/custom/audio/");
            /* one-shot sound; tag is ev->tag (e.g., "notify") */
            char opts[192];
            snprintf(opts, sizeof(opts),
                     "\"type\":\"sound\",\"tag\":\"%s\",\"key\":\"dd.notify.levelup\","
                     "\"volume\":%d,\"priority\":80",
                     (ev->tag ? ev->tag : "notify"), vol);
            GMCP_Media_Play(who->desc, name, opts);
        }
    }

    /* 2) Broadcast to everyone who would receive INFO per talk_channel rules */
    {
        DESCRIPTOR_DATA *d;
        for (d = descriptor_list; d; d = d->next)
        {
            if (d->connected != CON_PLAYING)
                continue;
            if (!d->character)
                continue;
            if (d->character == who)
                continue; /* mirror talk_channel: skip sender */

            CHAR_DATA *vch = d->character;
            CHAR_DATA *och = d->original ? d->original : d->character;

            if (!receives_channel_info(och))
                continue;
            if (!d->pProtocol || !d->pProtocol->bGMCP)
                continue;
            if (!d->pProtocol->bGMCPSupport[GMCP_SUPPORT_CLIENT_MEDIA])
                continue;
            if (d->pProtocol->MediaSuppress)
                continue;
            if (!vch->pcdata || !vch->pcdata->snd_enabled)
                continue;

            {
                int vol = media_apply_master_volume(ev->default_volume, vch);
                if (vol <= 0)
                    continue;

                GMCP_Media_Default(d, "https://www.dragons-domain.org/main/gui/custom/audio/");
                char opts[192];
                snprintf(opts, sizeof(opts),
                         "\"type\":\"sound\",\"tag\":\"%s\",\"key\":\"dd.notify.levelup\","
                         "\"volume\":%d,\"priority\":80",
                         (ev->tag ? ev->tag : "notify"), vol);
                GMCP_Media_Play(d, name, opts);
            }
        }
    }
}

void do_sconfig(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
        return;

    /* Ensure sensible one-time defaults if zeroed/unset */
    if (ch->pcdata->snd_master == 0 &&
        ch->pcdata->snd_env == 0 &&
        ch->pcdata->snd_music == 0 &&
        ch->pcdata->snd_foley == 0 &&
        ch->pcdata->snd_sfx == 0 &&
        ch->pcdata->snd_ui == 0 &&
        ch->pcdata->snd_notify == 0)
    {
        ch->pcdata->snd_enabled = SND_DEF_ENABLED;
        ch->pcdata->snd_master = SND_DEF_MASTER;
        ch->pcdata->snd_env = SND_DEF_ENV;
        ch->pcdata->snd_music = SND_DEF_MUSIC;
        ch->pcdata->snd_foley = SND_DEF_FOLEY;
        ch->pcdata->snd_sfx = SND_DEF_SFX;
        ch->pcdata->snd_ui = SND_DEF_UI;
        ch->pcdata->snd_notify = SND_DEF_NOTIFY;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    /* No args or 'status' -> show current settings */
    if (arg1[0] == '\0' || !str_cmp(arg1, "status"))
    {
        send_to_char("<14>[<0> <15>Keyword<0>  <14>]<0> <15>Option<0>\n\r-------------------\n\r", ch);

        send_to_char(ch->pcdata->snd_enabled
                         ? "<6>[<40>+<0><556>SOUND<0>    <6>]<0> Sound is on.\n\r"
                         : "<6>[<196>-<0><246>sound<0>    <6>]<0> Sound is off.\n\r",
                     ch);

        const int master = URANGE(0, ch->pcdata->snd_master, 100);
        const bool snd_on = (ch->pcdata->snd_enabled && master > 0);

        const int v_env = URANGE(0, ch->pcdata->snd_env, 100);
        const int v_music = URANGE(0, ch->pcdata->snd_music, 100);
        const int v_foley = URANGE(0, ch->pcdata->snd_foley, 100);
        const int v_sfx = URANGE(0, ch->pcdata->snd_sfx, 100);
        const int v_ui = URANGE(0, ch->pcdata->snd_ui, 100);
        const int v_notify = URANGE(0, ch->pcdata->snd_notify, 100);

        const bool on_env = snd_on && v_env > 0;
        const bool on_music = snd_on && v_music > 0;
        const bool on_foley = snd_on && v_foley > 0;
        const bool on_sfx = snd_on && v_sfx > 0;
        const bool on_ui = snd_on && v_ui > 0;
        const bool on_notify = snd_on && v_notify > 0;

        char line[MAX_STRING_LENGTH];

        sprintf(line, "<6>[<40>+<0><556>MASTER<0>   <6>]<0> Master volume:      {W%3d{x\n\r", master);
        send_to_char(line, ch);

        if (on_env)
            sprintf(line, "<6>[<40>+<0><556>AMBIENT<0>  <6>]<0> Environment volume: {W%3d{x\n\r", v_env);
        else
            sprintf(line, "<6>[<196>-<0><246>ambient<0>  <6>]<0> Ambient sound is off.\n\r");
        send_to_char(line, ch);

        if (on_music)
            sprintf(line, "<6>[<40>+<0><556>MUSIC<0>    <6>]<0> Music volume:       {W%3d{x\n\r", v_music);
        else
            sprintf(line, "<6>[<196>-<0><246>music<0>    <6>]<0> Music is off.\n\r");
        send_to_char(line, ch);

        if (on_foley)
            sprintf(line, "<6>[<40>+<0><556>FOLEY<0>    <6>]<0> Foley volume:       {W%3d{x\n\r", v_foley);
        else
            sprintf(line, "<6>[<196>-<0><246>foley<0>    <6>]<0> Foley is off.\n\r");
        send_to_char(line, ch);

        if (on_sfx)
            sprintf(line, "<6>[<40>+<0><556>SFX<0>      <6>]<0> SFX volume:         {W%3d{x\n\r", v_sfx);
        else
            sprintf(line, "<6>[<196>-<0><246>sfx<0>      <6>]<0> SFX is off.\n\r");
        send_to_char(line, ch);

        if (on_ui)
            sprintf(line, "<6>[<40>+<0><556>UI<0>       <6>]<0> UI volume:          {W%3d{x\n\r", v_ui);
        else
            sprintf(line, "<6>[<196>-<0><246>ui<0>       <6>]<0> UI sounds are off.\n\r");
        send_to_char(line, ch);

        if (on_notify)
            sprintf(line, "<6>[<40>+<0><556>NOTIFY<0>   <6>]<0> Notifications:      {W%3d{x\n\r", v_notify);
        else
            sprintf(line, "<6>[<196>-<0><246>notify<0>   <6>]<0> Notification sounds are off.\n\r");
        send_to_char(line, ch);

        send_to_char("\n\rShortcuts: +sound/-sound, +ambient/-ambient, +notify/-notify\n\r", ch);
        send_to_char("Setters:   master/ambient/music/foley/sfx/ui/notify <<0..100>\n\r", ch);
        return;
    }

    /* +sound / -sound toggles */
    if (arg1[0] == '+' || arg1[0] == '-')
    {
        bool set = (arg1[0] == '+');

        if (!str_cmp(arg1 + 1, "sound"))
        {
            ch->pcdata->snd_enabled = set ? TRUE : FALSE;

            if (!set && ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
            {
                /* Fade everything out politely if turning sound off */
                GMCP_Media_Stop(ch->desc, "\"fadeaway\":true,\"fadeout\":800");

                /* ALSO clear all cached media state so +sound will fully republish */
                {
                    protocol_t *p = ch->desc->pProtocol;

                    /* Weather */
                    if (p->MediaWeatherName)
                    {
                        free_string(p->MediaWeatherName);
                    }
                    p->MediaWeatherName = NULL;
                    p->MediaWeatherVol = 0;
                    p->MediaWeatherActive = FALSE;

                    /* Sector (A/B lane) */
                    if (p->MediaSectorName)
                    {
                        free_string((char *)p->MediaSectorName);
                    }
                    p->MediaSectorName = NULL;
                    p->MediaSectorVol = 0;
                    p->MediaSectorActive = FALSE;
                    /* keep flip state or reset as you prefer; resetting is fine: */
                    p->MediaSectorFlip = FALSE;

                    /* Room */
                    if (p->MediaRoomName)
                    {
                        free_string((char *)p->MediaRoomName);
                    }
                    p->MediaRoomName = NULL;
                    p->MediaRoomVol = 0;
                    p->MediaRoomActive = FALSE;

                    /* Area */
                    if (p->MediaAreaName)
                    {
                        free_string((char *)p->MediaAreaName);
                    }
                    p->MediaAreaName = NULL;
                    p->MediaAreaVol = 0;
                    p->MediaAreaActive = FALSE;
                }
            }

            if (set && ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
            {
                /* Force a full re-assert when turning sound back on */
                ch->desc->pProtocol->MediaSuppress = FALSE;
                media_env_refresh(ch, ch->in_room, TRUE);
                update_weather_for_char(ch);
            }

            send_to_char(set ? "Sound is now {GON{x.\n\r" : "Sound is now {ROFF{x.\n\r", ch);
            return;
        }
        else if (!str_cmp(arg1 + 1, "ambient"))
        {
            if (set)
            {
                if (ch->pcdata->snd_env == 0)
                    ch->pcdata->snd_env = SND_DEF_ENV;

                if (ch->pcdata->snd_enabled && ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
                {
                    media_env_refresh(ch, ch->in_room, TRUE);
                    update_weather_for_char(ch);
                }

                send_to_char("Ambient/environment sound is now {GON{x.\n\r", ch);
                log_stringf("WeatherSFX: reassert after sconfig for %s", ch->name);
            }
            else
            {
                ch->pcdata->snd_env = 0;

                if (ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
                {
                    /* Stop ambient lanes (room/area/sector/weather) */
                    GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.room\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                    GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.area\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                    GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.sector.A\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                    GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.sector.B\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                    GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.weather\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");

                    /* ALSO clear our weather cache so a later toggle ON will republish */
                    {
                        protocol_t *p = ch->desc->pProtocol;
                        if (p)
                        {
                            if (p->MediaWeatherName)
                            {
                                free_string(p->MediaWeatherName);
                            }
                            p->MediaWeatherName = NULL;
                            p->MediaWeatherVol = 0;
                            p->MediaWeatherActive = FALSE;
                        }
                    }
                }
                send_to_char("Ambient/environment sound is now {ROFF{x.\n\r", ch);
            }
            return;
        }
        else if (!str_cmp(arg1 + 1, "notify"))
        {
            if (set)
            {
                if (ch->pcdata->snd_notify == 0)
                    ch->pcdata->snd_notify = SND_DEF_NOTIFY;
                send_to_char("Notifications are now {GON{x.\n\r", ch);
            }
            else
            {
                ch->pcdata->snd_notify = 0;
                send_to_char("Notifications are now {ROFF{x.\n\r", ch);
            }
            return;
        }

        send_to_char("Use +sound/-sound, +ambient/-ambient, or +notify/-notify.\n\r", ch);
        return;
    }

    /* value setters */
    {
        int *slot = NULL;
        const char *which = arg1;

        if (!str_cmp(arg1, "master"))
            slot = &ch->pcdata->snd_master;
        else if (!str_cmp(arg1, "ambient") || !str_cmp(arg1, "env") || !str_cmp(arg1, "ambience"))
            slot = &ch->pcdata->snd_env;
        else if (!str_cmp(arg1, "music"))
            slot = &ch->pcdata->snd_music;
        else if (!str_cmp(arg1, "foley"))
            slot = &ch->pcdata->snd_foley;
        else if (!str_cmp(arg1, "sfx"))
            slot = &ch->pcdata->snd_sfx;
        else if (!str_cmp(arg1, "ui"))
            slot = &ch->pcdata->snd_ui;
        else if (!str_cmp(arg1, "notify"))
            slot = &ch->pcdata->snd_notify;

        if (!slot)
        {
            send_to_char("Syntax:\n\r", ch);
            send_to_char("  sconfig status\n\r", ch);
            send_to_char("  +sound | -sound\n\r", ch);
            send_to_char("  +ambient | -ambient\n\r", ch);
            send_to_char("  +notify  | -notify\n\r", ch);
            send_to_char("  master/ambient/music/foley/sfx/ui/notify <0..100>\n\r", ch);
            return;
        }

        if (arg2[0] == '\0')
        {
            char buf[MAX_STRING_LENGTH];
            sprintf(buf, "%s is currently {W%3d{x.\n\r", which, URANGE(0, *slot, 100));
            send_to_char(buf, ch);
            return;
        }

        *slot = URANGE(0, atoi(arg2), 100);

        {
            char buf[MAX_STRING_LENGTH];
            sprintf(buf, "%s set to {W%3d{x.\n\r", which, *slot);
            send_to_char(buf, ch);
        }

        if ((!ch->pcdata->snd_enabled || ch->pcdata->snd_master == 0) &&
            ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
        {
            GMCP_Media_Stop(ch->desc, "\"fadeaway\":true,\"fadeout\":800");
            return;
        }

        if ((slot == &ch->pcdata->snd_env) && ch->desc && ch->desc->pProtocol && ch->desc->pProtocol->bGMCP)
        {
            if (*slot == 0)
            {
                /* Turning ambient down to 0 -> stop ambient AND weather */
                GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.room\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.area\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.sector.A\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.sector.B\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");
                GMCP_Media_Stop(ch->desc, "\"key\":\"dd.ambient.weather\",\"type\":\"music\",\"fadeaway\":true,\"fadeout\":700");

                /* Clear weather cache so future ON will republish */
                {
                    protocol_t *p = ch->desc->pProtocol;
                    if (p)
                    {
                        if (p->MediaWeatherName)
                        {
                            free_string(p->MediaWeatherName);
                        }
                        p->MediaWeatherName = NULL;
                        p->MediaWeatherVol = 0;
                        p->MediaWeatherActive = FALSE;
                    }
                }
            }
            else
            {
                media_env_refresh(ch, ch->in_room, TRUE);
                update_weather_for_char(ch);
            }
        }
    }
}

/* Stub function for SFX updates - called from game loop */
void sound_sfx_update(void)
{
    /* Currently a no-op. Could be used for processing queued SFX or cleanup */
    return;
}
