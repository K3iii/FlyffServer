#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabInfo.h"
#include "WndGuildTabApp.h"
#include "WndGuild.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;


#include "guild.h"
extern	CGuildMng	g_GuildMng;


CWndGuild::CWndGuild()
{
}
CWndGuild::~CWndGuild()
{
}
void CWndGuild::OnDraw( C2DRender* p2DRender )
{
}

void CWndGuild::UpdateDataAll()
{
	m_WndGuildTabInfo.UpdateData();
	m_WndGuildTabApp.UpdateData();
	m_WndGuildTabMember.UpdateData();
}

void CWndGuild::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_WndGuildTabInfo.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABINFO );
	m_WndGuildTabApp.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABAPPELLATION );
	m_WndGuildTabMember.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILD_TABMEMBER_EX );

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDINFO);
	tabTabItem.pWndBase = &m_WndGuildTabInfo;
	pWndTabCtrl->InsertItem( 0, &tabTabItem );


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_GUILDMEMBER);
	tabTabItem.pWndBase = &m_WndGuildTabMember;
	pWndTabCtrl->InsertItem( 1, &tabTabItem );

	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_APPELLATION);
	tabTabItem.pWndBase = &m_WndGuildTabApp;
	pWndTabCtrl->InsertItem( 2, &tabTabItem );


#ifdef __S_SERVER_UNIFY
	if( g_WndMng.m_bAllAction == FALSE )
	{
		Destroy();
		return;
	}
#endif // __S_SERVER_UNIFY


	MoveParentCenter();

	{
		CRect r = GetWindowRect( TRUE );
		m_WndGuildTabMember.m_rectParent = r;
	}

}

BOOL CWndGuild::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	BOOL bFlag = CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD, 0, CPoint( 0, 0 ), pWndParent );

	return bFlag;
}
BOOL CWndGuild::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuild::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuild::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndGuild::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndGuild::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndGuild::OnMouseMove(UINT nFlags, CPoint point )
{
}

#if __VER >= 15 // __GUILD_HOUSE
void CWndGuild::SetCurTab( int index )
{
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
	assert( pWndTabCtrl );

	pWndTabCtrl->SetCurSel( index );
}
#endif

CWndGuildConfirm::CWndGuildConfirm()
{
	m_idMaster = 0;
}
CWndGuildConfirm::~CWndGuildConfirm()
{
}
void CWndGuildConfirm::SetGuildName( char* pGuildName )
{
	strGuildName.Format( "%s", pGuildName );
}
void CWndGuildConfirm::OnDraw( C2DRender* p2DRender )
{
}
void CWndGuildConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem( WIDC_STATIC3 );
	pEdit->SetTitle( strGuildName );
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );
}

BOOL CWndGuildConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_INVATE, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndGuildConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuildConfirm::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuildConfirm::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndGuildConfirm::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndGuildConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		if( g_pPlayer )
			g_DPlay.SendAddGuildMember( m_idMaster );

		Destroy();
	}
	else if( nID == WIDC_NO )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

