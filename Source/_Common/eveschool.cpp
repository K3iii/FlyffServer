#include "stdafx.h"

#include "eveschool.h"
#ifdef __WORLDSERVER
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "guild.h"
#include "dpdatabaseclient.h"
#include "user.h"
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CGuildMng	g_GuildMng;

extern	CDPDatabaseClient	g_dpDBClient;

extern	CUserMng	g_UserMng;

#include "WorldMng.h"
extern CWorldMng g_WorldMng;

#include "definetext.h"
#include "defineobj.h"
#include "definesound.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifdef __FL_BATTLE_PASS
#if defined (__WORLDSERVER) || defined (__CLIENT)
#include "BattlePass.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif
#endif

#endif // __WORLDSERVER

CGuildCombat::CGuildCombat()
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_nGuildCombatIndex = 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
	m_vecGCGetPoint.clear();
	m_vecGCPlayerPoint.clear();
#ifdef __WORLDSERVER
	m_vecGuildCombatMem.clear();
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	m_nJoinPanya = 0;
	m_nGuildLevel = 0;
	m_nMaxJoinMember = 0;
	m_nMaxPlayerLife = 0;
	m_nMaxWarPlayer = 0;
	m_nMaxPlayerPercent = 0;
	m_nMaxGuildPercent = 0;
	m_nRequestCanclePercent = 0;
	m_nNotRequestPercent = 0;
	m_nItemPenya = 0;
	m_bMutex = FALSE;
	m_bMutexMsg = FALSE;

	memset(__AutoOpen, 0, sizeof(__AUTO_OPEN) * 7);

	m_nDay = 0;
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

CGuildCombat::~CGuildCombat()
{
	GuildCombatClear(99);
}

void CGuildCombat::GuildCombatClear(int nState)
{
	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;

	if (nState == 99 || nState == 1)
	{
		m_nGuildCombatIndex = 0;
		m_uWinGuildId = 0;
		m_nWinGuildCount = 0;
		m_uBestPlayer = 0;
	}

	m_vecGCGetPoint.clear();
#ifdef __WORLDSERVER
	for (int gcmi = 0; gcmi < (int)(m_vecGuildCombatMem.size()); ++gcmi)
		SAFE_DELETE(m_vecGuildCombatMem[gcmi]);

	m_vecGuildCombatMem.clear();
	vecRequestRanking.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	if (g_eLocal.GetState(EVE_GUILDCOMBAT))
		g_DPCoreClient.SendGuildCombatState(CLOSE_STATE);

	if (nState == 99 || nState == 3)
	{
		m_nJoinPanya = 0;
		m_nGuildLevel = 0;
		m_nMaxJoinMember = 0;
		m_nMaxPlayerLife = 0;
		m_nMaxWarPlayer = 0;
		m_nMaxPlayerPercent = 0;
		m_nMaxGuildPercent = 0;
		m_nRequestCanclePercent = 0;
		m_nNotRequestPercent = 0;
		m_nItemPenya = 0;
	}
#endif // __WORLDSERVER
#ifdef __CLIENT
	m_bRequest = FALSE;
#endif // __CLIENT
}

void CGuildCombat::GuildCombatGameClear()
{
	GuildCombatClear(2);
}

void CGuildCombat::AddvecGCGetPoint(u_long uidGuildAttack, u_long uidGuildDefence, u_long uidPlayerAttack, u_long uidPlayerDefence, int nPoint,
	BOOL bKillDiffernceGuild, BOOL bMaster, BOOL bDefender, BOOL bLastLife)
{
	__GCGETPOINT GCGetPoint;
	GCGetPoint.uidGuildAttack = uidGuildAttack;
	GCGetPoint.uidGuildDefence = uidGuildDefence;
	GCGetPoint.uidPlayerAttack = uidPlayerAttack;
	GCGetPoint.uidPlayerDefence = uidPlayerDefence;
	GCGetPoint.nPoint = nPoint;
	GCGetPoint.bKillDiffernceGuild = bKillDiffernceGuild;
	GCGetPoint.bMaster = bMaster;
	GCGetPoint.bDefender = bDefender;
	GCGetPoint.bLastLife = bLastLife;
	m_vecGCGetPoint.push_back(GCGetPoint);
#ifdef __WORLDSERVER
	g_UserMng.AddGCLogRealTimeWorld(GCGetPoint);
#endif // __WORLDSERVER
}

void CGuildCombat::AddvecGCPlayerPoint(u_long uidPlayer, int nJob, int nPoint)
{
	__GCPLAYERPOINT GCPlayerPoint;
	GCPlayerPoint.uidPlayer = uidPlayer;
	GCPlayerPoint.nJob = nJob;
	GCPlayerPoint.nPoint = nPoint;
	m_vecGCPlayerPoint.push_back(GCPlayerPoint);
}

#ifdef __WORLDSERVER
void CGuildCombat::SelectPlayerClear(u_long uidGuild)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(uidGuild);
	if (pGCMember != NULL)
		pGCMember->SelectMemberClear();
}

void CGuildCombat::JoinGuildCombat(u_long idGuild, DWORD dwPenya, BOOL bRequest)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(idGuild);
	if (pGCMember != NULL)
	{
		pGCMember->dwPenya = dwPenya;
		pGCMember->bRequest = bRequest;
	}
	else
	{
		pGCMember = new __GuildCombatMember;
		pGCMember->Clear();
		pGCMember->uGuildId = idGuild;
		pGCMember->dwPenya = dwPenya;
		pGCMember->bRequest = bRequest;
		m_vecGuildCombatMem.push_back(pGCMember);
	}

	SetRequestRanking();
}

void CGuildCombat::OutGuildCombat(u_long idGuild, DWORD dwChannel)
{
	if (!g_eLocal.GetState(EVE_GUILDCOMBAT))
		return;

	__GuildCombatMember* pGCMember = FindGuildCombatMember(idGuild);
	if (pGCMember != NULL)
	{
		CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
		if (pGuild && pGCMember->bRequest && (g_uIdofMulti == dwChannel))
		{
			CItemElem itemElem;
			char szMsg[1000];
			sprintf(szMsg, "%s", prj.GetText(TID_GAME_GC_CANCELREQUEST));
			char szMsg1[1000];
			sprintf(szMsg1, "%s", prj.GetText(TID_GAME_GC_CANCELREQUEST1));
			g_dpDBClient.SendQueryPostMail(pGuild->m_idMaster, 0, itemElem, MulDiv(pGCMember->dwPenya, m_nRequestCanclePercent, 100), szMsg, szMsg1);
		}
		pGCMember->bRequest = FALSE;
		pGCMember->dwPenya = 0;
	}
	SetRequestRanking();
}


void CGuildCombat::AddSelectPlayer(u_long idGuild, u_long uidPlayer)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(idGuild);
	if (pGCMember != NULL)
	{
		__JOINPLAYER* pJoinPlayer = new __JOINPLAYER;
		pJoinPlayer->nlife = m_nMaxPlayerLife;
		pJoinPlayer->uidPlayer = uidPlayer;
		pJoinPlayer->nPoint = 0;
		pJoinPlayer->uKillidGuild = 0;
		pJoinPlayer->dwTelTime = 0;
		pGCMember->vecGCSelectMember.push_back(pJoinPlayer);
	}
}
void CGuildCombat::GetSelectPlayer(u_long idGuild, vector<__JOINPLAYER>& vecSelectPlayer)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(idGuild);
	if (pGCMember != NULL)
	{
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			vecSelectPlayer.push_back(*pJoinPlayer);
		}
	}
}

void CGuildCombat::JoinWar(CUser* pUser, BOOL bWar)
{
	if (m_nState == WAR_STATE)
	{
		if (m_nGCState == WAR_WAR_STATE)
			pUser->m_nGuildCombatState = 1;
		else if (m_nGCState == MAINTENANCE_STATE)
			pUser->m_nGuildCombatState = 2;

		if (m_nGCState == WAR_WAR_STATE || m_nGCState == MAINTENANCE_STATE)
			g_UserMng.AddGuildCombatUserState(pUser);
	}

	int nRandomX = 240 + rand() % (300 - 240);
	int nRandomZ = 230 + rand() % (280 - 230);
	((CMover*)pUser)->REPLACE(g_uIdofMulti, WI_WORLD_GUILDWAR, D3DXVECTOR3(static_cast<float>(nRandomX), 85.0f, static_cast<float>(nRandomZ)), REPLACE_NORMAL, nDefaultLayer);
	pUser->m_vtInfo.SetOther(NULL);
}

