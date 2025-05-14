#include "stdafx.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "defineText.h"
#include "ModelObject.h"
#include "CreateObj.h"
#include "defineobj.h"
#include "eveschool.h"
#include "party.h"

#include "definequest.h"

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __CLIENT
#include "DPClient.h"
#else	// __CLIENT
#include "..\_aiinterface\AIPet.h"
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#ifdef __SYS_TICKET
#include "ticket.h"
#endif	// __SYS_TICKET
#if __VER >= 15 // __PETVIS
#include "ItemUpgrade.h"
#endif  // __PETVIS
#endif	// __CLIENT

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13
#include "honor.h"
#endif

#ifdef __CLIENT
extern	CDPClient			g_DPlay;
#else
extern	CUserMng			g_UserMng;
extern	CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE
#endif

#ifdef __WORLDSERVER
#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif
#endif

extern	CPartyMng				g_PartyMng;
extern	CGuildCombat			g_GuildCombatMng;

extern void __SetQuest(DWORD dwIdMover, int nQuest);

#ifdef __WORLDSERVER

#if __VER >= 10
void	CMover::SetMasterSkillPointUp()
{
	if (m_nLevel != 72 && m_nLevel != 84 && m_nLevel != 96 && m_nLevel != 108)
		return;
	if (IsHero())
		return;
	if (IsLegendHero())
		return;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill = &(m_aJobSkill[i]);
		if (lpSkill && lpSkill->dwSkill != NULL_ID)
		{
			ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
			if (pSkillProp == NULL)
				continue;
			if (pSkillProp->dwItemKind1 != JTYPE_MASTER)
				continue;
			lpSkill->dwLevel++;
		}
	}
	g_UserMng.AddCreateSfxObj(this, XI_SYS_EXCHAN01, GetPos().x, GetPos().y, GetPos().z);

	CUser* pUser = (CUser*)this;
	pUser->AddDoUseSkillPoint(&m_aJobSkill[0], m_nSkillPoint);
}
#endif

int	CMover::GetQueueCastingTime()
{
	if (IsNPC())
		return 0;

	CUser* pUser = (CUser*)this;
	SHORTCUT* pSlotQueue = pUser->m_playTaskBar.m_aSlotQueue;
	int		nTotalTime = 0;

	for (int i = 0; i < MAX_SLOT_QUEUE; i++)
	{
		if (pSlotQueue[i].m_dwShortcut)
		{
			LPSKILL pSkill = GetSkill(0, pSlotQueue[i].m_dwId);
			if (pSkill == NULL)
				continue;

			ItemProp* pSkillProp = pSkill->GetProp();
			if (!(pSkillProp))
			{
				Error("%s. 스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill);
				return FALSE;
			}

			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, pSkill->dwLevel);
			if (!(pAddSkillProp))
			{
				Error("%s. 애드스킬(%d)의 프로퍼티가 없다.", m_szName, pSkill->dwSkill);
				return FALSE;
			}

			nTotalTime += (int)pAddSkillProp->dwCastingTime;
		}
	}

	nTotalTime = (int)(nTotalTime * 0.7f);
	nTotalTime = (int)((nTotalTime / 1000.0f) * SEC1);
	nTotalTime = GetCastingTime(nTotalTime);

	return nTotalTime;
}

#endif // worldserver

#ifdef __WORLDSERVER
BOOL CMover::DoUseObject(OBJID idFocusObj)
{
	if (!IsPlayer())
		return FALSE;

	CUser* pUser = (CUser*)this;

	CCtrl* pObj = prj.GetCtrl(idFocusObj);
	if (!IsValidObj((CObj*)pObj))
		return FALSE;

	DWORD dwObjType = pObj->GetType();
	if (dwObjType != OT_ITEM && dwObjType != OT_CTRL)
		return FALSE;

	if (!pUser->IsRangeObj(pObj->GetPos(), 2.0f))
		return FALSE;

	if (dwObjType == OT_ITEM)
	{
		CItem* pItem = (CItem*)pObj;
		if (!pUser->IsLoot(pItem))
		{
			pUser->AddDefinedText(TID_GAME_PRIORITYITEMPER, "\"%s\"", pItem->GetProp()->szName);
			return FALSE;
		}

		if (!pUser->DoLoot(pItem))
			return FALSE;
	}
	else // OT_CTRL
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)pObj;
		if (!pCommonCtrl)
			return FALSE;

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM)
		{
			int nItemCount = pUser->GetAvailableItemNum(pCommonCtrl->m_CtrlElem.m_dwSetItem);
			if (nItemCount < (int)(pCommonCtrl->m_CtrlElem.m_dwSetItemCount))
			{
				pUser->AddDefinedText(TID_ITEMBOX_MESSAGE1, "\"%s\"", prj.GetItemProp(pCommonCtrl->m_CtrlElem.m_dwSetItem)->szName);
				return FALSE;
			}
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_LEVEL)
		{
			if (pUser->GetLevel() < (int)(pCommonCtrl->m_CtrlElem.m_dwSetLevel))
			{
				pUser->AddDefinedText(TID_ITEMBOX_MESSAGE2);
				return FALSE;
			}
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST)
		{
			LPQUEST pQuest = pUser->FindQuest(pCommonCtrl->m_CtrlElem.m_dwSetQuestNum);
			if (pQuest)
			{
				if (pQuest->m_nState != pCommonCtrl->m_CtrlElem.m_dwSetFlagNum)
				{
					QuestProp* pQuestPorp = prj.m_aPropQuest.GetAt(pQuest->m_wId);
					if (pQuestPorp)
						pUser->AddDefinedText(TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle);

					return FALSE;
				}
			}
			else
			{
				QuestProp* pQuestPorp = prj.m_aPropQuest.GetAt(pCommonCtrl->m_CtrlElem.m_dwSetQuestNum);
				if (pQuestPorp)
					pUser->AddDefinedText(TID_ITEMBOX_MESSAGE4, "\"%s\"", pQuestPorp->m_szTitle);

				return FALSE;
			}
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_CLASS)
		{
			if (pUser->GetJob() != pCommonCtrl->m_CtrlElem.m_bSetJob[pUser->GetJob()])
			{
				pUser->AddDefinedText(TID_ITEMBOX_MESSAGE5);
				return FALSE;
			}
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_GENDER)
		{
			if (pUser->GetSex() != pCommonCtrl->m_CtrlElem.m_dwSetGender)
			{
				pUser->AddDefinedText(TID_ITEMBOX_MESSAGE3);
				return FALSE;
			}
		}

		pCommonCtrl->m_nMoverID = pUser->GetId();

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_TELEPORT)
		{
			D3DXVECTOR3 vPos((float)(pCommonCtrl->m_CtrlElem.m_dwTeleX), (float)(pCommonCtrl->m_CtrlElem.m_dwTeleY), (float)(pCommonCtrl->m_CtrlElem.m_dwTeleZ));
			pUser->REPLACE(g_uIdofMulti, pCommonCtrl->m_CtrlElem.m_dwTeleWorldId, vPos, REPLACE_NORMAL, nTempLayer);
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_QUEST_END)
		{
			if (pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1 != 0)
			{
				LPQUEST pQuest = pUser->FindQuest(pCommonCtrl->m_CtrlElem.m_dwSetQuestNum1);
				if (pQuest)
				{
					QUEST quest;
					if (pUser->SetQuest(pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum1, &quest))
						pUser->AddSetQuest(&quest);
				}
			}

			if (pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2 != 0)
			{
				LPQUEST pQuest = pUser->FindQuest(pCommonCtrl->m_CtrlElem.m_dwSetQuestNum2);
				if (pQuest)
				{
					QUEST quest;
					if (pUser->SetQuest(pQuest->m_wId, pCommonCtrl->m_CtrlElem.m_dwSetFlagNum2, &quest))
						pUser->AddSetQuest(&quest);
				}
			}
		}

		if (pCommonCtrl->m_CtrlElem.m_dwSet & UA_ITEM)
			pUser->RemoveAvailableItem(pCommonCtrl->m_CtrlElem.m_dwSetItem, pCommonCtrl->m_CtrlElem.m_dwSetItemCount);

		pCommonCtrl->SetActionPlay();
	}

	g_UserMng.AddUseObject(pUser);
	return TRUE;
}
#endif

#ifdef __CLIENT
BOOL	CMover::DoUseSkill(int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime, DWORD dwSkill, DWORD dwLevel)
#else // __CLIENT
BOOL	CMover::DoUseSkill(int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl)
#endif // __CLIENT
{
	if (IsNPC())
		return FALSE;

#ifdef __WORLDSERVER
	DWORD dwSkill = 0, dwLevel;
#endif	// __WORLDSERVER

	if (nIdx >= 0)
	{
		LPSKILL pSkill = GetSkill(nType, nIdx);
		if (pSkill == NULL)
		{
			Error("CMover::DoUseSkill : %s는 nIdx에 스킬을 가지고 있지 않다. %d", m_szName, nIdx);
			return FALSE;
		}
		dwSkill = pSkill->dwSkill;
#ifdef __SKILL0517
		dwLevel = GetSkillLevel(pSkill);
#else	// __SKILL0517
		dwLevel = pSkill->dwLevel;
#endif	// __SKILL0517

		if (dwSkill == DWORD(-1))
			return FALSE;

		if (dwLevel <= 0)
			return FALSE;
	}

#ifdef __WORLDSERVER
	int nCastingTime = 0;
#endif

	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	GetSkillProp(&pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "DoUseSkill_P");

	if (pSkillProp == NULL)
		return FALSE;

	int nMotion = (int)pSkillProp->dwUseMotion;

	if (pSkillProp->dwSkillType == KT_MAGIC)
	{
		if (sutType == SUT_QUEUESTART)
		{
#ifdef __WORLDSERVER
			nCastingTime = GetQueueCastingTime();
#endif
		}
		else if (sutType == SUT_QUEUEING)
		{
#ifdef __WORLDSERVER
			nCastingTime = 0;
#endif
			nMotion += 2;
		}
		else
		{
#ifdef __WORLDSERVER

			nCastingTime = (int)((pAddSkillProp->dwCastingTime / 1000.0f) * SEC1);
			nCastingTime = GetCastingTime(nCastingTime);
#endif	// __WORLDSERVER
		}
	}
	else if (pSkillProp->dwSkillType == KT_SKILL)
	{
#ifdef __WORLDSERVER
		nCastingTime = 1;
#endif
	}

	BOOL bSuccess = DoUseSkill(dwSkill, dwLevel, idFocusObj, sutType, bControl, nCastingTime);
	return  bSuccess;
}

BOOL CMover::DoUseSkill(DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime)
{
	if (IsDie())
		return FALSE;
	if (m_pActMover->IsFly())
		return FALSE;
	if (m_dwFlag & MVRF_NOATTACK)
	{
#ifdef __FIX_STUNBUG
#ifdef __CLIENT
		if (IsPlayer())
			g_WndMng.PutString(prj.GetText(TID_GAME_STILLNOTUSE));
#else
		if (IsPlayer())
			((CUser*)this)->AddDefinedText(TID_GAME_STILLNOTUSE);
#endif
#endif
		return FALSE;
	}


#if defined(__WORLDSERVER)
	CWorld* pWorld = GetWorld();
	if (pWorld && pWorld->GetID() == WI_WORLD_GUILDWAR)
	{
		if (m_nGuildCombatState == 0)
		{
			if (IsPlayer())
				((CUser*)this)->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_STANDS_NOTUSESKILL));
			return FALSE;
		}
	}
#endif

	if (IsMode(NO_ATTACK_MODE))
		return FALSE;

	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	GetSkillProp(&pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoUseSkill");

#ifdef __CLIENT
	DWORD dwWorldID(GetWorld()->GetID());
	if (dwWorldID >= WI_WORLD_GUILDWAR1TO1_0 && dwWorldID <= WI_WORLD_GUILDWAR1TO1_L)
	{
		if (pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_NEVERKILLSTOP), NULL, prj.GetTextColor(TID_GAME_NEVERKILLSTOP));
			return FALSE;
		}
	}
#endif // __CLIENT

	CMover* pTarget = prj.GetMover(idFocusObj);
	if (IsInvalidObj(pTarget))
		return FALSE;

	if (pTarget->IsMode(TRANSPARENT_MODE) && pTarget != this)
		return FALSE;

	if (pSkillProp->dwID != SI_ASS_HEAL_RESURRECTION)
	{
		if (pTarget->IsDie())
			return FALSE;
	}
	else
	{
		if (!pTarget->IsDie())
			return FALSE;
	}


#ifdef __WORLDSERVER
	if (!CheckAttackDistance(pTarget, pSkillProp->dwAttackRange))
	{
		//if (IsPlayer())
			//((CUser*)this)->AddText("Not in range!", 0xffffffff);

		return FALSE;
	}

#ifdef __PK_PVP_SKILL_REGION
	if (pSkillProp->nEvildoing < 0)
	{
		int nAttackerPK, nDefenderPK;
		nAttackerPK = nDefenderPK = 0;
		if (IsPlayer() && pTarget->IsPlayer() && (this != pTarget))
		{
			if (!(IsGuildCombatTarget(pTarget)
#ifdef __JEFF_11_4
				|| IsArenaTarget(pTarget)
#endif	// __JEFF_11_4
#ifdef __FL_FFA
				|| (CFFAManager::GetInstance()->IsFFAEnjoyer(this) && CFFAManager::GetInstance()->IsFFAEnjoyer(pTarget))
#endif
#ifdef __FL_TDM
				|| CTDMManager::GetInstance()->IsCanKill(this, pTarget)
#endif
				))
			{
				((CUser*)this)->AddDefinedText(TID_GMAE_PK_NOT_AREA, "");
				return FALSE;
			}
		}
	}
#endif // __PK_PVP_SKILL_REGION
	if (HasBuff(BUFF_SKILL, SI_BLD_SUP_BERSERK))
		return FALSE;

#ifdef __NPC_BUFF
	if ((pSkillProp->dwID == SI_ASS_CHEER_QUICKSTEP && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_QUICKSTEP))
		|| (pSkillProp->dwID == SI_ASS_CHEER_HASTE && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_HASTE))
		|| (pSkillProp->dwID == SI_ASS_CHEER_HEAPUP && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_HEAPUP))
		|| (pSkillProp->dwID == SI_ASS_CHEER_ACCURACY && pTarget->HasBuff(BUFF_SKILL, SI_GEN_EVE_ACCURACY)))
	{
		if (pTarget->IsPlayer())
			((CUser*)pTarget)->AddDefinedText(TID_GAME_NPCBUFF_FAILED, "\"%s\"", pSkillProp->szName);
		return FALSE;
	}
#endif // __NPC_BUFF

	if (pSkillProp->dwID == SI_ACR_YOYO_SNITCH || pSkillProp->dwID == SI_JST_YOYO_VATALSTAB)
	{
		if (!HasBuff(BUFF_SKILL, SI_ACR_SUP_DARKILLUSION))
		{
			if (IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_REQ_DARK, "");
			return FALSE;
		}
	}

