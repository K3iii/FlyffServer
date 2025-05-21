#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "defineSkill.h"
#include "Ship.h"
#include "ModelObject.h"
#include "CreateObj.h"
//#include "ModelGlobal.h"


#ifdef __CLIENT
#include "AppDefine.h"
#include "defineSound.h"
#include "Sfx.h"
#include "DPClient.h"
#include "..\_Common\ParticleMng.h"
#else	// __CLIENT
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#include "..\_AIInterface\AIInterface.h"
#include "Party.h"
#endif	// not __CLIENT

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER



#ifdef __CLIENT
extern	CDPClient			g_DPlay;
#else	// __CLIENT
extern	CUserMng			g_UserMng;
extern	CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CPartyMng			g_PartyMng;
#endif	// not __CLIENT


BOOL CMover::OnMeleeSkill(int nType, int nCount)
{
	if (GetAct() != OBJACT_USESKILL)
		return FALSE;

	int		nSkill = GetActParam(0);
	OBJID	idTarget = GetActParam(1);
	DWORD	dwLevel = GetActParam(3);

	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;

	GetSkillProp(&pSkillProp, &pAddSkillProp, nSkill, dwLevel, "OnMeleeSkill");


	CMover* pFocusObj = prj.GetMover(idTarget);
	if (IsInvalidObj(pFocusObj))
		return FALSE;

#if defined(__CLIENT)
	CModelObject* pModel = (CModelObject*)m_pModel;
	if (pModel)
	{
		pModel->m_SparkInfo.m_bUsed = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp = 1.0f;

		D3DXVECTOR3 vDir;
		if (pFocusObj == this)
			vDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		else
			vDir = GetPos() - pFocusObj->GetPos();

		D3DXVec3Normalize(&pModel->m_SparkInfo.m_v3SparkDir, &vDir);
	}
#endif //defined(__CLIENT)

	int nDamage = 0;


	if (pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT)
	{
#ifdef __WORLDSERVER
		float fDmgPower = 0;
		if (pSkillProp->dwDmgShift != NULL_ID)
			fDmgPower = (int)pSkillProp->dwDmgShift * 0.2f;

		OnAttackSFX(pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, (pSkillProp->dwSkillType == KT_SKILL ? AF_MELEESKILL : AF_MAGICSKILL), FALSE);
#endif	// __WORLDSERVER
		nDamage = ShootSkill(pFocusObj, pSkillProp, pAddSkillProp);
	}
	else
	{
		nDamage = DoApplySkill(pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, FALSE);
	}
	TRACE("OnMeleeSkill%s, ", pSkillProp->szName);

	if (nDamage && nCount == 0)
	{

#ifdef __CLIENT
		if (IsActiveMover())
#endif
		{
			if (pAddSkillProp->dwCooldown != 0xFFFFFFFF)
				SetCoolTime(pAddSkillProp, "OnMeleeSkill");
		}
	}
	return TRUE;
}



void	CMover::SubReferTime(DWORD* pTime1, DWORD* pTime2, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
	DWORD dwTime1 = 0, dwTime2 = 0;

	if (pSkillProp->dwReferTarget1 == RT_TIME)
	{
		switch (pSkillProp->dwReferStat1)
		{
		case DST_STA:	dwTime1 = GetSta();		break;
		case DST_DEX:	dwTime1 = GetDex();		break;
		case DST_INT:	dwTime1 = GetInt();		break;
		}

		dwTime1 = (DWORD)(((pSkillProp->dwReferValue1 / 10.0f) * dwTime1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime1 / 50.0f)));
	}

	if (pSkillProp->dwReferTarget2 == RT_TIME)
	{
		switch (pSkillProp->dwReferStat2)
		{
		case DST_STA:	dwTime2 = GetSta();		break;
		case DST_DEX:	dwTime2 = GetDex();		break;
		case DST_INT:	dwTime2 = GetInt();		break;
		}

		dwTime2 = (DWORD)(((pSkillProp->dwReferValue2 / 10.0f) * dwTime2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwTime2 / 50.0f)));
	}

	*pTime1 = dwTime1;
	*pTime2 = dwTime2;
}

BOOL	CMover::ApplyParam(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, BOOL bSend, int nDamage)
{
	int		i;
	CMover* pTarget = (CMover*)this;

	DWORD	dwDestParam, dwSkillTime, dwPainTime, dwAbilityMin;
	int		nAdjParam, nChgParam;
	int		nDestData[3];

	BOOL bPVP = pSrc->GetType() == OT_MOVER && ((CMover*)pSrc)->IsPlayer() && IsPlayer();

	for (i = 0; i < PROP_BONUSES; i++)
	{
		if (pAddSkillProp)
		{
			if (i > 1)
				break;

			dwDestParam = pAddSkillProp->dwDestParam[i];
			dwSkillTime = pAddSkillProp->dwSkillTime;
			dwPainTime = pAddSkillProp->dwPainTime;
			dwAbilityMin = bPVP ? pAddSkillProp->dwAbilityMinPVP : pAddSkillProp->dwAbilityMin;
			nAdjParam = pAddSkillProp->nAdjParamVal[i];
			nChgParam = (int)pAddSkillProp->dwChgParamVal[i];
			nDestData[0] = pAddSkillProp->nDestData1[0];
			nDestData[1] = pAddSkillProp->nDestData1[1];
			nDestData[2] = pAddSkillProp->nDestData1[2];
		}
		else
		{
			dwDestParam = pSkillProp->dwDestParam[i];
			dwSkillTime = pSkillProp->dwSkillTime;
			dwPainTime = pSkillProp->tmContinuousPain;
			dwAbilityMin = pSkillProp->dwAbilityMin;
			nAdjParam = pSkillProp->nAdjParamVal[i];
			nChgParam = (int)pSkillProp->dwChgParamVal[i];
			nDestData[0] = pSkillProp->nDestData1[0];
			nDestData[1] = pSkillProp->nDestData1[1];
			nDestData[2] = pSkillProp->nDestData1[2];
		}
		// destParam 1
		switch (dwDestParam)
		{
		case NULL_ID:
			break;
#ifndef __FIX_STUNBUG
		case DST_CHRSTATE:
			if (nAdjParam & CHS_STUN)
			{
				if (pTarget->SetStun(TRUE, dwSkillTime) == FALSE)
					return FALSE;
				pTarget->m_wStunCnt = 0;
			}
			else if (nAdjParam & CHS_POISON)
			{
				if (pTarget->SetPoison(TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0]) == FALSE)
					return FALSE;
				pTarget->m_wPoisonCnt = 0;
			}
			else if (nAdjParam & CHS_BLEEDING)
			{
				if (nDestData[0] < 0)
					nDestData[0] = nDamage / 10;
				if (pTarget->SetBleeding(TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0]) == FALSE)
					return FALSE;
				pTarget->m_wBleedingCnt = 0;
			}
			else
			{
				BOOL bAble = TRUE;
				if ((nAdjParam & CHS_SLOW) && pTarget->GetProp()->dwClass == RANK_MIDBOSS)
					bAble = FALSE;
				if (bAble)
					pTarget->SetDestParam(dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE);
			}
			break;
#else
		case DST_CHRSTATE:
		{
			if (!pAddSkillProp) // Skills only
				continue;

			if (nAdjParam & CHS_STUN)
				pTarget->SetStun(TRUE);
			else if (nAdjParam & CHS_INVISIBILITY)
				pTarget->SetDarkCover(TRUE);
			else if (nAdjParam & CHS_POISON)
				pTarget->SetPoison(TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0]);
			else if (nAdjParam & CHS_BLEEDING)
			{
				if (nDestData[0] < 0)
					nDestData[0] = nDamage / 10;
				pTarget->SetBleeding(TRUE, pSrc->GetId(), dwSkillTime, dwPainTime, nDestData[0]);
			}
			else
				pTarget->SetDestParam(dwDestParam, nAdjParam, NULL_CHGPARAM, TRUE);
		}
		break;
