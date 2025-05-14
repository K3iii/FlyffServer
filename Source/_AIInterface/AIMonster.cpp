#include "stdafx.h"
#include "AIMonster.h"
#include "lang.h"
#include "defineObj.h"
#include "dialogMsg.h"
#include "User.h"
#include "dpcoreclient.h"
#include "Party.h"
#include "DefineSkill.h"
#include "PathObj.h"



extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CPartyMng		g_PartyMng;

#define RANGE_CANCEL	120.0f
#define RANGE_MOVE	30.0f
#if __VER >= 9	//__AI_0509
#define	RANGE_RETURN_TO_BEGIN	120.0F
#endif	// __AI_0509

#if __VER >= 9	//__AI_0509
#define	TIMEGETTIME		g_tmCurrent
#else	// __AI_0509
#define	TIMEGETTIME		timeGetTime()
#endif	// __AI_0509

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_WANDER,
	STATE_PURSUE,
	STATE_EVADE,
	STATE_RUNAWAY,
	STATE_RAGE
	, STATE_STAND
	, STATE_PATROL
	, STATE_RAGE_PATROL
};

BEGIN_AISTATE_MAP(CAIMonster, CAIInterface)

	ON_STATE(STATE_INIT, &CAIMonster::StateInit)
	ON_STATE(STATE_IDLE, &CAIMonster::StateIdle)
	ON_STATE(STATE_WANDER, &CAIMonster::StateWander)
	ON_STATE(STATE_EVADE, &CAIMonster::StateEvade)
	ON_STATE(STATE_RAGE, &CAIMonster::StateRage)
	ON_STATE(STATE_RUNAWAY, &CAIMonster::StateRunaway)
	ON_STATE(STATE_STAND, &CAIMonster::StateStand)
	ON_STATE(STATE_PATROL, &CAIMonster::StatePatrol)
	ON_STATE(STATE_RAGE_PATROL, &CAIMonster::StateRagePatrol)

END_AISTATE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __INTERNALSERVER
const int INITIAL_AI_MONSTER = 4;
#else
const int INITIAL_AI_MONSTER = 1024 * 5;
#endif

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __VM_0819
MemPooler<CAIMonster>* CAIMonster::m_pPool = new MemPooler<CAIMonster>(INITIAL_AI_MONSTER, "CAIMonster");
#else	// __VM_0819
MemPooler<CAIMonster>* CAIMonster::m_pPool = new MemPooler<CAIMonster>(INITIAL_AI_MONSTER);
#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

#if __VER >= 9	// __JEFF_9
static DWORD s_tmAttack = SEC(15);
#else	// __JEFF_9
static DWORD s_tmAttack = SEC(20);
#endif	// __JEFF_9

CAIMonster::CAIMonster()
{
	Init();
}

CAIMonster::CAIMonster(CObj* pObj) : CAIInterface(pObj)
{
	Init();
}

void CAIMonster::Init()
{
	SetTarget(NULL_ID, 0);
	m_bReturnToBegin = FALSE;
#ifndef __FL_UPDATE_REAGGRO 
	m_bGoTarget = FALSE;
#endif
	m_vOldPos.x = m_vOldPos.y = m_vOldPos.z = 0;
	m_bTargetNoMovePos = FALSE;
	m_dwAtkMethod = 0;
	m_bFirstRunaway = FALSE;
	m_bCallHelper = FALSE;
	m_tmAttack = TIMEGETTIME + s_tmAttack;
	m_bRangeAttack = FALSE;
	m_tmSummon = TIMEGETTIME;
	m_tmHelp = 0;
	m_tmAttackDelay = m_tmMove = m_tmReturnToBegin = TIMEGETTIME;
	m_fAngleBegine = 0.0f;
#if __VER >= 9	//__AI_0509
	m_vPosDamage = D3DXVECTOR3(0, 0, 0);
#endif	// __AI_0509
}

CAIMonster::~CAIMonster()
{
}

void CAIMonster::InitAI()
{
	m_vPosBegin = GetMover()->GetPos();
	m_fAngleBegine = GetMover()->GetAngle();
	PostAIMsg(AIMSG_SETSTATE, GetMover()->m_dwAIInterfaceState);
}

void CAIMonster::SetStop(DWORD dwTime)
{
	m_tmMove = TIMEGETTIME + dwTime;
	GetMover()->SetStop();
}

BOOL CAIMonster::IsEndStop()
{
	return (TIMEGETTIME > m_tmMove) && (IsMove() == FALSE);
}

void CAIMonster::MoveToDst(const D3DXVECTOR3& vDst)
{
	CMover* pMover = GetMover();
	if (pMover->GetDestPos() == vDst)
		return;

	pMover->SetDestPos(vDst);
	g_UserMng.AddSetDestPos(pMover, vDst, 1);
}

void CAIMonster::MoveToDst(OBJID dwIdTarget)
{
	CMover* pMover = GetMover();
	if (pMover->GetDestId() == dwIdTarget && pMover->m_fArrivalRange == 0.0f)
		return;

	GetMover()->SetSpeedFactor(2.0F);

	CMover* pTarget = prj.GetMover(dwIdTarget);
	if (IsValidObj(pTarget))
		SetTarget(pTarget->GetId(), pTarget->m_idparty);

	pMover->SetDestObj(dwIdTarget, 0.0f, TRUE);
}

void CAIMonster::MoveToRandom(UINT nState)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	D3DXVECTOR3 vPos = pMover->GetPos();
	DWORD x1 = xRand();
	DWORD x2 = xRand();

	vPos.x += int(x1 % 21) - 10;
	vPos.z += int(x2 % 21) - 10;

	if (nState == STATE_IDLE)
	{
		D3DXVECTOR3 vDistant = vPos - m_vPosBegin;
		vDistant.y = 0.0f;

		float fRange = RANGE_MOVE;
		if (pMover->IsFlyingNPC())
			fRange *= 3.0f;

		if (IsInRange(vDistant, fRange))
		{
			MoveToDst(vPos);
		}
		else
		{

			if (m_bReturnToBegin)
				MoveToDst(vPos);
			else
			{

				vDistant = pMover->GetPos() - m_vPosBegin;
				if (IsInRange(vDistant, fRange) == FALSE)
					DoReturnToBegin();
			}
		}
	}
	else
	{
		MoveToDst(vPos);
	}
}

BOOL CAIMonster::IsInRange(D3DXVECTOR3& vDistant, FLOAT fRange)
{
	D3DXVECTOR3 vDist = vDistant;
	vDist.y = 0.0f;
	const float fDistSq = D3DXVec3LengthSq(&vDist);
	if (fDistSq > fRange * fRange)
		return FALSE;
	return TRUE;
}

BOOL CAIMonster::StateInit(const AIMSG& msg)
{
	return TRUE;
}

void CAIMonster::DoReturnToBegin(BOOL bReturn)
{
	if (bReturn)
	{
		m_bReturnToBegin = TRUE;
		m_tmReturnToBegin = TIMEGETTIME;
		SetTarget(NULL_ID, 0);
#if __VER >= 9	//__AI_0509
		CMover* pMover = GetMover();
		pMover->SetSpeedFactor(2.66F);
#endif	// __AI_0509
		MoveToDst(m_vPosBegin);
	}
	else
	{
		m_bReturnToBegin = FALSE;
		SetTarget(NULL_ID, 0);
#if __VER >= 9	//__AI_0509
		CMover* pMover = GetMover();
		pMover->SetPointParam(DST_HP, pMover->GetMaxHitPoint());
		pMover->RemoveAllEnemies();
#endif	// __AI_0509
	}
}

