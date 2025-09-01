/* write_json.c — centralised JSON writers (project-local ./api)
 *
 * - Resolves ./api at runtime (./api, ../api, or exe_dir/../api)
 * - Creates the directory if needed
 * - Writes atomically: create unique temp in same dir -> rename()
 * - Exports: json_write_status()
 *
 * Add write_json.c to your Makefile object list and call json_write_status()
 * from update_handler() (e.g., once per minute) or temporarily from do_time().
 */

#if defined(macintosh)
#  include <types.h>
#else
#  include <sys/types.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include <libgen.h>   /* basename (POSIX); safe on Linux/Docker */

#if !defined(macintosh)
#  include <unistd.h>     /* access, getcwd, readlink, fsync */
#  include <sys/stat.h>   /* mkdir */
#  include <limits.h>     /* PATH_MAX */
#  include <string.h>     /* memcpy, strcat, strchr, strrchr, snprintf */
#  include <fcntl.h>      /* open, O_CREAT|O_EXCL */
#  include <errno.h>      /* errno */
#endif


#define EVENTS_FILE "../api/events.json"
#define MAX_EVENTS 50

#ifndef DD4_VERSION_STR
#  define DD4_VERSION_STR "DD4 4.0.0"
#endif
#define MAX_LINE 2048

typedef struct {
    char month[16];
    int year;
    char *text;
} ChangeEntry;

typedef struct {
    time_t timestamp;
    char player[MAX_INPUT_LENGTH];
    char message[MAX_STRING_LENGTH];
} EVENT_ENTRY;

/* Provided by your codebase */
extern time_t current_time;
extern time_t boot_time;
extern DESCRIPTOR_DATA *descriptor_list;
extern int max_on;

/* ------------------------------------------------------------------------- */
/* Utility: path resolution for ./api                                        */
/* ------------------------------------------------------------------------- */

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif

/* Normalise trailing slash: remove any final '/' in-place */
static void strip_trailing_slash(char *s) {
    size_t n = strlen(s);
    while (n && s[n-1] == '/') { s[--n] = '\0'; }
}

static void json_escape_string(const char *src, char *dst, size_t dst_size)
{
    size_t j = 0;
    for (size_t i = 0; src[i] && j + 1 < dst_size; i++) {
        unsigned char c = src[i];
        if (c == '"' || c == '\\') {
            if (j + 2 >= dst_size) break;
            dst[j++] = '\\';
            dst[j++] = c;
        } else if (c < 0x20) { // control chars → \u00XX
            if (j + 6 >= dst_size) break;
            j += snprintf(dst + j, dst_size - j, "\\u%04x", c);
        } else {
            dst[j++] = c;
        }
    }
    dst[j] = '\0';
}

static const char *json_escape(const char *src) {
    static char buf[MAX_STRING_LENGTH * 4]; // plenty of space
    char *dst = buf;

    while (*src && (dst - buf) < (int)sizeof(buf) - 2) {
        unsigned char c = *src;

        switch (c) {
            case '\"': *dst++ = '\\'; *dst++ = '\"'; break;
            case '\\': *dst++ = '\\'; *dst++ = '\\'; break;
            case '\n': *dst++ = '\\'; *dst++ = 'n'; break;
            case '\r': /* drop carriage returns entirely */ break;
            case '\t': *dst++ = ' '; break; // collapse tabs to single space
            default:
                if (c < 0x20) {
                    // other control chars → \u00XX
                    dst += snprintf(dst, sizeof(buf) - (dst - buf), "\\u%04x", c);
                } else {
                    *dst++ = c;
                }
                break;
        }
        src++;
    }
    *dst = '\0';
    return buf;
}

/* Helper to get class/immortal title */
static const char *who_class_title(CHAR_DATA *ch) {
    if (IS_NPC(ch))
        return "Mob";

    if (ch->level > LEVEL_HERO) {
        switch (ch->level) {
            case L_IMM: return "GOD";
            case L_DIR: return "DIRECTOR";
            case L_SEN: return "SENIOR";
            case L_JUN: return "JUNIOR";
            case L_APP: return "APPRENTICE";
            case L_BUI: return "BUILDER";
            case L_HER: return "HERO";
            default:    return "Immortal";
        }
    }

    if (ch->sub_class)
    {
        return sub_class_table[ch->sub_class].show_name;
    }

    return class_table[ch->class].show_name;
}

