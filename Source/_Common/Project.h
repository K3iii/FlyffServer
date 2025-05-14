#if !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
#define AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "defineNeuz.h"
#include "ModelMng.h"
#include "TerrainMng.h"
#include "ObjMap.h"
#include "ProjectCmn.h"
#include "Script.h"
#include "guildquest.h"

#include "partyquest.h"
#include <set>
using	namespace	std;

#ifdef __EVE_MINIGAME
#include "MiniGame.h"
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
#include "UltimateWeapon.h"
#endif // __ULTIMATE

#ifdef __TRADESYS
#include "Exchange.h"
#endif // __TRADESYS

#if __VER >= 9 && defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
#include "EventLua.h"
#endif // __EVENTLUA && __WORLDSERVER

#ifdef __CLIENT
#include "MapInformationManager.h"
#endif // __CLIENT

#define		MAX_RESPAWN			1536
#define		MAX_MONSTER_PROP	1024
#define		MAX_GUILDAPPELL		5

extern CString GetLangFileName(int nLang, int nType);

#define MAX_QUESTPROPITEM  4
#define MAX_QUESTCONDITEM  64
#define MAX_QUESTREMOVE    12

struct QuestState
{
	TCHAR	m_szDesc[512];
};

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
struct QuestGoalData
{
	QuestGoalData(void);

	DWORD m_dwGoalIndex;
	FLOAT m_fGoalPositionX;
	FLOAT m_fGoalPositionZ;
	DWORD m_dwGoalTextID;
};
#endif // __IMPROVE_QUEST_INTERFACE

struct QuestPropItem
{
	char    m_nSex;
	char	m_nType;
	int     m_nJobOrItem;
	int		m_nItemIdx;
	int		m_nItemNum;
#ifdef __JEFF_11
	int		m_nAbilityOption;
#endif	//__JEFF_11
#if __VER >= 13 // __CHIPI_QUESTITEM_FLAG
	BYTE	m_byFlag;
#endif // __CHIPI_QUESTITEM_FLAG
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	QuestGoalData m_ItemGoalData;
#endif // __IMPROVE_QUEST_INTERFACE
};

struct QuestProp
{
	WORD	m_wId;
	TCHAR	m_szTitle[64];
	TCHAR	m_szNpcName[32];
	WORD	m_nHeadQuest;
	CHAR	m_nQuestType;
	bool	m_bNoRemove;

	int		m_nParam[4];
	char    m_nBeginCondPreviousQuestType;
	WORD	m_anBeginCondPreviousQuest[6];
	WORD	m_anBeginCondExclusiveQuest[6];
	char	m_nBeginCondJob[MAX_JOB];
	char	m_nBeginCondJobNum;
	BYTE	m_nBeginCondLevelMax;
	BYTE	m_nBeginCondLevelMin;
	char	m_nBeginCondParty;
	char 	m_nBeginCondPartyNumComp;
	char	m_nBeginCondPartyNum;
	char	m_nBeginCondPartyLeader;
	char	m_nBeginCondGuild;
	char 	m_nBeginCondGuildNumComp;
	WORD	m_nBeginCondGuildNum;
	char	m_nBeginCondGuildLeader;
	QuestPropItem* m_paBeginCondItem;
	char	m_nBeginCondItemNum;
	char	m_nBeginCondSex;
	int		m_nBeginCondSkillIdx;
	char	m_nBeginCondSkillLvl;
	int		m_nBeginCondDisguiseMoverIndex;
	int  	m_nBeginSetAddItemIdx[4];
	char	m_nBeginSetAddItemNum[4];
	int		m_nBeginSetAddGold;
	int     m_nBeginSetDisguiseMoverIndex;
	int		m_nBeginCondPetLevel;
	int		m_nBeginCondPetExp;
	int     m_nEndCondLimitTime;
	QuestPropItem* m_paEndCondItem;
	char	m_nEndCondItemNum;
	int     m_nEndCondKillNPCIdx[2];
	int     m_nEndCondKillNPCNum[2];
	QuestGoalData m_KillNPCGoalData[2];
	DWORD   m_dwEndCondPatrolWorld;
	DWORD	m_dwPatrolDestinationID;
	QuestGoalData m_PatrolWorldGoalData;
	CRect   m_rectEndCondPatrol;
	CHAR    m_szEndCondCharacter[64];
	QuestGoalData m_MeetCharacterGoalData;
	CHAR* m_lpszEndCondMultiCharacter;
	int		m_nEndCondSkillIdx;
	char	m_nEndCondSkillLvl;
	int		m_nEndCondGold;
	BYTE	m_nEndCondLevelMin;
	BYTE	m_nEndCondLevelMax;
	int		m_nEndCondExpPercentMin;
	int		m_nEndCondExpPercentMax;
	int		m_nEndCondPetLevel;
	int		m_nEndCondPetExp;
	int		m_nEndCondDisguiseMoverIndex;
	char	m_nEndCondParty;
	char 	m_nEndCondPartyNumComp;
	WORD 	m_nEndCondPartyNum;
	char	m_nEndCondPartyLeader;
	char	m_nEndCondGuild;
	char 	m_nEndCondGuildNumComp;
	WORD	m_nEndCondGuildNum;
	char	m_nEndCondGuildLeader;
	BYTE	m_nEndCondState;
	BYTE    m_nEndCondCompleteQuestOper; 
	WORD	m_nEndCondCompleteQuest[6];
	CHAR    m_szEndCondDlgCharKey[64];
	QuestGoalData m_DialogCharacterGoalData;
	CHAR    m_szEndCondDlgAddKey[64];
	CHAR    m_szPatrolZoneName[64];

