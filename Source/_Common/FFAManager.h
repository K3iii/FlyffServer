#include "StdAfx.h"

#ifdef __FL_FFA
class CFFAManager
{
private:
	// Static config
	BOOL m_bValidConfig;
	int m_nJoinCost;
	int m_nJoinLevel;
	int m_nMinMembers;
	int m_nMaxMembers;
	int m_nLifes;
	int m_nAnnouncementTime;
	int m_nRespawnTime;
	int m_nPrepareTime;
	int m_nWarTime;
	DWORD m_dwRewardID;
	int m_nRewardAmount;

	struct FFA_TIME
	{
		int nHour;
		int nMinute;
	};
	map<int, vector<FFA_TIME>> mapTimes;

	// Dynamic config
	BYTE m_nCurrentState;
	time_t m_timeNextStateTime;

	struct FFA_PLAYER
	{
		int nKils;
		int nLifes;
		DWORD nRespawn;
	};
	map<u_long, FFA_PLAYER> mapPlayers;

public:
	CFFAManager();
	~CFFAManager();
	static CFFAManager* GetInstance();

	void ReadConfig();
	void Process();
	void AnnounceFFA();
	void OpenFFA();
	void StartBattle();
	void EndBattle();
	void CloseFFA();
	void ProcessRespawn();

	BOOL IsRegisteredPlayer(CUser* pUser);
	void RegisterPlayer(CUser* pUser);
	BOOL JoinFFAWorld(CUser* pUser);
	void TeleportToSpawn(CUser* pUser);
	BOOL IsFFAEnjoyer(CMover* pMover);
	BOOL Kill(CMover* pAttacker, CMover* pDefender);

	u_long GetPlayerCount() { return mapPlayers.size(); };
	int GetMaxPlayers() { return m_nMaxMembers; };
	int GetJoinFee() { return m_nJoinCost; };
	int GetMaxLifes() { return m_nLifes; };
	int GetRewardAmount() { return m_nRewardAmount; };
	DWORD GetRewardID() { return m_dwRewardID; };

	time_t GetNextFFA();
	BYTE GetCurrentState() { return m_nCurrentState; };
	void SetNextStateTime(int nMinutes, int nSeconds);
	time_t GetNextStateTime() { return m_timeNextStateTime; };
	BOOL IsNextStateTime();

	map<u_long, FFA_PLAYER>* GetPlayerList()
	{
		return &mapPlayers;
	}
};
#endif