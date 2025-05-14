#include "stdafx.h"

#if __VER >= 13 // __HOUSING
#include "HousingDBCtrl.h"
#include "Housing.h"
#include "dptrans.h"
extern AppInfo g_appInfo;

//////////////////////////////////////////////////////////////
/// CHousingDBCtrl ///////////////////////////////////////////
//////////////////////////////////////////////////////////////
CHousingDBCtrl::CHousingDBCtrl(void)
{
}

CHousingDBCtrl::~CHousingDBCtrl(void)
{
}

void CHousingDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	switch( pov->nQueryMode )
	{
		case QUERY_HOUSING_LOAD:
			{
				DWORD dwPlayerId;
				ar >> dwPlayerId;

				LoadHousingInfo( dwPlayerId, dwCompletionKey );
			}
			break;

		case QUERY_HOUSING_INSERT_LIST:
			{
				DWORD dwPlayerId, dwItemId;
				ar >> dwPlayerId >> dwItemId;

				InsertFurnitureList( dwPlayerId, dwItemId, dwCompletionKey );
			}
			break;

		case QUERY_HOUSING_SETUP_FURNITURE:
			{
				DWORD dwPlayerId;
				HOUSINGINFO housingInfo;

				ar >> dwPlayerId;
				housingInfo.Serialize( ar );

				SetupFurniture( dwPlayerId, housingInfo, dwCompletionKey );
			}
			break;

		case QUERY_HOUSING_SET_VISITALLOW:
			{
				DWORD dwPlayerId, dwTargetId;
				BOOL bAllow;

				ar >> dwPlayerId >> dwTargetId >> bAllow;

				SetVisitAllow( dwPlayerId, dwTargetId, bAllow, dwCompletionKey );
			}
			break;

		case QUERY_HOUSING_GM_REMOVEALL:
			{
				DWORD dwPlayerId;
				ar >> dwPlayerId;
				CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
				if( pHousing )
					pHousing->ProcessExpiredFurniture( time_null(), TRUE );
			}
			break;
	}

}

void CHousingDBCtrl::LoadHousingInfo( DWORD dwPlayerId, DPID dpId )
{
	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing( dwPlayerId );
	if( !pHousing )
	{
		pHousing = CHousingMng::GetInstance()->CreateHousing( dwPlayerId );
		if( !pHousing )
			return;


		CQuery* pQuery = GetQueryObject();
		if( pQuery->Execute( "usp_Housing_Load '%02d', '%07d'", g_appInfo.dwSys, dwPlayerId ) )
		{
			while( pQuery->Fetch() )
			{
				HOUSINGINFO housingInfo;
				housingInfo.dwItemId = static_cast<DWORD>( pQuery->GetInt( "ItemIndex" ) );
				housingInfo.tKeepTime = static_cast<time_t>( pQuery->GetInt64( "tKeepTime" ) );
				housingInfo.bSetup = static_cast<BOOL>( pQuery->GetInt( "bSetup" ) );
				housingInfo.vPos.x = pQuery->GetFloat( "x_Pos" );
				housingInfo.vPos.y = pQuery->GetFloat( "y_Pos" );
				housingInfo.vPos.z = pQuery->GetFloat( "z_Pos" );
				housingInfo.fAngle = pQuery->GetFloat( "fAngle" );

				ItemProp* pItemProp = prj.GetItemProp( housingInfo.dwItemId );
				if( pItemProp && pItemProp->dwItemKind1 == IK1_HOUSING )
					pHousing->SetFurnitureList( housingInfo, TRUE );
			}

			if( pQuery->MoreResults() )
			{
				while( pQuery->Fetch() )
					pHousing->SetVisitAllow( pQuery->GetInt( "f_idPlayer" ), TRUE );
			}
		}
	}


	CDPTrans::GetInstance()->SendHousingLoadInfo( dwPlayerId, pHousing, dpId );
}

