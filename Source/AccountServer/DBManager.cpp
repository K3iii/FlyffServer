#include "stdafx.h"
#include "dbmanager.h"
#include "account.h"
#include <afxdisp.h>
#include "lang.h"

CDbManager	g_DbManager;

extern	CAccountMng		g_AccountMng;
#include "dpdbsrvr.h"
extern	CDPDBSrvr	g_dpDbSrvr;


CDbManager::CDbManager()
{
	m_pDbIOData = new CMemPool<DB_OVERLAPPED_PLUS>(512);
	m_bTracking = FALSE;
	m_bLogItem = TRUE;
#ifdef __INTERNALSERVER
	m_bLogItem = FALSE;
#endif

	m_szLoginPWD[0] = '\0';
}

CDbManager::~CDbManager()
{
	SAFE_DELETE(m_pDbIOData);
}

HANDLE s_hHandle = (HANDLE)NULL;

void CDbManager::CreateDbWorkers(void)
{
	s_hHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hDbCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	ASSERT(m_hDbCompletionPort);
	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++)
	{
		HANDLE hThread = chBEGINTHREADEX(NULL, 0, DbWorkerThread, (LPVOID)this, 0, NULL);
		ASSERT(hThread);
		m_hDbWorkerThreadTerminate[i] = hThread;

		if (WaitForSingleObject(s_hHandle, SEC(3)) == WAIT_TIMEOUT)
			OutputDebugString("ACCOUNTSERVER.EXE\t// TIMEOUT\t// ODBC");
	}

	CloseHandle(s_hHandle);
}

void CDbManager::CloseDbWorkers(void)
{
	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++)
		PostQueuedCompletionStatus(m_hDbCompletionPort, 0, NULL, NULL);

	WaitForMultipleObjects(DEFAULT_DB_WORKER_THREAD_NUM, m_hDbWorkerThreadTerminate, TRUE, INFINITE);

	CLOSE_HANDLE(m_hDbCompletionPort);

	for (int i = 0; i < DEFAULT_DB_WORKER_THREAD_NUM; i++) {
		CLOSE_HANDLE(m_hDbWorkerThreadTerminate[i]);
	}

	SAFE_DELETE(m_pDbIOData);
}

void CDbManager::SetTracking(BOOL bEnable)
{
	m_bTracking = bEnable;
	if (GetLanguage() == LANG_TWN)
		m_bTracking = TRUE;
}