void CGuildCombat::OutWar(CUser* pUser, CUser* pLeader, BOOL bLogOut)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(pUser->m_idGuild);
	if (pGCMember != NULL)
	{
		SetPlayerChange(pUser, pLeader);

		if (bLogOut)
		{
			if (pUser->m_idPlayer == pGCMember->m_uidDefender)
			{
				CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
				if (pGuild)
				{
					vector<int> vecDefender;
					vecDefender.clear();
					for (int vecii0 = 0; vecii0 < (int)(pGCMember->vecGCSelectMember.size()); ++vecii0)
					{
						__JOINPLAYER* pJoinPlayer3 = pGCMember->vecGCSelectMember.at(vecii0);
						CMover* pMover = prj.GetUserByID(pJoinPlayer3->uidPlayer);
						if (IsValidObj(pMover))
						{
							if (pGuild->IsMaster(pMover->m_idPlayer) == FALSE && pGCMember->m_uidDefender != pMover->m_idPlayer && 0 < pJoinPlayer3->nlife)
							{
								vecDefender.push_back(vecii0);
							}
						}
					}
					if (0 < vecDefender.size())
					{
						int nPlayerPos = xRandom(vecDefender.size());
						int nDefender = vecDefender[nPlayerPos];
						__JOINPLAYER* pJoinPlayer4 = pGCMember->vecGCSelectMember.at(nDefender);
						pGCMember->m_uidDefender = pJoinPlayer4->uidPlayer;
					}
				}
			}
		}	// if( bLogOut )
	}
	g_UserMng.AddGCGuildStatus(pUser->m_idGuild);
	g_UserMng.AddGCWarPlayerlist();


	UserOutGuildCombatResult(pUser);
}
void CGuildCombat::JoinObserver(CUser* pUser, BOOL bMessage)
{
	if (pUser->IsBaseJob())
	{
		pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_WAR_ENTER));
		return;
	}

	pUser->RemoveCommonBuffs();
	g_UserMng.AddRemoveAllSkillInfluence(pUser);
	((CMover*)pUser)->REPLACE(g_uIdofMulti, WI_WORLD_GUILDWAR, D3DXVECTOR3(340.6f, 108.0f, 253.3f), REPLACE_NORMAL, nDefaultLayer);
	pUser->m_vtInfo.SetOther(NULL);

	if (bMessage)
	{
		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME));
		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_ZOOM_USE));
	}

	if (m_nGCState <= ENTER_STATE)
		pUser->AddGCWarPlayerlist();

	if (IsSelectPlayer(pUser))
	{
		pUser->m_nGuildCombatState = 2;
		g_UserMng.AddGuildCombatUserState((CMover*)pUser);
		if (ENTER_STATE <= m_nGCState && m_nGCState <= WAR_CLOSE_STATE)
			g_DPCoreClient.SendGCRemoveParty(pUser->GetPartyId(), pUser->m_idPlayer);
	}
	g_UserMng.AddGCGuildStatus(pUser->m_idGuild, pUser);
	g_UserMng.AddGCGuildPrecedence(pUser);
	g_UserMng.AddGCPlayerPrecedence(pUser);
}

void CGuildCombat::GuildCombatRequest(CUser* pUser, DWORD dwPenya)
{
	CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	if (pGuild && pGuild->IsMaster(pUser->m_idPlayer))
	{
		if (m_nState != CGuildCombat::CLOSE_STATE)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_REQUESTEND));
			return;
		}

		if (pGuild->m_nLevel < m_nGuildLevel)
		{
			CString strMsg;
			strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_REQUEST_LEVEL), m_nGuildLevel);
			pUser->AddDiagText(strMsg);
			return;
		}

		if (strlen(pGuild->m_szGuild) < 1)
		{
			pUser->AddDefinedText(TID_GAME_NAME_PET_E01);
			return;
		}

		DWORD dwExistingPenya = 0;
		__GuildCombatMember* pGCMember = FindGuildCombatMember(pUser->m_idGuild);
		if (pGCMember != NULL)
			dwExistingPenya = pGCMember->dwPenya;

		if (dwPenya > INT_MAX)
			return;

		if ((int)(dwPenya) < m_nJoinPanya)
		{
			CString strMsg;
			strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_REQUEST_BASEPENYA), m_nJoinPanya);
			pUser->AddDiagText(strMsg);
			pUser->AddText(prj.GetText(TID_GAME_LACKMONEY));
			return;
		}

		if (dwPenya <= dwExistingPenya)
		{
			CString strMsg;
			strMsg.Format(prj.GetText(TID_GAME_GUILDCOMBAT_REQUEST_PENYA), dwPenya);
			pUser->AddDiagText(strMsg);
			return;
		}

		if (pUser->CheckUserGold((int)((dwPenya - dwExistingPenya)), false) == false)
		{
			pUser->AddDiagText(prj.GetText(TID_GAME_GUILDCOMBAT_REQUEST_NOTPENYA));
			return;
		}

#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "9";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "GUILDCOMBAT";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold();
		aLogItem.Gold2 = pUser->GetGold() - (dwPenya - dwExistingPenya);
		//aLogItem.ItemName = "SEED";
		_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
		aLogItem.itemNumber = dwPenya - dwExistingPenya;
		g_DPSrvr.OnLogItem(aLogItem);
#endif

		pUser->AddGold((int)((-1) * (int)(dwPenya - dwExistingPenya)));
		g_dpDBClient.SendInGuildCombat(pUser->m_idGuild, dwPenya, dwExistingPenya);
	}
	else
	{

		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER));
	}
}

void CGuildCombat::GuildCombatCancel(CUser* pUser)
{
	CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	if (pGuild && pGuild->IsMaster(pUser->m_idPlayer))
	{
		__GuildCombatMember* pGCMember = FindGuildCombatMember(pUser->m_idGuild);
		if (pGCMember != NULL)
		{
			if (pGCMember->bRequest)
			{
				if (m_nState != CGuildCombat::CLOSE_STATE)
				{
					pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));
					return;
				}
				g_dpDBClient.SendOutGuildCombat(pUser->m_idGuild);
			}
			else
			{
				pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP));
			}
		}
		else
		{

			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP));
		}
	}
	else
	{

		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER));
	}
}

void CGuildCombat::UserOutGuildCombatResult(CUser* pUser)
{
	int nCount = 0;

	__GuildCombatMember* pGCMember = FindGuildCombatMember(pUser->m_idGuild);
	if (pGCMember != NULL)
	{
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];

			if (0 < pJoinPlayer->nlife)
				return;
		}


		for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
		{
			if (nVeci >= m_nMaxGuild)
				break;

			__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
			__GuildCombatMember* pGCMember2 = FindGuildCombatMember(RequestGuild.uidGuild);
			if (pGCMember2 != NULL)
			{
				if (pUser->m_idGuild != pGCMember2->uGuildId)
				{
					BOOL bLive = FALSE;
					for (int veci2 = 0; veci2 < (int)(pGCMember2->vecGCSelectMember.size()); ++veci2)
					{
						__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[veci2];
						if (0 < pJoinPlayer->nlife)
							bLive = TRUE;
					}
					if (bLive)
					{
						++nCount;
						if (nCount == 2)
						{
							return;
						}
					}
				}
			}
		}

		if (nCount == 1)
			m_nStopWar = 1;
	}
}

