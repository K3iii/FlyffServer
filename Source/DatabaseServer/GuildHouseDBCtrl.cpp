// GuildHouseDBCtrl.cpp: implementation of the CGuildHouseDBCtrl class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuildHouseDBCtrl.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "MsgHdr.h"
#include "dptrans.h"
extern AppInfo g_appInfo;

////////////////////////////////////////////////////////////////
// CGuildHouseDBCtrl::CDbController
////////////////////////////////////////////////////////////////
CGuildHouseDBCtrl::CGuildHouseDBCtrl()
{

}

CGuildHouseDBCtrl::~CGuildHouseDBCtrl()
{

}

void CGuildHouseDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );

	switch( pov->nQueryMode )
	{
		case GUILDHOUSE_LOAD:
			if( !LoadGuildHouse() )
				AfxMessageBox( "GuildHouse Load Failed!!!" );
			break;

		case GUILDHOUSE_SEND:
			SendGuildHouse( dwCompletionKey );
			break;

		case GUILDHOUSE_CREATE:
			CreateGuildHouse( ar, dwCompletionKey );
			break;

		case GUILDHOUSE_REMOVE:
			RemoveGuildHouse( dwCompletionKey );
			break;

		case GUILDHOUSE_DEFAULT_PACKET:
			RunDefaultPacket( ar, dwCompletionKey );
			break;
	}
}

void CGuildHouseDBCtrl::OnTimer()
{
	GuildHouseMng->ProcessExpired();
}

void CGuildHouseDBCtrl::SendGuildHouse( DPID dpId )
{
	if( !GuildHouseMng->IsEmpty() )
	{
		BEFORESENDDUAL( ar, PACKETTYPE_GUILDHOUSE_LOAD, DPID_UNKNOWN, DPID_UNKNOWN );
		GuildHouseMng->Serialize( ar );
		SEND( ar, CDPTrans::GetInstance(), dpId );
	}
}

void CGuildHouseDBCtrl::CreateGuildHouse( CAr & ar, DPID dpId )
{
	DWORD dwPlayerId, dwGuildId;
	ar >> dwPlayerId >> dwGuildId;

	BOOL bResult = FALSE;
	CQuery* pQuery = GetQueryObject();

	BEFORESENDDUAL( arCreate, PACKETTYPE_GUILDHOUSE_BUY, DPID_UNKNOWN, DPID_UNKNOWN );
	{
		arCreate << dwPlayerId << dwGuildId;
		CGuildHouseBase* pGuildHouse = GuildHouseMng->MakeGuildHouse( dwGuildId, WI_GUILDHOUSE_SMALL );
		if( pGuildHouse )
		{
			pGuildHouse->SetUpkeeptime( time_null() + ( UPKEEP_DAY_TIME * 7 ) );
			if( pQuery->Execute( "usp_GuildHouse_Insert '%02d', '%06d', %d, %d", g_appInfo.dwSys, dwGuildId, WI_GUILDHOUSE_SMALL, pGuildHouse->GetUpkeepTime() )
				&& GuildHouseMng->AddGuildHouse( dwGuildId, pGuildHouse ) )
			{
				arCreate << TRUE;
				dpId = DPID_ALLPLAYERS;
				GH_Fntr_Info gfi( II_GHOU_FUR_NPC_TELEPORTER, TRUE, pGuildHouse->GetTeleporterPos(), 0.0f, ( UPKEEP_DAY_TIME * 7 ) );
				if( ExcuteQuery( dwGuildId, pGuildHouse, GUILDHOUSE_PCKTTYPE_LISTUP, gfi, NULL_ID ) )
					pGuildHouse->OnGuildHousePacket( GUILDHOUSE_PCKTTYPE_LISTUP, gfi, NULL_ID );
				pGuildHouse->SerializeAllInfo( arCreate );
			}
			else
			{
				arCreate << FALSE;
				SAFE_DELETE( pGuildHouse );
			}
		}
		else
			arCreate << FALSE;
	}
	SEND( arCreate, CDPTrans::GetInstance(), dpId );
}

void CGuildHouseDBCtrl::RemoveGuildHouse( DWORD dwGuildId )
{
	if( GetQueryObject()->Execute( "usp_GuildHouse_Delete '%02d', '%06d'", g_appInfo.dwSys, dwGuildId ) )
		GuildHouseMng->RemoveGuildHouse( dwGuildId );

	BEFORESENDDUAL( arOut, PACKETTYPE_GUILDHOUSE_REMOVE, DPID_UNKNOWN, DPID_UNKNOWN );
	arOut << dwGuildId;
	SEND( arOut, CDPTrans::GetInstance(), DPID_ALLPLAYERS );

}

