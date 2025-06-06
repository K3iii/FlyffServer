// Certifiier.cpp : Defines the entry point for the application.
#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "mytrace.h"
#include "dbmanager.h"
extern	CDbManager	g_DbManager;

#include "user.h"

extern	CMyTrace	g_MyTrace;

#define MAX_LOADSTRING 100

//////////////////////////////////////////////////////////////////////////
class Heartbeat
{
public:
	Heartbeat( const char* name );
	virtual	~Heartbeat();
	void	Init();
	void	Run();
private:
	HANDLE	_heartbeat;
	const	string	_name;
};
Heartbeat hb( "10" );
//////////////////////////////////////////////////////////////////////////

// Global Variables:
HINSTANCE hInst;	// current instance
TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text
HWND	hMainWnd;
char	lpConnection[16];
extern	CDPCertifier	g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;

ATOM	MyRegisterClass(HINSTANCE hInstance);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance( void );
BOOL	Script( LPCTSTR lpszFileName );
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

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


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	InitLanguageFromResource( hInstance );

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CERTIFIIER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		ExitInstance();
		return FALSE;
	}

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CERTIFIIER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CERTIFIIER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst	= hInstance;	

	hMainWnd	= hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	g_fInternal	= TRUE;

	int x = 800, y = 416;
	SetWindowPos( hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW );

	g_MyTrace.Initialize( hWnd, "SquireD", RGB( 0x00, 0x00, 0x00 ), RGB( 0xff, 0xff, 0xff ) );

	CTime time	= CTime::GetCurrentTime();
	g_MyTrace.Add( 0, FALSE, "%s", time.Format( "%Y/%m/%d %H:%M:%S" ) );
	g_MyTrace.AddLine( '\0' );

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (Script("Script\\Certifier.ini") == FALSE)
		return FALSE;

	if( g_DbManager.CreateDbWorkers() == FALSE )
	{
		AfxMessageBox( "g_DbManager.CreateDbWorkers odbc connect timeout." );
		return FALSE;
	}

	if( InitializeNetLib() == FALSE )
		return FALSE;

#ifdef __CRC
	if( g_dpCertifier.StartServer( PN_CERTIFIER, FALSE, CSock::crcRead ) == FALSE )
#else	// __CRC
	if( g_dpCertifier.StartServer( PN_CERTIFIER ) == FALSE )
#endif	// __CRC
		return FALSE;
	if( FALSE == g_dpAccountClient.ConnectToServer( lpConnection, PN_ACCOUNTSRVR_0 ) )
		return FALSE;

	SetTimer( hWnd, 0, SEC( 10 ), NULL );

	return TRUE;
}

void ExitInstance( void )
{
	g_dpCertifier.DeleteDPObject();
	g_dpAccountClient.DeleteDPObject();

	UninitializeNetLib();
}

BOOL Script( LPCTSTR lpszFileName )
{
#ifdef __INTERNALSERVER
	strcpy( g_DbManager.m_szLoginPWD, "#^#^account" );
#endif

	CScanner s;

	if( s.Load( lpszFileName ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			if( s.Token == ";" )
			{
				s.GetToken();
				continue;
			}
			else if( s.Token == "Account" )
			{
				s.GetToken();
				strcpy( lpConnection, s.Token );
			}
			else if( s.Token == "LIMIT" )
			{
				g_dpCertifier.SetLimitation( s.GetNumber() );
			}
			else if( s.Token == "DB_PWD_LOGIN" )
			{
				s.GetToken();
				::GetPWDFromToken( s.Token, g_DbManager.m_szLoginPWD );
				TRACE("%s\n", g_DbManager.m_szLoginPWD );
			}
			else if( s.Token == "HEARTBEAT" )
			{
				hb.Init();
			}
			s.GetToken();
		}
		return TRUE;
	}

	Error( "Can't open file, %s.\n", lpszFileName );
	return FALSE;
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
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_TIMER:
			{
				WORD wTimerID	= wParam;
				switch( wTimerID )
				{
					case 0:
						{
							CUserMng::GetInstance()->ClearDum( (CDPMng*)&g_dpCertifier );
							hb.Run();
							break;
						}
				}
				break;
			}
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_MyTrace.Paint( hdc );
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			KillTimer( hMainWnd, 0 );
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////
Heartbeat::Heartbeat( const char* name )
:
_heartbeat( 0 ),
_name( name )
{
}

Heartbeat::~Heartbeat()
{
	CLOSE_HANDLE( _heartbeat );
}

void Heartbeat::Init()
{
	if( !( _heartbeat	= ::CreateEvent( NULL, FALSE, FALSE, _name.c_str() ) ) )
		_heartbeat		= ::OpenEvent( EVENT_MODIFY_STATE, FALSE, _name.c_str() );
	if( !_heartbeat )
		Error( "Heartbeat.Init" );
}

void Heartbeat::Run()
{
	if( _heartbeat )
		::SetEvent( _heartbeat );
}

////////////////////////////////////////////////////////////////////////////////