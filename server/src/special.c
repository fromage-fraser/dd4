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


#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*
 * Local
 */

void wear_obj           args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );

/*
 * The following special functions are available for mobiles.
 *
 * REMEMBER: check xp bonus in load_specials in db.c and update spec_fun_name() and spec_lookup() if you add a special
 */

DECLARE_SPEC_FUN( spec_breath_any          );
DECLARE_SPEC_FUN( spec_breath_acid         );
DECLARE_SPEC_FUN( spec_breath_fire         );
DECLARE_SPEC_FUN( spec_breath_frost        );
DECLARE_SPEC_FUN( spec_breath_gas          );
DECLARE_SPEC_FUN( spec_breath_lightning    );
DECLARE_SPEC_FUN( spec_cast_adept          );
DECLARE_SPEC_FUN( spec_cast_hooker         );
DECLARE_SPEC_FUN( spec_buddha              );
DECLARE_SPEC_FUN( spec_kungfu_poison       );
DECLARE_SPEC_FUN( spec_clan_guard          );
DECLARE_SPEC_FUN( spec_cast_cleric         );
DECLARE_SPEC_FUN( spec_cast_judge          );
DECLARE_SPEC_FUN( spec_cast_mage           );
DECLARE_SPEC_FUN( spec_cast_druid          );
DECLARE_SPEC_FUN( spec_cast_water_sprite   );
DECLARE_SPEC_FUN( spec_cast_psionicist     );
DECLARE_SPEC_FUN( spec_cast_undead         );
DECLARE_SPEC_FUN( spec_executioner         );
DECLARE_SPEC_FUN( spec_fido                );
DECLARE_SPEC_FUN( spec_guard               );
DECLARE_SPEC_FUN( spec_janitor             );
DECLARE_SPEC_FUN( spec_poison              );
DECLARE_SPEC_FUN( spec_repairman           );
DECLARE_SPEC_FUN( spec_thief               );
DECLARE_SPEC_FUN( spec_bounty              );
DECLARE_SPEC_FUN( spec_grail               );
DECLARE_SPEC_FUN( spec_cast_orb            );
DECLARE_SPEC_FUN( spec_assassin            );
DECLARE_SPEC_FUN( spec_warrior             );
DECLARE_SPEC_FUN( spec_vampire             );
DECLARE_SPEC_FUN( spec_cast_archmage       );
DECLARE_SPEC_FUN( spec_cast_priestess      );
DECLARE_SPEC_FUN( spec_mast_vampire        );
DECLARE_SPEC_FUN( spec_bloodsucker         );
DECLARE_SPEC_FUN( spec_spectral_minion     );
DECLARE_SPEC_FUN( spec_celestial_repairman );
DECLARE_SPEC_FUN( spec_sahuagin            );
DECLARE_SPEC_FUN( spec_evil_evil_gezhp     );
DECLARE_SPEC_FUN( spec_demon               );
DECLARE_SPEC_FUN( spec_cast_electric       );
DECLARE_SPEC_FUN( spec_small_whale         );
DECLARE_SPEC_FUN( spec_large_whale         );
DECLARE_SPEC_FUN( spec_kappa               );
DECLARE_SPEC_FUN( spec_aboleth             );
DECLARE_SPEC_FUN( spec_laghathti           );
DECLARE_SPEC_FUN( spec_superwimpy          );
DECLARE_SPEC_FUN( spec_uzollru             );


/*
 * Given a name, return the appropriate spec fun.
 */

SPEC_FUN *spec_lookup (const char *name )
{
        if (!str_cmp(name, "spec_breath_any"))           return spec_breath_any;
        if (!str_cmp(name, "spec_breath_acid"))          return spec_breath_acid;
        if (!str_cmp(name, "spec_breath_fire"))          return spec_breath_fire;
        if (!str_cmp(name, "spec_breath_frost"))         return spec_breath_frost;
        if (!str_cmp(name, "spec_breath_gas"))           return spec_breath_gas;
        if (!str_cmp(name, "spec_breath_lightning"))     return spec_breath_lightning;
        if (!str_cmp(name, "spec_cast_adept"))           return spec_cast_adept;
        if (!str_cmp(name, "spec_cast_hooker"))          return spec_cast_hooker;
        if (!str_cmp(name, "spec_buddha"))               return spec_buddha;
        if (!str_cmp(name, "spec_kungfu_poison"))        return spec_kungfu_poison;
        if (!str_cmp(name, "spec_cast_cleric"))          return spec_cast_cleric;
        if (!str_cmp(name, "spec_cast_judge"))           return spec_cast_judge;
        if (!str_cmp(name, "spec_cast_mage"))            return spec_cast_mage;
        if (!str_cmp(name, "spec_cast_druid"))           return spec_cast_druid;
        if (!str_cmp(name, "spec_cast_water_sprite"))    return spec_cast_water_sprite;
        if (!str_cmp(name, "spec_cast_psionicist"))      return spec_cast_psionicist;
        if (!str_cmp(name, "spec_cast_undead"))          return spec_cast_undead;
        if (!str_cmp(name, "spec_executioner"))          return spec_executioner;
        if (!str_cmp(name, "spec_fido"))                 return spec_fido;
        if (!str_cmp(name, "spec_clan_guard"))           return spec_clan_guard;
        if (!str_cmp(name, "spec_guard"))                return spec_guard;
        if (!str_cmp(name, "spec_janitor"))              return spec_janitor;
        if (!str_cmp(name, "spec_poison"))               return spec_poison;
        if (!str_cmp(name, "spec_repairman"))            return spec_repairman;
        if (!str_cmp(name, "spec_thief"))                return spec_thief;
        if (!str_cmp(name, "spec_bounty"))               return spec_bounty;
        if (!str_cmp(name, "spec_grail"))                return spec_grail;
        if (!str_cmp(name, "spec_cast_orb"))             return spec_cast_orb;
        if (!str_cmp(name, "spec_assassin"))             return spec_assassin;
        if (!str_cmp(name, "spec_warrior"))              return spec_warrior;
        if (!str_cmp(name, "spec_vampire"))              return spec_vampire;
        if (!str_cmp(name, "spec_cast_archmage"))        return spec_cast_archmage;
        if (!str_cmp(name, "spec_cast_priestess"))       return spec_cast_priestess;
        if (!str_cmp(name, "spec_mast_vampire"))         return spec_mast_vampire;
        if (!str_cmp(name, "spec_bloodsucker"))          return spec_bloodsucker;
        if (!str_cmp(name, "spec_spectral_minion"))      return spec_spectral_minion;
        if (!str_cmp(name, "spec_celestial_repairman"))  return spec_celestial_repairman;
        if (!str_cmp(name, "spec_sahuagin"))             return spec_sahuagin;
        if (!str_cmp(name, "spec_evil_evil_gezhp"))      return spec_evil_evil_gezhp;
        if (!str_cmp(name, "spec_demon"))                return spec_demon;
        if (!str_cmp(name, "spec_cast_electric"))        return spec_cast_electric;
        if (!str_cmp(name, "spec_small_whale"))          return spec_small_whale;
        if (!str_cmp(name, "spec_large_whale"))          return spec_large_whale;
        if (!str_cmp(name, "spec_kappa"))                return spec_kappa;
        if (!str_cmp(name, "spec_aboleth"))              return spec_aboleth;
        if (!str_cmp(name, "spec_laghathti"))            return spec_laghathti;
        if (!str_cmp(name, "spec_superwimpy"))           return spec_superwimpy;
        if (!str_cmp(name, "spec_uzollru"))              return spec_uzollru;


        return 0;

}

