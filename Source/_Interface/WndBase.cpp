// WndBase.cpp: implementation of the CWndBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resdata.h"

////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////

#define CALL_CHILD_WND(x) for(int i = 0; i < m_wndArray.GetSize(); i++) if(((CWndBase*)m_wndArray.GetAt(i))->x == FALSE) break;

BOOL      CWndBase::m_bCling = TRUE;
BOOL      CWndBase::m_bEdit = FALSE;
BOOL      CWndBase::m_bFullWnd = FALSE;
CWndBase* CWndBase::m_pWndRoot = NULL;
CWndBase* CWndBase::m_pWndFocus = NULL;
CWndBase* CWndBase::m_pCurFocus = NULL;

#ifndef __VS2003
CPtrArray CWndBase::m_wndOrder;
#endif

CPtrArray CWndBase::m_wndRemove;
CPtrArray CWndBase::m_postMessage;
SHORTCUT  CWndBase::m_GlobalShortcut;
//CWndBase* CWndBase::m_pWndWorld = NULL;
CTexture* CWndBase::m_pTexForbid = NULL;
CTimer    CWndBase::m_timerForbid;
BOOL      CWndBase::m_bForbid = FALSE;
CPoint    CWndBase::m_ptForbid = 0;
HWND      CWndBase::m_hWnd = NULL;
HCURSOR   CWndBase::m_hDefaultCursor = NULL;
CWndBase* CWndBase::m_pWndOnMouseMove = NULL;
CWndBase* CWndBase::m_pWndOnSetCursor = NULL;
CPoint    CWndBase::m_pointOld;
CRect     CWndBase::m_rectOld;
int       CWndBase::m_nAlpha = 255;
CTheme    CWndBase::m_Theme;
/*
DWORD     CWndBase::m_dwGlobalShortCutType;
DWORD     CWndBase::m_dwGlobalShortCutIndex;
DWORD     CWndBase::m_dwGlobalShortCutUserId;
DWORD     CWndBase::m_dwGlobalShortCutData;
CString   CWndBase::m_strGlobalShortCutString;
*/

//CTexturePack CWndBase::m_texturePack;
CTextureMng CWndBase::m_textureMng;
CResManager CWndBase::m_resMng;
CWndBase* CWndBase::m_pWndCapture;
CMapStringToPtr CWndBase::m_strWndTileMap;

void CWndBase::FreeTileTexture()
{
	POSITION pos = m_strWndTileMap.GetStartPosition();
	CString string;
	LPIMAGE lpImage;
	while (pos)
	{
		m_strWndTileMap.GetNextAssoc(pos, string, (void*&)lpImage);
		SAFE_DELETE(lpImage);
	}
	m_strWndTileMap.RemoveAll();
}
BOOL CWndBase::SetForbidTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName)
{
	m_pTexForbid = CWndBase::m_textureMng.AddTexture(pd3dDevice, lpszFileName, 0xffff00ff);
	if (m_pTexForbid) return TRUE;
	return FALSE;
}

