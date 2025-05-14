#include "stdafx.h"
#include "DefineObj.h"
#include "defineSkill.h"
#include "ActionMover.h"
#include "User.h"
#include "defineitem.h"
#include "AttackArbiter.h"
#include "guild.h"
extern	CGuildMng			g_GuildMng;

extern	CUserMng			g_UserMng;
#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "dpdatabaseclient.h"
extern	CDPDatabaseClient	g_dpDBClient;

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#ifdef __FL_FFA
#include "FFAManager.h"
#endif

#include "CreateMonster.h"

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

////////////////////////////////////////////////////////////////
// ATTACK_INFO
////////////////////////////////////////////////////////////////

ATK_TYPE ATTACK_INFO::GetAtkType() const
{
	ATK_TYPE type = ATK_GENERIC;

	if (dwAtkFlags & AF_MELEESKILL)
	{
		type = ATK_MELEESKILL;
	}
	else if (dwAtkFlags & AF_MAGICSKILL)
	{
		type = ATK_MAGICSKILL;
	}
	else if (dwAtkFlags & AF_MAGIC)
	{
		type = ATK_MAGIC;
	}
	else if (dwAtkFlags & AF_FORCE)
	{
		type = ATK_FORCE;
	}

	return type;
}


BOOL ATTACK_INFO::CanIgnoreDEF() const
{
	int nSkill = GetSkill();
	switch (nSkill)
	{
	case SI_BIL_PST_ASALRAALAIKUM:
		return TRUE;
	case SI_JST_YOYO_HITOFPENYA:
		return TRUE;
	}

	if (dwAtkFlags & AF_FORCE)
		return TRUE;
	return FALSE;
}

int ATTACK_INFO::GetChargeLevel() const
{
	if (IsSkillAttack(dwAtkFlags))
		return 0;

	return (nParam & 0xFF);
}

int ATTACK_INFO::GetSkillLevel() const
{
	ASSERT(IsSkillAttack(dwAtkFlags));
	return (nParam & 0xFF);
}

int ATTACK_INFO::GetAttackCount() const
{
	return (nParam >> 8) & 0xFF;
}

int ATTACK_INFO::GetSkill() const
{
	if (IsSkillAttack(dwAtkFlags))
		return (nParam >> 16) & 0xFFFF;
	else
		return 0;
}

BOOL ATTACK_INFO::IsRangeAttack() const
{
	return (dwAtkFlags & AF_RANGE);
}

////////////////////////////////////////////////////////////////
// CAttackArbiter
////////////////////////////////////////////////////////////////

CAttackArbiter::CAttackArbiter(DWORD dwMsg, CMover* pAttacker, CMover* pDefender, DWORD dwAtkFlags, int nParam, BOOL bTarget, int nReflect)
	: m_dwMsg(dwMsg),
	m_pAttacker(pAttacker),
	m_pDefender(pDefender),
	m_dwAtkFlags(dwAtkFlags),
	m_nParam(nParam),
	m_bTarget(bTarget),
	m_nReflect(nReflect)
{
	m_nReflectDmg = 0;
}

CAttackArbiter::~CAttackArbiter()
{
}