	int		m_nDlgRewardItemIdx[4];
	int		m_nDlgRewardItemNum[4];

	int		m_nEndRemoveItemIdx[8];
	int		m_nEndRemoveItemNum[8];
	int		m_nEndRemoveGold;
	int		m_anEndRemoveQuest[MAX_QUESTREMOVE];

	QuestPropItem* m_paEndRewardItem;
	int		m_nEndRewardItemNum;
	int		m_nEndRewardGoldMin;
	int		m_nEndRewardGoldMax;
	int		m_nEndRewardExpMin;
	int		m_nEndRewardExpMax;

	int		m_nEndRewardTeleport;
	D3DXVECTOR3		m_nEndRewardTeleportPos;
	BOOL	m_bEndRewardPetLevelup;
	int		m_nEndRewardSkillPoint;
	bool    m_bEndRewardHide;
	bool    m_bRepeat;

#if defined( __WORLDSERVER )
	CHAR* m_apQuestDialog[32];
#endif
	QuestState* m_questState[16];
};

typedef struct
{
	TCHAR	m_szSrc[64];
	TCHAR	m_szDst[64];

} FILTER, * LPFILTER;

typedef struct tagJOB
{
	TCHAR	szName[32];
	TCHAR	szEName[32];
	DWORD	dwJobBase;
	DWORD	dwJobType;

} JOB, * LPJOB;

typedef struct tagSTRUCTURE
{
	TCHAR	szName[32];
	TCHAR	szEName[32];

} STRUCTURE, * LPSTRUCTURE;


typedef struct tagGUILD_APPELL
{
	TCHAR	szName[32];
	TCHAR	szEName[32];

} GUILD_APPELL, * LPGUILD_APPELL;

typedef struct tagFxpCharacter
{
	DWORD	dwFxp;
	DWORD	dwFxp2;
} FXPCHARACTER, * LPFXPCHARACTER;

#ifndef __S_ADD_RESTATE
typedef struct tagJobAbility
{
	DWORD	dwStr, dwStrAdd;
	DWORD	dwSta, dwStaAdd;
	DWORD	dwDex, dwDexAdd;
	DWORD	dwInt, dwIntAdd;

} JOBABILITY, * LPJOBABILITY;
#endif // __S_ADD_RESTATE

typedef struct tagATKSTYLE
{
	DWORD	dwParam1, dwParam2;

} ATKSTYLE, * LPATKSTYLE;

typedef struct tagADDEXPPARTY
{
	DWORD	Exp, Level;
} ADDEXPPARTY, * LPADDEXPPARTY;



struct MotionProp
{
	DWORD	dwID;
	DWORD	dwAction;
	DWORD	dwMotion;
	DWORD	dwPlay;				// 0:1play    1:cont		2:loop
	DWORD	dwRequireLv;
	DWORD	dwRequireExp;
	TCHAR	szName[32];
	TCHAR	szRoot[32];
	TCHAR	szLink[128];
	TCHAR	szIconName[32];
	TCHAR	szDesc[128];
	CTexture* pTexture;
};

typedef struct _VENDOR_ITEM
{
	DWORD	m_dwItemId;
	int		m_nShopCost;
	int		m_nItemkind3;
	int		m_nItemJob;
	int		m_nUniqueMin;
	int		m_nUniqueMax;

	_VENDOR_ITEM() { Init(); };
	~_VENDOR_ITEM() {};

	void Init()
	{
		m_dwItemId = NULL_ID;
		m_nShopCost = 0;
		m_nItemkind3 = 0;
		m_nItemJob = 0;
		m_nUniqueMin = 0;
		m_nUniqueMax = 0;
	};

} VENDOR_ITEM, * LPVENDOR_ITEM;

#ifdef __NPC_BUFF
typedef struct _NPC_BUFF_SKILL
{
	DWORD	dwSkillID;
	DWORD	dwSkillLV;
	int		nMinPlayerLV;
	int		nMaxPlayerLV;
	DWORD	dwSkillTime;
} NPC_BUFF_SKILL;
#endif // __NPC_BUFF

#ifdef __FL_DUNGEON_TREASURE
#ifdef __WORLDSERVER
struct DUNGEON_TREASURES
{
	DWORD dwWorldID;
	int nChance;
	DWORD dwItemID;
	int nItemAmount;
};
#endif
#endif

#ifdef __FL_DAILY_REWARDS
#if defined __WORLDSERVER || defined __CLIENT
struct DAILY_REWARD
{
	DWORD dwItemID;
	int nItemNum;
	BOOL bItemBind;
};
#endif
#endif

#ifdef __FL_FARM_DISPLAY
#ifdef __CLIENT
struct FARM_DISPLAY
{
	DWORD dwItem;
	int nCount;
	
	FARM_DISPLAY() : dwItem(NULL_ID), nCount(0)
	{
	}
};
#endif // __CLIENT
#endif

#ifdef __FL_EQUALIZED_MODE
#if defined (__WORLDSERVER) || defined (__CLIENT)
struct TEMPORAL_WORLDS
{
	DWORD dwWorldID;
	int nMode;
	int nMaxJob[8];
};
struct TEMPORAL_LIMITS
{
	int nJob;
	int nCurrentAmountOfPlayers;
};
#endif
#endif

