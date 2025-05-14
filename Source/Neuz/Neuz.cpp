#include "stdafx.h"

#include "AppDefine.h"
#include "Material.h"
#include "DialogMsg.h"
#include "party.h"
#include "../_Common/ModelObject.h"
#include "../_Common/HwOption.h"
#include "../_Common/ParticleMng.h"
#include "TailEffectMng.h"
#include "Sfx.h"
#include "definetext.h"
#include "DPClient.h"
#include "DPLoginClient.h"
#include "dpcertified.h"
#include "guild.h"
#include "guildwar.h"
#include "resource.h"
#include "Chatting.h"
#include "webbox.h"
#include "network.h"
#include "defineSound.h"
#include "ClientMsg.h"
#include "Environment.h"

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __MODEL_CACHE
#include "BetterCache.h"
#endif

#ifdef __FL_FIX_LOADING
HANDLE g_hMutexDelay = NULL;
#endif

#include "WndBase.h"
#include "WorldMap.h"
#include "tools.h"
#include "GuildHouse.h"

#ifdef __DISCORD
#include "DiscordFlyff.h"
DiscordFlyff* g_DiscordNeuz;
#endif

extern	CGuildMng		g_GuildMng;
extern	CDPClient		g_DPlay;
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPCertified	g_dpCertified;
extern CSfxTexture      g_SfxTex;
extern CSfxMng          g_SfxMng;

extern CSfxMeshMng      g_SfxMeshMng;
extern HANDLE			g_hMutex;

CDamageNumMng			g_DamageNumMng;
CModelObject			g_Shadow;

enum
{
	NOTIFY_STATUS_NONE,
	NOTIFY_STATUS_45MIN,
	NOTIFY_STATUS_50MIN,
	NOTIFY_STATUS_55MIN,
	NOTIFY_STATUS_56MIN,
	NOTIFY_STATUS_57MIN,
	NOTIFY_STATUS_58MIN,
	NOTIFY_STATUS_59MIN,
};

void SetMouseCursor(DWORD dwCursor)
{
	SetCursor(g_Neuz.m_ahCursor[dwCursor]);
}

CNeuzApp::CNeuzApp()
{
#ifdef __FL_CASH_SHOP
	m_nDonatePoints = 0;
	m_nVotePoints = 0;
#endif

	m_nRealWeather = WEATHER_NONE;

	m_dwTimeOutDis = 0xffffffff;

	m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	m_d3dEnumeration.AppMinDepthBits = 16;
	m_d3dEnumeration.AppUsesMixedVP = TRUE;

	m_dwCreationWidth = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics(SM_CYMENU);

#ifndef __NEUZ_TITLE
	m_strWindowTitle = _T("FLYFF");
#else
	m_strWindowTitle = _T(__NEUZ_TITLE);
#endif

	g_pD3dApp = this;

	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bFrameSkip = TRUE;
	m_dwTempMessage = 0;

	m_lpCertifierAddr[0] = '\0';
	memset(m_lpCacheAddr, 0, sizeof(TCHAR) * 16);
	m_uCachePort = PN_CACHESRVR;

	m_dwShoutLimitSecond = 0;
	m_nShoutLimitCount = 0;

	m_szAccount[0] = 0;
	m_szPassword[0] = 0; // password

	m_szCaptureFileName[0] = 0;
	m_bCapture = FALSE;
	m_nCaptureCnt = 0;
	g_pBipedMesh = NULL;
	m_dwSys = 0;
	m_bQuit = FALSE;

	m_uIdofMulti = NULL_ID;
	m_b18Server = FALSE;
	m_dwAuthKey = 0;
	m_cbAccountFlag = 0X00;

	m_nLeftTimeNotifyStatus = NOTIFY_STATUS_NONE;

	m_bActiveNeuz = TRUE;

	m_dwSummonPartyData = 0;
	m_dwSummonPartyObjid = 0;
	ZeroMemory(m_szSummonPartyWorldName, sizeof(m_szSummonPartyWorldName));

	m_vCursorPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pCreateMonItem = NULL;

	m_idPlayer = 0;
	m_hThread = INVALID_HANDLE_VALUE;
	memset(m_apPlayer, 0, sizeof(m_apPlayer));
	memset(m_aSavedInven, 0, sizeof(m_aSavedInven));
	for (int i = 0; i < 6; i++) m_pMasterIcon[i] = NULL;
	m_pHeroIcon = NULL;

	pOldModalWindow = NULL;
}

BOOL CNeuzApp::LoadOption()
{
	g_Option.Load("neuz.ini");
	ApplyWidthHeightChanges();
	SetLODDetail(0);
	return TRUE;
}

void CNeuzApp::ApplyWidthHeightChanges()
{
	FULLSCREEN_WIDTH = g_Option.m_nResWidth;
	FULLSCREEN_HEIGHT = g_Option.m_nResHeight;

	m_dwCreationWidth = FULLSCREEN_WIDTH;
	m_dwCreationHeight = FULLSCREEN_HEIGHT - GetSystemMetrics(SM_CYMENU);
}

