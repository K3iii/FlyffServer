// CoreServer.cpp : Defines the entry point for the application.


#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dpcoresrvr.h"
#include "dpcachesrvr.h"
#include "dploginsrvr.h"
#include "msghdr.h"
#include "MyTrace.h"

#include "xutil.h"
#include "dpdatabaseclient.h"
extern	CDPDatabaseClient g_dpDatabaseClient;

#include "lang.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

extern	CDPCoreSrvr		g_dpCoreSrvr;
extern	CDPCacheSrvr	g_DPCacheSrvr;
extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CMyTrace	g_MyTrace;

#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "Environment.h"

#include "player.h"
extern	CPlayerMng	g_PlayerMng;

#include "flyffevent.h"

///////////////////////////////////////////////////////
#define			MAX_LOADSTRING	100
const WORD		TIMER_ID_DEFAULT = 100;

///////////////////////////////////////////////////////
HINSTANCE	hInst;
HWND		hMainWnd;
TCHAR		szTitle[MAX_LOADSTRING];
TCHAR		szWindowClass[MAX_LOADSTRING];
DWORD		g_dwSys;
DWORD		g_dwId;
CHAR		szCertifierAddr[16];
CHAR		szDataBaseAddr[16];


ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitInstance(HINSTANCE, int);
void				ExitInstance( void );
BOOL				Script( LPCSTR lpszFileName );
void				OnTimer( WORD wTimerID );

void InitLanguageFromResource( HINSTANCE hInstance )
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);

	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);
	g_codePage = atoi( szCodePage );

	::SetLanguageInfo( atoi( szLang ), atoi( szSubLang ) );
}

///////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	InitLanguageFromResource( hInstance );

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CORESERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitInstance();

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CORESERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground	= (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName	= (LPCSTR)IDC_CORESERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

	hMainWnd	= hWnd	=
		CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

	int x = 0, y = 416;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_GameTimer.Compute();
	SetTimer( hWnd, TIMER_ID_DEFAULT, 1000, NULL );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0xff, 0xff, 0xff ), RGB( 0x00, 0x00, 0x00 ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

#ifndef _DEBUG
//	InitEH();
#endif	// _DEBUG

	g_TickCount.Start();

	if (!Script("Script\\CoreServer.ini"))
	{
		return FALSE;
	}

	if( InitializeNetLib() == FALSE )
		return FALSE;

	if( !g_dpCoreSrvr.StartServer( PN_CORESRVR + 0 ) )
		return FALSE;

	if( !g_DPCacheSrvr.StartServer( PN_CORESRVR + 1 ) )
		return FALSE;

	if( !g_dpLoginSrvr.StartServer( PN_CORESRVR + 2 ) )
		return FALSE;

	if( !g_dpDatabaseClient.ConnectToServer( szDataBaseAddr, PN_DBSRVR_2, FALSE ) )
	{
		Error( "Not TransServer Connect" );
		return FALSE;
	}

	if( !g_PartyMng.CreateWorkers() )
	{
		assert( 0 );
	}

	if( CEnvironment::GetInstance()->CreateWorkers() == FALSE )
	{
		assert( 0 );
	}

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ExitInstance( void )
{
	g_dpCoreSrvr.DeleteDPObject();
	g_DPCacheSrvr.DeleteDPObject();
	g_dpLoginSrvr.DeleteDPObject();
	g_dpDatabaseClient.DeleteDPObject();

	g_PartyMng.Clear();

	g_PlayerMng.Free();

	SAFE_DELETE( CParty::m_pPool );
	SAFE_DELETE( CPlayer::m_pPool );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );

	g_GuildMng.Clear();
	SAFE_DELETE( CGuildMember::sm_pPool );
	SAFE_DELETE( CGuild::sm_pPool );

	UninitializeNetLib();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				OnTimer( wParam ); // wTimerID = wParam
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
//			RECT rt;
//			GetClientRect(hWnd, &rt);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, TIMER_ID_DEFAULT );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void OnTimer( WORD wTimerID )
{
	if( wTimerID == TIMER_ID_DEFAULT )
	{
		g_GameTimer.Compute();
		return;
	}
}

BOOL Script( LPCSTR lpszFileName )
{
	CScanner s;
	CServerDesc* pServer;
	POINT topLeft;
	SIZE	size;

	if( s.Load( lpszFileName ) )
	{
		g_MyTrace.AddLine( '-' );

		s.GetToken();

		while( s.tok != FINISHED )
		{
			if( s.Token == "Sys" )
			{
				g_dwSys	= s.GetNumber();
			}
			else if( s.Token ==  "dwId" )
			{
				g_dwId = s.GetNumber();
			}
			else if( s.Token == "Database" )
			{
				s.GetToken();
				strcpy( szDataBaseAddr, s.Token );
			}
			else if( s.Token == "Certifier" )
			{
				s.GetToken();
				strcpy( szCertifierAddr, s.Token );
			}
			else if( s.Token == "PartyExpRate" )
			{
				s_fPartyExpRate		= s.GetFloat();
			}
			else
			{
				pServer		= new CServerDesc;
				u_long uKey	= (u_long)_ttoi( s.Token );
				pServer->SetKey( uKey );

				if( s.GetToken() == DELIMITER )
				{
					while( s.GetToken() != DELIMITER )
					{
						CJurisdiction* pJurisdiction	= new CJurisdiction;
						pJurisdiction->m_dwWorldID	= (DWORD)_ttoi( s.Token );
						topLeft.x	= s.GetNumber();	topLeft.y	= s.GetNumber();
						size.cx		= s.GetNumber();	size.cy		= s.GetNumber();
						pJurisdiction->m_rect.SetRect( topLeft.x, topLeft.y, topLeft.x + size.cx, topLeft.y + size.cy );
						pJurisdiction->m_wLeft	= s.GetNumber();	pJurisdiction->m_wRight		= s.GetNumber();
						pServer->m_lspJurisdiction.push_back( pJurisdiction );
					}
				}
		#ifdef __STL_0402
				bool bResult	= g_dpCoreSrvr.m_apSleepServer.insert( CServerDescArray::value_type( pServer->GetKey(), pServer ) ).second;
				ASSERT( bResult );
		#else	// __STL_0402
				g_dpCoreSrvr.m_apSleepServer.SetAt( pServer->GetKey(), pServer );
		#endif	// __STL_0402
				g_MyTrace.Add( pServer->GetKey(), TRUE, "%04d", pServer->GetKey() );
			}
			s.GetToken();
		}
		g_MyTrace.AddLine( '-' );

		return TRUE;
	}

	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
}