#include "stdafx.h"
#include "defineText.h"
#include "defineObj.h"
#include "party.h"
#include "User.h"
#include "guildquest.h"
#include "defineQuest.h"
#include "worldmng.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpSrvr.h"
#include "guild.h"
#include "ScriptHelper.h"
#if defined(__REMOVE_SCIRPT_060712)
#include "WorldDialog.h"
#endif
#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

extern	CPartyMng			g_PartyMng;
extern	CWorldMng			g_WorldMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr				g_DPSrvr;
extern	CGuildMng			g_GuildMng;
extern	CUserMng			g_UserMng;


Functions		g_functions;


BOOL HasKey(NPCDIALOG_INFO* pInfo, LPCTSTR szKey)
{
	return CWorldDialog::GetInstance().Find(pInfo->GetName(), szKey);
}

int APIENTRY SayName(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY Speak(NPCDIALOG_INFO* pInfo, int nId, LPCTSTR lpszStr)
{
	CMover* pMover = prj.GetMover(nId);
	if (pMover && pMover->GetType() == OT_MOVER)
	{
		g_UserMng.AddChat((CCtrl*)pMover, lpszStr);
	}
	return 1;
}

int APIENTRY SpeakName(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY GetVal(NPCDIALOG_INFO* pInfo)
{
	return pInfo->GetReturn();
}

int APIENTRY GetSrcId(NPCDIALOG_INFO* pInfo)
{
	return pInfo->GetPcId();
}

int APIENTRY GetDstId(NPCDIALOG_INFO* pInfo)
{
	return pInfo->GetNpcId();
}

int APIENTRY NpcId(NPCDIALOG_INFO* pInfo)
{
	return GetDstId(pInfo);
}

int APIENTRY PcId(NPCDIALOG_INFO* pInfo)
{
	return GetSrcId(pInfo);
}

int APIENTRY GetGValue_(NPCDIALOG_INFO* pInfo)
{
	return 1;
}
int APIENTRY SetGValue(NPCDIALOG_INFO* pInfo)
{
	return 1;
}
int APIENTRY AddGValue(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY AddKey(NPCDIALOG_INFO* pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam)
{
	dwParam = 0;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	if (strKey.IsEmpty())
		strKey = strWord;

	if (HasKey(pInfo, strKey) == FALSE)
	{
		Error("key error AddKey %s %s", pInfo->GetName(), strKey);
		return 1;
	}
	__AddKey(pInfo->GetPcId(), strWord, strKey, dwParam);
	return 1;
}

int APIENTRY AddCondKey(NPCDIALOG_INFO* pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam)
{
	dwParam = 0;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	if (strKey.IsEmpty())
		strKey = strWord;

	if (HasKey(pInfo, strKey) == FALSE)
	{
		Error("key error %s %s", pInfo->GetName(), strKey);
		return 1;
	}
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CMover* pMover = prj.GetMover(pInfo->GetNpcId());

	for (int i = 0; i < pUser->m_nQuestSize; i++)
	{
		QuestProp* pQuestProp = pUser->m_aQuest[i].GetProp();
		if (pQuestProp)
		{
			if (strcmp(pQuestProp->m_szEndCondDlgCharKey, pMover->m_szCharacterKey) == 0)
			{
				if (strcmp(pQuestProp->m_szEndCondDlgAddKey, strKey) == 0)
				{
					__AddKey(pInfo->GetPcId(), strWord, strKey, dwParam);
					return 1;
				}
			}
		}
	}
	return 0;
}

int APIENTRY AddAnswer(NPCDIALOG_INFO* pInfo, LPCTSTR szWord, LPCTSTR szKey, DWORD dwParam)
{
	DWORD nParam1, nParam2;
	CString strWord, strKey;

	strWord = szWord;
	strKey = szKey;

	nParam1 = dwParam;
	nParam2 = 0;

	if (strKey.IsEmpty())
		strKey = strWord;

	if (HasKey(pInfo, strKey) == FALSE)
	{
		Error("key error AddAnswer %s %s", pInfo->GetName(), strKey);
		return 1;
	}
	return __AddAnswer(pInfo->GetPcId(), strWord, strKey, nParam1, nParam2);
}

int APIENTRY RemoveQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	return __RemoveQuest(pInfo->GetPcId(), nQuest);
}

int APIENTRY RemoveAllKey(NPCDIALOG_INFO* pInfo)
{
	__RemoveAllKey(pInfo->GetPcId());
	return 1;
}

int APIENTRY SayQuest(NPCDIALOG_INFO* pInfo, int nQuest, int nDialog)
{
	__SayQuest(pInfo->GetPcId(), nQuest, nDialog);
	return 1;
}

int APIENTRY BeginQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	return __RunQuest(pInfo->GetPcId(), pInfo->GetNpcId(), nQuest);
}

int APIENTRY EndQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	return __EndQuest(pInfo->GetPcId(), nQuest, FALSE);
}

