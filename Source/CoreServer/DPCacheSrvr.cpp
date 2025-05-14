#include "StdAfx.h"
#include "dpcoresrvr.h"
#include "dpdatabaseclient.h"
#include "dploginsrvr.h"
extern	CDPLoginSrvr	g_dpLoginSrvr;
#include "dpcachesrvr.h"
#include "msghdr.h"
#include "mytrace.h"
#include "player.h"
#include "misc.h"
#include "party.h"
#include "definetext.h"

extern	CPlayerMng	g_PlayerMng;
extern	CDPCoreSrvr	g_dpCoreSrvr;
extern	CDPDatabaseClient	g_dpDatabaseClient;
extern	CMyTrace	g_MyTrace;
extern	CPartyMng	g_PartyMng;

#include "guild.h"
#include "guildwar.h"
extern	CGuildMng	g_GuildMng;

#include "lang.h"

// %Y%m%d%H%M%S

CDPCacheSrvr::CDPCacheSrvr()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_JOIN, &CDPCacheSrvr::OnAddPlayer);
	ON_MSG(PACKETTYPE_DESTROY_PLAYER, &CDPCacheSrvr::OnQueryRemovePlayer);
	ON_MSG(PACKETTYPE_REMOVEPARTYMEMBER, &CDPCacheSrvr::OnRemovePartyMember);
	ON_MSG(PACKETTYPE_CHANGETROUP, &CDPCacheSrvr::OnPartyChangeTroup);
	ON_MSG(PACKETTYPE_CHANPARTYNAME, &CDPCacheSrvr::OnPartyChangeName);
	ON_MSG(PACKETTYPE_PARTYCHANGEITEMMODE, &CDPCacheSrvr::OnPartyChangeItemMode);
	ON_MSG(PACKETTYPE_PARTYCHANGEEXPMODE, &CDPCacheSrvr::OnPartyChangeExpMode);
	ON_MSG(PACKETTYPE_GETFRIENDSTATE, &CDPCacheSrvr::OnGetFriendState);
	ON_MSG(PACKETTYPE_SETFRIENDSTATE, &CDPCacheSrvr::OnSetFrinedState);
	ON_MSG(PACKETTYPE_FRIENDINTERCEPTSTATE, &CDPCacheSrvr::OnFriendInterceptState);
	ON_MSG(PACKETTYPE_REMOVEFRIEND, &CDPCacheSrvr::OnRemoveFriend);
	ON_MSG(PACKETTYPE_GUILD_SETNAME, &CDPCacheSrvr::OnGuildSetName);
	ON_MSG(PACKETTYPE_GUILD_AUTHORITY, &CDPCacheSrvr::OnGuildAuthority);
#ifdef __FL_GUILD_FINDER
	ON_MSG(PACKETTYPE_GUILD_FINDERSTATE, &CDPCacheSrvr::OnGuildFinderState);
#endif
	ON_MSG(PACKETTYPE_DESTROY_GUILD, &CDPCacheSrvr::OnDestroyGuild);
	ON_MSG(PACKETTYPE_REMOVE_GUILD_MEMBER, &CDPCacheSrvr::OnRemoveGuildMember);
	ON_MSG(PACKETTYPE_GUILD_MEMBER_LEVEL, &CDPCacheSrvr::OnGuildMemberLv);
	ON_MSG(PACKETTYPE_GUILD_CLASS, &CDPCacheSrvr::OnGuildClass);
	ON_MSG(PACKETTYPE_GUILD_NICKNAME, &CDPCacheSrvr::OnGuildNickName);
	ON_MSG(PACKETTYPE_PARTYCHANGELEADER, &CDPCacheSrvr::OnPartyChangeLeader);
	ON_MSG(PACKETTYPE_CHG_MASTER, &CDPCacheSrvr::OnChgMaster);
#ifdef __FL_PARTY_SORT
	ON_MSG(PACKETTYPE_MOVE_PARTYMEMBERS, &CDPCacheSrvr::OnSortPartyMembers);
#endif //
#ifdef __VALID_CONNECTION_CONFORMATION
	ON_MSG(PACKETTYPE_CONNECTION_CONFORMATION, &CDPCacheSrvr::OnConfirmPlayer);
#endif
#ifdef __FL_PARTY_FINDER
	ON_MSG(PACKETTYPE_PARTYCHANGEJOINMODE, &CDPCacheSrvr::OnPartyChangeJoinMode);
#endif
}

CDPCacheSrvr::~CDPCacheSrvr()
{

}

void CDPCacheSrvr::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
	{
		LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
		OnAddConnection(lpCreatePlayer->dpId);
		break;
	}
	case DPSYS_DESTROYPLAYERORGROUP:
	{
		LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
		OnRemoveConnection(lpDestroyPlayer->dpId);
		break;
	}
	}
}

void CDPCacheSrvr::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	CAr ar((LPBYTE)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID));
	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn) {
		(this->*(pfn))(ar, idFrom, *(UNALIGNED LPDPID)lpMsg, dwMsgSize - sizeof(DPID) - sizeof(DWORD));
	}
	else {
		TRACE("Handler not found(%08x)\n", lpMsg->dwType);
	}
}

void CDPCacheSrvr::SendProcServerList(DPID dpid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_PROCSERVER_LIST, DPID_UNKNOWN);

	ar << (short)g_dpCoreSrvr.m_apServer.size();
	for (CServerDescArray::iterator i = g_dpCoreSrvr.m_apServer.begin(); i != g_dpCoreSrvr.m_apServer.end(); ++i)
		i->second->Serialize(ar);

	SEND(ar, this, dpid);
}

DPID	s_Cachedpid = 0xFFFFFFFF;
void CDPCacheSrvr::OnAddConnection(DPID dpid)
{
	if (s_Cachedpid == 0xFFFFFFFF)
	{
		s_Cachedpid = dpid;
		SendProcServerList(dpid);

		CServerDesc* pServer = new CServerDesc;
		GetPlayerAddr(dpid, pServer->m_szAddr);
		bool bResult = m_apServer.insert(CServerDescArray::value_type(dpid, pServer)).second;
		ASSERT(bResult);
		g_MyTrace.Add(CMyTrace::Key(pServer->m_szAddr), FALSE, "%s", pServer->m_szAddr);
		g_PlayerMng.AddCache(dpid);
	}
	else
	{
		CServerDesc* pServer = new CServerDesc;
		GetPlayerAddr(dpid, pServer->m_szAddr);
		Error("Other Cache Connection - IP : %s", pServer->m_szAddr);
		SAFE_DELETE(pServer);
	}
}

void CDPCacheSrvr::OnRemoveConnection(DPID dpid)
{
	if (s_Cachedpid == dpid)
	{
		s_Cachedpid = 0xFFFFFFFF;
		CServerDesc* pServer = m_apServer.GetAt(dpid);
		m_apServer.erase(dpid);
		if (pServer)
			g_MyTrace.Add(CMyTrace::Key(pServer->m_szAddr), TRUE, "%s", pServer->m_szAddr);
		SAFE_DELETE(pServer);
		g_PlayerMng.RemoveCache(dpid);
	}
}

