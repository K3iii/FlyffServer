#include "StdAfx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "User.h"
#include "ServerDesc.h"
#include "WorldMng.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;

extern  CDPDatabaseClient	g_dpDBClient;

#include "Party.h"
extern	CPartyMng	g_PartyMng;
#include "Environment.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#ifndef GETBLOCK
#define	GETBLOCK( ar, lpBuf, nBufSize )	\
										int nBufSize;	\
										LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
#endif	// __GETBLOCK


CDPCoreClient	g_DPCoreClient;


CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;

	ON_MSG(PACKETTYPE_LOAD_WORLD, &CDPCoreClient::OnLoadWorld);
	ON_MSG(PACKETTYPE_QUERYTICKCOUNT, &CDPCoreClient::OnQueryTickCount);
	ON_MSG(PACKETTYPE_RECHARGE_IDSTACK, &CDPCoreClient::OnRecharge);
	ON_MSG(PACKETTYPE_SHOUT, &CDPCoreClient::OnShout);
	ON_MSG(PACKETTYPE_PLAYMUSIC, &CDPCoreClient::OnPlayMusic);
	ON_MSG(PACKETTYPE_PLAYSOUND, &CDPCoreClient::OnPlaySound);

	ON_MSG(PACKETTYPE_ERRORPARTY, &CDPCoreClient::OnErrorParty);
	ON_MSG(PACKETTYPE_ADDPARTYMEMBER, &CDPCoreClient::OnAddPartyMember);
	ON_MSG(PACKETTYPE_REMOVEPARTYMEMBER, &CDPCoreClient::OnRemovePartyMember);
	ON_MSG(PACKETTYPE_ADDPLAYERPARTY, &CDPCoreClient::OnAddPlayerParty);
	ON_MSG(PACKETTYPE_REMOVEPLAYERPARTY, &CDPCoreClient::OnRemovePlayerParty);
	ON_MSG(PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPCoreClient::OnGuildMemberLv);
	ON_MSG(PACKETTYPE_SETPARTYEXP, &CDPCoreClient::OnSetPartyExp);
	ON_MSG(PACKETTYPE_REMOVEPARTYPOINT, &CDPCoreClient::OnRemovePartyPoint);
	ON_MSG(PACKETTYPE_CHANGETROUP, &CDPCoreClient::OnPartyChangeTroup);
	ON_MSG(PACKETTYPE_CHANPARTYNAME, &CDPCoreClient::OnPartyChangeName);
	ON_MSG(PACKETTYPE_SETPARTYMODE, &CDPCoreClient::OnSetPartyMode);
	ON_MSG(PACKETTYPE_PARTYCHANGEITEMMODE, &CDPCoreClient::OnPartyChangeItemMode);
	ON_MSG(PACKETTYPE_PARTYCHANGEEXPMODE, &CDPCoreClient::OnPartyChangeExpMode);
	ON_MSG(PACKETTYPE_ADDFRIEND, &CDPCoreClient::OnAddFriend);
	ON_MSG(PACKETTYPE_REMOVEFRIEND, &CDPCoreClient::OnRemovefriend);

	ON_MSG(PACKETTYPE_ENVIRONMENTALL, &CDPCoreClient::OnEnvironmentEffect);

	ON_MSG(PACKETTYPE_PARTYCHAT, &CDPCoreClient::OnPartyChat);

	ON_MSG(PACKETTYPE_CREATE_GUILD, &CDPCoreClient::OnCreateGuild);
	ON_MSG(PACKETTYPE_DESTROY_GUILD, &CDPCoreClient::OnDestroyGuild);
	ON_MSG(PACKETTYPE_ADD_GUILD_MEMBER, &CDPCoreClient::OnAddGuildMember);
	ON_MSG(PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPCoreClient::OnRemoveGuildMember);
	ON_MSG(PACKETTYPE_CW_GUILDCONTRIBUTION, &CDPCoreClient::OnGuildContributionACK);
	ON_MSG(PACKETTYPE_CW_GUILDNOTICE, &CDPCoreClient::OnGuildNoticeACk);
	ON_MSG(PACKETTYPE_GUILD_AUTHORITY, &CDPCoreClient::OnGuildAuthority);
#ifdef __FL_GUILD_FINDER
	ON_MSG(PACKETTYPE_GUILD_FINDERSTATE, &CDPCoreClient::OnGuildFinderState);
#endif
	ON_MSG(PACKETTYPE_GUILD_SETNAME, &CDPCoreClient::OnGuildSetName);
	ON_MSG(PACKETTYPE_GUILD_MSG_CONTROL, &CDPCoreClient::OnGuildMsgControl);
	ON_MSG(PACKETTYPE_GUILD_CLASS, &CDPCoreClient::OnGuildClass);
	ON_MSG(PACKETTYPE_GUILD_NICKNAME, &CDPCoreClient::OnGuildNickName);

	ON_MSG(PACKETTYPE_GUILD_MEMBERLOGOUT, &CDPCoreClient::OnGuildMemberLogOut);

	ON_MSG(PACKETTYPE_FRIENDINTERCEPTSTATE, &CDPCoreClient::OnFriendInterceptState);
	ON_MSG(PACKETTYPE_SETFRIENDSTATE, &CDPCoreClient::OnSetFriendState);

	ON_MSG(PACKETTYPE_GAMERATE, &CDPCoreClient::OnGameRate);
	ON_MSG(PACKETTYPE_LOADCONSTANT, &CDPCoreClient::OnLoadConstant);
	ON_MSG(PACKETTYPE_SETMONSTERRESPAWN, &CDPCoreClient::OnSetMonsterRespawn);
	ON_MSG(PACKETTYPE_PARTYCHANGELEADER, &CDPCoreClient::OnPartyChangeLeader);
	ON_MSG(PACKETTYPE_QUERYSETPLAYERNAME, &CDPCoreClient::OnSetPlayerName);
	ON_MSG(PACKETTYPE_SETSNOOP, &CDPCoreClient::OnSetSnoop);

	ON_MSG(PACKETTYPE_CHG_MASTER, &CDPCoreClient::OnChgMaster);
	ON_MSG(PACKETTYPE_GUILDCOMBAT_STATE, &CDPCoreClient::OnGuildCombatState);
	ON_MSG(PACKETTYPE_UNIFY, &CDPCoreClient::OnRemoveUserFromCORE);
	ON_MSG(PACKETTYPE_PING, &CDPCoreClient::OnPing);

	ON_MSG(PACKETTYPE_DESTROY_PLAYER, &CDPCoreClient::OnDestroyPlayer);

	ON_MSG(PACKETTYPE_INSTANCEDUNGEON_ALLINFO, &CDPCoreClient::OnInstanceDungeonAllInfo);
	ON_MSG(PACKETTYPE_INSTANCEDUNGEON_CREATE, &CDPCoreClient::OnInstanceDungeonCreate);
	ON_MSG(PACKETTYPE_INSTANCEDUNGEON_DESTROY, &CDPCoreClient::OnInstanceDungeonDestroy);
	ON_MSG(PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, &CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo);
	ON_MSG(PACKETTYPE_INSTANCEDUNGEON_DELETECOOLTIME, &CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo);

	ON_MSG(PACKETTYPE_UPDATE_PARTYSKILLS, &CDPCoreClient::OnUpdatePartySkills);

#ifdef __FL_PARTY_SORT
	ON_MSG(PACKETTYPE_MOVE_PARTYMEMBERS, &CDPCoreClient::OnSortPartyMembers);
#endif

#ifdef __FL_GW_STATUE_V2
	ON_MSG(PACKETTYPE_STATUE_SETTINGS, &CDPCoreClient::OnStatueSettings);
#endif

#ifdef __VALID_CONNECTION_CONFORMATION
	ON_MSG(PACKETTYPE_CONNECTION_CONFORMATION, &CDPCoreClient::OnConfirmPlayer);
#endif
#ifdef __FL_PARTY_FINDER
	ON_MSG(PACKETTYPE_PARTYCHANGEJOINMODE, &CDPCoreClient::OnPartyChangeJoinMode);
#endif

	m_bAlive = TRUE;
	m_hWait = WSACreateEvent();
	m_uRecharge = 0;
}

CDPCoreClient::~CDPCoreClient()
{
	CLOSE_HANDLE(m_hWait);
}

void CDPCoreClient::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{

}

void CDPCoreClient::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	CAr ar((LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - (sizeof(DPID) + sizeof(DPID)));
	GETTYPE(ar);

	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn) {
		(this->*(pfn))(ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + sizeof(DPID)), NULL_ID);
	}
	else {
		switch (dw)
		{
		case PACKETTYPE_PASSAGE:
		{
			DWORD objid, dwtmp;
			ar >> objid >> dwtmp;

			pfn = GetHandler(dwtmp);
			ASSERT(pfn != NULL);
			(this->*(pfn))(ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + sizeof(DPID)), (OBJID)objid);
			break;
		}
		case PACKETTYPE_BROADCAST:
		{
			DWORD dwtmp;
			ar >> dwtmp;

			pfn = GetHandler(dwtmp);
			ASSERT(pfn != NULL);
			(this->*(pfn))(ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + sizeof(DPID)), NULL_ID);
			break;
		}
		default:
			Error("Handler not found(%08x)\n", dw);
			break;
		}
	}


}

BOOL CDPCoreClient::Run(LPSTR lpszAddr, USHORT uPort, u_long uKey)
{
	if (ConnectToServer(lpszAddr, uPort, TRUE))
	{
		MyRegister(uKey);
		return (WaitForSingleObject(m_hWait, INFINITE) != WAIT_TIMEOUT);
	}
	return FALSE;
}

