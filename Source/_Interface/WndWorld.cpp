#include "stdafx.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineItem.h"
#include "AppDefine.h"

#include "DialogMsg.h"
#include "ClientMsg.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "WndMessenger.h"
#include "WndIndirectTalk.h"

#include "guild.h"

#include "WorldMap.h"
#include "Commonctrl.h"

#include "WndNavigator.h"

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

extern	CGuildMng	g_GuildMng;
extern	CDPClient	g_DPlay;

#include "party.h"
extern	CParty		g_Party;
extern	CPartyMng	g_PartyMng;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;
#include "WndSummonFriend.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013

#include "yUtil.h"
extern CFLASH g_Flash;

#include "defineskill.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

#ifdef __SYS_ITEMTRANSY
#include "wndchangeface.h"
#endif //__SYS_ITEMTRANSY

#include "GuildHouse.h"
#include "WndGuildHouse.h"
#include "WndMapEx.h"

int g_nSkillCurSelect;
const int MAX_POWER_GAUGE = 1800;
const DWORD POWER_ATK_DELAY = 1800;


CCaption::CCaption()
{
	m_nCount = 0;
	m_bEnd = FALSE;

}
CCaption::~CCaption()
{
	RemoveAll();
}
void CCaption::RemoveAll()
{
	for (int i = 0; i < m_aCaption.GetSize(); i++)
	{
		LPCAPTION lpCaption = (LPCAPTION)m_aCaption.GetAt(i);
		SAFE_RELEASE(lpCaption->m_texture.m_pTexture);
		SAFE_DELETE(lpCaption);
	}
	m_aCaption.RemoveAll();
	m_nCount = 0;
	m_bEnd = FALSE;
}
void CCaption::Process()
{
	if (m_nCount == 0 && m_aCaption.GetSize())
		m_nCount = 1;
	if (m_bEnd)
	{
		if (m_timer.IsTimeOut())
		{
			for (int i = 0; i < m_aCaption.GetSize(); i++)
			{
				LPCAPTION lpCaption = (LPCAPTION)m_aCaption.GetAt(i);
				lpCaption->m_nAlpha -= 3;
				if (lpCaption->m_nAlpha < 0)
					lpCaption->m_nAlpha = 0;
			}
		}
	}
	else
	{
		for (int i = 0; i < m_nCount; i++)
		{
			LPCAPTION lpCaption = (LPCAPTION)m_aCaption.GetAt(i);
			lpCaption->m_fAddScale += 0.002f;
			lpCaption->m_fXScale += lpCaption->m_fAddScale;
			lpCaption->m_fYScale += lpCaption->m_fAddScale;
			if (lpCaption->m_fXScale > 1.0f)
				lpCaption->m_fXScale = 1.0f;
			if (lpCaption->m_fYScale > 1.0f)
				lpCaption->m_fYScale = 1.0f;
			if (lpCaption->m_fXScale > 0.1f)
			{
				if (i == m_nCount - 1)
				{
					m_nCount++;
					if (m_nCount > m_aCaption.GetSize())
					{
						m_nCount = m_aCaption.GetSize();
						if (lpCaption->m_fYScale >= 1.0f)
						{
							m_bEnd = TRUE;
							m_timer.Set(SEC(5));
						}
					}
					break;
				}
			}
		}
	}
}
HRESULT CCaption::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	return S_OK;
}
HRESULT CCaption::DeleteDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
HRESULT CCaption::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCaption::InvalidateDeviceObjects()
{
	RemoveAll();
	return S_OK;
}
void CCaption::Render(CPoint ptBegin, C2DRender* p2DRender)
{
	int nCount = m_nCount > m_aCaption.GetSize() ? m_aCaption.GetSize() : m_nCount;

	for (int i = 0; i < nCount; i++)
	{
		LPCAPTION lpCaption = (LPCAPTION)m_aCaption.GetAt(i);
		CRect rect = p2DRender->m_clipRect;//GetWndRect();
		CPoint point = CPoint(rect.Width() / 2, 0);

		CSize size = lpCaption->m_size;

		FLOAT fXScale = (7.0f - lpCaption->m_fXScale * 6.0f);
		FLOAT fYScale = (7.0f - lpCaption->m_fYScale * 6.0f);

		if (fXScale < 1.0f || fYScale < 1.0f)
		{
			fXScale = 1.0f;
			fYScale = 1.0f;
		}

		point.x = (LONG)(point.x - ((size.cx / 2) * fXScale));
		point += ptBegin;

		if (point.x < 1)
			point.x = 1;

		if (lpCaption->m_nAlpha)
			CWndBase::m_Theme.m_pFontCaption->DrawText((FLOAT)(point.x), (FLOAT)(point.y), fXScale, fYScale, D3DCOLOR_ARGB((int)(lpCaption->m_fXScale * 255) - (255 - lpCaption->m_nAlpha), 250, 250, 255), lpCaption->m_szCaption);

		ptBegin.y += size.cy;
	}
}
void CCaption::AddCaption(LPCTSTR lpszCaption, CD3DFontAPI* pFont, BOOL bChatLog, DWORD dwColor)
{
	BOOL bRenderOK = FALSE;
	if (m_aCaption.GetSize() > 3 || (m_bEnd && m_timer.IsTimeOut()))
		RemoveAll();

	CSize size;

	CWndBase::m_Theme.m_pFontCaption->GetTextExtent(lpszCaption, &size);

	LPCAPTION lpCaption = new CAPTION;
	strcpy(lpCaption->m_szCaption, lpszCaption);
	lpCaption->m_pFont = pFont;
	lpCaption->m_fXScale = 0.0f;
	lpCaption->m_fYScale = 0.0f;
	lpCaption->m_fAddScale = 0.0f;
	lpCaption->m_nAlpha = 255;
	lpCaption->m_size = size;

	m_aCaption.Add(lpCaption);

	if (bChatLog && g_WndMng.m_pWndChatLog)
	{
		g_WndMng.m_pWndChatLog->PutString(lpszCaption);
	}

	m_bEnd = FALSE;
}
HRESULT CCapTime::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	return S_OK;
}
HRESULT CCapTime::DeleteDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}
HRESULT CCapTime::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CCapTime::InvalidateDeviceObjects()
{
	m_texture.DeleteDeviceObjects();
	return S_OK;
}

CCapTime::CCapTime()
{
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_fAddScale = 0;
	m_nAlpha = 255;
	m_nTime = 0;
	m_bRender = FALSE;
	m_nStep = 0;
	//	m_size
}
CCapTime::~CCapTime()
{
	DeleteDeviceObjects();
}
void CCapTime::Render(CPoint ptBegin, C2DRender* p2DRender)
{
	if (m_bRender == FALSE)
		return;

	if (::GetLanguage() != LANG_JAP)
	{
		if (m_texture.m_pTexture == NULL)
			return;
	}

	CEditString strTemp;
	CString strTime;
	strTime.Format(GETTEXT(TID_QUEST_LIMIT_TIME), m_nTime);
	strTemp.AddParsingString(strTime);
	strTime = strTemp;

	CRect rect = p2DRender->m_clipRect;//GetWndRect();
	CPoint point = CPoint(rect.Width() / 2, 0);
	FLOAT fXScale = m_fXScale; // 최대 7배 사이즈 
	FLOAT fYScale = m_fYScale; // 최대 7배 사이즈 
	point.x = (LONG)(point.x - ((m_size.cx / 2) * fXScale));
	point.y = (LONG)(point.y - ((m_size.cy / 2) * fYScale));
	point += ptBegin;

	if (fXScale < 1.0f || fYScale < 1.0f)
	{
		fXScale = 1.0f;
		fYScale = 1.0f;
	}
	else if (fXScale > 7.0f || fYScale > 7.0f)
	{
		fXScale = 7.0f;
		fYScale = 7.0f;
	}

	if (m_nAlpha)
		CWndBase::m_Theme.m_pFontCaption->DrawText((FLOAT)(point.x), (FLOAT)(point.y), fXScale, fYScale, D3DCOLOR_ARGB((int)(m_fXScale * 255) - (255 - m_nAlpha), 250, 250, 255), strTime);
}
void CCapTime::SetTime(int nTime, CD3DFontAPI* pFont)
{
	m_nTime = nTime;
	CEditString strTemp;
	CString strTime;
	strTime.Format(GETTEXT(TID_QUEST_LIMIT_TIME), m_nTime);
	strTemp.AddParsingString(strTime);
	strTime = strTemp;
	m_bRender = TRUE;

	//	CSize size;
	m_fAddScale = 0.001f;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_nStep = 0;
	m_nAlpha = 0;

	CWndBase::m_Theme.m_pFontCaption->GetTextExtent(strTime, &m_size);
}
void CCapTime::Process()
{
	if (m_nStep == 0)
	{
		m_nAlpha += 3;
		if (m_nAlpha >= 255)
		{
			m_nAlpha = 255;
			m_nStep++;
		}
	}
	else
	{
		m_fAddScale += 0.001f;
		m_fXScale += m_fAddScale;
		m_fYScale -= m_fAddScale;
		m_nAlpha -= 5;
		if (m_nAlpha < 0)
			m_nAlpha = 0;
		if (m_fXScale > 3.0f)
			m_fXScale = 3.0f;
		if (m_fYScale < 0.0f)
			m_fYScale = 0.0f;
	}
}

CCapTime g_CapTime;
CCaption g_Caption1;

D3DXVECTOR3 CWndWorld::m_vTerrainPoint;

#ifdef __YCLOTH
CCloth g_Cloth;
#endif

CWndWorld::CWndWorld()
#ifdef __BUFF_1107
	: m_buffs(NULL)
#endif	// __BUFF_1107
{
#ifdef __FL_FFA
	ClearFFA();
#endif
#ifdef __FL_TDM
	ClearTDM();
#endif

	nWidthOld = 0;
	m_nMoverHPOld = 0;
	objRenderOld = NULL_ID;

	m_bFreeMove = FALSE;
	SetFullMax(TRUE);
	g_WndMng.m_pWndWorld = this;
	m_fHigh = -1.0f;
	m_fLow = 999999.0f;
	m_bFlyMove = FALSE;

	s_bAccKeyed = FALSE;

	m_bSelectTarget = FALSE;
	m_timerFocusAttack.Set(500);

	m_dwIdBgmMusic = 0;

	m_pVBGauge = NULL;
	m_vTerrainPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_bAutoAttack = FALSE;
	m_dwNextSkill = NEXTSKILL_NONE;
	m_objidTracking = NULL_ID;

	m_vTelePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_nSelect = 0;
	ClearFlyTarget();
	m_fRollAng = 0;
	m_pFontAPICaption = NULL;
	m_pFontAPITitle = NULL;
	m_pFontAPITime = NULL;

	m_pBuffTexture.clear();
	m_pBuffTexture.resize(3);

	m_dwOneSecCount = GetTickCount();

	for (int j = 0; j < SM_MAX; ++j)
	{
		m_dwSMItemTexture[j] = NULL;
		m_bSMFlsh[j] = FALSE;
		m_nSMAlpha[j] = 202;
	}

	m_nDubleUp = 0;
	m_timerAutoRunPush.Set(200);
	m_timerAutoRun.Set(200);
	m_timerAutoRunBlock.Set(200);

	m_dwPowerTick = 0;
	m_idLastTarget = NULL_ID;

	m_dwGuildCombatTime = 0xffffffff;
	memset(&m_QuestTime, 0, sizeof(m_QuestTime));
	memset(m_szGuildCombatStr, 0, sizeof(char) * 64);
	m_pSelectRenderObj = NULL;
	m_IsMailTexRender = FALSE;
	m_mapGC_GuildStatus.clear();

	m_bCtrlPushed = FALSE;
	m_bRenderFPS = FALSE;

	m_rcCheck.reserve(4);

	m_pWndBuffStatus = NULL;
	m_bShiftPushed = FALSE;
	m_bAngelFinish = FALSE;
	m_pNextTargetObj = NULL;
	m_pRenderTargetObj = NULL;

	memset(m_szSecretRoomStr, 0, sizeof(char) * 256);

	for (int i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
		m_stKillCountCiphers[i].szMyGuildKillCount = '0';
		m_stKillCountCiphers[i].ptPos = CPoint(0, 0);
		m_stKillCountCiphers[i].fScaleX = 1.0f;
		m_stKillCountCiphers[i].fScaleY = 1.0f;
		m_stKillCountCiphers[i].nAlpha = 255;
	}

	m_bFlashBackground = FALSE;
	m_bSetQuestNPCDest = FALSE;
	m_vDestinationArrow = D3DXVECTOR3(-1.0F, 0.0F, -1.0F);
}

CWndWorld::~CWndWorld()
{
	g_ClientMsg.ClearAllMessage();
	DeleteDeviceObjects();
}
int nColor;
BOOL g_bAlpha;
FLOAT g_fXScale, g_fYScale;
FLOAT g_fAddScale;
int		g_nFlySpeed = 0;
void CWndWorld::OnDraw(C2DRender* p2DRender)
{
	if (g_pPlayer && (m_bRenderFPS || g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR)))
	{
		if (!g_WndMng.m_clearFlag)
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontWorld);
			CString strFPS = "";
			strFPS.Format("%d fps | %d ms | %d sfx | %d obj", static_cast<int>(g_Neuz.m_fFPS), g_Neuz.m_dwPingTime, CWorld::m_nSfxCullSize, CWorld::m_nObjCullSize);
			p2DRender->TextOut(2, 112, strFPS, D3DCOLOR_ARGB(255, 0, 255, 255));
			p2DRender->SetFont(pOldFont);
		}
	}

	g_DialogMsg.Render(p2DRender);
	g_Caption1.Render(CPoint(0, 184 * p2DRender->m_clipRect.Height() / 768), p2DRender);
	g_CapTime.Render(CPoint(0, 50 * p2DRender->m_clipRect.Height() / 768), p2DRender);

}
void CWndWorld::Projection(LPDIRECT3DDEVICE9 pd3dDevice)
{

	CRect rectRoot = m_pWndRoot->GetWindowRect();
	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = rectRoot.Width();
	viewport.Height = rectRoot.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	CRect rect = GetClientRect();
	viewport.Width = rect.Width();
	viewport.Height = rect.Height();

	g_WorldMng.Get()->Projection(pd3dDevice, viewport.Width, viewport.Height);
}

BOOL CWndWorld::OnEraseBkgnd(C2DRender* p2DRender)
{
	CHECK1();

	Projection(D3DDEVICE);
	DWORD dwColor = CWorld::GetDiffuseColor();
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, dwColor /*D3DCOLOR_ARGB( 255, 255, 255, 255 )*/, 1.0f, 0);

	if (g_pPlayer == NULL)		return FALSE;

	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	CWorld* pWorld = g_WorldMng.Get();
	pWorld->SetCamera(&g_Neuz.m_camera);
	pWorld->Render(pd3dDevice, m_Theme.m_pFontWorld);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);

	CObj* pFocus = pWorld->GetObjFocus();

	int nGaugeValue;
	int nPower = GetGaugePower(&nGaugeValue);
	BOOL bRenderGauge = FALSE;
	DWORD dwShootItemKind = 0;


	CItemElem* pItemElem = g_pPlayer->GetWeaponItem();


	if (pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW)
	{

		if (pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive())
		{
			bRenderGauge = TRUE;
			if (m_bLButtonDown && m_bSelectTarget)
			{
				if (nPower >= MAX_CHARGE_LEVEL)
					dwShootItemKind = IK3_BOW;
			}
		}
	}

	if (pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
	{

		if (pFocus && pFocus->GetType() == OT_MOVER && g_pPlayer->IsLive())
		{
			bRenderGauge = TRUE;
			if (m_bLButtonDown && m_bSelectTarget)
			{
				if (g_pPlayer->GetManaPoint() >= 5)
				{
					if (nPower >= MAX_CHARGE_LEVEL)
						dwShootItemKind = IK3_WAND;
				}
				else
				{
					if (nPower >= (MAX_CHARGE_LEVEL - 1))
						dwShootItemKind = IK3_WAND;
				}
			}
		}
	} // IK3_WAND

	if (dwShootItemKind)
	{
		ASSERT(nPower > 0);
		if (g_pPlayer->IsAttackAble(pFocus))
		{
			OBJID idTarget = ((CCtrl*)pFocus)->GetId();
			switch (dwShootItemKind)
			{
			case IK3_WAND:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetMagicAttack(idTarget, nPower);
				break;
			case IK3_BOW:
				m_dwPowerTick = ::GetTickCount() + POWER_ATK_DELAY;
				g_pPlayer->CMD_SetRangeAttack(idTarget, nPower);
				break;
			}
		}
	}

	if (bRenderGauge)
	{
		if (nPower == -1)
			nPower = MAX_CHARGE_LEVEL;
		else if (m_bLButtonDown == FALSE || m_bSelectTarget == FALSE)
			nPower = 0;

		g_pPlayer->RenderGauge(pd3dDevice, nPower);
	}

	if (g_pPlayer->IsFly())
		g_pPlayer->RenderTurboGauge(pd3dDevice, 0xff0000ff, g_pPlayer->m_tmAccFuel, 12 * 1000);

	D3DXVECTOR3 v3CameraDir, v3PartyMemberDir;
	v3CameraDir = g_Neuz.m_camera.m_vLookAt - g_Neuz.m_camera.m_vPos;
	D3DXVec3Normalize(&v3CameraDir, &v3CameraDir);

	for (int i = 0; i < g_Party.GetSizeofMember(); i++)
	{
		u_long idPlayer = g_Party.GetPlayerId(i);
		CMover* pMover = prj.GetUserByID(idPlayer);

		if (pMover == g_pPlayer)
			continue;

		if (!IsValidObj(pMover))
			continue;

		v3PartyMemberDir = pMover->GetPos() - g_Neuz.m_camera.m_vPos;
		D3DXVec3Normalize(&v3PartyMemberDir, &v3PartyMemberDir);

		if (D3DXVec3Dot(&v3CameraDir, &v3PartyMemberDir) < 0.0f)
			continue;

		pMover->RenderHP(g_Neuz.m_pd3dDevice);
	}

	if (IsValidObj(g_pPlayer))
	{
		if (CWorld::m_nZoomLevel == 0)
		{
			g_pPlayer->RenderHP(g_Neuz.m_pd3dDevice);
			g_pPlayer->RenderCasting(g_Neuz.m_pd3dDevice);
			g_pPlayer->RenderSkillCasting(g_Neuz.m_pd3dDevice);
			g_pPlayer->RenderCltGauge(g_Neuz.m_pd3dDevice);
		}
	}

	if (pFocus && pFocus->GetType() == OT_MOVER)
	{
		if (((CMover*)pFocus)->IsMode(TRANSPARENT_MODE))
			pWorld->SetObjFocus(NULL);
	}

	if (IsValidObjID(GuildHouse->m_dwSelectedObjID) && prj.GetCtrl(GuildHouse->m_dwSelectedObjID))
		RenderSelectObj(p2DRender, prj.GetCtrl(GuildHouse->m_dwSelectedObjID));
	else if (IsValidObj(pWorld->GetObjFocus()))
		RenderSelectObj(p2DRender, pWorld->GetObjFocus());
	else if (IsValidObj(m_pSelectRenderObj))
		RenderSelectObj(p2DRender, m_pSelectRenderObj);

	if (g_pPlayer && g_pPlayer->m_pActMover->IsFly() == TRUE)
		RenderAltimeter();

	CRect rectClient = GetClientRect();
	g_Neuz.m_camera.Transform(g_Neuz.m_pd3dDevice, g_WorldMng.Get());

	g_DamageNumMng.m_matView = g_Neuz.m_camera.m_matView;
	g_DamageNumMng.m_matProj = g_WorldMng.Get()->m_matProj;
	pd3dDevice->GetViewport(&g_DamageNumMng.m_viewport);

	if (g_pPlayer->IsDie())
		CObj::m_pObjHighlight = NULL;
	else
		HighlightObj(GetMousePoint());

	CRect rect = GetClientRect();
	D3DXVECTOR3 vRayEnd;

	g_DamageNumMng.Render();
	if (g_WndMng.m_pWndTaskBar->m_nCurQueue >= 0)
	{
		int nCount = 5 - g_WndMng.m_pWndTaskBar->m_nCurQueue % 5;
		g_DamageNumMng.m_textPackNum.Render(p2DRender, CPoint(rectClient.Width() - 20, rectClient.Height() - 20), nCount);
	}

	RenderArrow();

	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);


	{
		int	i, nSize = m_aFlyTarget.GetSize();
		CRect rect;
		CMover* pMover;
		OBJID idMover;

		for (i = 0; i < nSize; i++)
		{
			idMover = m_aFlyTarget.GetAt(i);
			pMover = prj.GetMover(idMover);
			if (IsValidObj(pMover))
			{
				GetBoundRect(pMover, &rect);
				g_Neuz.m_2DRender.RenderRect(rect, D3DCOLOR_ARGB(0xff, 255, 32, 32));
			}
		}
	}

	g_Flash.RenderFlash(p2DRender);

	if (!g_WndMng.m_clearFlag)
		RenderBuff(p2DRender);


	if (m_dwGuildCombatTime != 0xffffffff)
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

		CTimeSpan ct(((m_dwGuildCombatTime - GetTickCount()) / 1000));

		char szMsg[256] = { 0 };

		sprintf(szMsg, "%s %.2d:%.2d:%.2d", m_szGuildCombatStr, ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());

		if (ct.GetHours() <= 0 && ct.GetMinutes() <= 0 && ct.GetSeconds() <= 0)
			m_dwGuildCombatTime = 0xffffffff;

		int nX = m_rectWindow.Width() / 2;
		int nY = 110;

		CSize cSize = p2DRender->m_pFont->GetTextExtent(szMsg);
		nX -= (cSize.cx / 2);

		p2DRender->TextOut(nX, nY, szMsg, 0xffffffff, 0xff990099);
		g_Neuz.m_2DRender.SetFont(pOldFont);
	}

#if __VER >= 12 // __SECRET_ROOM
	DWORD dwSecretTime = CSecretRoomMng::GetInstance()->m_dwRemainTime;

	if (dwSecretTime != 0)
	{
		CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
		g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

		CTimeSpan ct(((dwSecretTime - GetTickCount()) / 1000));

		char szMsg[256] = { 0 };

		sprintf(szMsg, "%s %.2d:%.2d:%.2d", m_szSecretRoomStr, ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());

		if (ct.GetHours() <= 0 && ct.GetMinutes() <= 0 && ct.GetSeconds() <= 0)
		{
			dwSecretTime = 0;
			CSecretRoomMng::GetInstance()->m_dwRemainTime = 0;
		}

		int nX = m_rectWindow.Width() / 2;
		int nY = 110;

		CSize cSize = p2DRender->m_pFont->GetTextExtent(szMsg);
		nX -= (cSize.cx / 2);

		p2DRender->TextOut(nX, nY, szMsg, 0xffffffff, 0xff990099);
		g_Neuz.m_2DRender.SetFont(pOldFont);
	}

	if (g_pPlayer && CSecretRoomMng::GetInstance()->m_nState >= SRCONT_WAR)
	{
		if (CSecretRoomMng::GetInstance()->IsInTheSecretRoom(g_pPlayer))
			DrawSecretRoomInfo(p2DRender);
	}
#endif //__SECRET_ROOM

#ifdef __FL_FFA
	RenderFFA(p2DRender);
#endif
#ifdef __FL_TDM
	RenderTDM(p2DRender);
#endif

	if (g_pPlayer && g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
	{
		int		nGap = 16;
		int		nRate = 0;
		CString strFormat;
		CPoint  cPoint = CPoint(GetClientRect().Width() - 150, 200);

		CRect crBoard;

		crBoard.left = cPoint.x - 10;
		crBoard.top = cPoint.y - 10;
		crBoard.right = cPoint.x + 140;
		crBoard.bottom = crBoard.top + ((m_mmapGuildCombat_GuildPrecedence.size() + 3) * 16);
		p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(30, 0, 0, 0));

		DWORD dwFontColor = 0xFF9ED3FF;
		int     nPoint;
		int     nOldPoint = 0xffffffff;
		CString str;
		p2DRender->TextOut(cPoint.x + 10, cPoint.y, prj.GetText(TID_GAME_GUILDCOMBAT_RATE), 0xFFEBAD18, 0xFF000000);
		cPoint.y += (nGap + (nGap / 2));
		for (multimap<int, CString>::reverse_iterator i = m_mmapGuildCombat_GuildPrecedence.rbegin(); i != m_mmapGuildCombat_GuildPrecedence.rend(); ++i)
		{
			nPoint = i->first;
			str = i->second;

			if (nOldPoint != nPoint)
				nRate++;

			//			if( nPoint > 0 )
			{
				if (g_pPlayer->GetGuild())
				{
					if (stricmp(str, g_pPlayer->GetGuild()->m_szGuild) == 0)
						dwFontColor = 0xFF9ED3FF;
					else
						dwFontColor = 0xFFFFFF99;
				}
				else
				{
					dwFontColor = 0xFFFFFF99;
				}

				if (nOldPoint != nPoint)
				{
					strFormat.Format("%2d", nRate);
					p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
				}
				else
				{
					strFormat = "  ";
					p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
				}

				g_WndMng.StringSize(str, 6);
				strFormat.Format("%s", str);
				p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwFontColor, 0xFF000000);

				strFormat.Format("%d", nPoint);
				p2DRender->TextOut(cPoint.x + 110, cPoint.y, strFormat, dwFontColor, 0xFF000000);

			}

			cPoint.y += nGap;
			nOldPoint = nPoint;
		}


		dwFontColor = 0xFFFFFF99;
		nOldPoint = 0xffffffff;
		nRate = 0;
		cPoint.y += 50;

		crBoard.left = cPoint.x - 10;
		crBoard.top = cPoint.y - 10;
		crBoard.right = cPoint.x + 140;
		crBoard.bottom = crBoard.top + ((m_mmapGuildCombat_PlayerPrecedence.size() + 3) * 16);
		p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(30, 0, 0, 0));

		BOOL bPlayerRender = FALSE;
		int  nPlayerRate = 0;
		int  nPlayerPoint = 0;
		const int nMaxRender = 10;
		int nMaxIndex = 0;
		u_long uiPlayer;
		p2DRender->TextOut(cPoint.x + 10, cPoint.y, prj.GetText(TID_GAME_GUILDCOMBAT_PERSON_RATE), 0xFFEBAD18, 0xFF000000);
		cPoint.y += (nGap + (nGap / 2));
		for (multimap<int, u_long>::reverse_iterator j = m_mmapGuildCombat_PlayerPrecedence.rbegin(); j != m_mmapGuildCombat_PlayerPrecedence.rend(); ++j)
		{
			nPoint = j->first;
			uiPlayer = j->second;


			if (nOldPoint != nPoint)
				nRate++;

			nMaxIndex++;

			if (nMaxIndex > nMaxRender)
			{
				if (uiPlayer == g_pPlayer->m_idPlayer)
				{
					nPlayerPoint = nPoint;
					nPlayerRate = nRate;
					bPlayerRender = FALSE;
					break;
				}
				else
				{
					continue;
				}
			}
			else if (uiPlayer == g_pPlayer->m_idPlayer)
			{
				bPlayerRender = TRUE;
			}

			{
				str = CPlayerDataCenter::GetInstance()->GetPlayerString(uiPlayer);

				if (uiPlayer == g_pPlayer->m_idPlayer)
				{
					dwFontColor = 0xFF9ED3FF;
				}
				else
				{
					dwFontColor = 0xFFFFFF99;
				}

				if (nOldPoint != nPoint)
				{
					strFormat.Format("%2d", nRate);
					p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
				}
				else
				{
					strFormat = "  ";
					p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
				}

				g_WndMng.StringSize(str, 6);
				strFormat.Format("%s", str);
				p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwFontColor, 0xFF000000);

				strFormat.Format("%d", nPoint);
				p2DRender->TextOut(cPoint.x + 110, cPoint.y, strFormat, dwFontColor, 0xFF000000);
			}

			cPoint.y += nGap;
			nOldPoint = nPoint;
		}

		if (m_mmapGuildCombat_PlayerPrecedence.size() && bPlayerRender == FALSE)
		{
			cPoint.y += nGap;

			str = g_pPlayer->GetName();
			g_WndMng.StringSize(str, 6);

			dwFontColor = 0xFF9ED3FF;
			strFormat.Format("%2d", nPlayerRate);
			p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);
			strFormat.Format("%s", str);
			p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwFontColor, 0xFF000000);
			strFormat.Format("%d", nPlayerPoint);
			p2DRender->TextOut(cPoint.x + 110, cPoint.y, strFormat, dwFontColor, 0xFF000000);
		}
	}


	if (g_pPlayer && g_pPlayer->GetGuild())
	{
		int nState = IsGCStatusPlayerWar(g_pPlayer->m_idPlayer);

		if (nState != -1 && nState != -2)
		{
			CString str;
			int   nRate = 0;
			int	  nGap = 16;
			__GUILDRATE GuildRate;
			DWORD dwFontColor = 0xFFFFFF99;

			CPoint cPoint;
			CRect  crBoard;

			cPoint.x = 10;
			cPoint.y = 150;

			crBoard.left = cPoint.x - 5;
			crBoard.top = cPoint.y - 30;
			crBoard.right = cPoint.x + 130;

			crBoard.bottom = crBoard.top + ((m_vecGuildCombat_GuildStatus.size() + 2) * 16);
			p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(30, 0, 0, 0));

			BOOL bJoinMessage = FALSE;
			BOOL bJoinReady = FALSE;
			BOOL bSpace = FALSE;

			CString strFormat = g_pPlayer->GetGuild()->m_szGuild;
			p2DRender->TextOut(cPoint.x, cPoint.y - (nGap + 5), strFormat, 0xFFEBAD18, 0xFF000000);

			CRect   cRectHP;
			CMover* pObjMember = NULL;
			int		nLeftTemp = 0;

			for (int k = 0; k < (int)(m_vecGuildCombat_GuildStatus.size()); k++)
			{
				GuildRate = m_vecGuildCombat_GuildStatus[k];

				nRate++;

				str = CPlayerDataCenter::GetInstance()->GetPlayerString(GuildRate.m_uidPlayer);

				if (GuildRate.m_uidPlayer == g_pPlayer->m_idPlayer)
				{
					dwFontColor = 0xFF9ED3FF;


					if (GuildRate.bJoinReady == TRUE && GuildRate.nLife > 0)
						bJoinMessage = TRUE;
				}
				else
				{
					dwFontColor = 0xFFFFFF99;
				}


				if (GuildRate.bJoinReady && GuildRate.nLife > 0)
				{
					cPoint.y += nGap;
					bJoinReady = TRUE;
				}


				if (GuildRate.nLife <= 0 && bSpace == FALSE)
				{
					cPoint.y += nGap;
					bSpace = TRUE;
				}

				if (bJoinReady || bSpace)
				{
					if (bJoinReady)
					{
						dwFontColor = 0xFF00CB00;
						bJoinReady = FALSE;
					}
					else
						if (bSpace)
						{
							dwFontColor = 0xFFCBCBCB;
						}
				}

				if (GetGCStatusDefender(GuildRate.m_uidPlayer) == GuildRate.m_uidPlayer)
				{
					p2DRender->TextOut(cPoint.x - 6, cPoint.y, "D", dwFontColor, 0xFF000000);
				}

				strFormat.Format("%2d", nRate);
				p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwFontColor, 0xFF000000);

				g_WndMng.StringSize(str, 6);
				strFormat.Format("%s", str);
				p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwFontColor, 0xFF000000);

				nLeftTemp = GuildRate.nLife - 1;

				if (nLeftTemp < 0)
					nLeftTemp = 0;

				strFormat.Format("%d", nLeftTemp);
				p2DRender->TextOut(cPoint.x + 110, cPoint.y, strFormat, dwFontColor, 0xFF000000);


				pObjMember = prj.GetUserByID(GuildRate.m_uidPlayer);
				FLOAT fPersent = (IsValidObj(pObjMember) ? (FLOAT)pObjMember->GetHitPoint() / (FLOAT)pObjMember->GetMaxHitPoint() : 0);

				const FLOAT fFullWidth = 60;
				FLOAT fDrawHP = fFullWidth * fPersent;

				cRectHP.SetRect(cPoint.x + 140, cPoint.y, (int)(cPoint.x + 140 + fFullWidth), cPoint.y + 11);
				p2DRender->RenderFillRect(cRectHP, D3DCOLOR_ARGB(100, 0, 0, 0));

				cRectHP.DeflateRect(1, 1, (int)((fFullWidth - fDrawHP) + 1), 1);
				p2DRender->RenderFillRect(cRectHP, D3DCOLOR_ARGB(100, 0, 255, 0));
				//////////////////////////////

				cPoint.y += nGap;
			}

			if (bJoinMessage && g_GuildCombatMng.m_nState == CGuildCombat::WAR_STATE)
			{
				CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
				g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

				strFormat = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_READY);
				int nCenter = p2DRender->m_pFont->GetTextExtent(strFormat).cx / 2;
				int nY = p2DRender->m_pFont->GetTextExtent(strFormat).cy;
				p2DRender->TextOut((g_Option.m_nResWidth / 2) - nCenter, (g_Option.m_nResHeight / 2) - 150, strFormat, 0xFFFFFF00, 0xFF000000);

				strFormat = prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_READY2);
				nCenter = p2DRender->m_pFont->GetTextExtent(strFormat).cx / 2;
				p2DRender->TextOut((g_Option.m_nResWidth / 2) - nCenter, ((g_Option.m_nResHeight / 2) - 148) + nY, strFormat, 0xFFFFFF00, 0xFF000000);

				g_Neuz.m_2DRender.SetFont(pOldFont);
			}
		}
	}


	return TRUE;
}

