#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#include "definequest.h"

extern	BOOL CheckValidItem(DWORD dwItemId, short nNum);

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;

#include "guildwar.h"

#if __VER >= 13 // __HONORABLE_TITLE
#include "honor.h"
#endif	// __HONORABLE_TITLE

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
		if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer, lpDbOverlappedPlus ) )		return;

#define	VERIFY_GUILD_STRING( lpString, lpszGuild )	\
if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszGuild ) )		return;

// mirchang_100416 VERIFYSTRING use return value
#define	VERIFYSTRING_RETURN( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer) )		return FALSE;

#define	VERIFYSTRING_BANK( lpString, lpszPlayer )	\
	if( FALSE == VerifyString( lpString, __FILE__, __LINE__, lpszPlayer ) )		return FALSE;
// mirchang_100416

void CDbManager::GetStrTime(CTime* time, const char* strbuf)
{
	char cYear[5] = { 0, };
	char cMonth[3] = { 0, };
	char cDay[3] = { 0, };
	char cHour[3] = { 0, };
	char cMin[3] = { 0, };

	strncpy(cYear, strbuf, 4);
	strncpy(cMonth, strbuf + 4, 2);
	strncpy(cDay, strbuf + 6, 2);
	strncpy(cHour, strbuf + 8, 2);
	strncpy(cMin, strbuf + 10, 2);

	CTime Timebuf(atoi(cYear), atoi(cMonth), atoi(cDay), atoi(cHour), atoi(cMin), 0);
	*time = Timebuf;
}

#ifdef __PET_1024
void CDbManager::GetDBFormatStr(char* szDst, int nMaxLen, const char* szSrc)
{
	char szDigit[3] = { 0, };
#ifdef __VS2003
	int ch2;
#else	// __VS2003
	char ch;
#endif // __VS2003
	char* pCur = const_cast<char*>(szSrc);
	int nLen = strlen(pCur);
	if (nLen % 2 != 0)
	{
		pCur[nLen - 1] = '\0';
		nLen--;
	}
	if (nLen >= nMaxLen * 2)
	{
		Error("GetDBFormatStr: nLen = %d, nMaxLen = %d", nLen, nMaxLen);
		*szDst = '\0';
		return;
	}
	while (*pCur)
	{
		szDigit[0] = pCur[0];
		szDigit[1] = pCur[1];
		pCur += 2;
#ifdef __VS2003
		sscanf(szDigit, "%2X", &ch2);
		*szDst++ = ch2;
#else // __VS2003
		sscanf(szDigit, "%2X", &ch);
		*szDst++ = ch;
#endif // __VS2003
	}
	*szDst = '\0';
}

void CDbManager::SetDBFormatStr(char* szDst, int nMaxLen, const char* szSrc)
{
	int nLen = strlen(szSrc);
	if (nMaxLen < nLen * 2 + 1)
	{
		Error("SetDBFormatStr: nLen = %d, nMaxLen = %d", nLen, nMaxLen);
		*szDst = '\0';
		return;
	}

	char* pCur = szDst;
	while (*szSrc)
	{
		sprintf(pCur, "%2X", (BYTE)*szSrc);
		pCur += 2;
		szSrc++;
	}
	*pCur = '\0';
}
#endif	// __PET_1024



void CDbManager::GetStrFromDBFormat(char* szDst, const char* szSrc, int& n)
{
	char szDigit[3] = { 0, };
	char ch;
#ifdef __VS2003
	int ch2;
#endif // __VS2003

	const char* pCur = szSrc + n;
	while (*pCur != '/' && *pCur)
	{
		szDigit[0] = pCur[0];
		szDigit[1] = pCur[1];
		pCur += 2;

#ifdef __VS2003
		sscanf(szDigit, "%2X", &ch2);
		ch = ch2;
#else // __VS2003
		sscanf(szDigit, "%2X", &ch);
#endif // __VS2003
		* szDst++ = ch;
	}
	*szDst = '\0';
	n = (pCur - szSrc) + 1;
}

#ifndef __FL_PACKED_ITEMS
BOOL CDbManager::GetBank(CMover* pMover, CQuery* qry, int nSlot)
#else
BOOL CDbManager::GetBank(CMover* pMover, CQuery* qry, int nSlot, CQuery* qry1)
#endif
{
	u_long uPlayerid = (u_long)qry->GetInt("m_idPlayer");
	DWORD dwGold = qry->GetInt("m_dwGoldBank");

	ItemContainerStruct	ics;
	sprintf(ics.szItem, "m_Bank");
	sprintf(ics.szIndex, "m_apIndex_Bank");
	sprintf(ics.szObjIndex, "m_dwObjIndex_Bank");
	sprintf(ics.szExt, "m_extBank");
	sprintf(ics.szPiercing, "m_BankPiercing");
	sprintf(ics.szPet, "szBankPet");

	if (!GetContainer(qry, &pMover->m_Bank[nSlot], &ics))
	{
		Error("Can't load bank %d, slot %d", uPlayerid, nSlot);
		return FALSE;
	}

	pMover->m_idPlayerBank[nSlot] = uPlayerid;
	pMover->m_dwGoldBank[nSlot] = dwGold;

#ifndef __FL_PACKED_ITEMS
	return TRUE;
#else
	return LoadPackages(qry1, &pMover->m_Bank[nSlot], uPlayerid, PRT_BANK);
#endif
}

#ifndef __FL_PACKED_ITEMS
BOOL CDbManager::GetInventory(CMover* pMover, CQuery* qry)
#else
BOOL CDbManager::GetInventory(CMover* pMover, CQuery* qry, CQuery* qry1)
#endif
{
	ItemContainerStruct	ics;
	sprintf(ics.szItem, "m_Inventory");
	sprintf(ics.szIndex, "m_apIndex");
	sprintf(ics.szObjIndex, "m_dwObjIndex");
	sprintf(ics.szExt, "m_extInventory");
	sprintf(ics.szPiercing, "m_InventoryPiercing");
	sprintf(ics.szPet, "szInventoryPet");

	if (!GetContainer(qry, &pMover->m_Inventory, &ics))
	{
		Error("Can't load inventory %d", pMover->m_idPlayer);
		return FALSE;
	}

#ifndef __FL_PACKED_ITEMS
	return TRUE;
#else
	return LoadPackages(qry1, &pMover->m_Inventory, pMover->m_idPlayer, PRT_INVENTORY);
#endif
}


