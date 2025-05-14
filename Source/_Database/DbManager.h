

#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include "Query.h"
#include "DPlay.h"
#include "Ar.h"
#include "DPMng.h"
#include "mempooler.h"
#include "misc.h"
#include "..\_Network\Objects\Obj.h"
#include "AccountCacheMgr.h"

const int	MAX_GETTHREAD_SIZE = 8;
const int	MAX_PUTTHREAD_SIZE = 16;
const int	MAX_UPDATETHREAD_SIZE = 8;

const int	MAX_JOIN_SIZE = 16;
const int	MAX_TASKBAR = MAX_SHORTCUT_STRING * 2 + 44;
const int	MAX_APPLETTASKBAR = MAX_TASKBAR * MAX_SLOT_APPLET;
const int	MAX_ITEMTASKBAR = MAX_TASKBAR * MAX_SLOT_ITEM_COUNT * MAX_SLOT_ITEM;
const char	NullStr[2] = "$";

#define	QUERY_SIZE	40960

#ifdef __FL_PACKED_ITEMS
enum PACKAGE_REQUEST_TYPE
{
	PRT_INVENTORY = 1,
	PRT_BANK,
	PRT_CHEST,
	PRT_GUILDBANK,
	PRT_VENDORS
};
#endif

enum QUERYMODE
{
	DB_CONNECT, DB_THREAD_END, CREATEACCOUNT, CREATEPLAYER, LOGIN,
	REMOVEPLAYER, SEL_ECT, SENDPLAYERLIST, JOIN, SAVEPLAYER, SAVECONCURRENTUSERNUMBER,
	SAVE_PLAY_TIME, LOG_PLAY_CONNECT, LOG_ITEM, LOG_LEVELUP,
	LOG_UNIQUEITEM, LOG_QUEST,
	CREATE_GUILD, DESTROY_GUILD, ADD_GUILD_MEMBER, REMOVE_GUILD_MEMBER,
	UPDATE_GUILD_CONTRIBUTION, UPDATE_GUILD_NOTICE,
	UPDATE_GUILD_MEMBER_LEVEL, UPDATE_GUILD_AUTHORITY, GUILD_SETNAME,
	UPDATE_GUILD_REALPAY, GUILD_BANK_UPDAATE, GUILD_BANK_QUERY,
	LOG_GUILD_CREATE, LOG_GUILD_REMOVE,
	ACPT_GUILD_WAR, WAR_END, SURRENDER, WAR_DEAD, WAR_MASTER_ABSENT,
	REMOVE_FRIEND, ADD_PARTYNAME, CHANGE_BANKPASS,
	INSERT_FREQUENCY,
	UPDATE_GUILD_CLASS, UPDATE_GUILD_NICKNAME,
	QM_QUERYGUILDQUEST,
	QM_INSERTGUILDQUEST,
	QM_UPDATEGUILDQUEST,
	BS_TEST,
	QM_DELETEGUILDQUEST,
	QM_SET_PLAYER_NAME,
	START_GUILDCOMBAT,
	IN_GUILDCOMBAT,
	OUT_GUILDCOMBAT,
	RESULT_GUILDCOMBAT,
	CONTINUE_GUILDCOMBAT,
	ADD_MAIL,
	REMOVE_MAIL,
	REMOVE_MAIL_ITEM,
	REMOVE_MAIL_GOLD,
	READ_MAIL,
	QM_UPDATEGUILDMASTER,
	QM_DELETE_REMOVE_GUILD_BANK_TBL,
	QM_CALL_USPLOGGINGQUEST,
	QM_CALL_XXX_MULTI_SERVER,
#ifdef __GETMAILREALTIME
	QM_GETMAIL_REALTIME,
#endif // __GETMAILREALTIME
	QM_ADD_MESSENGER,
	QM_DELETE_MESSENGER,
	QM_UPDATE_MESSENGER,
#if __VER >= 11
	QM_GUILDBANK_LOG_VIEW,
#endif
#ifdef __JEFF_FIX_0
	QM_QUERY_MAIL_BOX,
	QM_ALL_PLAYER_DATA,
#endif	// __JEFF_FIX_0
	LOG_GUILDFURNITURE
#ifdef __FL_GW_STATUE_V2
	, GW_STATUES_INSTALL
#endif
#ifdef __FL_EQUALIZED_MODE
	, TMP_EQUIP_SAVE
	, TMP_EQUIP_GET
#endif
#ifdef __FL_DAILY_REWARDS
	, UPDATE_DAILYGIFT
#endif
#ifdef __FL_VENDORS
	, ADD_VENDOR
	, DELETE_VENDOR
	, UPDATE_VENDOR
	, GET_VENDORS
#endif
#ifdef __FL_CASH_SHOP
	, CSHOP_LOAD
	, CSHOP_BALANCE
	, CSHOP_ITEM
#endif
#ifdef __FL_GUILD_FINDER
	, UPDATE_GUILD_FINDERSTATE
#endif
};