typedef struct tagCHARACTER
{
	CHAR			m_szKey[64];
	CString			m_strName;
	CHAR			m_szChar[64];
	CHAR			m_szDialog[64];
	CHAR			m_szDlgQuest[64];
	int				m_nStructure;
	int				m_nEquipNum;
	DWORD			m_dwMoverIdx;
	DWORD			m_dwMusicId;
	DWORD			m_adwEquip[MAX_HUMAN_PARTS];
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;
	BOOL			m_abMoverMenu[MAX_MOVER_MENU];
#ifdef __NPC_ADDMENU2
	BOOL			m_abMoverMenu2[MAX_MOVER_MENU];
	CString			m_abMoverMenuText[MAX_MOVER_MENU];
#endif
	CString			m_venderSlot[4];
	CPtrArray		m_venderItemAry[4];

	CWordArray		m_awSrcQuest;
	CWordArray		m_awDstQuest;
	CUIntArray		m_anSrcQuestItem;
	CUIntArray		m_anDstQuestItem;
	void Clear();

#if __VER >= 11 // __CSC_VER11_3
#ifdef __NPC_SETVENDORTYPE
	DWORD			m_nVenderType;
#else
	int				m_nVenderType;
#endif
	CPtrArray		m_venderItemAry2[4];
#endif //__CSC_VER11_3
#ifdef __NPC_BUFF
	vector<NPC_BUFF_SKILL> m_vecNPCBuffSkill;
#endif // __NPC_BUFF
#ifdef __CHIPI_DYO
	vector<DWORD>	m_vecdwLanguage;
	BOOL			bOutput;
#endif // __CHIPI_DYO
#if __VER >= 13 // __QUEST_HELPER
	DWORD			m_dwWorldId;
	D3DXVECTOR3		m_vPos;
#endif // __QUEST_HELPER
#if __VER >= 15 // __TELEPORTER
	vector<D3DXVECTOR3> m_vecTeleportPos;
#endif // __TELEPORTER
} CHARACTER, * LPCHARACTER;

#ifdef __S1108_BACK_END_SYSTEM

typedef struct	_MONSTER_RESPAWN
{
	char		szMonsterName[32];
	int			nRespawnIndex;
	D3DXVECTOR3	vPos;
	int			nQuantity;
	int			nAggressive;
	int			nRange;
	int			nTime;

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

#ifndef __DBSERVER
struct AMP_STORE_INFO
{
	DWORD dwIDSecond;
	DWORD dwIDThird;
	DWORD dwIDFourth;
	DWORD dwIDFifth;

	AMP_STORE_INFO() : dwIDSecond(0), dwIDThird(0), dwIDFourth(0), dwIDFifth(0) {}
};
#endif // __DBSERVER

inline void LOG_RANGE(const char* szMsg, int nMin, int nMax, int nIndex)
{
	LPCTSTR szErr = Error(_T("%s min:%d, max:%d, index:%d"), szMsg, nMin, nMax, nIndex);
	//ADDERRORMSG( szErr );
}

inline void LOG_CALLSTACK()
{
#ifdef _DEBUG
	__asm int 3
#endif

#if defined(__INTERNALSERVER)
	int* p = NULL;
	*p = 1;
#endif
}

#define VERIFY_RANGE( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			return (result); \
		}  \
	} while (0)

#define VERIFY_RANGE_ASSERT( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			LOG_CALLSTACK(); \
			return (result); \
		}  \
	} while (0)



#if defined (__WORLDSERVER) || defined (__CLIENT) // __FL_BOXES_V2
#define	MAX_GIFTBOX_ITEM	128
typedef	struct	_GIFTBOX
{
	DWORD	dwGiftbox;
	int		nSum;
	int		nSize;
	DWORD	adwItem[MAX_GIFTBOX_ITEM];
	DWORD	adwProbability[MAX_GIFTBOX_ITEM];
	int		anNum[MAX_GIFTBOX_ITEM];
	BYTE	anFlag[MAX_GIFTBOX_ITEM];
	int		anSpan[MAX_GIFTBOX_ITEM];
	int		anAbilityOption[MAX_GIFTBOX_ITEM];

#ifdef __FL_BOXES_V2
#ifdef __CLIENT
	DWORD dwChance[MAX_GIFTBOX_ITEM];
#endif
#endif
}	GIFTBOX, * PGIFTBOX;

typedef struct	_GIFTBOXRESULT
{
	DWORD	dwItem;
	int		nNum;
	BYTE nFlag;
	int nSpan;
	int	nAbilityOption;
	_GIFTBOXRESULT()
	{
		dwItem = 0;
		nNum = 0;
		nFlag = 0;
		nSpan = 0;
		nAbilityOption = 0;
	}
}	GIFTBOXRESULT, * PGIFTBOXRESULT;

class	CGiftboxMan
{
private:
	vector<GIFTBOX> m_vGiftBox;
	map<DWORD, int>	m_mapIdx;
	int	m_nQuery;

public:
	CGiftboxMan();
	virtual	~CGiftboxMan() {}

	static	CGiftboxMan* GetInstance(void);

	BOOL	AddItem(DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag = 0, int nSpan = 0, int nAbilityOption = 0);
	BOOL	Open(DWORD dwGiftBox, PGIFTBOXRESULT pGiftboxResult);
	void	Verify(void);
#ifdef __FL_BOXES_V2
	PGIFTBOX TakeInfo(DWORD dwGiftbox);
#endif
};
#endif	// __WORLDSERVER

#define	MAX_ITEM_PER_PACK	32
#define	MAX_PACKITEM		1024

