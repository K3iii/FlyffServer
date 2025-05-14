#include "stdafx.h"
#include "player.h"
#include "dpcachesrvr.h"
#include "DPCoreSrvr.h"
#include "dpdatabaseclient.h"


#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "guild.h"
extern	CGuildMng			g_GuildMng;
#include "guildwar.h"

extern	CPlayerMng			g_PlayerMng;
extern	CDPCacheSrvr		g_DPCacheSrvr;
extern  CDPCoreSrvr			g_dpCoreSrvr;

CDPDatabaseClient::CDPDatabaseClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_PARTYNAME, &CDPDatabaseClient::OnPartyName );
	ON_MSG( PACKETTYPE_GLOBAL_DATA, &CDPDatabaseClient::OnGlobalData );
	ON_MSG( PACKETTYPE_DEL_PLAYER, &CDPDatabaseClient::OnDelPlayer );
	ON_MSG( PACKETTYPE_GUILD_MEMBERTIME, &CDPDatabaseClient::OnGuildMemberTime );
	ON_MSG( PACKETTYPE_QUERYSETPLAYERNAME, &CDPDatabaseClient::OnSetPlayerName );
	ON_MSG( PACKETTYPE_UPDATE_PLAYER_DATA, &CDPDatabaseClient::OnUpdatePlayerData );
#ifdef __AUTO_NOTICE
	ON_MSG( PACKETTYPE_EVENTLUA_NOTICE, &CDPDatabaseClient::OnEventNotice );
#endif // __AUTO_NOTICE
	ON_MSG( PACKETTYPE_REMOVEFRIEND, &CDPDatabaseClient::OnRemovePlayerFriend );
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	ON_MSG(PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPDatabaseClient::OnCloseExistingConnection);
#endif
}

CDPDatabaseClient::~CDPDatabaseClient()
{

}

void CDPDatabaseClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			break;
		case DPSYS_DESTROYPLAYERORGROUP:
			break;
	}
}

void CDPDatabaseClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );

	GETTYPE( ar );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn ) 
	{
		( this->*( pfn ) )( ar );
	}
}

void CDPDatabaseClient::OnPartyName( CAr & ar )
{
	u_long uSize;
	u_long uidPlayer;
	char szPartyName[36];

	ar >> uSize;
	for( DWORD i = 0 ; i < uSize ; i++ )
	{
		ar >> uidPlayer;
		ar.ReadString( szPartyName, 36 );
		g_PartyMng.AddPartyName( uidPlayer, szPartyName );
	}
}

