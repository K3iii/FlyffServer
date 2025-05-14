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

#ifdef __FL_RECORD_BOOK
#include "RecordParameters.h"
#endif

extern const int MAX_TASKBAR;
extern const int MAX_APPLETTASKBAR;
extern const int MAX_ITEMTASKBAR;
extern const char NullStr[2];

#if __VER >= 13
#include "honor.h"
#endif

#if defined( __VERIFY_PLAYER ) || defined( __PROVIDE ) || defined( __S0707_ITEM_CONV ) || defined(__RECOVER0816)
#define	MAX_QUERY_SIZE	1024 * 64
#include "resource.h"
BOOL	CALLBACK	VerifyPlayerDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
#endif	// __VERIFY_PLAYER OR __PROVIDE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

void CDbManager::SavePlayer(CQuery* qry, CQuery* pQueryLog, CMover* pMover, char* szQuery)
{
	szQuery[0] = '\0';

	///////// actMover
	char cActMover[50] = { 0, };
	sprintf(cActMover, "%d,%d", pMover->m_pActMover->m_dwState, pMover->m_pActMover->m_dwStateFlag);

	///////// Skill
	char JobSkill[1024] = { 0, };
	char LicenseSkill[1024] = { 0, };

	///////// JobLv
	char JobLv[500] = { 0, };
	SaveJobLv(pMover, JobLv);

	///////// Quest
	char QuestCnt[3072] = { 0, };
	char m_aCompleteQuest[1024] = { 0, };
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	char CheckedQuest[100] = { 0, };
	SaveQuest(pMover, QuestCnt, m_aCompleteQuest, CheckedQuest);
#else // __IMPROVE_QUEST_INTERFACE
	SaveQuest(pMover, QuestCnt, m_aCompleteQuest);
#endif // __IMPROVE_QUEST_INTERFACE

	///////// Inventory
	ItemContainerStruct	icsInventory, icsBank[3];
	SaveContainer(&pMover->m_Inventory, &icsInventory);
	for (int k = 0; k < 3; ++k)
		SaveContainer(&pMover->m_Bank[k], &icsBank[k]);

	///////// CardCube
	char Card[2] = "$";
	char sCardIndex[2] = "$";
	char sCardObjIndex[2] = "$";
	char Cube[2] = "$";
	char sCubeIndex[2] = "$";
	char sCubeObjIndex[2] = "$";

	///////// TaskBar
	char AppletTaskBar[MAX_APPLETTASKBAR] = { 0, };
	char ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };
	char SkillTaskBar[1024] = { 0, };
	SaveTaskBar(pMover, AppletTaskBar, ItemTaskBar, SkillTaskBar);


	char szSMTime[3072] = { 0, };
	SaveSMCode(pMover, szSMTime);

	char szSkillInfluence[7500] = { 0, };
	SaveSkillInfluence(pMover, szSkillInfluence);

	PutExpLog(pQueryLog, pMover, (pMover->m_dwPeriodicTick == 0 ? 'O' : 'P'));


	sprintf(szQuery, "{call CHARACTER_STR('U1','%07d','%02d','',"
		"?,?,?,?,?,?,?,?,?,?,?,"		// 1
		"?,?,?,?,?,?,?,?,?,?,?,"		// 2
		"?,?,?,?,?,?,?,?,?,?,"			// 3
		"?,?,?,?,?,?,?,?,?,?,?,"		// 4
		"?,?,?,?,?,?,"		// 5
		"?,?,?,?,?,?,?,?,?,?,"		// 6
		"?,?,?,?,?,?,?,?,?,?,?,"		// 7
//					  "?,?,?,?,?,?,?,?,?,?,?,"		// 8
"?,?,?,?,?,?,?,%7.0f,%7.0f,"		// 8
"%7.0f,?,?"
",?,?,?"
#if __VER >= 13
",?"
#endif
#ifdef __LAYER_1015
",?"
#endif	// __LAYER_1015
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
",?"
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __FL_BATTLE_PASS
",?"
",?"
",?"
#endif
#ifdef __MODEL_HIDEFASH
",?"
#endif


")}", pMover->m_idPlayer, g_appInfo.dwSys, pMover->m_vReturnPos.x, pMover->m_vReturnPos.y, pMover->m_vReturnPos.z);		// +3

	char szAuthority[2] = "F";
	int nSlot = 0;
	int nSex = pMover->GetSex();
	int nExpertLv = 0;
	int nRemainLP = 0;
	int nTotalPlayTime = 0;
	DWORD dwGold = pMover->GetGold();
	int i = 0;
	int j = -1;
	__int64 nExp2 = 0;
	DWORD dwSkillPoint = 0;

	////////////////////////////////////////////////////////////////////////////////
	int MAX_SAVEPARAM = 76;
	MAX_SAVEPARAM += 3;
#if __VER >= 8 // __S8_PK
	MAX_SAVEPARAM += 1;
#endif // __VER >= 8 // __S8_PK
#if __VER >= 8 // __CSC_VER8_5
	MAX_SAVEPARAM += 2;
#endif // __CSC_VER8_5

	MAX_SAVEPARAM += 3;

#if __VER >= 13
	MAX_SAVEPARAM += 1;
#endif
#ifdef __LAYER_1015
	MAX_SAVEPARAM += 1;	// m_nLayer
#endif	// __LAYER_1015
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	MAX_SAVEPARAM += 1;	// m_aCheckedQuest
#endif // __IMPROVE_QUEST_INTERFACE
#ifdef __FL_BATTLE_PASS
	MAX_SAVEPARAM += 3;
#endif
#ifdef __MODEL_HIDEFASH
	MAX_SAVEPARAM += 1;