#if __VER >= 12 // __SECRET_ROOM
bool prKCountAsce(__SRGUILDINFO guild1, __SRGUILDINFO guild2)
{
	bool rtn_val = false;

	if (guild1.nWarState > guild2.nWarState)
		rtn_val = true;
	else if (guild1.nWarState == guild2.nWarState)
	{
		if (guild1.nKillCount > guild2.nKillCount)
			rtn_val = true;
	}

	return rtn_val;
}

void CWndWorld::DrawSecretRoomGuildInfo(C2DRender* p2DRender, BOOL bIsMyGuild, int nRank, __SRGUILDINFO stGuildInfo, CPoint ptRank, CPoint ptLogo,
	CPoint ptGName, CPoint ptHypoon, CPoint ptState, float fLogoScaleX, float fLogoScaleY, CRect rectBg)
{
	CString strFormat;
	DWORD dwColor = 0xFFFFFFFF;
	// Draw Rank
	strFormat.Format("%d.", nRank + 1);
	p2DRender->TextOut(ptRank.x, ptRank.y, strFormat, 0xFFFFFFFF);

	// Draw Guild Name
	strFormat = g_GuildMng.GetGuild(stGuildInfo.dwGuildId)->m_szGuild;
	g_WndMng.StringSize(strFormat, 14);
	p2DRender->TextOut(ptGName.x, ptGName.y, strFormat, 0xFFFEBB1B);

	// Draw Hypoon
	if (stGuildInfo.nWarState == MONSTER_MIDBOSS || stGuildInfo.nWarState == MONSTER_BOSS)
		p2DRender->TextOut(ptHypoon.x - 3, ptHypoon.y, "vs", 0xFFFFFFFF);
	else
		p2DRender->TextOut(ptHypoon.x, ptHypoon.y, "-", 0xFFFFFFFF);

	// Draw State
	int nMax = CSecretRoomMng::GetInstance()->m_mapMonsterNum.find(stGuildInfo.nWarState)->second;

	switch (stGuildInfo.nWarState)
	{
	case MONSTER_WIN:
		DrawOutLineLamp(p2DRender, rectBg, 0x00FFFF00, 0xBEFFFF00, stGuildInfo.nWarState, nRank, bIsMyGuild);
		strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_WIN));
		dwColor = 0xFFFFFF00;
		p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		break;
	case MONSTER_FAILED:
		strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_LOSS));
		dwColor = 0xFFFF0000;
		p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		break;
	case MONSTER_NORMAL:
	{
		if (bIsMyGuild)
		{
			DrawMyGuildKillCount(p2DRender, stGuildInfo, ptState, nMax);
			DrawOutLineFlash(p2DRender, rectBg, 0x00FFFFFF, 0xBEFFFFFF);
		}
		else
		{
			strFormat.Format("%03d", stGuildInfo.nKillCount);

			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);

			ptState.x += 21;
			p2DRender->TextOut(ptState.x, ptState.y, "/", 0xFFC2D1FF);

			strFormat.Format("%03d", nMax);
			ptState.x += 7;
			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		}
	}
	break;
	case MONSTER_MIDBOSS:
		DrawOutLineLamp(p2DRender, rectBg, 0x008BE3E2, 0xBE8BE3E2, stGuildInfo.nWarState, nRank, bIsMyGuild);
		strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_MID_BOSS));
		dwColor = 0xFF8BE3E2;
		p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		break;
	case MONSTER_BOSS:
		DrawOutLineLamp(p2DRender, rectBg, 0x00F67608, 0xBEF67608, stGuildInfo.nWarState, nRank, bIsMyGuild);
		strFormat.Format("%s", prj.GetText(TID_GAME_SECRETROOM_FIN_BOSS));
		dwColor = 0xFFF67608;
		p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		break;
	}
}

void CWndWorld::DrawMyGuildKillCount(C2DRender* p2DRender, __SRGUILDINFO stGuildInfo, CPoint ptState, int nMax)
{
	CString strFormat;
	int i;
	int nCipher = 0;
	char strTempKillCount[MAX_KILLCOUNT_CIPHERS + 1];
	char strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS + 1];
	DWORD dwColor = 0xFFFFFFFF;

	sprintf(strTempKillCount, "%03d", stGuildInfo.nKillCount);

	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		strMyGuildKillCount[i] = m_stKillCountCiphers[i].szMyGuildKillCount;
		if (strTempKillCount[i] > '0' && nCipher == 0)
			nCipher = MAX_KILLCOUNT_CIPHERS - i;
	}

	strMyGuildKillCount[MAX_KILLCOUNT_CIPHERS] = NULL;
	int nMyGuildKillCount = atoi(strMyGuildKillCount);

	if (nMyGuildKillCount != stGuildInfo.nKillCount)
		m_bFlashBackground = TRUE;

	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		if (nMyGuildKillCount != stGuildInfo.nKillCount)
		{
			if (MAX_KILLCOUNT_CIPHERS - i <= nCipher)
			{
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = FALSE;
				m_stKillCountCiphers[i].fScaleX = 2.5f;
				m_stKillCountCiphers[i].fScaleY = 2.5f;
			}

			m_stKillCountCiphers[i].szMyGuildKillCount = strTempKillCount[i];
		}

		m_stKillCountCiphers[i].ptPos = ptState;

		if (m_stKillCountCiphers[i].bDrawMyGuildKillCount)
		{
			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);
		}

		ptState.x += 9;
	}

	p2DRender->TextOut(ptState.x, ptState.y, "/", 0xFFC2D1FF);

	ptState.x += 7;
	strFormat.Format("%03d", nMax);
	p2DRender->TextOut(ptState.x, ptState.y, strFormat, dwColor);

	// Kill Count Animation
	for (i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
	{
		if (m_stKillCountCiphers[i].bDrawMyGuildKillCount == FALSE)
		{
			m_stKillCountCiphers[i].fScaleX -= 0.15f;
			m_stKillCountCiphers[i].fScaleY -= 0.15f;

			if (m_stKillCountCiphers[i].fScaleX <= 1.0f && m_stKillCountCiphers[i].fScaleY <= 1.0f)
			{
				m_stKillCountCiphers[i].fScaleX = 1.0f;
				m_stKillCountCiphers[i].fScaleY = 1.0f;
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
			}

			strFormat.Format("%c", m_stKillCountCiphers[i].szMyGuildKillCount);
			int nPosx = (int)(m_stKillCountCiphers[i].ptPos.x - (6 * (m_stKillCountCiphers[i].fScaleX - 1.0f)));
			int nPosy = (int)(m_stKillCountCiphers[i].ptPos.y - (8 * (m_stKillCountCiphers[i].fScaleY - 1.0f)));
			dwColor = D3DCOLOR_ARGB((m_stKillCountCiphers[i].nAlpha -= 10), 255, 255, 255);

			p2DRender->TextOut(nPosx, nPosy, m_stKillCountCiphers[i].fScaleX,
				m_stKillCountCiphers[i].fScaleY, strFormat, dwColor);
		}
	}
}

void CWndWorld::DrawOutLineLamp(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, int nState, int nRank, BOOL bIsMyGuild)
{
	CPoint Point = CPoint(rectBg.TopLeft().x, rectBg.TopLeft().y);
	LONG thick;
	float fVar = 0.01f;

	if (bIsMyGuild)
		thick = 4;
	else
		thick = 2;

	if (nState == MONSTER_BOSS || nState == MONSTER_MIDBOSS)
		fVar = 0.04f;
	else if (nState == MONSTER_WIN)
		fVar = 0.01f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

	static BOOL  bReverse[8] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
	static FLOAT fLerp[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	if (bReverse[nRank])
		fLerp[nRank] -= fVar;
	else
		fLerp[nRank] += fVar;

	if (fLerp[nRank] > 1.0f)
	{
		bReverse[nRank] = TRUE;
		fLerp[nRank] = 1.0f;
	}
	else if (fLerp[nRank] < 0.0f)
	{
		bReverse[nRank] = FALSE;
		fLerp[nRank] = 0.0f;
	}

	D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp[nRank]);

	CRect Rect = CRect(0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left) - thick - 1, Point.y + Rect.top - (thick / 2), (Point.x + Rect.left), Point.y + Rect.bottom + (thick / 2)),
		dwColor1, dwColor2, dwColor1, dwColor2);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.top) - thick - 1, (Point.x + Rect.right), Point.y + Rect.top),
		dwColor1, dwColor1, dwColor2, dwColor2);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.right), Point.y + Rect.top - (thick / 2), (Point.x + Rect.right) + thick, Point.y + Rect.bottom + (thick / 2)),
		dwColor2, dwColor1, dwColor2, dwColor1);

	p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.bottom), (Point.x + Rect.right), (Point.y + Rect.bottom) + thick),
		dwColor2, dwColor2, dwColor1, dwColor1);
}

void CWndWorld::DrawOutLineFlash(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	if (m_bFlashBackground)
	{
		CPoint Point = CPoint(rectBg.TopLeft().x, rectBg.TopLeft().y);
		LONG thick = 6;

		D3DXCOLOR dwColorDest2 = dwColorstart;
		D3DXCOLOR dwColor = dwColorend;
		D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
		D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

		static BOOL  bReverse = FALSE;
		static FLOAT fLerp = 0.0f;

		if (bReverse)
			fLerp -= 0.15f;
		else
			fLerp += 0.15f;

		if (fLerp > 1.0f)
		{
			bReverse = TRUE;
			fLerp = 1.0f;
			m_bFlashBackground = FALSE;
		}
		else if (fLerp < 0.0f)
		{
			bReverse = FALSE;
			fLerp = 0.0f;
		}

		D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp);

		CRect Rect = CRect(0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left) - thick, Point.y + Rect.top - (thick / 2), (Point.x + Rect.left), Point.y + Rect.bottom + (thick / 2)),
			dwColor1, dwColor2, dwColor1, dwColor2);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.top) - thick, (Point.x + Rect.right), Point.y + Rect.top),
			dwColor1, dwColor1, dwColor2, dwColor2);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.right), Point.y + Rect.top - (thick / 2), (Point.x + Rect.right) + thick, Point.y + Rect.bottom + (thick / 2)),
			dwColor2, dwColor1, dwColor2, dwColor1);

		p2DRender->RenderFillRect(CRect((Point.x + Rect.left), (Point.y + Rect.bottom), (Point.x + Rect.right), (Point.y + Rect.bottom) + thick),
			dwColor2, dwColor2, dwColor1, dwColor1);
	}
}

void CWndWorld::DrawSecretRoomInfo(C2DRender* p2DRender)
{
#ifdef __CLIENT
	CPoint cPoint;
	CRect crBoard, crMyGuildBoard;
	DWORD dwBgColor;

	// Background Board Setting
	crBoard.left = 20;
	crBoard.top = 120;
	crBoard.right = crBoard.left + 270;
	crBoard.bottom = crBoard.top + 20;

	crMyGuildBoard.left = 10;
	crMyGuildBoard.top = 120;
	crMyGuildBoard.right = crMyGuildBoard.left + 305;
	crMyGuildBoard.bottom = crMyGuildBoard.top;

	if (g_pPlayer && g_pPlayer->GetGuild())
	{
		CString strFormat;
		__SRGUILDINFO stGuildInfo;
		CSecretRoomContinent* pSRCont = CSecretRoomMng::GetInstance()->m_pSRCont;

		CD3DFont* pOldFont = p2DRender->GetFont();

		if (pSRCont)
		{
			int size = pSRCont->m_vecSecretRoomTender.size();

			// Set Guild List & Sort by KillCount
			m_vecGuildList.clear();
			for (int i = 0; i < size; i++)
			{
				stGuildInfo.dwGuildId = pSRCont->m_vecSecretRoomTender[i].dwGuildId;
				stGuildInfo.nWarState = pSRCont->m_vecSecretRoomTender[i].nWarState;
				stGuildInfo.nKillCount = pSRCont->m_vecSecretRoomTender[i].nKillCount;

				m_vecGuildList.push_back(stGuildInfo);
			}

			std::sort(m_vecGuildList.begin(), m_vecGuildList.end(), prKCountAsce);

			vector<__SRGUILDINFO>::iterator iter = m_vecGuildList.begin();

			for (int j = 0; j < (int)(m_vecGuildList.size()); j++)
			{
				if (iter != m_vecGuildList.end())
				{
					stGuildInfo = (*iter);
					// Darw Background

					if (g_GuildMng.GetGuild(stGuildInfo.dwGuildId) == NULL)
					{
						iter++;
						continue;
					}

					if (stGuildInfo.dwGuildId == g_pPlayer->GetGuild()->m_idGuild)
					{
						if (stGuildInfo.nWarState == MONSTER_FAILED)
							dwBgColor = D3DCOLOR_ARGB(150, 255, 0, 0);
						else
							dwBgColor = D3DCOLOR_ARGB(100, 0, 100, 255);

						p2DRender->SetFont(CWndBase::m_Theme.m_pFontSRMyGiuld);

						crMyGuildBoard.top = crBoard.top + 18;
						crMyGuildBoard.bottom = crMyGuildBoard.top + 25;

						crBoard.top += 15;
						crBoard.bottom += 15;

						cPoint.x = crMyGuildBoard.left + 6;
						cPoint.y = crMyGuildBoard.top + 4;

						p2DRender->RenderFillRect(crMyGuildBoard, dwBgColor);

						DrawSecretRoomGuildInfo(p2DRender, TRUE, j, stGuildInfo, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 16, crMyGuildBoard.top),
							CPoint(cPoint.x + 50, cPoint.y), CPoint(cPoint.x + 210, cPoint.y), CPoint(cPoint.x + 230, cPoint.y), 1.0f, 1.0f, crMyGuildBoard);
					}
					else
					{
						if (stGuildInfo.nWarState == MONSTER_FAILED)
							dwBgColor = D3DCOLOR_ARGB(150, 255, 0, 0);
						else
							dwBgColor = D3DCOLOR_ARGB(100, 245, 204, 176);

						p2DRender->SetFont(CWndBase::m_Theme.m_pFontSRGiuld);

						crBoard.top += 25;
						crBoard.bottom += 25;

						cPoint.x = crBoard.left + 4;
						cPoint.y = crBoard.top + 4;

						p2DRender->RenderFillRect(crBoard, dwBgColor);

						DrawSecretRoomGuildInfo(p2DRender, FALSE, j, stGuildInfo, CPoint(cPoint.x, cPoint.y), CPoint(crBoard.left + 18, crBoard.top),
							CPoint(cPoint.x + 36, cPoint.y), CPoint(cPoint.x + 180, cPoint.y), CPoint(cPoint.x + 200, cPoint.y), 0.8f, 0.8f, crBoard);
					}

					iter++;
				}
				p2DRender->SetFont(pOldFont);
			}
		}
	}
#endif //__CLIENT
}

#endif //__SECRET_ROOM

void CWndWorld::RenderArrow()
{

	CWorld* pWorld = g_WorldMng();
	if (!g_pPlayer || !pWorld)
		return;
	D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
	D3DXVECTOR3 vDest(0.0F, 0.0F, 0.0F);
	LPDIRECT3DDEVICE9 pd3dDevice = m_pApp->m_pd3dDevice;

	int nBlend = 255;
	if (m_vDestinationArrow == D3DXVECTOR3(-1.0F, 0.0F, -1.0F) || g_pPlayer->GetWorld()->GetID() != WI_WORLD_MADRIGAL)
		nBlend = 0;
	else
	{
		vDest = m_vDestinationArrow;
		vDest.y = g_pPlayer->GetPos().y;
	}

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	/*
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	*/
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);


	D3DXMatrixLookAtLH(&matWorld, &vDest, &vSrc, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	matWorld._41 = g_pPlayer->GetPos().x;
	matWorld._42 = g_pPlayer->GetPos().y + 2.0f;
	matWorld._43 = g_pPlayer->GetPos().z;

	/*D3DXVECTOR3 vDir = vDest - vSrc;
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXMATRIX matRot;
	static float fAngle = 0.0f;
	fAngle += 0.01f;
	D3DXMatrixRotationAxis( &matRot, &vDir, fAngle );
	D3DXMatrixMultiply( &matWorld, &matRot, &matWorld ); */

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

#if __VER >= 13 // __QUEST_HELPER
	m_meshArrow.SetBlendFactor(nBlend);
	m_meshArrow.Render(pd3dDevice, &matWorld);

	if (m_bSetQuestNPCDest)
		RenderArrow_Text(pd3dDevice, vDest, matWorld);		//gmpbisgun : refactoring 2009_10_20

#else //__QUEST_HELPER
	m_meshArrow.SetBlendFactor(80);
	m_meshArrow.Render(pd3dDevice, &matWorld);
#endif //__QUEST_HELPER
}

void CWndWorld::RenderArrow_Text(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3& vDest, const D3DXMATRIX& matWorld)
{

	D3DXVECTOR3 vOut, vPos, vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_meshArrow.GetBBVector();

	pd3dDevice->GetViewport(&vp);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	CWorld* pWorld = g_WorldMng.Get();
	if (pWorld)
	{
		D3DXVec3Project(&vOut, &vPosHeight, &vp, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &matWorld);
	}

	CPoint point;
	point.x = (LONG)(vOut.x);
	point.y = (LONG)(vOut.y);

	D3DXVECTOR3 vSrc = g_pPlayer->GetPos();
	D3DXVECTOR3 vDist = vDest - vSrc;
	float fDistSq = D3DXVec3Length(&vDist);
	CString strTemp;
	int nMeter = (int)fDistSq;

	strTemp.Format("%dm", nMeter);
	if (nMeter < 1000)
		strTemp.Format("%dm", nMeter);
	else
	{
		int nKMeter = nMeter / 1000;
		strTemp.Format("%dKm", nKMeter);
	}

	int nCount = 0;
	do
	{
		nMeter = nMeter / 10;
		nCount++;
	} while (nMeter > 0);

#ifdef __BS_PUTNAME_QUESTARROW
	strTemp = m_strDestName + CString("(") + strTemp + CString(")");
#endif

	CD3DFont* pFont = g_WndMng.m_Theme.m_pFontWorld;
	if (pFont)
	{
		CSize size = pFont->GetTextExtent(strTemp);
		pFont->DrawText((FLOAT)(point.x - size.cx * 0.5f), (FLOAT)(point.y - 30), D3DCOLOR_XRGB(50, 150, 200), strTemp);
	}
}

BOOL CWndWorld::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	DWORD dwCursor = 0xffffffff;
	if (m_bMButtonDown)
		dwCursor = CUR_CAMERA;
	else
		if (CObj::m_pObjHighlight)
		{
			if (CObj::m_pObjHighlight->IsDelete() == FALSE)
			{
				if (CObj::m_pObjHighlight->GetType() == OT_MOVER)
				{
					CMover* pMover = (CMover*)CObj::m_pObjHighlight;
					if (pMover->IsPeaceful())
					{
						if (pMover->IsPlayer() == FALSE)
						{
							dwCursor = CUR_NPC;
						}
					}
					else
						dwCursor = CUR_ATTACK;
				}
				else
					if (CObj::m_pObjHighlight->GetType() == OT_ITEM)
						dwCursor = CUR_GETITEM;
					else if (CObj::m_pObjHighlight->GetType() == OT_CTRL)
					{
						ObjProp* pObjProp = CObj::m_pObjHighlight->GetProp();

						if (pObjProp)
						{
							CtrlProp* pCtrlProp;
							pCtrlProp = prj.GetCtrlProp(pObjProp->dwID);

							if (pCtrlProp)
							{
								switch (pCtrlProp->dwCtrlKind1)
								{
								case CK1_HOUSING:
									break;

								case CK1_GUILD_HOUSE:
									dwCursor = CUR_CONTROL;
									break;
								case CK1_CHEST:
								default:
									dwCursor = CUR_CONTROL;
									break;
								}
							}
						}
					}
					else
						dwCursor = CUR_NPC;
			}
		}
		else
		{
			if ((CMover::GetActiveMover() && CMover::GetActiveMover()->m_vtInfo.VendorIsVendor()) || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
				dwCursor = CUR_NO;

#ifdef __EVE_MINIGAME
			if (g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO))
				dwCursor = CUR_NO;
#endif

			if (g_WndMng.GetWndBase(APP_RR_MINIGAME_KAWIBAWIBO) || g_WndMng.GetWndBase(APP_RR_MINIGAME_DICE) || g_WndMng.GetWndBase(APP_RR_MINIGAME_ARITHMETIC)
				|| g_WndMng.GetWndBase(APP_RR_MINIGAME_STOPWATCH) || g_WndMng.GetWndBase(APP_RR_MINIGAME_TYPING) || g_WndMng.GetWndBase(APP_RR_MINIGAME_CARD)
				|| g_WndMng.GetWndBase(APP_RR_MINIGAME_LADDER))
				dwCursor = CUR_NO;

			if (g_WndMng.GetWndBase(APP_SMELT_JEWEL))
				dwCursor = CUR_NO;

#ifdef __S_SERVER_UNIFY
			if (g_WndMng.m_bAllAction == FALSE)
				dwCursor = CUR_NO;
#endif

		}
	if (dwCursor == 0xffffffff)
		CWndNeuz::OnSetCursor(pWndBase, nHitTest, message);
	else
		SetMouseCursor(dwCursor);
	return TRUE;
}
void CWndWorld::GetBoundRect(CObj* pObj, CRect* pRect)
{
	CWorld* pWorld = g_WorldMng.Get();
	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vMin, vMax, vPos;

	vPos = pObj->GetScrPos();

	D3DVIEWPORT9 vp;
	pd3dDevice->GetViewport(&vp);

	vp.X = 0;
	vp.Y = 0;

	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &pObj->GetMatrixScale());
	D3DXMatrixMultiply(&matWorld, &matWorld, &pObj->GetMatrixRotation());
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	const BOUND_BOX* pBB = pModel->GetBBVector();
	D3DXVECTOR3 vOut[8];
	for (int i = 0; i < 8; i++)
		D3DXVec3Project(&vOut[i], &pBB->m_vPos[i], &vp, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &matWorld);

	CRect rectClient = GetClientRect();
	//m_rectBound.SetRect( rectClient.right, rectClient.bottom, rectClient.left, rectClient.top );
	pRect->SetRect(65535, 65535, -65535, -65535);
	for (int i = 0; i < 8; i++)
	{
		vPos = vOut[i];
		if (vPos.x < pRect->left)
			pRect->left = (LONG)(vPos.x);
		if (vPos.x > pRect->right)
			pRect->right = (LONG)(vPos.x);
		if (vPos.y < pRect->top)
			pRect->top = (LONG)(vPos.y);
		if (vPos.y > pRect->bottom)
			pRect->bottom = (LONG)(vPos.y);
	}
}