BOOL CAIMonster::MoveProcessIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (!pWorld)
		return FALSE;

	if (m_bReturnToBegin)
	{
		if (IsInRange(m_vPosBegin - pMover->GetPos(), 7.0f))
		{
			DoReturnToBegin(FALSE);
			SendAIMsg(AIMSG_ARRIVAL, NULL_ID);
		}

		if (TIMEGETTIME - m_tmReturnToBegin >= SEC(20))
		{
			pMover->REPLACE(g_uIdofMulti, pWorld->GetID(), m_vPosBegin, REPLACE_FORCE, pMover->GetLayer());
			m_tmReturnToBegin = TIMEGETTIME;
		}
	}
	else
	{
		if (pMover->GetMovePattern() == 0)
		{
			if (IsEndStop())
				MoveToRandom(STATE_IDLE);
		}
		else
		{
			if (pMover->GetMoveEvent() == 99)
				pMover->SetMovePattern(0);
		}


		if (pProp->m_nScanJob != -1)
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if (nAttackFirstRange > 60 || nAttackFirstRange <= 0)
			{
				Error("m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName());
				return TRUE;
			}


			if (pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao)
				pTarget = ScanTarget(pMover, nAttackFirstRange, pProp->m_nScanJob, pProp->m_dwScanQuestId, pProp->m_dwScanItemIdx, pProp->m_nScanChao);


			if (pTarget == NULL && GetMover()->m_bActiveAttack)//&& GetMover()->IsRegionAttr( RA_SAFETY ) == FALSE )
				pTarget = ScanTarget(pMover, nAttackFirstRange, JOB_ALL);

			if (pTarget)
			{
				if (IsValidObj((CObj*)pTarget))
				{
					if ((pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE))
					{
						if (pTarget)
						{
							g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_ATTACK);
							SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}

BOOL CAIMonster::StateIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		int nRangeMove = (int)(RANGE_MOVE);
	if (pMover->IsFlyingNPC())
		nRangeMove *= 3;

	D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
	if (IsInRange(vTargetTo, (float)(nRangeMove)) == FALSE)
		DoReturnToBegin();
	SetStop(SEC(0));

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		SetTarget(NULL_ID, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessIdle(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
		if (m_bReturnToBegin == FALSE)
		{
			MoveToDst(msg.dwParam1);
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1);
			m_tmAttack = TIMEGETTIME + s_tmAttack;
			m_vPosDamage = pMover->GetPos();
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

BOOL CAIMonster::StateWander(const AIMSG& msg)
{
	return TRUE;
}
BOOL CAIMonster::StateEvade(const AIMSG& msg)
{
	return TRUE;
}


BOOL CAIMonster::StateStand(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
	int nRangeMove = (int)(RANGE_MOVE);
	if (pMover->IsFlyingNPC())
		nRangeMove *= 3;
	if (IsInRange(vTargetTo, (float)(nRangeMove)) == FALSE)
		DoReturnToBegin();

	SetStop(SEC(0));

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		SetTarget(NULL_ID, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessStand(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
		if (m_bReturnToBegin == FALSE)
		{
			MoveToDst(msg.dwParam1);
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1);
			m_tmAttack = TIMEGETTIME + s_tmAttack;
#if __VER >= 9	//__AI_0509
			m_vPosDamage = pMover->GetPos();
#endif	// __AI_0509
		}


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)
		SendAIMsg(AIMSG_EXIT);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)
		//		MoveToRandom( STATE_IDLE );

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
			{
				SetStop(xRandom(SEC(1)));
			}
			else
			{
				SetStop(SEC(5) + xRandom(SEC(1)));
			}
		}

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_EXIT)

		EndAIHandler()
		return TRUE;
}
BOOL CAIMonster::StatePatrol(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		D3DXVECTOR3	vTargetTo = pMover->GetPos() - m_vPosBegin;
	int nRangeMove = (int)(RANGE_MOVE);
	if (pMover->IsFlyingNPC())
		nRangeMove *= 3;
	if (IsInRange(vTargetTo, (float)(nRangeMove)) == FALSE)
		DoReturnToBegin();
	SetStop(SEC(0));
	if (pMover->m_dwPatrolIndex != NULL_ID && m_bReturnToBegin == FALSE)
	{
		CPatrolPath::GetInstance()->GetNextPosInfo((CObj*)pMover, pMover->GetPos(), m_vDestPos, m_vecPatrolInfo);
		g_UserMng.AddMotionArrive(pMover, (m_vecPatrolInfo.m_dwDelay == 0) ? OBJMSG_STOP_RUN : OBJMSG_STOP);
		m_vPosBegin = m_vDestPos;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		SetTarget(NULL_ID, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessPatrol(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_TIMEOUT)


		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DAMAGE)
		if (m_bReturnToBegin == FALSE)
		{
			MoveToDst(msg.dwParam1);
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, msg.dwParam1);
			m_tmAttack = TIMEGETTIME + s_tmAttack;
#if __VER >= 9	//__AI_0509
			m_vPosDamage = pMover->GetPos();
#endif	// __AI_0509
		}


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)
		SendAIMsg(AIMSG_EXIT);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_ARRIVAL)
	{
		g_UserMng.AddMotionArrive(pMover, (m_vecPatrolInfo.m_dwDelay == 0) ? OBJMSG_STOP_RUN : OBJMSG_STOP);
		SetStop(SEC(m_vecPatrolInfo.m_dwDelay));
		D3DXVECTOR3 v3Pos = m_vDestPos;
		CPatrolPath::GetInstance()->GetNextPosInfo((CObj*)pMover, v3Pos, m_vDestPos, m_vecPatrolInfo);
		m_vPosBegin = m_vDestPos;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_EXIT)

		EndAIHandler()
		return TRUE;
}

BOOL CAIMonster::MoveProcessStand(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (!pWorld)
		return FALSE;

	if (m_bReturnToBegin)
	{
		if (IsInRange(m_vPosBegin - pMover->GetPos(), 0.1f))
		{
			DoReturnToBegin(FALSE);
			pMover->SetAngle(m_fAngleBegine);
			g_UserMng.AddMoverBehavior(pMover);
			SendAIMsg(AIMSG_ARRIVAL, NULL_ID);
		}

		if (TIMEGETTIME - m_tmReturnToBegin >= SEC(10))
		{
			pMover->REPLACE(g_uIdofMulti, pWorld->GetID(), m_vPosBegin, REPLACE_FORCE, pMover->GetLayer());
			m_tmReturnToBegin = TIMEGETTIME;
		}
	}
	else
	{
		if (pMover->GetMovePattern() == 0)
		{
		}
		else
		{
			if (pMover->GetMoveEvent() == 99)
				pMover->SetMovePattern(0);
		}

		if (pProp->m_nScanJob != -1)
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if (nAttackFirstRange > 60 || nAttackFirstRange <= 0)
			{
				Error("m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName());
				return TRUE;
			}


			if (pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao)
				pTarget = ScanTarget(pMover, nAttackFirstRange, pProp->m_nScanJob, pProp->m_dwScanQuestId, pProp->m_dwScanItemIdx, pProp->m_nScanChao);


			if (pTarget == NULL && GetMover()->m_bActiveAttack)
				pTarget = ScanTarget(pMover, nAttackFirstRange, JOB_ALL);

			if (pTarget)
			{
				if (IsValidObj((CObj*)pTarget))
				{

#ifdef __JHMA_VER_8_6
					if (pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly())
#else	// __VER >= 8
					if ((pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE))
#endif	// __VER >= 8
					{

						if (pTarget)
						{
							g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_ATTACK);
							SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}
BOOL CAIMonster::MoveProcessPatrol(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (!pWorld)
		return FALSE;


	if (m_bReturnToBegin)
	{
		if (IsInRange(m_vPosBegin - pMover->GetPos(), 3.0f))
		{
			DoReturnToBegin(FALSE);
		}

		if (TIMEGETTIME - m_tmReturnToBegin >= SEC(10))
		{
			pMover->REPLACE(g_uIdofMulti, pWorld->GetID(), m_vPosBegin, REPLACE_FORCE, pMover->GetLayer());
			m_tmReturnToBegin = TIMEGETTIME;
		}
	}
	else
	{
		if (pMover->GetMovePattern() == 0)
		{
			D3DXVECTOR3 vPos1 = pMover->GetPos();
			D3DXVECTOR3 vPos2 = m_vDestPos;

			if (IsEndStop())
			{
				MoveToDst(m_vDestPos);
			}
		}
		else
		{
			if (pMover->GetMoveEvent() == 99)
				pMover->SetMovePattern(0);
		}

		if (pProp->m_nScanJob != -1)
		{
			CMover* pTarget = NULL;
			int nAttackFirstRange = pProp->m_nAttackFirstRange;

			if (nAttackFirstRange > 60 || nAttackFirstRange <= 0)
			{
				Error("m_nAttackFirstRange 이상:%d %s\n", pProp->m_nAttackFirstRange, pMover->GetName());
				return TRUE;
			}


			if (pProp->m_nScanJob || pProp->m_dwScanQuestId || pProp->m_dwScanItemIdx || pProp->m_nScanChao)
				pTarget = ScanTarget(pMover, nAttackFirstRange, pProp->m_nScanJob, pProp->m_dwScanQuestId, pProp->m_dwScanItemIdx, pProp->m_nScanChao);


			if (pTarget == NULL && GetMover()->m_bActiveAttack)
				pTarget = ScanTarget(pMover, nAttackFirstRange, JOB_ALL);

			if (pTarget)
			{
				if (IsValidObj((CObj*)pTarget))
				{

#ifdef __JHMA_VER_8_6
					if (pMover->IsFlyingNPC() == FALSE || pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly())
#else	// __VER >= 8
					if ((pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE))
#endif	// __VER >= 8
					{
						if (pTarget)
						{
							g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_ATTACK);
							SendAIMsg(AIMSG_SETSTATE, STATE_RAGE_PATROL, pTarget->GetId());
						}
					}
				}
			}	// if( target)
		}	// if( scan )

	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_ARRAY 40

void CAIMonster::CallHelper(const MoverProp* pMoverProp)
{
	CMover* pMover = GetMover();
	D3DXVECTOR3 vPos = pMover->GetPos();
	int anCountMonster[5];
	int nCountPlayer = 0;

	ZeroMemory(anCountMonster, sizeof(anCountMonster));

	CMover* pTarget = NULL;
	CParty* pParty = NULL;
	BOOL bParty = FALSE;

	CObj* pObj;

	CMover* apMonster[MAX_ARRAY][5];
	CMover* apPlayer[MAX_ARRAY];

	/////////////////////////////////////////////////


	int nRange = 20;
	switch (pMoverProp->m_nHelpRangeMul)
	{
	case 0: nRange = pMoverProp->m_nAttackFirstRange / 2;	break;
	default: nRange = pMoverProp->m_nAttackFirstRange * pMoverProp->m_nHelpRangeMul;	break;
	}

	FOR_LINKMAP(pMover->GetWorld(), vPos, pObj, nRange / MPU, CObj::linkDynamic, pMover->GetLayer())
	{
		if (pObj != pMover && pObj->GetType() == OT_MOVER)
		{
			for (int i = 0; i < pMoverProp->m_nCallHelperMax; i++)
			{
				int nCallHelperIdx = pMoverProp->m_nCallHelperIdx[i];
				int nCallHelperNum = pMoverProp->m_nCallHelperNum[i];


				if ((pMoverProp->m_bHelpWho == 1 || (pMoverProp->m_bHelpWho == 2 && pObj->GetIndex() == pMover->GetIndex())) &&
					(anCountMonster[i] < nCallHelperNum || nCallHelperNum == 0))
				{
					apMonster[anCountMonster[i]][i] = (CMover*)pObj;
					anCountMonster[i]++;
					if (anCountMonster[i] >= MAX_ARRAY)
						goto NEXT;
				}
			}
		}
	}
	END_LINKMAP
		NEXT :

	pTarget = prj.GetMover(m_dwIdTarget);
	for (int i = 0; i < pMoverProp->m_nCallHelperMax; i++)
		bParty |= pMoverProp->m_bCallHelperParty[i] ? TRUE : FALSE;

	if (bParty && IsValidObj((CObj*)pTarget))
	{
		pParty = g_PartyMng.GetParty(pTarget->m_idparty);

		if (pParty)
		{
			FOR_LINKMAP(pMover->GetWorld(), vPos, pObj, 20 / MPU, CObj::linkPlayer, pMover->GetLayer())
			{
				if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && pParty->IsMember(((CMover*)pObj)->m_idPlayer))
				{
					apPlayer[nCountPlayer++] = (CMover*)pObj;
					if (nCountPlayer >= MAX_ARRAY)
						goto NEXT2;
				}
			}
			END_LINKMAP
		}
	}
NEXT2:
	for (int i = 0; i < pMoverProp->m_nCallHelperMax; i++)
	{
		if (pParty)
		{
			if (nCountPlayer)
			{
				for (int j = 0; j < anCountMonster[i]; j++)
				{

					CMover* pNewTarget = (CMover*)apPlayer[rand() % nCountPlayer];
					if (pNewTarget)
						apMonster[j][i]->PostAIMsg(AIMSG_SETSTATE, STATE_RAGE, pNewTarget->GetId());
				}
			}
		}
		else
		{
			for (int j = 0; j < anCountMonster[i]; j++)
				apMonster[j][i]->PostAIMsg(AIMSG_SETSTATE, STATE_RAGE, m_dwIdTarget);
		}
	}
}

DWORD CAIMonster::GetAtkMethod_Far()
{
	CMover* pMover = GetMover();
	DWORD dwRandom = xRandom(100);


	int nDeathblow = (int)(pMover->GetHitPointPercent() / 4.5f);
	if (nDeathblow)
	{
		if (!xRandom(nDeathblow))
			return 3;
	}

	switch (pMover->m_dwBelligerence)
	{
	case BELLI_MELEE2X:
		if (dwRandom < 50)
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if (dwRandom < 30)
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if (dwRandom < 20)
			return 1;
		return 2;
	}
	return 1;
}
DWORD CAIMonster::GetAtkMethod_Near()
{
	CMover* pMover = GetMover();
	DWORD dwRandom = xRandom(100);
	int nDeathblow = (int)(pMover->GetHitPointPercent() / 4.5f);
	if (nDeathblow)
	{
		if (!xRandom(nDeathblow))
			return 3;
	}

	switch (pMover->m_dwBelligerence)
	{
	case BELLI_MELEE2X:
		if (dwRandom < 60)
			return 1;
		return 2;
		break;
	case BELLI_MELEE:
		if (dwRandom < 80)
			return 1;
		return 2;
		break;
	case BELLI_RANGE:
		if (dwRandom < 50)
			return 1;
		return 2;
		break;
	}
	return 1;
}
DWORD CAIMonster::GetAtkRange(DWORD dwAtkMethod)
{
	ItemProp* pItemProp = GetAtkProp(dwAtkMethod);
#ifdef _DEBUG
	if (pItemProp == NULL)
	{
		Error("CAIMonster::GetAtkRange : ItemProp->dwAtk%d이 NULL이다. (%s)", dwAtkMethod, GetMover()->GetName());
	}
#endif
	return pItemProp->dwAttackRange;
}

ItemProp* CAIMonster::GetAtkProp(DWORD dwAtkMethod)
{
	return prj.GetItemProp(GetAtkItemId(dwAtkMethod));
}

DWORD CAIMonster::GetAtkItemId(DWORD dwAtkMethod)
{
	CMover* pMover = GetMover();
	switch (dwAtkMethod)
	{
	case 1:
		return pMover->GetProp()->dwAtk1;
	case 2:
		return pMover->GetProp()->dwAtk2;
	case 3:
		return pMover->GetProp()->dwAtk3;
	}
	return 0;
}

void CAIMonster::DoAttack(DWORD dwAtkMethod, CMover* pTarget)
{
	CMover* pMover = GetMover();
	if (IsInvalidObj(pMover) || dwAtkMethod == 0)
		return;

	DWORD dwAtkItem = GetAtkItemId(dwAtkMethod);
	ItemProp* pItemProp = prj.GetItemProp(dwAtkItem);
	if (pItemProp == NULL)
	{
		Error("CAIMonster::DoAttack : %s %d %d", pMover->GetName(), dwAtkMethod, dwAtkItem);
		return;
	}

	if (pItemProp->dwAttackRange == AR_RANGE || pItemProp->dwAttackRange == AR_WAND)
	{
		int id = pMover->m_sfxHitArray.Add(0, pTarget->GetId(), AF_GENERIC, 0);
		DWORD dwItemID = dwAtkItem;
		if (dwItemID == NULL_ID)
			Error("Attack item is broken %s", pMover->GetName());

		if (pMover->DoAttackRange(pTarget, dwItemID, id) != 1)
			pMover->m_sfxHitArray.RemoveSfxHit(id, FALSE);
	}
	else
	{
		DWORD dwItemID = dwAtkItem;
		if (dwItemID == NULL_ID)
			Error("Attack item is broken %s %d", pMover->GetName(), dwAtkMethod);
		if (dwAtkMethod == 3)
			pMover->DoAttackSP(pTarget, dwItemID);
		else
			pMover->DoAttackMelee(pTarget, OBJMSG_ATK1);
	}

	m_tmAttack = TIMEGETTIME + s_tmAttack;
}


BOOL CAIMonster::SubAttackChance(const AIMSG& msg, CMover* pTarget)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	BOOL bAttack = TRUE;
	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();

	if (pProp->m_bRecvCond)
	{
		if (pMover->GetHitPointPercent(100) <= pProp->m_nRecvCondMe)
		{
			if (pMover->GetManaPoint() > 0)
			{

				int nHP = (int)(pMover->GetMaxHitPoint() * (pProp->m_nRecvCondHow / 100.0f));
				pMover->IncHitPoint(nHP);

				g_UserMng.AddCreateSfxObj(pMover, XI_GEN_CURE01, pMover->GetPos().x, pMover->GetPos().y, pMover->GetPos().z);
				if (pProp->m_nRecvCondMP > 0)
				{

					int nMP = pProp->m_nRecvCondMP;
					pMover->IncManaPoint(-nMP);
				}
				else
				{

					int nMP = pMover->GetMaxManaPoint();
					pMover->IncManaPoint(-nMP);
				}
				bAttack = FALSE;
			}
		}
		m_tmAttackDelay = TIMEGETTIME + SEC(3);
	}

	if (bAttack)
	{
		if (m_bRangeAttack)
			m_dwAtkMethod = GetAtkMethod_Far();
		else
			m_dwAtkMethod = GetAtkMethod_Near();

		DWORD dwAtkRange = GetAtkRange(m_dwAtkMethod);

		if (dwAtkRange == AR_RANGE || dwAtkRange == AR_WAND)
		{

			FLOAT fRange = (float)(pProp->m_bRangeAttack & 0x7F);
			fRange += 1.0f;
			if (IsInRange(vTargetTo, fRange) == TRUE)
			{
				DoAttack(m_dwAtkMethod, (CMover*)pTarget);
				m_tmAttackDelay = TIMEGETTIME + SEC(3);
			}
		}
		else
		{
			FLOAT fRange = pMover->GetAttackRange(dwAtkRange);
			if (pMover->IsRangeObj(pTarget, fRange))
			{
				DoAttack(m_dwAtkMethod, pTarget);
#ifndef __FL_UPDATE_REAGGRO 
				m_bGoTarget = TRUE;
#endif
				int nDelay = (int)pProp->dwReAttackDelay - SEC(1) + xRandom(SEC(2));
				m_tmAttackDelay = TIMEGETTIME + nDelay;
			}
		}

	}

	return TRUE;
}






int CAIMonster::SelectAttackType(CMover* pTarget)
{
	int nAttackType = 0;

	CMover* pMover = GetMover();
	MoverProp* pProp = pMover->GetProp();

	int	nRange = pProp->m_bRangeAttack & 0x7F;

	BOOL bRange = (pProp->m_bRangeAttack) ? TRUE : FALSE;
	BOOL bMelee = (BOOL)pProp->m_bMeleeAttack;


	if (bRange && bMelee)
	{
		D3DXVECTOR3	vDist = pTarget->GetPos() - pMover->GetPos();
		FLOAT		fDistSq = D3DXVec3LengthSq(&vDist);

		int nHalfRange = nRange / 2;
		if (fDistSq < (nHalfRange * nHalfRange))
			nAttackType = 0;
		else
			nAttackType = 1;
	}
	else
		if (bRange && bMelee == FALSE)
		{
			nAttackType = 1;
		}
		else
			if (bRange == FALSE && bMelee)
			{
				nAttackType = 0;
			}

	return nAttackType;
}

void CAIMonster::SubSummonProcess(CMover* pTarget)
{
	if (TIMEGETTIME - m_tmSummon < SEC(10))
		return;

	m_tmSummon = TIMEGETTIME;

	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pProp && pProp->m_nSummProb)
	{
		for (vector<OBJID>::iterator it = m_vecIdSummon.begin(); it != m_vecIdSummon.end(); )
		{
			CMover* pZako = prj.GetMover((*it));
			if (IsValidObj(pZako) && pZako->IsLive())
				it++;
			else
				it = m_vecIdSummon.erase(it);
		}

		if (pProp->m_nSummNum > (int)(m_vecIdSummon.size()))
		{
			if ((int)(xRandom(100)) < pProp->m_nSummProb)
			{
				D3DXVECTOR3 vLocal, vPos;
				CObj* pObj = CreateObj(D3DDEVICE, OT_MOVER, (DWORD)pProp->m_nSummID);
				if (NULL == pObj)
				{
					Error("SubSummonProcess : 소환 실패 %d", pProp->m_nSummID);
					return;
				}
				vLocal.x = 1.5f - xRandomF(3.0f);
				vLocal.y = 0;
				vLocal.z = 1.5f - xRandomF(3.0f);
				vPos = pMover->GetPos();
				vPos += vLocal;
				pObj->SetPos(vPos);
				pObj->InitMotion(MTI_STAND);
				pObj->UpdateLocalMatrix();
				((CMover*)pObj)->m_bActiveAttack = TRUE;
				((CMover*)pObj)->AddItToGlobalId();
				if (pWorld->ADDOBJ(pObj, FALSE, pMover->GetLayer()) == TRUE)
				{
					m_vecIdSummon.push_back(((CMover*)pObj)->GetId());
					g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_SUMMON);
				}
				else
				{
					SAFE_DELETE(pObj);
				}
			}
		}
	}
}


BOOL CAIMonster::MoveProcessRage(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (!pWorld)
		return FALSE;

	CMover* pTarget = prj.GetMover(m_dwIdTarget);

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	if (IsValidObj(pTarget) == FALSE || ((CMover*)pTarget)->IsDie() || pTarget->GetWorld() != pMover->GetWorld() || pTarget->GetLayer() != pMover->GetLayer())
	{
		CMover* pTarget = NULL;
		if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pWorld->GetID()))
			pTarget = ScanTargetNext(pMover, 160, m_dwIdTarget, m_uParty);
		else
			pTarget = ScanTargetNext(pMover, 15, m_dwIdTarget, m_uParty);

		if (pTarget == NULL)
		{
			if (dwAIState == STATE_STAND)
				DoReturnToBegin();
			SendAIMsg(AIMSG_SETSTATE, dwAIState);
			return TRUE;
		}
		else
		{
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
			return TRUE;
		}
	}

	if (pTarget->IsRegionAttr(RA_SAFETY))
	{
		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}

	if (pMover->IsFlyingNPC() != ((CMover*)pTarget)->m_pActMover->IsFly())
	{
		if (dwAIState == STATE_STAND)
			DoReturnToBegin();

		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}

	if (TIMEGETTIME > m_tmAttack)
	{
		m_tmAttack = TIMEGETTIME + s_tmAttack;	

		D3DXVECTOR3 vDist = pMover->GetPos() - m_vOldPos;
		vDist.y = 0;	
		if (IsInRange(vDist, 2.0f))
		{
			if (pMover->GetSpeed(pMover->m_pActMover->m_fSpeed) > 0)
			{
				m_tmReturnToBegin = TIMEGETTIME;
				DoReturnToBegin();
				SendAIMsg(AIMSG_SETSTATE, dwAIState);
				return TRUE;
			}
		}

		m_vOldPos = pMover->GetPos();	
	}

	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();
	D3DXVECTOR3 v1 = pMover->GetPos() - m_vPosBegin;

	FLOAT fRange = RANGE_MOVE;
	if (pMover->IsFlyingNPC())
		fRange *= 3.0F;

	if (IsInRange(v1, RANGE_RETURN_TO_BEGIN + fRange) == FALSE && !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pWorld->GetID()))
	{
		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}
	if (m_vPosDamage == D3DXVECTOR3(0, 0, 0))
		m_vPosDamage = pMover->GetPos();

	D3DXVECTOR3 v2 = pMover->GetPos() - m_vPosDamage;
	if (IsInRange(v2, RANGE_RETURN_TO_BEGIN) == FALSE && !CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pWorld->GetID()))
	{
		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}
	else if (pTarget->GetType() == OT_MOVER && ((CMover*)pTarget)->IsMode(TRANSPARENT_MODE))
	{
		if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pWorld->GetID()))
		{
			CMover* pTarget = ScanTargetNext(pMover, 160, m_dwIdTarget, m_uParty);
			if (pTarget)
			{
				SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
				return TRUE;
			}
		}
		if (dwAIState == STATE_STAND)
			DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}
	else
	{
		m_bRangeAttack = SelectAttackType(pTarget);

		if (m_bRangeAttack)
		{
			FLOAT	fRange = (FLOAT)(pProp->m_bRangeAttack & 0x7F);
			BOOL	bKeepRange = (pProp->m_bRangeAttack & 0x80) ? TRUE : FALSE;
			FLOAT	fDist = D3DXVec3Length(&vTargetTo);
			if (bKeepRange)
			{
				if (fabs(fDist - fRange) < 1.0f)
				{
					if (pMover->IsEmptyDest() == FALSE)
						MoveToDst(pMover->GetPos());
				}
				else
				{

					if (fDist < fRange)
					{
						FLOAT fAngle = GetDegreeX(pMover->GetPos(), pTarget->GetPos());
						D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, 10);
						MoveToDst(vDst);
					}
					else
						MoveToDst(pTarget->GetId());
				}
			}
			else
			{
				if (fDist < fRange)
				{
					if (pMover->IsEmptyDest() == FALSE)
						MoveToDst(pMover->GetPos());
				}
				else
					MoveToDst(pTarget->GetId());
			}
		}
		else
		{
			if (!pMover->IsRangeObj(pTarget, 2.0f))
				MoveToDst(pTarget->GetId());
		}

		if (pProp->m_nSummProb)
			SubSummonProcess(pTarget);


		if (TIMEGETTIME > m_tmAttackDelay)
		{
			if (SubAttackChance(msg, pTarget) == FALSE)
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CAIMonster::MoveProcessRagePatrol(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (!pWorld)
		return FALSE;

	CMover* pTarget = prj.GetMover(m_dwIdTarget);

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	if (IsValidObj(pTarget) == FALSE)
	{
		DoReturnToBegin();	// SetTarget( NULL_ID, 0 );
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}

	if (((CMover*)pTarget)->IsDie())
	{
		DoReturnToBegin();	// SetTarget( NULL_ID, 0 );
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}

#ifdef __JHMA_VER_8_6
	if (pMover->IsFlyingNPC() == TRUE && ((CMover*)pTarget)->m_pActMover->IsFly() == FALSE)
#else	// __VER >= 8
	if (pMover->IsFlyingNPC() == FALSE && ((CMover*)pTarget)->m_pActMover->IsFly())
#endif	// __VER >= 8
	{
		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}

	if (TIMEGETTIME > m_tmAttack)	
	{
		m_tmAttack = TIMEGETTIME + s_tmAttack;	
		D3DXVECTOR3 vDist = pMover->GetPos() - m_vOldPos;
		vDist.y = 0;					
		if (IsInRange(vDist, 2.0f))	
		{
			if (pMover->GetSpeed(pMover->m_pActMover->m_fSpeed) > 0)	
			{
				m_tmReturnToBegin = TIMEGETTIME;
				DoReturnToBegin();
				SendAIMsg(AIMSG_SETSTATE, dwAIState);
				return TRUE;
			}
		}
		else
		{
			m_tmReturnToBegin = TIMEGETTIME;			
			DoReturnToBegin();
			SendAIMsg(AIMSG_SETSTATE, dwAIState);
			return TRUE;
		}
		m_vOldPos = pMover->GetPos();	
	}

	D3DXVECTOR3	vTargetTo = pTarget->GetPos() - pMover->GetPos();

	if (IsInRange(vTargetTo, RANGE_CANCEL) == FALSE)
	{
		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}
	else if (pTarget->GetType() == OT_MOVER && ((CMover*)pTarget)->IsMode(TRANSPARENT_MODE))
	{
#if __VER >= 14 // __INSTANCE_DUNGEON
		if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pWorld->GetID()))
		{
			CMover* pTarget = ScanTargetNext(pMover, 160, m_dwIdTarget, m_uParty);
			if (pTarget)
			{
				SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
				return TRUE;
			}
		}
#endif // __INSTANCE_DUNGEON

		DoReturnToBegin();
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
		return TRUE;
	}
	else
	{
		m_bRangeAttack = SelectAttackType(pTarget);

		if (m_bRangeAttack)
		{
			int nTargetJob = pTarget->GetJob();
			if (nTargetJob < 0)	nTargetJob = 0;
			if (nTargetJob >= MAX_JOB)
			{
				Error("CAIMonster::MoverProcessRage : 범위침범 %d", nTargetJob);
				nTargetJob = 0;
			}

			FLOAT	fRange = (FLOAT)(pProp->m_bRangeAttack & 0x7F);
			BOOL	bKeepRange = (pProp->m_bRangeAttack & 0x80) ? TRUE : FALSE;
			FLOAT	fDist = D3DXVec3Length(&vTargetTo);
			if (bKeepRange)
			{
				if (fabs(fDist - fRange) < 1.0f)
				{
					if (pMover->IsEmptyDest() == FALSE)
						MoveToDst(pMover->GetPos());
				}
				else
				{

					if (fDist < fRange)
					{
						FLOAT fAngle = GetDegreeX(pMover->GetPos(), pTarget->GetPos());
						D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, 10);
						MoveToDst(vDst);
					}
					else
						MoveToDst(pTarget->GetId());
				}
			}
			else
			{
				if (fDist < fRange)
				{
					if (pMover->IsEmptyDest() == FALSE)
						MoveToDst(pMover->GetPos());
				}
				else
					MoveToDst(pTarget->GetId());
			}
		}
		else
		{
			if (!pMover->IsRangeObj(pTarget, 2.0f))
				MoveToDst(pTarget->GetId());
		}

		if (pProp->m_nSummProb)
			SubSummonProcess(pTarget);


		if (TIMEGETTIME > m_tmAttackDelay)
		{
			if (SubAttackChance(msg, pTarget) == FALSE)
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CAIMonster::StateRagePatrol(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	if (IsInvalidObj(pMover))	return FALSE;

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		if (msg.dwParam2 != NULL_ID)
		{
			CMover* pTarget = prj.GetMover(msg.dwParam2);
			if (IsValidObj(pTarget))
				SetTarget(msg.dwParam2, pTarget->m_idparty);
		}
	m_tmAttack = TIMEGETTIME + s_tmAttack;
	m_tmAttackDelay = TIMEGETTIME;
	SetStop(0);


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
		SetTarget(NULL_ID, 0);
	SendAIMsg(AIMSG_SETSTATE, STATE_PATROL);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessRagePatrol(msg);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
#ifndef __FL_UPDATE_REAGGRO
		if (IsMove() && m_bGoTarget == FALSE)
		{
			if (xRandom(2) == 0)
			{
				MoveToDst(msg.dwParam1);
			}

			m_bGoTarget = TRUE;
		}
#else
		if (IsMove())
		{
			if (xRandom(4) == 0)
			{
				MoveToDst(msg.dwParam1);
			}
		}
#endif
	MoverProp* pMoverProp = pMover->GetProp();
	int nHPPercent = pMover->GetHitPointPercent(100);


	if (pMoverProp->m_bHelpWho)
	{
		if (TIMEGETTIME - m_tmHelp > pMoverProp->m_tmUnitHelp)
		{
			if (m_bCallHelper == FALSE)
			{
				CallHelper(pMoverProp);
				g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_HELPER);
				m_tmHelp = TIMEGETTIME;
				if (pMoverProp->m_tmUnitHelp == 0)
					m_bCallHelper = TRUE;
			}
		}
	}
	else
		m_bCallHelper = FALSE;

#if __VER >= 9	//__AI_0509
	m_vPosDamage = pMover->GetPos();
#endif	// __AI_0509

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DIE)

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DSTDIE)
		if (m_dwIdTarget == msg.dwParam1)
		{
#if __VER >= 9	//__AI_0509
			CMover* pTarget = ScanTargetNext(pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwIdTarget, m_uParty);
			if (pTarget)
			{
				SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
				return TRUE;
			}
			else
#endif	// __AI_0509
			{
				SetTarget(NULL_ID, 0);
			}
		}

	SendAIMsg(AIMSG_SETSTATE, STATE_PATROL);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_BEGINMOVE)
