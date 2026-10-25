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
#include "webgate.h"
#include "sound.h"

enum healer_service_kind
{
        HEALER_SPELL,
        HEALER_MANA,
        HEALER_BLESS_WEAPON,
        HEALER_REFILL,
        HEALER_MEND
};

struct healer_spell
{
        char *keyword;
        char *spell_name;
        int price;
        unsigned long int required_act;
        enum healer_service_kind kind;
        int amount;
};

static const struct healer_spell healer_services[] =
{
        /* Keep the ordinary healer menu and its prices. */
        {"light", "cure light", 2, 0, HEALER_SPELL, 0},
        {"serious", "cure serious", 5, 0, HEALER_SPELL, 0},
        {"critical", "cure critical", 10, 0, HEALER_SPELL, 0},
        {"heal", "heal", 15, 0, HEALER_SPELL, 0},
        {"power", "power heal", 35, 0, HEALER_SPELL, 0},
        {"blindness", "cure blindness", 5, 0, HEALER_SPELL, 0},
        {"poison", "cure poison", 5, 0, HEALER_SPELL, 0},
        {"stabilise", "stabilise", 5, 0, HEALER_SPELL, 0},
        {"regenerate", "regenerate", 35, 0, HEALER_SPELL, 0},
        {"curse", "remove curse", 5, 0, HEALER_SPELL, 0},
        {"refresh", "refresh", 1, 0, HEALER_SPELL, 0},
        {"bless", "bless", 10, 0, HEALER_SPELL, 0},
        {"mana", "restore mana", 40, 0, HEALER_MANA, 100},
        {"vitalize", "vitalize", 120, 0, HEALER_SPELL, 0},
        {"detect", "detect undead", 5, ACT_PRIEST_SERVICES, HEALER_SPELL, 0},
        {"protection", "protection vs evil", 10, ACT_PRIEST_SERVICES, HEALER_SPELL, 0},
        {"ward", "protection vs undead", 20, ACT_PRIEST_SERVICES, HEALER_SPELL, 0},
        {"blessweapon", "bless weapon", 100, ACT_PRIEST_SERVICES, HEALER_BLESS_WEAPON, 0},
        {"refill", "holy-water refill", 8, ACT_PRIEST_SERVICES, HEALER_REFILL, 15},
        {"mend", "mend undead", 15, ACT_UNDEAD_HEALER, HEALER_MEND, 100},
        {"greatermend", "greater mend undead", 35, ACT_UNDEAD_HEALER, HEALER_MEND, 300}
};

/* Two healers in one room can offer different services. */
static CHAR_DATA *healer_for_service(
        CHAR_DATA *ch, const struct healer_spell *service)
{
        CHAR_DATA *mob;

        for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
        {
                if (!mob->deleted && IS_NPC(mob)
                &&  IS_SET(mob->act, ACT_IS_HEALER)
                &&  (!service || !service->required_act
                     || IS_SET(mob->act, service->required_act)))
                        return mob;
        }
        return NULL;
}

static bool healer_service_harms_undead(
        CHAR_DATA *ch, CHAR_DATA *mob, const struct healer_spell *service)
{
        if (service->kind != HEALER_SPELL)
                return FALSE;
        return helpful_spell_harms_target(
            skill_lookup(service->spell_name), mob, ch);
}

static bool healer_can_mend_undead(CHAR_DATA *ch)
{
        return IS_UNDEAD(ch) && !is_noncorporeal_player(ch)
            && ch->position != POS_DEAD
            && !(IS_NPC(ch) && IS_SET(ch->act, ACT_NO_HEAL))
            && ch->hit < aggravated_healing_limit(ch);
}

