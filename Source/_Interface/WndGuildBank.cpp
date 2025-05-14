#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildBank.h"

#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

CWndGuildBank::CWndGuildBank()
{
	m_pwndGuildMerit = NULL;
	m_pWndGuildBankLog = NULL;
}
CWndGuildBank::~CWndGuildBank()
{
	g_DPlay.SendCloseGuildBankWnd();

	SAFE_DELETE(m_pwndGuildMerit);
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
	SAFE_DELETE(m_pWndGuildBankLog);
}
void CWndGuildBank::OnDraw(C2DRender* p2DRender)
{
	if (g_pPlayer && g_pPlayer->GetGuild())
	{
		CString			string;
		string.Format(_T("%u"), g_pPlayer->GetGuild()->m_nGoldGuild);
		CWndStatic* pCost = (CWndStatic*)GetDlgItem(WIDC_GUILDBANK_NUM);

		pCost->SetTitle(string);
	}
}
void CWndGuildBank::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif
	if (g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
	{
		Destroy();
		return;
	}


	CWndStatic* pCost = (CWndStatic*)GetDlgItem(WIDC_GUILDBANK_NUM);
	pCost->AddWndStyle(WSS_MONEY);

	CWndTabCtrl* pTabCtrl = reinterpret_cast<CWndTabCtrl*>(GetDlgItem(WIDC_TABCTRL1));

	m_wndItemCtrl.Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, 11);
	m_wndItemCtrl.InitItem(&g_pPlayer->GetGuild()->m_GuildBank, APP_GUILD_BANK);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_ITEM);
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem(0, &tabTabItem);

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	m_wndGold.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_CUSTOM1);
	m_wndGold.AddWndStyle(WBS_NODRAWFRAME);

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

	CWndButton* pWndButtLog = (CWndButton*)GetDlgItem(WIDC_LOG);
	pWndButtLog->EnableWindow(TRUE);
	pWndButtLog->SetVisible(TRUE);

	CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pSearchBox)
		pSearchBox->SetStringBg("Search..");
}

BOOL CWndGuildBank::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_BANK, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildBank::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildBank::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildBank::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildBank::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildBank::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (WIN_DBLCLK == message)
		return FALSE;

	CWndBase* pWndFrame = NULL;
	LPSHORTCUT	lpShortcut = NULL;
	if (message == WNM_CLICKED)
		pWndFrame = (CWndBase*)pLResult;
#ifdef __CONTAINER_RCLICKS
	else if (message == WNM_ITEMCLICK)
		pWndFrame = (CWndBase*)pLResult;
#endif
	else if (WIN_ITEMDROP)
	{
		lpShortcut = reinterpret_cast<LPSHORTCUT>(pLResult);
		if (lpShortcut == NULL)
			return FALSE;
		if (lpShortcut->m_pFromWnd == NULL)
			return FALSE;

		pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
	}
	else
		return FALSE;

	if (!pWndFrame)
		return FALSE;

	if (pWndFrame->GetWndId() == APP_INVENTORY)
	{
		switch (message)
		{
		case	WIN_ITEMDROP:
		{
			if (nID == 11) // item
			{
				BOOL bForbid = TRUE;
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
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GUILD_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;

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
							g_DPlay.SendPutItemGuildBank((BYTE)(lpShortcut->m_dwId), 1, 1);
						}

					}
				}
			}
		}
		break;
		}
	}
	else
	{
		switch (nID)
		{
		case WIDC_BUTTON1:
		{
			SAFE_DELETE(m_pwndGuildMerit);

			CGuild* pGuild = g_pPlayer->GetGuild();
			if (pGuild)
			{
				if (pGuild->m_szGuild[0] == '\0')
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDNOTHINGNAME), MB_OK, this);
				}
				else
				{
					m_pwndGuildMerit = new CWndGuildMerit;
					m_pwndGuildMerit->Initialize(this);
				}
			}
		}
		break;
		case WIDC_LOG:
		{
			CGuild* pGuild = g_pPlayer->GetGuild();
			CGuildMember* pGuildMemberl;

			pGuildMemberl = pGuild->GetMember(g_pPlayer->m_idPlayer);

			if (pGuildMemberl && (pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN))
			{
				CWndGuildBankLog* pwndbanklog = (CWndGuildBankLog*)GetWndBase(APP_GUILD_BANK_LOG);
				if (pwndbanklog == NULL)
				{
					SAFE_DELETE(m_pWndGuildBankLog);
					m_pWndGuildBankLog = new CWndGuildBankLog();
					m_pWndGuildBankLog->Initialize(this);
				}
				else
					SAFE_DELETE(m_pWndGuildBankLog);
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_CANNT_USE_GLOG), NULL, prj.GetTextColor(TID_GAME_CANNT_USE_GLOG));
		}
		break;
		case WIDC_BUTTON_1:
		{
			CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
			KillFocus(pSearchBox, NULL);
			pSearchBox->Empty();
		}
		break;

		}
	}

