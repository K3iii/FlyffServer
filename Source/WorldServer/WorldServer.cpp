#include "StdAfx.h"
#include "Resource.h"
#include "GlobalTime.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "WorldServer.h"
#include "ThreadMng.h"
#include "AIBear.h"
#include "AIBigMuscle.h"
#include "AIClockWorks.h"
#include "AIGuard.h"
#include "AIKrrr.h"
#include "AIPet.h"
#include "guild.h"
#include "guildwar.h"
#include "party.h"
#include "Chatting.h"
#ifdef __MINIDUMP
#include "MiniDumper.h"
#endif
#include "spevent.h"
#include "ItemScript.h"
#include "WorldDialog.h"
#include "WScript.h"

#ifdef __VM_0819
#include "vmlog.h"
#endif	// __VM_0819

#if __VER >= 13 // __COUPLE_1117
#include "couple.h"
#endif	// __COUPLE_1117
#if __VER >= 13 // __HOUSING
#include "housing.h"
#endif	// __HOUSING

#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER
#endif // __ITEMCREATEMON_S0602

#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD


#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				Script(LPCSTR lpszFileName);
void				ExitInstance(void);
void				SetTitle();
extern BOOL			LoadAIScript();

const int			MAX_LOADSTRING = 100;
static TCHAR		g_szTitle[MAX_LOADSTRING];
static TCHAR		g_szWindowClass[MAX_LOADSTRING];
static HWND			g_hMainWnd;
static char			g_szBuffer[LOGTYPE_MAX][256] = { 0, };
char				g_szCoreAddr[16];
/*
#ifdef __GIFTBOX0213
static char			s_szAccountAddr[16];
#endif
*/

static char  		g_szINI[] = "Script\\WorldServer.ini";


extern	CGuildMng			g_GuildMng;
extern	CChattingMng		g_ChattingMng;
extern	CPartyMng			g_PartyMng;
extern	char				g_szDBAddr[16];
extern	char				g_sHeartbeat[32];
extern	CDPSrvr				g_DPSrvr;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;
extern	CWorldMng			g_WorldMng;
extern CGuildCombat			g_GuildCombatMng;
#if __VER >= 12 // __ITEMCREATEMON_S0602
#if __VER < 12 // __NEW_ITEMCREATEMON_SERVER
extern	CCreateMonster		g_CreateMonster;
#endif // __NEW_ITEMCREATEMON_SERVER
#endif


#if defined(__LUASCRIPT060908)
#ifdef _DEBUG
#pragma comment(lib, "../lua/luaD.lib" )
#else
#pragma comment(lib, "../lua/lua.lib" )
#endif
#endif

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

extern void md5(char* out, const char* in);


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	LoadString(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WORLDSERVER, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	InitLanguageFromResource(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		ExitInstance();
		return FALSE;
	}

	MSG msg;
	// Main message loop:
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
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_WORLDSERVER);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_WORLDSERVER;
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

void SetLogInfo(LOGTYPE type, LPCTSTR lpszFormat, ...)
{
	if (type == LOGTYPE_REDRAW)
	{
		InvalidateRect(g_hMainWnd, NULL, TRUE);
		return;
	}

	va_list args;
	va_start(args, lpszFormat);

	_vsntprintf(g_szBuffer[type], 255, lpszFormat, args);

	va_end(args);
}


