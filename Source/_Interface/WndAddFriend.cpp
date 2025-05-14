#include "stdafx.h"
#include "WndAddFriend.h"




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


	CWndEdit* pWndName = (CWndEdit*) GetDlgItem( WIDC_EDIT1 );
	pWndName->SetFocus();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndAddFriend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADD_FRIEND, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndAddFriend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch( nID )
	{
	case WIDC_OK:
		break;
	case WIDC_CANCEL:
		Destroy();
		break;
	}
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
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