BOOL CDbManager::GetContainer(CQuery* qry, CItemContainer<CItemElem  >* pContainer, ItemContainerStruct* pStruct)
{
	char szPlayerName[128] = { 0, };
	sprintf(szPlayerName, "GetContainer() error");

	if (pStruct->szItem[0] != '\0')
	{
		qry->GetStr(pStruct->szItem, pStruct->szItem);
		VERIFYSTRING_RETURN(pStruct->szItem, szPlayerName);
	}
	else
		strncat(pStruct->szItem, NullStr, sizeof(NullStr));

	if (pStruct->szIndex[0] != '\0')
	{
		qry->GetStr(pStruct->szIndex, pStruct->szIndex);
		VERIFYSTRING_RETURN(pStruct->szIndex, szPlayerName);
	}
	else
		strncat(pStruct->szIndex, NullStr, sizeof(NullStr));

	if (pStruct->szObjIndex[0] != '\0')
	{
		qry->GetStr(pStruct->szObjIndex, pStruct->szObjIndex);
		VERIFYSTRING_RETURN(pStruct->szObjIndex, szPlayerName);
	}
	else
		strncat(pStruct->szObjIndex, NullStr, sizeof(NullStr));

	if (pStruct->szExt[0] != '\0')
	{
		qry->GetStr(pStruct->szExt, pStruct->szExt);
		VERIFYSTRING_RETURN(pStruct->szExt, szPlayerName);
	}
	else
		strncat(pStruct->szExt, NullStr, sizeof(NullStr));

	if (pStruct->szPiercing[0] != '\0')
	{
		qry->GetStr(pStruct->szPiercing, pStruct->szPiercing);
		VERIFYSTRING_RETURN(pStruct->szPiercing, szPlayerName);
	}
	else
		strncat(pStruct->szPiercing, NullStr, sizeof(NullStr));

	if (pStruct->szPet[0] != '\0')
	{
		qry->GetStr(pStruct->szPet, pStruct->szPet);
		VERIFYSTRING_RETURN(pStruct->szPet, szPlayerName);
	}
	else
		strncat(pStruct->szPet, NullStr, sizeof(NullStr));

	int CountStr = 0;
	int IndexItem = 0;
	while ('$' != pStruct->szItem[CountStr])
	{
		CItemElem BufItemElem;
		IndexItem = GetOneItem(&BufItemElem, pStruct->szItem, &CountStr);
		if (IndexItem == -1)
			Error("GetContainer(1):  %s, %d", szPlayerName, BufItemElem.m_dwItemId);
		else
		{
			if (IndexItem >= pContainer->m_dwItemMax)
			{
				Error("GetContainer(2): %s, %d", szPlayerName, IndexItem);
				Error("GetContainer(2): Items = %s", pStruct->szItem);
				return FALSE;
			}
			pContainer->m_apItem[IndexItem] = BufItemElem;
		}
	}

	CountStr = 0;
	IndexItem = 0;
	while ('$' != pStruct->szIndex[CountStr])
	{
		pContainer->m_apIndex[IndexItem] = (DWORD)GetIntFromStr(pStruct->szIndex, &CountStr);
		IndexItem++;
	}

	CountStr = 0;
	IndexItem = 0;
	while ('$' != pStruct->szObjIndex[CountStr])
	{
		pContainer->m_apItem[IndexItem].m_dwObjIndex = (DWORD)GetIntFromStr(pStruct->szObjIndex, &CountStr);
		IndexItem++;
	}

	CountStr = 0;
	IndexItem = 0;
	while ('$' != pStruct->szExt[CountStr])
	{
		pContainer->m_apItem[IndexItem].m_dwKeepTime = (DWORD)GetIntPaFromStr(pStruct->szExt, &CountStr);
		__int64 iRandomOptItemId = GetInt64PaFromStr(pStruct->szExt, &CountStr);
		pContainer->m_apItem[IndexItem].SetRandomOptItemId(iRandomOptItemId);
		pContainer->m_apItem[IndexItem].m_bTranformVisPet = static_cast<BOOL>(GetIntPaFromStr(pStruct->szExt, &CountStr));

		CountStr++;
		IndexItem++;
	}

	CountStr = 0;
	IndexItem = 0;
	while ('$' != pStruct->szPiercing[CountStr])
	{
		LoadPiercingInfo(pContainer->m_apItem[IndexItem], pStruct->szPiercing, &CountStr);
		IndexItem++;
	}

	CountStr = 0;
	IndexItem = 0;
	while ('$' != pStruct->szPet[CountStr])
	{
		BOOL bPet = (BOOL)GetIntFromStr(pStruct->szPet, &CountStr);
		if (bPet)
		{
			SAFE_DELETE(pContainer->m_apItem[IndexItem].m_pPet);
			CPet* pPet = pContainer->m_apItem[IndexItem].m_pPet = new CPet;
			BYTE nKind = (BYTE)GetIntFromStr(pStruct->szPet, &CountStr);
			pPet->SetKind(nKind);
			BYTE nLevel = (BYTE)GetIntFromStr(pStruct->szPet, &CountStr);
			pPet->SetLevel(nLevel);
			DWORD dwExp = (DWORD)GetIntFromStr(pStruct->szPet, &CountStr);
			pPet->SetExp(dwExp);
			WORD wEnergy = (WORD)GetIntFromStr(pStruct->szPet, &CountStr);
			pPet->SetEnergy(wEnergy);

			WORD wLife = (WORD)GetIntPaFromStr(pStruct->szPet, &CountStr);
			pPet->SetLife(wLife);
			for (int i = PL_D; i <= pPet->GetLevel(); i++)
			{
				BYTE nAvailLevel = (BYTE)GetIntPaFromStr(pStruct->szPet, &CountStr);
				pPet->SetAvailLevel(i, nAvailLevel);
			}
			char szFmt[MAX_PET_NAME_FMT] = { 0, };
			GetStrFromStr(pStruct->szPet, szFmt, &CountStr);
			char szName[MAX_PET_NAME] = { 0, };
			GetDBFormatStr(szName, MAX_PET_NAME, szFmt);
			pPet->SetName(szName);
		}
		IndexItem++;
	}

	return TRUE;
}

#if __VER >= 12 // __EXT_PIERCING
void CDbManager::LoadPiercingInfo(CItemElem& itemElem, char* szPirecingInven, int* pLocation)
{
	ItemProp* itemProp = itemElem.GetProp();
	if (itemProp && itemProp->IsUltimate())
	{
		itemElem.SetUltimatePiercingSize(GetIntPaFromStr(szPirecingInven, pLocation));
		for (int i = 0; i < itemElem.GetUltimatePiercingSize(); ++i)
			itemElem.SetUltimatePiercingItem(i, (DWORD)GetIntPaFromStr(szPirecingInven, pLocation));
	}
	itemElem.SetPiercingSize(GetIntFromStr(szPirecingInven, pLocation));
	for (int i = 0; i < itemElem.GetPiercingSize(); ++i)
		itemElem.SetPiercingItem(i, (DWORD)GetIntFromStr(szPirecingInven, pLocation));
#if __VER >= 15 // __PETVIS
	if (itemElem.IsVisPet())
	{
		itemElem.SetVisKeepTimeSize(itemElem.GetPiercingSize());
		for (int i = 0; i < itemElem.GetPiercingSize(); i++)
			itemElem.SetVisKeepTime(i, (time_t)GetIntFromStr(szPirecingInven, pLocation));
	}
#endif // __PETVIS
}





void CDbManager::GetPiercingInfoFromMail(CQuery* pQuery, CItemElem* pItemElem)
{
	int nAddCount = 1;
	if (pItemElem->GetProp() && pItemElem->GetProp()->IsUltimate())
	{
		pItemElem->SetUltimatePiercingSize(pQuery->GetInt("nPiercedSize"));
		for (int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++)
		{
			char szItem[32];
			sprintf(szItem, "dwItemId%d", i + nAddCount);
			pItemElem->SetUltimatePiercingItem(i, pQuery->GetInt(szItem));
		}
		pItemElem->SetPiercingSize(pQuery->GetInt("nPiercedSize2"));
		nAddCount = 6;
	}
	else
		pItemElem->SetPiercingSize(pQuery->GetInt("nPiercedSize"));

	for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
	{
		char szItem[32];
		sprintf(szItem, "dwItemId%d", i + nAddCount);
		pItemElem->SetPiercingItem(i, pQuery->GetInt(szItem));
	}
}
#endif // __EXT_PIERCING