typedef	struct	_PACKITEMELEM
{
	DWORD dwPackItem;
	int		nSize;
	int		nSpan;
	DWORD	adwItem[MAX_ITEM_PER_PACK];
	int		anAbilityOption[MAX_ITEM_PER_PACK];
	int		anNum[MAX_ITEM_PER_PACK];
}	PACKITEMELEM, * PPACKITEMELEM;

class CPackItem
{
private:
	int		m_nSize;
	PACKITEMELEM	m_packitem[MAX_PACKITEM];
	map<DWORD, int>	m_mapIdx;

public:
	CPackItem();
	virtual	~CPackItem() {}

	static	CPackItem* GetInstance(void);

	BOOL	AddItem(DWORD dwPackItem, DWORD dwItem, int nAbilityOption, int nNum);
	PPACKITEMELEM	Open(DWORD dwPackItem);
};


#ifdef __FL_NEW_BOXES
struct	NEWBOXELEM
{
	DWORD dwReward;
	int nItemCount;
	BOOL bBinded;
};

class CNewBoxes
{
private:
	map<DWORD, vector<NEWBOXELEM>> m_mapNewBoxes;

public:
	CNewBoxes();
	virtual	~CNewBoxes() {}

	static	CNewBoxes* GetInstance(void);
	
	BOOL LoadFromFile();
	vector<NEWBOXELEM>* GetBoxElements(DWORD dwID);
};
#endif


#define	MAX_ITEMAVAIL	32
typedef struct	__ITEMAVAIL
{
	int		nSize;
	int		anDstParam[MAX_ITEMAVAIL];
	int		anAdjParam[MAX_ITEMAVAIL];
	__ITEMAVAIL()
	{
		nSize = 0;
	}
}	ITEMAVAIL, * PITEMAVAIL;

typedef	struct	__NEWSETITEMAVAIL : public ITEMAVAIL
{
	int		anEquiped[MAX_ITEMAVAIL];
}	NEWSETITEMAVAIL, * PNEWSETITEMAVAIL;

#define	MAX_SETITEM_STRING		64
#define	MAX_SETITEM_ELEM		8
class CSetItem
{
public:
	int		m_nId;
	char	m_pszString[MAX_SETITEM_STRING];
	int		m_nElemSize;
	int		m_anParts[MAX_SETITEM_ELEM];
	DWORD	m_adwItemId[MAX_SETITEM_ELEM];
	NEWSETITEMAVAIL		m_avail;

public:
	//	Constructions
	CSetItem(int nId, const char* pszString);
	virtual	~CSetItem() {}

	BOOL	AddSetItemElem(DWORD dwItemId, int nParts);
	BOOL	AddItemAvail(int nDstParam, int nAdjParam, int nEquiped);
	void	SortItemAvail(void);
	void    GetItemEquiped(int& nEqui, PITEMAVAIL pItemAvail, int nEquiped);
	void	GetItemAvail(PITEMAVAIL pItemAvail, int nEquiped, BOOL bAll);
	char* GetString(void) { return m_pszString; }
};

class CSetItemFinder
{
private:
	map<DWORD, CSetItem*>		m_mapItemId;
	map<int, CSetItem*>		m_mapSetId;
public:
	CSetItemFinder() {}
	virtual	~CSetItemFinder() { Free(); }

	void	AddSetItem(CSetItem* pSetItem);
	CSetItem* GetSetItem(int nSetItemId);
	CSetItem* GetSetItemByItemId(DWORD dwItemId);
	void	Free();
	static	CSetItemFinder* GetInstance(void);
};

typedef struct	_SETITEMAVAIL
{
	int		nHitRate;
	int		nBlock;
	int		nMaxHitPointRate;
	int		nAddMagic;
	int		nAdded;
}	SETITEMAVAIL, * PSETITEMAVAIL;

#define	MAX_PIERCING_DSTPARAM	32
typedef struct _PIERCINGAVAIL
{
	DWORD	dwItemId;
	int		nSize;
	int		anDstParam[MAX_PIERCING_DSTPARAM];
	int		anAdjParam[MAX_PIERCING_DSTPARAM];
} PIERCINGAVAIL, * PPIERCINGAVAIL;

#define	MAX_PIERCING_MATERIAL	128
class CPiercingAvail
{
private:
	int		m_nSize;
	map<DWORD, int>		m_mapIdx;
	PIERCINGAVAIL	m_pPiercingAvail[MAX_PIERCING_MATERIAL];

public:
	CPiercingAvail();
	~CPiercingAvail();
	BOOL	AddPiercingAvail(DWORD dwItemId, int nDstParam, int nAdjParam);
	PPIERCINGAVAIL	GetPiercingAvail(DWORD dwItemId);

	static	CPiercingAvail* GetInstance(void);
};



#define	MAX_RANDOMOPTITEMSTRING		32
typedef	struct	_RANDOMOPTITEM
{
	int	nId;
	int nLevel;
	char pszString[MAX_RANDOMOPTITEMSTRING];
	DWORD	dwProbability;
	ITEMAVAIL	ia;
	_RANDOMOPTITEM()
	{
		nId = 0;	nLevel = 0;	*pszString = '\0';	dwProbability = 0;
	}
}	RANDOMOPTITEM, * PRANDOMOPTITEM;

#if __VER >= 11 // __SYS_IDENTIFY
#define	MAX_RANDOMOPTITEM		256
#else	// __SYS_IDENTIFY
#define	MAX_RANDOMOPTITEM		1024
#endif	// __SYS_IDENTIFY

