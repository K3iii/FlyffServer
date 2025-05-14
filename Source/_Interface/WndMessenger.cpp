#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndMessenger.h"
#include "WndFriendCtrl.h"
#include "messenger.h"
#include "WndFriendConFirm.h"
#include "dpcertified.h"
extern	CDPCertified	g_dpCertified;

#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 11 // __CSC_VER11_4
CWndMessengerEx::CWndMessengerEx()
{
}

CWndMessengerEx::~CWndMessengerEx()
{
	m_TexMail.DeleteDeviceObjects();
}

void CWndMessengerEx::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	if (ar.IsLoading())
	{
		int nCurSel;
		ar >> nCurSel;
		lpTabCtrl->SetCurSel(nCurSel);
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}

void CWndMessengerEx::OnDraw(C2DRender* p2DRender)
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	// Draw Status Icon
	DWORD dwMyState;
	if (g_WndMng.m_RTMessenger.GetState() == FRS_ONLINE)
		dwMyState = 2;
	else if (g_WndMng.m_RTMessenger.GetState() == FRS_OFFLINE)
		dwMyState = 8;
	else
		dwMyState = g_WndMng.m_RTMessenger.GetState();

	TEXTUREVERTEX2 vertex[6];
	TEXTUREVERTEX2* pVertices = vertex;

	pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, CPoint(8, 8), dwMyState - 2, &pVertices, 0xffffffff);
	pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, vertex, ((int)pVertices - (int)vertex) / sizeof(TEXTUREVERTEX2));

	// Draw Name & Status
	CString strState;
	switch (g_WndMng.m_RTMessenger.GetState())
	{
	case FRS_ONLINE:
		strState.Format("(%s)", prj.GetText(TID_FRS_ONLINE_STATUS));
		break;
	case FRS_OFFLINE:
		strState.Format("(%s)", prj.GetText(TID_FRS_OFFLINE_STATUS));
		break;
	case FRS_ABSENT:
		strState.Format("(%s)", prj.GetText(TID_FRS_ABSENT));
		break;
	case FRS_HARDPLAY:
		strState.Format("(%s)", prj.GetText(TID_FRS_HARDPLAY));
		break;
	case FRS_EAT:
		strState.Format("(%s)", prj.GetText(TID_FRS_EAT));
		break;
	case FRS_REST:
		strState.Format("(%s)", prj.GetText(TID_FRS_REST));
		break;
	case FRS_MOVE:
		strState.Format("(%s)", prj.GetText(TID_FRS_MOVE));
		break;
	}

	LPCTSTR pszName = g_pPlayer->GetName(TRUE);
	p2DRender->TextOut(47, 10, 1, 1, pszName, 0xff606060);
	p2DRender->TextOut(48, 10, 1, 1, pszName, 0xff606060);
	p2DRender->TextOut(46, 27, 1, 1, strState, 0xff606060);

	if (g_pPlayer && g_pPlayer->IsMode(MODE_MAILBOX))
	{
		LPWNDCTRL lpMail = GetWndCtrl(WIDC_CUSTOM1);
		p2DRender->RenderTexture(CPoint(lpMail->rect.left, lpMail->rect.top), &m_TexMail, m_nFlashCounter);
	}


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	if (pWndTabCtrl == NULL)
		return;

	CString strServerName = _T("");
	strServerName.Format("%s", g_dpCertified.GetServerName(g_Option.m_nSer));
	g_WndMng.StringSize(strServerName, 18);
	p2DRender->TextOut(170, 13, 1, 1, strServerName, 0xff606060);

	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for (int j = 0; j < (int)(g_dpCertified.m_dwSizeofServerset); ++j)
	{
		if (g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if (nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if (g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if (pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format("%s", g_dpCertified.m_aServerset[j + g_Option.m_nMSer].lpName);
				g_WndMng.StringSize(strServerName, 18);
				p2DRender->TextOut(170, 27, 1, 1, strServerName, 0xff606060);
				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}
}

BOOL CWndMessengerEx::Process()
{
	if (g_pPlayer && g_pPlayer->IsMode(MODE_MAILBOX))
	{
		m_nFlashCounter += (m_nSwitch * 8);

		if (m_nFlashCounter >= 255)
		{
			m_nFlashCounter = 255;
			m_nSwitch = -1;
		}

		if (m_nFlashCounter <= 50)
		{
			m_nFlashCounter = 50;
			m_nSwitch = 1;
		}

		LPWNDCTRL lpMail = GetWndCtrl(WIDC_CUSTOM1);
		lpMail->m_bDisabled = FALSE;

		CRect  rect = lpMail->rect;
		CPoint point = GetMousePoint();
		if (PtInRect(lpMail->rect, point))
		{
			ClientToScreen(&point);
			ClientToScreen(&rect);
			g_toolTip.PutToolTip((DWORD)this, prj.GetText(TID_MAIL_RECEIVE), rect, point);
		}
	}
	return TRUE;
}

#ifdef __FL_SAVE_MESSAGES
void CWndMessengerEx::UpdateList()
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndList->ResetContent();

	for (auto& it : g_Neuz.m_mapPrivateMessages)
		pWndList->AddString(it.first.c_str());
}
void CWndMessengerEx::SetExtension(BOOL eflag)
{
	CRect rect = GetWindowRect(TRUE);

	if (eflag)
		rect.right += 144;
	else
		rect.right -= 144;

	SetWndRect(rect);
}
#endif

void CWndMessengerEx::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

#ifdef __FL_SAVE_MESSAGES
	m_PostMessagesMenu.CreateMenu(this);
	UpdateList();

	if (!g_Option.m_bBigMessenger)
		SetExtension(FALSE);
#endif

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	WTCITEM tabTabItem;

	m_wndFriend.Create(CRect(0, 0, 250, 250), pWndTabCtrl, 11);
	m_wndFriend.AddWndStyle(WBS_NODRAWFRAME);
	if (g_WndMng.m_RTMessenger.size())
	{
		g_DPlay.SendGetFriendState();
	}

	m_wndGuild.Create(CRect(0, 0, 250, 250), pWndTabCtrl, 13);
	m_wndGuild.AddWndStyle(WBS_NODRAWFRAME);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMMUNITY_FRIEND);
	tabTabItem.pWndBase = &m_wndFriend;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_APP_COMPANY);
	tabTabItem.pWndBase = &m_wndGuild;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	m_wndFriend.ScrollBarPos(0);
	m_wndGuild.ScrollBarPos(0);

	m_menuState.CreateMenu(this);
	m_menuState.AppendMenu(0, FRS_ONLINE, prj.GetText(TID_FRS_ONLINE_STATUS));
	m_menuState.AppendMenu(0, FRS_ABSENT, prj.GetText(TID_FRS_ABSENT));
	m_menuState.AppendMenu(0, FRS_HARDPLAY, prj.GetText(TID_FRS_HARDPLAY));
	m_menuState.AppendMenu(0, FRS_EAT, prj.GetText(TID_FRS_EAT));
	m_menuState.AppendMenu(0, FRS_REST, prj.GetText(TID_FRS_REST));
	m_menuState.AppendMenu(0, FRS_MOVE, prj.GetText(TID_FRS_MOVE));
	m_menuState.AppendMenu(0, FRS_OFFLINE, prj.GetText(TID_FRS_OFFLINE_STATUS));

	m_TexMail.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndMail.dds"), 0xffff00ff);
	m_nFlashCounter = 0;
	m_nSwitch = 1;


	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_CHANNEL);
	if (pWndButton)
	{
		//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerChannel.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerChannel.bmp"), 0xffff00ff);
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_STATE);
	if (pWndButton)
	{
		//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerState.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerState.bmp"), 0xffff00ff);
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_LEVEL);
	if (pWndButton)
	{
		//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerLevel.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerLevel.bmp"), 0xffff00ff);
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_JOB);
	if (pWndButton)
	{
		//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerJob.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerJob.bmp"), 0xffff00ff);
	}

	pWndButton = (CWndButton*)GetDlgItem(WIDC_NAME);
	if (pWndButton)
	{
		//		if(::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerChar.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtMessengerChar.bmp"), 0xffff00ff);
	}


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), rectRoot.bottom - rectWindow.Height());
	Move(point);
}


