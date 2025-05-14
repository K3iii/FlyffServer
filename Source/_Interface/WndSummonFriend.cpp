#include "stdafx.h"
#include "resData.h"
#include "WndSummonFriend.h"
#include "dpclient.h"
#include "definetext.h"
#include "party.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

extern		CDPClient		g_DPlay;





CWndSummonFriendMsg::CWndSummonFriendMsg()
	: m_objid(0)
	, m_dwData(0)
{
}
CWndSummonFriendMsg::~CWndSummonFriendMsg()
{
	g_DPlay.SendSummonFriendCancel(m_objid, m_dwData);
}
void CWndSummonFriendMsg::OnDraw( C2DRender* p2DRender )
{
}
void CWndSummonFriendMsg::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSummonFriendMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX_SUMMONFRINED, 0, CPoint( 0, 0 ), pWndParent );
}
void CWndSummonFriendMsg::SetData( OBJID objid, DWORD dwData, char* szName, char* szWorldName )
{
	m_objid = objid;
	m_dwData = dwData;
	strcpy( m_szName, szName );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format(_T( prj.GetText(TID_DIAG_SUMMONFRIEND_CONFIRM) ),m_szName, szWorldName );
	pWndText->SetString( strTemp );
}
BOOL CWndSummonFriendMsg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSummonFriendMsg::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSummonFriendMsg::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSummonFriendMsg::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSummonFriendMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case WIDC_OK:
			{
				g_DPlay.SendSummonFriendConfirm( m_objid, m_dwData );
				Destroy();
			}
			break;
		case WIDC_CANCEL:
			{
				Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}



CWndSummonFriend::CWndSummonFriend()
{
}
CWndSummonFriend::~CWndSummonFriend()
{
}
void CWndSummonFriend::OnDraw( C2DRender* p2DRender )
{
}
void CWndSummonFriend::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_dwData	= 0;

	CWndComboBox* pWndCombo		= (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	for( map<u_long, Friend>::iterator i = g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i )
	{
		u_long idPlayer		= i->first;
		Friend* pFriend		= &i->second;
		if( pFriend->dwState != FRS_OFFLINE && !pFriend->bBlock	)
			pWndCombo->AddString( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
	}
	pWndCombo->SetFocus();


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSummonFriend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SUMMON_FRIEND, 0, CPoint( 0, 0 ), pWndParent );
}
void CWndSummonFriend::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );
}
BOOL CWndSummonFriend::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSummonFriend::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSummonFriend::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSummonFriend::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSummonFriend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			CWndComboBox* pWndCombo		= (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CString string	= pWndCombo->GetString();
			if( 3 <= string.GetLength() && string.GetLength() < MAX_NAME )
			{
				if( 0 != strcmp( g_pPlayer->GetName(), string ) )
				{
					WORD wId	= LOWORD( m_dwData );
					WORD wMode	= HIWORD( m_dwData );
					CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( wId );
					if( pItemElem )
					{
						g_DPlay.SendSummonFriend( m_dwData, (LPSTR)(LPCSTR)string );
					}
					else
					{
						ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_FRIENDSUMMON_A );
						if( pItemProp )
						{
							char lpString[260]	= { 0, };
							sprintf( lpString, prj.GetText( TID_ERROR_SUMMONFRIEND_NOITEM ), pItemProp->szName, string );
							g_WndMng.PutString( lpString, NULL, prj.GetTextColor( TID_ERROR_SUMMONFRIEND_NOITEM ) );
						}
					}
				}
				else
				{
					g_WndMng.PutString( prj.GetText( TID_GAME_SUMMON_FRIEND_MY_NOUSE ), NULL, prj.GetTextColor( TID_GAME_SUMMON_FRIEND_MY_NOUSE ) );
				}
				Destroy();
			}
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}



