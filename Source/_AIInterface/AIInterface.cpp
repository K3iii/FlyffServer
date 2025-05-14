#include "stdafx.h"
#include "AIInterface.h"
#include "DefineSkill.h"

#ifndef __CLIENT
#include "User.h"
#endif	// __CLIENT


BEGIN_AISTATE_MAP(CAIInterface, CAIInterface)
END_AISTATE_MAP()

CAIInterface::CAIInterface()
{
	m_pObj = NULL;
	m_pStateFunc = NULL;
	m_uParty = 0;
}
CAIInterface::CAIInterface(CObj* pObj)
{
	m_pObj = pObj;
	m_pStateFunc = NULL;
	m_uParty = 0;
}

CAIInterface::~CAIInterface()
{
}

FUNCAISTATE CAIInterface::GetAIStateFunc(DWORD dwState)
{
	const STATEMAP* lpCurrentStateMap = GetStateMap();
	const STATEMAP_ENTRIES* lpEntries = lpCurrentStateMap->lpStateMapEntries;

	while (lpCurrentStateMap->lpBaseStateMap->lpStateMapEntries != lpEntries)
	{
		int i = 0;
		while (lpEntries[i].pStateFunc)
		{
			if (lpEntries[i].dwAIStateId == dwState)
			{
				return lpEntries[i].pStateFunc;
			}
			i++;
		}

		lpCurrentStateMap = lpCurrentStateMap->lpBaseStateMap;
		lpEntries = lpCurrentStateMap->lpStateMapEntries;
	}
	return NULL;
}


void CAIInterface::RouteMessage()
{
	AIMSG msg;

	while (!m_MsgQueue.empty())
	{
		msg = m_MsgQueue.front();
		m_MsgQueue.pop();
		if (msg.dwMessage == AIMSG_SETSTATE)
		{
			msg.dwMessage = AIMSG_INIT;
			m_pStateFunc = GetAIStateFunc(msg.dwParam1);
		}
		if (m_pStateFunc)
			(this->*(m_pStateFunc)) (msg);
	}
	if (m_pStateFunc)
	{
		msg.dwMessage = AIMSG_PROCESS;
		msg.dwParam1 = 0;
		msg.dwParam2 = 0;
		(this->*(m_pStateFunc)) (msg);
	}
}

void CAIInterface::SendAIMsg(DWORD dwMessage, DWORD dwParam1, DWORD dwParam2)
{
	AIMSG msg;
	msg.dwMessage = dwMessage;
	msg.dwParam1 = dwParam1;
	msg.dwParam2 = dwParam2;
	if (dwMessage == AIMSG_SETSTATE)
	{
		msg.dwMessage = AIMSG_INIT;
		m_pStateFunc = GetAIStateFunc(dwParam1);
	}
	if (m_pStateFunc)
		(this->*(m_pStateFunc)) (msg);
}

void CAIInterface::PostAIMsg(DWORD dwMessage, DWORD dwParam1, DWORD dwParam2)
{
	switch (dwMessage)
	{
	case AIMSG_ARRIVAL:
	{
		AIMSG msg;
		msg.dwMessage = dwMessage;
		msg.dwParam1 = dwParam1;
		msg.dwParam2 = dwParam2;
		m_MsgQueue.push(msg);
		break;
	}
	default:	SendAIMsg(dwMessage, dwParam1, dwParam2);	break;
	}
}

CMover* CAIInterface::ScanTargetNext(CObj* pObjCenter, int nRange, OBJID dwIdTarget, u_long uParty)
{
	if (uParty == 0 || nRange <= 0)
		return NULL;

	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	CMover* pTarget = NULL;

	BOOL bFlyMob = GetMover()->IsFlyingNPC();
	float fRadius = (float)(nRange * nRange);

	FOR_LINKMAP(pObjCenter->GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetMover()->GetLayer())
	{
		if (pObj->GetType() != OT_MOVER)
		{
			continue;
		}
		else
		{
			if (((CMover*)pObj)->IsPlayer() == FALSE)
				continue;
		}

		pTarget = (CMover*)pObj;
		if (pTarget->IsLive() && pTarget->GetId() != dwIdTarget)
		{
			if (bFlyMob == pTarget->IsFly() || bFlyMob == FALSE)
			{
				vDist = pTarget->GetPos() - vPos;
				if (D3DXVec3LengthSq(&vDist) < fRadius && pTarget->m_idparty == uParty)
				{
					if (pTarget->IsMode(TRANSPARENT_MODE) == FALSE)
						if (pTarget->HasBuffByIk3(IK3_TEXT_DISGUISE) == FALSE)
							return pTarget;
				}
			}
		}
	}
	END_LINKMAP
		return NULL;
}