#endif

	int nNumSkill = 0;
	int nSlaughter = 0;
	SQLINTEGER cbLen = SQL_NTS;
	BOOL* bOK = new BOOL[MAX_SAVEPARAM];

	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, pMover->m_szName, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nSlot, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwWorldID, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwIndex, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vPos.x, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vPos.y, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vPos.z, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 32, 0, pMover->m_szCharacterKey, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwSkinSet, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwHairMesh, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwHairColor, 0, 0);
	// 1
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwHeadMesh, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nSex, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vScale.x, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwMotion, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_fAngle, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nHitPoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nManaPoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nFatiguePoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwRideItemIdx, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dwGold, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nJob, 0, 0);
	// 2
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, cActMover, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nStr, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nSta, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nDex, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nInt, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nLevel, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &pMover->m_nExp1, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &nExp2, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 500, 0, JobSkill, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 500, 0, LicenseSkill, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 500, 0, JobLv, 0, &cbLen);
	// 3
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nExpertLv, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_idMarkingWorld, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vMarkingPos.x, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vMarkingPos.y, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &pMover->m_vMarkingPos.z, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nRemainGP, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nRemainLP, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nFxp, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nFxp, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 3072, 0, QuestCnt, 0, &cbLen);
	// 4
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 1, 0, szAuthority, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwMode, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_idparty, 0, 0);

#ifdef __JEFF_9_20
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwMute, 0, 0);
#else	// __JEFF_9_20
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwFlyTime, 0, 0);
#endif	// __JEFF_9_20
	DWORD dwState = pMover->m_RTMessenger.GetState();
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dwState, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nTotalPlayTime, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1980, 0, Card, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, sCardIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, sCardObjIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1980, 0, Cube, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, sCubeIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, sCubeObjIndex, 0, &cbLen);
#ifndef __INVENTORY_168_SLOTS
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 6940, 0, icsInventory.szItem, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 345, 0, icsInventory.szIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 345, 0, icsInventory.szObjIndex, 0, &cbLen);
#else
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szItem, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1000, 0, icsInventory.szIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1000, 0, icsInventory.szObjIndex, 0, &cbLen);
#endif
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 3100, 0, AppletTaskBar, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 6885, 0, ItemTaskBar, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 225, 0, SkillTaskBar, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, &pMover->m_UserTaskBar.m_nActionPoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 4290, 0, icsBank[pMover->m_nSlot].szItem, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, icsBank[pMover->m_nSlot].szIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, icsBank[pMover->m_nSlot].szObjIndex, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwGoldBank[pMover->m_nSlot], 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nFuel, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_tmAccFuel, 0, 0);
	// 7
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2560, 0, szSMTime, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7500, 0, szSkillInfluence, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dwSkillPoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1024, 0, m_aCompleteQuest, 0, &cbLen);
#ifndef __INVENTORY_168_SLOTS
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsInventory.szExt, 0, &cbLen);
#else
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szExt, 0, &cbLen);
#endif
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsBank[pMover->m_nSlot].szExt, 0, &cbLen);
#ifndef __INVENTORY_168_SLOTS
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7800, 0, icsInventory.szPiercing, 0, &cbLen);
#else
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szPiercing, 0, &cbLen);
#endif
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7800, 0, icsBank[pMover->m_nSlot].szPiercing, 0, &cbLen);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_dwReturnWorldID, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nSkillPoint, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nSkillLevel, 0, 0);
#ifndef __INVENTORY_168_SLOTS
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsInventory.szPet, 0, &cbLen);
#else
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, icsInventory.szPet, 0, &cbLen);
#endif
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, icsBank[pMover->m_nSlot].szPet, 0, &cbLen);
	DWORD dwPetId = pMover->GetPetId();
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dwPetId, 0, 0);

	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &pMover->m_nHonor, 0, 0);
#ifdef __LAYER_1015
	int nLayer = pMover->GetLayer();
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nLayer, 0, 0);
#endif	// 
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, CheckedQuest, 0, &cbLen);

#ifdef __FL_BATTLE_PASS
	int m_nLastBattlePassID = pMover->GetLastBattlePassID();
	int m_nBattlePassLevel = pMover->GetBattlePassLevel();
	int m_nBattlePassExp = pMover->GetBattlePassExp();

	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_nLastBattlePassID, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_nBattlePassLevel, 0, 0);
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_nBattlePassExp, 0, 0);
#endif
#ifdef __MODEL_HIDEFASH
	int nHideCoat = 0;
	for (int z = 0; z < 6; ++z)
	{
		int c = (pMover->m_bHideFashion[z] ? 1 : 0);
		nHideCoat |= (c << z);
	}
	bOK[++j] = qry->BindParameter(++i, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nHideCoat, 0, 0);
#endif

	for (i = 0; i < j; ++i)
	{
		if (bOK[i] == FALSE)
		{
			char szBuffer[128];
			sprintf(szBuffer, "SavePlayer(%s) - %dth bindparam error - %d", pMover->m_szName, i + 1, ::GetCurrentThreadId());
			WriteLog(szBuffer);


			WriteLog("try DB Reconnect... - ThreadId : %d", ::GetCurrentThreadId());
			qry->DisConnect();
			if (qry->Connect(3, qry->DBName, qry->DBId, qry->DBPass))
				WriteLog("DB Reconnect Success! - ThreadId : %d", ::GetCurrentThreadId());
			else
				WriteLog("DB Reconnect Failed! - ThreadId : %d", ::GetCurrentThreadId());
			qry->Clear();


			SAFE_DELETE_ARRAY(bOK);
			return;
		}
	}

	if (qry->Exec(szQuery) == FALSE)
	{
		WriteLog("SavePlayer(%s) - Exec RETURN FALSE, ThreadID : %d", pMover->m_szName, ::GetCurrentThreadId());
		SAFE_DELETE_ARRAY(bOK);
		return;
	}

	if (qry->Fetch())
	{
		int nError = qry->GetInt("fError");
		if (nError != 1)
		{
			WriteLog("SavePlayer(%s) - fError:%d", pMover->m_szName, nError);
			SAFE_DELETE_ARRAY(bOK);
			return;
		}
	}
	SaveHonor(qry, pMover->m_idPlayer, pMover->m_aHonorTitle, szQuery);
	SaveSkill(qry, pMover->m_idPlayer, pMover->m_aJobSkill, pMover->m_abUpdateSkill, szQuery);

#ifdef __FL_CONSUMABLES_STORAGE
#ifndef __FL_PACKED_ITEMS
	SaveChest(pMover, qry);
#else
	if (SaveChest(pMover, qry))
		SavePackages(qry, &pMover->m_Chest, pMover->m_idPlayer, PRT_CHEST);
#endif
#endif

#ifdef __FL_SWITCH_EQ
	SaveSwitches(pMover, qry);
