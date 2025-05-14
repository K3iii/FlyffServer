

#include "stdafx.h"
#include "resData.h"
#include "WndPetRes.h"
#include "DPClient.h"
#include "Item.h"
#include "DefineText.h"


#ifdef __JEFF_11

extern CDPClient g_DPlay;



CWndPetRes::CWndPetRes()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
}
CWndPetRes::~CWndPetRes()
{
}
void CWndPetRes::OnDraw(C2DRender* p2DRender)
{
	ItemProp* pItemProp;

	if (m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
		if (pItemProp != NULL)
		{
			if (m_pTexture != NULL)
				m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
		}
	}
}
void CWndPetRes::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pDesc = (CWndText*)GetDlgItem(WIDC_DESC);
	pDesc->m_string.AddParsingString(prj.GetText(TID_GAME_PETTRADE));
	pDesc->ResetString();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndPetRes::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_RES, 0, CPoint(0, 0), pWndParent);
}


void CWndPetRes::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndPetRes::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetRes::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPetRes::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPetRes::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPetRes::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	if (nID == WIDC_OK)
	{
		if (m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_OK);
			pButton->EnableWindow(FALSE);


			if (m_pItemElem)
			{
				g_DPlay.SendQuePetResurrection(m_pItemElem->m_dwObjId);
				Destroy();
			}
		}
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetRes::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem) 
		return;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	CRect rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;
	}
}

BOOL CWndPetRes::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	CPet* pPet = pTempElem->m_pPet;
	if (!pPet)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000);
		return FALSE;
	}

	BYTE nLevel = pPet->GetLevel();
	if ((g_pPlayer->GetPetId() == pTempElem->m_dwObjId) || (!IsUsableItem(pTempElem))
		|| nLevel < PL_B || nLevel > PL_S)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000);
		return FALSE;
	}

	if (pTempElem->IsFlag(CItemElem::expired))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_PETTRADE_ERROR), NULL, 0xffff0000);
		return FALSE;
	}

	if (m_pItemElem)
		m_pItemElem->SetExtra(0);
	m_pItemElem = pTempElem;
	m_pEItemProp = m_pItemElem->GetProp();
	m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(TRUE);

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	if (m_pEItemProp != NULL)
		m_pTexture = m_pItemElem->m_pTexture;

	return TRUE;

}

#endif
