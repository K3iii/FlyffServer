#include "stdafx.h"

#if __VER >= 12 // __LORD

#include "tlord.h"
#include "lordskillexecutable.h"
#include "cmnhdr.h"
#include "dptrans.h"
#include "dbmanager.h"
#include "definetext.h"
#include "playerdata.h"

extern	AppInfo		g_appInfo;

#pragma warning ( disable : 4355 )

//////////////////////////////////////////////////////////////////////////

CTLord::CTLord()
	: CLord(),
	m_pController(0)
{
}

CTLord::~CTLord()
{
}

void CTLord::CreateColleagues(void)
{

	m_pController = new CLController(this);
	m_pController->CreateDbHandler(MIN(1));
	CTElection* pElection = new CTElection(this);
	CLEvent* pEvent = new CLEvent(this);
	CLordSkill* pSkills = new CTLordSkill(this);

	pElection->Initialize("election.inc");
	pEvent->Initialize("lordevent.inc");
	pSkills->Initialize("lordskill.txt");

	m_pElection = pElection;
	m_pEvent = pEvent;
	m_pSkills = pSkills;

	if (!RestoreAll())
	{
		election::OutputDebugString("CTLord.CreateColleagues");
		Error("CTLord.CreateColleagues");
		exit(-1);
	}
}

void CTLord::DestroyColleagues(void)
{
	if (m_pController)
		m_pController->CloseDbHandler();

	SAFE_DELETE(m_pElection);
	SAFE_DELETE(m_pEvent);
	SAFE_DELETE(m_pSkills);
	SAFE_DELETE(m_pController);
}

CTLord* CTLord::Instance(void)
{
	static	CTLord	sLord;
	return &sLord;
}

BOOL CTLord::PostRequest(int nQuery, BYTE* lpBuf, int nBufSize, DWORD dwCompletionKey)
{
	return m_pController->PostRequest(nQuery, lpBuf, nBufSize, dwCompletionKey);
}

void CTLord::Handler(LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey)
{
	switch (pov->nQueryMode)
	{
	case eBeginCandidacy:
		GetElection()->BeginCandidacy();
		break;
	case eBeginVote:
		GetElection()->BeginVote(0);
		break;
	case eEndVote:
		GetElection()->EndVote(0);
		break;
		//--------------------------------------------------------------------------------
	case eAddDeposit:
	{
		u_long idPlayer;
		__int64 iDeposit;
		CAr ar(pov->lpBuf, pov->uBufSize);
		ar >> idPlayer >> iDeposit;
		GetElection()->AddDeposit(idPlayer, iDeposit, time(NULL));
		break;
	}
	case eSetPledge:
	{
		u_long idPlayer;
		char szPledge[CCandidate::nMaxPledgeLen] = { 0, };
		CAr ar(pov->lpBuf, pov->uBufSize);
		ar >> idPlayer;
		ar.ReadString(szPledge, CCandidate::nMaxPledgeLen);
		GetElection()->SetPledge(idPlayer, szPledge);
		break;
	}
	case eIncVote:
	{
		u_long idPlayer, idElector;
		CAr ar(pov->lpBuf, pov->uBufSize);
		ar >> idPlayer >> idElector;
		GetElection()->IncVote(idPlayer, idElector);
		break;
	}
	//--------------------------------------------------------------------------------
	case eInit:

		CDPTrans::GetInstance()->SendLord(dwCompletionKey);
		break;
		//--------------------------------------------------------------------------------
	case eLEventCreate:
	{
		u_long idPlayer;
		int iEEvent, iIEvent;
		CAr ar(pov->lpBuf, pov->uBufSize);
		ar >> idPlayer >> iEEvent >> iIEvent;
#ifdef __FL_UPDATE_LORDEVENT
		int iPEvent;
		ar >> iPEvent;
		GetEvent()->AddComponent(idPlayer, iEEvent, iIEvent, iPEvent);
#else
		GetEvent()->AddComponent(idPlayer, iEEvent, iIEvent);
#endif
		break;
	}
	case eLEventInitialize:
	{
		GetEvent()->Initialize();
		break;
	}
	case eLordSkillUse:
	{
		u_long idPlayer, idTarget;
		int nSkill;
		CAr ar(pov->lpBuf, pov->uBufSize);
		ar >> idPlayer >> idTarget >> nSkill;
		CLordSkillComponentExecutable* pComponent = GetSkills()->GetSkill(nSkill);
		if (pComponent)
			pComponent->Execute(idPlayer, idTarget, static_cast<LPVOID>(GetController()));
		break;
	}
	}
}

