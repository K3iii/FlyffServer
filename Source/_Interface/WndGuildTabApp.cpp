#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndGuildTabApp.h"
#include "WndManager.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "guild.h"
extern	CGuildMng	g_GuildMng;




/****************************************************
  WndId : APP_GUILD_TABAPP
****************************************************/
CWndGuildTabApp::CWndGuildTabApp()
{
	for( int i=0; i<MAX_GM_LEVEL; i++ )
		m_adwPower[i] = 0xffffffff;

}
CWndGuildTabApp::~CWndGuildTabApp()
{

}
void CWndGuildTabApp::OnDraw( C2DRender* p2DRender )
{
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );

	TEXTUREVERTEX2 pVertex[ 6 * MAX_GM_LEVEL ];
	TEXTUREVERTEX2* pVertices = pVertex;

	LPWNDCTRL pCustom = NULL;

	pCustom = GetWndCtrl( WIDC_CUSTOM1 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 0 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM2 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 1 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM3 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 2 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM4 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 3 + ( 6 * 1 ), &pVertices, 0xffffffff );
	pCustom = GetWndCtrl( WIDC_CUSTOM5 );
	pWndWorld->m_texMsgIcon.MakeVertex( p2DRender, pCustom->rect.TopLeft(), 43 + 4 + ( 6 * 1 ), &pVertices, 0xffffffff );

	pWndWorld->m_texMsgIcon.Render( m_pApp->m_pd3dDevice, pVertex, ( (int) pVertices - (int) pVertex ) / sizeof( TEXTUREVERTEX2 ) );
}
void CWndGuildTabApp::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	UpdateData();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}
BOOL CWndGuildTabApp::Initialize( CWndBase* pWndParent, DWORD )
{
	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_GUILD_TABAPPELLATION, 0, CPoint( 0, 0 ), pWndParent );
}

