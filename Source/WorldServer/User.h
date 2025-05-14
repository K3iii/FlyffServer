#ifndef __USER_H__
#define __USER_H__

#pragma once

#include <DPlay.h>
#include "MsgHdr.h"
#include "Snapshot.h"
#include "UserTaskBar.h"
#include "Party.h"
#include "rtmessenger.h"
#include "misc.h"
#include "guild.h"
#include "eveschool.h"
#include "Chatting.h"
#include "CooltimeMgr.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM
#if __VER >= 13 // __HOUSING
#include "Housing.h"
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE
#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif
#ifdef __FL_RECORD_BOOK
#include "RecordParameters.h"
#endif

#define GETID( pCtrl )		( (pCtrl) ? (pCtrl)->GetId() : NULL_ID )
typedef	MemPooler<CUser>	CUserPool;

#ifdef __AZRIA_1023
#define	MAX_INPUT_LEN	100
#endif	// __AZRIA_1023

class CMail;
class CMailBox;

#define CHECK_TICK_FROM_CLIENT	SEC(3)

class CLordSkill;

class CUser : public CMover
{
public:
	CUser();
	CUser(DPID dpidCache, DPID dpidUser);
	virtual	~CUser();

	////////////////////////////////////////////////////////////////////

private:
	BOOL			m_bCheckTransMailBox;
	int				m_nCountFromClient;
	DWORD			m_dwTickFromClient;

	////////////////////////////////////////////////////////////////////

public:
#ifdef __FL_RECORD_BOOK
	std::array<PlayerRecord_Own, MAX_RECORDTYPE> m_arrayRecords;
#endif

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	vector<INVITE_REQUESTS> vecFriendInvites;
	vector<INVITE_REQUESTS> vecGuildInvites;
	vector<INVITE_REQUESTS> vecPartyInvites;
#endif

#ifdef __FL_EQUALIZED_MODE //todo: rework
	BOOL		 bTmpMode;
	CItemContainer<CItemElem> m_InventoryTemp;
	int			 m_nTmpJob;
	int			 m_nTmpLv;
	EXPINTEGER	 m_nTmpExp;
	CUserTaskBar m_UserTaskBarTemp;
	DWORD		 m_dwSMTimeTmp[SM_MAX];
	CBuffMgr	 m_buffsTmp;
	SKILL		 m_aJobSkillTmp[MAX_SKILL_JOB];
	int			 m_nSkillLevelTmp;
	int			 m_nSkillPointTmp;
	long		 m_nRemainGPTmp;
	LONG		 m_nStrTmp, m_nStaTmp, m_nDexTmp, m_nIntTmp;
#ifdef __FL_SWITCH_EQ
	vector<EQ_SWITCH> m_vecSwitchesTmp;
#endif
#ifdef __SWITCH_EQUIP
	vector<__EQUIP_SWITCH_INFO> m_vEquipSwitchInfoTmp;
#endif
#endif

#ifdef __AIO_PETS
private:
	DWORD savedEatPetId;
public:
	DWORD GetSavedEatPetId() { return savedEatPetId; }
	void  SetSavedEatPetId(const DWORD dw) { savedEatPetId = dw; }
#endif

#ifdef __FL_DUNGEON_TREASURE
private:
	BOOL bTreasureOpenState;
public:
	BOOL GetTreasureOpenState() { return bTreasureOpenState; }
	void SetTreasureOpenState(BOOL bState) { bTreasureOpenState = bState; }
#endif

#ifdef __FL_FIX_BLADEHACK
private:
	BYTE nLastAnimation;
public:
	BYTE GetLastAnimation() { return nLastAnimation; }
	void SetLastAnimation(BYTE nAni) { nLastAnimation = nAni; }
#endif

#ifdef __FL_UPDATE_CHATTING
private:
	DWORD dwLastSentChatMessage;
public:
	DWORD GetLastChatMessage() { return dwLastSentChatMessage; }
	void  SetLastChatMessage(const DWORD dw) { dwLastSentChatMessage = dw; }
#endif

#ifdef __FL_PET_FILTER
	int m_nPetLootType;
	BYTE m_nLootPlace;
#endif

#ifdef __FL_DAILY_REWARDS
	int m_nLastDailyReward;
	void AddDailyReward();
#endif

#ifdef __FL_SWITCH_EQ
	DWORD m_dwNextSwitch;
#endif

	DWORD			m_dwSerial;
	BOOL			m_bValid;
	CSnapshot		m_Snapshot;
	map<DWORD, CCtrl*>	m_2npc;
	DWORD			m_dwTickSFS;
	DWORD			m_dwTickNotify;
	DWORD			m_dwTimeout4Save;
	DWORD			m_dwLeavePenatyTime;
	DWORD			m_dwDestroyTime;

	DWORD			m_dwAuthKey;
	PLAY_ACCOUNT	m_playAccount;
	CUserTaskBar	m_playTaskBar;
	OBJID			m_idSetTarget;
	CRTMessenger	m_RTMessenger;
	char			m_szPartyName[32];
	char			m_szBankPass[5];

	int				m_nOverHeal;
	CObj* m_pWall[2];
	BOOL			m_bPosting;

