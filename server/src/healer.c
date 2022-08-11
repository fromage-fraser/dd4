/*
 * Healer code written for Merc 2.0 muds by Alander
 * direct questions or comments to rtaylor@cie-2.uoregon.edu
 * any use of this code must include this header
 */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"


struct healer_spell
{
        char *keyword;
        char *spell_name;
        int price;
};


void do_heal(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *mob;
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int i, sn;

        const int NUMBER_SPELLS = 13;
        const struct healer_spell spell_list[13] =
        {
                /*  keyword, spell name, price in gold pieces */
                { "light",     "cure light wounds",     2     },
                { "serious",   "cure serious wounds",   5     },
                { "critical",  "cure critical wounds",  10    },
                { "heal",      "heal",                  15    },
                { "power",     "power heal",            35    },
                { "blindness", "cure blindness",        5     },
                { "poison",    "cure poison",           5     },
                { "stabilise", "stabilise",            5     },
                { "curse",     "remove curse",          5     },
                { "refresh",   "refresh",               1     },
                { "bless",     "bless",                 10    },
                { "mana",      "restore mana",          40    },
                { "vitalize",  "vitalize",              120   }
        };

        for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        {
                if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
                        break;
        }

        if ( ch->fighting )
        {
                send_to_char( "Not while fighting!\n\r", ch);
                return;
        }

        if (!mob)
        {
                send_to_char( "You can't do that here.\n\r", ch );
                return;
        }

        one_argument(argument,arg);

        if (arg[0] == '\0')
        {
                send_to_char("\n\r{WThe following spells are available from the healer:{x\n\r\n\r", ch);
                for (i = 0; i < NUMBER_SPELLS; i++)
                {
                        sprintf(buf, " {W%-13s{x {w%-22s {Y%5d gold{x\n\r",
                                spell_list[i].keyword,
                                spell_list[i].spell_name,
                                spell_list[i].price);
                        send_to_char(buf, ch);
                }
                send_to_char("\n\rType '{WHEAL <spell>{x' to receive healing.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch) && ch->pcdata->fame < 0)
        {
                act("$N says 'I won't heal the likes of you!'", ch, NULL, mob, TO_CHAR);
                return;
        }

        for (i = 0; i < NUMBER_SPELLS; i++)
        {
                if (is_name(arg, spell_list[i].keyword))
                {
                        if ((spell_list[i].price * 100) > total_coins_char(ch))
                        {
                                act("$C says 'You do not have enough money for my services.'",
                                    ch, NULL, mob, TO_CHAR);
                                return;
                        }

                        coins_from_char(spell_list[i].price * 100, ch);
                        /* coins_to_char(spell_list[i].price * 100, mob, COINS_ADD); */
                        WAIT_STATE(ch, PULSE_VIOLENCE);

                        act("$c makes a magical pass over you.", mob, NULL, ch, TO_VICT);
                        act("$c makes a magical pass over $N.", mob, NULL, ch, TO_NOTVICT);

                        if (is_name(arg, "mana"))
                        {
                                ch->mana += 100;
                                ch->mana = UMIN(ch->mana,ch->max_mana);
                                send_to_char("A warm glow passes through you.\n\r",ch);
                                return;
                        }

                        sn = skill_lookup(spell_list[i].spell_name);
                        if (sn < 0 || sn >= MAX_SKILL)
                                return;
                        (*skill_table[sn].spell_fun) (sn, mob->level, mob, ch);
                        return;
                }
        }

        send_to_char("Type HEAL for a list of spells you may select.\n\r", ch);
}


/* EOF healer.c */
