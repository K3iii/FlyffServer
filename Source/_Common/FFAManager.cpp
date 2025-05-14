#include "StdAfx.h"
#include "FFAManager.h"

#include "User.h"
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern	CUserMng	g_UserMng;
extern CDPDatabaseClient g_dpDBClient;

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#ifdef __FL_FFA
CFFAManager::CFFAManager()
	: m_bValidConfig(FALSE)
	, m_nJoinCost(0)
	, m_nJoinLevel(0)
	, m_nMinMembers(0)
	, m_nMaxMembers(0)
	, m_nLifes(0)
	, m_nAnnouncementTime(0)
	, m_nRespawnTime(-1)
	, m_nPrepareTime(0)
	, m_nWarTime(0)
	, m_dwRewardID(NULL_ID)
	, m_nRewardAmount(0)
	, m_nCurrentState(STATE_CHILL)
	, m_timeNextStateTime(0)
{

}
CFFAManager::~CFFAManager()
{
	mapTimes.clear();
	mapPlayers.clear();
}
CFFAManager* CFFAManager::GetInstance()
{
	static CFFAManager sBP;
	return &sBP;
}
void CFFAManager::ReadConfig()
{
	CScript s;
	if (!s.Load("FFAManager.inc"))
	{
		Error("CFFaManager::ReadConfig cant load FFAManager.inc");
		return;
	}

	if (m_nCurrentState != STATE_CHILL)
	{
		Error("CFFaManager::ReadConfig live refresh only possible when STATE_CHILL");
		return;
	}

	m_bValidConfig = FALSE;
	mapTimes.clear();

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "FFA_COST")
		{
			m_nJoinCost = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_MIN_LEVEL")
		{
			m_nJoinLevel = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_MIN_MEMBERS")
		{
			m_nMinMembers = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_MAX_MEMBERS")
		{
			m_nMaxMembers = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_LIFES")
		{
			m_nLifes = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_ANNOUNCE_TIME")
		{
			m_nAnnouncementTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_RESPAWN_TIME")
		{
			m_nRespawnTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_PREPARATION_TIME")
		{
			m_nPrepareTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_WAR_TIME")
		{
			m_nWarTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "FFA_REWARD")
		{
			m_dwRewardID = s.GetNumber();
			m_nRewardAmount = s.GetNumber();

			ItemProp* pProp = prj.GetItemProp(m_dwRewardID);
			if (!pProp)
			{
				Error("Your FFA config is fucked up, buddy (Invalid reward ID)");
				return;
			}

			if (m_nRewardAmount < 1 || m_nRewardAmount > pProp->dwPackMax)
			{
				Error("Your FFA config is fucked up, buddy (Invalid reward amount)");
				return;
			}

			s.GetToken();
		}
		if (s.Token == "FFA_BEGIN")
		{
			int nTmpDay = s.GetNumber();
			if (nTmpDay < 1 || nTmpDay > 7)
			{
				Error("Your FFA config is fucked up, buddy (Day: %d)", nTmpDay);
				return;
			}
			int nTmpHour = s.GetNumber();
			if (nTmpHour < 0 || nTmpHour > 23)
			{
				Error("Your FFA config is fucked up, buddy (Hour: %d)", nTmpHour);
				return;
			}
			int nTmpMinute = s.GetNumber();
			if (nTmpMinute < 0 || nTmpMinute > 59)
			{
				Error("Your FFA config is fucked up, buddy (Minute: %d)", nTmpMinute);
				return;
			}

			nTmpDay = nTmpDay + 1;
			if (nTmpDay == 8)
				nTmpDay = 1;

			FFA_TIME tmpTimes;
			tmpTimes.nHour = nTmpHour;
			tmpTimes.nMinute = nTmpMinute;

			auto it = mapTimes.find(nTmpDay);
			if (it != mapTimes.end())
			{
				for (auto& it2 : it->second)
				{
					if (it2.nHour == nTmpHour)
					{
						Error("[FFA] You should have at least 1 hour distance between starting time");
						return;
					}
				}
				it->second.push_back(tmpTimes);
			}
			else
			{
				vector<FFA_TIME> vecTmp;
				vecTmp.push_back(tmpTimes);
				mapTimes.insert(make_pair(nTmpDay, vecTmp));
			}

			s.GetToken();
		}
	}

	m_bValidConfig = TRUE;
}
void CFFAManager::Process()
{
	if (!m_bValidConfig)
		return;

	if (g_uIdofMulti != 1) // 1 channel only
		return;

	if (m_nCurrentState == STATE_CHILL)
	{
		CTime ctime = CTime::GetCurrentTime();
		int nDayofWeek = ctime.GetDayOfWeek();

		auto it = mapTimes.find(nDayofWeek);
		if (it != mapTimes.end())
		{
			for (auto& it2 : it->second)
			{
				if (it2.nHour == ctime.GetHour() && it2.nMinute == ctime.GetMinute())
				{
					AnnounceFFA();
					break;
				}
			}
		}
	}
	else if (m_nCurrentState == STATE_ANNOUNCE)
	{
		if (IsNextStateTime())
			OpenFFA();
	}
	else if (m_nCurrentState == STATE_PREPARE_BATTLE)
	{
		if (IsNextStateTime())
			StartBattle();
	}
	else if (m_nCurrentState == STATE_PROCESS_BATTLE)
	{
		if (IsNextStateTime())
			EndBattle();
		else
			ProcessRespawn();
	}
	else if (m_nCurrentState == STATE_PREPARE_CLOSE)
	{
		if (IsNextStateTime())
			CloseFFA();
	}
}
void CFFAManager::AnnounceFFA()
{
	if (m_nCurrentState != STATE_CHILL)
		return;

	m_nCurrentState = STATE_ANNOUNCE;
	SetNextStateTime(m_nAnnouncementTime, 0);

	CString str;
	str.Format("FFA will start in %d minute(s). Register now!", m_nAnnouncementTime);
	g_DPCoreClient.SendSystem(str);
}
void CFFAManager::OpenFFA()
{
	if (m_nCurrentState != STATE_ANNOUNCE)
		return;

	if (GetPlayerCount() < m_nMinMembers)
	{
		m_nCurrentState = STATE_CHILL;
		return;
	}

	m_nCurrentState = STATE_PREPARE_BATTLE;
	SetNextStateTime(m_nPrepareTime, 0);

	for (auto& it : mapPlayers)
	{
		it.second.nLifes = m_nLifes;

		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
			pUser->AddFFAState(FALSE);
	}

	CString str;
	str.Format("FFA will start in %d minute(s). All registered players can join now!", m_nPrepareTime);
	g_DPCoreClient.SendSystem(str);
}
void CFFAManager::StartBattle()
{
	m_nCurrentState = STATE_PROCESS_BATTLE;
	SetNextStateTime(m_nWarTime, 0);

	for (auto& it : mapPlayers)
	{
		it.second.nRespawn = m_nRespawnTime;

		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
			pUser->AddFFAState(FALSE);
	}

	CString str;
	str.Format("FFA started. It will be finished in %d minute(s). Enjoy the battle!", m_nWarTime);
	g_DPCoreClient.SendSystem(str);
}
void CFFAManager::EndBattle()
{
	m_nCurrentState = STATE_PREPARE_CLOSE;
	SetNextStateTime(0, 15);

	ItemProp* pProp = prj.GetItemProp(m_dwRewardID);
	multimap<int, u_long> mmapByKills;

	for (auto& it : mapPlayers)
	{
		// Prepare for announce
		mmapByKills.insert(make_pair(it.second.nKils, it.first));

		// Pay rewards
		int nRewardTotal = m_nRewardAmount * it.second.nKils;
		if (nRewardTotal > 9999)
			nRewardTotal = 9999;

		CItemElem pItemElem;
		pItemElem.m_dwItemId = m_dwRewardID;
		pItemElem.m_nItemNum = nRewardTotal;
		pItemElem.m_nHitPoint = pProp->dwEndurance;
		pItemElem.SetSerialNumber();

		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
		{
			if (nRewardTotal >= 1)
			{
				if (!pUser->CreateItem(&pItemElem))
					g_dpDBClient.SendQueryPostMail(it.first, 0, pItemElem, 0, "FFA Rewards", "Here is your rewards for FFA battle!");
			}

			// Add timer
			pUser->AddFFAState(FALSE);
		}
		else
		{
			if (nRewardTotal >= 1)
				g_dpDBClient.SendQueryPostMail(it.first, 0, pItemElem, 0, "FFA Rewards", "Here is your rewards for FFA battle!");
		}
	}

	// Announce best player(s)
	CString strMain;
	for (multimap<int, u_long>::reverse_iterator j = mmapByKills.rbegin(); j != mmapByKills.rend(); ++j)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(j->second);
		if (pUser)
			strMain.Format("FFA finished! Best player(s) is: %s", pUser->GetName());
		else
		{
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(j->second);
			if (pPlayerData)
				strMain.Format("FFA finished! Best player(s) is: %s", pPlayerData->szPlayer);
		}

		// Send create statue
		break;
	}

	g_DPCoreClient.SendSystem(strMain);
}
void CFFAManager::CloseFFA()
{
	m_nCurrentState = STATE_CHILL;
	m_timeNextStateTime = 0;

	for (auto& it : mapPlayers)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
		{
			pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_FFA_CLEAR);

			if (pUser->GetWorld() && pUser->GetWorld()->IsFFA())
				pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 100.0f, 3328.8f), REPLACE_NORMAL, nDefaultLayer);
		}
	}

	// TODO: Ranking

	mapPlayers.clear();

	CString str;
	str.Format("FFA closed.");
	g_DPCoreClient.SendSystem(str);
}
BOOL CFFAManager::IsRegisteredPlayer(CUser* pUser)
{
	auto it = mapPlayers.find(pUser->m_idPlayer);
	if (it != mapPlayers.end())
		return TRUE;

	return FALSE;
}
void CFFAManager::RegisterPlayer(CUser* pUser)
{
	if (!m_bValidConfig)
		return;

	if (!IsValidObj(pUser))
		return;

	if (m_nCurrentState != STATE_CHILL && m_nCurrentState != STATE_ANNOUNCE)
	{
		pUser->AddDefinedText(TID_SBEVE_SBNJOIN);
		return;
	}

	if (IsRegisteredPlayer(pUser))
	{
		pUser->AddText("You already registered.", 0xffffffff);
		return;
	}

	if (GetPlayerCount() >= m_nMaxMembers)
	{
		pUser->AddText("There is no more space for new participants.", 0xffffffff);
		return;
	}

	if (pUser->GetLevel() < m_nJoinLevel)
	{
		pUser->AddDefinedText(TID_GAME_USINGNOTLEVEL);
		return;
	}

	if (m_nJoinCost > 0)
	{
		//if (pUser->GetGold() < m_nJoinCost)
		if (pUser->CheckUserGold(m_nJoinCost, false) == false)
		{
			pUser->AddDefinedText(TID_DIAG_0009);
			return;
		}
		pUser->AddGold(-m_nJoinCost);
	}

	FFA_PLAYER tmp;
	tmp.nRespawn = -1;
	tmp.nKils = 0;
	tmp.nLifes = 0;
	mapPlayers.insert(make_pair(pUser->m_idPlayer, tmp));

	pUser->AddText("You successfully registered!", 0xffffffff);
	pUser->AddFFAData();
}
time_t CFFAManager::GetNextFFA()
{
	if (!m_bValidConfig)
		return 0;

	CTime currenttime = CTime::GetCurrentTime();
	int nDayofWeek = currenttime.GetDayOfWeek();
	time_t dTimet = 0;

	auto it = mapTimes.find(nDayofWeek);
	if (it != mapTimes.end())
	{
		int nMinFoundHour = 24;
		for (auto& it2 : it->second)
		{
			if (it2.nHour > currenttime.GetHour() || (it2.nHour == currenttime.GetHour() && it2.nMinute > currenttime.GetMinute()))
			{
				if (nMinFoundHour > it2.nHour)
				{
					nMinFoundHour = it2.nHour;
					CTime starttime(currenttime.GetYear(), currenttime.GetMonth(), currenttime.GetDay(), it2.nHour, it2.nMinute + m_nAnnouncementTime, 0);
					dTimet = (time_t)(starttime.GetTime() - time_null());
				}
			}
		}
	}

	return dTimet;
}
void CFFAManager::SetNextStateTime(int nMinutes, int nSeconds)
{
	CTime currenttime = CTime::GetCurrentTime();
	currenttime += CTimeSpan(0, 0, nMinutes, nSeconds);
	m_timeNextStateTime = (time_t)(currenttime.GetTime());
}
BOOL CFFAManager::IsNextStateTime()
{
	CTime currenttime = CTime::GetCurrentTime();
	return m_timeNextStateTime < currenttime.GetTime();
}
BOOL CFFAManager::JoinFFAWorld(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return FALSE;

	if (pUser->GetWorld() && pUser->GetWorld()->IsFFA())
		return FALSE;

	if (GetCurrentState() != STATE_PREPARE_BATTLE && GetCurrentState() != STATE_PROCESS_BATTLE)
		return FALSE;

	auto it = mapPlayers.find(pUser->m_idPlayer);
	if (it != mapPlayers.end())
		it->second.nRespawn = m_nRespawnTime;
	else
		return FALSE;

	TeleportToSpawn(pUser);
	pUser->AddFFAState(TRUE);

	return TRUE;
}
void CFFAManager::TeleportToSpawn(CUser* pUser)
{
	int nRandomX = 1742 + rand() % (1774 - 1742);
	int nRandomZ = 1750 + rand() % (1770 - 1750);
	pUser->REPLACE(g_uIdofMulti, WI_WORLD_FFA, D3DXVECTOR3(static_cast<float>(nRandomX), 100.0f, static_cast<float>(nRandomZ)), REPLACE_NORMAL, nDefaultLayer);
}
void CFFAManager::ProcessRespawn()
{
	CUser* pUser = NULL;
	CString str;
	int nTotalValidPlayers = 0;

	for (auto& it : mapPlayers)
	{
		pUser = (CUser*)prj.GetUserByID(it.first);
		if (!IsValidObj(pUser))
			continue;

		if (!pUser->GetWorld())
			continue;

		if (!pUser->GetWorld()->IsFFA())
			continue;

		if (it.second.nLifes <= 0)
			continue;

		nTotalValidPlayers++;

		if (it.second.nRespawn == -1)
			continue;

		if (it.second.nRespawn == 0)
		{
			int nRandomX = 1720 + rand() % (1790 - 1720);
			int nRandomZ = 1820 + rand() % (1900 - 1820);
			pUser->REPLACE(g_uIdofMulti, WI_WORLD_FFA, D3DXVECTOR3(static_cast<float>(nRandomX), 100.0f, static_cast<float>(nRandomZ)), REPLACE_NORMAL, nDefaultLayer);

			str.Format("Lifes left: %d", it.second.nLifes);
			pUser->AddText(str, 0xffffffff);
		}
		else
		{
			str.Format("Respawn in: %d", it.second.nRespawn);
			pUser->AddText(str, 0xffffffff);
		}

		it.second.nRespawn--;
	}

	if (nTotalValidPlayers < 2)
		EndBattle();
}
BOOL CFFAManager::IsFFAEnjoyer(CMover* pMover)
{
	if (!IsValidObj(pMover))
		return FALSE;

	if (!pMover->IsPlayer())
		return FALSE;

	if (!pMover->GetWorld() || !pMover->GetWorld()->IsFFA())
		return FALSE;

	auto it = mapPlayers.find(pMover->m_idPlayer);
	if (it == mapPlayers.end())
		return FALSE;

	if (GetCurrentState() != STATE_PROCESS_BATTLE)
		return FALSE;

	if (it->second.nLifes <= 0)
		return FALSE;

	if (it->second.nRespawn != -1)
		return FALSE;

	return TRUE;

}
BOOL CFFAManager::Kill(CMover* pAttacker, CMover* pDefender)
{
	if (!IsFFAEnjoyer(pAttacker) || !IsFFAEnjoyer(pDefender))
		return FALSE;

	auto itAttacker = mapPlayers.find(pAttacker->m_idPlayer);
	auto itDefender = mapPlayers.find(pDefender->m_idPlayer);

	itAttacker->second.nKils++;

	itDefender->second.nLifes--;
	itDefender->second.nRespawn = m_nRespawnTime;

	for (auto& it : mapPlayers)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (!IsValidObj(pUser))
			continue;

		pUser->AddFFAKill(pAttacker->m_idPlayer, pDefender->m_idPlayer);
	}

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pAttacker, RecordType::FFA_KILLS, 1, true);
#endif

	return TRUE;
}
#endif