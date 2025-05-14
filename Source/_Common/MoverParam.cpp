#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "defineQuest.h"
#include "resdata.h"
#include "defineObj.h"
#ifdef __WORLDSERVER
#include "DialogMsg.h"
#include "user.h"
#include "dpcoreclient.h"
#include "spevent.h"
#include "slord.h"
extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER

#ifdef __CLIENT
#include "Dialogmsg.h"
#endif

#include "Party.h"
extern	CPartyMng	g_PartyMng;

#ifdef __WORLDSERVER
#include "DPSrvr.h"
extern	CDPSrvr		g_DPSrvr;
#endif // __WORLDSERVER

#include "Project.h"
#include "ProjectCmn.h"

#ifdef __WORLDSERVER
#include "Environment.h"
#include "GuildHouse.h"
#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif
#endif // __WORLDSERVER

LPSKILL CMover::GetSkill(int nType, int nIdx)
{
	if (nIdx >= MAX_SKILL_JOB)
	{
		Error("CMover::GetSkill : %s nId = %d ������ ���", m_szName, nIdx);
		nIdx = 0;
	}
	return &m_aJobSkill[nIdx];
}
LPSKILL CMover::GetSkill(DWORD dwSkill)
{
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_aJobSkill[i].dwSkill == dwSkill)
			return &m_aJobSkill[i];
	}
	return NULL;
}

int		CMover::GetSkillIdx(DWORD dwSkill)
{
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_aJobSkill[i].dwSkill == dwSkill)
			return i;
	}
	return -1;
}


BOOL CMover::CheckSkill(DWORD dwSkill)
{
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL || pSkillProp->nLog == 1)
		return FALSE;

	if (!IsMaster() && !IsHero() && !IsLegendHero() && GetLevel() < (int)(pSkillProp->dwReqDisLV))
		return FALSE;

	if (pSkillProp->dwReSkill1 != 0xffffffff)
	{
		LPSKILL pSkillBuf = GetSkill(pSkillProp->dwReSkill1);
		if (pSkillBuf)
		{
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1)
				return FALSE;
		}
	}

	if (pSkillProp->dwReSkill2 != 0xffffffff)
	{
		LPSKILL pSkillBuf = GetSkill(pSkillProp->dwReSkill2);
		if (pSkillBuf)
		{
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2)
				return FALSE;
		}
	}

	return TRUE;
}

void CMover::PutLvUpSkillName_1(DWORD dwLevel)
{
#ifdef __CLIENT
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_aJobSkill[i].dwSkill == NULL_ID)
			continue;

		ItemProp* pSkillProp = prj.GetSkillProp(m_aJobSkill[i].dwSkill);

		if (pSkillProp && dwLevel == pSkillProp->dwReqDisLV)
		{
			if (pSkillProp->dwReSkill1 != -1)
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill1);

				if (pSkillProp->dwReSkillLevel1 > pSkill->dwLevel)
					continue;
			}
			if (pSkillProp->dwReSkill2 != -1)
			{
				LPSKILL pSkill = GetSkill(pSkillProp->dwReSkill2);

				if (pSkillProp->dwReSkillLevel2 > pSkill->dwLevel)
					continue;
			}

			CString str;
			str.Format(prj.GetText(TID_GAME_REAPSKILL), pSkillProp->szName);
			g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_REAPSKILL));
		}
	}
#endif //__CLIENT
}

BOOL IsSkillParent(CMover* pMover, DWORD dwSkill, DWORD dwChildSkill)
{
	LPSKILL pSkill = pMover->GetSkill(dwSkill);
	if (pSkill == NULL)
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL)
		return FALSE;

	if (pSkillProp->dwReSkill1 == dwChildSkill || pSkillProp->dwReSkill2 == dwChildSkill)
		return TRUE;

	return FALSE;
}

BOOL IsActive(CMover* pMover, DWORD dwSkill)
{
	LPSKILL pSkill = pMover->GetSkill(dwSkill);
	if (pSkill == NULL)
		return FALSE;

	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
	if (pSkillProp == NULL)
		return FALSE;

	if (pSkillProp->dwReSkill1 != NULL_ID)
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;

		pSkill1 = pMover->GetSkill(pSkillProp->dwReSkill1);
		if (pSkill1 == NULL)
			return FALSE;

		pSkillProp1 = prj.GetSkillProp(pSkill1->dwSkill);
		if (pSkillProp1 == NULL)
			return FALSE;

		if ((int)(pSkillProp1->dwReqDisLV) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero())
			return FALSE;

		if (pSkillProp->dwReSkillLevel1 != pSkill1->dwLevel)
			return FALSE;
	}

	if (pSkillProp->dwReSkill2 != NULL_ID)
	{
		LPSKILL pSkill1;
		ItemProp* pSkillProp1;

		pSkill1 = pMover->GetSkill(pSkillProp->dwReSkill2);
		if (pSkill1 == NULL)
			return FALSE;

		pSkillProp1 = prj.GetSkillProp(pSkill1->dwSkill);
		if (pSkillProp1 == NULL)
			return FALSE;

		if ((int)(pSkillProp1->dwReqDisLV) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero())
			return FALSE;

		if (pSkillProp->dwReSkillLevel2 != pSkill1->dwLevel)
			return FALSE;
	}

	if ((int)(pSkillProp->dwReqDisLV) > pMover->GetLevel() && !pMover->IsMaster() && !pMover->IsHero() && !pMover->IsLegendHero())
		return FALSE;

	return TRUE;
}



void CMover::PutLvUpSkillName_2(DWORD dwSkill)
{
#ifdef __CLIENT
	LPSKILL pSkill = GetSkill(dwSkill);
	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);


	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_aJobSkill[i].dwSkill == NULL_ID)
			continue;

		if (IsSkillParent(this, m_aJobSkill[i].dwSkill, dwSkill))
		{
			if (IsActive(this, m_aJobSkill[i].dwSkill))
			{
				ItemProp* pSkillProp1;
				pSkillProp1 = prj.GetSkillProp(m_aJobSkill[i].dwSkill);

				CString str;
				str.Format(prj.GetText(TID_GAME_REAPSKILL), pSkillProp1->szName);
				g_WndMng.PutString((LPCTSTR)str, NULL, prj.GetTextColor(TID_GAME_REAPSKILL));
			}
		}
	}
#endif // CLIENT
}

#ifdef __WORLDSERVER
DWORD CMover::GetSummonState(BOOL bIsSummoner)
{
	// If no world, then cant summon and cant be summoned
	CWorld* pWorld = GetWorld();
	if (!pWorld)
		return TID_GAME_SMELT_SAFETY_ERROR16;

	// If world is gw, ffa, sr or clockworks zone, then cant summon and cant be summoned
	DWORD dwWorldID = pWorld->GetID();
	if (dwWorldID == WI_WORLD_GUILDWAR || dwWorldID == WI_WORLD_FFA
#ifdef __FL_TDM
		|| dwWorldID == WI_WORLD_TDM
#endif
		)
		return TID_ERROR_NOFLY;

	if (dwWorldID == WI_DUNGEON_SECRET_0 || (dwWorldID >= WI_DUNGEON_SECRET_1 && dwWorldID <= WI_DUNGEON_SECRET_L))
		return TID_ERROR_NOFLY;

	if (dwWorldID == WI_WORLD_MADRIGAL && prj.IsGuildQuestRegion(GetPos()))
		return TID_ERROR_NOFLY;

	// If dead, then cant summon and cant be summoned
	if (IsDie())
		return TID_GAME_SMELT_SAFETY_ERROR16;

	// If is a guy who summoned other guy
	if (bIsSummoner)
	{
		// If while flying, then cant summon
		if (IsFly())
			return TID_PK_FLIGHT_NOUSE;
	}
	else // If is a guy who was summoned BY other guy
	{
		// If trading then cant teleport
		if (m_vtInfo.GetOther() != NULL)
			return TID_GAME_TRADELIMITUSING;

		// If vending then cant teleport
		if (m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen())
			return TID_GAME_TRADELIMITUSING;

		// If bank or guildbank opened then cant teleport
		if (m_bBank || m_bGuildBank)
			return TID_GAME_TRADELIMITUSING;

		// If collecting then cant teleport
		if (IsCollecting())
			return TID_GAME_SMELT_SAFETY_ERROR16;

		// If attacking/casting skills then cant teleport
		if (m_pActMover->IsActAttack())
			return TID_GAME_ATTACK_NOTUSE;
	}

	return 0;
}
#endif


#ifdef __CLIENT
LPCTSTR CMover::GetJobString()
{
	if (m_nJob != -1)
		return prj.m_aJob[m_nJob].szName;
	return _T("");
}
#endif	// __CLIENT

BOOL  CMover::IsJobType(DWORD dwJobType)
{
	if (IsExpert())
	{
		if (dwJobType == JTYPE_EXPERT)
			return TRUE;
		return FALSE;
	}
	return JTYPE_BASE == dwJobType;
}
BOOL CMover::IsBaseJob()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_BASE;
}
BOOL CMover::IsExpert()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_EXPERT;
}
BOOL CMover::IsPro()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_PRO;
}

BYTE	CMover::GetLegendChar()
{
	if (IsMaster())
		return LEGEND_CLASS_MASTER;
	else if (IsHero())
		return	LEGEND_CLASS_HERO;
	else if (IsLegendHero())
		return LEGEND_CLASS_LEGENDHERO;
	else
		return LEGEND_CLASS_NORMAL;
}

BOOL	CMover::IsMaster()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_MASTER;//temp JTYPE_MASTER;
}

BOOL	CMover::IsHero()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_HERO;//temp JTYPE_HERO;
}

BOOL	CMover::IsLegendHero()
{
	return prj.m_aJob[m_nJob].dwJobType == JTYPE_LEGEND_HERO;
}


BOOL CMover::IsInteriorityJob(int nJob)
{
	if (nJob == JOB_VAGRANT || nJob == m_nJob)
		return TRUE;

	if (IsPro() && JOB_VAGRANT < nJob && nJob < MAX_EXPERT)
	{
		if (nJob * 2 + 4 == m_nJob || nJob * 2 + 5 == m_nJob)
		{
			return TRUE;
		}
	}

	if (IsMaster())
	{
		if (nJob < MAX_EXPERT)
		{
			if (nJob * 2 + 14 == m_nJob || nJob * 2 + 15 == m_nJob)
				return TRUE;
		}
		else if (nJob < MAX_PROFESSIONAL)
		{
			if (nJob + 10 == m_nJob)
				return TRUE;
		}
	}
	if (IsHero())
	{
		if (nJob < MAX_EXPERT)
		{
			if (nJob * 2 + 22 == m_nJob || nJob * 2 + 23 == m_nJob)
				return TRUE;
		}
		else if (nJob < MAX_PROFESSIONAL)
		{
			if (nJob + 18 == m_nJob)
				return TRUE;
		}

		else if (nJob < MAX_MASTER)
		{
			if (nJob + 8 == m_nJob)
				return TRUE;
		}

	}

	if (IsLegendHero())
	{
		if (nJob < MAX_EXPERT)
		{
			if (nJob * 2 + 30 == m_nJob || nJob * 2 + 31 == m_nJob
				)
				return TRUE;
		}
		else if (nJob < MAX_PROFESSIONAL)
		{
			if (nJob + 26 == m_nJob
				)
				return TRUE;
		}
		else if (nJob < MAX_MASTER)
		{
			if (nJob + 16 == m_nJob
				)
				return TRUE;
		}
		else if (nJob < MAX_HERO)
		{
			if (nJob + 8 == m_nJob
				)
				return TRUE;
		}
	}

	return FALSE;
}



