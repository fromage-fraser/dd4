/***************************************************************************
 *  map.c
 *
 *  Mapping system for Dragons Domain III
 *
 *  Originally coded by Tavolir, Oct 1999
 *
 *  Allows characters to record maps from 1 point to another on the mud
 *  and then retrace their steps via command line. Also allows characters
 *  and mobs to copy maps between themselves. Summary of available mapping
 *  commands follows:
 *
 *  MAP RECORD
 *
 *  MAP STOP
 *
 *  MAP FOLLOW
 *
 *  MAP RETRACE
 *
 *  MAP RESUME
 *
 *  MAP RENAME
 *
 *  MAP COPY
 *
 ***************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"



/* Local functions */
void    do_map_list     args( ( CHAR_DATA *ch, char *argument ) );
void    do_map_record   args( ( CHAR_DATA *ch, char *argument ) );



void do_map_list ( CHAR_DATA *ch, char *argument )
{
  MAPBOOK_DATA * map;
  int i;

  char buf  [ MAX_STRING_LENGTH ];
  char tmpbuf  [ MAX_STRING_LENGTH ];

  i = 1;

  if(!ch->mapbook) {
    sprintf( buf, "\n\rYou currently have no maps in your map book!\n\r");
  } else {
    sprintf(buf, "\n\r #  Map Name      Map Description \n\r");
    strcat( buf, "\n\r~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");

    for ( map = ch->mapbook; map != NULL; map = ch->mapbook->next )
    {
      sprintf(tmpbuf, "%d %15s %s\n\r", i, ch->mapbook->name, ch->mapbook->desc);
      i++;
    }
    strcat(buf, tmpbuf);
  }

  send_to_char(buf, ch);

}

void do_map_record ( CHAR_DATA *ch, char *argument )
{


}



void do_map (CHAR_DATA *ch, char *argument)
{
  char arg [MAX_INPUT_LENGTH];
  char buf [ MAX_STRING_LENGTH ];

  if ( ch->position == POS_FIGHTING ) {
    send_to_char( "You're too busy to examine your maps at the moment.\n\r", ch );
    return;
  }

  if ( ch->position < POS_FIGHTING ) {
    send_to_char( "You're too relaxed to examine your maps at the moment.\n\r", ch );
    return;
  }

  if ( !check_blind( ch ) ) {
    send_to_char( "How can you examine your maps when you are blinded!\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );


  /*
   * Process map options from here
   */

  /* Default option is to list characters maps */
  if ( arg[0] == '\0' || !str_prefix( arg, "list" ) ) {
    do_map_list(ch, argument);
    return;
  }

  if ( !str_prefix( arg, "record" ) ) {
    do_map_record (ch, argument);
    return;
  }

/*
  if ( !str_prefix( arg, "info" ) ) {
    do_map_info (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "stop" ) ) {
    do_map_record_stop (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "follow" ) ) {
    do_map_follow (ch, argument, MAP_FOLLOW);
    return;
  }

  if ( !str_prefix( arg, "retrace", MAP_RETRACE ) ) {
    do_map_follow (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "resume" ) ) {
    do_map_resume (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "rename" ) ) {
    do_map_rename (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "where" ) ) {
    do_map_show_locations (ch, argument);
    return;
  }

  if ( !str_prefix( arg, "copy" ) ) {
    do_map_copy (ch, argument);
    return;
  }
*/

  /* No valid option found */
  sprintf(buf, "Sorry, but you cannot %s your maps.\n\r", arg);
  send_to_char( buf, ch );
  return;
};