BOOL CWndMessengerEx::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MESSENGER_EX, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndMessengerEx::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
#ifdef __FL_SAVE_MESSAGES
	if (pWndBase == &m_PostMessagesMenu)
	{
		if (nID == 0)
		{
			CWndBase* pWndBaseBuf = g_WndMng.GetWndBase(APP_TRADE);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTPOST), NULL, prj.GetTextColor(TID_GAME_TRADENOTPOST));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_POST);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_MAIL_OTHERWORK), NULL, prj.GetTextColor(TID_MAIL_OTHERWORK));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_COMMON_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKTRADESELF), NULL, prj.GetTextColor(TID_GAME_BANKTRADESELF));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_CONFIRM_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_GUILD_BANK);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BANK_PASSWORD);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BANKSTILLUSING), NULL, prj.GetTextColor(TID_GAME_BANKSTILLUSING));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_REPAIR);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOACTION), NULL, prj.GetTextColor(TID_GAME_REPAIR_NOACTION));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SHOP_);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_TRADENOTBEAUTYSHOP), NULL, prj.GetTextColor(TID_GAME_TRADENOTBEAUTYSHOP));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_SKIN);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_FACESHOPUSING), NULL, prj.GetTextColor(TID_GAME_FACESHOPUSING));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_BEAUTY_SHOP_EX);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_BEAUTYSHOPUSING), NULL, prj.GetTextColor(TID_GAME_BEAUTYSHOPUSING));
				return FALSE;
			}
			pWndBaseBuf = g_WndMng.GetWndBase(APP_SUMMON_ANGEL);
			if (pWndBaseBuf)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_SUMMONANGELUSING), NULL, prj.GetTextColor(TID_GAME_SUMMONANGELUSING));
				return FALSE;
			}
			if (g_pPlayer->m_pActMover->IsMove() == TRUE)
			{
				g_WndMng.PutString(prj.GetText(TID_PK_BLINK_LIMIT), NULL, prj.GetTextColor(TID_PK_BLINK_LIMIT));
				return FALSE;
			}

			g_WndMng.CreateApplet(APP_INVENTORY);
			SAFE_DELETE(g_WndMng.m_pWndPost);
			g_WndMng.m_pWndPost = new CWndPost;
			g_WndMng.m_pWndPost->Initialize(NULL, APP_POST);
		}
		else if (nID == 1)
		{
			if (!g_WndMng.m_pWndAddFriend)
			{
				g_WndMng.m_pWndAddFriend = new CWndAddFriend;
				g_WndMng.m_pWndAddFriend->Initialize();
			}
		}
		m_PostMessagesMenu.SetVisible(FALSE);
	}