#endif //defined(__WORLDSERVER)

	MoverProp* pMoverProp = pTarget->GetProp();
	if (pMoverProp == NULL)
		Error("DoUseSkill : %s , 타겟 %s 의 프로퍼티가 없다", m_szName, pTarget->m_szName);

	if (pTarget != this)
	{
#ifdef __WORLDSERVER
		if (pSkillProp->dwUseChance == WUI_NOW)
			return FALSE;
		if (pSkillProp->dwExeTarget == EXT_SELFCHGPARAMET)
			return FALSE;

		if (pTarget->IsNPC() && pTarget->IsPeaceful())
			return FALSE;

		if (!g_eLocal.GetState(EVE_STEAL))
		{
			if (pSkillProp->nEvildoing < 0)
				if (IsSteal(pTarget))
					return FALSE;
		}

		if (pWorld && !pWorld->IsArena()
#ifdef __FL_FFA
			&& !pWorld->IsFFA()
#endif	
#ifdef __FL_TDM
			&& !pWorld->IsTDM()
#endif	
			)
		{
			if (pSkillProp->nEvildoing > 0)
			{
				HITTYPE hy;
				if ((hy = GetHitType2(pTarget, TRUE, TRUE)) != HITTYPE_FAIL)
				{
					if (IsPlayer())
						((CUser*)this)->AddDefinedText(TID_GAME_NEVERKILLSTOP, "");
					return FALSE;
				}
			}
		}

#endif
		SetAngle(GetDegree(pTarget->GetPos(), GetPos()));
	}


	if (GetWorld()->GetID() == WI_WORLD_GUILDWAR)
	{
		if (pSkillProp->dwID == SI_MAG_MAG_BLINKPOOL || pSkillProp->dwID == SI_RIG_HERO_RETURN)
			return FALSE;

		if (g_GuildCombatMng.m_nState != CGuildCombat::WAR_STATE)
		{
			if (pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION)
				return FALSE;
		}
	}

#ifdef __WORLDSERVER



	ItemProp* pItemProp = NULL;
	DWORD	dwItemKind3;

	if (pSkillProp->dwLinkKind == IK3_SHIELD
#if __VER >= 16
		|| pSkillProp->dwLinkKind == IK3_MAGICBARUNA
		|| pSkillProp->dwLinkKind == IK3_ZEMBARUNA
#endif
		)
	{
		CItemElem* pItemElem = GetEquipItem(PARTS_SHIELD);
		if (pItemElem)
		{
			pItemProp = pItemElem->GetProp();
			dwItemKind3 = pItemProp->dwItemKind3;
		}
		else
			dwItemKind3 = NULL_ID;
	}
	else
	{
		pItemProp = GetActiveHandItemProp();
		dwItemKind3 = pItemProp->dwItemKind3;
	}

	switch (pSkillProp->dwLinkKind)
	{
	case IK3_ENCHANTWEAPON:
	{
		if (IsPlayer())
		{
			BOOL bFail = FALSE;

			CItemElem* pItemElemR = GetWeaponItem();
			CItemElem* pItemElemL = GetLWeaponItem();

			if (pItemElemR && pItemElemL)
			{
				if ((pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP) &&
					(pItemElemL && pItemElemL->m_bItemResist == SAI79::NO_PROP))
					bFail = TRUE;
			}
			else if (pItemElemR && pItemElemL == NULL)
			{
				if (pItemElemR && pItemElemR->m_bItemResist == SAI79::NO_PROP)
					bFail = TRUE;
			}
			else if (pItemElemR == NULL && pItemElemL == NULL)
			{
				bFail = TRUE;
			}

			if (bFail)
			{
				((CUser*)this)->AddDefinedText(TID_SKILL_NOTELEWEAPON, "");
				return FALSE;
			}
		}
	}
	break;
	case IK3_MAGICBOTH:
		if (dwItemKind3 != IK3_WAND && dwItemKind3 != IK3_STAFF)
		{
			if (IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGITEM, "");
			return FALSE;
		}
		break;
	case IK3_YOBO:
		if (dwItemKind3 != IK3_YOYO && dwItemKind3 != IK3_BOW)
		{
			if (IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGITEM, "");
			return FALSE;
		}
		break;
	default:
		if (pSkillProp->dwLinkKind != NULL_ID && pSkillProp->dwLinkKind != dwItemKind3)
		{
			if (IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGITEM, "");
			return FALSE;
		}
		break;
	}

#if __VER >= 8
	if (pSkillProp->dwSkillType == BUFF_SKILL)
	{
		CMover* searchMover;
		if (pTarget == this)
			searchMover = this;
		else
			searchMover = pTarget;

#ifdef __BUFF_1107
		IBuff* pBuff = searchMover->m_buffs.GetBuff(BUFF_SKILL, (WORD)(pSkillProp->dwID));
		if (pBuff)
		{
			if (pBuff->GetLevel() > (DWORD)(nLevel))
#else	// __BUFF_1107
		SKILLINFLUENCE* hasSkill = searchMover->m_SkillState.Find(BUFF_SKILL, pSkillProp->dwID);
		if (hasSkill)
		{
			if (hasSkill->dwLevel > nLevel)
#endif	// __BUFF_1107
			{
				if (IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSEBUFF, "");
				return FALSE;
			}
		}
	}
#endif //__CSC_VER8_3


	if (pSkillProp->dwHanded != NULL_ID)
	{
		if (pItemProp)
		{
			if (IsPlayer())
			{
				if (pSkillProp->dwHanded == HD_DUAL)
				{
					if (IsDualWeapon() == FALSE)
					{
						((CUser*)this)->AddDefinedText(TID_GAME_WRONGITEM, "");
						return FALSE;
					}
				}
				else if (pItemProp->dwHanded != pSkillProp->dwHanded)
				{
					((CUser*)this)->AddDefinedText(TID_GAME_WRONGITEM, "");
					return FALSE;
				}
			}
		}
	}


	if (IsBullet(pSkillProp) == FALSE)
		return FALSE;

	if (IsPlayer())
	{
		if (pSkillProp->dwReqDisLV != NULL_ID && GetLevel() < (int)(pSkillProp->dwReqDisLV) && !IsMaster() && !IsHero() && !IsLegendHero())
			return FALSE;

		if (pSkillProp->dwReSkill1 != 0xffffffff)
		{
			LPSKILL pSkillBuf = GetSkill(pSkillProp->dwReSkill1);
			if (pSkillBuf)
			{
				if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1)
				{
					((CUser*)this)->AddText("SkillLevel1 ");
					return FALSE;
				}
			}
			else
			{
				int nIdx = GetSkillIdx(dwSkill);
				Error("DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2);
			}
		}

		if (pSkillProp->dwReSkill2 != 0xffffffff)
		{
			LPSKILL pSkillBuf = GetSkill(pSkillProp->dwReSkill2);
			if (pSkillBuf)
			{
				if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2)
				{
					((CUser*)this)->AddText("SkillLevel2 ");
					return FALSE;
				}
			}
			else
			{
				int nIdx = GetSkillIdx(dwSkill);
				Error("DoUseSkill : %s NULL GetSkill %d = dwReSkill(%d, %d)", m_szName, nIdx, pSkillProp->dwReSkill1, pSkillProp->dwReSkill2);
			}
		}
	}

	if (pSkillProp->dwID == SI_BIL_PST_ASALRAALAIKUM)
	{

		if (IsPlayer() && GetFatiguePoint() < GetReqFp(pAddSkillProp->nReqFp))
		{
			((CUser*)this)->AddDefinedText(TID_GAME_REQFP); // chipi_00917
			return FALSE;
		}
	}

	int nSkillIdx = GetSkillIdx(dwSkill);
	if (nSkillIdx >= 0)
	{
		if (GetReuseDelay(nSkillIdx))
		{
			if (IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_SKILLWAITTIME, "");
			return FALSE;
		}
	}

	if (!pTarget->IsDie() && pTarget->GetWorld() == GetWorld() && pTarget->GetLayer() == GetLayer())
	{
		if ((pAddSkillProp->nReqFp == NULL_ID || GetFatiguePoint() >= pAddSkillProp->nReqFp) &&
			(pAddSkillProp->nReqMp == NULL_ID || GetManaPoint() >= pAddSkillProp->nReqMp))
		{
			if (pSkillProp->dwID == SI_WIN_YOYO_BACKSTEP)
			{
				FLOAT fX = m_vPos.x + ((pTarget->m_vPos.x - m_vPos.x) * 0.8f);
				FLOAT fZ = m_vPos.z + ((pTarget->m_vPos.z - m_vPos.z) * 0.8f);
				if (GetWorld()->VecInWorld(fX, fZ))
				{
					D3DXVECTOR3 vPos = D3DXVECTOR3(fX, 100.0f, fZ);
					vPos.y = GetWorld()->GetFullHeight(vPos);

					REPLACE(g_uIdofMulti, GetWorld()->GetID(), vPos, REPLACE_FORCE, GetLayer());
				}
				else
					return FALSE;
			}
		}
	}
#endif // __WORLDSERVER

	OBJMSG dwMsg = OBJMSG_NONE;
	int nMotion = (int)pSkillProp->dwUseMotion;


	if (pSkillProp->dwSkillType == KT_SKILL)
	{
#ifdef __WORLDSERVER

#if __VER >= 10
		if (GetReqFp(pAddSkillProp->nReqFp) == 9999999)
		{
			if (GetManaPoint() != GetMaxManaPoint())
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
				return FALSE;
			}
			if (GetFatiguePoint() != GetMaxFatiguePoint())
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQFP, "");
				return FALSE;
			}
		}
		else
		{
			if (GetFatiguePoint() - GetReqFp(pAddSkillProp->nReqFp) < 0)
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQFP, "");
				return FALSE;
			}


			if ((int)pAddSkillProp->nReqMp > 0)
			{
				if (GetManaPoint() - GetReqMp(pAddSkillProp->nReqMp) < 0)
				{
					if (TRUE == IsPlayer())
						((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
					return FALSE;
				}
			}
		}
#else
		if (GetFatiguePoint() - GetReqFp(pAddSkillProp->nReqFp) < 0)
		{
			if (TRUE == IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_REQFP, "");
			return FALSE;
		}


		if ((int)pAddSkillProp->nReqMp > 0)
		{
			if (GetManaPoint() - GetReqMp(pAddSkillProp->nReqMp) < 0)
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
				return FALSE;
			}
		}
#endif

#endif
		dwMsg = OBJMSG_MELEESKILL;
	}
	else if (pSkillProp->dwSkillType == KT_MAGIC)
	{
#ifdef __WORLDSERVER
		if (GetAdjParam(DST_CHRSTATE) & CHS_SILENT)
			return FALSE;

#if __VER >= 10
		if (GetReqMp(pAddSkillProp->nReqMp) == 9999999)
		{
			if (GetManaPoint() != GetMaxManaPoint())
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
				return FALSE;
			}
			if (GetFatiguePoint() != GetMaxFatiguePoint())
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQFP, "");
				return FALSE;
			}
		}
		else
		{
			if (GetManaPoint() - GetReqMp(pAddSkillProp->nReqMp) < 0)
			{
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
				return FALSE;
			}
		}
#else
		if (GetManaPoint() - GetReqMp(pAddSkillProp->nReqMp) < 0)
		{
			if (TRUE == IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_REQMP, "");
			return FALSE;
		}
#endif

#endif // worldserver
		if (sutType == SUT_QUEUESTART)
		{
			if (IsPlayer())
				dwMsg = OBJMSG_MAGICCASTING;
		}
		else if (sutType == SUT_QUEUEING)
		{
			nMotion += 2;
			dwMsg = OBJMSG_MAGICSKILL;
		}
		else
		{
			dwMsg = OBJMSG_MAGICCASTING;
		}
	}
	else
		Error("DoUseSkill : %s dwSkillType = %d", m_szName, pSkillProp->dwSkillType);

	if (sutType != SUT_QUEUEING && nCastingTime == 0 && pSkillProp->dwID != SI_GEN_ATK_COUNTER)
		nMotion += 2;

	SendActMsg(OBJMSG_STOP);
	ClearDest(FALSE);

	int nRet = m_pActMover->SendActMsg(dwMsg, nMotion, idFocusObj, nCastingTime, dwSkill);

	if (nRet == 1)
	{
		SetActParam(OBJACT_USESKILL, dwSkill, idFocusObj);
		m_nAParam[3] = nLevel;
		m_nAParam[4] = bControl;

#ifdef __WORLDSERVER
		if (pSkillProp->dwSkillType == KT_SKILL)
		{

			if ((int)pAddSkillProp->nReqFp > 0)
				IncFatiguePoint(-GetReqFp(pAddSkillProp->nReqFp));

			int nReqMp = GetReqMp(pAddSkillProp->nReqMp);
			if (nReqMp > 0)
			{
				IncManaPoint(-nReqMp);
			}
		}
#if __VER >= 10
		if (pSkillProp->dwSkillType == KT_MAGIC)
		{
			if (GetReqMp(pAddSkillProp->nReqMp) == 9999999)
			{
				IncFatiguePoint(-GetFatiguePoint());
				IncManaPoint(-GetManaPoint());
			}
		}
#endif

		if (pSkillProp->dwLinkKindBullet != NULL_ID)
		{
			CItemElem* pItemElem = GetEquipItem(PARTS_BULLET);
			if (pItemElem)
				ArrowDown();
		}

		if (pSkillProp->nEvildoing < 0)
		{
			pTarget->SetJJim(this);
			m_nAtkCnt = 1;
		}


		g_UserMng.AddUseSkill(this, dwSkill, nLevel, idFocusObj, sutType, nCastingTime);
#endif	// __WORLDSERVER
	}
	else if (nRet == -2)
	{
		return FALSE;
	}

#ifdef __CLIENT
	if (IsActiveMover())
	{
		CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
		if (pTaskBar->m_nExecute == 1)
			pTaskBar->m_nExecute = 2;
	}
#endif

