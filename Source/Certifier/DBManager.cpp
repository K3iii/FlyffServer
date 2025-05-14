#include "stdafx.h"
#include "dbmanager.h"

#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "AccountMgr.h"
#include "..\Resource\Lang.h"

///////////////////////////////////////////////////////////////////////////////////////////
// extern
extern	CDPCertifier		g_dpCertifier;
extern	CDPAccountClient	g_dpAccountClient;
extern	UINT				HashKey(const char* key);

///////////////////////////////////////////////////////////////////////////////////////////
// global
CDbManager	g_DbManager;
HANDLE		s_hHandle = (HANDLE)NULL;
///////////////////////////////////////////////////////////////////////////////////////////
CDbManager::CDbManager()
{
	m_pDbIOData = new CMemPool<DB_OVERLAPPED_PLUS>(512);
	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++)
	{
		m_hIOCP[i] = NULL;
		m_hDbWorkerThreadTerminate[i] = NULL;
	}
	m_szLoginPWD[0] = '\0';
}

CDbManager::~CDbManager()
{
	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++)
	{
		PostQueuedCompletionStatus(m_hIOCP[i], 0, NULL, NULL);
		CLOSE_HANDLE(m_hIOCP[i]);
	}

	WaitForMultipleObjects(DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThreadTerminate, TRUE, INFINITE);

	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++) {
		CLOSE_HANDLE(m_hDbWorkerThreadTerminate[i]);
	}

	SAFE_DELETE(m_pDbIOData);
}

BOOL CDbManager::CreateDbWorkers()
{
	s_hHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++)
	{
		m_hIOCP[i] = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		ASSERT(m_hIOCP[i]);
		HANDLE hThread = chBEGINTHREADEX(NULL, 0, DbWorkerThread, (LPVOID)i, 0, NULL);
		ASSERT(hThread);
		m_hDbWorkerThreadTerminate[i] = hThread;
		if (WaitForSingleObject(s_hHandle, SEC(10)) == WAIT_TIMEOUT)
		{
			OutputDebugString("CERTIFIER.EXE\t// TIMEOUT\t// ODBC");
			return FALSE;
		}
	}

	CloseHandle(s_hHandle);
	return TRUE;
}

void CDbManager::DBQryAccount(char* qryAccount, LPDB_OVERLAPPED_PLUS pData)
{
	char lpAddr[16] = { 0, };
	g_dpCertifier.GetPlayerAddr(pData->dpId, lpAddr);

	wsprintf(qryAccount, "LOGIN_STR '%s', '%s', '%s'", pData->AccountInfo.szAccount, pData->AccountInfo.szPassword, lpAddr);
}

void CDbManager::OnCertifyQueryOK(CQuery& query, LPDB_OVERLAPPED_PLUS pData, const char* szCheck)
{
	char lpAddr[16] = { 0, };
	g_dpCertifier.GetPlayerAddr(pData->dpId, lpAddr);
	g_dpAccountClient.SendAddAccount(lpAddr, pData->AccountInfo.szAccount, 0x00, pData->dpId, 0, pData->AccountInfo.szBak);
}

void CDbManager::Certify(CQuery& query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr)
{
	ACCOUNT_CHECK result = CHECK_OK;
	if (pData->dwIP)
	{
		result = accountMgr.Check(pData->dwIP);
		switch (result)
		{
		case CHECK_3TIMES_ERROR:
			g_dpCertifier.SendError(ERROR_15MIN_PREVENT, pData->dpId);
			return;
		}
	}

	query.Clear();

	char	szQuery[256];

	DBQryAccount(szQuery, pData);

	int nCode = ERROR_CERT_GENERAL;

	if (query.Exec(szQuery))
	{
		if (query.Fetch())
		{
			int nError = query.GetInt("fError");
			switch (nError)
			{
			case 0:
				if (pData->dwIP)
					accountMgr.SetError(0);

				lstrcpy(pData->AccountInfo.szBak, pData->AccountInfo.szAccount);
				OnCertifyQueryOK(query, pData);
				return;
			case 1:
				if (pData->dwIP)
					accountMgr.SetError(1);
				nCode = ERROR_FLYFF_PASSWORD;
				break;
			case 3:
				nCode = ERROR_BLOCKGOLD_ACCOUNT;
				break;
			case 4:
				nCode = ERROR_FLYFF_AUTH;
				break;
			case 5:
				nCode = ERROR_FLYFF_PERMIT;
				break;
			case 6:
				nCode = ERROR_FLYFF_NEED_AGREEMENT;
				break;
			case 7:
				nCode = ERROR_FLYFF_NO_MEMBERSHIP;
				break;
			case 9:
				nCode = ERROR_FLYFF_DB_JOB_ING;
				break;
			case 91:
				nCode = ERROR_FLYFF_EXPIRED_SESSION_PASSWORD;
				break;
			default:
				nCode = ERROR_FLYFF_ACCOUNT;
				break;
			}
		}
	}

	g_dpCertifier.SendError(nCode, pData->dpId);
}

