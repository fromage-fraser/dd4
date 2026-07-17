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

#define FOOTSTEP_TERRAIN_SET(gait) \
    { "sfx.foley.step." #gait ".inside",            { "sfx/foley/step/" #gait "_inside.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".city",              { "sfx/foley/step/" #gait "_city.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".field",             { "sfx/foley/step/" #gait "_field.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".forest",            { "sfx/foley/step/" #gait "_forest.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".hills",             { "sfx/foley/step/" #gait "_hills.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".mountain",          { "sfx/foley/step/" #gait "_mountain.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".desert",            { "sfx/foley/step/" #gait "_desert.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".swamp",             { "sfx/foley/step/" #gait "_swamp.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".water",             { "sfx/foley/step/" #gait "_water.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".underwater",        { "sfx/foley/step/" #gait "_underwater.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".underwater_ground", { "sfx/foley/step/" #gait "_underwater_ground.1.mp3", NULL }, { 0 }, 45, "foley", 1 }, \
    { "sfx.foley.step." #gait ".air",               { "sfx/foley/step/" #gait "_air.1.mp3", NULL }, { 0 }, 45, "foley", 1 }

#define FOOTSTEP_BASE(gait) \
    { "sfx.foley.step." #gait ".generic", { "sfx/foley/step/" #gait "_generic.1.mp3", NULL }, { 0 }, 45, "foley", 1 }

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
    { "notify.disconnect",       { "notify/disconnect.mp3",       NULL }, { 0, 0}, 60, "notify", 1 },

    /* SFX */
    { "sfx.door.open.generic",   { "sfx/door/open_generic.mp3",   NULL }, { 0,0 }, 60, "sfx"   , 1 },
    { "sfx.door.close.generic",  { "sfx/door/close_generic.mp3",  NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.door.lock.generic",   { "sfx/door/lock_generic.mp3",   NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.door.unlock.generic", { "sfx/door/unlock_generic.mp3", NULL }, { 0,0 }, 55, "sfx"   , 1 },
    { "sfx.consume.eat",         { "sfx/misc/eat_generic.mp3",    NULL }, { 0,0 }, 35, "sfx"   , 1 },
    { "sfx.consume.drink",       { "sfx/misc/drink_generic.mp3",  NULL }, { 0,0 }, 35, "sfx"   , 1 },
    { "sfx.consume.pill",        { "sfx/misc/pill_generic.mp3",   NULL }, { 0,0 }, 35, "sfx"   , 1 },
    { "sfx.consume.quaff",       { "sfx/misc/quaff_generic.mp3",  NULL }, { 0,0 }, 35, "sfx"   , 1 },
    { "sfx.consume.smoke",
        { "sfx/misc/smoke_generic.1.mp3",
          "sfx/misc/smoke_generic.2.mp3",
          "sfx/misc/smoke_generic.3.mp3",
           NULL
        }, { 0 }, 35, "sfx", 1 },
    { "sfx.consume.smear",       { "sfx/misc/smear_generic.mp3",  NULL }, { 0,0 }, 35, "sfx"   , 1 },

    /* SFX - Combat */

    {
        "sfx.combat.hit.organic",
        {
            "sfx/combat/hit/pool.organic.1.mp3",
            "sfx/combat/hit/pool.organic.2.mp3",
            "sfx/combat/hit/pool.organic.3.mp3",
            "sfx/combat/hit/pool.organic.4.mp3",
            "sfx/combat/hit/pool.organic.5.mp3",
            "sfx/combat/hit/pool.organic.6.mp3",
            "sfx/combat/hit/pool.organic.7.mp3",
            "sfx/combat/hit/pool.organic.8.mp3",
            "sfx/combat/hit/pool.organic.9.mp3",
            "sfx/combat/hit/pool.organic.10.mp3",
            "sfx/combat/hit/pool.organic.11.mp3",
            "sfx/combat/hit/pool.organic.12.mp3",
            "sfx/combat/hit/pool.organic.13.mp3",
            "sfx/combat/hit/pool.organic.14.mp3",
            "sfx/combat/hit/pool.organic.15.mp3",
            "sfx/combat/hit/pool.organic.16.mp3",
            "sfx/combat/hit/pool.organic.17.mp3",
            "sfx/combat/hit/pool.organic.18.mp3",
            "sfx/combat/hit/pool.organic.19.mp3",
            "sfx/combat/hit/pool.organic.20.mp3",
            "sfx/combat/hit/pool.organic.21.mp3",
            "sfx/combat/hit/pool.organic.22.mp3",
            "sfx/combat/hit/pool.organic.23.mp3",
            "sfx/combat/hit/pool.organic.24.mp3",
            "sfx/combat/hit/pool.organic.25.mp3",
            "sfx/combat/hit/pool.organic.26.mp3",
            "sfx/combat/hit/pool.organic.27.mp3",
            "sfx/combat/hit/pool.organic.28.mp3",
            "sfx/combat/hit/pool.organic.29.mp3",
            "sfx/combat/hit/pool.organic.30.mp3",
            "sfx/combat/hit/pool.organic.31.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },
    {
        "sfx.combat.hit.inorganic",
        {
            "sfx/combat/hit/pool.inorganic.1.mp3",
            "sfx/combat/hit/pool.inorganic.2.mp3",
            "sfx/combat/hit/pool.inorganic.3.mp3",
            "sfx/combat/hit/pool.inorganic.4.mp3",
            "sfx/combat/hit/pool.inorganic.5.mp3",
            "sfx/combat/hit/pool.inorganic.6.mp3",
            "sfx/combat/hit/pool.inorganic.7.mp3",
            "sfx/combat/hit/pool.inorganic.8.mp3",
            "sfx/combat/hit/pool.inorganic.9.mp3",
            "sfx/combat/hit/pool.inorganic.10.mp3",
            "sfx/combat/hit/pool.inorganic.11.mp3",
            "sfx/combat/hit/pool.inorganic.12.mp3",
            "sfx/combat/hit/pool.inorganic.13.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.slash.organic",
        {
            "sfx/combat/slash/pool.organic.1.mp3",
            "sfx/combat/slash/pool.organic.2.mp3",
            "sfx/combat/slash/pool.organic.3.mp3",
            "sfx/combat/slash/pool.organic.4.mp3",
            "sfx/combat/slash/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.slash.inorganic",
        {
            "sfx/combat/slash/pool.inorganic.1.mp3",
            "sfx/combat/slash/pool.inorganic.2.mp3",
            "sfx/combat/slash/pool.inorganic.3.mp3",
            "sfx/combat/slash/pool.inorganic.4.mp3",
            "sfx/combat/slash/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.whip.organic",
        {
            "sfx/combat/whip/pool.organic.1.mp3",
            "sfx/combat/whip/pool.organic.2.mp3",
            "sfx/combat/whip/pool.organic.3.mp3",
            "sfx/combat/whip/pool.organic.4.mp3",
            "sfx/combat/whip/pool.organic.5.mp3",
            "sfx/combat/whip/pool.organic.6.mp3",
            "sfx/combat/whip/pool.organic.7.mp3",
            "sfx/combat/whip/pool.organic.8.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.whip.inorganic",
        {
            "sfx/combat/whip/pool.inorganic.1.mp3",
            "sfx/combat/whip/pool.inorganic.2.mp3",
            "sfx/combat/whip/pool.inorganic.3.mp3",
            "sfx/combat/whip/pool.inorganic.4.mp3",
            "sfx/combat/whip/pool.inorganic.5.mp3",
            "sfx/combat/whip/pool.inorganic.6.mp3",
            "sfx/combat/whip/pool.inorganic.7.mp3",
            "sfx/combat/whip/pool.inorganic.8.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.chop.organic",
        {
            "sfx/combat/chop/pool.organic.1.mp3",
            "sfx/combat/chop/pool.organic.2.mp3",
            "sfx/combat/chop/pool.organic.3.mp3",
            "sfx/combat/chop/pool.organic.4.mp3",
            "sfx/combat/chop/pool.organic.5.mp3",
            "sfx/combat/chop/pool.organic.6.mp3",
            "sfx/combat/chop/pool.organic.7.mp3",
            "sfx/combat/chop/pool.organic.8.mp3",
            NULL
        },
        { 0 },
        85,
        "sfx",
        1
    },

    { "sfx.combat.chop.inorganic",
        {
            "sfx/combat/chop/pool.inorganic.1.mp3",
            "sfx/combat/chop/pool.inorganic.2.mp3",
            "sfx/combat/chop/pool.inorganic.3.mp3",
            "sfx/combat/chop/pool.inorganic.4.mp3",
            "sfx/combat/chop/pool.inorganic.5.mp3",
            "sfx/combat/chop/pool.inorganic.6.mp3",
            "sfx/combat/chop/pool.inorganic.7.mp3",
            "sfx/combat/chop/pool.inorganic.8.mp3",
            NULL
        },
        { 0 },
        85,
        "sfx",
        1
    },

    { "sfx.combat.bow.organic",
        {
            "sfx/combat/bow/pool.organic.1.mp3",
            "sfx/combat/bow/pool.organic.2.mp3",
            "sfx/combat/bow/pool.organic.3.mp3",
            "sfx/combat/bow/pool.organic.4.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.bow.inorganic",
        {
            "sfx/combat/bow/pool.inorganic.1.mp3",
            "sfx/combat/bow/pool.inorganic.2.mp3",
            "sfx/combat/bow/pool.inorganic.3.mp3",
            "sfx/combat/bow/pool.inorganic.4.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },
    { "sfx.combat.stab.organic",
        {
            "sfx/combat/stab/pool.organic.1.mp3",
            "sfx/combat/stab/pool.organic.2.mp3",
            "sfx/combat/stab/pool.organic.3.mp3",
            "sfx/combat/stab/pool.organic.4.mp3",
            "sfx/combat/stab/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.stab.inorganic",
        {
            "sfx/combat/stab/pool.inorganic.1.mp3",
            "sfx/combat/stab/pool.inorganic.2.mp3",
            "sfx/combat/stab/pool.inorganic.3.mp3",
            "sfx/combat/stab/pool.inorganic.4.mp3",
            "sfx/combat/stab/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

        { "sfx.combat.pound.organic",
        {
            "sfx/combat/pound/pool.organic.1.mp3",
            "sfx/combat/pound/pool.organic.2.mp3",
            "sfx/combat/pound/pool.organic.3.mp3",
            "sfx/combat/pound/pool.organic.4.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.pound.inorganic",
        {
            "sfx/combat/pound/pool.inorganic.1.mp3",
            "sfx/combat/pound/pool.inorganic.2.mp3",
            "sfx/combat/pound/pool.inorganic.3.mp3",
            NULL
        },
        { 0 },
        65,
        "sfx",
        1
    },

    { "sfx.combat.pierce.organic",
        {
            "sfx/combat/pierce/pool.organic.1.mp3",
            "sfx/combat/pierce/pool.organic.2.mp3",
            "sfx/combat/pierce/pool.organic.3.mp3",
            "sfx/combat/pierce/pool.organic.4.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.pierce.inorganic",
        {
            "sfx/combat/pierce/pool.inorganic.1.mp3",
            "sfx/combat/pierce/pool.inorganic.2.mp3",
            "sfx/combat/pierce/pool.inorganic.3.mp3",
            "sfx/combat/pierce/pool.inorganic.4.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.slice.organic",
        {
            "sfx/combat/slice/pool.organic.1.mp3",
            "sfx/combat/slice/pool.organic.2.mp3",
            "sfx/combat/slice/pool.organic.3.mp3",
            "sfx/combat/slice/pool.organic.4.mp3",
            "sfx/combat/slice/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.slice.inorganic",
        {
            "sfx/combat/slice/pool.inorganic.1.mp3",
            "sfx/combat/slice/pool.inorganic.2.mp3",
            "sfx/combat/slice/pool.inorganic.3.mp3",
            "sfx/combat/slice/pool.inorganic.4.mp3",
            "sfx/combat/slice/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.claw.organic",
        {
            "sfx/combat/claw/pool.organic.1.mp3",
            "sfx/combat/claw/pool.organic.2.mp3",
            "sfx/combat/claw/pool.organic.3.mp3",
            "sfx/combat/claw/pool.organic.4.mp3",
            "sfx/combat/claw/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.claw.inorganic",
        {
            "sfx/combat/claw/pool.inorganic.1.mp3",
            "sfx/combat/claw/pool.inorganic.2.mp3",
            "sfx/combat/claw/pool.inorganic.3.mp3",
            "sfx/combat/claw/pool.inorganic.4.mp3",
            "sfx/combat/claw/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        50,
        "sfx",
        1
    },

    { "sfx.combat.blast.organic",
        {
            "sfx/combat/blast/pool.organic.1.mp3",
            "sfx/combat/blast/pool.organic.2.mp3",
            "sfx/combat/blast/pool.organic.3.mp3",
            "sfx/combat/blast/pool.organic.4.mp3",
            "sfx/combat/blast/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.blast.inorganic",
        {
            "sfx/combat/blast/pool.inorganic.1.mp3",
            "sfx/combat/blast/pool.inorganic.2.mp3",
            "sfx/combat/blast/pool.inorganic.3.mp3",
            "sfx/combat/blast/pool.inorganic.4.mp3",
            "sfx/combat/blast/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.crush.organic",
        {
            "sfx/combat/crush/pool.organic.1.mp3",
            "sfx/combat/crush/pool.organic.2.mp3",
            "sfx/combat/crush/pool.organic.3.mp3",
            "sfx/combat/crush/pool.organic.4.mp3",
            "sfx/combat/crush/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.crush.inorganic",
        {
            "sfx/combat/crush/pool.inorganic.1.mp3",
            "sfx/combat/crush/pool.inorganic.2.mp3",
            "sfx/combat/crush/pool.inorganic.3.mp3",
            "sfx/combat/crush/pool.inorganic.4.mp3",
            "sfx/combat/crush/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.grep",
        {
            "sfx/combat/grep/pool.1.mp3",
            "sfx/combat/grep/pool.2.mp3",
            "sfx/combat/grep/pool.3.mp3",
            "sfx/combat/grep/pool.4.mp3",
            "sfx/combat/grep/pool.5.mp3",
            "sfx/combat/grep/pool.6.mp3",
            "sfx/combat/grep/pool.7.mp3",
            "sfx/combat/grep/pool.8.mp3",
            "sfx/combat/grep/pool.9.mp3",
            "sfx/combat/grep/pool.10.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.bite.organic",
        {
            "sfx/combat/bite/pool.organic.1.mp3",
            "sfx/combat/bite/pool.organic.2.mp3",
            "sfx/combat/bite/pool.organic.3.mp3",
            "sfx/combat/bite/pool.organic.4.mp3",
            "sfx/combat/bite/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.bite.inorganic",
        {
            "sfx/combat/bite/pool.inorganic.1.mp3",
            "sfx/combat/bite/pool.inorganic.2.mp3",
            "sfx/combat/bite/pool.inorganic.3.mp3",
            "sfx/combat/bite/pool.inorganic.4.mp3",
            "sfx/combat/bite/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.suction.organic",
        {
            "sfx/combat/suction/pool.organic.1.mp3",
            "sfx/combat/suction/pool.organic.2.mp3",
            "sfx/combat/suction/pool.organic.3.mp3",
            "sfx/combat/suction/pool.organic.4.mp3",
            "sfx/combat/suction/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.suction.inorganic",
        {
            "sfx/combat/suction/pool.inorganic.1.mp3",
            "sfx/combat/suction/pool.inorganic.2.mp3",
            "sfx/combat/suction/pool.inorganic.3.mp3",
            "sfx/combat/suction/pool.inorganic.4.mp3",
            "sfx/combat/suction/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.rake.organic",
        {
            "sfx/combat/rake/pool.organic.1.mp3",
            "sfx/combat/rake/pool.organic.2.mp3",
            "sfx/combat/rake/pool.organic.3.mp3",
            "sfx/combat/rake/pool.organic.4.mp3",
            "sfx/combat/rake/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.rake.inorganic",
        {
            "sfx/combat/rake/pool.inorganic.1.mp3",
            "sfx/combat/rake/pool.inorganic.2.mp3",
            "sfx/combat/rake/pool.inorganic.3.mp3",
            "sfx/combat/rake/pool.inorganic.4.mp3",
            "sfx/combat/rake/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

        { "sfx.combat.swipe.organic",
        {
            "sfx/combat/swipe/pool.organic.1.mp3",
            "sfx/combat/swipe/pool.organic.2.mp3",
            "sfx/combat/swipe/pool.organic.3.mp3",
            "sfx/combat/swipe/pool.organic.4.mp3",
            "sfx/combat/swipe/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.swipe.inorganic",
        {
            "sfx/combat/swipe/pool.inorganic.1.mp3",
            "sfx/combat/swipe/pool.inorganic.2.mp3",
            "sfx/combat/swipe/pool.inorganic.3.mp3",
            "sfx/combat/swipe/pool.inorganic.4.mp3",
            "sfx/combat/swipe/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        50,
        "sfx",
        1
    },

    { "sfx.combat.sting",
        {
            "sfx/combat/sting/pool.1.mp3",
            "sfx/combat/sting/pool.2.mp3",
            "sfx/combat/sting/pool.3.mp3",
            "sfx/combat/sting/pool.4.mp3",
            "sfx/combat/sting/pool.5.mp3",
            "sfx/combat/sting/pool.6.mp3",
            "sfx/combat/sting/pool.7.mp3",
            "sfx/combat/sting/pool.8.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.scoop.organic",
        {
            "sfx/combat/scoop/pool.organic.1.mp3",
            "sfx/combat/scoop/pool.organic.2.mp3",
            "sfx/combat/scoop/pool.organic.3.mp3",
            "sfx/combat/scoop/pool.organic.4.mp3",
            "sfx/combat/scoop/pool.organic.5.mp3",
            "sfx/combat/scoop/pool.organic.6.mp3",
            "sfx/combat/scoop/pool.organic.7.mp3",
            "sfx/combat/scoop/pool.organic.8.mp3",
            "sfx/combat/scoop/pool.organic.9.mp3",
            "sfx/combat/scoop/pool.organic.10.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.scoop.inorganic",
        {
            "sfx/combat/scoop/pool.inorganic.1.mp3",
            "sfx/combat/scoop/pool.inorganic.2.mp3",
            "sfx/combat/scoop/pool.inorganic.3.mp3",
            "sfx/combat/scoop/pool.inorganic.4.mp3",
            "sfx/combat/scoop/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.mash.organic",
        {
            "sfx/combat/mash/pool.organic.1.mp3",
            "sfx/combat/mash/pool.organic.2.mp3",
            "sfx/combat/mash/pool.organic.3.mp3",
            "sfx/combat/mash/pool.organic.4.mp3",
            "sfx/combat/mash/pool.organic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.mash.inorganic",
        {
            "sfx/combat/mash/pool.inorganic.1.mp3",
            "sfx/combat/mash/pool.inorganic.2.mp3",
            "sfx/combat/mash/pool.inorganic.3.mp3",
            "sfx/combat/mash/pool.inorganic.4.mp3",
            "sfx/combat/mash/pool.inorganic.5.mp3",
            NULL
        },
        { 0 },
        100,
        "sfx",
        1
    },

    { "sfx.combat.hack.organic",
        {
            "sfx/combat/hack/pool.organic.1.mp3",
            "sfx/combat/hack/pool.organic.2.mp3",
            "sfx/combat/hack/pool.organic.3.mp3",
            "sfx/combat/hack/pool.organic.4.mp3",
            "sfx/combat/hack/pool.organic.5.mp3",
            "sfx/combat/hack/pool.organic.6.mp3",
            "sfx/combat/hack/pool.organic.7.mp3",
            "sfx/combat/hack/pool.organic.8.mp3",
            NULL
        },
        { 0 },
        85,
        "sfx",
        1
    },

    { "sfx.combat.hack.inorganic",
        {
            "sfx/combat/hack/pool.inorganic.1.mp3",
            "sfx/combat/hack/pool.inorganic.2.mp3",
            "sfx/combat/hack/pool.inorganic.3.mp3",
            "sfx/combat/hack/pool.inorganic.4.mp3",
            "sfx/combat/hack/pool.inorganic.5.mp3",
            "sfx/combat/hack/pool.inorganic.6.mp3",
            "sfx/combat/hack/pool.inorganic.7.mp3",
            "sfx/combat/hack/pool.inorganic.8.mp3",
            NULL
        },
        { 0 },
        85,
        "sfx",
        1
    },

    { "sfx.combat.shield_block",
        {
                "sfx/combat/shield_block/pool.1.mp3",
                "sfx/combat/shield_block/pool.2.mp3",
                "sfx/combat/shield_block/pool.3.mp3",
                "sfx/combat/shield_block/pool.4.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.miss",
        {
                "sfx/combat/miss/pool.1.mp3",
                "sfx/combat/miss/pool.2.mp3",
                "sfx/combat/miss/pool.3.mp3",
                "sfx/combat/miss/pool.4.mp3",
                "sfx/combat/miss/pool.5.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.dodge",
        {
                "sfx/combat/dodge/pool.1.mp3",
                "sfx/combat/dodge/pool.2.mp3",
                "sfx/combat/dodge/pool.3.mp3",
                "sfx/combat/dodge/pool.4.mp3",
                "sfx/combat/dodge/pool.5.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.parry.weapon",
        {
            "sfx/combat/parry/pool.weapon.1.mp3",
            "sfx/combat/parry/pool.weapon.2.mp3",
            "sfx/combat/parry/pool.weapon.3.mp3",
            "sfx/combat/parry/pool.weapon.4.mp3",
            "sfx/combat/parry/pool.weapon.5.mp3",
            NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.parry.noweapon",
        {
            "sfx/combat/parry/pool.noweapon.1.mp3",
            "sfx/combat/parry/pool.noweapon.2.mp3",
            "sfx/combat/parry/pool.noweapon.3.mp3",
            "sfx/combat/parry/pool.noweapon.4.mp3",
            "sfx/combat/parry/pool.noweapon.5.mp3",
            "sfx/combat/parry/pool.noweapon.6.mp3",
            "sfx/combat/parry/pool.noweapon.7.mp3",
            NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.acrobatics",
        {
                "sfx/combat/acrobatics/pool.1.mp3",
                "sfx/combat/acrobatics/pool.2.mp3",
                "sfx/combat/acrobatics/pool.3.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.transfix",
        {
                "sfx/combat/transfix/pool.1.mp3",
                "sfx/combat/transfix/pool.2.mp3",
                "sfx/combat/transfix/pool.3.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    { "sfx.combat.blink",
        {
                "sfx/combat/blink/pool.1.mp3",
                "sfx/combat/blink/pool.2.mp3",
                "sfx/combat/blink/pool.3.mp3",
                NULL
        },
        { 0 },
        60,
        "sfx",
        1
    },

    {
        "sfx.combat.resist_toxin",
        {
                "sfx/combat/resist_toxin/pool.1.mp3",
                "sfx/combat/resist_toxin/pool.2.mp3",
                "sfx/combat/resist_toxin/pool.3.mp3",
                NULL
        },
        { 0 },
        50,
        "sfx",
        1
    },

    {
        "sfx.combat.maul",
        {
                "sfx/combat/maul/pool.1.mp3",
                "sfx/combat/maul/pool.2.mp3",
                "sfx/combat/maul/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.combat.crush",
        {
                "sfx/combat/crush_bear/pool.1.mp3",
                "sfx/combat/crush_bear/pool.2.mp3",
                "sfx/combat/crush_bear/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.combat.web",
        {
                "sfx/combat/web/pool.1.mp3",
                "sfx/combat/web/pool.2.mp3",
                "sfx/combat/web/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.combat.venom",
        {
                "sfx/combat/venom/pool.1.mp3",
                "sfx/combat/venom/pool.2.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.combat.assassinate",
        {
                "sfx/combat/assassinate/pool.1.mp3",
                "sfx/combat/assassinate/pool.2.mp3",
                "sfx/combat/assassinate/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.combat.backstab",
        {
                "sfx/combat/backstab/pool.1.mp3",
                "sfx/combat/backstab/pool.2.mp3",
                "sfx/combat/backstab/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.skill.morph",
        {
                "sfx/skill/morph/pool.1.mp3",
                "sfx/skill/morph/pool.2.mp3",
                "sfx/skill/morph/pool.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    {
        "sfx.skill.bubble_jet",
        { "sfx/skill/bubble_jet/bubble_jet.1.mp3", NULL },
        { 0,0 },
        55,
        "sfx",
        1
    },

    /* Conditions */

    {
        "sfx.condition.nausea",
        {
                "sfx/condition/nausea/pool.1.mp3",
                "sfx/condition/nausea/pool.2.mp3",
                "sfx/condition/nausea/pool.3.mp3",
                NULL
        },
        { 0 },
        55,
        "sfx",
        1
    },

    {
        "sfx.condition.tinnitus",
        {
                "sfx/condition/tinnitus/pool.1.mp3",
                "sfx/condition/tinnitus/pool.2.mp3",
                "sfx/condition/tinnitus/pool.3.mp3",
                NULL
        },
        { 0 },
        55,
        "sfx",
        1
    },

    {
        "sfx.condition.poison",
        {
                "sfx/condition/poison/pool.1.mp3",
                "sfx/condition/poison/pool.2.mp3",
                "sfx/condition/poison/pool.3.mp3",
                NULL
        },
        { 0 },
        55,
        "sfx",
        1
    },

    {
        "sfx.condition.hold_breath",
        {
                "sfx/condition/hold_breath/pool.1.mp3",
                "sfx/condition/hold_breath/pool.2.mp3",
                "sfx/condition/hold_breath/pool.3.mp3",
                "sfx/condition/hold_breath/pool.4.mp3",
                NULL
        },
        { 0 },
        55,
        "sfx",
        1
    },

    {
        "sfx.condition.drowning",
        {
                "sfx/condition/drowning/pool.1.mp3",
                "sfx/condition/drowning/pool.2.mp3",
                "sfx/condition/drowning/pool.3.mp3",
                NULL
        },
        { 0 },
        55,
        "sfx",
        1
    },

    {
        "sfx.condition.void",
        {
                "sfx/condition/void/pool.1.mp3",
                NULL
        },
        { 0 },
        75,
        "sfx",
        1
    },

    {
        "sfx.condition.light_out",
        {
                "sfx/condition/light_out/pool.1.mp3",
                NULL
        },
        { 0 },
        75,
        "sfx",
        1
    },

    {
        "sfx.condition.wake",
        {
            "sfx/condition/wake/pool.1.mp3",
            "sfx/condition/wake/pool.2.mp3",
            "sfx/condition/wake/pool.3.mp3",
            "sfx/condition/wake/pool.4.mp3",
            "sfx/condition/wake/pool.5.mp3",
            "sfx/condition/wake/pool.6.mp3",
            NULL
        },
        { 0 },
        45,
        "sfx",
        1
    },

    { "sfx.condition.sleep",    { "sfx/condition/sleep/pool.1.mp3", "sfx/condition/sleep/pool.2.mp3", "sfx/condition/sleep/pool.3.mp3", NULL }, { 0 }, 45, "sfx", 1 },

    { "sfx.condition.meditate", { "sfx/condition/meditate/pool.1.mp3", "sfx/condition/meditate/pool.2.mp3", "sfx/condition/meditate/pool.3.mp3", NULL }, { 0 }, 45, "sfx", 1 },

    { "sfx.condition.rest", { "sfx/condition/rest/pool.1.mp3", "sfx/condition/rest/pool.2.mp3", "sfx/condition/rest/pool.3.mp3", NULL }, { 0 }, 45, "sfx", 1 },

    { "sfx.combat.mobdeath.1",     { "sfx/combat/mobdeath/1.mp3", NULL }, { 0,0 }, 70, "sfx", 1 },
    { "sfx.combat.mobdeath.2",     { "sfx/combat/mobdeath/2.mp3", NULL }, { 0,0 }, 70, "sfx", 1 },

    { "sfx.spell.cast.generic",
        {
                "sfx/spell/cast/generic.1.mp3",
                "sfx/spell/cast/generic.2.mp3",
                "sfx/spell/cast/generic.3.mp3",
                "sfx/spell/cast/generic.4.mp3",
                "sfx/spell/cast/generic.5.mp3",
                "sfx/spell/cast/generic.6.mp3",
                "sfx/spell/cast/generic.7.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.wearoff.generic",
        {
                "sfx/spell/wearoff/generic.1.mp3",
                "sfx/spell/wearoff/generic.2.mp3",
                "sfx/spell/wearoff/generic.3.mp3",
                "sfx/spell/wearoff/generic.4.mp3",
                "sfx/spell/wearoff/generic.5.mp3",
                "sfx/spell/wearoff/generic.6.mp3",
                "sfx/spell/wearoff/generic.7.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.fireshield",
        {
                "sfx/spell/cast/fireshield.1.mp3",
                "sfx/spell/cast/fireshield.2.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.wearoff.fireshield",
        {
                "sfx/spell/wearoff/fireshield.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.biofeedback",
        {
                "sfx/spell/cast/biofeedback.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.steam_breath",
        {
                "sfx/spell/cast/steam_breath.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.wearoff.biofeedback",
        {
                "sfx/spell/wearoff/biofeedback.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.fireball",
        {
                "sfx/spell/cast/fireball.1.mp3",
                "sfx/spell/cast/fireball.2.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.summon_demon",
        {
                "sfx/spell/cast/summon_demon.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.teleport",
        {
                "sfx/spell/cast/teleport.1.mp3",
                "sfx/spell/cast/teleport.2.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.hellfire",
        {
                "sfx/spell/cast/hellfire.1.mp3",
                "sfx/spell/cast/hellfire.2.mp3",
                "sfx/spell/cast/hellfire.3.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.cast.sonic_blast",
        {
                "sfx/spell/cast/sonic_blast.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.spell.wearoff.sonic_blast",
        {
                "sfx/spell/wearoff/sonic_blast.1.mp3",
                NULL
        },
        { 0 },
        70,
        "sfx",
        1
    },

    { "sfx.portal.enter",
        { "sfx/portal/enter/pool.1.mp3", "sfx/portal/enter/pool.2.mp3", "sfx/portal/enter/pool.3.mp3", NULL },
        { 0 }, 60, "sfx", 1 },
    { "sfx.portal.exit",
        { "sfx/portal/exit/pool.1.mp3", "sfx/portal/exit/pool.2.mp3", "sfx/portal/exit/pool.3.mp3", NULL },
        { 0 }, 60, "sfx", 1 },

    /* Footstep foley */

    { "sfx.foley.step.generic", { "sfx/foley/step/generic.1.mp3", NULL }, { 0 }, 45, "foley", 1 },

    FOOTSTEP_TERRAIN_SET(generic),
    FOOTSTEP_TERRAIN_SET(clop),
    FOOTSTEP_BASE(clop),
    FOOTSTEP_TERRAIN_SET(flying),
    FOOTSTEP_BASE(flying),
    FOOTSTEP_TERRAIN_SET(hawk_flying),
    FOOTSTEP_BASE(hawk_flying),
    FOOTSTEP_TERRAIN_SET(hawk),
    FOOTSTEP_BASE(hawk),
    FOOTSTEP_TERRAIN_SET(dragon_flying),
    FOOTSTEP_BASE(dragon_flying),
    FOOTSTEP_TERRAIN_SET(dragon),
    FOOTSTEP_BASE(dragon),
    FOOTSTEP_TERRAIN_SET(phoenix_flying),
    FOOTSTEP_BASE(phoenix_flying),
    FOOTSTEP_TERRAIN_SET(phoenix),
    FOOTSTEP_BASE(phoenix),
    FOOTSTEP_TERRAIN_SET(fly_flying),
    FOOTSTEP_BASE(fly_flying),
    FOOTSTEP_TERRAIN_SET(fly),
    FOOTSTEP_BASE(fly),
    FOOTSTEP_TERRAIN_SET(bat_flying),
    FOOTSTEP_BASE(bat_flying),
    FOOTSTEP_TERRAIN_SET(bat),
    FOOTSTEP_BASE(bat),
    FOOTSTEP_TERRAIN_SET(demon_flying),
    FOOTSTEP_BASE(demon_flying),
    FOOTSTEP_TERRAIN_SET(demon),
    FOOTSTEP_BASE(demon),
    FOOTSTEP_TERRAIN_SET(griffin_flying),
    FOOTSTEP_BASE(griffin_flying),
    FOOTSTEP_TERRAIN_SET(griffin),
    FOOTSTEP_BASE(griffin),
    FOOTSTEP_TERRAIN_SET(chameleon),
    FOOTSTEP_BASE(chameleon),
    FOOTSTEP_TERRAIN_SET(hawk),
    FOOTSTEP_BASE(hawk),
    FOOTSTEP_TERRAIN_SET(cat),
    FOOTSTEP_BASE(cat),
    FOOTSTEP_TERRAIN_SET(snake),
    FOOTSTEP_BASE(snake),
    FOOTSTEP_TERRAIN_SET(scorpion),
    FOOTSTEP_BASE(scorpion),
    FOOTSTEP_TERRAIN_SET(spider),
    FOOTSTEP_BASE(spider),
    FOOTSTEP_TERRAIN_SET(bear),
    FOOTSTEP_BASE(bear),
    FOOTSTEP_TERRAIN_SET(tiger),
    FOOTSTEP_BASE(tiger),
    FOOTSTEP_TERRAIN_SET(hydra),
    FOOTSTEP_BASE(hydra),
    FOOTSTEP_TERRAIN_SET(direwolf),
    FOOTSTEP_BASE(direwolf),
    FOOTSTEP_TERRAIN_SET(wolf),
    FOOTSTEP_BASE(wolf),
    FOOTSTEP_TERRAIN_SET(mist),
    FOOTSTEP_BASE(mist),

    /* Mob footstep foley */

    { "sfx.foley.mob.3060.city",                { "sfx/foley/mob/3060/city.1.mp3",
                                                  "sfx/foley/mob/3060/city.2.mp3", NULL }, { 0 }, 45, "foley", 1 },
    { "sfx.foley.mob.3060.inside",              { "sfx/foley/mob/3060/inside.1.mp3",
                                                  "sfx/foley/mob/3060/inside.2.mp3", NULL }, { 0 }, 45, "foley", 1 },

    { "sfx.foley.mob.3062.city",                { "sfx/foley/mob/3062/city.1.mp3", NULL }, { 0 }, 45, "foley", 1 },
    { "sfx.foley.mob.3062.inside",              { "sfx/foley/mob/3062/inside.1.mp3", NULL }, { 0 }, 45, "foley", 1 },

    { "sfx.foley.mob.3063.city",                { "sfx/foley/mob/3063/city.1.mp3", NULL }, { 0 }, 45, "foley", 1 },
    { "sfx.foley.mob.3063.inside",              { "sfx/foley/mob/3063/inside.1.mp3", NULL }, { 0 }, 45, "foley", 1 },

    { "sfx.foley.mob.29200.underwater",         { "sfx/foley/mob/29200/underwater.1.mp3", NULL }, { 0 }, 45, "foley", 1 },
    { "sfx.foley.mob.29200.underwater_ground",  { "sfx/foley/mob/29200/underwater_ground.1.mp3", NULL }, { 0 }, 45, "foley", 1 },

    /* Other foley */
    { "sfx.foley.fill",             { "sfx/foley/fill/pool.1.mp3", NULL }, { 0 }, 65, "foley", 1 },

    /* Weather */
    { "ambient.weather.rain",      { "ambient/weather/rain_loop1.mp3", NULL }, {0,0}, 20, "ambient.weather", 1 },
    { "ambient.weather.lightning", { "ambient/weather/lightning_loop1.mp3", NULL }, {0,0}, 20, "ambient.weather", 1 },

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

/* Map damage max thresholds to event keys (ORGANIC keys for test pass). */
typedef struct combat_hit_tier_map
{
        int         max_damage;
        const char *key_organic;
        const char *key_inorganic; /* keep for later; can be NULL for now */
} COMBAT_HIT_TIER_MAP;

const COMBAT_HIT_TIER_MAP combat_hit_tiers[] =
{
        { 8,  "sfx.combat.hit.scratch.organic",   "sfx.combat.hit.scratch.inorganic"   },
        { 16, "sfx.combat.hit.graze.organic",     "sfx.combat.hit.graze.inorganic"     },
        { 24, "sfx.combat.hit.hit.organic",       "sfx.combat.hit.hit.inorganic"       },
        { 32, "sfx.combat.hit.injure.organic",    "sfx.combat.hit.injure.inorganic"    },
        { 40, "sfx.combat.hit.wound.organic",     "sfx.combat.hit.wound.inorganic"     },
        { 48, "sfx.combat.hit.maul.organic",      "sfx.combat.hit.maul.inorganic"      },
        { 56, "sfx.combat.hit.decimate.organic",  "sfx.combat.hit.decimate.inorganic"  },
        { 64, "sfx.combat.hit.mangle.organic",    "sfx.combat.hit.mangle.inorganic"    },
        { 72, "sfx.combat.hit.maim.organic",      "sfx.combat.hit.maim.inorganic"      },

        { -1, NULL, NULL } /* sentinel */
};

extern const COMBAT_HIT_TIER_MAP combat_hit_tiers[];

/*---END SOUND STUFF---*/
