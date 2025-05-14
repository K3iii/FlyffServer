#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineObj.h"
#include "AppDefine.h"
#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndWarning::CWndWarning()
{
	m_pMover = NULL;
	m_pItemElem = NULL;
}
CWndWarning::~CWndWarning()
{

}

BOOL CWndWarning::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY, 0, pWndParent);
	MoveParentCenter();
	//	CWndStatic* pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC1 );
	//	pLabel->m_dwColor = 0xffff0000;
	//	pLabel  = (CWndStatic  *)GetDlgItem( WIDC_STATIC2 );
	//	pLabel->m_dwColor = 0xffff0000;
	return TRUE;
}

BOOL CWndWarning::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		SAFE_DELETE(((CWndShop*)m_pParentWnd)->m_pWndConfirmSell);
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell = new CWndConfirmSell;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pItemElem = m_pItemElem;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->m_pMover = m_pMover;
		((CWndShop*)m_pParentWnd)->m_pWndConfirmSell->Initialize(m_pParentWnd, APP_CONFIRM_SELL);
		Destroy();
	}
	if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmSell::CWndConfirmSell()
{
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_pStatic = NULL;
}
CWndConfirmSell::~CWndConfirmSell()
{
}
#if __VER >= 14 // __DROP_CONFIRM_BUG
BOOL CWndConfirmSell::Process(void)
{
	if (m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
#endif // __DROP_CONFIRM_BUG
void CWndConfirmSell::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemElem->IsEmpty())
	{
		m_pStaticGold->SetTitle(CString("0"));
		return;
	}
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi(szNumber);

	if (m_pItemElem->m_nItemNum == 1)
	{
		m_pEdit->SetString("1");
	}
	else
	{
		if (m_pItemElem->m_nItemNum < nNumber)
		{
			char szNumberbuf[16] = { 0, };
			nNumber = m_pItemElem->m_nItemNum;
			_itoa(m_pItemElem->m_nItemNum, szNumberbuf, 10);
			m_pEdit->SetString(szNumberbuf);
		}
		for (int i = 0; i < 8; i++)
		{
			char szNumberbuf[8] = { 0, };
			strncpy(szNumberbuf, szNumber, 8);


			if (47 >= szNumberbuf[i] || szNumberbuf[i] >= 58)
			{
				if (szNumberbuf[i] != 0)
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa(m_pItemElem->m_nItemNum, szNumberbuf, 10);
					m_pEdit->SetString(szNumberbuf);
					break;
				}
			}
		}
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
	if (m_pItemElem->IsOnePenyaPrice())
		BuyGold = 1;

#ifdef __SHOP_COST_RATE
	BuyGold = static_cast<DWORD>(static_cast<float>(BuyGold) * prj.m_fShopSellRate);
#endif // __SHOP_COST_RATE

	if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, m_pItemElem))
		BuyGold -= (static_cast<DWORD>(BuyGold * CTax::GetInstance()->GetSalesTaxRate(g_pPlayer)));
	CString szNumberGold;

	if (BuyGold < 1)
		BuyGold = 1;

	BuyGold *= nNumber;
	szNumberGold.Format("%u", BuyGold);
	m_pStaticGold->SetTitle(szNumberGold);
}
void CWndConfirmSell::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pOk = (CWndButton*)GetDlgItem(WIDC_OK);
	CWndBase* pEdit = (CWndButton*)GetDlgItem(WIDC_EDITSELL);
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	pStatic->AddWndStyle(WSS_MONEY);
	pOk->SetDefault(TRUE);
	pEdit->SetFocus();


}
BOOL CWndConfirmSell::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), APP_CONFIRM_SELL, WBS_KEY, 0, pWndParent);
	MoveParentCenter();

	CWndStatic* pLabel = (CWndStatic*)GetDlgItem(WIDC_STATIC);

	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDITSELL);
	m_pStatic = (CWndStatic*)GetDlgItem(WIDC_CONTROL1);
	m_pStaticGold = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

	if (m_pItemElem->m_nItemNum == 1)
		m_pEdit->SetString("1");
	else
	{
		TCHAR szNumber[64];
		_itot(m_pItemElem->m_nItemNum, szNumber, 10);
		m_pEdit->SetString(szNumber);
	}
	DWORD dwCost = m_pItemElem->GetCost();
	DWORD BuyGold = dwCost / 4;
	if (m_pItemElem->IsOnePenyaPrice())
		BuyGold = 1;

#ifdef __SHOP_COST_RATE
	BuyGold = static_cast<DWORD>(static_cast<float>(BuyGold) * prj.m_fShopSellRate);
#endif // __SHOP_COST_RATE
	BuyGold -= (static_cast<DWORD>(BuyGold * CTax::GetInstance()->GetSalesTaxRate(g_pPlayer)));

	if (BuyGold < 1)
		BuyGold = 1;

	BuyGold *= m_pItemElem->m_nItemNum;
	CString szNumberGold;
	szNumberGold.Format("%u", BuyGold);
	m_pStaticGold->SetTitle(szNumberGold);
	return TRUE;
}
BOOL CWndConfirmSell::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndConfirmSell::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndConfirmSell::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndConfirmSell::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndConfirmSell::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		PLAYSND(SND_INF_TRADE);

		int SellNum = 0;
		if (m_pItemElem->m_nItemNum >= 1)
			SellNum = atoi(m_pEdit->GetString());

		if (SellNum > 0)
			g_DPlay.SendSellItem(m_pItemElem->m_dwObjId, SellNum);
	}
	if (nID != WIDC_EDITSELL)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmBuy::CWndConfirmBuy()
{
	m_pMover = NULL;
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_pStatic = NULL;
#if __VER >= 11 // __CSC_VER11_3
	m_nBuyType = 0;
#endif //__CSC_VER11_3
}
CWndConfirmBuy::~CWndConfirmBuy()
{
}

void CWndConfirmBuy::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}

BOOL CWndConfirmBuy::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndConfirmBuy::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndConfirmBuy::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndConfirmBuy::OnLButtonDown(UINT nFlags, CPoint point)
{
}


void CWndConfirmBuy::OnChangeBuyCount(DWORD dwBuy)
{
	//	TCHAR szString[ 64 ];
	CString szString;

	if (dwBuy != atoi(m_pEdit->GetString()))
	{
		//		_itot( dwBuy, szString, 10 );	// integer to string
		szString.Format("%u", dwBuy);
		m_pEdit->SetString(szString);
	}

	DWORD dwTotalBuy = 0;
	DWORD dwCost = 0;
	if (m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
		dwCost = static_cast<int>(static_cast<float>(dwCost) * prj.m_fShopBuyRate);
#endif // __SHOP_COST_RATE

		if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, m_pItemElem))
			dwCost += (static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer)));
	}
#ifndef __NPC_SETVENDORTYPE
	else if (m_nBuyType == 1)
		dwCost = m_pItemElem->GetChipCost();
#else
	else
		dwCost = m_pItemElem->GetChipCost();
#endif

	dwTotalBuy = (DWORD)(dwBuy * dwCost * prj.m_fShopCost);
	if (m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
	{
		dwCost = PERIN_VALUE;
		dwTotalBuy = dwBuy * dwCost;
	}

	if (dwTotalBuy > INT_MAX)
	{
		dwBuy--;
		szString.Format("%u", dwBuy);
		m_pEdit->SetString(szString);
		dwTotalBuy = (DWORD)(dwBuy * dwCost * prj.m_fShopCost);
	}

	szString.Format("%u", dwTotalBuy);
	//	_itot( dwTotalBuy, szString, 10 );	// integer to string
	m_pStaticGold->SetTitle(szString);
}