#endif

#ifdef __FL_RECORD_BOOK
	SaveRecords(pMover, qry);
#endif

#ifdef __FL_MONSTER_ALBUM
	SaveMonsterAlbum(pMover, qry);
#endif

#ifdef __FL_PACKED_ITEMS
	SavePackages(qry, &pMover->m_Inventory, pMover->m_idPlayer, PRT_INVENTORY);
	SavePackages(qry, &pMover->m_Bank[pMover->m_nSlot], pMover->m_idPlayer, PRT_BANK);
#endif

	for (i = 0; i < 3; ++i)
	{
		if (pMover->m_idPlayerBank[i] != 0 && i != pMover->m_nSlot)
		{
			ADDBANK_QUERYINFO info("U1");
			info.idPlayer = pMover->m_idPlayerBank[i];
			info.dwGoldBank = pMover->m_dwGoldBank[i];
			info.pszBank = icsBank[i].szItem;
			info.pszBankIndex = icsBank[i].szIndex;
			info.pszObjIndexBank = icsBank[i].szObjIndex;
			info.pszExtBank = icsBank[i].szExt;
			info.pszPirecingBank = icsBank[i].szPiercing;
			info.pszBankPet = icsBank[i].szPet;

			DBQryAddBankSave(szQuery, info);

			{
				SQLINTEGER cbLen = SQL_NTS;
				BOOL bOK[7];
				bOK[0] = qry->BindParameter(1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 4290, 0, (char*)info.pszBank, 0, &cbLen);
				bOK[1] = qry->BindParameter(2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, (char*)info.pszBankIndex, 0, &cbLen);
				bOK[2] = qry->BindParameter(3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 215, 0, (char*)info.pszObjIndexBank, 0, &cbLen);
				bOK[3] = qry->BindParameter(4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &info.dwGoldBank, 0, 0);
				bOK[4] = qry->BindParameter(5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, (char*)info.pszExtBank, 0, &cbLen);
#if __VER >= 15 // __PETVIS
				bOK[5] = qry->BindParameter(6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 7800, 0, (char*)info.pszPirecingBank, 0, &cbLen);
#else // __PETVIS
				bOK[5] = qry->BindParameter(6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2000, 0, (char*)info.pszPirecingBank, 0, &cbLen);
#endif // __PETVIS
				bOK[6] = qry->BindParameter(7, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 2689, 0, (char*)info.pszBankPet, 0, &cbLen);
			}

			if (FALSE == qry->Exec(szQuery))
			{
				SAFE_DELETE_ARRAY(bOK);
				return;
			}

#ifdef __FL_PACKED_ITEMS
			SavePackages(qry, &pMover->m_Bank[i], pMover->m_idPlayerBank[i], PRT_BANK);
#endif
		}
	}

	SAFE_DELETE_ARRAY(bOK);
}

void CDbManager::UpdateGuildBankUpdate(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr							ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	CItemContainer<CItemElem>	GuildBank;
	int							nGoldGuild;
	int							nGuildId = 0;
	BYTE						cbUpdateContribution;
	u_long						idPlayer, idLogPlayer;
	int							nMode;		// GUILD_PUT_ITEM, GUILD_GET_ITEM, GUILD_PUT_PENYA, GUILD_GET_PENYA, GUILD_CLOAK
	DWORD						dwItemId;
	short						nItemCount;
	int							nAbilityOption;
	SERIALNUMBER	iSerialNumber;
	DWORD						dwPenya;
	GuildBank.SetItemContainer(ITYPE_ITEM, MAX_GUILDBANK);

	ar >> nGuildId;
	ar >> nGoldGuild;
	GuildBank.Serialize(ar);
	ar >> cbUpdateContribution;
	ar >> idPlayer;
	ar >> nMode;
	ar >> dwItemId;
	ar >> nAbilityOption;
	ar >> iSerialNumber;
	ar >> nItemCount;
	ar >> dwPenya;

	idLogPlayer = idPlayer;
	if (cbUpdateContribution != 1)
		idPlayer = 0;

	ItemContainerStruct icsGuildBank;
	SaveContainer(&GuildBank, &icsGuildBank);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"GUILD_BANK_STR 'U1','%06d','%02d','%d','%s','%s','%s','%07d','%s','%s','%s'",
		nGuildId, g_appInfo.dwSys, nGoldGuild, icsGuildBank.szIndex, icsGuildBank.szObjIndex, icsGuildBank.szItem, idPlayer, icsGuildBank.szExt, icsGuildBank.szPiercing, icsGuildBank.szPet);

	if (FALSE == pQuery->Exec(szQuery))
	{
		Error("update guild bank:%s ", szQuery);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	SavePackages(pQuery, &GuildBank, nGuildId, PRT_GUILDBANK);
#endif

#ifndef __NOLOG
	GUILDLOG_QUERYINFO qi("L2");
	qi.idGuild = nGuildId;
	qi.idPlayer = idLogPlayer;
	qi.nGuildGold = nGoldGuild;

	switch (nMode)
	{
	case GUILD_PUT_ITEM:
		qi.pszState = "A";
		qi.nItem = dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_ITEM:
		qi.pszState = "D";
		qi.nItem = dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_CLOAK:
		qi.pszState = "C";
		qi.nItem = dwItemId;
		qi.nAbilityOption = nAbilityOption;
		qi.iUniqueNo = iSerialNumber;
		qi.nItem_count = nItemCount;
		break;
	case GUILD_GET_PENYA:
		qi.pszState = "O";
		qi.nItem = dwPenya;
		break;
	case GUILD_QUERY_REMOVE_GUILD_BANK:
		qi.pszState = "R";
		break;
	}

	qi.pszGuildBank = icsGuildBank.szItem;

	DBQryGuildLog(szQuery, qi);
	if (FALSE == pQueryLog->Exec(szQuery))
	{

		return;
	}
#endif // __NOLOG

}


