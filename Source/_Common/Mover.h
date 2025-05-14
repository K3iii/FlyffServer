#if !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)
#define AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pocket.h"

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

#include "Ctrl.h"
#include "..\_AIInterface\ActionMover.h"
#include "SkillInfluence.h"
#include "CooltimeMgr.h"
#include "mempooler.h"

#ifdef __WORLDSERVER
#include "NpcProperty.h"
#include "SfxHitArray.h"
#include "..\_AIInterface\AIInterface.h"
#else
#include "CooltimeMgr.h"
#endif

#ifdef __FL_SWITCH_EQ
#include "Switches.h"
#endif

#ifdef __FL_MONSTER_ALBUM
#include "MonsterAlbum.h"
#endif

#include "Synchronizer.h"

typedef	MemPooler<CMover>	CMoverPool;

#define	MAX_NAME				64

#define	COLOR_PLAYER			0xffffffff
#define COLOR_GAMEMASTER        0xff2D7CAE
#define COLOR_ADMINISTRATOR     0xffffffff
#define	COLOR_NPC				0xffa0a0ff
#define	COLOR_MONSTER			0xffffffa0
#define	COLOR_PARTY				0xff70f070
#define COLOR_GUILDCOMBAT		0xff00ff00

/// SetMotion() option
#define	MOP_NONE				0x00000000
#define	MOP_SWDFORCE			0x00000001
#define	MOP_FIXED				0x00000002
#define	MOP_NO_TRANS			0x00000004
#define	MOP_HITSLOW				0x00000008

#define	REQ_USESKILL			0x00000001
#define	REQ_USEOBJECT			0x00000002

#define	OBJTYPE_PLAYER			0x00000001
#define	OBJTYPE_MONSTER			0x00000002
#define OBJTYPE_CTRL			0x00000004
#define	OBJTYPE_FLYING			0x00010000

#define	CRITICAL_PROBABILITY	3
#define	CRITICAL_BERSERK_HP		30
#define	CRITICAL_BERSERK_PROB	22
#define	CRITICAL_AFTER_DEATH	22

// UPDATEITEM
#define	UI_NUM					0
#define	UI_HP					1
#define	UI_RN					2
#define	UI_AO					3
#define	UI_RAO					4
#define	UI_IR					5
#define	UI_PIERCING_SIZE		6
#define	UI_PIERCING				7
#define UI_COOLTIME				8
#define	UI_FLAG					9
#define	UI_RANDOMOPTITEMID		10
#define	UI_KEEPTIME				11
#define UI_REPAIR_NUM			12

#define UI_ULTIMATE_PIERCING_SIZE	13
#define UI_ULTIMATE_PIERCING		14

#define UI_PETVIS_SIZE			15
#define UI_PETVIS_ITEM			16
#define UI_PETVIS_ITEMSWAP		17
#define UI_TRANSFORM_VISPET		18
#define UI_STACKBEAD 19

const int PETVIS_DEFAULT_VIS_SLOT_SZIE = 2;


#ifdef __ADD_RESTATE_LOW
#define RESTATE_LOW				50
#endif // __ADD_RESTATE_LOW

#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL )

#ifdef __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 1 )
#else	// __INTERNALSERVER
#define	TICK_CHEERPOINT	MIN( 30 )
#endif	// __INTERNALSERVER

#define	MAX_CHEERPOINT	10
#define	NULL_CHGPARAM	0x7FFFFFFF

const DWORD NEXT_TICK_RECOVERY = SEC(2);


#define	LEGEND_CLASS_NORMAL			0
#define	LEGEND_CLASS_MASTER			1
#define	LEGEND_CLASS_HERO			2
#define LEGEND_CLASS_LEGENDHERO		3

enum REPLACE_TYPE
{
	REPLACE_NORMAL,
	REPLACE_FORCE,
};


enum SKILLUSETYPE
{
	SUT_NORMAL,
	SUT_QUEUESTART,
	SUT_QUEUEING,
};


enum TRADE_CONFIRM_TYPE
{
	TRADE_CONFIRM_ERROR,
	TRADE_CONFIRM_OK,
};


enum TRADE_STATE
{
	TRADE_STEP_ITEM,
	TRADE_STEP_OK,
	TRADE_STEP_CONFIRM,
};

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
enum NPC_TYPE
{
	NPC_TYPE_NORMAL,
#ifdef __FL_GW_STATUE_V2
	NPC_TYPE_MVPSTATUE,
	NPC_TYPE_GUILDLEADERSTATUE,
#endif
#ifdef __FL_VENDORS
	NPC_TYPE_VENDOR_HUMAN,
	NPC_TYPE_VENDOR_NPC
#endif
};
#endif


#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
#ifdef __WORLDSERVER
struct PLAYER_COPY
{
	u_long idPlayer;
	CString strName;
	int nJob;
	int nLevel;
	DWORD dwIndex;
	DWORD dwHairId;
	DWORD dwHairColor;
	DWORD dwHeadId;
	CItemContainer<CItemElem>* tmpInventoryOwner;
	D3DXVECTOR3 vPos;
	DWORD dwWorld;
	float fAngle;

	PLAYER_COPY() { Init(); };
	~PLAYER_COPY() {};

	void Init()
	{
		idPlayer = NULL_ID;
		strName = "";
		nJob = JOB_VAGRANT;
		nLevel = 1;
		dwIndex = SEX_MALE;
		dwHairId = 1;
		dwHairColor = 1;
		dwHeadId = 1;
		tmpInventoryOwner = NULL;
		vPos = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		dwWorld = WI_WORLD_MADRIGAL;
		fAngle = 0.0f;
	}
};
#endif
#endif

typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
#ifdef __MODEL_CHANGE
	DWORD	dwLook;
	_EQUIP_INFO() : dwId(NULL_ID), nOption(0), byFlag(0), dwLook(NULL_ID)
	{
	}
	bool isLook()
	{
		return dwLook != NULL_ID;
	}
#endif

}	EQUIP_INFO, * PEQUIP_INFO;

typedef struct tagPLAYER
{
	EQUIP_INFO	m_aEquipInfo[MAX_HUMAN_PARTS];
	char		m_byCostume;
	char		m_bySkinSet;
	char		m_byFace;
	char		m_byHairMesh;
	char		m_byHairColor;
	char		m_byHeadMesh;
	char		m_bySex;
	char		m_byJob;
	u_short		m_uSlot;
} PLAYER, * LPPLAYER;


typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;

	WORD		m_nKillNPCNum[2];
	BYTE		m_bPatrol : 1;
	BYTE		m_bDialog : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;

	QuestProp* GetProp() { return prj.m_aPropQuest.GetAt(m_wId); }

} QUEST, * LPQUEST;


struct HIT_INFO
{
	int		nHit;
	DWORD	dwTick;
};

typedef map<OBJID, HIT_INFO> SET_OBJID;


enum HITTYPE
{
	HITTYPE_FAIL = 0,
	HITTYPE_GENERIC,	
#ifdef __JEFF_11_4
	HITTYPE_ARENA,
#endif	// __JEFF_11_4
};


typedef struct RESURRECTION_DATA
{
	u_long		  dwPlayerID;
	BOOL		  bUseing;
	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	int			  nDamage;
} RESURRECTION_DATA;


struct VENDOR_SELL_RESULT
{
	CItemElem	item;
	int			nRemain;
	int			nErrorCode;
};

#ifdef __FL_EQUALIZED_MODE
#ifdef __WORLDSERVER
struct TEMPORAL_PRESETS
{
	CItemContainer<CItemElem>* m_InventoryPreset;
	DWORD dwWorldID;
	DWORD dwSex;
	DWORD dwJob;
};
#endif
#endif

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
#ifdef __WORLDSERVER
struct INVITE_REQUESTS
{
	u_long dwInvieMember;
	DWORD dwTimeEnd;
};
#endif
#endif

class CVTInfo
{
private:
	DWORD					m_dwTradeGold;
	CItemElem* m_apItem_VT[MAX_VENDITEM];		/// vendor and trader share pointer array
	OBJID					m_objId;
	CMover* m_pOwner;
	TRADE_STATE				m_state;
	string					m_strTitle;

public:
	OBJID					GetOtherID() { return m_objId; }
	CMover* GetOther();
	void					SetOther(CMover* pMover);
	void					Init(CMover* pMover);
	CItemElem* GetItem(BYTE i);
	void					SetItem(BYTE i, CItemElem* pItemBase);
	LPCTSTR					GetTitle();
	void					SetTitle(LPCTSTR szTitle);
	BOOL					IsVendorOpen();