void CDPCacheSrvr::OnAddPlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	char lpszPlayer[MAX_PLAYER];
	char lpszAccount[MAX_ACCOUNT];
	DWORD dwAuthKey;
	u_long idPlayer, dpidSocket;

	ar >> dwAuthKey;
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	ar >> idPlayer;
	ar >> dpidSocket;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CPlayer* pPlayer = g_PlayerMng.GetPlayer(idPlayer);
	if (pPlayer)
	{
		if (pPlayer->m_dwAuthKey != dwAuthKey)
		{
			Error("CDPCacheSrvr::OnAddPlayer() wrong auth key. ID = %d, legal key = %d, received key = %d", idPlayer, pPlayer->m_dwAuthKey, dwAuthKey);
			return;
		}

		if (lstrcmp(lpszAccount, pPlayer->lpszAccount) != 0)
		{
			Error("CDPCacheSrvr::OnAddPlayer() wrong account. ID = %d, legal account = %s, received account = %s", idPlayer, pPlayer->lpszAccount, lpszAccount);
			return;
		}

		CPlayer* p;
		p = g_PlayerMng.GetPlayerBySocket(dpidSocket);
		if (p)
			g_PlayerMng.UnregisterPlayerInfo(p, FALSE);

		pPlayer->m_dwSerial = dpidUser;
		pPlayer->dpidCache = dpidCache;
		pPlayer->dpidUser = dpidSocket;
		ar >> pPlayer->m_uPartyId;

		g_GuildMng.m_AddRemoveLock.Enter(theLineFile);
		ar >> pPlayer->m_idGuild;
		if (NULL == g_GuildMng.GetGuild(pPlayer->m_idGuild))
			pPlayer->m_idGuild = 0;

		g_GuildMng.m_AddRemoveLock.Leave(theLineFile);

		ar >> pPlayer->m_uIdofMulti;
		pPlayer->m_RTMessenger.Serialize(ar);
		ar.ReadString(lpszPlayer, MAX_PLAYER);
		ar.ReadString(pPlayer->lpAddr, 16);

		g_PlayerMng.RegisterPlayerInfo(pPlayer);

		BEFORESENDSOLE(out, PACKETTYPE_JOIN, pPlayer->dpidUser);
		out << pPlayer->m_dwSerial;
		out << (BYTE)1;
		SEND(out, this, dpidCache);
	}
}

void CDPCacheSrvr::OnQueryRemovePlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (pPlayer)
	{
		if (*pPlayer->lpszAccount != '\0')
			g_dpLoginSrvr.SendQueryRemovePlayer(pPlayer->lpszAccount);
		else
			WriteLog("OnQueryRemovePlayer(): lpszAccount == '\0'");
		g_PlayerMng.RemovePlayer(pPlayer);
	}
}

