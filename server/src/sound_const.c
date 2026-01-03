#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "sound.h"


/* Default ambience per sector (edit names/volumes to your files). */
const sector_ambience_t sector_ambience_defaults[SECT_MAX] =
{
    { "ambient/sector/inside.mp3",            20 },     /* SECT_INSIDE             0  */
    { "ambient/sector/city.mp3",              25 },     /* SECT_CITY               1  */
    { "ambient/sector/field.mp3",             25 },     /* SECT_FIELD              2  */
    { "ambient/sector/forest.mp3",            30 },     /* SECT_FOREST             3  */
    { "ambient/sector/hills.mp3",             25 },     /* SECT_HILLS              4  */
    { "ambient/sector/mountain.mp3",          30 },     /* SECT_MOUNTAIN           5  */
    { "ambient/sector/water_swim.mp3",        35 },     /* SECT_WATER_SWIM         6  */
    { "ambient/sector/water_noswim.mp3",      35 },     /* SECT_WATER_NOSWIM       7  */
    { "ambient/sector/underwater.mp3",        20 },     /* SECT_UNDERWATER         8  */
    { "ambient/sector/air.mp3",               35 },     /* SECT_AIR                9  */
    { "ambient/sector/desert.mp3",            30 },     /* SECT_DESERT             10 */
    { "ambient/sector/swamp.mp3",             20 },     /* SECT_SWAMP              11 */
    { "ambient/sector/underwater_ground.mp3", 25 },     /* SECT_UNDERWATER_GROUND  12 */
};

/* Safe lookup */
const sector_ambience_t *sector_ambience_for(int sector)
{
    if (sector < 0 || sector >= SECT_MAX) return NULL;
    const sector_ambience_t *sa = &sector_ambience_defaults[sector];
    if (!sa->name || !*sa->name || sa->volume <= 0) return NULL;
    return sa;
}

/* --- Sound event registry ----------------------------------------------- */

static const sound_event_def sound_events[] = {
    /* Notifications */
    { "notify.levelup",          { "notify/levelup1.mp3",         NULL }, { 0,0 }, 70, "notify", 1 },
    { "notify.channel.dirtalk",  { "notify/channel_dirtalk.mp3",  NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.immtalk",  { "notify/channel_immtalk.mp3",  NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.chat",     { "notify/channel_chat.mp3",     NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.auction",  { "notify/channel_auction.mp3",  NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.info",     { "notify/channel_info.mp3",     NULL }, { 0,0 }, 55, "notify", 1 },
    { "notify.channel.clan",     { "notify/channel_clan.mp3",     NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.shout",    { "notify/channel_shout.mp3",    NULL }, { 0,0 }, 65, "notify", 1 },
    { "notify.channel.music",    { "notify/channel_music.mp3",    NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.question", { "notify/channel_question.mp3", NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.yell",     { "notify/channel_yell.mp3",     NULL }, { 0,0 }, 65, "notify", 1 },
    { "notify.channel.server",   { "notify/channel_server.mp3",   NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.channel.arena",    { "notify/channel_arena.mp3",    NULL }, { 0,0 }, 65, "notify", 1 },
    { "notify.channel.newbie",   { "notify/channel_newbie.mp3",   NULL }, { 0,0 }, 60, "notify", 1 },
    { "notify.auction.start",    { "notify/auction_start.mp3",    NULL }, { 0,0 }, 65, "notify", 1 },

    /* SFX */
    { "sfx.door.open.generic",   { "sfx/door/open_generic.mp3",   NULL }, { 0,0 }, 60, "sfx"   , 1 },
    { "sfx.door.close.generic",  { "sfx/door/close_generic.mp3",  NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.door.lock.generic",   { "sfx/door/lock_generic.mp3",   NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.door.unlock.generic", { "sfx/door/unlock_generic.mp3", NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.consume.eat",         { "sfx/misc/eat_generic.mp3",    NULL }, { 0,0 }, 65, "sfx"   , 1 },
    { "sfx.consume.drink",       { "sfx/misc/drink_generic.mp3",  NULL }, { 0,0 }, 65, "sfx"   , 1 },
    { "sfx.consume.pill",        { "sfx/misc/pill_generic.mp3",   NULL }, { 0,0 }, 65, "sfx"   , 1 },
    { "sfx.consume.quaff",       { "sfx/misc/quaff_generic.mp3",  NULL }, { 0,0 }, 65, "sfx"   , 1 },
    { "sfx.consume.smoke",       { "sfx/misc/smoke_generic.mp3",  NULL }, { 0,0 }, 65, "sfx"   , 1 },
    { "sfx.consume.smear",       { "sfx/misc/smear_generic.mp3",  NULL }, { 0,0 }, 65, "sfx"   , 1 },

    /* Weather */
    { "ambient.weather.rain",      { "ambient/weather/rain_loop1.mp3", NULL }, {0,0}, 15, "ambient.weather", 1 },
    { "ambient.weather.lightning", { "ambient/weather/lightning_loop1.mp3", NULL }, {0,0}, 15, "ambient.weather", 1 },



    /* Sentinel */
    { NULL, { NULL }, { 0 }, 0, NULL, 0 }
};

const sound_event_def *sound_event_lookup(const char *key)
{
    if (!key || !*key) return NULL;
    for (int i = 0; sound_events[i].key; ++i) {
        if (!str_cmp(key, sound_events[i].key))
            return &sound_events[i];
    }
    return NULL;
}

/*---END SOUND STUFF---*/