#ifndef __FL_UPDATE_REAGGRO
		m_bGoTarget = FALSE;
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_ARRIVAL)

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_EXIT)

		EndAIHandler()
		return TRUE;
}

BOOL CAIMonster::StateRage(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp* pProp = pMover->GetProp();
	if (IsInvalidObj(pMover))	return FALSE;

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	BeginAIHandler()

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_INIT)
		if (msg.dwParam2 != NULL_ID)
		{
			CMover* pTarget = prj.GetMover(msg.dwParam2);
			if (IsValidObj(pTarget))
				SetTarget(msg.dwParam2, pTarget->m_idparty);
		}
	m_tmAttack = TIMEGETTIME + s_tmAttack;
	m_tmAttackDelay = TIMEGETTIME;
	SetStop(0);


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_INIT_TARGETCLEAR)
#if __VER >= 9	//__AI_0509
		CMover* pTarget = ScanTargetNext(pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwIdTarget, m_uParty);
	if (pTarget)
	{
		SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
		return TRUE;
	}
	else
	{
		SetTarget(NULL_ID, 0);
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
	}
#else	// __AI_0509
		SetTarget(NULL_ID, 0);
	SendAIMsg(AIMSG_SETSTATE, dwAIState);
#endif	// __AI_0509

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
				MoveToDst(msg.dwParam1);
			}
			m_bGoTarget = TRUE;
		}
