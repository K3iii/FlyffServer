#ifndef __DPDATABASECLIENT_H__
#define __DPDATABASECLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"
#include "guild.h"

#include "eveschool.h"
#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#undef	theClass
#define theClass	CDPDatabaseClient
#undef theParameters
#define theParameters CAr & ar, DPID, DPID

class CUser;
class CDPDatabaseClient : public CDPMng
{
private:
	int		m_cbPing;
	BOOL	m_bAlive;

public:
	//	Constructions
	CDPDatabaseClient();
	virtual	~CDPDatabaseClient();

	//	Operations
	virtual	void	SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	virtual	void	UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);

#ifdef __LAYER_1015
	void	SavePlayer(CUser* pUser, DWORD dwWorldId, D3DXVECTOR3& vPos, int nLayer, BOOL bLogout = FALSE);
#else	// __LAYER_1015
	void	SavePlayer(CUser* pUser, DWORD dwWorldId, const D3DXVECTOR3& vPos, BOOL bLogout = FALSE);
#endif	// __LAYER_1015

	void	SendLogConnect(CUser* pUser);
	void	SendLogLevelUp(CMover* pSender, int Action);
#ifndef __FL_FIX_USELESS_LOGS
	void	SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption);
	void	SendLogUniqueItem2(CMover* pMover, ItemProp* pItemProp, int nOption);
#endif
	void	SendChangeBankPass(const char* szName, const char* szNewPass, u_long uidPlayer);
	void	SendPing(void);
	void	SendInGuildCombat(u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya);
	void	SendOutGuildCombat(u_long idGuild);
	void	SendGuildCombatResult(void);
	void	SendGuildCombatStart(void);
	void	SendGuildcombatContinue(int nGuildCombatID, u_long uidGuild, int nWinCount);
#ifdef __INVALID_LOGIN_0320
	void	CalluspXXXMultiServer(u_long uKey, CUser* pUser);
#else	// __INVALID_LOGIN_0320
	void	CalluspXXXMultiServer(u_long uKey, u_long idPlayer);
#endif	// __INVALID_LOGIN_0320
	void	SendQueryGuildQuest(void);
	void	SendInsertGuildQuest(u_long idGuild, int nId);
	void	SendUpdateGuildQuest(u_long idGuild, int nId, int nState);
	void	SendQueryGuildBank();
	void	SendGuildContribution(CONTRIBUTION_CHANGED_INFO& info, BYTE nLevelUp, LONG nMemberLevel);
	void	SendQuerySetPlayerName(u_long idPlayer, const char* lpszPlayer, DWORD dwData);
	void	SendQueryMailBox(u_long idReceiver);


	////////////////////////////////////////////////////////////////////
	void	SendQueryMailBoxReq(u_long idReceiver);
	void	SendQueryMailBoxCount(u_long idReceiver, int nCount);
	void	OnMailBoxReq(CAr& ar, DPID, DPID);
	////////////////////////////////////////////////////////////////////


	void	OnMailBox(CAr& ar, DPID, DPID);
	void	SendQueryPostMail(u_long idReceiver, u_long idSender, CItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText);
	void	SendQueryRemoveMail(u_long idReceiver, u_long nMail);
	void	SendQueryGetMailItem(u_long idReceiver, u_long nMail);
	void	SendQueryGetMailGold(u_long idReceiver, u_long nMail);
	void	SendQueryReadMail(u_long idReceiver, u_long nMail);
	void	OnPostMail(CAr& ar, DPID, DPID);
	void	OnRemoveMail(CAr& ar, DPID, DPID);
	void	OnGetMailItem(CAr& ar, DPID, DPID);
	void	OnGetMailGold(CAr& ar, DPID, DPID);
	void	OnReadMail(CAr& ar, DPID, DPID);
	void	OnAllMail(CAr& ar, DPID, DPID);
	void	CalluspLoggingQuest(u_long idPlayer, int nQuest, int nState, const char* pszComment = "");
	void	SendQueryGetGuildBankLogList(u_long idReceiver, DWORD	idGuild, BYTE byListType);
	void	SendEventLuaChanged(void);
	void	SendUpdatePlayerData(CUser* pUser);

