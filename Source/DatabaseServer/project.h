#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "ProjectCmn.h"

#include <vector>
#include <map>
#include "Mymap.h"
#if __VER >= 9 // __EVENTLUA
#include "EventLua.h"
#endif // __EVENTLUA
using namespace std;



typedef map< string, void* > CMapStrToPtr;
#define	MAX_WORLD	256


#ifdef __S1108_BACK_END_SYSTEM
#define MAX_RESPAWN 1024
#define MAX_MONSTER_PROP 1024
typedef struct	_MONSTER_RESPAWN
{
	char	szMonsterName[32];
	int		nRespawnIndex;
	D3DXVECTOR3	vPos;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;
	_MONSTER_RESPAWN()
	{
		szMonsterName[0] = '\0';
		vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		nQuantity = 0;
		nAggressive = 0;
		nRange = 0;
		nTime = 0;
		nRespawnIndex = 0;
	}
}
MONSTER_RESPAWN, * PMONSTER_RESPAWN;

typedef struct _MONSTER_PROP
{
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;
	_MONSTER_PROP()
	{
		szMonsterName[0] = '\0';
		nHitPoint = 0;
		nAttackPower = 0;
		nDefence = 0;
		nExp = 0;
		nItemDrop = 0;
		nPenya = 0;
	}
}
MONSTER_PROP, * PMONSTER_PROP;
#endif // __S1108_BACK_END_SYSTEM

class CScript;

class CProject
{
public:
	CProject();
	virtual	~CProject();

private:
	map<string, DWORD>	m_mapII;

public:
	ItemProp* GetItemProp(LPCTSTR lpszItem);
	CFixedArray<ItemProp>	m_aPropSkill;
	CFixedArray<ItemProp>	m_aPropItem;
	int						m_nMoverPropSize;
	MoverProp* m_pPropMover;
	CFixedArray< tagColorText >	m_colorText;

#if __VER >= 9 // __EVENTLUA
	CEventLua	m_EventLua;
#endif // __EVENTLUA

#ifdef __CONV_SKILL_11_MONTH_JOB1
	CFixedArray< AddSkillProp > m_aPropAddSkill;
	DWORD        m_aExpSkill[MAX_EXPSKILL];
#endif // __CONV_SKILL_11_MONTH_JOB1

	ItemProp* m_aJobSkill[MAX_JOB][40];
	DWORD	m_aJobSkillNum[MAX_JOB];
	EXPPARTY	m_aExpParty[MAX_PARTYLEVEL];
	EXPCHARACTER m_aExpCharacter[MAX_EXPCHARACTER];

	FLOAT m_fItemDropRate;
	FLOAT m_fGoldDropRate;
	FLOAT m_fMonsterExpRate;
	FLOAT m_fMonsterHitRate;
	FLOAT m_fShopCost;

#ifdef __S1108_BACK_END_SYSTEM
	FLOAT m_fMonsterRebirthRate;
	FLOAT m_fMonsterHitpointRate;
	FLOAT m_fMonsterAggressiveRate;
	FLOAT m_fMonsterRespawnRate;
	BOOL  m_bBaseGameSetting;
	BOOL  m_bBackEndSystem;
	MONSTER_RESPAWN	m_aMonsterRespawn[MAX_RESPAWN];		// Respawn
	MONSTER_RESPAWN n_aAddRespawn[MAX_RESPAWN];
	int				m_nAddMonsterRespawnSize;
	UINT			m_nMonsterRespawnSize;			// Respawn Size
	MONSTER_PROP	m_aMonsterProp[MAX_MONSTER_PROP];	// Monster Prop
	MONSTER_PROP	m_aAddProp[MAX_MONSTER_PROP];	// Monster Prop
	char			m_aRemoveProp[MAX_MONSTER_PROP][32];	// Monster Prop
	int				m_nAddMonsterPropSize;
	int				m_nRemoveMonsterPropSize;
	UINT			m_nMonsterPropSize;		// Monster Prop Size
	char			m_chBackEndSystemTime[15];
	char			m_chBackEndSystemChatTime[15];
	char			m_chGMChat[10][256];
#endif // __S1108_BACK_END_SYSTEM