#else
		if (IsMove())
		{
			if (xRandom(4) == 0)
			{
				MoveToDst(msg.dwParam1);
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
		if (TIMEGETTIME - m_tmHelp > pMoverProp->m_tmUnitHelp)
		{
			if (m_bCallHelper == FALSE)
			{
				CallHelper(pMoverProp);
				g_UserMng.AddDlgEmoticon(pMover, DLGEMOT_HELPER);
				m_tmHelp = TIMEGETTIME;
				if (pMoverProp->m_tmUnitHelp == 0)
					m_bCallHelper = TRUE;
			}
		}
	}
	else
		m_bCallHelper = FALSE;

#if __VER >= 9	//__AI_0509
	m_vPosDamage = pMover->GetPos();
#endif	// __AI_0509

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)
		MoveToRandom(STATE_RAGE);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)

		/////////////////////////////////////////////////////////////////////////////////////////////
	//	OnMessage( AIMSG_DSTDAMAGE )

		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DSTDIE)
		if (m_dwIdTarget == msg.dwParam1)
		{
#if __VER >= 9	//__AI_0509
			CMover* pTarget = ScanTargetNext(pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwIdTarget, m_uParty);
			if (pTarget)
			{
				SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
				return TRUE;
			}
			else
#endif	// __AI_0509
			{
				SetTarget(NULL_ID, 0);
			}
		}
	SendAIMsg(AIMSG_SETSTATE, dwAIState);

	/////////////////////////////////////////////////////////////////////////////////////////////