void CDPCacheSrvr::SendWhisper(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch)
{
	if (!pTo)
		return;
	//	ASSERT( pTo );

	BEFORESENDSOLE(ar, PACKETTYPE_WHISPER, pTo->dpidUser);
	ar.WriteString(sPlayerFrom);
	ar.WriteString(sPlayerTo);
	ar.WriteString(lpString);
	ar << idFrom << idTo;
	ar << nSearch;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendSay(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, u_long idFrom, u_long idTo, const CHAR* lpString, CPlayer* pTo, int nSearch)
{
	if (!pTo)
		return;
	//	ASSERT( pTo );

	BEFORESENDSOLE(ar, PACKETTYPE_SAY, pTo->dpidUser);
	ar.WriteString(sPlayerFrom);
	ar.WriteString(sPlayerTo);
	ar.WriteString(lpString);
	ar << idFrom << idTo;
	ar << nSearch;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendGMSay(const CHAR* sPlayerFrom, const CHAR* sPlayerTo, const CHAR* lpString, CPlayer* pTo)
{
	if (!pTo)
		return;
	//	ASSERT( pTo );

	BEFORESENDSOLE(ar, PACKETTYPE_GMSAY, pTo->dpidUser);
	ar.WriteString(sPlayerFrom);
	ar.WriteString(sPlayerTo);
	ar.WriteString(lpString);
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendFriendState(CPlayer* pTo)
{
	vector< u_long > vecIdFriend;
	vector< u_long > vecIdBlock;

	DWORD dwState;
	u_long uIdofMulti;
	BEFORESENDSOLE(ar, PACKETTYPE_GETFRIENDSTATE, pTo->dpidUser);

	pTo->Lock();
	for (map<u_long, Friend>::iterator i = pTo->m_RTMessenger.begin(); i != pTo->m_RTMessenger.end(); ++i)
	{
		Friend* pFriend = &i->second;
		if (!pFriend->bBlock)
			vecIdFriend.push_back(i->first);
		else
			vecIdBlock.push_back(i->first);
	}
	pTo->Unlock();

	ar << static_cast<int>(vecIdFriend.size());
	ar << static_cast<int>(vecIdBlock.size());

	for (DWORD j = 0; j < vecIdFriend.size(); j++)
	{
		CPlayer* pPlayer = g_PlayerMng.GetPlayer(vecIdFriend[j]);
		if (pPlayer)
		{
			pPlayer->Lock();
			Friend* pFriend = pPlayer->m_RTMessenger.GetFriend(pTo->uKey);
			if (pFriend)
			{
				if (pFriend->bBlock)
					dwState = FRS_OFFLINE;
				else
					dwState = pPlayer->m_RTMessenger.GetState();
			}
			else
				dwState = FRS_OFFLINE;

			pPlayer->Unlock();
		}
		else
			dwState = FRS_OFFLINE;

		ar << vecIdFriend[j] << dwState;
		uIdofMulti = (pPlayer ? pPlayer->m_uIdofMulti : 100);
		ar << uIdofMulti;
	}

	for (DWORD k = 0; k < vecIdBlock.size(); k++)
	{
		CPlayer* pPlayer = g_PlayerMng.GetPlayer(vecIdBlock[k]);
		if (pPlayer)
		{
			pPlayer->Lock();
			Friend* pFriend = pPlayer->m_RTMessenger.GetFriend(pTo->uKey);
			if (pFriend)
			{
				if (pFriend->bBlock)
					dwState = FRS_OFFLINE;
				else
					dwState = pPlayer->m_RTMessenger.GetState();
			}
			else
			{
				dwState = FRS_OFFLINE;
			}
			pPlayer->Unlock();
		}
		else
		{
			dwState = FRS_OFFLINE;
		}
		ar << vecIdBlock[k] << dwState;
		uIdofMulti = (pPlayer ? pPlayer->m_uIdofMulti : 100);
		ar << uIdofMulti;
	}

	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendSetFriendState(CPlayer* pTo)
{
	vector< u_long > vecIdFriend;

	pTo->Lock();
	u_long idPlayer = pTo->uKey;
	DWORD  dwState = pTo->m_RTMessenger.GetState();
	for (map<u_long, Friend>::iterator i = pTo->m_RTMessenger.begin(); i != pTo->m_RTMessenger.end(); ++i)
	{
		Friend* pFriend = &i->second;
		if (!pFriend->bBlock)
			vecIdFriend.push_back(i->first);
	}

	BEFORESENDSOLE(ar, PACKETTYPE_SETFRIENDSTATE, pTo->dpidUser);
	ar << idPlayer;
	ar << dwState;
	SEND(ar, this, pTo->dpidCache);

	pTo->Unlock();

	for (DWORD j = 0; j < vecIdFriend.size(); j++)
	{
		CPlayer* pPlayer = g_PlayerMng.GetPlayer(vecIdFriend[j]);
		if (pPlayer)
		{
			pPlayer->Lock();
			Friend* pFriend = pPlayer->m_RTMessenger.GetFriend(idPlayer);
			if (pFriend)
			{
				pFriend->dwState = dwState;
				BEFORESENDSOLE(ar, PACKETTYPE_SETFRIENDSTATE, pPlayer->dpidUser);
				ar << idPlayer;
				ar << dwState;
				SEND(ar, this, pPlayer->dpidCache);
			}
			pPlayer->Unlock();
		}
	}
}

void CDPCacheSrvr::SendFriendJoin(CPlayer* pTo, CPlayer* pFriend)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDJOIN, pTo->dpidUser);

	ar << pFriend->uKey;
	ar << pFriend->m_RTMessenger.GetState();
	ar << pFriend->m_uIdofMulti;

	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendFriendLogOut(CPlayer* pTo, u_long uidPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDLOGOUT, pTo->dpidUser);
	ar << uidPlayer;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendFriendNoIntercept(CPlayer* pTo, CPlayer* pFriend, int state)
{
	BEFORESENDSOLE(ar, PACKETTYPE_FRIENDNOINTERCEPT, pTo->dpidUser);
	ar << pFriend->uKey;
	ar << state;
	SEND(ar, this, pTo->dpidCache);

	SendFriendJoin(pFriend, pTo);
}

void CDPCacheSrvr::SendFriendNoIntercept(CPlayer* pTo, u_long uFriendid, int state)
{
	BEFORESENDSOLE(ar, PACKETTYPE_FRIENDNOINTERCEPT, pTo->dpidUser);
	ar << uFriendid;
	ar << state;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendFriendIntercept(CPlayer* pPlayer, CPlayer* pFriend)
{
	{
		BEFORESENDSOLE(ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pPlayer->dpidUser);
		ar << pPlayer->uKey << pFriend->uKey;
		SEND(ar, this, pPlayer->dpidCache);
	}
	{
		BEFORESENDSOLE(ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pFriend->dpidUser);
		ar << pPlayer->uKey << pFriend->uKey;
		SEND(ar, this, pFriend->dpidCache);
	}
}

void CDPCacheSrvr::SendFriendIntercept(CPlayer* pPlayer, u_long uFriendid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_FRIENDINTERCEPTSTATE, pPlayer->dpidUser);
	ar << pPlayer->uKey << uFriendid;
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendOneFriendState(CPlayer* pTo, u_long uidPlayer, DWORD dwState)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ONEFRIEMDSTATE, pTo->dpidUser);
	ar << uidPlayer << dwState;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendModifyMode(DWORD dwMode, BYTE f, u_long idFrom, CPlayer* pTo)
{
	if (!pTo)
		return;
	//	ASSERT( pTo );

	BEFORESENDSOLE(ar, PACKETTYPE_MODIFYMODE, pTo->dpidUser);
	ar << dwMode << f << idFrom;
	ar << pTo->dpidUser;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendKillPlayer(CPlayer* pPlayer)
{
	SendHdr(PACKETTYPE_KILLPLAYER, pPlayer->dpidCache, pPlayer->dpidUser);
}

void CDPCacheSrvr::SendGetPlayerAddr(const CHAR* lpszPlayer, const CHAR* lpAddr, CPlayer* pOperator)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETPLAYERADDR, pOperator->dpidUser);
	ar.WriteString(lpszPlayer);
	ar.WriteString(lpAddr);
	SEND(ar, this, pOperator->dpidCache);
}

void CDPCacheSrvr::SendGetPlayerCount(u_short uCount, CPlayer* pOperator)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETPLAYERCOUNT, pOperator->dpidUser);
	ar << uCount;
	SEND(ar, this, pOperator->dpidCache);
}

void CDPCacheSrvr::SendGetCorePlayer(CPlayer* pOperator)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GETCOREPLAYER, pOperator->dpidUser);
	g_PlayerMng.PackName(ar);
	SEND(ar, this, pOperator->dpidCache);
}

void CDPCacheSrvr::SendSystem(const CHAR* lpString)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SYSTEM, DPID_ALLPLAYERS);
	ar.WriteString(lpString);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCacheSrvr::SendCaption(const CHAR* lpString, DWORD dwWorldId, BOOL bSmall)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CAPTION, DPID_ALLPLAYERS);
	ar << bSmall;
	ar << dwWorldId;
	ar.WriteString(lpString);
	SEND(ar, this, DPID_ALLPLAYERS);
}


void CDPCacheSrvr::SendGameRate(FLOAT fRate, BYTE nFlag)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GAMERATE, DPID_ALLPLAYERS);
	ar << fRate;
	ar << nFlag;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCacheSrvr::SendDefinedText(int dwText, DPID dpidCache, DPID dpidUser, LPCSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	TCHAR szBuffer[128];
	nBuf = _vsntprintf(szBuffer, 128, lpszFormat, args);
	va_end(args);

	BEFORESENDSOLE(ar, PACKETTYPE_DEFINEDTEXT, dpidUser);
	ar << dwText;
	ar.WriteString(szBuffer);
	SEND(ar, this, dpidCache);
}

void CDPCacheSrvr::OnPartyChangeLeader(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long idChangeLeader;
	ar >> idChangeLeader;

	CPlayer* pLeader;
	CPlayer* pChangeLeader;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pLeader = g_PlayerMng.GetPlayerBySerial(dpidUser);
	pChangeLeader = g_PlayerMng.GetPlayer(idChangeLeader);

	if (!pLeader)
	{
		//		player not found
		return;
	}

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pLeader->m_uPartyId);
	if (!pParty || !pParty->IsMember(pLeader->uKey))
	{
		// leader's party not found
		// OR is not party member
		pLeader->m_uPartyId = 0;
		return;
	}
	if (pLeader->uKey != idChangeLeader)
	{
		if (FALSE == pParty->IsLeader(pLeader->uKey))
		{
			// is not leader
			return;
		}
		if (!pParty->IsMember(idChangeLeader))
		{
			// pMember is not pLeader's party member
			SendErrorParty(ERROR_NOTMAMBER, pLeader);
			return;
		}
	}
	else
	{
		SendErrorParty(ERROR_SAMLEADER, pLeader);
		return;
	}

	int nFind = pParty->FindMember(idChangeLeader);
	if (nFind == -1 || pParty->m_aMember[nFind].m_bRemove)
		return;

	pParty->ChangeLeader(idChangeLeader);
	g_dpCoreSrvr.SendChangeLeader(pParty->m_uPartyId, idChangeLeader);
}