int APIENTRY AddQuestKey(NPCDIALOG_INFO* pInfo, int nQuest, LPCTSTR szKey)
{
	int nParam = 0;
	CString strKey = szKey;

	if (strKey.IsEmpty())
	{
		QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(nQuest);
		if (pQuestProp)
			strKey = pQuestProp->m_szTitle;
	}

	if (HasKey(pInfo, strKey) == FALSE)
	{
		Error("key error AddQuestKey %s %s", pInfo->GetName(), strKey);
		return 1;
	}

	return __AddQuestKey(pInfo->GetPcId(), nQuest, strKey, nParam);
}


int APIENTRY CreateMover(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY CreateChar(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY RemoveMover(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY GetDay(NPCDIALOG_INFO* pInfo)
{
	return g_GameTimer.m_nDay;
}

int APIENTRY GetMin(NPCDIALOG_INFO* pInfo)
{
	return g_GameTimer.m_nMin;
}

int APIENTRY GetHour(NPCDIALOG_INFO* pInfo)
{
	return g_GameTimer.m_nHour;
}

int APIENTRY SetTimer(NPCDIALOG_INFO* pInfo)
{
	return 1;
}
int APIENTRY IsTimeOut(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY LaunchQuest(NPCDIALOG_INFO* pInfo)
{
	int* p = pInfo->GetGlobal();
	*p = 0;
	__QuestEnd(pInfo->GetPcId(), pInfo->GetNpcId(), *p);
	return 1;
}
int APIENTRY GetEmptyInventoryNum(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->m_Inventory.GetEmptyCount();
}

int APIENTRY GetQuestState(NPCDIALOG_INFO* pInfo, int nQuest)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	LPQUEST pQuest = pUser->FindQuest(nQuest);
	if (pQuest)
		return pQuest->m_nState;
	QUEST quest;
	if (pUser->MakeCompleteQuest(nQuest, &quest))
		return quest.m_nState;
	return -1;
}
int APIENTRY IsSetQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	LPQUEST pQuest = pUser->FindQuest(nQuest);
	if (pQuest) return TRUE;
	if (pUser->IsCompleteQuest(nQuest))
		return TRUE;
	return FALSE;
}

int APIENTRY SetMark(NPCDIALOG_INFO* pInfo)
{
	return 1;
}
int APIENTRY GoMark(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY RandomIndex(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY RemoveKey(NPCDIALOG_INFO* pInfo, LPCTSTR szKey)
{
	if (HasKey(pInfo, szKey))
	{
		CUser* pUser = prj.GetUser(pInfo->GetPcId());

		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_REMOVEKEY;
		lstrcpy(rsf.lpszVal1, szKey);
		pUser->AddRunScriptFunc(rsf);
	}
	return 1;
}

int APIENTRY Say(NPCDIALOG_INFO* pInfo, LPCTSTR szMsg)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	RunScriptFunc rsf;
	rsf.wFuncType = FUNCTYPE_SAY;
	lstrcpy(rsf.lpszVal1, szMsg);
	rsf.dwVal2 = 0;
	pUser->AddRunScriptFunc(rsf);
	return 1;
}

int APIENTRY EndSay(NPCDIALOG_INFO* pInfo)
{
	return 1;
}

int APIENTRY Random(NPCDIALOG_INFO* pInfo, int n)
{
	return rand() % n;
}

int APIENTRY Exit(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	RunScriptFunc rsf;
	rsf.wFuncType = FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc(rsf);
	return TRUE;
}

int APIENTRY  SetScriptTimer(NPCDIALOG_INFO* pInfo, int nTimer)
{
	return 1;
}

// Replace( int nWorld, int x, int y, int z )
int APIENTRY Replace(NPCDIALOG_INFO* pInfo, int nWorld, float x, float y, float z)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	if (x > 0.0f && z > 0.0f)
	{
		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_EXIT;
		pUser->AddRunScriptFunc(rsf);
		pUser->REPLACE(g_uIdofMulti, nWorld, D3DXVECTOR3(x, y, z), REPLACE_NORMAL, nTempLayer);
	}
	return 1;
}

int APIENTRY ReplaceKey(NPCDIALOG_INFO* pInfo, int nWorld, LPCTSTR szKey)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	RunScriptFunc rsf;
	rsf.wFuncType = FUNCTYPE_EXIT;
	pUser->AddRunScriptFunc(rsf);

	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos(nWorld, szKey);
	if (pRgnElem)
		pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_NORMAL, nRevivalLayer);
	return 1;
}

