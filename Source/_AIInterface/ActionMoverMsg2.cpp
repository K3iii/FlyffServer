#include "stdafx.h"
#include "ActionMover.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef __WORLDSERVER
#include "user.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "..\_AIInterface\AIInterface.h"

extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
#endif	// __WORLDSERVER





//	Action Message Process





int		CActionMover::ProcessActMsg2(CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5)
{
	ItemProp* pItemProp = prj.GetItemProp(m_pMover->m_dwRideItemIdx);
	if (!pItemProp)
		Error("ItemProp null ProcessActMsg2: %d %d", dwMsg, m_pMover->m_dwRideItemIdx);

	switch (dwMsg)
	{

	case OBJMSG_STAND:
	{
#ifdef __Y_INTERFACE_VER3
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE))
#else //__Y_INTERFACE_VER3
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE))
#endif //__Y_INTERFACE_VER3
		{
			ResetState(OBJSTA_MOVE_ALL);
		}
		if (GetMoveState() == OBJSTA_STAND)	return 0;
		if (IsActAttack())		return -2;
		if (IsActTurn())		return 0;

		SetMoveState(OBJSTA_STAND);
		BOOL ret;
		if (pItemProp)
			ret = pMover->SetMotion(pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED);
		if (ret == TRUE)
			if (pMover->m_pRide)	
				pMover->m_pRide->m_fFrameCurrent = 0;
		break;
	}
	case OBJMSG_STOP:
	case OBJMSG_ASTOP:
#ifdef __Y_INTERFACE_VER3
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE) || (GetMoveState() == OBJSTA_LMOVE) || (GetMoveState() == OBJSTA_RMOVE))
#else //__Y_INTERFACE_VER3
		if ((GetMoveState() == OBJSTA_FMOVE) || (GetMoveState() == OBJSTA_BMOVE))
#endif //__Y_INTERFACE_VER3
		{
			pMover->ClearDest();
			SendActMsg(OBJMSG_ACC_STOP);
			ResetState(OBJSTA_MOVE_ALL);
		}
		break;

	case OBJMSG_FORWARD:
		if (GetMoveState() == OBJSTA_FMOVE)	return 0;
		SetMoveState(OBJSTA_FMOVE);
		if (IsActTurn())	break;
		if (GetState() & OBJSTA_ATK_ALL)
			return 1;
		
		if (pItemProp)
			pMover->SetMotion(pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED);

		break;

	case OBJMSG_LTURN:
	{
		FLOAT fTurnAngle = (FLOAT)nParam1 / 100.0f;
		if (GetTurnState() == OBJSTA_LTURN && m_fTurnAngle == fTurnAngle)	return 0;
		m_fTurnAngle = fTurnAngle;
		SetTurnState(OBJSTA_LTURN);
		if (GetState() & OBJSTA_ATK_ALL)	return 0;

		if (fTurnAngle > 1.0f && pItemProp)
			pMover->SetMotion(pItemProp->dwUseMotion + MTA_FLTURN1, ANILOOP_LOOP, MOP_FIXED);
	}
	break;
	case OBJMSG_RTURN:
	{
		FLOAT fTurnAngle = (FLOAT)nParam1 / 100.0f;
		if (GetTurnState() == OBJSTA_RTURN && m_fTurnAngle == fTurnAngle)	return 0;
		m_fTurnAngle = fTurnAngle;
		SetTurnState(OBJSTA_RTURN);
		if (GetState() & OBJSTA_ATK_ALL)	return 0;

		if (fTurnAngle > 1.0f && pItemProp)
			pMover->SetMotion(pItemProp->dwUseMotion + MTA_FRTURN1, ANILOOP_LOOP, MOP_FIXED);
	}
	break;
	case OBJMSG_STOP_TURN:
		if (GetTurnState() == 0)		return 0;
		m_fTurnAngle = 0.0f;
		SetTurnState(0);
		if (GetState() & OBJSTA_ATK_ALL)	return 0;

		if (pItemProp)
		{
			if (GetMoveState() == OBJSTA_FMOVE)
				pMover->SetMotion(pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED);
			else
				pMover->SetMotion(pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED);
		}

		break;

	case OBJMSG_LOOKUP:
		if (GetLookState() == OBJSTA_LOOKUP)	return 0;
		SetTurnState(OBJSTA_LOOKUP);
		break;
	case OBJMSG_LOOKDOWN:
		if (GetLookState() == OBJSTA_LOOKDOWN)	return 0;
		SetLookState(OBJSTA_LOOKDOWN);
		break;
	case OBJMSG_STOP_LOOK:
		if (GetLookState() == 0)		return 0;
		SetLookState(0);
		break;


	case OBJMSG_ATK1:
	{
		if ((GetState() & OBJSTA_ATK_ALL) == OBJSTA_ATK1)
			return 0;

		SetObjHit((OBJID)nParam1);
		CMover* pHitObj = prj.GetMover(GetObjHit());
		if (IsInvalidObj(pHitObj) || pHitObj->IsDie())
			return -2;

#ifdef __CLIENT
		if (pMover->IsActiveMover() && pMover->IsRangeObj(pHitObj, 3.0f) == FALSE)
			return 0;
#endif
#ifdef __WORLDSERVER
		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pMover->IsSteal(pHitObj))
				return -2;
		}
