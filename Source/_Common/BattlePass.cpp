#include "StdAfx.h"
#include "BattlePass.h"

#ifdef __FL_BATTLE_PASS
#if defined (__WORLDSERVER)
#include "defineObj.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern	CUserMng	g_UserMng;
extern CDPDatabaseClient g_dpDBClient;
#endif //__WORLDSERVER

#if defined (__WORLDSERVER) || defined (__CLIENT)
CBattlePass::CBattlePass()
	: nBattlePassID(0),
	bActiveTime(FALSE),
	bRequiredItem(FALSE),
	nMaxBattlePassLevel(0),
	nExpPerMonster(0),
	nExpPerBoss(0),
	nExpGW_Kill(0),
	nExpGW_MvP(0),
	nExpGW_Win(0),
	nExpSR_Win(0),
	nLvlBundle_1(0),
	nLvlBundle_2(0),
	nLvlBundle_3(0)
{

}
CBattlePass::~CBattlePass()
{
	mapBattlePass.clear();
}
CBattlePass* CBattlePass::GetInstance()
{
	static CBattlePass sBP;
	return &sBP;
}
void CBattlePass::ReadConfig(const LPCTSTR strFile)
{
	CScript s;
	if (!s.Load(strFile))
	{
		Error("CBattlePass::ReadScript cant load %s", strFile);
		return;
	}

	mapBattlePass.clear();
	nBattlePassID = 0;

	int nBattleLevel = 0;
	s.GetToken();
	while (s.tok != FINISHED)
	{
		BATTLE_PASS_LEVELS bpLevels;

		if (s.Token == "ADD_PASS")
		{
			nBattlePassID = s.GetNumber();
			bActiveTime = TRUE;
			s.GetToken();
		}
		else if (s.Token == "REQUIRED_ITEM")
		{
			bRequiredItem = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "DATETIME_END")
		{
			int nDayEnd, nMonthEnd, nYearEnd, nHourEnd, nMinEnd, nSecEnd;

			nDayEnd = s.GetNumber();
			nMonthEnd = s.GetNumber();
			nYearEnd = s.GetNumber();
			nHourEnd = s.GetNumber();
			s.GetToken();
			nMinEnd = s.GetNumber();
			s.GetToken();
			nSecEnd = s.GetNumber();
			s.GetToken();

#ifdef __WORLDSERVER
			CTime tm(nYearEnd, nMonthEnd, nDayEnd, nHourEnd, nMinEnd, nSecEnd);
			tEnd = tm;
#endif
		}
		else if (s.Token == "EXP_MOB")
		{
			nExpPerMonster = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "EXP_BOSS")
		{
			nExpPerBoss = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "EXP_GW_KILL")
		{
			nExpGW_Kill = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "EXP_GW_MVP")
		{
			nExpGW_MvP = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "EXP_GW")
		{
			nExpGW_Win = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "EXP_SR")
		{
			nExpSR_Win = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "LEVEL_BATTLE_PASS_BUNDLE_1")
		{
			nLvlBundle_1 = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "LEVEL_BATTLE_PASS_BUNDLE_2")
		{
			nLvlBundle_2 = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "LEVEL_BATTLE_PASS_BUNDLE_3")
		{
			nLvlBundle_3 = s.GetNumber();
			s.GetToken();
		}
		else if (s.Token == "ADD_LEVEL")
		{
			nBattleLevel += 1;
			bpLevels.nExp = s.GetNumber();
			bpLevels.dwItemID = s.GetNumber();
			bpLevels.nItemNum = s.GetNumber();
			bpLevels.bBinds = s.GetNumber();
			s.GetToken();

			mapBattlePass.insert(make_pair(nBattleLevel, bpLevels));
			nMaxBattlePassLevel = nBattleLevel;
		}
	}
}
int CBattlePass::GetExpForThisLevel(const int nLevel)
{
	map<int, BATTLE_PASS_LEVELS>::iterator i = mapBattlePass.find(nLevel);
	if (i != mapBattlePass.end())
		return i->second.nExp;

	return 0;
}
DWORD CBattlePass::GetRewardIdForThisLevel(const int nLevel)
{
	map<int, BATTLE_PASS_LEVELS>::iterator i = mapBattlePass.find(nLevel);
	if (i != mapBattlePass.end())
		return i->second.dwItemID;

	return NULL_ID;
}
int CBattlePass::GetRewardNumForThisLevel(const int nLevel)
{
	map<int, BATTLE_PASS_LEVELS>::iterator i = mapBattlePass.find(nLevel);
	if (i != mapBattlePass.end())
		return i->second.nItemNum;

	return 0;
}
BOOL CBattlePass::GetRewardBindsForThisLevel(const int nLevel)
{
	map<int, BATTLE_PASS_LEVELS>::iterator i = mapBattlePass.find(nLevel);
	if (i != mapBattlePass.end())
		return i->second.bBinds;

	return FALSE;
}
void CBattlePass::Process()
{
#ifdef __WORLDSERVER
	if (!bActiveTime)
		return;

	CTime timeCurrent = CTime::GetCurrentTime();
	if (timeCurrent > tEnd)
	{
		bActiveTime = FALSE;
		g_DPCoreClient.SendCaption("The Battle Pass season is over.");
	}
#else // __CLIENT
	m_tCurrentTime = (m_tEndTime - time_null());

	if (m_tEndTime && m_tCurrentTime > 0)
		m_ct = m_tCurrentTime;
#endif
}
#endif //__WORLDSERVER || __CLIENT

