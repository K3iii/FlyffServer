// WndBase.cpp: implementation of the CWndBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndTradeCtrl
//////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_TRADE 25
CTextureMng CWndTradeCtrl::m_textureMng;

CWndTradeCtrl::CWndTradeCtrl()
{
	m_nWndColor = D3DCOLOR_TEMP(255, 0x5 << 3, 0x5 << 3, 0x5 << 3);
	m_nFontColor = D3DCOLOR_TEMP(255, 255, 255, 255);
	m_nSelectColor = D3DCOLOR_TEMP(255, 255, 255, 0);
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pMover = NULL;
	m_bDrag = FALSE;
}
CWndTradeCtrl::~CWndTradeCtrl()
{
}
void CWndTradeCtrl::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create(m_dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID);
}
void CWndTradeCtrl::InitItem(CMover* pMover)
{
	m_pMover = pMover;
	m_pFocusItem = NULL;
}
void CWndTradeCtrl::OnInitialUpdate()
{
}
void CWndTradeCtrl::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	CRect rect = GetClientRect();

	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	for (int i = 0; i < MAX_TRADE; i++)
	{
		int x = i % 5;
		int y = i / 5;

		CPoint point;
		point = CPoint(x * 32, y * 32);
		CRect crBoard;
		crBoard.left = point.x;
		crBoard.top = point.y;
		crBoard.right = point.x + 31;
		crBoard.bottom = crBoard.top + 31;

		if (crBoard.PtInRect(GetMousePoint()))
			p2DRender->RenderRect(crBoard, 0xff118701);
		else
			p2DRender->RenderRect(crBoard, D3DCOLOR_ARGB(175, 198, 179, 172));

		CRect crRect = crBoard;
		crRect.DeflateRect(1, 1, 1, 1);
		p2DRender->RenderFillRect(crRect, D3DCOLOR_ARGB(255, 255, 255, 255));

		CItemElem* pItemBase = m_pMover->m_vtInfo.GetItem(i);
		if (pItemBase)
		{
			if (pItemBase->IsFlag(CItemElem::expired))
				pItemBase->GetTexture()->Render2(p2DRender, CPoint(x * 32, y * 32), D3DCOLOR_ARGB(255, 255, 100, 100));
			else
				pItemBase->GetTexture()->Render(p2DRender, CPoint(x * 32, y * 32));

			CRect rectHittest = CRect(x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3);
			CPoint point = GetMousePoint();
			if (rectHittest.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&rectHittest);

				g_WndMng.PutToolTip_Item(pItemBase, point2, &rectHittest, APP_TRADE);
			}

			CItemElem* pItemElem = pItemBase;
			if (pItemElem->GetProp()->dwPackMax > 1)
			{
				short n = pItemElem->m_nItemNum;
				if (IsUsingItem(pItemBase))
					n = pItemBase->GetExtra();

				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", n);

				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
				CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
				p2DRender->TextOut(x * 32 + 32 - size.cx - 4, y * 32 + 32 - size.cy + 1, szTemp, 0xffeaebdd);
				p2DRender->SetFont(pOldFont);
			}
		}
	}
}
void CWndTradeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bDrag = FALSE;
}
void CWndTradeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrag == FALSE)
		return;

	m_bDrag = FALSE;
}

BOOL CWndTradeCtrl::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndTradeGold* pWndTradeGold = (CWndTradeGold*)g_WndMng.GetWndBase(APP_TRADE_GOLD);
	if (pWndTradeGold != NULL)
		return TRUE;

	int nDstId = -1;
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;
	CPoint pt(3, 3);

	if (pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData == 0)
	{
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
		return TRUE;
	}
	else
	{
		CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (IsUsableItem(pItemBase))
		{
			CItemElem* pItemElem = pItemBase;
			if (pItemElem->IsFlag(CItemElem::expired))
				return FALSE;

			if (pItemBase->m_nItemNum > 1)
			{
				for (int i = 0; i < MAX_TRADE; i++)
				{
					int x = i % 5;
					int y = i / 5;
					rect.SetRect(x * 32, y * 32, x * 32 + 32, y * 32 + 32);
					if (rect.PtInRect(point) && m_pMover->m_vtInfo.GetItem(i) == NULL)
					{
						pShortcut->m_dwData = i + 100;
						CWndBase* pParent = (CWndBase*)GetParentWnd();
						pParent->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
					}
				}
				return TRUE;
			}
		}
	}

	for (int i = 0; i < MAX_TRADE; i++)
	{
		int x = i % 5;
		int y = i / 5;
		rect.SetRect(x * 32, y * 32, x * 32 + 32, y * 32 + 32);
		if (rect.PtInRect(point) && m_pMover->m_vtInfo.GetItem(i) == NULL)
		{
			g_DPlay.SendTradePut(i, (BYTE)(pShortcut->m_dwId));
			return TRUE;
		}
	}
	return TRUE;
}

void CWndTradeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTradeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
#ifdef __CONTAINER_RCLICKS
	if (m_pMover)
	{
		for (int i = 0; i < MAX_TRADE; i++)
		{
			int x = i % 5;
			int y = i / 5;
			CItemElem* pItemBase = m_pMover->m_vtInfo.GetItem(i);
			if (pItemBase)
			{
				CRect rectHittest = CRect(x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3);
				CPoint point = GetMousePoint();
				if (rectHittest.PtInRect(point))
				{
					g_DPlay.SendTradePull(i);
					break;
				}
			}
		}
	}
#endif
}
void CWndTradeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTradeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_pMover)
	{
		for (int i = 0; i < MAX_TRADE; i++)
		{
			int x = i % 5;
			int y = i / 5;
			CItemElem* pItemBase = m_pMover->m_vtInfo.GetItem(i);
			if (pItemBase)
			{
				CRect rectHittest = CRect(x * 32, y * 32 + 3, x * 32 + 32, y * 32 + 32 + 3);
				CPoint point = GetMousePoint();
				if (rectHittest.PtInRect(point))
				{
					if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
					{
#ifdef __FL_PACKED_ITEMS
						if (pItemBase->IsPackage())
							g_WndMng.PreviewPackage(pItemBase);
#endif
#ifdef __MODEL_VIEW
						else
							g_WndMng.PreviewModel(pItemBase);
#endif
						return;
					}
					break;
				}
			}
		}
	}
}
void CWndTradeCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndTradeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;

	CWndBase::OnSize(nType, cx, cy);
}
void CWndTradeCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndTradeCtrl::PaintFrame(C2DRender* p2DRender)
{

}

BOOL CWndTradeCtrl::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
int CWndTradeCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
