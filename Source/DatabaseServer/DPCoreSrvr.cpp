#include "stdafx.h"
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
#include "dpcoresrvr.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#include "project.h"
extern	CProject	prj;

CDPCoreSrvr::CDPCoreSrvr()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_ADD_MESSENGER, &CDPCoreSrvr::OnAddMessenger );
	ON_MSG( PACKETTYPE_DELETE_MESSENGER, &CDPCoreSrvr::OnDeleteMessenger );
	ON_MSG( PACKETTYPE_UPDATE_MESSENGER, &CDPCoreSrvr::OnUpdateMessenger );
	ON_MSG( PACKETTYPE_ADDPARTYNAME, &CDPCoreSrvr::OnAddPartyName );
	ON_MSG( PACKETTYPE_CREATE_GUILD, &CDPCoreSrvr::OnCreateGuild );
	ON_MSG( PACKETTYPE_DESTROY_GUILD, &CDPCoreSrvr::OnDestroyGuild );
	ON_MSG( PACKETTYPE_ADD_GUILD_MEMBER, &CDPCoreSrvr::OnAddGuildMember );
	ON_MSG( PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPCoreSrvr::OnRemoveGuildMember );
	ON_MSG( PACKETTYPE_CD_GUILD_NOTICE, &CDPCoreSrvr::OnGuildNotice );
	ON_MSG( PACKETTYPE_GUILD_AUTHORITY, &CDPCoreSrvr::OnGuildAuthority );
#ifdef __FL_GUILD_FINDER
	ON_MSG(PACKETTYPE_GUILD_FINDERSTATE, &CDPCoreSrvr::OnGuildFinderState);
#endif
	ON_MSG( PACKETTYPE_GUILD_SETNAME, &CDPCoreSrvr::OnGuildSetName );
	ON_MSG( PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPCoreSrvr::OnGuildMemberLv );
	ON_MSG( PACKETTYPE_GUILD_CLASS, &CDPCoreSrvr::OnGuildClass );
	ON_MSG( PACKETTYPE_GUILD_NICKNAME, &CDPCoreSrvr::OnGuildNickName );
	ON_MSG( PACKETTYPE_CHG_MASTER, &CDPCoreSrvr::OnChgMaster );

}

CDPCoreSrvr::~CDPCoreSrvr()
{

}

void CDPCoreSrvr::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_CREATEPLAYERORGROUP:
			{
				LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer	= (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
				OnAddConnection( lpCreatePlayer->dpId );
				break;
			}
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer	= (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
				OnRemoveConnection( lpDestroyPlayer->dpId );
				break;
			}
	}
}

void CDPCoreSrvr::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );

	if( pfn )
		( this->*( pfn ) )( ar, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPCoreSrvr::OnAddConnection( DPID dpid )
{
	SendPartyName( dpid );
	BEFORESEND( ar, PACKETTYPE_GLOBAL_DATA );
	g_DbManager.OpenGuild();
	g_GuildMng.Serialize( ar, FALSE );
	CGuildTable::GetInstance().Serialize( ar );
	ar.Write( (const void*)prj.m_aExpParty, sizeof(prj.m_aExpParty) );

	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnRemoveConnection( DPID dpid )
{

}

void CDPCoreSrvr::SendPartyName( DPID dpid )
{
	BEFORESEND( ar, PACKETTYPE_PARTYNAME );

	ar << (u_long)g_DbManager.m_2PartyNamePtr.size();
	for( ULONG2STRING::iterator i = g_DbManager.m_2PartyNamePtr.begin(); i != g_DbManager.m_2PartyNamePtr.end(); ++i )
	{
		ar << (*i).first;
		ar.WriteString( (*i).second.data() );
	}
	SEND( ar, this, dpid );
}

void CDPCoreSrvr::OnCreateGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnCreateGuild\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= CREATE_GUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnDestroyGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnDestroyGuild\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= DESTROY_GUILD;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnAddGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnAddGuildMember\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_GUILD_MEMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnRemoveGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnRemoveGuildMember\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= REMOVE_GUILD_MEMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildMemberLv( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnGuildMemberLv\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_MEMBER_LEVEL;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildClass( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnGuildClass\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_CLASS;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPCoreSrvr::OnGuildNickName( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnGuildNickName\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_NICKNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
void CDPCoreSrvr::OnChgMaster( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "OnChgMaster\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= QM_UPDATEGUILDMASTER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildNotice( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "D CDPCoreSrvr::OnGuildNotice\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_NOTICE;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPCoreSrvr::OnGuildAuthority( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "D CDPCoreSrvr::OnGuildAuthority\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= UPDATE_GUILD_AUTHORITY;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

#ifdef __FL_GUILD_FINDER
void CDPCoreSrvr::OnGuildFinderState(CAr& ar, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = UPDATE_GUILD_FINDERSTATE;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

void CDPCoreSrvr::OnGuildSetName( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	TRACE( "D CDPCoreSrvr::OnGuildSetName\n" );
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= GUILD_SETNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::OnAddPartyName( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= ADD_PARTYNAME;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}


void CDPCoreSrvr::SendDelPlayer( u_long idPlayer, u_long idGuild )
{
	BEFORESEND( ar, PACKETTYPE_DEL_PLAYER );
	ar << idPlayer << idGuild;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendMemberTime( u_long idPlayer, LPCTSTR tGuildMember )
{
	BEFORESEND( ar, PACKETTYPE_GUILD_MEMBERTIME );
	ar << idPlayer;
	ar.WriteString( tGuildMember );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f )
{
	BEFORESEND( ar, PACKETTYPE_QUERYSETPLAYERNAME );
	ar << idPlayer;
	ar.WriteString( lpszPlayer );
	ar << dwData << f;
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	BEFORESEND( ar, PACKETTYPE_UPDATE_PLAYER_DATA );
	ar << idPlayer;
	ar.Write( &pPlayerData->data, sizeof(sPlayerData) );
	SEND( ar, this, DPID_ALLPLAYERS );
}

void CDPCoreSrvr::OnAddMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_ADD_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::OnDeleteMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_DELETE_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::OnUpdateMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS pov	= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( pov, lpBuf, uBufSize );
	pov->nQueryMode	= QM_UPDATE_MESSENGER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &pov->Overlapped );
}

void CDPCoreSrvr::SendRemovePlayerFriend( u_long uPlayerId, u_long uFriendId )
{
	BEFORESEND( ar, PACKETTYPE_REMOVEFRIEND );
	ar << uPlayerId << uFriendId;
	SEND( ar, this, DPID_ALLPLAYERS );
}

#ifdef __V21_CLOSE_EXISTING_CONNECTION
void	CDPCoreSrvr::SendCloseExistingConnection(const char* lpszAccount, LONG lError)
{
	BEFORESEND(ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION);
	ar.WriteString(lpszAccount);
	ar << lError;
	SEND(ar, this, DPID_ALLPLAYERS);
}
#endif


#ifdef __AUTO_NOTICE
void CDPCoreSrvr::SendEventLuaNotice()
{
	BEFORESEND( ar, PACKETTYPE_EVENTLUA_NOTICE );
	vector<string> vecstrMessage;
	vecstrMessage = prj.m_EventLua.GetNoticeMessage();
	ar << vecstrMessage.size();
	for( DWORD i=0; i<vecstrMessage.size(); i++ )
	ar.WriteString( vecstrMessage[i].c_str() );
	SEND( ar, this, DPID_ALLPLAYERS );
}
#endif // __AUTO_NOTICE


CDPCoreSrvr		g_dpCoreSrvr;