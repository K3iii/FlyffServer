#include "stdafx.h"
#include "resData.h"
#include "WndInvenRemoveItem.h"
#include "defineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;




CWndInvenRemoveItem::CWndInvenRemoveItem()
{
	m_pWndItemCtrl	= NULL;
	m_pWndEditNum		= NULL;
	m_pItemElem		= NULL;
	m_nRemoveNum = 0;
}
CWndInvenRemoveItem::~CWndInvenRemoveItem()
{
}
void CWndInvenRemoveItem::InitItem( CItemElem* pItemElem )
{
	m_pItemElem = pItemElem;
	int nItemNum = pItemElem->m_nItemNum;

	char szItemNum[10] = {0,};
	_itoa( pItemElem->m_nItemNum , szItemNum, 10 );
	m_pWndEditNum->SetString( szItemNum );

	if( nItemNum <= 1 )
		m_pWndEditNum->EnableWindow( FALSE );
	else
		m_pWndEditNum->SetFocus();

	m_nRemoveNum = 0;
}
void CWndInvenRemoveItem::OnDraw( C2DRender* p2DRender )
{
	if( m_pItemElem )
		m_pItemElem->GetTexture()->Render( p2DRender, m_pWndItemCtrl->rect.TopLeft(), 255 );
}
void CWndInvenRemoveItem::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndItemCtrl = GetWndCtrl( WIDC_CUSTOM1 );
	m_pWndEditNum = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndInvenRemoveItem::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_INVEN_REMOVE_ITEM, WBS_MODAL, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndInvenRemoveItem::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndInvenRemoveItem::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndInvenRemoveItem::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndInvenRemoveItem::OnLButtonDown( UINT nFlags, CPoint point )
{
}
void CWndInvenRemoveItem::OnMouseWndSurface( CPoint point )
{
	if( m_pItemElem && m_pWndItemCtrl->rect.PtInRect( point ) )
	{
		CPoint point2 = point;
		CRect  DrawRect = m_pWndItemCtrl->rect;
		ClientToScreen( &point2 );
		ClientToScreen( &DrawRect );
		g_WndMng.PutToolTip_Item( m_pItemElem, point2, &DrawRect, APP_INVEN_REMOVE_ITEM );
	}
}
BOOL CWndInvenRemoveItem::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_CANCEL:
		Destroy();
		break;

	case WIDC_OK:
		{
			if( OnButtonOK() )
			{
				// 100304_mirchang vendor item check
				for( int i = 0; i < MAX_VENDITEM; i++ )
				{
					if( g_Neuz.m_aSavedInven[i].m_dwObjId == m_pItemElem->m_dwObjId && g_Neuz.m_aSavedInven[i].m_dwItemId == m_pItemElem->m_dwItemId )
					{
						g_Neuz.m_aSavedInven[i].m_dwObjId = 0;
						g_Neuz.m_aSavedInven[i].m_nCost = 0;
						g_Neuz.m_aSavedInven[i].m_nExtra = 0;
						g_Neuz.m_aSavedInven[i].m_dwItemId = 0;
						break;
					}
				}
				g_DPlay.SendRemoveItem( m_pItemElem, m_nRemoveNum );
				Destroy();
			}
			else
			{
				g_WndMng.PutString( prj.GetText(TID_ERROR_NOT_ITEM_NUM), NULL, prj.GetTextColor(TID_ERROR_NOT_ITEM_NUM) );
			}
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndInvenRemoveItem::OnButtonOK( void )
{
	LPCTSTR szNumber = m_pWndEditNum->GetString();
	BOOL bOK = TRUE;


	int nlen = strlen(szNumber);
	for( int i = 0 ; i < nlen ; i++ )
	{
		if( (isdigit2( szNumber[i] ) == FALSE) )
		{
			bOK = FALSE;
			break;
		}
	}


	if( bOK )
	{
		int nNum = atoi( m_pWndEditNum->GetString() );
		if( 0 < nNum && nNum <= m_pItemElem->m_nItemNum )
			m_nRemoveNum = nNum;
		else
			bOK = FALSE;
	}

	if( bOK == FALSE )
	{
		char szItemNum[10] = {0,};
		_itoa( m_pItemElem->m_nItemNum , szItemNum, 10 );
		m_pWndEditNum->SetString( szItemNum );
	}

	return bOK;
}