/*
* Get text string for a mobile's spec_fun, used in mstat --Owl 22/2/22
*
*/
char* spec_fun_name (CHAR_DATA *ch)
{
    /* fprintf(stderr, "has spec fun var: %p\r\n", ch->spec_fun); */
    /* fprintf(stderr, "spec lookup var: %p\r\n", spec_lookup("spec_cast_hooker")); */

    if ( IS_NPC(ch)
         &&   ch->spec_fun )
    {
        if (ch->spec_fun == spec_lookup("spec_breath_any"))         return "spec_breath_any";
        if (ch->spec_fun == spec_lookup("spec_breath_acid"))        return "spec_breath_acid";
        if (ch->spec_fun == spec_lookup("spec_breath_fire"))        return "spec_breath_fire";
        if (ch->spec_fun == spec_lookup("spec_breath_frost"))       return "spec_breath_frost";
        if (ch->spec_fun == spec_lookup("spec_breath_gas"))         return "spec_breath_gas";
        if (ch->spec_fun == spec_lookup("spec_breath_lightning"))   return "spec_breath_lightning";
        if (ch->spec_fun == spec_lookup("spec_cast_adept"))         return "spec_cast_adept";
        if (ch->spec_fun == spec_lookup("spec_cast_hooker"))        return "spec_cast_hooker";
        if (ch->spec_fun == spec_lookup("spec_buddha"))             return "spec_buddha";
        if (ch->spec_fun == spec_lookup("spec_kungfu_poison"))      return "spec_kungfu_poison";
        if (ch->spec_fun == spec_lookup("spec_cast_cleric"))        return "spec_cast_cleric";
        if (ch->spec_fun == spec_lookup("spec_cast_judge"))         return "spec_cast_judge";
        if (ch->spec_fun == spec_lookup("spec_cast_mage"))          return "spec_cast_mage";
        if (ch->spec_fun == spec_lookup("spec_cast_druid"))         return "spec_cast_druid";
        if (ch->spec_fun == spec_lookup("spec_cast_water_sprite"))  return "spec_cast_water_sprite";
        if (ch->spec_fun == spec_lookup("spec_cast_psionicist"))    return "spec_cast_psionicist";
        if (ch->spec_fun == spec_lookup("spec_cast_undead"))        return "spec_cast_undead";
        if (ch->spec_fun == spec_lookup("spec_executioner"))        return "spec_executioner";
        if (ch->spec_fun == spec_lookup("spec_fido"))               return "spec_fido";
        if (ch->spec_fun == spec_lookup("spec_clan_guard"))         return "spec_clan_guard";
        if (ch->spec_fun == spec_lookup("spec_guard"))              return "spec_guard";
        if (ch->spec_fun == spec_lookup("spec_janitor"))            return "spec_janitor";
        if (ch->spec_fun == spec_lookup("spec_poison"))             return "spec_poison";
        if (ch->spec_fun == spec_lookup("spec_repairman"))          return "spec_repairman";
        if (ch->spec_fun == spec_lookup("spec_thief"))              return "spec_thief";
        if (ch->spec_fun == spec_lookup("spec_bounty"))             return "spec_bounty";
        if (ch->spec_fun == spec_lookup("spec_grail"))              return "spec_grail";
        if (ch->spec_fun == spec_lookup("spec_cast_orb"))           return "spec_cast_orb";
        if (ch->spec_fun == spec_lookup("spec_assassin"))           return "spec_assassin";
        if (ch->spec_fun == spec_lookup("spec_warrior"))            return "spec_warrior";
        if (ch->spec_fun == spec_lookup("spec_vampire"))            return "spec_vampire";
        if (ch->spec_fun == spec_lookup("spec_cast_archmage"))      return "spec_cast_archmage";
        if (ch->spec_fun == spec_lookup("spec_cast_priestess"))     return "spec_cast_priestess";
        if (ch->spec_fun == spec_lookup("spec_mast_vampire"))       return "spec_mast_vampire";
        if (ch->spec_fun == spec_lookup("spec_bloodsucker"))        return "spec_bloodsucker";
        if (ch->spec_fun == spec_lookup("spec_spectral_minion"))    return "spec_spectral_minion";
        if (ch->spec_fun == spec_lookup("spec_celestial_repairman"))return "spec_celestial_repairman";
        if (ch->spec_fun == spec_lookup("spec_sahuagin"))           return "spec_sahuagin";
        if (ch->spec_fun == spec_lookup("spec_evil_evil_gezhp"))    return "spec_evil_evil_gezhp";
        if (ch->spec_fun == spec_lookup("spec_demon"))              return "spec_demon";
        if (ch->spec_fun == spec_lookup("spec_cast_electric"))      return "spec_cast_electric";
        if (ch->spec_fun == spec_lookup("spec_small_whale"))        return "spec_small_whale";
        if (ch->spec_fun == spec_lookup("spec_large_whale"))        return "spec_large_whale";
        if (ch->spec_fun == spec_lookup("spec_kappa"))              return "spec_kappa";
        if (ch->spec_fun == spec_lookup("spec_aboleth"))            return "spec_aboleth";
        if (ch->spec_fun == spec_lookup("spec_laghathti"))          return "spec_laghathti";
        if (ch->spec_fun == spec_lookup("spec_superwimpy"))         return "spec_superwimpy";
        if (ch->spec_fun == spec_lookup("spec_uzollru"))            return "spec_uzollru";
    }
    else {
        return "none";
    }
    return "none";
}



/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
        CHAR_DATA *victim;
        int sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(2))
                        break;
        }

        if (!victim)
                return FALSE;

        if ( ( sn = skill_lookup( spell_name ) ) < 0 )
                return FALSE;

        if (sn == skill_lookup("acid breath"))
                act("$n rears and a stream of acid spurts from $s mouth!",
                    ch, NULL, NULL, TO_ROOM);

        else if (sn == skill_lookup("fire breath"))
                act("Fire bursts forth from $n's open maw!",
                    ch, NULL, NULL, TO_ROOM);

        else if (sn == skill_lookup("frost breath"))
                act("An icy blast shoots forth from $n's mouth!",
                    ch, NULL, NULL, TO_ROOM);

        else if (sn == skill_lookup("lightning breath"))
                act("Bolts of lightning streak from $n's mouth!",
                    ch, NULL, NULL, TO_ROOM);


        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
        switch ( number_bits( 3 ) )
        {
            case 0: return spec_breath_fire             ( ch );
            case 1:
            case 2: return spec_breath_lightning        ( ch );
            case 3: return spec_breath_gas              ( ch );
            case 4: return spec_breath_acid             ( ch );
            case 5:
            case 6:
            case 7: return spec_breath_frost            ( ch );
        }

        return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
        return dragon( ch, "acid breath" );
}


bool spec_breath_fire( CHAR_DATA *ch )
{
        return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
        return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
        int sn;

        if ( ch->position != POS_FIGHTING )
                return FALSE;

        if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
                return FALSE;

        act("A vile cloud of poisonous gas erupts from $n's wide-open jaws!",
            ch, NULL, NULL, TO_ROOM);

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL );

        return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
        return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        if ( !IS_AWAKE( ch ) || ch->fighting )
                return FALSE;

        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
                        break;
        }

        if ( !victim || IS_NPC( victim ) )
                return FALSE;

        switch ( number_bits( 3 ) )
        {
            case 0:
                act( "$n utters the word 'Edahs'.", ch, NULL, NULL, TO_ROOM );
                spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
                return TRUE;

            case 1:
                act( "$n utters the word 'Lwo'.",    ch, NULL, NULL, TO_ROOM );
                spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
                return TRUE;

            case 2:
                act( "$n utters the word 'Phzeg'.",   ch, NULL, NULL, TO_ROOM );
                spell_cure_blindness( skill_lookup( "cure blindness" ),
                                     ch->level, ch, victim );
                return TRUE;

            case 3:
                act( "$n utters the word 'Nixmas'.",    ch, NULL, NULL, TO_ROOM );
                spell_cure_light( skill_lookup( "cure light" ), ch->level, ch, victim );
                return TRUE;

            case 4:
                act( "$n utters the word 'Suturb'.",  ch, NULL, NULL, TO_ROOM );
                spell_cure_poison( skill_lookup( "cure poison" ), ch->level, ch, victim );
                return TRUE;

            case 5:
                act( "$n utters the word 'Aitseh'.", ch, NULL, NULL, TO_ROOM );
                spell_refresh( skill_lookup( "refresh" ), ch->level, ch, victim );
                return TRUE;

            case 6:
                act("$n says the word 'Tnemrot'.",ch,NULL,NULL,TO_ROOM);
                spell_cure_serious( skill_lookup( "cure serious" ), ch->level, ch, victim );
                return TRUE;

            case 7:
                act("$n utters the word 'Enroht'.", ch, NULL, NULL, TO_ROOM );
                spell_cure_mana( skill_lookup( "cure mana" ), ch->level, ch, victim );
                return TRUE;

        }

        return FALSE;
}



bool spec_cast_hooker(CHAR_DATA *ch)
{
        CHAR_DATA *victim;

        if(!IS_AWAKE(ch) || ch->fighting)
                return FALSE;

        for(victim=ch->in_room->people;victim;victim=victim->next_in_room)
        {
                if(victim != ch && can_see(ch, victim) && number_bits(1) == 0)
                        break;
        }

        if(!victim || IS_NPC(victim))
                return FALSE;

        switch (number_bits(3))
        {
            case 0:
                act("$n begins to slowly rub your crotch.", ch, NULL, NULL, TO_ROOM);
                spell_armor(skill_lookup("armor"), ch->level, ch, victim);
                return TRUE;

            case 1:
                act("$n spanks you on your bottom. OUCH!", ch, NULL, NULL, TO_ROOM);
                spell_bless(skill_lookup("bless"), ch->level, ch, victim);
                return TRUE;

            case 2:
                act("$n gently caresses your face.", ch, NULL, NULL, TO_ROOM);
                spell_cure_blindness(skill_lookup("cure blindness"), ch->level, ch,
                                     victim);
                return TRUE;

            case 3:
                act("$n eyes you up and down, seductively.", ch, NULL, NULL, TO_ROOM);
                spell_cure_mana(skill_lookup("cure mana"), ch->level, ch, victim);
                return TRUE;

            case 4:
                act("$n playfully starts to suck on your finger.",ch,NULL,NULL,TO_ROOM);
                spell_cure_poison(skill_lookup("cure poison"), ch->level, ch, victim);
                return TRUE;

            case 5:
                act("$n moves closer, licking her lips.", ch, NULL, NULL, TO_ROOM);
                spell_refresh(skill_lookup("refresh"), ch->level, ch, victim);
                return TRUE;

            case 6:
                if ( CAN_SPEAK(ch) ) { act("$n says, 'Hey baby, want a date?'", ch, NULL, NULL, TO_ROOM); }
                spell_cure_serious(skill_lookup("cure serious"), ch->level, ch, victim);
                return TRUE;

            case 7:
                if ( CAN_SPEAK(ch) ) { act("$n says, 'I go all the way for 500 coins.'", ch, NULL, NULL,
                    TO_ROOM); }
                spell_cure_mana(skill_lookup("cure mana"), ch->level, ch, victim);
                return TRUE;

        }

        return FALSE;

}

