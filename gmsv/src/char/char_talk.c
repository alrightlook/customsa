#include "version.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "chatmagic.h"
#include "battle.h"
#include "log.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "family.h"
#include "net.h"

extern int channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];

#ifdef _TALK_ACTION

void TalkAction(int charaindex, char *message);

#endif

#ifdef _GM_ITEM
static int player_useChatMagic( int charaindex, char* data, int isDebug);
#endif

#define DEBUGCDKEYNUM 100
struct tagDebugCDKey {
  int use;
  char cdkey[9];
};
static struct tagDebugCDKey DebugCDKey[DEBUGCDKEYNUM];

typedef void (*CHATMAGICFUNC)(int, char *);

typedef struct tagCHAR_ChatMagicTable {
  char *magicname;
  CHATMAGICFUNC func;
  int isdebug;
  int hash;
  int level;
  char *usestring;
} CHAR_ChatMagicTable;

static CHAR_ChatMagicTable CHAR_cmtbl[] = {
    //工程师专用
    {"programming_engineer", CHAR_CHAT_DEBUG_engineer, TRUE, 0, 3, ""},
    {"petlevelup", CHAR_CHAT_DEBUG_petlevelup, TRUE, 0, 2, "宠物栏号 等级 (账号)"},
    {"petexpup", CHAR_CHAT_DEBUG_petexpup, TRUE, 0, 2, "宠物栏号 经验 (账号)"},
    {"help", CHAR_CHAT_DEBUG_help, TRUE, 0, 1, "指令/all"},
#ifdef _EQUIT_NEGLECTGUARD
    {"setneguard", CHAR_CHAT_DEBUG_setneguard, TRUE, 0, 3, "waei"},
#endif
    //人物属性
    {"info", CHAR_CHAT_DEBUG_info, TRUE, 0, 1, "数值"},
    {"level", CHAR_CHAT_DEBUG_level, TRUE, 0, 1, "数值 (账号)"},
    {"settrans", CHAR_CHAT_DEBUG_setTrans, TRUE, 0, 2, "数值 (账号)"},
    {"exp", CHAR_CHAT_DEBUG_exp, TRUE, 0, 2, "数值 (账号)"},
    {"hp", CHAR_CHAT_DEBUG_hp, TRUE, 0, 2, "数值 (账号)"},
    {"mp", CHAR_CHAT_DEBUG_mp, TRUE, 0, 2, "数值 (账号)"},
    {"setmp", CHAR_CHAT_DEBUG_setmp, TRUE, 0, 2, "数值 (账号)"},
    {"str", CHAR_CHAT_DEBUG_str, TRUE, 0, 2, "数值*100 (账号)"},
    {"dex", CHAR_CHAT_DEBUG_dex, TRUE, 0, 2, "数值*100 (账号)"},
    {"tgh", CHAR_CHAT_DEBUG_tgh, TRUE, 0, 2, "数值*100 (账号)"},
    {"vital", CHAR_CHAT_DEBUG_vital, TRUE, 0, 2, "数值*100 (账号)"},
    {"luck", CHAR_CHAT_DEBUG_luck, TRUE, 0, 2, "数值 (账号)"},
    {"superman", CHAR_CHAT_DEBUG_superman, TRUE, 0, 2, "(账号)"},
    {"dp", CHAR_CHAT_DEBUG_dp, TRUE, 0, 2, "数值 (账号)"},
#ifdef _EQUIT_SEQUENCE
    {"sequence", CHAR_CHAT_DEBUG_sequence, TRUE, 0, 3, "数值"},
#endif

    //系统
    {"announce", CHAR_CHAT_DEBUG_announce, TRUE, 0, 1, "内容"},
    {"loginannounce", CHAR_CHAT_DEBUG_loginannounce, TRUE, 0, 1, "内容"},
    {"sysinfo", CHAR_CHAT_DEBUG_sysinfo, TRUE, 0, 1, ""},
    {"effect", CHAR_CHAT_DEBUG_effect, TRUE, 0, 1, "alloff/地图号 特效"},
    {"reset", CHAR_CHAT_DEBUG_reset, TRUE, 0, 2, "enemy/encount/magic/warppoint/petskill/pettalk/npc/all"},
    {"clean_floor", CHAR_CHAT_DEBUG_cleanfloor, TRUE, 0, 3, "地图号"},
    {"printcount", CHAR_CHAT_printcount, TRUE, 0, 1, ""},
    {"enemyrestart", CHAR_CHAT_DEBUG_enemyrestart, TRUE, 0, 3, "无"},
    {"cleanfreepet", CHAR_CHAT_DEBUG_cleanfreepet, TRUE, 0, 3, "无"},

#ifdef _GMRELOAD
    {"gmreload", CHAR_CHAT_DEBUG_gmreload, TRUE, 0, 3, "all/cdkey level"},
#endif

    {"waeikickall", CHAR_CHAT_DEBUG_waeikickall, TRUE, 0, 3, "无"},
    //工具	初级GM
    {"debug", CHAR_CHAT_DEBUG_debug, TRUE, 0, 1, "on/off"},
    {"metamo", CHAR_CHAT_DEBUG_metamo, TRUE, 0, 1, "变身图号 (账号)"},
    {"checklock", CHAR_CHAT_DEBUG_checklock, TRUE, 0, 1, "帐号"},
    {"shutup", CHAR_CHAT_DEBUG_shutup, TRUE, 0, 1, "帐号 ON/OFF"},
    {"gmkick", CHAR_CHAT_DEBUG_gmkick, TRUE, 0, 1, "帐号 LSLOCK/KICK/DEUNLOCK/UNLOCKALL/LOCK/TYPE/UNLOCK"},
    {"battlein", CHAR_CHAT_DEBUG_battlein, TRUE, 0, 1, "无"},
    {"battleout", CHAR_CHAT_DEBUG_battleout, TRUE, 0, 1, "无"},
    {"battlewatch", CHAR_CHAT_DEBUG_battlewatch, TRUE, 0, 1, "无"},
    {"getuser", CHAR_CHAT_DEBUG_getuser, TRUE, 0, 1, "人物名 地图号 (npc)"},
    {"warp", CHAR_CHAT_DEBUG_warp, TRUE, 0, 1, "地图号 x y"},
    {"waeikick", CHAR_CHAT_DEBUG_waeikick, TRUE, 0, 1, "帐号"},
    {"jail", CHAR_CHAT_DEBUG_jail, TRUE, 0, 1, "帐号"},
    {"send", CHAR_CHAT_DEBUG_send, TRUE, 0, 1, "floor x y 帐号"},
#ifdef _SendTo
    { "sendto",			CHAR_CHAT_DEBUG_Sendto,			TRUE,	0,	1, "帐号"},
#endif
    {"noenemy", CHAR_CHAT_DEBUG_noenemy, TRUE, 0, 1, "on/off"},
    {"watchevent", CHAR_CHAT_DEBUG_watchevent, TRUE, 0, 1, "无"},
    {"silent", CHAR_CHAT_DEBUG_silent, TRUE, 0, 2, "帐号 分钟"},//禁言
    {"unlock", CHAR_CHAT_DEBUG_unlock, TRUE, 0, 2, "帐号"},
    {"eventclean", CHAR_CHAT_DEBUG_eventclean, TRUE, 0, 2, "all/旗标 帐号 人物名"},
    {"eventsetend", CHAR_CHAT_DEBUG_eventsetend, TRUE, 0, 2, "all/旗标 帐号 人物名"},
    {"eventsetnow", CHAR_CHAT_DEBUG_eventsetnow, TRUE, 0, 2, "all/旗标 帐号 人物名"},

    {"playerspread", CHAR_CHAT_DEBUG_playerspread, TRUE, 0, 3, "waei"},
    {"shutupall", CHAR_CHAT_DEBUG_shutupall, TRUE, 0, 3, "无"},
    {"unlockserver", CHAR_CHAT_DEBUG_unlockserver, TRUE, 0, 3, "无"},
    {"shutdown", CHAR_CHAT_DEBUG_shutdown, TRUE, 0, 3, "(分钟)"},

    //制成
    {"delitem", CHAR_CHAT_DEBUG_delitem, TRUE, 0, 1, "all/位置"},
    {"delpet", CHAR_CHAT_DEBUG_deletepet, TRUE, 0, 1, "all/位置"},
    {"additem", CHAR_CHAT_DEBUG_additem, TRUE, 0, 2, "道具ID ((数量) (账号))"},
    {"petmake", CHAR_CHAT_DEBUG_petmake, TRUE, 0, 2, "宠物ID ((等级) (账号))"},
    {"gold", CHAR_CHAT_DEBUG_gold, TRUE, 0, 2, "数量 (账号)"},

    //家族工具
    {"manorpk", CHAR_CHAT_DEBUG_manorpk, TRUE, 0, 2, "allpeace/peace 庄园编号"},
    {"fixfmleader", CHAR_CHAT_DEBUG_fixfmleader, TRUE, 0, 2, "帐号 1"},
    {"fixfmpk", CHAR_CHAT_DEBUG_fixfmpk, TRUE, 0, 3, ""},
    {"fixfmdata", CHAR_CHAT_DEBUG_fixfmdata, TRUE, 0, 2, ""},

#ifdef _TEST_DROPITEMS
    {"dropmypet", CHAR_CHAT_DEBUG_dropmypet, TRUE, 0, 3, "宠物编号"},
    {"dropmyitem", CHAR_CHAT_DEBUG_dropmyitem, TRUE, 0, 2, "道具编号/(0/1)"},
#endif
    {"checktime", CHAR_CHAT_DEBUG_checktime, TRUE, 0, 3, ""},

#ifdef _GAMBLE_BANK
    {"setgamblenum", CHAR_CHAT_DEBUG_setgamblenum, TRUE, 0, 3, "数值"},
#endif
    // WON ADD 当机指令
    {"crash", CHAR_CHAT_DEBUG_crash, TRUE, 0, 3, ""},
#ifdef _PETSKILL_SETDUCK
    {"setDuck", CHAR_CHAT_DEBUG_SetDuck, TRUE, 0, 3, ""},
#endif

#ifdef _TYPE_TOXICATION
    {"setTox", CHAR_CHAT_DEBUG_Toxication, TRUE, 0, 3, ""},
#endif
    {"showMem", CHAR_CHAT_DEBUG_showMem, TRUE, 0, 2, "无"},
    {"samecode", CHAR_CHAT_DEBUG_samecode, TRUE, 0, 3, "pet/item/set 编码"},
    {"itemreload", CHAR_CHAT_DEBUG_itemreload, TRUE, 0, 2, ""},

    {"skywalker", CHAR_CHAT_DEBUG_skywalker, TRUE, 0, 1, ""},
#ifdef _ITEM_ADDEXP
    {"itemaddexp", CHAR_CHAT_DEBUG_itemaddexp, TRUE, 0, 1, ""},
#endif

#ifdef _DEF_GETYOU
    {"getyou", CHAR_CHAT_DEBUG_getyou, TRUE, 0, 1, "格数 1~3"},
#endif
#ifdef _DEF_NEWSEND
    {"newsend", CHAR_CHAT_DEBUG_newsend, TRUE, 0, 1, "floor x y 帐号 无/要说的话"},
#endif
#ifdef _DEF_SUPERSEND
    {"supersend", CHAR_CHAT_DEBUG_supersend, TRUE, 0, 3, "floor x y 格数 无/要说的话"},
#endif

#ifdef _CREATE_MM_1_2
    { "MM",		CHAR_CHAT_DEBUG_MM,      TRUE,   0,  3, "MM 1/2 (账号)"},
#endif
#ifdef _GM_ITEM
    { "gmfunction",		CHAR_CHAT_DEBUG_GMFUNCTION,      TRUE,   0,  3, "命令 次数 (账号)"},
#endif

#ifdef _GM_RIDE
{ "setride",		CHAR_CHAT_DEBUG_SETRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
{ "mvride",		CHAR_CHAT_DEBUG_MVRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
#endif

#ifdef _LOCK_IP
    {"addlock", CHAR_CHAT_DEBUG_ADD_LOCK, TRUE, 0, 3, "flag 账号/IP IP"},
    {"dellock", CHAR_CHAT_DEBUG_DEL_LOCK, TRUE, 0, 3, "flag 账号/IP IP"},
    {"showip", CHAR_CHAT_DEBUG_SHOWIP, TRUE, 0, 3, "账号"},
#endif
    {"setfame", CHAR_CHAT_DEBUG_SET_FAME, TRUE, 0, 3, "账号 声望值"},

#ifdef _AUTO_PK
    {"pktime", CHAR_CHAT_DEBUG_SET_AUTOPK, TRUE, 0, 3, "分钟"},
#endif

#ifdef _PLAYER_NUM
    {"playernum", CHAR_CHAT_DEBUG_SET_PLAYERNUM, TRUE, 0, 3, "人数"},
#endif

#ifdef _RELOAD_CF
    {"reloadcf", CHAR_CHAT_DEBUG_SET_RELOADCF, TRUE, 0, 3, ""},
#endif

#ifdef _TRANS
    { "trans",		CHAR_CHAT_DEBUG_Trans,      TRUE,   0,  3, "(账号)"},
#endif
#ifdef _MAKE_PET_CF
    {"petmakecf", CHAR_CHAT_DEBUG_petmakecf, TRUE, 0, 3, "宠物编号 转物/等级/成长"},
#endif
#ifdef _MAKE_PET_ABILITY
    {"petmakeabi", CHAR_CHAT_DEBUG_petmakeabi, TRUE, 0, 3, "宠物编号 转物/等级/成长"},
#endif
#ifdef _MAKE_MAP
    {"delmap", CHAR_CHAT_DelMap, TRUE, 0, 3, ""},
    {"getmap", CHAR_CHAT_GetMap, TRUE, 0, 3, ""},
    {"map", CHAR_CHAT_Map, TRUE, 0, 3, ""},
    {"tile", CHAR_CHAT_Fixtile, TRUE, 0, 3, ""},
    {"obj", CHAR_CHAT_Fixobj, TRUE, 0, 3, ""},
    {"删除地图", CHAR_CHAT_GetMap, TRUE, 0, 3, ""},
    {"获取地图", CHAR_CHAT_GetMap, TRUE, 0, 3, ""},
    {"图片对像", CHAR_CHAT_Map, TRUE, 0, 3, ""},
    {"图片", CHAR_CHAT_Fixtile, TRUE, 0, 3, ""},
    {"对象", CHAR_CHAT_Fixobj, TRUE, 0, 3, ""},
//	{ "fukuwa",		CHAR_CHAT_Fukuwa,      TRUE,   0,  3, ""},
#endif
/////////////////////////////////////////////////////////////////////////
    //工程师专用
//	{ "programming_engineer",	CHAR_CHAT_DEBUG_engineer,	TRUE,	0,	3, ""},
    {"宠物等级", CHAR_CHAT_DEBUG_petlevelup, TRUE, 0, 2, "宠物栏号 等级 (账号)"},
    {"宠物经验", CHAR_CHAT_DEBUG_petexpup, TRUE, 0, 2, "宠物栏号 经验 (账号)"},
    {"帮助", CHAR_CHAT_DEBUG_help, TRUE, 0, 1, "指令/all"},
#ifdef _EQUIT_NEGLECTGUARD
//	{ "setneguard",		CHAR_CHAT_DEBUG_setneguard,			TRUE,	0,	3, "waei"},
#endif
    //人物属性
    {"人物信息", CHAR_CHAT_DEBUG_info, TRUE, 0, 1, "数值"},
    {"等级", CHAR_CHAT_DEBUG_level, TRUE, 0, 1, "数值 (账号)"},
    {"转生", CHAR_CHAT_DEBUG_setTrans, TRUE, 0, 2, "数值 (账号)"},
    {"经验", CHAR_CHAT_DEBUG_exp, TRUE, 0, 2, "数值 (账号)"},
    {"血", CHAR_CHAT_DEBUG_hp, TRUE, 0, 2, "数值 (账号)"},
    {"魔法", CHAR_CHAT_DEBUG_mp, TRUE, 0, 2, "数值 (账号)"},
    {"设置魔法", CHAR_CHAT_DEBUG_setmp, TRUE, 0, 2, "数值 (账号)"},
    {"腕力", CHAR_CHAT_DEBUG_str, TRUE, 0, 2, "数值*100 (账号)"},
    {"敏捷", CHAR_CHAT_DEBUG_dex, TRUE, 0, 2, "数值*100 (账号)"},
    {"耐力", CHAR_CHAT_DEBUG_tgh, TRUE, 0, 2, "数值*100 (账号)"},
    {"体力", CHAR_CHAT_DEBUG_vital, TRUE, 0, 2, "数值*100 (账号)"},
    {"运气", CHAR_CHAT_DEBUG_luck, TRUE, 0, 2, "数值 (账号)"},
    {"超人", CHAR_CHAT_DEBUG_superman, TRUE, 0, 2, "(账号)"},
    {"dp", CHAR_CHAT_DEBUG_dp, TRUE, 0, 2, "数值 (账号)"},

#ifdef _EQUIT_SEQUENCE
    {"顺序", CHAR_CHAT_DEBUG_sequence, TRUE, 0, 3, "数值"},
#endif

    //系统
    {"公告", CHAR_CHAT_DEBUG_announce, TRUE, 0, 1, "内容"},
    {"登陆公告", CHAR_CHAT_DEBUG_loginannounce, TRUE, 0, 1, "内容"},
    {"系统信息", CHAR_CHAT_DEBUG_sysinfo, TRUE, 0, 1, ""},
    {"地图特效", CHAR_CHAT_DEBUG_effect, TRUE, 0, 1, "alloff/地图号 特效"},
    {"重读", CHAR_CHAT_DEBUG_reset, TRUE, 0, 2, "enemy/encount/magic/warppoint/petskill/pettalk/npc/all"},
    {"清除地图物品", CHAR_CHAT_DEBUG_cleanfloor, TRUE, 0, 3, "地图号"},
    {"统计", CHAR_CHAT_printcount, TRUE, 0, 1, ""},
    {"重读宠物资料", CHAR_CHAT_DEBUG_enemyrestart, TRUE, 0, 3, "无"},
    {"清除地上宠物", CHAR_CHAT_DEBUG_cleanfreepet, TRUE, 0, 3, "无"},

#ifdef _GMRELOAD
    {"读取GM设置", CHAR_CHAT_DEBUG_gmreload, TRUE, 0, 3, "all/cdkey level"},
#endif

    {"踢除所有玩家", CHAR_CHAT_DEBUG_waeikickall, TRUE, 0, 3, "无"},
//	{ "checktrade",		CHAR_CHAT_DEBUG_checktrade,		TRUE,	0,	3, "waei"},
    //工具	初级GM
    {"调试", CHAR_CHAT_DEBUG_debug, TRUE, 0, 1, "密码 调试 on/off"},
    {"造型", CHAR_CHAT_DEBUG_metamo, TRUE, 0, 1, "变身图号 (账号)"},
    {"锁定帐号", CHAR_CHAT_DEBUG_checklock, TRUE, 0, 1, "帐号"},
    {"封嘴", CHAR_CHAT_DEBUG_shutup, TRUE, 0, 1, "帐号 ON/OFF"},
    {"gm踢人", CHAR_CHAT_DEBUG_gmkick, TRUE, 0, 1, "帐号 LSLOCK/KICK/DEUNLOCK/UNLOCKALL/LOCK/TYPE/UNLOCK"},
    {"加入战斗", CHAR_CHAT_DEBUG_battlein, TRUE, 0, 1, "无"},
    {"结束战斗", CHAR_CHAT_DEBUG_battleout, TRUE, 0, 1, "无"},
    {"观战", CHAR_CHAT_DEBUG_battlewatch, TRUE, 0, 1, "无"},
    {"显示信息", CHAR_CHAT_DEBUG_getuser, TRUE, 0, 1, "人物名 地图号 (npc)"},
    {"传送自己", CHAR_CHAT_DEBUG_warp, TRUE, 0, 1, "地图号 x y"},
    {"踢除玩家", CHAR_CHAT_DEBUG_waeikick, TRUE, 0, 1, "帐号"},
    {"关入监狱", CHAR_CHAT_DEBUG_jail, TRUE, 0, 1, "帐号"},
    {"传送", CHAR_CHAT_DEBUG_send, TRUE, 0, 1, "地图编号 x y 帐号"},
#ifdef _SendTo
    { "传送到玩家",			CHAR_CHAT_DEBUG_Sendto,			TRUE,	0,	1, "帐号"},
#endif
    {"不遇敌", CHAR_CHAT_DEBUG_noenemy, TRUE, 0, 1, "on/off"},
    {"显示完成任务", CHAR_CHAT_DEBUG_watchevent, TRUE, 0, 1, "无"},
    {"禁言", CHAR_CHAT_DEBUG_silent, TRUE, 0, 2, "帐号 分钟"},//禁言
    {"解锁", CHAR_CHAT_DEBUG_unlock, TRUE, 0, 2, "帐号"},
    {"清除旗标", CHAR_CHAT_DEBUG_eventclean, TRUE, 0, 2, "all/旗标 帐号 人物名"},
    {"增加旗标", CHAR_CHAT_DEBUG_eventsetend, TRUE, 0, 2, "all/旗标 帐号 人物名"},
    {"重新任务", CHAR_CHAT_DEBUG_eventsetnow, TRUE, 0, 2, "all/旗标 帐号 人物名"},

    {"人物地区显示", CHAR_CHAT_DEBUG_playerspread, TRUE, 0, 3, "waei"},
//	{ "shutupall",		CHAR_CHAT_DEBUG_shutupall,		TRUE,	0,	3, "无"},
    {"解除服务器", CHAR_CHAT_DEBUG_unlockserver, TRUE, 0, 3, "无"},
    {"关服", CHAR_CHAT_DEBUG_shutdown, TRUE, 0, 3, "分钟"},

    //制成
    {"删除物品", CHAR_CHAT_DEBUG_delitem, TRUE, 0, 1, "all/位置"},
    {"删除宠物", CHAR_CHAT_DEBUG_deletepet, TRUE, 0, 1, "all/位置"},
    {"制作物品", CHAR_CHAT_DEBUG_additem, TRUE, 0, 2, "道具ID ((数量) (账号))"},
    {"制作宠物", CHAR_CHAT_DEBUG_petmake, TRUE, 0, 2, "宠物ID ((等级) (账号))"},
    {"金钱", CHAR_CHAT_DEBUG_gold, TRUE, 0, 2, "数量 (账号)"},

    //家族工具
//	{ "manorpk",		CHAR_CHAT_DEBUG_manorpk,		TRUE,	0,	2, "allpeace/peace 庄园编号"},
    {"修改族长", CHAR_CHAT_DEBUG_fixfmleader, TRUE, 0, 2, "帐号 1"},
    {"修复家族PK", CHAR_CHAT_DEBUG_fixfmpk, TRUE, 0, 3, ""},
    {"修复家族数据", CHAR_CHAT_DEBUG_fixfmdata, TRUE, 0, 2, ""},

#ifdef _TEST_DROPITEMS
    {"满地宠物", CHAR_CHAT_DEBUG_dropmypet, TRUE, 0, 3, "宠物编号"},
    {"满地物品", CHAR_CHAT_DEBUG_dropmyitem, TRUE, 0, 3, "道具编号/(0/1)"},
#endif
    {"显示时间", CHAR_CHAT_DEBUG_checktime, TRUE, 0, 3, ""},

#ifdef _GAMBLE_BANK
    {"积分", CHAR_CHAT_DEBUG_setgamblenum, TRUE, 0, 3, "数值"},
#endif
    // WON ADD 当机指令
    {"当机", CHAR_CHAT_DEBUG_crash, TRUE, 0, 3, ""},
#ifdef _PETSKILL_SETDUCK
//	{ "setDuck",		CHAR_CHAT_DEBUG_SetDuck,		TRUE,	0,	3, ""},
#endif

#ifdef _TYPE_TOXICATION
    {"中毒", CHAR_CHAT_DEBUG_Toxication, TRUE, 0, 3, ""},
#endif
    {"显示内存", CHAR_CHAT_DEBUG_showMem, TRUE, 0, 2, "无"},
    {"编码", CHAR_CHAT_DEBUG_samecode, TRUE, 0, 3, "pet/item/set 编码"},

    {"读取物品", CHAR_CHAT_DEBUG_itemreload, TRUE, 0, 2, ""},

    {"天行者", CHAR_CHAT_DEBUG_skywalker, TRUE, 0, 1, ""},
#ifdef _ITEM_ADDEXP
    {"读取经验物品", CHAR_CHAT_DEBUG_itemaddexp, TRUE, 0, 1, ""},
#endif
#ifdef _DEF_GETYOU
    {"获取账号", CHAR_CHAT_DEBUG_getyou, TRUE, 0, 1, "格数 1~3"},
#endif
#ifdef _DEF_NEWSEND
    {"传送玩家", CHAR_CHAT_DEBUG_newsend, TRUE, 0, 1, "地图编号 x y 帐号 无/要说的话"},
#endif
#ifdef _DEF_SUPERSEND
    {"群体传送", CHAR_CHAT_DEBUG_supersend, TRUE, 0, 3, "地图编号 x y 格数 无/要说的话"},
#endif

#ifdef _GM_ITEM
    { "权限",		CHAR_CHAT_DEBUG_GMFUNCTION,      TRUE,   0,  3, "命令 次数 (账号)"},
#endif

#ifdef _GM_RIDE
{ "设置骑乘",		CHAR_CHAT_DEBUG_SETRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
{ "移除骑乘",		CHAR_CHAT_DEBUG_MVRIDE,      TRUE,   0,  3, "骑宠号 (账号)"},
#endif

#ifdef _LOCK_IP
    {"锁定", CHAR_CHAT_DEBUG_ADD_LOCK, TRUE, 0, 3, "flag 账号/IP IP"},
    {"解除锁定", CHAR_CHAT_DEBUG_DEL_LOCK, TRUE, 0, 3, "flag 账号/IP IP"},
    {"显示玩家IP", CHAR_CHAT_DEBUG_SHOWIP, TRUE, 0, 3, "账号"},
#endif
    {"声望", CHAR_CHAT_DEBUG_SET_FAME, TRUE, 0, 3, "账号 声望值"},
#ifdef _AUTO_PK
    {"pk时间", CHAR_CHAT_DEBUG_SET_AUTOPK, TRUE, 0, 3, "分钟"},
#endif

#ifdef _PLAYER_NUM
    {"玩家数", CHAR_CHAT_DEBUG_SET_PLAYERNUM, TRUE, 0, 3, "人数"},
#endif
#ifdef _RELOAD_CF
    {"读取设置", CHAR_CHAT_DEBUG_SET_RELOADCF, TRUE, 0, 3, ""},
#endif
#ifdef _TRANS
    { "高级转生",		CHAR_CHAT_DEBUG_Trans,      TRUE,   0,  3, "(账号)"},
#endif
#ifdef _MAKE_PET_CF
    {"宠物成长", CHAR_CHAT_DEBUG_petmakecf, TRUE, 0, 3, "宠物编号 转物/等级/成长"},
#endif
#ifdef _MAKE_PET_ABILITY
    {"宠物四围", CHAR_CHAT_DEBUG_petmakeabi, TRUE, 0, 3, "宠物编号 血 攻 防 敏"},
#endif
};

void CHAR_initChatMagic(void) {
  int i;
  for(i = 0; i < arraysizeof(CHAR_cmtbl); i++)
    CHAR_cmtbl[i].hash = hashpjw(CHAR_cmtbl[i].magicname);
  for(i = 0; i < DEBUGCDKEYNUM; i++) {
    DebugCDKey[i].use = FALSE;
    DebugCDKey[i].cdkey[0] = '\0';
  }
}

static CHATMAGICFUNC CHAR_getChatMagicFuncPointer(char *name, int isDebug) {
  int i;
  int hash = hashpjw(name);
  for(i = 0; i < arraysizeof(CHAR_cmtbl); i++)
    if(CHAR_cmtbl[i].hash == hash
       && CHAR_cmtbl[i].isdebug == isDebug
       && strcmp(CHAR_cmtbl[i].magicname, name) == 0)
      return CHAR_cmtbl[i].func;
  return NULL;
}

int CHAR_getChatMagicFuncLevel(char *name, int isDebug) {
  int i;
  int hash = hashpjw(name);
  for(i = 0; i < arraysizeof(CHAR_cmtbl); i++)
    if(CHAR_cmtbl[i].hash == hash
       && CHAR_cmtbl[i].isdebug == isDebug
       && strcmp(CHAR_cmtbl[i].magicname, name) == 0)
      return CHAR_cmtbl[i].level;
  return -1;
}

int CHAR_getChatMagicFuncNameAndString(int ti, char *name, char *usestring, int level, int isDebug) {
  if(name == NULL || usestring == NULL) return -1;
  if(ti < 0 || ti >= arraysizeof(CHAR_cmtbl)) return -1;
  if(CHAR_cmtbl[ti].isdebug == isDebug &&
     CHAR_cmtbl[ti].level <= level) {
    sprintf(name, "%s", CHAR_cmtbl[ti].magicname);
    sprintf(usestring, "%s", CHAR_cmtbl[ti].usestring);
    return 1;
  }
  return 0;
}

int CHAR_getChatMagicFuncMaxNum() {
  return arraysizeof(CHAR_cmtbl);
}

static int CHAR_useChatMagic(int charaindex, char *data, int isDebug) {
  char magicname[256];
  int ret;
  int i;
  int gmLevel = 0, magicLevel;

  CHATMAGICFUNC func;

#ifdef _GMRELOAD
  extern struct GMINFO gminfo[GMMAXNUM];
#else
#endif
  char *p = CHAR_getChar(charaindex, CHAR_CDKEY);
  if(!p) {
    fprint("err nothing cdkey\n");
    return FALSE;
  }

  if(getChatMagicCDKeyCheck() == 1) { //第一次确认GM帐号
    if(CHAR_getWorkInt(charaindex, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
      gmLevel = CHAR_getWorkInt(charaindex, CHAR_WORKGMLEVEL);
    } else {

#ifdef _GMRELOAD
      for(i = 0; i < GMMAXNUM; i++) {
        if(strcmp(p, gminfo[i].cdkey) == 0) {
          gmLevel = gminfo[i].level;
          CHAR_setWorkInt(charaindex, CHAR_WORKGMLEVEL, gmLevel);
          break;
        }
      }
#else
#endif

      if(i >= GMMAXNUM) {
        for(i = 0; i < DEBUGCDKEYNUM; i++) {
          if(DebugCDKey[i].use && strcmp(p, DebugCDKey[i].cdkey) == 0) {
            break;
          }
        }
        if(i >= DEBUGCDKEYNUM) return FALSE;
      }
    }

  } else {
    gmLevel = 3;
    CHAR_setWorkInt(charaindex, CHAR_WORKGMLEVEL, gmLevel);
  }

  ret = getStringFromIndexWithDelim(data, " ", 1, magicname, sizeof(magicname));
  if(ret == FALSE)return FALSE;

  // Robin 0618  chaeck GM Level
  magicLevel = CHAR_getChatMagicFuncLevel(magicname, isDebug);
  if(gmLevel < magicLevel)
    return FALSE;

  func = CHAR_getChatMagicFuncPointer(magicname, isDebug);
  if(func) {
    LogGM(CHAR_getUseName(charaindex), CHAR_getChar(charaindex, CHAR_CDKEY), data,
          CHAR_getInt(charaindex, CHAR_FLOOR), CHAR_getInt(charaindex, CHAR_X),
          CHAR_getInt(charaindex, CHAR_Y));
    func(charaindex, data + strlen(magicname) + 1);
    return TRUE;
  } else {
    return FALSE;
  }
}

/*------------------------------------------------------------
 * 祥汹井日汔毛菲户月
 * 娄醒
 *  volume  int     祥汹
 * 忒曰袄
 *  穴永皿匹及穴旦
 ------------------------------------------------------------*/
static int CHAR_getRangeFromVolume(int volume) {
  static int chatvol[] = {
      3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31
  };

  if(volume < 0)return 0;
  else if(arraysizeof(chatvol) <= volume) {
    return chatvol[arraysizeof(chatvol) - 1];
  }
  return chatvol[volume];
}

/*------------------------------------------------------------
 * 擘及示伉亘□丞毛茧月［条卅中支曰井凶［
 * 娄醒
 *  mesg        char*       仄扎屯月蜕邯
 * 忒曰袄
 *  -1 及午五反仇公仇公
 *  0  及午五反孔勾丹
 *  1 方曰云云五中午｝仃勾卞勾中化中月 ! 及醒
 ------------------------------------------------------------*/
static int CHAR_getVolume(char *mesg) {
  int stringlen = strlen(mesg);
  if(stringlen == 0)
    return 0;
  else if(stringlen == 1) {
    if(mesg[0] == '!')
      return 1;
    else
      return 0;
  }
  else if(stringlen == 2) {
    if(mesg[1] == '!') {
      if(mesg[0] == '!')
        return 2;
      else
        return 1;
    }
    else
      return 0;
  } else {
    /*  3动晓   */
    if(mesg[stringlen - 1] == '.') {
      /*  仇公仇公及第  岭丐曰    */
      if(mesg[stringlen - 2] == '.' && mesg[stringlen - 3] == '.') {
        /*  仇公仇公    */
        return -1;
      }
      return 0;
    } else {
      /*  仇仇井日 ! 毛醒尹月*/
      int exnum = 0;
      int i;
      for(i = stringlen - 1; i >= 0; i--) {
        if(mesg[i] != '!')
          break;
        else
          exnum++;
      }
      return exnum;
    }
  }
}

/*------------------------------------------------------------
 * 丢永本□斥及    井日丢永本□斥及      坌毛潸曰请允
 * 娄醒
 *  message     char*           丢永本□斥    
 *  kind        char*           p or s or i卅及丢永本□斥
 *  kindlen     int             kind 及赢今
 *  body        char**          丢永本□斥    及禾奶件正□及伞  桦赭
 * 忒曰袄
 *  卅仄
 ------------------------------------------------------------*/
void CHAR_getMessageBody(char *message, char *kind, int kindlen, char **body) {
  int firstchar;

  /* 1  侬  毛切之匀仁［1  侬匹丐月仇午反lssproto.html互忡据 */
  // Nuke +1: For invalid message attack
  *body = 0;
  firstchar = message[0];
  if(firstchar == 'P' ||
     firstchar == 'S' ||
     firstchar == 'D'

     // CoolFish: Trade 2001/4/18
     || firstchar == 'C'
     || firstchar == 'T'
     || firstchar == 'W'

     // CoolFish: Family 2001/5/28
     || firstchar == 'A'
     || firstchar == 'J'
     || firstchar == 'E'
     || firstchar == 'M'

     || firstchar == 'B'
     || firstchar == 'X'
     || firstchar == 'R'
     || firstchar == 'L'

      ) {
    if(kindlen >= 2) {
      kind[0] = firstchar;
      kind[1] = '\0';
    } else {
      return;
    }
  } else {
    return;
  }

  *body = message + 2;
}


static int CHAR_Talk_check(int talkerindex, int talkedindex, int micflg) {
#if 1
  if(!CHAR_getFlg(talkerindex, CHAR_ISPARTYCHAT)) {
    int talker_b_mode = CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE);
    int talked_b_mode = CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEMODE);
    if(talker_b_mode != BATTLE_CHARMODE_NONE
       && CHAR_getInt(talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
      return FALSE;
    }
    if(micflg != 0)return TRUE;
    if(talker_b_mode != BATTLE_CHARMODE_NONE && talked_b_mode != BATTLE_CHARMODE_NONE) {
      if(CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEINDEX)
         != CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEINDEX)) {
        return FALSE;
      }
    } else if(talker_b_mode != BATTLE_CHARMODE_NONE || talked_b_mode != BATTLE_CHARMODE_NONE) {
      return FALSE;
    }
    return TRUE;
  }
  if(CHAR_getInt(talkerindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    int talker_b_mode = CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEMODE);
    int talked_b_mode = CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEMODE);

    if(talker_b_mode != BATTLE_CHARMODE_NONE && CHAR_getInt(talkedindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
      return FALSE;
    }
    if(micflg != 0)return TRUE;
    if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      if(CHAR_getWorkInt(talkedindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
        if(CHAR_getWorkInt(talkerindex, CHAR_WORKPARTYINDEX1)
           == CHAR_getWorkInt(talkedindex, CHAR_WORKPARTYINDEX1)) {
          return TRUE;
        }
      }
    } else {
      if(talker_b_mode != BATTLE_CHARMODE_NONE && talked_b_mode != BATTLE_CHARMODE_NONE) {
        if(CHAR_getWorkInt(talkerindex, CHAR_WORKBATTLEINDEX)
           != CHAR_getWorkInt(talkedindex, CHAR_WORKBATTLEINDEX)) {
          return FALSE;
        }
      } else if(talker_b_mode != BATTLE_CHARMODE_NONE ||
                talked_b_mode != BATTLE_CHARMODE_NONE) {
        return FALSE;
      }
      return TRUE;
    }
  }
  return FALSE;

#else
  int		MyBattleMode;
  int		ToBattleMode;

  MyBattleMode = CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEMODE);
  ToBattleMode = CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEMODE);

  /*   爵    及凛 */
  if( MyBattleMode == BATTLE_CHARMODE_NONE ) {
    if( ToBattleMode == BATTLE_CHARMODE_NONE ) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  /* 爵    及凛 */
  else {
    /* 爵  仄化中卅中谛卞反霜日卅中［ */
    if( ToBattleMode == BATTLE_CHARMODE_NONE) {
      return FALSE;
    }
    /*   元爵  匹  元扔奶玉及谛卞仄井霜耨请  卅中 */
    if( CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLEINDEX)
      == CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLEINDEX) &&
      CHAR_getWorkInt( talkerindex, CHAR_WORKBATTLESIDE)
      == CHAR_getWorkInt( talkedindex, CHAR_WORKBATTLESIDE ) )
    {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  return FALSE;
#endif
}