int CAttackArbiter::OnDamageMsgW()
{
	if (!CheckValidDamageMsg())
		return 0;

	ATTACK_INFO info;
	info.dwAtkFlags = m_dwAtkFlags;
	info.pAttacker = m_pAttacker;
	info.pDefender = m_pDefender;
	info.nParam = m_nParam;
	info.nParts = PARTS_RWEAPON;

	int nDamage = 0;
	BYTE cbHandFlag = GetHandFlag();
	for (BYTE cbFlag = 0x01; cbFlag <= 0x02; ++cbFlag)
	{
		if (cbHandFlag & cbFlag)
		{
			info.nParts = (cbFlag & 0x01) ? PARTS_RWEAPON : PARTS_LWEAPON;
#ifdef __EVENTLUA_SPAWN
			if (m_pDefender->IsNPC() && prj.m_EventLua.IsEventSpawnMonster(m_pDefender->GetIndex()) && !m_pAttacker->IsMode(ONEKILL_MODE))
			{
				nDamage = 1;
				continue;
			}
#endif // 
			int n = CalcDamage(&info);
			if (n > 0)
			{
				ProcessAbnormal(n, &info);
				StealHP(n, info.GetAtkType());
				nDamage += n;
			}
		}
	}

	if (m_pAttacker->IsPlayer() && m_pDefender->IsPlayer())
		nDamage -= (nDamage * m_pDefender->GetParam(DST_PVP_DMG_RATE, 0) / 100);

	nDamage = max(nDamage, 1);
	int nHP = MinusHP(&nDamage);

	if (CMonsterSkill::GetInstance()->MonsterTransform(m_pDefender, nHP))
		return 0;

#ifndef __AIO_POSI_RELATED
	m_pDefender->m_pActMover->SendActMsg(OBJMSG_STAND);
#endif

	m_pDefender->OnAttacked(m_pAttacker, nDamage, m_bTarget, m_nReflect);

#ifdef __FL_RECORD_BOOK
	bool bAttackerPlayer = m_pAttacker->IsPlayer();
	bool bDefenderPlayer = m_pDefender->IsPlayer();

	int nSpecialRecordType = -1;
	if (info.GetSkill() == SI_BIL_PST_ASALRAALAIKUM)
		nSpecialRecordType = bDefenderPlayer ? RecordType::PVP_ASAL : RecordType::PVE_ASAL;
	else if (info.GetSkill() == SI_JST_YOYO_HITOFPENYA)
		nSpecialRecordType = bDefenderPlayer ? RecordType::PVP_HOP : RecordType::PVE_HOP;
	else if (info.IsRangeAttack() && info.GetChargeLevel() == 4)
		nSpecialRecordType = bDefenderPlayer ? RecordType::PVP_CHARGEBOW : RecordType::PVE_CHARGEBOW;

	if (bAttackerPlayer)
	{
		CRecordBook::GetInstance()->AddPlayerRecord(m_pAttacker, bDefenderPlayer ? RecordType::PVP_DMG : RecordType::PVE_DMG, nDamage, false);
		if (nSpecialRecordType != -1)
			CRecordBook::GetInstance()->AddPlayerRecord(m_pAttacker, nSpecialRecordType, nDamage, false);
	}
#endif

	if (nHP > 0)
	{
		m_pDefender->m_nAtkCnt = 1;

		if ((m_dwAtkFlags & AF_FLYING))
			m_pDefender->FlyByAttack();

		g_UserMng.AddDamage(m_pDefender, GETID(m_pAttacker), nDamage, m_dwAtkFlags);

#ifdef __FL_UPDATE_REAGGRO
		if (m_pAttacker->IsPlayer() && m_pDefender->IsNPC())
			m_pDefender->PostAIMsg(AIMSG_DAMAGE, m_pAttacker->GetId(), 0);
#endif

		int nActionHPRate = m_pDefender->GetAdjParam(DST_AUTOHP);
		if (nActionHPRate > 0)
		{
			float fHPPercent = (float)nHP / m_pDefender->GetMaxHitPoint();
			if (fHPPercent <= (nActionHPRate / 100.0f))
			{
				float fAddHP = m_pDefender->GetChgParam(DST_AUTOHP) / 100.0f;
				int nAddHP = (int)(m_pDefender->GetMaxHitPoint() * fAddHP);
				nHP += nAddHP;
				m_pDefender->SetPointParam(DST_HP, nHP, TRUE);
				g_UserMng.AddCreateSfxObj(m_pDefender, XI_GEN_CURE01);
				m_pDefender->RemoveBuff(BUFF_SKILL, SI_ASS_HEAL_PREVENTION);
			}
		}

		if (m_nReflectDmg)
		{
			m_pAttacker->m_pActMover->SendDamage(AF_FORCE, m_pDefender->GetId(), m_nReflectDmg, TRUE, 2);
			m_nReflectDmg = 0;
		}

	}
	else
		OnDied();

	m_pDefender->SetPointParam(DST_HP, nHP);

	return nDamage;
}

