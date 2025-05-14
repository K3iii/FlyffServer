#include "stdafx.h"
#if __VER >= 11 // __SYS_PLAYER_DATA
#include "dpcoresrvr.h"
extern	CDPCoreSrvr	g_dpCoreSrvr;
#endif	// __SYS_PLAYER_DATA
#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;
#include "dptrans.h"

#include "..\_network\objects\Obj.h"
#include "guild.h"
#include "post.h"
#include "spevent.h"

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#if __VER >= 12 // __LORD
#include "tlord.h"
#endif	// __LORD
#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

extern	APP_INFO	g_appInfo;

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoomDBMng.h"
#endif // __SECRET_ROOM
#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX
#if __VER >= 13 // __HOUSING
#include "HousingDBCtrl.h"
#endif // __HOUSING
#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouseDBCtrl.h"
#endif // __GUILD_HOUSE
#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

CDPTrans::CDPTrans()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_JOIN, &CDPTrans::OnJoin);
	ON_MSG(PACKETTYPE_SAVE_PLAYER, &CDPTrans::OnSavePlayer);
	ON_MSG(PACKETTYPE_LOG_PLAY_CONNECT, &CDPTrans::OnLogPlayConnect);
	ON_MSG(PACKETTYPE_LOG_LEVELUP, &CDPTrans::OnLogLevelUp);
	ON_MSG(PACKETTYPE_LOG_UNIQUEITEM, &CDPTrans::OnLogUniqueItem);
	ON_MSG(PACKETTYPE_LOG_ALLITEM, &CDPTrans::OnLogItem);
	ON_MSG(PACKETTYPE_LOG_QUEST, &CDPTrans::OnLogQuest);
	ON_MSG(PACKETTYPE_CHANGEBANKPASS, &CDPTrans::OnChangeBankPass);
	ON_MSG(PACKETTYPE_GUILD_BANK_QUERY, &CDPTrans::OnQueryGuildBank);
	ON_MSG(PACKETTYPE_GUILD_BANK_UPDATE, &CDPTrans::OnGuildBankUpdate);
	ON_MSG(PACKETTYPE_WD_GUILD_CONTRIBUTION, &CDPTrans::OnGuildContribution);
	ON_MSG(PACKETTYPE_QUERYGUILDQUEST, &CDPTrans::OnQueryGuildQuest);
	ON_MSG(PACKETTYPE_INSERTGUILDQUEST, &CDPTrans::OnInsertGuildQuest);
	ON_MSG(PACKETTYPE_UPDATEGUILDQUEST, &CDPTrans::OnUpdateGuildQuest);
	ON_MSG(PACKETTYPE_QUERYSETPLAYERNAME, &CDPTrans::OnQuerySetPlayerName);
	ON_MSG(PACKETTYPE_PING, &CDPTrans::OnPing);
	ON_MSG(PACKETTYPE_QUERYMAILBOX, &CDPTrans::OnQueryMailBox);


	////////////////////////////////////////////////////////////////////
	ON_MSG(PACKETTYPE_QUERYMAILBOX_COUNT, &CDPTrans::OnQueryMailBoxCount);
	ON_MSG(PACKETTYPE_QUERYMAILBOX_REQ, &CDPTrans::OnQueryMailBoxReq);
	////////////////////////////////////////////////////////////////////


	ON_MSG(PACKETTYPE_QUERYPOSTMAIL, &CDPTrans::OnQueryPostMail);
	ON_MSG(PACKETTYPE_QUERYREMOVEMAIL, &CDPTrans::OnQueryRemoveMail);
	ON_MSG(PACKETTYPE_QUERYGETMAILITEM, &CDPTrans::OnQueryGetMailItem);
	ON_MSG(PACKETTYPE_QUERYGETMAILGOLD, &CDPTrans::OnQueryGetMailGold);
	ON_MSG(PACKETTYPE_READMAIL, &CDPTrans::OnReadMail);

	ON_MSG(PACKETTYPE_START_GUILDCOMBAT, &CDPTrans::OnStartGuildCombat);
	ON_MSG(PACKETTYPE_IN_GUILDCOMBAT, &CDPTrans::OnGuidCombatInGuild);
	ON_MSG(PACKETTYPE_OUT_GUILDCOMBAT, &CDPTrans::OnGuidCombatOutGuild);
	ON_MSG(PACKETTYPE_RESULT_GUILDCOMBAT, &CDPTrans::OnResultGuildCombat);
	ON_MSG(PACKETTYPE_CONTINUE_GUILDCOMBAT, &CDPTrans::OnGCContinue);

	ON_MSG(PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, &CDPTrans::OnQueryRemoveGuildBankTbl);
	ON_MSG(PACKETTYPE_CALL_USPLOGGINGQUEST, &CDPTrans::OnCalluspLoggingQuest);
	ON_MSG(PACKETTYPE_CALL_XXX_MULTI_SERVER, &CDPTrans::OnCalluspXXXMultiServer);

#if __VER >= 9 // __EVENTLUA
	ON_MSG(PACKETTYPE_EVENTLUA_CHANGED, &CDPTrans::OnEventLuaChanged);
#endif // __EVENTLUA

#if __VER >= 11 // __SYS_PLAYER_DATA
	ON_MSG(PACKETTYPE_UPDATE_PLAYER_DATA, &CDPTrans::OnUpdatePlayerData);
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11
	ON_MSG(PACKETTYPE_GUILDLOG_VIEW, &CDPTrans::OnGuildBankLogView);
#endif

#if __VER >= 12 // __LORD

	ON_MSG(PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPTrans::OnElectionAddDeposit);

	ON_MSG(PACKETTYPE_ELECTION_SET_PLEDGE, &CDPTrans::OnElectionSetPledge);

	ON_MSG(PACKETTYPE_ELECTION_INC_VOTE, &CDPTrans::OnElectionIncVote);

	ON_MSG(PACKETTYPE_L_EVENT_CREATE, &CDPTrans::OnLEventCreate);

	ON_MSG(PACKETTYPE_LORD_SKILL_USE, &CDPTrans::OnLordSkillUse);

	ON_MSG(PACKETTYPE_ELECTION_PROCESS, &CDPTrans::OnElectionProcess);

	ON_MSG(PACKETTYPE_ELECTION_BEGIN_CANDIDACY, &CDPTrans::OnElectionBeginCandidacy);

	ON_MSG(PACKETTYPE_ELECTION_BEGIN_VOTE, &CDPTrans::OnElectionBeginVote);

	ON_MSG(PACKETTYPE_ELECTION_END_VOTE, &CDPTrans::OnElectionEndVote);

	ON_MSG(PACKETTYPE_L_EVENT_INITIALIZE, &CDPTrans::OnLEventInitialize);
