
#include "stdafx.h"
#include "resData.h"
#include "WndQuitRoom.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if __VER >= 13 // __HOUSING


CWndQuitRoom::CWndQuitRoom()
{
}
CWndQuitRoom::~CWndQuitRoom()
{
}
void CWndQuitRoom::OnDraw( C2DRender* p2DRender )
{
}
void CWndQuitRoom::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();




	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndQuitRoom::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_QUIT_ROOM, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndQuitRoom::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndQuitRoom::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndQuitRoom::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndQuitRoom::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndQuitRoom::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch(nID)
	{
		case WIDC_BUTTON1:
			g_DPlay.SendHousingGoOut();
			Destroy();
			break;

		case WIDC_BUTTON2:
			Destroy();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#endif // __HOUSING