BOOL CWndConfirmBuy::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), APP_CONFIRM_BUY_, 0, 0, pWndParent);
	MoveParentCenter();

	CWndStatic* pLabel = (CWndStatic*)GetDlgItem(WIDC_STATIC);
	//	pLabel->m_dwColor = 0xff000000;
	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_CONTROL2);
	m_pStatic = (CWndStatic*)GetDlgItem(WIDC_CONTROL1);
	m_pStaticGold = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	//	m_pStatic->m_dwColor = m_pStaticGold->m_dwColor = 0xff000000;
	m_pStaticGold->AddWndStyle(WSS_MONEY);

	DWORD dwCost;
	if (m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
		dwCost = static_cast<int>(static_cast<float>(dwCost) * prj.m_fShopBuyRate);
#endif // __SHOP_COST_RATE

		if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, m_pItemElem))
			dwCost += (static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer)));

	}
#ifndef __NPC_SETVENDORTYPE
	else if (m_nBuyType == 1)
		dwCost = m_pItemElem->GetChipCost();
#else
	else
		dwCost = m_pItemElem->GetChipCost();
#endif

#ifdef __NPC_SETVENDORTYPE
	if (dwCost <= 0 && m_pItemElem->GetProp()->dwReferValue1 <= 0)
#else
	if (dwCost == 0)
#endif
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0006)));
		Destroy();
		return TRUE;
	}

	OnChangeBuyCount(1);
	return TRUE;
}

void CWndConfirmBuy::OnDraw(C2DRender* p2DRender)
{
	DWORD dwCost = m_pItemElem->GetCost();
#ifdef __SHOP_COST_RATE
	dwCost = static_cast<int>(static_cast<float>(dwCost) * prj.m_fShopBuyRate);
#endif // __SHOP_COST_RATE

	if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, m_pItemElem))
		dwCost += (static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer)));

#ifdef __NPC_SETVENDORTYPE
	if (dwCost <= 0 && m_pItemElem->GetProp()->dwReferValue1 <= 0)
#else
	if (dwCost == 0)
#endif
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0006)));
		Destroy();
		return;
	}

	return;
}

const int MAX_BUY_ITEMCOUNT = 9999;

BOOL CWndConfirmBuy::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int nBuyNum = 0;

	switch (nID)
	{
	case WIDC_PLUS:
		nBuyNum = atoi(m_pEdit->GetString());
		++nBuyNum;

		if (m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
			m_pItemElem->GetProp()->dwID == II_CHP_RED
			)
		{
			if (nBuyNum > 9999)
				nBuyNum = 9999;
		}
		else
		{
			if (nBuyNum > MAX_BUY_ITEMCOUNT)
				nBuyNum = MAX_BUY_ITEMCOUNT;
		}

		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MINUS:
		nBuyNum = atoi(m_pEdit->GetString());
		if (--nBuyNum < 1)
			nBuyNum = 1;

		OnChangeBuyCount(nBuyNum);
		break;

	case WIDC_MAX:
	{
		if (m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
			m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
			m_pItemElem->GetProp()->dwID == II_CHP_RED
			)
			OnChangeBuyCount(9999);

		else if (m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
			OnChangeBuyCount(21);

		else
			OnChangeBuyCount(MAX_BUY_ITEMCOUNT);
	}
	break;

	case WIDC_CONTROL2:
		if (EN_CHANGE == message)
		{
			if (m_pEdit == NULL)
			{
				char szMsg[256];
				sprintf(szMsg, "CWndConfirmBuy::OnChildNotify : more info(%d, %d)", pLResult, GetDlgItem(WIDC_CONTROL2));
				nBuyNum = 1;
			}
			else
				nBuyNum = atoi(m_pEdit->GetString());

			nBuyNum = max(nBuyNum, 0);
			DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
			if (m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
				m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
				m_pItemElem->GetProp()->dwID == II_CHP_RED
				)
				dwMAXCount = 9999;
			else if (m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_PERIN)
				dwMAXCount = 21;
			nBuyNum = min(nBuyNum, (int)(dwMAXCount));

			OnChangeBuyCount(nBuyNum);
		}
		break;

	case WIDC_CANCEL:
		Destroy();
		break;

	case WIDC_OK:
		OnOK();
		Destroy();
		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndConfirmBuy::OnOK()
{
	DWORD dwCost;
	int nBuy;


#ifdef __NPC_SETVENDORTYPE
	if (m_nBuyType != 0)
#else
	if (m_nBuyType == 1)
#endif
	{
		dwCost = m_pItemElem->GetChipCost();
		if (m_pItemElem->m_nItemNum < 1 || dwCost == 0)
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0006)));
			return;
		}

		nBuy = atoi(m_pEdit->GetString());
#ifdef __NPC_SETVENDORTYPE
		CWndShop* pwndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
		LPCHARACTER lpCharacter = pwndShop->m_pMover->GetCharacter();
		if ((int)((nBuy * dwCost)) > g_pPlayer->m_Inventory.GetAtItemNum(lpCharacter->m_nVenderType))
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_CANNTBUY_REDCHIP)));
			return;
		}
#else
		if ((int)((nBuy * dwCost)) > g_pPlayer->m_Inventory.GetAtItemNum(II_CHP_RED))
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_CANNTBUY_REDCHIP)));
			return;
		}
#endif
	}
	else if (m_nBuyType == 0)
	{
		dwCost = m_pItemElem->GetCost();

		if (m_pItemElem->m_dwItemId != II_SYS_SYS_SCR_PERIN)
		{
			if (CTax::GetInstance()->IsApplyTaxRate(g_pPlayer, m_pItemElem))
				dwCost += (static_cast<DWORD>(dwCost * CTax::GetInstance()->GetPurchaseTaxRate(g_pPlayer)));
		}

		if (m_pItemElem->m_nItemNum < 1 || dwCost == 0)
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0006)));
			return;
		}

		nBuy = atoi(m_pEdit->GetString());
		if ((int)((nBuy * dwCost)) > g_pPlayer->GetGold())
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0009)));
			return;
		}
	}

	DWORD dwMAXCount = MAX_BUY_ITEMCOUNT;
	if (m_pItemElem->GetProp()->dwItemKind3 == IK3_BCHARM ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_RCHARM ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_ARROW ||
		m_pItemElem->GetProp()->dwID == II_CHP_RED
		)
		dwMAXCount = 9999;

	if (nBuy < 1 || nBuy >(int)(dwMAXCount))
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0086)));
		return;
	}
	if (nBuy > m_pItemElem->m_nItemNum)
	{
		g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0087)));
		return;
	}

	CWndShop* pWndShop = (CWndShop*)GetWndBase(APP_SHOP_);

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)pWndShop->GetDlgItem(WIDC_INVENTORY);
	CHAR cTab = (CHAR)pTabCtrl->GetCurSel();

	if (m_nBuyType == 0)
		g_DPlay.SendBuyItem(cTab, m_pItemElem->m_dwObjId, nBuy);
#ifndef __NPC_SETVENDORTYPE
	else if (m_nBuyType == 1)
		g_DPlay.SendBuyChipItem(cTab, m_pItemElem->m_dwObjId, nBuy);
#else
	else
		g_DPlay.SendBuyChipItem(cTab, m_pItemElem->m_dwObjId, nBuy);
#endif

	PLAYSND(SND_INF_TRADE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndItemCtrlVendor::CWndItemCtrlVendor()
{
	m_bVisibleCount = FALSE;
}
CWndItemCtrlVendor::~CWndItemCtrlVendor()
{
}
BOOL CWndItemCtrlVendor::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
		GetParentWnd()->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
	return TRUE;
}

CWndShop::CWndShop()
{
	m_pMover = NULL;
	m_pWndConfirmSell = NULL;
}
CWndShop::~CWndShop()
{
	SAFE_DELETE(m_pWndConfirmSell);
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
}
void CWndShop::OnDraw(C2DRender* p2DRender)
{
	LPCHARACTER lpCharacter = m_pMover->GetCharacter();
	if (lpCharacter == NULL)
		return;

#ifdef __NPC_SETVENDORTYPE
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	if (wndCtrl)
	{
		ItemProp* pItem = prj.GetItemProp(lpCharacter->m_nVenderType);

		if (pItem == NULL)
			pItem = prj.GetItemProp(II_GOLD_SEED1);

		if (!pItem)
			return;

		CTexture* pTexture = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pItem->szIcon), 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
	}