class CRandomOptItemGen
{
	int		m_nSize;
	RANDOMOPTITEM	m_aRandomOptItem[MAX_RANDOMOPTITEM];
	map<int, int>	m_mapid;

#if __VER >= 10
#if __VER >= 15
	int		m_anIndex[MAX_MONSTER_LEVEL];
#else
	int		m_anIndex[MAX_LEGEND_LEVEL];
#endif
#else
	int		m_anIndex[MAX_LEVEL];
#endif

public:
	//	Constructions
	CRandomOptItemGen();
	virtual	~CRandomOptItemGen() { Free(); }

	BOOL	AddRandomOption(PRANDOMOPTITEM pRandomOptItem);
	PRANDOMOPTITEM	GetRandomOptItem(int nId);
	const char* GetRandomOptItemString(int nId);
	int		GenRandomOptItem(int nLevel, FLOAT fPenalty, ItemProp* pItemProp, DWORD dwClass);
	void	Arrange(void);
	void	Free(void);

	static	CRandomOptItemGen* GetInstance();
};


//////////////////////////////////////////////////////////////////////////////////////////////
// typedef
//////////////////////////////////////////////////////////////////////////////////////////////

typedef map<u_long, string>	ULONG2STRING;
typedef map<string, u_long>	STRING2ULONG;
#ifndef __DBSERVER
typedef map<DWORD, AMP_STORE_INFO*> MAPAMPSTORE;
#endif // __DBSERVER

//////////////////////////////////////////////////////////////////////////////////////////////
// CProject
//////////////////////////////////////////////////////////////////////////////////////////////

class CObj;
class CCtrl;
class CSfx;
class CItem;
class CMover;
#ifdef __WORLDSERVER
class CUser;
#endif

class CProject
{
public:
	CProject();
	virtual ~CProject();

private:
	map<string, DWORD>			m_mapII;
	map<string, DWORD>			m_mapMVI;
	map<string, DWORD>			m_mapCtrl;
#ifdef __CLIENT
	CDWordArray					m_aStateQuest;
#endif

#ifdef __FL_UPDATE_READSTAFF
#ifdef __WORLDSERVER
	map<CString, DWORD> mapServerStaff;
#endif
#endif


#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
	map< int, CString >			m_mapQuestDestination;
	map< int, CString >			m_mapPatrolDestination;
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

public:
	static FLOAT				m_fItemDropRate;
	static FLOAT				m_fGoldDropRate;
	static FLOAT				m_fMonsterExpRate;
	static FLOAT				m_fMonsterHitRate;
	static FLOAT				m_fShopCost;
	static FLOAT				m_fSkillExpRate;
	static DWORD				m_dwVagSP;
	static DWORD				m_dwExpertSP;
	static DWORD				m_dwProSP;
	static DWORD				m_dwLegendSP;

#ifndef __DBSERVER
	MAPAMPSTORE m_mapAmpStore;
#endif // __DBSERVER

	map<u_long, CMover*>		m_idPlayerToUserPtr;
	CObjMap						m_objmap;
	CModelMng					m_modelMng;
	int							m_nMoverPropSize;
	MoverProp* m_pPropMover;
	CFixedArray< ItemProp >		m_aPartySkill;
	CFixedArray< CtrlProp >		m_aPropCtrl;
	CFixedArray< MotionProp >	m_aPropMotion;
	CFixedArray< ItemProp >		m_aPropItem;
	CFixedArray< ItemProp >		m_aPropSkill;
	CFixedArray< AddSkillProp > m_aPropAddSkill;
	CFixedArray< tagColorText >	m_colorText;
	CFixedArray< QuestProp >	m_aPropQuest;
	CFixedArray<GUILDQUESTPROP>	m_aPropGuildQuest;
	CMapStringToPtr				m_mapCharacter;
	JobProp						m_aPropJob[MAX_JOB];
	ItemProp* m_aJobSkill[MAX_JOB][40];
	DWORD						m_aJobSkillNum[MAX_JOB];
	JOB							m_aJob[MAX_JOB];
	JOBITEM						m_jobItem[MAX_JOBITEM];
	STRUCTURE					m_aStructure[MAX_STRUCTURE];
	GUILD_APPELL				m_aGuildAppell[MAX_GUILDAPPELL];
	EXPCHARACTER				m_aExpCharacter[MAX_EXPCHARACTER];
	FXPCHARACTER				m_aFxpCharacter[MAX_FXPCHARACTER];
#ifndef __S_ADD_RESTATE
	JOBABILITY					m_aJobAbillity[MAX_JOB + 1];
#endif // __S_ADD_RESTATE
	DWORD						m_aExpLPPoint[MAX_EXPLPPOINT];
	DWORD						m_aExpSkill[MAX_EXPSKILL];
	EXPPARTY					m_aExpParty[MAX_PARTYLEVEL];
	ADDEXPPARTY					m_aAddExpParty[MAX_ADDEXPPARTY];
	CPtrArray					m_itemKindAry[MAX_ITEM_KIND3];
	int							m_aExpUpItem[6][11];
	DWORD						m_adwExpDropLuck[122][11];
	SETITEMAVAIL				m_aSetItemAvail[11];
	SIZE						m_minMaxIdxAry[MAX_ITEM_KIND3][MAX_UNIQUE_SIZE];
	map<int, PARTYQUESTPROP>	m_propPartyQuest;