int APIENTRY GetPlayerLvl(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetLevel();
}

int APIENTRY GetPlayerJob(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetJob();
}

int APIENTRY IsGuildQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	CGuild* pGuild = pUser->GetGuild();
	if (pGuild)
	{
		PGUILDQUEST pQuest = pGuild->GetQuest(nQuest);
		if (pQuest)
			return TRUE;
	}
	return FALSE;
}

int APIENTRY GetGuildQuestState(NPCDIALOG_INFO* pInfo, int nQuest)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CGuild* pGuild = pUser->GetGuild();
	if (pGuild)
	{
		PGUILDQUEST pQuest = pGuild->GetQuest(nQuest);
		if (pQuest)
		{
			int nState = pQuest->nState;
			return nState;
		}
	}
	return -1;
}

int APIENTRY IsWormonServer(NPCDIALOG_INFO* pInfo)
{
	return g_eLocal.GetState(EVE_WORMON);
}

int APIENTRY PrintSystemMessage(NPCDIALOG_INFO* pInfo, int nText)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->AddDefinedText((DWORD)nText);
	return 1;
}

int APIENTRY  MonHuntStart(NPCDIALOG_INFO* pInfo, int nQuest, int nState, int nState2, int n)
{
	CGuildQuestProcessor* pProcessor = CGuildQuestProcessor::GetInstance();
	if (pProcessor->IsQuesting(nQuest))
	{
		return 0;
	}
	else
	{
		CUser* pUser = prj.GetUser(pInfo->GetPcId());
		CGuild* pGuild = pUser->GetGuild();
		if (pGuild)
		{
			GUILDQUESTPROP* pProp = prj.GetGuildQuestProp(nQuest);
			if (pProp)
			{
				CMover* pWormon = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, pProp->dwWormon);
				if (pWormon)
				{
					pWormon->SetPos(pProp->vPos);
					pWormon->InitMotion(MTI_STAND);
					pWormon->UpdateLocalMatrix();
					pWormon->m_nQuestKeeping = nQuest;

					CWorld* pWorld = pUser->GetWorld();
					if (pWorld)
					{
						RunScriptFunc rsf;	// Exit()
						rsf.wFuncType = FUNCTYPE_EXIT;
						pUser->AddRunScriptFunc(rsf);
						pWormon->AddItToGlobalId();
						if (pWorld->ADDOBJ(pWormon, FALSE, pUser->GetLayer()) == TRUE)
						{
							pGuild->SetQuest(nQuest, nState);
							if (nState == QS_BEGIN)
								g_dpDBClient.SendInsertGuildQuest(pGuild->m_idGuild, nQuest);
							else
								g_dpDBClient.SendUpdateGuildQuest(pGuild->m_idGuild, nQuest, nState);

							pProcessor->SetGuildQuest(nQuest, nState, nState2, n, pGuild->m_idGuild, pWormon->GetId());
							D3DXVECTOR3 vPos(pProp->vPos.x, pProp->vPos.y, ((pProp->vPos.z * 2) + pProp->y2) / 3);
							pGuild->Replace(pProp->dwWorldId, vPos, TRUE);
							return 1;
						}
						else
						{
							SAFE_DELETE(pWormon);
							return 0;
						}
					}
					else
					{
						SAFE_DELETE(pWormon);
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

int APIENTRY  MonHuntStartParty(NPCDIALOG_INFO* pInfo, int nQuest, int nState, int nState2, int n)
{
	CPartyQuestProcessor* pProcessor = CPartyQuestProcessor::GetInstance();
	if (pProcessor->IsQuesting(nQuest))
	{
		return 0;
	}
	else
	{
		CUser* pUser = prj.GetUser(pInfo->GetPcId());

		CParty* pParty = g_PartyMng.GetParty(pUser->GetPartyId());
		if (pParty)
		{
			PARTYQUESTPROP* pProp = prj.GetPartyQuestProp(nQuest);
			if (pProp)
			{
				if (pParty->ReplaceChkLv(pProp->nLevel + 5) == FALSE)
				{
					CString str;
					str.Format(prj.GetText(TID_GAME_PARTYQUEST_NOT_JOIN), pProp->nLevel + 5);
					pUser->AddDiagText(str);

					return 0;
				}

				BOOL bSuccess = FALSE;

				for (DWORD i = 0; i < pProp->vecWormon.size(); i++)
				{
					WORMON* WorMon;
					WorMon = &(pProp->vecWormon[i]);
					CMover* pWormon = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, WorMon->dwWormon);

					if (pWormon)
					{
						pWormon->SetPos(WorMon->vPos);
						pWormon->InitMotion(MTI_STAND);
						pWormon->UpdateLocalMatrix();
						pWormon->m_nPartyQuestKeeping = nQuest;

						CWorld* pWorld = g_WorldMng.GetWorld(pProp->dwWorldId);
						if (pWorld)
						{
							RunScriptFunc rsf;	// Exit()
							rsf.wFuncType = FUNCTYPE_EXIT;
							pUser->AddRunScriptFunc(rsf);
							pWormon->AddItToGlobalId();
							pWormon->m_bActiveAttack = TRUE;
							if (pWorld->ADDOBJ(pWormon, FALSE, pUser->GetLayer()) == TRUE)
							{
								bSuccess = TRUE;
							}
							else
							{
								SAFE_DELETE(pWormon);
								bSuccess = FALSE;
							}
						}
						else
						{
							SAFE_DELETE(pWormon);
							bSuccess = FALSE;
						}
					}
				}

				if (bSuccess)
				{
					pProcessor->SetPartyQuest(nQuest, nState, nState2, n, pUser->GetPartyId(), 0);
					pParty->Replace(pProp->dwWorldId, pProp->szWorldKey);

					return 1;
				}
			}
		}
	}
	return 0;
}

int APIENTRY InitStat(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->ReState();
	return 1;
}

int	APIENTRY InitStr(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->ReStateOne(0);
	return 1;
}
int	APIENTRY InitSta(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->ReStateOne(1);
	return 1;
}
int	APIENTRY InitDex(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->ReStateOne(2);
	return 1;
}
int	APIENTRY InitInt(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->ReStateOne(3);
	return 1;
}

int APIENTRY SetQuestState(NPCDIALOG_INFO* pInfo, int nQuest, int n)
{
	return __SetQuestState(pInfo->GetPcId(), nQuest, n);
}
int APIENTRY SetQuest(NPCDIALOG_INFO* pInfo, int nQuest)
{
	__SetQuest(pInfo->GetPcId(), nQuest);
	return 1;
}

int APIENTRY CreateItem(NPCDIALOG_INFO* pInfo, DWORD dwID, int nCount)
{
	__CreateItem(pInfo->GetPcId(), dwID, nCount);
	return 1;
}

int APIENTRY AddGold(NPCDIALOG_INFO* pInfo, int nGold)
{
	__AddGold(pInfo->GetPcId(), nGold);
	return 1;
}

int	APIENTRY RemoveGold(NPCDIALOG_INFO* pInfo, int nGold)
{
	__RemoveGold(pInfo->GetPcId(), nGold);
	return 1;
}

int APIENTRY GetPlayerGold(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetGold();
}

int APIENTRY GetLocalEventState(NPCDIALOG_INFO* pInfo, int nEvent)
{
	return g_eLocal.GetState(nEvent);
}

int APIENTRY AddExp(NPCDIALOG_INFO* pInfo, int nExp)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	if (pUser->AddExperience(nExp, TRUE, FALSE))
		pUser->LevelUpSetting();

	pUser->AddSetExperience(pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel);
	pUser->AddDefinedText(TID_GAME_REAPEXP);
	return 1;
}

int APIENTRY IsParty(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	int f = (pParty != NULL && pParty->IsMember(pUser->m_idPlayer));
	return f;
}

int APIENTRY IsPartyMaster(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	int f = (pParty != NULL && pParty->IsLeader(pUser->m_idPlayer));
	return f;
}

int APIENTRY GetPartyNum(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	int nSize = 0;
	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty)
		nSize = pParty->GetSizeofMember();
	return nSize;
}

int	APIENTRY GetPartyLevel(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	int nLevel = 0;
	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty)
		nLevel = pParty->GetLevel();
	return nLevel;
}

int APIENTRY IsPartyGuild(NPCDIALOG_INFO* pInfo)
{
	int f = 0;
	u_long	aidPlayer[MAX_PTMEMBER_SIZE];
	int	nSize = 0;
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty)
	{
		nSize = pParty->m_nSizeofMember;
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
			aidPlayer[i] = pParty->m_aMember[i].m_uPlayerId;
	}
	else
	{
		f = 1;
	}

	for (int i = 0; i < nSize; i++)
	{
		CUser* pUsertmp = g_UserMng.GetUserByPlayerID(aidPlayer[i]);
		if (IsValidObj(pUsertmp))
		{
			if (pUsertmp->GetGuild())
			{
				f = 1;
				break;
			}
#ifndef __FL_FIX_PROPER_GUILD_COOLDOWN
			CTime tCurrent = CTime::GetCurrentTime();
			if (tCurrent < pUsertmp->m_tGuildMember)
			{
				f = 2;
				break;
			}
#endif
		}
		else
		{
			f = 1;
			break;
		}
	}
	return f;
}