void CTLord::OnTimer()
{
	TRACE("CTLord.OnTimer: %d\n", time(NULL));
	CTElection* pElection = static_cast<CTElection*>(GetElection());
	pElection->OnTimer();
	CLEvent* pEvent = static_cast<CLEvent*>(GetEvent());
	pEvent->OnTimer();
	CTLordSkill* pSkills = static_cast<CTLordSkill*>(GetSkills());
	pSkills->OnTimer();
}

BOOL CTLord::RestoreAll(void)
{
	return GetController()->Restore();
};

BOOL CTLord::Restore(CQuery* pQuery)
{
	if (!pQuery->Execute("uspRestoreLord %d", g_appInfo.dwSys))
		return FALSE;
	if (pQuery->Fetch())
		Set(pQuery->GetInt("idLord"));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CTElection::CTElection(CLord* pLord)
	:
	IElection(pLord),
#ifdef __INTERNALSERVER
	m_bRun(FALSE)
#else	// __INTERNALSERVER
	m_bRun(TRUE)
#endif	// __INTERNALSERVER
{
}

CTElection::~CTElection()
{
}

BOOL CTElection::Initialize(const char* szFile)
{
	if (!ReadIni(szFile))
	{
		election::OutputDebugString("CTElection::Initialize(): LoadScript(): file not found: %s", szFile);
		return FALSE;
	}
	return TRUE;
}

BOOL CTElection::ReadIni(const char* szFile)
{
	CScript s;
	if (s.Load(szFile) == FALSE)
		return FALSE;

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == _T("property"))
		{
			s.GetToken();	// {
			s.GetToken();
			while (*s.token != '}')
			{
				if (s.Token == _T("nDayOfWeek"))
					property.nDayOfWeek = s.GetNumber();
				else if (s.Token == _T("nHour"))
					property.nHour = s.GetNumber();
				else if (s.Token == _T("tCandidacy"))
					property.tCandidacy = s.GetNumber();
				else if (s.Token == _T("tVote"))
					property.tVote = s.GetNumber();
				else if (s.Token == _T("nDays"))
					property.nDays = s.GetNumber();
				s.GetToken();
			}
		}
		else if (s.Token == _T("rate"))
		{
			s.GetToken();	// {
			float fRate = s.GetFloat();
			while (*s.token != '}')
			{
				property.m_vReturnDepositRates.push_back(fRate);
				fRate = s.GetFloat();
			}
		}
		else if (s.Token == _T("fRequirementFactor"))
		{
			property.fRequirementFactor = s.GetFloat();
		}
		else if (s.Token == _T("item"))
		{
			int iIndex = s.GetNumber();
			s.GetToken();	// {
			int nItem = s.GetNumber();
			while (*s.token != '}')
			{
				property.m_vItems[iIndex].push_back(nItem);
				nItem = s.GetNumber();
			}
		}
		s.GetToken();
	}
	return TRUE;
}

CString	CTElection::ttos(time_t t)
{
	CTime tm(t);
	return tm.Format("%Y%m%d%H%M%S");
}

time_t CTElection::stot(const char* sz)
{
	if (strlen(sz) != 14)
		Error("CTElection.stot: %s", sz);
	char szYear[5] = { 0, };
	char szMonth[3] = { 0, };
	char szDay[3] = { 0, };
	char szHour[3] = { 0, };
	char szMin[3] = { 0, };
	char szSec[3] = { 0, };
	strncpy(szYear, sz, 4);
	strncpy(szMonth, sz + 4, 2);
	strncpy(szDay, sz + 6, 2);
	strncpy(szHour, sz + 8, 2);
	strncpy(szMin, sz + 10, 2);
	strncpy(szSec, sz + 12, 2);
	CTime tm(atoi(szYear), atoi(szMonth), atoi(szDay), atoi(szHour), atoi(szMin), atoi(szSec));
	return	((time_t)(tm.GetTime()));
}