BOOL  CMover::SetExpert(int nJob)
{
	if (m_nJob == nJob)
		return FALSE;
	m_nJob = nJob;

	int nNum = 3;
	LPSKILL lpSkill;
	ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
	int nJobNum = prj.m_aJobSkillNum[m_nJob];
	for (int i = 0; i < nJobNum; i++)
	{
		ItemProp* pSkillProp = apSkillProp[i];
		lpSkill = &m_aJobSkill[nNum++];
		lpSkill->dwSkill = pSkillProp->dwID;
	}
	return TRUE;
}

int   CMover::GetJob()
{
	return m_nJob;
}

int   CMover::GetExpPercent()
{
	int nExpPercent = 0;

	nExpPercent = (int)(GetExp1() * 10000 / GetMaxExp1());

	return nExpPercent;
}


int   CMover::GetRemainGP()
{
	return m_nRemainGP;
}


int	CMover::GetAdjHitRate(void)
{
	return GetParam(DST_ADJ_HITRATE, m_nAdjHitRate);
}


int CMover::GetParrying()
{
	if (IsPlayer())
		return	((int)((GetDex() * 0.5f) + GetParam(DST_PARRY, m_nAdjParry)));
	else
		return GetProp()->dwER;
}


void CMover::UpdateParam()
{
	if (IsPlayer())
	{
		SumEquipDefenseAbility(&m_nDefenseMin, &m_nDefenseMax);
		SumEquipAdjValue(&m_nAdjHitRate, &m_nAdjParry);
	}
	else
	{
		m_nDefenseMin = m_nDefenseMax = GetNaturalArmor();
		m_nAdjHitRate = 0;
		m_nAdjParry = 0;
	}
}

int CMover::GetItemAbility(int nItem)
{
	ItemProp* pItemProp = prj.GetItemProp(nItem);
	int nAbilityMin = GetParam(DST_ABILITY_MIN, pItemProp->dwAbilityMin);
	int nAbilityMax = GetParam(DST_ABILITY_MAX, pItemProp->dwAbilityMax);
	if (nAbilityMax == nAbilityMin)
		return nAbilityMin;
	return nAbilityMin + (xRand() % (nAbilityMax - nAbilityMin));
}

int CMover::GetItemAbilityMin(int nItem)
{
	ItemProp* pItemProp = prj.GetItemProp(nItem);
	int nAbilityMin = GetParam(DST_ABILITY_MIN, pItemProp->dwAbilityMin);
	return nAbilityMin;
}

int CMover::GetItemAbilityMax(int nItem)
{
	ItemProp* pItemProp = prj.GetItemProp(nItem);
	int nAbilityMax = GetParam(DST_ABILITY_MAX, pItemProp->dwAbilityMax);
	return nAbilityMax;
}


void CMover::IncHitPoint(int nVal)
{
	SetPointParam(DST_HP, m_nHitPoint + nVal);
}

void CMover::IncManaPoint(int nVal)
{
#ifdef __WORLDSERVER
	if (IsSMMode(SM_MAINTAIN_MP) && nVal < 0)
		return;
#endif // __WORLDSERVER
	SetPointParam(DST_MP, m_nManaPoint + nVal);
}

void CMover::IncFatiguePoint(int nVal)
{
#ifdef __WORLDSERVER
	if (IsSMMode(SM_MAINTAIN_FP) && nVal < 0)
		return;
#endif // __WORLDSERVER
	SetPointParam(DST_FP, m_nFatiguePoint + nVal);
}

void CMover::SetHitPoint(int nVal)
{
	m_nHitPoint = nVal;
	if (m_nHitPoint < 0)
		m_nHitPoint = 0;
	if (m_nHitPoint > GetMaxHitPoint())
		m_nHitPoint = GetMaxHitPoint();
}

void CMover::SetManaPoint(int nVal)
{
	m_nManaPoint = nVal;
	if (m_nManaPoint < 0)
		m_nManaPoint = 0;
	if (m_nManaPoint > GetMaxManaPoint())
		m_nManaPoint = GetMaxManaPoint();
}

void CMover::SetFatiguePoint(int nVal)
{
	m_nFatiguePoint = nVal;
	if (m_nFatiguePoint < 0)
		m_nFatiguePoint = 0;
	if (m_nFatiguePoint > GetMaxFatiguePoint())
		m_nFatiguePoint = GetMaxFatiguePoint();
}

int CMover::GetPointParam(int nDestParameter)
{
	switch (nDestParameter)
	{
	case DST_HP:	return(GetHitPoint());
	case DST_MP:	return(GetManaPoint());
	case DST_FP:	return(GetFatiguePoint());
	case DST_GOLD:	return(GetGold());
	}
	return(0);
}

void CMover::SetPointParam(int nDstParameter, int nValue, BOOL bTrans)
{
#ifdef __WORLDSERVER
	int nMax = nValue;
#endif // __WORLDSERVER
	if (nValue < 0)
		nValue = 0;

	BOOL	bTransfer = bTrans;

	switch (nDstParameter)
	{
	case DST_HP:
#ifdef __WORLDSERVER
		if (nValue > GetMaxHitPoint() || nMax == 999999999)	nValue = GetMaxHitPoint();
#else // __WORLDSERVER
		if (nValue > GetMaxHitPoint())	nValue = GetMaxHitPoint();
#endif // __WORLDSERVER
		if (m_nHitPoint != nValue)
		{
			bTransfer = TRUE;
			m_nHitPoint = nValue;
		}
		break;
	case DST_MP:
#ifdef __WORLDSERVER
		if (nValue > GetMaxManaPoint() || nMax == 999999999)	nValue = GetMaxManaPoint();
#else // __WORLDSERVER
		if (nValue > GetMaxManaPoint())	nValue = GetMaxManaPoint();
#endif // __WORLDSERVER
		if (m_nManaPoint != nValue)
		{
			bTransfer = TRUE;
			m_nManaPoint = nValue;
		}
		break;
	case DST_FP:
#ifdef __WORLDSERVER
		if (nValue > GetMaxFatiguePoint() || nMax == 999999999)	nValue = GetMaxFatiguePoint();
#else // __WORLDSERVER
		if (nValue > GetMaxFatiguePoint())		nValue = GetMaxFatiguePoint();
#endif // __WORLDSERVER
		if (m_nFatiguePoint != nValue)
		{
			bTransfer = TRUE;
			m_nFatiguePoint = nValue;
		}
		break;
	case DST_GOLD:
#ifdef __WORLDSERVER
		Error("SetPointParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nValue);
		return;
#endif // 

#ifdef __CLIENT
		if (IsActiveMover())
		{
			PLAYSND(SND_INF_TRADE);

			int nOldGold = GetGold();
			int nNewGold = nValue;
			int nGoldDifference = 0;

			CString str;
			if (nNewGold > nOldGold)
			{
				nGoldDifference = nNewGold - nOldGold;
				str.Format("You received: %s Penya (Total: %s Penya).", Separator(nGoldDifference), Separator(nValue));

#ifdef __FL_FARM_DISPLAY
				CWndFarmDisplay* pWndBase = (CWndFarmDisplay*)g_WndMng.GetWndBase(APP_FARM_DISPLAY);
				if (nGoldDifference != PERIN_VALUE && pWndBase)
					pWndBase->AddPenya(nGoldDifference);
#endif
			}
			else
			{
				nGoldDifference = nOldGold - nNewGold;
				str.Format("You paid: %s Penya (Total: %s Penya).", Separator(nGoldDifference), Separator(nValue));
			}

			g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_REAPMONEY));
		}
#endif //__CLIENT
		bTransfer = TRUE;
		SetGold(nValue);
		break;
	}
#ifdef __WORLDSERVER
	if (bTransfer)
		g_UserMng.AddSetPointParam(this, nDstParameter, nValue);
#endif	// __WORLDSERVER
}