int APIENTRY IsGuild(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	int f = (pGuild && pGuild->IsMember(pUser->m_idPlayer));
	return f;
}

int APIENTRY IsGuildMaster(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	int f = (pGuild && pGuild->IsMaster(pUser->m_idPlayer));
	return f;
}

int APIENTRY CreateGuild(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	int nSize = 0;
	GUILD_MEMBER_INFO	info[MAX_PTMEMBER_SIZE];

	CParty* pParty = g_PartyMng.GetParty(pUser->m_idparty);
	if (pParty && pParty->IsLeader(pUser->m_idPlayer))
	{
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
		{
			CUser* pUsertmp = g_UserMng.GetUserByPlayerID(pParty->GetPlayerId(i));
			if (IsValidObj(pUsertmp))
			{
				info[nSize].idPlayer = pUsertmp->m_idPlayer;
#if __VER < 11 // __SYS_PLAYER_DATA
				info[nSize].nLevel = pUsertmp->GetLevel();
				info[nSize].nJob = pUsertmp->GetJob();
				info[nSize].dwSex = pUsertmp->GetSex();
#endif	// __SYS_PLAYER_DATA
				nSize++;
			}
			else
			{
				// error	- offline
				return 0;
			}
		}
	}
	else
	{
		// error	-
		return 0;
	}

	g_DPCoreClient.SendCreateGuild(info, nSize, "");

	return 1;
}

