#include "stdafx.h"

#include "AIGuard.h"
#include "lang.h"
#include "defineObj.h"
#include "dialogMsg.h"
#include "User.h"
#include "dpcoreclient.h"
#include "Party.h"

extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CPartyMng		g_PartyMng;


#define RANGE_MOVE   15.0f

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RUNAWAY,
	STATE_RAGE
};
BEGIN_AISTATE_MAP(CAIGuard, CAIInterface)

	ON_STATE(STATE_INIT, &CAIGuard::StateInit)
	ON_STATE(STATE_IDLE, &CAIGuard::StateIdle)
	ON_STATE(STATE_WANDER, &CAIGuard::StateWander)
	ON_STATE(STATE_EVADE, &CAIGuard::StateEvade)
	ON_STATE(STATE_RAGE, &CAIGuard::StateRage)
	ON_STATE(STATE_RUNAWAY, &CAIGuard::StateRunaway)

END_AISTATE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __VM_0819
MemPooler<CAIGuard>* CAIGuard::m_pPool = new MemPooler<CAIGuard>(16, "CAIGuard");
#else	// __VM_0819
MemPooler<CAIGuard>* CAIGuard::m_pPool = new MemPooler<CAIGuard>(16);
#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

static DWORD s_tmAttack = SEC(10);

CAIGuard::CAIGuard()
{
}
CAIGuard::CAIGuard(CObj* pObj) : CAIMonster(pObj)
{
}

CAIGuard::~CAIGuard()
{
}

void CAIGuard::InitAI()
{

}


BOOL CAIGuard::StateInit(const AIMSG& msg)
{
	return TRUE;
}

BOOL CAIGuard::MoveProcessIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();


	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;



	{




		if (GetMover()->m_bActiveAttack)
		{
			int nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;

			if (pMover->GetProp()->m_nAttackFirstRange > 60 || pMover->GetProp()->m_nAttackFirstRange <= 0)
			{
				Error("m_nAttackFirstRange ÀÌ»ף:%d %s\n", pMover->GetProp()->m_nAttackFirstRange, pMover->GetName());
				return TRUE;
			}

			MoverProp* pProp = GetMover()->GetProp();
			CMover* pTarget = ScanTarget(pMover, nAttackFirstRange, pProp->m_nScanJob);
			if (IsValidObj((CObj*)pTarget))
			{

				if (pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE)
				{
					m_dwIdTarget = pTarget->GetId();

					if (m_dwIdTarget != NULL_ID)
					{
						g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_ATTACK);
						SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, NULL_ID);
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CAIGuard::StateIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		float fRangeMove = RANGE_MOVE;
	if (pMover->IsFlyingNPC())
		fRangeMove *= 3.0f;
	D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
	if (IsInRange(vTargetTo, fRangeMove) == FALSE)
		m_bReturnToBegin = TRUE;
	SetStop(SEC(0));
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		m_dwIdTarget = NULL_ID;

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessIdle(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
		if (m_bReturnToBegin == FALSE || (m_bReturnToBegin && (int)(g_tmCurrent - m_tmReturnToBegin) > SEC(5)))
		{
			m_dwIdTarget = msg.dwParam1;
			MoveToDst(m_dwIdTarget);
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, NULL_ID);
			m_tmAttack = timeGetTime() + s_tmAttack;
		}


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)
		SendAIMsg(AIMSG_EXIT);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)
		MoveToRandom(STATE_IDLE);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_ARRIVAL)
		if (pMover->IsFlyingNPC())
		{

			if (msg.dwParam2 == 1)
			{
				int nPattern = 2;
				pMover->SetMovePattern(nPattern);

				g_UserMng.AddSetMovePattern(pMover, nPattern, pMover->GetPos(), pMover->GetAngle(), pMover->GetAngleX());
			}
		}
		else
		{
			if (m_bReturnToBegin)
				SetStop(xRandom(SEC(1)));
			else
				SetStop(SEC(5) + xRandom(SEC(1)));
		}

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_EXIT)

		EndAIHandler()
		return TRUE;
}

BOOL CAIGuard::StateWander(const AIMSG& msg)
{
	return TRUE;
}
BOOL CAIGuard::StateEvade(const AIMSG& msg)
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////




BOOL CAIGuard::MoveProcessRage(const AIMSG& msg)
{
	CAIMonster::MoveProcessRage(msg);
	return TRUE;
}

