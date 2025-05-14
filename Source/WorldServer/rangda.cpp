#include "stdafx.h"

#if __VER >= 12 // __RANGDA_0521
#include "rangda.h"
#include "worldmng.h"
#include "lord.h"
extern	CWorldMng	g_WorldMng;

CRangda::CRangda(DWORD dwMonster)
	:
	m_dwMonster(dwMonster),
	m_nInterval(0),
	m_nReplace(0),
	m_bActiveAttack(FALSE),
	m_nGenerateCountdown(0),
	m_nReplaceCountdown(0),
	m_bReplaceable(TRUE),
	m_nOldPos(-1),
	m_objid(NULL_ID)
{
}

CRangda::CRangda(DWORD dwMonster, int nInterval, int nReplace, BOOL bActiveAttack)
	:
	m_dwMonster(dwMonster),
	m_nInterval(nInterval),
	m_nReplace(nReplace),
	m_bActiveAttack(bActiveAttack),
	m_nGenerateCountdown(0),
	m_nReplaceCountdown(0),
	m_bReplaceable(TRUE),
	m_nOldPos(-1),
	m_objid(NULL_ID)
{
}

CRangda::~CRangda()
{
}

void CRangda::AddPos(const RANGDA_POS& vPos)
{
	m_vvPos.push_back(vPos);
}

RANGDA_POS CRangda::GetRandomPos(void)
{
	ASSERT(!m_vvPos.empty());



	if (m_vvPos.size() == 1)
		return m_vvPos[0];

	int nPos;
	do {
		nPos = xRandom(0, m_vvPos.size());
	} while (nPos == m_nOldPos);

	m_nOldPos = nPos;
	return m_vvPos[nPos];
}

void CRangda::SetObj(OBJID objid)
{
	m_objid = objid;
	m_nGenerateCountdown = m_nInterval;
	m_nReplaceCountdown = m_nReplace;
	m_bReplaceable = TRUE;
}

void CRangda::OnTimer(void)
{
	CMover* pMonster = GetMonster();
	if (IsValidObj(pMonster))
		ProcessReplace(pMonster);
	else
		ProcessGenerate();
}

CMover* CRangda::GetMonster(void)
{
	if (m_objid != NULL_ID)
	{
		CMover* pMonster = prj.GetMover(m_objid);
		if (IsValidObj(pMonster))
			return pMonster;
		m_objid = NULL_ID;
	}
	return NULL;
}

void CRangda::ProcessReplace(CMover* pMonster)
{
	if (m_bReplaceable)
	{
		if (IsDamaged(pMonster))
			m_bReplaceable = FALSE;
		else if (HavetoReplace())
		{
			election::OutputDebugString("CRangda.ProcessReplace:");
			pMonster->Delete();
			CreateMonster();
		}
	}
}

void CRangda::ProcessGenerate(void)
{
	if (HavetoGenerate())
	{
		election::OutputDebugString("CRangda.ProcessGenerate:");
		CreateMonster();
	}
}

BOOL CRangda::IsDamaged(CMover* pMonster)
{
	return (pMonster->GetHitPoint() < pMonster->GetMaxHitPoint());
}

BOOL CRangda::HavetoReplace(void)
{
	if (m_nReplaceCountdown > 0)
		m_nReplaceCountdown--;
	return (m_nReplaceCountdown == 0);
}

BOOL CRangda::HavetoGenerate(void)
{
	if (m_nGenerateCountdown > 0)
		m_nGenerateCountdown--;
	return (m_nGenerateCountdown == 0);
}

void CRangda::CreateMonster(void)
{
	RANGDA_POS pos = GetRandomPos();
	CWorld* pWorld = g_WorldMng.GetWorld(pos.dwWorldId);
	if (!pWorld)
		return;
	CMover* pMonster = static_cast<CMover*>(::CreateObj(D3DDEVICE, OT_MOVER, m_dwMonster));
	if (pMonster)
	{
		pMonster->m_bActiveAttack = m_bActiveAttack;
		pMonster->SetPos(pos.vPos);
		pMonster->InitMotion(MTI_STAND);
		pMonster->UpdateLocalMatrix();
		pMonster->AddItToGlobalId();
		if (pWorld->ADDOBJ(pMonster, FALSE, nDefaultLayer) == TRUE)
		{
			SetObj(pMonster->GetId());
			election::OutputDebugString("CRangda.CreateMonster: monster = %d, x = %0.2f, y = %0.2f, z = %0.2f\n",
				pMonster->GetIndex(), pMonster->GetPos().x, pMonster->GetPos().y, pMonster->GetPos().z);
		}
		else
		{
			SAFE_DELETE(pMonster);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

CRangdaController::CRangdaController()
{
}

CRangdaController::~CRangdaController()
{
	for (VR::iterator i = m_vRangda.begin(); i != m_vRangda.end(); ++i)
		SAFE_DELETE(*i);
	m_vRangda.clear();
}

CRangdaController* CRangdaController::Instance(void)
{
	static CRangdaController sRangdaController;
	return &sRangdaController;
}

void CRangdaController::AddRangda(CRangda* pRangda)
{
	m_vRangda.push_back(pRangda);
}

void CRangdaController::OnTimer(void)
{
	for (VR::iterator i = m_vRangda.begin(); i != m_vRangda.end(); ++i)
		(*i)->OnTimer();
}

BOOL CRangdaController::LoadScript(const char* szFile)
{
	CScript s;
	if (s.Load(szFile) == FALSE)
		return FALSE;

	DWORD dwMonster = s.GetNumber();		// subject or FINISHED
	while (s.tok != FINISHED)
	{
		CRangda* pRangda = new CRangda(dwMonster);
		AddRangda(pRangda);
		s.GetToken();	// {
		s.GetToken();	// subject or '}'

		while (*s.token != '}')
		{
			if (s.Token == _T("nInterval"))
				pRangda->SetInterval(s.GetNumber());
			else if (s.Token == _T("nReplace"))
				pRangda->SetReplace(s.GetNumber());
			else if (s.Token == _T("bActiveAttack"))
				pRangda->SetActiveAttack(static_cast<BOOL>(s.GetNumber()));
			else if (s.Token == _T("vRangda"))
			{
				RANGDA_POS pos;
				s.GetToken();	// {
				pos.dwWorldId = s.GetNumber();
				while (*s.token != '}')
				{
					pos.vPos.x = s.GetFloat();
					pos.vPos.y = s.GetFloat();
					pos.vPos.z = s.GetFloat();
					pRangda->AddPos(pos);
					pos.dwWorldId = s.GetNumber();
				}
			}
			s.GetToken();
		}
		dwMonster = s.GetNumber();
	}
	return TRUE;
}

#endif	// __RANGDA_0521