#ifdef __CLIENT
	{
		DWORD dwLinkSfxDouble = NULL_ID;
		switch (dwSkill)
		{
		case SI_BLD_DOUBLEAX_SPRINGATTACK:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLEAX_SPRINGATTACK01;	break;
		case SI_BLD_DOUBLE_SONICBLADE:		dwLinkSfxDouble = XI_SKILL_BLD_DOUBLE_SONICBLADE01;	break;
		case SI_KNT_TWOSW_CHARGE:			dwLinkSfxDouble = XI_SKILL_KNT_TWOSW_CHARGE01;	break;
		case SI_BLD_DOUBLESW_BLADEDANCE:	dwLinkSfxDouble = XI_SKILL_BLD_DOUBLESW_BLADEDANCE01;      break;


		case SI_JST_YOYO_VATALSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_VATALSTAB01;	break;
		case SI_JST_YOYO_HITOFPENYA:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_HITOFPENYA01;	break;

		case SI_JST_YOYO_CRITICALSWING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_CRITICALSWING01;	break;
		case SI_ACR_SUP_SLOWSTEP:	dwLinkSfxDouble = XI_SKILL_ACR_YOYO_SLOWSTEP01;	break;
		case SI_JST_SUP_POISON:		dwLinkSfxDouble = XI_SKILL_JST_SUP_POISON01;	break;
		case SI_JST_SUP_BLEEDING:	dwLinkSfxDouble = XI_SKILL_JST_SUP_BLEEDING01;	break;
		case SI_JST_YOYO_BACKSTAB:	dwLinkSfxDouble = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		case SI_JST_SUP_ABSORB:	dwLinkSfxDouble = XI_SKILL_JST_SUP_ABSORB01;	break;
			break;
		}
		if (dwLinkSfxDouble != NULL_ID)
		{
			CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId());
			if (pSfx)
			{
				pSfx->SetPartLink(0);
				pSfx = CreateSfx(g_Neuz.m_pd3dDevice, dwLinkSfxDouble, GetPos(), GetId());
				if (pSfx)
					pSfx->SetPartLink(1);
			}
		}
	}


	DWORD dwLinkSfxLeft = NULL_ID;
	switch (dwSkill)
	{
	case SI_RAG_BOW_ICEARROW:	dwLinkSfxLeft = XI_SKILL_RAG_BOW_ICEARROW01;	break;
	case SI_ACR_BOW_JUNKBOW:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_JUNKBOW01;	break;
	case SI_ACR_BOW_AIMEDSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AIMEDSHOT01;	break;
	case SI_ACR_BOW_SILENTSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_SILENTSHOT01;	break;
	case SI_ACR_BOW_ARROWRAIN:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_ARROWRAIN01;      break;
	case SI_ACR_BOW_AUTOSHOT:	dwLinkSfxLeft = XI_SKILL_ACR_BOW_AUTOSHOT01;      break;

	case SI_RAG_SUP_FASTATTACK:	  dwLinkSfxLeft = XI_SKILL_RAG_SUP_FASTATTACK01; break;
	case SI_RAG_BOW_FLAMEARROW:	  dwLinkSfxLeft = XI_SKILL_RAG_BOW_FLAMEARROW01; break;
	case SI_RAG_BOW_PIRCINGARROW: dwLinkSfxLeft = XI_SKILL_RAG_BOW_PIRCINGARROW01; break;
	case SI_RAG_BOW_POISONARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_POISONARROW01; break;
	case SI_RAG_BOW_SILENTARROW:  dwLinkSfxLeft = XI_SKILL_RAG_BOW_SILENTARROW01; break;
	case SI_RAG_BOW_TRIPLESHOT:   dwLinkSfxLeft = XI_SKILL_RAG_BOW_TRIPLESHOT01; break;

	case SI_WIN_YOYO_MADHURRICANE:	  dwLinkSfxLeft = XI_SKILL_WINDL_MADHURRICANE01; break;
	case SI_WIN_SUP_EVASIONINCREASE:	dwLinkSfxLeft = XI_SKILL_WINDL_EVASIONINCREASE01; break;
	case SI_WIN_SUP_CONTROLINCREASE: dwLinkSfxLeft = XI_SKILL_WINDL_CONTROLINCREASE01; break;
	case SI_WIN_YOYO_BACKSTEP:  dwLinkSfxLeft = XI_SKILL_WINDL_COUNTERBACK01; break;
	case SI_CRA_CRBOW_RANGESTRIKE:  dwLinkSfxLeft = XI_SKILL_CRACK_RANGESTRIKE01; break;
	case SI_CRA_SUP_POWERINCREASE:   dwLinkSfxLeft = XI_SKILL_CRACK_POWERINCREASE01; break;
	case SI_CRA_SUP_CONTROLINCREASE: dwLinkSfxLeft = XI_SKILL_CRACK_CONTROL01; break;
	case SI_CRA_SUP_HAWKEYE:  dwLinkSfxLeft = XI_SKILL_CRACK_HWAKEYE01; break;

	case SI_LOD_SUP_PULLING:  dwLinkSfxLeft = XI_SKILL_LORDK_TEMPLARPULLING01; break;
	case SI_LOD_ONE_GRANDRAGE:  dwLinkSfxLeft = XI_SKILL_LORDK_GRANDRAGE01; break;
	case SI_LOD_SUP_HOLYARMOR:  dwLinkSfxLeft = XI_SKILL_LORDK_HOLYARMOR01; break;
	case SI_LOD_SUP_SCOPESTRIKE:  dwLinkSfxLeft = XI_SKILL_LORDK_SCOPESTRIKE01; break;
	case SI_LOD_SUP_ANGER:  dwLinkSfxLeft = XI_SKILL_LORDK_ANGRYINCREASE01; break;
	case SI_LOD_ONE_SHILDSTRIKE:  dwLinkSfxLeft = XI_SKILL_LORDK_SHILDSTRIKE01; break;
	case SI_STORM_DOUBLE_CROSSBLOOD:  dwLinkSfxLeft = XI_SKILL_STORM_CROSSOFBLOOD01; break;
	case SI_STORM_SUP_POWERINCREASE:  dwLinkSfxLeft = XI_SKILL_STORM_POWERINCREASE01; break;
	case SI_STORM_DOUBLE_STORMBLAST:  dwLinkSfxLeft = XI_SKILL_STORM_STORMBLASTE01; break;
	case SI_STORM_DOUBLE_HOLDINGSTORM:  dwLinkSfxLeft = XI_SKILL_STORM_HOLDINGSTORM05; break;


	case SI_MEN_WAN_FEARSCREAM:  dwLinkSfxLeft = XI_SKILL_MENT_DARKNESSSCREAM01; break;
	case SI_MEN_WAN_ATKDECREASE:  dwLinkSfxLeft = XI_SKILL_MENT_ATTACKDECREASE01; break;
	case SI_MEN_WAN_DEFDECREASE:  dwLinkSfxLeft = XI_SKILL_MENT_DEFENDERDECREASE01; break;
	case SI_MEN_WAN_SPEDECREASE:  dwLinkSfxLeft = XI_SKILL_MENT_SPEEDDECREASE01; break;
	case SI_MEN_WAN_DARKNESSLAKE:  dwLinkSfxLeft = XI_SKILL_MENT_DARKNESSRAKE01; break;
	case SI_ELE_STF_FINALSPEAR:  dwLinkSfxLeft = XI_SKILL_ELE_FINALSPEAR01; break;
	case SI_ELE_STF_COSMICELEMENT:  dwLinkSfxLeft = XI_SKILL_ELE_COSMICELEMENT01; break;
	case SI_ELE_STF_THUNDERBOLT:  dwLinkSfxLeft = XI_SKILL_ELE_THUNDERBOLTS01; break;
	case SI_ELE_STF_SLIPPING:  dwLinkSfxLeft = XI_SKILL_ELE_SLEEPING01; break;

	case SI_FOR_SQU_FORCETENACITY:  dwLinkSfxLeft = XI_SKILL_FORCEM_AURORAOFTHETENACITY01; break;
	case SI_FOR_SQU_FORCESPEED:  dwLinkSfxLeft = XI_SKILL_FORCEM_AURORAOFTHESPEED01; break;
	case SI_FOR_SQU_FORCEANGER:  dwLinkSfxLeft = XI_SKILL_FORCEM_AURORAOFTHERAGE01; break;
	case SI_FOR_SQU_FORCEMAD:  dwLinkSfxLeft = XI_SKILL_FORCEM_AURORAOFTHEMAD01; break;
	case SI_FLO_HEAL_DOT:  dwLinkSfxLeft = XI_SKILL_FLO_PLAYEROFTHEREVIVAL01; break;
	case SI_FLO_SQU_BLESSSTEP:  dwLinkSfxLeft = XI_SKILL_ELE_SLEEPING01; break;
	case SI_FLO_SQU_BLESSBODY:  dwLinkSfxLeft = XI_SKILL_FLO_BLESSEDBODY01; break;
	case SI_FLO_SQU_BLESSARMOR:  dwLinkSfxLeft = XI_SKILL_FLO_BLESSEDARMOR01; break;
	case SI_FLO_CHEER_PATTERS:  dwLinkSfxLeft = XI_SKILL_FLO_FETTERS01; break;
	case SI_FLO_SUP_ABSOLUTE:  dwLinkSfxLeft = XI_SKILL_FLO_ABSOLUTEBARRIER01; break;
		break;
	}

	if (dwLinkSfxLeft != NULL_ID)
	{
		CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, dwLinkSfxLeft, GetPos(), GetId());
		if (pSfx)
			pSfx->SetPartLink(1);
	}


	DWORD dwLinkSfxRight = NULL_ID;
	switch (dwSkill)
	{
	case SI_MER_ONE_SPLMASH: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT04; break;
	case SI_MER_ONE_GUILOTINE: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT02; break;
	case SI_MER_ONE_REFLEXHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT01; break;
	case SI_MER_ONE_SPECIALHIT: dwLinkSfxRight = XI_SKILL_MER_ONE_SUPPORT03; break;
	case SI_ASS_KNU_POWERFIST:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT03;	break;
	case SI_ASS_KNU_TAMPINGHOLE:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT02;	break;
	case SI_ASS_KNU_BURSTCRACK:	dwLinkSfxRight = XI_SKILL_ASS_KNU_SUPPORT01;	break;
	case SI_GEN_ATK_COUNTER:	dwLinkSfxRight = XI_SKILL_JST_YOYO_BACKSTAB01;	break;
		break;
	}

	if (dwLinkSfxRight != NULL_ID)
	{
		CSfx* pSfx = CreateSfx(g_Neuz.m_pd3dDevice, dwLinkSfxRight, GetPos(), GetId());
		if (pSfx)
			pSfx->SetPartLink(0);
	}

	if (pSkillProp->dwSfxObj != NULL_ID)
	{
		CreateSfx(g_Neuz.m_pd3dDevice, pSkillProp->dwSfxObj, GetPos(), GetId());
	}

	if (pSkillProp->dwExeTarget != EXT_MELEEATK &&
		pSkillProp->dwExeTarget != EXT_MAGICATK &&
		pSkillProp->dwExeTarget != EXT_AROUNDATK &&
		pSkillProp->dwExeTarget != EXT_MAGICATKSHOT &&
		pTarget->IsPlayer())
	{
		CString str;
		if (this == g_pPlayer)
		{
			str.Format(prj.GetText(TID_GAME_SKILL_SELF), pSkillProp->szName);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_SKILL_SELF));
		}
		else
			if (pTarget == g_pPlayer && (this != pTarget || pSkillProp->dwID == SI_ASS_CHEER_CIRCLEHEALING))
			{
				str.Format(prj.GetText(TID_GAME_SKILL_MATE), GetName(), pSkillProp->szName);
				g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_SKILL_MATE));
			}
	}
#endif	// __CLIENT

	return TRUE;
}

void	CMover::OnEndSkillState(DWORD dwSkill, DWORD dwLevel)
{
	if (dwSkill < 0 || (int)(dwSkill) > prj.m_aPropSkill.GetSize())
	{
		Error("OnEndSkillState : 스킬의 범위를 넘어섬. %d", dwSkill);
		return;
	}
	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	GetSkillProp(&pSkillProp, &pAddSkillProp, dwSkill, dwLevel, "OnEndSkillState");

	if (dwSkill == SI_ACR_YOYO_COUNTER)
		m_pActMover->ResetState(OBJSTA_ATK_ALL);

#ifdef __WORLDSERVER
	if (dwSkill == SI_FLO_SUP_ABSOLUTE)
		ResetDestParam(DST_IGNORE_DMG_PVP, GetParam(DST_IGNORE_DMG_PVP, 0), TRUE);
	else
	{
#ifndef __FIX_STUNBUG
		ResetDestParam(pAddSkillProp->dwDestParam1, pAddSkillProp->nAdjParamVal1, TRUE);
		ResetDestParam(pAddSkillProp->dwDestParam2, pAddSkillProp->nAdjParamVal2, TRUE);
#else
		int		nAdjParam;
		DWORD	dwDestParam;
		for (int i = 0; i < 2; i++)
		{
			dwDestParam = pAddSkillProp->dwDestParam[i];
			nAdjParam = pAddSkillProp->nAdjParamVal[i];

			if (dwDestParam == DST_CHRSTATE)
			{
				if (nAdjParam & CHS_STUN)
				{
					if (m_buffs.GetTotalDebuffs(this, CHS_STUN) <= 1)
						SetStun(FALSE);
				}
				else if (nAdjParam & CHS_POISON)
				{
					if (m_buffs.GetTotalDebuffs(this, CHS_POISON) <= 1)
						SetPoison(FALSE);
				}
				else if (nAdjParam & CHS_BLEEDING)
				{
					if (m_buffs.GetTotalDebuffs(this, CHS_BLEEDING) <= 1)
						SetBleeding(FALSE);
				}
				else if (nAdjParam & CHS_INVISIBILITY)
				{
					if (m_buffs.GetTotalDebuffs(this, CHS_INVISIBILITY) <= 1)
						SetDarkCover(FALSE);
				}
				else
					ResetDestParam(dwDestParam, nAdjParam, TRUE);
			}
			else
				ResetDestParam(dwDestParam, nAdjParam, TRUE);
		}
#endif
	}
#endif

}

void	CMover::OnEndMeleeAttack(DWORD dwState)
{
#ifdef __WORLDSERVER
	PostAIMsg(AIMSG_END_MELEEATTACK);
#endif // WorldServer
}

BOOL CMover::DoUseItemVirtual(DWORD dwItemId, BOOL bEffectSkip)
{
#ifdef __WORLDSERVER
	ItemProp* pItemProp = prj.GetItemProp(dwItemId);
	if (pItemProp)
	{
		if (IsItemRedyTime(pItemProp, pItemProp->dwID, FALSE) == FALSE)
			return FALSE;

		D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		if (dwItemId == II_SYS_SYS_SCR_RETURN)
		{

			if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_RETURN))
				RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_RETURN);

			DWORD dwWorldID;
			D3DXVECTOR3 vPos = ((CUser*)this)->GetReturnPos(&dwWorldID);
			((CUser*)this)->REPLACE(g_uIdofMulti, dwWorldID, vPos, REPLACE_NORMAL, nTempLayer);
			return TRUE;
		}


		if (dwItemId == II_CHR_SYS_SCR_ESCAPEBLINKWING)
		{
			if (pItemProp->dwCircleTime != (DWORD)-1)
			{
				if (!IsSMMode(SM_ESCAPE))
				{
					SetSMMode(SM_ESCAPE, pItemProp->dwCircleTime);
				}
				else
				{
					return FALSE;
				}
			}
			return DoUseItemBlinkWing(pItemProp, NULL, FALSE);
		}

		DWORD	dwSfxID = pItemProp->dwSfxObj3;
		if (dwSfxID != NULL_ID && !bEffectSkip)
		{
			g_UserMng.AddCreateSfxObj(this, dwSfxID, sPos.x, sPos.y, sPos.z);
		}
	}
#endif //__WORLDSERVER

	return TRUE;
}


