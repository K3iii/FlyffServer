#ifndef __WHEELOFFORTUNE_H__
#define	__WHEELOFFORTUNE_H__

#ifdef __FL_WHEEL_OF_FORTUNE
typedef	struct	_WOF_ITEM
{
	DWORD	dwItemId;
	int nItemCount;
	bool bBinded;
	DWORD dwChance;
	DWORD dwColor;
#ifdef __WORLDSERVER
	DWORD	dwProb;
#else
	CTexture* pTexture;
#endif

	_WOF_ITEM()
	{
		dwItemId	= 0;
		nItemCount = 0;
		bBinded = false;
		dwChance = 0;
		dwColor = 0;
#ifdef __WORLDSERVER
		dwProb	= 0;
#else
		pTexture = nullptr;
#endif
	}
}	WOF_ITEM;

class CWheelOfFortune
{
	const DWORD MAX_CHANCE = 1000000000;

	bool m_bActive;
	DWORD m_dwTicket;
	int m_nTicketCount;
	DWORD m_dwMostRareChance;
	vector<WOF_ITEM> m_vecItem;
#ifdef __WORLDSERVER
	map<u_long, WOF_ITEM> m_mapParticipants;
#else
	deque<WOF_ITEM> m_listRecentRewards;
	CTexture* m_pTicketTexture;
#endif

	enum { COLOR_DEFAULT, COLOR_YELLOW, COLOR_ORANGE, COLOR_BLUE, COLOR_GREEN, COLOR_VIOLET, COLOR_MAX};

public:
	CWheelOfFortune();
	virtual ~CWheelOfFortune();
	static CWheelOfFortune*	GetInstance();

#ifdef __WORLDSERVER
	bool LoadScript(BOOL bWithNotify);
#endif
	void Serialize(CAr& ar);

	bool IsActive() { return m_bActive; }
	DWORD GetTicket() { return m_dwTicket; }
	int GetTicketCount() { return m_nTicketCount; }
	vector<WOF_ITEM>* GetItemList(void) { return &m_vecItem; }
#ifdef __WORLDSERVER
	int GetPrize();
	void InitProcess(CUser* pUser);
	void FinishProcess(CUser* pUser);
#else
	CTexture* GetTicketTexture() { return m_pTicketTexture; }
	void SetTicketTexture(CTexture* pTexture) { m_pTicketTexture = pTexture; }
	DWORD GetColor(bool bLowAlpha, int nColor);
	deque<WOF_ITEM>* GetRecentRewardsList(void) { return &m_listRecentRewards; }
	void AddRecentReward(const WOF_ITEM& item);
#endif

};
#endif

#endif	// __WHEELOFFORTUNE_H__