BOOL CDbManager::GetTaskBar(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	int CountStr = 0;
	int nIndex = 0;
	char AppletTaskBar[MAX_APPLETTASKBAR] = { 0, };
	qry->GetStr("m_aSlotApplet", AppletTaskBar);
	VERIFYSTRING_RETURN(AppletTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer);
	while ('$' != AppletTaskBar[CountStr])
	{
		nIndex = GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwId = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwType = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwIndex = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwUserId = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwData = (DWORD)GetIntFromStr(AppletTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_nLocation = GetIntFromStr(AppletTaskBar, &CountStr);

		if (pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_dwShortcut == SHORTCUT_CHAT)
			GetStrFromDBFormat(pMover->m_UserTaskBar.m_aSlotApplet[nIndex].m_szString, AppletTaskBar, CountStr);
	}

	CountStr = 0;
	nIndex = 0;
	int nSlotIndex = 0;
	char ItemTaskBar[MAX_ITEMTASKBAR] = { 0, };
	qry->GetStr("m_aSlotItem", ItemTaskBar);
	VERIFYSTRING_RETURN(ItemTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer);
	while ('$' != ItemTaskBar[CountStr])
	{
		nSlotIndex = GetIntFromStr(ItemTaskBar, &CountStr);
		nIndex = GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwId = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwType = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwIndex = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwUserId = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwData = (DWORD)GetIntFromStr(ItemTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_nLocation = GetIntFromStr(ItemTaskBar, &CountStr);

		if (pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_dwShortcut == SHORTCUT_CHAT)
			GetStrFromDBFormat(pMover->m_UserTaskBar.m_aSlotItem[nSlotIndex][nIndex].m_szString, ItemTaskBar, CountStr);
	}

	CountStr = 0;
	nIndex = 0;
	char SkillTaskBar[1024] = { 0, };
	qry->GetStr("m_aSlotQueue", SkillTaskBar);
	VERIFYSTRING_RETURN(SkillTaskBar, lpDbOverlappedPlus->AccountInfo.szPlayer);
	while ('$' != SkillTaskBar[CountStr])
	{
		nIndex = GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwShortcut = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwId = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwType = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwIndex = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwUserId = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_dwData = (DWORD)GetIntFromStr(SkillTaskBar, &CountStr);
		pMover->m_UserTaskBar.m_aSlotQueue[nIndex].m_nLocation = GetIntFromStr(SkillTaskBar, &CountStr);
	}
	pMover->m_UserTaskBar.m_nActionPoint = qry->GetInt("m_SkillBar");
	return TRUE;
}

void CDbManager::GetBaseCharacter(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	qry->GetStr("m_szName", pMover->m_szName);
	pMover->m_vScale.x = qry->GetFloat("m_vScale_x");
	pMover->m_dwMotion = (DWORD)qry->GetInt("m_dwMotion");
	pMover->m_dwWorldID = qry->GetInt("dwWorldID");
	pMover->m_vPos.x = qry->GetFloat("m_vPos_x");
	pMover->m_vPos.y = qry->GetFloat("m_vPos_y");
	pMover->m_vPos.z = qry->GetFloat("m_vPos_z");
	pMover->m_fAngle = qry->GetFloat("m_fAngle");
	qry->GetStr("m_szCharacterKey", pMover->m_szCharacterKey);
	pMover->m_nHitPoint = (LONG)qry->GetInt("m_nHitPoint");
	pMover->m_nManaPoint = (LONG)qry->GetInt("m_nManaPoint");
	pMover->m_nFatiguePoint = (LONG)qry->GetInt("m_nFatiguePoint");
	pMover->m_dwSkinSet = (DWORD)qry->GetInt("m_dwSkinSet");
	pMover->m_dwHairMesh = (DWORD)qry->GetInt("m_dwHairMesh");
	pMover->m_dwHairColor = (DWORD)qry->GetInt("m_dwHairColor");
	pMover->m_dwHeadMesh = (DWORD)qry->GetInt("m_dwHeadMesh");
	pMover->SetSex((BYTE)qry->GetInt("m_dwSex"));
	pMover->m_dwRideItemIdx = (DWORD)qry->GetInt("m_dwRideItemIdx");
	pMover->SetGold(qry->GetInt("m_dwGold"));
	pMover->m_nJob = (LONG)qry->GetInt("m_nJob");
	pMover->m_idparty = (u_long)qry->GetInt("m_idparty");

	int CountStr = 0;
	char ActMover[50] = { 0, };
	qry->GetStr("m_pActMover", ActMover);
	pMover->m_pActMover->m_dwState = (DWORD)GetIntFromStr(ActMover, &CountStr);
	pMover->m_pActMover->m_dwStateFlag = (DWORD)GetIntFromStr(ActMover, &CountStr);

	CountStr = 0;
	pMover->m_nStr = (LONG)qry->GetInt("m_nStr");
	pMover->m_nSta = (LONG)qry->GetInt("m_nSta");
	pMover->m_nDex = (LONG)qry->GetInt("m_nDex");
	pMover->m_nInt = (LONG)qry->GetInt("m_nInt");
	pMover->m_nLevel = (LONG)qry->GetInt("m_nLevel");
	pMover->m_nExp1 = qry->GetExpInteger("m_nExp1");
	pMover->m_nFuel = qry->GetInt("m_nFuel");
	pMover->m_tmAccFuel = qry->GetInt("m_tmAccFuel");

	pMover->m_idMarkingWorld = (DWORD)qry->GetInt("m_idMarkingWorld");
	pMover->m_vMarkingPos.x = qry->GetFloat("m_vMarkingPos_x");
	pMover->m_vMarkingPos.y = qry->GetFloat("m_vMarkingPos_y");
	pMover->m_vMarkingPos.z = qry->GetFloat("m_vMarkingPos_z");
	pMover->m_nRemainGP = (long)qry->GetInt("m_nRemainGP");
	pMover->m_nFxp = (long)qry->GetInt("m_nFxp");
	pMover->m_dwMode = (DWORD)qry->GetInt("m_dwMode");
	pMover->m_bPlayer = 1;
	pMover->m_nSkillLevel = qry->GetInt("m_SkillLv");
	pMover->m_nSkillPoint = qry->GetInt("m_SkillPoint");
#if __VER >= 13
	pMover->m_nHonor = qry->GetInt("m_nHonor");
#endif

}
#if __VER >= 13
void	CDbManager::GetHonor(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"usp_Master_Select '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer);
	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	int aTempHonor[3][50] = { 0, };
	int nSec = 0;
	while (qry->Fetch())
	{
		int nSec = qry->GetInt("sec");
		nSec--;
		if (nSec > 2 || nSec < 0)
			return;

		aTempHonor[nSec][0] = qry->GetInt("c01");
		aTempHonor[nSec][1] = qry->GetInt("c02");
		aTempHonor[nSec][2] = qry->GetInt("c03");
		aTempHonor[nSec][3] = qry->GetInt("c04");
		aTempHonor[nSec][4] = qry->GetInt("c05");
		aTempHonor[nSec][5] = qry->GetInt("c06");
		aTempHonor[nSec][6] = qry->GetInt("c07");
		aTempHonor[nSec][7] = qry->GetInt("c08");
		aTempHonor[nSec][8] = qry->GetInt("c09");
		aTempHonor[nSec][9] = qry->GetInt("c10");

		aTempHonor[nSec][10] = qry->GetInt("c11");
		aTempHonor[nSec][11] = qry->GetInt("c12");
		aTempHonor[nSec][12] = qry->GetInt("c13");
		aTempHonor[nSec][13] = qry->GetInt("c14");
		aTempHonor[nSec][14] = qry->GetInt("c15");
		aTempHonor[nSec][15] = qry->GetInt("c16");
		aTempHonor[nSec][16] = qry->GetInt("c17");
		aTempHonor[nSec][17] = qry->GetInt("c18");
		aTempHonor[nSec][18] = qry->GetInt("c19");
		aTempHonor[nSec][19] = qry->GetInt("c20");

		aTempHonor[nSec][20] = qry->GetInt("c21");
		aTempHonor[nSec][21] = qry->GetInt("c22");
		aTempHonor[nSec][22] = qry->GetInt("c23");
		aTempHonor[nSec][23] = qry->GetInt("c24");
		aTempHonor[nSec][24] = qry->GetInt("c25");
		aTempHonor[nSec][25] = qry->GetInt("c26");
		aTempHonor[nSec][26] = qry->GetInt("c27");
		aTempHonor[nSec][27] = qry->GetInt("c28");
		aTempHonor[nSec][28] = qry->GetInt("c29");
		aTempHonor[nSec][29] = qry->GetInt("c30");

		aTempHonor[nSec][30] = qry->GetInt("c31");
		aTempHonor[nSec][31] = qry->GetInt("c32");
		aTempHonor[nSec][32] = qry->GetInt("c33");
		aTempHonor[nSec][33] = qry->GetInt("c34");
		aTempHonor[nSec][34] = qry->GetInt("c35");
		aTempHonor[nSec][35] = qry->GetInt("c36");
		aTempHonor[nSec][36] = qry->GetInt("c37");
		aTempHonor[nSec][37] = qry->GetInt("c38");
		aTempHonor[nSec][38] = qry->GetInt("c39");
		aTempHonor[nSec][39] = qry->GetInt("c40");

		aTempHonor[nSec][40] = qry->GetInt("c41");
		aTempHonor[nSec][41] = qry->GetInt("c42");
		aTempHonor[nSec][42] = qry->GetInt("c43");
		aTempHonor[nSec][43] = qry->GetInt("c44");
		aTempHonor[nSec][44] = qry->GetInt("c45");
		aTempHonor[nSec][45] = qry->GetInt("c46");
		aTempHonor[nSec][46] = qry->GetInt("c47");
		aTempHonor[nSec][47] = qry->GetInt("c48");
		aTempHonor[nSec][48] = qry->GetInt("c49");
		aTempHonor[nSec][49] = qry->GetInt("c50");
	}

	int nMonster = 0, nItem = 0, nEtc = 0;
	int nType = 0;
	int nCurrentTitleCount = CTitleManager::Instance()->m_nCurrentTitleCount;
	ASSERT(nCurrentTitleCount <= MAX_HONOR_TITLE);
	for (int i = 0; i < nCurrentTitleCount; i++)
	{
		nType = CTitleManager::Instance()->GetIdxType(i);
		if (nType == HI_HUNT_MONSTER)
		{
			pMover->SetHonorCount(i, aTempHonor[2][nMonster]);
			nMonster++;
		}
		else if (nType == HI_USE_ITEM)
		{
			pMover->SetHonorCount(i, aTempHonor[1][nItem]);
			nItem++;
		}
		else
		{
			pMover->SetHonorCount(i, aTempHonor[0][nEtc]);
			nEtc++;
		}
	}
}

#endif

BOOL CDbManager::GetSkill(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"uspLoadCharacterSkill '%02d','%07d'",
		g_appInfo.dwSys, pMover->m_idPlayer);
	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return FALSE;
	}

	while (qry->Fetch())
	{
		SKILL BufSkill;
		DWORD dwSkillPosition;
		dwSkillPosition = (DWORD)qry->GetInt("SkillPosition");
		BufSkill.dwSkill = (DWORD)qry->GetInt("SkillID");
		BufSkill.dwLevel = (DWORD)qry->GetInt("SkillLv");

		const ItemProp* pSkillProp = prj.GetSkillProp(BufSkill.dwSkill);
		if (pSkillProp == NULL)
		{
			Error("[ INVALID SkillID. PLAYER_ID(%07d), dwSkillID:(%u) ]", pMover->m_idPlayer, BufSkill.dwSkill);
			continue;
		}

		if (BufSkill.dwLevel > pSkillProp->dwExpertMax)
			BufSkill.dwLevel = pSkillProp->dwExpertMax;

		pMover->m_aJobSkill[dwSkillPosition] = BufSkill;
	}
	return TRUE;
}