	void					TradeClear();
	void					TradeSetItem(DWORD dwItemObjID, BYTE byNth, int nItemNum);
	BOOL					TradeClearItem(BYTE byNth);
	void					TradeSetGold(DWORD dwGold);
	int						TradeGetGold();
	BOOL					TradeConsent();
	DWORD					TradeSetItem2(DWORD dwItemObjID, BYTE byNth, int& nItemNum);
	TRADE_CONFIRM_TYPE		TradeLastConfirm(CAr& ar);
	TRADE_STATE				TradeGetState();
	void					TradeSetState(TRADE_STATE state);

	void					VendorClose(BOOL bClearTitle = TRUE);
	void					VendorCopyItems(CItemElem** ppItemVd);
	void					VendorItemNum(BYTE i, short nNum);
	BOOL					VendorClearItem(BYTE i);
	BOOL					VendorSellItem(CMover* pBuyer, BYTE nItem, short nNum, VENDOR_SELL_RESULT& result);
	BOOL					VendorIsVendor();
	BOOL				IsTrading(CItemElem* pItemElem);

#ifndef __FL_VENDORS
	void					VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost);
#else
	void					VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost, int nPerinCost);
#ifdef __WORLDSERVER
	void PlayerToVendorAll(CUser* pUserPlayer);
	void PlayerToVendorSingle(CUser* pUserPlayer, CItemElem* pItemBase, BYTE i, short nNum, int nCost, int nPerinCost);
	void VendorToPlayerSingle(CUser* pUserPlayer, BYTE i);
#endif
#endif
};


struct SFXHIT_INFO
{
	OBJID	idTarget;
	int		nMagicPower;
	DWORD	dwSkill;
	int		nDmgCnt;
	float	fDmgAngle;
	float	fDmgPower;
	DWORD	dwAtkFlags;
	DWORD	dwTickCount;
};

struct	SFXHIT_COUNT
{
	DWORD	dwSkill;
	DWORD	dwTickCount;
};

class	CGuild;
class	CGuildWar;
class	CNpcProperty;
struct	ATTACK_INFO;
struct	REGIONELEM;
class	CParty;
class	CActionMover;

#ifdef __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type, nLayer )	Replace( (uMulti), (dwWorld), (vPos), (type), (nLayer) )
#else	// __LAYER_1015
#define	REPLACE( uMulti, dwWorld, vPos, type, nLayer )	Replace( (uMulti), (dwWorld), (vPos), (type) )
#endif	// __LAYER_1015

#ifdef __CLIENT
class CClientPet
{
public:
	CClientPet();
	virtual	~CClientPet();
public:
	void	SetObj(CMover* pObj) { m_pObj = pObj; }
	CMover* GetObj() { return m_pObj; }
	void	SetLevelup(BYTE nLevelup) { m_nLevelup = nLevelup; }
	BYTE	GetLevelup() { return m_nLevelup; }
#ifdef __PET_1024
	void	SetName(char* szName);
	char* GetName() { return m_szName; }
	BOOL	HasName() { return strlen(m_szName) > 0; }
#endif	// __PET_1024
private:
	CMover* m_pObj;
	BYTE	m_nLevelup;

#ifdef __PET_1024
	char	m_szName[MAX_PET_NAME];
#endif	// __PET_1024
};
#endif	// __CLIENT


class CMover : public CCtrl
{
public:
	CMover();
	virtual ~CMover();

	friend			CActionMover;

public:
#ifdef __CLIENT
	int nWidthOld;
#endif

#ifdef __FL_SWITCH_EQ
	CSwitches m_Switches;
#endif

#ifdef __FL_MONSTER_ALBUM
	CMonsterAlbum m_MonsterAlbum;
#endif

	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	CActionMover* m_pActMover = nullptr;
	DWORD			m_dwTypeFlag;
	DWORD			m_dwMode;
	DWORD			m_dwFlag;
	DWORD			m_dwStateMode;
	float			m_fAniSpeed;
	float			m_fArrivalRange;
	OBJID			m_idDest;
	D3DXVECTOR3		m_vDestPos;
	bool			m_bForward;
	DWORD			m_dwRegionAttr;
	DWORD			m_dwOldRegionAttr;
	DWORD			m_dwMotion;
	DWORD			m_dwMotionOption;
	OBJMSG			m_dwMotionArrive;
	bool			m_bPositiveX;			
	bool			m_bPositiveZ;

	DWORD			m_dwTickRecovery;

	BOOL	HasBuff(WORD wType, WORD wId);
	void	RemoveBuff(WORD wType, WORD wId);
	BOOL	HasBuffByIk3(DWORD dwIk3);
	BOOL	HasPet();
#ifdef __WORLDSERVER
	void	RemovePet();
#endif	// __WORLDSERVER
	DWORD	GetDisguise();
	BOOL	AddBuff(WORD wType, WORD wId, DWORD dwLevel, DWORD tmTime, OBJID oiAttacker = NULL_ID);
	void	RemoveCommonBuffs();
	void	RemoveChrStateBuffs(DWORD dwChrState);
	void	RemoveIk3Buffs(DWORD dwIk3);
	void	RemoveAllBuff();
	void	RemoveAttackerBuffs(OBJID oiAttacker);
	void	RemoveDstParamBuffs(DWORD dwDstParam);
	void	RemoveDebufBuffs();
	void	ProcessBuff();
#ifdef __CLIENT
	void	ClearBuffInst();
	int		GetCurrentMaxSkillPoint();
#endif	// __CLIENT

#ifdef __BUFF_1107
	CBuffMgr	m_buffs;
#else	// __BUFF_1107
	CSkillInfluence m_SkillState;
#endif	// __BUFF_1107
	DWORD			m_dwSMTime[SM_MAX];
	SKILL			m_aJobSkill[MAX_SKILL_JOB];
	DWORD			m_tmReUseDelay[MAX_SKILL_JOB];

	LONG			m_nStr, m_nSta, m_nDex, m_nInt;
	LONG			m_nLevel;
	EXPINTEGER		m_nExp1;

public:
	LONG	GetFlightLv(void) { return (GetLevel() >= 20 ? 1 : 0); }

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
private:
	BYTE m_nNpcType;
	u_long m_idNpcOwner;
public:
	void SetNpcType(BYTE nType) { m_nNpcType = nType; }
	BYTE	GetNpcType(void) { return m_nNpcType; }

	void SetNpcOwner(u_long idOwner) { m_idNpcOwner = idOwner; }
	u_long GetNpcOwner(void) { return m_idNpcOwner; }

	void SerializeAbnormalNpc(CAr& ar);
#ifdef __CLIENT
	void RenderPlayerCopy(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont);
#ifdef __FL_GW_STATUE_V2
	void RenderStatueEmoticon(LPDIRECT3DDEVICE9 pd3dDevice);
#endif
#endif
#endif //  defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)


#ifdef __FL_BATTLE_PASS
private:
	int	m_nBattlePassLevel;
	int m_nBattlePassExp;

public:
	int GetBattlePassLevel() const { return m_nBattlePassLevel; }
	int GetBattlePassExp() const { return m_nBattlePassExp; }

	void SetBattlePassLevel(const int nValue) { m_nBattlePassLevel = nValue; }
	void SetBattlePassExp(const int nValue) { m_nBattlePassExp = nValue; }
#endif

#if defined (__FL_CONSUMABLES_STORAGE)
	CItemContainer<CItemElem> m_Chest;
#endif

	CItemContainer<CItemElem>* GetContainerByType(int nType);
	BOOL CanBeUsedOutsideInventory(ItemProp* pItemProp);

	LONG			m_nFxp;
	LONG			m_nHitPoint;				/// HP
	LONG			m_nManaPoint;				/// MP
	LONG			m_nFatiguePoint;
	LONG			m_nRemainGP;
	LONG			m_nDefenseMin, m_nDefenseMax;
	int				m_nAdjHitRate, m_nAdjParry;
	BYTE			m_bySex;
	DWORD			m_dwSkinSet, m_dwFace, m_dwHairMesh, m_dwHairColor, m_dwHeadMesh;
	FLOAT			m_fHairColorR, m_fHairColorG, m_fHairColorB;
	LONG			m_nJob;
	DWORD			m_dwAuthorization;

	TCHAR			m_szCharacterKey[32];
	DWORD			m_dwBelligerence;
	BOOL			m_bActiveAttack;
	DWORD			m_dwVirtItem;
private:
	OBJID	m_oiEatPet;
public:
	int				m_nFuel;
	int				m_tmAccFuel;
	BOOL			m_bItemFind;
	int				m_nReadyTime;
	OBJID			m_dwUseItemId;