#endif

		case DST_CHR_STEALHP:
		{
			int nStealHP = (int)(nDamage * (nAdjParam / 100.0f));
			if (pSrc->GetType() == OT_MOVER)
				((CMover*)pSrc)->SetDestParam(DST_HP, nStealHP, NULL_CHGPARAM);
		}
		break;
		case DST_CHR_STEALHP_IMM:
		{
			if (GetType() == OT_MOVER)
			{
				int nHP = GetHitPoint();
				int nStealHP = nAdjParam;
				if (nHP < nStealHP)
					nStealHP = nHP;
				if (pSrc->GetType() == OT_MOVER)
				{
					((CMover*)this)->m_pActMover->SendDamage(AF_FORCE,
						((CMover*)pSrc)->GetId(),
						nStealHP);


					((CMover*)pSrc)->SetDestParam(DST_HP, nStealHP, NULL_CHGPARAM, TRUE);
				}
			}
		}
		break;

		case DST_CURECHR:
			pTarget->RemoveDebuff(nAdjParam);
			break;

		case DST_CLEARBUFF:
			if (nAdjParam == NULL_ID)
			{
				pTarget->RemoveBuffAll();
			}
			else
			{
				for (int i = 0; i < nAdjParam; i++)
					pTarget->RemoveBuffOne();
			}
			break;
		case DST_CHR_CHANCESTUN:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
			pTarget->SetDestParam(dwDestParam, nAdjParam, nChgParam, TRUE);
			break;

#ifdef __WORLDSERVER
		case DST_HP:
			if (pAddSkillProp)
			{
				if (pSkillProp->dwReferTarget1 == RT_HEAL || pSkillProp->dwReferTarget2 == RT_HEAL)
				{
					DWORD dwHp1 = 0, dwHp2 = 0;

					if (pSrc->GetType() == OT_MOVER)
					{
						if (pSkillProp->dwReferTarget1 == RT_HEAL)
						{
							switch (pSkillProp->dwReferStat1)
							{
							case DST_STA:		dwHp1 = ((CMover*)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp1 = ((CMover*)pSrc)->GetDex();			break;
							case DST_INT:		dwHp1 = ((CMover*)pSrc)->GetInt();			break;
							}
							dwHp1 = (DWORD)(((pSkillProp->dwReferValue1 / 10.0f) * dwHp1) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp1 / 50.0f)));
						}

						if (pSkillProp->dwReferTarget2 == RT_HEAL)
						{
							switch (pSkillProp->dwReferStat2)
							{
							case DST_STA:		dwHp2 = ((CMover*)pSrc)->GetSta();			break;
							case DST_DEX:		dwHp2 = ((CMover*)pSrc)->GetDex();			break;
							case DST_INT:		dwHp2 = ((CMover*)pSrc)->GetInt();			break;
							}
							dwHp2 = (DWORD)(((pSkillProp->dwReferValue2 / 10.0f) * dwHp2) + (pAddSkillProp->dwSkillLvl * FLOAT(dwHp2 / 50.0f)));
						}
					}


					int nIncHP = pAddSkillProp->nAdjParamVal1 + dwHp1 + dwHp2;

					if ((pAddSkillProp->dwDestParam1 == DST_HP || pAddSkillProp->dwDestParam2 == DST_HP) && nIncHP > 0)
					{
						if (pSrc->GetType() == OT_MOVER)
							if (((CMover*)pSrc)->IsPlayer())
								if (pTarget->GetMaxHitPoint() > pTarget->GetHitPoint() + nIncHP)
									((CUser*)pSrc)->m_nOverHeal = PROCESS_COUNT * 30;
					}


					pTarget->SetDestParam(pAddSkillProp->dwDestParam1, nIncHP, pAddSkillProp->dwChgParamVal1, TRUE);

				}
			}
			break;
#endif // WorldServer
		default:
			pTarget->SetDestParam(dwDestParam, nAdjParam, nChgParam, bSend);
			break;
		}
	}

	return TRUE;
}


BOOL	CMover::GetSkillProp(ItemProp** ppSkillProp, AddSkillProp** ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr)
{
	char szStr[256] = { 0, };
	sprintf(szStr, "%s-%s", m_szName, szErr);

	return CCtrl::GetSkillProp(ppSkillProp, ppAddSkillProp, nSkill, dwLevel, szStr);
}



#if __VER >= 9	// __SKILL_0706
void	CMover::SetCoolTime(AddSkillProp* pAddSkillProp, LPCTSTR szCall)
{
	int nIdx = GetSkillIdx(pAddSkillProp->dwName);

	if (nIdx < 0 || nIdx >= MAX_SKILL_JOB)
		Error("szCall SetCoolTime : %d %d��ų�� ã�� �� ���� %s", nIdx, pAddSkillProp->dwName, GetName());
	else
		m_tmReUseDelay[nIdx] = pAddSkillProp->dwCooldown + timeGetTime();
}
#else	// __SKILL_0706
void	CMover::SetCoolTime(ItemProp* pSkillProp, LPCTSTR szCall)
{
	int nIdx = GetSkillIdx(pSkillProp->dwID);

	if (nIdx < 0 || nIdx >= MAX_SKILL_JOB)
		Error("szCall SetCoolTime : %d %d��ų�� ã�� �� ���� %s", nIdx, pSkillProp->dwID, GetName());
	else
		m_tmReUseDelay[nIdx] = pSkillProp->dwSkillReady + timeGetTime();
}
#endif	// __SKILL_0706




BOOL CMover::OnMagicSkill(int nType, int nCount)
{
	if (GetAct() != OBJACT_USESKILL)
		return FALSE;

	int			nSkill = GetActParam(0);
	OBJID		idTarget = GetActParam(1);
	DWORD		dwLevel = GetActParam(3);
	BOOL		bControl = GetActParam(4);

	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	LPSKILL		pSkill = NULL;

	BOOL bRet = GetSkillProp(&pSkillProp, &pAddSkillProp, nSkill, dwLevel, "CMover::OnMagicSkill");
	if (bRet == FALSE)
	{
		Error("OnMagicSkill : ��ų�����б� ������. ������������ Motion=%d %d", m_dwMotion, dwLevel);
		return FALSE;
	}

	CMover* pFocusObj = prj.GetMover(idTarget);
	if (IsInvalidObj(pFocusObj))
		return FALSE;

	if (pSkillProp->dwID == SI_ASS_HEAL_HEALING)
	{
		if (pFocusObj->IsDie())
			return FALSE;
	}

	BOOL bSuccess = FALSE;
	BOOL bShoot = pSkillProp->dwExeTarget == EXT_MAGICATKSHOT || pSkillProp->dwExeTarget == EXT_MAGICSHOT;
	if (bShoot)
	{
#ifdef __WORLDSERVER
		float fDmgPower = 0;
		if (pSkillProp->dwDmgShift != NULL_ID)
			fDmgPower = (int)pSkillProp->dwDmgShift * 0.2f;

		OnAttackSFX(pFocusObj->GetId(), pAddSkillProp->dwSkillLvl, pSkillProp->dwID, 0, GetAngle(), fDmgPower, (pSkillProp->dwSkillType == KT_SKILL ? AF_MELEESKILL : AF_MAGICSKILL), bControl);
#endif	// __WORLDSERVER
		bSuccess = ShootSkill(pFocusObj, pSkillProp, pAddSkillProp);
	}
	else
	{
		int nDamage = DoApplySkill(pFocusObj, pSkillProp, pAddSkillProp, false, 0, FALSE, bControl);
		if (nDamage == 0)
			return FALSE;
		bSuccess = (BOOL)nDamage;

#ifdef __CLIENT
		if (pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL)
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

			if (IsActiveMover())
			{
				D3DXVECTOR3 vStart;
				D3DXVECTOR3 vEnd;

				vStart = GetPos();		vStart.y += 1.0f;
				vEnd = pWndWorld->m_vTelePos;

				CWorld* pWorld = GetWorld();

				if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
				{
					g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
					g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
					return FALSE;
				}

				g_DPlay.SendTeleSkill(GetId(), pWndWorld->m_vTelePos);
			}
		}
#endif // Client
	}

	if (bSuccess)
	{
#ifdef __WORLDSERVER

		int nReqMp = GetReqMp(pAddSkillProp->nReqMp);
		if (nReqMp != 9999999)
		{
			if (pAddSkillProp->nSkillCount > 1)
				nReqMp /= pAddSkillProp->nSkillCount;
			IncManaPoint(-nReqMp);
			if (pAddSkillProp->nReqFp != NULL_ID)
				IncFatiguePoint(-GetReqFp(pAddSkillProp->nReqFp));
		}

		if (pSkillProp->dwID == SI_RIG_HERO_RETURN)
		{
			REPLACE_TYPE type = REPLACE_NORMAL;

			PRegionElem pRgnElem = NULL;
			CWorld* pWorld = GetWorld();
			if (!pWorld)
				return FALSE;

			if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
				pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);
			if (NULL == pRgnElem)	// Find near revival pos
				pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), GetPos());

			if (NULL != pRgnElem)
				REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, type, nRevivalLayer);
			else
				REPLACE(g_uIdofMulti, pWorld->GetID(), GetPos(), type, nDefaultLayer);
		}
		if (pSkillProp->dwID == SI_KNT_HERO_DRAWING)
		{
			CWorld* pFocusObjWorld = pFocusObj->GetWorld();
			if (NULL == pFocusObjWorld)
				return FALSE;

			CWorld* pUseMoverWorld = GetWorld();
			if (NULL == pUseMoverWorld)
				return FALSE;

			if (pFocusObjWorld->GetID() != pUseMoverWorld->GetID() || pFocusObj->GetLayer() != GetLayer())
				return FALSE;

			const float fMaxDistance = 60.0f;
			const D3DXVECTOR3 vPosDifference = GetPos() - pFocusObj->GetPos();
			float fTotalDistance = ::sqrt(D3DXVec3LengthSq(&vPosDifference));
			if (fTotalDistance > fMaxDistance)
				fTotalDistance = fMaxDistance;

			const float fPower = fTotalDistance * 0.1f;
			pFocusObj->pushedByAnotherPower(GetDegree(GetPos(), pFocusObj->GetPos()), fPower);
		}
