#include "stdafx.h"
#include "ThreadMng.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "User.h"
#include "WorldServer.h"
#include "GlobalTime.h"
#include "party.h"
#include "dpdatabaseclient.h"
#include "guildquest.h"

#include "eveschool.h"
extern	CGuildCombat	g_GuildCombatMng;

#ifdef __FL_EQUALIZED_MODE
BOOL bPresetsLoaded = FALSE;
#endif

#ifdef __FL_VENDORS
BOOL bVendorsLoaded = FALSE;
#endif

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER

#if __VER >= 12 // __RANGDA_0521
#include "rangda.h"
#endif	// __RANGDA_0521

#if __VER >= 14 // __INSTANCE_DUNGEON
#include "InstanceDungeonParty.h"
#endif // __INSTANCE_DUNGEON

#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "spevent.h"
#include "guild.h"
#include "guildwar.h"
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	char				g_szDBAddr[16];
extern	char				g_szCoreAddr[16];
extern	CGuildMng			g_GuildMng;

extern	CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CUserMng			g_UserMng;
extern  DWORD				g_tmCurrent;
//extern	time_t	g_tCurrent;
extern	char				g_sHeartbeat[32];
extern	CPartyMng			g_PartyMng;

extern  BOOL LoadAIScript();		// aimonster2.cpp

float	r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;

#if defined(_DEBUG) || defined(__TESTSERVER)
BOOL g_bProfiling = TRUE;
#else
BOOL g_bProfiling = FALSE;
#endif

void LogPerformance(DWORD dwCurTick);

//////////////////////////////////////////////////////////////////////////////////////////////////
// CTimeout
//////////////////////////////////////////////////////////////////////////////////////////////////

CTimeout::CTimeout(DWORD dwInterval, DWORD dwFirstTimeout)
{
	m_dwInterval = dwInterval;
	m_endTime = timeGetTime() + dwFirstTimeout;
}

