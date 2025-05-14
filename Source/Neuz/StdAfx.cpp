// stdafx.cpp : source file that includes just the standard includes
//	Neuz.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "Ship.h"

CProject         prj;
CWorldMng  	     g_WorldMng;
CToolTip         g_toolTip;
CToolTip         g_toolTipSub1;
CToolTip         g_toolTipSub2;
CULLINFO         g_cullinfo;
CMover*          g_pPlayer;
BYTE             g_bKeyTable[256];
BOOL             g_bSlotSwitchAboutEquipItem[ MAX_SLOT_ITEM ];
CModelObject*    g_pBipedMesh;
CD3DApplication* g_pD3dApp;
CGameTimer       g_GameTimer;
CTickCount       g_TickCount;
CFlyffEvent      g_eLocal;
ADDSMMODE        g_AddSMMode;
DWORD	         g_tmCurrent = 0;
time_t		g_tCurrent;		//	time( NULL )
int		         g_nRenderCnt = 0;
int		         g_nProcessCnt = 0;

#ifdef __BS_CONSOLE
DlgConsole g_Console;
#endif

#ifdef __CERTIFIER_COLLECTING_SYSTEM
vector< CString > g_vecEncryptedValidCertifierIP;
#endif // __CERTIFIER_COLLECTING_SYSTEM

CNeuzApp         g_Neuz;