void CWndWorld::RenderSelectObj(C2DRender* p2DRender, CObj* pObj)
{
	CWorld* pWorld = g_WorldMng.Get();
	if (!pWorld || !pObj)
		return;

	if (pObj->GetType() == OT_MOVER)
	{
		CMover* pMover = (CMover*)pObj;
		if (IsValidObj(pMover) && (pMover->IsPeaceful() == FALSE || pMover->IsPlayer()))
		{
			BOOL bSkip = FALSE;
			if (pMover->IsMode(TRANSPARENT_MODE))
				bSkip = TRUE;

			if (bSkip == FALSE)
			{
				CRect rect;
				int nWidthClient = 200;
				int nWidth = (int)((__int64)nWidthClient * (__int64)pMover->GetHitPoint() / (__int64)pMover->GetMaxHitPoint());
				if (nWidth > nWidthClient)
					nWidth = nWidthClient;

				if (pMover->GetId() == objRenderOld)
				{
					if (nWidthOld > nWidthClient || nWidthOld < 0)
						nWidthOld = nWidthOld > nWidthClient ? nWidthClient : 0;

					if (nWidthOld < nWidth)
					{
						int nWidthMinusOld = nWidth - nWidthOld;
						nWidthMinusOld = nWidthMinusOld / 5;
						if (nWidthMinusOld <= 1)
							nWidthMinusOld = 1;

						nWidth = nWidthOld + (nWidth - nWidthOld > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOld);
						nWidthOld = nWidth;
					}
					else if (nWidthOld > nWidth)
					{
						int nWidthMinusOld = nWidthOld - nWidth;
						nWidthMinusOld = nWidthMinusOld / 5;
						if (nWidthMinusOld <= 1)
							nWidthMinusOld = 1;

						nWidth = nWidthOld - (nWidthOld - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOld - nWidth);
						nWidthOld = nWidth;
					}
				}
				else
					nWidthOld = nWidth;

				int nPos = (GetClientRect().Width() - nWidthClient) / 2;
				rect = CRect(nPos, 30, nPos + nWidthClient, 0);
				CPoint point = rect.TopLeft();
				CRect rectTemp = rect;
				rectTemp.right = rectTemp.left + nWidth;

				ClientToScreen(rect);
				ClientToScreen(rectTemp);
				m_Theme.MakeGaugeVertex(p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB(200, 255, 255, 255), m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.RenderGauge(p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.MakeGaugeVertex(p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB(128, 255, 15, 15), m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.RenderGauge(p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal);

				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(m_Theme.m_pFontWorld);

				// Job and element icon rendering
				point.y -= 20;
				if (pMover->IsPlayer())
				{
					int nJob = pMover->GetJob();
					if (prj.m_aJob[nJob].dwJobType == JTYPE_PRO)
						m_texPlayerDataIcon.Render(p2DRender, point, (19 + nJob - 6), 0xffffffff, 1.05f, 1.05f);
					else if (prj.m_aJob[nJob].dwJobType == JTYPE_MASTER)
					{
						int nMasterIndex = 27;
						if (pMover->GetLevel() < 70)
							nMasterIndex = 27;
						else if (pMover->GetLevel() >= 70 && pMover->GetLevel() < 80)
							nMasterIndex = 28;
						else if (pMover->GetLevel() >= 80 && pMover->GetLevel() < 90)
							nMasterIndex = 29;
						else if (pMover->GetLevel() >= 90 && pMover->GetLevel() < 100)
							nMasterIndex = 30;
						else if (pMover->GetLevel() >= 100 && pMover->GetLevel() < 110)
							nMasterIndex = 31;
						else if (pMover->GetLevel() >= 110 && pMover->GetLevel() <= 120)
							nMasterIndex = 32;

						m_texPlayerDataIcon.Render(p2DRender, point, nMasterIndex, 0xffffffff, 1.05f, 1.05f);
						point.x += 20;
						m_texPlayerDataIcon.Render(p2DRender, point, (19 + nJob - 16), 0xffffffff, 1.05f, 1.05f);
					}
					else if (prj.m_aJob[nJob].dwJobType == JTYPE_HERO)
					{
						m_texPlayerDataIcon.Render(p2DRender, point, 33, 0xffffffff, 1.05f, 1.05f);
						point.x += 20;
						m_texPlayerDataIcon.Render(p2DRender, point, (19 + nJob - 24), 0xffffffff, 1.05f, 1.05f);
					}
					else if (prj.m_aJob[nJob].dwJobType == JTYPE_LEGEND_HERO)
					{
						m_texPlayerDataIcon.Render(p2DRender, point, 33, 0xffffffff, 1.05f, 1.05f);
						point.x += 20;
						m_texPlayerDataIcon.Render(p2DRender, point, (19 + nJob - 32), 0xffffffff, 1.05f, 1.05f);
					}
					else
						m_texPlayerDataIcon.Render(p2DRender, point, 14 + nJob, 0xffffffff, 1.05f, 1.05f);

					point.x += 20;
				}
				else
				{
					if (pMover->GetProp())
					{
						if (pMover->GetProp()->eElementType)
						{
							m_texAttrIcon.Render(p2DRender, point, pMover->GetProp()->eElementType - 1, 255, 1.05f, 1.05f);

							CRect rectEle;
							rectEle = CRect(point.x, point.y, point.x + 20, point.y + 20);
							CPoint ptMouse = GetMousePoint();
							if (rectEle.PtInRect(ptMouse))
							{
								g_toolTip.PutToolTip(m_nIdWnd, "                           ", rectEle, ptMouse, 4); // ?!
								g_toolTip.SetElementTooltip(m_nIdWnd, pMover->GetProp()->eElementType - 1);
							}

							point.x += 20;
						}
					}
				}

				DWORD dwTextColor = COLOR_PLAYER;

				if (pMover->IsNPC())
				{
					dwTextColor = COLOR_MONSTER;
					if (pMover->m_bActiveAttack)
						dwTextColor = 0xffff0000;
				}

				CString strText = pMover->GetName();
				if (!strText.IsEmpty())
				{
					g_WndMng.StringSize(strText, 18);
					p2DRender->TextOut(point.x, point.y, strText, dwTextColor);
				}

				// Level rendering
				BOOL bViewLevel = TRUE;
				if (pMover->GetProp() && pMover->GetProp()->dwHideLevel == 1)
					bViewLevel = FALSE;

				if (bViewLevel && pMover->GetLevel() >= 1)
				{
					if (pMover->IsPlayer() == FALSE)
						strText.Format("%d", pMover->GetLevel());
					else
					{
						if (pMover->IsHero() || pMover->IsLegendHero())
							strText.Format("%d-H", pMover->GetLevel());
						else if (pMover->IsMaster())
							strText.Format("%d-M", pMover->GetLevel());
						else
							strText.Format("%d", pMover->GetLevel());
					}
				}
				else
					strText.Format("1");

				CSize cSize = p2DRender->m_pFont->GetTextExtent(strText);
				point.x = rect.right - cSize.cx - 3;
				p2DRender->TextOut(point.x, point.y, strText, dwTextColor);

				// Normal hp rendering
				//if (!pMover->IsPlayer())
				{
					__int64 nMoverHP = pMover->GetHitPoint();
					if (pMover->GetId() == objRenderOld)
						g_WndMng.MakeNumberSmooth64(&nMoverHP, &m_nMoverHPOld);
					else
						m_nMoverHPOld = nMoverHP;

					strText.Format("HP: %s", Separator(nMoverHP, '.'));
					cSize = p2DRender->m_pFont->GetTextExtent(strText);
					point.x = rect.right - (nWidthClient / 2) - (cSize.cx / 2);
					point.y = rect.top - 3;
					p2DRender->TextOut(point.x, point.y, strText, 0xffffffff);
				}

				// Other
				RenderMoverBuff(pMover, p2DRender);
				p2DRender->SetFont(pOldFont);
				objRenderOld = pMover->GetId();
			}
		}
	}

	pObj = pWorld->GetObjFocus();
	if (pObj && pObj->GetType() != OT_CTRL)
	{
		if (pObj->IsCull() == TRUE)
		{
		}
		else
		{
			if (pObj == pWorld->GetObjFocus())
			{
				CRect rectBound;
				GetBoundRect(pObj, &rectBound);
				RenderFocusObj(pObj, rectBound, D3DCOLOR_ARGB(100, 255, 0, 0), 0xffffff00);
			}
		}
	}

	if (IsValidObjID(GuildHouse->m_dwSelectedObjID))
	{
		CCtrl* pCtrl = prj.GetCtrl(GuildHouse->m_dwSelectedObjID);
		if (pCtrl)
		{
			CRect recBound;
			GetBoundRect(pCtrl, &recBound);
			RenderFocusObj(pCtrl, recBound, D3DCOLOR_ARGB(100, 255, 0, 0), 0xffffff00);
		}
	}

	if (m_bAutoAttack && m_pNextTargetObj && m_pNextTargetObj->GetType() != OT_CTRL)
	{
		if (m_pNextTargetObj->IsCull() == TRUE)
		{
		}
		else
		{
			if (m_pNextTargetObj)
			{
				CRect rectBound;
				GetBoundRect(m_pNextTargetObj, &rectBound);
				RenderFocusObj(m_pNextTargetObj, rectBound, D3DCOLOR_ARGB(100, 255, 0, 0), 0xffffff00);
			}
		}
	}
}

void CWndWorld::RenderFocusObj(CObj* pObj, CRect rect, DWORD dwColor1, DWORD dwColor2)
{
	CPoint pt1, pt2, pt3, pt4;
	pt1 = CPoint(rect.left - 32, rect.top - 32);
	pt2 = CPoint(rect.right, rect.top - 32);
	pt3 = CPoint(rect.left - 32, rect.bottom);
	pt4 = CPoint(rect.right, rect.bottom);

	if (pObj && pObj != m_pNextTargetObj && (m_bSelectTarget || m_bAutoAttack || g_pPlayer->GetCmd() == OBJACT_USESKILL) &&
		pObj->GetType() == OT_MOVER && (((CMover*)pObj)->IsPeaceful() == FALSE || ((CMover*)pObj)->IsPlayer()))
	{
		m_texTarget.GetAt(8)->Render(&g_Neuz.m_2DRender, pt1);
		m_texTarget.GetAt(9)->Render(&g_Neuz.m_2DRender, pt2);
		m_texTarget.GetAt(10)->Render(&g_Neuz.m_2DRender, pt3);
		m_texTarget.GetAt(11)->Render(&g_Neuz.m_2DRender, pt4);
	}
	else
	{
		if (IsValidObjID(GuildHouse->m_dwSelectedObjID) && pObj->GetType() == OT_CTRL)
		{
			if (GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->m_iMode)
			{
				m_texTarget.GetAt(8)->Render(&g_Neuz.m_2DRender, pt1);
				m_texTarget.GetAt(9)->Render(&g_Neuz.m_2DRender, pt2);
				m_texTarget.GetAt(10)->Render(&g_Neuz.m_2DRender, pt3);
				m_texTarget.GetAt(11)->Render(&g_Neuz.m_2DRender, pt4);

			}
			else
			{
				m_texTarget.GetAt(0)->Render(&g_Neuz.m_2DRender, pt1);
				m_texTarget.GetAt(1)->Render(&g_Neuz.m_2DRender, pt2);
				m_texTarget.GetAt(2)->Render(&g_Neuz.m_2DRender, pt3);
				m_texTarget.GetAt(3)->Render(&g_Neuz.m_2DRender, pt4);
			}
		}
		else
		{
			m_texTarget.GetAt(0)->Render(&g_Neuz.m_2DRender, pt1);
			m_texTarget.GetAt(1)->Render(&g_Neuz.m_2DRender, pt2);
			m_texTarget.GetAt(2)->Render(&g_Neuz.m_2DRender, pt3);
			m_texTarget.GetAt(3)->Render(&g_Neuz.m_2DRender, pt4);
		}
	}
}

void CWndWorld::RenderFocusArrow(CPoint pt)
{
	CWorld* pWorld = g_WorldMng.Get();
	CObj* pObj;

	pObj = pWorld->GetObjFocus();
	if (pObj == NULL)	return;

	BOOL bAdjust = FALSE;
	BOOL bAdjust2 = FALSE;
	CPoint	ptOut;
	CRect rectBound;
	if (g_pPlayer->m_pActMover->IsFly())
	{
		if (pObj->GetType() == OT_MOVER)
			bAdjust = TRUE;
	}
	if (bAdjust)
	{
		GetBoundRect(pObj, &rectBound);
		int nWidth = (rectBound.right - rectBound.left);
		int nHeight = (rectBound.bottom - rectBound.top);

		ptOut = CPoint(rectBound.left + nWidth / 2, rectBound.top + nHeight / 2);
		if ((int)ptOut.x > (int)g_Neuz.GetCreationWidth())
		{
			bAdjust2 = TRUE;
			ptOut.x = (LONG)g_Neuz.GetCreationWidth() - 32;
		}
		if ((int)ptOut.x < 0)
		{
			bAdjust2 = TRUE;
			ptOut.x = 32;
		}
		if ((int)ptOut.y > (int)g_Neuz.GetCreationHeight())
		{
			bAdjust2 = TRUE;
			ptOut.y = (int)g_Neuz.GetCreationHeight() - 32;
		}
		if ((int)rectBound.top < 0)
		{
			bAdjust2 = TRUE;
			ptOut.y = 32;
		}
	}

	if (bAdjust2 == FALSE)
		return;

	pt = ptOut;

	D3DXVECTOR2	vC;
	vC.x = (FLOAT)g_Neuz.GetCreationWidth() / 2;
	vC.y = (FLOAT)g_Neuz.GetCreationHeight() / 2;
	FLOAT xDist = pt.x - vC.x;
	FLOAT yDist = pt.y - vC.y;
	FLOAT fTheta = atan2(yDist, xDist);

	m_texTargetArrow.GetAt(0)->RenderRotate(&g_Neuz.m_2DRender, pt, fTheta);

	D3DXVECTOR2 vOut, vPt;

	vPt.x = (FLOAT)(pt.x);
	vPt.y = (FLOAT)(pt.y);

}

void CWndWorld::RenderGauFlight(C2DRender* p2DRender)
{
	if (g_pPlayer && !g_pPlayer->IsVisible())
		return;
}


void CWndWorld::RenderAltimeter()
{
	return;

	CRect rect = GetClientRect();

	if (m_nWinSize == WSIZE_MAX)
	{
		int nHeight = rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		rect.top += 130;
		rect.bottom -= 135;
	}
	else
	{
		//int nHeight =  rect.Height() * 60 / 100;
		rect.left = rect.right - 30;
		//rect.bottom = rect.top + nHeight;
	}
	g_Neuz.m_2DRender.RenderFillRect(rect, D3DCOLOR_ARGB(0x30, 0, 0, 0));
	rect.top += 10;
	rect.bottom -= 10;

	FLOAT fHigh1 = -1.0f, fLow1 = 999999.0f;
	FLOAT fHigh2 = -1.0f, fLow2 = 999999.0f;
	CObj* pObj;
	CLandscape* pLand;
	//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND(g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE)
	{
		FOR_OBJ(pLand, pObj, OT_MOVER)
		{
			D3DXVECTOR3 vPos = pObj->GetPos();
			if (fHigh1 < vPos.y) fHigh1 = vPos.y;
			if (fLow1 > vPos.y) fLow1 = vPos.y;
		}
		END_OBJ
	}
	END_LAND

		if (fHigh1 == -1.0f && fLow1 == 999999.0f)
			return;
	fHigh2 = fHigh1;
	fLow2 = fLow1;
	if (m_fHigh == -1.0f)
	{
		m_fHigh = fHigh2 = fHigh1;
		m_fLow = fLow2 = fLow1;
	}
	else
	{
		if (fHigh1 < m_fHigh)
		{
			fHigh2 = m_fHigh - 2;
			if (fHigh2 < fHigh1)
				fHigh2 = fHigh1;
		}
		else
			if (fHigh1 > m_fHigh)
			{
				fHigh2 = m_fHigh + 2;
				if (fHigh2 > fHigh1)
					fHigh2 = fHigh1;
			}
		if (fLow1 > m_fLow)
		{
			fLow2 = m_fLow + 2;
			if (fLow2 > fLow1)
				fLow2 = fLow1;
		}
		else
			if (fLow1 < m_fLow)
			{
				fLow2 = m_fLow - 2;
				if (fLow2 < fLow1)
					fLow2 = fLow1;
			}
	}
	//	CWorld* pWorld = &g_World;
	CWorld* pWorld = g_WorldMng.Get();

	//	FOR_LAND( &g_World, pLand, g_World.m_nVisibilityLand, FALSE )
	FOR_LAND(g_WorldMng.Get(), pLand, g_WorldMng.Get()->m_nVisibilityLand, FALSE)
	{
		FOR_OBJ(pLand, pObj, OT_MOVER)
		{
			CMover* pMover = (CMover*)pObj;
			if (!pMover || pMover->IsMode(TRANSPARENT_MODE))
				continue;
			DWORD dwColor = 0xffffffff;
			if (pObj == g_pPlayer)
				dwColor = 0xffffff00;

			D3DXVECTOR3 vPos = pObj->GetPos();
			vPos.y -= fLow2;
			// ( fHigh - fLow ) : vPos.z = rect.Height() : z
			int y = (int)(vPos.y * (FLOAT)rect.Height() / (fHigh2 - fLow2));
			g_Neuz.m_2DRender.RenderLine(CPoint(rect.left + 7, rect.bottom - y), CPoint(rect.right, rect.bottom - y), dwColor);
			int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
			CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent(pMover->GetName());
			size.cx += 5;
			g_Neuz.m_2DRender.TextOut(rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000);
			g_Neuz.m_2DRender.TextOut(rect.left - size.cx, rect.bottom - y - nFontMid, pMover->GetName(), dwColor);
			g_Neuz.m_2DRender.RenderFillTriangle(
				CPoint(rect.left, rect.bottom - y - 5),
				CPoint(rect.left, rect.bottom - y + 5),
				CPoint(rect.left + 5, rect.bottom - y),
				dwColor);
		}
		END_OBJ
	}
	END_LAND

		pObj = pWorld->GetObjFocus();
	if (pObj)
	{
		DWORD dwColor = dwColor = 0xffff0000;
		CMover* pMover = (CMover*)pObj;
		D3DXVECTOR3 vPos = pObj->GetPos();
		vPos.y -= fLow2;
		// ( fHigh - fLow ) : vPos.z = rect.Height() : z
		int y = (int)(vPos.y * (FLOAT)rect.Height() / (fHigh2 - fLow2));
		g_Neuz.m_2DRender.RenderLine(CPoint(rect.left + 7, rect.bottom - y), CPoint(rect.right, rect.bottom - y), dwColor);
		int nFontMid = g_Neuz.m_2DRender.m_pFont->GetMaxHeight() / 2;
		CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent(pMover->GetName());
		size.cx += 5;
		g_Neuz.m_2DRender.TextOut(rect.left - size.cx + 1, rect.bottom - y + 1 - nFontMid, pMover->GetName(), 0xff000000);
		g_Neuz.m_2DRender.TextOut(rect.left - size.cx, rect.bottom - y - nFontMid, pMover->GetName(), dwColor);
		g_Neuz.m_2DRender.RenderFillTriangle(
			CPoint(rect.left, rect.bottom - y - 5),
			CPoint(rect.left, rect.bottom - y + 5),
			CPoint(rect.left + 5, rect.bottom - y),
			dwColor);
	}
	FLOAT y = (FLOAT)rect.Height() / (fHigh2 - fLow2);
	for (FLOAT j = 0; j < rect.Height(); j += y)
	{
		g_Neuz.m_2DRender.RenderLine(CPoint(rect.left + 15, (int)(rect.bottom - j)), CPoint(rect.right, (int)(rect.bottom - j)), 0x80ffffff);
	}
	m_fHigh = fHigh2;
	m_fLow = fLow2;
}


void CWndWorld::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	AddWndStyle(WBS_THICKFRAME);

	m_strTexture = "WndTile200.tga";

	SetSizeMax();
	CRect rectClient = GetClientRect();

	m_wndMenuMover.CreateMenu(this);

	m_texTarget.LoadScript(D3DDEVICE, MakePath(DIR_ICON, "icon_target.inc"));
	m_texTargetFly.LoadScript(D3DDEVICE, MakePath(DIR_ICON, "icon_FlightTargetB.inc"));
	m_texTargetArrow.LoadScript(D3DDEVICE, MakePath(DIR_ICON, "icon_FlightTargetArrow.inc"));
	m_texTargetArrow.GetAt(0)->m_ptCenter.x += 32;
	m_texTargetArrow.GetAt(0)->m_ptCenter.y += 32;
	m_texGauFlight.LoadScript(D3DDEVICE, MakePath(DIR_THEME, "Theme_GauFlight.inc"));
	m_texFontDigital.LoadScript(D3DDEVICE, MakePath(DIR_THEME, "Theme_FontDigital1.inc"));

	m_meshArrow.InitDeviceObjects(m_pApp->m_pd3dDevice);
	m_meshArrow.LoadModel("etc_arrow.o3d");
	m_dwRenderArrowTime = 0;

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texLvUp.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "LvUp.bmp"), 0xffff00ff, TRUE);
	m_texLvDn.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "LvDn.bmp"), 0xffff00ff, TRUE);
	m_texLvUp2.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "LvUp2.bmp"), 0xffff00ff, TRUE);
	m_texLvDn2.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "LvDn2.bmp"), 0xffff00ff, TRUE);

	int i = 0;
	BUFFSKILL   buffskill;
	for (i = 0; i < prj.m_aPropAddSkill.GetSize(); i++)
	{
		AddSkillProp* pAddSkill = prj.m_aPropAddSkill.GetAt(i);

		if (!pAddSkill)
			continue;

		if (pAddSkill->dwSkillTime == -1)
			continue;

		ItemProp* pItem = prj.GetSkillProp(pAddSkill->dwName);
		if (!pItem)
			continue;

		buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ICON, pItem->szIcon), 0xffff00ff);
		m_pBuffTexture[0].insert(make_pair(pItem->dwID, buffskill));
	}

	for (i = 0; i < prj.m_aPartySkill.GetSize(); i++)
	{
		ItemProp* pItemProp = prj.m_aPartySkill.GetAt(i);
		if (!pItemProp)
			continue;

		if (pItemProp->dwSkillTime == -1)
			continue;

		buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ICON, pItemProp->szIcon), 0xffff00ff);
		m_pBuffTexture[1].insert(make_pair(pItemProp->dwID, buffskill));
	}

	for (i = 0; i < prj.m_aPropItem.GetSize(); i++)
	{
		ItemProp* pItemProp = (ItemProp*)prj.m_aPropItem.GetAt(i);
		if (!pItemProp)
			continue;

		if (pItemProp->dwSkillTime != -1
			|| pItemProp->IsGiftBoxItem()
			|| pItemProp->dwItemKind3 == IK3_EGG
			|| pItemProp->dwItemKind3 == IK3_PET
#ifdef	__FL_VENDORS
			|| pItemProp->dwItemKind3 == IK3_VENDOR
#endif
			)
		{
			CString strIcon = pItemProp->szIcon;
			if (pItemProp->dwItemKind3 == IK3_EGG && pItemProp->dwID != II_PET_EGG)
			{
				strIcon.Replace(".", "_00.");
				buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, strIcon), 0xffff00ff);
				m_pBuffTexture[2].insert(make_pair(MAKELONG((WORD)pItemProp->dwID, 0), buffskill));
				strIcon.Replace("0.", "1.");
				buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, strIcon), 0xffff00ff);
				m_pBuffTexture[2].insert(make_pair(MAKELONG((WORD)pItemProp->dwID, 1), buffskill));
				strIcon.Replace("1.", "2.");
				buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, strIcon), 0xffff00ff);
				m_pBuffTexture[2].insert(make_pair(MAKELONG((WORD)pItemProp->dwID, 2), buffskill));
			}
			else
			{
				buffskill.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
				m_pBuffTexture[2].insert(make_pair(pItemProp->dwID, buffskill));
			}
		}
	}

	RefreshLimitBuffCount();

	m_texMsgIcon.LoadScript(m_pApp->m_pd3dDevice, "icon\\icon_IconMessenger.inc");
	m_texAttrIcon.LoadScript(m_pApp->m_pd3dDevice, "icon\\Icon_MonElemantkind.inc");
	m_texPlayerDataIcon.LoadScript(m_pApp->m_pd3dDevice, "icon\\icon_PlayerData.inc");

	for (int j = 0; j < SM_MAX; ++j)
	{
		ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[j]);
		if (pItem)
			m_dwSMItemTexture[j] = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, pItem->szIcon), 0xffff00ff);
	}

	m_wndTitleBar.SetVisible(FALSE);

	g_DialogMsg.ClearVendorObjMsg();

	Projection(D3DDEVICE);

	g_WndMng.m_ChatString = "";

	if (g_pPlayer && g_pPlayer->m_nSkillPoint > 0)
	{
		CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase(APP_SKILL4);
		if (!pWndBase)
		{
			g_nSkillCurSelect = -1;
		}
	}


	g_Neuz.m_NeuzEnemy.Clear();

	if (g_pPlayer->IsMode(MODE_MAILBOX))
	{
		CWndInstantMsg* pWndInstantMsg = g_WndMng.OpenInstantMsg("Mail");
		if (pWndInstantMsg)
		{
			pWndInstantMsg->AddPostMessage(prj.GetText(TID_MAIL_RECEIVE));
			pWndInstantMsg->m_timer.Set(MIN(1));
		}

	}

	m_AdvMgr.Init(this);
	m_mmapGuildCombat_GuildPrecedence.clear();
	m_mmapGuildCombat_PlayerPrecedence.clear();
	m_vecGuildCombat_GuildStatus.clear();

	m_TexGuildWinner.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "TexGuildCombatWinner.bmp"), 0xffff00ff);
	m_TexGuildBest.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "TexGuildCombatBest.bmp"), 0xffff00ff);

#ifdef __FL_TDM
	m_TextureBlueTeam.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "tdm_blue_team.png"), 0xffff00ff);
	m_TextureRedTeam.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "tdm_red_team.png"), 0xffff00ff);
	m_TextureFightTDM.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "tdm_fight_icon.png"), 0xffff00ff);
#endif

#ifdef __FL_CASH_SHOP
	GetAdvMgr()->AddAdvButton(APP_CASH_SHOP);
#endif
}

void CWndWorld::AddGuildPrecedence(int nRate, CString str)
{
	m_mmapGuildCombat_GuildPrecedence.insert(make_pair(nRate, str));
}

void CWndWorld::AddPlayerPrecedence(int nRate, u_long uiPlayer)
{
	m_mmapGuildCombat_PlayerPrecedence.insert(make_pair(nRate, uiPlayer));
}

void CWndWorld::AddGuildStatus(u_long uidPlayer, int nLife, BOOL bJoinReady)
{
	__GUILDRATE GuildRate;
	GuildRate.m_uidPlayer = uidPlayer;
	GuildRate.nLife = nLife;
	GuildRate.bJoinReady = bJoinReady;
	m_vecGuildCombat_GuildStatus.push_back(GuildRate);
}

void CWndWorld::AddGCStatus(u_long uidDefender, u_long uidPlayer, BOOL bWar)
{
	map< u_long, vector<__GCWARSTATE> >::iterator it = m_mapGC_GuildStatus.find(uidDefender);

	__GCWARSTATE gcTemp;

	if (it != m_mapGC_GuildStatus.end())
	{
		gcTemp.m_uidPlayer = uidPlayer;
		gcTemp.m_bWar = bWar;
		it->second.push_back(gcTemp);
	}
	else
	{
		vector<__GCWARSTATE> vecTemp;
		gcTemp.m_uidPlayer = uidPlayer;
		gcTemp.m_bWar = bWar;
		vecTemp.push_back(gcTemp);
		m_mapGC_GuildStatus.insert(map< u_long, vector<__GCWARSTATE> >::value_type(uidDefender, vecTemp));
	}
}

u_long CWndWorld::GetGCStatusDefender(u_long uidDefender)
{
	map< u_long, vector<__GCWARSTATE> >::iterator it = m_mapGC_GuildStatus.find(uidDefender);

	if (it != m_mapGC_GuildStatus.end())
	{
		return it->first;
	}

	return -1;
}

int CWndWorld::IsGCStatusPlayerWar(u_long uidPlayer)
{

	if (m_mapGC_GuildStatus.size() == 0)
		return -2;

	__GCWARSTATE gcTemp;
	map< u_long, vector<__GCWARSTATE> >::iterator i = m_mapGC_GuildStatus.begin();

	while (i != m_mapGC_GuildStatus.end())
	{
		for (int j = 0; j < (int)(i->second.size()); j++)
		{
			gcTemp = i->second[j];

			if (gcTemp.m_uidPlayer == uidPlayer)
			{

				return gcTemp.m_bWar;
			}
		}

		++i;
	}

	return -1;
}