	u_long			m_idChatting;
	u_long			m_idSnoop;
	BOOL			m_bInstantBank;
	CCooltimeMgr	m_cooltimeMgr;
	DWORD			m_tmEscape;
	D3DXVECTOR3		m_vReturnPos;
	DWORD			m_dwReturnWorldID;
	LPCTSTR			m_lpszVillage;

	DWORD			m_dwPlayerTime;
	DWORD			m_dwSavePlayerTime;

#ifndef __JEFF_9_20
	DWORD			m_dwFlyTime;
	DWORD			m_dwSaveFlyTime;
	DWORD			m_dwSendFlyTime;
#endif	// __JEFF_9_20

#ifdef __QUEST_1208
	DWORD	m_tickScript;
#endif	// __QUEST_1208

#ifdef __S_SERVER_UNIFY
	BOOL	m_bAllAction;
#endif // __S_SERVER_UNIFY

#ifdef __EVE_MINIGAME
	int		m_nKawibawiboWin;
	int		m_nKawibawiboState;
#endif // __EVE_MINIGAME

#ifdef __EVENTLUA_KEEPCONNECT
	DWORD	m_dwTickKeepConnect;
#endif // __EVENTLUA_KEEPCONNECT

public:
	virtual void	Process();
	void			CheckFiniteItem();
	void			LevelUpSetting(void);
	void			SetValid(BOOL bValid) { m_bValid = bValid; }
	BOOL			IsValid();
	void			SetReturnPos(const D3DXVECTOR3& vPos);
	D3DXVECTOR3& GetReturnPos(DWORD* pdwWorldID);
	void			SetPlayerTime(DWORD dwSaveTime) { m_dwPlayerTime = timeGetTime(); m_dwSavePlayerTime = dwSaveTime; };
	DWORD			GetPlayerTime() { return (((timeGetTime() - m_dwPlayerTime) / 1000) + m_dwSavePlayerTime); };
#ifndef __JEFF_9_20
	void			SetSaveFlyTime(DWORD dwFlyTime) { m_dwFlyTime = m_dwSendFlyTime = 0; m_dwSaveFlyTime = dwFlyTime; };
	void			SetFlyTimeOn() { m_dwFlyTime = timeGetTime(); };
	void			SetFlyTimeOff() { m_dwSendFlyTime += (timeGetTime() - m_dwFlyTime) / 1000; m_dwFlyTime = 0; };
	DWORD			GetFlyTime() { return m_dwSendFlyTime; };
#endif	// __JEFF_9_20
	void			NPCSetAt(OBJID objid, CCtrl* pCtrl) { m_2npc[objid] = pCtrl; }
	BOOL			NPCRemoveKey(OBJID objid) { return m_2npc.erase(objid) > 0; }
	void			RemoveItFromView2(BOOL bRemoveall = FALSE);
#ifdef __MAP_SECURITY
	void			Open(DWORD dwWorldId);
#else // __MAP_SECURITY
	void			Open();
#endif // __MAP_SECURITY
	//	void			Notify();
	int		Notify(void);
	//	void			Flush()	{	Notify();	m_Snapshot.Flush();		}
#ifdef __S_SERVER_UNIFY
	void			SetAllAction();
#endif // __S_SERVER_UNIFY

	void			AddPostMail(CMail* pMail);
	void			AddRemoveMail(u_long nMail, int nType);
	void			AddMailBox(CMailBox* pMailBox);




	////////////////////////////////////////////////////////////////////
	void			SendCheckMailBoxReq(BOOL bCheckTransMailBox);
	void			CheckTransMailBox(BOOL bCheckTransMailBox);
	BOOL			GetCheckTransMailBox();
	bool			CheckClientReq();
	void			ResetCheckClientReq();
	int				GetCountClientReq();
	////////////////////////////////////////////////////////////////////




	void			SetPosting(BOOL bPosting) { m_bPosting = bPosting; }
	BOOL			IsPosting(void) { return m_bPosting; }
	BOOL			DoUsePackItem(CItemElem* pItemElem, PPACKITEMELEM pPackItemElem, int nType, bool bBulk);
	BOOL			DoUseGiftbox(CItemElem* pItemElem, int nType, bool bBulk);
	void			ADDGameJoin();
	void			AddHdr(OBJID objid, WORD wHdr);
	void			AddBlock(LPBYTE lpBlock, u_long uBlockSize);
	void			AddAddObj(CCtrl* pCtrl);
	void			AddRemoveObj(OBJID objid) { AddHdr(objid, SNAPSHOTTYPE_DEL_OBJ); }
	void			AddDiagText(LPCSTR lpszText);
	void			AddText(LPCSTR lpsz, DWORD dwColor = 0xff00ff00);
	void			AddReplace(DWORD dwWorldID, D3DXVECTOR3& vPos);
	void			AddCreateItem(CItemElem* pItemElem, int nType);
	void			AddMoveItem(BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex);
	void			AddTrade(CUser* pTrader, u_long uidPlayer);
	void			AddComfirmTrade(OBJID objid);
	void			AddComfirmTradeCancel(OBJID objid);
	void			AddTradePut(OBJID objid, BYTE byNth, CItemElem* pItemElem, int nItemNum = 1);
	void			AddTradePutError();
	void			AddTradePull(OBJID objid, BYTE byNth);
	void			AddTradePutGold(OBJID objid, DWORD dwGold);
	//raiders.2006.11.28
	//	void			AddTradeClearGold( OBJID objid );
	void			AddTradeCancel(OBJID objid, u_long uidPlayer, int nMode = 0);
	void			AddTradeOk(OBJID objid) { AddHdr(objid, SNAPSHOTTYPE_TRADEOK); }
	void			AddTradeConsent() { AddHdr(NULL_ID, SNAPSHOTTYPE_TRADECONSENT); }
	void			AddTradelastConfirm() { AddHdr(NULL_ID, SNAPSHOTTYPE_TRADELASTCONFIRM); }
	void			AddTradelastConfirmOk(OBJID objid) { AddHdr(objid, SNAPSHOTTYPE_TRADELASTCONFIRMOK); }
	void			AddOpenShopWnd(CMover* pVendor);
	void			AddUpdateItem(CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime, int nMode = TYPE_INVENTORY);
	void			AddUpdateItemEx(unsigned char id, char cParam, __int64 iValue);
	void			AddQuePetResurrectionResult(BOOL bResult);

