#include "stdafx.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"
#include "account.h"
#include "misc.h"

#include "dbmanager.h"

extern	CDbManager	g_DbManager;
extern	CDPSrvr		g_dpSrvr;
extern	CAccountMng	g_AccountMng;


CDPDBSrvr::CDPDBSrvr()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_MYREG, &CDPDBSrvr::OnAddConnection);
	ON_MSG(PACKETTYPE_REMOVE_ACCOUNT, &CDPDBSrvr::OnRemoveAccount);
	ON_MSG(PACKETTYPE_GETPLAYERLIST, &CDPDBSrvr::OnGetPlayerList);
	ON_MSG(PACKETTYPE_CREATE_PLAYER, &CDPDBSrvr::OnCreatePlayer);
	ON_MSG(PACKETTYPE_DEL_PLAYER, &CDPDBSrvr::OnRemovePlayer);
	ON_MSG(PACKETTYPE_JOIN, &CDPDBSrvr::OnJoin);
	ON_MSG(PACKETTYPE_REMOVE_ALLACCOUNTS, &CDPDBSrvr::OnRemoveAllAccounts);
}

CDPDBSrvr::~CDPDBSrvr()
{

}

void CDPDBSrvr::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_DESTROYPLAYERORGROUP:
	{
		LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
		OnRemoveConnection(lpDestroyPlayer->dpId);
		break;
	}
	}
}

void CDPDBSrvr::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId)
{
	CAr ar((LPBYTE)lpMsg, dwMsgSize);
	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters)
		= GetHandler(dw);
	ASSERT(pfn);
	(this->*(pfn))(ar, dpId, (LPBYTE)lpMsg, (u_long)dwMsgSize);
}

void CDPDBSrvr::OnAddConnection(CAr& ar, DPID dpid, LPBYTE, u_long)
{
	DWORD dwIdofServer;
	ar >> dwIdofServer;
	g_AccountMng.AddIdofServer(dpid, dwIdofServer);

}

void CDPDBSrvr::OnRemoveConnection(DPID dpid)
{
	g_AccountMng.RemoveIdofServer(dpid);
}

void CDPDBSrvr::OnRemoveAccount(CAr& ar, DPID dpid, LPBYTE, u_long)
{
	TCHAR lpszAccount[MAX_ACCOUNT];
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	g_AccountMng.RemoveAccount(lpszAccount);
}

void CDPDBSrvr::OnCreatePlayer(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	char szAccount[MAX_ACCOUNT] = { 0, }, szPassword[MAX_PASSWORD] = { 0, };

	ar.ReadString(szAccount, MAX_ACCOUNT);
	ar.ReadString(szPassword, MAX_PASSWORD);

	char szPlayer[MAX_PLAYER] = { 0, };
	BYTE nSlot, nFace, nCostume, nSkinSet, nHairMesh;
	DWORD dwHairColor;
	BYTE nSex, nJob, nHeadMesh;
	int nBankPW = 0;

	ar >> nSlot;
	ar.ReadString(szPlayer, MAX_PLAYER);
	ar >> nFace >> nCostume >> nSkinSet >> nHairMesh;
	ar >> dwHairColor;
	ar >> nSex >> nJob >> nHeadMesh;
	ar >> nBankPW;

	DWORD dwAuthKey;
	ar >> dwAuthKey;

	CMclAutoLock	Lock(g_AccountMng.m_AddRemoveLock);
	CAccount* pAccount = g_AccountMng.GetAccount(szAccount);
	if (pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1)
	{
		Send((LPVOID)lpBuf, (DWORD)uBufSize, dpid);
		OutputDebugString("ACCOUNTSERVER.EXE\t// PACKETTYPE_CREATE_PLAYER");
	}
}

#ifdef __REMOVE_PLAYER_0221
void CDPDBSrvr::OnRemovePlayer(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	char szAccount[MAX_ACCOUNT] = { 0, }, szTemp[255] = { 0, };
	ar.ReadString(szAccount, MAX_ACCOUNT);
	ar.ReadString(szTemp, 255);	// szPassword
	ar.ReadString(szTemp, 255);	// szNo
	u_long idPlayer;
	ar >> idPlayer;		// x
	DWORD dwAuthKey;
	ar >> dwAuthKey;		// o

	size_t nSize; u_long uTemp;
	ar >> nSize;
	for (size_t i = 0; i < nSize; i++)
		ar >> uTemp;


	CMclAutoLock	Lock(g_AccountMng.m_AddRemoveLock);
	CAccount* pAccount = g_AccountMng.GetAccount(szAccount);
	if (pAccount && pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_cbRef == 1/*step : select character*/)
	{
		Send((LPVOID)lpBuf, (DWORD)uBufSize, dpid);
		OutputDebugString("ACCOUNTSERVER.EXE\t// PACKETTYPE_DEL_PLAYER");
	}
}
#endif	// __REMOVE_PLAYER_0221