BOOL CWndWorld::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(100, 100, 500, 400);
	SetTitle(_T("World"));
	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME | WBS_MANAGER | WBS_MAXIMIZEBOX, rect, pWndParent, dwWndId);
}
BOOL CWndWorld::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		m_AdvMgr.RunButton(nID);
	}
	return FALSE;

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WTBID_CLOSE:
			return FALSE;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndWorld::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	CCtrl* pFocusObj = (CCtrl*)g_WorldMng()->GetObjFocus();
	CMover* pFocusMover = (CMover*)pFocusObj;
	if (pWndBase == &m_wndMenuMover && pFocusObj)
	{
		switch (nID)
		{

		case MMI_GUILDWAR_APP:
		{
			g_DPlay.SendGuildCombatWindow();
		}
		break;

		case  MMI_GUILDWAR_STATE:
		{
			g_DPlay.SendGCRequestStatusWindow();
		}
		break;

		case MMI_GUILDWAR_CANCEL:
		{
			SAFE_DELETE(g_WndMng.m_pWndGuildWarCancelConfirm);
			g_WndMng.m_pWndGuildWarCancelConfirm = new CWndGuildWarCancelConfirm;
			g_WndMng.m_pWndGuildWarCancelConfirm->Initialize(NULL);
		}
		break;

		case MMI_GUILDWAR_JOIN:
		{
			SAFE_DELETE(g_WndMng.m_pWndGuildWarJoinConfirm);
			g_WndMng.m_pWndGuildWarJoinConfirm = new CWndGuildWarJoinConfirm;
			g_WndMng.m_pWndGuildWarJoinConfirm->Initialize(NULL);
		}
		break;

		case MMI_GUILDCOMBAT_RANKING:
		{
			g_DPlay.SendGCPlayerPoint();
		}
		break;

		case MMI_GUILDCOMBAT_SELECTION:
		{
			g_DPlay.SendGCSelectPlayerWindow();
		}
		break;

		case MMI_GUILDCOMBAT_PENYA_RETURN:
		{
		}
		break;

#if __VER >= 12 // __SECRET_ROOM
		case MMI_SECRET_OFFER:
		{
			g_DPlay.SendSecretRoomTenderOpenWnd();
		}
		break;
		case MMI_SECRET_CANCEL:
		{
			if (!g_WndMng.m_pWndSecretRoomCancelConfirm)
			{
				g_WndMng.m_pWndSecretRoomCancelConfirm = new CWndSecretRoomCancelConfirm;
				g_WndMng.m_pWndSecretRoomCancelConfirm->Initialize();
			}
		}
		break;
		case MMI_SECRET_OFFERSTATE:
		{
			g_DPlay.SendSecretRoomTenderView();
		}
		break;
		case MMI_SECRET_SELECTION:
		{
			g_DPlay.SendSecretRoomLineUpOpenWnd();
		}
		break;
		case MMI_SECRET_BOARD:
		{
			SAFE_DELETE(g_WndMng.m_pWndSecretRoomBoard);
			g_WndMng.m_pWndSecretRoomBoard = new CWndSecretRoomBoard;
			g_WndMng.m_pWndSecretRoomBoard->Initialize();
			g_WndMng.m_pWndSecretRoomBoard->SetString();
		}
		break;
		case MMI_SECRET_ENTRANCE:
		{
			for (int i = 0; i < MAX_KILLCOUNT_CIPHERS; i++)
			{
				m_stKillCountCiphers[i].bDrawMyGuildKillCount = TRUE;
				m_stKillCountCiphers[i].szMyGuildKillCount = '0';
				m_stKillCountCiphers[i].ptPos = CPoint(0, 0);
				m_stKillCountCiphers[i].fScaleX = 1.0f;
				m_stKillCountCiphers[i].fScaleY = 1.0f;
				m_stKillCountCiphers[i].nAlpha = 255;
			}

			if (g_WndMng.m_pWndSecretRoomMsg)
				SAFE_DELETE(g_WndMng.m_pWndSecretRoomMsg);

			g_DPlay.SendSecretRoomEntrance();
		}
		break;
		case MMI_SECRET_TAXRATES_CHECK:
		{
			SAFE_DELETE(g_WndMng.m_pWndSecretRoomCheckTaxRate);
			g_WndMng.m_pWndSecretRoomCheckTaxRate = new CWndSecretRoomCheckTaxRate;
			g_WndMng.m_pWndSecretRoomCheckTaxRate->Initialize(NULL);
		}
		break;
#endif //__SECRET_ROOM
#if __VER >= 11 // __SYS_IDENTIFY
		case MMI_LVREQDOWN_CANCEL:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_LVREQDOWN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndLvReqDown);
			g_WndMng.m_pWndLvReqDown = new CWndLvReqDown;
			//g_WndMng.m_pWndLvReqDown->Initialize();
			g_WndMng.m_pWndLvReqDown->Initialize(&g_WndMng, APP_LVREQDOWN);
		}
		break;

		case MMI_BLESSING_CANCEL:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_CANCEL_BLESSING);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndBlessingCancel);
			g_WndMng.m_pWndBlessingCancel = new CWndBlessingCancel;
			g_WndMng.m_pWndBlessingCancel->Initialize(&g_WndMng, APP_CANCEL_BLESSING);
		}
		break;
#endif	// __SYS_IDENTIFY
#ifdef __JEFF_11
		case MMI_PET_RES:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_PET_RES);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pPetRes);
			g_WndMng.m_pPetRes = new CWndPetRes;
			//g_WndMng.m_pPetRes->Initialize();
			g_WndMng.m_pPetRes->Initialize(&g_WndMng, APP_PET_RES);
		}
		break;
#endif
		case MMI_POST:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);

			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTPOST), NULL, prj.GetTextColor(TID_GAME_TRADENOTPOST));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_POST);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_MAIL_OTHERWORK), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKTRADESELF), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP));
				break;
			}
#if __VER >= 8 // __CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
#if __VER >= 8 //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_4
			g_WndMng.CreateApplet(APP_INVENTORY);

			SAFE_DELETE(g_WndMng.m_pWndPost);
			g_WndMng.m_pWndPost = new CWndPost;
			g_WndMng.m_pWndPost->Initialize(NULL, APP_POST);
		}
		break;
		case MMI_BEAUTYSHOP:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP));
				break;
			}
#if __VER >= 8 // __CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
#if __VER >= 8 // __CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_5

#ifdef __NEWYEARDAY_EVENT_COUPON
			{
				g_WndMng.CreateApplet(APP_INVENTORY);
				SAFE_DELETE(g_WndMng.m_pWndBeautyShop);
				g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
				g_WndMng.m_pWndBeautyShop->Initialize(NULL, APP_BEAUTY_SHOP_EX);
			}
#else //__NEWYEARDAY_EVENT_COUPON
			g_WndMng.CreateApplet(APP_INVENTORY);
			SAFE_DELETE(g_WndMng.m_pWndBeautyShop);
			g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
			g_WndMng.m_pWndBeautyShop->Initialize(NULL, APP_BEAUTY_SHOP_EX);
#endif //__NEWYEARDAY_EVENT_COUPON
		}
		break;
#if __VER >= 8 // __CSC_VER8_4
		case MMI_BEAUTYSHOP_SKIN:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTFACESHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTFACESHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTFACESHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTFACESHOP));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}

#ifdef __NEWYEARDAY_EVENT_COUPON

			{
				g_WndMng.CreateApplet(APP_INVENTORY);
				SAFE_DELETE(g_WndMng.m_pWndFaceShop);
				g_WndMng.m_pWndFaceShop = new CWndFaceShop;
				g_WndMng.m_pWndFaceShop->Initialize(NULL, APP_BEAUTY_SHOP_SKIN);
			}
#else //__NEWYEARDAY_EVENT_COUPON
			g_WndMng.CreateApplet(APP_INVENTORY);
			SAFE_DELETE(g_WndMng.m_pWndFaceShop);
			g_WndMng.m_pWndFaceShop = new CWndFaceShop;
			g_WndMng.m_pWndFaceShop->Initialize(NULL, APP_BEAUTY_SHOP_SKIN);
#endif //__NEWYEARDAY_EVENT_COUPON
		}
		break;
#endif //__CSC_VER8_4
#ifdef __EVE_MINIGAME
		case MMI_KAWIBAWIBO:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_MINIGAMEUSING), NULL, prj.GetTextColor(TID_GAME_MINIGAMEUSING));
				break;
			}

			if (g_pPlayer->m_Inventory.GetAtItemId(II_SYS_SYS_EVE_KAWIBAWIBO))
			{
				if (g_WndMng.m_pWndKawiBawiBoGameConfirm == NULL)
				{
					g_WndMng.m_pWndKawiBawiBoGameConfirm = new CWndKawiBawiBoGameConfirm;
					g_WndMng.m_pWndKawiBawiBoGameConfirm->Initialize();
				}
			}
			else
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_KAWIBAWIBO_STARTGUID1)));
		}
		break;
#endif //__EVE_MINIGAME
#if __VER >= 9 // __CSC_VER9_1
		case MMI_SMELT_MIXJEWEL:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_SMELT_MIXJEWEL);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			g_WndMng.CreateApplet(APP_INVENTORY);

			SAFE_DELETE(g_WndMng.m_pWndSmeltMixJewel);
			g_WndMng.m_pWndSmeltMixJewel = new CWndMixJewel;
			g_WndMng.m_pWndSmeltMixJewel->Initialize();
		}
		break;
		case MMI_SMELT_JEWEL:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_SMELT_JEWEL);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltJewel);
			g_WndMng.m_pWndSmeltJewel = new CWndSmeltJewel;
			g_WndMng.m_pWndSmeltJewel->Initialize();
		}
		break;
		case MMI_SMELT_EXTRACTION:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_SMELT_EXTRACTION);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndExtraction);
			g_WndMng.m_pWndExtraction = new CWndExtraction;
			g_WndMng.m_pWndExtraction->Initialize();
		}
		break;
		case MMI_SMELT_CHANGEGWEAPON:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_UNIQUE);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
			break;
		}
		break;
		case MMI_SMELT_CHANGEUWEAPON:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ULTIMATE);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
			break;
		}
		break;
#if __VER >= 10 //__CSC_VER9_1 -> __LEGEND
		case MMI_LEGEND_SKILLUP:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_HERO_SKILLUP);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndHeroSkillUp);
			g_WndMng.m_pWndHeroSkillUp = new CWndHeroSkillUp;
			g_WndMng.m_pWndHeroSkillUp->Initialize();
		}
		break;
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#if __VER >= 10 // __REMOVE_ATTRIBUTE
		case MMI_ATTRIBUTE:
		{
			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_REMOVE_ATTRIBUTE);
			if (pWndBase)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndRemoveAttribute);
			g_WndMng.m_pWndRemoveAttribute = new CWndRemoveAttribute;
			g_WndMng.m_pWndRemoveAttribute->Initialize();
		}
		break;
#endif //__REMOVE_ATTRIBUTE
#if __VER >= 11 // __PIERCING_REMOVE
		case MMI_SMELT_REMOVE_PIERCING:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_SMELT_REMOVE_PIERCING_EX);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndRemovePiercing);
			g_WndMng.m_pWndRemovePiercing = new CWndRemovePiercing;
			g_WndMng.m_pWndRemovePiercing->Initialize();
		}
		break;
#endif //__PIERCING_REMOVE
#if __VER >= 12 // __HEAVEN_TOWER
		case MMI_HEAVEN_TOWER:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_HEAVEN_TOWER);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_WND_OVERLAPED), NULL, prj.GetTextColor(TID_GAME_WND_OVERLAPED));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndHeavenTower);
			g_WndMng.m_pWndHeavenTower = new CWndHeavenTower;
			g_WndMng.m_pWndHeavenTower->Initialize();
		}
		break;
#endif //__HEAVEN_TOWER
#ifdef __NPC_BUFF
		case MMI_NPC_BUFF:
		{
			g_DPlay.SendNPCBuff(pFocusMover->m_szCharacterKey);
		}
		break;
#endif //__NPC_BUFF
		case MMI_MESSAGE:
			g_WndMng.OpenMessage(pFocusMover->GetName());
			break;
		case MMI_ADD_MESSENGER:
		{
			if (g_WndMng.m_RTMessenger.GetFriend(pFocusMover->m_idPlayer))
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0051)));
			}
			else
			{
				g_DPlay.SendAddFriendReqest(pFocusMover->m_idPlayer);
				CString str;
				str.Format(prj.GetText(TID_GAME_MSGINVATE), pFocusMover->GetName());
				g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE));
			}
		}
		break;
		case MMI_DIALOG: // dialog
		{
			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (pWndBase)
				break;
		}
		g_DPlay.SendScriptDialogReq(((CMover*)pFocusObj)->GetId(), NULL, 0, 0, 0, 0);
		SAFE_DELETE(g_WndMng.m_pWndDialog);
		g_WndMng.m_pWndDialog = new CWndDialog;
		g_WndMng.m_pWndDialog->m_idMover = ((CMover*)pFocusObj)->GetId();
		g_WndMng.m_pWndDialog->Initialize(&g_WndMng, APP_DIALOG_EX);
		break;
#if __VER >= 12 // __LORD
		case MMI_LORD_STATE:
			SAFE_DELETE(g_WndMng.m_pWndLordState);
			g_WndMng.m_pWndLordState = new CWndLordState;
			g_WndMng.m_pWndLordState->Initialize(&g_WndMng);
			break;
		case MMI_LORD_TENDER:
			SAFE_DELETE(g_WndMng.m_pWndLordTender);
			g_WndMng.m_pWndLordTender = new CWndLordTender;
			g_WndMng.m_pWndLordTender->Initialize(&g_WndMng);
			break;
		case MMI_LORD_VOTE:
			SAFE_DELETE(g_WndMng.m_pWndLordVote);
			g_WndMng.m_pWndLordVote = new CWndLordVote;
			g_WndMng.m_pWndLordVote->Initialize(&g_WndMng);
			break;
		case MMI_LORD_EVENT:
			SAFE_DELETE(g_WndMng.m_pWndLordEvent);
			g_WndMng.m_pWndLordEvent = new CWndLordEvent;
			g_WndMng.m_pWndLordEvent->Initialize(&g_WndMng);
			break;
		case MMI_LORD_INFO:
			SAFE_DELETE(g_WndMng.m_pWndLordInfo);
			g_WndMng.m_pWndLordInfo = new CWndLordInfo;
			g_WndMng.m_pWndLordInfo->Initialize(&g_WndMng);
			break;
		case MMI_LORD_RPINFO:
			SAFE_DELETE(g_WndMng.m_pWndLordRPInfo);
			g_WndMng.m_pWndLordRPInfo = new CWndLordRPInfo;
			g_WndMng.m_pWndLordRPInfo->Initialize(&g_WndMng);
			break;
#endif
#if __VER >= 13 // __HOUSING
		case MMI_VISIT_FRIEND:
			SAFE_DELETE(g_WndMng.m_pWndRoomList);
			g_WndMng.m_pWndRoomList = new CWndRoomList;
			g_WndMng.m_pWndRoomList->Initialize(&g_WndMng);
			break;
		case MMI_RETURNTO_WORLD:
			SAFE_DELETE(g_WndMng.m_pWndQuitRoom);
			g_WndMng.m_pWndQuitRoom = new CWndQuitRoom;
			g_WndMng.m_pWndQuitRoom->Initialize(&g_WndMng);
			break;
		case MMI_VISIT_MYROOM:
			if (g_pPlayer)
				g_DPlay.SendHousingVisitRoom(NULL_ID);
			break;
#endif // __HOUSING
#if __VER >= 12 // __PET_0519
		case MMI_PET_AWAK_CANCEL:
			SAFE_DELETE(g_WndMng.m_pWndPetAwakCancel);
			g_WndMng.m_pWndPetAwakCancel = new CWndPetAwakCancel;
			g_WndMng.m_pWndPetAwakCancel->Initialize(&g_WndMng);
			break;
#endif
#ifdef __PET_1024
		case MMI_RENAME_CANCEL:
			g_DPlay.SendClearPetName();
			break;
#endif
		case MMI_QUERYEQUIP:
		{
			if (pFocusMover && pFocusMover->IsPlayer())
			{
				g_DPlay.SendQueryEquip(pFocusMover->GetId());
			}
		}
		break;

		case MMI_TRADE:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);

			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKTRADESELF), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_POST);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_MAIL_OTHERWORK), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}

#if __VER >= 8 // __CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}

			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
#if __VER >= 8 // __CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);

			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				break;
			}

			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_TRADE);
			if (pWndBaseBuf)
			{
#ifndef __THAI_0525VER
				g_WndMng.PutString(prj.GetText(TID_GAME_YETTRADE), NULL, prj.GetTextColor(TID_GAME_YETTRADE));
#endif//__THAI_0525VER
				break;
			}

			pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
#ifndef __THAI_0525VER
				g_WndMng.PutString(prj.GetText(TID_GAME_YETTRADE), NULL, prj.GetTextColor(TID_GAME_YETTRADE));
#endif//__THAI_0525VER
				break;
			}

			if (pFocusMover->IsPlayer())
			{
				g_DPlay.SendConfirmTrade(pFocusMover);
				CString sMessageBuf;
				sMessageBuf.Format(prj.GetText(TID_GAME_TRADEREQUEST), pFocusMover->GetName());
				g_WndMng.PutString(sMessageBuf, NULL, prj.GetTextColor(TID_GAME_TRADEREQUEST));
			}
			else
			{
				if (g_pPlayer->m_vtInfo.GetOther() == NULL)
					g_DPlay.SendOpenShopWnd(pFocusMover->GetId());
			}
			break;
		}
		case MMI_REPAIR:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_TAKEBANK), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBANK));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_TAKETRADE), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKETRADE));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_TAKESHOP), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKESHOP));
				break;
			}
#if __VER >= 8 // __CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}

			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_TAKEBEAUTY), NULL, prj.GetTextColor(TID_GAME_REPAIR_TAKEBEAUTY));
				break;
			}
#if __VER >= 8 //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_5

			pWndBaseBuf = g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_TRADEING), NULL, prj.GetTextColor(TID_GAME_REPAIR_TRADEING));
				break;
			}
			break;
		}
		case MMI_BANKING:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOBANK), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKNOTRADE), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_DROP_ITEM);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
#if __VER >= 8 //__CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
#if __VER >= 8 //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_5
			g_DPlay.SendOpenBankWnd(NULL_ID, 0);
			break;
		}
		case	MMI_GUILDBANKING:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOBANK), NULL, prj.GetTextColor(TID_GAME_TRADENOBANK));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKNOTRADE), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				break;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{

				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				break;
			}

			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				break;
			}
#if __VER >= 8 //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				//Summon Angel Using
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				break;
			}
#endif //__CSC_VER8_5
			pWndBaseBuf = g_WndMng.GetWndBase(APP_DROP_ITEM);
			if (pWndBaseBuf)
				break;

			g_DPlay.SendOpenGuildBankWnd();
			break;
		}
		case MMI_INVITE_PARTY:
		{
			if (pFocusMover->IsPlayer())
				InviteParty(pFocusMover->m_idPlayer);
			break;
		}
		case MMI_INVITE_COMPANY:
		{
			if (pFocusMover->IsPlayer())
				InviteCompany(pFocusMover->GetId());
			break;
		}
		case MMI_TRACE:
			if (g_pPlayer->IsPossibleSetCmd())
			{
				m_objidTracking = pFocusMover->GetId();
				g_pPlayer->SetDestObj(pFocusMover);
			}
			break;
		case MMI_CHEER:
			if (g_pPlayer->m_pActMover->IsFly() == FALSE)
			{
				if (pFocusMover->IsPlayer())
					g_DPlay.SendCheering(pFocusMover->GetId());
			}
			break;
		case MMI_UPGRADE:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_DROP_ITEM);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
				break;
#if __VER >= 8 //__CSC_VER8_4
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
#else
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP);
#endif //__CSC_VER8_4
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_TEST);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMERCIAL_ELEM);
			if (pWndBaseBuf)
				break;
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REMOVE_ELEM);
			if (pWndBaseBuf)
				break;
			SAFE_DELETE(g_WndMng.m_pWndUpgradeBase);
			g_WndMng.m_pWndUpgradeBase = new CWndUpgradeBase;
			g_WndMng.m_pWndUpgradeBase->Initialize(&g_WndMng, APP_TEST);
		}
		break;
#if __VER >= 9	// __PET_0410
		case MMI_PET_FOODMILL:
		{
			if (g_WndMng.m_pWndPetFoodMill)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_NOWUSING), NULL, prj.GetTextColor(TID_GAME_NOWUSING));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndPetFoodMill);
			g_WndMng.m_pWndPetFoodMill = new CWndPetFoodMill;
			g_WndMng.m_pWndPetFoodMill->Initialize();
		}
		break;
		case MMI_PET_STATUS:
		{
			if (g_WndMng.m_pWndPetStatus)
			{
#ifdef __AIO_PETS
				g_Option.m_bHideWindowPet = TRUE;
				g_WndMng.PutString("Window deactivated. It will not be opened in the future until you manually open it by clicking on the pet -> Pet State.");
#endif
				g_WndMng.m_pWndPetStatus->Destroy();
				break;
			}


#ifdef __AIO_PETS
			g_Option.m_bHideWindowPet = FALSE;
			g_WndMng.PutString("Window activated. You can deactivate it again by clicking on the pet -> Pet State, or on close button in window.");
#endif
			SAFE_DELETE(g_WndMng.m_pWndPetStatus);
			g_WndMng.m_pWndPetStatus = new CWndPetStatus;
			g_WndMng.m_pWndPetStatus->Initialize();
		}
		break;
		case MMI_PET_RELEASE:
		{
			g_DPlay.SendPetRelease();
			if (g_WndMng.m_pWndPetStatus)
				g_WndMng.m_pWndPetStatus->Destroy();
		}
		break;
		case MMI_BUFFPET_STATUS:
		{
			if (g_WndMng.m_pWndBuffPetStatus)
			{
#ifdef __AIO_PETS
				g_Option.m_bHideWindowLooter = TRUE;
				g_WndMng.PutString("Window deactivated. It will not be opened in the future until you manually open it by clicking on the pet -> Toggle Buff Pet Status.");
#endif
				g_WndMng.m_pWndBuffPetStatus->Destroy();
				break;
			}

#ifdef __AIO_PETS
			g_Option.m_bHideWindowLooter = FALSE;
			g_WndMng.PutString("Window activated. You can deactivate it again by clicking on the pet -> Toggle Buff Pet Status, or on close button in window.");
#endif
			SAFE_DELETE(g_WndMng.m_pWndBuffPetStatus);
			g_WndMng.m_pWndBuffPetStatus = new CWndBuffPetStatus;
			g_WndMng.m_pWndBuffPetStatus->Initialize();
		}

		break;

		case MMI_BUFFPET_RELEASE:
		{
			CItemElem* pItem = g_pPlayer->GetVisPetItem();
			if (pItem)
				g_DPlay.SendDoUseItem(pItem->m_dwObjId);
		}
		break;
#endif	// __PET_0410
#ifdef __JEFF_11_4
		case MMI_ARENA_ENTER:
			g_DPlay.SendHdr(PACKETTYPE_ARENA_ENTER);
			break;
		case MMI_ARENA_EXIT:
			g_DPlay.SendHdr(PACKETTYPE_ARENA_EXIT);
			break;
#endif	// __JEFF_11_4
#ifdef __SYS_ITEMTRANSY
		case MMI_ITEM_TRANSY:
		{
			SAFE_DELETE(g_WndMng.m_pWndItemTransy);
			g_WndMng.m_pWndItemTransy = new CWndItemTransy;
			g_WndMng.m_pWndItemTransy->Initialize(&g_WndMng);
			g_WndMng.m_pWndItemTransy->Init(NULL, TRUE);
		}
		break;
#endif //__SYS_ITEMTRANSY
		case MMI_TELEPORTER:
		{
			CWndMapEx* pWndMapEx = (CWndMapEx*)g_WndMng.CreateApplet(APP_MAP_EX);
			if (pWndMapEx == NULL)
			{
				break;
			}

			pWndMapEx->SetConstructionMode(CWndMapEx::TELEPORTATION);
			pWndMapEx->InitializeTeleportationInformation(pFocusMover);
		}
		break;
#ifdef __MODEL_CHANGE
		case MMI_LOOKCHANGE:
		{
			SAFE_DELETE(g_WndMng.m_pWndLookChange);
			g_WndMng.m_pWndLookChange = new CWndLookChange();
			g_WndMng.m_pWndLookChange->Initialize();
		}
		break;
#endif
#ifdef __FL_RECYCLE
		case MMI_RECYCLE:
		{
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_RECYCLE);
		}
		break;
#endif
#ifdef __TRADESYS
		default:
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_DIALOG_EVENT);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_NOWUSING), NULL, prj.GetTextColor(TID_GAME_NOWUSING));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndDialogEvent);
			g_WndMng.m_pWndDialogEvent = new CWndDialogEvent;
			g_WndMng.m_pWndDialogEvent->SetMMI(nID);
			g_WndMng.m_pWndDialogEvent->Initialize();
		}
		break;
#endif //__TRADESYS
#if __VER >= 14 // __SMELT_SAFETY
		case MMI_SMELT_SAFETY_GENERAL:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_NORMAL);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);
			break;
		}
		case MMI_SMELT_SAFETY_ACCESSORY:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ACCESSARY);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);

			break;
		}
		case MMI_SMELT_SAFETY_PIERCING:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_PIERCING);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);

			break;
		}
#endif // __SMELT_SAFETY
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
		case MMI_SMELT_SAFETY_ELEMENT:
		{
			if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR16), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR16));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndSmeltSafety);
			g_WndMng.m_pWndSmeltSafety = new CWndSmeltSafety(CWndSmeltSafety::WND_ELEMENT);
			g_WndMng.m_pWndSmeltSafety->Initialize(NULL);

			break;
		}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY

#if __VER >= 15 // __GUILD_HOUSE
		case MMI_GUILDHOUSE_CHARGE:
		{
			BOOL bAuthority = FALSE;
			CGuild* pGuild = g_pPlayer->GetGuild();
			if (pGuild)
				bAuthority = pGuild->IsAuthority(g_pPlayer->m_idPlayer, PF_GUILDHOUSE_UPKEEP);

			if (!bAuthority)
			{
				g_WndMng.PutString(GETTEXT(TID_GAME_GUILDHOUSE_TAX_LEVEL));
				break;
			}

			SAFE_DELETE(g_WndMng.m_pWndUpkeep);
			g_WndMng.m_pWndUpkeep = new CWndGHUpkeep;
			g_WndMng.m_pWndUpkeep->Initialize(&g_WndMng, APP_CONFIRM_BUY_);
		}
		break;

		case MMI_GUILDHOUSE_ENTER:
		{
			g_DPlay.SendGuildHouseEnter();
		}
		break;

		case MMI_GUILDHOUSE_OUT:
		{
			g_DPlay.SendGuildHouseGoOut();
		}
		break;

		case MMI_GUILDHOUSE_SALE:
		{
			g_DPlay.SendBuyGuildHouse();
		}
		break;

#endif  //__GUILD_HOUSE
		case MMI_COLLECTOR_DETAILS:
		{
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_COLLECTING_INFO);
			break;
		}
#ifdef __FL_GW_STATUE_V2
		case MMI_CONFIGURE_BUFF_MVP:
		case MMI_CONFIGURE_BUFF_GUILD:
		{
			CWndStatueConfiguration* pWnd = (CWndStatueConfiguration*)g_WndMng.CreateApplet(APP_STATUE_CONFIGURATION);
			if (pWnd)
			{
				pWnd->SetWndMode(nID == MMI_CONFIGURE_BUFF_MVP ? NPC_TYPE_MVPSTATUE : NPC_TYPE_GUILDLEADERSTATUE);
				pWnd->UpdateStaticParams();
			}

			break;
		}
		case MMI_RECEIVE_BUFF_MVP:
		case MMI_RECEIVE_BUFF_GUILD:
		{
			CWndStatuePurchase* pWnd = (CWndStatuePurchase*)g_WndMng.CreateApplet(APP_STATUE_PURCHASE);
			if (pWnd)
			{
				pWnd->SetWndMode(nID == MMI_RECEIVE_BUFF_MVP ? NPC_TYPE_MVPSTATUE : NPC_TYPE_GUILDLEADERSTATUE);
				pWnd->UpdateStaticParams();
			}

			break;
		}
#endif

		} // switch

		SetFocus();
	}

	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndWorld::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);

	RefreshLimitBuffCount();
}
void CWndWorld::RefreshLimitBuffCount()
{
	int nFormula = (m_rectWindow.Width() / 2) + 130;
	nFormula = (m_rectWindow.Width() - nFormula) / 37;
	m_nLimitBuffCount = nFormula - 1;
}
void CWndWorld::OnSize(UINT nType, int cx, int cy)
{
	if (m_nWinSize != WSIZE_MAX)
	{
		CRect rectWnd = GetWndRect();
		CSize size2(rectWnd.Width(), rectWnd.Height());
		CSize sizeDiv = size2;
		sizeDiv.cx %= 16;
		sizeDiv.cy %= 16;
		size2.cx /= 16; size2.cx *= 16;
		size2.cy /= 16; size2.cy *= 16;
		if (sizeDiv.cx) size2.cx += 16;
		if (sizeDiv.cy) size2.cy += 16;
		rectWnd.bottom = rectWnd.top + size2.cy;
		rectWnd.right = rectWnd.left + size2.cx;
		SetWndRect(rectWnd, FALSE);
	}
	AdjustWndBase();
	m_wndTitleBar.Replace();

	CWndBase::OnSize(nType, cx, cy);
}
LRESULT CWndWorld::WndMsgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