BOOL CMover::AddExperience(EXPINTEGER nExp, BOOL bFirstCall, BOOL bMultiPly, BOOL bMonster)
{
	if (IsMode(MODE_EXPUP_STOP))
		return FALSE;

	if (nExp <= 0)
		return FALSE;

	if (m_nHitPoint <= 0)
		return FALSE;

	if (IsMaster() || IsHero() || IsLegendHero())
		nExp /= 2;

#ifdef __WORLDSERVER
	if (bFirstCall && bMultiPly)
	{
		nExp *= GetExpFactor();
	}
#endif // __WORLDSERVER

	if (IsBaseJob())
	{
		if (m_nLevel >= MAX_JOB_LEVEL)
		{
			m_nExp1 = 0;
			return TRUE;
		}
	}
	else if (IsExpert())
	{
		if (m_nLevel >= MAX_JOB_LEVEL + MAX_EXP_LEVEL)
		{
			m_nExp1 = 0;
			return TRUE;
		}
	}

	else if (IsPro())
	{
		if (m_nLevel > MAX_GENERAL_LEVEL)
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			return TRUE;
		}
	}
	else if (IsMaster())
	{
		if (m_nLevel > MAX_GENERAL_LEVEL)
		{
			m_nLevel = MAX_GENERAL_LEVEL;
			return TRUE;
		}
	}
	else if (IsHero())
	{
		if (m_nLevel > MAX_LEGEND_LEVEL)
		{
			m_nLevel = MAX_LEGEND_LEVEL;
			return TRUE;
		}
	}
	else if (IsLegendHero())
	{
		if (m_nLevel > MAX_3RD_LEGEND_LEVEL)
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;
			return TRUE;
		}
	}

	int nLevelbuf = m_nLevel;
	int nNextLevel = m_nLevel + 1;

	m_nExp1 += nExp;

	if (m_nExp1 >= prj.m_aExpCharacter[nNextLevel].nExp1)
	{
		if (IsLegendHero() && (nNextLevel > MAX_3RD_LEGEND_LEVEL))
		{
			m_nLevel = MAX_3RD_LEGEND_LEVEL;

			m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
		else if (IsHero() && (nNextLevel > MAX_LEGEND_LEVEL))
		{
			m_nLevel = MAX_LEGEND_LEVEL;

			m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}
		else if (!IsHero() && !IsLegendHero() && nNextLevel > MAX_GENERAL_LEVEL)
		{
			m_nLevel = MAX_GENERAL_LEVEL;

			m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
			return FALSE;
		}

		EXPINTEGER nExptmp;
		{
			m_nRemainGP += prj.m_aExpCharacter[nNextLevel].dwLPPoint;

			if (IsMaster() || IsHero() || IsLegendHero())
				m_nRemainGP++;


			nExptmp = m_nExp1 - prj.m_aExpCharacter[nNextLevel].nExp1;
			m_nExp1 = 0;
			m_nLevel = nNextLevel;

			BOOL bLevelUp = TRUE;
			if (IsBaseJob() && m_nLevel > MAX_JOB_LEVEL)
			{
				m_nLevel = MAX_JOB_LEVEL;
				bLevelUp = FALSE;
			}
			else if (IsExpert() && m_nLevel > MAX_JOB_LEVEL + MAX_EXP_LEVEL)
			{
				m_nLevel = MAX_JOB_LEVEL + MAX_EXP_LEVEL;
				bLevelUp = FALSE;
			}
			else if (IsPro() && m_nLevel > MAX_GENERAL_LEVEL)
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;

				m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp = 0;
				return FALSE;
			}
			else if (IsMaster() && m_nLevel > MAX_GENERAL_LEVEL)
			{
				m_nLevel = MAX_GENERAL_LEVEL;
				bLevelUp = FALSE;
				m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp = 0;
				return FALSE;
			}
			else if (IsHero() && (m_nLevel > MAX_LEGEND_LEVEL))
			{
				m_nLevel = MAX_LEGEND_LEVEL;
				bLevelUp = FALSE;
				m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp = 0;
				return FALSE;
			}
			else if (IsLegendHero() && m_nLevel > MAX_3RD_LEGEND_LEVEL)
			{
				m_nLevel = MAX_3RD_LEGEND_LEVEL;
				bLevelUp = FALSE;
				m_nExp1 = (prj.m_aExpCharacter[nNextLevel].nExp1 - 1);
				nExptmp = 0;
				return FALSE;
			}

			if (bLevelUp)
			{
				m_nHitPoint = GetMaxHitPoint();
				m_nManaPoint = GetMaxManaPoint();
				m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __WORLDSERVER
				int nGetPoint = 0;

				if (IsLegendHero() == TRUE)
				{
					if (GetJob() == JOB_LORDTEMPLER_HERO
						|| GetJob() == JOB_FLORIST_HERO
						)
						nGetPoint = SKILLPOINT_LEGENDHERO_EXCEPTION;
					else
						nGetPoint = SKILLPOINT_LEGENDHERO_BASE;
				}
				else
					nGetPoint = ((GetLevel() - 1) / SKILLPOINT_DIVIDE) + SKILLPOINT_BASE;

				if (IsMaster() || IsHero())
					SetMasterSkillPointUp();
				else
					AddSkillPoint(nGetPoint);

#ifdef __EVENTLUA_GIFT
				prj.m_EventLua.SetLevelUpGift((CUser*)this, m_nLevel);
#endif // __EVENTLUA_GIFT
#endif // __WORLDSERVER
			}
			else
			{
				m_nRemainGP -= prj.m_aExpCharacter[nNextLevel].dwLPPoint;

				if (IsMaster() || IsHero() || IsLegendHero())
					m_nRemainGP--;

				nExptmp = m_nExp1 = 0;
			}

#ifdef __CLIENT
			if (m_pActMover && (m_pActMover->IsState(OBJSTA_STAND) || m_pActMover->IsState(OBJSTA_STAND2)))
				SetMotion(MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED);
			CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, GetPos(), GetId());
			PlayMusic(BGM_IN_LEVELUP);
#endif	// __CLIENT
		}

		if (nExptmp > 0)
			AddExperience(nExptmp, FALSE, bMultiPly);
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::AddFxp(int nFxp)
{
	if (nFxp <= 0)
		return FALSE;

	if (m_nHitPoint <= 0)
		return FALSE;

	if (GetFlightLv() == 0)
		return FALSE;

	int nNextLevel = GetFlightLv() + 1;

	m_nFxp += nFxp;
	if ((DWORD)(m_nFxp) >= prj.m_aFxpCharacter[nNextLevel].dwFxp)
	{
		int		FxpBuf;
		FxpBuf = m_nFxp - prj.m_aFxpCharacter[nNextLevel].dwFxp;
		m_nFxp = 0;

#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, GetPos(), GetId());
		PlayMusic(BGM_IN_LEVELUP);
#endif	// __CLIENT


		if (FxpBuf > 0)
		{
			AddFxp(FxpBuf);
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CMover::AddChangeJob(int nJob)
{
	BOOL	bResult = FALSE;
	LPSKILL lpSkill;

	if (nJob == JOB_VAGRANT)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[nJob];
		int nJobNum = prj.m_aJobSkillNum[nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
	}

	if (MAX_JOBBASE <= nJob && nJob < MAX_EXPERT)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i + MAX_JOB_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	if (MAX_EXPERT <= nJob && nJob < MAX_PROFESSIONAL)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i + MAX_JOB_SKILL + MAX_EXPERT_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}

	if (MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
			lpSkill->dwLevel = 1;
		}
		bResult = TRUE;
	}

	if (MAX_MASTER <= nJob && nJob < MAX_HERO)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}
	else if (MAX_HERO <= nJob && nJob < MAX_LEGEND_HERO)
	{
		m_nJob = nJob;

		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i + MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
		bResult = TRUE;
	}

#ifdef __WORLDSERVER
	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange(this, m_nHonor);
#endif // __WORLDSERVER
	return bResult;
}

BOOL CMover::SetFxp(int nFxp, int nFlightLv)
{
	m_nFxp = nFxp;


	if (nFlightLv > GetFlightLv())
	{
#ifdef __CLIENT
		CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, GetPos(), GetId());
		PlayMusic(BGM_IN_LEVELUP);
		g_WndMng.PutString(prj.GetText(TID_GAME_FLYLVLUP), NULL, prj.GetTextColor(TID_GAME_FLYLVLUP));
#endif // CLIENT
		return TRUE;
	}

	return FALSE;
}

BOOL CMover::SetExperience(EXPINTEGER nExp1, int nLevel)
{
	m_nExp1 = nExp1;

	if (IsInvalidObj(this))
		return 0;

#ifdef __JOB_CHANGE
#ifdef __CLIENT
	if ((GetLevel() == MAX_GENERAL_LEVEL && IsPro() && GetExpPercent() == 9999)
		|| (GetLevel() == MAX_GENERAL_LEVEL && IsMaster() && GetExpPercent() == 9999))
	{
		if (!g_WndMng.GetApplet(APP_CHANGE_JOB_03))
			g_WndMng.CreateApplet(APP_CHANGE_JOB_03);
	}
#endif // __CLIENT
#endif

	if (nLevel > m_nLevel)
	{
#ifdef __CLIENT

		if (nLevel == 15)
			g_Option.m_nInstantHelp = 0;

		PutLvUpSkillName_1(nLevel);

		if (m_pActMover && (m_pActMover->IsState(OBJSTA_STAND) || m_pActMover->IsState(OBJSTA_STAND2)))
			SetMotion(MTI_LEVELUP, ANILOOP_1PLAY, MOP_FIXED);
		CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_LEVEL_UP01, GetPos(), GetId());
		PlayMusic(BGM_IN_LEVELUP);
		g_WndMng.PutString(prj.GetText(TID_GAME_LEVELUP), NULL, prj.GetTextColor(TID_GAME_LEVELUP));

		if (g_WndMng.m_pWndWorld)
		{
			if (::GetLanguage() != LANG_JAP)
				g_Caption1.AddCaption(prj.GetText(TID_GAME_LEVELUP_CAPTION), g_WndMng.m_pWndWorld->m_pFontAPICaption);// CWndBase::m_Theme.m_pFontCaption );
			else
				g_Caption1.AddCaption(prj.GetText(TID_GAME_LEVELUP_CAPTION), NULL);// CWndBase::m_Theme.m_pFontCaption );
		}

		// 2, 8, 12
		static	int nPatLv[4] = { 2, 8, 12, 0 };
		for (int i = 0; i < 3; i++) {
			if (m_nLevel < nPatLv[i] && nLevel >= nPatLv[i]) {
				g_WndMng.PutString(prj.GetText(TID_GAME_PATTERNUP), NULL, prj.GetTextColor(TID_GAME_PATTERNUP));
				break;
			}
			else if (m_nLevel >= nPatLv[2])
				break;
		}

		if (TRUE == IsJobType(JTYPE_BASE))
		{
			if (nLevel == 15)
				g_WndMng.PutString(prj.GetText(TID_EVE_LEVEL15), NULL, prj.GetTextColor(TID_EVE_LEVEL15));

			static	int nNum = prj.m_aJobSkillNum[JOB_VAGRANT];
		}
		if (m_nLevel < 20 && nLevel >= 20)
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_FLYLVLUP), NULL, prj.GetTextColor(TID_GAME_FLYLVLUP));
		}
#endif	// __CLIENT
		m_nLevel = nLevel;

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = GetMaxManaPoint();
		m_nFatiguePoint = GetMaxFatiguePoint();

#ifdef __CLIENT
		if (IsPlayer())
		{
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
			if (pWndWorld)
			{
				pWndWorld->GetAdvMgr()->AddAdvButton(APP_CHARACTER3);
#ifdef __JOB_CHANGE
				if (GetLevel() == MAX_JOB_LEVEL && IsBaseJob())
				{
					if (!g_WndMng.GetApplet(APP_CHANGE_JOB_01))
						g_WndMng.CreateApplet(APP_CHANGE_JOB_01);
				}
				if (GetLevel() == (MAX_JOB_LEVEL + MAX_EXP_LEVEL) && IsExpert())
				{
					if (!g_WndMng.GetApplet(APP_CHANGE_JOB_02))
						g_WndMng.CreateApplet(APP_CHANGE_JOB_02);
				}
				if (GetLevel() == MAX_LEGEND_LEVEL && IsHero())
				{
					if (!g_WndMng.GetApplet(APP_CHANGE_JOB_03))
						g_WndMng.CreateApplet(APP_CHANGE_JOB_03);
				}
#endif
			}
		}
#endif //__CLIENT

		return TRUE;
	}
	else if (nLevel < m_nLevel)
	{
		m_nLevel = nLevel;
	}


	return FALSE;
}


void CMover::SetEquipDstParam()
{
	if (IsPlayer())
	{
		CItemElem* pItemElem;
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			pItemElem = GetEquipItem(i);
			if (pItemElem)
				SetDestParam(pItemElem->m_dwItemId, FALSE);	//don't send
		}
	}
}