void do_heal(CHAR_DATA *ch, char *argument)
{
        const struct healer_spell *service;
        CHAR_DATA *mob;
        OBJ_DATA *obj = NULL;
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int i, sn = -1, missing = 0, casting_level;
        int count = (int)(sizeof(healer_services) / sizeof(healer_services[0]));
        int64_t price;
        bool omitted_service = FALSE;

        if (!ch || ch->deleted || !ch->in_room)
                return;
        if (ch->fighting)
        {
                send_to_char("Not while fighting!\n\r", ch);
                return;
        }
        if (!(mob = healer_for_service(ch, NULL)))
        {
                send_to_char("You can't do that here.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);
        if (arg[0] == '\0')
        {
                if (webgate_is_web_client(ch))
                {
                        webgate_send_healer_services(ch, mob);
                        return;
                }

                send_to_char("\n\r{WThe following services are available here:{x\n\r\n\r", ch);
                for (i = 0; i < count; i++)
                {
                        service = &healer_services[i];
                        if (!(mob = healer_for_service(ch, service)))
                                continue;
                        if (healer_service_harms_undead(ch, mob, service))
                        {
                                omitted_service = TRUE;
                                continue;
                        }
                        if (service->kind == HEALER_MEND && !IS_UNDEAD(ch))
                                continue;
                        if (service->kind == HEALER_BLESS_WEAPON)
                                sprintf(buf, " {W%-13s{x {w%-22s {Y100 + 5 per weapon level gold{x\n\r",
                                        service->keyword, service->spell_name);
                        else if (service->kind == HEALER_REFILL)
                                sprintf(buf, " {W%-13s{x {w%-22s {Y8 gold / 15 units, rounded up{x\n\r",
                                        service->keyword, service->spell_name);
                        else
                                sprintf(buf, " {W%-13s{x {w%-22s {Y%5d gold{x\n\r",
                                        service->keyword, service->spell_name, service->price);
                        send_to_char(buf, ch);
                }
                if (omitted_service)
                        send_to_char("\n\rThe healer leaves out spells that would harm you.\n\r", ch);
                if (!IS_NPC(ch) && ch->sub_class == SUB_CLASS_VAMPIRE)
                        send_to_char("\n\rHoly HP healing restores 75% of its usual amount for vampire players. Regenerate is unchanged.\n\r", ch);
                send_to_char("\n\rUse HEAL <service>. Item services also need the item's name.\n\r", ch);
                webgate_send_healer_services(ch, healer_for_service(ch, NULL));
                return;
        }

        if (!IS_NPC(ch) && (!ch->pcdata || ch->pcdata->fame < 0))
        {
                act("$N says 'I won't heal the likes of you!'", ch, NULL, mob, TO_CHAR);
                return;
        }

        for (i = 0; i < count; i++)
                if (is_name(arg, healer_services[i].keyword))
                        break;
        if (i == count)
        {
                send_to_char("Type HEAL for a list of services you may select.\n\r", ch);
                return;
        }
        service = &healer_services[i];
        if (!(mob = healer_for_service(ch, service)))
        {
                send_to_char("No healer here offers that service.\n\r", ch);
                return;
        }
        if (healer_service_harms_undead(ch, mob, service))
        {
                act("$N says 'That spell would harm you.'", ch, NULL, mob, TO_CHAR);
                return;
        }

        price = service->price;
        casting_level = mob->level;
        if (service->kind == HEALER_SPELL || service->kind == HEALER_BLESS_WEAPON)
        {
                sn = skill_lookup(service->spell_name);
                if (sn < 0 || sn >= MAX_SKILL || !skill_table[sn].spell_fun
                ||  skill_table[sn].spell_fun == spell_null)
                {
                        send_to_char("That service is unavailable.\n\r", ch);
                        return;
                }
        }

        if (service->kind == HEALER_BLESS_WEAPON)
        {
                if (argument[0] == '\0')
                {
                        send_to_char("Syntax: heal blessweapon <carried weapon>\n\r", ch);
                        return;
                }
                obj = get_obj_carry(ch, argument);
                if (!obj || obj->deleted || obj->wear_loc != WEAR_NONE
                ||  obj->item_type != ITEM_WEAPON || IS_OBJ_STAT(obj, ITEM_BLESS)
                ||  obj->affected)
                {
                        send_to_char("Carry an unblessed weapon without existing affects for the priest to bless.\n\r", ch);
                        return;
                }
                if (IS_AFFECTED(mob, AFF_HEAD_TRAUMA))
                {
                        send_to_char("The priest cannot remember the blessing just now.\n\r", ch);
                        return;
                }
                price += 5 * (int64_t)UMAX(0, obj->level);
                casting_level = URANGE(1, UMIN(ch->level, mob->level), MAX_LEVEL);
        }
        else if (service->kind == HEALER_REFILL)
        {
                if (argument[0] == '\0')
                {
                        send_to_char("Syntax: heal refill <carried drink container>\n\r", ch);
                        return;
                }
                obj = get_obj_carry(ch, argument);
                if (!obj || obj->deleted || obj->wear_loc != WEAR_NONE
                ||  obj->item_type != ITEM_DRINK_CON || obj->value[0] <= 0
                ||  obj->value[1] < 0 || obj->value[1] > obj->value[0]
                ||  obj->value[3] != 0
                ||  (obj->value[1] > 0 && obj->value[2] != LIQ_HOLY_WATER))
                {
                        send_to_char("Use an empty, clean drink container, or one holding only clean holy water.\n\r", ch);
                        return;
                }
                missing = obj->value[0] - obj->value[1];
                if (missing == 0)
                {
                        send_to_char("That container is already full.\n\r", ch);
                        return;
                }
                price = ((int64_t)service->price * missing + service->amount - 1)
                      / service->amount;
        }
        else if (service->kind == HEALER_MEND)
        {
                if (!healer_can_mend_undead(ch))
                {
                        send_to_char("The dark energy cannot restore any more of your strength.\n\r", ch);
                        return;
                }
        }
        else if (service->required_act && service->kind == HEALER_SPELL)
        {
                bool already_present = sn == gsn_detect_undead
                    ? is_affected(ch, sn)
                    : is_affected_source(ch, sn, AFFECT_SOURCE_NONE, 0);
                if (already_present)
                {
                        send_to_char("That protection is already working for you.\n\r", ch);
                        return;
                }
                if (IS_NPC(ch) && IS_SET(ch->act, ACT_OBJECT))
                {
                        send_to_char("That protection is meant for a creature.\n\r", ch);
                        return;
                }
        }

        if (price * COIN_GOLD > total_coins_char(ch)
        ||  !coins_from_char(price * COIN_GOLD, ch))
        {
                act("$N says 'You do not have enough money for my services.'",
                    ch, NULL, mob, TO_CHAR);
                return;
        }
        WAIT_STATE(ch, PULSE_VIOLENCE);

        if (service->kind == HEALER_REFILL)
        {
                obj->value[1] += missing;
                obj->value[2] = LIQ_HOLY_WATER;
                act("$N refills $p with holy water.", ch, obj, mob, TO_CHAR);
                sprintf(buf, "The refill costs %lld gold.\n\r", (long long)price);
                send_to_char(buf, ch);
        }
        else if (service->kind == HEALER_MEND)
                restore_undead_energy(mob, ch, service->amount, FALSE);
        else if (service->kind == HEALER_MANA)
        {
                if (ch->mana < effective_max_mana(ch))
                        ch->mana = UMIN((long long)ch->mana + service->amount, effective_max_mana(ch));
                sound_spell_sfx(ch, 0, "cast");
                send_to_char("A warm glow passes through you.\n\r", ch);
        }
        else
        {
                act("$n makes a magical pass over you.", mob, NULL, ch, TO_VICT);
                act("$n makes a magical pass over $N.", mob, NULL, ch, TO_NOTVICT);
                sound_spell_sfx(ch, sn, "cast");
                (*skill_table[sn].spell_fun)(sn, casting_level, mob,
                    service->kind == HEALER_BLESS_WEAPON ? (void *)obj : (void *)ch);
                if (service->kind == HEALER_BLESS_WEAPON)
                {
                        sprintf(buf, "The blessing costs %lld gold.\n\r", (long long)price);
                        send_to_char(buf, ch);
                }
        }
        webgate_send_healer_services(ch, healer_for_service(ch, NULL));
}

/* EOF healer.c */
