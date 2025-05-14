#include "StdAfx.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "User.h"
#include "WorldMng.h"
#include "misc.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#include "eveschool.h"
#include "definetext.h"
#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

extern	CDPSrvr		g_DPSrvr;
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPCoreClient	g_DPCoreClient;

#include "party.h"
extern	CPartyMng	g_PartyMng;
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "post.h"
extern	BOOL CanAdd(DWORD dwGold, int nPlus);
#include "spevent.h"

#if __VER >= 12 // __LORD
#include "slord.h"
#endif	// __LORD

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

extern char		g_szDBAddr[16];

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonBase.h"
#endif // __INSTANCE_DUNGEON

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

#ifdef __FL_MONSTER_ALBUM
#include "MonsterAlbum.h"
#endif

CDPDatabaseClient	g_dpDBClient;

CDPDatabaseClient::CDPDatabaseClient()
{
	BEGIN_MSG;
	ON_MSG(PACKETTYPE_JOIN, &CDPDatabaseClient::OnJoin);
	ON_MSG(PACKETTYPE_ALL_PLAYER_DATA, &CDPDatabaseClient::OnAllPlayerData);
	ON_MSG(PACKETTYPE_ADD_PLAYER_DATA, &CDPDatabaseClient::OnAddPlayerData);
	ON_MSG(PACKETTYPE_DELETE_PLAYER_DATA, &CDPDatabaseClient::OnDeletePlayerData);
	ON_MSG(PACKETTYPE_UPDATE_PLAYER_DATA, &CDPDatabaseClient::OnUpdatePlayerData);
	ON_MSG(PACKETTYPE_GUILD_BANK, &CDPDatabaseClient::OnGuildBank);
	ON_MSG(PACKETTYPE_QUERYGUILDQUEST, &CDPDatabaseClient::OnQueryGuildQuest);
	ON_MSG(PACKETTYPE_BASEGAMESETTING, &CDPDatabaseClient::OnBaseGameSetting);
	ON_MSG(PACKETTYPE_MONSTERRESPAWNSETTING, &CDPDatabaseClient::OnMonsterRespawnSetting);
	ON_MSG(PACKETTYPE_MONSTERPROPGAMESETTING, &CDPDatabaseClient::OnMonsterPropSetting);
	ON_MSG(PACKETTYPE_GAMEMASTER_CHATTING, &CDPDatabaseClient::OnGMChat);
	ON_MSG(PACKETTYPE_PING, &CDPDatabaseClient::OnPing);

	ON_MSG(PACKETTYPE_ADD_GUILDCOMBAT, &CDPDatabaseClient::OnAllGuildCombat);
	ON_MSG(PACKETTYPE_IN_GUILDCOMBAT, &CDPDatabaseClient::OnInGuildCombat);
	ON_MSG(PACKETTYPE_OUT_GUILDCOMBAT, &CDPDatabaseClient::OnOutGuildCombat);
	ON_MSG(PACKETTYPE_RESULT_GUILDCOMBAT, &CDPDatabaseClient::OnResultGuildCombat);
	ON_MSG(PACKETTYPE_PLAYERPOINT_GUILDCOMBAT, &CDPDatabaseClient::OnPlayerPointGuildCombat);
	ON_MSG(PACKETTYPE_CONTINUE_GUILDCOMBAT, &CDPDatabaseClient::OnContinueGC);
	ON_MSG(PACKETTYPE_QUERYPOSTMAIL, &CDPDatabaseClient::OnPostMail);
	ON_MSG(PACKETTYPE_QUERYREMOVEMAIL, &CDPDatabaseClient::OnRemoveMail);
	ON_MSG(PACKETTYPE_QUERYGETMAILITEM, &CDPDatabaseClient::OnGetMailItem);
	ON_MSG(PACKETTYPE_QUERYGETMAILGOLD, &CDPDatabaseClient::OnGetMailGold);
	ON_MSG(PACKETTYPE_READMAIL, &CDPDatabaseClient::OnReadMail);
	ON_MSG(PACKETTYPE_ALLMAIL, &CDPDatabaseClient::OnAllMail);
	ON_MSG(PACKETTYPE_QUERYMAILBOX, &CDPDatabaseClient::OnMailBox);


	////////////////////////////////////////////////////////////////////
	ON_MSG(PACKETTYPE_QUERYMAILBOX_REQ, &CDPDatabaseClient::OnMailBoxReq);
	////////////////////////////////////////////////////////////////////

	ON_MSG(PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, &CDPDatabaseClient::OnQueryRemoveGuildBankTbl);

#if __VER >= 9 // __EVENTLUA
	ON_MSG(PACKETTYPE_EVENTLUA_STATE, &CDPDatabaseClient::OnEventStateLua);
	ON_MSG(PACKETTYPE_EVENTLUA_CHANGED, &CDPDatabaseClient::OnEventLuaChanged);
#endif // __EVENTLUA

#if __VER >= 11
	ON_MSG(PACKETTYPE_GUILDLOG_VIEW, &CDPDatabaseClient::OnGuildBankLogViewFromDB);
#endif

#if __VER >= 12 // __LORD
	ON_MSG(PACKETTYPE_ELECTION_ADD_DEPOSIT, &CDPDatabaseClient::OnElectionAddDeposit);
	ON_MSG(PACKETTYPE_ELECTION_SET_PLEDGE, &CDPDatabaseClient::OnElectionSetPledge);
	ON_MSG(PACKETTYPE_ELECTION_INC_VOTE, &CDPDatabaseClient::OnElectionIncVote);
	ON_MSG(PACKETTYPE_ELECTION_BEGIN_CANDIDACY, &CDPDatabaseClient::OnElectionBeginCandidacy);
	ON_MSG(PACKETTYPE_ELECTION_BEGIN_VOTE, &CDPDatabaseClient::OnElectionBeginVote);
	ON_MSG(PACKETTYPE_ELECTION_END_VOTE, &CDPDatabaseClient::OnElectionEndVote);
	ON_MSG(PACKETTYPE_LORD, &CDPDatabaseClient::OnLord);
	ON_MSG(PACKETTYPE_L_EVENT_CREATE, &CDPDatabaseClient::OnLEventCreate);
	ON_MSG(PACKETTYPE_L_EVENT_INITIALIZE, &CDPDatabaseClient::OnLEventInitialize);
	ON_MSG(PACKETTYPE_LORD_SKILL_USE, &CDPDatabaseClient::OnLordSkillUse);
	ON_MSG(PACKETTYPE_LORD_SKILL_TICK, &CDPDatabaseClient::OnLordSkillTick);
	ON_MSG(PACKETTYPE_L_EVENT_TICK, &CDPDatabaseClient::OnLEventTick);
#endif	// __LORD

#if __VER >= 12 // __TAX
	ON_MSG(PACKETTYPE_TAX_ALLINFO, &CDPDatabaseClient::OnTaxInfo);
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
	ON_MSG(PACKETTYPE_SECRETROOM_INFO_CLEAR, &CDPDatabaseClient::OnSecretRoomInfoClear);
	ON_MSG(PACKETTYPE_SECRETROOM_TENDERINFO_TO_WSERVER, &CDPDatabaseClient::OnSecretRoomTenderInfo);
#endif // __SECRET_ROOM


#if __VER >= 13 // __HOUSING
	ON_MSG(PACKETTYPE_HOUSING_LOADINFO, &CDPDatabaseClient::OnHousingLoadInfo);
	ON_MSG(PACKETTYPE_HOUSING_FURNITURELIST, &CDPDatabaseClient::OnHousingSetFunitureList);
	ON_MSG(PACKETTYPE_HOUSING_SETUPFURNITURE, &CDPDatabaseClient::OnHousingSetupFuniture);
	ON_MSG(PACKETTYPE_HOUSING_SETVISITALLOW, &CDPDatabaseClient::OnHousingSetVisitAllow);
	ON_MSG(PACKETTYPE_HOUSING_DBFAILED, &CDPDatabaseClient::OnHousingDBFailed);
#endif // __HOUSING
	m_bAlive = TRUE;
	m_cbPing = 0;
#if __VER >= 13 // __COUPLE_1117
	ON_MSG(PACKETTYPE_PROPOSE, &CDPDatabaseClient::OnProposeResult);
	ON_MSG(PACKETTYPE_COUPLE, &CDPDatabaseClient::OnCoupleResult);
	ON_MSG(PACKETTYPE_DECOUPLE, &CDPDatabaseClient::OnDecoupleResult);
	ON_MSG(PACKETTYPE_ALL_COUPLES, &CDPDatabaseClient::OnCouple);
#if __VER >= 13 // __COUPLE_1202
	ON_MSG(PACKETTYPE_ADD_COUPLE_EXPERIENCE, &CDPDatabaseClient::OnAddCoupleExperience);
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117

#if __VER >= 15 // __GUILD_HOUSE
	ON_MSG(PACKETTYPE_GUILDHOUSE_LOAD, &CDPDatabaseClient::OnLoadGuildHouse);
	ON_MSG(PACKETTYPE_GUILDHOUSE_BUY, &CDPDatabaseClient::OnBuyGuildHouse);
	ON_MSG(PACKETTYPE_GUILDHOUSE_REMOVE, &CDPDatabaseClient::OnRemoveGuildHouse);
	ON_MSG(PACKETTYPE_GUILDHOUSE_PACKET, &CDPDatabaseClient::OnGuildHousePacket);
#endif // __GUILD_HOUSE

#ifdef __FL_GW_STATUE_V2
	ON_MSG(PACKETTYPE_REQUEST_INSTALL_STATUES, &CDPDatabaseClient::OnInstallStatues);
#endif
#ifdef __FL_EQUALIZED_MODE
	ON_MSG(PACKETTYPE_GETEQUIPPRESETS, &CDPDatabaseClient::OnGetTemporalPresets);
#endif
#ifdef __FL_VENDORS
	ON_MSG(PACKETTYPE_VENDORS_GET, &CDPDatabaseClient::OnGetVendors);
#endif
#ifdef __FL_CASH_SHOP
	ON_MSG(PACKETTYPE_CSHOP_LOAD, &CDPDatabaseClient::OnCShop);
	ON_MSG(PACKETTYPE_CSHOP_BALANCE, &CDPDatabaseClient::OnCShopBalance);
	ON_MSG(PACKETTYPE_CSHOP_ITEM, &CDPDatabaseClient::OnCShopItem);
#endif
#ifdef __FL_RECORD_BOOK
	ON_MSG(PACKETTYPE_RECORD_BOOK, &CDPDatabaseClient::OnRecordBook);
#endif
}

CDPDatabaseClient::~CDPDatabaseClient()
{
}

void CDPDatabaseClient::SysMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	switch (lpMsg->dwType)
	{
	case DPSYS_DESTROYPLAYERORGROUP:
		break;
	}
}

void CDPDatabaseClient::UserMessageHandler(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom)
{
	CAr ar((LPBYTE)lpMsg + sizeof(DPID) + sizeof(DPID), dwMsgSize - (sizeof(DPID) + sizeof(DPID)));
	GETTYPE(ar);

	void (theClass:: * pfn)(theParameters) = GetHandler(dw);

	if (pfn)
		(this->*(pfn))(ar, *(UNALIGNED LPDPID)lpMsg, *(UNALIGNED LPDPID)((LPBYTE)lpMsg + sizeof(DPID)));
	else
		Error("Handler not found(%08x)\n", dw);
}

void CDPDatabaseClient::SavePlayer(CUser* pUser, DWORD dwWorldId, D3DXVECTOR3& vPos, int nLayer, BOOL bLogout)
{
#ifdef __FL_EQUALIZED_MODE
	if (pUser->bTmpMode)
		return;
#endif

	BEFORESENDDUAL(ar, PACKETTYPE_SAVE_PLAYER, DPID_UNKNOWN, DPID_UNKNOWN);

	ar << bLogout;

	ar.WriteString(pUser->m_playAccount.lpszAccount);
	ar << ((CMover*)pUser)->m_nSlot;

	pUser->m_dwTimeout4Save = SEC_SAVEPLAYER;

	ar << dwWorldId;
	ar << vPos;
	ar << nLayer;

	CObj::SetMethod(METHOD_NONE);
	int nTrade = pUser->m_vtInfo.TradeGetGold();
	pUser->AddGold(nTrade, FALSE);
	pUser->Serialize(ar);
	pUser->AddGold(-nTrade, FALSE);


	pUser->m_playTaskBar.Serialize(ar);
	ar << pUser->m_RTMessenger.GetState();

	ar << pUser->m_dwReturnWorldID;
	ar << pUser->m_vReturnPos;

#ifdef __FL_BATTLE_PASS
	int nCurrentBattlePassID = CBattlePass::GetInstance()->GetBattlePassID();
	ar << nCurrentBattlePassID;
#endif

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::TOTAL_PLAYTIME, pUser->GetPlayerTime(), false);

	for (int i = 0; i < MAX_RECORDTYPE; i++)
	{
		pUser->m_arrayRecords[i].SerializeUpdated(ar);
		pUser->m_arrayRecords[i].bUpdated = false;
	}
