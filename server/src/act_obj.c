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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"


/*
 * Local functions.
 */
void        get_obj         args((CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container));
bool        remove_obj      args((CHAR_DATA *ch, int iWear, bool fReplace));
void        wear_obj        args((CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace));
CHAR_DATA * find_keeper     args((CHAR_DATA *ch));
int         get_cost        args((CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy));


void get_obj (CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container)
{
        char buf [MAX_STRING_LENGTH];

        if (!IS_SET(obj->wear_flags, ITEM_TAKE) || obj->item_type == ITEM_PORTAL)
        {
                send_to_char("You can't take that.\n\r", ch);
                return;
        }

        if (!is_obj_owner(obj, ch))
        {
                sprintf(buf, "You can't take %s: it belongs to %s.\n\r",
                        obj->short_descr,
                        capitalize(get_obj_owner(obj)));
                send_to_char(buf, ch);
                return;
        }

        if (IS_SET( obj->extra_flags, ITEM_DEPLOYED) )
        {
                send_to_char("The turret is deployed, try RETRACTING it.\n\r", ch);
                return;
        }

        if (obj->item_type == ITEM_CLAN_OBJECT)
        {
                ROOM_INDEX_DATA *room, *home;

                if (ch->fighting)
                {
                        send_to_char("You can't get that while you are fighting!\n\r", ch);
                        return;
                }

                if (IS_NPC(ch) || (!ch->clan && ch->level <= LEVEL_HERO))
                {
                        send_to_char("You can't take that.\n\r", ch);
                        return;
                }

                if (ch->level <= LEVEL_HERO)
                {
                        home = get_room_index(clan_table[ch->clan].heal_room);

                        if (ch->in_room->vnum == clan_table[ch->clan].heal_room
                            || !home)
                        {
                                send_to_char("You can't take that.\n\r", ch);
                                return;
                        }

                        act("You get $p, which disappears in a flash of light.",
                            ch, obj, NULL, TO_CHAR);
                        act("$n gets $p, which disappears in a flash of light.",
                            ch, obj, NULL, TO_ROOM);

                        obj_from_room(obj);
                        obj_to_room(obj, home);
                        room = ch->in_room;
                        char_from_room(ch);
                        char_to_room(ch, home);

                        act("$p appears from thin air, filling the room with a soft orange glow.",
                            ch, obj, NULL, TO_ROOM);

                        char_from_room(ch);
                        char_to_room(ch, room);
                        return;
                }
        }

        /* for traps */
        if (checkgetput(ch, obj))
                return;

        if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
        {
                act("$d: you can't carry that many items.", ch, NULL, obj->name, TO_CHAR);
                return;
        }

        if ( (ch->carry_weight + ch->coin_weight) + get_obj_weight(obj) > can_carry_w(ch))
        {
                act("$d: you can't carry that much weight.", ch, NULL, obj->name, TO_CHAR);
                return;
        }

        if (container)
        {
                act("You get $p from $P.", ch, obj, container, TO_CHAR);
                act("$n gets $p from $P.", ch, obj, container, TO_ROOM);
                obj_from_obj(obj);
        }
        else
        {
                act("You get $p.", ch, obj, container, TO_CHAR);
                act("$n gets $p.", ch, obj, container, TO_ROOM);
                obj_from_room(obj);
        }

        if (obj->item_type == ITEM_MONEY)
        {
                char buf [ MAX_STRING_LENGTH ], tmp [256];
                const char *coin [] = { "copper", "silver", "gold", "platinum" };
                bool found = FALSE;
                int i;

                ch->copper += obj->value[0];
                ch->silver += obj->value[1];
                ch->gold   += obj->value[2];
                ch->plat   += obj->value[3];

                strcpy (buf, "You count");
                for (i = 3; i >= 0; i--)
                {
                        if (obj->value[i] > 0)
                        {
                                if (found)
                                        strcat (buf, ",");
                                found = TRUE;
                                sprintf (tmp, " %d %s coin%s",
                                         obj->value[i],
                                         coin[i],
                                         (obj->value[i] == 1) ? "" : "s");
                                strcat (buf, tmp);
                        }
                }
                if (found)
                {
                        strcat (buf, ".\n\r");
                        send_to_char(buf, ch);
                }

                calc_coin_weight(ch);
                extract_obj(obj);
        }
        else
                obj_to_char(obj, ch);

        return;
}

/* Retrieve objects from a player's vault */

void get_obj_vault (CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container)
{
        char buf [MAX_STRING_LENGTH];

        if (!IS_SET(obj->wear_flags, ITEM_TAKE) || obj->item_type == ITEM_PORTAL)
        {
                send_to_char("You can't claim that.\n\r", ch);
                return;
        }

        if (!is_obj_owner(obj, ch))
        {
                sprintf(buf, "You can't claim %s: it belongs to %s.\n\r",
                        obj->short_descr,
                        capitalize(get_obj_owner(obj)));
                send_to_char(buf, ch);
                return;
        }

        /* for traps */
        if (checkgetput(ch, obj))
                return;

        if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
        {
                act("$d: you can't carry that many items.", ch, NULL, obj->name, TO_CHAR);
                return;
        }

        if ( (ch->carry_weight + ch->coin_weight) + get_obj_weight(obj) > can_carry_w(ch))
        {
                act("$d: you can't carry that much weight.", ch, NULL, obj->name, TO_CHAR);
                return;
        }

        if (container)
        {
                act("You get $p from $P in your vault.", ch, obj, container, TO_CHAR);
                act("$n gets $p from $P in your vault.", ch, obj, container, TO_ROOM);
                obj_from_objvault(obj);
                ch->pcdata->vault_number++;
        }
        else
        {
                act("You get $p from your vault.", ch, obj, container, TO_CHAR);
                act("$n gets $p from your vault.", ch, obj, container, TO_ROOM);
                obj_from_charvault(obj);
        }

        obj_to_charfromvault(obj, ch);

        return;
}


void do_get (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pobj;
        OBJ_DATA *container;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        bool      found;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (arg1[0] == '\0')
        {
                send_to_char("Get what?\n\r", ch);
                return;
        }

        if (arg2[0] == '\0')
        {
                if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
                {
                        /* 'get obj' */
                        obj = get_obj_list(ch, arg1, ch->in_room->contents);
                        if (!obj)
                        {
                                act("I see no $T here.", ch, NULL, arg1, TO_CHAR);
                                return;
                        }
                        get_obj(ch, obj, NULL);

                        /* Do we get the object?? */
                        for (pobj = ch->carrying; pobj; pobj = pobj->next_content)
                        {
                                if (pobj == obj)
                                        break;
                        }

                        if (!pobj)
                                return;

                        /* Auto wield if in combat */
                        if (obj->item_type == ITEM_WEAPON
                            && ch->position == POS_FIGHTING
                            && IS_SET (ch->act, PLR_AUTOWIELD))
                        {
                                int class = 0;

                                /*
                                 * Check if current form can wield the weapon;
                                 * Have had problem with shifters losing weapon in normal
                                 * form, morphing, getting weapons and then equipping.
                                 * Gezhp 2000
                                 */
                                if (ch->class == CLASS_SHAPE_SHIFTER)
                                {
                                        if (ch->sub_class)
                                                class = ch->sub_class + MAX_CLASS - 1;
                                        else
                                                class = ch->class;
                                }

                                if (!get_eq_char(ch, WEAR_WIELD)
                                    || (!get_eq_char(ch, WEAR_DUAL) && CAN_DO(ch,gsn_dual)))
                                {
                                        if (!class || CAN_WEAR(class, ch->form, obj, ITEM_WIELD, BIT_WIELD))
                                                wear_obj(ch, obj, TRUE);
                                }
                        }
                }
                else
                {
                        /* 'get all' or 'get all.obj' */
                        OBJ_DATA *obj_next;
                        found = FALSE;

                        for (obj = ch->in_room->contents; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;

                                if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
                                    && can_see_obj(ch, obj))
                                {
                                        found = TRUE;
                                        get_obj(ch, obj, NULL);
                                }
                        }

                        if (!found)
                        {
                                if (arg1[3] == '\0')
                                        send_to_char("You see nothing here.\n\r", ch);
                                else
                                        act("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
                        }
                }
        }
        else
        {
                /* 'get ... container' */
                if (ch->position == POS_FIGHTING)
                {
                        send_to_char( "You cannot arrange your inventory while fighting.\n\r", ch );
                        return;
                }

                if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
                {
                        send_to_char("You can't do that.\n\r", ch);
                        return;
                }

                if (!(container = get_obj_here(ch, arg2)))
                {
                        act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
                        return;
                }

                switch (container->item_type)
                {
                    default:
                        send_to_char("That's not a container.\n\r", ch);
                        return;

                    case ITEM_CONTAINER:
                    case ITEM_TURRET:
                    case ITEM_CORPSE_NPC:
                    case ITEM_REMAINS:
                        break;

                    case ITEM_CORPSE_PC:
                        {
                                char      *pd;
                                char       name[ MAX_INPUT_LENGTH ];

                                if (IS_NPC(ch))
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }

                                pd = container->short_descr;
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);

                                if (!str_cmp(name, ch->name) && ch->level <= LEVEL_HERO)
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }
                        }
                }

                if (IS_SET(container->value[1], CONT_CLOSED))
                {
                        act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
                        return;
                }

                if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
                {
                        /* 'get obj container' */
                        obj = get_obj_list(ch, arg1, container->contains);
                        if (!obj)
                        {
                                act("You see nothing like that in the $T.",
                                    ch, NULL, arg2, TO_CHAR);
                                return;
                        }
                        get_obj(ch, obj, container);
                }
                else
                {
                        /* 'get all container' or 'get all.obj container' */
                        OBJ_DATA *obj_next;

                        found = FALSE;
                        for (obj = container->contains; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;
                                /* detect curse prevents autolooting of cursed and noremove/nodrop items */
                                /* Fix this so it checks spell num AND name at some point -- Owl 7/3/22 */
                                if ( ( ( IS_OBJ_STAT( obj, ITEM_NODROP )
                                        || IS_OBJ_STAT( obj, ITEM_NOREMOVE )
                                        || IS_OBJ_STAT( obj, ITEM_CURSED )
                                        || ( obj->value[1] == 33 )
                                        || ( obj->value[1] == 304 )
                                        || ( obj->value[1] == 458 )
                                        || ( obj->value[2] == 33 )
                                        || ( obj->value[2] == 304 )
                                        || ( obj->value[2] == 458 )
                                        || ( obj->value[3] == 33 )
                                        || ( obj->value[3] == 304 )
                                        || ( obj->value[3] == 458 ) )
                                        && IS_AFFECTED( ch, AFF_DETECT_CURSE ) ) )
                                {
                                        send_to_char("{WYou are reluctant to acquire a cursed item.{x\n\r", ch);
                                        continue;
                                }

                                if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
                                    && can_see_obj(ch, obj))
                                {
                                        found = TRUE;
                                        get_obj(ch, obj, container);
                                }
                        }

                        if (!found)
                        {
                               /* if (arg1[3] == '\0' && !IS_AFFECTED( ch, AFF_DETECT_CURSE ))
                                        act("You see nothing in the $T.",
                                            ch, NULL, arg2, TO_CHAR);
                                else if (arg1[3] == '\0' && IS_AFFECTED( ch, AFF_DETECT_CURSE ))
                                        act("You see nothing desirable in the $T.",
                                            ch, NULL, arg2, TO_CHAR);
                                else
                                        act("You see nothing like that in the $T.",
                                            ch, NULL, arg2, TO_CHAR);*/
                                if (arg1[3] == '\0' && !IS_AFFECTED( ch, AFF_DETECT_CURSE ))
                                        act("You see nothing in $P.",
                                            ch, NULL, container, TO_CHAR);
                                else if (arg1[3] == '\0' && IS_AFFECTED( ch, AFF_DETECT_CURSE ))
                                        act("You see nothing desirable in $P.",
                                            ch, NULL, container, TO_CHAR);
                                else
                                        act("You see nothing like that in $P.",
                                            ch, NULL, container, TO_CHAR);
                        }
                }
        }

        return;
}

/* Get objects from vault into inventory.  --Owl 23/2/23 */

void do_claim (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pobj;
        OBJ_DATA *container;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        char      buf  [ MAX_STRING_LENGTH ];
        bool      found;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
        {
            return;
        }

        if (!IS_SET(ch->in_room->room_flags, ROOM_VAULT))
        {
            send_to_char( "You cannot access your vault from this location.\n\r", ch );
            return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You can't claim items from your vault in your current form.\n\r", ch);
                return;
        }

        if (arg1[0] == '\0')
        {
                send_to_char("Claim what?\n\r", ch);
                return;
        }

        if (arg2[0] == '\0')
        {
                if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
                {
                        /* 'claim obj' */
                        obj = get_obj_list(ch, arg1, ch->pcdata->vault);
                        if (!obj)
                        {
                                act("There is no \"$T\" in your vault.", ch, NULL, arg1, TO_CHAR);
                                return;
                        }
                        get_obj_vault(ch, obj, NULL);

                        /* Do we claim the object?? */
                        for (pobj = ch->pcdata->vault; pobj; pobj = pobj->next_content)
                        {
                                if (pobj == obj)
                                        break;
                        }

                        if (!pobj)
                                return;
                }
                else
                {
                        /* 'claim all' or 'claim all.obj' */
                        OBJ_DATA *obj_next;
                        found = FALSE;

                        for (obj = ch->pcdata->vault; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;

                                if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
                                    && can_see_obj(ch, obj))
                                {
                                        found = TRUE;
                                        get_obj_vault(ch, obj, NULL);
                                }
                        }

                        if (!found)
                        {
                                if (arg1[3] == '\0')
                                        send_to_char("You don't see that in your vault.\n\r", ch);
                                else
                                        act("There is no \"$T\" in your vault.", ch, NULL, &arg1[4], TO_CHAR);
                        }
                }
        }
        else
        {
                /* 'claim ... container' */
                if (ch->position == POS_FIGHTING)
                {
                        send_to_char( "You can't claim items from your vault while fighting.\n\r", ch );
                        return;
                }

                if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
                {
                        send_to_char("You can't do that.\n\r", ch);
                        return;
                }

                if (!(container = get_obj_herevault(ch, arg2)))
                {
                        act("There is no \"$T\" in your vault.", ch, NULL, arg2, TO_CHAR);
                        return;
                }

                switch (container->item_type)
                {
                    default:
                        send_to_char("That's not a container.\n\r", ch);
                        return;

                    case ITEM_CONTAINER:
                    case ITEM_TURRET:
                    case ITEM_CORPSE_NPC:
                    case ITEM_REMAINS:
                        break;

                    case ITEM_CORPSE_PC:
                        {
                                char      *pd;
                                char       name[ MAX_INPUT_LENGTH ];

                                if (IS_NPC(ch))
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }

                                pd = container->short_descr;
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);

                                if (!str_cmp(name, ch->name) && ch->level <= LEVEL_HERO)
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }
                        }
                }

                if (IS_SET(container->value[1], CONT_CLOSED))
                {
                        act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
                        return;
                }

                if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
                {
                        /* 'claim obj container' */
                        obj = get_obj_list(ch, arg1, container->contains);
                        if (!obj)
                        {
                                act("You see nothing like that in the $T.",
                                    ch, NULL, arg2, TO_CHAR);
                                return;
                        }
                        get_obj_vault(ch, obj, container);
                }
                else
                {
                        /* 'claim all container' or 'claim all.obj container' */
                        OBJ_DATA *obj_next;

                        found = FALSE;
                        for (obj = container->contains; obj; obj = obj_next)
                        {
                                obj_next = obj->next_content;

                                if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
                                    && can_see_obj(ch, obj))
                                {
                                        found = TRUE;
                                        get_obj_vault(ch, obj, container);
                                }
                        }

                        if (!found)
                        {
                            sprintf(buf, "You don't see anything like that in %s.\r\n", container->short_descr );
                            send_to_char(buf, ch);
                            return;
                        }

                }
        }

        return;
}