HRESULT CNeuzApp::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior,
	D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat)
{

	if (dwBehavior & D3DCREATE_PUREDEVICE)
		return E_FAIL; // GetTransform doesn't work on PUREDEVICE

	if (pCaps->VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
		g_bUsableVS = FALSE;
		if (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING)
			return E_FAIL;
	}
	// Check that the device can blend vertices with at least two matrices
	// (Software can always do up to 4 blend matrices)
	if ((dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING))
	{
		if (pCaps->MaxPointSize <= 1.0f)
			return E_FAIL;
		if (pCaps->MaxVertexBlendMatrices < 2)
			return E_FAIL;
	}

	// If this is a TnL device, make sure it supports directional lights
	if ((dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
		(dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING))
	{
		if (!(pCaps->VertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
			return E_FAIL;
	}

	if (pCaps->RasterCaps & D3DPRASTERCAPS_FOGVERTEX)
		return S_OK;

	return S_OK;
}

void CNeuzApp::UpdateCursor()
{
	map<DWORD, CString> mapCur;
	mapCur[CUR_ATTACK] = "curAttack";
	mapCur[CUR_BASE] = "curBase";
	mapCur[CUR_CAMERA] = "curCamera";
	mapCur[CUR_CONTROL] = "curControl";
	mapCur[CUR_DELAY] = "curDelay";
	mapCur[CUR_EDIT] = "edit";
	mapCur[CUR_GETITEM] = "curGetItem";
	mapCur[CUR_HELP] = "cursor2";
	mapCur[CUR_NO] = "cursor3";
	mapCur[CUR_NPC] = "curNPC";
	mapCur[CUR_SELECT] = "curSelect";
	mapCur[CUR_RESIZE_HORI] = "hori";
	mapCur[CUR_RESIZE_VERT] = "resize_h";
	mapCur[CUR_RESIZE_HV1] = "cur00001";
	mapCur[CUR_RESIZE_HV2] = "cur00002";
	mapCur[CUR_DOOR] = "curDoor";
	mapCur[CUR_GATE] = "curGate";
	mapCur[CUR_LEVER] = "curLever";
	mapCur[CUR_HAMMER] = "anvil";

	const static char* szPath[] = { "CurDefault", "CurCustom" };

	CString str;
	for (auto it = mapCur.begin(); it != mapCur.end(); ++it)
	{
		str.Format("Cur\\%s\\%s.cur", szPath[g_Option.m_nCurFold], it->second);
		m_ahCursor[it->first] = LoadCursorFromFile(str);
		if (m_ahCursor[it->first] == NULL)
		{
			str.Format("Cur\\%s\\%s.ani", szPath[g_Option.m_nCurFold], it->second);
			m_ahCursor[it->first] = LoadCursorFromFile(str);
			if (m_ahCursor[it->first] == NULL)
			{
				MessageBox(0, "Check the integrity of the game files! (Cur)", "Error", 0);
				ExitProcess(-1);
			}
		}
	}
}


HRESULT CNeuzApp::OneTimeSceneInit()
{
	HRESULT hr = g_SoundMng.Initialize(GetSafeHwnd(), DSSCL_PRIORITY, 2, 22050, 16);
	InitCustomSound(FAILED(hr) ? FALSE : TRUE);

	g_SoundMng.m_nSoundVolume = (int)((-1) * (1.0f - g_Option.m_fEffectVolume) * 5000);

	::SetVolume(g_Option.m_fBGMVolume);

	UpdateCursor();

	CWndBase::m_hDefaultCursor = m_ahCursor[CUR_BASE];
	CWndEdit::m_hEditCursor = m_ahCursor[CUR_EDIT];

	return S_OK;
}

HRESULT CNeuzApp::RestoreDeviceObjects()
{
	if ((m_dwCreateFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) || (m_dwCreateFlags & D3DCREATE_MIXED_VERTEXPROCESSING) ||
		m_d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
	{
		CreateSkinningVS(m_pd3dDevice, _T("skining2.vsh"));
	}

	if (m_d3dCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS)
		g_ModelGlobal.m_bDepthBias = TRUE;
	if (g_Option.m_nShadow < 2 && m_d3dCaps.MaxSimultaneousTextures <= 2)
	{
		g_Option.m_nShadow = 2;
		MessageBox(GetSafeHwnd(), m_strArray.GetAt(13), m_strArray.GetAt(11), MB_OK);
	}
	UINT nMem = m_pd3dDevice->GetAvailableTextureMem();
	if (g_Option.m_nTextureQuality == 0 && nMem <= (1024 * 1024 * 64))
	{
		g_Option.m_nTextureQuality = 1;
		MessageBox(GetSafeHwnd(), m_strArray.GetAt(15), m_strArray.GetAt(14), MB_OK);
	}
	if (g_Option.m_nShadow < 2)
	{
		if (nMem <= (1024 * 1024 * 64))
		{
			g_Option.m_nShadow = 2;
			MessageBox(GetSafeHwnd(), m_strArray.GetAt(16), m_strArray.GetAt(14), MB_OK);
		}
		else
		{
			BOOL bSuccess = CreateShadowMap(m_pd3dDevice, D3DFMT_R5G6B5);
			if (bSuccess == FALSE)
			{
				if (g_Option.m_nShadow < 2)
					g_Option.m_nShadow++;
				if (g_Option.m_nTextureQuality < 2)
					g_Option.m_nTextureQuality++;
			}
		}
	}

	g_ParticleMng.RestoreDeviceObjects(m_pd3dDevice);
	g_TailEffectMng.RestoreDeviceObjects(m_pd3dDevice);

	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	if (FAILED(LoadTextureFromRes(m_pd3dDevice, MakePath(DIR_MODELTEX, "etc_reflect.tga"), &g_pReflectMap)))
	{
		Error("etc_Reflect.tga 텍스쳐 못읽음");
	}

	g_ModelGlobal.RestoreDeviceObjects(m_pd3dDevice);

	m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	CWndBase::m_Theme.RestoreDeviceObjects();

	g_WndMng.RestoreDeviceObjects();
	if (m_pd3dDevice)
	{
		m_2DRender.RestoreDeviceObjects(&m_d3dsdBackBuffer);
		if (g_WorldMng.Get())
			g_WorldMng.Get()->RestoreDeviceObjects(m_pd3dDevice);
		CWorld::StaticRestoreDeviceObjects(m_pd3dDevice);
		prj.m_modelMng.RestoreDeviceObjects(m_pd3dDevice);
		prj.m_terrainMng.RestoreDeviceObjects();
	}
	if (g_pBipedMesh)
		g_pBipedMesh->RestoreDeviceObjects();

	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		if (g_Neuz.m_apPlayer[i] && g_Neuz.m_apPlayer[i]->m_pModel)
			g_Neuz.m_apPlayer[i]->m_pModel->RestoreDeviceObjects();
	}

	g_SfxMng.RestoreDeviceObjects();
	g_SfxMeshMng.RestoreDeviceObjects();

	return S_OK;
}

HRESULT CNeuzApp::InvalidateDeviceObjects()
{
	g_Glare.DeleteDeviceObjects();
	DeleteShadowMap(m_pd3dDevice);
	DeleteVertexShader(m_pd3dDevice);
	CWndBase::m_Theme.InvalidateDeviceObjects();
	if (g_WorldMng.Get())
		g_WorldMng.Get()->InvalidateDeviceObjects();
	CWorld::StaticInvalidateDeviceObjects();
	g_WndMng.InvalidateDeviceObjects();
	prj.m_modelMng.InvalidateDeviceObjects();
	prj.m_terrainMng.InvalidateDeviceObjects();
	m_2DRender.InvalidateDeviceObjects();
	if (g_pBipedMesh)
		g_pBipedMesh->InvalidateDeviceObjects();

	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		if (g_Neuz.m_apPlayer[i] && g_Neuz.m_apPlayer[i]->m_pModel)
			g_Neuz.m_apPlayer[i]->m_pModel->InvalidateDeviceObjects();
	}

	g_SfxMng.InvalidateDeviceObjects();
	g_SfxMeshMng.InvalidateDeviceObjects();

	extern LPDIRECT3DTEXTURE9 g_pReflectMap;
	SAFE_RELEASE(g_pReflectMap);

	g_ParticleMng.InvalidateDeviceObjects();
	g_TailEffectMng.InvalidateDeviceObjects();

	g_ModelGlobal.InvalidateDeviceObjects(m_pd3dDevice);

	return S_OK;
}

HRESULT CNeuzApp::DeleteDeviceObjects()
{
	RestoreFakeDisplayMode();

	g_toolTip.Delete();
	g_toolTipSub1.Delete();
	g_toolTipSub2.Delete();

	g_Glare.DeleteDeviceObjects();
	CWndBase::m_Theme.DeleteDeviceObjects();
	g_WndMng.DestroyApplet();
	g_WndMng.Free();

	if (g_WorldMng.Get())
		g_WorldMng.Get()->DeleteDeviceObjects();

	g_WorldMng.Free();
	CWorld::StaticDeleteDeviceObjects();
	g_WndMng.DeleteDeviceObjects();
	prj.m_modelMng.DeleteDeviceObjects();
	prj.m_terrainMng.DeleteDeviceObjects();
	m_2DRender.DeleteDeviceObjects();
	g_DamageNumMng.DeleteDeviceObjects();
	g_DialogMsg.DeleteDeviceObjects();
	if (g_pBipedMesh)
		g_pBipedMesh->DeleteDeviceObjects();

	g_TextureMng.DeleteDeviceObjects();

	SAFE_DELETE(g_pBipedMesh);

	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
		SAFE_DELETE(m_apPlayer[i]);

	g_Shadow.DeleteDeviceObjects();

	g_Object3DMng.DeleteDeviceObjects();
#ifdef __PROF
	g_Prof.Show(m_fFPS);
#endif

	g_SfxMng.DeleteDeviceObjects();
	g_SfxTex.DeleteAll();
	g_SfxMeshMng.DeleteAll();

	g_Object3DMng.Destroy();
	CWndNeuz::FreeTileTexture();

	m_TexCltGauge[0].DeleteDeviceObjects();
	m_TexCltGauge[1].DeleteDeviceObjects();

	m_TextureGauge[0].DeleteDeviceObjects();
	m_TextureGauge[1].DeleteDeviceObjects();
	m_TextureGauge[2].DeleteDeviceObjects();
	m_TextureGauge[3].DeleteDeviceObjects();
	m_TextureGauge[4].DeleteDeviceObjects();

	m_TextureHPGauge[0].DeleteDeviceObjects();
	m_TextureHPGauge[1].DeleteDeviceObjects();
	m_TextureHPGauge[2].DeleteDeviceObjects();

	m_TextureTurboGauge[0].DeleteDeviceObjects();
	m_TextureTurboGauge[1].DeleteDeviceObjects();
	m_TextureCastingGauge[0].DeleteDeviceObjects();
	m_TextureCastingGauge[1].DeleteDeviceObjects();

	m_TexLoading.DeleteDeviceObjects();
	m_texQuestEmoticon.DeleteDeviceObjects();
#ifdef __FL_GW_STATUE_V2
	m_IconStatueQuestion.DeleteDeviceObjects();
#endif
	return S_OK;
}

HRESULT CNeuzApp::FinalCleanup()
{
	if (g_hMutex)
		::CloseHandle(g_hMutex);

#ifdef __FL_FIX_LOADING
	StartDelay(FALSE);
#endif

	::SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	g_dpCertified.DeleteDPObject();
	g_dpLoginClient.DeleteDPObject();
	g_DPlay.DeleteDPObject();
	g_GuildMng.Clear();

	SAFE_DELETE(CParty::m_pPool);
	SAFE_DELETE(CChatting::m_pPool);
	SAFE_DELETE(CGuildMember::sm_pPool);
	SAFE_DELETE(CGuild::sm_pPool);
	SAFE_DELETE(CMover::m_pPool);
	SAFE_DELETE(CActionMover::m_pPool);
	SAFE_DELETE(CItem::m_pPool);
	SAFE_DELETE(CDamageNum::m_pPool);
	SAFE_DELETE(CSfxGenMoveMark::m_pPool);
	SAFE_DELETE(CSfxGenWaterCircle::m_pPool);
	SAFE_DELETE(CSfxGenNormalDmg01::m_pPool);
	SAFE_DELETE(CSfxModel::m_pPool);
	SAFE_DELETE(CSfxGenRainCircle::m_pPool);

	CCLord::Instance()->DestroyColleagues();

	UninitializeNetLib();

	CWndNeuz::FreeTileTexture();
	CResFile::FreeResource();
	UnInitCustomSound();

	g_Option.Save("neuz.ini");
	return S_OK;
}


HRESULT CNeuzApp::Render()
{
	if (m_bActive == FALSE)
		return S_FALSE;

	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, CWndBase::m_Theme.m_d3dcBackground, 1.0f, 0);

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	if (pWndWorld)
		pWndWorld->ClearFlyTarget();

	if (g_Option.m_nShadow < 2)
	{
		CHECK1();
		void RenderShadowMap(LPDIRECT3DDEVICE9 pd3dDevice, CObj * *pList, int nMax);

		if (g_pPlayer)
		{
			CWorld* pWorld = g_pPlayer->GetWorld();
			if (pWorld)
			{
				if (pWorld->GetID() != WI_WORLD_MINIROOM)
					RenderShadowMap(m_pd3dDevice, pWorld->m_aobjCull, pWorld->m_nObjCullSize);
			}
		}
		CHECK2(" Render ShadowMap");
	}

	CHECK1();
	if (SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		CHECK2("Begin");
		g_nRenderCnt++;

		CHECK1();

		if (g_WndMng.IsWindowProcessState())
			g_WndMng.PaintRoot(&m_2DRender);

		CHECK2("Render WndMng");

		if (g_WndMng.m_pWndWorld)
			g_WndMng.m_pWndWorld->RenderFocusArrow(CPoint(0, 0));


		if (CWndBase::m_GlobalShortcut.IsEmpty() == FALSE)
		{
			GET_CLIENT_POINT(GetSafeHwnd(), point);
			if (CWndBase::m_GlobalShortcut.m_pTexture)
				CWndBase::m_GlobalShortcut.m_pTexture->Render(&m_2DRender, CPoint(point.x - 16, point.y - 16));
			else
			{
				int nHeight = m_2DRender.m_pFont->GetMaxHeight() / 2;
				m_2DRender.TextOut(point.x - nHeight, point.y - nHeight, CWndBase::m_GlobalShortcut.m_szString);
			}
		}


		g_toolTip.Paint(&m_2DRender);
		g_toolTipSub1.Paint(&m_2DRender);
		g_toolTipSub2.Paint(&m_2DRender);
		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_2DRender.SetFont(CWndBase::m_Theme.m_pFontWorld);
		g_ClientMsg.Render(CPoint(m_2DRender.m_clipRect.Width() / 2, 520 * m_2DRender.m_clipRect.Height() / 768), &m_2DRender);
		m_2DRender.SetFont(CWndBase::m_Theme.m_pFontText);

		if (m_bTexLoad && m_bTexLoadAlpha)
		{
			m_nTexAlpha = (int)(m_nTexAlpha - 6.0f);

			if (m_nTexAlpha < 0 || m_nTexAlpha > 255)
			{
				m_nTexAlpha = 0;
				m_bTexLoad = FALSE;
				m_bTexLoadAlpha = FALSE;

				m_TexLoading.DeleteDeviceObjects();

			}
			else
			{
				int wideOffsetX = 0;
				m_TexLoading.m_size = CSize(FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
				m_2DRender.RenderTexture(CPoint(0 + wideOffsetX, 0), &m_TexLoading, m_nTexAlpha);
			}
		}

		CHECK1();
		m_pd3dDevice->EndScene();
		CHECK2("End");
	}

	return S_OK;
}

extern int g_nOldVirtKey;

#ifdef __AIO_NEUZAPP
BOOL g_bForWindowDragging = FALSE;
POINT g_ptForWindowDragging;
#endif


HRESULT CNeuzApp::FrameMove()
{
#ifdef __AIO_NEUZAPP
	if (g_bForWindowDragging)
	{
		const int logicalLeftButton = ::GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
		if (false == ::GetAsyncKeyState(logicalLeftButton))
			g_bForWindowDragging = FALSE;
		else
		{
			POINT pt;
			::GetCursorPos(&pt);

			int nOffset_X = pt.x - g_ptForWindowDragging.x;
			int nOffset_Y = pt.y - g_ptForWindowDragging.y;

			if (nOffset_X != 0 || nOffset_Y != 0)
			{
				g_ptForWindowDragging = pt;

				if (m_bMaximized)
				{
					ShowWindow(g_Neuz.GetSafeHwnd(), SW_RESTORE);
					g_bForWindowDragging = FALSE;
				}
				else
				{
					::GetWindowRect(m_hWnd, &m_rcWindowBounds);
					::SetWindowPos(m_hWnd, NULL, m_rcWindowBounds.left + nOffset_X, m_rcWindowBounds.top + nOffset_Y, m_rcWindowBounds.right - m_rcWindowBounds.left, m_rcWindowBounds.bottom - m_rcWindowBounds.top, 0);
				}
			}
		}
	}
#endif

	g_tmCurrent = timeGetTime();
	g_tCurrent = time(NULL);
	g_nProcessCnt++;
	if (g_GameTimer.m_nHour >= 21 || g_GameTimer.m_nHour <= 6)
		::SetNight(TRUE);
	else
		::SetNight(FALSE);

	g_DPlay.ReceiveMessage();
	g_DPlay.SendSnapshot();

	g_DPlay.FlushPlayerAngle();

	g_dpLoginClient.ReceiveMessage();
	g_dpCertified.ReceiveMessage();

	if (m_dwTempMessage)
	{
		CWndBase* pWndBase;
		if (m_timerConnect.TimeOut())
		{
			m_dwTempMessage = 0;
			pWndBase = g_WndMng.GetWndBase(APP_SELECT_CHAR);
			if (pWndBase)
			{
				((CWndSelectChar*)pWndBase)->Connected();
				goto NEXT;
			}
		}
	}

	if (g_pBipedMesh)
		g_pBipedMesh->FrameMove();

	if (g_WndMng.IsWindowProcessState())
		g_WndMng.Process();

#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->Process();
#endif
#ifdef __FL_BATTLE_PASS
	CBattlePass::GetInstance()->Process();
#endif

	if (g_nOldVirtKey != 0)
		g_bKeyTable[g_nOldVirtKey] = FALSE;

NEXT:
	if (g_WndMng.IsTitle() == TRUE)
		CWndBase::m_Theme.FrameMove();

	GET_CLIENT_POINT(GetSafeHwnd(), point);
	g_toolTip.Process(point, &m_2DRender);

	g_toolTipSub1.Process(point, &m_2DRender);
	g_toolTipSub2.Process(point, &m_2DRender);

	g_DamageNumMng.Process();
	g_GameTimer.Compute();

	CalcUPS();
	g_Object3DMng.Process();

	g_ParticleMng.Process();
	g_TailEffectMng.Process();

	g_ClientMsg.Process();

	if (m_bQuit)
	{
		if (g_pPlayer == NULL || m_timerQuit.IsTimeOut())
			::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0);
	}

	return S_OK;
}


void CNeuzApp::HashMD5(char* szResult, LPCTSTR szPWD)
{
	if (strlen(szPWD) == 32)
	{
		strcpy(szResult, szPWD);
		return;
	}

	char szBuffer[256];
	sprintf(szBuffer, "%s%s", __NEUZ_PHASH, szPWD);
	md5(szResult, szBuffer);
}

void CNeuzApp::SetAccountInfo(LPCTSTR lpszAccount, LPCTSTR lpszpw)
{
	CString strAccount = lpszAccount;
	strAccount.TrimLeft();
	strAccount.TrimRight();		// TRIM
	_tcscpy(g_Neuz.m_szAccount, (LPCTSTR)strAccount);
	_tcslwr(g_Neuz.m_szAccount);
	HashMD5(g_Neuz.m_szPassword, lpszpw);
}

int g_nOldVirtKey = 0;

LRESULT CNeuzApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHook = GuildDeploy()->MsgProc(hWnd, msg, wParam, lParam);
	if (bHook)
		return 0;

	CWndEdit* pWndEdit = CWndEdit::GetWndEditCtrl();
	if (pWndEdit)
	{
		if (!((msg == WM_KEYDOWN || msg == WM_KEYUP) && (wParam == VK_ESCAPE || wParam == VK_F1 || wParam == VK_F2 || wParam == VK_F3 ||
			wParam == VK_F4 || wParam == VK_F5 || wParam == VK_F6 || wParam == VK_F7 || wParam == VK_F8 || wParam == VK_F9)))
		{
			if (pWndEdit->IsYouMessage(msg, wParam, lParam))
				return 0;
			if ((msg == WM_KEYDOWN || msg == WM_KEYUP) && wParam != VK_RETURN)
				return 0;
		}
	}
	else
	{
		if (msg >= WM_IME_SETCONTEXT && msg <= WM_IME_KEYUP)
			return 0;

		if (CWndEdit::m_hIMCBackup == NULL)
		{
			CWndEdit::m_hIMCBackup = ::ImmAssociateContext(CWndEdit::m_hWnd, NULL);
		}
		CWndEdit::SetAlphaNumeric();
	}


	if (g_WndMng.IsWindowProcessState())
		g_WndMng.WindowRootProc(msg, wParam, lParam);

	if (g_WndMng.IsOpenModalWnd() == TRUE && msg != WM_LOGOUT)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		{
			if (g_WndMng.m_pLogOutWaitting)
				break;

			if (KillWindow(wParam))
				break;

			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet(APP_COMMUNICATION_CHAT);
			int nVirtKey = (int)wParam;
			if (pWndChat)
			{
				switch (nVirtKey)
				{
				case VK_RETURN:
					if (pWndEdit == NULL)
					{
						pWndChat->AddWndStyle(WBS_MODAL);
						pOldModalWindow = g_WndMng.m_pWndFocus;

						pWndChat->SetFocus();
						CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
						pWndEdit->SetFocus();

						break;
					}
				}
			}

			break;
		}
		case WM_CLOSE:
		{
			prj.m_MapInformationManager.DeleteAllMapInformation();
			g_Neuz.m_bQuit = TRUE;
			if (wParam == FALSE)
			{
				if (g_pPlayer)
				{
#ifndef __AIO_NEUZAPP
					StopMusic();
#endif
					return 1;
				}
			}
			break;
		}
	}
		return CD3DApplication::MsgProc(hWnd, msg, wParam, lParam);
}

	switch (msg)
	{
	case WM_KEYDOWN:
#ifdef __AIO_NEUZAPP
	case WM_SYSKEYDOWN:
#endif
	{
#ifdef __AIO_NEUZAPP
		//		WORD keyFlags = HIWORD(lParam);
		//		BOOL repeatFlag = (keyFlags & KF_REPEAT) == KF_REPEAT;
		//		if (repeatFlag)
		//			break;
#endif

		if (g_WndMng.m_pLogOutWaitting)
			break;

		if (KillWindow(wParam))
			break;

		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet(APP_COMMUNICATION_CHAT);
		int nVirtKey = (int)wParam;
		int lKeyData = lParam;
		if (pWndChat)
		{
			switch (nVirtKey)
			{
			case VK_RETURN:
				if (pWndChat && pWndEdit == NULL)
				{
					pWndChat->SetFocus();
					CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;
					pWndEdit->SetFocus();
				}
				break;
			}
		}
		if (nVirtKey == VK_ESCAPE)
		{
			if (CDeployManager::GetInstance()->IsReady())
			{
				CDeployManager::GetInstance()->EndDeploy();
				return FALSE;
			}

			GuildHouse->m_dwSelectedObjID = NULL_ID;
		}
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
		if (pWndWorld && pWndWorld->IsFocusWnd() == FALSE)
			pWndWorld->OnKeyDown(nVirtKey, 0, 0);

		g_bKeyTable[nVirtKey] = TRUE;

	}
	break;
	case WM_KEYUP:
#ifdef __AIO_NEUZAPP
	case WM_SYSKEYUP:
#endif
	{
		int nVirtKey = (int)wParam;
		int lKeyData = lParam;
		for (int i = 0; i < MAX_SLOT_ITEM; ++i)
		{
			if (g_bSlotSwitchAboutEquipItem[i] == TRUE)
				g_bSlotSwitchAboutEquipItem[i] = FALSE;
		}
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
		if (pWndWorld && pWndWorld->IsFocusWnd() == FALSE)
			pWndWorld->OnKeyUp(nVirtKey, 0, 0);

		if (nVirtKey == VK_NEXT && g_bKeyTable[VK_NEXT])
		{
			if (CDeployManager::GetInstance()->IsReady())
			{
				CDeployManager::GetInstance()->m_fAngle += 45.0f;
				if (CDeployManager::GetInstance()->m_fAngle > 359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
				CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
		}
		else if (nVirtKey == VK_PRIOR && g_bKeyTable[VK_PRIOR])
		{
			if (CDeployManager::GetInstance()->IsReady())
			{
				CDeployManager::GetInstance()->m_fAngle -= 45.0f;
				if (CDeployManager::GetInstance()->m_fAngle < -359.0f) CDeployManager::GetInstance()->m_fAngle = 0.0f;
				CDeployManager::GetInstance()->m_pTargetObj->SetAngle(CDeployManager::GetInstance()->m_fAngle);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
		}

		if (nVirtKey == VK_MULTIPLY && g_bKeyTable[VK_MULTIPLY])
		{
			if (!g_WndMng.m_clearFlag)
				g_WndMng.ClearAllWnd();
			else
				g_WndMng.RestoreWnd();
		}

		g_bKeyTable[nVirtKey] = FALSE;
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		if (CDeployManager::GetInstance()->IsReady() && msg == WM_MOUSEWHEEL)
		{
			int nDelta = (short)HIWORD(wParam);
			if (nDelta < 0)
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
			else
			{
				CDeployManager::GetInstance()->m_pTargetObj->AddAngle(-2.5f);
				CDeployManager::GetInstance()->m_pTargetObj->UpdateLocalMatrix();
			}
		}
		break;
	case WM_CLOSE:
	{
		prj.m_MapInformationManager.DeleteAllMapInformation();

		g_Neuz.m_bQuit = TRUE;
		if (wParam == FALSE)
		{
			if (g_pPlayer)
			{
				StopMusic();
				return 1;
			}
		}
	}
	break;
	case WM_LOGOUT:
	{
		g_WndMng.OpenTitle();
		PlayMusic(BGM_TITLE, 0);
		return 1;
	}
	break;

	case WM_MOVE:
	{

	}
	break;

	case WM_COMMAND:
		break;

#ifdef __AIO_NEUZAPP
	case WM_SYSCOMMAND:
	{
		if ((wParam & 0xFFF0) == SC_MOVE)
		{
			if (g_bForWindowDragging)
			{
				::GetCursorPos(&g_ptForWindowDragging);
				return 0;
			}
		}
	}
	break;

	case WM_NCLBUTTONUP:
		g_bForWindowDragging = FALSE;
		break;

	case WM_NCLBUTTONDOWN:
	{
		if (m_bWindowed == true && wParam == HTCAPTION)
		{
			g_bForWindowDragging = TRUE;
		}
	}
	break;

	case WM_KILLFOCUS:
		g_bForWindowDragging = FALSE;
		break;
#endif
	}
	return CD3DApplication::MsgProc(hWnd, msg, wParam, lParam);
}

BOOL CNeuzApp::KillWindow(int wParam)
{
	CWndStateConfirm* pWndStateConfirm = (CWndStateConfirm*)g_WndMng.GetWndBase(APP_STATE_CONFIRM);
	if (pWndStateConfirm)
	{
		if (wParam == VK_RETURN)
		{
			
		}
		else if (wParam == VK_ESCAPE)
		{
			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_CHARACTER3);
			pWndStateConfirm->Destroy();
		}
		return TRUE;
	}

	return FALSE;
}

unsigned int __stdcall LoadProperty(void* p)
{
	BOOL bOK = prj.OpenProject("Masquerade.prj");
	if (FALSE == bOK)
	{
		::AfxMessageBox("Error Open Project");
		::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, TRUE, 0);
	}

#ifdef __FL_FIX_LOADING
	g_Neuz.StartDelay(FALSE);
#endif

	return 0;
}

void CNeuzApp::BeginLoadThread()
{
	CResFile::ScanResource("");

	prj.LoadPreFiles();

	unsigned int dwThreadId(0);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadProperty, (LPVOID)this, 0, &dwThreadId);
}

