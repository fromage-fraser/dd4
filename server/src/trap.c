#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 *  trap.c
 *
 *  Written by Helix of Twilight and Vego Mud
 *
 *  Tidied by Gezhp 2000
 */


void trapdamage args( ( CHAR_DATA *ch, OBJ_DATA *obj) );

void trap_damage_message (CHAR_DATA *ch, int dam);


void do_trapremove(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char arg1[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg1);

        if ( ( obj = get_obj_here(ch, arg1) ) == NULL)  {
                send_to_char("That isn't here!\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_TRAP) )
                REMOVE_BIT(obj->extra_flags, ITEM_TRAP);

        obj->trap_dam = 0;
        obj->trap_eff = 0;
        obj->trap_charge = 0;
        send_to_char("Trap removed.\n\r", ch);
        return;
}


void do_disable(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char arg1[MAX_INPUT_LENGTH];
        int chance;

        if ( IS_NPC( ch ) )
                return;

        argument = one_argument(argument, arg1);

        if ( ( obj = get_obj_here(ch, arg1) ) == NULL) {
                send_to_char("That isn't here!\n\r", ch);
                return;
        }

        if ( ch->fighting ) {
                send_to_char( "You can't break off your fight.\n\r", ch );
                return;
        }

        if (!IS_SET(obj->extra_flags, ITEM_TRAP))
        {
                send_to_char("That doesn't appear to be trapped.\n\r", ch);
                return;
        }

        chance = ch->pcdata->learned[gsn_disable]/2;

        if (IS_AFFECTED(ch, AFF_ARM_TRAUMA))
                chance = ((chance * 2) / 3);

        if ( !IS_NPC( ch ) && get_curr_dex( ch ) > 22 )
                chance += 10;
        if (  get_curr_int( ch ) > 19 )
                chance += 10;

        if ( number_percent( ) > chance ) {
                send_to_char("As you begin to disable the trap you slip, and hear a faint *click*.\n\r",ch);
                damage( ch, ch, ch->level*4,gsn_disable, FALSE);
                return;
        }

        REMOVE_BIT(obj->extra_flags, ITEM_TRAP);

        obj->trap_dam = 0;
        obj->trap_eff = 0;
        obj->trap_charge = 0;
        send_to_char("With a steady hand, you deftly remove the triggering device from the object.\n\r", ch);
        return;
}


void do_trapstat(CHAR_DATA *ch, char *argument)
{
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        argument = one_argument(argument, arg1);

        if (( obj = get_obj_here(ch, arg1) ) == NULL) {
                send_to_char("That is not here!\n\r", ch);
                return;
        }

        if (IS_SET(obj->extra_flags, ITEM_TRAP) ) {
                send_to_char("Object status: {Rtrapped{x\n\r", ch);
        }
        else {
                send_to_char("Object status: {Rnot trapped{x\n\r", ch);
                return;
        }

        switch (obj->trap_dam) {
            case TRAP_DAM_SLEEP:
                send_to_char("Damage type: {Gsleep{x\n\r", ch);
                break;
            case TRAP_DAM_TELEPORT:
                send_to_char("Damage type: {Gteleport{x\n\r", ch);
                break;
            case TRAP_DAM_FIRE:
                send_to_char("Damage type: {Gfire{x\n\r", ch);
                break;
            case TRAP_DAM_COLD:
                send_to_char("Damage type: {Gfrost{x\n\r", ch);
                break;
            case TRAP_DAM_ACID:
                send_to_char("Damage type: {Gacid{x\n\r", ch);
                break;
            case TRAP_DAM_ENERGY:
                send_to_char("Damage type: {Genergy{x\n\r", ch);
                break;
            case TRAP_DAM_BLUNT:
                send_to_char("Damage type: {Gblunt{x\n\r", ch);
                break;
            case TRAP_DAM_PIERCE:
                send_to_char("Damage type: {Gpierce{x\n\r", ch);
                break;
            case TRAP_DAM_SLASH:
                send_to_char("Damage type: {Gslash{x\n\r", ch);
                break;
            case TRAP_DAM_POISON:
                send_to_char("Damage type: {Gpoison{x\n\r", ch );
                break;
            case TRAP_DAM_SNARE:
                send_to_char("Damage type: {Gsnare{x\n\r", ch );
                break;
            case TRAP_DAM_CURSE:
                send_to_char("Damage type: {Gcurse{x\n\r", ch );
                break;
            case TRAP_DAM_HEX:
                send_to_char("Damage type: {Ghex{x\n\r", ch );
                break;
            case TRAP_DAM_SPIRIT:
                send_to_char("Damage type: {Gspirit{x\n\r", ch );
                break;
        }

        if (obj->trap_eff == 0) {
                send_to_char("The trap has no effect.\n\r", ch);
        }
        else {
                if (IS_SET(obj->trap_eff, TRAP_EFF_MOVE) ) {
                        if (IS_SET(obj->trap_eff, TRAP_EFF_NORTH) )
                                send_to_char("Trap affects movement {Rnorth{x\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) )
                                send_to_char("Trap affects movement {Rsouth{x\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_EAST) )
                                send_to_char("Trap affects movement {Reast{x\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_WEST) )
                                send_to_char("Trap affects movement {Rwest{x\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_UP) )
                                send_to_char("Trap affects movement {Rup{x\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_DOWN) )
                                send_to_char("Trap affects movement {Rdown{x\n\r", ch);
                }

                if (IS_SET(obj->trap_eff, TRAP_EFF_OBJECT) )
                        send_to_char("Trigger: {Gget / put{x\n\r", ch);

                if (IS_SET(obj->trap_eff, TRAP_EFF_OPEN) )
                        send_to_char("Trigger: {Gopen{x\n\r", ch);

                if (IS_SET(obj->trap_eff, TRAP_EFF_ROOM) )
                        send_to_char("Trap effect: {Groom{x\n\r", ch);
        }

        sprintf (buf, "Charges remaining: {R%d{x\n\r", obj->trap_charge);
        send_to_char(buf, ch);
        return;
}


void do_traplist(CHAR_DATA *ch, char *argument)
{
        char buf[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;
        OBJ_DATA *in_obj;
        bool found;

        found = FALSE;

        for ( obj = object_list; obj != NULL; obj = obj->next ) {
                if ( !can_see_obj( ch, obj ) || !IS_SET(obj->extra_flags, ITEM_TRAP) )
                        continue;

                found = TRUE;

                for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
                        ;

                if ( in_obj->carried_by != NULL ) {
                        sprintf( buf, "{g%s{x carried by {W%s{x\n\r",
                                obj->short_descr, PERS(in_obj->carried_by, ch) );
                }
                else if ( in_obj->vaulted_by != NULL ) {
                        sprintf( buf, "{g%s{x in {W%s's vault{x\n\r",
                                obj->short_descr, PERS(in_obj->vaulted_by, ch) );
                }
                else if ( in_obj->in_room != NULL ) {
                        sprintf( buf, "{g%s{x in {W%s [{R%d{x{W]{x\n\r",
                                obj->short_descr,
                                in_obj->in_room->name,
                                in_obj->in_room->vnum);
                }
                else {
                        sprintf( buf, "{g%s{x in {Wsomewhere{x\n\r",
                                obj->short_descr );
                }

                buf[0] = UPPER(buf[0]);
                send_to_char( buf, ch );
        }

        if ( !found )
                send_to_char( "No traps found.\n\r", ch );

        return;
}


void do_trapset(CHAR_DATA *ch, char *argument)
{
        OBJ_DATA *obj;
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char arg3[MAX_INPUT_LENGTH];
        int val = 0;

        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);
        argument = one_argument(argument, arg3);

        if (arg1[0] == '\0' || arg2[0] == '\0' ) {
                send_to_char("Syntax: trapset <<object> <<field> <<value>\n\r\n\r", ch);
                send_to_char("Field: move, object(get and put), room, open, damage, charge\n\r\n\r", ch);
                send_to_char("Values: Move> north, south, east, west, up, down, and all.\n\r\n\r", ch);
                send_to_char("        Damage> sleep, teleport, fire, cold, acid, energy,\n\r", ch);
                send_to_char("                blunt, pierce, slash, poison, snare, curse, hex.\n\r\n\r", ch);
                send_to_char("        Object, open, room> no values\n\r", ch);
                return;
        }

        if ( ( obj = get_obj_here(ch, arg1) ) == NULL) {
                send_to_char("Nothing like that here!\n\r", ch);
                return;
        }

        if (!IS_SET(obj->extra_flags, ITEM_TRAP) )
                SET_BIT(obj->extra_flags, ITEM_TRAP);

        if (!str_cmp( arg2, "move") ) {
                if (arg3[0] == '\0') {
                        send_to_char("Syntax: trapset <<object> <<field> <<value>\n\r\n\r", ch);
                        send_to_char("Field: move, object(get and put), room, open, damage\n\r\n\r", ch);
                        send_to_char("Values: Move> north, south, east, west, up, down, and all.\n\r", ch);
                        send_to_char("        Damage> sleep, teleport, fire, cold, acid, energy,\n\r", ch);
                        send_to_char("                blunt, pierce, slash, poison, snare, curse, hex.\n\r\n\r", ch);
                        send_to_char("        Object, open, room> no values\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "north") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_NORTH) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_NORTH);
                        send_to_char("You set a trap for northward movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "south") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_SOUTH);
                        send_to_char("You set a trap for southern movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "east") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_EAST) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_EAST);
                        send_to_char("You set a trap for eastern movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "west") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_WEST) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_WEST);
                        send_to_char("You set a trap for western movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "up") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_UP) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_UP);
                        send_to_char("You set a trap for upward movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "down") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_DOWN) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_DOWN);
                        send_to_char("You set a trap for downward movement!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "all") ) {
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_MOVE) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_MOVE);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_DOWN) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_DOWN);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_UP) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_UP);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_EAST) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_EAST);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_WEST) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_WEST);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_NORTH) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_NORTH);
                        if (!IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) )
                                SET_BIT(obj->trap_eff, TRAP_EFF_SOUTH);
                        send_to_char("You set a trap for all movement!\n\r", ch);
                              return;
                }

                send_to_char("Value specified was not a valid option for move.\n\r", ch);
                return;
        }

        if (!str_cmp(arg2, "object") ) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_OBJECT) )
                        SET_BIT(obj->trap_eff, TRAP_EFF_OBJECT);
                send_to_char("You have set an object (get or put) trap!\n\r", ch);
                return;
        }

        if ( !str_cmp(arg2, "room") ) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) )
                        SET_BIT(obj->trap_eff, TRAP_EFF_ROOM);
                send_to_char("You have made the trap affect the whole room!\n\r", ch);
                return;
        }

        if ( !str_cmp(arg2, "open") ) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_OPEN) )
                        SET_BIT(obj->trap_eff, TRAP_EFF_OPEN);
                send_to_char("You have made the trap spring when opened!\n\r", ch);
                return;
        }

        if ( !str_cmp(arg2, "damage") ) {
                if (arg3[0] == '\0') {
                        send_to_char("You need to specify a value!\n\r", ch);
                        return;
                }
                if (!str_cmp(arg3, "sleep") ) {
                        obj->trap_dam = TRAP_DAM_SLEEP;
                        send_to_char("You have made the trap put people to sleep!\n\r", ch);
                        return;
                }
                if (!str_cmp(arg3, "teleport") ) {
                        obj->trap_dam = TRAP_DAM_TELEPORT;
                        send_to_char("The trap will now teleport people!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "fire") ) {
                        obj->trap_dam = TRAP_DAM_FIRE;
                        send_to_char("The trap will now shoot fire!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "cold") ) {
                        obj->trap_dam = TRAP_DAM_COLD;
                        send_to_char("The trap will now freeze its victims!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "acid") ) {
                        obj->trap_dam = TRAP_DAM_ACID;
                        send_to_char("You carefully fill the trap with acid!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "energy") ) {
                        obj->trap_dam = TRAP_DAM_ENERGY;
                        send_to_char("The trap will now zap people with energy!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "blunt") ) {
                        obj->trap_dam = TRAP_DAM_BLUNT;
                        send_to_char("The trap will now bludgeon its victims!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "pierce") ) {
                        obj->trap_dam = TRAP_DAM_PIERCE;
                        send_to_char("The trap will now pierce its victims!\n\r", ch);
                        return;
                }

                if (!str_cmp(arg3, "slash") ) {
                        obj->trap_dam = TRAP_DAM_SLASH;
                        send_to_char("The trap will now slash its victims!\n\r", ch);
                        return;
                }

                if ( !str_cmp(arg3, "poison") ) {
                        obj->trap_dam = TRAP_DAM_POISON;
                        send_to_char("The trap will now poison its victims!\n\r", ch );
                        return;
                }

                if ( !str_cmp(arg3, "snare") ) {
                        obj->trap_dam = TRAP_DAM_SNARE;
                        send_to_char("The trap will now ensnare its victims!\n\r", ch );
                        return;
                }

                if ( !str_cmp(arg3, "curse") ) {
                        obj->trap_dam = TRAP_DAM_CURSE;
                        send_to_char("The trap will now curse its victims!\n\r", ch );
                        return;
                }

                if ( !str_cmp(arg3, "hex") ) {
                        obj->trap_dam = TRAP_DAM_HEX;
                        send_to_char("The trap will now hex its victims!\n\r", ch );
                        return;
                }

                if ( !str_cmp(arg3, "spirit") ) {
                        obj->trap_dam = TRAP_DAM_SPIRIT;
                        send_to_char("The trap will now summon a spirit guardian!\n\r", ch );
                        return;
                }

                send_to_char("That is not a permitted value!\n\r", ch);
                return;
        }

        if (!str_cmp(arg2, "charge") ) {
                if (arg3[0] == '\0') {
                        send_to_char("You need to specify how many charges.\n\r", ch);
                        return;
                }
                if ( ( val = atoi(arg3) ) > 100 || val < 0) {
                        send_to_char("Current allowed range is 1 to 100.\n\r", ch);
                        return;
                }
                obj->trap_charge = val;
                send_to_char("Charge value set.\n\r", ch);
                return;
        }

        send_to_char("That is not a permitted option!\n\r", ch);
        return;
}


bool checkmovetrap(CHAR_DATA *ch, int dir)
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        bool found;

        if ( IS_NPC(ch) )
                return FALSE;

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
                obj_next = obj->next_content;

                if( IS_SET(obj->extra_flags, ITEM_TRAP)
                   && IS_SET(obj->trap_eff, TRAP_EFF_MOVE)
                   && obj->trap_charge > 0)
                        found = TRUE;
                else
                        found = FALSE;

                if (found == TRUE) {
                        if (IS_SET(obj->trap_eff, TRAP_EFF_NORTH) && dir == 0) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_EAST) && dir == 1) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) && dir == 2) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_WEST) && dir == 3) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_UP) && dir == 4) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_DOWN) && dir == 5) {
                                trapdamage(ch, obj);
                                return TRUE;
                        }
                }
        }
        return FALSE;
}


