#include "stdafx.h"
#include "dbcontroller.h"
#include "post.h"
#include "dptrans.h"

CDbController::CDbController()
	:
	m_pQuery(0),
	m_hIocp(0),
	m_hDbHndlr(0),
	m_dwTimer(0)
{
}

CDbController::~CDbController()
{
}

BOOL CDbController::CreateDbHandler(DWORD dwMilliseconds)
{
	m_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_hIocp)
		return FALSE;
	DWORD dwThreadId;
	m_hDbHndlr = chBEGINTHREADEX(NULL, 0, _DbHandler, (LPVOID)this, 0, &dwThreadId);
	if (!m_hDbHndlr)
	{
		::CloseHandle(m_hIocp);
		return FALSE;
	}

	SetTimer(dwMilliseconds);

	return TRUE;
}

void CDbController::SetTimer(DWORD dwMilliseconds)
{
	if (dwMilliseconds != INFINITE)
	{
		m_dwTimer = dwMilliseconds;
		CDbControllerTimer::GetInstance()->Register(this);
	}
}

void CDbController::CloseDbHandler(void)
{
	if (m_hIocp)
	{
		::PostQueuedCompletionStatus(m_hIocp, 0, NULL, NULL);
		::WaitForSingleObject(m_hDbHndlr, INFINITE);
		CLOSE_HANDLE(m_hIocp);
		CLOSE_HANDLE(m_hDbHndlr);
	}
}

BOOL CDbController::PostRequest(int nQuery, BYTE* lpBuf, int nBufSize, DWORD dwCompletionKey)
{
	LPDB_OVERLAPPED_PLUS pov = new DB_OVERLAPPED_PLUS;

	if (lpBuf != NULL)
	{
		if (nBufSize < 0 || nBufSize >(1024 * 1024))
		{
			Error("PostRequest nBufSize error. nQuery[%d], nBufSize[%d]", nQuery, nBufSize);

			return	FALSE;
		}
		else
		{
			pov->lpBuf = new BYTE[nBufSize];
			::memset(pov->lpBuf, 0, nBufSize);

			::memcpy(pov->lpBuf, lpBuf, nBufSize);
			pov->uBufSize = nBufSize;
		}
	}


	pov->nQueryMode = nQuery;
	return ::PostQueuedCompletionStatus(m_hIocp, 1, dwCompletionKey, &pov->Overlapped);
}

u_long	CDbController::_DbHandler(LPVOID* pParam)
{
	CDbController* pController = reinterpret_cast<CDbController*>(pParam);
	pController->DbHandler();
	return 0;
}

CQuery* CDbController::CreateQuery(void)
{
	CQuery* pQuery = new CQuery;
	const char* pass = CDbManager::GetInstance().DB_ADMIN_PASS_CHARACTER01;
	if (!pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, pass))
	{
		::AfxMessageBox("Can't connect db: CDbController.CreateQuery");
		SAFE_DELETE(pQuery);
	}
	return pQuery;
}

void CDbController::DbHandler(void)
{
	CQuery* pQuery = CreateQuery();
	if (pQuery == NULL)
	{
		Error("CreateQuery Failed");
		return;
	}

	SetQueryObject(pQuery);

	BOOL bReturnValue = FALSE;
	DWORD dwBytesTransferred = 0;
	DWORD dwCompletionKey = 0;
	LPDB_OVERLAPPED_PLUS pov = NULL;

	while (1)
	{
		bReturnValue
			= ::GetQueuedCompletionStatus(m_hIocp,
				&dwBytesTransferred,
				&dwCompletionKey,
				(LPOVERLAPPED*)&pov,
				INFINITE
			);

		if (FALSE == bReturnValue)
		{
			Error("GetQueuedCompletionStatus return FALSE. Error:[%d]", ::GetLastError());
			continue;
		}

		if (dwBytesTransferred == 0)
		{
			SAFE_DELETE(pQuery);
			return;
		}

		if (pov->nQueryMode == eTimer)
			OnTimer();

		Handler(pov, dwCompletionKey);

		SAFE_DELETE_ARRAY(pov->lpBuf);

		SAFE_DELETE(pov);
	}
}

//////////////////////////////////////////////////////////////////////////
CDbControllerTimer::CDbControllerTimer()
	:
	m_hTickThread(0),
	m_hCloseTick(0)
{
}

CDbControllerTimer::~CDbControllerTimer()
{
}

void CDbControllerTimer::Register(CDbController* dbCtrl)
{
	CMclAutoLock Lock(m_csLock);
	m_vDbController.push_back(make_pair(dbCtrl, ::GetTickCount() + dbCtrl->GetTimer()));
}

u_int CDbControllerTimer::_Tick(LPVOID pParam)
{
	CDbControllerTimer* pTimer = reinterpret_cast<CDbControllerTimer*>(pParam);
	pTimer->Tick();
	return 0;
}

void CDbControllerTimer::Tick(void)
{
	while (WaitForSingleObject(m_hCloseTick, SEC(1)) != WAIT_OBJECT_0)
	{
		DWORD dwTick = GetTickCount();
		CMclAutoLock Lock(m_csLock);
		for (VPDD::iterator i = m_vDbController.begin(); i != m_vDbController.end(); i++)
		{
			if (dwTick > (*i).second)
			{
				(*i).first->PostRequest(CDbController::eTimer, NULL, 0, 0);
				(*i).second += (*i).first->GetTimer();
			}
		}
	}
}

void CDbControllerTimer::Create(void)
{
	DWORD dwThreadId = 0;
	m_hCloseTick = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hTickThread = chBEGINTHREADEX(NULL, 0, _Tick, (LPVOID)this, 0, &dwThreadId);
	ASSERT(m_hCloseTick);
	ASSERT(m_hTickThread);
}

void CDbControllerTimer::Destroy(void)
{
	CLOSE_THREAD(m_hTickThread, m_hCloseTick);
}

CDbControllerTimer* CDbControllerTimer::GetInstance(void)
{
	static CDbControllerTimer sDbControllerTimer;
	return &sDbControllerTimer;
}