#endif
	else if (pWndBase == &m_menuState)
	{
		if (FRS_ONLINE <= nID && nID < MAX_FRIENDSTAT)
		{
			g_DPlay.SendSetState(nID);
		}
	}

	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndMessengerEx::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
#ifdef __FL_SAVE_MESSAGES
	if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_LISTBOX1)
		{
			CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

			if (pWndList->GetCurSel() >= 0)
			{
				CString str = pWndList->GetString(pWndList->GetCurSel());

				CWndMessage* pWndMessage = g_WndMng.GetMessagge(str);
				if (!pWndMessage)
					pWndMessage = g_WndMng.OpenMessage(str);
			}
		}
	}

	if (nID == WIDC_BUTTON3)
	{
		CPoint point = GetMousePoint();
		ClientToScreen(&point);
		m_PostMessagesMenu.DeleteAllMenu();
		m_PostMessagesMenu.AppendMenu(0, 0, "Postbox");
		m_PostMessagesMenu.AppendMenu(0, 1, "Invite Friend");
		m_PostMessagesMenu.Move(CPoint(point.x, point.y - m_PostMessagesMenu.GetMenuItemCount() * 25));
		m_PostMessagesMenu.SetVisible(TRUE);
		m_PostMessagesMenu.SetFocus();
	}
	else if (nID == WIDC_BUTTON4)
	{
		SetExtension(!g_Option.m_bBigMessenger);
		g_Option.m_bBigMessenger = !g_Option.m_bBigMessenger;
	}