void CGuildCombat::GuildCombatResult(BOOL nResult, u_long idGuildWin)
{
	vector<u_long> vecSameidGuildWin;
	vecSameidGuildWin.clear();
	int nMaxPoint = -1;


	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			int nPoint = 0;
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				nPoint += pJoinPlayer->nPoint + pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint = nPoint;

			if (nMaxPoint < nPoint)
			{
				nMaxPoint = nPoint;
				vecSameidGuildWin.clear();
				vecSameidGuildWin.push_back(pGCMember->uGuildId);
			}
			else if (nMaxPoint == nPoint)
			{
				vecSameidGuildWin.push_back(pGCMember->uGuildId);
			}
		}
	}


	int nMaxLife = 0;
	if (1 < vecSameidGuildWin.size())
	{
		vector<u_long> vecSameidGuildWinTmp;
		vecSameidGuildWinTmp.clear();
		for (int vecSame = 0; vecSame < (int)(vecSameidGuildWin.size()); ++vecSame)
		{
			int nLife = 0;

			__GuildCombatMember* pGCMember = FindGuildCombatMember(vecSameidGuildWin[vecSame]);
			if (pGCMember != NULL)
			{
				for (int veci2 = 0; veci2 < (int)(pGCMember->vecGCSelectMember.size()); ++veci2)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					nLife += pJoinPlayer->nlife;
				}


				if (nMaxLife < nLife)
				{
					nMaxLife = nLife;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back(pGCMember->uGuildId);
				}
				else if (nMaxLife == nLife)
				{
					vecSameidGuildWinTmp.push_back(pGCMember->uGuildId);
				}
			}
		}
		vecSameidGuildWin = vecSameidGuildWinTmp;
	}


	float fMaxAvgLv = 0.0f;
	if (1 < vecSameidGuildWin.size())
	{
		vector<u_long> vecSameidGuildWinTmp;
		vecSameidGuildWinTmp.clear();
		for (int vecSame = 0; vecSame < (int)(vecSameidGuildWin.size()); ++vecSame)
		{
			float fAvgLv = 0.0f;
			int nSubLevel = 0;
			int nValidObjCount = 0;

			__GuildCombatMember* pGCMember = FindGuildCombatMember(vecSameidGuildWin[vecSame]);
			if (pGCMember != NULL)
			{
				for (int veci2 = 0; veci2 < (int)(pGCMember->vecGCSelectMember.size()); ++veci2)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci2];
					if (0 < pJoinPlayer->nlife)
					{
						CUser* pUsertmp = (CUser*)prj.GetUserByID(pJoinPlayer->uidPlayer);
						if (IsValidObj(pUsertmp))
						{
							nSubLevel += pUsertmp->GetLevel();
							++nValidObjCount;
						}
					}
				}

				if (nValidObjCount)
				{
					fAvgLv = (float)(nSubLevel / nValidObjCount);
				}


				if (fMaxAvgLv < fAvgLv)
				{
					fMaxAvgLv = fAvgLv;
					vecSameidGuildWinTmp.clear();
					vecSameidGuildWinTmp.push_back(pGCMember->uGuildId);
				}
				else if (fMaxAvgLv == fAvgLv)
				{
					vecSameidGuildWinTmp.push_back(pGCMember->uGuildId);
				}
			}
		}
		vecSameidGuildWin = vecSameidGuildWinTmp;
	}

	char str[512] = { 0, };
	CString strPrizeMsg;


	if (0 < vecSameidGuildWin.size())
	{
		CGuild* pGuild = g_GuildMng.GetGuild(vecSameidGuildWin[0]);
		if (pGuild)
		{
			if (g_eLocal.GetState(EVE_GUILDCOMBAT))
			{
				g_UserMng.AddWorldCreateSfxObj(XI_NAT_ROCKET02, 263.3f, 85.0f, 253.0f, FALSE, WI_WORLD_GUILDWAR);
				g_DPCoreClient.SendPlayMusic(WI_WORLD_GUILDWAR, BGM_IN_FITUP);
			}


			if (m_uWinGuildId == vecSameidGuildWin[0])
				++m_nWinGuildCount;
			else
				m_nWinGuildCount = 1;

			m_uWinGuildId = vecSameidGuildWin[0];
			sprintf(str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER), pGuild->m_szGuild);
			g_DPCoreClient.SendSystem(str);
		}
		u_long uBestPlayerGuild;
		int nGetPoint;
		m_uBestPlayer = GetBestPlayer(&uBestPlayerGuild, &nGetPoint);

		++m_nGuildCombatIndex;
		g_UserMng.AddGCWinGuild();
		g_UserMng.AddGCBestPlayer();

		g_UserMng.AddGCLogWorld();

#ifdef __FL_GW_STATUE_V2
		// Send request to database to update statues on all channels when GW is done
		CGuild* pWinGuild = g_GuildMng.GetGuild(m_uWinGuildId);
		if (pWinGuild)
			g_dpDBClient.SendRequestToInstallStatues(m_uBestPlayer, pWinGuild->m_idMaster);
#endif

#ifdef __FL_RECORD_BOOK
		__GuildCombatMember* pGCMember = FindGuildCombatMember(m_uWinGuildId);
		if (pGCMember != NULL)
		{
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pMover))
					CRecordBook::GetInstance()->AddPlayerRecord(pMover, RecordType::GW_WIN, 1, true);
			}
		}

		CUser* pUsertmp = (CUser*)prj.GetUserByID(m_uBestPlayer);
		if (IsValidObj(pUsertmp))
			CRecordBook::GetInstance()->AddPlayerRecord(pUsertmp, RecordType::MVP, 1, true);
#endif
	}
}

#if __VER >= 11 // __GUILDCOMBATCHIP
void CGuildCombat::GuildCombatResultRanking()
{
	vector<__REQUESTGUILD> vecGCRanking;
	vecGCRanking = vecRequestRanking;

	for (int i = 0; i < (int)(vecGCRanking.size() - 1); i++)
	{
		if (i >= m_nMaxGuild)
			break;

		for (int j = 0; j < (int)(vecGCRanking.size() - 1 - i); j++)
		{
			__REQUESTGUILD RequestGuild1 = vecGCRanking[j];
			__GuildCombatMember* pGCMember1 = FindGuildCombatMember(RequestGuild1.uidGuild);
			__REQUESTGUILD RequestGuild2 = vecGCRanking[j + 1];
			__GuildCombatMember* pGCMember2 = FindGuildCombatMember(RequestGuild2.uidGuild);

			if (!pGCMember1 || !pGCMember2)
			{
				Error("GuildCombatResultRanking() - pGCMember1 or pGCMember2 is NULL");
				return;
			}


			if (pGCMember1->nGuildPoint < pGCMember2->nGuildPoint)
			{
				__REQUESTGUILD temp = vecGCRanking[j];
				vecGCRanking[j] = vecGCRanking[j + 1];
				vecGCRanking[j + 1] = temp;
			}

			else if (pGCMember1->nGuildPoint == pGCMember2->nGuildPoint)
			{
				int nLife1 = 0, nLife2 = 0;
				for (int k = 0; k < (int)(pGCMember1->vecGCSelectMember.size()); k++)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember1->vecGCSelectMember[k];
					if (pJoinPlayer)
						nLife1 += pJoinPlayer->nlife;
				}
				for (int k = 0; k < (int)(pGCMember2->vecGCSelectMember.size()); k++)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[k];
					if (pJoinPlayer)
						nLife2 += pJoinPlayer->nlife;
				}

				if (nLife1 < nLife2)
				{
					__REQUESTGUILD temp = vecGCRanking[j];
					vecGCRanking[j] = vecGCRanking[j + 1];
					vecGCRanking[j + 1] = temp;
				}

				else if (nLife1 == nLife2)
				{
					int nLevel = 0, nCount = 0;
					float fAvg1 = 0, fAvg2 = 0;
					for (int k = 0; k < (int)(pGCMember1->vecGCSelectMember.size()); k++)
					{
						__JOINPLAYER* pJoinPlayer = pGCMember1->vecGCSelectMember[k];
						if (pJoinPlayer && 0 < pJoinPlayer->nlife)
						{
							CUser* pUsertmp = (CUser*)prj.GetUserByID(pJoinPlayer->uidPlayer);
							if (IsValidObj(pUsertmp))
							{
								nLevel += pUsertmp->GetLevel();
								nCount++;
							}
						}
					}
					if (nCount)
						fAvg1 = (float)(nLevel / nCount);
					else
						fAvg1 = 1.0f;
					nLevel = nCount = 0;
					for (int k = 0; k < (int)(pGCMember2->vecGCSelectMember.size()); k++)
					{
						__JOINPLAYER* pJoinPlayer = pGCMember2->vecGCSelectMember[k];
						if (pJoinPlayer && 0 < pJoinPlayer->nlife)
						{
							CUser* pUsertmp = (CUser*)prj.GetUserByID(pJoinPlayer->uidPlayer);
							if (IsValidObj(pUsertmp))
							{
								nLevel += pUsertmp->GetLevel();
								nCount++;
							}
						}
					}
					if (nCount)
						fAvg2 = (float)(nLevel / nCount);
					else
						fAvg2 = 1.0f;

					if (fAvg1 < fAvg2)
					{
						__REQUESTGUILD temp = vecGCRanking[j];
						vecGCRanking[j] = vecGCRanking[j + 1];
						vecGCRanking[j + 1] = temp;
					}
				}
			}
		} //for j
	} //for i


	for (int i = 0; i < (int)(vecGCRanking.size()); i++)
	{
		if (i >= 3) break;

		__REQUESTGUILD RequestGuild = vecGCRanking[i];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (!pGCMember)
		{
			Error("GuildCombatResultRanking() - pGCMember is NULL");
			continue;
		}

		CItemElem itemElem;
		itemElem.m_dwItemId = II_CHP_RED;
		float fChipNum = m_nJoinPanya * vecGCRanking.size() * 0.9f * 0.00001f * 0.1f;
		switch (i)
		{
		case 0:
			fChipNum *= 0.7f;
			break;
		case 1:
			fChipNum *= 0.2f;
			break;
		case 2:
			fChipNum *= 0.1f;
			break;
		}
		itemElem.m_nItemNum = (int)fChipNum;
		if (itemElem.m_nItemNum < 1)
			itemElem.m_nItemNum = 1;

		for (int j = 0; j < (int)(pGCMember->vecGCSelectMember.size()); j++)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[j];
			if (!pJoinPlayer)
			{
				Error("GuildCombatResultRanking() - pJoinPlayer is NULL");
				continue;
			}

			CUser* pUsertmp = (CUser*)prj.GetUserByID(pJoinPlayer->uidPlayer);
			if (IsValidObj(pUsertmp))
			{
				itemElem.SetSerialNumber();
				if (pUsertmp->CreateItem(&itemElem))
					pUsertmp->AddDefinedText(TID_GAME_GUILDCOMBAT1TO1_WARGUILDWINCHIP, "%d", itemElem.m_nItemNum);
				else
					g_dpDBClient.SendQueryPostMail(pUsertmp->m_idPlayer, 0, itemElem, 0, itemElem.GetProp()->szName, itemElem.GetProp()->szName);
			}
		}
	}