#ifdef __CONTAINER_RCLICKS
	if (message == WNM_ITEMCLICK)
	{
		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild && pGuild->IsGetItem(g_pPlayer->m_idPlayer))
		{
			CItemElem* pItem = (CItemElem*)pLResult;
			if (IsUsableItem(pItem))
			{
				if (g_WndMng.GetWndBase(APP_INVENTORY))
					g_DPlay.SendGetItemGuildBank((BYTE)pItem->m_dwObjId, pItem->m_nItemNum, 1);
			}
		}
	}
#endif

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildBank::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndGuildBankLog)
		SAFE_DELETE(m_pWndGuildBankLog);
	if (pWndChild == m_pwndGuildMerit)
		SAFE_DELETE(m_pwndGuildMerit);
}

#if __VER >= 11 // __GUILD_BANK_LOG

CWndGuildBankLog::CWndGuildBankLog()
{
}

CWndGuildBankLog::~CWndGuildBankLog()
{
}

void CWndGuildBankLog::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildBankLog::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	WTCITEM tabTabItem;

	CRect rect = pWndTabCtrl->GetClientRect();
	rect.bottom -= 15;

	m_wndAddItemLog.Create(rect, pWndTabCtrl, 11);
	m_wndAddItemLog.AddWndStyle(WBS_NODRAWFRAME);

	m_wndRemoveItemLog.Create(rect, pWndTabCtrl, 12);
	m_wndRemoveItemLog.AddWndStyle(WBS_NODRAWFRAME);

	m_wndReceivePenyaLog.Create(rect, pWndTabCtrl, 13);
	m_wndReceivePenyaLog.AddWndStyle(WBS_NODRAWFRAME);

	m_wndInvestPenyaLog.Create(rect, pWndTabCtrl, 14);
	m_wndInvestPenyaLog.AddWndStyle(WBS_NODRAWFRAME);

	m_wndAddItemLog.UpdateLogList();

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_GUILD_BANK_ADDITEMLOG);
	tabTabItem.pWndBase = &m_wndAddItemLog;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_GUILD_BANK_REMOVEITEMLOG);
	tabTabItem.pWndBase = &m_wndRemoveItemLog;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_GUILD_BANK_RECEIVEPENYALOG);
	tabTabItem.pWndBase = &m_wndReceivePenyaLog;
	pWndTabCtrl->InsertItem(2, &tabTabItem);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_GUILD_BANK_INVESTPENYALOG);
	tabTabItem.pWndBase = &m_wndInvestPenyaLog;
	pWndTabCtrl->InsertItem(3, &tabTabItem);

	CWndGuildBank* pWndGuildBank = (CWndGuildBank*)GetWndBase(APP_GUILD_BANK);
	if (pWndGuildBank != NULL)
	{
		CRect rectRoot = pWndGuildBank->m_pWndRoot->GetLayoutRect();
		CRect rectBank = pWndGuildBank->GetWindowRect(TRUE);

		CPoint ptMove;
		CPoint ptBank;

		CRect rect = GetWindowRect(TRUE);
		int wndWidth = rect.Width();

		if (rectRoot.right - rectBank.right < wndWidth)
		{
			ptBank = rectBank.TopLeft();
			ptMove = ptBank + CPoint(-(10 + wndWidth), 0);
		}
		else
		{
			ptBank = rectBank.BottomRight();
			ptMove = ptBank + CPoint(10, -rectBank.Height());
		}

		Move(ptMove);
	}
	else
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point(rectRoot.right - rectWindow.Width(), rectRoot.bottom - rectWindow.Height());
		Move(point);
	}
}


BOOL CWndGuildBankLog::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_BANK_LOG, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildBankLog::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildBankLog::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildBankLog::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildBankLog::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_TABCTRL1)
	{
		CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
		CWndBase* pChild = pTabCtrl->GetFocusChild();
		if (pChild == &m_wndAddItemLog)
			m_wndAddItemLog.UpdateLogList();
		else if (pChild == &m_wndRemoveItemLog)
			m_wndRemoveItemLog.UpdateLogList();
		else if (pChild == &m_wndReceivePenyaLog)
			m_wndReceivePenyaLog.UpdateLogList();
		else if (pChild == &m_wndInvestPenyaLog)
			m_wndInvestPenyaLog.UpdateLogList();
	}
	else if (nID == WIDC_OK)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildBankLog::UpdateScroll()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if (pChild == &m_wndAddItemLog)
		m_wndAddItemLog.UpdateScroll();
	else if (pChild == &m_wndRemoveItemLog)
		m_wndRemoveItemLog.UpdateScroll();
	else if (pChild == &m_wndReceivePenyaLog)
		m_wndReceivePenyaLog.UpdateScroll();
	else if (pChild == &m_wndInvestPenyaLog)
		m_wndInvestPenyaLog.UpdateScroll();
}