void CGuildHouseDBCtrl::RunDefaultPacket( CAr & ar, DPID dpId )
{
	DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> dwGuildId >> nPacketType >> nIndex;
	gfi.Serialize( ar );

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse( dwGuildId );
	if( pGuildHouse )
	{
		BOOL bResult = FALSE;
		if( ExcuteQuery( dwGuildId, pGuildHouse, nPacketType, gfi, nIndex ) )
			 bResult = pGuildHouse->OnGuildHousePacket( nPacketType, gfi, nIndex );

		if( bResult ) dpId = DPID_ALLPLAYERS;
		BEFORESENDDUAL( arOut, PACKETTYPE_GUILDHOUSE_PACKET, DPID_UNKNOWN, DPID_UNKNOWN );
		arOut << bResult << dwGuildId;
		arOut << nPacketType << nIndex;
		gfi.Serialize( arOut );
		SEND( arOut, CDPTrans::GetInstance(), dpId );
	}
}

BOOL CGuildHouseDBCtrl::ExcuteQuery( DWORD dwGuildId, CGuildHouseBase* pGuildHouse, int nPacketType, GH_Fntr_Info & gfi, int nIndex )
{
	switch( nPacketType )
	{
		case GUILDHOUSE_PCKTTYPE_LISTUP:
			if( GetQueryObject()->Execute( "usp_GuildFurniture_Insert '%02d', '%06d', %d, %d, %d, %d,\
											%f, %f, %f, %f",
											g_appInfo.dwSys, dwGuildId,	GuildHouseMng->GetSeqNum() + 1, gfi.dwItemId, gfi.tKeepTime, static_cast<int>( gfi.bSetup ),
											gfi.vPos.x, gfi.vPos.y, gfi.vPos.z, gfi.fAngle ) )
			{
				gfi.nSeqNum = GuildHouseMng->AddSeqNum();
				return TRUE;
			}
			else
				return FALSE;

		case GUILDHOUSE_PCKTTYPE_LISTDROP:
			if( nIndex >= pGuildHouse->GetFurnitureListSize() )
				return FALSE;
			gfi.nSeqNum = pGuildHouse->GetFurnitureInfo( nIndex ).nSeqNum;
			return GetQueryObject()->Execute( "usp_GuildFurniture_Delete '%02d', '%06d', %d", g_appInfo.dwSys, dwGuildId, gfi.nSeqNum );

		case GUILDHOUSE_PCKTTYPE_SETUP:
		case GUILDHOUSE_PCKTTYPE_REMOVE:
		case GUILDHOUSE_PCKTTYPE_RESET:
			if( nIndex >= pGuildHouse->GetFurnitureListSize() )
				return FALSE;
			gfi.nSeqNum = pGuildHouse->GetFurnitureInfo( nIndex ).nSeqNum;
			return GetQueryObject()->Execute(  "usp_GuildFurniture_Update '%02d', '%06d', %d, %d, %d, \
												%f, %f, %f, %f, %d",
												g_appInfo.dwSys, dwGuildId, gfi.nSeqNum, gfi.dwItemId, static_cast<int>( gfi.bSetup ),
												gfi.vPos.x, gfi.vPos.y, gfi.vPos.z, gfi.fAngle, gfi.tKeepTime );

		case GUILDHOUSE_PCKTTYPE_UPKEEP:
			return GetQueryObject()->Execute( "usp_GuildHouse_Update '%02d', '%06d', %d", g_appInfo.dwSys, dwGuildId, gfi.tKeepTime );

		case GUILDHOUSE_PCKTTYPE_EXPIRED:
			return GetQueryObject()->Execute( "usp_GuildHouse_Expired '%02d', '%06d'", g_appInfo.dwSys, dwGuildId );
	}

	return FALSE;
}


CGuildHouseDBMng::CGuildHouseDBMng()
{
	if( !m_GHDBCtrl.CreateDbHandler( SEC( 5 ) ) )
		Error( "CGuildHouseDBMng - m_GHDBCtrl.CreateDbHandler() FAILED!" );
}

CGuildHouseDBMng::~CGuildHouseDBMng()
{
	m_GHDBCtrl.CloseDbHandler();
}

CGuildHouseDBMng* CGuildHouseDBMng::GetInstance()
{
	static CGuildHouseDBMng sGHDBMng;
	return & sGHDBMng;
}
#endif // __GUILD_HOUSE