void CHousingDBCtrl::InsertFurnitureList( DWORD dwPlayerId, DWORD dwItemId, DPID dpId )
{
	HOUSINGINFO housingInfo( dwItemId );
	// int n = sizeof(time_t);


	if( GetQueryObject()->Execute( "usp_Housing_Insert '%02d', '%07d', %d, %I64d",
		g_appInfo.dwSys, dwPlayerId, housingInfo.dwItemId, static_cast<__int64>(housingInfo.tKeepTime) ) )
	{
		CHousingMng::GetInstance()->SetFurnitureList( dwPlayerId, housingInfo, TRUE );
		CDPTrans::GetInstance()->SendHousingFurnitureList( dwPlayerId, housingInfo, TRUE, dpId );
	}
	else
		CDPTrans::GetInstance()->SendHousingDBFailed( dwPlayerId, housingInfo.dwItemId, dpId );
}

void CHousingDBCtrl::SetupFurniture( DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId )
{
	if( GetQueryObject()->Execute( "usp_Housing_Update '%02d', '%07d', %d, %d, %f, %f, %f, %f",
		g_appInfo.dwSys, dwPlayerId, housingInfo.dwItemId, housingInfo.bSetup,
		housingInfo.vPos.x, housingInfo.vPos.y, housingInfo.vPos.z, housingInfo.fAngle ) )
	{
		CHousingMng::GetInstance()->SetupFurniture( dwPlayerId, housingInfo );
		CDPTrans::GetInstance()->SendHousingSetupFurniture( dwPlayerId, housingInfo, dpId );
	}
	else
		CDPTrans::GetInstance()->SendHousingDBFailed( dwPlayerId, NULL_ID, dpId );
}

void CHousingDBCtrl::DeleteFurnitureList( DWORD dwPlayerId, HOUSINGINFO housingInfo )
{
	if( GetQueryObject()->Execute( "usp_Housing_Del '%02d', '%07d', %d", g_appInfo.dwSys, dwPlayerId, housingInfo.dwItemId ) )
	{
		CHousingMng::GetInstance()->SetFurnitureList( dwPlayerId, housingInfo, FALSE );
		CDPTrans::GetInstance()->SendHousingFurnitureList( dwPlayerId, housingInfo, FALSE, DPID_ALLPLAYERS );
	}
}

void CHousingDBCtrl::SetVisitAllow( DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId )
{
	if( GetQueryObject()->Execute( "%s '%02d', '%07d', '%07d'",
		bAllow? "usp_Housing_Visit_Insert": "usp_Housing_Visit_Delete",
		g_appInfo.dwSys, dwPlayerId, dwTargetId ) )
	{
		CHousingMng::GetInstance()->SetVisitAllow( dwPlayerId, dwTargetId, bAllow );
		CDPTrans::GetInstance()->SendHousingSetVisitAllow( dwPlayerId, dwTargetId, bAllow, dpId );
	}
	else
		CDPTrans::GetInstance()->SendHousingDBFailed( dwPlayerId, NULL_ID, dpId );
}

void CHousingDBCtrl::OnTimer()
{
	CHousingMng::GetInstance()->ProcessRemoveExpiredFurniture();
}

//////////////////////////////////////////////////////////////
/// CHousingDBMng ////////////////////////////////////////////
//////////////////////////////////////////////////////////////
CHousingDBMng::CHousingDBMng(void)
{
	if( !m_HousingDBCtrl.CreateDbHandler( MIN(1) ) )
		Error( "CHousingDBMng - m_HousingDBCtrl.CreateDbHandler()" );
}

CHousingDBMng::~CHousingDBMng(void)
{
	m_HousingDBCtrl.CloseDbHandler();
}

CHousingDBMng* CHousingDBMng::GetInstance()
{
	static CHousingDBMng sHousingDBMng;
	return & sHousingDBMng;
}

void CHousingDBMng::DeleteFurnitureList( DWORD dwPlayerId, HOUSINGINFO housingInfo )
{
	m_HousingDBCtrl.DeleteFurnitureList( dwPlayerId, housingInfo );
}
#endif // __HOUSING