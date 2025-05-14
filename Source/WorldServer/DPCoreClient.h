#ifndef __DPCORECLIENT_H__
#define __DPCORECLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"
#include "WorldServer.h"
#include "misc.h"

#include "guild.h"

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"
#endif // __INSTANCE_DUNGEON

#define BEFOREPASS( ar, dw, wWorldSrvr, wCacheSrvr, dpid, objid )	\
	BEFORESENDDUAL( ar, PACKETTYPE_PASSAGE, MAKELONG( wWorldSrvr, wCacheSrvr ), dpid );	\
	ar << objid << dw;

#define PASS( ar )	\
	SEND( ar, this, DPID_SERVERPLAYER );

#define	BEFOREBROADCAST( ar, dw )	\
	BEFORESENDDUAL( ar, PACKETTYPE_BROADCAST, DPID_UNKNOWN, DPID_UNKNOWN );	\
	ar << dw;

#define	BROADCAST( ar )	\
	SEND( ar, this, DPID_SERVERPLAYER );


#undef	theClass
#define theClass	CDPCoreClient
#undef theParameters
#define theParameters CAr & ar, DPID, DPID, OBJID objid

class CDPCoreClient : public CDPMng
{
private:
	WSAEVENT	m_hWait;
	u_long		m_uRecharge;
	BOOL		m_bAlive;
private:
	void	MyRegister( u_long uKey );
	void	Recharge( u_long uBlockSize );
	BOOL	Contribute( CUser* pUser, DWORD dwPxpCount, DWORD dwPenya );

public:

public:
//	Constructions
	CDPCoreClient();
	virtual	~CDPCoreClient();
//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	BOOL	Run( LPSTR lpszAddr, USHORT uPort, u_long uKey );
	BOOL	CheckIdStack( void );

	void	SendJoin( u_long idPlayer, const char* szPlayer, BOOL bOperator );
	void	SendSystemMessage( LPCTSTR lpsz );
//	Operator commands
	void	SendWhisper( u_long idFrom, u_long idTo, const CHAR* lpString );

	void	SendSay( u_long idFrom, u_long idTo, const CHAR* lpString );
	void	SendModifyMode( DWORD dwMode, BYTE fAdd, u_long idFrom, u_long idTo );
	void	SendShout( CUser* pUser, const CHAR* lpString );
	void	SendPartyChat( CUser* pUser, const CHAR* lpString );
	void	SendUserPartySkill( u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint , int nCachMode );
	void	SendGMSay( u_long idPlayer, DWORD dwWorldID, const CHAR* lpString );
	void	SendPlayMusic( DWORD dwWorldID, u_long idmusic );
	void	SendPlaySound( DWORD dwWorldID, u_long idsound );
	void	SendKillPlayer( u_long idOperator, u_long idPlayer );
	void	SendGetPlayerAddr( u_long idOperator, u_long idPlayer );
	void	SendGetPlayerCount( u_long idOperator );
	void	SendGetCorePlayer( u_long idOperator );
	void	SendSystem( const CHAR* lpString );
	void	SendToServer( LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser );

	void	SendCaption( const CHAR* lpString, DWORD dwWorldId = 0, BOOL bSmall = FALSE );

	void	SendEventRealItem( u_long uIdPlayer, int nRealItemIndex, int nRealItemCount );
	void	SendPartyLevel( CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp );
	void	SendPartyMemberFlightLevel( CUser* pUser );
	void	SendAddPartyExp( u_long uPartyId, int nMonLv, BOOL bSuperLeader , BOOL bLeaderSMExpUp );
	void	SendRemovePartyPoint( u_long uPartyId, int nRemovePoint );
	void	SendGameRate( FLOAT fRate, BYTE nFlag );
	void	SendLoadConstant();
	void	SendSetMonsterRespawn( u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying );
	void	SendFallSnow();
	void	SendFallRain();
	void	SendStopSnow();
	void	SendStopRain();
	void	SendGuildCombatState( int nState );
	void	SendGCRemoveParty( u_long uidPartyid, u_long uidPlayer );
	void	SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, const char* szGuild );
	void	SendGuildChat( CUser* pUser, const char* sChat );
	BOOL	SendGuildStat(  CUser* pUser, GUILD_STAT stat, DWORD data );
	void	SendGuildMsgControl_Bank_Penya( CUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak );
	void	SendBlock( BYTE nGu, u_long uidPlayerTo, char *szNameTo, u_long uidPlayerFrom );
	void	SendQuerySetGuildName( u_long idPlayer, u_long idGuild, const char* lpszGuild, BYTE nId );
	void	SendSetSnoop( u_long idPlayer, u_long idSnoop, BOOL bRelease  );
	void	OnSetSnoop( CAr & ar, DPID, DPID, OBJID );
	void	SendChat( u_long idPlayer1, u_long idPlayer2, const char* lpszChat );
	void	SendPing( void );
	void	OnDestroyPlayer( CAr & ar, DPID, DPID, OBJID );
private:
	void	OnInstanceDungeonAllInfo( CAr & ar, DPID, DPID, OBJID );
	void	OnInstanceDungeonCreate( CAr & ar, DPID, DPID, OBJID );
	void	OnInstanceDungeonDestroy( CAr & ar, DPID, DPID, OBJID );
	void	OnInstanceDungeonSetCoolTimeInfo( CAr & ar, DPID, DPID, OBJID );
	void	OnInstanceDungeonDeleteCoolTimeInfo( CAr & ar, DPID, DPID, OBJID );
