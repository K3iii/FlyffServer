#ifndef __PROJECTCMN_H__
#define	__PROJECTCMN_H__

#define	MAX_OBJARRAY			8
#define	MAX_QUICKSLOT			21
#define	MAX_EQUIPMENT			18
#ifdef __INVENTORY_168_SLOTS
#define	MAX_INVENTORY	168	
#else
#define	MAX_INVENTORY	42	
#endif
#define MAX_GUILDBANK			42
#define	MAX_BANK				42
#define	MAX_REPAIR				16
#define	MAX_TRADE				25
#define	MAX_VENDITEM			30
#define	MAX_REPAIRINGITEM		25
#define	MAX_VENDORNAME			48
#define	MAX_STUFF				( MAX_INVENTORY + MAX_QUICKSLOT )
#define	MAX_EXPLPPOINT			41
#define	MAX_EXPSKILL			21
#define	MAX_EXPCHARACTER		200
#define	MAX_FXPCHARACTER		200
#define	MAX_CHARACTER_LIST		3
#define	MAX_VENDOR_INVENTORY	100
#define	MAX_VENDOR_INVENTORY_TAB	4
#define	MAX_QUEST				100
#define	MAX_COMPLETE_QUEST		300
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
#define MAX_CHECKED_QUEST		5
#endif // __IMPROVE_QUEST_INTERFACE
#define MAX_PARTYLEVEL			10
#define MAX_ADDEXPPARTY			16
#define MAX_RENEWEVENT			8000
#define MAX_REALITEM			8
#define MAX_EVENTREALITEM		4096
#define MAX_EXPJOBLEVEL			90
#define	ITEM_KIND_MAX			100
#define RARITY_COMMON 0
#define RARITY_UNCOMMON 1
#define RARITY_RARE 2
#define RARITY_EPIC 3
#define RARITY_LEGENDARY 4


const int	MAX_PROPMOVER = 4000;

#ifndef __DBSERVER
#define MAX_AMP_STACK_COUNT		5
#endif // __DBSERVER

#define	MAX_HONOR_TITLE		150

#if __VER >= 8 // __S8_VENDOR_REVISION
#define MAX_VENDOR_REVISION		20
#endif //__S8_VENDOR_REVISION

#ifdef __FL_CONSUMABLES_STORAGE
#define MAX_CHEST 72
#endif

#ifdef __FL_PACKED_ITEMS
#define MAX_ITEMS_IN_PACKAGE 10
#endif

#define dwDestParam1	dwDestParam[0]
#define dwDestParam2	dwDestParam[1]
#define nAdjParamVal1	nAdjParamVal[0]
#define nAdjParamVal2	nAdjParamVal[1]
#define dwChgParamVal1	dwChgParamVal[0]
#define dwChgParamVal2	dwChgParamVal[1]
#define dwDestParam3	dwDestParam[2]
#define nAdjParamVal3	nAdjParamVal[2]
#define dwChgParamVal3	dwChgParamVal[2]

#define dwDestParam4	dwDestParam[3]
#define dwDestParam5	dwDestParam[4]
#define dwDestParam6	dwDestParam[5]
#define nAdjParamVal4	nAdjParamVal[3]
#define nAdjParamVal5	nAdjParamVal[4]
#define nAdjParamVal6	nAdjParamVal[5]
#define dwChgParamVal4	dwChgParamVal[3]
#define dwChgParamVal5	dwChgParamVal[4]
#define dwChgParamVal6	dwChgParamVal[5]

#if __VER >= 9
#include "defineitemkind.h"
#endif

struct tagColorText
{
	DWORD dwColor;
	TCHAR* lpszData;
};

struct ObjProp
{
	DWORD	dwID;
	TCHAR	szName[64];
	DWORD	dwType;
	DWORD	dwAI;		// AIInterface
	DWORD	dwHP;
	ObjProp()
	{
		*szName = '\0';
		dwID = dwType = dwAI = dwHP = 0;
	}
};

struct CtrlProp : ObjProp
{
	DWORD   dwCtrlKind1;
	DWORD   dwCtrlKind2;
	DWORD   dwCtrlKind3;
	DWORD   dwSfxCtrl;
	DWORD   dwSndDamage;
	CtrlProp() : ObjProp()
	{
		dwCtrlKind1 = dwCtrlKind2 = dwCtrlKind3 = dwSfxCtrl = dwSndDamage = 0;
	}

#if __VER >= 15 // __GUILD_HOUSE
	BOOL IsGuildHousingObj() { return CK1_GUILD_HOUSE == dwCtrlKind1; }
	BOOL IsHousingObj() { return CK1_HOUSING == dwCtrlKind1; }
#endif // __GUILD_HOUSE
};

