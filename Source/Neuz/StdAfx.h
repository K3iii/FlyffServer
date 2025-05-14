#pragma once

#pragma warning(disable:4786)

#define WIN32_LEAN_AND_MEAN
#define _USE_32BIT_TIME_T
#define POINTER_64 __ptr64
#define	__CLIENT
#define _WIN32_WINNT 0x0A00 // Windows 10

// Windows or Standard
#include <afxwin.h>
#include <AFXTEMPL.H>        // MFC Automation classes
#include <winuser.h>
#include <basetsd.h>
#include <memory.h>
#include <mmsystem.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <process.h>
#include <chrono>
#include <afxdisp.h>        // MFC Automation classes

// STL
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <string>
#include <queue>
using namespace std;


#define		__CLIENT

#define		ADDR_ACCUMULATOR	"211.33.142.133"

#include "NeuzVersion.h"
#include "VersionCommon.h"
#include "CustomCommon.h"
#include "DefineCommon.h"
#include "Resource.h"
#include "authorization.h"

// DirectX or 3D Core
#include <D3D9.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>

#include "memtrace.h"

#include "dxutil.h"
#include "d3denumeration.h"
#include "d3dsettings.h"
#include "d3dapp.h"
#include "vutil.h"
#include "d3dfont.h"
#include "d3dutil.h"
#include "DSUtil.h"

// Network
#include "CmnHdr.h"
#include "File.h"
#include "chassert.h"

// Resource
#include "misc.h"
#include "define.h"
#include "defineAttribute.h"
#include "defineItemKind.h"
#include "defineJob.h"
#include "defineWorld.h"
#include "WndStyle.h"

// MultiMedia
#include <atlbase.h>

// Common
#include "Data.h"
#include "Timer.h"
#include "Scanner.h"
#include "Script.h"
#include "2DRender.h"
#include "SkyBox.h"
#include "patch.h"
#include "Project.h"
#include "path.h"
#include "landscape.h"
#include "xUtil3D.h"
#include "Graphic3D.h"
#include "Debug.h"
#include "Music.h"
#include "HwOption.h"
#include "Messenger.h"
#include "yUtil.h"

#if __VER >= 15 // __BS_CHANGING_ENVIR
#include "TexturePool.h"	//gmpbigsun
#endif	//__BS_CHANGING_ENVIR

// Model
#include "Model.h"
#include "Billboard.h"
#include "Material.h"
#include "Bone.h"
#include "Object3D.h"
#include "ModelObject.h"
#include "ModelMng.h"
#include "Particle.h"

// object
#include "Obj.h"
#include "Ctrl.h"
#include "Item.h"
#include "sfxbase.h"
#include "Mover.h"
#include "Region.h"
#include "Weather.h"
#include "Camera.h"
#include "Light.h"
#include "World.h"
#include "WorldMng.h"

#include "CreateObj.h"
#include "authorization.h"

// Interface
#include "EditString.h"
#include "ToolTip.h"
#include "ITheme.h"
#include "WndBase.h"
#include "WndControl.h"
#include "wndEditCtrl.h"
#include "WndItemCtrl.h"
#include "WndTradeCtrl.h"
#include "WndNeuz.h"
#include "WndMessageBox.h"
#include "WndGuideSystem.h"
#include "WndTitle.h"
#include "WndTaskBar.h"
#include "WndField.h"
#include "WndCommunication.h"
#include "WndOption.h"
#include "WndOptionGame.h"
#include "WndDialog.h"
#include "WndShop.h"
#include "WndMessengerSearch.h"
#include "WndManager.h"
#include "WndWorld.h"
#include "WndGuild.h"
#include "WndRankGuild.h"
#include "WndRankInfo.h"
#include "WndRankWar.h"
#include "WndGuildVote.h"
#include "WndUpgradeBase.h"

#ifdef __YCLOTH
#include "cloth.h"
#endif

int (WINAPIV* __vsnprintf)(char*, size_t, const char*, va_list) = _vsnprintf;
int (WINAPIV* __vsnwprintf)(wchar_t*, size_t, const wchar_t*, va_list) = _vsnwprintf;

// Application
#include "DialogMsg.h"
#include "Neuz.h"
#include "tickcount.h"
#include "flyffevent.h"

// console
#ifdef __BS_CONSOLE
#include "Console.h"
#endif

#define D3DDEVICE   g_Neuz.m_pd3dDevice

extern BYTE             g_bKeyTable[256];
extern BOOL             g_bSlotSwitchAboutEquipItem[ MAX_SLOT_ITEM ];
#ifdef __XKEYEDGE
extern BYTE             g_bKeyTable2[256];
#endif //__XKEYEDGE
extern CNeuzApp         g_Neuz        ;
extern CToolTip         g_toolTip     ;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
extern CToolTip         g_toolTipSub1 ;
extern CToolTip         g_toolTipSub2 ;
#endif // __IMPROVE_SYSTEM_VER15
extern CMover*          g_pPlayer     ;
extern CWorldMng	    g_WorldMng    ;
extern CProject         prj;
extern CModelObject*    g_pBipedMesh;
extern CD3DApplication* g_pD3dApp;
extern CGameTimer       g_GameTimer;

struct CULLINFO
{
    D3DXVECTOR3 vecFrustum[8];    // corners of the view frustum
    D3DXPLANE planeFrustum[6];    // planes of the view frustum
};
extern CULLINFO     g_cullinfo;
extern	CTickCount	g_TickCount;
extern CFlyffEvent	g_eLocal;
extern ADDSMMODE	g_AddSMMode;
extern DWORD		g_tmCurrent;
extern int			g_nRenderCnt;
extern int			g_nProcessCnt;


#ifdef __BS_CONSOLE
extern DlgConsole g_Console;
#endif

#ifdef __CERTIFIER_COLLECTING_SYSTEM
extern vector< CString > g_vecEncryptedValidCertifierIP;
#endif // __CERTIFIER_COLLECTING_SYSTEM




////////////////////////////////////////////////////////////////////

#include "../_Common/Rijndael.h"

////////////////////////////////////////////////////////////////////

