#include "stdafx.h"
#include "defineObj.h"
#include "defineskill.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"

#ifdef __CLIENT
#include "Sfx.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif // CLIENT

#ifdef __WORLDSERVER
#include "User.h"
#include "..\_AIInterface\AIInterface.h"
extern	CUserMng			g_UserMng;
#endif

#if __VER >= 13
#include "honor.h"
#endif

#include "party.h"
extern	CParty g_Party;




//	State process


void	CActionMover::ProcessState1(CMover* pMover, DWORD dwState, float fSpeed)
{
	int nParam = 0;
	FLOAT fTheta = D3DXToRadian(pMover->GetAngle());
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{

	case OBJSTA_STAND:
		if (GetState() == OBJSTA_STAND)
		{
			if (GetStateFlag() & OBJSTAF_ETC)
			{
#ifdef __CLIENT
				switch (pModel->m_nLoop)
				{
				case ANILOOP_1PLAY:
					if (pModel->IsEndFrame())
						RemoveStateFlag(OBJSTAF_ETC);
					break;
				case ANILOOP_CONT:
				case ANILOOP_LOOP:
					if (pMover->IsActiveMover() && g_WndMng.m_pWndWorld->m_bLButtonDown)
					{
						if (pModel->IsEndFrame())
							RemoveStateFlag(OBJSTAF_ETC);
					}
					break;
				}
#else // client
				if (pModel->IsEndFrame())
					RemoveStateFlag(OBJSTAF_ETC);
#endif // not client
			}
			else
			{
				if (pModel->IsEndFrame())
				{
					if (pMover->IsStateMode(STATE_BASEMOTION_MODE))
					{
						pMover->SetMotion(MTI_STAND);
					}
					else
					{
						if (xRandom(5000) < 2)
							pMover->SetMotion(MTI_IDLE2, ANILOOP_1PLAY);
						else
							if (xRandom(2000) < 2)
								pMover->SetMotion(MTI_IDLE1, ANILOOP_1PLAY);
							else
								pMover->SetMotion(MTI_STAND);
					}
				}
			}
		}
		if (m_bGround && m_vDelta.y <= 0)
			m_vDelta.x = m_vDelta.z = 0;
		break;

	case OBJSTA_FMOVE:
	{
		if (pMover->m_dwFlag & MVRF_NOMOVE)
			break;

		if (pMover->GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
			break;


		if (IsStateFlag(OBJSTAF_WALK))
		{
			if (pMover->m_dwTypeFlag & OBJTYPE_FLYING)
			{
				FLOAT fAngXZ = pMover->GetAngle();
				FLOAT fAngH = pMover->GetAngleX();
				AngleToVector(&m_vDelta, fAngXZ, -fAngH, fSpeed / 4.0f);
			}
			else
			{
				m_vDelta.x = sinf(fTheta) * (fSpeed / 4.0f);
				m_vDelta.z = -cosf(fTheta) * (fSpeed / 4.0f);
			}
			if (GetState() != OBJSTA_FMOVE)
				break;

			pMover->SetMotion(MTI_WALK);
		}
		else
		{
			if (pMover->m_dwTypeFlag & OBJTYPE_FLYING)
			{
				FLOAT fAngXZ = pMover->GetAngle();
				FLOAT fAngH = pMover->GetAngleX();
				AngleToVector(&m_vDelta, fAngXZ, -fAngH, fSpeed);
			}
			else
			{
				m_vDelta.x = sinf(fTheta) * fSpeed;
				m_vDelta.z = -cosf(fTheta) * fSpeed;
			}
			if (GetState() != OBJSTA_FMOVE)
				break;

			pMover->SetMotion(MTI_RUN);
		}
#ifdef __CLIENT
		if (pMover->IsNPC() && GetWorld())
			if (pModel->IsAttrHit())
				GetWorld()->m_pCamera->SetQuake(15);
#endif // CLIENT
	}
	break;

#ifdef __CLIENT
	case OBJSTA_STOP_RUN:
		if (IsActDamage())	break;
		m_vDelta.x = 0.0f;
		m_vDelta.z = 0.0f;
		if (GetState() != OBJSTA_STOP_RUN)		break;
		pMover->SetMotion(MTI_RUN);
		break;
#endif //__CLIENT

	case OBJSTA_BMOVE:
		if (IsActDamage())	break;
		m_vDelta.x = sinf(fTheta) * (-fSpeed / 5.0f);
		m_vDelta.z = -cosf(fTheta) * (-fSpeed / 5.0f);
		if (GetState() != OBJSTA_BMOVE)
			break;

		pMover->SetMotion(MTI_BACK);
		break;

	case OBJSTA_WAIT:
		break;

	case OBJSTA_LEFT:
	case OBJSTA_RIGHT:
		break;
	case OBJSTA_PICKUP:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_MOVE_ALL);
		break;
	case OBJSTA_COLLECT:
		break;
	case OBJSTA_STUN:
		if (pMover->m_dwMotion != MTI_GROGGY)
			if ((GetState() & (~OBJSTA_ACTION_ALL)) != OBJSTA_NOT_STAND)
				pMover->SetMotion(MTI_GROGGY, ANILOOP_LOOP, MOP_FIXED);

#ifndef __FIX_STUNBUG
		if ((pMover->GetAdjParam(DST_CHRSTATE) & CHS_STUN) == 0)
		{
			pMover->m_wStunCnt = 0;
			pMover->m_dwFlag &= (~MVRF_NOACTION);
			ResetState(OBJSTA_ACTION_ALL);
		}
#endif
		break;

	case OBJSTA_APPEAR:
		if (pModel->IsEndFrame())
		{
#ifdef __WORLDSERVER
			pMover->PostAIMsg(AIMSG_END_APPEAR);
#endif
			ResetState(OBJSTA_ACTION_ALL);
		}
		break;

	case OBJSTA_LTURN:
		if (IsActAttack())		break;
		pMover->AddAngle(4);
		break;
	case OBJSTA_RTURN:
		if (IsActAttack())		break;
		pMover->AddAngle(-4);
		break;

	case OBJSTA_LOOKUP:
		if (IsActAttack())		break;
		if (pMover->GetAngleX() > -45.0f)
			pMover->AddAngleX(-1);
		break;
	case OBJSTA_LOOKDOWN:
		if (IsActAttack())		break;
		if (pMover->GetAngleX() < 45.0f)
			pMover->AddAngleX(1);
		break;


	case OBJSTA_SJUMP1:
		if (pMover->m_dwFlag & MVRF_NOMOVE)
			break;

		SetJumpState(OBJSTA_SJUMP2);
		m_vDelta.y = 0.2f + (pMover->GetParam(DST_JUMPING, 0) / 1000.0f);

		if ((GetState() & OBJSTA_COLLECT) == 0)
			pMover->SetMotion(MTI_JUMP2, ANILOOP_CONT);

		pModel->SetMotionBlending(FALSE);
		break;

	case OBJSTA_SJUMP2:
		if (pMover->m_dwFlag & MVRF_NOMOVE)
			break;

		if ((GetState() & OBJSTA_COLLECT) == 0)
			pMover->SetMotion(MTI_JUMP2, ANILOOP_CONT);

		if (m_vDelta.y <= 0)
		{
			SetJumpState(OBJSTA_SJUMP3);
			if ((GetState() & OBJSTA_COLLECT) == 0) pMover->SetMotion(MTI_JUMP3, ANILOOP_CONT);
			pModel->SetMotionBlending(FALSE);
		}
		break;

	case OBJSTA_SJUMP3:
		if (pMover->m_dwFlag & MVRF_NOMOVE)
			break;

		if ((GetState() & OBJSTA_DMG_ALL) == 0 || (GetState() & OBJSTA_COLLECT) == 0)
		{
			if (TRUE == pMover->SetMotion(MTI_JUMP3, ANILOOP_CONT))
				pModel->SetMotionBlending(FALSE);
		}
		if (m_bGround)
		{
			SetJumpState(OBJSTA_SJUMP4);
			pMover->OnActLanding();
			if (GetState() & OBJSTA_DMG_ALL)
				break;
			if (TRUE == pMover->SetMotion(MTI_JUMP4, ANILOOP_1PLAY))
			{
				pModel->SetMotionBlending(FALSE);
			}
		}
		break;

	case OBJSTA_SJUMP4:
		ResetState(OBJSTA_JUMP_ALL);

		if (pModel->IsEndFrame() || (GetState() & OBJSTA_DMG_ALL))
		{
			ResetState(OBJSTA_JUMP_ALL);
			break;
		}

#ifdef __WORLDSERVER
		if (pMover->IsPlayer())
			((CUser*)pMover)->SetHonorAdd(HS_JUMP, HI_COUNT_CHECK);
#endif	// __WORLDSERVER
		break;

	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		_ProcessStateAttack(dwState, nParam);
		break;

	case OBJSTA_SP_ATK1:
	case OBJSTA_SP_ATK2:
		ProcessStateAttackSP(dwState, nParam);
		break;


	case OBJSTA_ATK_MAGIC1:
		ProcessStateAttackMagic(dwState, nParam);
		break;

	case OBJSTA_ATK_RANGE1:
		ProcessStateAttackRange(dwState, nParam);
		break;

	case OBJSTA_ATK_MELEESKILL:
		ProcessStateMeleeSkill(dwState, nParam);
		break;
	case OBJSTA_ATK_CASTING1:
	case OBJSTA_ATK_CASTING2:
		ProcessStateMagicCasting(dwState, nParam);
		break;
	case OBJSTA_ATK_MAGICSKILL:
		ProcessStateMagicSkill(dwState, nParam);
		break;


	case OBJSTA_DMG:
		if (pModel->IsEndFrame())
		{
			ResetState(OBJSTA_DMG_ALL);
		}
		break;
	case OBJSTA_DMG_FLY:
		if (m_bGround)
		{
			if ((GetState() & OBJSTA_STUN) || (GetState() & OBJSTA_COLLECT))
			{
				ResetState(OBJSTA_DMG_FLY);
				break;
			}

			if (m_vDelta.y == 0)
			{
				m_vDelta.x = m_vDelta.z = 0;
				m_vDeltaE.x = m_vDelta.x;
				m_vDeltaE.z = m_vDelta.z;
				m_vDelta.y = 0.04f;
				SetState(OBJSTA_DMG_ALL, OBJSTA_DMG_DIE);
				pMover->SetMotion(MTI_DMGDIE, ANILOOP_CONT);
				break;
			}
		}
		break;

	case OBJSTA_DMG_DIE:
		m_vDelta.x = 0;
		m_vDelta.z = 0;

		if (pModel->IsEndFrame())
		{
			if (pMover->m_nHitPoint > 0)
			{
				SetState(OBJSTA_DMG_ALL, OBJSTA_DMG_LIVE);
				pMover->SetMotion(MTI_DMGLIVE, ANILOOP_1PLAY);
			}
			else
			{
				SetState(OBJSTA_DMG_ALL, OBJSTA_DEAD);
			}
		}
		break;

	case OBJSTA_DMG_LIVE:
		if (pModel->IsEndFrame())
		{
			pMover->m_dwFlag &= (~MVRF_NOACTION);
			ResetState(OBJSTA_DMG_ALL);
			ResetState(OBJSTA_ATK_ALL);
		}
		break;
	case OBJSTA_DEAD:
		if (pModel->m_nLoop != ANILOOP_CONT)
			pModel->m_nLoop = ANILOOP_CONT;

		pMover->m_dwFlag &= (~MVRF_NOACTION);
#ifdef __WORLDSERVER
		if (!pMover->IsPlayer())
		{
			SetState(OBJSTA_DMG_ALL, OBJSTA_DISAPPEAR);
			m_nCount = 0;
		}
#endif // WORLDSERVER

#ifdef __CLIENT
		if (pMover->IsNPC())
		{
			if (pModel->IsAttrQuake())
			{
				if (pMover->GetProp()->dwClass == RANK_MATERIAL)
					GetWorld()->m_pCamera->SetQuake(60, 1.0f);
				else
					GetWorld()->m_pCamera->SetQuake(15);
			}
		}
#endif	// __CLIENT
		break;
	case OBJSTA_DISAPPEAR:
#ifndef __CLIENT
		if (!pMover->IsPlayer() && m_nCount > FRAME_PER_SEC)
			pMover->Delete();
#endif	//__CLIENT
		break;
	case OBJSTA_RESURRECTION:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_DMG_ALL);
		break;
	}
}





