#ifndef __DPCORESRVR_H__
#define __DPCORESRVR_H__

#include "DPMng.h"
#include "msghdr.h"
#include "ServerDesc.h"
#include "ObjMap.h"
#include "guild.h"

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"
#endif // __INSTANCE_DUNGEON

#undef	theClass
#define theClass	CDPCoreSrvr
#undef	theParameters
#define theParameters	CAr & ar, DPID, DPID, DPID, u_long

class CDPCoreSrvr : public CDPMng
{
public:
	int					m_nGCState;
public:
	CServerDescArray	m_apSleepServer;
	CServerDescArray	m_apServer;	// active
#ifdef __STL_0402
	map<u_long, DPID>	m_toHandle;	// key to dpid
#else	// __STL_0402
	CMyMap<DPID>	m_toHandle;	// key to dpid
#endif	// __STL_0402
	CMclCritSec		m_AccessLock;
	CObjMap		m_objmap;
public:
	// Constructions
	CDPCoreSrvr();
	virtual	~CDPCoreSrvr();

	// Operations
	virtual void	SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	virtual void	UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);

	void	SendRecharge(u_long uBlockSize, DPID dpid);
	void	SendDuplicate(u_long uWorldSrvr);
	void	SendUnify(u_long uWorldSrvr);
	void	SendQueryTickCount(DWORD dwTime, DPID dpid, double dCurrentTime);
	//	Operator commands
	void	SendShout(u_long idPlayer, const CHAR* lpString, DPID dpid);
	void	SendPlayMusic(u_long idmusic, DWORD dwWorldID, DPID dpid);
	void	SendPlaySound(u_long idsound, DWORD dwWorldID, DPID dpid);

	void	SendSetPartyMode(u_long idParty, int nMode, BOOL bOnOff, LONG nPoint = 0, DWORD dwSkillTime = 0);
	void	SendRemoveParty(u_long idParty, u_long idLeader, u_long idMember);
	void	SendPartyChangeTroup(u_long idParty, const char* szPartyName);
	void	SendPartyChangeName(u_long idParty, const char* szPartyName);
	void	SendPartyChangeItemMode(u_long idParty, int nItemMode);
	void	SendPartyChangeExpMode(u_long idParty, int nExpMode);

	void	SendUpdatePartySkills(u_long uPlayerID, u_long uPartyId, int* nModeTime, DPID dpid);
	void	SendAddPartyMember(const u_long uPartyId, const u_long idLeader, const u_long idMember, int* nModeTime);


	void	SendAddFriend(u_long uidSender, u_long uidFriend);
	void	SendRemoveFriend(u_long uidSender, u_long uidFriend);
	void	SendEnvironmentEffect();
	void	SendPartyChat(u_long idParty, const CHAR* lpName, const CHAR* lpString, OBJID objid);
	void	SendSetPlayerName(u_long idPlayer, const char* lpszPlayer, DWORD dwData, BOOL f);
	void	SendDestroyPlayer(u_long idPlayer, DPID dpid);
	void	SendGuildCombatState(void);
	void	SendSetSnoop(u_long idPlayer, u_long idSnoop, BOOL bRelease);

	// Handlers
	USES_PFNENTRIES;
	void	OnAddConnection(CAr& ar, DPID dpid, DPID, DPID, u_long);
	void	OnRemoveConnection(DPID dpid);
	void	OnRecharge(CAr& ar, DPID dpid, DPID, DPID, u_long);
	void	OnJoin(CAr& ar, DPID dpid, DPID, DPID, u_long);		// idPlayer, szPlayer, bOperator