//	OnMessage( AIMSG_NOMOVE )

//		MoveToRandom( STATE_RAGE );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_BEGINMOVE)
#ifndef __FL_UPDATE_REAGGRO
		m_bGoTarget = FALSE;
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_ARRIVAL)

		/////////////////////////////////////////////////////////////////////////////////////////////
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

BOOL CAIMonster::MoveProcessRunaway()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();


	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

	CCtrl* pTarget = prj.GetCtrl(m_dwIdTarget);


	if (IsValidObj(pTarget) == FALSE)
	{
		SetTarget(NULL_ID, 0);
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
	}
	else

		if (IsEndStop())
		{
			CMover* pMoverTarget = (CMover*)pTarget;
			DWORD dwAttackRange = pMoverTarget->GetActiveHandItemProp()->dwAttackRange;
			FLOAT fRange = pMoverTarget->GetAttackRange(dwAttackRange) * 1.7f;


			if (IsInRange(pTarget->GetPos() - pMover->GetPos(), fRange) == TRUE)
			{

				pMover->AddAngle((float)(20 - xRandom(40)));

				FLOAT fAngle = pMover->GetAngle();
				D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, fRange);
				MoveToDst(vDst);
			}
			if (m_bTargetNoMovePos == FALSE)
			{

				if (::TIMEGETTIME > pMover->m_dwTickRecovery)	// .
				{
					pMover->m_dwTickRecovery = ::TIMEGETTIME + NEXT_TICK_RECOVERY;
					pMover->IncHitPoint(pMover->GetHPRecovery());

					if (pMover->GetHitPointPercent(100) >= 50)
						SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, NULL_ID);
				}
			}
		}
	return TRUE;
}