int APIENTRY DestroyGuild(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	CGuild* pGuild = g_GuildMng.GetGuild(pUser->m_idGuild);
	if (pGuild && pGuild->IsMember(pUser->m_idPlayer))
	{
		if (pGuild->IsMaster(pUser->m_idPlayer))
		{
			RunScriptFunc rsf;
			rsf.wFuncType = FUNCTYPE_DESTROYGUILD;
			pUser->AddRunScriptFunc(rsf);
		}
		else
			pUser->AddDefinedText(TID_GAME_COMDELNOTKINGPIN);
	}
	else
		pUser->AddDefinedText(TID_GAME_COMNOHAVECOM);

	return 1;
}

int APIENTRY IsPlayerSkillPoint(NPCDIALOG_INFO* pInfo)
{
	return 0;
}

int APIENTRY ChangeJob(NPCDIALOG_INFO* pInfo, int nJob)
{
	int v1 = nJob;
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	if (pUser->IsBaseJob() && pUser->GetLevel() != MAX_JOB_LEVEL)
	{
		Error("Error ChangeJob : 1");
		return 0;
	}
	if (pUser->IsExpert() && pUser->GetLevel() != (MAX_JOB_LEVEL + MAX_EXP_LEVEL))
	{
		Error("Error ChangeJob : 2");
		return 0;
	}
	if (pUser->IsPro() && (pUser->GetLevel() != MAX_GENERAL_LEVEL || pUser->GetExpPercent() < 9999))
	{
		Error("Error ChangeJob : 3");
		return 0;
	}
	if (pUser->IsHero() && pUser->GetLevel() != MAX_LEGEND_LEVEL)
	{
		Error("Error ChangeJob : 4");
		return 0;
	}

	if (pUser->AddChangeJob(v1))
	{
		pUser->AddSetChangeJob(v1);
		g_UserMng.AddNearSetChangeJob((CMover*)pUser, v1, &pUser->m_aJobSkill[MAX_JOB_SKILL]);
		// #dlvr
		g_dpDBClient.SendLogLevelUp((CUser*)pUser, 4);

		g_dpDBClient.SendUpdatePlayerData(pUser);

		pUser->RefreshGrilledEel();

#ifdef __FL_RECORD_BOOK
		CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::HP, pUser->GetMaxHitPoint(), false);
#endif
	}
	else
	{
		Error("Error ChangeJob!");
	}
	return 1;
}