BOOL CDbManager::GetQuest(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	int CountStr = 0;
	int IndexQuest = 0;
	char QuestCnt[3072] = { 0, };
	qry->GetStr("m_lpQuestCntArray", QuestCnt);
	VERIFYSTRING_RETURN(QuestCnt, pMover->m_szName);
	while ('$' != QuestCnt[CountStr])
	{
		QUEST BufQuest;
		GetOneQuest(&BufQuest, QuestCnt, &CountStr);
		pMover->m_aQuest[IndexQuest] = BufQuest;
		IndexQuest++;
	}
	pMover->m_nQuestSize = IndexQuest;

	int nQuestSize = IndexQuest;
	CountStr = 0;
	IndexQuest = 0;
	char CompleteQuest[1024] = { 0, };
	qry->GetStr("m_aCompleteQuest", CompleteQuest);
	VERIFYSTRING_RETURN(CompleteQuest, pMover->m_szName);
	while ('$' != CompleteQuest[CountStr])
	{
		pMover->m_aCompleteQuest[IndexQuest] = (WORD)GetIntFromStr(CompleteQuest, &CountStr);
		IndexQuest++;
	}
	pMover->m_nCompleteQuestSize = IndexQuest;

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	CountStr = 0;
	IndexQuest = 0;
	char CheckedQuest[100] = { 0, };
	qry->GetStr("m_aCheckedQuest", CheckedQuest);
	VERIFYSTRING_RETURN(CheckedQuest, pMover->m_szName);
	while ('$' != CheckedQuest[CountStr])
	{
		pMover->m_aCheckedQuest[IndexQuest] = (WORD)GetIntFromStr(CheckedQuest, &CountStr);
		IndexQuest++;
	}
	pMover->m_nCheckedQuestSize = IndexQuest;
#endif // __IMPROVE_QUEST_INTERFACE


	for (int i = 0; i < nQuestSize; i++)
	{
		if (pMover->m_aQuest[i].m_nState == QS_END)
		{
			int j = NULL;
			for (; j < pMover->m_nCompleteQuestSize; j++)
			{
				if (pMover->m_aCompleteQuest[j] == pMover->m_aQuest[i].m_wId)
					break;
			}
			if (j == pMover->m_nCompleteQuestSize)
			{
				pMover->m_aCompleteQuest[pMover->m_nCompleteQuestSize] = pMover->m_aQuest[i].m_wId;
				pMover->m_nCompleteQuestSize++;
			}
			for (j = i; j < nQuestSize - 1; j++)
			{
				pMover->m_aQuest[j] = pMover->m_aQuest[j + 1];
			}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
			for (int k = 0; k < pMover->m_nCheckedQuestSize; ++k)
			{
				if (pMover->m_aCheckedQuest[k] == pMover->m_aQuest[i].m_wId)
				{
					for (int l = k; l < pMover->m_nCheckedQuestSize - 1; ++l)
						pMover->m_aCheckedQuest[k] = pMover->m_aCheckedQuest[k + 1];
					pMover->m_aCheckedQuest[--pMover->m_nCheckedQuestSize] = 0;
					break;
				}
			}
#endif // __IMPROVE_QUEST_INTERFACE
			nQuestSize--;
			i--;
		}
		else
		{
			int j = 0;
			do
			{
				for (j = i + 1; j < nQuestSize; j++)
				{
					if (pMover->m_aQuest[j].m_wId == pMover->m_aQuest[i].m_wId)
						break;
				}
				if (j != nQuestSize)
				{
					for (int k = j; k < nQuestSize - 1; k++)
					{
						pMover->m_aQuest[k] = pMover->m_aQuest[k + 1];
					}
					nQuestSize--;
				}
			} while (j != nQuestSize);
		}
	}
	pMover->m_nQuestSize = nQuestSize;
	return TRUE;
}

BOOL CDbManager::GetSMMode(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	int CountStr = 0;
	int IndexSM = 0;
	char SMCnt[3072] = { 0, };
	qry->GetStr("m_dwSMTime", SMCnt);
	if (0 != strcmp(SMCnt, "NULL"))
	{
		VERIFYSTRING_RETURN(SMCnt, pMover->m_szName);
		while ('$' != SMCnt[CountStr])
		{
			pMover->m_dwSMTime[IndexSM] = (DWORD)GetIntFromStr(SMCnt, &CountStr);
			switch (IndexSM)
			{
			case SM_MAX_HP50:
			{
				pMover->m_nPlusMaxHitPoint = (LONG)GetIntFromStr(SMCnt, &CountStr);
			}
			break;
			}
			++IndexSM;
		}
	}
	return TRUE;
}

BOOL CDbManager::GetSKillInfluence(CMover* pMover, CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	int CountStr = 0;
	int iSkillIndex = 0;
	char szSKillInfluence[7500] = { 0, };
	qry->GetStr("SkillInfluence", szSKillInfluence);
	VERIFYSTRING_RETURN(szSKillInfluence, pMover->m_szName);
	while ('$' != szSKillInfluence[CountStr])
	{
		WORD wType = (WORD)GetIntFromStr(szSKillInfluence, &CountStr);
		WORD wId = (WORD)GetIntFromStr(szSKillInfluence, &CountStr);
		DWORD dwLevel = (DWORD)GetIntFromStr(szSKillInfluence, &CountStr);
		DWORD tmTotal = GetIntFromStr(szSKillInfluence, &CountStr);
		if (wType == 0 && wId == 0 && dwLevel == 0 && tmTotal == 0)
			break;
		IBuff* pBuff = pMover->m_buffs.CreateBuff(wType);
		pBuff->SetId(wId);
		pBuff->SetLevel(dwLevel);
		pBuff->SetTotal(tmTotal);
		bool bResult = pMover->m_buffs.Add(pBuff);
		ASSERT(bResult);

		++iSkillIndex;
	}
	return TRUE;
}