BOOL CWndWorld::UseFocusObj(CCtrl* pFocusObj)
{
	if (!IsValidObj(g_pPlayer) || !g_pPlayer->IsPossibleSetCmd())
		return FALSE;

	if (!IsValidObj(pFocusObj))
		return FALSE;

	if (pFocusObj->GetType() == OT_OBJ)
		return FALSE;

	CWorld* pWorld = g_WorldMng();
	CRect rect = GetClientRect();

	if (pFocusObj->GetType() == OT_ITEM || pFocusObj->GetType() == OT_CTRL)
	{
		if (m_bLButtonDown && !m_bLButtonDowned)
			g_pPlayer->CMD_SetUseObject((CCtrl*)pFocusObj);
		m_bLButtonDowned = m_bLButtonDown;
	}
	else if (pFocusObj->GetType() == OT_MOVER)
	{
		CMover* pMover = (CMover*)pFocusObj;
		MoverProp* pMoverProp = pMover->GetProp();

		CItemElem* pItemElem = g_pPlayer->GetWeaponItem();
		if (pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
		{

		}
		else
		{

			if (pItemElem && pItemElem->GetProp()->dwItemKind3 == IK3_BOW)
			{
				if (!m_bLButtonDown)
					g_pPlayer->CMD_SetRangeAttack(pFocusObj->GetId(), 0);
			}
			else
				g_pPlayer->CMD_SetMeleeAttack(pFocusObj->GetId());
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWndWorld::UseSkillToFocusObj(CCtrl* pFocusObj)
{
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		BOOL bRet = pTaskBar->UseSkillQueue((CCtrl*)pFocusObj);
		LPSKILL pSkill = pTaskBar->GetCurrentSkillQueue();
		if (bRet)
		{
			ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
			if (pItemProp)
			{
				ItemProp* pSkillProp;
				if (pSkill != NULL)
					pSkillProp = prj.GetSkillProp(pSkill->dwSkill);


				if (pItemProp->dwItemKind3 != IK3_STAFF && pItemProp->dwItemKind3 != IK3_CHEERSTICK &&
					pSkillProp != NULL && pSkillProp->dwExeTarget != EXT_SELFCHGPARAMET)
				{
					if (g_Option.m_bAutoAttack)
						m_bAutoAttack = TRUE;
				}
			}
		}
		return bRet;
	}

	return FALSE;
}
CObj* CWndWorld::PickObj(POINT point, BOOL bOnlyNPC)
{
	CWorld* pWorld = g_WorldMng();
	CRect rectClient = GetClientRect();
	CObj* pObj;


	pObj = pWorld->PickObject_Fast(rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView,
		OF_MOVER | OF_ITEM | OF_CTRL, CMover::GetActiveMover(), TRUE, bOnlyNPC);
	if (pObj)
	{
		if (pObj->GetType() == OT_CTRL)
		{
			if (!pWorld->GetObjFocus())
				m_pSelectRenderObj = pObj;
			else
				m_pSelectRenderObj = NULL;
		}
		else if (pObj->GetType() == OT_MOVER)
		{
			DWORD dwClass = ((CMover*)pObj)->GetProp()->dwClass;
			switch (dwClass)
			{
			case RANK_MIDBOSS:
			case RANK_SUPER:
			case RANK_BOSS:
			case RANK_MATERIAL:

				pObj = pWorld->PickObject(rectClient, point, &pWorld->m_matProj, &g_Neuz.m_camera.m_matView, OF_MOVER | OF_ITEM, CMover::GetActiveMover(), NULL, FALSE, bOnlyNPC);
				break;
			}

			if (pObj && pObj->GetType() == OT_MOVER)
			{

				CMover* pSelectMover = (CMover*)pObj;

				if (pSelectMover->IsMode(GCWAR_NOT_CLICK_MODE))
				{
					return NULL;
				}
			}

			if (!pWorld->GetObjFocus())
				m_pSelectRenderObj = pObj;
			else
				m_pSelectRenderObj = NULL;
		}
	}
	return pObj;
}

CObj* CWndWorld::SelectObj(POINT point)
{
	CWorld* pWorld = g_WorldMng();

	BOOL bOnlyNPC = FALSE;
	if (GetAsyncKeyState(VK_TAB) & 0x8000)
		bOnlyNPC = TRUE;

	CObj* pObj = PickObj(point, bOnlyNPC);
	if (pObj)
	{
		if (pObj->GetType() == OT_MOVER)
		{
			if (pWorld->GetID() == WI_WORLD_GUILDWAR)
			{
				if (((CMover*)pObj)->IsDie())
					return NULL;
			}
			if (((CMover*)pObj)->IsMode(TRANSPARENT_MODE))
			{
				return NULL;
			}
		}

		if (!m_bAutoAttack)
			pWorld->SetObjFocus(pObj);
	}
	return pObj;
}

#if __VER >= 11 // __CSC_VER11_2
void CWndWorld::SetNextTarget()
{
	CWorld* pWorld = g_WorldMng();

	if (m_pNextTargetObj && pWorld)
	{
		if (m_pNextTargetObj->GetType() == OT_MOVER)
		{
			if (pWorld->GetID() == WI_WORLD_GUILDWAR)
			{
				if (((CMover*)m_pNextTargetObj)->IsDie())
					return;
			}
			if (((CMover*)m_pNextTargetObj)->IsMode(TRANSPARENT_MODE))
			{
				return;
			}
		}
		pWorld->SetObjFocus(m_pNextTargetObj);
		if (m_pNextTargetObj)
		{
			CObj* pObj = pWorld->GetObjFocus();
			m_pNextTargetObj = NULL;
			CMover* pMover = (CMover*)pObj;
			if (pMover->IsPeaceful() == FALSE)
				m_pRenderTargetObj = pObj;
		}
	}
	m_bAutoAttack = FALSE;
}
#endif //__CSC_VER11_2

CObj* CWndWorld::HighlightObj(POINT point)
{
	BOOL bOnlyNPC = FALSE;
	if (GetAsyncKeyState(VK_TAB) & 0x8000)
		bOnlyNPC = TRUE;
	CObj* pObj = PickObj(point, bOnlyNPC);
	CObj::m_pObjHighlight = pObj;
	if (pObj && pObj->GetType() == OT_ITEM)
	{
		CRect rect;
		GetBoundRect(pObj, &rect);
		ClientToScreen(&point);
		ClientToScreen(&rect);

		if (m_pWndOnSetCursor)
		{
			if (APP_WORLD == m_pWndOnSetCursor->GetWndId())
				g_WndMng.PutToolTip_Item(((CItem*)pObj)->m_pItemBase, point, &rect);
		}
		else
			g_WndMng.PutToolTip_Item(((CItem*)pObj)->m_pItemBase, point, &rect);
	}

#if __VER >= 11 // __CSC_VER11_2
	if (pObj == NULL)
		m_pSelectRenderObj = NULL;
#endif //__CSC_VER11_2
	return pObj;
}



int CWndWorld::GetGaugePower(int* pnValue)
{
	int nTime = GetTickCount() - m_dwPowerTick;
	if (nTime < 0)
	{
		if (pnValue)
			*pnValue = 0;
		return -1;
	}

	nTime = min(nTime, MAX_POWER_GAUGE);
	if (pnValue)
		*pnValue = nTime;

	return MulDiv(nTime, 4, MAX_POWER_GAUGE);
}

void CWndWorld::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_pPlayer == NULL || g_pPlayer->IsDie())
		return;

	if (m_bRButtonDown == FALSE)
		ReleaseCapture();

	CWorld* pWorld = g_WorldMng.Get();
	m_bFlyMove = FALSE;


	CObj* pFocusObj = pWorld->GetObjFocus();
	if (pFocusObj && m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE)
	{
		if (g_pPlayer->IsAttackAble(pFocusObj))
		{
			if (pFocusObj->GetType() == OT_MOVER)
			{
				OBJID idTarget = ((CCtrl*)pFocusObj)->GetId();

				int nPower = GetGaugePower(NULL);
				if (nPower >= 0)
				{
					m_dwPowerTick = GetTickCount();

					ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
					if (pItemProp->dwItemKind3 == IK3_WAND)
						g_pPlayer->CMD_SetMagicAttack(idTarget, nPower);
					else if (pItemProp->dwItemKind3 == IK3_BOW)
						g_pPlayer->CMD_SetRangeAttack(idTarget, nPower);

				}
			}
		}
	}

	if (!CDeployManager::GetInstance()->IsReady())
		ControlPlayer(WM_LBUTTONUP, point);

	m_bSelectTarget = FALSE;
}

void CWndWorld::ShowMoverMenu(CMover* pTarget)
{
	if (!pTarget || pTarget->IsMode(TRANSPARENT_MODE))
		return;

	D3DXVECTOR3 vDist = pTarget->GetPos() - g_pPlayer->GetPos();
	FLOAT fDistSq = D3DXVec3LengthSq(&vDist);
	if ((fDistSq < 20.0f * 20.0f) && (m_bRButtonDown == FALSE || (pTarget->IsPlayer() && m_bRButtonDown == TRUE)))
	{
		g_pPlayer->ClearDest();
		m_wndMenuMover.DeleteAllMenu();
		m_wndMenuMover.SetLargeWidth(0);

		ReleaseCapture();
		BOOL bView = FALSE;

		if (pTarget->IsPlayer())
		{
			if (pTarget->m_vtInfo.IsVendorOpen())
			{
				m_wndMenuMover.AppendMenu(0, MMI_MESSAGE, prj.GetText(TID_GAME_TAGSEND));
				m_wndMenuMover.AppendMenu(0, MMI_ADD_MESSENGER, prj.GetText(TID_MMI_ADD_MESSENGER));
				m_wndMenuMover.AppendMenu(0, MMI_INVITE_PARTY, prj.GetText(TID_MMI_INVITE_PARTY));
				CGuild* pGuild = g_pPlayer->GetGuild();
				if (pGuild)
				{
					CGuildMember* pMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
					if (pMember)
					{
						if (pGuild->m_adwPower[pMember->m_nMemberLv] & PF_INVITATION)
							m_wndMenuMover.AppendMenu(0, MMI_INVITE_COMPANY, prj.GetText(TID_MMI_INVITE_COMPANY));
					}
				}
				m_wndMenuMover.AppendMenu(0, MMI_QUERYEQUIP, prj.GetText(TID_MMI_QUERYEQUIP));
			}
			else
			{
				m_wndMenuMover.AppendMenu(0, MMI_TRADE, prj.GetText(TID_MMI_TRADE));
				m_wndMenuMover.AppendMenu(0, MMI_MESSAGE, prj.GetText(TID_GAME_TAGSEND));
				m_wndMenuMover.AppendMenu(0, MMI_ADD_MESSENGER, prj.GetText(TID_MMI_ADD_MESSENGER));
				m_wndMenuMover.AppendMenu(0, MMI_INVITE_PARTY, prj.GetText(TID_MMI_INVITE_PARTY));

				CGuild* pGuild = g_pPlayer->GetGuild();
				if (pGuild)
				{
					CGuildMember* pMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
					if (pMember)
					{
						if (pGuild->m_adwPower[pMember->m_nMemberLv] & PF_INVITATION)
							m_wndMenuMover.AppendMenu(0, MMI_INVITE_COMPANY, prj.GetText(TID_MMI_INVITE_COMPANY));
					}
				}

				m_wndMenuMover.AppendMenu(0, MMI_TRACE, prj.GetText(TID_MMI_TRACE));
				m_wndMenuMover.AppendMenu(0, MMI_QUERYEQUIP, prj.GetText(TID_MMI_QUERYEQUIP));
				m_wndMenuMover.AppendMenu(0, MMI_CHEER, prj.GetText(TID_MMI_CHEER));
			}
			bView = TRUE;
		}
		else
		{
			LPCHARACTER lpCharacter = pTarget->GetCharacter();
			if (lpCharacter)
			{
				int nCount = 0;
				UINT nMenu = 0;
				for (int j = 0; j < MAX_MOVER_MENU; ++j)
				{
					if (lpCharacter->m_abMoverMenu[j])
					{
						if (nCount == 0)
							nMenu = j;

						++nCount;
						if (1 < nCount)
						{
							break;
						}
					}
				}

				if (1 < nCount)
				{
					for (int i = 0; i < MAX_MOVER_MENU; i++)
					{
						if (lpCharacter->m_abMoverMenu[i])
						{
#ifdef __NPC_ADDMENU2
							if (lpCharacter->m_abMoverMenu2[i] == FALSE)
#endif
							{
								if (i == MMI_GUILDBANKING)
								{
									CGuild* pGuild = g_pPlayer->GetGuild();
									if (pGuild && g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
										m_wndMenuMover.AppendMenu(0, i, prj.GetText(TID_MMI_DIALOG + i));
									bView = TRUE;
								}
								else if (i == MMI_ARENA_ENTER)
								{
									m_wndMenuMover.AppendMenu(0, i, prj.GetText(TID_MMI_DIALOG + i));
									bView = TRUE;
								}
								else
								{
									m_wndMenuMover.AppendMenu(0, i, prj.GetText(TID_MMI_DIALOG + i));
									bView = TRUE;
								}
							}
#ifdef __NPC_ADDMENU2
							else
							{
								m_wndMenuMover.AppendMenu(0, i, lpCharacter->m_abMoverMenuText[i]);
								bView = TRUE;
							}
#endif
						}
					}
				}
				else
				{
					OnCommand(nMenu, 0, &m_wndMenuMover);
				}
			}
		}

#ifdef __FL_GW_STATUE_V2
		if (pTarget->GetNpcType() == NPC_TYPE_MVPSTATUE)
		{
			m_wndMenuMover.AppendMenu(0, MMI_RECEIVE_BUFF_MVP, "Purchase MvP Buff");
			if (pTarget->GetNpcOwner() == g_pPlayer->m_idPlayer)
				m_wndMenuMover.AppendMenu(0, MMI_CONFIGURE_BUFF_MVP, "Configure MvP Statue");

			bView = TRUE;
		}
		else if (pTarget->GetNpcType() == NPC_TYPE_GUILDLEADERSTATUE)
		{
			m_wndMenuMover.AppendMenu(0, MMI_RECEIVE_BUFF_GUILD, "Purchase Guild Buff");
			if (pTarget->GetNpcOwner() == g_pPlayer->m_idPlayer)
				m_wndMenuMover.AppendMenu(0, MMI_CONFIGURE_BUFF_GUILD, "Configure Guild Statue");

			bView = TRUE;
		}
#endif

		if (bView)
		{
			CRect rectBound;
			GetBoundRect(pTarget, &rectBound);
			m_wndMenuMover.Move(CPoint(rectBound.right, rectBound.top));
			m_wndMenuMover.SetVisible(TRUE);
			m_wndMenuMover.SetFocus();
		}
		else
		{
			m_wndMenuMover.SetVisible(FALSE);
		}
	}
	else
	{
		if (g_pPlayer->IsPossibleSetCmd())
			g_pPlayer->SetDestObj(pTarget);
	}
}

void CWndWorld::OnLButtonDown(UINT nFlags, CPoint point)
{
	ClearTracking();

	if (!IsValidObj(g_pPlayer) || g_pPlayer->IsDie())
		return;
	if (g_pPlayer->m_dwMode & DONMOVE_MODE)
		return;
	if (g_pPlayer->GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return;
	if (g_pPlayer->m_vtInfo.VendorIsVendor())
		return;
#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return;
#endif // __S_SERVER_UNIFY
	if (g_WndMng.GetWndBase(APP_VENDOR_REVISION))
		return;

#ifdef __EVE_MINIGAME
	if (g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO))
		return;
#endif

#if __VER >= 9 // __CSC_VER9_1
	if (g_WndMng.GetWndBase(APP_SMELT_JEWEL))
		return;
#endif //__CSC_VER9_1

	if (GetBuffIconRect(II_SYS_SYS_SCR_RETURN, point))
		return;
	if (GetBuffIconRect(II_SYS_SYS_SCR_PARTYSUMMON, point))
		return;

#if __VER >= 9	// __PET_0410
	if (GetBuffIconRect(II_SYS_SYS_SCR_PET_FEED_POCKET, point))
		return;
#endif //__PET_0410

	m_timerLButtonDown.Set(0);

	if (m_timerRButtonDown.GetLeftTime() < 200)
		return;

#if __VER >= 13 // __HOUSING
	if (CDeployManager::GetInstance()->IsReady())
	{
		if (CDeployManager::GetInstance()->IsCollide())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_HOUSING_INSTALL_FAIL01), NULL, prj.GetTextColor(TID_GAME_HOUSING_INSTALL_FAIL01));
			return;
		}
		HOUSINGINFO* phousingInfo;
		phousingInfo = CDeployManager::GetInstance()->EndDeploy();
		g_DPlay.SendHousingReqSetupFurniture(*phousingInfo);
		return;
	}
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	if (GuildDeploy()->IsReady())
	{
		if (GuildDeploy()->IsCollide())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_HOUSING_INSTALL_FAIL01), NULL, prj.GetTextColor(TID_GAME_HOUSING_INSTALL_FAIL01));
			return;
		}

		HOUSING_ITEM* pItem = GuildDeploy()->EndDeploy();
		if (GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->GetMode())
			GuildHouse->Setup(*pItem);
		else
			if (GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->GetMode())
				GuildHouse->Reset(*pItem);
		return;
	}
#endif

	SetCapture();
	m_ptMouseOld = point;
	CWorld* pWorld = g_WorldMng.Get();

	CObj* pSelectOld = pWorld->GetObjFocus();
	CObj* pSelectObj = CObj::m_pObjHighlight;
#if __VER < 11 // __CSC_VER11_2
	m_bAutoAttack = FALSE;
#else //__CSC_VER11_2
	if (m_pNextTargetObj == NULL && pSelectOld == NULL)
		m_bAutoAttack = FALSE;
#endif //__CSC_VER11_2
	g_pPlayer->ClearCmd();
	if (pSelectObj && pSelectObj->GetType() == OT_MOVER)
	{
		CMover* pSelectMover = (CMover*)pSelectObj;
		if (m_bRButtonDown)
		{
			m_bAutoAttack = FALSE;
			pSelectObj = NULL;
		}
		else
		{
			if (g_pPlayer->m_pActMover->IsFly())
			{
#if __VER >= 11 // __CSC_VER11_2
				m_bAutoAttack = FALSE;
#endif //__CSC_VER11_2
				if (pSelectMover->IsNPC())
				{
					if (pSelectMover->IsPeaceful() == FALSE && pSelectMover->IsFlyingNPC() == FALSE)
						pSelectObj = NULL;
				}
				else if (pSelectMover->IsPlayer())
					pSelectObj = NULL;
				else
					pSelectObj = SelectObj(point);
			}
			else
			{
				pSelectObj = SelectObj(point);
#if __VER >= 11 // __CSC_VER11_2
				CMover* pMover = (CMover*)pSelectObj;
				if (pMover && (pMover->IsPlayer() || pMover->IsPeaceful() == FALSE))
					m_pRenderTargetObj = pSelectObj;
#endif //__CSC_VER11_2
				if (pSelectOld && pSelectOld == pSelectObj)
				{
#if __VER >= 11 // __CSC_VER11_2
					m_bAutoAttack = FALSE;
					if (m_pNextTargetObj != NULL)
						m_pNextTargetObj = NULL;
#endif //__CSC_VER11_2


					ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
					if (pItemProp)
					{
						if (pItemProp->dwItemKind3 != IK3_WAND)
						{
							if (g_Option.m_bAutoAttack)
								m_bAutoAttack = TRUE;
						}
					}
				}
#if __VER >= 11 // __CSC_VER11_2
				else if (pSelectOld != NULL && m_bAutoAttack && pSelectOld != pSelectObj)
				{
					m_pNextTargetObj = pSelectObj;
				}
				else
					m_bAutoAttack = FALSE;
#endif //__CSC_VER11_2
			}
		}
	}
	else
	{
		pSelectObj = SelectObj(point);
		m_bAutoAttack = FALSE;
		m_pNextTargetObj = NULL;
		m_pRenderTargetObj = NULL;
	}

	m_dwNextSkill = NEXTSKILL_NONE;

	CWndTaskBar* pTaskBar = (CWndTaskBar*)g_WndMng.m_pWndTaskBar;
	if (pTaskBar->m_nExecute)
	{
		pTaskBar->OnCancelSkill();
		g_DPlay.SendEndSkillQueue();
	}

	if (pSelectObj == NULL)
	{
		m_bFreeMove = TRUE;
		if (g_pPlayer->m_pActMover->IsFly())
		{
			s_bAccKeyed = FALSE;
			m_bFlyMove = TRUE;
		}
	}
	else
	{
		if (pSelectObj->GetType() == OT_MOVER)
		{
			CMover* pMover = (CMover*)pSelectObj;
#ifndef __FL_VENDORS
			if (pMover->IsPlayer() && !pMover->IsActiveMover()
				&& pMover->m_vtInfo.IsVendorOpen()
				&& CMover::GetActiveMover()->m_vtInfo.GetOther() == NULL
				&& !(GetAsyncKeyState(VK_MENU) & 0x8000))
#else
			if (!pMover->IsActiveMover()
				&& pMover->m_vtInfo.IsVendorOpen()
				&& CMover::GetActiveMover()->m_vtInfo.GetOther() == NULL
				&& !(GetAsyncKeyState(VK_MENU) & 0x8000))
#endif
			{
				if (pMover->IsRegionAttr(RA_SAFETY))
				{
					g_DPlay.SendQueryPVendorItem(pMover->GetId());
				}
				else if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) == FALSE)
				{
					g_DPlay.SendQueryPVendorItem(pMover->GetId());
				}
			}
			else if (pMover->IsPlayer() && FALSE == pMover->IsActiveMover() && g_bKeyTable[g_Neuz.Key.chTrace])
			{
				if (g_pPlayer->IsPossibleSetCmd())
				{
					m_objidTracking = pMover->GetId();
					g_pPlayer->SetDestObj(pMover);
				}
			}
			else
			{
				PLAYSND(pMover->GetProp()->dwSndIdle1);
				if (pMover->IsPeaceful() && !g_pPlayer->IsFly() &&
					(pMover->IsPlayer() == FALSE || (pMover->IsPlayer() && (GetAsyncKeyState(VK_MENU) & 0x8000))))
				{
					m_bAutoAttack = FALSE;
					ShowMoverMenu(pMover);
				}
			}

			m_bSelectTarget = TRUE;
			if (pSelectObj != pSelectOld)
				m_timerFocusAttack.Reset();
			m_dwPowerTick = ::GetTickCount();
		}
		else if (pSelectObj->GetType() == OT_ITEM)
		{
			m_bSelectTarget = TRUE;
		}
		else if (pSelectObj->GetType() == OT_CTRL)
		{
			m_bSelectTarget = TRUE;
			ShowCCtrlMenu((CCtrl*)pSelectObj);
		}
	}

	ControlPlayer(WM_LBUTTONDOWN, point);
}

void CWndWorld::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (m_dwNextSkill == NEXTSKILL_NONE)
		ControlPlayer(WM_RBUTTONUP, point);

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	if (point == m_ptMouseSpot && MenuException(point) == TRUE)
	{
		CObj* pSelectObj = CObj::m_pObjHighlight;
		CMover* pMover = (CMover*)pSelectObj;
		if (IsValidObj(pSelectObj) && pSelectObj->GetType() == OT_MOVER && pMover->IsPlayer())
		{
			m_bAutoAttack = FALSE;
			m_bSelectTarget = TRUE;
			g_WorldMng()->SetObjFocus(pSelectObj);
			ShowMoverMenu(pMover);
		}
	}
#endif // __IMPROVE_SYSTEM_VER15
	CMover* pMover = CMover::GetActiveMover();

	if (g_pPlayer && g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock())
		g_Neuz.m_camera.Unlock();
}
void CWndWorld::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsFullMax() && m_wndTitleBar.GetWindowRect(TRUE).PtInRect(point))
		return;
	if (g_pPlayer == NULL)
		return;
	if (CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
		return;

#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return;
#endif // __S_SERVER_UNIFY

	//	m_bAutoAttack = FALSE;
	m_ptMouseSpot = point;
	SetCapture();

	m_timerRButtonDown.Set(0);;
	if (m_timerLButtonDown.GetLeftTime() < 200)
		return;
}



void CWndWorld::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
#ifndef __VRCAMERA
	if (g_pPlayer->m_pActMover->IsFly() && !g_Neuz.m_camera.IsLock())
		g_Neuz.m_camera.Unlock();
#endif
}
void CWndWorld::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsFullMax() && m_wndTitleBar.GetWindowRect(TRUE).PtInRect(point))
		return;
#ifndef __VRCAMERA
	SetCapture();
	m_ptMouseOld = point;
#endif
}

BOOL CWndWorld::GetBuffIconRect(DWORD dwID, const CPoint& point)
{
	vector <BUFFICONRECT_INFO>::iterator it = m_rcCheck.begin();
	for (; it != m_rcCheck.end(); ++it)
	{
		BUFFICONRECT_INFO& info = *it;
		if (PtInRect(&info.rc, point) == TRUE && info.dwID == dwID)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CWndWorld::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ClearTracking();

	if (!CDeployManager::GetInstance()->IsReady())
		ControlPlayer(WM_LBUTTONDBLCLK, point);
	CRect rect = GetClientRect();
	CWorld* pWorld = g_WorldMng.Get();
	CObj* pFocusObj = g_WorldMng.Get()->GetObjFocus();
	if (pFocusObj && CObj::m_pObjHighlight == pFocusObj)
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp();
		if (pItemProp)
		{
			if (pItemProp->dwItemKind3 != IK3_WAND)
			{
				m_bAutoAttack = TRUE;
			}
		}
	}

	if (GetBuffIconRect(II_SYS_SYS_SCR_RETURN, point))
		g_DPlay.SendReturnScroll(-1);

	if (GetBuffIconRect(II_SYS_SYS_SCR_PARTYSUMMON, point))
	{
		if (g_Neuz.m_dwSummonPartyObjid == 0 || g_pPlayer->GetId() == g_Neuz.m_dwSummonPartyObjid)
			return;

		if (!g_WndMng.m_pWndSummonPartyUse)
		{
			g_WndMng.m_pWndSummonPartyUse = new CWndSummonPartyUse;
			g_WndMng.m_pWndSummonPartyUse->Initialize(&g_WndMng);
		}
	}

	if (GetBuffIconRect(II_SYS_SYS_SCR_PET_FEED_POCKET, point))
		g_DPlay.SendFeedPocketInactive();
}

void CWndWorld::OnMButtonDblClk(UINT nFlags, CPoint point)
{

}
void CWndWorld::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if (!g_pPlayer->m_pActMover->IsFly())
		g_Neuz.m_camera.m_fRotx = -g_pPlayer->GetAngle();
}
BOOL CWndWorld::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (CWorld::m_nZoomLevel != 0)
		return FALSE;

	if (CDeployManager::GetInstance()->IsReady())
		return FALSE;

	// zoom
	if (zDelta < 0)
	{
		g_Neuz.m_camera.m_fZoom -= 0.75f;

		if (g_Neuz.m_camera.m_fZoom < -1.5f)
			g_Neuz.m_camera.m_fZoom = -1.5f;
	}
	else
	{
		g_Neuz.m_camera.m_fZoom += 0.75f;

		if (g_Option.m_bZoomLimit)
		{
			if (g_Neuz.m_camera.m_fZoom > 7)
				g_Neuz.m_camera.m_fZoom = 7;
		}
		else
		{
			if (g_Neuz.m_camera.m_fZoom > 15)
				g_Neuz.m_camera.m_fZoom = 15;
		}
	}

	return TRUE;
}
void CWndWorld::OnMouseMove(UINT nFlags, CPoint point)
{
	if (g_pPlayer == NULL)
		return;

	CPoint pt2 = point;
	if (m_bRButtonDown)
	{
		SetCapture();
		CMover* pPlayer = CMover::GetActiveMover();
		if (pPlayer->m_pActMover->IsFly() /*&& m_bMButtonDown == FALSE*/)
		{
			int nWidth = (int)g_Neuz.GetCreationWidth();
			int nHeight = (int)g_Neuz.GetCreationHeight();
			int nLeft = 0, nTop = 0;
			if (g_Neuz.IsWindow())
			{
				RECT rrr;
				::GetWindowRect(g_Neuz.GetSafeHwnd(), &rrr);
				nLeft = rrr.left;		nTop = rrr.top;

			}
			g_Neuz.m_pd3dDevice->ShowCursor(FALSE);
			ControlFlying(WM_MOUSEMOVE, point);
			if ((int)point.x > nWidth - 10 || point.x < 10)
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition(nLeft + nWidth / 2, nTop + point.y, 0);
				point.x = nWidth / 2;
			}
			if (point.y < 10 || point.y > nHeight - 10)
			{
				g_Neuz.m_pd3dDevice->SetCursorPosition(nLeft + point.x, nTop + nHeight / 2, 0);
				point.y = nHeight / 2;
			}
		}
		else
			g_Neuz.m_pd3dDevice->ShowCursor(TRUE);

	}
	else
		g_Neuz.m_pd3dDevice->ShowCursor(TRUE);



	BOOL bCamera = FALSE;
	BOOL bLockCamera = FALSE;

	if (g_pPlayer->m_pActMover->IsFly())
	{
		if (m_bMButtonDown)
			bCamera = TRUE;
	}
	else
	{
		if (m_bRButtonDown)
			bCamera = TRUE;
	}

	if (bCamera)
	{
		FLOAT fRotSpeed = 0.2f;
		g_Neuz.m_camera.m_fRotx += ((point.x - m_ptMouseOld.x) * fRotSpeed);


		if (g_Neuz.m_camera.m_fRotx > 360) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
		if (g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;

		g_Neuz.m_camera.m_fRoty += ((point.y - m_ptMouseOld.y) * fRotSpeed);
		if (g_Neuz.m_camera.m_fRoty > 80 - g_Neuz.m_camera.m_fZoom * 4)
			g_Neuz.m_camera.m_fRoty = 80 - g_Neuz.m_camera.m_fZoom * 4;
		if (g_Neuz.m_camera.m_fRoty < -80)
			g_Neuz.m_camera.m_fRoty = -80;


	}

	m_ptMouseOld = point;
}