#endif	// __LORD

#if __VER >= 12 // __TAX
	ON_MSG(PACKETTYPE_TAX_SET_SECRETROOM_WINNER, &CDPTrans::OnSetSecretRoomWinGuild);
	ON_MSG(PACKETTYPE_TAX_SET_LORD, &CDPTrans::OnSetLord);
	ON_MSG(PACKETTYPE_TAX_SET_TAXRATE, &CDPTrans::OnTaxRate);
	ON_MSG(PACKETTYPE_TAX_ADDTAX, &CDPTrans::OnAddTax);
	ON_MSG(PACKETTYPE_TAX_APPLY_TAXRATE_NOW, &CDPTrans::OnApplyTaxRateNow);
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
	ON_MSG(PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, &CDPTrans::OnSecretRoomInsertToDB);
	ON_MSG(PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, &CDPTrans::OnSecretRoomUpdateToDB);
	ON_MSG(PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, &CDPTrans::OnSecretRoomLineUpToDB);
	ON_MSG(PACKETTYPE_SECRETROOM_CLOSED, &CDPTrans::OnSecretRoomClosed);
#endif // __SECRET_ROOM


#if __VER >= 13 // __HOUSING
	ON_MSG(PACKETTYPE_HOUSING_LOADINFO, &CDPTrans::OnHousingLoadInfo);
	ON_MSG(PACKETTYPE_HOUSING_FURNITURELIST, &CDPTrans::OnHousingReqSetFurnitureList);
	ON_MSG(PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPTrans::OnHousingReqSetupFurniture);
	ON_MSG(PACKETTYPE_HOUSING_SETVISITALLOW, &CDPTrans::OnHousingReqSetVisitAllow);
	ON_MSG(PACKETTYPE_HOUSING_GM_REMOVEALL, &CDPTrans::OnHousingReqGMRemoveAll);
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
	ON_MSG(PACKETTYPE_PROPOSE, &CDPTrans::OnPropose);
	ON_MSG(PACKETTYPE_COUPLE, &CDPTrans::OnCouple);
	ON_MSG(PACKETTYPE_DECOUPLE, &CDPTrans::OnDecouple);
	ON_MSG(PACKETTYPE_CLEAR_PROPOSE, &CDPTrans::OnClearPropose);
#if __VER >= 13 // __COUPLE_1202
	ON_MSG(PACKETTYPE_ADD_COUPLE_EXPERIENCE, &CDPTrans::OnQueryAddCoupleExperience);
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
	ON_MSG(PACKETTYPE_GUILDHOUSE_BUY, &CDPTrans::OnBuyGuildHouse);
	ON_MSG(PACKETTYPE_GUILDHOUSE_PACKET, &CDPTrans::OnGuildHousePacket);
	ON_MSG(PACKETTYPE_GUILDFURNITURE_LOG, &CDPTrans::OnLogGuildFurniture);
#endif // __GUILD_HOUSE

#ifdef __FL_GW_STATUE_V2
	ON_MSG(PACKETTYPE_REQUEST_INSTALL_STATUES, &CDPTrans::OnInstallStatues);
#endif

#ifdef __FL_EQUALIZED_MODE
	ON_MSG(PACKETTYPE_SAVEEQUIPPRESET, &CDPTrans::OnSaveTemporalPreset);
	ON_MSG(PACKETTYPE_GETEQUIPPRESETS, &CDPTrans::OnGetTemporaryPresets);
#endif

#ifdef __FL_DAILY_REWARDS
	ON_MSG(PACKETTYPE_COLLECT_DAILY_GIFT, &CDPTrans::OnUpdateDailyGift);
#endif

#ifdef __FL_VENDORS
	ON_MSG(PACKETTYPE_VENDOR_ADD, &CDPTrans::OnAddVendor);
	ON_MSG(PACKETTYPE_VENDOR_DELETE, &CDPTrans::OnDeleteVendor);
	ON_MSG(PACKETTYPE_VENDOR_UPDATE, &CDPTrans::OnUpdateVendor);
	ON_MSG(PACKETTYPE_VENDORS_GET, &CDPTrans::OnGetVendors);
#endif

#ifdef __FL_CASH_SHOP
	ON_MSG(PACKETTYPE_CSHOP_BALANCE, &CDPTrans::OnGetCShopBalance);
	ON_MSG(PACKETTYPE_CSHOP_ITEM, &CDPTrans::OnBuyCShopItem);
#endif
}

CDPTrans::~CDPTrans()
{
}

CDPTrans* CDPTrans::GetInstance(void)
{
	static	CDPTrans sDPTrans;
	return &sDPTrans;
}

void CDPTrans::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	//	SYSTEMTIME st;

	switch (lpMsg->dwType)
	{
	case DPSYS_CREATEPLAYERORGROUP:
	{
		LPDPMSG_CREATEPLAYERORGROUP lpCreatePlayer = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
#ifdef __JEFF_FIX_0
		LPDB_OVERLAPPED_PLUS pov = g_DbManager.AllocRequest();
		pov->nQueryMode = QM_ALL_PLAYER_DATA;
		pov->dpid = lpCreatePlayer->dpId;
		PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &pov->Overlapped);
#else	// __JEFF_FIX_0
		SendAllPlayerData(lpCreatePlayer->dpId);
#endif	// __JEFF_FIX_0

		CTLord::Instance()->PostRequest(CTLord::eInit, NULL, 0, lpCreatePlayer->dpId);
		CCoupleHelper::Instance()->PostRequest(CCoupleHelper::eTransfer, NULL, 0, lpCreatePlayer->dpId);

#ifdef __S1108_BACK_END_SYSTEM
		SendBaseGameSetting(TRUE, lpCreatePlayer->dpId);
		SendMonsterRespawnSetting(0, NULL, lpCreatePlayer->dpId);
		SendMonsterPropSetting(TRUE, lpCreatePlayer->dpId);
#endif // __S1108_BACK_END_SYSTEM

		PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, lpCreatePlayer->dpId, NULL);