#endif // WORLDSERVER


#ifdef __CLIENT
		if (IsActiveMover() && nCount == 0)
#endif // __CLIENT
			if (pAddSkillProp->dwCooldown != 0xFFFFFFFF)
				SetCoolTime(pAddSkillProp, "OnMagicSkill");
	} // bSuccess

#if defined(__CLIENT)
	CModelObject* pModel = (CModelObject*)m_pModel;
	if (pModel)
	{
		pModel->m_SparkInfo.m_bUsed = TRUE;
		pModel->m_SparkInfo.m_v3Color.x = 2.0f;
		pModel->m_SparkInfo.m_v3Color.y = 2.0f;
		pModel->m_SparkInfo.m_v3Color.z = 2.0f;
		pModel->m_SparkInfo.m_nCnt = 0;
		pModel->m_SparkInfo.m_fLerp = 1.0f;

		AngleToVectorXZ(&pModel->m_SparkInfo.m_v3SparkDir, GetAngle(), -1.0f);
	}
#endif //defined(__CLIENT)

	return TRUE;
} // OnMagicSkill

BOOL	CMover::OnAttackRange()
{
	int		idTarget = GetActParam(0);
	DWORD	dwItemID = GetActParam(1);
	int		idSfxHit = GetActParam(2);
	UNREFERENCED_PARAMETER(idSfxHit);

	CMover* pHit = prj.GetMover(idTarget);
	if (!IsValidObj(pHit))
		return FALSE;

	ItemProp* pItemProp = NULL;
	if (IsPlayer())
		pItemProp = GetActiveHandItemProp();
	else
	{
		if (dwItemID == NULL_ID || dwItemID == 0)
			pItemProp = GetActiveHandItemProp();
		else
			pItemProp = prj.GetItemProp(dwItemID);
	}

	if (!pItemProp)
		return FALSE;

#ifdef __WORLDSERVER
	if (IsNPC())
		CMonsterSkill::GetInstance()->ApplySkill(this, pHit, ATK_RANGE);
#endif 

	DWORD dwSfxObj = pItemProp->dwSfxObj2;

	// ItemProp* pPropBulet = GetActiveHandItemProp(PARTS_BULLET);
	// if (pPropBulet && pPropBulet->dwSfxObj2 != NULL_ID)
	// 	dwSfxObj = pPropBulet->dwSfxObj2;
	

	// if (dwSfxObj == NULL_ID)
	// 	return FALSE;
	

	D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vPosSrc = GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

#ifdef __CLIENT		
	CSfx* pSfx = NULL;
#endif

	if (IsPlayer())
	{
#ifdef __CLIENT
		if (IsActiveMover())
			pSfx = CreateSfxArrow(XI_ITEM_RANGE_ATK1, pItemProp->dwSfxObj3, vPosDest, idTarget);
#else
		g_UserMng.AddCreateSfxAllow(this, XI_ITEM_RANGE_ATK1, pItemProp->dwSfxObj3, vPosDest, idTarget);
#endif
	}
	else
	{
#ifdef __CLIENT		
		pSfx = CreateSfx(D3DDEVICE, dwSfxObj, vPosSrc, GetId(), vPosDest, idTarget);
#endif
	}

	DWORD dwAtkFlags = GetAct() == OBJACT_RANGE_ATTACK ? GetActParam(3) : AF_MISS;
	if (IsPlayer())
		dwAtkFlags |= AF_RANGE;

#ifdef __WORLDSERVER
	OnAttackSFX(pHit->GetId(), (IsPlayer() ? dwItemID : 0), NULL_ID, 0, 0, 0, dwAtkFlags, FALSE);
#else
	if (pSfx)
	{
		if (IsActiveMover())
		{
			idSfxHit = ++m_idSfxHit;
			g_DPlay.SendSfxID(idTarget, idSfxHit, dwAtkFlags);
			pSfx->m_idSfxHit = idSfxHit;
			pSfx->m_nMagicPower = IsPlayer() ? dwItemID : 0;

			CModelObject* pModel = (CModelObject*)m_pModel;
			if (pModel)
				pModel->m_nPause = 5;
		}

		if (IsActiveMover() || (IsNPC() && pHit->IsActiveMover()))
			pSfx->m_idSfxHit = idSfxHit;
	}
#endif 
	return TRUE;
}

BOOL	CMover::OnAttackMagic()
{
	int		idTarget = GetActParam(0);
	int		nMagicPower = GetActParam(1);
	BOOL	bControl = GetActParam(4);

	CMover* pHit = prj.GetMover(idTarget);
	if (IsInvalidObj(pHit))
		return FALSE;

	ItemProp* pItemProp = GetActiveHandItemProp();
	if (!pItemProp)
		return FALSE;

	DWORD dwSfxObj = pItemProp->dwSfxObj2;
	D3DXVECTOR3 vPosDest = pHit->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 vPosSrc = GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

#ifdef __WORLDSERVER
	const DWORD attackFlag = GetAct() == OBJACT_MAGIC_ATTACK ? GetActParam(3) : AF_MISS;
	OnAttackSFX(pHit->GetId(), nMagicPower, NULL_ID, 0, 0, 0, attackFlag, bControl);
	g_UserMng.AddCreateSfxAllow(this, dwSfxObj, pItemProp->dwSfxObj3, vPosDest, idTarget);
#else
	CSfx* pSfx = NULL;
	if (IsActiveMover())
		pSfx = CreateSfx(D3DDEVICE, dwSfxObj, vPosSrc, GetId(), vPosDest, idTarget);

	if (pSfx)
	{
		int idSfxHit = ++m_idSfxHit;
		g_DPlay.SendSfxID(idTarget, idSfxHit, AF_MAGIC);
		pSfx->m_idSfxHit = idSfxHit;
		pSfx->m_nMagicPower = nMagicPower;

		CModelObject* pModel = (CModelObject*)m_pModel;
		if (pModel)
			pModel->m_nPause = 5;
	}
#endif 

	return TRUE;
}

void	CMover::OnAttackMelee_ClockWorks(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);

#ifdef __WORLDSERVER
	UpdateMatrix();
	CModelObject* pModel = (CModelObject*)m_pModel;
	pModel->UpdateMatrixBone();
#endif // WorldServer

	switch (dwState)
	{
	case OBJSTA_ATK1:
	{
		D3DXVECTOR3 vPos;
		CModelObject* pModel = (CModelObject*)m_pModel;
		pModel->GetEventPos(&vPos, 0);
		D3DXVec3TransformCoord(&vPos, &vPos, GetMatrixWorldPtr());

		OnDamage(nItemID, dwState, NULL, &vPos);
#ifdef __CLIENT
		GetWorld()->m_pCamera->SetQuake(15, 0.5f);
		CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRFIRESP, vPos);
		if (pSfx)
			pSfx->SetAngle(-GetAngle());
#endif
	}
	break;
	case OBJSTA_ATK2:
	{
		D3DXVECTOR3 vPos;
		CModelObject* pModel = (CModelObject*)m_pModel;
		pModel->GetEventPos(&vPos, 1);
		D3DXVec3TransformCoord(&vPos, &vPos, GetMatrixWorldPtr());
#ifdef __CLIENT
		CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRCANNON, vPos, GetId());
		if (pSfx)
		{
			pSfx->SetAngle(-GetAngle());
			pSfx->SetScale(D3DXVECTOR3(4.0f, 4.0f, 4.0f));
		}