BOOL CAIGuard::StateRage(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if (IsInvalidObj(pMover))	return FALSE;

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		if (msg.dwParam2 != NULL_ID)
			m_dwIdTarget = msg.dwParam2;
	m_tmAttack = timeGetTime() + s_tmAttack;
	m_tmAttackDelay = timeGetTime();
	SetStop(0);


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		m_dwIdTarget = NULL_ID;
	SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessRage(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
#ifndef __FL_UPDATE_REAGGRO
		if (IsMove() && m_bGoTarget == FALSE)
		{
			if (xRandom(2) == 0)
			{
				m_dwIdTarget = msg.dwParam1;

				MoveToDst(m_dwIdTarget);
			}
			m_bGoTarget = TRUE;
		}
#else
		if (IsMove())
		{
			if (xRandom(4) == 0)
			{
				m_dwIdTarget = msg.dwParam1;

				MoveToDst(m_dwIdTarget);
			}
		}
#endif

	MoverProp* pMoverProp = pMover->GetProp();
	int nHPPercent = pMover->GetHitPointPercent(100);

	if (nHPPercent < pMoverProp->m_nRunawayHP)
	{

		SendAIMsg(AIMSG_SETSTATE, STATE_RUNAWAY);
		g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_EVADE);
	}

	if (pMoverProp->m_bHelpWho)
	{
		if (timeGetTime() - m_tmHelp > pMoverProp->m_tmUnitHelp)
		{
			if (m_bCallHelper == FALSE)
			{
				CallHelper(pMoverProp);
				g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_HELPER);
				m_tmHelp = timeGetTime();
				if (pMoverProp->m_tmUnitHelp == 0)
					m_bCallHelper = TRUE;
			}
		}
	}
	else
		m_bCallHelper = FALSE;
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)
		MoveToRandom(STATE_RAGE);
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DSTDIE)
		if (m_dwIdTarget == msg.dwParam1)
			m_dwIdTarget = NULL_ID;
	TRACE("TARGET CLEARED\n");
	SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);

	OnMessage(AIMSG_BEGINMOVE)
#ifndef __FL_UPDATE_REAGGRO 
		m_bGoTarget = FALSE;
#endif

	OnMessage(AIMSG_ARRIVAL)

		OnMessage(AIMSG_EXIT)

		EndAIHandler()

		return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CAIGuard::MoveProcessRunaway()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();


	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	CCtrl* pTarget = prj.GetCtrl(m_dwIdTarget);


	if (IsValidObj(pTarget) == FALSE)
	{
		m_dwIdTarget = NULL_ID;
		SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);
	}
	return TRUE;
}

BOOL CAIGuard::StateRunaway(const AIMSG& msg)
{

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)


		m_bTargetNoMovePos = msg.dwParam2;


	pMover->AddAngle((float)(180 + (20 - xRandom(40))));

	CMover* pMoverTarget = (CMover*)prj.GetCtrl(m_dwIdTarget);
	if (IsValidObj(pMoverTarget))
	{
		DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
		FLOAT fRange = pMoverTarget->GetAttackRange(dwAttackRange) * 1.7f;
		FLOAT fAngle = pMover->GetAngle();
		D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, fRange);
		MoveToDst(vDst);
		//			SendAIMsg( AIMSG_SETPROCESS, TRUE );
		m_bFirstRunaway = TRUE;
	}
	else
		SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessRunaway();
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
		if (m_bTargetNoMovePos == TRUE && m_bFirstRunaway == FALSE)
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, NULL_ID);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)


		pMover->AddAngle((float)(180 + (20 - xRandom(40))));

	FLOAT fAngle = pMover->GetAngle();
	D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, 5);
	MoveToDst(vDst);


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)
		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DSTDIE)
		if (m_dwIdTarget == msg.dwParam1)
			m_dwIdTarget = NULL_ID;
	SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);

	OnMessage(AIMSG_ARRIVAL)
		m_bFirstRunaway = FALSE;

	MoverProp* pMoverProp = pMover->GetProp();
	if (pMoverProp->m_dwRunawayDelay)
		SetStop(pMover->GetProp()->m_dwRunawayDelay + xRandom(SEC(1)));

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_EXIT)
		//		SendAIMsg( AIMSG_SETPROCESS, FALSE );

		EndAIHandler()

		return TRUE;
}

