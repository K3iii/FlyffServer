#include "stdafx.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndChangeFace.h"
#include "defineText.h"
#ifdef __CLIENT
#include "dpclient.h"
extern CDPClient	g_DPlay;
#endif




CWndChangeFace::CWndChangeFace()
{
	m_nSelectFace = 0;
}
CWndChangeFace::~CWndChangeFace()
{
}
void CWndChangeFace::OnDraw(C2DRender* p2DRender)
{
	if (m_nSelectFace >= 0 && m_nSelectFace < 5)
	{
		CRect rect(0, 0, 86, 170);
		rect.OffsetRect(10, 5);

		rect.OffsetRect(m_nSelectFace * 86, 0);

		p2DRender->RenderRect(rect, 0xff0000ff);

		rect.InflateRect(1, 1);
		p2DRender->RenderRect(rect, 0xff0000ff);
	}
}
void CWndChangeFace::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();




	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndChangeFace::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGEFACE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndChangeFace::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndChangeFace::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndChangeFace::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 10,25
	point.x -= 10;

	point.x /= 86;

	if (point.x >= 0 && point.x < 5 && point.y >= 0 && point.y < 180)
	{
		m_nSelectFace = point.x;
	}
}
void CWndChangeFace::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndChangeFace::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		CString strCommand;
		strCommand.Format("/ChangeFace %d", m_nSelectFace);
		g_DPlay.SendChat(strCommand);
		Destroy();
	}
	else
		if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		{
			nID = WTBID_CLOSE;
			Destroy();
			return TRUE;
		}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


#define MAX_FACE 5

CWndChangeSex::CWndChangeSex()
{
	m_nSelectFace = 0;
	m_pModel = NULL;
	m_dwItemId = NULL_ID;
}
CWndChangeSex::~CWndChangeSex()
{
	SAFE_DELETE(m_pModel);
}
void CWndChangeSex::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_STATIC1);
	VIEWPORT tmp;
	tmp.vecMatrixScaling = D3DXVECTOR3(4.5f, 4.5f, 4.5f);
	if (g_pPlayer->GetSex() == SEX_MALE)
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.6f, 0.0f);
	else
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.8f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pModel, g_pPlayer, lpFace, &tmp, GetWndId());
}
void CWndChangeSex::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SAFE_DELETE(m_pModel);

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_FEMALE : MI_MALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND2);

	UpdateModel();

	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndChangeSex::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGESEX, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndChangeSex::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndChangeSex::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndChangeSex::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndChangeSex::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		if (m_dwItemId == NULL_ID)
		{
			if (m_nSelectFace >= MAX_FACE || m_nSelectFace < 0)
				return FALSE;

			CString strCommand;
			strCommand.Format("/ChangeFace %d", m_nSelectFace);
			g_DPlay.SendChat(strCommand);
		}
		else
		{
			if (m_nSelectFace >= MAX_FACE || m_nSelectFace < 0)
				return FALSE;

			g_DPlay.SendUseTransy(m_dwItemId, m_nSelectFace);
		}
		Destroy();
	}
	else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
	{
		nID = WTBID_CLOSE;
		Destroy();
		return TRUE;
	}
	else if (nID == WIDC_LEFT)
	{
		m_nSelectFace--;

		if (m_nSelectFace < 0)
		{
			m_nSelectFace = MAX_FACE - 1;
		}

		if (m_nSelectFace >= MAX_FACE || m_nSelectFace < 0)
			return FALSE;

		UpdateModel();
	}
	else if (nID == WIDC_RIGHT)
	{
		m_nSelectFace++;

		if (m_nSelectFace >= MAX_FACE)
		{
			m_nSelectFace = 0;
		}

		if (m_nSelectFace >= MAX_FACE || m_nSelectFace < 0)
			return FALSE;

		UpdateModel();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndChangeSex::UpdateModel()
{
	if (m_pModel)
		CMover::UpdateParts(!g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, m_nSelectFace, g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory);
}


CWndItemTransy::CWndItemTransy()
{
}
CWndItemTransy::~CWndItemTransy()
{
}

#ifdef __SYS_ITEMTRANSY
void CWndItemTransy::Init(CItemElem* pItemElem, BOOL bMenu)
#else //__SYS_ITEMTRANSY
void CWndItemTransy::Init(CItemElem* pItemElem)
#endif //__SYS_ITEMTRANSY
{
	if (!IsUsableItem(pItemElem))
		return;

#ifdef __SYS_ITEMTRANSY
	m_bMenu = bMenu;
#endif // __SYS_ITEMTRANSY
	for (int i = 0; i < 2; ++i)
		m_pItemElem[i] = NULL;

	LPWNDCTRL pWndCtrl = NULL;
	pWndCtrl = GetWndCtrl(WIDC_STATIC1);
	m_Rect[0] = pWndCtrl->rect;

	pWndCtrl = GetWndCtrl(WIDC_STATIC4);
	m_Rect[1] = pWndCtrl->rect;

	if (pItemElem)
	{
		m_pItemElem[1] = pItemElem;
		m_pItemElem[1]->SetExtra(1);
	}
}
void CWndItemTransy::OnDestroy(void)
{
	if (m_pItemElem[0])
		m_pItemElem[0]->SetExtra(0);
	if (m_pItemElem[1])
		m_pItemElem[1]->SetExtra(0);
}
void CWndItemTransy::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemElem[0])
	{
		if (m_pItemElem[0]->GetTexture())
			m_pItemElem[0]->GetTexture()->Render(p2DRender, m_Rect[0].TopLeft(), 255);

		CTexture			Texture1;
		Texture1.LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pItemElemChange.GetProp()->szIcon), 0xffff00ff);
		Texture1.Render(p2DRender, m_Rect[1].TopLeft(), 255);
	}

	CPoint	Point = GetMousePoint();
	CRect HitRect = m_Rect[0];
	CPoint Point2 = Point;
	if (m_Rect[0].PtInRect(Point))
	{
		ClientToScreen(&Point2);
		ClientToScreen(&HitRect);
		if (m_pItemElem[0])
			g_WndMng.PutToolTip_Item(m_pItemElem[0], Point2, &HitRect);
		else
			g_toolTip.PutToolTip(100, prj.GetText(TID_GAME_TRANSITEM_PUT), *HitRect, Point2, 0);
	}

	HitRect = m_Rect[1];
	if (m_Rect[1].PtInRect(Point))
	{
		ClientToScreen(&Point2);
		ClientToScreen(&HitRect);
		if (m_pItemElem[0])
			g_WndMng.PutToolTip_Item(&m_pItemElemChange, Point2, &HitRect);
		else
			g_toolTip.PutToolTip(100, prj.GetText(TID_GAME_TRANSITEM_PUT1), *HitRect, Point2, 0);
	}
}
void CWndItemTransy::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();



	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndItemTransy::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_ITEM_TRANSY, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndItemTransy::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndItemTransy::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndItemTransy::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndItemTransy::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndItemTransy::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_pItemElem[0] && PtInRect(&m_Rect[0], point))
	{
		m_pItemElem[0]->SetExtra(0);
		m_pItemElem[0] = NULL;
	}
}
BOOL CWndItemTransy::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_OK:
	{
#ifdef __SYS_ITEMTRANSY
		if (m_bMenu)
		{
			if (m_pItemElem[0])
				g_DPlay.SendItemTransy(m_pItemElem[0]->m_dwObjId, NULL_ID, m_pItemElemChange.m_dwItemId, FALSE);
		}
		else
		{
			if (m_pItemElem[0] && m_pItemElem[1])
				g_DPlay.SendItemTransy(m_pItemElem[0]->m_dwObjId, m_pItemElem[1]->m_dwObjId, m_pItemElemChange.m_dwItemId, TRUE);
		}

		Destroy();
#else //__SYS_ITEMTRANSY
		if (m_pItemElem[0] && m_pItemElem[1])
		{
			g_DPlay.SendItemTransy(m_pItemElem[0]->m_dwObjId, m_pItemElem[1]->m_dwObjId);
			Destroy();
		}
#endif //__SYS_ITEMTRANSY
	}
	break;
	case WIDC_CANCEL:
	{
		Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndItemTransy::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!(pShortcut->m_dwShortcut == SHORTCUT_ITEM) && !(pWndFrame->GetWndId() == APP_INVENTORY))
		return FALSE;

	if (g_pPlayer->m_Inventory.IsEquip(pShortcut->m_dwId))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT));
		SetForbid(TRUE);
		return FALSE;
	}

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pItemElem))
	{
		SetForbid(TRUE);
		return FALSE;
	}

	if (PtInRect(&m_Rect[0], point))
	{
		ItemProp* pItemPropChange = NULL;

		ItemProp* pItemProp = pItemElem->GetProp();

		pItemPropChange = g_pPlayer->GetTransyItem(pItemProp);
		if (pItemPropChange == NULL)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_ITEM_TRANSY), NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY));
			SetForbid(TRUE);
			return FALSE;
		}

