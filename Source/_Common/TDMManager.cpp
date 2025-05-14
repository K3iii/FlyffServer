#include "StdAfx.h"
#include <algorithm>
#include <random>

#include "TDMManager.h"

#include "User.h"
#include "DPDatabaseClient.h"
#include "DPCoreClient.h"
extern  CDPCoreClient g_DPCoreClient;
extern	CUserMng	g_UserMng;
extern CDPDatabaseClient g_dpDBClient;

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#ifdef __FL_TDM
CTDMManager::CTDMManager()
	: m_bValidConfig(false)
	, m_nJoinCost(0)
	, m_nJoinLevel(0)
	, m_nMinMembers(0)
	, m_nMaxMembers(0)
	, m_nLifes(0)
	, m_nAnnouncementTime(0)
	, m_nRespawnTime(-1)
	, m_nPrepareTime(0)
	, m_nWarTime(0)
	, m_dwRewardTeamWinner(NULL_ID)
	, m_nRewardCountTeamWinner(0)
	, m_dwRewardBestPlayer(NULL_ID)
	, m_nRewardCountBestPlayer(0)
	, m_nCurrentState(STATE_CHILL)
	, m_timeNextStateTime(0)
{
	ResetTeamsPoints();
}
CTDMManager::~CTDMManager()
{
	m_mapTimes.clear();
	m_vecRegisteredPlayers.clear();
	m_mapPlayers.clear();
	m_mapTeams.clear();
}
CTDMManager* CTDMManager::GetInstance()
{
	static CTDMManager sBP;
	return &sBP;
}
void CTDMManager::ReadConfig()
{
	CScript s;
	if (!s.Load("TDMManager.inc"))
	{
		Error("CTDMManager::ReadConfig cant load TDMManager.inc");
		return;
	}

	if (m_nCurrentState != STATE_CHILL)
	{
		Error("CTDMManager::ReadConfig live refresh only possible when STATE_CHILL");
		return;
	}

	m_bValidConfig = false;
	m_mapTimes.clear();

	s.GetToken();
	while (s.tok != FINISHED)
	{
		if (s.Token == "TDM_COST")
		{
			m_nJoinCost = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_MIN_LEVEL")
		{
			m_nJoinLevel = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_MIN_MEMBERS")
		{
			m_nMinMembers = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_MAX_MEMBERS")
		{
			m_nMaxMembers = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_LIFES")
		{
			m_nLifes = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_ANNOUNCE_TIME")
		{
			m_nAnnouncementTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_RESPAWN_TIME")
		{
			m_nRespawnTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_PREPARATION_TIME")
		{
			m_nPrepareTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_WAR_TIME")
		{
			m_nWarTime = s.GetNumber();
			s.GetToken();
		}
		if (s.Token == "TDM_REWARD_TEAM_WINNER")
		{
			m_dwRewardTeamWinner = (DWORD)s.GetNumber();
			m_nRewardCountTeamWinner = s.GetNumber();

			ItemProp* pProp = prj.GetItemProp(m_dwRewardTeamWinner);
			if (!pProp)
			{
				Error("Your TDM config is fucked up, buddy (Invalid team reward ID)");
				return;
			}

			if (m_nRewardCountTeamWinner < 1 || m_nRewardCountTeamWinner > pProp->dwPackMax)
			{
				Error("Your TDM config is fucked up, buddy (Invalid team reward amount)");
				return;
			}

			s.GetToken();
		}
		if (s.Token == "TDM_REWARD_BEST_PLAYER")
		{
			m_dwRewardBestPlayer = (DWORD)s.GetNumber();
			m_nRewardCountBestPlayer = s.GetNumber();

			ItemProp* pProp = prj.GetItemProp(m_dwRewardBestPlayer);
			if (!pProp)
			{
				Error("Your TDM config is fucked up, buddy (Invalid player reward ID)");
				return;
			}

			if (m_nRewardCountBestPlayer < 1 || m_nRewardCountBestPlayer > pProp->dwPackMax)
			{
				Error("Your TDM config is fucked up, buddy (Invalid player reward amount)");
				return;
			}

			s.GetToken();
		}
		if (s.Token == "TDM_BEGIN")
		{
			int nTmpDay = s.GetNumber();
			if (nTmpDay < 1 || nTmpDay > 7)
			{
				Error("Your TDM config is fucked up, buddy (Day: %d)", nTmpDay);
				return;
			}
			int nTmpHour = s.GetNumber();
			if (nTmpHour < 0 || nTmpHour > 23)
			{
				Error("Your TDM config is fucked up, buddy (Hour: %d)", nTmpHour);
				return;
			}
			int nTmpMinute = s.GetNumber();
			if (nTmpMinute < 0 || nTmpMinute > 59)
			{
				Error("Your TDM config is fucked up, buddy (Minute: %d)", nTmpMinute);
				return;
			}

			nTmpDay = nTmpDay + 1;
			if (nTmpDay == 8)
				nTmpDay = 1;

			TDM_TIME tmpTimes;
			tmpTimes.nHour = nTmpHour;
			tmpTimes.nMinute = nTmpMinute;

			auto it = m_mapTimes.find(nTmpDay);
			if (it != m_mapTimes.end())
				it->second.push_back(tmpTimes);
			else
			{
				vector<TDM_TIME> vecTmp;
				vecTmp.push_back(tmpTimes);
				m_mapTimes.insert(make_pair(nTmpDay, vecTmp));
			}

			s.GetToken();
		}
	}

	m_bValidConfig = true;
}
void CTDMManager::Process()
{
	if (!m_bValidConfig)
		return;

	if (g_uIdofMulti != 1) // 1 channel only
		return;

	if (m_nCurrentState == STATE_CHILL)
	{
		CTime ctime = CTime::GetCurrentTime();
		int nDayofWeek = ctime.GetDayOfWeek();

		auto it = m_mapTimes.find(nDayofWeek);
		if (it != m_mapTimes.end())
		{
			for (auto& it2 : it->second)
			{
				if (it2.nHour == ctime.GetHour() && it2.nMinute == ctime.GetMinute())
				{
					AnnounceTDM();
					break;
				}
			}
		}
	}
	else if (m_nCurrentState == STATE_ANNOUNCE)
	{
		if (IsNextStateTime())
			OpenTDM();
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
			CloseTDM();
	}
}
void CTDMManager::AnnounceTDM()
{
	if (m_nCurrentState != STATE_CHILL)
		return;

	m_nCurrentState = STATE_ANNOUNCE;
	SetNextStateTime(m_nAnnouncementTime, 0);

	CString str;
	str.Format("Team deathmatch will start in %d minute(s). Register now!", m_nAnnouncementTime);
	g_DPCoreClient.SendSystem(str);
}
void CTDMManager::OpenTDM()
{
	if (m_nCurrentState != STATE_ANNOUNCE)
		return;

	if (GetRegisteredPlayersCount() < m_nMinMembers)
	{
		CString str;
		str.Format("Team deathmatch not started because amount of participants wasn't enough.");
		g_DPCoreClient.SendSystem(str);

		m_nCurrentState = STATE_CHILL;
		return;
	}

	m_nCurrentState = STATE_PREPARE_BATTLE;
	SetNextStateTime(m_nPrepareTime, 0);

	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(m_vecRegisteredPlayers.begin(), m_vecRegisteredPlayers.end(), g);

	TDM_PLAYER tmpPlayer;
	tmpPlayer.team = TDM_TEAM_RED;
	tmpPlayer.nLifes = m_nLifes;

	for (auto& it : m_vecRegisteredPlayers)
	{
		m_mapPlayers.insert(make_pair(it, tmpPlayer));
		tmpPlayer.team = tmpPlayer.team == TDM_TEAM_RED ? TDM_TEAM_BLUE : TDM_TEAM_RED;
	}

	for (auto& it : m_mapPlayers)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (!IsValidObj(pUser))
			continue;

		JoinTDMWorld(pUser);
	}

	CString str;
	str.Format("Team deathmatch will begin in %d minute(s). All registered players can join now!", m_nPrepareTime);
	g_DPCoreClient.SendSystem(str);
}
bool CTDMManager::JoinTDMWorld(CUser* pUser)
{
	if (m_nCurrentState != STATE_PREPARE_BATTLE && m_nCurrentState != STATE_PROCESS_BATTLE)
		return false;

	if (!IsValidObj(pUser))
		return false;

	if (pUser->GetSummonState(FALSE) != 0)
	{
		pUser->AddDiagText(prj.GetText(TID_TDM_CANNOTJOIN));
		return false;
	}

	auto it = m_mapPlayers.find(pUser->m_idPlayer);
	if (it != m_mapPlayers.end())
		it->second.nRespawn = m_nRespawnTime;
	else
		return false;

	TeleportToSpawn(pUser);
	pUser->AddTDMState(true);

	return true;
}
void CTDMManager::TeleportToSpawn(CUser* pUser)
{
	int nRandomX = 1189 + rand() % (1213 - 1189);
	int nRandomZ = 1360 + rand() % (1382 - 1360);
	pUser->REPLACE(g_uIdofMulti, WI_WORLD_TDM, D3DXVECTOR3(static_cast<float>(nRandomX), 100.0f, static_cast<float>(nRandomZ)), REPLACE_FORCE, nDefaultLayer);
}

void CTDMManager::StartBattle()
{
	if (m_nCurrentState != STATE_PREPARE_BATTLE)
		return;

	m_nCurrentState = STATE_PROCESS_BATTLE;
	SetNextStateTime(m_nWarTime, 0);

	for (auto& it : m_mapPlayers)
	{
		it.second.nRespawn = m_nRespawnTime;

		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
			pUser->AddTDMState(false);
	}

	CString str;
	str.Format("Team deathmatch started. It will be finished in %d minute(s). Enjoy the battle!", m_nWarTime);
	g_DPCoreClient.SendSystem(str);
}
void CTDMManager::EndBattle()
{
	if (m_nCurrentState != STATE_PROCESS_BATTLE)
		return;

	m_nCurrentState = STATE_PREPARE_CLOSE;
	SetNextStateTime(0, 15);

	// Find best team
	TDM_TEAMS teamWinner = TDM_TEAM_NONE;
	int nMaximumTeamPoints = 0;

	for (auto& it : m_mapTeams)
	{
		if (it.second > nMaximumTeamPoints)
		{
			teamWinner = it.first;
			nMaximumTeamPoints = it.second;
		}
	}

	// Add tdm state to all players, find best player and give rewards to members of best team
	u_long idBestPlayer = NULL_ID;
	int nMaximumPlayerPoints = 0;

	for (auto& it : m_mapPlayers)
	{
		// Find best player
		if (it.second.nPoints > nMaximumPlayerPoints)
		{
			idBestPlayer = it.first;
			nMaximumPlayerPoints = it.second.nPoints;
		}

		// Add tdm state
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
			pUser->AddTDMState(false);

		// Give reward if guy is part of best team and have more than 0 points
		if (it.second.team == teamWinner && it.second.nPoints > 0)
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = m_dwRewardTeamWinner;
			itemElem.m_nItemNum = m_nRewardCountTeamWinner;
			itemElem.SetSerialNumber();

			if (IsValidObj(pUser))
			{
				if (!pUser->CreateItem(&itemElem))
					g_dpDBClient.SendQueryPostMail(it.first, 0, itemElem, 0, "TDM Reward", "Here is your reward for the TDM battle!");
			}
			else
				g_dpDBClient.SendQueryPostMail(it.first, 0, itemElem, 0, "TDM Reward", "Here is your reward for the TDM battle!");
		}
	}

	// Give reward to best player
	if (idBestPlayer != NULL_ID)
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = m_dwRewardBestPlayer;
		itemElem.m_nItemNum = m_nRewardCountBestPlayer;
		itemElem.SetSerialNumber();

		CUser* pUserMVP = (CUser*)prj.GetUserByID(idBestPlayer);
		if (IsValidObj(pUserMVP))
		{
			if (!pUserMVP->CreateItem(&itemElem))
				g_dpDBClient.SendQueryPostMail(idBestPlayer, 0, itemElem, 0, "TDM Reward", "Here is your reward for the mvp in TDM battle!");
		}
		else
			g_dpDBClient.SendQueryPostMail(idBestPlayer, 0, itemElem, 0, "TDM Reward", "Here is your reward for the mvp in TDM battle!");
	}

	// Announce everything
	CString strMain;
	if (teamWinner == TDM_TEAM_NONE)
		strMain.Format("Team deathmatch finished.");
	else
		strMain.Format("Team %s won the battle with %d points!", teamWinner == TDM_TEAM_RED ? "Red" : "Blue", nMaximumTeamPoints);
	g_DPCoreClient.SendSystem(strMain);

	strMain = "";
	if (idBestPlayer != NULL_ID)
	{
		CUser* pUserMVP = (CUser*)prj.GetUserByID(idBestPlayer);
		if (IsValidObj(pUserMVP))
			strMain.Format("%s had best result in this battle (%d points).", pUserMVP->GetName(), nMaximumPlayerPoints);
		else
		{
			const char* lpszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idBestPlayer);
			if (lpszPlayer)
				strMain.Format("%s had best result in this battle (%d points).", lpszPlayer, nMaximumPlayerPoints);
		}

		if (!strMain.IsEmpty())
			g_DPCoreClient.SendSystem(strMain);
	}
}
void CTDMManager::CloseTDM()
{
	if (m_nCurrentState != STATE_PREPARE_CLOSE)
		return;

	m_nCurrentState = STATE_CHILL;
	m_timeNextStateTime = 0;

	for (auto& it : m_mapPlayers)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (IsValidObj(pUser))
		{
			pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_TDM_CLEAR);

			if (pUser->GetWorld() && pUser->GetWorld()->IsTDM())
				pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 100.0f, 3328.8f), REPLACE_FORCE, nDefaultLayer);
		}
	}

	m_vecRegisteredPlayers.clear();
	m_mapPlayers.clear();
	ResetTeamsPoints();

	CString str;
	str.Format("Team deathmatch closed.");
	g_DPCoreClient.SendSystem(str);
}