bool spec_cast_cleric( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;

                switch ( number_bits( 4 ) )
                {
                    case  0: min_level =  0; spell = "blindness";      break;
                    case  1: min_level =  3; spell = "cause serious";  break;
                    case  2: min_level =  7; spell = "earthquake";     break;
                    case  3: min_level =  9; spell = "cause critical"; break;
                    case  4: min_level = 10; spell = "dispel evil";    break;
                    case  5: min_level = 12; spell = "curse";          break;
                    case  6:
                    case  7: min_level = 13; spell = "flamestrike";    break;
                    case  8:
                    case  9:
                    case 10: min_level = 15; spell = "harm";           break;
                    default: min_level = 16; spell = "dispel magic";   break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}



bool spec_cast_judge( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        spell = "high explosive";

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;

                switch ( number_bits( 4 ) )
                {
                    case  0: min_level =  0; spell = "blindness";      break;
                    case  1: min_level =  3; spell = "chill touch";    break;
                    case  2: min_level =  7; spell = "weaken";         break;
                    case  3:
                    case  4: min_level = 10; spell = "lightning bolt"; break;
                    case  5: min_level = 15; spell = "fireball";       break;
                    case  6: min_level = 25; spell = "acid blast";     break;
                    case  7: min_level = 20; spell = "dispel magic";   break;
                    case  8: min_level = 25; spell = "energy drain";   break;
                    default: min_level = 10; spell = "colour spray";   break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}



bool spec_cast_druid( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 10) )
                {
                    case  0:
                    case  1:
                        min_level =  0;
                        spell = "faerie fire";
                        break;

                    case  2:
                        min_level =  5;
                        spell = "bark skin";
                        target_self = TRUE;
                        break;

                    case  3:
                        min_level =  10;
                        spell = "flamestrike";
                        break;

                    case  4:
                        min_level = 11;
                        spell = "inertial barrier";
                        target_self = TRUE;
                        break;

                    case  5:
                        min_level = 15;
                        spell = "fear";
                        break;

                    case  6:
                        min_level = 18;
                        spell = "moonray";
                        break;

                    case  7:
                        min_level = 18;
                        spell = "sunray";
                        break;

                    default:
                        min_level = 20;
                        spell = "wither";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        if (target_self)
                act ("$c concentrates intensely.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c gestures towards $N.", ch, NULL, victim, TO_NOTVICT);
                act ("$c gestures towards you.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;
}

bool spec_cast_water_sprite( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 13) )
                {
                    case  0:
                    case  1:
                        min_level =  0;
                        spell = "faerie fire";
                        break;

                    case  2:
                        min_level =  5;
                        spell = "shocking grasp";
                        break;

                    case  3:
                        min_level =  10;
                        spell = "chill touch";
                        break;

                    case  4:
                        min_level = 11;
                        spell = "armor";
                        target_self = TRUE;
                        break;

                    case  5:
                        min_level = 15;
                        spell = "fear";
                        break;

                    case  6:
                        min_level = 18;
                        spell = "feeblemind";
                        break;

                    case  7:
                        min_level = 18;
                        spell = "steal strength";
                        break;

                    case  8:
                        min_level = 20;
                        spell = "paralysis";
                        break;

                    case  9:
                        min_level = 25;
                        spell = "energy drain";
                        break;

                    case  10:
                        min_level = 35;
                        spell = "slow";
                        break;

                    default:
                        min_level = 25;
                        spell = "fury of nature";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        if (target_self)
                act ("$c concentrates intensely.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c gestures towards $N.", ch, NULL, victim, TO_NOTVICT);
                act ("$c gestures towards you.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;

                switch ( number_bits( 4 ) )
                {
                    case  0: min_level =  0; spell = "curse";          break;
                    case  1:
                    case  2: min_level =  6; spell = "chill touch";    break;
                    case  3: min_level =  9; spell = "blindness";      break;
                    case  4: min_level = 12; spell = "poison";         break;
                    case  5: min_level = 15; spell = "energy drain";   break;
                    case  6:
                    case  7: min_level = 18; spell = "harm";           break;
                    default: min_level = 20; spell = "gate";           break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}



bool spec_executioner( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *crime;
        char       buf [ MAX_STRING_LENGTH ];

        if ( !IS_AWAKE( ch ) || ch->fighting )
                return FALSE;

        crime = "";
        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if ( victim->deleted )
                        continue;

                if ( !IS_NPC( victim ) && IS_SET( victim->status, PLR_KILLER ) )
                {
                        crime = "KILLER";
                        break;
                }

                if ( !IS_NPC( victim ) && IS_SET( victim->status, PLR_THIEF))
                {
                        crime = "THIEF";
                        break;
                }
        }

        if ( !victim )
                return FALSE;

        if ( CAN_SPEAK(ch) )
        {
                sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
                        victim->name, crime );
        }

        do_shout( ch, buf );
        multi_hit( ch, victim, TYPE_UNDEFINED );

        char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
                     ch->in_room );
        char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
                     ch->in_room );

        return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        OBJ_DATA *corpse;
        OBJ_DATA *corpse_next;

        if ( !IS_AWAKE( ch ) )
                return FALSE;

        for ( corpse = ch->in_room->contents; corpse; corpse = corpse_next )
        {
                corpse_next = corpse->next_content;

                if ( corpse->deleted )
                        continue;

                if ( corpse->item_type != ITEM_CORPSE_NPC )
                        continue;

                act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
                for ( obj = corpse->contains; obj; obj = obj_next )
                {
                        obj_next = obj->next_content;

                        if ( obj->deleted )
                                continue;

                        obj_from_obj( obj );
                        obj_to_room( obj, ch->in_room );
                }

                extract_obj( corpse );
                return TRUE;
        }

        return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        CHAR_DATA *ech;
        char      *crime;
        char       buf [ MAX_STRING_LENGTH ];
        int        max_evil;

        if (!IS_AWAKE(ch))
                return FALSE;

        max_evil = 300;
        ech      = NULL;
        crime    = "";

        /*
         * Guards got some combat training - Shade July 03
         */

        if (!ch->fighting)
        {

                for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
                {
                        if ( victim->deleted )
                                continue;

                        if ( !IS_NPC( victim ) && IS_SET( victim->status, PLR_KILLER ) )
                        {
                                crime = "KILLER";
                                break;
                        }

                        if ( !IS_NPC( victim ) && IS_SET( victim->status, PLR_THIEF))
                        {
                                crime = "THIEF";
                                break;
                        }

                        if ( victim->fighting
                        && victim->fighting != ch
                        && IS_NPC(victim->fighting)
                        && victim->alignment < max_evil
                        && (!IS_NPC(victim) ||
                                (IS_NPC(victim) && victim->pIndexData->vnum != ch->pIndexData->vnum)))
                        {
                                max_evil = victim->alignment;
                                ech      = victim;
                        }
                }

                if ( victim )
                {
                        sprintf( buf, "%s is a %s!  %s must be put to DEATH!",
                                victim->name, crime, victim->name );
                        do_shout( ch, buf );
                        multi_hit( ch, victim, TYPE_UNDEFINED );
                        return TRUE;
                }

                if ( ech )
                {
                        if( CAN_SPEAK(ch))
                        {
                                act( "{Y$c screams, '$C must DIE!'{x", ch, NULL, ech, TO_NOTVICT );
                                act( "{Y$c screams, 'You must DIE!'{x", ch, NULL, ech, TO_VICT );
                        }
                        else
                        {
                                act( "{Y$c suddenly attacks $C!{x", ch, NULL, ech, TO_NOTVICT );
                                act( "{Y$c suddenly attacks you!{x", ch, NULL, ech, TO_VICT );
                        }

                        multi_hit( ch, ech, TYPE_UNDEFINED );
                        return TRUE;
                }
        }
        else
        {
                victim = ch->fighting;

                if (!victim || number_bits(1))
                        return FALSE;

                switch( number_bits(2) )
                {
                case 0:
                        if( CAN_SPEAK(ch)) { do_say(ch,"Feel my head!!"); }
                        do_headbutt(ch, victim->name);
                        return TRUE;

                case 1:
                        if( CAN_SPEAK(ch)) { do_say(ch, "Eat this!!"); }
                        do_smash( ch, victim->name);
                        return TRUE;

                default:
                        if( CAN_SPEAK(ch)) { do_say(ch, "Mighty foot engaged!!"); }
                        do_kick( ch, "");
                        return TRUE;

                }

        }

        return FALSE;
}


bool spec_clan_guard( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char       buf [ MAX_STRING_LENGTH ];
        int        tmp;

        if ( !IS_AWAKE( ch ) || ch->fighting )
                return FALSE;

        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if ( !IS_NPC( victim )
                    && victim->level <= LEVEL_HERO
                    && victim->clan
                    && !clan_table[victim->clan].independent
                    && clan_table[victim->clan].guard_room != victim->in_room->vnum )
                        break;
        }

        if ( victim )
        {
                tmp = number_percent();
                if (tmp < 33)
                        sprintf( buf, "%s of the %s clan is invading!  I must protect the clan!  %s must DIE!",
                                victim->name, clan_table[victim->clan].who_name, victim->name );

                else if (tmp < 66)
                        sprintf( buf, "Ack!  %s of the %s clan has penetrated our lair!  I must repel the invader!",
                                victim->name, clan_table[victim->clan].who_name );

                else
                        sprintf( buf, "We're being attacked!  %s of the %s clan has infiltrated our head-quarters!  DEATH to %s!",
                                victim->name, clan_table[victim->clan].who_name, victim->name );

                if ( CAN_SPEAK(ch) ) { do_chat( ch, buf ); }
                multi_hit( ch, victim, TYPE_UNDEFINED );

                return TRUE;
        }

        return FALSE;
}


bool spec_janitor( CHAR_DATA *ch )
{
        OBJ_DATA *trash;
        OBJ_DATA *trash_next;

        if ( !IS_AWAKE( ch ) )
                return FALSE;

        for ( trash = ch->in_room->contents; trash; trash = trash_next )
        {
                trash_next = trash->next_content;

                if (trash->deleted)
                        continue;

                if (!IS_SET(trash->wear_flags, ITEM_TAKE))
                        continue;

                if (trash->owner[0] != '\0')
                        continue;

                if (trash->item_type == ITEM_DRINK_CON
                    || trash->item_type == ITEM_TRASH
                    || trash->cost < 10 )
                {
                        act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
                        obj_from_room( trash );
                        obj_to_char( trash, ch );
                        return TRUE;
                }
        }

        return FALSE;
}