BOOL CTElection::Restore(CQuery* pQuery)
{
	if (!pQuery->Execute("uspRestoreElection %d", g_appInfo.dwSys))
		return FALSE;
	if (pQuery->Fetch())
	{
		m_idElection = pQuery->GetInt("idElection");
		ELECTION_STATE eState = static_cast<ELECTION_STATE>(pQuery->GetInt("eState"));
		SetState(eState);
		SetRequirement((int)(pQuery->GetInt("nRequirement") * property.fRequirementFactor));
		char szBegin[17] = { 0, };
		pQuery->GetStr("szBegin", szBegin);
		SetBegin(CTElection::stot(szBegin));
	}
	if (!pQuery->Execute("uspRestoreCandidates %d, %d", g_appInfo.dwSys, GetId()))
	{
		Error("couldn't execute uspRestoreCandidates");
		return FALSE;
	}
	while (pQuery->Fetch())
	{
		u_long idPlayer = static_cast<u_long>(pQuery->GetInt("idPlayer"));
		__int64 iDeposit = pQuery->GetInt64("iDeposit");
		char szPledge[CCandidate::nMaxPledgeLen] = { 0, };
		pQuery->GetStr("szPledge", szPledge);
		int nVote = pQuery->GetInt("nVote");
		time_t tCreate = pQuery->GetInt("tCreate");
		AddCandidate(idPlayer, iDeposit, szPledge, nVote, tCreate);
	}
	if (GetState() == eVote)
		SortVote();
	else
		SortDeposit();

	if (!GetBegin() || GetState() == eExpired)
	{
		PrepareNext();
		if (!pQuery->Execute("uspElectionInitialize %d, %d, %s", g_appInfo.dwSys, GetId(), CTElection::ttos(GetBegin())))
		{
			Error("couldn't execute uspElectionInitialize");
			return FALSE;
		}
	}
	return TRUE;
}

float CTElection::GetRetRate(u_long idPlayer)
{
	int nOrder = GetOrder(idPlayer);
	nOrder = min(nOrder, IElection::nMaxCandidates);
	if (nOrder < 0)
		return 0.0F;
	return property.m_vReturnDepositRates[nOrder];
}

BOOL CTElection::DoTestBeginCandidacy(void)
{
	// IElection::BeginCandidacy
	election::OutputDebugString("CTElection.DoTestBeginCandidacy");
	CLController* pController = m_pLord->GetController();
	BOOL bResult = pController->BeginCandidacy();
	if (bResult)
		CDPTrans::GetInstance()->SendElectionBeginCandidacy();
	return bResult;
}

void CTElection::DoReturnDeposit(void)
{
	// IElection::BeginVote
	election::OutputDebugString("CTElection.DoReturnDeposit");
	CLController* pController = m_pLord->GetController();

	auto* m_pQuery = pController->GetQueryObject();
	for_each(m_vCandidates.begin() + IElection::nMaxCandidates, m_vCandidates.end(), [m_pQuery](const SPC& lhs)
		{
			const u_long idReceiver = lhs->GetIdPlayer();
			CTElection* pElection = static_cast<CTElection*>(CTLord::Instance()->GetElection());

			float fRate = pElection->GetRetRate(idReceiver);
			if (fRate == 0.0F)
				return;

			__int64 iDeposit = (__int64)(lhs->GetDeposit() / 100 * (fRate * 100));

			TCHAR szMailTitle[MAX_MAILTITLE] = { 0, };
			TCHAR szMailText[MAX_MAILTEXT] = { 0, };
			CItemElem kItemElem;

			const int nOrder = pElection->GetOrder(idReceiver);

			if (nOrder < IElection::nMaxCandidates)
			{
				strcpy(szMailTitle, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TITLE_02));	// title
				sprintf(szMailText, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TEXT_02),
					static_cast<__int64>(lhs->GetDeposit()),
					nOrder + 1,
					static_cast<float>(fRate * 100.0F),
					static_cast<__int64>(iDeposit));
			}
			else
			{
				strcpy(szMailTitle, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TITLE_01));	// title
				sprintf(szMailText, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TEXT_01),
					static_cast<__int64>(lhs->GetDeposit()),
					static_cast<float>(fRate * 100.0F),
					static_cast<__int64>(iDeposit));

				if (!m_pQuery->Execute("uspElectionLeaveOut %d, %d, %d", g_appInfo.dwSys, pElection->GetId(), idReceiver))
					Error("couldn't execute uspElectionLeaveOut");
			}

			// 반환금을 페린과 페냐로 분리하여 첨부
			const int nPerin = max(static_cast<int>(iDeposit / static_cast<__int64>(PERIN_VALUE)), 0);
			const int nPenya = max(static_cast<int>(iDeposit - static_cast<__int64>(PERIN_VALUE) * static_cast<__int64>(nPerin)), 0);

			if (nPerin > 0)
			{
				kItemElem.m_dwItemId = II_SYS_SYS_SCR_PERIN;
				kItemElem.m_nItemNum = nPerin;
				kItemElem.SetSerialNumber(xRand());
			}

			CDbManager::PostMail(*m_pQuery, idReceiver, 0, kItemElem, nPenya, szMailTitle, szMailText);
		});
}