CWndAddItemLog::CWndAddItemLog()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;

	m_vLogList.clear();
}

CWndAddItemLog::~CWndAddItemLog()
{
}

void CWndAddItemLog::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndAddItemLog::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);
}

void CWndAddItemLog::UpdateScroll()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vLogList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.SetScrollPos(0);
}

void CWndAddItemLog::UpdateLogList()
{
	m_vLogList.clear();
	BYTE nType = 0x01;
	g_DPlay.SendReqGuildBankLogList(nType);
}

void CWndAddItemLog::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	vector < CString >::iterator iter = m_vLogList.begin();

	for (int i = 0; iter < m_vLogList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i > nMax)
			return;

		p2DRender->TextOut(4, pt.y, *iter, 0xff000000);

		pt.y += m_nFontHeight;
	}
}

int CWndAddItemLog::GetDrawCount(void)
{
	int nMax = 0;
	nMax = m_vLogList.size();

	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}


CWndRemoveItemLog::CWndRemoveItemLog()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;

	m_vLogList.clear();
}

CWndRemoveItemLog::~CWndRemoveItemLog()
{
}

void CWndRemoveItemLog::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndRemoveItemLog::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);
}

void CWndRemoveItemLog::UpdateScroll()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vLogList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.SetScrollPos(0);
}

void CWndRemoveItemLog::UpdateLogList()
{
	m_vLogList.clear();
	BYTE nType = 0x02;
	g_DPlay.SendReqGuildBankLogList(nType);
}

void CWndRemoveItemLog::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	vector < CString >::iterator iter = m_vLogList.begin();

	for (int i = 0; iter < m_vLogList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i > nMax)
			return;

		p2DRender->TextOut(4, pt.y, *iter, 0xff000000);

		pt.y += m_nFontHeight;
	}
}

int CWndRemoveItemLog::GetDrawCount(void)
{
	int nMax = 0;
	nMax = m_vLogList.size();

	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}

CWndReceivePenyaLog::CWndReceivePenyaLog()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;

	m_vLogList.clear();
}

CWndReceivePenyaLog::~CWndReceivePenyaLog()
{
}

void CWndReceivePenyaLog::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndReceivePenyaLog::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);
}

void CWndReceivePenyaLog::UpdateScroll()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vLogList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.SetScrollPos(0);
}

void CWndReceivePenyaLog::UpdateLogList()
{
	m_vLogList.clear();
	BYTE nType = 0x03;
	g_DPlay.SendReqGuildBankLogList(nType);
}

void CWndReceivePenyaLog::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	vector < CString >::iterator iter = m_vLogList.begin();

	for (int i = 0; iter < m_vLogList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i > nMax)
			return;

		p2DRender->TextOut(10, pt.y, *iter, 0xff000000);

		pt.y += m_nFontHeight;
	}
}

int CWndReceivePenyaLog::GetDrawCount(void)
{
	int nMax = 0;
	nMax = m_vLogList.size();

	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}

CWndInvestPenyaLog::CWndInvestPenyaLog()
{
	m_nCurSelect = -1;
	m_nFontHeight = 20;
	m_nDrawCount = 0;

	m_vLogList.clear();
}

CWndInvestPenyaLog::~CWndInvestPenyaLog()
{
}

void CWndInvestPenyaLog::Create(RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndInvestPenyaLog::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);
}

void CWndInvestPenyaLog::UpdateScroll()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = m_vLogList.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
	m_wndScrollBar.SetScrollPos(0);
}

void CWndInvestPenyaLog::UpdateLogList()
{
	m_vLogList.clear();
	BYTE nType = 0x04;
	g_DPlay.SendReqGuildBankLogList(nType);
}

void CWndInvestPenyaLog::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nDrawCount = 0;

	if (NULL == g_pPlayer)
		return;

	int nMax = GetDrawCount();
	m_nDrawCount = m_wndScrollBar.GetScrollPos();

	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	vector < CString >::iterator iter = m_vLogList.begin();

	for (int i = 0; iter < m_vLogList.end(); i++, iter++)
	{
		CString strFormat;

		if (i < m_nDrawCount)
			continue;
		if (i > nMax)
			return;

		p2DRender->TextOut(10, pt.y, *iter, 0xff000000);

		pt.y += m_nFontHeight;
	}
}

int CWndInvestPenyaLog::GetDrawCount(void)
{
	int nMax = 0;
	nMax = m_vLogList.size();

	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	return nMax;
}
#endif //__GUILD_BANK_LOG