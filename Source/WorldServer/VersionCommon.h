#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
#define __INTERNALSERVER
#endif

#if (_MSC_VER > 1200)
#define		__VS2003
#endif


#define		D3DDEVICE	NULL
#define		__SERVER
#define		__X15
#define		__VERIFY_LOOP041010
#define		__S1108_BACK_END_SYSTEM
#define		__RES0807
#define		__CPU_UTILDOWN_060502
#define		__SLIDE_060502
#define		__S8_SERVER_PORT
#define		__NEWYEARDAY_EVENT_COUPON

#define		__LANG_1013
#define		__HACK_1130
#define		__QUEST_1208
#define		__S_SERVER_UNIFY


#define		__BUGFIX_0326


#define		__INFINITE_0227

#define		__LANG_IME_0327
#define		__STL_0402						// stl

#define		__TRADESYS
#define		__EVE_BALLOON

#define		__ITEMDROPRATE

#define		__PK_PVP_SKILL_REGION
#define		__PVPDEBUFSKILL
#define		__S_RECOMMEND_EVE
#define		__EVE_MINIGAME

#define		__ANGEL_LOG
#define		__S_ADD_EXP

#define		__RULE_0615
#define		__S_ADD_RESTATE




#define		__PROP_0827
#define		__RIGHTHAND_SKILL
#define		__LOG_MATCHLESS

#define		__PKSERVER_USE_ANGEL


#define		__EVENTLUA_COUPON
#define		__NOLIMIT_RIDE_ITEM
#define		__NPC_BUFF

#define		__JEFF_11
#define		__SYS_TICKET

#define		__HACK_1023
#define		__VENDOR_1106

#define		__INVALID_LOGIN_0320

#define		__JEFF_9_20

#define		__EVENTLUA_GIFT

#define		__JEFF_11_4
#define		__JEFF_11_5
#define		__JEFF_11_6

#define		__DST_GIFTBOX
#define		__EVENT_MONSTER

#define		__CHIPI_DYO
#define		__STL_GIFTBOX_VECTOR

#define		__VM_0820


#define		__LAYER_1015
#define		__LAYER_1020
#define		__LAYER_1021
#define		__AZRIA_1023
#define		__PET_1024
#define		__BUFF_1107

#define		__OCCUPATION_SHOPITEM

#define		__SYNC_1217
#define		__SPEED_SYNC_0108
#define		__SYS_ITEMTRANSY

#define		__EVENTLUA_CHEEREXP

#define		__FUNNY_COIN

#define		__MAP_SECURITY

#define		__QUIZ

#define		__BUFF_TOGIFT
#define		__EVENTLUA_SPAWN
#define		__EVENTLUA_KEEPCONNECT

#define		__PERIN_BUY_BUG


#define		__EVENTLUA_RAIN

#define		__EVENTLUA_SNOW

#define		__ADD_RESTATE_LOW


#define		__FORCE_KILL_SERVER

#define		__SHOP_COST_RATE


#define		__PROTECT_AWAKE

#if	  defined(__INTERNALSERVER)
//	#define	__RULE_0615

//	#define		__VERIFY_MEMPOOL
#define		__GUILDVOTE
#define		__SKILL0517

#define		__Y_CASTING_SKIP
#define		__YAIMONSTER_EX
#define		__Y_PATROL
#define		__V060721_TEXTDRAG
#define		__Y_BEAUTY_SHOP_CHARGE

#define		__Y_FLAG_SKILL_BUFF


#define		__Y_MAX_LEVEL_8
#define		__Y_HAIR_BUG_FIX
#define		__EVENT_0117

#define		__TRAFIC_1215



#define		__BUGFIX_0326

#define		__GLOBAL_COUNT_0705			// CTime::GetTimer


#define		__JEFF_11_1
#define		__JEFF_11_3


#define		__OPT_MEM_0811
//	#define		__MEM_TRACE

#define		__PROTECT_AWAKE


#undef		__VER
#define		__VER 16

#define		__GUILD_HOUSE_MIDDLE

#define		__MOVER_STATE_EFFECT

#define		__NEW_ITEM_VARUNA


#elif defined(__TESTSERVER)

#define		__GUILDVOTE

#define		__Y_CASTING_SKIP
#define		__YAIMONSTER_EX
#define		__Y_PATROL
#define		__V060721_TEXTDRAG
#define		__Y_BEAUTY_SHOP_CHARGE
#define		__Y_FLAG_SKILL_BUFF

#define		__Y_MAX_LEVEL_8
#define		__TRAFIC_1215






//	#define		__PVPDEMAGE0608				// 9th PVP DEMAGE edit


//	#define		__Y_DRAGON_FIRE





//	#define		__METEONYKER_0608




#define		__GLOBAL_COUNT_0705			// CTime::GetTimer








//	#undef	__VER


#define		__JEFF_11_1
#define		__JEFF_11_3





#define		__OPT_MEM_0811
//	#define		__MEM_TRACE


#define		__GUILD_HOUSE_MIDDLE

#elif defined(__MAINSERVER)

#define		__ON_ERROR
#define		__IDC
#define		__Y_BEAUTY_SHOP_CHARGE
#define		__TRAFIC_1215




#define		__JEFF_11_1
#define		__JEFF_11_3

#define		__EVENT_0117				// propEvent.inc spawn



#define		__OPT_MEM_0811
//	#define		__MEM_TRACE

#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

#endif


#define		__REMOVE_SCIRPT_060712

#endif