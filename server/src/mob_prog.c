/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  The MOBprograms have been contributed by N'Atas-ha.  Any support for   *
 *  these routines should not be expected from Merc Industries.  However,  *
 *  under no circumstances should the blame for bugs, etc be placed on     *
 *  Merc Industries.  They are not guaranteed to work on all systems due   *
 *  to their frequent use of strxxx functions.  They are also not the most *
 *  efficient way to perform their tasks, but hopefully should be in the   *
 *  easiest possible way to install and begin using. Documentation for     *
 *  such installation can be found in INSTALL.  Enjoy...         N'Atas-Ha *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"

/*
 *  Mudprogram additions
 */
ACT_PROG_DATA *	mob_act_list;

/*
 * Local function prototypes
 */

char *  mprog_next_command      args( ( char* clist ) );
bool    mprog_seval             args( ( char* lhs, char* opr, char* rhs ) );
bool    mprog_veval             args( ( int lhs, char* opr, int rhs ) );
bool    mprog_do_ifchck         args( ( char* ifchck, CHAR_DATA* mob,
                                       CHAR_DATA* actor, OBJ_DATA* obj,
                                       void* vo, CHAR_DATA* rndm ) );
char *  mprog_process_if        args( ( char* ifchck, char* com_list,
                                       CHAR_DATA* mob, CHAR_DATA* actor,
                                       OBJ_DATA* obj, void* vo,
                                       CHAR_DATA* rndm ) );
void    mprog_translate         args( ( char ch, char* t, CHAR_DATA* mob,
                                       CHAR_DATA* actor, OBJ_DATA* obj,
                                       void* vo, CHAR_DATA* rndm ) );
void    mprog_process_cmnd      args( ( char* cmnd, CHAR_DATA* mob,
                                       CHAR_DATA* actor, OBJ_DATA* obj,
                                       void* vo, CHAR_DATA* rndm ) );
void    mprog_driver            args( ( char* com_list, CHAR_DATA* mob,
                                       CHAR_DATA* actor, OBJ_DATA* obj,
                                       void* vo ) );

/***************************************************************************
 * Local function code and brief comments.
 */

/* if you dont have these functions, you damn well should... */

#ifdef DUNNO_STRSTR
char * strstr(s1,s2) const char *s1; const char *s2;
{
  char *cp;
  int i,j=strlen(s1)-strlen(s2),k=strlen(s2);
  if(j<0)
    return NULL;
  for(i=0; i<=j && strncmp(s1++,s2, k)!=0; i++);
  return (i>j) ? NULL : (s1-1);
}
#endif

/* Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 */
char *mprog_next_command( char *clist )
{

  char *pointer = clist;

  while ( *pointer != '\n' && *pointer != '\0' )
    pointer++;
  if ( *pointer == '\n' )
    *pointer++ = '\0';
  if ( *pointer == '\r' )
    *pointer++ = '\0';

  return ( pointer );

}

/* These two functions do the basic evaluation of ifcheck operators.
 *  It is important to note that the string operations are not what
 *  you probably expect.  Equality is exact and division is substring.
 *  remember that lhs has been stripped of leading space, but can
 *  still have trailing spaces so be careful when editing since:
 *  "guard" and "guard " are not equal.
 */
bool mprog_seval( char *lhs, char *opr, char *rhs )
{

  if ( !str_cmp( opr, "==" ) )
    return ( bool )( !str_cmp( lhs, rhs ) );
  if ( !str_cmp( opr, "!=" ) )
    return ( bool )( str_cmp( lhs, rhs ) );
  if ( !str_cmp( opr, "/" ) )
    return ( bool )( !str_infix( rhs, lhs ) );
  if ( !str_cmp( opr, "!/" ) )
    return ( bool )( str_infix( rhs, lhs ) );

  bug ( "Improper MOBprog operator\n\r", 0 );
  return 0;

}

bool mprog_veval( int lhs, char *opr, int rhs )
{

  if ( !str_cmp( opr, "==" ) )
    return ( lhs == rhs );
  if ( !str_cmp( opr, "!=" ) )
    return ( lhs != rhs );
  if ( !str_cmp( opr, ">" ) )
    return ( lhs > rhs );
  if ( !str_cmp( opr, "<" ) )
    return ( lhs < rhs );
  if ( !str_cmp( opr, ">=" ) )
    return ( lhs <= rhs );
  if ( !str_cmp( opr, ">=" ) )
    return ( lhs >= rhs );
  if ( !str_cmp( opr, "&" ) )
    return ( lhs & rhs );
  if ( !str_cmp( opr, "|" ) )
    return ( lhs | rhs );

  bug ( "Improper MOBprog operator\n\r", 0 );
  return 0;

}

/* This function performs the evaluation of the if checks.  It is
 * here that you can add any ifchecks which you so desire. Hopefully
 * it is clear from what follows how one would go about adding your
 * own. The syntax for an if check is: ifchck ( arg ) [opr val]
 * where the parenthesis are required and the opr and val fields are
 * optional but if one is there then both must be. The spaces are all
 * optional. The evaluation of the opr expressions is farmed out
 * to reduce the redundancy of the mammoth if statement list.
 * If there are errors, then return FALSE otherwise return boolean 1,0
 */