BOOL CAIMonster::StateRunaway(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	DWORD dwAIState = pMover->m_dwAIInterfaceState;

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
		m_bFirstRunaway = TRUE;
	}
	else
	{
		SendAIMsg(AIMSG_SETSTATE, dwAIState);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_PROCESS)
		MoveProcessRunaway();
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DAMAGE)
		if (m_bTargetNoMovePos == TRUE && m_bFirstRunaway == FALSE)
			SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, NULL_ID);

#if __VER >= 9	//__AI_0509
	m_vPosDamage = pMover->GetPos();
#endif	// __AI_0509

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_COLLISION)


		pMover->AddAngle((float)(180 + (20 - xRandom(40))));

	FLOAT fAngle = pMover->GetAngle();
	D3DXVECTOR3 vDst = pMover->GetPos() + VelocityToVec(fAngle, 5);
	MoveToDst(vDst);

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_DIE)
		/////////////////////////////////////////////////////////////////////////////////////////////
	//	OnMessage( AIMSG_DSTDAMAGE )
		/////////////////////////////////////////////////////////////////////////////////////////////
		OnMessage(AIMSG_DSTDIE)
		if (m_dwIdTarget == msg.dwParam1)
		{
#if __VER >= 9	//__AI_0509
			CMover* pTarget = ScanTargetNext(pMover, /*pProp->m_nAttackFirstRange*/ 15, m_dwIdTarget, m_uParty);
			if (pTarget)
			{
				SendAIMsg(AIMSG_SETSTATE, STATE_RAGE, pTarget->GetId());
				return TRUE;
			}
			else
#endif	// __AI_0509
			{
				SetTarget(NULL_ID, 0);
			}
		}
	SendAIMsg(AIMSG_SETSTATE, dwAIState);

	OnMessage(AIMSG_ARRIVAL)
		m_bFirstRunaway = FALSE;
	MoverProp* pMoverProp = pMover->GetProp();
	if (pMoverProp->m_dwRunawayDelay)
		SetStop(pMover->GetProp()->m_dwRunawayDelay + xRandom(SEC(1)));

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage(AIMSG_EXIT)

		EndAIHandler()

		return TRUE;
}