void CMover::OnAfterUseItem(const ItemProp * pItemProp, bool bSfx)
{
#ifdef __WORLDSERVER
	if (pItemProp == NULL)
		return;

	D3DXVECTOR3 sPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	DWORD dwSfxID = pItemProp->dwSfxObj3;

	if (pItemProp->dwItemKind3 == IK3_MAGICTRICK)
	{
		float	fTheta = D3DXToRadian(GetAngle());
		switch (pItemProp->dwID)
		{
		case II_GEN_MAG_TRI_FIRESHOWER:
		case II_GEN_MAG_TRI_HWFIREWORKS:
		case II_CHR_MAG_TRI_ROCKETBOMB:
		case II_CHR_MAG_TRI_HEARTBOMB:
		case II_CHR_MAG_TRI_TWISTERBOMB:
			sPos.x = GetPos().x + sinf(fTheta) * 3.0f;
			sPos.z = GetPos().z + -cosf(fTheta) * 3.0f;
			sPos.y = GetPos().y + 1.5f;
			sPos.y = GetWorld()->GetUnderHeight(D3DXVECTOR3(sPos.x, sPos.y, sPos.z));
			break;
		case II_GEN_MAG_TRI_NEWYEARBOMB:
		case II_GEN_MAG_TRI_SULNALBOMB:
		case II_GEN_MAG_TRI_GOODBYEBOMB:
			sPos.x = GetPos().x;
			sPos.z = GetPos().z;
			sPos.y = GetPos().y + 3.0f;
			break;
		}
	}

	if (bSfx && dwSfxID != NULL_ID)
		g_UserMng.AddCreateSfxObj(this, dwSfxID, sPos.x, sPos.y, sPos.z);

	if (IsPlayer())
		((CUser*)this)->SetHonorAdd(pItemProp->dwID, HI_USE_ITEM);
#endif // __WORLDSERVER
}

BOOL CMover::DoUseItem(DWORD dwData, int nType)
{
#ifdef __WORLDSERVER
	if (!IsPossibleUseItem())
		return FALSE;

	CUser* pUser = (CUser*)this;

	CItemContainer<CItemElem>* pContainer = GetContainerByType(nType);
	if (!pContainer)
		return FALSE;

	CItemElem* pItemBase = pContainer->GetAtId(dwData);
	if (!IsUsableItem(pItemBase))
		return FALSE;

	CItemElem* pItemElem = pItemBase;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return FALSE;

	if (nType != TYPE_INVENTORY)
	{
#ifdef __FL_EQUALIZED_MODE
		if (pUser->bTmpMode)
			return FALSE;
#endif
		if (!CanBeUsedOutsideInventory(pItemProp))
			return FALSE;
	}

	if (pItemProp->dwParts != NULL_ID)
	{
		if (!m_pActMover->IsActAttack())
			DoEquipItem(pItemElem);

		return TRUE;
	}

	if (pItemElem->IsFlag(CItemElem::expired))
	{
		if (pItemProp->dwItemKind3 == IK3_EGG)
			pUser->AddDefinedText(TID_GAME_PET_DEAD);
		else
			pUser->AddDefinedText(TID_GAME_ITEM_EXPIRED);

		return FALSE;
	}

	DWORD dwGroup = pUser->m_cooltimeMgr.GetGroup(pItemProp);
	if (dwGroup)
	{
		if (!pUser->m_cooltimeMgr.CanUse(dwGroup))
			return FALSE;
	}

	if (!IsItemRedyTime(pItemProp, pItemElem->m_dwObjId, TRUE))
		return FALSE;

	PPACKITEMELEM pPackItemElem = CPackItem::GetInstance()->Open(pItemProp->dwID);
	if (pPackItemElem)
	{
		pUser->DoUsePackItem(pItemElem, pPackItemElem, nType, false);
		return TRUE;
	}
	else if (pUser->DoUseGiftbox(pItemElem, nType, false))
	{
		return TRUE;
	}
	else if (pItemProp->dwItemKind3 == IK3_VIS)
	{
		CItemUpgrade::GetInstance()->SetPetVisItem(pUser, pItemElem->m_dwObjId, nType);
		return TRUE;
	}
	else if (pItemProp->dwID == II_SYS_SYS_VIS_KEY01)
	{
		CItemUpgrade::GetInstance()->PetVisSize(pUser, pItemElem->m_dwObjId, nType);
		return TRUE;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_PET_MAGIC)
	{
		CItemUpgrade::GetInstance()->TransFormVisPet(pUser, pItemElem->m_dwObjId, nType);
		return TRUE;
	}
#ifdef __FL_MONSTER_ALBUM
	else if (pItemProp->dwID == II_SYS_SYS_MA_SLOT_UNLOCK)
	{
		pUser->m_MonsterAlbum.BossCardSlotUnlock(pItemElem->m_dwObjId, nType);
		return TRUE;
	}
	else if (pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM_BOSS_CARD)
	{
		pUser->m_MonsterAlbum.BossCardSlotInsert(pItemElem->m_dwObjId, nType);
		return TRUE;
	}
	else if (pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM)
	{
		pUser->m_MonsterAlbum.AddMonsterCardPiece(pItemElem->m_dwObjId, nType);
		return TRUE;
	}
#endif

	switch (pItemProp->dwItemKind2)
	{
	case IK2_GUILDHOUSE_FURNITURE:
	case IK2_GUILDHOUSE_NPC:
	case IK2_GUILDHOUSE_PAPERING:
	{
		if (GuildHouseMng->SendWorldToDatabase(pUser, GUILDHOUSE_PCKTTYPE_LISTUP, GH_Fntr_Info(pItemProp->dwID)))
		{
#ifndef __FL_FIX_USELESS_LOGS
			g_DPSrvr.PutItemLog(pUser, "f", "GUILDHOUSE_LISTUP", pItemElem, 1);
#endif
		}
		else
			return FALSE;
		break;
	}
	case IK2_GUILDHOUES_COMEBACK:
	{
		if (!GuildHouseMng->EnteranceGuildHouse(pUser, pItemProp->dwID))
			return FALSE;
		break;
	}

	case IK2_FURNITURE:
	case IK2_PAPERING:
	{
		if (CHousingMng::GetInstance()->ReqSetFurnitureList(pUser, pItemProp->dwID))
		{
#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.SendName = GetName();
			aLogItem.RecvName = "HOUSING_USE";
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = GetGold();
			aLogItem.Gold2 = GetGold();
			aLogItem.Action = "f";
			g_DPSrvr.OnLogItem(aLogItem, pItemElem);
#endif
		}
		else
			return FALSE;

		break;
	}

	case IK2_WARP:
		return DoUseItemWarp(pItemProp, pItemElem);

	case IK2_BUFF2:
	{
		if (IsDoUseBuff(pItemProp) != 0)
			return FALSE;

		CTime tm = CTime::GetCurrentTime() + CTimeSpan(0, 0, pItemProp->dwAbilityMin, 0);
		time_t t = (time_t)(tm.GetTime());
		AddBuff(BUFF_ITEM2, (WORD)(pItemProp->dwID), t, 0);
#ifdef __FL_FIX_IK2BUFF2
		ApplyParam(this, pItemProp, NULL, TRUE, 0);
#endif
		break;
	}

#ifdef __BUFF_TOGIFT
	case IK2_BUFF_TOGIFT:
#endif // 
	case IK2_BUFF:
	{
		if (IsDoUseBuff(pItemProp) != 0)
			return FALSE;

		DoApplySkill(this, pItemProp, NULL);
	}
	break;
	case IK2_SYSTEM:
	{
#ifdef __JEFF_9_20
#ifdef __WORLDSERVER
		if (pItemProp->dwLimitLevel1 != 0xFFFFFFFF)
		{
			if (GetLevel() < (int)(pItemProp->dwLimitLevel1))
			{
				pUser->AddDefinedText(TID_GAME_ITEM_LEVEL, "\"%d\"", pItemProp->dwLimitLevel1);
				return FALSE;
			}
		}
#endif	// 
#endif	// 
		int nResult = 0;
		nResult = DoUseItemSystem(pItemProp, pItemElem);
		{
			if (0 < nResult)
			{
				if (nResult == 2)
				{
					pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
				}
				else if (nResult == 3)
				{
					pUser->AddDefinedText(TID_GAME_NOTOVERLAP_USE, "");
				}
				return FALSE;
			}
		}
	}
	break;

	case IK2_KEEP:
	{
		int nResult = 0;
		nResult = DoUseItemKeep(pItemProp, pItemElem);
		{
			if (nResult == 1)
			{
				pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
				return FALSE;
			}
			else if (nResult == 2)
			{
				int nExtraEXP = 0;
				for (MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i)
				{
					ItemProp* pAmpProp = prj.GetItemProp(i->second->GetId());
					if (pAmpProp)
					{
						if (pAmpProp->dwItemKind3 == IK3_EXP_RATE)
							nExtraEXP += 50;
					}
				}
				CString strTemp;
				strTemp.Format("%s %d%% %s", prj.GetText(TID_GAME_EXPITEM_TOOLTIP), nExtraEXP, prj.GetText(TID_GAME_EXPITEM_TOOLTIP1));
				pUser->AddText(strTemp);
			}
			else if (nResult == 3)
			{
				pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
				return FALSE;
			}
		}
	}
	break;

	case IK2_BLINKWING:
		return DoUseItemBlinkWing(pItemProp, pItemElem);

	case IK2_REFRESHER:
	{
		if (pItemProp->dwCircleTime != (DWORD)-1)
		{
			if (IsSMMode(SM_MAINTAIN_MP) == FALSE && SetSMMode(SM_MAINTAIN_MP, pItemProp->dwCircleTime))
			{
				SetPointParam(DST_MP, GetMaxManaPoint());
			}
			else
			{
				pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
				return FALSE;
			}
		}
		else
		{
			if (DoUseItemFood(pItemProp, pItemElem) == FALSE)
				return FALSE;
		}
		break;
	}
	case IK2_POTION:
		if (pItemProp->dwCircleTime != (DWORD)-1)
		{
			if (IsSMMode(SM_MAINTAIN_FP) == FALSE && SetSMMode(SM_MAINTAIN_FP, pItemProp->dwCircleTime))
			{
				SetPointParam(DST_FP, GetMaxFatiguePoint());
			}
			else
			{
				pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
				return FALSE;
			}
		}
		else
		{
			if (DoUseItemFood(pItemProp, pItemElem) == FALSE)
				return FALSE;
		}
		break;
	case IK2_FOOD:
		if (DoUseItemFood(pItemProp, pItemElem) == FALSE)
			return FALSE;
		break;
	case IK2_AIRFUEL:
	{
		ItemProp* pRideProp = prj.GetItemProp(m_dwRideItemIdx);
		if (pRideProp && IsPlayer() && m_pActMover->IsFly())
		{
			if (pItemProp->dwItemKind3 == IK3_ACCEL)
			{
				m_tmAccFuel = pItemProp->dwAFuelReMax * 1000;;
			}
			else
			{
				if (pRideProp->dwItemKind3 == IK3_BOARD && pItemProp->dwItemKind3 == IK3_CFLIGHT)
				{
					pUser->AddDefinedText(TID_GAME_COMBFUEL, "");
					return FALSE;
				}
				else if (pRideProp->dwItemKind3 == IK3_STICK && pItemProp->dwItemKind3 == IK3_BFLIGHT)
				{
					pUser->AddDefinedText(TID_GAME_BOARDFUEL, "");
					return FALSE;
				}

				m_nFuel += pItemProp->dwFuelRe;
				if ((DWORD)(m_nFuel) >= pRideProp->dwFFuelReMax)
					m_nFuel = pRideProp->dwFFuelReMax;
			}
			pUser->AddSetFuel(m_nFuel, m_tmAccFuel);
		}
		else
			return FALSE;
	}
	break;

	case IK2_GMTEXT:
	{
		BOOL bGMTime = TRUE;
		if (pItemProp->dwSkillTime != NULL_ID)
		{
			if (pItemProp->dwItemKind3 == IK3_TEXT_DISGUISE)
			{
				if (IsFly())
				{
					pUser->AddDefinedText(TID_PK_FLIGHT_NOUSE, "");
					bGMTime = FALSE;
				}
				else if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(GetWorld()->GetID()))
				{
					pUser->AddDefinedText(TID_GAME_INSTANCE_DISGUISE01);
					bGMTime = FALSE;
				}
			}
			if (bGMTime)
			{
				if (!HasBuffByIk3(pItemProp->dwItemKind3))
				{
					DoApplySkill(this, pItemProp, NULL);
				}
				else
				{
					pUser->AddDefinedText(TID_GAME_LIMITED_USE, "");
					bGMTime = FALSE;
				}
			}
		}

		if (pItemProp->dwID == II_SYS_SYS_GM_NODISGUISE)
		{
			if (HasBuffByIk3(IK3_TEXT_DISGUISE))
				RemoveIk3Buffs(IK3_TEXT_DISGUISE);
			else
				bGMTime = FALSE;
		}

		if (bGMTime)
		{
			if (pItemProp->dwID != II_SYS_SYS_GM_NODISGUISE)
			{
				char szGMCommand[64] = { 0, };
				CString szGMText = pItemProp->szTextFileName;
				szGMText.Replace('(', '\"');
				szGMText.Replace(')', '\"');
				int nGMCount = szGMText.Find("/", 1);
				if (nGMCount != -1)
					strncpy(szGMCommand, szGMText, nGMCount);
				else
					strcpy(szGMCommand, szGMText);
				ParsingCommand(szGMCommand, this, TRUE);
			}
		}
		else
			return FALSE;
	}
	break;
	case IK2_SKILL:
	{
		BOOL bUseItem = FALSE;
		DWORD dwActiveSkill = pItemProp->dwActiveSkill;
		if (dwActiveSkill != NULL_ID)
		{
			if (pItemProp->dwActiveSkillRate == NULL_ID ||
				xRandom(100) < pItemProp->dwActiveSkillRate)
			{
				ItemProp* pSkillProp = (ItemProp*)prj.GetSkillProp(dwActiveSkill);
				if (pSkillProp)
				{
					if (pSkillProp->dwUseChance == WUI_TARGETINGOBJ)
					{
						if (IsPlayer())
						{
							DWORD idTarget = pUser->m_idSetTarget;
							CMover* pTarget = prj.GetMover(idTarget);
							if (IsValidObj(pTarget))
								DoActiveSkill(dwActiveSkill, pItemProp->dwActiveSkillLv, pTarget);
						}
					}
					else if (pSkillProp->dwUseChance == WUI_NOW)
						DoActiveSkill(dwActiveSkill, pItemProp->dwActiveSkillLv, this);
				}
				else
					Error("DoUseItem, IK2_SKILL, item %s -> %d skill not prop", pItemProp->szName, dwActiveSkill);
			}
		}
	}
	break;
	default:
	{
		if (pItemProp->dwSkillTime != NULL_ID)
			DoApplySkill(this, pItemProp, NULL);
	}
	} // switch ik2

	switch (pItemProp->dwItemKind3)
	{
	case IK3_EGG:
		DoUseSystemPet(pItemElem);
		break;
	case IK3_PET:
		DoUseEatPet(pItemElem);
		break;
	}

	OnAfterUseItem(pItemProp);
	pItemElem->UseItem();

	CHAR cUIParam = UI_NUM;
	if (dwGroup)
	{
		pUser->m_cooltimeMgr.SetTime(dwGroup, pItemProp->GetCoolTime());
		cUIParam = UI_COOLTIME;
	}

	UpdateItem((BYTE)(dwData), cUIParam, pItemElem->m_nItemNum, 0, nType);