void CWndWorld::OnMouseWndSurface(CPoint point)
{

}


void CWndWorld::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
#ifdef __CLIENT
	CMover* pMover = CMover::GetActiveMover();

	if (m_bCtrlPushed && nChar == 'F')
		m_bRenderFPS = !m_bRenderFPS;

	if (nChar == VK_CONTROL && m_bCtrlPushed == TRUE)
		m_bCtrlPushed = FALSE;

	if (nChar == VK_SHIFT && m_bShiftPushed != FALSE)
		m_bShiftPushed = FALSE;

	if (nChar == VK_SCROLL)
	{
		CWorld::m_nZoomLevel++;

		if (CWorld::m_nZoomLevel > 3)
		{
			CWorld::m_nZoomLevel = 0;
		}

		if (CWorld::m_nZoomLevel != 0)
		{
			g_Neuz.m_camera.m_fZoom = 0.0f;
			CString strCameraZoomText = _T("");
			strCameraZoomText.Format(prj.GetText(TID_GAME_CAMERA_ZOOM_LEVEL), CWorld::m_nZoomLevel); // %dX Zoom
			g_WndMng.PutString(strCameraZoomText, NULL, 0xff00ff00);
		}
	}

	CWorld* pWorld = g_WorldMng.Get();
	if (nChar == 'C')
	{
		CObj* pTargetObj = pWorld->GetObjFocus();
		if (g_pPlayer->m_pActMover->IsFly() == FALSE)
		{
			if (pTargetObj)
			{
				CMover* pMover = (CMover*)pTargetObj;
				if (pMover->GetType() == OT_MOVER)
					m_dwNextSkill = NEXTSKILL_ACTIONSLOT;
			}
			else
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;
		}
	}

#endif //__CLIENT

}
void CWndWorld::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'A' || nChar == 'S' || nChar == 'D' || nChar == 'W')
		m_bAutoAttack = FALSE;

	if (nChar == VK_CONTROL)
		m_bCtrlPushed = TRUE;

	if (nChar == VK_SHIFT)
		m_bShiftPushed = TRUE;

	if (nChar == VK_TAB)
	{
		if (g_pPlayer)
			g_pPlayer->DoSelectNearestPlayer();
	}
}

BOOL CWndWorld::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (g_WndMng.GetWndBase(APP_TEST))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM));
		SetForbid(TRUE);
		return FALSE;
	}

	if (g_WndMng.GetWndBase(APP_SHOP_) ||
		g_WndMng.GetWndBase(APP_GUILD_MERIT1) ||
		g_WndMng.GetWndBase(APP_GUILD_BANK) ||
		g_WndMng.GetWndBase(APP_COMMON_BANK) ||
		g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX) ||
		g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN) ||
		g_WndMng.GetWndBase(APP_SUMMON_ANGEL) ||
		g_WndMng.GetWndBase(APP_TRADE))
	{
		if (pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_TRADELIMITITEM), NULL, prj.GetTextColor(TID_GAME_TRADELIMITITEM));

			SetForbid(TRUE);
			return FALSE;
		}
	}
	else if (g_WndMng.GetWndBase(APP_REPAIR))
	{
		if (pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOTDROP), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOTDROP));

			SetForbid(TRUE);
			return FALSE;
		}
	}


	if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
		CRect rect = GetClientRect();
		CWorld* pWorld = g_WorldMng.Get();
		D3DXVECTOR3 vPos, vOut;
		pWorld->ClientPointToVector(NULL, rect, point, &pWorld->m_matProj, &pWorld->m_pCamera->m_matView, &vPos, FALSE);
		vOut = vPos - g_pPlayer->GetPos();
		D3DXVec3Normalize(&vPos, &vOut);
		vPos += g_pPlayer->GetPos();
		vPos.y += 1000.0f;
		FLOAT fHeight = pWorld->GetFullHeight(vPos);
		if (fabs(fHeight - g_pPlayer->GetPos().y) > 1.0f)
			vPos = g_pPlayer->GetPos();
		else
			vPos.y = fHeight + 0.2f;

		if (pWndFrame && pWndFrame->GetWndId() == APP_INVENTORY)
		{
			PLAYSND(SND_INF_GROUNDDROP);
			if (pShortcut->m_dwData == 0)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_CANNOT_DROPMONEY), NULL, prj.GetTextColor(TID_GAME_CANNOT_DROPMONEY));
				SetForbid(TRUE);
				return FALSE;
			}
			else
			{
				CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
				if (pItemBase == NULL)
					return FALSE;

				if (IsUsingItem(pItemBase))
				{
					SetForbid(TRUE);
					return FALSE;
				}

				CItemElem* pItemElem = (CItemElem*)pShortcut->m_dwData;
				if (pItemElem->IsUndestructable() == TRUE)
				{
					g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM), NULL, prj.GetTextColor(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM));
					return FALSE;
				}

				if (pItemElem->IsQuest())
				{
					SAFE_DELETE(g_WndMng.m_pWndQuestItemWarning);
					g_WndMng.m_pWndQuestItemWarning = new CWndQuestItemWarning;
					g_WndMng.m_pWndQuestItemWarning->m_pItemElem = pItemElem;
					g_WndMng.m_pWndQuestItemWarning->m_vPos = vPos;
					g_WndMng.m_pWndQuestItemWarning->Initialize(NULL, APP_QUEITMWARNING);
				}
				else
				{
					if (pItemElem->m_nItemNum == 1)
					{
						SAFE_DELETE(g_WndMng.m_pWndDropConfirm);
						g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
						g_WndMng.m_pWndDropConfirm->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropConfirm->m_vPos = vPos;
						g_WndMng.m_pWndDropConfirm->Initialize(NULL, APP_DROP_CONFIRM);

						if (g_eLocal.GetState(EVE_DROPITEMREMOVE))
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropConfirm->GetDlgItem(WIDC_STATIC1));
							pWndStatic->SetTitle(prj.GetText(TID_GAME_DROPITEMREMOVE));
						}
					}
					else
					{
						SAFE_DELETE(g_WndMng.m_pWndDropItem);
						g_WndMng.m_pWndDropItem = new CWndDropItem;
						g_WndMng.m_pWndDropItem->m_pItemElem = pItemElem;
						g_WndMng.m_pWndDropItem->m_vPos = vPos;
						g_WndMng.m_pWndDropItem->Initialize(NULL, APP_DROP_ITEM);

						if (g_eLocal.GetState(EVE_DROPITEMREMOVE))
						{
							CWndStatic* pWndStatic = (CWndStatic*)(g_WndMng.m_pWndDropItem->GetDlgItem(WIDC_STATIC1));
							pWndStatic->SetTitle(prj.GetText(TID_GAME_DROPITEMREMOVE2));
						}
					}
				}

				return TRUE;
			}
		}
		else
		{
			if (pWndFrame && APP_BUFFPET_STATUS == pWndFrame->GetWndId())
			{
				CWndBuffPetStatus* pWnd = (CWndBuffPetStatus*)pWndFrame;
				pWnd->DoModal_ConfirmQuestion(0, 0, pShortcut->m_dwIndex, pShortcut->m_dwData, CWndConfirmVis::CVS_UNEQUIP_VIS);

				return FALSE;
			}

		}
	}
	SetForbid(TRUE);
	return FALSE;
}

void ProcessQuestEmoticon()
{

	if (CMover::m_bQuestEmoticonAdd)
	{
		CMover::m_fQuestEmoticonScale += 0.01f;
		if (CMover::m_fQuestEmoticonScale >= 1.2f)
			CMover::m_bQuestEmoticonAdd = FALSE;
	}
	else
		if (CMover::m_bQuestEmoticonAdd == FALSE)
		{
			CMover::m_fQuestEmoticonScale -= 0.01f;
			if (CMover::m_fQuestEmoticonScale <= 1.0f)
				CMover::m_bQuestEmoticonAdd = TRUE;
		}
}

BOOL CWndWorld::Process()
{
#ifdef __FL_FFA
	ProcessFFA();
#endif
#ifdef __FL_TDM
	ProcessTDM();
#endif

	if (g_pPlayer)
	{
		if (g_Option.m_bCameraLock && !m_bRButtonDown &&
			(g_bKeyTable[g_Neuz.Key.chUp] || g_bKeyTable['S'] || g_bKeyTable[g_Neuz.Key.chLeft] || g_bKeyTable['D']) && g_pPlayer->IsFly() == FALSE)
		{
			g_Neuz.m_camera.m_fRotx = -g_pPlayer->GetAngle();
		}

		::ProcessQuestEmoticon();

		CWorld* pWorld = g_WorldMng();

		if (g_pPlayer->m_pActMover->IsState(OBJSTA_STAND) == FALSE || g_pPlayer->m_pActMover->GetMoveState() == OBJSTA_BMOVE)
		{
#ifndef __AIO
			if (CWndBase::m_GlobalShortcut.IsEmpty() == FALSE)
			{
				CWndBase* pWndFrame = CWndBase::m_GlobalShortcut.m_pFromWnd->GetFrameWnd();

				if (pWndFrame && pWndFrame->GetWndId() != APP_INVENTORY)
					CWndBase::m_GlobalShortcut.Empty();
			}
#endif

			m_wndMenuMover.SetVisible(FALSE);

			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_DIALOG_EX);
			if (pWndBase)
				pWndBase->Destroy();

			if (g_pPlayer->m_vtInfo.GetOther())
			{
				if (g_pPlayer->m_vtInfo.GetOther()->IsPlayer())
					g_DPlay.SendTradeCancel();
				else
				{
					g_DPlay.SendCloseShopWnd();
					g_pPlayer->m_vtInfo.SetOther(NULL);
				}
			}

			pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
			if (pWndBase)
			{
				((CWndInventory*)pWndBase)->BaseMouseCursor();
				if (g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
			}

			pWndBase = g_WndMng.GetWndBase(APP_VENDOR_REVISION);
			if (pWndBase)
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
				if (pWndBase)
					pWndBase->Destroy();
			}

			pWndBase = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBase)
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_TRADE_GOLD);
				if (pWndBase)
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
				if (pWndBase)
					pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBase)
			{
				pWndBase->Destroy();

				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);

				if (pWndBase)
					pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
			if (pWndBase)
			{
				pWndBase->Destroy();

				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);

				if (pWndBase)
					pWndBase->Destroy();
			}
#if __VER >= 8 //__CSC_VER8_5
			pWndBase = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBase)
			{
				pWndBase->Destroy();

				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);

				if (pWndBase)
					pWndBase->Destroy();
			}
#endif //__CSC_VER8_5
#ifdef __EVE_MINIGAME
			if (g_WndMng.m_pWndKawiBawiBoGameConfirm)
				g_WndMng.m_pWndKawiBawiBoGameConfirm->Destroy();

			pWndBase = g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif //__EVE_MINIGAME
#if __VER >= 9 // __CSC_VER9_1
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_MIXJEWEL);
			if (pWndBase)
			{
				pWndBase->Destroy();

				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);

				if (pWndBase)
					pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_JEWEL);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_EXTRACTION);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_CHANGEWEAPON);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_PET_FOODMILL);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_REMOVE_PIERCING_EX);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#ifdef __JEFF_11
			pWndBase = g_WndMng.GetWndBase(APP_PET_RES);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif
#if __VER >= 11 // __SYS_IDENTIFY
			pWndBase = g_WndMng.GetWndBase(APP_LVREQDOWN);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_CANCEL_BLESSING);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif
#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
			pWndBase = g_WndMng.GetWndBase(APP_HERO_SKILLUP);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif //__CSC_VER9_1 -> __LEGEND
#endif //__CSC_VER9_1
#ifdef __TRADESYS
			pWndBase = g_WndMng.GetWndBase(APP_DIALOG_EVENT);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif //__TRADESYS
#if __VER >= 12 // __HEAVEN_TOWER
			pWndBase = g_WndMng.GetWndBase(APP_HEAVEN_TOWER);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif //__HEAVEN_TOWER
#if __VER >= 10 // __REMOVE_ATTRIBUTE
			pWndBase = g_WndMng.GetWndBase(APP_REMOVE_ATTRIBUTE);
			if (pWndBase)
			{
				pWndBase->Destroy();
			}
#endif //__REMOVE_ATTRIBUTE
			if (pWndBase)
			{
				pWndBase->Destroy();

				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);

				if (pWndBase)
					pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBase)
			{
				g_DPlay.SendCloseBankWnd();
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_TRADE_GOLD);
				if (pWndBase)
					pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
				if (pWndBase)
					pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBase)
			{
				pWndBase->Destroy();
				pWndBase = g_WndMng.GetWndBase(APP_INVENTORY);
				if (pWndBase)	pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_DROP_CONFIRM);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_DROP_ITEM);
			if (pWndBase)
				pWndBase->Destroy();

			pWndBase = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBase)
			{
				g_DPlay.SendCloseGuildBankWnd();
				pWndBase->Destroy();
			}
			pWndBase = g_WndMng.GetWndBase(APP_RANK_WAR);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_RANK_INFO);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_RANK_GUILD);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_TEST);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_COMMERCIAL_ELEM);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_REMOVE_ELEM);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_CHANGECLASS_1);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_CHANGECLASS_2);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_POST);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_POST_SEND);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_POST_RECEIVE);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_POST_READ);
			if (pWndBase)
				pWndBase->Destroy();

			pWndBase = g_WndMng.GetWndBase(APP_GUILDCOMBAT_RANK_P);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_GUILDCOMBAT_RANKING);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_GUILD_WAR_STATE);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_CLOSE_EXISTING_CONNECTION);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_GUILDCOMBAT_SELECTION);
			if (pWndBase)
				pWndBase->Destroy();
#if __VER >= 12 // __SECRET_ROOM
			pWndBase = g_WndMng.GetWndBase(APP_SECRETROOM_SELECTION);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_SECRETROOM_OFFER);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_SECRETROOM_OFFERSTATE);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_SECRETROOM_TEXRATE_CHECK);
			if (pWndBase)
				pWndBase->Destroy();
			pWndBase = g_WndMng.GetWndBase(APP_SECRETROOM_BOARD);
			if (pWndBase)
				pWndBase->Destroy();
#endif //__SECRET_ROOM
#if __VER >= 13 // __EXT_ENCHANT
			pWndBase = g_WndMng.GetWndBase(APP_CHANGE_ATTRIBUTE);
			if (pWndBase)
				pWndBase->Destroy();
#endif //__EXT_ENCHANT
#if __VER >= 14 // __SMELT_SAFETY
			pWndBase = g_WndMng.GetWndBase(APP_SMELT_SAFETY);
			if (pWndBase)
				pWndBase->Destroy();
#endif // __SMELT_SAFETY
#ifdef __MODEL_CHANGE
			if (g_WndMng.m_pWndLookChange)
				g_WndMng.m_pWndLookChange->Destroy();
#endif
		}
		g_SoundMng.SetListener(g_pPlayer->GetPos(), g_Neuz.m_camera.m_fCurRotx);
		g_WorldMng()->SetCamera(&g_Neuz.m_camera);
		g_WorldMng()->Process();


		if (g_pPlayer->m_pActMover && !g_pPlayer->m_pActMover->IsFly())
		{
			CObj* pObj = pWorld->GetObjFocus();
			if (m_dwNextSkill == NEXTSKILL_ACTIONSLOT)
			{
				UseSkillToFocusObj((CCtrl*)pObj);
			}
			else if (m_dwNextSkill != NEXTSKILL_NONE)
			{
				g_WndMng.UseSkillShortCut(0, m_dwNextSkill);
			}
			else
			{
				if ((m_bLButtonDown || m_bAutoAttack) && (g_pPlayer->GetCmd() != OBJACT_USESKILL))
				{
					if (m_bAutoAttack || (m_bSelectTarget && m_timerFocusAttack.IsTimeOut() == TRUE))
					{
						if (pObj)
						{

							if (pObj->GetType() == OT_CTRL || pObj->GetType() == OT_MOVER || pObj->GetType() == OT_ITEM)
							{
								if (g_pPlayer->IsAttackAble(pObj))
									UseFocusObj((CCtrl*)pObj);
							}
						}
						else
							ControlPlayer(0, GetMousePoint());
					}
				}
				else
				{

					ControlPlayer(0, GetMousePoint());
					m_bSelectTarget = FALSE;
				}
			}
		}
		else
		{
			CObj* pObj = pWorld->GetObjFocus();
			if (m_bLButtonDown && m_bSelectTarget)
			{
				if (pObj)
				{
					if (pObj->GetType() == OT_ITEM || pObj->GetType() == OT_CTRL)
					{
						if (m_bLButtonDown && !m_bLButtonDowned)
							g_pPlayer->CMD_SetUseObject((CCtrl*)pObj);
						m_bLButtonDowned = m_bLButtonDown;
					}
				}
			}
			else
			{

				ControlPlayer(0, GetMousePoint());
				m_bSelectTarget = FALSE;
			}
		}
		m_bLButtonDowned = m_bLButtonDown;
		int nSize = pWorld->m_aRegion.GetSize();
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		for (int i = 0; i < nSize; i++)
		{
			LPREGIONELEM lpRegionElem = pWorld->m_aRegion.GetAt(i);
			if (!lpRegionElem)
				continue;

			if (lpRegionElem->m_rect.PtInRect(CPoint((int)(vPos.x), (int)(vPos.z))))
			{
				if (lpRegionElem->m_bInside == FALSE)
				{
					if (lpRegionElem->m_dwIdTeleWorld == WI_WORLD_NONE)
					{
						m_dwIdBgmMusic = lpRegionElem->m_dwIdMusic;
						if (!lpRegionElem->m_bDirectMusic || m_dwIdBgmMusic == NULL_ID || m_dwIdBgmMusic == 0)
							SetWorldMusic();

						PlayBGM(m_dwIdBgmMusic);
					}

					lpRegionElem->m_bInside = TRUE;

					BOOL bResult = TRUE;
					if (lpRegionElem->m_dwAttribute & RA_INN)
					{
						if ((int)g_pPlayer->GetPos().y != (int)g_WorldMng()->GetLandHeight(g_pPlayer->GetPos()))
							bResult = FALSE;
					}
					if (bResult)
					{
						CHAR szDesc[MAX_REGIONDESC];
						CHAR* pDescSrc = lpRegionElem->m_szDesc;
						CHAR* pDescDsc = szDesc;
						int nNumCaption = 0;
						while (1)
						{
							if ((*pDescSrc == '\r' && *(pDescSrc + 1) == '\n') || *pDescSrc == 0)
							{
								*pDescDsc = 0;
								if (szDesc[0])
									g_ClientMsg.AddMessage(szDesc, 0xffffffff);
								nNumCaption++;
								pDescDsc = szDesc;
								pDescSrc += 2;
								if (*pDescSrc == 0)
									break;
							}
							else
							{
								*pDescDsc++ = *pDescSrc++;
								if (IsDBCSLeadByte(*pDescSrc))
									*pDescDsc++ = *pDescSrc++;
							}
						}
						pDescSrc = lpRegionElem->m_szTitle;
						pDescDsc = szDesc;
						int nCnt = 0;
						while (1)
						{
							if ((*pDescSrc == '\r' && *(pDescSrc + 1) == '\n') || *pDescSrc == 0)
							{
								*pDescDsc = 0;
								if (szDesc[0])
								{
									if (nCnt == 0)
									{
										if (::GetLanguage() != LANG_JAP)
											g_Caption1.AddCaption(szDesc, m_pFontAPITitle);// CWndBase::m_Theme.m_pFontCaption );
										else
											g_Caption1.AddCaption(szDesc, NULL);// CWndBase::m_Theme.m_pFontCaption );
									}
									else
									{
										if (::GetLanguage() != LANG_JAP)
											g_Caption1.AddCaption(szDesc, m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
										else
											g_Caption1.AddCaption(szDesc, NULL);// CWndBase::m_Theme.m_pFontCaption );
									}
								}
								pDescDsc = szDesc;
								pDescSrc += 2;
								nCnt++;
								if (*pDescSrc == 0)
									break;
							}
							else
							{
								*pDescDsc++ = *pDescSrc++;
								if (IsDBCSLeadByte(*pDescSrc))
									*pDescDsc++ = *pDescSrc++;
							}
						}
					}
					lpRegionElem->m_bInside = (bResult == TRUE);
					break;
				}
			}
			else
				lpRegionElem->m_bInside = FALSE;
		}

		if (m_dwIdBgmMusic)
		{
			if (IsStopMusic())
				PlayBGM(m_dwIdBgmMusic);
		}
		else
			SetWorldMusic();

		if (g_bKeyTable[VK_LEFT])
		{
			g_Neuz.m_camera.m_fRotx -= 4;
			if (g_Neuz.m_camera.m_fRotx > 360) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if (g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;

		}
		if (g_bKeyTable[VK_RIGHT])
		{
			g_Neuz.m_camera.m_fRotx += 4;

			if (g_Neuz.m_camera.m_fRotx > 360) g_Neuz.m_camera.m_fRotx = g_Neuz.m_camera.m_fRotx - 360;
			if (g_Neuz.m_camera.m_fRotx < 0) g_Neuz.m_camera.m_fRotx = 360 + g_Neuz.m_camera.m_fRotx;

		}
		if (g_bKeyTable[VK_UP])
		{
			g_Neuz.m_camera.m_fRoty += 2;
			if (g_Neuz.m_camera.m_fRoty > 80 - g_Neuz.m_camera.m_fZoom * 4) g_Neuz.m_camera.m_fRoty = 80 - g_Neuz.m_camera.m_fZoom * 4;
			if (g_Neuz.m_camera.m_fRoty < -80) g_Neuz.m_camera.m_fRoty = -80;

		}
		if (g_bKeyTable[VK_DOWN])
		{
			g_Neuz.m_camera.m_fRoty -= 2;
			if (g_Neuz.m_camera.m_fRoty > 80 - g_Neuz.m_camera.m_fZoom * 4) g_Neuz.m_camera.m_fRoty = 80 - g_Neuz.m_camera.m_fZoom * 4;
			if (g_Neuz.m_camera.m_fRoty < -80) g_Neuz.m_camera.m_fRoty = -80;

		}

		if (g_bKeyTable[VK_PRIOR])
		{

			if (!CDeployManager::GetInstance()->IsReady())
				if (!GuildDeploy()->IsReady())
					g_Neuz.m_camera.m_fZoom -= 0.6f;

			if (g_Neuz.m_camera.m_fZoom < 0)
				g_Neuz.m_camera.m_fZoom = 0;
		}
		if (g_bKeyTable[VK_NEXT])
		{
			if (!CDeployManager::GetInstance()->IsReady())
				if (!GuildDeploy()->IsReady())
					g_Neuz.m_camera.m_fZoom += 0.6f;

			if (g_Neuz.m_camera.m_fZoom > 7)
				g_Neuz.m_camera.m_fZoom = 7;
		}
	}
	if (nColor > 0)
	{
		g_fAddScale += 0.002f;
		g_fXScale += g_fAddScale;
		g_fYScale += g_fAddScale;
		if (g_fXScale > 1.0f) g_fXScale = 1.0f;
		if (g_fYScale > 1.0f) g_fYScale = 1.0f;
	}

	g_Caption1.Process();
	g_CapTime.Process();

	if (g_pPlayer)
		m_buffs.Process();

	DWORD dwBufCount = GetTickCount();
	if (g_pPlayer && dwBufCount >= m_dwOneSecCount + 1000)
	{
		int Mod = (dwBufCount - m_dwOneSecCount) % 1000;
		m_dwOneSecCount = GetTickCount() - Mod;
		for (int j = 0; j < SM_MAX; ++j)
		{
			if (0 < g_pPlayer->m_dwSMTime[j] && g_pPlayer->m_dwSMTime[j] != 1)
			{
				--g_pPlayer->m_dwSMTime[j];
			}
		}
	}

#if __VER >= 8 //__CSC_VER8_3
	BOOL buffstatus = FALSE;
	if (g_pPlayer)
	{
		for (auto& i : g_pPlayer->m_buffs.m_mapBuffs)
		{
			IBuff* pBuff = i.second;
			if (pBuff->GetType() == BUFF_SKILL)
			{
				buffstatus = TRUE;
				break;
			}
		}
	}

	if (buffstatus)
	{
		if (m_pWndBuffStatus == NULL)
			m_pWndBuffStatus = (CWndBuffStatus*)g_WndMng.CreateApplet(APP_BUFF_STATUS);
	}
	else
	{
		if (m_pWndBuffStatus != NULL)
		{
			m_pWndBuffStatus->Destroy();
			m_pWndBuffStatus = NULL;
			CWndBase* pWndBaseFocus = (CWndBase*)g_WndMng.GetFocusWnd();
			if (pWndBaseFocus && pWndBaseFocus->GetWndApplet() && pWndBaseFocus->GetWndApplet()->dwWndId != APP_COMMUNICATION_CHAT)
				SetFocus();
		}
	}
#endif //__CSC_VER8_3
	return CWndNeuz::Process();
}
HRESULT CWndWorld::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if (m_pVBGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);

	m_meshArrow.RestoreDeviceObjects();

	if (m_pFontAPICaption)
		m_pFontAPICaption->RestoreDeviceObjects();
	if (m_pFontAPITitle)
		m_pFontAPITitle->RestoreDeviceObjects();
	if (m_pFontAPITime)
		m_pFontAPITime->RestoreDeviceObjects();

	return S_OK;
}
HRESULT CWndWorld::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	m_meshArrow.InvalidateDeviceObjects();

	SAFE_RELEASE(m_pVBGauge);

	if (m_pFontAPICaption)
		m_pFontAPICaption->InvalidateDeviceObjects();
	if (m_pFontAPITitle)
		m_pFontAPITitle->InvalidateDeviceObjects();
	if (m_pFontAPITime)
		m_pFontAPITime->InvalidateDeviceObjects();
	g_CapTime.InvalidateDeviceObjects();
	g_Caption1.InvalidateDeviceObjects();

	return S_OK;
}
HRESULT CWndWorld::DeleteDeviceObjects()
{
	//	m_AdvMgr.RemoveButton();

	CWndBase::DeleteDeviceObjects();

	m_meshArrow.DeleteDeviceObjects();

	m_texTarget.DeleteDeviceObjects();
	m_texTargetFly.DeleteDeviceObjects();
	m_texLvUp.DeleteDeviceObjects();
	m_texLvDn.DeleteDeviceObjects();
	m_texLvUp2.DeleteDeviceObjects();
	m_texLvDn2.DeleteDeviceObjects();
	m_texGauEmptyNormal.DeleteDeviceObjects();
	m_texGauFillNormal.DeleteDeviceObjects();
	if (m_pFontAPICaption)
		m_pFontAPICaption->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontAPICaption);
	if (m_pFontAPITitle)
		m_pFontAPITitle->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontAPITitle);
	if (m_pFontAPITime)
		m_pFontAPITime->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontAPITime);
	g_CapTime.DeleteDeviceObjects();

	g_Caption1.DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);

	m_TexGuildWinner.DeleteDeviceObjects();
	m_TexGuildBest.DeleteDeviceObjects();

#ifdef __FL_TDM
	m_TextureBlueTeam.DeleteDeviceObjects();
	m_TextureRedTeam.DeleteDeviceObjects();
	m_TextureFightTDM.DeleteDeviceObjects();
#endif

	return S_OK;
}

void CWndWorld::UseSkill()
{
	CWorld* pWorld = g_WorldMng.Get();


	CObj* pTargetObj = pWorld->GetObjFocus();
	if (g_pPlayer->m_pActMover->IsFly() == FALSE)
	{
		if (pTargetObj)
		{
			pWorld->SetObjFocus(pTargetObj);

			CMover* pMover = (CMover*)pTargetObj;
			if (pMover->GetType() == OT_MOVER)
				m_dwNextSkill = NEXTSKILL_ACTIONSLOT;
		}
		else
			m_dwNextSkill = NEXTSKILL_ACTIONSLOT;
	}
}

#ifdef __BUFF_1107
DWORD CWndWorld::GetSystemPetTextureKey(IBuff* pBuff)
#else	// __BUFF_1107
DWORD CWndWorld::GetSystemPetTextureKey(SKILLINFLUENCE* pSkill)
#endif	// __BUFF_1107
{
#ifdef __BUFF_1107
	WORD wId = pBuff->GetId();
#else	// __BUFF_1107
	WORD wId = pSkill->wID;
#endif	// __BUFF_1107
	CPet* pPet = g_pPlayer->GetPet();
	if (pPet)
	{
		BYTE nLevel = pPet->GetLevel();
		if (nLevel == PL_D || nLevel == PL_C)
			return MAKELONG(wId, 0);
		else if (nLevel == PL_B || nLevel == PL_A)
			return MAKELONG(wId, 1);
		else if (nLevel == PL_S)
			return MAKELONG(wId, 2);
	}
	return static_cast<DWORD>(wId);
}