BOOL CAttackArbiter::CheckValidDamageMsg()
{
#ifdef __EVENT_MONSTER
	if (m_pAttacker->IsPlayer() && !m_pDefender->IsPlayer())
	{
		MoverProp* lpMoverProp = m_pDefender->GetProp();
		if (lpMoverProp && CEventMonster::GetInstance()->SetEventMonster(lpMoverProp->dwID))
		{
			if (m_pAttacker->GetLevel() - m_pDefender->GetLevel() >= CEventMonster::GetInstance()->GetLevelGap())
				m_dwAtkFlags = AF_MISS;
		}
	}
#endif // __EVENT_MONSTER

	if (m_pAttacker->IsPlayer() && !CCreateMonster::GetInstance()->IsAttackAble(static_cast<CUser*>(m_pAttacker), m_pDefender, TRUE))
	{
		m_dwAtkFlags = AF_MISS;
		m_pDefender->m_idAttacker = NULL_ID;
		m_pDefender->m_idTargeter = NULL_ID;
	}

	if (m_dwAtkFlags & AF_MISS)
	{
		m_pDefender->PostAIMsg(AIMSG_DAMAGE, m_pAttacker->GetId(), 0);
		g_UserMng.AddDamage(m_pDefender, GETID(m_pAttacker), 0, m_dwAtkFlags);
		return FALSE;
	}

	if (!g_eLocal.GetState(EVE_STEAL))
	{
		if (m_pAttacker->IsSteal(m_pDefender))
			return FALSE;
	}

	return TRUE;
}

int CAttackArbiter::CalcATK(ATTACK_INFO* pInfo)
{
	int nATK = 0;
	int nCount = pInfo->GetAttackCount();

	switch (pInfo->GetAtkType())
	{
	case ATK_FORCE:
		nATK = m_nParam;
		nCount = 0;
		break;
	case ATK_MELEESKILL:
		nATK = m_pAttacker->GetMeleeSkillPower(pInfo);
		break;
	case ATK_MAGICSKILL:
		nATK = m_pAttacker->GetMagicSkillPower(pInfo);
		break;
	case ATK_MAGIC:
		nATK = m_pAttacker->GetMagicHitPower(pInfo->GetChargeLevel());
		break;
	case ATK_GENERIC:
		nATK = m_pAttacker->GetHitPower(pInfo);
		break;
	}

#ifndef __FL_DST_ATKPOWER_RATE_MAGIC
	nATK = (int)(nATK * m_pAttacker->GetATKMultiplier(m_pDefender, pInfo->dwAtkFlags));
#else
	nATK = (int)(nATK * m_pAttacker->GetATKMultiplier(m_pDefender, pInfo));
#endif
	if (nCount > 0)
		nATK = (int)(nATK * 0.1f);

	nATK += m_pAttacker->GetParam(DST_ATKPOWER, 0);
	if (m_pAttacker->IsPlayer())
		nATK += prj.m_EventLua.GetAttackPower();

#ifdef __JEFF_11
	if (nATK < 0)
		nATK = 0;
#endif	// __JEFF_11

	return nATK;
}



int CAttackArbiter::CalcDamage(ATTACK_INFO* pInfo)
{
	if (m_pAttacker->m_dwMode & ONEKILL_MODE && m_pAttacker->IsAuthHigher(AUTH_ADMINISTRATOR))
		return m_pDefender->GetHitPoint();

	int nDamage = PostCalcDamage(CalcATK(pInfo), pInfo);
	m_dwAtkFlags = pInfo->dwAtkFlags;

	return nDamage;
}

int CAttackArbiter::OnAfterDamage(ATTACK_INFO* pInfo, int nDamage)
{
	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;

	if (pAttacker->IsPlayer() && nDamage > 0)
	{
		if (pDefender->GetAdjParam(DST_CHRSTATE) & CHS_DOUBLE)
			pDefender->RemoveChrStateBuffs(CHS_DOUBLE);
	}

	if (pDefender->GetAdjParam(DST_CHRSTATE) & CHS_ATK_COUNTERATTACK)
	{
		if (pDefender->IsRangeObj(pAttacker->GetPos(), 3.0f))
		{
			pDefender->RemoveChrStateBuffs(CHS_ATK_COUNTERATTACK);

			int nLevel = 1;
			if (pDefender->IsPlayer())
			{
				LPSKILL pSkill = pDefender->GetSkill(SI_ACR_YOYO_COUNTER);
				if (pSkill)
				{
					nLevel = pSkill->dwLevel;
				}
			}
			pDefender->DoUseSkill(SI_GEN_ATK_COUNTER, nLevel, pAttacker->GetId(), SUT_NORMAL, FALSE, 0);

			nDamage = 0;
			pInfo->dwAtkFlags = AF_MISS;
		}
	}
	else
		if (pDefender->GetAdjParam(DST_CHRSTATE) & CHS_DMG_COUNTERATTACK)
		{
			if (pDefender->IsRangeObj(pAttacker->GetPos(), 3.0f))
			{
				pDefender->RemoveChrStateBuffs(CHS_DMG_COUNTERATTACK);
				pDefender->DoUseSkill(SI_GEN_ATK_COUNTER, pInfo->GetSkillLevel(), pAttacker->GetId(), SUT_NORMAL, FALSE, 0);
			}
		}

	if (pInfo->GetSkill() == SI_JST_YOYO_HITOFPENYA && nDamage > 0)
	{
#ifndef __FL_EQUALIZED_MODE
		nDamage = min(pAttacker->GetGold(), nDamage);
		pAttacker->AddGold(-nDamage);
#else
		if (pAttacker->IsPlayer() && ((CUser*)pAttacker)->bTmpMode)
		{
		}
		else
		{
			nDamage = min(pAttacker->GetGold(), nDamage);
			pAttacker->AddGold(-nDamage);
		}
#endif
	}

	if (pDefender->IsReturnToBegin())
	{
		nDamage = 0;
		pInfo->dwAtkFlags = AF_MISS;
	}

	return nDamage;
}