void	CAIMonster::SetTarget(OBJID dwIdTarget, u_long uParty)
{
	m_dwIdTarget = dwIdTarget;
#if __VER >= 9	//__AI_0509
	m_uParty = uParty;
	if (dwIdTarget == NULL_ID)
		GetMover()->SetSpeedFactor(1.0F);
	else
	{
		CMover* pTarget = prj.GetMover(dwIdTarget);
		if (IsValidObj(pTarget))
		{
			pTarget->AddEnemy(GetMover()->GetId(), 0);
			GetMover()->AddEnemy(pTarget->GetId(), 0);
		}
	}
#endif	// __AI_0509
}


#if __VER >= 12 // __MONSTER_SKILL
CMonsterSkill::CMonsterSkill()
{

}


CMonsterSkill::~CMonsterSkill()
{
	Clear();
}

void CMonsterSkill::Clear()
{
	for (MAPMONSTERSKILL::iterator it = m_mapMonsterSkill.begin(); it != m_mapMonsterSkill.end(); it++)
		it->second.clear();

	m_mapMonsterSkill.clear();
}


CMonsterSkill* CMonsterSkill::GetInstance(void)
{
	static CMonsterSkill sMonsterSkill;
	return &sMonsterSkill;
}

void CMonsterSkill::LoadScript()
{
	CLuaBase Lua;

	if (Lua.RunScript("MonsterSkill.lua") != 0)
	{
		Error("MonsterSkill.lua Run Failed!!!");
		exit(0);
	}

	Lua.GetGloabal("tMonsterSkill");
	Lua.PushNil();
	while (Lua.TableLoop(-2))
	{
		DWORD dwMonsterId = CScript::GetDefineNum(Lua.GetFieldToString(-1, "strMonsterId"));
		vector<__MONSTERSKILL> vecMonsterSkill;

		//lua_getfield( Lua.GetLuaState(), -1, "tSkill" );
		Lua.GetField(-1, "tSkill");
		Lua.PushNil();
		while (Lua.TableLoop(-2))
		{
			__MONSTERSKILL monsterSkill;
			monsterSkill.nAtkMethod = static_cast<int>(Lua.GetFieldToNumber(-1, "nAtkMethod"));
			monsterSkill.dwSkillID = CScript::GetDefineNum(Lua.GetFieldToString(-1, "strSkillId"));
			monsterSkill.dwSkillLV = static_cast<DWORD>(Lua.GetFieldToNumber(-1, "dwSkillLv"));
			monsterSkill.nHitCount = static_cast<int>(Lua.GetFieldToNumber(-1, "nHitCount"));
			monsterSkill.nRange = static_cast<int>(Lua.GetFieldToNumber(-1, "nRange"));
			monsterSkill.nApplyProbabilty = static_cast<int>(Lua.GetFieldToNumber(-1, "nProb"));
			monsterSkill.dwSkillTime = static_cast<DWORD>(Lua.GetFieldToNumber(-1, "dwSkillTime"));
			monsterSkill.bIgnoreSkillProb = Lua.GetFieldToBool(-1, "bIgnore") ? true : false;

			ItemProp* pSkillProp = prj.GetSkillProp(monsterSkill.dwSkillID);
			if (!pSkillProp || monsterSkill.dwSkillLV < 1 || monsterSkill.dwSkillLV > pSkillProp->dwExpertMax)
			{
				Error("Out of Skill Level Range -> CMonsterSkill::LoadScript() - %d, %s, %d", dwMonsterId, Lua.GetFieldToString(-1, "strSkillId"), monsterSkill.dwSkillLV);
				ASSERT(0);
			}

			vecMonsterSkill.push_back(monsterSkill);
			Lua.Pop(1);
		}

		m_mapMonsterSkill.insert(make_pair(dwMonsterId, vecMonsterSkill));
		Lua.Pop(2);
	}

	Lua.Pop(0);
}