typedef struct tagACCOUNT_INFO
{
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
	char	szPlayer[MAX_PLAYER];
	int		nPlayerSize;
	int		nPlayerSlot;
}	ACCOUNT_INFO, * LPACCOUNT_INFO;

typedef struct tagDB_OVERLAPPED_PLUS
{
	OVERLAPPED	Overlapped;
	int		nQueryMode;

	BYTE* lpBuf;

	u_long	uBufSize;
	ACCOUNT_INFO	AccountInfo;
	DPID	dpid;
	DPID	dpidCache;
	DPID	dpidUser;
	DWORD	dwAuthKey;
	tagDB_OVERLAPPED_PLUS()
	{

		lpBuf = NULL;
		uBufSize = 0;
	}
}	DB_OVERLAPPED_PLUS, * LPDB_OVERLAPPED_PLUS;

struct	MAIL_QUERYINFO
{
	LPCTSTR		pszType;
	int		nMail;
	u_long	idReceiver;
	u_long	idSender;
	int		nGold;
	time_t	tmCreate;
	BYTE	byRead;
	LPCTSTR		szTitle;
	LPCTSTR	szText;
	DWORD	dwItemId;
	int		nItemNum;
	int		nHitPoint;
	int		nMaterial;
	BYTE	byFlag;
	SERIALNUMBER	iSerialNumber;
	int		nOption;
	int		bItemResist;
	int		nResistAbilityOption;
	int		nResistSMItemId;
	DWORD	dwKeepTime;
	__int64	iRandomOptItemId;
	int		nPiercedSize;
	DWORD	dwItemId1;
	DWORD	dwItemId2;
	DWORD	dwItemId3;
	DWORD	dwItemId4;
	DWORD	dwItemId5;
	BOOL	bPet;
	BYTE	nKind;
	BYTE	nLevel;
	DWORD	dwExp;
	WORD	wEnergy;
	WORD	wLife;
	BYTE	anAvailLevel[PL_MAX];
#ifdef __PET_1024
	char	szPetName[MAX_PET_NAME];
#endif	// __PET_1024
#ifdef __FL_FIX_MAILVALUES
	BOOL	bTransformPet;
#ifdef __MODEL_CHANGE
	DWORD	nItemModel;
#endif
	int tmPetVis1;
	int tmPetVis2;
	int tmPetVis3;
	int tmPetVis4;
	int tmPetVis5;
	int tmPetVis6;
	int tmPetVis7;
	int tmPetVis8;
	int tmPetVis9;
#endif
#ifdef __FL_PACKED_ITEMS
	char	szItem[2048];
	char	szExt[1024];
	char	szPiercing[4096];
	char	szPet[2048];
#endif

	MAIL_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		nMail = 0;
		idReceiver = 0;
		idSender = 0;
		nGold = 0;
		tmCreate = 0;
		byRead = 0;
		szTitle = "";
		szText = "";
		dwItemId = 0;
		nItemNum = 0;
		nHitPoint = 0;
		nMaterial = 0;
		byFlag = 0;
		iSerialNumber = 0;
		nOption = 0;
		bItemResist = 0;
		nResistAbilityOption = 0;
		nResistSMItemId = 0;
		dwKeepTime = 0;
		iRandomOptItemId = 0;
		nPiercedSize = 0;
		dwItemId1 = 0;
		dwItemId2 = 0;
		dwItemId3 = 0;
		dwItemId4 = 0;
		dwItemId5 = 0;
		bPet = FALSE;
		nKind = nLevel = 0;
		dwExp = 0;
		wEnergy = wLife = 0;
		memset(anAvailLevel, 0, sizeof(BYTE) * PL_MAX);
#ifdef __PET_1024
		szPetName[0] = '\0';
#endif // __PET_1024
#ifdef __FL_FIX_MAILVALUES
		bTransformPet = 0;
#ifdef __MODEL_CHANGE
		nItemModel = NULL_ID;
#endif
		tmPetVis1 = 0;
		tmPetVis2 = 0;
		tmPetVis3 = 0;
		tmPetVis4 = 0;
		tmPetVis5 = 0;
		tmPetVis6 = 0;
		tmPetVis7 = 0;
		tmPetVis8 = 0;
		tmPetVis9 = 0;
#endif
#ifdef __FL_PACKED_ITEMS
		szItem[0] = '\0';
		szExt[0] = '\0';
		szPiercing[0] = '\0';
		szPet[0] = '\0';