	OBJID			m_idAttacker;
	OBJID			m_idTargeter;
	OBJID			m_idTracking;
	OBJID			m_idLastHitMover;
	int				m_nAtkCnt;
	int				m_nReflexDmg;
	LONG			m_nPlusMaxHitPoint;
	DWORD			m_tmPoisonUnit;
	DWORD			m_tmBleedingUnit;
	OBJID			m_idPoisonAttacker;
	OBJID			m_idBleedingAttacker;
	int				m_wPoisonDamage;
	int				m_wBleedingDamage;
	short			m_nHealCnt;

	bool	IsNoActionState();
	bool	IsZeroSpeed();

	FLOAT	m_fSpeedFactor;
	void	SetSpeedFactor(FLOAT fSpeedFactor);
	FLOAT	GetSpeedFactor(void) { return m_fSpeedFactor; }
#ifdef __WORLDSERVER
	BOOL	IsReturnToBegin(void);
#endif	// __WORLDSERVER

	BOOL	IsRank(DWORD dwClass)
	{
		MoverProp* pProp = GetProp();
		return(pProp && pProp->dwClass == dwClass);
	}

	u_long			m_idparty;
	u_long			m_idGuild;
	BOOL			m_bGuildBank;

	DWORD			m_tmActionPoint;
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;


#if __VER >= 13
	int				m_nHonor;
	DWORD			m_dwHonorCheckTime;
	int				m_aHonorTitle[MAX_HONOR_TITLE];
	int				GetHonorTitle(int nIdx) { return m_aHonorTitle[nIdx]; }
	void			SetHonorCount(int nIdx, int nCount);
	void			CheckHonorStat();
	void			CheckHonorTime();
	void			SetHonorAdd(int nSmallIdx, int nLargeGroup, int nDefault = 0);
	CString			m_strTitle;
	LPCTSTR			GetTitle();
	void			SetTitle(LPCTSTR	pTitle);

#endif

#ifdef __CLIENT
	CSfx* m_pSfxBuffPet;
	void SetSfxBuffPet(const DWORD idEffect);
#endif //__CLIENT

#ifdef __JEFF_9_20
	DWORD		m_dwMute;
#endif	// __JEFF_9_20

	RESURRECTION_DATA				m_Resurrection_Data;
	CItemContainer< CItemElem  >* m_ShopInventory[MAX_VENDOR_INVENTORY_TAB];
	CItemContainer< CItemElem  >	m_Inventory;

	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
	CVTInfo			m_vtInfo;

	BYTE			m_nSlot;
	BOOL			m_bBank;
	u_long			m_idPlayerBank[3];
	DWORD			m_dwGoldBank[3];
	CItemContainer< CItemElem >	m_Bank[3];

	BYTE			m_nQuestKeeping;
	BYTE			m_nPartyQuestKeeping;
	BYTE			m_nQuestSize;
	LPQUEST			m_aQuest;
	BYTE			m_nCompleteQuestSize;
	LPWORD   		m_aCompleteQuest;
	BYTE			m_nCheckedQuestSize;
	LPWORD			m_aCheckedQuest;

	int				m_nGuildCombatState;
	D3DXVECTOR3		m_vMarkingPos;
	OBJID			m_idMarkingWorld;

#ifdef __CLIENT
	static BOOL		m_bQuestEmoticonAdd;
	static FLOAT	m_fQuestEmoticonScale;
	int				m_nQuestEmoticonIndex;
	bool			m_bShowQuestEmoticon;

	NaviPoint		m_nvPoint;
	V_NaviPoint		m_vOtherPoint;
	int				m_idSfxHit;
	BYTE			m_nWaterCircleCount;
	DWORD			m_dwReqFlag;
	char			m_szPartyName[32];
	float			m_fDestScale;
	float			m_fDestScaleSlerp;
	CCooltimeMgr	m_cooltimeMgr;

#endif // __CLIENT

#ifdef __WORLDSERVER
	CNpcProperty* m_pNpcProperty;
	CTimer          m_timerQuestLimitTime;
	DWORD			m_dwTickCreated;
	CTime			m_tGuildMember;

	CSfxHitArray	m_sfxHitArray;
	map< OBJID, queue< SFXHIT_INFO> >	m_mapSFXInfo;
	map< OBJID, queue< SFXHIT_COUNT > >	m_mapSFXCount;
#endif	// __WORLDSERVER

#ifdef __WORLDSERVER
private:
	void	DoUseEatPet(CItemElem* PitemElem);
	void	DoUseSystemPet(CItemElem* pItemElem);
public:
	void	ActivateEatPet(CItemElem* pItemElem);
	void	ActivateSystemPet(CItemElem* pItemElem);
	void	InactivateEatPet(void);
	void	InactivateSystemPet(CItemElem* pItemElem);

	void	SetPetVisDST(CItemElem* pItemElem);
	void	ResetPetVisDST(CItemElem* pItemElem);
	void	ProcessVisPet();
#endif	// __WORLDSERVER
	BOOL	IsUsingEatPet(CItemElem* pItemElem);
	BOOL	HasActivatedEatPet(void) { return m_oiEatPet != NULL_ID; }
	BOOL	HasActivatedSystemPet(void) { return m_dwPetId != NULL_ID; }
	OBJID	GetEatPetId(void) { return m_oiEatPet; }
	void	SetEatPetId(OBJID oiEatPet) { m_oiEatPet = oiEatPet; }
	DWORD	GetPetId(void) { return m_dwPetId; }
	void	SetPetId(DWORD dwPetId) { m_dwPetId = dwPetId; }
private:
	DWORD	m_dwPetId;
#if __VER >= 12 // __PET_0519
	int		DoUseItemPetTonic(CItemElem* pItemElem);
#endif	// __PET_0519
#if __VER >= 15 // __PETVIS
private:
	BOOL	SetValidNeedVis(CItemElem* pItemElem, int nPos, vector<BYTE>& vecValid);
public:
	vector<BYTE>	GetValidVisTable(CItemElem* pItemElem);
	BYTE		IsSatisfyNeedVis(CItemElem* pItemElemVisPet, ItemProp* pItemPropVis);
	CItemElem* GetVisPetItem(void) { return m_Inventory.GetAtId(m_objIdVisPet); }
	void		SetVisPetItem(OBJID objId) { m_objIdVisPet = objId; }
	BOOL		HasActivatedVisPet() { return m_objIdVisPet != NULL_ID; }
	DWORD		m_dwMoverSfxId;
private:
	OBJID		m_objIdVisPet;
#endif // __PETVIS

public:
#if __VER >= 9	// __PET_0410
	CPet* GetPet(void);
	CItemElem* GetPetItem(void);
	void	PetLevelup(void);
	void	PetRelease(void);

#ifdef __CLIENT
	CClientPet	m_pet;
	CModelObject* GetPetModel(void) { return (CModelObject*)(m_pet.GetObj() ? m_pet.GetObj()->m_pModel : NULL); }
	void	CreatePetSfx(void);
#else	// __CLIENT	// __WORLDSERVER
	void	ProcessPetAvail(void);
	void	ProcessPetEnergy(void);
	void	ProcessPetExp(void);
	int		DoUseItemFeedPocket(CItemElem* pItemElem);
#endif	// __CLIENT
#endif	// __PET_0410

#ifdef __SYS_TICKET
	CItemElem* GetTicket(void);
#endif	// __SYS_TICKET

#if __VER >= 11 // __SYS_COLLECTING
	int						m_nMaxCltTime;
	int						m_nCltTime;
	BOOL	IsCollecting(void) { return	m_pActMover->GetActionState() == OBJSTA_COLLECT; }
	virtual	void	ProcessCollecting(void);
	virtual	void	StartCollecting(void);
	virtual	void	StopCollecting(void);
	CItemElem* GetCollector(void);
#endif	//	__SYS_COLLECTING

