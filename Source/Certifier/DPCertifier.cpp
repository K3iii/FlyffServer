#include "stdafx.h"
#include "dpaccountclient.h"
#include "dpcertifier.h"
#include "dbmanager.h"
#include "..\Resource\Lang.h"

#ifdef __TRACE1027

#endif	// __TRACE1027

#include "user.h"

extern	CDPAccountClient	g_dpAccountClient;
extern	CDbManager			g_DbManager;

CLoginLimit::CLoginLimit()
{
	m_nLimitation = INT_MAX;
	m_dwTick = 0;
	m_nRequestPerSecond = 0;
}


bool CLoginLimit::Check()
{
	m_nRequestPerSecond++;
	if (GetTickCount() > m_dwTick)
	{
		m_dwTick = GetTickCount() + 1000;
		m_nRequestPerSecond = 1;
	}

	return (m_nRequestPerSecond > m_nLimitation);
}

CDPCertifier::CDPCertifier()
{
	m_dwSizeofServerset = 0;
	*m_szVer = '\0';
#ifdef __SECURITY_0628
	* m_szResVer = '\0';
#endif	// __SECURITY_0628

	BEGIN_MSG;
	ON_MSG(PACKETTYPE_CERTIFY, &CDPCertifier::OnCertify);
	ON_MSG(PACKETTYPE_PING, &CDPCertifier::OnPing);
	ON_MSG(PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPCertifier::OnCloseExistingConnection);
	ON_MSG(PACKETTYPE_KEEP_ALIVE, &CDPCertifier::OnKeepAlive);
	ON_MSG(PACKETTYPE_ERROR, &CDPCertifier::OnError);
}

CDPCertifier::~CDPCertifier()
{

}

void CDPCertifier::SetLimitation(int nLimit)
{
	//	if( ::GetLanguage() == LANG_JAP )
	//	{
	//		if( nLimit > 0 )
	//			m_limit.SetLimitation( nLimit );
	//	}
}

void CDPCertifier::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid)
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

void CDPCertifier::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid)
{
#ifdef __TEST_PACKET_CRYPT
	BYTE* pData = (BYTE*)lpMsg;
	for (DWORD i = 0; i < dwMsgSize; i++)
	{
		pData[i] = pData[i] ^ ((BYTE)(i % sizeof(BYTE)) ^ 171);
		pData[i] = pData[i] ^ ((sizeof(BYTE) - (BYTE)(i % sizeof(BYTE))) ^ 84);
	}
#endif

	CAr ar((LPBYTE)lpMsg, dwMsgSize);

	if (dwMsgSize < 4)
	{
		WriteError("PACKET//0");
		return;
	}

	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters)
		= GetHandler(dw);

	//	ASSERT( pfn );
	if (pfn)
		(this->*(pfn))(ar, dpid, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD));
}

#ifdef __TEST_PACKET_CRYPT
BOOL CDPCertifier::Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo)
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

void CDPCertifier::OnAddConnection(DPID dpid)
{
#ifdef __US_LOGIN_0223
	if (!CUserMng::GetInstance()->AddUser(dpid))
		DestroyPlayer(dpid);
#else	// __US_LOGIN_0223
	CUserMng::GetInstance()->AddUser(dpid);
#endif	// __US_LOGIN_0223
}

void CDPCertifier::SendServerList(DPID dpId, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szBak)
{
	BEFORESEND(ar, PACKETTYPE_SRVR_LIST);
	ar << dwAuthKey;
	ar << cbAccountFlag;
	ar.WriteString(szBak);

	ar << m_dwSizeofServerset;
	for (DWORD i = 0; i < m_dwSizeofServerset; i++)
	{
		LPSERVER_DESC pServer = m_aServerset + i;

		ar << pServer->dwParent;
		ar << pServer->dwID;
		ar.WriteString(pServer->lpName);
		ar.WriteString(pServer->lpAddr);
		ar << pServer->b18;
		ar << pServer->lCount;
		ar << pServer->lEnable;
		ar << pServer->lMax;
	}
	SEND(ar, this, dpId);
}

void CDPCertifier::SendError(LONG lError, DPID dpId)
{
	BEFORESEND(ar, PACKETTYPE_ERROR);
	ar << lError;
	SEND(ar, this, dpId);

}

// Handlers
/*________________________________________________________________________________*/
void CDPCertifier::OnRemoveConnection(DPID dpid)
{
	CUserMng::GetInstance()->RemoveUser(dpid);
	g_dpAccountClient.SendRemoveAccount(dpid);
}

void CDPCertifier::OnError(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	DestroyPlayer(dpid);
}

