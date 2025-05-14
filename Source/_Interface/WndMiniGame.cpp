#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndMiniGame.h"
#include "MiniGame.h"
#include "defineSound.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#ifdef __EVE_MINIGAME

/*************************
	CWndKawiBawiBoGame
*************************/
CWndKawiBawiBoGame::CWndKawiBawiBoGame()
{
	m_nWinningCount = 0;
	m_nWinningMaxCount = 0;
	m_nCount = 0;
	m_nDelay = 1;
	m_nMyChoice = KAWI;
	m_nComChoice = KAWI;
	m_strChoice[0].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_KAWI ));
	m_strChoice[1].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BAWI ));
	m_strChoice[2].Format("        %s", prj.GetText( TID_GAME_KAWIBAWIBO_BO ));

	m_pWndGameWin = NULL;
	m_nStatus = 0;
	m_nResult = -1;
	m_nPrevResult = -1;
	m_bCheckCoupon = TRUE;

	m_dwRItemId = -1;
	m_dwRNextItemId = -1;
	m_nItemCount = -1;
	m_nNextItemCount = -1;
}

CWndKawiBawiBoGame::~CWndKawiBawiBoGame()
{
	if(m_pWndGameWin != NULL)
		SAFE_DELETE(m_pWndGameWin);
}

void CWndKawiBawiBoGame::OnDestroy()
{
}

BOOL CWndKawiBawiBoGame::Process()
{
	int nCom = -1;

	if(m_nStatus == 1)
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult > -1)
			{
				if(m_nDelay < 20)
				{
					PLAYSND( "InfOpen.wav" );
					m_nDelay += 1;
				}
				else
				{
					PLAYSND( "InfOpen.wav" );
					nCom = prj.m_MiniGame.GetKawibawiboYou(m_nMyChoice, m_nResult);
				}
			}

			m_nComChoice++;
			( m_nComChoice > 2 ) ? m_nComChoice = 0 : m_nComChoice;
			m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );

			if(m_nComChoice == nCom)
			{
				m_nStatus = 2;
				m_nDelay = 40;
			}

			m_nCount = 0;
		}
		m_nCount++;
	}
	else if(m_nStatus == 2)
	{
		if(m_nCount > m_nDelay)
		{
			if(m_nResult == CMiniGame::KAWIBAWIBO_WIN)
			{
				PlayMusic( BGM_IN_FITUP );
				CreateWinningWnd();
			}
			else if(m_nResult == CMiniGame::KAWIBAWIBO_DRAW)
			{
				EnableAllBtn();
			}
			else if(m_nResult == CMiniGame::KAWIBAWIBO_LOST)
			{
				g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_DEFEAT ) );
				m_nWinningCount = 0;
				EnableAllBtn();
				RefreshInfo();
			}

			m_nStatus = 0;
			m_nDelay = 1;
			m_nResult = -1;
		}
		m_nCount++;
	}

	return TRUE;
}

void CWndKawiBawiBoGame::OnDraw( C2DRender* p2DRender )
{
	CTexture* pTexture;
	CString strMyPath, strComPath;

	for(int i=0; i<2; i++)
	{
		int nChoice;
		CString* pstr;
		if(i == 0)
		{
			nChoice = m_nMyChoice;
			pstr = &strMyPath;
			switch(nChoice)
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi1.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi1.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo1.dds");
				break;
			}
		}
		else if(i == 1)
		{
			nChoice = m_nComChoice;
			pstr = &strComPath;
			switch(nChoice)
			{
			case KAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Kawi2.dds");
				break;
			case BAWI:
				*pstr = MakePath( DIR_ICON, "Icon_Bawi2.dds");
				break;
			case BO:
				*pstr = MakePath( DIR_ICON, "Icon_Bo2.dds");
				break;
			}
		}
	}

	if(strMyPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strMyPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_MY )->rect.left + 3, GetWndCtrl( WIDC_STATIC_MY )->rect.top + 8 ) );
	}

	if(strComPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, strComPath, 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( GetWndCtrl( WIDC_STATIC_COM )->rect.left + 3, GetWndCtrl( WIDC_STATIC_COM )->rect.top + 8 ) );
	}
}