	int				m_nSkillLevel;
	int				m_nSkillPoint;

#if __VER >= 8 //__CSC_VER8_5
	BOOL			m_pAngelFlag;				/// Angel Buff Exist?
	CModelObject* m_pAngel;					/// Angel Model Object.
	D3DXVECTOR3		m_AngelPos;					/// Angel Position.
	D3DXMATRIX		m_AngelWorldM;				/// Angel Matrix.
	DWORD			m_dwAngelKind;				/// Angel Kind.
#endif //__CSC_VER8_5

#ifdef __EVE_BALLOON
	BOOL			m_pBalloonFlag;				/// Balloon Buff Exist?
	CModelObject* m_pBalloon;					/// Balloon Model Object.
	D3DXVECTOR3		m_BalloonPos;				/// Balloon Position.
	D3DXMATRIX		m_BalloonWorldM;			/// Balloon Matrix.
	DWORD			m_dwBalloonKind;			/// Balloon Kind.
	float			m_fBalloonYPos;				/// Balloon Y Position
	BOOL			m_bBalloon;
#endif //__EVE_BALLOON

private:
#ifdef __WORLDSERVER
	SET_OBJID		m_idEnemies;
#endif

	OBJACT			m_oaCmd;
	int				m_nCParam[3];
	OBJACT			m_oaAct;
	int				m_nAParam[5];

	LONG			m_adjParamAry[MAX_ADJPARAMARY];
	LONG			m_chgParamAry[MAX_ADJPARAMARY];

	int				m_nMovePattern;
	int				m_nMoveEvent;
	int				m_nMoveEventCnt;

#ifdef	__CLIENT
	DWORD			m_dwLadolfFlag;
	CModelObject* m_pLadolf;
	enum WingStateFlag
	{
		FLOATING, FLYING, TURNING
	};

	WingStateFlag	m_eWingStateFlag;
	float			m_fOldLengthSq;
public:
	BOOL			m_bExclusiveLowBody;
#endif // __CLIENT


protected:
	int				m_nCount;
	DWORD			m_dwGold;
	DWORD			m_dwRideItemIdx;
	CModelObject* m_pRide;
	TCHAR			m_szName[MAX_NAME];

public:
	static CMover* GetActiveMover() { return (CMover*)m_pObjActive; }
	static	int		GetHairCost(CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair);
	static void		UpdateParts(int nSex, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts = TRUE, CMover* pMover = NULL);
	BOOL	DoEquipMover(CItemElem* pItemElem, DWORD dwPart, BOOL bEquip, const EQUIP_INFO& rEquipInfo, CItemContainer< CItemElem  >* pInventory, PEQUIP_INFO pEquipeInfo);

	virtual	BOOL	SetIndex(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE, BOOL bDestParam = TRUE);
	virtual BOOL	Read(CFileIO* pFile);
	virtual void	Process();
	virtual	void	Serialize(CAr& ar);
	virtual	CModel* LoadModel(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex);
	virtual void	InitProp(BOOL bInitAI = TRUE);

	virtual int		SendDamage(DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE) { return m_pActMover->SendDamage(dwAtkFlag, idAttacker, nParam, bTarget); }
	virtual int		SendDamageForce(DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE) { return m_pActMover->SendDamageForce(dwAtkFlag, idAttacker, nParam, bTarget); }

	void			Init();
	void			SetDefaultLowerBody(CModelObject* pModel);
	void			InitLevel(int nJob, LONG nLevel, BOOL bGamma = TRUE);
	void			ProcessAniSpeed();
	void			AllocShopInventory(LPCHARACTER pCharacter);
	BOOL			IsVendorNPC();
	void			CheckTickCheer();
	void			SetCheerParam(int nCheerPoint, DWORD dwTickCount, DWORD dwRest);
	void			ClearEquipInfo();
	int				GetMaxPoint(int nDest);
	DWORD			GetReuseDelay(int nIndex);
	BOOL			InitSkillExp();
	void			InitCharacter(LPCHARACTER lpCharacter);
	LPCHARACTER		GetCharacter();
	void			InitNPCProperty();
	BOOL			LoadDialog();
	void			ProcessRecovery();
	BOOL			IsActiveMover() { return m_pObjActive == this; }
	int				IsSteal(CMover* pTarget);
	int				IsSteal(OBJID idTaget);
	u_long			GetPartyId() { return m_idparty; }
	BOOL			IsMode(DWORD dwMode);
	void			SetMode(DWORD dwMode) { m_dwMode |= dwMode; }
	void			SetNotMode(DWORD dwMode) { m_dwMode &= (~dwMode); }
	BOOL			SetStun(BOOL bApply);
	BOOL			SetDarkCover(BOOL bApply);
	BOOL			SetPoison(BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, int wDamage = 0);
	BOOL			SetBleeding(BOOL bApply, OBJID idAttacker = NULL_ID, DWORD tmMaxTime = 0, DWORD tmUnit = 0, int wDamage = 0);
	void			RemoveDebuff(DWORD dwState);
	void			RemoveBuffOne(DWORD dwSkill = 0);
	void			RemoveBuffAll();
	BOOL			IsStateMode(DWORD dwMode);
	void			SetStateMode(DWORD dwMode, BYTE nFlag);
	void			SetStateNotMode(DWORD dwMode, BYTE nFlag);
#ifdef __CLIENT
	BOOL			IsUseItemReadyTime(ItemProp* pItemProp, OBJID dwObjItemId, int nType = TYPE_INVENTORY);
#endif
	BOOL			IsNPC() { return !m_bPlayer; }
	BOOL			IsPlayer() const { return m_bPlayer; }
	BOOL			IsEquipableNPC() { return(GetCharacter() && GetCharacter()->m_nEquipNum > 0); }
	BOOL			IsFlyingNPC() { return (m_dwTypeFlag & OBJTYPE_FLYING) ? TRUE : FALSE; }
	BOOL			IsFly();
	MoverProp* GetProp() { return prj.GetMoverProp(m_dwIndex); }
	ItemProp* GetActiveHandItemProp(int nParts = PARTS_RWEAPON);
	ItemProp* GetTransyItem(ItemProp* pItemProp, BOOL bCheck = FALSE, LPCTSTR lpszFileName = NULL);


	OBJID			GetDestId() { return m_idDest; }
	CCtrl* GetDestObj() { return prj.GetCtrl(m_idDest); }
	void			SetDestObj(CCtrl* pObj, float fRange = 0.0f) { SetDestObj(pObj->GetId(), fRange); }
	D3DXVECTOR3		GetDestPos() { return m_vDestPos; }
	void			SetDestPos(CONST D3DXVECTOR3& vDestPos, bool bForward = true, BOOL fTransfer = TRUE);

	void			ClearDestObj(BOOL bCancelTaskbar = TRUE);
	void			ClearDestPos();
	void			ClearDest(BOOL bCancelTaskbar = TRUE) { ClearDestObj(bCancelTaskbar); ClearDestPos(); }
	void			SetStop() { SendActMsg(OBJMSG_STAND); ClearDestObj(); }

	BOOL			IsEmptyDestObj() { return m_idDest == NULL_ID; }
	BOOL			IsEmptyDestPos() { return m_vDestPos.x == 0.0f && m_vDestPos.z == 0.0f; }
	BOOL			IsEmptyDest() { return IsEmptyDestPos() && IsEmptyDestObj(); }

	BOOL			IsRegionAttr(DWORD dwAttribite) { return (m_dwRegionAttr & dwAttribite) == dwAttribite ? TRUE : FALSE; }
	REGIONELEM* UpdateRegionAttr();
	DWORD			GetRideItemIdx() { return m_dwRideItemIdx; }
	void			SetRide(CModel* pModel, int nRideItemIdx = 0);
	int				SendActMsg(OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0);
	void			SendAIMsg(DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0);
	void			PostAIMsg(DWORD dwMsg, DWORD dwParam1 = 0, DWORD dwParam2 = 0);
	BOOL			SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0);	// MOP_SWDFORCE, MOP_FIXED, MOP_NO_TRANS, MOP_HITSLOW
	BOOL			InitMotion(DWORD dwMotion);
	void			FlyByAttack(BOOL bCheckSpeed = TRUE);
	CItemElem* GetWeaponItem(int nParts = PARTS_RWEAPON);
	CItemElem* GetLWeaponItem();
	CItemElem* GetEquipItem(int nParts);
	BOOL			IsDualWeapon();
	void			RedoEquip(BOOL fFakeParts, BOOL bDestParam = TRUE);
	void			UpdateParts(BOOL bFakeParts = FALSE); // normal or fake

#ifdef __WORLDSERVER
	int				InvalidEquipOff();
#endif

	void			DoEquipItem(CItemElem* pItemElem);
	BOOL			IsEquipAble(CItemElem* pItem, BOOL bIgnoreLevel = FALSE);
	BOOL			IsUnEquipAble(ItemProp* pItemProp);
	void			SetEquipDstParam();
	void			SumEquipAdjValue(int* pnAdjHitRate, int* pnAdjParray);
	int				SumEquipDefenseAbility(LONG* pnMin, LONG* pnMax);
	void			SetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem = FALSE);