#ifndef __JEFF_FIX_0
		SendAllMail(lpCreatePlayer->dpId);
#endif	// __JEFF_FIX_0

		prj.m_EventLua.m_Access.Enter();
		vector<BYTE> vecEventList = prj.m_EventLua.GetEventList();
		prj.m_EventLua.m_Access.Leave();
		if (!vecEventList.empty())
		{
			map<BYTE, BOOL> mapEventList;
			for (DWORD i = 0; i < vecEventList.size(); i++)
				mapEventList.insert(make_pair(vecEventList[i], TRUE));
			SendEventLuaState(mapEventList, FALSE, lpCreatePlayer->dpId);
		}

		CSecretRoomDBMng::GetInstance()->PostRequest(QUERY_SECRETROOM_LOAD, NULL, 0, lpCreatePlayer->dpId);
		CTax::GetInstance()->LoadTaxInfo(lpCreatePlayer->dpId);
		GuildHouseDBMng->PostRequest(GUILDHOUSE_SEND, NULL, 0, lpCreatePlayer->dpId);
#ifdef __FL_RECORD_BOOK
		CRecordBook::GetInstance()->SendRecordBook(lpCreatePlayer->dpId);
#endif

#ifdef __FL_CASH_SHOP
		LPDB_OVERLAPPED_PLUS pov2 = g_DbManager.AllocRequest();
		pov2->nQueryMode = CSHOP_LOAD;
		pov2->dpid = lpCreatePlayer->dpId;
		PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &pov2->Overlapped);
#endif

		break;
	}
	case DPSYS_DESTROYPLAYERORGROUP:
	{
		LPDPMSG_DESTROYPLAYERORGROUP lpDestroyPlayer = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
		OnRemoveConnection(lpDestroyPlayer->dpId);
		break;
	}
	}
}

void CDPTrans::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	static size_t	nSize = sizeof(DPID);

	CAr ar((LPBYTE)lpMsg + (nSize + nSize), dwMsgSize - (nSize + nSize));
	GETTYPE(ar);
	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn) {
		(this->*(pfn))(ar, idFrom, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + nSize), (LPBYTE)lpMsg + nSize + nSize + nSize, dwMsgSize - (nSize + nSize + nSize));
	}
	else {

	}
}

void CDPTrans::OnRemoveConnection(DPID dpid)
{

}

void CDPTrans::SendAllGuildCombat(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ADD_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	g_DbManager.SerializeGuildCombat(ar);
	SEND(ar, this, dpId);
}

void CDPTrans::SendAllPlayerData(DPID dpid)
{

	int nTotal = CPlayerDataCenter::GetInstance()->size();
	int nFirst = 0;
	int nCount = 10000;
	while (nFirst < nTotal)
	{
		BEFORESENDDUAL(ar, PACKETTYPE_ALL_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN);
		int nRet = CPlayerDataCenter::GetInstance()->Serialize(ar, nFirst, nCount);
		nFirst += nRet;
		SEND(ar, this, dpid);
	}

}