void CDbManager::SetLogging(BOOL bEnable)
{
	m_bLogItem = bEnable;
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

void CDbManager::DBQryAccount(char* qryAccount, char* Gu, char* szAccount)
{
	sprintf(qryAccount, "LOGINJOIN_STR '%s', '%s'", Gu, szAccount);
}

void CDbManager::UpdateTracking(BOOL bON, LPCTSTR lpszAccount)
{
	if (m_bTracking == FALSE)
		return;

#ifdef __FIX_SQL_INJECTS // test thingy
	CString strAccount;
	strAccount.Format("%s", lpszAccount);

	if (strAccount.Find("--") >= 0 || strAccount.Find("'") >= 0 || strAccount.Find("/*") >= 0 || strAccount.Find("*/") >= 0 || strAccount.Find(";") >= 0)
	{
		Error("CDbManager::UpdateTracking() Account: %s", strAccount);
		return;
	}
#endif

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = m_pDbIOData->Alloc();
	strcpy(lpDbOverlappedPlus->szAccount, lpszAccount);
	if (bON)
		lpDbOverlappedPlus->nQueryMode = ACCOUNT_ON;
	else
		lpDbOverlappedPlus->nQueryMode = ACCOUNT_OFF;

	PostQueuedCompletionStatus(m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}


BOOL CDbManager::AllOff()
{
	if (m_bTracking == FALSE)
		return TRUE;

	CQuery qry;
	if (FALSE == qry.Connect(3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, m_szLoginPWD))
	{
		AfxMessageBox("Error AllOff: DB Connect Loing InitInstance Failed ");
		return FALSE;
	}

	char szQuery[1024] = { 0, };
	DBQryAccount(szQuery, "A3");

	if (FALSE == qry.Exec(szQuery))
	{
		AfxMessageBox(" DB Qry : ALL_OFF 구문 실패 ");
		return FALSE;
	}
	return TRUE;
}

void CDbManager::AccountOn(CQuery& qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	DBQryAccount(lpDbOverlappedPlus->szQuery, "A1", lpDbOverlappedPlus->szAccount);
	if (FALSE == qry.Exec(lpDbOverlappedPlus->szQuery))
		WriteLog("%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery);
}


void CDbManager::AccountOff(CQuery& qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	if (GetLanguage() == LANG_TWN)
	{
		sprintf(lpDbOverlappedPlus->szQuery, "dbo.uspExtendSession @paccount = '%s'", lpDbOverlappedPlus->szAccount);
		if (FALSE == qry.Exec(lpDbOverlappedPlus->szQuery))
			WriteLog("%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery);
	}
	DBQryAccount(lpDbOverlappedPlus->szQuery, "A2", lpDbOverlappedPlus->szAccount);
	if (FALSE == qry.Exec(lpDbOverlappedPlus->szQuery))
		WriteLog("%s, %d\r\n\t%s", __FILE__, __LINE__, lpDbOverlappedPlus->szQuery);
}

u_int __stdcall DbWorkerThread(LPVOID lpvDbManager)
{
	CDbManager* pDbManager = (CDbManager*)lpvDbManager;

	CQuery qryLogin;
	CQuery qryLog;

	if (pDbManager->m_bTracking)
	{
		if (FALSE == qryLogin.Connect(3, DSN_NAME_LOGIN, DB_ADMIN_ID_LOGIN, pDbManager->m_szLoginPWD))
		{
			AfxMessageBox(" DB Login Connect Failed ");
			return 0;
		}
	}


	if (pDbManager->m_bLogItem)
	{
		if (FALSE == qryLog.Connect(3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pDbManager->m_szLogPWD))
		{
			AfxMessageBox(" DB Log Connect Failed ");
			return 0;
		}
	}

	SetEvent(s_hHandle);

	BOOL fOk;
	HANDLE hCompletionPort = pDbManager->m_hDbCompletionPort;
	DWORD dwBytesTransferred = 0;
	LPDWORD lpCompletionKey = NULL;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL;

	while (1)
	{
		fOk = GetQueuedCompletionStatus(hCompletionPort,
			&dwBytesTransferred,
			(LPDWORD)&lpCompletionKey,
			(LPOVERLAPPED*)&lpDbOverlappedPlus,
			INFINITE);

		if (fOk == FALSE)
		{
			if (lpDbOverlappedPlus != NULL)
			{
				g_DbManager.m_pDbIOData->Free(lpDbOverlappedPlus);
				lpDbOverlappedPlus = NULL;
			}
			continue;
		}

		if (dwBytesTransferred == 0)	// terminate
		{
			if (lpDbOverlappedPlus != NULL)
			{
				g_DbManager.m_pDbIOData->Free(lpDbOverlappedPlus);
				lpDbOverlappedPlus = NULL;
			}
			return(0);
		}

		switch (lpDbOverlappedPlus->nQueryMode)
		{
		case ACCOUNT_ON:
			pDbManager->AccountOn(qryLogin, lpDbOverlappedPlus);
			break;
		case ACCOUNT_OFF:
			pDbManager->AccountOff(qryLogin, lpDbOverlappedPlus);
			break;
		default: Error("DbWorkerThread1 nQueryMode Invalid : [%d]", lpDbOverlappedPlus->nQueryMode); break;
		}

		if (lpDbOverlappedPlus != NULL)
		{
			g_DbManager.m_pDbIOData->Free(lpDbOverlappedPlus);
			lpDbOverlappedPlus = NULL;
		}

	}
	return(0);
}

