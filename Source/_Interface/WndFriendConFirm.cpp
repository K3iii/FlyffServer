#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndFriendConFirm.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"

/****************************************************
  WndId : APP_FRIEND_CONFIRM - Confirm
****************************************************/

CWndFriendConFirm::CWndFriendConFirm()
{
	m_uLeader = m_uMember = 0;
	m_szLeaderName[ 0 ] = '\0';
}
CWndFriendConFirm::~CWndFriendConFirm()
{
}

void CWndFriendConFirm::SetMember( u_long uLeader, u_long uMember, LONG nLeaderJob, BYTE nLeaderSex, char * szLeadName )
{
	m_uLeader = uLeader;
	m_uMember = uMember;
	strcpy( m_szLeaderName, szLeadName );
}
void CWndFriendConFirm::OnDraw( C2DRender* p2DRender )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format(_T( prj.GetText(TID_DIAG_0071) ),m_szLeaderName);

	pWndText->SetString( strTemp );
}
void CWndFriendConFirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
}

BOOL CWndFriendConFirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_FRIEND_CONFIRM, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndFriendConFirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndFriendConFirm::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndFriendConFirm::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndFriendConFirm::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndFriendConFirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_YES )
	{

		g_DPlay.SendAddFriend( m_uLeader);
		Destroy();
	}
	else
	if( nID == WIDC_NO )
	{

		g_DPlay.SendFriendCancel( m_uLeader );
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}



CWndAddFriend::CWndAddFriend()
{
}
CWndAddFriend::~CWndAddFriend()
{
}
void CWndAddFriend::OnDraw( C2DRender* p2DRender )
{
}
void CWndAddFriend::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	pWndEdit->SetFocus();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndAddFriend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADDFRIEND, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndAddFriend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndAddFriend::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndAddFriend::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndAddFriend::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndAddFriend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_OK )
	{
		LPCTSTR szAddName;
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		szAddName = pWndEdit->GetString();
		if( strlen( szAddName ) < MAX_NAME )
		{
			if( strcmp( szAddName, g_pPlayer->GetName() ) != 0 )
			{
				if( g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->GetID() == WI_WORLD_GUILDWAR )
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_GUILDCOMBAT_CANNOT_FRIENDADD ) );
				}
				else
				{
					g_DPlay.SendAddFriendNameReqest( szAddName );

					CString str;
					str.Format( prj.GetText(TID_GAME_MSGINVATE), szAddName );
					g_WndMng.PutString( str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE) );
				}
				Destroy( );
			}
			else
			{
				pWndEdit->SetString("");
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0056) ) );

			}
		}
		else
		{
			pWndEdit->SetString("");
			g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0057) ) );

		}
	}
	else
	if( nID == WIDC_CANCEL )
	{
		Destroy( );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