enum POSTCALC_TYPE
{
	POSTCALC_DPC,
	POSTCALC_MAGICSKILL,
	POSTCALC_GENERIC,
};


POSTCALC_TYPE GetPostCalcType(DWORD dwAtkFlags)
{
	if (dwAtkFlags & AF_MAGICSKILL)
		return POSTCALC_MAGICSKILL;

	if (dwAtkFlags & AF_GENERIC)
		return POSTCALC_GENERIC;

	return POSTCALC_DPC;
}

int CAttackArbiter::PostCalcDamage(int nATK, ATTACK_INFO* pInfo)
{
	if (pInfo->dwAtkFlags & AF_FORCE)
		return nATK;

	CMover* pAttacker = pInfo->pAttacker;
	CMover* pDefender = pInfo->pDefender;


	if (((pInfo->dwAtkFlags & AF_MAGICSKILL) == 0) && pAttacker->IsNPC() && pDefender->IsPlayer())
	{
		int nDelta = pAttacker->GetLevel() - pDefender->GetLevel();
		if (nDelta > 0)
		{
			float fpower = 1.0f + (0.05f * nDelta);
			nATK = (int)(nATK * fpower);
		}
	}

	int nDamage = nATK;
	switch (GetPostCalcType(pInfo->dwAtkFlags))
	{
	case POSTCALC_DPC:
		nDamage = pDefender->ApplyDPC(nATK, pInfo);
		break;
	case POSTCALC_MAGICSKILL:
		nDamage = pAttacker->PostCalcMagicSkill(nATK, pInfo);
		break;
	case POSTCALC_GENERIC:
		nDamage = pAttacker->PostCalcGeneric(nATK, pInfo);
		break;
	}

	if (nDamage <= 0)
		return 0;

	nDamage += pAttacker->CalcLinkAttackDamage(nDamage);
	if (pInfo->GetSkill() == SI_BIL_PST_ASALRAALAIKUM)
		nDamage += PostAsalraalaikum();

	nDamage = (int)(nDamage * pAttacker->GetDamageMultiplier(pInfo));

	if (pDefender->IsPlayer() && pAttacker->IsPlayer())
	{
		int nImmunity = pDefender->GetParam(DST_IGNORE_DMG_PVP, 0);
		if (nImmunity > 0)
		{
			if ((nImmunity - nDamage) <= 0)
			{
				nDamage -= nImmunity;
				pDefender->RemoveBuff(BUFF_SKILL, SI_FLO_SUP_ABSOLUTE);
			}
			else
			{
				pDefender->ResetDestParam(DST_IGNORE_DMG_PVP, nDamage, TRUE);
				nDamage = 0;
			}
		}
	}

	nDamage = OnAfterDamage(pInfo, nDamage);
	return nDamage;
}

int CAttackArbiter::PostAsalraalaikum()
{
	int nAddDmg = 0;

	LPSKILL pSkill = m_pAttacker->GetSkill(SI_BIL_PST_ASALRAALAIKUM);
	DWORD dwSkillLevel = pSkill ? pSkill->dwLevel : 0;

	if (m_pAttacker->IsNPC())
		dwSkillLevel = CMonsterSkill::GetInstance()->GetMonsterSkillLevel(m_pAttacker, SI_BIL_PST_ASALRAALAIKUM);

	switch (dwSkillLevel)
	{
	case 1:		nAddDmg = 20;	break;
	case 2:		nAddDmg = 30;	break;
	case 3:		nAddDmg = 40;	break;
	case 4:		nAddDmg = 50;	break;
	case 5:		nAddDmg = 60;	break;
	case 6:		nAddDmg = 70;	break;
	case 7:		nAddDmg = 80;	break;
	case 8:		nAddDmg = 90;	break;
	case 9:		nAddDmg = 100;	break;
	case 10:	nAddDmg = 150;	break;
	}

	int nMP = m_pAttacker->GetManaPoint();
	if (m_pAttacker->IsPlayer() && m_pAttacker->IsSMMode(SM_MAINTAIN_MP) == FALSE)
		m_pAttacker->SetPointParam(DST_MP, 0);
	return (((m_pAttacker->GetStr() / 10) * dwSkillLevel) * (5 + nMP / 10) + nAddDmg);
}



