#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER

#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#if (_MSC_VER > 1200)
#define		__VS2003
#endif

#define __CPU_UTILDOWN_060502

#define		__TRANSFER_ERROR_TEXT
#define		__CRC
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__INFO_SOCKLIB0516
#define		__S1108_BACK_END_SYSTEM
#define		__FIX_WND_1109

#define		__S_SERVER_UNIFY
#define		__LANG_IME_0327
#define		__STL_0402					// stl

#define		__NEWYEARDAY_EVENT_COUPON

#define		__LANG_1013

#define		__ITEMDROPRATE


// 10
#define		__TRADESYS
#define		__EVE_BALLOON
#define		__CSC_GAME_GRADE

#define		__PK_PVP_SKILL_REGION
#define		__S_RECOMMEND_EVE
#define		__EVE_MINIGAME
#define		__S_ADD_EXP

#define		__RULE_0615
#define		__S_ADD_RESTATE

#define		__PROP_0827

#define		__PKSERVER_USE_ANGEL
#define		__CSC_ENCHANT_EFFECT_2




















#define		__EVENTLUA_COUPON
#define		__NOLIMIT_RIDE_ITEM
#define		__NPC_BUFF











#define		__SYS_TICKET

#define		__HACK_1023
#define		__VENDOR_1106
#define		__JEFF_11

#define		__JEFF_9_20
#define		__JEFF_11_4
#define		__JEFF_11_5
#define		__JEFF_11_6
#define		__DST_GIFTBOX

#define		__SFX_OPT
//#define		__PERF_0229

#define		__HELP_BUG_FIX

#define		__CHIPI_DYO





















//	#define		__LEAK_0827





	#define		__HOUSING

//	#define		__CHIPI_QUESTITEM_FLAG


	#define		__MAX_BUY_ITEM9999






#define		__LAYER_1020
#define		__AZRIA_1023
#define		__PET_1024
#define		__BUFF_1107
#define		__SYNC_1217
#define		__SYS_ITEMTRANSY

#define		__SPEED_SYNC_0108
#define		__PARTY_DEBUG_0129

#define		__FUNNY_COIN
#define		__WINDOW_INTERFACE_BUG

#define		__MAP_SECURITY


	#define		__NEW_CONTINENT
	#define		__SMELT_SAFETY
	#define		__INSTANCE_DUNGEON
	#define		__EQUIP_BIND
	#define		__EXT_ATTRIBUTE
	#define		__NEW_ITEM_LIMIT_LEVEL
	#define		__WATER_EXT
	#define		__WND_EDIT_NUMBER_MODE
#define		__SHIFT_KEY_CORRECTION
	#define		__STATIC_ALIGN
	#define		__ITEM_DROP_SOUND
	#define		__JOB_TEXT
	#define		__RESTATE_CONFIRM
	#define		__PREVENTION_TOOLTIP_BUG
	#define		__DROP_CONFIRM_BUG
	#define		__CLOUD_ANIMATION_BUG
	#define		__BUFF_CRASH
	#define		__BS_FIX_SHADOW_ONOBJECT
	#define		__BS_FIX_HAIR_AMBIENT
	#define		__BALLOON_CODE_IMPROVEMENT
	#define		__WING_ITEM
	#define		__PCBANG
//	end 14th


	#define		__DYNAMIC_MPU
	#define		__BOUND_BOX_COLLISION
	#define		__BS_CHANGING_ENVIR
	#define		__USING_CONTINENT_DATA
	#define		__IMPROVE_QUEST_INTERFACE
	#define		__IMPROVE_SYSTEM_VER15
	#define		__15TH_INSTANCE_DUNGEON
	#define		__PETVIS
	#define		__GUILD_HOUSE
	#define		__TELEPORTER
	#define		__FIND_OBJ_INSIGHT
	#define		__HERO129_VER15
	#define		__MUSIC2
