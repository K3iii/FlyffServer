#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndVendor.h"

#include "WndManager.h"

#include "dpclient.h"
extern	CDPClient	g_DPlay;

#include "Chatting.h"
extern	CChatting g_Chatting;


#ifdef __FL_VENDORS
CWndVendorPreview::CWndVendorPreview()
{
	m_pWndRegVend = NULL;
}

CWndVendorPreview::~CWndVendorPreview()
{
	SAFE_DELETE(m_pWndRegVend);
}

void CWndVendorPreview::OnDraw(C2DRender* p2DRender)
{
}

void CWndVendorPreview::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SetTitle(prj.GetText(TID_GAME_VENDOR_TITLE));
	m_wndctrlVendor.Create(WLVS_ICON | WBS_NODRAWFRAME, CRect(5, 60, 440, 460), this, 100);
	m_wndctrlVendor.InitVendor(&m_pMover);

	g_WndMng.PutString("You can add or remove items from your vendor in this window without deactivating it!");
	MoveParentCenter();
}

BOOL CWndVendorPreview::Process()
{
	return TRUE;
}

BOOL CWndVendorPreview::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_VENDOR_PREVIEW, 0, 0, pWndParent);
}

BOOL CWndVendorPreview::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndVendorPreview::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndVendorPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndVendorPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndVendorPreview::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == 100 && nID == 100)
	{
		LPSHORTCUT pShortcut = (LPSHORTCUT)pLResult;
		if (!pShortcut)
			return FALSE;

		CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
		if (!pWndFrame)
			return FALSE;

		if (pWndFrame->GetWndId() != APP_INVENTORY)
			return FALSE;

		CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pItemBase))
			return FALSE;

		if (IsUsingItem(pItemBase))
			return FALSE;

		CItemElem* pItemElem = pItemBase;
		if (pItemElem->IsFlag(CItemElem::expired))
			return TRUE;

		int iIndex = pShortcut->m_dwData - 100;
		if (iIndex < 0 || iIndex >= MAX_VENDITEM)
			return FALSE;

		SAFE_DELETE(m_pWndRegVend);
		m_pWndRegVend = new CWndRegVend;
		m_pWndRegVend->Initialize(this);
		m_pWndRegVend->SetItem(iIndex, pItemBase);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndVendorPreview::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndRegVend)
		SAFE_DELETE(m_pWndRegVend);
}

void CWndVendorPreview::OnDestroy(void)
{
	SAFE_DELETE(m_pWndRegVend);
}
#endif

CWndVendor::CWndVendor()
{
	SetPutRegInfo(FALSE);
	m_pVendor = NULL;
	m_pWndRegVend = NULL;
	m_pWndVendorBuy = NULL;
#ifdef __FL_VENDORS
	m_pWndVendorItemSet = NULL;
#endif
	m_pwndVenderMessage = NULL;

}

CWndVendor::~CWndVendor()
{
	SAFE_DELETE(m_pWndRegVend);
	SAFE_DELETE(m_pWndVendorBuy);
#ifdef __FL_VENDORS
	SAFE_DELETE(m_pWndVendorItemSet);
#endif
	SAFE_DELETE(m_pwndVenderMessage);
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
}

void CWndVendor::SetVendor(CMover* pVendor)
{
	m_pVendor = pVendor;
	m_wndctrlVendor.InitVendor(m_pVendor);
	if (!m_pVendor->IsActiveMover())
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		pWndEdit->SetString(pVendor->m_vtInfo.GetTitle());
		pWndEdit->EnableWindow(FALSE);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->SetVisible(FALSE);
		pButton = (CWndButton*)GetDlgItem(WIDC_CANCEL);
		pButton->SetVisible(FALSE);
#ifndef __FL_VENDORS
	}
#else
		pButton = (CWndButton*)GetDlgItem(WIDC_RESET);
		pButton->SetVisible(FALSE);
		pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);

		if (!m_pVendor->IsPlayer())
		{
			pButton = (CWndButton*)GetDlgItem(WIDC_CHAT);
			pButton->SetVisible(FALSE);
		}
	}
	else
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		pWndEdit->SetString("Private Shop");
	}
#endif

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
	if (pWndWorld)
		pWndWorld->ClearTracking();

	if (g_WndMng.m_pWndDialog)
		Destroy();
}