	TCHAR	m_apszWorld[MAX_WORLD][64];
	TCHAR	m_apszWorldName[MAX_WORLD][64];
	vector<BEGINITEM>	m_aryBeginItem;
	CMapStrToPtr	m_mapBeginPos;
	JOBITEM		m_jobItem[MAX_JOBITEM];
	//	DWORD	m_aExpJobLevel[MAX_EXPJOBLEVEL + 1];

#ifdef __RULE_0615
	set<char>	m_sAllowedLetterAuth;
	set<char>	m_sAllowedLetterGame;
	BOOL	LoadAllowedLetter(void);
	BOOL	IsAllowedLetter(LPCSTR szName, BOOL bAuth);
	void	Formalize(LPSTR szName);
#endif	// __RULE_0615

public:
	void	LoadDefines();
	void	LoadPreFiles();
	void	LoadStrings();
	BOOL	OpenProject(LPCTSTR lpszFileName);
	int		GetBeginItemSize(void);
	LPBEGINITEM	GetBeginItem(int nIndex);
	void	LoadBeginPos(void);
	void	AddBeginPos(const char* lpszWorld, const D3DXVECTOR3& vPos);
	BOOL	GetRandomBeginPos(DWORD dwWorldID, LPD3DXVECTOR3 pvOut);
	BOOL	LoadPropMover(LPCTSTR lpszFileName);
	BOOL	LoadPropItem(LPCTSTR lpszFileName, CFixedArray<ItemProp>* apObjProp);
	BOOL	LoadPropSkill(LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp);

	BOOL	LoadDefOfWorld(LPCTSTR lpszFileName);
	BOOL	LoadJobItem(LPCTSTR lpszFileName);
	BOOL	LoadText(LPCTSTR lpszFileName);
	LPCTSTR GetText(DWORD dwIndex)
	{
		if (m_colorText.GetAt(dwIndex) == NULL)
			return "";
		return m_colorText.GetAt(dwIndex)->lpszData;
	}
	DWORD GetTextColor(DWORD dwIndex)
	{
		if (m_colorText.GetAt(dwIndex) == NULL)
			return 0;
		return m_colorText.GetAt(dwIndex)->dwColor;
	}

	CString GetLangScript(CScript& script);

#if defined(__DBSERVER) || defined(__VPW)
	BOOL	LoadExpTable(LPCTSTR lpszFileName);
#endif	// __DBSERVER
	ItemProp* GetItemProp(int nIndex) { return m_aPropItem.GetAt(nIndex); }
	MoverProp* GetMoverProp(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_nMoverPropSize)
		{
			LPCTSTR szErr = Error("CProject::GetMoverProp out of range. %d %d", nIndex, m_nMoverPropSize);
			TRACE(szErr);
			return NULL;
		}
		if (m_pPropMover[nIndex].dwID)
			return m_pPropMover + nIndex;
		return NULL;
	}

	ItemProp* GetSkillProp(int nIndex)
	{
		if (nIndex < 0 && nIndex >= m_aPropSkill.GetSize())
			return NULL;
		return m_aPropSkill.GetAt(nIndex);
	}
};

inline LPBEGINITEM CProject::GetBeginItem(int nIndex)
{
	return (LPBEGINITEM)&m_aryBeginItem.at(nIndex);
}

inline int CProject::GetBeginItemSize(void)
{
	return m_aryBeginItem.size();
}

inline ItemProp* CProject::GetItemProp(LPCTSTR lpszItem)
{
	map<string, DWORD>::iterator i = m_mapII.find(lpszItem);
	if (i != m_mapII.end())
		return GetItemProp(i->second);
	return NULL;
}

#endif	// __PROJECT_H__