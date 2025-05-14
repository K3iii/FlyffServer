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

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif // __GUILD_HOUSE

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	CMyTrace			g_MyTrace;

#define	VERIFYSTRING( lpString, lpszPlayer )	\
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

void CDbManager::LogItem(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	char chAction[2];
	char lpszPlayer1[36], lpszPlayer2[36], lpszItemText[36];
	arRead.ReadString(chAction, 2);
	char chswitch = chAction[0];
	arRead.ReadString(lpszPlayer1, 36);
	arRead.ReadString(lpszPlayer2, 36);

	CString strPlayer1 = lpszPlayer1;
	CString strPlayer2 = lpszPlayer2;
	strPlayer1.Replace('\'', ' ');
	lstrcpy(lpszPlayer1, (LPCSTR)strPlayer1);
	strPlayer2.Replace('\'', ' ');
	lstrcpy(lpszPlayer2, (LPCSTR)strPlayer2);

	DWORD dwWorldID;
	arRead >> dwWorldID;

	int iSeed1, iSeed2;
	int iReMainder1, iReMainder2;
	arRead >> iSeed1 >> iSeed2;

	char szQuery[QUERY_SIZE] = { 0, };

	switch (chswitch)
	{
	case 'T':
	{
		int nTradeID = GetTradeNo();

		call_uspLoggingTrade(qry, 0, nTradeID, dwWorldID);
		arRead >> iReMainder1 >> iReMainder2;
		u_long idPlayer1, idPlayer2;
		int nLevel1, nLevel2, nJob1, nJob2;
		arRead >> idPlayer1 >> nLevel1 >> nJob1;
		arRead >> idPlayer2 >> nLevel2 >> nJob2;
		char lpAddr1[100] = { 0, };
		char lpAddr2[100] = { 0, };
		arRead.ReadString(lpAddr1, 100);
		arRead.ReadString(lpAddr2, 100);

		call_uspLoggingTrade(qry, 1, nTradeID, 0, idPlayer1, iSeed1, lpAddr1, nLevel1, nJob1);
		call_uspLoggingTrade(qry, 1, nTradeID, 0, idPlayer2, iSeed2, lpAddr2, nLevel2, nJob2);
		u_long uSize1, uSize2;
		arRead >> uSize1 >> uSize2;
		for (DWORD i = 0; i < uSize1; i++)
		{
			memset(lpszItemText, 0, sizeof(lpszItemText));
			SERIALNUMBER iSerialNumber;
			short nItemNum = 0;
			int	  nAbilityOption = 0;
			int	  nItemResist = 0;
			int	  nResistAbilityOption = 0;
			int   nNegudo = 0;
			DWORD dwItemId = 0;
			arRead >> dwItemId;
			arRead >> iSerialNumber;
			arRead.ReadString(lpszItemText, 36);
			CString strItem = lpszItemText;
			strItem.Replace('\'', ' ');
			lstrcpy(lpszItemText, (LPCSTR)strItem);
			arRead >> nItemNum;
			arRead >> nAbilityOption;
			arRead >> nItemResist;
			arRead >> nResistAbilityOption;
			arRead >> nNegudo;

			LogItemInfo aLogItem;
			::memset(&(aLogItem), 0, sizeof(&(aLogItem)));
			aLogItem.SendName = lpszPlayer1;
			aLogItem.RecvName = lpszPlayer2;
			aLogItem.WorldId = dwWorldID;
			aLogItem.Gold = iReMainder1 + iSeed1;
			aLogItem.Gold2 = iReMainder1 + iSeed1;
			aLogItem.ItemNo = iSerialNumber;
			//aLogItem.ItemName = lpszItemText;
			_tcsncpy(aLogItem.szItemName, lpszItemText, 30);

			aLogItem.itemNumber = nItemNum;
			aLogItem.nAbilityOption = nAbilityOption;
			aLogItem.nItemResist = nItemResist;
			aLogItem.nResistAbilityOption = nResistAbilityOption;
			aLogItem.Negudo = nNegudo;
			aLogItem.Gold_1 = iReMainder2;
			aLogItem.Action = chAction;

			arRead >> aLogItem.m_dwKeepTime;
			arRead >> aLogItem.nPiercedSize;

			int	k = 0;

			for (k = 0; k < aLogItem.nPiercedSize; k++)
				arRead >> aLogItem.adwItemId[k];
			arRead >> aLogItem.nUMPiercedSize;
			for (k = 0; k < aLogItem.nUMPiercedSize; k++)
				arRead >> aLogItem.adwUMItemId[k];

			arRead >> aLogItem.m_iRandomOptItemId;

			arRead >> aLogItem.nPetKind;
			arRead >> aLogItem.nPetLevel;
			arRead >> aLogItem.dwPetExp;
			arRead >> aLogItem.wPetEnergy;
			arRead >> aLogItem.wPetLife;
			arRead >> aLogItem.nPetAL_D;
			arRead >> aLogItem.nPetAL_C;
			arRead >> aLogItem.nPetAL_B;
			arRead >> aLogItem.nPetAL_A;
			arRead >> aLogItem.nPetAL_S;

			DBQryNewItemLog(szQuery, aLogItem);

			if (FALSE == qry->Exec(szQuery))
			{
				WriteLog("%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2);

				return;
			}

			call_uspLoggingTrade(qry, 2, nTradeID, 0, idPlayer2, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_iRandomOptItemId);
		}
		for (u_long i = 0; i < uSize2; i++)
		{
			memset(lpszItemText, 0, sizeof(lpszItemText));
			SERIALNUMBER iSerialNumber;
			short nItemNum = 0;
			int	  nAbilityOption = 0;
			int	  nItemResist = 0;
			int	  nResistAbilityOption = 0;
			int   nNegudo = 0;
			DWORD dwItemId = 0;
			arRead >> dwItemId;
			arRead >> iSerialNumber;
			arRead.ReadString(lpszItemText, 36);
			CString strItem = lpszItemText;
			strItem.Replace('\'', ' ');
			lstrcpy(lpszItemText, (LPCSTR)strItem);
			arRead >> nItemNum;
			arRead >> nAbilityOption;
			arRead >> nItemResist;
			arRead >> nResistAbilityOption;
			arRead >> nNegudo;

			LogItemInfo aLogItem;
			::memset(&(aLogItem), 0, sizeof(&(aLogItem)));
			aLogItem.SendName = lpszPlayer2;
			aLogItem.RecvName = lpszPlayer1;
			aLogItem.WorldId = dwWorldID;
			aLogItem.Gold = iReMainder2 + iSeed2;
			aLogItem.Gold2 = iReMainder2 + iSeed2;
			aLogItem.ItemNo = iSerialNumber;
			_tcsncpy(aLogItem.szItemName, lpszItemText, 30);

			aLogItem.itemNumber = nItemNum;
			aLogItem.nAbilityOption = nAbilityOption;
			aLogItem.nItemResist = nItemResist;
			aLogItem.nResistAbilityOption = nResistAbilityOption;
			aLogItem.Negudo = nNegudo;
			aLogItem.Gold_1 = iReMainder1;
			aLogItem.Action = chAction;

			arRead >> aLogItem.m_dwKeepTime;

			arRead >> aLogItem.nPiercedSize;
			for (int i = 0; i < aLogItem.nPiercedSize; i++)
				arRead >> aLogItem.adwItemId[i];
			arRead >> aLogItem.nUMPiercedSize;
			for (int i = 0; i < aLogItem.nUMPiercedSize; i++)
				arRead >> aLogItem.adwUMItemId[i];

			arRead >> aLogItem.m_iRandomOptItemId;


#if __VER >= 9 // __PET_0410
			arRead >> aLogItem.nPetKind;
			arRead >> aLogItem.nPetLevel;
			arRead >> aLogItem.dwPetExp;
			arRead >> aLogItem.wPetEnergy;
			arRead >> aLogItem.wPetLife;
			arRead >> aLogItem.nPetAL_D;
			arRead >> aLogItem.nPetAL_C;
			arRead >> aLogItem.nPetAL_B;
			arRead >> aLogItem.nPetAL_A;
			arRead >> aLogItem.nPetAL_S;
#endif // __PET_0410

			DBQryNewItemLog(szQuery, aLogItem);

			if (FALSE == qry->Exec(szQuery))
			{
				WriteLog("%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2);

				return;
			}

			call_uspLoggingTrade(qry, 2, nTradeID, 0, idPlayer1, 0, "", 0, 0, dwItemId, iSerialNumber, nItemNum, nAbilityOption, nItemResist, nResistAbilityOption, aLogItem.m_iRandomOptItemId);

		}
		if (iSeed1 > 0)
		{
			LogItemInfo aLogItem;
			::memset(&(aLogItem), 0, sizeof(&(aLogItem)));
			aLogItem.SendName = lpszPlayer1;
			aLogItem.RecvName = lpszPlayer2;
			aLogItem.WorldId = dwWorldID;
			aLogItem.Gold = iReMainder1 + iSeed1;
			aLogItem.Gold2 = iReMainder1;
			//aLogItem.ItemName = "SEED";
			_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
			aLogItem.itemNumber = iSeed1;
			aLogItem.Gold_1 = iReMainder2;
			aLogItem.Action = chAction;
			DBQryNewItemLog(szQuery, aLogItem);

			if (FALSE == qry->Exec(szQuery))
			{
				WriteLog("%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2);

				return;
			}
		}
		if (iSeed2 > 0)
		{
			LogItemInfo aLogItem;
			::memset(&(aLogItem), 0, sizeof(&(aLogItem)));
			aLogItem.SendName = lpszPlayer2;
			aLogItem.RecvName = lpszPlayer1;
			aLogItem.WorldId = dwWorldID;
			aLogItem.Gold = iReMainder2 + iSeed2;
			aLogItem.Gold2 = iReMainder2;
			//aLogItem.ItemName = "SEED";
			_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
			aLogItem.itemNumber = iSeed2;
			aLogItem.Gold_1 = iReMainder1;
			aLogItem.Action = chAction;
			DBQryNewItemLog(szQuery, aLogItem);

			if (FALSE == qry->Exec(szQuery))
			{
				WriteLog("%s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2);

				return;
			}
		}
		break;
	}
	default:
	{
		memset(lpszItemText, 0, sizeof(lpszItemText));
		SERIALNUMBER	ItemNo;
		DWORD nItemNum = 0;
		int nNegudo = 0;
		int	  nAbilityOption = 0;
		DWORD dwGold_1 = 0;
		int nSlot = 0;
		int nSlot1 = 0;
		int nItemResist = 0;
		int nResistAbilityOption = 0;

		arRead >> ItemNo;
		arRead >> nNegudo;
		arRead.ReadString(lpszItemText, 36);
		CString strItem = lpszItemText;
		strItem.Replace('\'', ' ');
		lstrcpy(lpszItemText, (LPCSTR)strItem);
		arRead >> nItemNum;
		arRead >> nAbilityOption;
		arRead >> dwGold_1;
		arRead >> nSlot;
		arRead >> nSlot1;
		arRead >> nItemResist;
		arRead >> nResistAbilityOption;

		LogItemInfo aLogItem;
		::memset(&(aLogItem), 0, sizeof(&(aLogItem)));
		aLogItem.SendName = lpszPlayer1;
		aLogItem.RecvName = lpszPlayer2;
		aLogItem.WorldId = dwWorldID;
		aLogItem.Gold = iSeed1;
		aLogItem.Gold2 = iSeed2;
		aLogItem.ItemNo = ItemNo;
		//aLogItem.ItemName = lpszItemText;
		_tcsncpy(aLogItem.szItemName, lpszItemText, 30);

		aLogItem.itemNumber = nItemNum;
		aLogItem.nAbilityOption = nAbilityOption;
		aLogItem.nItemResist = nItemResist;
		aLogItem.nResistAbilityOption = nResistAbilityOption;
		aLogItem.Negudo = nNegudo;
		aLogItem.Gold_1 = dwGold_1;
		aLogItem.nSlot = nSlot;
		aLogItem.nSlot1 = nSlot1;
		aLogItem.Action = chAction;

		arRead >> aLogItem.m_dwKeepTime;

		arRead >> aLogItem.nPiercedSize;
		for (int i = 0; i < aLogItem.nPiercedSize; i++)
			arRead >> aLogItem.adwItemId[i];
		arRead >> aLogItem.nUMPiercedSize;
		for (int i = 0; i < aLogItem.nUMPiercedSize; i++)
			arRead >> aLogItem.adwUMItemId[i];


		arRead >> aLogItem.m_iRandomOptItemId;


#if __VER >= 9 // __PET_0410
		arRead >> aLogItem.nPetKind;
		arRead >> aLogItem.nPetLevel;
		arRead >> aLogItem.dwPetExp;
		arRead >> aLogItem.wPetEnergy;
		arRead >> aLogItem.wPetLife;
		arRead >> aLogItem.nPetAL_D;
		arRead >> aLogItem.nPetAL_C;
		arRead >> aLogItem.nPetAL_B;
		arRead >> aLogItem.nPetAL_A;
		arRead >> aLogItem.nPetAL_S;
#endif // __PET_0410
		DBQryNewItemLog(szQuery, aLogItem);

		if (FALSE == qry->Exec(szQuery))
		{
			WriteLog("LogItem %s, %d\t%s, %s", __FILE__, __LINE__, lpszPlayer1, lpszPlayer2);
			if (strlen(szQuery) < 4096)
				WriteLog(" LogItem Qyery => %s", szQuery);

			return;
		}
		break;
	}
	}

}

void CDbManager::LogLevelUp(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	char		cPlay[17] = { 0, };
	int			iServer_No;
	int			iLevel;
	int			iPlayJob;
	int			iJobLevel;
	int			iStr;
	int			iDex;
	int			iInt;
	int			iSta;
	int			iGP;
	int			iLP;
	EXPINTEGER	iExp1;
	int			iFlightLevel;
	char		cAction;
	u_long		uidPlayer;
	DWORD		dwGold;
	DWORD		dwPlayerTime;

	iServer_No = g_appInfo.dwSys;

	arRead >> iExp1;
	arRead >> iFlightLevel;
	arRead >> iLevel;
	arRead >> iPlayJob;
	arRead >> iJobLevel;
	arRead >> iStr;
	arRead >> iDex;
	arRead >> iInt;
	arRead >> iSta;
	arRead >> iGP;
	arRead >> iLP;
	arRead >> dwGold;
	arRead >> dwPlayerTime;

	int iAction;
	arRead >> iAction;
	arRead >> uidPlayer;
	switch (iAction)
	{
	case 1:
		cAction = 'L';
		break;
	case 2:
		cAction = 'S';
		break;
	case 3:
		cAction = 'J';
		break;
	case 4:
		cAction = 'C';
		break;
	case 5:
		cAction = 'E';
		break;
	case 6:
		cAction = 'D';
		break;
	case 7:
		cAction = 'M';
		break;
	case 8:
		cAction = '1';
		break;
	case 9:
		cAction = '2';
		break;
	case 10:
		cAction = '3';
		break;
	default:
	{
		WriteLog("%s, %d\t%c", __FILE__, __LINE__, iAction);

		return;
	}
	}

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryLog(szQuery, "L1", uidPlayer, iServer_No, iExp1, iLevel, iPlayJob, iJobLevel, iFlightLevel, iStr, iDex, iInt, iSta, iGP, iLP, cAction, 0, '\0', 0, 0, 0, dwGold, dwPlayerTime);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%d, %s", __FILE__, __LINE__, uidPlayer, szQuery);

		return;
	}


}

void CDbManager::LogUniqueItem(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	int			iServer_No;
	char		cCrateMan[17] = { 0, };
	int			iWorld;
	D3DXVECTOR3	Pos;
	char		cUniqueName[51] = { 0, };
	int			cItemAddLevel;
	u_long		uidPlayer;

	iServer_No = g_appInfo.dwSys;
	arRead.ReadString(cCrateMan, 17);
	arRead >> iWorld;
	arRead >> Pos;
	arRead.ReadString(cUniqueName, 51);
	arRead >> cItemAddLevel;
	arRead >> uidPlayer;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryLog(szQuery, "L4", uidPlayer, iServer_No, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 'A',
		iWorld, '\0', Pos.x, Pos.y, Pos.z, 0, 0, '\0', 0, 0, 0, cUniqueName, 0, 0, cItemAddLevel);

	qry->Exec(szQuery);


}

void CDbManager::LogQuest(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	int Action;
	char cName[17] = { 0, };
	int iServer_No;
	int nQuest;
	CTime		tQuest = CTime::GetCurrentTime();
	u_long	uidPlayer;

	iServer_No = g_appInfo.dwSys;
	CString strQuestTime = tQuest.Format("%Y%m%d%H%M%S");

	arRead >> Action;
	arRead.ReadString(cName, 17);
	arRead >> nQuest;
	arRead >> uidPlayer;

	char szQuery[QUERY_SIZE] = { 0, };

	//	qry->Clear();
	if (Action == 1)
	{
		DBQryLog(szQuery, "L6", uidPlayer, iServer_No, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 'C', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer(strQuestTime.GetLength()), 0, '\0', nQuest);

		if (FALSE == qry->Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);

			return;
		}

		//		qry->Clear();
	}
	else
	{
		DBQryLog(szQuery, "L7", uidPlayer, iServer_No, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 'A', 0, '\0', 0.0f, 0.0f, 0.0f, 0, 0, 0,
			0, 0, 0, '\0', 0, 0, 0, strQuestTime.GetBuffer(strQuestTime.GetLength()), 0, '\0', nQuest);

		if (FALSE == qry->Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);

			return;
		}

		//		qry->Clear();
	}

}

void CDbManager::DBQryLog(char* qryLog, char* Gu, u_long idPlayer, int nserverindex, EXPINTEGER nExp1, int nLevel,
	int nJob, int JobLv, int FlightLv, int nStr, int nDex,
	int nInt, int nSta, int nRemainGP, int nRemainLP, char szState,
	int WorldID, char* szkilled_szName, float vPos_x, float vPos_y, float vPos_z,
	int nAttackPower, int nMax_HP, char* uGetidPlayer, DWORD dwGold, int nRemainGold,
	int nItem_UniqueNo, char* szItem_Name, int Item_durability, int Item_count, int ItemAddLv,
	char* szStart_Time, int TotalPlayerTime, char* szRemoteIP, int nQuest_Index, u_long uKill_idPlayer)
{
	CString strItem = szItem_Name;
	strItem.Replace('\'', ' ');
	sprintf(qryLog, "LOG_STR '%s',@im_idPlayer='%07d',@iserverindex='%02d',@im_nExp1=%I64d,@im_nLevel=%d,"
		"@im_nJob=%d,@im_nJobLv=%d,@im_nStr=%d,@im_nDex=%d,"
		"@im_nInt=%d,@im_nSta=%d,@im_nRemainGP=%d,@im_nRemainLP=%d,@iState='%c',"
		"@idwWorldID=%d,@ikilled_m_szName='%s',@im_vPos_x=%f,@im_vPos_y=%f,@im_vPos_z=%f,"
		"@iattackPower=%d,@imax_HP=%d,@im_GetszName='%s',@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@iItem_count=%d,@iItem_AddLv=%d,"
		"@iStart_Time='%s',@iTotalPlayTime=%d,@iremoteIP='%s',@iQuest_Index=%d,@ikilled_m_idPlayer='%07d'",
		Gu, idPlayer, nserverindex, nExp1, nLevel,
		nJob, JobLv, nStr, nDex,
		nInt, nSta, nRemainGP, nRemainLP, szState,
		WorldID, szkilled_szName, vPos_x, vPos_y, vPos_z,
		nAttackPower, nMax_HP, uGetidPlayer, dwGold, nRemainGold,
		//		nItem_UniqueNo, szItem_Name, Item_durability, Item_count, ItemAddLv,
		nItem_UniqueNo, (LPCSTR)strItem, Item_durability, Item_count, ItemAddLv,
		szStart_Time, TotalPlayerTime, szRemoteIP, nQuest_Index, uKill_idPlayer);
	if (strlen(qryLog) > 40960)
		ASSERT(0);

}

void CDbManager::LogConcurrentUserNumber(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	//	qry->Clear();

	int cbSize;
	int anCount[64];
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	int nChannel;
	ar >> nChannel;
#endif // __LOG_PLAYERCOUNT_CHANNEL
	ar >> cbSize;
	//	memset( (void*)anCount, 0, sizeof(int) * 64 );
	ar.Read((void*)anCount, sizeof(int) * cbSize);
	//	LOG_USER_CNT_STR '00', 0, 0, 0, 0
	char szQuery[QUERY_SIZE] = { 0, };
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	sprintf(szQuery, "LOG_USER_CNT_STR '%02d', %d", g_appInfo.dwSys, nChannel);
#else // __LOG_PLAYERCOUNT_CHANNEL
	sprintf(szQuery, "LOG_USER_CNT_STR '%02d'", g_appInfo.dwSys);
#endif // __LOG_PLAYERCOUNT_CHANNEL

	char szCount[10];
	for (int i = 0; i < cbSize; i++)
	{
		sprintf(szCount, ", %d", anCount[i]);
		strcat(szQuery, szCount);
	}

	qry->Exec(szQuery);

	//	qry->Clear();


}

void CDbManager::DBQryNewItemLog(char* qryLog, const LogItemInfo& info)
{
	sprintf(qryLog, "LOG_ITEM_STR "
		"@im_szName='%s',@iserverindex='%02d',@im_GetszName='%s',@idwWorldID=%d,@im_dwGold=%d,@im_nRemainGold=%d,"
		"@iItem_UniqueNo=%d,@iItem_Name='%s',@iItem_durability=%d,@im_nAbilityOption=%d,@Im_GetdwGold=%d,"
		"@iItem_count=%d,@iState='%s',@im_nSlot0=%d,@im_nSlot1=%d,@im_bItemResist=%d,@im_nResistAbilityOption=%d"
		",@im_dwKeepTime=%d,"
#if __VER >= 11 // __SYS_IDENTIFY
		"@im_nRandomOptItemId=%I64d"
#else	// __SYS_IDENTIFY
		"@im_nRandomOptItemId=%d"
#endif	// __SYS_IDENTIFY
		",@inPiercedSize=%d,@iadwItemId0=%d,@iadwItemId1=%d,@iadwItemId2=%d,@iadwItemId3=%d"
#if __VER >= 9 // __ULTIMATE
		",@iadwItemId4=%d"
#endif // __ULTIMATE

		",@iMaxDurability=%d"

#if __VER >= 9 // __PET_0410
		",@inPetKind=%d,@inPetLevel=%d,@idwPetExp=%d,@iwPetEnergy=%d,@iwPetLife=%d"
		",@inPetAL_D=%d,@inPetAL_C=%d,@inPetAL_B=%d,@inPetAL_A=%d,@inPetAL_S=%d"
#endif // __PET_0410
#if __VER >= 12 // __EXT_PIERCING
		",@iadwItemId5=%d,@iadwItemId6=%d,@iadwItemId7=%d,@iadwItemId8=%d,@iadwItemId9=%d"
		",@inUMPiercedSize=%d,@iadwUMItemId0=%d,@iadwUMItemId1=%d,@iadwUMItemId2=%d,@iadwUMItemId3=%d,@iadwUMItemId4=%d"
#endif // __EXT_PIERCING
		,
		info.SendName, g_appInfo.dwSys, info.RecvName, info.WorldId, info.Gold, info.Gold2,
		info.ItemNo, info.szItemName, info.Negudo, info.nAbilityOption, info.Gold_1,
		info.itemNumber, info.Action, info.nSlot, info.nSlot1, info.nItemResist, info.nResistAbilityOption
		, info.m_dwKeepTime,
		info.m_iRandomOptItemId
		, info.nPiercedSize, info.adwItemId[0], info.adwItemId[1], info.adwItemId[2], info.adwItemId[3]
		, info.adwItemId[4]
		, 0
		, info.nPetKind, info.nPetLevel, info.dwPetExp, info.wPetEnergy, info.wPetLife
		, info.nPetAL_D, info.nPetAL_C, info.nPetAL_B, info.nPetAL_A, info.nPetAL_S
		, info.adwItemId[5], info.adwItemId[6], info.adwItemId[7], info.adwItemId[8], info.adwItemId[9]
		, info.nUMPiercedSize, info.adwUMItemId[0], info.adwUMItemId[1], info.adwUMItemId[2], info.adwUMItemId[3], info.adwUMItemId[4]
	);
	if (strlen(qryLog) > 40960)
		ASSERT(0);
}

BOOL CDbManager::call_uspLoggingTrade(CQuery* pQuery, int nFlag, int nTradeID, DWORD dwWorldID, u_long idPlayer, DWORD dwTradeGold, const char* lpAddr, int nLevel, int nJob, int nItemIndex, SERIALNUMBER iItemSerialNum, int nItemCnt, int nAbilityOpt, int nItemResist, int nResistAbilityOpt, __int64 iRandomOpt)
{
	char pszQuery[1024] = { 0, };
	switch (nFlag)
	{
	case 0:
	{
		sprintf(pszQuery, "uspLoggingTrade @pFlag=0, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d",
			nTradeID, g_appInfo.dwSys, dwWorldID);
		break;
	}
	case 1:
	{
		sprintf(pszQuery, "uspLoggingTrade @pFlag=1, @pTradeID=%d, @pserverindex='%02d', @pWorldID=%d, @pidPlayer='%07d', @pTradeGold=%d, @pPlayerIP='%s', @pLevel=%d, @pJob=%d",
			nTradeID, g_appInfo.dwSys, dwWorldID, idPlayer, dwTradeGold, lpAddr, nLevel, nJob);
		break;
	}
	case 2:
	{
		sprintf(pszQuery, "uspLoggingTrade @pFlag=2, @pTradeID=%d, @pserverindex='%02d', @pidPlayer='%07d', @pItemIndex='%d', @pItemSerialNum=%d, @pItemCnt=%d, @pAbilityOpt=%d, @pItemResist=%d, @pResistAbilityOpt=%d,"
#if __VER >= 11 // __SYS_IDENTIFY
			"@pRandomOpt=%I64d",
#else	// __SYS_IDENTIFY
			"@pRandomOpt=%d",
#endif	// __SYS_IDENTIFY
#if __VER >= 11 // __SYS_IDENTIFY
			nTradeID, g_appInfo.dwSys, idPlayer, nItemIndex, iItemSerialNum, nItemCnt, nAbilityOpt, nItemResist, nResistAbilityOpt, iRandomOpt);
#else	// __SYS_IDENTIFY
			nTradeID, g_appInfo.dwSys, idPlayer, nItemIndex, iItemSerialNum, nItemCnt, nAbilityOpt, nItemResist, nResistAbilityOpt, nRandomOpt );
#endif	// __SYS_IDENTIFY
		break;
	}
	}
	if (FALSE == pQuery->Exec(pszQuery))
	{
		WriteLog("%s, %d:\t%s", __FILE__, __LINE__, pszQuery);
		return FALSE;
	}
	//	OutputDebugString( pszQuery );
	return TRUE;
}


#if __VER >= 15 // __GUILD_HOUSE
void CDbManager::LogGuildFurniture(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	DWORD dwGuildId;
	GH_Fntr_Info gfi;
	char chState;

	arRead >> dwGuildId;
	gfi.Serialize(arRead);
	arRead >> chState;

	pQuery->Execute("usp_GuildFurniture_Log @serverindex = '%02d', @m_idGuild = '%06d', @SEQ = %d",
		g_appInfo.dwSys, dwGuildId, gfi.nSeqNum);


}
#endif // __GUILD_HOUSE