	set<DWORD>					m_setExcept;

#ifdef __CLIENT
	CTerrainMng					m_terrainMng;
	CMapStringToString			m_mapHelp;
#endif


#ifdef __FL_GW_STATUE_V2
#ifdef __WORLDSERVER
private:
	u_long m_idCurrentMvpStatueId;
	u_long m_idCurrentLeaderStatueId;

	DWORD m_dwMvpBuffCurrency;
	DWORD m_dwLeaderBuffCurrency;

	int m_nMvpBuffPrice;
	int m_nLeaderBuffPrice;

public:
	void WipeStatuesParam();

	// Mover ID
	void SetCurrentMvpStatueId(u_long val) { m_idCurrentMvpStatueId = val; }
	u_long	GetCurrentMvpStatueId(void) { return m_idCurrentMvpStatueId; }

	void SetCurrentLeaderStatueId(u_long val) { m_idCurrentLeaderStatueId = val; }
	u_long	GetCurrentLeaderStatueId(void) { return m_idCurrentLeaderStatueId; }

	// Buff Price
	void SetCurrentMvpStatuePrice(int val) { m_nMvpBuffPrice = val; }
	int	GetCurrentMvpStatuePrice(void) { return m_nMvpBuffPrice; }

	void SetCurrentLeaderStatuePrice(int val) { m_nLeaderBuffPrice = val; }
	int	GetCurrentLeaderStatuePrice(void) { return m_nLeaderBuffPrice; }
#endif
#endif

#ifdef __S1108_BACK_END_SYSTEM
	FLOAT						m_fMonsterRebirthRate;
	FLOAT						m_fMonsterHitpointRate;
	FLOAT						m_fMonsterAggressiveRate;
	FLOAT						m_fMonsterRespawnRate;
	MONSTER_PROP				m_aMonsterProp[MAX_MONSTER_PROP];	// Monster Prop
	MONSTER_PROP				m_aAddProp[MAX_MONSTER_PROP];	// Monster Prop
	char						m_aRemoveProp[MAX_MONSTER_PROP][32];	// Monster Prop
	int							m_nAddMonsterPropSize;
	int							m_nRemoveMonsterPropSize;
	int							m_nMonsterPropSize;
	char						m_chGMChat[10][256];
#endif // __S1108_BACK_END_SYSTEM

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	int				m_nEnchantLimitLevel[3];
	float			m_fEnchantLevelScal[2][10];
#endif //__Y_ADV_ENCHANT_EFFECT

#ifdef __EVE_MINIGAME
	CMiniGame		m_MiniGame;
#endif // __EVE_MINIGAEM

#if __VER >= 9 // __ULTIMATE
	CUltimateWeapon	m_UltimateWeapon;
#endif //__ULTIMATE

#ifdef __TRADESYS
	CExchange m_Exchange;
#endif // __TRADESYS

#if __VER >= 9 && defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
	CEventLua m_EventLua;
#endif // __EVENTLUA && __WORLDSERVER

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT
	enum { BLOCKING_NUMBER_MAX = 1000 };
	set< CString > m_setBlockedUserID;
#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __CLIENT
#ifdef __SHOP_COST_RATE
public:
	float	m_fShopBuyRate;
	float	m_fShopSellRate;
#endif // __SHOP_COST_RATE

	BOOL m_bLoadedMoverProp;
	BOOL m_bLoadedItemProp;
#endif // __CLIENT

#ifdef __CLIENT
	CMapInformationManager m_MapInformationManager;
#endif // __CLIENT

public:
#ifdef __FL_UPDATE_READSTAFF
#ifdef __WORLDSERVER
	void LoadServerStaff();
	DWORD CheckStaff(CString strName);
#endif
#endif

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
	BOOL			LoadPropEnchant(LPCTSTR lpszFileName);
#endif //__Y_ADV_ENCHANT_EFFECT
	static void		SetGlobal(UINT type, float fValue);

	PSETITEMAVAIL	GetSetItemAvail(int nAbilityOption);
	int				GetMinIdx(int nItemKind3, DWORD dwItemRare);
	int				GetMaxIdx(int nItemKind3, DWORD dwItemRare);
	ObjProp* GetProp(int nType, int nIndex);
	JobProp* GetJobProp(int nIndex);
	GUILDQUESTPROP* GetGuildQuestProp(int nQuestId);
	PARTYQUESTPROP* GetPartyQuestProp(int nQuestId);
	BOOL			IsGuildQuestRegion(const D3DXVECTOR3& vPos);
	CtrlProp* GetCtrlProp(int nIndex);
	ItemProp* GetItemProp(int nIndex);
	ItemProp* GetSkillProp(int nIndex);
	CPtrArray* GetItemKindAry(int nKind);
	MoverProp* GetMoverProp(int nIndex);
	MoverProp* GetMoverPropEx(int nIndex);
	DWORD			GetSkillPoint(ItemProp* pSkillProp);
	ItemProp* GetItemProp(LPCTSTR lpszItem);
	MoverProp* GetMoverProp(LPCTSTR lpszMover);
	CtrlProp* GetCtrlProp(LPCTSTR lpszMover);
	MotionProp* GetMotionProp(int nIndex) { return m_aPropMotion.GetAt(nIndex); }
	//	LPATKSTYLE		GetAttackStyle( int nIndex ) { return &m_aAttackStyle[ nIndex ]; }
	int				GetExpUpItem(DWORD dwItemKind3, int nOption);
	AddSkillProp* GetAddSkillProp(DWORD dwSubDefine, DWORD dwLevel);
	AddSkillProp* GetAddSkillProp(DWORD dwSubDefine);
	ItemProp* GetPartySkill(int nIndex) { return m_aPartySkill.GetAt(nIndex); }
	void			LoadDefines();
	void			LoadPreFiles();
	void			LoadStrings();
	BOOL			OpenProject(LPCTSTR lpszFileName);
	BOOL			LoadPropJob(LPCTSTR lpszFileName);
	BOOL			LoadPropMover(LPCTSTR lpszFileName);
	BOOL			LoadPropItem(LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp);