#endif // WORLDSERVER

	return TRUE;
}


BOOL CMover::DoUseItemBlinkWing(ItemProp * pItemProp, CItemElem * pItemElem, BOOL bUse)
{
#ifdef __WORLDSERVER
	if (IsStateMode(STATE_BASEMOTION_MODE))
		return FALSE;

	if (bUse)
	{
		if (GetWorld()->m_dwWorldID == WI_WORLD_KEBARAS)
			return FALSE;
	}

	if (GetWorld()->m_dwWorldID == WI_WORLD_GUILDWAR)
		return FALSE;

	if (CSecretRoomMng::GetInstance()->IsInTheSecretRoom(this))
	{
		if (IsPlayer())
			((CUser*)this)->AddDefinedText(TID_GAME_SECRETROOM_USEBRINKITEM);
		return FALSE;
	}

	if (GetWorld()->GetID() == WI_WORLD_MINIROOM)
		return FALSE;

	if (pItemProp->dwLimitLevel1 != NULL_ID && (int)(pItemProp->dwLimitLevel1) > GetLevel())
	{
		if (TRUE == IsPlayer())
			((CUser*)this)->AddDefinedText(TID_GAME_USINGNOTLEVEL, "\"%s\"", pItemProp->szName);
		return FALSE;
	}

	if (pItemProp->dwItemKind3 == IK3_TOWNBLINKWING)	// IK3_TOWNBLINKWING
	{
		REPLACE_TYPE type = REPLACE_NORMAL;
		if (pItemProp->dwID == II_CHR_SYS_SCR_ESCAPEBLINKWING)
			type = REPLACE_FORCE;

		// lodestar
		if (bUse)
		{
			if (IsUsableItem(pItemElem) == FALSE)
				return FALSE;

			RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
		}

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
	else
	{
		//		x: dwItemAtkOrder1
		//		y: dwItemAtkOrder2
		//		z: dwItemAtkOrder3
		//		angle: dwItemAtkOrder4
		//		dwWeaponType: worldid
		if (pItemProp->dwWeaponType == WI_WORLD_NONE || pItemProp->dwWeaponType == 0xffffffff)
			return FALSE;

		D3DXVECTOR3 vPos((float)(pItemProp->dwItemAtkOrder1), (float)(pItemProp->dwItemAtkOrder2), (float)(pItemProp->dwItemAtkOrder3));
		SetAngle((float)pItemProp->dwItemAtkOrder4);
		// remove & transfer
		if (bUse)
		{
			if (IsUsableItem(pItemElem) == FALSE)
				return FALSE;

			RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
		}

		REPLACE(g_uIdofMulti, pItemProp->dwWeaponType, vPos, REPLACE_NORMAL, nTempLayer);
	}
#endif // worldserver
	return TRUE;
}

int	CMover::DoUseItemSystem(ItemProp * pItemProp, CItemElem * pItemElem)
{

	int nResult = 0;
#ifdef __WORLDSERVER
	CUser* pUser = ((CUser*)this);
	if (IsInvalidObj(pUser))
		return 0;

#ifdef __AZRIA_1023
	if (IsPlayer())
	{
		CUser* pThis = static_cast<CUser*>(this);
		if (pThis->HasInput())
			return pThis->DoUseItemInput(pItemProp, pItemElem);
	}
#endif	// 

	switch (pItemProp->dwID)
	{
#ifdef __FL_BATTLE_PASS
	case II_CHR_SYS_SCR_BATTLEPASS01:
	case II_CHR_SYS_SCR_BATTLEPASS02:
	case II_CHR_SYS_SCR_BATTLEPASS03:
	{
		if (!CBattlePass::GetInstance()->CheckRequirments((CUser*)this))
		{
			((CUser*)this)->AddText("You cant use it right now!");
			nResult = 1;
		}
		else
		{
			if (pItemProp->dwID == II_CHR_SYS_SCR_BATTLEPASS01)
				CBattlePass::GetInstance()->AddProgress(((CUser*)this), LEVEL_BATTLE_PASS_BUNDLE_1);
			else if (pItemProp->dwID == II_CHR_SYS_SCR_BATTLEPASS02)
				CBattlePass::GetInstance()->AddProgress(((CUser*)this), LEVEL_BATTLE_PASS_BUNDLE_2);
			else if (pItemProp->dwID == II_CHR_SYS_SCR_BATTLEPASS03)
				CBattlePass::GetInstance()->AddProgress(((CUser*)this), LEVEL_BATTLE_PASS_BUNDLE_3);
		}
	}
	break;
#endif

	case II_CHR_SYS_SCR_RESTATE:
	{
		if (m_nInt == 15 && m_nSta == 15 && m_nStr == 15 && m_nDex == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReState();
	}
	break;
#ifdef __S_ADD_RESTATE
	case II_CHR_SYS_SCR_RESTATE_STR:
	{
		if (m_nStr == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOne(0);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_STA:
	{
		if (m_nSta == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOne(1);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_DEX:
	{
		if (m_nDex == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOne(2);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_INT:
	{
		if (m_nInt == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOne(3);
	}
	break;
#endif // __S_ADD_RESTATE
	case II_SYS_SYS_SCR_PET_LIFE:
	case II_SYS_SYS_SCR_PET_LIFE02:
	{
		CPet* pPet = GetPet();
		if (pPet == NULL)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_PET_NOT_FOUND);
			nResult = 1;
			break;
		}

		if (pPet->GetLevel() == PL_EGG)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_PET_CANT_USE_TO_EGG);
			nResult = 1;
			break;
		}

		if (pPet->GetLife() >= MAX_PET_LIFE)
		{
			nResult = 1;
			break;
		}

		WORD wLife = pPet->GetLife();
		wLife += (pItemProp->dwID == II_SYS_SYS_SCR_PET_LIFE ? CPetProperty::GetInstance()->GetAddLife() : 1);

		pPet->SetLife(wLife > MAX_PET_LIFE ? MAX_PET_LIFE : wLife);

		CItemElem* pItemElem = GetPetItem();
		((CUser*)this)->AddPetState(pItemElem->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp());
		break;
	}
	case II_SYS_SYS_SCR_PET_HATCH:
	{
		CPet* pPet = GetPet();
		if (pPet == NULL)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_PET_NOT_FOUND);
			nResult = 1;
			break;
		}
		if (pPet->GetLevel() != PL_EGG)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_PET_4EGG);
			nResult = 1;
			break;
		}
		PetLevelup();
		break;
	}
	case II_GEN_TOO_COL_NORMALBATTERY:
	{
		CUser* pThis = (CUser*)this;
		if (!pThis->DoUseItemBattery())
		{
			pThis->AddDefinedText(TID_GAME_CANT_USE_BATTERY);
			nResult = 1;
		}
		break;
	}
	case II_CHR_SYS_SCR_SHOUTFULL15:
	{
		if (!IsShoutFull())
			SetSMMode(SM_SHOUT15, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_SHOUTFULL30:
	{
		if (!IsShoutFull())
			SetSMMode(SM_SHOUT30, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_SHOUTFULL001:
	{
		if (!IsShoutFull())
			SetSMMode(SM_SHOUT001, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_PSKILLFULL1:
	{
		if (!(IsSMMode(SM_PARTYSKILL1) || IsSMMode(SM_PARTYSKILL15) || IsSMMode(SM_PARTYSKILL30)))
		{
			SetSMMode(SM_PARTYSKILL1, pItemProp->dwCircleTime);
			g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
		}
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_PSKILLFULL15:
	{
		if (!(IsSMMode(SM_PARTYSKILL1) || IsSMMode(SM_PARTYSKILL15) || IsSMMode(SM_PARTYSKILL30)))
		{
			SetSMMode(SM_PARTYSKILL15, pItemProp->dwCircleTime);
			g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
		}
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_PSKILLFULL30:
	{
		if (!(IsSMMode(SM_PARTYSKILL1) || IsSMMode(SM_PARTYSKILL15) || IsSMMode(SM_PARTYSKILL30)))
		{
			SetSMMode(SM_PARTYSKILL30, pItemProp->dwCircleTime);
			g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
		}
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_COMMBANK15:
	{
		if (!IsCommBank())
			SetSMMode(SM_BANK15, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_COMMBANK30:
	{
		if (!IsCommBank())
			SetSMMode(SM_BANK30, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_COMMBANK001:
	{
		if (!IsCommBank())
			SetSMMode(SM_BANK001, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_ACTIVITION:
	{
		if (!(IsSMMode(SM_ACTPOINT)))
			SetSMMode(SM_ACTPOINT, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_CHR_SYS_SCR_UPCUTSTONE:
	{
		if (!(IsSMMode(SM_ATTACK_UP)))
			SetSMMode(SM_ATTACK_UP, pItemProp->dwCircleTime);
		else
			nResult = 2;
	}
	break;
	case II_SYS_SYS_SCR_SELPAR:
	{
		CParty* pParty;
		pParty = g_PartyMng.GetParty(GetPartyId());
		if (pParty)
		{
			if (pParty->m_nLevel < MAX_PARTYLEVEL)
				pParty->SetPartyLevel(((CUser*)this), 10, 180, 0);
			else
				nResult = 1;
		}
		else
		{
			nResult = 1;
		}
		if (nResult == 1)
			((CUser*)this)->AddDefinedText(TID_GAME_NTROUPEO10, "");
	}
	break;
	case II_SYS_SYS_SCR_HOLY:
		RemoveDebufBuffs();
		break;
	case II_SYS_SYS_SCR_VELOCIJUMP:
	{
		if (!(IsSMMode(SM_VELOCIJUMP)))
		{
			if (pItemProp->dwDestParam1 != -1)
				SetDestParam(pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->nAdjParamVal1);
			if (pItemProp->dwDestParam2 != -1)
				SetDestParam(pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->nAdjParamVal2);
			SetSMMode(SM_VELOCIJUMP, pItemProp->dwCircleTime);
		}
		else
		{
			nResult = 2;
		}
	}
	break;
	case II_SYS_SYS_SCR_RECCURENCE:
	{
		if (InitSkillExp())
			((CUser*)this)->AddInitSkill();
		else
			nResult = 1;
	}
	break;
	case II_SYS_SYS_SCR_GLVCHEER:
	case II_SYS_SYS_SCR_SMELPROT:
	case II_SYS_SYS_SCR_SMELPROT3:
	case II_SYS_SYS_SCR_SMELPROT4:
	case II_SYS_SYS_SCR_SMELTING:
	case II_SYS_SYS_SCR_SUPERLEADERPARTY:
	case II_SYS_SYS_SCR_SUPERSMELTING:
	case II_SYS_SYS_SCR_FONTEDIT:	// "Font Edit"
#ifdef __Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_VENDOR:	// "Vendor Charge"
#endif //__Y_BEAUTY_SHOP_CHARGE
	case II_SYS_SYS_SCR_SKLINSTRUCT:
	case II_SYS_SYS_SCR_SMELTING2:

	{
		if (HasBuff(BUFF_ITEM, (WORD)(pItemProp->dwID)))
			nResult = 2;
		else
			DoApplySkill((CCtrl*)this, pItemProp, NULL);
	}
	break;

#ifdef	__PROTECT_AWAKE
	case	II_SYS_SYS_SCR_AWAKESAFE:
	{
		if (HasBuff(BUFF_ITEM, (WORD)(pItemProp->dwID)))
		{
			nResult = 2;
		}
		else
		{
			DoApplySkill((CCtrl*)this, pItemProp, NULL);
		}
	}
	break;
#endif	//__PROTECT_AWAKE

	case II_SYS_SYS_SCR_PET_TONIC_A:
	case II_SYS_SYS_SCR_PET_TONIC_B:
		nResult = DoUseItemPetTonic(pItemElem);
		break;
	case II_SYS_SYS_SCR_PET_FEED_POCKET:
	{
		nResult = DoUseItemFeedPocket(pItemElem);
		break;
	}
	case II_SYS_SYS_SCR_GET01:
	case II_SYS_SYS_SCR_GET02:
	{
		if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_GET01) ||
			HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_GET02))
			nResult = 2;
		else
			DoApplySkill((CCtrl*)this, pItemProp, NULL);
	}
	break;
	case II_SYS_SYS_SCR_RETURN:
	{
		if (IsPlayer())
		{
			if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_RETURN))
				nResult = 2;
			else
			{
				CWorld* pWorld = GetWorld();
				if (!pWorld)
					return 0;

				D3DXVECTOR3 vPos = GetPos();

				if (pWorld->GetID() == WI_WORLD_GUILDWAR || pWorld->GetID() == WI_WORLD_KEBARAS || prj.IsGuildQuestRegion(vPos))
				{
					((CUser*)this)->AddDefinedText(TID_GAME_LIMITZONE_USE);
					return 0;
				}

				((CUser*)this)->SetReturnPos(vPos);


				DoApplySkill((CCtrl*)this, pItemProp, NULL);

				LPCTSTR lpszKey = ((CUser*)this)->m_lpszVillage;
				if (lpszKey)
				{
					PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(GetWorld()->GetID(), lpszKey);
					if (pRgnElem)
						REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
				}
				else
				{
					Error("II_SYS_SYS_SCR_RETURN selected village is NULL\n");
				}
			}
		}
	}
	break;
	case II_SYS_SYS_SCR_CUSTODY2:
	{
		CMover* pMover = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, MI_INFO_PENG);
		lstrcpy(pMover->m_szCharacterKey, "MaFl_InstantBank");
		pMover->InitNPCProperty();
		pMover->InitCharacter(pMover->GetCharacter());
		pMover->SetPos(GetPos());
		pMover->InitMotion(MTI_STAND);
		pMover->UpdateLocalMatrix();
		pMover->m_dwTickCreated = GetTickCount();
		if (GetWorld()->ADDOBJ(pMover, TRUE, pUser->GetLayer()) == FALSE)
		{
			SAFE_DELETE(pMover);
			return 0;
		}
		break;
	}

	case II_SYS_SYS_SCR_FRIENDSUMMON_A:
	case II_SYS_SYS_SCR_FRIENDSUMMON_B:
	{
		((CUser*)this)->AddSummonFriendUse(pItemElem);
		nResult = 99;
	}
	break;

	case II_SYS_SYS_SCR_PERIN:
	{
		float	fTmpGold = (float)(((CUser*)this)->GetGold());
		if (fTmpGold >= 2000000000 || fTmpGold < 0)
		{
			((CUser*)this)->AddDiagText(prj.GetText(TID_GAME_TOOMANYMONEY_USE_PERIN));
			return 99;
		}
		else
		{
			((CUser*)this)->AddGold(100000000, true);

#ifndef __FL_FIX_USELESS_LOGS
			LogItemInfo aLogItem;
			aLogItem.Action = "C";
			aLogItem.SendName = ((CUser*)this)->GetName();
			aLogItem.RecvName = "USE_PERIN";
			aLogItem.WorldId = ((CUser*)this)->GetWorld()->GetID();
			aLogItem.Gold = (DWORD)(fTmpGold);
			aLogItem.Gold2 = ((CUser*)this)->GetGold();
			aLogItem.Gold_1 = ((CUser*)this)->GetGold() - static_cast<int>(fTmpGold);
			//aLogItem.ItemName = pItemElem->GetName();
			_stprintf(aLogItem.szItemName, "%d", pItemProp->dwID);
			aLogItem.itemNumber = 1;
			aLogItem.ItemNo = pItemElem->GetSerialNumber();
			g_DPSrvr.OnLogItem(aLogItem);
#endif

		}
	}
	break;
#ifdef __ADD_RESTATE_LOW
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
	{
		if (m_nStr == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOneLow(0);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
	{
		if (m_nSta == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOneLow(1);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
	{
		if (m_nDex == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOneLow(2);
	}
	break;
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
	{
		if (m_nInt == 15)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_RESTATE);
			nResult = 1;
		}
		else
			ReStateOneLow(3);
	}
	break;
#endif // __ADD_RESTATE_LOW

	default:
		nResult = 1;
		break;
	}

	if (pItemProp->dwItemKind3 == IK3_BALLOON)
	{
		if (HasBuffByIk3(IK3_BALLOON) != FALSE)
			nResult = 2;
		else
		{
			DoApplySkill((CCtrl*)this, pItemProp, NULL);
			nResult = 0;
		}
	}

#endif // __WORLDSERVER

	return nResult;
}

int CMover::DoUseItemKeep(ItemProp * pItemProp, CItemElem * pItemElem)
{
#ifdef __WORLDSERVER
	int nAmount = 0;
	for (MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i)
	{
		ItemProp* pItemProp = prj.GetItemProp(i->second->GetId());

		if (pItemProp)
		{
			if (pItemProp->dwItemKind3 == IK3_EXP_RATE)
			{
				nAmount++;
			}
		}
	}

	if (nAmount >= MAX_AMP_STACK_COUNT)
		return 1;

	if (HasBuff(BUFF_ITEM, (WORD)(pItemProp->dwID)))
	{
		MAPAMPSTORE::iterator i = prj.m_mapAmpStore.find(pItemProp->dwID);
		if (i != prj.m_mapAmpStore.end())
		{
			AMP_STORE_INFO* info = i->second;
			if (HasBuff(BUFF_ITEM, (WORD)(info->dwIDSecond)))
			{
				if (HasBuff(BUFF_ITEM, (WORD)(info->dwIDThird)))
				{
					if (HasBuff(BUFF_ITEM, (WORD)(info->dwIDFourth)))
					{
						if (HasBuff(BUFF_ITEM, (WORD)(info->dwIDFifth)))
						{
							return 1;
						}
						else
						{
							ItemProp* pAmpProp = prj.GetItemProp(info->dwIDFifth);
							DoApplySkill((CCtrl*)this, pAmpProp, NULL);
							return 2;
						}
					}
					else
					{
						ItemProp* pAmpProp = prj.GetItemProp(info->dwIDFourth);
						DoApplySkill((CCtrl*)this, pAmpProp, NULL);
						return 2;
					}
				}
				else
				{
					ItemProp* pAmpProp = prj.GetItemProp(info->dwIDThird);
					DoApplySkill((CCtrl*)this, pAmpProp, NULL);
					return 2;
				}
			}
			else
			{
				ItemProp* pAmpProp = prj.GetItemProp(info->dwIDSecond);
				DoApplySkill((CCtrl*)this, pAmpProp, NULL);
				return 2;
			}
		}
	}
	else
	{
		AddBuff(BUFF_ITEM, (WORD)(pItemProp->dwID), 0, pItemProp->dwSkillTime, GetId());
		return 2;
	}
#endif // __WORLDSERVER
	return 3;
}

#ifdef __WORLDSERVER
BOOL CMover::IsDoUseBuff(ItemProp * pItemProp)
{
	int nResult = 0;
	if (pItemProp->dwItemKind3 == IK3_POTION_BUFF_STR
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_DEX
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_INT
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_STA
		|| pItemProp->dwItemKind3 == IK3_POTION_BUFF_DEFENSE
		|| pItemProp->dwItemKind3 == IK3_ANGEL_BUFF
#ifdef __BUFF_TOGIFT
		|| pItemProp->dwItemKind2 == IK2_BUFF_TOGIFT
#endif // __BUFF_TOGIFT
		)
	{
		if (HasBuffByIk3(pItemProp->dwItemKind3))
			nResult = 1;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_DEFENSE20 || pItemProp->dwID == II_SYS_SYS_SCR_DEFENSE50)
	{
		if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_DEFENSE20) || HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_DEFENSE50))
			nResult = 1;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET02)
	{
		CItemElem* pItem;
		for (int i = 0; i < m_Inventory.GetMax(); i++)
		{
			pItem = m_Inventory.GetAtId(i);
			if (IsUsableItem(pItem)
				&& pItem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET
				&& pItem->m_dwKeepTime > 0
				&& !pItem->IsFlag(CItemElem::expired)
				)
			{
				nResult = 1;
				break;
			}
		}
		if (nResult != 1)
		{
			IBuff* pBuff = m_buffs.GetBuff(BUFF_ITEM, (WORD)(pItemProp->dwID));
			if (pBuff)
			{
				DWORD dwCurr = ::timeGetTime();
				if (static_cast<int>(pItemProp->dwSkillTime) > static_cast<int>(pItemProp->dwSkillTime + (pBuff->GetTotal() - (dwCurr - pBuff->GetInst()))))
					nResult = 1;
			}
		}
	}
	else if (HasBuff(BUFF_ITEM, (WORD)(pItemProp->dwID)))
	{
		nResult = 1;
	}
#ifdef __FL_FIX_IK2BUFF2
	else if (HasBuff(BUFF_ITEM2, (WORD)(pItemProp->dwID)))
	{
		nResult = 1;
	}
#endif
	else if (pItemProp->dwID == II_GEN_TOO_COL_SILVERBATTERY
		|| pItemProp->dwID == II_GEN_TOO_COL_GOLDBATTERY
		|| pItemProp->dwID == II_GEN_TOO_COL_BATTERY001
		)
	{
		if (HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_SILVERBATTERY)
			|| HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_GOLDBATTERY)
			|| HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_BATTERY001)
			)
			nResult = 1;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP01
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP02
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYEXPUP01_01
		)
	{
		if (HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01)
			|| HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP02)
			|| HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYEXPUP01_01)
			)
			nResult = 1;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_SPETGOOD)
	{
		if (HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_SPETGOOD))
			nResult = 1;
	}
	else if (pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP01
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP02
		|| pItemProp->dwID == II_SYS_SYS_SCR_PARTYSKILLUP01_01
		)
	{
		if (HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01)
			|| HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP02)
			|| HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_PARTYSKILLUP01_01)
			)
			nResult = 1;
	}
#ifdef __FL_BATTLE_PASS
	else if (pItemProp->dwItemKind3 == IK3_BATTLEPASS)
	{
		if (pItemProp->nMaxRepair != CBattlePass::GetInstance()->GetBattlePassID())
		{
			((CUser*)this)->AddText("This ticked was created for other battle pass season.", 0xffffffff);
			return 1;
		}

		if (HasBuffByIk3(IK3_BATTLEPASS))
			nResult = 1;
	}
#endif

	switch (nResult)
	{
	case 1:
		((CUser*)this)->AddDefinedText(TID_GAME_LIMITED_USE, "");
		break;
	case 2:
		((CUser*)this)->AddDefinedText(TID_GAME_PKVALUE_NOT_ANGEL);
		break;
	case 3:
		((CUser*)this)->AddDefinedText(TID_GAME_NOT_QUEST_ANGEL);
		break;
	case 4:
		((CUser*)this)->AddDefinedText(TID_GAME_NOT_COMPLETE_ANGEL);
		break;
	}
	return nResult;
}
#endif // __WORLDSERVER


BOOL	CMover::DoUseItemFood_SM(ItemProp * pItemProp, CItemElem * pItemElem)
{
#ifdef __WORLDSERVER
	DWORD nAdjParamVal = NULL_ID;
	int nType = -1;

	switch (pItemProp->dwID)
	{
	case II_CHR_FOO_COO_BULLHAMS:
	{
		if (IsSMMode(SM_MAX_HP))
		{
			((CUser*)this)->AddDefinedText(TID_GAME_LIMITED_USE, "");
			return FALSE;
		}

		nAdjParamVal = pItemProp->nAdjParamVal2;
		nType = SM_MAX_HP;
	}
	break;
	case II_CHR_FOO_COO_GRILLEDEEL:
	{
		if (IsSMMode(SM_MAX_HP50))
		{
			((CUser*)this)->AddDefinedText(TID_GAME_LIMITED_USE, "");
			return FALSE;
		}

		m_nPlusMaxHitPoint = nAdjParamVal = int(GetMaxOriginHitPoint() / (100 / pItemProp->nAdjParamVal1));
		nType = SM_MAX_HP50;
	}
	break;
	default: return FALSE;
	}

	if (nAdjParamVal == NULL_ID || nType == -1)
		return FALSE;

	SetDestParam(pItemProp->dwDestParam1, nAdjParamVal, nAdjParamVal);
	SetSMMode(nType, pItemProp->dwCircleTime);
#endif
	return TRUE;
}

// IK2_FOOD / IK2_REFRESHER
BOOL	CMover::DoUseItemFood(ItemProp * pItemProp, CItemElem * pItemElem)
{
#ifdef __WORLDSERVER
	if (pItemProp->dwID == II_GEN_FOO_INS_SUPERHOTDOG)
	{
		SetPointParam(DST_HP, GetMaxHitPoint());
		SetPointParam(DST_MP, GetMaxManaPoint());
		SetPointParam(DST_FP, GetMaxFatiguePoint());
		return TRUE;
	}

	int nPoint;
	int nMax, nHP, nHPMax;

	for (int i = 0; i < PROP_BONUSES; i++)
	{
		if (pItemProp->dwDestParam[i] != (DWORD)-1)
		{
			nHP = GetPointParam(pItemProp->dwDestParam[i]);
			nPoint = pItemProp->nAdjParamVal[i];
			nHPMax = GetMaxPoint(pItemProp->dwDestParam[i]);
			nMax = pItemProp->dwAbilityMin;

			if (pItemProp->dwCircleTime != -1)
			{
				BOOL bRet = DoUseItemFood_SM(pItemProp, pItemElem);
				if (bRet == FALSE)	return FALSE;
			}
			else
			{
				if (pItemProp->dwDestParam[i] == DST_HP || pItemProp->dwDestParam[i] == DST_MP || pItemProp->dwDestParam[i] == DST_FP)
				{
					SetPointParam(pItemProp->dwDestParam[i], nHP + nPoint);
				}
				else
				{
					DoApplySkill(this, pItemProp, NULL);
				}
			}
		}
	} // for
#endif // worldserver

	return TRUE;
}


#ifdef __WORLDSERVER
BOOL CMover::DoUseItemSexChange(int nFace)
{
	// 1.
	for (DWORD dwParts = 0; dwParts < MAX_HUMAN_PARTS; dwParts++)
	{
		if (dwParts == PARTS_HEAD || dwParts == PARTS_HAIR || dwParts == PARTS_LWEAPON
			|| dwParts == PARTS_RWEAPON || dwParts == PARTS_SHIELD || dwParts == PARTS_RIDE
			|| (dwParts >= PARTS_NECKLACE1 && dwParts <= PARTS_BULLET))
			continue;

		CItemElem* pArmor = m_Inventory.GetEquip(dwParts);
		if (pArmor)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_CHECK_EQUIP, "");
			return FALSE;
		}
	}

#ifdef __FL_EQUALIZED_MODE
	CUser* pUser = (CUser*)this;
	if (pUser->bTmpMode)
	{
		pUser->AddDefinedText(TID_GAME_GUILDCOMBAT_NOT_USE);
		return FALSE;
	}
#endif

	// 2.
	DWORD dwIndex;
	if (GetSex() == SEX_MALE)
	{
		dwIndex = MI_FEMALE;
		SetSex(SEX_FEMALE);
	}
	else
	{
		dwIndex = MI_MALE;
		SetSex(SEX_MALE);
	}

	SetTypeIndex(D3DDEVICE, OT_MOVER, dwIndex);
	ResetScale();
	SetMotion(MTI_WALK);

	RedoEquip(FALSE, FALSE);
	UpdateLocalMatrix();

#ifdef __FL_SWITCH_EQ
	m_Switches.ClearAllSwitches();
#endif
	// 3.
	g_UserMng.AddSexChange(this);

	// 4.
	m_dwHeadMesh = (DWORD)nFace;
	g_UserMng.AddChangeFace(m_idPlayer, (DWORD)nFace);

#if __VER >= 11 // __SYS_PLAYER_DATA
	g_dpDBClient.SendUpdatePlayerData(static_cast<CUser*>(this));
#endif // __SYS_PLAYER_DATA

	return TRUE;
}
#endif	// __WORLDSERVER