BOOL CTElection::DoTestBeginVote(int& nRequirement)
{
	election::OutputDebugString("CTElection.DoTestBeginVote");
	CLController* pController = m_pLord->GetController();
	int nTotal = pController->BeginVote();
	if (nTotal >= 0)
	{
		nRequirement = static_cast<int>(nTotal * property.fRequirementFactor);
		CDPTrans::GetInstance()->SendElectionBeginVote(nRequirement);
		return TRUE;
	}
	return FALSE;
}

BOOL CTElection::DoTestEndVote(u_long idPlayer)
{
	// IElection::EndVote
	election::OutputDebugString("CTElection.DoTestEndVote");
	CLController* pController = m_pLord->GetController();
	if (pController->EndVote(idPlayer))
	{
		CDPTrans::GetInstance()->SendElectionEndVote(idPlayer);
		PostDepositToAllCandidate();
		PostLordItems(idPlayer);
		return TRUE;
	}
	return FALSE;
}

void CTElection::PostDepositToAllCandidate(void)
{
	CLController* pController = m_pLord->GetController();
	CQuery* m_pQuery = pController->GetQueryObject();

	for_each(m_vCandidates.begin(), m_vCandidates.end(), [m_pQuery](const SPC& lhs)
		{
			const u_long idReceiver = lhs->GetIdPlayer();
			CTElection* pElection = static_cast<CTElection*>(CTLord::Instance()->GetElection());

			float fRate = pElection->GetRetRate(idReceiver);
			if (fRate == 0.0F)
				return;
			__int64 iDeposit = (__int64)(lhs->GetDeposit() / 100 * (fRate * 100));

			TCHAR szMailTitle[MAX_MAILTITLE] = { 0, };
			TCHAR szMailText[MAX_MAILTEXT] = { 0, };
			CItemElem kItemElem;

			const int nOrder = pElection->GetOrder(idReceiver);

			if (nOrder < IElection::nMaxCandidates)
			{
				strcpy(szMailTitle, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TITLE_02));	// title
				sprintf(szMailText, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TEXT_02),
					static_cast<__int64>(lhs->GetDeposit()),
					nOrder + 1,
					static_cast<float>(fRate * 100.0F),
					static_cast<__int64>(iDeposit));
			}
			else
			{
				strcpy(szMailTitle, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TITLE_01));	// title
				sprintf(szMailText, prj.GetText(TID_GAME_ELECTION_RETURN_DEPOSIT_MAIL_TEXT_01),
					static_cast<__int64>(lhs->GetDeposit()),
					static_cast<float>(fRate * 100.0F),
					static_cast<__int64>(iDeposit));

				if (!m_pQuery->Execute("uspElectionLeaveOut %d, %d, %d", g_appInfo.dwSys, pElection->GetId(), idReceiver))
					Error("couldn't execute uspElectionLeaveOut");
			}

			const int nPerin = max(static_cast<int>(iDeposit / static_cast<__int64>(PERIN_VALUE)), 0);
			const int nPenya = max(static_cast<int>(iDeposit - static_cast<__int64>(PERIN_VALUE) * static_cast<__int64>(nPerin)), 0);

			if (nPerin > 0)
			{
				kItemElem.m_dwItemId = II_SYS_SYS_SCR_PERIN;
				kItemElem.m_nItemNum = nPerin;
				kItemElem.SetSerialNumber(xRand());
			}

			CDbManager::PostMail(*m_pQuery, idReceiver, 0, kItemElem, nPenya, szMailTitle, szMailText);
		});
}

