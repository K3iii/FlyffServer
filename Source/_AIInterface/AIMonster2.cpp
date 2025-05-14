
// g_UserMng.AddDlgEmoticon( pMover, DLGEMOT_EVADE );
// AIMSG_INIT_TARGETCLEAR

#include "stdafx.h"
#include "AIMonster2.h"
#include "FSM.h"
#include "FSMstate.h"
#include "User.h"
#include "definetext.h"

extern	CUserMng		g_UserMng;

#define		CAT_NONE					0
#define		CAT_NORMAL					1
#define		CAT_NORMAL2					2
#define		CAT_QUAKE_ONE				3
#define		CAT_QUAKEDOUBLE				4

///////////////////////////////////////////////////////////////////
// AI script related
///////////////////////////////////////////////////////////////////

static map< string, int > g_stringTable;

int GetAIConst(const char* szString)
{
	map< string, int >::iterator it = g_stringTable.find(szString);
	if (g_stringTable.end() != it)
		return it->second;
	ASSERT(0);
	return 0;
}

enum READ_TYPE
{
	READ_TOKEN0,
	READ_TOKEN1,
	READ_TOKEN2,
	READ_TOKEN3,
	READ_COMMENT = 100,
	READ_TYPE_FILE_ERROR,
	READ_TYPE_FILE_EOF,
};

READ_TYPE ReadAILine(FILE* fp, char szTokens[3][64])
{
	szTokens[0][0] = '\0';
	szTokens[1][0] = '\0';
	szTokens[2][0] = '\0';

	char szBuffer[128];
	if (fgets(szBuffer, sizeof(szBuffer), fp) == NULL)
	{
		if (ferror(fp))
			return READ_TYPE_FILE_ERROR;
		else
			return READ_TYPE_FILE_EOF;
	}

	int n = sscanf(szBuffer, "%s %s %s", szTokens[0], szTokens[1], szTokens[2]);
	if (szTokens[0][0] == '/' && szTokens[0][1] == '/')
		return READ_COMMENT;

	//	printf("%s %s\n", szTokens[0], szTokens[1] );
	return (READ_TYPE)n;
}

BOOL ReadAIScriptHelper(FILE* fp)
{
	READ_TYPE type;
	char szTokens[3][64];
	string strState;
	int nAI, nState;
	int nBlock = 0;
	CFSMstate* pState = NULL;

	for (;; )
	{
		type = ReadAILine(fp, szTokens);
		switch (type)
		{
		case READ_TYPE_FILE_EOF:
			return TRUE;
		case READ_TYPE_FILE_ERROR:
			return FALSE;
		case READ_TOKEN0:
		case READ_COMMENT:
			continue;
		case READ_TOKEN1:
			if (szTokens[0][0] == '{')
				++nBlock;
			else if (szTokens[0][0] == '}')
				--nBlock;
			break;
		case READ_TOKEN2:
			if (nBlock == 0)
			{
				if (memcmp("TYPE", szTokens[0], 4) == 0)
				{
					nAI = atoi(szTokens[1]);
					//printf("type:%d\n", nAI);
				}
			}
			else if (nBlock == 1)
			{
				nState = GetAIConst(szTokens[0]);
				pState = new CFSMstate(nState);

				int nPolling = atoi(szTokens[1]);
				pState->SetPolling(nPolling);
				CFSM* pFSM = GetFSM(nAI); // nAI = FSM_TYPE_0
				pFSM->AddState(pState);

				//printf("state:%s(%d)\n", szTokens[0], nState );
			}
			break;
		case READ_TOKEN3:
			if (nBlock == 2)
			{
				// AIMSG_TIMEOUT 1000 AI2_SEARCH
				pState->AddTransition(GetAIConst(szTokens[0]),
					GetAIConst(szTokens[2]),
					atoi(szTokens[1]));
			}
			break;
		}
	}

	return TRUE;
}