	void			AddSetFxp(int nFxp, int nFlightLv);
	void			AddSetGrowthLearningPoint(long nRemainGP);
	void			AddDefinedText(int dwText, LPCSTR lpszFormat, ...);
	void			AddChatText(int dwText, LPCSTR lpszFormat, ...);
	void			AddDefinedText(int dwText);
	void			AddDefinedCaption(BOOL bSmall, int dwText, LPCSTR lpszFormat, ...);
	void			AddSetQuest(LPQUEST lpQuest);
	void			AddSetGuildQuest(int nQuestId, int nState);
	void			AddRemoveGuildQuest(int nQuestId);
	void			AddSetChangeJob(int nJob);
	void			AddReturnSay(int ReturnFlag, const CHAR* lpszPlayer);
	void			AddGameTimer(double dCurrentTime);
	void			AddPartyChangeLeader(u_long uidChangeLeader);
	void			AddCancelQuest(DWORD dwQuestCancelID);
	void			AddRemoveQuest(DWORD dwQuestCancelID);
	void			AddRemoveAllQuest();
	void			AddRemoveCompleteQuest();
	void			AddGetItemBank(CItemElem* pItemElem);
	void			AddMoveBankItem(BYTE nSrcIndex, BYTE nDestIndex);
	void			AddBankIsFull();
	void			AddRemoveGuildBankItem(u_long idGuild, DWORD dwId, DWORD dwItemNum);
	void			AddChangeBankPass(int nMode, DWORD dwId, DWORD dwItemId);
	void			AddBankWindow(int nMode, DWORD dwId, DWORD dwItemId);
	void			AddconfirmBankPass(int nMode, DWORD dwId, DWORD dwItemId);
	void			AddTaskBar();
	void			AddSendErrorParty(DWORD dw, DWORD dwSkill = 0);
	void			AddPartyMember(CParty* pParty, u_long idPlayer, const char* pszLeader, const char* pszMember);
	void			AddPartyRequest(CUser* pLeader, CUser* pMember, BOOL bTroup);
	void			AddPartyRequestCancel(u_long uLeaderid, u_long uMemberid, int nMode = 0);
	void			AddPartyExpLevel(int Exp, int Level, int nPoint);
	void			AddPartyMemberFlightLevel(int nMemberIndex, int nLevel);
	void			AddPartyChangeTroup(const char* szPartyName);
	void			AddPartyChangeName(const char* szPartyName);
	void			AddSetPartyMode(int nMode, BOOL bOnOff, LONG nPoint, DWORD dwSkillTime);
	void			AddPartyChangeItemMode(int nItemMode);
	void			AddPartyChangeExpMode(int nExpMode);

	void			AddSetPartyMemberParam(u_long idPlayer, BYTE nParam, int nVal);
	void			AddPartyName();
	void			AddPartySkillCall(const D3DXVECTOR3& vLeader);
	void			AddPartySkillBlitz(OBJID idTarget);
	void			AddFriendGameJoin();
	void			AddFriendReqest(u_long uLeader, LONG nJob, BYTE nSex, const char* szName);
	void			AddFriendCancel();
	void			AddFriendError(BYTE nError, const char* szName);