BOOL CTElection::DoTestAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate)
{
	// IElection::AddDeposit
	election::OutputDebugString("CTElection.DoTestAddDeposit: %07d, %I64d, %d", idPlayer, iDeposit, tCreate);
	CLController* pController = m_pLord->GetController();
	BOOL bRet = (GetState() == eCandidacy) && pController->AddDeposit(idPlayer, iDeposit, tCreate);
	CDPTrans::GetInstance()->SendElectionAddDeposit(idPlayer, iDeposit, tCreate, bRet);
	return bRet;
}

void CTElection::DoAddDepositComplete(u_long idPlayer, __int64 iDeposit, time_t tCreate)
{
}

BOOL CTElection::DoTestSetPledge(u_long idPlayer, const char* szPledge)
{
	// IElection::SetPledge
	BOOL bRet = FALSE;
	election::OutputDebugString("CTElection.DoTestSetPledge: %07d, %s", idPlayer, szPledge);
	if (GetState() == eVote)
	{
		CLController* pController = m_pLord->GetController();
		bRet = pController->SetPledge(idPlayer, szPledge);
		CDPTrans::GetInstance()->SendElectionSetPledge(idPlayer, szPledge, bRet);
	}
	return bRet;
}

BOOL CTElection::DoTestIncVote(u_long idPlayer, u_long idElector)
{
	// IElection::IncVote
	election::OutputDebugString("CTElection.DoTestIncVote: %07d, %07d", idPlayer, idElector);
	CLController* pController = m_pLord->GetController();
	BOOL bRet = pController->IncVote(idPlayer, idElector);
	CDPTrans::GetInstance()->SendElectionIncVote(idPlayer, idElector, bRet);
	return bRet;
}

void CTElection::PostLordItems(u_long idLord)
{
	if (idLord != NULL_ID)
	{
		BYTE iIndex = SEX_MALE;
		{
			CMclAutoLock	Lock(CPlayerDataCenter::GetInstance()->m_Access);
			PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idLord);
			if (pData)
				iIndex = pData->data.nSex;
			ASSERT(iIndex >= 0 && iIndex < 2);
			if (iIndex < 0 || iIndex > 2)
				iIndex = 0;
		}
		for (vector<int>::iterator i = property.m_vItems[iIndex].begin(); i != property.m_vItems[iIndex].end(); ++i)
		{
			PostItemStruct pi;
			pi.nItem = *i;
			pi.dwKeepTime = time(NULL) + 1209600;
			PostItem(idLord, pi);
		}
	}
}

void CTElection::PostItem(u_long idPlayer, PostItemStruct& pi)
{
	TCHAR szMailTitle[MAX_MAILTITLE] = { 0, };
	TCHAR szMailText[MAX_MAILTEXT] = { 0, };
	CItemElem kItemElem;

	sprintf(szMailTitle, prj.GetText(TID_GAME_ELECTION_SET_LORD_MAIL_TITLE));
	sprintf(szMailText, prj.GetText(TID_GAME_ELECTION_SET_LORD_MAIL_TEXT));
	kItemElem.m_dwItemId = pi.nItem;
	kItemElem.m_nItemNum = 1;
	kItemElem.m_dwKeepTime = pi.dwKeepTime;
	kItemElem.SetSerialNumber(xRand());
	CLController* pController = m_pLord->GetController();

	CDbManager::PostMail(*pController->GetQueryObject(), idPlayer, 0, kItemElem, 0, szMailTitle, szMailText);
}

void CTElection::OnTimer(void)
{
	if (!IsRunable())
		return;

	if (GetState() != GetPropertyState())
	{
		if (GetState() == eReady)
			BeginCandidacy();
		else if (GetState() == eCandidacy)
			BeginVote(0);
		else if (GetState() == eVote)
			EndVote(0);
	}
}

//////////////////////////////////////////////////////////////////////////

CLEvent::CLEvent(CLord* pLord)
	:
	ILordEvent(pLord)
{
}

CLEvent::~CLEvent()
{
}