void OnPaint(HDC& hDC)
{
	int x, y;
	x = 0;
	y = 0;

	for (int i = 0; i < LOGTYPE_MAX; ++i)
	{
		TextOut(hDC, x, y, g_szBuffer[i], strlen(g_szBuffer[i]));
		y += 20;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_TIMER:
	{
		WORD wTimerID = wParam;
		switch (wTimerID)
		{
		case 0:
			g_dpDBClient.SendPing();
			g_DPCoreClient.SendPing();
			break;
		case 1:
			// g_UserMng.OutputStatistics();
			break;
		}
		break;
	}
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_MENU_OUTALL:
			g_UserMng.KickAllPlayers();
			break;
#ifdef __FL_UPDATE_READSTAFF
		case ID_MENU_RELOADSTAFF: prj.LoadServerStaff(); break;
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
		case ID_MENU_RELOADWOF: CWheelOfFortune::GetInstance()->LoadScript(TRUE); break;
#endif
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		CRunObject::GetInstance()->ToggleProfiling();
#ifdef __MEM_TRACE
#ifndef _DEBUG
		CMemTrace::Instance()->Print();
#endif	// _DEBUG
#endif	// __MEM_TRACE
#ifdef __VM_0819
		CVMLog::Instance()->Log();
#endif	// __VM_0819
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_DESTROY:
		KillTimer(g_hMainWnd, 0);
		KillTimer(g_hMainWnd, 1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void EnableFloatException()
{
#ifndef __AIO_NEUZAPP
	int cw = _controlfp(0, 0);
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL);
	_controlfp(cw, MCW_EM);
#else
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
#endif
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Set up the highest resolution timer we can manage
//	TIMECAPS tc;
//	UINT nPeriod;
//	if( TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc)) )
//		nPeriod = tc.wPeriodMin;
//	else
//		nPeriod = 1;
//	timeBeginPeriod( nPeriod );

	HWND hWnd;
	g_hMainWnd = hWnd = CreateWindow(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	int x = 480, y = 0;
	SetWindowPos(hWnd, NULL, x, y, 800, 416, SWP_SHOWWINDOW);

	if (!hWnd)
		return FALSE;

	SetTimer(g_hMainWnd, 0, SEC(60), NULL);	// 60
	SetTimer(g_hMainWnd, 1, MIN(60), NULL);	// 3600

	CTime time = CTime::GetCurrentTime();
	SetLogInfo(LOGTYPE_TIME, "%s", time.Format("%Y/%m/%d %H:%M:%S"));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef __MINIDUMP
	InitDumper(DUMP_LEVEL_2);
#endif

	EnableFloatException();

	if (InitializeNetLib() == FALSE)
		return FALSE;

	if (InitializeScriptLib() == FALSE)
		return FALSE;

	xSRand(timeGetTime());

	if (Script(g_szINI) == TRUE)
	{
		if (!prj.OpenProject("Masquerade.prj"))
		{
			Error("OpenProject ERROR check Masquerade.prj");
			return FALSE;
		}

		SetLogInfo(LOGTYPE_REDRAW, "");
		SetTitle();

		if (CompileItemScript("ItemScript.lua") == FALSE)
			return FALSE;

#if defined(__REMOVE_SCIRPT_060712)
		if (CWorldDialog::GetInstance().Init() == FALSE)
		{
			OutputDebugString("WorldDialog.DLL load failed\n");
			return FALSE;
		}
#endif
		return CRunObject::GetInstance()->Init();
	}
	else
	{
		char szBuffer[300] = { 0, };
		char szDir[255] = { 0, };
		GetCurrentDirectory(255, szDir);
		sprintf(szBuffer, "%s\\WorldServer.ini File not found", szDir);
		AfxMessageBox(szBuffer);
		return FALSE;
	}
}

void ExitInstance(void)
{
	CRunObject::GetInstance()->Close();
	/*
#ifdef __GIFTBOX0213
	CDPAccountClient::GetInstance()->DeleteDPObject();
#endif	// __GIFTBOX0213
	*/

	g_DPSrvr.DeleteDPObject();
	g_DPCoreClient.DeleteDPObject();
	g_dpDBClient.DeleteDPObject();

	g_WorldMng.Free();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CUser::m_pPool);
	SAFE_DELETE(CMover::m_pPool);
	SAFE_DELETE(CActionMover::m_pPool);
	SAFE_DELETE(CAIMonster::m_pPool);
	SAFE_DELETE(CItem::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820

	g_PartyMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CParty::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_ChattingMng.Clear();
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CChatting::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820

	g_GuildMng.Clear();

#if __VER >= 13 // __HOUSING
	CHousingMng::GetInstance()->Clear();
#endif	// __HOUSING
#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CGuildMember::sm_pPool);
	SAFE_DELETE(CGuild::sm_pPool);
	SAFE_DELETE(CAIPet::m_pPool);
	SAFE_DELETE(CAIKrrr::m_pPool);
	SAFE_DELETE(CAIGuard::m_pPool);
	SAFE_DELETE(CAIClockWorks::m_pPool);
	SAFE_DELETE(CAIBigMuscle::m_pPool);
	SAFE_DELETE(CAIBear::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	UninitializeNetLib();
	UninitializeScriptLib();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	SAFE_DELETE(CAIPet::m_pPool);
#endif	// __MEM_TRACE
#endif	// __VM_0820
	g_Object3DMng.Destroy();
	prj.m_modelMng.DeleteDeviceObjects();

#if __VER >= 12 // __LORD
	CSLord::Instance()->DestroyColleagues();
#endif	// __LORD
}


void SetTitle()
{
	char szLang[MAX_PATH] = { 0, };
	sprintf(szLang, CLangMan::GetInstance()->GetLangData(::GetLanguage())->szTitle);

	char szTitle[256];
	sprintf(szTitle, "%s(%d)-%s", g_szTitle, g_uKey, szLang);
	SetWindowText(g_hMainWnd, szTitle);
}

BOOL Script(LPCSTR lpszFileName)
{
	g_eLocal.SetState(EVE_RESPAWN, 1);

	CScanner s;
	if (s.Load(lpszFileName) == FALSE)
	{
		Error("Fail to open file - %s", lpszFileName);
		return FALSE;
	}

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "ResourceFolder")
		{
			s.GetToken();
			if (!SetCurrentDirectory(s.Token))
			{
				Error("Folder not found, %s", s.Token);
				return FALSE;
			}
		}
		else if (s.Token == "Key")
		{
			g_uKey = (DWORD)s.GetNumber();
#ifdef __S8_SERVER_PORT
			g_uIdofMulti = g_uKey % 100;
#else // __S8_SERVER_PORT
			g_uIdofMulti = g_uKey / 100;
#endif // __S8_SERVER_PORT
		}
		else if (s.Token == "Core")
		{
			s.GetToken();
			strcpy(g_szCoreAddr, s.Token);
		}
		else if (s.Token == "DB")
		{
			s.GetToken();
			strcpy(g_szDBAddr, s.Token);
		}
		else if (s.Token == "Account")
		{
			s.GetToken();
		}
		else if (s.Token == "Heartbeat")
		{
			s.GetToken();
			strcpy(g_sHeartbeat, s.Token);
		}
		else if (s.Token == "STEAL")
		{
			g_eLocal.SetState(EVE_STEAL, 1);
		}
		else if (s.Token == "GUILDBANK")
		{
			g_eLocal.SetState(ENABLE_GUILD_INVENTORY, 1);
		}
		else if (s.Token == "DROPITEM_REMOVE")
		{
			g_eLocal.SetState(EVE_DROPITEMREMOVE, 1);
		}
		else if (s.Token == "GUILDCOMBAT")
		{
			g_eLocal.SetState(EVE_GUILDCOMBAT, 1);
		}
		else if (s.Token == "WORMON")
		{
			g_eLocal.SetState(EVE_WORMON, 1);
		}
		else if (s.Token == "respawn")
		{
			g_eLocal.SetState(EVE_RESPAWN, (BYTE)s.GetNumber());
		}
		else if (s.Token == "ARENA")
		{
			g_eLocal.SetState(EVE_ARENA, 1);
		}
		else if (s.Token == "SECRETROOM")
		{
			g_eLocal.SetState(EVE_SECRETROOM, 1);
		}
		else if (s.Token == "RANGDA")
		{
			g_eLocal.SetState(EVE_RANGDA, 1);
		}

		s.GetToken();
		}
	return TRUE;
	}