void CWndBase::SetTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName, BOOL bMyLoader)
{
	m_pTexture = m_textureMng.AddTexture(pd3dDevice, lpszFileName, 0xffff00ff, bMyLoader);
}
void CWndBase::SetTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpKey, CTexture* pTexture)
{
	if (lpKey)
		m_pTexture = m_textureMng.AddTexture(pd3dDevice, lpKey, pTexture);
	else
		m_pTexture = pTexture;
}
void CWndBase::SetForbid(BOOL bForbid)
{
	m_bForbid = bForbid;
	m_timerForbid.Set(500);
	GET_CLIENT_POINT(m_hWnd, point);
	m_ptForbid = point;
}
CWndBase::CWndBase()
{
	m_pApp = NULL;
	m_pTheme = NULL;
	m_pFont = NULL;
	m_nIdWnd = 0;
	m_dwStyle = 0;
	//	m_pSprPack     = NULL ;
	m_bVisible = FALSE;
	m_bLButtonUp = FALSE;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bMButtonDown = FALSE;
	m_bLButtonDowned = FALSE;
	m_bPush = FALSE;
	//m_bCapture     = FALSE;
	m_bPickup = FALSE;
	m_p2DRender = NULL;
	m_pParentWnd = NULL;
	m_ptMouse = 0;
	m_bEnable = TRUE;
	m_pWndFocusChild = NULL;
	m_bTabStop = FALSE;
	m_bDefault = FALSE;
	m_bActive = FALSE;
	m_nToolTipPos = 0;
	m_nResizeDir = 0;
	//m_strSndEffect = "";//Sound\\SndSysLevelUp.wav";
	m_nWinSize = 0;
	m_bFullWnd = FALSE;
	m_pTexture = NULL;
	m_byWndType = WTYPE_BASE;
	m_bKeyButton = FALSE;
	m_nAlphaCount = 255;
	m_dwColor = 0xff000000;
	m_pVB = NULL;
	m_bTile = FALSE;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_bNoCloseButton = FALSE;
#endif // __IMPROVE_QUEST_INTERFACE
}
CWndBase::~CWndBase()
{
	if (m_GlobalShortcut.m_pFromWnd == this)
		m_GlobalShortcut.m_pFromWnd = NULL;

	if (m_pParentWnd && m_pParentWnd->m_pWndFocusChild == this)
		m_pParentWnd->m_pWndFocusChild = NULL;

	if (m_pParentWnd &&
		m_pParentWnd != this)
		m_pParentWnd->RemoveWnd(this);

	for (int i = 0; i < m_wndRemove.GetSize(); i++)
	{
		if (m_wndRemove.GetAt(i) == this)
			m_wndRemove.SetAt(i, NULL);
	}

	if (IsWndRoot())
	{
		RemoveDestroyWnd();		
		DestroyAllWnd(this);	

	}

	m_pParentWnd = NULL;
	DeleteDeviceObjects();
}
void CWndBase::AddWnd(CWndBase* pWndBase)
{
	if (pWndBase != this)
	{
		m_wndArray.Add(pWndBase);
	}
	if (!(pWndBase->m_dwStyle & WBS_CHILD))
	{
		if (pWndBase->m_dwStyle & WBS_MANAGER)
		{
			int i = NULL;
			for (; i < m_wndOrder.GetSize(); i++)
			{
				CWndBase* pWndBase = (CWndBase*)m_wndOrder.GetAt(i);
				if (!(pWndBase->m_dwStyle & WBS_MANAGER))
					break;
			}
			m_wndOrder.InsertAt(i, pWndBase);
		}
		else
		{
			if (m_wndOrder.GetSize())
			{
				if (pWndBase->IsWndStyle(WBS_TOPMOST) || pWndBase->IsWndStyle(WBS_MODAL) || pWndBase->IsWndStyle(WBS_POPUP))
					m_wndOrder.Add(pWndBase);
				else
				{
					int i = NULL;
					for (; i < m_wndOrder.GetSize(); i++)
					{
						CWndBase* pWnd = (CWndBase*)m_wndOrder[i];
						if ((pWnd->IsWndStyle(WBS_TOPMOST) || pWnd->IsWndStyle(WBS_POPUP) || pWnd->IsWndStyle(WBS_MODAL)) && pWnd->IsVisible())
							break;
					}
					m_wndOrder.InsertAt(i, pWndBase);
				}
			}
			else
				m_wndOrder.Add(pWndBase);
		}
		if (!(pWndBase->m_dwStyle & WBS_NOFOCUS))
		{
			if (pWndBase)
				pWndBase->SetFocus();
			else
				m_pWndFocus->OnKillFocus(NULL);
		}
	}
	else
	{
		if (GetFrameWnd()->IsWndStyle(WBS_NOFOCUS) == FALSE)
		{
			if (pWndBase)
				pWndBase->SetFocus();
			else
				m_pWndFocusChild->OnKillFocus(NULL);
		}
	}
}
void CWndBase::RemoveWnd(CWndBase* pWndBase)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWndOrder = (CWndBase*)m_wndOrder.GetAt(i);

		if (pWndOrder == pWndBase)
		{
			m_wndOrder.RemoveAt(i);
			break;
		}
	}

	if (pWndBase == m_pWndFocus)
	{
		CWndBase* pWndBaseTemp = m_pWndFocus->GetParentWnd();
		if (pWndBaseTemp)
			pWndBaseTemp->SetFocus();
	}
	for (int i = 0; i < m_wndArray.GetSize(); i++)
	{
		if (m_wndArray.GetAt(i) == pWndBase)
		{
			((CWndBase*)m_wndArray.GetAt(i))->OnKillFocus(NULL);
			m_wndArray.RemoveAt(i); i--;
			break;
		}
	}
	if (pWndBase == m_pWndFocusChild)
	{
		CWndBase* pWndBaseTemp = m_wndArray.GetSize() ? (CWndBase*)m_wndArray[m_wndArray.GetUpperBound()] : NULL;
		int nSize = m_wndArray.GetSize();
		if (nSize)
		{
			int nidx = m_wndArray.GetUpperBound();
			if (nidx < 0 || nidx >= nSize)
			{
				Error("m_wndArray 범위침범 %d %d", nidx, nSize);
			}
		}
		if (pWndBaseTemp) 
			pWndBaseTemp->SetFocus();
	}
}
void CWndBase::FitTextureSize()
{
	CRect rect = GetWindowRect(TRUE);
	rect.right = rect.left + m_pTexture->m_size.cx;
	rect.bottom = rect.top + m_pTexture->m_size.cy;
	SetWndRect(rect);
}
BOOL CWndBase::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	m_dwStyle |= dwStyle;
	m_bVisible = TRUE;
	m_rectWindow = rect;
	m_rectClient = rect;
	m_rectLayout = rect;
	m_nIdWnd = nID;
	if (pParentWnd)
	{
		m_pParentWnd = pParentWnd;
		if (m_pParentWnd == this)
			m_pWndRoot = m_pParentWnd;
	}
	else
		m_pParentWnd = m_pWndRoot;

	m_pTheme = m_pParentWnd->m_pTheme;
	m_pApp = m_pParentWnd->m_pApp;
	m_pFont = m_pParentWnd->m_pFont;

	SetWndRect(rect, FALSE);
	if (m_pWndRoot == this)
		m_dwStyle |= WBS_MANAGER;
	m_pParentWnd->AddWnd(this);
	GET_CLIENT_POINT(m_pApp->GetSafeHwnd(), point);
	m_ptMouse = point - GetScreenRect().TopLeft();
	OnInitialUpdate();
	OnSize(0, m_rectClient.Width(), m_rectClient.Height());

	return TRUE;
}
void CWndBase::OnDraw(C2DRender* p2DRender)
{
}
BOOL CWndBase::OnDrawIcon(CWndBase* pWndBase, C2DRender* p2DRender)
{
	return FALSE;
}
void CWndBase::PaintRoot(C2DRender* p2DRender)
{
	if (m_nWinSize == 2)
		m_bFullWnd = TRUE;
	m_p2DRender = p2DRender;
	CRectClip rectOld = p2DRender->m_clipRect;
	CPoint ptViewPortOld = p2DRender->GetViewportOrg();
	m_bFullWnd = FALSE;
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pApp->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);



	int nSize = m_wndOrder.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder[i];
		if (pWnd->IsVisible())//&& pWnd != this && pWnd != m_pWndWorld )
		{
			//p2DRender->m_clipRect.OffsetRect(pWnd->m_pParentWnd->m_rectWindow.TopLeft());
			p2DRender->m_clipRect = pWnd->m_rectWindow;
			if (rectOld.Clipping(p2DRender->m_clipRect))
			{
				//p2DRender->m_clipRect+=  rectRoot.TopLeft();
				p2DRender->SetViewportOrg(pWnd->m_rectWindow.TopLeft());
				p2DRender->SetViewport(p2DRender->m_clipRect);
				pWnd->m_rectCurrentWindow = pWnd->m_rectWindow;
				pWnd->m_rectCurrentClient = pWnd->m_rectClient;
				pWnd->Paint(p2DRender);
			}
		}
	}
	p2DRender->m_clipRect = rectOld;
	p2DRender->SetViewportOrg(ptViewPortOld);
	p2DRender->SetViewport(m_pApp->GetDeviceRect());
	if (IsForbid())
	{
		CPoint point = m_ptForbid;
		point.x -= m_pTexForbid->m_size.cx / 2;
		point.y -= m_pTexForbid->m_size.cy / 2;
		p2DRender->RenderTexture(point, m_pTexForbid);
		if (m_timerForbid.TimeOut())
			SetForbid(FALSE);
	}
}
void CWndBase::Paint(C2DRender* p2DRender, BOOL bPaintChild)
{
	if (m_nWinSize == 2)
		m_bFullWnd = TRUE;
	m_p2DRender = p2DRender;

	if (!IsWndStyle(WBS_NODRAWFRAME))
		PaintFrame(p2DRender);

	CRectClip rectOld = p2DRender->m_clipRect;
	CRectClip rectNew = m_rectCurrentClient;

	if (rectOld.Clipping(rectNew))
	{
		if (IsWndStyle(WBS_CHILD) == FALSE)
		{
			p2DRender->SetViewportOrg(m_rectCurrentClient.TopLeft());
			p2DRender->SetViewport(rectNew);
			OnEraseBkgnd(p2DRender);
		}
		p2DRender->SetViewportOrg(m_rectCurrentClient.TopLeft());
		p2DRender->SetViewport(rectNew);
		OnDraw(p2DRender);
		if (bPaintChild)
		{
			p2DRender->SetViewportOrg(rectOld.TopLeft());
			p2DRender->SetViewport(rectOld);
			PaintChild(p2DRender);
		}
	}
}
void CWndBase::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	if (m_pTexture)
	{
		RenderWnd();

		if (IsWndStyle(WBS_CAPTION))
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);

			CSize cs1 = m_pFont->GetTextExtent(m_strTitle);
			p2DRender->TextOut(rect.Width() / 2 - cs1.cx / 2, 3, m_strTitle, m_dwColor);

			p2DRender->SetFont(pOldFont);
		}

	}
	else if (m_strTexture.IsEmpty())
	{
		m_pTheme->RenderWndBaseFrame(p2DRender, &rect);
		if (IsWndStyle(WBS_CAPTION))
		{

			rect.bottom = 21;
			{
				m_pTheme->RenderWndBaseTitleBar(p2DRender, &rect, m_strTitle, m_dwColor);
			}
		}
	}
}
void CWndBase::PaintChild(C2DRender* p2DRender)
{
	CRectClip rectOld = p2DRender->m_clipRect;

	{
		for (int i = 0; i < m_wndArray.GetSize(); i++)
		{
			CWndBase* pWnd = (CWndBase*)m_wndArray[i];
			if (pWnd->IsVisible() && pWnd->IsWndStyle(WBS_CHILD))
			{
				if (pWnd->IsWndStyle(WBS_DOCKING))
				{
					pWnd->m_rectCurrentWindow = pWnd->m_rectWindow + m_rectCurrentWindow.TopLeft();
					pWnd->m_rectCurrentClient = pWnd->m_rectClient + m_rectCurrentWindow.TopLeft();
					p2DRender->m_clipRect = pWnd->m_rectCurrentWindow;
					//if(((CRectClip)m_rectCurrentWindow).Clipping(p2DRender->m_clipRect))
					if (rectOld.Clipping(p2DRender->m_clipRect))
					{
						p2DRender->SetViewport(p2DRender->m_clipRect);
						p2DRender->SetViewportOrg(pWnd->m_rectCurrentWindow.TopLeft());
						pWnd->Paint(p2DRender);
					}
				}
				else
				{
					pWnd->m_rectCurrentWindow = pWnd->m_rectWindow + m_rectCurrentClient.TopLeft();
					pWnd->m_rectCurrentClient = pWnd->m_rectClient + m_rectCurrentClient.TopLeft();
					p2DRender->m_clipRect = pWnd->m_rectCurrentWindow;
					//if(((CRectClip)m_rectCurrentClient).Clipping(p2DRender->m_clipRect))
					if (rectOld.Clipping(p2DRender->m_clipRect))
					{
						p2DRender->SetViewport(p2DRender->m_clipRect);
						p2DRender->SetViewportOrg(pWnd->m_rectCurrentWindow.TopLeft());
						pWnd->Paint(p2DRender);
					}
				}
			}
		}
	}
}
BOOL CWndBase::Process()
{
	if (IsWndRoot())
	{
		RemoveDestroyWnd();
	}
	CALL_CHILD_WND(Process());
	m_nAlphaCount += 8;
	if (m_nAlphaCount > m_nAlpha)
		m_nAlphaCount = m_nAlpha;
	return TRUE;
}
HRESULT CWndBase::InitDeviceObjects()
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
		((CWndBase*)m_wndArray.GetAt(i))->InitDeviceObjects();

	return S_OK;
}
void CWndBase::RenderWnd()
{
	if (m_pTexture == NULL || m_pVB == NULL)
		return;

	LPDIRECT3DDEVICE9 m_pd3dDevice = m_pApp->m_pd3dDevice;
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_nAlphaCount, 0, 0, 0));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, m_pTexture->m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(TEXTUREVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
void CWndBase::MakeVertexBuffer()
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
		((CWndBase*)m_wndArray.GetAt(i))->MakeVertexBuffer();

	if (m_pTexture == NULL || m_pVB == NULL)
		return;
	CRect rect = GetWindowRect();
	rect = GetScreenRect();//WindowToScreen( rect );
	CPoint pt = rect.TopLeft() - m_pTexture->m_ptCenter;
	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = (FLOAT)(pt.x + m_pTexture->m_size.cx);
	FLOAT bottom = (FLOAT)(pt.y + m_pTexture->m_size.cy);
	int nVertexNum = 4;

	TEXTUREVERTEX* pVertices;
	HRESULT hr = m_pVB->Lock(0, sizeof(TEXTUREVERTEX) * nVertexNum, (void**)&pVertices, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return;
	{
		SetTextureVertex(pVertices, (FLOAT)left, (FLOAT)top, m_pTexture->m_fuLT, m_pTexture->m_fvLT);
		pVertices++;
		SetTextureVertex(pVertices, (FLOAT)right, (FLOAT)top, m_pTexture->m_fuRT, m_pTexture->m_fvRT);
		pVertices++;
		SetTextureVertex(pVertices, (FLOAT)left, (FLOAT)bottom, m_pTexture->m_fuLB, m_pTexture->m_fvLB);
		pVertices++;
		SetTextureVertex(pVertices, (FLOAT)right, (FLOAT)bottom, m_pTexture->m_fuRB, m_pTexture->m_fvRB);
		pVertices++;
	}
	m_pVB->Unlock();
}
HRESULT CWndBase::RestoreDeviceObjects()
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
		((CWndBase*)m_wndArray.GetAt(i))->RestoreDeviceObjects();

	if (m_pVB == NULL)
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX) * 4, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_DEFAULT, &m_pVB, NULL);
		MakeVertexBuffer();
	}

	return S_OK;
}
HRESULT CWndBase::InvalidateDeviceObjects()
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
		((CWndBase*)m_wndArray.GetAt(i))->InvalidateDeviceObjects();

	SAFE_RELEASE(m_pVB);

	return S_OK;
}
HRESULT CWndBase::DeleteDeviceObjects()
{
	if (IsWndRoot())
		m_textureMng.DeleteDeviceObjects();

	for (int i = 0; i < m_wndArray.GetSize(); i++)
		((CWndBase*)m_wndArray.GetAt(i))->DeleteDeviceObjects();

	SAFE_RELEASE(m_pVB);

	return S_OK;
}