//	Operator commands
	void	OnWhisper(CAr& ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long);
	void	OnSay(CAr& ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long);
	void	OnModifyMode(CAr& ar, DPID dpidFrom, DPID dpidCache, DPID dpidUser, u_long);
	void	OnShout(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGMSay(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnPlayMusic(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnPlaySound(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnKillPlayer(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGetPlayerAddr(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGetPlayerCount(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGetCorePlayer(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnSystem(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnCaption(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnAddPartyExp(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnRemovePartyPoint(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnFallSnow(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnFallRain(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnStopSnow(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnStopRain(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnPartySkillUse(CAr& ar, DPID, DPID, DPID, u_long uBufSize);
	void	OnPartyChat(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnPartyLevel(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnLoadConstant(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGameRate(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnSetMonsterRespawn(CAr& ar, DPID, DPID, DPID, u_long);

	void    OnGuildContribution(CAr& ar, DPID, DPID, DPID, u_long);
	void    OnGuildNotice(CAr& ar, DPID, DPID, DPID, u_long);

	void	OnSetSnoop(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnChat(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnPing(CAr& ar, DPID dpid, DPID, DPID, u_long);

	void	OnGCState(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGCRemoveParty(CAr& ar, DPID, DPID, DPID, u_long);

private:
	DPID	GetWorldSrvrDPID(u_long uWorldSrvr);
private:
	DPID	GetWorldSrvrDPID(u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3& vPos);
	u_long	GetIdofMulti(DPID dpid);

public:
	void	OnGuildMsgControl(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnCreateGuild(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnGuildChat(CAr& ar, DPID, DPID, DPID, u_long);
	void	SendCreateGuild(GUILD_MEMBER_INFO* pInfo, int nSize, u_long idGuild, const char* szGuild);
	void	SendDestroyGuild(u_long idGuild);
	void	SendAddGuildMember(const GUILD_MEMBER_INFO& info, u_long idGuild);
	void	SendRemoveGuildMember(u_long idMember, u_long idGuild);
	void	SendGuildMemberLv(u_long idPlayer, int nMemberLv, u_long idGuild);

	void	SendGuildClass(u_long idPlayer, int nClass, u_long idGuild);
	void	SendGuildNickName(u_long idPlayer, LPCTSTR strNickName, u_long idGuild);
	void	SendGuildMemberLogOut(u_long idGuild, u_long idPlayer);
	void	SendGuildAuthority(u_long uGuildId, DWORD dwAuthority[]);
#ifdef __FL_GUILD_FINDER
	void	SendGuildFinderState(u_long uGuildId, BOOL bFinder);
#endif
	void	SendChgMaster(u_long idPlayer, u_long idPlayer2, u_long idGuild);
	void	SendGuildSetName(u_long idGuild, const char* lpszGuild, u_long idPlayer = 0, BYTE nId = 0xff, BYTE nError = 0);

public:
	void	SendSetFriendState(u_long uidPlayer, DWORD dwState);
	void	SendChangeLeader(u_long uPartyId, u_long uidChangeLeader);
	void	SendFriendInterceptState(u_long uIdPlayer, u_long uIdFriend);

	void	OnBlock(CAr& ar, DPID, DPID, DPID, u_long);

	void	OnQuerySetGuildName(CAr& ar, DPID, DPID, DPID, u_long);
	void	SendRemoveUser(DWORD dwSerial);
public:
	void	SendInstanceDungeonAllInfo(int nType, CInstanceDungeonBase* pID, DPID dpId);
	void	SendInstanceDungeonCreate(int nType, DWORD dwDungeonId, ID_INFO& ID_Info);
	void	SendInstanceDungeonDestroy(int nType, DWORD dwDungeonId, ID_INFO& ID_Info);
	void	SendInstanceDungeonSetCoolTimeInfo(ULONG uKey, int nType, DWORD dwPlayerId, COOLTIME_INFO CT_Info);
	void	SendInstanceDungeonDeleteCoolTimeInfo(int nType, DWORD dwPlayerId);
private:
	void	OnInstanceDungeonCreate(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnInstanceDungeonDestroy(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnInstanceDungeonSetCoolTimeInfo(CAr& ar, DPID, DPID, DPID, u_long);

	void	OnAddPartyMember(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnAddFriend(CAr& ar, DPID, DPID, DPID, u_long);
	void	OnAddGuildMember(CAr& ar, DPID, DPID, DPID, u_long);
#ifdef __FL_GW_STATUE_V2
	void	OnStatueSettings(CAr& ar, DPID, DPID, DPID, u_long);
#endif

#ifdef __FL_PARTY_SORT
public:
	void	SendChangePosition(u_long uPartyId, u_long uidChangeMember, BOOL bUpDown);
#endif
#ifdef __VALID_CONNECTION_CONFORMATION
public:
	void SendConfirmPlayer(CPlayer* pPlayer);
private:
	void OnConfirmPlayer(CAr& ar, DPID, DPID, DPID, u_long);
#endif
#ifdef __FL_PARTY_FINDER
	void	SendPartyChangeJoinMode(u_long idParty, BOOL bMode);
#endif
};

inline DPID CDPCoreSrvr::GetWorldSrvrDPID(u_long uWorldSrvr)
{
#ifdef __STL_0402
	map<u_long, DPID>::iterator i = m_toHandle.find(uWorldSrvr);
	if (i != m_toHandle.end())
		return i->second;
	return DPID_UNKNOWN;
#else	// __STL_0402
	DPID dpid;
	if (m_toHandle.Lookup(uWorldSrvr, dpid))
		return dpid;
	return DPID_UNKNOWN;
#endif	// __STL_0402
}

inline DPID CDPCoreSrvr::GetWorldSrvrDPID(u_long uIdofMulti, DWORD dwWorldID, const D3DXVECTOR3& vPos)
{
	if (uIdofMulti == NULL_ID)
		return DPID_UNKNOWN;

#ifdef __STL_0402
	for (CServerDescArray::iterator i = m_apServer.begin(); i != m_apServer.end(); ++i)
	{
		CServerDesc* pServer = i->second;
		if (pServer->GetIdofMulti() == uIdofMulti && pServer->IsUnderJurisdiction(dwWorldID, vPos))
			return (DPID)i->first;
	}
	return DPID_UNKNOWN;
#else	// __STL_0402
	CMyBucket<CServerDesc*>* pBucket = m_apServer.GetFirstActive();
	while (pBucket)
	{
		CServerDesc* pServer = pBucket->m_value;
		if (pServer->GetIdofMulti() == uIdofMulti && pServer->IsUnderJurisdiction(dwWorldID, vPos))
			return (DPID)pBucket->m_dwKey;
		pBucket = pBucket->pNext;
	}
	return DPID_UNKNOWN;
#endif	// __STL_0402

}

inline u_long CDPCoreSrvr::GetIdofMulti(DPID dpid)
{
#ifdef __STL_0402
	CServerDescArray::iterator i = m_apServer.find(dpid);
	if (i != m_apServer.end())
		return i->second->GetIdofMulti();
	return NULL_ID;
#else	// __STL_0402
	CServerDesc* pServerDesc;
	if (m_apServer.Lookup(dpid, pServerDesc))
		return pServerDesc->GetIdofMulti();
	return NULL_ID;
#endif	// __STL_0402
}
#endif	// __DPCORESRVR_H__