void CDPCacheSrvr::OnRemovePartyMember(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long idMember;
	ar >> idMember;

	CPlayer* pLeader;
	CPlayer* pMember;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pLeader = g_PlayerMng.GetPlayerBySerial(dpidUser);
	pMember = g_PlayerMng.GetPlayer(idMember);

	if (!pLeader)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pLeader->m_uPartyId);
	if (!pParty || !pParty->IsMember(pLeader->uKey))
	{
		// leader's party not found
		// OR is not party member
		pLeader->m_uPartyId = 0;
		return;
	}

	if (pLeader->uKey != idMember)
	{
		if (FALSE == pParty->IsLeader(pLeader->uKey))
		{
			// is not leader
			return;
		}
		if (!pParty->IsMember(idMember))
		{
			// pMember is not pLeader's party member
			SendErrorParty(ERROR_DIFFRENTPARTY, pLeader);
			return;
		}
	}
#if __VER >= 12 // __PARSKILL1001
	BOOL bLeader = pParty->IsLeader(idMember);
#endif // ____PARSKILL1001
	if (pParty->DeleteMember(idMember))
	{
		if (pMember)
			pMember->m_uPartyId = 0;
		g_dpCoreSrvr.SendRemoveParty(pParty->m_uPartyId, pLeader->uKey, idMember);
		if (pParty->GetSizeofMember() < 2)
		{
			CPlayer* pLeadertmp = g_PlayerMng.GetPlayer(pParty->GetPlayerId(0));
			if (pLeadertmp)
				pLeadertmp->m_uPartyId = 0;
			g_PartyMng.DeleteParty(pParty->m_uPartyId);
		}
#if __VER >= 12
		else
		{
			if (bLeader)
			{
				u_long idChangeLeader = pParty->GetPlayerId(0);
				g_dpCoreSrvr.SendChangeLeader(pParty->m_uPartyId, idChangeLeader);
			}

			if (pParty->m_nModeTime[PARTY_PARSKILL_MODE])
				g_dpCoreSrvr.SendSetPartyMode(pParty->m_uPartyId, PARTY_PARSKILL_MODE, FALSE);
		}
#endif
	}
}

void CDPCacheSrvr::OnPartyChangeName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	if (uBufSize > 40)	return;		// 4 + 4 + 32

	TCHAR	sParty[33];	// 32
	ar.ReadString(sParty, 33);
	CPlayer* pPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (!pPlayer)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pPlayer->m_uPartyId);
	if (!pParty || FALSE == pParty->IsMember(pPlayer->uKey))
	{
		// party not found
		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if (pParty->IsLeader(pPlayer->uKey))
	{
		if (pParty->m_nKindTroup == 1)
		{
			if (g_PartyMng.IsPartyName(sParty) == FALSE)
			{
				strcpy(pParty->m_sParty, sParty);
				g_PartyMng.AddPartyName(pPlayer->uKey, sParty);
				g_dpDatabaseClient.SendAddPartyName(pPlayer->uKey, sParty);
				g_dpCoreSrvr.SendPartyChangeName(pParty->m_uPartyId, sParty);
			}
			else
				SendErrorParty(ERROR_DIFFERNTUSERNAME, pPlayer);
		}
	}
}



void CDPCacheSrvr::OnPartyChangeItemMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	int nItemMode;
	ar >> nItemMode;

	CPlayer* pPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (!pPlayer)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pPlayer->m_uPartyId);
	if (NULL == pParty)
	{

		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if (pParty->IsLeader(pPlayer->uKey))
	{
		pParty->m_nTroupeShareItem = nItemMode;
		g_dpCoreSrvr.SendPartyChangeItemMode(pPlayer->m_uPartyId, nItemMode);
	}
	else
	{

	}
}

void CDPCacheSrvr::OnPartyChangeExpMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	int nExpMode;
	ar >> nExpMode;

	CPlayer* pPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (!pPlayer)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pPlayer->m_uPartyId);
	if (NULL == pParty)
	{

		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if (pParty->IsLeader(pPlayer->uKey))
	{
		if (pParty->m_nKindTroup == 1)
		{
			pParty->m_nTroupsShareExp = nExpMode;
			g_dpCoreSrvr.SendPartyChangeExpMode(pPlayer->m_uPartyId, nExpMode);
		}
	}
}

void CDPCacheSrvr::OnPartyChangeTroup(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	BOOL   bSendName;
	char szPartyName[33];
	ar >> bSendName;
	if (bSendName)
	{
		ar.ReadString(szPartyName, 33);
	}

	CPlayer* pPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (!pPlayer)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pPlayer->m_uPartyId);
	if (NULL == pParty)
	{

		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if (pParty->IsLeader(pPlayer->uKey))
	{
		if (pParty->m_nKindTroup != 1 && pParty->GetLevel() >= 10)
		{
			LPCSTR lpszPlayer = g_PartyMng.GetPartyString(pPlayer->uKey);
			if (lpszPlayer != NULL && 0 == strcmp(lpszPlayer, szPartyName))
			{
				strcpy(pParty->m_sParty, lpszPlayer);
				pParty->m_nKindTroup = 1;
				g_dpCoreSrvr.SendPartyChangeTroup(pParty->m_uPartyId, pParty->m_sParty);
			}
			else
			{
				if (g_PartyMng.IsPartyName(szPartyName) == FALSE)
				{
					pParty->m_nKindTroup = 1;
					g_dpCoreSrvr.SendPartyChangeTroup(pParty->m_uPartyId, pParty->m_sParty);

					if (lpszPlayer != NULL)
						g_PartyMng.RemovePartyName(pPlayer->uKey, lpszPlayer);

					g_PartyMng.AddPartyName(pPlayer->uKey, szPartyName);
					g_dpDatabaseClient.SendAddPartyName(pPlayer->uKey, szPartyName);
					g_dpCoreSrvr.SendPartyChangeName(pParty->m_uPartyId, szPartyName);
					strcpy(pParty->m_sParty, szPartyName);
				}
				else
					SendErrorParty(ERROR_DIFFERNTUSERNAME, pPlayer);
			}
		}
	}
}