/////////////////////////////////////////////////////////////////////////////// aaaaaaaaa
int CMover::SumEquipDefenseAbility(LONG* pnMin, LONG* pnMax)
{
	if (IsPlayer())
	{
		int	nMin = 0, nMax = 0;
		CItemElem* pItemElem;
		ItemProp* pItemProp;

		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			pItemElem = GetEquipItem(i);
			if (pItemElem)
			{
				pItemProp = pItemElem->GetProp();

				if (pItemProp == NULL)
				{
					Error("Parts Not Property = %d", i);
				}

				if (pItemProp)
				{
					if (pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC)
					{
						int nOption = pItemElem->GetAbilityOption();
						int nOptionVal = 0;
						if (nOption > 0)
							nOptionVal = (int)(pow((float)(nOption), 1.5f));

						float f = GetItemMultiplier(pItemElem);
						nMin += (int)(pItemProp->dwAbilityMin * f) + nOptionVal;
						nMax += (int)(pItemProp->dwAbilityMax * f) + nOptionVal;
						#ifdef __WEAPON_RARITY
						if (pItemElem->m_nWeaponRarity > 0)
						{
							int nMinRarity;
							int nMaxRarity;
							switch (pItemElem->m_nWeaponRarity)
							{
							case 5:
								nMinRarity = (int)(nMin * 0.25f);
								nMaxRarity = (int)(nMax * 0.25f);
								break;
							case 4:
								nMinRarity = (int)(nMin * 0.2f);
								nMaxRarity = (int)(nMax * 0.2f);
								break;
							case 3:
								nMinRarity = (int)(nMin * 0.15f);
								nMaxRarity = (int)(nMax * 0.15f);
								break;
							case 2:
								nMinRarity = (int)(nMin * 0.1f);
								nMaxRarity = (int)(nMax * 0.1f);
								break;
							case 1:
								nMinRarity = (int)(nMin * 0.05f);
								nMaxRarity = (int)(nMax * 0.05f);
								break;
							}
							if (nMinRarity < 1)
								nMin += 1;
							else
								nMin += nMinRarity;

							if (nMaxRarity < 1)
								nMax += 1;
							else
								nMax += nMaxRarity;
						}
					#endif // __WEAPON_RARITY
					}
				}
			}
		}

		nMin = GetParam(DST_ABILITY_MIN, nMin);
		nMax = GetParam(DST_ABILITY_MAX, nMax);
		if (pnMin) *pnMin = nMin;
		if (pnMax) *pnMax = nMax;
		return ((nMin + nMax) / 2);
	}
	else
	{
	}

	return 0;
}

void CMover::SumEquipAdjValue(int* pnAdjHitRate, int* pnAdjParray)
{
	*pnAdjHitRate = 0;
	*pnAdjParray = 0;

	CItemElem* pItemElem;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		pItemElem = GetEquipItem(i);
		if (pItemElem)
		{
			if (pItemElem->GetProp())
			{
				if ((int)pItemElem->GetProp()->nAdjHitRate > 0)
					*pnAdjHitRate += pItemElem->GetProp()->nAdjHitRate;
				if ((int)pItemElem->GetProp()->dwParry > 0)
					*pnAdjParray += pItemElem->GetProp()->dwParry;
			}
		}
	}
}

BOOL CMover::SetDarkCover(BOOL bApply)
{
	if (bApply)
	{
		if (GetAdjParam(DST_IMMUNITY) & CHS_INVISIBILITY)
			return FALSE;

		SetDestParam(DST_CHRSTATE, CHS_INVISIBILITY, NULL_CHGPARAM);
	}
	else
	{
		if (GetAdjParam(DST_CHRSTATE) & CHS_INVISIBILITY)
			ResetDestParam(DST_CHRSTATE, CHS_INVISIBILITY, TRUE);
	}

	return TRUE;
}


BOOL CMover::SetStun(BOOL bApply)
{
	if (bApply)
	{
		MoverProp* pMoverProp = GetProp();
		if (pMoverProp)
		{
			if (pMoverProp->dwClass == RANK_MIDBOSS || pMoverProp->dwClass == RANK_BOSS || pMoverProp->dwClass == RANK_SUPER)
				return FALSE;
		}

		if (GetAdjParam(DST_IMMUNITY) & CHS_STUN)
			return FALSE;

		SetDestParam(DST_CHRSTATE, CHS_STUN, NULL_CHGPARAM, FALSE);
		SendActMsg(OBJMSG_STUN);
	}
	else
	{
		if (GetAdjParam(DST_CHRSTATE) & CHS_STUN)
			ResetDestParam(DST_CHRSTATE, CHS_STUN, FALSE);

		m_dwFlag &= (~MVRF_NOACTION);
#ifdef __FIX_STUNBUG
		m_pActMover->ResetState(OBJSTA_ACTION_ALL);
#endif
	}

#ifdef __WORLDSERVER
	g_UserMng.AddSetStun(this, bApply);
#endif // Worldserver

	return TRUE;
}


BOOL CMover::SetPoison(BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, int wDamage)
{
	if (bApply)
	{
		if (GetAdjParam(DST_IMMUNITY) & CHS_POISON)
			return FALSE;
		SetDestParam(DST_CHRSTATE, CHS_POISON, NULL_CHGPARAM);
		if (tmMaxTime == -1)
			Error("SetPoison : %s ��ȿ���� ���ӽð��� �������� ����", GetName());
		if (tmUnit == -1)
			Error("SetPoison : %s ��ȿ���� tick �ð��� �������� ����", GetName());
		if (wDamage == -1)
			Error("SetPoison : %s ��ȿ���� ƽ�� �������� �������� ����", GetName());

		m_wPoisonDamage = wDamage;
		m_tmPoisonUnit = tmUnit;
		m_idPoisonAttacker = idAttacker;
	}
	else
	{
		if (GetAdjParam(DST_CHRSTATE) & CHS_POISON)
			ResetDestParam(DST_CHRSTATE, CHS_POISON, TRUE);

		m_wPoisonDamage = 0;
		m_tmPoisonUnit = 0;
	}

	return TRUE;
}

BOOL	CMover::SetBleeding(BOOL bApply, OBJID idAttacker, DWORD tmMaxTime, DWORD tmUnit, int wDamage)
{
	if (bApply)
	{
		if (GetAdjParam(DST_IMMUNITY) & CHS_BLEEDING)
			return FALSE;

		SetDestParam(DST_CHRSTATE, CHS_BLEEDING, NULL_CHGPARAM);
		if (tmMaxTime == -1)
			Error("SetBleeding : %s ����ȿ���� ���ӽð��� �������� ����", GetName());
		if (tmUnit == -1)
			Error("SetBleeding : %s ����ȿ���� tick �ð��� �������� ����", GetName());
		if (wDamage == -1)
			Error("SetBleeding : %s ����ȿ���� ƽ�� �������� �������� ����", GetName());

		m_wBleedingDamage = wDamage;
		m_tmBleedingUnit = tmUnit;
		m_idBleedingAttacker = idAttacker;
	}
	else
	{
		if (GetAdjParam(DST_CHRSTATE) & CHS_BLEEDING)
			ResetDestParam(DST_CHRSTATE, CHS_BLEEDING, TRUE);

		m_wBleedingDamage = 0;
		m_tmBleedingUnit = 0;
	}
	return TRUE;
}



void CMover::RemoveDebuff(DWORD dwState)
{
	RemoveChrStateBuffs(dwState);

	if (dwState & CHS_POISON)
		SetPoison(FALSE);
	if (dwState & CHS_STUN)
		SetStun(FALSE);
	if (dwState & CHS_BLEEDING)
		SetBleeding(FALSE);

	SetDestParam(DST_CURECHR, dwState, NULL_CHGPARAM);
}

void CMover::RemoveBuffOne(DWORD dwSkill)
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs(RBF_ONCE | RBF_GOODSKILL, 0);
#else	// __BUFF_1107
	m_SkillState.RemoveOneSkillBuff();
#endif	// __BUFF_1107
}

void CMover::RemoveBuffAll(void)
{
#ifdef __BUFF_1107
	m_buffs.RemoveBuffs(RBF_GOODSKILL, 0);
#else	// __BUFF_1107
	m_SkillState.RemoveAllSkillBuff();
#endif	// __BUFF_1107
}


void CMover::SetDestParam(int nIdx, ItemProp* pProp, BOOL bSend)
{
	SetDestParam(pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], pProp->dwChgParamVal[nIdx], bSend);
}

void CMover::ResetDestParam(int nIdx, ItemProp* pProp, BOOL bSend)
{
	ResetDestParam(pProp->dwDestParam[nIdx], pProp->nAdjParamVal[nIdx], bSend);
}


void CMover::SetDestParam(int nItemIdx, BOOL bSend)
{
	ItemProp* pItemProp = prj.GetItemProp(nItemIdx);
	if (!pItemProp)
		return;

	for (int i = 0; i < PROP_BONUSES; i++)
	{
		if (pItemProp->dwDestParam[i] != -1)
			SetDestParam(pItemProp->dwDestParam[i], pItemProp->nAdjParamVal[i], pItemProp->dwChgParamVal[i], bSend);
	}
}


