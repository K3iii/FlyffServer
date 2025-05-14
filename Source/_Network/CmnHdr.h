#ifndef __CMNHDR_H__
#define __CMNHDR_H__

//#define FD_SETSIZE	1024

#include <Winsock2.h>
#include <tlhelp32.h.>
#include "CMcl.h"

#if defined (__WORLDSERVER) || defined(__CLIENT)
#include "defineItem.h"
extern	time_t	g_tCurrent;
#endif // ( __WORLDSERVER || __CLIENT )

#include "profile.h"

#pragma warning( disable:4001 )	// nonstandard extension 'single line comment' was used
#pragma warning( disable:4100 )	// unreferenced formal parameter
#pragma warning( disable:4699 )	// Note: Creating precompiled header
#pragma warning( disable:4710 )	// function not inlined
#pragma warning( disable:4514 )	// unreferenced inline function has been removed
#pragma warning( disable:4512 )	// assignment operator could not be generated
#pragma warning( disable:4310 )	// cast truncates constant value
#pragma warning( disable:4786 )	// identifier was truncated to '255' characters in the browser information

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
	pvParam, fdwCreate, pdwThreadId)                 \
	((HANDLE)_beginthreadex(                      \
	(void *)        (psa),                     \
	(unsigned)      (cbStack),                 \
	(PTHREAD_START) (pfnStartAddr),            \
	(void *)        (pvParam),                 \
	(unsigned)      (fdwCreate),               \
	(unsigned *)    (pdwThreadId)))

#define chSTR2(x)   #x
#define chSTR(x)    chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))
#define chDIMOF(Array) (sizeof(Array) / sizeof(Array[0]))
#define chSIZEOFSTRING(psz)   ((lstrlen(psz) + 1) * sizeof(TCHAR))

//#define IDHash(id,HashSize)	(((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
inline DWORD IDHash( DWORD id, size_t HashSize )
{
	return (((id) + (id>>8) + (id>>16) + (id>>24)) & (HashSize-1));
}


#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

inline void chMB( PCSTR s )
{
	char szTMP[256];
	GetModuleFileNameA( NULL, szTMP, chDIMOF( szTMP ) );
	HWND hwnd	= GetActiveWindow();
	MessageBoxA( hwnd, s, szTMP, MB_OK | ( ( hwnd == NULL ) ? MB_SERVICE_NOTIFICATION : 0 ) );
}

inline void chMBANDDEBUG( PSTR szMsg )
{
	chMB( szMsg );
	DebugBreak();
}
/*
inline void chASSERTFAIL( LPCSTR file, int line, PCSTR expr )
{
	char sz[256];
	wsprintfA( sz, "File %s, line %d : %s", file, line, expr );
	chMBANDDEBUG( sz );
}
*/
inline BOOL IsPrime( u_long n )
{
	u_long i = 2, j = n;
	while( i < j ) {
		if( n % i == 0 )
			return FALSE;
		j = n / i++;
	}
	return TRUE;
}

typedef	DWORD	SERIALNUMBER;


inline int GetIntFromStr(char *pBuf, int *pLocation)
{
	char strTemp[50];
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strTemp[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strTemp[count]=0;(*pLocation)++;
	return atoi(strTemp);
}

inline void GetStrFromStr(char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!='/' && pBuf[*pLocation]!=0) {
		strReturn[count]=pBuf[*pLocation];
		count++; (*pLocation)++;
	}
	strReturn[count]=0;(*pLocation)++;
}

#if __VER >= 11 // __SYS_IDENTIFY
inline	__int64 GetInt64PaFromStr( char *pBuf, int *pLocation )
{
	char strTemp[50];
	int count	=0;
	while( pBuf[*pLocation] != ',' && pBuf[*pLocation] != 0 )
	{
		if( pBuf[*pLocation] != '/' )
		{
			strTemp[count]	=pBuf[*pLocation];
			count++;	( *pLocation )++;
		}
		else
		{
			if( count == 0 )
			{
				return 0;
			}
			else
			{
				strTemp[count]	=0;
				return _atoi64( strTemp );
			}
		}
	}
	strTemp[count]	=0;	( *pLocation )++;
	return _atoi64( strTemp );
}
#endif	// __SYS_IDENTIFY