CRect CWndBase::GetScreenRect()
{
	CWndBase* pWndCur = this;
	CRect rect = pWndCur->m_rectWindow;

	while (pWndCur != m_pWndRoot && pWndCur->IsWndStyle(WBS_CHILD))
	{
		if (pWndCur->IsWndStyle(WBS_DOCKING))
			rect += pWndCur->m_pParentWnd->m_rectWindow.TopLeft();
		else
			rect += pWndCur->m_pParentWnd->m_rectClient.TopLeft();
		pWndCur = pWndCur->m_pParentWnd;
	}
	return rect;
}
LRESULT CWndBase::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return WindowProc(message, wParam, lParam);
}
BOOL CWndBase::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	LPWNDMESSAGE lpWndMessage = new WNDMESSAGE;
	lpWndMessage->m_pWndBase = this;
	lpWndMessage->m_message = message;
	lpWndMessage->m_wParam = wParam;
	lpWndMessage->m_lParam = lParam;
	m_postMessage.Add(lpWndMessage);
	return TRUE;
}
void CWndBase::SetChildFocus(CWndBase* pWndBase, POINT point)
{
	for (int i = 0; i < pWndBase->m_wndArray.GetSize(); i++)
	{
		CWndBase* pWndCur = (CWndBase*)pWndBase->m_wndArray.GetAt(i);
		if (pWndCur->IsWndStyle(WBS_CHILD) && pWndCur->IsVisible() && pWndCur->IsWindowEnabled())
		{
			if (pWndCur->GetScreenRect().PtInRect(point))
				pWndCur->SetFocus();
			SetChildFocus(pWndCur, point);
		}
	}
}
CWndBase* CWndBase::GetChildFocus(CWndBase* pWndBase, POINT point)
{
	CWndBase* pWndSelect = pWndBase;
	CWndBase* pWndTemp = NULL;
	for (int i = 0; i < pWndBase->m_wndArray.GetSize(); i++)
	{
		CWndBase* pWndCur = (CWndBase*)pWndBase->m_wndArray.GetAt(i);
		if (pWndCur->IsWndStyle(WBS_CHILD) && pWndCur->IsVisible() && pWndCur->IsWindowEnabled())
		{
			CPoint ptWindow = point;
			pWndCur->ScreenToWindow(&ptWindow);

			if (pWndCur->IsPickupSpace(ptWindow))
				pWndSelect = pWndCur;

			pWndTemp = GetChildFocus(pWndCur, point);
			if (pWndTemp != pWndCur)
				pWndSelect = pWndTemp;
		}
	}
	return pWndSelect;
}
LRESULT CWndBase::WindowRootProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pApp == NULL)
		return 0;

	m_rectCurrentWindow = m_rectWindow;
	m_rectCurrentClient = m_rectClient;

	GET_CLIENT_POINT(m_pApp->GetSafeHwnd(), point);
	CRect rectWnd = m_rectWindow;
	CPoint ptClient = point - m_rectCurrentClient.TopLeft();
	CPoint ptWindow = point - m_rectCurrentWindow.TopLeft();
	m_ptMouse = ptClient;
	if (message == WM_MOUSEMOVE)
	{
		static CPoint ptOld;
		if (point == ptOld)
			return 0;
		ptOld = point;
	}
	if (message == WM_SETFOCUS || message == WM_KILLFOCUS || message == WM_GETFONT || message == WM_CAPTURECHANGED)
		return 0;
	if (message == WM_IME_SETCONTEXT || message == WM_IME_NOTIFY || message == WM_IME_CONTROL || message == WM_IME_COMPOSITIONFULL || message == WM_IME_SELECT)
		return 0;
	if (message == WM_GETFONT || message == WM_GETTEXT || message == WM_IME_NOTIFY || message == WM_IME_CHAR || message == WM_IME_KEYDOWN || message == WM_IME_KEYUP || message == 0x287)
		return 0;


	CWndBase* pWndOnMouseMain = NULL;
	CWndBase* pWndOnMouseChild = NULL;
	m_pWndOnSetCursor = NULL;
	m_pWndOnMouseMove = NULL;
	if (m_pWndCapture)
	{
		m_pWndOnMouseMove = m_pWndCapture;
		m_pWndOnSetCursor = m_pWndCapture;
		pWndOnMouseMain = m_pWndCapture;
	}
	else
		for (int i = m_wndOrder.GetSize() - 1; i >= 0; i--)
		{
			CWndBase* pWndBase = (CWndBase*)m_wndOrder[i];
			if (pWndBase->IsVisible())
			{
				CRect rect = pWndBase->GetScreenRect();
				if (rect.PtInRect(ptClient))
				{
					pWndOnMouseMain = pWndBase;
					if (m_pWndCapture == NULL)
					{
						CWndBase* pWndChild = GetChildFocus(pWndBase, ptClient);
						if (pWndBase != pWndChild)
						{
							pWndBase = pWndChild;
							pWndOnMouseChild = pWndChild;
						}
						m_pWndOnSetCursor = pWndBase;
						m_pWndOnMouseMove = pWndBase;
					}
					break;
				}
			}
		}

	switch (message)
	{
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDBLCLK: case WM_LBUTTONDBLCLK:
		g_toolTip.CancelToolTip();
		g_toolTipSub1.CancelToolTip();
		g_toolTipSub2.CancelToolTip();

		if (m_pWndFocus && m_pWndFocus->m_dwStyle & WBS_MODAL)
		{
			GetChildFocus(m_pWndFocus, ptClient)->SetFocus();
			break;
		}
		if (pWndOnMouseMain)
			pWndOnMouseMain->SetFocus();
		if (pWndOnMouseChild)
			pWndOnMouseChild->SetFocus();
		break;
	case WM_LBUTTONUP:
		if (m_GlobalShortcut.m_dwShortcut != SHORTCUT_NONE)
		{
			if (pWndOnMouseChild == NULL)
				pWndOnMouseChild = pWndOnMouseMain;
			if (pWndOnMouseChild)
			{
				pWndOnMouseChild->OnDropIcon(&m_GlobalShortcut, pWndOnMouseChild->GetMousePoint());
				if (pWndOnMouseChild->IsWndStyle(WBS_POPUP))
				{
					CWndBase* pWndTemp = pWndOnMouseChild;
					while (pWndTemp && pWndTemp->IsWndStyle(WBS_POPUP))
						pWndTemp = (CWndMenu*)pWndTemp->GetParentWnd();
					if (pWndTemp) pWndTemp->m_pParentWnd->SetFocus();
				}
			}

			ZeroMemory(&m_GlobalShortcut, sizeof(m_GlobalShortcut));
		}
		break;
	}
	return WindowProc(message, wParam, lParam);
}