	void			AddEnvironmentSetting();
	void			AddEnvironment();
	void			AddPartyChat(const CHAR* lpName, const CHAR* lpString, OBJID objid);
	void			AddAddFriend(u_long idPlayer, const char* lpszPlayer);
	void			AddRemoveFriend(u_long uidSender);
	void			AddQueryPlayerData(u_long idPlayer, PlayerData* pPlayerData);
	void			AddPlayerData(void);
	void			AddSetSkill(DWORD dwSkill, DWORD dwSkillLevel);
	void			AddResurrectionMessage();
	void			AddCorrReq(CMover* pMover);
	void			AddSetFuel(int nFuel, DWORD tmAccFuel);
	void			AddSetSkillState(CMover* pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime);
	void			AddPlaySound(u_long idSound);
	void			AddPlayMusic(u_long idMusic);
	void			AddUnregisterPVendorItem(BYTE iIndex);
	void			AddPVendorItem(CUser* pUser, BOOL bState);
	void			AddPVendorClose(OBJID objidVendor);
	void			AddPutItemBank(BYTE nSlot, CItemElem* pItemElem);
	void			AddPutGoldBank(BYTE nSlot, DWORD dwGold, DWORD dwGoldBank);
	void			AddGuildBankWindow(int nMode);
	void			AddPutItemGuildBank(CItemElem* pItemElem);
	void			AddGetItemGuildBank(CItemElem* pItemElem, BOOL bItsMe = TRUE);
	void			AddGetGoldGuildBank(DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak);
	void			AddContribution(CONTRIBUTION_CHANGED_INFO& info);
	void			AddSetNotice(u_long idGuild, const char* szNotice);
	void			AddSetGuildAuthority(DWORD dwAuthority[]);
	void			AddAllGuilds();
	void			AddMyGuild();
	void			AddGuildInvite(u_long idGuild, u_long idMaster);
	void			AddSMModeAll();
	void			AddFlyffEvent();
#if __VER >= 9 // __EVENTLUA
	void			AddEventLuaDesc(int nState, string strDesc);
#endif // __EVENTLUA

	void			AddCommonSkill(DWORD dwSkill, DWORD dwLevel);
	void			AddPlaySound2(const char* szSound);
	void			AddRunScriptFunc(const RunScriptFunc& runScriptFunc);
	void			AddRemoveSkillInfluence(WORD wType, WORD wID);
	void			AddGCWindow(__int64 dwPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya);
	void			AddGCRequestStatus(__int64 nPrizePenya, vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus);
	void			AddGCSelectPlayerWindow(vector<CGuildCombat::__JOINPLAYER>& vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar);
	void			AddGCJoinWarWindow(int nTelTime);
	void			AddGuildCombatNextTime(DWORD dwTime, DWORD dwState);
	void			AddGuildCombatEnterTime(DWORD dwTime);
	void			AddGCWinGuild(void);
	void			AddGCBestPlayer(void);
	void			AddGCIsRequest(BOOL bRequest);
	void			AddGCDiagMessage(const char* str);
	void			AddGCTele(const char* str);
	void			AddGCWarPlayerlist(void);
	void			AddGuildCombatState(void);
	void			AddQuestTextTime(BOOL bFlag, int nState, DWORD dwTime);
	void			AddGCLog(void);
	void			AddGCPlayerPoint(void);
	void			AddFocusObj(OBJID objid);
	void			AddEnterChatting(CUser* pUser);
	void			AddNewChatting(CChatting* pChatting);
	void			AddRemoveChatting(u_long uidPlayer);
	void			AddDeleteChatting(void);
	void			AddChatting(u_long uidPlayer, char* pszChat);
	void			AddChttingRoomState(BOOL bState);
	void			AddGameRate(FLOAT fRate, BYTE nFlag);
	void			AddGameSetting();
	void			AddInitSkill();
	void			AddSetNaviPoint(const NaviPoint& nv, OBJID objid, const char* Name);
	void			AddEscape(DWORD tmMaxEscape);
	void			AddSetActionPoint(int nAP);
	void			AddSetTarget(OBJID idTarget);
	void			AddReturnScroll();
	void			AddSnoop(const char* lpString);
	void			AddSetCheerParam(int nCheerPoint, DWORD dwRest, BOOL bAdd = FALSE);
	void			AddQueryEquip(CUser* pUser);
	void			AddSummonFriendUse(CItemElem* pItemElem);
	void			AddSummonFriendConfirm(OBJID objid, DWORD dwData, const char* szName, const char* szWorldName);
	void			AddSummonPartyConfirm(OBJID objid, DWORD dwData, const char* szWorldName);

