#ifndef __DPCACHESRVR_H__
#define __DPCACHESRVR_H__

#pragma once

#include "DPMng.h"
#include "ServerDesc.h"
#include "Player.h"
#include "guild.h"
#include "playerdata.h"

#undef	theClass
#define theClass	CDPCacheSrvr
#undef	theParameters
#define theParameters	CAr & ar, DPID, DPID, u_long

class CGuild;

class CDPCacheSrvr : public CDPMng
{
private:
	CServerDescArray	m_apServer;

public:
	CDPCacheSrvr();
	virtual	~CDPCacheSrvr();

	virtual void	SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	virtual void	UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);

	void	SendProcServerList(DPID dpid);

	void	SendHdr(DWORD dwHdr, DPID dpidCache, DPID dpidUser);
	void	SendWhisper(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch = 0);
	void	SendSay(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch = 0);
	void	SendGMSay(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, const CHAR* lpString, CPlayer* pTo);
	void	SendFriendState(CPlayer* pTo);
	void	SendFriendJoin(CPlayer* pTo, CPlayer* pFriend);
	void	SendFriendLogOut(CPlayer* pTo, u_long uidPlayer);
	void	SendSetFriendState(CPlayer* pTo);
	void	SendFriendNoIntercept(CPlayer* pTo, CPlayer* pFriend, int state);
	void	SendFriendIntercept(CPlayer* pPlayer, CPlayer* pFriend);
	void	SendFriendIntercept(CPlayer* pPlayer, u_long uFriendid);
	void	SendFriendNoIntercept(CPlayer* pTo, u_long uFriendid, int state);
	void	SendOneFriendState(CPlayer* pTo, u_long uidPlayer, DWORD dwState);
	void	SendModifyMode(DWORD dwMode, BYTE f, u_long idFrom, CPlayer* pTo);
	void	SendKillPlayer(CPlayer* pPlayer);
	void	SendGetPlayerAddr(const CHAR* lpszPlayer, const CHAR* lpAddr, CPlayer* pOperator);
	void	SendGetPlayerCount(u_short uCount, CPlayer* pOperator);
	void	SendGetCorePlayer(CPlayer* pOperator);
	void	SendSystem(const CHAR* lpString);
	void	SendCaption(const CHAR* lpString, DWORD dwWorldId, BOOL bSmall);

	void	SendDefinedText(int dwText, DPID dpidCache, DPID dpidUser, LPCSTR lpszFormat, ...);
	void	SendErrorParty(DWORD dwError, CPlayer* pPlayer);
	void	SendAddFriendNotFound(char* szMemberName, CPlayer* pLeader);
	void	SendBlock(BYTE nGu, char* szName, CPlayer* pTo);
	void	SendGameRate(FLOAT fRate, BYTE nFlag);

	void	SendSetPlayerName(u_long idPlayer, const char* lpszPlayer);
	void	SendSnoop(const char* lpszString, CPlayer* pSnoop);

	void	SendUpdatePlayerData(u_long idPlayer, PlayerData* pPlayerData, CPlayer* pTo);
	void	SendLogout(u_long idPlayer, CPlayer* pTo);
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	void	SendCloseExistingConnection(const char* lpszAccount, LONG lError);
#endif

	USES_PFNENTRIES;
	//	Handlers
	void	OnAddConnection(DPID dpid);
	void	OnRemoveConnection(DPID dpid);

	void	OnAddPlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnQueryRemovePlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);

	void	OnRemovePartyMember(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnPartyChangeTroup(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnPartyChangeName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnPartyChangeItemMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnPartyChangeExpMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);

	//	void	OnAddFriend( CAr & ar, DPID dpidCache, DPID dpidUser, u_long );
	void	OnGetFriendState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnSetFrinedState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnFriendInterceptState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnRemoveFriend(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
public:
	void	OnDestroyGuild(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnRemoveGuildMember(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnGuildMemberLv(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnGuildAuthority(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
	void	OnGuildSetName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
	void	OnGuildClass(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
	void	OnGuildNickName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
	void	OnChgMaster(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
	void	SendAddGuildMember(const GUILD_MEMBER_INFO& info, const char* lpszPlayer, u_long idGuild, CPlayer* pPlayer);
public:
	void	SendRemoveGuildMember(u_long idPlayer, u_long idGuild, CPlayer* pPlayer);
	//private:
	void	SendGuildMemberLv(u_long idPlayer, int nMemberLv, CPlayer* pPlayer);
	void	SendGuildSetName(u_long idGuild, const char* szName);
	void	SendGuildClass(u_long idPlayer, int nClass, CPlayer* pPlayer);
	void	SendGuildNickName(u_long idPlayer, LPCTSTR strNickName, CPlayer* pPlayer);
#ifndef __FL_GUILD_FINDER
	void	SendChgMaster(u_long idPlayer, u_long idPlayer2, CPlayer* pPlayer);
#else
	void	SendChgMaster(u_long idGuild, u_long idPlayer, u_long idPlayer2);
	void	OnGuildFinderState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
#endif
public:
	void	SendGuild(CGuild* pGuild, CPlayer* pPlayer);
	void	SendGuildChat(const char* lpszPlayer, const char* sChat, CPlayer* pPlayer, OBJID objid);
	void	SendGuildMemberLogin(CPlayer* pTo, BYTE nLogin, u_long uPlayerId, u_long uMultiNo);
	void	SendGuildMemberGameJoin(CPlayer* pTo, int nMaxLogin, u_long uLoginPlayerId[], u_long uLoginGuildMulti[]);
	void	SendGuildError(CPlayer* pTo, int nError);
	void	SendGuildMsgControl(GUILD_MSG_HEADER* pHeader, DWORD pPenya, BYTE cbCloak);

private:
	void	OnPartyChangeLeader(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
#ifdef __FL_PARTY_SORT
	void	OnSortPartyMembers(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize);
#endif
#ifdef __VALID_CONNECTION_CONFORMATION
public:
	void	SendConfirmPlayer(DPID dpidCache, DWORD serial, bool state);
private:
	void	OnConfirmPlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
#endif
#ifdef __FL_PARTY_FINDER
	void	OnPartyChangeJoinMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long);
#endif
};

inline void CDPCacheSrvr::SendHdr(DWORD dwHdr, DPID dpidCache, DPID dpidUser)
{
	BEFORESENDSOLE(ar, dwHdr, dpidUser);
	SEND(ar, this, dpidCache);
}

#endif	// __DPCACHESRVR_H__