#include "stdafx.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"
#include "ErrorCode.h"
#include "party.h"

#ifdef __CLIENT
#include "DPClient.h"
#else
#include "user.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "..\_AIInterface\AIInterface.h"
#endif

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER



extern	CPartyMng	g_PartyMng;

#ifdef __CLIENT
extern	CDPClient	g_DPlay;
#else
extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
#endif

int		CActionMover::ProcessActMsg1(CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5)
{
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwMsg)
	{

	case OBJMSG_STAND:
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE))
		{
			ResetState(OBJSTA_MOVE_ALL);
			m_vDelta.x = m_vDelta.z = 0;
			if (pMover->IsFlyingNPC())
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}

		if (GetMoveState() == OBJSTA_STAND)	return 0;
		if (GetMoveState() == OBJSTA_PICKUP)	return 0;
		if (IsActJump())		return -1;
		if (IsActAttack())		return -2;
		if (IsActDamage())		return -3;
		if (IsDie())			return -4;
		if (IsAction())		return 0;
		SetMoveState(OBJSTA_STAND);
		pMover->SetMotion(MTI_STAND);
		RemoveStateFlag(OBJSTAF_ETC);
		break;
	case OBJMSG_STOP:
	case OBJMSG_ASTOP:
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE))
		{
			ResetState(OBJSTA_MOVE_ALL);
			m_vDelta.x = m_vDelta.z = 0;
			if (pMover->IsFlyingNPC())
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}

		return 0;
		break;

	case OBJMSG_WAIT:
	{
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE))
		{
			ResetState(OBJSTA_MOVE_ALL);
			m_vDelta.x = m_vDelta.z = 0;
			if (pMover->IsFlyingNPC())
			{
				m_vDelta.y = 0;
				pMover->SetAngleX(0);
			}
		}

		SetMoveState(OBJSTA_WAIT);
		pMover->SetMotion(MTI_STAND);
	}
	break;
	case OBJMSG_PICKUP:
		if (IsAction())	return 0;
		if (IsActJump())	return 0;
		if (IsActAttack())	return 0;
		if (IsActDamage())	return 0;
		if (IsDie())
			return 0;

		if (GetMoveState() == OBJSTA_PICKUP)	return 0;
		SetMoveState(OBJSTA_PICKUP);
		pMover->SetMotion(MTI_PICKUP, ANILOOP_1PLAY, MOP_FIXED);
		break;

	case OBJMSG_COLLECT:
		ClearState();
		ResetState(OBJSTA_ACTION_ALL);
		SetState(OBJSTA_ACTION_ALL, OBJSTA_COLLECT);
		pMover->m_dwFlag |= MVRF_NOACTION;
		pMover->SetMotion(MTI_COLLECT, ANILOOP_LOOP, MOP_FIXED);
		pMover->RemoveInvisibleSkill();
		break;

	case OBJMSG_STUN:
		SetState(OBJSTA_ACTION_ALL, OBJSTA_STUN);
		pMover->m_dwFlag |= MVRF_NOACTION;
#ifndef __FIX_STUNBUG
		pMover->m_wStunCnt = (short)nParam1;
		if (GetState() == OBJSTA_STAND)
			pMover->SetMotion(MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED);
#else
		pMover->SetMotion(MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED);