bool mprog_do_ifchck( char *ifchck, CHAR_DATA *mob, CHAR_DATA *actor,
                     OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{

  char buf[ MAX_INPUT_LENGTH ];
  char arg[ MAX_INPUT_LENGTH ];
  char opr[ MAX_INPUT_LENGTH ];
  char val[ MAX_INPUT_LENGTH ];
  CHAR_DATA *vict = (CHAR_DATA *) vo;
  OBJ_DATA *v_obj = (OBJ_DATA  *) vo;
  char     *bufpt = buf;
  char     *argpt = arg;
  char     *oprpt = opr;
  char     *valpt = val;
  char     *point = ifchck;
  int       lhsvl;
  int       rhsvl;

  if ( *point == '\0' )
    {
      bug ( "Mob: %d null ifchck", mob->pIndexData->vnum );
      return FALSE;
    }
  /* skip leading spaces */
  while ( *point == ' ' )
    point++;

  /* get whatever comes before the left paren.. ignore spaces */
  while ( *point != '(' )
    if ( *point == '\0' )
      {
        bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum );
        return FALSE;
      }
    else
      if ( *point == ' ' )
        point++;
      else
        *bufpt++ = *point++;

  *bufpt = '\0';
  point++;

  /* get whatever is in between the parens.. ignore spaces */
  while ( *point != ')' )
    if ( *point == '\0' )
      {
        bug ( "Mob: %d ifchck syntax error", mob->pIndexData->vnum );
        return FALSE;
      }
    else
      if ( *point == ' ' )
        point++;
      else
        *argpt++ = *point++;

  *argpt = '\0';
  point++;

  /* check to see if there is an operator */
  while ( *point == ' ' )
    point++;
  if ( *point == '\0' )
    {
      *opr = '\0';
      *val = '\0';
    }
  else /* there should be an operator and value, so get them */
    {
      while ( ( *point != ' ' ) && ( !isalnum( *point ) ) )
        if ( *point == '\0' )
          {
            bug ( "Mob: %d ifchck operator without value",
                 mob->pIndexData->vnum );
            return FALSE;
          }
        else
          *oprpt++ = *point++;

      *oprpt = '\0';

      /* finished with operator, skip spaces and then get the value */
      while ( *point == ' ' )
        point++;
      for( ; ; )
        {
          if ( ( *point != ' ' ) && ( *point == '\0' ) )
            break;
          else
            *valpt++ = *point++;
        }

      *valpt = '\0';
    }
  bufpt = buf;
  argpt = arg;
  oprpt = opr;
  valpt = val;

  /* Ok... now buf contains the ifchck, arg contains the inside of the
   *  parentheses, opr contains an operator if one is present, and val
   *  has the value if an operator was present.
   *  So.. basically use if statements and run over all known ifchecks
   *  Once inside, use the argument and expand the lhs. Then if need be
   *  send the lhs,opr,rhs off to be evaluated.
   */

  if ( !str_cmp( buf, "rand" ) )
    {
      return ( number_percent() <= atoi(arg) );
    }

  if ( !str_cmp( buf, "ispc" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return 0;
        case 'n': if ( actor )
                     return ( !IS_NPC( actor ) );
                  else return FALSE;
        case 't': if ( vict )
                     return ( !IS_NPC( vict ) );
                  else return FALSE;
        case 'r': if ( rndm )
                     return ( !IS_NPC( rndm ) );
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'ispc'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isnpc" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return 1;
        case 'n': if ( actor )
                     return IS_NPC( actor );
                  else return FALSE;
        case 't': if ( vict )
                     return IS_NPC( vict );
                  else return FALSE;
        case 'r': if ( rndm )
                     return IS_NPC( rndm );
                  else return FALSE;
        default:
          bug ("Mob: %d bad argument to 'isnpc'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isgood" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return IS_GOOD( mob );
        case 'n': if ( actor )
                     return IS_GOOD( actor );
                  else return FALSE;
        case 't': if ( vict )
                     return IS_GOOD( vict );
                  else return FALSE;
        case 'r': if ( rndm )
                     return IS_GOOD( rndm );
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'isgood'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isevil" ) )
    {
      switch ( arg[1] )
        {
        case 'i': return IS_EVIL( mob );
        case 'n': if ( actor )
                     return IS_EVIL( actor );
                  else return FALSE;
        case 't': if ( vict )
                     return IS_EVIL( vict );
                  else return FALSE;
        case 'r': if ( rndm )
                     return IS_EVIL( rndm );
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'isevil'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isfight" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return ( mob->fighting ) ? 1 : 0;
        case 'n': if ( actor )
                     return ( actor->fighting ) ? 1 : 0;
                  else return FALSE;
        case 't': if ( vict )
                     return ( vict->fighting ) ? 1 : 0;
                  else return FALSE;
        case 'r': if ( rndm )
                     return ( rndm->fighting ) ? 1 : 0;
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'isfight'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isimmort" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i':
                if (mob)
                {
                        if (!IS_NPC(mob) && mob->level > LEVEL_HERO)
                                return 1;
                        else
                                return 0;
                }
                return FALSE;

        case 'n':
                if (actor)
                {
                        if (!IS_NPC(actor) && actor->level > LEVEL_HERO)
                                return TRUE;
                        else
                                return FALSE;
                }
                return FALSE;

        case 't':
                if (vict)
                {
                        if (!IS_NPC(vict) && vict->level > LEVEL_HERO)
                                return TRUE;
                        else
                                return FALSE;
                }
                return FALSE;

        case 'r':
                if (rndm)
                {
                        if (!IS_NPC(rndm) && rndm->level > LEVEL_HERO)
                                return TRUE;
                        else
                                return FALSE;
                }
                return FALSE;

        default:
          bug ( "Mob: %d bad argument to 'isimmort'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "ischarmed" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return (IS_AFFECTED( mob, AFF_CHARM ) != 0);
        case 'n': if ( actor )
                     return (IS_AFFECTED( actor, AFF_CHARM ) != 0);
                  else return FALSE;
        case 't': if ( vict )
                     return (IS_AFFECTED( vict, AFF_CHARM ) != 0);
                  else return FALSE;
        case 'r': if ( rndm )
                     return (IS_AFFECTED( rndm, AFF_CHARM ) != 0);
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'ischarmed'",
               mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isfollow" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return ( mob->master != NULL
                          && mob->master->in_room == mob->in_room );
        case 'n': if ( actor )
                     return ( actor->master != NULL
                             && actor->master->in_room == actor->in_room );
                  else return FALSE;
        case 't': if ( vict )
                     return ( vict->master != NULL
                             && vict->master->in_room == vict->in_room );
                  else return FALSE;
        case 'r': if ( rndm )
                     return ( rndm->master != NULL
                             && rndm->master->in_room == rndm->in_room );
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'isfollow'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "isaffected" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': rhsvl = atoi( val );
                  return (IS_AFFECTED( mob, rhsvl ));
        case 'n': if ( actor ) {
                     rhsvl = atoi( val );
                     return (IS_AFFECTED( actor, rhsvl ) != 0); }
                  else return FALSE;
        case 't': if ( vict ) {
                     rhsvl = atoi( val );
                     return (IS_AFFECTED( vict, rhsvl ) != 0); }
                  else return FALSE;
        case 'r': if ( rndm ) {
                     rhsvl = atoi( val );
                     return (IS_AFFECTED( rndm, rhsvl ) != 0); }
                  else return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'isaffected'",
               mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "hitprcnt" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->hit / mob->max_hit;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->hit / actor->max_hit;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->hit / vict->max_hit;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->hit / rndm->max_hit;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'hitprcnt'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "inroom" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->in_room->vnum;
                  rhsvl = atoi(val);
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->in_room->vnum;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->in_room->vnum;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->in_room->vnum;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'inroom'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "sex" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->sex;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->sex;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->sex;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->sex;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'sex'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "position" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->position;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->position;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->position;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->position;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'position'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "level" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i':
                  /* lhsvl = get_trust( mob ); */
                  lhsvl = mob->level;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    /* lhsvl = get_trust( actor ); */
                    lhsvl = actor->level;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    /* lhsvl = get_trust( vict ); */
                    lhsvl = vict->level;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    /* lhsvl = get_trust( rndm ); */
                    lhsvl = rndm->level;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'level'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "size" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i':
                  /* lhsvl = get_trust( mob ); */
                  lhsvl = race_table[mob->race].size;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    /* lhsvl = get_trust( actor ); */
                    lhsvl = race_table[actor->race].size;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    /* lhsvl = get_trust( vict ); */
                    lhsvl = race_table[vict->race].size;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    /* lhsvl = get_trust( rndm ); */
                    lhsvl = race_table[rndm->race].size;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'size'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  /* Add testing based on race, Owl 5/4/22 */

  if ( !str_cmp( buf, "race" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->race;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->race;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->race;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->race;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'race'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

    /* Add testing based on subclass, Owl 24/1/23 */

    if ( !str_cmp( buf, "subclass" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->sub_class;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->sub_class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->sub_class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->sub_class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'subclass'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "class" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->class;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->class;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'class'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "goldamt" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->gold;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    lhsvl = actor->gold;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    lhsvl = vict->gold;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    lhsvl = rndm->gold;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'goldamt'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objtype" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'o': if ( obj )
                  {
                    lhsvl = obj->item_type;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                 else
                   return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->item_type;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'objtype'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objval0" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'o': if ( obj )
                  {
                    lhsvl = obj->value[0];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->value[0];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'objval0'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objval1" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'o': if ( obj )
                  {
                    lhsvl = obj->value[1];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->value[1];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'objval1'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objval2" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'o': if ( obj )
                  {
                    lhsvl = obj->value[2];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->value[2];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'objval2'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objval3" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'o': if ( obj )
                  {
                    lhsvl = obj->value[3];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->value[3];
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'objval3'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "objcount" ) || !str_cmp( buf, "objinstances" ) )
    {
        int vnum = atoi( arg );  /* 'arg' is expected to be like '28704' */
        OBJ_INDEX_DATA *pObjIndex;

        if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
        {
            bug( "Mob: %d 'objcount' bad vnum", mob->pIndexData->vnum );
            return FALSE;
        }

        lhsvl = pObjIndex->spawn_count;
        rhsvl = atoi( val );  /* 'val' should be the comparison value, e.g. '3' */
        return mprog_veval( lhsvl, opr, rhsvl );
    }

  if ( !str_cmp( buf, "number" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': lhsvl = mob->gold;
                  rhsvl = atoi( val );
                  return mprog_veval( lhsvl, opr, rhsvl );
        case 'n': if ( actor )
                  {
                    if IS_NPC( actor )
                    {
                      lhsvl = actor->pIndexData->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
                    }
                  }
                  else
                    return FALSE;
        case 't': if ( vict )
                  {
                    if IS_NPC( actor )
                    {
                      lhsvl = vict->pIndexData->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
                    }
                  }
                  else
                    return FALSE;
        case 'r': if ( rndm )
                  {
                    if IS_NPC( actor )
                    {
                      lhsvl = rndm->pIndexData->vnum;
                      rhsvl = atoi( val );
                      return mprog_veval( lhsvl, opr, rhsvl );
                    }
                  }
                 else return FALSE;
        case 'o': if ( obj )
                  {
                    lhsvl = obj->pIndexData->vnum;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                  {
                    lhsvl = v_obj->pIndexData->vnum;
                    rhsvl = atoi( val );
                    return mprog_veval( lhsvl, opr, rhsvl );
                  }
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'number'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  if ( !str_cmp( buf, "name" ) )
    {
      switch ( arg[1] )  /* arg should be "$*" so just get the letter */
        {
        case 'i': return mprog_seval( mob->name, opr, val );
        case 'n': if ( actor )
                    return mprog_seval( actor->name, opr, val );
                  else
                    return FALSE;
        case 't': if ( vict )
                    return mprog_seval( vict->name, opr, val );
                  else
                    return FALSE;
        case 'r': if ( rndm )
                    return mprog_seval( rndm->name, opr, val );
                  else
                    return FALSE;
        case 'o': if ( obj )
                    return mprog_seval( obj->name, opr, val );
                  else
                    return FALSE;
        case 'p': if ( v_obj )
                    return mprog_seval( v_obj->name, opr, val );
                  else
                    return FALSE;
        default:
          bug ( "Mob: %d bad argument to 'name'", mob->pIndexData->vnum );
          return FALSE;
        }
    }

  /*
   * Clan check by Gezhp 2001
   */
        if (!str_cmp(buf, "clan"))
        {
                int clan = clan_lookup(val);

                switch (arg[1])
                {
                    case 'i':
                        return FALSE;

                    case 'n':
                        if (actor && !IS_NPC(actor))
                            return mprog_veval(actor->clan, opr, clan);
                        else
                            return FALSE;

                    case 't':
                        if (vict)
                            return mprog_veval(vict->clan, opr, clan);
                        else
                            return FALSE;

                    case 'r':
                        return FALSE;

                    case 'o':
                        return FALSE;

                    case 'p':
                        return FALSE;

                    default:
                        bug("Mob: %d bad argument to 'clan'",
                            mob->pIndexData->vnum);
                        return FALSE;
                }
        }

        /*
         * Is carrying - Shade 2003
         *
         * carrying($n) == obj
         */

        if (!str_cmp(buf, "carrying"))
        {
                int vnum = atoi(val);

                switch (arg[1])
                {
                    case 'i':
                        if (mob)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = mob->carrying; pobj; pobj = pobj->next_content)
                            {
                                if (pobj->pIndexData->vnum == vnum)
                                   return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return mprog_veval(0, opr, vnum);
                        }
                        else
                            return FALSE;

                    case 'n':
                        if (actor)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = actor->carrying; pobj; pobj = pobj->next_content)
                            {
                                if (pobj->pIndexData->vnum == vnum)
                                   return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return mprog_veval(0, opr, vnum);
                        }
                        else
                            return FALSE;

                    case 't':
                        if (vict)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = vict->carrying; pobj; pobj = pobj->next_content)
                            {
                                if (pobj->pIndexData->vnum == vnum)
                                   return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return FALSE;
                        }
                        else
                            return FALSE;

                    case 'r':
                        return FALSE;

                    case 'o':
                        return FALSE;

                    case 'p':
                        return FALSE;

                    default:
                        bug("Mob: %d bad argument to 'carrying'",
                            mob->pIndexData->vnum);
                        return FALSE;
                }
        }

        /*
         * Is an object in the room? Made so mobs dropping traps can check
         * to ensure one isn't already present. --Owl 29/3/24
         *
         * objroom($n) == obj
         */

        if (!str_cmp(buf, "objroom"))
        {
                int vnum = atoi(val);

                switch (arg[1])
                {
                    case 'i':
                        if (mob)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = mob->in_room->contents; pobj; pobj = pobj->next_content)
                            {
                                    if (pobj->pIndexData->vnum == vnum )
                                        return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return mprog_veval(0, opr, vnum);
                        }
                        else
                            return FALSE;

                    case 'n':
                        if (actor)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = actor->in_room->contents; pobj; pobj = pobj->next_content)
                            {
                                if (pobj->pIndexData->vnum == vnum)
                                   return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return mprog_veval(0, opr, vnum);
                        }
                        else
                            return FALSE;

                    case 't':
                        if (vict)
                        {
                            OBJ_DATA* pobj;

                            for (pobj = vict->in_room->contents; pobj; pobj = pobj->next_content)
                            {
                                if (pobj->pIndexData->vnum == vnum)
                                   return mprog_veval(pobj->pIndexData->vnum, opr, vnum);
                            }

                            return FALSE;
                        }
                        else
                            return FALSE;

                    case 'r':
                        return FALSE;

                    case 'o':
                        return FALSE;

                    case 'p':
                        return FALSE;

                    default:
                        bug("Mob: %d bad argument to 'objroom'",
                            mob->pIndexData->vnum);
                        return FALSE;
                }
        }

        if ( !str_cmp( buf, "insect" ) )
        {
        switch ( arg[1] )  /* arg should be "$*" so just get the letter */
            {
            case 'i': lhsvl = mob->in_room->sector_type;
                    rhsvl = atoi(val);
                    return mprog_veval( lhsvl, opr, rhsvl );
            case 'n': if ( actor )
                    {
                        lhsvl = actor->in_room->sector_type;
                        rhsvl = atoi( val );
                        return mprog_veval( lhsvl, opr, rhsvl );
                    }
                    else
                        return FALSE;
            case 't': if ( vict )
                    {
                        lhsvl = vict->in_room->sector_type;
                        rhsvl = atoi( val );
                        return mprog_veval( lhsvl, opr, rhsvl );
                    }
                    else
                        return FALSE;
            case 'r': if ( rndm )
                    {
                        lhsvl = rndm->in_room->sector_type;
                        rhsvl = atoi( val );
                        return mprog_veval( lhsvl, opr, rhsvl );
                    }
                    else
                        return FALSE;
            default:
            bug ( "Mob: %d bad argument to 'insect'", mob->pIndexData->vnum );
            return FALSE;
            }
        }

        if ( !str_cmp( buf, "inrflag" ) )
        {
        switch ( arg[1] )  /* arg should be "$*" so just get the letter */
            {
            case 'i': rhsvl = atoi( val );
                    return (IS_SET( mob->in_room->room_flags, rhsvl ));
            case 'n': if ( actor ) {
                        rhsvl = atoi( val );
                        return (IS_SET( actor->in_room->room_flags, rhsvl ) != 0); }
                    else return FALSE;
            case 't': if ( vict ) {
                        rhsvl = atoi( val );
                        return (IS_SET( vict->in_room->room_flags, rhsvl ) != 0); }
                    else return FALSE;
            case 'r': if ( rndm ) {
                        rhsvl = atoi( val );
                        return (IS_SET( rndm->in_room->room_flags, rhsvl ) != 0); }
                    else return FALSE;
            default:
            bug ( "Mob: %d bad argument to 'inrflag'",
                mob->pIndexData->vnum );
            return FALSE;
            }
        }

        if ( !str_cmp( buf, "affspell" ) )
        {
        switch ( arg[1] )  /* arg should be "$*" so just get the letter */
            {
            case 'i': rhsvl = atoi( val );
                    return (is_affected( mob, rhsvl ));
            case 'n': if ( actor ) {
                        rhsvl = atoi( val );
                        return ((is_affected( actor, rhsvl )) != 0); }
                    else return FALSE;
            case 't': if ( vict ) {
                        rhsvl = atoi( val );
                        return ((is_affected( vict, rhsvl )) != 0); }
                    else return FALSE;
            case 'r': if ( rndm ) {
                        rhsvl = atoi( val );
                        return ((is_affected( rndm, rhsvl )) != 0); }
                    else return FALSE;
            default:
            bug ( "Mob: %d bad argument to 'affspell'",
                mob->pIndexData->vnum );
            return FALSE;
            }
        }


  /* Ok... all the ifchcks are done, so if we didnt find ours then something
   * odd happened.  So report the bug and abort the MOBprogram (return error)
   */
  bug ( "Mob: %d unknown ifchck", mob->pIndexData->vnum );
  return FALSE;

}


/* Quite a long and arduous function, this guy handles the control
 * flow part of MOBprograms.  Basicially once the driver sees an
 * 'if' attention shifts to here.  While many syntax errors are
 * caught, some will still get through due to the handling of break
 * and errors in the same fashion.  The desire to break out of the
 * recursion without catastrophe in the event of a mis-parse was
 * believed to be high. Thus, if an error is found, it is bugged and
 * the parser acts as though a break were issued and just bails out
 * at that point. I havent tested all the possibilites, so I'm speaking
 * in theory, but it is 'guaranteed' to work on syntactically correct
 * MOBprograms, so if the mud crashes here, check the mob carefully!
 */
char *mprog_process_if( char *ifchck, char *com_list, CHAR_DATA *mob,
                       CHAR_DATA *actor, OBJ_DATA *obj, void *vo,
                       CHAR_DATA *rndm )
{

 static char null[1];
 char buf[ MAX_INPUT_LENGTH ];
 char *morebuf = '\0';
 char    *cmnd = '\0';
 bool loopdone = FALSE;
 bool     flag = FALSE;
 int  legal;

 *null = '\0';

 /* check for trueness of the ifcheck */
 if ( ( legal = mprog_do_ifchck( ifchck, mob, actor, obj, vo, rndm ) ) ) {
   if ( legal == 1 )
     flag = TRUE;
   else
     return null;
 }

 while( loopdone == FALSE ) /*scan over any existing or statements */
 {
     cmnd     = com_list;
     com_list = mprog_next_command( com_list );
     while ( *cmnd == ' ' )
       cmnd++;
     if ( *cmnd == '\0' )
     {
         bug ( "Mob: %d no commands after IF/OR", mob->pIndexData->vnum );
         return null;
     }
     morebuf = one_argument( cmnd, buf );
     if ( !str_cmp( buf, "or" ) )
     {
         if ( ( legal = mprog_do_ifchck( morebuf,mob,actor,obj,vo,rndm ) ) ) {
           if ( legal == 1 )
             flag = TRUE;
           else
             return null;
         }
     }
     else
       loopdone = TRUE;
 }

 if ( flag )
   for ( ; ; ) /*ifcheck was true, do commands but ignore else to endif*/
   {
       if ( !str_cmp( buf, "if" ) )
       {
           com_list = mprog_process_if(morebuf,com_list,mob,actor,obj,vo,rndm);
           while ( *cmnd==' ' )
             cmnd++;
           if ( *com_list == '\0' )
             return null;
           cmnd     = com_list;
           com_list = mprog_next_command( com_list );
           morebuf  = one_argument( cmnd,buf );
           continue;
       }
       if ( !str_cmp( buf, "break" ) )
         return null;
       if ( !str_cmp( buf, "endif" ) )
         return com_list;
       if ( !str_cmp( buf, "else" ) )
       {
           while ( str_cmp( buf, "endif" ) )
           {
               cmnd     = com_list;
               com_list = mprog_next_command( com_list );
               while ( *cmnd == ' ' )
                 cmnd++;
               if ( *cmnd == '\0' )
               {
                   bug ( "Mob: %d missing endif after else",
                        mob->pIndexData->vnum );
                   return null;
               }
               morebuf = one_argument( cmnd,buf );
           }
           return com_list;
       }
       mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
       cmnd     = com_list;
       com_list = mprog_next_command( com_list );
       while ( *cmnd == ' ' )
         cmnd++;
       if ( *cmnd == '\0' )
       {
           bug ( "Mob: %d missing else or endif", mob->pIndexData->vnum );
           return null;
       }
       morebuf = one_argument( cmnd, buf );
   }
 else /*false ifcheck, find else and do existing commands or quit at endif*/
   {
     while ( ( str_cmp( buf, "else" ) ) && ( str_cmp( buf, "endif" ) ) )
       {
         cmnd     = com_list;
         com_list = mprog_next_command( com_list );
         while ( *cmnd == ' ' )
           cmnd++;
         if ( *cmnd == '\0' )
           {
             bug ( "Mob: %d missing an else or endif",
                  mob->pIndexData->vnum );
             return null;
           }
         morebuf = one_argument( cmnd, buf );
       }

     /* found either an else or an endif.. act accordingly */
     if ( !str_cmp( buf, "endif" ) )
       return com_list;
     cmnd     = com_list;
     com_list = mprog_next_command( com_list );
     while ( *cmnd == ' ' )
       cmnd++;
     if ( *cmnd == '\0' )
       {
         bug ( "Mob: %d missing endif", mob->pIndexData->vnum );
         return null;
       }
     morebuf = one_argument( cmnd, buf );

     for ( ; ; ) /*process the post-else commands until an endif is found.*/
       {
         if ( !str_cmp( buf, "if" ) )
           {
             com_list = mprog_process_if( morebuf, com_list, mob, actor,
                                         obj, vo, rndm );
             while ( *cmnd == ' ' )
               cmnd++;
             if ( *com_list == '\0' )
               return null;
             cmnd     = com_list;
             com_list = mprog_next_command( com_list );
             morebuf  = one_argument( cmnd,buf );
             continue;
           }
         if ( !str_cmp( buf, "else" ) )
           {
             bug ( "Mob: %d found else in an else section",
                  mob->pIndexData->vnum );
             return null;
           }
         if ( !str_cmp( buf, "break" ) )
           return null;
         if ( !str_cmp( buf, "endif" ) )
           return com_list;
         mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
         cmnd     = com_list;
         com_list = mprog_next_command( com_list );
         while ( *cmnd == ' ' )
           cmnd++;
         if ( *cmnd == '\0' )
           {
             bug ( "Mob:%d missing endif in else section",
                  mob->pIndexData->vnum );
             return null;
           }
         morebuf = one_argument( cmnd, buf );
       }
   }
}

/* This routine handles the variables for command expansion.
 * If you want to add any go right ahead, it should be fairly
 * clear how it is done and they are quite easy to do, so you
 * can be as creative as you want. The only catch is to check
 * that your variables exist before you use them. At the moment,
 * using $t when the secondary target refers to an object
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>
 * probably makes the mud crash (vice versa as well) The cure
 * would be to change act() so that vo becomes vict & v_obj.
 * but this would require a lot of small changes all over the code.
 */
void mprog_translate( char ch, char *t, CHAR_DATA *mob, CHAR_DATA *actor,
                    OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
 static char *he_she        [] = { "it",  "he",  "she" };
 static char *him_her       [] = { "it",  "him", "her" };
 static char *his_her       [] = { "its", "his", "her" };
 CHAR_DATA   *vict             = (CHAR_DATA *) vo;
 OBJ_DATA    *v_obj            = (OBJ_DATA  *) vo;

 *t = '\0';
 switch ( ch ) {
     case 'i':
         one_argument( mob->name, t );
      break;

     case 'I':
         strcpy( t, mob->short_descr );
      break;

     case 'n':
         if ( actor )
           if ( can_see( mob,actor ) )
             one_argument( actor->name, t );
         if ( !IS_NPC( actor ) )
           *t = UPPER( *t );
      break;

     case 'N':
         if ( actor ) {
            if ( can_see( mob, actor ) )
               if ( IS_NPC( actor ) )
                 strcpy( t, actor->short_descr );
               else
               {
                   strcpy( t, actor->name );
                   strcat( t, " " );
                   strcat( t, actor->pcdata->title );
               }
            else
              strcpy( t, "someone" );
         }
         break;

     case 't':
         if ( vict )
           if ( can_see( mob, vict ) )
             one_argument( vict->name, t );
         if ( !IS_NPC( vict ) )
           *t = UPPER( *t );
         break;

     case 'T':
         if ( vict ) {
            if ( can_see( mob, vict ) )
               if ( IS_NPC( vict ) )
                 strcpy( t, vict->short_descr );
               else
               {
                 strcpy( t, vict->name );
                 strcat( t, " " );
                 strcat( t, vict->pcdata->title );
               }
            else
              strcpy( t, "someone" );
         }
         break;

     case 'r':
         if ( rndm )
           if ( can_see( mob, rndm ) )
             one_argument( rndm->name, t );
         if ( !IS_NPC( rndm ) )
           *t = UPPER( *t );
      break;

     case 'R':
         if ( rndm ) {
            if ( can_see( mob, rndm ) )
               if ( IS_NPC( rndm ) )
                 strcpy(t,rndm->short_descr);
               else
               {
                 strcpy( t, rndm->name );
                 strcat( t, " " );
                 strcat( t, rndm->pcdata->title );
               }
            else
              strcpy( t, "someone" );
         }
         break;

     case 'e':
         if ( actor )
           can_see( mob, actor ) ? strcpy( t, he_she[ actor->sex ] )
                                 : strcpy( t, "someone" );
         break;

     case 'm':
         if ( actor )
           can_see( mob, actor ) ? strcpy( t, him_her[ actor->sex ] )
                                 : strcpy( t, "someone" );
         break;

     case 's':
         if ( actor )
           can_see( mob, actor ) ? strcpy( t, his_her[ actor->sex ] )
                                 : strcpy( t, "someone's" );
         break;

     case 'E':
         if ( vict )
           can_see( mob, vict ) ? strcpy( t, he_she[ vict->sex ] )
                                : strcpy( t, "someone" );
         break;

     case 'M':
         if ( vict )
           can_see( mob, vict ) ? strcpy( t, him_her[ vict->sex ] )
                                : strcpy( t, "someone" );
         break;

     case 'S':
         if ( vict )
           can_see( mob, vict ) ? strcpy( t, his_her[ vict->sex ] )
                                : strcpy( t, "someone's" );
         break;

     case 'j':
         strcpy( t, he_she[ mob->sex ] );
         break;

     case 'k':
         strcpy( t, him_her[ mob->sex ] );
         break;

     case 'l':
         strcpy( t, his_her[ mob->sex ] );
         break;

     case 'J':
         if ( rndm )
           can_see( mob, rndm ) ? strcpy( t, he_she[ rndm->sex ] )
                                : strcpy( t, "someone" );
         break;

     case 'K':
         if ( rndm )
           can_see( mob, rndm ) ? strcpy( t, him_her[ rndm->sex ] )
                                : strcpy( t, "someone" );
         break;

     case 'L':
         if ( rndm )
           can_see( mob, rndm ) ? strcpy( t, his_her[ rndm->sex ] )
                                : strcpy( t, "someone's" );
         break;

     case 'o':
         if ( obj )
           can_see_obj( mob, obj ) ? one_argument( obj->name, t )
                                   : strcpy( t, "something" );
         break;

     case 'O':
         if ( obj )
           can_see_obj( mob, obj ) ? strcpy( t, obj->short_descr )
                                   : strcpy( t, "something" );
         break;

     case 'p':
         if ( v_obj )
           can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t )
                                     : strcpy( t, "something" );
         break;

     case 'P':
         if ( v_obj )
           can_see_obj( mob, v_obj ) ? strcpy( t, v_obj->short_descr )
                                     : strcpy( t, "something" );
      break;

     case 'a':
         if ( obj )
          switch ( *( obj->name ) )
          {
            case 'a': case 'e': case 'i':
            case 'o': case 'u': strcpy( t, "an" );
              break;
            default: strcpy( t, "a" );
          }
         break;

     case 'A':
         if ( v_obj )
          switch ( *( v_obj->name ) )
          {
            case 'a': case 'e': case 'i':
            case 'o': case 'u': strcpy( t, "an" );
              break;
            default: strcpy( t, "a" );
          }
         break;

     case '$':
         strcpy( t, "$" );
         break;

     default:
         bug( "Mob: %d bad $var", mob->pIndexData->vnum );
         break;
       }

 return;

}

/* This procedure simply copies the cmnd to a buffer while expanding
 * any variables by calling the translate procedure.  The observant
 * code scrutinizer will notice that this is taken from act()
 */
void mprog_process_cmnd( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor,
                        OBJ_DATA *obj, void *vo, CHAR_DATA *rndm )
{
  char buf[ MAX_INPUT_LENGTH ];
  char tmp[ MAX_INPUT_LENGTH ];
  char *str;
  char *i;
  char *point;

  point   = buf;
  str     = cmnd;

  while ( *str != '\0' )
  {
    if ( *str != '$' )
    {
      *point++ = *str++;
      continue;
    }
    str++;
    mprog_translate( *str, tmp, mob, actor, obj, vo, rndm );
    i = tmp;
    ++str;
    while ( ( *point = *i ) != '\0' )
      ++point, ++i;
  }
  *point = '\0';
  interpret( mob, buf );

  return;

}

/* The main focus of the MOBprograms.  This routine is called
 *  whenever a trigger is successful.  It is responsible for parsing
 *  the command list and figuring out what to do. However, like all
 *  complex procedures, everything is farmed out to the other guys.
 */
void mprog_driver ( char *com_list, CHAR_DATA *mob, CHAR_DATA *actor,
                   OBJ_DATA *obj, void *vo)
{

 char tmpcmndlst[ MAX_STRING_LENGTH ];
 char buf       [ MAX_INPUT_LENGTH ];
 char *morebuf;
 char *command_list;
 char *cmnd;
 CHAR_DATA *rndm  = NULL;
 CHAR_DATA *vch   = NULL;
 int        count = 0;

 if IS_AFFECTED( mob, AFF_CHARM )
   return;

 /* get a random visable mortal player who is in the room with the mob */

 /* riposte is killing mobs and setting the room to null. temp fix Shade */
 if (!mob->in_room)
  return;

 for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
   if ( !IS_NPC( vch )
       &&  vch->level < MAX_LEVEL-5
       &&  can_see( mob, vch ) )
     {
       if ( number_range( 0, count ) == 0 )
         rndm = vch;
       count++;
     }

 strcpy( tmpcmndlst, com_list );
 command_list = tmpcmndlst;
 cmnd         = command_list;
 command_list = mprog_next_command( command_list );
 while ( *cmnd != '\0' )
   {
     morebuf = one_argument( cmnd, buf );
     if ( !str_cmp( buf, "if" ) )
       command_list = mprog_process_if( morebuf, command_list, mob,
                                       actor, obj, vo, rndm );
     else
       mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
     cmnd         = command_list;
     command_list = mprog_next_command( command_list );
   }

 return;

}

/***************************************************************************
 * Global function code and brief comments.
 */

/* The next two routines are the basic trigger types. Either trigger
 *  on a certain percent, or trigger on a keyword or word phrase.
 *  To see how this works, look at the various trigger routines..
 */
void mprog_wordlist_check( char *arg, CHAR_DATA *mob, CHAR_DATA *actor,
                          OBJ_DATA *obj, void *vo, int type )
{

  char          temp1[ MAX_STRING_LENGTH ];
  char          temp2[ MAX_INPUT_LENGTH ];
  char          word[ MAX_INPUT_LENGTH ];
  MPROG_DATA    *mprg;
  char          *list;
  char          *start;
  char          *dupl;
  char          *end;
  unsigned int  i;

  for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
    if ( mprg->type & type )
      {
        strcpy( temp1, mprg->arglist );
        list = temp1;
        for ( i = 0; i < strlen( list ); i++ )
        list[i] = LOWER( list[i] );
         /*
	     * BUGFIX: Copy AT MOST sizeof( temp2) characters.  Walker
	     */
	    strncpy( temp2, arg, sizeof( temp2 ) );
	    /*
	     * Add a NULL char at end of temp2.  Walker
	     */
	    temp2[ MAX_INPUT_LENGTH - 1 ] = '\0';
        dupl = temp2;

        for ( i = 0; i < strlen( dupl ); i++ )
          dupl[i] = LOWER( dupl[i] );
        if ( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
          {
            list += 2;
            while ( ( start = strstr( dupl, list ) ) )
              if ( (start == dupl || *(start-1) == ' ' )
                  && ( *(end = start + strlen( list ) ) == ' '
                      || *end == '\n'
                      || *end == '\r'
                      || *end == 0 ) )
                {
                  mprog_driver( mprg->comlist, mob, actor, obj, vo );
                  break;
                }
              else
                dupl = start+1;
          }
        else
          {
            list = one_argument( list, word );
            for( ; word[0] != '\0'; list = one_argument( list, word ) )
              while ( ( start = strstr( dupl, word ) ) )
                if ( ( start == dupl || *(start-1) == ' ' )
                    && ( *(end = start + strlen( word ) ) == ' '
                        || *end == '\n'
                        || *end == '\r'
                        || *end == 0 ) )
                  {
                    mprog_driver( mprg->comlist, mob, actor, obj, vo );
                    break;
                  }
                else
                  dupl = start+1;
          }
      }

  return;

}

bool moveprog_percent_check( CHAR_DATA *mob )
{
    MPROG_DATA * mprg;
    bool fired;
    int percent;

    fired = FALSE;

    for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
    {
        if ( mprg->type & MOVE_PROG )
        {
            percent = number_percent( );

            if ( percent <= atoi( mprg->arglist ) )
            {
                fired = TRUE;
                return fired;
            }
        }
    }
    return fired;

}

void mprog_percent_check( CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj,
                         void *vo, int type)
{
    MPROG_DATA * mprg;

    for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
    {

        if ( mprg->type & type )
        {
            if ( number_percent( ) <= atoi( mprg->arglist ) )
            {
                mprog_driver( mprg->comlist, mob, actor, obj, vo );

                if ( type != GREET_PROG && type != ALL_GREET_PROG )
                    break;
            }
        }
    }
    return;

}


void mob_act_add( CHAR_DATA *mob )
{
    ACT_PROG_DATA * runner;

    for ( runner = mob_act_list; runner; runner = runner->next )
	if ( runner->vo == mob )
	    return;

    runner		= alloc_mem( sizeof( ACT_PROG_DATA ) );
    runner->vo		= mob;
    runner->next	= mob_act_list;
    mob_act_list	= runner;
}

/* The triggers.. These are really quite basic, and since most appear only
 * once in the code (hmm. i think they all do) it would be more efficient
 * to substitute the code in and make the mprog_xxx_check routines global.
 * However, they are all here in one nice place at the moment to make it
 * easier to see what they look like. If you do substitute them back in,
 * make sure you remember to modify the variable names to the ones in the
 * trigger calls.
 */

void mprog_act_trigger( char *buf, CHAR_DATA * mob, CHAR_DATA * ch,
		       OBJ_DATA * obj, void *vo )
{
    MPROG_ACT_LIST *tmp_act;

    if ( IS_NPC( mob )
	&& IS_SET( mob->pIndexData->progtypes, ACT_PROG ) )
    {
        /*
         * Don't let a mob trigger itself, nor one instance of a mob
         * trigger another instance.
         */
	if ( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData )
	    return;
	tmp_act = alloc_mem( sizeof( MPROG_ACT_LIST ) );
	if ( mob->mpactnum > 0 )
	    tmp_act->next = mob->mpact->next;
	else
	    tmp_act->next = NULL;

	mob->mpact = tmp_act;
	mob->mpact->buf = str_dup( buf );
	mob->mpact->ch = ch;
	mob->mpact->obj = obj;
	mob->mpact->vo = vo;
	mob->mpactnum++;
	mob_act_add( mob );
    }

    return;
}

void mprog_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{

  char        buf[ MAX_STRING_LENGTH ];
  MPROG_DATA *mprg;
  OBJ_DATA   *obj;

  if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & BRIBE_PROG ) )
    {
      obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0, "common", FALSE );
      sprintf( buf, obj->short_descr, amount );
      free_string( obj->short_descr );
      obj->short_descr = str_dup( buf );
      obj->value[0]    = amount;
      obj_to_char( obj, mob );
      mob->gold -= amount;

      for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
        if ( ( mprg->type & BRIBE_PROG )
            && ( amount >= atoi( mprg->arglist ) ) )
          {
            mprog_driver( mprg->comlist, mob, ch, obj, NULL );
            break;
          }
    }

  return;

}

/* If this fires movement in the direction will not automatically happen, so do an mptransfer
 and mpforce look at the new location to 'fake' a successful move --Owl */

bool mprog_move_trigger( CHAR_DATA *ch, int door )
{

    CHAR_DATA *vmob;
    bool fired;

    char *dirs[6]   = {"north", "east", "south", "west", "up", "down"};
    char *txt       = dirs[door];

    fired = FALSE;

    for ( vmob = ch->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
    {
        fired = FALSE;

        if ( IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & MOVE_PROG ) )
        {
            fired = moveprog_percent_check( vmob );

            if (fired)
            {
                mprog_wordlist_check( txt, vmob, ch, NULL, NULL, MOVE_PROG );
            }
        }
    }

    return fired;

}

void mprog_death_trigger( CHAR_DATA *mob )
{

 if ( IS_NPC( mob )
     && ( mob->pIndexData->progtypes & DEATH_PROG ) )
   {
     mprog_percent_check( mob, NULL, NULL, NULL, DEATH_PROG );
   }

 /* death_cry( mob ); */

 return;

}

void mprog_entry_trigger( CHAR_DATA *mob )
{

 if ( IS_NPC( mob )
     && ( mob->pIndexData->progtypes & ENTRY_PROG ) )
   mprog_percent_check( mob, NULL, NULL, NULL, ENTRY_PROG );

 return;

}

void mprog_fight_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{

 if ( IS_NPC( mob )
     && ( mob->pIndexData->progtypes & FIGHT_PROG ) )
   mprog_percent_check( mob, ch, NULL, NULL, FIGHT_PROG );

 return;

}

void mprog_give_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{

 char        buf[MAX_INPUT_LENGTH];
 MPROG_DATA *mprg;

 if ( IS_NPC( mob )
     && ( mob->pIndexData->progtypes & GIVE_PROG ) )
   for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
     {
       one_argument( mprg->arglist, buf );
       if ( ( mprg->type & GIVE_PROG )
           && ( ( !str_cmp( obj->name, mprg->arglist ) )
               || ( !str_cmp( "all", buf ) ) ) )
         {
           mprog_driver( mprg->comlist, mob, ch, obj, NULL );
           break;
         }
     }

 return;

}

void mprog_greet_trigger( CHAR_DATA *ch )
{

 CHAR_DATA *vmob;

 for ( vmob = ch->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
   if ( IS_NPC( vmob )
       && ch != vmob
       && can_see( vmob, ch )
       && ( vmob->fighting == NULL )
       && IS_AWAKE( vmob )
       && ( vmob->pIndexData->progtypes & GREET_PROG) )
     mprog_percent_check( vmob, ch, NULL, NULL, GREET_PROG );
   else
     if ( IS_NPC( vmob )
         && ( vmob->fighting == NULL )
         && IS_AWAKE( vmob )
         && ( vmob->pIndexData->progtypes & ALL_GREET_PROG ) )
       mprog_percent_check(vmob,ch,NULL,NULL,ALL_GREET_PROG);

 return;

}

void mprog_hitprcnt_trigger( CHAR_DATA *mob, CHAR_DATA *ch)
{

 MPROG_DATA *mprg;

 if ( IS_NPC( mob )
     && ( mob->pIndexData->progtypes & HITPRCNT_PROG ) )
   for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
     if ( ( mprg->type & HITPRCNT_PROG )
         && ( ( 100*mob->hit / mob->max_hit ) < atoi( mprg->arglist ) ) )
       {
         mprog_driver( mprg->comlist, mob, ch, NULL, NULL );
         break;
       }

 return;

}

void mprog_random_trigger( CHAR_DATA *mob )
{

  if ( mob->pIndexData->progtypes & RAND_PROG)
    mprog_percent_check(mob,NULL,NULL,NULL,RAND_PROG);

  return;

}

void mprog_speech_trigger( char *txt, CHAR_DATA *mob )
{

  CHAR_DATA *vmob;

  for ( vmob = mob->in_room->people; vmob != NULL; vmob = vmob->next_in_room )
    if ( IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & SPEECH_PROG ) )
      mprog_wordlist_check( txt, vmob, mob, NULL, NULL, SPEECH_PROG );

  return;

}

