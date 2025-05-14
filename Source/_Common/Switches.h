#pragma once

#ifdef __FL_SWITCH_EQ

#define MAX_SWITCHES 15

struct EQ_SWITCH
{
	char szSwitchName[32];
	DWORD dwItem[MAX_HUMAN_PARTS];
	DWORD dwLooterPet;
	DWORD dwPet;
	DWORD dwRegen;

	EQ_SWITCH() { Init(); };
	~EQ_SWITCH() {};

	void Init()
	{
		szSwitchName[0] = '\0';
		memset(dwItem, 0xff, sizeof(dwItem));
		dwLooterPet = NULL_ID;
		dwPet = NULL_ID;
		dwRegen = NULL_ID;
	}
};

class CSwitches
{
	CMover* m_pMover;
	vector<EQ_SWITCH> m_vecSwitches;

public:
	CSwitches(CMover* pMover);
	~CSwitches();

	vector<EQ_SWITCH>* GetSwitches() { return &m_vecSwitches; };
	void Serialize(CAr& ar);
	int GetTotalSwitches() { return (int)m_vecSwitches.size(); };
	BOOL AddSwitch(int nSelected, LPCTSTR szText, DWORD dwLooter, DWORD dwPet, DWORD dwRegen, BOOL bAdd);
	BOOL DeleteSwitch(int nSel);
#ifdef __WORLDSERVER
	void UseSwitch(int nSel);
	int GetRequiredEmptySpace(EQ_SWITCH* pSwitch);
#endif
	void FindAndRemove(DWORD dwObjId);
	void ClearAllSwitches();
};

#endif