#endif
		break;

	case OBJMSG_TURNMOVE:
		if (pMover->m_dwFlag & MVRF_NOMOVE)
			return 0;

		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
			return 0;

		if (GetState() & OBJSTA_DMG_FLY_ALL)
			return 0;

		if (IsActAttack())
			return -2;

		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;

		pMover->SetAngle((float)nParam1);
		if (pMover->IsFlyingNPC())
			pMover->SetAngleX((float)nParam2);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_FORWARD);
		break;
	case OBJMSG_TURNMOVE2:
		if (GetMoveState() == OBJSTA_FMOVE && (int)pMover->GetAngle() == nParam1)
			return 0;

		if (pMover->m_dwFlag & MVRF_NOMOVE)
			return 0;

		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
			return 0;

		if (IsAction())
			return 0;

		if (GetState() & OBJSTA_DMG_FLY_ALL)
			return 0;

		if (IsActJump())
			return -1;

		if (IsActAttack())
			return -2;

		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;

		pMover->SetAngle((float)nParam1);
		if (pMover->IsFlyingNPC())
			pMover->SetAngleX((float)nParam2);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_BACKWARD);
		break;

	case OBJMSG_FORWARD:
		//if (IsActJump())	
			//return -1;

		if (IsActAttack())
			return -2;

		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;

		if (dwMsg == OBJMSG_FORWARD && GetMoveState() == OBJSTA_FMOVE)
			return 0;

		if (pMover->m_dwFlag & MVRF_NOMOVE)
			return 0;

		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
			return 0;

		SetMoveState(OBJSTA_FMOVE);
		if (IsStateFlag(OBJSTAF_WALK))
		{
			pMover->SetMotion(MTI_WALK);
		}
		else
		{
			if (pMover->SetMotion(MTI_RUN))
				pModel->SetMotionBlending(TRUE);
		}
		pMover->OnActFMove();
		break;

	case OBJMSG_STOP_RUN:
#ifdef __CLIENT
		if (IsActJump())	return -1;
		if (IsActAttack())		return -2;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (GetMoveState() == OBJSTA_STOP_RUN)	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SetMoveState(OBJSTA_STOP_RUN);
#endif
		break;



	case OBJMSG_BACKWARD:
		//if (IsActJump())	
			//return -1;	

		if (IsActAttack())
			return -2;

		if (IsActDamage())
			return -3;

		if (GetMoveState() == OBJSTA_BMOVE)
			return 0;

		if (IsAction())
			return 0;

		if (pMover->m_dwFlag & MVRF_NOMOVE)
			return 0;

		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
			return 0;

		pMover->SetMotion(MTI_WALK);
		SetMoveState(OBJSTA_BMOVE);
		break;

	case OBJMSG_LTURN:
		//if (IsActJump())	return -1;
		if (IsActAttack())		return -2;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (GetTurnState() == OBJSTA_LTURN)	return 0;
		if (IsAction())	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SetTurnState(OBJSTA_LTURN);
		break;
	case OBJMSG_RTURN:
		//if (IsActJump())	return -1;
		if (IsActAttack())		return -2;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (GetTurnState() == OBJSTA_RTURN)	return 0;
		if (IsAction())	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SetTurnState(OBJSTA_RTURN);
		break;
	case OBJMSG_STOP_TURN:
		if (GetTurnState() == 0)		return 0;
		SetTurnState(0);
		break;


	case OBJMSG_LOOKUP:
		if (IsActJump())	return -1;
		if (IsActAttack())		return -2;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (GetTurnState() == OBJSTA_LOOKUP)	return 0;
		if (IsAction())	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SetLookState(OBJSTA_LOOKUP);
		break;
	case OBJMSG_LOOKDOWN:
		if (IsActJump())	return -1;
		if (IsActAttack())		return -2;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (GetTurnState() == OBJSTA_LOOKDOWN)	return 0;
		if (IsAction())	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SetLookState(OBJSTA_LOOKDOWN);
		break;
	case OBJMSG_STOP_LOOK:
		if (GetLookState() == 0)		return 0;
		SetLookState(0);
		break;

	case OBJMSG_JUMP:
		if (IsActJump())		return 0;
		if (IsActAttack())		return -2;
		if (IsActDamage())		return -3;
		if (IsAction())		return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		SendActMsg(OBJMSG_STOP_TURN);
		SetJumpState(OBJSTA_SJUMP1);
		if ((GetState() & OBJSTA_COLLECT) == 0) pMover->SetMotion(MTI_JUMP1, ANILOOP_1PLAY);
		pModel->SetMotionBlending(FALSE);
		break;

	case OBJMSG_ATK_MAGIC1:
	{
		if (IsAction())
			return 0;
		if (GetState() & OBJSTA_ATK_ALL)
			return 0;
		if (pMover->m_dwFlag & MVRF_NOATTACK)
			return -1;
		if (pMover->IsMode(NO_ATTACK_MODE))
			return -1;
		CMover* pHitObj = prj.GetMover(nParam1);
		if (IsInvalidObj(pHitObj))
			return -1;
		if (pHitObj->IsDie())
			return -1;
#ifdef __WORLDSERVER
		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pMover->IsSteal(pHitObj))
				return -1;
		}
