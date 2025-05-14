#include "stdafx.h"
#include "resData.h"
#include "WndBlessingCancel.h"
#include "DPClient.h"
#include "defineText.h"
#include "randomoption.h"


#if __VER >= 11 // __SYS_IDENTIFY
extern CDPClient g_DPlay;



CWndBlessingCancel::CWndBlessingCancel()
{

	m_pItemElem  = NULL;
	m_pEItemProp = NULL;
	m_pText		 = NULL;
	m_pTexture	 = NULL;

}
CWndBlessingCancel::~CWndBlessingCancel()
{
}
void CWndBlessingCancel::OnDraw( C2DRender* p2DRender )
{

	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );

	if(m_pTexture != NULL)
		m_pTexture->Render( p2DRender, CPoint( wndCtrl->rect.left, wndCtrl->rect.top ) );

}
void CWndBlessingCancel::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem( WIDC_DESC );

	SetDescription();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndBlessingCancel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_CANCEL_BLESSING, 0, CPoint( 0, 0 ), pWndParent );
}

void CWndBlessingCancel::OnDestroy()
{
	if(m_pItemElem != NULL)
	{
		if( !g_pPlayer->m_vtInfo.IsTrading( m_pItemElem ) )
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndBlessingCancel::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndBlessingCancel::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndBlessingCancel::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndBlessingCancel::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndBlessingCancel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{

	if( nID == WIDC_START )
	{

		if(m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem( WIDC_START );
			pButton->EnableWindow(FALSE);


			if(m_pItemElem)
			{
				g_DPlay.SendBlessednessCancel(m_pItemElem->m_dwObjId);
				Destroy();
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void  CWndBlessingCancel::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if(!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );
	rect = wndCtrl->rect;
	if( rect.PtInRect( point ) )
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture	= NULL;
	}

}

BOOL  CWndBlessingCancel::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CItemElem* pTempElem  = g_pPlayer->m_Inventory.GetAtId( pShortcut->m_dwId );
	if (!IsUsableItem(pTempElem))
		return FALSE;

    if( g_xRandomOptionProperty->GetRandomOptionKind( pTempElem ) == CRandomOptionProperty::eBlessing
		&& g_xRandomOptionProperty->GetRandomOptionSize( pTempElem->GetRandomOptItemId() ))
	{

		if(pTempElem != NULL)
		{
			if(m_pItemElem) m_pItemElem->SetExtra(0);
			m_pItemElem = pTempElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pItemElem->SetExtra(m_pItemElem->GetExtra()+1);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);

			LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_CHANGE );
			if(m_pEItemProp != NULL)
				m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff );
		}
	}
	else
	{
		g_WndMng.PutString( prj.GetText(TID_GAME_BLESSEDNESS_CANCEL), NULL, 0xffff0000 );
		return FALSE;
	}

	return TRUE;

}

void  CWndBlessingCancel::SetDescription()
{

	CScript scanner;
	BOOL checkflag;
	CHAR* szChar;


	checkflag = scanner.Load( MakePath( DIR_CLIENT,  _T( "ItemBlessingCancel.inc" ) ));
	szChar = scanner.m_pProg;

	if(m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString( szChar );
		m_pText->ResetString();
	}

}
#endif