CWndSummonPartyMsg::CWndSummonPartyMsg()
{
}
CWndSummonPartyMsg::~CWndSummonPartyMsg()
{
}
HRESULT CWndSummonPartyMsg::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	m_Texture.DeleteDeviceObjects();
	return TRUE;
}
void CWndSummonPartyMsg::OnDraw( C2DRender* p2DRender )
{
	int	sx, sy;
	sx = 8;
	sy = 120;
	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture, 255  );
}
void CWndSummonPartyMsg::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
	if( pItemProp )
		m_Texture.LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSummonPartyMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX_SUMMONPARTY, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndSummonPartyMsg::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSummonPartyMsg::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSummonPartyMsg::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSummonPartyMsg::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSummonPartyMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndSummonPartyMsg::SetData( OBJID objid, DWORD dwData, const char* szWorldName )
{
	g_Neuz.m_dwSummonPartyObjid = objid;
	g_Neuz.m_dwSummonPartyData = dwData;
	strcpy( g_Neuz.m_szSummonPartyWorldName, szWorldName );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CWndText* pWndText1 = (CWndText*)GetDlgItem( WIDC_TEXT2 );
	CString strTemp;
	strTemp.Format( _T( prj.GetText(TID_DIAG_SUMMONPARTY_CONFIRM) ), g_Party.m_sParty );
	pWndText->SetString( strTemp );

	ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
	if( pItemProp )
	{
		strTemp.Format( _T( prj.GetText(TID_DIAG_SUMMONPARTY_CONFIRM_USE) ), pItemProp->szName );
		pWndText1->SetString( strTemp );
	}
}
/****************************************************
  WndId : APP_SUMMON_PARTY - Applet
  CtrlId : WIDC_TEXT1 -
  CtrlId : WIDC_CANCEL - Button
  CtrlId : WIDC_OK - Button
****************************************************/

CWndSummonParty::CWndSummonParty()
{
}
CWndSummonParty::~CWndSummonParty()
{
}
void CWndSummonParty::OnDraw( C2DRender* p2DRender )
{
}
void CWndSummonParty::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_dwData	= 0;


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSummonParty::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SUMMON_PARTY, 0, CPoint( 0, 0 ), pWndParent );
}
BOOL CWndSummonParty::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSummonParty::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSummonParty::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSummonParty::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSummonParty::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			WORD wId	= LOWORD( m_dwData );
			WORD wMode	= HIWORD( m_dwData );
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( wId );
			if( pItemElem )
			{
				g_DPlay.SendSummonParty( m_dwData );
			}
			else
			{
				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
				if( pItemProp )
				{
					char lpString[260]	= { 0, };
					sprintf( lpString, prj.GetText( TID_ERROR_SUMMONPARTY_NOITEM ), pItemProp->szName );
					g_WndMng.PutString( lpString, NULL, prj.GetTextColor( TID_ERROR_SUMMONPARTY_NOITEM ) );
				}
			}
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
void CWndSummonParty::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format( _T( prj.GetText(TID_GAME_SUMMONPARTY) ), g_pPlayer->GetWorld()->m_szWorldName );
	pWndText->SetString( strTemp );

}



CWndSummonPartyUse::CWndSummonPartyUse()
{
}
CWndSummonPartyUse::~CWndSummonPartyUse()
{
}
void CWndSummonPartyUse::OnDraw( C2DRender* p2DRender )
{
}
void CWndSummonPartyUse::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strMsg;
	strMsg.Format( prj.GetText(TID_DIAG_SUMMONPARTY_USE) , g_Neuz.m_szSummonPartyWorldName );
	pWndText->SetString( strMsg );


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndSummonPartyUse::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ )
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_SUMMON_PARTY_USE, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndSummonPartyUse::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndSummonPartyUse::OnSize( UINT nType, int cx, int cy )
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndSummonPartyUse::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndSummonPartyUse::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndSummonPartyUse::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_OK:
		{
			g_DPlay.SendSummonPartyConfirm( g_Neuz.m_dwSummonPartyObjid, g_Neuz.m_dwSummonPartyData );
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