void CWndVendor::OnDraw(C2DRender* p2DRender)
{

}

void CWndVendor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SetTitle(prj.GetText(TID_GAME_VENDOR_TITLE));
	m_wndctrlVendor.Create(WLVS_ICON | WBS_NODRAWFRAME, CRect(5, 60, 440, 460), this, 100);

	SetVendor(g_pPlayer);
	if (g_pPlayer && g_pPlayer->IsMode(COMMUNITY_MODE))
		Destroy();

#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif
	if (g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank)
	{
		Destroy();
		return;
	}


#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
	{
		Destroy();
		return;
	}
#endif // __S_SERVER_UNIFY

	ReloadItemList();

	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
	CRect rcInventory = pWndInventory->GetWindowRect(TRUE);
	CRect rcVendor = GetWindowRect(TRUE);
	CPoint ptInventory = rcInventory.TopLeft();
	CPoint point;
	if (ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2)
		point = ptInventory - CPoint(rcVendor.Width(), 0);
	else
		point = ptInventory + CPoint(rcInventory.Width(), 0);
	Move(point);
}

void CWndVendor::ReloadItemList()
{
	if (!g_pPlayer->m_vtInfo.IsVendorOpen())
	{
		for (int i = 0; i < MAX_VENDITEM; i++)
		{
			for (int nIndex = 0; nIndex < g_pPlayer->m_Inventory.GetMax(); nIndex++)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(nIndex);
				if (IsUsableItem(pItemElem))
				{
					if (g_Neuz.m_aSavedInven[i].m_dwObjId == pItemElem->m_dwObjId
						&& g_Neuz.m_aSavedInven[i].m_dwItemId == pItemElem->m_dwItemId)
					{

						if (g_Neuz.m_aSavedInven[i].m_nExtra > pItemElem->m_nItemNum)
						{
							g_Neuz.m_aSavedInven[i].m_dwObjId = 0;
							g_Neuz.m_aSavedInven[i].m_nExtra = 0;
							g_Neuz.m_aSavedInven[i].m_nCost = 0;
#ifdef __FL_VENDORS
							g_Neuz.m_aSavedInven[i].m_nPerinCost = 0;
#endif
							g_Neuz.m_aSavedInven[i].m_dwItemId = 0;
						}
						else
						{
#ifndef __FL_VENDORS
							g_DPlay.SendRegisterPVendorItem(i, 0, (BYTE)(g_Neuz.m_aSavedInven[i].m_dwObjId),
								g_Neuz.m_aSavedInven[i].m_nExtra, g_Neuz.m_aSavedInven[i].m_nCost);
#else
							g_DPlay.SendRegisterPVendorItem(i, 0, (BYTE)(g_Neuz.m_aSavedInven[i].m_dwObjId),
								g_Neuz.m_aSavedInven[i].m_nExtra, g_Neuz.m_aSavedInven[i].m_nCost, g_Neuz.m_aSavedInven[i].m_nPerinCost);
#endif
						}
					}
				}
			}
		}
	}
}

BOOL CWndVendor::Process()
{
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_RESET);

#ifndef __FL_VENDORS
	if (g_pPlayer->m_vtInfo.IsVendorOpen())
		pButton->SetVisible(FALSE);
	else
		pButton->SetVisible(TRUE);
#else 
	if (m_pVendor == g_pPlayer)
	{
		if (g_pPlayer->m_vtInfo.IsVendorOpen())
			pButton->SetVisible(FALSE);
		else
			pButton->SetVisible(TRUE);
	}
	else
		pButton->SetVisible(FALSE);
#endif

	return TRUE;
}

BOOL CWndVendor::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_VENDOR_REVISION, 0, 0, pWndParent);
}

BOOL CWndVendor::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndVendor::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndVendor::OnLButtonUp(UINT nFlags, CPoint point)
{

}

void CWndVendor::OnLButtonDown(UINT nFlags, CPoint point)
{

}