#endif

}

#ifdef __NPC_SETVENDORTYPE
void CWndShop::OnMouseWndSurface(CPoint point)
{
	LPWNDCTRL pCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CRect Draw;
	CPoint p2;
	if (pCtrl && pCtrl->rect.PtInRect(point))
	{
		LPCHARACTER lpCharacter = m_pMover->GetCharacter();
		if (lpCharacter)
		{
			CItemElem pItem;
			pItem.m_dwItemId = lpCharacter->m_nVenderType;

			if (pItem.m_dwItemId == NULL)
				pItem.m_dwItemId = II_GOLD_SEED1;

			p2 = point;
			Draw = pCtrl->rect;
			ClientToScreen(&p2);
			ClientToScreen(&Draw);
			g_WndMng.PutToolTip_Item(&pItem, p2, &Draw);
		}
	}
}
#endif

void CWndShop::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif
	if (g_WndMng.m_pWndTrade || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_INVENTORY);
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	LPCHARACTER lpCharacter = m_pMover->GetCharacter();

	if (lpCharacter)
	{
		int i = NULL;
		for (; i < MAX_VENDOR_INVENTORY_TAB; i++)
		{
			if (lpCharacter->m_venderSlot[i].IsEmpty() == FALSE)
			{
				m_wndItemCtrl[i].Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, i + 10);
				m_wndItemCtrl[i].InitItem(m_pMover->m_ShopInventory[i], APP_SHOP_);
				tabTabItem.pszText = lpCharacter->m_venderSlot[i].LockBuffer();
				lpCharacter->m_venderSlot[i].UnlockBuffer();
				tabTabItem.pWndBase = &m_wndItemCtrl[i];
				pTabCtrl->InsertItem(i, &tabTabItem);
			}
		}
		for (i = pTabCtrl->GetSize(); i < 3; i++)
		{
			tabTabItem.pszText = "";
			tabTabItem.pWndBase = NULL;
			pTabCtrl->InsertItem(i, &tabTabItem);
		}
	}

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	CRect rectInventory = pWndInventory->GetWindowRect(TRUE);
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	CRect rect = GetWindowRect(TRUE);

	if (ptInventory.x > rect.Width() / 2)
		ptMove = ptInventory - CPoint(rect.Width(), 0);
	else
		ptMove = ptInventory + CPoint(rectInventory.Width(), 0);

	Move(ptMove);

	CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pSearchBox)
		pSearchBox->SetStringBg("Search..");
}

BOOL CWndShop::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_SHOP_, 0, 0, pWndParent);
}

BOOL CWndShop::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndShop::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndShop::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndShop::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndShop::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	BOOL	bWarning = FALSE;

	if (message == WIN_ITEMDROP)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();

		BOOL bForbid = TRUE;
		if (lpShortcut->m_dwType == ITYPE_ITEM && lpShortcut->m_dwData != 0)
		{
			if (nID == 10 || nID == 11 || nID == 12 || nID == 13) // item
			{
				CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
				if (!itemElem || !itemElem->GetProp())
					return FALSE;

				BOOL bResult = TRUE;
				if (itemElem->IsQuest())
				{
					bResult = FALSE;
				}

				if (bResult && pWndFrame->GetWndId() == APP_INVENTORY)
				{
					if (FALSE == g_pPlayer->m_Inventory.IsEquip(itemElem->m_dwObjId))
					{
						SAFE_DELETE(m_pWndConfirmSell);
						m_pWndConfirmSell = new CWndConfirmSell;
						m_pWndConfirmSell->m_pItemElem = itemElem;
						m_pWndConfirmSell->m_pMover = m_pMover;
						m_pWndConfirmSell->Initialize(this, APP_CONFIRM_SELL);

						bForbid = FALSE;
					}
					else
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPTRADE), NULL, prj.GetTextColor(TID_GAME_EQUIPTRADE));
					}
				}
			}
		}
		SetForbid(bForbid);
	}

	switch (nID)
	{
	case WIDC_BUTTON_1:
	{
		CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		KillFocus(pSearchBox, NULL);
		pSearchBox->Empty();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndShop::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndConfirmSell)
		SAFE_DELETE(m_pWndConfirmSell);
}

void CWndShop::OnDestroy(void)
{
	g_pPlayer->m_vtInfo.SetOther(NULL);
	g_DPlay.SendCloseShopWnd();
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.GetWndBase(APP_INVENTORY);
	if (pWndInventory)
		SAFE_DELETE(pWndInventory->m_pWndConfirmBuy);

	SAFE_DELETE(m_pWndConfirmSell);
}


CWndBeautyShop::CWndBeautyShop()
{
	//	SetPutRegInfo( FALSE );
	m_pWndConfirmSell = NULL;
	m_pModel = NULL;
	m_dwHairMesh = 1;
	memset(m_ColorRect, 0, sizeof(CRect) * 3);
	memset(m_fColor, 0, sizeof(FLOAT) * 3);
	m_bLButtonClick = FALSE;
	m_nHairCost = 0;
	m_pApplyModel = NULL;
	m_nHairColorCost = 0;

#ifdef __Y_BEAUTY_SHOP_CHARGE
	m_bChange = FALSE;
#endif //__Y_BEAUTY_SHOP_CHARGE

	for (int i = 0; i < 4; i++)
	{
		m_nHairNum[i] = 0;
	}
	m_pHairModel = NULL;
	m_dwSelectHairMesh = 1;
	m_ChoiceBar = -1;
	m_pWndBeautyShopConfirm = NULL;

#ifdef __NEWYEARDAY_EVENT_COUPON
	m_bUseCoupon = FALSE;
	m_pWndUseCouponConfirm = NULL;
#endif //__NEWYEARDAY_EVENT_COUPON
}

CWndBeautyShop::~CWndBeautyShop()
{
	//	m_Texture.DeleteDeviceObjects();
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmSell);
#if __VER >= 8 //__CSC_VER8_4
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pHairModel);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
#endif //__CSC_VER8_4
#ifdef __NEWYEARDAY_EVENT_COUPON
	SAFE_DELETE(m_pWndUseCouponConfirm);
#endif //__NEWYEARDAY_EVENT_COUPON

	DeleteDeviceObjects();
}

#ifdef __NEWYEARDAY_EVENT_COUPON
void CWndBeautyShop::UseHairCoupon(BOOL isUse)
{
	m_bUseCoupon = isUse;
	if (m_bUseCoupon)
	{
		CString title = GetTitle();
		CString addText;
		addText.Format("  %s", prj.GetText(TID_GAME_NOWUSING_COUPON));
		title = title + addText;
		SetTitle(title);
	}
}
#endif //__NEWYEARDAY_EVENT_COUPON

HRESULT CWndBeautyShop::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndBeautyShop::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}
HRESULT CWndBeautyShop::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}

