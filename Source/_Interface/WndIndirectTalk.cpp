#include "stdafx.h"
#include "AppDefine.h"
#include "WndIndirectTalk.h"



CWndIndirectTalk::CWndIndirectTalk()
{
}
CWndIndirectTalk::~CWndIndirectTalk()
{
}
void CWndIndirectTalk::OnDraw( C2DRender* p2DRender )
{
	CWorld* pWorld = g_WorldMng();
	CMover* pMover = (CMover*)pWorld->GetObjFocus();
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	TCHAR szNum[ 32 ];
	if( pMover && pMover->GetType() == OT_MOVER )
		itoa( pMover->GetId(), szNum, 10 );
	else
		itoa( 0, szNum, 10 );
	pWndEdit->SetString( szNum );
}
void CWndIndirectTalk::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT2);
	pWndEdit->AddWndStyle( EBS_AUTOVSCROLL );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndIndirectTalk::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_ADMIN_INDIRECT_TALK, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndIndirectTalk::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndIndirectTalk::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndIndirectTalk::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndIndirectTalk::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndIndirectTalk::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CWorld* pWorld = g_WorldMng();
	CObj* pObj = pWorld->GetObjFocus();
	if( pObj && pObj->GetType() == OT_MOVER )
	{
		switch( nID )
		{
		case WIDC_EDIT2:
			if( message != EN_RETURN )
				break;
		case WIDC_BUTTON1:
			{
				CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
				CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
				LPCTSTR lpId = pWndEdit1->m_string;
				LPCTSTR lpText = pWndEdit2->m_string;
				CString string;
				string.Format( "/id %s %s", lpId, lpText );
				ParsingCommand( string.LockBuffer(), g_pPlayer );
				string.UnlockBuffer();
				pWndEdit2->Empty();
			}
			break;
		}
	}
	if( nID == WTBID_CLOSE )
	{
		Destroy();
		return TRUE;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

