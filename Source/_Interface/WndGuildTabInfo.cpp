#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabInfo.h"
#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineQuest.h"
extern	CDPClient	g_DPlay;

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

/****************************************************
  WndId : APP_GUILD_DISMISS
****************************************************/
CWndGuildDisMiss::CWndGuildDisMiss()
{
}
CWndGuildDisMiss::~CWndGuildDisMiss()
{
}
void CWndGuildDisMiss::OnDraw(C2DRender* p2DRender)
{
}
void CWndGuildDisMiss::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();




	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildDisMiss::Initialize(CWndBase* pWndParent)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_GUILD_DISMISS);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);
	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_DISMISS);
}

BOOL CWndGuildDisMiss::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildDisMiss::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildDisMiss::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildDisMiss::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildDisMiss::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_YES:
	{
		if (g_GuildCombatMng.m_bRequest)
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_DISSOLVE_GUILD));
		else
			g_DPlay.SendDestroyGuild();
		Destroy();
	}
	break;
	case WIDC_NO:
	{
		Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



/****************************************************
  WndId : APP_GUILD_NOTICE
****************************************************/
CWndGuildNotice::CWndGuildNotice()
{
}
CWndGuildNotice::~CWndGuildNotice()
{
}
void CWndGuildNotice::OnDraw(C2DRender* p2DRender)
{
}
void CWndGuildNotice::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		CWndEdit* pNotice = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		pNotice->AddWndStyle(EBS_WANTRETURN);
		pNotice->AddWndStyle(EBS_AUTOVSCROLL);

		LPWNDCTRL pWndCtrl = GetWndCtrl(WIDC_EDIT1);
		if (pWndCtrl)
			pNotice->SetWndRect(pWndCtrl->rect);

		pNotice->SetString(pGuild->m_szNotice);
	}



	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildNotice::Initialize(CWndBase* pWndParent)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_GUILD_NOTICE);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);
	return CWndNeuz::Create( /*WBS_THICKFRAME |*/ WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_GUILD_NOTICE);
}

BOOL CWndGuildNotice::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildNotice::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildNotice::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildNotice::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildNotice::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndEdit* pNotice = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	switch (nID)
	{
	case WIDC_BUTTON2:
	{
		LPCTSTR szNotice = pNotice->GetString();

		if (strlen(szNotice) < MAX_BYTE_NOTICE)
		{
			g_DPlay.SendGuildNotice(szNotice);
			Destroy();
		}
		else
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GUILD_NOTICE_ERROR));
		}
		break;
	}
	case WIDC_BUTTON1:
		pNotice->SetString("");
		break;
	case WIDC_BUTTON3:
		pNotice->SetFocus();
		break;
	}


	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndGuildTabInfo::CWndGuildTabInfo()
{
	m_pwndGuildNotice = NULL;
	m_pwndGuildDisMiss = NULL;

}
CWndGuildTabInfo::~CWndGuildTabInfo()
{
	SAFE_DELETE(m_pwndGuildNotice);
	SAFE_DELETE(m_pwndGuildDisMiss);
}
void CWndGuildTabInfo::OnDraw(C2DRender* p2DRender)
{
#ifdef __FL_GUILD_FINDER
	BOOL bButton = FALSE;
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK1);

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		pWndButton->SetCheck(pGuild->m_bFinder);
		if (pGuild->IsMaster(g_pPlayer->m_idPlayer))
			bButton = TRUE;
	}

	pWndButton->EnableWindow(bButton);

#endif
}