void CWndWorld::RenderBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse)
{
	RECT rectHittest;
	multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;

	pInfo->pt.x += pInfo->nDelta;
	ItemProp* pItem = NULL;
	WORD wID = pBuff->GetId();
	int nTexture = bPlayer ? 0 : 1;

	if (pBuff->GetType() == BUFF_SKILL)
	{
		auto it = m_pBuffTexture[nTexture].find(pBuff->GetId());
		if (it != m_pBuffTexture[nTexture].end())
			pp = &(*it);

		pItem = bPlayer ? prj.GetSkillProp(pBuff->GetId()) : prj.GetPartySkill(pBuff->GetId());
	}
	else if (pBuff->GetType() == BUFF_ITEM || pBuff->GetType() == BUFF_ITEM2 || pBuff->GetType() == BUFF_EQUIP)
	{
		auto it = m_pBuffTexture[2].find(pBuff->GetId());
		if (it != m_pBuffTexture[2].end())
			pp = &(*it);

		pItem = prj.GetItemProp(pBuff->GetId());
	}
	else if (pBuff->GetType() == BUFF_PET)
	{
		auto it = m_pBuffTexture[2].find(GetSystemPetTextureKey(pBuff));
		if (it != m_pBuffTexture[2].end())
			pp = &(*it);

		pItem = prj.GetItemProp(pBuff->GetId());
	}
	if (pp == NULL)
		return;

	ASSERT(pItem);
	if (pp->second.m_pTexture == NULL)
		return;

	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;
	if (pBuff->GetTotal() > 0)
	{
		dwOddTime = pBuff->GetTotal() - (g_tmCurrent - pBuff->GetInst());
		bFlash = (dwOddTime < 20 * 1000);

		if (pBuff->GetTotal() < (g_tmCurrent - pBuff->GetInst()))
			dwOddTime = 0;

		if (pItem->dwID == II_SYS_SYS_SCR_RETURN)
			bFlash = TRUE;
	}

	D3DXCOLOR color;

	if (bFlash)
	{
		if (pp->second.m_bFlsh == TRUE)
		{
			pp->second.m_nAlpha += 6;

			if (pp->second.m_nAlpha > 202)
			{
				pp->second.m_nAlpha = 202;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha -= 6;

			if (pp->second.m_nAlpha < 64)
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}

		if (pItem->nEvildoing < 0)
			color = D3DCOLOR_ARGB(pp->second.m_nAlpha, 255, 120, 255);
		else
			color = D3DCOLOR_ARGB(pp->second.m_nAlpha, 255, 255, 255);

		p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1, 1, color);
	}
	else
	{
		if (pItem->nEvildoing < 0)
			color = D3DCOLOR_ARGB(202, 255, 120, 255);
		else
			color = D3DCOLOR_ARGB(202, 255, 255, 255);

		p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1, 1, color);
	}

	if (dwOddTime > 0 && pItem->dwSkillTime != 999999999
		&& pBuff->GetType() != BUFF_EQUIP
		)
	{
		CTimeSpan ct((long)(dwOddTime / 1000.0f));
		RenderOptBuffTime(p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB(240, 240, 0));
	}
	else if (pItem->dwItemKind2 == IK2_BUFF2)
	{
		time_t	t = (time_t)pBuff->GetLevel() - time_null();
		if (t < 0)
			t = 0;
		CTimeSpan ts(t);
		RenderOptBuffTime(p2DRender, pInfo->pt, ts, D3DCOLOR_XRGB(240, 240, 0));
	}

	SetRect(&rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 32, pInfo->pt.y + 32);
	ClientToScreen(&rectHittest);

	CRect rc = rectHittest;
	if (rc.PtInRect(ptMouse))
	{
		CEditString strEdit;
		if (pItem->dwItemRare == 102)
			strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
		else if (pItem->dwItemRare == 103)
			strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
		else if (pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET)
			strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(46, 112, 169), ESSTY_BOLD);
		else if (pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02)
			strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(46, 112, 169), ESSTY_BOLD);
		else
			strEdit.AddString(pItem->szName, 0xff2fbe6d, ESSTY_BOLD);

		BOOL bItemKind3 = FALSE;
		CString str;
		if (pItem->dwItemKind3 == IK3_EGG)
		{
			CItemElem* pItemElem = g_pPlayer->GetPetItem();
			if (pItemElem != NULL)
			{
				PutPetTooltipInfo(pItemElem, &strEdit);
				bItemKind3 = TRUE;
			}
		}
		else if (pItem->dwItemKind3 == IK3_COUPLE_BUFF)
		{
			bItemKind3 = TRUE;
		}
		else if (pItem->dwItemKind3 == IK3_TS_BUFF)
			bItemKind3 = TRUE;

		if (pItem->dwItemKind3 == IK3_PET)
		{
			CItemElem* pItemElem = g_pPlayer->GetVisPetItem();
			if (pItemElem && (pItemElem->m_dwItemId == pItem->dwID))
			{
				ItemProp* pProp = pItemElem->GetProp();
				if (pProp)
					g_WndMng.PutVisPetInfo(pItemElem, pProp, &strEdit);
				bItemKind3 = TRUE;
			}
		}

		switch (pItem->dwID)
		{
		case II_SYS_SYS_SCR_SMELPROT:
		case II_SYS_SYS_SCR_SMELPROT3:
		case II_SYS_SYS_SCR_SMELPROT4:
		case II_SYS_SYS_SCR_SMELTING:
		case II_SYS_SYS_SCR_SMELTING2:
			str.Format("\n%s", prj.GetText(TID_GAME_DEMOL_USE));
			break;
		case II_SYS_SYS_SCR_RETURN:
		{
			BUFFICONRECT_INFO info;
			CopyRect(&info.rc, &rectHittest);
			info.dwID = pItem->dwID;
			m_rcCheck.push_back(info);
		}
		break;
		case II_SYS_SYS_SCR_SUPERSMELTING:
			str.Format("\n%s", prj.GetText(TID_GAME_DEMOL_USE));
			break;
		case II_SYS_SYS_SCR_PARTYSUMMON:
		{
			BUFFICONRECT_INFO info;
			CopyRect(&info.rc, &rectHittest);
			info.dwID = pItem->dwID;
			m_rcCheck.push_back(info);

			CTimeSpan ct((long)(dwOddTime / 1000.0f));
			if (ct.GetHours() >= 1)
				str.Format("\n%.2d:%.2d:%.2d\n%s", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds(), prj.GetText(TID_GAME_SUMMON_BUFF_ICON));
			else
				str.Format("\n%.2d:%.2d\n%s", ct.GetMinutes(), ct.GetSeconds(), prj.GetText(TID_GAME_SUMMON_BUFF_ICON));
			RenderOptBuffTime(p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB(240, 240, 0));
		}
		break;
		case II_SYS_SYS_SCR_PET_FEED_POCKET:
		{
			CItemElem* ptr;
			CItemElem* pItemElem = NULL;

			int nMax = g_pPlayer->m_Inventory.GetMax();
			for (int i = 0; i < nMax; i++)
			{
				ptr = g_pPlayer->m_Inventory.GetAtId(i);
				if (IsUsableItem(ptr) && ptr->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET &&
					ptr->m_dwKeepTime > 0 && !ptr->IsFlag(CItemElem::expired))
				{
					BUFFICONRECT_INFO info;
					CopyRect(&info.rc, &rectHittest);
					info.dwID = pItem->dwID;
					m_rcCheck.push_back(info);

					pItemElem = ptr;
					i = nMax;
				}
			}

			if (pItemElem != NULL)
			{
				CString strTemp;

				time_t t = pItemElem->m_dwKeepTime - time_null();
				{
					if (t > 0)
					{
						CTimeSpan time(t);
						if (time.GetDays())
							str.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(time.GetDays() + 1));
						else if (time.GetHours())
							str.Format(prj.GetText(TID_PK_LIMIT_HOUR), time.GetHours());
						else if (time.GetMinutes() > 1)
							str.Format(prj.GetText(TID_PK_LIMIT_MINUTE), time.GetMinutes());
						else
							str.Format(prj.GetText(TID_PK_LIMIT_SECOND), time.GetSeconds());
					}
					strTemp = str;
					strEdit.AddString("\n");
					strEdit.AddString(strTemp, D3DCOLOR_XRGB(255, 20, 20));
				}

				strEdit.AddString("\n");
				strTemp.Format("%s %d", prj.GetText(TID_GAME_PET_FEED_COUNT), g_pPlayer->m_Inventory.GetItemCount(II_SYS_SYS_FEED_01));
				strEdit.AddString(strTemp, D3DCOLOR_XRGB(50, 50, 205));

				strEdit.AddString("\n");
				strTemp.Format("%s", pItem->szCommand);
				strEdit.AddString(strTemp, D3DCOLOR_XRGB(178, 0, 255));
			}
		}
		break;

#ifdef __PROTECT_AWAKE
		case II_SYS_SYS_SCR_AWAKESAFE:
			break;
#endif

		default:
		{
			if (bItemKind3)
				break;
			if (pBuff->GetType() == BUFF_EQUIP)
				break;

			if (pBuff->GetTotal() > 0 && pItem->dwSkillTime != 999999999)
			{
				CTimeSpan ct((long)(dwOddTime / 1000.0f));
				if (ct.GetDays() != 0)
				{
					str.Format("\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
				}
				else
				{
					if (ct.GetHours() >= 1)
						str.Format("\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
					else
						str.Format("\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds());
				}
			}
			else if (prj.GetItemProp(pBuff->GetId())->dwItemKind2 == IK2_BUFF2)
			{
				time_t	t = (time_t)pBuff->GetLevel() - time_null();
				if (t < 0)
					t = 0;
				CTimeSpan ts(t);
				if (ts.GetDays() != 0)
				{
					str.Format("\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ts.GetDays()), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
				}
				else
				{
					if (ts.GetHours() >= 1)
						str.Format("\n%.2d:%.2d:%.2d", ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
					else
						str.Format("\n%.2d:%.2d", ts.GetMinutes(), ts.GetSeconds());
				}
			}
		}
		break;
		}	// switch

		if (pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02)
		{
			strEdit.AddString("\n");
			CString str;
			str.Format("%s %d", prj.GetText(TID_GAME_PET_FEED_COUNT), g_pPlayer->m_Inventory.GetItemCount(II_SYS_SYS_FEED_01));
			strEdit.AddString(str, D3DCOLOR_XRGB(50, 50, 205));
		}

		BOOL bPutDescription = TRUE;
		if (g_WndMng.PutDestParam(pItem, NULL, &strEdit))
			bPutDescription = FALSE;

		if (bPutDescription && strlen(pItem->szCommand))
		{
			CString str;
			str.Format("\n%s", pItem->szCommand);
			strEdit += str;
		}

		if (pItem->dwID != II_SYS_SYS_SCR_PET_FEED_POCKET
			&& pItem->dwID != II_PET_EGG)
			strEdit.AddString(str);

		if (pItem->dwItemKind3 == IK3_PET)
		{

			CItemElem* pItemElem = g_pPlayer->GetVisPetItem();
			if (pItemElem && (pItemElem->m_dwItemId == pItem->dwID))
			{
				ItemProp* pProp = pItemElem->GetProp();
				if (pProp)
					g_WndMng.PutAwakeningBlessing(pItemElem, pProp, &strEdit);
			}
		}

		g_toolTip.PutToolTip(wID, strEdit, rectHittest, ptMouse, 1);

	}

	++pInfo->nCount;
	if ((pInfo->nCount % m_nLimitBuffCount) == 0)
	{
		if (pBuff->GetType() == BUFF_SKILL)
			pInfo->pt.x = (m_rectWindow.Width() / 2) - 100;
		else
			pInfo->pt.x = (m_rectWindow.Width() / 2) + 130;

		pInfo->pt.y += GetBuffTimeGap();
	}

}

#if __VER >= 9 // __CSC_VER9_1
void CWndWorld::PutPetTooltipInfo(CItemElem* pItemElem, CEditString* pEdit)
{
	if (pItemElem == NULL || pEdit == NULL)
		return;

	CString strTemp;
	pEdit->Empty();
	//Name
//	strTemp.Format( "%s", pItemElem->GetProp()->szName );
	strTemp = pItemElem->GetName();
	pEdit->AddString(strTemp, D3DCOLOR_XRGB(46, 112, 169), ESSTY_BOLD);

	//Level
	int nLevel = PL_EGG;
	if (pItemElem->m_pPet)
	{
		nLevel = pItemElem->m_pPet->GetLevel();

		DWORD dwLevelText;
		switch (nLevel)
		{
		case PL_EGG:
			dwLevelText = TID_GAME_PETGRADE_E;
			break;
		case PL_D:
			dwLevelText = TID_GAME_PETGRADE_D;
			break;
		case PL_C:
			dwLevelText = TID_GAME_PETGRADE_C;
			break;
		case PL_B:
			dwLevelText = TID_GAME_PETGRADE_B;
			break;
		case PL_A:
			dwLevelText = TID_GAME_PETGRADE_A;
			break;
		case PL_S:
			dwLevelText = TID_GAME_PETGRADE_S;
			break;
		}
		strTemp.Format("%s : %s", prj.GetText(TID_GAME_CHARACTER_02), prj.GetText(dwLevelText));
		pEdit->AddString("\n");
		pEdit->AddString(strTemp, D3DCOLOR_XRGB(0, 158, 0));

		//Ability value
		if (nLevel != PL_EGG)
		{
			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			pItemElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);

			if (g_WndMng.IsDst_Rate(dwDstParam))
			{
				if (dwDstParam == DST_ATTACKSPEED)
					strTemp.Format("\nBonus: %s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam / 2 / 10);
				else
					strTemp.Format("\nBonus: %s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam);
			}
			else
			{
				strTemp.Format("\nBonus: %s %+d", g_WndMng.FindDstString(dwDstParam), nParam);
			}

			pEdit->AddString(strTemp, g_WndMng.dwItemColor.dwResistSM1);
		}

		//Level History
		int nLevel = pItemElem->m_pPet->GetLevel();
		if (nLevel > PL_EGG)
		{
			pEdit->AddString("\n");
			pEdit->AddString("(", D3DCOLOR_XRGB(0, 200, 255));
			for (int i = PL_D; i <= nLevel; i++)
			{
				BYTE bLevel = pItemElem->m_pPet->GetAvailLevel(i);
				strTemp.Format("Lv%d", bLevel);
				pEdit->AddString(strTemp, D3DCOLOR_XRGB(0, 127, 255));
				if (i != nLevel)
					pEdit->AddString("/", D3DCOLOR_XRGB(0, 200, 255));
			}
			pEdit->AddString(")", D3DCOLOR_XRGB(0, 200, 255));
		}

	}
}
#endif //__CSC_VER9_1

void CWndWorld::RenderExpBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID)
{
	int nExpCount = 0;

	DWORD dwItemId[MAX_AMP_STACK_COUNT];
	ZeroMemory(dwItemId, sizeof(dwItemId));

	DWORD dwExpTime[MAX_AMP_STACK_COUNT];
	ZeroMemory(dwExpTime, sizeof(dwExpTime));

	for (auto& it : g_pPlayer->m_buffs.m_mapBuffs)
	{
		IBuff* ptr = it.second;
		ItemProp* pItemProp = prj.GetItemProp(ptr->GetId());
		if (!pItemProp)
			continue;
		if (pItemProp->dwItemKind3 == IK3_EXP_RATE)
		{
			dwExpTime[nExpCount] = ptr->GetTotal() - (g_tmCurrent - ptr->GetInst());
			dwItemId[nExpCount] = ptr->GetId();
			++nExpCount;
		}
	}

	RECT rectHittest;
	multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;

	pInfo->pt.x += pInfo->nDelta;

	D3DXCOLOR color = D3DCOLOR_ARGB(202, 255, 255, 255);

	for (int j = 0; j < nExpCount; ++j)
	{
		auto it = m_pBuffTexture[2].find(dwItemId[j]);
		if (it != m_pBuffTexture[2].end())
			pp = &(*it);

		if (pp == NULL)
			return;
		if (pp->second.m_pTexture == NULL)
			return;

		switch (j)
		{
		case 0:
		{
			p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1, 1, color);
			break;
		}
		case 1:
		{
			p2DRender->RenderTexture2(CPoint(pInfo->pt.x + 5, pInfo->pt.y + 30), pp->second.m_pTexture, 0.5f, 0.5f, color);
			break;
		}
		case 2:
		{
			p2DRender->RenderTexture2(CPoint(pInfo->pt.x + 22, pInfo->pt.y + 30), pp->second.m_pTexture, 0.5f, 0.5f, color);
			break;
		}
		case 3:
		{
			p2DRender->RenderTexture2(CPoint(pInfo->pt.x + 5, pInfo->pt.y + 47), pp->second.m_pTexture, 0.5f, 0.5f, color);
			break;
		}
		case 4:
		{
			p2DRender->RenderTexture2(CPoint(pInfo->pt.x + 22, pInfo->pt.y + 47), pp->second.m_pTexture, 0.5f, 0.5f, color);
			break;
		}

		}
	}


	SetRect(&rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 32, pInfo->pt.y + 32);
	ClientToScreen(&rectHittest);

	CEditString strEdit;

	int nExtraEXP = 0;
	for (int i = 0; i < nExpCount; i++)
	{
		ItemProp* pItemProp = prj.GetItemProp(dwItemId[i]);
		if (pItemProp)
		{
			nExtraEXP += 50;
		}
	}

	CString strTemp;
	strEdit.AddString(prj.GetText(TID_GAME_EXPITEM_TOOLTIP));
	strTemp.Format(" %d%% ", nExtraEXP);
	strEdit.AddString(strTemp, D3DCOLOR_ARGB(255, 255, 90, 80));
	strEdit.AddString(prj.GetText(TID_GAME_EXPITEM_TOOLTIP1));

	strEdit.AddString('\n');
	strEdit.AddString(prj.GetText(TID_GAME_EXP_COUTMSG0));//, 0xff99cc00 );

	for (int i = 0; i < nExpCount; ++i)
	{
		DWORD dwMsg = TID_GAME_EXP_COUTMSG5;
		if (i == 0)
			dwMsg = TID_GAME_EXP_COUTMSG1;
		else if (i == 1)
			dwMsg = TID_GAME_EXP_COUTMSG2;
		else if (i == 2)
			dwMsg = TID_GAME_EXP_COUTMSG3;
		else if (i == 3)
			dwMsg = TID_GAME_EXP_COUTMSG4;

		strEdit.AddString("\n");
		strEdit.AddString(prj.GetText(dwMsg));

		CString strTime;
		DWORD dwOddTime = dwExpTime[i];
		CTimeSpan ct((long)(dwOddTime / 1000.0f));

		if (ct.GetDays() != 0)
		{
			strTime.Format(" %.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
		}
		else
		{
			if (ct.GetHours() >= 1)
				strTime.Format(" %.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
			else
				strTime.Format(" %.2d:%.2d", ct.GetMinutes(), ct.GetSeconds());
		}
		strEdit.AddString(strTime);
	}

	g_toolTip.PutToolTip(dwItemID, strEdit, rectHittest, ptMouse, 1);

	++pInfo->nCount;
	if ((pInfo->nCount % m_nLimitBuffCount) == 0)
	{
		if (pBuff->GetType() == BUFF_SKILL)
			pInfo->pt.x = (m_rectWindow.Width() / 2) - 100;
		else
			pInfo->pt.x = (m_rectWindow.Width() / 2) + 75;

		pInfo->pt.y += GetBuffTimeGap();
	}
}


void CWndWorld::RenderSMBuff(C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse)
{
	RECT rectHittest;

	for (int i = 0; i < SM_MAX; ++i)
	{
		if (g_pPlayer->m_dwSMTime[i] <= 0)
			continue;

		int nResistTexture = 1000;
		ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);

		if (pItem != NULL && m_dwSMItemTexture[i] != NULL)
		{
			CEditString strEdit;
			CTimeSpan ct(g_pPlayer->m_dwSMTime[i]);
			{
				pInfo->pt.x += (32 + 5);
				if (g_pPlayer->m_dwSMTime[i] < 20)
				{
					p2DRender->RenderTexture(pInfo->pt, m_dwSMItemTexture[i], m_nSMAlpha[i]);

					if (m_bSMFlsh[i] == TRUE)
					{
						m_nSMAlpha[i] += 6;

						if (m_nSMAlpha[i] > 202)
						{
							m_nSMAlpha[i] = 202;
							m_bSMFlsh[i] = FALSE;
						}
					}
					else
					{
						m_nSMAlpha[i] -= 6;

						if (m_nSMAlpha[i] < 64)
						{
							m_nSMAlpha[i] = 64;
							m_bSMFlsh[i] = TRUE;
						}
					}
				}
				else
					p2DRender->RenderTexture(pInfo->pt, m_dwSMItemTexture[i], 202);

				SetRect(&rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 32, pInfo->pt.y + 32);
				ClientToScreen(&rectHittest);
				if (pItem->dwItemRare == 102)
					strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
				else if (pItem->dwItemRare == 103)
					strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
				else
					strEdit.AddString(pItem->szName, 0xff2fbe6d, ESSTY_BOLD);

				CString str;
				if (pItem->dwCircleTime == 1)
					str.Format("\n%s", prj.GetText(TID_GAME_COND_USE));
				else
					str.Format("\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());

				strEdit += str;


				g_toolTip.PutToolTip(g_AddSMMode.dwSMItemID[i], strEdit, rectHittest, ptMouse, 1);

				pInfo->nCount++;

				if (pItem->dwCircleTime != 1)
					RenderOptBuffTime(p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB(240, 240, 0));

				if ((pInfo->nCount % m_nLimitBuffCount) == 0)
				{
					pInfo->pt.x = (m_rectWindow.Width() / 2) + 130;
					pInfo->pt.y += GetBuffTimeGap();
				}
			}
		}
	}
}


void CWndWorld::RenderBuff(C2DRender* p2DRender)
{
	m_rcCheck.clear();

	if (g_pPlayer == NULL)
		return;

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontWorld);

	int i = 0, nCount = 0;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);
	BUFFICON_INFO left, right;

	left.pt = CPoint((m_rectWindow.Width() / 2) - 100, 10);
	left.nCount = 0;
	left.nDelta = -(32 + 5);
	right.pt = CPoint((m_rectWindow.Width() / 2) + 130, 10);
	right.nCount = 0;
	right.nDelta = (32 + 5);

	BUFFICON_INFO* pInfo;
	BOOL bHasExpRender = FALSE;

	for (auto& it : g_pPlayer->m_buffs.m_mapBuffs)
	{
		IBuff* pBuff = it.second;
		WORD wType = pBuff->GetType();
		WORD wId = pBuff->GetId();

		if (wId)
		{
			if (wType == BUFF_SKILL)
				pInfo = &left;
			else
				pInfo = &right;

			if (wType == BUFF_ITEM)
			{
				ItemProp* pItemProp = prj.GetItemProp(wId);
				if (pItemProp && pItemProp->dwItemKind3 == IK3_TICKET)
					continue;

				if (pItemProp && pItemProp->dwItemKind1 == IK1_HOUSING)
					continue;

				if (pItemProp && pItemProp->dwItemKind3 == IK3_EXP_RATE)
				{
					if (bHasExpRender == TRUE)
						continue;
					else
					{
						RenderExpBuffIcon(p2DRender, pBuff, pInfo, ptMouse, wId);
						bHasExpRender = TRUE;
						continue;
					}
				}
			}

			if (wType != BUFF_SKILL)
				RenderBuffIcon(p2DRender, pBuff, TRUE, pInfo, ptMouse);
		}
	}

	BOOL	bNearByLeader = false;
	CMover* pLeader = prj.GetUserByID(g_Party.m_aMember[0].m_uPlayerId);
	if (IsValidObj((CObj*)pLeader)/* && pLeader->IsNearPC( (CUser*)pAttacker ) && IsOrigin()*/)
		bNearByLeader = true;

	if (g_Party.m_nModeTime[PARTY_PARSKILL_MODE] || bNearByLeader)
	{
		for (auto& it2 : m_buffs.m_mapBuffs)
		{
			IBuff* ptr2 = it2.second;
			pInfo = &left;
			RenderBuffIcon(p2DRender, ptr2, FALSE, pInfo, ptMouse);
		}
	}

	RenderSMBuff(p2DRender, &right, ptMouse);
	RenderEventIcon(p2DRender, &right, ptMouse);

	p2DRender->SetFont(pOldFont);

}

#if __VER >= 12 // __LORD
#define	TTI_LORD_EVENT	123456789
void CWndWorld::RenderEventIcon(C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse)
{
	RECT rectHittest;
	ILordEvent* pEvent = CCLord::Instance()->GetEvent();
	for (int i = 0; i < pEvent->GetComponentSize(); i++)
	{
		CLEComponent* pComponent = pEvent->GetComponentAt(i);
		pInfo->pt.x += (32 + 5);
		p2DRender->RenderTexture(pInfo->pt, pComponent->GetTexture(), 202);
		SetRect(&rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 32, pInfo->pt.y + 32);
		ClientToScreen(&rectHittest);

		CEditString editString;
#ifndef __FL_UPDATE_LORDEVENT
		char szTooltip[255] = { 0, };
		sprintf(szTooltip, prj.GetText(TID_GAME_LORD_EVENT_TOOLTIP),
			CPlayerDataCenter::GetInstance()->GetPlayerString(pComponent->GetIdPlayer()),
			pComponent->GetEFactor() * 100, pComponent->GetIFactor() * 100);
		editString.AddString(szTooltip, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
#else
		CString strInfo;
		strInfo.Format("Lord %s is running EXP %3.1f%%, Drop Rate %3.1f%%, Penya Rate %3.1f%% increase event.",
			CPlayerDataCenter::GetInstance()->GetPlayerString(pComponent->GetIdPlayer()),
			pComponent->GetEFactor() * 100,
			pComponent->GetIFactor() * 100,
			pComponent->GetPFactor() * 100
		);
		editString.AddString(strInfo, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
#endif

		CString strRest;
		CTimeSpan timeSpan(0, 0, pComponent->GetTick(), 0);
		strRest.Format("\n%d", timeSpan.GetTotalMinutes());
		editString += strRest;
		g_toolTip.PutToolTip(TTI_LORD_EVENT, editString, rectHittest, ptMouse, 1);
		pInfo->nCount++;
		RenderOptBuffTime(p2DRender, pInfo->pt, timeSpan, D3DCOLOR_XRGB(240, 240, 0));
		if ((pInfo->nCount % m_nLimitBuffCount) == 0)
		{
			pInfo->pt.x = (m_rectWindow.Width() / 2) + 130;
			pInfo->pt.y += GetBuffTimeGap();
		}
	}
}
#endif	// __LORD


void CWndWorld::RenderCasting(C2DRender* p2DRender)
{
	if (g_pPlayer && g_pPlayer->IsStateMode(STATE_BASEMOTION_MODE))
	{
		if (g_pPlayer->m_nReadyTime)
		{
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(g_pPlayer->m_dwUseItemId);
			if (pItemElem)
			{
				DWORD dwTime = g_tmCurrent - g_pPlayer->m_nReadyTime;

				CRect rect;// = GetClientRect();
				int nWidthClient = 200;
				int nWidth;

				nWidth = (int)(nWidthClient * ((float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType));
				TRACE("%d / %d : %f\n", pItemElem->GetProp()->dwSkillReadyType, dwTime, ((float)dwTime / (float)pItemElem->GetProp()->dwSkillReadyType));
				int nPos = (GetClientRect().Width() - nWidthClient) / 2;
				rect = CRect(nPos, GetClientRect().Height() / 2, nPos + nWidthClient, 0);
				CRect rectTemp = rect;
				rectTemp.right = rectTemp.left + nWidth;
				ClientToScreen(rect);
				ClientToScreen(rectTemp);
				m_Theme.MakeGaugeVertex(p2DRender->m_pd3dDevice, &rect, D3DCOLOR_ARGB(200, 255, 255, 255), m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.RenderGauge(p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.MakeGaugeVertex(p2DRender->m_pd3dDevice, &rectTemp, D3DCOLOR_ARGB(128, 255, 15, 15), m_pVBGauge, &m_texGauEmptyNormal);
				m_Theme.RenderGauge(p2DRender->m_pd3dDevice, m_pVBGauge, &m_texGauEmptyNormal);
			}
		}
	}
}


void CWndWorld::RenderMoverBuff(CMover* pMover, C2DRender* p2DRender)
{
	if (pMover == NULL || p2DRender == NULL)
		return;

	int nIconSize = 24;

	CPoint Lpoint = CPoint((GetClientRect().Width() - 200) / 2 - 20, 44);
	int Count = 0;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);

	BOOL bExpRander = FALSE;
	for (auto& i : pMover->m_buffs.m_mapBuffs)
	{
		IBuff* pBuff = i.second;
		WORD wType = pBuff->GetType();
		DWORD dwSkillID = pBuff->GetId();

		ItemProp* pItem = NULL;
		if (wType == BUFF_SKILL)
			pItem = prj.GetSkillProp(dwSkillID);
		else
			pItem = prj.GetItemProp(dwSkillID);

		if (wType == BUFF_ITEM || wType == BUFF_ITEM2)
			continue;

		multimap< DWORD, BUFFSKILL >::value_type* pp = NULL;

		if (wType == BUFF_EQUIP)
		{
			auto it = m_pBuffTexture[2].find(dwSkillID);
			if (it != m_pBuffTexture[2].end())
				pp = &(*it);
		}
		else if (wType == BUFF_PET)
		{
			auto it = m_pBuffTexture[2].find(dwSkillID);
			if (it != m_pBuffTexture[2].end())
				pp = &(*it);
		}
		else if (wType == BUFF_SKILL)
		{
			auto it = m_pBuffTexture[0].find(dwSkillID);
			if (it != m_pBuffTexture[0].end())
				pp = &(*it);
		}
		else
		{
			auto it = m_pBuffTexture[1].find(dwSkillID);
			if (it != m_pBuffTexture[1].end())
				pp = &(*it);
		}

		if (pp != NULL && pp->second.m_pTexture != NULL)
		{
			DWORD dwOddTime = pBuff->GetTotal() - (g_tmCurrent - pBuff->GetInst());
			if (dwSkillID == II_SYS_SYS_SCR_AMPESS)
			{
				if (bExpRander == FALSE)
				{
					Lpoint.x += nIconSize;
					pp->second.m_pTexture->Render(p2DRender, Lpoint, CPoint(nIconSize, nIconSize), 202);
				}
			}
			else
			{
				Lpoint.x += nIconSize;
				if (pBuff->GetTotal() > 0 && dwOddTime < 20 * 1000)
				{
					pp->second.m_pTexture->Render(p2DRender, Lpoint, CPoint(nIconSize, nIconSize), pp->second.m_nAlpha);

					if (pp->second.m_bFlsh == TRUE)
					{
						pp->second.m_nAlpha += 6;

						if (pp->second.m_nAlpha > 220)
						{
							pp->second.m_nAlpha = 220;
							pp->second.m_bFlsh = FALSE;
						}
					}
					else
					{
						pp->second.m_nAlpha -= 6;

						if (pp->second.m_nAlpha < 32)
						{
							pp->second.m_nAlpha = 32;
							pp->second.m_bFlsh = TRUE;
						}
					}
				}
				else
				{
					pp->second.m_pTexture->Render(p2DRender, Lpoint, CPoint(nIconSize, nIconSize), 220);
				}
			}

			BOOL bExpMsg = TRUE;
			if (dwSkillID == II_SYS_SYS_SCR_AMPESS)
			{
				if (bExpRander)
					bExpMsg = FALSE;

				bExpRander = TRUE;
			}

			if (bExpMsg)
			{
				SetRect(&rectHittest, Lpoint.x, Lpoint.y, Lpoint.x + nIconSize, Lpoint.y + nIconSize);
				ClientToScreen(&rectHittest);

				CEditString strEdit;
				CString strLevel("");
				if (wType == BUFF_SKILL)
					strLevel.Format("   Lvl %d", pBuff->GetLevel());

				if (pItem->dwItemRare == 102)
				{
					strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
					strEdit.AddString(strLevel, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
				}
				else if (pItem->dwItemRare == 103)
				{
					strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
					strEdit.AddString(strLevel, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
				}
				else
				{
					strEdit.AddString(pItem->szName, 0xff2fbe6d, ESSTY_BOLD);
					strEdit.AddString(strLevel, 0xff2fbe6d, ESSTY_BOLD);
				}

				BOOL bPutDescription = TRUE;
				if (wType == BUFF_SKILL)
				{
					if (dwSkillID != SI_RIG_MASTER_BLESSING &&
						dwSkillID != SI_ASS_CHEER_STONEHAND &&
						dwSkillID != SI_MAG_EARTH_LOOTING &&
						dwSkillID != SI_ASS_HEAL_PREVENTION &&
						dwSkillID != SI_FLO_CHEER_PATTERS &&
						dwSkillID != SI_STORM_DOUBLE_HOLDINGSTORM
						)
					{
						AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pItem->dwSubDefine, pBuff->GetLevel());
						if (pAddSkillProp)
						{
							if (g_WndMng.PutDestParam(NULL, pAddSkillProp, &strEdit))
								bPutDescription = FALSE;
						}
					}
				}
				else
				{
					if (g_WndMng.PutDestParam(pItem, NULL, &strEdit))
						bPutDescription = FALSE;
				}

				if (bPutDescription)
				{
					CString str;
					str.Format("\n%s", pItem->szCommand);
					strEdit += str;
				}

				BOOL bTime = TRUE;
				if (dwSkillID == II_SYS_SYS_SCR_SMELPROT || dwSkillID == II_SYS_SYS_SCR_SMELPROT2
					|| dwSkillID == II_SYS_SYS_SCR_SMELTING || dwSkillID == II_SYS_SYS_SCR_RETURN || dwSkillID == II_SYS_SYS_SCR_SUPERSMELTING
					|| dwSkillID == II_SYS_SYS_SCR_SMELPROT3
					|| dwSkillID == II_SYS_SYS_SCR_SMELPROT4
					|| dwSkillID == II_SYS_SYS_SCR_SMELTING2
					|| wType == BUFF_EQUIP
					)
					bTime = FALSE;

				if (bTime && pBuff->GetTotal() > 0)
				{
					CTimeSpan ct((long)(dwOddTime / 1000.0f));
					CString str;
					str.Format("\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
					strEdit += str;

					if (dwOddTime > pBuff->GetTotal())
						g_toolTip.CancelToolTip();
					else
						g_toolTip.PutToolTip(dwSkillID, strEdit, rectHittest, ptMouse, 1);
				}
				else
					g_toolTip.PutToolTip(dwSkillID, strEdit, rectHittest, ptMouse, 1);

				Count++;
				if ((Count % 8) == 0)
				{
					Lpoint.x = (GetClientRect().Width() - 200) / 2 - 20;
					Lpoint.y += nIconSize;
				}
			}
		}
	}
}

void CWndWorld::RenderOptBuffTime(C2DRender* p2DRender, CPoint& point, CTimeSpan& ct, DWORD dwColor)
{
	CString str;

	if (ct.GetDays() >= 1)
		str.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(ct.GetDays()));
	else if (ct.GetHours() >= 1)
		str.Format(prj.GetText(TID_PK_LIMIT_HOUR), ct.GetHours());
	else if (ct.GetMinutes() >= 1)
		str.Format(prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes());
	else
		str.Format(prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds());


	CD3DFont* pFontOld = p2DRender->GetFont();
	CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
	p2DRender->TextOut(point.x + 16 - (size.cx / 2), point.y + 32, str, dwColor);
}

int CWndWorld::GetBuffTimeGap()
{
	return 40 + 8;
}

void CWndWorld::InviteParty(u_long uidPlayer)
{

	if (g_Party.m_aMember[0].m_uPlayerId == 0 || g_Party.IsLeader(g_pPlayer->m_idPlayer))
	{
		int nMaxPartyMember = MAX_PTMEMBER_SIZE_SPECIAL;
		CString szMessageBuf;
		szMessageBuf = prj.GetText(TID_GAME_FULLPARTY3);

		if (nMaxPartyMember >= g_Party.m_nSizeofMember + 1)
		{
			g_DPlay.SendPartyMemberRequest(uidPlayer, FALSE);
		}
		else
		{
			g_WndMng.PutString(szMessageBuf, NULL, prj.GetTextColor(TID_GAME_FULLPARTY1));
		}
	}
	else
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0007)));
	}
}

void CWndWorld::InviteCompany(OBJID objId)
{
	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		if (g_GuildCombatMng.m_bRequest && g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE && g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_CLOSE_STATE)
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_INVITATION_GUILD));
		else
			g_DPlay.SendGuildInvite(objId);
	}
}
void CWndWorld::OnDestroyChildWnd(CWndBase* pWndChild)
{
	m_AdvMgr.DestroyChildWnd(pWndChild);
}