bool CTDMManager::IsRegisteredPlayer(CUser* pUser)
{
	if (!IsValidObj(pUser))
		return false;

	for (auto& it : m_vecRegisteredPlayers)
	{
		if (it == pUser->m_idPlayer)
			return true;
	}

	return false;
}
void CTDMManager::RegisterPlayer(CUser* pUser)
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

	if (GetRegisteredPlayersCount() >= m_nMaxMembers)
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

	m_vecRegisteredPlayers.push_back(pUser->m_idPlayer);

	pUser->AddText("You successfully registered!", 0xffffffff);
	pUser->AddTDMData();

	for (auto& it : m_vecRegisteredPlayers)
	{
		CUser* pUser2 = (CUser*)prj.GetUserByID(it);
		if (IsValidObj(pUser2) && pUser2 != pUser)
			pUser2->AddTDMRegister(pUser);
	}
}
time_t CTDMManager::GetNextTDM()
{
	if (!m_bValidConfig)
		return 0;

	CTime currenttime = CTime::GetCurrentTime();
	int nDayofWeek = currenttime.GetDayOfWeek();
	time_t dTimet = 0;

	auto it = m_mapTimes.find(nDayofWeek);
	if (it != m_mapTimes.end())
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
void CTDMManager::SetNextStateTime(int nMinutes, int nSeconds)
{
	CTime currenttime = CTime::GetCurrentTime();
	currenttime += CTimeSpan(0, 0, nMinutes, nSeconds);
	m_timeNextStateTime = (time_t)(currenttime.GetTime());
}
bool CTDMManager::IsNextStateTime()
{
	CTime currenttime = CTime::GetCurrentTime();
	return m_timeNextStateTime < currenttime.GetTime();
}
void CTDMManager::ProcessRespawn()
{
	CUser* pUser = nullptr;
	CString str;
	int nTotalValidPlayersRED = 0;
	int nTotalValidPlayersBLUE = 0;

	for (auto& it : m_mapPlayers)
	{
		pUser = (CUser*)prj.GetUserByID(it.first);
		if (!IsValidObj(pUser))
			continue;

		if (!pUser->GetWorld())
			continue;

		if (!pUser->GetWorld()->IsTDM())
			continue;

		if (it.second.nLifes <= 0)
			continue;

		if (it.second.team == TDM_TEAM_RED)
			nTotalValidPlayersRED++;
		else
			nTotalValidPlayersBLUE++;

		if (it.second.nRespawn == -1)
			continue;

		if (it.second.nRespawn == 0)
		{
			int nRandomX = 1209 + rand() % (1271 - 1209);
			int nRandomZ = 1240 + rand() % (1300 - 1240);
			pUser->REPLACE(g_uIdofMulti, WI_WORLD_TDM, D3DXVECTOR3(static_cast<float>(nRandomX), 100.0f, static_cast<float>(nRandomZ)), REPLACE_FORCE, nDefaultLayer);

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

	if (nTotalValidPlayersRED < 1 || nTotalValidPlayersBLUE < 1)
		EndBattle();
}
bool CTDMManager::IsCanKill(CMover* pAttacker, CMover* pDefender)
{
	if (GetCurrentState() != STATE_PROCESS_BATTLE)
		return false;

	if (!IsValidObj(pAttacker) || !IsValidObj(pDefender))
		return false;

	if (!pAttacker->IsPlayer() || !pDefender->IsPlayer())
		return false;

	if (!pAttacker->GetWorld() || !pDefender->GetWorld() || !pAttacker->GetWorld()->IsTDM() || !pDefender->GetWorld()->IsTDM())
		return false;

	auto itAttacker = m_mapPlayers.find(pAttacker->m_idPlayer);
	auto itDefender = m_mapPlayers.find(pDefender->m_idPlayer);
	if (itAttacker == m_mapPlayers.end() || itDefender == m_mapPlayers.end())
		return false;

	if (itAttacker->second.team == itDefender->second.team)
		return false;

	if (itAttacker->second.nLifes <= 0 || itDefender->second.nLifes <= 0)
		return false;

	if (itAttacker->second.nRespawn != -1 || itDefender->second.nRespawn != -1)
		return false;

	return true;

}
bool CTDMManager::Kill(CMover* pAttacker, CMover* pDefender)
{
	if (!IsCanKill(pAttacker, pDefender))
		return false;

	auto itAttacker = m_mapPlayers.find(pAttacker->m_idPlayer);
	auto itDefender = m_mapPlayers.find(pDefender->m_idPlayer);

	itAttacker->second.nPoints++;
	m_mapTeams[itAttacker->second.team]++;

	itDefender->second.nLifes--;
	itDefender->second.nRespawn = m_nRespawnTime;

	for (auto& it : m_mapPlayers)
	{
		CUser* pUser = (CUser*)prj.GetUserByID(it.first);
		if (!IsValidObj(pUser))
			continue;

		pUser->AddTDMKill(pAttacker->m_idPlayer, pDefender->m_idPlayer);
	}

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pAttacker, RecordType::TDM_KILLS, 1, true);
#endif

	return true;
}
void CTDMManager::ResetTeamsPoints()
{
	for (int i = 1; i < TDM_TEAM_MAX; i++)
		m_mapTeams[(TDM_TEAMS)i] = 0;
}
#endif