void CDPDatabaseClient::SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, u_long idGuild, const char* szGuild )
{
	BEFORESEND( ar, PACKETTYPE_CREATE_GUILD );
	ar << nSize;
	ar.Write( info, sizeof(GUILD_MEMBER_INFO)*nSize );
	ar << idGuild;
	ar.WriteString( szGuild );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendDestroyGuild( u_long idGuild, u_long idMaster )
{
	BEFORESEND( ar, PACKETTYPE_DESTROY_GUILD );
	ar << idGuild;
	ar << idMaster;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendAddGuildMember( u_long idPlayer, u_long idGuild, u_long DoidPlayer )
{
	BEFORESEND( ar, PACKETTYPE_ADD_GUILD_MEMBER );
	ar << idPlayer << idGuild << DoidPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendRemoveGuildMember( u_long idPlayer, u_long idGuild, u_long DoidPlayer )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_GUILD_MEMBER );
	ar << idPlayer << idGuild << DoidPlayer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildMemberLv( u_long idPlayer, int nMemberLv )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_MEMBER_LEVEL );
	ar << idPlayer << nMemberLv;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildClass( u_long idPlayer, int nClass )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_CLASS );
	ar << idPlayer << nClass;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildNickName( u_long idPlayer, LPCTSTR strNickName )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_NICKNAME );
	ar << idPlayer;
	ar.WriteString( strNickName );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendChgMaster( u_long idPlayer, u_long idPlayer2 )
{
	BEFORESEND( ar, PACKETTYPE_CHG_MASTER );
	ar << idPlayer << idPlayer2;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildNotice( u_long idGuild, LPCTSTR szNotice )
{
	BEFORESEND( ar, PACKETTYPE_CD_GUILD_NOTICE );
	ar << idGuild;
	ar.WriteString( szNotice );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendGuildAuthority( u_long idGuild, DWORD adwAuthority[] )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_AUTHORITY );
	ar << idGuild;
	ar.Write( adwAuthority, sizeof(DWORD) * MAX_GM_LEVEL );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __FL_GUILD_FINDER
void CDPDatabaseClient::SendGuildFinderState(u_long uGuildId, BOOL bFinder)
{
	BEFORESEND(ar, PACKETTYPE_GUILD_FINDERSTATE);
	ar << uGuildId << bFinder;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif


void CDPDatabaseClient::SendGuildSetName( u_long uidGuild, char * szName )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_SETNAME );
	ar << uidGuild;
	ar.WriteString( szName );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::SendAddPartyName( u_long uidPlayer, const char * sParty )
{
	BEFORESEND( ar, PACKETTYPE_ADDPARTYNAME );
	ar << uidPlayer;
	ar.WriteString( sParty );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnGlobalData( CAr & ar )
{
	g_GuildMng.Serialize( ar, FALSE );
	CGuildTable::GetInstance().Serialize( ar );
	ar.Read( (void*)g_PartyMng.m_aExpParty, sizeof(g_PartyMng.m_aExpParty) );

	extern DWORD   g_dwId;
	NotifyProcessStart( g_dwId );
}

void CDPDatabaseClient::OnDelPlayer( CAr & ar )
{
	u_long idGuild, idPlayer;
	ar >> idPlayer >> idGuild;

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );

	CGuild* pGuild	= g_GuildMng.GetGuild( idGuild );

	if( NULL == pGuild )
		return;
	if( FALSE == pGuild->IsMember( idPlayer ) )
		return;

	if( pGuild->IsMaster( idPlayer ) )	// master
	{
		CGuildMember* pMember;
		CPlayer* pPlayer;
		for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i )
		{
			pMember		= i->second;
			pPlayer	= g_PlayerMng.GetPlayer( pMember->m_idPlayer );
			if( pPlayer )
			{
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
				pPlayer->m_tGuildMember = CTime::GetCurrentTime();
				pPlayer->m_tGuildMember += CTimeSpan( 2, 0, 0, 0 );
#endif
				pPlayer->m_idGuild	= 0;
			}
		}
		g_GuildMng.RemoveGuild( idGuild );
		SendDestroyGuild( idGuild, idPlayer );	// databaseserver
		g_dpCoreSrvr.SendDestroyGuild( idGuild );	// worldserver
	}
	else	// member
	{
		if( pGuild->RemoveMember( idPlayer ) )
		{
			CPlayer* pPlayer;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
				i != pGuild->m_mapPMember.end(); ++i )
			{
				pPlayer	= g_PlayerMng.GetPlayer( i->second->m_idPlayer );
				if( pPlayer )
					g_DPCacheSrvr.SendRemoveGuildMember( idPlayer, pGuild->m_idGuild, pPlayer );
			}
			g_dpCoreSrvr.SendRemoveGuildMember( idPlayer, idGuild );	// worldserver
			SendRemoveGuildMember( idPlayer, idGuild, pGuild->m_idMaster );		// databaseserver
		}
	}
}

void CDPDatabaseClient::OnGuildMemberTime( CAr & ar )
{
	u_long idPlayer;
	char GuildMemberTime[15] = {0,};
	ar >> idPlayer;
	ar.ReadString( GuildMemberTime, 15 );

	char cYear[5] = {0,};
	char cMonth[3] = {0,};
	char cDay[3] = {0,};
	char cHour[3] = {0,};
	char cMin[3] = {0,};

	strncpy(cYear, GuildMemberTime, 4);
	strncpy(cMonth, GuildMemberTime + 4, 2 );
	strncpy(cDay, GuildMemberTime + 6, 2 );
	strncpy(cHour, GuildMemberTime + 8, 2 );
	strncpy(cMin, GuildMemberTime + 10, 2 );

	CTime tGuildMember( atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0 );

	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	if( pPlayer )
	{
		pPlayer->m_tGuildMember = tGuildMember;
	}
}

void CDPDatabaseClient::OnSetPlayerName( CAr & ar )
{
	OutputDebugString( "CORESERVER.EXE\tPACKETTYPE_QUERYSETPLAYERNAME" );
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER]	= { 0, };
	DWORD dwData;
	BOOL f;
	ar >> idPlayer;
	ar.ReadString( lpszPlayer, MAX_PLAYER );
	ar >> dwData >> f;
	if( f )
	{
		CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
		CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
		if( pPlayer )
		{
			lstrcpy( pPlayer->lpszPlayer, lpszPlayer );
		}
		g_DPCacheSrvr.SendSetPlayerName( idPlayer, lpszPlayer );
	}
	g_dpCoreSrvr.SendSetPlayerName( idPlayer, lpszPlayer, dwData, f );
}

