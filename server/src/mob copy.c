/***************************************************************************
 *  mob.c
 *
 *  Mapping system for Dragons Domain III
 *
 *  
 *
 * 
 *
 ***************************************************************************/
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"


struct race_type
{
	char      race_name[15];           /* Race Names */
	int    race_mod[5];             /* Str, Dex, Int, Wis, Con */
	int    race_class[MAX_CLASS];   /* Ran, Gla, Mar, Nin, Ele, Ill, Mon, Nec */
	int    max_speed;               /* 0-walk 1-normal 2-jog 3-run 4-haste */
	int    move_rate;               /* .66 to 1.5  by 1 to 4 */
	int    vision;                  /* 0-normal 1-night 2-dark */
	int    hp_mod;                  /* bonus when advancing a level */
	int    mana_mod;
	int    move_mod;
	char      race_special[80];        /* info about abilities for start */
	char      race_description[512];
	sh_int    enter;                   /* first reincarnation allowed */
	sh_int    leave;                   /* last reincarnation allowed */
	sh_int    weight;                  /* weight in pounds */
	sh_int    height;                  /* height in feet */
	int       res_low;                 /* Resistances */
	int       res_high;
	int       vul_low;                 /* Vulnerabilities */
	int       vul_high;
	long long flags;                   /* specialties */
	int       attack_parts;            /* body parts race attacks with */
	int       language;
	sh_int    min_appearance[APPEAR_MAX];
	sh_int    max_appearance[APPEAR_MAX];
};

struct bitvector_type	/* text of particular body parts */
{
	char				* name;	/* name of bit */
	long long				value;
};

struct body_type  /* text of particular body parts */
{
	char				* name;		/* names of body part when sliced */
	char				* short_descr;	/* short_descr of body part when sliced */
	char				* long_descr;	/* long_descr of body part when sliced */
	char 		* description;	/* description of sliced body part */
	char				* attack;		/* kind of attack the body part does */
	char				* sliced;		/* what it does when it is sliced off */
	int					dam_type;
};

extern	char * const	part_flags 	[];

BODY_HEAD|BODY_MOUTH|BODY_EYE|BODY_TORSO|BODY_HIP|BODY_LEG|BODY_ARM|BODY_WING|BODY_TAIL|BODY_TENTACLE|BODY_HORN|BODY_CLAW|BODY_HAND|BODY_FOOT|BODY_BRANCH|BODY_ROOT|BODY_TWIG|BODY_TRUNK


smaug:

typedef enum
{
  ATCK_BITE, ATCK_CLAWS, ATCK_TAIL, ATCK_STING, ATCK_PUNCH, ATCK_KICK,
  ATCK_TRIP, ATCK_BASH, ATCK_STUN, ATCK_GOUGE, ATCK_BACKSTAB, ATCK_FEED,
  ATCK_DRAIN, ATCK_FIREBREATH, ATCK_FROSTBREATH, ATCK_ACIDBREATH,
  ATCK_LIGHTNBREATH, ATCK_GASBREATH, ATCK_POISON, ATCK_NASTYPOISON, ATCK_GAZE,
  ATCK_BLINDNESS, ATCK_CAUSESERIOUS, ATCK_EARTHQUAKE, ATCK_CAUSECRITICAL,
  ATCK_CURSE, ATCK_FLAMESTRIKE, ATCK_HARM, ATCK_FIREBALL, ATCK_COLORSPRAY,
  ATCK_WEAKEN, ATCK_SPIRALBLAST, ATCK_POUNCE, MAX_ATTACK_TYPE
} attack_types;

/*
 * Resistant Immune Susceptible flags
 */
#define RIS_FIRE		  BV00
#define RIS_COLD		  BV01
#define RIS_ELECTRICITY		  BV02
#define RIS_ENERGY		  BV03
#define RIS_BLUNT		  BV04
#define RIS_PIERCE		  BV05
#define RIS_SLASH		  BV06
#define RIS_ACID		  BV07
#define RIS_POISON		  BV08
#define RIS_DRAIN		  BV09
#define RIS_SLEEP		  BV10
#define RIS_CHARM		  BV11
#define RIS_HOLD		  BV12
#define RIS_NONMAGIC		  BV13
#define RIS_PLUS1		  BV14
#define RIS_PLUS2		  BV15
#define RIS_PLUS3		  BV16
#define RIS_PLUS4		  BV17
#define RIS_PLUS5		  BV18
#define RIS_PLUS6		  BV19
#define RIS_MAGIC		  BV20
#define RIS_PARALYSIS		  BV21
#ifdef ENABLE_WEAPONPROF
#define RIS_HACK      BV22
#define RIS_LASH      BV23
#endif