#endif

		AddStateFlag(OBJSTAF_COMBAT);
		SetState(OBJSTA_ATK_ALL, OBJSTA_ATK1);
		if (pItemProp)
			pMover->SetMotion(pItemProp->dwUseMotion + MTA_FATK1, ANILOOP_1PLAY, MOP_NO_TRANS | MOP_SWDFORCE | MOP_FIXED);

		pHitObj->SetJJim(pMover);
		pMover->RemoveInvisibleSkill();

#ifdef __CLIENT
		if (pMover->IsActiveMover())
			g_DPlay.SendMeleeAttack2(GetObjHit());
#endif
	}

	break;
	case OBJMSG_ATK_MAGIC1:
	{
		if (IsAction())
			return 0;
		if (GetState() & OBJSTA_ATK_ALL)
			return 0;
		CMover* pHitObj = prj.GetMover(nParam1);
		if (IsInvalidObj(pHitObj))
			return -1;

#ifdef __WORLDSERVER
		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pMover->IsSteal(pHitObj))
				return -2;
		}
#endif
		if (IsActDamage())
			ResetState(OBJSTA_DMG_ALL);
		AddStateFlag(OBJSTAF_COMBAT);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_STAND);
		SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_MAGIC1);
		if (pItemProp)
			pMover->SetMotion(pItemProp->dwUseMotion + MTA_FATK1, ANILOOP_1PLAY, MOP_NO_TRANS | MOP_SWDFORCE | MOP_FIXED);

		pHitObj->SetJJim(pMover);
		pMover->RemoveInvisibleSkill();
	}
	break;
	case OBJMSG_JUMP:
		break;
	case OBJMSG_DAMAGE:
	{
		CMover* pAttacker = PreProcessDamage(pMover, (OBJID)nParam2, nParam4, nParam5);
		if (pAttacker == NULL)
			return 0;

		int nDamage = _ProcessMsgDmg(dwMsg, pAttacker, (DWORD)nParam1, nParam3, nParam4, nParam5);
		pMover->PostAIMsg(AIMSG_DAMAGE, pAttacker->GetId(), nDamage);
		pAttacker->RemoveInvisibleSkill();
		return nDamage;
	}
	break;
	case OBJMSG_DIE:
#ifdef __WORLDSERVER
		pMover->PostAIMsg(AIMSG_DIE, nParam2);
#endif
		SendActMsg(OBJMSG_STAND);
		ResetState(OBJSTA_TURN_ALL);
		SetState(OBJSTA_DMG_ALL, OBJSTA_DEAD);

		m_vDelta.x = m_vDelta.z = 0;

		pMover->SetMotion(MTI_DIE1, ANILOOP_CONT);
		break;

	case OBJMSG_ACC_START:
		AddStateFlag(OBJSTAF_ACC);
		break;
	case OBJMSG_ACC_STOP:
		RemoveStateFlag(OBJSTAF_ACC);
		break;

	case OBJMSG_MODE_GROUND:
		if (nParam3 == 0)
		{
			CWorld* pWorld = pMover->GetWorld();
			if (pWorld)
			{
				int nAttr = pMover->GetWorld()->GetHeightAttribute(pMover->GetPos().x, pMover->GetPos().z);
				if (nAttr == HATTR_NOWALK)
					return 0;
			}
		}
		RemoveStateFlag(OBJSTAF_FLY);
		RemoveStateFlag(OBJSTAF_ACC);
		ClearState();

		pMover->SetAngleX(0);
#ifdef __CLIENT
		if (pMover->IsActiveMover()) 
		{
			if (g_WndMng.m_pWndWorld) 
				g_WndMng.m_pWndWorld->m_fRollAng = 0;

			g_Neuz.m_camera.Lock();
			g_DPlay.SendPlayerBehavior();
		}
#endif

		m_pMover->SetRide(NULL);
		break;

	case OBJMSG_MODE_TURBO_ON:
		if (GetStateFlag() & OBJSTAF_TURBO)		return 0;
		if (pMover->m_tmAccFuel <= 0)	return 0;
		AddStateFlag(OBJSTAF_TURBO);
		break;
	case OBJMSG_MODE_TURBO_OFF:
		if ((GetStateFlag() & OBJSTAF_TURBO) == 0)		return 0;
		RemoveStateFlag(OBJSTAF_TURBO);
		break;


	case OBJMSG_TURNMOVE:
	{
		if (IsActAttack())	return -2;
		if (IsActDamage())	return -3;

		pMover->SetAngle((float)nParam1);
		pMover->SetAngleX((float)nParam2);

		SendActMsg(OBJMSG_STOP_TURN);
		SendActMsg(OBJMSG_ACC_START);
		SendActMsg(OBJMSG_FORWARD);

		break;
	}
	}
	return 1;
}

