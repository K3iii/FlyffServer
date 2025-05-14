#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "resdata.h"
#include "defineObj.h"
#include "Party.h"
#ifdef __CLIENT
#include "Dialogmsg.h"
#endif

#include "eveschool.h"
#include "Mover.h"
#include "defineskill.h"

#ifdef __WORLDSERVER
#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif
#include "user.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "DPSrvr.h"
#include "AttackArbiter.h"

extern	CDPSrvr		g_DPSrvr;
extern	CUserMng			g_UserMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;
#include "ItemUpgrade.h"
#endif	// __WORLDSERVER

extern	CPartyMng				g_PartyMng;


static const int MIN_HIT_RATE = 20;
static const int MAX_HIT_RATE = 96;

enum
{
	SCRIPT_GETATTACKRESULT,
	SCRIPT_GETCRITICALPROBABILITY,
	SCRIPT_GETDAMAGEBASE,
	SCRIPT_ISBLOCKING,
	SCRIPT_CALCDEFENSE,
};


float GetAttackSpeedPlusValue(int n)
{
	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;

	int nIndex = n / 10;
	nIndex = max(nIndex, 0);
	nIndex = min(nIndex, (MAX_ATTACK_SPEED_PLUSVALUE - 1));

	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};

	return fPlusValue[nIndex];
}


float GetChargeMultiplier(int nLevel)
{
	ASSERT(nLevel <= 4);

	static float factors[5] = { 1.0f, 1.2f, 1.5f, 1.8f, 2.2f };
	if (0 <= nLevel && nLevel <= 4)
		return factors[nLevel];

	return 1.0f;
}


float GetWandATKMultiplier(int nLevel)
{
	switch (nLevel)
	{
	case 0:	return 0.6f;
	case 1:	return 0.8f;
	case 2: return 1.05f;
	case 3: return 1.1f;
	case 4: return 1.3f;
	}
	return 1.0f;
}

int GetWeaponPlusDamage(int nDamage, BOOL bRandom, ItemProp* pItemProp, int nOption)
{
	return 0;
}



int CMover::GetWeaponPlusDamage(int nDamage, BOOL bRandom)
{
	int nPlus = 0;
	CItemElem* pWeapon = GetWeaponItem();
	if (pWeapon)
	{
		int nOption = pWeapon->GetAbilityOption();
		if (nOption > 10)
			return nPlus;

		ItemProp* pItemProp = GetActiveHandItemProp();
		return ::GetWeaponPlusDamage(nDamage, bRandom, pItemProp, nOption);
	}
	return nPlus;
}



float CMover::GetAttackSpeed()
{
	float fSpeed = 1.0f;

	if (IsPlayer())
	{
		JobProp* pProperty = prj.GetJobProp(GetJob());
		if (pProperty == NULL)
			return fSpeed;

		float fItem = 1.0f;

		ItemProp* pWeaponProp = GetActiveHandItemProp();
		if (pWeaponProp != NULL)
			fItem = pWeaponProp->fAttackSpeed;

		int A = int(pProperty->fAttackSpeed + (fItem * (4.0f * GetDex() + GetLevel() / 8.0f)) - 3.0f);
		if (187.5f <= A)
			A = (int)(187.5f);
		fSpeed = ((50.0f / (200.f - A)) / 2.0f) + GetAttackSpeedPlusValue(A);
	}

	float fDstParam = GetParam(DST_ATTACKSPEED, 0) / 1000.0f;
	fSpeed += fDstParam;

	if (0 < GetParam(DST_ATTACKSPEED_RATE, 0))
		fSpeed = fSpeed + (fSpeed * GetParam(DST_ATTACKSPEED_RATE, 0) / 100);


	if (fSpeed < 0.1f)
	{
		fSpeed = 0.1f;
	}
	else if (fSpeed > 2.0f)
	{
		fSpeed = 2.0f;
	}

	return fSpeed;
}

int CMover::GetSpeedRun()
{
	int nSpeedRun;
	nSpeedRun = GetParam(DST_SPEED, 0);
	if (nSpeedRun > __MAX_SPEED)
		nSpeedRun = __MAX_SPEED;

	return nSpeedRun;
}

int CMover::GetCriticalRate()
{
	int nCriticalRate;
	nCriticalRate = GetParam(DST_CRITICAL_BONUS, 0);
	return nCriticalRate;
}

int CMover::GetSpellCastSpeed()
{
	int nSpellCastSpeed;
	nSpellCastSpeed = GetParam(DST_SPELL_RATE, 0);

	if (nSpellCastSpeed > 100)
		nSpellCastSpeed = 100;

	return nSpellCastSpeed;
}

int CMover::GetPvpDamage()
{
	int nPvpDamage;
	nPvpDamage = GetParam(DST_PVP_DMG, 0);
	return nPvpDamage;
}

int CMover::GetHitRate()
{
	int nHitRate;
	nHitRate = GetParam(DST_ADJ_HITRATE, 0);
	return nHitRate;
}

int CMover::GetBlockMelee()
{
	int nBlockMelee;
	nBlockMelee = GetParam(DST_BLOCK_MELEE, 0);
#ifdef __JEFF_11
	if (nBlockMelee < 0)
		nBlockMelee = 0;
	if (nBlockMelee > 96)
		nBlockMelee = 96;
#endif	// __JEFF_11
	return nBlockMelee;
}

int CMover::GetBlockRange()
{
	int nBlockRange;
	nBlockRange = GetParam(DST_BLOCK_RANGE, 0);
#ifdef __JEFF_11
	if (nBlockRange < 0)
		nBlockRange = 0;
	if (nBlockRange > 96)
		nBlockRange = 96;
#endif	// __JEFF_11
	return nBlockRange;
}

int CMover::GetDodgeRate()
{
	int nDodgeRate;
	nDodgeRate = (int)((GetDex() * 0.5));
	nDodgeRate = GetParam(DST_PARRY, 0);
#ifdef __JEFF_11
	if (nDodgeRate < 0)
		nDodgeRate = 0;
#endif	// __JEFF_11
	return nDodgeRate;
}

float CMover::GetCastingAniSpeed(void)
{
	float fSpeed = 1.0F + GetParam(DST_SPELL_RATE, 0) / 100.0F;
	if (fSpeed < 0.1f)
		fSpeed = 0.1f;
	else if (fSpeed > 2.0f)
		fSpeed = 2.0f;

	return fSpeed;
}

int CMover::GetCastingTime(int nCastingTime)
{
	int nTime = nCastingTime - nCastingTime * GetParam(DST_SPELL_RATE, 0) / 100;
	if (nTime < 0)
		nTime = 0;
	return nTime;
}

int CMover::GetDefenseByItem(BOOL bRandom)
{
	int nValue = m_nDefenseMax - m_nDefenseMin;
	if (bRandom)
		return m_nDefenseMin + (nValue > 0 ? xRandom(nValue) : 0);
	else
		return m_nDefenseMin + (nValue / 2);
}

int	CMover::GetShowDefense(BOOL bRandom)
{
	ATTACK_INFO info = { 0 };
	info.dwAtkFlags = AF_GENERIC;

	return CalcDefense(&info, FALSE);
}

// Hit rating
int CMover::GetHR()
{
	if (IsPlayer())
		return GetDex();
	else
		return GetProp()->dwHR;
}

bool CMover::isUnconditionedHit(CMover* pDefender) const
{
	if (::IsInvalidObj(pDefender))
		return false;

	if (m_dwMode & ONEKILL_MODE)
		return true;

#ifdef __EVENTLUA_SPAWN
	if (IsPlayer() && pDefender->IsNPC() && prj.m_EventLua.IsEventSpawnMonster(pDefender->GetIndex()))
		return true;
#endif // __EVENTLUA_SPAWN

	return false;
}

