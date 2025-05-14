// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
//#include "WndSample.h"
#include "WndDebugInfo.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

CWndDebugInfo::CWndDebugInfo()
{
	SetPutRegInfo( FALSE );
}

CWndDebugInfo::~CWndDebugInfo()
{

}
void CWndDebugInfo::OnDraw(C2DRender* p2DRender)
{
    // Show frame rate
	p2DRender->TextOut( 2,  0 ,g_Neuz.m_strFrameStats , D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	p2DRender->TextOut( 2, 20 ,g_Neuz.m_strDeviceStats, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );

	TCHAR strDebug[90];

	if( g_pPlayer && g_pPlayer->GetWorld() )
	{
		D3DXVECTOR3 vPos = g_pPlayer->GetPos();
		_stprintf( strDebug, _T("x:%6.3f y:%6.3f z:%6.3f %6.3f"), vPos.x, vPos.y, vPos.z,(float)g_pPlayer->GetAngle());
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		_stprintf( strDebug, _T("Objects:%d  Faces:%d" ), g_pPlayer->GetWorld()->m_nObjCullSize, g_nMaxTri);
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}
	else
	{
		_stprintf( strDebug, _T( "x:0.0 y:0.0 z:%0.0 0.0" ) );
		p2DRender->TextOut( 2, 40, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		_stprintf( strDebug, _T("Objects:0  Faces:0" ) );
		p2DRender->TextOut( 2, 55, strDebug, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}


	if( g_pPlayer && g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
	{
		CString string;
		string.Format( "Ping(%d ms)", g_Neuz.m_dwPingTime );
		p2DRender->TextOut( 200, 260, string, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
	}
}
void CWndDebugInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pWndButton;

	pWndButton = (CWndButton*)GetDlgItem( WIDC_GRID       ); pWndButton->SetCheck( g_WorldMng()->m_bViewGrid      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_TERRAIN    ); pWndButton->SetCheck( g_WorldMng()->m_bViewTerrain   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_TERRAIN); pWndButton->SetCheck( g_WorldMng()->m_bViewLODTerrain);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LOD_OBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bViewLODObj    );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ANIMATION  ); pWndButton->SetCheck( CObj::IsAnimate()	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_LIGHT      ); pWndButton->SetCheck( g_WorldMng()->m_bViewLight     );
	//pWndButton = (CWndButton*)GetDlgItem( WIDC_CAPTURE    ); pWndButton->SetCheck( 	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HIDE       ); pWndButton->SetCheck( !g_pPlayer->IsVisible() );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WIREFRAME  ); pWndButton->SetCheck( g_WorldMng()->m_bViewWireframe  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_OBJECT     ); pWndButton->SetCheck( g_WorldMng()->m_bViewAllObjects );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FRAME_SKIP ); pWndButton->SetCheck( GetFrameSkip()	               );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WEATHER    ); pWndButton->SetCheck( g_WorldMng()->m_bViewWeather	   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_BOUNDBOX   ); pWndButton->SetCheck( g_WorldMng()->m_bViewBoundBox   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_ATTRIBUTE  ); pWndButton->SetCheck( g_WorldMng()->m_bViewHeightAttribute	 	);
	pWndButton = (CWndButton*)GetDlgItem( WIDC_SKYBOX     ); pWndButton->SetCheck( g_WorldMng()->m_bViewSkybox 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_NAME       ); pWndButton->SetCheck( g_WorldMng()->m_bViewName 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_CULLOBJ    ); pWndButton->SetCheck( g_WorldMng()->m_bCullObj	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_COLLISION  ); pWndButton->SetCheck( CObj::m_bCollision	 	      );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_FOG        ); pWndButton->SetCheck( g_WorldMng()->m_bViewFog 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_IDSTATE    ); pWndButton->SetCheck( g_WorldMng()->m_bViewIdState   );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_WATER      ); pWndButton->SetCheck( g_WorldMng()->m_bViewWater 	  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR       ); pWndButton->SetCheck( g_GameTimer.IsFixed()  );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_CHECK1     ); pWndButton->SetCheck( g_WorldMng()->m_bViewHP 	  );

	CString string;
	string.Format( "%d", g_GameTimer.m_nHour );
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR ); pWndEdit->SetString( string );
	pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR ); pWndButton->SetCheck(  g_GameTimer.IsFixed() );
	//g_World.m_bViewWater
	////////////////////////////////////////////////

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	MoveParentCenter();
}