void CWndBeautyShop::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer || !m_pModel || !m_pApplyModel)
		return;

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM5);
	VIEWPORT tmp;
	tmp.vecMatrixScaling = D3DXVECTOR3(4.5f, 4.5f, 4.5f);
	if (g_pPlayer->GetSex() == SEX_MALE)
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.6f, 0.0f);
	else
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.2f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pModel, g_pPlayer, lpFace, &tmp, GetWndId());


	lpFace = GetWndCtrl(WIDC_CUSTOM6);
	tmp.m_fRot = static_cast<float>(g_tmCurrent / 50.0f);

	O3D_ELEMENT* pElem = m_pApplyModel->GetParts(PARTS_HAIR);
	if (pElem && pElem->m_pObject3D)
	{
		pElem->m_pObject3D->m_fAmbient[0] = m_fColor[0];
		pElem->m_pObject3D->m_fAmbient[1] = m_fColor[1];
		pElem->m_pObject3D->m_fAmbient[2] = m_fColor[2];
	}

	g_WndMng.RenderObjectViewport(p2DRender, m_pApplyModel, g_pPlayer, lpFace, &tmp, GetWndId());

	DrawHairKind(p2DRender);

	for (int i = 0; i < 3; i++)
	{
		CPoint pt = CPoint(m_ColorScrollBar[i].x - (m_Texture.m_size.cx / 2), m_ColorScrollBar[i].y);
		m_Texture.Render(p2DRender, pt);
	}

	for (int j = 0; j < 3; j++)
	{
		if (m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x)
			m_Texture.Render(p2DRender, CPoint(m_OriginalColorScrollBar[j].x - (m_Texture.m_size.cx / 2), m_OriginalColorScrollBar[j].y), 160);
	}

	BYTE nColorR = (BYTE)((m_fColor[0] * 255));
	BYTE nColorG = (BYTE)((m_fColor[1] * 255));
	BYTE nColorB = (BYTE)((m_fColor[2] * 255));

	BYTE nOrignalR = (BYTE)(g_pPlayer->m_fHairColorR * 255);
	BYTE nOrignalG = (BYTE)(g_pPlayer->m_fHairColorG * 255);
	BYTE nOrignalB = (BYTE)(g_pPlayer->m_fHairColorB * 255);

#ifdef __NEWYEARDAY_EVENT_COUPON
	if ((nColorR != nOrignalR || nColorG != nOrignalG || nColorB != nOrignalB) && !m_bUseCoupon)
#else 
	if (nColorR != nOrignalR || nColorG != nOrignalG || nColorB != nOrignalB)
#endif 
		m_nHairColorCost = HAIRCOLOR_COST;
	else
		m_nHairColorCost = 0;

	CString string;
	CWndStatic* pCost = (CWndStatic*)GetDlgItem(WIDC_COST);

	if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		string = "0";
	else
		string.Format(_T("%d"), m_nHairCost + m_nHairColorCost);

	pCost->SetTitle(string);
	pCost = (CWndStatic*)GetDlgItem(WIDC_STATIC1);

	if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		string = "0";
	else
		string.Format(_T("%d"), m_nHairCost);

	pCost->SetTitle(string);
	pCost = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

	if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		string = "0";
	else
		string.Format(_T("%d"), m_nHairColorCost);

	pCost->SetTitle(string);

}

void CWndBeautyShop::DrawHairKind(C2DRender* p2DRender)
{
	//Hair Kind
	int custom[4] = { WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4 };

	if (!m_pHairModel)
		return;

	DWORD HairNum = m_dwHairMesh;
	LPWNDCTRL lpHair = NULL;;

	for (int i = 0; i < 4; i++)
	{
		HairNum = HairNum > MAX_HAIR ? 1 : HairNum;
		m_nHairNum[i] = HairNum;
		CMover::UpdateParts(g_pPlayer->GetSex(), HairNum - 1, g_pPlayer->m_dwHeadMesh, NULL, m_pHairModel, &g_pPlayer->m_Inventory);

		lpHair = GetWndCtrl(custom[i]);
		VIEWPORT tmp;
		tmp.vecMatrixScaling = D3DXVECTOR3(6.0f, 6.0f, 6.0f);
		if (g_pPlayer->GetSex() == SEX_MALE)
			tmp.vecMatrixTranslation = D3DXVECTOR3(0.05f, -8.0f, -1.0f);
		else
			tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -7.5f, -1.0f);

		O3D_ELEMENT* pElem = m_pHairModel->GetParts(PARTS_HAIR);
		if (pElem && pElem->m_pObject3D)
		{
			pElem->m_pObject3D->m_fAmbient[0] = m_fColor[0];
			pElem->m_pObject3D->m_fAmbient[1] = m_fColor[1];
			pElem->m_pObject3D->m_fAmbient[2] = m_fColor[2];
		}

		g_WndMng.RenderObjectViewport(p2DRender, m_pHairModel, g_pPlayer, lpHair, &tmp, GetWndId());

		if (m_dwSelectHairMesh == m_nHairNum[i])
		{
			CRect rect;
			rect = lpHair->rect;
			p2DRender->RenderFillRect(rect, 0x60ffff00);
		}
		HairNum++;
	}
}

void CWndBeautyShop::UpdateModels()
{
	if (m_pModel != NULL)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pModel, &g_pPlayer->m_Inventory);
	if (m_pApplyModel != NULL)
		CMover::UpdateParts(g_pPlayer->GetSex(), m_dwSelectHairMesh - 1, g_pPlayer->m_dwHeadMesh, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);
}

void CWndBeautyShop::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	CRect rectInventory = pWndInventory->GetWindowRect(TRUE);
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	CRect rect = GetWindowRect(TRUE);

	if (ptInventory.x > rect.Width() / 2)
		ptMove = ptInventory - CPoint(rect.Width(), 0);
	else
		ptMove = ptInventory + CPoint(rectInventory.Width(), 0);

	Move(ptMove);

#if __VER >= 8 //__CSC_VER8_4
	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_R);
	rect = lpWndCtrl->rect;
	m_ColorRect[0].left = rect.left;
	m_ColorRect[0].top = rect.top + 25;
	m_ColorRect[0].right = rect.right;
	m_ColorRect[0].bottom = rect.bottom + 25;
	m_ColorRect[0] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_G);
	rect = lpWndCtrl->rect;
	m_ColorRect[1].left = rect.left;
	m_ColorRect[1].top = rect.top + 25;
	m_ColorRect[1].right = rect.right;
	m_ColorRect[1].bottom = rect.bottom + 25;
	m_ColorRect[1] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM_B);
	rect = lpWndCtrl->rect;
	m_ColorRect[2].left = rect.left;
	m_ColorRect[2].top = rect.top + 25;
	m_ColorRect[2].right = rect.right;
	m_ColorRect[2].bottom = rect.bottom + 25;
	m_ColorRect[2] = rect;

	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);

	m_OriginalColorScrollBar[0] = m_ColorScrollBar[0];
	m_OriginalColorScrollBar[1] = m_ColorScrollBar[1];
	m_OriginalColorScrollBar[2] = m_ColorScrollBar[2];

	m_pRGBEdit[0] = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pRGBEdit[1] = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pRGBEdit[2] = (CWndEdit*)GetDlgItem(WIDC_EDIT3);

	SetRGBToEdit(m_fColor[0], 0);
	SetRGBToEdit(m_fColor[1], 1);
	SetRGBToEdit(m_fColor[2], 2);

	CWndStatic* kindcost = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndStatic* colorcost = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CWndStatic* totalcost = (CWndStatic*)GetDlgItem(WIDC_COST);
	kindcost->AddWndStyle(WSS_MONEY);
	colorcost->AddWndStyle(WSS_MONEY);
	totalcost->AddWndStyle(WSS_MONEY);
#endif //__CSC_VER8_4
}

BOOL CWndBeautyShop::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	m_bLButtonClick = FALSE;
	m_dwHairMesh = g_pPlayer->m_dwHairMesh + 1;

	SAFE_DELETE(m_pModel);

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pModel, &g_pPlayer->m_Inventory);
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	SAFE_DELETE(m_pApplyModel);
	m_pApplyModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pApplyModel, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);
	m_pApplyModel->InitDeviceObjects(g_Neuz.GetDevice());

	m_fColor[0] = g_pPlayer->m_fHairColorR;
	m_fColor[1] = g_pPlayer->m_fHairColorG;
	m_fColor[2] = g_pPlayer->m_fHairColorB;

	m_dwSelectHairMesh = m_dwHairMesh;

	SAFE_DELETE(m_pHairModel);
	m_pHairModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pHairModel, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pHairModel, &g_pPlayer->m_Inventory);
	m_pHairModel->InitDeviceObjects(g_Neuz.GetDevice());

	m_nHairCost = 0;
	m_nHairColorCost = 0;

	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), 0xffff00ff, TRUE);

	return InitDialog(g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX, 0, 0, pWndParent);
}