void CDPDBSrvr::OnGetPlayerList(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwAuthKey;
	u_long uIdofMulti;
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	char lpszPassword[MAX_PASSWORD] = { 0, };

	ar >> dwAuthKey;
	ar >> uIdofMulti;
	ar.ReadString(lpszAccount, MAX_ACCOUNT);
	ar.ReadString(lpszPassword, MAX_PASSWORD);

	CMclAutoLock	Lock(g_AccountMng.m_AddRemoveLock);

	CAccount* pAccount = g_AccountMng.GetAccount(lpszAccount);
	if (pAccount == NULL)
	{
		Error("[ Account Data Not Found. szAccount(%s) ]", lpszAccount);
		SendFail(ERROR_FLYFF_ACCOUNT, lpszAccount, dpid);
		return;
	}

	if (pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_bCertify == TRUE)
	{
		Send((LPVOID)lpBuf, (DWORD)uBufSize, dpid);

		if (pAccount->m_cbRef == 0)
		{
			++pAccount->m_cbRef;
			pAccount->m_fRoute = TRUE;
			pAccount->m_dwIdofServer = g_AccountMng.GetIdofServer(dpid);
			pAccount->m_uIdofMulti = uIdofMulti;
			u_long uId = pAccount->m_dwIdofServer * 100 + uIdofMulti;
			map<u_long, LPSERVER_DESC>::iterator i = g_dpSrvr.m_2ServersetPtr.find(uId);
			if (i != g_dpSrvr.m_2ServersetPtr.end())
			{
				g_dpSrvr.DestroyPlayer(pAccount->m_dpid1, pAccount->m_dpid2);
			}
		}
	}
	else
	{
		Error("[ Account Data Invalid. szAccount(%s), ExsitAuthKey(%d), ReceiveAuthKey(%d), m_bCertify(%d) ]", lpszAccount, pAccount->m_dwAuthKey, dwAuthKey, pAccount->m_bCertify);
		SendFail(ERROR_FLYFF_ACCOUNT, lpszAccount, dpid);
	}
}

void CDPDBSrvr::OnJoin(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	DWORD dwAuthKey;
	char lpszAccount[MAX_ACCOUNT] = { 0, };

	ar >> dwAuthKey;	// o
	ar.ReadString(lpszAccount, MAX_ACCOUNT);	// o

	CMclAutoLock	Lock(g_AccountMng.m_AddRemoveLock);
	CAccount* pAccount = g_AccountMng.GetAccount(lpszAccount);
	if (pAccount == NULL)
	{
		Error("[ Get Account Failed. Account(%s) ]", lpszAccount);
		return;
	}

	if (pAccount->m_dwAuthKey == dwAuthKey && pAccount->m_bCertify == TRUE && pAccount->m_cbRef == 1)
	{
		pAccount->m_cbRef++;
		Send((LPVOID)lpBuf, (DWORD)uBufSize, dpid);

		u_long uId = pAccount->m_dwIdofServer * 100 + pAccount->m_uIdofMulti;
		map<u_long, LPSERVER_DESC>::iterator i = g_dpSrvr.m_2ServersetPtr.find(uId);
		if (i != g_dpSrvr.m_2ServersetPtr.end())
		{
			long lCount = InterlockedIncrement(&i->second->lCount);
			g_dpSrvr.SendPlayerCount(uId, lCount);
		}
	}
	else
	{
		Error("[ Account Data Invalid. Account(%s), Account_AuthKey(%d), Receive_AuthKey(%d), Account_bCertify(%d), Account_cbRef(%d) ]"
			, lpszAccount, pAccount->m_dwAuthKey, dwAuthKey, pAccount->m_bCertify, pAccount->m_cbRef);
	}
}

