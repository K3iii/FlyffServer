#include "stdafx.h"
#include "resource.h"
#include "defineText.h"
#include "AppDefine.h"
#include "Webbox.h"
#include "dpmng.h"
#include "WorldMap.h"
#include "msghdr.h"

HANDLE	g_hMutex = NULL;
static char		g_szProtocolVersion[32];			// MSG_VER
static HHOOK	g_hHook;


LPCTSTR GetProtocolVersion()
{
	return g_szProtocolVersion;
}

#ifndef __AIO_NEUZAPP
LRESULT CALLBACK LowLevelKeyboardProc (INT nCode, WPARAM wParam, LPARAM lParam)
{
    // By returning a non-zero value from the hook procedure, the
    // message does not get passed to the target window
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    BOOL bControlKeyDown = 0;

    switch (nCode)
    {
	case HC_ACTION:
        {
            // Check to see if the CTRL key is pressed
            bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);

            // Disable CTRL+ESC
            if (pkbhs->vkCode == VK_ESCAPE && bControlKeyDown)
                return 1;

            // Disable ALT+ESC
            if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags & LLKHF_ALTDOWN)
                return 1;
			if (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN)
				return 1;

            break;
        }

	default:
		break;
    }
    return CallNextHookEx (g_hHook, nCode, wParam, lParam);
}
#endif

void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];

	LoadString(hInstance, IDS_LANG, szLang, 16);
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);
	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );
#ifdef __LANG_1013
	char szCodePage[16]	= { 0,};
	LoadString( hInstance, IDS_CODEPAGE, szCodePage, 16 );
	g_codePage	= atoi( szCodePage );
#endif	// __LANG_1013
}

void InitGlobalVars( HINSTANCE hInstance )
{
	InitLanguageFromResource( hInstance );
	SetCodePage( ::GetLanguage() );

	char szEnryptPWD[4];
	char szCachePort[16];
	char szSecond[16];
	char szCount[16];

	LoadString(hInstance, IDS_ENCRYPTPWD, szEnryptPWD, 4);
#ifndef __NEUZ_MSGVR
	LoadString(hInstance, IDS_MSGVER, g_szProtocolVersion, 32);
#endif
	LoadString(hInstance, IDS_CACHE_PORT, szCachePort, 16);
	LoadString(hInstance, IDS_SHOUTLIMIT_SECOND, szSecond, 16);
	LoadString(hInstance, IDS_SHOUTLIMIT_COUNT, szCount, 16);

	g_Neuz.m_dwShoutLimitSecond = atoi( szSecond );
	g_Neuz.m_nShoutLimitCount = atoi( szCount );

	g_Neuz.m_uCachePort = atoi( szCachePort );

#ifdef __NEUZ_MSGVR
	strcpy(g_szProtocolVersion, __NEUZ_MSGVR);
#endif

#ifdef __NEUZ_IPSET
	strcpy(g_Neuz.m_lpCertifierAddr, __NEUZ_IPSET);
#endif

	if( ::GetLanguage() == LANG_FRE )
	{
		g_Neuz.Key.chLeft	= 'Q';
		g_Neuz.Key.chUp	= 'Z';
		g_Neuz.Key.chTrace	= 'F';
		g_Neuz.Key.chQuest	= 'L';
		g_Neuz.Key.chWalk	= 'W';
	}
	else
	{
		g_Neuz.Key.chLeft	= 'A';
		g_Neuz.Key.chUp	= 'W';
		g_Neuz.Key.chTrace	= 'Z';
		g_Neuz.Key.chQuest	= 'Q';
		g_Neuz.Key.chWalk	= 'X';
	}
}


BOOL ParseCmdLine( LPCTSTR lpCmdLine )
{
	TCHAR szArg1[64] = {0, };
	TCHAR szArg2[64] = {0, };
	TCHAR szArg3[64] = {0, };
	TCHAR szArg4[64] = {0, };

	_stscanf( lpCmdLine, "%s %s %s %s", szArg1, szArg2, szArg3, szArg4 );

#ifndef __NEUZ_IPSET
	strcpy(g_Neuz.m_lpCertifierAddr, szArg1);
#endif

#ifdef __NEUZ_BOOTS 
	if (strcmpi(szArg1, __NEUZ_BOOTS))
		return FALSE;
#endif

	return TRUE;
}

void EnableFloatException()
{
#ifdef __AIO_NEUZAPP
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
	unsigned int dwCurrent = 0;
	::_controlfp_s(&dwCurrent, 0, 0);
	unsigned int dwNew = dwCurrent & ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL);
	::_controlfp_s(NULL, dwNew, MCW_EM);
#else
	int cw = _controlfp(0, 0);
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL);
	_controlfp(cw, MCW_EM);
#endif
#else
	int cw = _controlfp(0, 0);
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL);
	_controlfp(cw, MCW_EM);
#endif
}

BOOL InitApp()
{
	// Get Os Version
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL bGetOsVr = GetVersionEx( &versionInfo );

	// Get VGA Identifier
	LPDIRECT3D9 pD3D;
	D3DADAPTER_IDENTIFIER9 Identifier;
	pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	HRESULT hres = pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &Identifier );

	if( bGetOsVr && hres == S_OK && versionInfo.dwMajorVersion != 6 && Identifier.VendorId != 4098 )
		EnableFloatException();

	SAFE_RELEASE(pD3D);

	// Set up the highest resolution timer we can manage
	TIMECAPS tc;
	UINT uPeriod = (TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc))) ? tc.wPeriodMin : 1;
	timeBeginPeriod( uPeriod );

	xSRand( timeGetTime() );
	srand( timeGetTime() );

	InitUPS();

	if( InitializeNetLib() == FALSE )
		return FALSE;

#ifndef __AIO_NEUZAPP
	TestNetLib( g_Neuz.m_lpCertifierAddr, PN_CERTIFIER );
#endif

	return TRUE;
}

void SkipPatch()
{
	g_hMutex = OpenMutex( MUTEX_ALL_ACCESS, TRUE, "sdfghtryr1g" ); // __AIO_NEUZAPP neuz
	if( g_hMutex == NULL )
	{
		g_hMutex = CreateMutex( NULL, TRUE, "sdfghtryr1g" ); // __AIO_NEUZAPP neuz
		if( g_hMutex )
			ReleaseMutex( g_hMutex );
	}
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#ifdef __FL_FIX_LOADING
	if (g_Neuz.IsAlreadyRunning())
		return 0;

	g_Neuz.StartDelay(TRUE);
#endif

	g_Option.Init();		
	g_Neuz.LoadOption();	

	SkipPatch();
	InitGlobalVars( hInstance );

	if( ParseCmdLine( lpCmdLine ) == FALSE )
		return 0;

	InitApp();

	if( FAILED( g_Neuz.Create( hInstance ) ) )
		return 0;


	return g_Neuz.Run();
}