void CDPCoreClient::MyRegister(u_long uKey)
{
	BEFORESENDDUAL(ar, PACKETTYPE_MYREG, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uKey;
	ar << (DWORD)timeGetTime();
	SEND(ar, this, DPID_SERVERPLAYER);
}

BOOL CDPCoreClient::CheckIdStack(void)
{
	int cbUser = g_UserMng.GetCount();
	u_long uMax = cbUser * 128 + 20480;
	int cbStack = prj.m_objmap.m_idStack.GetCount();

	if (cbStack + m_uRecharge < uMax * 5 / 10)	// 50%
	{
		m_uRecharge = uMax - cbStack;
		Recharge(m_uRecharge);
		return FALSE;
	}
	return (m_uRecharge == 0);
}

void CDPCoreClient::Recharge(u_long uBlockSize)
{
	BEFORESENDDUAL(ar, PACKETTYPE_RECHARGE_IDSTACK, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uBlockSize;
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPCoreClient::SendToServer(LPBYTE lpBuffer, u_long uBufSize, DPID dpidCache, DPID dpidUser)
{
	CAr ar;
	int nBufSize;
	BYTE* lpBuf;

	ar << dpidCache << dpidUser;
	ar.Write(lpBuffer, uBufSize);	// overhead

	lpBuf = ar.GetBuffer(&nBufSize);
	Send((LPVOID)lpBuf, nBufSize, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendEventRealItem(u_long uIdPlayer, int nRealItemIndex, int nRealItemCount)
{
	BEFORESENDDUAL(ar, PACKETTYPE_RENEWEVNET, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uIdPlayer << nRealItemIndex << nRealItemCount;
	PASS(ar);
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::SendPartyMemberLevel(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_MEMBERLEVEL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idparty << pUser->m_idPlayer << pUser->GetLevel();
	PASS(ar);
}

void CDPCoreClient::SendPartyMemberJob(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_MEMBERJOB, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idparty << pUser->m_idPlayer << pUser->GetJob();
	PASS(ar);
}
#endif	// SYS_PLAYER_DATA

void CDPCoreClient::SendPartyLevel(CUser* pUser, DWORD dwLevel, DWORD dwPoint, DWORD dwExp)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PARTYLEVEL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idparty << pUser->m_idPlayer << dwLevel << dwPoint << dwExp;
	PASS(ar);
}
#if __VER >= 12
void CDPCoreClient::SendAddPartyExp(u_long uPartyId, int nMonLv, BOOL bSuperLeader, BOOL bLeaderSMExpUp)
{

	BEFORESENDDUAL(ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uPartyId << nMonLv << bSuperLeader << bLeaderSMExpUp;
	PASS(ar);
}
#else
void CDPCoreClient::SendAddPartyExp(u_long uPartyId, int nMonLv, BOOL bSuperLeader)
{

	BEFORESENDDUAL(ar, PACKETTYPE_ADDPARTYEXP, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uPartyId << nMonLv << bSuperLeader;
	PASS(ar);
}
#endif

void CDPCoreClient::SendRemovePartyPoint(u_long uPartyId, int nRemovePoint)
{
	if (nRemovePoint != 0)
	{

		BEFORESENDDUAL(ar, PACKETTYPE_REMOVEPARTYPOINT, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << uPartyId << nRemovePoint;
		PASS(ar);
	}
}

#if __VER < 11 // __SYS_PLAYER_DATA
void CDPCoreClient::SendFriendChangeJob(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CHANGEJOB, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idPlayer << pUser->GetJob();
	PASS(ar);
}
#endif	// __SYS_PLAYER_DATA

void CDPCoreClient::SendGameRate(FLOAT fRate, BYTE nFlag)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GAMERATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << fRate;
	ar << nFlag;
	PASS(ar);
}

void CDPCoreClient::SendLoadConstant()
{
	BEFORESENDDUAL(ar, PACKETTYPE_LOADCONSTANT, DPID_UNKNOWN, DPID_UNKNOWN);
	PASS(ar);
}

void CDPCoreClient::SendSetMonsterRespawn(u_long uidPlayer, DWORD dwMonsterID, DWORD dwRespawnNum, DWORD dwAttackNum, DWORD dwRect, DWORD dwRespawnTime, BOOL bFlying)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SETMONSTERRESPAWN, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uidPlayer;
	ar << dwMonsterID << dwRespawnNum << dwAttackNum << dwRect << dwRespawnTime;
	ar << bFlying;
	PASS(ar);
}


void CDPCoreClient::SendGuildMsgControl_Bank_Penya(CUser* pUser, DWORD p_Penya, BYTE p_Mode, BYTE cbCloak)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_UNKNOWN, DPID_UNKNOWN);

	GUILD_MSG_HEADER	Header;
	Header.HeadAMain = p_Mode;
	Header.HeadASub = (WORD)(pUser->m_idGuild);
	Header.HeadBMain = GUILD_MSG_HEADER::GUILD_BANK;
	Header.HeadBSub = GUILD_MSG_HEADER::PENYA;

	if (pUser->GetGuild())
	{
		ar.Write(&Header, sizeof(GUILD_MSG_HEADER));
		ar << p_Penya;
		ar << cbCloak;
	}

	PASS(ar);
}

void CDPCoreClient::SendBlock(BYTE nGu, u_long uidPlayerTo, char* szNameTo, u_long uidPlayerFrom)
{
	BEFORESENDDUAL(ar, PACKETTYPE_BLOCK, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nGu;
	ar << uidPlayerTo << uidPlayerFrom;
	ar.WriteString(szNameTo);
	PASS(ar);

}

// Handlers
void CDPCoreClient::OnLoadWorld(CAr& ar, DPID, DPID, OBJID)
{
	CServerDesc desc;
	desc.Serialize(ar);
	g_PartyMng.Serialize(ar);
	g_GuildMng.Serialize(ar, FALSE);

	CEnvironment::GetInstance()->Serialize(ar);

	for (list<CJurisdiction*>::iterator i = desc.m_lspJurisdiction.begin(); i != desc.m_lspJurisdiction.end(); ++i)
	{
		CJurisdiction* pJurisdiction = *i;

		LPWORLD lpWorld = g_WorldMng.GetWorldStruct(pJurisdiction->m_dwWorldID);
		if (lpWorld == NULL)
		{
			Error("[ LOAD WORLD FAILED. Check world.inc file. WORLD_ID(%d) ]", pJurisdiction->m_dwWorldID);
			continue;
		}

		g_WorldMng.Add(pJurisdiction);
	}

	g_WorldMng.ReadObject();

	WSASetEvent(m_hWait);
}

void CDPCoreClient::OnRecharge(CAr& ar, DPID, DPID, OBJID)
{
	OBJID idBase;
	u_long uBlockSize;

	ar >> idBase >> uBlockSize;
	//	u_long uMax		= 4096 * 32 + 10240;	// MAX_USER
	u_long uMax = 2048 * 128 + 20480;	// MAX_USER
	if (uBlockSize > uMax)
	{
		WriteLog("OnRecharge(): BLOCK SIZE TOO BIG, %d", uBlockSize);
		uBlockSize = uMax;
	}
	prj.m_objmap.m_idStack.PushIdBlock(idBase, uBlockSize);
	m_uRecharge = 0;
}
void CDPCoreClient::SendJoin(u_long idPlayer, const char* szPlayer, BOOL bOperator)
{
	BEFORESENDDUAL(ar, PACKETTYPE_JOIN, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(szPlayer);
	ar << bOperator;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendWhisper(u_long idFrom, u_long idTo, const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_WHISPER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idFrom << idTo;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendSay(u_long idFrom, u_long idTo, const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SAY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idFrom << idTo;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendModifyMode(DWORD dwMode, BYTE fAdd, u_long idFrom, u_long idTo)
{
	BEFORESENDDUAL(ar, PACKETTYPE_MODIFYMODE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwMode << fAdd << idFrom << idTo;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendShout(CUser* pUser, const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SHOUT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->m_idPlayer;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendPartyChat(CUser* pUser, const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PARTYCHAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << GETID(pUser);
	ar << pUser->m_idPlayer;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::SendUserPartySkill(u_long uidPlayer, int nMode, DWORD dwSkillTime, int nRemovePoint, int nCachMode)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PARTYSKILLUSE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uidPlayer;
	ar << nMode;
	ar << dwSkillTime;
	ar << nRemovePoint;
	ar << nCachMode;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGMSay(u_long idPlayer, DWORD dwWorldID, const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GMSAY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar << dwWorldID;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendPlayMusic(DWORD dwWorldID, u_long idmusic)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PLAYMUSIC, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idmusic;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendPlaySound(DWORD dwWorldID, u_long idsound)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PLAYSOUND, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << g_uIdofMulti;
	ar << dwWorldID;
	ar << idsound;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendKillPlayer(u_long idOperator, u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_KILLPLAYER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idOperator << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGetPlayerAddr(u_long idOperator, u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GETPLAYERADDR, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idOperator << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGetPlayerCount(u_long idOperator)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GETPLAYERCOUNT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idOperator;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGetCorePlayer(u_long idOperator)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GETCOREPLAYER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idOperator;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendSystem(const CHAR* lpString)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SYSTEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendCaption(const CHAR* lpString, DWORD dwWorldId, BOOL bSmall)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CAPTION, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << bSmall;
	ar << dwWorldId;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendFallSnow()
{
	BEFORESENDDUAL(ar, PACKETTYPE_FALLSNOW, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::SendFallRain()
{
	BEFORESENDDUAL(ar, PACKETTYPE_FALLRAIN, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::SendStopSnow()
{
	BEFORESENDDUAL(ar, PACKETTYPE_STOPSNOW, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::SendStopRain()
{
	BEFORESENDDUAL(ar, PACKETTYPE_STOPRAIN, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGuildCombatState(int nState)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILDCOMBAT_STATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nState;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::SendGCRemoveParty(u_long uidPartyid, u_long uidPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_REMOVEPARTY_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << uidPartyid << uidPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::OnShout(CAr& ar, DPID, DPID, OBJID)
{
	char	lpString[1024];
	u_long idPlayer;
	ar >> idPlayer;
	ar.ReadString(lpString, 1024);

	const char* lpszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
	if (lpszPlayer)
	{
		CAr ar1;
		ar1 << NULL_ID << SNAPSHOTTYPE_SHOUT;
		ar1 << NULL_ID;	//GETID( pUser );
		ar1.WriteString(lpszPlayer);
		ar1.WriteString(lpString);
#if __VER >= 12 // __LORD
		ar1 << (DWORD)0xffff99cc;
#endif	// __LORD
		GETBLOCK(ar1, lpBlock, uBlockSize);
		g_UserMng.AddBlock(lpBlock, uBlockSize);
	}
}

void CDPCoreClient::OnPlayMusic(CAr& ar, DPID, DPID, OBJID)
{
	DWORD dwWorldID;
	u_long idmusic;

	ar >> idmusic;
	ar >> dwWorldID;

	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldID);

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYMUSIC << idmusic;
	GETBLOCK(arBlock, lpBlock, uBlockSize);
	g_UserMng.AddBlock(lpBlock, uBlockSize, pWorld);
}

void CDPCoreClient::OnPlaySound(CAr& ar, DPID, DPID, OBJID)
{
	DWORD dwWorldID;
	u_long idsound;

	ar >> idsound;
	ar >> dwWorldID;

	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldID);
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_PLAYSOUND;
	arBlock << (BYTE)0;
	arBlock << idsound;
	GETBLOCK(arBlock, lpBlock, uBlockSize);
	g_UserMng.AddBlock(lpBlock, uBlockSize, pWorld);
}

void CDPCoreClient::OnErrorParty(CAr& ar, DPID, DPID, OBJID)
{
	DWORD dw;
	u_long uLeader;
	ar >> dw;
	ar >> uLeader;

	CUser* pUser = (CUser*)prj.GetUserByID(uLeader);
	if (IsValidObj((CObj*)pUser))
		pUser->AddSendErrorParty(dw);
}

void CDPCoreClient::OnAddPartyMember(CAr& ar, DPID, DPID, OBJID)
{
	u_long	idParty, idLeader, idMember;

	ar >> idParty;
	ar >> idLeader;
	ar >> idMember;
	int nMode[MAX_PARTYMODE];
	ar.Read(nMode, sizeof(int) * MAX_PARTYMODE);

	char pszLeader[MAX_PLAYER] = { 0, };
	char pszMember[MAX_PLAYER] = { 0, };
	const char* lpPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idLeader);
	if (lpPlayer)
		lstrcpy(pszLeader, lpPlayer);
	lpPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idMember);
	if (lpPlayer)
		lstrcpy(pszMember, lpPlayer);

	CUser* pUserLeader = (CUser*)prj.GetUserByID(idLeader);
	CUser* pUserNewMember = (CUser*)prj.GetUserByID(idMember);

	CParty* pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
		if (pParty->NewMember(idMember))
		{
			if (IsValidObj(pUserNewMember))
			{
				// Refresh Red scroll mode for whole party if new member have it
				if (!pParty->m_nModeTime[PARTY_PARSKILL_MODE])
				{
					if (pUserNewMember->IsSMMode(SM_PARTYSKILL1) || pUserNewMember->IsSMMode(SM_PARTYSKILL15) || pUserNewMember->IsSMMode(SM_PARTYSKILL30))
						g_DPCoreClient.SendUserPartySkill(pUserNewMember->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
				}

				// Refresh party skills for join player
				for (int i = 0; i < MAX_PARTYMODE; ++i)
				{
					if (nMode[i])
						pUserNewMember->AddSetPartyMode(i, TRUE, 0, (DWORD)nMode[i]);
				}
			}

			for (int i = 0; i < pParty->m_nSizeofMember; i++)
			{
				CUser* pMember = (CUser*)prj.GetUserByID(pParty->m_aMember[i].m_uPlayerId);

#ifdef __FL_FIX_PLAYERDATA
				// Send player data of new member for other party members
				if (IsValidObj(pMember))
				{
					PlayerData* pPlayerDataJoiner = CPlayerDataCenter::GetInstance()->GetPlayerData(idMember);
					if (pPlayerDataJoiner)
						pMember->AddQueryPlayerData(idMember, pPlayerDataJoiner);
				}

				// Refresh player data of party members for join player
				if (IsValidObj(pUserNewMember))
				{
					PlayerData* pPlayerDataMember = CPlayerDataCenter::GetInstance()->GetPlayerData(pParty->m_aMember[i].m_uPlayerId);
					if (pPlayerDataMember)
						pUserNewMember->AddQueryPlayerData(pParty->m_aMember[i].m_uPlayerId, pPlayerDataMember);
				}
#endif
				if (IsValidObj((CObj*)pMember))
				{
					pMember->AddPartyMember(pParty, idMember, pszLeader, pszMember);
					if (pMember->m_idparty != idParty)
						pMember->m_idparty = idParty;

				}
			}
		}
	}
	else
	{
		if (idParty == g_PartyMng.NewParty(idLeader, idMember, idParty))
		{
			pParty = g_PartyMng.GetParty(idParty);
			if (pParty)
			{
				// Refresh Red scroll mode for new party if one of players have it
				if (!pParty->m_nModeTime[PARTY_PARSKILL_MODE])
				{
					BOOL bRefreshed = FALSE;

					// Check if leader have it
					if (IsValidObj(pUserLeader))
					{
						if (pUserLeader->IsSMMode(SM_PARTYSKILL1) || pUserLeader->IsSMMode(SM_PARTYSKILL15) || pUserLeader->IsSMMode(SM_PARTYSKILL30))
						{
							g_DPCoreClient.SendUserPartySkill(pUserLeader->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
							bRefreshed = TRUE;
						}
					}

					// If not, then check if second member have it
					if (!bRefreshed)
					{
						if (IsValidObj(pUserNewMember))
						{
							if (pUserNewMember->IsSMMode(SM_PARTYSKILL1) || pUserNewMember->IsSMMode(SM_PARTYSKILL15) || pUserNewMember->IsSMMode(SM_PARTYSKILL30))
								g_DPCoreClient.SendUserPartySkill(pUserNewMember->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
						}
					}
				}

#ifdef __FL_FIX_PLAYERDATA
				// Add player data member -> leader
				if (IsValidObj(pUserLeader))
				{
					PlayerData* pPlayerDataMember = CPlayerDataCenter::GetInstance()->GetPlayerData(idMember);
					if (pPlayerDataMember)
						pUserLeader->AddQueryPlayerData(idMember, pPlayerDataMember);
				}

				// Add player data leader -> member
				if (IsValidObj(pUserNewMember))
				{
					PlayerData* pPlayerDataLeader = CPlayerDataCenter::GetInstance()->GetPlayerData(idLeader);
					if (pPlayerDataLeader)
						pUserNewMember->AddQueryPlayerData(idLeader, pPlayerDataLeader);
				}
#endif

				CUser* pMember;
				for (int i = 0; i < pParty->m_nSizeofMember; i++)
				{
					pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
					if (IsValidObj((CObj*)pMember))
					{
						pMember->m_idparty = idParty;
						pMember->AddPartyMember(pParty, idMember, pszLeader, pszMember);
					}
				}
			}
		}
	}
}

void CDPCoreClient::OnRemovePartyMember(CAr& ar, DPID, DPID, OBJID)
{
	u_long idParty, idLeader, idMember;
	ar >> idParty >> idLeader >> idMember;

	CParty* pParty;
	pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
#if __VER >= 11 // __SYS_PLAYER_DATA
		char pszLeader[MAX_PLAYER] = { 0, };
		char pszMember[MAX_PLAYER] = { 0, };
		const char* lpPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idLeader);
		if (lpPlayer)
			lstrcpy(pszLeader, lpPlayer);
		lpPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idMember);
		if (lpPlayer)
			lstrcpy(pszMember, lpPlayer);
#endif	// __SYS_PLAYER_DATA

		if (pParty->DeleteMember(idMember))
		{
			if (pParty->GetSizeofMember() < 2)
			{
				CUser* pMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(0));
				if (IsValidObj(pMember))
				{
					pMember->AddPartyMember(NULL, 0, pszLeader, pszMember);
					pMember->m_idparty = 0;
				}
				pMember = g_UserMng.GetUserByPlayerID(idMember);
				g_PartyMng.DeleteParty(pParty->m_uPartyId);
			}
			else
			{
				// leave
				CUser* pMember;
				for (int i = 0; i < pParty->m_nSizeofMember; i++)
				{
					pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
					if (IsValidObj((CObj*)pMember))
						pMember->AddPartyMember(pParty, idMember, pszLeader, pszMember);
				}
			}

			CUser* pRemovd = g_UserMng.GetUserByPlayerID(idMember);
			if (IsValidObj((CObj*)pRemovd))
			{
				pRemovd->m_idparty = 0;
				pRemovd->AddPartyMember(NULL, idMember, pszLeader, pszMember);
			}
		}
		else
		{
			Error("OnRemovePartyMember:: Not DeleteMember");
		}
	}
	else
	{
		Error("OnRemovePartyMember:: Not Party");
	}
}

void CDPCoreClient::OnAddPlayerParty(CAr& ar, DPID, DPID, OBJID)
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
		int i = pParty->FindMember(idPlayer);
		if (i < 0)
			return;

		pParty->m_aMember[i].m_bRemove = FALSE;

		CUser* pMember;
		for (int j = 0; j < pParty->m_nSizeofMember; j++)
		{
			pMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(j));
			if (IsValidObj((CObj*)pMember))
				pMember->AddSetPartyMemberParam(idPlayer, PP_REMOVE, 0);
		}
	}
}

void CDPCoreClient::OnRemovePlayerParty(CAr& ar, DPID, DPID, OBJID)
{
	u_long idParty, idPlayer;
	ar >> idParty >> idPlayer;

	CParty* pParty;

	pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
		int i = pParty->FindMember(idPlayer);
		if (i < 0)
			return;

		pParty->m_aMember[i].m_bRemove = TRUE;

		CUser* pMember;
		for (int j = 0; j < pParty->m_nSizeofMember; j++)
		{
			pMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(j));
			if (IsValidObj((CObj*)pMember))
				pMember->AddSetPartyMemberParam(idPlayer, PP_REMOVE, 1);
		}

		if (i == 0)
		{
			bool fRemoveParty = true;
			for (int j = 1; j < pParty->m_nSizeofMember; j++)
			{
				if (pParty->m_aMember[j].m_bRemove == FALSE)
				{
					fRemoveParty = false;
					pParty->SwapPartyMember(0, j);
					// swap
					break;
				}
			}

			if (fRemoveParty)
			{
				CUser* pMember;
				for (int j = 0; j < pParty->m_nSizeofMember; j++)
				{
					pMember = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(j));
					if (IsValidObj((CObj*)pMember))
						pMember->m_idparty = 0;
				}
				g_PartyMng.DeleteParty(pParty->m_uPartyId);
			}
		}
	}
}

void CDPCoreClient::OnSetPartyMode(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	int nMode;
	BOOL bOnOfff;
	LONG nPoint;
	DWORD	dwSkillTime;
	ar >> uPartyId >> nMode >> dwSkillTime >> bOnOfff;
	if (bOnOfff == TRUE)
		ar >> nPoint;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		if (bOnOfff == TRUE)
			pParty->m_nPoint = nPoint;

		if (nMode == PARTY_PARSKILL_MODE && !bOnOfff)
		{
			for (int j = 0; j < pParty->m_nSizeofMember; ++j)
			{
				if (pParty->m_aMember[j].m_bRemove)
					continue;
				CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[j].m_uPlayerId);
				if (IsValidObj(pUser) && (pUser->IsSMMode(SM_PARTYSKILL30) || pUser->IsSMMode(SM_PARTYSKILL15) || pUser->IsSMMode(SM_PARTYSKILL1)))
				{
					bOnOfff = TRUE;
					g_DPCoreClient.SendUserPartySkill(pUser->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
					break;
				}
			}
		}

		pParty->m_nModeTime[nMode] = bOnOfff;

		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddSetPartyMode(nMode, bOnOfff, pParty->m_nPoint, dwSkillTime);
		}
	}
}

void CDPCoreClient::OnPartyChangeItemMode(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->m_nTroupeShareItem = nMode;

		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyChangeItemMode(nMode);
		}
	}
}

void CDPCoreClient::OnPartyChangeExpMode(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	int nMode;
	ar >> uPartyId >> nMode;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->m_nTroupsShareExp = nMode;

		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyChangeExpMode(nMode);
		}
	}
}


void CDPCoreClient::OnSetPartyExp(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	LONG nExp, nPoint, nLevel;
	ar >> uPartyId >> nExp >> nPoint >> nLevel;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->m_nExp = nExp;
		pParty->m_nPoint = nPoint;
		pParty->m_nLevel = nLevel;
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyExpLevel(pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint());
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnRemovePartyPoint(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	LONG nPartyPoint;
	ar >> uPartyId;
	ar >> nPartyPoint;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->m_nPoint = nPartyPoint;

		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyExpLevel(pParty->GetExp(), pParty->GetLevel(), pParty->GetPoint());
		}
	}
	else
	{
		// error
	}
}

void CDPCoreClient::OnPartyChangeName(CAr& ar, DPID, DPID, OBJID)
{
	u_long uidParty;
	TCHAR	sParty[128];

	ar >> uidParty;
	ar.ReadString(sParty, 128);

	CParty* pParty;

	pParty = g_PartyMng.GetParty(uidParty);
	if (pParty)
	{
		strcpy(pParty->m_sParty, sParty);
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyChangeName(pParty->m_sParty);
		}
	}
}

void CDPCoreClient::OnPartyChangeTroup(CAr& ar, DPID, DPID, OBJID)
{
	u_long uidParty;
	TCHAR	sParty[33];

	ar >> uidParty;
	ar.ReadString(sParty, 33);

	CParty* pParty;

	pParty = g_PartyMng.GetParty(uidParty);
	if (pParty)
	{
		pParty->m_nKindTroup = 1;
		strcpy(pParty->m_sParty, sParty);
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyChangeTroup(pParty->m_sParty);
		}

	}
}

void CDPCoreClient::OnAddFriend(CAr& ar, DPID, DPID, OBJID)
{
	int bAdd = 0;
	u_long uidSend, uidFriend;
	ar >> uidSend >> uidFriend;

	CUser* pSender;
	CUser* pFriend;

	pSender = g_UserMng.GetUserByPlayerID(uidSend);
	pFriend = g_UserMng.GetUserByPlayerID(uidFriend);

	const char* lpszFriendPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(uidFriend);
	if (lpszFriendPlayer == NULL)
		return;
	char lpszFriend[MAX_PLAYER];
	strcpy(lpszFriend, lpszFriendPlayer);

	if (IsValidObj((CObj*)pSender))
	{
		if (MAX_FRIEND <= pSender->m_RTMessenger.size())
		{
			pSender->AddDefinedText(TID_GAME_MSGMAXUSER, "");
		}
		else
		{
#ifdef __FL_FIX_PLAYERDATA
			PlayerData* pPlayerDataFriend = CPlayerDataCenter::GetInstance()->GetPlayerData(uidFriend);
			if (pPlayerDataFriend)
				pSender->AddQueryPlayerData(uidFriend, pPlayerDataFriend);
#endif
			pSender->m_RTMessenger.SetFriend(uidFriend, NULL);
			pSender->AddAddFriend(uidFriend, lpszFriend);
			bAdd++;
		}
	}

	const char* lpszSendPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(uidSend);
	if (lpszSendPlayer == NULL)
		return;
	char lpszSend[MAX_PLAYER];
	strcpy(lpszSend, lpszSendPlayer);

	if (IsValidObj((CObj*)pFriend))
	{
		if (MAX_FRIEND <= pFriend->m_RTMessenger.size())
		{
			if (IsValidObj((CObj*)pSender))
				pSender->AddDefinedText(TID_GAME_MSGMAXUSER, "");
		}
		else
		{
#ifdef __FL_FIX_PLAYERDATA
			PlayerData* pPlayerDataSender = CPlayerDataCenter::GetInstance()->GetPlayerData(uidSend);
			if (pPlayerDataSender)
				pFriend->AddQueryPlayerData(uidSend, pPlayerDataSender);
#endif
			pFriend->m_RTMessenger.SetFriend(uidSend, NULL);
			pFriend->AddAddFriend(uidSend, lpszSend);
			bAdd += 2;
		}
	}

	if (IsValidObj((CObj*)pSender) && strlen(lpszFriend) && (bAdd == 2 || bAdd == 3))
		pSender->AddDefinedText(TID_GAME_MSGINVATECOM, "%s", lpszFriend);

	if (IsValidObj((CObj*)pFriend) && strlen(lpszSend) && (bAdd == 1 || bAdd == 3))
		pFriend->AddDefinedText(TID_GAME_MSGINVATECOM, "%s", lpszSend);
}

void CDPCoreClient::OnRemovefriend(CAr& ar, DPID, DPID, OBJID)
{
	u_long uidSend, uidFriend;
	ar >> uidSend >> uidFriend;

	CUser* pSender = (CUser*)prj.GetUserByID(uidSend);

	if (IsValidObj(pSender))
	{
		CHousingMng::GetInstance()->ReqSetAllowVisit(pSender, uidFriend, FALSE);
		pSender->m_RTMessenger.RemoveFriend(uidFriend);
		pSender->AddRemoveFriend(uidFriend);
	}

	CUser* pFriend = g_UserMng.GetUserByPlayerID(uidFriend);
	if (IsValidObj(pFriend))
	{
		CHousingMng::GetInstance()->ReqSetAllowVisit(pFriend, uidSend, FALSE);
		pFriend->m_RTMessenger.RemoveFriend(uidSend);
		pFriend->AddRemoveFriend(uidSend);
	}
}

void CDPCoreClient::OnQueryTickCount(CAr& ar, DPID, DPID, OBJID)
{
	DWORD dwTime;
	__int64 nTickCount;
	double dCurrentTime;
	ar >> dwTime >> nTickCount >> dCurrentTime;

	dwTime = timeGetTime() - dwTime;
	nTickCount += (dwTime * 5000);	// ( dwTime / 2 ) * 10000

	g_TickCount.Start(nTickCount);
	g_GameTimer.SetCurrentTime(dCurrentTime);
}

void CDPCoreClient::OnEnvironmentEffect(CAr& ar, DPID, DPID, OBJID)
{
	CEnvironment::GetInstance()->Serialize(ar);

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_ENVIRONMENT;
	CEnvironment::GetInstance()->Serialize(arBlock);
	arBlock.WriteString(prj.m_EventLua.GetWeatherEventTitle().c_str());

	GETBLOCK(arBlock, lpBlock, uBlockSize);
	g_UserMng.AddBlock(lpBlock, uBlockSize);
}

void CDPCoreClient::OnPartyChat(CAr& ar, DPID, DPID, OBJID)
{
	u_long idParty;
	static	\
		CHAR	lpName[MAX_PLAYER], lpString[260], lpSnoopString[512];

	OBJID objid;
	ar >> objid;
	ar >> idParty;
	ar.ReadString(lpName, MAX_PLAYER);
	ar.ReadString(lpString, 260);

	CParty* pParty;

	pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
		CUser* pMember = NULL;
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			pMember = (CUser*)prj.GetUserByID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pMember))
			{
				pMember->AddPartyChat(lpName, lpString, objid);
				if (pMember->m_idSnoop)
				{
					CUser* pSnoop = g_UserMng.GetUserByPlayerID(pMember->m_idSnoop);
					if (IsValidObj((CObj*)pSnoop))
					{
						sprintf(lpSnoopString, "[p][%s->%s] %s", lpName, pMember->GetName(), lpString);
						pSnoop->AddSnoop(lpSnoopString);
					}
				}
			}
		}
	}
}


void CDPCoreClient::SendCreateGuild(GUILD_MEMBER_INFO* info, int nSize, const char* szGuild)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CREATE_GUILD, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nSize;
	ar.Write(info, sizeof(GUILD_MEMBER_INFO) * nSize);
	ar.WriteString(szGuild);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendGuildChat(CUser* pUser, const char* sChat)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILD_CHAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->GetId();
	ar << pUser->m_idPlayer;
	ar.WriteString(sChat);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnCreateGuild(CAr& ar, DPID, DPID, OBJID)
{
	int nSize;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];

	ar >> nSize;

	if (nSize < 0 || nSize > MAX_PTMEMBER_SIZE)
		return;

	ar.Read(info, sizeof(GUILD_MEMBER_INFO) * nSize);
	ar >> idGuild;
	ar.ReadString(szGuild, MAX_G_NAME);

	int	cb = 0;
	CUser* apUser[MAX_PTMEMBER_SIZE];

	CGuild* pGuild = new CGuild;
	pGuild->m_idGuild = idGuild;
	lstrcpy(pGuild->m_szGuild, szGuild);
	pGuild->m_idMaster = info[0].idPlayer;
	pGuild->m_adwPower[GUD_MASTER] = 0x000000FF;
	idGuild = g_GuildMng.AddGuild(pGuild);
	if (idGuild > 0)
	{
		// master
		CGuildMember* pMember = new CGuildMember;
		pMember->m_idPlayer = info[0].idPlayer;
		pMember->m_nMemberLv = GUD_MASTER;
		if (pGuild->AddMember(pMember))
		{
			CUser* pUser = (CUser*)prj.GetUserByID(info[0].idPlayer);
			if (IsValidObj(pUser))
			{
				pUser->m_idGuild = idGuild;
				apUser[cb++] = pUser;
			}
			// member
			for (int i = 1; i < nSize; i++)
			{
				pMember = new CGuildMember;
				pMember->m_idPlayer = info[i].idPlayer;
				pMember->m_nMemberLv = GUD_ROOKIE;
				if (pGuild->AddMember(pMember))
				{
					pUser = (CUser*)prj.GetUserByID(info[i].idPlayer);
					if (IsValidObj(pUser))
					{
						pUser->m_idGuild = idGuild;
						apUser[cb++] = pUser;
					}
				}
				else
				{
					info[i].idPlayer = 0;
					SAFE_DELETE(pMember);
				}
			}
		}
		else
		{
			SAFE_DELETE(pMember);
			g_GuildMng.RemoveGuild(idGuild);
		}
	}
	else
	{
		SAFE_DELETE(pGuild);
	}

#ifndef __FL_GUILD_FINDER // Bugfix: Guilds should be created always and everywhere, on all channels and for all players
	if (cb > 0)
#else
	if (pGuild)
#endif
	{
		g_UserMng.AddCreateGuild(info[0].idPlayer, CPlayerDataCenter::GetInstance()->GetPlayerString(info[0].idPlayer), idGuild, szGuild);	// g_UserMng.Lock();
	}

	for (int i = 0; i < cb; i++)
	{
		g_UserMng.AddSetGuild(apUser[i], idGuild);
		apUser[i]->AddMyGuild();

		// This actually not needed because with default guild-creation quest players should be at the same party 
		// amd their playerdata will be updated in party-function, but this code will be needed in case if guild-creation system will be changed someday
#ifdef __FL_FIX_PLAYERDATA
		for (int z = 0; z < cb; z++)
		{
			PlayerData* pPlayerDataZ = CPlayerDataCenter::GetInstance()->GetPlayerData(apUser[z]->m_idPlayer);
			if (pPlayerDataZ)
				apUser[i]->AddQueryPlayerData(apUser[z]->m_idPlayer, pPlayerDataZ);
		}
#endif

	}
}