void CHAR_Talk(int fd, int index, char *message, int color, int area) {
  char firstToken[64];
  char messageeraseescape[512];
  char *messagebody;
  int mesgvolume = 0;
  int lastvolume = 0;
  int fl, x, y;
  int stringlen;
  int micflg = 0;

  int fmindexi = CHAR_getWorkInt(index, CHAR_WORKFMINDEXI);
  int channel = CHAR_getWorkInt(index, CHAR_WORKFMCHANNEL);
  int quickchannel = CHAR_getWorkInt(index, CHAR_WORKFMCHANNELQUICK);
  {
    char *cdkey = CHAR_getChar(index, CHAR_CDKEY);
    char *charname = CHAR_getChar(index, CHAR_NAME);
    if(strlen(message) > 3) {
      if(CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
        // shan begin
        if(strstr(message, "[") && strstr(message, "]")) {
        } else {
          // original
          LogTalk(charname, cdkey, CHAR_getInt(index, CHAR_FLOOR),
                  CHAR_getInt(index, CHAR_X), CHAR_getInt(index, CHAR_Y),
                  message);
        }
        // end
      }
    }
  }
  //bg|0|r0|fc|d4B8|p0|bn|10|bt|10|
  // Nuke +1 08/27: For invalid message attack
  if(*message == 0)
    return;
  CHAR_getMessageBody(message, firstToken, sizeof(firstToken),
                      &messagebody);
  // Nuke +1: For invalid message attack
  if(!messagebody)
    return;

  strcpysafe(messageeraseescape, sizeof(messageeraseescape),
             messagebody);
  makeStringFromEscaped(messageeraseescape);
  stringlen = strlen(messageeraseescape);
  if(stringlen <= 0)
    return;

  if(messageeraseescape[0] == '['
     && messageeraseescape[stringlen - 1] == ']') {
    char gm[128];
    char *pass = strstr(messageeraseescape + 1, getChatMagicPasswd());
    // Arminius: limit ip +2
    char* ip = CONNECT_get_userip(fd);
    messageeraseescape[stringlen - 1] = '\0';
    // Arminius: limit ip
    if(pass == messageeraseescape + 1) {
      if(CHAR_useChatMagic(index, messageeraseescape + 1 + strlen(getChatMagicPasswd()) + 1, TRUE)) {
        print("\n账号:%s 名字:%s\n来自ip:%s\n使用GM命令%s]\n", CHAR_getChar(index, CHAR_CDKEY), CHAR_getChar(index, CHAR_NAME), ip, messageeraseescape);
        sprintf(gm, "您使用GM命令%s]已成功", messageeraseescape);
        CHAR_talkToCli(index, -1, gm, CHAR_COLORYELLOW);
      }
    } else {
      if(CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
        if(CHAR_useChatMagic(index, messageeraseescape + 1, TRUE)) {
          print("\n账号:%s 名字:%s\n来自ip:%s\n使用GM命令%s]\n", CHAR_getChar(index, CHAR_CDKEY), CHAR_getChar(index, CHAR_NAME), ip, messageeraseescape);
          sprintf(gm, "您使用GM命令%s]已成功", messageeraseescape);
          CHAR_talkToCli(index, -1, gm, CHAR_COLORYELLOW);
        }
      } else {
#ifdef _GM_ITEM
        if(CHAR_getInt( index, CHAR_GMTIME)>0){
          char magicname[32];
          char token[64];
          getStringFromIndexWithDelim( messageeraseescape + 1, " ", 1, magicname,  sizeof( magicname));
          if(!strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), magicname)){
            player_useChatMagic( index,messageeraseescape + 1,TRUE);
            print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用物品命令%s]次数剩下%d\n", CHAR_getChar( index, CHAR_CDKEY),
                              CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape, CHAR_getInt( index, CHAR_GMTIME) - 1);
            CHAR_setInt( index , CHAR_GMTIME, CHAR_getInt( index, CHAR_GMTIME) - 1 );
            if(CHAR_getInt( index, CHAR_GMTIME) > 0){
              sprintf( token, "你还能使用%s权限%d次!", CHAR_getChar( index, CHAR_GMFUNCTION),CHAR_getInt( index, CHAR_GMTIME));
              CHAR_talkToCli( index, -1,token, CHAR_COLORRED );
            }else{
              sprintf( token, "你已经没有使用%s权限了!", CHAR_getChar( index, CHAR_GMFUNCTION));
              CHAR_talkToCli( index, -1,token, CHAR_COLORRED );
            }
          }else if(!strcmp( "help", magicname) || !strcmp( "帮助", magicname)){
            getStringFromIndexWithDelim( messageeraseescape + 1, " ", 2, magicname,  sizeof( magicname));
            if(!strcmp( CHAR_getChar( index, CHAR_GMFUNCTION), magicname)){
              player_useChatMagic( index,messageeraseescape + 1,TRUE);
              print( "\n账号:%s 名字:%s\n来自ip:%d.%d.%d.%d\n使用帮助命令%s]\n", CHAR_getChar( index, CHAR_CDKEY),
                              CHAR_getChar( index, CHAR_NAME), a, b, c, d,messageeraseescape);
            }
          }
        }else