void InitAIConst()
{
	g_stringTable["AI2_IDLE"] = 0;
	g_stringTable["AI2_MOVE"] = 1;
	g_stringTable["AI2_RAGE"] = 2;
	g_stringTable["AI2_SEARCH"] = 3;
	g_stringTable["AI2_TRACKING"] = 4;
	g_stringTable["AI2_ATTACK"] = 5;

	g_stringTable["AIMSG_INIT"] = 1;
	g_stringTable["AIMSG_PROCESS"] = 2;
	g_stringTable["AIMSG_TIMEOUT"] = 3;
	g_stringTable["AIMSG_SETSTATE"] = 4;
	g_stringTable["AIMSG_SETPROCESS"] = 5;
	g_stringTable["AIMSG_DSTDAMAGE"] = 6;
	g_stringTable["AIMSG_DSTDIE"] = 7;
	g_stringTable["AIMSG_DAMAGE"] = 8;
	g_stringTable["AIMSG_DIE"] = 9;
	g_stringTable["AIMSG_COLLISION"] = 10;
	g_stringTable["AIMSG_ARRIVAL"] = 11;
	g_stringTable["AIMSG_NOMOVE"] = 12;
	g_stringTable["AIMSG_BEGINMOVE"] = 13;
	g_stringTable["AIMSG_ATTACK_MELEE"] = 14;
	g_stringTable["AIMSG_END_MELEEATTACK"] = 15;
	g_stringTable["AIMSG_END_APPEAR"] = 16;
	g_stringTable["AIMSG_INIT_TARGETCLEAR"] = 17;
	g_stringTable["AIMSG_EXIT"] = 18;
	g_stringTable["AIMSG_TARGET_FIND"] = 19;
	g_stringTable["AIMSG_REATTACK_TIMEOUT"] = 20;
	g_stringTable["AIMSG_INVALID_TARGET"] = 21;
}

BOOL ReadAIScript()
{
	InitAIConst();

	const char szFile[] = ".\\AI_Script.txt";
	FILE* fp = fopen(szFile, "rt");
	if (fp == NULL)
	{
		char szMsg[80];
		sprintf(szMsg, "%s file not found.", szFile);
		AfxMessageBox(szFile);
		return FALSE;
	}

	BOOL bResult = ReadAIScriptHelper(fp);
	if (bResult == FALSE)
	{
		char szMsg[80];
		sprintf(szMsg, "%s file syntax error.", szFile);
		AfxMessageBox(szFile);
	}

	fclose(fp);
	return bResult;
}

BOOL LoadAIScript()
{
#ifdef __INTERNALSERVER
	return ReadAIScript();
#else
	return TRUE;
#endif// __INTERNALSERVER
}

/////////////////////////////////////////////////////////////
CAIMonster2::CAIMonster2()
{
	Init();
}

CAIMonster2::CAIMonster2(CObj* pObj) : CAIInterface(pObj)
{
	Init();
}

CAIMonster2::~CAIMonster2()
{
}

void CAIMonster2::Init()
{
	m_dwFsmType = FSM_TYPE_0;
	m_nState = AI2_IDLE;
	m_dwTick = ::GetTickCount();

	m_vPosBegin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	DWORD dwCur = GetTickCount();

	m_idLastAttacker = NULL_ID;
	m_dwReattack = dwCur;
	m_idTarget = NULL_ID;
	m_vTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nAttackType = CAT_NORMAL;
	m_fAttackRange = 5.0f;
}


void CAIMonster2::InitAI()
{
	CMover* pMover = GetMover();
	MoverProp* pProperty = pMover->GetProp();
	ASSERT(pProperty);
	if (pProperty->dwAI >= AII_VER2_TYPE0)
		m_dwFsmType = pProperty->dwAI - AII_VER2_TYPE0;

	m_vPosBegin = pMover->GetPos();
	m_fAttackRange = pMover->GetRadiusXZ();
}


BOOL CAIMonster2::IsControllable()
{
	CMover* pMover = GetMover();

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;
	return TRUE;
}