void CDPCoreClient::OnDestroyGuild(CAr& ar, DPID, DPID, OBJID)
{
	u_long idGuild;
	ar >> idGuild;

	int nSize = 0;
	CUser* pUserIk3[MAX_GM_SIZE];

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CUser* pUsertmp;
		CGuildMember* pMember;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;

			pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
			if (IsValidObj(pUsertmp))
			{
#if __VER >= 15 // __GUILD_HOUSE
				GuildHouseMng->ResetApplyDST(pUsertmp);
#endif // __GUILD_HOUSE
				pUsertmp->m_idGuild = 0;
				g_UserMng.AddSetGuild(pUsertmp, 0);
				pUserIk3[nSize] = pUsertmp;
				++nSize;
			}
		}

		const char* lpszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(pGuild->m_idMaster);

		g_GuildMng.RemoveGuild(pGuild->m_idGuild);
		g_UserMng.AddDestroyGuild(lpszPlayer, idGuild);	// g_UserMng.Lock();

		for (int cou = 0; cou < nSize; ++cou)
		{
			if (IsValidObj(pUserIk3[cou]))
			{
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
				pUserIk3[cou]->m_tGuildMember = CTime::GetCurrentTime();
				pUserIk3[cou]->m_tGuildMember += CTimeSpan(2, 0, 0, 0);
#endif
				if (pUserIk3[cou]->GetWorld())
				{
					PRegionElem pRgnElem = NULL;
					DWORD dwWorldID = pUserIk3[cou]->GetWorld()->GetID();
					CGuildQuestProcessor* pProcessor = CGuildQuestProcessor::GetInstance();
					int nId = pProcessor->PtInQuestRect(pUserIk3[cou]->GetPos());
					if (nId > -1)
					{
						PGUILDQUESTELEM pElem = pProcessor->GetGuildQuest(nId);
						if (!pElem || pElem->idGuild != pUserIk3[cou]->m_idGuild)
						{
							CWorld* pWorld = g_WorldMng.GetWorld(dwWorldID);
							if (pWorld)
							{
								if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
									pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);
								if (!pRgnElem)	// Find near revival pos
									pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), pUserIk3[cou]->GetPos());
							}
						}
					}

					if (pRgnElem)
						pUserIk3[cou]->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
				}
				//////////
			}
		}
		return;
	}
}

