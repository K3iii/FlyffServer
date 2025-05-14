#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndBank.h"

#include "WndManager.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


CWndBank::CWndBank()
{
}
CWndBank::~CWndBank()
{
	g_DPlay.SendCloseBankWnd();
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
}

void CWndBank::OnDraw(C2DRender* p2DRender)
{
	CString str;
	for (int i = 0; i < 3; ++i)
	{
		str.Format(_T("%d"), g_pPlayer->m_dwGoldBank[i]);
		pCost[i]->SetTitle(str);
	}
}

void CWndBank::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif
	if (g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
	{
		Destroy();
		return;
	}

	if (!g_pPlayer)
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_BANK_0);

	m_wndItemCtrl[0].Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, 11);
	m_wndItemCtrl[0].InitItem(&g_pPlayer->m_Bank[0], APP_COMMON_BANK);

	CWndTabCtrl* pTabCtrl1 = (CWndTabCtrl*)GetDlgItem(WIDC_BANK_1);

	m_wndItemCtrl[1].Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl1, 11);
	m_wndItemCtrl[1].InitItem(&g_pPlayer->m_Bank[1], APP_COMMON_BANK);

	CWndTabCtrl* pTabCtrl2 = (CWndTabCtrl*)GetDlgItem(WIDC_BANK_3);

	m_wndItemCtrl[2].Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl2, 11);
	m_wndItemCtrl[2].InitItem(&g_pPlayer->m_Bank[2], APP_COMMON_BANK);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_ITEM);
	tabTabItem.pWndBase = &m_wndItemCtrl[0];
	pTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pWndBase = &m_wndItemCtrl[1];
	pTabCtrl1->InsertItem(0, &tabTabItem);


	tabTabItem.pWndBase = &m_wndItemCtrl[2];
	pTabCtrl2->InsertItem(0, &tabTabItem);


	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_BANKGO_0);
	m_wndGold[0].Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_0);
	m_wndGold[0].AddWndStyle(WBS_NODRAWFRAME);

	lpWndCtrl = GetWndCtrl(WIDC_BANKGO_1);
	m_wndGold[1].Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_1);
	m_wndGold[1].AddWndStyle(WBS_NODRAWFRAME);

	lpWndCtrl = GetWndCtrl(WIDC_BANKGO_2);
	m_wndGold[2].Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_BANKGO_2);
	m_wndGold[2].AddWndStyle(WBS_NODRAWFRAME);

	pCost[0] = (CWndStatic*)GetDlgItem(WIDC_BANKGOLD_0);
	pCost[1] = (CWndStatic*)GetDlgItem(WIDC_BANKGOLD_1);
	pCost[2] = (CWndStatic*)GetDlgItem(WIDC_BANKGOLD_2);

	for (int i = 0; i < 3; i++)
		pCost[i]->AddWndStyle(WSS_MONEY);

	ReSetBank();

	RectItemCtrl[0] = pTabCtrl->m_rectClient + CRect(-3, 2, -18, -20);
	RectItemCtrl[1] = pTabCtrl1->m_rectClient + CRect(-3, 2, -19, -20);
	RectItemCtrl[2] = pTabCtrl2->m_rectClient + CRect(-3, 2, -20, -20);

	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	CRect rectInventory = pWndInventory->GetWindowRect(TRUE);
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	if (ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2)
		ptMove = ptInventory - CPoint((int)(rectInventory.Width() * 2.525), 0);
	else
		ptMove = ptInventory + CPoint(rectInventory.Width(), 0);
	Move(ptMove);

	m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndCommonBankNotUse.tga"), 0xffff00ff);

	CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pSearchBox)
		pSearchBox->SetStringBg("Search..");
}