void	CActionMover::ProcessStateMeleeSkill(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	int nSkill = pMover->GetActParam(0);

	if (nSkill == SI_ACR_YOYO_COUNTER)
	{
	}
	else
		if (nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE)
		{
			if (pModel->IsEndFrame())
			{
				ResetState(OBJSTA_ATK_ALL);
				return;
			}
		}
		else
		{
			if (pModel->m_fFrameCurrent >= (pModel->m_nFrameMax * 0.65f))
			{
				pModel->m_bEndFrame = TRUE;
				ResetState(OBJSTA_ATK_ALL);
				return;
			}
		}

	if ((g_nProcessCnt & 3) == 0)
	{
		if (m_nShootCnt > 0)
		{
			pMover->OnMeleeSkill(0);
			m_nShootCnt--;
		}
	}


	if (pModel->m_nPause > 0)
	{
		pModel->m_nPause--;
	}
	else
	{
		if (pModel->IsAttrHit())
		{
			if (nSkill == SI_BLD_DOUBLESW_SILENTSTRIKE)
			{
				D3DXVECTOR3 vForce;
				AngleToVectorXZ(&vForce, pMover->GetAngle(), 0.75f);
				SetExternalForce(vForce);
			}
			else if (nSkill == SI_BIL_KNU_BELIALSMESHING)
			{
				D3DXVECTOR3 vForce;
				AngleToVectorXZ(&vForce, pMover->GetAngle(), 0.35f);
				SetExternalForce(vForce);
			}

			pMover->OnMeleeSkill(0, m_nMotionHitCount);
			m_nMotionHitCount++;


			if (pMover->GetAct() != OBJACT_USESKILL)
				return;
			int		nSkill = pMover->GetActParam(0);
			OBJID	idTarget = pMover->GetActParam(1);
			DWORD	dwLevel = pMover->GetActParam(3);

			ItemProp* pSkillProp = NULL;
			AddSkillProp* pAddSkillProp = NULL;
			BOOL bRet = pMover->GetSkillProp(&pSkillProp, &pAddSkillProp, nSkill, dwLevel, "ProcessStateMeleeSkill");
			if (bRet == FALSE)
				return;

			if (pAddSkillProp->nSkillCount > 0)
			{
				m_nShootCnt = pAddSkillProp->nSkillCount;
				m_nShootCnt--;
			}
		}
	}
}