void CDPCoreClient::OnAddGuildMember(CAr& ar, DPID, DPID, OBJID)
{
	u_long idGuild;
	GUILD_MEMBER_INFO info;
	ar.Read(&info, sizeof(GUILD_MEMBER_INFO));
	ar >> idGuild;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		if (pGuild->GetSize() >= MAX_GM_SIZE)
			return;

		CGuildMember* pMember = new CGuildMember;
		pMember->m_idPlayer = info.idPlayer;
		pMember->m_nMemberLv = GUD_ROOKIE;
		if (pGuild->AddMember(pMember))
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(info.idPlayer);
			BOOL bValidUser = FALSE;
			if (IsValidObj(pUser))
				bValidUser = TRUE;

			if (bValidUser)
			{
				pUser->m_idGuild = idGuild;
				g_UserMng.AddSetGuild(pUser, idGuild);
				pUser->AddGuildHouseAllInfo(GuildHouseMng->GetGuildHouse(idGuild));
				GuildHouseMng->SetApplyDST(pUser);
			}

#ifdef __FL_FIX_PLAYERDATA
			PlayerData* pPlayerDataNewMember = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);

			CUser* pGuildUser;
			for (map<u_long, CGuildMember*>::iterator i2 = pGuild->m_mapPMember.begin(); i2 != pGuild->m_mapPMember.end(); ++i2)
			{
				// Update player data of new member for all existing guild members
				pGuildUser = (CUser*)prj.GetUserByID(i2->second->m_idPlayer);
				if (IsValidObj(pGuildUser))
				{
					if (pPlayerDataNewMember)
						pGuildUser->AddQueryPlayerData(pMember->m_idPlayer, pPlayerDataNewMember);
				}

				// Update player data of guildmembers for new member
				if (bValidUser)
				{
					PlayerData* pPlayerDataGuildMember = CPlayerDataCenter::GetInstance()->GetPlayerData(i2->second->m_idPlayer);
					if (pPlayerDataGuildMember)
						pUser->AddQueryPlayerData(i2->second->m_idPlayer, pPlayerDataGuildMember);
				}
			}