#endif
		CMover* pTarget = pHitObj;
		if (IsValidObj(pTarget))
		{
			OnDamage(nItemID, dwState, NULL, &pTarget->GetPos());
#ifdef __CLIENT
			pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1RANBALL, vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1);
			if (pSfx)
			{
				pSfx->SetScale(D3DXVECTOR3(4.0f, 4.0f, 4.0f));
				{
					FLOAT fAngXZ, fAngH;
					D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
					xGetDegree(&fAngXZ, &fAngH, vDist);
					((CSfxAtkStraight*)pSfx)->ShootSfx(fAngXZ, -fAngH, 2.0f, XI_NPCSP1RANBOOM);
				}
			}
#endif
		}
	}
	break;
	case OBJSTA_ATK3:
#ifdef __CLIENT
		GetWorld()->m_pCamera->SetQuake(30, 0.7f);
#endif
		break;
	case OBJSTA_ATK4:
	{
		D3DXVECTOR3 vPos;

		vPos = GetPos();
		DWORD dwFlag = AF_GENERIC;
		if (xRandom(100) < 15)
			dwFlag |= AF_FLYING;
		OnDamage(nItemID, dwState, NULL, &vPos, dwFlag);

		CModelObject* pModel = (CModelObject*)m_pModel;
		pModel->GetEventPos(&vPos, 3);
		D3DXVec3TransformCoord(&vPos, &vPos, GetMatrixWorldPtr());

#ifdef __CLIENT
		GetWorld()->m_pCamera->SetQuake(60, 1.5f);

		CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos);
		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
		pModel->GetEventPos(&vPos, 4);
		D3DXVec3TransformCoord(&vPos, &vPos, GetMatrixWorldPtr());

		pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRCIRCLE, vPos);
		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
#endif
	}
	break;
	}
}

void	CMover::OnAttackMelee_BigMuscle(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);
	CModelObject* pModel = (CModelObject*)m_pModel;

#ifdef __WORLDSERVER
	UpdateMatrix();
	pModel->UpdateMatrixBone();
#endif // WorldServer

	switch (dwState)
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	{
		if (IsValidObj(pHitObj))
		{
			OnDamage(nItemID, dwState, pHitObj);
		}
	}
	break;
	case OBJSTA_ATK3:
	{
		D3DXMATRIX mMatrix;

		D3DXMatrixMultiply(&mMatrix, (pModel->GetMatrixBone(12)), &(GetMatrixWorld()));

		D3DXVECTOR3 v2;
		v2.x = mMatrix._41;
		v2.y = mMatrix._42;
		v2.z = mMatrix._43;
		v2.y -= 5.0f;

#ifdef __CLIENT

		CSfx* pSfx = CreateSfx(D3DDEVICE, XI_SKILL_CIRCLE_DUST, v2);
		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(12.0f, 12.0f, 12.0f));

		PLAYSND(SND_PC_SKILLD_HITOFPENYA);//, &v2 );
#endif //__CLIENT

		D3DXMatrixMultiply(&mMatrix, (pModel->GetMatrixBone(16)), &(GetMatrixWorld()));
		D3DXVECTOR3 v3;
		v3.x = mMatrix._41;
		v3.y = mMatrix._42;
		v3.z = mMatrix._43;
		v3.y -= 5.0f;

#ifdef __CLIENT

		pSfx = CreateSfx(D3DDEVICE, XI_SKILL_CIRCLE_DUST, v3);
		if (pSfx)
			pSfx->SetScale(D3DXVECTOR3(12.0f, 12.0f, 12.0f));
#endif //__CLIENT

		if (m_pActMover->m_nMotionHitCount == 1)
		{
#ifdef __WORLDSERVER
			GetWorld()->SendDamageAround(&GetPos(), (AF_GENERIC | AF_CRITICAL), this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
				, 16.0f);

			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_BLEEDING, 10, "OnAttackMelee_BigMuscle");
			if (bSuccess)
				ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);
#endif //__WORLDSERVER

		}
		else
		{
#ifdef __CLIENT
			PLAYSND(SND_PC_BOSS_STUN);
#endif //__CLIENT
#ifdef __WORLDSERVER
			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_BIGMUSCLE_STUNGROUP, 1, "OnAttackMelee_BigMuscle");
			if (bSuccess)
				ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);
#endif //__WORLDSERVER
		}
	}
	break;
	case OBJSTA_ATK4:
	{
		static D3DXVECTOR3 v3OldPos;

		if (m_pActMover->m_nMotionHitCount == 0)
		{
			v3OldPos = pHitObj->GetPos();
		}
		else
			if (m_pActMover->m_nMotionHitCount == 1)
			{
				D3DXMATRIX mMatrix;

				D3DXMatrixMultiply(&mMatrix, (pModel->GetMatrixBone(12)), &(GetMatrixWorld()));

				D3DXVECTOR3 v2;
				v2.x = mMatrix._41;
				v2.y = mMatrix._42;
				v2.z = mMatrix._43;
				v2.y -= 5.0f;

#ifdef __CLIENT

				CSfx* pSfx = CreateSfx(D3DDEVICE, XI_SKILL_CIRCLE_DUST, v2);
				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(9.0f, 9.0f, 9.0f));

				pSfx = CreateSfx(D3DDEVICE, XI_SKILL_MAG_EARTH_ROCKCRASH01, v3OldPos);
				if (pSfx)
					pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));

				PLAYSND(SND_PC_SKILLD_HITOFPENYA);
#endif //__CLIENT

#ifdef __WORLDSERVER
				ItemProp* pSkillProp;
				AddSkillProp* pAddSkillProp;
				BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_BLEEDING, 10, "OnAttackMelee_BigMuscle");
				if (bSuccess)
					DoApplySkill(pHitObj, pSkillProp, pAddSkillProp);

				GetWorld()->SendDamageAround(&(v3OldPos), AF_GENERIC | AF_CRITICAL, this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
					, 4.0f);
#endif //__WORLDSERVER


			}
	}

	break;
	}
}

void	CMover::OnAttackMelee_Krrr(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);

	CModelObject* pModel = (CModelObject*)m_pModel;

#ifdef __WORLDSERVER
	UpdateMatrix();
	pModel->UpdateMatrixBone();
#endif // WorldServer

	switch (dwState)
	{
	case OBJSTA_ATK1:
	{
#ifdef __WORLDSERVER
		OnDamage(nItemID, dwState, pHitObj);
#endif //__WORLDSERVER
	}
	break;
	case OBJSTA_ATK2:
	{
#ifdef __WORLDSERVER
		int nRandom = xRandom(100);
		if (nRandom <= 50)
		{
			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_KRRR_POISON, 1, "OnSfxHit");
			if (bSuccess)
			{
				pHitObj->ApplySkill(this, pSkillProp, pAddSkillProp);
			}
		}
		else
		{
			OnDamage(nItemID, dwState, pHitObj);
		}
#endif // WorldServer
	}
	break;
	case OBJSTA_ATK3:
	{
#ifdef __CLIENT
		CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1DIRFIRESP, pHitObj->GetPos());
		if (pSfx)
			pSfx->SetAngle(-GetAngle());

		PLAYSND(SND_PC_BOSS_CRASH, &pHitObj->GetPos());
#endif //__CLIENT

#ifdef __WORLDSERVER
		ItemProp* pSkillProp;
		AddSkillProp* pAddSkillProp;
		BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_KNOCK_BACK, 4, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);

		GetWorld()->SendDamageAround(&(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK1
			, 7.0f);
#endif //__WORLDSERVER
	}
	break;
	case OBJSTA_ATK4:
	{
		static D3DXVECTOR3	  vLocal;
		if (m_pActMover->m_nMotionHitCount == 0)
		{
#ifdef __CLIENT
			PLAYSND(SND_PC_BOSS_SUMMONS);
#endif //__CLIENT

			AngleToVectorXZ(&vLocal, GetAngle(), 5.0f);
			vLocal += GetPos();

			FLOAT fHeight = GetWorld()->GetLandHeight(vLocal.x, vLocal.z);
			vLocal.y = fHeight;

#ifdef __WORLDSERVER
			CPartyQuestProcessor* pProc = CPartyQuestProcessor::GetInstance();
			CWorld* pWorld = GetWorld();
			if (pWorld)
			{
				int nId = pProc->PtInQuestRect(pWorld->GetID(), GetPos());
				if (nId > -1)
				{
					PPARTYQUESTELEM pElem = pProc->GetPartyQuest(nId);
					PPARTYQUESTRECT pRect = pProc->GetPartyQuestRect(nId);
					if (pElem && pRect)
						g_UserMng.ReplaceWorldArea(pElem->idParty, pRect->dwWorldId, pRect->dwWorldId, vLocal.x, vLocal.z, 0.5f, TRUE, GetLayer());
				}
			}
#endif //__WORLDSERVER
		}
		else
			if (m_pActMover->m_nMotionHitCount == 1)
			{
#ifdef __CLIENT
				PLAYSND(SND_PC_BOSS_STUN);
#endif //__CLIENT
#ifdef __WORLDSERVER
				ItemProp* pSkillProp;
				AddSkillProp* pAddSkillProp;
				BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_KRRR_STUNGROUP, 1, "OnAttackMelee_Krrr");
				if (bSuccess)
					ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);