void CMover::SetDestParam(int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL bSend)
{
	BOOL fSuccess = FALSE;

	if ((nDstParameter < 0) && (nDstParameter != DST_ADJDEF_RATE && nDstParameter != DST_ADJDEF))
		return;

	switch (nDstParameter)
	{
#ifdef __WORLDSERVER
	case DST_HP:
	{
		if (nAdjParameterValue == -1)
			SetPointParam(nDstParameter, GetMaxHitPoint());
		else
			SetPointParam(nDstParameter, GetHitPoint() + nAdjParameterValue);
	}
	return;
	case DST_MP:
	{
		if (nAdjParameterValue == -1)
			SetPointParam(nDstParameter, GetMaxManaPoint());
		else
			SetPointParam(nDstParameter, GetManaPoint() + nAdjParameterValue);

	}
	return;
	case DST_FP:
	{
		if (nAdjParameterValue == -1)
			SetPointParam(nDstParameter, GetMaxFatiguePoint());
		else
			SetPointParam(nDstParameter, GetFatiguePoint() + nAdjParameterValue);
	}
	return;
#else // __WORLDSERVER
	case DST_HP:	SetPointParam(nDstParameter, GetHitPoint() + nAdjParameterValue);	return;
	case DST_MP:	SetPointParam(nDstParameter, GetManaPoint() + nAdjParameterValue);	return;
	case DST_FP:	SetPointParam(nDstParameter, GetFatiguePoint() + nAdjParameterValue);	return;
#endif // __WORLDSERVER
	case DST_GOLD:
#ifdef __WORLDSERVER
		Error("SetDestParam( DST_GOLD ) - [User:%s(%07d)], [Value:%d]", static_cast<CUser*>(this)->GetName(), static_cast<CUser*>(this)->m_idPlayer, nAdjParameterValue);
		return;
#else
		if (AddGold(nAdjParameterValue) == FALSE)
			SetGold(0);

		return;
#endif

	case DST_RESIST_ALL:
		SetDestParam(DST_RESIST_ELECTRICITY, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_RESIST_FIRE, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_RESIST_WIND, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_RESIST_WATER, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_RESIST_EARTH, nAdjParameterValue, nChgParameterValue, bSend);
		return;
	case DST_STAT_ALLUP:
		SetDestParam(DST_STR, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_DEX, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_INT, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_STA, nAdjParameterValue, nChgParameterValue, bSend);
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG
		SetDestParam(DST_HP_MAX, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_CHR_DMG, nAdjParameterValue, nChgParameterValue, bSend);
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		SetDestParam(DST_ADJDEF, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_ADJ_HITRATE, nAdjParameterValue, nChgParameterValue, bSend);
		return;
	case DST_LOCOMOTION:
		SetDestParam(DST_SPEED, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_JUMPING, (nAdjParameterValue * 3), nChgParameterValue, bSend);
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		SetDestParam(DST_MASTRY_EARTH, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_MASTRY_FIRE, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_MASTRY_WATER, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_MASTRY_ELECTRICITY, nAdjParameterValue, nChgParameterValue, bSend);
		SetDestParam(DST_MASTRY_WIND, nAdjParameterValue, nChgParameterValue, bSend);
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_MP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginManaPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_FP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginFatiguePoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_ALL_RECOVERY:
	{
		// HP
		SetDestParam(DST_HP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend);
		// MP
		SetDestParam(DST_MP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend);
		// FP
		SetDestParam(DST_FP_RECOVERY, nAdjParameterValue, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_ALL_RECOVERY_RATE:
	{
		// HP
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_HP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);

		// MP
		nMax = GetMaxOriginManaPoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_MP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);

		// FP
		nMax = GetMaxOriginFatiguePoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_FP_RECOVERY, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_KILL_ALL:
	{
		// HP
		SetDestParam(DST_KILL_HP, nAdjParameterValue, NULL_CHGPARAM, bSend);
		// MP
		SetDestParam(DST_KILL_MP, nAdjParameterValue, NULL_CHGPARAM, bSend);
		// FP
		SetDestParam(DST_KILL_FP, nAdjParameterValue, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_KILL_HP_RATE:
	{
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_KILL_MP_RATE:
	{
		int nMax = GetMaxOriginManaPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_KILL_FP_RATE:
	{
		int nMax = GetMaxOriginFatiguePoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_KILL_ALL_RATE:
	{
		// HP
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_HP, nRecv, NULL_CHGPARAM, bSend);

		// MP
		nMax = GetMaxOriginManaPoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_MP, nRecv, NULL_CHGPARAM, bSend);

		// FP
		nMax = GetMaxOriginFatiguePoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		SetDestParam(DST_KILL_FP, nRecv, NULL_CHGPARAM, bSend);
	}
	return;
	case DST_ALL_DEC_RATE:
	{
		SetDestParam(DST_MP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend);
		SetDestParam(DST_FP_DEC_RATE, nAdjParameterValue, NULL_CHGPARAM, bSend);
	}
	return;
	} // switch

	if (nDstParameter >= MAX_ADJPARAMARY)
		return;

	if (nAdjParameterValue != 0)
	{
		fSuccess = TRUE;

		switch (nDstParameter)
		{
		case DST_CHRSTATE:
		case DST_IMMUNITY:
			if (nAdjParameterValue != NULL_ID)
			{
				if (nAdjParameterValue == 0xffffffff)
				{
					Error("SetDestParam : Adj == -1, %s", GetName());
					return;
				}
				m_adjParamAry[nDstParameter] |= nAdjParameterValue;
			}
			else
				fSuccess = FALSE;
			break;
		case DST_CURECHR:
			ResetDestParam(DST_CHRSTATE, nAdjParameterValue, bSend);
			return;
		case DST_REFLECT_DAMAGE:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;
			m_chgParamAry[nDstParameter] = nChgParameterValue;
			break;
		case DST_CHR_CHANCEBLEEDING:
		case DST_CHR_CHANCESTEALHP:
		case DST_CHR_CHANCEPOISON:
		case DST_CHR_CHANCEDARK:
		case DST_CHR_CHANCESTUN:
		case DST_AUTOHP:
			m_adjParamAry[nDstParameter] += nAdjParameterValue;
			m_chgParamAry[nDstParameter] = nChgParameterValue;
			break;
		case DST_HEAL:
			m_nHealCnt = (short)(PROCESS_COUNT * 2.0f);
			m_adjParamAry[nDstParameter] += nAdjParameterValue;
			break;
		default:
		{
			m_adjParamAry[nDstParameter] += nAdjParameterValue;

#if defined (__FL_RECORD_BOOK) && defined (__WORLDSERVER)
			if (IsPlayer())
			{
				switch (nDstParameter)
				{
				case DST_STR: CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::STR, GetStr(), false); break;
				case DST_STA:
				{
					CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::STA, GetSta(), false);
					CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::HP, GetMaxHitPoint(), false);
				}
				break;
				case DST_DEX: CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::DEX, GetDex(), false); break;
				case DST_INT:
				{
					CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::INTT, GetInt(), false);
				}
				break;
				case DST_CRITICAL_BONUS: CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::ADOCH, GetParam(DST_CRITICAL_BONUS, 0), false); break;
				case DST_HP_MAX:
				case DST_HP_MAX_RATE: CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::HP, GetMaxHitPoint(), false); break;
				default: break;
				}
			}
#endif
		}
		break;
		}
	}

	else if (nChgParameterValue != 0x7FFFFFFF)
	{
		fSuccess = TRUE;


		if (nDstParameter == DST_SPEED)
		{
			if (m_chgParamAry[nDstParameter] == 0x7FFFFFFF)
				m_chgParamAry[nDstParameter] = 0;
			m_chgParamAry[nDstParameter]++;
		}
		else
		{
			m_chgParamAry[nDstParameter] = nChgParameterValue;
		}
	}


#ifdef __WORLDSERVER
	if (TRUE == (fSuccess & bSend))
	{
		if (IsSMMode(SM_MAINTAIN_MP))
			SetManaPoint(GetMaxManaPoint());

		if (IsSMMode(SM_MAINTAIN_FP))
			SetFatiguePoint(GetMaxFatiguePoint());

		g_UserMng.AddSetDestParam(this, nDstParameter, nAdjParameterValue, nChgParameterValue);
	}
#endif	// __WORLDSERVER
}




void CMover::ResetDestParam(int nDstParameter, int nAdjParameterValue, BOOL fSend)
{
	switch (nDstParameter)
	{
	case DST_RESIST_ALL:
		ResetDestParam(DST_RESIST_ELECTRICITY, nAdjParameterValue, TRUE);
		ResetDestParam(DST_RESIST_FIRE, nAdjParameterValue, TRUE);
		ResetDestParam(DST_RESIST_WIND, nAdjParameterValue, TRUE);
		ResetDestParam(DST_RESIST_WATER, nAdjParameterValue, TRUE);
		ResetDestParam(DST_RESIST_EARTH, nAdjParameterValue, TRUE);
		return;
	case DST_STAT_ALLUP:
		ResetDestParam(DST_STR, nAdjParameterValue, TRUE);
		ResetDestParam(DST_DEX, nAdjParameterValue, TRUE);
		ResetDestParam(DST_INT, nAdjParameterValue, TRUE);
		ResetDestParam(DST_STA, nAdjParameterValue, TRUE);
		return;
	case DST_HPDMG_UP:	// DST_HP_MAX + DST_CHR_DMG
		ResetDestParam(DST_HP_MAX, nAdjParameterValue, TRUE);
		ResetDestParam(DST_CHR_DMG, nAdjParameterValue, TRUE);
		return;
	case DST_DEFHITRATE_DOWN: // DST_ADJDEF + DST_ADJ_HITRATE
		ResetDestParam(DST_ADJDEF, nAdjParameterValue, TRUE);
		ResetDestParam(DST_ADJ_HITRATE, nAdjParameterValue, TRUE);
		return;
	case DST_LOCOMOTION:
		ResetDestParam(DST_SPEED, nAdjParameterValue, TRUE);
		ResetDestParam(DST_JUMPING, (nAdjParameterValue * 3), TRUE);
		return;
#ifdef __NEWWPN1024
	case DST_MASTRY_ALL:
		ResetDestParam(DST_MASTRY_EARTH, nAdjParameterValue, TRUE);
		ResetDestParam(DST_MASTRY_FIRE, nAdjParameterValue, TRUE);
		ResetDestParam(DST_MASTRY_WATER, nAdjParameterValue, TRUE);
		ResetDestParam(DST_MASTRY_ELECTRICITY, nAdjParameterValue, TRUE);
		ResetDestParam(DST_MASTRY_WIND, nAdjParameterValue, TRUE);
		return;
#endif	// __NEWWPN1024

	case DST_HP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_HP_RECOVERY, nRecv, TRUE);
	}
	return;
	case DST_MP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginManaPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_MP_RECOVERY, nRecv, TRUE);
	}
	return;
	case DST_FP_RECOVERY_RATE:
	{
		int nMax = GetMaxOriginFatiguePoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_FP_RECOVERY, nRecv, TRUE);
	}
	return;
	case DST_ALL_RECOVERY:
	{
		// HP
		ResetDestParam(DST_HP_RECOVERY, nAdjParameterValue, TRUE);
		// MP
		ResetDestParam(DST_MP_RECOVERY, nAdjParameterValue, TRUE);
		// FP
		ResetDestParam(DST_FP_RECOVERY, nAdjParameterValue, TRUE);
	}
	return;
	case DST_ALL_RECOVERY_RATE:
	{
		// HP
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_HP_RECOVERY, nRecv, TRUE);

		// MP
		nMax = GetMaxOriginManaPoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_MP_RECOVERY, nRecv, TRUE);

		// FP
		nMax = GetMaxOriginFatiguePoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_FP_RECOVERY, nRecv, TRUE);
	}
	return;
	case DST_KILL_ALL:
	{
		// HP
		ResetDestParam(DST_KILL_HP, nAdjParameterValue, TRUE);
		// MP
		SetDestParam(DST_KILL_MP, nAdjParameterValue, TRUE);
		// FP
		SetDestParam(DST_KILL_FP, nAdjParameterValue, TRUE);
	}
	return;
	case DST_KILL_HP_RATE:
	{
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_HP, nRecv, TRUE);
	}
	return;
	case DST_KILL_MP_RATE:
	{
		int nMax = GetMaxOriginManaPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_MP, nRecv, TRUE);
	}
	return;
	case DST_KILL_FP_RATE:
	{
		int nMax = GetMaxOriginFatiguePoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_FP, nRecv, TRUE);
	}
	return;
	case DST_KILL_ALL_RATE:
	{
		// HP
		int nMax = GetMaxOriginHitPoint();
		int nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_HP, nRecv, TRUE);
		// MP
		nMax = GetMaxOriginManaPoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_MP, nRecv, TRUE);
		// FP
		nMax = GetMaxOriginFatiguePoint();
		nRecv = (int)((nMax * (nAdjParameterValue / 100.0f)));
		ResetDestParam(DST_KILL_FP, nRecv, TRUE);
	}
	return;
	case DST_ALL_DEC_RATE:
	{
		SetDestParam(DST_MP_DEC_RATE, nAdjParameterValue, TRUE);
		SetDestParam(DST_FP_DEC_RATE, nAdjParameterValue, TRUE);
	}
	}

	if (nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0)
		return;

	if (nAdjParameterValue != 0)
	{
		if (nDstParameter == DST_CHRSTATE)
			m_adjParamAry[nDstParameter] &= (~nAdjParameterValue);
		else
			m_adjParamAry[nDstParameter] += (-nAdjParameterValue);
	}

	if (nAdjParameterValue == 0)
	{
		if (nDstParameter == DST_SPEED && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0)
		{
			if (--m_chgParamAry[nDstParameter] == 0)
				m_chgParamAry[nDstParameter] = 0x7FFFFFFF;
		}
		else
		{
			m_chgParamAry[nDstParameter] = 0x7FFFFFFF;
		}
	}