void	CActionMover::ProcessStateMagicCasting(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	if (dwState == OBJSTA_ATK_CASTING1)
	{
		if (pModel->IsEndFrame())
		{
			pMover->SetMotion(pMover->m_dwMotion + 1, ANILOOP_LOOP, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED);
			//			m_nCount = 0;
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_CASTING2);
		}
	}
	else if (dwState == OBJSTA_ATK_CASTING2)
	{
		//		if( ++m_nCount >= m_nCastingTime )
		if (m_nCount >= (m_nCastingTime))
		{
			int dwMotion = pMover->m_dwMotion + 1;
#ifdef __CLIENT
			if (pMover->HasBuffByIk3(IK3_TEXT_DISGUISE))
				dwMotion = MTI_ATK1;
#endif
			pMover->SetMotion(dwMotion, ANILOOP_1PLAY, MOP_SWDFORCE | MOP_NO_TRANS | MOP_FIXED);
			SetState(OBJSTA_ATK_ALL, OBJSTA_ATK_MAGICSKILL);
		}
	}

#if defined(__CLIENT)
	pModel->m_SparkInfo.m_bUsed = TRUE;
	pModel->m_SparkInfo.m_v3Color.x = 0.0f;
	pModel->m_SparkInfo.m_v3Color.y = 2.0f;
	pModel->m_SparkInfo.m_v3Color.z = 2.0f;
	pModel->m_SparkInfo.m_nCnt = 0;
	pModel->m_SparkInfo.m_fLerp = 1.0f;

	AngleToVectorXZ(&pModel->m_SparkInfo.m_v3SparkDir, pMover->GetAngle(), -1.0f);