#ifdef __FL_EQUALIZED_MODE
	void			ResetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem = FALSE);
#else
	void			ResetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem);
#endif
	LPQUEST			FindQuest(int nQuestId);
	BOOL			RemoveQuest(int nQuestId);
	BOOL			IsCompleteQuest(int nQuestId);
	BOOL			MakeCompleteQuest(int nQuestId, LPQUEST lpQuest);
	BOOL			SetQuest(int nQuestId, int nState, LPQUEST lpReturnQuest);
	BOOL			SetQuest(LPQUEST lpQuest);
	void			RemoveAllQuest();
	void			RemoveCompleteQuest();
	BOOL            IsDisguise();
	BOOL			NoDisguise(LPDIRECT3DDEVICE9 pd3dDevice = NULL);
	BOOL			Disguise(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex);
	DWORD			IsAuthHigher(DWORD dwAuthorization) { return dwAuthorization <= m_dwAuthorization; }
	void			UpdateParam();
	int				GetParam(int nDestParameter, int nParam);
	void			SetDestParam(int nDstParameter, int nAdjParameterValue, int nChgParameterValue, BOOL fSend = TRUE);
	void			SetDestParam(int nItemIdx2, BOOL fSend = TRUE);
	void			SetDestParam(int nIdx, ItemProp* pProp, BOOL bSend = TRUE);
	void			ResetDestParam(int nDstParameter, int nAdjParameterValue, BOOL fSend = TRUE);
#ifdef __SPEED_SYNC_0108
	void			ResetDestParamSync(int nDstParameter, int nAdjParameterValue, int nParameterValue, BOOL fSend = TRUE);
#endif
	void			ResetDestParam(int nIdx, ItemProp* pProp, BOOL bSend = TRUE);
	int				GetAdjParam(int nDestParameter);
	int				GetChgParam(int nDestParameter);
	int				GetPointParam(int nDstParameter);
	void			SetPointParam(int nDstParameter, int nValue, BOOL bTrans = FALSE);
	void			OnApplySM();
	LPSKILL			GetSkill(int nType, int nIdx);
	LPSKILL			GetSkill(DWORD dwSkill);
	int				GetSkillIdx(DWORD dwSkill);
	void			OnEndSkillState(DWORD dwSkill, DWORD dwLevel);
	BOOL			CheckSkill(DWORD dwSkill);
	void			SetHair(int nHair);
	void			SetHairColor(DWORD dwHairColor);
	void			SetHairColor(FLOAT r, FLOAT g, FLOAT b);
	void			SetHead(int nHead);
	void			SetName(const char* lpszName) { lstrcpy(m_szName, lpszName); }
	LPCTSTR			GetName(BOOL bNickname = FALSE);
	BYTE			GetSex() { return m_bySex; }
	void			SetSex(BYTE bySex) { m_bySex = bySex; }
	BOOL			IsPeaceful() { return m_dwBelligerence == BELLI_PEACEFUL; }
	BOOL			IsBaseJob();
	BOOL			IsExpert();
	BOOL			IsPro();
	BOOL			IsInteriorityJob(int nJob);
	BOOL			SetExpert(int nExpert);
	BOOL			AddChangeJob(int nJob);
	int				GetJob();
	int				GetExpPercent();

	BOOL			IsJobType(DWORD dwJobType);
	int				GetLevel() { return m_nLevel; }
	int				GetFxp() { return m_nFxp; }
	int				GetTxp() { return m_nFxp; }
	EXPINTEGER		GetExp1() { return m_nExp1; }
	EXPINTEGER		GetMaxExp1() { return prj.m_aExpCharacter[m_nLevel + 1].nExp1; }
	EXPINTEGER		GetMaxExp2() { return prj.m_aExpCharacter[m_nLevel].nExp2; }
	int				GetRemainGP(); // growth
	void			IncHitPoint(int nVal);
	void			IncManaPoint(int nVal);
	void			IncFatiguePoint(int nVal);
	void			SetHitPoint(int nVal);
	void			SetManaPoint(int nVal);
	void			SetFatiguePoint(int nVal);
	float			GetExpFactor();
	float			GetItemDropRateFactor(CMover* pAttacker);
	float			GetPieceItemDropRateFactor(CMover* pAttacker);
	BOOL			AddExperience(EXPINTEGER nExp, BOOL bFirstCall = TRUE, BOOL bMultiply = TRUE, BOOL bMonster = FALSE);

	BOOL			AddFxp(int nFxp);
	BOOL			SetFxp(int nFxp, int nFlightLv);
	HITTYPE			GetHitType(CMover* pMover, BOOL bTarget, int nReflect);
#ifdef __JEFF_11_4
	BOOL	IsArenaTarget(CMover* pMover);
#endif	// __JEFF_11_4
	HITTYPE			GetHitType2(CMover* pMover, BOOL bTarget, BOOL bGood);

	int				GetHR();
	void			SetStr(int nStr) { m_nStr = nStr; }
	void			SetSta(int nSta) { m_nSta = nSta; }
	void			SetDex(int nDex) { m_nDex = nDex; }
	void			SetInt(int nInt) { m_nInt = nInt; }
	int				GetStr();
	int				GetDex();
	int				GetInt();
	int				GetSta();
	FLOAT			GetSpeed(FLOAT fSrcSpeed);
	int				GetGold();
#ifdef __WORLDSERVER
	void MakeCharacterAlive(BOOL bSamePlace, BOOL bScroll);
#endif

	bool	CheckAttackDistance(CObj* pTarget, const DWORD AttackRangeType);

	int		GetPerinNum(void);
	__int64		GetTotalGold(void);
#ifdef __WORLDSERVER
	int		RemoveTotalGold(__int64 iGold);
	int		RemovePerin(int nPerin);
#ifdef __FL_PENYA_CONVERTER
	void PenyaToPerins();
#endif
#endif	// __WORLDSERVER
	void			SetGold(int nGold);
	BOOL			AddGold(int nGold, BOOL bSend = TRUE);
	int				GetHitPointPercent(int nPercent = 100);
	int				GetManaPointPercent(int nPercent = 100);
	int				GetFatiguePointPercent(int nPercent = 100);
	int				GetHitPoint();
	int				GetManaPoint();
	int				GetFatiguePoint();
	int				GetMaxHitPoint();
	int				GetMaxManaPoint();
	int				GetMaxFatiguePoint();
	int				GetMaxOriginHitPoint(BOOL bOrinal = TRUE);
	int				GetMaxOriginManaPoint(BOOL bOrinal = TRUE);
	int				GetMaxOriginFatiguePoint(BOOL bOrinal = TRUE);
	int				GetHPRecovery();
	int				GetMPRecovery();
	int				GetFPRecovery();
	int				GetNaturalArmor();
	float			GetAttackSpeed();

	int				GetSpeedRun();
	int				GetCriticalRate();
	int				GetSpellCastSpeed();
	int				GetPvpDamage();
	int				GetHitRate();
	int				GetBlockMelee();
	int				GetBlockRange();
	int				GetDodgeRate();

	float			GetCastingAniSpeed();
	int				GetCastingTime(int nCastingTime);
	float			GetDamageMultiplier(ATTACK_INFO* pInfo);
	int				PostCalcMagicSkill(int nATK, ATTACK_INFO* pInfo);
	int				PostCalcGeneric(int nATK, ATTACK_INFO* pInfo);
	float			GetMagicSkillFactor(CMover* pDefender, SAI79::ePropType skillType);
#ifndef __FL_DST_ATKPOWER_RATE_MAGIC
	float			GetATKMultiplier(CMover* pDefender, DWORD dwAtkFlags);
#else
	float			GetATKMultiplier(CMover* pDefender, ATTACK_INFO* pInfo);