#ifdef __WORLDSERVER
	if (fSend == TRUE)
	{
		if (GetHitPoint() > GetMaxHitPoint())
			SetHitPoint(GetMaxHitPoint());

		if (GetManaPoint() > GetMaxManaPoint())
			SetManaPoint(GetMaxManaPoint());

		if (GetFatiguePoint() > GetMaxFatiguePoint())
			SetFatiguePoint(GetMaxFatiguePoint());

#ifdef	__SPEED_SYNC_0108
		if (nDstParameter == DST_SPEED)
			g_UserMng.AddResetDestParamSync(this, nDstParameter, nAdjParameterValue, (int)m_adjParamAry[nDstParameter]);
		else
			g_UserMng.AddResetDestParam(this, nDstParameter, nAdjParameterValue);
#else
		g_UserMng.AddResetDestParam(this, nDstParameter, nAdjParameterValue);
#endif
	}
#endif	// __WORLDSERVER
}

#ifdef __SPEED_SYNC_0108
void CMover::ResetDestParamSync(int nDstParameter, int nAdjParameterValue, int nParameterValue, BOOL fSend)
{
#ifdef __CLIENT
	if (nDstParameter >= MAX_ADJPARAMARY || nDstParameter < 0)
	{

		return;
	}

	switch (nDstParameter)
	{
	case DST_SPEED:
	{
		m_adjParamAry[nDstParameter] = nParameterValue;
		if (nAdjParameterValue == 0 && m_chgParamAry[nDstParameter] != 0x7FFFFFFF && m_chgParamAry[nDstParameter] > 0)
		{
			if (--m_chgParamAry[nDstParameter] == 0)
				m_chgParamAry[nDstParameter] = 0x7FFFFFFF;
		}
	}
	break;
	}
#endif	// __CLIENT
}
#endif


int CMover::GetAdjParam(int nDestParameter)
{
	if (nDestParameter < MAX_ADJPARAMARY)
		return m_adjParamAry[nDestParameter];
	return 0;
}

int CMover::GetChgParam(int nDestParameter)
{
	if (nDestParameter < MAX_ADJPARAMARY)
		return m_chgParamAry[nDestParameter];
	return 0x7FFFFFFF;
}


int CMover::GetParam(int nDest, int nParam)
{
	int nChgParam = GetChgParam(nDest);
	if (nChgParam != 0x7FFFFFFF)
		return nChgParam;

	int nAdjParam = GetAdjParam(nDest);
	if (nAdjParam)
		return nParam + nAdjParam;
	else
		return nParam;
}


int CMover::GetHitPoint()
{
	return GetParam(DST_HP, m_nHitPoint);
}

int CMover::GetManaPoint()
{
	return GetParam(DST_MP, m_nManaPoint);
}

int CMover::GetFatiguePoint()
{
	return GetParam(DST_FP, m_nFatiguePoint);
}


int CMover::GetMaxHitPoint()
{
	if (IsInvalidObj(this))
		return 0;

	float factor = 1.0f;
	int nResult = GetParam(DST_HP_MAX, GetMaxOriginHitPoint(FALSE));
	int nFactor = GetParam(DST_HP_MAX_RATE, 0);

	factor += (float)nFactor / (float)100;
	nResult = (int)(nResult * factor);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11
	return nResult;
}


int CMover::GetMaxManaPoint()
{
	float factor = 1.0f;
	int nResult = GetParam(DST_MP_MAX, GetMaxOriginManaPoint(FALSE));
	int nFactor = GetParam(DST_MP_MAX_RATE, 0);

	factor += (float)nFactor / (float)100;
	nResult = (int)(nResult * factor);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11
	return nResult;
}


int CMover::GetMaxFatiguePoint()
{
	float factor = 1.0f;
	int nResult = GetParam(DST_FP_MAX, GetMaxOriginFatiguePoint(FALSE));
	int nFactor = GetParam(DST_FP_MAX_RATE, 0);

	factor += (float)nFactor / (float)100;
	nResult = (int)(nResult * factor);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11
	return nResult;
}

int CMover::GetMaxPoint(int nDest)
{
	switch (nDest)
	{
	case DST_HP:
		return GetMaxHitPoint();
	case DST_MP:
		return GetMaxManaPoint();
	case DST_FP:
		return GetMaxFatiguePoint();
	}

	return 0;
}

int CMover::GetMaxOriginHitPoint(BOOL bOriginal)
{
	if (IsInvalidObj(this))
		return 0;

	if (IsPlayer())
	{
		int nSta;
		if (bOriginal)
			nSta = m_nSta;
		else
			nSta = GetSta();

		JobProp* pProperty = prj.GetJobProp(GetJob());

		float a = (pProperty->fFactorMaxHP * m_nLevel) / 2.0f;
		float b = a * ((m_nLevel + 1.0f) / 4.0f) * (1.0f + nSta / 50.0f) + (nSta * 10.0f);
		float maxHP = b + 80.f;
		return (int)maxHP;
	}
	else
	{
		MoverProp* pMoverProp = GetProp();
#ifdef __S1108_BACK_END_SYSTEM
		return int(pMoverProp->dwAddHp * prj.m_fMonsterHitpointRate * pMoverProp->m_fHitPoint_Rate);
#else // __S1108_BACK_END_SYSTEM
		return pMoverProp->dwAddHp;
#endif // __S1108_BACK_END_SYSTEM
	}

	return 0;
}

int CMover::GetMaxOriginManaPoint(BOOL bOriginal)
{
	int nInt = 0;
	if (bOriginal)
		nInt = m_nInt;
	else
		nInt = GetInt();

	if (IsPlayer())
	{


		JobProp* pProperty = prj.GetJobProp(GetJob());
		float factor = pProperty->fFactorMaxMP;

		int nMaxMP = (int)(((((m_nLevel * 2.0f) + (nInt * 8.0f)) * factor) + 22.0f) + (nInt * factor));
		return nMaxMP;
	}
	return 	((m_nLevel * 2) + (nInt * 8) + 22);
}

int CMover::GetMaxOriginFatiguePoint(BOOL bOriginal)
{
	int nSta, nStr, nDex;
	if (bOriginal)
	{
		nSta = m_nSta;
		nStr = m_nStr;
		nDex = m_nDex;
	}
	else
	{
		nSta = GetSta();
		nStr = GetStr();
		nDex = GetDex();
	}

	if (IsPlayer())
	{

		JobProp* pProperty = prj.GetJobProp(GetJob());
		float factor = pProperty->fFactorMaxFP;

		int nMaxFP = (int)((((m_nLevel * 2.0f) + (nSta * 6.0f)) * factor) + (nSta * factor));
		return nMaxFP;
	}

	return ((m_nLevel * 2) + (nStr * 7) + (nSta * 2) + (nDex * 4));
}

int CMover::GetHitPointPercent(int nPercent)
{
	int nMax = GetMaxHitPoint();
	if (nMax == 0) return 0;
	//	return GetHitPoint() * nPercent / nMax;
	return MulDiv(GetHitPoint(), nPercent, nMax);
}
int CMover::GetManaPointPercent(int nPercent)
{
	int nMax = GetMaxManaPoint();
	if (nMax == 0) return 0;
	//	return GetManaPoint() * nPercent / GetMaxManaPoint();
	return MulDiv(GetManaPoint(), nPercent, nMax);
}
int CMover::GetFatiguePointPercent(int nPercent)
{
	int nMax = GetMaxFatiguePoint();
	if (nMax == 0) return 0;
	//	return GetFatiguePoint() * nPercent / GetMaxFatiguePoint();
	return MulDiv(GetFatiguePoint(), nPercent, nMax);
}



int CMover::GetHPRecovery()
{
	float fFactor = 1.0f;
	if (IsPlayer())
	{
		JobProp* pProperty = prj.GetJobProp(GetJob());
		fFactor = pProperty->fFactorHPRecovery;
	}


	int nValue = (int)((GetLevel() / 3.0f) + (GetMaxHitPoint() / (500.f * GetLevel())) + (GetSta() * fFactor));
#if __VER >= 9 // __RECOVERY10
	nValue = (int)(nValue - (nValue * 0.1f));
#endif //__RECOVERY10
	return GetParam(DST_HP_RECOVERY, nValue);
}

int CMover::GetMPRecovery()
{
	float fFactor = 1.0f;
	if (IsPlayer())
	{
		JobProp* pProperty = prj.GetJobProp(GetJob());
		fFactor = pProperty->fFactorMPRecovery;
	}

	int nValue = (int)(((GetLevel() * 1.5f) + (GetMaxManaPoint() / (500.f * GetLevel())) + (GetInt() * fFactor)) * 0.2f);
	nValue = (int)(nValue - (nValue * 0.1f));
	return GetParam(DST_MP_RECOVERY, nValue);
}

int CMover::GetFPRecovery()
{
	float fFactor = 1.0f;
	if (IsPlayer())
	{
		JobProp* pProperty = prj.GetJobProp(GetJob());
		fFactor = pProperty->fFactorFPRecovery;
	}

	int nValue = (int)(((GetLevel() * 2.0f) + (GetMaxFatiguePoint() / (500.f * GetLevel())) + (GetSta() * fFactor)) * 0.2f);
	nValue = (int)(nValue - (nValue * 0.1f));
	return GetParam(DST_FP_RECOVERY, nValue);
}

int CMover::GetResistMagic()
{
	return GetParam(DST_RESIST_MAGIC, 0);
}
int CMover::GetResistSpell(int nDestParam)
{
	MoverProp* pProp = GetProp();
	if (pProp == NULL)
		Error("CMover::GetReistSpell : %d ������Ƽ �б� ����", GetName());

	int		nResist = 0;

	switch (nDestParam)
	{
	case DST_RESIST_ELECTRICITY:	nResist = pProp->nResistElecricity;		break;
	case DST_RESIST_FIRE:			nResist = pProp->nResistFire;		break;
	case DST_RESIST_WATER:			nResist = pProp->nResistWater;		break;
	case DST_RESIST_EARTH:			nResist = pProp->nResistEarth;		break;
	case DST_RESIST_WIND:			nResist = pProp->nResistWind;		break;
	default:
		Error("CMover::GetResistSpell : %s �Ķ���� �߸��� %d", GetName(), nDestParam);
		break;
	}
	return GetParam(nDestParam, nResist);
}

int CMover::GetNaturalArmor()
{
	return GetProp()->dwNaturalArmor;
}

