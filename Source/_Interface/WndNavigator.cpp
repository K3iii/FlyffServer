
#include "StdAfx.h"
#include "WndNavigator.h"
#include "WndMapEx.h"
#include "defineText.h"
#include <party.h>
#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;

CWndNavigator::CWndNavigator()
	: m_bObjFilterPlayer(TRUE)
	, m_bObjFilterParty(TRUE)
	, m_bObjFilterNPC(TRUE)
	, m_bObjFilterMonster(TRUE)
	, m_pDestinationPositionTexture(NULL)
	, m_pTexDungeonFog(NULL)
	, m_pTexArrow(NULL)
	, m_pTexAlpha(NULL)
	, m_pTexBlind(NULL)
	, m_iFrame(NULL)
	, m_iPastTime(NULL)
	, m_size(0, 0)
	, m_kSizeCircle(166, 166)
{
	ZeroMemory(m_szName, sizeof(TCHAR) * 64);
	ZeroMemory(m_arrayObjVertex, sizeof(TEXTUREVERTEX) * (MAX_DRAW_OBJECT_COUNT * VERTEXCOUNT_OF_ONE_RECTANGLE));

	m_bNoCloseButton = TRUE;
}

CWndNavigator::~CWndNavigator()
{
	DeleteDeviceObjects();
}


BOOL CWndNavigator::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	ASSERT(pWndParent);
	ASSERT(dwWndId);

	m_texNavPos.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "NavPosition.inc"));
	m_texNavObjs.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "Navigator.inc"));

	m_pTexArrow = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("ImgNavArrow.bmp")), 0xffff00ff);

	ZeroMemory(&m_billboard, sizeof(m_billboard));
	m_billboard.rect.SetRect(0, 0, m_pTexArrow->m_size.cx, m_pTexArrow->m_size.cy);
	m_billboard.ptCenter = CPoint(m_pTexArrow->m_size.cx / 2, m_pTexArrow->m_size.cy / 2);
	m_billArrow.InitDeviceObjects(g_Neuz.m_pd3dDevice, &m_billboard, m_pTexArrow);
	m_billArrow.RestoreDeviceObjects();

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(792, 130), pWndParent);
}

void CWndNavigator::InitializeMenu()
{
	m_wndMenuPlace.CreateMenu(this);

	m_wndMenuPlace.AppendMenu(0, 0, prj.GetText(TID_GAME_PLAYER));
	m_wndMenuPlace.AppendMenu(0, 1, prj.GetText(TID_GAME_PARTYTEXT));
	m_wndMenuPlace.AppendMenu(0, 2, prj.GetText(TID_GAME_NPC));
	m_wndMenuPlace.AppendMenu(0, 3, prj.GetText(TID_GAME_MONSTER));

	m_wndMenuPlace.CheckMenuItem(0, m_bObjFilterPlayer);
	m_wndMenuPlace.CheckMenuItem(1, m_bObjFilterParty);
	m_wndMenuPlace.CheckMenuItem(2, m_bObjFilterNPC);
	m_wndMenuPlace.CheckMenuItem(3, m_bObjFilterMonster);

	m_wndMenuMover.CreateMenu(this);
}

void CWndNavigator::CalculationWindowPosition()
{
	CRect	rectWindow(GetWindowRect());
	CRect	rectRoot(m_pWndRoot->GetLayoutRect());

	CPoint	point(rectRoot.right - rectWindow.Width(), rectRoot.top);

	Move(point);
}

void CWndNavigator::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectClient = GetClientRect();

	int nSizeXY = 20;
	m_wndPlace.Create("", 0, CRect(rectClient.left, 1, rectClient.left + nSizeXY, 1 + nSizeXY), this, 100000);
	m_wndPlace.SetTexture(D3DDEVICE, MakePath(DIR_THEME, "ButtNavLeft.tga"), TRUE);
	m_wndPlace.FitTextureSize();

	m_wndZoomIn.Create("", 0, CRect(rectClient.left, 54, rectClient.left + nSizeXY, 54 + nSizeXY), this, 100005);
	m_wndZoomIn.SetTexture(D3DDEVICE, MakePath(DIR_THEME, "ButtNavZoomIn.tga"), TRUE);
	m_wndZoomIn.FitTextureSize();

	m_wndZoomOut.Create("", 0, CRect(rectClient.left, 70, rectClient.left + nSizeXY, 70 + nSizeXY), this, 100006);
	m_wndZoomOut.SetTexture(D3DDEVICE, MakePath(DIR_THEME, "ButtNavZoomOut.tga"), TRUE);
	m_wndZoomOut.FitTextureSize();

	m_wndNewClose.Create("", 0, CRect(rectClient.left, 10, rectClient.left + nSizeXY, 10 + nSizeXY), this, 100007);
	m_wndNewClose.SetTexture(D3DDEVICE, MakePath(DIR_THEME, "ButtWndExit.tga"), TRUE);
	m_wndNewClose.FitTextureSize();

	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;
	ASSERT(pd3dDevice);

	m_pTexDungeonFog = m_textureMng.AddTexture(pd3dDevice, MakePath(DIR_THEME, _T("NavDunFog.tga")), NULL, TRUE);
	m_pDestinationPositionTexture = m_textureMng.AddTexture(pd3dDevice, MakePath(DIR_THEME, _T("ButtDestination.bmp")), 0xffff00ff);

	m_GuildCombatTextureMask.LoadTexture(pd3dDevice, MakePath(DIR_WORLD_GUILDCOMBAT, "WdGuildWar_Mask.dds"), 0);

	m_size = CSize(256, 256);
	m_nSizeCnt = 0;

	CalculationWindowPosition();
	InitializeMenu();
	ResizeMiniMap();

	CRect rectWnd(GetWndRect());

	m_kSizeCircle.cx = 166;
	m_kSizeCircle.cy = 166;

	m_pTexAlpha = m_textureMng.AddTexture(pd3dDevice, MakePath(DIR_THEME, _T("alphacircle.tga")), 0xffff00ff, TRUE);
	m_pTexBlind = m_textureMng.AddTexture(pd3dDevice, MakePath(DIR_THEME, _T("map3_1.tga")), 0xffff00ff, TRUE);
}