#endif
		}
		else
		{
			SAFE_DELETE(pMember);
		}
	}
}

void CDPCoreClient::OnRemoveGuildMember(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idGuild;
	ar >> idPlayer >> idGuild;

	BOOL bRemove = FALSE;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	CUser* pUser;

	if (pGuild)
	{
		if (pGuild->RemoveMember(idPlayer))
		{
			pUser = (CUser*)prj.GetUserByID(idPlayer);
			if (IsValidObj(pUser))
			{
				GuildHouseMng->ResetApplyDST(pUser);
				GuildHouseMng->GoOutGuildHouse(pUser);
				pUser->m_idGuild = 0;
				g_UserMng.AddSetGuild(pUser, 0);
				bRemove = TRUE;
			}
		}
	}

	if (bRemove)
	{
		if (IsValidObj(pUser))
		{
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
			pUser->m_tGuildMember = CTime::GetCurrentTime();
			pUser->m_tGuildMember += CTimeSpan(2, 0, 0, 0);
#endif

			if (pUser->GetWorld())
			{

				PRegionElem pRgnElem = NULL;
				DWORD dwWorldID = pUser->GetWorld()->GetID();
				CGuildQuestProcessor* pProcessor = CGuildQuestProcessor::GetInstance();
				int nId = pProcessor->PtInQuestRect(pUser->GetPos());
				if (nId > -1)
				{
					PGUILDQUESTELEM pElem = pProcessor->GetGuildQuest(nId);
					if (!pElem || pElem->idGuild != pUser->m_idGuild)
					{
						CWorld* pWorld = g_WorldMng.GetWorld(dwWorldID);
						if (pWorld)
						{
							if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
								pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);
							if (!pRgnElem)
								pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), pUser->GetPos());
						}
					}
				}
				if (pRgnElem)
					pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
			}
		}
	}
}