	void			AddDoUseSkillPoint(SKILL aJobSkill[], int nSkillPoint);
	void			AddPartyMapInfo(int nIndex, D3DXVECTOR3 vPos);	// __S1005_PARTY_MAP
	void			AddSetExperience(EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel);
#ifdef __S_SERVER_UNIFY
	void			AddAllAction(BOOL bCharacter);
#endif // __S_SERVER_UNIFY

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM

#ifdef __EVE_MINIGAME
	void			AddKawibawiboResult(int nResult, int nWinCount, DWORD dwItemId = 0, int nItemCount = 0, DWORD dwNextItemId = 0, int nNextItemCount = 0);
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
	void			AddUltimateMakeItem(BOOL bResult);
	void			AddUltimateMakeGem(int nResult, int nNum);
	void			AddUltimateWeapon(BYTE state, int nResult);
#endif // __ULTIMATE

#ifdef __TRADESYS
	void			AddExchangeResult(BYTE state, int nResult);
#endif // __TRADESYS

#if __VER >= 9	// __PET_0410
	void	AddPet(CPet* pPet, BYTE nPetLevelup = PF_PET);
	void	AddPetState(DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp);
	void	AddPetSetExp(DWORD dwExp);
	void	AddPetFoodMill(int nResult, int nCount);
#endif	// __PET_0410

#if __VER >= 10
	void			AddLegendSkillResult(int nResult);
#endif

#if __VER >= 10 // __REMOVE_ATTRIBUTE
	void			AddRemoveAttribute(BOOL bSuccess);
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 11 // __SYS_COLLECTING
	virtual	void	ProcessCollecting(void);
	virtual	void	StartCollecting(void);
	virtual	void	StopCollecting(void);
	BOOL	DoUseItemBattery(void);
#if __VER < 14 // __SMELT_SAFETY
	BOOL	PreRefine(DWORD dwTarget, DWORD dwMaterial);
	void	RefineCollector(CItemElem* pTarget, CItemElem* pMaterial);
	void	RefineAccessory(CItemElem* pTarget, CItemElem* pMaterial);
#endif // __SMELT_SAFETY
	void	AddRestartCollecting(DWORD dwItemId);
	int		m_nCollecting;
#endif	// __SYS_COLLECTING

#ifdef __AZRIA_1023
	BOOL	HasInput() { return m_szInput[0] != '\0'; }
	void	ResetInput() { m_szInput[0] = '\0'; }
	void	SetInput(char* szInput) { strncpy(m_szInput, szInput, MAX_INPUT_LEN);	m_szInput[MAX_INPUT_LEN - 1] = '\0'; }
	char* GetInput() { return m_szInput; }
	int		DoUseItemInput(ItemProp* pProp, CItemElem* pItem);
#endif	// __AZRIA_1023

#ifdef __PET_1024
	int		DoUseItemPetNaming();
#endif	// __PET_1024

#ifdef __SYS_TICKET
private:
	CItemElem* FindActiveTicket(DWORD dwItemId);
public:
	void	DoUseItemTicket(CItemElem* pItemElem);
#endif	// __SYS_TICKET

#if __VER >= 11
	void	AddGuildBankLogView(BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs);
#endif

#if __VER >= 13
	void	AddHonorListAck();
#endif

#if __VER >= 12 // __SECRET_ROOM
	void	AddSecretRoomMngState(int nState, DWORD dwRemainTime);
	void	AddSecretRoomContInfo(BYTE nContinent, BYTE nType, vector<__SECRETROOM_TENDER>& vecSecreetRoomTender, int nIndex);
	void	AddSecretRoomTenderOpenWnd(int nTenderPenya);
	void	AddSecretRoomLineUpOpenWnd(vector<DWORD>& vecLineUpMember);
	void	AddSecretRoomTenderView(int nTenderPenya, int nRanking, time_t t, vector<__SECRETROOM_TENDER>& vecSRTender);
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
	void	AddLord(void);
	void	AddLordSkillTick(CLordSkill* pSkills);
	int		GetMuteText(void);
	void	SetElection(u_long idElection) { m_idElection = idElection; }
	u_long	GetElection(void) { return m_idElection; }
	BOOL	IsQuerying(void) { return m_bQuerying; }
	void	SetQuerying(BOOL bQuerying) { m_bQuerying = bQuerying; }
#endif	// __LORD

#if __VER >= 12 // __TAX
	void	AddTaxInfo(void);
	void	AddTaxSetTaxRateOpenWnd(BYTE nCont);
#endif // __TAX

	BOOL	IsGuildMaster();

	void	ValidateItem(void);
	void	AdjustGuildQuest(DWORD dwWorldId);
	void	AdjustMailboxState(void);
	void	AddEventLuaDesc(void);
	void	AdjustPartyQuest(DWORD dwWorldId);

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static CUserPool* m_pPool;
	void* operator new(size_t nSize) { return CUser::m_pPool->Alloc(); }
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine) { return CUser::m_pPool->Alloc(); }
	void	operator delete(void* lpMem) { CUser::m_pPool->Free((CUser*)lpMem); }
	void	operator delete(void* lpMem, LPCSTR lpszFileName, int nLine) { CUser::m_pPool->Free((CUser*)lpMem); }
#endif	// __MEM_TRACE
#endif	// __VM_0820


#if __VER >= 13 // __HOUSING
	void AddHousingAllInfo(CHousing* pHousing);
	void AddHousingSetFurnitureList(HOUSINGINFO& housingInfo, BOOL bAdd);
	void AddHousingSetupFurniture(HOUSINGINFO* pHousingInfo);
	void AddHousingPaperingInfo(DWORD dwItemId, BOOL bSetup);
	void AddHousingSetVisitAllow(DWORD dwTargetId, BOOL bAllow);
	void AddHousingVisitableList(vector<DWORD>& vecVisitable);
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
	void AddNPCPos(const D3DXVECTOR3& vPos);
#endif // __QUEST_HELPER

private:
	void			Init(DPID dpidCache = DPID_UNKNOWN, DPID dpidUser = DPID_UNKNOWN);

#if __VER >= 12 // __LORD
	u_long	m_idElection;
	BOOL	m_bQuerying;
#endif	// __LORD
#ifdef __AZRIA_1023
	char	m_szInput[MAX_INPUT_LEN];
#endif	// __AZRIA_1023

#if __VER >= 13 // __COUPLE_1117
public:
	void	SetProposer(u_long idProposer) { m_idProposer = idProposer; }
	u_long	GetProposer() { return m_idProposer; }
	void	AddCouple();
	void	AddProposeResult(u_long idProposer, const char* pszProposer);
	void	AddCoupleResult(u_long idPartner, const char* pszPartner);
	void	AddDecoupleResult();