static void json_who_data(FILE *fp, CHAR_DATA *ch) {
    const char *class = who_class_title(ch);
    const char *race  = race_table[ch->race].who_name;
    const char *clan  = (IS_SET(ch->status, PLR_RONIN) ? "Ron" :
                         (ch->clan ? clan_table[ch->clan].who_name : ""));
    const char *clan_title_str = (ch->clan ? clan_title[ch->clan][ch->clan_level] : "");

    /* Collect flags into a JSON array */
    fprintf(fp, "    {\n");
    fprintf(fp, "      \"name\": \"%s\",\n", json_escape(ch->name));
    fprintf(fp, "      \"title\": \"%s\",\n", json_escape(ch->pcdata->title));
    fprintf(fp, "      \"race\": \"%s\",\n", json_escape(race));
    fprintf(fp, "      \"level\": %d,\n", ch->level);
    fprintf(fp, "      \"class\": \"%s\",\n", json_escape(class));
    fprintf(fp, "      \"clan\": \"%s\",\n", json_escape(clan));
    fprintf(fp, "      \"clanTitle\": \"%s\",\n", json_escape(clan_title_str));
    fprintf(fp, "      \"flags\": [");

    bool first = true;
    if (ch->timer > 5) {
        fprintf(fp, "\"Idle\"");
        first = false;
    }
    if (ch->desc && ch->desc->connected >= CON_NOTE_TO && ch->desc->connected <= CON_NOTE_FINISH) {
        fprintf(fp, "%s\"Note\"", first ? "" : ",");
        first = false;
    }
    if (ch->silent_mode) {
        fprintf(fp, "%s\"Quiet\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->act, PLR_AFK)) {
        fprintf(fp, "%s\"AFK\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->status, PLR_KILLER)) {
        fprintf(fp, "%s\"Killer\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->status, PLR_THIEF)) {
        fprintf(fp, "%s\"Thief\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->status, PLR_RONIN)) {
        fprintf(fp, "%s\"Ronin\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->status, PLR_HUNTED)) {
        fprintf(fp, "%s\"Hunted\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->act, PLR_LEADER)) {
        fprintf(fp, "%s\"Leader\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->act, PLR_GUIDE)) {
        fprintf(fp, "%s\"Guide\"", first ? "" : ",");
        first = false;
    }
    if (IS_SET(ch->act, PLR_WIZINVIS)) {
        fprintf(fp, "%s\"Wizinvis\"", first ? "" : ",");
        first = false;
    }
    fprintf(fp, "]\n");
    fprintf(fp, "    }");
}

/* Try, in order:
 *   1) <cwd>/api
 *   2) <cwd>/../api        (common if binary runs from ./src)
 *   3) dirname(/proc/self/exe)/../api (robust when cwd is elsewhere)
 * Falls back to <cwd>/api (and will create it).
 */
/* Resolve repo-root and return "<root>/api" (create on first use). */
static const char *resolve_api_dir(void)
{
    static char api[PATH_MAX];
    char cwd[PATH_MAX];

    if (!getcwd(cwd, sizeof(cwd))) {
        /* ultra‑fallback: local ./api */
        strncpy(api, "api", sizeof(api));
        api[sizeof(api)-1] = '\0';
        return api;
    }

    strip_trailing_slash(cwd);

    /* If we’re in .../area or .../src, step up one level to repo root */
    const char *leaf = strrchr(cwd, '/');
    if (leaf && (strcmp(leaf+1, "area") == 0 || strcmp(leaf+1, "src") == 0)) {
        /* parent dir */
        size_t root_len = (size_t)(leaf - cwd);
        if (root_len >= sizeof(api)) {
            strncpy(api, "api", sizeof(api));
            api[sizeof(api)-1] = '\0';
            return api;
        }
        memcpy(api, cwd, root_len);
        api[root_len] = '\0';
    } else {
        /* otherwise, treat CWD as root */
        strncpy(api, cwd, sizeof(api));
        api[sizeof(api)-1] = '\0';
    }

    /* Append /api */
    size_t n = strlen(api);
    if (n + 4 + 1 >= sizeof(api)) {  /* "/api" + NUL */
        strncpy(api, "api", sizeof(api));
        api[sizeof(api)-1] = '\0';
        return api;
    }
    memcpy(api + n, "/api", 5);  /* includes NUL */

    /* Ensure it exists (harmless if it already does) */
#if !defined(macintosh)
    (void)mkdir(api, 0755);
#endif

    return api;
}


/* ------------------------------------------------------------------------- */
/* Atomic write helper                                                       */
/* ------------------------------------------------------------------------- */

/* final_path is the target (e.g., ".../api/status.json") */
static void write_atomic_json(const char *final_path, const char *payload)
{
#if defined(macintosh)
    /* Simple fallback: not strictly atomic on classic MacOS */
    FILE *fp = fopen(final_path, "w");
    if (!fp) return;
    fputs(payload, fp);
    fputc('\n', fp);
    fflush(fp);
    fclose(fp);
#else
    char dir[PATH_MAX], tmp[PATH_MAX];
    const char *slash = strrchr(final_path, '/');
    if (!slash) return;

    /* directory portion */
    size_t dlen = (size_t)(slash - final_path);
    if (dlen >= sizeof(dir)) return;
    memcpy(dir, final_path, dlen);
    dir[dlen] = '\0';

    /* create unique temp in same dir */
    int fd = -1;
    for (int attempt = 0; attempt < 128; ++attempt) {
        int n = snprintf(tmp, sizeof(tmp), "%s/.json-%ld-%d.tmp",
                         dir, (long)getpid(), attempt);
        if (n < 0 || (size_t)n >= sizeof(tmp)) return;

        fd = open(tmp, O_CREAT | O_EXCL | O_WRONLY, 0644);
        if (fd >= 0) break;
        if (errno != EEXIST) return; /* unexpected error */
    }
    if (fd < 0) return;

    /* write payload via stdio for convenience */
    FILE *fp = fdopen(fd, "w");
    if (!fp) { close(fd); unlink(tmp); return; }

    if (fputs(payload, fp) == EOF || fputc('\n', fp) == EOF) {
        fclose(fp); unlink(tmp); return;
    }
    fflush(fp);
    fsync(fd);
    fclose(fp); /* closes fd */

    /* atomic replace */
    if (rename(tmp, final_path) != 0) {
        unlink(tmp);
    }
#endif
}

/* ------------------------------------------------------------------------- */
/* Data helpers                                                               */
/* ------------------------------------------------------------------------- */

static int players_online(void)
{
    int n = 0;
    DESCRIPTOR_DATA *d;
    for (d = descriptor_list; d; d = d->next) {
        if (d->connected == CON_PLAYING && d->character && !IS_NPC(d->character))
            ++n;
    }
    return n;
}

/* ------------------------------------------------------------------------- */
/* Public writers                                                             */
/* ------------------------------------------------------------------------- */

/* status.json: { "players_online": N, "uptime_sec": S, "version": "…" } */

// --- helper: escape JSON string safely ---

void json_write_status(void)
{
    const char *dir = resolve_api_dir();
    char path[PATH_MAX], buf[256];

    if (snprintf(path, sizeof(path), "%s/status.json", dir) >= (int)sizeof(path)) return;

    long long uptime = (long long)(current_time - boot_time);
    int online = players_online();

    snprintf(buf, sizeof(buf),
        "{"
          "\"players_online\": %d,"
          "\"uptime_sec\": %lld,"
          "\"version\": \"%s\""
        "}",
        online, uptime, DD4_VERSION_STR
    );

    write_atomic_json(path, buf);
}

void json_write_who(void)
{
    const char *dir = resolve_api_dir();
    char path[PATH_MAX], tmpfile[PATH_MAX];
    FILE *fp;
    DESCRIPTOR_DATA *d;
    int nImm = 0, nHero = 0, nMort = 0, count = 0;

    if (snprintf(path, sizeof(path), "%s/who.json", dir) >= (int)sizeof(path)) {
        return;
    }
    if (snprintf(tmpfile, sizeof(tmpfile), "%s.tmp", path) >= (int)sizeof(path)) {
        return;
    }

    fp = fopen(tmpfile, "w");
    if (!fp) {
        perror("json_write_who: fopen");
        return;
    }

    fprintf(fp, "{\n");
    fprintf(fp, "  \"immortals\": [\n");

    int firstImm = 1, firstHero = 1, firstMort = 1;

    for (d = descriptor_list; d; d = d->next) {
        CHAR_DATA *wch = d->original ? d->original : d->character;

        if ((d->connected != CON_PLAYING &&
             (d->connected < CON_NOTE_TO || d->connected > CON_NOTE_FINISH)) ||
            !wch)
            continue;

        if (!can_see(d->character, wch))
            continue;

        const char *name = wch->name ? wch->name : "";
        const char *clss = class_table[wch->class].who_name;
        const char *clan = (wch->clan > 0 ? clan_table[wch->clan].name : "");
        const char *race = (wch->race >= 0 && wch->race < MAX_RACE ? race_table[wch->race].race_name : "");
        const char *title = (wch->pcdata && wch->pcdata->title) ? wch->pcdata->title : "";

        char esc_name[MAX_STRING_LENGTH];
        char esc_clss[MAX_STRING_LENGTH];
        char esc_clan[MAX_STRING_LENGTH];
        char esc_race[MAX_STRING_LENGTH];
        char esc_title[MAX_STRING_LENGTH];

        json_escape_string(name, esc_name, sizeof(esc_name));
        json_escape_string(clss, esc_clss, sizeof(esc_clss));
        json_escape_string(clan, esc_clan, sizeof(esc_clan));
        json_escape_string(race, esc_race, sizeof(esc_race));
        json_escape_string(title, esc_title, sizeof(esc_title));

        if (wch->level > LEVEL_HERO) {
            if (!firstImm) fprintf(fp, ",\n");
            firstImm = 0;
            json_who_data(fp, wch);
            nImm++;
        }
    }

    fprintf(fp, "\n  ],\n  \"heroes\": [\n");

    for (d = descriptor_list; d; d = d->next) {
        CHAR_DATA *wch = d->original ? d->original : d->character;

        if ((d->connected != CON_PLAYING &&
             (d->connected < CON_NOTE_TO || d->connected > CON_NOTE_FINISH)) ||
            !wch)
            continue;

        if (!can_see(d->character, wch))
            continue;

        if (wch->level == LEVEL_HERO) {
            const char *name = wch->name ? wch->name : "";
            const char *clss = class_table[wch->class].who_name;
            const char *clan = (wch->clan > 0 ? clan_table[wch->clan].name : "");
            const char *race = (wch->race >= 0 && wch->race < MAX_RACE ? race_table[wch->race].race_name : "");
            const char *title = (wch->pcdata && wch->pcdata->title) ? wch->pcdata->title : "";

            char esc_name[MAX_STRING_LENGTH];
            char esc_clss[MAX_STRING_LENGTH];
            char esc_clan[MAX_STRING_LENGTH];
            char esc_race[MAX_STRING_LENGTH];
            char esc_title[MAX_STRING_LENGTH];

            json_escape_string(name, esc_name, sizeof(esc_name));
            json_escape_string(clss, esc_clss, sizeof(esc_clss));
            json_escape_string(clan, esc_clan, sizeof(esc_clan));
            json_escape_string(race, esc_race, sizeof(esc_race));
            json_escape_string(title, esc_title, sizeof(esc_title));

            if (!firstHero) fprintf(fp, ",\n");
            firstHero = 0;
            json_who_data(fp, wch);
            nHero++;
        }
    }

    fprintf(fp, "\n  ],\n  \"mortals\": [\n");

    for (d = descriptor_list; d; d = d->next) {
        CHAR_DATA *wch = d->original ? d->original : d->character;

        if ((d->connected != CON_PLAYING &&
             (d->connected < CON_NOTE_TO || d->connected > CON_NOTE_FINISH)) ||
            !wch)
            continue;

        if (!can_see(d->character, wch))
            continue;

        if (wch->level < LEVEL_HERO) {
            const char *name = wch->name ? wch->name : "";
            const char *clss = class_table[wch->class].who_name;
            const char *clan = (wch->clan > 0 ? clan_table[wch->clan].name : "");
            const char *race = (wch->race >= 0 && wch->race < MAX_RACE ? race_table[wch->race].race_name : "");
            const char *title = (wch->pcdata && wch->pcdata->title) ? wch->pcdata->title : "";

            char esc_name[MAX_STRING_LENGTH];
            char esc_clss[MAX_STRING_LENGTH];
            char esc_clan[MAX_STRING_LENGTH];
            char esc_race[MAX_STRING_LENGTH];
            char esc_title[MAX_STRING_LENGTH];

            json_escape_string(name, esc_name, sizeof(esc_name));
            json_escape_string(clss, esc_clss, sizeof(esc_clss));
            json_escape_string(clan, esc_clan, sizeof(esc_clan));
            json_escape_string(race, esc_race, sizeof(esc_race));
            json_escape_string(title, esc_title, sizeof(esc_title));

            if (!firstMort) fprintf(fp, ",\n");
            firstMort = 0;
            json_who_data(fp, wch);
            nMort++;
        }
    }

    fprintf(fp, "\n  ],\n");

    // Session stats
    for (d = descriptor_list; d; d = d->next) {
        if ((d->connected == CON_PLAYING ||
             (d->connected >= CON_NOTE_TO && d->connected <= CON_NOTE_FINISH))
            && can_see(d->character, d->character))
            count++;
    }
    max_on = UMAX(count, max_on);

    fprintf(fp, "  \"stats\": {\"current\": %d, \"max_session\": %d}\n", count, max_on);
    fprintf(fp, "}\n");

    fclose(fp);

    if (rename(tmpfile, path) < 0) {
        perror("json_write_who: rename");
        unlink(tmpfile);
    }
}


void json_write_events(CHAR_DATA *ch, const char *message)
{
    EVENT_ENTRY events[MAX_EVENTS + 1];
    int count = 0;

    // Load existing JSON
    FILE *f = fopen(EVENTS_FILE, "r");
    if (f) {
        char buf[8192];
        size_t n = fread(buf, 1, sizeof(buf) - 1, f);
        buf[n] = '\0';
        fclose(f);

        char *p = buf;
        while ((p = strstr(p, "\"timestamp\"")) && count < MAX_EVENTS) {
            EVENT_ENTRY *e = &events[count];
            char player[MAX_INPUT_LENGTH] = "";
            char msg[MAX_STRING_LENGTH] = "";
            long ts = 0;

            sscanf(p, "\"timestamp\":%ld", &ts);
            char *pp = strstr(p, "\"player\"");
            char *pm = strstr(p, "\"message\"");
            if (pp) sscanf(pp, "\"player\":\"%[^\"]\"", player);
            if (pm) sscanf(pm, "\"message\":\"%[^\"]\"", msg);

            e->timestamp = ts;
            strncpy(e->player, player, sizeof(e->player)-1);
            strncpy(e->message, msg, sizeof(e->message)-1);

            count++;
            p++;
        }
    }

    // Add new event
    if (message && *message) {
        EVENT_ENTRY *e = &events[count++];
        e->timestamp = time(NULL);
        strncpy(e->player, IS_NPC(ch) ? "NPC" : ch->name, sizeof(e->player)-1);
        strncpy(e->message, message, sizeof(e->message)-1);
    }

    if (count > MAX_EVENTS) {
        memmove(events, events + (count - MAX_EVENTS),
                sizeof(EVENT_ENTRY) * MAX_EVENTS);
        count = MAX_EVENTS;
    }

    // Write fresh JSON
    char tmpname[256];
    snprintf(tmpname, sizeof(tmpname), "%s.tmp", EVENTS_FILE);
    f = fopen(tmpname, "w");
    if (!f) return;

    fprintf(f, "[\n");
    for (int i = 0; i < count; i++) {
        char esc_player[2*MAX_INPUT_LENGTH];
        char esc_message[2*MAX_STRING_LENGTH];

        json_escape_string(events[i].player, esc_player, sizeof(esc_player));
        json_escape_string(events[i].message, esc_message, sizeof(esc_message));

        fprintf(f,
            "  {\"timestamp\":%ld,\"player\":\"%s\",\"message\":\"%s\"}%s\n",
            (long)events[i].timestamp,
            esc_player,
            esc_message,
            (i == count - 1 ? "" : ","));
    }
    fprintf(f, "]\n");
    fclose(f);

    rename(tmpname, EVENTS_FILE);
}


void json_write_changes(const char *infile, const char *outfile) {
    FILE *fp = fopen(infile, "r");
    if (!fp) {
        perror(infile);
        return;
    }

    char line[MAX_LINE];
    int in_changes = 0;
    ChangeEntry entries[512];
    int count = 0;
    char buffer[65536];
    buffer[0] = '\0';
    int in_bullet = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Start of CHANGES section
        if (!in_changes) {
            if (strstr(line, "0 CHANGES~")) {
                in_changes = 1;
            }
            continue;
        }

        // End of CHANGES (~ on its own line)
        if (line[0] == '~')
            break;

        // Detect a new section header {WMonth YEAR{x
        if (line[0] == '{' && line[1] == 'W') {
            // save previous entry text
            if (count > 0) {
                entries[count-1].text = strdup(buffer);
                buffer[0] = '\0';
            }

            char month[16];
            int year;
            if (sscanf(line, "{W%15s %d", month, &year) == 2) {
                strcpy(entries[count].month, month);
                entries[count].year = year;
                entries[count].text = NULL;
                count++;
            }
            continue;
        }

        // --- Clean input line ---
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        // Collapse multiple spaces → single
        char clean[MAX_LINE];
        {
            int j = 0;
            int space_seen = 0;
            for (size_t i = 0; i < len; i++) {
                if (line[i] == ' ' || line[i] == '\t') {
                    if (!space_seen) {
                        clean[j++] = ' ';
                        space_seen = 1;
                    }
                } else {
                    clean[j++] = line[i];
                    space_seen = 0;
                }
            }
            // strip trailing space
            while (j > 0 && clean[j-1] == ' ')
                j--;
            clean[j] = '\0';
        }

        // --- Bullet / continuation / paragraph handling ---
        if (clean[0] == '-' && clean[1] == ' ') {
            // New bullet: newline first
            strncat(buffer, (strlen(buffer) > 0 ? "\n" : ""), sizeof(buffer) - strlen(buffer) - 1);
            strncat(buffer, clean, sizeof(buffer) - strlen(buffer) - 1);
            in_bullet = 1;
        } else if (in_bullet && clean[0] != '\0') {
            // Continuation of bullet → inline with space
            strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 1);
            strncat(buffer, clean, sizeof(buffer) - strlen(buffer) - 1);
        } else {
            in_bullet = 0;
            if (clean[0] == '\0') {
                // blank line = paragraph break
                strncat(buffer, "\n\n", sizeof(buffer) - strlen(buffer) - 1);
            } else {
                // normal paragraph line: collapse wraps into space
                if (buffer[0] && buffer[strlen(buffer)-1] != '\n')
                    strncat(buffer, " ", sizeof(buffer) - strlen(buffer) - 1);
                strncat(buffer, clean, sizeof(buffer) - strlen(buffer) - 1);
            }
        }
    }

    // Save last entry
    if (count > 0) {
        entries[count-1].text = strdup(buffer);
    }

    fclose(fp);

    // --- Write JSON ---
    FILE *out = fopen(outfile, "w");
    if (!out) {
        perror(outfile);
        return;
    }

    fprintf(out, "[\n");
    for (int i = 0; i < count; i++) {
        char esc_text[131072];
        json_escape_string(entries[i].text ? entries[i].text : "", esc_text, sizeof(esc_text));
        fprintf(out,
            "  {\n"
            "    \"month\": \"%s\",\n"
            "    \"year\": %d,\n"
            "    \"text\": \"%s\"\n"
            "  }%s\n",
            entries[i].month,
            entries[i].year,
            esc_text,
            (i == count-1 ? "" : ","));
    }
    fprintf(out, "]\n");
    fclose(out);

    // cleanup
    for (int i = 0; i < count; i++) {
        free(entries[i].text);
    }
}

void json_write_races(void) {
    const char *dir = resolve_api_dir();
    char path[PATH_MAX];
    FILE *fp;

    if (snprintf(path, sizeof(path), "%s/race.json", dir) >= (int)sizeof(path)) {
        return;
    }

    fp = fopen(path, "w");
    if (!fp) {
        perror("json_write_races: fopen");
        return;
    }

    fprintf(fp, "[\n");

    for (int i = 1; i < MAX_RACE; i++) {  // skip RACE_NONE
        const struct race_struct *r = &race_table[i];
        const struct ws_race_terrain *t = &ws_race_terrain_mod[i];

        // Escape strings
        char esc_who[MAX_STRING_LENGTH], esc_name[MAX_STRING_LENGTH];
        char esc_skill1[MAX_STRING_LENGTH], esc_skill2[MAX_STRING_LENGTH];
        json_escape_string(r->who_name, esc_who, sizeof(esc_who));
        json_escape_string(r->race_name, esc_name, sizeof(esc_name));
        json_escape_string(r->spell_one, esc_skill1, sizeof(esc_skill1));
        json_escape_string(r->spell_two, esc_skill2, sizeof(esc_skill2));

        fprintf(fp,
            "  {\n"
            "    \"id\": %d,\n"
            "    \"who_name\": \"%s\",\n"
            "    \"name\": \"%s\",\n"
            "    \"stats\": {\"str\": %d, \"int\": %d, \"wis\": %d, \"dex\": %d, \"con\": %d},\n"
            "    \"regen\": {\"hp\": %d, \"mana\": %d, \"move\": %d},\n"
            "    \"skills\": [\"%s\", \"%s\"],\n"
            "    \"size\": \"%s\",\n"
            "    \"terrain_mods\": {"
            "\"inside\": %d, \"city\": %d, \"field\": %d, \"forest\": %d, \"hills\": %d, "
            "\"mountain\": %d, \"water_swim\": %d, \"water_noswim\": %d, "
            "\"underwater\": %d, \"air\": %d, \"desert\": %d, \"swamp\": %d, \"underwater_ground\": %d"
            "}\n"
            "  }%s\n",
            i,
            esc_who, esc_name,
            r->str_bonus, r->int_bonus, r->wis_bonus, r->dex_bonus, r->con_bonus,
            r->hp_regen, r->mana_regen, r->move_regen,
            esc_skill1, esc_skill2,
            (r->size == CHAR_SIZE_SMALL ? "Small" :
             r->size == CHAR_SIZE_MEDIUM ? "Medium" :
             r->size == CHAR_SIZE_LARGE ? "Large" : "Unknown"),
            t->race_mod[SECT_INSIDE], t->race_mod[SECT_CITY],
            t->race_mod[SECT_FIELD], t->race_mod[SECT_FOREST],
            t->race_mod[SECT_HILLS], t->race_mod[SECT_MOUNTAIN],
            t->race_mod[SECT_WATER_SWIM], t->race_mod[SECT_WATER_NOSWIM],
            t->race_mod[SECT_UNDERWATER], t->race_mod[SECT_AIR],
            t->race_mod[SECT_DESERT], t->race_mod[SECT_SWAMP],
            t->race_mod[SECT_UNDERWATER_GROUND],
            (i == MAX_RACE - 1 ? "" : ",")
        );
    }

    fprintf(fp, "]\n");
    fclose(fp);
}

void json_write_areas(void)  { /* TODO */ }
