#pragma once

#ifdef __FL_BATTLE_PASS
struct BATTLE_PASS_LEVELS
{
	int nExp;
	DWORD dwItemID;
	int nItemNum;
	BOOL bBinds;
};

enum	eAddExpType
{
	EXP_MOB = 0, EXP_BOSS,
	EXP_GW_KILL, EXP_GW_MVP, EXP_GW,
	EXP_SR,
	LEVEL_BATTLE_PASS_BUNDLE_1, LEVEL_BATTLE_PASS_BUNDLE_2, LEVEL_BATTLE_PASS_BUNDLE_3
};

class CBattlePass
{
private:
	int nBattlePassID;
	BOOL bActiveTime;
	BOOL bRequiredItem;
	CTime tEnd;
	int nMaxBattlePassLevel;
	int nExpPerMonster, nExpPerBoss, nExpGW_Kill, nExpGW_MvP, nExpGW_Win, nExpSR_Win, nLvlBundle_1, nLvlBundle_2, nLvlBundle_3;

	// Battle Pass Levels
	map<int, BATTLE_PASS_LEVELS> mapBattlePass;

#if defined (__CLIENT)
	CTimeSpan		m_ct;
	time_t    		m_tEndTime;
	time_t    		m_tCurrentTime;
#elif defined (__WORLDSERVER)
	void PayReward(CUser* pUser, BOOL bAddText);
#endif

public:
	CBattlePass();
	~CBattlePass();
	static CBattlePass* GetInstance();

	void Process();
	void ReadConfig(const LPCTSTR strFile);

	int GetBattlePassID() const { return nBattlePassID; }
	int GetMaxBattlePassLevel() const { return nMaxBattlePassLevel; }
	BOOL IsNotFree() const { return bRequiredItem; }
	CTime GetTimeEnd() { return tEnd; };

	int	GetExpForThisLevel(const int nLevel);
	DWORD GetRewardIdForThisLevel(const int nLevel);
	int	GetRewardNumForThisLevel(const int nLevel);
	BOOL GetRewardBindsForThisLevel(const int nLevel);

	int GetExpPerMonster() const { return nExpPerMonster; }
	int GetExpPerBoss() const { return nExpPerBoss; }
	int GetExpGW_Kill() const { return nExpGW_Kill; }
	int GetExpGW_MvP() const { return nExpGW_MvP; }
	int GetExpGW_Win() const { return nExpGW_Win; }
	int GetExpSR_Win() const { return nExpSR_Win; }
	int GetLvlBundle_1() const { return nLvlBundle_1; }
	int GetLvlBundle_2() const { return nLvlBundle_2; }
	int GetLvlBundle_3() const { return nLvlBundle_3; }

#if defined (__CLIENT)
	CTimeSpan	 GetTimeSpan() { return m_ct; };
	time_t		 GetCurrentTimeT() { return m_tCurrentTime; };
	void	 	 SetTime(time_t tTime) { m_tCurrentTime = 0; m_tEndTime = time_null() + tTime; }
#elif defined (__WORLDSERVER)
	BOOL CheckRequirments(CUser* pUser);
	BOOL CheckRemoveTicket(CUser* pUser);
	void AddProgress(CUser* pUser, const int nType);
#endif
};
#endif