BOOL CAIMonster2::Check(int nInput, DWORD dwCurTick, DWORD dwValue)
{
	switch (nInput)
	{
	case AIMSG_TIMEOUT:
		if (dwCurTick > m_dwTick + dwValue)
			return TRUE;
		break;
	case AIMSG_TARGET_FIND:
		if (Search())
			return TRUE;
		break;
	case AIMSG_REATTACK_TIMEOUT:
		if (dwCurTick > m_dwReattack)
			return TRUE;
		break;
	case AIMSG_INVALID_TARGET:
		if (IsInValidTarget())
			return TRUE;
		break;
	}
	return FALSE;
}


void CAIMonster2::ProcessAIMsg(int nInput, int nOutput, DWORD dwParam1, DWORD dwParam2)
{
	OnEndState(nInput, dwParam1, dwParam2);

	m_nState = (AI2_STATE)nOutput;
	m_dwTick = ::GetTickCount();

	OnBeginState(nInput, dwParam1, dwParam2);
}

void CAIMonster2::OnEndState(int nInput, DWORD dwParam1, DWORD dwParam2)
{
}

void CAIMonster2::OnBeginState(int nInput, DWORD dwParam1, DWORD dwParam2)
{
	CMover* pMover = GetMover();

	switch (GetState())
	{
	case AI2_MOVE:
	{
		D3DXVECTOR3 vPos = m_vPosBegin;
		int x = xRandom(20);
		int z = xRandom(20);
		vPos.x += (float)(x - 10);
		vPos.z += (float)(z - 10);

		MoveToDst(vPos);
	}
	break;
	case AI2_IDLE:
		//m_dwReattack = GetTickCount() + xRandom( 0, 2000 );
		m_dwReattack = GetTickCount() + 2000;
		m_idTarget = NULL_ID;
		m_vTarget.x = m_vTarget.y = m_vTarget.z = 0.0f;
		pMover->SetStop();
		MoveToDst(pMover->GetPos());
		break;

	case AI2_ATTACK:
		if (BeginAttack() == FALSE)
			SendAIMsg(AIMSG_END_MELEEATTACK, 0, 0);
		break;

	case AI2_TRACKING:
		if (m_idTarget == NULL_ID)
		{
			m_idTarget = m_idLastAttacker;
			ASSERT(m_idTarget != NULL_ID);
		}

		if (pMover->GetDestId() == m_idTarget && pMover->m_fArrivalRange == m_fAttackRange)
			break;

		pMover->SetDestObj(m_idTarget, m_fAttackRange, TRUE);
		break;
	case AI2_SEARCH:
		break;
	}
}

void CAIMonster2::RouteMessage()
{
	if (IsControllable() == FALSE)
		return;

	DWORD dwCurTick = ::GetTickCount();
	CFSMstate* pState = GetFSM(m_dwFsmType)->GetState(m_nState);
	if (pState == NULL)
		return;

	for (int i = 0; i < pState->GetPolling(); ++i)
	{
		FSM_STATE* p = pState->GetEntry(i);
		BOOL bHandle = Check(p->nInput, dwCurTick, p->dwExtra);
		if (bHandle)
		{
			ProcessAIMsg(p->nInput, p->nOutput, 0, 0);	// 3rd, 4th data is default
			break;
		}
	}
}


void CAIMonster2::PostAIMsg(DWORD dwMessage, DWORD dwParam1, DWORD dwParam2)
{
	SendAIMsg(dwMessage, dwParam1, dwParam2);
}


void CAIMonster2::SendAIMsg(DWORD dwMessage, DWORD dwParam1, DWORD dwParam2)
{
	if (dwMessage == AIMSG_DAMAGE)
		m_idLastAttacker = dwParam1;

	if (IsControllable() == FALSE)
		return;

	int nInput, nOutput;
	nInput = (int)dwMessage;

	BOOL bResult = GetFSM(m_dwFsmType)->GetTransition(m_nState, nInput, nOutput);
	if (bResult == FALSE)
		return;

	ProcessAIMsg(nInput, nOutput, dwParam1, dwParam2);
}