bool spec_poison( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(2))
                        break;
        }

        if (!victim)
                return FALSE;

        act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
        act( "$n bites you!", ch, NULL, victim, TO_VICT    );
        act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
        spell_poison( gsn_poison, ch->level, ch, victim );
        return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        int        gold;

        if ( ch->position != POS_STANDING )
                return FALSE;

        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if ( IS_NPC( victim )
                    || victim->level >= LEVEL_IMMORTAL
                    || number_bits( 3 ) != 0
                    || !can_see( ch, victim ) ) /* Thx Glop */
                        continue;

                if ( IS_AWAKE( victim ) && number_percent( ) >= ch->level )
                {
                        act( "You discover $n's hands in your wallet!",
                            ch, NULL, victim, TO_VICT );
                        act( "$N discovers $n's hands in $S wallet!",
                            ch, NULL, victim, TO_NOTVICT );
                        return TRUE;
                }
                else
                {
                        gold = ( total_coins_char(victim) * number_range( 1, 20 ) ) / 100;
                        coins_to_char(  (7 * gold / 8) ,ch, COINS_ADD);
                        coins_from_char(gold, victim);

                        return TRUE;
                }
        }

        return FALSE;
}


/*
 * Psionicist spec_fun by Thelonius for EnvyMud.
 */
bool spec_cast_psionicist( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;

                switch ( number_bits( 4 ) )
                {
                    case 0:
                    case 1:
                    case 2:
                        min_level =  0;
                        spell = "mind thrust";
                        break;

                    case 3:
                    case 4:
                min_level = 13;
                        spell = "ego whip";
                        break;

                    case 5:
                    case 6:
                        min_level = 14;
                        spell = "energy drain";
                        break;

                    case 7:
                    case 8: min_level = 25;
                        spell = "ultrablast";
                        break;

                    case 9:
                    case 10:
                        min_level = 35;
                        spell = "disintegrate";
                        break;

                    case 11:
                    case 12:
                        min_level = 50;
                        spell = "synaptic blast";
                        break;

                    default:
                        min_level = 15;
                        spell = "psionic blast";
                        break;

                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}

/*
 * Aboleth special.  Similar to psionic.
 */
bool spec_aboleth( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;
        char buf[MAX_STRING_LENGTH];

        spell = "ego whip";

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 9) )
                {
                    case  0:
                        min_level = 5;
                        if (is_affected(victim, gsn_psychic_drain))
                        {
                                break;
                        }
                        else {
                                if ( CAN_SPEAK(ch) ) { sprintf(buf,"So strong, so confident..."); }
                                spell = "psychic drain";
                                break;
                        }

                    case  1:
                        min_level = 11;
                        if (is_affected(ch, gsn_biofeedback))
                        {
                                break;
                        }
                        else {
                                if ( CAN_SPEAK(ch) ) { sprintf(buf,"I am not without my defenses, fleshling..."); }
                                spell = "biofeedback";
                                target_self = TRUE;
                                break;
                        }

                    case  2:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "What a sticky situation this is..."); }
                        do_spit_mucus( ch, victim->name);
                        return TRUE;

                    case  3:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Thinking?  Oh we can't have that!"); }
                        do_spit_mucus( ch, victim->name);
                        return TRUE;

                    case  4:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Mmmmmm... energy..."); }
                        min_level = 14;
                        spell = "energy drain";
                        break;

                    case  5:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"How about a shock to the system?"); }
                        min_level = 50;
                        spell = "synaptic blast";
                        break;

                    case  6:
                        min_level = 25;
                        if (is_affected(ch, gsn_combat_mind))
                        {
                                break;
                        }
                        else {
                                if ( CAN_SPEAK(ch) ) { sprintf(buf,"Let me retrieve some relevant memories..."); }
                                spell = "combat mind";
                                target_self = TRUE;
                                break;
                        }

                    case  7:

                    default:
                        min_level = 1;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Behold the power of an ancient mind!"); }
                        spell = "ultrablast";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        do_say(ch,buf);

        if (target_self)
                act ("$c lifts $s hideous head up and closes one of $s three eyes.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c motions towards $N with $s tentacles.", ch, NULL, victim, TO_NOTVICT);
                act ("$c motions towards you with $s tentacles.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;
}



/*
 * spec_fun to repair bashed doors by Thelonius for EnvyMud.
 */
bool spec_repairman( CHAR_DATA *ch )
{
        EXIT_DATA       *pexit;
        EXIT_DATA       *pexit_rev;
        ROOM_INDEX_DATA *to_room;
        int              door;

        if ( !IS_AWAKE( ch ) )
                return FALSE;

        door = number_range( 0, 5 );

        if ( !( pexit = ch->in_room->exit[door] ) )
                return FALSE;

        if ( IS_SET( pexit->exit_info, EX_BASHED ) )
        {
                REMOVE_BIT( pexit->exit_info, EX_BASHED );
                act( "You repair the $d.", ch, NULL, pexit->keyword, TO_CHAR );
                act( "$n repairs the $d.", ch, NULL, pexit->keyword, TO_ROOM );

                if (   ( to_room   = pexit->to_room               )
                    && ( pexit_rev = to_room->exit[directions[door].reverse] )
                    && pexit_rev->to_room == ch->in_room )
                {
                        CHAR_DATA *rch;

                        REMOVE_BIT( pexit_rev->exit_info, EX_BASHED );

                        for ( rch = to_room->people; rch; rch = rch->next_in_room )
                                act( "The $d is set back on its hinges.",
                                    rch, NULL, pexit_rev->keyword, TO_CHAR );
                }

                return TRUE;
        }

        return FALSE;
}


