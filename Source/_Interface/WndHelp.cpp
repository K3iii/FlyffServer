#include "stdafx.h"
#include "AppDefine.h"
#include "WndHelp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CWndHelp::CWndHelp()
{
}
CWndHelp::~CWndHelp()
{
}
void CWndHelp::OnDraw(C2DRender* p2DRender)
{
	DWORD dwLeft = m_rectClient.Width() * 50 / 100;
	CRect rect;

	rect.SetRect( dwLeft + 5, 5, m_rectClient.Width() - 5, 23 );

//	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150), D3DCOLOR_ARGB(255,150,150,200), D3DCOLOR_ARGB(255,100,100,150));
	//p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	p2DRender->TextOut( rect.left + 10, rect.top + 8, m_strKeyword, 0xff000000 );
	/*

	rect.SetRect( dwLeft + 5, 25, m_rectClient.Width() - 5, m_rectClient.Height() - 5 );

	p2DRender->RenderFillRect ( rect, D3DCOLOR_ARGB(255,170,170,230) );
	p2DRender->RenderRoundRect( rect, D3DCOLOR_ARGB(255,200,200,250) );

	CStringArray* pStrArray = prj.GetHelp(m_strKeyword);
	if(pStrArray)
	{
		m_strArray.RemoveAll();
		ClipStrArray(&g_Neuz.m_2DRender,rect,0,pStrArray,&m_strArray);
		CString string;
		for(int i = 0; i < m_strArray.GetSize(); i++)
		{
			string = m_strArray.GetAt(i);
			p2DRender->TextOut(rect.left + 5, rect.top + 5 + i * 16, string);
		}
	}
	*/
}
void CWndHelp::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwRight = m_rectClient.Width() * 50 / 100;
	//m_wndViewCtrl.Create( WBS_CHILD, CRect( 5, 5, dwRight,m_rectClient.Height() - 5 ), this, 1005 );//,m_pSprPack,-1);//m_pSprPack,16);

	CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREE1 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->AddWndStyle( WBS_VSCROLL );
	pWndTreeCtrl->LoadTreeScript( MakePath( DIR_CLIENT,  _T( "treeHelp.inc" ) ) );
	CRect rect( 210, 0, 300, 100 );
	//m_wndText.AddWndStyle( WBS_VSCROLL );
	//m_wndText.Create( 0, rect, this, 10 );
	//rect = m_wndText.GetClientRect();


	//m_wndText.m_string.Reset( m_pFont, &rect );
	MoveParentCenter();

	//SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "WndHelp.bmp" ), TRUE );
	//FitTextureSize();
}
BOOL CWndHelp::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect(400,350);
//etTitle( GETTEXT( TID_APP_HELPER_HELP ) );
	//return CWndNeuz::Create(0|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_HELP, 0, CPoint( 0, 0 ), pWndParent );

}

BOOL CWndHelp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndHelp::OnSize(UINT nType, int cx, int cy)
{
	DWORD dwRight = m_rectClient.Width() * 50 / 100;

	CRect rect;
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelp::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

}
void CWndHelp::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;
}
BOOL CWndHelp::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case WIDC_TREE1: // view ctrl
			{
				LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
				if(lpTreeElem)
				{
					//m_strArray.RemoveAll();
					m_strKeyword = lpTreeElem->m_strKeyword;

					CString string = prj.GetHelp( m_strKeyword );

					CWndTreeCtrl* pWndTreeCtrl = (CWndTreeCtrl*)GetDlgItem( WIDC_TREE1 );
					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
					pWndText->SetString( string );
					pWndText->UpdateScrollBar();
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}

CWndHelpFAQ::CWndHelpFAQ()
{
}
CWndHelpFAQ::~CWndHelpFAQ()
{
}
void CWndHelpFAQ::OnDraw(C2DRender* p2DRender)
{

}
void CWndHelpFAQ::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	DWORD dwRight = m_rectClient.Width() * 50 / 100;

	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CRect rect( 210, 0, 300, 100 );

	LoadFAQ( MakePath( DIR_CLIENT,  _T( "Faq.inc" ) ) );

	MoveParentCenter();
}
BOOL CWndHelpFAQ::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CRect rect = m_pWndRoot->MakeCenterRect(400,350);
//etTitle( GETTEXT( TID_APP_HELPER_HELP ) );
	//return CWndNeuz::Create(0|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME,rect,pWndParent,dwWndId);
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_FAQ, 0, CPoint( 0, 0 ), pWndParent );

}