void CNeuzApp::WaitLoading()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

HRESULT CNeuzApp::InitDeviceObjects()
{
	BeginLoadThread();

	SetMouseCursor(CUR_DELAY);

	g_MusicMng.LoadScript(MakePath(DIR_MUSIC, _T("default.bgm")));
	g_SoundMng.LoadScript(MakePath(DIR_CLIENT, _T("sound.inc")));

#ifdef __AIO_NEUZAPP
	UINT nMem = m_pd3dDevice->GetAvailableTextureMem();
#else
	int nMem = m_pd3dDevice->GetAvailableTextureMem();
#endif

	if (m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
	{
		MessageBox(GetSafeHwnd(), m_strArray.GetAt(12), m_strArray.GetAt(11), MB_OK);
		int* p = NULL;
		*p = 1;
	}

	m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	prj.m_modelMng.InitDeviceObjects(m_pd3dDevice);
	prj.m_terrainMng.InitDeviceObjects(m_pd3dDevice);

	CWorld::StaticInitDeviceObjects(m_pd3dDevice);

	g_DamageNumMng.LoadTexture(m_pd3dDevice);

	g_SfxMng.InitDeviceObjects(m_pd3dDevice);
	g_SfxMeshMng.InitDeviceObjects(m_pd3dDevice);

	g_Shadow.InitDeviceObjects(m_pd3dDevice);
	g_Shadow.LoadElement("Shadow.o3d");

	g_ParticleMng.InitDeviceObjects(m_pd3dDevice);
	if (g_Option.m_nTextureQuality == 2)
		g_ParticleMng.m_bActive = FALSE;

	m_TextureGauge[0].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauMagChargeBG.TGA"), 0xffff00ff, TRUE);
	m_TextureGauge[1].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauMagChargeHead.TGA"), 0xffff00ff, TRUE);
	m_TextureGauge[2].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauMagChargeBody.TGA"), 0xffff00ff, TRUE);
	m_TextureGauge[3].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauMagChargeBody2.tga"), 0xffff00ff, TRUE);
	m_TextureGauge[4].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauMagChargeTail.TGA"), 0xffff00ff, TRUE);

	m_TextureHPGauge[0].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauPartyHpBG.TGA"), 0xffff00ff, TRUE);
	m_TextureHPGauge[1].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauPartyHp.TGA"), 0xffff00ff, TRUE);
	m_TextureHPGauge[2].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauPartyHp_Dbf.TGA"), 0xffff00ff, TRUE);

	m_TextureTurboGauge[0].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelBG.TGA"), 0xffff00ff, TRUE);
	m_TextureTurboGauge[1].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelInner.TGA"), 0xffff00ff, TRUE);
	m_TextureCastingGauge[0].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelBG.TGA"), 0xffff00ff, TRUE);
	m_TextureCastingGauge[1].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelInner.TGA"), 0xffff00ff, TRUE);

	m_pMasterIcon[0] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert1.dds"), 0xffff00ff);
	m_pMasterIcon[1] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert2.dds"), 0xffff00ff);
	m_pMasterIcon[2] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert3.dds"), 0xffff00ff);
	m_pMasterIcon[3] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert4.dds"), 0xffff00ff);
	m_pMasterIcon[4] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert5.dds"), 0xffff00ff);
	m_pMasterIcon[5] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Expert6.dds"), 0xffff00ff);
	m_pHeroIcon = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "icon_Hero.dds"), 0xffff00ff);

	m_TexCltGauge[0].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelBG.TGA"), 0xffff00ff, TRUE);
	m_TexCltGauge[1].LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "GauAccelInner.TGA"), 0xffff00ff, TRUE);