bool spec_grail (CHAR_DATA *ch)
{
        /*
         *  This special function used by Grail the Hunter.
         *  Written by Madboy/Jammer/Brutus, I'm guessing :)
         *
         *  Updated to include active extermination of killers and thieves
         *  by Gezhp, May 2000.
         *
         *  If more than one mob has this spec, some weirdness with the timer
         *  will occur.
         */

        CHAR_DATA       *victim;
        char            buf [MAX_STRING_LENGTH];
        int             random;
        static int      hunt_timer = 0;
        static int      warning_timer = -1;
        static int      attempts = 0;
        DESCRIPTOR_DATA *d;

        if( !IS_AWAKE( ch ))
                return FALSE;

        if (ch->fighting)
        {
                for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
                {
                        if (can_see(ch, victim)
                            && victim->fighting == ch
                            && !IS_NPC(victim)
                            && !number_bits(1))
                                break;
                }

                if (!victim)
                        victim = ch->fighting;

                random = number_range(0,3);

                switch (random)
                {
                    case 0:
                        do_say(ch, "I'm a bad motherfucker!");
                        do_headbutt(ch, victim->name);
                        break;

                    case 1:
                        do_say(ch, "Surprise!");
                        do_focus(ch, victim->name);
                        break;

                    default:
                        do_say(ch, "Feel my wrath!");
                        one_hit(ch, victim, gsn_thrust);
                }

                return TRUE;
        }

        if (--hunt_timer > 0)
        {
                if (ch->in_room->vnum != 75)
                {
                        act ("$c fades from view.", ch, NULL, NULL, TO_ROOM);
                        do_mpgoto (ch, "75");
                }

                return FALSE;
        }


        /*
         *  See if any killers or thieves are about.  Leave newbies with flags
         *  alone, because we're nice people, deep down.
         */
        for (d = descriptor_list; d; d = d->next)
        {
                if (d->connected == CON_PLAYING
                    && d->character
                    && d->character->level > 14
                    && (number_bits(3) < 3 || warning_timer > -1)
                    && d->character->level <= LEVEL_HERO
                    && (IS_SET (d->character->status, PLR_KILLER)
                        || IS_SET (d->character->status, PLR_THIEF)))
                {
                        if (warning_timer < 0)
                                warning_timer = 15;

                        if (warning_timer > 0)
                        {
                                if (ch->in_room->vnum != 75)
                                {
                                        act ("$c fades from view.", ch, NULL, NULL, TO_ROOM);
                                        do_mpgoto (ch, "75");
                                }

                                if (warning_timer-- == 5)
                                        do_shout (ch, "The guilty must be PUNISHED!");

                                return FALSE;
                        }

                        if (++attempts == 5
                            || d->character->in_room->vnum == ROOM_VNUM_PURGATORY_A)
                        {
                                hunt_timer = 150;
                                warning_timer = -1;
                                attempts = 0;
                        }

                        act ("$c cackles and fades from view.", ch, NULL, NULL, TO_ROOM);

                        /*  No spam kills thanks  */
                        if (d->character->in_room->vnum == ROOM_VNUM_PURGATORY_A)
                        {
                                do_mpgoto (ch, "75");
                                return FALSE;
                        }

                        do_mpgoto (ch, d->character->name);

                        /*  Double check!  */
                        if (ch->in_room != d->character->in_room)
                                break;

                        act ("$c appears from nowhere!", ch, NULL, NULL, TO_ROOM);
                        do_mpkill (ch, d->character->name);
                        return TRUE;
                }
        }


        /*
         *  No killers or thieves here so let's wander.
         *  Checks for killers and thieves are still included below,
         *  if above code is to be removed.
         */

        warning_timer = -1;

        for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if( victim != ch && !IS_NPC( victim )
                   && IS_SET( victim->status, PLR_KILLER )
                   && can_see( ch, victim ) )
                {
                        sprintf( buf, "%s, you are a foul killer!  I must avenge your "
                                "murdered victims!",
                                capitalize_initial( victim->name ) );
                        do_shout( ch, buf );
                        do_mpkill( ch, victim->name );
                        return TRUE;
                }

                if( victim != ch
                   && !IS_NPC( victim )
                   && IS_SET( victim->status, PLR_THIEF )
                   && can_see( ch, victim ) )
                {
                        sprintf( buf, "%s, you are a rank thief!  I must avenge your victims!",
                                capitalize_initial( victim->name ) );
                        do_shout( ch, buf );
                        do_mpkill( ch, victim->name );
                        return TRUE;
                }

                if( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
                        break;
        }

        if( !victim  )
        {
                switch( number_bits( 3 ) )
                {
                    case 0:
                    case 1:
                    case 2:
                        act("$c says 'I must atone, I must repay my debt...'", ch, NULL, NULL,
                            TO_ROOM );
                        return TRUE;

                    case 3:
                        act("$c says 'I must hunt elsewhere!'", ch, NULL, NULL, TO_ROOM );
                        if( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
                           || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) ) {
                                act( "$c's form shimmers and slowly fades to nothing.",
                                    ch, NULL, NULL, TO_ROOM );
                                do_mpgoto( ch, "75" );
                        }
                        else
                                spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );
                        return TRUE;
                }
        }
        else
        {
                switch( number_bits( 3 ) )
                {
                    case 0:
                        act("$c tells you, 'Fear me, for I am the cruellest of this realm...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_heal( skill_lookup( "heal" ), ch->level, ch, victim );
                        return TRUE;

                    case 1:
                        act("$c says, 'Blood, I must shed more blood for my masters...'", ch,
                            NULL, NULL, TO_ROOM);
                        spell_combat_mind( skill_lookup( "combat mind" ), ch->level,
                                          ch, victim );
                        return TRUE;

                    case 2:
                        act("$c asks you 'Be vigilant.  There are many agents of evil in this plane.'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_detect_evil( skill_lookup( "detect evil" ), ch->level,
                                          ch, victim );
                        return TRUE;

                    case 3:
                        act("$c tells you 'May you walk with the gods, and fear them...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
                        return TRUE;


                    case 4:
                        act("$c tells you 'I may never rest, I must hunt forever...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_shield( skill_lookup( "shield" ), ch->level, ch, victim );
                        return TRUE;

                    case 5:
                        act("$c says 'My steel thirsts for sinners' blood!'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_sanctuary( skill_lookup( "sanctuary" ), ch->level, ch, victim );
                        return TRUE;

                    case 6:
                        act("$c says 'My master, I must leave this sinful place...'",
                            ch, NULL, NULL, TO_ROOM );
                        if( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
                           || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
                        {
                                act( "$c's form shimmers and slowly fades to nothing.", ch, NULL,
                                    NULL, TO_ROOM );
                                do_mpgoto( ch, "75" );
                        }
                        else
                                spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );
                        return TRUE;
                }
        }

        return FALSE;
}


bool spec_bounty (CHAR_DATA *ch)
{
        /*
         * Temporary Grail spec does not make him hunt killers.
         * spec_grail_PROPER (above) is the original function.
         */

        CHAR_DATA       *victim;
        char            buf [MAX_STRING_LENGTH];

        if( !IS_AWAKE( ch ) || ch->fighting )
                return FALSE;

        for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if( victim != ch && !IS_NPC( victim )
                   && IS_SET( victim->status, PLR_KILLER )
                   && can_see( ch, victim ) )
                {
                        sprintf( buf, "%s, you are a foul killer!  I must avenge your "
                                "murdered victims!",
                                capitalize_initial( victim->name ) );
                        do_shout( ch, buf );
                        do_mpkill( ch, victim->name );
                        return TRUE;
                }

                if( victim != ch
                   && !IS_NPC( victim )
                   && IS_SET( victim->status, PLR_THIEF )
                   && can_see( ch, victim ) )
                {
                        sprintf( buf, "%s, you are a rank thief!  I must avenge your victims!",
                                capitalize_initial( victim->name ) );
                        do_shout( ch, buf );
                        do_mpkill( ch, victim->name );
                        return TRUE;
                }

                if( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
                        break;
        }

        if( !victim  )
        {
                switch( number_bits( 3 ) )
                {
                    case 0:
                    case 1:
                    case 2:
                        act("$c says 'I must atone, I must repay my debt...'", ch, NULL, NULL,
                            TO_ROOM );
                        return TRUE;

                    case 3:
                        act("$c says 'I must hunt elsewhere!'", ch, NULL, NULL, TO_ROOM );
                        if( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
                           || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) ) {
                                act( "$c's form shimmers and slowly fades to nothing.",
                                    ch, NULL, NULL, TO_ROOM );
                                do_mpgoto( ch, "75" );
                        }
                        else
                                spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );
                        return TRUE;
                }
        }
        else
        {
                switch( number_bits( 3 ) )
                {
                    case 0:
                        act("$c tells you 'Fear me, for I am the Cruelest of this Realm...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_heal( skill_lookup( "heal" ), ch->level, ch, victim );
                        return TRUE;

                    case 1:
                        act("$c says 'Blood, I must shed more blood for my Masters...'", ch,
                            NULL, NULL, TO_ROOM);
                        spell_combat_mind( skill_lookup( "combat mind" ), ch->level,
                                          ch, victim );
                        return TRUE;

                    case 2:
                        act("$c asks you 'Be vigilant.  There are many agents of Evil in this Plane.'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_detect_evil( skill_lookup( "detect evil" ), ch->level,
                                          ch, victim );
                        return TRUE;

                    case 3:
                        act("$c tells you 'May you walk with the Gods, and fear Them...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
                        return TRUE;


                    case 4:
                        act("$c tells you 'I may never rest, I must hunt forever...'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_shield( skill_lookup( "shield" ), ch->level, ch, victim );
                        return TRUE;

                    case 5:
                        act("$c says 'My steel thirsts for sinners' blood!'",
                            ch, NULL, NULL, TO_ROOM );
                        spell_sanctuary( skill_lookup( "sanctuary" ), ch->level, ch, victim );
                        return TRUE;

                    case 6:
                        act("$c says 'My Master, I must leave this sinful place...'",
                            ch, NULL, NULL, TO_ROOM );
                        if( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
                           || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
                        {
                                act( "$c's form shimmers and slowly fades to nothing.", ch, NULL,
                                    NULL, TO_ROOM );
                                do_mpgoto( ch, "75" );
                        }
                        else
                                spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );
                        return TRUE;
                }
        }

        return FALSE;
}


bool spec_cast_orb( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        if ( !IS_AWAKE( ch ) || ch->fighting )
                return FALSE;

        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
                if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
                        break;

        }

        if ( !victim || IS_NPC( victim ) )
                return FALSE;

        switch ( number_bits( 3 ) )
        {
            case 0:
                act( "$n pulses briefly.", ch, NULL, NULL, TO_ROOM );
                spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
                return TRUE;

            case 1:
                act( "$n pulses briefly.",    ch, NULL, NULL, TO_ROOM );
                spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
                return TRUE;

            case 2:
                act( "$n pulses briefly.",   ch, NULL, NULL, TO_ROOM );
                spell_heal( skill_lookup( "heal" ),
                           ch->level, ch, victim );
                return TRUE;

            case 3:
                act( "$n pulses briefly.",    ch, NULL, NULL, TO_ROOM );
                spell_combat_mind( skill_lookup( "combat mind" ),
                                  ch->level, ch, victim );
                return TRUE;

            case 4:
                act( "$n pulses briefly.",  ch, NULL, NULL, TO_ROOM );
                spell_cure_poison( skill_lookup( "cure poison" ),
                                  ch->level, ch, victim );
                return TRUE;

            case 5:
                act( "$n pulses briefly.", ch, NULL, NULL, TO_ROOM );
                spell_refresh( skill_lookup( "refresh" ), ch->level, ch, victim );
                return TRUE;

            case 6:
                act("$n pulses briefly.",ch,NULL,NULL,TO_ROOM);
                spell_sanctuary( skill_lookup( "sanctuary" ),
                                ch->level, ch, victim );
                return TRUE;

            case 7:
                act("$n pulses briefly.", ch, NULL, NULL, TO_ROOM );
                spell_cure_mana( skill_lookup( "cure mana" ),
                                ch->level, ch, victim );
                return TRUE;

        }

        return FALSE;
}


bool spec_buddha( CHAR_DATA *ch )
{
        if ( ch->position != POS_FIGHTING )
                return FALSE;

        switch ( number_bits( 3 ) )
        {
            case 0: return spec_breath_fire             ( ch );
            case 1:
            case 2: return spec_breath_lightning        ( ch );
            case 3: return spec_breath_gas              ( ch );
            case 4: return spec_breath_acid             ( ch );
            case 5: return spec_cast_cleric             ( ch );
            case 6:
            case 7: return spec_breath_frost            ( ch );
        }

        return FALSE;
}


bool spec_kungfu_poison( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        if ( ch->position != POS_FIGHTING
            || !( victim = ch->fighting )
            || !number_bits(2))
                return FALSE;

        act( "You hit $N with a poison palm technique!",  ch, NULL, victim, TO_CHAR    );
        act( "$n hits $N with the poison palm technique!",  ch, NULL, victim, TO_NOTVICT );
        act( "$n hits you with the poison palm technique!", ch, NULL, victim, TO_VICT    );

        spell_poison( gsn_poison, ch->level, ch, victim );

        return TRUE;
}

bool spec_assassin( CHAR_DATA *ch )
{
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        CHAR_DATA *v_next;
        int rnd_say;
        int random;

        if (!ch->fighting && ch->pIndexData->vnum != BOT_VNUM)
        {
                for (victim=ch->in_room->people; victim; victim = v_next)
                {
                        v_next = victim->next_in_room;

                        if (ch->position < POS_STANDING)  /* Prevent backstabbing if stunned etc --Owl 10/4/22 */
                                continue;

                        if (IS_NPC(victim))
                                continue;

                        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                                continue;

                        if (victim == ch)
                                continue;

                        if (victim->level > LEVEL_HERO)
                                continue;

                        if (victim->level > ch->level + 7)
                                continue;

                        if (victim->class == CLASS_THIEF)
                                continue;

                        break;
                }

                if (!victim)
                        return FALSE;

                rnd_say = number_range (1, 10);

                if ( rnd_say <= 5 && CAN_SPEAK(ch))
                        sprintf( buf, "Death is the true end...");

                else if ( rnd_say <= 6 && CAN_SPEAK(ch))
                        sprintf( buf, "Time to die...");

                else if ( rnd_say <= 7 && CAN_SPEAK(ch))
                        sprintf( buf, "Cabrone...");

                else if ( rnd_say <= 8 && CAN_SPEAK(ch))
                        sprintf( buf, "Welcome to your fate...");

                else if ( rnd_say <= 10 && CAN_SPEAK(ch))
                        sprintf( buf, "Ever dance with the devil?");

                do_say( ch, buf );

                one_hit( ch, victim, gsn_backstab );

                return TRUE;
        }
        else
        {
                /* in combat bit - Shade Sept 99 */

                for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
                {
                        if (can_see(ch, victim)
                            && victim->fighting == ch
                            && !IS_NPC(victim)
                            && !number_bits(1))
                                break;
                }

                if (!victim)
                        return FALSE;

                random = number_range(0,3);

                switch (random)
                {
                    case 0:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Now you see me..."); }
                        do_dirt_kick(ch, victim->name);
                        break;

                    case 1:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Down you go..."); }
                        do_trip(ch, victim->name);
                        break;

                    default:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Hey, look over there!"); }
                        one_hit(ch, victim, gsn_circle);
                }
        }

        return TRUE;
}


bool spec_warrior( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        switch( number_bits(2) )
        {
            case 0:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"Feel my head!!"); }
                do_headbutt(ch, victim->name);
                return TRUE;

            case 1:
                if ( CAN_SPEAK(ch) ) { do_say(ch, "Eat this!!"); }
                do_smash( ch, victim->name);
                return TRUE;

            default:
                if ( CAN_SPEAK(ch) ) { do_say(ch, "Mighty foot engaged!!"); }
                do_kick( ch, "");
                return TRUE;

        }

        return FALSE;
}


bool spec_vampire( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        switch (number_bits(2))
        {
            case 0:
            case 1:
                act("$c lunges for your neck!  You feel a sharp pain.",
                    ch, NULL, NULL, TO_ROOM );

                if (one_hit(ch, victim, gsn_suck))
                        aggro_damage(ch, victim, ch->level);

                return TRUE;

            case 2:
                if ((sn = skill_lookup("fireball")) < 0)
                        return FALSE;

                if ( CAN_SPEAK(ch) ) { do_say(ch, "Burn in fiery hell!"); }
                (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                return TRUE;

            case 3:
                if ((sn = skill_lookup("acid blast")) < 0)
                        return FALSE;

                act("Acid streams from $n's fingertips!", ch, NULL, NULL, TO_ROOM);
                (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
                return TRUE;
        }

        return FALSE;
}


bool spec_cast_archmage (CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        char *spell;
        int sn;
        char buf[MAX_STRING_LENGTH];

        if (ch->level < 26)
                return FALSE;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;
                int random = number_range(0,12);

                switch (random)
                {
                    case 0:
                    case 1:
                        min_level = 25;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Smoke on this..."); }
                        spell = "fireball";
                        break;

                    case 2:
                        min_level = 30;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Kaboom!!"); }
                        spell = "firestorm";
                        break;

                    case 3:
                        min_level = 35;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"This won't hurt a bit..."); }
                        spell = "dispel magic";
                        break;

                    case 4:
                        min_level = 40;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Feel your life force drain away!"); }
                        spell = "energy drain";
                        break;

                    case 5: min_level = 40;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"I am invincible!!"); }
                        spell = "meteor storm";
                        break;

                    case 6:
                        min_level = 45;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Come get some..."); }
                        spell = "prismatic spray";
                        break;

                    default:
                        min_level = 15;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Damn you're ugly..."); }
                        spell = "acid blast";
                }

                if (ch->level >= min_level)
                        break;
        }

        sn = skill_lookup(spell);

        if (!sn)
                return FALSE;

        do_say(ch,buf);

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
        return TRUE;
}