void CWndBank::ReSetBank()
{
	for (int i = 0; i < 3; ++i)
	{
		if (g_pPlayer->m_nSlot == i)
		{
			bUse[i] = TRUE;
		}
		else
			if (g_pPlayer->m_idPlayerBank[i] == 0 || !g_pPlayer->IsCommBank())
			{
				m_wndItemCtrl[i].SetScrollBarVisible(FALSE);
				m_wndItemCtrl[i].EnableWindow(FALSE);
				m_wndGold[i].EnableWindow(FALSE);
				bUse[i] = FALSE;
			}
			else
			{
				m_wndItemCtrl[i].SetScrollBarVisible(TRUE);
				m_wndItemCtrl[i].EnableWindow(TRUE);
				m_wndGold[i].EnableWindow(TRUE);
				bUse[i] = TRUE;
			}
		RectGold[i] = pCost[i]->m_rectClient;
	}
}


BOOL CWndBank::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COMMON_BANK, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndBank::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndBank::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndBank::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndBank::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndBank::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	if (message == WIN_ITEMDROP)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		CWndBase* pWndTarget = pWndFrame->m_pWndOnMouseMove;
		CWndBase* pWndPut = pWndFrame->m_pCurFocus;
		BYTE nSlot;
		if (pWndTarget == &(m_wndItemCtrl[0]))
		{
			nSlot = 0;
		}
		else if (pWndTarget == &(m_wndItemCtrl[1]))
		{
			nSlot = 1;
		}
		else
		{
			nSlot = 2;
		}

		BOOL bForbid = TRUE;
		if (nID == 11) // item
		{
			if (pWndFrame->GetWndId() == APP_INVENTORY)
			{
				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if (SelectCount != 1)
					{

						g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT));

					}
					else
					{
						for (int i = 0; i < (int)(SelectCount); i++)
						{
							int nItem = pWndItemCtrl->GetSelectedItem(i);
							pWndItemCtrl->GetItem(nItem);
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if (!itemElem || !itemElem->GetProp())
							return FALSE;

						if (itemElem->m_nItemNum > 1)
						{
							SAFE_DELETE(g_WndMng.m_pWndTradeGold);
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;

							g_WndMng.m_pWndTradeGold->Initialize(NULL, APP_TRADE_GOLD);
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
							CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
							CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendPutItemBank(nSlot, (BYTE)(lpShortcut->m_dwId), 1);
						}

					}
				}
				else
				{
					SAFE_DELETE(g_WndMng.m_pWndTradeGold);
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;

					g_WndMng.m_pWndTradeGold->Initialize(NULL, APP_TRADE_GOLD);
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
					CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);

					//CString strCount = " SEED : ";
					CString strMain = prj.GetText(TID_GAME_MOVEPENYA);
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);// " pennya : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
			}
			else if (pWndFrame->GetWndId() == APP_COMMON_BANK)
			{
				BYTE nPutSolt;

				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					if (pWndPut == &(m_wndItemCtrl[0]))
					{
						nPutSolt = 0;
					}
					else if (pWndPut == &(m_wndItemCtrl[1]))
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
					}

					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if (SelectCount != 1)
					{

						g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT));

					}
					else
					{
						for (int i = 0; i < (int)(SelectCount); i++)
						{
							int nItem = pWndItemCtrl->GetSelectedItem(i);
							pWndItemCtrl->GetItem(nItem);
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if (!itemElem || !itemElem->GetProp())
							return FALSE;

						if (itemElem->m_nItemNum > 1)
						{
							SAFE_DELETE(g_WndMng.m_pWndTradeGold);
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;

							g_WndMng.m_pWndTradeGold->Initialize(NULL, APP_TRADE_GOLD);
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
							CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
							CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendPutItemBankToBank(nPutSolt, nSlot, (BYTE)(lpShortcut->m_dwId), 1);
						}

					}
				}
				else
				{

					if (pWndPut == &(m_wndItemCtrl[0]))
					{
						nPutSolt = 0;
					}
					else if (pWndPut == &(m_wndItemCtrl[1]))
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
					}


					if (pWndPut == &(m_wndGold[0]))
					{
						nPutSolt = 0;
					}
					else if (pWndPut == &(m_wndGold[1]))
					{
						nPutSolt = 1;
					}
					else
					{
						nPutSolt = 2;
					}

					SAFE_DELETE(g_WndMng.m_pWndTradeGold);
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nPutSolt];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;

					g_WndMng.m_pWndTradeGold->Initialize(NULL, APP_TRADE_GOLD);
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
					CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);

					//CString strCount = " SEED : ";
					CString strMain = prj.GetText(TID_GAME_MOVEPENYA);
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);// " pennya : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
			}
		}
	}
	else if (WIN_DBLCLK == message)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (pItem)
		{
			CWndBase* pWndPut = m_pCurFocus;
			if (pWndPut && pWndPut->GetParentWnd())
			{
				int nType = g_WndMng.GetContainerTypeByWindowId(GetWndId(), pWndPut->GetParentWnd()->GetWndId());
				g_WndMng.UseItem(pItem, nType);
			}
		}
	}