	BOOL	LoadPropSkill(LPCTSTR lpszFileName, CFixedArray<ItemProp>* apObjProp);

	void			OnAfterLoadPropItem();
	BOOL			LoadPropSfx(LPCTSTR lpszFileName);
	BOOL			LoadPropCtrl(LPCTSTR lpszFileName, CFixedArray<CtrlProp>* apObjProp);
	BOOL			LoadMotionProp(LPCTSTR lpszFileName);
	BOOL			LoadText(LPCTSTR lpszFileName);
	BOOL			LoadExpTable(LPCTSTR lpszFileName);
	BOOL			LoadPropMoverEx_AI_SCAN(LPCTSTR szFileName, CScript& script, int nVal);
	BOOL			LoadPropMoverEx_AI_BATTLE(LPCTSTR szFileName, CScript& script, int nVal);
	BOOL			LoadPropMoverEx_AI_MOVE(LPCTSTR szFileName, CScript& script, int nVal);
	BOOL			LoadPropMoverEx_AI(LPCTSTR szFileName, CScript& script, int nVal);
	BOOL			LoadPropMoverEx(LPCTSTR szFileName);
	BOOL			LoadJobItem(LPCTSTR szFileName);
	CString			GetLangScript(CScript& script);
	BOOL			LoadCharacter(LPCTSTR szFileName);
	BOOL			LoadEtc(LPCTSTR szFileName);
	BOOL			LoadPropAddSkill(LPCTSTR lpszFileName);

	BOOL			LoadPropQuest(LPCTSTR szFileName, BOOL bOptimize = TRUE);
	BOOL			LoadPropGuildQuest(LPCTSTR szFilename);
	BOOL			LoadPropPartyQuest(LPCTSTR szFilename);
	BOOL			LoadDropEvent(LPCTSTR lpszFileName);
	BOOL			LoadGiftbox(LPCTSTR lpszFileName);
	BOOL			LoadPackItem(LPCTSTR lpszFileName);
	BOOL			LoadPiercingAvail(LPCTSTR lpszFileName);
	DWORD			GetTextColor(DWORD dwIndex);
	LPCTSTR			GetText(DWORD dwIndex);
	LPCTSTR			GetGuildAppell(int nAppell);
	CCtrl* GetCtrl(OBJID objid);
	CItem* GetItem(OBJID objid);
	CMover* GetMover(OBJID objid);
	CMover* GetUserByID(u_long idPlayer);
	LPCHARACTER		GetCharacter(LPCTSTR lpStrKey);
	void			LoadSkill();


#ifdef __EVE_MINIGAME
	BOOL			LoadMiniGame();
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	BOOL			LoadUltimateWeapon();
#endif // __ULTIMATE

#ifdef __CLIENT
#ifdef __FL_PET_FILTER
	map<CString, int> mapPetFilterPresets;
	BOOL	LoadPetFilterPresets();
	void	SavePetFilterPresets();
#endif

	map<CString, CString> mapUserAccounts;
	BOOL	LoadUserAccounts();
	void	SaveUserAccounts();
#endif

#ifdef __WORLDSERVER
	CUser* GetUser(OBJID objid);
	BOOL			SortDropItem(void);

#ifdef __JEFF_11_3
	BOOL	LoadServerScript(const char* sFile);
#endif	// __JEFF_11_3

#ifdef __FL_DUNGEON_TREASURE
	vector<DUNGEON_TREASURES> m_vecDungeonTreasures;
	BOOL LoadDungeonTreasures(LPCTSTR lpszFilename);
#endif

#ifdef __FL_SHOP_SEARCH
	std::unordered_set<u_long> m_setPrivateShops;
	void AddToShopSearch(u_long idOwner);
	void RemoveFromShopSearch(u_long idOwner);
#endif

#endif	// __WORLDSERVER

#ifdef __FL_EQUALIZED_MODE
#if defined (__WORLDSERVER) || defined (__CLIENT)
	vector<TEMPORAL_WORLDS> m_vecTmpWorlds;
	BOOL	LoadTemporal(LPCTSTR lpszFilename);
	map<u_long, vector<TEMPORAL_LIMITS>> map_DynamicTemporalCounter;
#endif
#endif

#ifdef __FL_FARM_DISPLAY
#ifdef __CLIENT
	vector<FARM_DISPLAY> m_vecFarmDisplay;
	BOOL LoadFarmDisplay();
	void SaveFarmDisplay();
#endif // __CLIENT
#endif

#ifdef __FL_DAILY_REWARDS
#if defined (__WORLDSERVER) || defined (__CLIENT)
	map<int, DAILY_REWARD> mapDailyRewards;
	BOOL LoadDailyRewards(const char* sFile);
#endif
#endif

#ifdef __FL_RECYCLE
	map<DWORD, int> mapRecycle;
	BOOL LoadRecycle(const char* sFile);
#endif

#ifdef __CLIENT
	BOOL			LoadHelp(LPCTSTR lpszFileName);
	CString			GetHelp(LPCTSTR lpStr);
	CSfx* GetSfx(OBJID objid);
#endif	// __CLIENT

#ifdef __RULE_0615
	set<char>	m_sAllowedLetterAuth;
	set<char>	m_sAllowedLetterGame;
	BOOL	LoadAllowedLetter(void);
	BOOL	IsAllowedLetter(LPCSTR szName, BOOL bAuth);
	void	Formalize(LPSTR szName);
#endif	// __RULE_0615

#ifdef __OUTPUT_INFO_0803
	void			OutputStore(const char* lpStrKey, CMover* pMover);
#endif	// __OUTPUT_INFO_0803
	void			OutputDropItem(void);

#ifdef __ADDSKILLPROP0811
	void			OutputSkill(void);
#endif

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp(MONSTER_PROP MonsterProp);
	void			RemoveMonsterProp(char* lpszMonsterName);
#endif

#if __VER >= 15 /* __IMPROVE_QUEST_INTERFACE */ && defined( __CLIENT )
	BOOL LoadQuestDestination(void);
	const CString& GetQuestDestination(DWORD dwKey) const;
	BOOL LoadPatrolDestination(void);
	const CString& GetPatrolDestination(DWORD dwKey) const;
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#ifdef __CLIENT
	BOOL LoadPropMapComboBoxData(const CString& strFileName);
#endif // __CLIENT

#ifdef __FL_VENDORS
#ifdef __WORLDSERVER
	map<u_long, u_long> m_mapPlayerShops;
#endif
#endif
};