#endif

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendLogConnect(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LOG_PLAY_CONNECT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pUser->GetWorld()->GetID();	// world id
	ar.Write(&pUser->m_playAccount.m_stLogin, sizeof(SYSTEMTIME));
	ar.WriteString(pUser->m_playAccount.lpAddr);	// ip
	DWORD dwSeed = pUser->GetGold() + pUser->m_dwGoldBank[pUser->m_nSlot];
	ar << dwSeed;	// seed
	ar << pUser->m_idPlayer;
	ar.WriteString(pUser->m_playAccount.lpszAccount);
	ar << pUser->GetLevel();
	ar << pUser->GetJob();
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendInGuildCombat(u_long idGuild, DWORD dwPenya, DWORD dwExistingPenya)
{
	BEFORESENDDUAL(ar, PACKETTYPE_IN_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild << dwPenya << dwExistingPenya;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendOutGuildCombat(u_long idGuild)
{
	BEFORESENDDUAL(ar, PACKETTYPE_OUT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild;
	ar << g_uIdofMulti;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendGuildCombatStart(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_START_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendGuildCombatResult(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_RESULT_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);

	float fRequstCanclePercent = g_GuildCombatMng.m_nRequestCanclePercent / 100.0f;
	float fNotRequestPercent = g_GuildCombatMng.m_nNotRequestPercent / 100.0f;
	ar << g_GuildCombatMng.m_uWinGuildId;
	ar << g_GuildCombatMng.m_nWinGuildCount;
	ar << g_GuildCombatMng.m_uBestPlayer;

	ar << (u_long)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for (int gcmi = 0; gcmi != g_GuildCombatMng.m_vecGuildCombatMem.size(); ++gcmi)
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];

		BOOL bSelectGuild = FALSE;
		for (int veci = 0; veci < (int)(g_GuildCombatMng.vecRequestRanking.size()); ++veci)
		{
			if (veci >= g_GuildCombatMng.m_nMaxGuild)
				break;

			CGuildCombat::__REQUESTGUILD RequestGuild = g_GuildCombatMng.vecRequestRanking[veci];
			if (RequestGuild.uidGuild == pGCMember->uGuildId)
			{
				bSelectGuild = TRUE;
				break;
			}
		}

		ar << pGCMember->uGuildId;
		ar << pGCMember->nGuildPoint;
		if (bSelectGuild)
		{
			if (g_GuildCombatMng.m_uWinGuildId == pGCMember->uGuildId)
			{
				ar << g_GuildCombatMng.GetPrizePenya(0);
				ar << (DWORD)0;
				ar << g_GuildCombatMng.m_nWinGuildCount;
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0;
				ar << (int)0;
			}
		}
		else
		{
			if (pGCMember->bRequest)
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fNotRequestPercent );
				ar << (int)0;


				CGuild* pGuild = g_GuildMng.GetGuild(pGCMember->uGuildId);
				if (pGuild)
				{
					CItemElem pItemElem;
					char szMsg[1000];
					sprintf(szMsg, "%s", prj.GetText(TID_GAME_GC_NOTREQUEST));
					char szMsg1[1000];
					sprintf(szMsg1, "%s", prj.GetText(TID_GAME_GC_NOTREQUEST1));
					SendQueryPostMail(pGuild->m_idMaster, 0, pItemElem, (int)(pGCMember->dwPenya * fNotRequestPercent), szMsg, szMsg1);
				}
			}
			else
			{
				ar << (__int64)0;
				ar << (DWORD)0; //(DWORD)(pGCMember->dwPenya * fRequstCanclePercent );
				ar << (int)0;
			}
		}


		ar << (u_long)pGCMember->vecGCSelectMember.size();
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			ar << pJoinPlayer->uidPlayer;
			ar << pJoinPlayer->nPoint;
			if (pJoinPlayer->uidPlayer == g_GuildCombatMng.m_uBestPlayer)
			{
				ar << g_GuildCombatMng.GetPrizePenya(1);
			}
			else
			{
				ar << (__int64)0;
			}
		}
	}
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendGuildcombatContinue(int nGuildCombatID, u_long uidGuild, int nWinCount)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CONTINUE_GUILDCOMBAT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nGuildCombatID;
	ar << uidGuild;
	ar << nWinCount;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnJoin(CAr& ar, DPID dpidCache, DPID dpidUser)
{
	DWORD dwWorldId, dwIndex;
	DWORD dwSavePlayerTime = 0;
	DWORD dwSaveFlyTime = 0;
	int nMaximumLevel = 0;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser, true);
	if (!pUser)
	{
		Error("CDPDatabaseClient::OnJoin err invalid user");
		return;
	}

	DWORD dwAuthKey;
	ar >> dwAuthKey;
	if (pUser->m_dwAuthKey != dwAuthKey)
	{
		WriteLog("OnJoin(): Different dwAuthKey");
		return;
	}

	char lpAddrtmp[16];

#ifdef __ON_ERROR
	int nOnError = 0;
	try
	{
#endif	// __ON_ERROR

		memcpy(lpAddrtmp, pUser->m_playAccount.lpAddr, 16);
		ar >> pUser->m_playAccount;
		memcpy(pUser->m_playAccount.lpAddr, lpAddrtmp, 16);

		ar >> dwWorldId;
#ifdef __LAYER_1015
		int nLayer;
		ar >> nLayer;
#endif // __LAYER_1015

#ifdef __FL_BATTLE_PASS
		int nOldBattlePassID;
		ar >> nOldBattlePassID;
#endif
#ifdef __FL_RECORD_BOOK
		for (int i = 0; i < MAX_RECORDTYPE; i++)
			pUser->m_arrayRecords[i].SerializeAll(ar);
#endif

		ar >> dwIndex;

#ifdef __ON_ERROR
		nOnError = 1;
#endif	// __ON_ERROR

		pUser->SetIndex(NULL, dwIndex, FALSE);
		CObj::SetMethod(METHOD_NONE);
		pUser->Serialize(ar);

#ifdef __FL_UPDATE_READSTAFF
		pUser->m_dwAuthorization = prj.CheckStaff(pUser->GetName());
#endif

		pUser->InitMotion(pUser->m_dwMotion);

		pUser->m_dwTickCheer = GetTickCount() + TICK_CHEERPOINT;

		pUser->SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);

		if (pUser->GetLevel() > MAX_3RD_LEGEND_LEVEL)
		{
			WriteError("MAX_3RD_LEGEND_LEVEL//%s//%d", pUser->GetName(), pUser->GetLevel());
			pUser->m_nLevel = MAX_3RD_LEGEND_LEVEL;
		}

		if (!pUser->GetPet())
			pUser->SetPetId(NULL_ID);

		pUser->m_pActMover->RemoveStateFlag(OBJSTAF_ACC);

		CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
		if (pWorld && pWorld->VecInWorld(pUser->GetPos()) == FALSE)
		{
			dwWorldId = 1;
			pUser->SetPos(D3DXVECTOR3(6971.984f, 100.0f, 3336.884f));
		}

		pUser->AdjustGuildQuest(dwWorldId);

#ifdef __ON_ERROR
		nOnError = 2;
#endif	// __ON_ERROR

		pUser->m_playTaskBar.Serialize(ar);

#ifdef __ON_ERROR
		nOnError = 3;
#endif	// __ON_ERROR

		pUser->m_RTMessenger.Serialize(ar);

#ifdef __ON_ERROR
		nOnError = 4;
#endif	// __ON_ERROR
		ar.ReadString(pUser->m_szPartyName, 32);
		ar.ReadString(pUser->m_szBankPass, 5);
		ar >> dwSavePlayerTime;
		pUser->SetPlayerTime(dwSavePlayerTime);

		ar >> nMaximumLevel;

		char GuildMemberTime[15] = { 0, };
		ar.ReadString(GuildMemberTime, 15);

		char cYear[5] = { 0, };
		char cMonth[3] = { 0, };
		char cDay[3] = { 0, };
		char cHour[3] = { 0, };
		char cMin[3] = { 0, };

		strncpy(cYear, GuildMemberTime, 4);
		strncpy(cMonth, GuildMemberTime + 4, 2);
		strncpy(cDay, GuildMemberTime + 6, 2);
		strncpy(cHour, GuildMemberTime + 8, 2);
		strncpy(cMin, GuildMemberTime + 10, 2);

		CTime tGuildMember(atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0);
		pUser->m_tGuildMember = tGuildMember;

		ar >> pUser->m_dwReturnWorldID;
		ar >> pUser->m_vReturnPos;

#ifdef __FL_DAILY_REWARDS
		BOOL bIsRewardTime;
		int nRewardDay;

		ar >> bIsRewardTime;
		ar >> nRewardDay;
#endif
#ifdef __FL_CASH_SHOP
		int nDonatePoints, nVotePoints;
		ar >> nDonatePoints;
		ar >> nVotePoints;
#endif

#ifdef __ON_ERROR
		nOnError = 5;
#endif	// __ON_ERROR

		pUser->m_pActMover->ClearState();
		pUser->RedoEquip(FALSE);

		if (pUser->m_nHitPoint == 0)
			pUser->m_nHitPoint = pUser->GetMaxHitPoint();

		pUser->SetHitPoint(pUser->GetHitPoint());
		pUser->SetManaPoint(pUser->GetManaPoint());
		pUser->SetFatiguePoint(pUser->GetFatiguePoint());

		pUser->SetId(NULL_ID);

		pUser->CheckHonorStat();

		if (prj.GetUserByID(pUser->m_idPlayer))
		{
			WriteLog("OnJoin(): an ocular spectrum %d", pUser->m_idPlayer);
			pUser->m_idPlayer = (u_long)0xffffffff;
			return;
		}

#ifdef __ON_ERROR
		nOnError = 6;
#endif	// __ON_ERROR

		pUser->ValidateItem();
		pUser->m_nGuildCombatState = 0;

#ifdef __ON_ERROR
		nOnError = 700;
#endif	// __ON_ERROR

		pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_PARTYSUMMON);

#ifdef __ON_ERROR
		nOnError = 701;
#endif	// __ON_ERROR

#ifdef __ON_ERROR
		nOnError = 702;
#endif	// __ON_ERROR

		pUser->AdjustMailboxState();

#ifdef __ON_ERROR
		nOnError = 703;
#endif	// __ON_ERROR

		// Guild
		if (pUser->m_idGuild != 0)
		{
			CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
			if (!pGuild || !pGuild->IsMember(pUser->m_idPlayer))
				pUser->m_idGuild = 0;
		}

		// World
		BOOL bDefaultPosition = FALSE;
		if (!pWorld || !pWorld->m_linkMap.GetLinkMap(nLayer))
			bDefaultPosition = TRUE;
		else if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(dwWorldId))
			bDefaultPosition = TRUE;
		else if (dwWorldId >= WI_DUNGEON_SECRET_1 && dwWorldId <= WI_DUNGEON_SECRET_L)
			bDefaultPosition = TRUE;
		else if (dwWorldId == WI_DUNGEON_SECRET_0)
			bDefaultPosition = TRUE;
		else if (dwWorldId == WI_WORLD_GUILDWAR)
			bDefaultPosition = TRUE;
		else if (dwWorldId == WI_WORLD_MINIROOM)
			bDefaultPosition = TRUE;
		else if (!GuildHouseMng->IsLoginAble(pUser, dwWorldId, nLayer))
			bDefaultPosition = TRUE;
#ifdef __FL_FFA
		else if (dwWorldId == WI_WORLD_FFA)
			bDefaultPosition = TRUE;
#endif
#ifdef __FL_TDM
		else if (dwWorldId == WI_WORLD_TDM)
			bDefaultPosition = TRUE;
#endif

		if (!bDefaultPosition)
		{
			g_UserMng.AddPlayer(pUser, dwWorldId, nLayer);
		}
		else
		{
			dwWorldId = WI_WORLD_MADRIGAL;
			pUser->SetPos(D3DXVECTOR3(6971.984f, 100.0f, 3336.884f));
			g_UserMng.AddPlayer(pUser, dwWorldId, nDefaultLayer);
		}

		if (!pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			if (pUser->m_dwMode & (MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE))
			{
				Error("Abnormal User!! - %s(%07d), m_dwMode:%08x", pUser->GetName(), pUser->m_idPlayer, pUser->m_dwMode);
				pUser->SetNotMode(MATCHLESS_MODE | TRANSPARENT_MODE | ONEKILL_MODE | MATCHLESS2_MODE);
			}
		}


#ifdef __ON_ERROR
		nOnError = 704;
#endif	// __ON_ERROR

		pUser->AddEventLuaDesc();

#ifdef __ON_ERROR
		nOnError = 705;
#endif	// __ON_ERROR

		pUser->AdjustPartyQuest(dwWorldId);

#ifdef __ON_ERROR
		nOnError = 706;
#endif	// __ON_ERROR