#if __VER >= 13
void	CDbManager::SaveHonor(CQuery* qry, u_long uidPlayer, int* aHonor, char* szQuery)
{
	int	aHonorEtc[50] = { 0, };
	int	aHonorItem[50] = { 0, };
	int	aHonorMonster[50] = { 0, };

	int nEtc = 0, nItem = 0, nMonster = 0;
	int nType = 0;
	int nCurrentTitleCount = CTitleManager::Instance()->m_nCurrentTitleCount;
	for (int i = 0; i < nCurrentTitleCount; i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if (nType == HI_HUNT_MONSTER)
		{
			aHonorMonster[nMonster] = aHonor[i];
			nMonster++;
		}
		else if (nType == HI_USE_ITEM)
		{
			aHonorItem[nItem] = aHonor[i];
			nItem++;
		}
		else
		{
			aHonorEtc[nEtc] = aHonor[i];
			nEtc++;
		}
	}

	sprintf(szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 1,
		aHonorEtc[0], aHonorEtc[1], aHonorEtc[2], aHonorEtc[3], aHonorEtc[4], aHonorEtc[5], aHonorEtc[6], aHonorEtc[7], aHonorEtc[8], aHonorEtc[9],
		aHonorEtc[10], aHonorEtc[11], aHonorEtc[12], aHonorEtc[13], aHonorEtc[14], aHonorEtc[15], aHonorEtc[16], aHonorEtc[17], aHonorEtc[18], aHonorEtc[19],
		aHonorEtc[20], aHonorEtc[21], aHonorEtc[22], aHonorEtc[23], aHonorEtc[24], aHonorEtc[25], aHonorEtc[26], aHonorEtc[27], aHonorEtc[28], aHonorEtc[29],
		aHonorEtc[30], aHonorEtc[31], aHonorEtc[32], aHonorEtc[33], aHonorEtc[34], aHonorEtc[35], aHonorEtc[36], aHonorEtc[37], aHonorEtc[38], aHonorEtc[39],
		aHonorEtc[40], aHonorEtc[41], aHonorEtc[42], aHonorEtc[43], aHonorEtc[44], aHonorEtc[45], aHonorEtc[46], aHonorEtc[47], aHonorEtc[48], aHonorEtc[49]);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	sprintf(szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 2,
		aHonorItem[0], aHonorItem[1], aHonorItem[2], aHonorItem[3], aHonorItem[4], aHonorItem[5], aHonorItem[6], aHonorItem[7], aHonorItem[8], aHonorItem[9],
		aHonorItem[10], aHonorItem[11], aHonorItem[12], aHonorItem[13], aHonorItem[14], aHonorItem[15], aHonorItem[16], aHonorItem[17], aHonorItem[18], aHonorItem[19],
		aHonorItem[20], aHonorItem[21], aHonorItem[22], aHonorItem[23], aHonorItem[24], aHonorItem[25], aHonorItem[26], aHonorItem[27], aHonorItem[28], aHonorItem[29],
		aHonorItem[30], aHonorItem[31], aHonorItem[32], aHonorItem[33], aHonorItem[34], aHonorItem[35], aHonorItem[36], aHonorItem[37], aHonorItem[38], aHonorItem[39],
		aHonorItem[40], aHonorItem[41], aHonorItem[42], aHonorItem[43], aHonorItem[44], aHonorItem[45], aHonorItem[46], aHonorItem[47], aHonorItem[48], aHonorItem[49]);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	sprintf(szQuery,
		"usp_Master_Update @serverindex='%02d',@m_idPlayer='%07d',@sec=%d,@c01=%d,@c02=%d,@c03=%d,@c04=%d,@c05=%d,@c06=%d,@c07=%d,@c08=%d,@c09=%d,@c10=%d,@c11=%d,@c12=%d,@c13=%d,@c14=%d,@c15=%d,@c16=%d,@c17=%d,@c18=%d,@c19=%d,@c20=%d,@c21=%d,@c22=%d,@c23=%d,@c24=%d,@c25=%d,@c26=%d,@c27=%d,@c28=%d,@c29=%d,@c30=%d,@c31=%d,@c32=%d,@c33=%d,@c34=%d,@c35=%d,@c36=%d,@c37=%d,@c38=%d,@c39=%d,@c40=%d,@c41=%d,@c42=%d,@c43=%d,@c44=%d,@c45=%d,@c46=%d,@c47=%d,@c48=%d,@c49=%d,@c50=%d",
		g_appInfo.dwSys, uidPlayer, 3,
		aHonorMonster[0], aHonorMonster[1], aHonorMonster[2], aHonorMonster[3], aHonorMonster[4], aHonorMonster[5], aHonorMonster[6], aHonorMonster[7], aHonorMonster[8], aHonorMonster[9],
		aHonorMonster[10], aHonorMonster[11], aHonorMonster[12], aHonorMonster[13], aHonorMonster[14], aHonorMonster[15], aHonorMonster[16], aHonorMonster[17], aHonorMonster[18], aHonorMonster[19],
		aHonorMonster[20], aHonorMonster[21], aHonorMonster[22], aHonorMonster[23], aHonorMonster[24], aHonorMonster[25], aHonorMonster[26], aHonorMonster[27], aHonorMonster[28], aHonorMonster[29],
		aHonorMonster[30], aHonorMonster[31], aHonorMonster[32], aHonorMonster[33], aHonorMonster[34], aHonorMonster[35], aHonorMonster[36], aHonorMonster[37], aHonorMonster[38], aHonorMonster[39],
		aHonorMonster[40], aHonorMonster[41], aHonorMonster[42], aHonorMonster[43], aHonorMonster[44], aHonorMonster[45], aHonorMonster[46], aHonorMonster[47], aHonorMonster[48], aHonorMonster[49]);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}


}

#endif

void CDbManager::SaveSkill(CQuery* qry, u_long uidPlayer, LPSKILL aJobSkill, LPBYTE abUpdateSkill, char* szQuery)
{
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (abUpdateSkill[i] == 0)
			continue;

		sprintf(szQuery,
			"uspLearnSkill @serverindex='%02d',@pPlayerID='%07d',@pSkillID=%d,@pSkillLv=%d,@pSkillPosition=%d",
			g_appInfo.dwSys, uidPlayer, aJobSkill[i].dwSkill, aJobSkill[i].dwLevel, i);

		if (FALSE == qry->Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
			return;
		}
	}
}

