#include "stdafx.h"
#include "Environment.h"

#ifdef __CORESERVER
#include "dpcoresrvr.h"
extern	CDPCoreSrvr g_dpCoreSrvr;
#endif // __CORESERVER

/*--------------------------------------------------------------------------------*/

CEnvironment::CEnvironment()
{
#ifdef __WORLDSERVER
	::memset(m_aEnvironmentEffect, 0, sizeof(m_aEnvironmentEffect));
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_nSeason = SEASON_NONE;
#endif // __CLIENT

	m_bEffect = FALSE;
	m_tEffectTime.Set(MIN(60), TRUE);

#ifdef __CORESERVER
	m_Authority = FALSE;
	m_hWorker = m_hCloseWorker = NULL;
#endif // __CORESERVER
}

CEnvironment::~CEnvironment()
{
#ifdef __CORESERVER
	CloseWorkers();
#endif // __CORESERVER
}


CEnvironment* CEnvironment::GetInstance()
{
	static CEnvironment sEnvironment;
	return &sEnvironment;
}

#if defined (__WORLDSERVER)
BOOL CEnvironment::LoadScript()
{
	CScript scanner;

	if (scanner.Load("EnvironmentEffect.txt", FALSE) == FALSE)
	{
		Error("CEnvironment::LoadScript - Script Load Failed");
		return FALSE;
	}

	::memset(m_aEnvironmentEffect, 0, sizeof(m_aEnvironmentEffect));

	int nLang = scanner.GetNumber();
	while (scanner.tok != FINISHED)
	{
		if (nLang < 0 || nLang >= LANG_MAX)
		{
			Error("CEnvironment::LoadScript - Invalid Lang : %[d]", nLang);
			return FALSE;
		}

		for (int i = 0; i < 12; ++i)
		{
			int nSeason = scanner.GetNumber();
			if (nSeason < 0 || nSeason >= SEASON_MAX)
			{
				Error("CEnvironment::LoadScript - Invalid Season : %[d]", nSeason);
				return FALSE;
			}
			m_aEnvironmentEffect[nLang][i] = nSeason;
		}
		nLang = scanner.GetNumber();
	}
	return TRUE;
}

int CEnvironment::GetSeason()
{
	int nLang = ::GetLanguage();

	if (nLang < 0 || nLang >= LANG_MAX)
	{
		Error("CEnvironment::GetSeason - Invalid argument. Lang : [%d]", nLang);
		return SEASON_NONE;
	}

	CTime timeCurr = CTime::GetCurrentTime();
	int nMonth = timeCurr.GetMonth();

	return m_aEnvironmentEffect[nLang][nMonth - 1];
}
#endif // __WORLDSERVER

#ifdef __CLIENT
void CEnvironment::SetSeason(int nSeason)
{
	if (nSeason < SEASON_NONE || nSeason >= SEASON_MAX)
	{
		m_nSeason = SEASON_NONE;
	}
	else
	{
		m_nSeason = nSeason;
	}
}
#endif // __CLIENT

void CEnvironment::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_bEffect;
	}
	else
	{
		ar >> m_bEffect;
	}
}

#ifdef __CORESERVER
BOOL CEnvironment::CreateWorkers(void)
{
	DWORD dwThreadId;
	m_hCloseWorker = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hWorker = chBEGINTHREADEX(NULL, 0, _Worker, this, 0, &dwThreadId);
	if (m_hWorker == NULL)
		return FALSE;
	return TRUE;
}

void CEnvironment::CloseWorkers(void)
{
	CLOSE_THREAD(m_hWorker, m_hCloseWorker);
}

UINT CEnvironment::_Worker(LPVOID pParam)
{
	CEnvironment* pEnvironment = (CEnvironment*)pParam;
	pEnvironment->Worker();
	return 0;
}

void CEnvironment::Worker(void)
{
	HANDLE hHandle = m_hCloseWorker;
	while (WaitForSingleObject(hHandle, 60000) == WAIT_TIMEOUT)
	{
		if (m_Authority)
		{
			if (m_bEffect == TRUE)
			{
				m_Authority = FALSE;
				m_tEffectTime.Reset();
				g_dpCoreSrvr.SendEnvironmentEffect();
			}
		}
		else
		{
			if (m_tEffectTime.IsTimeOut() == TRUE)
			{
				const bool prevEffect = m_bEffect;
				m_bEffect = (::xRandom(10000) < 3000) ? true : false;
				m_tEffectTime.Reset();

				if (prevEffect != m_bEffect)
					g_dpCoreSrvr.SendEnvironmentEffect();
			}
		}

		TRACE("CEnvironment Worker\n");
	}
}
#endif // __CORESERVER