struct AddSkillProp
{
	DWORD	dwID;
	DWORD	dwName;
	DWORD	dwSkillLvl;
	DWORD	dwAbilityMin;
	DWORD	dwAbilityMax;
	DWORD	dwAttackSpeed;
	DWORD	dwDmgShift;
	DWORD	nProbability;
	DWORD	dwTaunt;
	DWORD	dwDestParam[2];
	DWORD	nAdjParamVal[2];
	DWORD	dwChgParamVal[2];
	int		nDestData1[3];
	DWORD	dwActiveSkill;
	DWORD	dwActiveSkillRate;
#if __VER >= 9	// __SKILL_0706
	DWORD	dwActiveSkillRatePVP;
#endif	// __SKILL_0706
	int		nReqMp;
	int		nReqFp;
#if __VER >= 9	// __SKILL_0706
	DWORD	dwCooldown;
	DWORD	dwCastingTime;
#else	// __SKILL_0706
	DWORD	dwSkillReady;
#endif	// __SKILL_0706
	DWORD	dwSkillRange;
	DWORD	dwCircleTime;
	DWORD   dwPainTime;
	DWORD	dwSkillTime;
	int		nSkillCount;
	DWORD   dwSkillExp;
	DWORD	dwExp;
	DWORD	dwComboSkillTime;
#if __VER >= 9	// __SKILL_0706
	DWORD	dwAbilityMinPVP;
	DWORD	dwAbilityMaxPVP;
	DWORD	nProbabilityPVP;
#endif	// __SKILL_0706