#define		__NEW_CONTINENT15
	#define		__BS_BBOX_ABS_EXTENT
	#define		__CAMPUS
	#define		__BS_FIXED_KNOCKBACK
	#define		__BS_FIXED_EQUIPMOTION
	#define		__15_5TH_ELEMENTAL_SMELT_SAFETY
	#define		__2ND_PASSWORD_SYSTEM
 // end 15th

	#define		__QUIZ

	#define		__EVENTLUA_RAIN

	#define		__EVENTLUA_SNOW

	#define		__ADD_RESTATE_LOW

	#define		__YS_CHATTING_BLOCKING_SYSTEM
	#define		__BAN_CHATTING_SYSTEM


	#define __ENCRYPT_PASSWORD



	#define		__DELETE_CHAR_CHANGE_KEY_VALUE
	#define		__BS_ADJUST_SYNC

	#define		__SHOP_COST_RATE


	#define		__PROTECT_AWAKE
	#define		__MAIL_REQUESTING_BOX

	#define		__BS_FIX_ARRIVEPOS_ALGO
	#define		__BS_ITEM_UNLIMITEDTIME



#if	  defined(__INTERNALSERVER)
//	#define	__RULE_0615

	#define		NO_GAMEGUARD
	#undef		__TRANSFER_ERROR_TEXT

	#define		__CPU_UTILDOWN_060502
	#define		__SLIDE_060502
	#define		__GUILDVOTE
	#define		__YNOTICE_UNI1026
//	#define		__YENV
//	#define		__YENV_WITHOUT_BUMP
	#define		__SKILL0517
	#define		__YAIMONSTER_EX

	#define		__VERIFY_MEMPOOL

	#define		__Y_ROTXZ
	#define		__Y_PATROL
	#define		__Y_CHARACTER_TEXT
	#define		__Y_RENDER_NAME_OPTION
	#define		__V060721_TEXTDRAG
	#define		__Y_MAPVIEW_EX
	#define		__Y_BEAUTY_SHOP_CHARGE
















	#define		__FIX_WND_1109








	#define		__TRAFIC_1218
	#define		__Y_HAIR_BUG_FIX
	#define		__FOR_PROLOGUE_UPDATE

	#define		__TRAFIC_1215

























//	#define		__METEONYKER_0608



	#define		__SECURITY_0628



	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

















//	#define		__JEFF_11

	#define		__JEFF_11_1





	#define		__DISABLE_GAMMA_WND

	#define		__ATTACH_MODEL


//	#define		__FLYFF_INITPAGE_EXT

	#define		__BS_CHECKLEAK
	#define		__BS_ADJUST_COLLISION


	#undef		__VER
	#define		__VER 15

	#define		__GUILD_HOUSE_MIDDLE
	#define		__BS_ADDOBJATTR_INVISIBLE
	#define		__BS_DEATH_ACTION
	#define		__BS_ADD_CONTINENT_WEATHER
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD


#elif defined(__TESTSERVER)

	#define		NO_GAMEGUARD

	#define		__CPU_UTILDOWN_060502
	#define		__SLIDE_060502
	#define     __GUILDVOTE

	#define		__Y_CHARACTER_TEXT
	#define		__V060721_TEXTDRAG
	#define		__Y_ROTXZ
	#define		__Y_PATROL
	#define		__Y_BEAUTY_SHOP_CHARGE
	#define		__TRAFIC_1215

























//	#define		__METEONYKER_0608


	#define		__GLOBAL_COUNT_0705			// CTime::GetTimer

//	#undef		__VER


	#define		__JEFF_11_1






//   VER 16 __TESTSERVER begin
	#define		__GUILD_HOUSE_MIDDLE
	#define		__BS_ADDOBJATTR_INVISIBLE
	#define		__BS_DEATH_ACTION

	#define		__BS_ADD_CONTINENT_WEATHER
	#define		__BS_CHANGEABLE_WORLD_SEACLOUD
//   VER 16 __TESTSERVER end


#elif defined(__MAINSERVER)

	#define		__Y_BEAUTY_SHOP_CHARGE
	#define		__TRAFIC_1215




	#define		__JEFF_11_1


#endif
#endif // VERSION_COMMON_H