DWORD CMonsterSkill::GetMonsterSkillLevel(CMover* pAttacker, DWORD dwSkillId)
{
	MAPMONSTERSKILL::iterator it = m_mapMonsterSkill.find(pAttacker->m_dwIndex);
	if (it == m_mapMonsterSkill.end())
		return 0;

	vector<__MONSTERSKILL>& vecMonsterSkill = it->second;
	for (int i = 0; i < (int)(vecMonsterSkill.size()); i++)
	{
		if (vecMonsterSkill[i].dwSkillID == dwSkillId)
			return vecMonsterSkill[i].dwSkillLV;
	}

	return 0;
}

BOOL CMonsterSkill::ApplySkill(CMover* pAttacker, CMover* pTarget, DWORD dwAtkMethod)
{
	if (!IsValidObj(pAttacker) || !IsValidObj(pTarget))
		return FALSE;

	MAPMONSTERSKILL::iterator it = m_mapMonsterSkill.find(pAttacker->m_dwIndex);
	if (it == m_mapMonsterSkill.end())
		return FALSE;

	vector<__MONSTERSKILL>& vecMonsterSkill = it->second;
	//	vecMonsterSkill.assign( it->second.begin(), it->second.end() );


	BOOL bSuccess = FALSE;
	int nTotalProb = 0;
	int nProb = xRandom(100);
	int	i = 0;
	for (i = 0; i < (int)(vecMonsterSkill.size()); i++)
	{
		if (vecMonsterSkill[i].nAtkMethod != dwAtkMethod)
			continue;

		nTotalProb += vecMonsterSkill[i].nApplyProbabilty;
		if (nProb < nTotalProb)
		{
			bSuccess = TRUE;
			break;
		}
	}

	if (!bSuccess)
		return FALSE;


	ItemProp* pSkillProp = prj.GetSkillProp(vecMonsterSkill[i].dwSkillID);
	if (!pSkillProp) return FALSE;
	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine + vecMonsterSkill[i].dwSkillLV - 1);
	if (!pAddSkillProp) return FALSE;

	if (pSkillProp->dwID == SI_MAG_FIRE_HOTAIR)
		return FALSE;

	if (vecMonsterSkill[i].nHitCount != pAttacker->m_pActMover->m_nMotionHitCount)
		return FALSE;


	DWORD dwReferTarget1Backup = pSkillProp->dwReferTarget1;
	DWORD dwReferTarget2Backup = pSkillProp->dwReferTarget2;
	DWORD dwSkillTimeBackup = pAddSkillProp->dwSkillTime;
	pSkillProp->dwReferTarget1 = NULL_ID;
	pSkillProp->dwReferTarget2 = NULL_ID;
	pAddSkillProp->dwSkillTime = vecMonsterSkill[i].dwSkillTime;


	if (vecMonsterSkill[i].nRange == 0)
	{
		pTarget->ApplySkill(pAttacker, pSkillProp, pAddSkillProp, vecMonsterSkill[i].bIgnoreSkillProb);
		if (pSkillProp->dwSfxObj2 != NULL_ID)
			g_UserMng.AddCreateSfxObj(pTarget, pSkillProp->dwSfxObj2);
		g_UserMng.AddCreateSfxObj(pTarget, pSkillProp->dwSfxObj3);
	}
	else
	{
		CObj* pObj = NULL;
		FOR_LINKMAP(pAttacker->GetWorld(), pAttacker->GetPos(), pObj, vecMonsterSkill[i].nRange, CObj::linkPlayer, pAttacker->GetLayer())
		{
			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && ((CMover*)pObj)->IsLive()
				&& pObj != pAttacker && pObj->IsRangeObj(pAttacker, (float)(vecMonsterSkill[i].nRange)))
			{
				((CMover*)pObj)->ApplySkill(pAttacker, pSkillProp, pAddSkillProp, vecMonsterSkill[i].bIgnoreSkillProb);
				if (pSkillProp->dwSfxObj2 != NULL_ID)
					g_UserMng.AddCreateSfxObj((CMover*)pObj, pSkillProp->dwSfxObj2);
				g_UserMng.AddCreateSfxObj((CMover*)pObj, pSkillProp->dwSfxObj3);
			}
		}
		END_LINKMAP
	}


	pSkillProp->dwReferTarget1 = dwReferTarget1Backup;
	pSkillProp->dwReferTarget2 = dwReferTarget2Backup;
	pAddSkillProp->dwSkillTime = dwSkillTimeBackup;

	return TRUE;
}

#if __VER >= 14 // __INSTANCE_DUNGEON
BOOL CMonsterSkill::MonsterTransform(CMover* pMover, int nMoverHP)
{
	if (!IsValidObj(pMover) || !pMover->IsNPC() || pMover->GetProp()->m_MonsterTransform.dwMonsterId == NULL_ID)
		return FALSE;

	if (pMover->GetProp()->m_MonsterTransform.fHPRate < MulDiv(nMoverHP, 100, pMover->GetMaxHitPoint()))
		return FALSE;

	CWorld* pWorld = pMover->GetWorld();
	MoverProp* pMoverProp = prj.GetMoverProp(pMover->GetProp()->m_MonsterTransform.dwMonsterId);

	if (pWorld && pMoverProp)
	{
		CObj* pObj = CreateObj(D3DDEVICE, OT_MOVER, pMoverProp->dwID);
		if (NULL == pObj)
			return FALSE;
		pObj->SetPos(pMover->GetPos());
		pObj->InitMotion(MTI_STAND);
		pObj->UpdateLocalMatrix();
		((CMover*)pObj)->m_bActiveAttack = TRUE;
		((CMover*)pObj)->AddItToGlobalId();
		if (pWorld->ADDOBJ(pObj, FALSE, pMover->GetLayer()) == TRUE)
		{
			g_UserMng.AddCreateSfxObj(pMover, XI_SKILL_MAG_EARTH_LOOTING01);

			if (pMover->IsLive() && !pMover->IsDelete())
				pMover->Delete();
		}
		else
		{
			SAFE_DELETE(pObj);
			return FALSE;
		}
	}
	else
		return FALSE;

	return TRUE;
}
#endif // __INSTANCE_DUNGEON

#endif // __MONSTER_SKILL