#ifdef __FL_GW_STATUE_V2
	m_IconStatueQuestion.LoadTexture(m_pd3dDevice, MakePath(DIR_ICON, "iconstatuequestion.dds"), 0xffff00ff);
#endif

	m_nTexAlpha = 255;
	m_bTexLoad = FALSE;
	m_bTexLoadAlpha = FALSE;

	g_toolTip.InitTexture();
	g_toolTipSub1.InitTexture();
	g_toolTipSub2.InitTexture();

	CWndBase::m_Theme.LoadTheme(m_pd3dDevice, "Default.thm");
	CWndBase::m_Theme.InitDeviceObjects(m_pd3dDevice);


	g_WndMng.m_pApp = this;
	g_WndMng.m_pTheme = &CWndBase::m_Theme;
	g_WndMng.m_pFont = CWndBase::m_Theme.m_pFontText;
	g_WndMng.Initialize();
	g_WndMng.InitDeviceObjects();
	CWndBase::m_hWnd = GetSafeHwnd();
	CWndBase::SetForbidTexture(m_pd3dDevice, MakePath(DIR_ICON, "icon_forbid.dds"));


	g_DialogMsg.InitDeviceObjects(m_pd3dDevice);


	m_2DRender.InitDeviceObjects(m_pd3dDevice);
	m_2DRender.m_pFont = CWndBase::m_Theme.m_pFontText;
	g_WndMng.OpenTitle(TRUE);

	InitFST();

	m_texQuestEmoticon.LoadScript(m_pd3dDevice, MakePath(DIR_ICON, "icon_QuestEmoticon.inc"));

	TexturePool::Get()->Init(m_pd3dDevice);

	prj.m_MapInformationManager.LoadMapInformationData();

	PlayMusic(BGM_TITLE, 0);
