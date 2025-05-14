#include "stdafx.h"
#include "AppDefine.h"
#include "WndCommand.h"
#include "FuncTextCmd.h"


CWndCommand::CWndCommand()
{
	m_pWndEdit = NULL;
}
CWndCommand::~CWndCommand()
{
}
void CWndCommand::OnDraw( C2DRender* p2DRender )
{
	CWndListBox::OnDraw( p2DRender );

	if( ::GetLanguage() == LANG_KOR )
	{
		CPoint pt(3,3);
		int nFontHeight = GetFontHeight() + 3;
		pt.y -= ( nFontHeight ) * m_wndScrollBar.GetScrollPos();
		LPLISTITEM pListItem;
		CRect rectHittest;
		CPoint point;
		CRect rect = GetWindowRect();
		CString string;
		for(int i = 0; i < m_listItemArray.GetSize(); i++)
		{
			pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
			point = GetMousePoint();
			rectHittest.SetRect( 0, pt.y - 3 , 0 + rect.Width(), pt.y + nFontHeight - 3 );
			if( rectHittest.PtInRect( point ) )
			{
				TextCmdFunc* pTextCmdFunc = (TextCmdFunc*)pListItem->m_dwData;
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &rectHittest );
				string += pTextCmdFunc->m_pszDesc;
				g_toolTip.PutToolTip( i + 1000, string, rectHittest, point2, 3 );
				break;
			}
			pt.y += nFontHeight;
		}
	}
}
void CWndCommand::OnInitialUpdate()
{
	CWndListBox::OnInitialUpdate();

	int nCount = 0, nIndex, nNum = 0;
	while( m_textCmdFunc[ nCount ].m_pFunc )
	{
		TextCmdFunc* pTextCmdFunc = &m_textCmdFunc[nCount];
		if( pTextCmdFunc->m_dwAuthorization <= g_pPlayer->m_dwAuthorization )
		{
			nIndex = AddString( &(*pTextCmdFunc->m_pCommand) );
			SetItemDataPtr( nIndex, pTextCmdFunc );
			nNum++;
		}
		nCount++;
	}
	SortListBox();

	if( nNum > 20 ) nNum = 20;
	if( nNum < 5 ) nNum = 5;
	SetWndRect( CRect( 0, 0, 160, nNum * ( GetFontHeight() + 3 ) + 8 ) );

	m_pWndEdit->SetFocus();
}


BOOL CWndCommand::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect( 0, 0, 150, 200 );
	SetTitle( _T( "World" ) );

	CWndBase::Create( WBS_TOPMOST | WBS_VSCROLL, rect, pWndParent, APP_COMMAND );
	AdjustWndBase();
	RestoreDeviceObjects();

	return TRUE;
}

BOOL CWndCommand::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndListBox::OnCommand( nID, dwMessage, pWndBase );
}
void CWndCommand::OnSize( UINT nType, int cx, int cy ) \
{
	CWndListBox::OnSize( nType, cx, cy );
}
void CWndCommand::OnLButtonUp( UINT nFlags, CPoint point )
{
//	m_pWndEdit->SetFocus();
	CWndListBox::OnLButtonUp( nFlags, point );
	m_pParentWnd->SetFocus();
}
void CWndCommand::OnLButtonDown( UINT nFlags, CPoint point )
{
	CWndListBox::OnLButtonDown( nFlags, point );
}
void CWndCommand::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	CWndListBox::OnLButtonDblClk( nFlags, point );
	m_pParentWnd->SetFocus();
}
BOOL CWndCommand::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	return CWndListBox::OnChildNotify( message, nID, pLResult );
}
void CWndCommand::OnSetFocus( CWndBase* pOldWnd )
{
	//m_pWndEdit-etFocus();
}
void CWndCommand::OnKillFocus(CWndBase* pNewWnd)
{
	if( pNewWnd != m_pParentWnd )
		Destroy();
	//m_pWndEdit->SetFocus();
}