BOOL CLEvent::Restore(CQuery* pQuery)
{
	if (!pQuery->Execute("uspRestoreLEvent %d", g_appInfo.dwSys))
		return FALSE;
	while (pQuery->Fetch())
	{
		int nTick = pQuery->GetInt("nTick");
		u_long idPlayer = pQuery->GetInt("idPlayer");
		float fEFactor = pQuery->GetFloat("fEFactor");
		float fIFactor = pQuery->GetFloat("fIFactor");
#ifdef __FL_UPDATE_LORDEVENT
		float fPFactor = pQuery->GetFloat("fPFactor");
		CLEComponent* pComponent = new CLEComponent(nTick, idPlayer, fEFactor, fIFactor, fPFactor);
#else
		CLEComponent* pComponent = new CLEComponent(nTick, idPlayer, fEFactor, fIFactor);
#endif
		AddComponent(pComponent, FALSE);
	}
	return TRUE;
}

void CLEvent::OnTimer(void)
{

	BOOL bExpired = DecrementAllComponentTick();
	CDPTrans::GetInstance()->SendLEventTick(this);

	if (bExpired)
		EraseExpiredComponents();
}

BOOL CLEvent::DecrementAllComponentTick(void)
{

	BOOL bExpired = FALSE;
	for (VLEC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i)
	{
		if ((*i)->Decrement() == 0)
			bExpired = TRUE;
		m_pLord->GetController()->UpdateLordEventTick(*i);
	}
	return bExpired;
}

BOOL CLEvent::DoTestAddComponent(CLEComponent* pComponent)
{
	// ILordEvent::AddComponent
	CLController* pController = m_pLord->GetController();
	BOOL bResult = pController->AddLEComponent(pComponent);
	CDPTrans::GetInstance()->SendLEventCreate(pComponent, bResult);
	return bResult;
}

BOOL CLEvent::DoTestInitialize(void)
{
	// ILordEvent::Initialize
	CLController* pController = m_pLord->GetController();
	if (!pController->InitializeLEvent())
		return FALSE;
	CDPTrans::GetInstance()->SendLEventInitialize();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CLController::CLController(CTLord* pLord)
	:
	m_pLord(pLord),
	CDbController()
{
}

CLController::~CLController()
{
}

void CLController::Handler(LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey)
{
	m_pLord->Handler(pov, dwCompletionKey);
}

void CLController::OnTimer(void)
{
	m_pLord->OnTimer();
}

BOOL CLController::Restore(void)
{
	CQuery* pQuery = CreateQuery();
	if (!pQuery)
		return FALSE;
	CTElection* pElection = static_cast<CTElection*>(m_pLord->GetElection());
	CLEvent* pEvent = static_cast<CLEvent*>(m_pLord->GetEvent());
	CTLordSkill* pSkills = static_cast<CTLordSkill*>(m_pLord->GetSkills());

	if (m_pLord->Restore(pQuery) && pElection->Restore(pQuery)
		&& pEvent->Restore(pQuery) && pSkills->Restore(pQuery))
	{
		SAFE_DELETE(pQuery);
		return TRUE;
	}
	SAFE_DELETE(pQuery);
	return FALSE;
}

BOOL CLController::BeginCandidacy(void)
{
	char szQuery[255] = { 0, };
	IElection* pElection = m_pLord->GetElection();
	sprintf(szQuery, "uspElectionBeginCandidacy %d, %d", g_appInfo.dwSys, pElection->GetId());
	return GetQueryObject()->Exec(szQuery);
}

int CLController::BeginVote(void)
{
	int nRequirement = -1;
	char szQuery[255] = { 0, };
	IElection* pElection = m_pLord->GetElection();
	sprintf(szQuery, "uspElectionBeginVote %d, %d", g_appInfo.dwSys, pElection->GetId());
	if (GetQueryObject()->Exec(szQuery) && GetQueryObject()->Fetch())
		nRequirement = GetQueryObject()->GetInt("nRequirement");
	return nRequirement;
}

BOOL CLController::EndVote(u_long idPlayer)
{
	IElection* pElection = m_pLord->GetElection();
	return GetQueryObject()->Execute("uspElectionEndVote %d, %d, %d, %s", g_appInfo.dwSys, pElection->GetId(), idPlayer,
		(LPCSTR)CTElection::ttos(pElection->GetNextBegin()));
}

BOOL CLController::AddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate)
{
	char szQuery[255] = { 0, };
	IElection* pElection = m_pLord->GetElection();
	sprintf(szQuery, "uspElectionAddDeposit %d, %d, %d, %I64d, %d", g_appInfo.dwSys, pElection->GetId(), idPlayer, iDeposit, tCreate);
	return GetQueryObject()->Exec(szQuery);
}