#if __VER >= 8 //__CSC_VER8_4
void CWndBeautyShop::SetRGBToEdit(float color, int editnum)
{
	char szNumberbuf[8] = { 0, };
#ifdef __Y_HAIR_BUG_FIX
	float colorval = (color / 1.0f) * 255;
#else //__Y_HAIR_BUG_FIX
	float colorval = ((color - 0.3f) / (1.0f - 0.3f)) * 255 + 0.5f;
#endif //__Y_HAIR_BUG_FIX
	_itot((int)(colorval), szNumberbuf, 10);
	m_pRGBEdit[editnum]->SetString(szNumberbuf);
}

void CWndBeautyShop::SetRGBToBar(int editnum)
{
	float RGBNum;
	int ColorNum;
	CWndEdit* pWndEdit;

	if (editnum == WIDC_EDIT1)
	{
		ColorNum = 0;
		pWndEdit = m_pRGBEdit[0];
	}
	else if (editnum == WIDC_EDIT2)
	{
		ColorNum = 1;
		pWndEdit = m_pRGBEdit[1];
	}
	else if (editnum == WIDC_EDIT3)
	{
		ColorNum = 2;
		pWndEdit = m_pRGBEdit[2];
	}

	RGBNum = (float)(atoi(pWndEdit->GetString()));

	if (RGBNum < 0)
	{
		RGBNum = 0;
		pWndEdit->SetString("0");
	}
#ifdef __Y_HAIR_BUG_FIX
	else if (RGBNum > 255)
	{
		RGBNum = 255;
		pWndEdit->SetString("255");
	}
#else //__Y_HAIR_BUG_FIX
	else if (RGBNum > 254)
	{
		RGBNum = 254;
		pWndEdit->SetString("254");
	}
#endif //__Y_HAIR_BUG_FIX

#ifdef __Y_HAIR_BUG_FIX
	m_fColor[ColorNum] = (RGBNum / 255) * 1.0f;
#else //__Y_HAIR_BUG_FIX
	m_fColor[ColorNum] = (RGBNum / 255) * (1.0f - 0.3f) + 0.3f;
#endif //__Y_HAIR_BUG_FIX
	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);
}
#endif //__CSC_VER8_4

void CWndBeautyShop::ReSetBar(FLOAT r, FLOAT g, FLOAT b)
{
#ifdef __Y_HAIR_BUG_FIX
	FLOAT fR = (r / 1.0f) * 100.0f;
	FLOAT fG = (g / 1.0f) * 100.0f;
	FLOAT fB = (b / 1.0f) * 100.0f;
#else //__Y_HAIR_BUG_FIX
	FLOAT fR = ((r - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fG = ((g - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fB = ((b - 0.3f) / (1.0f - 0.3f)) * 100.0f;
#endif //__Y_HAIR_BUG_FIX

	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top;
}

void CWndBeautyShop::OnMouseWndSurface(CPoint point)
{
	if (g_pPlayer == NULL)
		return;
	CRect rect = CRect(44, 198, 186, 398);
	if (!rect.PtInRect(point))
		m_bLButtonClick = FALSE;

	if (m_ChoiceBar != -1 && m_bLButtonClick)
	{
		CRect DrawRect = m_ColorRect[m_ChoiceBar];
		point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;

		LONG Width = DrawRect.right - DrawRect.left;
		LONG Pos = point.x - DrawRect.left;

		FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

#ifdef __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
#else //__Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.3f, 0.998f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(0.998f, 0.998f);
#endif //__Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec3;

		D3DXVec2Lerp(&vec3, &vec1, &vec2, p);

		m_fColor[m_ChoiceBar] = vec3.x;

		m_ColorScrollBar[m_ChoiceBar].x = point.x;

#ifdef __Y_BEAUTY_SHOP_CHARGE
		if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
			m_bChange = TRUE;
#endif //__Y_BEAUTY_SHOP_CHARGE
		SetRGBToEdit(m_fColor[m_ChoiceBar], m_ChoiceBar);
	}
}

BOOL CWndBeautyShop::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndBeautyShop::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndBeautyShop::OnLButtonUp(UINT nFlags, CPoint point)
{
#if __VER >= 8 //__CSC_VER8_4
	m_ChoiceBar = -1;
#endif //__CSC_VER8_4
	m_bLButtonClick = FALSE;
}

void CWndBeautyShop::OnLButtonDown(UINT nFlags, CPoint point)
{
	int i;
	int custom[4] = { WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4 };
	LPWNDCTRL lpWndCtrl;

	for (i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];
		if (DrawRect.PtInRect(point))
			m_ChoiceBar = i;
	}
	for (i = 0; i < 4; i++)
	{
		lpWndCtrl = GetWndCtrl(custom[i]);
		CRect DrawRect = lpWndCtrl->rect;
		if (DrawRect.PtInRect(point))
		{

			m_dwSelectHairMesh = m_nHairNum[i];
			CMover::UpdateParts(g_pPlayer->GetSex(), m_dwSelectHairMesh - 1, g_pPlayer->m_dwHeadMesh, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);

#ifdef __NEWYEARDAY_EVENT_COUPON
			if (g_pPlayer->m_dwHairMesh != m_dwSelectHairMesh - 1 && !m_bUseCoupon)
#else //__NEWYEARDAY_EVENT_COUPON
			if (g_pPlayer->m_dwHairMesh != m_dwSelectHairMesh - 1)
#endif //__NEWYEARDAY_EVENT_COUPON
			{
				m_nHairCost = HAIR_COST;
#ifdef __Y_BEAUTY_SHOP_CHARGE
				if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
					m_bChange = TRUE;
#endif //__Y_BEAUTY_SHOP_CHARGE
			}
			else
				m_nHairCost = 0;
		}
	}
	m_bLButtonClick = TRUE;
}

BOOL CWndBeautyShop::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (g_pPlayer == NULL)
		return FALSE;

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1:
		{
			m_dwHairMesh = g_pPlayer->m_dwHairMesh + 1;
			CMover::UpdateParts(g_pPlayer->GetSex(), m_dwHairMesh - 1, g_pPlayer->m_dwHeadMesh, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);

			m_fColor[0] = g_pPlayer->m_fHairColorR;
			m_fColor[1] = g_pPlayer->m_fHairColorG;
			m_fColor[2] = g_pPlayer->m_fHairColorB;

			m_nHairCost = 0;

			ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);
#if __VER >= 8 //__CSC_VER8_4
			m_nHairColorCost = 0;
			m_dwSelectHairMesh = m_dwHairMesh;
			SetRGBToEdit(m_fColor[0], 0);
			SetRGBToEdit(m_fColor[1], 1);
			SetRGBToEdit(m_fColor[2], 2);
#endif //__CSC_VER8_4
#ifdef __Y_BEAUTY_SHOP_CHARGE
			if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
				m_bChange = FALSE;
#endif //__Y_BEAUTY_SHOP_CHARGE
		}
		break;
		case WIDC_HAIRSTYLE_LEFT: // hair
		{
			m_dwHairMesh--;
			(m_dwHairMesh < 1) ? m_dwHairMesh = MAX_HAIR : m_dwHairMesh;
		}
		break;
		case WIDC_HAIRSTYLE_RIGHT: // hair
		{
			m_dwHairMesh++;
			(m_dwHairMesh > MAX_HAIR) ? m_dwHairMesh = 1 : m_dwHairMesh;
		}
		break;
		case WIDC_OK:
		{
#ifdef __NEWYEARDAY_EVENT_COUPON
			BOOL noChange = FALSE;
			BYTE nColorR = (BYTE)((m_fColor[0] * 255));
			BYTE nColorG = (BYTE)((m_fColor[1] * 255));
			BYTE nColorB = (BYTE)((m_fColor[2] * 255));

			BYTE nOrignalR = (BYTE)(g_pPlayer->m_fHairColorR * 255);
			BYTE nOrignalG = (BYTE)(g_pPlayer->m_fHairColorG * 255);
			BYTE nOrignalB = (BYTE)(g_pPlayer->m_fHairColorB * 255);

			if ((g_pPlayer->m_dwHairMesh == m_dwSelectHairMesh - 1) && (nColorR == nOrignalR && nColorG == nOrignalG && nColorB == nOrignalB))
				noChange = TRUE;
#endif //__NEWYEARDAY_EVENT_COUPON
#ifdef __Y_BEAUTY_SHOP_CHARGE
			if (::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
			{
				if (m_bChange && g_pPlayer)
				{
#if __VER >= 8 //__CSC_VER8_4
					if (m_pWndBeautyShopConfirm == NULL)
					{
						m_pWndBeautyShopConfirm = new CWndBeautyShopConfirm;
						m_pWndBeautyShopConfirm->Initialize(this);
					}
#else
					g_DPlay.SendSetHair(m_dwHairMesh - 1, m_fColor[0], m_fColor[1], m_fColor[2]);	//, nCost );
#endif //__CSC_VER8_4
				}
			}
			else
#endif //__Y_BEAUTY_SHOP_CHARGE
			{
				if (g_pPlayer)
				{
#if __VER >= 8 //__CSC_VER8_4
					int nCost = m_nHairCost + m_nHairColorCost;
#else
					int nCost = m_nHairCost + m_nHairColorCostR + m_nHairColorCostG + m_nHairColorCostB;
#endif //__CSC_VER8_4

					if (nCost < 0)
						nCost = 0;
#if __VER >= 8 //__CSC_VER8_4
#ifdef __NEWYEARDAY_EVENT_COUPON
					if (m_bUseCoupon && !noChange)
					{
						if (m_pWndUseCouponConfirm == NULL)
						{
							m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
							m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_EX, 1);
							m_pWndUseCouponConfirm->Initialize(this);
						}
					}
#endif //__NEWYEARDAY_EVENT_COUPON
					if (nCost > 0)
					{
						if (m_pWndBeautyShopConfirm == NULL)
						{
							m_pWndBeautyShopConfirm = new CWndBeautyShopConfirm;
							m_pWndBeautyShopConfirm->Initialize(this);
						}
					}
#else
					g_DPlay.SendSetHair(m_dwHairMesh - 1, m_fColor[0], m_fColor[1], m_fColor[2]);	//, nCost );
#endif //__CSC_VER8_4
				}
			}
#if __VER >= 8 //__CSC_VER8_4
			int nCost = m_nHairCost + m_nHairColorCost;
#ifdef __NEWYEARDAY_EVENT_COUPON
			if (nCost <= 0 && (!m_bUseCoupon || noChange))
#else //__NEWYEARDAY_EVENT_COUPON
			if (nCost <= 0)
#endif //__NEWYEARDAY_EVENT_COUPON
				Destroy();
#else
			Destroy();
#endif //__CSC_VER8_4
		}
		break;
		case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
		}
	}
	if (nID == WIDC_EDIT1 || nID == WIDC_EDIT2 || nID == WIDC_EDIT3)
		SetRGBToBar(nID);

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndBeautyShop::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndBeautyShopConfirm)
		SAFE_DELETE(m_pWndBeautyShopConfirm);
	if (pWndChild == m_pWndUseCouponConfirm)
		SAFE_DELETE(m_pWndUseCouponConfirm);
}