LRESULT CWndBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pParentWnd && IsWndStyle(WBS_CHILD))
	{
		if (IsWndStyle(WBS_DOCKING))
		{
			m_rectCurrentWindow = m_rectWindow + m_pParentWnd->m_rectCurrentWindow.TopLeft();
			m_rectCurrentClient = m_rectClient + m_pParentWnd->m_rectCurrentWindow.TopLeft();
		}
		else
		{
			m_rectCurrentWindow = m_rectWindow + m_pParentWnd->m_rectCurrentClient.TopLeft();
			m_rectCurrentClient = m_rectClient + m_pParentWnd->m_rectCurrentClient.TopLeft();
		}
	}
	else
	{
		if (1) //m_pParentWnd == this )
		{
			m_rectCurrentWindow = m_rectWindow;
			m_rectCurrentClient = m_rectClient;
		}
		else
		{
			m_rectCurrentWindow = m_rectWindow + m_pParentWnd->m_rectCurrentClient.TopLeft();
			m_rectCurrentClient = m_rectClient + m_pParentWnd->m_rectCurrentClient.TopLeft();
		}
	}
	GET_CLIENT_POINT(m_pApp->GetSafeHwnd(), point);
	CRect rectWnd = m_rectWindow;
	CPoint ptClient = point - m_rectCurrentClient.TopLeft();
	CPoint ptWindow = point - m_rectCurrentWindow.TopLeft();
	m_ptMouse = ptClient;
	if (IsVisible() == FALSE)
		return 0;
	int i;

	if (IsVisible())
	{
		if (m_pWndOnMouseMove == this && m_strToolTip.IsEmpty() == FALSE)
			g_toolTip.PutToolTip((DWORD)this, m_strToolTip, m_rectCurrentClient, point, m_nToolTipPos);

		if (m_pCurFocus == this)
		{
			CRect rect = GetWindowRect();
			if (GetWindowRect().PtInRect(GetMousePoint()) && (m_bLButtonDown || m_bRButtonDown))
				m_bPush = TRUE;
			else
				if (m_bKeyButton == FALSE)
					m_bPush = FALSE;
			if (m_bPush == FALSE)
				m_bKeyButton = FALSE;
			if (IsWindowEnabled())
				DefWindowProc(message, wParam, lParam);
		}

		if (g_WndMng.IsWindowProcessState())
		{
			if (m_pWndOnMouseMove == this)
			{
				if (message == WM_MOUSEMOVE)
					OnMouseMove(wParam, ptClient);
				OnMouseWndSurface(ptClient);
			}
		}

		if (IsWindowEnabled())
		{
			BOOL bMouseInChildWindow = FALSE;
			CWndBase* pWndMouseInChildWindow = NULL;
			for (i = m_wndArray.GetSize() - 1; i >= 0; i--)
			{
				CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt(i);
				pWndBase->WindowProc(message, wParam, lParam);
				if (pWndMouseInChildWindow == NULL && pWndBase->GetWindowRect(TRUE).PtInRect(GetMousePoint()))
					pWndMouseInChildWindow = pWndBase;
			}
		}
	}
	if (m_pWndOnSetCursor == this)
		m_pWndOnSetCursor->OnSetCursor(this, 0, 0);
	switch (message)
	{
	case WM_SETFOCUS:
		SetFocus();
		break;
	case WM_KILLFOCUS:
		//KillFocus();
		break;
	case WM_LBUTTONUP:
		m_bPush = FALSE;
		m_nResizeDir = 0;
		m_bLButtonUp = TRUE;
		m_bLButtonDown = FALSE;
		if (m_bPickup == TRUE)
		{
			m_bPickup = FALSE;
			ReleaseCapture();
		}
		break;
	case WM_RBUTTONUP:
		m_bPush = FALSE;
		m_bRButtonDown = FALSE;
		break;

	case WM_MBUTTONUP:
		m_bPush = FALSE;
		m_bMButtonDown = FALSE;
		break;
	}

	if (rectWnd != m_rectWindow)
	{
		SetWndRect(m_rectWindow);
		rectWnd = m_rectWindow;
	}
	else

		if (m_nResizeDir)
		{
			// 1 = top
			// 2 = bottom
			// 3 = left
			// 4 = right
			// 5 = topLeft
			// 6 = topRight
			// 7 = bottomLeft
			// 8 = bottomRigh;
			//CPoint pt = ptWindow - m_pointOld;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			if (point.x < 0 || point.y < 0 || point.x > m_pWndRoot->m_rectWindow.right || point.y > m_pWndRoot->m_rectWindow.bottom)
			{
				m_bPush = FALSE;
				m_nResizeDir = 0;
				m_bLButtonUp = TRUE;
				m_bLButtonDown = FALSE;
				if (m_bPickup == TRUE)
				{
					m_bPickup = FALSE;
					ReleaseCapture();
				}
			}
#endif // __IMPROVE_QUEST_INTERFACE
			CPoint pt = point - m_pointOld;
			//#ifdef __NEWINTERFACE
			pt.x = (pt.x / 16) * 16;
			pt.y = (pt.y / 16) * 16;
			//#endif
					//CPoint pt = point - m_pointOld;
			if (m_nWinSize == 0)
			{
				switch (m_nResizeDir)
				{
				case 1: rectWnd.top = m_rectOld.top + pt.y; break;
				case 2: rectWnd.bottom = m_rectOld.bottom + pt.y; break;
				case 3: rectWnd.left = m_rectOld.left + pt.x; break;
				case 4: rectWnd.right = m_rectOld.right + pt.x; break;
				case 5: rectWnd.left = m_rectOld.left + pt.x; rectWnd.top = m_rectOld.top + pt.y; break;
				case 6: rectWnd.right = m_rectOld.right + pt.x; rectWnd.top = m_rectOld.top + pt.y; break;
				case 7: rectWnd.left = m_rectOld.left + pt.x; rectWnd.bottom = m_rectOld.bottom + pt.y; break;
				case 8: rectWnd.right = m_rectOld.right + pt.x; rectWnd.bottom = m_rectOld.bottom + pt.y; break;
				}
				if (m_bCling) //&& !IsWndStyle( WBS_NOCLING ) )
				{
					CRect rect;
					//if( m_pParentWnd == m_pWndRoot )
					//	rect = m_pParentWnd->GetClientRect();
					//else
						//rect = m_pParentWnd->GetLayoutRect();

					//rect = m_pParentWnd->GetLayoutRect();
					rect = m_pWndRoot->GetLayoutRect();
					if (rectWnd.top < rect.top + 10) rectWnd.top = rect.top;
					if (rectWnd.bottom > rect.bottom - 10) rectWnd.bottom = rect.bottom;
					if (rectWnd.left < rect.left + 10) rectWnd.left = rect.left;
					if (rectWnd.right > rect.right - 10) rectWnd.right = rect.right;
				}
			}
			else
				if (m_nWinSize == 2)
				{
					if (m_nResizeDir == 3)
					{
						rectWnd.left += pt.x;

						for (int i = 0; i < m_wndOrder.GetSize(); i++)
						{
							CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
							if (pWnd != this && pWnd->m_nWinSize == 2)
								pWnd->SetWndRect(rectWnd);
						}
					}
				}
			SetWndRect(rectWnd, TRUE);
			//	m_pointOld = point - m_rectWindow.TopLeft();
		}
		else

			if (m_bPickup == TRUE)
			{
				if (m_nWinSize == 0)
				{
					CPoint pt = point - m_pointOld;
					int w = rectWnd.Width();
					int h = rectWnd.Height();

					if (m_bCling)//&& !IsWndStyle( WBS_NOCLING ) )
					{
						CRect rect = m_pWndRoot->GetLayoutRect();

						if (pt.x < rect.left + 10 && pt.x > rect.left) pt.x = rect.left;
						if (pt.y < rect.top + 10 && pt.y > rect.top) pt.y = rect.top;
						if (pt.x + w > rect.right - 10 && pt.x + w < rect.right) pt.x = rect.right - w;
						if (pt.y + h > rect.bottom - 10 && pt.y + h < rect.bottom) pt.y = rect.bottom - h;

						if (pt.x < rect.left) pt.x = rect.left;
						if (pt.y < rect.top) pt.y = rect.top;
						if (pt.x + w > rect.right) pt.x = rect.right - w;
						if (pt.y + h > rect.bottom) pt.y = rect.bottom - h;
						//rectWnd.SetRect( pt.x, pt.y, pt.x + w, pt.y + h );
					}
					rectWnd.SetRect(pt.x, pt.y, pt.x + w, pt.y + h);
					SetWndRect(rectWnd, TRUE);
					//m_pointOld = point;
				}
			}
	if (IsWndRoot())
	{
		RemoveDestroyWnd();

		for (i = 0; i < m_postMessage.GetSize(); i++)
		{
			LPWNDMESSAGE lpWndMessage = (LPWNDMESSAGE)m_postMessage.GetAt(i);
			lpWndMessage->m_pWndBase->WindowProc(lpWndMessage->m_message, lpWndMessage->m_wParam, lpWndMessage->m_lParam);
			SAFE_DELETE(lpWndMessage);
			m_postMessage.RemoveAt(i--);
		}
	}

	return TRUE;
}
LRESULT CWndBase::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	GET_CLIENT_POINT(m_pApp->GetSafeHwnd(), point);
	CPoint ptClient = point - m_rectCurrentClient.TopLeft();
	CPoint ptWindow = point - m_rectCurrentWindow.TopLeft();
	CRect rectWnd = m_rectWindow;


	switch (message)
	{
	case WM_LBUTTONUP:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			OnLButtonUp(wParam, ptClient);
			if (m_dwStyle & WBS_CHILD && m_bPush == TRUE && m_bEnable == TRUE)
			{
				m_pParentWnd->OnCommand(m_nIdWnd, message, this);
				//TRACE( "WM_LBUTTONUP\n" );
			}
		}
		else if (m_rectCurrentWindow.PtInRect(point))
		{
			OnLButtonUp(wParam, ptWindow);
		}
		//ReleaseCapture();
		break;
	case WM_LBUTTONDOWN:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			m_pointOld = point - rectWnd.TopLeft();
			m_bPush = TRUE;
			m_bLButtonDown = TRUE;
			m_bLButtonUp = FALSE;
			OnLButtonDown(wParam, ptClient);
		}
		else if (m_rectCurrentWindow.PtInRect(point))
		{
			m_bPush = TRUE;
			m_bLButtonDown = TRUE;
			m_bLButtonUp = FALSE;
			m_pointOld = point - rectWnd.TopLeft();
			if (m_pCurFocus == this)
			{
				//SetCapture();
				if ((m_dwStyle & WBS_THICKFRAME))
				{
					// 1 = top
					// 2 = bottom
					// 3 = left
					// 4 = right
					// 5 = topLeft
					// 6 = topRight
					// 7 = bottomLeft
					// 8 = bottomRigh;
					m_nResizeDir = GetResizeDir(ptWindow);
					if (m_nResizeDir)
					{
						m_pointOld = point;// - rectWnd.TopLeft();
						m_rectOld = rectWnd;
					}
				}
			}
		}
		if (m_pWndFocus == this)
		{
			if (m_nResizeDir == 0)
			{
				if ((m_dwStyle & WBS_MOVE) && IsPickupSpace(ptWindow))
				{
					m_bPickup = TRUE;
					SetCapture();
				}
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			m_bPush = TRUE;
			m_bLButtonDown = TRUE;
			m_bLButtonUp = FALSE;
			m_pointOld = point - rectWnd.TopLeft();
			//if((m_dwStyle & WBS_MOVE) && m_pWndFocus == this && IsPickupSpace(ptClient))
				//m_bPickup = TRUE;
			OnLButtonDblClk(wParam, ptClient);
			if (m_dwStyle & WBS_CHILD && m_bEnable == TRUE)
				m_pParentWnd->OnCommand(m_nIdWnd, message);
		}
		else
			if (m_rectCurrentWindow.PtInRect(point))
			{
				m_bPush = TRUE;
				m_bLButtonDown = TRUE;
				m_bLButtonUp = FALSE;
				m_pointOld = point - rectWnd.TopLeft();
				//if((m_dwStyle & WBS_MOVE) && m_pWndFocus == this && IsPickupSpace(ptClient))
					//m_bPickup = TRUE;
				OnNonClientLButtonDblClk(wParam, ptClient);
				if (m_dwStyle & WBS_CHILD && m_bEnable == TRUE)
					m_pParentWnd->OnCommand(m_nIdWnd, message);
			}
		break;
	case WM_RBUTTONDBLCLK:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			//m_bPush = TRUE;
			//m_bRButtonDown = TRUE;
			if (m_dwStyle & WBS_CHILD && m_bEnable == TRUE)
				m_pParentWnd->OnCommand(m_nIdWnd, message);
			OnRButtonDblClk(wParam, ptClient);
		}
		break;
	case WM_MBUTTONDBLCLK:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			m_bPush = TRUE;
			m_bMButtonDown = TRUE;
			m_pointOld = point - rectWnd.TopLeft();
			//if((m_dwStyle & WBS_MOVE) && m_pWndFocus == this && IsPickupSpace(ptClient))
				//m_bPickup = TRUE;
			OnMButtonDblClk(wParam, ptClient);
			if (m_dwStyle & WBS_CHILD && m_bEnable == TRUE)
				m_pParentWnd->OnCommand(m_nIdWnd, message);
		}
		break;
	case WM_RBUTTONUP:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			OnRButtonUp(wParam, ptClient);
			if (m_dwStyle & WBS_CHILD && m_bPush == TRUE && m_bEnable == TRUE)
				m_pParentWnd->OnCommand(m_nIdWnd, message, this);
		}
		break;
	case WM_RBUTTONDOWN:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			m_bPush = TRUE;
			m_bRButtonDown = TRUE;
			OnRButtonDown(wParam, ptClient);
		}
		break;
	case WM_MBUTTONUP:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			OnMButtonUp(wParam, ptClient);
			if (m_dwStyle & WBS_CHILD && m_bPush == TRUE && m_bEnable == TRUE)
				m_pParentWnd->OnCommand(m_nIdWnd, message);
		}
		break;
	case WM_MBUTTONDOWN:
		if (m_rectCurrentClient.PtInRect(point) || m_pWndCapture == this)
		{
			m_bPush = TRUE;
			m_bMButtonDown = TRUE;
			OnMButtonDown(wParam, ptClient);
		}
		break;
		/*
	case WM_MOUSEMOVE:
		if( m_pWndOnMouseMove == this )
		{
			//if( m_rectCurrentClient.PtInRect( point ) ||  m_pWndCapture == this  )
			OnMouseMove(wParam,ptClient);
		}
		//else
		//	m_bPush = FALSE;
		break;
		*/
	case WM_MOUSEWHEEL:
		if (m_rectCurrentClient.PtInRect(point))
		{
			WORD fwKeys = LOWORD(wParam);    // key flags
			WORD zDelta = (short)HIWORD(wParam);    // wheel rotation
			int xPos = (short)LOWORD(lParam);    // horizontal position of pointer
			int yPos = (short)HIWORD(lParam);
			OnMouseWheel(fwKeys, zDelta, CPoint(xPos, yPos));
		}
		break;
	case WM_SIZE:
		//	OnSize(m_nWinSize,0,0);
		break;
	case WM_MOVE:
		OnMove(ptClient.x, ptClient.y);
		break;
	case WM_KEYDOWN:
		if (m_pWndFocus->IsWndStyle(WBS_KEY))
		{
			CWndBase* pWndDefault = this;
			if (wParam == VK_RETURN)
			{
				for (int i = 0; i < m_pWndFocus->m_wndArray.GetSize(); i++)
				{
					if (((CWndBase*)m_pWndFocus->m_wndArray.GetAt(i))->IsDefault())
					{
						pWndDefault = ((CWndBase*)m_pWndFocus->m_wndArray.GetAt(i));
						wParam = VK_SPACE;
						if (this != pWndDefault)
							pWndDefault->PostMessage(WM_SETFOCUS);
						break;
					}
				}
			}
			if (pWndDefault->IsWndStyle(WBS_CHILD) && pWndDefault->m_bPush == FALSE)
			{
				if (wParam == VK_SPACE)
				{
					pWndDefault->m_bKeyButton = TRUE;
					pWndDefault->m_bPush = TRUE;
					pWndDefault->m_bLButtonDown = TRUE;
					pWndDefault->m_bLButtonUp = FALSE;
					pWndDefault->m_pointOld = point - rectWnd.TopLeft();
					if ((pWndDefault->m_dwStyle & WBS_MOVE) && pWndDefault->m_pWndFocus == this && pWndDefault->IsPickupSpace(ptClient))
						pWndDefault->m_bPickup = TRUE;
					pWndDefault->OnLButtonDown(wParam, ptClient);
				}
			}
		}
		OnKeyDown(wParam, LOWORD(lParam), HIWORD(lParam));
		if (m_pWndFocus != this)
			m_pWndFocus->OnKeyDown(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYUP:
		if (m_pWndFocus->IsWndStyle(WBS_KEY))
		{
			CWndBase* pWndDefault = this;
			UINT idWnd = pWndDefault->m_nIdWnd;
			if (wParam == VK_RETURN)
			{
				for (int i = 0; i < m_pWndFocus->m_wndArray.GetSize(); i++)
				{
					if (((CWndBase*)m_pWndFocus->m_wndArray.GetAt(i))->IsDefault())
					{
						wParam = VK_SPACE;
						idWnd = ((CWndBase*)m_pWndFocus->m_wndArray.GetAt(i))->GetWndId();
						pWndDefault = ((CWndBase*)m_pWndFocus->m_wndArray.GetAt(i));
						if (this != pWndDefault)
							pWndDefault->PostMessage(WM_SETFOCUS);
						break;
					}
				}
			}

			if (pWndDefault->IsWndStyle(WBS_CHILD) && pWndDefault->m_bKeyButton == TRUE && pWndDefault->m_bEnable == TRUE)
			{
				if (wParam == VK_SPACE)
				{
					pWndDefault->OnLButtonUp(wParam, ptClient);
					pWndDefault->m_bPush = FALSE;
					pWndDefault->m_bLButtonDown = FALSE;
					pWndDefault->m_bLButtonUp = TRUE;
				}
			}
		}
		OnKeyUp(wParam, LOWORD(lParam), HIWORD(lParam));
		if (m_pWndFocus != this)
			m_pWndFocus->OnKeyUp(wParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		break;
	case WM_CHAR:
		if (m_pWndFocus->IsWndStyle(WBS_KEY))
		{
			{
				if (wParam == VK_TAB)
				{
					CWndBase* pWndTemp = m_pWndFocus;
					CWndBase* pWndBase;
					int nCurIdx = -1;
					int i = NULL;
					for (; i < pWndTemp->m_wndArray.GetSize() && nCurIdx == -1; i++)
					{
						if (((CWndBase*)pWndTemp->m_wndArray.GetAt(i))->IsFocusChild())
							nCurIdx = i;
					}
					if (nCurIdx != -1)
					{
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
						{
							i = nCurIdx - 1;
							if (i < 0)
								i = pWndTemp->m_wndArray.GetSize() - 1;
							pWndBase = (CWndBase*)pWndTemp->m_wndArray.GetAt(i);
							while (pWndBase != pWndTemp->GetFocusChild())
							{
								pWndBase = (CWndBase*)pWndTemp->m_wndArray.GetAt(i--);
								if (i < 0) i = pWndTemp->m_wndArray.GetSize() - 1;
								if (pWndBase->m_bTabStop)
								{
									pWndBase->PostMessage(WM_SETFOCUS);
									break;
								}
							}
						}
						else
						{
							i = nCurIdx + 1;
							if (i >= pWndTemp->m_wndArray.GetSize()) i = 0;
							pWndBase = (CWndBase*)pWndTemp->m_wndArray.GetAt(i);
							while (pWndBase != pWndTemp->GetFocusChild())
							{
								pWndBase = (CWndBase*)pWndTemp->m_wndArray.GetAt(i++);
								if (i >= pWndTemp->m_wndArray.GetSize()) i = 0;
								if (pWndBase->m_bTabStop)
								{
									pWndBase->PostMessage(WM_SETFOCUS);
									break;
								}
							}
						}
					}
				}
			}
		}
		OnChar(wParam);

	}
	return 1;
}
void CWndBase::SetFocus()
{
	if (IsWndStyle(WBS_CHILD))
	{
		if (m_pParentWnd->m_pWndFocusChild != this)
		{
			if (m_pParentWnd->m_pWndFocusChild)
				m_pParentWnd->m_pWndFocusChild->OnKillFocus(this);

			OnSetFocus(m_pParentWnd->m_pWndFocusChild);
			m_pParentWnd->m_pWndFocusChild = this;
		}
		else
			OnSetFocus(this);

		m_pWndFocus = GetFrameWnd();
		m_pCurFocus = this;
	}
	else
	{
		if (!IsWndStyle(WBS_CHILD) && m_wndOrder.GetSize())
		{
			if (!(m_dwStyle & WBS_MANAGER) && this != m_pWndRoot)
			{
				CWndBase* pWndTop = (CWndBase*)m_wndOrder.GetAt(m_wndOrder.GetUpperBound());
				for (int i = m_wndOrder.GetSize() - 1; i >= 0; i--)
				{
					CWndBase* pWnd = (CWndBase*)m_wndOrder[i];
					if (this == pWnd && pWnd->IsVisible())
					{
						m_wndOrder.RemoveAt(i);
						for (; i < m_wndOrder.GetSize(); i++)
						{
							CWndBase* pWnd2 = (CWndBase*)m_wndOrder[i];
							if ((pWnd2->IsWndStyle(WBS_TOPMOST) || (pWnd2->IsWndStyle(WBS_POPUP) && !pWnd->IsWndStyle(WBS_POPUP)) || pWnd2->IsWndStyle(WBS_MODAL)) && pWnd2->IsVisible())
								break;
						}
						m_wndOrder.InsertAt(i, this);
						break;
					}
				}
			}
		}

		if (m_pWndFocus != this)
		{
			if (m_pWndFocus)
				KillFocus(m_pWndFocus, this);

			CWndBase* pWndOld = m_pWndFocus;

#if !defined (__AIO)
			if (m_pWndFocusChild)
				m_pWndFocusChild->OnSetFocus(m_pWndFocus->m_pWndFocusChild);
#endif
			m_pWndFocus = this;
			m_pCurFocus = this;
			OnSetFocus(pWndOld);
		}
		else
			m_pCurFocus = this;
	}
}
void CWndBase::KillFocus(CWndBase* pWndFocus, CWndBase* pWndNew)
{
	pWndFocus->OnKillFocus(pWndNew);
	if (pWndFocus->m_pWndFocusChild)
		KillFocus(pWndFocus->m_pWndFocusChild, m_pParentWnd->m_pWndFocusChild);
}
void CWndBase::RemoveDestroyWnd()
{
	for (int i = 0; i < m_wndRemove.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndRemove.GetAt(i);
		if (pWndBase)
		{
			pWndBase->OnDestroy();

			if (pWndBase->m_pParentWnd)
			{
				pWndBase->m_pParentWnd->RemoveWnd(pWndBase);
				pWndBase->m_pParentWnd->OnDestroyChildWnd(pWndBase);
			}

			pWndBase = (CWndBase*)m_wndRemove.GetAt(i);

			if (pWndBase)
				Error("CWndBase::RemoveDestroyWnd: %d not deleted", pWndBase->GetWndId());

			SAFE_DELETE(pWndBase);
		}
	}

	m_wndRemove.RemoveAll();
}
void CWndBase::DestroyAllWnd(CWndBase* pWndRoot)
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt(i);
		if (pWndBase && pWndBase->IsWndStyle(WBS_CHILD) == FALSE)
		{
			pWndBase->DestroyAllWnd(this);
			SAFE_DELETE(pWndBase);
			i--;
		}
	}
	m_wndArray.RemoveAll();
}
BOOL CWndBase::IsDestroy()
{
	for (int i = 0; i < m_wndRemove.GetSize(); i++)
		if (this == m_wndRemove[i])
			return TRUE;

	return FALSE;
}