#endif
		if (IsActDamage())
			ResetState(OBJSTA_DMG_ALL);
		if (IsStateFlag(OBJSTAF_COMBAT) == FALSE)
			SendActMsg(OBJMSG_MODE_COMBAT);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_STAND);
		SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_MAGIC1);
		pMover->SetMotion(MTI_ATK1, ANILOOP_1PLAY);
		pHitObj->SetJJim(pMover);
		pMover->m_nAtkCnt = 1;
	}
	break;

	case OBJMSG_ATK_RANGE1:
	{
		if (IsAction())
			return 0;
		if (pMover->m_dwFlag & MVRF_NOATTACK)
			return -1;
		if (pMover->IsMode(NO_ATTACK_MODE))
			return -1;
		if (GetState() & OBJSTA_ATK_ALL)
			return 0;

		CMover* pHitObj = prj.GetMover(nParam1);
		if (IsValidObj((CObj*)pHitObj) == FALSE)
			return -1;
		if (pHitObj->IsDie())
			return -1;

		m_idTarget = (DWORD)nParam1;

#ifdef __WORLDSERVER
		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pMover->IsSteal(pHitObj))
				return -1;
		}
#endif
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -1;

		if (IsStateFlag(OBJSTAF_COMBAT) == FALSE)
			SendActMsg(OBJMSG_MODE_COMBAT);

		int nUseMotion = nParam2;
		SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_RANGE1);
		pMover->SetMotion(nUseMotion, ANILOOP_1PLAY);
		pHitObj->SetJJim(pMover);
		pMover->m_nAtkCnt = 1;
	}
	break;

	case OBJMSG_ATK1:
	case OBJMSG_ATK2:
	case OBJMSG_ATK3:
	case OBJMSG_ATK4:
	{
		if (IsAction())
			return 0;
		if (pMover->m_dwFlag & MVRF_NOATTACK)
			return -2;
		if (pMover->IsMode(NO_ATTACK_MODE))
			return -2;

#ifdef __CLIENT
		if (pMover->IsActiveMover())
#endif
		{
			CItemElem* pItemElem = pMover->GetWeaponItem();
			if (pItemElem && (pItemElem->GetProp()->dwItemKind3 == IK3_WAND || pItemElem->GetProp()->dwItemKind3 == IK3_BOW))
				return -2;
		}

		CMover* pHitObj = prj.GetMover(nParam1);
		if (IsValidObj((CObj*)pHitObj) == FALSE)
			return(-1);

		m_idTarget = (DWORD)nParam1;
#ifdef __WORLDSERVER
		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pMover->IsSteal(pHitObj))
				return -2;
		}