void CDPCacheSrvr::OnDestroyGuild(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (!pMaster)
	{
		// player not found
		return;
	}

	CGuild* pGuild;
	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);
	pGuild = g_GuildMng.GetGuild(pMaster->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pMaster->uKey))
	{
		// guild not found
		// OR is not member
		SendDefinedText(TID_GAME_COMNOHAVECOM, pMaster->dpidCache, pMaster->dpidUser, "");
		pMaster->m_idGuild = 0;
		return;
	}

	if (FALSE == pGuild->IsMaster(pMaster->uKey))
	{
		// is not leader
		SendDefinedText(TID_GAME_COMDELNOTKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}

	CGuildMember* pMember;
	CPlayer* pPlayer;
	for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i)
	{
		pMember = i->second;
		pPlayer = g_PlayerMng.GetPlayer(pMember->m_idPlayer);
		if (pPlayer)
		{
			pPlayer->m_idGuild = 0;
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
			pPlayer->m_tGuildMember = CTime::GetCurrentTime();
			pPlayer->m_tGuildMember += CTimeSpan(2, 0, 0, 0);
#endif
		}

	}
	u_long idGuild = pGuild->m_idGuild;
	g_GuildMng.RemoveGuild(idGuild);
	g_dpDatabaseClient.SendDestroyGuild(idGuild, pMaster->uKey);	// databaseserver
	g_dpCoreSrvr.SendDestroyGuild(idGuild);	// worldserver
}

void CDPCacheSrvr::OnRemoveGuildMember(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long idPlayer;
	ar >> idPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer* pPlayer = g_PlayerMng.GetPlayer(idPlayer);

	if (!pMaster)
		return;

	CGuild* pGuild;
	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);
	//	1
	pGuild = g_GuildMng.GetGuild(pMaster->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pMaster->uKey))
	{
		// guild not found
		// OR is not member
		SendDefinedText(TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "");
		pMaster->m_idGuild = 0;
		return;
	}

	if (pMaster->uKey != idPlayer)
	{
		//		2
		if (FALSE == pGuild->IsMember(idPlayer))
		{
			// is not member
			SendDefinedText(TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild);
			return;
		}
		//		3
		if (FALSE == pGuild->IsMaster(pMaster->uKey))
		{
			// is not leader
			SendDefinedText(TID_GAME_COMLEAVENOKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "");
			return;
		}
	}
	else
	{
		//		4
		if (pGuild->IsMaster(pMaster->uKey))
		{
			// do not remove leader
			SendDefinedText(TID_GAME_COMLEAVEKINGPIN, pMaster->dpidCache, pMaster->dpidUser, "");
			return;
		}
	}

	if (pGuild->RemoveMember(idPlayer))
	{
		if (pPlayer)
		{
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
			pPlayer->m_tGuildMember = CTime::GetCurrentTime();
			pPlayer->m_tGuildMember += CTimeSpan(2, 0, 0, 0);
#endif
			pPlayer->m_idGuild = 0;
			SendRemoveGuildMember(idPlayer, pGuild->m_idGuild, pPlayer);
		}

		CPlayer* pPlayertmp;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i)
		{
			pPlayertmp = g_PlayerMng.GetPlayer(i->second->m_idPlayer);
			if (pPlayertmp)
				SendRemoveGuildMember(idPlayer, pGuild->m_idGuild, pPlayertmp);
		}

		g_dpCoreSrvr.SendRemoveGuildMember(idPlayer, pGuild->m_idGuild);
		g_dpDatabaseClient.SendRemoveGuildMember(idPlayer, pGuild->m_idGuild, pMaster->uKey);
	}
}