void do_put (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *container;
        OBJ_DATA *obj;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (ch->position == POS_FIGHTING)
        {
                send_to_char( "You cannot arrange your inventory while fighting.\n\r", ch );
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot do that in your current form.\n\r",ch);
                return;
        }

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char("Put what in what?\n\r", ch);
                return;
        }

        if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
        {
                send_to_char("You can't do that.\n\r", ch);
                return;
        }

        if (!(container = get_obj_here(ch, arg2)))
        {
                act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
                return;
        }

        if (container->item_type != ITEM_CONTAINER)
        {
                if (container->item_type != ITEM_TURRET)
                {
                        send_to_char("That's not a container.\n\r", ch);
                        return;
                }
        }

        if (IS_SET(container->value[1], CONT_CLOSED))
        {
                act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
                return;
        }

        /* for traps */
        if (checkgetput(ch, container))
                return;

        if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
        {
                /* 'put obj container' */
                if (!(obj = get_obj_carry(ch, arg1)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                if (obj == container)
                {
                        send_to_char("You can't fold it into itself.\n\r", ch);
                        return;
                }

                if (!can_drop_obj(ch, obj))
                {
                        send_to_char("You can't let go of it.\n\r", ch);
                        return;
                }

                if ( (IS_SET(container->wear_flags, ITEM_WEAR_POUCH ) )
                    && ( obj->item_type != ITEM_POTION)
                    && ( obj->item_type != ITEM_PILL)
                    && ( obj->item_type != ITEM_PAINT) )
                {
                        send_to_char("You can only put potions, pills and paints into your pouch.\n\r", ch);
                        return;
                }

                if ( (IS_SET(container->ego_flags, EGO_ITEM_TURRET )) && (!IS_SET(obj->ego_flags, EGO_ITEM_TURRET_MODULE)))
                {
                        send_to_char("You can only put modules into the turret slots.\n\r", ch);
                        return;
                };

                if (IS_SET(container->ego_flags, EGO_ITEM_TURRET ))
                {
                      if ( ( (ch->pcdata->learned[gsn_turret] < 60 ) && (get_container_count(container) >=1 ) )
                      || ((ch->pcdata->learned[gsn_turret] < 85 ) && (get_container_count(container) >= 2 ) )
                      || ((ch->pcdata->learned[gsn_turret] < 95 ) && (get_container_count(container) >= 3 ) ) )
                      {
                                send_to_char("You will need to improve your knowledge of turrets.\n\r", ch);
                                return;
                      }
                }

                if (get_obj_weight(obj) + get_obj_weight(container)
                    > container->value[0])
                {
                        send_to_char("It won't fit.\n\r", ch);
                        return;
                }

                if (obj->level > ch->level)
                {
                        act("$p is too high level to be put in there.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                obj_from_char(obj);
                obj_to_obj(obj, container);
                act("You put $p in $P.", ch, obj, container, TO_CHAR);
                act("$n puts $p in $P.", ch, obj, container, TO_ROOM);
        }
        else
        {
                /* 'put all container' or 'put all.obj container' */
                OBJ_DATA *obj_next;

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if ((arg1[3] == '\0' || is_name(&arg1[4], obj->name))
                            && can_see_obj(ch, obj)
                            && obj->wear_loc == WEAR_NONE
                            && obj != container
                            && can_drop_obj(ch, obj)
                            && ch->level >= obj->level
                            && get_obj_weight(obj) + get_obj_weight(container)
                               <= container->value[0])
                        {
                                obj_from_char(obj);
                                obj_to_obj(obj, container);
                                act("You put $p in $P.", ch, obj, container, TO_CHAR);
                                act("$n puts $p in $P.", ch, obj, container, TO_ROOM);
                        }
                }
        }

        return;
}

void do_lodge (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *container;
        OBJ_DATA *obj;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        bool      found;
        int       maxlevel;

        argument        = one_argument(argument, arg1);
        argument        = one_argument(argument, arg2);
        container       = get_obj_vaulted(ch, arg2);


        if (IS_NPC(ch))
        {
            return;
        }

        if (!IS_SET(ch->in_room->room_flags, ROOM_VAULT))
        {
                send_to_char( "You cannot access your vault from this location.\n\r", ch );
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char( "You can't lodge items in your vault while fighting.\n\r", ch );
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You can't lodge items in your vault in your current form.\n\r",ch);
                return;
        }

        if (arg1[0] == '\0')
        {
                send_to_char("Which item do you wish to lodge in your vault?\n\r", ch);
                return;
        }

        if (container == NULL && arg2[0] != '\0')
        {
                act("I see no \"$T\" in your vault.", ch, NULL, arg2, TO_CHAR);
                return;
        }

        /* If you're not trying to put an item in a container and not an "all command" */
        if ( ( arg2[0] == '\0')
        &&     str_cmp(arg1, "all")
        &&     str_prefix("all.", arg1))
        {
                if (!(obj = get_obj_carry(ch, arg1)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                if (!can_drop_obj(ch, obj))
                {
                        send_to_char("You can't let go of it.\n\r", ch);
                        return;
                }

                if ( ( get_obj_weight(obj) + ch->pcdata->vault_weight ) > can_vault_w( ch ) )
                {
                        send_to_char("You can't put that much weight into your vault.\n\r", ch);
                        return;
                }

                if ( ( ( ch->pcdata->vault_number + get_obj_number(obj) ) > can_vault_n( ch ) ) )
                {
                        send_to_char("You can't fit that many items into your vault.\n\r", ch);
                        return;
                }

                if (obj->level > ( ch->level + VAULT_LEVEL_BUFFER) )
                {
                        act("$p is too high level to be put in your vault.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                maxlevel = max_obj_in_obj_level(obj);

                if (maxlevel > ( ch->level + VAULT_LEVEL_BUFFER ) )
                {
                        act("$p contains an item or items too high level to go in your vault.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                obj_from_char(obj);
                obj_to_charvault(obj, ch);
                act("You lodge $p in your vault.", ch, obj, NULL, TO_CHAR);
                act("$n lodges $p in $S vault.", ch, obj, ch, TO_ROOM);
                return;
        }

        /* If you are trying to put an item in a container and not an "all command" */
        if ( ( arg2[0] != '\0')
        &&     str_cmp(arg1, "all")
        &&     str_prefix("all.", arg1)
        &&     ( container != NULL ) )
        {
                if (IS_SET(container->value[1], CONT_CLOSED))
                {
                        act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
                        return;
                }

                /* 'put obj container' */
                if (!(container = get_obj_vaulted(ch, arg2)))
                {
                        send_to_char("You don't have that item in your vault.\n\r", ch);
                        return;
                }

                /* 'put obj container' */
                if (!(obj = get_obj_carry(ch, arg1)))
                {
                        send_to_char("You don't have that item on you.\n\r", ch);
                        return;
                }

                if (!can_drop_obj(ch, obj))
                {
                        send_to_char("You can't let go of it.\n\r", ch);
                        return;
                }

                if ( (IS_SET(container->wear_flags, ITEM_WEAR_POUCH ) )
                    && ( obj->item_type != ITEM_POTION)
                    && ( obj->item_type != ITEM_PILL)
                    && ( obj->item_type != ITEM_PAINT) )
                {
                        send_to_char("You can only put potions, pills and paints into your pouch.\n\r", ch);
                        return;
                }

                if ( (IS_SET(container->ego_flags, EGO_ITEM_TURRET )) && (!IS_SET(obj->ego_flags, EGO_ITEM_TURRET_MODULE)))
                {
                        send_to_char("You can only put modules into the turret slots.\n\r", ch);
                        return;
                };

                if (IS_SET(container->ego_flags, EGO_ITEM_TURRET ))
                {
                      if ( ( (ch->pcdata->learned[gsn_turret] < 60 ) && (get_container_count(container) >=1 ) )
                      || ((ch->pcdata->learned[gsn_turret] < 85 ) && (get_container_count(container) >= 2 ) )
                      || ((ch->pcdata->learned[gsn_turret] < 95 ) && (get_container_count(container) >= 3 ) ) )
                      {
                                send_to_char("You will need to improve your knowledge of turrets.\n\r", ch);
                                return;
                      }
                }

                if (get_obj_weight(obj) + get_obj_weight(container)
                    > container->value[0])
                {
                        send_to_char("It won't fit.\n\r", ch);
                        return;
                }

                if (obj->level > ( ch->level + VAULT_LEVEL_BUFFER ) )
                {
                        act("$p is too high level to be put in there.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                maxlevel = max_obj_in_obj_level(obj);

                if (maxlevel > ( ch->level + VAULT_LEVEL_BUFFER ) )
                {
                        act("$p contains an item or items too high level to go in your vault.", ch, obj, NULL, TO_CHAR);
                        return;
                }

                obj_from_char(obj);
                obj_to_objvault(obj, container);
                act("You put $p in $P in your vault.", ch, obj, container, TO_CHAR);
                act("$n puts $p in $P in $S vault.", ch, obj, container, TO_ROOM);

                return;
        }

        /* lodge all or lodge all.object */
        if ( ( !str_cmp(arg1, "all")
          ||   !str_prefix("all.", arg1) )
        &&   ( arg2[0] == '\0' ) )
        {
                OBJ_DATA *obj_next;
                container = NULL;
                found = FALSE;
                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if ( ( arg1[3] == '\0' || is_name(&arg1[4], obj->name ) )
                        &&     can_see_obj(ch, obj)
                        &&     obj->wear_loc == WEAR_NONE
                        &&     can_drop_obj(ch, obj)
                        &&     ( ch->level + VAULT_LEVEL_BUFFER ) >= obj->level
                        &&     (!obj->deleted)
                        &&     ( ( get_obj_number(obj) + ch->pcdata->vault_number) <= can_vault_n( ch ) )
                        &&     ( ( get_obj_weight(obj) + ch->pcdata->vault_weight) <= can_vault_w( ch ) ) )
                        {
                                found = TRUE;
                                obj_from_char(obj);
                                obj_to_charvault(obj, ch);
                                act("You lodge $p in your vault.", ch, obj, NULL, TO_CHAR);
                                act("$n lodges $p in $S vault.", ch, obj, ch, TO_ROOM);

                        }
                }

                if (!found)
                {
                        if (!ch->carrying)
                        {
                            send_to_char("You don't have anything to put in your vault.", ch);
                            return;

                        }
                        else {
                            send_to_char("You can't do that.", ch);
                            return;
                        }
                }
        }
        /* sprintf(log_buf,"value of container weight is %d\r\n", container->value[0]);
        log_string(log_buf); */
        /* lodge all container or lodge all.object container */
        if ( ( !str_cmp(arg1, "all")
          ||   !str_prefix("all.", arg1) )
        &&   ( container != NULL ) )
        {
                OBJ_DATA *obj_next;

                found = FALSE;

                if (IS_SET(container->value[1], CONT_CLOSED))
                {
                        act("The $d is closed.", ch, NULL, container->name, TO_CHAR);
                        return;
                }

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if ( ( arg1[3] == '\0' || is_name(&arg1[4], obj->name ) )
                        &&     can_see_obj(ch, obj)
                        &&     obj->wear_loc == WEAR_NONE
                        &&     can_drop_obj(ch, obj)
                        &&     (!obj->deleted)
                        &&     ( ch->level + VAULT_LEVEL_BUFFER ) >= obj->level
                        &&     ( ( get_obj_weight(obj) + get_obj_weight(container)) <= container->value[0] )
                        &&     ( ( get_obj_weight(obj) + ch->pcdata->vault_weight) <= can_vault_w( ch ) ) )
                        {
                                found = TRUE;
                                obj_from_char(obj);
                                obj_to_objvault(obj, container);
                                act("You lodge $p in $P in your vault.", ch, obj, container, TO_CHAR);
                                act("$n lodges $p in $P in $S vault.", ch, obj, container, TO_ROOM);

                        }
                }

                if (!found)
                {
                        if (!ch->carrying)
                        {
                            send_to_char("You don't have anything to put in your vault.", ch);
                            return;

                        }
                        else {
                            send_to_char("You can't do that.", ch);
                            return;
                        }
                }
        }

        return;
}


void do_deploy (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Deploy what?\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
	    {
                send_to_char ("<39>A powerful enchantment prevents you from deploying anything here.<0>\n\r", ch);
                return;
	    }

        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {
                /* 'drop obj' */
                if (!(obj = get_obj_carry(ch, arg)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);
                SET_BIT(obj->extra_flags, ITEM_DEPLOYED);
                act("You deploy your $p.", ch, obj, NULL, TO_CHAR);
                act("$n deploys their $p.", ch, obj, NULL, TO_ROOM);
        }
        else
        {
                act("One deployment at a time please.", ch, NULL, NULL, TO_CHAR);
                return;
        }
}

void do_retract (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Retract what?\n\r", ch);
                return;
        }


        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {


           /* 'get obj' */
                        obj = get_obj_list(ch, arg, ch->in_room->contents);
                        if (!obj)
                        {
                                act("I see no $T here.", ch, NULL, arg, TO_CHAR);
                                return;
                        }

                        REMOVE_BIT(obj->extra_flags, ITEM_DEPLOYED);
                        act("You retract your $p.", ch, obj, NULL, TO_CHAR);
                        act("$n retracts their $p.", ch, obj, NULL, TO_ROOM);
                        get_obj(ch, obj, NULL);

                        /* Do we get the object??
                        for (obj = ch->carrying; obj; obj = obj->next_content)
                        {
                                if (pobj == obj)
                                        break;
                        }

                        if (!pobj)
                                return; */

        }
        else
        {
                act("One object at a time please.", ch, NULL, NULL, TO_CHAR);
                return;
        }
}

/* Empty contents of containers in rooms. --Owl 9/12/23 */

void do_empty (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *container;
        char      arg [ MAX_INPUT_LENGTH ];
        bool      found;
        bool      first;

        argument = one_argument(argument, arg);

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char( "Not while you're fighting!\n\r", ch );
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Empty what?\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from emptying anything here.<0>\n\r", ch);
                return;
        }

        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {
                /* 'empty obj' */

                if (!(container = get_obj_here(ch, arg)))
                {
                        act("I see no $T here.", ch, NULL, arg, TO_CHAR);
                        return;
                }

                switch (container->item_type)
                {
                    default:
                        send_to_char("You can't empty that.\n\r", ch);
                        return;

                    case ITEM_CONTAINER:
                    case ITEM_CORPSE_NPC:
                    case ITEM_REMAINS:
                        break;

                    case ITEM_DRINK_CON:
                        {
                            if ( container->value[1] == 0)
                            {
                                send_to_char("It's already empty.\n\r", ch);
                                return;
                            }

                            act("You empty $p.", ch, container, NULL, TO_CHAR);
                            act("$n empties $p.", ch, container, NULL, TO_ROOM);
                            container->value[1] = 0;
                            return;
                        }

                    case ITEM_FOUNTAIN:
                        send_to_char("You can't empty that--its contents are inexhaustible!\n\r", ch);
                        return;

                    case ITEM_CORPSE_PC:
                        {
                                char      *pd;
                                char       name[ MAX_INPUT_LENGTH ];

                                if (IS_NPC(ch))
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }

                                pd = container->short_descr;
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);
                                pd = one_argument(pd, name);

                                if (!str_cmp(name, ch->name) && ch->level <= LEVEL_HERO)
                                {
                                        send_to_char("You can't do that.\n\r", ch);
                                        return;
                                }
                                else {
                                    /* Be an Imm looting a player corpse or a player emptying their own corpse? */
                                    OBJ_DATA *obj_next;
                                    found = FALSE;
                                    first = FALSE;

                                    for (obj = container->contains; obj; obj = obj_next)
                                    {
                                        obj_next = obj->next_content;

                                        if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
                                        &&   can_see_obj(ch, obj))
                                        {
                                                found = TRUE;

                                                if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
                                                {
                                                        act("Your hands are too full to get items from $p.", ch, container, NULL, TO_CHAR);
                                                        return;
                                                }

                                                if (first == FALSE)
                                                {
                                                    first = TRUE;
                                                    act("{WYou remove all the items from $p.{x", ch, container, NULL, TO_CHAR);
                                                    act("{W$n removes all the items from $p.{x", ch, container, NULL, TO_ROOM);
                                                }
                                                get_obj(ch, obj, container);
                                                obj_from_char(obj);
                                                obj_to_room(obj, ch->in_room);
                                                act("You drop $p.", ch, obj, NULL, TO_CHAR);
                                                act("$n drops $p.", ch, obj, NULL, TO_ROOM);
                                        }

                                    }

                                    if (!found)
                                    {
                                        /* Means there was nothing in the corpse */
                                        send_to_char("You can't see anything in it.\n\r", ch);
                                        return;

                                    }

                                    return;
                                }
                        }
                }

                if (IS_SET(container->value[1], CONT_CLOSED))
                {
                        act("$P is closed.", ch, NULL, container, TO_CHAR);
                        return;
                }

                OBJ_DATA *obj_next;
                found = FALSE;
                first = FALSE;

                for (obj = container->contains; obj; obj = obj_next)
                {
                    obj_next = obj->next_content;

                    if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
                    &&   can_see_obj(ch, obj))
                    {
                            found = TRUE;

                            if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
                            {
                                    act("Your hands are too full to empty $p.", ch, container, NULL, TO_CHAR);
                                    return;
                            }

                            if (first == FALSE)
                            {
                                first = TRUE;
                                act("{WYou empty $p.{x", ch, container, NULL, TO_CHAR);
                                act("{W$n empties $p.{x", ch, container, NULL, TO_ROOM);
                            }
                            get_obj(ch, obj, container);
                            obj_from_char(obj);
                            obj_to_room(obj, ch->in_room);
                            act("You drop $p.", ch, obj, NULL, TO_CHAR);
                            act("$n drops $p.", ch, obj, NULL, TO_ROOM);
                    }

                }

                if (!found)
                {
                    /* Means there was nothing in the container */
                    send_to_char("It's already empty.\n\r", ch);
                    return;

                }
                return;

        }
        else
        {
            send_to_char("What are you, a juggler?  One thing at a time.\n\r", ch);
            return;
        }

}

void do_drop (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        bool      found;

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Drop what?\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_DROP))
        {
                send_to_char ("<39>A powerful enchantment prevents you from dropping anything here.<0>\n\r", ch);
                return;
        }

        if (is_number(arg))
        {
                /* 'drop NNNN coins' */
                OBJ_DATA *obj_next;
                char      buf [ MAX_STRING_LENGTH ];
                int       amount, plat, gold, silver, copper;
                bool      dropok;

                amount   = atoi(arg);
                argument = one_argument(argument, arg);

                if (amount <= 0 )
                {
                        send_to_char("You can't drop negative coin amounts.\n\r", ch);
                        return;
                }

                dropok = FALSE;
                /* Zero out coins first */
                plat = gold = silver = copper = 0;

                if( !str_prefix(arg, "platinum") )
                {
                        if (ch->plat <= 0 )
                        {
                                sprintf(buf, "%s, you have no platinum coins to drop.\n\r", ch->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->plat )
                        {
                                sprintf(buf, "%s, you do not have %d platinum coins to drop.\n\r", ch->name, amount);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->plat -= amount;
                        plat = amount;
                        dropok = TRUE;
                }

                if( !str_prefix(arg, "gold") )
                {
                        if (ch->gold <= 0 )
                        {
                                sprintf(buf, "%s, you have no gold coins to drop.\n\r", ch->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->gold )
                        {
                                sprintf(buf, "%s, you do not have %d gold coins to drop.\n\r", ch->name, amount);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->gold -= amount;
                        gold = amount;
                        dropok = TRUE;
                }

                if( !str_prefix(arg, "silver") )
                {
                        if (ch->silver <= 0 )
                        {
                                sprintf(buf, "%s, you have no silver coins to drop.\n\r", ch->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->silver )
                        {
                                sprintf(buf, "%s, you do not have %d silver coins to drop.\n\r", ch->name, amount);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->silver -= amount;
                        silver = amount;
                        dropok = TRUE;
                }

                if( !str_prefix(arg, "copper") )
                {
                        if (ch->copper <= 0 )
                        {
                                sprintf(buf, "%s, you have no copper coins to drop.\n\r", ch->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->copper )
                        {
                                sprintf(buf, "%s, you do not have %d copper coins to drop.\n\r", ch->name, amount);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->copper -= amount;
                        copper = amount;
                        dropok = TRUE;
                }

                if( !dropok )
                {
                        sprintf(buf, "%s is not a valid coin of the realm.\n\r", arg);
                        send_to_char(buf, ch);
                        return;
                }

                calc_coin_weight(ch);

                for (obj = ch->in_room->contents; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if (obj->deleted)
                                continue;

                        switch (obj->pIndexData->vnum)
                        {
                            case OBJ_VNUM_MONEY_ONE:
                            case OBJ_VNUM_MONEY_SOME:
                                copper += obj->value[0];
                                silver += obj->value[1];
                                gold   += obj->value[2];
                                plat   += obj->value[3];
                                extract_obj(obj);
                                break;
                        }
                }

                obj_to_room(create_money(plat, gold, silver, copper), ch->in_room);

                send_to_char("Ok.\n\r", ch);
                act("$n drops some coins.", ch, NULL, NULL, TO_ROOM);
                return;
        }

        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {
                /* 'drop obj' */
                if (!(obj = get_obj_carry(ch, arg)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                if (!can_drop_obj(ch, obj))
                {
                        send_to_char("You can't let go of it.\n\r", ch);
                        return;
                }

                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);
                act("You drop $p.", ch, obj, NULL, TO_CHAR);
                act("$n drops $p.", ch, obj, NULL, TO_ROOM);

                if (obj->item_type == ITEM_CLAN_OBJECT
                    && ch->in_room->vnum == clan_table[ch->clan].heal_room )
                {
                        act("A orange glow emanates from $p as it is dropped, filling the room with warmth.",ch,obj,NULL,TO_CHAR);
                        act("A orange glow emanates from $p as it is dropped, filling the room with warmth.",ch,obj,NULL,TO_ROOM);
                }
        }
        else
        {
                /* 'drop all' or 'drop all.obj' */
                OBJ_DATA *obj_next;

                found = FALSE;
                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
                            && can_see_obj(ch, obj)
                            && obj->wear_loc == WEAR_NONE
                            && can_drop_obj(ch, obj))
                        {
                                found = TRUE;
                                obj_from_char(obj);
                                obj_to_room(obj, ch->in_room);
                                act("You drop $p.", ch, obj, NULL, TO_CHAR);
                                act("$n drops $p.", ch, obj, NULL, TO_ROOM);

                                if (obj->item_type == ITEM_CLAN_OBJECT &&  ch->in_room->vnum == clan_table[ch->clan].heal_room )
                                {
                                        act("A orange glow emanates from $p as it is dropped, filling the room with warmth.",ch,obj,NULL,TO_CHAR);
                                        act("A orange glow emanates from $p as it is dropped, filling the room with warmth.",ch,obj,NULL,TO_ROOM);
                                }
                        }
                }

                if (!found)
                {
                        if (arg[3] == '\0')
                                send_to_char("You are not carrying anything.", ch);
                        else
                                act("You are not carrying any $T.",
                                    ch, NULL, &arg[4], TO_CHAR);
                }
        }
}


void do_give (CHAR_DATA *ch, char *argument)
{
        /*
         * Edited so it uses the correct reference for mob names (victim->short_descr) when
         * money is being given to them. - Owl 19/2/22
         */

        CHAR_DATA *victim;
        OBJ_DATA  *obj;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot do that in your current form.\n\r",ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You're a bit busy for that!\n\r",ch);
                return;
        }

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char("Give what to whom?\n\r", ch);
                return;
        }

        if (is_number(arg1))
        {
                /* 'give NNNN coins victim' */
                int amount;
                char buf [ MAX_STRING_LENGTH ];
                bool giveok;

                amount = atoi(arg1);

                if (amount <= 0 )
                {
                        send_to_char("Sorry, you can't do that.\n\r", ch);
                        return;
                }

                argument = one_argument(argument, arg1);
                if (arg1[0] == '\0')
                {
                        send_to_char("Give what to whom?\n\r", ch);
                        return;
                }

                if (!(victim = get_char_room(ch, arg1)))
                {
                        send_to_char("They aren't here.\n\r", ch);
                        return;
                }

                if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                {
                        send_to_char("Not in their current form.\n\r", ch);
                        return;
                }

                giveok = FALSE;

                if( !str_prefix(arg2, "platinum") )
                {
                        if (ch->plat <= 0 )
                        {
                                sprintf(buf, "You have no platinum coins to give to %s.\n\r",
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->plat )
                        {
                                sprintf(buf, "You do not have %d platinum coins to give to %s.\n\r", amount,
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->plat -= amount;
                        victim->plat += amount;
                        amount = amount * COIN_PLAT;
                        giveok = TRUE;
                }

                if( !str_prefix(arg2, "gold") )
                {
                        if (ch->gold <= 0 )
                        {
                                sprintf(buf, "You have no gold coins to give to %s.\n\r",
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->gold )
                        {
                                sprintf(buf, "You do not have %d gold coins to give to %s.\n\r", amount,
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->gold -= amount;
                        victim->gold += amount;
                        amount = amount * COIN_GOLD;
                        giveok = TRUE;
                }

                if( !str_prefix(arg2, "silver") )
                {
                        if (ch->silver <= 0 )
                        {
                                sprintf(buf, "You have no silver coins to give to %s.\n\r",
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->silver )
                        {
                                sprintf(buf, "You do not have %d silver coins to give to %s.\n\r", amount,
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->silver -= amount;
                        victim->silver += amount;
                        amount = amount * COIN_SILVER;
                        giveok = TRUE;
                }

                if( !str_prefix(arg2, "copper") )
                {
                        if (ch->copper <= 0 )
                        {
                                sprintf(buf, "You have no copper coins to give to %s.\n\r",
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        if (amount > ch->copper )
                        {
                                sprintf(buf, "You do not have %d copper coins to give to %s.\n\r", amount,
                                        IS_NPC(victim) ? victim->short_descr : victim->name);
                                send_to_char(buf, ch);
                                return;
                        }
                        ch->copper -= amount;
                        victim->copper += amount;
                        giveok = TRUE;
                }

                if( !giveok )
                {
                        sprintf(buf, "%s is not a valid coin of the realm.\n\r", arg2);
                        send_to_char(buf, ch);
                        return;
                }

                if (amount == 1)
                {
                        act("You give $N a coin.",  ch, NULL, victim, TO_CHAR);
                        act("$n gives you a coin.", ch, NULL, victim, TO_VICT);
                        act("$n gives $N a coin.",  ch, NULL, victim, TO_NOTVICT);}
                else {
                        act("You give $N some coins.",  ch, NULL, victim, TO_CHAR);
                        act("$n gives you some coins.", ch, NULL, victim, TO_VICT);
                        act("$n gives $N some coins.",  ch, NULL, victim, TO_NOTVICT);
                }

                mprog_bribe_trigger(victim, ch, amount);

                return;
        }

        if (!(obj = get_obj_carry(ch, arg1)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (obj->wear_loc != WEAR_NONE)
        {
                send_to_char("You must remove it first.\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg2)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (!can_drop_obj(ch, obj))
        {
                send_to_char("You can't let go of it.\n\r", ch);
                return;
        }

        if (victim->carry_number + get_obj_number(obj) > can_carry_n(victim))
        {
                act("$N has $S hands full.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (victim->carry_weight + victim->coin_weight + get_obj_weight(obj) > can_carry_w(victim))
        {
                act("$N can't carry that much weight.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (!can_see_obj(victim, obj))
        {
                act("$N can't see it.", ch, NULL, victim, TO_CHAR);
                return;
        }

        if (!is_obj_owner(obj, victim))
        {
                act("$p doesn't belong to $N.", ch, obj, victim, TO_CHAR);
                return;
        }

        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in their current form.\n\r", ch);
                return;
        }

        obj_from_char(obj);
        obj_to_char(obj, victim);
        act("You give $p to $N.", ch, obj, victim, TO_CHAR);
        act("$n gives you $p.",   ch, obj, victim, TO_VICT);
        act("$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
        mprog_give_trigger (victim, ch, obj);   /* for mob progs - Brutus */
}


void do_fill (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *fountain;
        char      arg [ MAX_INPUT_LENGTH ];
        bool      found;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Fill what?\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        found = FALSE;

        for (fountain = ch->in_room->contents; fountain;
             fountain = fountain->next_content)
        {
                if (fountain->item_type == ITEM_FOUNTAIN)
                {
                        found = TRUE;
                        break;
                }
        }

        if (!found)
        {
                send_to_char("There is no fountain here!\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_DRINK_CON)
        {
                send_to_char("You can't fill that.\n\r", ch);
                return;
        }

        if (obj->value[1] != 0 && obj->value[2] != fountain->value[2])
        {
                send_to_char("There is already another liquid in it.\n\r", ch);
                return;
        }

        if (obj->value[1] >= obj->value[0])
        {
                send_to_char("Your container is full.\n\r", ch);
                return;
        }

        act("You fill $p with $T.", ch, obj, liq_table[fountain->value[2]].liq_name, TO_CHAR);
        act("$n fills $p with $T.", ch, obj, liq_table[fountain->value[2]].liq_name, TO_ROOM);

        obj->value[2] = fountain->value[2];
        obj->value[1] = obj->value[0];
        return;
}


void do_drink (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];
        int       amount;
        int       liquid;

        one_argument(argument, arg);

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                {
                        if (obj->item_type == ITEM_FOUNTAIN)
                                break;
                }

                if (!obj)
                {
                        if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                        {
                                send_to_char("Get moisture from what?\n\r", ch);
                        }
                        else {
                                send_to_char("Drink what?\n\r", ch);
                        }
                        return;
                }
        }
        else
        {
                if (!(obj = get_obj_here(ch, arg)))
                {
                        send_to_char("You can't find it.\n\r", ch);
                        return;
                }
        }

        if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] >= MAX_DRUNK)
        {
                if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                {
                        send_to_char("You're too drunk to do that.  *Hic*\n\r", ch);
                }
                else {
                        send_to_char("You fail to reach your mouth.  *Hic*\n\r", ch);
                }
                return;
        }

        switch (obj->item_type)
        {
            default:
                if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                {
                        send_to_char("You can't get moisture from that.\n\r", ch);
                }
                else {
                        send_to_char("You can't drink from that.\n\r", ch);
                }
                break;

            case ITEM_FOUNTAIN:

                liquid = obj->value[2];

                if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                {
                        /* Bathing in any liquid restores body moisture for sahuagin and grung */
                        if (!IS_NPC(ch))
                        {
                            ch->pcdata->condition[COND_THIRST] = MAX_DRINK;
                        }
                        act("You bathe yourself in $T from $p.",ch,obj,liq_table[obj->value[2]].liq_name,TO_CHAR);
                        send_to_char("You are fully hydrated.\n\r",ch);
                        act("$n bathes $mself in $T from $p.", ch, obj, liq_table[obj->value[2]].liq_name, TO_ROOM);
                }
                else {
                        act("You drink $T from $p.", ch, obj, liq_table[obj->value[2]].liq_name, TO_CHAR);
                        act("$n drinks $T from $p.", ch, obj, liq_table[obj->value[2]].liq_name, TO_ROOM);

                        amount = MAX_DRINK;

                        gain_condition(ch, COND_DRUNK, amount * liq_table[liquid].liq_effect[COND_DRUNK  ]);
                        gain_condition(ch, COND_FULL, amount * liq_table[liquid].liq_effect[COND_FULL   ]);
                        gain_condition(ch, COND_THIRST, amount * liq_table[liquid].liq_effect[COND_THIRST ]);

                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] >= 1 && ch->pcdata->condition[COND_DRUNK] <= 13)
                                send_to_char("You feel a bit tipsy.\n\r", ch);

                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 13 && ch->pcdata->condition[COND_DRUNK] <= 26)
                                send_to_char("You are inebriated.\n\r", ch);

                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 26 && ch->pcdata->condition[COND_DRUNK] <= 39)
                                send_to_char("You are really quite drunk.\n\r", ch);

                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 39)
                                send_to_char("You are absolutely blind roaring drunk.\n\r", ch);

                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > (MAX_DRINK - 5))
                        {
                                send_to_char("You do not feel thirsty.\n\r", ch);
                        }

                        if ( ch->race != RACE_SAHUAGIN && ch->race != RACE_GRUNG )
                        {
                                if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > ( MAX_FOOD - 5))
                                {
                                        send_to_char("You are full.\n\r", ch);
                                }
                        }

                        /* Fountain has been poisoned */
                        if ( obj->value[3] != 0 )
                        {
                                AFFECT_DATA af;

                                send_to_char("You choke and gag.\n\r", ch);
                                act("$n chokes and gags.", ch, NULL, NULL, TO_ROOM);
                                af.type      = gsn_poison;
                                af.duration  = 3 * amount;
                                af.location  = APPLY_STR;
                                af.modifier  = -5;
                                af.bitvector = AFF_POISON;
                                affect_join(ch, &af);
                        }

                }

                /* Vampires can't just drink blood from containers/fountains */
                if ( ( ch->sub_class == SUB_CLASS_VAMPIRE)
                &&   ( liquid == 13 )
                &&   ( obj->value[3] == 0) )
                {
                    send_to_char("{RDelicious, but blood must be from a fresh corpse to satisfy your bloodlust.{x\n\r", ch);
                }

                break;

            case ITEM_DRINK_CON:
                if (obj->value[1] <= 0)
                {
                        send_to_char("It's empty.\n\r", ch);
                        return;
                }

                if ((liquid = obj->value[2]) >= LIQ_MAX)
                {
                        bug("Do_drink: bad liquid number %d.", liquid);
                        liquid = obj->value[2] = 0;
                }

                if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                {
                        act("You cover yourself in $T from $p.",
                                ch, obj, liq_table[liquid].liq_name, TO_CHAR);
                        act("$n covers $mself in $T from $p.",
                                ch, obj, liq_table[liquid].liq_name, TO_ROOM);
                }
                else {
                        act("You drink $T from $p.",
                                ch, obj, liq_table[liquid].liq_name, TO_CHAR);
                        act("$n drinks $T from $p.",
                                ch, obj, liq_table[liquid].liq_name, TO_ROOM);
                }
                amount = number_range(3, 10);
                amount = UMIN(amount, obj->value[1]);

                /* Salt water has the same effect as regular water for sahuagin & grung */

                if ( ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                &&   ( liquid == 14 ) )
                        liquid = 0;

                gain_condition(ch, COND_DRUNK,
                               amount * liq_table[liquid].liq_effect[COND_DRUNK  ]);
                gain_condition(ch, COND_FULL,
                               amount * liq_table[liquid].liq_effect[COND_FULL   ]);
                gain_condition(ch, COND_THIRST,
                               amount * liq_table[liquid].liq_effect[COND_THIRST ]);


                if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] >= 1 && ch->pcdata->condition[COND_DRUNK] <= 13)
                        send_to_char("You feel a bit tipsy.\n\r", ch);

                if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 13 && ch->pcdata->condition[COND_DRUNK] <= 26)
                        send_to_char("You are inebriated.\n\r", ch);

                if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 26 && ch->pcdata->condition[COND_DRUNK] <= 39)
                        send_to_char("You are really quite drunk.\n\r", ch);

                if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK ] > 39)
                        send_to_char("You are absolutely blind roaring drunk.\n\r", ch);


                if ( ch->race != RACE_SAHUAGIN && ch->race != RACE_GRUNG )
                {
                    if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL  ] > ( MAX_FOOD - 5))
                            send_to_char("You are full.\n\r", ch);
                }

                if ( ch->race == RACE_SAHUAGIN || ch->race == RACE_GRUNG )
                {
                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > ( MAX_DRINK - 5))
                                send_to_char("Your body's moisture is completely restored.\n\r", ch);
                }
                else {
                        if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > ( MAX_DRINK - 5))
                                send_to_char("You do not feel thirsty.\n\r", ch);
                }

                /* Vampires can't just drink blood from containers/fountains */
                if ( (ch->sub_class == SUB_CLASS_VAMPIRE)
                    && ( liquid == 13 )
                    && (obj->value[3] == 0) )
                {
                    send_to_char("{RDelicious, but blood must be from a fresh corpse to satisfy your bloodlust.{x\n\r", ch);
                }

                if ( obj->value[3] != 0 )
                {
                        AFFECT_DATA af;

                        send_to_char("You choke and gag.\n\r", ch);
                        act("$n chokes and gags.", ch, NULL, NULL, TO_ROOM);
                        af.type      = gsn_poison;
                        af.duration  = 3 * amount;
                        af.location  = APPLY_STR;
                        af.modifier  = -5;
                        af.bitvector = AFF_POISON;
                        affect_join(ch, &af);
                }

                obj->value[1] -= amount;

                if (obj->value[1] <= 0)
                {
                        obj->value[1] = 0;
                        send_to_char("The container is now empty.\n\r", ch);
                }
                break;
        }

        return;
}


void do_enter (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char arg [ MAX_INPUT_LENGTH ];
        ROOM_INDEX_DATA *room = NULL;
        int i;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                for (obj = ch->in_room->contents; obj; obj = obj->next_content)
                {
                        if (obj->item_type == ITEM_PORTAL)
                                break;
                }

                if (!obj)
                {
                        send_to_char("Enter what?\n\r", ch);
                        return;
                }
        }
        else
        {
                if (!(obj = get_obj_here(ch, arg)))
                {
                        send_to_char("You can't find it.\n\r", ch);
                        return;
                }
        }

        if (obj->item_type != ITEM_PORTAL)
        {
                send_to_char("You can't enter that.\n\r", ch);
                return;
        }


        /*
         * Portal code updated by Gezhp, November 2000
         *
         * Values are now:
         * <room vnun low> <room vnum high> <lower level limit> <upper level limit>
         *
         * If value0 < 1, portal is dead and cannot be used.
         *
         * If value1 > 0, attempt made to take random room vnum within low-high
         * range.
         *
         * Level limits restrict mortals entering the portal
         */

        if (obj->value[0] < 1)
        {
                send_to_char("The portal is dead: you cannot use it.\n\r", ch);
                return;
        }

        if (!IS_NPC(ch) && ch->level <= LEVEL_HERO
            && (ch->level < obj->value[2] || ch->level > obj->value[3]))
        {
                send_to_char("You cannot use that portal.\n\r", ch);
                return;
        }

        if (obj->value[1] > 0)
        {
                for (i = 0; i < 100; i++)
                {
                        if ((room = get_room_index(number_range(obj->value[0],
                                                                obj->value[1]))))
                                break;
                }
        }
        else
                room = get_room_index(obj->value[0]);

        if (!room)
        {
                send_to_char("You are completely lost.\n\r", ch);
                return;
        }

        if (is_affected(ch,gsn_mist_walk))
        {
                act( "<87>A glowing mist drifts through the shimmering portal...<0>",
                    ch, NULL, NULL, TO_ROOM );
                act( "<87>You drift into the shimmering portal...<0>\n\r",
                    ch, NULL, NULL,TO_CHAR );
        }
        else
        {
                act( "<87>$n calmly steps through the shimmering portal...<0>",
                    ch, NULL, NULL, TO_ROOM );
                act( "<87>You step into the shimmering portal...<0>\n\r",
                    ch, NULL, NULL, TO_CHAR );
        }

        char_from_room( ch );
        char_to_room( ch, room );

        if (is_affected(ch,gsn_mist_walk))
                act( "<87>A glowing mist emerges from the portal.<0>", ch, NULL, NULL,TO_ROOM);
        else
                act( "<87>$n emerges from the portal.<0>", ch, NULL, NULL, TO_ROOM );

        do_look( ch, "auto" );

        if (ch->mount)
        {
                char_from_room (ch->mount);
                char_to_room (ch->mount, room);
        }

        if (obj->pIndexData->vnum == OBJECT_VNUM_PURGATORY_PORTAL)
                save_char_obj(ch);
}


void do_eat (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pouch;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        /* Imms can scoff their entire inventories.  Why not?  -- Owl 9/3/22 */

        if ( ( !str_cmp(arg, "all") )
         &&  ( ch->level > LEVEL_HERO ) )
        {
                OBJ_DATA *obj_next;

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;
                        extract_obj(obj);
                }
                act("You eat everything you are carrying.", ch, obj, NULL, TO_CHAR);
                act("$n eats everything $e is carrying.", ch, obj, NULL, TO_ROOM);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Eat what?\n\r", ch);
                return;
        }

        if (!(obj = get_obj_here(ch, arg)))
        {
                send_to_char("You can't find it.\n\r", ch);
                return;
        }

        /* Pouch code follows... */

        pouch = get_eq_char(ch, WEAR_POUCH);

        if ( ch->position == POS_FIGHTING
        &&  !IS_NPC(ch) )
        {
                if (!pouch)
                {
                        send_to_char("You are not wearing a pouch to get a pill from!\n\r", ch);
                        return;
                }
                else
                {
                        /*
                        if (!check_blind(ch))
                                return;
                        */

                        if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that in your pouch.\n\r", ch);
                                return;
                        }
                }
        }
        else
        {
                if (!(obj = get_obj_carry(ch, arg))
                &&  !IS_NPC(ch) )
                {
                        if (!pouch) {
                                send_to_char("You do not have that pill on your person.\n\r", ch);
                                return;
                        }
                        else if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that pill on you or in your pouch.\n\r", ch);
                                return;
                        }
                }
        }

        if ( ch->position == POS_FIGHTING
        &&   IS_NPC(ch) )
        {
                /* Mobs can only eat pills they are holding during combat -- Owl 1/7/22 */
                if (!(obj = get_eq_char(ch, WEAR_HOLD)))
                {
                        send_to_char("You can only eat pills you are holding during combat!\n\r", ch);
                        return;
                }

        }

        if (ch->level <= LEVEL_HERO)
        {
                if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL)
                {
                        send_to_char("That's not edible.\n\r", ch);
                        return;
                }

                if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > ( MAX_FOOD - 5))
                {
                        send_to_char("You are too full to eat more.\n\r", ch);
                        return;
                }
        }

        if (obj->item_type == ITEM_PILL
            && tournament_action_illegal(ch, TOURNAMENT_OPTION_NOPILL))
        {
                send_to_char("You may not eat pills during this tournament!\n\r", ch);
                return;
        }

        act("You eat $p.", ch, obj, NULL, TO_CHAR);
        act("$n eats $p.", ch, obj, NULL, TO_ROOM);

        switch (obj->item_type)
        {
            case ITEM_FOOD:
                if (!IS_NPC(ch))
                {
                        int condition;

                        condition = ch->pcdata->condition[COND_FULL];
                        gain_condition(ch, COND_FULL, obj->value[0]);
                        if (ch->pcdata->condition[COND_FULL] > (MAX_FOOD - 5))
                                send_to_char("You are full.\n\r", ch);
                        else if (condition == 0 && ch->pcdata->condition[COND_FULL] > 0)
                                send_to_char("You are no longer hungry.\n\r", ch);
                }

                if (obj->value[3] != 0)
                {
                        AFFECT_DATA af;

                        act("$n chokes and gags.", ch, 0, 0, TO_ROOM);
                        send_to_char("You choke and gag.\n\r", ch);

                        af.type      = gsn_poison;
                        af.duration  = 2 * obj->value[0];
                        af.location  = APPLY_STR;
                        af.modifier  = -5;
                        af.bitvector = AFF_POISON;
                        affect_join(ch, &af);
                }
                break;

            case ITEM_PILL:
                if (obj->level > ch->level)
                        act("$p is too high level for you.", ch, obj, NULL, TO_CHAR);
                else
                {
                        obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
                        obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
                        obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
                }
                break;
        }

        if (ch->fighting)
                WAIT_STATE(ch, PULSE_VIOLENCE/2);

        extract_obj(obj);
        return;
}