#endif
		if (pHitObj->m_pActMover->IsDie())
			return -1;
		if (GetState() & OBJSTA_ATK_ALL)
			return 0;

		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;

		if (IsStateFlag(OBJSTAF_COMBAT) == FALSE)
			SendActMsg(OBJMSG_MODE_COMBAT);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_STAND);


		DWORD dwOption = MOP_SWDFORCE | MOP_NO_TRANS;

		pMover->m_fAniSpeed = pMover->GetAttackSpeed();

		switch (dwMsg)
		{
		case OBJMSG_ATK1:
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK1);
			pMover->SetMotion(MTI_ATK1, ANILOOP_1PLAY, dwOption);
			break;
		case OBJMSG_ATK2:
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK2);
			pMover->SetMotion(MTI_ATK2, ANILOOP_1PLAY, dwOption);
			break;
		case OBJMSG_ATK3:
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK3);
			pMover->SetMotion(MTI_ATK3, ANILOOP_1PLAY, dwOption);
			break;
		case OBJMSG_ATK4:
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK4);
			pMover->SetMotion(MTI_ATK4, ANILOOP_1PLAY, dwOption);
			break;
		}
		pMover->SetAngle(GetDegree(pHitObj->GetPos(), m_pMover->GetPos()));
		pHitObj->SetJJim(pMover);

		pMover->m_nAtkCnt = 1;
		SetObjHit((OBJID)nParam1);
		return 1;
	}
	break;

	case OBJMSG_SP_ATK1:
	case OBJMSG_SP_ATK2:
	{
		CMover* pHitObj = prj.GetMover(nParam1);

		int		nUseMotion = nParam3;
		if (IsValidObj((CObj*)pHitObj) == FALSE)
			return(-1);
		if (pHitObj->m_pActMover->IsDie())
			return(-1);
		if (GetState() & OBJSTA_ATK_ALL)
			return(0);
		if (pMover->m_dwFlag & MVRF_NOATTACK)
			return 0;
		if (GetDmgState() == OBJSTA_DMG)
			ResetState(OBJSTA_DMG_ALL);
		else if (GetState() & OBJSTA_DMG_ALL)
			return -3;
		if (IsStateFlag(OBJSTAF_COMBAT) == FALSE)
			SendActMsg(OBJMSG_MODE_COMBAT);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_STAND);

		DWORD dwOption = MOP_SWDFORCE;
		SetState(OBJSTA_ATK_ALL, OBJSTA_SP_ATK1);
		pMover->SetMotion(nUseMotion, ANILOOP_1PLAY, dwOption);
		pMover->SetAngle(GetDegree(pHitObj->GetPos(), m_pMover->GetPos()));
		return 1;
	}
	break;


	case OBJMSG_MELEESKILL:
	{
		if (IsAction())
			return -2;
		if (pMover->m_dwFlag & MVRF_NOATTACK)
			return -2;
		CMover* pHitObj = prj.GetMover(nParam2);
		if (IsValidObj(pHitObj) == FALSE)
			return -2;
#ifdef __WORLDSERVER
		if (GetState() & OBJSTA_ATK_RANGE1)
			ResetState(OBJSTA_ATK_RANGE1);
		if (GetState() & OBJSTA_ATK_ALL)
			return -2;
#endif

		pMover->m_dwMotion = static_cast<DWORD>(-1);
		ClearState();
		SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_MELEESKILL);
		DWORD dwMotion = (DWORD)nParam1;
		int	nLoop = ANILOOP_1PLAY;

		int	nSkill = pMover->GetActParam(0);
		if (nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE || nSkill == SI_ACR_YOYO_COUNTER)
			nLoop = ANILOOP_CONT;

#ifdef __CLIENT
		if (pMover->HasBuffByIk3(IK3_TEXT_DISGUISE))
			dwMotion = MTI_ATK1;
#endif

		pMover->SetMotion(dwMotion, nLoop, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED);

		m_nMotionHitCount = 0;

		break;
	}

	case OBJMSG_MAGICSKILL:
	case OBJMSG_MAGICCASTING:
	{
		if (IsAction())	return -2;
		if (pMover->m_dwFlag & MVRF_NOATTACK)	return -2;
		CMover* pHitObj = prj.GetMover(nParam2);
		if (IsValidObj(pHitObj) == FALSE)	return -2;
#ifdef __WORLDSERVER
		if (GetState() & OBJSTA_ATK_ALL)	return -2;
#endif

		pMover->m_dwMotion = -1;
		ClearState();

		m_nCastingTime = nParam3 * 4;

		m_dwCastingEndTick = (DWORD)(GetTickCount() + nParam3 * 66.66F);
		m_nCastingTick = (int)(nParam3 * 66.66F);
		m_nCastingSKillID = nParam4;

		if (m_nCastingTime > 0)
		{
			m_nCount = 0;
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_CASTING1);
		}
		else
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_MAGICSKILL);