int		CMover::calculateHitRate(CMover* pDefender)
{
	if (::IsInvalidObj(pDefender))
		return MAX_HIT_RATE;

	int nHitRate = 0;
	int nHR = GetHR();

	if (IsNPC() && pDefender->IsPlayer())
	{
		nHitRate = (int)(((nHR * 1.5f) / (float)(nHR + pDefender->GetParrying())) * 2.0f *
			(GetLevel() * 0.5f / (float)(GetLevel() + pDefender->GetLevel() * 0.3f)) * 100.0f);
	}
	else if (IsPlayer() && pDefender->IsNPC())
	{
		nHitRate = (int)(((nHR * 1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.5f *
			(GetLevel() * 1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f);
	}
	else
		nHitRate = (int)(((nHR * 1.6f) / (float)(nHR + pDefender->GetParrying())) * 1.2f *
			(GetLevel() * 1.2f / (float)(GetLevel() + pDefender->GetLevel())) * 100.0f);

	nHitRate += GetAdjHitRate();

	return nHitRate;
}

BOOL CMover::GetAttackResult(CMover* pDefender)
{
	if (isUnconditionedHit(pDefender))
		return TRUE;

	const int calculatedHitRate = calculateHitRate(pDefender);

	const int correctedHitRate = calculatedHitRate < MAX_HIT_RATE ? max(calculatedHitRate, MIN_HIT_RATE) : MAX_HIT_RATE;

	return correctedHitRate > static_cast<int>(::xRandom(100));
}

int CMover::GetPlusWeaponATK(DWORD dwWeaponType)
{
	int nATK = 0;
	switch (dwWeaponType)
	{
	case WT_MELEE_SWD:
		nATK = GetParam(DST_SWD_DMG, 0);
		break;
	case WT_MELEE_AXE:
		nATK = GetParam(DST_AXE_DMG, 0);
		break;
	case WT_MELEE_KNUCKLE:
		nATK = GetParam(DST_KNUCKLE_DMG, 0);
		break;
	case WT_MELEE_YOYO:
		nATK = GetParam(DST_YOY_DMG, 0);
		break;
	case WT_RANGE_BOW:
		nATK = GetParam(DST_BOW_DMG, 0);
		break;
	}

	if (IsDualWeapon() == TRUE)
		nATK += GetParam(DST_ONEHANDMASTER_DMG, 0);
	else
	{
		switch (dwWeaponType)
		{
		case WT_MELEE_KNUCKLE:
			nATK += GetParam(DST_KNUCKLEMASTER_DMG, 0);
			break;
		case WT_MELEE_YOYO:
			nATK += GetParam(DST_YOYOMASTER_DMG, 0);
			break;
		case WT_RANGE_BOW:
			nATK += GetParam(DST_BOWMASTER_DMG, 0);
			break;
		case WT_MELEE_SWD:
		case WT_MELEE_AXE:
			nATK += GetParam(DST_TWOHANDMASTER_DMG, 0);
			break;
		}
	}

	return nATK;
}




float CMover::GetJobPropFactor(JOB_PROP_TYPE type)
{
	if (IsPlayer() == FALSE)
		return 1.0f;

	JobProp* pProperty = prj.GetJobProp(GetJob());
	ASSERT(pProperty);

	switch (type)
	{
	case JOB_PROP_SWD:
		return pProperty->fMeleeSWD;
	case JOB_PROP_AXE:
		return pProperty->fMeleeAXE;
	case JOB_PROP_STAFF:
		return pProperty->fMeleeSTAFF;
	case JOB_PROP_STICK:
		return pProperty->fMeleeSTICK;
	case JOB_PROP_KNUCKLE:
		return pProperty->fMeleeKNUCKLE;
	case JOB_PROP_WAND:
		return pProperty->fMagicWAND;
	case JOB_PROP_YOYO:
		return pProperty->fMeleeYOYO;
	case JOB_PROP_BLOCKING:
		return pProperty->fBlocking;
	case JOB_PROP_CRITICAL:
		return pProperty->fCritical;

	default:
		ASSERT(0);
		return 1.0f;
	}
}


int CMover::GetWeaponATK(DWORD dwWeaponType)
{
	int nATK = 0;
	switch (dwWeaponType)
	{
	case WT_MELEE_SWD:
		nATK = (int)(float((GetStr() - 12) * GetJobPropFactor(JOB_PROP_SWD)) + (float(GetLevel() * 1.1f)));
		break;
	case WT_MELEE_AXE:
		nATK = (int)(float((GetStr() - 12) * GetJobPropFactor(JOB_PROP_AXE)) + (float(GetLevel() * 1.2f)));
		break;
	case WT_MELEE_STAFF:
		nATK = (int)(float((GetStr() - 10) * GetJobPropFactor(JOB_PROP_STAFF)) + (float(GetLevel() * 1.1f)));
		break;
	case WT_MELEE_STICK:
		nATK = (int)(float((GetStr() - 10) * GetJobPropFactor(JOB_PROP_STICK)) + (float(GetLevel() * 1.3f)));
		break;
	case WT_MELEE_KNUCKLE:
		nATK = (int)(float((GetStr() - 10) * GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(GetLevel() * 1.2f)));
		break;
	case WT_MAGIC_WAND:
		nATK = (int)((GetInt() - 10) * GetJobPropFactor(JOB_PROP_WAND) + GetLevel() * 1.2f);
		break;
	case WT_MELEE_YOYO:
		nATK = (int)(float((GetStr() - 12) * GetJobPropFactor(JOB_PROP_YOYO)) + (float(GetLevel() * 1.1f)));
		break;
	case WT_RANGE_BOW:
		nATK = (int)((((GetDex() - 14) * 4.0f + (GetLevel() * 1.3f)) * 0.7f));
		break;
	}

	nATK += GetPlusWeaponATK(dwWeaponType);
	return nATK;
}



void CMover::GetHitMinMax(int* pnMin, int* pnMax, ATTACK_INFO* pInfo)
{
	int nParts = PARTS_RWEAPON;
	if (pInfo)
		nParts = pInfo->nParts;

	*pnMin = 0;
	*pnMax = 0;

	if (IsInvalidObj(this))
		return;

	if (IsPlayer())
	{
		ItemProp* pItemProp = GetActiveHandItemProp(nParts);
		if (pItemProp == NULL)
			return;

		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;

		*pnMin = GetParam(DST_ABILITY_MIN, *pnMin);
		*pnMax = GetParam(DST_ABILITY_MAX, *pnMax);

		int nPlus = GetWeaponATK(pItemProp->dwWeaponType) + GetParam(DST_CHR_DMG, 0);
		*pnMin += nPlus;
		*pnMax += nPlus;

		CItemElem* pWeapon = GetWeaponItem(nParts);
		if (pWeapon && pWeapon->GetProp())
		{
			float f = GetItemMultiplier(pWeapon);
			*pnMin = (int)(*pnMin * f);
			*pnMax = (int)(*pnMax * f);
		}

		if (pWeapon)
		{
			int nOption = pWeapon->GetAbilityOption();
			if (nOption > 0)
			{
				int nValue = (int)(pow((float)(nOption), 1.5f));

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
		#ifdef __WEAPON_RARITY
		if (pWeapon && pWeapon->m_nWeaponRarity > 0)
		{
			int nMinRarity;
			int nMaxRarity;
			switch (pWeapon->m_nWeaponRarity)
			{
			case 5:
				nMinRarity = (int)(*pnMin * 0.25f);
				nMaxRarity = (int)(*pnMax * 0.25f);
				break;
			case 4:
				nMinRarity = (int)(*pnMin * 0.2f);
				nMaxRarity = (int)(*pnMax * 0.2f);
				break;
			case 3:
				nMinRarity = (int)(*pnMin * 0.15f);
				nMaxRarity = (int)(*pnMax * 0.15f);
				break;
			case 2:
				nMinRarity = (int)(*pnMin * 0.1f);
				nMaxRarity = (int)(*pnMax * 0.1f);
				break;
			case 1:
				nMinRarity = (int)(*pnMin * 0.05f);
				nMaxRarity = (int)(*pnMax * 0.05f);
				break;
			}
			if (nMinRarity < 1)
				*pnMin += 1;
			else
				*pnMin += nMinRarity;

			if (nMaxRarity < 1)
				*pnMax += 1;
			else
				*pnMax += nMaxRarity;
		}
		#endif // __WEAPON_RARITY
	}
	else
	{
		MoverProp* pMoverProp = GetProp();
		*pnMin = pMoverProp->dwAtkMin;
		*pnMax = pMoverProp->dwAtkMax;

		*pnMin = GetParam(DST_ABILITY_MIN, *pnMin);
		*pnMax = GetParam(DST_ABILITY_MAX, *pnMax);

		*pnMin = GetParam(DST_CHR_DMG, *pnMin);
		*pnMax = GetParam(DST_CHR_DMG, *pnMax);

		DWORD dwAtk = pMoverProp->dwAtk1;
		if (pInfo)
		{
			DWORD dwValue = pInfo->nParam >> 16;
			if (dwValue != 0 && dwValue != NULL_ID)
				dwAtk = dwValue;
		}

		if (dwAtk != NULL_ID)
		{
			ItemProp* pItemProp = prj.GetItemProp(dwAtk);
			if (pItemProp)
			{
				*pnMin += pItemProp->dwAbilityMin;
				*pnMax += pItemProp->dwAbilityMax;
			}
		}
	}
}


float CMover::GetDEFMultiplier(ATTACK_INFO* pInfo)
{
	float fFactor = 1.0f;
	if (IsNPC())
	{
#ifdef __S1108_BACK_END_SYSTEM
		fFactor *= GetProp()->m_fDefence_Rate;
#endif //	__S1108_BACK_END_SYSTEM
	}


	if (pInfo->GetSkill() == SI_BLD_DOUBLE_ARMORPENETRATE)
		fFactor *= 0.5f;


	fFactor *= (1.0f + (float)GetParam(DST_ADJDEF_RATE, 0) / 100.0f);

	return fFactor;
}


int CMover::CalcDefense(ATTACK_INFO* pInfo, BOOL bRandom)
{
	int nDefense = CalcDefenseCore(pInfo->pAttacker, pInfo->dwAtkFlags, bRandom);
	nDefense = (int)(nDefense * GetDEFMultiplier(pInfo));
#if defined(__WORLDSERVER) // __EVENTLUA_ATKDEF
	if (IsPlayer())
		nDefense += prj.m_EventLua.GetDefensePower();
#endif // __EVENTLUA_ATKDEF
	return nDefense;
}


int CMover::CalcDefenseNPC(CMover* pAttacker, DWORD dwAtkFlags)
{
	float	fDefense = 0.0f;

	MoverProp* pProp = GetProp();
	if (dwAtkFlags & AF_MAGIC)
	{
		fDefense = pProp->dwResisMgic / 7.0f + 1;
	}
	else
	{
		ASSERT((dwAtkFlags & AF_MELEESKILL) == AF_MELEESKILL);
		fDefense = pProp->dwNaturalArmor / 7.0f + 1;
	}

	return (int)fDefense;
}


int CMover::CalcDefensePlayer(CMover* pAttacker, DWORD dwAtkFlags)
{
	int nDefense = 0;
	if (pAttacker->IsPlayer())
	{
		if (dwAtkFlags & AF_MAGIC)
		{
			nDefense = (int)(float((GetInt() * 9.04f) + (GetLevel() * 35.98f)));
		}
		else
		{
			int nDEF = GetDefenseByItem() + GetParam(DST_ADJDEF, 0);
			nDefense = (int)((float(nDEF * 2.3f) + (float(GetLevel() + (GetSta() / 2) + GetDex()) / 2.8f) - 4 + GetLevel() * 2));
		}
	}
	else
	{
		nDefense = (int)(((GetDefenseByItem() / 4 + GetParam(DST_ADJDEF, 0)) +
			(float(GetLevel() + (GetSta() / 2) + GetDex()) / 2.8f) - 4 + GetLevel() * 2));
	}
#ifdef __JEFF_11
	if (nDefense < 0)
		nDefense = 0;
#endif	// __JEFF_11
	return nDefense;
}


int CMover::CalcDefenseCore(CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom)
{
	if (dwAtkFlags & AF_MAGICSKILL)
		return GetResistMagic();

	BOOL bGeneric = (dwAtkFlags & AF_GENERIC);
	if (IsPlayer() && pAttacker && pAttacker->IsPlayer())
		bGeneric = TRUE;

	if (bGeneric)
	{
		float fFactor = 1.0f;
		if (IsPlayer())
		{
			JobProp* pProperty = prj.GetJobProp(GetJob());
			assert(pProperty);
			fFactor = pProperty->fFactorDef;
		}
		int nDefense = (int)(((((GetLevel() * 2) + (GetSta() / 2)) / 2.8f) - 4) + ((GetSta() - 14) * fFactor));
		nDefense += (GetDefenseByItem(bRandom) / 4);
		nDefense += GetParam(DST_ADJDEF, 0);
#ifdef __JEFF_11
		if (nDefense < 0)
			nDefense = 0;
#endif	// __JEFF_11
		return nDefense;
	}

	if (IsNPC())
		return CalcDefenseNPC(pAttacker, dwAtkFlags);
	else
		return CalcDefensePlayer(pAttacker, dwAtkFlags);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int CMover::GetCriticalProb(void)
{
	int nProb;
	nProb = (GetDex() / 10);
	nProb = (int)(nProb * GetJobPropFactor(JOB_PROP_CRITICAL));
	nProb = GetParam(DST_CHR_CHANCECRITICAL, nProb);
#ifdef __JEFF_11
	if (nProb < 0)
		nProb = 0;
#endif	// __JEFF_11

	if (IsPlayer())
	{
		if (m_idparty && (m_dwFlag & MVRF_CRITICAL))
		{
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty(m_idparty);
			if (pParty && pParty->IsMember(m_idPlayer))
				nProb += (pParty->m_nSizeofMember / 2);
#else // __WORLDSERVER
			if (g_Party.IsMember(m_idPlayer))
				nProb += (g_Party.m_nSizeofMember / 2);
#endif // __WORLDSERVER
			m_dwFlag &= (~MVRF_CRITICAL);
		}

	}
	return nProb;
}

#ifdef __WORLDSERVER
int CMover::CalcLinkAttackDamage(int nDamage)
{
	int nAdd = 0;
	if (IsPlayer() && m_idparty && nDamage > 0)
	{
		CParty* pParty = g_PartyMng.GetParty(m_idparty);
		if (pParty && pParty->IsMember(m_idPlayer))
		{
			if (pParty->m_nKindTroup == 1 && pParty->m_nModeTime[PARTY_LINKATTACK_MODE])
			{
				int nJoinMember = 0;

				// If red scroll in party, then LA working everywhere and based on amount members on party
				if (pParty->m_nModeTime[PARTY_PARSKILL_MODE])
					nJoinMember = pParty->m_nSizeofMember;
				else
				{
					// If no red scroll in party, then LA working only for leader and all near members and based on amount members on party
					CUser* pLeader = g_UserMng.GetUserByPlayerID(pParty->m_aMember[0].m_uPlayerId);
					if (IsValidObj((CObj*)pLeader))
					{
						if (pLeader->IsValidArea(this, 255.0f))
							nJoinMember = pParty->m_nSizeofMember;
						else
							return 0;
					}
					else
						return 0;
				}

				nAdd = (nDamage * nJoinMember / 10);
			}
		}
	}
	return nAdd;
}


BOOL CMover::IsCriticalAttack(CMover* pDefender, DWORD dwAtkFlags)
{
	if (IsSkillAttack(dwAtkFlags))
		return FALSE;

	return ((int)(xRandom(100)) < GetCriticalProb());
}

float CMover::GetBlockFactor(CMover* pAttacker, ATTACK_INFO* pInfo)
{
	if (IsPlayer())
	{
		int r = xRandom(80);
		if (r <= 5)
			return 1.0f;
		if (r >= 75)
			return 0.1f;

		float fBlockA = GetLevel() / ((GetLevel() + pAttacker->GetLevel()) * 15.0f);
		float fBlockB = (GetDex() + pAttacker->GetDex() + 2) * ((GetDex() - pAttacker->GetDex()) / 800.0f);
		if (fBlockB > 10.0f)
			fBlockB = 10.0f;
		float fAdd = fBlockA + fBlockB;
		if (fAdd < 0.0f)
			fAdd = 0.0f;

		if (pInfo->IsRangeAttack())
			fAdd += GetParam(DST_BLOCK_RANGE, 0);
		else
			fAdd += GetParam(DST_BLOCK_MELEE, 0);

		int nBR = (int)((GetDex() / 8.0f) * GetJobPropFactor(JOB_PROP_BLOCKING) + fAdd);
		if (nBR < 0)
			nBR = 0;

		if (nBR > r)
			return 0.0f;
	}
	else
	{
		int r = xRandom(100);

		if (r <= 5)
			return 1.0f;
		if (r >= 95)
			return 0.1f;

		int	nBR = (int)((GetParrying() - GetLevel()) * 0.5f);
		if (nBR < 0)
			nBR = 0;

		if (nBR > r)
			return 0.2f;
	}
	return 1.0f;
}

BOOL CMover::IsBlocking(CMover* pAttacker)
{
	int r = xRandom(100);

	if (r <= 5)
		return FALSE;
	if (r >= 95)
		return TRUE;

	int nBR = 0;

	if (IsPlayer())
	{
		CItemElem* pShield = GetEquipItem(PARTS_SHIELD);
		if (pShield)
		{
			nBR = (GetDex() - 30) / (2 * GetLevel());
			ItemProp* pProp = pShield->GetProp();
			nBR += pProp->dwblockRating;


			if (nBR < 2)
				nBR = 2;
			else if (nBR > 65)
				nBR = 65;
		}
		else
			nBR = 0;
	}
	else
	{
		nBR = (int)((GetParrying() - GetLevel()) * 0.5f);
		if (nBR < 0)
			nBR = 0;
	}

	return (nBR > r);
}


float CMover::GetDamageMultiplier(ATTACK_INFO* pInfo)
{
	float factor = 1.0f;
	int nSkill = pInfo->GetSkill();

	ItemProp* pSkillProp = NULL;
	AddSkillProp* pAddSkillProp = NULL;
	DWORD	dwProbability = 0;

	if (nSkill)
	{
		pSkillProp = prj.GetSkillProp(nSkill);
		if (pSkillProp)
		{
			pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, pInfo->GetSkillLevel());
			if (pAddSkillProp)
			{
				dwProbability = pAddSkillProp->nProbability;

				if (pInfo->pAttacker->IsPlayer() && pInfo->pDefender->IsPlayer())
					dwProbability = pAddSkillProp->nProbabilityPVP;

				if (pAddSkillProp->nSkillCount > 0)
					factor /= (float)pAddSkillProp->nSkillCount;
			}
		}
	}


	if (HasBuff(BUFF_SKILL, SI_ACR_SUP_DARKILLUSION))
	{
		switch (nSkill)
		{
		case SI_JST_YOYO_BACKSTAB:
		case SI_JST_YOYO_VATALSTAB:
		case SI_ACR_BOW_SILENTSHOT:
		{
			factor *= 1.4f;
			break;
		}
		}
	}

	switch (nSkill)
	{
	case SI_JST_YOYO_VATALSTAB:
		if (xRandom(100) < dwProbability)
			factor *= 2.0f;
		break;
	case SI_ACR_BOW_AIMEDSHOT:
		if (xRandom(100) < dwProbability)
			factor = 4.0f;
		break;
	case SI_ACR_BOW_JUNKBOW:
		if (xRandom(100) > dwProbability)
			factor = 0.0f;
		break;
	case SI_JST_YOYO_HITOFPENYA:
		factor *= (pAddSkillProp->nDestData1[0] / 100.0f);
		break;
	}


	if (nSkill == SI_PSY_PSY_SPRITBOMB)
	{
		int nPercent = GetManaPointPercent();
		factor *= ((float)nPercent / 100) * 2.0f;
	}

	if (IsNPC())
	{
		int nBerserkHP = GetProp()->m_nBerserkHP;
		if (nBerserkHP > 0 && GetHitPointPercent(100) <= nBerserkHP)
			factor *= GetProp()->m_fBerserkDmgMul;
	}
	else
	{
		if (pInfo->pDefender->IsPlayer())
			factor *= 0.60f;

		if (pInfo->nParts == PARTS_LWEAPON)
			factor *= 0.75f;
	}

	if (pInfo->pAttacker->IsPlayer())
		if (pInfo->pDefender->GetAdjParam(DST_CHRSTATE) & CHS_DOUBLE)
			factor *= 2.0f;

	int nDelta = pInfo->pDefender->GetLevel() - pInfo->pAttacker->GetLevel();


	if (pInfo->pAttacker->IsNPC())
	{
		DWORD dwClass = pInfo->pAttacker->GetProp()->dwClass;
		switch (dwClass)
		{
		case RANK_SUPER:

			nDelta = 0;
			break;
		}
	}

	if (nDelta > 0)
	{
		if (pInfo->pAttacker->IsNPC() || pInfo->pDefender->IsNPC())
		{
			const int MAX_OVER_ATK = 16;
			nDelta = min(nDelta, (MAX_OVER_ATK - 1));

			static const float DeltaFactor[MAX_OVER_ATK] =
			{ 1.0f,
			0.995f,0.981f,0.957f,0.924f,0.882f,
			0.831f,0.773f,0.707f,0.634f,0.556f,
			0.471f,0.383f,0.290f,0.195f,0.098f
			};

			factor *= DeltaFactor[nDelta];
		}
	}

	return factor;
}



int CMover::GetMagicSkillPower(ATTACK_INFO* pInfo)
{
	CMover* pDefender = pInfo->pDefender;
	int nSkill = pInfo->GetSkill();
	int nATK = GetMeleeSkillPower(pInfo);
	nATK = GetParam(DST_ADDMAGIC, nATK);

	ItemProp* pSkillProp = prj.GetSkillProp(nSkill);
	if (!pSkillProp)
		return nATK;

	float fRatio = 0.0f;
	switch (pSkillProp->dwSpellType)
	{
	case ST_FIRE:
		fRatio = GetParam(DST_MASTRY_FIRE, 0) / 100.0f;
		nATK = (int)(nATK + (nATK * fRatio));
		break;
	case ST_WATER:
		fRatio = GetParam(DST_MASTRY_WATER, 0) / 100.0f;
		nATK = (int)(nATK + (nATK * fRatio));
		break;
	case ST_ELECTRICITY:
		fRatio = GetParam(DST_MASTRY_ELECTRICITY, 0) / 100.0f;
		nATK = (int)(nATK + (nATK * fRatio));
		break;
	case ST_WIND:
		fRatio = GetParam(DST_MASTRY_WIND, 0) / 100.0f;
		nATK = (int)(nATK + (nATK * fRatio));
		break;
	case ST_EARTH:
		fRatio = GetParam(DST_MASTRY_EARTH, 0) / 100.0f;
		nATK = (int)(nATK + (nATK * fRatio));
		break;
	}

	return nATK;
}


float CMover::GetMagicSkillFactor(CMover* pDefender, SAI79::ePropType skillType) // Attacker damage based on weapon+skill element
{
	SAI79::ePropType itemType;
	CItemElem* pWeapon = GetWeaponItem();
	if (pWeapon && pWeapon->m_bItemResist != SAI79::NO_PROP)
		itemType = (SAI79::ePropType)pWeapon->m_bItemResist;
	else
	{
		ItemProp* pItemProp = GetActiveHandItemProp();
		if (pItemProp)
			itemType = pItemProp->eItemType;
		else
			return 1.0f;
	}

	// If skill have element and weapon is same element then 1.1
	// If Skill have element and weapon is anti element then 0.9f
	// If skill have not element or weapon is neutral element then 1.0
#ifndef __AIO // Element on magic weapons will not decrease damage which done by skills with no-element
	if (skillType == itemType)
#else
	if ((skillType == itemType) && skillType != 0)
#endif
	{
		return 1.1f;
	}
	else if ((skillType == 1 && itemType == 2) ||
		(skillType == 2 && itemType == 3) ||
		(skillType == 3 && itemType == 5) ||
		(skillType == 5 && itemType == 4) ||
		(skillType == 4 && itemType == 1))
	{
		return 0.9f;
	}
	else
		return 1.0f;
}

int CMover::PostCalcMagicSkill(int nATK, ATTACK_INFO* pInfo)
{
	CMover* pDefender = pInfo->pDefender;
	int					nSkill = pInfo->GetSkill();
	int					nDelta = pDefender->GetLevel() - GetLevel();
	SAI79::ePropType	skillType = SAI79::NO_PROP;
	ItemProp* pSkillProp = prj.GetSkillProp(nSkill);
	if (!pSkillProp)
		return 0;
	else
		skillType = pSkillProp->eItemType;

	float a, b;
	if (IsNPC() || pDefender->IsNPC())
		b = 1.0f + (int)(nDelta / 0.05f);
	else
		b = 1.0f;

	if (b <= 0.0f)
		b = 0.0f;

	int nDEF = pDefender->CalcDefense(pInfo);

	if (pInfo->dwAtkFlags & AF_MAGICSKILL)
		nATK = nATK - nATK * pDefender->GetParam(DST_RESIST_MAGIC_RATE, 0) / 100;

	float fDefenderResist = 0.0f; //__AIO : Suit element useless vs skill attacks in pvp now
	if (IsPlayer() && pDefender->IsPlayer())
		fDefenderResist = 0.0f;
	else
		fDefenderResist = pDefender->GetResist(skillType);

	a = (nATK - nDEF) * (1.0f - fDefenderResist);

	return	((int)((int)a * GetMagicSkillFactor(pDefender, skillType)));
}

#ifndef __FL_DST_ATKPOWER_RATE_MAGIC
float CMover::GetATKMultiplier(CMover* pDefender, DWORD dwAtkFlags)
#else
float CMover::GetATKMultiplier(CMover* pDefender, ATTACK_INFO* pInfo)
#endif
{
	float fMultiplier = 1.0f;

	int nParam = GetParam(DST_ATKPOWER_RATE, 0);
#ifdef __FL_DST_ATKPOWER_RATE_MAGIC
	ATK_TYPE type = pInfo->GetAtkType();
	if (type == ATK_MAGICSKILL)
		nParam += GetParam(DST_ATKPOWER_RATE_MAGIC, 0);
#endif

	FLOAT fRate = 1.0f + (nParam / 100.0f);
	fMultiplier *= fRate;

	int nDstDmg = 0;
	if (IsPlayer())
	{
		if (IsSMMode(SM_ATTACK_UP))
			fMultiplier *= 1.2f;

		if (pDefender->IsPlayer())
			nDstDmg = GetParam(DST_PVP_DMG, 0);
		else
			nDstDmg = GetParam(DST_MONSTER_DMG, 0);
	}
	else
	{
#ifdef __S1108_BACK_END_SYSTEM
		fMultiplier *= (prj.m_fMonsterHitRate * GetProp()->m_fAttackPower_Rate);
#else
		fMultiplier *= prj.m_fMonsterHitRate;
#endif
	}

	if (0 < nDstDmg)
		fMultiplier += (fMultiplier * nDstDmg / 100.0f);

	return fMultiplier;
}

void CMover::GetDamagePropertyFactor(CMover* pDefender, int* pnATKFactor, int* pnDEFFactor, int nParts)
{
	*pnATKFactor = 10000;
	*pnDEFFactor = 10000;

	SAI79::ePropType atkType, defType;
	int atkLevel, defLevel;
	atkType = defType = SAI79::NO_PROP;

	atkLevel = defLevel = 0;

	if (IsPlayer())
	{
		CItemElem* pItemElem = GetWeaponItem(nParts);
		if (pItemElem && pItemElem->m_bItemResist != SAI79::NO_PROP)
		{
			atkType = (SAI79::ePropType)pItemElem->m_bItemResist;
			atkLevel = pItemElem->m_nResistAbilityOption;
		}
	}
	else
	{
		atkType = GetProp()->eElementType;
		atkLevel = GetProp()->wElementAtk;
	}


	if (atkType == SAI79::NO_PROP)
		return;

	if (pDefender->IsPlayer())
	{
		CItemElem* pItemElem = pDefender->GetEquipItem(PARTS_UPPER_BODY);
		if (pItemElem)
		{
			defType = (SAI79::ePropType)pItemElem->m_bItemResist;
			defLevel = pItemElem->m_nResistAbilityOption;
		}
	}
	else
	{
		defType = pDefender->GetProp()->eElementType;
		defLevel = pDefender->GetProp()->wElementAtk;
	}

	if (defType == SAI79::NO_PROP)
		return;

	if (IsPlayer() && pDefender->IsPlayer()) //__AIO : Suit element useless vs physical attacks in pvp now
		return;

	static int table[SAI79::END_PROP][SAI79::END_PROP] = {
		{0, 0, 0, 0, 0, 0},
		{0, 1, 2, 0, 3, 0},
		{0, 3, 1, 2, 0, 0},
		{0, 0, 3, 1, 0, 2},
		{0, 2, 0, 0, 1, 3},
		{0, 0, 0, 3, 2, 1}
	};

	int result = table[atkType][defType];

	int nFactor = 0, nLevel = 0;
	switch (result)
	{
	case 0:
	case 1:
	{
		if (atkLevel > 0 && defLevel == 0)
			nFactor += CItemUpgrade::GetInstance()->GetAttributeDamageFactor(atkLevel);
		else if (atkLevel == 0 && defLevel > 0)
			nFactor -= CItemUpgrade::GetInstance()->GetAttributeDefenseFactor(defLevel - 3);
		else if (atkLevel > 0 && defLevel > 0)
			nLevel = atkLevel - defLevel;
		break;
	}

	case 2:
		nLevel = (atkLevel - 5) - defLevel;
		break;

	case 3:
		nLevel = atkLevel - (defLevel > 5 ? defLevel - 5 : 0);
		if (nLevel > 0)
			nFactor += CItemUpgrade::GetInstance()->GetAttributeAddAtkDmgFactor(nLevel);
		break;
	}

	if (nLevel != 0)
		nFactor += (nLevel > 0) ? CItemUpgrade::GetInstance()->GetAttributeDamageFactor(nLevel)
		: -(CItemUpgrade::GetInstance()->GetAttributeDefenseFactor(-nLevel));
	*pnATKFactor += nFactor;
	*pnDEFFactor += nFactor;
}

int CMover::GetHitPower(ATTACK_INFO* pInfo)
{
	int nMin, nMax, nATKFactor, nATK;
	GetDamagePropertyFactor(pInfo->pDefender, &nATKFactor, &pInfo->nDEFFactor, pInfo->nParts);

	GetHitMinMax(&nMin, &nMax, pInfo);

	if (IsCriticalAttack(pInfo->pDefender, pInfo->dwAtkFlags))
	{
		pInfo->dwAtkFlags |= AF_CRITICAL;

		float fMin = 1.1f;
		float fMax = 1.4f;
		if (GetLevel() > pInfo->pDefender->GetLevel())
		{
			if (pInfo->pDefender->IsNPC())
			{
				fMin = 1.2f;
				fMax = 2.0f;
			}
			if (IsNPC())
			{
				fMin = 1.4f;
				fMax = 1.8f;
			}
		}


		float fCriticalBonus = 1 + (float)GetParam(DST_CRITICAL_BONUS, 0) / 100.0F;
#ifdef __JEFF_11
		if (fCriticalBonus < 0.1F)
			fCriticalBonus = 0.1F;
#endif	// __JEFF_11
		nMin = (int)(nMin * fMin * fCriticalBonus);
		nMax = (int)(nMax * fMax * fCriticalBonus);

		BOOL bFlyByAttack = xRandom(100) < 15;
		ItemProp* pItemProp = GetActiveHandItemProp();
		if (pItemProp && pItemProp->dwWeaponType == WT_MELEE_YOYO || (pInfo->dwAtkFlags & AF_FORCE))
			bFlyByAttack = FALSE;
		if (pInfo->pDefender->IsPlayer())
			bFlyByAttack = FALSE;

		if (pInfo->pDefender->CanFlyByAttack() && bFlyByAttack)
			pInfo->dwAtkFlags |= AF_FLYING;
	}

	nATK = xRandom(nMin, nMax);
	nATK = MulDiv(nATK, nATKFactor, 10000);
	if (pInfo->IsRangeAttack())
		nATK = (int)(nATK * GetChargeMultiplier(pInfo->GetChargeLevel()));

	return nATK;
}

int CMover::PostCalcGeneric(int nATK, ATTACK_INFO* pInfo)
{
	int nDEF = pInfo->pDefender->CalcDefense(pInfo);
	nDEF = MulDiv(nDEF, pInfo->nDEFFactor, 10000);

	int nDamage = nATK - nDEF;
	if (nDamage > 0)
	{
		float fBlock = pInfo->pDefender->GetBlockFactor(this, pInfo);
		if (fBlock < 1.0f)
		{
			pInfo->dwAtkFlags |= AF_BLOCKING;
			nDamage = (int)(nDamage * fBlock);
		}
	}
	else
	{
		nDamage = 0;
	}


	if (pInfo->pAttacker->IsNPC() && pInfo->pDefender->IsPlayer())
	{
		int nMin = (int)(max(0, nATK * 0.1f));
		nDamage = max(nDamage, nMin);
	}

	nDamage += GetWeaponPlusDamage(nDamage);
	if (nDamage == 0)
	{
		pInfo->dwAtkFlags &= ~AF_CRITICAL;
		pInfo->dwAtkFlags &= ~AF_FLYING;
	}
	return nDamage;
}

int	CMover::GetMeleeSkillPower(ATTACK_INFO* pInfo)
{
	int nSkill = pInfo->GetSkill();
	int nSkillLv = pInfo->GetSkillLevel();

	ItemProp* pItemProp = GetActiveHandItemProp();
	ItemProp* pSkillProp = prj.GetSkillProp(nSkill);
	if (!pItemProp || !pSkillProp)
	{
		Error("CMover::GetMeleeSkillPower() - nSkill : %d, Name : %s, Item : %d",
			nSkill, pInfo->pAttacker ? pInfo->pAttacker->GetName() : "xxx", pItemProp ? pItemProp->dwID : 0);
		return 1;
	}
	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, nSkillLv);
	if (!pAddSkillProp)
		return 1;

	CItemElem* pWeapon = GetWeaponItem();
	int nMin, nMax;
	GetItemATKPower(&nMin, &nMax, pItemProp, pWeapon);

	int   nReferStat = 0, nReferStat1 = 0, nReferStat2 = 0;
	switch (pSkillProp->dwReferStat1)
	{
	case DST_STR:		nReferStat1 = GetStr();		break;
	case DST_DEX:		nReferStat1 = GetDex();		break;
	case DST_INT:		nReferStat1 = GetInt();		break;
	case DST_STA:		nReferStat1 = GetSta();		break;
	}
	switch (pSkillProp->dwReferStat2)
	{
	case DST_STR:		nReferStat2 = GetStr();		break;
	case DST_DEX:		nReferStat2 = GetDex();		break;
	case DST_INT:		nReferStat2 = GetInt();		break;
	case DST_STA:		nReferStat2 = GetSta();		break;
	}

	if (pSkillProp->dwReferTarget1 == RT_ATTACK)
		if (pSkillProp->dwReferValue1 != NULL_ID)
			nReferStat1 = (int)(((pSkillProp->dwReferValue1 / 10.0f) * nReferStat1) + (pAddSkillProp->dwSkillLvl * FLOAT(nReferStat1 / 50.0f)));

	if (pSkillProp->dwReferTarget2 == RT_ATTACK)
		if (pSkillProp->dwReferValue2 != NULL_ID)
			nReferStat2 = (int)(((pSkillProp->dwReferValue2 / 10.0f) * nReferStat2) + (pAddSkillProp->dwSkillLvl * FLOAT(nReferStat2 / 50.0f)));

	nReferStat = nReferStat1 + nReferStat2;
	float fPowerMin, fPowerMax;

	DWORD dwSkillAbilityMin = pAddSkillProp->dwAbilityMin;
	DWORD dwSkillAbilityMax = pAddSkillProp->dwAbilityMax;

	if (pInfo->pAttacker->IsPlayer() && pInfo->pDefender->IsPlayer())
	{
		dwSkillAbilityMin = pAddSkillProp->dwAbilityMinPVP;
		dwSkillAbilityMax = pAddSkillProp->dwAbilityMaxPVP;
	}

	if (pWeapon)
	{
		fPowerMin = (float)(((nMin + ((int)dwSkillAbilityMin + pWeapon->GetProp()->nAddSkillMin) * 5 + nReferStat - 20) * (16 + nSkillLv) / 13));
		fPowerMax = (float)(((nMax + ((int)dwSkillAbilityMax + pWeapon->GetProp()->nAddSkillMax) * 5 + nReferStat - 20) * (16 + nSkillLv) / 13));
	}
	else
	{
		fPowerMin = (float)(((nMin + (int)dwSkillAbilityMin * 5 + nReferStat - 20) * (16 + nSkillLv) / 13));
		fPowerMax = (float)(((nMax + (int)dwSkillAbilityMax * 5 + nReferStat - 20) * (16 + nSkillLv) / 13));
	}

	int nWeaponDamage = GetPlusWeaponATK(pItemProp->dwWeaponType);

	fPowerMin = (float)(GetParam(DST_CHR_DMG, (int)(fPowerMin)) + nWeaponDamage);
	fPowerMax = (float)(GetParam(DST_CHR_DMG, (int)(fPowerMax)) + nWeaponDamage);

	float fMinMax = (fPowerMax - fPowerMin) + 1;
	if (fMinMax <= 0)
		fMinMax = 1;
	float fPower = (float)(fPowerMin + xRandom((DWORD)(fMinMax)));

	return (int)fPower;
}




int CMover::GetMagicHitPower(int nChargeLevel)
{
	ItemProp* pItemProp = GetActiveHandItemProp();
	CItemElem* pWeapon = GetWeaponItem();

	int nMin, nMax;
	GetItemATKPower(&nMin, &nMax, pItemProp, pWeapon);

	int nATK = GetWeaponATK(WT_MAGIC_WAND);
	nMin += nATK;
	nMax += nATK;

	int nDamage = xRandom(nMin, nMax);
	nDamage += GetParam(DST_CHR_DMG, 0);
	nDamage = (int)(nDamage * GetWandATKMultiplier(nChargeLevel));
	return nDamage;
}


int CMover::ApplyDPC(int nATK, ATTACK_INFO* pInfo)
{
	int nDamage;
	if (pInfo->CanIgnoreDEF())
		nDamage = nATK;
	else
	{
		int nDefense = CalcDefense(pInfo);
		nDamage = nATK - nDefense;
	}

	if (nDamage < 0)
		nDamage = 0;


	if (pInfo->pAttacker->IsCriticalAttack(this, pInfo->dwAtkFlags))
	{
		pInfo->dwAtkFlags |= AF_CRITICAL;

		int nChargeLevel = pInfo->GetChargeLevel();
		if ((pInfo->pAttacker->m_pActMover->GetState() & OBJSTA_ATK4) ||
			nChargeLevel == MAX_CHARGE_LEVEL)
		{
			nDamage = (int)(nDamage * 2.6f);
			if (CanFlyByAttack() && xRandom(100) < 50)
				pInfo->dwAtkFlags |= AF_FLYING;
		}
		else
		{
			nDamage = (int)(nDamage * 2.3f);
			if (CanFlyByAttack() && xRandom(100) < 30)
				pInfo->dwAtkFlags |= AF_FLYING;
		}
		float fCriticalBonus = 1 + (float)pInfo->pAttacker->GetParam(DST_CRITICAL_BONUS, 0) / 100.0f;
#ifdef __JEFF_11
		if (fCriticalBonus < 0.1F)
			fCriticalBonus = 0.1F;
#endif	// __JEFF_11
		nDamage = (int)(nDamage * fCriticalBonus);
	}
	return nDamage;
}

void CMover::OnAttacked(CMover* pAttacker, int nDamage, BOOL bTarget, int nReflect)
{
	CMover* pDefender = this;
	if (pDefender == pAttacker)
		return;

	HITTYPE type = pAttacker->GetHitType(pDefender, bTarget, nReflect);
	if (type == HITTYPE_GENERIC)
	{
		pDefender->AddEnemy(pAttacker->GetId(), nDamage);
		pAttacker->AddEnemy(pDefender->GetId(), 0);
	}

	if (pAttacker->IsPlayer() && pDefender->IsNPC())
	{
		if (pDefender->m_idAttacker == NULL_ID)
			((CUser*)pAttacker)->AddCorrReq(pDefender);

		pDefender->m_idAttacker = pAttacker->GetId();
	}

	pAttacker->m_idLastHitMover = pDefender->GetId();
}

HITTYPE	CMover::GetHitType2(CMover* pMover, BOOL bTarget, BOOL bGood)
{
	if (!IsValidObj((CObj*)this) || !IsValidObj((CObj*)pMover))
		return HITTYPE_GENERIC;
	if (IsDie() || pMover->IsDie())
		return HITTYPE_FAIL;

	if (IsPlayer() && pMover->IsPlayer())
	{
		if (m_pActMover->IsFly() || pMover->m_pActMover->IsFly())
			return HITTYPE_FAIL;
		if (IsGuildCombatTarget(pMover))
			return HITTYPE_ARENA;
#ifdef __JEFF_11_4
		if (IsArenaTarget(pMover))
			return HITTYPE_ARENA;
#endif	// 
#ifdef __FL_FFA
		if (CFFAManager::GetInstance()->IsFFAEnjoyer(this) && CFFAManager::GetInstance()->IsFFAEnjoyer(pMover))
			return HITTYPE_ARENA;
#endif
#ifdef __FL_TDM
		if (CTDMManager::GetInstance()->IsCanKill(this, pMover))
			return HITTYPE_ARENA;
#endif
		return HITTYPE_FAIL;
	}
	else
		return HITTYPE_GENERIC;

	return HITTYPE_FAIL;
}

HITTYPE	CMover::GetHitType(CMover* pMover, BOOL bTarget, int nReflect)
{
	if (!IsValidObj((CObj*)this) || !IsValidObj((CObj*)pMover))
		return HITTYPE_FAIL;
	if (IsDie() || pMover->IsDie())
		return HITTYPE_FAIL;

	if (this == pMover)
		return HITTYPE_GENERIC;

	if (IsPlayer() && pMover->IsPlayer())
	{
		if (m_pActMover->IsFly() || pMover->m_pActMover->IsFly())
			return HITTYPE_FAIL;

		if (IsGuildCombatTarget(pMover))
			return HITTYPE_ARENA;

#ifdef __JEFF_11_4
		if (IsArenaTarget(pMover))
			return HITTYPE_ARENA;
#endif	// __JEFF_11_4

#ifdef __FL_FFA
		if (CFFAManager::GetInstance()->IsFFAEnjoyer(this) && CFFAManager::GetInstance()->IsFFAEnjoyer(pMover))
			return HITTYPE_ARENA;
#endif
#ifdef __FL_TDM
		if (CTDMManager::GetInstance()->IsCanKill(this, pMover))
			return HITTYPE_ARENA;
#endif

		return HITTYPE_FAIL;
	}
	else
	{
		if (pMover->IsNPC() && IsAttackAbleNPC(pMover) == FALSE)
			return HITTYPE_FAIL;

		bool bAttackerFly = (IsNPC() && IsFlyingNPC()) || (IsPlayer() && IsFly());
		bool bDefenderFly = (pMover->IsNPC() && pMover->IsFlyingNPC()) || (pMover->IsPlayer() && pMover->IsFly());

		if (bAttackerFly != bDefenderFly)
			return HITTYPE_FAIL;

		return HITTYPE_GENERIC;
	}

	return HITTYPE_FAIL;
}
#endif // __WORLDSERVER

#ifdef __JEFF_11_4
BOOL	CMover::IsArenaTarget(CMover* pMover)
{
	CWorld* pWorld = GetWorld();
	if (pWorld && pWorld->IsArena())
	{
		int dx = (int)(pMover->GetPos().x - 490);
		int dy = (int)(pMover->GetPos().z - 507);
		int d = dx * dx + dy * dy;
		if (d > 1246)
			return FALSE;
		dx = (int)(GetPos().x - 490);
		dy = (int)(GetPos().z - 507);
		d = dx * dx + dy * dy;
		if (d > 1246)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}
#endif	// __JEFF_11_4

BOOL CMover::IsGuildCombatTarget(CMover* pMover)
{
#ifdef __WORLDSERVER
	if (g_GuildCombatMng.m_nGCState != CGuildCombat::WAR_WAR_STATE)
		return FALSE;
#endif // __WORLDSERVER

	if (m_nGuildCombatState != 1 || pMover->m_nGuildCombatState != 1)
		return FALSE;

	if (m_idGuild == pMover->m_idGuild)
		return FALSE;

	return TRUE;
}

bool	CMover::pushedByAnotherPower(const float angle, const float power)
{
	if (power == 0.0f)
		return false;

	MoverProp* pMoverProp = prj.GetMoverProp(GetIndex());
	if (!pMoverProp)
		return false;

	if (pMoverProp->dwClass == RANK_MIDBOSS || pMoverProp->dwClass == RANK_BOSS || pMoverProp->dwClass == RANK_SUPER || pMoverProp->dwClass == RANK_MATERIAL)
		return false;

	if (IsFlyingNPC() || IsFly())
		return false;

	if (m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL)
		return false;

	if (GetSpeed(m_pActMover->m_fSpeed) <= 0)
		return false;

	AngleToVectorXZ(&m_pActMover->m_vDeltaE, angle, power);
#ifdef __WORLDSERVER
	g_UserMng.AddPushPower(this, GetPos(), angle, power);
#endif

	return true;
}

bool CMover::CanFlyByAttack(BOOL bCheckSpeed)
{
	MoverProp* pMoverProp = prj.GetMoverProp(GetIndex());
	if (!pMoverProp)
		return false;

	if (pMoverProp->dwClass == RANK_MIDBOSS || pMoverProp->dwClass == RANK_BOSS || pMoverProp->dwClass == RANK_SUPER || pMoverProp->dwClass == RANK_MATERIAL)
		return false;

	if (IsFlyingNPC() || IsFly())
		return false;

	if (m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL)
		return false;

	if (bCheckSpeed)
	{
		if (GetSpeed(m_pActMover->m_fSpeed) <= 0)
			return false;
	}

	return true;
}

void CMover::FlyByAttack(BOOL bCheckSpeed)
{
	if (!CanFlyByAttack(bCheckSpeed))
		return;

	ClearCastingSkills();

	m_pActMover->ResetState(OBJSTA_JUMP_ALL);
	m_pActMover->SetState(OBJSTA_DMG_ALL, OBJSTA_DMG_FLY);
	SetMotion(MTI_DMGFLY, ANILOOP_CONT);
	m_pActMover->SendActMsg(OBJMSG_STOP);

	const float fAngleXZ = GetAngle();	
	const float fAngleY = 145.0f;
	const float fPower = 0.13f;

	float fTheta = D3DXToRadian(fAngleXZ);
	float fTheta2 = D3DXToRadian(fAngleY);
	m_pActMover->m_vDelta.y += -cosf(fTheta2) * fPower;
	float fDist = sinf(fTheta2) * fPower;
	m_pActMover->m_vDeltaE.x += sinf(fTheta) * fDist;
	m_pActMover->m_vDeltaE.z += -cosf(fTheta) * fDist;
}

float CMover::GetItemMultiplier(CItemElem* pItemElem)
{
	if (pItemElem->IsFlag(CItemElem::expired))
		return 0.0f;

	ItemProp* pItemProp = pItemElem->GetProp();
	ASSERT(pItemProp);

	float fValue = 1.0f;


	int nOption = pItemElem->GetAbilityOption();
	if (pItemProp->dwReferStat1 == WEAPON_ULTIMATE)
		nOption = 10;

	if (nOption)
	{
		int nValue = prj.GetExpUpItem(pItemProp->dwItemKind3, nOption);
		nValue += 100;
		fValue = fValue * nValue / 100.0f;
	}
	return fValue;
}

void CMover::GetItemATKPower(int* pnMin, int* pnMax, ItemProp* pItemProp, CItemElem* pWeapon)
{
	float f = 1.0f;
	int nOption, nAdd;
	nAdd = nOption = 0;

	if (pWeapon)
		nOption = pWeapon->GetAbilityOption();

	if (nOption > 0)
		nAdd = (int)(pow((float)(nOption), 1.5f));

	if (pWeapon)
	{
		f = GetItemMultiplier(pWeapon);
	}
	*pnMin = (int)(GetItemAbilityMin(pItemProp->dwID) * f) + nAdd;
	*pnMax = (int)(GetItemAbilityMax(pItemProp->dwID) * f) + nAdd;
}


int CMover::GetReqMp(int nReqMp)
{
	if (nReqMp == 9999999)
		return nReqMp;

	return nReqMp - nReqMp * GetParam(DST_MP_DEC_RATE, 0) / 100;
}

int CMover::GetReqFp(int nReqFp)
{
	if (nReqFp == 9999999)
		return nReqFp;

	return nReqFp - nReqFp * GetParam(DST_FP_DEC_RATE, 0) / 100;
}

float CMover::GetResist(SAI79::ePropType type)
{
	if (SAI79::NO_PROP == type)
		return 0.0f;

	int n;
	switch (type)
	{
	case SAI79::FIRE:
		n = DST_RESIST_FIRE;
		break;
	case SAI79::WATER:
		n = DST_RESIST_WATER;
		break;
	case SAI79::ELECTRICITY:
		n = DST_RESIST_ELECTRICITY;
		break;
	case SAI79::WIND:
		n = DST_RESIST_WIND;
		break;
	case SAI79::EARTH:
		n = DST_RESIST_EARTH;
		break;
	}

	return GetResistSpell(n) / 100.0f;
}

void CMover::OnAttackSFX(OBJID	idTarget, int nMagicPower, DWORD dwSkill, int nDmgCnt, float fDmgAngle, float fDmgPower, DWORD dwAtkFlags, BOOL bControl)
{
#ifdef __WORLDSERVER
	SFXHIT_INFO si =
	{ idTarget, nMagicPower, dwSkill, nDmgCnt, fDmgAngle, fDmgPower, dwAtkFlags, bControl };

	AddSFXInfo(idTarget, si);
#endif
}

#ifdef __WORLDSERVER
int CMover::GetSFXCount(OBJID idTarget)
{
	map<OBJID, queue<SFXHIT_COUNT> >::iterator it = m_mapSFXCount.find(idTarget);
	if (it != m_mapSFXCount.end())
		return it->second.size();
	return 0;
}

void CMover::IncSFXCount(OBJID idTarget, DWORD dwSkill)
{
	map<OBJID, queue<SFXHIT_COUNT> >::iterator it = m_mapSFXCount.find(idTarget);
	if (it != m_mapSFXCount.end())
	{
		queue<SFXHIT_COUNT>& q = it->second;
		SFXHIT_COUNT c = { dwSkill, GetTickCount() };
		q.push(c);
	}
	else
	{
		queue<SFXHIT_COUNT> q;
		SFXHIT_COUNT c = { dwSkill, GetTickCount() };
		q.push(c);
		m_mapSFXCount.insert(map<OBJID, queue<SFXHIT_COUNT> >::value_type(idTarget, q));
	}
}

void CMover::AddSFXInfo(OBJID idTarget, SFXHIT_INFO& info)
{
	info.dwTickCount = GetTickCount();

	map<OBJID, queue<SFXHIT_INFO> >::iterator it = m_mapSFXInfo.find(idTarget);
	if (it != m_mapSFXInfo.end())
	{
		queue<SFXHIT_INFO>& q = it->second;
		q.push(info);
	}
	else
	{
		queue<SFXHIT_INFO> q;
		q.push(info);
		m_mapSFXInfo.insert(map<OBJID, queue<SFXHIT_INFO> >::value_type(idTarget, q));
	}
}

void CMover::ClearSFX(OBJID idTarget)
{
	m_mapSFXInfo.erase(idTarget);
	m_mapSFXCount.erase(idTarget);
}

void CMover::RemoveSFX(OBJID idTarget, int id, BOOL bForce, DWORD dwSkill)
{
	if (idTarget != NULL_ID)
	{
		CMover* pTarget = prj.GetMover(idTarget);
		if (IsValidObj(pTarget))
			IncSFXCount(idTarget, dwSkill);
		else
			ClearSFX(idTarget);
	}
}


BOOL AttackBySFX(CMover* pAttacker, SFXHIT_INFO& info)
{
	CMover* pTarget = prj.GetMover(info.idTarget);
	if (IsInvalidObj(pTarget))
		return FALSE;

	int	nParam = pTarget->m_pActMover->MakeParamDamage(info.nMagicPower, (int)info.dwSkill, info.nDmgCnt);
	if (pTarget->IsLive() == FALSE)
		return FALSE;


	if (info.dwAtkFlags & AF_GENERIC || info.dwAtkFlags & AF_MAGIC)
	{
		if (pAttacker->GetAttackResult(pTarget) == FALSE)
			info.dwAtkFlags = AF_MISS;
	}

	if (!g_eLocal.GetState(EVE_STEAL) && pAttacker->IsSteal(pTarget))
		return TRUE;

	if (IsSkillAttack(info.dwAtkFlags))
	{

		LPSKILL pSkill = pAttacker->GetSkill(info.dwSkill);
		if (pSkill)
		{
			ItemProp* pSkillProp;
			AddSkillProp* pAddSkillProp;
			BOOL bSuccess = pAttacker->GetSkillProp(&pSkillProp, &pAddSkillProp, info.dwSkill, pSkill->dwLevel, "OnSfxHit");
			if (bSuccess)
				pAttacker->DoApplySkill(pTarget, pSkillProp, pAddSkillProp, false, nParam, FALSE, FALSE);
		}
		else
			Error("OnSfxHit : skill=%d�� ������ ����", info.dwSkill);
	}
	else
		pTarget->m_pActMover->SendDamage(info.dwAtkFlags, pAttacker->GetId(), nParam);

	if (info.fDmgPower)
		pTarget->pushedByAnotherPower(info.fDmgAngle, info.fDmgPower);

	if (pTarget->IsLive() == FALSE)
		return FALSE;

	return TRUE;
}
#endif