void CWndBase::Destroy()
{
	for (int i = 0; i < m_wndRemove.GetSize(); i++)
		if (this == m_wndRemove[i])
			return;

	m_wndRemove.Add(this);
	EnableWindow(FALSE);
	SetVisible(FALSE);
}
CRect CWndBase::MakeCenterRect(int nWidth, int nHeight)
{
	CPoint pt = GetClientCenter();
	CRect rect(0, 0, nWidth, nHeight);
	pt.x -= nWidth / 2;
	pt.y -= nHeight / 2;
	rect.OffsetRect(pt);
	return rect;
}
CPoint CWndBase::GetClientCenter()
{
	return CPoint(m_rectWindow.Width() / 2, m_rectWindow.Height() / 2);
}
CPoint CWndBase::GetStrCenter(C2DRender* p2DRender, LPCTSTR str)
{
	CSize sizeStr(0, 0);
	p2DRender->m_pFont->GetTextExtent((TCHAR*)str, &sizeStr);
	return CPoint(m_rectWindow.Width() / 2 - sizeStr.cx / 2, m_rectWindow.Height() / 2 - sizeStr.cy / 2);
}
CPoint CWndBase::GetStrRectCenter(C2DRender* p2DRender, CRect rect, LPCTSTR str)
{
	CSize sizeStr(0, 0);// = p2DRender->GetTextExtent(str,str.GetLength());
	p2DRender->m_pFont->GetTextExtent((TCHAR*)str, &sizeStr);
	return CPoint(rect.Width() / 2 - sizeStr.cx / 2, rect.Height() / 2 - sizeStr.cy / 2);
}
void CWndBase::Move(CPoint pt)
{
	SetWndRect(CRect(pt.x, pt.y, pt.x + m_rectWindow.Width(), pt.y + m_rectWindow.Height()));
}