#ifdef __CLIENT
		if (dwMsg == OBJMSG_MAGICSKILL && pMover->HasBuffByIk3(IK3_TEXT_DISGUISE))
			nParam1 = MTI_ATK1;
#endif

		pMover->SetMotion(nParam1, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED);
		m_nMotionHitCount = 0;
		break;
	}

	case OBJMSG_DAMAGE:
	case OBJMSG_DAMAGE_FORCE:
	{
		CMover* pAttacker = PreProcessDamage(pMover, (OBJID)nParam2, nParam4, nParam5);
		if (pAttacker == NULL)
			return 0;

		if (dwMsg == OBJMSG_DAMAGE_FORCE)
			if (!pMover->CanFlyByAttack())
				dwMsg = OBJMSG_DAMAGE;


		int nDamage = _ProcessMsgDmg(dwMsg, pAttacker, (DWORD)nParam1, nParam3, nParam4, nParam5);
#ifdef __WORLDSERVER
		if (pAttacker->IsPlayer() && CCreateMonster::GetInstance()->IsAttackAble(static_cast<CUser*>(pAttacker), pMover))
#endif // __WORLDSERVER
			pMover->PostAIMsg(AIMSG_DAMAGE, pAttacker->GetId(), nDamage);
		return nDamage;
	}
	break;


	case OBJMSG_DIE:
	{
		ClearState();
		m_vDelta.x = m_vDelta.z = 0;

		pMover->RemoveInvisibleSkill();
#ifdef __WORLDSERVER
		pMover->PostAIMsg(AIMSG_DIE, nParam2);
#endif
		CMover* pAttacker = (CMover*)nParam2;
		if (pAttacker)
		{
			if (pAttacker->IsNPC())
			{
				if (pAttacker->m_idAttacker == pMover->GetId())
				{
					pAttacker->m_idAttacker = NULL_ID;
					pAttacker->m_idTargeter = NULL_ID;
				}
			}
		}

		switch (nParam1)
		{
		case OBJMSG_DAMAGE_FORCE:
		{
			if (pMover->CanFlyByAttack(FALSE))
			{
				pMover->FlyByAttack(FALSE);
				break;
			}
		}
		case OBJMSG_DAMAGE:
		default:
			SetState(OBJSTA_DMG_ALL, OBJSTA_DEAD);
			pMover->SetMotion(MTI_DIE1, ANILOOP_CONT);
			break;

		}

		break;
	}

	case OBJMSG_APPEAR:
		SetState(OBJSTA_ACTION_ALL, OBJSTA_APPEAR);
		pMover->SetMotion(MTI_APPEAR2, ANILOOP_1PLAY, MOP_FIXED | MOP_NO_TRANS);
		break;

	case OBJMSG_MODE_COMBAT:
		if (GetStateFlag() & OBJSTAF_COMBAT)	return 0;
		if (IsActJump())	return 0;
		if (IsActAttack())	return 0;
		if (IsActDamage())	return 0;
		if (IsAction())	return 0;
		AddStateFlag(OBJSTAF_COMBAT);
		pMover->m_dwMotion = -1;
		pMover->SetMotion(MTI_STAND);
		break;
	case OBJMSG_MODE_PEACE:
		if ((GetStateFlag() & OBJSTAF_COMBAT) == 0)	return 0;
		if (IsActJump())	return 0;
		if (IsActAttack())	return 0;
		if (IsActDamage())	return 0;
		if (IsAction())	return 0;
		RemoveStateFlag(OBJSTAF_COMBAT);
		pMover->m_dwMotion = -1;
		pMover->SetMotion(MTI_STAND);
		break;
	case OBJMSG_MODE_WALK:
		if (IsAction())	return 0;
		if (GetStateFlag() & OBJSTAF_WALK)	return 0;
		AddStateFlag(OBJSTAF_WALK);
		break;
	case OBJMSG_MODE_RUN:
		if (IsAction())	return 0;
		if ((GetStateFlag() & OBJSTAF_WALK) == 0)	return 0;
		RemoveStateFlag(OBJSTAF_WALK);
		break;
	case OBJMSG_MODE_FLY:
	{
		if (nParam3 == 0)
		{
			if (IsAction())	return 0;
			if (IsActAttack())	return 0;
			if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
			if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

			{
				CWorld* pWorld = pMover->GetWorld();
				if (pWorld)
				{
					int nAttr = pMover->GetWorld()->GetHeightAttribute(pMover->GetPos().x, pMover->GetPos().z);
					if (nAttr == HATTR_NOFLY)
						return 0;
				}
			}
		}
		AddStateFlag(OBJSTAF_FLY);
		ItemProp* pItemProp = prj.GetItemProp(nParam1);
		if (pItemProp)
		{
			if (pMover->m_nFuel == -1)
				pMover->m_nFuel = (int)pItemProp->dwFFuelReMax;

		}
		ClearState();
#ifdef __CLIENT
		if (m_pMover == CMover::GetActiveMover())
		{
			m_pMover->GetWorld()->SetObjFocus(NULL);
			g_Neuz.m_camera.Unlock();
			g_WndMng.m_pWndTaskBar->OnCancelSkill();
			g_DPlay.SendPlayerBehavior();
		}
#endif

		CModel* pModel = prj.m_modelMng.LoadModel(D3DDEVICE, OT_ITEM, (DWORD)nParam1);

		CModelObject* pModelObject = (CModelObject*)pModel;
		if (pModelObject->m_pBone)
		{
			CString strMotion = pModelObject->GetMotionFileName(_T("stand"));
			assert(strMotion != _T(""));
			pModelObject->LoadMotion(strMotion);
		}

		m_pMover->SetRide(pModel, (DWORD)nParam1);
		break;
	}
	case OBJMSG_MOTION:
	{
		if (IsAction())	return 0;
		if (IsActJump())	return 0;
		if (IsActAttack())	return 0;
		if (IsActDamage())	return 0;
		if (pMover->m_dwFlag & MVRF_NOMOVE)	return 0;
#if __VER >= 10
		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)			return 0;