#ifdef __DISCORD // Initialize
	ManageDiscord(DIS_START);
#endif

	return S_OK;
}

void CNeuzApp::ResetStaticValues()
{
	GuildHouse->ResetValues();
	CEnvironment::GetInstance()->SetEnvironmentEffect(FALSE);
}

#ifdef __DISCORD
void CNeuzApp::ManageDiscord(int nType)
{
	switch (nType)
	{
	case DIS_START:
		g_DiscordNeuz->Initialize();
		break;
	case DIS_END:
		g_DiscordNeuz->ShutDown();
		break;
	case DIS_DEFAULT_DETAILS:
		g_DiscordNeuz->SetDefaultDetails();
		break;
	case DIS_CHARACTER_DETAILS:
		g_DiscordNeuz->SetCharacterDetails();
		break;
	}
}
#endif

#ifdef __FL_FIX_LOADING
BOOL CNeuzApp::IsAlreadyRunning()
{
	HANDLE hNeuzMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, "IfNEqzKLLgh");
	if (hNeuzMutex != NULL)
		return TRUE;

	return FALSE;
}

void CNeuzApp::StartDelay(BOOL bStart)
{
	if (bStart)
	{
		g_hMutexDelay = OpenMutex(MUTEX_ALL_ACCESS, TRUE, "IfNEqzKLLgh");
		if (g_hMutexDelay == NULL)
		{
			g_hMutexDelay = CreateMutex(NULL, TRUE, "IfNEqzKLLgh");
			if (g_hMutexDelay)
				ReleaseMutex(g_hMutexDelay);
		}
	}
	else
	{
		if (g_hMutexDelay != NULL)
			::CloseHandle(g_hMutexDelay);

		g_hMutexDelay = NULL;
	}
}
#endif