void CDPDatabaseClient::OnUpdatePlayerData( CAr & ar )
{
	u_long idPlayer;
	PlayerData pd;
	ar >> idPlayer;
	ar.Read( &pd.data, sizeof(sPlayerData) );
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );
	CPlayer* pPlayer	= g_PlayerMng.GetPlayer( idPlayer );
	map<u_long, int>	mapIdPlayer;
	if( pPlayer )
	{

		{
			CMclAutoLock	Lock2( g_GuildMng.m_AddRemoveLock );
			CGuild* pGuild	= g_GuildMng.GetGuild( pPlayer->m_idGuild );
			if( pGuild && pGuild->IsMember( idPlayer ) )
			{
				for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i )
					mapIdPlayer.insert( map<u_long, u_long>::value_type( i->second->m_idPlayer, 0 ) );
			}
		}

		{
			CMclAutoLock	Lock3( g_PartyMng.m_AddRemoveLock );
			CParty* pParty	= g_PartyMng.GetParty( pPlayer->m_uPartyId );
			if( pParty && pParty->IsMember( idPlayer ) )
			{
				for( int i = 0; i < pParty->m_nSizeofMember; i++ )
					mapIdPlayer.insert( map<u_long, u_long>::value_type( pParty->m_aMember[i].m_uPlayerId, 0 ) );
			}
		}

		{
			pPlayer->Lock();
			for( map<u_long, Friend>::iterator i = pPlayer->m_RTMessenger.begin(); i != pPlayer->m_RTMessenger.end(); ++i )
				mapIdPlayer.insert( map<u_long, u_long>::value_type( i->first, 0 ) );
			pPlayer->Unlock();
		}
		for( map<u_long, int>::iterator i = mapIdPlayer.begin(); i != mapIdPlayer.end(); ++i )
		{
			CPlayer* pTo	= g_PlayerMng.GetPlayer( i->first );
			if( pTo )
				g_DPCacheSrvr.SendUpdatePlayerData( idPlayer, &pd, pTo );
		}
	}
}

void CDPDatabaseClient::QueryAddMessenger( u_long idPlayer, u_long idFriend )
{
	BEFORESEND( ar, PACKETTYPE_ADD_MESSENGER );
	ar << idPlayer << idFriend;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::QueryDeleteMessenger( u_long idPlayer, u_long idFriend )
{
	BEFORESEND( ar, PACKETTYPE_DELETE_MESSENGER );
	ar << idPlayer << idFriend;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::QueryUpdateMessenger( u_long idPlayer, u_long idFriend, BOOL bBlock )
{
	BEFORESEND( ar, PACKETTYPE_UPDATE_MESSENGER );
	ar << idPlayer << idFriend << bBlock;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPDatabaseClient::OnRemovePlayerFriend( CAr & ar )
{
	u_long uPlayerId, uFriendId;
	ar >> uPlayerId >> uFriendId;

	CPlayer* pFriendUser;
	CMclAutoLock	Lock( g_PlayerMng.m_AddRemoveLock );

	pFriendUser = g_PlayerMng.GetPlayer( uFriendId );
	if( pFriendUser )
	{
		pFriendUser->Lock();
		pFriendUser->m_RTMessenger.RemoveFriend( uPlayerId );
		pFriendUser->Unlock();
		g_dpCoreSrvr.SendRemoveFriend( pFriendUser->uKey, uPlayerId );
	}
}

#ifdef __AUTO_NOTICE
void CDPDatabaseClient::OnEventNotice( CAr & ar )
{
	int nSize = 0;
	char szTemp[255] = {0,};
	vector<string> vecstrMessage;
	ar >> nSize;
	for( int i=0; i<nSize; i++ )
	{
		ar.ReadString( szTemp, 254 );
		g_DPCacheSrvr.SendSystem( szTemp );
	}
}
#endif // __AUTO_NOTICE

#ifdef __V21_CLOSE_EXISTING_CONNECTION
void	CDPDatabaseClient::OnCloseExistingConnection(CAr& ar)
{
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	LONG lError = 0;

	ar.ReadString(lpszAccount, _countof(lpszAccount));
	ar >> lError;

	g_DPCacheSrvr.SendCloseExistingConnection(lpszAccount, lError);
}
#endif

CDPDatabaseClient	g_dpDatabaseClient;