BOOL CWndGuildTabApp::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndGuildTabApp::OnSize( UINT nType, int cx, int cy ) \
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndGuildTabApp::OnLButtonUp( UINT nFlags, CPoint point )
{
}
void CWndGuildTabApp::OnLButtonDown( UINT nFlags, CPoint point )
{
}
BOOL CWndGuildTabApp::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	CGuild* pGuild = g_pPlayer->GetGuild();

	if( pGuild == NULL )
		return FALSE;

	CGuildMember* pGuildMember = pGuild->GetMember(g_pPlayer->m_idPlayer);

	if( pGuildMember == NULL )
		return FALSE;

	if(	pGuildMember->m_nMemberLv != GUD_MASTER )
		return FALSE;

	//Kingpin
	if( nID == WIDC_CHECK2 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_MEMBERLEVEL : m_adwPower[GUD_KINGPIN] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK7 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_LEVEL : m_adwPower[GUD_KINGPIN] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK12 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_INVITATION : m_adwPower[GUD_KINGPIN] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK17 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_PENYA : m_adwPower[GUD_KINGPIN] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK22 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_ITEM : m_adwPower[GUD_KINGPIN] &= (~PF_ITEM);
	}

	if (nID == WIDC_CHECK26)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK26);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_KINGPIN] &= (~PF_GUILDHOUSE_FURNITURE);
	}

	if (nID == WIDC_CHECK27)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK27);
		pWndCheck->GetCheck() ? m_adwPower[GUD_KINGPIN] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_KINGPIN] &= (~PF_GUILDHOUSE_UPKEEP);
	}


	//GA_LEADER
	if( nID == WIDC_CHECK3 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_MEMBERLEVEL : m_adwPower[GUD_CAPTAIN] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK8 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_LEVEL : m_adwPower[GUD_CAPTAIN] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK13 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_INVITATION : m_adwPower[GUD_CAPTAIN] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK18 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_PENYA : m_adwPower[GUD_CAPTAIN] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK23 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_ITEM : m_adwPower[GUD_CAPTAIN] &= (~PF_ITEM);
	}

	if (nID == WIDC_CHECK28)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK28);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_CAPTAIN] &= (~PF_GUILDHOUSE_FURNITURE);
	}

	if (nID == WIDC_CHECK29)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK29);
		pWndCheck->GetCheck() ? m_adwPower[GUD_CAPTAIN] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_CAPTAIN] &= (~PF_GUILDHOUSE_UPKEEP);
	}


	//GA_SUPPORTER
	if( nID == WIDC_CHECK4 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_MEMBERLEVEL : m_adwPower[GUD_SUPPORTER] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK9 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_LEVEL : m_adwPower[GUD_SUPPORTER] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK14 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_INVITATION : m_adwPower[GUD_SUPPORTER] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK19 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_PENYA : m_adwPower[GUD_SUPPORTER] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK24 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_ITEM : m_adwPower[GUD_SUPPORTER] &= (~PF_ITEM);
	}

	if (nID == WIDC_CHECK30)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK30);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_SUPPORTER] &= (~PF_GUILDHOUSE_FURNITURE);
	}

	if (nID == WIDC_CHECK31)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK31);
		pWndCheck->GetCheck() ? m_adwPower[GUD_SUPPORTER] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_SUPPORTER] &= (~PF_GUILDHOUSE_UPKEEP);
	}


	//GA_ROOKIE
	if( nID == WIDC_CHECK5 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_MEMBERLEVEL : m_adwPower[GUD_ROOKIE] &= (~PF_MEMBERLEVEL);
	}
	if( nID == WIDC_CHECK10 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_LEVEL : m_adwPower[GUD_ROOKIE] &= (~PF_LEVEL);
	}
	if( nID == WIDC_CHECK15 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_INVITATION : m_adwPower[GUD_ROOKIE] &= (~PF_INVITATION);
	}
	if( nID == WIDC_CHECK20 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_PENYA : m_adwPower[GUD_ROOKIE] &= (~PF_PENYA);
	}
	if( nID == WIDC_CHECK25 )
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_ITEM : m_adwPower[GUD_ROOKIE] &= (~PF_ITEM);
	}

	if (nID == WIDC_CHECK32)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK32);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_GUILDHOUSE_FURNITURE : m_adwPower[GUD_ROOKIE] &= (~PF_GUILDHOUSE_FURNITURE);
	}

	if (nID == WIDC_CHECK33)
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK33);
		pWndCheck->GetCheck() ? m_adwPower[GUD_ROOKIE] |= PF_GUILDHOUSE_UPKEEP : m_adwPower[GUD_ROOKIE] &= (~PF_GUILDHOUSE_UPKEEP);
	}


	if( nID == WIDC_OK )
	{
		g_DPlay.SendGuildAuthority( pGuild->GetGuildId(), m_adwPower );
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


void CWndGuildTabApp::SetData(DWORD dwPower[])
{
	CWndButton* pWndCheck = NULL;

	memcpy( m_adwPower, dwPower, sizeof(DWORD)*MAX_GM_LEVEL );

	// Master
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	if( m_adwPower[GUD_MASTER] & PF_MEMBERLEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	if( m_adwPower[GUD_MASTER] & PF_LEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	if( m_adwPower[GUD_MASTER] & PF_INVITATION )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	if( m_adwPower[GUD_MASTER] & PF_PENYA )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	if( m_adwPower[GUD_MASTER] & PF_ITEM )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK34);
	if (m_adwPower[GUD_MASTER] & PF_GUILDHOUSE_FURNITURE)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK35);
	if (m_adwPower[GUD_MASTER] & PF_GUILDHOUSE_UPKEEP)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);


	// GUD_KINGPIN
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	if( m_adwPower[GUD_KINGPIN] & PF_MEMBERLEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	if( m_adwPower[GUD_KINGPIN] & PF_LEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	if( m_adwPower[GUD_KINGPIN] & PF_INVITATION )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
	if( m_adwPower[GUD_KINGPIN] & PF_PENYA )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
	if( m_adwPower[GUD_KINGPIN] & PF_ITEM )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK26);
	if (m_adwPower[GUD_KINGPIN] & PF_GUILDHOUSE_FURNITURE)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK27);
	if (m_adwPower[GUD_KINGPIN] & PF_GUILDHOUSE_UPKEEP)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);


	// GUD_CAPTAIN
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	if( m_adwPower[GUD_CAPTAIN] & PF_MEMBERLEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	if( m_adwPower[GUD_CAPTAIN] & PF_LEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
	if( m_adwPower[GUD_CAPTAIN] & PF_INVITATION )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
	if( m_adwPower[GUD_CAPTAIN] & PF_PENYA )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
	if( m_adwPower[GUD_CAPTAIN] & PF_ITEM )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK28);
	if (m_adwPower[GUD_CAPTAIN] & PF_GUILDHOUSE_FURNITURE)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK29);
	if (m_adwPower[GUD_CAPTAIN] & PF_GUILDHOUSE_UPKEEP)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);


	// GUD_SUPPORTER
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if( m_adwPower[GUD_SUPPORTER] & PF_MEMBERLEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	if( m_adwPower[GUD_SUPPORTER] & PF_LEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
	if( m_adwPower[GUD_SUPPORTER] & PF_INVITATION )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
	if( m_adwPower[GUD_SUPPORTER] & PF_PENYA )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
	if( m_adwPower[GUD_SUPPORTER] & PF_ITEM )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK30);
	if (m_adwPower[GUD_SUPPORTER] & PF_GUILDHOUSE_FURNITURE)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK31);
	if (m_adwPower[GUD_SUPPORTER] & PF_GUILDHOUSE_UPKEEP)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);


	// GUD_ROOKIE
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	if( m_adwPower[GUD_ROOKIE] & PF_MEMBERLEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	if( m_adwPower[GUD_ROOKIE] & PF_LEVEL )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
	if( m_adwPower[GUD_ROOKIE] & PF_INVITATION )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
	if( m_adwPower[GUD_ROOKIE] & PF_PENYA )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
	if( m_adwPower[GUD_ROOKIE] & PF_ITEM )
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK32);
	if (m_adwPower[GUD_ROOKIE] & PF_GUILDHOUSE_FURNITURE)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK33);
	if (m_adwPower[GUD_ROOKIE] & PF_GUILDHOUSE_UPKEEP)
		pWndCheck->SetCheck(1);
	else
		pWndCheck->SetCheck(0);

}