BOOL CWndHelpFAQ::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand(nID,dwMessage,pWndBase);
}
void CWndHelpFAQ::OnSize(UINT nType, int cx, int cy)
{
	DWORD dwRight = m_rectClient.Width() * 50 / 100;

	CRect rect;

	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelpFAQ::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(IsWndRoot())
		return;

}
void CWndHelpFAQ::OnLButtonDown(UINT nFlags, CPoint point)
{
//	CWndBase::OnLButtonDown(nFlags,point
	if(IsWndRoot())
		return;
//	return;
}
BOOL CWndHelpFAQ::LoadFAQ( LPCTSTR lpszFileName )
{
	CScript s;
	if( s.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString strKeyword;
	s.GetToken();	// keyword
	while( s.tok != FINISHED )
	{
		strKeyword	= s.token;
		s.GetToken();	// {
		s.GetToken();
		CString string = "    " + s.Token;
		m_mapFAQ.SetAt( strKeyword, string );
		CWndListBox* pWndListBox	= (CWndListBox*)GetDlgItem( WIDC_LIST1 );
		CString strFAQ;
		strFAQ = "Q.";
		strFAQ	+= strKeyword;
		pWndListBox->AddString( strFAQ );
		s.GetToken();	// }
		s.GetToken();
	}
	return TRUE;
}

BOOL CWndHelpFAQ::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	switch(nID)
	{
		case WIDC_LIST1: // view ctrl
			{
				CString strKey, string;
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem( WIDC_LIST1 );
				pWndListBox->GetText( pWndListBox->GetCurSel(), strKey );
				{
					strKey = strKey.Right( strKey.GetLength() - 2 );
					m_mapFAQ.Lookup( strKey, string );
					m_strKeyword = strKey;

					CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
					pWndText->m_string.Init( m_pFont, &pWndText->GetClientRect() );
					pWndText->m_string.SetString("");
					pWndText->m_string.AddString(m_strKeyword, 0xff8080ff, ESSTY_BOLD);
					pWndText->m_string.AddString("\n\n");
					pWndText->m_string.AddString( string );
					pWndText->UpdateScrollBar();
				}
			}
			break;
	}
	return CWndNeuz::OnChildNotify(message,nID,pLResult);
}

CWndHelpTip::CWndHelpTip()
{
}
CWndHelpTip::~CWndHelpTip()
{
}
void CWndHelpTip::OnDraw( C2DRender* p2DRender )
{
}
void CWndHelpTip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
	pWndText->AddWndStyle( WBS_NOFRAME );
	pWndText->AddWndStyle( WBS_NODRAWFRAME );
	CScript s;
	if( s.Load( MakePath( DIR_CLIENT, _T("tip.inc" ) ) ) == FALSE )
		return;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		CString string = "    " + s.Token;
		m_aString.Add( string );
		s.GetToken();
	}
	m_nPosString = 0;
	pWndText->SetString( m_aString.GetAt( 0 ), 0xff000000 );
}
BOOL CWndHelpTip::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	InitDialog( g_Neuz.GetSafeHwnd(), APP_HELPER_TIP );
	MoveParentCenter();
	return TRUE;
}
BOOL CWndHelpTip::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	switch( nID )
	{
	case WIDC_NEXT:
		{
			m_nPosString++;
			if( m_nPosString == m_aString.GetSize() )
				m_nPosString = 0;
			CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT );
			pWndText->SetString( m_aString.GetAt( m_nPosString ), 0xff000000 );
		}
		break;
	case WIDC_CLOSE:
		Destroy();
	}
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndHelpTip::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelpTip::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndHelpTip::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndHelpTip::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_CHECK1:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

CWndHelpInstant::CWndHelpInstant()
{
}
CWndHelpInstant::~CWndHelpInstant()
{
}
void CWndHelpInstant::OnDraw( C2DRender* p2DRender )
{
}
void CWndHelpInstant::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();



	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	pWndText->SetString( prj.GetHelp( m_strHelpKey ) );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndHelpInstant::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_HELP_INSTANT, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndHelpInstant::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndHelpInstant::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndHelpInstant::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndHelpInstant::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndHelpInstant::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == WIDC_CLOSE )
	{
#ifdef __HELP_BUG_FIX
		Destroy();
#else //__HELP_BUG_FIX

		vector<CString>::iterator where = find(g_vecHelpInsKey.begin(), g_vecHelpInsKey.end(), m_strHelpKey );

		if(where != g_vecHelpInsKey.end())
			g_vecHelpInsKey.erase(where);

		Destroy( TRUE );
#endif //__HELP_BUG_FIX
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