		strcat(szItem, NullStr);
		strcat(szExt, NullStr);
		strcat(szPiercing, NullStr);
		strcat(szPet, NullStr);
#endif
	}
};


struct GUILD_QUERYINFO
{
	LPCTSTR	pszType;
	u_long	idPlayer;
	u_long	idGuild;
	LPCTSTR	pszGuild;
	DWORD   dwLv1;
	DWORD   dwLv2;
	DWORD   dwLv3;
	DWORD   dwLv4;
	WORD	nLevel;
	int     nGuildGold;
	int     nGuildPxp;
	int		nClass;
	LPCTSTR pszNotice;
#ifdef __FL_GUILD_FINDER
	BOOL bFinder;
#endif

	GUILD_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 1;
		idGuild = 1;
		pszGuild = "";
		dwLv1 = dwLv2 = dwLv3 = dwLv4 = 0;
		nLevel = 0;
		nGuildGold = 0;
		nGuildPxp = 0;
		nClass = 0;
		pszNotice = "";
#ifdef __FL_GUILD_FINDER
		bFinder = TRUE;
#endif
	};
};

struct GUILDLOG_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idGuild;
	u_long		idPlayer;
	u_long		idDoPlayer;
	int			nGuildGold;
	int			nGuildPxp;
	int			nGuildLv;
	int			nLevel;
	int 		nItem;
	int			nAbilityOption;
	int			nItem_count;
	SERIALNUMBER	iUniqueNo;
	LPCTSTR		pszTime;
	LPCTSTR		pszGuildBank;
	LPCTSTR		pszState;

	GUILDLOG_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idGuild = 0;
		idPlayer = 0;
		idDoPlayer = 0;
		nGuildGold = 0;
		nGuildPxp = 0;
		nGuildLv = 0;
		nLevel = 0;
		nItem = 0;
		nAbilityOption = 0;
		nItem_count = 0;
		iUniqueNo = 0;
		pszTime = "";
		pszGuildBank = "";
		pszState = "";
	};
};

struct ADDBANK_QUERYINFO
{
	LPCTSTR		pszType;
	u_long		idPlayer;
	LPCTSTR		pszBank;
	LPCTSTR		pszBankIndex;
	LPCTSTR		pszObjIndexBank;
	LPCTSTR		pszExtBank;
	LPCTSTR		pszPirecingBank;
	DWORD		dwGoldBank;
	LPCTSTR		pszBankPet;

	ADDBANK_QUERYINFO(LPCTSTR pszQueryType)
	{
		pszType = pszQueryType;
		idPlayer = 0;
		pszBank = "";
		pszBankIndex = "";
		pszObjIndexBank = "";
		pszExtBank = "";
		pszPirecingBank = "";
		dwGoldBank = 0;
		pszBankPet = "";
	};
};

struct ACCOUNT_CACHE
{
	CMover* pMover[3];
	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[3];
	DWORD							m_dwGoldBank[3];
	time_t							m_tmLastAccess;
	string							m_strAccount;

	void	Init();
	void	Clear();
	void	ClearMover(BYTE nSlot, u_long idPlayer);
	BOOL	IsReleaseable();
#ifdef __INVALID_LOGIN_0320
private:
	u_long	m_uMultiServer;
public:
	u_long	GetMultiServer(void) { return m_uMultiServer; }
	void	ChangeMultiServer(u_long uMultiServer) { m_uMultiServer = uMultiServer; }
	BOOL	IsPlaying(void) { return m_uMultiServer != 0; }
#endif	// __INVALID_LOGIN_0320
};