HRESULT CWndNavigator::DeleteDeviceObjects()
{
	m_texNavObjs.DeleteDeviceObjects();
	m_texNavPos.DeleteDeviceObjects();
	m_billArrow.DeleteDeviceObjects();

	m_GuildCombatTextureMask.DeleteDeviceObjects();
	m_kTexRenderTarget.DeleteDeviceObjects();

	return CWndNeuz::DeleteDeviceObjects();
}


HRESULT CWndNavigator::InvalidateDeviceObjects()
{
	m_billArrow.InvalidateDeviceObjects();

	m_kTexRenderTarget.DeleteDeviceObjects();

	return CWndNeuz::InvalidateDeviceObjects();
}

HRESULT CWndNavigator::RestoreDeviceObjects()
{
	m_billArrow.RestoreDeviceObjects();

	m_kTexRenderTarget.CreateTexture(m_pApp->m_pd3dDevice, m_kSizeCircle.cx, m_kSizeCircle.cy, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT);

	ResizeMiniMap();

	return CWndNeuz::RestoreDeviceObjects();
}

void CWndNavigator::OnDraw(C2DRender* p2DRender)
{
	if (NULL == p2DRender)
		return;

	RenderPlayerArrow(p2DRender);
}


void CWndNavigator::PaintFrame(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);

	CSize kSize = CWndBase::m_Theme.m_pFontWndTitle->GetTextExtent(m_strTitle.GetString());
	int nX = (int)((float)m_rectWindow.Width() * 0.5f - (float)kSize.cx * 0.5f);

	p2DRender->TextOut(nX, 8, m_strTitle, m_dwColor);
	p2DRender->SetFont(pOldFont);
	return;

	CWndNeuz::PaintFrame(p2DRender);
}


bool CWndNavigator::RenderLandScape(C2DRender* p2DRender)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return false;

	CWorld* pWorld(g_WorldMng());
	if (NULL == pWorld)
		return false;

	CLandscape* pLand(pWorld->GetLandscape(pWorld->m_pCamera->m_vPos));
	if (pLand == NULL)
		return false;

	CRect	rect(GetClientRect());
	p2DRender->RenderTextureForMinimap(m_rectWindow, &m_kTexRenderTarget, m_pTexAlpha, m_kSizeCircle);

	return true;
}


bool CWndNavigator::IsPointInNavigationCircle(const CPoint ptDestPos)
{
	D3DXVECTOR2		vNaviCenter((float)GetClientCenter().x, (float)GetClientCenter().y);

	float			fLength(0.0f);
	float			fNaviRadius((float)GetClientRect().Width() * 0.3f);

	D3DXVECTOR2 vDestPos(D3DXVECTOR2((float)ptDestPos.x, (float)ptDestPos.y));
	D3DXVECTOR2 vDir(vNaviCenter - vDestPos);
	fLength = D3DXVec2Length(&vDir);
	if (fLength > fNaviRadius)
		return false;
	else
		return true;


	return true;
}