void CDbManager::SaveJobLv(CMover* pMover, char* szJobLv)
{
	char OneJobLv[16] = { 0, };
	for (int i = 0; i < MAX_JOB; i++)
	{
		sprintf(OneJobLv, "%d/", 0);
		strncat(szJobLv, OneJobLv, sizeof(OneJobLv));
	}
	strncat(szJobLv, NullStr, sizeof(NullStr));
}

void CDbManager::SaveQuest(CMover* pMover, char* szQuestCnt, char* szm_aCompleteQuest, char* szCheckedQuest)
{
	char sPerQuest[128];
	char OneCompleteQuest[20] = { 0, };
	char OneCheckedQuest[20] = { 0, };

	for (int i = 0; i < pMover->m_nQuestSize; i++)
	{
		sprintf(sPerQuest, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d/",
			pMover->m_aQuest[i].m_wId, pMover->m_aQuest[i].m_nState, pMover->m_aQuest[i].m_wTime,
			pMover->m_aQuest[i].m_nKillNPCNum[0], pMover->m_aQuest[i].m_nKillNPCNum[1],
			pMover->m_aQuest[i].m_bPatrol, pMover->m_aQuest[i].m_bReserve2, pMover->m_aQuest[i].m_bReserve3, pMover->m_aQuest[i].m_bReserve4,
			pMover->m_aQuest[i].m_bReserve5, pMover->m_aQuest[i].m_bReserve6, pMover->m_aQuest[i].m_bReserve7, pMover->m_aQuest[i].m_bReserve8
		);
		strncat(szQuestCnt, sPerQuest, sizeof(sPerQuest));
	}

	for (int i = 0; i < pMover->m_nCompleteQuestSize; i++)
	{
		sprintf(OneCompleteQuest, "%d/", pMover->m_aCompleteQuest[i]);
		strncat(szm_aCompleteQuest, OneCompleteQuest, sizeof(OneCompleteQuest));
	}

	strcat(szQuestCnt, NullStr);
	strcat(szm_aCompleteQuest, NullStr);

	for (int i = 0; i < pMover->m_nCheckedQuestSize; ++i)
	{
		sprintf(OneCheckedQuest, "%d/", pMover->m_aCheckedQuest[i]);
		strncat(szCheckedQuest, OneCheckedQuest, sizeof(OneCheckedQuest));
	}
	strcat(szCheckedQuest, NullStr);
}

void CDbManager::SaveContainer(CItemContainer<CItemElem  >* pContainer, ItemContainerStruct* pStruct)
{
	ItemStruct	is;
	char Onem_apIndex[10] = { 0, };
	char sPerObjIndex[16] = { 0, };
	BOOL bExtInven = FALSE;
	BOOL bPirecingInven = FALSE;
	BOOL bPet = FALSE;

	for (DWORD ch = 0; ch < pContainer->m_dwItemMax; ch++)
	{
		SaveOneItem(&pContainer->m_apItem[ch], &is);
		strncat(pStruct->szItem, is.szItem, sizeof(is.szItem));
		strncat(pStruct->szExt, is.szExt, sizeof(is.szExt));
		strncat(pStruct->szPiercing, is.szPiercing, sizeof(is.szPiercing));
		sprintf(Onem_apIndex, "%d/", pContainer->m_apIndex[ch]);
		strncat(pStruct->szIndex, Onem_apIndex, sizeof(Onem_apIndex));
		sprintf(sPerObjIndex, "%d/", pContainer->m_apItem[ch].m_dwObjIndex);
		strcat(pStruct->szObjIndex, sPerObjIndex);
		strncat(pStruct->szPet, is.szPet, sizeof(is.szPet));

		if (0 < pContainer->m_apItem[ch].m_dwKeepTime
			|| 0 != pContainer->m_apItem[ch].GetRandomOptItemId()
			|| FALSE != pContainer->m_apItem[ch].m_bTranformVisPet
			)
			bExtInven = TRUE;
		if (pContainer->m_apItem[ch].IsPiercedItem())
			bPirecingInven = TRUE;
		if (pContainer->m_apItem[ch].m_pPet)
			bPet = TRUE;
	}

	strncat(pStruct->szItem, NullStr, sizeof(NullStr));
	strncat(pStruct->szIndex, NullStr, sizeof(NullStr));
	strcat(pStruct->szObjIndex, NullStr);
	if (bExtInven == FALSE)
		*pStruct->szExt = '\0';
	if (bPirecingInven == FALSE)
		*pStruct->szPiercing = '\0';
	if (bPet == FALSE)
		*pStruct->szPet = '\0';
	strcat(pStruct->szExt, NullStr);
	strcat(pStruct->szPiercing, NullStr);
	strcat(pStruct->szPet, NullStr);
}


void CDbManager::SaveTaskBar(CMover* pMover, char* szAppletTaskBar, char* szItemTaskBar, char* szSkillTaskBar)
{
	char OneAppletTaskBar[MAX_TASKBAR] = { 0, };
	char OneItemTaskBar[MAX_TASKBAR] = { 0, };
	char OneSkillTaskBar[256] = { 0, };
	for (int ch = 0; ch < MAX_SLOT_APPLET; ch++)
	{
		if (pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut != SHORTCUT_NONE)
		{
			sprintf(OneAppletTaskBar, "%d,%d,%d,%d,%d,%d,%d,%d", ch,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwData,
				pMover->m_UserTaskBar.m_aSlotApplet[ch].m_nLocation
			);

			if (pMover->m_UserTaskBar.m_aSlotApplet[ch].m_dwShortcut == SHORTCUT_CHAT)
				SetStrDBFormat(OneAppletTaskBar, pMover->m_UserTaskBar.m_aSlotApplet[ch].m_szString);

			strncat(szAppletTaskBar, OneAppletTaskBar, sizeof(OneAppletTaskBar));
			strcat(szAppletTaskBar, "/");
		}
	}
	strcat(szAppletTaskBar, NullStr);

	for (int ch = 0; ch < MAX_SLOT_ITEM_COUNT; ch++)
	{
		for (int j = 0; j < MAX_SLOT_ITEM; j++)
		{
			if (pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut != SHORTCUT_NONE)
			{
				sprintf(OneItemTaskBar, "%d,%d,%d,%d,%d,%d,%d,%d,%d", ch, j,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwType,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwIndex, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwUserId, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwData,
					pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_nLocation
				);
				if (pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_dwShortcut == SHORTCUT_CHAT)
					SetStrDBFormat(OneItemTaskBar, pMover->m_UserTaskBar.m_aSlotItem[ch][j].m_szString);

				strncat(szItemTaskBar, OneItemTaskBar, sizeof(OneItemTaskBar));
				strcat(szItemTaskBar, "/");
			}
		}
	}
	strcat(szItemTaskBar, NullStr);

	for (int ch = 0; ch < MAX_SLOT_QUEUE; ch++)
	{
		if (pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut != SHORTCUT_NONE)
		{
			sprintf(OneSkillTaskBar, "%d,%d,%d,%d,%d,%d,%d,%d/", ch,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwShortcut, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwType,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwIndex, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwUserId, pMover->m_UserTaskBar.m_aSlotQueue[ch].m_dwData,
				pMover->m_UserTaskBar.m_aSlotQueue[ch].m_nLocation
			);
			strncat(szSkillTaskBar, OneSkillTaskBar, sizeof(OneSkillTaskBar));
		}
	}
	strcat(szSkillTaskBar, NullStr);
}