void CDPTrans::SendAddPlayerData(u_long idPlayer, PlayerData* pPlayerData)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ADD_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(pPlayerData->szPlayer);
	ar.Write(&pPlayerData->data, sizeof(sPlayerData));
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendDeletePlayerData(u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_DELETE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendUpdatePlayerData(u_long idPlayer, PlayerData* pPlayerData)
{
	BEFORESENDDUAL(ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.Write(&pPlayerData->data, sizeof(sPlayerData));
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendHdr(DWORD dwHdr, DPID dpid)
{
	BEFORESENDDUAL(ar, dwHdr, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, dpid);
}


void CDPTrans::SendBaseGameSetting(BOOL bFirst, DPID dpid)
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL(ar, PACKETTYPE_BASEGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << bFirst;
	ar << prj.m_bBaseGameSetting;
	if (prj.m_bBaseGameSetting)
	{
		ar << prj.m_fMonsterExpRate << prj.m_fGoldDropRate << prj.m_fItemDropRate << prj.m_fMonsterHitRate;
		ar << prj.m_fShopCost << prj.m_fMonsterRebirthRate << prj.m_fMonsterHitpointRate << prj.m_fMonsterAggressiveRate << prj.m_fMonsterRespawnRate;
	}
	//	SEND( ar, this, DPID_ALLPLAYERS );
	SEND(ar, this, dpid);
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterRespawnSetting(int nRemoveSize, int nRemoveRespawn[], DPID dpid)
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL(ar, PACKETTYPE_MONSTERRESPAWNSETTING, DPID_UNKNOWN, DPID_UNKNOWN);
	if (nRemoveRespawn == NULL)
	{
		ar << prj.m_nMonsterRespawnSize;
		for (DWORD i = 0; i < prj.m_nMonsterRespawnSize; ++i)
		{
			ar << prj.m_aMonsterRespawn[i].nRespawnIndex;
			ar.WriteString(prj.m_aMonsterRespawn[i].szMonsterName);
			ar << prj.m_aMonsterRespawn[i].vPos;
			ar << prj.m_aMonsterRespawn[i].nQuantity;
			ar << prj.m_aMonsterRespawn[i].nAggressive;
			ar << prj.m_aMonsterRespawn[i].nRange;
			ar << prj.m_aMonsterRespawn[i].nTime;
		}
	}
	else
	{
		ar << prj.m_nAddMonsterRespawnSize;
		for (int i = 0; i < prj.m_nAddMonsterRespawnSize; ++i)
		{
			ar << prj.n_aAddRespawn[i].nRespawnIndex;
			ar.WriteString(prj.n_aAddRespawn[i].szMonsterName);
			ar << prj.n_aAddRespawn[i].vPos;
			ar << prj.n_aAddRespawn[i].nQuantity;
			ar << prj.n_aAddRespawn[i].nAggressive;
			ar << prj.n_aAddRespawn[i].nRange;
			ar << prj.n_aAddRespawn[i].nTime;
		}
	}

	ar << nRemoveSize;
	for (int i = 0; i < nRemoveSize; ++i)
	{
		ar << nRemoveRespawn[i];
	}

	SEND(ar, this, dpid);
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendMonsterPropSetting(BOOL bFirst, DPID dpid)
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL(ar, PACKETTYPE_MONSTERPROPGAMESETTING, DPID_UNKNOWN, DPID_UNKNOWN);
	if (bFirst == TRUE)
	{
		ar << prj.m_nMonsterPropSize;
		for (DWORD i = 0; i < prj.m_nMonsterPropSize; ++i)
		{
			ar.WriteString(prj.m_aMonsterProp[i].szMonsterName);
			ar << prj.m_aMonsterProp[i].nHitPoint;
			ar << prj.m_aMonsterProp[i].nAttackPower;
			ar << prj.m_aMonsterProp[i].nDefence;
			ar << prj.m_aMonsterProp[i].nExp;
			ar << prj.m_aMonsterProp[i].nItemDrop;
			ar << prj.m_aMonsterProp[i].nPenya;
		}
	}
	else
	{
		ar << prj.m_nAddMonsterPropSize;
		for (int i = 0; i < prj.m_nAddMonsterPropSize; ++i)
		{
			ar.WriteString(prj.m_aAddProp[i].szMonsterName);
			ar << prj.m_aAddProp[i].nHitPoint;
			ar << prj.m_aAddProp[i].nAttackPower;
			ar << prj.m_aAddProp[i].nDefence;
			ar << prj.m_aAddProp[i].nExp;
			ar << prj.m_aAddProp[i].nItemDrop;
			ar << prj.m_aAddProp[i].nPenya;
		}
	}

	ar << prj.m_nRemoveMonsterPropSize;
	for (int i = 0; i < prj.m_nRemoveMonsterPropSize; ++i)
	{
		ar.WriteString(prj.m_aRemoveProp[i]);
	}

	SEND(ar, this, dpid);
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::SendGMChat(int nCount)
{
#ifdef __S1108_BACK_END_SYSTEM
	BEFORESENDDUAL(ar, PACKETTYPE_GAMEMASTER_CHATTING, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nCount;
	for (int i = 0; i < nCount; ++i)
	{
		ar.WriteString(prj.m_chGMChat[i]);
	}
	SEND(ar, this, DPID_ALLPLAYERS);
#endif // __S1108_BACK_END_SYSTEM
}

void CDPTrans::OnJoin(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CAr ar1;
	ar1 << PACKETTYPE_JOIN;
	ar1.Write(lpBuf, uBufSize);
	ar1 << dpid << dpidCache << dpidUser;

	int nBlockSize;
	LPBYTE pBlock = ar1.GetBuffer(&nBlockSize);
	g_dpAccountClient.Send(pBlock, nBlockSize, DPID_SERVERPLAYER);
}

void CDPTrans::OnSavePlayer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHAR sAccount[MAX_ACCOUNT] = { 0, };
	DWORD dwWorldID;
	D3DXVECTOR3 vPos;
	BOOL bLogout = FALSE;
	CMover* pMover = NULL;
	BYTE nSlot;
	ACCOUNT_CACHE* pCache;

	ar >> bLogout;
	ar.ReadString(sAccount, MAX_ACCOUNT);
	ar >> nSlot;
	ar >> dwWorldID >> vPos;
#ifdef __LAYER_1015
	int nLayer;
	ar >> nLayer;
#endif	// __LAYER_1015

	if (nSlot >= 3)
	{
		Error("CDPTrans::OnSavePlayer() : Invalid Player Slot [%d] account: %s", nSlot, sAccount);
		return;
	}

	g_DbManager.m_AddRemoveLock.Enter();
	pCache = g_DbManager.m_AccountCacheMgr.Find(sAccount);
	if (pCache)
		pMover = pCache->pMover[nSlot];

	if (pCache == NULL || pMover == NULL)
	{
		Error("CDPTrans::OnSavePlayer - Cache Hit Failed, AccountCahce = %p, pMover = %p, Account = %s, slot = %d", pCache, pMover, sAccount, nSlot);
		g_DbManager.m_AddRemoveLock.Leave();
		return;
	}


	pCache->m_tmLastAccess = ::time(NULL);

	pMover->Lock();
	pMover->Serialize(ar);
	pMover->m_UserTaskBar.Serialize(ar);

	DWORD dwState;
	ar >> dwState;
	pMover->m_RTMessenger.SetState(dwState);

#ifndef __JEFF_9_20
	ar >> pMover->m_dwFlyTime;
#endif	// __JEFF_9_20
	ar >> pMover->m_dwReturnWorldID;
	ar >> pMover->m_vReturnPos;

	pMover->m_dwWorldID = dwWorldID;
	pMover->m_vPos = vPos;
#ifdef __LAYER_1015
	pMover->SetLayer(nLayer);
#endif	// __LAYER_1015
	pMover->m_dwMode &= ~FRESH_MODE;	// ata2k

#ifdef __FL_BATTLE_PASS
	int nCurrentBattlePassID;
	ar >> nCurrentBattlePassID;
	pMover->SetLastBattlePassID(nCurrentBattlePassID);
#endif

#ifdef __FL_RECORD_BOOK
	for (int i = 0; i < MAX_RECORDTYPE; i++)
		pMover->m_arrayRecords[i].SerializeUpdated(ar);

	CRecordBook::GetInstance()->UpdateGlobalRecords(pMover);
#endif

	for (int j = 0; j < 3; ++j)
	{
		pCache->m_Bank[j].Copy(pMover->m_Bank[j]);
		pCache->m_idPlayerBank[j] = pMover->m_idPlayerBank[j];
		pCache->m_dwGoldBank[j] = pMover->m_dwGoldBank[j];
	}

	if (bLogout)
		pMover->m_dwPeriodicTick = 0;

	//	u_long idPlayer		= pMover->m_idPlayer;
	CMover* pMover2 = new CMover;
	pMover2->Copy(pMover, TRUE);
	pMover->PeriodTick();

	pMover->Unlock();

	g_DbManager.m_AddRemoveLock.Leave();
#ifdef __INVALID_LOGIN_0612
	pMover2->m_bLogout = bLogout;
	lstrcpy(pMover2->m_szAccount, sAccount);
#endif	// __INVALID_LOGIN_0612

	PostQueuedCompletionStatus(g_DbManager.m_hIOCPUpdate, (DWORD)pMover2, (DWORD)0, NULL);
}

void CDPTrans::OnLogPlayConnect(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_PLAY_CONNECT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}


void CDPTrans::OnLogLevelUp(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_LEVELUP;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnLogUniqueItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_UNIQUEITEM;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnLogItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_ITEM;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnLogQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_QUEST;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnChangeBankPass(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = CHANGE_BANKPASS;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnQueryGuildBank(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = GUILD_BANK_QUERY;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}


void CDPTrans::OnGuildBankUpdate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = GUILD_BANK_UPDAATE;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnGuildContribution(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = UPDATE_GUILD_CONTRIBUTION;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnQueryGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->nQueryMode = QM_QUERYGUILDQUEST;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnInsertGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_INSERTGUILDQUEST;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnUpdateGuildQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_UPDATEGUILDQUEST;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}


void CDPTrans::OnQuerySetPlayerName(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("DATABASESERVER.EXE\t// PACKETTYPE_QUERYSETPLAYERNAME\n");
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_SET_PLAYER_NAME;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnPing(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	time_t tSend;
	ar >> tSend;

	{
		BEFORESENDDUAL(ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN);
		ar << tSend << time(NULL);
		SEND(ar, this, dpid);
	}
}

void CDPTrans::OnQueryMailBox(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
#ifdef __JEFF_FIX_0
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_QUERY_MAIL_BOX;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
#else	// __JEFF_FIX_0
	u_long idReceiver;
	ar >> idReceiver;

	CPost* pPost = CPost::GetInstance();

	CMclAutoLock	Lock(pPost->m_csPost);

	CMailBox* pMailBox = NULL;
	pMailBox = pPost->GetMailBox(idReceiver);

	if (pMailBox != NULL)
	{
		SendMailBox(pMailBox, dpid);
	}
	else
	{
		SendMailBoxReq(idReceiver, dpid, FALSE, pMailBox);
	}

#endif	// __JEFF_FIX_0
}


void CDPTrans::OnQueryMailBoxCount(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long idReceiver;
	int nCount;
	ar >> idReceiver >> nCount;

	CPost* pPost = CPost::GetInstance();

	CMclAutoLock	Lock(pPost->m_csPost);

	CMailBox* pMailBox = NULL;
	pMailBox = pPost->GetMailBox(idReceiver);

	if (pMailBox != NULL)
	{
		SendMailBox(pMailBox, dpid);
	}
	else
	{
		SendMailBoxReq(idReceiver, dpid, FALSE, pMailBox);
	}

	return;
}

void CDPTrans::OnQueryMailBoxReq(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long idReceiver;
	ar >> idReceiver;

	CPost* pPost = CPost::GetInstance();

	CMclAutoLock	Lock(pPost->m_csPost);

	CMailBox* pMailBox = NULL;
	pMailBox = pPost->GetMailBox(idReceiver);

	BOOL	bHaveMailBox = FALSE;
	if (pMailBox != NULL)
	{
		bHaveMailBox = TRUE;
	}

	SendMailBoxReq(idReceiver, dpid, bHaveMailBox, pMailBox);

	return;
}

void CDPTrans::SendMailBox(CMailBox* pMailBox, DPID dpid)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pMailBox->m_idReceiver;
	pMailBox->Write(ar);
	SEND(ar, this, dpid);
}

void CDPTrans::SendMailBoxReq(u_long idReceiver, DPID dpid, BOOL bHaveMailBox, CMailBox* pMailBox)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver;
	ar << bHaveMailBox;
	if (pMailBox != NULL)
	{
		pMailBox->Write(ar);
	}
	SEND(ar, this, dpid);
}

void CDPTrans::OnQueryPostMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = ADD_MAIL;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnQueryRemoveMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = REMOVE_MAIL;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnQueryGetMailItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = REMOVE_MAIL_ITEM;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnQueryGetMailGold(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = REMOVE_MAIL_GOLD;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnReadMail(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = READ_MAIL;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::SendPostMail(BOOL bResult, u_long idReceiver, CMail* pMail)
{
	// 	//	BEGINTEST
	// 	Error( "CDPTrans::SendPostMail Result[%d], Receiver[%d]", bResult, idReceiver );

	BEFORESENDDUAL(ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << bResult << idReceiver;
	pMail->Serialize(ar);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendRemoveMail(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendGetMailItem(u_long idReceiver, u_long nMail, u_long uQuery)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail << uQuery;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendGetMailGold(u_long idReceiver, u_long nMail, u_long uQuery)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail << uQuery;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendReadMail(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendAllMail(DPID dpid)
{
	//	BEFORESENDDUAL( ar, PACKETTYPE_ALLMAIL, DPID_UNKNOWN, DPID_UNKNOWN );

	CAr ar;
	ar.Reserve(5100016);

	int nBufSize;
	ar << DPID_UNKNOWN << DPID_UNKNOWN << PACKETTYPE_ALLMAIL;

	CPost::GetInstance()->m_csPost.Enter();
	CPost::GetInstance()->Serialize(ar, FALSE);
	CPost::GetInstance()->m_csPost.Leave();

	SEND(ar, this, dpid);
}

void CDPTrans::OnGuidCombatInGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = IN_GUILDCOMBAT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnGuidCombatOutGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = OUT_GUILDCOMBAT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnStartGuildCombat(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = START_GUILDCOMBAT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnResultGuildCombat(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = RESULT_GUILDCOMBAT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnGCContinue(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = CONTINUE_GUILDCOMBAT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::SendInGuildCombat(u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya)
{
	BEFORESENDDUAL(ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild;
	ar << dwPenya;
	ar << dwExistingPenya;
	SEND(ar, this, DPID_ALLPLAYERS);
}
void CDPTrans::SendOutGuildCombat(u_long idGuild, u_long dwChannel)
{
	BEFORESENDDUAL(ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild;
	ar << dwChannel;
	SEND(ar, this, DPID_ALLPLAYERS);
}
void CDPTrans::SendResultGuildCombat(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << g_DbManager.m_nGuildCombatIndex;
	ar << g_DbManager.m_uWinGuildId;
	ar << g_DbManager.m_nWinGuildCount;
	ar << g_DbManager.m_uBestPlayer;
	SEND(ar, this, DPID_ALLPLAYERS);
}
void CDPTrans::SendPlayerPointGuildCombat(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	g_DbManager.SerializePlayerPoint(ar);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendContinueGC()
{
	BEFORESENDDUAL(ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendQueryRemoveGuildBankTbl(int nNo, u_long idGuild, DWORD dwItemId, SERIALNUMBER iSerialNumber, DWORD dwItemNum)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nNo << idGuild << dwItemId << iSerialNumber << dwItemNum;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::OnQueryRemoveGuildBankTbl(CAr& ar, DPID, DPID, DPID, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_DELETE_REMOVE_GUILD_BANK_TBL;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

#if __VER >= 9 // __EVENTLUA
void CDPTrans::SendEventLuaState(map<BYTE, BOOL> mapState, BOOL bTextOut, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_EVENTLUA_STATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << bTextOut;
	ar << mapState.size();
	for (map<BYTE, BOOL>::iterator it = mapState.begin(); it != mapState.end(); it++)
		ar << it->first << it->second;
	SEND(ar, this, dpId);
}

void CDPTrans::SendEventLuaChanged(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::OnEventLuaChanged(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	prj.m_EventLua.m_Access.Enter();
	prj.m_EventLua.LoadScript();
	SendEventLuaChanged();
	if (prj.m_EventLua.CheckEventState())
		SendEventLuaState(prj.m_EventLua.m_mapState, FALSE);
	prj.m_EventLua.m_Access.Leave();
}
#endif // __EVENTLUA

void CDPTrans::OnCalluspLoggingQuest(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_CALL_USPLOGGINGQUEST;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnCalluspXXXMultiServer(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
#ifdef __INVALID_LOGIN_0320
	char	szAccount[MAX_ACCOUNT];
	u_long uKey, idPlayer;
	ar >> uKey >> idPlayer;
	ar.ReadString(szAccount, MAX_ACCOUNT);
	if (uKey > 0)
	{
		if (idPlayer > 0)
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer(szAccount, uKey);
		else
			g_DbManager.m_AccountCacheMgr.InitMultiServer(uKey);
	}
	else
	{
#ifndef __INVALID_LOGIN_0612

		if (idPlayer > 0)
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer(szAccount, 0);
#endif	// __INVALID_LOGIN_0612
	}
#endif	// __INVALID_LOGIN_0320
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_CALL_XXX_MULTI_SERVER;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);

	if (idPlayer > 0)
	{
		CPlayerDataCenter::GetInstance()->m_Access.Enter();
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
		if (pPlayerData)
		{
			pPlayerData->data.uLogin = (BYTE)(uKey);
			pPlayerData->data.nVer++;
			SendUpdatePlayerData(idPlayer, pPlayerData);
			g_dpCoreSrvr.SendUpdatePlayerData(idPlayer, pPlayerData);
		}
		CPlayerDataCenter::GetInstance()->m_Access.Leave();
	}
	}

#if __VER >= 11 // __SYS_PLAYER_DATA
void CDPTrans::OnUpdatePlayerData(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	u_long idPlayer;
	sPlayerData	data;
	ar >> idPlayer;
	ar.Read(&data, sizeof(sPlayerData));

	CPlayerDataCenter* pPlayerDataCenter = CPlayerDataCenter::GetInstance();
	pPlayerDataCenter->m_Access.Enter();
	PlayerData* pPlayerData = pPlayerDataCenter->GetPlayerData(idPlayer);
	if (!pPlayerData)
	{
		pPlayerDataCenter->m_Access.Leave();
		return;
	}

	pPlayerData->data.nJob = data.nJob;
	pPlayerData->data.nLevel = data.nLevel;
	pPlayerData->data.nSex = data.nSex;
	pPlayerData->data.nVer++;
	PlayerData pd;
	memcpy(&pd, pPlayerData, sizeof(PlayerData));
	pPlayerDataCenter->m_Access.Leave();
	SendUpdatePlayerData(idPlayer, &pd);
	g_dpCoreSrvr.SendUpdatePlayerData(idPlayer, &pd);
}
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11
void CDPTrans::OnGuildBankLogView(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = QM_GUILDBANK_LOG_VIEW;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

#if __VER >= 12 // __LORD
void CDPTrans::SendElectionAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate, BOOL bRet)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << iDeposit << tCreate << bRet;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendElectionSetPledge(u_long idPlayer, const char* szPledge, BOOL bRet)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(szPledge);
	ar << bRet;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendElectionIncVote(u_long idPlayer, u_long idElector, BOOL bRet)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idElector << bRet;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendElectionBeginCandidacy(void)
{
	SendHdr(PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_ALLPLAYERS);
}

void CDPTrans::SendElectionBeginVote(int nRequirement)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nRequirement;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendElectionEndVote(u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::OnElectionAddDeposit(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionAddDeposit()");
	CTLord::Instance()->PostRequest(CTLord::eAddDeposit, lpBuf, uBufSize);
}

void CDPTrans::OnElectionSetPledge(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionSetPledge()");
	CTLord::Instance()->PostRequest(CTLord::eSetPledge, lpBuf, uBufSize);
}

void CDPTrans::OnElectionIncVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionIncVote()");
	CTLord::Instance()->PostRequest(CTLord::eIncVote, lpBuf, uBufSize);
}

void CDPTrans::OnLEventCreate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnLEventCreate()");
	CTLord::Instance()->PostRequest(CTLord::eLEventCreate, lpBuf, uBufSize);
}

void CDPTrans::OnLordSkillUse(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnLordSkillUse()");
	CTLord::Instance()->PostRequest(CTLord::eLordSkillUse, lpBuf, uBufSize);
}

void CDPTrans::OnLEventInitialize(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnLEventInitialize()");
	CTLord::Instance()->PostRequest(CTLord::eLEventInitialize, lpBuf, uBufSize);
}

// operator commands
void CDPTrans::OnElectionProcess(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionProcess()");
	BOOL bRun;
	ar >> bRun;
	CTElection* pElection = static_cast<CTElection*>(CTLord::Instance()->GetElection());
	pElection->SetRun(bRun);
}
void CDPTrans::OnElectionBeginCandidacy(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionBeginCandidacy()");
	CTLord::Instance()->PostRequest(CTLord::eBeginCandidacy);
}
void CDPTrans::OnElectionBeginVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionBeginVote()");
	CTLord::Instance()->PostRequest(CTLord::eBeginVote);
}
void CDPTrans::OnElectionEndVote(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	election::OutputDebugString("CDPTrans::OnElectionEndVote()");
	CTLord::Instance()->PostRequest(CTLord::eEndVote);
}

void CDPTrans::SendLord(DPID dpid)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LORD, DPID_UNKNOWN, DPID_UNKNOWN);
	CTLord::Instance()->Serialize(ar);
	SEND(ar, this, dpid);
}

void CDPTrans::SendLEventCreate(CLEComponent* pComponent, BOOL bResult)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN);
	pComponent->Serialize(ar);
	ar << bResult;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendLEventInitialize(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendLordSkillUse(u_long idPlayer, u_long idTarget, int nSkill, int nRet)
{

	BEFORESENDDUAL(ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idTarget << nSkill << nRet;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendLordSkillTick(CLordSkill* pSkills)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LORD_SKILL_TICK, DPID_UNKNOWN, DPID_UNKNOWN);
	pSkills->SerializeTick(ar);
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendLEventTick(ILordEvent* pEvent)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_TICK, DPID_UNKNOWN, DPID_UNKNOWN);
	pEvent->SerializeTick(ar);
	SEND(ar, this, DPID_ALLPLAYERS);
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPTrans::SendTaxInfo(DPID dpId, BOOL bConnect, BOOL bToAllClient)
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_ALLINFO, DPID_UNKNOWN, DPID_UNKNOWN);
	CTax::GetInstance()->Serialize(ar);
	ar << bConnect;
	if (bConnect)
	{
		ar << CTax::GetInstance()->m_nMinTaxRate;
		ar << CTax::GetInstance()->m_nMaxTaxRate;
#ifdef __OCCUPATION_SHOPITEM
		ar << CTax::GetInstance()->m_vecdwOccupationShopItem.size();
		for (size_t i = 0; i < CTax::GetInstance()->m_vecdwOccupationShopItem.size(); i++)
			ar << CTax::GetInstance()->m_vecdwOccupationShopItem[i];
#endif // __OCCUPATION_SHOPITEM
	}
	ar << bToAllClient;
	SEND(ar, this, dpId);
}

void CDPTrans::OnSetSecretRoomWinGuild(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CTax::GetInstance()->m_taxDBController.PostRequest(QUERY_TAX_SETNEXT_SECRETROOMWINGUILD, lpBuf, uBufSize, 0);
}

void CDPTrans::OnSetLord(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CTax::GetInstance()->m_taxDBController.PostRequest(QUERY_TAX_SETNEXT_LORD, lpBuf, uBufSize, 0);
}

void CDPTrans::OnTaxRate(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CTax::GetInstance()->m_taxDBController.PostRequest(QUERY_TAX_SETNEXT_TAXRATE, lpBuf, uBufSize, 0);
}

void CDPTrans::OnAddTax(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CTax::GetInstance()->m_taxDBController.PostRequest(QUERY_TAX_ADDTAX, lpBuf, uBufSize, 0);
}

void CDPTrans::OnApplyTaxRateNow(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CTax::GetInstance()->SetApplyTaxRateNow();
}
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
void CDPTrans::OnSecretRoomInsertToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CSecretRoomDBMng::GetInstance()->PostRequest(QUERY_SECRETROOM_TENDER_INSERT, lpBuf, uBufSize);
}

void CDPTrans::OnSecretRoomUpdateToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CSecretRoomDBMng::GetInstance()->PostRequest(QUERY_SECRETROOM_TENDER_UPDATE, lpBuf, uBufSize);
}

void CDPTrans::OnSecretRoomLineUpToDB(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CSecretRoomDBMng::GetInstance()->PostRequest(QUERY_SECRETROOM_LINEUP_INSERT, lpBuf, uBufSize);
}

void CDPTrans::OnSecretRoomClosed(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CSecretRoomDBMng::GetInstance()->PostRequest(QUERY_SECRETROOM_CLOSED, lpBuf, uBufSize);
}

void CDPTrans::SendSecretRoomInfoClear(DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_INFO_CLEAR, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, dpId);
}

void CDPTrans::SendSecretRoomTenderInfo(BYTE nContinent, DWORD dwGuildId, int nPenya, vector<DWORD>& vecMemberId, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nContinent << dwGuildId << nPenya;
	ar << vecMemberId.size();
	for (DWORD i = 0; i < vecMemberId.size(); i++)
		ar << vecMemberId[i];
	SEND(ar, this, dpId);
}
#endif // __SECRET_ROOM

#if __VER >= 13 // __HOUSING
void CDPTrans::SendHousingLoadInfo(DWORD dwPlayerId, CHousing* pHousing, DPID dpId)
{
	ASSERT(pHousing);
	BEFORESENDDUAL(ar, PACKETTYPE_HOUSING_LOADINFO, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwPlayerId;
	pHousing->Serialize(ar);
	SEND(ar, this, dpId);
}

void CDPTrans::SendHousingFurnitureList(DWORD dwPlayerId, HOUSINGINFO& housingInfo, BOOL bAdd, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_HOUSING_FURNITURELIST, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwPlayerId;
	housingInfo.Serialize(ar);
	ar << bAdd;
	SEND(ar, this, dpId);
}

void CDPTrans::SendHousingSetupFurniture(DWORD dwPlayerId, HOUSINGINFO housingInfo, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_HOUSING_SETUPFURNITURE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwPlayerId;
	housingInfo.Serialize(ar);
	SEND(ar, this, dpId);
}

void CDPTrans::SendHousingSetVisitAllow(DWORD dwPlayerId, DWORD dwTargetId, BOOL bAllow, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_HOUSING_SETVISITALLOW, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwPlayerId << dwTargetId << bAllow;
	SEND(ar, this, dpId);
}

void CDPTrans::SendHousingDBFailed(DWORD dwPlayerId, DWORD dwItemId, DPID dpId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_HOUSING_DBFAILED, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwPlayerId << dwItemId;
	SEND(ar, this, dpId);
}

void CDPTrans::OnHousingLoadInfo(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHousingDBMng::GetInstance()->PostRequest(QUERY_HOUSING_LOAD, lpBuf, uBufSize, dpid);
}
void CDPTrans::OnHousingReqSetFurnitureList(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHousingDBMng::GetInstance()->PostRequest(QUERY_HOUSING_INSERT_LIST, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnHousingReqSetupFurniture(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHousingDBMng::GetInstance()->PostRequest(QUERY_HOUSING_SETUP_FURNITURE, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnHousingReqSetVisitAllow(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHousingDBMng::GetInstance()->PostRequest(QUERY_HOUSING_SET_VISITALLOW, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnHousingReqGMRemoveAll(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	CHousingDBMng::GetInstance()->PostRequest(QUERY_HOUSING_GM_REMOVEALL, lpBuf, uBufSize);
}
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
void CDPTrans::OnPropose(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("\nCDPTrans.OnPropose\n");
	CCoupleHelper::Instance()->PostRequest(CCoupleHelper::ePropose, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnCouple(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("\nCDPTrans.OnCouple\n");
	CCoupleHelper::Instance()->PostRequest(CCoupleHelper::eCouple, lpBuf, uBufSize);
}

void CDPTrans::OnDecouple(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("\nCDPTrans.OnDecouple\n");
	CCoupleHelper::Instance()->PostRequest(CCoupleHelper::eDecouple, lpBuf, uBufSize);
}

void CDPTrans::OnClearPropose(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("\nCDPTrans.OnClearPropose\n");
	CCoupleHelper::Instance()->PostRequest(CCoupleHelper::eClearPropose, lpBuf, uBufSize);
}

#if __VER >= 13 // __COUPLE_1202
void CDPTrans::OnQueryAddCoupleExperience(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OutputDebugString("\nCDPTrans.OnQueryAddCoupleExperience\n");
	CCoupleHelper::Instance()->PostRequest(CCoupleHelper::eAddExperience, lpBuf, uBufSize);
}

void CDPTrans::SendAddCoupleExperience(u_long idPlayer, int nExperience)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << nExperience;
	SEND(ar, this, DPID_ALLPLAYERS);
}
#endif	// __COUPLE_1202

void CDPTrans::SendCouple(CCoupleMgr* pMgr, DPID dpid)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ALL_COUPLES, DPID_UNKNOWN, DPID_UNKNOWN);
	CCoupleHelper::Instance()->Serialize(ar);
	SEND(ar, this, dpid);
}

void CDPTrans::SendProposeResult(u_long idProposer, u_long idTarget, int nResult, time_t t, DPID dpid)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idProposer << idTarget << nResult << t;
	SEND(ar, this, dpid);
}

void CDPTrans::SendCoupleResult(u_long idProposer, u_long idTarget, int nResult)
{
	BEFORESENDDUAL(ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idProposer << idTarget << nResult;
	SEND(ar, this, DPID_ALLPLAYERS);
}

void CDPTrans::SendDecoupleResult(u_long idPlayer, int nResult)
{
	BEFORESENDDUAL(ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << nResult;
	SEND(ar, this, DPID_ALLPLAYERS);
}

#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
void CDPTrans::OnBuyGuildHouse(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	GuildHouseDBMng->PostRequest(GUILDHOUSE_CREATE, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnGuildHousePacket(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	GuildHouseDBMng->PostRequest(GUILDHOUSE_DEFAULT_PACKET, lpBuf, uBufSize, dpid);
}

void CDPTrans::OnLogGuildFurniture(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = LOG_GUILDFURNITURE;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

#endif // __GUILD_HOUSE


#ifdef __FL_GW_STATUE_V2
void CDPTrans::OnInstallStatues(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);

	lpDbOverlappedPlus->nQueryMode = GW_STATUES_INSTALL;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

#ifdef __FL_EQUALIZED_MODE
void CDPTrans::OnSaveTemporalPreset(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = TMP_EQUIP_SAVE;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

void CDPTrans::OnGetTemporaryPresets(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);

	lpDbOverlappedPlus->nQueryMode = TMP_EQUIP_GET;
	lpDbOverlappedPlus->dpid = dpid;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

#ifdef __FL_DAILY_REWARDS
void CDPTrans::OnUpdateDailyGift(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = UPDATE_DAILYGIFT;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

#ifdef __FL_VENDORS
void CDPTrans::OnAddVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = ADD_VENDOR;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnUpdateVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = UPDATE_VENDOR;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnDeleteVendor(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->dpid = dpid;
	lpDbOverlappedPlus->dpidCache = dpidCache;
	lpDbOverlappedPlus->dpidUser = dpidUser;
	lpDbOverlappedPlus->nQueryMode = DELETE_VENDOR;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::OnGetVendors(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);

	lpDbOverlappedPlus->nQueryMode = GET_VENDORS;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
#endif

#ifdef __FL_CASH_SHOP
void CDPTrans::SendCShop(DPID dpid)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CSHOP_LOAD, DPID_UNKNOWN, DPID_UNKNOWN);
	CFlyFFShop::GetInstance()->SerializeFlyffShop(ar);
	SEND(ar, this, dpid);
}
void CDPTrans::OnGetCShopBalance(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);

	lpDbOverlappedPlus->nQueryMode = CSHOP_BALANCE;
	lpDbOverlappedPlus->dpid = dpid;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::SendCShopBalance(DPID dpid, u_long idPlayer, int nDonate, int nVote, BOOL bPurchaseError)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CSHOP_BALANCE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << nDonate << nVote;
	ar << bPurchaseError;
	SEND(ar, this, dpid);
}
void CDPTrans::OnBuyCShopItem(CAr& ar, DPID dpid, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);

	lpDbOverlappedPlus->nQueryMode = CSHOP_ITEM;
	lpDbOverlappedPlus->dpid = dpid;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGuild, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}
void CDPTrans::SendCShopItem(DPID dpid, u_long idBuyer, u_long idReceiver, int nNewDonate, int nNewVote, DWORD dwItemID, int nItemNum)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CSHOP_ITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idBuyer << idReceiver << nNewDonate << nNewVote << dwItemID << nItemNum;
	SEND(ar, this, dpid);
}
#endif