#endif
	float			GetDEFMultiplier(ATTACK_INFO* pInfo);
	float			GetBlockFactor(CMover* pAttacker, ATTACK_INFO* pInfo);
	int				GetWeaponATK(DWORD dwWeaponType);
	int				GetPlusWeaponATK(DWORD dwWeaponType);
	int				GetWeaponPlusDamage(int nDamage, BOOL bRandom = TRUE);
	void			GetDamagePropertyFactor(CMover* pDefender, int* pnATKFactor, int* pnDEFFactor, int nParts);
	void			GetDamageRange(int& nMin, int& nMax);
	BOOL			IsBlocking(CMover* pAttacker);
	bool			CanFlyByAttack(BOOL bCheckSpeed = TRUE);
	BOOL			IsCriticalAttack(CMover* pDefense, DWORD dwAtkFlags);
	int				GetCriticalProb();
	int				GetReqMp(int nReqMp);
	int				GetReqFp(int nReqFp);
	bool			isUnconditionedHit(CMover* pDefender) const;
	int				calculateHitRate(CMover* pDefender);
	BOOL			GetAttackResult(CMover* pHitObj);
	int				GetAdjHitRate();
	int				GetHitPower(ATTACK_INFO* pInfo);
	int				GetRangeHitPower(ATTACK_INFO* pInfo);
	int				GetParrying();
	int				GetDefenseByItem(BOOL bRandom = TRUE);
	int				GetShowDefense(BOOL bRandom);
	int				GetResistMagic();
	int				GetResistSpell(int nDestParam);
	int				GetMeleeSkillPower(ATTACK_INFO* pInfo);
	int				GetMagicHitPower(int nMagicPower);
	int				GetItemAbility(int nItem);
	int				GetItemAbilityMin(int nItem);
	int				GetItemAbilityMax(int nItem);
	void			GetHitMinMax(int* pnMin, int* pnMax, ATTACK_INFO* pInfo = NULL);
	BOOL			IsDie() { return m_pActMover->IsDie() || m_nHitPoint == 0; }
	BOOL			IsLive() { return m_pActMover->IsDie() == FALSE || m_nHitPoint > 0; }
	int				GetCount() { return m_nCount; }

	float			GetResist(SAI79::ePropType p_PropType);
	int				GetSetItemParts(DWORD dwParts);
	int				GetSetItemClient();
	int				GetSetItem(CItemElem* pItemElem = NULL);
	BOOL			IsSetItemPart(DWORD dwParts);
	void			SetSetItemAvail(int nAbilityOption);
	void			ResetSetItemAvail(int nAbilityOption);
	void			DestParamPiercingAvail(CItemElem* pItemElem, BOOL bSET = TRUE);
	BOOL			Pierce(CItemElem* pSuit, DWORD dwItemId);
	void			SetDestParamSetItem(CItemElem* pItemElem);
	void			ResetDestParamSetItem(CItemElem* pItemElem);
	int				GetEquipedSetItemNumber(CSetItem* pSetItem);
	void			GetEquipedSetItem(int nSetItemId, BOOL* pbEquiped, int* pnEquip);
	void			SetDestParamRandomOpt(CItemElem* pItemElem);
	void			ResetDestParamRandomOpt(CItemElem* pItemElem);

	void	SetDestParamRandomOptOrigin(CItemElem* pItemElem);
	void	SetDestParamRandomOptExtension(CItemElem* pItemElem);
	void	ResetDestParamRandomOptOrigin(CItemElem* pItemElem);
	void	ResetDestParamRandomOptExtension(CItemElem* pItemElem);

	void			GetItemATKPower(int* pnMin, int* pnMax, ItemProp* pItemProp, CItemElem* pWeapon);
	float			GetItemMultiplier(CItemElem* pItemElem);
	BOOL			SubLootDropNotMob(CItem* pItem);
	BOOL			SubLootDropMobSingle(CItem* pItem);
	BOOL			SubLootDropMobParty(CItem* pItem, CParty* pParty);
	BOOL			SubLootDropMob(CItem* pItem);
	BOOL			DoLoot(CItem* pItem);
	void			PickupGoldCore(int nGold);
	void			PickupGold(int nGold, BOOL bDropMob);
	BOOL			IsDropable(CItemElem* pItemElem, BOOL bPK);
	BOOL			IsDropableState(BOOL bPK);
	void			UpdateItem(BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime = 0, int nType = TYPE_INVENTORY);
	void		UpdateItemEx(unsigned char id, char cParam, __int64 iValue);
	CItem* _DropItemNPC(DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3& vPos);
	CItem* DropItem(DWORD dwID, short nDropNum, const D3DXVECTOR3& vPos, BOOL bPlayer = FALSE);

	int				GetItemNum(DWORD dwItemId);
	int				RemoveAllItem(DWORD dwItemId);

#ifdef __WORLDSERVER
	int GetAvailableItemNum(DWORD dwItemId);
	void RemoveAvailableItem(DWORD dwItemId, int nNum);
#endif

	void			OnTradeRemoveUser();
	void			GenerateVendorItem(ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor);
#ifdef __WORLDSERVER
	BOOL			DropItemByDied(CMover* pAttacker);
#endif
	BOOL			DoUseSkill(DWORD dwSkill, int nLevel, OBJID idFocusObj, SKILLUSETYPE sutType = SUT_NORMAL, BOOL bControl = FALSE, const int nCastingTime = 0);

	void			ClearCmd();
	OBJACT			GetCmd() { return m_oaCmd; }
	OBJACT			GetAct() { return m_oaAct; }
	int				GetCmdParam(int nIdx) { return m_nCParam[nIdx]; }
	void			ClearActParam();

#ifdef __WORLDSERVER
	OBJID			GetMaxEnemyHitID();
	DWORD			AddEnemy(OBJID objid, int nHit);
	int				GetEnemyHit(OBJID objid, DWORD* pdwTick = NULL);
	void			RemoveEnemy(OBJID objid);
	void			RemoveAllEnemies();
#endif

	BOOL			IsAttackAble(CObj* pObj);
	FLOAT			GetAttackRange(DWORD dwAttackRange);
	BOOL			IsAttackAbleNPC(CMover* pNPC);

	void			PrintString(CMover* pMover, DWORD dwId);

	BOOL			IsPossibleSetCmd(BOOL bControl = FALSE, BOOL bCheckTaskbar = TRUE);
	int				CMD_SetUseSkill(OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType = SUT_NORMAL);
	void			CMD_SetMeleeAttack(OBJID idTarget, FLOAT fRange = 0.0f);
	void			CMD_SetMagicAttack(OBJID idTarget, int nMagicPower);
	void			CMD_SetRangeAttack(OBJID idTarget, int nPower);
	void			CMD_SetUseObject(CCtrl* pCtrl);
	void			OnAfterUseItem(const ItemProp* pItemProp, bool bSfx = true);


	BOOL			DoUseItemVirtual(DWORD dwItemId, BOOL bEffectSkip);
	BOOL			DoUseItem(DWORD dwItemId, int nType = TYPE_INVENTORY);
	BOOL			DoUseItemBlinkWing(ItemProp* pItemProp, CItemElem* pItemElem, BOOL bUes = TRUE);
	BOOL			DoUseItemFood_SM(ItemProp* pItemProp, CItemElem* pItemElem);
	BOOL			DoUseItemFood(ItemProp* pItemProp, CItemElem* pItemElem);
	BOOL			DoUseItemWarp(ItemProp* pItemProp, CItemElem* pItemElem);
	int				DoUseItemSystem(ItemProp* pItemProp, CItemElem* pItemElem);
	int				DoUseItemKeep(ItemProp* pItemProp, CItemElem* pItemElem);
	void			DoPickupItemAround();
	void			DoSelectNearestPlayer();
	int				DoAttackMagic(CObj* pTargetObj, int nMagicPower, int idSfxHit = 0);
	int				DoAttackRange(CObj* pTargetObj, DWORD dwItemID, int idSfxHit);
	int				DoAttackSP(CObj* pTargetObj, DWORD dwItemID);
	int             DoAttackMelee(CMover* pTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0, bool bCheckDistance = true);
	int				DoAttackMelee(OBJID idTarget, OBJMSG dwMsg = OBJMSG_ATK1, DWORD dwItemID = 0);
	int				DoDie(CCtrl* pAttackCtrl, DWORD dwMsg = 0);
	int				SubExperience(CMover* pDead);
	void			AddKillRecovery();
	BOOL			IsValidArea(CMover* pMover, float fLength);
	void			SetJJim(CMover* pJJimer);
	void			SubReferTime(DWORD* pTime1, DWORD* pTime2, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp);
	BOOL			ApplyParam(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, BOOL bSend = TRUE, int nDamage = 0);
	BOOL			ApplyMagicSkill(CMover* pMover, DWORD dwSkill, DWORD dwLevel, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp);
	BOOL			GetSkillProp(ItemProp** ppSkillProp, AddSkillProp** ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr);
	BOOL			CreateSkillEffect(OBJID idTarget, int nSkill, DWORD dwLevel);
	BOOL			IsInvisibleSkill(DWORD dwSkillID);
	void			RemoveInvisibleSkill();
	void			SendDamageAround(int nDmgType, CMover* pAttacker, int nApplyType, int nAttackID, float fRange, float fTargetRatio, float fAroundRatio);
	void			OnAttacked(CMover* pAttacker, int nDamage, BOOL bTarget, int nReflect);
	BOOL			OnDamage(int nItemID, DWORD dwState, CMover* pHitObj, const D3DXVECTOR3* pvDamagePos = NULL, DWORD dwAtkFlag = AF_GENERIC);
	void			SetCoolTime(AddSkillProp* pAddSkillProp, LPCTSTR szCall);
	BOOL			OnMeleeSkill(int nType, int nCount = 1);
	BOOL			OnMagicSkill(int nType, int nCount = 1);
	BOOL			OnAttackRange();
	BOOL			OnAttackMelee(DWORD dwState, CMover* pHitObj);
	BOOL			OnAttackMeleeContinue(DWORD dwState);
	void			OnAttackMelee_ClockWorks(DWORD dwState, CMover* pHitObj);
	void			OnAttackMelee_BigMuscle(DWORD dwState, CMover* pHitObj);
	void			OnAttackMelee_Krrr(DWORD dwState, CMover* pHitObj);
	void			OnAttackMelee_Bear(DWORD dwState, CMover* pHitObj);
	void			OnAttackMelee_Meteonyker(DWORD dwState, CMover* pHitObj);
	BOOL			OnAttackMagic();
	BOOL			OnAttackSP();
	void			OnActDrop();
	void			OnActCollision();
	void			OnActEndMeleeSkill();
	void			OnActEndMagicSkill();
	void			OnEndMeleeAttack(DWORD dwState);
	void			OnActEndJump4();
	void			OnActLanding();
	void			OnActFMove();
	BOOL			IsBullet(ItemProp* pItemProp);
	void			ProcessMovePattern();
	int				GetMovePattern() { return m_nMovePattern; }
	int				GetMoveEvent() { return m_nMoveEvent; }
	void			SetMovePattern(int nPattern);
	CGuild* GetGuild();
	BOOL			IsSMMode(int nType) { return (m_dwSMTime[nType] > 0) ? TRUE : FALSE; }
	void			ReState();
