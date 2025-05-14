#ifndef __VERSION_COMMON__
#define __VERSION_COMMON__

#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER
#define		__S1108_BACK_END_SYSTEM

#define		__LANG_1013

#define		__STL_0402					// stl

#define		__ITEMDROPRATE
#define		__GETMAILREALTIME
#define		__S_RECOMMEND_EVE

#define		__RULE_0615

#define		__PROP_0827


#define		__EVENTLUA_COUPON
#define		__LOG_PLAYERCOUNT_CHANNEL

#define		__CONV_0906
#define		__INVALID_LOGIN_0320
#define		__INVALID_LOGIN_0612
#define		__VENDOR_1106

#define		__JEFF_9_20
#define		__JEFF_FIX_0

#define	__REMOVE_PLAYER_0221

#define		__SEND_ITEM_ULTIMATE

#if (_MSC_VER > 1200)
#define		__VS2003
#endif

#define		__AUTO_NOTICE


//	#define		__SECRET_ROOM
//	#define		__TAX
//	#define		__LORD






#define		__PET_1024
#define		__BUFF_1107

//	#define		__COUPLE_1117



#define		__OCCUPATION_SHOPITEM

#define		__POST_DUP_1209

#define		__LAYER_1015

#define		__FUNNY_COIN

#define		__REMOVEITEM_POCKET

#define		__PERIN_BUY_BUG





#define		__QUIZ













#if	  defined(__INTERNALSERVER)
//	#define	__LANG_1013
//	#define	__RULE_0615

	#define		__GUILDVOTE
	#define		__VERIFY_MEMPOOL


	#define		__Y_MAX_LEVEL_8













//	#define		__CONV_0906


//	#define		__SYS_COLLECTING







	#define		__JEFF_11_2

	#define		 __GUILD_HOUSE_MIDDLE



#elif defined(__TESTSERVER)


	#define		__GUILDVOTE
	#define		__Y_MAX_LEVEL_8






//	#undef	__VER



//	#define		__SYS_COLLECTING



//	#define		__CONV_0906




	#define	__JEFF_11_2

	#define __GUILD_HOUSE_MIDDLE


#elif defined(__MAINSERVER)

	#define		__IDC
//	#define		__TOOMANY_PENDINGLOG1115

#endif

#endif