#if defined (__WORLDSERVER)
BOOL CBattlePass::CheckRequirments(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return FALSE;
	else if (!bActiveTime)
		return FALSE;
	else if (pUser->GetBattlePassLevel() >= GetMaxBattlePassLevel())
		return FALSE;
	else if (IsNotFree())
	{
		if (!CheckRemoveTicket(pUser))
			return FALSE;
	}

	return TRUE;

}
BOOL CBattlePass::CheckRemoveTicket(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return FALSE;

	IBuff* pBuff = pUser->m_buffs.GetBuffByIk3(IK3_BATTLEPASS);
	if (!pBuff)
		return FALSE;

	ItemProp* pItemProp = pBuff->GetProp();
	if (!pItemProp)
		return FALSE;

	if (pItemProp->nMaxRepair != GetBattlePassID())
	{
		pUser->RemoveIk3Buffs(IK3_BATTLEPASS);
		return FALSE;
	}
	
	return TRUE;
}
void CBattlePass::AddProgress(CUser* pUser, const int nType)
{
	if (!CheckRequirments(pUser))
		return;

	// LEVEL BUNDLES
	int nLvlTmp = pUser->GetBattlePassLevel();
	switch (nType)
	{
	case LEVEL_BATTLE_PASS_BUNDLE_1: nLvlTmp += nLvlBundle_1; break;
	case LEVEL_BATTLE_PASS_BUNDLE_2: nLvlTmp += nLvlBundle_2; break;
	case LEVEL_BATTLE_PASS_BUNDLE_3: nLvlTmp += nLvlBundle_3; break;
	}

	if (nLvlTmp != pUser->GetBattlePassLevel())
	{
		if (nLvlTmp > GetMaxBattlePassLevel())
			nLvlTmp = GetMaxBattlePassLevel();

		while (pUser->GetBattlePassLevel() != nLvlTmp)
		{
			pUser->SetBattlePassLevel(pUser->GetBattlePassLevel() + 1);
			PayReward(pUser, FALSE);
		}

		if (pUser->GetBattlePassLevel() >= GetMaxBattlePassLevel())
			pUser->SetBattlePassExp(0);

		g_UserMng.AddCreateSfxObj(((CMover*)pUser), XI_NPCMETEONYKER);
		pUser->AddBattlePassState(pUser->GetBattlePassLevel(), pUser->GetBattlePassExp());

		CString str;
		str.Format("Bundle accepted. Your battle pass level has increased!");
		pUser->AddText(str);
		return;
	}

	// EXP
	int nExp = pUser->GetBattlePassExp();
	int nExpTmp = nExp;
	switch (nType)
	{
	case EXP_MOB: nExpTmp += nExpPerMonster; break;
	case EXP_BOSS: nExpTmp += nExpPerBoss; break;
	case EXP_GW_KILL: nExpTmp += nExpGW_Kill; break;
	case EXP_GW_MVP: nExpTmp += nExpGW_MvP; break;
	case EXP_GW: nExpTmp += nExpGW_Win; break;
	case EXP_SR: nExpTmp += nExpSR_Win; break;
	}

	if (nExp == nExpTmp) // Check if exp was not increased (+0), if so, then we have no reason to continue
		return;

	pUser->SetBattlePassExp(nExpTmp);

	while (pUser->GetBattlePassExp() >= GetExpForThisLevel(pUser->GetBattlePassLevel() + 1))
	{
		if (pUser->GetBattlePassLevel() >= GetMaxBattlePassLevel())
		{
			pUser->SetBattlePassExp(0);
			break;
		}

		pUser->SetBattlePassExp(pUser->GetBattlePassExp() - GetExpForThisLevel(pUser->GetBattlePassLevel() + 1));
		pUser->SetBattlePassLevel(pUser->GetBattlePassLevel() + 1);
		PayReward(pUser, TRUE);
		g_UserMng.AddCreateSfxObj(((CMover*)pUser), XI_BATTLE_PASS);
	}
	pUser->AddBattlePassState(pUser->GetBattlePassLevel(), pUser->GetBattlePassExp());
}
void CBattlePass::PayReward(CUser* pUser, BOOL bAddText)
{
	ItemProp* pItemProp = prj.GetItemProp(GetRewardIdForThisLevel(pUser->GetBattlePassLevel()));
	if (!pItemProp)
		return;

	int nNum = GetRewardNumForThisLevel(pUser->GetBattlePassLevel());
	if (nNum == 0)
		return;
	else if (nNum > (int)pItemProp->dwPackMax)
		nNum = 1;

	CItemElem pItemElem;
	pItemElem.m_dwItemId = pItemProp->dwID;
	pItemElem.m_nItemNum = nNum;
	pItemElem.m_nHitPoint = pItemProp->dwEndurance;
	if (GetRewardBindsForThisLevel(pUser->GetBattlePassLevel()))
		pItemElem.SetFlag(CItemElem::binds);
	pItemElem.SetSerialNumber();

	if (pUser->CreateItem(&pItemElem) == FALSE)
		g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, pItemElem, 0, "Battle Pass", "Your battle pass reward!");

	if (bAddText)
	{
		CString str;
		str.Format("Congratulations! The battle pass level has been increased.");
		pUser->AddText(str, 0xff9fad21);
	}
}
#endif
#endif