void CWndBeautyShop::OnDestroy(void)
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmSell);
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pHairModel);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
	SAFE_DELETE(m_pWndUseCouponConfirm);
}

#if __VER >= 8 //__CSC_VER8_4
#ifdef __NEWYEARDAY_EVENT_COUPON
CWndUseCouponConfirm::CWndUseCouponConfirm()
{
	m_bUseCoupon = FALSE;
	m_checkClose = TRUE;
	m_TargetWndId = -1;
}

CWndUseCouponConfirm::~CWndUseCouponConfirm()
{
}

void CWndUseCouponConfirm::OnDestroy()
{
	if (m_bUseCoupon == FALSE)
		return;

	if (!m_checkClose && m_MainFlag == 0)
	{
		if (m_TargetWndId != -1)
		{
			if (m_TargetWndId == APP_BEAUTY_SHOP_EX)
			{
				g_WndMng.CreateApplet(APP_INVENTORY);
				SAFE_DELETE(g_WndMng.m_pWndBeautyShop);
				g_WndMng.m_pWndBeautyShop = new CWndBeautyShop;
				g_WndMng.m_pWndBeautyShop->Initialize(NULL, APP_BEAUTY_SHOP_EX);
				g_WndMng.m_pWndBeautyShop->UseHairCoupon(m_bUseCoupon);
			}
			else if (m_TargetWndId == APP_BEAUTY_SHOP_SKIN)
			{
				g_WndMng.CreateApplet(APP_INVENTORY);
				SAFE_DELETE(g_WndMng.m_pWndFaceShop);
				g_WndMng.m_pWndFaceShop = new CWndFaceShop;
				g_WndMng.m_pWndFaceShop->Initialize(NULL, APP_BEAUTY_SHOP_EX);
				g_WndMng.m_pWndFaceShop->UseFaceCoupon(m_bUseCoupon);
			}
		}
	}
	if (this->GetParentWnd() == g_WndMng.GetWndBase(APP_WORLD))
		g_WndMng.m_pWndUseCouponConfirm = NULL;
	else if (this->GetParentWnd() == g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX))
		g_WndMng.m_pWndBeautyShop->m_pWndUseCouponConfirm = NULL;
	else if (this->GetParentWnd() == g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN))
		g_WndMng.m_pWndFaceShop->m_pWndUseCouponConfirm = NULL;
}

void CWndUseCouponConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndUseCouponConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	SetTitle(prj.GetText(TID_GAME_USE_CHANGE_COUPON));

	if (m_TargetWndId != -1)
	{
		if (m_MainFlag == 0)
			pText->AddString(prj.GetText(TID_GAME_ASKUSE_COUPON1));
		else if (m_MainFlag == 1)
		{
			pText->AddString(prj.GetText(TID_GAME_ASKUSE_COUPON2));
			pText->AddString("\n");
			pText->AddString(prj.GetText(TID_GAME_WARNNING_USE_COUPON), 0xffff0000);
		}
	}

	MoveParentCenter();
}

BOOL CWndUseCouponConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX_CONFIRM, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndUseCouponConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndUseCouponConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndUseCouponConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndUseCouponConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndUseCouponConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (m_MainFlag == 0)
			m_bUseCoupon = TRUE;
		else if (m_MainFlag == 1)
		{
			if (m_TargetWndId == APP_BEAUTY_SHOP_EX)
			{
				CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)this->GetParentWnd();
				g_DPlay.SendSetHair((BYTE)(pWndBeautyShop->m_dwSelectHairMesh - 1), pWndBeautyShop->m_fColor[0], pWndBeautyShop->m_fColor[1], pWndBeautyShop->m_fColor[2]);
			}
			else if (m_TargetWndId == APP_BEAUTY_SHOP_SKIN)
			{
				CWndFaceShop* pWndFaceShop = (CWndFaceShop*)this->GetParentWnd();
				g_DPlay.SendChangeFace(pWndFaceShop->m_nSelectedFace - 1);
			}
		}
		m_checkClose = FALSE;
	}
	else if (nID == WIDC_NO)
		m_checkClose = FALSE;
	else if (nID == WTBID_CLOSE)
		m_checkClose = TRUE;

	Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndUseCouponConfirm::SetInfo(DWORD targetWndId, int flag)
{
	m_TargetWndId = targetWndId;
	m_MainFlag = flag;
}
#endif //__NEWYEARDAY_EVENT_COUPON
/*************************
CWndBeautyShopConfirm Class
*************************/
CWndBeautyShopConfirm::CWndBeautyShopConfirm()
{
	m_ParentId = 0;
}

CWndBeautyShopConfirm::~CWndBeautyShopConfirm()
{
}