int APIENTRY GetPlayerSex(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetSex();
}
int APIENTRY GetItems(NPCDIALOG_INFO* pInfo, DWORD dwID)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetAvailableItemNum(dwID);
}
int APIENTRY RemoveAllItem(NPCDIALOG_INFO* pInfo, DWORD dwID)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->RemoveAllItem(dwID);
	return 1;
}
int APIENTRY PlaySound(NPCDIALOG_INFO* pInfo, LPCTSTR szMusic)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->AddPlaySound2(szMusic);
	return 1;
}

int APIENTRY DropQuestItem(NPCDIALOG_INFO* pInfo, DWORD dwID, DWORD dwProb)
{
	int v1 = dwID, v2 = dwProb;
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	if ((int)(xRandom(1000)) < v2)
	{
		ItemProp* pItemProp = prj.GetItemProp(v1);
		if (pItemProp)
		{
			CWorld* pWorld = pUser->GetWorld();
			if (pWorld)
			{
				CItemElem* pItemElem = new CItemElem;
				pItemElem->m_dwItemId = v1;
				CItem* pItem = new CItem;
				pItem->m_pItemBase = pItemElem;
				if (pItemElem->m_dwItemId == 0) Error("DropQuestItem SetIndex\n");
				pItem->SetIndex(D3DDEVICE, pItemElem->m_dwItemId);
				pItem->m_idHolder = pUser->m_idPlayer;
				pItem->m_idOwn = pUser->GetId();
				pItem->m_dwDropTime = timeGetTime();
				CMover* pMover = prj.GetMover(pInfo->GetNpcId());
				D3DXVECTOR3 vPos = pMover->GetPos();
				vPos.x += (xRandomF(2.0f) - 1.0f);
				vPos.z += (xRandomF(2.0f) - 1.0f);
				pItem->SetPos(vPos);
				pItem->SetAngle((float)(xRandom(360)));
				if (pWorld->ADDOBJ(pItem, TRUE, pUser->GetLayer()) == TRUE)
				{
					return 1;
				}
				else
				{
					SAFE_DELETE(pItem);
					return 0;
				}
			}
		}
		else
		{
			Error("DropQuestItem, %d null prop", v1);
		}
	}
	return 0;
}