inline void ACCOUNT_CACHE::Init()
{
	m_tmLastAccess = ::time(NULL);

	for (int i = 0; i < 3; ++i)
	{
		pMover[i] = NULL;
		m_Bank[i].SetItemContainer(ITYPE_ITEM, MAX_BANK);
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
#ifdef __INVALID_LOGIN_0320
	m_uMultiServer = 0;
#endif	// __INVALID_LOGIN_0320
}


inline void ACCOUNT_CACHE::Clear()
{
	for (int i = 0; i < 3; ++i)
		SAFE_DELETE(pMover[i]);
}

inline void ACCOUNT_CACHE::ClearMover(BYTE nSlot, u_long idPlayer)
{
	m_Bank[nSlot].Clear();
	m_dwGoldBank[nSlot] = 0;
	m_idPlayerBank[nSlot] = idPlayer;
	SAFE_DELETE(pMover[nSlot]);
}

inline BOOL ACCOUNT_CACHE::IsReleaseable()
{
	time_t t = m_tmLastAccess + TIMEWAIT_RELEASE;
	if (::time(NULL) > t)
		return TRUE;
	return FALSE;
}

#define	MAX_QUERY_RESERVED	2

typedef	set<string>			SET_STRING;
typedef map<u_long, string>	ULONG2STRING;
typedef map<int, string>	INT2STRING;

struct	ItemStruct
{
	char	szItem[512];
	char	szExt[128];
	char	szPiercing[256];
	char	szPet[100];

	ItemStruct()
	{
		szItem[0] = '\0';
		szExt[0] = '\0';
		szPiercing[0] = '\0';
		szPet[0] = '\0';
	}
};

struct	ItemContainerStruct
{
#ifdef __INVENTORY_168_SLOTS
	char szItem[50000], szIndex[512 * 4], szObjIndex[512 * 4];
	char szExt[2048 * 4], szPiercing[8000 * 4], szPet[4200 * 4];
#else
	char	szItem[6144];
	char	szIndex[512];
	char	szObjIndex[512];
	char	szExt[2048];
	char	szPiercing[8000];
	char	szPet[4200];
#endif

	ItemContainerStruct()
	{
		*szItem = '\0';
		*szIndex = '\0';
		*szObjIndex = '\0';
		*szExt = '\0';
		*szPiercing = '\0';
		*szPet = '\0';
	}
};

class CGuildMng;
class CMail;

class CDbManager
{
	struct __GUILDCOMBATJOIN
	{
		DWORD dwPenya;
		BOOL  bRequest;
		u_long uGuildId;
	};
	struct __SendItemContents
	{
		char OneItem[MAX_PATH];
		int nNo, Item_Count;
		__int64 m_dwKeepTime;
		CItemElem itemElem;
	};
	struct __GCPLAYERPOINT
	{
		u_long	uidPlayer;
		int		nJob;
		int		nPoint;

		__GCPLAYERPOINT()
		{
			uidPlayer = nJob = nPoint = 0;
		}
	};

	struct __MAIL_REALTIME
	{
		int nMail_Before;
		int nMail_After;
		SERIALNUMBER m_liSerialNumber;
		int m_nHitPoint;
	};

public:
	INT2STRING		m_int2StrItemUpdate;

	CAccountCacheMgr m_AccountCacheMgr;
	CMclCritSec		m_AddRemoveLock;
	ULONG2STRING	m_2PartyNamePtr;
	CMclCritSec		m_joinLock;
	char			m_aszJoin[MAX_JOIN_SIZE][MAX_PLAYER];
	int				m_nJoin;
	CMclCritSec		m_csMapSavePlayer;
	map<u_long, int>	m_mapSavePlayer;
	HANDLE			m_hIOCPGet;
	HANDLE			m_hIOCPPut;
	HANDLE			m_hIOCPUpdate;
	HANDLE			m_hIOCPGuild;
#ifdef __S1108_BACK_END_SYSTEM
	HANDLE			m_hWorker;
	HANDLE			m_hCloseWorker;
#endif // __S1108_BACK_END_SYSTEM

	DB_OVERLAPPED_PLUS* AllocRequest(void);
	void	FreeRequest_V21(DB_OVERLAPPED_PLUS* pOverlappedPlus);
	void	MakeRequest(DB_OVERLAPPED_PLUS* pOverlappedPlus, LPBYTE lpBuf, u_long uBufSize);

	MemPooler<DB_OVERLAPPED_PLUS>* m_pDbIOData;

	vector<__GCPLAYERPOINT> m_vecGCPlayerPoint;
	vector<__GUILDCOMBATJOIN>	m_vecGuildCombat;

	int		m_nGuildCombatIndex;
	u_long	m_uWinGuildId;
	u_long	m_uBestPlayer;
	int		m_nWinGuildCount;

	char DB_ADMIN_PASS_LOG[256];
	char DB_ADMIN_PASS_CHARACTER01[256];
	char DB_ADMIN_PASS_BACKSYSTEM[256];

public:
	static CDbManager& GetInstance();
	~CDbManager();

	void	PostSavePlayer(u_long idPlayer, BYTE nSlot);
	void	SavePlayer(CQuery* qry, CQuery* pQueryLog, CMover* pMover, char* szQuery);
	void	SavePlayTime(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	void	SaveHonor(CQuery* qry, u_long uidPlayer, int* aHonor, char* szQuery);
	void	SaveSkill(CQuery* qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery);
	void	SaveJobLv(CMover* pMover, char* szJobLv);
	void	SaveQuest(CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest);

	void	SaveOneItem(CItemElem* pItemElem, ItemStruct* pItemStruct);
	void	SaveContainer(CItemContainer<CItemElem>* pContainer, ItemContainerStruct* pStruct);

	void	SaveTaskBar(CMover* pMover, char* szAppletTaskBar, char* szItemTaskBar, char* szSkillTaskBar);
	void	SaveSMCode(CMover* pMover, char* szszSMTime);
	void	SaveSkillInfluence(CMover* pMover, char* szszSkillInfluence);
	void	DBQryAddBankSave(char* szSql, const ADDBANK_QUERYINFO& info);
	void	SendPlayerList(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	void SendJoin(CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey,
		LPCTSTR szPartyName, LPCTSTR szBankPass,
		DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel,
		LPCTSTR szTimeGuild
#ifdef __FL_DAILY_REWARDS
		, BOOL bIsRewardTime, int nLastReward
#endif
#ifdef  __FL_CASH_SHOP
		, int nDonatePoints, int nVotePoints
#endif
	);

	void	Join(CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	CreatePlayer(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	RemovePlayer(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	ChangeBankPass(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	BOOL	LoadPlayerData(void);

	BOOL	GetPartyName(void);
	void	SerializeGuildCombat(CAr& ar);
	void	SerializePlayerPoint(CAr& ar);
	BOOL	OpenGuildCombat(void);
	BOOL	LoadPost(void);
	void	GetItemFromMail(CQuery* pQuery, CItemElem* pItemElem);
#ifdef __POST_1204
	void	RemoveMail(list<CMail*>& lspMail, time_t t);
#else	// __POST_1204
	void	RemoveMail(list<CMail*>& lspMail);
#endif	// __POST_1204
	CQuery* m_apQuery[MAX_QUERY_RESERVED];
	BOOL	QueryRemoveGuildBankTbl(void);
	void	DeleteRemoveGuildBankTbl(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	call_uspLoggingQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
#ifdef __GETMAILREALTIME
	BOOL	QueryGetMailRealTime(CQuery* pQuery);
#endif // __GETMAILREALTIME
	void	GuildBankLogView(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	GetMyPartyName(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char* szPartyName);

	void	CreateGuild(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	DestroyGuild(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	AddGuildMember(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	RemoveGuildMember(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	OpenGuild(void);
	void	UpdateGuildMemberLv(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildClass(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildNickName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildMaster(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildContribution(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildNotice(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildAuthority(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#ifdef __FL_GUILD_FINDER
	void	UpdateGuildFinderState(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif
	void	UpdateGuildSetName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildBankUpdate(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#ifndef __FL_PACKED_ITEMS
	void	OpenQueryGuildBank(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#else
	void	OpenQueryGuildBank(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, CQuery* pQuery1);
#endif
	void	AddPartyName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	BOOL	call_uspLoggingTrade(CQuery* pQuery, int nFlag, int nTradeID, DWORD dwWorldID = 0, u_long idPlayer = 0, DWORD dwTradeGold = 0, const char* lpAddr = "", int nLevel = 0, int nJob = 0, int nItemIndex = 0, SERIALNUMBER iItemSerialNum = 0, int nItemCnt = 0, int nAbilityOpt = 0, int nItemResist = 0, int nResistAbilityOpt = 0, __int64 iRandomOpt = 0);
	LONG	m_cbTrade;
	int		GetTradeNo(void);
	void	call_uspXXXMultiServer(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);

	BOOL	CreateDbWorkers(void);
	void	CloseDbWorkers(void);
	void	GetThread(void);
	void	PutThread(void);
	void	SPThread(void);
	void	UpdateThread(void);
	void	GuildThread(void);
#ifdef __S1108_BACK_END_SYSTEM
	void	BackSystem(void);
#endif // __S1108_BACK_END_SYSTEM

	BOOL GetContainer(CQuery* qry, CItemContainer<CItemElem  >* pContainer, ItemContainerStruct* pStruct);

#ifdef __FL_PACKED_ITEMS
	BOOL LoadPackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType);
	void GetOnePackage(CQuery* qry, CItemContainer<CItemElem  >* pContainer, CItemElem* pItemElem);

	void SavePackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType);
	bool UpdatePackages(CQuery* qry, u_long idOwner, int nType);
	void InsertPackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType);
	void SaveOnePackage(CItemElem* pItemElem, CItemContainer<CItemElem>* pContainer, ItemContainerStruct* pStruct);
	void DeletePackages(CQuery* qry, u_long idOwner, int nType);
#endif

#ifndef __FL_PACKED_ITEMS
	BOOL	GetBank(CMover* pMover, CQuery* qry, int nSlot);
	BOOL GetInventory(CMover* pMover, CQuery* qry);
#else
	BOOL GetBank(CMover* pMover, CQuery* qry, int nSlot, CQuery* qry1);
	BOOL GetInventory(CMover* pMover, CQuery* qry, CQuery* qry1);
#endif

	void	LoadPiercingInfo(CItemElem& itemElem, char* szPirecingInven, int* pLocation);
	void	GetPiercingInfoFromMail(CQuery* pQuery, CItemElem* pItemElem);
	BOOL	GetTaskBar(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	GetBaseCharacter(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	GetSkill(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	GetQuest(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	GetSMMode(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	GetSKillInfluence(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	GetHonor(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	static	UINT	_GetThread(LPVOID pParam);
	static	UINT	_PutThread(LPVOID pParam);
	static	UINT	_SPThread(LPVOID pParam);
	static	UINT	_UpdateThread(LPVOID pParam);
	static	UINT	_GuildThread(LPVOID pParam);
#ifdef __S1108_BACK_END_SYSTEM
	static  UINT	_BackSystem(LPVOID pParam);
#endif // __S1108_BACK_END_SYSTEM

	void	Clear(void);

	void	DBQryCharacter(char* qryCharacter, char* Gu, u_long idPlaeyr, int nserverindex, char* szAccount = '\0', char* szName = '\0',
		int nPlayerslot = 0, DWORD dwWorldID = 0, DWORD dwIndex = 0, float vPos_x = 0.0f, float vPos_y = 0.0f,
		float vPos_z = 0.0f, char* szCharacterKey = '\0', DWORD dwSkinSet = 0, DWORD dwHairMesh = 0, DWORD dwHairColor = 0,
		DWORD dwHeadMesh = 0, DWORD dwSex = 0, float vScale_x = 0.0f, DWORD dwMotion = 0, float fAngle = 0.0f,
		int nHitPoint = 0, int nManaPoint = 0, int nFatiguePoint = 0, DWORD dwRideItemIdx = 0, DWORD dwGold = 0,
		int nJob = 0, char* szActMover = '\0', int nStr = 0, int nSta = 0, int nDex = 0,
		int nInt = 0, int nLevel = 0, EXPINTEGER nExp1 = 0, EXPINTEGER nExp2 = 0, char* szJobSkill = '\0',
		char* szLicenseSkill = '\0', char* aJobLv = '\0', DWORD dwExpertLv = 0, int nidMarkingWorld = 0, float vMarkingPos_x = 0.0f,
		float vMarkingPos_y = 0.0f, float vMarkingPos_z = 0.0f, int nRemainGP = 0, int nRemainLP = 0, int nFlightLv = 0,
		int nFxp = 0, int nTxp = 0, char* szQuestCntArray = '\0', char szAuthority = 'S', DWORD dwMode = 0,
		int nidparty = 0, 
		DWORD dwFlyTime = 0, DWORD dwMessengerState = 0,
		int nTotalPlayTime = 0, char* szCard = '\0', char* szIndex_Card = '\0', char* szObjIndexCard = '\0', char* szCube = '\0',
		char* szIndex_Cube = '\0', char* szObjIndex_Cube = '\0', char* szInventory = '\0', char* dzapIndex = '\0', char* szadwEquipment = '\0',
		char* szdwObjIndex = '\0', char* szSlotApplet = '\0', char* szSlotItem = '\0', char* szSlotQueue = '\0', int nActionPoint = 0,
		char* szBank = '\0', char* m_apIndex_Bank = '\0', char* sadwObjIndex_Bank = '\0', DWORD dwGoldBank = 0,
		int nFuel = 0, int tmAccFuel = 0, char* szSMTime = '\0', char* szSkillInfluence = '\0', DWORD m_dwSkillPoint = 0
		, char* m_aCompleteQuest = '\0'
		, char* ExtInven = '\0', char* PirecingInven = '\0', char* ExtBank = '\0', char* PirecingBank = '\0'
		, DWORD dwReturnWorldID = 0, float fReturnPosX = 0.0f, float fReturnPosY = 0.0f, float fReturnPosZ = 0.0f
		, char* szInventoryPet = '\0', char* szBankPet = '\0', DWORD dwPetId = 0
		, int nHonor = -1
		, int nLayer = 0
		, char* m_aCheckedQuest = '\0'
	);

	void	DBQryLog(char* qryLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1 = 0, int nLevel = 0,
		int nJob = 0, int JobLv = 0, int FlightLv = 0, int nStr = 0, int nDex = 0,
		int nInt = 0, int nSta = 0, int nRemainGP = 0, int nRemainLP = 0, char szState = 'A',
		int WorldID = 0, char* szkilled_szName = '\0', float vPos_x = 0.0f, float vPos_y = 0.0f, float vPos_z = 0.0f,
		int nAttackPower = 0, int nMax_HP = 0, char* uGetidPlayer = '\0', DWORD dwGold = 0, int nRemainGold = 0,
		int nItem_UniqueNo = 0, char* szItem_Name = '\0', int Item_durability = 0, int Item_count = 0, int ItemAddLv = 0,
		char* szStart_Time = '\0', int TotalPlayerTime = 0, char* szRemoteIP = '\0', int nQuest_Index = 0, u_long uKill_idPlayer = 0);

	void	DBQryGuild(char* szSql, const char* szType, u_long idPlayer = 0, u_long idGuild = 0, const char* szGuild = "");
	void	DBQryGuild(char* szSql, const GUILD_QUERYINFO& info);
	void	DBQryGuildLog(char* szSql, const GUILDLOG_QUERYINFO& info);
	void	DBQryNewItemLog(char* qryLog, const LogItemInfo& info);

	int		GetOneItem(CItemElem* pItemElem, char* pstrItem, int* pLocation);
	void	GetOneSkill(LPSKILL pSkill, char* pstrSkill, int* pLocation);
	void	GetOneQuest(LPQUEST pQuest, char* pstrQuest, int* pLocation);

	static	void	MakeQueryAddMail(char* szSql, CMail* pMail, u_long idReceiver);

private:
	CDbManager();
	void	LoadMessenger(CMover* pMover, CQuery* pQuery);
	void	AddMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	DeleteMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	UpdateMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	BOOL	IsAbnormalPlayerData(CMover* pMover);
	BOOL	NormalizePlayerData(CMover* pMover);
	void	GetStrTime(CTime* time, const char* strbuf);
	BOOL	VerifyString(const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL);
	BOOL	SN(void);
	void	LogPlayConnect(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogLevelUp(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogUniqueItem(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogQuest(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogSkillPoint(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogItem(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogConcurrentUserNumber(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogInstanceDungeon(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	LogGuildFurniture(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	void	SetPlayerName(CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	void	PutExpLog(CQuery* pQueryLog, CMover* pMover, char ch);
	void	QueryGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	InsertGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	UpdateGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

	BOOL	SendItemtoCharacter(int nSlot, CMover* pMover, CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	RemoveItemtoCharacter(CMover* pMover, CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	BOOL	RemovePenya(CQuery* pQry, char* szSql, CMover* pMover, int nItem_Count, int nNo, char chState);
	SERIALNUMBER	RemoveItem(CQuery* pQry, char* szSql, int nNo, CMover* pMover, char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState);
	SERIALNUMBER	RemoveItemBank(ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount);

	BOOL	GetSendItem(CQuery* pQry, __SendItemContents* pSendItemContents);
	SERIALNUMBER	SendItem(CQuery* pQry, char* szSql, CMover* pMover, __SendItemContents& SendItemContents);
	SERIALNUMBER	RemoveItemInventory(ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount);
	BOOL	GetRemoveItem(CQuery* pQry, int& nNo, char* pOneItem, int& nItem_Count, int& nAbilityOption, int& nItemResist, int& nResistAbilityOption, char& chState);

	BOOL	SendPenya(CQuery* pQry, char* szSql, CMover* pMover, int nPenya, int nNo);
	BOOL	SendItemDeleteQuery(CQuery* pQry, char* szSql, int nNo);
	BOOL	ItemLogQuery(CQuery* pQryLog, char* szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char* szItemName);
	BOOL	RemoveItemDeleteQuery(CQuery* pQry, char* szSql, int nNo);

	CQuery	m_qryPostProc;
	void	AddMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
public:
	static void	PostMail(CQuery& kQuery, const u_long uReceiver, const u_long uSender, CItemElem& kItemElem, const int nGold, const char* szTitle, const char* szText);
private:
	void	RemoveMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	RemoveMailItem(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	RemoveMailGold(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	ReadMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov);
	void	DbQryMail(char* szSql, const MAIL_QUERYINFO& info);

#ifdef __S1108_BACK_END_SYSTEM
	void	GetGemeSettingtime(CQuery* pQuery, int nChat = 0);
	BOOL	GetBaseGameSetting(CQuery* pQuery, char* lpstrTime = '\0');
	void	GetMonsterRespawnSetting(CQuery* pQuery, int& nRemoveMaxRespawn, int aRemoveRespawn[] = NULL, char* lpstrTime = '\0');
	void	GetMonsterPropSetting(CQuery* pQuery, char* lpstrTime = '\0');
	BOOL	AddMonsterRespawn(MONSTER_RESPAWN BufMonsterRespawn);
	BOOL	RemoveMonsterRespawn(int nIndex, int& nMaxRemove, int aRemoveRespawn[]);
	BOOL	AddMonsterProp(MONSTER_PROP BufMonsterProp);
	BOOL	RemoveMonsterProp(MONSTER_PROP BufMonsterProp);
	BOOL	GetGMChat(CQuery* pQuery, char* lpstrTime, int& nChatCount);
#endif // __S1108_BACK_END_SYSTEM

	void	StartGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	InGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	OutGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	ResultGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void	ContinueGC(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);

private:
	void	GetStrFromDBFormat(char* szDst, const char* szSrc, int& n);
	void	SetStrDBFormat(char* szDst, const char* szSrc);
#ifdef __PET_1024
	void	GetDBFormatStr(char* szDst, int nMaxLen, const char* szSrc);
	void	SetDBFormatStr(char* szDst, int nMaxLen, const char* szSrc);
#endif	// __PET_1024

#ifdef __FL_GW_STATUE_V2
	void InstallStatues(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	bool	SaveChest(CMover* pMover, CQuery* qry);
#ifndef __FL_PACKED_ITEMS
	BOOL	GetChest(CMover* pMover, CQuery* qry);
#else
	BOOL	GetChest(CMover* pMover, CQuery* qry, CQuery* qry1);
#endif
#endif

#ifdef __FL_EQUALIZED_MODE
	void SaveTemporalEquipment(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void LoadTemporalEquipment(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif

#ifdef __FL_DAILY_REWARDS
	void UpdateDailyGift(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif

#ifdef __FL_VENDORS
	void AddVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void UpdateVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void DeleteVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#ifndef __FL_PACKED_ITEMS
	void GetVendors(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#else
	void GetVendors(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, CQuery* pQuery1);
#endif
#endif

#ifdef __FL_CASH_SHOP
public:
	BOOL LoadCShop();
private:
	void GetCShopBalance(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
	void BuyCShopItem(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus);
#endif
#ifdef __FL_SWITCH_EQ
	void	SaveSwitches(CMover* pMover, CQuery* qry);
	void	GetSwitches(CMover* pMover, CQuery* qry);
#endif
#ifdef __FL_RECORD_BOOK
	void	LoadRecords(CMover* pMover, CQuery* qry);
	void	SaveRecords(CMover* pMover, CQuery* qry);
#endif
#ifdef __FL_MONSTER_ALBUM
	void	SaveMonsterAlbum(CMover* pMover, CQuery* qry);
	void	LoadMonsterAlbum(CMover* pMover, CQuery* qry);
#endif

	u_long			m_idPlayer;
	HANDLE			m_hThreadGet[MAX_GETTHREAD_SIZE];
	HANDLE			m_hThreadPut[MAX_PUTTHREAD_SIZE];
	HANDLE			m_hSPThread;
	HANDLE			m_hCloseSPThread;
	HANDLE			m_hThreadUpdate[MAX_UPDATETHREAD_SIZE];
	HANDLE			m_hThreadGuild;
	HANDLE			m_hWait;
};

inline BOOL CDbManager::VerifyString(const char* lpString, const char* lpFileName, int nLine, const char* lpName, LPDB_OVERLAPPED_PLUS lpOverlapped)
{
	int len = strlen(lpString);
	if (len > 0 && lpString[len - 1] == '$')
		return TRUE;

	WriteLog("%s, %d\t%s", lpFileName, nLine, lpName);
	return FALSE;
}

extern CDbManager& g_DbManager;

#endif	// _DB_MANAGER_H_