#ifdef __FL_BATTLE_PASS
	__GuildCombatMember* pGCMember = FindGuildCombatMember(m_uWinGuildId);
	if (pGCMember != NULL)
	{
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
			if (IsValidObj(pMover))
				CBattlePass::GetInstance()->AddProgress((CUser*)pMover, EXP_GW);
		}
	}

	CUser* pUsertmp = (CUser*)prj.GetUserByID(m_uBestPlayer);
	if (IsValidObj(pUsertmp))
		CBattlePass::GetInstance()->AddProgress(pUsertmp, EXP_GW_MVP);
#endif

}
#endif // __GUILDCOMBATCHIP

void CGuildCombat::GuildCombatCloseTeleport()
{
	if (m_nStopWar != 200)
	{
		g_dpDBClient.SendGuildCombatResult();
	}

	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(WI_WORLD_MADRIGAL, "flaris");
	if (NULL == pRgnElem)
		return;

	g_UserMng.ReplaceWorld(WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f, nDefaultLayer);

	for (int i = 0; i < (int)(m_vecstrGuildMsg.size()); ++i)
		g_UserMng.AddGuildMsg(m_uWinGuildId, m_vecstrGuildMsg[i]);

	m_nState = CLOSE_STATE;
	m_nGCState = WAR_CLOSE_STATE;
	m_dwTime = 0;
	m_nStopWar = 0;
	if (g_eLocal.GetState(EVE_GUILDCOMBAT))
		g_DPCoreClient.SendGuildCombatState(CLOSE_STATE);

	m_bMutex = FALSE;

}
void CGuildCombat::GuildCombatOpen(void)
{
	if (m_nState != CLOSE_STATE)
		return;

	m_vecstrGuildMsg.clear();
	if (g_eLocal.GetState(EVE_GUILDCOMBAT))
		g_DPCoreClient.SendGuildCombatState(OPEN_STATE);

	m_vecGCGetPoint.clear();
	g_dpDBClient.SendGuildCombatStart();
	m_nGCState = NOTENTER_STATE;
	m_nProcessGo = 0;
	CString strOK;
	CString strCancle;
	strOK.Format("%s", prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_OK));
	strCancle.Format("%s", prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_CANCLE));

	for (int veci = 0; veci < (int)(vecRequestRanking.size()); ++veci)
	{
		__REQUESTGUILD RequestGuild = vecRequestRanking[veci];

		CGuild* pGuild = g_GuildMng.GetGuild(RequestGuild.uidGuild);
		if (pGuild)
		{
			for (map<u_long, CGuildMember*>::iterator Guildi = pGuild->m_mapPMember.begin();	// neuz
				Guildi != pGuild->m_mapPMember.end(); ++Guildi)
			{
				CGuildMember* pGuildMember = Guildi->second;
				if (pGuildMember->m_nMemberLv == GUD_MASTER || pGuildMember->m_nMemberLv == GUD_KINGPIN)
				{
					CUser* pUser = (CUser*)prj.GetUserByID(pGuildMember->m_idPlayer);
					if (IsValidObj(pUser))
					{
						if (veci < m_nMaxGuild)
							pUser->AddGCDiagMessage(strOK);
						else
							pUser->AddGCDiagMessage(strCancle);
					}
				}
			}
		}
	}
}
void CGuildCombat::SetRequestRanking(void)
{
	vecRequestRanking.clear();
	for (int gcmi = 0; gcmi < (int)(m_vecGuildCombatMem.size()); ++gcmi)
	{
		__GuildCombatMember* pGCMember = m_vecGuildCombatMem[gcmi];

		if (pGCMember->bRequest == FALSE)
			continue;

		__REQUESTGUILD RequstGuild;
		RequstGuild.dwPenya = pGCMember->dwPenya;
		RequstGuild.uidGuild = pGCMember->uGuildId;
		if (vecRequestRanking.empty() == true)
		{
			vecRequestRanking.push_back(RequstGuild);
		}
		else
		{
			BOOL bInsert = FALSE;
			for (vector<__REQUESTGUILD>::iterator itv = vecRequestRanking.begin(); itv != vecRequestRanking.end(); ++itv)
			{
				if (((__REQUESTGUILD)*itv).dwPenya == pGCMember->dwPenya)
				{
					BOOL bSamPenya = FALSE;
					while (((__REQUESTGUILD)*itv).dwPenya == pGCMember->dwPenya)
					{
						++itv;
						if (itv == vecRequestRanking.end())
						{
							bSamPenya = TRUE;
							--itv;
							break;
						}
					}

					if (bSamPenya)
					{
						vecRequestRanking.push_back(RequstGuild);
					}
					else
					{
						vecRequestRanking.insert(itv, RequstGuild);
					}
					bInsert = TRUE;
					break;
				}
				else
					if (((__REQUESTGUILD)*itv).dwPenya < pGCMember->dwPenya)
					{
						vecRequestRanking.insert(itv, RequstGuild);
						bInsert = TRUE;
						break;
					}
			}
			if (bInsert == FALSE)
			{
				vecRequestRanking.push_back(RequstGuild);
			}
		}
	}
}
void CGuildCombat::SetDefender(u_long uidGuild, u_long uidDefender)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(uidGuild);
	if (pGCMember != NULL)
		pGCMember->m_uidDefender = uidDefender;
}
u_long CGuildCombat::GetDefender(u_long uidGuild)
{
	u_long uidDefender = 0;
	__GuildCombatMember* pGCMember = FindGuildCombatMember(uidGuild);
	if (pGCMember != NULL)
		uidDefender = pGCMember->m_uidDefender;
	return uidDefender;
}
u_long CGuildCombat::GetBestPlayer(u_long* dwGetGuildId, int* nGetPoint)
{
	u_long uBestPlayer = 0;
	map<u_long, int> mapSame;

	BOOL bResult = FALSE;
	int nMaxPoint = -1;
	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				if (nMaxPoint < pJoinPlayer->nPoint)
				{
					nMaxPoint = pJoinPlayer->nPoint;
					uBestPlayer = pJoinPlayer->uidPlayer;
					*nGetPoint = pJoinPlayer->nPoint;
					*dwGetGuildId = pGCMember->uGuildId;
					bResult = TRUE;
					mapSame.clear();
					mapSame.insert(make_pair(pJoinPlayer->uidPlayer, nVeci));
				}
				else if (nMaxPoint == pJoinPlayer->nPoint)
				{
					bResult = FALSE;
					mapSame.insert(make_pair(pJoinPlayer->uidPlayer, nVeci));
				}
			}
		}
	}

	int nMinLevel = 0x7fffffff;
	EXPINTEGER nMinExp = (EXPINTEGER)0x7fffffffffffffff;

	if (bResult == FALSE)
	{
		for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
		{
			if (nVeci >= m_nMaxGuild)
				break;

			__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
			__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
			if (pGCMember != NULL)
			{
				for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];

					if (!pJoinPlayer || mapSame.find(pJoinPlayer->uidPlayer) == mapSame.end())
						continue;

					CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
					if (IsValidObj(pMover))
					{
						//if( nMaxLevel < pMover->GetLevel() )
						if (nMinLevel > pMover->GetLevel())
						{
							//nMaxLevel = pMover->GetLevel();
							//nMaxExp = pMover->GetExp1();
							nMinLevel = pMover->GetLevel();
							nMinExp = pMover->GetExp1();
							uBestPlayer = pJoinPlayer->uidPlayer;
							*nGetPoint = pJoinPlayer->nPoint;
							*dwGetGuildId = pGCMember->uGuildId;
						}
						//else if( nMaxLevel == pMover->GetLevel() )
						else if (nMinLevel == pMover->GetLevel())
						{
							if (nMinExp > pMover->GetExp1())
							{
								//nMaxLevel = pMover->GetLevel();
								//nMaxExp = pMover->GetExp1();
								nMinLevel = pMover->GetLevel();
								nMinExp = pMover->GetExp1();
								uBestPlayer = pJoinPlayer->uidPlayer;
								*nGetPoint = pJoinPlayer->nPoint;
								*dwGetGuildId = pGCMember->uGuildId;
							}
						}
					}
				}
			}
		}
	}
	return uBestPlayer;
}