#endif
        CHAR_useChatMagic(index, messageeraseescape + 1, FALSE);
      }
    }
    messageeraseescape[stringlen - 1] = ']';
    return;
  } else {
    if(CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_DEBUGMODE) {
      if(strstr(messageeraseescape, "[") != NULL ||
         strstr(messageeraseescape, "]") != NULL) {
        return;
      }
    }
  }

#ifdef _PLAYER_MOVE
  if(getPMove() != -1) {
    if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'g' && messageeraseescape[2] == 'o') {
      int point = CHAR_getInt(index, CHAR_AMPOINT);
      if(point > getPMove() || getPMove() == 0) {
        char x[4], y[4];
        easyGetTokenFromString(messageeraseescape, 2, x, sizeof(x));
        easyGetTokenFromString(messageeraseescape, 3, y, sizeof(y));
        CHAR_warpToSpecificPoint(index, CHAR_getInt(index, CHAR_FLOOR), atoi(x), atoi(y));
        CHAR_setInt(index, CHAR_AMPOINT, point - getPMove());
      } else
        CHAR_talkToCli(index, -1, "你已经没有足够的会员点数顺移了！", CHAR_COLORRED);
      return;
    }
  }
#endif

#ifdef _PLAYER_ANNOUNCE
  if(getPAnnounce() != -1) {
    if(messageeraseescape[0] == '/' && messageeraseescape[1] == 'g' && messageeraseescape[2] == 'g') {

      int point = CHAR_getInt(index, CHAR_AMPOINT);
      if(point > getPAnnounce() || getPAnnounce() == 0) {
        int i;
        int playernum = CHAR_getPlayerMaxNum();
        char buff[255];
        char xlbmsg[255];
        char *MyName = CHAR_getChar(index, CHAR_NAME);

        easyGetTokenFromString(messageeraseescape, 2, buff, sizeof(buff));
        for(i = 0; i < playernum; i++) {
          if(CHAR_getCharUse(i) != FALSE)
            sprintf(xlbmsg, "<小喇叭>%s说：%s", MyName, buff);
          CHAR_talkToCli(i, -1, xlbmsg, CHAR_COLORGREEN);
        }
        CHAR_setInt(index, CHAR_AMPOINT, point - getPAnnounce());
      } else
        CHAR_talkToCli(index, -1, "你已经没有足够的会员点数小喇叭了！", CHAR_COLORRED);
      return;
    }
  }