#endif //defined(__CLIENT)
}



void	CActionMover::ProcessStateMagicSkill(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	if (pModel->IsEndFrame())
	{
		ResetState(OBJSTA_ATK_ALL);
		return;
	}

	if ((g_nProcessCnt & 3) == 0)
	{
		if (m_nShootCnt > 0)
		{
			pMover->OnMagicSkill(0);
			m_nShootCnt--;
		}
	}

	if (pModel->m_nPause > 0)
	{
		pModel->m_nPause--;
	}
	else
	{
		if (pModel->IsAttrHit())
		{
			pMover->OnMagicSkill(0, m_nMotionHitCount);
			m_nMotionHitCount++;

			if (pMover->GetAct() != OBJACT_USESKILL)
				return;
			int		nSkill = pMover->GetActParam(0);
			OBJID	idTarget = pMover->GetActParam(1);
			DWORD	dwLevel = pMover->GetActParam(3);

			ItemProp* pSkillProp = NULL;
			AddSkillProp* pAddSkillProp = NULL;
			BOOL bRet = pMover->GetSkillProp(&pSkillProp, &pAddSkillProp, nSkill, dwLevel, "ProcessStateMagicSkill");
			if (bRet == FALSE)
				return;

			if (pAddSkillProp->nSkillCount > 0)
			{
				m_nShootCnt = pAddSkillProp->nSkillCount;
				m_nShootCnt--;
			}
		}
	}
}