__int64 CGuildCombat::GetPrizePenya(int nFlag)
{

	__int64 nPrizePenya = 0;
	for (int veci = 0; veci < (int)(vecRequestRanking.size()); ++veci)
	{
		if (veci >= m_nMaxGuild)
			break;

		nPrizePenya += vecRequestRanking[veci].dwPenya;
	}
	__int64 nResult;

	switch (nFlag)
	{
	case 0:
		nResult = MulDiv((int)(nPrizePenya), m_nMaxGuildPercent, 100);
		break;
	case 1:
		nResult = MulDiv((int)(nPrizePenya), m_nMaxPlayerPercent, 100);
		break;
	case 2:
		nResult = nPrizePenya;
	}

	return nResult;
}

DWORD CGuildCombat::GetRequstPenya(u_long uidGuild)
{
	DWORD dwRequestPenya = 0;
	__GuildCombatMember* pGCMember = FindGuildCombatMember(uidGuild);
	if (pGCMember != NULL)
		dwRequestPenya = pGCMember->dwPenya;
	return dwRequestPenya;
}

CTime CGuildCombat::GetNextGuildCobmatTime()
{
	CTime tCurrentTime = CTime::GetCurrentTime();
	CTime tNextCombat;
	int nDayofWeek = m_nDay - tCurrentTime.GetDayOfWeek();
	if (0 < nDayofWeek)
		tNextCombat = tCurrentTime + CTimeSpan(nDayofWeek, 0, 0, 0);
	else if (0 == nDayofWeek)
	{
		CTimeSpan tCTime = CTimeSpan(0, tCurrentTime.GetHour(), tCurrentTime.GetMinute(), tCurrentTime.GetSecond());
		CTimeSpan tNTime = CTimeSpan(0, __AutoOpen[m_nDay - 1].nHour, __AutoOpen[m_nDay - 1].nMinute, 0);
		if (tCTime <= tNTime)
			tNextCombat = tCurrentTime;
		else
			tNextCombat = tCurrentTime + CTimeSpan(m_nDay, 0, 0, 0);
	}
	else if (0 > nDayofWeek)
	{
		nDayofWeek = 7 + nDayofWeek;
		tNextCombat = tCurrentTime + CTimeSpan(nDayofWeek, 0, 0, 0);
	}
	tNextCombat = CTime(tNextCombat.GetYear(), tNextCombat.GetMonth(), tNextCombat.GetDay(), __AutoOpen[m_nDay - 1].nHour, __AutoOpen[m_nDay - 1].nMinute, 0);
	return tNextCombat;
}

CGuildCombat::__GuildCombatMember* CGuildCombat::FindGuildCombatMember(u_long GuildId)
{
	__GuildCombatMember* pGCMem = NULL;
	for (int i = 0; i < (int)(m_vecGuildCombatMem.size()); ++i)
	{
		if (GuildId == m_vecGuildCombatMem.at(i)->uGuildId)
		{
			pGCMem = m_vecGuildCombatMem.at(i);
			break;
		}
	}
	return pGCMem;
}

void CGuildCombat::GuildCombatEnter(CUser* pUser)
{
	BOOL bJoin = TRUE;
	if (m_nState == OPEN_STATE || m_nState == CLOSE_STATE || m_nState == WAR_STATE)
	{
		BOOL bJoin = TRUE;
		if (m_nState == OPEN_STATE && m_nGCState != ENTER_STATE)
		{
			bJoin = FALSE;
			pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_JOIN));
		}

		if (bJoin)
		{
			JoinObserver(pUser, TRUE);
		}
	}
	else if (m_nState == COMPLET_CLOSE_STATE)
	{

		pUser->AddText(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_OPEN));
		bJoin = FALSE;
	}
	else
	{
		bJoin = FALSE;
	}

	if (bJoin)
	{
		if (0 < m_dwTime)
		{
			pUser->AddGuildCombatNextTime(m_dwTime - GetTickCount(), GuildCombatProcess[m_nProcessGo - 1].dwParam);
		}
	}
}

void CGuildCombat::SetMaintenance()
{
	BOOL bWinGuild_Continue = FALSE;
	int nCount = 0;
	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{

		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];

		if (RequestGuild.uidGuild == m_uWinGuildId)
			bWinGuild_Continue = TRUE;

		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			CGuild* pGuild = g_GuildMng.GetGuild(pGCMember->uGuildId);
			if (pGuild)
			{
				pGCMember->nJoinCount = 0;
				pGCMember->nWarCount = 0;
				BOOL bMaxWarCount = FALSE;

				for (int i = 0; i < (int)(pGCMember->vecGCSelectMember.size()); ++i)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[i];
					if (0 < pJoinPlayer->nlife)
						pGCMember->lspFifo.push_back(pJoinPlayer);
				}

				while (pGCMember->lspFifo.empty() == false)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->lspFifo.front();
					CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
					if (IsValidObj(pMover) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
					{
						pGCMember->nWarCount++;
						pJoinPlayer->dwTelTime = timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow(m_nMaxMapTime);
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if (pJoinPlayer->nlife > 0)
							pGCMember->lspFifo.push_back(pJoinPlayer);
						if (pJoinPlayer->nlife < 0)
							pJoinPlayer->nlife = 0;
					}
					if (pGCMember->nWarCount == m_nMaxWarPlayer)
						break;
				}

				g_UserMng.AddGCGuildStatus(pGCMember->uGuildId);

				if (pGCMember->vecGCSelectMember.empty() == false)
					++nCount;
			}
		}
	}

	if (nCount < m_nMinGuild)
	{
		if (bWinGuild_Continue == FALSE)
		{
			g_dpDBClient.SendGuildcombatContinue(m_nGuildCombatIndex - 1, m_uWinGuildId, 0);
			m_uWinGuildId = 0;
			m_nWinGuildCount = 0;

			g_UserMng.AddGCWinGuild();
		}
		SetGuildCombatCloseWait(TRUE);
	}
}

void CGuildCombat::SetEnter()
{
	char str[512] = { 0, };
	sprintf(str, prj.GetText(TID_GAME_GUILDCOMBAT_ENTER));
	g_DPCoreClient.SendSystem(str);

	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{

		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pMover))
				{
					sprintf(str, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_TELE));
					((CUser*)pMover)->AddGCTele(str);
				}
			}
		}
	}
}

void CGuildCombat::SetGuildCombatStart()
{
	__GuildCombatMember* pGuildCombatMem;
	__JOINPLAYER* pJoinPlayer;
	__REQUESTGUILD RequestGuild;

	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		RequestGuild = vecRequestRanking[nVeci];
		pGuildCombatMem = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGuildCombatMem != NULL)
		{
			for (int veci = 0; veci < (int)(pGuildCombatMem->vecGCSelectMember.size()); ++veci)
			{
				pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
				BOOL bFind = FALSE;
				for (list<__JOINPLAYER*>::iterator i1 = pGuildCombatMem->lspFifo.begin(); i1 != pGuildCombatMem->lspFifo.end(); ++i1)
				{
					if (pJoinPlayer == *i1)
					{
						bFind = TRUE;
						break;
					}
				}
				if (pJoinPlayer->nlife > 0 && !bFind)
				{
					CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
					if (IsValidObj(pMover))
					{
						pMover->m_nGuildCombatState = 1;
						g_UserMng.AddGuildCombatUserState(pMover);
					}
				}
			}
		}
	}
}

void CGuildCombat::SetGuildCombatClose(BOOL bGM)
{
	if (m_nState == CLOSE_STATE)
		return;

	if (bGM)
	{
		m_nStopWar = 1;
		return;
	}

	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pMover))
				{
					pMover->m_nGuildCombatState = 0;
					g_UserMng.AddGuildCombatUserState(pMover);
				}
			}
		}
	}


	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			int nRevivalPoint = 0;
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				nRevivalPoint += pJoinPlayer->nlife;
			}
			pGCMember->nGuildPoint += nRevivalPoint;

			CGuild* pGuildMsg;
			CGuildMember* pMember;
			CUser* pUsertmp;
			pGuildMsg = g_GuildMng.GetGuild(RequestGuild.uidGuild);
			if (pGuildMsg)
			{
				for (map<u_long, CGuildMember*>::iterator iGuild = pGuildMsg->m_mapPMember.begin();
					iGuild != pGuildMsg->m_mapPMember.end(); ++iGuild)
				{
					pMember = iGuild->second;
					pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
					if (IsValidObj(pUsertmp) && pUsertmp->GetWorld() && pUsertmp->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
						pUsertmp->AddDefinedCaption(TRUE, TID_GAME_GUILDCOMBAT_POINT_REVIVAL, "%d", nRevivalPoint);
				}
			}
		}
	}
	g_UserMng.AddGCGuildPrecedence();
}

