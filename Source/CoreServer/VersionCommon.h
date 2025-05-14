#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER


#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#if (_MSC_VER > 1200)
#define		__VS2003
#endif

#define		__SERVER
#define		__MAP_SIZE
#define		__S8_SERVER_PORT
#define		__EVE_NEWYEAR
#define		__STL_0402				// stl


#define		__JEFF_9_20


#define		__MA_VER11_01


#define		__AUTO_NOTICE







#define		__LAYER_1015




	#define		__QUIZ

	#define		__EVENTLUA_RAIN

	#define		__EVENTLUA_SNOW





#if	  defined(__INTERNALSERVER)

	#define		__GUILDVOTE

//	#define		__SYS_PLAYER_DATA



#elif defined(__TESTSERVER)


	#define		__GUILDVOTE

//	#define		__SYS_PLAYER_DATA





#elif defined(__MAINSERVER)



#endif


#endif


