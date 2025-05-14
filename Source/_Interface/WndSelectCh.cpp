#include "stdafx.h"
#include "resData.h"

#ifdef __AZRIA_1023
#include "defineText.h"
#include "AppDefine.h"

#include "WndSelectCh.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CWndSelectCh::CWndSelectCh(int nItemId, int nChCount)
{
	m_nItemId  = nItemId;
	m_nChCount = nChCount + 1;
}

CWndSelectCh::~CWndSelectCh()
{
}

void CWndSelectCh::OnDraw( C2DRender* p2DRender )
{
}

void CWndSelectCh::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	pWndListBox->ResetContent();
	CString strTitle;
	for(int i=0; i < m_nChCount; ++i)
	{
		strTitle.Format( "%s	%d", prj.GetText(TID_GAME_CHAR_SERVERNAME), i+1);
		pWndListBox->AddString(strTitle);
	}


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSelectCh::Initialize( CWndBase* pWndParent, DWORD  )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SELECT_CHANNEL, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndSelectCh::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSelectCh::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSelectCh::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSelectCh::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSelectCh::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	int nSelect = 0;

	switch(nID)
	{
		case WIDC_LISTBOX1: // view ctrl
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
				char strTemp[8];
				nSelect = pWndListBox->GetCurSel() * -1;
				_itoa(nSelect, strTemp, 10);
				g_DPlay.SendDoUseItem(m_nItemId, strTemp);
				Destroy();
			}
			break;
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

#endif