#ifdef __S_ADD_RESTATE
	void			ReStateOne(int nKind);
#endif // __S_ADD_RESTATE
#ifdef __ADD_RESTATE_LOW
	void			ReStateOneLow(int nKind);
#endif // __ADD_RESTATE_LOW
	float			GetJobPropFactor(JOB_PROP_TYPE type);
	int				CalcDefense(ATTACK_INFO* pInfo, BOOL bRandom = TRUE);
	int				CalcDefenseCore(CMover* pAttacker, DWORD dwAtkFlags, BOOL bRandom = TRUE);
	int				CalcDefensePlayer(CMover* pAttacker, DWORD dwAtkFlags);
	int				CalcDefenseNPC(CMover* pAttacker, DWORD dwAtkFlags);
	void			PutLvUpSkillName_1(DWORD dwLevel);
	void			PutLvUpSkillName_2(DWORD dwSkill);

	CItemElem* GetVendorItem();
	BOOL			IsAttackMode();
	void			AddSkillPoint(int nPoint);

	BYTE			GetLegendChar();
	BOOL			IsMaster();
	BOOL			IsHero();
	BOOL			IsLegendHero();

	void			SetMasterSkillPointUp();

	DWORD			GetJobType(int nJob = NULL_ID) { if (nJob == NULL_ID) nJob = m_nJob;	return prj.m_aJob[nJob].dwJobType; }

	void			AngelMoveProcess();
#ifdef __EVE_BALLOON
	void			BalloonMoveProcess();
#endif //__EVE_BALLOON

	void			OnAttackSFX(OBJID	idTarget, int nMagicPower, DWORD dwSkill, int nDmgCnt, float fDmgAngle, float fDmgPower, DWORD dwAtkFlags, BOOL bControl);
	BOOL			IsGuildCombatTarget(CMover* pMover);
#ifdef __WORLDSERVER
	void	ProcessSFXDamage(void);
	void	ProcessSFXExpire(void);
#endif	// __WORLDSERVER

	void	ProcessPet(void);

#ifdef __WORLDSERVER
	void			SetDestObj(OBJID idObj, float fRange = 0.0f, BOOL bSend = FALSE);
#else
	void			SetDestObj(OBJID idObj, float fRange = 0.0f, BOOL bSend = TRUE);
#endif

#if defined(__WORLDSERVER)
	BOOL			IsPVPInspection(CMover* pMover, int nFlag = 0);
	void			AddExperienceKillMember(CMover* pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue);
	void			AddExperienceSolo(EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, BOOL bParty);
	void			AddExperienceParty(CMover* pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize);
	void			AddExperiencePartyContribution(CMover* pDead, CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10);
	void			AddExperiencePartyLevel(CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10);
	BOOL			GetPartyMemberFind(CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize);
	float			GetExperienceReduceFactor(int nLevel, int nMaxLevel);

	void			ArrowDown();
	int				GetQueueCastingTime();
	BOOL			DoUseSkill(int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl);
	BOOL			DoUseObject(OBJID idFocusObj);

	void			SubAroundExp(CMover* pAttacker, float fRange);
	void			AddPartyMemberExperience(CUser* pUser, EXPINTEGER nExp, int nFxp);

	BOOL			CreateItem(CItemElem* pItemElem, int nType = TYPE_INVENTORY);
	void			RemoveItem(BYTE nId, short nNum, int nType = TYPE_INVENTORY);
	BOOL			DoUseItemSexChange(int nFace);
	BOOL			ReplaceInspection(REGIONELEM* pPortkey);
#ifdef __LAYER_1015
	BOOL			Replace(u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3& vPos, REPLACE_TYPE type, int nLayer);
#else	// __LAYER_1015
	BOOL			Replace(u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3& vPos, REPLACE_TYPE type);
#endif	// __LAYER_1015
	BOOL			IsLoot(CItem* pItem, BOOL bPet = FALSE);
	void			ProcInstantBanker();
	BOOL			IsItemRedyTime(ItemProp* pItemProp, OBJID dwObjid, BOOL bItemFind);
	int				ApplyDPC(int ATK, ATTACK_INFO* pInfo);
	int				CalcLinkAttackDamage(int nDamage);
	int				CalcGenericDamage(CMover* pDefender, DWORD& dwAtkFlags);
	int				GetMagicSkillPower(ATTACK_INFO* pInfo);
	void			SubSMMode();
	void			ClearAllSMMode();
	BOOL			SetSMMode(int nType, DWORD dwTime);
	void			SetMarkingPos();
	void			RemoveSFX(OBJID idTarget, int id, BOOL bForce, DWORD dwSkill);
	int				GetSFXCount(OBJID idTarget);
	void			IncSFXCount(OBJID idTarget, DWORD dwSkill);
	void			AddSFXInfo(OBJID idTarget, SFXHIT_INFO& info);
	void			ClearSFX(OBJID idTarget);
	DWORD			GetSummonState(BOOL bIsSummoner);
	BOOL			IsDoUseBuff(ItemProp* pItemProp);

#endif // __WORLDSERVER

#ifdef __CLIENT
	virtual void	Render(LPDIRECT3DDEVICE9 pd3dDevice);
	BOOL			IsStateDbuff();
	BOOL			EndMotion();
	BOOL			IsPVPAttackAble(CMover* pMover);
	CSfx* CreateSfxArrow(DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget);
	void			CreateAbilityOption_SetItemSFX(int nAbilityOption);
	BOOL			GetEquipFlag(int nParts, BYTE* pbyFlag);
	void			OverCoatItemRenderCheck(CModelObject* pModel);
	void			PlayCombatMusic();
	BOOL			IsLoot(CItem* pItem) { return TRUE; }
	LPCTSTR			GetJobString();
	BOOL			DoEquipItemOther(const EQUIP_INFO& rEquipInfo, BOOL bEquip, DWORD dwPart);
	void			RenderGauge(LPDIRECT3DDEVICE9 pd3dDevice, int nValue);
	void			RenderTurboGauge(LPDIRECT3DDEVICE9 pd3dDevice, DWORD nColor, int nValue, int nMaxValue);
	void			SetRenderPartsEffect(int nParts);
	void			RenderPartsEffect(LPDIRECT3DDEVICE9 pd3dDevice);
	void			RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont);
	void			RenderHP(LPDIRECT3DDEVICE9 pd3dDevice);
#if __VER >= 11 // __SYS_COLLECTING
	void			RenderCltGauge(LPDIRECT3DDEVICE9 pd3dDevice);
