#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildName.h"
#include "WndManager.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;




CWndGuildName::CWndGuildName()
{
	m_nId = 0xff;
}
CWndGuildName::~CWndGuildName()
{
}

void CWndGuildName::OnDraw(C2DRender* p2DRender)
{
#ifdef __S_SERVER_UNIFY
	if (g_WndMng.m_bAllAction == FALSE)
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CRect rectWindow = GetWindowRect();
		CPoint point(rectRoot.right - rectWindow.Width(), 110);
		Move(point);
		MoveParentCenter();
	}
#endif // __S_SERVER_UNIFY
}
void CWndGuildName::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CGuild* pGuild = g_pPlayer->GetGuild();
	if (pGuild)
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		pWndEdit->SetString(pGuild->m_szGuild);
	}
	else
	{
		Destroy();
	}


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildName::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDNAME, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildName::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildName::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildName::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildName::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildName::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		LPCTSTR szName = pWndEdit->GetString();

		//__FIX_SQL_INJECTS
		if (IsInvalidGameName(szName))
		{
			g_WndMng.PutString(prj.GetText(TID_DIAG_0013));
			return TRUE;
		}

		if (m_nId == 0xff)
			g_DPlay.SendGuildSetName(szName);
		else
			g_DPlay.SendQuerySetGuildName(szName, m_nId);

		Destroy();
	}
	else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
	{
		if (g_WndMng.m_bAllAction == FALSE)
			return TRUE;
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndGuildNickName::CWndGuildNickName()
{
	m_idPlayer = 0;
}
CWndGuildNickName::~CWndGuildNickName()
{
}
void CWndGuildNickName::OnDraw(C2DRender* p2DRender)
{
}
void CWndGuildNickName::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();




	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndGuildNickName::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_NICKNAME, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildNickName::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildNickName::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildNickName::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildNickName::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildNickName::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

		LPCTSTR szName = pEdit->GetString();

		//__FIX_SQL_INJECTS
		if (IsInvalidGameName(szName))
		{
			g_WndMng.PutString(prj.GetText(TID_DIAG_0013));
			return TRUE;
		}

		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild)
		{
			CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);
			if (pGuildMember)
			{
				if (strcmp(pGuildMember->m_szAlias, szName))
				{
					g_DPlay.SendGuildNickName(m_idPlayer, szName);
					Destroy();
				}
			}
		}
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}