int	 CMover::DoAttackMagic(CObj * pTargetObj, int nMagicPower, int idSfxHit)
{
	if (IsInvalidObj(pTargetObj))
		return(-1);

	ItemProp* pItemProp;
#ifndef __CLIENT
	CItemElem* pItemElem = GetWeaponItem();
	if (pItemElem == NULL)
		return(-1);

	pItemProp = pItemElem->GetProp();
#else
	if (IsActiveMover())
	{
		CItemElem* pItemElem = GetWeaponItem();
		if (pItemElem == NULL)
			return(-1);

		pItemProp = pItemElem->GetProp();
	}
	else
	{
		DWORD dwItemId = m_aEquipInfo[PARTS_RWEAPON].dwId;
		if (dwItemId == NULL_ID)
			return(-1);

		pItemProp = prj.GetItemProp(dwItemId);
	}
#endif

	if (pItemProp->dwItemKind3 != IK3_WAND)
		return(-1);

#ifdef __CLIENT
	CWorld* pWorld = GetWorld();
	D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
	D3DXVECTOR3 vEnd = pTargetObj->GetPos();	vEnd.y += 0.5f;

	if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
		return -1;
	}
#endif

	SendActMsg(OBJMSG_STOP);

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if ((nError = m_pActMover->SendActMsg(OBJMSG_ATK_MAGIC1, idTarget)) == 1)
	{
		if (m_pActMover->IsFly() == FALSE)
		{
			const float fAngle = GetDegree(pTargetObj->GetPos(), GetPos());
			if (GetAngle() != fAngle)
			{
				SetAngle(fAngle);
#ifdef __WORLDSERVER
				if (!IsPlayer())
					g_UserMng.AddSetPosAngle(this, GetPos(), GetAngle());
#endif
			}
		}

		SetActParam(OBJACT_MAGIC_ATTACK, idTarget, nMagicPower, 0, CheckAttackDistance(pTargetObj, pItemProp->dwAttackRange) ? AF_MAGIC : AF_MISS);

#ifdef __CLIENT	
		if (nMagicPower >= 1)
		{
			if (g_WndMng.m_pWndTaskBar->m_nActionPoint < 100)
				g_WndMng.m_pWndTaskBar->m_nActionPoint++;
		}

		if (IsActiveMover())
		{
			SetActParam(OBJACT_MAGIC_ATTACK, idTarget, nMagicPower);
			g_DPlay.SendMagicAttack(((CCtrl*)pTargetObj)->GetId(), nMagicPower, m_idSfxHit);
		}
#endif

#ifdef __WORLDSERVER
		if (IsPlayer() == TRUE)
		{
			if (nMagicPower >= 1 && ((CUser*)this)->m_playTaskBar.m_nActionPoint < 100)
				((CUser*)this)->m_playTaskBar.m_nActionPoint++;
		}

		if (nMagicPower == 3)
			IncManaPoint(-GetReqMp(pItemProp->dwReqMp));

		g_UserMng.AddMagicAttack(this, idTarget, nMagicPower, idSfxHit);
#endif
	}

	return(nError);
}