void CWndNavigator::MakeVertexNormalObject(C2DRender* p2DRender, const CPoint ptCenter, int& nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;

	CWorld* pWorld(g_WorldMng());
	if (NULL == pWorld)
		return;

	CLandscape* pLand(pWorld->GetLandscape(pWorld->m_pCamera->m_vPos));
	if (NULL == pLand)
		return;

	CObj* pObj(NULL);
	CPoint			point(0, 0);
	CRect			rtWindow(GetClientRect());

	int				nIndex(NULL);
	int				nPartyMapCount(NULL);
	int				nPartyMap[MAX_PTMEMBER_SIZE] = { NULL, };

	FOR_LAND(pWorld, pLand, pWorld->m_nVisibilityLand, FALSE)
	{
		FOR_OBJ(pLand, pObj, OT_MOVER)
		{
			if (pObj != g_pPlayer)
			{
				CMover* pMover = (CMover*)pObj;
				if (NULL == pMover)
					continue;

				if (m_bObjFilterPlayer == FALSE && pMover->IsPlayer() == TRUE)
				{

					if (m_bObjFilterParty == FALSE)
						continue;
					if (g_Party.IsMember(pMover->m_idPlayer) == FALSE)
						continue;
				}
				if (m_bObjFilterMonster == FALSE && (pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == FALSE))
					continue;

				if (m_bObjFilterNPC == FALSE && (pMover->IsPlayer() == FALSE && pMover->IsPeaceful() == TRUE))
				{

					LPCHARACTER lpCharacter = pMover->GetCharacter();
					if (lpCharacter == NULL || lpCharacter->m_nStructure == -1)
						continue;
				}

				if (pMover->IsMode(TRANSPARENT_MODE) ||
					pMover->IsAuthHigher(AUTH_GAMEMASTER))
					continue;

				CPoint			ptMouse(GetMousePoint());
				D3DXVECTOR3		vPos(pObj->GetPos());

				vPos.x *= (float)m_size.cx / (MAP_SIZE * MPU);
				vPos.z *= (float)m_size.cy / (MAP_SIZE * MPU);
				point.x = (LONG)((rtWindow.Width() / 2) + vPos.x);
				point.y = (LONG)((rtWindow.Height() / 2) - vPos.z);
				point.x -= ptCenter.x;
				point.y -= ptCenter.y;

				if (false == IsPointInNavigationCircle(point))
					continue;

				CRect rectHittest(point.x, point.y, point.x + 5, point.y + 5);

				if (rectHittest.PtInRect(ptMouse))
				{
					ClientToScreen(&ptMouse);
					ClientToScreen(&rectHittest);
					g_toolTip.PutToolTip(10000 + ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName(TRUE), rectHittest, ptMouse, 0);
				}

				int nQuestIndex(pMover->m_nQuestEmoticonIndex >> 8);


				if (pMover->IsNPC() && pMover->m_nQuestEmoticonIndex != -1)
					ppQuest[nQuestNum++] = pMover;
				else if (pMover->IsNPC() && pMover->IsPeaceful())
					nIndex = 1;
				else if (g_pPlayer->m_idGuild && g_pPlayer->m_idGuild == pMover->m_idGuild)
					nIndex = 3;
				else if (pMover->IsNPC() && pMover->IsPeaceful() == FALSE)
					nIndex = 2;
				else if (pMover->IsPlayer())
				{
					nPartyMap[nPartyMapCount] = g_Party.FindMember(pMover->m_idPlayer);
					if (nPartyMap[nPartyMapCount] != -1)
					{
						g_Party.m_aMember[nPartyMap[nPartyMapCount]].m_vPos = pMover->GetPos();
						++nPartyMapCount;
						nIndex = 4;
					}
					else
						nIndex = 0;
				}
				else
				{
					ASSERT(NULL);
				}

				if (wCurrentObjCount >= MAX_DRAW_OBJECT_COUNT)
				{
					ASSERT(NULL);
					continue;
				}

				LPCHARACTER lpCharacter = pMover->GetCharacter();

				if (lpCharacter && lpCharacter->m_nStructure != -1)
					m_texNavObjs.MakeVertex(p2DRender, CPoint(point.x - 8, point.y - 8), 6 + lpCharacter->m_nStructure, &pVertices);
				else
				{

					if (pMover->m_nQuestEmoticonIndex == -1)
					{
						if (pMover->IsRank(RANK_SUPER) || pMover->IsRank(RANK_BOSS))
							m_texNavObjs.MakeVertex(p2DRender, CPoint(point.x - 2, point.y - 2), 5, &pVertices);
						else
							m_texNavObjs.MakeVertex(p2DRender, CPoint(point.x - 2, point.y - 2), nIndex, &pVertices);
					}
				}

				wCurrentObjCount++;
			}
		}
		END_OBJ
	}
	END_LAND
}


void CWndNavigator::MakeVertexQuestEmoticon(C2DRender* p2DRender, const CPoint ptCenter, const int nQuestNum, CMover** ppQuest, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;

	CRect rtWindow(GetClientRect());
}