DWORD CTimeout::Over(DWORD dwCurr)
{
	if (dwCurr > m_endTime)
		return (dwCurr - m_endTime);
	return 0;
}
void CTimeout::Reset(DWORD dwCurr, DWORD dwTimeout)
{
	if (dwTimeout > m_dwInterval)
		m_endTime = dwCurr;
	else
		m_endTime = dwCurr + (m_dwInterval - dwTimeout);
}
BOOL CTimeout::TimeoutReset(DWORD dwCurr)
{
	DWORD dwTimeout = Over(dwCurr);
	if (dwTimeout > 0)
	{
		Reset(dwCurr, dwTimeout);
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void LogPerformance(DWORD dwCurTick)
{
	static int nFrame = 0;
	static DWORD dwPrev = 0;
	static DWORD dwElapsed = 0;

	++nFrame;
	DWORD dwTick = dwCurTick - dwPrev;
	dwPrev = dwCurTick;
	dwElapsed += dwTick;

	if (dwTick > 100)
	{
		SetLogInfo(LOGTYPE_WARN1, "warning - tick:%d", dwTick);

		if (g_bProfiling)
		{
			float sum = r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8 + r9 + r10 + r11 + r12 + 1.0f;

			char szBuffer[256];
			sprintf(szBuffer,
				"%d, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f, %2.1f\n",
				dwTick,
				(r1 * 100.0f / sum),
				(r2 * 100.0f / sum),
				(r3 * 100.0f / sum),
				(r4 * 100.0f / sum),
				(r5 * 100.0f / sum),
				(r6 * 100.0f / sum),
				(r7 * 100.0f / sum),
				(r8 * 100.0f / sum),
				(r9 * 100.0f / sum),
				(r10 * 100.0f / sum),
				(r11 * 100.0f / sum),
				(r12 * 100.0f / sum));

			SetLogInfo(LOGTYPE_WARN2, szBuffer);
			OutputDebugString(szBuffer);
		}
	}

	if (dwElapsed > 1000)
	{
		SetLogInfo(LOGTYPE_CCU, "CCU: %d", g_UserMng.GetCount());

#ifdef __FL_VENDORS
		SetLogInfo(LOGTYPE_VENDOR, "Vendors: %d", prj.m_mapPlayerShops.size());
#endif

		if (g_bProfiling)
			SetLogInfo(LOGTYPE_PERFOMANCE, "frame: %d tick: %d", nFrame, dwTick);

		dwElapsed = 0; // -= 1000
		nFrame = 0;

		r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = r9 = r10 = r11 = r12 = 0;
		SetLogInfo(LOGTYPE_REDRAW, "");

		SetLogInfo(LOGTYPE_DUNGEONS, "Dungeons: %d", CInstanceDungeonHelper::GetInstance()->m_nIDLayerNum);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// CRunObject
//////////////////////////////////////////////////////////////////////////////////////////////////
CRunObject::CRunObject()
{
	m_hRunObject = (HANDLE)0;
	m_hClose = (HANDLE)0;
}

CRunObject::~CRunObject()
{
	Close();
}

BOOL CRunObject::Init(void)
{
	if (LoadAIScript() == FALSE)
		return FALSE;


	m_hClose = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD dwThreadId;
	m_hRunObject = chBEGINTHREADEX(NULL, 0, _Run, (LPVOID)this, 0, &dwThreadId);

	if (!g_DPSrvr.StartServer((u_short)(g_uKey + PN_WORLDSRVR), TRUE))
	{
		OutputDebugString("Can't start server.");
		return FALSE;
	}
	if (!g_DPCoreClient.Run(g_szCoreAddr, PN_CORESRVR + 0, g_uKey))
	{
		OutputDebugString("Can't connect to core server.");
		return FALSE;
	}
	if (!g_dpDBClient.ConnectToServer(g_szDBAddr, PN_DBSRVR_1, TRUE))
	{
		OutputDebugString("Can't connect to database server.");
		return FALSE;
	}
	if (g_eLocal.GetState(ENABLE_GUILD_INVENTORY))
		g_dpDBClient.SendQueryGuildBank();
	if (g_eLocal.GetState(EVE_WORMON))
		g_dpDBClient.SendQueryGuildQuest();

#ifdef __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer(g_uIdofMulti, NULL);
#else	// __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer(g_uIdofMulti, 0);
#endif	// __INVALID_LOGIN_0320
	if (g_eLocal.GetState(EVE_GUILDCOMBAT) && !g_GuildCombatMng.LoadScript("GuildCombat.txt"))
	{
		OutputDebugString("GuildCombat.txt not found");
		return FALSE;
	}

#ifdef __FL_EQUALIZED_MODE
	if (!bPresetsLoaded)
	{
		g_dpDBClient.SendGetTmpPresets();
		bPresetsLoaded = TRUE;
	}
#endif

#ifdef __FL_VENDORS
	if (!bVendorsLoaded)
	{
		g_dpDBClient.SendGetVendors();
		bVendorsLoaded = TRUE;
	}
#endif

	return TRUE;
}

void CRunObject::Close(void)
{
	CLOSE_THREAD(m_hRunObject, m_hClose);
}

u_int CRunObject::_Run(LPVOID pParam)
{
	CRunObject* pRunObject = (CRunObject*)pParam;
	pRunObject->Run();
	return 0;
}

void CRunObject::Run(void)
{
	HANDLE hHeartbeat;
	if (!(hHeartbeat = CreateEvent(NULL, FALSE, FALSE, g_sHeartbeat)))
		hHeartbeat = OpenEvent(EVENT_MODIFY_STATE, FALSE, g_sHeartbeat);
	if (NULL == hHeartbeat)
		Error("MAPI_E_NOT_ENOUGH_RESOURCES");
	DWORD dwTickHearbeat = timeGetTime() + SEC(2);

	CTimeout	timeoutObject(67, 0);
	CTimeout	timeoutReadAgent(67, 20);
	CTimeout	timeoutReadCore(67, 40);
	CTimeout	timeoutReadTrans(67, 50);
	CTimeout	timeoutReadAccount(67, 60);
	CTimeout	timeoutRespawn(1000, 200);
	CTimeout	timeout(1000, 600);
	CTimeout	timeoutCallTheRoll(MIN(1), 70);

	while (WaitForSingleObject(m_hClose, 1) != WAIT_OBJECT_0)
	{
		{
			g_tmCurrent = timeGetTime();
			g_tCurrent = time(NULL);

			if (g_tmCurrent > dwTickHearbeat)
			{
				if (SetEvent(hHeartbeat) == FALSE)
					Error("Heartbeat SetEvent failed, GetLastError: %d", ::GetLastError());

				dwTickHearbeat = g_tmCurrent + SEC(2);
			}

			if (timeoutObject.TimeoutReset(g_tmCurrent))
			{
				g_WorldMng.Process();
			}

			if (timeoutReadAgent.TimeoutReset(g_tmCurrent))
			{
				g_DPSrvr.ReceiveMessage();
			}

			if (timeoutReadCore.TimeoutReset(g_tmCurrent))
			{
				g_DPCoreClient.ReceiveMessage();
			}

			if (timeoutReadTrans.TimeoutReset(g_tmCurrent))
			{
				g_dpDBClient.ReceiveMessage();
			}

#ifdef __EVENTLUA_KEEPCONNECT
			if (timeoutReadAccount.TimeoutReset(g_tmCurrent))
				prj.m_EventLua.KeepConnectEventProcess();
#endif // __EVENTLUA_KEEPCONNECT

			if (timeoutRespawn.TimeoutReset(g_tmCurrent))
			{
				u_long uRespawned = g_WorldMng.Respawn();
				SetLogInfo(LOGTYPE_RESPAWN, "Respawn:%d Object:%d", uRespawned, g_WorldMng.GetObjCount());
			}

			if (timeout.TimeoutReset(g_tmCurrent))
			{
				g_GameTimer.Compute();
				CGuildQuestProcessor::GetInstance()->Process();
				CPartyQuestProcessor::GetInstance()->Process();
#ifdef __FL_BATTLE_PASS
				CBattlePass::GetInstance()->Process();
#endif
#ifdef __FL_FFA
				CFFAManager::GetInstance()->Process();
#endif
#ifdef __FL_TDM
				CTDMManager::GetInstance()->Process();
#endif

				if (g_eLocal.GetState(EVE_GUILDCOMBAT))
					g_GuildCombatMng.Process();

				if (g_eLocal.GetState(EVE_SECRETROOM))
					CSecretRoomMng::GetInstance()->Process();

				CInstanceDungeonParty::GetInstance()->Process();

				g_PartyMng.PartyMapInfo();

#if __VER >= 12 // __RANGDA_0521

				if (g_eLocal.GetState(EVE_RANGDA))
					CRangdaController::Instance()->OnTimer();

#endif	// __RANGDA_0521
#ifdef __EVENTLUA_SPAWN
				prj.m_EventLua.EventSpawnProcess();
#endif // __EVENTLUA_SPAWN
			}

			if (timeoutCallTheRoll.TimeoutReset(g_tmCurrent))
				CCreateMonster::GetInstance()->ProcessRemoveMonster();
		}

		LogPerformance(timeGetTime());

	}
	CLOSE_HANDLE(m_hClose);
	CLOSE_HANDLE(hHeartbeat);
}

CRunObject* CRunObject::GetInstance(void)
{
	static CRunObject	sRunObject;
	return &sRunObject;
}

void CRunObject::ToggleProfiling()
{
	g_bProfiling = !g_bProfiling;
}