int APIENTRY ExpQuestEffect(NPCDIALOG_INFO* pInfo, DWORD dwID, BOOL bNPC)
{
	int v1 = dwID, v2 = bNPC;
	CCtrl* pCtrl = (v2 ? prj.GetCtrl(pInfo->GetNpcId()) : prj.GetCtrl(pInfo->GetPcId()));
	g_UserMng.AddCreateSfxObj(pCtrl, v1);
	return 1;
}

int APIENTRY RemoveItems(NPCDIALOG_INFO* pInfo, DWORD dwID, int nCount)
{
	int v1 = dwID, v2 = nCount;
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	pUser->RemoveAvailableItem(v1, v2);
	return 0;
}

int APIENTRY Run(NPCDIALOG_INFO* pInfo, LPCTSTR szKey, int n)
{
	int v1 = n;
	CString string = szKey;

	CMover* pMover = prj.GetMover(pInfo->GetNpcId());
	int nResult = 0;
	pMover->m_pNpcProperty->RunDialog(string, &nResult, v1, pInfo->GetNpcId(), pInfo->GetPcId(), 0);

	return nResult;
}

int APIENTRY GetQuestId(NPCDIALOG_INFO* pInfo)
{
	return pInfo->GetQuest();
}
int APIENTRY GetLang(NPCDIALOG_INFO* pInfo)
{
	return ::GetLanguage();
}


int	APIENTRY QuerySetPlayerName(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	if (pUser->IsAuthHigher(AUTH_GAMEMASTER) || pUser->IsMode(QUERYSETPLAYERNAME_MODE))
	{
		RunScriptFunc rsf;
		rsf.wFuncType = FUNCTYPE_QUERYSETPLAYERNAME;
		pUser->AddRunScriptFunc(rsf);
		return 1;
	}
	return 0;
}

int APIENTRY Trace(NPCDIALOG_INFO* pInfo, LPCTSTR szMsg)
{
	TRACE("%s\n", szMsg);
	return 1;
}


int APIENTRY GetPlayerExpPercent(NPCDIALOG_INFO* pInfo)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());
	return pUser->GetExpPercent();
}

int APIENTRY SetLevel(NPCDIALOG_INFO* pInfo, int nSetLevel)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	pUser->m_nLevel = nSetLevel;
	pUser->m_nExp1 = 0;

	pUser->LevelUpSetting();
	pUser->AddSetExperience(pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel);

	return 0;
}

int APIENTRY AddGPPoint(NPCDIALOG_INFO* pInfo, int nAddGPPoint)
{
	CUser* pUser = prj.GetUser(pInfo->GetPcId());

	pUser->m_nRemainGP += nAddGPPoint;
	pUser->AddSetGrowthLearningPoint(pUser->m_nRemainGP);
	g_dpDBClient.SendLogLevelUp(pUser, 1);

	return 0;
}

Functions* GetDialogFunctions()
{
	return &g_functions;
}