#endif
	void			RenderChrState(LPDIRECT3DDEVICE9 pd3dDevice);
	void			RenderCasting(LPDIRECT3DDEVICE9 pd3dDevice);
#if __VER >= 10
	void			RenderSkillCasting(LPDIRECT3DDEVICE9 pd3dDevice);
#endif
	void			RenderQuestEmoticon(LPDIRECT3DDEVICE9 pd3dDevice);
	void			RenderGuildNameLogo(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor);
	BOOL			DoUseSkill(int nType, int nIdx, OBJID idFocusObj, SKILLUSETYPE sutType, BOOL bControl, const int nCastingTime, DWORD dwSkill = 0, DWORD dwLevel = 0);
#endif	// __CLIENT

	int				GetSkillLevel(SKILL* pSkill);
	BOOL			SetExperience(EXPINTEGER nExp1, int nLevel);

	BOOL	IsUsing(CItemElem* pItemElem);

	BOOL	IsShoutFull(void);
	BOOL	IsCommBank(void);

private:
#ifdef __WORLDSERVER
	void			ApplyNpcItems();
#endif
	void			ProcessMove();
	void			ProcessMoveArrival(CCtrl* pObj);
	void			ProcessRegion();
	void			ProcessQuest();
	void			ProcessActionPoint();
#ifdef __CLIENT
	void			ProcessScaleSlerp();
	void			ProcessWaterCircle(const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vPos);
	void			ProcessETC();
#else
	void			ProcessScript();
	void			ProcessAbnormalState();
	void			ProcessTarget();
#endif // __CLIENT

	BOOL			DropItem(CMover* pAttacker);

	void			OnArriveAtPos();
	void			OnArrive(DWORD dwParam1, DWORD dwParam2);

	void			SetActParam(OBJACT act, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0, int nParam5 = 0);
	int				GetActParam(int nIdx) { return m_nAParam[nIdx]; }

	void			SetCmd(OBJACT cmd, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	void			SetCmdParam(int nIdx, int nValue) { m_nCParam[nIdx] = nValue; }
	BOOL			__SetQuest(LPQUEST lpQuest, LPQUEST lpNewQuest);

public:
#ifdef __CLIENT
	void			CreateAngelParticle(D3DXVECTOR3 vPos);
	void			WingMotionSetting(const CModelObject* pModel);
	float			GetRideFrameSpeed(void);
#endif //__CLIENT

	bool CheckUserGold(int nGold, bool bAdd);

	void UpdateElementResist(CItemElem* pItemElem, BOOL bSet);

#ifdef __MODEL_HIDEFASH
	BOOL m_bHideFashion[6];
	BOOL	IsOptionRenderCostume(int nCostumeIdx);
#endif

#ifdef __FL_EQUALIZED_MODE
#ifdef __WORLDSERVER
	BOOL TemporalPreCheck(DWORD dwWorld);
	void TemporalEnter(int nType, int nClass);
	void TemporalOut(BOOL bSave = TRUE);
	void DisableAllPets();
	void DestroyEquipBonuses();
	void UpdateTemporalCounter(BOOL bAdd);
#endif
#endif

#ifdef __JOB_CHANGE
	void ChangeJob(int nJob);
#endif

	bool pushedByAnotherPower(const float angle, const float power);

	int GetGeneralSkillPoint(const DWORD dwJob, const DWORD dwLevel);
	int GetTotalSkillPoint(const DWORD dwJob, const DWORD dwLevel);
	DWORD GetSubLineJob(const DWORD dwJob);

	BOOL IsPossibleUseItem();

	void ClearCastingSkills(BOOL bSend = FALSE);

	BOOL IsIgnoreCollisionSkill(DWORD dwSkill);

#ifdef __WORLDSERVER
	void RefreshGrilledEel();
#endif

#ifdef __FL_VENDORS
#ifdef __WORLDSERVER
private:
	DWORD m_dwVendorTime;
	DWORD m_dwVendorItem;
public:
	void SetVendorTime(DWORD dwTime) { m_dwVendorTime = dwTime; };
	DWORD GetVendorTime() { return m_dwVendorTime; };
	void SetVendorItemId(DWORD dwItem) { m_dwVendorItem = dwItem; };
	DWORD GetVendorItemId() { return m_dwVendorItem; };
	void ShutdownVendor(BOOL bForceAll = FALSE);
#endif
#endif

private:
	AutoSynchronizer	m_autoSynchronizer;
public:
	AutoSynchronizer* AutoSynchronizer() { return &m_autoSynchronizer; }
	virtual D3DXVECTOR3		GetScrPos() { return AutoSynchronizer()->GetSyncPos(); }
	float	GetScrAngle() { return AutoSynchronizer()->GetSyncAngle(); }
	float	GetScrAngleX() { return AutoSynchronizer()->GetSyncAngleX(); }
#ifdef __WORLDSERVER
	void	ProcessMoveArrivalBySynchronizer();
#endif // __WORLDSERVER

#ifndef __VM_0820
#ifndef __MEM_TRACE
	static			CMoverPool* m_pPool;
	void* operator new(size_t nSize) { return CMover::m_pPool->Alloc(); }
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine) { return CMover::m_pPool->Alloc(); }
	void			operator delete(void* lpMem) { CMover::m_pPool->Free((CMover*)lpMem); }
	void			operator delete(void* lpMem, LPCSTR lpszFileName, int nLine) { CMover::m_pPool->Free((CMover*)lpMem); }
#endif	// __MEM_TRACE
#endif	// __VM_0820
};



inline	BOOL CMover::IsMode(DWORD dwMode)
{
	switch (dwMode)
	{
	case MATCHLESS_MODE:
	case ONEKILL_MODE:
	case MATCHLESS2_MODE:
		if (m_dwAuthorization == AUTH_GENERAL)
			return FALSE;
		break;
	}

	if ((dwMode & TRANSPARENT_MODE))
		if (GetAdjParam(DST_CHRSTATE) & CHS_INVISIBILITY)
			return TRUE;
	return ((m_dwMode & dwMode) == dwMode) ? TRUE : FALSE;
}

inline	BOOL CMover::IsStateMode(DWORD dwMode)
{
	return ((m_dwStateMode & dwMode) == dwMode) ? TRUE : FALSE;
}

inline BOOL CMover::IsFly()
{
	return m_pActMover->IsFly();
}

inline int	CMover::DoAttackMelee(OBJID idTarget, OBJMSG dwMsg, DWORD dwItemID)
{
	CMover* pTarget = (CMover*)prj.GetMover(idTarget);
	if (IsInvalidObj(pTarget))
		return 0;

	return DoAttackMelee(pTarget, dwMsg, dwItemID);
}

#if !defined(__WORLDSERVER)
inline int CMover::IsSteal(OBJID idTaget)
{
	CMover* pTarget = prj.GetMover(idTaget);
	if (IsInvalidObj(pTarget))
		return 0;
	else
		return 	IsSteal(pTarget);
}
#endif	// __WORLDSERVER


inline int CMover::SendActMsg(OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4)
{
	if (m_pActMover)
		return m_pActMover->SendActMsg(dwMsg, nParam1, nParam2, nParam3, nParam4);
	else
		return 0;
}

inline int CMover::GetGold()
{
	if (m_dwGold > INT_MAX)
		Error("GetGold(). UserId:[%07d], Gold:[%d]", m_idPlayer, m_dwGold);

	int nGold = m_dwGold;
	return nGold;
}

inline void CMover::SetGold(int nGold)
{
	if (nGold < 0)
	{
		Error("SetGold(). UserId:[%07d], Gold:[%d]", m_idPlayer, nGold);
		return;
	}
	m_dwGold = (DWORD)nGold;
}

inline void CMover::ClearCmd()
{
	m_oaCmd = OBJACT_NONE;
	memset(m_nCParam, 0, sizeof(m_nCParam));
}

inline void CMover::ClearActParam()
{
	m_oaAct = OBJACT_NONE;
	memset(m_nAParam, 0, sizeof(m_nAParam));
}

extern int GetWeaponPlusDamage(int nDamage, BOOL bRandom, ItemProp* pItemProp, int nOption);
extern int __IsEndQuestCondition(CMover* pMover, int nQuestId);
extern int __IsBeginQuestCondition(CMover* pMover, int nQuestId);
extern int __IsNextLevelQuest(CMover* pMover, int nQuestId);
extern	BOOL AttackBySFX(CMover* pAttacker, SFXHIT_INFO& info);

#endif // !defined(AFX_MOVER_H__4B7B21D7_A2D3_4115_946C_68DC9045A845__INCLUDED_)