inline int GetIntPaFromStr(char *pBuf, int *pLocation, int nReturnValue = 0 )
{
	char strTemp[50];
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!=0)
	{
		if( pBuf[*pLocation]!='/' )
		{
			strTemp[count]=pBuf[*pLocation];
			count++; (*pLocation)++;
		}
		else
		{
			if( count == 0 )
			{
				return nReturnValue;
			}
			else
			{
				strTemp[count]=0;
				return atoi(strTemp);
			}
		}
	}
	strTemp[count]=0;(*pLocation)++;
	return atoi(strTemp);
}

inline SERIALNUMBER	GetSerialNumberPaFromStr(char *pBuf, int *pLocation )
{
	char strTemp[50];
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!=0)
	{
		if( pBuf[*pLocation]!='/' )
		{
			strTemp[count]=pBuf[*pLocation];
			count++; (*pLocation)++;
		}
		else
		{
			if( count == 0 )
			{
				return (SERIALNUMBER)0;
			}
			else
			{
				strTemp[count]=0;
				return atoi(strTemp);
			}
		}
	}
	strTemp[count]=0;(*pLocation)++;
	return atoi(strTemp);
}

inline void GetStrPaFromStr(char *pBuf, char *strReturn, int *pLocation)
{
	int count=0;
	while(pBuf[*pLocation]!=',' && pBuf[*pLocation]!=0)
	{
		if( pBuf[*pLocation]!='/' )
		{
			strReturn[count]=pBuf[*pLocation];
			count++; (*pLocation)++;
		}
		else
		{
			if( count == 0 )
			{
				strReturn[0] = '\0';
			}
			else
			{
				strReturn[count]=pBuf[*pLocation];
			}
			return;
		}
	}
	strReturn[count]=0;(*pLocation)++;
}

inline BOOL chWindows9x( void )
{
	OSVERSIONINFO vi	= { sizeof(vi) };
	GetVersionEx( &vi );
	return( vi.dwPlatformId != VER_PLATFORM_WIN32_NT );
}

inline HANDLE CreateNewCompletionPort( DWORD dwNumberOfConcurrentThreads )
{
	return( CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads ) );
}

class CSystemInfo : public SYSTEM_INFO
{
public:
	CSystemInfo()	{	GetSystemInfo( this );	}
};

#define	CLOSE_HANDLE(h)		\
	{	\
		if( h )	\
		{	\
			CloseHandle( h );	\
			( h )	= (HANDLE)NULL;	\
		}	\
	}

#define	CLOSE_WSAEVENT(h)	\
	{	\
		if( h != WSA_INVALID_EVENT )	\
		{	\
			WSACloseEvent( h );	\
			( h )	= WSA_INVALID_EVENT;	\
		}	\
	}

#define	CLOSE_THREAD( hThread, hClose )	\
	{	\
		if( hThread )	\
		{	\
			SetEvent( hClose );	\
			WaitForSingleObject( hThread, INFINITE );	\
			CloseHandle( hThread );	\
			CloseHandle( hClose );	\
			hThread		= hClose	= (HANDLE)NULL;	\
		}	\
	}

#define CLOSE_SOCKET( s )	\
	{	\
		if( s != INVALID_SOCKET )	\
		{	\
			shutdown( s, SD_BOTH );	\
			closesocket( s );	\
			( s )	= INVALID_SOCKET;	\
		}	\
	}

inline void	GetAddr( CHAR* lpAddr )
{
	WORD wVersionRequested	= MAKEWORD( 2, 2 );
	WSADATA wsaData;
	PHOSTENT hostinfo;

	if( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
//		if( LOBYTE( wsaData.wVersion ) == 2 &&
//			HIBYTE( wsaData.wVersion ) == 2 )
//		{
			CHAR name[260];
			if( gethostname( name, sizeof(name) ) == 0 )
			{
				if( ( hostinfo = gethostbyname( name ) ) )
					strcpy( lpAddr, inet_ntoa( *(struct in_addr*)*hostinfo->h_addr_list ) );
			}
//		}
	}
	WSACleanup( );
}

inline void	NotifyProcessStart( DWORD dwId )
{
#ifdef __NOTIFYSTART0706

	char szName[256];
	sprintf( szName, "%d", dwId );
	HANDLE hEvent = OpenEvent( EVENT_MODIFY_STATE, FALSE, szName );
	if( hEvent )
	{
		SetEvent( hEvent );
		CloseHandle( hEvent );
	}
	else
	{
		char szBuffer[256];
		sprintf( szBuffer, "NotifyProcessStart() - can't open Event:%s\n", szName );
		OutputDebugString( szBuffer );
	}

#else

	DWORD dwData = 0x00000001;
	HANDLE	hWriteHandle	= GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD dwBytesWritten;
	WriteFile( hWriteHandle, &dwData, sizeof(dwData), &dwBytesWritten, NULL );

#endif
}


