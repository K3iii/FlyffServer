#include "StdAfx.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
extern	CDPCoreClient	g_dpCoreClient;
#include "DPLoginSrvr.h"
#include "User.h"

extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CUserMng			g_UserMng;
extern  char				g_szMSG_VER[];

CDPLoginSrvr::CDPLoginSrvr()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_PRE_JOIN, &CDPLoginSrvr::OnPreJoin);
	ON_MSG(PACKETTYPE_PING, &CDPLoginSrvr::OnPing);
	ON_MSG(PACKETTYPE_QUERYTICKCOUNT, &CDPLoginSrvr::OnQueryTickCount);
	ON_MSG(PACKETTYPE_GETPLAYERLIST, &CDPLoginSrvr::OnAddConnection);

	memset(m_aCache, 0, sizeof(CACHESTRUCT) * MAX_CACHE_SIZE);
	m_nSizeOfCache = 0;
}

CDPLoginSrvr::~CDPLoginSrvr()
{

}


void CDPLoginSrvr::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
	{
		LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
		DPID dpid = lpCreatePlayer->dpId;

		CUser* pUser = new CUser(dpid);

		if (g_UserMng.AddUser(dpid, pUser))
		{
		}
		else
		{
			DestroyPlayer(dpid);
			SAFE_DELETE(pUser);
		}
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


void CDPLoginSrvr::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	static size_t	nSize = sizeof(DPID);

#ifdef __TEST_PACKET_CRYPT
	BYTE* pData = (BYTE*)lpMsg;
	for (DWORD i = 0; i < dwMsgSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
#endif

	CAr ar((LPBYTE)lpMsg + nSize, dwMsgSize - nSize);		// Skip dpid

	if (dwMsgSize < 8)		// INVALID PACKET
	{
		WriteError("PACKET//0");
		return;
	}

	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn)
	{
		(this->*(pfn))(ar, idFrom);
	}
	else
	{
#ifdef __AIO // https://github.com/SPSquonK/useleSS/commit/2a00f5df5199da9b77977aa2ad762c35c6608d1a
		if (dw == PACKETTYPE_CREATE_PLAYER || dw == PACKETTYPE_DEL_PLAYER)
			g_dpDBClient.SendToServer(idFrom, lpMsg, dwMsgSize);
#else
		g_dpDBClient.SendToServer(idFrom, lpMsg, dwMsgSize);
#endif
	}
}

