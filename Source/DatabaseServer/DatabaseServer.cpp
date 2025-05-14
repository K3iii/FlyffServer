// DatabaseServer.cpp : Defines the entry point for the application.


#include "stdafx.h"
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "resource.h"
#include "dploginsrvr.h"
#include "dptrans.h"
#include "dpaccountclient.h"
#include "MyTrace.h"
#include "..\_Network\Objects\Obj.h"

#include "xutil.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "post.h"
#include <mmsystem.h>
#include "dbcontroller.h"

#ifdef __MINIDUMP
#include "../WorldServer/MiniDumper.h"
#endif

extern	CDPLoginSrvr	g_dpLoginSrvr;
extern	CDPAccountClient	g_dpAccountClient;
extern	CMyTrace	g_MyTrace;
extern	CProject	prj;

#include "DPCoreSrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;

#include "spevent.h"

#include "tlord.h"

#define MAX_LOADSTRING 100

extern	APP_INFO	g_appInfo;
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
char	lpConnection[16];

#include "DbController.h"

#include "Tax.h"
#include "GuildHouseDBCtrl.h"

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text

// Foward declarations of functions included in this code module:
ATOM	MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	InitInstance(HINSTANCE, int);
void	ExitInstance(void);
BOOL	Script(LPCSTR lpszFileName);
BOOL	LoadLastItem(LPCSTR lpszFileName);
BOOL	LoadRemoveItem(LPCSTR lpszFileName);

#define	TID_MEM_RELEASE		0
#define TID_RELOAD_PROJECT  1
#define	TID_POSTPROC	2
#define	TID_QUERY_REMOVE_GUILD_BANK_TBL	3
#define	TIMER_ID_EVENT_GENERIC	4
#ifdef __FL_RECORD_BOOK
#define	TIMER_UPDATE_RECORD_BOOK	5
#endif
static char g_szINI[] = "Script\\databaseserver.ini";


void InitLanguageFromResource(HINSTANCE hInstance)
{
	char szLang[16], szSubLang[16];
	LoadString(hInstance, IDS_LANG, szLang, 16);
	LoadString(hInstance, IDS_SUBLANG, szSubLang, 16);

	char szCodePage[16];
	LoadString(hInstance, IDS_CODEPAGE, szCodePage, 16);
	g_codePage = atoi(szCodePage);

	::SetLanguageInfo(atoi(szLang), atoi(szSubLang));
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	InitLanguageFromResource(hInstance);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DATABASESERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_DATABASESERVER);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_MENUTEXT;
	wcex.lpszMenuName = (LPCSTR)IDC_DATABASESERVER;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

#ifdef __FL_RECORD_BOOK // FIX_05.06.2024
HANDLE g_hDbMutex = NULL;

bool StartRunning()
{
	CString str;
	str.Format("flyff_db_mutex_jfausetlg1z6_%d", g_appInfo.dwSys);

	g_hDbMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, str);
	if (g_hDbMutex == NULL)
	{
		g_hDbMutex = CreateMutex(NULL, TRUE, str);
		if (g_hDbMutex)
		{
			ReleaseMutex(g_hDbMutex);
			return true;
		}
	}

	return false;
}
#endif

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hMainWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;


	xSRand(timeGetTime());

	if (InitializeNetLib() == FALSE)
		return FALSE;

	if (Script(g_szINI) == FALSE)
	{
		char szMsg[256];
		sprintf(szMsg, "FAIL read ini - %s", g_szINI);
		AfxMessageBox(szMsg, MB_OK);
		return FALSE;
	}

#ifdef __FL_RECORD_BOOK // FIX_05.06.2024
	if (!StartRunning())
	{
		char szMsg[256];
		sprintf(szMsg, "DatabaseServer (%d) already running", g_appInfo.dwSys);
		AfxMessageBox(szMsg, MB_OK);
		return FALSE;
	}
#endif

	if (CQuery::EnableConnectionPooling())
		OutputDebugString("EnableConnectionPooling\n");
	g_DbManager.CreateDbWorkers();

#ifdef __INTERNALSERVER
	SetTimer(hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN(1), NULL);
#else	// __INTERNALSERVER
	SetTimer(hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL, MIN(10), NULL);
#endif	// __INTERNALSERVER

	if (!g_DbManager.LoadPlayerData())
		return FALSE;

#ifdef __FL_CASH_SHOP
	if (!g_DbManager.LoadCShop())
		return FALSE;
