#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__

#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__DOS1101
#define		__SERVER
#define		__CRC
#define		__SO1014
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__VERIFYNETLIB
#define		__MAP_SIZE
#define		__S8_SERVER_PORT
#define		__STL_0402					// stl

// 11
#define		__MA_VER11_07

#if (_MSC_VER > 1200)
#define		__VS2003
#endif

//#define		__US_LOGIN_0223


#if	  defined(__INTERNALSERVER)
	#define		__GUILDVOTE
	#define		__VERIFY_ARCHIVE1129
	#define		__CRASH_0404
#elif defined(__TESTSERVER)
	#define		__GUILDVOTE
#elif defined(__MAINSERVER)

#endif


#endif