void CWndKawiBawiBoGame::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pStWinningCount = (CWndStatic*)GetDlgItem( WIDC_WINNING_COUNT );
	m_pStMyChoice = (CWndStatic*)GetDlgItem( WIDC_MY_CHOICE );
	m_pStComChoice = (CWndStatic*)GetDlgItem( WIDC_COM_CHOICE );

	RefreshInfo();

	MoveParentCenter();
}

BOOL CWndKawiBawiBoGame::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_KAWIBAWIBO, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndKawiBawiBoGame::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndKawiBawiBoGame::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndKawiBawiBoGame::OnLButtonUp( UINT nFlags, CPoint point )
{
}

void CWndKawiBawiBoGame::OnLButtonDown( UINT nFlags, CPoint point )
{
}

BOOL CWndKawiBawiBoGame::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if(message == WNM_CLICKED)
	{
		switch(nID)
		{
			case WIDC_BTN_LEFT:
				m_nMyChoice--;
				( m_nMyChoice < 0 ) ? m_nMyChoice = 2 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_BTN_RIGHT:
				m_nMyChoice++;
				( m_nMyChoice > 2 ) ? m_nMyChoice = 0 : m_nMyChoice;
				RefreshInfo();
				break;
			case WIDC_BTN_START:
				if(m_bCheckCoupon && g_pPlayer->IsPlayer() &&
					g_pPlayer->m_Inventory.GetAtItemId( II_SYS_SYS_EVE_KAWIBAWIBO ) == NULL)
				{
					g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID1 ) );
					Destroy();
				}

				DisableAllBtn();
				m_nStatus = 1;
				g_DPlay.SendKawibawiboStart();
				break;
			case WTBID_CLOSE:
				if( m_nWinningCount > 0 || m_nStatus != 0 || m_nPrevResult == CMiniGame::KAWIBAWIBO_DRAW )
					return FALSE;
				break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndKawiBawiBoGame::ReceiveResult(int nResult, int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId)
{
	if(nResult == CMiniGame::KAWIBAWIBO_FAILED)
	{
		g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID1 ) );
		Destroy();
	}
	else
	{
		if(nResult == CMiniGame::KAWIBAWIBO_DRAW)
			m_bCheckCoupon = FALSE;
		else if(nResult == CMiniGame::KAWIBAWIBO_LOST || nResult == CMiniGame::KAWIBAWIBO_WIN)
			m_bCheckCoupon = TRUE;

		m_nPrevResult = m_nResult = nResult;
		m_nItemCount = nItemCount;
		m_nNextItemCount = nNextItemCount;
		m_dwRItemId = dwItemId;
		m_dwRNextItemId = dwNextItemId;
	}
}

void CWndKawiBawiBoGame::CreateWinningWnd()
{
	m_nWinningCount++;
	RefreshInfo();

	SAFE_DELETE(m_pWndGameWin);
	m_pWndGameWin = new CWndKawiBawiBoGameWin;
	m_pWndGameWin->Initialize(this);
	m_pWndGameWin->SetInfo(m_nItemCount, m_nNextItemCount, m_dwRItemId, m_dwRNextItemId, m_nWinningCount, m_nWinningMaxCount);
}

void CWndKawiBawiBoGame::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndGameWin)
		SAFE_DELETE(m_pWndGameWin);
}

void CWndKawiBawiBoGame::RefreshInfo()
{
	CString temp;
	temp.Format(prj.GetText(TID_GAME_KWAIBAWIBO_WINCOUNT), m_nWinningCount);
	m_pStWinningCount->SetTitle( temp );
	m_pStMyChoice->SetTitle( m_strChoice[m_nMyChoice] );
	m_pStComChoice->SetTitle( m_strChoice[m_nComChoice] );
}

void CWndKawiBawiBoGame::DisableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_LEFT );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_RIGHT );
	pButton->EnableWindow(FALSE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(FALSE);
}

void CWndKawiBawiBoGame::EnableAllBtn()
{
	CWndButton* pButton;
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_LEFT );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_RIGHT );
	pButton->EnableWindow(TRUE);
	pButton = (CWndButton*)GetDlgItem( WIDC_BTN_START );
	pButton->EnableWindow(TRUE);
}