CMover* CAIInterface::ScanTarget(CObj* pObjCenter, int nRangeMeter, int nJobCond, DWORD dwQuest, DWORD dwItem, int nChao)
{
	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;
	CMover* pTarget = NULL;
	int nAble = 0;
	BOOL bCondScan = FALSE;
	if (nJobCond || dwQuest || dwItem || nChao)
		bCondScan = TRUE;

	BOOL bGuard = FALSE;
	BOOL bFlyMob = GetMover()->IsFlyingNPC();

	float fRadius = (float)(nRangeMeter * nRangeMeter);

	FOR_LINKMAP(pObjCenter->GetWorld(), vPos, pObj, nRangeMeter, CObj::linkPlayer, GetMover()->GetLayer())
	{
		if (pObj->GetType() != OT_MOVER)
		{
			continue;
		}
		else
		{
			if (((CMover*)pObj)->IsPlayer() == FALSE)
				continue;
		}

		pTarget = (CMover*)pObj;


		nAble = 0;

		if (bCondScan == TRUE && pTarget->IsLive())
		{
			if (nJobCond == 0)
				nAble++;
			else if (nJobCond == JOB_ALL || pTarget->GetJob() == nJobCond)
				nAble++;

			if (dwItem == 0)
				nAble++;
			else if (pTarget->GetItemNum(dwItem) != 0)
				nAble++;

			if (dwQuest == 0)
				nAble++;
			else if (pTarget->FindQuest(dwQuest) != NULL)
				nAble++;

			if (nChao == 0)
				nAble++;
			else
			{
				if (nChao == 101)
					nAble++;
			}
		}
		if (nAble == 4)
		{
#ifdef __JHMA_VER_8_6
			if (bGuard || bFlyMob == pTarget->IsFly() || bFlyMob == FALSE)
#else	// __VER >= 8
			if (bGuard || bFlyMob == pTarget->IsFly())
#endif	// __VER >= 8
			{
				vDist = pTarget->GetPos() - vPos;
				if (D3DXVec3LengthSq(&vDist) < fRadius)
				{
					if (pTarget->IsMode(TRANSPARENT_MODE) == FALSE)
						if (pTarget->HasBuffByIk3(IK3_TEXT_DISGUISE) == FALSE)
							return pTarget;
				}
			}
		}
	}
	END_LINKMAP
		return NULL;
}


CMover* CAIInterface::ScanTargetStrong(CObj* pObjCenter, FLOAT fRangeMeter)
{
	CObj* pObj;
	CObj* pObjMax = NULL;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;


	FLOAT fRadius = fRangeMeter * fRangeMeter;

	FOR_LINKMAP(pObjCenter->GetWorld(), vPos, pObj, (int)(fRangeMeter / MPU), CObj::linkPlayer, GetMover()->GetLayer())
	{

		if (pObj->GetType() != OT_MOVER)
		{
			continue;
		}
		else
		{
			if (((CMover*)pObj)->IsPlayer() == FALSE)
				continue;
		}

		vDist = pObj->GetPos() - vPos;
		float fDistSq = D3DXVec3LengthSq(&vDist);
		if (fDistSq < fRadius)
		{
			if (!(((CMover*)pObj)->IsMode(TRANSPARENT_MODE)))
			{
				if (pObjMax)
				{
					if (((CMover*)pObj)->GetLevel() > ((CMover*)pObjMax)->GetLevel())
						pObjMax = pObj;
				}
				else
					pObjMax = pObj;

			}
		}
	}
	END_LINKMAP

		return (CMover*)pObjMax;
}


CMover* CAIInterface::ScanTargetOverHealer(CObj* pObjCenter, FLOAT fRangeMeter)
{
#ifndef __CLIENT
	CObj* pObj;
	D3DXVECTOR3 vPos = pObjCenter->GetPos();
	D3DXVECTOR3 vDist;


	FLOAT fRadius = fRangeMeter * fRangeMeter;

	FOR_LINKMAP(pObjCenter->GetWorld(), vPos, pObj, (int)(fRangeMeter / MPU), CObj::linkPlayer, GetMover()->GetLayer())
	{
		if (pObj->GetType() != OT_MOVER)
		{
			continue;
		}
		else
		{
			if (((CMover*)pObj)->IsPlayer() == FALSE)
				continue;
		}

		vDist = pObj->GetPos() - vPos;
		float fDistSq = D3DXVec3LengthSq(&vDist);
		if (fDistSq < fRadius)
		{
			if (!(((CMover*)pObj)->IsMode(TRANSPARENT_MODE)))
			{
				CMover* pMover = (CMover*)pObj;
				if (pMover->IsPlayer())
					if (((CUser*)pMover)->m_nOverHeal > 0)
						return pMover;
			}
		}
	}
	END_LINKMAP
#endif	// __CLIENT
		return NULL;
}