#if __VER >= 13 // __COUPLE_1202
	void	ProcessCouple();
	void	ActiveCoupleBuff(int nLevel);
	void	AddAddCoupleExperience(int nExperience);
#endif	// __COUPLE_1202
#if __VER >= 14 // __SMELT_SAFETY
	void AddSmeltSafety(BYTE nResult);
#endif // __SMELT_SAFETY
#ifdef __MAP_SECURITY
	void AddWorldReadInfo(DWORD dwWorldId, D3DXVECTOR3 vPos);
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
	void AddActivateVisPet(OBJID objIdVisPetItem, OBJID objIdVisPetId);
#endif // PETVIS

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	void AddCheckedQuest();
#endif // __IMPROVE_QUEST_INTERFACE

#if __VER >= 15 // __GUILD_HOUSE
public:
	void	AddGuildHousePakcet(int nPacketType, GH_Fntr_Info& gfi, int nIndex);
	void	AddGuildHouseAllInfo(CGuildHouseBase* pGuildHouse);
	void	AddGuildHouseRemvoe();
#endif // __GUILD_HOUSE

private:
	u_long	m_idProposer;
#if __VER >= 13 // __COUPLE_1202
	int		m_cbProcessCouple;
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117



public:
	void	SelectAwakeningValue(DWORD dwItemObjID, DWORD dwSerialNumber, BYTE bySelectFlag);
	void	SendNewRandomOption(unsigned char id, DWORD dwSerialNumber, __int64 n64NewRandomOption);


#ifdef __FL_DUNGEON_TREASURE
	void AddDungeonTreasureWnd(BOOL bOpen);
	void AddDungeonTreasureResult(int nSelected, BOOL bResult);
#endif

#ifdef __FL_BATTLE_PASS
	void AddBattlePassTime();
	void AddBattlePassState(int nBPLvl, int nBPExp);
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	void AddPutItemToChest(CItemElem* pItemElem);
	void AddMoveItemInsideChest(BYTE nSrcIndex, BYTE nDestIndex);
#endif

#ifdef __FL_SHOP_SEARCH
	void AddShopSearch();
#endif

	void	SummonPlayer(CUser* pDestUser);

#ifdef __FL_EQUALIZED_MODE
	void AddInvAndBuffsSerialize(bool bWithBuffs);
	void AddOpenTempWindow(int nType);
#endif

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	void ProcessInvites();
#endif

#ifdef __FL_PARTY_SORT
	void AddPartyChangePosition(u_long uidChangeMember, BOOL bUpDown);
#endif

#ifdef __FL_GW_STATUE_V2
	void AddStatueParams(int nCost);
#endif

#ifdef __FL_FFA
	void AddFFAData();
	void AddFFAState(BOOL bPlayerList);
	void AddFFAKill(u_long idKiller, u_long idDead);
#endif

#ifdef __MODEL_CHANGE
	void AddLookChange(DWORD dwID, DWORD dwLook);
#endif

#ifdef __FL_PARTY_FINDER
	void AddPartyFinder(vector<PARTY_LIST>* vecParties);
	void AddPartyChangeJoinMode(BOOL bMode);
#endif

#ifndef __FL_VENDORS
	void AddRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost);
#else
	void AddRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost, int nPerinCost);
	void ActivateClientVendor();
	void AddVendorPreview(CMover* pMover);
#endif
#ifdef __FL_CASH_SHOP
	void AddCShop();
	void AddCShopBalance(int nDonate, int nVote);
#endif

#ifdef __FL_SWITCH_EQ
	void AddSwitchAdd(int nSel, const char* lpszName, DWORD dwLooter, DWORD dwPet, DWORD dwRegen);
	void AddSwitchDelete(int nSel);
	void AddSwitches();
#endif
#ifdef __FL_GUILD_FINDER
	void AddGuildFinder();
	void AddSetGuildFinderState(BOOL bFinder);
	void AddGuildFinderAddon(CGuild* pGuild);
#endif
#ifdef __FL_PACKED_ITEMS
	void AddPackageOpen(DWORD dwObjId);
	void AddPackageAdd(DWORD dwPackObjId, DWORD dwItemObjId);
	void AddPackageRestore(DWORD dwObjId, int nCurSel);
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
	void AddWof();
	void AddWofInit(int nPrize);
	void AddWofFinish(WOF_ITEM* pItem);
#endif
#ifdef __FL_RECORD_BOOK
	void AddRecordBook();
#endif
#ifdef __FL_MONSTER_ALBUM
	void AddMAAddCardPiece(DWORD dwMobID, int nAddedPieces);
	void AddMABossCardSlotUnlock(int nSlot);
	void AddMABossCardSlotInsert(int nSlot, DWORD dwBossCardID);
	void AddMABossCardSlotRestore(int nSlot);
#endif

#ifdef __FL_TDM
	void AddTDMData();
	void AddTDMRegister(CUser* pUser);
	void AddTDMState(bool bPlayerList);
	void AddTDMKill(u_long idKiller, u_long idDead);
#endif
};

#define	SEC_SAVEPLAYER	900

class CDPSrvr;
class CLEComponent;
class ILordEvent;