BOOL CWndVendor::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == 100 && nID == 100)
	{
		LPSHORTCUT pShortcut = (LPSHORTCUT)pLResult;
		if (pShortcut)
		{
			CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
			if (pWndFrame->GetWndId() == APP_INVENTORY)
			{
				CItemElem* pItemBase = m_pVendor->m_Inventory.GetAtId(pShortcut->m_dwId);
				if (pItemBase)
				{
					if (pItemBase->IsQuest())
						return FALSE;

					CItemElem* pItemElem = pItemBase;
					if (pItemElem->IsFlag(CItemElem::expired))
						return TRUE;

					int iIndex = pShortcut->m_dwData - 100;
					if (0 <= iIndex && iIndex < MAX_VENDITEM)
					{
						if (m_pVendor->m_vtInfo.GetItem(iIndex) != NULL)
							return FALSE;

						if (IsUsingItem(pItemBase) == FALSE && m_pVendor->IsActiveMover() && !m_pVendor->m_vtInfo.IsVendorOpen())
						{
							SAFE_DELETE(m_pWndRegVend);
							m_pWndRegVend = new CWndRegVend;
							m_pWndRegVend->Initialize(this);
							m_pWndRegVend->SetItem(iIndex, pItemBase);
						}
					}
				}
			}	// APP_INVENTORY
		}
	}
	else
	{
#ifndef __FL_VENDORS
		if (nID == WIDC_OK)
#else
		if (nID == WIDC_OK || nID == WIDC_BUTTON1)
#endif
		{
			if (g_pPlayer->m_pActMover->IsFly())
				return TRUE;
			if (g_pPlayer->m_vtInfo.VendorIsVendor() == FALSE)
				return TRUE;
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
			CString strVendor = pWndEdit->GetString();
			strVendor.TrimLeft();
			strVendor.TrimRight();

#ifndef __FL_VENDORS
			int nGold = g_pPlayer->GetGold();

			for (int iv = 0; iv < MAX_VENDITEM; ++iv)
			{
				CItemElem* pItemBase = g_pPlayer->m_vtInfo.GetItem(iv);
				if (pItemBase == NULL)
					continue;

				nGold += pItemBase->m_nCost * pItemBase->GetExtra();
			}

			if (nGold <= 0 || 2100000000 <= nGold)
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_VENDOR_MAX_ALL_GOLD)), MB_OK, this);
				return TRUE;
			}
#endif

			g_WndMng.WordChange(strVendor);

			if (strVendor.GetLength() < 6 || strVendor.GetLength() > 32)
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0011)));
				return TRUE;
			}

			CString strFormat = strVendor;

			if (!strVendor.IsEmpty())
			{
				pWndEdit->m_string = strFormat;
				pWndEdit->m_string.GetTextFormat(strFormat);
			}

#ifndef __FL_VENDORS
			g_DPlay.SendPVendorOpen((const char*)strFormat);
			CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
			pWndButton->SetVisible(FALSE);
#else
			SAFE_DELETE(m_pWndVendorItemSet);

			if (nID == WIDC_OK)
			{
				g_DPlay.SendPVendorOpen((const char*)strFormat, TRUE, NULL_ID);
				CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
				pWndButton->SetVisible(FALSE);
				pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				pWndButton->SetVisible(FALSE);
			}
			else
			{
				m_pWndVendorItemSet = new CWndVendorItemSet(strFormat);
				m_pWndVendorItemSet->Initialize(this, APP_VENDOR_SETITEM);
			}