#endif

#ifdef _TALK_ACTION
  TalkAction(index, messageeraseescape);
#endif

  mesgvolume = CHAR_getVolume(messageeraseescape);
  if(area == 0) area = 3;
  if(mesgvolume == -1) lastvolume = -1;
  else lastvolume = area - 1 + mesgvolume;
  if(CHAR_getWorkInt(index, CHAR_WORKFLG) & WORKFLG_MICMODE) {
    lastvolume = 15;
    micflg = 1;
  }
  fl = CHAR_getInt(index, CHAR_FLOOR);
  x = CHAR_getInt(index, CHAR_X);
  y = CHAR_getInt(index, CHAR_Y);
  // Robin 0705 channel
  if(messageeraseescape[0] == ':') {
    if((channel == -1) && (quickchannel != -1))
      channel = quickchannel;
    else if(channel != -1)
      channel = -1;
  }
  {
    typedef void (*TALKF)(int, int, char *, int, int);
    TALKF talkedfunc = NULL;
    talkedfunc = (TALKF) CHAR_getFunctionPointer(index, CHAR_TALKEDFUNC);
    if(talkedfunc) {
      talkedfunc(index, index, messageeraseescape, color, channel);
    }

  }
  if(lastvolume == -1) {
    int j;
    int xx[2] = {x, x + CHAR_getDX(CHAR_getInt(index, CHAR_DIR))};
    int yy[2] = {y, y + CHAR_getDY(CHAR_getInt(index, CHAR_DIR))};
    int talk = FALSE;
    for(j = 0; j < 2; j++) {
      OBJECT object;
      for(object = MAP_getTopObj(fl, xx[j], yy[j]); object; object = NEXT_OBJECT(object)) {
        int objindex = GET_OBJINDEX(object);
        int toindex = OBJECT_getIndex(objindex);
        if(OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index) {
          if(CHAR_Talk_check(index, toindex, 0)) {
            typedef void (*TALKF)(int, int, char *, int, int);
            TALKF talkedfunc = NULL;
            talkedfunc = (TALKF) CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
            if(talkedfunc) {
              talkedfunc(toindex, index, messageeraseescape, color, -1);
            }
            if(CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
              talk = TRUE;
            }
          }
        }
      }
    }
    if(talk)CHAR_setInt(index, CHAR_TALKCOUNT, CHAR_getInt(index, CHAR_TALKCOUNT) + 1);
  } else {
    int range = CHAR_getRangeFromVolume(lastvolume);
    int i, j;
    int talk = FALSE;
    int channelTalk = FALSE;
    int partyindex[CHAR_PARTYMAX];
    for(i = 0; i < CHAR_PARTYMAX; i++) {
      partyindex[i] = -1;
    }
    if(CHAR_getWorkInt(index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
      for(i = 0; i < CHAR_PARTYMAX; i++) {
        int toindex = CHAR_getPartyIndex(index, i);
        if(CHAR_CHECKINDEX(toindex) && toindex != index) {
          typedef void (*TALKF)(int, int, char *, int, int);
          TALKF talkedfunc = NULL;
          talkedfunc = (TALKF) CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
          if(talkedfunc)
            talkedfunc(toindex, index, messageeraseescape, color, -1);
          talk = TRUE;
          partyindex[i] = toindex;
        }
      }
    }
    if(channel > -1 && fmindexi >= 0) {
      int i, tindex;

      channelTalk = TRUE;
      i = 0;
#ifdef _FMVER21
      if(channel == FAMILY_MAXCHANNEL && CHAR_getInt(index, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
        if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( index, CHAR_FMLEADERFLAG ) == 1)
#endif
      {
        char buf[512];
        sprintf(buf, "[族长广播]%s: %s", CHAR_getChar(index, CHAR_NAME), messageeraseescape);
        saacproto_ACFMAnnounce_send(acfd,
                                    CHAR_getChar(index, CHAR_FMNAME),
                                    CHAR_getInt(index, CHAR_FMINDEX),
                                    CHAR_getWorkInt(index, CHAR_WORKFMINDEXI),
                                    buf,
                                    color
        );
        return;
      }
      else

        for(i = 0; i < FAMILY_MAXMEMBER; i++) {
          tindex = channelMember[fmindexi][channel][i];
          if(!CHAR_CHECKINDEX(tindex))
            continue;
          if(tindex >= 0 && tindex != index) {
            typedef void (*TALKF)(int, int, char *, int, int);
            TALKF talkedfunc = NULL;

            talkedfunc = (TALKF) CHAR_getFunctionPointer(tindex, CHAR_TALKEDFUNC);

            if(talkedfunc)
              talkedfunc(tindex, index, messageeraseescape, color, channel);
          }
        }

      talk = TRUE;

    } else {
      for(i = x - range / 2; i <= x + range / 2; i++) {
        for(j = y - range / 2; j <= y + range / 2; j++) {
          OBJECT object;
          for(object = MAP_getTopObj(fl, i, j); object; object = NEXT_OBJECT(object)) {
            int objindex = GET_OBJINDEX(object);
            int toindex = OBJECT_getIndex(objindex);
            if(OBJECT_getType(objindex) == OBJTYPE_CHARA && toindex != index) {
              int k;
              for(k = 0; k < CHAR_PARTYMAX; k++) {
                if(toindex == partyindex[k]) {
                  break;
                }
              }
              if(k != CHAR_PARTYMAX)
                continue;
              if(CHAR_Talk_check(index, toindex, micflg)) {
                typedef void (*TALKF)(int, int, char *, int, int);
                TALKF talkedfunc = NULL;
                talkedfunc = (TALKF) CHAR_getFunctionPointer(toindex, CHAR_TALKEDFUNC);
                if(talkedfunc) {
                  talkedfunc(toindex, index, messageeraseescape, color, -1);
                }
                if(CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
                  talk = TRUE;
                }
              }
            }
          }
        }
      }
    }
    if(talk) {
      CHAR_setInt(index, CHAR_TALKCOUNT, CHAR_getInt(index, CHAR_TALKCOUNT) + 1);
    }
    if(CHAR_getInt(index, CHAR_POPUPNAMECOLOR) != color) {
      CHAR_setInt(index, CHAR_POPUPNAMECOLOR, color);
      if(talk) {
        int opt[1];
        opt[0] = color;
        CHAR_sendWatchEvent(CHAR_getWorkInt(index, CHAR_WORKOBJINDEX),
                            CHAR_ACTPOPUPNAME, opt, 1, FALSE);
      }
    }
  }
}

int CHAR_talkToCli(int talkedcharaindex, int talkcharaindex, char *message, CHAR_COLOR color) {
  static char lastbuf[2048];
  static char escapebuf[2048];
  static char mesgbuf[1024];
  int fd;

  if(!CHAR_CHECKINDEX(talkedcharaindex))
    return FALSE;
  if(CHAR_getInt(talkedcharaindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER) {
    print("err CHAR_talkToCli CHAR_WHICHTYPE != CHAR_TYPEPLAYER\n");
    return FALSE;
  }

  fd = getfdFromCharaIndex(talkedcharaindex);
  if(fd == -1) {
    print("err CHAR_talkToCli can't get fd from:%d \n", talkedcharaindex);
    return FALSE;
  }

  if(color < CHAR_COLORWHITE && color > CHAR_COLORGREEN2) {
    print("CHAR_talkToCli color err\n");
    return FALSE;
  }
  snprintf(lastbuf, sizeof(lastbuf), "P|%s", makeEscapeString(CHAR_appendNameAndTitle(talkcharaindex, message, mesgbuf, sizeof(mesgbuf)), escapebuf, sizeof(escapebuf)));
  int talkchar = -1;
  if(talkcharaindex != -1)
    talkchar = CHAR_getWorkInt(talkcharaindex, CHAR_WORKOBJINDEX);
  lssproto_TK_send(fd, talkchar, lastbuf, color);
  return TRUE;
}

#ifdef _DROPSTAKENEW

void CHAR_talkToFloor(int floor, int talkindex, char *message, CHAR_COLOR color) {
  int i = 0;
  int playernum = CHAR_getPlayerMaxNum();
  for(i = 0; i < playernum; i++) {
    if(CHAR_getCharUse(i) == FALSE) continue;
    if(!CHAR_CHECKINDEX(i))
      continue;
    if(CHAR_getInt(i, CHAR_FLOOR) == floor) {
      if(CHAR_getWorkInt(i, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE)
        CHAR_talkToCli(i, talkindex, message, color);
    }
  }
}

#endif

#ifdef _TALK_ACTION

void TalkAction(int charaindex, char *message) {
  int i;
  typedef struct {
    char talk[5];
    int action;
  } tagTlakAction;
  tagTlakAction TlakAction[13] = {
      {"坐下", 11},
      {"挥手", 12},
      {"点头", 18},
      {"高兴", 13},
      {"生气", 14},
      {"悲伤", 15},
      {"站立", 19},
      {"走动", 17},
      {"晕倒", 10},
      {"攻击", 2},
      {"防御", 16},
      {"受伤", 4},
      {"投掷", 3}
  };
  for(i = 0; i < 13; i++)
    if(strstr(message, TlakAction[i].talk))
      break;

  if(i < 13) {
    CHAR_setWorkInt(charaindex, CHAR_WORKACTION, TlakAction[i].action);
    CHAR_sendWatchEvent(CHAR_getWorkInt(charaindex, CHAR_WORKOBJINDEX), TlakAction[i].action, NULL, 0, TRUE);
  }
}

#endif
#ifdef _GM_ITEM
static int player_useChatMagic( int charaindex, char* data, int isDebug)
{
  char    magicname[256];
  int     ret;

  CHATMAGICFUNC   func;

  ret = getStringFromIndexWithDelim( data, " ", 1, magicname,  sizeof( magicname));
  if( ret == FALSE)return FALSE;

  func = CHAR_getChatMagicFuncPointer(magicname,isDebug);

  if( func ){
    LogGM( CHAR_getUseName( charaindex), CHAR_getChar( charaindex, CHAR_CDKEY), data,
      CHAR_getInt( charaindex, CHAR_FLOOR), CHAR_getInt( charaindex, CHAR_X),
      CHAR_getInt( charaindex, CHAR_Y) );
    func( charaindex, data + strlen( magicname)+1);
    return TRUE;
  }else{
    return FALSE;
  }
}
#endif