BOOL CLController::SetPledge(u_long idPlayer, const char* szPledge)
{
	char szQuery[255] = { 0, };
	IElection* pElection = m_pLord->GetElection();
	sprintf(szQuery, "{call uspElectionSetPledge( %d, %d, %d, ? )}", g_appInfo.dwSys, pElection->GetId(), idPlayer);
	SQLINTEGER cbLen = SQL_NTS;
	if (!GetQueryObject()->BindParameter(1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, CCandidate::nMaxPledgeLen, 0, const_cast<char*>(szPledge), 0, &cbLen))
		return FALSE;
	return GetQueryObject()->Exec(szQuery);
}

BOOL CLController::IncVote(u_long idPlayer, u_long idElector)
{

	char szQuery[255] = { 0, };
	IElection* pElection = m_pLord->GetElection();
	sprintf(szQuery, "uspElectionIncVote %d, %d, %d, %d", g_appInfo.dwSys, pElection->GetId(), idPlayer, idElector);
	if (!GetQueryObject()->Exec(szQuery))
		return FALSE;
	if (!GetQueryObject()->Fetch())
		return FALSE;
	return static_cast<BOOL>(GetQueryObject()->GetInt("bResult"));
}

BOOL CLController::AddLEComponent(CLEComponent* pComponent)
{
#ifndef __FL_UPDATE_LORDEVENT
	if (GetQueryObject()->Execute("uspAddLEComponent %d, %d, %d, %3.2f, %3.2f", g_appInfo.dwSys, pComponent->GetIdPlayer(), pComponent->GetTick(), pComponent->GetEFactor(), pComponent->GetIFactor())
#else
	if (GetQueryObject()->Execute("uspAddLEComponent %d, %d, %d, %3.2f, %3.2f, %3.2f", g_appInfo.dwSys, pComponent->GetIdPlayer(), pComponent->GetTick(), pComponent->GetEFactor(), pComponent->GetIFactor(), pComponent->GetPFactor())
#endif
		&& GetQueryObject()->Fetch())
		return static_cast<BOOL>(GetQueryObject()->GetInt("bResult"));
		return FALSE;
}

BOOL CLController::InitializeLEvent(void)
{
	return GetQueryObject()->Execute("uspInitializeLEvent %d", g_appInfo.dwSys);
}

BOOL CLController::UpdateLordSkillTick(CLordSkillComponent* pSkill, int nTick)
{
	return GetQueryObject()->Execute("uspLordSkillTick %d, %d, %d", g_appInfo.dwSys, pSkill->GetId(), nTick);
}

BOOL CLController::UpdateLordEventTick(CLEComponent* pComponent)
{
	return GetQueryObject()->Execute("uspLordEventTick %d, %d, %d", g_appInfo.dwSys, pComponent->GetIdPlayer(), pComponent->GetTick());
}
//////////////////////////////////////////////////////////////////////////

CTLordSkill::CTLordSkill(CLord* pLord)
	: CLordSkill(pLord)
{
}

CTLordSkill::~CTLordSkill()
{
}

CLordSkillComponentExecutable* CTLordSkill::CreateSkillComponent(int nType)
{
	// CLordSkillComponentODBC
	return new CLordSkillComponentODBC;
}

BOOL CTLordSkill::Restore(CQuery* pQuery)
{
	if (!pQuery->Execute("uspRestoreLordSkill %d", g_appInfo.dwSys))
		return FALSE;
	while (pQuery->Fetch())
	{
		CLordSkillComponent* pComponent = GetSkill(pQuery->GetInt("nSkill"));
		pComponent->SetTick(pQuery->GetInt("nTick"));
	}
	return TRUE;
}

void CTLordSkill::OnTimer(void)
{
	CLController* pController = m_pLord->GetController();
	for (VLSC::iterator i = m_vComponents.begin(); i != m_vComponents.end(); ++i)
	{
		CLordSkillComponentExecutable* pSkill = *i;
		if (pSkill->GetTick() > 0)
		{

			pSkill->SetTick(pSkill->GetTick() - 1);

			pController->UpdateLordSkillTick(pSkill, pSkill->GetTick());
		}
	}
	CDPTrans::GetInstance()->SendLordSkillTick(this);
}
//////////////////////////////////////////////////////////////////////////

#endif	// __LORD