void CWndNavigator::MakeVertexRegionObject(C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;

	CWorld* pWorld(g_WorldMng());
	if (NULL == pWorld)
		return;

	int			nSize(pWorld->m_aStructure.GetSize());
	CRect		rtWindow(GetClientRect());
	CPoint		point(0, 0);

	for (int i = 0; i < nSize; i++)
	{
		LPREGIONELEM lpRegionElem = pWorld->m_aStructure.GetAt(i);
		if (NULL == lpRegionElem)
			continue;

		D3DXVECTOR3 vPos(lpRegionElem->m_vPos);
		vPos.x *= (float)m_size.cx / (MAP_SIZE * MPU);
		vPos.z *= (float)m_size.cy / (MAP_SIZE * MPU);
		point.x = (LONG)((rtWindow.Width() / 2) + vPos.x);
		point.y = (LONG)((rtWindow.Height() / 2) - vPos.z);

		point.x -= ptCenter.x;
		point.y -= ptCenter.y;

		if (IsPointInNavigationCircle(point))
		{
			if (wCurrentObjCount < MAX_DRAW_OBJECT_COUNT)
			{
				m_texNavObjs.MakeVertex(p2DRender, CPoint(point.x - 8, point.y - 8), 6 + lpRegionElem->m_dwStructure, &pVertices);
				wCurrentObjCount++;
			}
			else
			{
				ASSERT(NULL);
				return;
			}
		}
	}
}

void CWndNavigator::MakeVertexFocusObject(C2DRender* p2DRender, const CPoint ptCenter, WORD& wCurrentObjCount, TEXTUREVERTEX*& pVertices)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;

	CWorld* pWorld(g_WorldMng());
	if (NULL == pWorld)
		return;

	CObj* pObjFocus = pWorld->GetObjFocus();
	if (NULL == pObjFocus)
		return;

	CRect			rtWindow(GetClientRect());
	CPoint			point(0, 0);

	D3DXVECTOR3 vPos(pObjFocus->GetPos());
	vPos.x *= (float)m_size.cx / (MAP_SIZE * MPU);
	vPos.z *= (float)m_size.cy / (MAP_SIZE * MPU);

	point.x = (LONG)((rtWindow.Width() / 2) + vPos.x);
	point.y = (LONG)((rtWindow.Height() / 2) - vPos.z);
	point.x -= ptCenter.x;
	point.y -= ptCenter.y;

	if (IsPointInNavigationCircle(point))
	{
		point.x -= 3;
		point.y -= 3;

		if (wCurrentObjCount < MAX_DRAW_OBJECT_COUNT)
		{
			m_texNavObjs.MakeVertex(p2DRender, point, 5, &pVertices);
			wCurrentObjCount++;
		}
		else
		{
			ASSERT(NULL);
			return;
		}
	}
}

void CWndNavigator::RenderDestinationArrow(C2DRender* p2DRender, const CPoint ptCenter)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;

	D3DXVECTOR3& rDestinationArrow(g_WndMng.m_pWndWorld->m_vDestinationArrow);

	CRect			rect(GetClientRect());
	CPoint			point(0, 0);

	if (rDestinationArrow != D3DXVECTOR3(-1.0F, 0.0F, -1.0F))
	{
		D3DXVECTOR3 vPos(rDestinationArrow);
		vPos.x *= (float)m_size.cx / (MAP_SIZE * MPU);
		vPos.z *= (float)m_size.cy / (MAP_SIZE * MPU);
		point.x = (LONG)((rect.Width() / 2) + vPos.x);
		point.y = (LONG)((rect.Height() / 2) - vPos.z);
		point.x -= ptCenter.x;
		point.y -= ptCenter.y;

		if (IsPointInNavigationCircle(point))
		{
			point.x -= 3;
			point.y -= 3;

			m_pDestinationPositionTexture->RenderScal(p2DRender, point, 255, 0.4F, 0.4F);
		}
	}
}


void CWndNavigator::RenderPlayerArrow(C2DRender* p2DRender)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	if (NULL == pd3dDevice)
		return;


	D3DVIEWPORT9	viewport;

	viewport.X = p2DRender->m_clipRect.left + (p2DRender->m_clipRect.Width() / 2);
	viewport.Y = p2DRender->m_clipRect.top + (p2DRender->m_clipRect.Height() / 2);
	viewport.Width = 32;
	viewport.Height = 32;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	viewport.X -= 16;
	viewport.Y -= 16;

	pd3dDevice->SetViewport(&viewport);

	D3DXMATRIX		matProj;
	D3DXMATRIX		matView;
	D3DXMATRIX		matScale;
	D3DXMATRIX		matWorld;

	D3DXMatrixIdentity(&matProj);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixScaling(&matScale, 1.2f, 1.2f, 1.2f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);

	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);


	D3DXVECTOR3		vDir(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	D3DXVECTOR3		vDestNor(g_pPlayer->GetPos() - g_Neuz.m_camera.m_vPos);
	D3DXVECTOR3		vAxis(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	D3DXVECTOR3		vYPW(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	D3DXQUATERNION  qRot;
	FLOAT			fTheta(0.0f);

	D3DXVec3Normalize(&vDestNor, &vDestNor);
	D3DXVec3Cross(&vAxis, &vDir, &vDestNor);
	fTheta = D3DXVec3Dot(&vDir, &vDestNor);
	D3DXQuaternionRotationAxis(&qRot, &vAxis, acosf(fTheta));

	QuaternionRotationToYPW(qRot, vYPW);
	D3DXMatrixRotationZ(&matScale, -(vYPW.y));

	D3DXMatrixMultiply(&matWorld, &matWorld, &matScale);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	DWORD dwOldAlphaTestEnable(NULL);
	pd3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &dwOldAlphaTestEnable);

	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_billArrow.Render(pd3dDevice);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, dwOldAlphaTestEnable);
}