void CDbManager::SaveSMCode(CMover* pMover, char* szszSMTime)
{
	BOOL bSMSave = FALSE;
	for (int ch = 0; ch < SM_MAX; ++ch)
	{
		if (0 < pMover->m_dwSMTime[ch])
		{
			bSMSave = TRUE;
		}
		char OneSMTime[256] = { 0, };
		if (ch == SM_MAX_HP50)
			sprintf(OneSMTime, "%d,%d,", pMover->m_dwSMTime[ch], pMover->m_nPlusMaxHitPoint);
		else
			sprintf(OneSMTime, "%d,", pMover->m_dwSMTime[ch]);

		strcat(szszSMTime, OneSMTime);
	}
	strcat(szszSMTime, NullStr);

	if (bSMSave == FALSE)
	{
		memset(szszSMTime, 0, sizeof(szszSMTime));
	}
}

void CDbManager::SaveSkillInfluence(CMover* pMover, char* szszSkillInfluence)
{
#ifdef __BUFF_1107
	pMover->m_buffs.ToString(szszSkillInfluence);
#else	// __BUFF_1107
	char OneSkillInfluence[256] = { 0, };
	for (int ch = 0; ch < MAX_SKILLINFLUENCE; ++ch)
	{
		SKILLINFLUENCE* pSkillInfluenece = &pMover->m_SkillState.m_aSkillInfluence[ch];
		if (pSkillInfluenece->wType == BUFF_EQUIP)
			continue;
#if __VER >= 13 // __HOUSING
		if (pSkillInfluenece->wType == 0 && pSkillInfluenece->wID == 0
			&& pSkillInfluenece->dwLevel == 0 && pSkillInfluenece->tmCount == 0)
			break;
#endif // __HOUSING
		sprintf(OneSkillInfluence, "%d,%d,%d,%d/",
			pSkillInfluenece->wType, pSkillInfluenece->wID,
			pSkillInfluenece->dwLevel, pSkillInfluenece->tmCount);
		strcat(szszSkillInfluence, OneSkillInfluence);
	}
	strcat(szszSkillInfluence, NullStr);
#endif	// __BUFF_1107
}

void CDbManager::SavePlayTime(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	DWORD dwTime;
	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT);
	u_long idPlayer;
	arRead >> idPlayer;
	arRead >> dwTime;
	dwTime /= 1000;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "U3", idPlayer, g_appInfo.dwSys, '\0', "", dwTime);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s\r\n\t%s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szPlayer);
	}
	}

void CDbManager::DBQryAddBankSave(char* szSql, const ADDBANK_QUERYINFO& info)
{
	sprintf(szSql, "{call ADD_BANK_STR('U1','%07d','%02d', ?, ?, ?, ?, ?, ?, ?)}", info.idPlayer, g_appInfo.dwSys);
}

void CDbManager::SaveOneItem(CItemElem* pItemElem, ItemStruct* pItemStruct)
{
	char szPiercing[32] = { 0, };

	if (pItemElem->IsEmpty() == FALSE)
	{
		sprintf(pItemStruct->szItem, "%d,%d,%s,%d,%d,%d,%d,%d,%d,%d"
#ifdef __MODEL_CHANGE
			",%d"
#endif // 
#ifdef __WEAPON_RARITY
			",%d"
#endif // __WEAPON_RARITY

			"/",
			pItemElem->m_dwObjId, pItemElem->m_dwItemId,
			pItemElem->m_szItemText,
			pItemElem->m_nItemNum,
			pItemElem->m_nHitPoint,
			pItemElem->m_byFlag,
			pItemElem->GetSerialNumber(), pItemElem->GetOption(),
			pItemElem->m_bItemResist, pItemElem->m_nResistAbilityOption
#ifdef __MODEL_CHANGE
			, pItemElem->GetLook()
#endif
#ifdef __WEAPON_RARITY
			, pItemElem->m_nWeaponRarity
#endif // __WEAPON_RARITY
		);

		sprintf(pItemStruct->szExt, "%d,%I64d"
			",%d"
			"/", pItemElem->m_dwKeepTime, pItemElem->GetRandomOptItemId()
			, static_cast<int>(pItemElem->m_bTranformVisPet)
		);

		ItemProp* itemProp = prj.GetItemProp(pItemElem->m_dwItemId);
		if (itemProp && itemProp->IsUltimate())
		{
			sprintf(pItemStruct->szPiercing, "%d", pItemElem->GetUltimatePiercingSize());
			for (int nPirecing = 0; nPirecing < pItemElem->GetUltimatePiercingSize(); ++nPirecing)
			{
				sprintf(szPiercing, ",%d", pItemElem->GetUltimatePiercingItem(nPirecing));
				strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
			}
			if (pItemElem->GetPiercingSize() > 0)
			{
				sprintf(szPiercing, ",%d", pItemElem->GetPiercingSize());
				strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
			}
		}
		else
			sprintf(pItemStruct->szPiercing, "%d", pItemElem->GetPiercingSize());

		for (int nPirecing = 0; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing)
		{
			sprintf(szPiercing, ",%d", pItemElem->GetPiercingItem(nPirecing));
			strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
		}
		if (pItemElem->IsVisPet())
		{
			for (int nPirecing = 0; nPirecing < pItemElem->GetPiercingSize(); ++nPirecing)
			{
				sprintf(szPiercing, ",%d", pItemElem->GetVisKeepTime(nPirecing));
				strncat(pItemStruct->szPiercing, szPiercing, sizeof(szPiercing));
			}
		}
		strcat(pItemStruct->szPiercing, "/");

		// pet
		if (pItemElem->m_pPet)
		{
			sprintf(pItemStruct->szPet, "1,%d,%d,%d,%d,%d",
				pItemElem->m_pPet->GetKind(), pItemElem->m_pPet->GetLevel(),
				pItemElem->m_pPet->GetExp(), pItemElem->m_pPet->GetEnergy(),
				pItemElem->m_pPet->GetLife());
			for (int i = PL_D; i <= pItemElem->m_pPet->GetLevel(); i++)
			{
				char szAvailLevel[16] = { 0, };
				sprintf(szAvailLevel, ",%d", pItemElem->m_pPet->GetAvailLevel(i));
				strcat(pItemStruct->szPet, szAvailLevel);
			}
#ifdef __PET_1024
			char szTemp[MAX_PET_NAME_FMT + 1] = { 0, };
			char szFmt[MAX_PET_NAME_FMT] = { 0, };
			SetDBFormatStr(szFmt, MAX_PET_NAME_FMT, pItemElem->m_pPet->GetName());
			sprintf(szTemp, ",%s", szFmt);
			strcat(pItemStruct->szPet, szTemp);
#endif
			strcat(pItemStruct->szPet, "/");
		}
		else
		{
			sprintf(pItemStruct->szPet, "0/");
		}
	}
	else
	{
		// item
		ZeroMemory(pItemStruct->szItem, sizeof(pItemStruct->szItem));
		// ext
		sprintf(pItemStruct->szExt, "%d,%d,%d/", 0, 0, 0);
		// piercing
		sprintf(pItemStruct->szPiercing, "%d/", 0);
		// pet
		sprintf(pItemStruct->szPet, "0/");
	}
}