void InitDialogFunctions()
{
	InitPredefineKey();

	g_functions.SayName = SayName;
	g_functions.Speak = Speak;
	g_functions.SpeakName = SpeakName;
	g_functions.GetVal = GetVal;
	g_functions.GetSrcId = GetSrcId;
	g_functions.GetDstId = GetDstId;
	g_functions.NpcId = NpcId;
	g_functions.PcId = PcId;
	g_functions.GetGValue_ = GetGValue_;
	g_functions.SetGValue = SetGValue;
	g_functions.AddGValue = AddGValue;
	g_functions.AddKey = AddKey;
	g_functions.AddCondKey = AddCondKey;
	g_functions.AddAnswer = AddAnswer;
	g_functions.RemoveQuest = RemoveQuest;
	g_functions.RemoveAllKey = RemoveAllKey;
	g_functions.SayQuest = SayQuest;
	g_functions.BeginQuest = BeginQuest;
	g_functions.EndQuest = EndQuest;
	g_functions.AddQuestKey = AddQuestKey;
	g_functions.CreateMover = CreateMover;
	g_functions.CreateChar = CreateChar;
	g_functions.RemoveMover = RemoveMover;
	g_functions.GetDay = GetDay;
	g_functions.GetMin = GetMin;
	g_functions.GetHour = GetHour;
	g_functions.SetTimer = SetTimer;
	g_functions.IsTimeOut = IsTimeOut;
	g_functions.LaunchQuest = LaunchQuest;
	g_functions.GetEmptyInventoryNum = GetEmptyInventoryNum;
	g_functions.GetQuestState = GetQuestState;
	g_functions.IsSetQuest = IsSetQuest;
	g_functions.SetMark = SetMark;
	g_functions.GoMark = GoMark;
	g_functions.RandomIndex = RandomIndex;
	g_functions.RemoveKey = RemoveKey;
	g_functions.Say = Say;
	g_functions.EndSay = EndSay;
	g_functions.Random = Random;
	g_functions.Exit = Exit;
	g_functions.SetScriptTimer = SetScriptTimer;
	g_functions.Replace = Replace;
	g_functions.ReplaceKey = ReplaceKey;
	g_functions.GetPlayerLvl = GetPlayerLvl;
	g_functions.GetPlayerJob = GetPlayerJob;
	g_functions.IsGuildQuest = IsGuildQuest;
	g_functions.GetGuildQuestState = GetGuildQuestState;
	g_functions.IsWormonServer = IsWormonServer;
	g_functions.PrintSystemMessage = PrintSystemMessage;
	g_functions.MonHuntStart = MonHuntStart;
	g_functions.MonHuntStartParty = MonHuntStartParty;
	g_functions.InitStat = InitStat;
	g_functions.InitStr = InitStr;
	g_functions.InitSta = InitSta;
	g_functions.InitDex = InitDex;
	g_functions.InitInt = InitInt;
	g_functions.SetQuestState = SetQuestState;
	g_functions.SetQuest = SetQuest;
	g_functions.CreateItem = CreateItem;
	g_functions.AddGold = AddGold;
	g_functions.RemoveGold = RemoveGold;
	g_functions.GetPlayerGold = GetPlayerGold;
	g_functions.GetLocalEventState = GetLocalEventState;
	g_functions.AddExp = AddExp;
	g_functions.IsParty = IsParty;
	g_functions.IsPartyMaster = IsPartyMaster;
	g_functions.GetPartyNum = GetPartyNum;
	g_functions.GetPartyLevel = GetPartyLevel;
	g_functions.IsPartyGuild = IsPartyGuild;
	g_functions.IsGuild = IsGuild;
	g_functions.IsGuildMaster = IsGuildMaster;
	g_functions.CreateGuild = CreateGuild;
	g_functions.DestroyGuild = DestroyGuild;
	g_functions.IsPlayerSkillPoint = IsPlayerSkillPoint;
	g_functions.ChangeJob = ChangeJob;
	g_functions.GetPlayerSex = GetPlayerSex;
	g_functions.GetItems = GetItems;
	g_functions.RemoveAllItem = RemoveAllItem;
	g_functions.PlaySound = PlaySound;
	g_functions.DropQuestItem = DropQuestItem;
	g_functions.ExpQuestEffect = ExpQuestEffect;
	g_functions.RemoveItems = RemoveItems;
	g_functions.Run = Run;
	g_functions.GetQuestId = GetQuestId;
	g_functions.GetLang = GetLang;
	g_functions.QuerySetPlayerName = QuerySetPlayerName;
	g_functions.Trace = Trace;
	g_functions.GetPlayerExpPercent = GetPlayerExpPercent;
	g_functions.SetLevel = SetLevel;
	g_functions.AddGPPoint = AddGPPoint;
}
