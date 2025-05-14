#ifndef __DPDATABASECLIENT_H__
#define	__DPDATABASECLIENT_H__

#include "DPMng.h"
#include "msghdr.h"
#include "misc.h"
#include "guild.h"

#undef	theClass
#define theClass	CDPDatabaseClient
#undef theParameters
#define theParameters	CAr & ar


class CDPDatabaseClient : public CDPMng
{
public:
//	Constructions
	CDPDatabaseClient();
	virtual	~CDPDatabaseClient();
//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

	void	SendCreateGuild( GUILD_MEMBER_INFO* info, int nSize, u_long idGuild, const char* szGuild );
	void	SendDestroyGuild( u_long idGuild, u_long idMaster );
	void	SendAddGuildMember( u_long idPlayer, u_long idGuild, u_long DoidPlayer );
	void	SendRemoveGuildMember( u_long idPlayer, u_long idGuild, u_long DoidPlayer );
	void	SendGuildMemberLv( u_long idPlayer, int nMemberLv );
	void	SendGuildClass( u_long idPlayer, int nClass );
	void	SendGuildNickName( u_long idPlayer, LPCTSTR strNickName );
	void	SendChgMaster( u_long idPlayer, u_long idPlayer2 );
	void	SendGuildContribution( CONTRIBUTION_CHANGED_INFO & info );
	void	SendGuildNotice( u_long idGuild, LPCTSTR szNotice );
	void	SendGuildAuthority( u_long idGuild, DWORD adwAuthority[] );
#ifdef __FL_GUILD_FINDER
	void	SendGuildFinderState(u_long idGuild, BOOL bFinder);
#endif
	void	SendGuildSetName( u_long uidGuild, char * szName );
	void	QueryAddMessenger( u_long idPlayer, u_long idFriend );
	void	QueryDeleteMessenger( u_long idPlayer, u_long idFriend );
	void	QueryUpdateMessenger( u_long idPlayer, u_long idFriend, BOOL bBlock );
	void	OnRemovePlayerFriend( CAr & ar );
	void	SendAddPartyName( u_long uidPlayer, const char * sParty );
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	void	OnCloseExistingConnection(CAr& ar);
#endif

	USES_PFNENTRIES;
	void	OnPartyName( CAr & ar );
	void	OnGlobalData( CAr & ar );
	void	OnDelPlayer( CAr & ar );
	void	OnGuildMemberTime( CAr & ar );
	void	OnSetPlayerName( CAr & ar );
	void	OnUpdatePlayerData( CAr & ar );
#ifdef __AUTO_NOTICE
	void	OnEventNotice( CAr & ar );
#endif // __AUTO_NOTICE
};


#endif	// __DPDATABASECLIENT_H__