void CAIMonster2::MoveToDst(const D3DXVECTOR3& vDst)
{
	CMover* pMover = GetMover();
	if (pMover->GetDestPos() == vDst)
		return;

	pMover->SetDestPos(vDst);
	g_UserMng.AddSetDestPos(pMover, vDst, 1);
}

BOOL CAIMonster2::SelectTarget()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	int		nAttackFirstRange = pMover->GetProp()->m_nAttackFirstRange;

	FLOAT fRadius = pMover->GetRadiusXZ();
	FLOAT fRadiusSq = fRadius * fRadius;

	CMover* pLastAttacker = prj.GetMover(m_idLastAttacker);
	if (IsValidObj(pLastAttacker) && pLastAttacker->IsDie())
	{
		m_idLastAttacker = NULL_ID;
		pLastAttacker = NULL;
	}

	if (pLastAttacker == NULL)
	{
		m_idLastAttacker = NULL_ID;
	}
	else
	{
		D3DXVECTOR3 vDist = pLastAttacker->GetPos() - pMover->GetPos();
		FLOAT fDistSq = D3DXVec3LengthSq(&vDist);
		if (fDistSq >= fRadiusSq * 10.0f)
		{

			m_idLastAttacker = NULL_ID;
			pLastAttacker = NULL;
		}
	}

	m_idTarget = NULL_ID;
	m_vTarget.x = m_vTarget.y = m_vTarget.z = 0;

	if (m_idLastAttacker == NULL_ID)
	{
		CMover* pTarget = NULL;
		pTarget = ScanTarget(pMover, nAttackFirstRange, JOB_ALL);
		if (pTarget)
		{
			if (pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly())
				m_idTarget = pTarget->GetId();
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
	{

		DWORD dwNum = xRandom(100);
		DWORD dwAggroRate = 50;

		if (IsValidObj(pLastAttacker))
		{
			if (pLastAttacker->GetJob() == JOB_MERCENARY)
				dwAggroRate = 70;
		}

		if (dwNum < dwAggroRate)
		{

			m_idTarget = m_idLastAttacker;
		}
		else if (dwNum < 75)
		{

			CMover* pTarget = ScanTargetStrong(pMover, (float)(nAttackFirstRange));
			if (pTarget)
			{

				if (pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly())
					m_idTarget = pTarget->GetId();
				else
					m_idTarget = m_idLastAttacker;
			}
			else
				m_idTarget = m_idLastAttacker;
		}
		else if (dwNum < 100)
		{

			CMover* pTarget = ScanTargetOverHealer(pMover, (float)(nAttackFirstRange));
			if (pTarget)
			{

				if (pMover->IsFlyingNPC() == pTarget->m_pActMover->IsFly())
					m_idTarget = pTarget->GetId();
				else
					m_idTarget = m_idLastAttacker;
			}
			else
				m_idTarget = m_idLastAttacker;
		}
	}

	return TRUE;
}


BOOL CAIMonster2::Search()
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	CModelObject* pModel = (CModelObject*)pMover->GetModel();

	FLOAT fRadius = pMover->GetRadiusXZ();
	FLOAT fRadiusSq = fRadius * fRadius;

	if (m_idTarget == NULL_ID &&
		(m_vTarget.x == 0 && m_vTarget.y == 0 && m_vTarget.z == 0))
	{

		if (SelectTarget() == FALSE)
			return FALSE;
	}

	D3DXVECTOR3	vTarget;
	FLOAT	fDistSq = 0;
	CMover* pTarget = NULL;

	if (m_idTarget != NULL_ID)
	{
		pTarget = prj.GetMover(m_idTarget);
		if (IsValidObj(pTarget))
			vTarget = pTarget->GetPos();
		else
		{
			m_idTarget = NULL_ID;
			return FALSE;
		}
	}
	else if (m_vTarget.x && m_vTarget.y && m_vTarget.z)
	{
		vTarget = m_vTarget;
	}
	else
	{
		return FALSE;
	}

	D3DXVECTOR3 vDist = vTarget - pMover->GetPos();
	fDistSq = D3DXVec3LengthSq(&vDist);
	m_fAttackRange = fRadius;

	if (fDistSq < fRadiusSq * 4.0f)
	{
		DWORD dwNum = xRandom(100);

		if (dwNum < 85)
		{
			if (xRandom(2))
				m_nAttackType = CAT_NORMAL;
			else
				m_nAttackType = CAT_NORMAL2;

			m_fAttackRange = 5.0f;
		}
		else
		{
			m_nAttackType = CAT_QUAKEDOUBLE;
		}
	}
	else if (fDistSq < fRadiusSq * 6.0f)
	{
		DWORD dwNum = xRandom(100);

		if (dwNum < 60)
		{
			m_nAttackType = CAT_QUAKE_ONE;
			m_fAttackRange = 15.0f;
		}
		else
		{
			m_idTarget = NULL_ID;
			return FALSE;
		}
	}
	else
	{
		m_nAttackType = CAT_NORMAL;
		m_fAttackRange = fRadius;		//10.0f;
	}

	return TRUE;
}