#endif

	if (!g_DbManager.GetPartyName())
	{
		ASSERT(0);
	}

	if (!g_DbManager.LoadPost())
	{
		ASSERT(0);
	}
	SetTimer(hWnd, TID_POSTPROC, MIN(1), NULL);
	SetTimer(hWnd, TIMER_ID_EVENT_GENERIC, MIN(1), NULL);

	if (!g_DbManager.OpenGuildCombat())
	{
		ASSERT(0);
	}

	CTLord::Instance()->CreateColleagues();
	CDbControllerTimer::GetInstance()->Create();
	GuildHouseDBMng->PostRequest(GUILDHOUSE_LOAD);
#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->LoadRecordBook();
	SetTimer(hWnd, TIMER_UPDATE_RECORD_BOOK, MIN(15), NULL);
#endif

	if (!g_dpLoginSrvr.StartServer(PN_DBSRVR_0))
	{
		return FALSE;
	}

	if (!CDPTrans::GetInstance()->StartServer(PN_DBSRVR_1))
	{
		return FALSE;
	}

	if (!g_dpCoreSrvr.StartServer(PN_DBSRVR_2))
	{
		return FALSE;
	}

	if (FALSE == g_dpAccountClient.ConnectToServer(lpConnection, PN_ACCOUNTSRVR_1))
		return FALSE;
	g_dpAccountClient.SendAddIdofServer(g_appInfo.dwSys);

	NotifyProcessStart(g_appInfo.dwId);

	int x = 0, y = 0;
	SetWindowPos(hWnd, NULL, x, y, 400, 416, SWP_SHOWWINDOW);

	g_MyTrace.Initialize(hWnd, "SquireD", RGB(0xff, 0xff, 0xff), RGB(0x00, 0x00, 0x00));

	CTime time = CTime::GetCurrentTime();
	g_MyTrace.Add(0, FALSE, "%s", time.Format("%Y/%m/%d %H:%M:%S"));
	g_MyTrace.AddLine('\0');

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef __MINIDUMP
	InitDumper(DUMP_LEVEL_2);
#endif

	return TRUE;
}

void ExitInstance(void)
{
	g_dpLoginSrvr.DeleteDPObject();
	CDPTrans::GetInstance()->DeleteDPObject();
	g_dpAccountClient.DeleteDPObject();
	g_dpCoreSrvr.DeleteDPObject();
	g_DbManager.Clear();
	g_GuildMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CGuildMember::sm_pPool);
	SAFE_DELETE(CGuild::sm_pPool);
	SAFE_DELETE(CMover::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820

	CDbControllerTimer::GetInstance()->Destroy();
	CTLord::Instance()->DestroyColleagues();

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
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_EVENTLUA_APPLYNOW:
		{
			prj.m_EventLua.m_Access.Enter();
			prj.m_EventLua.LoadScript();
			CDPTrans::GetInstance()->SendEventLuaChanged();
			if (prj.m_EventLua.CheckEventState())
				CDPTrans::GetInstance()->SendEventLuaState(prj.m_EventLua.m_mapState, FALSE);
			prj.m_EventLua.m_Access.Leave();
		}
		break;
#ifdef __FL_CASH_SHOP
		case IDM_LOAD_CSHOP:
		{
			g_DbManager.LoadCShop();

			LPDB_OVERLAPPED_PLUS pov2 = g_DbManager.AllocRequest();
			pov2->nQueryMode = CSHOP_LOAD;
			PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &pov2->Overlapped);
		}
		break;
#endif
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
	{
		WORD wTimerID = wParam;
		switch (wTimerID)
		{
		case TID_POSTPROC:
			CPost::GetInstance()->Process();
			break;
		case TID_QUERY_REMOVE_GUILD_BANK_TBL:
			g_DbManager.QueryRemoveGuildBankTbl();
			break;
#ifdef __FL_RECORD_BOOK
		case TIMER_UPDATE_RECORD_BOOK:
		{
			CRecordBook::GetInstance()->SendRecordBook(DPID_ALLPLAYERS);
			CRecordBook::GetInstance()->SaveRecordBook();
			break;
		}
#endif
		case TIMER_ID_EVENT_GENERIC:
		{
#if __VER >= 9 // __EVENTLUA
			prj.m_EventLua.m_Access.Enter();
#ifdef __AUTO_NOTICE
			if (prj.m_EventLua.IsNoticeTime())
				g_dpCoreSrvr.SendEventLuaNotice();
#endif // __AUTO_NOTICE
			if (prj.m_EventLua.CheckEventState())
				CDPTrans::GetInstance()->SendEventLuaState(prj.m_EventLua.m_mapState, TRUE);
			prj.m_EventLua.m_Access.Leave();
#endif // __EVENTLUA

#ifdef __GETMAILREALTIME
			LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
			lpDbOverlappedPlus->nQueryMode = QM_GETMAIL_REALTIME;
			PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
#endif // __GETMAILREALTIME
		}
		break;

		default:
			break;
		}
		break;
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		g_MyTrace.Paint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, TID_POSTPROC);
		KillTimer(hWnd, TID_QUERY_REMOVE_GUILD_BANK_TBL);
		KillTimer(hWnd, TIMER_ID_EVENT_GENERIC);