void CDPCoreClient::OnGuildMemberLv(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idGuild;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv >> idGuild;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);
		if (pMember)
		{
			pMember->m_nMemberLv = nMemberLv;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildClass(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idGuild;
	int nClass;
	ar >> idPlayer >> nClass >> idGuild;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);
		if (pMember)
			pMember->m_nClass = nClass;
	}
}

void CDPCoreClient::OnGuildNickName(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idGuild;
	char strNickName[MAX_GM_ALIAS] = { 0, };
	ar >> idPlayer >> idGuild;
	ar.ReadString(strNickName, MAX_GM_ALIAS);
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		CGuildMember* pMember = pGuild->GetMember(idPlayer);
		if (pMember)
			strcpy(pMember->m_szAlias, strNickName);
	}
}

void CDPCoreClient::OnChgMaster(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idPlayer2, idGuild;
	ar >> idPlayer >> idPlayer2 >> idGuild;
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		pGuild->m_idMaster = idPlayer2;
		CGuildMember* pMember = pGuild->GetMember(idPlayer);
		if (pMember)
		{
			pMember->m_nMemberLv = GUD_ROOKIE;
			pMember->m_nClass = 0;
		}
		pMember = pGuild->GetMember(idPlayer2);
		if (pMember)
		{
			pMember->m_nMemberLv = GUD_MASTER;
			pMember->m_nClass = 0;
		}
	}
}

void CDPCoreClient::OnGuildMemberLogOut(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idGuild;
	ar >> idGuild >> idPlayer;
}


void CDPCoreClient::OnGuildContributionACK(CAr& ar, DPID, DPID, OBJID)
{
	CONTRIBUTION_CHANGED_INFO info;
	ULONG uServerID;

	ar >> uServerID;
	ar >> info;

	TRACE("PACKETTYPE_CW_GUILDCONTRIBUTION GUILD:%d idPlayer:%d dwPxpCount:%d dwPenya:%d "
		"dwGuildPxpCount:%d dwGuildPenya:%d nGuildLevel:%d\n",
		info.idGuild, info.idPlayer, info.dwPxpCount, info.dwPenya,
		info.dwGuildPxpCount, info.dwGuildPenya, info.nGuildLevel);

	CGuild* pGuild = g_GuildMng.GetGuild(info.idGuild);
	if (pGuild)
	{
		if (uServerID != ::g_uKey)
			pGuild->SetContribution(info);

		CUser* pUser;
		CGuildMember* pMember;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pUser = (CUser*)prj.GetUserByID(pMember->m_idPlayer);

			if (IsValidObj(pUser))
				pUser->AddContribution(info);
		}
	}
}

void CDPCoreClient::OnGuildNoticeACk(CAr& ar, DPID, DPID, OBJID)
{
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];

	ar >> idGuild;
	ar.ReadString(szNotice, MAX_BYTE_NOTICE);
	TRACE("PACKETTYPE_CW_GUILDNOTICE - GUILD:%d Notice:%s\n", idGuild, szNotice);

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		pGuild->SetNotice(szNotice);

		CUser* pUser;
		CGuildMember* pMember;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pUser = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
			if (IsValidObj(pUser))
				pUser->AddSetNotice(idGuild, szNotice);
		}
	}
}

void CDPCoreClient::OnGuildAuthority(CAr& ar, DPID, DPID, OBJID)
{
	u_long uGuildId;
	DWORD dwAuthority[MAX_GM_LEVEL];

	ar >> uGuildId;

	ar.Read(dwAuthority, sizeof(dwAuthority));

	CGuild* pGuild = g_GuildMng.GetGuild(uGuildId);
	if (pGuild)
	{
		memcpy(pGuild->m_adwPower, dwAuthority, sizeof(dwAuthority));

		CUser* pUser;
		CGuildMember* pMember;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pUser = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
			if (IsValidObj(pUser))
				pUser->AddSetGuildAuthority(dwAuthority);
		}
	}
}

#ifdef __FL_GUILD_FINDER
void CDPCoreClient::OnGuildFinderState(CAr& ar, DPID, DPID, OBJID)
{
	u_long uGuildId;
	BOOL bFinder;

	ar >> uGuildId >> bFinder;

	CGuild* pGuild = g_GuildMng.GetGuild(uGuildId);
	if (!pGuild)
		return;

	pGuild->m_bFinder = bFinder;

	CUser* pUser;
	CGuildMember* pMember;
	for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
	{
		pMember = i->second;
		pUser = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
		if (IsValidObj(pUser))
			pUser->AddSetGuildFinderState(bFinder);
	}
}
#endif


void CDPCoreClient::OnGuildSetName(CAr& ar, DPID, DPID, OBJID)
{
	u_long idGuild;
	char lpszGuild[MAX_G_NAME];

	ar >> idGuild;
	ar.ReadString(lpszGuild, MAX_G_NAME);
	u_long idPlayer;
	BYTE nId, nError;
	ar >> idPlayer >> nId >> nError;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		if (nError == 0)	// no error
			g_GuildMng.SetName(pGuild, lpszGuild);

		CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
		if (!IsValidObj((CObj*)pUser))
			return;

		switch (nError)
		{
		case 1:
			// error message	// have not guild
			pUser->AddDefinedText(TID_GAME_COMNOHAVECOM);
			break;
		case 2:
			// error message	// is not kingpin
			pUser->AddDefinedText(TID_GAME_COMDELNOTKINGPIN);
			break;
		case 3:
			// error message	// already exists
			pUser->AddDefinedText(TID_GAME_COMOVERLAPNAME);
			break;
		case 0:
			break;
		}

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction)
#endif // __S_SERVER_UNIFY
		{
			if (nError != 0)
			{
				CItemElem pItemElem;
				pItemElem.m_dwItemId = II_SYS_SYS_SCR_GCHANAM;
				pItemElem.m_nItemNum = 1;
				pItemElem.SetSerialNumber();

				if (pUser->CreateItem(&pItemElem) == FALSE)
					g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, pItemElem, 0, "FlyFF", "");
			}

			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(nId);
			if (pItemElem)
				pItemElem->m_bQuery = FALSE;
		}

#ifdef __S_SERVER_UNIFY
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
	}
}