BOOL CAIMonster2::BeginAttack()
{
	CMover* pMover = GetMover();

	OBJMSG		dwMsg = OBJMSG_NONE;
	DWORD		dwItemID = 0;
	MoverProp* pProp = pMover->GetProp();


	switch (m_nAttackType)
	{
	case CAT_NORMAL:		dwMsg = OBJMSG_ATK1;	dwItemID = pProp->dwAtk1;	break;
	case CAT_NORMAL2:		dwMsg = OBJMSG_ATK2;	dwItemID = pProp->dwAtk1;	break;
	case CAT_QUAKEDOUBLE:	dwMsg = OBJMSG_ATK3;	dwItemID = pProp->dwAtk3;	break;
	case CAT_QUAKE_ONE:		dwMsg = OBJMSG_ATK4;	dwItemID = pProp->dwAtk2;	break;
	default:
		ASSERT(0);
	}

	if (dwMsg == OBJMSG_NONE)
		return FALSE;

	if (m_idTarget == NULL_ID)
		return FALSE;

	//	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem( OT_MOVER, pMover->GetIndex() );
	//	if( lpModelElem == NULL )
	//		return FALSE;
	//	if( lpModelElem->m_nMax
	dwMsg = OBJMSG_ATK1;

	int nResult = pMover->DoAttackMelee(m_idTarget, dwMsg, dwItemID);
	if (nResult)
	{
		CMover* pTarget = prj.GetMover(m_idTarget);


		switch (m_nAttackType)
		{
		case CAT_QUAKEDOUBLE:
		{
			if (pTarget)
			{
				g_UserMng.AddWorldShout(pMover->GetName(), prj.GetText(TID_GAME_BOSS_BIGMUSCLE_MSG_04),
					pTarget->GetPos(), pTarget->GetWorld());
			}
		}
		break;
		case CAT_QUAKE_ONE:
		{
			if (pTarget)
			{
				TCHAR szChar[128] = { 0 };
				sprintf(szChar, prj.GetText(TID_GAME_BOSS_BIGMUSCLE_MSG_05), pTarget->GetName());
				g_UserMng.AddWorldShout(pMover->GetName(), szChar,
					pTarget->GetPos(), pTarget->GetWorld());
			}
		}
		break;
		}

		return TRUE;
	}

	return FALSE;
}


BOOL CAIMonster2::IsInValidTarget()
{
	if (m_idTarget == NULL_ID)
		return TRUE;

	CMover* pTarget = prj.GetMover(m_idTarget);
	if (IsValidObj(pTarget) == FALSE)
		return TRUE;

	if (pTarget->IsDie())
		return TRUE;

	if (GetMover()->IsFlyingNPC() != pTarget->m_pActMover->IsFly())
		return TRUE;

	return FALSE;
}