bool checkgetput(CHAR_DATA *ch, OBJ_DATA *obj)
{
        if (IS_NPC(ch) )
                return FALSE;

        if (!IS_SET(obj->extra_flags, ITEM_TRAP) )
                return FALSE;

        if (IS_SET(obj->trap_eff, TRAP_EFF_OBJECT) && obj->trap_charge > 0) {
                trapdamage(ch, obj);
                return TRUE;
        }
        return FALSE;
}


bool checkopen(CHAR_DATA *ch, OBJ_DATA *obj)
{
        if (IS_NPC(ch) )
                return FALSE;

        if (!IS_SET(obj->extra_flags, ITEM_TRAP) )
                return FALSE;

        if (IS_SET(obj->trap_eff, TRAP_EFF_OPEN) && obj->trap_charge > 0) {
                trapdamage(ch, obj);
                return TRUE;
        }
        return FALSE;
}


/* Finish each sleep victim before a fall can move or extract either partner. */
static void sleep_trap_victim(CHAR_DATA *ch, int level, bool room_trap)
{
        AFFECT_DATA af;
        bool already_asleep;

        if (!ch || ch->deleted || !ch->in_room || ch->position == POS_DEAD)
                return;

        already_asleep = IS_AFFECTED(ch, AFF_SLEEP) != 0;
        if (already_asleep && !room_trap)
                return;

        if (!already_asleep)
        {
                af.type = room_trap ? gsn_trap : gsn_sleep;
                af.duration = 4 + (room_trap
                    ? number_range(level / 3, level)
                    : number_range(level / 6, level / 4));
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_SLEEP;
                affect_join(ch, &af);

                if (IS_AWAKE(ch))
                        send_to_char("You feel very sleepy...\n\r", ch);
        }

        /* Preserve the reveal from the old zero-damage trap tail. */
        if (IS_AFFECTED(ch, AFF_INVISIBLE))
        {
                affect_strip(ch, gsn_invis);
                affect_strip(ch, gsn_mass_invis);
                affect_strip_raw_bit(ch, AFF_INVISIBLE);
                if (!IS_AFFECTED(ch, AFF_INVISIBLE))
                        act("$n fades into existence.", ch, NULL, NULL,
                            TO_ROOM);
        }

        /* Traps have no character attacker to receive fall credit. */
        if (!already_asleep)
                force_sleep(ch, NULL);
}