BOOL CWndDebugInfo::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{

	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), APP_DEBUGINFO, 0, CPoint( 0, 0 ), pWndParent );

}
BOOL CWndDebugInfo::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*)*pLResult;
	//if( message == WNM_CLICKED )
	{
		switch( nID )
		{
		case WIDC_BUTTON1: // exp up
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			int nExp = _ttoi( pWndEdit->GetString() );

			if( nExp >= 0 && nExp < 0x7fffffff )
			{
				g_DPlay.SendExp( nExp );
			}
			break;
		}

		case WIDC_GRID: g_WorldMng()->m_bViewGrid      = !g_WorldMng()->m_bViewGrid      ;
			break;

		case WIDC_WIREFRAME: g_WorldMng()->m_bViewWireframe = !g_WorldMng()->m_bViewWireframe ; break;
		case WIDC_SKYBOX: g_WorldMng()->m_bViewSkybox    = !g_WorldMng()->m_bViewSkybox    ; break;
		case WIDC_TERRAIN: g_WorldMng()->m_bViewTerrain   = !g_WorldMng()->m_bViewTerrain   ; break;
		case WIDC_OBJECT: g_WorldMng()->m_bViewAllObjects   = !g_WorldMng()->m_bViewAllObjects   ; break;
		case 105:
			g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog;
			g_WorldMng()->SetFogEnable( g_Neuz.m_pd3dDevice, g_WorldMng()->m_bViewFog );
			break;
		case WIDC_LOD_TERRAIN: g_WorldMng()->m_bViewLODTerrain       = !g_WorldMng()->m_bViewLODTerrain       ; break;
		case WIDC_LOD_OBJ: g_WorldMng()->m_bViewLODObj      = !g_WorldMng()->m_bViewLODObj       ; break;
		case WIDC_WATER: g_WorldMng()->m_bViewWater     = !g_WorldMng()->m_bViewWater     ; break;
		case WIDC_NAME: g_WorldMng()->m_bViewName      = !g_WorldMng()->m_bViewName      ; break;
		case WIDC_ANIMATION: CObj::AnimateEnable( !CObj::IsAnimate() ); break;
		case WIDC_FRAME_SKIP: SetFrameSkip( !GetFrameSkip() ); break;
		case WIDC_FOG: g_WorldMng()->m_bViewFog = !g_WorldMng()->m_bViewFog; break;
		case WIDC_CHECK1: g_WorldMng()->m_bViewHP      = !g_WorldMng()->m_bViewHP      ; break;

		case WIDC_CULLOBJ: g_WorldMng()->m_bCullObj = !g_WorldMng()->m_bCullObj; break;
		case WIDC_LIGHT: g_WorldMng()->m_bViewLight = !g_WorldMng()->m_bViewLight; break;
		case WIDC_WEATHER: g_WorldMng()->m_bViewWeather = !g_WorldMng()->m_bViewWeather; break;
		case WIDC_ATTRIBUTE: g_WorldMng()->m_bViewHeightAttribute = !g_WorldMng()->m_bViewHeightAttribute; break;
		case WIDC_HIDE: g_pPlayer->SetVisible( !g_pPlayer->IsVisible() ); break;
		case WIDC_BOUNDBOX:
//			g_World.m_bViewBoundBox = !g_World.m_bViewBoundBox;
			g_WorldMng()->m_bViewBoundBox = !g_WorldMng()->m_bViewBoundBox;
			break;
		case WIDC_COLLISION:
			CObj::m_bCollision = !CObj::m_bCollision;
			break;

		case WIDC_IDSTATE:
			g_WorldMng()->m_bViewIdState = !g_WorldMng()->m_bViewIdState;
			break;
		case WIDC_HOUR:
			 g_GameTimer.SetFixed( !g_GameTimer.IsFixed() );
			 pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
			 if( pWndButton->GetCheck() )
			 {
				 CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
				 int nHour = atoi( pWndEdit->GetString() );
				 if( nHour > 23 ) nHour = 23;
				 if( nHour < 0 ) nHour = 0;
				 g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
			 }

			break;
		case WIDC_EDIT_HOUR:
			{
				pWndButton = (CWndButton*)GetDlgItem( WIDC_HOUR );
				if( pWndButton->GetCheck() )
				{
					CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT_HOUR );
					int nHour = atoi( pWndEdit->GetString() );
					if( nHour > 23 ) nHour = 23;
					if( nHour < 0 ) nHour = 0;
					g_GameTimer.SetTime( 0, nHour, 0, 0 );//Fixed( 	g_GameTimer.IsFixed() );//g_WorldMng()->m_nFixedHour = nHour;
				}
			}
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}
BOOL CWndDebugInfo::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase );
}
void CWndDebugInfo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType,cx,cy);
}
void CWndDebugInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonUp(nFlags,point);
}
void CWndDebugInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndNeuz::OnLButtonDown(nFlags,point);
}
void CWndDebugInfo::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if( m_pWndCapture == pWndChild )
		SAFE_DELETE( m_pWndCapture );
}