typedef	struct	tagPLAY_ACCOUNT
{
	TCHAR	lpszAccount[36];
	SYSTEMTIME	m_stLogin;
	TCHAR	lpAddr[16];
}
PLAY_ACCOUNT;


class CMover;
extern int ParsingCommand( LPCTSTR lpszString, CMover* pMover, BOOL bItem = FALSE );
extern void ParsingEffect( TCHAR* pChar, int nLen );
extern	void RemoveCRLF( char* szString );
extern void Removeat(char* szString);

#define	MAX_ACCOUNT		42
#define	MAX_PASSWORD	42
#define	MAX_PLAYER		42
#define	MAX_SESSION_PWD		64

#define	CRIT_SEC	CMclCritSec
#define	theLineFile

#if defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)


enum
{
	SM_BANK15, SM_BANK30, SM_PARTYSKILL15, SM_PARTYSKILL30, SM_SHOUT15, SM_SHOUT30,
    SM_MAINTAIN_MP, SM_MAINTAIN_FP, SM_MAX_HP, SM_MAX_HP50, SM_ACTPOINT,
	SM_STR_DOWN, SM_STA_DOWN, SM_DEX_DOWN, SM_INT_DOWN,
	SM_ATTACK_UP,
	SM_ESCAPE, SM_REVIVAL, SM_VELOCIJUMP, SM_PARTYSKILL1,
	SM_SHOUT001, SM_BANK001,
	SM_MAX
};


#if defined (__WORLDSERVER) || defined(__CLIENT)

typedef struct _ADDSMMODE
{
	BOOL bSMModetime[SM_MAX];
	DWORD dwSMItemID[SM_MAX];
//	int	 nValue[SM_MAX];
	_ADDSMMODE()
	{
		memset( bSMModetime, 0, sizeof( BOOL ) * SM_MAX );
		memset( dwSMItemID, 0, sizeof( DWORD ) * SM_MAX );
//		memset( nValue, 0, sizeof( int ) * SM_MAX );

		bSMModetime[SM_BANK15] = TRUE;
		bSMModetime[SM_BANK30] = TRUE;
		bSMModetime[SM_BANK001] = TRUE;
		bSMModetime[SM_PARTYSKILL1]		= TRUE;
		bSMModetime[SM_PARTYSKILL15]	= TRUE;
		bSMModetime[SM_PARTYSKILL30]	= TRUE;
		bSMModetime[SM_SHOUT15] = TRUE;
		bSMModetime[SM_SHOUT30] = TRUE;
		bSMModetime[SM_SHOUT001] = TRUE;
		bSMModetime[SM_ESCAPE] = TRUE;
		dwSMItemID[SM_BANK15] = II_CHR_SYS_SCR_COMMBANK15;
		dwSMItemID[SM_BANK30] = II_CHR_SYS_SCR_COMMBANK30;
		dwSMItemID[SM_BANK001] = II_CHR_SYS_SCR_COMMBANK001;
		dwSMItemID[SM_PARTYSKILL1]		= II_CHR_SYS_SCR_PSKILLFULL1;
		dwSMItemID[SM_PARTYSKILL15] = II_CHR_SYS_SCR_PSKILLFULL15;
		dwSMItemID[SM_PARTYSKILL30] = II_CHR_SYS_SCR_PSKILLFULL30;
		dwSMItemID[SM_SHOUT15] = II_CHR_SYS_SCR_SHOUTFULL15;
		dwSMItemID[SM_SHOUT30] = II_CHR_SYS_SCR_SHOUTFULL30;
		dwSMItemID[SM_SHOUT001] = II_CHR_SYS_SCR_SHOUTFULL001;
		dwSMItemID[SM_MAINTAIN_MP] = II_CHR_REF_REF_HOLD;
		dwSMItemID[SM_MAINTAIN_FP] = II_CHR_POT_DRI_VITALX;
		dwSMItemID[SM_MAX_HP] = II_CHR_FOO_COO_BULLHAMS;
		dwSMItemID[SM_MAX_HP50] = II_CHR_FOO_COO_GRILLEDEEL;
		dwSMItemID[SM_ACTPOINT] = II_CHR_SYS_SCR_ACTIVITION;
		dwSMItemID[SM_STR_DOWN] = II_SYS_SYS_VIR_PENALTYSTR;
		dwSMItemID[SM_STA_DOWN] = II_SYS_SYS_VIR_PENALTYSTA;
		dwSMItemID[SM_DEX_DOWN] = II_SYS_SYS_VIR_PENALTYDEX;
		dwSMItemID[SM_INT_DOWN] = II_SYS_SYS_VIR_PENALTYINT;
		dwSMItemID[SM_ATTACK_UP]				= II_CHR_SYS_SCR_UPCUTSTONE;
		dwSMItemID[SM_REVIVAL]					= II_SYS_SYS_SCR_BLESSING;
		dwSMItemID[SM_VELOCIJUMP]				= II_SYS_SYS_SCR_VELOCIJUMP;
		dwSMItemID[SM_ESCAPE]					= II_CHR_SYS_SCR_ESCAPEBLINKWING;
	}
}
ADDSMMODE, *PADDSMMODE;
#endif // defined (__WORLDSERVER) || defined(__CLIENT)