void CDPCoreClient::OnGuildMsgControl(CAr& ar, DPID, DPID, OBJID)
{
	if (!g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
	{
		GUILD_MSG_HEADER	Header;
		DWORD				dwPenya;
		BYTE				cbCloak;

		ar.Read(&Header, sizeof(GUILD_MSG_HEADER));
		ar >> dwPenya;
		ar >> cbCloak;

		CGuild* pGuild = g_GuildMng.GetGuild(Header.HeadASub);
		if (pGuild)
		{
			CGuildMember* pMember;
			CUser* pUsertmp;
			for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
			{
				pMember = i->second;
				pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
				if (IsValidObj(pUsertmp))
				{
					pUsertmp->AddGetGoldGuildBank(dwPenya, 2, pMember->m_idPlayer, cbCloak);
				}
			}
		}
	}
}

BOOL CDPCoreClient::Contribute(CUser* pUser, DWORD dwPxpCount, DWORD dwPenya)
{
	u_long idGuild, idPlayer;
	idGuild = pUser->m_idGuild;
	idPlayer = pUser->m_idPlayer;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild == NULL)
		return FALSE;

	CGuildMember* pGuildMember = pGuild->GetMember(idPlayer);
	if (pGuildMember == NULL)
		return FALSE;


	int nLastGuildLv = pGuild->m_nLevel;
	if (pGuild->AddContribution(dwPxpCount, dwPenya, idPlayer) == FALSE)
		return FALSE;




	CONTRIBUTION_CHANGED_INFO info;

	info.idGuild = idGuild;
	info.idPlayer = idPlayer;
	info.dwPxpCount = dwPxpCount;
	info.dwPenya = dwPenya;
	info.dwGuildPxpCount = pGuild->m_dwContributionPxp;
	info.dwGuildPenya = pGuild->m_nGoldGuild;
	info.nGuildLevel = pGuild->m_nLevel;

#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
	if (pPlayerData)
		g_dpDBClient.SendGuildContribution(info, (nLastGuildLv < pGuild->m_nLevel ? 1 : 0), pPlayerData->data.nLevel);
#else	// __SYS_PLAYER_DATA
	g_dpDBClient.SendGuildContribution(info, (nLastGuildLv < pGuild->m_nLevel ? 1 : 0), pGuildMember->m_nLevel);
#endif	// __SYS_PLAYER_DATA

	{
		BEFORESENDDUAL(ar, PACKETTYPE_WC_GUILDCONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << ::g_uKey;
		ar << info;
		SEND(ar, this, DPID_SERVERPLAYER);
	}
	return TRUE;
}


BOOL CDPCoreClient::SendGuildStat(CUser* pUser, GUILD_STAT stat, DWORD data)
{
	BOOL bResult = TRUE;

	switch (stat)
	{
	case GUILD_STAT_PXPCOUNT:
		bResult = Contribute(pUser, data, 0);
		break;

	case GUILD_STAT_PENYA:
		bResult = Contribute(pUser, 0, data);
		break;

	case GUILD_STAT_NOTICE:
	{
		BEFORESENDDUAL(ar, PACKETTYPE_WC_GUILDNOTICE, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << pUser->m_idGuild << pUser->m_idPlayer;


		char szNotice[MAX_BYTE_NOTICE];
		strncpy(szNotice, (char*)data, MAX_BYTE_NOTICE);
		szNotice[MAX_BYTE_NOTICE - 1] = '\0';

		ar.WriteString(szNotice);
		SEND(ar, this, DPID_SERVERPLAYER);
	}
	break;
	case GUILD_STAT_LEVEL:
		bResult = Contribute(pUser, 0, 0);
		break;

	default:
		ASSERT(0);
	}

	return bResult;
}



void CDPCoreClient::OnSetFriendState(CAr& ar, DPID, DPID, OBJID)
{
	CUser* pUser;
	u_long uidPlayer;
	DWORD dwState;
	ar >> uidPlayer >> dwState;

	pUser = (CUser*)prj.GetUserByID(uidPlayer);
	if (IsValidObj(pUser))
		pUser->m_RTMessenger.SetState(dwState);
}

void CDPCoreClient::OnFriendInterceptState(CAr& ar, DPID, DPID, OBJID)
{
	u_long uidPlayer;
	u_long uidFriend;
	ar >> uidPlayer >> uidFriend;

	CUser* pUser, * pUserFriend;

	pUser = (CUser*)prj.GetUserByID(uidPlayer);
	pUserFriend = (CUser*)prj.GetUserByID(uidFriend);

	if (IsValidObj(pUser) == FALSE)
		return;

	Friend* pFriend = pUser->m_RTMessenger.GetFriend(uidFriend);
	if (pFriend)
	{
		if (pFriend->bBlock)
		{
			pFriend->bBlock = FALSE;
			if (IsValidObj(pUserFriend))
				pFriend->dwState = pUserFriend->m_RTMessenger.GetState();
			else
				pFriend->dwState = FRS_OFFLINE;
		}
		else
		{
			pFriend->bBlock = TRUE;
			pFriend->dwState = 0;
		}
	}
}

void CDPCoreClient::OnPartyChangeLeader(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId, idChangeLeader;
	ar >> uPartyId >> idChangeLeader;

	CParty* pParty;

	pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->ChangeLeader(idChangeLeader);

		CUser* pMember;
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			pMember = (CUser*)prj.GetUserByID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj(pMember))
				pMember->AddPartyChangeLeader(idChangeLeader);
		}

	}
}

void CDPCoreClient::OnGameRate(CAr& ar, DPID, DPID, OBJID)
{
	FLOAT fRate;
	BYTE nFlag;
	ar >> fRate;
	ar >> nFlag;
	switch (nFlag)
	{
	case GAME_RATE_SHOPCOST:
	{
		prj.m_fShopCost = fRate;
	}
	break;
	case GAME_RATE_ITEMDROP:
	{
		prj.m_fItemDropRate = fRate;
	}
	break;
	case GAME_RATE_GOLDDROP:
	{
		prj.m_fGoldDropRate = fRate;
	}
	break;
	case GAME_RATE_MONSTEREXP:
	{
		prj.m_fMonsterExpRate = fRate;
	}
	break;
	case GAME_RATE_MONSTERHIT:
	{
		prj.m_fMonsterHitRate = fRate;
	}
	break;
	case GAME_RATE_X3:
	{
		prj.m_fItemDropRate = fRate;
		prj.m_fGoldDropRate = fRate;
		prj.m_fMonsterExpRate = fRate;
	}
	break;
	}
}

void CDPCoreClient::OnLoadConstant(CAr& ar, DPID, DPID, OBJID)
{
	g_UserMng.AddGameSetting();
}

void CDPCoreClient::OnSetMonsterRespawn(CAr& ar, DPID, DPID, OBJID)
{
	u_long uidPlayer;
	DWORD dwMonsterID;
	DWORD dwRespawnNum;
	DWORD dwAttackNum;
	DWORD dwRect;
	DWORD dwRespawnTime;
	BOOL  bFlying;
	ar >> uidPlayer;
	ar >> dwMonsterID >> dwRespawnNum >> dwAttackNum >> dwRect >> dwRespawnTime;
	ar >> bFlying;

	D3DXVECTOR3 vPos;
	CWorld* pWorld = NULL;

	CUser* pUser = (CUser*)prj.GetUserByID(uidPlayer);
	if (IsValidObj((CObj*)pUser))
	{
		vPos = pUser->GetPos();
		pWorld = pUser->GetWorld();
	}

	if (pWorld != NULL)
	{
		CRespawnInfo ri;
		ri.m_dwType = OT_MOVER;
		ri.m_dwIndex = dwMonsterID;
		ri.m_cb = dwRespawnNum;
		ri.m_nActiveAttackNum = dwAttackNum;
		if (bFlying)
			ri.m_vPos = vPos;
		ri.m_rect.left = (LONG)(vPos.x - dwRect);
		ri.m_rect.right = (LONG)(vPos.x + dwRect);
		ri.m_rect.top = (LONG)(vPos.z - dwRect);
		ri.m_rect.bottom = (LONG)(vPos.z + dwRect);
		ri.m_uTime = (u_short)(dwRespawnTime);
		ri.m_cbTime = 0;

#ifdef __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add(ri, TRUE);
#else // __S1108_BACK_END_SYSTEM
		pWorld->m_respawner.Add(ri);
#endif // __S1108_BACK_END_SYSTEM

		char chMessage[512] = { 0, };
		sprintf(chMessage, "Add Respwan MonsterID = %d(%d/%d) Rect(%d, %d, %d, %d) Time = %d",
			ri.m_dwIndex, ri.m_cb, ri.m_nActiveAttackNum, ri.m_rect.left, ri.m_rect.right, ri.m_rect.top, ri.m_rect.bottom, ri.m_uTime);
		pUser->AddText(chMessage);

	}
}

void CDPCoreClient::OnSetPlayerName(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer;
	char lpszPlayer[MAX_PLAYER] = { 0, };
	DWORD dwData;
	BOOL f;

	ar >> idPlayer;
	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar >> dwData >> f;

	char lpOutputString[256] = { 0, };
	sprintf(lpOutputString, "WORLDSERVER.EXE\t// %d\t// %s\t// %08x\t// %d", idPlayer, lpszPlayer, dwData, f);
	OutputDebugString(lpOutputString);

	CUser* pUser = (CUser*)prj.GetUserByID(idPlayer);
	if (IsValidObj((CObj*)pUser))
	{
		if (f)
		{
			if (pUser->IsMode(QUERYSETPLAYERNAME_MODE))
			{
				pUser->SetNotMode(QUERYSETPLAYERNAME_MODE);
				g_UserMng.AddModifyMode(pUser);
			}

			pUser->SetName(lpszPlayer);
		}
		else
			pUser->AddDefinedText(TID_DIAG_0034);

#ifdef __S_SERVER_UNIFY
		if (pUser->m_bAllAction)
#endif // __S_SERVER_UNIFY
		{
			if (!f)
			{
				CItemElem pItemElem;
				pItemElem.m_dwItemId = II_SYS_SYS_SCR_CHANAM;
				pItemElem.m_nItemNum = 1;
				pItemElem.SetSerialNumber();

				if (pUser->CreateItem(&pItemElem) == FALSE)
					g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, pItemElem, 0, "FlyFF", "");
			}

			CItemElem* pItemElem = pUser->m_Inventory.GetAtId(dwData);
			if (pItemElem)
				pItemElem->m_bQuery = FALSE;
		}

#ifdef __S_SERVER_UNIFY
		pUser->SetAllAction();
#endif // __S_SERVER_UNIFY
	}

	if (f)
	{
		char lpOutputString[128] = { 0, };
		sprintf(lpOutputString, "WORLDSERVER.EXE\tPACKETTYPE_QUERYSETPLAYERNAME\t//%d:%s\t-%08x", idPlayer, lpszPlayer, dwData);
		OutputDebugString(lpOutputString);

		CPlayerDataCenter* pPlayerDataCenter = CPlayerDataCenter::GetInstance();
		PlayerData* pPlayerData = pPlayerDataCenter->GetPlayerData(idPlayer);
		if (pPlayerData)
		{
			PlayerData pd;
			memcpy(&pd, pPlayerData, sizeof(PlayerData));
			pPlayerDataCenter->DeletePlayerData(idPlayer);
			lstrcpy(pd.szPlayer, lpszPlayer);
			pPlayerDataCenter->AddPlayerData(idPlayer, pd);
		}
	}
}