void CDPCacheSrvr::OnGuildMemberLv(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long _idMaster, idPlayer;
	int nMemberLv;
	ar >> _idMaster >> idPlayer >> nMemberLv;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer* pPlayer = g_PlayerMng.GetPlayer(idPlayer);

	if (!pMaster)
		return;

	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CGuild* pGuild = g_GuildMng.GetGuild(pMaster->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pMaster->uKey))
	{
		// guild not found
		// OR is not member
		SendDefinedText(TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "");
		pMaster->m_idGuild = 0;
		return;
	}

	if (FALSE == pGuild->IsMember(idPlayer))
	{
		// is not member
		SendDefinedText(TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild);
		return;
	}
	CGuildMember* pMember1 = pGuild->GetMember(pMaster->uKey);
	CGuildMember* pMember2 = pGuild->GetMember(idPlayer);
	if (pMember1->m_nMemberLv >= pMember2->m_nMemberLv)
	{
		SendDefinedText(TID_GAME_GUILDAPPOVER, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}
	if (pMember1->m_nMemberLv >= nMemberLv)
	{
		SendDefinedText(TID_GAME_GUILDWARRANTREGOVER, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}
	if (!pGuild->IsCmdCap(pMember1->m_nMemberLv, PF_MEMBERLEVEL))
	{
		SendDefinedText(TID_GAME_GUILDAPPNOTWARRANT, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}

	if (nMemberLv < 0 || nMemberLv >= 5)
	{
		SendDefinedText(TID_GAME_GUILDWARRANTREGOVER, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}

	if ((pGuild->GetMemberLvSize(nMemberLv) + 1) > pGuild->GetMaxMemberLvSize(nMemberLv))
	{
		SendDefinedText(TID_GAME_GUILDAPPNUMOVER, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}

	pMember2->m_nMemberLv = nMemberLv;
	pMember2->m_nClass = 0;

	g_dpCoreSrvr.SendGuildMemberLv(idPlayer, nMemberLv, pGuild->m_idGuild);
	g_dpDatabaseClient.SendGuildMemberLv(idPlayer, nMemberLv);

	CGuildMember* pMember;
	CPlayer* pPlayertmp;
	for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i)
	{
		pMember = i->second;
		pPlayertmp = g_PlayerMng.GetPlayer(pMember->m_idPlayer);
		if (pPlayertmp)
			SendGuildMemberLv(idPlayer, nMemberLv, pPlayertmp);
	}
}

void CDPCacheSrvr::OnGuildAuthority(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long _uGuildId;
	DWORD dwAuthority[MAX_GM_LEVEL];

	ar >> _uGuildId;

	ar.Read(dwAuthority, sizeof(dwAuthority));

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (pPlayer == NULL)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(pPlayer->m_idGuild);
	if (pGuild && pGuild->IsMaster(pPlayer->uKey))
	{
		memcpy(pGuild->m_adwPower, dwAuthority, sizeof(dwAuthority));
		g_dpCoreSrvr.SendGuildAuthority(pPlayer->m_idGuild, dwAuthority);

		// GUILD DB AUTHORITY UPDATE
		g_dpDatabaseClient.SendGuildAuthority(pPlayer->m_idGuild, pGuild->m_adwPower);
	}
}

#ifdef __FL_GUILD_FINDER
void CDPCacheSrvr::OnGuildFinderState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long uGuildId;
	ar >> uGuildId;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (pPlayer == NULL)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(pPlayer->m_idGuild);
	if (pGuild && pGuild->IsMaster(pPlayer->uKey))
	{
		pGuild->m_bFinder = !pGuild->m_bFinder;

		g_dpCoreSrvr.SendGuildFinderState(pPlayer->m_idGuild, pGuild->m_bFinder);
		g_dpDatabaseClient.SendGuildFinderState(pPlayer->m_idGuild, pGuild->m_bFinder);
	}
}
#endif

void CDPCacheSrvr::OnGuildSetName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	char szName[MAX_G_NAME];
	ar.ReadString(szName, MAX_G_NAME);

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (pPlayer == NULL)
		return;

	CGuild* pGuild = g_GuildMng.GetGuild(pPlayer->m_idGuild);
	if (pGuild && pGuild->IsMaster(pPlayer->uKey))
	{
		CString str1, str2;
		str1 = pGuild->m_szGuild;
		str2 = "";
		if (str1 == str2)
		{
			if (g_GuildMng.SetName(pGuild, szName))
			{
				g_dpCoreSrvr.SendGuildSetName(pPlayer->m_idGuild, pGuild->m_szGuild);
				g_dpDatabaseClient.SendGuildSetName(pPlayer->m_idGuild, pGuild->m_szGuild);
				SendGuildSetName(pPlayer->m_idGuild, pGuild->m_szGuild);
			}
			else
			{
				// duplicated
				SendGuildError(pPlayer, 1);
			}
		}
	}
	else
	{
		// is not master
		SendDefinedText(TID_GAME_COMDELNOTKINGPIN, pPlayer->dpidCache, pPlayer->dpidUser, "");
	}
}

void CDPCacheSrvr::OnGuildClass(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	BYTE nFlag;
	u_long _idMaster, idPlayer;
	ar >> nFlag >> _idMaster >> idPlayer;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer* pPlayer = g_PlayerMng.GetPlayer(idPlayer);

	if (!pMaster)
		return;

	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CGuild* pGuild = g_GuildMng.GetGuild(pMaster->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pMaster->uKey))
	{
		// guild not found
		// OR is not member
		SendDefinedText(TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "");
		pMaster->m_idGuild = 0;
		return;
	}

	if (FALSE == pGuild->IsMember(idPlayer))
	{
		// is not member
		SendDefinedText(TID_GAME_GUILDNOTMEMBER, pMaster->dpidCache, pMaster->dpidUser, "%s", pGuild->m_szGuild);
		return;
	}

	CGuildMember* pMember1 = pGuild->GetMember(pMaster->uKey);
	CGuildMember* pMember2 = pGuild->GetMember(idPlayer);

	if (!pGuild->IsCmdCap(pMember1->m_nMemberLv, PF_LEVEL))
	{
		SendDefinedText(TID_GAME_GUILDAPPNOTWARRANT, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}
	int nClass = pMember2->m_nClass;
	if (nFlag == 1) // UP
	{
		++nClass;
	}
	else	// DOWN
	{
		--nClass;
	}

	if (nClass < 0 || nClass > 2)
	{
		return;
	}

	pMember2->m_nClass = nClass;

	g_dpCoreSrvr.SendGuildClass(idPlayer, nClass, pGuild->m_idGuild);
	g_dpDatabaseClient.SendGuildClass(idPlayer, nClass);

	CGuildMember* pMember;
	CPlayer* pPlayertmp;
	for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i)
	{
		pMember = i->second;
		pPlayertmp = g_PlayerMng.GetPlayer(pMember->m_idPlayer);
		if (pPlayertmp)
			SendGuildClass(idPlayer, nClass, pPlayertmp);
	}
}

void CDPCacheSrvr::OnChgMaster(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long _idPlayer, idPlayer2;
	ar >> _idPlayer >> idPlayer2;

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer* pPlayer2 = g_PlayerMng.GetPlayer(idPlayer2);
	if (!pPlayer || !pPlayer2)
		return;
	if (pPlayer == pPlayer2)
		return;

	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CGuild* pGuild = g_GuildMng.GetGuild(pPlayer->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pPlayer->uKey) || FALSE == pGuild->IsMember(pPlayer2->uKey))
	{
		// guild not found
		// OR is not member
		SendDefinedText(TID_GAME_COMLEAVENOCOM, pPlayer->dpidCache, pPlayer->dpidUser, "");
		pPlayer->m_idGuild = 0;
		return;
	}

	if (pGuild->IsMaster(pPlayer->uKey))
	{
		pGuild->m_idMaster = idPlayer2;	// set master to guild

		g_dpCoreSrvr.SendChgMaster(pPlayer->uKey, idPlayer2, pGuild->m_idGuild);
		g_dpDatabaseClient.SendChgMaster(pPlayer->uKey, idPlayer2);

		CGuildMember* pMember;
		pMember = pGuild->GetMember(pPlayer->uKey);
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

#ifndef __FL_GUILD_FINDER // Bugfix: Master should be changed for ALL players, not only for guildmembers
		CPlayer* pPlayertmp;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pPlayertmp = g_PlayerMng.GetPlayer(pMember->m_idPlayer);
			if (pPlayertmp)
				SendChgMaster(pPlayer->uKey, idPlayer2, pPlayertmp);
		}
#else
		SendChgMaster(pGuild->m_idGuild, pPlayer->uKey, idPlayer2);
#endif
	}
}

void CDPCacheSrvr::OnGuildNickName(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long idPlayer;
	char strNickName[MAX_GM_ALIAS] = { 0, };
	ar >> idPlayer;
	ar.ReadString(strNickName, MAX_GM_ALIAS);

	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	CPlayer* pMaster = g_PlayerMng.GetPlayerBySerial(dpidUser);
	CPlayer* pPlayer = g_PlayerMng.GetPlayer(idPlayer);

	if (!pMaster)
		return;

	CMclAutoLock	Lock2(g_GuildMng.m_AddRemoveLock);

	CGuild* pGuild = g_GuildMng.GetGuild(pMaster->m_idGuild);
	if (!pGuild || FALSE == pGuild->IsMember(pMaster->uKey))
	{
		SendDefinedText(TID_GAME_COMLEAVENOCOM, pMaster->dpidCache, pMaster->dpidUser, "");
		pMaster->m_idGuild = 0;
		return;
	}

	if (pGuild->m_nLevel < 10)
	{
		SendDefinedText(TID_GAME_GUILDNOTLEVEL, pMaster->dpidCache, pMaster->dpidUser, "");
		return;
	}

	if (pGuild->IsMaster(pMaster->uKey))
	{
		int nLen = lstrlen(strNickName);

		if (nLen < 4 || nLen > 16)
		{
			SendDefinedText(TID_DIAG_0011_01, pMaster->dpidCache, pMaster->dpidUser, "");
			return;
		}

		CGuildMember* pMember2 = pGuild->GetMember(idPlayer);
		if (!pMember2)
			return;

		strcpy(pMember2->m_szAlias, strNickName);

		g_dpCoreSrvr.SendGuildNickName(idPlayer, pMember2->m_szAlias, pGuild->m_idGuild);
		g_dpDatabaseClient.SendGuildNickName(idPlayer, pMember2->m_szAlias);

		CGuildMember* pMember;
		CPlayer* pPlayertmp;
		for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i)
		{
			pMember = i->second;
			pPlayertmp = g_PlayerMng.GetPlayer(pMember->m_idPlayer);
			if (pPlayertmp)
				SendGuildNickName(idPlayer, pMember2->m_szAlias, pPlayertmp);
		}
	}
}