#endif //__WORLDSERVER

			}
			else
				if (m_pActMover->m_nMotionHitCount == 2)
				{
#ifdef __CLIENT
					D3DXVECTOR3 v3Pos = vLocal;
					CSfx* pSfx = CreateSfx(D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos);
					if (pSfx)
						pSfx->SetScale(D3DXVECTOR3(0.8f, 0.8f, 0.8f));

					v3Pos.x += (xRandomF(12.0f) + -6.0f);
					pSfx = CreateSfx(D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos);
					if (pSfx)
						pSfx->SetScale(D3DXVECTOR3(0.8f, 0.8f, 0.8f));

					v3Pos = vLocal;
					v3Pos.z += (xRandomF(12.0f) + -6.0f);
					pSfx = CreateSfx(D3DDEVICE, XI_SKILL_ELE_MULTI_METEOSHOWER02, v3Pos);
					if (pSfx)
						pSfx->SetScale(D3DXVECTOR3(0.8f, 0.8f, 0.8f));

#endif //__CLIENT

				}
				else
					if (m_pActMover->m_nMotionHitCount == 3)
					{
#ifdef __WORLDSERVER
						GetWorld()->SendDamageAround(&(pHitObj->GetPos()), AF_GENERIC, this, OBJTYPE_PLAYER, II_WEA_MOB_MONSTER4_ATK2
							, 5.0f);
#endif //__WORLDSERVER

					}
	}
	break;
	}
}

#if __VER >= 10 // __Y_DRAGON_FIRE
void	CMover::OnAttackMelee_Meteonyker(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);

	CModelObject* pModel = (CModelObject*)m_pModel;

#ifdef __WORLDSERVER
	UpdateMatrix();
	pModel->UpdateMatrixBone();
#endif // WorldServer

	switch (dwState)
	{
	case OBJSTA_ATK1:
	{
		if (IsValidObj(pHitObj))
		{
			OnDamage(nItemID, dwState, pHitObj);
		}
	}
	break;
	case OBJSTA_ATK3:
	{
		if (m_pActMover->m_nMotionHitCount == 0)
		{
			CMover* pTarget = pHitObj;

			D3DXVECTOR3 vPos;
			CModelObject* pModel = (CModelObject*)m_pModel;
			pModel->GetEventPos(&vPos, 0);
			D3DXVec3TransformCoord(&vPos, &vPos, GetMatrixWorldPtr());

			if (IsValidObj(pTarget))
			{
				OnDamage(nItemID, dwState, pTarget, &pTarget->GetPos());
				ItemProp* pSkillProp;
				AddSkillProp* pAddSkillProp;
				BOOL bFind = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_STUNSINGLE, 5, "SI_GEN_STUNSINGLE");
				if (bFind)
				{
					if (xRand() % 2)
						DoApplySkill(pTarget, pSkillProp, pAddSkillProp, false);
				}

#ifdef __CLIENT
				CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1RANBALL, vPos, GetId(), pTarget->GetPos(), pTarget->GetId(), -1);
				if (pSfx)
				{
					pSfx->SetScale(D3DXVECTOR3(4.0f, 4.0f, 4.0f));
					{
						FLOAT fAngXZ, fAngH;
						D3DXVECTOR3 vDist = pTarget->GetPos() - vPos;
						xGetDegree(&fAngXZ, &fAngH, vDist);
						((CSfxAtkStraight*)pSfx)->ShootSfx(fAngXZ, -fAngH, 2.0f, XI_NPCSP1RANBOOM);
					}
				}
#endif	// __CLIENT
			}
		}
	}
	break;
	case OBJSTA_ATK4:
	{
		if (m_pActMover->m_nMotionHitCount == 0)
		{

			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_BURN, 1, "OnAttackMelee_Meteonyker");
			if (bSuccess)
			{
				D3DXVECTOR3 vPos = GetPos();
				ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false, FALSE, 10.0F);
				MoverProp* pProp = GetProp();
				OnDamage(pProp->dwAtk4, OBJSTA_ATK4, NULL, &vPos);
#ifdef __CLIENT
				CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCMETEONYKER, vPos);
#endif	// __CLIENT
			}
		}
	}
	break;
	}
}
#endif //__Y_DRAGON_FIRE

void	CMover::OnAttackMelee_Bear(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);

	CModelObject* pModel = (CModelObject*)m_pModel;

#ifdef __WORLDSERVER
	UpdateMatrix();
	pModel->UpdateMatrixBone();
#endif // WorldServer

	switch (dwState)
	{
	case OBJSTA_ATK1:
	{
#ifdef __WORLDSERVER
		ItemProp* pSkillProp;
		AddSkillProp* pAddSkillProp;
		BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_ATKSPELL_RATEGROUP, 5, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);

		OnDamage(nItemID, dwState, pHitObj);
#endif // WorldServer
	}
	break;
	case OBJSTA_ATK2:
	{
#ifdef __WORLDSERVER
		OnDamage(nItemID, dwState, pHitObj);
#endif // WorldServer
	}
	break;
	case OBJSTA_ATK3:
	{
		if (m_pActMover->m_nMotionHitCount == 0)
		{
#ifdef __CLIENT
			CSfx* pSfx = CreateSfx(D3DDEVICE, XI_SKILL_CIRCLE_DUST, GetPos());
			if (pSfx)
				pSfx->SetScale(D3DXVECTOR3(12.0f, 12.0f, 12.0f));

			PLAYSND(SND_PC_SKILLD_HITOFPENYA);//, &v2 );
#endif //__CLIENT

#ifdef __WORLDSERVER
			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_KNOCK_BACK, 4, "OnAttackMelee_BigMuscle");
			if (bSuccess)
				ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);
#endif //__WORLDSERVER
		}
		else
			if (m_pActMover->m_nMotionHitCount == 1)
			{
#ifdef __CLIENT
				PLAYSND(SND_PC_BOSS_STUN);
#endif //__CLIENT

#ifdef __WORLDSERVER
				ItemProp* pSkillProp;
				AddSkillProp* pAddSkillProp;
				BOOL bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_GEN_STUNGROUP, 5, "OnAttackMelee_Bear");
				if (bSuccess)
					ApplySkillAround(this, OBJTYPE_PLAYER, pSkillProp, pAddSkillProp, false);
#endif //__WORLDSERVER
			}
	}
	break;
	case OBJSTA_ATK4:
	{
#ifdef __WORLDSERVER
		ItemProp* pSkillProp;
		AddSkillProp* pAddSkillProp;
		BOOL bSuccess;


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_HEAL_HEALING, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_MUSHMOOT_BERSERK, 1, "OnAttackMelee_Bear");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_MUSHMOOT_WOODENSKIN, 1, "OnAttackMelee_Bear");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_HEAPUP, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_CANNONBALL, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_MENTALSIGN, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_BEEFUP, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);



		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_CATSREFLEX, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_ACCURACY, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);


		bSuccess = GetSkillProp(&pSkillProp, &pAddSkillProp, SI_ASS_CHEER_HASTE, 1, "OnAttackMelee_BigMuscle");
		if (bSuccess)
			DoApplySkill(this, pSkillProp, pAddSkillProp, false);



		D3DXVECTOR3 vScale = GetScale();

		vScale *= 1.3f;

		if (vScale.x < 10.0f)
		{
			SetScale(vScale);
			g_UserMng.AddSetScale(this, (vScale.x / m_pModel->m_pModelElem->m_fScale));
		}

#endif //__WORLDSERVER
	}
	break;
	}
}