/*
 * Body parts
 */
#define PART_HEAD		  BV00
#define PART_ARMS		  BV01
#define PART_LEGS		  BV02
#define PART_HEART		  BV03
#define PART_BRAINS		  BV04
#define PART_GUTS		  BV05
#define PART_HANDS		  BV06
#define PART_FEET		  BV07
#define PART_FINGERS		  BV08
#define PART_EAR		  BV09
#define PART_EYE		  BV10
#define PART_LONG_TONGUE	  BV11
#define PART_EYESTALKS		  BV12
#define PART_TENTACLES		  BV13
#define PART_FINS		  BV14
#define PART_WINGS		  BV15
#define PART_TAIL		  BV16
#define PART_SCALES		  BV17
/* for combat */
#define PART_CLAWS		  BV18
#define PART_FANGS		  BV19
#define PART_HORNS		  BV20
#define PART_TUSKS		  BV21
#define PART_TAILATTACK		  BV22
#define PART_SHARPSCALES	  BV23
#define PART_BEAK		  BV24

#define PART_HAUNCH		  BV25
#define PART_HOOVES		  BV26
#define PART_PAWS		  BV27
#define PART_FORELEGS		  BV28
#define PART_FEATHERS		  BV29
#define PART_HUSK_SHELL		  BV30

/* race dedicated stuff */
struct race_type
{
  char race_name[16];		
  EXT_BV affected;		
  sh_int str_plus;		
  sh_int dex_plus;	
  sh_int wis_plus;	
  sh_int int_plus;		
  sh_int con_plus;		
  sh_int cha_plus;		
  sh_int lck_plus;	
  sh_int hit;
  sh_int mana;
  int resist;
  int suscept;
  int class_restriction;	
  int language;			
  sh_int ac_plus;
  sh_int alignment;
  EXT_BV attacks;
  EXT_BV defenses;
  sh_int minalign;
  sh_int maxalign;
  sh_int exp_multiplier;
  sh_int height;
  sh_int weight;
  sh_int hunger_mod;
  sh_int thirst_mod;
  sh_int saving_poison_death;
  sh_int saving_wand;
  sh_int saving_para_petri;
  sh_int saving_breath;
  sh_int saving_spell_staff;
  char *where_name[MAX_WHERE_NAME];
  sh_int mana_regen;
  sh_int hp_regen;
  sh_int race_recall;
};

struct mob_type
{
  char mob_species[32];
  char mob_greater_species[32];
  int spell_resist;
  int spell_vuln;
  int physical_resist;
  int physical_vuln;
  int hp_mod;
  int dam_mod;
  int height;
  int weight;
  int size;
  int body_parts;
  int attack_parts;            
  int language;
  SPEC_FUN *  spec_fun_1;
  SPEC_FUN *  spec_fun_2;
  SPEC_FUN *  spec_fun_boss;
}
*/

/*
 *  Skill table
 */
const struct mob_type mob_table [MAX_MOB] =
{
  /*
    * name, pointer to gsn
    * practice type, target, minimum position,
    * pointer to spell function, minimum mana, beats
    * damage noun, wear off message
    *
    * When adding new spells add them near the end, just
    * before the base skills.  Elements are accessed by
    * index and results will be unpredictable (BUT BAD)
    * if the current order gets screwed up.
    */
  {
          "goat", "mammal",
          1, 1, 1, 
          20, 20, -10, -2, 
          1, 30,
          22, 12, 3,
          "spec_default", "NULL", "spec_default"
  },

  {
          "fire_elemental", "elemental",
          1, 1, 1, 
          20, 100, 2, 20, 
          1, 30,
          22, 12, 3,
          "spec_default", "NULL", "spec_default"
  }

}
  