#endif

		AddStateFlag(OBJSTAF_ETC);
		m_nMotionEx = nParam1;
		int nPlayMode = nParam2;
		if (m_nMotionEx != -1)
			pMover->SetMotion(m_nMotionEx, nPlayMode, MOP_FIXED/* | MOP_SWDFORCE*/);

		pMover->RemoveInvisibleSkill();
	}
	break;

	case OBJMSG_RESURRECTION:
		ClearState();
		SetState(OBJSTA_DMG_ALL, OBJSTA_RESURRECTION);
		pMover->SetMotion(MTI_ASS_RESURRECTION, ANILOOP_1PLAY, MOP_FIXED);
		break;
	}
	return 1;
}

CMover* CActionMover::PreProcessDamage(CMover* pMover, OBJID idAttacker, BOOL bTarget, int nReflect)
{
	if ((pMover->m_dwMode & MATCHLESS_MODE) && pMover->IsAuthHigher(AUTH_ADMINISTRATOR))
		return NULL;

	if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return NULL;

	CCtrl* pCtrl = prj.GetCtrl(idAttacker);
	if (IsValidObj(pCtrl) == FALSE)
		return NULL;

	CMover* pAttacker = NULL;
	if (pCtrl->GetType() == OT_MOVER)
		pAttacker = (CMover*)pCtrl;
	else
		return NULL;

	if (pMover->IsNPC())
	{
		MoverProp* pProp = pMover->GetProp();
		if (pProp && pProp->bKillable != 1)
			return NULL;
	}

#ifdef __WORLDSERVER
	HITTYPE ht = pAttacker->GetHitType(pMover, bTarget, nReflect);
	if (ht == HITTYPE_FAIL)
		return 0;

#endif

	pAttacker->RemoveInvisibleSkill();
	return pAttacker;
}