BOOL	CMover::OnDamage(int nItemID, DWORD dwState, CMover* pHitObj, const D3DXVECTOR3* pvDamagePos, DWORD dwAtkFlag)
{
	int nParam = 0x01;		//sun

	if (IsPlayer() && IsDualWeapon())
	{
		if (dwAtkFlag & AF_GENERIC)
		{
			switch (dwState)
			{
			case OBJSTA_ATK1:	nParam |= 0x01;		break;
			case OBJSTA_ATK2:	nParam |= 0x02;		break;
			case OBJSTA_ATK3:	nParam |= 0x01;		break;
			case OBJSTA_ATK4:	nParam |= 0x03;		break;
			}
		}
	}


#if defined(__CLIENT)
	if (IsValidObj(pHitObj))
	{
		CModelObject* pHitModel = (CModelObject*)pHitObj->m_pModel;
		if (pHitModel)
		{
			D3DXVECTOR3 vDir;
			pHitModel->m_SparkInfo.m_bUsed = TRUE;
			pHitModel->m_SparkInfo.m_v3Color.x = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.y = 2.0f;
			pHitModel->m_SparkInfo.m_v3Color.z = 2.0f;
			pHitModel->m_SparkInfo.m_nCnt = 0;
			pHitModel->m_SparkInfo.m_fLerp = 1.0f;

			vDir = pHitObj->GetPos() - GetPos();
			D3DXVec3Normalize(&pHitModel->m_SparkInfo.m_v3SparkDir, &vDir);
		}
	}
#endif //defined(__CLIENT)

	int	nDamage = 0;
	if (IsPlayer())
	{
		nDamage = pHitObj->m_pActMover->SendDamage(dwAtkFlag, GetId(), nParam);
	}
	else
	{
		ItemProp* pItemProp = prj.GetItemProp(nItemID);
		if (pItemProp)
		{
			if (pItemProp->dwSpellRegion == SRO_AROUND)
			{
				int nType = OBJTYPE_PLAYER;
				if (pHitObj)
					pHitObj->SendDamageAround(dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange, 1.0f, 1.0f);
				else
					GetWorld()->SendDamageAround(pvDamagePos, dwAtkFlag, this, nType, nItemID, (float)pItemProp->_dwSkillRange);
			}
			else
			{
				if (pHitObj)
					nDamage = pHitObj->m_pActMover->SendDamage(dwAtkFlag, GetId(), 1);
			}
		}
		else
		{
			if (pHitObj)
				nDamage = pHitObj->m_pActMover->SendDamage(dwAtkFlag, GetId(), 1);
		}
	}


#ifdef __WORLDSERVER
	if (nDamage > 0)
	{
		if (pHitObj->m_pActMover->GetState() & OBJSTA_ATK_MELEESKILL)
		{
			DWORD dwSkill = pHitObj->GetActParam(0);
			if (dwSkill == SI_MER_ONE_REFLEXHIT)
			{
				pHitObj->m_nReflexDmg += nDamage;
			}
		}
	}
#endif
	return nDamage > 0;
}



BOOL	CMover::OnAttackMelee(DWORD dwState, CMover* pHitObj)
{
	int		nItemID = GetActParam(1);

#ifdef __WORLDSERVER
	PostAIMsg(AIMSG_ATTACK_MELEE);
#endif

	if (m_dwIndex == MI_CLOCKWORK1)
	{
		OnAttackMelee_ClockWorks(dwState, pHitObj);
		return TRUE;
	}

	if (m_dwIndex == MI_BIGMUSCLE)
	{
		OnAttackMelee_BigMuscle(dwState, pHitObj);
		m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}

	if (m_dwIndex == MI_KRRR)
	{
		OnAttackMelee_Krrr(dwState, pHitObj);
		m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}

	if (m_dwIndex == MI_MUSHMOOT)
	{
		OnAttackMelee_Bear(dwState, pHitObj);
		m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}

	if (m_dwIndex == MI_DU_METEONYKER || m_dwIndex == MI_DU_METEONYKER2 || m_dwIndex == MI_DU_METEONYKER3 ||
		m_dwIndex == MI_DU_METEONYKER4)
	{
		OnAttackMelee_Meteonyker(dwState, pHitObj);
		m_pActMover->m_nMotionHitCount++;
		return TRUE;
	}

#ifdef __WORLDSERVER
	if (IsNPC() && CMonsterSkill::GetInstance()->ApplySkill(this, pHitObj, ATK_MELEE))
		return TRUE;
#endif // __WORLDSERVER

	BOOL bSuccess = FALSE;
	BOOL bDamage = TRUE;

#ifdef __WORLDSERVER
	if (IsNPC() && pHitObj->IsPlayer() && pHitObj->IsRangeObj(this, 10.0f) == FALSE)
		bDamage = FALSE;
#endif
	if (bDamage)
	{
		const DWORD attackFlag = GetAct() == OBJACT_MELEE_ATTACK ? GetActParam(3) : AF_MISS;
		bSuccess = OnDamage(nItemID, dwState, pHitObj, NULL, attackFlag);
	}

#ifdef __WORLDSERVER
	if (bSuccess)
	{
		if (IsPlayer())
		{
			if (dwState == OBJSTA_ATK1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100)
				((CUser*)this)->m_playTaskBar.m_nActionPoint++;
		}
	}
#endif	// worldserver

#ifdef __CLIENT
	if (bSuccess)
	{
		if (IsActiveMover())
		{
			if (dwState == OBJSTA_ATK1 && g_WndMng.m_pWndTaskBar->m_nActionPoint < 100)
				g_WndMng.m_pWndTaskBar->m_nActionPoint++;

			CModelObject* pModel = (CModelObject*)m_pModel;
			if (pModel)
				pModel->m_nPause = 5;
		}

		ItemProp* pItemProp = GetActiveHandItemProp();
		if (pItemProp)
			PLAYSND(pItemProp->dwSndAttack1, &GetPos());

		CModelObject* pModel = (CModelObject*)m_pModel;
		if (pModel)
		{
			if (IsValidObj(pHitObj))
			{
				pModel->m_SparkInfo.m_bUsed = TRUE;
				pModel->m_SparkInfo.m_v3Color.x = 2.0f;
				pModel->m_SparkInfo.m_v3Color.y = 2.0f;
				pModel->m_SparkInfo.m_v3Color.z = 2.0f;
				pModel->m_SparkInfo.m_nCnt = 0;
				pModel->m_SparkInfo.m_fLerp = 1.0f;

				D3DXVECTOR3 vDir = GetPos() - pHitObj->GetPos();
				D3DXVec3Normalize(&pModel->m_SparkInfo.m_v3SparkDir, &vDir);
			}
		}
	}
#endif // client

	return TRUE;
}


BOOL	CMover::OnAttackMeleeContinue(DWORD dwState)
{
	int		nItemID = GetActParam(1);

	if (m_dwIndex == MI_CLOCKWORK1)
	{
#ifdef __WORLDSERVER
		UpdateMatrix();
		CModelObject* pModel = (CModelObject*)m_pModel;
		pModel->UpdateMatrixBone();
#endif // WorldServer

		switch (dwState)
		{
		case OBJSTA_ATK3:
		{
			D3DXVECTOR3 v1, v2;
			CModelObject* pModel = (CModelObject*)m_pModel;
			pModel->GetEventPos(&v1, 2);
			D3DXVec3TransformCoord(&v1, &v1, GetMatrixWorldPtr());

			v2 = D3DXVECTOR3(0, 0, 40.0f);
			D3DXVec3TransformCoord(&v2, &v2, pModel->GetMatrixBone(6));
			D3DXVec3TransformCoord(&v2, &v2, GetMatrixWorldPtr());

#ifdef __CLIENT
			extern CPartsLaser g_Laser;
			g_Laser.SetPos(v1, v2, 0.5f);
#endif

#ifdef __CLIENT
			if ((m_pActMover->m_nCount & 1) == 0)
#else
			if ((m_pActMover->m_nCount & 3) == 0)
#endif
			{
				CWorld* pWorld = GetWorld();
				D3DXVECTOR3 vOut;
				if (pWorld->IntersectObjLine(&vOut, v1, v2, FALSE, TRUE, FALSE))
				{
					OnDamage(nItemID, dwState, NULL, &vOut);

#ifdef __CLIENT
					CSfx* pSfx = CreateSfx(D3DDEVICE, XI_NPCSP1RANSPARK, vOut);
					if (pSfx)
						pSfx->SetScale(D3DXVECTOR3(2.0f, 2.0f, 2.0f));
#endif // __CLIENT
				}

			}
		}
		break;
		}
	}
#if __VER >= 10 // __Y_DRAGON_FIRE
	else
		if (m_dwIndex == MI_DU_METEONYKER || m_dwIndex == MI_DU_METEONYKER2 || m_dwIndex == MI_DU_METEONYKER3 ||
			m_dwIndex == MI_DU_METEONYKER4)
		{
#ifdef __WORLDSERVER
			UpdateMatrix();
			CModelObject* pModel = (CModelObject*)m_pModel;
			pModel->UpdateMatrixBone();
#endif // WorldServer

			switch (dwState)
			{
			case OBJSTA_ATK2:
			{
				if (m_pActMover->m_nMotionHitCount == 1)
				{
					D3DXVECTOR3 v1, v2, v3;
					CModelObject* pModel = (CModelObject*)m_pModel;
					pModel->GetEventPos(&v1, 0);
					pModel->GetEventPos(&v3, 1);
					D3DXVec3TransformCoord(&v1, &v1, GetMatrixWorldPtr());
					D3DXVec3TransformCoord(&v3, &v3, GetMatrixWorldPtr());

					v2 = D3DXVECTOR3(0, 0, 40.0f);
					D3DXVec3TransformCoord(&v2, &v2, pModel->GetMatrixBone(6));
					D3DXVec3TransformCoord(&v2, &v2, GetMatrixWorldPtr());

#ifdef __CLIENT
					D3DXVECTOR3 vDir = v3 - v1;
					D3DXVec3Normalize(&vDir, &vDir);
					D3DXVECTOR3 vTemp;
					D3DXVECTOR3 vScal = D3DXVECTOR3(0.3f, 0.3f, 0.3f);
					D3DXVECTOR3 vModelScal = GetScale();

					vScal.x *= vModelScal.x;
					vScal.y *= vModelScal.y;
					vScal.z *= vModelScal.z;

					for (int i = 0; i < 2; i++)
					{
						vTemp = vDir * (xRandomF(0.2f) + 0.1f);
						vTemp *= 0.8f;
						vTemp.y -= (xRandomF(0.05f) + 0.01f);

						extern CPartsFireDragon	g_FireDragon;
						g_FireDragon.Create(D3DDEVICE, v1, XI_NAT_FIRE01_ADV, vScal, vTemp);
					}

#endif

#ifdef __CLIENT
					if ((m_pActMover->m_nCount & 1) == 0)
#else
					if ((m_pActMover->m_nCount & 3) == 0)
#endif
					{
						CWorld* pWorld = GetWorld();
						D3DXVECTOR3 vOut = GetPos();
						//						if( pWorld->IntersectObjLine( &vOut, v1, v2, FALSE, TRUE, FALSE ) )
						{
							OnDamage(nItemID, dwState, NULL, &vOut);
							/*
												#ifdef __CLIENT
													CSfx *pSfx = CreateSfx( D3DDEVICE,  XI_NPCSP1RANSPARK, vOut );
													if( pSfx )
														pSfx->SetScale( D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
												#endif // __CLIENT
							*/
						}

					}
				}
				break;
			}
			}
		}
#endif //__Y_DRAGON_FIRE

	return TRUE;
}