void CDPCacheSrvr::SendGuild(CGuild* pGuild, CPlayer* pPlayer)
{
	if (!pPlayer)
		return;
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD, pPlayer->dpidUser);
	ar << pGuild->m_idGuild;
	pGuild->Serialize(ar, FALSE);
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendAddGuildMember(const GUILD_MEMBER_INFO& info, const char* lpszPlayer, u_long idGuild, CPlayer* pPlayer)
{
	if (!pPlayer)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_ADD_GUILD_MEMBER, pPlayer->dpidUser);
	ar.Write(&info, sizeof(GUILD_MEMBER_INFO));
	ar << idGuild;
	ar.WriteString(lpszPlayer);
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendRemoveGuildMember(u_long idPlayer, u_long idGuild, CPlayer* pPlayer)
{
	if (!pPlayer)
		return;

	BEFORESENDSOLE(ar, PACKETTYPE_REMOVE_GUILD_MEMBER, pPlayer->dpidUser);
	ar << idPlayer << idGuild;
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendGuildMemberLv(u_long idPlayer, int nMemberLv, CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_MEMBER_LEVEL, pPlayer->dpidUser);
	ar << idPlayer << nMemberLv;
	SEND(ar, this, pPlayer->dpidCache);
}


void CDPCacheSrvr::SendGuildClass(u_long idPlayer, int nClass, CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_CLASS, pPlayer->dpidUser);
	ar << idPlayer << nClass;
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendGuildNickName(u_long idPlayer, LPCTSTR strNickName, CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_NICKNAME, pPlayer->dpidUser);
	ar << idPlayer;
	ar.WriteString(strNickName);
	SEND(ar, this, pPlayer->dpidCache);
}

#ifndef __FL_GUILD_FINDER // Bugfix: Master should be changed for ALL players, not only for guildmembers
void CDPCacheSrvr::SendChgMaster(u_long idPlayer, u_long idPlayer2, CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHG_MASTER, pPlayer->dpidUser);
	ar << idPlayer << idPlayer2;
	SEND(ar, this, pPlayer->dpidCache);
}
#else
void CDPCacheSrvr::SendChgMaster(u_long idGuild, u_long idPlayer, u_long idPlayer2)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CHG_MASTER, DPID_ALLPLAYERS);
	ar << idGuild << idPlayer << idPlayer2;
	SEND(ar, this, DPID_ALLPLAYERS);
}
#endif

void CDPCacheSrvr::SendGuildSetName(u_long idGuild, const char* szName)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_SETNAME, DPID_ALLPLAYERS);
	ar << idGuild;
	ar.WriteString(szName);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCacheSrvr::SendGuildMsgControl(GUILD_MSG_HEADER* pHeader, DWORD pPenya, BYTE cbCloak)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_MSG_CONTROL, DPID_ALLPLAYERS);

	ar.Write(pHeader, sizeof(GUILD_MSG_HEADER));
	ar << pPenya;
	ar << cbCloak;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCacheSrvr::SendGuildChat(const char* lpszPlayer, const char* sChat, CPlayer* pPlayer, OBJID objid)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_CHAT, pPlayer->dpidUser);
	ar << objid;
	ar.WriteString(lpszPlayer);
	ar.WriteString(sChat);
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendGuildMemberLogin(CPlayer* pTo, BYTE nLogin, u_long uPlayerId, u_long uMultiNo)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_GAMELOGIN, pTo->dpidUser);
	ar << nLogin << uPlayerId << uMultiNo;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendGuildMemberGameJoin(CPlayer* pTo, int nMaxLogin, u_long uLoginPlayerId[], u_long uLoginGuildMulti[])
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_GAMEJOIN, pTo->dpidUser);
	ar << nMaxLogin;
	ar.Write(uLoginPlayerId, sizeof(u_long) * nMaxLogin);
	ar.Write(uLoginGuildMulti, sizeof(u_long) * nMaxLogin);
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendGuildError(CPlayer* pTo, int nError)
{
	BEFORESENDSOLE(ar, PACKETTYPE_GUILD_ERROR, pTo->dpidUser);
	ar << nError;
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::OnGetFriendState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	CPlayer* pPlayer;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (pPlayer)
		SendFriendState(pPlayer);
}

void CDPCacheSrvr::OnSetFrinedState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	CPlayer* pPlayer;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (pPlayer)
	{
		int state;
		ar >> state;

		if (state < FRS_ONLINE || state >= MAX_FRIENDSTAT)
			return;

		pPlayer->m_RTMessenger.SetState(state);
		SendSetFriendState(pPlayer);
		g_dpCoreSrvr.SendSetFriendState(pPlayer->uKey, pPlayer->m_RTMessenger.GetState());
	}
}

void CDPCacheSrvr::OnFriendInterceptState(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long uidFriend;
	ar >> uidFriend;

	CPlayer* pPlayer, * pFriendUser;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	pFriendUser = g_PlayerMng.GetPlayer(uidFriend);

	if (!pPlayer)
		return;
	pPlayer->Lock();
	Friend* pFriend = pPlayer->m_RTMessenger.GetFriend(uidFriend);
	if (pFriend)
	{
		if (pFriend->bBlock)	// release block
		{
			pFriend->bBlock = FALSE;
			pFriend->dwState = pFriendUser ? pFriendUser->m_RTMessenger.GetState() : FRS_OFFLINE;
			if (pFriendUser)
				SendFriendNoIntercept(pPlayer, pFriendUser, pFriend->dwState);
			else
				SendFriendNoIntercept(pPlayer, uidFriend, pFriend->dwState);
			g_dpDatabaseClient.QueryUpdateMessenger(pPlayer->uKey, uidFriend, FALSE);
		}
		else	// set block
		{
			pFriend->bBlock = TRUE;
			if (pFriendUser)
				SendFriendIntercept(pPlayer, pFriendUser);
			else
				SendFriendIntercept(pPlayer, uidFriend);
			g_dpDatabaseClient.QueryUpdateMessenger(pPlayer->uKey, uidFriend, TRUE);
		}
	}
	pPlayer->Unlock();
	g_dpCoreSrvr.SendFriendInterceptState(pPlayer->uKey, uidFriend);
}