BOOL CWndNavigator::OnEraseBkgnd(C2DRender* p2DRender)
{
	if (NULL == p2DRender)
		return FALSE;

	LPDIRECT3DDEVICE9	pd3dDevice(p2DRender->m_pd3dDevice);

	if (NULL == pd3dDevice ||
		NULL == g_pPlayer)
		return FALSE;

	CWorld* pWorld(g_WorldMng());
	if (NULL == pWorld)
		return FALSE;

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	if (false == RenderLandScape(p2DRender))
		return FALSE;

	AccuFrame();

	D3DXVECTOR3		vCenter(g_pPlayer->GetPos());

	vCenter.x *= (float)m_size.cx / (MAP_SIZE * MPU);
	vCenter.z *= (float)m_size.cx / (MAP_SIZE * MPU);

	int				xCenter(static_cast<int>(vCenter.x));
	int				yCenter(static_cast<int>(-vCenter.z));

	ZeroMemory(m_arrayObjVertex, sizeof(TEXTUREVERTEX) * (MAX_DRAW_OBJECT_COUNT * VERTEXCOUNT_OF_ONE_RECTANGLE));

	TEXTUREVERTEX* pVertices = m_arrayObjVertex;
	WORD			wObjectCount = 0;

	CMover* apQuest[100] = { NULL, };
	int				nQuestNum(NULL);

	MakeVertexNormalObject(p2DRender, CPoint(xCenter, yCenter), nQuestNum, &apQuest[0], wObjectCount, pVertices);
	MakeVertexRegionObject(p2DRender, CPoint(xCenter, yCenter), wObjectCount, pVertices);
	MakeVertexFocusObject(p2DRender, CPoint(xCenter, yCenter), wObjectCount, pVertices);
	MakeVertexQuestEmoticon(p2DRender, CPoint(xCenter, yCenter), nQuestNum, &apQuest[0], wObjectCount, pVertices);


	m_texNavObjs.Render(m_pApp->m_pd3dDevice, m_arrayObjVertex, wObjectCount * VERTEXCOUNT_OF_ONE_RECTANGLE);

	RenderDestinationArrow(p2DRender, CPoint(xCenter, yCenter));
	RenderMarkAll(p2DRender, g_pPlayer);

	m_wndPlace.EnableWindow(!pWorld->m_bIsIndoor);

	return TRUE;
}