class CUserMng
{
public:
	CUserMng();
	virtual	~CUserMng();

private:
	long				m_lCount;
public:
	map<DWORD, CUser*>	m_users;

public:
	void			DestroyPlayer(CUser* pUser);
	LONG			GetCount() { return m_lCount; }
	void			RemoveAllUsers();
	CUser* AddUser(DPID dpidCache, DPID dpidUser, DPID dpidSocket);
	void			RemoveUser(DWORD dwSerial);
	CUser* GetUser(DPID dpidCache, DPID dpidUser, bool checkValidity = false);
	CUser* GetUserByPlayerID(u_long idPlayer);
#ifdef __VALID_CONNECTION_CONFORMATION
	CUser* GetUserBySerial(u_long serial);
#endif
	BOOL			AddPlayer(CUser* pUser, DWORD dwWorldID, int nLayer);
	void			AddDisguise(CMover* pMover, DWORD dwMoverIdx);
	void			AddNoDisguise(CMover* pMover);
	void			AddHdr(CCtrl* pCtrl, WORD wHdr);
	void			AddChat(CCtrl* pCtrl, const TCHAR* szChat);
	void			AddStartCollecting(CUser* pUser);
	void			AddStopCollecting(CUser* pUser);
	void			AddDlgEmoticon(CCtrl* pCtrl, int nIdx);
	void			AddDamage(CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags);
	void			AddDoEquip(CMover* pMover, DWORD dwPart, CItemElem* pItemElem, BOOL bEquip);
	void			AddUpdateVendor(CMover* pVendor, CHAR cTab, BYTE nId, short nNum);
	void			AddSetDestParam(CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue);
	void			AddResetDestParam(CMover* pMover, int nDstParamter, int nAdjParameterValue);
#ifdef __SPEED_SYNC_0108
	void			AddResetDestParamSync(CMover* pMover, int nDstParameter, int nAdjParameterValue, int nParameterValue);
#endif
	void			AddSetPointParam(CMover* pMover, int nDstParameter, int nValue);
	void			AddSetPos(CCtrl* pCtrl, D3DXVECTOR3& vPos);
	void			AddSetPosAngle(CCtrl* pCtrl, const D3DXVECTOR3& vPos, FLOAT fAngle);
	void			AddSetLevel(CMover* pMover, WORD wLevel);
	void			AddSetFlightLevel(CMover* pMover, int nFlightLv);
	void			AddSetSkillLevel(CMover* pMover, DWORD dwSkill, DWORD dwLevel);
	void			AddSetScale(CMover* pMover, float fScalePercent);
	void			AddWorldShout(const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld);
	void			AddSetSkillState(CMover* pCenter, CMover* pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime);
	void			AddMeleeAttack(CMover* pMover, OBJMSG dwAtkMsg, OBJID objid);
	void			AddMeleeAttack2(CMover* pMover, OBJID objid);
	void			AddMagicAttack(CMover* pMover, OBJID objid, int nMagicPower, int idSfxHit);
	void			AddRangeAttack(CMover* pMover, OBJID objid, int nParam2, int idSfxHit);
	void			AddAttackSP(CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3);
	void			AddMoverSetDestObj(CMover* pMover, OBJID objid, float fRange = 0.0f, BOOL fTransferToMe = FALSE);
	void			AddMoverDeath(CMover* pMover, CMover* pAttacker, DWORD dwMsg);
	void			AddSetMovePattern(CMover* pMover, int nPattern, const D3DXVECTOR3& vPos, FLOAT fAngle, FLOAT fAngleX);

	void			AddCreateSfxAllow(CMover* pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget);
	void			AddCreateSfxObj(CCtrl* pCtrl, DWORD dwSfxObj, float x = 0, float y = 0, float z = 0, BOOL bFlag = FALSE);
	void			AddNearSetChangeJob(CMover* pMover, int nJob, LPSKILL lpSkill);
	void			AddModifyMode(CUser* pUser);
	void			AddStateMode(CUser* pUser, BYTE nFlag);

	void			AddDefinedText(CMover* pMover, int dwText, LPCSTR lpszFormat, ...);
	void			AddBlock(LPBYTE lpBlock, u_long uBlockSize);
	void			AddBlock(LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld);
	void			AddBlock(CWorld* pWorld, const D3DXVECTOR3& vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize);
	void			AddPVendorOpen(CUser* pUser);
	void			AddPVendorClose(CUser* pUser);
	void			AddPVendorItemNum(CUser* pUser, BYTE nItem, short nVend, const char* sBuyer);
	void			AddSetHair(CUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b);
	void			AddCreateSkillEffect(CMover* pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel);
	void			AddSetStun(CMover* pMover, BOOL bApply);
	void			AddSendActMsg(CMover* pMover, OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0);
	void			AddPushPower(CMover* pMover, D3DXVECTOR3 vPos, FLOAT fPushAngle, FLOAT fPower);
	void			AddRemoveSkillInfluence(CMover* pMover, WORD wType, WORD wID);
	void			AddDoApplySkill(CCtrl* pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel);
	void			AddWorldCreateSfxObj(DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId);

	void			AddMoverBehavior(CMover* pMover, BOOL bTransferToMe = FALSE);