void CWndGuildTabApp::EnableButton(BOOL bEnable)
{
	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK34);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK35);
	pWndCheck->EnableWindow(bEnable);


	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK17);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK22);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK26);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK27);
	pWndCheck->EnableWindow(bEnable);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK13);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK18);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK23);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK28);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK29);
	pWndCheck->EnableWindow(bEnable);


	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK14);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK19);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK24);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK30);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK31);
	pWndCheck->EnableWindow(bEnable);

	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK15);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK20);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK25);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK32);
	pWndCheck->EnableWindow(bEnable);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK33);
	pWndCheck->EnableWindow(bEnable);


	pWndCheck = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndCheck->EnableWindow(bEnable);

}


void CWndGuildTabApp::UpdateData()
{
	CGuild* pGuild = g_pPlayer->GetGuild();

	if( pGuild  )
	{
		CString str;
		CWndStatic* pStatic;

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_MASTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_MASTER), pGuild->GetMaxMemberLvSize( GUD_MASTER ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_KINGPIN );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_KINGPIN), pGuild->GetMaxMemberLvSize( GUD_KINGPIN ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_CAPTAIN  );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_CAPTAIN), pGuild->GetMaxMemberLvSize( GUD_CAPTAIN ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_SUPPORTER );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_SUPPORTER), pGuild->GetMaxMemberLvSize( GUD_SUPPORTER ) );
		pStatic->SetTitle( str );

		pStatic = (CWndStatic*)GetDlgItem( WIDC_GUILD_NUM_ROOKIE );
		str.Format( "%.2d/%.2d", pGuild->GetMemberLvSize(GUD_ROOKIE), pGuild->GetMaxMemberSize() );
		pStatic->SetTitle( str );

		SetData( pGuild->m_adwPower );

		if(	pGuild->IsMaster( g_pPlayer->m_idPlayer ) )
			EnableButton( TRUE );
		else
			EnableButton( FALSE );
	}
	else
	{
		DWORD adwPower [MAX_GM_LEVEL] = { 0 };
		SetData( adwPower );

		EnableButton( FALSE );
	}


	CWndButton* pWndCheck = NULL;
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK34);
	pWndCheck->EnableWindow(FALSE);
	pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK35);
	pWndCheck->EnableWindow(FALSE);
}