void CDPCacheSrvr::OnRemoveFriend(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long uidFriend;
	ar >> uidFriend;

	CPlayer* pPlayer, * pFriendUser;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (pPlayer)
	{
		pPlayer->Lock();
		pPlayer->m_RTMessenger.RemoveFriend(uidFriend);
		pPlayer->Unlock();
		g_dpCoreSrvr.SendRemoveFriend(pPlayer->uKey, uidFriend);
		g_dpDatabaseClient.QueryDeleteMessenger(pPlayer->uKey, uidFriend);

		pFriendUser = g_PlayerMng.GetPlayer(uidFriend);
		if (!pFriendUser)
			return;
		pFriendUser->Lock();
		pFriendUser->m_RTMessenger.RemoveFriend(pPlayer->uKey);
		pFriendUser->Unlock();
		BEFORESENDSOLE(ar, PACKETTYPE_REMOVEFRIENDSTATE, pFriendUser->dpidUser);
		ar << pPlayer->uKey;
		SEND(ar, this, pFriendUser->dpidCache);
	}
}

void CDPCacheSrvr::SendErrorParty(DWORD dwError, CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ERRORPARTY, pPlayer->dpidUser);
	ar << dwError;
	SEND(ar, this, pPlayer->dpidCache);
}

void CDPCacheSrvr::SendAddFriendNotFound(char* szMemberName, CPlayer* pLeader)
{
	BEFORESENDSOLE(ar, PACKETTYPE_ADDFRIENDNAMENOTFOUND, pLeader->dpidUser);
	ar.WriteString(szMemberName);
	SEND(ar, this, pLeader->dpidCache);
}

void CDPCacheSrvr::SendBlock(BYTE nGu, char* szName, CPlayer* pTo)
{
	BEFORESENDSOLE(ar, PACKETTYPE_BLOCK, pTo->dpidUser);
	ar << nGu;
	ar.WriteString(szName);
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendSetPlayerName(u_long idPlayer, const char* lpszPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_ALLPLAYERS);
	ar << idPlayer;
	ar.WriteString(lpszPlayer);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPCacheSrvr::SendSnoop(const char* lpszString, CPlayer* pSnoop)
{
	BEFORESENDSOLE(ar, PACKETTYPE_SNOOP, pSnoop->dpidUser);
	ar.WriteString(lpszString);
	SEND(ar, this, pSnoop->dpidCache);
}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPCacheSrvr::SendUpdatePlayerData(u_long idPlayer, PlayerData* pPlayerData, CPlayer* pTo)
{
	BEFORESENDSOLE(ar, PACKETTYPE_UPDATE_PLAYER_DATA, pTo->dpidUser);
	ar << idPlayer;
	ar.Write(&pPlayerData->data, sizeof(sPlayerData));
	SEND(ar, this, pTo->dpidCache);
}

void CDPCacheSrvr::SendLogout(u_long idPlayer, CPlayer* pTo)
{
	BEFORESENDSOLE(ar, PACKETTYPE_LOGOUT, pTo->dpidUser);
	ar << idPlayer;
	SEND(ar, this, pTo->dpidCache);
}
#endif	// __SYS_PLAYER_DATA

#ifdef __FL_PARTY_SORT
void CDPCacheSrvr::OnSortPartyMembers(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	u_long idChangeMember;
	BOOL bUpDown;
	ar >> idChangeMember >> bUpDown;

	CPlayer* pLeader;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);
	pLeader = g_PlayerMng.GetPlayerBySerial(dpidUser);

	if (!pLeader)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pLeader->m_uPartyId);
	if (!pParty || !pParty->IsMember(pLeader->uKey))
	{
		pLeader->m_uPartyId = 0;
		return;
	}
	if (pLeader->uKey != idChangeMember)
	{
		if (!pParty->IsLeader(pLeader->uKey))
			return;

		if (!pParty->IsMember(idChangeMember))
		{
			SendErrorParty(ERROR_NOTMAMBER, pLeader);
			return;
		}
	}
	else
	{
		SendErrorParty(ERROR_SAMLEADER, pLeader);
		return;
	}

	int nFind = pParty->FindMember(idChangeMember);
	if (nFind <= 0 || pParty->m_aMember[nFind].m_bRemove) // If it's not member OR leader OR m_bRemove
		return;

	if (pParty->ChangePosition(idChangeMember, bUpDown))
		g_dpCoreSrvr.SendChangePosition(pParty->m_uPartyId, idChangeMember, bUpDown);
}
#endif

#ifdef __VALID_CONNECTION_CONFORMATION
void CDPCacheSrvr::SendConfirmPlayer(DPID dpidCache, DWORD serial, bool state)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CONNECTION_CONFORMATION, DPID_UNKNOWN);
	ar << dpidCache << serial << state;
	SEND(ar, this, DPID_ALLPLAYERS);
}
void CDPCacheSrvr::OnConfirmPlayer(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	DPID socket;
	ar >> socket;
	CMclAutoLock Lock(g_PlayerMng.m_AddRemoveLock);
	CPlayer* pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (pPlayer)
	{
		g_dpCoreSrvr.SendConfirmPlayer(pPlayer);
	}
	else
	{
		pPlayer = g_PlayerMng.GetPlayerBySocket(socket);
		if (pPlayer)
		{
			g_dpCoreSrvr.SendConfirmPlayer(pPlayer);
		}
		else
		{
			DestroyPlayer(socket);
		}
	}
}
#endif

#ifdef __V21_CLOSE_EXISTING_CONNECTION
void	CDPCacheSrvr::SendCloseExistingConnection(const char* lpszAccount, LONG lError)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION, DPID_ALLPLAYERS);
	ar.WriteString(lpszAccount);
	ar << lError;
	SEND(ar, this, DPID_ALLPLAYERS);
}
#endif

#ifdef __FL_PARTY_FINDER
void CDPCacheSrvr::OnPartyChangeJoinMode(CAr& ar, DPID dpidCache, DPID dpidUser, u_long uBufSize)
{
	CPlayer* pPlayer;
	CMclAutoLock	Lock(g_PlayerMng.m_AddRemoveLock);

	pPlayer = g_PlayerMng.GetPlayerBySerial(dpidUser);
	if (!pPlayer)
		return;

	CParty* pParty;
	CMclAutoLock	Lock2(g_PartyMng.m_AddRemoveLock);

	pParty = g_PartyMng.GetParty(pPlayer->m_uPartyId);
	if (NULL == pParty)
	{
		SendErrorParty(ERROR_NOPARTY, pPlayer);
		return;
	}

	if (pParty->IsLeader(pPlayer->uKey))
	{
		pParty->m_bAllowed = !pParty->m_bAllowed;
		g_dpCoreSrvr.SendPartyChangeJoinMode(pPlayer->m_uPartyId, pParty->m_bAllowed);
	}
}
#endif

CDPCacheSrvr	g_DPCacheSrvr;