int CDbManager::GetOneItem(CItemElem* pItemElem, char* pstrItem, int* pLocation)
{
	int IndexItem = 0;
	IndexItem = GetIntPaFromStr(pstrItem, pLocation);
	pItemElem->m_dwItemId = (DWORD)GetIntPaFromStr(pstrItem, pLocation);
	GetStrPaFromStr(pstrItem, pItemElem->m_szItemText, pLocation);
	pItemElem->m_nItemNum = GetIntPaFromStr(pstrItem, pLocation);

	if (CheckValidItem(pItemElem->m_dwItemId, pItemElem->m_nItemNum) == FALSE)
		pItemElem->m_nItemNum = 1;

	pItemElem->m_nHitPoint = GetIntPaFromStr(pstrItem, pLocation);
	pItemElem->m_byFlag = (BYTE)GetIntPaFromStr(pstrItem, pLocation);
	pItemElem->SetSerialNumber(GetSerialNumberPaFromStr(pstrItem, pLocation));
	pItemElem->SetOption(GetIntPaFromStr(pstrItem, pLocation));
	pItemElem->m_bItemResist = (BYTE)GetIntPaFromStr(pstrItem, pLocation);
	pItemElem->m_nResistAbilityOption = GetIntPaFromStr(pstrItem, pLocation);
#ifdef __MODEL_CHANGE
	DWORD dwItemModel = (DWORD)GetIntPaFromStr(pstrItem, pLocation);
	if (dwItemModel <= 0)
		dwItemModel = NULL_ID;
	pItemElem->SetLook(dwItemModel);
#endif


	pItemElem->m_dwObjId = IndexItem;

	++*pLocation;
	return IndexItem;
}

void CDbManager::GetOneSkill(LPSKILL pSkill, char* pstrSkill, int* pLocation)
{
	(DWORD)GetIntPaFromStr(pstrSkill, pLocation);
	pSkill->dwLevel = (DWORD)GetIntPaFromStr(pstrSkill, pLocation);
	pSkill->dwSkill = (DWORD)GetIntPaFromStr(pstrSkill, pLocation);
	(BOOL)GetIntPaFromStr(pstrSkill, pLocation);

	++*pLocation;
}

void CDbManager::GetOneQuest(LPQUEST pQuest, char* pstrQuest, int* pLocation)
{
	pQuest->m_wId = (WORD)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_nState = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_wTime = (WORD)GetIntPaFromStr(pstrQuest, pLocation);



	pQuest->m_nKillNPCNum[0] = (WORD)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_nKillNPCNum[1] = (WORD)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bPatrol = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve2 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve3 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve4 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve5 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve6 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve7 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);
	pQuest->m_bReserve8 = (BYTE)GetIntPaFromStr(pstrQuest, pLocation);

	++*pLocation;
}


BOOL CDbManager::GetRemoveItem(CQuery* pQry, int& nNo, char* pOneItem, int& nItem_Count, int& nAbilityOption, int& nItemResist, int& nResistAbilityOption, char& chState)
{
	nNo = pQry->GetInt("m_nNo");
	pQry->GetStr("Item_Name", pOneItem);

	nItem_Count = pQry->GetInt("Item_count");
	nAbilityOption = pQry->GetInt("m_nAbilityOption");			// AbilityOption ( + ) Option
	chState = pQry->GetChar("State");
	nItemResist = pQry->GetInt("m_bItemResist");
	nResistAbilityOption = pQry->GetInt("m_nResistAbilityOption");

	if (0 == strcmp(pOneItem, "NULL"))
	{
		WriteLog("Remove NULL Item_Name : %s", pOneItem);
		return FALSE;
	}

	if (chState != 'I' && chState != 'B' && chState != 'P')
	{
		WriteLog("Remove Not Data State : %c", chState);
		return FALSE;
	}


	if (nItem_Count == -100 || nItem_Count == 0)
		nItem_Count = 1;

	if (nAbilityOption == -100)
		nAbilityOption = 0;

	if (nItemResist == -100)
		nItemResist = 0;

	if (nResistAbilityOption == -100)
		nResistAbilityOption = 0;


	return TRUE;
}

BOOL CDbManager::RemovePenya(CQuery* pQry, char* szSql, CMover* pMover, int nItem_Count, int nNo, char chState)
{
	if (RemoveItemDeleteQuery(pQry, szSql, nNo) == FALSE)
		return FALSE;

	if (chState == 'I')	// Inventory
	{
		pMover->AddGold(-nItem_Count, FALSE);
	}
	else	// Bank
	{
		DWORD dwGoldBank = pMover->m_dwGoldBank[pMover->m_nSlot];
		pMover->m_dwGoldBank[pMover->m_nSlot] -= nItem_Count;
		if (dwGoldBank < pMover->m_dwGoldBank[pMover->m_nSlot])
			pMover->m_dwGoldBank[pMover->m_nSlot] = 0;
	}

	return TRUE;
}

SERIALNUMBER CDbManager::RemoveItem(CQuery* pQry, char* szSql, int nNo, CMover* pMover, char* szItemName, int nItemCount, int nAbilityOption, int nItemResist, int nResistAbilityOption, char chState)
{
	SERIALNUMBER iSerialNumber = 0;

	int nIndex = atoi(szItemName);
	ItemProp* pItemProp = NULL;
	if (nIndex > 0)
		pItemProp = prj.GetItemProp(nIndex);
	else
		pItemProp = prj.GetItemProp(szItemName);

	if (pItemProp)
	{
		if (RemoveItemDeleteQuery(pQry, szSql, nNo) == FALSE)
			return 0;

		switch (chState)
		{
		case 'I':
			iSerialNumber = RemoveItemInventory(pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount);
			break;

		case 'B':
			iSerialNumber = RemoveItemBank(pItemProp, pMover, nAbilityOption, nItemResist, nResistAbilityOption, nItemCount);
			break;
		}


		if (iSerialNumber == 0)
			return 0;
		else
			return iSerialNumber;
	}
	else
	{	// no propItem
		return 0;
	}
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemInventory(ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount)
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem* pItemElem = pMover->m_Inventory.GetItem(pItemProp, nAbilityOption, nItemResist, nResistAbilityOption);

	if (pItemElem != NULL)
	{
		iSerialNumber = pItemElem->GetSerialNumber();
		if (pMover->m_Inventory.IsEquip(pItemElem->m_dwObjId))
		{
			if (!pMover->m_Inventory.UnEquip(pItemProp->dwParts))
			{
				WriteLog("Not UnEquip : %s, %s ", pMover->m_szName, pItemProp->szName);
				return (SERIALNUMBER)0;
			}
			else
				pMover->m_aEquipInfo[pItemProp->dwParts].dwId = NULL_ID;
		}

		if (pItemElem->m_nItemNum <= nItemCount)
			pMover->m_Inventory.RemoveAt(pItemElem->m_dwObjIndex);
		else
			pItemElem->m_nItemNum -= nItemCount;

		return iSerialNumber;
	}
	else
	{
		WriteLog("Not Inventory : %s, %s ", pMover->m_szName, pItemProp->szName);
		return 0;
	}
	return 0;
}

SERIALNUMBER CDbManager::RemoveItemBank(ItemProp* pItemProp, CMover* pMover, int nAbilityOption, int nItemResist, int nResistAbilityOption, int nItemCount)
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem* pItemElem = pMover->m_Bank[pMover->m_nSlot].GetItem(pItemProp, nAbilityOption, nItemResist, nResistAbilityOption);

	if (pItemElem != NULL)
	{
		iSerialNumber = pItemElem->GetSerialNumber();

		if (pItemElem->m_nItemNum <= nItemCount)
			pMover->m_Bank[pMover->m_nSlot].RemoveAt(pItemElem->m_dwObjIndex);
		else
			pItemElem->m_nItemNum -= nItemCount;

		return iSerialNumber;
	}
	else
	{
		WriteLog("item not found - bank: %s, %s ", pMover->m_szName, pItemProp->szName);
		return 0;
	}
	return 0;
}

