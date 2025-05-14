#include "stdafx.h"
#include "AppDefine.h"
#include "WndExtra.h"

#if 0
CWndExtraCapture::CWndExtraCapture()
{
}
CWndExtraCapture::~CWndExtraCapture()
{
}
void CWndExtraCapture::OnDraw( C2DRender* p2DRender )
{
}
void CWndExtraCapture::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndExtraCapture::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_EXTRA_CAPTURE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndExtraCapture::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndExtraCapture::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndExtraCapture::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndExtraCapture::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndExtraCapture::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#endif // if 0