/*
 * Spec priestess - Shade
 */
bool spec_cast_priestess (CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        char *spell;
        int sn;
        int min_level;
        int random;
        char buf[MAX_STRING_LENGTH];

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                random = number_range(0,7);

                switch (random)
                {
                    case 0:
                        min_level = 1;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Heretic!"); }
                        spell = "curse";
                        break;

                    case 1:
                        min_level = 25;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "May God strike you blind!"); }
                        spell = "blindness";
                        break;

                    case 2:
                        min_level = 30;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "This won't hurt a bit..."); }
                        spell = "dispel magic";
                        break;

                    case 3:
                    case 4:
                        min_level = 40;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "I'm a mean bitch!"); }
                        spell = "unholy word";
                        break;

                    case 5:
                        min_level = 60;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "I sentence you to eternal damnation!"); }
                        spell = "hex";
                        break;

                    default: min_level = 20;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Die, infidel!"); }
                        spell = "flamestrike";
                }

                if (ch->level >= min_level)
                        break;
        }

        if ((sn = skill_lookup(spell)) < 0)
                return FALSE;

        do_say(ch,buf);
        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}


/*
 * spec_mast_vampire - Shade & Owl Sept 99
 */
bool spec_mast_vampire(CHAR_DATA *ch)
{
        int random, sn, cast_spell;
        char *spell;
        CHAR_DATA *victim;
        CHAR_DATA *v_next;
        char buf[MAX_STRING_LENGTH];

        /* Starting fight */
        if (!(victim = ch->fighting))
        {
                for ( victim = ch->in_room->people; victim; victim = v_next )
                {
                        v_next = victim->next_in_room;

                        if (IS_NPC(victim))
                                continue;

                        if (victim->sub_class == SUB_CLASS_VAMPIRE)
                                continue;

                        if (IS_AFFECTED(victim, AFF_NON_CORPOREAL))
                                continue;

                        if (victim->level > LEVEL_HERO)
                                continue;

                        break;
                }

                if (!victim)
                        return FALSE;

                random = number_range(0, 3);

                switch (random)
                {
                    case 0:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Ever dance with the devil by the pale moonlight?"); }
                        break;

                    case 1:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "You dare invade my sanctuary!?"); }
                        break;

                    case 2:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Velcome to your destiny, come join me beyond the grave."); }
                        break;

                    default:
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Ahhhh more blood!"); }
                }

                do_say(ch, buf);
                multi_hit(ch, victim, gsn_lunge);

                return TRUE;
        }


        random = number_range(0,3);
        cast_spell = 0;
        spell = "";

        switch(random)
        {
            case 0:
            case 1:
                if ( CAN_SPEAK(ch) ) { act("$n screams, 'The streets will run red with your {Rblood{x!'",
                    ch, NULL, NULL, TO_ROOM); }
                one_hit(ch, victim, gsn_suck);
                break;

            case 2:
                if ( CAN_SPEAK(ch) ) { sprintf(buf, "Flee puny mortal!"); }
                spell = "fear";
                cast_spell = 1;
                break;

            default:
                if ( CAN_SPEAK(ch) ) { sprintf(buf, "I will flay the flesh from your bones!"); }
                spell = "prismatic spray";
                cast_spell = 1;
                break;
        }


        if (cast_spell)
        {
                do_say(ch,buf);

                if ((sn = skill_lookup(spell)) < 0)
                        return FALSE;

                (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
        }

        return TRUE;
}


/*
 * Bloodsucking worms.  -Owl 27/04/00
 */
bool spec_bloodsucker( CHAR_DATA *ch )
{
        CHAR_DATA *victim;

        if (!( victim = ch->fighting )
            || number_percent() < 50 )
                return FALSE;

        act( "You lunge at $N's neck!", ch, NULL, victim, TO_CHAR    );
        act( "$n lunges at $N's neck!", ch, NULL, victim, TO_NOTVICT );
        act( "$n lunges at your neck!", ch, NULL, victim, TO_VICT    );

        if (one_hit( ch, victim,gsn_suck ))
                aggro_damage(ch, victim, ch->level);

        return TRUE;
}


/*
 *  Spectral minion special for "bastion.are".
 *  Gezhp May 2000
 */
bool spec_spectral_minion (CHAR_DATA* ch)
{
        int destination;

        const int rooms [40] =
        {
                30655, 30670, 30671, 30672, 30673, 30674, 30675, 30676, 30679, 30680,
                30681, 30682, 30683, 30684, 30686, 30687, 30688, 30689, 30690, 30693,
                30694, 30695, 30696, 30697, 30757, 30758, 30759, 30761, 30763, 30765,
                30767, 30770, 30771, 30773, 30775, 30777, 30779, 30783, 30784, 30769
        };

        static int timer = 5;

        if (!IS_NPC(ch) || ch->fighting || !IS_AWAKE(ch))
                return FALSE;

        if (--timer > 0)
                return FALSE;

        timer = 5;

        while (1)
        {
                destination = rooms [number_range (0, 39)];

                if (destination != ch->in_room->vnum)
                        break;
        }

        act ("{wA spectral figure shimmers and disappears before your eyes.{x",
             ch, NULL, NULL, TO_ROOM);
        char_from_room (ch);
        char_to_room (ch, get_room_index(destination));
        act ("{wA spectral figure materialises before you and issues a blood-chilling shriek.{x",
             ch, NULL, NULL, TO_ROOM);

        return TRUE;
}



