#include "version.h"

#define PETSKILL_NONE ( 0 )
#define PETSKILL_NORMALATTACK ( 1 )
#define PETSKILL_NORMALGUARD ( 2 )
#define PETSKILL_GUARDBREAK ( 3 )
#define PETSKILL_CONTINUATIONATTACK1 ( 10 )
#define PETSKILL_CONTINUATIONATTACK2 ( 11 )
#define PETSKILL_CONTINUATIONATTACK3 ( 12 )
#define PETSKILL_CONTINUATIONATTACK4 ( 13 )
#define PETSKILL_GUARDIAN ( 25 )
#define PETSKILL_CHARGE1 ( 30 )
#define PETSKILL_CHARGE2 ( 31 )
#define PETSKILL_MIGHTY1 ( 40 )
#define PETSKILL_MIGHTY2 ( 41 )
#define PETSKILL_POWERBALANCE1 ( 50 )
#define PETSKILL_POWERBALANCE2 ( 51 )
#define PETSKILL_POWERBALANCE3 ( 52 )
#define PETSKILL_POISON_ATTACK1 ( 60 )
#define PETSKILL_POISON_ATTACK2 ( 61 )
#define PETSKILL_STONE ( 80 )
#define PETSKILL_CONFUSION_ATTACK ( 90 )
#define PETSKILL_DRUNK_ATTACK ( 100 )
#define PETSKILL_SLEEP_ATTACK ( 110 )
#define PETSKILL_EARTHROUND1 ( 120 )
#define PETSKILL_Abduct ( 130 )
#define PETSKILL_STEAL ( 140 )
#define PETSKILL_NOGUARD1 ( 150 )
#define PETSKILL_NOGUARD2 ( 151 )
#define PETSKILL_NOGUARD3 ( 152 )
#define PETSKILL_MERGE ( 200 )
#define PETSKILL_MERGE2 ( 201 )
#ifdef _PSKILL_FALLGROUND
#define PETSKILL_FALLGROUND ( 210 )
#endif
#ifdef _BATTLESTEAL_FIX
#define PETSKILL_STEALMONEY ( 211 )
#endif
#ifdef _PRO_BATTLEENEMYSKILL
#define ENEMYSKILL_RELIFES ( 500 )
#define ENEMYSKILL_RELIFHP ( 501 )
#define ENEMYSKILL_HELP ( 502 )
#endif
#ifdef _SKILL_DAMAGETOHP
#define PETSKILL_DAMAGETOHP ( 503)
#endif
#ifdef _Skill_MPDAMAGE
#define PETSKILL_MPDAMAGE ( 504)
#endif
#ifdef _SKILL_WILDVIOLENT_ATT
#define PETSKILL_WILDVIOLENTATTACK ( 540)//狂暴 vincent add 2002/05/16
#endif
#ifdef _SKILL_SPEEDY_ATT
#define PETSKILL_SPEEDYATTACK ( 541)     //疾速 vincent add 2002/05/16
#endif
#ifdef _SKILL_GUARDBREAK2
#define PETSKILL_GUARDBREAK2 ( 542)      //破除防御2 vincent add 2002/05/20
#endif
#ifdef _SKILL_SACRIFICE
#define PETSKILL_SACRIFICE ( 543)     //救援 vincent add 2002/05/30
#endif

#ifdef _SKILL_WEAKEN
#define PETSKILL_WEAKEN ( 544)           //虚弱 vincent add 2002/07/11
#endif

#ifdef _SKILL_DEEPPOISON
#define PETSKILL_DEEPPOISON ( 545)       //剧毒 vincent add 2002/07/16
#endif

#ifdef _SKILL_BARRIER
#define PETSKILL_BARRIER ( 546)          //魔障 vincent add 2002/07/16
#endif

#ifdef _SKILL_ROAR
#define PETSKILL_ROAR ( 548)             //大吼 vincent add 2002/07/11
#endif

#ifdef _ITEM_INSLAY
#define PETSKILL_INSLAY (572)
#endif

#ifdef _PETSKILL_FIXITEM
#define PETSKILL_FIXITEM (573)
#endif

#ifdef _SKILL_TOOTH
#define PETSKILL_TOOTHCRUSHE (574)
#endif

#ifdef _SKILL_REFRESH
#define PETSKILL_REFRESH ( 575)     //vincent add 2002/08/08
#endif

#ifdef _VARY_WOLF
#define PETSKILL_VARY ( 600 )
#endif

#ifdef _PETSKILL_SETDUCK
#define PETSKILL_SETDUCK ( 600 )
#endif

#ifdef _MAGICPET_SKILL
#define PETSKILL_SETMAGICPET ( 601 )
#endif

#ifdef _BATTLE_LIGHTTAKE
#define PETSKILL_Light ( 608)
#endif

#ifdef _BATTLE_ATTCRAZED
#define PETSKILL_AttCrazed ( 608)
#endif

#ifdef _PETSKILL_BECOMEPIG
#define PETSKILL_BECOMEPIG ( 628 )
#endif

#ifdef _PETSKILL_BATTLE_MODEL
#define PETSKILL_BATTLE_MODEL ( 635 )
#endif