private:
	USES_PFNENTRIES;

	//	Handlers
	void	OnJoin(CAr& ar, DPID dpidCache, DPID dpidUser);
	void	OnAllPlayerData(CAr& ar, DPID, DPID);
	void	OnAddPlayerData(CAr& ar, DPID, DPID);
	void	OnDeletePlayerData(CAr& ar, DPID, DPID);
	void	OnUpdatePlayerData(CAr& ar, DPID, DPID);
	void	OnGCPlayerPoint(CAr& ar);
	void	OnAllGuildCombat(CAr& ar, DPID, DPID);
	void	OnInGuildCombat(CAr& ar, DPID, DPID);
	void	OnOutGuildCombat(CAr& ar, DPID, DPID);
	void	OnResultGuildCombat(CAr& ar, DPID, DPID);
	void	OnPlayerPointGuildCombat(CAr& ar, DPID, DPID);
	void	OnContinueGC(CAr& ar, DPID, DPID);
	void	OnBaseGameSetting(CAr& ar, DPID, DPID);
	void	OnMonsterRespawnSetting(CAr& ar, DPID, DPID);
	void	OnMonsterPropSetting(CAr& ar, DPID, DPID);
	void	OnGMChat(CAr& ar, DPID, DPID);
	void	OnGuildBank(CAr& ar, DPID, DPID);
	void	OnQueryGuildQuest(CAr& ar, DPID, DPID);
	void	OnPing(CAr& ar, DPID, DPID);
	void	OnQueryRemoveGuildBankTbl(CAr& ar, DPID, DPID);
	void	SendQueryRemoveGuildBankTbl(int nNo, DWORD dwRemoved);

#if __VER >= 9 // __EVENTLUA
	void	OnEventStateLua(CAr& ar, DPID, DPID);
	void	OnEventLuaChanged(CAr& ar, DPID, DPID);
#endif // __EVENTLUA

#if __VER >= 11
	void	OnGuildBankLogViewFromDB(CAr& ar, DPID, DPID);
#endif

#if __VER >= 12 // __LORD
private:
	void	OnElectionAddDeposit(CAr& ar, DPID, DPID);
	void	OnElectionSetPledge(CAr& ar, DPID, DPID);
	void	OnElectionIncVote(CAr& ar, DPID, DPID);
	void	OnElectionBeginCandidacy(CAr& ar, DPID, DPID);
	void	OnElectionBeginVote(CAr& ar, DPID, DPID);
	void	OnElectionEndVote(CAr& ar, DPID, DPID);
	void	OnLord(CAr& ar, DPID, DPID);
	void	OnLEventCreate(CAr& ar, DPID, DPID);
	void	OnLEventInitialize(CAr& ar, DPID, DPID);
	void	OnLordSkillUse(CAr& ar, DPID, DPID);
	void	OnLordSkillTick(CAr& ar, DPID, DPID);
	void	OnLEventTick(CAr& ar, DPID, DPID);
public:
	void	SendElectionAddDeposit(u_long idPlayer, __int64 iDeposit);
	void	SendElectionSetPledge(u_long idPlayer, const char* szPledge);
	void	SendElectionIncVote(u_long idPlayer, u_long idElector);
#ifndef __FL_UPDATE_LORDEVENT
	void	SendLEventCreate(u_long idPlayer, int iEEvent, int iIEvent);
#else
	void	SendLEventCreate(u_long idPlayer, int iEEvent, int iIEvent, int iPEvent);
#endif
	void	SendLordSkillUse(u_long idPlayer, u_long idTarget, int nSkill);
	// operator commands
	void	SendElectionProcess(BOOL bRun);
	void	SendElectionBeginCandidacy(void);
	void	SendElectionBeginVote(void);
	void	SendElectionBeginEndVote(void);
	void	SendLEventInitialize(void);
#endif	// __LORD

#if __VER >= 12 // __TAX
private:
	void	OnTaxInfo(CAr& ar, DPID, DPID);
