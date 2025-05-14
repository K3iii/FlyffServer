#include "StdAfx.h"

#ifdef __FL_TDM
class CTDMManager
{
	bool m_bValidConfig;
	int m_nJoinCost;
	int m_nJoinLevel;
	int m_nMinMembers;
	int m_nMaxMembers;
	int m_nLifes;
	int m_nAnnouncementTime;
	int m_nRespawnTime;
	int m_nPrepareTime;
	int m_nWarTime;
	DWORD m_dwRewardTeamWinner;
	int m_nRewardCountTeamWinner;
	DWORD m_dwRewardBestPlayer;
	int m_nRewardCountBestPlayer;

	struct TDM_TIME
	{
		int nHour;
		int nMinute;

		TDM_TIME()
		{
			nHour = 0;
			nMinute = 0;
		}
	};

	unordered_map<int, vector<TDM_TIME>> m_mapTimes;

	struct TDM_PLAYER
	{
		TDM_TEAMS team;
		int nPoints;
		int nLifes;
		int nRespawn;

		TDM_PLAYER()
		{
			team = TDM_TEAM_NONE;
			nPoints = 0;
			nLifes = 0;
			nRespawn = 0;
		}
	};

	vector<u_long> m_vecRegisteredPlayers;
	unordered_map<u_long, TDM_PLAYER> m_mapPlayers;
	unordered_map<TDM_TEAMS, int> m_mapTeams;

	BYTE m_nCurrentState;
	time_t m_timeNextStateTime;

public:
	CTDMManager();
	~CTDMManager();
	static CTDMManager* GetInstance();

	void ReadConfig();
	void Process();
	void AnnounceTDM();
	void OpenTDM();
	void StartBattle();
	void ProcessRespawn();
	void EndBattle();
	void CloseTDM();

	bool IsRegisteredPlayer(CUser* pUser);
	void RegisterPlayer(CUser* pUser);
	bool JoinTDMWorld(CUser* pUser);
	void TeleportToSpawn(CUser* pUser);
	bool IsCanKill(CMover* pAttacker, CMover* pDefender);
	bool Kill(CMover* pAttacker, CMover* pDefender);

	size_t GetRegisteredPlayersCount() { return m_vecRegisteredPlayers.size(); };
	int GetMaxPlayers() { return m_nMaxMembers; };
	int GetJoinFee() { return m_nJoinCost; };
	int GetMaxLifes() { return m_nLifes; };

	time_t GetNextTDM();
	BYTE GetCurrentState() { return m_nCurrentState; };
	void SetNextStateTime(int nMinutes, int nSeconds);
	time_t GetNextStateTime() { return m_timeNextStateTime; };
	bool IsNextStateTime();

	void ResetTeamsPoints();

	vector<u_long>* GetRegisteredPlayerList()
	{
		return &m_vecRegisteredPlayers;
	}
	unordered_map<u_long, TDM_PLAYER>* GetPlayerList()
	{
		return &m_mapPlayers;
	}
	unordered_map<TDM_TEAMS, int>* GetTeamList()
	{
		return &m_mapTeams;
	}
};
#endif