#include "StdAfx.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "DPCacheSrvr.h"
#include "Player.h"
#ifdef __TRACE1027

#endif	// __TRACE1027

#ifdef __PL_0917
#include "packetlog.h"
#endif	// __PL_0917
#include "mytrace.h"
extern	CMyTrace	g_MyTrace;

extern	CDPClientArray	g_DPClientArray;
extern	CDPCoreClient	g_DPCoreClient;

#include "messenger.h"

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

CDPCacheSrvr::CDPCacheSrvr()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_JOIN, &CDPCacheSrvr::OnAddConnection);
	ON_MSG(PACKETTYPE_KEEP_ALIVE, &CDPCacheSrvr::OnKeepAlive);
}

CDPCacheSrvr::~CDPCacheSrvr()
{

}

void CDPCacheSrvr::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	static	int	cbPlayer = 0;

	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
	{
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->SetLastPacket(this, 0xCCCCCCCC);
#endif	// __CRASH_0404
		LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
		g_MyTrace.Add(CMyTrace::Key("so"), FALSE, "//%04d//", ++cbPlayer);
		CPlayerMng::Instance()->AddPlayer(lpCreatePlayer->dpId);
		break;
	}
	case DPSYS_DESTROYPLAYERORGROUP:
	{
#ifdef __CRASH_0404
		CCrashStatus::GetInstance()->SetLastPacket(this, 0xDDDDDDDD);
#endif	// __CRASH_0404
		g_MyTrace.Add(CMyTrace::Key("so"), FALSE, "//%04d//", --cbPlayer);
		LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
		OnRemoveConnection(lpDestroyPlayer->dpId);
		break;
	}
	}

#ifdef __CRASH_0404
	CCrashStatus::GetInstance()->SetLastPacket(this, 0xFFFFFFFF);
#endif	// __CRASH_0404
}


void CDPCacheSrvr::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
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

	if (dwMsgSize < 8)
		return;

	GETTYPE(ar);

	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer(idFrom);
	if (!pPlayer)
		return;

	if (pfn)
	{
		(this->*(pfn))(ar, idFrom, (BYTE*)lpMsg + sizeof(DPID), dwMsgSize - sizeof(DPID));
	}
	else
	{
#ifdef __VALID_CONNECTION_CONFORMATION
		m_socketTimer[idFrom] = time(nullptr);
#endif
		switch (dw)
		{
		case PACKETTYPE_REMOVEPARTYMEMBER:
		case PACKETTYPE_CHANGETROUP:
		case PACKETTYPE_CHANPARTYNAME:
		case PACKETTYPE_PARTYCHANGEITEMMODE:
		case PACKETTYPE_PARTYCHANGEEXPMODE:
		case PACKETTYPE_GETFRIENDSTATE:
		case PACKETTYPE_SETFRIENDSTATE:
		case PACKETTYPE_FRIENDINTERCEPTSTATE:
		case PACKETTYPE_REMOVEFRIEND:
		case PACKETTYPE_PARTYCHANGELEADER:
		case PACKETTYPE_DESTROY_GUILD:
		case PACKETTYPE_REMOVE_GUILD_MEMBER:
		case PACKETTYPE_GUILD_MEMBER_LEVEL:
		case PACKETTYPE_GUILD_AUTHORITY:
#ifdef __FL_GUILD_FINDER
		case PACKETTYPE_GUILD_FINDERSTATE:
#endif
		case PACKETTYPE_GUILD_SETNAME:
		case PACKETTYPE_GUILD_CLASS:
		case PACKETTYPE_GUILD_NICKNAME:
		case PACKETTYPE_CHG_MASTER:
#ifdef __FL_PARTY_SORT
		case PACKETTYPE_MOVE_PARTYMEMBERS:
#endif
#ifdef __FL_PARTY_FINDER
		case PACKETTYPE_PARTYCHANGEJOINMODE:
#endif
			g_DPCoreClient.SendToServer(idFrom, lpMsg, dwMsgSize);
			break;
		case PACKETTYPE_LEAVE:
		case PACKETTYPE_MODIFYMODE:
			break;
		default:
			g_DPClientArray.SendToServer(idFrom, lpMsg, dwMsgSize);
			break;
		}
	}
}


#ifdef __TEST_PACKET_CRYPT
BOOL CDPCacheSrvr::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
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

void CDPCacheSrvr::OnAddConnection(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	CMclAutoLock	Lock(CPlayerMng::Instance()->m_AddRemoveLock);
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer(dpid);
	if (pPlayer)
	{
		pPlayer->SetAddr(this);
		pPlayer->Join(ar);
		CRTMessenger rtmessenger;
		int nSize = rtmessenger.Serialize(ar);

		if (pPlayer->IsConnected())
		{
			Error("CDPCacheSrvr::OnAddConnection() user already connected. Playerid = %d, Account = %s", pPlayer->GetPlayerId(), pPlayer->GetAccount());
			DestroyPlayer(dpid);
			return;
		}

		pPlayer->SetConnected(true);

		if (nSize)
		{
			Error("CRTMessenger.Serialize: %s(%d)", pPlayer->GetAddr(), nSize);
			DestroyPlayer(dpid);
		}
		else
			g_DPCoreClient.SendAddPlayer(pPlayer, rtmessenger);
	}
	else
		DestroyPlayer(dpid);
}

void CDPCacheSrvr::OnRemoveConnection(DPID dpid)
{
	CMclAutoLock	Lock(CPlayerMng::Instance()->m_AddRemoveLock);

	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer(dpid);
	if (pPlayer)
	{
		if (pPlayer->GetClient())
		{
			BEFORESENDSOLE(ar, PACKETTYPE_LEAVE, DPID_UNKNOWN);
			ar << pPlayer->GetPlayerId();
			LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
			g_DPClientArray.SendToServer(dpid, lpBuf, nBufSize);
		}
		CPlayerMng::Instance()->RemovePlayer(dpid);
	}
	else
	{
		WriteError("CDPCacheSrvr::OnRemoveConnection GetPlayer return NULL.");
	}
}

void CDPCacheSrvr::OnKeepAlive(CAr& ar, DPID dpid, LPBYTE, u_long)
{
	CMclAutoLock	Lock(CPlayerMng::Instance()->m_AddRemoveLock);
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer(dpid);
	if (pPlayer)
		pPlayer->SetAlive(TRUE);
}

CDPCacheSrvr	g_DPCacheSrvr;