#ifdef __ON_ERROR
		nOnError = 8;
#endif	// __ON_ERROR

		g_DPCoreClient.SendJoin(pUser->m_idPlayer, pUser->GetName(), (pUser->m_dwAuthorization >= AUTH_GAMEMASTER));

#ifdef __ON_ERROR
		nOnError = 9;
#endif	// __ON_ERROR

		// Party
		CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
		if (pParty && pParty->IsMember(pUser->m_idPlayer))
		{
			const char* pszLeader = CPlayerDataCenter::GetInstance()->GetPlayerString(pParty->GetPlayerId(0));
			pUser->AddPartyMember(pParty, 0, (pszLeader ? pszLeader : ""), "");
		}
		else
			pUser->m_idparty = 0;

#ifdef __ON_ERROR
		nOnError = 10;
#endif	// __ON_ERROR

		pUser->AddSMModeAll();
		pUser->AddGameSetting();
		pUser->AddMonsterProp();
		pUser->SetAllAction();

#ifdef __ON_ERROR
		nOnError = 11;
#endif	// __ON_ERROR

		pUser->AddGCIsRequest(g_GuildCombatMng.IsRequestWarGuild(pUser->m_idGuild, TRUE));
		pUser->AddGuildCombatState();
		if (g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam == CGuildCombat::MAINTENANCE_STATE)
			pUser->AddGuildCombatEnterTime(g_GuildCombatMng.m_dwTime - GetTickCount());
		else
			pUser->AddGuildCombatNextTime(g_GuildCombatMng.m_dwTime - GetTickCount(), g_GuildCombatMng.GuildCombatProcess[g_GuildCombatMng.m_nProcessGo - 1].dwParam);
		pUser->AddGCWinGuild();
		pUser->AddGCBestPlayer();
		if (g_GuildCombatMng.m_nState != CGuildCombat::CLOSE_STATE)
		{
			g_UserMng.AddGCGuildStatus(pUser->m_idGuild, pUser);
			g_UserMng.AddGCWarPlayerlist(pUser->m_idGuild, pUser);
		}


#ifdef __ON_ERROR
		nOnError = 12;
#endif	// __ON_ERROR

		CalluspXXXMultiServer(g_uIdofMulti, pUser);

#ifdef __ON_ERROR
		nOnError = 13;
#endif	// __ON_ERROR

		pUser->UpdateRegionAttr();
		if (pUser->IsCollecting() || pUser->IsRegionAttr(RA_COLLECTING))
			pUser->StopCollecting();

#ifdef __ON_ERROR
		nOnError = 14;
#endif	// __ON_ERROR

#ifdef __ON_ERROR
		nOnError = 15;
#endif	// __ON_ERROR

		pUser->AddTaxInfo();
		CTax::GetInstance()->SendNoSetTaxRateOpenWnd(pUser);

#ifdef __ON_ERROR
		nOnError = 16;
#endif	// __ON_ERROR

		CItemElem* pItem = pUser->GetPetItem();
		if (pItem)
			pUser->SetDestParamRandomOptExtension(pItem);


#ifdef __ON_ERROR
		nOnError = 17;
#endif	// __ON_ERROR

		CHousingMng::GetInstance()->CreateRoomLayer(pUser->m_idPlayer);
		CHousingMng::GetInstance()->ReqLoadHousingInfo(pUser->m_idPlayer);
		CSecretRoomMng::GetInstance()->GetAllInfo(pUser);

		pUser->AddGuildHouseAllInfo(GuildHouseMng->GetGuildHouse(pUser->m_idGuild));
		GuildHouseMng->SetApplyDST(pUser);
#ifdef __FL_MONSTER_ALBUM
		pUser->m_MonsterAlbum.SetApplyDST();
#endif

#ifdef __FL_BATTLE_PASS
		const int nCurrentBattlePassID = CBattlePass::GetInstance()->GetBattlePassID();
		const int nBattlePassLevel = pUser->GetBattlePassLevel();
		const int nBattlePassExp = pUser->GetBattlePassExp();

		if (nCurrentBattlePassID != nOldBattlePassID)
		{
			pUser->SetBattlePassLevel(0);
			pUser->SetBattlePassExp(0);
		}

		CBattlePass::GetInstance()->CheckRemoveTicket(pUser);
		pUser->AddBattlePassTime();
		pUser->AddBattlePassState(pUser->GetBattlePassLevel(), pUser->GetBattlePassExp());
#endif

#ifdef __FL_FFA
		CFFAManager::GetInstance()->JoinFFAWorld(pUser);
#endif
#ifdef __FL_TDM
		CTDMManager::GetInstance()->JoinTDMWorld(pUser);
#endif

#ifdef __FL_DAILY_REWARDS
		if (bIsRewardTime && nRewardDay >= 0)
		{
			auto it = prj.mapDailyRewards.find(nRewardDay);
			if (it == prj.mapDailyRewards.end())
				nRewardDay = 0;

			pUser->m_nLastDailyReward = nRewardDay;
			pUser->AddDailyReward();
		}
#endif

#ifdef __FL_VENDORS
		pUser->ActivateClientVendor();
#endif

#ifdef __FL_CASH_SHOP
		pUser->AddCShop();
		pUser->AddCShopBalance(nDonatePoints, nVotePoints);
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
		pUser->AddWof();
#endif

#ifdef __FL_EQUALIZED_MODE
		if (pUser->GetWorld())
		{
			pUser->TemporalOut();
			for (auto& it : prj.m_vecTmpWorlds)
			{
				if (it.dwWorldID == pUser->GetWorld()->GetID())
				{
					if (pUser->TemporalPreCheck(pUser->GetWorld()->GetID()))
						pUser->AddOpenTempWindow(it.nMode);

					break;
				}
			}
		}
#endif


#ifdef __ON_ERROR
	}
	catch (...)
	{
		WriteLog("OnJoin(): %s, %d", pUser->GetName(), nOnError);
	}
#endif	// __ON_ERROR
}
void CDPDatabaseClient::OnGCPlayerPoint(CAr& ar)
{
	g_GuildCombatMng.m_vecGCPlayerPoint.clear();

	u_long uSize;
	ar >> uSize;
	for (int i = 0; i < (int)(uSize); ++i)
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint;
		ar >> GCPlayerPoint.uidPlayer;
		ar >> GCPlayerPoint.nJob;
		ar >> GCPlayerPoint.nPoint;
		g_GuildCombatMng.m_vecGCPlayerPoint.push_back(GCPlayerPoint);
	}
}
void CDPDatabaseClient::OnAllGuildCombat(CAr& ar, DPID, DPID)
{
	g_GuildCombatMng.GuildCombatClear(1);
	u_long uSize, idGuild;
	DWORD dwPenya;
	BOOL bRequest;
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;


	ar >> uSize;
	for (u_long i = 0; i < uSize; ++i)
	{
		ar >> idGuild;
		ar >> dwPenya;
		ar >> bRequest;

		CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
		if (pGuild)
		{
			g_GuildCombatMng.JoinGuildCombat(idGuild, dwPenya, bRequest);
		}
	}

	OnGCPlayerPoint(ar);

#ifdef __FL_GW_STATUE_V2
	// Send request to database to create statues when channel just started
	CGuild* pWinGuild = g_GuildMng.GetGuild(g_GuildCombatMng.m_uWinGuildId);
	if (pWinGuild)
		g_dpDBClient.SendRequestToInstallStatues(g_GuildCombatMng.m_uBestPlayer, pWinGuild->m_idMaster);
#endif

}

void CDPDatabaseClient::OnInGuildCombat(CAr& ar, DPID, DPID)
{
	u_long idGuild;
	DWORD dwPenya, dwExistingPenya;
	ar >> idGuild;
	ar >> dwPenya;
	ar >> dwExistingPenya;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		g_GuildCombatMng.JoinGuildCombat(idGuild, dwPenya, TRUE);
		CUser* pMaster = (CUser*)prj.GetUserByID(pGuild->m_idMaster);
		if (IsValidObj(pMaster))
		{
			CString str;


			if (dwExistingPenya == 0)
			{
				str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), 0, dwPenya);
			}
			else
			{

				str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_APP_CONFIRM), dwExistingPenya, dwPenya - dwExistingPenya);
			}

			pMaster->AddDiagText(str);
			g_UserMng.AddGCIsRequest(idGuild, g_GuildCombatMng.IsRequestWarGuild(idGuild, TRUE));
		}

		CString strMsg;

		strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_REQUEST_GUILDCOMBAT));
		g_UserMng.AddGuildMsg(pGuild, strMsg);
		strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_APP));
		g_UserMng.AddGuildMsg(pGuild, strMsg);
	}
}

void CDPDatabaseClient::OnOutGuildCombat(CAr& ar, DPID, DPID)
{
	u_long idGuild;
	u_long dwChannel;
	ar >> idGuild;
	ar >> dwChannel;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
	{
		BOOL bJoin = TRUE;

		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember(idGuild);
		if (pGCMember == NULL)
		{
			bJoin = FALSE;
		}
		else
		{
			if (pGCMember->bRequest == FALSE)
				bJoin = FALSE;
		}

		if (bJoin)
		{
			g_GuildCombatMng.OutGuildCombat(idGuild, dwChannel);
			CUser* pMaster = (CUser*)prj.GetUserByID(pGuild->m_idMaster);
			if (IsValidObj(pMaster))
			{
				CString strMsg;
				strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_EXPENSE_RETURN));
				pMaster->AddText(strMsg);
			}

			CString strMsg;

			strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL));
			g_UserMng.AddGuildMsg(pGuild, strMsg);
			g_UserMng.AddGCIsRequest(idGuild, g_GuildCombatMng.IsRequestWarGuild(idGuild, TRUE));
		}
	}
}

void CDPDatabaseClient::OnResultGuildCombat(CAr& ar, DPID, DPID)
{
	ar >> g_GuildCombatMng.m_nGuildCombatIndex;
	ar >> g_GuildCombatMng.m_uWinGuildId;
	ar >> g_GuildCombatMng.m_nWinGuildCount;
	ar >> g_GuildCombatMng.m_uBestPlayer;

	g_GuildCombatMng.GuildCombatGameClear();

	g_UserMng.AddGCWinGuild();
	g_UserMng.AddGCBestPlayer();
}
void CDPDatabaseClient::OnPlayerPointGuildCombat(CAr& ar, DPID, DPID)
{
	OnGCPlayerPoint(ar);
}
void CDPDatabaseClient::OnContinueGC(CAr& ar, DPID, DPID)
{
	g_GuildCombatMng.m_uWinGuildId = 0;
	g_GuildCombatMng.m_nWinGuildCount = 0;

	g_UserMng.AddGCWinGuild();
}

void CDPDatabaseClient::OnAllPlayerData(CAr& ar, DPID, DPID)
{
	CPlayerDataCenter::GetInstance()->Serialize(ar);
}

void CDPDatabaseClient::OnAddPlayerData(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	PlayerData pd;
	ar >> idPlayer;
	ar.ReadString(pd.szPlayer, MAX_PLAYER);
	ar.Read(&pd.data, sizeof(sPlayerData));
	CPlayerDataCenter::GetInstance()->AddPlayerData(idPlayer, pd);
}

void CDPDatabaseClient::OnDeletePlayerData(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	ar >> idPlayer;
	CPlayerDataCenter::GetInstance()->DeletePlayerData(idPlayer);
}

void CDPDatabaseClient::SendUpdatePlayerData(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_UPDATE_PLAYER_DATA, DPID_UNKNOWN, DPID_UNKNOWN);
	sPlayerData data;
	data.nJob = pUser->GetJob();
	data.nLevel = pUser->GetLevel();
	data.nSex = pUser->GetSex();
	ar << pUser->m_idPlayer;
	ar.Write(&data, sizeof(sPlayerData));
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnUpdatePlayerData(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	sPlayerData data;
	ar >> idPlayer;
	ar.Read(&data, sizeof(sPlayerData));

	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
	if (pPlayerData)
	{
		memcpy(&pPlayerData->data, &data, sizeof(sPlayerData));
#if __VER >= 13 // __COUPLE_1117
		CCoupleHelper::Instance()->OnUpdatePlayerData(idPlayer, pPlayerData);
#endif	// __COUPLE_1117
	}
}