CAdvMgr::CAdvMgr()
{
	m_nIndex = 0;
	m_pParentWnd = NULL;
	m_vecButton.reserve(MAX_ADVBUTTON);
}
CAdvMgr::~CAdvMgr()
{
	RemoveButtons();
}
void CAdvMgr::Init(CWndBase* pParentWnd)
{
	m_pParentWnd = pParentWnd;
}
void CAdvMgr::RemoveButtons()
{
	m_nIndex = 0;

	vector<BUTTON_INFO>::iterator i = m_vecButton.begin();
	for (; i != m_vecButton.end(); ++i)
	{
		BUTTON_INFO* vecButton = &(*i);
		SAFE_DELETE(vecButton->m_pwndButton);
	}

	m_vecButton.clear();
}

void CAdvMgr::SetVisible(BOOL bVisible)
{
	for (auto& it : m_vecButton)
		it.m_pwndButton->SetVisible(bVisible);
}

void CAdvMgr::AddAdvButton(DWORD dwid)
{
	if (m_pParentWnd == NULL)
		return;

	if (m_vecButton.size() >= MAX_ADVBUTTON)
	{
		Error("CAdvMgr::AddAdvButton에서 MAX_ADVBUTTON보다 큼 : %d", m_nIndex);
		return;
	}

	BUTTON_INFO* pButton = FindRunWindowButton(dwid);
	if (pButton)
		return;

	int x = 3;
	int y = 135;

	CWndBase* pWndBase = new CWndButton;
	((CWndButton*)pWndBase)->Create("", 0, CRect(x, y, x + 32, y + 32), m_pParentWnd, m_nIndex + 2500);

	if (dwid == APP_CHARACTER3)
		((CWndButton*)pWndBase)->SetTexture(m_pParentWnd->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "Alert_Character.bmp"), TRUE);
	else if (dwid == APP_SKILL4)
		((CWndButton*)pWndBase)->SetTexture(m_pParentWnd->m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "Alert_SkillTree.bmp"), TRUE);
	else
	{
		CString str = "Alert_Character.bmp";
		if (dwid == APP_CASH_SHOP)
			str = "CashShop.png";
#ifdef __FL_WHEEL_OF_FORTUNE
		else if (dwid == APP_WOF)
			str = "Wof.png";
#endif
		CTexture* tex1 = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, str), TRUE);
		((CWndButton*)pWndBase)->SetTexture(D3DDEVICE, NULL, tex1);
		((CWndButton*)pWndBase)->FitTextureSize();
	}

	BUTTON_INFO button;
	button.m_pwndButton = (CWndButton*)pWndBase;
	button.m_dwRunWindow = dwid;
	m_vecButton.push_back(button);
	m_nIndex++;

	SortButton();
}

#ifdef __FL_WHEEL_OF_FORTUNE
BOOL CAdvMgr::DeleteButton(DWORD dwID)
{
	BUTTON_INFO* pButton = FindRunWindowButton(dwID);
	if (!pButton || !pButton->m_pwndButton)
		return FALSE;

	pButton->m_pwndButton->Destroy();

	return TRUE;
}
#endif

BOOL CAdvMgr::RunButton(DWORD dwID)
{
	vector<BUTTON_INFO>::iterator i = m_vecButton.begin();
	for (; i != m_vecButton.end(); ++i)
	{
		BUTTON_INFO* vecButton = &(*i);

		if (vecButton->m_pwndButton && vecButton->m_pwndButton->m_nIdWnd == dwID)
		{
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, vecButton->m_dwRunWindow);

			if (vecButton->m_dwRunWindow == APP_CASH_SHOP
#ifdef __FL_WHEEL_OF_FORTUNE
				|| vecButton->m_dwRunWindow == APP_WOF
#endif
				)
				return TRUE;

			vecButton->m_pwndButton->Destroy();
			return TRUE;
		}
	}

	return FALSE;
}

void CAdvMgr::DestroyChildWnd(CWndBase* pWndChild)
{
	auto i = m_vecButton.begin();
	for (; i != m_vecButton.end(); ++i)
	{
		BUTTON_INFO* vecButton = &(*i);
		if (pWndChild == vecButton->m_pwndButton)
		{
			SAFE_DELETE(vecButton->m_pwndButton);
			m_vecButton.erase(i);
			SortButton();
			break;
		}
	}
}

BUTTON_INFO* CAdvMgr::FindRunWindowButton(DWORD dwID)
{
	vector<BUTTON_INFO>::iterator i = m_vecButton.begin();
	for (; i != m_vecButton.end(); ++i)
	{
		BUTTON_INFO* vecButton = &(*i);
		if (vecButton->m_pwndButton && vecButton->m_dwRunWindow == dwID)
			return vecButton;
	}

	return NULL;
}


void CAdvMgr::SortButton()
{
	for (int i = 0; i < (int)(m_vecButton.size()); i++)
	{
		if (m_vecButton[i].m_pwndButton)
			m_vecButton[i].m_pwndButton->Move(3 + (i * 40), 135);
	}
}

#if __VER >= 15 // __GUILD_HOUSE
void CWndWorld::ShowCCtrlMenu(CCtrl* pCCtrl)
{
	if (!pCCtrl)
	{
		assert(0);
		return;
	}

	CtrlProp* pProp = prj.GetCtrlProp(pCCtrl->GetIndex());
	if (!pProp)
		return;

	if (!pProp->IsGuildHousingObj())
		return;


	m_wndMenuMover.DeleteAllMenu();
	ReleaseCapture();

	OBJID objId = pCCtrl->GetId();
	GH_Fntr_Info* pInfo = GuildHouse->Find(objId);
	if (!pInfo)
		return;

	int slotIndex = GuildHouse->FindIndex(objId);
	if (slotIndex < 0)
		return;

	m_wndMenuMover.AppendMenu(slotIndex, MMI_GHOUSE_INFO, GETTEXT(TID_APP_INFOMATION));
	m_wndMenuMover.AppendMenu(slotIndex, MMI_GHOUSE_REINSTALL, GETTEXT(TID_MMI_GHOUSE_REINSTALL));
	m_wndMenuMover.AppendMenu(slotIndex, MMI_GHOUSE_RECALL, GETTEXT(TID_MMI_GHOUSE_RECALL));

	CRect rectBound;
	GetBoundRect(pCCtrl, &rectBound);
	m_wndMenuMover.Move(CPoint(rectBound.right, rectBound.top));
	m_wndMenuMover.SetVisible(TRUE);
}

#endif //__GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
BOOL CWndWorld::MenuException(CPoint point)
{
	if (!IsValidObj(g_pPlayer) || g_pPlayer->IsDie())
		return FALSE;
	if (g_pPlayer->m_dwMode & DONMOVE_MODE)
		return FALSE;
	if (g_pPlayer->GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return FALSE;
	if (g_pPlayer->m_vtInfo.VendorIsVendor())
		return FALSE;
#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
		return FALSE;
#endif // __S_SERVER_UNIFY
	if (g_WndMng.GetWndBase(APP_VENDOR_REVISION))
		return FALSE;
#ifdef __EVE_MINIGAME
	if (g_WndMng.GetWndBase(APP_MINIGAME_KAWIBAWIBO))
		return FALSE;
#endif
	if (g_WndMng.GetWndBase(APP_SMELT_JEWEL))
		return FALSE;

	if (GetBuffIconRect(II_SYS_SYS_SCR_RETURN, point))
		return FALSE;
	if (GetBuffIconRect(II_SYS_SYS_SCR_PARTYSUMMON, point))
		return FALSE;

	if (GetBuffIconRect(II_SYS_SYS_SCR_PET_FEED_POCKET, point))
		return FALSE;

	if (CDeployManager::GetInstance()->IsReady())
	{
		if (CDeployManager::GetInstance()->IsCollide())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_HOUSING_INSTALL_FAIL01), NULL, prj.GetTextColor(TID_GAME_HOUSING_INSTALL_FAIL01));
			return FALSE;
		}
		HOUSINGINFO* phousingInfo;
		phousingInfo = CDeployManager::GetInstance()->EndDeploy();
		g_DPlay.SendHousingReqSetupFurniture(*phousingInfo);
		return FALSE;
	}

	if (GuildDeploy()->IsReady())
	{
		if (GuildDeploy()->IsCollide())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_HOUSING_INSTALL_FAIL01), NULL, prj.GetTextColor(TID_GAME_HOUSING_INSTALL_FAIL01));
			return FALSE;
		}
		HOUSING_ITEM* pItem = GuildDeploy()->EndDeploy();
		if (GUILDHOUSE_PCKTTYPE_SETUP == GuildHouse->GetMode())
			GuildHouse->Setup(*pItem);
		else if (GUILDHOUSE_PCKTTYPE_RESET == GuildHouse->GetMode())
			GuildHouse->Reset(*pItem);
		return FALSE;
	}

	return TRUE;
}
#endif // __IMPROVE_SYSTEM_VER15

#ifdef __FL_FFA
void CWndWorld::RenderFFA(C2DRender* p2DRender)
{
	if (!g_pPlayer || !g_pPlayer->GetWorld())
		return;

	if (m_nStateFFA == STATE_CHILL)
		return;

	CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
	g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

	int nX = m_rectWindow.Width() / 2;
	int nY = 110;

	CString strStateName;
	CString str;
	switch (m_nStateFFA)
	{
	case STATE_PREPARE_BATTLE: strStateName = "Match starts in:"; break;
	case STATE_PROCESS_BATTLE: strStateName = "Match ends in:"; break;
	case STATE_PREPARE_CLOSE: strStateName = "Teleport in:"; break;
	default: strStateName = ""; break;
	}
	str.Format("%s %.2dh:%.2dm:%.2ds", strStateName, m_ctFFA.GetHours(), m_ctFFA.GetMinutes(), m_ctFFA.GetSeconds());

	CSize cSize = p2DRender->m_pFont->GetTextExtent(str);
	nX -= (cSize.cx / 2);

	p2DRender->TextOut(nX, nY, str, 0xffffffff, 0xff990099);
	g_Neuz.m_2DRender.SetFont(pOldFont);

	if (!g_pPlayer->GetWorld()->IsFFA())
		return;

	// Render background
	CPoint  cPoint = CPoint(GetClientRect().Width() - 200, 200);
	CRect crBoard;
	crBoard.left = cPoint.x - 5;
	crBoard.top = cPoint.y - 5;
	crBoard.right = cPoint.x + 200;
	crBoard.bottom = crBoard.top + ((vecPlayerFFA.size() + 5) * 16);
	p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(75, 0, 0, 0));

	// Render members
	int	nGap = 16;
	int nCurrentIndex = 0;
	DWORD dwNameColor = 0xFFFFFF99;
	CString strFormat;
	for (auto& it : vecPlayerFFA)
	{
		if (nCurrentIndex > 25)
			break;
		nCurrentIndex++;

		if (it.idPlayer == g_pPlayer->m_idPlayer)
			dwNameColor = 0xFF9ED3FF;
		else
			dwNameColor = 0xFFFFFF99;

		// Place
		strFormat.Format("%d", nCurrentIndex);
		p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwNameColor, 0xFF000000);

		// Name
		strFormat = CPlayerDataCenter::GetInstance()->GetPlayerString(it.idPlayer);
		g_WndMng.StringSize(strFormat, 8);
		p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwNameColor, 0xFF000000);

		strFormat.Format("%d", it.nKills);
		p2DRender->TextOut(cPoint.x + 105, cPoint.y, strFormat, dwNameColor, 0xFF000000);

		strFormat.Format("%d", it.nDeaths);
		p2DRender->TextOut(cPoint.x + 135, cPoint.y, strFormat, dwNameColor, 0xFF000000);

		strFormat.Format("%.2f", static_cast<float>(it.nKills) / (it.nDeaths == 0 ? 1 : it.nDeaths));
		p2DRender->TextOut(cPoint.x + 165, cPoint.y, strFormat, dwNameColor, 0xFF000000);

		cPoint.y += nGap;
	}
}
void CWndWorld::ProcessFFA()
{
	m_tCurrentTimeFFA = m_tEndTimeFFA - time_null();
	m_ctFFA = m_tCurrentTimeFFA >= 0 ? m_tCurrentTimeFFA : 0;
}
void CWndWorld::ClearFFA()
{
	m_nStateFFA = STATE_CHILL;
	m_ctFFA = 0;
	m_tEndTimeFFA = 0;
	m_tCurrentTimeFFA = 0;
	vecPlayerFFA.clear();
}
bool SortFromHighToLow(FFA_PLAYER& first, FFA_PLAYER& second)
{
	return (first.nKills > second.nKills);
}
void CWndWorld::SortFFA()
{
	std::sort(vecPlayerFFA.begin(), vecPlayerFFA.end(), SortFromHighToLow);
}
#endif

void CWndWorld::SetWorldMusic(BOOL bForceStop)
{
	m_dwIdBgmMusic = g_WorldMng()->m_dwIdMusic;
	if (m_dwIdBgmMusic == NULL_ID || m_dwIdBgmMusic == 0)
		m_dwIdBgmMusic = BGM_TH_GENERAL;

	if (bForceStop)
		StopMusic();
}

void CWndWorld::ClearTracking()
{
	if (m_objidTracking == NULL_ID)
		return;

	m_objidTracking = NULL_ID;
	g_pPlayer->ClearDestObj();
	g_DPlay.SendPlayerBehavior();
}

#ifdef __FL_TDM
void CWndWorld::RenderTDM(C2DRender* p2DRender)
{
	if (!g_pPlayer || !g_pPlayer->GetWorld() || !g_pPlayer->GetWorld()->IsTDM())
		return;

	if (m_nStateTDM == STATE_CHILL)
		return;

	RenderTDM_Members(p2DRender);
	RenderTDM_Teams(p2DRender);

}
void CWndWorld::RenderTDM_Members(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
	g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontGuildCombatText);

	int nX = m_rectWindow.Width() / 2;
	int nY = 110;

	CString strStateName;
	CString str;
	switch (m_nStateTDM)
	{
	case STATE_PREPARE_BATTLE: strStateName = "Match starts in:"; break;
	case STATE_PROCESS_BATTLE: strStateName = "Match ends in:"; break;
	case STATE_PREPARE_CLOSE: strStateName = "Teleport in:"; break;
	default: strStateName = ""; break;
	}
	str.Format("%s %.2dh:%.2dm:%.2ds", strStateName, m_ctTDM.GetHours(), m_ctTDM.GetMinutes(), m_ctTDM.GetSeconds());

	CSize cSize = p2DRender->m_pFont->GetTextExtent(str);
	nX -= (cSize.cx / 2);

	p2DRender->TextOut(nX, nY, str, 0xffffffff, 0xff990099);
	g_Neuz.m_2DRender.SetFont(pOldFont);

	// Render background
	CPoint  cPoint = CPoint(GetClientRect().Width() - 200, 200);
	CRect crBoard;
	crBoard.left = cPoint.x - 5;
	crBoard.top = cPoint.y - 5;
	crBoard.right = cPoint.x + 200;
	crBoard.bottom = crBoard.top + ((m_vecPlayersTDM.size() + 5) * 16);
	p2DRender->RenderFillRect(crBoard, D3DCOLOR_ARGB(75, 0, 0, 0));

	// Render members
	int	nGap = 16;
	int nCurrentIndex = 0;
	DWORD dwTextColor = 0xFFFFFFFF;
	CString strFormat;
	for (auto& it : m_vecPlayersTDM)
	{
		if (nCurrentIndex > 25)
			break;

		nCurrentIndex++;

		if (it.idPlayer == g_pPlayer->m_idPlayer)
			dwTextColor = 0xFF9ED3FF;
		else
			dwTextColor = 0xFFFFFF99;

		// Place
		strFormat.Format("%d", nCurrentIndex);
		p2DRender->TextOut(cPoint.x, cPoint.y, strFormat, dwTextColor, 0xFF000000);

		// Name
		strFormat = CPlayerDataCenter::GetInstance()->GetPlayerString(it.idPlayer);
		g_WndMng.StringSize(strFormat, 8);
		p2DRender->TextOut(cPoint.x + 25, cPoint.y, strFormat, dwTextColor, 0xFF000000);

		strFormat.Format("%d", it.nKills);
		p2DRender->TextOut(cPoint.x + 105, cPoint.y, strFormat, dwTextColor, 0xFF000000);

		strFormat.Format("%d", it.nDeaths);
		p2DRender->TextOut(cPoint.x + 135, cPoint.y, strFormat, dwTextColor, 0xFF000000);

		strFormat.Format("%.2f", static_cast<float>(it.nKills) / (it.nDeaths == 0 ? 1 : it.nDeaths));
		p2DRender->TextOut(cPoint.x + 165, cPoint.y, strFormat, dwTextColor, 0xFF000000);

		cPoint.y += nGap;
	}
}
void CWndWorld::RenderTDM_Teams(C2DRender* p2DRender)
{
	const int nX = m_rectWindow.Width() / 2;
	const int nY = 140;
	const int nOffset = 5;

	// Fight texture
	CPoint ptFightTexture = CPoint(nX - m_TextureFightTDM.m_size.cx / 2, nY);
	p2DRender->RenderTexture(ptFightTexture, &m_TextureFightTDM);

	CD3DFont* pOldFont = g_Neuz.m_2DRender.GetFont();
	g_Neuz.m_2DRender.SetFont(CWndBase::m_Theme.m_pFontCaption);

	// Red team
	CString str;  
	str.Format("%04d", m_mapTeamsTDM[TDM_TEAM_RED]);
	CSize size = p2DRender->m_pFont->GetTextExtent(str);
	CPoint ptText = CPoint(nX - m_TextureFightTDM.m_size.cx / 2 - nOffset - size.cx - 4, nY + m_TextureFightTDM.m_size.cy / 2 - size.cy / 2);
	p2DRender->TextOut(ptText.x, ptText.y, str, D3DCOLOR_ARGB(255, 250, 250, 255), 0xFF000000);
	p2DRender->RenderTexture(CPoint(ptText.x - nOffset - m_TextureRedTeam.m_size.cx, ptFightTexture.y), &m_TextureRedTeam);

	// Blue Team
	str.Format("%04d", m_mapTeamsTDM[TDM_TEAM_BLUE]);
	size = p2DRender->m_pFont->GetTextExtent(str);
	ptText = CPoint(nX + m_TextureFightTDM.m_size.cx / 2 + nOffset, nY + m_TextureFightTDM.m_size.cy / 2 - size.cy / 2);
	p2DRender->TextOut(ptText.x, ptText.y, str, D3DCOLOR_ARGB(255, 250, 250, 255), 0xFF000000);
	p2DRender->RenderTexture(CPoint(ptText.x + size.cx + nOffset, ptFightTexture.y), &m_TextureBlueTeam);

	g_Neuz.m_2DRender.SetFont(pOldFont);
}
void CWndWorld::ProcessTDM()
{
	m_tCurrentTimeTDM = m_tEndTimeTDM - time_null();
	m_ctTDM = m_tCurrentTimeTDM >= 0 ? m_tCurrentTimeTDM : 0;
}
void CWndWorld::ClearTDM()
{
	m_nStateTDM = STATE_CHILL;
	m_ctTDM = 0;
	m_tEndTimeTDM = 0;
	m_tCurrentTimeTDM = 0;

	m_vecPlayersTDM.clear();
	m_mapPlayersTDM.clear();
	m_mapTeamsTDM.clear();
}
bool SortTDMFromHighToLow(TDM_PLAYER& first, TDM_PLAYER& second)
{
	return (first.nKills > second.nKills);
}
void CWndWorld::SortTDM()
{
	std::sort(m_vecPlayersTDM.begin(), m_vecPlayersTDM.end(), SortTDMFromHighToLow);
}
bool CWndWorld::IsCanKillTDM(CMover* pDefender)
{
	if (m_nStateTDM != STATE_PROCESS_BATTLE)
		return false;

	if (!IsValidObj(pDefender))
		return false;

	if (!pDefender->IsPlayer())
		return false;

	if (!g_pPlayer->GetWorld() || !pDefender->GetWorld() || !g_pPlayer->GetWorld()->IsTDM() || !pDefender->GetWorld()->IsTDM())
		return false;

	auto itAttacker = m_mapPlayersTDM.find(g_pPlayer->m_idPlayer);
	auto itDefender = m_mapPlayersTDM.find(pDefender->m_idPlayer);
	if (itAttacker == m_mapPlayersTDM.end() || itDefender == m_mapPlayersTDM.end())
		return false;

	if (itAttacker->second.team == itDefender->second.team)
		return false;

	return true;
}
TDM_TEAMS CWndWorld::GetTeamByPlayerId(u_long idPlayer)
{
	auto it = m_mapPlayersTDM.find(idPlayer);
	if (it != m_mapPlayersTDM.end())
		return it->second.team;

	return TDM_TEAM_NONE;
}
DWORD CWndWorld::GetColorByTeam(TDM_TEAMS team)
{
	DWORD dwColor = 0xff000000;
	switch (team)
	{
	case TDM_TEAM_RED: dwColor = 0xffff0000; break;
	case TDM_TEAM_BLUE: dwColor = 0xff0000ff; break;
	default: break;
	}

	return dwColor;
}
#endif