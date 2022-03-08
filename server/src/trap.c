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
                send_to_char("That object is registered as a trap.\n\r", ch);
        }
        else {
                send_to_char("That object in not registered as a trap.\n\r", ch);
        }

        switch (obj->trap_dam) {
            case TRAP_DAM_SLEEP:
                send_to_char("Damage type is sleep.\n\r", ch);
                break;
            case TRAP_DAM_TELEPORT:
                send_to_char("Damage type is teleport.\n\r", ch);
                break;
            case TRAP_DAM_FIRE:
                send_to_char("Damage type is fire.\n\r", ch);
                break;
            case TRAP_DAM_COLD:
                send_to_char("Damage type is frost.\n\r", ch);
                break;
            case TRAP_DAM_ACID:
                send_to_char("Damage type is acid.\n\r", ch);
                break;
            case TRAP_DAM_ENERGY:
                send_to_char("Damage type is energy.\n\r", ch);
                break;
            case TRAP_DAM_BLUNT:
                send_to_char("Damage type is blunt.\n\r", ch);
                break;
            case TRAP_DAM_PIERCE:
                send_to_char("Damage type is pierce.\n\r", ch);
                break;
            case TRAP_DAM_SLASH:
                send_to_char("Damage type is slash.\n\r", ch);
                break;
            case TRAP_DAM_POISON:
                send_to_char("Damage type is poison.\n\r", ch );
                break;
        }

        if (obj->trap_eff == 0) {
                send_to_char("The trap has no effect.\n\r", ch);
        }
        else {
                if (IS_SET(obj->trap_eff, TRAP_EFF_MOVE) ) {
                        if (IS_SET(obj->trap_eff, TRAP_EFF_NORTH) )
                                send_to_char("The trap affects movement to the north.\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) )
                                send_to_char("The trap affects movement to the south.\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_EAST) )
                                send_to_char("The trap affects movement to the east.\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_WEST) )
                                send_to_char("The trap affects movement to the west.\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_UP) )
                                send_to_char("The trap affects movement up.\n\r", ch);
                        if (IS_SET(obj->trap_eff, TRAP_EFF_DOWN) )
                                send_to_char("The trap affects movement down.\n\r", ch);
                }
                
                if (IS_SET(obj->trap_eff, TRAP_EFF_OBJECT) )
                        send_to_char("The trap is set off by get or put.\n\r", ch);

                if (IS_SET(obj->trap_eff, TRAP_EFF_OPEN) )
                        send_to_char("The trap is set off when opened.\n\r", ch);

                if (IS_SET(obj->trap_eff, TRAP_EFF_ROOM) )
                        send_to_char("The trap affects the whole room.\n\r", ch);
        }
        
        sprintf (buf, "Trap Charges left: %d.\n\r", obj->trap_charge);
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
                        sprintf( buf, "%s carried by %s.\n\r",
                                obj->short_descr, PERS(in_obj->carried_by, ch) );
                }
                else {
                        sprintf( buf, "%s in %s.\n\r",
                                obj->short_descr,
                                in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name );
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
                send_to_char("Syntax: trapset <object> <field> <value>\n\r\n\r", ch);
                send_to_char("Field: move, object(get and put), room, open, damage, charge\n\r\n\r", ch);
                send_to_char("Values: Move> north, south, east, west, up, down, and all.\n\r\n\r", ch);
                send_to_char("        Damage> sleep, teleport, fire, cold, acid, energy,\n\r", ch);
                send_to_char("                blunt, pierce, slash.\n\r\n\r", ch);
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
                        send_to_char("Syntax: trapset <object> <field> <value>\n\r\n\r", ch);
                        send_to_char("Field: move, object(get and put), room, open, damage\n\r\n\r", ch);
                        send_to_char("Values: Move> north, south, east, west, up, down, and all.\n\r", ch);
                        send_to_char("        Damage> sleep, teleport, fire, cold, acid, energy,\n\r", ch);
                        send_to_char("                blunt, pierce, slash.\n\r\n\r", ch);
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
                                return FALSE;
                        }
                        
                        if (IS_SET(obj->trap_eff, TRAP_EFF_EAST) && dir == 1) {
                                trapdamage(ch, obj);
                                return FALSE;
                        }
        
                        if (IS_SET(obj->trap_eff, TRAP_EFF_SOUTH) && dir == 2) {
                                trapdamage(ch, obj);
                                return FALSE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_WEST) && dir == 3) {
                                trapdamage(ch, obj);
                                return FALSE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_UP) && dir == 4) {
                                trapdamage(ch, obj);
                                return FALSE;
                        }

                        if (IS_SET(obj->trap_eff, TRAP_EFF_DOWN) && dir == 5) {
                                trapdamage(ch, obj);
                                return FALSE;
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


void trapdamage(CHAR_DATA *ch, OBJ_DATA *obj)
{
        ROOM_INDEX_DATA *pRoomIndex;
        AFFECT_DATA af;
        CHAR_DATA *wch;
        int dam = 0;

        if (obj->trap_charge <= 0)
                return;

        act("You hear a strange noise...", ch, NULL, NULL, TO_ROOM);
        act("You hear a strange noise...", ch, NULL, NULL, TO_CHAR);
        obj->trap_charge--;

        switch(obj->trap_dam) {
                
            case TRAP_DAM_SLEEP:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        
                        if ( IS_AFFECTED(ch, AFF_SLEEP) )
                                return;

                        af.type      = gsn_sleep;
                        af.duration  = 4 + number_range(obj->level/6, obj->level/4);
                        af.location  = APPLY_NONE;
                        af.modifier  = 0;
                        af.bitvector = AFF_SLEEP;
                        affect_join( ch, &af );

                        if ( IS_AWAKE(ch) ) {
                                send_to_char( "You feel very sleepy...\n\r", ch );
                                act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
                                ch->position = POS_SLEEPING;
                        }
                }
                else {
                        for (wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room) {
                                
                                if ( IS_AFFECTED(wch, AFF_SLEEP) )
                                        continue;

                                af.type      = gsn_trap;
                                af.duration  = 4 + number_range(obj->level/3, obj->level);
                                af.location  = APPLY_NONE;
                                af.modifier  = 0;
                                af.bitvector = AFF_SLEEP;
                                affect_join( wch, &af );

                                if ( IS_AWAKE(wch) ) {
                                        send_to_char( "You feel very sleepy...\n\r", wch );
                                        act( "$n goes to sleep.", wch, NULL, NULL, TO_ROOM );
                                        wch->position = POS_SLEEPING;
                                }
                        }
                }
                break;

            case TRAP_DAM_POISON:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        if ( IS_AFFECTED(ch, AFF_SLEEP) )
                                return;

                        af.type      = gsn_poison;
                        af.duration  = 4 + number_range(obj->level/3, obj->level);
                        af.location  = APPLY_STR;
                        af.modifier  = -4;
                        af.bitvector = AFF_POISON;
                        affect_join( ch, &af );
                        
                        if ( IS_AWAKE(ch) ) {
                                send_to_char( "You are struck by a small dart... your blood begins to burn!\n\r", ch );
                                act( "$n is struck by a small dart!", ch, NULL, NULL, TO_ROOM );
                        }
                }
                else {
                        for (wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room) {

                                if ( IS_AFFECTED(wch, AFF_POISON) )
                                        continue;

                                af.type      = gsn_poison;
                                af.duration  = 4 + number_range(obj->level/3, obj->level);
                                af.location  = APPLY_STR;
                                af.modifier  = -4;
                                af.bitvector = AFF_POISON;
                                affect_join( wch, &af );
 
                                if ( IS_AWAKE(wch) ) {
                                        send_to_char( "You are struck by a small dart... your blood begins to burn!\n\r", ch );
                                        act( "$n is struck by a small dart!", ch, NULL, NULL, TO_ROOM );
                                }
                        }
                }
                break;

            case TRAP_DAM_TELEPORT:
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        if ( ch->in_room == NULL
                            ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
                            || ( !IS_NPC(ch) && ch->fighting != NULL ) ) {
                                send_to_char( "Wow, that was close... you set off a trap and it malfunctioned!\n\r", ch );
                                return;
                        }

                        for ( ; ; ) {
                                pRoomIndex = get_room_index( number_range( 0, 65535 ) );
                                if ( pRoomIndex != NULL )
                                        if ( !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
                                            &&   !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) )
                                                break;
                        }

                        act( "$n slowly fades out of existence.", ch, NULL, NULL, TO_ROOM );
                        char_from_room( ch );
                        char_to_room( ch, pRoomIndex );
                        act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
                        do_look( ch, "auto" );
                        return;
                }
                else {
                        for (wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room) {
                                if ( wch->in_room == NULL
                                    ||   IS_SET(wch->in_room->room_flags, ROOM_NO_RECALL)
                                    || ( !IS_NPC(wch) && wch->fighting != NULL ) ) {
                                        send_to_char( "Wow, that was close... a trap was set off and malfunctioned!\n\r", wch);
                                        continue;
                                }

                                for ( ; ; ) {
                                        pRoomIndex = get_room_index( number_range( 0, 65535 ) );
                                        if ( pRoomIndex != NULL )
                                                if ( !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
                                                    &&   !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) )
                                                        break;
                                }
                                act( "$n slowly fades out of existence.", wch, NULL, NULL, TO_ROOM );
                                char_from_room( wch );
                                char_to_room( wch, pRoomIndex );
                                act( "$n slowly fades into existence.", wch, NULL, NULL, TO_ROOM );
                                do_look( ch, "auto" );
                        }
                }
                break;

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
        }
        
        if (obj->trap_dam == TRAP_DAM_BLUNT) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n sets off a trap on $p and is hit by a blunt object!", ch, obj, NULL, TO_ROOM);
                        act("You are hit by a blunt object from $p!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$n sets off a trap on $p and you are hit by a blunt object!", ch, obj, NULL, TO_ROOM);
                        act("You are hit by a blunt object from $p!", ch, obj, NULL, TO_CHAR);
                }
        }

        if (obj->trap_dam == TRAP_DAM_PIERCE) {
                if (!IS_SET( obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n sets off a trap on $p and is pierced in the chest!", ch, obj, NULL, TO_ROOM);
                        act("You set off a trap on $p and are pierced through the chest!", ch, obj, NULL, TO_CHAR);
                }
                else {
                        act("$n sets off a trap on $p and you are hit by a piercing object!", ch, obj, NULL, TO_ROOM);
                        act("You set off a trap on $p and are pierced through the chest!", ch, obj, NULL, TO_CHAR);
                }
        }

        if (obj->trap_dam == TRAP_DAM_SLASH) {
                if (!IS_SET(obj->trap_eff, TRAP_EFF_ROOM) ) {
                        act("$n just got slashed by a trap on $p.", ch, obj, NULL, TO_ROOM);
                        act("You just got slashed by a trap on $p!", ch, obj, NULL, TO_CHAR);
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

                /*
                 * Stop up any residual loopholes
                 */

                if ( dam > MAX_DAMAGE ) {
                        bug( "Damage: %d: more than MAX_DAMAGE points in trap", dam);
                        dam = MAX_DAMAGE;
                }

                if ( IS_AFFECTED(ch, AFF_INVISIBLE) ) {
                        affect_strip( ch, gsn_invis );
                        affect_strip( ch, gsn_mass_invis );
                        REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
                        act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
                }

                /*
                 * Damage modifiers
                 */
                
                if ( IS_AFFECTED(ch, AFF_SANCTUARY) )
                        dam /= 2;

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
                        send_to_char( "You have been KILLED!!\n\r\n\r", ch );
                        break;

                    default:
                        if ( dam > ch->max_hit / 4 )
                                send_to_char( "That really did HURT!\n\r", ch );
                        if ( ch->hit < ch->max_hit / 4 )
                                send_to_char( "You sure are BLEEDING!\n\r", ch );
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
                CHAR_DATA *wch_next;
                
                for (wch = ch->in_room->people; wch; wch = wch_next) {
                        
                        wch_next = wch->next_in_room;
                        
                        if (wch->deleted || wch->position == POS_DEAD)
                                continue;
                        
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

                        if ( IS_AFFECTED(wch, AFF_INVISIBLE) ) {
                                affect_strip( wch, gsn_invis );
                                affect_strip( wch, gsn_mass_invis );
                                REMOVE_BIT( wch->affected_by, AFF_INVISIBLE );
                                act( "$n fades into existence.", wch, NULL, NULL, TO_ROOM );
                        }

                        /*
                         * Damage modifiers
                         */
                        
                        if ( IS_AFFECTED(wch, AFF_SANCTUARY) )
                                dam /= 2;

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
                                send_to_char( "You have been KILLED!!\n\r\n\r", wch );
                                break;

                            default:
                                if ( dam > wch->max_hit / 4 )
                                        send_to_char( "That really did HURT!\n\r", wch );
                                if ( wch->hit < wch->max_hit / 4 )
                                        send_to_char( "You sure are BLEEDING!\n\r", wch );
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
 */
void trap_damage_message (CHAR_DATA *ch, int dam) {
        
        char buf [MAX_STRING_LENGTH];
        char* verb;

        if (dam < 1)            return;
        else if (dam < 4)       verb = "scratches";
        else if (dam < 8)       verb = "grazes";
        else if (dam < 12)      verb = "hits";
        else if (dam < 16)      verb = "injures";
        else if (dam < 20)      verb = "wounds";
        else if (dam < 24)      verb = "mauls";
        else if (dam < 28)      verb = "decimates";
        else if (dam < 32)      verb = "mangles";
        else if (dam < 36)      verb = "maims";
        else if (dam <= 40)     verb = "$7MUTILATES$R";
        else if (dam <= 44)     verb = "$7DISEMBOWELS$R";
        else if (dam <= 48)     verb = "$BEVISCERATES$R";
        else if (dam <= 52)     verb = "$BMASSACRES$R";
        else if (dam <= 75)     verb = "$4*** $BDEMOLISHES$R$4 ***$R";
        else if (dam <= 100)    verb = "$4*** $BDEVASTATES$R$4 ***$R";
        else if (dam <= 150)    verb = "$4*** $BANNIHILATES$R$4 ***$R";
        else if (dam <= 250)    verb = "$4-=<**$FOBLITERATES$R$4**>=-$R";
        else if (dam <= 400)    verb = "$4-=<||$FRAVAGES$R$4||>=-$R";
        else if (dam <= 600)    verb = "$4-=*>|$FCRIPPLES$R$4|<*=-$R";
        else if (dam <= 900)    verb = "$4-=**>>$FBRUTALISES$R$4<<**=-$R";
        else if (dam <= 1200)   verb = "$4-=**>>$FVAPOURISES$R$4<<**=-$R";
        else if (dam <= 1500)   verb = "$4-+*>>>$FATOMIZE$R$4<<<*+-$R";
        else if (dam <= 2000)   verb = "$4-+*>#$FELIMINATE$R$4#<*+-$R";
        else if (dam <= 2500)   verb = "$4-+*###$FEXTERMINATE$R$4###*+-$R";
        else if (dam <= 3000)   verb = "$4--=##>>$FUTTERLY DESTROYS$R$4<<##=--$R";
        else                    verb = "$4-+<<[[ $FPARTICLIZES$R$4 ]]>>+-$R";

        sprintf (buf, "The trap %s $n!", verb);
        act (buf, ch, NULL, NULL, TO_ROOM);

        sprintf (buf, "The trap %s you!", verb);
        act (buf, ch, NULL, NULL, TO_CHAR);
}