void CDPDatabaseClient::OnBaseGameSetting(CAr& ar, DPID, DPID)
{
#ifdef __S1108_BACK_END_SYSTEM
	BOOL bBaseGameSetting = FALSE;
	BOOL bFirst = FALSE;
	ar >> bFirst;
	ar >> bBaseGameSetting;
	if (bBaseGameSetting)
	{
		ar >> prj.m_fMonsterExpRate >> prj.m_fGoldDropRate >> prj.m_fItemDropRate >> prj.m_fMonsterHitRate;
		ar >> prj.m_fShopCost >> prj.m_fMonsterRebirthRate >> prj.m_fMonsterHitpointRate >> prj.m_fMonsterAggressiveRate >> prj.m_fMonsterRespawnRate;
	}

	g_UserMng.AddGameRate(prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP);
	g_UserMng.AddGameRate(prj.m_fGoldDropRate, GAME_RATE_GOLDDROP);
	g_UserMng.AddGameRate(prj.m_fItemDropRate, GAME_RATE_ITEMDROP);
	g_UserMng.AddGameRate(prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT);
	g_UserMng.AddGameRate(prj.m_fShopCost, GAME_RATE_SHOPCOST);
	g_UserMng.AddGameRate(prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH);
	g_UserMng.AddGameRate(prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT);
	g_UserMng.AddGameRate(prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE);
	g_UserMng.AddGameRate(prj.m_fMonsterRespawnRate, GAME_RATE_RESPAWN);

	if (bFirst)
	{
		CWorld* pWorld;
		pWorld = g_WorldMng.GetFirstActive();
		while (pWorld)
		{
			pWorld->LoadRegion();
			CRespawner* pRespawner = pWorld->m_respawner.Proto();
			for (int i = 0; i < (int)(pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION].size()); ++i)
			{
				CRespawnInfo* pRespawnInfo = &(pRespawner->m_vRespawnInfo[RESPAWNTYPE_REGION][i]);
				if (pRespawnInfo->m_dwType == OT_MOVER)
				{
					MoverProp* pMoverProp = prj.GetMoverProp(pRespawnInfo->m_dwIndex);
					if (pMoverProp && pMoverProp->dwFlying == 0 && pMoverProp->dwLevel > 0 && pMoverProp->dwLevel <= MAX_MONSTER_LEVEL)
					{
#ifdef __EVENTLUA_SPAWN
						if (pWorld->GetID() == WI_WORLD_MADRIGAL)
							prj.m_EventLua.SetAddSpawnRegion(pWorld->GetID(), pRespawnInfo->m_rect);
#endif // __EVENTLUA_SPAWN
					}	// if
				}	// if
			}	// for
			pWorld = pWorld->nextptr;
		}	// while
	}	// if
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterRespawnSetting(CAr& ar, DPID, DPID)
{
#ifdef __S1108_BACK_END_SYSTEM
	char szMonsterName[32];
	D3DXVECTOR3	vPos;
	int		nAddIndex;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;

	int nMonsterRespawnSize;
	ar >> nMonsterRespawnSize;
	for (int i = 0; i < nMonsterRespawnSize; ++i)
	{
		ar >> nAddIndex;
		ar.ReadString(szMonsterName, 32);
		ar >> vPos;
		ar >> nQuantity;
		ar >> nAggressive;
		ar >> nRange;
		ar >> nTime;

		MoverProp* pMoverProp = NULL;
		pMoverProp = prj.GetMoverProp(szMonsterName);

		if (pMoverProp)
		{
			CWorld* pWorld = g_WorldMng.GetWorld(WI_WORLD_MADRIGAL);

			if (pWorld == NULL)
				return;

			CRespawnInfo ri;
			ri.m_dwType = OT_MOVER;
			ri.m_dwIndex = pMoverProp->dwID;
			ri.m_nMaxcb = nQuantity;
			ri.m_nMaxAttackNum = nAggressive;
			if (pMoverProp->dwFlying != 0)
				ri.m_vPos = vPos;
			ri.m_rect.left = (LONG)(vPos.x - nRange);
			ri.m_rect.right = (LONG)(vPos.x + nRange);
			ri.m_rect.top = (LONG)(vPos.z - nRange);
			ri.m_rect.bottom = (LONG)(vPos.z + nRange);
			ri.m_uTime = nTime;
			ri.m_nGMIndex = nAddIndex;

			pWorld->m_respawner.Add(ri, TRUE);
		}
	}

	int nRemoveSize;
	int nRemoveIndex;
	ar >> nRemoveSize;
	for (int i = 0; i < nRemoveSize; ++i)
	{
		ar >> nRemoveIndex;

		CWorld* pWorld = g_WorldMng.GetWorld(WI_WORLD_MADRIGAL);

		if (pWorld == NULL)
			return;
		pWorld->m_respawner.Remove(nRemoveIndex, TRUE);
	}
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnMonsterPropSetting(CAr& ar, DPID, DPID)
{
#ifdef __S1108_BACK_END_SYSTEM
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;

	int		nMonsterPropSize;

	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;

	ar >> nMonsterPropSize;
	for (int i = 0; i < nMonsterPropSize; ++i)
	{
		ar.ReadString(szMonsterName, 32);
		ar >> nHitPoint;
		ar >> nAttackPower;
		ar >> nDefence;
		ar >> nExp;
		ar >> nItemDrop;
		ar >> nPenya;

		MoverProp* pMoverProp = NULL;
		pMoverProp = prj.GetMoverProp(szMonsterName);
		if (pMoverProp)
		{
			pMoverProp->m_fHitPoint_Rate = (float)nHitPoint / 100.0f;
			pMoverProp->m_fAttackPower_Rate = (float)nAttackPower / 100.0f;
			pMoverProp->m_fDefence_Rate = (float)nDefence / 100.0f;
			pMoverProp->m_fExp_Rate = (float)nExp / 100.0f;
			pMoverProp->m_fItemDrop_Rate = (float)nItemDrop / 100.0f;
			pMoverProp->m_fPenya_Rate = (float)nPenya / 100.0f;
			pMoverProp->m_bRate = TRUE;

			MONSTER_PROP MonsterProp;
			strcpy(MonsterProp.szMonsterName, szMonsterName);
			MonsterProp.nHitPoint = nHitPoint;
			MonsterProp.nAttackPower = nAttackPower;
			MonsterProp.nDefence = nDefence;
			MonsterProp.nExp = nExp;
			MonsterProp.nItemDrop = nItemDrop;
			MonsterProp.nPenya = nPenya;
			prj.AddMonsterProp(MonsterProp);
		}
	}

	int	nRemovePropSize;
	ar >> nRemovePropSize;
	for (int i = 0; i < nRemovePropSize; ++i)
	{
		ar.ReadString(szMonsterName, 32);
		MoverProp* pMoverProp = NULL;
		pMoverProp = prj.GetMoverProp(szMonsterName);
		if (pMoverProp)
		{
			pMoverProp->m_fHitPoint_Rate = 1.0f;
			pMoverProp->m_fAttackPower_Rate = 1.0f;
			pMoverProp->m_fDefence_Rate = 1.0f;
			pMoverProp->m_fExp_Rate = 1.0f;
			pMoverProp->m_fItemDrop_Rate = 1.0f;
			pMoverProp->m_fPenya_Rate = 1.0f;
			pMoverProp->m_bRate = FALSE;
			prj.RemoveMonsterProp(szMonsterName);
		}
	}

	g_UserMng.AddMonsterProp();
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::OnGMChat(CAr& ar, DPID, DPID)
{
#ifdef __S1108_BACK_END_SYSTEM
	int nSize;
	ar >> nSize;
	for (int i = 0; i < nSize; ++i)
	{
		ar.ReadString(prj.m_chGMChat[i], 256);
	}
	if (0 < nSize)
		g_UserMng.AddGMChat(nSize);
#endif // __S1108_BACK_END_SYSTEM
}

void CDPDatabaseClient::SendLogLevelUp(CMover* pSender, int Action)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LOG_LEVELUP, DPID_UNKNOWN, DPID_UNKNOWN);

	ar << pSender->GetExp1();
	ar << pSender->GetFlightLv();
	ar << pSender->GetLevel();
	ar << pSender->GetJob();
	ar << (DWORD)1;				// joblevel
	ar << pSender->m_nStr;
	ar << pSender->m_nDex;
	ar << pSender->m_nInt;
	ar << pSender->m_nSta;
	ar << pSender->m_nRemainGP;
	ar << (LONG)0;
	ar << pSender->GetGold();
	ar << ((CUser*)pSender)->GetPlayerTime();
	ar << Action;
	ar << pSender->m_idPlayer;

	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendChangeBankPass(const char* szName, const char* szNewPass, u_long uidPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CHANGEBANKPASS, DPID_UNKNOWN, DPID_UNKNOWN);

	ar.WriteString(szName);
	ar.WriteString(szNewPass);
	ar << uidPlayer;

	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifndef __FL_FIX_USELESS_LOGS
void CDPDatabaseClient::SendLogUniqueItem(CMover* pMover, CItem* pItem, int nOption)
{
	SendLogUniqueItem2(pMover, pItem->GetProp(), nOption);
}

void CDPDatabaseClient::SendLogUniqueItem2(CMover* pMover, ItemProp* pItemProp, int nOption)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LOG_UNIQUEITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar.WriteString(pMover->GetName());
	ar << pMover->GetWorld()->GetID();
	ar << pMover->GetPos();

	char szItemId[32] = { 0, };
	::memset(szItemId, 0, sizeof(szItemId));
	_stprintf(szItemId, "%d", pItemProp->dwID);
	ar.WriteString(szItemId);

	ar << nOption;
	ar << pMover->m_idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

void CDPDatabaseClient::SendGuildContribution(CONTRIBUTION_CHANGED_INFO& info, BYTE nLevelUp, LONG nMemberLevel)
{
	BEFORESENDDUAL(ar, PACKETTYPE_WD_GUILD_CONTRIBUTION, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << info;
	ar << nLevelUp;
	ar << nMemberLevel;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryGuildBank()
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILD_BANK_QUERY, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}


void CDPDatabaseClient::OnGuildBank(CAr& ar, DPID, DPID)
{
	int nCount;


	ar >> nCount;


	for (int i = 0; i < nCount; ++i)
	{
		int nGuildId = 0, nGoldGuild = 0;


		ar >> nGuildId;
		ar >> nGoldGuild;


		CGuild* pGuild = g_GuildMng.GetGuild(nGuildId);
		if (pGuild)
		{
			pGuild->m_nGoldGuild = nGoldGuild;
			pGuild->m_GuildBank.Serialize(ar);
		}
		else
		{

			CGuild tempGuild;
			tempGuild.m_GuildBank.Serialize(ar);
		}
	}
}

void CDPDatabaseClient::SendQueryGuildQuest(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendInsertGuildQuest(u_long idGuild, int nId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_INSERTGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild << nId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendUpdateGuildQuest(u_long idGuild, int nId, int nState)
{
	BEFORESENDDUAL(ar, PACKETTYPE_UPDATEGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idGuild << nId << nState;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnQueryGuildQuest(CAr& ar, DPID, DPID)
{
	int nCount;
	u_long idGuild;
	CGuild* pGuild;

	ar >> nCount;

	for (int i = 0; i < nCount; i++)
	{
		ar >> idGuild;
		pGuild = g_GuildMng.GetGuild(idGuild);
		if (pGuild)
		{
			ar >> pGuild->m_nQuestSize;
			ar.Read((void*)pGuild->m_aQuest, sizeof(GUILDQUEST) * pGuild->m_nQuestSize);
		}
		else
		{
			CGuild waste;
			ar >> waste.m_nQuestSize;
			ar.Read((void*)waste.m_aQuest, sizeof(GUILDQUEST) * waste.m_nQuestSize);
		}
	}
}

void CDPDatabaseClient::SendQuerySetPlayerName(u_long idPlayer, const char* lpszPlayer, DWORD dwData)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYSETPLAYERNAME, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(lpszPlayer);
	ar << dwData;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendPing(void)
{
	if (FALSE == m_bAlive)
	{
#ifndef _DEBUG
		Error("TRANS : is not alive");
		if (m_cbPing <= 1)
		{
			ExitProcess(-1);
			return;
		}
#endif	// _DEBUG
	}

	m_cbPing++;
	m_bAlive = FALSE;
	BEFORESENDDUAL(ar, PACKETTYPE_PING, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << time_null();
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnPing(CAr& ar, DPID, DPID)
{
	time_t tSend, tTrans;
	ar >> tSend >> tTrans;

	time_t d = time_null() - tSend;
	CTime tmTrans(tTrans);
#if !defined(_DEBUG)
	if (d > 10)
		Error("ping TRANS - %d, trans:%s", d, tmTrans.Format("%Y/%m/%d %H:%M:%S"));
#endif

	m_bAlive = TRUE;
}

void CDPDatabaseClient::SendQueryMailBox(u_long idReceiver)
{
	// 	//	BEGINTEST
	// 	Error( "SendQueryMailBox [%d]", idReceiver );

	BEFORESENDDUAL(ar, PACKETTYPE_QUERYMAILBOX, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryMailBoxReq(u_long idReceiver)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYMAILBOX_REQ, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryMailBoxCount(u_long idReceiver, int nCount)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYMAILBOX_COUNT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nCount;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnMailBox(CAr& ar, DPID, DPID)
{
	u_long idReceiver;
	ar >> idReceiver;
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);
	if (pMailBox)
	{
		// 		//	BEGINTEST
		// 		Error( "OnMailBox [%d]", idReceiver );

		pMailBox->Read(ar);	// pMailBox->m_nStatus	= CMailBox::data;
		CUser* pUser = g_UserMng.GetUserByPlayerID(idReceiver);
		if (IsValidObj(pUser))
		{
			// 			//	BEGINTEST
			// 			Error( "OnMailBox AddMailBox [%d]", idReceiver );

			pUser->AddMailBox(pMailBox);
			pUser->ResetCheckClientReq();
		}
	}
	else
	{
		Error("GetMailBox - pMailBox is NULL. idReceiver : %d", idReceiver);
	}
}

void CDPDatabaseClient::OnMailBoxReq(CAr& ar, DPID, DPID)
{
	u_long idReceiver;
	BOOL bHaveMailBox = FALSE;
	ar >> idReceiver >> bHaveMailBox;

	CUser* pUser = g_UserMng.GetUserByPlayerID(idReceiver);
	if (IsValidObj(pUser))
	{
		pUser->CheckTransMailBox(TRUE);
		pUser->SendCheckMailBoxReq(bHaveMailBox);
	}

	if (bHaveMailBox == TRUE)
	{
		CMailBox* pMailBox = NULL;
		pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);

		if (pMailBox == NULL)
		{
			CMailBox* pNewMailBox = new CMailBox(idReceiver);
			if (pNewMailBox != NULL)
			{
				if (CPost::GetInstance()->AddMailBox(pNewMailBox) == TRUE)
				{
					pNewMailBox->ReadReq(ar);	// pMailBox->m_nStatus	= CMailBox::data;

					CUser* pUser = g_UserMng.GetUserByPlayerID(idReceiver);
					if (IsValidObj(pUser))
					{
						pUser->AddMailBox(pNewMailBox);
						pUser->ResetCheckClientReq();
					}
				}
				else
				{
					Error("CDPDatabaseClient::OnMailBoxReq - AddMailBox Failed. idReceiver : %d", idReceiver);
					SAFE_DELETE(pNewMailBox);
				}
			}
			else
			{
				Error("CDPDatabaseClient::OnMailBoxReq - MailBox Create Failed");
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////////
			//	??????????
			Error("CDPDatabaseClient::OnMailBoxReq - pMailBox is NOT NULL. idReceiver : %d", idReceiver);
		}
	}
	else
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(idReceiver);
		if (IsValidObj(pUser))
		{
			pUser->ResetCheckClientReq();
		}
	}
}

void CDPDatabaseClient::SendQueryPostMail(u_long idReceiver, u_long idSender, CItemElem& itemElem, int nGold, char* lpszTitle, char* lpszText)
{
	// 	//	BEGINTEST
	// 	Error( "SendQueryPostMail  Receiver[%d] Sender[%d]", idReceiver, idSender );

	BEFORESENDDUAL(ar, PACKETTYPE_QUERYPOSTMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << idSender;
	if (FALSE == itemElem.IsEmpty())
	{
		ar << (BYTE)1;
		itemElem.Serialize(ar);
	}
	else
	{
		ar << (BYTE)0;
	}
	ar << nGold;
	ar.WriteString(lpszTitle);
	ar.WriteString(lpszText);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryRemoveMail(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYREMOVEMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryGetMailItem(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGETMAILITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryGetMailGold(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGETMAILGOLD, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail << g_uIdofMulti/*uQuery*/;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendQueryReadMail(u_long idReceiver, u_long nMail)
{
	BEFORESENDDUAL(ar, PACKETTYPE_READMAIL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << nMail;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnPostMail(CAr& ar, DPID, DPID)
{
	BOOL	bResult;
	u_long idReceiver;
	ar >> bResult >> idReceiver;
	CMail* pMail = new CMail;
	pMail->Serialize(ar);

	BOOL bBuying = FALSE;

	if (TRUE == bResult)
	{
		if (CPost::GetInstance()->AddMail(idReceiver, pMail) <= 0)
		{
			Error("OnPostMail - pMail->m_nMail : %d", pMail->m_nMail);
			SAFE_DELETE(pMail);
		}

		CUser* pUser = (CUser*)prj.GetUserByID(idReceiver);
		if (IsValidObj(pUser))
		{
			if (pUser->IsPosting())
			{
				pUser->AddPostMail(pMail);
			}
			{
				pUser->SetMode(MODE_MAILBOX);
				g_UserMng.AddModifyMode(pUser);
			}
		}
		pUser = (CUser*)prj.GetUserByID(pMail->m_idSender);
		if (IsValidObj(pUser) && !bBuying)
			pUser->AddDiagText(prj.GetText(TID_MAIL_SEND_OK));
	}	// FAIL
	else
	{
		Error("OnPostMail - Send Mail Failed. idSender : %d, idReceiver : %d", pMail->m_idSender, idReceiver);
		CUser* pUser = (CUser*)prj.GetUserByID(pMail->m_idSender);
		if (IsValidObj(pUser))
		{
			if (pMail->m_pItemElem)
				pUser->CreateItem(pMail->m_pItemElem);

			pUser->AddGold(500 + pMail->m_nGold, TRUE);

			pUser->AddDiagText(prj.GetText(TID_GAME_POST_ERROR));
			CWorld* pWorld = pUser->GetWorld();
			if (pWorld)
				g_dpDBClient.SavePlayer(pUser, pWorld->GetID(), pUser->GetPos(), pUser->GetLayer());
		}

		SAFE_DELETE(pMail);
	}
}

void CDPDatabaseClient::OnRemoveMail(CAr& ar, DPID, DPID)
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);

	if (pMailBox)
	{
		pMailBox->RemoveMail(nMail);

		CUser* pUser = (CUser*)prj.GetUserByID(idReceiver);
		if (IsValidObj(pUser))
		{
			pUser->AddRemoveMail(nMail, CMail::mail);
			if (pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode(MODE_MAILBOX))
			{
				pUser->SetNotMode(MODE_MAILBOX);
				g_UserMng.AddModifyMode(pUser);
			}
		}
	}
}

void CDPDatabaseClient::OnGetMailItem(CAr& ar, DPID, DPID)
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);
	CItemElem	itemRestore;

	if (pMailBox)
	{
		CMail* pMail = pMailBox->GetMail(nMail);
		if (pMail && pMail->m_pItemElem)
		{
			itemRestore = *(pMail->m_pItemElem);
			CUser* pUser = (CUser*)prj.GetUserByID(idReceiver);
			if (IsValidObj(pUser))
			{
				if (g_uIdofMulti == uQuery)
				{
					if (pUser->CreateItem(pMail->m_pItemElem))
					{
						pUser->AddRemoveMail(nMail, CMail::item);
						itemRestore.Empty();

						if (pUser->GetWorld())
							g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
					}
				}
				else
				{
					pUser->AddRemoveMail(nMail, CMail::item);
					itemRestore.Empty();
				}
			}
		}
		pMailBox->RemoveMailItem(nMail);
	}
	if (!itemRestore.IsEmpty() && g_uIdofMulti == uQuery)
		g_dpDBClient.SendQueryPostMail(idReceiver, 0, itemRestore, 0, "", "");
}

void CDPDatabaseClient::OnGetMailGold(CAr& ar, DPID, DPID)
{
	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);
	int nRestore = 0;

	if (pMailBox)
	{
		CMail* pMail = pMailBox->GetMail(nMail);
		if (pMail && pMail->m_nGold > 0)
		{
			nRestore = pMail->m_nGold;
			CUser* pUser = (CUser*)prj.GetUserByID(idReceiver);
			if (IsValidObj(pUser))
			{
				if (g_uIdofMulti == uQuery)
				{
					if (CanAdd(pUser->GetGold(), pMail->m_nGold))
					{
						pUser->AddGold(pMail->m_nGold);
						pUser->AddRemoveMail(nMail, CMail::gold);
						nRestore = 0;

						if (pUser->GetWorld())
							g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

					}
				}
				else
				{
					pUser->AddRemoveMail(nMail, CMail::gold);
					nRestore = 0;
				}
			}
		}
		pMailBox->RemoveMailGold(nMail);
	}
	if (nRestore > 0 && g_uIdofMulti == uQuery)
	{
		CItemElem itemElem;		// empty item
		g_dpDBClient.SendQueryPostMail(idReceiver, 0, itemElem, nRestore, "", "");
	}
}

void CDPDatabaseClient::OnReadMail(CAr& ar, DPID, DPID)
{
	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(idReceiver);

	if (pMailBox)
	{
		pMailBox->ReadMail(nMail);
		CUser* pUser = (CUser*)prj.GetUserByID(idReceiver);
		if (IsValidObj(pUser))
		{
			pUser->ResetCheckClientReq();
			pUser->AddRemoveMail(nMail, CMail::read);
			if (pMailBox->IsStampedMailExists() == FALSE && pUser->IsMode(MODE_MAILBOX))
			{
				pUser->SetNotMode(MODE_MAILBOX);
				g_UserMng.AddModifyMode(pUser);
			}
		}
	}
}

void CDPDatabaseClient::OnAllMail(CAr& ar, DPID, DPID)
{
	CPost::GetInstance()->Serialize(ar, FALSE);
}

void CDPDatabaseClient::OnQueryRemoveGuildBankTbl(CAr& ar, DPID, DPID)
{
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY) == 0)
		return;

	int nNo;
	u_long idGuild;
	DWORD dwItemId;
	SERIALNUMBER iSerialNumber;
	DWORD dwItemNum;
	ar >> nNo >> idGuild >> dwItemId >> iSerialNumber >> dwItemNum;

	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	DWORD dwRemoved = 0;
	if (pGuild)
	{
		if (dwItemNum < 1)
			return;

		if (dwItemId == 0)		// penya
		{
			dwRemoved = dwItemNum;
			if (pGuild->m_nGoldGuild - dwRemoved > pGuild->m_nGoldGuild)	// overflow
				dwRemoved = pGuild->m_nGoldGuild;
			pGuild->m_nGoldGuild -= dwRemoved;
			if (dwRemoved > 0)
			{
				g_DPSrvr.UpdateGuildBank(pGuild, GUILD_QUERY_REMOVE_GUILD_BANK);
				CUser* pUsertmp;
				CGuildMember* pMember;
				for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
				{
					pMember = i->second;
					pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
					if (IsValidObj(pUsertmp) && pUsertmp->m_bGuildBank)
						pUsertmp->AddRemoveGuildBankItem(idGuild, NULL_ID, dwRemoved);
				}
			}
		}
		else	// item
		{
			CItemElem* pItemElem = pGuild->GetItem(dwItemId, iSerialNumber);
			if (pItemElem)
			{
				DWORD dwId = pItemElem->m_dwObjId;
				if (static_cast<short>(dwItemNum) < 0)
					dwItemNum = 0;

				dwRemoved = (DWORD)pGuild->RemoveItem(dwId, static_cast<short>(dwItemNum));
				if (dwRemoved > 0)
				{
					g_DPSrvr.UpdateGuildBank(pGuild, GUILD_QUERY_REMOVE_GUILD_BANK);

					CUser* pUsertmp;
					CGuildMember* pMember;
					for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
					{
						pMember = i->second;
						pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
						if (IsValidObj(pUsertmp) && pUsertmp->m_bGuildBank)
							pUsertmp->AddRemoveGuildBankItem(idGuild, dwId, dwRemoved);
					}
				}
			}
		}
	}

	SendQueryRemoveGuildBankTbl(nNo, dwRemoved);
}

void CDPDatabaseClient::SendQueryRemoveGuildBankTbl(int nNo, DWORD dwRemoved)
{
	BEFORESENDDUAL(ar, PACKETTYPE_QUERY_REMOVE_GUILD_BANK_TBL, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nNo << dwRemoved;
	SEND(ar, this, DPID_SERVERPLAYER);
}

#if __VER >= 9 // __EVENTLUA
void CDPDatabaseClient::OnEventStateLua(CAr& ar, DPID, DPID)
{
	BOOL bTextOut = FALSE;
	int nMapSize = 0;
	ar >> bTextOut;
	ar >> nMapSize;

	BYTE nId;
	BOOL bState;
	for (int i = 0; i < nMapSize; i++)
	{
		ar >> nId;
		ar >> bState;
		prj.m_EventLua.SetState(nId, bState);
		if (bTextOut)
			g_UserMng.AddEventLua(nId, bState);
	}
#if __VER >= 12 // __EVENTLUA_0826
	prj.m_EventLua.PrepareProxy();
#endif	// __EVENTLUA_0826
}

void CDPDatabaseClient::OnEventLuaChanged(CAr& ar, DPID, DPID)
{
	prj.m_EventLua.LoadScript();
	if (prj.m_EventLua.m_bRun)
		Error("Event.lua reloaded");
}
#endif // __EVENTLUA

void CDPDatabaseClient::CalluspLoggingQuest(u_long idPlayer, int nQuest, int nState, const char* pszComment)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CALL_USPLOGGINGQUEST, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << nQuest << nState;
	ar.WriteString(pszComment);
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifdef __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer(u_long uKey, CUser* pUser)
#else	// __INVALID_LOGIN_0320
void CDPDatabaseClient::CalluspXXXMultiServer(u_long uKey, u_long idPlayer)
#endif	// __INVALID_LOGIN_0320
{
	BEFORESENDDUAL(ar, PACKETTYPE_CALL_XXX_MULTI_SERVER, DPID_UNKNOWN, DPID_UNKNOWN);
#ifdef __INVALID_LOGIN_0320
	if (pUser)
	{
		ar << uKey << pUser->m_idPlayer;
		ar.WriteString(pUser->m_playAccount.lpszAccount);
	}
	else
	{
		ar << uKey << 0;
		ar.WriteString("");	// not used
}
#else	// __INVALID_LOGIN_0320
	ar << uKey << idPlayer;
#endif	// __INVALID_LOGIN_0320
	SEND(ar, this, DPID_SERVERPLAYER);
}

#if __VER >= 9 // __EVENTLUA
void CDPDatabaseClient::SendEventLuaChanged(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_EVENTLUA_CHANGED, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __EVENTLUA

#if __VER >= 11

void CDPDatabaseClient::SendQueryGetGuildBankLogList(u_long idReceiver, DWORD idGuild, BYTE byListType)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idReceiver << idGuild << byListType << g_uIdofMulti/*uQuery*/;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnGuildBankLogViewFromDB(CAr& ar, DPID, DPID)
{
	u_long idReceiver;
	int	nCount = 0;
	BYTE	byListType = 0;
	ar >> nCount;
	ar >> byListType;
	ar >> idReceiver;

	if (nCount > GUILDBANKLOGVIEW_MAX)
		return;

	CUser* pUser = g_UserMng.GetUserByPlayerID(idReceiver);
	if (IsValidObj(pUser))
	{
		__GUILDBANKLOG_ENTRY	logs[GUILDBANKLOGVIEW_MAX];
		for (int i = 0; i < nCount; ++i)
		{
			ar.ReadString(logs[i].szPlayer, 42);
			ar >> logs[i].nDate;
			ar >> logs[i].nItemID;
			ar >> logs[i].nItemAbilityOption;
			ar >> logs[i].nItemCount;
		}
		pUser->AddGuildBankLogView(byListType, (short)nCount, logs);
	}
}

#endif

#if __VER >= 12 // __LORD
void CDPDatabaseClient::OnElectionAddDeposit(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	__int64 iDeposit;
	time_t tCreate;
	BOOL bRet;
	ar >> idPlayer >> iDeposit >> tCreate >> bRet;

	CUser* pUser = static_cast<CUser*>(prj.GetUserByID(idPlayer));
	if (IsValidObj(pUser))
		pUser->SetQuerying(FALSE);

	if (bRet)
	{
		IElection* pElection = CSLord::Instance()->GetElection();
		pElection->AddDeposit(idPlayer, iDeposit, tCreate);
	}
	else
	{
		// *Restore
		if (IsValidObj(pUser))
			pUser->AddGold(static_cast<int>(iDeposit));
	}
}

void CDPDatabaseClient::OnElectionSetPledge(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	char szPledge[CCandidate::nMaxPledgeLen] = { 0, };
	BOOL bRet;
	ar >> idPlayer;
	ar.ReadString(szPledge, CCandidate::nMaxPledgeLen);
	ar >> bRet;

	CUser* pUser = static_cast<CUser*>(prj.GetUserByID(idPlayer));
	if (IsValidObj(pUser))
		pUser->SetQuerying(FALSE);

	IElection* pElection = CSLord::Instance()->GetElection();
	if (bRet)
	{
		pElection->SetPledge(idPlayer, szPledge);
	}
	else
	{
		// *Restore
		if (pElection->HasPledge(idPlayer))
		{
			if (IsValidObj(pUser))
				pUser->AddGold(static_cast<int>(CCandidate::nSetPledgeCost));
		}
	}
}

void CDPDatabaseClient::OnElectionIncVote(CAr& ar, DPID, DPID)
{
	u_long idPlayer, idElector;
	BOOL bRet;
	ar >> idPlayer >> idElector >> bRet;

	CUser* pUser = g_UserMng.GetUserByPlayerID(idElector);
	if (IsValidObj(pUser))
		pUser->SetQuerying(FALSE);

	if (bRet)
	{
		IElection* pElection = CSLord::Instance()->GetElection();
		pElection->IncVote(idPlayer, idElector);
		if (IsValidObj(pUser))
		{
			pUser->SetElection(pElection->GetId());

			ItemProp* pProp = prj.GetItemProp(II_SYS_SYS_VOTE_THANKS);
			if (pProp)
				pUser->DoApplySkill(pUser, pProp, NULL);
		}
	}
	else
	{
		// *print
		if (IsValidObj(pUser))
			pUser->AddDefinedText(TID_GAME_ELECTION_INC_VOTE_E004);
	}
}

void CDPDatabaseClient::OnElectionBeginCandidacy(CAr& ar, DPID, DPID)
{
	IElection* pElection = CSLord::Instance()->GetElection();
	pElection->BeginCandidacy();
}

void CDPDatabaseClient::OnElectionBeginVote(CAr& ar, DPID, DPID)
{
	int nRequirement;
	ar >> nRequirement;

	IElection* pElection = CSLord::Instance()->GetElection();
	pElection->BeginVote(nRequirement);
}

void CDPDatabaseClient::OnElectionEndVote(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	ar >> idPlayer;

	IElection* pElection = CSLord::Instance()->GetElection();
	pElection->EndVote(idPlayer);
}

void CDPDatabaseClient::OnLord(CAr& ar, DPID, DPID)
{
	CSLord::Instance()->Serialize(ar);
}

void CDPDatabaseClient::OnLEventCreate(CAr& ar, DPID, DPID)
{
	CLEComponent* pComponent = new CLEComponent;
	pComponent->Serialize(ar);
	BOOL bResult;
	ar >> bResult;

	CUser* pUser = g_UserMng.GetUserByPlayerID(pComponent->GetIdPlayer());
	if (IsValidObj(pUser))
		pUser->SetQuerying(FALSE);

	if (bResult)
	{
		ILordEvent* pEvent = CSLord::Instance()->GetEvent();
		pEvent->AddComponent(pComponent);
	}
	else
		SAFE_DELETE(pComponent);
}

void CDPDatabaseClient::OnLEventInitialize(CAr& ar, DPID, DPID)
{
	ILordEvent* pEvent = CSLord::Instance()->GetEvent();
	pEvent->Initialize();
}

void CDPDatabaseClient::OnLordSkillUse(CAr& ar, DPID, DPID)
{
	u_long idPlayer, idTarget;
	int nSkill, nRet;
	ar >> idPlayer >> idTarget >> nSkill >> nRet;

	CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
	if (!nRet)
	{
		CLordSkill* pSkills = CSLord::Instance()->GetSkills();
		CLordSkillComponentExecutable* pComponent = pSkills->GetSkill(nSkill);
		pComponent->Execute(idPlayer, idTarget, NULL);
		if (IsValidObj(pUser))
			g_UserMng.AddLordSkillUse(pUser, idTarget, nSkill);
	}
	else
	{
		CUser* pUser = g_UserMng.GetUserByPlayerID(idPlayer);
		if (IsValidObj(pUser))
			pUser->AddDefinedText(nRet);
	}
}

void CDPDatabaseClient::OnLordSkillTick(CAr& ar, DPID, DPID)
{
	CLordSkill* pSkills = CSLord::Instance()->GetSkills();
	pSkills->SerializeTick(ar);
	if (CSLord::Instance()->Get() == NULL_ID)
		return;
	CUser* pLord = g_UserMng.GetUserByPlayerID(CSLord::Instance()->Get());
	if (IsValidObj(pLord))
		pLord->AddLordSkillTick(pSkills);
}

void CDPDatabaseClient::OnLEventTick(CAr& ar, DPID, DPID)
{
	ILordEvent* pEvent = CSLord::Instance()->GetEvent();
	pEvent->SerializeTick(ar);

	if (pEvent->GetComponentSize() == 0)
		return;

	g_UserMng.AddLEventTick(pEvent);
	pEvent->EraseExpiredComponents();
}

void CDPDatabaseClient::SendElectionAddDeposit(u_long idPlayer, __int64 iDeposit)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_ADD_DEPOSIT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << iDeposit;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendElectionSetPledge(u_long idPlayer, const char* szPledge)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_SET_PLEDGE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(szPledge);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendElectionIncVote(u_long idPlayer, u_long idElector)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_INC_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idElector;
	SEND(ar, this, DPID_SERVERPLAYER);
}

#ifndef __FL_UPDATE_LORDEVENT
void CDPDatabaseClient::SendLEventCreate(u_long idPlayer, int iEEvent, int iIEvent)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << iEEvent << iIEvent;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#else
void CDPDatabaseClient::SendLEventCreate(u_long idPlayer, int iEEvent, int iIEvent, int iPEvent)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_CREATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << iEEvent << iIEvent;
	ar << iPEvent;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

void CDPDatabaseClient::SendLordSkillUse(u_long idPlayer, u_long idTarget, int nSkill)
{
	BEFORESENDDUAL(ar, PACKETTYPE_LORD_SKILL_USE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << idTarget << nSkill;
	SEND(ar, this, DPID_SERVERPLAYER);
}

// operator commands
void CDPDatabaseClient::SendLEventInitialize(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_L_EVENT_INITIALIZE, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendElectionProcess(BOOL bRun)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_PROCESS, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << bRun;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendElectionBeginCandidacy(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_BEGIN_CANDIDACY, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendElectionBeginVote(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_BEGIN_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendElectionBeginEndVote(void)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ELECTION_END_VOTE, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CDPDatabaseClient::SendSecretRoomWinGuild(BYTE nCont, DWORD dwGuildId)
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_SET_SECRETROOM_WINNER, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nCont << dwGuildId;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendLord(DWORD dwIdPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_SET_LORD, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwIdPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendTaxRate(BYTE nCont, int nSalesTaxRate, int nPurchaseTaxRate)
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_SET_TAXRATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nCont;
	ar << nSalesTaxRate << nPurchaseTaxRate;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendApplyTaxRateNow()
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_APPLY_TAXRATE_NOW, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnTaxInfo(CAr& ar, DPID, DPID)
{
	CTax::GetInstance()->Serialize(ar);

	BOOL bConnect, bToAllClient;
	ar >> bConnect;
	if (bConnect)
	{
		ar >> CTax::GetInstance()->m_nMinTaxRate;
		ar >> CTax::GetInstance()->m_nMaxTaxRate;
#ifdef __OCCUPATION_SHOPITEM
		size_t nSize;
		ar >> nSize;
		for (size_t i = 0; i < nSize; i++)
		{
			DWORD dwItemId;
			ar >> dwItemId;
			CTax::GetInstance()->m_vecdwOccupationShopItem.push_back(dwItemId);
		}
#endif // __OCCUPATION_SHOPITEM
	}
	ar >> bToAllClient;
	if (bToAllClient)
	{
		g_UserMng.AddTaxInfo();
#if __VER >= 12 // __SECRET_ROOM
		if (g_eLocal.GetState(EVE_SECRETROOM))
			g_DPCoreClient.SendCaption(GETTEXT(TID_GAME_TAX_CHANGENEXT));
#endif // __SECRET_ROOM

		CWorld* pWorld = g_WorldMng.GetWorld(WI_DUNGEON_SECRET_0);
		if (!pWorld)
			return;

		CObj* pObj;
		for (int i = 0; i < (int)(pWorld->m_dwObjNum); i++)
		{
			pObj = pWorld->m_apObject[i];
			if (pObj && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && !pObj->IsDelete())
				((CUser*)pObj)->REPLACE(g_uIdofMulti, ((CUser*)pObj)->m_idMarkingWorld, ((CUser*)pObj)->m_vMarkingPos, REPLACE_NORMAL, nTempLayer);
		}
	}
}

void CDPDatabaseClient::SendAddTax(BYTE nCont, int nTax, BYTE nTaxKind)
{
	BEFORESENDDUAL(ar, PACKETTYPE_TAX_ADDTAX, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nCont << nTax << nTaxKind;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __TAX

#if __VER >= 12 // __SECRET_ROOM
void CDPDatabaseClient::SendSecretRoomInsertToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_TENDER_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nContinent << srTender.dwGuildId << srTender.nPenya;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendSecretRoomUpdateToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender, char chState)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_TENDER_UPDATETODB, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nContinent << srTender.dwGuildId << srTender.nPenya << chState;
	if (chState == 'W' || chState == 'L')
		ar << srTender.dwWorldId << srTender.nWarState << srTender.nKillCount;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendSecretRoomInsertLineUpToDB(BYTE nContinent, __SECRETROOM_TENDER& srTender)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_LINEUP_INSERTTODB, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << nContinent << srTender.dwGuildId;
	ar << srTender.vecLineUpMember.size();
	for (int i = 0; i < (int)(srTender.vecLineUpMember.size()); i++)
		ar << srTender.vecLineUpMember[i];
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendSecretRoomClosed()
{
	BEFORESENDDUAL(ar, PACKETTYPE_SECRETROOM_CLOSED, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnSecretRoomInfoClear(CAr& ar, DPID, DPID)
{
	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.begin();
	for (; it != pSRMng->m_mapSecretRoomContinent.end(); it++)
	{
		CSecretRoomContinent* pSRCont = it->second;
		pSRCont->m_vecSecretRoomTender.clear();
	}
}

void CDPDatabaseClient::OnSecretRoomTenderInfo(CAr& ar, DPID, DPID)
{
	BYTE nContinent; DWORD dwGuildId, dwMemberId; int nPenya, nSize;
	vector<DWORD> vecMemberId;
	ar >> nContinent >> dwGuildId >> nPenya;
	ar >> nSize;
	for (int i = 0; i < nSize; i++)
	{
		ar >> dwMemberId;
		vecMemberId.push_back(dwMemberId);
	}

	if (!g_eLocal.GetState(EVE_SECRETROOM))
		return;

	CSecretRoomMng* pSRMng = CSecretRoomMng::GetInstance();
	map<BYTE, CSecretRoomContinent*>::iterator it = pSRMng->m_mapSecretRoomContinent.find(nContinent);
	if (it != pSRMng->m_mapSecretRoomContinent.end())
	{
		CSecretRoomContinent* pSRCont = it->second;
		__SECRETROOM_TENDER srTender;
		srTender.dwGuildId = dwGuildId;
		srTender.nPenya = nPenya;
		srTender.vecLineUpMember.assign(vecMemberId.begin(), vecMemberId.end());
		pSRCont->m_vecSecretRoomTender.push_back(srTender);
	}
}
#endif // __SECRET_ROOM

#if __VER >= 13 // __HOUSING
void CDPDatabaseClient::OnHousingLoadInfo(CAr& ar, DPID, DPID)
{
	DWORD dwPlayerId;
	ar >> dwPlayerId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing(dwPlayerId);
	if (!pHousing)
		pHousing = CHousingMng::GetInstance()->CreateHousing(dwPlayerId);
	else
		Error("CDPDatabaseClient::OnHousingLoadInfo - %d", dwPlayerId);

	if (pHousing)
	{
		pHousing->Serialize(ar);
		CUser* pUser = static_cast<CUser*>(prj.GetUserByID(dwPlayerId));
		if (IsValidObj(pUser))
			pUser->AddHousingAllInfo(pHousing);
		pHousing->AddAllFurnitureControl();
	}
}

void CDPDatabaseClient::OnHousingSetFunitureList(CAr& ar, DPID, DPID)
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	BOOL bAdd;

	ar >> dwPlayerId;
	housingInfo.Serialize(ar);
	ar >> bAdd;

	CHousingMng::GetInstance()->SetFurnitureList(dwPlayerId, housingInfo, bAdd);
}

void CDPDatabaseClient::OnHousingSetupFuniture(CAr& ar, DPID, DPID)
{
	DWORD dwPlayerId;
	HOUSINGINFO housingInfo;
	ar >> dwPlayerId;
	housingInfo.Serialize(ar);

	CHousingMng::GetInstance()->SetupFurniture(dwPlayerId, housingInfo);
}

void CDPDatabaseClient::OnHousingSetVisitAllow(CAr& ar, DPID, DPID)
{
	DWORD dwPlayerId, dwTargetId;
	BOOL bAllow;
	ar >> dwPlayerId >> dwTargetId >> bAllow;

	CHousingMng::GetInstance()->SetVisitAllow(dwPlayerId, dwTargetId, bAllow);
}

void CDPDatabaseClient::OnHousingDBFailed(CAr& ar, DPID, DPID)
{
	DWORD dwPlayerId, dwItemId;
	ar >> dwPlayerId >> dwItemId;

	CHousing* pHousing = CHousingMng::GetInstance()->GetHousing(dwPlayerId);
	if (pHousing)	pHousing->Setting(FALSE);

	if (dwItemId != NULL_ID)
	{
		CUser* pUser = static_cast<CUser*>(prj.GetUserByID(dwPlayerId));
		if (IsValidObj(pUser))
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = 1;
			if (!pUser->CreateItem(&itemElem))
				Error("CDPDatabaseClient::OnHousingDBFailed -> CreateItem Failed! - %d, %s, %d", dwPlayerId, pUser->GetName(), dwItemId);
		}
		else
			Error("CDPDatabaseClient::OnHousingDBFailed -> Invalid User - %d, %d", dwPlayerId, dwItemId);
	}
}
#endif // __HOUSING

#if __VER >= 13 // __COUPLE_1117
void CDPDatabaseClient::SendPropose(u_long idProposer, u_long idTarget)
{
	BEFORESENDDUAL(ar, PACKETTYPE_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idProposer << idTarget;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendCouple(u_long idProposer, u_long idTarget)
{
	BEFORESENDDUAL(ar, PACKETTYPE_COUPLE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idProposer << idTarget;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendDecouple(u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_DECOUPLE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendClearPropose()
{
	BEFORESENDDUAL(ar, PACKETTYPE_CLEAR_PROPOSE, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnProposeResult(CAr& ar, DPID, DPID)
{
	CCoupleHelper::Instance()->OnProposeResult(ar);
}

void CDPDatabaseClient::OnCoupleResult(CAr& ar, DPID, DPID)
{
	CCoupleHelper::Instance()->OnCoupleResult(ar);
}

void CDPDatabaseClient::OnDecoupleResult(CAr& ar, DPID, DPID)
{
	CCoupleHelper::Instance()->OnDecoupleResult(ar);
}

void CDPDatabaseClient::OnCouple(CAr& ar, DPID, DPID)
{
	CCoupleHelper::Instance()->Serialize(ar);
}

#if __VER >= 13 // __COUPLE_1202
void CDPDatabaseClient::SendQueryAddCoupleExperience(u_long idPlayer, int nExperience)
{
	BEFORESENDDUAL(ar, PACKETTYPE_ADD_COUPLE_EXPERIENCE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer << nExperience;
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::OnAddCoupleExperience(CAr& ar, DPID, DPID)
{
	CCoupleHelper::Instance()->OnAddCoupleExperience(ar);
}
#endif	// __COUPLE_1202
#endif	// __COUPLE_1117


#if __VER >= 15 // __GUILD_HOUSE
void CDPDatabaseClient::OnLoadGuildHouse(CAr& ar, DPID, DPID)
{
	GuildHouseMng->Serialize(ar);
}
void CDPDatabaseClient::OnBuyGuildHouse(CAr& ar, DPID, DPID)
{
	GuildHouseMng->OnBuyGuildHouse(ar);
}
void CDPDatabaseClient::OnRemoveGuildHouse(CAr& ar, DPID, DPID)
{
	GuildHouseMng->OnRemoveGuildHouse(ar);
}

void CDPDatabaseClient::OnGuildHousePacket(CAr& ar, DPID, DPID)
{
	BOOL bResult; DWORD dwGuildId;
	int nPacketType, nIndex;
	GH_Fntr_Info gfi;

	ar >> bResult >> dwGuildId;
	ar >> nPacketType >> nIndex;
	gfi.Serialize(ar);

	CGuildHouseBase* pGuildHouse = GuildHouseMng->GetGuildHouse(dwGuildId);
	if (pGuildHouse)
	{
		if (bResult)
		{
			if (pGuildHouse->OnGuildHousePacket(nPacketType, gfi, nIndex))
				pGuildHouse->SendWorldToClient(nPacketType, gfi, nIndex);
		}
		else
		{
			pGuildHouse->AfterFailedGuildHousePacket(nPacketType, gfi, nIndex);
		}
		pGuildHouse->SetWaitDBAckPlayerId(NULL_ID);
	}
}

void CDPDatabaseClient::SendLogGuildFurniture(DWORD dwGuildId, GH_Fntr_Info& gfi, char chState)
{
	BEFORESENDDUAL(ar, PACKETTYPE_GUILDFURNITURE_LOG, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << dwGuildId;
	gfi.Serialize(ar);
	ar << chState;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif // __GUILD_HOUSE

#ifdef __FL_GW_STATUE_V2
void CDPDatabaseClient::SendRequestToInstallStatues(u_long idMvp, u_long idLeader)
{
	BEFORESENDDUAL(ar, PACKETTYPE_REQUEST_INSTALL_STATUES, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idMvp << idLeader;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::OnInstallStatues(CAr& ar, DPID, DPID)
{
	// Delete old statues before creating new ones
	g_GuildCombatMng.DeleteStatues();

	int nCount;
	ar >> nCount;

	for (int i = 0; i < nCount; i++)
	{
		DWORD dwWhoIsThat;
		DWORD dwidPlayer, dwJob, dwLevel, dwSexIndex, dwHairId, dwHairColorId, dwHeadId;
		char szName[64];
		CItemContainer< CItemElem > tmpInventoryOwner;
		tmpInventoryOwner.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ar >> dwWhoIsThat;
		ar >> dwidPlayer >> dwJob >> dwLevel >> dwSexIndex >> dwHairId >> dwHairColorId >> dwHeadId;
		ar.ReadString(szName, 64);
		tmpInventoryOwner.Serialize(ar);

		PLAYER_COPY tmpStruc;
		tmpStruc.idPlayer = dwidPlayer;
		tmpStruc.strName = szName;
		tmpStruc.nJob = dwJob;
		tmpStruc.nLevel = dwLevel;
		tmpStruc.dwIndex = dwSexIndex == SEX_FEMALE ? MI_FEMALE : MI_MALE;
		tmpStruc.dwHairId = dwHairId;
		tmpStruc.dwHairColor = dwHairColorId;
		tmpStruc.dwHeadId = dwHeadId;
		tmpStruc.tmpInventoryOwner = &tmpInventoryOwner;

		if (dwWhoIsThat == 1) // Same person is mvp and leader (nCount = 1)
		{
			tmpStruc.vPos = vMvpStatuePos;
			tmpStruc.dwWorld = dwMvpStatueWorld;
			tmpStruc.fAngle = fMvpStatueAngle;

			if (g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_MVPSTATUE, tmpStruc))
				g_UserMng.AddWorldCreateSfxObj(dwStatuesAppearSfx, tmpStruc.vPos.x, tmpStruc.vPos.y, tmpStruc.vPos.z, FALSE, tmpStruc.dwWorld);

			tmpStruc.vPos = vLeaderStatuePos;
			tmpStruc.dwWorld = dwLeaderStatueWorld;
			tmpStruc.fAngle = fLeaderStatueAngle;

			if (g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_GUILDLEADERSTATUE, tmpStruc))
				g_UserMng.AddWorldCreateSfxObj(dwStatuesAppearSfx, tmpStruc.vPos.x, tmpStruc.vPos.y, tmpStruc.vPos.z, FALSE, tmpStruc.dwWorld);

			return;
		}
		else if (dwWhoIsThat == 2) // Mvp
		{
			tmpStruc.vPos = vMvpStatuePos;
			tmpStruc.dwWorld = dwMvpStatueWorld;
			tmpStruc.fAngle = fMvpStatueAngle;

			if (g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_MVPSTATUE, tmpStruc))
				g_UserMng.AddWorldCreateSfxObj(dwStatuesAppearSfx, tmpStruc.vPos.x, tmpStruc.vPos.y, tmpStruc.vPos.z, FALSE, tmpStruc.dwWorld);
		}
		else if (dwWhoIsThat == 3) // Leader
		{
			tmpStruc.vPos = vLeaderStatuePos;
			tmpStruc.dwWorld = dwLeaderStatueWorld;
			tmpStruc.fAngle = fLeaderStatueAngle;

			if (g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_GUILDLEADERSTATUE, tmpStruc))
				g_UserMng.AddWorldCreateSfxObj(dwStatuesAppearSfx, tmpStruc.vPos.x, tmpStruc.vPos.y, tmpStruc.vPos.z, FALSE, tmpStruc.dwWorld);
		}
	}
}
#endif
#ifdef __FL_EQUALIZED_MODE
void CDPDatabaseClient::SendSaveTmpPreset(CUser* pUser)
{
	BEFORESENDDUAL(ar, PACKETTYPE_SAVEEQUIPPRESET, DPID_UNKNOWN, DPID_UNKNOWN);
	DWORD dwSex = pUser->GetSex() == SEX_MALE ? 0 : 1;
	DWORD dwWorldID = pUser->GetWorld()->GetID();
	int nJob = pUser->m_nJob;

	ar << dwWorldID << dwSex << nJob;
	pUser->m_Inventory.Serialize(ar);
	SEND(ar, this, DPID_SERVERPLAYER);
}

void CDPDatabaseClient::SendGetTmpPresets()
{
	BEFORESENDDUAL(ar, PACKETTYPE_GETEQUIPPRESETS, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::OnGetTemporalPresets(CAr& ar, DPID, DPID)
{
	int nCount;
	ar >> nCount;

	for (auto& it : g_DPSrvr.m_vecTemporalConfig)
		SAFE_DELETE(it.m_InventoryPreset);

	g_DPSrvr.m_vecTemporalConfig.clear();

	for (int i = 0; i < nCount; i++)
	{
		DWORD       dwWorldID;
		DWORD       dwSex;
		DWORD       dwJob;

		ar >> dwWorldID >> dwSex >> dwJob;
		CItemContainer< CItemElem > tmpInventorySerialize;
		tmpInventorySerialize.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
		tmpInventorySerialize.Serialize(ar);

		TEMPORAL_PRESETS presets;
		presets.dwWorldID = dwWorldID;
		presets.dwSex = dwSex;
		presets.dwJob = dwJob;
		presets.m_InventoryPreset = new CItemContainer< CItemElem  >;
		presets.m_InventoryPreset->SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
		presets.m_InventoryPreset->Copy(tmpInventorySerialize);
		g_DPSrvr.m_vecTemporalConfig.push_back(presets);
	}
}
#endif

#ifdef __FL_DAILY_REWARDS
void CDPDatabaseClient::SendUpdateDailyGift(u_long idPlayer, int nValue)
{
	BEFORESENDDUAL(ar, PACKETTYPE_COLLECT_DAILY_GIFT, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar << nValue;
	SEND(ar, this, DPID_SERVERPLAYER);
}
#endif

#ifdef __FL_VENDORS
void CDPDatabaseClient::SendAddVendor(CMover* pMoverVendor)
{
	char szTitle[64];
	strcpy_s(szTitle, 64, pMoverVendor->m_vtInfo.GetTitle());
	szTitle[63] = 0;

	BEFORESENDDUAL(ar, PACKETTYPE_VENDOR_ADD, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pMoverVendor->GetNpcOwner() << pMoverVendor->GetWorld()->GetID() << pMoverVendor->GetPos() << pMoverVendor->GetAngle();
	ar << pMoverVendor->GetVendorTime();
	ar << pMoverVendor->GetVendorItemId();
	ar.WriteString(szTitle);
	pMoverVendor->m_Inventory.Serialize(ar);

	// Serialize prices
	int nItemsCount = 0;
	u_long uOffset = ar.GetOffset();
	ar << nItemsCount;
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemElem = pMoverVendor->m_Inventory.GetAtId(i);
		if (!pItemElem)
			continue;

		ar << (BYTE)i;
		ar << pItemElem->m_nCost;
		ar << pItemElem->m_nPerinCost;

		nItemsCount++;
	}

	int nBufSize1;
	LPBYTE lpBuf1 = ar.GetBuffer(&nBufSize1);
	*(UNALIGNED int*)(lpBuf1 + uOffset) = nItemsCount;

	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendUpdateVendor(CMover* pMoverVendor)
{
	BEFORESENDDUAL(ar, PACKETTYPE_VENDOR_UPDATE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << pMoverVendor->GetNpcOwner();
	pMoverVendor->m_Inventory.Serialize(ar);

	// Serialize prices
	int nItemsCount = 0;
	u_long uOffset = ar.GetOffset();
	ar << nItemsCount;
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemElem = pMoverVendor->m_Inventory.GetAtId(i);
		if (!pItemElem)
			continue;

		ar << (BYTE)i;
		ar << pItemElem->m_nCost;
		ar << pItemElem->m_nPerinCost;

		nItemsCount++;
	}

	int nBufSize1;
	LPBYTE lpBuf1 = ar.GetBuffer(&nBufSize1);
	*(UNALIGNED int*)(lpBuf1 + uOffset) = nItemsCount;

	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendDeleteVendor(u_long idPlayer)
{
	BEFORESENDDUAL(ar, PACKETTYPE_VENDOR_DELETE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::SendGetVendors()
{
	if (g_uIdofMulti != __FL_VENDORS)
		return;

	BEFORESENDDUAL(ar, PACKETTYPE_VENDORS_GET, DPID_UNKNOWN, DPID_UNKNOWN);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::OnGetVendors(CAr& ar, DPID, DPID)
{
	int nCount;
	ar >> nCount;

	for (int i = 0; i < nCount; i++)
	{
		// Vendor data
		u_long idPlayer;
		DWORD idWorld;
		D3DXVECTOR3 vPos;
		float fAngle;
		DWORD dwSkillTime;
		DWORD dwVendorItem;
		char szShopName[64];
		CItemContainer< CItemElem > tmpInventoryVendor;
		tmpInventoryVendor.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

		int nCost[MAX_VENDITEM] = { };
		int nPerinCost[MAX_VENDITEM] = { };

		ar >> idPlayer >> idWorld >> vPos >> fAngle >> dwSkillTime >> dwVendorItem;
		ar.ReadString(szShopName, 64);
		tmpInventoryVendor.Serialize(ar);

		// Serialize prices
		int nItemsCount;
		ar >> nItemsCount;

		for (int z = 0; z < nItemsCount; z++)
		{
			BYTE nIndex;
			ar >> nIndex;

			ar >> nCost[nIndex];
			ar >> nPerinCost[nIndex];
		}

		// Owner data
		DWORD dwSexIndex;
		DWORD dwHairId;
		DWORD dwHairColorId;
		DWORD dwHeadId;
		DWORD dwLevel;
		DWORD dwJob;
		char szPlayerName[64];
		CItemContainer< CItemElem > tmpInventoryOwner;
		tmpInventoryOwner.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ar >> dwSexIndex >> dwHairId >> dwHairColorId >> dwHeadId >> dwLevel >> dwJob;
		ar.ReadString(szPlayerName, 64);
		tmpInventoryOwner.Serialize(ar);

		ItemProp* pProp = prj.GetItemProp(dwVendorItem);
		if (!pProp || pProp->dwItemKind3 != IK3_VENDOR)
			continue;

		if (g_uIdofMulti != __FL_VENDORS)
			continue;

		auto it = prj.m_mapPlayerShops.find(idPlayer);
		if (it != prj.m_mapPlayerShops.end())
		{
			Error("User vendor already exist !!! [%d]", idPlayer);
			continue;
		}

		CMover* pMoverVendor = NULL;

		PLAYER_COPY tmpStruc;
		tmpStruc.idPlayer = idPlayer;
		tmpStruc.strName = szPlayerName;
		tmpStruc.nJob = dwJob;
		tmpStruc.nLevel = dwLevel;
		tmpStruc.dwHairId = dwHairId;
		tmpStruc.dwHairColor = dwHairColorId;
		tmpStruc.dwHeadId = dwHeadId;
		tmpStruc.vPos = vPos;
		tmpStruc.dwWorld = idWorld;
		tmpStruc.fAngle = fAngle;

		if (pProp->dwAbilityMin != NULL_ID)
		{
			MoverProp* pMoverProp = prj.GetMoverProp(pProp->dwAbilityMin);
			if (!pMoverProp)
				return;

			tmpStruc.dwIndex = pMoverProp->dwID;
			tmpStruc.tmpInventoryOwner = NULL;
			pMoverVendor = g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_VENDOR_NPC, tmpStruc);
		}
		else
		{
			tmpStruc.dwIndex = dwSexIndex == SEX_FEMALE ? MI_FEMALE : MI_MALE;
			tmpStruc.tmpInventoryOwner = &tmpInventoryOwner;
			pMoverVendor = g_GuildCombatMng.CreatePlayerCopy(NPC_TYPE_VENDOR_HUMAN, tmpStruc);
		}

		if (!pMoverVendor)
			return;

		pMoverVendor->SetVendorTime(dwSkillTime);
		pMoverVendor->SetVendorItemId(pProp->dwID);
		pMoverVendor->m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

		pMoverVendor->m_Inventory.Copy(tmpInventoryVendor);
		pMoverVendor->m_vtInfo.SetTitle(szShopName);

		for (int j = 0; j < MAX_VENDITEM; j++)
		{
			CItemElem* pItemElem = pMoverVendor->m_Inventory.GetAt(j);
			if (!pItemElem)
				continue;

			pMoverVendor->m_vtInfo.VendorSetItem((BYTE)j, (BYTE)j, pItemElem->m_nItemNum, nCost[j], nPerinCost[j]);
		}
#ifdef __FL_SHOP_SEARCH
		prj.AddToShopSearch(pMoverVendor->GetId());
#endif
	}
}
#endif

#ifdef __FL_CASH_SHOP
void CDPDatabaseClient::OnCShop(CAr& ar, DPID, DPID)
{
	CFlyFFShop::GetInstance()->SerializeFlyffShop(ar);

	map<DWORD, CUser*>::iterator it;
	for (it = g_UserMng.m_users.begin(); it != g_UserMng.m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (!pUser->IsValid())
			continue;

		pUser->AddCShop();
	}
}
void CDPDatabaseClient::SendGetCShopBalance(u_long idPlayer, char* lpszAccount)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CSHOP_BALANCE, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(lpszAccount);
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::OnCShopBalance(CAr& ar, DPID, DPID)
{
	u_long idPlayer;
	int nDonate, nVote;
	BOOL bPurchaseError;

	ar >> idPlayer >> nDonate >> nVote;
	ar >> bPurchaseError;

	CUser* pUser = (CUser*)prj.GetUserByID(idPlayer);
	if (!IsValidObj(pUser))
		return;

	pUser->AddCShopBalance(nDonate, nVote);

	if (bPurchaseError)
		pUser->AddText("Not enough money to purchase this item.", 0xffff0000);
}
void CDPDatabaseClient::SendBuyCShopItem(u_long idPlayer, char* lpszAccount, DWORD dwItemID, int nItemAmount, int nTotalPrice, BOOL bDonate, u_long idReceiver)
{
	BEFORESENDDUAL(ar, PACKETTYPE_CSHOP_ITEM, DPID_UNKNOWN, DPID_UNKNOWN);
	ar << idPlayer;
	ar.WriteString(lpszAccount);
	ar << dwItemID << nItemAmount << nTotalPrice << bDonate;
	ar << idReceiver;
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPDatabaseClient::OnCShopItem(CAr& ar, DPID, DPID)
{
	u_long idBuyer;
	u_long idReceiver;
	int nNewDonate;
	int nNewVote;
	DWORD dwItemID;
	int nItemNum;

	ar >> idBuyer >> idReceiver >> nNewDonate >> nNewVote >> dwItemID >> nItemNum;

	ItemProp* pProp = prj.GetItemProp(dwItemID);
	if (!pProp)
	{
		Error("CDPDatabaseClient::OnCShopItem: itemprop is null [%d]", dwItemID);
		return;
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = dwItemID;
	itemElem.m_nItemNum = nItemNum;
	itemElem.SetSerialNumber();

	// Send item to receiver
	CString str;
	BOOL bSendToPostBox = TRUE;

	CUser* pUserReceiver = (CUser*)prj.GetUserByID(idReceiver);
	if (IsValidObj(pUserReceiver))
	{
		if (pUserReceiver->CreateItem(&itemElem))
		{
			bSendToPostBox = FALSE;

			CWorld* pWorld = pUserReceiver->GetWorld();
			if (pWorld)
				g_dpDBClient.SavePlayer(pUserReceiver, pWorld->GetID(), pUserReceiver->GetPos(), pUserReceiver->GetLayer());
		}
	}

	if (bSendToPostBox)
	{
		int nTotalItems = nItemNum;
		int nMaxPossibleItems = (int)pProp->dwPackMax;

		while (nTotalItems >= nMaxPossibleItems) // Retarded postbox allowed to send more than dwPackMax per mail, but after restart mail will not be displayed, so here we go
		{
			itemElem.m_nItemNum = nMaxPossibleItems;
			g_dpDBClient.SendQueryPostMail(idReceiver, 0, itemElem, 0, "Cash Shop", "Item purchased in the cash shop.");
			nTotalItems -= nMaxPossibleItems;
		}

		if (nTotalItems > 0)
		{
			itemElem.m_nItemNum = nTotalItems;
			g_dpDBClient.SendQueryPostMail(idReceiver, 0, itemElem, 0, "Cash Shop", "Item purchased in the cash shop.");
		}
	}

	// Refresh buyer balance and send text
	CUser* pUserBuyer = (CUser*)prj.GetUserByID(idBuyer);
	if (IsValidObj(pUserBuyer))
	{
		pUserBuyer->AddCShopBalance(nNewDonate, nNewVote);

		if (idBuyer != idReceiver)
		{
			str.Format("You successfully purchased: #u#b#cff2fbe6d%s (x%d)#nc#nb#nu.", pProp->szName, nItemNum);
			pUserBuyer->AddText(str, 0xffffff00);
		}
	}
}
#endif
#ifdef __FL_RECORD_BOOK
void CDPDatabaseClient::OnRecordBook(CAr& ar, DPID, DPID)
{
	CRecordBook::GetInstance()->SerializeAllRecords(ar);
}
#endif