#ifdef __FL_CONSUMABLES_STORAGE
bool CDbManager::SaveChest(CMover* pMover, CQuery* qry)
{
	ItemContainerStruct icsChest;
	SaveContainer(&pMover->m_Chest, &icsChest);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspChestSave '%07d', '%02d', '%s', '%s', '%s', '%s', '%s', '%s'",
		pMover->m_idPlayer, g_appInfo.dwSys, icsChest.szItem, icsChest.szIndex, icsChest.szObjIndex, icsChest.szExt, icsChest.szPiercing, icsChest.szPet);

	if (!qry->Exec(szQuery))
	{
		Error("CDbManager::uspChestSave[Line:%u] Failed Exec, Query: %s", __LINE__, szQuery);
		return false;
	}

	return true;
}
#endif

#ifdef __FL_EQUALIZED_MODE
void CDbManager::SaveTemporalEquipment(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	DWORD dwWorldID;
	DWORD dwSex;
	int nJob;
	CItemContainer<CItemElem>tmpInventory;

	ar >> dwWorldID >> dwSex >> nJob;
	tmpInventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
	tmpInventory.Serialize(ar);

	ItemContainerStruct icsTemporal;
	SaveContainer(&tmpInventory, &icsTemporal);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspTemporalPresetsSave %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s'",
		dwWorldID, dwSex, nJob, icsTemporal.szItem, icsTemporal.szPet, icsTemporal.szExt, icsTemporal.szPiercing, icsTemporal.szIndex, icsTemporal.szObjIndex);

	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspTemporalPresetsSave[Line:%u] Failed Exec, Query: %s", __LINE__, szQuery);
		return;
	}
}
#endif

#ifdef __FL_VENDORS
void CDbManager::AddVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	// Receive data begin
	u_long idPlayer;
	DWORD dwWorldId;
	D3DXVECTOR3 vecPos;
	float fAngle;
	char szShopName[64] = { 0, };
	DWORD dwVendorTime;
	DWORD dwVendorItem;
	CItemContainer<CItemElem>tmpInventory;
	tmpInventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

	int nCost[MAX_VENDITEM] = { };
	int nPerinCost[MAX_VENDITEM] = { };

	ar >> idPlayer >> dwWorldId >> vecPos >> fAngle;
	ar >> dwVendorTime;
	ar >> dwVendorItem;
	ar.ReadString(szShopName, 64);
	tmpInventory.Serialize(ar);

	// Serialize prices
	int nItemsCount;
	ar >> nItemsCount;

	for (int i = 0; i < nItemsCount; i++)
	{
		BYTE nIndex;
		ar >> nIndex;

		ar >> nCost[nIndex];
		ar >> nPerinCost[nIndex];
	}

	CString strShopName = szShopName;
	strShopName.Replace("--", "");
	strShopName.Replace("'", "");
	strShopName.Replace("/*", "");
	strShopName.Replace("*/", "");
	strShopName.Replace(";", "");
	if (strShopName.IsEmpty())
		strShopName.Format("Vendor Shop");
	// Receive data end

	ItemContainerStruct icsInventory;
	SaveContainer(&tmpInventory, &icsInventory);

	char szPrice[1024] = { 0, };
	char szOneItemPrice[128] = { 0, };

	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		sprintf(szOneItemPrice, "%d,%d/", nCost[i], nPerinCost[i]);
		strcat(szPrice, szOneItemPrice);
	}
	strcat(szPrice, NullStr);


	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspOfflineVendorSave '%07d', '%02d', %d, %1.2f, %1.2f, %1.2f, %1.2f, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s'",
		idPlayer, g_appInfo.dwSys, dwWorldId, vecPos.x, vecPos.y, vecPos.z, fAngle, dwVendorTime, dwVendorItem, strShopName,
		icsInventory.szItem, icsInventory.szPet, icsInventory.szExt, icsInventory.szPiercing, icsInventory.szIndex, icsInventory.szObjIndex, szPrice);

	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspOfflineVendorSave[Line:%u] Failed Exec, Query: %s", __LINE__, szQuery);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	SavePackages(pQueryChar, &tmpInventory, idPlayer, PRT_VENDORS);