#endif

	if (nID == WIDC_BUTTON1)
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		CRect rect = pWndButton->GetWindowRect(TRUE);
		ClientToScreen(&rect);
		rect.OffsetRect(CPoint(0, rect.Height()));
		m_menuState.Move(rect.TopLeft());
		m_menuState.SetVisible(TRUE);
		m_menuState.SetFocus();
	}
	else if (nID == WIDC_TABCTRL1)
	{
		CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
		CWndBase* pChild = pTabCtrl->GetFocusChild();
		if (pChild == &m_wndFriend)
			m_wndFriend.UpdatePlayerList();
		else if (pChild == &m_wndGuild)
			m_wndGuild.UpdatePlayerList();
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	int nFocusChild = 0;
	if (pChild == &m_wndFriend)
		nFocusChild = 1;
	else if (pChild == &m_wndGuild)
		nFocusChild = 2;

	switch (nID)
	{
	case WIDC_CHANNEL:
	{
		if (nFocusChild == 1)
			m_wndFriend.SortbyChannel();
		else if (nFocusChild == 2)
			m_wndGuild.SortbyChannel();
	}
	break;
	case WIDC_STATE:
	{
		if (nFocusChild == 1)
			m_wndFriend.SortbyStatus();
		else if (nFocusChild == 2)
			m_wndGuild.SortbyStatus();
	}
	break;
	case WIDC_LEVEL:
	{
		if (nFocusChild == 1)
			m_wndFriend.SortbyLevel();
		else if (nFocusChild == 2)
			m_wndGuild.SortbyLevel();
	}
	break;
	case WIDC_JOB:
	{
		if (nFocusChild == 1)
			m_wndFriend.SortbyJob();
		else if (nFocusChild == 2)
			m_wndGuild.SortbyJob();
	}
	break;
	case WIDC_NAME:
	{
		if (nFocusChild == 1)
			m_wndFriend.SortbyName();
		else if (nFocusChild == 2)
			m_wndGuild.SortbyName();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndMessengerEx::UpdateFriendList()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if (pChild == &m_wndFriend)
		m_wndFriend.UpdatePlayerList();
}

void CWndMessengerEx::UpdateGuildMemberList()
{
	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CWndBase* pChild = pTabCtrl->GetFocusChild();
	if (pChild == &m_wndGuild)
		m_wndGuild.UpdatePlayerList();
}

#endif //__CSC_VER11_4

CWndInstantMsg::CWndInstantMsg()
{
}
CWndInstantMsg::~CWndInstantMsg()
{
}
void CWndInstantMsg::OnDraw(C2DRender* p2DRender)
{
	if (m_timer.IsTimeOut())
	{
		Destroy();
	}
}
void CWndInstantMsg::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_timer.Set(SEC(10));

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), rectRoot.bottom - rectWindow.Height());
	Move(point);
}

BOOL CWndInstantMsg::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_INSTANTMSG, WBS_NOFOCUS, CPoint(0, 0), pWndParent);
}

BOOL CWndInstantMsg::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndInstantMsg::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndInstantMsg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_strPlayer == "Mail" || m_strPlayer == "Event")
		return;

	CWndMessage* pWndMessage = g_WndMng.OpenMessage(m_strPlayer);
#ifndef __FL_SAVE_MESSAGES
	pWndMessage->AddMessage(m_strPlayer, m_strMessage);
