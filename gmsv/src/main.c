#include "version.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "init.h"
#include "net.h"
#include "handletime.h"
#include "char.h"
#include "map_warppoint.h"
#include "npcgen.h"
#include "log.h"
#include "configfile.h"
#include "battle.h"
#include "petmail.h"
#include "autil.h"
#include "family.h"


static void ShutdownProc(void);

void mainloop(void);

struct tm tmNow, tmOld;

void family_proc();

int main(int argc, char **argv, char **env) {
  setNewTime();
/*
    if ( argc > 1 && 0==strcmp(argv[1],"-v"))
    {
        printf("%s",genver);
        exit(0);
    }
    else fprintf(stderr,"%s",genver);
*/

  EXITWITHEXITCODEIFFALSE(util_Init(), 1);

  LoadAnnounce();  // Arminius 7.12 loginannounce

  memcpy(&tmOld, localtime((time_t *) &NowTime.tv_sec), sizeof(tmNow));

  EXITWITHEXITCODEIFFALSE(init(argc, argv, env), 1);

  LoadPetTalk();  // Arminius 8.14 pet talk

#ifdef _GAMBLE_BANK
  Load_GambleBankItems();
#endif

#ifdef _CFREE_petskill
  Load_PetSkillCodes();
#endif

  mainloop();

  return 0;
}

void mainloop(void) {
  print("初始化NPC...");
  NPC_generateLoop(1);
  print("完成\n");
  print("初始化signal1...");
  signal(SIGUSR1, sigusr1);
  print("完成\n");
  print("初始化signal2...");
  signal(SIGUSR2, sigusr2);
  print("完成\n");
  
  print("初始化地图传送点...");
  MAPPOINT_InitMapWarpPoint();
  print("完成\n");
  if(!MAPPOINT_loadMapWarpPoint()) {
    return;
  }

//#ifdef _ALLDOMAN
//	print("初始化英雄表列...");
//	InitHeroList();
//	print("完成\n");
//#endif

  int itime = 0;
  while(1) {
    if(getCpuUse() != -1) {
      itime++;
      if(itime > getCpuUse()) {
        itime = 0;
        usleep(1);
      }
    }

    setNewTime();
    memcpy(&tmNow, localtime(&NowTime.tv_sec), sizeof(tmNow));
    if(tmOld.tm_hour != getLogHour() && tmNow.tm_hour == getLogHour()) {
      backupAllLogFile(&tmOld);
    }

    setNewTime();
    netloop_faster();
    NPC_generateLoop(0);
    BATTLE_Loop();
    CHAR_Loop();
    PETMAIL_proc();
    family_proc();
    chardatasavecheck();
    if(SERVSTATE_getShutdown() > 0) {
      ShutdownProc();
    }
  }
}

static void sendmsg_toall(char *msg) {
  int i;
  int playernum = CHAR_getPlayerMaxNum();

  for(i = 0; i < playernum; i++) {
    if(CHAR_getCharUse(i) != FALSE) {
      CHAR_talkToCli(i, -1, msg, CHAR_COLORYELLOW);
    }
  }
}

static void ShutdownProc(void) {
#define    SYSINFO_SHUTDOWN_MSG    "%d 分钟后开始进行系统停机维护, 请大家提前下线以免数据丢失。"
#define    SYSINFO_SHUTDOWN_MSG_COMP  "服务器已关闭。"
  int diff, hun;

  diff = NowTime.tv_sec - SERVSTATE_getShutdown();
  hun = SERVSTATE_getLimittime() - (diff / 60);
  if(hun != SERVSTATE_getDsptime()) {
    char buff[256];
    if(hun != 0) {
      snprintf(buff, sizeof(buff), SYSINFO_SHUTDOWN_MSG, hun);
    }
    else {
      strcpy(buff, SYSINFO_SHUTDOWN_MSG_COMP);
    }
    sendmsg_toall(buff);
    SERVSTATE_setDsptime(hun);
    if(hun == 1) {
      SERVSTATE_SetAcceptMore(0);
    }
  }
  /* closesallsockets */
  if(hun == 0) {
    closeAllConnectionandSaveData();
    SERVSTATE_setShutdown(0);
    SERVSTATE_setDsptime(0);
    SERVSTATE_setLimittime(0);
  }

}

void family_proc() {
  static unsigned long gettime = 0;
  static unsigned long checktime = 0;
  static unsigned long proctime = 0;

  if(time(NULL) < proctime) return;
  proctime = time(NULL) + 5;

  if((unsigned long) NowTime.tv_sec > gettime) {
    getNewFMList();
    gettime = (unsigned long) NowTime.tv_sec + 60 * 10;
  }

  if((unsigned long) NowTime.tv_sec > checktime) {
    checkFamilyIndex();
    checktime = (unsigned long) NowTime.tv_sec + 60 * 30;
  }
}