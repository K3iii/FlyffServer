#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

#ifdef __FL_RECORD_BOOK
#include "RecordParameters.h"
#endif

extern const int MAX_TASKBAR;
extern const int MAX_APPLETTASKBAR;
extern const int MAX_ITEMTASKBAR;
extern const char NullStr[2];

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDBOP ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

#ifndef __FL_DAILY_REWARDS
void CDbManager::SendJoin(CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey,
	LPCTSTR szPartyName, LPCTSTR szBankPass,
	DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel,
	LPCTSTR szTimeGuild)
#else
void CDbManager::SendJoin(CMover* pMover, LPDB_OVERLAPPED_PLUS lpDBOP, DWORD dwAuthKey,
	LPCTSTR szPartyName, LPCTSTR szBankPass,
	DWORD dwPlayTime, DWORD dwFlyTime, int nMaximumLevel,
	LPCTSTR szTimeGuild, BOOL bIsRewardTime, int nLastReward
#endif
#ifdef  __FL_CASH_SHOP
	, int nDonatePoints, int nVotePoints
#endif
)
{
	PLAY_ACCOUNT playAccount;
	_tcscpy(playAccount.lpszAccount, lpDBOP->AccountInfo.szAccount);
	GetLocalTime(&playAccount.m_stLogin);

	BEFORESENDDUAL(ar, PACKETTYPE_JOIN, lpDBOP->dpidCache, lpDBOP->dpidUser);
	ar << dwAuthKey;
	ar << playAccount;
	ar << pMover->m_dwWorldID;
#ifdef __LAYER_1015
	ar << pMover->GetLayer();
#endif	// __LAYER_1015
#ifdef __FL_BATTLE_PASS
	ar << pMover->GetLastBattlePassID();
#endif
#ifdef __FL_RECORD_BOOK
	for (int i = 0; i < MAX_RECORDTYPE; i++)
		pMover->m_arrayRecords[i].SerializeAll(ar);
#endif

	ar << pMover->m_dwIndex;
	pMover->Serialize(ar);
	pMover->m_UserTaskBar.Serialize(ar);
	pMover->m_RTMessenger.Serialize(ar);
	ar.WriteString(szPartyName);
	ar.WriteString(szBankPass);
	ar << dwPlayTime;
#ifndef __JEFF_9_20
	ar << dwFlyTime;
#endif	// __JEFF_9_20
	ar << nMaximumLevel;
	ar.WriteString(szTimeGuild);
	ar << pMover->m_dwReturnWorldID;
	ar << pMover->m_vReturnPos;

#ifdef __FL_DAILY_REWARDS
	ar << bIsRewardTime;
	ar << nLastReward;
#endif

#ifdef __FL_CASH_SHOP
	ar << nDonatePoints;
	ar << nVotePoints;
#endif

	SEND(ar, CDPTrans::GetInstance(), lpDBOP->dpid);

	char lpOutputString[128] = { 0, };
	sprintf(lpOutputString, "DATABASESERVER.EXE\t// JOIN_1\t// %d\n", lpDBOP->dpid);
	OutputDebugString(lpOutputString);
}

void CDbManager::Join(CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDBOP)
{
	BYTE	_nSlot;
	u_long	idPlayer;
	DWORD	dwAuthKey = 0;
	char	szBankPass[5] = { 0, };
	char	szPartyName[36] = { 0, };
	char	szTimeGuild[15] = { 0, };

	CAr arRead(lpDBOP->lpBuf, lpDBOP->uBufSize);
	arRead >> dwAuthKey;	// o
	arRead.ReadString(lpDBOP->AccountInfo.szAccount, MAX_ACCOUNT);
	arRead.ReadString(lpDBOP->AccountInfo.szPassword, MAX_PASSWORD);
	arRead >> _nSlot >> idPlayer;
	lpDBOP->AccountInfo.nPlayerSlot = _nSlot;

	arRead >> lpDBOP->dpid >> lpDBOP->dpidCache >> lpDBOP->dpidUser;

	char lpOutputString[128] = { 0, };
	sprintf(lpOutputString, "DATABASESERVER.EXE\t// JOIN_0\t// %d\n", lpDBOP->dpid);
	OutputDebugString(lpOutputString);

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidAccountName(lpDBOP->AccountInfo.szAccount) || IsInvalidPasswordName(lpDBOP->AccountInfo.szPassword))
	{
		CString str;
		str.Format("CDbManager::Join() err: %s - account, %s - password", lpDBOP->AccountInfo.szAccount, lpDBOP->AccountInfo.szPassword);
		Error(str);
		return;
	}