BOOL CDbManager::IsAbnormalPlayerData(CMover* pMover)
{
	DWORD dwId;
	int nMax = pMover->m_Inventory.m_dwItemMax;
	int nNumberofIndex = pMover->m_Inventory.m_dwIndexNum;

	for (int i = 0; i < nMax; i++)
	{
		dwId = pMover->m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
		{
			if (pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex != i)
				return TRUE;
		}
	}

	DWORD dwObjIndex;
	for (int i = 0; i < nMax; i++)
	{
		dwObjIndex = pMover->m_Inventory.m_apItem[i].m_dwObjIndex;
		if (dwObjIndex != NULL_ID)
		{
			if (pMover->m_Inventory.m_apIndex[dwObjIndex] != i)
				return TRUE;
		}
		if (FALSE == pMover->m_Inventory.m_apItem[i].IsEmpty())
		{
			ItemProp* pItemProp
				= prj.GetItemProp(pMover->m_Inventory.m_apItem[i].m_dwItemId);
			if (!pItemProp	/*It have not property*/
				|| pItemProp->dwItemKind3 == IK3_VIRTUAL/*It is monster's virtual weapon*/)
				return TRUE;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset(adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS);
	for (int i = nNumberofIndex; i < nMax; i++)
	{
		dwId = pMover->m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
			adwEquipment[i - nNumberofIndex] = dwId;
	}
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (adwEquipment[i] != pMover->m_aEquipInfo[i].dwId)
			return TRUE;
	}

	return FALSE;
}

BOOL CDbManager::NormalizePlayerData(CMover* pMover)
{
	int nMax = pMover->m_Inventory.m_dwItemMax;
	int nNumberofIndex = pMover->m_Inventory.m_dwIndexNum;
	DWORD dwId;
	for (int i = 0; i < nMax; i++)
	{
		dwId = pMover->m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
		{
			if (pMover->m_Inventory.m_apItem[dwId].m_dwObjIndex != i)
			{

				DWORD dwIndex = NULL_ID;
				for (int j = 0; j < nMax; j++)
				{
					if (pMover->m_Inventory.m_apItem[j].m_dwObjIndex == i)
					{
						dwIndex = j;
						break;
					}
				}
				if (dwIndex != NULL_ID)
				{
					pMover->m_Inventory.m_apIndex[i] = dwIndex;
				}
				else
				{
					if (i >= nNumberofIndex)
					{
						pMover->m_Inventory.m_apIndex[i] = NULL_ID;
					}
					else
					{
						int j = NULL;
						for (; j < nMax; j++)
						{
							if (pMover->m_Inventory.m_apItem[j].m_dwObjIndex == NULL_ID)
							{
								pMover->m_Inventory.m_apItem[j].m_dwObjIndex = i;
								pMover->m_Inventory.m_apIndex[i] = j;
								break;
							}
						}
						if (j == nMax) {
							ASSERT(0);
							return FALSE;
						}
					}
				}
			}
		}
	}

	DWORD dwObjIndex;
	for (int i = 0; i < nMax; i++)
	{
		dwObjIndex = pMover->m_Inventory.m_apItem[i].m_dwObjIndex;
		if (dwObjIndex != NULL_ID)
		{
			if (pMover->m_Inventory.m_apIndex[dwObjIndex] != i)
				pMover->m_Inventory.m_apItem[i].m_dwObjIndex = NULL_ID;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset(adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS);
	for (int i = nNumberofIndex; i < nMax; i++)
	{
		dwId = pMover->m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
			adwEquipment[i - nNumberofIndex] = dwId;
	}
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (pMover->m_aEquipInfo[i].dwId != adwEquipment[i])
			pMover->m_aEquipInfo[i].dwId = adwEquipment[i];
	}
	return TRUE;
}

#ifdef __FL_CONSUMABLES_STORAGE
#ifndef __FL_PACKED_ITEMS
BOOL CDbManager::GetChest(CMover* pMover, CQuery* qry)
#else
BOOL CDbManager::GetChest(CMover* pMover, CQuery* qry, CQuery* qry1)
#endif
{
	CString strQuery;
	strQuery.Format("uspChestLoad '%02d','%07d'", g_appInfo.dwSys, pMover->m_idPlayer);

	if (!qry->Exec(strQuery))
	{
		Error("uspChestLoad err");
		return FALSE;
	}

	if (qry->Fetch())
	{
		ItemContainerStruct	ics;
		sprintf(ics.szItem, "m_Chest");
		sprintf(ics.szIndex, "m_apIndexChest");
		sprintf(ics.szObjIndex, "m_dwObjIndexChest");
		sprintf(ics.szExt, "m_extChest");
		sprintf(ics.szPiercing, "m_ChestPiercing");
		sprintf(ics.szPet, "szChestPet");

		if (!GetContainer(qry, &pMover->m_Chest, &ics))
		{
			Error("Can't load chest %d", pMover->m_idPlayer);
			return FALSE;
		}

#ifdef __FL_PACKED_ITEMS
		return LoadPackages(qry1, &pMover->m_Chest, pMover->m_idPlayer, PRT_CHEST);
#endif
	}

	return TRUE;
}
#endif

#ifdef __FL_SWITCH_EQ
void CDbManager::GetSwitches(CMover* pMover, CQuery* qry)
{
	CString strQuery;
	strQuery.Format("uspSwitchesLoad '%02d','%07d'", g_appInfo.dwSys, pMover->m_idPlayer);

	if (!qry->Exec(strQuery))
	{
		Error("uspSwitchesLoad err");
		return;
	}

	if (!qry->Fetch())
		return;

	auto vecSwitches = pMover->m_Switches.GetSwitches();
	vecSwitches->clear();

	char SwitchesNames[2048] = { 0, };
	char SwitchesParts[2048] = { 0, };
	char SwitchesOther[2048] = { 0, };

	qry->GetStr("SwitchesNames", SwitchesNames);
	qry->GetStr("SwitchesParts", SwitchesParts);
	qry->GetStr("SwitchesOther", SwitchesOther);

	EQ_SWITCH tmp;

	int CountStr = 0;
	int nIndex = 0;
	char szTmp[64] = { 0, };

	while ('$' != SwitchesNames[CountStr])
	{
		GetStrFromStr(SwitchesNames, szTmp, &CountStr);
		char szName[32] = { 0, };
		GetDBFormatStr(szName, 32, szTmp);
		sprintf(tmp.szSwitchName, "%s", szName);

		vecSwitches->push_back(tmp);
	}

	CountStr = 0;
	nIndex = 0;
	while ('$' != SwitchesParts[CountStr])
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			vecSwitches->at(nIndex).dwItem[i] = (DWORD)GetIntPaFromStr(SwitchesParts, &CountStr, -1);

		++CountStr;
		++nIndex;
	}

	CountStr = 0;
	nIndex = 0;
	while ('$' != SwitchesOther[CountStr])
	{
		vecSwitches->at(nIndex).dwLooterPet = (DWORD)GetIntPaFromStr(SwitchesOther, &CountStr, -1);
		vecSwitches->at(nIndex).dwPet = (DWORD)GetIntPaFromStr(SwitchesOther, &CountStr, -1);
		vecSwitches->at(nIndex).dwRegen = (DWORD)GetIntPaFromStr(SwitchesOther, &CountStr, -1);

		++CountStr;
		++nIndex;
	}

}
#endif

#ifndef __FL_PACKED_ITEMS
void CDbManager::OpenQueryGuildBank(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
#else
void CDbManager::OpenQueryGuildBank(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, CQuery* pQuery1)
#endif
{
	CAr	ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	char szSql[128];
	sprintf(szSql, "GUILD_BANK_STR 'S1','0','%02d'", g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szSql))
	{

		return;
	}

	while (1)
	{
		BEFORESENDDUAL(ar2, PACKETTYPE_GUILD_BANK, DPID_UNKNOWN, DPID_UNKNOWN);

		int							nGoldGuild;
		int							nGuildId = 0;
		int							nBufsize = 0;
		int							nCount = 0;
		u_long						ulOffSet = ar2.GetOffset();


		ar2 << static_cast<int>(0);
		BOOL bFetch = FALSE;
		while (bFetch = pQuery->Fetch())
		{
			nCount++;
			CItemContainer<CItemElem>	GuildBank;
			GuildBank.SetItemContainer(ITYPE_ITEM, MAX_GUILDBANK);
			//		GuildBank.Clear();

			nGuildId = pQuery->GetInt("m_idGuild");
			nGoldGuild = pQuery->GetInt("m_nGuildGold");

			ItemContainerStruct	ics;
			sprintf(ics.szItem, "m_GuildBank");
			sprintf(ics.szIndex, "m_apIndex");
			sprintf(ics.szObjIndex, "m_dwObjIndex");
			sprintf(ics.szExt, "m_extGuildBank");
			sprintf(ics.szPiercing, "m_GuildBankPiercing");
			sprintf(ics.szPet, "szGuildBankPet");

			if (!GetContainer(pQuery, &GuildBank, &ics))
			{
				Error("Can't load guildbank %d", nGuildId);
				return;
			}

#ifdef __FL_PACKED_ITEMS
			if (!LoadPackages(pQuery1, &GuildBank, nGuildId, PRT_GUILDBANK))
			{
				Error("Can't load packages for guild %d", nGuildId);
				return;
			}
#endif

			ar2 << nGuildId;
			ar2 << nGoldGuild;
			GuildBank.Serialize(ar2);

			if (nCount >= 1000)
				break;
		}

		BYTE* pBuf = ar2.GetBuffer(&nBufsize);

		*(UNALIGNED int*)(pBuf + ulOffSet) = nCount;

		SEND(ar2, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid);

		if (bFetch == FALSE)
			break;
	}
}

#ifdef __FL_VENDORS
#ifndef __FL_PACKED_ITEMS
void CDbManager::GetVendors(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
#else
void CDbManager::GetVendors(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, CQuery* pQuery1)
#endif
{
	int nCount = 0;

	BEFORESENDDUAL(out, PACKETTYPE_VENDORS_GET, DPID_UNKNOWN, DPID_UNKNOWN);
	u_long uOffset = out.GetOffset();
	out << nCount;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspOfflineVendorGet '%02d'", g_appInfo.dwSys);
	if (!pQueryChar->Exec(szQuery))
	{
		Error("uspOfflineVendorGet[Line:%u] Failed, Query: %s", __LINE__, szQuery);
		return;
	}

	while (pQueryChar->Fetch())
	{
		// Vendor details
		char        szPlayerID[16];
		pQueryChar->GetStr("m_idPlayer", szPlayerID);
		u_long      idPlayer = (DWORD)atoi(szPlayerID);
		DWORD      idWorld = (DWORD)pQueryChar->GetInt("dwWorldID");
		D3DXVECTOR3 vPos;
		vPos.x = pQueryChar->GetFloat("m_vPos_x");
		vPos.y = pQueryChar->GetFloat("m_vPos_y");
		vPos.z = pQueryChar->GetFloat("m_vPos_z");
		FLOAT fAngle = pQueryChar->GetFloat("m_fAngle");
		DWORD dwSkillTime = (DWORD)pQueryChar->GetInt("dwVendorTime");
		DWORD dwVendorItem = (DWORD)pQueryChar->GetInt("dwVendorItem");
		char        szShopName[64];
		pQueryChar->GetStr("szVendorName", szShopName);

		CItemContainer< CItemElem > tmpInventory;
		tmpInventory.SetItemContainer(ITYPE_ITEM, MAX_VENDITEM);

		ItemContainerStruct	ics;
		sprintf(ics.szItem, "szItem");
		sprintf(ics.szIndex, "szIndex");
		sprintf(ics.szObjIndex, "szObjIndex");
		sprintf(ics.szExt, "szExt");
		sprintf(ics.szPiercing, "szPiercing");
		sprintf(ics.szPet, "szPet");

		if (!GetContainer(pQueryChar, &tmpInventory, &ics))
		{
			Error("Can't load vendor %d", idPlayer);
			return;
		}

#ifdef __FL_PACKED_ITEMS
		if (!LoadPackages(pQuery1, &tmpInventory, idPlayer, PRT_VENDORS))
		{
			Error("Can't load packages for vendor %d", idPlayer);
			return;
		}
#endif

		// Price
		char szPrice[1024] = { 0, };
		pQueryChar->GetStr("szPrice", szPrice);

		int nCost[MAX_VENDITEM] = { };
		int nPerinCost[MAX_VENDITEM] = { };

		int IndexItem = 0;
		int CountStr = 0;

		while ('$' != szPrice[CountStr])
		{
			nCost[IndexItem] = GetIntPaFromStr(szPrice, &CountStr);
			nPerinCost[IndexItem] = GetIntPaFromStr(szPrice, &CountStr);

			++CountStr;
			++IndexItem;
		}

		// Owner details
		char szOwnerName[64];
		pQueryChar->GetStr("m_szName", szOwnerName);
		DWORD dwSexIndex = (DWORD)pQueryChar->GetInt("m_dwSex");
		DWORD dwHairId = (DWORD)pQueryChar->GetInt("m_dwHairMesh");
		DWORD dwHairColorId = (DWORD)pQueryChar->GetInt("m_dwHairColor");
		DWORD dwHeadId = (DWORD)pQueryChar->GetInt("m_dwHeadMesh");
		DWORD dwLevel = (DWORD)pQueryChar->GetInt("m_nLevel");
		DWORD dwJob = (DWORD)pQueryChar->GetInt("m_nJob");

		CItemContainer< CItemElem > tmpInventoryOwner;
		tmpInventoryOwner.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ItemContainerStruct	icsOwner;
		sprintf(icsOwner.szItem, "m_Inventory");
		sprintf(icsOwner.szObjIndex, "m_dwObjIndex");
		GetContainer(pQueryChar, &tmpInventoryOwner, &icsOwner);

		// Vendor details
		out << idPlayer << idWorld << vPos << fAngle << dwSkillTime << dwVendorItem;
		out.WriteString(szShopName);
		tmpInventory.Serialize(out);

		// Serialize prices
		int nItemsCount = 0;
		u_long uOffsetItemsCount = out.GetOffset();
		out << nItemsCount;
		for (int i = 0; i < MAX_VENDITEM; i++)
		{
			if (nCost[i] == 0 && nPerinCost[i] == 0)
				continue;

			out << (BYTE)i;
			out << nCost[i];
			out << nPerinCost[i];

			nItemsCount++;
		}

		int nBufSize1;
		LPBYTE lpBuf1 = out.GetBuffer(&nBufSize1);
		*(UNALIGNED int*)(lpBuf1 + uOffsetItemsCount) = nItemsCount;

		out << dwSexIndex << dwHairId << dwHairColorId << dwHeadId << dwLevel << dwJob;
		out.WriteString(szOwnerName);
		tmpInventoryOwner.Serialize(out);

		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer = out.GetBuffer(&nBufferSize);
	*(UNALIGNED int*)(pBuffer + uOffset) = nCount;
	SEND(out, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid);
}
#endif

#ifdef __FL_EQUALIZED_MODE
void CDbManager::LoadTemporalEquipment(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	int nCount = 0;
	DPID dpidMulti = lpDbOverlappedPlus->dpid;
	char szQuery[QUERY_SIZE] = { 0, };

	BEFORESENDDUAL(out, PACKETTYPE_GETEQUIPPRESETS, DPID_UNKNOWN, DPID_UNKNOWN);
	u_long uOffset = out.GetOffset();
	out << nCount;

	sprintf(szQuery, "uspTemporalPresetsGet");
	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspTemporalPresetsGet[Line:%u] Failed, Query: %s", __LINE__, szQuery);
		return;
	}
	while (pQueryChar->Fetch())
	{
		DWORD       dwWorldID = (DWORD)pQueryChar->GetInt("dwWorldID");
		DWORD       dwSex = (DWORD)pQueryChar->GetInt("dwSex");
		DWORD       dwJob = (DWORD)pQueryChar->GetInt("dwJob");

		CItemContainer< CItemElem > tmpInventory;
		tmpInventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ItemContainerStruct	ics;
		sprintf(ics.szItem, "szItem");
		sprintf(ics.szIndex, "szIndex");
		sprintf(ics.szObjIndex, "szObjIndex");
		sprintf(ics.szExt, "szExt");
		sprintf(ics.szPiercing, "szPiercing");
		sprintf(ics.szPet, "szPet");
		GetContainer(pQueryChar, &tmpInventory, &ics);

		CItemElem* pItem = NULL;
		for (int i = 0; i < tmpInventory.m_dwItemMax; i++)
		{
			pItem = tmpInventory.GetAtId(i);
			if (pItem)
				pItem->SetFlag(CItemElem::binds);
		}

		out << dwWorldID << dwSex << dwJob;
		tmpInventory.Serialize(out);
		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer = out.GetBuffer(&nBufferSize);
	*(UNALIGNED int*)(pBuffer + uOffset) = nCount;
	SEND(out, CDPTrans::GetInstance(), dpidMulti);
}
#endif

#ifdef __FL_PACKED_ITEMS
BOOL CDbManager::LoadPackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType)
{
	CString strQuery;
	strQuery.Format("uspPackedItemsLoad '%02d','%07d', %d", g_appInfo.dwSys, idOwner, nType);

	if (!qry->Exec(strQuery))
	{
		Error("Cant execute %s", strQuery);
		return FALSE;
	}

	CItemContainer<CItemElem> packageContainer;
	packageContainer.SetItemContainer(ITYPE_ITEM, MAX_ITEMS_IN_PACKAGE);

	DWORD dwPackageObjId = NULL_ID;

	while (qry->Fetch())
	{
		dwPackageObjId = (DWORD)qry->GetInt("dwPackageObjId");

		CItemElem* pPackageElem = pContainer->GetAtId(dwPackageObjId);
		if (!pPackageElem || !pPackageElem->IsPackage())
		{
			Error("LoadPackedItems: cannot find package %d for: %d owner, %d nType. ItemElem: %d.", dwPackageObjId, idOwner, nType, pPackageElem ? pPackageElem->m_dwItemId : 0);
			continue;
		}

		if (!pPackageElem->GetItemList()->empty())
			Error("Package is not empty: %d owner, %d nType", idOwner, nType);

		GetOnePackage(qry, &packageContainer, pPackageElem);

		packageContainer.Clear();
	}

	return TRUE;
}
void CDbManager::GetOnePackage(CQuery* qry, CItemContainer<CItemElem>* pContainer, CItemElem* pItemElem)
{
	ItemContainerStruct	ics;
	sprintf(ics.szItem, "szItem");
	sprintf(ics.szExt, "szExt");
	sprintf(ics.szPiercing, "szPiercing");
	sprintf(ics.szPet, "szPet");
	GetContainer(qry, pContainer, &ics);

	pItemElem->ClearPackedItems();
	for (int i = 0; i < pContainer->m_dwItemMax; i++)
	{
		CItemElem* pItemInPackage = pContainer->GetAtId(i);
		if (!pItemInPackage)
			continue;

		pItemElem->AddToPack(pItemInPackage);
	}
}
void CDbManager::SavePackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType)
{
	if (UpdatePackages(qry, idOwner, nType))
	{
		InsertPackages(qry, pContainer, idOwner, nType);
		DeletePackages(qry, idOwner, nType);
	}
	else
		Error("Packages Save Failed. idOwner:[%07d] nType:[%d]. Container: %d", idOwner, nType, pContainer ? TRUE : FALSE);
}
bool CDbManager::UpdatePackages(CQuery* qry, u_long idOwner, int nType)
{
	CString strQuery;
	strQuery.Format("uspPackedItemsUpdate '%02d','%07d', %d", g_appInfo.dwSys, idOwner, nType);

	if (!qry->Exec(strQuery))
	{
		Error("Cant execute %s", strQuery);
		return false;
	}

	if (qry->Fetch())
	{
		int nError = qry->GetInt("fError");

		if (nError == 0)
			return true;
		else
		{
			Error("UpdatePackages Failed. idOwner:[%07d] nType:[%d]", idOwner, nType);
			return false;
		}
	}

	Error("UpdatePackages Fetch Failed. idOwner:[%07d] nType:[%d]", idOwner, nType);
	return false;
}
void CDbManager::InsertPackages(CQuery* qry, CItemContainer<CItemElem  >* pContainer, u_long idOwner, int nType)
{
	if (!pContainer)
		return;

	CItemContainer<CItemElem> packageContainer;
	packageContainer.SetItemContainer(ITYPE_ITEM, MAX_ITEMS_IN_PACKAGE);

	for (int i = 0; i < pContainer->m_dwItemMax; i++)
	{
		CItemElem* pItemElem = pContainer->GetAtId(i);
		if (!pItemElem || !pItemElem->IsPackage())
			continue;

		if (pItemElem->GetItemList()->empty())
			continue;

		ItemContainerStruct	ics;
		SaveOnePackage(pItemElem, &packageContainer, &ics);

		CString strQuery;
		strQuery.Format("uspPackedItemsInsert '%02d','%07d', %d, %d, '%s', '%s', '%s', '%s'", g_appInfo.dwSys, idOwner, nType, pItemElem->m_dwObjId, ics.szItem, ics.szExt, ics.szPiercing, ics.szPet);

		if (!qry->Exec(strQuery))
			Error("Cant execute %s", strQuery);

		packageContainer.Clear();
	}
}
void CDbManager::SaveOnePackage(CItemElem* pItemElem, CItemContainer<CItemElem>* pContainer, ItemContainerStruct* pStruct)
{
	for (auto& it : *pItemElem->GetItemList())
		pContainer->Add(it);

	SaveContainer(pContainer, pStruct);
}
void CDbManager::DeletePackages(CQuery* qry, u_long idOwner, int nType)
{
	CString strQuery;
	strQuery.Format("uspPackedItemsDelete '%02d','%07d', %d", g_appInfo.dwSys, idOwner, nType);

	if (!qry->Exec(strQuery))
		Error("Cant execute %s", strQuery);
}
#endif

#ifdef __FL_RECORD_BOOK
void CDbManager::LoadRecords(CMover* pMover, CQuery* qry)
{
	CString strQuery;
	strQuery.Format("uspRecordsLoad '%02d','%07d'", g_appInfo.dwSys, pMover->m_idPlayer);

	if (!qry->Exec(strQuery))
	{
		Error("uspRecordsLoad err %s", strQuery);
		return;
	}

	if (!qry->Fetch())
		return;

	for (int i = 0; i < RecordType::MAX_RECORDTYPE; i++)
	{
		char szRecord[32];
		sprintf(szRecord, "rc_%d", i);

		char szRecordString[128] = { 0, };
		qry->GetStr(szRecord, szRecordString);

		int CountStr = 0;

		pMover->m_arrayRecords[i].nLevel = GetIntPaFromStr(szRecordString, &CountStr);
		pMover->m_arrayRecords[i].nJob = GetIntPaFromStr(szRecordString, &CountStr);
		pMover->m_arrayRecords[i].n64Value = GetInt64PaFromStr(szRecordString, &CountStr);
	}
}
#endif

#ifdef __FL_MONSTER_ALBUM
void CDbManager::LoadMonsterAlbum(CMover* pMover, CQuery* qry)
{
	CString strQuery;
	strQuery.Format("uspMonsterAlbumLoad '%02d','%07d'", g_appInfo.dwSys, pMover->m_idPlayer);

	if (!qry->Exec(strQuery))
	{
		Error("Cant execute %s", strQuery);
		return;
	}

	if (!qry->Fetch())
		return;

	auto pMapMonsters = pMover->m_MonsterAlbum.GetMonsterProgress();
	auto pBossCards = pMover->m_MonsterAlbum.GetBossCards();
	pMapMonsters->clear();

	char stringMonsters[2048] = { 0, };
	char stringBossCards[256] = { 0, };
	qry->GetStr("stringMonsters", stringMonsters);
	qry->GetStr("stringBossCards", stringBossCards);

	int CountStr = 0;
	int nIndex = 0;

	while ('$' != stringMonsters[CountStr])
	{
		DWORD dwID = (DWORD)GetIntPaFromStr(stringMonsters, &CountStr, -1);
		int nProgress = GetIntPaFromStr(stringMonsters, &CountStr);

		pMapMonsters->insert(make_pair(dwID, nProgress));

		++CountStr;
		++nIndex;
	}

	CountStr = 0;
	nIndex = 0;

	while ('$' != stringBossCards[CountStr])
	{
		DWORD dwID = (DWORD)GetIntPaFromStr(stringBossCards, &CountStr, -1);

		if (nIndex < MAX_BOSS_CARDS)
			pBossCards->at(nIndex) = dwID;

		++CountStr;
		++nIndex;
	}
}
#endif