void CWndBeautyShopConfirm::OnDestroy()
{
}

void CWndBeautyShopConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndBeautyShopConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	int TotalCost;
	CString strText, strNum;

	CWndBase* pWndBase = this->GetParentWnd();
	m_ParentId = pWndBase->GetWndApplet()->dwWndId;
	if (m_ParentId == APP_BEAUTY_SHOP_EX)
	{
		CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)pWndBase;
		TotalCost = pWndBeautyShop->m_nHairCost + pWndBeautyShop->m_nHairColorCost;
		strNum.Format("%d", TotalCost);
		strText.Format("%s %s %s %s %s %s", prj.GetText(TID_GAME_SHOP_CHOICE), prj.GetText(TID_GAME_CHANGE_HAIR), prj.GetText(TID_GAME_SHOP_COST),
			GetNumberFormatEx(strNum), prj.GetText(TID_GAME_SHOP_PENYA), prj.GetText(TID_GAME_CHANGE_HAIR_QUESTION));
	}
	else if (m_ParentId == APP_BEAUTY_SHOP_SKIN)
	{
		CWndFaceShop* pWndFaceShop = (CWndFaceShop*)pWndBase;
		TotalCost = pWndFaceShop->m_nCost;
		strNum.Format("%d", TotalCost);
		strText.Format("%s %s %s %s %s %s", prj.GetText(TID_GAME_SHOP_CHOICE), prj.GetText(TID_GAME_CHANGE_FACE), prj.GetText(TID_GAME_SHOP_COST),
			GetNumberFormatEx(strNum), prj.GetText(TID_GAME_SHOP_PENYA), prj.GetText(TID_GAME_CHANGE_FACE_QUESTION));
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->SetString(strText);

	MoveParentCenter();
}

BOOL CWndBeautyShopConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_EX_CONFIRM, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndBeautyShopConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndBeautyShopConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndBeautyShopConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndBeautyShopConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndBeautyShopConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (m_ParentId == APP_BEAUTY_SHOP_EX)
		{
			CWndBeautyShop* pWndBeautyShop = (CWndBeautyShop*)this->GetParentWnd();
			if (pWndBeautyShop)
				g_DPlay.SendSetHair((BYTE)(pWndBeautyShop->m_dwSelectHairMesh - 1), pWndBeautyShop->m_fColor[0], pWndBeautyShop->m_fColor[1], pWndBeautyShop->m_fColor[2]);
		}
		else if (m_ParentId == APP_BEAUTY_SHOP_SKIN)
		{
			CWndFaceShop* pWndFaceShop = (CWndFaceShop*)this->GetParentWnd();
			if (pWndFaceShop)
				g_DPlay.SendChangeFace(pWndFaceShop->m_nSelectedFace - 1);
		}
		Destroy();
	}
	else if (nID == WIDC_NO || nID == WTBID_CLOSE)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

/*************************
	CWndFaceShop Class
*************************/
CWndFaceShop::CWndFaceShop()
{
	m_pMainModel = NULL;
	m_pApplyModel = NULL;
	m_pFriendshipFace = NULL;
	m_pNewFace = NULL;
	m_pWndBeautyShopConfirm = NULL;
	m_nSelectedFace = 1;
	m_dwFriendshipFace = 1;
	m_dwNewFace = 6;
	m_nCost = 0;
	m_ChoiceBar = -1;
#ifdef __NEWYEARDAY_EVENT_COUPON
	m_bUseCoupon = FALSE;
	m_pWndUseCouponConfirm = NULL;
#endif //__NEWYEARDAY_EVENT_COUPON
}

CWndFaceShop::~CWndFaceShop()
{
	SAFE_DELETE(m_pMainModel);
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pFriendshipFace);
	SAFE_DELETE(m_pNewFace);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
#ifdef __NEWYEARDAY_EVENT_COUPON
	SAFE_DELETE(m_pWndUseCouponConfirm);
#endif //__NEWYEARDAY_EVENT_COUPON
}

void CWndFaceShop::OnDestroy()
{
	SAFE_DELETE(m_pMainModel);
	SAFE_DELETE(m_pApplyModel);
	SAFE_DELETE(m_pFriendshipFace);
	SAFE_DELETE(m_pNewFace);
	SAFE_DELETE(m_pWndBeautyShopConfirm);
#ifdef __NEWYEARDAY_EVENT_COUPON
	SAFE_DELETE(m_pWndUseCouponConfirm);
#endif
}

#ifdef __NEWYEARDAY_EVENT_COUPON
void CWndFaceShop::UseFaceCoupon(BOOL isUse)
{
	m_bUseCoupon = isUse;
	if (m_bUseCoupon)
	{
		CString title = GetTitle();
		CString addText;
		addText.Format("  %s", prj.GetText(TID_GAME_NOWUSING_COUPON));
		title = title + addText;
		SetTitle(title);
	}
}
#endif //__NEWYEARDAY_EVENT_COUPON

void CWndFaceShop::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer || !m_pMainModel || !m_pApplyModel)
		return;

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM5);
	VIEWPORT tmp;
	tmp.vecMatrixScaling = D3DXVECTOR3(4.5f, 4.5f, 4.5f);
	if (g_pPlayer->GetSex() == SEX_MALE)
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.6f, 0.0f);
	else
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.2f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pMainModel, g_pPlayer, lpFace, &tmp, GetWndId());


	lpFace = GetWndCtrl(WIDC_CUSTOM6);
	tmp.vecMatrixScaling = D3DXVECTOR3(4.5f, 4.5f, 4.5f);
	if (g_pPlayer->GetSex() == SEX_MALE)
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.6f, 0.0f);
	else
		tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -5.2f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pApplyModel, g_pPlayer, lpFace, &tmp, GetWndId());


	DrawFaces(0, p2DRender);
	DrawFaces(1, p2DRender);
}

void CWndFaceShop::DrawFaces(int ChoiceFlag, C2DRender* p2DRender)
{
	DWORD FaceNum = 0;
	CModelObject* m_pFaceModel = NULL;

	if (ChoiceFlag == 0)
	{
		m_pFaceModel = m_pFriendshipFace;
		FaceNum = m_dwFriendshipFace;
	}
	else if (ChoiceFlag == 1)
	{
		m_pFaceModel = m_pNewFace;
		FaceNum = m_dwNewFace;
	}

	if (!m_pFaceModel)
		return;

	int custom_friend[4] = { WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4 };
	int custom_new[4] = { WIDC_CUSTOM7, WIDC_CUSTOM8, WIDC_CUSTOM9, WIDC_CUSTOM10 };

	LPWNDCTRL lpFace = NULL;
	for (int i = 0; i < 4; i++)
	{
		if (ChoiceFlag == 0)
		{
			(FaceNum > MAX_DEFAULT_HEAD) ? FaceNum = 1 : FaceNum;
			lpFace = GetWndCtrl(custom_friend[i]);
			m_nFriendshipFaceNum[i] = FaceNum;
		}
		else if (ChoiceFlag == 1)
		{
			(FaceNum > MAX_HEAD) ? FaceNum = 6 : FaceNum;
			lpFace = GetWndCtrl(custom_new[i]);
			m_nNewFaceNum[i] = FaceNum;
		}

		if (!lpFace)
			continue;

		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, FaceNum - 1, NULL, m_pFaceModel, &g_pPlayer->m_Inventory);

		VIEWPORT tmp;
		tmp.vecMatrixScaling = D3DXVECTOR3(7.5f, 7.5f, 7.5f);
		if (g_pPlayer->GetSex() == SEX_MALE)
			tmp.vecMatrixTranslation = D3DXVECTOR3(0.05f, -10.0f, -1.0f);
		else
			tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -9.5f, -1.0f);

		g_WndMng.RenderObjectViewport(p2DRender, m_pFaceModel, g_pPlayer, lpFace, &tmp, GetWndId());

		//Select Draw
		if (ChoiceFlag == 0)
		{
			if (m_nSelectedFace == m_nFriendshipFaceNum[i])
			{
				CRect rect;
				rect = lpFace->rect;
				p2DRender->RenderFillRect(rect, 0x60ffff00);
			}
		}
		else if (ChoiceFlag == 1)
		{
			if (m_nSelectedFace == m_nNewFaceNum[i])
			{
				CRect rect;
				rect = lpFace->rect;
				p2DRender->RenderFillRect(rect, 0x60ffff00);
			}
		}
		FaceNum++;
	}
}