#endif
		}
		else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		{
			if (g_pPlayer->m_vtInfo.IsVendorOpen())
			{
				Destroy();
				return FALSE;
			}
			Destroy();
		}
		else if (nID == WIDC_CHAT)
		{
			BOOL bChattingRoom = FALSE;
			if (g_pPlayer->m_vtInfo.IsVendorOpen())
			{
				g_DPlay.SendOpenChattingRoom();
				bChattingRoom = TRUE;
			}
			else if (g_pPlayer->m_vtInfo.GetOther() && g_Chatting.m_bState)
			{
				bChattingRoom = TRUE;
			}

			if (bChattingRoom)
			{
				if (!m_pwndVenderMessage)
				{
					m_pwndVenderMessage = new CWndVendorMessage;

					if (m_pVendor->IsActiveMover())
						m_pwndVenderMessage->m_nIsOwner = TRUE;

					m_pwndVenderMessage->Initialize(this);
				}
				else
				{
					if (m_pwndVenderMessage->IsVisible()) 
						m_pwndVenderMessage->SetVisible(FALSE);
					else								
						m_pwndVenderMessage->SetVisible(TRUE);
				}

			}
		}
		else if (nID == WIDC_RESET)
		{
			CItemElem	apItem_VT[MAX_VENDITEM];
			ZeroMemory(apItem_VT, sizeof(apItem_VT));
			for (int i = 0; i < MAX_VENDITEM; i++)
			{
				CItemElem* pItemBase = g_pPlayer->m_vtInfo.GetItem(i);
				if (pItemBase)
					g_DPlay.SendUnregisterPVendorItem(i);
			}
			memset(g_Neuz.m_aSavedInven, 0, sizeof(g_Neuz.m_aSavedInven));
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndVendor::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndVendorBuy)
	{
		SAFE_DELETE(m_pWndVendorBuy);
	}
	else if (pWndChild == m_pWndRegVend)
	{
		SAFE_DELETE(m_pWndRegVend);
	}
	else if (pWndChild == m_pwndVenderMessage)
	{
		SAFE_DELETE(m_pwndVenderMessage);
	}
#ifdef __FL_VENDORS
	else if (pWndChild == m_pWndVendorItemSet)
	{
		SAFE_DELETE(m_pWndVendorItemSet);
	}
#endif
}

void CWndVendor::OnDestroy(void)
{
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemBase = g_pPlayer->m_vtInfo.GetItem(i);
		if (pItemBase)
		{
			g_Neuz.m_aSavedInven[i].m_dwObjId = pItemBase->m_dwObjId;
			g_Neuz.m_aSavedInven[i].m_nExtra = pItemBase->m_nExtra;
			g_Neuz.m_aSavedInven[i].m_nCost = pItemBase->m_nCost;
#ifdef __FL_VENDORS
			g_Neuz.m_aSavedInven[i].m_nPerinCost = pItemBase->m_nPerinCost;
#endif
			g_Neuz.m_aSavedInven[i].m_dwItemId = pItemBase->m_dwItemId;
		}
	}

	g_DPlay.SendPVendorClose(m_pVendor->GetId());

	SAFE_DELETE(m_pWndVendorBuy);
#ifdef __FL_VENDORS
	SAFE_DELETE(m_pWndVendorItemSet);
#endif
	SAFE_DELETE(m_pWndRegVend);
	SAFE_DELETE(m_pwndVenderMessage);
}


CWndVendorMessage::CWndVendorMessage()
{
#if __VER >= 11 // __MOD_VENDOR
	m_nIsOwner = FALSE;
#endif
}

CWndVendorMessage::~CWndVendorMessage()
{
#if __VER >= 11 // __MOD_VENDOR
	m_wndChat.Destroy();
	if (m_nIsOwner)
	{
		m_wndInfo.Destroy();
	}
#endif
	//	g_Chatting.m_nSizeofMember = 0;
}
void CWndVendorMessage::OnDraw(C2DRender* p2DRender)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	pWndList->ResetContent();

	for (int i = 0; i < g_Chatting.GetChattingMember(); ++i)
	{
		pWndList->AddString(g_Chatting.m_szName[i]);
	}
}
void CWndVendorMessage::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	pWndEdit->AddWndStyle(EBS_AUTOVSCROLL);
	pWndEdit->SetNativeMode();

	pWndEdit->SetFocus();

#if __VER >= 11 // __MOD_VENDOR
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	WTCITEM tabTabItem;

	m_wndChat.Create(WBS_NODRAWFRAME, CRect(0, 0, 300, 220), pWndTabCtrl, 10);
	m_wndChat.AddWndStyle(WBS_VSCROLL);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_DIALOG);
	tabTabItem.pWndBase = &m_wndChat;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	if (m_nIsOwner)
	{
		m_wndInfo.Create(WBS_NODRAWFRAME, CRect(0, 0, 300, 220), pWndTabCtrl, 11);
		m_wndInfo.AddWndStyle(WBS_VSCROLL);
		tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
		tabTabItem.pszText = prj.GetText(TID_APP_INFOMATION);
		tabTabItem.pWndBase = &m_wndInfo;
		pWndTabCtrl->InsertItem(1, &tabTabItem);
	}