void CDPCoreClient::SendQuerySetGuildName(u_long idPlayer, u_long idGuild, const char* lpszPlayer, BYTE nId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYSETGUILDNAME, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idGuild;
	ar.WriteString(lpszPlayer);
	ar << nId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendSetSnoop(u_long idPlayer, u_long idSnoop, BOOL bRelease)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SETSNOOP, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idSnoop << bRelease;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::SendChat(u_long idPlayer1, u_long idPlayer2, const char* lpszChat)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CHAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer1 << idPlayer2;
	ar.WriteString(lpszChat);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnSetSnoop(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer, idSnoop;
	BOOL bRelease;
	ar >> idPlayer >> idSnoop >> bRelease;

	CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
	if (IsValidObj((CObj*)pUser))
		pUser->m_idSnoop = idSnoop;
}

void CDPCoreClient::OnGuildCombatState(CAr& ar, DPID, DPID, DPID)
{
	int nState;
	ar >> nState;

	g_GuildCombatMng.m_nState = nState;
	if (g_GuildCombatMng.m_nState == CGuildCombat::OPEN_STATE)
		g_GuildCombatMng.m_dwTime = 0;

	g_UserMng.AddGuildCombatState();
}

void CDPCoreClient::OnRemoveUserFromCORE(CAr& ar, DPID, DPID, DPID)
{
	DWORD dwSerial;
	ar >> dwSerial;

	g_UserMng.RemoveUser(dwSerial);
}

void CDPCoreClient::SendPing(void)
{
	if (FALSE == m_bAlive)
	{
		Error("CORE : is not alive");
	}

	m_bAlive = FALSE;
	BEFORESENDDUAL(ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << time_null();
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnPing(CAr& ar, DPID, DPID, DPID)
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d = time_null() - tSend;
	CTime tmTrans(tTrans);
	if (d > 10)
		Error("ping CORE - %d, trans:%s", d, tmTrans.Format("%Y/%m/%d %H:%M:%S"));

	m_bAlive = TRUE;
}

void CDPCoreClient::OnDestroyPlayer(CAr& ar, DPID, DPID, OBJID)
{
	u_long idPlayer;
	ar >> idPlayer;
	CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
	if (pUser)
		g_DPSrvr.QueryDestroyPlayer(pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer);
}

void CDPCoreClient::OnInstanceDungeonAllInfo(CAr& ar, DPID, DPID, OBJID)
{
	CInstanceDungeonHelper::GetInstance()->OnInstanceDungeonAllInfo(ar);
}

void CDPCoreClient::OnInstanceDungeonCreate(CAr& ar, DPID, DPID, OBJID)
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info(NULL_ID, NULL_ID);

	ar >> nType >> dwDungeonId;
	ID_Info.Serialize(ar);

	CInstanceDungeonHelper::GetInstance()->OnCreateDungeon(nType, ID_Info, dwDungeonId);
}

void CDPCoreClient::SendInstanceDungeonCreate(int nType, DWORD dwDungeonId, ID_INFO& ID_Info)
{
	BEFORESENDDUAL(ar, PACKETTYPE_INSTANCEDUNGEON_CREATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nType << dwDungeonId;
	ID_Info.Serialize(ar);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnInstanceDungeonDestroy(CAr& ar, DPID, DPID, OBJID)
{
	int nType;
	DWORD dwDungeonId;
	ID_INFO ID_Info(NULL_ID, NULL_ID);

	ar >> nType >> dwDungeonId;
	ID_Info.Serialize(ar);

	CInstanceDungeonHelper::GetInstance()->OnDestroyDungeon(nType, ID_Info, dwDungeonId);
}

void CDPCoreClient::SendInstanceDungeonDestroy(int nType, DWORD dwDungeonId, ID_INFO& ID_Info)
{
	BEFORESENDDUAL(ar, PACKETTYPE_INSTANCEDUNGEON_DESTROY, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nType << dwDungeonId;
	ID_Info.Serialize(ar);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnInstanceDungeonSetCoolTimeInfo(CAr& ar, DPID, DPID, OBJID)
{
	int nType;
	DWORD dwPlayerId;
	COOLTIME_INFO CT_Info;
	ULONG uKey;

	ar >> uKey >> nType >> dwPlayerId;
	CT_Info.Serialize(ar);

	CInstanceDungeonHelper::GetInstance()->OnSetDungeonCoolTimeInfo(uKey, nType, CT_Info, dwPlayerId);
}

void CDPCoreClient::SendInstanceDungeonSetCoolTimeInfo(int nType, DWORD dwPlayerId, COOLTIME_INFO& CT_Info)
{
	BEFORESENDDUAL(ar, PACKETTYPE_INSTANCEDUNGEON_SETCOOLTIME, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << g_uKey << nType << dwPlayerId;
	CT_Info.Serialize(ar);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPCoreClient::OnInstanceDungeonDeleteCoolTimeInfo(CAr& ar, DPID, DPID, OBJID)
{
	int nType;
	DWORD dwPlayerId;

	ar >> nType >> dwPlayerId;

	CInstanceDungeonHelper::GetInstance()->OnDeleteDungeonCoolTimeInfo(nType, dwPlayerId);
}

void CDPCoreClient::OnUpdatePartySkills(CAr& ar, DPID, DPID, OBJID)
{
	u_long	idMember, idParty;
	ar >> idMember >> idParty;
	int nModeTime[MAX_PARTYMODE];
	ar.Read(nModeTime, sizeof(int) * MAX_PARTYMODE);

	CUser* pUser = (CUser*)prj.GetUserByID(idMember);
	if (!IsValidObj(pUser))
		return;

	CParty* pParty = g_PartyMng.GetParty(idParty);
	if (!pParty || !pParty->IsMember(pUser->m_idPlayer))
		return;

	if (!pParty->m_nModeTime[PARTY_PARSKILL_MODE])
	{
		if (pUser->IsSMMode(SM_PARTYSKILL1) || pUser->IsSMMode(SM_PARTYSKILL15) || pUser->IsSMMode(SM_PARTYSKILL30))
			g_DPCoreClient.SendUserPartySkill(pUser->m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
	}

	for (int i = 0; i < MAX_PARTYMODE; ++i)
	{
		if (nModeTime[i])
			pUser->AddSetPartyMode(i, TRUE, 0, (DWORD)nModeTime[i]);
	}

}

#ifdef __FL_PARTY_SORT
void CDPCoreClient::OnSortPartyMembers(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId, idChangeMember;
	BOOL bUpDown;
	ar >> uPartyId >> idChangeMember >> bUpDown;

	CParty* pParty;

	pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		if (pParty->ChangePosition(idChangeMember, bUpDown))
		{
			CUser* pMember;
			for (int i = 0; i < pParty->m_nSizeofMember; i++)
			{
				pMember = (CUser*)prj.GetUserByID(pParty->m_aMember[i].m_uPlayerId);
				if (IsValidObj(pMember))
					pMember->AddPartyChangePosition(idChangeMember, bUpDown);
			}
		}
	}
}
#endif //

#ifdef __FL_GW_STATUE_V2
void CDPCoreClient::OnStatueSettings(CAr& ar, DPID, DPID, OBJID)
{
	int nStatueType;
	int nStatuePrice;
	u_long dwUserID;

	ar >> nStatueType;
	ar >> nStatuePrice;
	ar >> dwUserID;

	// Validation is not needed here because we validate all in first step, so just apply it
	if (nStatueType == NPC_TYPE_MVPSTATUE)
		prj.SetCurrentMvpStatuePrice(nStatuePrice);
	else if (nStatueType == NPC_TYPE_GUILDLEADERSTATUE)
		prj.SetCurrentLeaderStatuePrice(nStatuePrice);

	CUser* pUser = (CUser*)prj.GetUserByID(dwUserID);
	if (IsValidObj(pUser))
		pUser->AddText("Statue parameters was applied successfully.");
}
#endif

#ifdef __VALID_CONNECTION_CONFORMATION
void CDPCoreClient::OnConfirmPlayer(CAr& ar, DPID, DPID, OBJID)
{
	DWORD serial;
	ar >> serial;

	CUser* pUser = g_UserMng.GetUserBySerial(serial);
	if (pUser)
	{
		SendConfirmPlayer(serial, true);
	}
	else
		SendConfirmPlayer(serial, false);
}

void CDPCoreClient::SendConfirmPlayer(DWORD serial, bool state)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CONNECTION_CONFORMATION, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << serial << state;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_PARTY_FINDER
void CDPCoreClient::OnPartyChangeJoinMode(CAr& ar, DPID, DPID, OBJID)
{
	u_long uPartyId;
	BOOL bMode;
	ar >> uPartyId >> bMode;

	CParty* pParty = g_PartyMng.GetParty(uPartyId);
	if (pParty)
	{
		pParty->m_bAllowed = bMode;

		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUser = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj((CObj*)pUser))
				pUser->AddPartyChangeJoinMode(bMode);
		}
	}
}
#endif