void CAttackArbiter::ChanceSkill(int nDestParam, CCtrl* pAttackCtrl, CCtrl* pDefendCtrl)
{
	if (m_pAttacker->GetType() != OT_MOVER)	return;
	if (m_pDefender->GetType() != OT_MOVER)	return;

	CMover* pAttacker = (CMover*)pAttackCtrl;
	CMover* pDefender = (CMover*)pDefendCtrl;

	int nRate = pAttacker->GetAdjParam(nDestParam);
	if (nRate > 0)
	{
		if ((int)(xRandom(100)) <= nRate)
		{
			DWORD dwAddSkill = pAttacker->GetChgParam(nDestParam);

			AddSkillProp* pAddSkillProp;
			pAddSkillProp = prj.GetAddSkillProp(dwAddSkill);
			if (pAddSkillProp)
				pAttacker->DoActiveSkill(pAddSkillProp->dwName, pAddSkillProp->dwSkillLvl, pDefender);
		}

	}
}



void CAttackArbiter::ProcessAbnormal(int nDamage, ATTACK_INFO* pInfo)
{
	int	nSkill = pInfo->GetSkill();


	int nReflectDmgRate = m_pDefender->GetAdjParam(DST_REFLECT_DAMAGE);
	if (nReflectDmgRate > 0)
	{
		int nReflectRate = m_pDefender->GetChgParam(DST_REFLECT_DAMAGE);

		BOOL bAble = TRUE;
		if (nReflectRate > 0)
			if ((int)(xRandom(100)) > nReflectRate)
				bAble = FALSE;

		ATK_TYPE atkType = pInfo->GetAtkType();
		switch (atkType)
		{
		case ATK_MELEESKILL:
		case ATK_MAGICSKILL:
			if (m_pDefender->HasBuff(BUFF_SKILL, SI_PSY_NLG_CRUCIOSPELL))
				bAble = FALSE;
			break;
		case ATK_FORCE:
			bAble = FALSE;
			break;
		}

		if (bAble)
		{
			int nDmg = (int)(nDamage * (float)(nReflectDmgRate / 100.0f));

			if (m_pAttacker->IsPlayer() && m_pDefender->IsPlayer())
				nDmg = (int)(nDmg * 0.1f);

			m_nReflectDmg = nDmg;
		}
	}


	if ((m_dwAtkFlags & AF_MAGICSKILL) && m_pAttacker->IsPlayer())
	{
		switch (nSkill)
		{
		case SI_BIL_PST_BGVURTIALBOLD:
		case SI_BIL_PST_ASALRAALAIKUM:
			ChanceSkill(DST_CHR_CHANCESTUN, m_pAttacker, m_pDefender);
			break;
		}
	}

	if (m_dwAtkFlags & (AF_GENERIC | AF_MELEESKILL))
	{
		if (m_pAttacker->IsPlayer())
		{
			ChanceSkill(DST_CHR_CHANCESTUN, m_pAttacker, m_pDefender);
			ChanceSkill(DST_CHR_CHANCEPOISON, m_pAttacker, m_pDefender);
			ChanceSkill(DST_CHR_CHANCEDARK, m_pAttacker, m_pDefender);
			ChanceSkill(DST_CHR_CHANCEBLEEDING, m_pAttacker, m_pDefender);
			ChanceSkill(DST_CHR_CHANCESTEALHP, m_pAttacker, m_pDefender);
		}


		{
			ItemProp* pAttackerHandItemProp = m_pAttacker->GetActiveHandItemProp();
			if (pAttackerHandItemProp)
			{
				DWORD dwActiveSkill = pAttackerHandItemProp->dwActiveSkill;
				DWORD dwActiveSkillLevel = pAttackerHandItemProp->dwActiveSkillLv;
				if (dwActiveSkillLevel == NULL_ID)
					dwActiveSkillLevel = 1;
				if (dwActiveSkill != NULL_ID)
				{
					if (pAttackerHandItemProp->dwActiveSkillRate == NULL_ID ||
						(DWORD)(random(100)) < pAttackerHandItemProp->dwActiveSkillRate)
					{
						m_pAttacker->DoActiveSkill(dwActiveSkill, dwActiveSkillLevel, m_pDefender, true);
						TRACE("%sÀÇ ActiveSkill ¹ßµ¿, ", m_pAttacker->GetName());
					}
				}
			}
		}
	} // AF_GENERIC

	if (m_dwAtkFlags & (AF_MELEESKILL | AF_MAGICSKILL))
	{
		ItemProp* pSkillProp;
		AddSkillProp* pAddSkillProp;
		m_pAttacker->GetSkillProp(&pSkillProp, &pAddSkillProp, nSkill, pInfo->GetSkillLevel(), "ProcessAbnormal");
		DWORD dwActiveSkill = pAddSkillProp->dwActiveSkill;
		if (dwActiveSkill != NULL_ID)
		{
			DWORD dwActiveSkillRate = pAddSkillProp->dwActiveSkillRate;

			if (m_pAttacker->IsPlayer() && m_pDefender->IsPlayer())
				dwActiveSkillRate = pAddSkillProp->dwActiveSkillRatePVP;

			if (dwActiveSkillRate == NULL_ID || xRandom(100) < dwActiveSkillRate)
				m_pAttacker->DoActiveSkill(dwActiveSkill, pAddSkillProp->dwSkillLvl, m_pDefender);
		}
	}
}