#endif

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "S8", idPlayer, g_appInfo.dwSys, lpDBOP->AccountInfo.szAccount);

	if (!qry->Exec(szQuery))
		return;

	if (!qry->Fetch())
		return;

	BYTE nSlot = (BYTE)qry->GetInt("playerslot");
	if (nSlot > 2)
		return;

	qry->GetStr("m_szName", lpDBOP->AccountInfo.szPlayer);
	m_joinLock.Enter();
	strcpy(m_aszJoin[m_nJoin], lpDBOP->AccountInfo.szPlayer);
	m_nJoin = (m_nJoin + 1) % MAX_JOIN_SIZE;
	m_joinLock.Leave();

	CMover	mover;
	mover.m_nSlot = nSlot;
	mover.m_dwIndex = (DWORD)qry->GetInt("m_dwIndex");
	mover.InitProp();
	mover.m_idPlayer = (u_long)qry->GetInt("m_idPlayer");

	BOOL bRefresh = FALSE;
	if (qry->GetInt("m_idCompany") > 0)
		bRefresh = TRUE;

	GetBaseCharacter(&mover, qry, lpDBOP);
#ifndef __FL_PACKED_ITEMS
	if (GetInventory(&mover, qry) == FALSE
#else
	if (GetInventory(&mover, qry, qry1) == FALSE
#endif
		|| GetTaskBar(&mover, qry, lpDBOP) == FALSE
		|| GetQuest(&mover, qry, lpDBOP) == FALSE
		|| GetSMMode(&mover, qry, lpDBOP) == FALSE
		|| GetSKillInfluence(&mover, qry, lpDBOP) == FALSE)
	{
		return;
	}

	mover.m_RTMessenger.SetState((DWORD)qry->GetInt("m_nMessengerState"));
	mover.m_dwReturnWorldID = qry->GetInt("m_dwReturnWorldID");
	mover.m_vReturnPos.x = qry->GetFloat("m_vReturnPos_x");
	mover.m_vReturnPos.y = qry->GetFloat("m_vReturnPos_y");
	mover.m_vReturnPos.z = qry->GetFloat("m_vReturnPos_z");

	DWORD dwPlayTime = qry->GetInt("TotalPlayTime");
#ifdef __JEFF_9_20
	mover.m_dwMute = qry->GetInt("m_dwFlyTime");
	DWORD dwFlyTime = 0;
#else	// __JEFF_9_20
	DWORD dwFlyTime = qry->GetInt("m_dwFlyTime");
#endif	// __JEFF_9_20
	int nMaximumLevel = qry->GetInt("m_nMaximumLevel");
	qry->GetStr("m_tGuildMember", szTimeGuild);

	char cAuth = qry->GetChar("m_chLoginAuthority");
	if (cAuth == 'S')
		cAuth = qry->GetChar("m_chAuthority");
	mover.m_dwAuthorization = (DWORD)cAuth;

	mover.m_idGuild = (u_long)qry->GetInt("m_idGuild");

	DWORD dwPetId = (DWORD)qry->GetInt("m_dwPetId");
	mover.SetPetId(dwPetId);

#ifdef __LAYER_1015
	mover.SetLayer(qry->GetInt("m_nLayer"));
#endif	// __LAYER_1015

	mover.m_fAngle = qry->GetFloat("m_fAngle");

#ifdef __FL_BATTLE_PASS
	mover.SetLastBattlePassID(qry->GetInt("m_nLastBattlePassID"));
	mover.SetBattlePassLevel(qry->GetInt("m_nBattlePassLevel"));
	mover.SetBattlePassExp(qry->GetInt("m_nBattlePassExp"));
#endif

#ifdef __FL_DAILY_REWARDS
	char szDailyRewardTime[9] = { 0, };
	qry->GetStr("m_tDailyReward", szDailyRewardTime);
	int nLastDailyReward = qry->GetInt("m_nLastDailyReward");

	BOOL bIsRewardTime = TRUE;
	if (szDailyRewardTime[0] != '0')
	{
		char cYear[5] = { 0, };
		char cMonth[3] = { 0, };
		char cDay[3] = { 0, };

		strncpy(cYear, szDailyRewardTime, 4);
		strncpy(cMonth, szDailyRewardTime + 4, 2);
		strncpy(cDay, szDailyRewardTime + 6, 2);

		CTime current = CTime::GetCurrentTime();
		CTime reward(atoi(cYear), atoi(cMonth), atoi(cDay), 0, 0, 0);

		COleDateTime CurrentTime;
		COleDateTime lastRewardTime;

		CurrentTime.SetDateTime(current.GetYear(), current.GetMonth(), current.GetDay(), 0, 0, 0);
		lastRewardTime.SetDateTime(reward.GetYear(), reward.GetMonth(), reward.GetDay(), 0, 0, 0);

		COleDateTimeSpan ts = CurrentTime - lastRewardTime;
		int nTotalDaysBetweenDates = static_cast<int>(ts.GetTotalDays());

		// If it's same day, then no reward
		if (nTotalDaysBetweenDates == 0)
		{
			bIsRewardTime = FALSE;
			nLastDailyReward = -1;
		}
		// If it's next day, then all fine
		else if (nTotalDaysBetweenDates == 1)
		{
			bIsRewardTime = TRUE;
		}
		// If 2 and more day's difference between dates, then reward, BUT wiped to first day
		//else if (nTotalDaysBetweenDates > 1)
		//{
		//	bIsRewardTime = TRUE;
		//	nLastDailyReward = 0;
		//}
	}
#endif

#ifdef __MODEL_HIDEFASH
	int nHide = qry->GetInt("m_nHideCoat");
	for (int z = 0; z < 6; ++z)
		mover.m_bHideFashion[z] = (nHide >> z) & 1;
#endif

#ifdef __FL_CASH_SHOP
	int nDonatePoints = qry->GetInt("cash");
	int nVotePoints = qry->GetInt("votepoint");
#endif

	if (!qry->MoreResults())
	{
		return;
	}
	while (qry->Fetch())
	{
		int nPlayerSlot = qry->GetInt("playerslot");
		if (nSlot == nPlayerSlot)
			qry->GetStr("m_BankPw", szBankPass);

#ifndef __FL_PACKED_ITEMS
		if (GetBank(&mover, qry, nPlayerSlot) == FALSE)
#else
		if (GetBank(&mover, qry, nPlayerSlot, qry1) == FALSE)
#endif
		{
			return;
		}
	}

	GetMyPartyName(qry, lpDBOP, mover.m_idPlayer, szPartyName);

	LoadMessenger(&mover, qry);

	GetHonor(&mover, qry, lpDBOP);

#ifdef __FL_CONSUMABLES_STORAGE
#ifndef __FL_PACKED_ITEMS
	if (!GetChest(&mover, qry))
#else
	if (!GetChest(&mover, qry, qry1))
#endif
	{
		return;
	}
#endif

#ifdef __FL_SWITCH_EQ
	GetSwitches(&mover, qry);
#endif
#ifdef __FL_RECORD_BOOK
	LoadRecords(&mover, qry);
#endif
#ifdef __FL_MONSTER_ALBUM
	LoadMonsterAlbum(&mover, qry);
#endif

	if (!GetSkill(&mover, qry, lpDBOP))
	{
		return;
	}

	m_AddRemoveLock.Enter();
	BOOL bCacheHit = FALSE;
	BOOL bExists = FALSE;
	ACCOUNT_CACHE* pCache = m_AccountCacheMgr.GetAccount(lpDBOP->AccountInfo.szAccount, &bCacheHit);
	if (bCacheHit)
	{
		if (pCache->IsPlaying())
			pCache->ChangeMultiServer(0);

		if (pCache->pMover[nSlot] == NULL)
			pCache->pMover[nSlot] = new CMover;
		else
			bExists = TRUE;
	}
	else
		pCache->pMover[nSlot] = new CMover;

	CMover* pMover = pCache->pMover[nSlot];

	if (memcmp(pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * 3))
	{
		bRefresh = TRUE;
	}
	if (dwPlayTime == 0 || pMover->m_idPlayer != mover.m_idPlayer)
		bRefresh = TRUE;

	if (!bCacheHit || bRefresh)
	{
		for (int j = 0; j < 3; j++)
			pCache->m_Bank[j].Copy(mover.m_Bank[j]);
		memcpy(pCache->m_idPlayerBank, mover.m_idPlayerBank, sizeof(u_long) * 3);
		memcpy(pCache->m_dwGoldBank, mover.m_dwGoldBank, sizeof(DWORD) * 3);
	}

	pMover->Lock();


	pMover->Copy(&mover, !bExists || bRefresh);

	for (int j = 0; j < 3; j++)
		pMover->m_Bank[j].Copy(pCache->m_Bank[j]);
	memcpy(pMover->m_idPlayerBank, pCache->m_idPlayerBank, sizeof(u_long) * 3);
	memcpy(pMover->m_dwGoldBank, pCache->m_dwGoldBank, sizeof(DWORD) * 3);

	pMover->PeriodTick();

	mover.Copy(pMover, TRUE);
	pMover->Unlock();

	m_AccountCacheMgr.AddAccount(pMover->m_idPlayer, lpDBOP->AccountInfo.szAccount, bCacheHit, pCache);
	m_AddRemoveLock.Leave();

	BOOL bResult = FALSE;
	bResult = SendItemtoCharacter(nSlot, &mover, qry, qry1, qrylog, lpDBOP);
	bResult = RemoveItemtoCharacter(&mover, qry, qry1, qrylog, lpDBOP) || bResult;
	bResult = mover.RemoveItemIK3() || bResult;

	if (bResult)
	{
		m_AddRemoveLock.Enter();
		pCache = m_AccountCacheMgr.Find(lpDBOP->AccountInfo.szAccount);
		if (pCache)
		{
			pMover = pCache->pMover[nSlot];
			if (pMover)
			{
				pMover->Lock();
				pMover->Copy(&mover, TRUE);
				pMover->Unlock();
				OutputDebugString("bResult: 1");
			}
		}
		m_AddRemoveLock.Leave();
	}

	PutExpLog(qrylog, &mover, 'I');

	SendJoin(&mover, lpDBOP, dwAuthKey, szPartyName, szBankPass, dwPlayTime, dwFlyTime, nMaximumLevel, szTimeGuild
#ifdef __FL_DAILY_REWARDS
		, bIsRewardTime, nLastDailyReward
#endif
#ifdef __FL_CASH_SHOP
		, nDonatePoints, nVotePoints
#endif
	);
	g_dpCoreSrvr.SendMemberTime(mover.m_idPlayer, szTimeGuild);
}