	void			AddUseSkill(CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime);
	void			AddEventMessage(CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId = 0);
	void			AddUseObject(CMover* pMover);
	void			AddRemoveAllSkillInfluence(CMover* pMover);
	void			AddCreateGuild(u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild);
	void			AddDestroyGuild(const char* lpszPlayer, u_long idGuild);
	void			AddSetGuild(CUser* pUser, u_long idGuild);
	void			AddGetItemElem(CUser* pUser, CItemElem* pItemElem);
	void			AddPutItemElem(CUser* pUser, CItemElem* pItemElem);
	void			AddPutItemElem(u_long uidGuild, CItemElem* pItemElem);
	void			ModifyMode(DWORD dwMode);
	void			AddSexChange(CMover* pMover);
	void			AddGameRate(FLOAT fRate, BYTE nFlag);
	void			AddChangeFace(u_long uidPlayer, DWORD dwFace);
	void			AddShout(CUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize);
	void			AddWorldMsg(const CRect* pRect, LPCTSTR lpszString);
	void			AddGameSetting();
	void			AddCommonPlace(CCtrl* pCtrl, BYTE nType);
	void			OutputStatistics(void);
	void			AddGuildCombatNextTimeWorld(DWORD dwTime, DWORD dwState);
	void			AddGuildCombatState(void);
	void			AddGuildCombatUserState(CMover* pMover);
	void			ReplaceWorld(DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer);
	void			ReplaceWorldArea(u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer);
	void			AddWorldMsg(DWORD dwWorldId, LPCTSTR lpszString);
	void			AddGCGuildStatus(u_long uidGuild, CUser* pSendUser = NULL);
	void			AddGCGuildPrecedence(CUser* pSendUser = NULL);
	void			AddGCPlayerPrecedence(CUser* pSendUser = NULL);
	void			AddGCWinGuild(void);
	void			AddGCBestPlayer(void);
	void			AddGCWarPlayerlist(u_long uidGuild = 0, CUser* pSendUser = NULL);
	void			AddGuildCombatEnterTime(DWORD dwTime);
	void			AddGCLogWorld(void);
	void			AddGCLogRealTimeWorld(CGuildCombat::__GCGETPOINT GCGetPoint);
	void			AddGCIsRequest(u_long uidGuild, BOOL bRequest);
	void			AddBlockNoLock(LPBYTE lpBlock, u_long uBlockSize);
	void			AddBlockNoLock(LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld);
	void			AddGuildMsg(u_long idGuild, LPCSTR lpsz);
	void			AddGuildMsg(CGuild* pGuild, LPCSTR lpsz);

#ifdef __S1108_BACK_END_SYSTEM
	void			AddMonsterProp();
	void			AddGMChat(int nSize);
#endif // 

	void			AddMotionArrive(CMover* pMover, OBJMSG objmsg);

	void	AddSetDestPos(CMover* pMover, CONST D3DXVECTOR3& vPos, BYTE fForward);
	void	AddPetCall(CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName);
	void	AddSetPetName(CUser* pUser, const char* szPetName);
	void	AddPetRelease(CMover* pMover);
	void	AddPetLevelup(CMover* pMover, DWORD dwPetId);
	void	AddPetFeed(CMover* pMover, WORD wEnergy);
	void	AddHonorTitleChange(CMover* pMover, int nChange);
	void	AddSetSpeedFactor(CMover* pMover, FLOAT fSpeedFactor);

	void	AddEventLua(BYTE nId, BOOL bState);
	void	AddElectionBeginCandidacy(void);
	void	AddElectionBeginVote(int nRequirement);
	void	AddElectionEndVote(u_long idPlayer);
	void	AddElectionAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate);
	void	AddElectionSetPledge(u_long idPlayer, const char* szPledge);
	void	AddElectionIncVote(u_long idPlayer, u_long idElector);
	void	AddLEventCreate(CLEComponent* pComponent);
	void	AddLEventInitialize(void);
	void	AddLEventTick(ILordEvent* pEvent);
	void	AddLordSkillUse(CUser* pUser, u_long idTarget, int nSkill);
	void	AddQueryPlayerData(u_long idPlayer);
	void	AddTaxInfo(void);
	void	AddHousingPaperingInfo(DWORD dwItemId, BOOL bSetup, DWORD dwMasterId);
	void	AddChangeMoverSfxId(CMover* pMover);
	BOOL	HasUserSameWorldnLayer(CUser* pUserSrc);

	void AddStatInfo(CMover* pMover, BOOL bRegen);
	void AddSMMode(CMover* pMover, int nType, DWORD dwTime);

#ifdef __FL_FAST_ELEMENT
	void AddElementChange(CMover* pMover, int nPosition, int nOpt);
#endif
#ifdef __FL_EQUALIZED_MODE
	void AddUpdateEquipInfo(CMover* pMover);
#endif
#ifdef __MODEL_HIDEFASH
	void AddHideFashionUpdate(CMover* pMover, int nPos, BOOL bState);
#endif
#ifdef __FL_VENDORS
	void AddNewItemToLiveVendor(CMover* pMover, BYTE iIndex);
#endif

	void KickAllPlayers();
};

#endif	// __USER_H__