#ifdef __CONTAINER_RCLICKS
	else if (message == WNM_ITEMCLICK)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (IsUsableItem(pItem))
		{
			if (g_WndMng.GetWndBase(APP_INVENTORY))
			{
				if (m_pCurFocus)
				{
					int nPutBank = 0;
					if (m_pCurFocus == &(m_wndItemCtrl[0]))
						nPutBank = 0;
					else if (m_pCurFocus == &(m_wndItemCtrl[1]))
						nPutBank = 1;
					else
						nPutBank = 2;

					g_DPlay.SendGetItemBank(nPutBank, (BYTE)pItem->m_dwObjId, pItem->m_nItemNum);

				}
			}
		}
	}
#endif
	else if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON_1)
		{
			CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
			KillFocus(pSearchBox, NULL);
			pSearchBox->Empty();
		}

	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
CWndConfirmBank::CWndConfirmBank()
{
	m_dwId = NULL_ID;
	m_dwItemId = 0;
}
CWndConfirmBank::~CWndConfirmBank()
{
}
void CWndConfirmBank::OnDraw(C2DRender* p2DRender)
{
}
void CWndConfirmBank::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndEdit* m_pPass = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (m_pPass)
	{
		m_pPass->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);
		m_pPass->SetMaxStringNumber(CWndCreateChar::MAX_2ND_PASSWORD_NUMBER);
		m_pPass->SetFocus();
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndConfirmBank::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CONFIRM_BANK, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndConfirmBank::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndConfirmBank::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndConfirmBank::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndConfirmBank::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndConfirmBank::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK || message == EN_RETURN)
	{
		CWndEdit* m_pPass = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		if (m_pPass)
		{

			char szPass[10] = { 0, };
			strncpy(szPass, m_pPass->GetString(), 10);
			if (strlen(szPass) == 4)
			{
				BOOL bDigit = TRUE;
				for (int i = 0; i < 4; i++)
				{
					if (isdigit2(szPass[i]) == FALSE)
					{
						bDigit = FALSE;
						break;
					}
				}
				if (bDigit)
				{
					g_DPlay.SendConfirmBank(szPass, m_dwId, m_dwItemId);
				}
				else
				{
					m_pPass->SetString("");
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));

				}
			}
			else
			{
				m_pPass->SetString("");
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));

			}
		}
	}
	else if (nID == WIDC_CHANGE_PASS)
	{
		SAFE_DELETE(g_WndMng.m_pWndBankPassword);
		g_WndMng.m_pWndBankPassword = new CWndBankPassword;
		g_WndMng.m_pWndBankPassword->SetBankPassword(1);
		g_WndMng.m_pWndBankPassword->Initialize(NULL, APP_BANK_PASSWORD);

		Destroy();
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
CWndBankPassword::CWndBankPassword()
{
	m_nFlags = 0;
	m_dwId = NULL_ID;
	m_dwItemId = 0;
}
CWndBankPassword::~CWndBankPassword()
{
}
void CWndBankPassword::OnDraw(C2DRender* p2DRender)
{
}
void CWndBankPassword::SetBankPassword(int nFlags)
{
	m_nFlags = nFlags;
}
void CWndBankPassword::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* m_pLastPass = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndEdit* m_pNewPass = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	CWndEdit* m_pConfirmPass = (CWndEdit*)GetDlgItem(WIDC_EDIT3);

	if (m_pLastPass && m_pNewPass && m_pConfirmPass)
	{
		m_pLastPass->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);
		m_pNewPass->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);
		m_pConfirmPass->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);

		m_pLastPass->SetMaxStringNumber(CWndCreateChar::MAX_2ND_PASSWORD_NUMBER);
		m_pNewPass->SetMaxStringNumber(CWndCreateChar::MAX_2ND_PASSWORD_NUMBER);
		m_pConfirmPass->SetMaxStringNumber(CWndCreateChar::MAX_2ND_PASSWORD_NUMBER);

		if (m_nFlags == 0)
		{
			m_pLastPass->SetString("0000");
			m_pNewPass->SetFocus();
		}
		else
		{
			m_pLastPass->SetFocus();
		}
	}


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndBankPassword::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BANK_PASSWORD, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndBankPassword::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndBankPassword::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndBankPassword::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndBankPassword::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndBankPassword::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK || message == EN_RETURN)
	{
		CWndEdit* m_pLastPass = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		CWndEdit* m_pNewPass = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
		CWndEdit* m_pConfirmPass = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
		if (m_pLastPass && m_pNewPass && m_pConfirmPass)
		{
			char szLastPass[10] = { 0, };
			char szNewPass[10] = { 0, };
			char szConfirmPass[10] = { 0, };
			strcpy(szLastPass, m_pLastPass->GetString());
			strcpy(szNewPass, m_pNewPass->GetString());
			strcpy(szConfirmPass, m_pConfirmPass->GetString());
			if (strlen(szLastPass) == 4 && strlen(szNewPass) == 4 && strlen(szConfirmPass) == 4)
			{
				if (0 == strcmp(szNewPass, szConfirmPass))
				{
					BOOL bDigit = TRUE;
					for (int i = 0; i < 4; i++)
					{
						if ((isdigit2(szLastPass[i]) == FALSE) || (isdigit2(szNewPass[i]) == FALSE) || (isdigit2(szConfirmPass[i]) == FALSE))
						{
							bDigit = FALSE;
							break;
						}
					}
					if (bDigit)
					{
						int nNewPass = atoi(szNewPass), nConfirmPass = atoi(szConfirmPass);
						if (nNewPass <= 0 || nConfirmPass <= 0 || nNewPass > 9999 || nConfirmPass > 9999)
						{
							g_WndMng.CloseMessageBox();
							g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR01));
							return FALSE;
						}

						g_DPlay.SendChangeBankPass(szLastPass, szNewPass, m_dwId, m_dwItemId);
					}
					else
					{
						if (m_nFlags == 0)
						{
							m_pLastPass->SetString("0000");
						}
						else
						{
							m_pLastPass->SetString("");
						}

						m_pNewPass->SetString("");
						m_pConfirmPass->SetString("");
						g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));

					}
				}
				else
				{
					m_pNewPass->SetString("");
					m_pConfirmPass->SetString("");
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0022)));

				}
			}
			else
			{
				if (m_nFlags == 0)
				{
					m_pLastPass->SetString("0000");
				}
				else
				{
					m_pLastPass->SetString("");
				}
				m_pNewPass->SetString("");
				m_pConfirmPass->SetString("");
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));

			}
		}
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	else if (m_nFlags == 0 && message == 0 && nID == WIDC_EDIT1)
	{
		CWndEdit* m_pNewPass = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
		m_pNewPass->SetFocus();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}