#endif
	Destroy();
}
void CWndInstantMsg::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndInstantMsg::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID != WTBID_CLOSE)
	{
		if (m_strPlayer == "Event")
			Destroy();
		else if (m_strPlayer != "Mail")
		{
			CWndMessage* pWndMessage = g_WndMng.OpenMessage(m_strPlayer);
#ifndef __FL_SAVE_MESSAGES
			pWndMessage->AddMessage(m_strPlayer, m_strMessage);
#endif
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndInstantMsg::AddMessage(LPCTSTR lpszFrom, LPCTSTR lpszMessage)
{
	PLAYSND(SND_INF_MESSENGERRING);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strMessage;
	m_strMessage = lpszMessage;
	strMessage.Format("#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);

	pWndText->AddString(strMessage);
	m_timer.Reset();
	//	pWndText->m_wndScrollBar.SetMaxScrollPos();
}

void CWndInstantMsg::AddMessageJoin(LPCTSTR lpszJoinName)
{
	PLAYSND(SND_INF_MESSENGERRING);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strMessage;
	strMessage.Format("#cff0000ff%s %s\n", lpszJoinName, prj.GetText(TID_GAME_LOGIN));

	pWndText->AddString(strMessage);
	m_timer.Reset();
}

void CWndInstantMsg::AddPostMessage(LPCTSTR lpszSendName)
{
	PLAYSND(SND_INF_MESSENGERRING);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	pWndText->AddString(lpszSendName);
	m_timer.Reset();
}
////////////////////////////////////////////////////

CWndMessage::CWndMessage()
{
}
CWndMessage::~CWndMessage()
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	if (pWndEdit->m_wndScrollBar.GetScrollPos() != pWndEdit->m_wndScrollBar.GetMaxScrollPos())
		pWndEdit->m_wndScrollBar.SetMaxScrollPos();
	else if (pWndEdit->m_wndScrollBar.GetMaxScrollPos())
		pWndEdit->m_wndScrollBar.EnableScrollBar();
}
void CWndMessage::OnDraw(C2DRender* p2DRender)
{
}
void CWndMessage::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	pWndEdit->AddWndStyle(EBS_AUTOVSCROLL);
	pWndEdit->AddWndStyle(EBS_AUTOHSCROLL);
	pWndEdit->SetNativeMode();

	pWndEdit->SetFocus();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
void CWndMessage::InitSize(void)
{
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndMessage::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MESSAGE, WBS_THICKFRAME, CPoint(0, 0), pWndParent);
}

BOOL CWndMessage::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndMessage::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 16 * 10, 16 * 10);
	//	AdjustMaxRect( &rectWnd, 16 * 12, 16 * 12 );
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndMessage::OnSize(UINT nType, int cx, int cy) \
{
	CRect rect = GetClientRect();//GetWndRect();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndButton* pWndSend = (CWndButton*)GetDlgItem(WIDC_SEND);

	rect.top += 4;
	rect.left += 4;
	rect.right -= 4;
	rect.bottom -= 50;
	pWndText->SetWndRect(rect);

	rect = GetClientRect();//GetWndRect();
	rect.top = rect.bottom - 45;
	rect.left += 4;
	rect.right -= 80;
	rect.bottom -= 4;
	pWndEdit->SetWndRect(rect);

	rect = GetClientRect();//GetWndRect();
	rect.top = rect.bottom - 45;
	rect.left = rect.right - 74;
	rect.right -= 4;
	rect.bottom -= 4;
	//pWndSend->SetWndRect( rect );
	pWndSend->Move(rect.TopLeft());//rect.right + 4, rect.top );

	//pAdd->Move( rect.left + 5, rect.bottom + 2 );
	//pFind->Move( rect.left + 25, rect.bottom + 2 );

	//m_wndPlace.Move( CPoint( 0, 0 ) );
	//m_wndZoomIn.Move( CPoint(0, cy - 32 ) );
	//m_wndZoomOut.Move( CPoint(0, cy - 16 ) );

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndMessage::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndMessage::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndMessage::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pWndText = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	switch (nID)
	{
	case WIDC_EDIT:
		pWndText->m_wndScrollBar.SetMaxScrollPos();
		if (message == EN_RETURN)
		{
			if (pWndText->m_string.IsEmpty() == FALSE)
			{
				CString strFormat = pWndText->m_string;
				pWndText->m_string.GetTextFormat(strFormat);
				CString string;
				string.Format("/say \"%s\" %s", m_strPlayer, strFormat);

				if (string.GetLength() >= 260)
				{
					g_WndMng.PutString(prj.GetText(TID_GAME_MESSFULLMSG), NULL, prj.GetTextColor(TID_GAME_MESSFULLMSG));
				}
				else
				{
					g_DPlay.SendChat(string);
					pWndText->Empty();
				}
			}
		}
		break;
	case WIDC_SEND:
	{
		CString str = pWndText->m_string;
		if (str.IsEmpty() == FALSE)
		{
			CString strFormat = pWndText->m_string;

			pWndText->m_string.GetTextFormat(strFormat);

			CString string;
			string.Format("/say \"%s\" %s", m_strPlayer, strFormat);

			if (string.GetLength() >= 260)
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_MESSFULLMSG), NULL, prj.GetTextColor(TID_GAME_MESSFULLMSG));
			}
			else
			{
				g_DPlay.SendChat(string);
				pWndText->Empty();
			}
		}
	}
	break;

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#ifndef __FL_SAVE_MESSAGES
void CWndMessage::AddMessage(LPCTSTR lpszFrom, LPCTSTR lpszMessage)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strMessage;

	if (!strcmp(lpszFrom, g_pPlayer->GetName()))
		strMessage.Format("#cffff0000%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);
	else
		strMessage.Format("#cff0000ff%s%s :#nc\n  %s\n", lpszFrom, prj.GetText(TID_GAME_FROM3), lpszMessage);

	pWndText->AddString(strMessage);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
}
#else
void CWndMessage::AddMessage(LPCTSTR lpszFrom, LPCTSTR lpszMessage, CTime time)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString strMessage;

	if (time == 0)
		time = CTime::GetCurrentTime();

	if (!strcmp(lpszFrom, g_pPlayer->GetName()))
		strMessage.Format("#cffff0000[%02d:%02d:%02d] %s: #nc\n  %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), lpszFrom, lpszMessage);
	else
		strMessage.Format("#cff0000ff[%02d:%02d:%02d] %s: #nc\n  %s\n", time.GetHour(), time.GetMinute(), time.GetSecond(), lpszFrom, lpszMessage);

	pWndText->AddString(strMessage);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
}
#endif