void CWndFaceShop::UpdateModels()
{
	if (m_pMainModel != NULL)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pMainModel, &g_pPlayer->m_Inventory);
	if (m_pApplyModel != NULL)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, m_nSelectedFace - 1, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);
}

void CWndFaceShop::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	CRect rectInventory = pWndInventory->GetWindowRect(TRUE);
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	CRect rect = GetWindowRect(TRUE);

	if (ptInventory.x > rect.Width() / 2)
		ptMove = ptInventory - CPoint(rect.Width(), 0);
	else
		ptMove = ptInventory + CPoint(rectInventory.Width(), 0);

	Move(ptMove);
}

BOOL CWndFaceShop::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	if (g_pPlayer->m_dwHeadMesh >= 0 && g_pPlayer->m_dwHeadMesh < 6)
		m_dwFriendshipFace = g_pPlayer->m_dwHeadMesh + 1;
	else if (g_pPlayer->m_dwHeadMesh >= 6 && g_pPlayer->m_dwHeadMesh < 15)
		m_dwNewFace = g_pPlayer->m_dwHeadMesh + 1;

	m_nSelectedFace = g_pPlayer->m_dwHeadMesh + 1;

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);

	SAFE_DELETE(m_pMainModel);
	m_pMainModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pMainModel, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pMainModel, &g_pPlayer->m_Inventory);
	m_pMainModel->InitDeviceObjects(g_Neuz.GetDevice());

	SAFE_DELETE(m_pApplyModel);
	m_pApplyModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pApplyModel, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);
	m_pApplyModel->InitDeviceObjects(g_Neuz.GetDevice());

	SAFE_DELETE(m_pFriendshipFace);
	m_pFriendshipFace = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pFriendshipFace, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pFriendshipFace, &g_pPlayer->m_Inventory);
	m_pFriendshipFace->InitDeviceObjects(g_Neuz.GetDevice());

	SAFE_DELETE(m_pNewFace);
	m_pNewFace = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pNewFace, OT_MOVER, nMover, MTI_STAND2);
	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, NULL, m_pNewFace, &g_pPlayer->m_Inventory);
	m_pNewFace->InitDeviceObjects(g_Neuz.GetDevice());

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BEAUTY_SHOP_SKIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndFaceShop::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndFaceShop::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndFaceShop::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndFaceShop::OnLButtonDown(UINT nFlags, CPoint point)
{
	int i;
	int custom[8] = { WIDC_CUSTOM1, WIDC_CUSTOM2, WIDC_CUSTOM3, WIDC_CUSTOM4,
					WIDC_CUSTOM7, WIDC_CUSTOM8, WIDC_CUSTOM9, WIDC_CUSTOM10 };
	LPWNDCTRL lpWndCtrl;

	for (i = 0; i < 8; i++)
	{
		lpWndCtrl = GetWndCtrl(custom[i]);
		CRect DrawRect = lpWndCtrl->rect;
		if (DrawRect.PtInRect(point))
		{

			if (i >= 0 && i < 4)
				m_nSelectedFace = m_nFriendshipFaceNum[i];
			else if (i >= 4 && i < 8)
				m_nSelectedFace = m_nNewFaceNum[i - 4];

			CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, m_nSelectedFace - 1, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);

#ifdef __NEWYEARDAY_EVENT_COUPON
			if (g_pPlayer->m_dwHeadMesh != m_nSelectedFace - 1 && !m_bUseCoupon)
#else //__NEWYEARDAY_EVENT_COUPON
			if (g_pPlayer->m_dwHeadMesh != m_nSelectedFace - 1)
#endif //__NEWYEARDAY_EVENT_COUPON
				m_nCost = CHANGE_FACE_COST;
			else
				m_nCost = 0;
		}
	}
}

BOOL CWndFaceShop::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (g_pPlayer == NULL)
		return FALSE;

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BTN_RESET:
		{
			if (g_pPlayer->m_dwHeadMesh >= 0 && g_pPlayer->m_dwHeadMesh < 6)
			{
				m_dwFriendshipFace = g_pPlayer->m_dwHeadMesh + 1;
				m_dwNewFace = 6;
			}
			else if (g_pPlayer->m_dwHeadMesh >= 6 && g_pPlayer->m_dwHeadMesh < 16)
			{
				m_dwNewFace = g_pPlayer->m_dwHeadMesh + 1;
				m_dwFriendshipFace = 1;
			}
			m_nSelectedFace = g_pPlayer->m_dwHeadMesh + 1;
			CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, m_nSelectedFace - 1, NULL, m_pApplyModel, &g_pPlayer->m_Inventory);

			m_nCost = 0;
		}
		break;
		case WIDC_FRIENDSHIPFACE_LEFT:
		{
			m_dwFriendshipFace--;
			(m_dwFriendshipFace < 1) ? m_dwFriendshipFace = MAX_DEFAULT_HEAD : m_dwFriendshipFace;
		}
		break;
		case WIDC_FRIENDSHIPFACE_RIGHT:
		{
			m_dwFriendshipFace++;
			(m_dwFriendshipFace > MAX_DEFAULT_HEAD) ? m_dwFriendshipFace = 1 : m_dwFriendshipFace;
		}
		break;
		case WIDC_NEWFACE_LEFT:
		{
			m_dwNewFace--;
			(m_dwNewFace < 6) ? m_dwNewFace = MAX_HEAD : m_dwNewFace;
		}
		break;
		case WIDC_NEWFACE_RIGHT:
		{
			m_dwNewFace++;
			(m_dwNewFace > MAX_HEAD) ? m_dwNewFace = 6 : m_dwNewFace;
		}
		break;
		case WIDC_BTN_OK:
		{
#ifdef __NEWYEARDAY_EVENT_COUPON
			BOOL noChange = FALSE;
			if (g_pPlayer->m_dwHeadMesh == m_nSelectedFace - 1)
				noChange = TRUE;

			if (m_nCost <= 0 && (!m_bUseCoupon || noChange))
#else //__NEWYEARDAY_EVENT_COUPON
			if (m_nCost <= 0)
#endif //__NEWYEARDAY_EVENT_COUPON
				Destroy();
			else if (g_pPlayer)
			{
				if (m_nCost < 0)
					m_nCost = 0;
#ifdef __NEWYEARDAY_EVENT_COUPON
				if (m_bUseCoupon && !noChange)
				{
					if (m_pWndUseCouponConfirm == NULL)
					{
						m_pWndUseCouponConfirm = new CWndUseCouponConfirm;
						m_pWndUseCouponConfirm->SetInfo(APP_BEAUTY_SHOP_SKIN, 1);
						m_pWndUseCouponConfirm->Initialize(this);
					}
				}
#endif //__NEWYEARDAY_EVENT_COUPON
				if (m_nCost > 0)
				{
					if (m_pWndBeautyShopConfirm == NULL)
					{
						m_pWndBeautyShopConfirm = new CWndBeautyShopConfirm;
						m_pWndBeautyShopConfirm->Initialize(this);
					}
				}
			}
		}
		break;
		case WIDC_BTN_CANCEL:
		{
			Destroy();
		}
		break;
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndFaceShop::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndUseCouponConfirm)
		SAFE_DELETE(m_pWndUseCouponConfirm);

	if (pWndChild == m_pWndBeautyShopConfirm)
		SAFE_DELETE(m_pWndBeautyShopConfirm);
}

#endif //__CSC_VER8_4