/*
 * A teleport trap moves one traveller or riding pair at a time.
 */
static void teleport_trap_victim(CHAR_DATA *victim, bool room_trap,
                                 bool combat_blocked)
{
        CHAR_DATA *companion = NULL;
        CHAR_DATA *mount = NULL;
        CHAR_DATA *rider = NULL;
        CHAR_DATA *rch;
        ROOM_INDEX_DATA *source;
        ROOM_INDEX_DATA *destination = NULL;
        bool pair_ready = FALSE;
        int attempts;
        const char *failure = room_trap
                ? "Wow, that was close... a trap was set off and malfunctioned!\n\r"
                : "Wow, that was close... you set off a trap and it malfunctioned!\n\r";

        if (!victim || victim->deleted || !victim->in_room
        ||  victim->position == POS_DEAD)
                return;

        source = victim->in_room;

        /* A mount can set off the trap too. Take its rider along. */
        if (victim->rider || victim->mount || is_affected(victim, gsn_mount))
        {
                if (victim->rider)
                {
                        mount = victim;
                        rider = victim->rider;
                        companion = rider;
                }
                else
                {
                        rider = victim;
                        mount = victim->mount;
                        companion = mount;
                }

                /* Find the partner before following a possibly stale link. */
                for (rch = source->people; rch; rch = rch->next_in_room)
                {
                        if (rch == companion)
                                break;
                }

                if (!rch || rch->deleted || rch->position == POS_DEAD
                ||  companion == victim || companion->in_room != source
                ||  !IS_NPC(mount) || IS_NPC(rider)
                ||  rider->mount != mount || mount->rider != rider
                ||  mount->mount || rider->rider
                ||  !is_affected(rider, gsn_mount))
                        goto failed;

                pair_ready = TRUE;
        }

        if (IS_SET(source->room_flags, ROOM_NO_RECALL)
        ||  combat_blocked
        ||  (!IS_NPC(victim) && victim->fighting)
        ||  (companion && !IS_NPC(companion) && companion->fighting))
                goto failed;

        /* Keep the old destination rules, but give the search a limit. */
        for (attempts = 0; attempts < 10000; attempts++)
        {
                destination = get_room_index(number_range(0, 65535));

                if (!destination
                ||  IS_SET(destination->room_flags, ROOM_PRIVATE)
                ||  IS_SET(destination->room_flags, ROOM_SOLITARY))
                        continue;

                /* A same-room refresh is not a new arrival. */
                if (destination != source
                &&  (!undead_can_travel_to_room(victim, destination)
                     || (mount
                         && IS_SET(destination->room_flags, ROOM_NO_MOUNT))))
                        continue;

                break;
        }

        if (attempts == 10000)
                goto failed;

        act("$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM);
        if (companion)
                act("$n slowly fades out of existence.", companion, NULL, NULL, TO_ROOM);

        stop_fighting(victim, TRUE);
        if (companion)
                stop_fighting(companion, TRUE);

        GMCPSetArrival(victim->desc, source->vnum, destination->vnum, -1, "forced");
        if (companion)
                GMCPSetArrival(companion->desc, source->vnum, destination->vnum, -1, "forced");

        char_from_room(victim);
        char_to_room(victim, destination);
        if (companion)
        {
                char_from_room(companion);
                char_to_room(companion, destination);
        }

        act("$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM);
        if (companion)
                act("$n slowly fades into existence.", companion, NULL, NULL, TO_ROOM);

        do_look(victim, "auto");
        if (companion)
                do_look(companion, "auto");
        return;

failed:
        send_to_char(failure, victim);
        if (pair_ready)
                send_to_char(failure, companion);
}

void trapdamage(CHAR_DATA *ch, OBJ_DATA *obj)
{
        AFFECT_DATA af;
        CHAR_DATA *wch;
        int dam = 0;
        int deaths = action_deaths(ch);
        ROOM_INDEX_DATA *room;
        bool physical_damage;

        if (!obj || obj->deleted || !action_character_alive(ch, deaths))
                return;
        room = ch->in_room;
        physical_damage = obj->trap_dam == TRAP_DAM_BLUNT
                || obj->trap_dam == TRAP_DAM_PIERCE
                || obj->trap_dam == TRAP_DAM_SLASH;

        if (obj->trap_charge <= 0)
                return;

        act("You hear a strange noise...", ch, NULL, NULL, TO_ROOM);
        act("You hear a strange noise...", ch, NULL, NULL, TO_CHAR);
        obj->trap_charge--;

        switch(obj->trap_dam) {

            case TRAP_DAM_SLEEP:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM))
                        sleep_trap_victim(ch, obj->level, FALSE);
                else
                {
                        struct sleep_target
                        {
                                CHAR_DATA *ch;
                                struct sleep_target *next;
                                int deaths;
                        };
                        struct sleep_target *targets = NULL;
                        struct sleep_target **tail = &targets;
                        struct sleep_target *entry;
                        ROOM_INDEX_DATA *room = ch->in_room;
                        const int level = obj->level;

                        /* A mount's fall can remove a later rider as well. */
                        for (wch = room->people; wch; wch = wch->next_in_room)
                        {
                                if (wch->deleted || wch->position == POS_DEAD)
                                        continue;
                                entry = alloc_mem(sizeof(*entry));
                                entry->ch = wch;
                                entry->deaths = IS_NPC(wch)
                                    ? 0 : wch->pcdata->killed;
                                entry->next = NULL;
                                *tail = entry;
                                tail = &entry->next;
                        }

                        while (targets)
                        {
                                int deaths;

                                entry = targets;
                                targets = entry->next;
                                wch = entry->ch;
                                deaths = entry->deaths;
                                free_mem(entry, sizeof(*entry));

                                if (wch->deleted || wch->in_room != room
                                ||  wch->position == POS_DEAD
                                || (!IS_NPC(wch)
                                    && wch->pcdata->killed != deaths))
                                        continue;
                                sleep_trap_victim(wch, level, TRUE);
                        }
                }
                /* A lethal fall may have moved ch or extracted obj. */
                return;

            case TRAP_DAM_POISON:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM)) {
                        if (IS_AFFECTED(ch, AFF_SLEEP) || !can_be_poisoned(ch))
                                return;

                        af.type      = gsn_poison;
                        af.duration  = 4 + number_range(obj->level/3, obj->level);
                        af.location  = APPLY_STR;
                        af.modifier  = -4;
                        af.bitvector = AFF_POISON;
                        affect_join(ch, &af);

                        if (IS_AWAKE(ch)) {
                                send_to_char("You are struck by a small dart... your blood begins to burn!\n\r", ch);
                                act("$n is struck by a small dart!", ch, NULL, NULL, TO_ROOM);
                        }
                }
                else {
                        ACTION_ROOM_TARGET *targets;
                        size_t count;
                        size_t i;

                        targets = action_room_snapshot(room, &count);
                        if (!targets)
                                return;
                        for (i = 0; i < count; i++) {
                                if (obj->deleted)
                                        break;
                                wch = targets[i].character;
                                if (!action_character_in_room(wch, room, targets[i].deaths)
                                ||  !can_be_poisoned(wch) || IS_AFFECTED(wch, AFF_POISON))
                                        continue;

                                af.type      = gsn_poison;
                                af.duration  = 4 + number_range(obj->level/3, obj->level);
                                af.location  = APPLY_STR;
                                af.modifier  = -4;
                                af.bitvector = AFF_POISON;
                                affect_join(wch, &af);

                                if (IS_AWAKE(wch)) {
                                        send_to_char("You are struck by a small dart... your blood begins to burn!\n\r", wch);
                                        act("$n is struck by a small dart!", wch, NULL, NULL, TO_ROOM);
                                }
                        }
                        free(targets);
                }
                break;

            case TRAP_DAM_SNARE:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        if ( IS_AFFECTED(ch, AFF_SLEEP) )
                                return;

                        af.type         = gsn_snare;
                        af.duration     = 6;
                        af.location     = APPLY_HITROLL;
                        af.modifier     = -10;
                        af.bitvector    = AFF_HOLD;
                        affect_join( ch, &af );

                        if ( IS_AWAKE(ch) ) {
                                send_to_char( "You are entangled in a snare--you cannot move!\n\r", ch );
                                act( "$c becomes entangled in a snare!", ch, NULL, NULL, TO_ROOM );
                        }
                }
                else {
                        for (wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room) {

                                if ( IS_AFFECTED(wch, AFF_HOLD) )
                                        continue;

                                af.type         = gsn_snare;
                                af.duration     = 6;
                                af.location     = APPLY_HITROLL;
                                af.modifier     = -10;
                                af.bitvector    = AFF_HOLD;
                                affect_join( wch, &af );

                                if ( IS_AWAKE(wch) ) {
                                        send_to_char( "You are entangled in a snare--you cannot move!\n\r", ch );
                                        act( "$c becomes entangled in a snare!", ch, NULL, NULL, TO_ROOM );
                                }
                        }
                }
                break;

            case TRAP_DAM_TELEPORT:
                if (ch->deleted || !ch->in_room)
                        return;

                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM))
                        teleport_trap_victim(ch, FALSE, FALSE);
                else
                {
                        struct teleport_target
                        {
                                CHAR_DATA *ch;
                                struct teleport_target *next;
                                int deaths;
                                bool combat_blocked;
                        };
                        struct teleport_target *targets = NULL;
                        struct teleport_target **tail = &targets;
                        struct teleport_target *entry;
                        ROOM_INDEX_DATA *source = ch->in_room;

                        /*
                         * Save the original targets before anyone moves.
                         * Riders take their mounts, so do not queue them twice.
                         */
                        for (wch = source->people; wch; wch = wch->next_in_room)
                        {
                                CHAR_DATA *partner;

                                if (wch->deleted || wch->position == POS_DEAD
                                ||  wch->rider)
                                        continue;

                                entry = alloc_mem(sizeof(*entry));
                                entry->ch = wch;
                                entry->deaths = IS_NPC(wch)
                                        ? 0 : wch->pcdata->killed;
                                /* Earlier departures must not enable a fighting pair. */
                                entry->combat_blocked = !IS_NPC(wch)
                                        && wch->fighting;
                                if (wch->mount || wch->rider)
                                {
                                        for (partner = source->people; partner;
                                             partner = partner->next_in_room)
                                        {
                                                if ((partner == wch->mount
                                                     || partner == wch->rider)
                                                &&  !partner->deleted
                                                &&  !IS_NPC(partner)
                                                &&  partner->fighting)
                                                        entry->combat_blocked = TRUE;
                                        }
                                }
                                entry->next = NULL;
                                *tail = entry;
                                tail = &entry->next;
                        }

                        while (targets)
                        {
                                int deaths;
                                bool combat_blocked;

                                entry = targets;
                                targets = entry->next;
                                wch = entry->ch;
                                deaths = entry->deaths;
                                combat_blocked = entry->combat_blocked;
                                free_mem(entry, sizeof(*entry));

                                if (wch->deleted || wch->in_room != source
                                ||  wch->position == POS_DEAD
                                || (!IS_NPC(wch)
                                    && wch->pcdata->killed != deaths))
                                        continue;

                                teleport_trap_victim(wch, TRUE, combat_blocked);
                        }
                }
                /* Do not run damage handling in the trigger's new room. */
                return;

            case TRAP_DAM_FIRE:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("A fireball shoots out of $p and hits $n!", ch, obj, NULL, TO_ROOM);
                        act("A fireball shoots out of $p and hits you!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 4;
                }
                else {
                        act("A fireball shoots out of $p and hits everyone in the room!", ch, obj, NULL, TO_ROOM);
                        act("A fireball shoots out of $p and hits everyone in the room!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 4;
                }
                break;

            case TRAP_DAM_COLD:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("A blast of frost from $p hits $n!", ch, obj, NULL, TO_ROOM);
                        act("A blast of frost from $p hits you!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 5;
                }
                else {
                        act("A blast of frost from $p fills the room freezing you!", ch, obj, NULL, TO_ROOM);
                        act("A blast of frost from $p fills the room freezing you!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 5;
                }
                break;

            case TRAP_DAM_ACID:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("A blast of acid erupts from $p, burning your skin!", ch, obj, NULL, TO_CHAR);
                        act("A blast of acid erupts from $p, burning $n's skin!", ch, obj, NULL, TO_ROOM);
                        dam = number_range(obj->level/2, obj->level) * 6;
                }
                else {
                        act("A blast of acid erupts from $p, burning your skin!", ch, obj, NULL, TO_ROOM);
                        act("A blast of acid erupts from $p, burning your skin!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 6;
                }
                break;

            case TRAP_DAM_ENERGY:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("A pulse of energy from $p zaps $n!", ch, obj, NULL, TO_ROOM);
                        act("A pulse of energy from $p zaps you!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 3;
                }
                else {
                        act("A pulse of energy from $p zaps you!", ch, obj, NULL, TO_ROOM);
                        act("A pulse of energy from $p zaps you!", ch, obj, NULL, TO_CHAR);
                        dam = number_range(obj->level/2, obj->level) * 3;
                }
                break;

            case TRAP_DAM_BLUNT:
                dam = (10* number_range(obj->level/2, obj->level)) + (GET_AC(ch) / 4);
                break;

            case TRAP_DAM_PIERCE:
                dam = (10* number_range(obj->level/2, obj->level)) + (GET_AC(ch) / 4);
                break;

            case TRAP_DAM_SLASH:
                dam = (10* number_range(obj->level/2, obj->level)) + (GET_AC(ch) / 4);
                break;

            case TRAP_DAM_CURSE:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM)) {
                        spell_curse(gsn_curse, ch->level, ch, ch);
                } else {
                        for (wch = ch->in_room->people; wch; wch = wch->next_in_room)
                                spell_curse(gsn_curse, ch->level, wch, wch);
                }
                break;

            case TRAP_DAM_HEX:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM)) {
                        spell_hex(gsn_hex, ch->level, ch, ch);
                } else {
                        for (wch = ch->in_room->people; wch; wch = wch->next_in_room)
                                spell_hex(gsn_hex, ch->level, wch, wch);
                }
                break;

            case TRAP_DAM_SPIRIT:
                {
                    CHAR_DATA *spirit;
                    MOB_INDEX_DATA *index;

                    if (ch->deleted || !ch->in_room)
                            return;

                    index = get_mob_index(MOB_VNUM_SPIRIT);
                    if (!index)
                    {
                            bug("Trapdamage: missing spirit mobile %d.", MOB_VNUM_SPIRIT);
                            break;
                    }

                    /* The trap still uses a charge, even if the spirit is barred. */
                    if (!undead_can_spawn_in_room(index, ch->in_room))
                    {
                            send_to_char("The room bars undead, so the spirit cannot appear.\n\r", ch);
                            break;
                    }

                    act("{CA furious spirit guardian materialises from thin air!{x", ch, NULL, NULL, TO_ROOM);
                    act("{CA furious spirit guardian materialises from thin air!{x", ch, NULL, NULL, TO_CHAR);
                    spirit = create_mobile(index);
                    spirit->level = URANGE(1, ch->level, LEVEL_HERO - 1);
                    spirit->max_hit = spirit->level * 10 + number_range(20, 100);
                    spirit->hit = effective_max_hit(spirit);
                    spirit->damroll = spirit->level / 2;
                    spirit->hitroll = spirit->level / 2;
                    spirit->alignment = -1000;
                    char_to_room(spirit, ch->in_room);
                    set_fighting(spirit, ch);
                    multi_hit(spirit, ch, TYPE_UNDEFINED);
                }
                break;
        }

        if (obj->deleted || !action_character_in_room(ch, room, deaths))
                return;

        if (obj->trap_dam == TRAP_DAM_BLUNT) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n sets off a trap and is hit by a blunt object!", ch, obj, NULL, TO_ROOM);
                        act("You are hit by a blunt object from $p!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$n sets off a trap and you are hit by a blunt object!", ch, obj, NULL, TO_ROOM);
                        act("You are hit by a blunt object from $p!", ch, obj, NULL, TO_CHAR);
                }
        }

        if (obj->trap_dam == TRAP_DAM_PIERCE) {
                if (!IS_SET( obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n sets off a trap and is pierced in the chest!", ch, obj, NULL, TO_ROOM);
                        act("You set off a trap and are pierced through the chest!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$n sets off a trap and you are hit by a piercing object!", ch, obj, NULL, TO_ROOM);
                        act("You set off a trap and are pierced through the chest!", ch, obj, NULL, TO_CHAR);
                }
        }

        if (obj->trap_dam == TRAP_DAM_SLASH) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n just got slashed by a trap.", ch, obj, NULL, TO_ROOM);
                        act("You just got slashed by a trap!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$n set off a trap releasing a blade that slashes you!", ch, obj, NULL, TO_ROOM);
                        act("You set off a trap releasing blades around the room..", ch, obj, NULL, TO_CHAR);
                        act("One of the blades slashes you in the chest!", ch, obj, NULL, TO_CHAR);
                }
        }


        /*
         * Do the damage
         */

        if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM)) {
                if ( ch->position == POS_DEAD )
                        return;

                /* Phased players cannot be struck by physical trap weapons. */
                if (physical_damage && !IS_NPC(ch)
                &&  IS_AFFECTED(ch, AFF_NON_CORPOREAL))
                        return;

                /*
                 * Stop up any residual loopholes
                 */

                if ( dam > MAX_DAMAGE ) {
                        bug( "Damage: %d: more than MAX_DAMAGE points in trap", dam);
                        dam = MAX_DAMAGE;
                }

                if (IS_AFFECTED(ch, AFF_INVISIBLE))
                {
                        affect_strip(ch, gsn_invis);
                        affect_strip(ch, gsn_mass_invis);

                        affect_strip_raw_bit(
                            ch,
                            AFF_INVISIBLE);

                        if (!IS_AFFECTED(ch, AFF_INVISIBLE))
                        {
                                act(
                                    "$n fades into existence.",
                                    ch,
                                    NULL,
                                    NULL,
                                    TO_ROOM);
                        }
                }

                /*
                 * Damage modifiers
                 */

                if ( IS_AFFECTED(ch, AFF_SANCTUARY) )
                        dam /= 2;

                if (!IS_NPC(ch)
                &&   IS_AFFECTED(ch, AFF_BONUS_RESILIENCE))
                        dam -= dam / 3;

                if ( dam < 0 )
                        dam = 0;

                trap_damage_message (ch, dam);

                ch->hit -= dam;

                if ( !IS_NPC(ch)
                    && ch->level >= LEVEL_IMMORTAL
                    && ch->hit < 1 )
                        ch->hit = 1;

                update_pos( ch );

                switch( ch->position ) {

                    case POS_MORTAL:
                        act( "$n is mortally wounded, and will die soon, if not aided.",
                            ch, NULL, NULL, TO_ROOM );
                        send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r", ch );
                        break;

                    case POS_INCAP:
                        act( "$n is incapacitated and will slowly die, if not aided.",
                            ch, NULL, NULL, TO_ROOM );
                        send_to_char("You are incapacitated and will slowly die, if not aided.\n\r", ch );
                        break;

                    case POS_STUNNED:
                        act( "$n is stunned, but will probably recover.",ch, NULL, NULL, TO_ROOM);
                        send_to_char("You are stunned, but will probably recover.\n\r", ch );
                        break;

                    case POS_DEAD:
                        act( "$n is DEAD!!", ch, 0, 0, TO_ROOM );
                        send_to_char( "{RYou have been KILLED!!{x\n\r\n\r", ch );
                        break;

                    default:
                        if ( dam > effective_max_hit(ch) / 4 )
                                send_to_char( "{WThat really did HURT!{x\n\r", ch );
                        if ( ch->hit < effective_max_hit(ch) / 4 )
                                send_to_char( "{WYou sure are {x{RBLEEDING!{x\n\r", ch );
                        break;
                }

                if (ch->position == POS_DEAD) {
                        if (!IS_NPC(ch)) {
                                sprintf( log_buf, "%s killed by a trap at %d",
                                        ch->name,
                                        ch->in_room->vnum );
                                log_string( log_buf );

                                sprintf( log_buf, "ACK! %s has been killed by a trap at %s!",
                                        ch->name,
                                        ch->in_room->name );
                                do_info( ch, log_buf );
                        }
                        raw_kill( ch, ch, TRUE );
                }
        }
        else {
                CHAR_DATA *wch_next = NULL;
                const int base_dam = dam;

                for (wch = ch->in_room->people; wch; wch = wch_next) {
                        wch_next = wch->next_in_room;

                        if (obj->trap_dam == TRAP_DAM_POISON && !can_be_poisoned(wch))
                                continue;

                        if (wch->deleted || wch->position == POS_DEAD)
                                continue;

                        if (physical_damage && !IS_NPC(wch)
                        &&  IS_AFFECTED(wch, AFF_NON_CORPOREAL))
                                continue;

                        /* Start fresh for each victim. */
                        dam = base_dam;

                        if (obj->trap_dam == TRAP_DAM_BLUNT
                            || obj->trap_dam == TRAP_DAM_PIERCE
                            || obj->trap_dam == TRAP_DAM_SLASH)
                                dam = (10* number_range(obj->level/2, obj->level)) + GET_AC(wch);

                        /*
                         * Stop up any residual loopholes
                         */

                        if ( dam > MAX_DAMAGE ) {
                                bug( "Trap damage: %d: more than MAX_DAMAGE points!", dam );
                                dam = MAX_DAMAGE;
                        }

                        if (IS_AFFECTED(wch, AFF_INVISIBLE))
                        {
                                affect_strip(wch, gsn_invis);
                                affect_strip(wch, gsn_mass_invis);

                                affect_strip_raw_bit(
                                    wch,
                                    AFF_INVISIBLE);

                                if (!IS_AFFECTED(wch, AFF_INVISIBLE))
                                {
                                        act(
                                            "$n fades into existence.",
                                            wch,
                                            NULL,
                                            NULL,
                                            TO_ROOM);
                                }
                        }

                        /*
                         * Damage modifiers
                         */

                        if ( IS_AFFECTED(wch, AFF_SANCTUARY) )
                                dam /= 2;

                        if (!IS_NPC(wch)
                        &&   IS_AFFECTED(wch, AFF_BONUS_RESILIENCE))
                                dam -= dam / 3;

                        if ( dam < 0 )
                                dam = 0;

                        trap_damage_message (wch, dam);

                        wch->hit -= dam;
                        if ( !IS_NPC(wch)
                            && wch->level >= LEVEL_IMMORTAL
                            && wch->hit < 1 )
                                wch->hit = 1;

                        update_pos( wch );

                        switch( wch->position ) {

                            case POS_MORTAL:
                                act( "$n is mortally wounded, and will die soon, if not aided.",
                                    wch, NULL, NULL, TO_ROOM );
                                send_to_char( "You are mortally wounded, and will die soon, if not aided.\n\r", wch );
                                break;

                            case POS_INCAP:
                                act( "$n is incapacitated and will slowly die, if not aided.",
                                    wch, NULL, NULL, TO_ROOM );
                                send_to_char( "You are incapacitated and will slowly die, if not aided.\n\r", wch );
                                break;

                            case POS_STUNNED:
                                act( "$n is stunned, but will probably recover.",wch, NULL, NULL, TO_ROOM);
                                send_to_char("You are stunned, but will probably recover.\n\r", wch);
                                break;

                            case POS_DEAD:
                                act( "$n is DEAD!!", wch, 0, 0, TO_ROOM );
                                send_to_char( "{RYou have been KILLED!!{x\n\r\n\r", wch );
                                break;

                            default:
                                if ( dam > effective_max_hit(wch) / 4 )
                                        send_to_char( "{WThat really did HURT!{x\n\r", wch );
                                if ( wch->hit < effective_max_hit(wch) / 4 )
                                        send_to_char( "{WYou sure are {x{RBLEEDING!{x\n\r", wch );
                                break;
                        }

                        if (wch->position == POS_DEAD) {
                                if (!IS_NPC(wch)) {
                                        sprintf( log_buf, "%s killed by a trap at %d",
                                                wch->name,
                                                wch->in_room->vnum );
                                        log_string( log_buf );

                                        sprintf( log_buf, "ACK! %s has been killed by a trap at %s!",
                                                wch->name,
                                                wch->in_room->name );
                                        do_info( wch, log_buf );
                                }
                                raw_kill( wch, wch, TRUE );
                        }
                }
        }
        return;
}


/*
 *  Let's print a damage message; verbs taken from fight.c
 *  Gezhp 2000
 *
 *  Updated with new helper function by Owl 12/7/22
 */
void trap_damage_message (CHAR_DATA *ch, int dam) {

        char buf [MAX_STRING_LENGTH];
        char* verb;

        if (dam < 1)
        {
                return;
        }
        else
        {
                verb = get_damage_string(dam, FALSE);
        }

        sprintf (buf, "The trap %s $n!", verb);
        act (buf, ch, NULL, NULL, TO_ROOM);

        sprintf (buf, "The trap %s you!", verb);
        act (buf, ch, NULL, NULL, TO_CHAR);
}