BOOL	CMover::OnAttackSP()
{
	int		idTarget = GetActParam(0);
	int		nItemID = GetActParam(1);

	CMover* pHit = prj.GetMover(idTarget);
	if (IsInvalidObj(pHit))
		return FALSE;

	ItemProp* pItemProp;
	if (nItemID == NULL_ID || nItemID == 0)
		pItemProp = GetActiveHandItemProp();
	else
		pItemProp = prj.GetItemProp(nItemID);

	if (pItemProp == NULL)
	{
		Error("%s onattacksp err %d", m_szName, nItemID);
		return FALSE;
	}

	if (pItemProp->dwSpellRegion == SRO_AROUND)
		SendDamageAround(AF_GENERIC, this, OBJTYPE_PLAYER, nItemID, (float)(pItemProp->_dwSkillRange), 0.0, 1.0f);
	else
	{
#ifdef __CLIENT
		if (pHit->IsLive())
			pHit->m_pActMover->SendDamage(AF_MONSTER_SP_CLIENT, GetId(), (nItemID << 16));
#else
		if (pHit->IsLive())
			pHit->m_pActMover->SendDamage(AF_GENERIC, GetId(), (nItemID << 16));
#endif
	}

	return TRUE;
}


void	CMover::OnActDrop()
{
#ifdef __WORLDSERVER
	if (m_pActMover->GetDmgState() & OBJSTA_DMG_FLY_ALL)
		return;

	if (FALSE == IsPlayer())
	{
		if (GetSpeedFactor() < 1.9F
			&& m_dwAIInterface != AII_PET
			)
		{
			ClearDestPos();
			SendActMsg(OBJMSG_STAND);
			g_UserMng.AddMoverBehavior(this);
		}
	}
#else	// __WORLDSERVER

#endif	// not __WORLDSERVER
}

void	CMover::OnActCollision()
{
#ifdef __WORLDSERVER
	if (FALSE == IsPlayer())
	{
		if (GetSpeedFactor() < 1.9F && m_dwAIInterface != AII_PET)
		{
			ClearDest();
			SendActMsg(OBJMSG_STAND);
			g_UserMng.AddMoverBehavior(this);
			PostAIMsg(AIMSG_COLLISION);
		}
	}
#else	// __WORLDSERVER
	if (TRUE == IsActiveMover())
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
			pWndWorld->m_bLButtonDown = FALSE;
	}
#endif	// __WORLDSERVER
}

void	CMover::OnActEndMeleeSkill()
{
	ClearActParam();

#ifdef __WORLDSERVER
	if (IsPlayer())
	{
		if (((CUser*)this)->m_playTaskBar.m_nUsedSkillQueue != -1)
			((CUser*)this)->m_playTaskBar.SetNextSkill((CUser*)this);
	}
#endif

}


void	CMover::OnActEndMagicSkill()
{
	ClearActParam();

#ifdef __WORLDSERVER
	if (IsPlayer())
	{
		if (((CUser*)this)->m_playTaskBar.m_nUsedSkillQueue != -1)
			((CUser*)this)->m_playTaskBar.SetNextSkill((CUser*)this);
	}
#endif
}


void	CMover::OnActEndJump4()
{
	if (FALSE == IsEmptyDestPos())
		SetDestPos(GetDestPos(), m_bForward, FALSE);

	if (m_dwIndex == MI_CLOCKWORK1)
	{
		SendActMsg(OBJMSG_APPEAR);
	}
}


void	CMover::OnActLanding()
{
#ifdef __CLIENT
	if (m_dwIndex == MI_CLOCKWORK1)
	{
		GetWorld()->m_pCamera->SetQuake(60, 1.0f);
	}
#endif
}


void	CMover::OnActFMove()
{
#ifdef __WORLDSERVER
	PostAIMsg(AIMSG_BEGINMOVE);
#endif
}


BOOL CMover::IsInvisibleSkill(DWORD dwSkillID)
{
	if (dwSkillID == SI_ACR_SUP_DARKILLUSION)
		return TRUE;

	return FALSE;
}

void	CMover::RemoveInvisibleSkill()
{
#ifdef __WORLDSERVER
	RemoveBuff(BUFF_SKILL, SI_ACR_SUP_DARKILLUSION);
#endif // __WORLDSERVER
}


#ifdef __WORLDSERVER
BOOL CMover::IsLoot(CItem* pItem, BOOL bPet)
{
	BOOL bTake = FALSE;

	if (pItem->IsDelete() || (pItem->m_idHolder != 0 && pItem->m_idHolder != m_idPlayer))
	{
		return bTake;
	}

	if (pItem->m_idOwn != NULL_ID)
	{

		CMover* pOther = prj.GetMover(pItem->m_idOwn);
		if (IsValidObj(pOther))
		{
			if (m_idparty && (pOther->m_idparty == m_idparty))
				bTake = TRUE;
		}
		else
			bTake = TRUE;

		if (pItem->m_idOwn == GetId())
			bTake = TRUE;

	}
	else
		bTake = TRUE;

	DWORD dwTime = g_tmCurrent;
#ifdef __EVENT_MONSTER
	DWORD dwMonsterId = pItem->m_IdEventMonster;
	if (dwMonsterId != NULL_ID && CEventMonster::GetInstance()->SetEventMonster(dwMonsterId))
	{
		DWORD dwLootTime = CEventMonster::GetInstance()->GetLootTime();
		if ((dwTime - pItem->m_dwDropTime) >= (DWORD)(SEC(dwLootTime)))
			bTake = TRUE;

		if (bPet && !CEventMonster::GetInstance()->IsPetAble())
			bTake = FALSE;
	}
	else
#endif // __EVENT_MONSTER
		if ((dwTime - pItem->m_dwDropTime) >= SEC(7))
			bTake = TRUE;

	if (bPet && bTake)
	{
		if (0 < ((CCtrl*)pItem)->m_lRespawn)
			bTake = FALSE;
#ifdef __JEFF_11
		CItemElem* pItemElem = pItem->m_pItemBase;
		ItemProp* pItemProp = pItemElem->GetProp();
#ifndef __FL_PET_FILTER
		if (pItemProp->dwItemKind3 != IK3_GOLD && m_Inventory.IsFull(pItemElem, pItemProp, pItemElem->m_nItemNum))
			bTake = FALSE;
#else
		if (pItemProp->dwItemKind3 != IK3_GOLD)
		{
			if (!IsPlayer())
			{
				if (m_Inventory.IsFull(pItemElem, pItemProp, pItemElem->m_nItemNum))
					bTake = FALSE;
			}
			else
			{
				CUser* pUser = (CUser*)this;
				CItemContainer<CItemElem>* m_pContainer = GetContainerByType(pUser->m_nLootPlace);
				if (m_pContainer)
				{
					if (m_pContainer->IsFull(pItemElem, pItemProp, pItemElem->m_nItemNum))
						bTake = FALSE;
				}
				else
					bTake = FALSE;
			}
		}
#endif

#endif	// __JEFF_11
	}

	return bTake;
}
#endif // WorldServer