public:
	void	SendSecretRoomWinGuild(BYTE nCont, DWORD dwGuildId);
	void	SendLord(DWORD dwIdPlayer);
	void	SendTaxRate(BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate);
	void	SendAddTax(BYTE nCont, int nTax, BYTE nTaxKind);
	void	SendApplyTaxRateNow();
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
private:
	void	OnSecretRoomInfoClear(CAr& ar, DPID, DPID);
	void	OnSecretRoomTenderInfo(CAr& ar, DPID, DPID);
public:
	void	SendSecretRoomInsertToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender);
	void	SendSecretRoomUpdateToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender, char chState);
	void	SendSecretRoomInsertLineUpToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender);
	void	SendSecretRoomClosed();
#endif // __SECRET_ROOM


#if __VER >= 13 // __HOUSING
	void	OnHousingLoadInfo(CAr& ar, DPID, DPID);
	void	OnHousingSetFunitureList(CAr& ar, DPID, DPID);
	void	OnHousingSetupFuniture(CAr& ar, DPID, DPID);
	void	OnHousingSetVisitAllow(CAr& ar, DPID, DPID);
	void	OnHousingDBFailed(CAr& ar, DPID, DPID);
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	void	SendPropose(u_long idProposer, u_long idTarget);
	void	SendCouple(u_long idProposer, u_long idTarget);
	void	SendDecouple(u_long idPlayer);
	void	SendClearPropose();
	void	OnProposeResult(CAr& ar, DPID, DPID);
	void	OnCoupleResult(CAr& ar, DPID, DPID);
	void	OnDecoupleResult(CAr& ar, DPID, DPID);
	void	OnCouple(CAr& ar, DPID, DPID);
#if __VER >= 13 // __COUPLE_1202
	void	SendQueryAddCoupleExperience(u_long idPlayer, int nExperience);
	void	OnAddCoupleExperience(CAr& ar, DPID, DPID);
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
	void	OnLoadGuildHouse(CAr& ar, DPID, DPID);
	void	OnBuyGuildHouse(CAr& ar, DPID, DPID);
	void	OnRemoveGuildHouse(CAr& ar, DPID, DPID);
	void	OnGuildHousePacket(CAr& ar, DPID, DPID);
	void	SendLogGuildFurniture(DWORD dwGuildId, GH_Fntr_Info& gfi, char chState);
#endif // __GUILD_HOUSE

#ifdef __FL_GW_STATUE_V2
	void	SendRequestToInstallStatues(u_long idMvp, u_long idLeader);
	void	OnInstallStatues(CAr& ar, DPID, DPID);
#endif
#ifdef __FL_EQUALIZED_MODE
	void SendSaveTmpPreset(CUser* pUser);
	void SendGetTmpPresets();
	void OnGetTemporalPresets(CAr& ar, DPID, DPID);
#endif
#ifdef __FL_DAILY_REWARDS
	void SendUpdateDailyGift(u_long idPlayer, int nValue);
#endif
#ifdef __FL_VENDORS
	void SendAddVendor(CMover* pMoverVendor);
	void SendUpdateVendor(CMover* pMoverVendor);
	void SendDeleteVendor(u_long idPlayer);
	void SendGetVendors();
	void OnGetVendors(CAr& ar, DPID, DPID);
#endif
#ifdef __FL_CASH_SHOP
private:
	void OnCShop(CAr& ar, DPID, DPID);
	void OnCShopBalance(CAr& ar, DPID, DPID);
	void OnCShopItem(CAr& ar, DPID, DPID);
public:
	void SendGetCShopBalance(u_long idPlayer, char* lpszAccount);
	void SendBuyCShopItem(u_long idPlayer, char* lpszAccount, DWORD dwItemID, int nItemAmount, int nTotalPrice, BOOL bDonate, u_long idReceiver);
#endif
#ifdef __FL_RECORD_BOOK
private:
	void OnRecordBook(CAr& ar, DPID, DPID);
#endif
};

#endif	// __DPDATABASECLIENT_H__