void CWndBase::MoveParentCenter()
{
	MoveRectCenter(m_pWndRoot->m_rectWindow);
}
void CWndBase::MoveRectCenter(CRect rect)
{
	int nWidth = m_rectWindow.Width();
	int nHeight = m_rectWindow.Height();
	int x = rect.left + (rect.Width() / 2) - (nWidth / 2);
	int y = rect.top + (rect.Height() / 2) - (nHeight / 2);
	SetWndRect(CRect(x, y, x + nWidth, y + nHeight));
}
void CWndBase::MovePointCenter(CPoint pt)
{
	int nWidth = m_rectWindow.Width();
	int nHeight = m_rectWindow.Height();
	pt.x -= (nWidth / 2);
	pt.y -= (nHeight / 2);
	SetWndRect(CRect(pt.x, pt.y, pt.x + nWidth, pt.y + nHeight));
}
void CWndBase::SetWndSize(int cx, int cy)
{
	m_rectWindow.right = m_rectWindow.left + cx;
	m_rectWindow.bottom = m_rectWindow.top + cy;
	SetWndRect(m_rectWindow);
}

void CWndBase::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectLayout = m_rectClient;
	if (IsWndRoot() == FALSE && !IsWndStyle(WBS_NOFRAME))
	{
		if (IsWndStyle(WBS_CAPTION) == TRUE)
			m_rectClient.top += 18;
		m_rectClient.DeflateRect(4, 4, 8, 10);
		m_rectLayout = m_rectClient;
		m_rectLayout.DeflateRect(8, 8);
	}
	if (bOnSize && (rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height()))
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
	else
		MakeVertexBuffer();
}

CRect CWndBase::GetClientRect(BOOL bParent)
{
	CRect rect = m_rectClient;
	if (bParent == FALSE)
		rect.OffsetRect(-m_rectClient.TopLeft());
	return rect;
}
CRect CWndBase::GetWindowRect(BOOL bParent)
{
	CRect rect = m_rectWindow;
	if (bParent == FALSE)
		rect.OffsetRect(-m_rectWindow.TopLeft());
	return rect;
}
CRect CWndBase::GetLayoutRect(BOOL bParent)
{
	CRect rect = m_rectLayout;
	if (bParent == FALSE)
		rect.OffsetRect(-m_rectClient.TopLeft());
	return rect;
}

