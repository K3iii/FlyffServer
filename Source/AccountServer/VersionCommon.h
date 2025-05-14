#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__
#define __MAINSERVER


#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER

#define		__STL_0402		// stl

#if (_MSC_VER > 1200)
#define		__VS2003
#endif

// 08-01-22

// 11th
#define __LOG_PLAYERCOUNT_CHANNEL

#define	__REMOVE_PLAYER_0221

// 14th




#if	  defined(__INTERNALSERVER)
	#define	__SECURITY_0628

#elif defined(__TESTSERVER)

#elif defined(__MAINSERVER)


#endif

#endif	// __VERSION_COMMON_H__