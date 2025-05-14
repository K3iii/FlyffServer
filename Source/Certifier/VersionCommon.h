#ifndef __VERSION_COMMON_H__
#define	__VERSION_COMMON_H__

#define __MAINSERVER
#if !defined( __TESTSERVER ) && !defined( __MAINSERVER )
	#define __INTERNALSERVER
#endif

#define		__SERVER

#define		__CRC
#define		__SO1014
#define		__PROTOCOL0910
#define		__PROTOCOL1021
#define		__VERIFYNETLIB
#define		__DOS1101
#define		__STL_0402				// stl

// 14th


#define __ENCRYPT_PASSWORD

#if (_MSC_VER > 1200)
#define		__VS2003
#endif




#if	  defined(__INTERNALSERVER)
	#define	__SECURITY_0628
#elif defined(__TESTSERVER)



#elif defined(__MAINSERVER)

#endif

#endif	// __VERSION_COMMON_H__