void CWndGuildTabInfo::UpdateData()
{
	if (!g_pPlayer)
		return;

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		CString strText;
		CWndBase* pWndText;

		pWndText = GetDlgItem(WIDC_GUILDNAME);
		pWndText->SetTitle(pGuild->m_szGuild);
		pWndText = GetDlgItem(WIDC_GUILDLEVEL);
		strText.Format("%d", pGuild->m_nLevel);
		pWndText->SetTitle(strText);
		pWndText = GetDlgItem(WIDC_GUILDMASTER);
		LPCTSTR szMaster = CPlayerDataCenter::GetInstance()->GetPlayerString(pGuild->m_idMaster);

		pWndText->SetTitle(szMaster);

		pWndText = GetDlgItem(WIDC_GUILDNUMBER);

		strText.Format("%d / %d", pGuild->GetSize(), CGuildTable::GetInstance().GetMaxMemeber(pGuild->m_nLevel));
		pWndText->SetTitle(strText);
		pWndText = GetDlgItem(WIDC_GUILDEXPMERIT);
		strText.Format("%u", pGuild->m_dwContributionPxp);
		pWndText->SetTitle(strText);
		pWndText = GetDlgItem(WIDC_GUILDPENYAMERIT);
		strText.Format("%u", pGuild->m_nGoldGuild);
		pWndText->SetTitle(strText);

		CWndText* pNotice = (CWndText*)GetDlgItem(WIDC_TEXT1);
		if (pNotice)
			pNotice->SetString(pGuild->m_szNotice);
	}
	else
	{
		CString strText;
		CWndBase* pWndText;

		pWndText = GetDlgItem(WIDC_GUILDNAME);
		pWndText->SetTitle("");
		pWndText = GetDlgItem(WIDC_GUILDLEVEL);
		pWndText->SetTitle("");
		pWndText = GetDlgItem(WIDC_GUILDMASTER);
		pWndText->SetTitle("");
		pWndText = GetDlgItem(WIDC_GUILDNUMBER);
		pWndText->SetTitle("");
		pWndText = GetDlgItem(WIDC_GUILDEXPMERIT);
		pWndText->SetTitle("");
		pWndText = GetDlgItem(WIDC_GUILDPENYAMERIT);
		pWndText->SetTitle("");

		CWndText* pNotice = (CWndText*)GetDlgItem(WIDC_TEXT1);
		if (pNotice)
			pNotice->SetString("");
	}
}
void CWndGuildTabInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	UpdateData();


	MoveParentCenter();
}

BOOL CWndGuildTabInfo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_TABINFO, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildTabInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildTabInfo::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildTabInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildTabInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndGuildTabInfo::OnMouseMove(UINT nFlags, CPoint point)
{
}


BOOL CWndGuildTabInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CGuild* pGuild = g_pPlayer->GetGuild();

	if (pGuild == NULL)
		return FALSE;

	if (!pGuild->IsMaster(g_pPlayer->m_idPlayer))
		return FALSE;

	switch (nID)
	{
#ifdef __FL_GUILD_FINDER
	case WIDC_CHECK1: 
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		g_DPlay.SendGuildFinderState(pGuild->m_idGuild);
		break;
	}
#endif
	case WIDC_BUTTON4:
	{
		if (strlen(pGuild->m_szGuild) == 0)
		{
			if (!g_WndMng.m_pWndGuildName)
			{
				g_WndMng.m_pWndGuildName = new CWndGuildName;
				g_WndMng.m_pWndGuildName->Initialize(&g_WndMng);
			}
			g_WndMng.m_pWndGuildName->SetData();
		}
		else
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDNOTCHGNAME), MB_OK, this);

	}
	break;
	case WIDC_BUTTON1:
	{
		SAFE_DELETE(m_pwndGuildNotice);
		m_pwndGuildNotice = new CWndGuildNotice;
		m_pwndGuildNotice->Initialize(this);
	}
	break;
	case WIDC_BUTTON3:
	{
		if (g_WndMng.m_pWndGuildBank)
		{
			return FALSE;
		}
		if (pGuild->GetQuest(QUEST_WARMON_LV1) != NULL && pGuild->GetQuest(QUEST_WARMON_LV1)->nState == QS_BEGIN)
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(QUEST_WARMON_LV1);
			if (pQuestProp)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GUILD_QUEST_LEAVEERROR), MB_OK, this);
			}
			return FALSE;
		}
		else
		{
			SAFE_DELETE(m_pwndGuildDisMiss);
			m_pwndGuildDisMiss = new CWndGuildDisMiss;
			m_pwndGuildDisMiss->Initialize(this);
		}
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


void CWndGuildTabInfo::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pwndGuildDisMiss)
		SAFE_DELETE(m_pwndGuildDisMiss);
	if (pWndChild == m_pwndGuildNotice)
		SAFE_DELETE(m_pwndGuildNotice);
}