void CGuildCombat::SetGuildCombatCloseWait(BOOL bGM)
{
	if (m_nState == CLOSE_STATE)
		return;

	if (bGM)
	{
		m_nStopWar = 2;
		return;
	}
	else
	{
		GuildCombatResult();
#if __VER >= 11 // __GUILDCOMBATCHIP
		GuildCombatResultRanking();
#endif // __GUILDCOMBATCHIP
	}
}

BOOL CGuildCombat::IsRequestWarGuild(u_long uidGuild, BOOL bAll)
{
	for (int veci = 0; veci < (int)(vecRequestRanking.size()); ++veci)
	{
		if (bAll == FALSE)
		{
			if (m_nMaxGuild <= veci)
				break;
		}

		__REQUESTGUILD RequestGuild = vecRequestRanking[veci];
		if (uidGuild == RequestGuild.uidGuild)
			return TRUE;
	}
	return FALSE;
}


BOOL CGuildCombat::IsSelectPlayer(CUser* pUser)
{
	__GuildCombatMember* pGuildCombatMem;
	__JOINPLAYER* pJoinPlayer;
	__REQUESTGUILD RequestGuild;

	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		RequestGuild = vecRequestRanking[nVeci];
		if (RequestGuild.uidGuild == pUser->m_idGuild)
		{
			pGuildCombatMem = FindGuildCombatMember(RequestGuild.uidGuild);
			if (pGuildCombatMem != NULL)
			{
				for (int veci = 0; veci < (int)(pGuildCombatMem->vecGCSelectMember.size()); ++veci)
				{
					pJoinPlayer = pGuildCombatMem->vecGCSelectMember[veci];
					if (pJoinPlayer->uidPlayer == pUser->m_idPlayer)
					{
						if (pJoinPlayer->nlife > 0)
							return TRUE;
						break;
					}
				}
			}
			break;
		}
	}
	return FALSE;
}

void CGuildCombat::Process()
{
#ifdef __WORLDSERVER
	if (m_nState != CLOSE_STATE)
	{
		if (m_nProcessGo < m_nProcessCount[0])
			ProcessCommand();
		ProcessJoinWar();
	}
	else
	{
		CTime ctime = CTime::GetCurrentTime();

		if (__AutoOpen[ctime.GetDayOfWeek() - 1].bUseing)
		{
			if (__AutoOpen[ctime.GetDayOfWeek() - 1].nHour == ctime.GetHour() &&
				__AutoOpen[ctime.GetDayOfWeek() - 1].nMinute == ctime.GetMinute() && m_bMutex == FALSE)
			{
				if ((int)(m_vecGuildCombatMem.size()) >= m_nMinGuild)
				{
					GuildCombatOpen();
				}
				else
				{
					m_ctrMutexOut.Set(SEC(60));
					m_bMutexMsg = TRUE;
					g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_GUILDCOMBAT_NEXT_COMBAT));
					g_DPCoreClient.SendSystem(prj.GetText(TID_GAME_GUILDCOMBAT_ENJOY));
				}

				m_bMutex = TRUE;
			}

			if (m_bMutexMsg && m_ctrMutexOut.IsTimeOut())
			{
				m_bMutexMsg = FALSE;
				m_bMutex = FALSE;
			}
		}
	}
#endif //__WORLDSERVER
}