void CWndNavigator::RenderMark(C2DRender* p2DRender, CMover* Player)
{
	FLOAT fx = (FLOAT)m_size.cx / (MAP_SIZE * MPU);
	FLOAT fy = (FLOAT)m_size.cy / (MAP_SIZE * MPU);

	if (Player->m_nvPoint.Time != 0)
	{
		D3DXVECTOR3 Pos = g_pPlayer->GetPos() - Player->m_nvPoint.Pos;
		Pos.x *= fx;
		Pos.z *= -fy;
		Player->m_nvPoint.Time--;

		CPoint point;
		CRect rect = GetClientRect();
		point.x = (LONG)((rect.Width() / 2) - (Pos.x + 2));
		point.y = (LONG)((rect.Height() / 2) - (Pos.z + 2));

		//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
		m_texNavPos.Render(p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

		CRect rectHit(point.x - 8, point.y - 8, point.x + 8, point.y + 8);
		CPoint ptMouse = GetMousePoint();
		if (rectHit.PtInRect(ptMouse))
		{
			//			CString toolTip("Mark : ");
			CString toolTip = prj.GetText(TID_GAME_NAV_MARK);
			toolTip += Player->GetName(TRUE);
			ClientToScreen(&ptMouse);
			ClientToScreen(&rectHit);
			g_toolTip.PutToolTip(10000 + Player->GetId(), toolTip, rectHit, ptMouse, 0);
		}
	}
}


void CWndNavigator::RenderMarkAll(C2DRender* p2DRender, CMover* Player)
{
	RenderMark(p2DRender, Player);

	FLOAT fx = (FLOAT)m_size.cx / (MAP_SIZE * MPU);
	FLOAT fy = (FLOAT)m_size.cy / (MAP_SIZE * MPU);

	V_NaviPoint::iterator nvi = g_pPlayer->m_vOtherPoint.begin();
	for (int i = 0; i < (int)(g_pPlayer->m_vOtherPoint.size()); )
	{
		if (g_pPlayer->m_vOtherPoint.at(i).Time != 0)
		{
			D3DXVECTOR3 Pos = g_pPlayer->GetPos() - g_pPlayer->m_vOtherPoint.at(i).Pos;
			Pos.x *= fx;
			Pos.z *= -fy;
			g_pPlayer->m_vOtherPoint.at(i).Time--;

			CPoint point;
			CRect rect = GetClientRect();
			point.x = (LONG)((rect.Width() / 2) - (Pos.x + 2));
			point.y = (LONG)((rect.Height() / 2) - (Pos.z + 2));

			//m_texNavPos.MakeVertex(p2DRender, point, 1, &pVertices);
			m_texNavPos.Render(p2DRender, point, m_iFrame, 255, 0.5f, 0.5f);

			CRect rectHit(point.x - 8, point.y - 8, point.x + 8, point.y + 8);
			CPoint ptMouse = GetMousePoint();
			if (rectHit.PtInRect(ptMouse))
			{
				//				CString toolTip("Mark : ");
				CString toolTip = prj.GetText(TID_GAME_NAV_MARK);
				toolTip += g_pPlayer->m_vOtherPoint.at(i).Name.c_str();
				ClientToScreen(&ptMouse);
				ClientToScreen(&rectHit);
				g_toolTip.PutToolTip(10000 + g_pPlayer->m_vOtherPoint.at(i).objid, toolTip, rectHit, ptMouse, 0);
			}
			nvi++;
			i++;
		}
		else
		{
			g_pPlayer->m_vOtherPoint.erase(nvi);
		}

	}
}





void CWndNavigator::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	if (ar.IsLoading())
	{
		if (dwVersion == 0)
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
		}
		else
		{
			ar >> m_bObjFilterPlayer >> m_bObjFilterParty >> m_bObjFilterNPC >> m_bObjFilterMonster;
			ar >> m_size.cx >> m_size.cy;
		}
		m_wndMenuPlace.CheckMenuItem(0, m_bObjFilterPlayer);
		m_wndMenuPlace.CheckMenuItem(1, m_bObjFilterParty);
		m_wndMenuPlace.CheckMenuItem(2, m_bObjFilterNPC);
		m_wndMenuPlace.CheckMenuItem(3, m_bObjFilterMonster);
	}
	else
	{
		dwVersion = 1;
		ar << m_bObjFilterPlayer << m_bObjFilterParty << m_bObjFilterNPC << m_bObjFilterMonster;
		ar << m_size.cx << m_size.cy;
	}
}

BOOL CWndNavigator::UpdateRenderTarget()
{
	CWorld* pWorld = g_WorldMng();
	if (!pWorld)
		return FALSE;

	C2DRender* p2DRender = &g_Neuz.m_2DRender;
	if (!p2DRender)
		return FALSE;

	CRect rect = GetClientRect();
	LPDIRECT3DDEVICE9 pd3dDevice = g_Neuz.m_pd3dDevice;

	D3DVIEWPORT9 viewportOld;
	pd3dDevice->GetViewport(&viewportOld);

	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = m_kSizeCircle.cx;//rect.Width();//.cx;
	viewport.Height = m_kSizeCircle.cy;//rect.Height();//size.cy;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&viewport);

	D3DDeviceCombo* pDeviceCombo = g_Neuz.m_d3dSettings.PDeviceCombo();
	HRESULT hr = g_Neuz.m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		pDeviceCombo->AdapterFormat,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		D3DFMT_A8R8G8B8);
	if (hr != D3D_OK)
		return FALSE;

	IDirect3DSurface9* pOldSurface = NULL;
	IDirect3DSurface9* pDstSurface = NULL;


	pd3dDevice->GetRenderTarget(0, &pOldSurface);
	m_kTexRenderTarget.m_pTexture->GetSurfaceLevel(0, &pDstSurface);
	pd3dDevice->SetRenderTarget(0, pDstSurface);//, pOldSurfaceZ );

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000/*CWorld::m_dwBgColor*/, 1.0f, 0);
	if (pd3dDevice->BeginScene() == S_OK)
	{

		CLandscape* pLand = pWorld->GetLandscape(pWorld->m_pCamera->m_vPos);
		if (pLand == NULL)
			return FALSE;

		int nImageBlock = pLand->m_texMiniMap.m_size.cx;

		CRectClip clipRect(0, 0, m_rectWindow.Width(), m_rectWindow.Height()); //p2DRender->m_clipRect.Width(), p2DRender->m_clipRect.Height() );
		CPoint point;
		CRect rectCur;

		FLOAT fx = (FLOAT)m_size.cx / (MAP_SIZE * MPU);
		FLOAT fy = (FLOAT)m_size.cy / (MAP_SIZE * MPU);
		D3DXVECTOR3 vPos, vCenter = (g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3(0, 0, 0));
		vCenter.x *= fx;
		vCenter.z *= fy;
		int xCenter = (int)(vCenter.x);
		int yCenter = (int)(-vCenter.z);

		DWORD dwWorldID = pWorld->GetID();

		for (int z = 0; z < pWorld->m_nLandHeight; z++)
		{
			for (int x = 0; x < pWorld->m_nLandWidth; x++)
			{
				point = CPoint(x * nImageBlock, z * nImageBlock);

				point.y -= (nImageBlock * pWorld->m_nLandHeight);
				point.x += rect.Width() / 2;
				point.y += rect.Height() / 2;
				point.x -= xCenter;
				point.y -= yCenter;
				rectCur.SetRect(point.x, point.y, point.x + nImageBlock, point.y + nImageBlock);
				CLandscape* pLand = pWorld->GetLandscape(x, pWorld->m_nLandHeight - z - 1);
				if (NULL == pLand)
					continue;

				pLand->m_texMiniMap.m_size = m_size;
				if (clipRect.RectLapRect(rectCur))
				{
					if (pLand->m_texMiniMap.m_pTexture)
						pLand->m_texMiniMap.Render(p2DRender, point, 255);//CWndBase::m_nAlpha );
				}

			}

		}

		pd3dDevice->EndScene();
	}  //end of if

	pd3dDevice->SetRenderTarget(0, pOldSurface);//, pOldSurfaceZ );
	pOldSurface->Release();
	pDstSurface->Release();
	pd3dDevice->SetViewport(&viewportOld);

	return TRUE;
}


