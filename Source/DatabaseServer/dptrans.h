#ifndef __DPDATABASESRVR_H__
#define __DPDATABASESRVR_H__

#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

#include "lord.h"

#undef	theClass
#define theClass	CDPTrans
#undef	theParameters
#define theParameters	CAr & ar, DPID, DPID, DPID, LPBYTE lpBuf, u_long uBufSize

class CMail;
class CMailBox;

#include "playerdata.h"
#include "Housing.h"
class CLordSkill;
class CCoupleMgr;

class CDPTrans : public CDPMng
{
public:
	//	Constructions
	CDPTrans();
	virtual	~CDPTrans();
	//	Overrides
	virtual void	SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	virtual void	UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom);
	//	Operations
	void	SendAllGuildCombat(DPID dpId);

	static	CDPTrans* GetInstance(void);
	void	SendAllPlayerData(DPID dpid);
	void	SendAddPlayerData(u_long idPlayer, PlayerData* pPlayerData);
	void	SendDeletePlayerData(u_long idPlayer);
	void	SendUpdatePlayerData(u_long idPlayer, PlayerData* pPlayerData);
	void	SendHdr(DWORD dwHdr, DPID dpid);

	void	SendBaseGameSetting(BOOL bFirst, DPID dpid);
	void	SendMonsterRespawnSetting(int nRemoveSize, int nRemoveRespawn[], DPID dpid);
	void	SendMonsterPropSetting(BOOL bFirst, DPID dpid);
	void	SendGMChat(int nCount);

	void	OnQueryMailBox(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);


	////////////////////////////////////////////////////////////////////
	void	OnQueryMailBoxCount(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnQueryMailBoxReq(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	SendMailBoxReq(u_long idReceiver, DPID dpid, BOOL bHaveMailBox, CMailBox* pMailBox);
	////////////////////////////////////////////////////////////////////


	void	SendMailBox(CMailBox* pMailBox, DPID dpid);
	void	OnQueryPostMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnQueryRemoveMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnQueryGetMailItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnQueryGetMailGold(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnReadMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	SendAllMail(DPID dpid);
	void	SendPostMail(BOOL bResult, u_long idReceiver, CMail* pMail);
	void	SendRemoveMail(u_long idReceiver, u_long nMail);
	void	SendGetMailItem(u_long idReceiver, u_long nMail, u_long uQuery);
	void	SendGetMailGold(u_long idReceiver, u_long nMail, u_long uQuery);
	void	SendReadMail(u_long idreceiver, u_long nMail);
	void	SendEventLuaState(map<BYTE, BOOL> mapState, BOOL bLuaChanged, DPID dpid = DPID_ALLPLAYERS);
	void	SendEventLuaChanged(void);
	void	OnEventLuaChanged(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	USES_PFNENTRIES;
	//	Handlers
	void	OnRemoveConnection(DPID dpid);
	void	OnJoin(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnSavePlayer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnLogItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnLogQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnCalluspXXXMultiServer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnLogPlayConnect(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnLogLevelUp(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnLogUniqueItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnChangeBankPass(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnPing(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnQueryGuildBank(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnGuildBankUpdate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnGuildContribution(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnOpenBattleServer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnCloseBattleServer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnQueryGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnInsertGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnUpdateGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnQuerySetPlayerName(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnGuildBankLogView(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnUpdatePlayerData(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnQueryRemoveGuildBankTbl(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnCalluspLoggingQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
public:
	void	OnGuidCombatInGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnGuidCombatOutGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnResultGuildCombat(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnStartGuildCombat(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnGCContinue(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	SendInGuildCombat(u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya);
	void	SendOutGuildCombat(u_long idGuild, u_long dwChannel);
	void	SendResultGuildCombat(void);
	void	SendPlayerPointGuildCombat(void);
	void	SendContinueGC(void);
	void	SendQueryRemoveGuildBankTbl(int nNo, u_long idGuild, DWORD dwItemId, SERIALNUMBER iSerialNumber, DWORD dwItemNum);


public:

	void	SendElectionAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate, BOOL bRet);

	void	SendElectionSetPledge(u_long idPlayer, const char* szPledge, BOOL bRet);

	void	SendElectionIncVote(u_long idPlayer, u_long idElector, BOOL bRet);

	void	SendElectionBeginCandidacy(void);

	void	SendElectionBeginVote(int nRequirement);

	void	SendElectionEndVote(u_long idPlayer);

	void	SendLord(DPID dpid);

	void	SendLEventCreate(CLEComponent* pComponent, BOOL bResult);

	void	SendLEventInitialize(void);


	void	SendLordSkillUse(u_long idPlayer, u_long idTarget, int nSkill, int nRet);

	void	SendLordSkillTick(CLordSkill* pSkills);

	void	SendLEventTick(ILordEvent* pEvent);
private:

	void	OnElectionAddDeposit(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionSetPledge(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionIncVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnLEventCreate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnLordSkillUse(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionProcess(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionBeginCandidacy(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionBeginVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnElectionEndVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnLEventInitialize(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

public:
	void	SendTaxInfo(DPID dpId, BOOL bConnect = FALSE, BOOL bToAllClient = FALSE);
private:
	void	OnSetSecretRoomWinGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnSetLord(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnTaxRate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnAddTax(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnApplyTaxRateNow(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

public:
	void	SendSecretRoomInfoClear(DPID dpId);
	void	SendSecretRoomTenderInfo(BYTE nContinent, DWORD dwGuildId, int nPenya, vector<DWORD>& vecMemberId, DPID dpId);
private:
	void	OnSecretRoomInsertToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnSecretRoomUpdateToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnSecretRoomLineUpToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnSecretRoomClosed(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

public:
	void	SendHousingLoadInfo(DWORD dwPlayerId, CHousing* pHousing, DPID dpId);
	void	SendHousingFurnitureList(DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd, DPID dpId);
	void	SendHousingSetupFurniture(DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId);
	void	SendHousingSetVisitAllow(DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId);
	void	SendHousingDBFailed(DWORD dwPlayerId, DWORD dwItemId, DPID dpId);
private:
	void	OnHousingLoadInfo(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnHousingReqSetFurnitureList(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnHousingReqSetupFurniture(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnHousingReqSetVisitAllow(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnHousingReqGMRemoveAll(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

public:
	void	OnPropose(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnCouple(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnDecouple(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnClearPropose(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	SendCouple(CCoupleMgr* pMgr, DPID dpid);
	void	SendProposeResult(u_long idProposer, u_long idTarget, int nResult, time_t t, DPID dpid);
	void	SendCoupleResult(u_long idProposer, u_long idTarget, int nResult);
	void	SendDecoupleResult(u_long idPlayer, int nResult);
	void	OnQueryAddCoupleExperience(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	SendAddCoupleExperience(u_long idPlayer, int nExperience);


	void	OnLogInstanceDungeon(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

	void	OnBuyGuildHouse(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnGuildHousePacket(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void	OnLogGuildFurniture(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);

#ifdef __FL_GW_STATUE_V2
	void	OnInstallStatues(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif
#ifdef __FL_EQUALIZED_MODE
	void OnSaveTemporalPreset(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void OnGetTemporaryPresets(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif
#ifdef __FL_DAILY_REWARDS
	void OnUpdateDailyGift(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif
#ifdef __FL_VENDORS
	void OnAddVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void OnUpdateVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void OnDeleteVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void OnGetVendors(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
#endif
#ifdef __FL_CASH_SHOP
private:
	void OnGetCShopBalance(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
	void OnBuyCShopItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize);
public:
	void SendCShop(DPID dpid);
	void SendCShopBalance(DPID dpid, u_long idPlayer, int nDonate, int nVote, BOOL bPurchaseError);
	void SendCShopItem(DPID dpid, u_long idBuyer, u_long idReceiver, int nNewDonate, int nNewVote, DWORD dwItemID, int nItemNum);
#endif
};

#endif	// __DPDATABASESRVR_H__