public:
	void	SendInstanceDungeonCreate( int nType, DWORD dwDungeonId, ID_INFO & ID_Info );
	void	SendInstanceDungeonDestroy( int nType, DWORD dwDungeonId, ID_INFO & ID_Info );
	void	SendInstanceDungeonSetCoolTimeInfo( int nType, DWORD dwPlayerId, COOLTIME_INFO & CT_Info );
#ifdef __VALID_CONNECTION_CONFORMATION
	void	SendConfirmPlayer(DWORD serial, bool state);
#endif

protected:
	// Handlers
	USES_PFNENTRIES;

	void	OnLoadWorld( CAr & ar, DPID, DPID, OBJID );
	void	OnQueryTickCount( CAr & ar, DPID, DPID, OBJID );
	void	OnRecharge( CAr & ar, DPID, DPID, OBJID );
	void	OnModifyMode( CAr & ar, DPID, DPID, OBJID objid );

	void	OnSetPartyExp( CAr & ar, DPID, DPID, OBJID objid );
	void	OnRemovePartyPoint( CAr & ar, DPID, DPID, OBJID objid );
	void	OnPartyChangeTroup( CAr & ar, DPID, DPID, OBJID );
	void	OnPartyChangeName( CAr & ar, DPID, DPID, OBJID );
	void	OnAddFriend( CAr & ar, DPID, DPID, OBJID );
	void	OnRemovefriend( CAr & ar, DPID, DPID, OBJID );
	void	OnShout( CAr & ar, DPID, DPID, OBJID );
	void	OnPlayMusic( CAr & ar, DPID, DPID, OBJID );
	void	OnPlaySound( CAr & ar, DPID, DPID, OBJID );
	void	OnErrorParty( CAr & ar, DPID, DPID, OBJID );
	void	OnAddPartyMember( CAr & ar, DPID, DPID, OBJID );
	void	OnRemovePartyMember( CAr & ar, DPID, DPID, OBJID );
	void	OnAddPlayerParty( CAr & ar, DPID, DPID, OBJID );
	void	OnRemovePlayerParty( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildMemberLv( CAr & ar, DPID, DPID, OBJID );
	void	OnSetPartyMode( CAr & ar, DPID, DPID, OBJID );
	void	OnPartyChangeItemMode( CAr & ar, DPID, DPID, OBJID );
	void	OnPartyChangeExpMode( CAr & ar, DPID, DPID, OBJID );

	void	OnEnvironmentEffect( CAr & ar, DPID, DPID, OBJID );

	void	OnPartyChat( CAr & ar , DPID, DPID, OBJID );
	void	OnCreateGuild( CAr & ar, DPID, DPID, OBJID );
	void	OnDestroyGuild( CAr & ar, DPID, DPID, OBJID );
	void	OnAddGuildMember( CAr & ar, DPID, DPID, OBJID );
	void	OnRemoveGuildMember( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildClass( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildNickName( CAr & ar, DPID, DPID, OBJID );
	void	OnChgMaster( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildMemberLogOut( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildContributionACK( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildNoticeACk( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildAuthority( CAr & ar, DPID, DPID, OBJID );
#ifdef __FL_GUILD_FINDER
	void	OnGuildFinderState(CAr& ar, DPID, DPID, OBJID);
#endif
	void	OnGuildSetName( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildMsgControl( CAr & ar, DPID, DPID, OBJID );

	void	OnSetPointParam( CAr & ar, DPID, DPID, OBJID );
	void	OnFriendInterceptState( CAr & ar, DPID, DPID, OBJID );
	void	OnSetFriendState( CAr & ar, DPID, DPID, OBJID );
	void	OnPartyChangeLeader( CAr & ar, DPID, DPID, OBJID );

	void	OnLoadConstant( CAr & ar, DPID, DPID, OBJID );
	void	OnGameRate( CAr & ar, DPID, DPID, OBJID );
	void	OnSetMonsterRespawn( CAr & ar, DPID, DPID, OBJID );
	void	OnSetPlayerName( CAr & ar, DPID, DPID, OBJID );
	void	OnGuildCombatState( CAr & ar, DPID, DPID, DPID );
	void	OnRemoveUserFromCORE( CAr & ar, DPID, DPID, DPID );
	void	OnPing( CAr & ar, DPID, DPID, DPID );
	void	OnUpdatePartySkills(CAr& ar, DPID, DPID, OBJID);
#ifdef __FL_PARTY_SORT
	void	OnSortPartyMembers(CAr& ar, DPID, DPID, OBJID);
#endif
#ifdef __FL_GW_STATUE_V2
	void	OnStatueSettings(CAr& ar, DPID, DPID, OBJID objid);
#endif
#ifdef __VALID_CONNECTION_CONFORMATION
	void	OnConfirmPlayer(CAr& ar, DPID, DPID, DPID);
#endif
#ifdef __FL_PARTY_FINDER
	void	OnPartyChangeJoinMode(CAr& ar, DPID, DPID, OBJID);
#endif
};

#endif	// __DPCORECLIENT_H__