int CMover::GetStr()
{
	int nResult = m_nStr + GetParam(DST_STR, 0);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetDex()
{
	int nResult = m_nDex + GetParam(DST_DEX, 0);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetInt()
{
	int nResult = m_nInt + GetParam(DST_INT, 0);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11

	return nResult;
}
int CMover::GetSta()
{
	int nResult = m_nSta + GetParam(DST_STA, 0);

#ifdef __JEFF_11
	if (nResult < 1)
		nResult = 1;
#endif	// __JEFF_11

	return nResult;
}

bool	CMover::IsNoActionState() //MVRF_NOACTION
{
	if (GetAdjParam(DST_CHRSTATE) & CHS_STUN)
		return true;

	return false;
}
bool CMover::IsZeroSpeed()
{
	if (IsDie())
		return true;
	if (IsNoActionState())
		return true;
	if (GetAdjParam(DST_CHRSTATE) & CHS_LOOT)
		return true;
	if (GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return true;
	if (m_dwMode & DONMOVE_MODE)
		return true;

	return false;
}

FLOAT CMover::GetSpeed(FLOAT fSrcSpeed)
{
#ifdef __CLIENT
	if (m_dwAIInterface == AII_EGG && m_pAIInterface)
	{
		CAIEgg* pAI = (CAIEgg*)m_pAIInterface;
		CMover* pOwner = prj.GetMover(pAI->GetOwnerId());
		if (IsValidObj(pOwner))
		{
			const float fOwnerSpeed = pOwner->GetSpeed(pOwner->m_pActMover->m_fSpeed);
			if (fOwnerSpeed > 0.0f)
				return pOwner->GetSpeed(pOwner->m_pActMover->m_fSpeed);
		}
	}
#endif	// __CLIENT

	if (IsNPC())
	{
		MoverProp* pProp = GetProp();
		if (pProp && pProp->dwAI == AII_PET)
		{
			CMover* pOwner = prj.GetMover(m_idDest);
			if (IsValidObj(pOwner))
			{
				const float fOwnerSpeed = pOwner->GetSpeed(pOwner->m_pActMover->m_fSpeed);
				if (fOwnerSpeed > 0.0f)
					return pOwner->GetSpeed(pOwner->m_pActMover->m_fSpeed);
			}
		}
	}

	if (IsZeroSpeed())
		return 0.0F;

	if (IsPlayer() == FALSE)
		fSrcSpeed *= GetSpeedFactor();

	int nChgValue = GetChgParam(DST_SPEED);
	if (nChgValue != 0x7FFFFFFF)
		return 0.0F;

	int nAdjValue = GetAdjParam(DST_SPEED);
	if (nAdjValue != 0)
	{
#ifdef __MAX_SPEED
		if (nAdjValue > __MAX_SPEED)
			nAdjValue = __MAX_SPEED;
#endif

		fSrcSpeed = fSrcSpeed + (fSrcSpeed * (nAdjValue / 100.0f));
		if (fSrcSpeed < 0.0F)
			fSrcSpeed = 0.0F;

		return fSrcSpeed;
	}

	if (fSrcSpeed < 0.0F)
		fSrcSpeed = 0.0F;

	return fSrcSpeed;
}

BOOL CMover::IsCompleteQuest(int nQuestId)
{
	for (int i = 0; i < m_nCompleteQuestSize; i++)
	{
		if (m_aCompleteQuest[i] == nQuestId)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::MakeCompleteQuest(int nQuestId, LPQUEST lpQuest)
{
	for (int i = 0; i < m_nCompleteQuestSize; i++)
	{
		if (m_aCompleteQuest[i] == nQuestId)
		{
			ZeroMemory(lpQuest, sizeof(QUEST));
			lpQuest->m_wId = nQuestId;
			lpQuest->m_nState = QS_END;
			return TRUE;
		}
	}
	return FALSE;
}
void CMover::RemoveAllQuest()
{
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	m_nCheckedQuestSize = 0;
}
void CMover::RemoveCompleteQuest()
{
	m_nCompleteQuestSize = 0;
}

LPQUEST CMover::FindQuest(int nQuestId)
{
	for (int i = 0; i < m_nQuestSize; i++)
	{
		if (m_aQuest[i].m_wId == nQuestId)
			return &m_aQuest[i];
	}

	return NULL;
}
BOOL CMover::RemoveQuest(int nQuestId)
{
	QuestProp* pProp = prj.m_aPropQuest.GetAt(nQuestId);

	for (int i = 0; i < m_nQuestSize; i++)
	{
		if (m_aQuest[i].m_wId == nQuestId)
		{
			for (; i < m_nQuestSize - 1; i++)
			{
				m_aQuest[i] = m_aQuest[i + 1];
			}
			m_nQuestSize--;
			break;
		}
	}
	for (int i = 0; i < m_nCompleteQuestSize; i++)
	{
		if (m_aCompleteQuest[i] == nQuestId)
		{
			for (; i < m_nCompleteQuestSize - 1; i++)
			{
				m_aCompleteQuest[i] = m_aCompleteQuest[i + 1];
			}
			m_nCompleteQuestSize--;
			break;
		}
	}
	for (int i = 0; i < m_nCheckedQuestSize; ++i)
	{
		if (m_aCheckedQuest[i] == nQuestId)
		{
			for (; i < m_nCheckedQuestSize - 1; ++i)
				m_aCheckedQuest[i] = m_aCheckedQuest[i + 1];
			m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
			break;
		}
	}

#ifdef __CLIENT
	PlayMusic(BGM_EV_END);
#endif
	return TRUE;
}

BOOL CMover::SetQuest(int nQuestId, int nState, LPQUEST lpReturnQuest)
{
	QUEST newQuest;
	ZeroMemory(&newQuest, sizeof(QUEST));
	newQuest.m_wId = nQuestId;
	newQuest.m_nState = nState;

	LPQUEST lpQuest = FindQuest(nQuestId);

	if (lpQuest == NULL && IsCompleteQuest(newQuest.m_wId))
		return FALSE;
	if (lpReturnQuest)
		memcpy(lpReturnQuest, &newQuest, sizeof(QUEST));
	return __SetQuest(lpQuest, &newQuest);
}
BOOL CMover::SetQuest(LPQUEST lpNewQuest)
{
	LPQUEST lpQuest = FindQuest(lpNewQuest->m_wId);

	if (lpQuest == NULL && IsCompleteQuest(lpNewQuest->m_wId))
		return FALSE;
	return __SetQuest(lpQuest, lpNewQuest);
}
BOOL CMover::__SetQuest(LPQUEST lpQuest, LPQUEST lpNewQuest)
{

	if (lpQuest == NULL)
	{
		if (m_nQuestSize >= MAX_QUEST)
		{
			Error("SetQuestCnt : ����Ʈ �ʰ�");
			return FALSE;
		}
#ifdef __CLIENT
		PlayMusic(BGM_EV_START);
#endif
		if (m_nCompleteQuestSize < MAX_COMPLETE_QUEST)
			lpQuest = &m_aQuest[m_nQuestSize++];
		else
			return FALSE;

		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(lpNewQuest->m_wId);
		if (pQuestProp && pQuestProp->m_nEndCondLimitTime)
			lpNewQuest->m_wTime = pQuestProp->m_nEndCondLimitTime;
#ifdef __WORLDSERVER

		m_timerQuestLimitTime.Reset();
#endif
	}
	else

		if (lpNewQuest->m_nState == QS_END)
		{

			if (m_nCompleteQuestSize < MAX_COMPLETE_QUEST)
			{
				for (int i = 0; i < m_nQuestSize; i++)
				{
					if (m_aQuest[i].m_wId == lpNewQuest->m_wId)
					{
						for (; i < m_nQuestSize - 1; i++)
						{
							m_aQuest[i] = m_aQuest[i + 1];
						}
						m_nQuestSize--;
						break;
					}
				}
				for (int i = 0; i < m_nCheckedQuestSize; ++i)
				{
					if (m_aCheckedQuest[i] == lpNewQuest->m_wId)
					{
						for (; i < m_nCheckedQuestSize - 1; ++i)
							m_aCheckedQuest[i] = m_aCheckedQuest[i + 1];
						m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
						break;
					}
				}
				QuestProp* pProp = prj.m_aPropQuest.GetAt(lpNewQuest->m_wId);

				if (pProp && pProp->m_bRepeat == FALSE)
					m_aCompleteQuest[m_nCompleteQuestSize++] = lpNewQuest->m_wId;
				lpQuest = NULL;
#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */  && defined( __CLIENT )
				g_QuestTreeInfoManager.DeleteTreeInformation(lpNewQuest->m_wId);
				D3DXVECTOR3& rDestinationArrow = g_WndMng.m_pWndWorld->m_vDestinationArrow;
				rDestinationArrow = D3DXVECTOR3(-1.0F, 0.0F, -1.0F);
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )
			}
			else
				return FALSE;
		}
	if (lpQuest)
		memcpy(lpQuest, lpNewQuest, sizeof(QUEST));

#ifdef __CLIENT
	if (lpNewQuest->m_nState == QS_END)
		PlayMusic(BGM_EV_END);
#endif
	return TRUE;
}

int CMover::GetItemNum(DWORD dwItemId)
{
	int nNum = 0;
	int nSize = m_Inventory.GetMax();
	CItemElem* pItemElem;
	for (int i = 0; i < nSize; i++)
	{
		pItemElem = m_Inventory.GetAtId(i);
		if (pItemElem)
		{
			//raiders.06.05.02 (prevent abuse quest ending condition : trade & quest )
			if (IsUsableItem(pItemElem) == FALSE)
				return 0;

			if (pItemElem->m_dwItemId == dwItemId)
				nNum += pItemElem->m_nItemNum;
		}
	}
	return nNum;
}

//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////

#ifdef __WORLDSERVER
int CMover::GetAvailableItemNum(DWORD dwItemId)
{
	int nNum = 0;

	CItemElem* pItemElem;
	for (int i = 0; i < m_Inventory.GetMax(); i++)
	{
		pItemElem = m_Inventory.GetAtId(i);

		if (!IsUsableItem(pItemElem))
			continue;

		if (pItemElem->m_dwItemId != dwItemId)
			continue;

		if (IsUsing(pItemElem))
			continue;

		if (m_Inventory.IsEquip(i))
			continue;

		nNum += pItemElem->m_nItemNum;
	}
	return nNum;
}
void CMover::RemoveAvailableItem(DWORD dwItemId, int nNum)
{
	if (nNum == -1) // Quest things
	{
		RemoveAllItem(dwItemId);
		return;
	}

	int nRemnant = nNum;

	CItemElem* pItemElem;
	for (int i = 0; i < m_Inventory.GetMax() && nRemnant > 0; i++)
	{
		pItemElem = m_Inventory.GetAtId(i);

		if (!IsUsableItem(pItemElem))
			continue;

		if (pItemElem->m_dwItemId != dwItemId)
			continue;

		if (IsUsing(pItemElem))
			continue;

		if (m_Inventory.IsEquip(i))
			continue;

		if (nRemnant > pItemElem->m_nItemNum)
		{
			nRemnant -= pItemElem->m_nItemNum;
			RemoveItem(i, pItemElem->m_nItemNum);
		}
		else
		{
			RemoveItem(i, nRemnant);
			nRemnant = 0;
		}
	}
}
#endif

//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////

#ifdef __WORLDSERVER
int CMover::RemoveAllItem(DWORD dwItemId)
{
	int nNum = 0;
	for (int i = 0; i < m_Inventory.GetMax(); ++i)
	{
		CItemElem* pItemElem = m_Inventory.GetAtId(i);
		if (IsUsableItem(pItemElem) && pItemElem->m_dwItemId == dwItemId)
		{
			nNum += pItemElem->m_nItemNum;
			RemoveItem((BYTE)(i), pItemElem->m_nItemNum);
		}
	}
	return nNum;
}
#endif

void CMover::OnTradeRemoveUser()
{
	if (IsPlayer())
	{
		CMover* pOther = m_vtInfo.GetOther();
		if (IsValidObj(pOther) && pOther->IsPlayer())
		{
			if (pOther->m_vtInfo.GetOther() == this)
				pOther->m_vtInfo.TradeClear();
		}
		m_vtInfo.TradeClear();
	}
}

void CMover::UpdateItemEx(unsigned char id, char cParam, __int64 iValue)
{
	CItemElem* pItemElem = m_Inventory.GetAtId(id);
	if (pItemElem)
	{
		switch (cParam)
		{
		case UI_RANDOMOPTITEMID:
		{
			pItemElem->SetRandomOptItemId(iValue);
			break;
		}
		default:
			break;
		}
#ifdef __CLIENT
		CWndInventory* pWnd = (CWndInventory*)g_WndMng.GetWndBase(APP_INVENTORY);
		if (pWnd)
			pWnd->UpdateTooltip();
#endif	// __CLIENT
	}
#ifdef __WORLDSERVER
	if (IsPlayer())
		((CUser*)this)->AddUpdateItemEx(id, cParam, iValue);
#endif
}

void CMover::UpdateItem(BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime, int nType)
{
	CItemContainer<CItemElem>* pContainer = GetContainerByType(nType);
	if (!pContainer)
	{
		Error("CMover::UpdateItem no container by type %d m_idPlayer [%d]", nType, m_idPlayer);
		return;
	}

	CItemElem* pItemBase = pContainer->GetAtId(nId);
	if (pItemBase)
	{
		switch (cParam)
		{
		case UI_COOLTIME:
		case UI_NUM:
		{
			ItemProp* pItemProp = pItemBase->GetProp();
			if ((int)dwValue <= 0 || (pItemProp && dwValue > pItemProp->dwPackMax))
				pContainer->RemoveAtId(nId);
			else
			{
				pItemBase->m_nItemNum = (short)(dwValue);

				if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
					pItemBase->SetExtra(pItemBase->m_nItemNum);
			}

			break;
		}
		case UI_HP:
		{
			pItemBase->m_nHitPoint = dwValue;
			UpdateParam();
			break;
		}
		case UI_AO:
		{
			DWORD dwMax = 20;
			if (pItemBase->IsAccessory())
				dwMax = 20;
			if (pItemBase->IsCollector(TRUE))
				dwMax = 5;
			if (pItemBase->GetAbilityOption() > (int)(dwMax))
				return;
			if (dwValue > dwMax)
				dwValue = dwMax;

			pItemBase->SetAbilityOption(dwValue);
			UpdateParam();
		}
		break;
		case UI_RAO:
		{
			if (pItemBase->m_nResistAbilityOption > 20)
				return;
			if (dwValue > 20)
				dwValue = 20;
			pItemBase->m_nResistAbilityOption = dwValue;
			UpdateParam();
		}
		break;
		case UI_IR:
		{
			pItemBase->m_bItemResist = (BYTE)(dwValue);
			UpdateParam();
		}
		break;

		case UI_PIERCING_SIZE:
			pItemBase->SetPiercingSize(dwValue);
			break;
		case UI_PIERCING:
		{
			WORD wIndex = LOWORD(dwValue), wItemId = HIWORD(dwValue);
			pItemBase->SetPiercingItem(wIndex, wItemId);
		}
		break;

		case UI_ULTIMATE_PIERCING_SIZE:
			pItemBase->SetUltimatePiercingSize(dwValue);
			break;
		case UI_ULTIMATE_PIERCING:
		{
			WORD wIndex = LOWORD(dwValue), wItemId = HIWORD(dwValue);
			pItemBase->SetUltimatePiercingItem(wIndex, wItemId);
		}
		break;

		case UI_PETVIS_SIZE:
		{
			pItemBase->SetPiercingSize(dwValue);
			pItemBase->SetVisKeepTimeSize(dwValue);

#ifdef __CLIENT
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
		}
		break;
		case UI_PETVIS_ITEM:
		{
			WORD wIndex = LOWORD(dwValue), wItemId = HIWORD(dwValue);
			pItemBase->SetPiercingItem(wIndex, wItemId);
			pItemBase->SetVisKeepTime(wIndex, (dwTime * 60) + time_null());

#ifdef __CLIENT
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
		}
		break;

		case UI_STACKBEAD:
		{
			WORD wIndex = LOWORD(dwValue), wItemId = HIWORD(dwValue);
			pItemBase->SetVisKeepTime(wIndex, dwTime);

#ifdef __CLIENT
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
		}
		break;

		case UI_PETVIS_ITEMSWAP:
		{
			WORD wPos1 = LOWORD(dwValue), wPos2 = HIWORD(dwValue);
			pItemBase->SetSwapVisItem(wPos1, wPos2);

#ifdef __CLIENT
			PLAYSND(SND_INF_EQUIPACCESSORY, &g_pPlayer->GetPos());
#endif // __CLIENT
		}
		break;

		case UI_TRANSFORM_VISPET:
		{
			pItemBase->m_bTranformVisPet = static_cast<BOOL>(dwValue);
		}
		break;

		case UI_RANDOMOPTITEMID:
		{
			pItemBase->SetRandomOpt(dwValue);
			break;
		}
		case UI_KEEPTIME:	// offset
		{
			CTime tm = CTime::GetCurrentTime() + CTimeSpan(0, 0, dwValue, 0);
			pItemBase->m_dwKeepTime = (DWORD)(tm.GetTime());
			break;
		}

		case UI_FLAG:
		{
#ifdef __CLIENT
			DWORD dwObjIndex = (DWORD)(short)LOWORD(dwValue);
			BYTE byFlag = (BYTE)HIWORD(dwValue);
			int nParts = dwObjIndex - m_Inventory.GetSize();
			if (nParts >= 0 && nParts < MAX_HUMAN_PARTS)
				m_aEquipInfo[nParts].byFlag = byFlag;
			pItemBase->m_byFlag = byFlag;
			UpdateParts(FALSE);
#endif	// __CLIENT
			UpdateParam();
			break;
		}
		#ifdef __WEAPON_RARITY
		case UI_WEAPONRARITY:
		{
			static_cast<CItemElem*>(pItemBase)->m_nWeaponRarity = static_cast<short>(dwValue);
		}
		break;
		#endif // __WEAPON_RARITY

		default:
			break;
		}
#ifdef __CLIENT
		CWndInventory* pWnd = (CWndInventory*)g_WndMng.GetWndBase(APP_INVENTORY);
		if (pWnd)
			pWnd->UpdateTooltip();
#endif	// __CLIENT
	}
	else
	{
		switch (cParam)
		{
		case UI_FLAG:
		{
#ifdef __CLIENT
			DWORD dwObjIndex = (DWORD)(short)LOWORD(dwValue);
			BYTE byFlag = (BYTE)HIWORD(dwValue);
			int nParts = dwObjIndex - m_Inventory.GetSize();
			if (nParts >= 0 && nParts < MAX_HUMAN_PARTS)
			{
				m_aEquipInfo[nParts].byFlag = byFlag;
				UpdateParts(TRUE);
			}
#endif	// __CLIENT
			break;
		}
		default:
			break;
		}
	}
#ifdef __WORLDSERVER
	if (IsPlayer())
	{
		static_cast<CUser*>(this)->AddUpdateItem(0, nId, cParam, dwValue, dwTime, nType);

		if (pItemBase && cParam == UI_TRANSFORM_VISPET && HasActivatedEatPet())
		{
			InactivateEatPet();
			ActivateEatPet(pItemBase);
		}
	}
#endif // __WORLDSERVER
}

float CMover::GetItemDropRateFactor(CMover* pAttacker)
{
	float fFactor = 1.0F;
#ifdef __WORLDSERVER
	fFactor *= prj.m_fItemDropRate;
	fFactor *= GetProp()->m_fItemDrop_Rate;
	fFactor *= prj.m_EventLua.GetItemDropRate();
#endif	// __WORLDSERVER
	return fFactor;
}

float CMover::GetPieceItemDropRateFactor(CMover* pAttacker)
{
	float fFactor = 1.0F;
#ifdef __WORLDSERVER
	fFactor *= prj.m_EventLua.GetPieceItemDropRate();
	ILordEvent* pEvent = CSLord::Instance()->GetEvent();
	fFactor *= pEvent->GetIFactor();
	if (pAttacker->HasBuff(BUFF_ITEM, II_SYS_SYS_LS_CHEERING))
		fFactor *= 1.1F;
	if (pAttacker->HasBuff(BUFF_ITEM, II_SYS_SYS_VOTE_THANKS))
		fFactor *= 1.05F;
	if (pAttacker->HasBuff(BUFF_ITEM, II_COUPLE_BUFF_MIRACLE_01))
		fFactor *= 1.05F;
#endif // __WORLDSERVER
	return fFactor;
}

float CMover::GetExpFactor(void)
{
	float fFactor = 1.0f;

	int nExtraEXP = 0;
	for (MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i)
	{
		ItemProp* pItemProp = prj.GetItemProp(i->second->GetId());

		if (pItemProp)
		{
			if (pItemProp->dwItemKind3 == IK3_EXP_RATE)
				nExtraEXP += 50;
		}
	}
	fFactor *= 1.0f + ((float)nExtraEXP / 100);

	if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_GLVCHEER))
		fFactor *= 1.10f;
	else if (HasBuff(BUFF_ITEM, II_CHEERUP))
	{
#ifdef __EVENTLUA_CHEEREXP
		if (prj.m_EventLua.GetCheerExpFactor() > 1.05f)
			fFactor *= prj.m_EventLua.GetCheerExpFactor();
		else
#endif
			fFactor *= 1.05f;
	}

#ifdef __WORLDSERVER
	fFactor *= prj.m_EventLua.GetExpFactor();
	fFactor *= prj.m_fMonsterExpRate;

	int nDstExp = GetParam(DST_EXPERIENCE, 0);
	if (0 < nDstExp)
		fFactor *= (1.0f + (nDstExp / 100.0f));

	ILordEvent* pEvent = CSLord::Instance()->GetEvent();
	fFactor *= pEvent->GetEFactor();
	if (HasBuff(BUFF_ITEM, II_SYS_SYS_LS_CHEERING))
		fFactor *= 1.1F;
	if (HasBuff(BUFF_ITEM, II_SYS_SYS_VOTE_THANKS))
		fFactor *= 1.05F;


	if (CEnvironment::GetInstance()->GetEnvironmentEffect() == TRUE)
		fFactor *= prj.m_EventLua.GetWeatherEventExpFactor();
#endif	// __WORLDSERVER


	return fFactor;
}
