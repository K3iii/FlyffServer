#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPartyChangeTroup.h"
#include "WndManager.h"


#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "party.h"
extern	CParty g_Party;


CWndPartyChangeName::CWndPartyChangeName()
{
}
CWndPartyChangeName::~CWndPartyChangeName()
{
}
void CWndPartyChangeName::OnDraw(C2DRender* p2DRender)
{
}
void CWndPartyChangeName::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	if (0 == strlen(g_pPlayer->m_szPartyName))
	{
		strcpy(m_sParty, prj.GetText(TID_GAME_PARTY_NAME));
	}
	else
	{
		strcpy(m_sParty, g_pPlayer->m_szPartyName);
	}



	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	pEdit->SetString(m_sParty);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}


BOOL CWndPartyChangeName::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTYCHANGENAME, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndPartyChangeName::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPartyChangeName::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPartyChangeName::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPartyChangeName::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPartyChangeName::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
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

		strcpy(m_sParty, szName);
		if (0 != strcmp(m_sParty, prj.GetText(TID_GAME_PARTY_NAME)))
		{
			g_DPlay.SendChangeTroup(TRUE, m_sParty);
			Destroy();
		}

	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

//--------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------


CWndPartyLeaveConfirm::CWndPartyLeaveConfirm()
{
}
CWndPartyLeaveConfirm::~CWndPartyLeaveConfirm()
{
}
void CWndPartyLeaveConfirm::SetLeaveId(u_long uidPlayer)
{
	uLeaveId = uidPlayer;

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (g_pPlayer->m_idPlayer == uLeaveId)
	{

		pWndEdit->SetString(prj.GetText(TID_DIAG_0084));
	}
	else
	{

		pWndEdit->SetString(prj.GetText(TID_DIAG_0085));
	}
	pWndEdit->EnableWindow(FALSE);
}
void CWndPartyLeaveConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndPartyLeaveConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();



	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndPartyLeaveConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTYLEAVE_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPartyLeaveConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPartyLeaveConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPartyLeaveConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPartyLeaveConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPartyLeaveConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_NO || nID == WTBID_CLOSE)
	{
		Destroy();
	}
	else if (nID == WIDC_YES)
	{
		g_DPlay.SendRemovePartyMember(uLeaveId);
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