	AddSkillProp()
	{
		dwID = dwName = 0;
		nProbability = 0;
		dwSkillLvl = dwAbilityMin = dwAbilityMax = dwAttackSpeed = dwDmgShift = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		nReqMp = nReqFp = 0;
		dwActiveSkillRate = 0;
#if __VER >= 9	// __SKILL_0706
		dwCastingTime = 0;
		dwAbilityMinPVP = 0;
		dwAbilityMaxPVP = 0;
		dwCooldown = 0;
		nProbabilityPVP = 0;
		dwActiveSkillRatePVP = 0;
#else	// __SKILL_0706
		dwSkillReady = 0;
#endif	// __SKILL_0706
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

struct PartySkillProp
{
	DWORD	dwID;
	DWORD	dwName;
	DWORD	dwSkillLvl;
	DWORD	dwAbilityMin;
	DWORD	dwAtkAbilityMax;
	DWORD	dwAttackSpeed;
	DWORD	dwDmgShift;
	DWORD	dwGroggy;
	DWORD	dwTaunt;
	DWORD	dwDestParam[2];
	DWORD	nAdjParamVal[2];
	DWORD	dwChgParamVal[2];
	DWORD	dwReqMp;
	DWORD	dwReqFp;
	DWORD	dwSkillReady;
	DWORD	dwCircleTime;
	DWORD	dwSkillTime;
	DWORD	dwExp;
	DWORD	dwComboSkillTime;
	PartySkillProp()
	{
		dwID = dwName = 0;
		dwSkillLvl = dwAbilityMin = dwAtkAbilityMax = dwAttackSpeed = dwDmgShift = dwGroggy = dwTaunt = 0;
		dwDestParam[0] = dwDestParam[1] = nAdjParamVal[0] = nAdjParamVal[1] = dwChgParamVal[0] = dwChgParamVal[1] = 0;
		dwReqMp = dwReqFp = dwSkillReady = 0;
		dwCircleTime = dwSkillTime = dwExp = dwComboSkillTime = 0;
	}
};

// Item Property Type
enum IP_TYPE
{
	IP_FLAG_NONE = 0x00,
	IP_FLAG_BINDS = 0x01,
	IP_FLAG_UNDESTRUCTABLE = 0x02
#if __VER >= 14 // __EQUIP_BIND
	, IP_FLAG_EQUIP_BIND = 0x04
#endif // __EQUIP_BIND
	//	0x04
};

#ifdef __FL_BOXES_V2
enum BOXES_TYPES
{
	BOX_TYPE_NOTBOX,
	BOX_TYPE_PACKITEM,
	BOX_TYPE_GIFTBOX,
	BOX_TYPE_SELECTBOX
};
#endif

struct ItemProp : CtrlProp
{
	DWORD	dwMotion;
	DWORD	dwNum;
	DWORD	dwPackMax;
	DWORD	dwItemKind1;
	DWORD	dwItemKind2;
	DWORD	dwItemKind3;
	DWORD	dwItemJob;
	BOOL	bPermanence;
	DWORD	dwUseable;
	DWORD	dwItemSex;
	DWORD	dwCost;
	DWORD	dwEndurance;
	int		nLog;
	int		nAbrasion;
	int		nMaxRepair;
	DWORD	dwHanded;
	DWORD	dwFlag;
	DWORD	dwParts;
	DWORD	dwPartsub;
	DWORD	bPartsFile;
	DWORD	dwExclusive;
	DWORD	dwBasePartsIgnore;
	DWORD	dwItemLV;
	DWORD	dwItemRare;
	DWORD   dwShopAble;
	int		nShellQuantity;
	DWORD	dwActiveSkillLv;
	DWORD   dwFuelRe;
	DWORD	dwAFuelReMax;
	DWORD	dwSpellType;
	DWORD	dwLinkKindBullet;
	DWORD	dwLinkKind;
	DWORD	dwAbilityMin;
	DWORD	dwAbilityMax;
	SAI79::ePropType	eItemType;
	short	wItemEatk;
	DWORD   dwParry;
	DWORD   dwblockRating;
	int		nAddSkillMin;
	int		nAddSkillMax;
	DWORD	dwAtkStyle;
	DWORD	dwWeaponType;
	DWORD	dwItemAtkOrder1;
	DWORD	dwItemAtkOrder2;
	DWORD	dwItemAtkOrder3;
	DWORD	dwItemAtkOrder4;
	DWORD	tmContinuousPain;
	DWORD	dwRecoil;
	DWORD	dwLoadingTime;
	LONG	nAdjHitRate;
	FLOAT	fAttackSpeed;
	DWORD	dwDmgShift;
	DWORD	dwAttackRange;
	int		nProbability;

	DWORD	dwDestParam[PROP_BONUSES];
	LONG	nAdjParamVal[PROP_BONUSES];
	DWORD	dwChgParamVal[PROP_BONUSES];

	int		nDestData1[3];
	DWORD	dwActiveSkill;
	DWORD	dwActiveSkillRate;
	DWORD	dwReqMp;
	DWORD	dwReqFp;
	DWORD	dwReqDisLV;
	DWORD   dwReSkill1;
	DWORD   dwReSkillLevel1;
	DWORD   dwReSkill2;
	DWORD   dwReSkillLevel2;
	DWORD	dwSkillReadyType;
	DWORD	dwSkillReady;
	DWORD	_dwSkillRange;
	DWORD	dwSfxElemental;
	DWORD	dwSfxObj;
	DWORD	dwSfxObj2;
	DWORD	dwSfxObj3;
	DWORD	dwSfxObj4;
	DWORD	dwSfxObj5;
	DWORD	dwUseMotion;
	DWORD	dwCircleTime;
	DWORD	dwSkillTime;
	DWORD	dwExeTarget;
	DWORD	dwUseChance;
	DWORD	dwSpellRegion;
	DWORD   dwReferStat1;
	DWORD   dwReferStat2;
	DWORD   dwReferTarget1;
	DWORD   dwReferTarget2;
	DWORD   dwReferValue1;
	DWORD   dwReferValue2;
	DWORD	dwSkillType;
	int		nItemResistElecricity;
	int		nItemResistDark;
	int		nItemResistFire;
	int		nItemResistWind;
	int		nItemResistWater;
	int		nItemResistEarth;
	LONG	nEvildoing;
	DWORD	dwExpertLV;
	DWORD	dwExpertMax;
	DWORD	dwSubDefine;
	DWORD	dwExp;
	DWORD	dwComboStyle;
	FLOAT	fFlightSpeed;
	FLOAT	fFlightLRAngle;
	FLOAT	fFlightTBAngle;
	DWORD	dwFlightLimit;
	DWORD	dwFFuelReMax;
	DWORD	dwLimitLevel1;
	int		nReflect;
	DWORD	dwSndAttack1;
	DWORD	dwSndAttack2;
	DWORD	dwQuestId;
	TCHAR	szTextFileName[64];

#ifdef __CLIENT
	TCHAR	szIcon[64];
	TCHAR	szCommand[256];
#endif
	int		nVer;
#ifdef __FL_BOXES_V2
	BYTE nBoxType;
#endif

	DWORD	GetCoolTime() { return dwSkillReady; }
	BOOL	IsUltimate(void)
	{
		return(
			(
				dwItemKind2 == IK2_WEAPON_DIRECT
				|| dwItemKind2 == IK2_WEAPON_MAGIC
				)
			&& dwReferStat1 == WEAPON_ULTIMATE
			);
	}

	BOOL	IsVisPet() { return (dwItemKind3 == IK3_PET) && (dwReferStat1 == PET_VIS); }
	BOOL	IsVis() { return (dwItemKind3 == IK3_VIS); }

#ifndef __DBSERVER
	BOOL IsExperienceScroll(void) { return(dwItemKind3 == IK3_EXP_RATE); }
#endif // __DBSERVER

#if defined (__WORLDSERVER) || defined (__CLIENT)
	BOOL IsNeedTarget() { return dwExeTarget == EXT_ITEM; }

	BOOL IsGiftBoxItem()
	{
		for (int i = 0; i < PROP_BONUSES; i++)
		{
			if (dwDestParam[i] == DST_GIFTBOX)
				return TRUE;
		}

		return FALSE;
	}
#endif

	BOOL IsEleRefineryAble()
	{
		if (dwItemKind3 == IK3_SUIT
			|| dwItemKind3 == IK3_SHIELD
#if __VER >= 16
			|| dwItemKind3 == IK3_MAGICBARUNA
			|| dwItemKind3 == IK3_ZEMBARUNA
#endif
			|| dwItemKind2 == IK2_WEAPON_MAGIC
			|| dwItemKind2 == IK2_WEAPON_DIRECT)
		{
			return TRUE;
		}

		return FALSE;
	}
};



enum JOB_PROP_TYPE
{
	JOB_PROP_SWD,
	JOB_PROP_AXE,
	JOB_PROP_STAFF,
	JOB_PROP_STICK,
	JOB_PROP_KNUCKLE,
	JOB_PROP_WAND,
	JOB_PROP_BLOCKING,
	JOB_PROP_YOYO,
	JOB_PROP_CRITICAL,
};


struct JobProp
{
	float	fAttackSpeed;
	float	fFactorMaxHP;
	float	fFactorMaxMP;
	float	fFactorMaxFP;
	float   fFactorDef;
	float	fFactorHPRecovery;
	float	fFactorMPRecovery;
	float	fFactorFPRecovery;
	float	fMeleeSWD;
	float	fMeleeAXE;
	float   fMeleeSTAFF;
	float   fMeleeSTICK;
	float   fMeleeKNUCKLE;
	float   fMagicWAND;
	float   fBlocking;
	float	fMeleeYOYO;
	float   fCritical;
};

/*----------------------------------------------------------------------------------------------------*/
enum DROPTYPE
{
	DROPTYPE_NORMAL,
	DROPTYPE_SEED,
};

typedef struct	tagDROPITEM
{
	DROPTYPE	dtType;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwLevel;
	DWORD	dwNumber;
	DWORD	dwNumber2;
}
DROPITEM, * LPDROPITEM;

typedef	struct	tagDROPKIND
{
	DWORD	dwIK3;
	short	nMinUniq;
	short	nMaxUniq;
}
DROPKIND, * LPDROPKIND;

typedef	struct	tagQUESTITEM
{
	DWORD	dwQuest;
	DWORD   dwState;
	DWORD	dwIndex;
	DWORD	dwProbability;
	DWORD	dwNumber;
}
QUESTITEM, * PQUESTITEM;

typedef struct tagEVENTITEM
{
	DWORD	dwIndex;
	DWORD	dwPrabability;
}
EVENTITEM, * PEVENTITEM;

#define MAX_QUESTITEM	16
#define	MAX_DROPKIND	80

class CDropItemGenerator
{
private:
	vector<DROPITEM>	m_dropItems;

public:
	DWORD				m_dwMax;

public:
	//	Contructions
	CDropItemGenerator() { m_dwMax = 0; }
	virtual	~CDropItemGenerator() { Clear(); }

	//	Operations
	void		AddTail(CONST DROPITEM& rDropItem, const char* s);
	void	Clear(void) { m_dropItems.clear(); }
	DWORD		GetSize(void) { return m_dropItems.size(); }
	DROPITEM* GetAt(int nIndex, BOOL bUniqueMode, float fProbability = 0.0f);
};

class CDropKindGenerator
{
private:
	int		m_nSize;
	DROPKIND	m_aDropKind[MAX_DROPKIND];
public:
	//	Contructions
	CDropKindGenerator()
	{
		m_nSize = 0;
	}
	virtual	~CDropKindGenerator() {}
	//	Operations
	void	AddTail(const DROPKIND& rDropKind);	// memcpy
	int		GetSize(void) { return m_nSize; }
	LPDROPKIND	GetAt(int nIndex);
};

class CQuestItemGenerator
{
private:
	u_long	m_uSize;
	QUESTITEM	m_pQuestItem[MAX_QUESTITEM];
public:
	CQuestItemGenerator()
	{
		m_uSize = 0;
	}
	~CQuestItemGenerator() {}
	void	AddTail(const QUESTITEM& rQuestItem);
	u_long	GetSize(void) { return m_uSize; }
	QUESTITEM* GetAt(int nIndex);
};

#if __VER >= 14 // __INSTANCE_DUNGEON
struct MonsterTransform
{
	float fHPRate;
	DWORD dwMonsterId;
	MonsterTransform() : fHPRate(0.0f), dwMonsterId(NULL_ID) {}
};
#endif // __INSTANCE_DUNGEON
/*----------------------------------------------------------------------------------------------------*/

struct MoverProp : CtrlProp
{
	DWORD	dwStr;
	DWORD	dwSta;
	DWORD	dwDex;
	DWORD	dwInt;
	DWORD	dwHR;
	DWORD   dwER;
	DWORD	dwRace;
	DWORD	dwBelligerence;
	DWORD	dwGender;
	DWORD	dwLevel;
	DWORD	dwFlightLevel;
	DWORD	dwSize;
	DWORD   dwClass;
	BOOL	bIfParts;

#ifdef __S1108_BACK_END_SYSTEM
	DWORD	dwUseable;
#else // __S1108_BACK_END_SYSTEM
	DWORD	dwDefExtent;
#endif // __S1108_BACK_END_SYSTEM
	DWORD	dwActionRadius;
	DWORD	dwAtkMin;
	DWORD	dwAtkMax;
	DWORD	dwAtk1;
	DWORD	dwAtk2;
	DWORD	dwAtk3;
	DWORD	dwAtk4;
#if __VER >= 9	//__AI_0509
	FLOAT	fFrame;
	DWORD	dwOrthograde;
#else	// __AI_0509
	DWORD	dwVerticalRate;
	DWORD	dwDiagonalRate;
#endif	// __AI_0509
	DWORD	dwThrustRate;

	DWORD	dwChestRate;
	DWORD	dwHeadRate;
	DWORD	dwArmRate;
	DWORD	dwLegRate;

	DWORD	dwAttackSpeed;
	DWORD	dwReAttackDelay;
	DWORD	dwAddHp;		// ,
	DWORD	dwAddMp;		// ,
	DWORD	dwNaturalArmor;
	int		nAbrasion;
	int		nHardness;
	DWORD	dwAdjAtkDelay;

	SAI79::ePropType	eElementType;
	short				wElementAtk;

	DWORD	dwHideLevel;
	FLOAT	fSpeed;
	DWORD	dwShelter;
	DWORD	dwFlying;
	DWORD	dwJumpIng;
	DWORD	dwAirJump;
	DWORD	bTaming;
	DWORD	dwResisMgic;

	int		nResistElecricity;
	int		nResistDark;
	int		nResistFire;
	int		nResistWind;
	int		nResistWater;
	int		nResistEarth;

	DWORD	dwCash;
	DWORD	dwSourceMaterial;
	DWORD	dwMaterialAmount;
	DWORD	dwCohesion;
	DWORD	dwHoldingTime;
	DWORD	dwCorrectionValue;
	EXPINTEGER	nExpValue;
	int		nFxpValue;
	DWORD	nBodyState;
	DWORD	dwAddAbility;
	DWORD	bKillable;

	DWORD	dwVirtItem[3];
	DWORD	bVirtType[3];

	DWORD   dwSndAtk1;
	DWORD   dwSndAtk2;
	DWORD   dwSndDie1;
	DWORD   dwSndDie2;
	DWORD   dwSndDmg1;
	DWORD   dwSndDmg2;
	DWORD   dwSndDmg3;
	DWORD   dwSndIdle1;
	DWORD   dwSndIdle2;

	short   m_nEvasionHP;
	short	m_nEvasionSec;
	short   m_nRunawayHP;
	short   m_nCallHelperMax;
	short   m_nCallHP;
	short   m_nCallHelperIdx[5];
	short   m_nCallHelperNum[5];
	short   m_bCallHelperParty[5];

	short   m_dwAttackMoveDelay;
	short   m_dwRunawayDelay;
	short   m_bPartyAttack;
	short   m_nHelperNum;

#if !defined(__CORESERVER)
	int		m_nScanJob;

	int		m_nHPCond;
	int		m_nLvCond;
	int		m_nRecvCondMe;
	int		m_nRecvCondHow;
	int		m_nRecvCondMP;
	BYTE	m_bMeleeAttack;
	BYTE	m_bRecvCondWho;
	BYTE	m_bRecvCond;
	BYTE	m_bHelpWho;
	BYTE	m_bRangeAttack;
	int		m_nSummProb;
	int		m_nSummNum;
	int		m_nSummID;
	int		m_nHelpRangeMul;
	DWORD	m_tmUnitHelp;
	int		m_nBerserkHP;
	float	m_fBerserkDmgMul;
	DWORD   m_dwScanQuestId;
	DWORD   m_dwScanItemIdx;
	int		m_nScanChao;
#endif // !__CORESERVER

#ifdef __S1108_BACK_END_SYSTEM
	float	m_fHitPoint_Rate;
	float	m_fAttackPower_Rate;
	float	m_fDefence_Rate;
	float	m_fExp_Rate;
	float	m_fItemDrop_Rate;
	float	m_fPenya_Rate;
	BOOL	m_bRate;
#endif // __S1108_BACK_END_SYSTEM


	short	m_nAttackItemNear;
	short	m_nAttackItemFar;
	short	m_nAttackItem1;
	short	m_nAttackItem2;
	short	m_nAttackItem3;
	short	m_nAttackItem4;
	short	m_nAttackItemSec;
	short	m_nMagicReflection;
	short	m_nImmortality;
	BOOL 	m_bBlow;
	short	m_nChangeTargetRand;

	short   m_nAttackFirstRange;
	CDropItemGenerator	m_DropItemGenerator;
	CQuestItemGenerator		m_QuestItemGenerator;
	CDropKindGenerator	m_DropKindGenerator;
#if __VER >= 14 // __INSTANCE_DUNGEON
	MonsterTransform	m_MonsterTransform;
#endif // __INSTANCE_DUNGEON

	MoverProp()
	{
		dwStr = dwSta = dwDex = dwInt = dwHR = dwER = dwRace = dwBelligerence = dwGender
			= dwLevel = dwFlightLevel = dwSize = dwClass = bIfParts
#ifdef __S1108_BACK_END_SYSTEM
			= dwUseable
#else // __S1108_BACK_END_SYSTEM
			= dwDefExtent
#endif // __S1108_BACK_END_SYSTEM
			= dwActionRadius = dwAtkMin = dwAtkMax = dwAtk1 = dwAtk2 = dwAtk3 = dwAtk4 = 0;
#if __VER >= 9	//__AI_0509
		fFrame = 1.0F;
		dwOrthograde = 0;
#else	// __AI_0509
		dwVerticalRate = dwDiagonalRate = 0;
#endif	// __AI_0509
		dwThrustRate = 0;
		dwChestRate = 0;
		dwHeadRate = 0;
		dwArmRate = 0;
		dwLegRate = 0;
		dwAttackSpeed = 0;
		dwReAttackDelay = 0;
		dwAddHp = 0;
		dwAddMp = 0;
		dwNaturalArmor = 0;
		nAbrasion = 0;
		nHardness = 0;
		dwAdjAtkDelay = 0;
		dwHideLevel = 0;
		eElementType = SAI79::NO_PROP;
		wElementAtk = 0;
		fSpeed = 0.0F;
		dwShelter = 0;
		dwFlying = 0;
		dwJumpIng = 0;
		dwAirJump = 0;
		bTaming = 0;
		dwResisMgic = 0;
		nResistElecricity = 0;
		nResistDark = 0;
		nResistFire = 0;
		nResistWind = 0;
		nResistWater = 0;
		nResistEarth = 0;
		dwCash = 0;
		dwSourceMaterial = 0;
		dwMaterialAmount = 0;
		dwCohesion = 0;
		dwHoldingTime = 0;
		dwCorrectionValue = 0;
		nExpValue = 0;
		nFxpValue = 0;
		nBodyState = 0;
		dwAddAbility = 0;
		bKillable = 0;

		memset(dwVirtItem, 0, sizeof(dwVirtItem));
		memset(bVirtType, 0, sizeof(bVirtType));

		dwSndAtk1 = 0;
		dwSndAtk2 = 0;
		dwSndDie1 = 0;
		dwSndDie2 = 0;
		dwSndDmg1 = 0;
		dwSndDmg2 = 0;
		dwSndDmg3 = 0;
		dwSndIdle1 = 0;
		dwSndIdle2 = 0;
		m_nEvasionHP = 0;
		m_nEvasionSec = 0;
		m_nRunawayHP = 0;
		m_nCallHelperMax = 0;
		m_nCallHP = 0;

		memset(m_nCallHelperIdx, 0, sizeof(m_nCallHelperIdx));
		memset(m_nCallHelperNum, 0, sizeof(m_nCallHelperNum));
		memset(m_bCallHelperParty, 0, sizeof(m_bCallHelperParty));

		m_dwAttackMoveDelay = 0;
		m_dwRunawayDelay = 0;
		m_bPartyAttack = 0;
		m_nHelperNum = 0;

#if !defined(__CORESERVER)
		m_nScanJob = 0;
		m_nHPCond = 0;
		m_nLvCond = 0;
		m_nRecvCondMe = 0;
		m_nRecvCondHow = 0;
		m_nRecvCondMP = 0;
		m_bMeleeAttack = 0;
		m_bRecvCondWho = 0;
		m_bRecvCond = 0;
		m_bHelpWho = 0;
		m_bRangeAttack = 0;
		m_nSummProb = 0;
		m_nSummNum = 0;
		m_nSummID = 0;
		m_nHelpRangeMul = 0;
		m_tmUnitHelp = 0;
		m_nBerserkHP = 0;
		m_fBerserkDmgMul = 0;
		m_dwScanQuestId = 0;
		m_dwScanItemIdx = 0;
		m_nScanChao = 0;
#endif // !__CORESERVER

#ifdef __S1108_BACK_END_SYSTEM
		m_fHitPoint_Rate =
			m_fAttackPower_Rate =
			m_fDefence_Rate =
			m_fExp_Rate =
			m_fItemDrop_Rate =
			m_fPenya_Rate = 0.0F;
		m_bRate =
#endif // __S1108_BACK_END_SYSTEM
			m_nAttackItemNear =
			m_nAttackItemFar =
			m_nAttackItem1 =
			m_nAttackItem2 =
			m_nAttackItem3 =
			m_nAttackItem4 =
			m_nAttackItemSec =
			m_nMagicReflection =
			m_nImmortality =
			m_bBlow =
			m_nChangeTargetRand =
			m_nAttackFirstRange = 0;
	}
};

typedef	struct	tagBeginItem
{
	DWORD	dwItemType;
	DWORD	dwItemId;
	DWORD	dwNum;
}
BEGINITEM, * LPBEGINITEM;

#define MAX_BEGINEQUIP  10
#define MAX_BEGINITEM   42 //20
#define MAX_JOBITEM     10

typedef struct tagJOBITEM
{
	DWORD adwMale[MAX_BEGINEQUIP][2];
	BEGINITEM beginItem[MAX_BEGINITEM];

} JOBITEM, * LPJOBITEM;

#define	MAX_BEGINPOS	16

typedef	struct tagBEGINPOSARR
{
	u_long	uSize;
	D3DXVECTOR3 avPos[MAX_BEGINPOS];
}
BEGINPOSARR, * LPBEGINPOSARR;

#define TASKBAR_TOP    0
#define TASKBAR_BOTTOM 1
#define TASKBAR_LEFT   2
#define TASKBAR_RIGHT  3

// short cut
#define MAX_SLOT_APPLET			18
#define MAX_SLOT_ITEM			9
#define MAX_SLOT_QUEUE			5
#if __VER >= 11 // __CSC_VER11_5
#define	MAX_SLOT_ITEM_COUNT		8
#else
#define	MAX_SLOT_ITEM_COUNT		4
#endif //__CSC_VER11_5


#define	SHORTCUT_NONE    0
#define	SHORTCUT_ETC     1
#define	SHORTCUT_APPLET  2
#define	SHORTCUT_MOTION  3
#define	SHORTCUT_SCRIPT  4
#define	SHORTCUT_ITEM    5
//#define	SHORTCUT_CARD    5
//#define	SHORTCUT_JACKBOX 6
#define	SHORTCUT_SKILL   6
#define SHORTCUT_OBJECT  7
#define SHORTCUT_CHAT    8
#define SHORTCUT_SKILLFUN 9
#define SHORTCUT_EMOTICON 10
#define SHORTCUT_LORDSKILL	 11
#ifdef __FL_SWITCH_EQ
#define SHORTCUT_EQ_SWITCH 12
#endif


#if defined ( __CLIENT )
class CWndBase;
#endif	// __CLIENT

typedef struct tagSHORTCUT
{
#if defined ( __CLIENT )
	CWndBase* m_pFromWnd;
	CTexture* m_pTexture;
	DWORD			m_dwItemId;
	DWORD			m_dwShortcut;
	DWORD     		m_dwId;
	DWORD     		m_dwType;
	DWORD     		m_dwIndex;
	DWORD     		m_dwUserId;
	DWORD     		m_dwData;
	int			m_nLocation;
	TCHAR     		m_szString[MAX_SHORTCUT_STRING];

	tagSHORTCUT::tagSHORTCUT(void)
		: m_pFromWnd(NULL)
		, m_pTexture(NULL)
		, m_dwItemId(NULL)
		, m_dwShortcut(SHORTCUT_NONE)
		, m_dwId(NULL)
		, m_dwType(NULL)
		, m_dwIndex(NULL)
		, m_dwUserId(NULL)
		, m_dwData(NULL)
		, m_nLocation(-1)
	{
		ZeroMemory(m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING);
	}

	void	Empty(void)
	{
		m_pFromWnd = NULL;
		m_pTexture = NULL;
		m_dwItemId = NULL;
		m_dwShortcut = SHORTCUT_NONE;
		m_dwId = NULL;
		m_dwType = NULL;
		m_dwIndex = NULL;
		m_dwUserId = NULL;
		m_dwData = NULL;
		m_nLocation = -1;

		ZeroMemory(m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING);
	}
#else

	DWORD			m_dwShortcut;
	DWORD     		m_dwId;
	DWORD     		m_dwType;
	DWORD     		m_dwIndex;
	DWORD     		m_dwUserId;
	DWORD     		m_dwData;
	int			m_nLocation;
	TCHAR     		m_szString[MAX_SHORTCUT_STRING];

	tagSHORTCUT::tagSHORTCUT(void)
		: m_dwShortcut(SHORTCUT_NONE)
		, m_dwId(NULL)
		, m_dwType(NULL)
		, m_dwIndex(NULL)
		, m_dwUserId(NULL)
		, m_dwData(NULL)
		, m_nLocation(-1)
	{
		ZeroMemory(m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING);
	}

	void	Empty(void)
	{
		m_dwShortcut = SHORTCUT_NONE;
		m_dwId = NULL;
		m_dwType = NULL;
		m_dwIndex = NULL;
		m_dwUserId = NULL;
		m_dwData = NULL;
		m_nLocation = -1;

		ZeroMemory(m_szString, sizeof(TCHAR) * MAX_SHORTCUT_STRING);
	}

#endif

	BOOL	IsEmpty(void) const { return m_dwShortcut == SHORTCUT_NONE; }

} SHORTCUT, * LPSHORTCUT;

typedef struct tagEXPPARTY
{
	DWORD	Exp;
	DWORD	Point;
}
EXPPARTY, * LPEXPPARTY;

typedef struct tagRENEWEVENT
{
	int		nLevel;
	DWORD	dwItemId;
	TCHAR	strItemName[64];
	float	fPercent;
}
RENEWEVENT, * LPRENEWEVENT;

typedef struct tagRENEWEVENTITEM
{
#ifdef __S0517
	CTime	EndTime;
#else // __S0517
	TCHAR strTime[32];
#endif // __S0517
	int nLevel;
	int	nRealitem[MAX_REALITEM];
	int nRealitemCount[MAX_REALITEM];
	int nSendRealitemCount[MAX_REALITEM];
	int nPercent[MAX_REALITEM];
	int nNextIndex;
}
RENEWEVENTITEM, * LPRENEWEVENTITEM;


typedef struct tagExpCharacter
{
	EXPINTEGER	nExp1;
	EXPINTEGER	nExp2;
	DWORD dwLPPoint;
	EXPINTEGER	nLimitExp;
}
EXPCHARACTER, * LPEXPCHARACTER;

#endif	// __PROJECTCMN_H__