int	 CMover::DoAttackRange(CObj * pTargetObj, DWORD dwItemID, int idSfxHit)
{
	ItemProp* pItemProp = NULL;
	int nPower = 0;

	if (IsInvalidObj(pTargetObj))
		return -1;

#ifdef __CLIENT
	if (IsActiveMover())
	{
		CWorld* pWorld = GetWorld();
		D3DXVECTOR3 vStart = GetPos();				vStart.y += 0.5f;
		D3DXVECTOR3 vEnd = pTargetObj->GetPos();	vEnd.y += 0.5f;

		if (pWorld->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_BLOCKTARGETING), NULL, prj.GetTextColor(TID_GAME_BLOCKTARGETING));
			return -1;
		}
	}
#endif 

	if (IsPlayer())
	{
		pItemProp = GetActiveHandItemProp();
		nPower = dwItemID;

#ifdef __WORLDSERVER
		CItemElem* pItemElem = m_Inventory.GetEquip(PARTS_BULLET);
		if (pItemElem == NULL || pItemElem->GetProp()->dwItemKind3 != IK3_ARROW)
			return -1;
#endif 
	}
	else
	{
		if (dwItemID == NULL_ID || dwItemID == 0)
			pItemProp = GetActiveHandItemProp();
		else
			pItemProp = prj.GetItemProp(dwItemID);
	}

	if (pItemProp == NULL)
	{
		Error("%s DoAttackRange ActiveHandItemProp = NULL", m_szName);
		return -1;
	}

	if (pItemProp->dwWeaponType != WT_RANGE && pItemProp->dwWeaponType != WT_RANGE_BOW)
		return -1;

	SendActMsg(OBJMSG_STOP);

	DWORD dwUseMotion = pItemProp->dwUseMotion;
	if (dwUseMotion == NULL_ID)
	{
		if (IsPlayer())
			dwUseMotion = MTI_ATK_13 - MTI_STAND_13;
		else
		{
			Error("%s [%d] dwUseMotion = NULL_ID", m_szName, pItemProp->dwID);
			dwUseMotion = MTI_ATK1;
		}
	}

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError = -1;
	if ((nError = m_pActMover->SendActMsg(OBJMSG_ATK_RANGE1, idTarget, (int)dwUseMotion)) == 1)
	{
		const float fAngle = GetDegree(pTargetObj->GetPos(), GetPos());
		if (GetAngle() != fAngle)
		{
			SetAngle(fAngle);
#ifdef __WORLDSERVER
			if (!IsPlayer())
				g_UserMng.AddSetPosAngle(this, GetPos(), GetAngle());
#endif
		}
		SetActParam(OBJACT_RANGE_ATTACK, idTarget, dwItemID, idSfxHit, CheckAttackDistance(pTargetObj, pItemProp->dwAttackRange) ? AF_GENERIC : AF_MISS);

#ifdef __WORLDSERVER
		g_UserMng.AddRangeAttack(this, idTarget, dwItemID, idSfxHit);
		ArrowDown();
#else
		if (IsActiveMover())
			g_DPlay.SendRangeAttack(((CCtrl*)pTargetObj)->GetId(), dwItemID, m_idSfxHit);
#endif
	}
	return nError;
}


int		CMover::DoAttackSP(CObj * pTargetObj, DWORD dwItemID)
{
	if (IsInvalidObj(pTargetObj))
		return(-1);

	ItemProp* pItemProp;
	if (dwItemID == NULL_ID || dwItemID == 0)
		pItemProp = GetActiveHandItemProp();
	else
		pItemProp = prj.GetItemProp(dwItemID);

	if (pItemProp == NULL)
	{
		Error("%s doattacksp prop %d", m_szName, (int)dwItemID);
		return FALSE;
	}

	DWORD dwUseMotion = pItemProp->dwUseMotion;
	if (dwUseMotion == NULL_ID)
	{
		Error("%s doattacksp motion %d", m_szName, pItemProp->dwID);
		dwUseMotion = MTI_ATK1;
	}

	OBJID	idTarget = ((CCtrl*)pTargetObj)->GetId();

	int nError;
	if ((nError = m_pActMover->SendActMsg(OBJMSG_SP_ATK1, idTarget, (int)dwItemID, (int)dwUseMotion)) == 1)
	{
		const float fAngle = GetDegree(pTargetObj->GetPos(), GetPos());
		if (GetAngle() != fAngle)
		{
			SetAngle(fAngle);
#ifdef __WORLDSERVER
			if (!IsPlayer())
				g_UserMng.AddSetPosAngle(this, GetPos(), GetAngle());
#endif //__WORDSERVER
		}

		SetActParam(OBJACT_SP_ATTACK, idTarget, (int)dwItemID);

#ifdef __WORLDSERVER
		g_UserMng.AddAttackSP(this, OBJMSG_SP_ATK1, idTarget, dwItemID, 0);
#else	// __WORLDSERVER

		DWORD dwSfxObj = pItemProp->dwSfxObj;
		if (NULL_ID != dwSfxObj)
			CreateSfx(D3DDEVICE, dwSfxObj, GetPos());
#endif	// __WORLDSERVER
	}

	return(nError);
}

int        CMover::DoAttackMelee(CMover * pTarget, OBJMSG dwMsg, DWORD dwItemID, bool bCheckDistance)
{
	SendActMsg(OBJMSG_STOP);

	if (IsInvalidObj(pTarget))
		return 0;

	ItemProp* pItemProp;
	if (dwItemID == NULL_ID || dwItemID == 0)
		pItemProp = GetActiveHandItemProp();
	else
		pItemProp = prj.GetItemProp(dwItemID);

	if (pItemProp == NULL)
	{
		Error("%s doattackmelee prop NULL %d", GetName(), (int)dwItemID);
		return 0;
	}

	int nError = SendActMsg(dwMsg, pTarget->GetId(), MAKELONG(0, 0), MAKELONG(0, 0));
	if (nError > 0)
	{
		const float fAngle = GetDegree(pTarget->GetPos(), GetPos());
		if (GetAngle() != fAngle)
		{
			SetAngle(fAngle);
#ifdef __WORLDSERVER
			if (!IsPlayer())
				g_UserMng.AddSetPosAngle(this, GetPos(), GetAngle());
#endif //__WORDSERVER
		}

		DWORD attackFlag = AF_MISS;
		bool bValidDistance = bCheckDistance ? CheckAttackDistance(pTarget, pItemProp->dwAttackRange) : true;
		if (bValidDistance)
			attackFlag = GetAttackResult(pTarget) ? AF_GENERIC : AF_MISS;
#ifdef __WORLDSERVER
		else
		{
			if (IsPlayer())
				((CUser*)this)->SetLastAnimation(0);
		}
#endif

		SetActParam(OBJACT_MELEE_ATTACK, pTarget->GetId(), (int)dwItemID, 0, attackFlag);

#ifdef __CLIENT
		if (IsActiveMover())
		{
			g_DPlay.SendMeleeAttack(dwMsg, pTarget->GetId());
			PlayCombatMusic();
		}
#else
		g_UserMng.AddMeleeAttack(this, dwMsg, pTarget->GetId());
#endif
	}
	else if (nError == 0 && IsPlayer())
	{
		m_pActMover->m_qMeleeAtkMsg.AddTail(new ACTMSG(dwMsg, pTarget->GetId(), 0, 0));
#ifdef __WORLDSERVER
		g_UserMng.AddMeleeAttack(this, dwMsg, pTarget->GetId());
#endif
	}

	return TRUE;
}

void	CMover::DoPickupItemAround()
{
#ifdef __CLIENT
	int nRange = 4;
	float fDistMin = 99999.0f, fDistSq;
	CObj* pObj, * pMinObj = NULL;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer())
	{
		if (pObj->GetType() == OT_ITEM)
		{
			vDist = pObj->GetPos() - vPos;
			fDistSq = D3DXVec3LengthSq(&vDist);
			if (fDistSq < 4.0f * 4.0f)
			{
				if (fDistSq < fDistMin)
				{
					fDistMin = fDistSq;
					pMinObj = pObj;
				}
			}
		}
	}
	END_LINKMAP


		if (pMinObj)
		{
			CMD_SetUseObject((CCtrl*)pMinObj);
		}
#endif // __CLIENT
}

void CMover::DoSelectNearestPlayer()
{
#ifdef __CLIENT
	CWorld* pWorld = GetWorld();
	if (!pWorld)
		return;

	if (pWorld->GetID() != WI_WORLD_GUILDWAR && !pWorld->IsArena()
#ifdef __FL_FFA
		&& !pWorld->IsFFA()
#endif
#ifdef __FL_TDM
		&& !pWorld->IsTDM()
#endif
		)
		return;

	int nRange = 32;
	float fDistMin = 99999.0f, fDistSq;
	CObj* pObj, * pMinObj = NULL;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer())
	{
		if (pObj->GetType() == OT_MOVER)
		{
			CMover* pMover = (CMover*)pObj;
			if (pMover != this && !pMover->IsDie() && !pMover->IsMode(TRANSPARENT_MODE))
			{
				bool bAble = false;
				if (pWorld->GetID() == WI_WORLD_GUILDWAR)
				{
					CGuild* pGuild1 = g_pPlayer->GetGuild();
					CGuild* pGuild2 = pMover->GetGuild();

					if (pGuild1 && pGuild2 && (pGuild1->GetGuildId() != pGuild2->GetGuildId()))
						bAble = true;
				}
#ifdef __FL_TDM
				else if (pWorld->IsTDM())
				{
					if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->IsCanKillTDM(pMover))
						bAble = true;
				}
#endif
				else
					bAble = true;

				if (bAble)
				{
					vDist = pObj->GetPos() - vPos;
					fDistSq = D3DXVec3LengthSq(&vDist);
					if (fDistSq < 32.0f * 32.0f)
					{
						if (fDistSq < fDistMin)
						{
							fDistMin = fDistSq;
							pMinObj = pObj;
						}
					}
				}
			}
		}
	}
	END_LINKMAP


		if (pMinObj)
			pWorld->SetObjFocus(pMinObj);
#endif
}

void	CMover::SendDamageAround(int nDmgType, CMover * pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio)
{
#ifdef __WORLDSERVER
	int nRange = 4;	// 4m
	float fDistSq;
	CObj* pObj;
	CMover* pTarget;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if (fRange <= 4.0f)
		nRange = 4;
	else if (fRange <= 8.0f)
		nRange = 8;
	else if (fRange <= 16.0f)
		nRange = 16;
	else
		nRange = 32;

	if (fRange <= 0)
		Error("CMover::SendDamageAround : D:%s A:%s %d %f", GetName(), pAttacker->GetName(), nAttackID, fRange);

	ItemProp* pProp;
	if (nDmgType == AF_MAGICSKILL)
	{
		pProp = prj.GetSkillProp(nAttackID);
		if (pProp == NULL)
		{
			Error("CMover::SendDamageAround : %s. 스킬(%d)의 프로퍼티가 업다.", m_szName, nAttackID);
			return;	// property not found
		}
	}
	else
	{
		pProp = prj.GetItemProp(nAttackID);
		if (pProp == NULL)
		{
			Error("CMover::SendDamageAround : %s. 아이템(%d)의 프로퍼티가 업다.", m_szName, nAttackID);
			return;	// property not found
		}
	}

	BOOL	bDamage = FALSE;
	BOOL	bTarget = FALSE;

	if (nApplyType & OBJTYPE_PLAYER)
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer())
		{
			if (pObj->GetType() == OT_MOVER)
			{
				bDamage = TRUE;

				if (bDamage)
				{
					vDist = pObj->GetPos() - vPos;
					fDistSq = D3DXVec3LengthSq(&vDist);
					if (fDistSq < fRange * fRange)
					{
						if (pObj == this && fTargetRatio == 0.0f)
						{
						}
						else
						{
							if (pObj != pAttacker)
							{
								pTarget = (CMover*)pObj;
								bTarget = (pTarget == this);
								if (IsValidObj((CObj*)pTarget) && pTarget->IsLive())
								{
									if (pProp->dwComboStyle == CT_FINISH)
										pTarget->m_pActMover->SendDamageForce(nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget);
									else
										pTarget->m_pActMover->SendDamage(nDmgType, pAttacker->GetId(), (nAttackID << 16), bTarget);
								}
							}
						}
					}

					bDamage = FALSE;
				} // bDamage
			}
		}
		END_LINKMAP
	}


	if (nApplyType & OBJTYPE_MONSTER)
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer())
		{
			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPeaceful() == FALSE)
			{
				vDist = pObj->GetPos() - vPos;
				fDistSq = D3DXVec3LengthSq(&vDist);
				if (fDistSq < fRange * fRange)
				{
					if (pObj == this && fTargetRatio == 0.0f)
					{
					}
					else
					{
						if (pObj != pAttacker)
						{
							pTarget = (CMover*)pObj;
							if (IsValidObj((CObj*)pTarget) && pTarget->IsLive())
							{
								if (pProp->dwComboStyle == CT_FINISH)
									pTarget->m_pActMover->SendDamageForce(nDmgType, pAttacker->GetId(), (nAttackID << 16));
								else
									pTarget->m_pActMover->SendDamage(nDmgType, pAttacker->GetId(), (nAttackID << 16));
							}
						}
					}
				}
			}
		}
		END_LINKMAP
	}

#endif // WORLDSERVER
} // SendDamageAround()


#ifdef __WORLDSERVER
BOOL CMover::DoUseItemWarp(ItemProp * pItemProp, CItemElem * pItemElem)
{
	switch (pItemProp->dwID)
	{
	case II_GEN_WARP_COUPLERING:
	case II_GEN_WARP_WEDDING_BAND:
	case II_GEN_WARP_COUPLERING01:
	{
		if (GetSummonState(FALSE) != 0)
			return FALSE;

		u_long idCouple = (u_long)(pItemElem->GetRandomOptItemId());
		if (idCouple == m_idPlayer)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_COUPLERING_ERR01, "%s", GetName());
			return FALSE;
		}

		if (idCouple > 0)
		{
			const char* lpszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idCouple);
			if (lpszPlayer)
			{
				CUser* pUser = (CUser*)prj.GetUserByID(idCouple);
				if (IsValidObj((CObj*)pUser))
				{
					if (pUser->GetSummonState(TRUE) != 0)
						return FALSE;

					if (GetWorld()->GetID() == pUser->GetWorld()->GetID() && GetLayer() == pUser->GetLayer())
						pUser->SummonPlayer((CUser*)this);
					else
						((CUser*)this)->AddDefinedText(TID_GAME_COUPLERING_ERR02, "%s", lpszPlayer);
				}
				else
					((CUser*)this)->AddDefinedText(TID_DIAG_0061, "%s", lpszPlayer);
			}
		}
		else
		{
			UpdateItemEx((BYTE)(pItemElem->m_dwObjId), UI_RANDOMOPTITEMID, (__int64)m_idPlayer);
			if (II_GEN_WARP_COUPLERING == pItemProp->dwID)
				UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_KEEPTIME, 21600);
			((CUser*)this)->AddDefinedText(TID_GAME_COUPLERING_CARVE, "%s", GetName());
		}
		return TRUE;
	}
	default:
		break;
	}
	return FALSE;
}
#endif	// __WORLDSERVER