void CGuildCombat::ProcessJoinWar()
{
	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking.at(nVeci);
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember.at(veci);
				if (pJoinPlayer->dwTelTime != 0)
				{
					if (pJoinPlayer->dwTelTime + m_nMaxMapTime * 1000 <= timeGetTime())
					{
						pJoinPlayer->dwTelTime = 0;
						CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
						if (IsValidObj(pMover))
						{
							JoinWar((CUser*)pMover);
						}
					}
				}
			}
		}
	}
}
void CGuildCombat::ProcessCommand()
{
	if (m_nStopWar == 1)
	{
		m_nProcessGo = m_nProcessCount[COMPLET_CLOSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 0;
	}
	else if (m_nStopWar == 2)
	{
		m_nProcessGo = m_nProcessCount[GM_COLSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 200;
	}
	if (m_dwTime <= GetTickCount())
	{
		if (GuildCombatProcess[m_nProcessGo].nState == WARSTATE)
		{
			if (GuildCombatProcess[m_nProcessGo].dwCommand == MAINTENANCE_STATE)
			{
				SetMaintenance();
			}
			else if (GuildCombatProcess[m_nProcessGo].dwCommand == ENTER_STATE)
			{

				SetEnter();
			}
			else if (GuildCombatProcess[m_nProcessGo].dwCommand == WAR_WAR_STATE)
			{

				SetGuildCombatStart();
			}
			else if (GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_STATE)
			{

				SetGuildCombatClose();
			}
			else if (GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_WAIT_STATE)
			{

				SetGuildCombatCloseWait();
			}
			else if (GuildCombatProcess[m_nProcessGo].dwCommand == WAR_TELEPORT_STATE)
			{

				GuildCombatCloseTeleport();
			}
		}

		switch (GuildCombatProcess[m_nProcessGo].nState)
		{
		case ALLMSG:
		{
			if (GuildCombatProcess[m_nProcessGo].dwCommand != TID_GAME_GUILDCOMBAT_OPEN_MSG)
			{
				CString str;
				str.Format(prj.GetText(GuildCombatProcess[m_nProcessGo].dwCommand), GuildCombatProcess[m_nProcessGo].dwTime / 1000);
				g_DPCoreClient.SendSystem(str);
			}

			if (GuildCombatProcess[m_nProcessGo].dwCommand == TID_GAME_GUILDCOMBAT_OPEN_MSG)
				SendGuildCombatEnterTime();
		}
		break;
		case GUILDMSG:
		{
		}
		break;
		case JOINMSG:
		{
			CString str;
			str.Format(prj.GetText(GuildCombatProcess[m_nProcessGo].dwCommand), GuildCombatProcess[m_nProcessGo].dwTime / 1000);
			//				SendJoinMsg( str );
		}
		break;
		case WORLDMSG:
		{
			CString str;
			str.Format(prj.GetText(GuildCombatProcess[m_nProcessGo].dwCommand), GuildCombatProcess[m_nProcessGo].dwTime / 1000);
			g_UserMng.AddWorldMsg(WI_WORLD_GUILDWAR, str);
		}
		break;
		case STATE:
		{
			if (g_eLocal.GetState(EVE_GUILDCOMBAT))
				g_DPCoreClient.SendGuildCombatState(GuildCombatProcess[m_nProcessGo].dwCommand);
		}
		break;
		case WARSTATE:
		{
			m_nGCState = GuildCombatProcess[m_nProcessGo].dwCommand;
			g_UserMng.AddGuildCombatState();
		}
		break;
		}
		m_dwTime = GetTickCount() + GuildCombatProcess[m_nProcessGo].dwTime;
		if (GuildCombatProcess[m_nProcessGo].dwParam != 0)
			g_UserMng.AddGuildCombatNextTimeWorld(GuildCombatProcess[m_nProcessGo].dwTime, GuildCombatProcess[m_nProcessGo].dwParam);
		++m_nProcessGo;
	}
}


BOOL CGuildCombat::LoadScript(LPCSTR lpszFileName)
{
	CScanner s;

	if (!s.Load(lpszFileName))
		return FALSE;

	ZeroMemory(m_nProcessCount, sizeof(int) * 25);
	ZeroMemory(GuildCombatProcess, sizeof(__GuildCombatProcess) * 250);
	int		nCount;
	int		nAutoOpenCnt = 0;

	while (s.tok != FINISHED)
	{
		nCount = -1;

		if (s.Token == _T("AUTO_OPEN_IDC"))
		{
			s.GetToken();	// {
			s.GetToken();	// {

			int nDay, nTime1, nTime2;
			while (*s.token != '}')
			{
				if (nAutoOpenCnt >= 7)
				{
					Error("CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!");
					return FALSE;
				}

				nDay = atoi(s.Token);
				m_nDay = nDay;
				if (nDay <= 0 || nDay > 7)
				{
					Error("CGuildCombat::LoadScript() Day Error = %d", nDay);
					return FALSE;
				}
				__AutoOpen[nDay - 1].bUseing = TRUE;

				nTime1 = s.GetNumber();
				if (nTime1 < 0 || nTime1 > 24)
				{
					Error("CGuildCombat::LoadScript() Time Error = %d", nTime1);
					return FALSE;
				}
				__AutoOpen[nDay - 1].nHour = nTime1;

				nTime2 = s.GetNumber();
				if (nTime2 < 0 || nTime2 > 59)
				{
					Error("CGuildCombat::LoadScript() Time Error = %d", nTime2);
					return FALSE;
				}
				__AutoOpen[nDay - 1].nMinute = nTime2;

				nAutoOpenCnt++;

				s.GetToken();
			}
		}
		else if (s.Token == _T("JOINPENYA"))
		{
			m_nJoinPanya = s.GetNumber();
		}
		else if (s.Token == _T("GUILDLEVEL"))
		{
			m_nGuildLevel = s.GetNumber();
		}
#if __VER >= 8 // __GUILDCOMBAT_85
		else if (s.Token == _T("MINJOINGUILDSIZE"))
		{
			m_nMinGuild = s.GetNumber();
		}
#endif // __VER >= 8
		else if (s.Token == _T("MAXJOINGUILDSIZE"))
		{
			m_nMaxGuild = s.GetNumber();
		}
		else if (s.Token == _T("MAXPLAYERLIFE"))
		{
			m_nMaxPlayerLife = s.GetNumber();
		}
		else if (s.Token == _T("MAXWARPLAYER"))
		{
			m_nMaxWarPlayer = s.GetNumber();
		}
		else if (s.Token == _T("MAXMAPTIME"))
		{
			m_nMaxMapTime = s.GetNumber();
		}
		else if (s.Token == _T("MAXJOINMEMBERSIZE"))
		{
			m_nMaxJoinMember = s.GetNumber();
		}
		else if (s.Token == _T("MAXGUILDPERCENT"))
		{
			m_nMaxGuildPercent = s.GetNumber();
		}
		else if (s.Token == _T("MAXPLAYERPERCENT"))
		{
			m_nMaxPlayerPercent = s.GetNumber();
		}
		else if (s.Token == _T("REQUESTCANCLEPERCENT"))
		{
			m_nRequestCanclePercent = s.GetNumber();
		}
		else if (s.Token == _T("NOTREQUESTPERCENT"))
		{
			m_nNotRequestPercent = s.GetNumber();
		}
		else if (s.Token == _T("ITEMPENYA"))
		{
			m_nItemPenya = s.GetNumber();
		}
		else if (s.Token == _T("OPEN"))
		{
			nCount = 0;
		}
		else if (s.Token == _T("MAINTENANCE"))
		{
			nCount = 1;
		}
		else if (s.Token == _T("WAR"))
		{
			nCount = 2;
		}

		if (nCount != -1)
		{
			m_nProcessCount[nCount] = s.GetNumber();
			s.GetToken();	// {
			s.GetToken();	// {
			int nSoCount = 0;
			while (*s.token != '}')
			{
				if (s.Token == _T("ALLMSG"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = ALLMSG;
				}
				else if (s.Token == _T("GUILDMSG"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = GUILDMSG;
				}
				else if (s.Token == _T("JOINMSG"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = JOINMSG;
				}
				else if (s.Token == _T("WORLDMSG"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = WORLDMSG;
				}
				else if (s.Token == _T("STATE"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = STATE;
				}
				else if (s.Token == _T("WARSTATE"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = WARSTATE;
				}
				else if (s.Token == _T("WAIT"))
				{
					GuildCombatProcess[25 * nCount + nSoCount].nState = WAIT;
				}
				GuildCombatProcess[25 * nCount + nSoCount].dwTime = s.GetNumber();
				GuildCombatProcess[25 * nCount + nSoCount].dwCommand = s.GetNumber();
				GuildCombatProcess[25 * nCount + nSoCount].dwParam = s.GetNumber();
				if (GuildCombatProcess[25 * nCount + nSoCount].nState == STATE)
					m_nProcessCount[GuildCombatProcess[25 * nCount + nSoCount].dwCommand] = 25 * nCount + nSoCount;

				++nSoCount;
				s.GetToken();
			}
		}

		s.GetToken();
	}
	return TRUE;
}

void CGuildCombat::SendGCLog(void)
{
	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			int nPoint = 0;
			for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
			{
				__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
				CUser* pUser = (CUser*)prj.GetUserByID(pJoinPlayer->uidPlayer);
				if (IsValidObj(pUser))
					pUser->AddGCLog();
			}
		}
	}
}

void CGuildCombat::SendJoinMsg(LPCTSTR lpszString)
{

}

void CGuildCombat::SendGuildCombatEnterTime(void)
{
	g_UserMng.AddGuildCombatEnterTime(GuildCombatProcess[m_nProcessGo].dwTime);
}

void CGuildCombat::SetPlayerChange(CUser* pUser, CUser* pLeader)
{
	__GuildCombatMember* pGCMember = FindGuildCombatMember(pUser->m_idGuild);
	if (pGCMember != NULL)
	{
		for (int i = 0; i < (int)(pGCMember->vecGCSelectMember.size()); ++i)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[i];
			if (pJoinPlayer->uidPlayer == pUser->m_idPlayer)
			{

				for (list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1)
				{
					if (pJoinPlayer == *i1)
						return;
				}
				//				pJoinPlayer->bEntry	= FALSE;
				pJoinPlayer->dwTelTime = 0;
				pUser->m_nGuildCombatState = 0;
				g_UserMng.AddGuildCombatUserState(pUser);
				--pGCMember->nWarCount;
				pJoinPlayer->nlife--;
				if (pJoinPlayer->nlife > 0)
					pGCMember->lspFifo.push_back(pJoinPlayer);
				if (pJoinPlayer->nlife < 0)
					pJoinPlayer->nlife = 0;

				while (pGCMember->lspFifo.empty() == false)
				{
					__JOINPLAYER* pJoinPlayer = pGCMember->lspFifo.front();
					CMover* pMover = prj.GetUserByID(pJoinPlayer->uidPlayer);
					if (IsValidObj(pMover) && pMover->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
					{
						pGCMember->nWarCount++;
						pJoinPlayer->dwTelTime = timeGetTime();
						((CUser*)pMover)->AddGCJoinWarWindow(m_nMaxMapTime);
						pGCMember->lspFifo.pop_front();
					}
					else
					{
						pGCMember->lspFifo.pop_front();
						pJoinPlayer->nlife--;
						if (pJoinPlayer->nlife > 0)
							pGCMember->lspFifo.push_back(pJoinPlayer);
						if (pJoinPlayer->nlife < 0)
							pJoinPlayer->nlife = 0;
					}
					if (pGCMember->nWarCount == m_nMaxWarPlayer)
						break;
				}	// while
				break;
			}	// if
		}	// for
	}	// if
}
void CGuildCombat::GetPoint(CUser* pAttacker, CUser* pDefender)
{

	BOOL bMaster = FALSE;
	BOOL bDefender = FALSE;
	BOOL bLastLife = FALSE;
	BOOL bKillDiffernceGuild = FALSE;
	int nGetPoint = 2;
	__JOINPLAYER* pJoinPlayerAttacker = NULL;
	__JOINPLAYER* pJoinPlayerDefender = NULL;
	__GuildCombatMember* pGCMemberAttacker = NULL;
	CGuild* pGuild = g_GuildMng.GetGuild(pAttacker->m_idGuild);
	if (pGuild && pGuild->IsMaster(pAttacker->m_idPlayer))
		bMaster = TRUE;

	pGCMemberAttacker = FindGuildCombatMember(pAttacker->m_idGuild);
	if (pGCMemberAttacker != NULL)
	{
		for (int veci = 0; veci < (int)(pGCMemberAttacker->vecGCSelectMember.size()); ++veci)
		{
			__JOINPLAYER* pJoinPlayer = pGCMemberAttacker->vecGCSelectMember[veci];
			if (pJoinPlayer->uidPlayer == pAttacker->m_idPlayer)
			{
				pJoinPlayerAttacker = pJoinPlayer;
				break;
			}
		}
		if (pJoinPlayerAttacker == NULL)
			return;

		if (pJoinPlayerAttacker->uKillidGuild != 0 && pJoinPlayerAttacker->uKillidGuild != pDefender->m_idGuild)
			bKillDiffernceGuild = TRUE;

		if (pJoinPlayerAttacker->nlife == 1)
			bLastLife = TRUE;
	}

	if (pGCMemberAttacker == NULL)
		return;

	__GuildCombatMember* pGCMember = FindGuildCombatMember(pDefender->m_idGuild);
	if (pGCMember != NULL)
	{
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if (pJoinPlayer->uidPlayer == pDefender->m_idPlayer)
			{
				pJoinPlayerDefender = pJoinPlayer;
				break;
			}
		}
		if (pJoinPlayerDefender == NULL)
			return;

		if (pDefender->m_idPlayer == pGCMember->m_uidDefender)
			bDefender = TRUE;
	}


	if (bKillDiffernceGuild)
		++nGetPoint;

	if (bMaster)
		++nGetPoint;

	if (bDefender)
		++nGetPoint;

	if (bLastLife)
		++nGetPoint;

	pJoinPlayerAttacker->nPoint += nGetPoint;
	pGCMemberAttacker->nGuildPoint += nGetPoint;

	CGuild* pGuildMsg;
	CGuildMember* pMember;
	CUser* pUsertmp;

	pGuildMsg = pAttacker->GetGuild();

	AddvecGCGetPoint(pAttacker->m_idGuild, pDefender->m_idGuild, pAttacker->m_idPlayer, pDefender->m_idPlayer, nGetPoint, bKillDiffernceGuild, bMaster, bDefender, bLastLife);
#ifdef __FL_BATTLE_PASS
	CBattlePass::GetInstance()->AddProgress(pAttacker, EXP_GW_KILL);
#endif
	g_UserMng.AddGCGuildPrecedence();
	g_UserMng.AddGCPlayerPrecedence();
}

void CGuildCombat::SerializeGCWarPlayerList(CAr& ar)
{
	if (m_nMaxGuild <= (int)(vecRequestRanking.size()))
		ar << m_nMaxGuild;
	else
		ar << (int)vecRequestRanking.size();

	for (int nVeci = 0; nVeci < (int)(vecRequestRanking.size()); ++nVeci)
	{
		if (nVeci >= m_nMaxGuild)
			break;

		__REQUESTGUILD RequestGuild = vecRequestRanking[nVeci];
		__GuildCombatMember* pGCMember = FindGuildCombatMember(RequestGuild.uidGuild);
		if (pGCMember != NULL)
		{
			__JOINPLAYER* pJoinPlayer1;

			ar << pGCMember->m_uidDefender;
			ar << (int)pGCMember->vecGCSelectMember.size();
			for (int vecSi = 0; vecSi < (int)(pGCMember->vecGCSelectMember.size()); ++vecSi)
			{
				pJoinPlayer1 = pGCMember->vecGCSelectMember[vecSi];

				ar << pJoinPlayer1->uidPlayer;
				if (0 < pJoinPlayer1->nlife)
				{
					BOOL bFind = FALSE;
					for (list<__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1)
					{
						if (pJoinPlayer1 == *i1)
						{
							bFind = TRUE;
							break;
						}
					}

					if (bFind)
					{
						ar << (int)0;
					}
					else
					{
						ar << (int)1;
					}
				}
				else
				{
					ar << (int)0;
				}
			}
		}
		else
		{
			ar << (u_long)0;
			ar << (int)0;
		}
	}
}
#ifdef __FL_GW_STATUE_V2
void CGuildCombat::DeleteStatues()
{
	CMover* pOldMvpStatue = prj.GetMover(prj.GetCurrentMvpStatueId());
	if (IsValidObj(pOldMvpStatue))
		pOldMvpStatue->Delete();

	CMover* pOldLeaderStatue = prj.GetMover(prj.GetCurrentLeaderStatueId());
	if (IsValidObj(pOldLeaderStatue))
		pOldLeaderStatue->Delete();

	prj.WipeStatuesParam();
}
void CGuildCombat::ApplyStatueBuff(CUser* pUser, BOOL bIsMvp, int nClientSidedPrice)
{
	DWORD dwMoverID = bIsMvp ? prj.GetCurrentMvpStatueId() : prj.GetCurrentLeaderStatueId();
	int nPrice = bIsMvp ? prj.GetCurrentMvpStatuePrice() : prj.GetCurrentLeaderStatuePrice();
	int nMaxPrice = 2000000000;
	DWORD dwBuffID = bIsMvp ? dwMvpStatueBuff : dwLeaderStatueBuff;
	DWORD dwBuffSFX = bIsMvp ? dwMvpAfterBuffEffect : dwLeaderAfterBuffEffect;

	CMover* pStatue = prj.GetMover(dwMoverID);
	if (!IsValidObj(pStatue))
		return;

	ItemProp* pBuffProp = prj.GetItemProp(dwBuffID);
	if (!pBuffProp)
		return;

	if (pUser->HasBuff(BUFF_ITEM, dwBuffID))
	{
		pUser->AddDefinedText(TID_GAME_HOUSING_ADDITION_FAIL);
		return;
	}

	if (nPrice != nClientSidedPrice)
	{
		pUser->AddStatueParams(nPrice);
		pUser->AddText("Price was outdated. Please check new parameters and accept it again.", 0xffffffff);
		return;
	}

	BOOL bFreeBuff = bIsMvp ? (pStatue->GetNpcOwner() == pUser->m_idPlayer) : (pStatue->m_idGuild == pUser->m_idGuild);
	if (!bFreeBuff)
	{
		if (nPrice < 1 || nPrice > nMaxPrice)
		{
			pUser->AddText("This statue is currently inactive. Owner should activate it firstly, after that you will be able to purchase buff-effect!", 0xffffffff);
			return;
		}

		CString str;
		if (pUser->GetGold() < nPrice)
		{
			str.Format("Not enough penya. You should have at least %d penya to purchase this effect!", nPrice);
			pUser->AddText(str, 0xffffffff);
			return;
		}
		else
		{
			pUser->AddGold(-nPrice);
			str.Format("Buff-effect purchased successfully!");
			pUser->AddText(str, 0xffffffff);

			int nCommision = nPrice / 10;
			nPrice = nPrice - nCommision;

			str.Format("Player %s just purchased your statue buff! Commision: %s penya.", pUser->GetName(), Separator(nCommision));
			char str2[1024] = { 0, };
			sprintf(str2, str);

			g_dpDBClient.SendQueryPostMail(pStatue->GetNpcOwner(), 0, CItemElem(), nPrice, "Statue Payment", str2);
		}
	}

	pUser->DoApplySkill(pUser, pBuffProp, NULL);
	g_UserMng.AddCreateSfxObj(((CMover*)pUser), dwBuffSFX);
}
#endif

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
CMover* CGuildCombat::CreatePlayerCopy(BYTE nType, PLAYER_COPY struc)
{
	CMover* pMover = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, struc.dwIndex);
	if (pMover)
	{
		CWorld* pWorld = g_WorldMng.GetWorld(struc.dwWorld);
		if (!pWorld)
			return NULL;

		pMover->SetPos(struc.vPos);
		pMover->InitMotion(MTI_STAND);
		pMover->SetName(struc.strName);
		pMover->SetAngle(struc.fAngle);
		pMover->m_fSpeedFactor = 0;
		pMover->SetAIInterface(AII_NONE);
		pMover->m_dwBelligerence = BELLI_PEACEFUL;
		pMover->m_bActiveAttack = 0;
		pMover->m_nJob = struc.nJob;
		pMover->m_nLevel = struc.nLevel;
		pMover->SetSex(struc.dwIndex == MI_MALE ? SEX_MALE : SEX_FEMALE);
		pMover->SetHair(struc.dwHairId);
		pMover->SetHairColor(struc.dwHairColor);
		pMover->SetHead(struc.dwHeadId);

		for (map<u_long, CGuild*>::iterator i = g_GuildMng.m_mapPGuild.begin(); i != g_GuildMng.m_mapPGuild.end(); ++i)
		{
			if ((i->second)->IsMember(struc.idPlayer))
			{
				pMover->m_idGuild = (i->second)->GetGuildId();
				break;
			}
		}

		pMover->UpdateLocalMatrix();
		pMover->AddItToGlobalId();
		pMover->SetNpcType(nType);
		pMover->SetNpcOwner(struc.idPlayer);

		if (struc.tmpInventoryOwner != NULL)
		{
			int nPart = 0;
			for (int i = 0; i < struc.tmpInventoryOwner->GetMax(); i++)
			{
				CItemElem* pItemElem = struc.tmpInventoryOwner->GetAtId(i);
				if (pItemElem && pItemElem->m_dwObjIndex >= MAX_INVENTORY)
				{
					nPart = pItemElem->m_dwObjIndex - MAX_INVENTORY;
					if (nPart == PARTS_RIDE)
						continue;

					pMover->m_aEquipInfo[nPart].dwId = pItemElem->m_dwItemId;
					pMover->m_aEquipInfo[nPart].nOption = pItemElem->GetAttrOption();
#ifdef __MODEL_CHANGE
					pMover->m_aEquipInfo[nPart].dwLook = pItemElem->GetLook();
#endif
				}
			}
		}

		if (pWorld->AddObj(pMover, FALSE, nDefaultLayer))
		{
			switch (nType)
			{
#ifdef __FL_GW_STATUE_V2
			case NPC_TYPE_MVPSTATUE: prj.SetCurrentMvpStatueId(pMover->GetId()); break;
			case NPC_TYPE_GUILDLEADERSTATUE: prj.SetCurrentLeaderStatueId(pMover->GetId()); break;
#endif
#ifdef __FL_VENDORS
			case NPC_TYPE_VENDOR_HUMAN:
			case NPC_TYPE_VENDOR_NPC:
			{
				prj.m_mapPlayerShops.insert(make_pair(struc.idPlayer, pMover->GetId()));
			}
			break;
#endif
			default: break;
			}

			return pMover;
		}
		else
			SAFE_DELETE(pMover);
	}

	return NULL;
}
#endif

#endif	// __WORLDSERVER


CGuildCombat	g_GuildCombatMng;