void CDPCertifier::OnCertify(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	char pszVer[32] = { 0, };
	ar.ReadString(pszVer, 32);

	if (strcmp(m_szVer, pszVer))
	{
		SendError(ERROR_ILLEGAL_VER, dpid);
		return;
	}

	char pszAccount[MAX_ACCOUNT] = { 0, }, pszPwd[MAX_PASSWORD] = { 0, };
	ar.ReadString(pszAccount, MAX_ACCOUNT);

	////////////////////////////////////////////////////////////////////

#ifdef __ENCRYPT_PASSWORD
	char		szEnc[16 * MAX_PASSWORD] = { 0, };
	char		szDec[16 * MAX_PASSWORD] = { 0, };

	ar.Read(szEnc, 16 * MAX_PASSWORD);

	g_xRijndael->ResetChain();
	g_xRijndael->Decrypt(szEnc, szDec, 16 * MAX_PASSWORD, CRijndael::CBC);

	::memcpy(pszPwd, szDec, MAX_PASSWORD);
#else
	ar.ReadString(pszPwd, MAX_PASSWORD);
#endif

	////////////////////////////////////////////////////////////////////


	if (pszAccount[0] == '\0' || StringFind(pszAccount, '\'') >= 0 || StringFind(pszPwd, '\'') >= 0)
	{
		DestroyPlayer(dpid);
		return;
	}

#ifdef __FIX_SQL_INJECTS // test thingy
	CString strAccount, strPassword;
	strAccount.Format("%s", pszAccount);
	strPassword.Format("%s", pszPwd);

	if (strAccount.Find("--") >= 0 || strAccount.Find("'") >= 0 || strAccount.Find("/*") >= 0 || strAccount.Find("*/") >= 0 || strAccount.Find(";") >= 0
		|| strPassword.Find("--") >= 0 || strPassword.Find("'") >= 0 || strPassword.Find("/*") >= 0 || strPassword.Find("*/") >= 0 || strPassword.Find(";") >= 0)
	{
		Error("CDPCertifier::OnCertify() Account: %s Password: %s", strAccount, strPassword);
		DestroyPlayer(dpid);
		return;
	}
#endif

	CMclAutoLock Lock(CUserMng::GetInstance()->m_AddRemoveLock);
	CUser* pUser = CUserMng::GetInstance()->GetUser(dpid);
	if (pUser)
		pUser->SetAccount(pszAccount);
	else
		return;

	LPDB_OVERLAPPED_PLUS pData = g_DbManager.m_pDbIOData->Alloc();
	memset(&pData->AccountInfo, 0, sizeof(ACCOUNT_INFO));
	strcpy(pData->AccountInfo.szAccount, pszAccount);
	strcpy(pData->AccountInfo.szPassword, pszPwd);
	_tcslwr(pData->AccountInfo.szAccount);

	pData->dwIP = GetPlayerAddr(dpid);
	pData->dpId = dpid;
	pData->nQueryMode = CERTIFY;
	g_DbManager.PostQ(pData);
}

void CDPCertifier::OnPing(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	g_dpAccountClient.SendPing(dpid);
}

void CDPCertifier::OnCloseExistingConnection(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	char pszAccount[MAX_ACCOUNT], pszPwd[MAX_PASSWORD];
	ar.ReadString(pszAccount, MAX_ACCOUNT);
	ar.ReadString(pszPwd, MAX_PASSWORD);

	if (pszAccount[0] == '\0')
		return;

#ifdef __FIX_SQL_INJECTS // test thingy
	CString strAccount, strPassword;
	strAccount.Format("%s", pszAccount);
	strPassword.Format("%s", pszPwd);

	if (strAccount.Find("--") >= 0 || strAccount.Find("'") >= 0 || strAccount.Find("/*") >= 0 || strAccount.Find("*/") >= 0 || strAccount.Find(";") >= 0
		|| strPassword.Find("--") >= 0 || strPassword.Find("'") >= 0 || strPassword.Find("/*") >= 0 || strPassword.Find("*/") >= 0 || strPassword.Find(";") >= 0)
	{
		Error("CDPCertifier::OnCloseExistingConnection() Account: %s Password: %s", strAccount, strPassword);
		return;
	}
#endif

	LPDB_OVERLAPPED_PLUS pData = g_DbManager.m_pDbIOData->Alloc();
	strcpy(pData->AccountInfo.szAccount, pszAccount);
	strcpy(pData->AccountInfo.szPassword, pszPwd);
	_tcslwr(pData->AccountInfo.szAccount);
	pData->dwIP = 0;		// don't use
	pData->dpId = dpid;
	pData->nQueryMode = CLOSE_EXISTING_CONNECTION;
	g_DbManager.PostQ(pData);
}

void CDPCertifier::OnKeepAlive(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize)
{
	CUserMng* pMng = CUserMng::GetInstance();
	CMclAutoLock	Lock(pMng->m_AddRemoveLock);

	CUser* pUser = pMng->GetUser(dpid);
	if (pUser)
		pUser->m_bValid = TRUE;
}
/*________________________________________________________________________________*/
CDPCertifier	g_dpCertifier;