bool spec_celestial_repairman (CHAR_DATA *ch)
{
        /*
         *  Wander the world fixing broken doors; Gezhp 2000
         */

        EXIT_DATA       *pexit;
        EXIT_DATA       *pexit_rev;
        ROOM_INDEX_DATA *to_room;
        int door;


        if (!IS_AWAKE(ch))
                return FALSE;

        switch (number_bits(3))
        {
            default:
                door = number_range (0, 5);

                if (!(pexit = ch->in_room->exit[door]))
                        return FALSE;

                if (IS_SET(pexit->exit_info, EX_BASHED))
                {
                        REMOVE_BIT(pexit->exit_info, EX_BASHED);
                        act ("You repair the $d.", ch, NULL, pexit->keyword, TO_CHAR);
                        act ("$n skilfully repairs the $d.", ch, NULL, pexit->keyword, TO_ROOM);

                        if ((to_room = pexit->to_room)
                            && (pexit_rev = to_room->exit[directions[door].reverse])
                            && pexit_rev->to_room == ch->in_room)
                        {
                                CHAR_DATA *rch;
                                REMOVE_BIT(pexit_rev->exit_info, EX_BASHED);

                                for (rch = to_room->people; rch; rch = rch->next_in_room)
                                        act ("The $d is set back on its hinges.",
                                             rch, NULL, pexit_rev->keyword, TO_CHAR);
                        }
                        return TRUE;
                }
                return FALSE;

            case 0:
                if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
                    || IS_SET(ch->in_room->room_flags, ROOM_CONE_OF_SILENCE))
                {
                        act( "$c's form shimmers and slowly fades to nothing.",
                            ch, NULL, NULL, TO_ROOM );
                        do_mpgoto( ch, "75" );
                }
                else
                        spell_teleport( skill_lookup( "teleport" ), ch->level, ch, ch );

                return TRUE;
        }
}


/*
 * Gezhp Dec 2000; for Sahuagin Colony area
 */
bool spec_sahuagin (CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        int sn;
        char *spell;
        bool target_self = FALSE;

        if (!IS_AWAKE(ch))
                return FALSE;

        if (ch->fighting)
        {
                for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
                {
                        if (victim->deleted)
                                continue;

                        if (victim->fighting == ch && (number_bits(3) < 3))
                                break;
                }

                if (!victim)
                        return FALSE;

                switch (number_bits(3))
                {
                    case 0:
                    case 1:
                        if (is_affected(ch, gsn_protection))
                                return FALSE;

                        spell = "protection";
                        act("$c's flesh begins to harden and swell!",
                            ch, NULL, NULL, TO_ROOM);
                        target_self = TRUE;
                        break;

                    case 2:
                    case 3:
                        if (ch->hit >= ch->max_hit)
                                return FALSE;

                        spell = "heal";
                        target_self = TRUE;
                        act("$c's wounds seem to disappear before your very eyes.",
                            ch, NULL, NULL, TO_ROOM);
                        break;

                    case 4:
                    case 5:
                        spell = "shocking grasp";
                        act("$c reaches for you with $s horrible claws...",
                            ch, NULL, victim, TO_VICT);
                        act("$c reaches for $N with $s horrible claws...",
                            ch, NULL, victim, TO_NOTVICT);
                        break;

                    case 6:
                        spell = "weaken";
                        act("$c reaches for you with $s horrible claws...",
                            ch, NULL, victim, TO_VICT);
                        act("$c reaches for $N with $s horrible claws...",
                            ch, NULL, victim, TO_NOTVICT);
                        break;

                    default:
                        if (is_affected(ch, gsn_berserk))
                                return FALSE;

                        do_berserk(ch, "");
                        return TRUE;
                }

                if ((sn = skill_lookup(spell)) < 0)
                        return FALSE;

                (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );
                return TRUE;
        }

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (!IS_NPC(victim)
                    && victim->level < ch->level + 10
                    && can_see(ch, victim)
                    && !(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                         || IS_SET(ch->in_room->area->area_flags, AREA_FLAG_SAFE))
                    && victim->level <= LEVEL_HERO
                    && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        if (number_bits(3) < 3)
        {
                if ( CAN_SPEAK(ch) ) { do_say(ch, "Surface dweller, DIE!"); }
                multi_hit(victim, ch, TYPE_UNDEFINED);
                return TRUE;
        }

        act("$c growls deeply at $N...", ch, NULL, victim, TO_NOTVICT);
        act("$c growls deeply at you...", ch, NULL, victim, TO_VICT);
        return FALSE;
}


/*
 * Evil, evil self-healing and disintegration spec;
 * Designed for Dahij, poor old guy; Gezhp 2001
 */
bool spec_evil_evil_gezhp (CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        const int sn_disintegrate = skill_lookup("disintegrate");
        const int sn_complete_heal = skill_lookup("complete heal");

        if (!IS_AWAKE(ch))
                return FALSE;

        if (ch->fighting)
        {
                for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
                {
                        if (victim->deleted)
                                continue;

                        if (victim->fighting == ch && number_bits(1))
                                break;
                }

                if (!victim)
                        return FALSE;

                if (number_bits(1) && sn_disintegrate >= 0)
                {
                        do_say(ch, "Die...");
                        act("White beams streak from $n's fingertips!",
                            ch, NULL, NULL, TO_ROOM);
                        (*skill_table[sn_disintegrate].spell_fun)
                                (sn_disintegrate, ch->level, ch, victim);
                        return TRUE;
                }

                else if (sn_complete_heal >= 0
                         && ch->hit < ch->max_hit * 3 / 4)
                {
                        do_say(ch, "I cannot be defeated!");
                        (*skill_table[sn_complete_heal].spell_fun)
                                (sn_complete_heal, ch->level, ch, ch);
                        return TRUE;
                }
        }

        return FALSE;
}

/* demon spec, -Owl 3/2/08 */

bool spec_demon( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;

                switch ( number_bits( 4 ) )
                {
                    case  0: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RCurse you, mortal scum!{x"); }
                                min_level =  0; spell = "curse";          break;
                    case  1:
                    case  2: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RFreeze, terrestrial vermin!{x"); }
                                min_level =  3; spell = "chill touch";    break;
                    case  3: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RBurn!{x"); }
                                min_level =  5; spell = "burning hands";  break;
                    case  4: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RGive me your power!{x"); }
                                min_level = 12; spell = "steal strength"; break;
                    case  5: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RAhh.. sweet lifeforce...{x"); }
                                min_level = 15; spell = "energy drain";   break;
                    case  6: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RYour life belongs to ME!{x"); }
                                min_level = 20; spell = "wither";         break;
                    case  7:
                    case  8: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RBe still, carrion!{x"); }
                                min_level = 18; spell = "abyssal hand";break;
                    case  9: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RFEEL MY MASTER'S CURSE!{x"); }
                                min_level = 40; spell = "hex";            break;
                    case  10:
                    case  11: min_level = 30; spell = "gate";             break;
                    case  12: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RPerish in flame!{x"); }
                                min_level = 50; spell = "fire breath";    break;
                    default: if ( CAN_SPEAK(ch) ) { do_say(ch,"{RBy the Dark Lord's power!{x"); }
                                min_level = 25; spell = "hellfire";     break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );

        return TRUE;
}

/* For 'electric' mobs, electric eels etc.  Owl 10/7/22 */

bool spec_cast_electric (CHAR_DATA *ch)
{
        CHAR_DATA *victim;
        char *spell;
        int sn;
        char buf[MAX_STRING_LENGTH];

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
                int min_level;
                int random = number_range(0,12);

                switch (random)
                {
                    case 0:
                    case 1:
                        min_level = 1;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Let's hold hands..."); }
                        spell = "shocking grasp";
                        break;

                    case 2:
                        min_level = 16;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"I need a recharge... you'll do!"); }
                        spell = "energy drain";
                        break;

                    case 3:
                        min_level = 25;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Hear me, gods of the storm!"); }
                        spell = "call lightning";
                        break;

                    case 4:
                        min_level = 30;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Let's charge your BRAIN!"); }
                        spell = "synaptic blast";
                        break;

                    case 5:
                        min_level = 40;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Let's share the love around..."); }
                        spell = "chain lightning";
                        break;

                    case 6:
                        min_level = 50;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf, "Behold, the electric breath of DEATH!"); }
                        spell = "lightning breath";
                        break;

                    default:
                        min_level = 5;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Hahaha, feel my power!"); }
                        spell = "lightning bolt";
                }

                if (ch->level >= min_level)
                        break;
        }

        sn = skill_lookup(spell);

        if (!sn)
                return FALSE;

        do_say(ch,buf);

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
        return TRUE;
}

bool spec_small_whale( CHAR_DATA *ch )
{
        /* give to dolphins, humanoid-sized whales */
        CHAR_DATA *victim;
        int random;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        random = number_range(0,1);

        switch( random )
        {
            case 0:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"It's slappin' time!"); }
                do_flukeslap(ch, victim->name);
                return TRUE;
            case 1:
                break;
            default:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"It's slappin' time!"); }
                do_flukeslap(ch, victim->name);
                return TRUE;
        }

        return FALSE;
}

bool spec_large_whale( CHAR_DATA *ch )
{
        /* A large whale big enough to swallow players */
        CHAR_DATA *victim;

        int random;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        random = number_range(0,3);

        switch( random )
        {
            case 0:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"It's slappin' time!"); }
                do_flukeslap(ch, victim->name);
                return TRUE;
            case 1:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"Oh you DO look delicious!"); }
                do_swallow(ch, victim->name);
                return TRUE;
            case 2:
                break;
            default:
                if ( CAN_SPEAK(ch) ) { do_say(ch,"It's slappin' time!"); }
                do_flukeslap(ch, victim->name);
                return TRUE;
        }

        return FALSE;
}