bool remove_obj (CHAR_DATA *ch, int iWear, bool fReplace)
{
        OBJ_DATA *obj;

        if (!(obj = get_eq_char(ch, iWear)))
                return TRUE;

        if (!fReplace)
                return FALSE;

        if (IS_SET(obj->extra_flags, ITEM_NOREMOVE))
        {
                act("You can't remove $p.", ch, obj, NULL, TO_CHAR);
                return FALSE;
        }

        if (IS_SET(obj->extra_flags, ITEM_BODY_PART))
        {
                act("You try in vain to pull your $p from your body, but to no avail.", ch, obj, NULL, TO_CHAR);
                return FALSE;
        }

        if( obj->item_type == ITEM_INSTRUMENT && obj->wear_loc == WEAR_HOLD )
                remove_songs( ch );

        unequip_char(ch, obj);
        act("$n stops using $p.", ch, obj, NULL, TO_ROOM);
        act("You stop using $p.", ch, obj, NULL, TO_CHAR);
        return TRUE;
}


/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 *
 * Learn C they said.  Write beautiful code they said.  Run a MUD they said.
 */
void wear_obj (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace)
{
        char buf [MAX_STRING_LENGTH];
        int eff_class;

        eff_class = ch->class;
        if (ch->sub_class)
                eff_class = MAX_CLASS - 1 + ch->sub_class;

        if (ch->level < obj->level)
        {
                sprintf(buf, "You must be at least level %d to use this object.\n\r",
                        obj->level);
                send_to_char(buf, ch);
                act("$n tries to use $p, but is too inexperienced.",
                    ch, obj, NULL, TO_ROOM);
                return;
        }

        if (!IS_NPC(ch)) {
            if (   (ch->class == CLASS_MAGE          && IS_OBJ_STAT(obj, ITEM_ANTI_MAGE))
                || (ch->class == CLASS_CLERIC        && IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC))
                || (ch->class == CLASS_THIEF         && IS_OBJ_STAT(obj, ITEM_ANTI_THIEF))
                || (ch->class == CLASS_WARRIOR       && IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR))
                || (ch->class == CLASS_PSIONICIST    && IS_OBJ_STAT(obj, ITEM_ANTI_PSIONIC))
                || (ch->class == CLASS_BRAWLER       && IS_OBJ_STAT(obj, ITEM_ANTI_BRAWLER))
                || (ch->class == CLASS_RANGER        && IS_OBJ_STAT(obj, ITEM_ANTI_RANGER))
                || (ch->class == CLASS_SMITHY        && IS_OBJ_STAT(obj, ITEM_ANTI_SMITHY))
                || (ch->class == CLASS_SHAPE_SHIFTER && IS_OBJ_STAT(obj, ITEM_ANTI_SHAPE_SHIFTER)))
            {
                    act("Your class cannot use $p.",ch,obj,NULL,TO_CHAR);
                    act("$n tries to use $p, but $s class cannot use it.", ch, obj, NULL, TO_ROOM);
                    return;
            }
        }

        if (!is_obj_owner(obj, ch))
        {
                sprintf(buf, "You can't wear %s: it belongs to %s.\n\r",
                        obj->short_descr,
                        capitalize_initial(get_obj_owner(obj)));
                send_to_char(buf, ch);
                return;
        }


        if (obj->item_type == ITEM_LIGHT)
        {
                if (CAN_WEAR(eff_class, ch->form, obj, ITEM_TAKE, BIT_LIGHT))
                {
                        if (!remove_obj(ch, WEAR_LIGHT, fReplace))
                                return;
                        act("You light $p and hold it.",  ch, obj, NULL, TO_CHAR);
                        act("$n lights $p and holds it.", ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_LIGHT);
                        return;
                }
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_FINGER, BIT_WEAR_FINGER))
        {
                if (get_eq_char(ch, WEAR_FINGER_L)
                    && get_eq_char(ch, WEAR_FINGER_R)
                    && !remove_obj(ch, WEAR_FINGER_L, fReplace)
                    && !remove_obj(ch, WEAR_FINGER_R, fReplace))
                        return;

                if (!get_eq_char(ch, WEAR_FINGER_L))
                {
                        act("You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR);
                        act("$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_FINGER_L);
                        return;
                }

                if (!get_eq_char(ch, WEAR_FINGER_R))
                {
                        act("You wear $p on your right finger.", ch, obj, NULL, TO_CHAR);
                        act("$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_FINGER_R);
                        return;
                }

                bug("Wear_obj: no free finger.", 0);
                send_to_char("You already wear two rings.\n\r", ch);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_NECK, BIT_WEAR_NECK))
        {
                if (get_eq_char(ch, WEAR_NECK_1)
                    &&   get_eq_char(ch, WEAR_NECK_2)
                    &&   !remove_obj(ch, WEAR_NECK_1, fReplace)
                    &&   !remove_obj(ch, WEAR_NECK_2, fReplace))
                        return;

                if (!get_eq_char(ch, WEAR_NECK_1))
                {
                        act("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
                        act("$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_NECK_1);
                        return;
                }

                if (!get_eq_char(ch, WEAR_NECK_2))
                {
                        act("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
                        act("$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_NECK_2);
                        return;
                }

                bug("Wear_obj: no free neck.", 0);
                send_to_char("You already wear two neck items.\n\r", ch);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_BODY, BIT_WEAR_BODY))
        {
                if (!remove_obj(ch, WEAR_BODY, fReplace))
                        return;
                act("You wear $p on your body.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s body.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_BODY);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_HEAD, BIT_WEAR_HEAD))
        {
                if (!remove_obj(ch, WEAR_HEAD, fReplace))
                        return;
                act("You wear $p on your head.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_HEAD);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_LEGS, BIT_WEAR_LEGS))
        {
                if (!remove_obj(ch, WEAR_LEGS, fReplace))
                        return;
                act("You wear $p on your legs.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_LEGS);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_FEET, BIT_WEAR_FEET))
        {
                if (!remove_obj(ch, WEAR_FEET, fReplace))
                        return;
                act("You wear $p on your feet.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_FEET);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_HANDS, BIT_WEAR_HANDS))
        {
                if (!remove_obj(ch, WEAR_HANDS, fReplace))
                        return;
                act("You wear $p on your hands.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_HANDS);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_ARMS, BIT_WEAR_ARMS))
        {
                if (!remove_obj(ch, WEAR_ARMS, fReplace))
                        return;
                act("You wear $p on your arms.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_ARMS);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_ABOUT, BIT_WEAR_ABOUT))
        {
                if (!remove_obj(ch, WEAR_ABOUT, fReplace))
                        return;
                act("You wear $p about your body.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_ABOUT);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_WAIST, BIT_WEAR_WAIST))
        {
                if (!remove_obj(ch, WEAR_WAIST, fReplace))
                        return;
                act("You wear $p about your waist.", ch, obj, NULL, TO_CHAR);
                act("$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_WAIST);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_POUCH, BIT_WEAR_POUCH))
        {
                if (!remove_obj(ch, WEAR_POUCH, fReplace))
                        return;
                act("You secure $p onto your belt.", ch, obj, NULL, TO_CHAR);
                act("$n secures $p onto $s belt.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_POUCH);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_WRIST, BIT_WEAR_WRIST))
        {
                if (get_eq_char(ch, WEAR_WRIST_L)
                    &&   get_eq_char(ch, WEAR_WRIST_R)
                    &&   !remove_obj(ch, WEAR_WRIST_L, fReplace)
                    &&   !remove_obj(ch, WEAR_WRIST_R, fReplace))
                        return;

                if (!get_eq_char(ch, WEAR_WRIST_L))
                {
                        act("You wear $p around your left wrist.", ch, obj, NULL, TO_CHAR);
                        act("$n wears $p around $s left wrist.", ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_WRIST_L);
                        return;
                }

                if (!get_eq_char(ch, WEAR_WRIST_R))
                {
                        act("You wear $p around your right wrist.", ch, obj, NULL, TO_CHAR);
                        act("$n wears $p around $s right wrist.", ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_WRIST_R);
                        return;
                }

                bug("Wear_obj: no free wrist.", 0);
                send_to_char("You already wear two wrist items.\n\r", ch);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WEAR_SHIELD, BIT_WEAR_SHIELD))
        {
                if (!remove_obj(ch, WEAR_DUAL, fReplace))
                        return;
                if (!remove_obj(ch, WEAR_SHIELD, fReplace))
                        return;
                act("$n wears $p as a shield.", ch, obj, NULL, TO_ROOM);
                act("You wear $p as a shield.", ch, obj, NULL, TO_CHAR);
                equip_char(ch, obj, WEAR_SHIELD);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_WIELD, BIT_WIELD))
        {
                if (obj->item_type != ITEM_WEAPON)
                {
                        send_to_char("You cannot use that as a weapon.\n\r", ch);
                        return;
                }

                if (IS_NPC(ch) || (!IS_NPC(ch) && CAN_DO(ch, gsn_dual)))
                {
                        /*
                         * If you think this looks ugly now, just imagine how
                         * I felt trying to write it!  --- Thelonius (Monk)
                         * Now, it will attempt to wear in 1 if open, then 2 if
                         * open, else replace 2, else replace 1.
                         */

                        if (IS_SET(obj->extra_flags, ITEM_POISONED) && !can_use_poison_weapon(ch))
                        {
                                send_to_char("That's an evil weapon, you can't wield that!\n\r", ch);
                                return;
                        }

                        if (IS_SET(obj->extra_flags, ITEM_BOW) || IS_SET(obj->extra_flags, ITEM_LANCE))
                        {
                                if (IS_SET(obj->extra_flags, ITEM_BOW)
                                    && !(ch->class == CLASS_RANGER))
                                {
                                        send_to_char("You cannot use bows.\n\r", ch);
                                        return;
                                }

                                if (IS_SET(obj->extra_flags, ITEM_LANCE)
                                    && ch->sub_class != SUB_CLASS_KNIGHT)
                                {
                                        send_to_char("You cannot use lances.\n\r", ch);
                                        return;
                                }

                                if (get_eq_char(ch, WEAR_RANGED_WEAPON))
                                {
                                        if (!remove_obj(ch, WEAR_RANGED_WEAPON, fReplace))
                                                return;
                                }

                                act("You ready $p.", ch, obj, NULL, TO_CHAR);
                                act("$n readies $p.", ch, obj, NULL, TO_ROOM);
                                equip_char(ch, obj, WEAR_RANGED_WEAPON);
                                return;
                        }

                        if (get_eq_char(ch, WEAR_WIELD) &&
                            ((get_eq_char(ch, WEAR_DUAL) && !remove_obj(ch, WEAR_DUAL, fReplace))
                             || !remove_obj(ch, WEAR_SHIELD,  fReplace))
                            && !remove_obj(ch, WEAR_WIELD, fReplace))
                                return;

                        if (!get_eq_char(ch, WEAR_WIELD))
                        {
                                OBJ_DATA *other_weapon;
                                int       weight = 0;

                                if ((other_weapon = get_eq_char(ch, WEAR_DUAL)))
                                        weight = get_obj_weight(other_weapon);

                                if (weight + get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
                                {
                                        send_to_char("It is too heavy for you to wield.\n\r", ch);
                                        return;
                                }

                                act("You wield $p.", ch, obj, NULL, TO_CHAR);
                                act("$n wields $p.", ch, obj, NULL, TO_ROOM);
                                equip_char(ch, obj, WEAR_WIELD);
                                return;
                        }

                        if (!get_eq_char(ch, WEAR_DUAL))
                        {
                                OBJ_DATA *primary_weapon;
                                int       weight;

                                primary_weapon = get_eq_char(ch, WEAR_WIELD);
                                weight = get_obj_weight(primary_weapon);

                                if (weight + get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
                                {
                                        send_to_char("It is too heavy for you to wield.\n\r", ch);
                                        return;
                                }

                                act("You dual wield $p.", ch, obj, NULL, TO_CHAR);
                                act("$n dual wields $p.", ch, obj, NULL, TO_ROOM);
                                equip_char(ch, obj, WEAR_DUAL);
                                return;
                        }

                        bug("Wear_obj: no free weapon slot.", 0);
                        send_to_char("You already wield two weapons.\n\r", ch);
                        return;
                }
                else    /* can only wield one weapon */
                {
                        if ((IS_SET(obj->extra_flags, ITEM_BOW))
                            || (IS_SET(obj->extra_flags,ITEM_LANCE)))
                        {
                                if (IS_SET(obj->extra_flags, ITEM_BOW)
                                    && !(ch->class == CLASS_RANGER))
                                {
                                        send_to_char("You cannot use bows.\n\r", ch);
                                        return;
                                }

                                if (IS_SET(obj->extra_flags, ITEM_LANCE)
                                    && ch->sub_class != SUB_CLASS_KNIGHT)
                                {
                                        send_to_char("You cannot use lances.\n\r", ch);
                                        return;
                                }

                                if (get_eq_char(ch, WEAR_RANGED_WEAPON))
                                {
                                        if (!remove_obj(ch, WEAR_RANGED_WEAPON, fReplace))
                                                return;
                                }

                                act("You ready $p.", ch, obj, NULL, TO_CHAR);
                                act("$n readies $p.", ch, obj, NULL, TO_ROOM);
                                equip_char(ch, obj, WEAR_RANGED_WEAPON);
                                return;
                        }

                        if (!remove_obj(ch, WEAR_WIELD, fReplace))
                                return;

                        if (IS_SET(obj->extra_flags, ITEM_POISONED)
                            && (!can_use_poison_weapon(ch)))
                        {
                                send_to_char("That's an evil weapon, you can't wield that!\n\r", ch);
                                return;
                        }

                        if (get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
                        {
                                send_to_char("It is too heavy for you to wield.\n\r", ch);
                                return;
                        }

                        act("You wield $p.", ch, obj, NULL, TO_CHAR);
                        act("$n wields $p.", ch, obj, NULL, TO_ROOM);
                        equip_char(ch, obj, WEAR_WIELD);
                        return;
                }
        }

        if (((obj->item_type == ITEM_WAND || obj->item_type == ITEM_STAFF || obj->item_type == ITEM_PIPE)
             && (!ch->form || form_wear_table[ch->form].can_wear[WEAR_HOLD]))
            || CAN_WEAR(eff_class, ch->form, obj, ITEM_HOLD, BIT_HOLD))
        {
                if (!remove_obj(ch, WEAR_HOLD, fReplace))
                        return;
                act("You hold $p in your hands.", ch, obj, NULL, TO_CHAR);
                act("$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_HOLD);
                return;
        }

        if (CAN_WEAR(eff_class, ch->form, obj, ITEM_FLOAT, BIT_FLOAT))
        {
                if (!remove_obj(ch, WEAR_FLOAT, fReplace))
                        return;
                act("You release $p and it starts orbiting your head.",
                    ch, obj, NULL, TO_CHAR);
                act("$n releases $p, which slowly orbits $s head.",
                    ch, obj, NULL, TO_ROOM);
                equip_char(ch, obj, WEAR_FLOAT);
                      return;
        }

        if ((obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF && obj->item_type != ITEM_PIPE )
            && !wear_table[eff_class].can_wear[obj->item_type])
        {
                if (ch->sub_class)
                        send_to_char("Your sub class cannot use that type of item.\n\r", ch);
                else
                        send_to_char("Your class cannot use that type of item.\n\r", ch);
                return;
        }

        /* deny message for form restrictions or class on location */
        {
                int i, j;
                for (i = 1, j = 1; i <= ITEM_BIT_COUNT; i++, j *= 2)
                {
                        if (IS_SET(obj->wear_flags, j))
                        {
                                if (!IS_NPC(ch) && !IS_IMMORTAL(ch)){
                                    if (!loc_wear_table[eff_class].can_wear[i])
                                    {
                                            send_to_char("Your profession prohibits wearing anything in that location.\n\r", ch);
                                            return;
                                    }

                                    if (!form_wear_table[ch->form].can_wear[i])
                                    {
                                            send_to_char("You cannot wear that in your current form.\n\r", ch);
                                            return;
                                    }
                                }
                        }
                }
        }

        if (fReplace)
                send_to_char("You can't wear, wield, or hold that.\n\r", ch);
}


void do_wear (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Wear, wield, or hold what?\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (!str_cmp(arg, "all"))
        {
                OBJ_DATA *obj_next;

                if (ch->fighting)
                {
                        send_to_char("Not while fighting.\n\r", ch);
                        return;
                }

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
                                wear_obj(ch, obj, FALSE);
                }
                return;
        }
        else
        {
                if (!(obj = get_obj_carry(ch, arg)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                if (ch->fighting && !(obj->item_type == ITEM_WEAPON))
                {
                        send_to_char("Not while fighting.\n\r", ch);
                        return;
                }

                wear_obj(ch, obj, TRUE);
        }

        return;
}


void do_remove (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Remove what?\n\r", ch);
                return;
        }

        /* No removing in combat - Shade Sept 99 */
        if (ch->fighting)
        {
                send_to_char("You're a bit busy for that!\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        /* For remove all command (hopefully) - Brutus */

        if (!str_cmp (arg, "all"))
        {
                OBJ_DATA *obj_next;

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if (obj->wear_loc != WEAR_NONE)
                                remove_obj(ch, obj->wear_loc, TRUE);
                }

                return;
        }
        else
        {
                if (!(obj = get_obj_wear(ch, arg)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }
        }

        remove_obj(ch, obj->wear_loc, TRUE);
}


/*
 * clan stuff - Brutus
 */

void do_leav (CHAR_DATA *ch, char *argument)
{
        send_to_char("You must type the full command to leave the clan.\n\r",ch);
        return;
}


void do_leave (CHAR_DATA *ch, char *argument)
{
        char buf[ MAX_STRING_LENGTH ];

        if (IS_NPC(ch))
                return;

        if (!ch->clan)
        {
                send_to_char("You must join a clan in order to leave.\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("You may not leave your clan while fighting.\n\r", ch);
                return;
        }

        if (ch->pcdata->confirm_leave)
        {
                if (argument[0] != '\0')
                {
                        send_to_char("Leave status removed.\n\r",ch);
                        ch->pcdata->confirm_leave = FALSE;
                        return;
                }
                else
                {
                        send_to_char("You are no longer a clan member!\n\r",ch);

                        sprintf(buf, "%s is no longer a member of clan %s.\n\r",
                                ch->name, clan_table[ch->clan].who_name);
                        do_info(ch, buf);

                        player_leaves_clan(ch);
                        return;
                }
        }

        if (argument[0] != '\0')
        {
                send_to_char("Just type leave. No argument.\n\r",ch);
                return;
        }

        send_paragraph_to_char("{RWarning!{x  You are about to leave your clan!  Once you leave "
                               "you will not be able to rejoin a clan unless an Immortal removes "
                               "your 'deserter' status.  You will not automatically become RONIN "
                               "if you leave your clan either: an Immortal must set your "
                               "ronin status.\n\r\n\rType LEAVE again to confirm this command.  "
                               "Type LEAVE with an argument to cancel leave status and reapply "
                               "this warning message.\n\r", ch, 0);

        ch->pcdata->confirm_leave = TRUE;
}


void do_bounty (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int amnt;

        const int max_bounty = 1000000;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (!(victim = get_char_world(ch, arg1)))
        {
                send_to_char("Put a bounty on whose head?\n\r", ch);
                return;
        }

        if (ch == victim)
        {
                send_to_char("Your bravery is noted, but your stupidity far exceeds it.\n\r", ch);
                return;
        }

        if (is_number(arg2))
                amnt = atoi(arg2);
        else
        {
                send_to_char("What amount of coin for the reward was that?\n\r",ch);
                return;
        }

        if (IS_NPC(victim))
        {
                send_to_char("Rewards may only be offered for the death of Players.\n\r",ch);
                return;
        }

        if (!ch->clan)
        {
                send_to_char("You must join a clan in order to offer bounties.\n\r", ch);
                return;
        }

        if (!victim->clan)
        {
                send_to_char("You can only offer bounties on other clan members.\n\r",ch);
                return;
        }

        if (ch->clan == victim->clan)
        {
                send_to_char("Traitors are reviled, why offer a reward for the scalp of your own brethren?\n\r", ch);
                return;
        }

        /*
        if (victim->pcdata->bounty > 0)
        {
                sprintf(buf, "There is already a bounty on %s's head.\n\r", victim->name);
                send_to_char(buf, ch);
                return;
        }
         */

        if (victim->level < 20)
        {
                send_to_char("You must wait until they are at least level 20.\n\r", ch);
                return;
        }

        if (victim->level > LEVEL_HERO)
        {
                send_to_char("Yeah right.\n\r", ch);
                return;
        }

        if (amnt < 100000)
        {
                send_to_char("Bounty-hunting is a highly-paid profession; murder comes at a higher price than that...\n\r",ch);
                return;
        }

        if ( total_coins_char(ch) < amnt)
        {
                send_to_char("You do not have enough money for the reward you are trying to post.\n\r", ch);
                return;
        }

        if (victim->pcdata->bounty + amnt > max_bounty)
        {
                send_to_char("You can't add that many more coins to the reward, sorry.\n\r", ch);
                return;
        }

        if (victim->pcdata->bounty)
        {
                sprintf(buf, "You increase the reward for %s's murder!\n\r", victim->name);
                send_to_char(buf, ch);
                sprintf(buf, "The reward for %s's murder has been increased by %d platinum coins!",
                        victim->name, amnt/1000);
                do_info(ch, buf);
        }
        else
        {
                sprintf(buf, "You offer a reward for %s's scalp!\n\r", victim->name);
                send_to_char(buf, ch);
                sprintf(buf, "A reward of %d platinum coins has been posted for the MURDER of %s!\n\r",
                        amnt/1000, victim->name);
                do_info(ch, buf);
        }

        coins_from_char(amnt, ch);
        victim->pcdata->bounty += amnt;
        SET_BIT(victim->status, PLR_HUNTED);
        update_wanted_table(victim);
        save_char_obj(victim);
}


void do_guild (CHAR_DATA *ch, char *argument)
{
        /*
         *  Tidied and extended by Gezhp 2000
         */

        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];

        CHAR_DATA *victim;
        int clan;

        if (IS_NPC(ch))
                return;

        if (!IS_SET(ch->act, PLR_LEADER) && ch->level <= LEVEL_HERO)
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0')
        {
                send_to_char("Syntax: guild <character> [clan|remove]\n\r",ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg1)))
        {
                send_to_char("That player is not in this room.\n\r", ch);
                return;
        }

        if (IS_NPC(victim))
                return;

        /*
         *  Add to clan
         */

        if (str_cmp(arg2, "none") && str_cmp(arg2, "remove"))
        {
                if (ch != victim && ch->clan && victim->clan == ch->clan
                    && ch->level <= LEVEL_HERO)
                {
                        send_to_char("They are already a member of your clan!\n\r", ch);
                        return;
                }

                if (victim->level > 40 && ch->level < L_JUN)
                {
                        send_to_char("That person is too high level to be clanned.\n\r", ch);
                        return;
                }

                if (victim->level < 20)
                {
                        send_to_char("You cannot guild people below level 20.\n\r", ch);
                        return;
                }

                if (victim->clan && victim->clan != ch->clan && ch->level <= LEVEL_HERO)
                {
                        send_to_char ("You cannot guild a member of another clan.\n\r", ch);
                        return;
                }

                if (arg2[0] == '\0' && ch->clan)
                        clan = ch->clan;
                else
                {
                        if ((clan = clan_lookup(arg2)) == -1)
                        {
                                send_to_char("No such clan exists.\n\r", ch);
                                return;
                        }
                        if (clan != ch->clan && ch->level <= LEVEL_HERO)
                        {
                                send_to_char("You must add people to YOUR clan!\n\r", ch);
                                return;
                        }
                }

                if (IS_SET(victim->status, PLR_LEFT_CLAN) && ch->level <= LEVEL_HERO)
                {
                        sprintf(buf, "%s is a clan deserter and may not be guilded by a mortal!\n\r",
                                victim->name);
                        send_to_char(buf, ch);
                        return;
                }

                if (clan_table[clan].independent)
                {
                        if (ch != victim)
                        {
                                sprintf(buf, "They are now a %s.\n\r", clan_table[clan].name);
                                send_to_char(buf,ch);
                        }
                        sprintf (buf, "You are now a %s.\n\r", clan_table[clan].name);
                        send_to_char(buf,victim);
                }
                else
                {
                        if (ch != victim)
                        {
                                sprintf(buf, "They are now a member of clan %s.\n\r",
                                        capitalize_initial(clan_table[clan].who_name));
                                send_to_char(buf,ch);
                        }

                        sprintf(buf, "%s is now a member of clan %s.\n\r",
                                victim->name,
                                capitalize_initial(clan_table[clan].who_name));
                        do_info(victim, buf);

                        sprintf(buf, "You are now a member of clan %s.\n\r",
                                capitalize_initial(clan_table[clan].who_name));
                        send_to_char(buf,victim);

                        if (IS_SET(victim->status, PLR_RONIN))
                        {
                                REMOVE_BIT(victim->status, PLR_RONIN);
                                send_to_char("You are no longer considered RONIN.\n\r", victim);
                                if (ch != victim)
                                        send_to_char("They are no longer considered RONIN.\n\r", ch);
                        }
                }

                victim->clan = clan;
                victim->clan_level = 0;
                REMOVE_BIT(victim->act, PLR_LEADER);
                REMOVE_BIT(victim->act, PLR_GUIDE);
                victim->pcdata->recall_points[CLAN_RECALL] = clan_table[clan].hall;
                destroy_clan_items(victim);
                update_wanted_table(victim);

                return;
        }

        /*
         *  Remove from clan
         */
        else
        {
                if (!victim->clan)
                {
                        send_to_char("That player is not in a clan.\n\r", ch);
                        return;
                }

                if (ch == victim && ch->level <= LEVEL_HERO)
                {
                        send_to_char("You must LEAVE your clan.\n\r", ch);
                        return;
                }

                if (ch->clan != victim->clan && ch->level <= LEVEL_HERO)
                {
                        send_to_char("You may only remove members of your own clan.\n\r", ch);
                        return;
                }

                if (IS_SET(victim->act, PLR_LEADER) && ch->level <= LEVEL_HERO)
                {
                        send_to_char("You may not remove other leaders.\n\r", ch);
                        return;
                }

                sprintf(buf, "You are no longer a member of clan %s.\n\r",
                        clan_table[victim->clan].who_name);
                send_to_char(buf, victim);

                if (victim != ch)
                        send_to_char("They are now clanless.\n\r",ch);

                sprintf(buf, "%s is no longer a member of clan %s.\n\r",
                        victim->name, clan_table[victim->clan].who_name);
                do_info(victim, buf);

                player_leaves_clan(victim);
                return;
        }
}


void do_sacrifice (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (arg[0] == '\0' || !str_cmp(arg, ch->name))
        {
                send_to_char("God appreciates your offer and may accept it later.\n\r", ch);
                act("$n offers $mself to God, who graciously declines.",
                    ch, NULL, NULL, TO_ROOM);
                return;
        }

        obj = get_obj_list(ch, arg, ch->in_room->contents);

        if (!obj)
        {
                send_to_char("You can't find it.\n\r", ch);
                return;
        }

        if ((!IS_SET(obj->wear_flags, ITEM_TAKE) && obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_REMAINS )
            || obj->item_type == ITEM_CLAN_OBJECT
            || obj->item_type == ITEM_PORTAL
            || obj->item_type == ITEM_FOUNTAIN
            || obj->item_type == ITEM_ANVIL
            || obj->item_type == ITEM_CORPSE_PC
            || !is_obj_owner(obj, ch))
        {
                send_to_char( "You can't sacrifice that.\n\r",ch);
                return;
        }

        act("$n sacrifices $p to $s god.", ch, obj, NULL, TO_ROOM);
        act("You sacrifice $p to $D.", ch, obj, NULL, TO_CHAR);

        if (obj->item_type == ITEM_CORPSE_NPC
            && !(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                 || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE)))
        {
                int level_dif = obj->level - ch->level;

                if (level_dif > 5)
                {
                        send_to_char("Your god gives you one gold coin for your sacrifice.\n\r", ch);
                        ch->gold++;
                }
                else if (level_dif > -6)
                {
                        send_to_char("Your god gives you one silver coin for your sacrifice.\n\r", ch);
                        ch->silver++;
                }
                else
                {
                        send_to_char("Your god gives you one copper coin for your sacrifice.\n\r", ch);
                        ch->copper++;
                }

                if (ch->pcdata->deity_patron > -1)
                {
                        if (level_dif < -9)
                        {
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_JUDGEMENTAL, 1);
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_PROUD, 1);
                        }
                        else if (level_dif > 5)
                        {
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_JUDGEMENTAL, -3);
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_PROUD, -3);
                        }
                        else
                                SET_DEITY_PERSONALITY_TIMER(ch, DEITY_PERSONALITY_JUDGEMENTAL, -1);

                        SET_DEITY_TYPE_TIMER(ch, DEITY_TYPE_DEATH, -2);
                }
        }

        extract_obj(obj);
}


void do_smear (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pouch;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Smear what?\n\r", ch);
                return;
        }

        /* Pouch code follows... */

        pouch = get_eq_char(ch, WEAR_POUCH);

        if ( ch->position == POS_FIGHTING
        &&  !IS_NPC(ch) )
        {
                if (!pouch)
                {
                        send_to_char("You are not wearing a pouch to get a paint from!\n\r", ch);
                        return;
                }
                else
                {
                        /*
                        if (!check_blind(ch))
                                return;
                        */

                        if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that paint in your pouch.\n\r", ch);
                                return;
                        }
                }
        }
        else
        {
                if (!(obj = get_obj_carry(ch, arg))
                &&  !IS_NPC(ch) )
                {
                        if (!pouch)
                        {
                                send_to_char("You do not have that paint on your person.\n\r", ch);
                                return;
                        }
                        else if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that paint on you or in your pouch.\n\r", ch);
                                return;
                        }
                }
        }

        if ( ch->position == POS_FIGHTING
        &&   IS_NPC(ch) )
        {
                /* Mobs can only smear paints they are holding during combat -- Owl 1/7/22 */
                if (!(obj = get_eq_char(ch, WEAR_HOLD)))
                {
                        send_to_char("You can only smear paints you are holding during combat!\n\r", ch);
                        return;
                }

        }

        if (obj->item_type != ITEM_PAINT)
        {
                send_to_char("You can only smear with paints and pigments.\n\r", ch);
                return;
        }

        act("You paint yourself with $p.", ch, obj, NULL ,TO_CHAR);
        act("$n paints $mself with $p.", ch, obj, NULL, TO_ROOM);

        if (obj->level > ch->level)
                act("$p is too high level for you.", ch, obj, NULL, TO_CHAR);
        else
        {
                obj->level = obj->value[0];
                obj_cast_spell(obj->value[1], obj->level, ch, ch, NULL);
                obj_cast_spell(obj->value[2], obj->level, ch, ch, NULL);
                obj_cast_spell(obj->value[3], obj->level, ch, ch, NULL);
        }

        extract_obj(obj);
        return;
}


void do_quaff (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pouch;
        char      arg [ MAX_INPUT_LENGTH ];

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Quaff what?\n\r", ch);
                return;
        }

        pouch = get_eq_char(ch, WEAR_POUCH);

        if ( ch->position == POS_FIGHTING && !IS_NPC(ch))
        {

                if (!pouch)
                {
                        send_to_char("You are not wearing a pouch to get a potion from!\n\r", ch);
                        return;
                }
                else
                {
                        if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that potion in your pouch.\n\r", ch);
                                return;
                        }
                }
        }
        else
        {
                if (!(obj = get_obj_carry(ch, arg))
                &&  !IS_NPC(ch))
                {
                        if (!pouch)
                        {
                                send_to_char("You do not have that potion on your person.\n\r", ch);
                                return;
                        }
                        else if (!(obj = get_obj_list(ch, arg, pouch->contains )))
                        {
                                send_to_char("You do not have that potion on you or in your pouch.\n\r", ch);
                                return;
                        }
                }
        }

        if ( ch->position == POS_FIGHTING
        &&   IS_NPC(ch) )
        {
                /* Mobs can only quaff potions they are holding during combat -- Owl 1/7/22 */
                if (!(obj = get_eq_char(ch, WEAR_HOLD)))
                {
                        send_to_char("You can only quaff potions you are holding during combat!\n\r", ch);
                        return;
                }

        }

        if (obj->item_type != ITEM_POTION)
        {
                send_to_char("You can quaff only potions.\n\r", ch);
                return;
        }

        if (tournament_action_illegal(ch, TOURNAMENT_OPTION_NOQUAFF))
        {
                send_to_char("You can't quaff potions in this tournament!\n\r", ch);
                return;
        }

        act("You quaff $p.", ch, obj, NULL ,TO_CHAR);
        act("$n quaffs $p.", ch, obj, NULL, TO_ROOM);

        if (obj->level > ch->level)
                act("$p is too high level for you.", ch, obj, NULL, TO_CHAR);
        else
        {
                obj->level = obj->value[0];
                obj_cast_spell(obj->value[1], obj->level, ch, ch, NULL);
                obj_cast_spell(obj->value[2], obj->level, ch, ch, NULL);
                obj_cast_spell(obj->value[3], obj->level, ch, ch, NULL);
        }

        if (ch->fighting)
                WAIT_STATE(ch, PULSE_VIOLENCE/2);

        extract_obj(obj);
        return;
}


void do_recite (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *scroll;
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       arg1 [ MAX_INPUT_LENGTH ];
        char       arg2 [ MAX_INPUT_LENGTH ];

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (!(scroll = get_eq_char(ch, WEAR_HOLD)))
        {
                send_to_char("You hold nothing in your hand.\n\r", ch);
                return;
        }

        if (scroll->item_type != ITEM_SCROLL)
        {
                send_to_char("You can recite only scrolls.\n\r", ch);
                return;
        }

        obj = NULL;
        if (arg2[0] == '\0')
        {
                victim = ch;
        }
        else
        {
                if (!(victim = get_char_room (ch, arg2))
                    && !(obj  = get_obj_here  (ch, arg2)))
                {
                        send_to_char("You can't find it.\n\r", ch);
                        return;
                }
        }

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        act("You recite $p.", ch, scroll, NULL, TO_CHAR);
        act("$n recites $p.", ch, scroll, NULL, TO_ROOM);

        if (scroll->level > ch->level)
                act("$p is too high level for you.", ch, scroll, NULL, TO_CHAR);
        else
        {
                scroll->level = scroll->value[0];
                obj_cast_spell(scroll->value[1], scroll->level, ch, victim, obj);
                obj_cast_spell(scroll->value[2], scroll->level, ch, victim, obj);
                obj_cast_spell(scroll->value[3], scroll->level, ch, victim, obj);
        }

        extract_obj(scroll);
        return;
}


void do_brandish (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *staff;
        CHAR_DATA *vch;
        int sn;
        int eff_class = 0;

        if (!IS_NPC(ch))
        {
                eff_class = ch->class;
                if (ch->sub_class)
                        eff_class = MAX_CLASS - 1 + ch->sub_class;
        }

        if (!(staff = get_eq_char(ch, WEAR_HOLD)))
        {
                send_to_char("You hold nothing in your hand.\n\r", ch);
                return;
        }

        if (staff->item_type != ITEM_STAFF)
        {
                send_to_char("You can brandish only with a staff.\n\r", ch);
                return;
        }

        if (tournament_action_illegal(ch, TOURNAMENT_OPTION_NOBRANDISH))
        {
                send_to_char("You may not brandish during this tournament!\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_CONE_OF_SILENCE))
        {
                send_to_char("An anti-magic field prevents any casting here.\n\r",ch);
                act("$n's attempt to brandish their staff fizzles out.",
                    ch, NULL, NULL, TO_ROOM);
                return;
        }

        if  (!IS_NPC(ch) && !wear_table[eff_class].can_wear[staff->item_type])
        {
                send_to_char("You shake the staff, twirl it around, and shake it again.\n\r", ch);
                send_to_char("You succeed only in stubbing your big toe.\n\r",ch);
                return;
        }

        if ((sn = staff->value[3]) < 0
            || sn >= MAX_SKILL
            || skill_table[sn].spell_fun == 0)
        {
                bug("Do_brandish: bad sn %d.", sn);
                return;
        }

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        if (staff->value[2] > 0)
        {
                CHAR_DATA *vch_next;

                act("You brandish $p.",  ch, staff, NULL, TO_CHAR);
                act("$n brandishes $p.", ch, staff, NULL, TO_ROOM);

                for (vch = ch->in_room->people; vch; vch = vch_next)
                {
                        vch_next = vch->next_in_room;

                        if (vch->deleted || !is_same_group(ch, vch))
                                continue;

                        switch (skill_table[sn].target)
                        {
                            default:
                                bug("Do_brandish: bad target for sn %d.", sn);
                                return;

                            case TAR_IGNORE:
                                if (vch != ch)
                                        continue;
                                break;

                            case TAR_CHAR_OFFENSIVE:
                                if (IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch))
                                        continue;
                                break;

                            case TAR_CHAR_DEFENSIVE:
                                if (IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch))
                                        continue;
                                break;

                            case TAR_CHAR_SELF:
                                if (vch != ch)
                                        continue;
                                break;
                        }

                        obj_cast_spell(staff->value[3], staff->value[0], ch, vch, NULL);
                }
        }

        if (--staff->value[2] <= 0)
        {
                act("$p blazes bright and is gone.", ch, staff, NULL, TO_CHAR);
                act("$p blazes bright and is gone.", ch, staff, NULL, TO_ROOM);
                extract_obj(staff);
        }

        return;
}

void do_smoke (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *pipe;
        OBJ_DATA *smokeable;
        char      arg [ MAX_INPUT_LENGTH ];
        int       base_thirst_inc   = 4;
        int       r_5;
        int       r_20;
        int       benefit_reduction;
        int       power_level;
        /* char      buf [ MAX_STRING_LENGTH ]; */

        one_argument(argument, arg);

        if ( ( ch->in_room->sector_type == SECT_UNDERWATER )
        ||   ( ch->in_room->sector_type == SECT_UNDERWATER_GROUND ) )
        {
                send_to_char( "You can't smoke a pipe underwater.\n\r", ch );
                return;
        }

        if (ch->position < POS_RESTING)
        {
                send_to_char("You're really in no state to do that.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Smoke what?\n\r", ch);
                return;
        }

        if (!(pipe = get_eq_char(ch, WEAR_HOLD)))
        {
                send_to_char("You need to be holding a pipe to smoke.\n\r", ch);
                return;
        }

        if (pipe->item_type != ITEM_PIPE)
        {
                send_to_char("What were you planning to smoke <558>with<559>, exactly?\n\r", ch);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char( "No way! You are fighting.\n\r", ch );
                return;
        }

        if (!(smokeable = get_obj_carry(ch, arg)))
        {
            send_to_char("You don't have any of that handy.\n\r", ch);
            return;
        }

        if (smokeable->item_type != ITEM_SMOKEABLE)
        {
                send_to_char("You can't smoke that, you goofball!\n\r", ch);
                return;
        }

        if (pipe->value[0] == 0)
        {
                send_to_char("Your pipe is completely blocked by resin.  It needs a clean.\n\r", ch);
                return;
        }

        /* Okay, smoking time.  Apply the effects of the smokeable substance, modified by the pipe. Decrease
        the pipe's benefit % and remove a use from the smokeable substance--destroying ("extracting") it if final use */

        if (pipe->level > ch->level)
        {
                act("The pipe is too sophisticated for you to use effectively.", ch, smokeable, NULL, TO_CHAR);
                return;
        }

        if (smokeable->level > ch->level)
                act("You're not ready to try smoking $p just yet.", ch, smokeable, NULL, TO_CHAR);
        else
        {
                act("You pack your pipe and smoke some {W$p{x.", ch, smokeable, NULL, TO_CHAR);
                act("$n packs $s pipe and smokes some $p.", ch, smokeable, NULL, TO_ROOM);

                /* Some random 'colour' effects */

                r_20 = rand() % 20 + 1;

                switch ( r_20 )
                {
                    default:
                        break;

                    case 11:
                        act("<111>You blow a perfect smoke ring, which wafts away and slowly loses its shape.<0>", ch, smokeable, NULL, TO_CHAR);
                        act("<111>$n blows a perfect smoke ring, which wafts away and slowly loses its shape.<0>", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 12:
                        act("Delightful.  Some of the best $p you've ever smoked.", ch, smokeable, NULL, TO_CHAR);
                        act("$n smiles contently as $e puffs on $s pipe.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 13:
                        act("As you inhale, the rich, smoky flavour of the $p fills your mouth.  You sigh.", ch, smokeable, NULL, TO_CHAR);
                        act("$n sighs contentedly as $e smokes $s pipe.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 14:
                        act("You savour the complex flavours swirling in your mouth with each puff.", ch, smokeable, NULL, TO_CHAR);
                        act("$n smiles, savouring the flavours of $s $p as he smokes it.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 15:
                        act("The ritual of tamping and relighting the $p is meditative and grounding.", ch, smokeable, NULL, TO_CHAR);
                        act("$n looks relaxed and content as $e tamps and relights $s pipe.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 16:
                        act("As the bowl of the pipe glows, you are drawn into the moment; the world around you fades.", ch, smokeable, NULL, TO_CHAR);
                        act("$n stares into the distance as $e smokes, seeming barely aware of your presence.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 17:
                        act("You hold the pipe with an air of elegance, puffing thoughtfully.", ch, smokeable, NULL, TO_CHAR);
                        act("$n holds $s pipe with an air of elegance, puffing thoughtfully.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 18:
                        act("You expertly manipulate the pipe, tamping the $p with practised movements.", ch, smokeable, NULL, TO_CHAR);
                        act("$n expertly manipulates $s pipe, tamping the $p with practised movements.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 19:
                        act("The warmth of the pipe in your hand is comforting and familiar.", ch, smokeable, NULL, TO_CHAR);
                        act("The rich scent of $n's pipe fills the space, distinct and aromatic.", ch, smokeable, NULL, TO_ROOM);
                        break;

                    case 20:
                        act("As you finish you gently knock out the ash, ending your relaxing session.", ch, smokeable, NULL, TO_CHAR);
                        act("$n gently knocks the ash out of $s pipe as $e finishes smoking.", ch, smokeable, NULL, TO_ROOM);
                        break;
                }

                power_level = ( ( ( (ch->level * 100) / 2 ) * ( pipe->value[0] * 100 ) ) / 1000000 );

                if (power_level <= 0 )
                {
                    /* We know by here that the pipe benefit is at least 1% and ch-> level is at least 1, so this is legit */
                    power_level = 1;
                }

                obj_cast_spell(smokeable->value[1], power_level, ch, ch, smokeable);
                obj_cast_spell(smokeable->value[2], power_level, ch, ch, smokeable);
                obj_cast_spell(smokeable->value[3], power_level, ch, ch, smokeable);
        }

        /* apply thirst */

        if (ch->level < LEVEL_HERO )
        {
            ch->pcdata->condition[COND_THIRST] -= ( ( pipe->value[2]/100 ) * base_thirst_inc );
        }

        /* pipe benefit reduction  (1-5% randomly of max_benefit (pipe->value[1]) )*/

        r_5 = rand() % 5 + 1;

        benefit_reduction = rintf( ( (float)pipe->value[1] * (float)100 ) * (float)r_5 / (float)10000 );

        pipe->value[0] = (pipe->value[0] - benefit_reduction);

        if (pipe->value[0] < 0)
        {
            send_to_char("When you finish smoking, you notice your pipe is completely blocked by resin.\n\r", ch);
            pipe->value[0] = 0;
        }

        /* smokeable quantity reduction */

        smokeable->value[0] -= 1;

        if (smokeable->value[0] <= 0)
        {
            act("You smoke the last of the $p.", ch, smokeable, NULL, TO_CHAR);
            extract_obj(smokeable);
        }

        if (ch->level < LEVEL_IMMORTAL)
        {
            WAIT_STATE(ch, pipe->value[3]);
        }

        return;
}

/* clean <pipe keyword> <pipe cleaner keyword> */

void do_clean (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *cleaner;
        OBJ_DATA *pipe;
        char      arg1 [ MAX_INPUT_LENGTH ];
        char      arg2 [ MAX_INPUT_LENGTH ];
        int       r_2;
        int       r_10_20;
        int       clean_reduction;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0')
        {
                send_to_char("Clean which pipe?\n\r", ch);
                return;
        }

        if (arg2[0] == '\0')
        {
                send_to_char("Clean your pipe with what?\n\r", ch);
                return;
        }

        if (ch->position < POS_RESTING)
        {
                send_to_char("You're really in no state to do that.\n\r", ch);
                return;
        }

        if (!(pipe = get_eq_char(ch, WEAR_HOLD)))
        {
                send_to_char("You need to be holding a pipe to clean it.\n\r", ch);
                return;
        }

        if (pipe->item_type != ITEM_PIPE)
        {
                send_to_char("That's not a pipe you're holding, unfortunately.\n\r", ch);
                return;
        }

        if (ch->position == POS_FIGHTING)
        {
                send_to_char( "No way! You are fighting.\n\r", ch );
                return;
        }

        /* need to check inventory for a pipe cleaner */

        if (!(cleaner = get_obj_carry(ch, arg2)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (cleaner->item_type != ITEM_PIPE_CLEANER)
        {
                send_to_char("That's not a pipe cleaner.\n\r", ch);
                return;
        }

        /* increase pipe benefit by up to pipe_cleaner->value[2] */

        if ( cleaner->value[0] == 0 )
        {
            send_to_char("Your pipe cleaner is in terrible shape.  You'd probably just make the pipe dirtier.\n\r", ch);
            return;
        }

        if ( ( pipe->value[0] < pipe->value[1] )
        &&   ( cleaner->value[0] > 0 ))
        {
            if ( ( pipe->value[0] + cleaner->value[2] ) >= pipe->value[1] )
            {
                pipe->value[0] = pipe->value[1];
                send_to_char("You give your pipe a thorough cleaning--it is as good as new!\n\r", ch);
            }
            else if ( cleaner->value[2] == 0)
            {
                send_to_char("Your pipe cleaner is too dirty to improve your pipe's condition.\n\r", ch);
                cleaner->value[0]--;
                return;
            }
            else
            {
                pipe->value[0] = (pipe->value[0] + cleaner->value[2]);
                send_to_char("You clean your pipe for a while--it is much improved!\n\r", ch);
            }

            /* remove a use from pipe cleaner, 50% chance of decreasing its clean value by 10-20% */

            if ( cleaner->value[0] > 0)
            {
                cleaner->value[0]--;

            }

            if ( cleaner->value[0] == 0)
            {
                send_to_char("Your pipe cleaner looks to have just about had it, though.\n\r", ch);
                return;
            }

            r_2 = rand() % 2 + 1;

            if (r_2 == 2)
            {
                r_10_20 = rand() % 10 + 10;
                clean_reduction = rintf( ( (float)cleaner->value[2] * (float)100 ) * (float)r_10_20 / (float)10000 );

                if ((cleaner->value[2] - clean_reduction ) >= 0)
                {
                    cleaner->value[2] = (cleaner->value[2] - clean_reduction);
                    send_to_char("Your pipe cleaner seems a little the worse for wear.\n\r", ch);
                }
            }

        }
        else {
                send_to_char("Nothing to do here--your pipe is already clean!\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        return;
}



void do_zap(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *wand;
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       arg [ MAX_INPUT_LENGTH ];
        int eff_class = 0;

        if (!IS_NPC(ch))
        {
                eff_class = ch->class;
                if (ch->sub_class)
                        eff_class = MAX_CLASS - 1 + ch->sub_class;
        }

        one_argument(argument, arg);
        if (arg[0] == '\0' && !ch->fighting)
        {
                send_to_char("Zap whom or what?\n\r", ch);
                return;
        }

        if (!(wand = get_eq_char(ch, WEAR_HOLD)))
        {
                send_to_char("You hold nothing in your hand.\n\r", ch);
                return;
        }

        if (wand->item_type != ITEM_WAND)
        {
                send_to_char("You can zap only with a wand.\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_CONE_OF_SILENCE))
        {
                send_to_char("An anti-magic field prevents any casting here.\n\r",ch);
                act("$n's attempt to use a wand fizzles out.", ch, NULL, NULL, TO_ROOM);
                return;
        }

        obj = NULL;

        if (arg[0] == '\0')
        {
                if (ch->fighting)
                        victim = ch->fighting;
                else
                {
                        send_to_char("Zap whom or what?\n\r", ch);
                        return;
                }
        }
        else
        {
                if (!(victim = get_char_room (ch, arg))
                    && !(obj = get_obj_here (ch, arg)))
                {
                        send_to_char("You can't find it.\n\r", ch);
                        return;
                }
        }

        if (!IS_NPC(ch) && !wear_table[eff_class].can_wear[wand->item_type])
        {
                send_to_char("You vigorously shake the wand in different directions.\n\r", ch);
                send_to_char("You succeed only in attracting some small rodents.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

        if (wand->value[2] > 0)
        {
                if (victim)
                {
                        if (ch == victim)
                        {
                                act("You zap yourself with $p.", ch, wand, victim, TO_CHAR);
                                act("$n zaps $mself with $p.", ch, wand, victim, TO_NOTVICT);
                        }
                        else
                        {
                                act("You zap $N with $p.", ch, wand, victim, TO_CHAR);
                                act("$n zaps you with $p!", ch, wand, victim, TO_VICT);
                                act("$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT);
                        }
                }
                else
                {
                        act("You zap $P with $p.", ch, wand, obj, TO_CHAR);
                        act("$n zaps $P with $p.", ch, wand, obj, TO_ROOM);
                }

                obj_cast_spell(wand->value[3], wand->value[0], ch, victim, obj);
        }

        if (--wand->value[2] <= 0)
        {
                act("Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR);
                act("$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM);
                extract_obj(wand);
        }

        return;
}


void do_steal (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *victim;
        char       buf  [ MAX_STRING_LENGTH ];
        char       arg1 [ MAX_INPUT_LENGTH  ];
        char       arg2 [ MAX_INPUT_LENGTH  ];
        int        percent;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                send_to_char("Steal what from whom?\n\r", ch);
                return;
        }

        if (!(victim = get_char_room(ch, arg2)))
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        /* Can't steal off people if they or you are in mist form - Owl 07/10/00 */
        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot rob your victim in their current form.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("You cannot rob your victim in your current form.\n\r", ch);
                return;
        }

        /* mobs can't steal from mobs */
        if (IS_NPC(ch) && IS_NPC(victim))
                return;

        if (victim == ch)
        {
                send_to_char("That's pointless.\n\r", ch);
                return;
        }

        if (!IS_NPC(victim) && !IS_NPC(ch))
        {
                if (IS_SET(victim->in_room->room_flags, ROOM_SAFE)
                    || IS_SET(victim->in_room->area->area_flags, AREA_FLAG_SAFE))
                {
                        send_to_char("Not in a place of Sanctuary.\n\r", ch);
                        return;
                }

                if (IS_SET(victim->in_room->room_flags, ROOM_PLAYER_KILLER))
                {
                        send_to_char("You can't steal within the Arena.\n\r", ch);
                        return;
                }

                if (ch->pcdata->tailing)
                {
                        send_to_char("You can't get close enough without revealing yourself!\n\r", ch);
                        return;
                }

                if ((ch->clan || IS_SET(ch->status, PLR_RONIN))
                    && (victim->clan || IS_SET(victim->status, PLR_RONIN))
                    && !in_pkill_range(ch, victim)) {
                        send_to_char("You may only steal from those within your pkilling range.\n\r", ch);
                        return;
                }
        }

        WAIT_STATE(ch, skill_table[gsn_steal].beats);
        percent  = number_percent() + (IS_AWAKE(victim) ? 5 : -50);

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA) )
            percent /= 2;

        if (ch->level + 15 < victim->level
            || victim->position == POS_FIGHTING
            || (percent > ch->pcdata->learned[gsn_steal]))
        {
                /*
                 * Failure.
                 */
                send_to_char("Oops.\n\r", ch);
                act("$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT);
                act("$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT);
                sprintf(buf, "%s is a bloody thief!", capitalize_initial(ch->name));
                do_shout(victim, buf);

                if (!IS_NPC(ch))
                {
                        if (IS_NPC(victim))
                                multi_hit(victim, ch, TYPE_UNDEFINED);
                        else
                        {
                                sprintf(buf, "%s has stolen from %s",
                                        ch->name, victim->name);
                                log_string(buf);
                                if (!IS_SET(ch->status, PLR_THIEF)
                                    && (ch->clan == 0 || victim->clan == 0))
                                {
                                        SET_BIT(ch->status, PLR_THIEF);
                                        send_to_char("*** You are now a THIEF!! ***\n\r", ch);
                                        ch->pcdata->fame -= 200;
                                        check_fame_table(ch);
                                        save_char_obj(ch);
                                }
                                else if (in_pkill_range(ch, victim)) {
                                        chat_killer(victim, ch);
                                        multi_hit(victim, ch, TYPE_UNDEFINED);
                                }
                        }
                }
                return;
        }

        if (!IS_NPC(victim))
        {
                if (!IS_SET(ch->status, PLR_THIEF) && (ch->clan ==0 || victim->clan == 0))
                {
                        send_to_char("Oops.\n\r", ch);
                        act("$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT);
                        act("$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT);
                        sprintf(buf, "%s is a bloody thief!", capitalize(ch->name));
                        do_shout(victim, buf);

                        SET_BIT(ch->status, PLR_THIEF);
                        send_to_char("*** You are now a THIEF!! ***\n\r", ch);
                        ch->pcdata->fame -= 200;
                        check_fame_table(ch);
                        save_char_obj(ch);
                        return;
                }
        }

        if (!str_prefix(arg1, "coins") )
        {
                int amount;
                amount = (total_coins_char(victim) * number_range(10, 30)) / 50;

                if (amount <= 0)
                {
                        send_to_char("You couldn't get any coins.\n\r", ch);
                        return;
                }

                coins_from_char(amount, victim);
                coins_to_char(amount, ch, COINS_ADD);
                sprintf(buf, "Jackpot!  You have stolen %d coppers worth of coins.\n\r", amount);
                send_to_char(buf, ch);
                return;
        }

        if (!(obj = get_obj_carry(victim, arg1)))
        {
                send_to_char("You can't find it.\n\r", ch);
                return;
        }

        if (!can_drop_obj(ch, obj)
            || IS_SET(obj->extra_flags, ITEM_INVENTORY)
            || obj->level > ch->level)
        {
                send_to_char("You can't pry it away.\n\r", ch);
                return;
        }

        if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
        {
                send_to_char("You have your hands full.\n\r", ch);
                return;
        }

        if ( (ch->carry_weight + ch->coin_weight) + get_obj_weight(obj) > can_carry_w(ch))
        {
                send_to_char("You can't carry that much weight.\n\r", ch);
                return;
        }

        obj_from_char(obj);
        obj_to_char(obj, ch);
        send_to_char("Muhaha! They didn't even notice! You slink away from them, grinning.\n\r", ch);
        return;
}



/*
 * Shopping commands.
 */

CHAR_DATA *find_keeper (CHAR_DATA *ch)
{
        CHAR_DATA *keeper;
        SHOP_DATA *pShop;
        char       buf [ MAX_STRING_LENGTH ];

        pShop = NULL;

        for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
        {
                if (IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop))
                        break;
        }

        if (!pShop || IS_AFFECTED(keeper, AFF_CHARM))
        {
                send_to_char("You can't do that here.\n\r", ch);
                return NULL;
        }

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                do_say(keeper, "I don't trade with folks I can't see.");
                return NULL;
        }

        if (ch->level > LEVEL_HERO)
                return keeper;

        /*
         * Undesirables.
         */
        if (!IS_NPC(ch) && IS_SET(ch->status, PLR_KILLER))
        {
                do_say(keeper, "Killers are not welcome!");
                sprintf(buf, "%s the KILLER is over here!", capitalize_initial(ch->name));
                do_shout(keeper, buf);
                return NULL;
        }

        if (!IS_NPC(ch) && IS_SET(ch->status, PLR_THIEF))
        {
                do_say(keeper, "Thieves are not welcome!");
                sprintf(buf, "%s the THIEF is over here!", capitalize_initial(ch->name));
                do_shout(keeper, buf);
                return NULL;
        }

        /*
         * Shop hours.
         */
        if (time_info.hour < pShop->open_hour)
        {
                do_say(keeper, "Sorry, come back later.");
                return NULL;
        }

        if (time_info.hour > pShop->close_hour)
        {
                do_say(keeper, "Sorry, come back tomorrow.");
                return NULL;
        }

        /*
         * Invisible or hidden people.
         */
        if (!can_see(keeper, ch))
        {
                do_say(keeper, "I don't trade with folks I can't see.");
                return NULL;
        }

        return keeper;
}


int get_cost (CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy)
{
        SHOP_DATA *pShop;
        int        cost;

        if (!obj || !(pShop = keeper->pIndexData->pShop))
                return 0;

        if (fBuy)
                cost = obj->cost * (pShop->profit_buy + 100)  / 100;
        else
        {
                OBJ_DATA *obj2;
                int       itype;

                cost = 0;

                for (itype = 0; itype < MAX_TRADE; itype++)
                {
                        if (obj->item_type == pShop->buy_type[itype])
                        {
                                cost = ( obj->cost * pShop->profit_sell ) / 100;
                                break;
                        }
                }

                for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content)
                {
                        if (obj->pIndexData == obj2->pIndexData)
                                cost /= 2;
                }
        }

        if (obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND)
                cost = cost * obj->value[2] / obj->value[1];

        return cost;
}


void do_buy (CHAR_DATA *ch, char *argument)
{
        char arg [ MAX_INPUT_LENGTH ];

        if (IS_NPC(ch))
                return;

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Buy what?\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP))
        {
                CHAR_DATA       *pet;
                ROOM_INDEX_DATA *pRoomIndexNext;
                ROOM_INDEX_DATA *in_room;
                char             buf [ MAX_STRING_LENGTH ];

                if (IS_NPC(ch))
                        return;

                pRoomIndexNext = get_room_index(ch->in_room->vnum + 1);

                if (!pRoomIndexNext)
                {
                        bug("Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum);
                        send_to_char("Sorry, you can't buy that here.\n\r", ch);
                        return;
                }

                in_room     = ch->in_room;
                ch->in_room = pRoomIndexNext;
                pet         = get_char_room(ch, arg);
                ch->in_room = in_room;

                if (!pet || !IS_SET(pet->act, ACT_PET))
                {
                        sprintf(log_buf, "Pet: %s Pet->act: %lu", pet ? "Created" : "Not Created", pet ? pet->act : 0);
                        log_string(log_buf);
                        send_to_char("Sorry, you can't buy that here.\n\r", ch);
                        return;
                }

                if ( IS_SET(ch->act, PLR_BOUGHT_PET)
                &&  !IS_IMMORTAL(ch) )
                {
                        send_to_char("You already bought one pet this level.\n\r", ch);
                        return;
                }

                if (ch->level < pet->level)
                {
                        send_to_char("You're not ready for this pet.\n\r", ch);
                        return;
                }

                if ( total_coins_char(ch) < (10 * pet->level * pet->level) )
                {
                        send_to_char("You can't afford it.\n\r", ch);
                        return;
                }

                coins_from_char( (10 * pet->level * pet->level), ch);
                pet = create_mobile(pet->pIndexData);
                SET_BIT(ch->act, PLR_BOUGHT_PET);
                SET_BIT(pet->act, ACT_PET);
                SET_BIT(pet->affected_by, AFF_CHARM);

                argument = one_argument(argument, arg);

                if (arg[0] != '\0')
                {
                        sprintf(buf, "%s %s", pet->name, arg);
                        free_string(pet->name);
                        pet->name = str_dup(buf);
                }

                sprintf(buf, "%sA neck tag says 'I belong to %s'.\n\r",
                        pet->description, ch->name);
                free_string(pet->description);
                pet->description = str_dup(buf);

                char_to_room(pet, ch->in_room);
                add_follower(pet, ch);
                send_to_char("Enjoy your pet.\n\r", ch);
                act("$n bought $N as a pet.", ch, NULL, pet, TO_ROOM);
                return;
        }
        else
        {
                char    buf [MAX_STRING_LENGTH];
                OBJ_DATA  *obj;
                CHAR_DATA *keeper;
                int        cost;
                int        charcoins;
                int        discount;

                /* multiple buy additions :P */
                char arg2[MAX_INPUT_LENGTH];
                int item_count = 1;

                argument = one_argument (argument, arg2);

                if (arg2[0])
                {
                        if (!is_number(arg))
                        {
                                send_to_char ("Syntax for BUY is: BUY [number] <<>item>\n\r\"number\" is an optional number of items to buy.\n\r",ch);
                                return;
                        }

                        item_count = atoi (arg);
                        strcpy (arg,arg2);
                }

                if (!(keeper = find_keeper(ch)))
                        return;

                discount = 0;

                if (ch->pcdata->fame > 400)
                        discount = UMIN((ch->pcdata->fame / 40), 50);

                obj  = get_obj_carry(keeper, arg);
                cost = get_cost(keeper, obj, TRUE);
                cost -= cost * discount / 100;

                if (cost <= 0)
                {
                        act("$n tells you 'I don't sell that -- try LIST.", keeper, NULL, ch, TO_VICT);
                        return;
                }

                if (!can_see_obj(ch, obj))
                {
                        send_to_char ("Buy what?  You can't see anything to buy.\n\r",ch);
                        return;
                }

                /* more mutiple buy additions */
                if (item_count < 1)
                {
                        send_to_char ("Buy how many? Number must be positive!\n\r",ch);
                        return;
                }

                if (ch->pcdata->fame < 0)
                {
                        act("$n tells you 'I will not serve the likes of YOU! GET OUT!'.",
                            keeper, NULL, ch, TO_VICT);
                        act("$n refuses to serve $N due to his reputation.",
                            keeper, NULL, ch, TO_ROOM);
                        return;
                }

                charcoins = total_coins_char(ch);

                if (charcoins < (cost * item_count))
                {
                        if (item_count == 1)
                                act("$n tells you 'You can't afford to buy $p'.",keeper, obj, ch, TO_VICT);
                        else
                        {
                                char buf[MAX_STRING_LENGTH];
                                if ((charcoins / cost) > 0)
                                        sprintf (buf, "$n tells you 'You can only afford %d of those!",
                                                 (charcoins / cost));
                                else
                                        sprintf (buf, "$n tells you '%s? You must be kidding - you can't even afford a single one, let alone %d!'",capitalize(obj->short_descr), item_count);

                                act(buf,keeper, obj, ch, TO_VICT);
                        }

                        return;
                }

                if (obj->level > ch->level)
                {
                        act("$n tells you 'You can't use $p yet'.", keeper, obj, ch, TO_VICT);
                        ch->reply = keeper;
                        return;
                }

                if (ch->carry_number + (get_obj_number(obj)*item_count) > can_carry_n(ch))
                {
                        send_to_char("You can't carry that many items.\n\r", ch);
                        return;
                }

                if ( (ch->carry_weight + ch->coin_weight) + item_count*get_obj_weight(obj) > can_carry_w(ch))
                {
                        send_to_char("You can't carry that much weight.\n\r", ch);
                        return;
                }

                /* check for objects sold to the keeper */
                if ((item_count > 1) && !IS_SET (obj->extra_flags,ITEM_INVENTORY))
                {
                        act("$n tells you 'Sorry--$p is something I have only one of.'",keeper, obj, ch, TO_VICT);
                        return;
                }

                if (ch->pcdata->fame < 400)
                        act("$n nods solemnly at you.", keeper, NULL, ch, TO_VICT);

                else if (ch->pcdata->fame < 700)
                        act("$n tells you 'I have heard rumors of your courage.'",
                            keeper, NULL, ch, TO_VICT);

                else if (ch->pcdata->fame < 1000)
                        act("$n tells you 'I have heard many rumors of your battles.'",
                            keeper, NULL, ch, TO_VICT);

                else if (ch->pcdata->fame < 1300)
                        act("$n smiles happily at you.", keeper, NULL, ch, TO_VICT);

                else if (ch->pcdata->fame < 1600)
                        act("$n grins broadly, recognising you immediately.",
                            keeper, NULL, ch, TO_VICT);

                else
                        act("$n tells you 'You are a legend to us $N!'", keeper, NULL,
                            ch, TO_VICT);

                if (item_count == 1)
                {
                        act("$n buys $p.", ch, obj, NULL, TO_ROOM);
                        act("You buy $p.", ch, obj, NULL, TO_CHAR);
                }
                else
                {
                        char buf[MAX_STRING_LENGTH];
                        sprintf (buf, "$n buys %d of $p.", item_count);
                        act (buf, ch, obj, NULL, TO_ROOM);
                        sprintf (buf, "You buy %d of $p.", item_count);
                        act(buf, ch, obj, NULL, TO_CHAR);
                }

                if (ch->pcdata->fame < 200 || discount == 0)
                        act("$n places the coins in his purse.", keeper, NULL, ch, TO_VICT);

                else if (ch->pcdata->fame < 500)
                {
                        sprintf(buf, "You get a %2d%% discount for your valour.\n\r",
                                discount);
                        send_to_char (buf, ch);
                }

                else if (ch->pcdata->fame < 900)
                {
                        sprintf(buf, "You receive a %2d%% off for you Heroic deeds!!\n\r",
                                discount);
                        send_to_char(buf, ch);
                }

                else
                {
                        act("$n is so excited you are in his store, he only charges you half price!\n\r",
                            keeper, NULL, ch, TO_VICT);
                }

                coins_from_char(cost * item_count, ch);
                /* coins_to_char ( (cost*item_count), keeper, COINS_ADD); */

                if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
                {
                        for (; item_count > 0; item_count--)
                        {
                                obj = create_object(obj->pIndexData, obj->level, "common",CREATED_NO_RANDOMISER);
                                obj_to_char(obj, ch);
                        }
                }
                else
                {
                        obj_from_char(obj);
                        obj_to_char(obj, ch);
                }
        }
}


void do_list (CHAR_DATA *ch, char *argument)
{
        char buf  [ MAX_STRING_LENGTH   ];
        char buf1 [ MAX_STRING_LENGTH*4 ];

        buf1[0] = '\0';

        if (IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP))
        {
                CHAR_DATA       *pet;
                ROOM_INDEX_DATA *pRoomIndexNext;
                bool             found;

                pRoomIndexNext = get_room_index(ch->in_room->vnum + 1);

                if (!pRoomIndexNext)
                {
                        bug("Do_list: bad pet shop at vnum %d.", ch->in_room->vnum);
                        send_to_char("You can't do that here.\n\r", ch);
                        return;
                }

                found = FALSE;

                for (pet = pRoomIndexNext->people; pet; pet = pet->next_in_room)
                {
                        if (IS_SET(pet->act, ACT_PET))
                        {
                                if (!found)
                                {
                                        found = TRUE;
                                        strcat(buf1, "{d[{WLvl  Price{d]{W  Creature for sale{x\n\r");
                                        /*strcat(buf1, "Pets for sale:\n\r");*/
                                }
                                sprintf(buf, "{d[{x{w%3d %6d{d]{x  %s\n\r",
                                        pet->level,
                                        10 * pet->level * pet->level,
                                        pet->short_descr);
                                strcat(buf1, buf);
                        }
                }
                if (!found)
                        send_to_char("Sorry, we're out of pets right now.\n\r", ch);

                send_to_char(buf1, ch);
                return;
        }
        else
        {
                OBJ_DATA  *obj;
                CHAR_DATA *keeper;
                char       arg [ MAX_INPUT_LENGTH ];
                int        cost;
                bool       found;

                one_argument(argument, arg);

                if (!(keeper = find_keeper(ch)))
                        return;

                found = FALSE;

                for (obj = keeper->carrying; obj; obj = obj->next_content)
                {
                        if (obj->wear_loc != WEAR_NONE
                            || (cost = get_cost(keeper, obj, TRUE)) < 0)
                                continue;

                        if (can_see_obj(ch, obj)
                            && (arg[0] == '\0' || is_name(arg, obj->name)))
                        {
                                if (!found)
                                {
                                        found = TRUE;
                                        strcat(buf1, "{d[{WLvl Price{d]{W  Item for sale{x\n\r");
                                }

                                sprintf(buf, "{d[{x{w%3d %5d{d]{x  %s\n\r",
                                        obj->level, cost, obj->short_descr);
                                strcat(buf1, buf);
                        }
                }

                if (!found)
                {
                        if (arg[0] == '\0')
                                send_to_char("You can't buy anything here.\n\r", ch);
                        else
                                send_to_char("You can't buy that here.\n\r", ch);
                        return;
                }

                send_to_char(buf1, ch);
                return;
        }
}


void do_sell (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *keeper;
        char       buf [ MAX_STRING_LENGTH ];
        char       arg [ MAX_INPUT_LENGTH  ];
        int        cost;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Sell what?\n\r", ch);
                return;
        }

        if (!(keeper = find_keeper(ch)))
                return;

        if (!(obj = get_obj_carry(ch, arg)))
        {
                act("$n tells you 'You don't have that item'.", keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        if (!can_drop_obj(ch, obj))
        {
                send_to_char("You can't let go of it.\n\r", ch);
                return;
        }

        if (obj->owner[0] != '\0')
        {
                act("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
                return;
        }

        if (IS_SET (obj->extra_flags, ITEM_DONATED))
        {
                send_to_char("You can't sell a donated item.\n\r", ch);
                return;
        }

        if (!can_see_obj(keeper, obj))
        {
                act("$n tells you 'I can't see that item'.", keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        if ((cost = get_cost(keeper, obj, FALSE)) <= 0 )
        {
                act("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_POISONED))
        {
                act("$n tells you 'I won't buy that!  It's poisoned!'",
                    keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        sprintf(buf, "You sell $p for %d coin%s.", cost, cost == 1 ? "" : "s");
        act(buf, ch, obj, NULL, TO_CHAR);
        act("$n sells $p.", ch, obj, NULL, TO_ROOM);

        if (ch->pcdata->bank < 0)
        {
                act("$n tells you 'I see you have a loan with the bank, I am giving them 1/2 the sale.",
                    keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                coins_to_char ( cost / 2, ch, COINS_ADD);
                ch->pcdata->bank += cost/2;
        }
        else
                coins_to_char ( cost, ch, COINS_ADD);

        if (total_coins_char(keeper) < cost)
                coins_to_char(0, keeper, COINS_REPLACE);
        else
                coins_from_char( cost, keeper);

        if (obj->item_type == ITEM_TRASH)
                extract_obj(obj);
        else
        {
                obj_from_char(obj);
                obj_to_char(obj, keeper);
        }

        return;
}


void do_value (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA  *obj;
        CHAR_DATA *keeper;
        char       buf [ MAX_STRING_LENGTH ];
        char       arg [ MAX_INPUT_LENGTH  ];
        int        cost;

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Value what?\n\r", ch);
                return;
        }

        if (!(keeper = find_keeper(ch)))
                return;

        if (!(obj = get_obj_carry(ch, arg)))
        {
                act("$n tells you 'You don't have that item'.", keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        if (!can_drop_obj(ch, obj))
        {
                send_to_char("You can't let go of it.\n\r", ch);
                return;
        }

        if (!can_see_obj(keeper, obj))
        {
                act("$n tells you 'You are offering me an imaginary object!?!?'.",
                    keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        if ((cost = get_cost(keeper, obj, FALSE)) <= 0)
        {
                act("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_POISONED))
        {
                act("$n tells you 'I won't buy that!  It's poisoned!'",
                    keeper, NULL, ch, TO_VICT);
                ch->reply = keeper;
                return;
        }

        sprintf(buf, "$n tells you 'I'll give you %d coins for $p'.", cost);
        act(buf, keeper, obj, ch, TO_VICT);
        ch->reply = keeper;

        return;
}


/* Poison weapon by Thelonius for EnvyMud */
void do_poison_weapon(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pobj = NULL;
        OBJ_DATA *wobj = NULL;
        char      arg [ MAX_INPUT_LENGTH ];
        bool in_c_room;
        int obj_craft_bonus;
        int mod_room_bonus;

        in_c_room = FALSE;

        obj_craft_bonus = get_craft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_POISON_WEAPON + obj_craft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_CRAFT ))
        {
             in_c_room = TRUE;
        }

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_poison_weapon))
        {
                send_to_char("What do you think you are, a thief?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What are you trying to poison?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that weapon.\n\r", ch);
                return;
        }

        if (!is_bladed_weapon(obj))
        {
                send_to_char("You can only poison bladed weapons.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_POISONED))
        {
                send_to_char("That weapon is already poisoned.\n\r", ch);
                return;
        }

        for (pobj = ch->carrying; pobj; pobj = pobj->next_content)
        {
                if (pobj->item_type == ITEM_POISON_POWDER)
                        break;
        }

        if (!pobj)
        {
                send_to_char("You do not have any poison powder.\n\r", ch);
                return;
        }

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_DRINK_CON
                    && wobj->value[1]  >  0
                    && wobj->value[2]  == 0)
                        break;
        }

        if (!wobj)
        {
                send_to_char("You have no water to mix with the powder.\n\r", ch);
                return;
        }

        if (get_curr_dex(ch) < 20 || ch->pcdata->condition[COND_DRUNK] > 0)
        {
                send_to_char("Your hands aren't steady enough to properly mix the poison.\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA) )
        {
                send_to_char("Your arms are too badly damaged to coordinate this task.\n\r", ch);
                return;
        }

        if (IS_AFFECTED(ch, AFF_HEAD_TRAUMA))
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_poison_weapon].beats);

        if ( IS_AFFECTED(ch, AFF_ARM_TRAUMA)
        &&   ( number_percent() >= (ch->pcdata->learned[gsn_poison_weapon] / 2) ) )
        {
                send_to_char("You fail due to clumsiness and spill some on yourself.  Ouch!\n\r", ch);
                act("$n spills the poison all over!", ch, NULL, NULL, TO_ROOM);
                damage(ch, ch, ch->level, gsn_poison_weapon, FALSE);
                extract_obj(pobj);
                extract_obj(wobj);
                return;
        }

        if ( number_percent() >= ch->pcdata->learned[gsn_poison_weapon])
        {
                send_to_char("You failed and spill some on yourself.  Ouch!\n\r", ch);
                act("$n spills the poison all over!", ch, NULL, NULL, TO_ROOM);
                damage(ch, ch, ch->level, gsn_poison_weapon, FALSE);
                extract_obj(pobj);
                extract_obj(wobj);
                return;
        }

        if (in_c_room)
                send_to_char("{CYour use of specialised tools improves the longevity of your poison!\n\r{x", ch);

        act("You pour poison over $p, which glistens wickedly!", ch, obj, NULL, TO_CHAR);
        act("$n pours poison over $p, which glistens wickedly!", ch, obj, NULL, TO_ROOM);

        SET_BIT(obj->extra_flags, ITEM_POISONED);
        set_obj_owner(obj, ch->name);
        obj->timer    = (in_c_room) ? 30 + ( ( ch->level * mod_room_bonus ) / 100 ) * 2  : 30 + ch->level * 2;
        obj->timermax = (in_c_room) ? 30 + ( ( ch->level * mod_room_bonus ) / 100 ) * 2  : 30 + ch->level * 2;

        act("The remainder of the poison eats through $p.", ch, wobj, NULL, TO_CHAR);
        act("The remainder of the poison eats through $p.", ch, wobj, NULL, TO_ROOM);
        extract_obj(pobj);
        extract_obj(wobj);
}


void do_bladethirst (CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *pobj;
        OBJ_DATA *wobj;
        AFFECT_DATA *paf;
        char arg [ MAX_INPUT_LENGTH ];
        bool in_sc_room;
        int obj_spellcraft_bonus;
        int mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_BLADETHIRST + obj_spellcraft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_SPELLCRAFT ))
        {
             in_sc_room = TRUE;
        }

        if (IS_NPC(ch))
            return;

        if (!CAN_DO(ch, gsn_bladethirst))
        {
                send_to_char("What do you think you are, a necromancer?\n\r", ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("What weapon do you want to use?\n\r", ch);
                return;
        }

        if (ch->fighting)
        {
                send_to_char("While you're fighting?  Nice try.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg)))
        {
                send_to_char("You do not have that weapon.\n\r", ch);
                return;
        }

        if (!is_bladed_weapon(obj))
        {
                send_to_char("You must have a bladed weapon.\n\r", ch);
                return;
        }

        if (IS_OBJ_STAT(obj, ITEM_BLADE_THIRST))
        {
                send_to_char("That weapon is already thirsty.\n\r", ch);
                return;
        }

        for (pobj = ch->carrying; pobj; pobj = pobj->next_content)
        {
                if (pobj->item_type == ITEM_MITHRIL)
                        break;
        }

        if (!pobj)
        {
                send_to_char("You do not have the mithril.\n\r", ch);
                return;
        }

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_DRINK_CON
                    && wobj->value[1]  >  0
                    && wobj->value[2]  == 13)
                        break;
        }

        if (!wobj)
        {
                send_to_char("You need some blood for this skill.\n\r", ch);
                return;
        }

        WAIT_STATE(ch, skill_table[gsn_bladethirst].beats);

        if (number_percent() > ch->pcdata->learned[gsn_bladethirst])
        {
                send_to_char("You failed and spill some on yourself.  Ouch!\n\r", ch);
                damage(ch, ch, ch->level * 2, gsn_bladethirst, FALSE);
                act("$n spills the blade thirst liquid all over!", ch, NULL, NULL, TO_ROOM);
                extract_obj(pobj);
                extract_obj(wobj);
                return;
        }

        act("You mix $p in $P, creating an evil looking potion!",
            ch, pobj, wobj, TO_CHAR);
        act("$n mixes $p in $P, creating an evil looking potion!",
            ch, pobj, wobj, TO_ROOM);

        if ( in_sc_room )
                send_to_char( "{MYour use of spellcrafting resources improves your potion's potency!{x\n\r", ch);

        act("You pour the potion over $p, which glistens wickedly!",
            ch, obj, NULL, TO_CHAR);
        act("$n pours the potion over $p, which glistens wickedly!",
            ch, obj, NULL, TO_ROOM);



        SET_BIT(obj->extra_flags, ITEM_BLADE_THIRST);
        set_obj_owner(obj, ch->name);
        obj->timer    = ( in_sc_room ) ? 60 + (ch->level / 15) * ( 30 * mod_room_bonus / 100) : 60 + (ch->level / 15) * 30;
        obj->timermax = ( in_sc_room ) ? 60 + (ch->level / 15) * ( 30 * mod_room_bonus / 100) : 60 + (ch->level / 15) * 30;

        act("The remainder of the potion eats through $p.", ch, wobj, NULL, TO_CHAR);
        act("The remainder of the potion eats through $p.", ch, wobj, NULL, TO_ROOM);
        extract_obj(pobj);
        extract_obj(wobj);

        if (!affect_free)
        {
                paf             = alloc_perm(sizeof(*paf));
        }
        else
        {
                paf             = affect_free;
                affect_free     = affect_free->next;
        }

        paf->type           = -1;
        paf->duration       = -1;
        paf->location       = APPLY_DAMROLL;
        paf->modifier       = ( in_sc_room ) ? 2 + ( ch->level / ( mod_room_bonus / 20 ) ) : 2 + ch->level / 5;
        paf->bitvector      = 0;
        paf->next           = obj->affected;
        obj->affected       = paf;

}


void do_donate (CHAR_DATA *ch, char *argument)
{
        ROOM_INDEX_DATA *pRoomIndex;
        ROOM_INDEX_DATA *was_in;
        char arg[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        bool found;

        argument = one_argument(argument, arg);
        pRoomIndex = get_room_index(ROOM_VNUM_DONATION);

        if (IS_AFFECTED(ch, AFF_NON_CORPOREAL))
        {
                send_to_char("Not in your current form.\n\r", ch);
                return;
        }

        if (arg[0] == '0')
        {
                send_to_char("Donate what?\n\r", ch);
                return;
        }

        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {
                if (!(obj = get_obj_carry(ch, arg)))
                {
                        send_to_char("You do not have that item.\n\r", ch);
                        return;
                }

                if (IS_SET(obj->extra_flags, ITEM_NODROP))
                {
                        send_to_char("You can't let go of it!\n\r", ch);
                        return;
                }

                if (IS_SET(obj->extra_flags, ITEM_CURSED))
                {
                        send_to_char("It is oddly resistant to being donated.\n\r", ch);
                        return;
                }

                if (obj->item_type == ITEM_WEAPON)
                {
                        if (obj->level <= 5)
                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_5);
                        else
                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_1);
                }

                else if (obj->item_type == ITEM_ARMOR)
                {
                        if (obj->level <= 5)
                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_5);
                        else
                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_2);
                }

                else if (obj->item_type == ITEM_TRASH
                         || obj->item_type == ITEM_FURNITURE
                         || obj->item_type == ITEM_CORPSE_NPC
                         || obj->item_type == ITEM_REMAINS)
                {
                        pRoomIndex = get_room_index(ROOM_VNUM_DONATION_4);
                }

                else
                        pRoomIndex = get_room_index(ROOM_VNUM_DONATION_3);

                obj_from_char(obj);
                obj_to_room(obj, pRoomIndex);
                SET_BIT(obj->extra_flags, ITEM_DONATED);

                act("$n donates $p.", ch, obj, NULL, TO_ROOM);
                act("You donate $p.", ch, obj, NULL, TO_CHAR);

                was_in = ch->in_room;
                char_from_room(ch);
                char_to_room(ch, pRoomIndex);
                act("$p appears suddenly before falling to the floor!", ch, obj, NULL, TO_ROOM);
                char_from_room(ch);
                char_to_room(ch, was_in);
        }
        else
        {
                /* 'donate all' or 'donate all.obj' */
                found = FALSE;

                for (obj = ch->carrying; obj; obj = obj_next)
                {
                        obj_next = obj->next_content;

                        if ((arg[3] == '\0' || is_name(&arg[4], obj->name))
                            && can_see_obj(ch, obj)
                            && obj->wear_loc == WEAR_NONE
                            && can_drop_obj(ch, obj))
                        {
                                if (IS_SET(obj->extra_flags, ITEM_NODROP))
                                {
                                        send_to_char("You can't let go of that!\n\r", ch);
                                        return;
                                }

                                if (IS_SET(obj->extra_flags, ITEM_CURSED))
                                {
                                        send_to_char("It is oddly resistant to being donated.\n\r", ch);
                                        return;
                                }

                                if (obj->item_type == ITEM_WEAPON)
                                {
                                        if (obj->level <= 5)
                                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_5);
                                        else
                                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_1);
                                }

                                else if (obj->item_type == ITEM_ARMOR)
                                {
                                        if (obj->level <= 5)
                                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_5);
                                        else
                                                pRoomIndex = get_room_index(ROOM_VNUM_DONATION_2);
                                }

                                else if (obj->item_type == ITEM_TRASH
                                         || obj->item_type == ITEM_FURNITURE
                                         || obj->item_type == ITEM_CORPSE_NPC
                                         || obj->item_type == ITEM_REMAINS)
                                {
                                        pRoomIndex = get_room_index(ROOM_VNUM_DONATION_4);
                                }

                                else
                                        pRoomIndex = get_room_index(ROOM_VNUM_DONATION_3);

                                found = TRUE;
                                obj_from_char(obj);
                                obj_to_room(obj, pRoomIndex);
                                SET_BIT(obj->extra_flags, ITEM_DONATED);
                                act("$n donates $p.", ch, obj, NULL, TO_ROOM);
                                act("You donate $p.", ch, obj, NULL, TO_CHAR);

                                was_in = ch->in_room;
                                char_from_room(ch);
                                char_to_room(ch, pRoomIndex);
                                act("$p appears suddenly before falling to the floor!", ch, obj,
                                    NULL, TO_ROOM);
                                char_from_room(ch);
                                char_to_room(ch, was_in);
                        }
                }

                if (!found)
                {
                        if (arg[3] == '\0')
                                act("You are not carrying anything.", ch, NULL, NULL, TO_CHAR);
                        else
                                act("You are not carrying any $T.", ch, NULL, &arg[4], TO_CHAR);
                }
        }

        return;
}


void do_brew (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;
        int sn;
        bool in_sc_room;
        int obj_spellcraft_bonus;
        int mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_BREW + obj_spellcraft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_SPELLCRAFT ))
        {
             in_sc_room = TRUE;
        }

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_brew))
        {
                send_to_char("You do not know how to brew potions.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Brew what spell?\n\r", ch);
                return;
        }

        for (obj = ch->carrying; obj; obj = obj->next_content)
        {
                if (obj->item_type == ITEM_POTION && obj->wear_loc == WEAR_HOLD)
                        break;
        }

        if (!obj)
        {
                send_to_char("You are not holding a vial.\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_HEAD_TRAUMA) )
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }

        if ((sn = skill_lookup(arg))  < 0)
        {
                send_to_char("You don't know any spells by that name.\n\r", ch);
                return;
        }

        send_to_char("You focus your mind's energies...\n\r",ch);
        act("$n begins preparing a potion.", ch, obj, NULL, TO_ROOM);
        WAIT_STATE(ch, skill_table[gsn_brew].beats);

        if (number_percent() > ch->pcdata->learned[gsn_brew]
            || number_percent() > ((get_curr_int(ch) - 13) * 5 + (get_curr_wis(ch) - 13) *3))
        {
                act("$p explodes violently!", ch, obj, NULL, TO_CHAR);
                act("$p explodes violently!", ch, obj, NULL, TO_ROOM);
                spell_acid_blast(skill_lookup("acid blast"), ((ch->level/4)*3), ch, ch);
                extract_obj(obj);
                return;
        }

        if ( in_sc_room )
                send_to_char( "{MYour brew's strength will be increased by the use of spellcrafting resources!{x\n\r", ch);

        obj->level = ch->level;
        obj->value[0] = (in_sc_room) ? ( ch->level / 2 * mod_room_bonus ) / 100 : ch->level / 2;
        spell_imprint(sn, ch->level, ch, obj);

        if (skill_table[sn].target == TAR_CHAR_SELF)
        {
                set_obj_owner(obj, ch->name);
        }

}


void do_scribe (CHAR_DATA *ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;
        int sn;
        bool in_sc_room;
        int obj_spellcraft_bonus;
        int mod_room_bonus;

        in_sc_room = FALSE;
        obj_spellcraft_bonus = get_spellcraft_obj_bonus( ch );
        mod_room_bonus = CRAFT_BONUS_SCRIBE + obj_spellcraft_bonus;

        if (IS_SET( ch->in_room->room_flags, ROOM_SPELLCRAFT ))
        {
             in_sc_room = TRUE;
        }

        if (IS_NPC(ch))
                return;

        if (!CAN_DO(ch, gsn_scribe))
        {
                send_to_char("You do not know how to scribe scrolls.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                send_to_char("Scribe what spell?\n\r", ch);
                return;
        }

        for (obj = ch->carrying; obj; obj = obj->next_content)
        {
                if (obj->item_type == ITEM_SCROLL && obj->wear_loc == WEAR_HOLD)
                        break;
        }
        if (!obj)
        {
                send_to_char("You are not holding a parchment.\n\r", ch);
                return;
        }

        if ((sn = skill_lookup(arg)) < 0)
        {
                send_to_char("You don't know any spells by that name.\n\r", ch);
                return;
        }

        if ( IS_AFFECTED(ch, AFF_HEAD_TRAUMA) )
        {
                send_to_char("You can't remember how to do that.\n\r", ch);
                return;
        }


        act("$n begins writing a scroll.", ch, obj, NULL, TO_ROOM);
        WAIT_STATE(ch, skill_table[gsn_scribe].beats);

        if (number_percent() > ch->pcdata->learned[gsn_scribe]
            || number_percent() > ((get_curr_int(ch) - 13) * 5 + (get_curr_wis(ch) - 13) * 3))
        {
                act("$p bursts in flames!", ch, obj, NULL, TO_CHAR);
                act("$p bursts in flames!", ch, obj, NULL, TO_ROOM);
                spell_fireball(skill_lookup("fireball"), ch->level, ch, ch);
                extract_obj(obj);
                return;
        }

        if ( in_sc_room )
                send_to_char( "{MYour scroll's power will be increased by the use of spellcrafting resources!{x\n\r", ch);

        obj->level = ch->level;
        obj->value[0] = (in_sc_room) ? ( ch->level / 2 * mod_room_bonus ) / 100 : ch->level / 2;
        spell_imprint(sn, ch->level, ch, obj);

        if (skill_table[sn].target == TAR_CHAR_SELF)
        {
                set_obj_owner(obj, ch->name);
        }
}


void do_auction (CHAR_DATA *ch, char *argument)
{
        /*
         * All bids are now in platinum.
         * auction->bet and auction->reserve are *platinum* amounts.
         */

        OBJ_DATA *obj;
        OBJ_DATA *wobj;
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (IS_NPC(ch))
                return;

        if (arg1[0] == '\0')
        {
                if (auction->item)
                {
                        sprintf(buf, "The following item is being auctioned: %s.\n\r\n\r",
                                auction->item->short_descr);
                        send_to_char(buf, ch);

                        spell_identify(0, LEVEL_HERO, ch, auction->item);

                        if (auction->bet > 0)
                                sprintf (buf, "\n\rCurrent bid on this item is {W%d{x platinum.\n\r",
                                         auction->bet);
                        else
                                sprintf (buf, "\n\rNo bids on this item have been received.\n\r"
                                         "The reserve price for this item is %d platinum.\n\r",
                                         auction->reserve);

                        send_to_char(buf,ch);

                        if (ch->level > LEVEL_HERO && auction->buyer)
                        {
                                sprintf(buf, "Seller: %s.  Bidder: %s.  Round: %d.\n\r",
                                        auction->seller->name,
                                        auction->buyer->name,
                                        auction->going + 1);
                                send_to_char(buf, ch);

                                sprintf(buf, "Time left in round: %d.\n\r", auction->pulse);
                                send_to_char(buf, ch);
                        }
                }
                else
                        send_to_char ("Auction what item?\n\r",ch);

                return;
        }

        if (ch->level > LEVEL_HERO && !str_cmp(arg1, "stop"))
        {
                if (!auction->item)
                {
                        send_to_char ("There is no auction to stop.\n\r",ch);
                        return;
                }
                else
                {
                        sprintf (buf, "Sale of %s has been stopped and the item confiscated.",
                                 auction->item->short_descr);
                        talk_auction(buf);

                        obj_to_char(auction->item, ch);
                        auction->item = NULL;

                        if (auction->buyer)
                        {
                                if (auction->from_bank)
                                        auction->buyer->pcdata->bank += auction->bet * COIN_PLAT;
                                else
                                        coins_to_char(auction->bet * COIN_PLAT, auction->buyer, COINS_ADD);

                                send_to_char("Your money has been refunded.\n\r", auction->buyer);
                        }

                        return;
                }
        }

        if (!str_cmp(arg1, "bet"))
        {
                /*
                if (ch->level > LEVEL_HERO)
                {
                        send_to_char("Immortals may not bet in auctions.\n\r", ch);
                        return;
                }
                Removed 2/3/22 by Owl. imms do what they want.
                */

                if (auction->item)
                {
                        int newbet;
                        bool use_bank_for_bet;

                        if (arg2[0] == '\0')
                        {
                                send_to_char("Bet how much?\n\r",ch);
                                return;
                        }

                        /* newbet is bet in platinum */
                        newbet = parsebet(auction->bet, arg2);

                        if (ch == auction->seller)
                        {
                                send_to_char("You can't bid on your own item.\n\r", ch);
                                return;
                        }

                        if (newbet < auction->reserve)
                        {
                                sprintf(buf, "You must meet the reserve price of %d platinum.\n\r",
                                        auction->reserve);
                                send_to_char(buf, ch);
                                return;
                        }

                        if (newbet < auction->bet + 5 && auction->bet > 0)
                        {
                                send_to_char("You must bid at least 5 platinum coins over the current bet.\n\r",ch);
                                return;
                        }

                        if (newbet > 1000000)
                        {
                                send_to_char("You can not bid more than one million platinum.\n\r", ch);
                                return;
                        }

                        /*
                         * Bet payment: try the bank first, then carried coin
                         */
                        if (newbet * COIN_PLAT <= ch->pcdata->bank)
                        {
                                ch->pcdata->bank -= newbet * COIN_PLAT;
                                use_bank_for_bet = TRUE;
                        }
                        else if (newbet * COIN_PLAT <= total_coins_char(ch))
                        {
                                coins_from_char(newbet * COIN_PLAT, ch);
                                use_bank_for_bet = FALSE;
                        }
                        else
                        {
                                send_to_char("You don't have that much money!\n\r",ch);
                                return;
                        }

                        if (auction->buyer)
                        {
                                if (auction->from_bank)
                                        auction->buyer->pcdata->bank += auction->bet * COIN_PLAT;
                                else
                                        coins_to_char(auction->bet * COIN_PLAT, auction->buyer, COINS_ADD);
                        }

                        auction->buyer = ch;
                        auction->bet   = newbet;
                        auction->going = 0;
                        auction->pulse = PULSE_AUCTION;
                        auction->from_bank = use_bank_for_bet;

                        sprintf (buf,"A bet of %d platinum has been received on %s.",
                                 newbet,
                                 auction->item->short_descr);
                        talk_auction (buf);
                }
                else
                        send_to_char ("There isn't anything being auctioned right now.\n\r",ch);

                return;
        }

        /*
        if (ch->level > LEVEL_HERO)
        {
                send_to_char("Immortals may not auction items.\n\r", ch);
                return;
        }
        Removed 2/3/22 by Owl. imms do what they want.
        */

        for (wobj = ch->carrying; wobj; wobj = wobj->next_content)
        {
                if (wobj->item_type == ITEM_AUCTION_TICKET)
                        break;
        }

        if (!wobj)
        {
                send_to_char("You need an auction ticket to sell an item.\n\r", ch);
                return;
        }

        if (!(obj = get_obj_carry(ch, arg1)))
        {
                send_to_char("You do not have that item.\n\r", ch);
                return;
        }

        if (!auction->item)
        {
                int min_reserve = UMAX(obj->level / 10, 1);
                int reserve = min_reserve;

                if (obj->item_type != ITEM_WEAPON
                    && obj->item_type != ITEM_ARMOR
                    && obj->item_type != ITEM_STAFF
                    && obj->item_type != ITEM_LIGHT
                    && obj->item_type != ITEM_WAND
                    && obj->item_type != ITEM_TREASURE
                    && obj->item_type != ITEM_FURNITURE
                    && obj->item_type != ITEM_CONTAINER
                    && obj->item_type != ITEM_DRINK_CON
                    && obj->item_type != ITEM_SCROLL
                    && obj->item_type != ITEM_POTION
                    && obj->item_type != ITEM_PAINT
                    && obj->item_type != ITEM_PILL
                    && obj->item_type != ITEM_INSTRUMENT
                    && obj->item_type != ITEM_SMOKEABLE
                    && obj->item_type != ITEM_PIPE
                    && obj->item_type != ITEM_COMBAT_PULSE
                    && obj->item_type != ITEM_DEFENSIVE_PULSE)
                {
                        send_to_char("You cannot auction that kind of item.\n\r", ch);
                        return;
                }

                if (obj->owner[0] != '\0')
                {
                        send_to_char("You cannot auction that item.\n\r", ch);
                        return;
                }

                if (str_cmp(obj->short_descr, obj->pIndexData->short_descr))
                {
                        send_to_char("You cannot auction renamed items.\n\r", ch);
                        return;
                }

                if (arg2[0] != '\0')
                {
                        if (!is_number(arg2))
                        {
                                send_to_char("Reserve price must be a number.\n\r", ch);
                                return;
                        }

                        reserve = atoi(arg2);

                        if (reserve < min_reserve)
                        {
                                sprintf(buf, "Sorry, the reserve price for this item must be at least %d platinum.\n\r",
                                        min_reserve);
                                send_to_char(buf, ch);
                                return;
                        }

                        if (reserve > 10000)
                        {
                                send_to_char("Sorry, the reserve price cannot be higher than 10000 platinum.\n\r", ch);
                                return;
                        }
                }

                extract_obj(wobj);
                obj_from_char(obj);

                auction->item = obj;
                auction->bet = 0;
                auction->buyer = NULL;
                auction->seller = ch;
                auction->pulse = PULSE_AUCTION * 2;
                auction->going = 0;
                auction->reserve = reserve;

                REMOVE_BIT(ch->deaf, CHANNEL_AUCTION);

                sprintf(buf, "A new item has been received: %s.", obj->short_descr);
                talk_auction(buf);

                sprintf(buf, "The reserve for this item is %d platinum.",
                        auction->reserve);
                talk_auction(buf);
                return;
        }

        act ("Try again later: $p is being auctioned right now!", ch, auction->item, NULL, TO_CHAR);
}


void coin_crunch (int numcoins, COIN_DATA *coins )
{
        if (numcoins > (COIN_PLAT - 1) )
        {
                coins->plat = numcoins / COIN_PLAT;
                numcoins = numcoins % (COIN_PLAT * coins->plat);
        }
        else
                coins->plat = 0;

        if (numcoins > (COIN_GOLD -1) )
        {
                coins->gold = numcoins / COIN_GOLD;
                numcoins = numcoins % (COIN_GOLD * coins->gold);
        }
        else
                coins->gold= 0;

        if (numcoins > (COIN_SILVER -1) )
        {
                coins->silver = numcoins / COIN_SILVER;
                numcoins = numcoins % (COIN_SILVER * coins->silver);
        }
        else
                coins->silver= 0;

        coins->copper = numcoins;
}


void do_balance (CHAR_DATA *ch, char *argument)
{
        COIN_DATA *coins;
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC(ch))
                return;

        if (ch->pcdata->bank < 0)
        {
                sprintf(buf, "You currently owe the bank %d coins.\n\r", -ch->pcdata->bank);
                send_to_char(buf, ch);
        }
        else
        {
                coins = (COIN_DATA *) malloc (sizeof(COIN_DATA));
                if (!coins)
                {
                        bug ("do_balance: COIN_DATA - not enough memory to allocate", 0);
                        exit (1);
                }
                coin_crunch (ch->pcdata->bank, coins);

                sprintf(buf, "You currently have %d platinum, %d gold, %d silver and %d copper coins in the bank.\n\r",
                        coins->plat, coins->gold, coins->silver, coins->copper);
                send_to_char(buf, ch);

                free (coins);
        }
}


void do_deposit (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *banker = NULL;
        char arg1 [MAX_INPUT_LENGTH];
        char arg2 [MAX_INPUT_LENGTH];
        char buf [MAX_STRING_LENGTH];
        int amnt = 0;
        bool depositok = FALSE;
        bool all = FALSE;

        const int max_balance = 1000000000;  /* 1 million platinum */

        if (IS_NPC(ch))
                return;

        for (banker = ch->in_room->people; banker; banker = banker->next_in_room)
        {
                if (IS_NPC(banker) && IS_SET(banker->pIndexData->act, ACT_BANKER))
                        break;
        }

        if (!banker)
        {
                send_to_char("There is no banker here.\n\r", ch);
                return;
        }

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0')
        {
                send_to_char("How many coins do you want to deposit?\n\r", ch);
                return;
        }

        if (!str_cmp(arg1, "all"))
                all = TRUE;
        else
        {
                if (!is_number(arg1) || atoi(arg1) < 1)
                {
                        send_to_char("That is not a valid number of coins to deposit.\n\r", ch);
                        return;
                }

                amnt = atoi(arg1);
        }

        if (arg2[0] == '\0')
        {
                if (!all)
                {
                        send_to_char("You must specify a coin type: platinum, gold, silver or copper.\n\r", ch);
                        return;
                }

                if (!ch->plat && !ch->gold && !ch->silver && !ch->copper)
                        send_to_char("You have no coins to deposit.\n\r", ch);
                else
                {
                        ch->pcdata->bank += ch->plat * COIN_PLAT
                                + ch->gold * COIN_GOLD
                                + ch->silver * COIN_SILVER
                                + ch->copper * COIN_COPPER;

                        ch->plat = 0;
                        ch->gold = 0;
                        ch->silver = 0;
                        ch->copper = 0;
                        calc_coin_weight(ch);
                        ch->pcdata->bank = UMIN(ch->pcdata->bank, max_balance);

                        send_to_char("You deposit all of your coins.\n\r", ch);
                        do_balance(ch, "");
                }

                return;
        }


        if( !str_prefix(arg2, "platinum") )
        {
                if (all)
                        amnt = ch->plat;

                if (ch->plat == 0)
                {
                        sprintf(buf, "%s, you have no platinum coins to deposit.", ch->name);
                        do_say(banker, buf);
                        return;
                }

                if (amnt > ch->plat)
                {
                        sprintf(buf, "%s, you do not have %d platinum coins to deposit.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }

                if (ch->pcdata->bank + (amnt * COIN_PLAT) > max_balance)
                {
                        sprintf (buf, "Sorry %s, but you may only keep the equivalent of one million "
                                 "platinum in your account.", ch->name);
                        do_say (banker, buf);
                        return;
                }

                ch->pcdata->bank += (amnt * COIN_PLAT);
                ch->plat -= amnt;
                depositok = TRUE;
        }

        if( !str_prefix(arg2, "gold") )
        {
                if (all)
                        amnt = ch->gold;

                if (ch->gold == 0 )
                {
                        sprintf(buf, "%s, you have no gold coins to deposit.", ch->name);
                        do_say(banker, buf);
                        return;
                }

                if (amnt > ch->gold)
                {
                        sprintf(buf, "%s, you do not have %d gold coins to deposit.",
                                ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }

                if (ch->pcdata->bank + (amnt * COIN_GOLD) > max_balance)
                {
                        sprintf (buf, "Sorry %s, but you may only keep the equivalent of one million "
                                 "platinum in your account.", ch->name);
                        do_say (banker, buf);
                        return;
                }

                ch->pcdata->bank += (amnt * COIN_GOLD);
                ch->gold -= amnt;
                depositok = TRUE;
        }

        if( !str_prefix(arg2, "silver") )
        {
                if (all)
                        amnt = ch->silver;

                if (ch->silver == 0 )
                {
                        sprintf(buf, "%s, you have no silver coins to deposit.", ch->name);
                        do_say(banker, buf);
                        return;
                }

                if (amnt > ch->silver)
                {
                        sprintf(buf, "%s, you do not have %d silver coins to deposit.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }

                if (ch->pcdata->bank + (amnt * COIN_SILVER) > max_balance)
                {
                        sprintf (buf, "Sorry %s, but you may only keep the equivalent of one million "
                                 "platinum in your account.", ch->name);
                        do_say (banker, buf);
                        return;
                }

                ch->pcdata->bank += (amnt * COIN_SILVER);
                ch->silver -= amnt;
                depositok = TRUE;
        }

        if( !str_prefix(arg2, "copper") )
        {
                if (all)
                        amnt = ch->copper;

                if (ch->copper == 0 )
                {
                        sprintf(buf, "%s, you have no copper coins to deposit.", ch->name);
                        do_say(banker, buf);
                        return;
                }

                if (amnt > ch->copper)
                {
                        sprintf(buf, "%s, you do not have %d copper coins to deposit.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }

                if (ch->pcdata->bank + (amnt * COIN_COPPER) > max_balance)
                {
                        sprintf (buf, "Sorry %s, but you may only keep the equivalent of one million "
                                 "platinum in your account.", ch->name);
                        do_say (banker, buf);
                        return;
                }

                ch->pcdata->bank += (amnt * COIN_COPPER);
                ch->copper -= amnt;
                depositok = TRUE;
        }

        if( !depositok )
        {
                sprintf(buf, "I'm sorry %s, but %s is not a valid coin of the realm.",
                        ch->name, arg2);
                do_say(banker, buf);
                return;
        }

        calc_coin_weight(ch);
        sprintf(buf, "Thank you for your custom %s.", ch->name );
        do_say(banker, buf);
        do_balance (ch, " ");

        return;
}


void do_withdraw (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *banker = NULL;
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int amnt;
        bool withdrawok;

        if (IS_NPC(ch))
                return;

        for (banker = ch->in_room->people; banker; banker = banker->next_in_room)
        {
                if (IS_NPC(banker) && IS_SET(banker->pIndexData->act, ACT_BANKER))
                        break;
        }

        if (!banker)
        {
                sprintf(buf, "The banker is currently not available.\n\r");
                send_to_char(buf, ch);
                return;
        }

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);

        if (arg1[0] == '\0' || arg2[0] == '\0')
        {
                sprintf(buf, "How many coins of what denomination do you wish to withdraw?\n\r");
                send_to_char(buf, ch);
                return;
        }

        if (!is_number(arg1) || atoi(arg1) < 1)
        {
                send_to_char("That is not a valid number of coins to withdraw.\n\r", ch);
                return;
        }

        amnt = atoi(arg1);
        withdrawok = FALSE;

        if( !str_prefix(arg2, "platinum") )
        {
                if (ch->pcdata->bank < COIN_PLAT ) {
                        sprintf(buf, "%s, you have no platinum coins to withdraw.", ch->name);
                        do_say(banker, buf);
                        return;
                }
                if (ch->pcdata->bank < (amnt * COIN_PLAT) )
                {
                        sprintf(buf, "%s, you do not have %d platinum coins to withdraw.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }
                ch->pcdata->bank -= (amnt * COIN_PLAT);
                ch->plat += amnt;
                withdrawok = TRUE;
        }

        if( !str_prefix(arg2, "gold") )
        {
                if (ch->pcdata->bank < COIN_GOLD )
                {
                        sprintf(buf, "%s, you have no gold coins to withdraw.", ch->name);
                        do_say(banker, buf);
                        return;
                }
                if (ch->pcdata->bank < (amnt * COIN_GOLD) )
                {
                        sprintf(buf, "%s, you do not have %d gold coins to withdraw.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }
                ch->pcdata->bank -= (amnt * COIN_GOLD);
                ch->gold += amnt;
                withdrawok = TRUE;
        }

        if( !str_prefix(arg2, "silver") )
        {
                if (ch->pcdata->bank < COIN_SILVER )
                {
                        sprintf(buf, "%s, you have no silver coins to withdraw.", ch->name);
                        do_say(banker, buf);
                        return;
                }
                if (ch->pcdata->bank < (amnt * COIN_SILVER) )
                {
                        sprintf(buf, "%s, you do not have %d silver coins to withdraw.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }
                ch->pcdata->bank -= (amnt * COIN_SILVER);
                ch->silver += amnt;
                withdrawok = TRUE;
        }

        if( !str_prefix(arg2, "copper") )
        {
                if (ch->pcdata->bank < COIN_COPPER )
                {
                        sprintf(buf, "%s, you have no copper coins to withdraw.", ch->name);
                        do_say(banker, buf);
                        return;
                }
                if (ch->pcdata->bank < (amnt * COIN_COPPER) )
                {
                        sprintf(buf, "%s, you do not have %d copper coins to withdraw.", ch->name, amnt);
                        do_say(banker, buf);
                        return;
                }
                ch->pcdata->bank -= (amnt * COIN_COPPER);
                ch->copper += amnt;
                withdrawok = TRUE;
        }

        if ( !withdrawok )
        {
                sprintf(buf, "I'm sorry %s, but %s is not a valid coin of the realm.", ch->name, arg2);
                do_say(banker, buf);
                return;
        }

        calc_coin_weight(ch);
        sprintf(buf, "Thank you for your custom %s.", ch->name );
        do_say(banker, buf);
        do_balance (ch, " ");

        return;
}


void do_borrow (CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *banker = NULL;
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        int amnt;

        if (IS_NPC(ch))
                return;

        for (banker = ch->in_room->people; banker; banker = banker->next_in_room)
        {
                if (IS_NPC(banker) && IS_SET(banker->pIndexData->act, ACT_BANKER))
                        break;
        }

        if (!banker)
        {
                sprintf(buf, "There is no banker here.\n\r");
                send_to_char(buf, ch);
                return;
        }

        one_argument(argument, arg);

        if (arg[0] == '\0')
        {
                sprintf(buf, "How many coins do you wish to borrow?\n\r");
                send_to_char(buf, ch);
                return;
        }

        amnt = atoi(arg);

        if ((amnt-ch->pcdata->bank) > ch->level * 2000)
        {
                sprintf(buf, "Your credit limit is %d %s.", ch->level*2000,ch->name);
                do_say(banker, buf);
                return;
        }

        if (amnt - ch->pcdata->bank < 0)
        {
                sprintf(buf, "If you are only borrowing that much, withdraw the coins instead %s.", ch->name);
                do_say(banker, buf);
                return;
        }

        if (amnt < 100)
        {
                sprintf(buf, "You must borrow at least 100 coins %s.", ch->name);
                do_say(banker, buf);
                return;
        }

        coins_to_char(amnt, ch, COINS_ADD);
        ch->pcdata->bank -= amnt;

        sprintf(buf, "%s, you now owe us: %d coins,", ch->name, -ch->pcdata->bank);
        do_say(banker, buf);
        sprintf(buf, "after borrowing: %d coins.", amnt);
        do_say(banker, buf);
        sprintf(buf, "%s, shopkeepers will now give the bank 50 percent of anything you sell.", ch->name);
        do_say(banker, buf);

        return;
}


/*
 * Calculate weight of coins carried by player and add to carry_weight
 */
void calc_coin_weight (CHAR_DATA *ch)
{
        int cw;

        cw = (ch->plat + ch->gold + ch->silver + ch->copper) / 10;
        ch->coin_weight = cw;
}


/* Call with mode where COINS_ADD = 0, all others will replace */
void coins_to_char( int numcoins, CHAR_DATA *ch, int mode )
{
        COIN_DATA * coins;

        coins = (COIN_DATA *) malloc (sizeof(COIN_DATA));
        if (!coins)
        {
                bug ("coins_to_char: COIN_DATA - not enough memory to allocate", 0);
                exit (1);
        }

        coin_crunch(numcoins, coins);

        if (mode == COINS_ADD)
        {
                ch->plat   += coins->plat;
                ch->gold   += coins->gold;
                ch->silver += coins->silver;
                ch->copper += coins->copper;
        }
        else
        {
                ch->plat   = coins->plat;
                ch->gold   = coins->gold;
                ch->silver = coins->silver;
                ch->copper = coins->copper;
        }

        free(coins);

        calc_coin_weight(ch);
        return;
}

void smelted_to_char( int st, int ti, int ad, int el, int sm, CHAR_DATA *ch, int mode )
{
        if (mode == COINS_ADD)
        {
                ch->smelted_steel       += st;
                ch->smelted_titanium    += ti;
                ch->smelted_adamantite  += ad;
                ch->smelted_electrum    += el;
                ch->smelted_starmetal   += sm;
        }
        else
        {
                ch->smelted_steel       -= st;
                ch->smelted_titanium    -= ti;
                ch->smelted_adamantite  -= ad;
                ch->smelted_electrum    -= el;
                ch->smelted_starmetal   -= sm;
        }
    return;
}


/*
 * MAKE SURE that you call total_coins_char BEFORE this function as there is
 * no bounds checking done here
 */
void coins_from_char( int numcoins, CHAR_DATA *ch )
{
        COIN_DATA * coins;
        int holder;

        if (ch->copper >= numcoins)
        {
                ch->copper -= numcoins;
                calc_coin_weight(ch);
                return;
        }
        else
        {
                numcoins -= ch->copper;
                ch->copper = 0;
        }

        coins = (COIN_DATA *) malloc (sizeof(COIN_DATA));
        if (!coins)
        {
                bug ("coins_from_char: COIN_DATA - not enough memory to allocate", 0);
                exit (1);
        }

        /* Process silver coins */
        holder = (ch->silver * COIN_SILVER);
        if (holder >= numcoins)
        {
                holder -= numcoins;
                coin_crunch(holder, coins);
                ch->silver = coins->silver + (coins->gold * 10);
                ch->copper = coins->copper;
                calc_coin_weight(ch);
                free (coins);
                return;
        }
        else
        {
                numcoins -= holder;
                ch->silver = 0;
        }

        /* Process gold coins */
        holder = (ch->gold * COIN_GOLD);
        if (holder >= numcoins)
        {
                holder -= numcoins;
                coin_crunch(holder, coins);
                ch->gold = coins->gold + (coins->plat * 10);
                ch->silver = coins->silver;
                ch->copper = coins->copper;
                calc_coin_weight(ch);
                free (coins);
                return;
        }
        else
        {
                numcoins -= holder;
                ch->gold = 0;
        }

        /* Process plat coins */
        holder = (ch->plat * COIN_PLAT);
        if (holder >= numcoins)
        {
                holder -= numcoins;
                coin_crunch(holder, coins);
                ch->plat = coins->plat;
                ch->gold = coins->gold;
                ch->silver = coins->silver;
                ch->copper = coins->copper;
                calc_coin_weight(ch);
                free (coins);
                return;
        }
        else
        {
                bug("coins_from_char: Called while numcoins < ch->coins.", 0);
                send_to_char("You do not have that many coins!\n\r", ch);
                calc_coin_weight(ch);
                free (coins);
                return;
        }
}


int total_coins_char( CHAR_DATA *ch)
{
        return (ch->plat * COIN_PLAT) + (ch->gold * COIN_GOLD)
                + (ch->silver * COIN_SILVER) + (ch->copper);
}


/*
 *  Get all money objects from a container.
 *  Used to collect coin from corpses when AUTOCOIN config option is set.
 *  Not a regular do_fun; no messages to be given to players upon
 *  errors, etc.   Gezzles 2000
 */
void do_get_coins(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        OBJ_DATA *container;
        OBJ_DATA *obj_next;
        char      arg [ MAX_INPUT_LENGTH ];

        /* Get name of container object */
        argument = one_argument(argument, arg);

        /* Some basic checks */
        if (IS_NPC(ch)
            || IS_AFFECTED(ch, AFF_NON_CORPOREAL)
            || arg[0] == '\0')
                return;

        /* Check we have a valid open container */
        if (!(container = get_obj_here(ch, arg))
            || (container->item_type != ITEM_CONTAINER
                && container->item_type != ITEM_CORPSE_NPC
                && container->item_type != ITEM_REMAINS)
            || IS_SET(container->value[1], CONT_CLOSED))
                return;

        /* Recover coins */
        for (obj = container->contains; obj; obj = obj_next)
        {
                obj_next = obj->next_content;
                if (obj->item_type == ITEM_MONEY && can_see_obj(ch, obj))
                        get_obj(ch, obj, container);
        }
}

/*
 * can_use_poison_weapon(ch)
 *
 * Used to check a pc is evil or a class that can use poisoned weapons
 */
bool can_use_poison_weapon(CHAR_DATA *ch)
{
        if (IS_NPC(ch))
                return TRUE;

        if (IS_EVIL(ch))
                return TRUE;

        if (ch->class == CLASS_THIEF
            || ch->sub_class == SUB_CLASS_THUG
            || ch->sub_class == SUB_CLASS_BOUNTY
            || ch->sub_class == SUB_CLASS_NINJA
            || ch->sub_class == SUB_CLASS_WITCH
            || ch->sub_class == SUB_CLASS_WEREWOLF
            || ch->sub_class == SUB_CLASS_VAMPIRE
            || ch->sub_class == SUB_CLASS_INFERNALIST
            || ch->sub_class == SUB_CLASS_NECROMANCER)
                return TRUE;

        if (ch->form == FORM_SPIDER
            || ch->form == FORM_SNAKE
            || ch->form == FORM_SCORPION)
                return TRUE;

        return FALSE;
}


/*
 *  Object ownership; Shade and Gezhp 2000
 */

char* get_obj_owner (OBJ_DATA *obj)
{
        if (!obj->owner)
                return "";

        return obj->owner;
}


void set_obj_owner (OBJ_DATA *obj, char *owner)
{
        if (!owner)
                obj->owner[0] = '\0';

        strncpy(obj->owner, owner, 30);
}

/*
 * function returns FALSE if obj has owner and owner isn't ch
 */
bool is_obj_owner (OBJ_DATA *obj, CHAR_DATA *ch)
{
        if (!obj->owner
            || obj->owner[0] == '\0'
            || ch->level > LEVEL_HERO)
                return TRUE;

        if (IS_NPC(ch))
                return FALSE;

        if (!str_cmp(get_obj_owner(obj), ch->name))
                return TRUE;

        return FALSE;
}


void destroy_clan_items (CHAR_DATA *ch)
{
        OBJ_DATA *obj, *obj_next;
        bool found = FALSE;
        int clan;

        for (obj = ch->carrying; obj; obj = obj_next)
        {
                obj_next = obj->next_content;

                if (obj->deleted)
                        continue;

                for (clan = 0; clan < MAX_CLAN; clan++)
                {
                        if (obj->pIndexData->vnum == clan_item_list[clan].first_item
                            || obj->pIndexData->vnum == clan_item_list[clan].second_item
                            || obj->pIndexData->vnum == clan_item_list[clan].third_item
                            || obj->pIndexData->vnum == clan_item_list[clan].pouch_item)
                        {
                                found = TRUE;

                                if (obj->wear_loc != WEAR_NONE)
                                        unequip_char(ch, obj);

                                obj_from_char(obj);
                                extract_obj(obj);
                        }
                }
        }

        if (found)
                send_to_char("Your clan items disappear in a flash of light!\n\r", ch);
}


bool has_ego_item_effect (CHAR_DATA *ch, int flag)
{
        OBJ_DATA *obj;

        if (flag < 1)
                return FALSE;

        for (obj = ch->carrying; obj; obj = obj->next_content)
        {
                if (obj->deleted || obj->wear_loc == WEAR_NONE)
                        continue;

                if (IS_SET(obj->extra_flags, ITEM_EGO)
                    && IS_SET(obj->ego_flags, flag))
                        return TRUE;
        }

        return FALSE;
}


void player_leaves_clan (CHAR_DATA *ch)
{
        /*
         *  Tidy up all clan related variables
         */

        if (IS_NPC(ch) || !ch->clan)
                return;

        SET_BIT(ch->status, PLR_LEFT_CLAN);
        REMOVE_BIT(ch->act, PLR_LEADER);
        REMOVE_BIT(ch->status, PLR_HUNTED);
        ch->clan = 0;
        ch->clan_level = 0;
        ch->pcdata->bounty = 0;
        ch->pcdata->recall_points[CLAN_RECALL] = -1;
        destroy_clan_items(ch);
        remove_from_wanted_table(ch->name);
}

int get_craft_obj_bonus(CHAR_DATA *ch)
{
        /*
         * Check if there is a crafting object in the room ch is in, if so
         * check its bonus value and return it. Shouldn't be multiple in a
         * room, so only return value of first object found.
         */

        OBJ_DATA *obj;

        int craft_bonus;
        craft_bonus = 0;

        for (obj = ch->in_room->contents; obj; obj = obj->next_content)
        {
                if (obj->item_type == ITEM_CRAFT )
                {
                        craft_bonus = obj->value[0];
                        return craft_bonus;
                }
        }

        return craft_bonus;
}

int get_spellcraft_obj_bonus(CHAR_DATA *ch)
{
        /*
         * Check if there is a crafting object in the room ch is in, if so
         * check its bonus value and return it. Shouldn't be multiple in a
         * room, so only return value of first object found.
         */

        OBJ_DATA *obj;

        int spellcraft_bonus;
        spellcraft_bonus = 0;

        for (obj = ch->in_room->contents; obj; obj = obj->next_content)
        {
                if (obj->item_type == ITEM_SPELLCRAFT )
                {
                        spellcraft_bonus = obj->value[0];
                        return spellcraft_bonus;
                }
        }

        return spellcraft_bonus;
}


/*
 * Auction betting bollocks
 */
/*
int advatoi (const char *s)
{
        char string[MAX_INPUT_LENGTH];
        char *stringptr = string;
        char tempstring[2];
        int number = 0;
        int multiplier = 0;

        strcpy (string,s);

        while ( isdigit (*stringptr))
        {
                strncpy (tempstring,stringptr,1);
                number = (number * 10) + atoi (tempstring);
                stringptr++;
        }

        switch (UPPER(*stringptr))
        {
            case 'K'  : multiplier = 1000;    number *= multiplier; stringptr++; break;
            case 'M'  : multiplier = 1000000; number *= multiplier; stringptr++; break;
            case '\0' : break;
            default   : return 0;
        }

        while ( isdigit (*stringptr) && (multiplier > 1))
        {
                strncpy (tempstring, stringptr, 1);
                multiplier = multiplier / 10;
                number = number + (atoi (tempstring) * multiplier);
                stringptr++;
        }

        if (*stringptr != '\0' && !isdigit(*stringptr))
                return 0;

        return (number);
}
*/

int parsebet (const int currentbet, const char *argument)
{
        int newbet = 0;
        char string[MAX_INPUT_LENGTH];
        char *stringptr = string;

        strcpy (string,argument);

        if (*stringptr)
        {
                if (isdigit (*stringptr))
                        newbet = advatoi (stringptr);

                else if (*stringptr == '+')
                {
                        if (strlen (stringptr) == 1)
                                newbet = (currentbet * 125) / 100;
                        else
                                newbet = (currentbet * (100 + atoi (++stringptr))) / 100;
                }
                else
                {
                        if ((*stringptr == '*') || (*stringptr == 'x'))
                        {
                                if (strlen (stringptr) == 1)
                                        newbet = currentbet * 2;
                                else
                                        newbet = currentbet * atoi (++stringptr);
                        }
                }
        }

        return newbet;
}


int random_qnd ( int ap_value, char *rank, int ap_type )
{
        /*

        Given an APPLY type from an item, this will transform it using a quasi-standard
        normal (but 3 SD hard-bounded) distribution, affected by the mobrank of a mob if
        it's loaded on a mob (assumes 'common' if not).  Used to randomise item statistics
        for items with the ITEM_WEAK_RANDOMISE/"steady" flag.

        If item has the ITEM_DONOT_RANDOMISE flag on it its apply values shouldn't be
        changed at all.  This should be checked by the calling function, not this one, which
        will assume you've checked before calling it.

        --Owl 1/1/23

        */

        float changed_ap_value = ap_value;

        float minus_3_lb;
        float minus_3_ub;
        float minus_2_lb;
        float minus_2_ub;
        float minus_1_lb;
        float plus_1_ub;
        float plus_2_lb;
        float plus_2_ub;
        float plus_3_lb;
        float plus_3_ub;
        float sd_divisor = 12.0;
        float av_sd = ap_value / sd_divisor;
        float bonus_from_rank = 0.0;
        float r;

        /* indicates whether a negative value for an APPLY is of benefit to a player or not */
        bool negative_benefits = false;

        char buf[MAX_STRING_LENGTH];

        int r_100 = rand() % 100 + 1;

        if (ap_type > APPLY_LAST)
        {
                sprintf(buf,"APPLY type (%d) supplied to random_qnd() function is > APPLY_LAST (%d). Edit function if APPLY type is valid.",
                        ap_type,
                        APPLY_LAST);
                log_string(buf);
                return ap_value;
        }

        /*
           Set the upper and lower boundary values for our distribution.
           See image at: http://dragons-domain.org/main/building/snd.png to understand
           what is happening here.
        */

        /* 2% chance, r_100 = 1 or 2 */
        minus_3_lb = ( ap_value - ( 3 * av_sd ) );
        minus_3_ub = ( ap_value - ( 2 * av_sd ) );

        /* 14% chance, r_100 = 3 to 16 */
        minus_2_lb = ( ap_value - ( 2 * av_sd ) + 0.000001 );
        minus_2_ub = ( ap_value - av_sd );

        /* 68% chance, r_100 = 17 to 84 */
        minus_1_lb = ( ap_value - av_sd + 0.000001 );
        plus_1_ub  = ( ap_value + av_sd );

        /* 14% chance, r_100 = 85 to 98 */
        plus_2_lb  = (ap_value + av_sd + 0.000001 );
        plus_2_ub  = (ap_value + ( 2 * av_sd ) );

        /* 2% chance, r_100 = 99 or 100 */
        plus_3_lb  = (ap_value + ( 2 * av_sd ) + 0.000001 );
        plus_3_ub  = (ap_value + ( 3 * av_sd ) );

        /* sprintf(buf,"r_100 is: %d", r_100);
        log_string(buf); */

        if ( r_100 == 1 || r_100 == 2 )
        {
                r = (float)rand() / (float)RAND_MAX;
                changed_ap_value = minus_3_lb + r * (minus_3_ub - minus_3_lb);
                /* printf("return ap value for -3SD between %f and %f: %f\n", minus_3_lb, minus_3_ub, changed_ap_value); */
        }

        if ( r_100 > 2 && r_100 < 17 )
        {
                r = (float)rand() / (float)RAND_MAX;
                changed_ap_value = minus_2_lb + r * (minus_2_ub - minus_2_lb);
                /* printf("return ap value for -2SD between %f and %f: %f\n", minus_2_lb, minus_2_ub, changed_ap_value); */
        }

        if ( r_100 > 16 && r_100 < 85 )
        {
                r = (float)rand() / (float)RAND_MAX;
                changed_ap_value = minus_1_lb + r * (plus_1_ub - minus_1_lb);
                /* printf("return ap value for -1SD/+1SD between %f and %f: %f\n", minus_1_lb, plus_1_ub, changed_ap_value); */
        }

        if ( r_100 > 84 && r_100 < 99 )
        {
                r = (float)rand() / (float)RAND_MAX;
                changed_ap_value = plus_2_lb + r * (plus_2_ub - plus_2_lb);
                /* printf("return ap value for +2SD between %f and %f: %f\n", plus_2_lb, plus_2_ub, changed_ap_value); */
        }

        if ( r_100 == 99 || r_100 == 100 )
        {
                r = (float)rand() / (float)RAND_MAX;
                changed_ap_value = plus_3_lb + r * (plus_3_ub - plus_3_lb);
                /* printf("return ap value for +3SD between %f and %f: %f\n", plus_3_lb, plus_3_ub, changed_ap_value); */
        }

        /* sprintf(buf,"ap_type: %d | ap_value: %d | changed_ap_value: %f | rank: %s | minus_3_lb: %f | minus_3_ub: %f | minus_2_lb: %f | minus_2_ub: %f | minus_1_lb: %f | plus_1_ub: %f | plus_2_lb: %f | plus_2_ub: %f | plus_3_lb: %f | plus_3_ub: %f |",
                ap_type,
                ap_value,
                changed_ap_value,
                rank,
                minus_3_lb,
                minus_3_ub,
                minus_2_lb,
                minus_2_ub,
                minus_1_lb,
                plus_1_ub,
                plus_2_lb,
                plus_2_ub,
                plus_3_lb,
                plus_3_ub
        );
        log_string(buf); */

        /* sprintf(buf, "Passed: ap_value: %d rank: %d ap_type: %d \n\r", ap_value, rank, ap_type);
        send_to_char(buf, ch); */

         /*
                ap_value is the base apply value from the area file. So might be 3 for APPLY_STRENGTH
                on an item or whatever.  May have to investigate types to see if some are prohibited
                from being certain sizes or negative etc.
        */

        switch (ap_type)
        {
            case APPLY_STR:
                break;

            case APPLY_DEX:
                break;

            case APPLY_INT:
                break;

            case APPLY_WIS:
                break;

            case APPLY_CON:
                break;

            case APPLY_MANA:
                break;

            case APPLY_HIT:
                break;

            case APPLY_MOVE:
                break;

            case APPLY_AC:
                negative_benefits = true;
                break;

            case APPLY_HITROLL:
                break;

            case APPLY_DAMROLL:
                break;

            case APPLY_SAVING_PARA:
                negative_benefits = true;
                break;

            case APPLY_SAVING_ROD:
                negative_benefits = true;
                break;

            case APPLY_SAVING_PETRI:
                negative_benefits = true;
                break;

            case APPLY_SAVING_BREATH:
                negative_benefits = true;
                break;

            case APPLY_SAVING_SPELL:
                negative_benefits = true;
                break;

            case APPLY_NONE:
            case APPLY_SANCTUARY:
            case APPLY_SNEAK:
            case APPLY_FLY:
            case APPLY_INVIS:
            case APPLY_DETECT_INVIS:
            case APPLY_DETECT_HIDDEN:
            case APPLY_FLAMING:
            case APPLY_PROTECT:
            case APPLY_PASS_DOOR:
            case APPLY_GLOBE:
            case APPLY_DRAGON_AURA:
            case APPLY_BREATHE_WATER:
            case APPLY_SET_UNCOMMON:
            case APPLY_SET_RARE:
            case APPLY_SET_EPIC:
            case APPLY_SET_LEGENDARY:
            case APPLY_INSCRIBED:
            case APPLY_BALANCE:
            case APPLY_ENGRAVED:
            case APPLY_STRENGTHEN:
            case APPLY_SERRATED:
            case APPLY_SEX:
            case APPLY_CLASS:
            case APPLY_LEVEL:
            case APPLY_AGE:
            case APPLY_HEIGHT:
            case APPLY_WEIGHT:
            case APPLY_GOLD:
            case APPLY_EXP:
                return ap_value;

            case APPLY_RESIST_HEAT:
                break;

            case APPLY_RESIST_COLD:
                break;

            case APPLY_RESIST_LIGHTNING:
                break;

            case APPLY_RESIST_ACID:
                break;

            case APPLY_CRIT:
                break;

            case APPLY_SWIFTNESS:
                break;
        }

        /*
                'ranks' possible are in rank_table in mob.c  Currently 5 is max.

                         name, rank_bonus, hp_bonus, who_format
                        { "none",       1,      1,      "{WCommon.{x "},
                        { "common",     1,      1,      "{WCommon.{x "},
                        { "rare",       3,      2,      "<39>[Rare]<0> "},
                        { "elite",      3,      5,      "<93>[Elite]<0> "},
                        { "boss",       4,      7,      "<514><556><16>[<560>BOSS<561>]<0><557> "},
                        { "world",      5,     30,      "<81>[WO<75>RL<69>D B<75>OS<81>S]<0> "}

                See mob.c.
                We assume common  if something not here is passed.
                Tweak this if you add more ranks.

        */

        if ( str_cmp(rank, "none") && str_cmp(rank, "common"))
        {

                 if (negative_benefits)
                 {
                        bonus_from_rank = -( rank_bonus(rank) * fabs(av_sd));
                        /* sprintf(buf,"Rank bonus is: %f with negative benefit YES and av_sd: %f",bonus_from_rank, av_sd);
                        log_string(buf); */
                 }
                 else {
                        bonus_from_rank = ( rank_bonus(rank) * fabs(av_sd));
                        /* sprintf(buf,"Rank bonus is: %f with negative benefit NO and av_sd: %f",bonus_from_rank, av_sd);
                        log_string(buf); */
                 }
        }
        /* round and convert back to int before returning  etc*/
        /* should also do mob level stuff here, as currently assuming common */
        /* sprintf(buf,"Pre-rounding value of changed_ap_value is: %f",
                changed_ap_value);
        log_string(buf); */

        /* sprintf(buf,"Rank bonus is: %f, based on a passed mob rank of %s, av_sd of %f, negative_benefits value of %d and apply type %d",
                bonus_from_rank, rank, av_sd, negative_benefits, ap_type);
        log_string(buf); */

        changed_ap_value = (changed_ap_value + bonus_from_rank);

        /* sprintf(buf,"Pre-rounding value of changed_ap_value with rank bonus is: %f",
                changed_ap_value);
        log_string(buf); */

        ap_value = round(changed_ap_value);

        /* sprintf(buf,"Post-rounding value of changed_ap_value is: %d",
                ap_value);
        log_string(buf); */
        return ap_value;


}