#ifdef __SYS_ITEMTRANSY
		if (!m_bMenu)
		{
#endif //__SYS_ITEMTRANSY
			if (m_pItemElem[1]->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A)
			{
				if (61 <= pItemElem->GetProp()->dwLimitLevel1)
				{
					CString strMsg;
					strMsg.Format(prj.GetText(TID_GAME_ITEM_TRANSY_NOT_LEVEL_0), m_pItemElem[1]->GetProp()->szName);
					g_WndMng.PutString(strMsg, NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY_NOT_LEVEL_0));
					SetForbid(TRUE);
					return FALSE;
				}
			}
			else
			{
				if (pItemElem->GetProp()->dwLimitLevel1 < 61)
				{
					CString strMsg;
					strMsg.Format(prj.GetText(TID_GAME_ITEM_TRANSY_NOT_LEVEL_1), m_pItemElem[1]->GetProp()->szName);
					g_WndMng.PutString(strMsg, NULL, prj.GetTextColor(TID_GAME_ITEM_TRANSY_NOT_LEVEL_1));
					SetForbid(TRUE);
					return FALSE;
				}
			}
#ifdef __SYS_ITEMTRANSY
		}
#endif //__SYS_ITEMTRANSY
		if (m_pItemElem[0])
		{
			m_pItemElem[0]->SetExtra(0);
		}

		pItemElem->SetExtra(1);
		m_pItemElem[0] = pItemElem;

		m_pItemElemChange = *m_pItemElem[0];
		m_pItemElemChange.m_dwItemId = pItemPropChange->dwID;
		m_pItemElemChange.m_nHitPoint = pItemPropChange->dwEndurance;
	}

	return TRUE;
}