CWndBase* CWndBase::FindFullWnd()
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWnd->m_nWinSize == 2)
			return pWnd;
	}
	return NULL;
}
void CWndBase::ClipStrArray(C2DRender* p2DRender, CRect rect, int nLineHeight, CStringArray* pStringArray, CStringArray* pNewStringArray)
{
	int nSize = pStringArray->GetSize();
	CString string;
	for (int i = 0; i < nSize; i++)
	{
		string = pStringArray->GetAt(i);
		int nWidth = rect.Width();// - 20;
		int nHeight = rect.Height();
		CSize sizeString;
		p2DRender->m_pFont->GetTextExtent(string, &sizeString);
		//sizeString.cy = 13;
		//sizeString.cx = string.GetLength() * 6;
		int nLength = string.GetLength();
		CString strTemp;
		int c = 0;
		char chr;
		BOOL bNextLine = FALSE;
		BOOL bHangul = FALSE;
		nLineHeight += sizeString.cy;
		//		sizeString.cy += nLineHeight;
		do
		{
			if (bNextLine)
			{
				while ((string[c] == ' ' || string[c] == '\n') && c < nLength)
					chr = string[c++];
			}
			strTemp = "";
			if (nLength)
			{
				do
				{
					chr = string[c++];

					//					if(chr == ' ');

					if (chr != '\r')
						strTemp += chr;

					if (chr & 0x80 && bHangul == FALSE)
						bHangul = TRUE;
					else
					{
						bHangul = FALSE;
						CSize sizeTemp;
						p2DRender->m_pFont->GetTextExtent(strTemp, &sizeTemp);
						if (sizeTemp.cx + 24 > nWidth)
							break;
						//if((strTemp.GetLength()*6) > nWidth)
							//break;
					}
				} while (c < nLength && chr != '\0' && chr != '\r');
			}
			bNextLine = TRUE;
			pNewStringArray->Add(strTemp);
			sizeString.cy += nLineHeight;
			if (sizeString.cy > nHeight)
				return;
		} while (chr != '\0' && c < nLength);
	}
}
void CWndBase::GetLogFont(C2DRender* p2DRender, LOGFONT* pLogFont)
{

}
int CWndBase::GetFontHeight()
{
	//CSize size;

	//return abs(size.cy);
	return m_pFont->GetMaxHeight();
}
int CWndBase::GetResizeDir(CPoint ptClient)
{
	// 1 = top
	// 2 = bottom
	// 3 = left
	// 4 = right
	// 5 = topLeft
	// 6 = topRight
	// 7 = bottomLeft
	// 8 = bottomRigh;

	CRect rect = GetWindowRect();
	if (rect.PtInRect(ptClient) == FALSE)
		return 0;
	rect.DeflateRect(6, 6);
	int nDir = 0;
	if (ptClient.x < rect.left && ptClient.y < rect.top)
		return 5;
	if (ptClient.x > rect.right && ptClient.y < rect.top)
		return 6;
	if (ptClient.x < rect.left && ptClient.y > rect.bottom)
		return 7;
	if (ptClient.x > rect.right && ptClient.y > rect.bottom)
		return 8;

	if (ptClient.y < rect.top)
		return 1;
	if (ptClient.y > rect.bottom)
		return 2;
	if (ptClient.x < rect.left)
		return 3;
	if (ptClient.x > rect.right)
		return 4;
	return nDir;
}

BOOL CWndBase::IsPickupSpace(CPoint ptWindow)
{
	CRect rect = GetWindowRect();
	if (IsWndStyle(WBS_CAPTION))
		rect.bottom = 20;
	if (rect.PtInRect(ptWindow))
	{

		return TRUE;
	}
	return FALSE;
}
BOOL CWndBase::IsOnWndBase(CPoint pt)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder[i];
		if (pWnd->IsVisible() && pWnd != m_pWndRoot)
		{
			CRect rect = pWnd->GetScreenRect();
			if (rect.PtInRect(pt))
				return TRUE;
		}
	}
	return FALSE;
}
CWndBase* CWndBase::GetWndBase(UINT idWnd)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
		if (idWnd == pWnd->GetWndId())
			return pWnd;
	}
	return NULL;
}
CWndBase* CWndBase::GetWndBase()
{
	for (int i = m_wndOrder.GetSize() - 1; i >= 0; i--)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWnd && pWnd->IsVisible() && !pWnd->IsWndStyle(WBS_MANAGER | WBS_TOPMOST))
		{
			if (pWnd->GetWndId() == APP_SKILL4)
			{
				CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase(APP_QUEITMWARNING);
				if (pWndBase)
					return NULL;

				if (((CWndSkill_16*)pWnd)->GetCurrSkillPoint() != g_pPlayer->m_nSkillPoint)
				{
					if (pWndBase == NULL)
					{
						SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
						g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
						g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
						g_WndMng.m_pWndReSkillWarning->InitItem(TRUE);
					}
					return NULL;
				}
			}
			if (pWnd->GetWndId() == APP_STATUS1 || pWnd->GetWndId() == APP_NAVIGATOR || pWnd->GetWndId() == APP_REVIVAL
				|| pWnd->GetWndId() == APP_BUFF_STATUS)
				continue;

			if (pWnd->GetWndId() == APP_VENDOR_REVISION)
				continue;

#ifdef __FL_UPDATE_TASKBAR
			if (pWnd->GetWndId() == APP_TASKBAR800 || pWnd->GetWndId() == APP_TASKBAR1024 || pWnd->GetWndId() == APP_TASKBAR1280)
				continue;
#endif

#ifdef __FL_DUNGEON_TREASURE
			if (pWnd->GetWndId() == APP_DUNGEON_TREASURE)
				continue;
#endif

#ifdef __S_SERVER_UNIFY
			if (g_WndMng.m_bAllAction == FALSE && (pWnd->GetWndId() == APP_GUILDNAME || pWnd->GetWndId() == APP_CHANGENAME))
				continue;
#endif

			return pWnd;
		}
	}
	return NULL;
}