inline ItemProp* CProject::GetItemProp(LPCTSTR lpszItem)
{
	map<string, DWORD>::iterator i = m_mapII.find(lpszItem);
	if (i != m_mapII.end())
		return GetItemProp(i->second);
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp(LPCTSTR lpszCtrl)
{
	map<string, DWORD>::iterator i = m_mapCtrl.find(lpszCtrl);
	if (i != m_mapCtrl.end())
		return (CtrlProp*)GetCtrlProp(i->second);
	return NULL;
}

inline MoverProp* CProject::GetMoverProp(LPCTSTR lpszMover)
{
	map<string, DWORD>::iterator i = m_mapMVI.find(lpszMover);
	if (i != m_mapMVI.end())
		return GetMoverProp(i->second);
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp(int nIndex)
{
	VERIFY_RANGE_ASSERT(nIndex, 0, m_aPropCtrl.GetSize(), "GetCtrlProp range_error", NULL);
	return m_aPropCtrl.GetAt(nIndex);
}

inline ItemProp* CProject::GetItemProp(int nIndex)
{
	VERIFY_RANGE_ASSERT(nIndex, 0, m_aPropItem.GetSize(), "GetItemProp range_error", NULL);
	return m_aPropItem.GetAt(nIndex);
}

inline ItemProp* CProject::GetSkillProp(int nIndex)
{
	VERIFY_RANGE(nIndex, 0, m_aPropSkill.GetSize(), "GetSkillProp range_error", NULL);
	return m_aPropSkill.GetAt(nIndex);
}

inline CPtrArray* CProject::GetItemKindAry(int nKind)
{
	return &m_itemKindAry[nKind];
}

inline MoverProp* CProject::GetMoverProp(int nIndex)
{
	VERIFY_RANGE(nIndex, 0, m_nMoverPropSize, "GetMoverProp range_error", NULL);
	return m_pPropMover + nIndex;
}

inline MoverProp* CProject::GetMoverPropEx(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nMoverPropSize)
		return NULL;
	return GetMoverProp(nIndex);
}

inline DWORD CProject::GetTextColor(DWORD dwIndex)
{
	VERIFY_RANGE((int)(dwIndex), 0, m_colorText.GetSize(), "GetTextColor range_error", 0xFFFFFFFF);
	if (m_colorText.GetAt(dwIndex) == NULL)
		return 0;
	return m_colorText.GetAt(dwIndex)->dwColor;
}

inline LPCTSTR CProject::GetText(DWORD dwIndex)
{
	VERIFY_RANGE((int)(dwIndex), 0, m_colorText.GetSize(), "GetText range_error", "error");
	if (m_colorText.GetAt(dwIndex) == NULL)
		return "";
	return m_colorText.GetAt(dwIndex)->lpszData;
}

inline CMover* CProject::GetUserByID(u_long idPlayer)
{
	map<u_long, CMover*>::iterator i = m_idPlayerToUserPtr.find(idPlayer);
	if (i != m_idPlayerToUserPtr.end())
		return i->second;
	return NULL;
}

inline LPCHARACTER CProject::GetCharacter(LPCTSTR lpStrKey)
{
	if (lpStrKey[0] == 0)	return NULL;
	LPCHARACTER lpCharacter = NULL;
	TCHAR szStrKeyLwr[64];
	_tcscpy(szStrKeyLwr, lpStrKey);
	_tcslwr(szStrKeyLwr);
	m_mapCharacter.Lookup(szStrKeyLwr, (void*&)lpCharacter);
	return lpCharacter;
}

extern CProject prj;

#define GETTEXTCOLOR( x )	prj.GetTextColor( x )
#define GETTEXT( x )		prj.GetText( x )
#define GETANIM( x )		prj.GetAnim( x )
#define GETITEM( x )		prj.GetItem( x )
#define GETMOVER( x )		prj.GetMover( x )

#endif // !defined(AFX_PROJECT_H__3C837668_F3CC_430F_87E6_792AF43D7626__INCLUDED_)