#endif // defined (__WORLDSERVER) || defined(__CLIENT) || defined(__CORESERVER) || defined(__DBSERVER)

#define MAX_PIERCING_SUIT		4
#define MAX_PIERCING_WEAPON		10
#define	MAX_PIERCING_ULTIMATE	5
#define	MAX_PIERCING			10
#if __VER >= 15 // __PETVIS
#define MAX_VIS					9

#define	FAILED_BOTH_NEEDVIS	0
#define	FAILED_1ST_NEEDVIS	1
#define	FAILED_2ND_NEEDVIS	2
#define SUCCSESS_NEEDVIS	3
#define UNDEFINED_NEEDVIS	4
#endif // __PETVIS

typedef	struct	_LogItemInfo
{
	LPCTSTR Action;
	LPCTSTR SendName;
	LPCTSTR RecvName;
	DWORD	WorldId;
	DWORD	Gold;
	DWORD	Gold2;
	SERIALNUMBER	ItemNo;
	int		Negudo;
	TCHAR	szItemName[32];
	DWORD	itemNumber;
	int		nAbilityOption;
	DWORD	Gold_1;
	int		nSlot;
	int		nSlot1;
	int		nItemResist;
	int		nResistAbilityOption;
	DWORD	m_dwKeepTime;
	__int64	m_iRandomOptItemId;
	int 	nPiercedSize;
	int		adwItemId[MAX_PIERCING_WEAPON];
	int		nUMPiercedSize;
	int		adwUMItemId[MAX_PIERCING_ULTIMATE];
	BYTE	nPetKind;
	BYTE	nPetLevel;
	DWORD	dwPetExp;
	WORD	wPetEnergy;
	WORD	wPetLife;
	BYTE	nPetAL_D;
	BYTE	nPetAL_C;
	BYTE	nPetAL_B;
	BYTE	nPetAL_A;
	BYTE	nPetAL_S;

	_LogItemInfo()
	{
		Action = _T("");
		SendName = _T("");
		RecvName = _T("");
		WorldId = Gold = Gold2 = 0;
		Negudo = 0;
		ItemNo = 0;
		::memset( szItemName, 0, sizeof(szItemName) );
		itemNumber = 0;
		nAbilityOption = 0;
		Gold_1 = 0;
		nSlot = 100;
		nSlot1 = 100;
		nItemResist = 0;
		nResistAbilityOption = 0;
		m_dwKeepTime = 0;
		m_iRandomOptItemId = 0;
		nPiercedSize = 0;
		memset( adwItemId, 0, sizeof(adwItemId) );
		nUMPiercedSize = 0;
		memset( adwUMItemId, 0, sizeof(adwUMItemId) );
		nPetKind = 0;
		nPetLevel = 0;
		dwPetExp = 0;
		wPetEnergy = 0;
		wPetLife = 0;
		nPetAL_D = 0;
		nPetAL_C = 0;
		nPetAL_B = 0;
		nPetAL_A = 0;
		nPetAL_S = 0;
	}
}	LogItemInfo,	*PLogItemInfo;

inline time_t	time_null( void )
{
#if defined(__WORLDSERVER) || defined(__CLIENT)
	return	g_tCurrent;
#else
	return time( NULL );
#endif
}

#endif	// __CMNHDR_H__
