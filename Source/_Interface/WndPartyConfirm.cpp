#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndPartyConfirm.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;


/****************************************************
  WndId : APP_PARTY_CONFIRM - party confirm
  CtrlId : WIDC_YES - yes
  CtrlId : WIDC_NO - No
  CtrlId : WIDC_STATIC1 - Static
****************************************************/

CWndPartyConfirm::CWndPartyConfirm()
{
	m_uLeader = m_uMember = 0;
	m_szLeaderName[ 0 ] = '\0';
	m_szMemberName[ 0 ] = '\0';
	bTroup = FALSE;
}
CWndPartyConfirm::~CWndPartyConfirm()
{
}

void CWndPartyConfirm::SetMember( u_long uLeader, u_long uMember, char * szLName, BOOL bTroupbuf )
{
	m_uLeader = uLeader;
	m_uMember = uMember;
	strcpy( m_szLeaderName, szLName );
	bTroup = bTroupbuf;
}

void CWndPartyConfirm::OnDraw( C2DRender* p2DRender )
{
}
void CWndPartyConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CString strTemp;
	strTemp.Format(_T(prj.GetText(TID_DIAG_0072)),m_szLeaderName);

	pWndText->SetString( strTemp );

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
}

BOOL CWndPartyConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_PARTY_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndPartyConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndPartyConfirm::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndPartyConfirm::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndPartyConfirm::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndPartyConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_YES )
	{

		if( bTroup == FALSE )
		{
			g_DPlay.SendAddPartyMember( m_uLeader );
		}
		Destroy();
	}
	else if(nID==WIDC_NO || nID==WTBID_CLOSE)
	{

		g_DPlay.SendPartyMemberCancle( m_uLeader, m_uMember );
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