void CDbManager::CloseExistingConnection(CQuery& query, LPDB_OVERLAPPED_PLUS pData)
{
	query.Clear();

	char	szQuery[128];

	DBQryAccount(szQuery, pData);

	if (query.Exec(szQuery))
	{
		if (query.Fetch())
		{
			if (0 == query.GetInt("fError"))
				g_dpAccountClient.SendCloseExistingConnection(pData->AccountInfo.szAccount);
		}
	}
}

u_int __stdcall DbWorkerThread(LPVOID nIndex)
{
	CAccountMgr mgr;

	CQuery query;
	if (FALSE == query.Connect(3, "login", "account", g_DbManager.m_szLoginPWD))
	{
		AfxMessageBox("Error : Not Connect Login DB");
	}

	SetEvent(s_hHandle);

	HANDLE hIOCP = g_DbManager.GetIOCPHandle((int)nIndex);
	DWORD dwBytesTransferred = 0;
	LPDWORD lpCompletionKey = NULL;
	LPDB_OVERLAPPED_PLUS pData = NULL;

	while (1)
	{
		BOOL fOk = GetQueuedCompletionStatus(hIOCP,
			&dwBytesTransferred,
			(LPDWORD)&lpCompletionKey,
			(LPOVERLAPPED*)&pData,
			INFINITE);

		if (fOk == FALSE)
		{
			if (pData != NULL)
			{
				g_DbManager.m_pDbIOData->Free(pData);
				pData = NULL;
			}
			continue;
		}

		if (dwBytesTransferred == 0)	// terminate
		{
			if (pData != NULL)
			{
				g_DbManager.m_pDbIOData->Free(pData);
				pData = NULL;
			}
			return(0);
		}

		switch (pData->nQueryMode)
		{
		case CERTIFY:
			g_DbManager.Certify(query, pData, mgr);
			break;
		case CLOSE_EXISTING_CONNECTION:
			g_DbManager.CloseExistingConnection(query, pData);
			break;
		default:  Error("DbWorkerThread2 nQueryMode Invalid : [%d]", pData->nQueryMode); break;
		}

		if (pData != NULL)
		{
			g_DbManager.m_pDbIOData->Free(pData);
			pData = NULL;
		}

	}
	return(0);
}

void CDbManager::GetStrTime(CTime* pTime, char* strbuf)
{
	char sYear[5] = { 0, };
	char sMonth[3] = { 0, };
	char sDay[3] = { 0, };
	char sHour[3] = { 0, };
	char sMin[3] = { 0, };

	strncpy(sYear, strbuf, 4);
	strncpy(sMonth, strbuf + 4, 2);
	strncpy(sDay, strbuf + 6, 2);
	strncpy(sHour, strbuf + 8, 2);
	strncpy(sMin, strbuf + 10, 2);

	*pTime = CTime(atoi(sYear), atoi(sMonth), atoi(sDay), atoi(sHour), atoi(sMin), 0);
}

void CDbManager::PostQ(LPDB_OVERLAPPED_PLUS pData)
{
	UINT nKey = ::HashKey(pData->AccountInfo.szAccount);
	int nIOCP = nKey % DEFAULT_DB_WORKER_THREAD_NUM;

	::PostQueuedCompletionStatus(GetIOCPHandle(nIOCP), 1, NULL, (LPOVERLAPPED)pData);
}

HANDLE CDbManager::GetIOCPHandle(int n)
{
	return m_hIOCP[n];
}