BOOL CMover::SubLootDropMobSingle(CItem* pItem)
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CItemElem* pItemBase = pItem->m_pItemBase;

#ifndef __FL_PET_FILTER 
	bSuccess = CreateItempItemBase;
#else
	if (!IsPlayer())
		bSuccess = CreateItem(pItemBase);
	else
		bSuccess = CreateItem(pItemBase, ((CUser*)this)->m_nLootPlace);
#endif

	if (IsPlayer() && !bSuccess)
		((CUser*)this)->AddDefinedText(TID_GAME_LACKSPACE);

#ifndef __FL_FIX_USELESS_LOGS
	if (pItemBase->IsLogable() && bSuccess)
	{
		switch (pItemBase->GetProp()->dwItemKind2)
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
		{
			if (pItemBase->m_nItemNum > 9)
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			}
			break;
		}
		default:
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "R";
			aLogItem.SendName = "GROUND";
			aLogItem.RecvName = GetName();
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = GetGold();
			g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			break;
		}
		}
	}
#endif

#endif // WorldServer
	return bSuccess;
} // SubLootDropMobSingle


BOOL CMover::SubLootDropMobParty(CItem* pItem, CParty* pParty)
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CUser* pListMember[8];
	int	nMaxListMember = 0;
	CUser* pMember = NULL;
	CUser* pGetUser = NULL;
	//	float fDist;
	D3DXVECTOR3	vDist;
	CItemElem* pItemBase = pItem->m_pItemBase;


	memset(pListMember, 0, sizeof(pListMember));
	for (int i = 0; i < pParty->m_nSizeofMember; i++)
	{
		pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
		if (IsValidArea(pMember, 32.0f))
			pListMember[nMaxListMember++] = pMember;
	}

	if (nMaxListMember == 0)
	{
		pGetUser = (CUser*)this;
	}
	else
	{
		switch (pParty->m_nTroupeShareItem)
		{
		case 1:

		{
			for (int i = 0; i < nMaxListMember; i++)
			{
				if (pParty->m_nGetItemPlayerId == pListMember[i]->m_idPlayer)
				{
					if (i + 1 >= nMaxListMember)
					{
						pGetUser = pListMember[0];
						break;
					}
					else
					{
						pGetUser = pListMember[i + 1];
						break;
					}
				}
			}

			if (pGetUser == NULL)
			{
				pGetUser = pListMember[0];
			}
		}
		break;
		case 2:
		{
			if (pParty->IsLeader(pListMember[0]->m_idPlayer))
			{
				pGetUser = pListMember[0];
			}
			else
			{
				pGetUser = (CUser*)this;
			}
		}
		break;
		case 3:
		{
			DWORD dwRand = xRandom(nMaxListMember);
			pGetUser = pListMember[dwRand];
		}
		break;
		default:

		{
			pGetUser = (CUser*)this;
		}
		break;
		}	// switch
	}
	pParty->m_nGetItemPlayerId = pGetUser->m_idPlayer;

#ifndef __FL_PET_FILTER
	bSuccess = pGetUser->CreateItempItemBase;
#else
	bSuccess = pGetUser->CreateItem(pItemBase, pGetUser->m_nLootPlace);
#endif

	for (int i = 0; i < nMaxListMember; i++)
	{
		pMember = pListMember[i];
		if (TRUE == pMember->IsPlayer() && pItemBase->GetProp() != NULL)
		{
			if (bSuccess)
			{
				if (pMember->m_idPlayer != pGetUser->m_idPlayer)
				{
					CString strFormat;
					strFormat.Format("%s %s: %s (x%d).", pGetUser->GetName(), prj.GetText(TID_GAME_TROUPEAUTOSHARE), pItemBase->GetProp()->szName, pItemBase->m_nItemNum);
					((CUser*)pMember)->AddText(strFormat, 0xffccffcc);
				}
			}
			else
			{
				if (pGetUser == pMember)
					((CUser*)pGetUser)->AddDefinedText(TID_GAME_LACKSPACE);
			}
		}
	}

#ifndef __FL_FIX_USELESS_LOGS
	if (pItemBase->IsLogable() && bSuccess)
	{
		switch (pItemBase->GetProp()->dwItemKind2)
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
		{
			if ((pItemBase->m_nItemNum) > 9)
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = pGetUser->GetName();
				aLogItem.WorldId = pGetUser->GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
				g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			}
			break;
		}
		default:
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "R";
			aLogItem.SendName = "GROUND";
			aLogItem.RecvName = pGetUser->GetName();
			aLogItem.WorldId = pGetUser->GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = pGetUser->GetGold();
			g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			break;
		}
		}
	}
#endif

#endif // __WORLDSERVER
	return bSuccess;
} // SubLootDropMobParty




BOOL CMover::SubLootDropMob(CItem* pItem)
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CParty* pParty = g_PartyMng.GetParty(m_idparty);
	if (pParty && pParty->IsMember(m_idPlayer))
	{

		bSuccess = SubLootDropMobParty(pItem, pParty);
	}
	else
	{
		bSuccess = SubLootDropMobSingle(pItem);
	}
#endif	// __WORLDSERVER

	return bSuccess;
}


BOOL CMover::SubLootDropNotMob(CItem* pItem)
{
	BOOL bSuccess = TRUE;
#ifdef __WORLDSERVER
	CItemElem* pItemBase = pItem->m_pItemBase;

#ifndef __FL_PET_FILTER
	bSuccess = CreateItempItemBase;
#else
	if (!IsPlayer())
		bSuccess = CreateItem(pItemBase);
	else
		bSuccess = CreateItem(pItemBase, ((CUser*)this)->m_nLootPlace);
#endif

	if (IsPlayer() && !bSuccess)
		((CUser*)this)->AddDefinedText(TID_GAME_LACKSPACE);

#ifndef __FL_FIX_USELESS_LOGS
	if (pItemBase->IsLogable() && bSuccess)
	{
		switch (pItemBase->GetProp()->dwItemKind2)
		{
		case IK2_FOOD:
		case IK2_MAGIC:
		case IK2_POTION:
		case IK2_REFRESHER:
		{
			if (pItemBase->m_nItemNum > 9)
			{
				LogItemInfo aLogItem;
				aLogItem.Action = "R";
				aLogItem.SendName = "GROUND";
				aLogItem.RecvName = GetName();
				aLogItem.WorldId = GetWorld()->GetID();
				aLogItem.Gold = aLogItem.Gold2 = GetGold();
				g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			}
			break;
		}
		default:
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "R";
			aLogItem.SendName = "GROUND";
			aLogItem.RecvName = GetName();
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = aLogItem.Gold2 = GetGold();
			g_DPSrvr.OnLogItem(aLogItem, pItemBase, pItemBase->m_nItemNum);
			break;
		}
		}
	}
#endif

#endif // __WORLDSERVER

	return bSuccess;
} // SubLootDropNotMob

////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
BOOL CMover::DoLoot(CItem* pItem)
{
	if (IsPlayer() == FALSE)
		return FALSE;

	CItemElem* pItemBase = pItem->m_pItemBase;
	BOOL bSuccess = TRUE;

	ItemProp* pItemProp = pItem->GetProp();
	if (pItemProp->dwItemKind1 == IK1_GOLD)
	{
		PickupGold(pItemBase->GetGold(), pItem->m_bDropMob);
	}
	else
	{

#ifdef __WORLDSERVER
		if (pItem->m_bDropMob == TRUE)
		{

			bSuccess = SubLootDropMob(pItem);
		}
		else
		{

			bSuccess = SubLootDropNotMob(pItem);

		}
#endif	// __WORLDSERVER
	}

	if (TRUE == bSuccess)
		pItem->Delete();

	return bSuccess;
}