void	CActionMover::_ProcessStateAttack(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		if (pMover->IsPlayer())
		{
			int		nReqLevel = 0;
			OBJMSG	dwNextMsg = OBJMSG_ATK2;
			switch (dwState)
			{
			case OBJSTA_ATK1:	nReqLevel = 2;	dwNextMsg = OBJMSG_ATK2;	break;
			case OBJSTA_ATK2:	nReqLevel = 8;	dwNextMsg = OBJMSG_ATK3;	break;
			case OBJSTA_ATK3:	nReqLevel = 12;	dwNextMsg = OBJMSG_ATK4;	break;
			case OBJSTA_ATK4:	nReqLevel = 999;	dwNextMsg = OBJMSG_ATK1;	break;
			}
			if (pModel->IsEndFrame())
			{
#ifdef __WORLDSERVER
				SetObjHit(NULL_ID);
#endif
				ResetState(OBJSTA_ATK_ALL);

#ifdef __CLIENT
				if (GetMover()->IsActiveMover())
				{
					if ((pMover->GetLevel() >= nReqLevel) && ((m_dwCtrlMsg & CTRLMSG_LDOWN) || ((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack))
					{
						ItemProp* pItemProp = pMover->GetActiveHandItemProp();
						if (pItemProp && pItemProp->dwItemKind3 == IK3_YOYO)
						{
							if (dwState == OBJSTA_ATK2 || dwState == OBJSTA_ATK3 || dwState == OBJSTA_ATK4)
								break;
						}

						CMover* pHitObj = prj.GetMover(GetObjHit());
						if (IsValidObj((CObj*)pHitObj)
							&& pHitObj->IsMode(TRANSPARENT_MODE) == 0
							&& g_WndMng.m_pWndWorld->GetNextSkill() == NEXTSKILL_NONE
							//&& pMover->IsRangeObj(pHitObj, pMover->GetAttackRange(pItemProp != NULL ? pItemProp->dwAttackRange : AR_SHORT)) == TRUE
							)
						{
							pMover->DoAttackMelee(pHitObj, dwNextMsg);
						}
						else
							SetObjHit(NULL_ID);
					}
				}
#endif	// CLIENT
			}
		}
		else
		{
#ifdef __CLIENT
			if (pMover->m_dwIndex == MI_CLOCKWORK1)
			{
				if (dwState == OBJSTA_ATK3)
				{
					if ((pMover->m_dwFlag & MVRF_LASERCHARGE) == 0)
					{

						CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRAMP, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
			}
			else if (pMover->m_dwIndex == MI_BIGMUSCLE)
			{
			}
			else if (pMover->m_dwIndex == MI_KRRR)
			{
			}
			else if (pMover->m_dwIndex == MI_MUSHMOOT)
			{
				if (dwState == OBJSTA_ATK3)
				{
					if ((pMover->m_dwFlag & MVRF_LASERCHARGE) == 0)
					{

						CSfxMushmootCharge* pSfx = (CSfxMushmootCharge*)CreateSfx(D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->m_nEventPos = 0;
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
				else if (dwState == OBJSTA_ATK4)
				{
					if ((pMover->m_dwFlag & MVRF_LASERCHARGE) == 0)
					{
						CSfxMushmootCharge* pSfx = NULL;

						pSfx = (CSfxMushmootCharge*)CreateSfx(D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->m_nEventPos = 1;
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx(D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->m_nEventPos = 2;
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx(D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->m_nEventPos = 3;
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
						pSfx = (CSfxMushmootCharge*)CreateSfx(D3DDEVICE, XI_SKILL_MUSHMOOT_CHARGE, pMover->GetPos(), pMover->GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
						if (pSfx)
						{
							pSfx->m_nEventPos = 4;
							pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
							pMover->m_dwFlag |= MVRF_LASERCHARGE;
						}
					}
				}
			}
#endif // Client
			if (pMover->m_dwFlag & MVRF_HITCONT)
				pMover->OnAttackMeleeContinue(dwState);

			if (pModel->IsEndFrame())
			{
				pMover->OnEndMeleeAttack(dwState);
				pMover->m_dwFlag &= (~MVRF_HITCONT);
				pMover->m_dwFlag &= (~MVRF_LASERCHARGE);
				SetObjHit(NULL_ID);
				ResetState(OBJSTA_ATK_ALL);
#ifdef __CLIENT
				extern CPartsLaser g_Laser;
				g_Laser.m_bActive = FALSE;
#endif
				pMover->m_pActMover->m_nMotionHitCount = 0;
			}
		}

		if (pModel->m_nPause > 0)
		{
#ifdef __CLIENT
			CMover* pHitObj = prj.GetMover(m_idTarget);
			D3DXVECTOR3 vDist = pMover->GetPos() - CMover::GetActiveMover()->GetPos();
			float fDistSq = D3DXVec3LengthSq(&vDist);
			if (pHitObj && pMover->IsPlayer() && fDistSq < 32.0f * 32.0f)
			{
				int i;
				D3DXVECTOR3	vPos = pHitObj->GetPos();

				D3DXVECTOR3 vLocal;
				AngleToVectorXZ(&vLocal, pMover->GetAngle(), 1.0f);
				vLocal += pMover->GetPos();
				vPos = vLocal;

				vPos.y += 0.7f;
				D3DXVECTOR3 vVel;
				float	fAngXZ, fAngH;
				float	fSpeed;
				float	fDist;
				float	fMoverAngle = pMover->GetAngle();
				CParticles* pParticles = NULL;

				int nOption = 0;
				int nEffLevel = 0;
				if (pMover->IsActiveMover())
				{
					CItemElem* pItemElem = pMover->GetWeaponItem();
					if (pItemElem)
						nOption = pItemElem->GetAbilityOption();
				}
				else
				{
					nOption = pMover->m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF;
				}

				if (nOption == 10)
					nEffLevel = 5;
				else if (nOption == 9)
					nEffLevel = 4;
				else if (nOption == 8)
					nEffLevel = 3;
				else if (nOption == 7)
					nEffLevel = 2;
				else if (nOption >= 4)	// 4 ~ 6
					nEffLevel = 1;
				else
					nEffLevel = 0;

				for (i = 0; i < 1; i++)
				{
					fAngXZ = xRandomF(360);

					fAngXZ = D3DXToRadian(fAngXZ);
					fAngH = 15.0f + xRandomF(45);
					fAngH = D3DXToRadian(fAngH);
					fSpeed = (0.1f + xRandomF(0.03f));// / 1.0f;

					fDist = cosf(-fAngH) * fSpeed;
					vVel.x = sinf(fAngXZ) * fDist;
					vVel.z = -cosf(fAngXZ) * fDist;
					vVel.y = -sinf(-fAngH) * fSpeed;
					if (g_Option.m_bSFXRenderOff != TRUE)
					{
						if (nEffLevel == 5)
						{
							pParticles = g_ParticleMng.CreateParticle(nEffLevel + xRandom(6), vPos, vVel, CMover::GetActiveMover()->GetPos().y);
						}
						else
							pParticles = g_ParticleMng.CreateParticle(nEffLevel, vPos, vVel, CMover::GetActiveMover()->GetPos().y);
					}
					if (pParticles)
						pParticles->m_bGravity = TRUE;
				}
			}
#endif // CLIENT

			--pModel->m_nPause;
		}
		else
		{
#if defined(__CLIENT)
			ItemProp* pItemProp = pMover->GetActiveHandItemProp();
			if (pMover->IsPlayer() && pItemProp && pItemProp->dwItemKind3 == IK3_YOYO && pModel->IsAttrQuake())
			{
				CMover* pHitObj = prj.GetMover(m_idTarget);
				if (IsInvalidObj(pHitObj))
					break;

				DWORD dwSfxObj = pItemProp->dwSfxObj2;

				if (dwSfxObj == NULL_ID)
					break;

				D3DXVECTOR3 vPos;
				D3DXVECTOR3 vLocal;
				D3DXVECTOR3 vPosSrc = pMover->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				D3DXVECTOR3 vPosDest = pHitObj->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

				CSfx* pSfx = NULL;

				if (pMover->m_dwMotion == MTI_ATK1)
				{
					pModel->GetHandPos(&vPos, PARTS_RWEAPON, pMover->GetMatrixWorld());
					pSfx = CreateSfxYoYo(D3DDEVICE, dwSfxObj, vPos, pMover->GetId(), vPosDest);
					((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);
				}
				else if (pMover->m_dwMotion == MTI_ATK2)
				{
					pModel->GetHandPos(&vPos, PARTS_LWEAPON, pMover->GetMatrixWorld());
					pSfx = CreateSfxYoYo(D3DDEVICE, dwSfxObj, vPos, pMover->GetId(), vPosDest);
					((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
				}
			}
#endif //__CLIENT

			if (pModel->IsAttrHit())
			{
				pMover->m_dwFlag |= MVRF_HITCONT;

				CMover* pHitObj = prj.GetMover(m_idTarget);
				if (IsValidObj((CObj*)pHitObj) && pHitObj->IsLive())
					pMover->OnAttackMelee(dwState, pHitObj);
			}

#if defined(__CLIENT)
			if (pModel->IsAttrQuake())
			{
				if (pMover->m_dwIndex == MI_BIGMUSCLE ||
					pMover->m_dwIndex == MI_KRRR ||
					pMover->m_dwIndex == MI_MUSHMOOT)
					pMover->GetWorld()->m_pCamera->SetQuake(60, 0.7f);
			}
#endif // defined(__CLIENT)

		}
		break;
	}
}


void	CActionMover::ProcessStateAttackSP(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{
	case OBJSTA_SP_ATK1:
	case OBJSTA_SP_ATK2:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_ATK_ALL);

		if (pModel->m_nPause > 0)
		{
			--pModel->m_nPause;
		}
		else
		{
			if (pModel->IsAttrHit())
				pMover->OnAttackSP();
		}

		break;
	}
}


void	CActionMover::ProcessStateAttackMagic(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{
	case OBJSTA_ATK_MAGIC1:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_ATK_ALL);

		if (pModel->m_nPause > 0)
		{
			--pModel->m_nPause;
		}
		else
		{
			if (pModel->IsAttrHit())
				pMover->OnAttackMagic();
		}

		break;
	}
}

void	CActionMover::ProcessStateAttackRange(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{
	case OBJSTA_ATK_RANGE1:

		if (pModel->IsEndFrame())
			ResetState(OBJSTA_ATK_ALL);

		if (pModel->m_nPause > 0)
		{
			--pModel->m_nPause;
		}
		else
		{
			if (pModel->IsAttrHit())
			{
				pMover->m_dwFlag |= MVRF_HITCONT;
				CMover* pHitObj = prj.GetMover(m_idTarget);

				if (IsValidObj((CObj*)pHitObj) && pHitObj->IsLive())
					pMover->OnAttackRange();
			}
		}

		break;
	}

}
