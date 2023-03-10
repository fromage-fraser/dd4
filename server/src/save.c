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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined(macintosh)
extern  int     _filbuf         args((FILE *));
#endif

#if defined(ultrix) || defined(sequent) || defined (SunOS) || defined (linux)
void    system          args((char *string));
#endif

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [ MAX_NEST ];



/*
 * Local functions.
 */
void    fwrite_char     args((CHAR_DATA *ch,  FILE *fp));
void    fwrite_obj      args((CHAR_DATA *ch,  OBJ_DATA  *obj,  FILE *fp, int iNest, bool vault));
void    fread_char      args((CHAR_DATA *ch,  FILE *fp));
void    fread_obj       args((CHAR_DATA *ch,  FILE *fp, bool vault));

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 * some of the infrastructure is provided.
 */
void save_char_obj (CHAR_DATA *ch)
{
        FILE *fp;
        FILE *vp;
        char  buf           [ MAX_STRING_LENGTH ];
        char  strsave       [ MAX_INPUT_LENGTH  ];
        char  strvaultsave  [ MAX_INPUT_LENGTH  ];

        if (IS_NPC(ch) || ch->level < 2)
                return;

        if (ch->desc && ch->desc->original)
                ch = ch->desc->original;

        ch->save_time = current_time;
        fclose(fpReserve);

        /* player files parsed directories by Yaz 4th Realm */
#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strsave, "%s%s%s%s", PLAYER_DIR, initial(ch->name),"/", capitalize(ch->name));
#else
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
#endif

    /* For .vault files */
#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strvaultsave, "%s%s%s%s.vault", PLAYER_DIR, initial(ch->name),"/", capitalize(ch->name));
#else
        sprintf(strvaultsave, "%s%s.vault", PLAYER_DIR, capitalize(ch->name));
#endif

        if (!(fp = fopen(strsave, "w")))
        {
                sprintf(buf, "Save_char_obj: fopen %s: ", ch->name);
                bug(buf, 0);
                perror(strsave);
        }
        else
        {
                fwrite_char(ch, fp);

                if (ch->carrying)
                        fwrite_obj(ch, ch->carrying, fp, 0, FALSE);

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        /* For vault file - Owl 23/2/22 */

        if (!(vp = fopen(strvaultsave, "w")))
        {
                sprintf(buf, "Save_char_obj: fopen %s.vault: ", ch->name);
                bug(buf, 0);
                perror(strvaultsave);
        }
        else
        {
                if (ch->pcdata->vault)
                {
                        fwrite_obj(ch, ch->pcdata->vault, vp, 0, TRUE);
                }

                fprintf(vp, "#END\n");
                fclose(vp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


/*
 * Write the char.
 */
void fwrite_char (CHAR_DATA *ch, FILE *fp)
{
        AFFECT_DATA *paf;
        int sn, i;

        if (IS_NPC(ch))
                return;

        fprintf(fp, "#PLAYER\n");
        fprintf(fp, "Nm          %s~\n", ch->name);
        fprintf(fp, "ShtDsc      %s~\n", ch->short_descr);
        fprintf(fp, "LngDsc      %s~\n", ch->long_descr);
        fprintf(fp, "Dscr        %s~\n", ch->description);
        fprintf(fp, "Prmpt       %s~\n", ch->prompt);
        fprintf(fp, "Sx          %d\n", ch->sex);
        fprintf(fp, "Cla         %d\n", ch->class);
        fprintf(fp, "SbCla       %d\n", ch->sub_class);
        fprintf(fp, "Rce         %d\n", ch->race);
        fprintf(fp, "Clan        %d\n", ch->clan);
        fprintf(fp, "ClnLvl      %d\n", ch->clan_level);
        fprintf(fp, "Lvl         %d\n", ch->level);
        fprintf(fp, "Trst        %d\n", ch->trust);
        fprintf(fp, "Wizbt       %d\n", ch->wizbit);
        fprintf(fp, "Playd       %d\n", ch->played + (int) (current_time - ch->logon));
        fprintf(fp, "Note        %ld\n", ch->last_note);

        fprintf(fp, "Room        %d\n",
                (ch->in_room == get_room_index(ROOM_VNUM_LIMBO) && ch->was_in_room)
                ? ch->was_in_room->vnum
                : ch->in_room->vnum);

        fprintf(fp, "HpMnMv      %d %d %d %d %d %d\n",
                ch->hit,
                ch->max_hit,
                ch->mana,
                ch->max_mana,
                ch->move,
                ch->max_move);

        fprintf(fp, "Rage        %d\n",  ch->rage);
        fprintf(fp, "Rage_Max    %d\n",  ch->max_rage);
        fprintf(fp, "Aggro_Dam   %d\n",  ch->aggro_dam);
        fprintf(fp, "Platinum    %d\n",  ch->plat);
        fprintf(fp, "Gold        %d\n",  ch->gold);
        fprintf(fp, "Silver      %d\n",  ch->silver);
        fprintf(fp, "Copper      %d\n",  ch->copper);
        fprintf(fp, "Steel       %d\n",  ch->smelted_steel);
        fprintf(fp, "Titanium    %d\n",  ch->smelted_titanium);
        fprintf(fp, "Adamantite  %d\n",  ch->smelted_adamantite);
        fprintf(fp, "Electrum    %d\n",  ch->smelted_electrum);
        fprintf(fp, "Starmetal   %d\n",  ch->smelted_starmetal);
        fprintf(fp, "Bank        %d\n",  ch->pcdata->bank);
        fprintf(fp, "Bounty      %d\n",  ch->pcdata->bounty);
        fprintf(fp, "Exp         %d\n",  ch->exp);
        fprintf(fp, "Act         %lu\n", ch->act);
        fprintf(fp, "Status      %d\n",  ch->status);
        fprintf(fp, "AffdBy      %lu\n", ch->affected_by);
        fprintf(fp, "Pos         %d\n",
                ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

        fprintf(fp, "Prac        %d\n",  ch->pcdata->str_prac);
        fprintf(fp, "SplPrac     %d\n",  ch->pcdata->int_prac);
        fprintf(fp, "StatTrain   %d\n",  ch->pcdata->stat_train);
        fprintf(fp, "SavThr      %d\n",  ch->saving_throw);
        fprintf(fp, "Align       %d\n",  ch->alignment);
        fprintf(fp, "Hit         %d\n",  ch->hitroll);
        fprintf(fp, "Dam         %d\n",  ch->damroll);
        fprintf(fp, "Armr        %d\n",  ch->armor);
        fprintf(fp, "Wimp        %d\n",  ch->wimpy);
        fprintf(fp, "Deaf        %d\n",  ch->deaf);
        fprintf(fp, "Kills       %d\n",  ch->pcdata->kills);
        fprintf(fp, "Killed      %d\n",  ch->pcdata->killed);
        fprintf(fp, "Pdeaths     %d\n",  ch->pcdata->pdeaths);
        fprintf(fp, "Pkills      %d\n",  ch->pcdata->pkills);
        fprintf(fp, "Pkscore     %d\n",  ch->pcdata->pkscore);
        fprintf(fp, "Form        %d\n",  ch->form);
        fprintf(fp, "Gag         %d\n",  ch->gag);
        fprintf(fp, "DamageMit   %d\n",  ch->damage_mitigation);
        fprintf(fp, "DamangeEnh  %d\n",  ch->damage_enhancement);
        fprintf(fp, "Crit        %d\n",  ch->crit);
        fprintf(fp, "Swiftness  %d\n",  ch->swiftness);
        fprintf(fp, "ResistAcid  %d\n",  ch->resist_acid);
        fprintf(fp, "ResistLightning  %d\n",  ch->resist_lightning);
        fprintf(fp, "ResistHeat  %d\n",  ch->resist_heat);
        fprintf(fp, "ResistCold  %d\n",  ch->resist_cold);
        fprintf(fp, "Quiet       %d\n",  ch->silent_mode);
        fprintf(fp, "AllowLook   %d\n",  ch->pcdata->allow_look);
        fprintf(fp, "XPLevel     %d\n",  ch->pcdata->level_xp_loss);
        fprintf(fp, "AirSupply   %d\n",  ch->pcdata->air_supply);
        fprintf(fp, "Paswd       %s~\n", ch->pcdata->pwd);
        fprintf(fp, "Bmfin       %s~\n", ch->pcdata->bamfin);
        fprintf(fp, "Bmfout      %s~\n", ch->pcdata->bamfout);
        fprintf(fp, "Ttle        %s~\n", ch->pcdata->title);

        fprintf(fp, "AtrPrm      %d %d %d %d %d\n",
                ch->pcdata->perm_str,
                ch->pcdata->perm_int,
                ch->pcdata->perm_wis,
                ch->pcdata->perm_dex,
                ch->pcdata->perm_con);

        fprintf(fp, "AtrMd       %d %d %d %d %d\n",
                ch->pcdata->mod_str,
                ch->pcdata->mod_int,
                ch->pcdata->mod_wis,
                ch->pcdata->mod_dex,
                ch->pcdata->mod_con);

        fprintf(fp, "Cond        %d %d %d\n",
                ch->pcdata->condition[0],
                ch->pcdata->condition[1],
                ch->pcdata->condition[2]);

        fprintf (fp, "Colors      %d %d %d %d %d %d %d %d %d %d\n",
                 ch->colors[0],
                 ch->colors[1],
                 ch->colors[2],
                 ch->colors[3],
                 ch->colors[4],
                 ch->colors[5],
                 ch->colors[6],
                 ch->colors[7],
                 ch->colors[8],
                 ch->colors[9]);

        fprintf(fp, "CurRecall   %d\n", ch->pcdata->current_recall);
        fprintf(fp, "SpellAttk   %d\n", ch->pcdata->spell_attacks);
        fprintf(fp, "Blink       %d\n", ch->pcdata->blink);

        fprintf(fp, "RecPoints  ");
        {
                for (i = 0; i < MAX_RECALL_POINTS; i++)
                        fprintf(fp, " %d", ch->pcdata->recall_points[i]);
                fprintf(fp, "\n");
        }

        fprintf(fp, "Pglen       %d\n", ch->pcdata->pagelen);
        fprintf(fp, "ChSub       %d\n", ch->pcdata->choose_subclass);
        fprintf(fp, "Fame        %d\n", ch->pcdata->fame);
        fprintf(fp, "Saved       %ld\n", ch->save_time);

        if (ch->pcdata->totalqp)
                fprintf(fp, "QPTotal     %d\n",  ch->pcdata->totalqp);

        if (ch->pcdata->questpoints)
                fprintf(fp, "QuestPnts   %d\n",  ch->pcdata->questpoints);

        if (ch->pcdata->nextquest)
                fprintf(fp, "QuestNext   %d\n",  ch->pcdata->nextquest);
        else if (ch->pcdata->countdown)
                fprintf(fp, "QuestNext   %d\n",  QUEST_ABORT_DELAY);

        fprintf (fp, "Patron      %s~\n",
                 (ch->pcdata->deity_patron < 0
                  || ch->pcdata->deity_patron >= NUMBER_DEITIES)
                 ? "None" : deity_info_table[ch->pcdata->deity_patron].name);

        fprintf (fp, "DeityTimer  %d\n", ch->pcdata->deity_timer);
        fprintf (fp, "DeityFlags  %d\n", ch->pcdata->deity_flags);

        for (i = 0; i < NUMBER_DEITIES; i++)
        {
                fprintf (fp, "DeityFavour %s~ %d\n",
                         deity_info_table[i].name,
                         ch->pcdata->deity_favour[i]);
        }

        fprintf (fp, "DeityType   ");
        for (i = 0; i < DEITY_NUMBER_TYPES; i++)
                fprintf(fp, "%d ", ch->pcdata->deity_type_timer[i]);
        fprintf(fp, "\n");

        fprintf (fp, "DeityPerson ");
        for (i = 0; i < DEITY_NUMBER_PERSONALITIES; i++)
                fprintf(fp, "%d ", ch->pcdata->deity_personality_timer[i]);
        fprintf(fp, "\n");

        fprintf (fp, "Boards      %d ", MAX_BOARD);
        for (i = 0; i < MAX_BOARD; i++)
                fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
        fprintf (fp, "\n");

        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                if (skill_table[sn].name && ch->pcdata->learned[sn] > 0)
                {
                        fprintf(fp, "Skll        %d '%s'\n",
                                ch->pcdata->learned[sn], skill_table[sn].name);
                }
        }

        for (paf = ch->affected; paf; paf = paf->next)
        {
                if (paf->deleted)
                        continue;

                fprintf(fp, "Aff         %3d %3d %3d %3d %20lu\n",
                        paf->type,
                        paf->duration,
                        paf->modifier,
                        paf->location,
                        paf->bitvector);
        }

        fprintf(fp, "End\n\n");
}



/*
 * Write an object and its contents.
 */
void fwrite_obj (CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, bool vault)
{
        AFFECT_DATA      *paf;
        EXTRA_DESCR_DATA *ed;

        /*
         * Slick recursion to write lists backwards,
         *   so loading them will load in forwards order.
         */
        if (obj->next_content)
                fwrite_obj(ch, obj->next_content, fp, iNest, vault);

        /*
         * Castrate storage characters.
         */
        if (obj->item_type == ITEM_KEY || obj->deleted)
                return;

        /* Don't put worn gear in vault */

        if( obj->wear_loc >= 0 && vault)
            return;

        /*
         * Prevent eq stealing and loss of clan healing items
         */
        if ( ( vault && ( ( ( ch->level + VAULT_LEVEL_BUFFER ) < obj->level ) ) )
          || (!vault && ( ch->level < obj->level ) )
          || ( obj->item_type == ITEM_CLAN_OBJECT ) )
                return;

        fprintf(fp, "#OBJECT\n");
        fprintf(fp, "Nest         %d\n",        iNest                       );
        fprintf(fp, "Name         %s~\n",       obj->name                   );
        fprintf(fp, "ShortDescr   %s~\n",       obj->short_descr            );
        fprintf(fp, "Description  %s~\n",       obj->description            );
        fprintf(fp, "Vnum         %d\n",        obj->pIndexData->vnum       );
        fprintf(fp, "ExtraFlags   %lu\n",       obj->extra_flags            );
        fprintf(fp, "WearFlags    %d\n",        obj->wear_flags             );
        fprintf(fp, "EgoFlags     %d\n",        obj->ego_flags              );
        fprintf(fp, "WearLoc      %d\n",        obj->wear_loc               );
        fprintf(fp, "ItemType     %d\n",        obj->item_type              );
        fprintf(fp, "Identified   %d\n",        obj->identified             );
        fprintf(fp, "Weight       %d\n",        obj->weight                 );
        fprintf(fp, "Level        %d\n",        obj->level                  );
        fprintf(fp, "Timer        %d\n",        obj->timer                  );
        fprintf(fp, "TimerMax     %d\n",        obj->timermax               );
        fprintf(fp, "Cost         %d\n",        obj->cost                   );
        fprintf(fp, "HowCreated   %d\n",        obj->how_created            );
        fprintf(fp, "Values       %d %d %d %d\n",
                obj->value[0], obj->value[1], obj->value[2], obj->value[3]  );

        switch (obj->item_type)
        {
            case ITEM_POTION:
            case ITEM_PAINT:
            case ITEM_SCROLL:
                if (obj->value[1] > 0)
                {
                        fprintf(fp, "Spell 1      '%s'\n",
                                skill_table[obj->value[1]].name);
                }

                if (obj->value[2] > 0)
                {
                        fprintf(fp, "Spell 2      '%s'\n",
                                skill_table[obj->value[2]].name);
                }

                if (obj->value[3] > 0)
                {
                        fprintf(fp, "Spell 3      '%s'\n",
                                skill_table[obj->value[3]].name);
                }

                break;

            case ITEM_PILL:
            case ITEM_STAFF:
            case ITEM_WAND:
            case ITEM_COMBAT_PULSE:
            case ITEM_DEFENSIVE_PULSE:
                if (obj->value[3] > 0)
                {
                        fprintf(fp, "Spell 3      '%s'\n",
                                skill_table[obj->value[3]].name);
                }

                break;
        }

        for (paf = obj->affected; paf; paf = paf->next)
        {
                fprintf(fp, "Affect       %d %d %d %d %lu\n",
                        paf->type,
                        paf->duration,
                        paf->modifier,
                        paf->location,
                        paf->bitvector);
        }

        for (ed = obj->extra_descr; ed; ed = ed->next)
        {
                fprintf(fp, "ExtraDescr   %s~ %s~\n",
                        ed->keyword, ed->description);
        }

        fprintf(fp, "Owner        %s~\n", get_obj_owner(obj));

        fprintf(fp, "End\n\n");

        if (obj->contains)
                fwrite_obj(ch, obj->contains, fp, iNest + 1, vault);

        tail_chain();
}


/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj (DESCRIPTOR_DATA *d, char *name)
{
        FILE      *fp;
        FILE      *vp;
        static PC_DATA    pcdata_zero;
        CHAR_DATA *ch;
#if !defined(MSDOS)
        char       buf          [ MAX_STRING_LENGTH ];
#endif
        char       strsave      [ MAX_INPUT_LENGTH ];
        char       strvaultsave [ MAX_INPUT_LENGTH ];
        bool       found;
        int        next;

        if (!char_free)
                ch = alloc_perm(sizeof(*ch));
        else
        {
                ch = char_free;
                char_free = char_free->next;
        }

        clear_char(ch);

        if (!pcdata_free)
                ch->pcdata = alloc_perm(sizeof(*ch->pcdata));
        else
        {
                ch->pcdata = pcdata_free;
                pcdata_free = pcdata_free->next;
        }

        *ch->pcdata = pcdata_zero;

        d->character = ch;
        ch->desc = d;
        ch->name = str_dup(name);
        ch->prompt = str_dup("<%hhp %mm %vmv> ");
        ch->last_note = 0;
        ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
        ch->gag = 0;
        ch->backstab = 0;
        ch->edrain = 0;
        ch->silent_mode = 0;
        ch->pcdata->allow_look = 0;
        ch->pcdata->level_xp_loss = 0;
        ch->crit = 5;
        ch->swiftness = 5;
        ch->resist_acid = 0;
        ch->resist_lightning = 0;
        ch->resist_heat = 0;
        ch->resist_cold = 0;
        ch->pcdata->air_supply = FULL_AIR_SUPPLY;
        ch->tournament_team = -1;
        ch->pcdata->deity_patron = -1;
        ch->pcdata->deity_timer = DEITY_DEFAULT_PLAYER_TIMER;
        ch->pcdata->deity_flags = 0;
        ch->pcdata->blink = 0;
        ch->pcdata->str_prac = 0;
        ch->pcdata->int_prac = 0;
        ch->pcdata->review_stamp = current_time;
        ch->pcdata->pattern = 0;
        ch->pcdata->soar = 0;

        for (next = 0; next < NUMBER_DEITIES; next++)
                ch->pcdata->deity_favour[next] = -1;

        for (next = 0; next < DEITY_NUMBER_TYPES; next++)
                ch->pcdata->deity_type_timer[next] = 0;

        for (next = 0; next < DEITY_NUMBER_PERSONALITIES; next++)
                ch->pcdata->deity_personality_timer[next] = 0;

        ch->pcdata->board = &boards[DEFAULT_BOARD];
        ch->pcdata->pwd = str_dup("");
        ch->pcdata->bamfin = str_dup("");
        ch->pcdata->bamfout = str_dup("");
        ch->pcdata->title = str_dup("");
        ch->pcdata->confirm_delete = FALSE;
        ch->pcdata->choose_subclass = FALSE;
        ch->pcdata->fame = 0;
        ch->pcdata->pkscore = 0;
        ch->pcdata->dam_bonus = 0;
        ch->pcdata->perm_str = 10;
        ch->pcdata->perm_int = 10;
        ch->pcdata->perm_wis = 10;
        ch->pcdata->perm_dex = 10;
        ch->pcdata->perm_con = 10;
        ch->pcdata->condition[COND_THIRST] = 48;
        ch->pcdata->condition[COND_FULL] = 48;
        ch->pcdata->stat_train = APPLY_STR;
        ch->pcdata->pagelen = 100;
        ch->pcdata->switched = FALSE;
        ch->pcdata->group_support_bonus = 0;

        for (next = 0; next < MAX_WEAR; next++)
                ch->pcdata->morph_list[next] = NULL;

        ch->pcdata->current_recall = 0;
        ch->pcdata->recall_points[0] = DEFAULT_RECALL;

        for (next = 1; next < MAX_RECALL_POINTS; next++)
                        ch->pcdata->recall_points[next] = -1;

        found = FALSE;
        fclose(fpReserve);

        /* parsed player file directories by Yaz of 4th Realm */
        /* decompress if .gz file exists - Thx Alander */
#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strsave, "%s%s%s%s%s", PLAYER_DIR, initial(ch->name),
                "/", capitalize(name), ".gz");
        if ((fp = fopen(strsave, "r")))
        {
                fclose(fp);
                sprintf(buf, "gzip -dfq %s", strsave);
                system(buf);
        }
#endif

#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strsave, "%s%s%s%s", PLAYER_DIR, initial(ch->name),
                "/", capitalize(name));
#else
        sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(name));
#endif

        if ((fp = fopen(strsave, "r")))
        {
                int iNest;

                for (iNest = 0; iNest < MAX_NEST; iNest++)
                        rgObjNest[iNest] = NULL;

                found = TRUE;

                while (1)
                {
                        char letter;
                        char *word;

                        letter = fread_letter(fp);

                        if (letter == '*')
                        {
                                fread_to_eol(fp);
                                continue;
                        }

                        if (letter != '#')
                        {
                                bug("Load_char_obj: # not found.", 0);
                                break;
                        }

                        word = fread_word(fp);

                        if (!str_cmp(word, "PLAYER"))
                                fread_char(ch, fp);
                        else if (!str_cmp(word, "OBJECT"))
                                fread_obj(ch, fp, FALSE);
                        else if (!str_cmp(word, "END"))
                                break;
                        else
                        {
                                bug("Load_char_obj: bad section.", 0);
                                break;
                        }
                }

                fclose(fp);
        }

 /* parsed player file directories by Yaz of 4th Realm */
        /* decompress if .gz file exists - Thx Alander */
        /* Modified for vault files - Owl 23/2/23 */
#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strvaultsave, "%s%s%s%s%s.vault", PLAYER_DIR, initial(ch->name),
                "/", capitalize(name), ".gz");
        if ((vp = fopen(strvaultsave, "r")))
        {
                fclose(vp);
                sprintf(buf, "gzip -dfq %s", strvaultsave);
                system(buf);
        }
#endif

#if !defined(macintosh) && !defined(MSDOS)
        sprintf(strvaultsave, "%s%s%s%s.vault", PLAYER_DIR, initial(ch->name),
                "/", capitalize(name));
#else
        sprintf(strvaultsave, "%s%s.vault", PLAYER_DIR, capitalize(name));
#endif

        if ((vp = fopen(strvaultsave, "r")))
        {
                int iNest;

                for (iNest = 0; iNest < MAX_NEST; iNest++)
                        rgObjNest[iNest] = NULL;

                found = TRUE;

                while (1)
                {
                        char letter;
                        char *word;

                        letter = fread_letter(vp);

                        if (letter == '*')
                        {
                                fread_to_eol(vp);
                                continue;
                        }

                        if (letter != '#')
                        {
                                bug("Load_char_obj VAULT: # not found.", 0);
                                break;
                        }

                        word = fread_word(vp);

                        if (!str_cmp(word, "OBJECT"))
                                fread_obj(ch, vp, TRUE);
                        else if (!str_cmp(word, "END"))
                                break;
                        else
                        {
                                bug("Load_char_obj VAULT: bad section.", 0);
                                break;
                        }
                }

                fclose(vp);
        }

        fpReserve = fopen(NULL_FILE, "r");
        return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY(literal, field, value)                              \
                                if (!str_cmp(word, literal))    \
                                {                               \
                                    field  = value;             \
                                    fMatch = TRUE;              \
                                    break;                      \
                                }

void fread_char(CHAR_DATA *ch, FILE *fp)
{
        char *word;
        char  buf [ MAX_STRING_LENGTH ];
        bool  fMatch, read = TRUE;
        int   next;
        int   stat;

        while (read)
        {
                word   = feof(fp) ? "End" : fread_word(fp);
                fMatch = FALSE;

                switch (UPPER(word[0]))
                {
                    case '*':
                        fMatch = TRUE;
                        fread_to_eol(fp);
                        break;

                    case 'A':
                        KEY("Act", ch->act, fread_number64( fp, &stat ));
                        KEY("AirSupply", ch->pcdata->air_supply, fread_number( fp, &stat ));
                        KEY("AffdBy", ch->affected_by, fread_number64( fp, &stat ));
                        KEY("Aggro_Dam", ch->aggro_dam, fread_number( fp, &stat ));
                        KEY("Align", ch->alignment, fread_number( fp, &stat ));
                        KEY("Armr", ch->armor, fread_number( fp, &stat ));
                        KEY("Adamantite", ch->smelted_adamantite, fread_number( fp, &stat ));
                        KEY("AllowLook", ch->pcdata->allow_look, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Aff"))
                        {
                                AFFECT_DATA *paf;

                                if (!affect_free)
                                        paf = alloc_perm(sizeof(*paf));
                                else
                                {
                                        paf = affect_free;
                                        affect_free = affect_free->next;
                                }

                                paf->type       = fread_number( fp, &stat );
                                paf->duration   = fread_number( fp, &stat );
                                paf->modifier   = fread_number( fp, &stat );
                                paf->location   = fread_number( fp, &stat );
                                paf->bitvector  = fread_number( fp, &stat );
                                paf->deleted    = FALSE;
                                paf->next       = ch->affected;
                                ch->affected    = paf;

                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "AtrMd" ))
                        {
                                ch->pcdata->mod_str  = fread_number( fp, &stat );
                                ch->pcdata->mod_int  = fread_number( fp, &stat );
                                ch->pcdata->mod_wis  = fread_number( fp, &stat );
                                ch->pcdata->mod_dex  = fread_number( fp, &stat );
                                ch->pcdata->mod_con  = fread_number( fp, &stat );

                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "AtrPrm"))
                        {
                                ch->pcdata->perm_str = fread_number( fp, &stat );
                                ch->pcdata->perm_int = fread_number( fp, &stat );
                                ch->pcdata->perm_wis = fread_number( fp, &stat );
                                ch->pcdata->perm_dex = fread_number( fp, &stat );
                                ch->pcdata->perm_con = fread_number( fp, &stat );

                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'B':
                        KEY("Bank", ch->pcdata->bank, fread_number( fp, &stat ));
                        KEY("Bmfin", ch->pcdata->bamfin, fread_string(fp));
                        KEY("Bmfout", ch->pcdata->bamfout, fread_string(fp));
                        KEY("Blink", ch->pcdata->blink, fread_number( fp, &stat ));
                        KEY("Bounty", ch->pcdata->bounty, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Boards"))
                        {
                                int i, num = fread_number( fp, &stat );
                                char *boardname;

                                for (; num ; num--)
                                {
                                        boardname = fread_word(fp);
                                        i = board_lookup (boardname);

                                        if (i == BOARD_NOTFOUND)
                                        {
                                                sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.", ch->name, boardname);
                                                log_string (buf);
                                                fread_number (fp, &stat);
                                        }
                                        else
                                                ch->pcdata->last_note[i] = fread_number (fp, &stat);
                                }

                                fMatch = TRUE;
                        }
                        break;

                    case 'C':
                        KEY("ChSub", ch->pcdata->choose_subclass, fread_number( fp, &stat ));
                        KEY("Cla", ch->class, fread_number( fp, &stat ));
                        KEY("Clan", ch->clan, fread_number( fp, &stat ));
                        KEY("ClnLvl", ch->clan_level, fread_number( fp, &stat ));
                        KEY("Copper", ch->copper, fread_number( fp, &stat ));
                        KEY("Crit", ch->crit, fread_number( fp, &stat ));
                        KEY("CurRecall", ch->pcdata->current_recall, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Cond"))
                        {
                                ch->pcdata->condition[0] = fread_number( fp, &stat );
                                ch->pcdata->condition[1] = fread_number( fp, &stat );
                                ch->pcdata->condition[2] = fread_number( fp, &stat );
                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "Colors"))
                        {
                                ch->colors[0] = fread_number( fp, &stat );
                                ch->colors[1] = fread_number( fp, &stat );
                                ch->colors[2] = fread_number( fp, &stat );
                                ch->colors[3] = fread_number( fp, &stat );
                                ch->colors[4] = fread_number( fp, &stat );
                                ch->colors[5] = fread_number( fp, &stat );
                                ch->colors[6] = fread_number( fp, &stat );
                                ch->colors[7] = fread_number( fp, &stat );
                                ch->colors[8] = fread_number( fp, &stat );
                                ch->colors[9] = fread_number( fp, &stat );
                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'D':
                        KEY("Dam", ch->damroll, fread_number( fp, &stat ));
                        KEY("DamageMit", ch->damage_mitigation, fread_number( fp, &stat ));
                        KEY("DamangeEnh", ch->damage_enhancement, fread_number( fp, &stat));
                        KEY("Deaf", ch->deaf, fread_number( fp, &stat ));
                        KEY("DeityFlags", ch->pcdata->deity_flags, fread_number( fp, &stat ));
                        KEY("DeityTimer", ch->pcdata->deity_timer, fread_number( fp, &stat ));
                        KEY("Dscr", ch->description, fread_string(fp));

                        if (!str_cmp(word, "DeityFavour"))
                        {
                                int tmp;
                                sprintf(buf, "%s", fread_string(fp));
                                tmp = fread_number( fp, &stat );

                                for (next = 0; next < NUMBER_DEITIES; next++)
                                {
                                        if (!str_cmp(deity_info_table[next].name, buf))
                                        {
                                                ch->pcdata->deity_favour[next] = tmp;
                                                break;
                                        }
                                }
                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "DeityType"))
                        {
                                for (next = 0; next < DEITY_NUMBER_TYPES; next++)
                                        ch->pcdata->deity_type_timer[next] = fread_number( fp, &stat );

                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "DeityPerson"))
                        {
                                for (next = 0; next < DEITY_NUMBER_PERSONALITIES; next++)
                                        ch->pcdata->deity_personality_timer[next] = fread_number( fp, &stat );

                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'E':
                        if (!str_cmp(word, "End"))
                        {
                                fMatch = TRUE;
                                read = FALSE;
                                break;
                        }

                        KEY("Exp", ch->exp, fread_number( fp, &stat ));
                        KEY("Electrum", ch->smelted_electrum, fread_number( fp, &stat ));
                        break;

                    case 'F':
                        KEY("Fame", ch->pcdata->fame, fread_number( fp, &stat ));
                        KEY("Form", ch->form, fread_number( fp, &stat ));
                        break;

                    case 'G':
                        KEY("Gag", ch->gag, fread_number( fp, &stat ));
                        KEY("Gold", ch->gold, fread_number( fp, &stat ));
                        break;

                    case 'H':
                        KEY("Hit", ch->hitroll, fread_number( fp, &stat ));

                        if (!str_cmp(word, "HpMnMv"))
                        {
                                ch->hit         = fread_number( fp, &stat );
                                ch->max_hit     = fread_number( fp, &stat );
                                ch->mana        = fread_number( fp, &stat );
                                ch->max_mana    = fread_number( fp, &stat );
                                ch->move        = fread_number( fp, &stat );
                                ch->max_move    = fread_number( fp, &stat );
                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'K':
                        KEY("Kills", ch->pcdata->kills, fread_number( fp, &stat ));
                        KEY("Killed", ch->pcdata->killed, fread_number( fp, &stat ));
                        break;

                    case 'L':
                        KEY("Lvl", ch->level, fread_number( fp, &stat ));
                        KEY("LngDsc", ch->long_descr, fread_string(fp));
                        break;

                    case 'N':
                        if (!str_cmp(word, "Nm"))
                        {
                                /* Name already set externally */
                                fread_to_eol(fp);
                                fMatch = TRUE;
                                break;
                        }
                        KEY("Note", ch->last_note, fread_number( fp, &stat ));
                        break;

                    case 'P':
                        KEY("Pdeaths", ch->pcdata->pdeaths, fread_number( fp, &stat ));
                        KEY("Pkills", ch->pcdata->pkills, fread_number( fp, &stat ));
                        KEY("Pkscore", ch->pcdata->pkscore, fread_number( fp, &stat ));
                        KEY("Pglen", ch->pcdata->pagelen, fread_number( fp, &stat ));
                        KEY("Paswd", ch->pcdata->pwd, fread_string(fp));
                        KEY("Platinum", ch->plat, fread_number( fp, &stat ));
                        KEY("Playd", ch->played, fread_number( fp, &stat ));
                        KEY("Pos", ch->position, fread_number( fp, &stat ));
                        KEY("Prac", ch->pcdata->str_prac, fread_number( fp, &stat ));
                        KEY("Prmpt", ch->prompt, fread_string(fp));

                        if (!str_cmp(word, "Patron"))
                        {
                                sprintf(buf, "%s", fread_string(fp));

                                for (next=0; next<NUMBER_DEITIES; next++)
                                {
                                        if (!str_cmp (deity_info_table[next].name, buf))
                                        {
                                                ch->pcdata->deity_patron = next;
                                                break;
                                        }
                                }
                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'Q':
                        KEY("QuestPnts", ch->pcdata->questpoints, fread_number( fp, &stat ));
                        KEY("QPTotal", ch->pcdata->totalqp, fread_number( fp, &stat ));
                        KEY("QuestNext", ch->pcdata->nextquest, fread_number( fp, &stat ));
                        KEY("Quiet", ch->silent_mode, fread_number( fp, &stat ));
                        break;

                    case 'R':
                        KEY("Rce", ch->race, fread_number( fp, &stat ));
                        KEY("Rage", ch->rage, fread_number( fp, &stat ));
                        KEY("ResistAcid", ch->resist_acid, fread_number( fp, &stat ));
                        KEY("ResistLightning", ch->resist_lightning, fread_number( fp, &stat ));
                        KEY("ResistHeat", ch->resist_heat, fread_number( fp, &stat ));
                        KEY("ResistCold", ch->resist_cold, fread_number( fp, &stat ));
                        KEY("Rage_Max", ch->max_rage, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Room"))
                        {
                                ch->in_room = get_room_index(fread_number( fp, &stat ));

                                if (!ch->in_room)
                                        ch->in_room = get_room_index(ROOM_VNUM_LIMBO);

                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "RecPoints"))
                        {
                                for (next = 0; next < MAX_RECALL_POINTS; next++)
                                        ch->pcdata->recall_points[next] = fread_number( fp, &stat );

                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'S':
                        KEY("Swiftness", ch->swiftness, fread_number( fp, &stat ));
                        KEY("SavThr", ch->saving_throw, fread_number( fp, &stat ));
                        KEY("Status", ch->status, fread_number( fp, &stat ));
                        KEY("Sx", ch->sex, fread_number( fp, &stat ));
                        KEY("ShtDsc", ch->short_descr, fread_string(fp));
                        KEY("Silver", ch->silver, fread_number( fp, &stat ));
                        KEY("SbCla", ch->sub_class, fread_number( fp, &stat ));
                        KEY("SplPrac", ch->pcdata->int_prac, fread_number( fp, &stat ));
                        KEY("StatTrain", ch->pcdata->stat_train, fread_number( fp, &stat ));
                        KEY("SpellAttk", ch->pcdata->spell_attacks, fread_number( fp, &stat ));
                        KEY("Steel", ch->smelted_steel, fread_number( fp, &stat ));
                        KEY("Starmetal", ch->smelted_starmetal, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Saved"))
                        {
                                int num_days, fame_loss_calc;
                                char log_buf [MAX_STRING_LENGTH];

                                ch->save_time = fread_number( fp, &stat );
                                num_days = (current_time - ch->save_time) / (24 * 3600);

                                if (num_days > 10 && ch->pcdata->fame > 500)
                                {
                                        fame_loss_calc = (ch->pcdata->fame / 20) * (num_days / 10);
                                        ch->pcdata->fame = ch->pcdata->fame - fame_loss_calc;

                                        if (ch->pcdata->fame < 500)
                                                ch->pcdata->fame = 500;

                                        sprintf(log_buf, "%s lost %d fame after %d days not logged on.",
                                                ch->name,
                                                fame_loss_calc ,
                                                num_days);
                                        log_string(log_buf);
                                }

                                fMatch = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "Skll"))
                        {
                                int sn;
                                int value;

                                value = fread_number( fp, &stat );
                                strncpy(buf, fread_word(fp), sizeof(buf));
                                sn = skill_lookup(buf);

                                if (sn < 0)
                                {
                                        char buf2[256];
                                        sprintf(buf2, "[*****] BUG: Fread_char: unknown skill: %s: %s (%d)",
                                                ch->name, buf, value);
                                        log_string (buf2);
                                }
                                else
                                        ch->pcdata->learned[sn] = value;

                                fMatch = TRUE;
                        }
                        break;

                    case 'T':
                        KEY("Trst", ch->trust, fread_number( fp, &stat ));
                        KEY("Titanium", ch->smelted_titanium, fread_number( fp, &stat ));

                        if (!str_cmp(word, "Ttle"))
                        {
                                ch->pcdata->title = fread_string(fp);

                                if (isalpha(ch->pcdata->title[0])
                                    || isdigit(ch->pcdata->title[0]))
                                {
                                        sprintf(buf, " %s", ch->pcdata->title);
                                        free_string(ch->pcdata->title);
                                        ch->pcdata->title = str_dup(buf);
                                }

                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'W':
                        KEY("Wimp", ch->wimpy, fread_number( fp, &stat ));
                        KEY("Wizbt", ch->wizbit, fread_number( fp, &stat ));
                        break;

                    case 'X':
                        KEY("XPLevel", ch->pcdata->level_xp_loss, fread_number( fp, &stat ));
                        break;
                }

                if (!fMatch)
                {
                        sprintf(buf, "fread_char: unknown key '%s', ignoring", word);
                        log_string(buf);
                        fread_to_eol(fp);
                }
        }

        /*
         *  Init some variables that aren't saved
         */
        ch->backstab = 0;
        ch->edrain = 0;
        ch->tournament_team = -1;
        ch->pcdata->suicide_code = 0;

        if (!ch->pcdata->pagelen)
                ch->pcdata->pagelen = 100;

        if (!ch->prompt)
                ch->prompt = str_dup ("<%hhp %mm %vmv> ");

        if (ch->pcdata->pagelen > 1000)
                ch->pcdata->pagelen = 1000;

        calc_coin_weight(ch);
}


void fread_obj (CHAR_DATA *ch, FILE *fp, bool vault)
{
        static OBJ_DATA obj_zero;
        OBJ_DATA        *obj;
        char            *word;
        int             iNest;
        int             stat;
        bool            fMatch;
        bool            fNest;
        bool            fVnum;

        if (!obj_free)
                obj             = alloc_perm(sizeof(*obj));
        else
        {
                obj             = obj_free;
                obj_free        = obj_free->next;
        }

        *obj                    = obj_zero;
        obj->name               = str_dup("");
        obj->short_descr        = str_dup("");
        obj->description        = str_dup("");
        obj->deleted            = FALSE;

        fNest                   = FALSE;
        fVnum                   = TRUE;
        iNest                   = 0;

        while (1)
        {
                word = feof(fp) ? "End" : fread_word(fp);
                fMatch = FALSE;

                switch (UPPER(word[0]))
                {
                    case '*':
                        fMatch = TRUE;
                        fread_to_eol(fp);
                        break;

                    case 'A':
                        if (!str_cmp(word, "Affect"))
                        {
                                AFFECT_DATA *paf;

                                if (!affect_free)
                                        paf             = alloc_perm(sizeof(*paf));
                                else
                                {
                                        paf             = affect_free;
                                        affect_free     = affect_free->next;
                                }

                                paf->type       = fread_number( fp, &stat );
                                paf->duration   = fread_number( fp, &stat );
                                paf->modifier   = fread_number( fp, &stat );
                                paf->location   = fread_number( fp, &stat );
                                paf->bitvector  = fread_number( fp, &stat );
                                paf->next       = obj->affected;
                                obj->affected   = paf;
                                fMatch          = TRUE;
                                break;
                        }

                        break;

                    case 'C':
                        KEY("Cost", obj->cost, fread_number( fp, &stat ));
                        break;

                    case 'D':
                        KEY("Description", obj->description, fread_string(fp));
                        break;

                    case 'E':
                        KEY("EgoFlags", obj->ego_flags, fread_number( fp, &stat ));
                        KEY("ExtraFlags", obj->extra_flags, fread_number64( fp, &stat ));

                        if (!str_cmp(word, "ExtraDescr"))
                        {
                                EXTRA_DESCR_DATA *ed;

                                if (!extra_descr_free)
                                        ed                      = alloc_perm(sizeof(*ed));
                                else
                                {
                                        ed                      = extra_descr_free;
                                        extra_descr_free        = extra_descr_free->next;
                                }

                                ed->keyword             = fread_string(fp);
                                ed->description         = fread_string(fp);
                                ed->next                = obj->extra_descr;
                                obj->extra_descr        = ed;
                                fMatch = TRUE;
                        }

                        if (!str_cmp(word, "End"))
                        {
                                if (!fNest || !fVnum)
                                {
                                        bug("Fread_obj: incomplete object.", 0);
                                        free_string(obj->name       );
                                        free_string(obj->description);
                                        free_string(obj->short_descr);
                                        obj->next = obj_free;
                                        obj_free  = obj;
                                        return;
                                }
                                else
                                {
                                        if (!obj->pIndexData)
                                                return;
                                        obj->next       = object_list;
                                        object_list     = obj;
                                        obj->pIndexData->count++;
                                        if (iNest == 0 || !rgObjNest[iNest])
                                        {
                                                if (vault) {
                                                    obj_to_charvault(obj, ch);
                                                }
                                                else {
                                                    obj_to_char(obj, ch);
                                                }
                                        }
                                        else {
                                                if (vault) {
                                                    obj_to_objvault(obj, rgObjNest[iNest-1]);
                                                }
                                                else {
                                                    obj_to_obj(obj, rgObjNest[iNest-1]);
                                                }
                                        }
                                        return;
                                }
                        }
                        break;

                    case 'H':
                        KEY("HowCreated", obj->how_created, fread_number( fp, &stat ));
                        break;

                    case 'I':
                        KEY("ItemType", obj->item_type, fread_number( fp, &stat ));
                        KEY("Identified", obj->identified, fread_number( fp, &stat));
                        break;

                    case 'L':
                        KEY("Level", obj->level, fread_number( fp, &stat ));
                        break;

                    case 'N':
                        KEY("Name", obj->name, fread_string(fp));

                        if (!str_cmp(word, "Nest"))
                        {
                                iNest = fread_number( fp, &stat );

                                if (iNest < 0 || iNest >= MAX_NEST)
                                        bug("Fread_obj: bad nest %d.", iNest);
                                else
                                {
                                        rgObjNest[iNest] = obj;
                                        fNest = TRUE;
                                }
                                fMatch = TRUE;
                        }
                        break;

                    case 'O':
                        if(!str_cmp(word, "Owner"))
                        {
                                set_obj_owner(obj, fread_string(fp));
                                fMatch = TRUE;
                        }

                        break;

                    case 'S':
                        KEY("ShortDescr", obj->short_descr, fread_string(fp));

                        if (!str_cmp(word, "Spell"))
                        {
                                int iValue;
                                int sn;

                                iValue = fread_number( fp, &stat );
                                sn     = skill_lookup(fread_word(fp));
                                if (iValue < 0 || iValue > 3)
                                {
                                        bug("Fread_obj: bad iValue %d.", iValue);
                                }
                                else if (sn < 0)
                                {
                                        bug("Fread_obj: unknown skill.", 0);
                                }
                                else
                                {
                                        obj->value[iValue] = sn;
                                }
                                fMatch = TRUE;
                                break;
                        }

                        break;

                    case 'T':
                        KEY("Timer",    obj->timer,             fread_number( fp, &stat ));
                        KEY("TimerMax", obj->timermax,          fread_number( fp, &stat ));
                        break;

                    case 'V':
                        if (!str_cmp(word, "Values"))
                        {
                                obj->value[0]   = fread_number( fp, &stat );
                                obj->value[1]   = fread_number( fp, &stat );
                                obj->value[2]   = fread_number( fp, &stat );
                                obj->value[3]   = fread_number( fp, &stat );
                                fMatch          = TRUE;
                                break;
                        }

                        if (!str_cmp(word, "Vnum"))
                        {
                                int vnum;
                                char buf2[100];

                                vnum = fread_number( fp, &stat );
                                if (!(obj->pIndexData = get_obj_index(vnum))) {
                                        obj->deleted = TRUE;
                                        sprintf(buf2, "[*****] BUG: Fread_obj: bad vnum %d (%s)",
                                                vnum, ch->name);
                                        log_string(buf2);
                                }
                                else
                                        fVnum = TRUE;
                                fMatch = TRUE;
                                break;
                        }
                        break;

                    case 'W':
                        KEY("WearFlags", obj->wear_flags,        fread_number( fp, &stat ));
                        KEY("WearLoc",   obj->wear_loc,          fread_number( fp, &stat ));
                        KEY("Weight",    obj->weight,            fread_number( fp, &stat ));
                        break;

                }

                if (!fMatch)
                {
                        char buf [MAX_STRING_LENGTH];

                        sprintf(buf, "Fread_obj: no match: %s", word);
                        bug(buf, 0);
                        break;
                }
        }
}


void save_fame_table ()
{
        FILE *fp;
        char fame_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(fame_file, "%s%s", PLAYER_DIR, "TABLES/fame.table");

        fclose(fpReserve);

        if (!(fp = fopen(fame_file, "w+")))
        {
                sprintf(buf, "Save_fame_table: fopen %s: ", fame_file);
                bug(buf, 0);
                perror(fame_file);
                log_string(buf);
        }
        else
        {
                int iter;

                for (iter = 0; iter < FAME_TABLE_LENGTH; iter++) {

                        fprintf(fp, "%s\n~\n%d\n",
                                fame_table[iter].name,
                                fame_table[iter].fame
                                );
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


void load_fame_table ()
{
        FILE *fp;
        int  stat;
        char fame_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(fame_file, "%s%s", PLAYER_DIR, "TABLES/fame.table");

        fclose(fpReserve);

        if (!(fp = fopen(fame_file, "r")))
        {
                sprintf(buf, "missing fame table file: %s", fame_file);
                log_string(buf);
        }
        else
        {
                int iter;

                for (iter = 0; iter < FAME_TABLE_LENGTH; iter++)
                {
                        char *word;

                        word = fread_word(fp);

                        if (!str_cmp(word, "#END"))
                                break;

                        strncpy(fame_table[iter].name, word, strlen(word));
                        word = fread_string(fp);
                        strncpy(fame_table[iter].title, word, strlen(word));
                        fame_table[iter].fame = fread_number( fp, &stat );
                }

                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}

void save_infamy_table ()
{
        FILE *fp;
        char infamy_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(infamy_file, "%s%s", PLAYER_DIR, "TABLES/infamy.table");

        fclose(fpReserve);

        if (!(fp = fopen(infamy_file, "w+")))
        {
                sprintf(buf, "Save_infamy_table: fopen %s: ", infamy_file);
                bug(buf, 0);
                perror(infamy_file);
                log_string(buf);
        }
        else
        {
                int iter;

                for (iter = 0; iter < INFAMY_TABLE_LENGTH; iter++) {

                        fprintf(fp, "%s~\n%s~\n%d\n%d\n",
                                infamy_table[iter].name,
                                infamy_table[iter].loc,
                                infamy_table[iter].kills,
                                infamy_table[iter].vnum
                                );
                }

                fprintf(fp, "#END~\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


void load_infamy_table ()
{
        FILE *fp;
        int  stat;
        char infamy_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];


        sprintf(buf, "Loading Infamy Table");
        log_string(buf);

        sprintf(infamy_file, "%s%s", PLAYER_DIR, "TABLES/infamy.table");

        fclose(fpReserve);

        if (!(fp = fopen(infamy_file, "r")))
        {
                sprintf(buf, "missing infamy table file: %s", infamy_file);
                log_string(buf);
        }
        else
        {
                int iter;

                for (iter = 0; iter < INFAMY_TABLE_LENGTH; iter++)
                {

                        char *word;

                        word = fread_string(fp); /* The executioner */

                        if (!str_cmp(word, "#END"))
                                break;

                        strncpy(infamy_table[iter].name, word, strlen(word));

                        word = fread_string(fp); /* location */
                        strncpy(infamy_table[iter].loc, word, strlen(word));

                        infamy_table[iter].kills = fread_number( fp, &stat );
                        infamy_table[iter].vnum = fread_number( fp, &stat );
                }

                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


void save_all_clan_tables()
{
        int iter;

        /* not from 0 - as 0 is the no clan and that does nothing ever.  no data */
        for (iter = 1; iter < MAX_CLAN; iter++)
        {
                save_vanq_table(iter);
                save_pkill_table(iter);
        }
}


void load_all_clan_tables()
{
        int iter;

        for (iter = 1; iter < MAX_CLAN; iter++)
        {
                load_vanq_table(iter);
                load_pkill_table(iter);
        }
}


void save_vanq_table(int clan)
{
        FILE *fp;
        char vanq_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(vanq_file, "%s%s%s%s", PLAYER_DIR, "TABLES/vanq.",
                clan_table[clan].who_name,".table");

        fclose(fpReserve);

        if (!(fp = fopen(vanq_file, "w+")))
        {
                sprintf(buf, "Save_vanq_table: fopen %s: ", vanq_file);
                bug(buf, 0);
                perror(vanq_file);
                log_string(buf);
        }
        else
        {
                int iter;

                for (iter = 0; iter < CLAN_VANQ_TABLE_LENGTH; iter++)
                {
                        fprintf(fp, "%-17s%d\n",
                                clan_vanq_table[clan][iter].name,
                                clan_vanq_table[clan][iter].killed);
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


void load_vanq_table (int clan)
{
        FILE *fp;
        char vanq_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(vanq_file, "%s%s%s%s", PLAYER_DIR, "TABLES/vanq.",
                clan_table[clan].who_name,".table");

        fclose(fpReserve);

        if ((fp = fopen(vanq_file, "r")))
        {
                int iter;
                int pos = 0;
                int killed;
                int stat;

                for (iter = 0; iter < CLAN_VANQ_TABLE_LENGTH; iter++)
                {
                        char *word;
                        word = fread_word(fp);

                        if (!str_cmp(word, "#END"))
                        {
                                fclose(fp);
                                fpReserve = fopen(NULL_FILE, "r");
                                return;
                        }

                        killed = fread_number( fp, &stat );

                        /*
                         * Temporary hack to deal with brain dead table implementation
                         */
                        if (killed > 0)
                        {
                                strncpy(clan_vanq_table[clan][pos].name, word, strlen(word));
                                clan_vanq_table[clan][pos].killed = killed;
                                pos++;
                        }
                }

                fclose(fp);
        }
        else
        {
                sprintf(buf, "missing fame table file: %s", vanq_file);
                log_string(buf);
        }

        fpReserve = fopen(NULL_FILE, "r");
}



void save_pkill_table(int clan)
{
        FILE *fp;
        char pkill_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];
        int iter;

        sprintf(pkill_file, "%s%s%s%s", PLAYER_DIR, "TABLES/pkill.",
                clan_table[clan].who_name,".table");

        fclose(fpReserve);

        if (!(fp = fopen(pkill_file, "w+")))
        {
                sprintf(buf, "Save_pkill_table: fopen %s: ", pkill_file);
                bug(buf, 0);
                perror(pkill_file);
                log_string(buf);
        }
        else
        {
                for (iter = 0; iter < CLAN_PKILL_TABLE_LENGTH; iter++)
                {
                        fprintf(fp, "%-17s%d\n",
                                clan_pkill_table[clan][iter].name,
                                clan_pkill_table[clan][iter].pkills);
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}



void load_pkill_table (int clan)
{
        FILE *fp;
        char pkill_file [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];

        sprintf(pkill_file, "%s%s%s%s", PLAYER_DIR, "TABLES/pkill.",
                clan_table[clan].who_name,".table");

        fclose(fpReserve);

        if ((fp = fopen(pkill_file, "r")))
        {
                int iter;
                int pos = 0;
                int pkills;
                int stat;

                for (iter = 0; iter < FAME_TABLE_LENGTH; iter++)
                {
                        char *word;
                        word = fread_word(fp);

                        if (!str_cmp(word, "#END"))
                        {
                                fclose(fp);
                                fpReserve = fopen(NULL_FILE, "r");
                                return;
                        }

                        pkills = fread_number( fp, &stat );

                        /*
                         *  Temporary hack to deal with brain dead table implementation
                         */
                        if (pkills > 0)
                        {
                                strncpy(clan_pkill_table[clan][pos].name, word, strlen(word));
                                clan_pkill_table[clan][pos].pkills = pkills;
                                pos++;
                        }
                }

                fclose(fp);
        }
        else
        {
                sprintf(buf, "missing fame table file: %s", pkill_file);
                log_string(buf);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


/*
 * New code for loading DD III heroes from file.
 */
bool fread_hero(HERO_DATA *hero, FILE *fp)
{
        char *word;
        bool  fMatch;
        int   stat;
        char  buf [MAX_STRING_LENGTH];

        while (1)
        {
                word   = feof(fp) ? "End" : fread_word(fp);
                fMatch = FALSE;

                switch (UPPER(word[0]))
                {
                    case '*':
                        fMatch = TRUE;
                        fread_to_eol(fp);
                        break;

                    case 'C':
                        KEY("Class", hero->class,       fread_number( fp, &stat ));
                        break;

                    case 'D':
                        KEY("Date", hero->date,       fread_string(fp));
                        break;

                    case 'E':
                        if (!str_cmp(word, "End"))
                                return TRUE;
                        break;

                    case 'N':
                        KEY("Name", hero->name,       fread_string(fp));
                        break;

                    case 'S':
                        KEY("Subclassed", hero->subclassed,       fread_number( fp, &stat ));
                        break;

                }

                if (!fMatch)
                {
                        sprintf(buf, "Load_hero_file: no match: %s", word);
                        bug(buf, 0);
                }
        }

        return FALSE;
}


void load_hero_table ()
{
        HERO_DATA *hero;
        FILE      *fp;
        char       hero_file [ MAX_STRING_LENGTH ];
        char buf [MAX_STRING_LENGTH];

        /* Set hero_first pointer to NULL */
        hero_first = NULL;
        hero = NULL;

        sprintf(hero_file, "%s%s", PLAYER_DIR, "TABLES/hero.table");

        if (!(fp = fopen(hero_file, "r")))
        {
                sprintf(buf, "missing hero table file: %s", hero_file);
                log_string(buf);
                return;
        }


        for (; ;)
        {
                char *word;
                int   letter;

                letter = fread_letter(fp);

                if (letter == '*')
                {
                        fread_to_eol(fp);
                        continue;
                }

                if (letter != '#')
                {
                        bug("Load_hero_file: # not found.", 0);
                        free_mem(hero, sizeof(HERO_DATA));
                        break;
                }

                word = fread_word(fp);

                if (!str_cmp(word, "HERO"))
                {
                        hero = alloc_perm(sizeof(HERO_DATA));

                        fread_hero(hero, fp);

                        if (!hero_first)
                                hero_first      = hero;
                        if ( hero_last )
                                hero_last->next = hero;

                        hero_last               = hero;
                        hero->next              = NULL;

                }
                else if (!str_cmp(word, "END" ))
                        break;
                else
                {
                        sprintf(buf, "Load_hero_file: bad section: %s.", word);
                        bug(buf, 0);
                        free_mem(hero, sizeof(HERO_DATA));
                        break;
                }
        }

        fclose(fp);
}


void save_hero_table ()
{
        FILE *fp;
        HERO_DATA *hero;
        char strsave [ MAX_INPUT_LENGTH ];

        sprintf(strsave, "%s%s", PLAYER_DIR, "TABLES/hero.table");

        fclose(fpReserve);

        if (!(fp = fopen(strsave, "w")))
        {
                bug("Cannot open HEROS.TABLE for writing", 0);
                perror(strsave);
        }
        else
        {
                for (hero = hero_first; hero; hero = hero->next)
                {
                        fprintf(fp, "#HERO\n");
                        fprintf(fp, "Name          %s~\n", hero->name);
                        fprintf(fp, "Date          %s~\n", hero->date);
                        fprintf(fp, "Class         %d\n", hero->class);
                        fprintf(fp, "Subclassed    %d\n", hero->subclassed);
                        fprintf(fp, "End\n\n");
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


/*
 * Legends table stuff
 */
void load_legend_table ()
{
        FILE *fp;
        int   stat;
        char legend_file [MAX_STRING_LENGTH];
        char buf [MAX_STRING_LENGTH];
        int i = 0;

        sprintf(legend_file, "%s%s", PLAYER_DIR, "TABLES/legend.table");

        if (!(fp = fopen(legend_file, "r")))
        {
                perror(legend_file);
                return;
        }

        while (1)
        {
                char *word;
                int   letter;

                letter = fread_letter(fp);
                if (letter == '*')
                {
                        fread_to_eol(fp);
                        continue;
                }

                if (letter != '#')
                {
                        bug("Load_legend_file: # not found.", 0);
                        break;
                }

                word = fread_word(fp);

                if (!str_cmp(word, "LEGEND"))
                {
                        char *word;
                        bool  fMatch, isok;
                        char buf [MAX_STRING_LENGTH];
                        char *tmpname;

                        isok = FALSE;

                        for (; isok != TRUE;)
                        {
                                word   = feof(fp) ? "End" : fread_word(fp);
                                fMatch = FALSE;

                                switch (UPPER(word[0]))
                                {
                                    case 'C':
                                        KEY("Class", legend_table[i].class,       fread_number( fp, &stat ));
                                        break;

                                    case 'E':
                                        if (!str_cmp(word, "End")) {
                                                fMatch = TRUE;
                                                isok = TRUE;
                                        }
                                        break;

                                    case 'F':
                                        KEY("Fame", legend_table[i].fame,       fread_number( fp, &stat ));
                                        break;

                                    case 'N':
                                        if (!str_cmp(word, "Name")) {
                                                tmpname = fread_string(fp);
                                                strncpy(legend_table[i].name, tmpname, 14);
                                                fMatch = TRUE;
                                        }
                                        break;

                                    case 'S':
                                        KEY("Subclassed", legend_table[i].subclassed,       fread_number( fp, &stat ));
                                        break;

                                }

                                if (!fMatch)
                                {
                                        sprintf(buf, "Load_legend_file: no match: %s", word);
                                        bug(buf, 0);
                                }
                        }
                        i++;
                }
                else if (!str_cmp(word, "END" ))
                        break;
                else
                {
                        sprintf(buf, "Load_legend_file: bad section: %s.", word);
                        bug(buf, 0);
                        break;
                }
        }

        fclose(fp);
}



void save_legend_table ()
{
        FILE         *fp;
        char         strsave [ MAX_INPUT_LENGTH ];
        int i;

        sprintf(strsave, "%s%s", PLAYER_DIR, "TABLES/legend.table");

        fclose(fpReserve);

        if (!(fp = fopen(strsave, "w")))
        {
                bug("Cannot open LEGEND.TABLE for writing", 0);
                perror(strsave);
        }
        else
        {
                for (i = 0; i < LEGEND_TABLE_LENGTH; i++)
                {
                        fprintf(fp, "#LEGEND\n");
                        fprintf(fp, "Name          %s~\n",    legend_table[i].name);
                        fprintf(fp, "Fame          %d\n",     legend_table[i].fame);
                        fprintf(fp, "Class         %d\n",     legend_table[i].class);
                        fprintf(fp, "Subclassed    %d\n",     legend_table[i].subclassed);
                        fprintf(fp, "End\n\n");
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


/*
 * Pkscore table stuff
 */
void load_pkscore_table ()
{
        FILE    *fp;
        int     stat;
        char    pkscore_file [ MAX_STRING_LENGTH ];
        char    buf [MAX_STRING_LENGTH];
        int     i = 0;

        sprintf(pkscore_file, "%s%s", PLAYER_DIR, "TABLES/pkscore.table");

        if (!(fp = fopen(pkscore_file, "r")))
        {
                perror(pkscore_file);
                return;
        }

        while (i < PKSCORE_TABLE_LENGTH)
        {
                char *word;
                int   letter;

                letter = fread_letter(fp);
                if (letter == '*')
                {
                        fread_to_eol(fp);
                        continue;
                }

                if (letter != '#')
                {
                        bug("Load_pkscore_file: # not found.", 0);
                        break;
                }

                word = fread_word(fp);

                if (!str_cmp(word, "PKSCORE"))
                {
                        char *word;
                        bool  fMatch, isok;
                        char buf [MAX_STRING_LENGTH];
                        char *tmpname;

                        isok = FALSE;

                        /*  Temporary hack  */
                        strcpy(pkscore_table[i].clan, "{x   ");

                        for (; isok != TRUE;)
                        {
                                word   = feof(fp) ? "End" : fread_word(fp);
                                fMatch = FALSE;

                                switch (UPPER(word[0]))
                                {
                                    case 'C':
                                        KEY("Class", pkscore_table[i].class,       fread_number( fp, &stat ));
                                        if (!str_cmp(word, "Clan"))
                                        {
                                                tmpname = fread_string(fp);
                                                strncpy(pkscore_table[i].clan, tmpname, 5);
                                                fMatch = TRUE;
                                        }
                                        break;

                                    case 'E':
                                        if (!str_cmp(word, "End"))
                                        {
                                                fMatch = TRUE;
                                                isok = TRUE;
                                        }
                                        break;

                                    case 'N':
                                        if (!str_cmp(word, "Name"))
                                        {
                                                tmpname = fread_string(fp);
                                                strncpy(pkscore_table[i].name, tmpname, 14);
                                                fMatch = TRUE;
                                        }
                                        break;

                                    case 'P':
                                        KEY("Pkills",  pkscore_table[i].pkills,       fread_number( fp, &stat ));
                                        KEY("Pkscore", pkscore_table[i].pkscore,      fread_number( fp, &stat ));
                                        KEY("Pdeaths", pkscore_table[i].pdeaths,      fread_number( fp, &stat ));
                                        break;

                                    case 'S':
                                        KEY("Subclassed", pkscore_table[i].subclassed,       fread_number( fp, &stat ));
                                        break;

                                }

                                if (!fMatch)
                                {
                                        sprintf(buf, "Load_pkscore_file: no match: %s", word);
                                        bug(buf, 0);
                                }
                        }
                        i++;
                }
                else if (!str_cmp(word, "END" ))
                        break;
                else
                {
                        sprintf(buf, "Load_pkscore_file: bad section: %s.", word);
                        bug(buf, 0);
                        break;
                }
        }

        while (i < PKSCORE_TABLE_LENGTH)
        {
                pkscore_table[i].name[0] = '\0';
                pkscore_table[i].pkills = 0;
                pkscore_table[i].pkscore = 0;
                pkscore_table[i].pdeaths = 0;
                pkscore_table[i].class = 0;
                pkscore_table[i].subclassed = FALSE;
                pkscore_table[i].clan[0] = '\0';

                ++i;
        }

        fclose(fp);
}


void save_pkscore_table ()
{
        FILE         *fp;
        char         strsave [ MAX_INPUT_LENGTH ];
        int i;

        sprintf(strsave, "%s%s", PLAYER_DIR, "TABLES/pkscore.table");

        fclose(fpReserve);

        if (!(fp = fopen(strsave, "w")))
        {
                bug("Cannot open PKSCORE.TABLE for writing", 0);
                perror(strsave);
        }
        else
        {
                for (i = 0; i < PKSCORE_TABLE_LENGTH; i++)
                {
                        fprintf(fp, "#PKSCORE\n"                                          );
                        fprintf(fp, "Name          %s~\n",    pkscore_table[i].name       );
                        fprintf(fp, "Pkills        %d\n",     pkscore_table[i].pkills     );
                        fprintf(fp, "Pdeaths       %d\n",        pkscore_table[i].pdeaths    );
                        fprintf(fp, "Pkscore       %d\n",     pkscore_table[i].pkscore    );
                        fprintf(fp, "Class         %d\n",     pkscore_table[i].class      );
                        fprintf(fp, "Subclassed    %d\n",     pkscore_table[i].subclassed );
                        fprintf(fp, "Clan          %s~\n",    pkscore_table[i].clan);
                        fprintf(fp, "End\n\n"                                             );
                }

                fprintf(fp, "#END\n");
                fclose(fp);
        }

        fpReserve = fopen(NULL_FILE, "r");
}