void CWndNavigator::ResizeMiniMap()
{
	CWorld* pWorld = g_WorldMng();
	if (pWorld == NULL)
		return;
	for (int z = 0; z < pWorld->m_nLandHeight; z++)
	{
		for (int x = 0; x < pWorld->m_nLandWidth; x++)
		{
			CLandscape* pLand = pWorld->GetLandscape(x, pWorld->m_nLandHeight - z - 1);
			if (pLand)
				pLand->m_texMiniMap.m_size = m_size;
		}
	}
}
BOOL CWndNavigator::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CRect rect = GetWindowRect(TRUE);
	CWndBase* pWndBase = (CWndBase*)pLResult;

	if (pWndBase->m_pParentWnd == &m_wndMenuPlace)
	{
		int nState = !m_wndMenuPlace.GetMenuState(nID, 0);
		m_wndMenuPlace.CheckMenuItem(nID, nState);
		SetFocus();
		switch (nID)
		{
		case eObjectFilter::OBJFILTER_PLAYER:
			m_bObjFilterPlayer = nState;
			break;
		case eObjectFilter::OBJFILTER_PARTY:
			m_bObjFilterParty = nState;
			break;
		case eObjectFilter::OBJFILTER_NPC:
			m_bObjFilterNPC = nState;
			break;
		case eObjectFilter::OBJFILTER_MONSTER:
			m_bObjFilterMonster = nState;
			break;
		}
	}
	else if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case 100000:
		{
			CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
			int nMenuPlaceLeft = rect.left - m_wndMenuPlace.GetWindowRect().Width();
			if (nMenuPlaceLeft < rectRootLayout.left)
				m_wndMenuPlace.Move(CPoint(rect.right, rect.top));
			else
				m_wndMenuPlace.Move(CPoint(nMenuPlaceLeft, rect.top));
			m_wndMenuPlace.SetVisible(!m_wndMenuPlace.IsVisible());
			m_wndMenuPlace.SetFocus();
		}
		break;
		case 100005: // zoom in
			m_nSizeCnt = 1;
			m_size.cx += 32;
			m_size.cy += 32;
			if (m_size.cx > 352)
				m_size.cx = 352;
			if (m_size.cy > 352)
				m_size.cy = 352;

			ResizeMiniMap();
			break;
		case 100006: // zoom out
			m_nSizeCnt = -1;
			m_size.cx -= 32;
			m_size.cy -= 32;
			if (m_size.cx < 128)
				m_size.cx = 128;
			if (m_size.cy < 128)
				m_size.cy = 128;
			ResizeMiniMap();
			break;

		case 100007:
			Destroy();
			break;

		case 100008:
		{
#ifdef __CLIENT
			LPWNDAPPLET lpWndApplet = m_resMng.GetAt(GetWndId());
			SAFE_DELETE(g_WndMng.m_pWndHelpInstant);
			g_WndMng.m_pWndHelpInstant = new CWndHelpInstant;
			g_WndMng.m_pWndHelpInstant->m_strHelpKey = lpWndApplet->strToolTip;
			g_WndMng.m_pWndHelpInstant->Initialize();
#endif
		}
		break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndNavigator::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (g_pPlayer == NULL)
		return;
	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
	{
		CRect rect = GetClientRect();
		CPoint pt = point;

		pt.x -= (rect.Width() / 2);
		pt.y -= (rect.Height() / 2);

		FLOAT fx = (FLOAT)m_size.cx / (MAP_SIZE * MPU);
		FLOAT fy = (FLOAT)m_size.cy / (MAP_SIZE * MPU);

		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		vPos.x += (pt.x / fx);
		vPos.z -= (pt.y / fy);

		CString string;
		string.Format("/teleport %d %f %f", g_WorldMng()->m_dwWorldID, vPos.x, vPos.z);
		g_DPlay.SendChat(string);
	}
}