/*************************
   CWndKawiBawiBoGameWin
*************************/
CWndKawiBawiBoGameWin::CWndKawiBawiBoGameWin()
{
	m_pWndGame = NULL;
	m_dwItemID = -1;
	m_dwNextItemID = -1;
	m_nItemCount = -1;
	m_nNextItemCount = -1;
}

CWndKawiBawiBoGameWin::~CWndKawiBawiBoGameWin()
{
}

void CWndKawiBawiBoGameWin::OnDestroy()
{
	if(m_pWndGame != NULL)
	{
		m_pWndGame->RefreshInfo();
		m_pWndGame->EnableAllBtn();
	}
}

void CWndKawiBawiBoGameWin::OnDraw( C2DRender* p2DRender )
{
	if(m_dwItemID != -1 && m_dwNextItemID != -1)
	{
		ItemProp* pItemProp;
		CTexture* pTexture;
		CPoint point;

		pItemProp = prj.GetItemProp( m_dwItemID );
		if(pItemProp != NULL)
		{
			point = CPoint( GetWndCtrl( WIDC_CURRENT_PRESENT )->rect.left + 16, GetWndCtrl( WIDC_CURRENT_PRESENT )->rect.top + 16 );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender,  point);
			if(m_nItemCount > 1)
			{
				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nItemCount );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				m_p2DRender->TextOut( point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff );
			}
		}

		pItemProp = prj.GetItemProp( m_dwNextItemID );
		if(pItemProp != NULL)
		{
			point = CPoint( GetWndCtrl( WIDC_NEXT_PRESENT )->rect.left + 16, GetWndCtrl( WIDC_NEXT_PRESENT )->rect.top + 16 );
			pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
			if(pTexture != NULL)
				pTexture->Render( p2DRender,  point);
			if(m_nNextItemCount > 1)
			{
				TCHAR szTemp[ 32 ];
				_stprintf( szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nNextItemCount );
				CSize size = m_p2DRender->m_pFont->GetTextExtent( szTemp );
				m_p2DRender->TextOut( point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff );
			}
		}
	}
}

void CWndKawiBawiBoGameWin::OnMouseWndSurface(CPoint point)
{
	CItemElem itemElem;
	CRect rectHittest = GetWndCtrl(WIDC_CURRENT_PRESENT)->rect;

	if( rectHittest.PtInRect(point) )
	{
		itemElem.m_dwItemId = m_dwItemID;
		itemElem.m_nItemNum	= 1;
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );

		g_WndMng.PutToolTip_Item( &itemElem, point2, &rectHittest );
	}

	rectHittest = GetWndCtrl(WIDC_NEXT_PRESENT)->rect;
	if( rectHittest.PtInRect(point) )
	{
		itemElem.m_dwItemId = m_dwNextItemID;
		itemElem.m_nItemNum	= 1;

		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &rectHittest );

		g_WndMng.PutToolTip_Item( &itemElem, point2, &rectHittest );
	}
}

void CWndKawiBawiBoGameWin::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndKawiBawiBoGame* pWndGame = (CWndKawiBawiBoGame*)GetWndBase( APP_MINIGAME_KAWIBAWIBO );
	if(pWndGame != NULL)
	{
		CRect rectRoot = pWndGame->m_pWndRoot->GetLayoutRect();
		CRect rectGame = pWndGame->GetWindowRect( TRUE );

		CPoint ptMove;
		CPoint ptGame;

		CRect rect = GetWindowRect( TRUE );
		int wndWidth = rect.Width();

		if(rectRoot.right - rectGame.right < wndWidth)
		{
			ptGame = rectGame.TopLeft();
			ptMove = ptGame + CPoint(-(10+wndWidth), 0);
		}
		else
		{
			ptGame = rectGame.BottomRight();
			ptMove = ptGame + CPoint(10, -rectGame.Height());
		}

		Move( ptMove );
	}

	CWndStatic* pStatic;
	CWndText* pText;
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	pStatic->SetTitle(prj.GetText(TID_GMAE_KAWIBAWIBO_CUR_PRESENT));
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	pStatic->SetTitle(prj.GetText(TID_GAME_KWAIBAWIBO_NXT_PRESENT));
	pText = (CWndEdit*)GetDlgItem( WIDC_TEXT_GUID );
	pText->SetString(prj.GetText( TID_GAME_KAWIBAWIBO_GUID1 ));
	pText->AddString(prj.GetText( TID_GAME_KAWIBAWIBO_GUID2 ), 0xffff0000);
}