int CAttackArbiter::MinusHP(int* pnDamage)
{
	int nHP = m_pDefender->GetHitPoint() - *pnDamage;
	if (nHP <= 0)
	{
		if (m_pDefender->m_dwMode & MATCHLESS2_MODE && m_pDefender->IsAuthHigher(AUTH_ADMINISTRATOR))
			nHP = 1;
		else
			nHP = 0;
		*pnDamage = m_pDefender->GetHitPoint() - nHP;
	}

	return nHP;
}

void CAttackArbiter::StealHP(int nDamage, ATK_TYPE type)
{
	ItemProp* pHandItem = m_pAttacker->GetActiveHandItemProp();
	if (pHandItem)
	{
		if (pHandItem->dwDestParam1 == DST_CHR_STEALHP)
		{
			int nStealHP = (int)(nDamage * (pHandItem->nAdjParamVal1 / 100.0f));
			m_pAttacker->SetDestParam(DST_HP, nStealHP, NULL_CHGPARAM);
		}
	}

	if (type == ATK_GENERIC && m_pAttacker->IsPlayer())
	{
		int nDstHP = m_pAttacker->GetParam(DST_MELEE_STEALHP, 0);
		int nStealHP = (int)(nDamage * (nDstHP / 100.0f));
		if (0 < nDstHP)
			m_pAttacker->SetDestParam(DST_HP, nStealHP, NULL_CHGPARAM);
	}
}


void CAttackArbiter::OnDied()
{
	if (!m_pDefender)
		return;
	if (m_pDefender->IsDie())
		return;

	m_pAttacker->AddKillRecovery();

	if (m_pDefender->IsNPC() && m_pDefender->IsDie())
	{
		m_pDefender->Delete();
		return;
	}

	if (m_pDefender->m_dwIndex == MI_CLOCKWORK1 || m_pDefender->m_dwIndex == MI_BIGMUSCLE || m_pDefender->m_dwIndex == MI_KRRR)
		m_pDefender->SubAroundExp(m_pAttacker, 50.0f);
	else
		m_pAttacker->SubExperience(m_pDefender);

	m_pDefender->DropItemByDied(m_pAttacker);

	m_pAttacker->m_nAtkCnt = 0;
	m_pDefender->DoDie(m_pAttacker, m_dwMsg);
}

BYTE CAttackArbiter::GetHandFlag(void)
{
	BYTE cbHandFlag = 0x01;

	if (m_dwAtkFlags & AF_RANGE)
		return cbHandFlag;

	if (m_dwAtkFlags & AF_GENERIC)
	{
		cbHandFlag = m_nParam & 0x03;
		if (cbHandFlag == 0)
			cbHandFlag = 0x01;
	}

	return cbHandFlag;
}