bool spec_kappa( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;
        char buf[MAX_STRING_LENGTH];

        spell = "fury of nature";
        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Mmmmm, how's the water?"); }

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 11) )
                {
                    case  0:
                    case  1:

                    case  2:

                    case  3:

                    case  4:
                        min_level = 11;
                        if (is_affected(ch, gsn_inertial_barrier))
                        {
                                break;
                        }
                        else {
                                if ( CAN_SPEAK(ch) ) { sprintf(buf,"Time to harden up!"); }
                                spell = "inertial barrier";
                                target_self = TRUE;
                                break;
                        }

                    case  5:
                        min_level = 5;
                        if (is_affected(ch, gsn_armor))
                        {
                                break;
                        }
                        else {
                                if ( CAN_SPEAK(ch) ) { sprintf(buf,"You think the only protection I have is this SHELL?"); }
                                spell = "armor";
                                target_self = TRUE;
                                break;
                        }

                    case  6:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Meet my melon!"); }
                        do_flying_headbutt( ch, victim->name);
                        return TRUE;

                    case  7:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Like to play a little footsie?"); }
                        do_kick( ch, "");
                        return TRUE;

                    case  8:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Let's take it down to pound town!"); }
                        do_grapple( ch, victim->name);
                        return TRUE;

                    case  9:
                        min_level = 25;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"You think you can hurt ME?"); }
                        spell = "power heal";
                        target_self = TRUE;
                        break;

                    case  10:

                    default:
                        min_level = 35;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Great weather we're having, no?"); }
                        spell = "fury of nature";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        do_say(ch,buf);

        if (target_self)
                act ("$c clutches at $s chest with $s claws.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c waves $s claws at $N.", ch, NULL, victim, TO_NOTVICT);
                act ("$c waves $s claws at you.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;

}

bool spec_laghathti( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;

        spell = "energy drain";

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 11) )
                {
                    case  0:
                        if (is_affected(victim, gsn_coil))
                        {
                                do_strangle( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  1:
                        if (is_affected(victim, gsn_coil))
                        {
                                do_constrict( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  2:
                        min_level = 11;
                        if (is_affected(victim, gsn_psychic_drain))
                        {
                                break;
                        }
                        else {
                                spell = "psychic drain";
                                break;
                        }

                    case  3:
                        if (is_affected(victim, gsn_coil))
                        {
                                do_constrict( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  4:
                        min_level = 11;
                        if (is_affected(victim, gsn_ego_whip))
                        {
                                break;
                        }
                        else {
                                spell = "ego whip";
                                break;
                        }

                    case  5:
                        if (is_affected(victim, gsn_coil))
                        {
                                do_strangle( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  6:
                        if (!is_affected(victim, gsn_coil))
                        {
                                do_coil( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  7:
                        if (!is_affected(victim, gsn_coil))
                        {
                                do_coil( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  8:
                        do_grapple( ch, victim->name);
                        return TRUE;

                    case  9:
                        min_level = 25;
                        if (is_affected(victim, gsn_feeblemind))
                        {
                                break;
                        }
                        else {
                                spell = "feeblemind";
                                break;
                        }

                    case  10:

                    default:
                        min_level = 35;
                        spell = "energy drain";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        if (target_self)
                act ("$c strokes $mself with $s hideous tentacles.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c waves $s hideous tentacles at $N.", ch, NULL, victim, TO_NOTVICT);
                act ("$c waves $s hideous tentacles at you.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;

}

bool spec_superwimpy( CHAR_DATA *ch )
{
        /* For cowardly magic-using mobs that just want to be left ALONE */

        CHAR_DATA *victim;
        EXIT_DATA *pexit;
        char       *spell;
        int        sn;
        bool       target_self;
        char       buf[MAX_STRING_LENGTH];
        int        door;
        int        i;
        bool       stopped_fight;

        door    = 0;
        spell   = "fear";
        stopped_fight = FALSE;

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (1, 6) )
                {
                    case  1:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Just go HOME, you bully!"); }
                        do_recall(victim, "");
                        return TRUE;

                    case  2:
                        min_level = 1;
                        if ( CAN_SPEAK(ch) ) { sprintf(buf,"Get out of here!  Shoo!"); }
                        spell = "fear";
                        break;

                    case  3:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Just STOP!"); }
                        stop_fighting( ch, TRUE );
                        stopped_fight = TRUE;
                        for (i = 0;i <=3; i++)
                        {
                                door = rand() % 5;
                                if ( (door <= 5)
                                && (pexit = ch->in_room->exit[door])
                                && pexit->to_room
                                && !IS_SET(pexit->exit_info, EX_CLOSED)
                                && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
                                && (stopped_fight == TRUE))
                                {
                                        move_char(ch, door);
                                        stopped_fight = FALSE;
                                }
                        }

                        stopped_fight = FALSE;
                        return TRUE;

                    case  4:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "You can't hurt what you can't see!"); }
                        do_smoke_bomb( ch, victim->name);
                        return TRUE;

                    case  5:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Leave me alone!  I didn't do anything!"); }
                        do_flee( ch, "");
                        return TRUE;

                    default:
                        if ( CAN_SPEAK(ch) ) { do_say(ch, "Leave me alone!  Stop hurting me!"); }
                        do_flee( ch, "");
                        return TRUE;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        do_say(ch,buf);

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;

}

bool spec_uzollru( CHAR_DATA *ch )
{
        CHAR_DATA *victim;
        char      *spell;
        int        sn;
        bool       target_self;

        spell = "poison";

        for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
        {
                if (victim->deleted)
                        continue;

                if (victim->fighting == ch && !number_bits(1))
                        break;
        }

        if (!victim)
                return FALSE;

        while (1)
        {
            int min_level;
            target_self = FALSE;

                switch ( number_range (0, 9) )
                {
                    case  0:
                        if (is_affected(victim, gsn_coil))
                        {
                                act( "You attempt to pull your tentacles tight about the neck of $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n attempts to pull $s tentacles tight about your neck!", ch, NULL, victim, TO_VICT  );
                                act( "$n attempts to pull $s tentacles tight about the neck of $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_strangle( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  1:
                        if (is_affected(victim, gsn_coil))
                        {
                                act( "You tighten your tentacles around $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n tightens the grip of $s tentacles on you!", ch, NULL, victim, TO_VICT  );
                                act( "$n tightens the grip of $s tentacles on $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_constrict( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  2:
                        min_level = 1;
                        act( "Your tentacles burrow into $N!",  ch, NULL, victim, TO_CHAR    );
                        act( "$n's tentacles burrow into you...", ch, NULL, victim, TO_VICT  );
                        act( "$n's tentacles burrow into $N!",  ch, NULL, victim, TO_NOTVICT );
                        spell_poison( gsn_poison, ch->level, ch, victim );
                        one_hit(ch, victim, gsn_suck);
                        one_hit(ch, victim, gsn_suck);
                        return TRUE;

                    case  3:
                        if (is_affected(victim, gsn_coil))
                        {
                                act( "You tighten your tentacles around $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n tightens the grip of $s tentacles on you!", ch, NULL, victim, TO_VICT  );
                                act( "$n tightens the grip of $s tentacles on $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_constrict( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  4:
                        min_level = 1;
                        act( "Your tentacles burrow into $N' flesh!",  ch, NULL, victim, TO_CHAR    );
                        act( "$n's tentacles burrow into your flesh!", ch, NULL, victim, TO_VICT    );
                        act( "$n's tentacles burrow into $N's flesh!",  ch, NULL, victim, TO_NOTVICT );
                        spell_poison( gsn_poison, ch->level, ch, victim );
                        one_hit(ch, victim, gsn_suck);
                        one_hit(ch, victim, gsn_suck);
                        return TRUE;

                    case  5:
                        if (is_affected(victim, gsn_coil))
                        {
                                act( "You attempt to pull your tentacles tight about the neck of $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n attempts to pull $s tentacles tight about your neck!", ch, NULL, victim, TO_VICT  );
                                act( "$n attempts to pull $s tentacles tight about the neck of $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_strangle( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  6:
                        if (!is_affected(victim, gsn_coil))
                        {
                                act( "You wrap your hideous tentacles around $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n wraps $s hideous tentacles around you!", ch, NULL, victim, TO_VICT  );
                                act( "$n wraps $s hideous tentacles around $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_coil( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  7:
                        if (!is_affected(victim, gsn_coil))
                        {
                                act( "You wrap your hideous tentacles around $N!",  ch, NULL, victim, TO_CHAR  );
                                act( "$n wraps $s hideous tentacles around you!", ch, NULL, victim, TO_VICT  );
                                act( "$n wraps $s hideous tentacles around $N!",  ch, NULL, victim, TO_NOTVICT );
                                do_coil( ch, victim->name);
                                return TRUE;
                        }
                        else {
                                return TRUE;
                        }

                    case  8:
                        min_level = 25;
                        if (is_affected(victim, gsn_feeblemind))
                        {
                                break;
                        }
                        else {
                                spell = "feeblemind";
                                break;
                        }

                    default:
                        min_level = 1;
                        spell = "poison";
                        break;
                }

                if ( ch->level >= min_level )
                        break;
        }

        if ( ( sn = skill_lookup( spell ) ) < 0 )
                return FALSE;

        if (target_self)
                act ("$c strokes $mself with $s hideous tentacles.", ch, NULL, NULL, TO_ROOM);
        else
        {
                act ("$c caresses $N with $s hideous tentacles.", ch, NULL, victim, TO_NOTVICT);
                act ("$c caresses you with $s hideous tentacles.", ch, NULL, victim, TO_VICT);
        }

        (*skill_table[sn].spell_fun) ( sn, ch->level, ch, target_self ? ch : victim );

        return TRUE;

}