CWndBase* CWndBase::GetChildWnd(UINT nID)
{
	for (int i = 0; i < m_wndArray.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndArray.GetAt(i);
		if (nID == pWnd->GetWndId())
			return pWnd;
	}
	return NULL;
}
CWndBase* CWndBase::GetFrameWnd()
{
	CWndBase* pWndCur = this;
	while (pWndCur)
	{
		if (pWndCur->IsWndStyle(WBS_CHILD) == FALSE)
			return pWndCur;
		pWndCur = pWndCur->GetParentWnd();
	}
	return pWndCur;
}
BOOL CWndBase::IsOpenModalWnd()
{
	if (m_pWndFocus)
		return m_pWndFocus->IsWndStyle(WBS_MODAL);
	return FALSE;
}
BOOL CWndBase::IsOpenWnd(UINT nId)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
		if (nId == pWnd->GetWndId())
			return TRUE;
	}
	return FALSE;
}
BOOL CWndBase::IsOpenWnd(CWndBase* pWndBase)
{
	for (int i = 0; i < m_wndOrder.GetSize(); i++)
	{
		CWndBase* pWnd = (CWndBase*)m_wndOrder.GetAt(i);
		if (pWndBase == pWnd)
			return TRUE;
	}
	return FALSE;
}
BOOL CWndBase::IsParentWnd(CWndBase* pParentWnd)
{
	CWndBase* pWndBase = this;
	while (pWndBase != m_pWndRoot && pWndBase != NULL)
	{
		pWndBase = pWndBase->m_pParentWnd;
		if (pWndBase == pParentWnd)
			return TRUE;
	}
	return FALSE;
}
/*
BOOL CWndBase::ScreenToClient(LPPOINT lpPoint)
{
	lpPoint->x -= m_rectWindow.left;
	lpPoint->y -= m_rectWindow.top;
	return TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////

void CWndBase::OnInitialUpdate()
{
	InitDeviceObjects();
	RestoreDeviceObjects();
}
void CWndBase::OnUpdate()
{

}
BOOL CWndBase::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{

	return TRUE;
}
BOOL CWndBase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return TRUE;
}

void CWndBase::OnLButtonUp(UINT nFlags, CPoint point)
{

}
void CWndBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndBase::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndBase::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndBase::OnMButtonUp(UINT nFlags, CPoint point)
{
}

void CWndBase::OnMButtonDown(UINT nFlags, CPoint point)
{
}
void CWndBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndBase::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndBase::OnMButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndBase::OnNonClientLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndBase::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndBase::OnChar(UINT nChar)
{
}

void CWndBase::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CWndBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CWndBase::OnSize(UINT nType, int cx, int cy)
{
	MakeVertexBuffer();
}

void CWndBase::OnPaint()
{
}
void CWndBase::OnClose()
{
	Destroy();
}
void CWndBase::OnDestroy()
{
#ifdef __CLIENT

	g_toolTip.CancelToolTip();
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	g_toolTipSub1.CancelToolTip();
	g_toolTipSub2.CancelToolTip();
#endif // __IMPROVE_SYSTEM_VER15
#endif
}
BOOL CWndBase::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	SetMouseCursor(CUR_BASE);
	return TRUE;
}
void CWndBase::OnTimer(UINT nIDEvent)
{
}
void CWndBase::OnMove(int x, int y)
{
}
void CWndBase::GradationRect(C2DRender* p2DRender, CRect rect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent)
{
	int nFirstHeight = rect.Height() * nMidPercent / 100;

	CRect rect1 = rect; rect1.bottom = rect1.top + nFirstHeight;
	CRect rect2 = rect; rect2.top = rect2.top + nFirstHeight;

	p2DRender->RenderFillRect(rect1, dwColor1t, dwColor1t, dwColor1b, dwColor1b);
	p2DRender->RenderFillRect(rect2, dwColor1b, dwColor1b, dwColor2b, dwColor2b);
}
BOOL CWndBase::OnEraseBkgnd(C2DRender* p2DRender)
{
	CRect rect = GetClientRect(FALSE);
	if (m_pTexture)
	{
	}
	else
	{
		m_pTheme->RenderWndBaseBkgr(p2DRender, &rect);
	}

	return TRUE;
}
void CWndBase::OnDestroyChildWnd(CWndBase* pWndChild)
{
}
void CWndBase::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndBase::OnKillFocus(CWndBase* pNewWnd)
{
	ReleaseCapture();
	m_bLButtonUp = FALSE;
	m_bLButtonDown = FALSE;
	m_bMButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	if (IsWndStyle(WBS_POPUP))
		SetVisible(FALSE);
}
BOOL CWndBase::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return TRUE;
}
BOOL CWndBase::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	SetForbid(TRUE);
	return FALSE;
}

void CWndBase::OnMouseWndSurface(CPoint point)
{
}

void CWndBase::SetCapture()
{
	//m_bCapture = TRUE;
	m_pWndCapture = this;
	::SetCapture(m_pApp->GetSafeHwnd());
}

void CWndBase::ReleaseCapture()
{
	//m_bCapture = FALSE;
	m_pWndCapture = NULL;
	::ReleaseCapture();
}
BOOL CWndBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return TRUE;
}
BOOL CWndBase::WindowToScreen(LPPOINT lpPoint)
{
	lpPoint->x += m_rectCurrentWindow.left;
	lpPoint->y += m_rectCurrentWindow.top;
	return TRUE;
}
BOOL CWndBase::WindowToScreen(LPRECT lpRect)
{
	lpRect->left += m_rectCurrentWindow.left;
	lpRect->top += m_rectCurrentWindow.top;
	lpRect->right += m_rectCurrentWindow.left;
	lpRect->bottom += m_rectCurrentWindow.top;
	return TRUE;
}

BOOL CWndBase::ClientToScreen(LPPOINT lpPoint)
{
	lpPoint->x += m_rectCurrentClient.left;
	lpPoint->y += m_rectCurrentClient.top;
	return TRUE;
}
BOOL CWndBase::ClientToScreen(LPRECT lpRect)
{

	lpRect->left += m_rectCurrentClient.left;
	lpRect->top += m_rectCurrentClient.top;
	lpRect->right += m_rectCurrentClient.left;
	lpRect->bottom += m_rectCurrentClient.top;
	return TRUE;
}
BOOL CWndBase::ScreenToWindow(LPPOINT lpPoint)
{
	lpPoint->x -= m_rectCurrentWindow.left;
	lpPoint->y -= m_rectCurrentWindow.top;

	return TRUE;
}
BOOL CWndBase::ScreenToClient(LPPOINT lpPoint)
{
	lpPoint->x -= m_rectCurrentClient.left;
	lpPoint->y -= m_rectCurrentClient.top;

	return TRUE;
}
BOOL CWndBase::ScreenToWindow(LPRECT lpRect)
{

	lpRect->left -= m_rectCurrentWindow.left;
	lpRect->top -= m_rectCurrentWindow.top;
	lpRect->right -= m_rectCurrentWindow.left;
	lpRect->bottom -= m_rectCurrentWindow.top;

	return TRUE;
}

BOOL CWndBase::ScreenToClient(LPRECT lpRect)
{

	lpRect->left -= m_rectCurrentClient.left;
	lpRect->top -= m_rectCurrentClient.top;
	lpRect->right -= m_rectCurrentClient.left;
	lpRect->bottom -= m_rectCurrentClient.top;

	return TRUE;
}

BOOL CWndBase::AdjustMinRect(CRect* pRect, int nWidth, int nHeight)
{
	if (pRect->Width() < nWidth || pRect->Height() < nHeight)
	{
		if (pRect->Width() < nWidth)
		{
			if (pRect->left > m_rectWindow.left)
				pRect->left = pRect->right - nWidth;
			else
				pRect->right = pRect->left + nWidth;
		}
		if (pRect->Height() < nHeight)
		{
			if (pRect->top > m_rectWindow.top)
				pRect->top = pRect->bottom - nHeight;
			else
				pRect->bottom = pRect->top + nHeight;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CWndBase::AdjustMaxRect(CRect* pRect, int nWidth, int nHeight)
{
	if (pRect->Width() > nWidth || pRect->Height() > nHeight)
	{
		if (pRect->Width() > nWidth)
		{
			if (pRect->left < m_rectWindow.left)
				pRect->left = pRect->right - nWidth;
			else
				pRect->right = pRect->left + nWidth;
		}
		if (pRect->Height() > nHeight)
		{
			if (pRect->top < m_rectWindow.top)
				pRect->top = pRect->bottom - nHeight;
			else
				pRect->bottom = pRect->top + nHeight;
		}
		return TRUE;
	}
	return FALSE;
}
void CWndBase::AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat)
{
	if (m_strTexture.IsEmpty() || IsVisible() == FALSE || IsWndStyle(WBS_NODRAWFRAME))
		return;

	if (m_bTile == FALSE)
	{
		LPIMAGE lpImage;

		if (m_strWndTileMap.Lookup(m_strTexture, (void*&)lpImage) == FALSE)
		{
			lpImage = new IMAGE;
			if (LoadImage(MakePath(DIR_THEME, m_strTexture), lpImage) == FALSE)
				Error("CWndBase::AdditionalSkinTexture에서 %s Open1 실패", m_strTexture);
			m_strWndTileMap.SetAt(m_strTexture, lpImage);
		}
		/////////////////////////////////////////////////
		CRect rect = GetWindowRect(TRUE);
		rect += (m_pParentWnd->GetClientRect(TRUE).TopLeft() - m_pParentWnd->GetWindowRect(TRUE).TopLeft());

		PaintTexture(pDest, lpImage, rect.TopLeft(), sizeSurface, d3dFormat);
		return;
	}

	CString strTile = m_strTexture;
	CString strFileName;
	LPIMAGE lpImage[9];

	int nImgBit = IMGBIT_32;
	for (int i = 0; i < 9; i++)
	{
		CString strTemp1 = strTile.Left(strTile.GetLength() - 6);
		CString strTemp2 = strTile.Right(4);
		strFileName.Format("%s%02d%s", strTemp1, i, strTemp2);
		if (m_strWndTileMap.Lookup(strFileName, (void*&)lpImage[i]) == FALSE)
		{
			lpImage[i] = new IMAGE;
			if (LoadImage(MakePath(DIR_THEME, strFileName), lpImage[i]) == FALSE)
				Error("CWndBase::AdditionalSkinTexture에서 %s Open2 실패", strFileName);
			m_strWndTileMap.SetAt(strFileName, lpImage[i]);
		}
	}
	/////////////////////////////////////////////////
	CRect rect;
	if (m_pParentWnd && IsWndStyle(WBS_CHILD))
	{
		rect = GetWindowRect(TRUE);
		rect += (m_pParentWnd->GetClientRect(TRUE).TopLeft() - m_pParentWnd->GetWindowRect(TRUE).TopLeft());
	}
	else
		rect = GetWindowRect();
#define EDIT_HEIGHT 8
	if (IsWndStyle(WBS_VSCROLL))
		rect.right -= 16;
	else if (IsWndStyle(WBS_HSCROLL))
		rect.bottom -= 16;

	int nWidth = rect.Width() / 8;
	int nHeight = rect.Height() / EDIT_HEIGHT;
	CPoint point;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			point = CPoint(j * 8, i * EDIT_HEIGHT) + rect.TopLeft();
			if (i == 0)
			{
				if (j == 0)
					PaintTexture(pDest, lpImage[0], point, sizeSurface, d3dFormat);
				else
					if (j == nWidth - 1)
						PaintTexture(pDest, lpImage[2], point, sizeSurface, d3dFormat);
					else
						PaintTexture(pDest, lpImage[1], point, sizeSurface, d3dFormat);
			}
			else
				if (i == nHeight - 1)
				{
					if (j == 0)
						PaintTexture(pDest, lpImage[6], point, sizeSurface, d3dFormat);
					else
						if (j == nWidth - 1)
							PaintTexture(pDest, lpImage[8], point, sizeSurface, d3dFormat);
						else
							PaintTexture(pDest, lpImage[7], point, sizeSurface, d3dFormat);
				}
				else
				{
					if (j == 0)
						PaintTexture(pDest, lpImage[3], point, sizeSurface, d3dFormat);
					else
						if (j == nWidth - 1)
							PaintTexture(pDest, lpImage[5], point, sizeSurface, d3dFormat);
						else
							PaintTexture(pDest, lpImage[4], point, sizeSurface, d3dFormat);
				}
		}
	}
}
void CWndBase::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat)
{
}


void CWndBase::AdjustWndBase(D3DFORMAT d3dFormat) //= D3DFMT_A4R4G4B4 )
{
	if (m_strTexture.IsEmpty() || IsVisible() == FALSE || IsWndStyle(WBS_NODRAWFRAME))
		return;


	CString strTextureId;
	strTextureId.Format("%p", this);
	m_textureMng.RemoveTexture(strTextureId);

	CRect rect = GetWindowRect(TRUE);
	CSize size2, size1 = CSize(rect.Width(), rect.Height());
	size2 = size1;

	AdjustSize(&size1);

	CTexture* pTexture = new CTexture;
	BOOL bOK = pTexture->CreateTexture(m_pApp->m_pd3dDevice, size1.cx, size1.cy, 1, 0, d3dFormat, D3DPOOL_MANAGED);
	ASSERT(pTexture);
	if (!bOK)
	{
		SAFE_DELETE(pTexture);
		return;
	}

	SetTexture(m_pApp->m_pd3dDevice, strTextureId, pTexture);

	CPoint point(0, 0);
	D3DLOCKED_RECT lockedRect;
	if (pTexture->m_pTexture->LockRect(0, &lockedRect, 0, 0) == D3D_OK)// Lock
	{
		LPWORD pDest = (LPWORD)lockedRect.pBits;
		AdditionalSkinTexture(pDest, size1, d3dFormat);
		for (int i = 0; i < m_wndArray.GetSize(); i++)
		{
			CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt(i);
			if (pWndBase->IsDestroy() == FALSE && pWndBase->IsVisible())
				pWndBase->AdditionalSkinTexture(pDest, size1, d3dFormat);
		}
		AfterSkinTexture(pDest, size1);
		SetWndRect(rect);
		pTexture->m_pTexture->UnlockRect(0);
	}
}