#ifdef __TEST_PACKET_CRYPT
BOOL CDPLoginSrvr::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
{
	BYTE* pData = (BYTE*)lpData;
	for (DWORD i = 0; i < dwDataSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
	return CDPMng::Send((LPVOID)pData, dwDataSize, dpidTo);
}
#endif

void CDPLoginSrvr::AddCahce(const char* lpCacheAddr)
{
	strcpy(m_aCache[m_nSizeOfCache++].lpAddr, lpCacheAddr);
}

int CDPLoginSrvr::CacheIn(void)
{
	int nIndexOfCache = 0;
	int nCount = m_aCache[0].uCount;
	for (int i = 1; i < m_nSizeOfCache; i++)
	{
		if ((int)(m_aCache[i].uCount) < nCount) {
			nIndexOfCache = i;
			nCount = m_aCache[i].uCount;
		}
	}
	m_aCache[nIndexOfCache].uCount++;
	return nIndexOfCache;
}

void CDPLoginSrvr::CacheOut(int nIndexOfCache)
{
	m_aCache[nIndexOfCache].uCount--;
}

void CDPLoginSrvr::SendCacheAddr(int nIndexOfCache, DPID dpid)
{
	BEFORESEND(ar, PACKETTYPE_CACHE_ADDR);
	ar.WriteString(m_aCache[nIndexOfCache].lpAddr);
	SEND(ar, this, dpid);
}

void CDPLoginSrvr::SendError(LONG lError, DPID dpid)
{
	BEFORESEND(ar, PACKETTYPE_ERROR);
	ar << lError;
	SEND(ar, this, dpid);
}

void CDPLoginSrvr::SendHdr(DWORD dwHdr, DPID dpid)
{
	BEFORESEND(ar, dwHdr);
	SEND(ar, this, dpid);
}

void CDPLoginSrvr::OnAddConnection(CAr& ar, DPID dpid)
{
	char lpszAccount[MAX_ACCOUNT], lpszPassword[MAX_PASSWORD], lpVer[32];
	DWORD dwAuthKey, dwID;

	ar.ReadString(lpVer, 32);
	ar >> dwAuthKey;
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	ar.ReadString(lpszPassword, MAX_PASSWORD);
	ar >> dwID;

	if (dwAuthKey == 0)
	{
		DestroyPlayer(dpid);
		return;
	}

	if (strcmp(lpVer, g_szMSG_VER) != 0)
	{
		SendError(ERROR_ILLEGAL_VER, dpid);
		return;
	}

	CMclAutoLock Lock(g_UserMng.m_AddRemoveLock);

	CUser* pUser = g_UserMng.GetUser(lpszAccount);
	if (pUser && pUser->m_dpid != dpid)
	{
		Error("CDPLoginSrvr::OnAddConnection dpid Error");
		DestroyPlayer(dpid);
		return;
	}

	pUser = g_UserMng.GetUser(dpid);
	if (pUser == NULL)
	{
		Error("CDPLoginSrvr::OnAddConnection pUser == NULL");
		return;
	}

	if (pUser->GetAuth() == FALSE)
	{
		Error("CDPLoginSrvr::OnAddConnection pUser->GetAuth() == FALSE");
		return;
	}

	pUser->SetExtra(lpszAccount, dwAuthKey);
	if (g_UserMng.AddUser(lpszAccount, pUser))
	{
		pUser->m_nIndexOfCache = g_dpLoginSrvr.CacheIn();
		g_dpLoginSrvr.SendCacheAddr(pUser->m_nIndexOfCache, dpid);
		g_dpDBClient.SendGetPlayerList(dpid, lpszAccount, lpszPassword, dwAuthKey, (u_long)dwID);
	}
	else
	{
		WriteError("OnAddUser(): AddUser() return 0, %s", lpszAccount);
		CUser* pUser = g_UserMng.GetUser(lpszAccount);
		if (pUser)
			DestroyPlayer(pUser->m_dpid);

		DestroyPlayer(dpid);
	}
}

void CDPLoginSrvr::OnRemoveConnection(DPID dpid)
{
	g_UserMng.RemoveUser(dpid);
}

void CDPLoginSrvr::OnPreJoin(CAr& ar, DPID dpid)
{
	char lpszAccount[MAX_ACCOUNT], lpszPlayer[MAX_PLAYER];
	u_long idPlayer;

	CMclAutoLock	Lock(g_UserMng.m_AddRemoveLock);
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	ar >> idPlayer;
	ar.ReadString(lpszPlayer, MAX_PLAYER);

	CUser* pUser = g_UserMng.GetUser(dpid);
	if (!pUser)
	{
		WriteLog("OnPreJoin(): dpid not found, %s", lpszAccount);
		return;
	}

	if (*pUser->m_pKey == '\0')
	{
		WriteLog("OnPreJoin(): trying prejoin with empty m_pKey, account %s", lpszAccount);
		return;
	}

	if (lstrcmp(lpszAccount, pUser->m_pKey) != 0)
	{
		WriteLog("OnPreJoin(): Different account: %s, m_pKey: %s, idPlayer: %d", lpszAccount, pUser->m_pKey, idPlayer);
		return;
	}

	pUser->m_idPlayer = idPlayer;
	g_dpCoreClient.SendPreJoin(pUser->m_dwAuthKey, lpszAccount, idPlayer, lpszPlayer);
}

void CDPLoginSrvr::OnPing(CAr& ar, DPID dpid)
{
	CMclAutoLock	Lock(g_UserMng.m_AddRemoveLock);
	CUser* pUser = g_UserMng.GetUser(dpid);
	if (pUser)
	{
		pUser->m_tPingRecvd = timeGetTime();

		DWORD dwPingTime;
		ar >> dwPingTime;

		BEFORESEND(ar1, PACKETTYPE_PING);
		ar1 << dwPingTime;
		SEND(ar1, this, dpid);
	}
}

void CDPLoginSrvr::OnQueryTickCount(CAr& ar, DPID dpid)
{
	DWORD dwTime;
	ar >> dwTime;

	BEFORESEND(ar1, PACKETTYPE_QUERYTICKCOUNT);
	ar1 << dwTime << g_TickCount.GetTickCount();
	SEND(ar1, this, dpid);
}

CDPLoginSrvr	g_dpLoginSrvr;