#ifdef __FL_RECORD_BOOK
		KillTimer(hWnd, TIMER_UPDATE_RECORD_BOOK);
#endif
		PostQuitMessage(0);
		break;
#ifdef __FL_RECORD_BOOK
	case WM_CLOSE:
	{
		CRecordBook::GetInstance()->SaveRecordBook();
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
#endif

	case WM_RBUTTONDOWN:
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


BOOL Script(LPCSTR lpszFileName)
{
	CDbManager& db = CDbManager::GetInstance();
#ifdef __INTERNALSERVER
	strcpy(db.DB_ADMIN_PASS_LOG, "#^#^log");
	strcpy(db.DB_ADMIN_PASS_CHARACTER01, "#^#^character");
	strcpy(db.DB_ADMIN_PASS_BACKSYSTEM, "!@!@backend");
#endif
	CScanner s;

	if (s.Load(lpszFileName))
	{
		s.GetToken();
		while (s.tok != FINISHED)
		{
			if (s.Token == "ResourceFolder")
			{
				s.GetToken();
				if (!SetCurrentDirectory(s.Token))
				{
					g_MyTrace.Add(CMyTrace::Key("status"), FALSE, "Folder not found, %s", s.Token);
					return FALSE;
				}
			}
			else if (s.Token == "Sys")
			{
				g_appInfo.dwSys = s.GetNumber();
			}
			else if (s.Token == "dwId")
			{
				g_appInfo.dwId = s.GetNumber();
			}
			else if (s.Token == "Account")
			{
				s.GetToken();
				strcpy(lpConnection, s.Token);
			}
			else if (s.Token == "DSN_NAME_CHARACTER")
			{
				s.GetToken();
				strcpy(DSN_NAME_CHARACTER01, s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_CHARACTER")
			{
				s.GetToken();
				strcpy(DB_ADMIN_ID_CHARACTER01, s.Token);
			}
			else if (s.Token == "DSN_NAME_LOG")
			{
				s.GetToken();
				strcpy(DSN_NAME_LOG, s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_LOG")
			{
				s.GetToken();
				strcpy(DB_ADMIN_ID_LOG, s.Token);
			}
#ifdef __S1108_BACK_END_SYSTEM
			else if (s.Token == "DSN_NAME_BACKSYSTEM")
			{
				s.GetToken();
				strcpy(DSN_NAME_BACKSYSTEM, s.Token);
			}
			else if (s.Token == "DB_ADMIN_ID_BACKSYSTEM")
			{
				s.GetToken();
				strcpy(DB_ADMIN_ID_BACKSYSTEM, s.Token);
			}
			else if (s.Token == "BACKENDSYSTEM")
			{
				prj.m_bBackEndSystem = TRUE;
			}
#endif // __S1108_BACK_END_SYSTEM
			else if (s.Token == "DB_PWD_LOG")
			{
				s.GetToken();
				::GetPWDFromToken(s.Token, db.DB_ADMIN_PASS_LOG);
				TRACE("%s\n", db.DB_ADMIN_PASS_LOG);
			}
			else if (s.Token == "DB_PWD_CHARACTER")
			{
				s.GetToken();
				::GetPWDFromToken(s.Token, db.DB_ADMIN_PASS_CHARACTER01);
				TRACE("%s\n", db.DB_ADMIN_PASS_CHARACTER01);
			}
			else if (s.Token == "DB_PWD_BACKEND")
			{
				s.GetToken();
				::GetPWDFromToken(s.Token, db.DB_ADMIN_PASS_BACKSYSTEM);
				TRACE("%s\n", db.DB_ADMIN_PASS_BACKSYSTEM);
			}
			s.GetToken();
		}

		if (!prj.OpenProject("masquerade.prj"))
		{
			g_MyTrace.Add(CMyTrace::Key("status"), FALSE, "Fail to open project");
			return FALSE;
		}
		else
		{
			g_MyTrace.Add(CMyTrace::Key("status"), FALSE, "Ready");
			return TRUE;
		}
	}

	g_MyTrace.Add(CMyTrace::Key("status"), FALSE, "Fail to open file, %s", lpszFileName);
	return FALSE;
};