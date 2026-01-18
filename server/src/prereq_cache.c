/***************************************************************************
 *  Prerequisite cache implementation for skill tree visualization         *
 *  Intent: Build and maintain cached prerequisite data at startup         *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* Global prerequisite cache - defined here */
struct skill_prereq_data skill_prereq_cache[MAX_SKILL];

/*
 * Intent: Build prerequisite cache at server startup for efficient skill tree visualization
 * Inputs: None (reads from global pre_req_table and skill_table)
 * Outputs: Populates skill_prereq_cache with linked lists of prerequisites
 * Preconditions: skill_table must be initialized with gsn assignments
 * Postconditions: Cache contains all valid prerequisites grouped by skill number
 * Failure Behavior: Logs errors for invalid skill numbers, cycles detected; skips invalid entries
 * Performance: O(n) where n is MAX_PRE_REQ entries; runs once at startup
 * Notes: Logs comprehensive statistics for monitoring and debugging
 */
void cache_prerequisites(void)
{
        int i, sn, pre_req_sn;
        int total_processed = 0;
        int skills_with_prereqs = 0;
        int invalid_skills_skipped = 0;
        int invalid_prereqs_skipped = 0;
        int cycles_detected = 0;
        int or_groups_found[28] = {0}; /* Track OR groups 1-27 */
        time_t start_time, end_time;
        char buf[MAX_STRING_LENGTH];
        char cycle_path[MAX_STRING_LENGTH];
        PREREQ_NODE *new_node;
        bool visited[MAX_SKILL];

        start_time = current_time;

        log_string("Prerequisite cache: Initializing...");

        /* Initialize cache to empty */
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                skill_prereq_cache[sn].prereqs = NULL;
                skill_prereq_cache[sn].prereq_count = 0;
        }

        /* Walk pre_req_table and build cache */
        for (i = 0; i < MAX_PRE_REQ; i++)
        {
                total_processed++;

                /* Skip if pointers are NULL */
                if (!pre_req_table[i].skill || !pre_req_table[i].pre_req)
                        continue;

                sn = *pre_req_table[i].skill;
                pre_req_sn = *pre_req_table[i].pre_req;

                /* Validate skill number */
                if (sn < 0 || sn >= MAX_SKILL)
                {
                        sprintf(buf, "Prerequisite cache: Invalid skill sn=%d at pre_req_table[%d] (skipping)", sn, i);
                        log_string(buf);
                        invalid_skills_skipped++;
                        continue;
                }

                /* Validate prerequisite skill number */
                if (pre_req_sn < 0 || pre_req_sn >= MAX_SKILL)
                {
                        sprintf(buf, "Prerequisite cache: Invalid pre_req sn=%d for skill '%s' at pre_req_table[%d] (skipping)",
                                pre_req_sn, skill_table[sn].name ? skill_table[sn].name : "unknown", i);
                        log_string(buf);
                        invalid_prereqs_skipped++;
                        continue;
                }

                /* Check for direct self-reference cycle */
                if (sn == pre_req_sn)
                {
                        sprintf(buf, "Prerequisite cache: WARNING - Cycle detected: skill '%s' (sn=%d) requires itself (breaking cycle)",
                                skill_table[sn].name ? skill_table[sn].name : "unknown", sn);
                        log_string(buf);
                        cycles_detected++;
                        continue;
                }

                /* Allocate new prerequisite node */
                new_node = (PREREQ_NODE *)malloc(sizeof(PREREQ_NODE));
                if (!new_node)
                {
                        sprintf(buf, "Prerequisite cache: FATAL - Failed to allocate memory for prereq node");
                        bug(buf, 0);
                        continue;
                }

                new_node->pre_req_sn = pre_req_sn;
                new_node->min_proficiency = pre_req_table[i].min;
                new_node->group = pre_req_table[i].group;
                new_node->next = skill_prereq_cache[sn].prereqs;

                skill_prereq_cache[sn].prereqs = new_node;
                skill_prereq_cache[sn].prereq_count++;

                /* Track OR groups (1-27) */
                if (pre_req_table[i].group > 0 && pre_req_table[i].group <= 27)
                {
                        or_groups_found[pre_req_table[i].group]++;
                }
        }

        /* Count skills with prerequisites */
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                if (skill_prereq_cache[sn].prereq_count > 0)
                        skills_with_prereqs++;
        }

        end_time = current_time;

        /* Log comprehensive statistics */
        sprintf(buf, "Prerequisite cache: Build complete in %ld seconds", end_time - start_time);
        log_string(buf);
        sprintf(buf, "  - Processed %d prerequisite entries", total_processed);
        log_string(buf);
        sprintf(buf, "  - %d skills have prerequisites", skills_with_prereqs);
        log_string(buf);

        /* Count unique OR groups */
        {
                int unique_or_groups = 0;
                int group;
                for (group = 1; group <= 27; group++)
                {
                        if (or_groups_found[group] > 0)
                                unique_or_groups++;
                }
                sprintf(buf, "  - %d unique OR groups found (groups 1-27)", unique_or_groups);
                log_string(buf);
        }

        if (invalid_skills_skipped > 0)
        {
                sprintf(buf, "  - WARNING: %d invalid skill numbers skipped", invalid_skills_skipped);
                log_string(buf);
        }
        if (invalid_prereqs_skipped > 0)
        {
                sprintf(buf, "  - WARNING: %d invalid prerequisite skill numbers skipped", invalid_prereqs_skipped);
                log_string(buf);
        }
        if (cycles_detected > 0)
        {
                sprintf(buf, "  - WARNING: %d direct cycles detected and broken", cycles_detected);
                log_string(buf);
        }

        log_string("Prerequisite cache: Ready");
}