BOOL CWndNavigator::Process()
{
	UpdateRenderTarget();

	return TRUE;
}

BOOL CWndNavigator::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_wndMenuMover)
	{
		if (nID >= 0 && nID < 100000)
		{
			CMover* pMover = prj.GetMover(nID);
			if (IsValidObj(pMover))
			{
				g_WorldMng()->SetObjFocus(pMover);
				SetFocus();

				if (g_WndMng.m_pWndWorld->m_vDestinationArrow != pMover->m_vPos)
					g_WndMng.m_pWndWorld->m_vDestinationArrow = pMover->m_vPos;
				else
					g_WndMng.m_pWndWorld->m_vDestinationArrow = D3DXVECTOR3(-1.0F, 0.0F, -1.0F);
			}
		}
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}



void CWndNavigator::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	rectWnd.left = rectWnd.right - 176;
	rectWnd.bottom = rectWnd.top + 208;

	AdjustMinRect(&rectWnd, 16 * 6, 16 * 7);
	AdjustMaxRect(&rectWnd, 16 * 12, 16 * 13);

	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndNavigator::OnSize(UINT nType, int cx, int cy)
{
	DelWndStyle(WBS_THICKFRAME);

	m_wndPlace.Move(CPoint(22, 15));
	m_wndZoomIn.Move(CPoint(2, 105));
	m_wndZoomOut.Move(CPoint(10, 120));

	m_wndNewClose.Move(CPoint(128, 15));
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndNavigator::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWorld* pWorld = g_WorldMng();
	pWorld->SetObjFocus(NULL);
	g_pPlayer->ClearDest();

	m_wndMenuMover.DeleteAllMenu();
	CLandscape* pLand;
	CObj* pObj;
	int i = 0;

	CMover* pMover = NULL;
	int nTarget = 0;
	FOR_LAND(pWorld, pLand, pWorld->m_nVisibilityLand, FALSE)
	{
		FOR_OBJ(pLand, pObj, OT_MOVER)
		{
			pMover = (CMover*)pObj;
			if (pMover && !pMover->IsPlayer() && pMover->GetCharacter())
			{
				MoverProp* pProp = pMover->GetProp();
				if (pProp && pProp->dwUseable != 1)
				{
					m_wndMenuMover.AppendMenu(i++, ((CMover*)pObj)->GetId(), ((CMover*)pObj)->GetName(TRUE));
					++nTarget;
				}
			}
		}
		END_OBJ
	}
	END_LAND

		if (nTarget > 0)
		{
			CRect rect = GetWindowRect(TRUE);
			CRect rectRootLayout = m_pWndRoot->GetLayoutRect();
			int nMenuMoverLeft = rect.left - m_wndMenuMover.GetWindowRect().Width();
			if (nMenuMoverLeft < rectRootLayout.left)
				m_wndMenuMover.Move(CPoint(rect.right, rect.top));
			else
				m_wndMenuMover.Move(CPoint(nMenuMoverLeft, rect.top));
			m_wndMenuMover.SetVisible(!m_wndMenuMover.IsVisible());
			m_wndMenuMover.SetFocus();
		}
}

void CWndNavigator::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetClientRect();

	CPoint kCenter = rect.CenterPoint();
	D3DXVECTOR3 kPosA((float)kCenter.x, 0.0f, (float)kCenter.y);
	D3DXVECTOR3 kPosB((float)point.x, 0.0f, (float)point.y);
	D3DXVECTOR3 kDiff = kPosB - kPosA;
	float fLen = D3DXVec3Length(&kDiff);

	if (fLen > 65.0f)
		return;

	D3DXVECTOR3 vPos, vCenter = (g_pPlayer != NULL ? g_pPlayer->GetPos() : D3DXVECTOR3(0, 0, 0));
	FLOAT fx = (FLOAT)m_size.cx / (MAP_SIZE * MPU);
	FLOAT fy = (FLOAT)m_size.cy / (MAP_SIZE * MPU);

	CWorld* pWorld = g_WorldMng();

	g_pPlayer->m_nvPoint.Pos.x = vCenter.x + ((float)(point.x - (rect.right / 2)) / fx);
	g_pPlayer->m_nvPoint.Pos.z = vCenter.z - ((float)(point.y - (rect.bottom / 2)) / fy);
	g_pPlayer->m_nvPoint.Time = 200;
	if (pWorld)
	{
		CObj* pObj = pWorld->GetObjFocus();
		if (IsValidObj(pObj) && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer())
		{
			g_DPlay.SendSetNaviPoint(g_pPlayer->m_nvPoint.Pos, ((CCtrl*)pObj)->GetId());
		}
		else
		{
			if (g_Party.IsMember(g_pPlayer->m_idPlayer))
				g_DPlay.SendSetNaviPoint(g_pPlayer->m_nvPoint.Pos, NULL_ID);
		}
	}
}