BOOL CWndKawiBawiBoGameWin::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{
	m_pWndGame  = (CWndKawiBawiBoGame*)pWndParent;

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MINIGAME_KAWIBAWIBO_WIN, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndKawiBawiBoGameWin::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}

void CWndKawiBawiBoGameWin::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}

void CWndKawiBawiBoGameWin::OnLButtonUp( UINT nFlags, CPoint point )
{
}

void CWndKawiBawiBoGameWin::OnLButtonDown( UINT nFlags, CPoint point )
{
}

BOOL CWndKawiBawiBoGameWin::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if(message == WNM_CLICKED)
	{
		switch(nID)
		{
			case WIDC_BTN_END:
				g_DPlay.SendKawibawiboGetItem();
				if(m_pWndGame != NULL)
				{
					//m_pWndGame->m_nWinningCount = 0;
					m_pWndGame->m_bCheckCoupon = TRUE;
				}
				Destroy();
				break;
			case WIDC_BTN_NEXT:
				if(m_pWndGame != NULL)
				{
					//m_pWndGame->m_nWinningCount++;
					m_pWndGame->m_bCheckCoupon = FALSE;
				}
				Destroy();
				break;
			case WTBID_CLOSE:
				return FALSE;
				break;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndKawiBawiBoGameWin::SetInfo(int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId, int nWinningCount, int nWinningMaxCount)
{
	m_nItemCount = nItemCount;
	m_nNextItemCount = nNextItemCount;
	m_dwItemID = dwItemId;
	m_dwNextItemID = dwNextItemId;

	if(nWinningCount >= nWinningMaxCount)
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BTN_NEXT);
		assert(pWndButton != NULL);
		pWndButton->EnableWindow(FALSE);
	}
}

/******************************
	CWndKawiBawiBoGameConfirm
*******************************/
CWndKawiBawiBoGameConfirm::CWndKawiBawiBoGameConfirm()
{
}

CWndKawiBawiBoGameConfirm::~CWndKawiBawiBoGameConfirm()
{
}

void CWndKawiBawiBoGameConfirm::OnDestroy()
{
}

void CWndKawiBawiBoGameConfirm::OnDraw( C2DRender* p2DRender )
{
}
void CWndKawiBawiBoGameConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60,25);

	CRect rect2_1( x - size.cx - 10, y, ( x - size.cx - 10 ) + size.cx, y + size.cy );
	CRect rect2_2( x + 10          , y, ( x + 10           ) + size.cx, y + size.cy );

	rect.DeflateRect( 10, 10, 10, 35 );
	m_wndText.AddWndStyle( WBS_VSCROLL );
	m_wndText.Create( WBS_NODRAWFRAME, rect, this, 0 );
	m_strText = prj.GetText( TID_GAME_KAWIBAWIBO_STARTGUID2 );
	m_wndText.SetString( m_strText, 0xff000000 );
	m_wndText.ResetString();

	m_wndButton1.Create("OK"    , 0, rect2_1, this, IDOK    );
	m_wndButton2.Create("CANCEL", 0, rect2_2, this, IDCANCEL);
	m_wndButton1.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtOk.tga" ) );
	m_wndButton2.SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtCancel.tga" ) );
	m_wndButton1.FitTextureSize();
	m_wndButton2.FitTextureSize();

	MoveParentCenter();
}

BOOL CWndKawiBawiBoGameConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndKawiBawiBoGameConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndKawiBawiBoGameConfirm::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndKawiBawiBoGameConfirm::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndKawiBawiBoGameConfirm::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndKawiBawiBoGameConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == IDOK )
	{
		SAFE_DELETE( g_WndMng.m_pWndKawiBawiBoGame );
		g_WndMng.m_pWndKawiBawiBoGame = new CWndKawiBawiBoGame;
		g_WndMng.m_pWndKawiBawiBoGame->Initialize();
	}
	else if( nID == IDCANCEL )
	{

	}
	Destroy();
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
#endif //__EVE_MINIGAME