DWORD CMover::GetReuseDelay(int nIndex)
{
	ASSERT(nIndex >= 0);

	DWORD dwCur = ::timeGetTime();
	if (dwCur > m_tmReUseDelay[nIndex])
		return 0;
	else
		return m_tmReUseDelay[nIndex] - dwCur;
}

#if __VER >= 9	// __PET_0410
#ifndef __CLIENT
int CMover::DoUseItemFeedPocket(CItemElem * pPocket)
{
	int nResult = 0;

#ifdef __JEFF_11_1
	if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET02))
	{
		((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_PETPOCKET);
		return	4;
	}
#endif	// __JEFF_11_1

	if (HasBuff(BUFF_ITEM, (WORD)(pPocket->m_dwItemId)))
	{
		if (pPocket->m_dwKeepTime != 0)
		{
			RemoveBuff(BUFF_ITEM, (WORD)(pPocket->m_dwItemId));
			nResult = 4;
		}
		else
			nResult = 2;
	}
	else
	{
		ItemProp* pProp = pPocket->GetProp();
		if (pPocket->m_dwKeepTime == 0)
		{
			CItemElem* pItemElem;
			for (int i = 0; i < m_Inventory.GetMax(); i++)
			{
				pItemElem = m_Inventory.GetAtId(i);
				if (IsUsableItem(pItemElem)
					&& pItemElem->m_dwItemId == pPocket->m_dwItemId
					&& pItemElem->m_dwKeepTime > 0
					&& !pItemElem->IsFlag(CItemElem::expired)
					)
				{

					((CUser*)this)->AddDefinedText(TID_GAME_DONOTUSE_PETPOCKET, "");
					nResult = 4;
					return nResult;
				}
			}
			ASSERT(pProp->dwAbilityMin < 259200);
			UpdateItem((BYTE)(pPocket->m_dwObjId), UI_KEEPTIME, pProp->dwAbilityMin);	// 1440	// 1 days
		}
		DoApplySkill((CCtrl*)this, pProp, NULL);
	}

	return nResult;
}
#endif	// __CLIENT
#endif	// __PET_0410

#ifdef __WORLDSERVER
void CMover::ActivateSystemPet(CItemElem * pItemElem)
{
	if (IsFly())
	{
		static_cast<CUser*>(this)->AddDefinedText(TID_GAME_CANNOT_CALL_PET_ON_FLYING);
		return;
	}
	BOOL bFirst = FALSE;
	if (pItemElem->m_pPet == NULL)
	{
		pItemElem->m_pPet = new CPet;
		pItemElem->m_pPet->InitEgg();
		bFirst = TRUE;
	}
	SetPetId(pItemElem->m_dwObjId);
#ifdef __FL_SWITCH_EQ
	ProcessPet();
#endif

#ifdef __PET_1024
	g_UserMng.AddPetCall(this, pItemElem->m_dwObjId, pItemElem->m_pPet->GetIndex(), (BYTE)pItemElem->m_pPet->GetLevel(), pItemElem->m_pPet->GetName());
#else	// __PET_1024
	g_UserMng.AddPetCall(this, pItemElem->m_dwObjId, pItemElem->m_pPet->GetIndex(), (BYTE)pItemElem->m_pPet->GetLevel());
#endif	// __PET_1024

	if (bFirst)
		static_cast<CUser*>(this)->AddPet(pItemElem->m_pPet, PF_PET);
}

void CMover::InactivateSystemPet(CItemElem * pItemElem)
{
	if (GetPetId() == pItemElem->m_dwObjId)
		PetRelease();
	else
		static_cast<CUser*>(this)->AddDefinedText(TID_GAME_PET_NOWUSE);
}

void CMover::DoUseSystemPet(CItemElem * pItemElem)
{
	if (HasActivatedSystemPet())
		InactivateSystemPet(pItemElem);
	else
		ActivateSystemPet(pItemElem);
}

void CMover::DoUseEatPet(CItemElem * pItemElem)
{
	if (HasActivatedEatPet())
		InactivateEatPet();
	else
		ActivateEatPet(pItemElem);
}

void CMover::ActivateEatPet(CItemElem * pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return;

	if (IsFly())
	{
		static_cast<CUser*>(this)->AddDefinedText(TID_GAME_CANNOT_CALL_PET_ON_FLYING);
		return;
	}

#ifdef __MODEL_CHANGE
	DWORD dwPetId;
	if (pItemElem && pItemElem->IsChangedLook())
	{
		ItemProp* pProp2 = prj.GetItemProp(pItemElem->GetLook());
		if (pProp2)
			dwPetId = pProp2->dwLinkKind;
	}
	else
		dwPetId = pProp->dwLinkKind;

	CMover* pEatPet = CreateMover(GetWorld(), dwPetId, GetPos(), GetLayer());
#else
	CMover* pEatPet = CreateMover(GetWorld(), pProp->dwLinkKind, GetPos(), GetLayer());
#endif

	if (pEatPet)
	{
		CAIPet* pAIPet = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
		if (pAIPet)
		{
			pAIPet->SetOwner(GetId());
			SetEatPetId(pEatPet->GetId());
			pAIPet->SetItem(this, pItemElem);

			for (int i = 0; i < PROP_BONUSES; i++)
			{
				if (pProp->dwDestParam[i] != NULL_ID)
					SetDestParam(pProp->dwDestParam[i], pProp->nAdjParamVal[i], pProp->dwChgParamVal[i]);
			}

			if (pItemElem->IsVisPet())
			{
				if (pItemElem->GetPiercingSize() <= 0)
					UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_PETVIS_SIZE, PETVIS_DEFAULT_VIS_SLOT_SZIE);

				SetVisPetItem(pItemElem->m_dwObjId);
				SetPetVisDST(pItemElem);
				pEatPet->m_dwMoverSfxId = pItemElem->GetVisPetSfxId();
				static_cast<CUser*>(this)->AddActivateVisPet(pItemElem->m_dwObjId, GetEatPetId());
			}
		}
	}
}

void CMover::InactivateEatPet(void)
{
	CMover* pEatPet = prj.GetMover(GetEatPetId());
	if (IsValidObj(pEatPet))
	{
		CAIPet* pAIPet = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
		if (pAIPet)
		{
			CItemElem* pPetItem = m_Inventory.GetAtId(pAIPet->GetPetItemId());
			if (pPetItem)
			{
				ItemProp* pProp = pPetItem->GetProp();
				if (pProp)
				{
					for (int i = 0; i < PROP_BONUSES; i++)
					{
						if (pProp->dwDestParam[i] != NULL_ID)
							ResetDestParam(pProp->dwDestParam[i], pProp->nAdjParamVal[i]);
					}
				}
			}

			CItemElem* pItemElem = GetVisPetItem();
			if (pItemElem)
			{
				ResetPetVisDST(pItemElem);
				SetVisPetItem(NULL_ID);
				static_cast<CUser*>(this)->AddActivateVisPet(NULL_ID, NULL_ID);
			}
			pAIPet->ResetItem();
		}

		pEatPet->Delete();
	}
	SetEatPetId(NULL_ID);
}

#if __VER >= 15 // __PETVIS
void CMover::SetPetVisDST(CItemElem * pItemElem)
{
	if (pItemElem && pItemElem->IsVisPet())
	{
		AddBuff(BUFF_EQUIP, (WORD)(pItemElem->m_dwItemId), 1, 999999999);

		vector<BYTE> vecValidTable = GetValidVisTable(pItemElem);
		for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
		{
			ItemProp* pItemProp = prj.GetItemProp(pItemElem->GetPiercingItem(i));
			if (vecValidTable[i] == SUCCSESS_NEEDVIS && pItemProp)
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					SetDestParam(i, pItemProp);
			}
			else
			{
				if (IsPlayer() && pItemProp)
					static_cast<CUser*>(this)->AddDefinedText(TID_GAME_BUFFPET_NOTREQVIS, "\"%s\"", pItemProp->szName);
			}
		}
	}
}

void CMover::ResetPetVisDST(CItemElem * pItemElem)
{
	if (pItemElem && pItemElem->IsVisPet())
	{
		m_buffs.RemoveBuff(BUFF_EQUIP, (WORD)(pItemElem->m_dwItemId), FALSE);

		vector<BYTE> vecValidTable = GetValidVisTable(pItemElem);
		for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
		{
			ItemProp* pItemProp = prj.GetItemProp(pItemElem->GetPiercingItem(i));
			if (vecValidTable[i] == SUCCSESS_NEEDVIS && pItemProp)
			{
				for (int i = 0; i < PROP_BONUSES; i++)
					ResetDestParam(i, pItemProp);
			}
		}
	}
}

void CMover::ProcessVisPet()
{
	if (HasActivatedVisPet())
	{
		CItemElem* pItemElem = GetVisPetItem();
		if (pItemElem)
		{
			for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
			{
				if (pItemElem->GetPiercingItem(i) != 0 && time_null() >= pItemElem->GetVisKeepTime(i))
				{
					ItemProp* pItemProp = prj.GetItemProp(pItemElem->GetPiercingItem(i));
					if (pItemProp)
						static_cast<CUser*>(this)->AddDefinedText(TID_GAME_BUFFPET_TIMEOUTVIS, "\"%s\"", pItemProp->szName);
					CItemUpgrade::GetInstance()->RemovePetVisItem(static_cast<CUser*>(this), i, TRUE);
				}
			}
		}
	}
}
#endif // __PETVIS

#endif	// __WORLDSERVER

#if __VER >= 15 // __PETVIS
vector<BYTE> CMover::GetValidVisTable(CItemElem * pItemElem)
{
	vector<BYTE> vecTemp;
	vecTemp.resize(pItemElem->GetPiercingSize(), UNDEFINED_NEEDVIS);

	for (DWORD i = 0; i < vecTemp.size(); i++)
		if (pItemElem->GetPiercingItem(i) == 0)
			vecTemp[i] = FAILED_BOTH_NEEDVIS;

	for (DWORD i = 0; i < vecTemp.size(); i++)
		if (vecTemp[i] == UNDEFINED_NEEDVIS)
			SetValidNeedVis(pItemElem, i, vecTemp);

	return vecTemp;
}

BOOL CMover::SetValidNeedVis(CItemElem * pItemElem, int nPos, vector<BYTE> &vecValid)
{
	if (vecValid[nPos] != UNDEFINED_NEEDVIS)
		return (vecValid[nPos] == SUCCSESS_NEEDVIS);

	ItemProp* pPropVis = prj.GetItemProp(pItemElem->GetPiercingItem(nPos));
	if (!pPropVis)
	{
		vecValid[nPos] = FAILED_BOTH_NEEDVIS;
		return FALSE;
	}

	if (pPropVis->dwReferTarget1 == NULL_ID && pPropVis->dwReferTarget2 == NULL_ID)
	{
		vecValid[nPos] = SUCCSESS_NEEDVIS;
		return TRUE;
	}

	BOOL bResult1 = FALSE, bResult2 = FALSE;
	if (pPropVis->dwReferTarget1 == NULL_ID) bResult1 = TRUE;
	if (pPropVis->dwReferTarget2 == NULL_ID) bResult2 = TRUE;

	for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
	{
		if (pPropVis->dwReferTarget1 == pItemElem->GetPiercingItem(i))
			bResult1 = SetValidNeedVis(pItemElem, i, vecValid);

		if (pPropVis->dwReferTarget2 == pItemElem->GetPiercingItem(i))
			bResult2 = SetValidNeedVis(pItemElem, i, vecValid);

		if (bResult1 && bResult2)
		{
			vecValid[nPos] = SUCCSESS_NEEDVIS;
			return TRUE;
		}
	}

	if (!bResult1 && bResult2)
		vecValid[nPos] = FAILED_1ST_NEEDVIS;
	else if (bResult1 && !bResult2)
		vecValid[nPos] = FAILED_2ND_NEEDVIS;
	else
		vecValid[nPos] = FAILED_BOTH_NEEDVIS;
	return FALSE;
}

BYTE CMover::IsSatisfyNeedVis(CItemElem * pItemElemVisPet, ItemProp * pItemPropVis)
{
	if (pItemPropVis)
	{
		DWORD	dwNeeds[2] = { pItemPropVis->dwReferTarget1, pItemPropVis->dwReferTarget2 };
		if (dwNeeds[0] == NULL_ID && dwNeeds[1] == NULL_ID)
			return SUCCSESS_NEEDVIS;

		vector<BYTE> vecValidTable = GetValidVisTable(pItemElemVisPet);
		for (int i = 0; i < pItemElemVisPet->GetPiercingSize(); i++)
		{
			DWORD dwVis = pItemElemVisPet->GetPiercingItem(i);
			if (dwNeeds[0] == dwVis && vecValidTable[i] == SUCCSESS_NEEDVIS && time_null() < pItemElemVisPet->GetVisKeepTime(i))	dwNeeds[0] = NULL_ID;
			if (dwNeeds[1] == dwVis && vecValidTable[i] == SUCCSESS_NEEDVIS && time_null() < pItemElemVisPet->GetVisKeepTime(i))	dwNeeds[1] = NULL_ID;
			if (dwNeeds[0] == NULL_ID && dwNeeds[1] == NULL_ID)
				return SUCCSESS_NEEDVIS;
		}

		if (dwNeeds[0] != NULL_ID && dwNeeds[1] == NULL_ID)
			return FAILED_1ST_NEEDVIS;
		else if (dwNeeds[0] == NULL_ID && dwNeeds[1] != NULL_ID)
			return FAILED_2ND_NEEDVIS;
	}

	return FAILED_BOTH_NEEDVIS;
}
#endif // __PETVIS

int CMover::DoUseItemPetTonic(CItemElem * pItemElem)
{
	if ((pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TONIC_A && HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B))
		|| (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_TONIC_B && HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_A))
		)
		return 2;


	DWORD dwCurr = ::timeGetTime();
	IBuff* pBuff = m_buffs.GetBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId));
	if (pBuff && static_cast<int>(pItemElem->GetProp()->dwSkillTime) > static_cast<int>(pItemElem->GetProp()->dwSkillTime + (pBuff->GetTotal() - (dwCurr - pBuff->GetInst()))))
		return 1;


	DoApplySkill(this, pItemElem->GetProp(), NULL);
	return 0;
}