void CDPDBSrvr::SendCloseExistingConnection(const char* lpszAccount, LONG lError)
{
	BEFORESEND(ar, PACKETTYPE_CLOSE_EXISTING_CONNECTION);
	ar.WriteString(lpszAccount);

	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPDBSrvr::SendPlayerCount(void)
{
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	for (int nChannel = 0; nChannel < (int)(m_vecstrChannelAccount.size() + 1); nChannel++)
#endif // __LOG_PLAYERCOUNT_CHANNEL
	{

		DWORD dwParent = NULL_ID;
		int cbSize;
		int anCount[64];
		DPID adpid[64];

		memset((void*)adpid, 0xff, sizeof(DPID) * 64);
		DWORD dwIdofServer;
		g_AccountMng.m_AddRemoveLock.Enter();
		for (map<DWORD, DWORD>::iterator i2 = g_AccountMng.m_2IdofServer.begin(); i2 != g_AccountMng.m_2IdofServer.end(); ++i2)
		{
			dwIdofServer = i2->second;
			if (dwIdofServer >= 0 && dwIdofServer < 64)
				adpid[dwIdofServer] = i2->first;
		}
		g_AccountMng.m_AddRemoveLock.Leave();

		LPSERVER_DESC pServer;
		for (int i = 0; i < (int)(g_dpSrvr.m_dwSizeofServerset); i++)
		{
			pServer = &g_dpSrvr.m_aServerset[i];
			if (pServer->dwParent == NULL_ID)
			{
				//			if( dwParent != NULL_ID )
				if (dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN)
				{
					BEFORESEND(ar, PACKETTYPE_PLAYER_COUNT);
#ifdef __LOG_PLAYERCOUNT_CHANNEL
					ar << nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
					ar << cbSize;
					ar.Write((void*)anCount, sizeof(int) * cbSize);
					SEND(ar, this, adpid[dwParent]);
				}
				dwParent = pServer->dwID;
				cbSize = 0;
			}
			else
			{
#ifdef __LOG_PLAYERCOUNT_CHANNEL
				if (nChannel == 0)
#endif // __LOG_PLAYERCOUNT_CHANNEL
					anCount[cbSize++] = pServer->lCount;
#ifdef __LOG_PLAYERCOUNT_CHANNEL
				else
				{
					CMclAutoLock	Lock(g_AccountMng.m_AddRemoveLock);
					long lCount = 0;
					map<string, CAccount*> mapTemp = g_AccountMng.GetMapAccount();
					for (map<string, CAccount*>::iterator it = mapTemp.begin(); it != mapTemp.end(); it++)
					{
						if (it->first.find(m_vecstrChannelAccount[nChannel - 1]) != -1
							&& it->second->m_dwIdofServer == pServer->dwParent
							&& it->second->m_uIdofMulti == pServer->dwID)
						{
							lCount++;
						}
					}
					anCount[cbSize++] = lCount;
				}
#endif // __LOG_PLAYERCOUNT_CHANNEL
			}
		}
		//	if( dwParent != NULL_ID )
		if (dwParent >= 0 && dwParent < 64 && adpid[dwParent] != DPID_UNKNOWN)
		{
			BEFORESEND(ar, PACKETTYPE_PLAYER_COUNT);
#ifdef __LOG_PLAYERCOUNT_CHANNEL
			ar << nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
			ar << cbSize;
			ar.Write((void*)anCount, sizeof(int) * cbSize);
			SEND(ar, this, adpid[dwParent]);
		}
	}
}

void CDPDBSrvr::OnRemoveAllAccounts(CAr& ar, DPID dpid, LPBYTE, u_long)
{
	g_AccountMng.RemoveIdofServer(dpid, FALSE);
}


void CDPDBSrvr::SendFail(long lError, const char* lpszAccount, DPID dpid)
{
	BEFORESEND(ar, PACKETTYPE_FAIL);
	ar << lError;
	ar.WriteString(lpszAccount);
	SEND(ar, this, dpid);
}




void CDPDBSrvr::SendOneHourNotify(CAccount* pAccount)
{
	BEFORESEND(ar, PACKETTYPE_ONE_HOUR_NOTIFY);
	ar.WriteString(pAccount->m_lpszAccount);

	SEND(ar, this, DPID_ALLPLAYERS);
}


CDPDBSrvr	g_dpDbSrvr;