#endif
}
void CDbManager::UpdateVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	// Receive data begin
	u_long idPlayer;
	CItemContainer<CItemElem>tmpInventory;
	tmpInventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

	int nCost[MAX_VENDITEM] = { };
	int nPerinCost[MAX_VENDITEM] = { };

	ar >> idPlayer;
	tmpInventory.Serialize(ar);

	// Serialize prices
	int nItemsCount;
	ar >> nItemsCount;

	for (int i = 0; i < nItemsCount; i++)
	{
		BYTE nIndex;
		ar >> nIndex;

		ar >> nCost[nIndex];
		ar >> nPerinCost[nIndex];
	}
	// Receive data end

	ItemContainerStruct icsInventory;
	SaveContainer(&tmpInventory, &icsInventory);

	char szPrice[1024] = { 0, };
	char szOneItemPrice[128] = { 0, };

	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		sprintf(szOneItemPrice, "%d,%d/", nCost[i], nPerinCost[i]);
		strcat(szPrice, szOneItemPrice);
	}
	strcat(szPrice, NullStr);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspOfflineVendorUpdate '%07d', '%02d', '%s', '%s', '%s', '%s', '%s', '%s', '%s'",
		idPlayer, g_appInfo.dwSys, icsInventory.szItem, icsInventory.szPet, icsInventory.szExt, icsInventory.szPiercing, icsInventory.szIndex, icsInventory.szObjIndex, szPrice);

	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspOfflineVendorUpdate[Line:%u] Failed Exec, Query: %s", __LINE__, szQuery);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	SavePackages(pQueryChar, &tmpInventory, idPlayer, PRT_VENDORS);
#endif
}
void CDbManager::DeleteVendor(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idPlayer;
	ar >> idPlayer;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspOfflineVendorDelete '%07d', '%02d'", idPlayer, g_appInfo.dwSys);
	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspOfflineVendorDelete[Line:%u] Failed, Query: %s", __LINE__, szQuery);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	SavePackages(pQueryChar, nullptr, idPlayer, PRT_VENDORS);
#endif
}
#endif

#ifdef __FL_SWITCH_EQ
void CDbManager::SaveSwitches(CMover* pMover, CQuery* qry)
{
	auto vecSwitches = pMover->m_Switches.GetSwitches();

	char SwitchesNames[2048] = { 0, };
	char SwitchesParts[2048] = { 0, };
	char SwitchesOther[2048] = { 0, };

	char szTmp[65] = { 0, };

	for (auto& it : *vecSwitches)
	{
		SetDBFormatStr(szTmp, 65, it.szSwitchName);
		sprintf(szTmp, "%s/", szTmp);
		strcat(SwitchesNames, szTmp);

		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			sprintf(szTmp, "%d,", it.dwItem[i]);
			strncat(SwitchesParts, szTmp, sizeof(SwitchesParts));
		}

		sprintf(szTmp, "%d,", it.dwLooterPet);
		strncat(SwitchesOther, szTmp, sizeof(SwitchesOther));

		sprintf(szTmp, "%d,", it.dwPet);
		strncat(SwitchesOther, szTmp, sizeof(SwitchesOther));

		sprintf(szTmp, "%d,", it.dwRegen);
		strncat(SwitchesOther, szTmp, sizeof(SwitchesOther));


		SwitchesParts[strlen(SwitchesParts) - 1] = '/';
		SwitchesOther[strlen(SwitchesOther) - 1] = '/';
	}

	strcat(SwitchesNames, NullStr);
	strcat(SwitchesParts, NullStr);
	strcat(SwitchesOther, NullStr);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspSwitchesSave '%07d', '%02d', '%s', '%s', '%s'",
		pMover->m_idPlayer, g_appInfo.dwSys, SwitchesNames, SwitchesParts, SwitchesOther);

	if (!qry->Exec(szQuery))
	{
		Error("CDbManager::uspSwitchesSave[Line:%u] Failed Exec, Query: %s", __LINE__, szQuery);
		return;
	}
}
#endif

#ifdef __FL_RECORD_BOOK
void CDbManager::SaveRecords(CMover* pMover, CQuery* qry)
{
	CString strQuery;
	strQuery.Format("uspRecordsSave @iserverindex='%02d', @im_idPlayer='%07d'", g_appInfo.dwSys, pMover->m_idPlayer);

	CString str;
	for (int i = 0; i < RecordType::MAX_RECORDTYPE; i++)
	{
		str.Format(", @irc_%d='%d,%d,%I64d'", i, pMover->m_arrayRecords[i].nLevel, pMover->m_arrayRecords[i].nJob, pMover->m_arrayRecords[i].n64Value);
		strQuery += str;
	}

	if (!qry->Exec(strQuery))
	{
		Error("Cannot execute %s", strQuery);
		return;
	}
}
#endif

#ifdef __FL_MONSTER_ALBUM
void CDbManager::SaveMonsterAlbum(CMover* pMover, CQuery* qry)
{
	auto pMapMonsters = pMover->m_MonsterAlbum.GetMonsterProgress();
	auto pBossCards = pMover->m_MonsterAlbum.GetBossCards();

	char stringMonsters[2048] = { 0, };
	char stringBossCards[256] = { 0, };
	char szTmp[65] = { 0, };

	for (auto& it : *pMapMonsters)
	{
		sprintf(szTmp, "%d,%d,", it.first, it.second);
		strncat(stringMonsters, szTmp, sizeof(stringMonsters));

		stringMonsters[strlen(stringMonsters) - 1] = '/';
	}

	for (auto& it : *pBossCards)
	{
		sprintf(szTmp, "%d,", it);
		strncat(stringBossCards, szTmp, sizeof(stringBossCards));

		stringBossCards[strlen(stringBossCards) - 1] = '/';
	}

	strcat(stringMonsters, NullStr);
	strcat(stringBossCards, NullStr);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspMonsterAlbumSave '%07d', '%02d', '%s', '%s'",
		pMover->m_idPlayer, g_appInfo.dwSys, stringMonsters, stringBossCards);

	if (!qry->Exec(szQuery))
	{
		Error("Cannot execute %s", szQuery);
		return;
	}
}
#endif