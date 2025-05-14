#ifndef __DPCORESRVR_H__
#define __DPCORESRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"
#include "guild.h"

#undef	theClass
#define theClass	CDPCoreSrvr
#undef	theParameters
#define theParameters	CAr & ar, LPBYTE lpBuf, u_long uBufSize

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

class CDPCoreSrvr : public CDPMng
{
public:
//	Constructions
	CDPCoreSrvr();
	virtual	~CDPCoreSrvr();

//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );

	void	SendPartyName( DPID dpid );
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	SendDelPlayer( u_long idPlayer, u_long idGuild );
	void	SendMemberTime( u_long idPlayer, LPCTSTR tGuildMember );
	void	SendSetPlayerName( u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f );
	void	SendUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData );
#ifdef __AUTO_NOTICE
	void	SendEventLuaNotice();
#endif // __AUTO_NOTICE
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	void	SendCloseExistingConnection(const char* lpszAccount, LONG lError);
#endif

//	Handlers
	USES_PFNENTRIES;

	void	OnCreateGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnDestroyGuild( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnAddGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveGuildMember( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildMemberLv( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildClass( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildNickName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnChgMaster( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildNotice( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnGuildAuthority( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
#ifdef __FL_GUILD_FINDER
	void	OnGuildFinderState(CAr& ar, LPBYTE lpBuf, u_long uBufSize);
#endif
	void	OnGuildSetName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );

	void	OnAddMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnDeleteMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnUpdateMessenger( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
public:
	void	SendRemovePlayerFriend( u_long uPlayerId, u_long uFriendId );
private:
	void	OnAddPartyName( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
};

#endif	// __DPLOGINSRVR_H__