#endif

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
void CWndVendorMessage::InitSize(void)
{
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndVendorMessage::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_VENDOREX_CHAT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndVendorMessage::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndVendorMessage::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 16 * 10, 16 * 10);
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndVendorMessage::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndVendorMessage::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndVendorMessage::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndVendorMessage::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pWndText = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	switch (nID)
	{
	case WIDC_EDIT1:
		if (message == EN_RETURN)
		{
			if (pWndText->m_string.IsEmpty() == FALSE)
			{
				char szChat[1024];
				if (strlen(pWndText->m_string) < 1020)
				{
					CString strFormat;
					pWndText->m_string.GetTextFormat(strFormat);
					if (strlen(strFormat) < 1020)
					{
						strcpy(szChat, strFormat);
						g_DPlay.SendChatting(szChat);
						pWndText->Empty();
					}
				}
			}
		}
		break;
	case WIDC_BUTTON1:
	{
		if (pWndText->m_string.IsEmpty() == FALSE)
		{
			char szChat[1024];
			if (strlen(pWndText->m_string) < 1020)
			{
				CString strFormat;
				pWndText->m_string.GetTextFormat(strFormat);
				if (strlen(strFormat) < 1020)
				{
					strcpy(szChat, strFormat);
					g_DPlay.SendChatting(szChat);
					pWndText->Empty();
				}
			}
		}
	}
	break;
	case WTBID_CLOSE:
	{
		if (g_pPlayer->m_vtInfo.IsVendorOpen())
			g_DPlay.SendCloseChattingRoom();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndVendorMessage::AddMessage(LPCTSTR lpszFrom, LPCTSTR lpszMessage)
{


#if __VER >= 11 // __MOD_VENDOR
	CString strMessage;

	if (!strcmp(lpszFrom, g_pPlayer->GetName()))
		strMessage.Format("#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);
	else
		strMessage.Format("#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);


	m_wndChat.AddString(strMessage);
	m_wndChat.m_wndScrollBar.SetMaxScrollPos();

#else
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	CString strMessage;

	if (!strcmp(lpszFrom, g_pPlayer->GetName()))
		strMessage.Format("#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);
	else
		strMessage.Format("#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);


	pWndText->AddString(strMessage);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif

}

void CWndVendorMessage::AddChattingMemver(LPCTSTR lpszName)
{
#if __VER >= 11 // __MOD_VENDOR
	if (m_nIsOwner)
	{
		CString str;
		str.Format(prj.GetText(TID_GAME_VENCHATIN), lpszName);

		CString strMessage;
		strMessage.Format("#cffff0000%s\n", str);

		m_wndInfo.AddString(strMessage);
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();
	}
#else
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	CString str;
	str.Format(prj.GetText(TID_GAME_VENCHATIN), lpszName);

	CString strMessage;
	strMessage.Format("#cffff0000%s\n", str);

	pWndText->AddString(strMessage);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif
}

#if __VER >= 11 // __MOD_VENDOR
void CWndVendorMessage::WriteBuyInfo(char* pBuyerName, CString strItem, int nItemNum, int nTotalCost)
{
	if (m_nIsOwner)
	{
		CString str;
		str.Format(prj.GetText(TID_GAME_BUY_INFO), pBuyerName, (LPSTR)(LPCSTR)strItem, nItemNum, nTotalCost);

		CString strMessage;
		strMessage.Format("#cff0000ff%s\n", str);

		m_wndInfo.AddString(strMessage);
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();
	}
}
#endif

void CWndVendorMessage::RemoveChattingMemver(LPCTSTR lpszName)
{
#if __VER >= 11 // __MOD_VENDOR
	if (m_nIsOwner)
	{
		CString str;
		str.Format(prj.GetText(TID_GAME_VENCHATOUT), lpszName);

		CString strMessage;
		strMessage.Format("#cffff0000%s\n", str);

		m_wndInfo.AddString(strMessage);
		m_wndInfo.m_wndScrollBar.SetMaxScrollPos();
	}
#else
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	CString str;
	str.Format(prj.GetText(TID_GAME_VENCHATOUT), lpszName);

	CString strMessage;
	strMessage.Format("#cffff0000%s\n", str);

	pWndText->AddString(strMessage);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif
}