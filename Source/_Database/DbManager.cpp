#include "StdAfx.h"
#include "defineObj.h"
#include "dbmanager.h"
#include "dploginsrvr.h"
#include "dpcoresrvr.h"
#include "dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include "mytrace.h"
#include "definetext.h"

#include "party.h"
#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#include "dpaccountclient.h"
extern	CDPAccountClient	g_dpAccountClient;

#include "serialnumber.h"
#include "post.h"

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouseDBCtrl.h"
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

CDbManager& g_DbManager = CDbManager::GetInstance();

extern const int MAX_TASKBAR;
extern const int MAX_APPLETTASKBAR;
extern const int MAX_ITEMTASKBAR;
extern const char NullStr[2];

#define	SAFE_DELETE_FREEREQUEST( pDBOverlappedPlus )	\
	if( pDBOverlappedPlus )	\
	{	\
		FreeRequest_V21( pDBOverlappedPlus );	\
		pDBOverlappedPlus = NULL;	\
	}	\


CDbManager& CDbManager::GetInstance()
{
	static CDbManager DbManager;
	return DbManager;
}

CDbManager::CDbManager()
{
	memset((void*)m_aszJoin, 0, sizeof(char) * MAX_JOIN_SIZE * MAX_PLAYER);
	m_nJoin = 0;

	m_idPlayer = 0;
	m_pDbIOData = new MemPooler<DB_OVERLAPPED_PLUS>(1024);

#ifdef __S1108_BACK_END_SYSTEM
	m_hWorker = m_hCloseWorker = NULL;
#endif // __S1108_BACK_END_SYSTEM

	memset(m_apQuery, 0, sizeof(m_apQuery));

	DB_ADMIN_PASS_LOG[0] = '\0';
	DB_ADMIN_PASS_CHARACTER01[0] = '\0';
	DB_ADMIN_PASS_BACKSYSTEM[0] = '\0';
	m_cbTrade = 0;
}

CDbManager::~CDbManager()
{
	Clear();
}

void CDbManager::Clear(void)
{
	CloseDbWorkers();

	CMclAutoLock Lock(m_AddRemoveLock);

	m_AccountCacheMgr.Clear();

	SAFE_DELETE(m_pDbIOData);
}

void CDbManager::CreatePlayer(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT);
	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD);

	BYTE nSlot, nFace, nCostume, nSkinSet, nHairMesh;
	DWORD dwHairColor;
	BYTE nSex, nJob, nHeadMesh;
	int nBankPW = 0;

	arRead >> nSlot;
	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szPlayer, MAX_PLAYER);

	if (IsInvalidGameName(lpDbOverlappedPlus->AccountInfo.szPlayer) || IsInvalidAccountName(lpDbOverlappedPlus->AccountInfo.szAccount) || IsInvalidPasswordName(lpDbOverlappedPlus->AccountInfo.szPassword))
	{
		CString str;
		str.Format("CDbManager::CreatePlayer() err: %s - IGN, %s - account, %s - password", lpDbOverlappedPlus->AccountInfo.szPlayer, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword);
		Error(str);
		return;
	}

	prj.Formalize(lpDbOverlappedPlus->AccountInfo.szPlayer);

	arRead >> nFace >> nCostume >> nSkinSet >> nHairMesh;
	arRead >> dwHairColor;
	arRead >> nSex >> nJob >> nHeadMesh;
	arRead >> nBankPW;

	DWORD dwAuthKey;
	arRead >> dwAuthKey;
	arRead >> lpDbOverlappedPlus->dpid;

	if (nSlot < 0 || nSlot > 2)
	{
		Error("CDbManager::CreatePlayer() : wrong slot %d acc %s", nSlot, lpDbOverlappedPlus->AccountInfo.szAccount);
		return;
	}

	DWORD dwIndex = (nSex == SEX_FEMALE ? MI_FEMALE : MI_MALE);
	D3DXVECTOR3	vPos;
	DWORD dwWorldID = WI_WORLD_MADRIGAL;
	if (prj.GetRandomBeginPos(dwWorldID, &vPos) == FALSE)
	{
		ASSERT(0);
	}

	if (nSex != SEX_FEMALE && nSex != SEX_MALE)
		return;

	if (nBankPW < 0 || nBankPW > 9999)
	{
		Error("nBankPW is Invalid! szPlayer : %s, nBankPW : %d", lpDbOverlappedPlus->AccountInfo.szPlayer, nBankPW);
		return;
	}

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "I1", 0, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPlayer, nSlot, dwWorldID,
		dwIndex, vPos.x, vPos.y, vPos.z, '\0', nSkinSet, nHairMesh, dwHairColor, nHeadMesh, nSex);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\r\n\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	if (qry->Fetch())
	{
		int nError;
		u_long nidPlayer;
		nError = qry->GetInt("fError");
		if (nError == 1)
		{
			nidPlayer = qry->GetInt("m_idPlayer");
			PlayerData pd;
			pd.data.nLevel = 1;
			pd.data.nSex = nSex;
			pd.data.nVer = 1;
			lstrcpy(pd.szPlayer, lpDbOverlappedPlus->AccountInfo.szPlayer);
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->AddPlayerData(nidPlayer, pd);
			m_AddRemoveLock.Enter();
			m_AccountCacheMgr.AddMover(nidPlayer, lpDbOverlappedPlus->AccountInfo.szAccount, nSlot);
			m_AddRemoveLock.Leave();

			CDPTrans::GetInstance()->SendAddPlayerData(nidPlayer, pPlayerData);

			g_dpLoginSrvr.SendPlayerList(lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey);
			char szQuery[QUERY_SIZE] = { 0, };
			sprintf(szQuery, "BANK_STR 'U', '%07d', '%02d', '%04d'", nidPlayer, g_appInfo.dwSys, nBankPW);

			if (FALSE == qry->Exec(szQuery))
			{
				WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
				return;
			}
		}
		else if (nError == 2)
		{
			Error("CDbManager::CreatePlayer() : Shit in account: %s", lpDbOverlappedPlus->AccountInfo.szAccount);
			g_dpLoginSrvr.SendError(ERROR_USER_EXISTS, lpDbOverlappedPlus->dpid);
		}
		else
			g_dpLoginSrvr.SendError(ERROR_USER_EXISTS, lpDbOverlappedPlus->dpid);
	}

}


void CDbManager::RemovePlayer(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idPlayer;
	char szNo[255] = { 0, };
	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT);
	arRead.ReadString(lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD);
	arRead.ReadString(szNo, 255);

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidAccountName(lpDbOverlappedPlus->AccountInfo.szAccount) || IsInvalidPasswordName(lpDbOverlappedPlus->AccountInfo.szPassword) || IsInvalidAccountName(szNo))
	{
		CString str;
		str.Format("CDbManager::RemovePlayer() err: %s - account, %s - password, %s - szNo", lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, szNo);
		Error(str);
		return;
	}
#endif

	arRead >> idPlayer;
	DWORD dwAuthKey;
	arRead >> dwAuthKey;

	size_t nSize;	u_long uMessengerId;
	vector<u_long> vecMessenger;
	arRead >> nSize;

	if (nSize > MAX_FRIEND)
	{
		Error("CDbManager::RemovePlayer() : nSize (friend count) = %d | Account %s", nSize, lpDbOverlappedPlus->AccountInfo.szAccount);
		return;
	}

	for (size_t i = 0; i < nSize; i++)
	{
		arRead >> uMessengerId;
		vecMessenger.push_back(uMessengerId);
	}

#ifdef __REMOVE_PLAYER_0221
	arRead >> lpDbOverlappedPlus->dpid;
#endif	// __REMOVE_PLAYER_0221

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "D1", idPlayer, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, szNo);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword);
		return;
	}

	if (qry->Fetch())
	{
		int nError = 0;
		nError = qry->GetInt("fError");
		switch (nError)
		{
		case 1:
			g_dpLoginSrvr.SendError(ERROR_NO_SUCH_GROUP, lpDbOverlappedPlus->dpid);
			return;
		case 2:
			return;
		case 3:
		{
			g_dpLoginSrvr.SendError(ERROR_WARTIME, lpDbOverlappedPlus->dpid);
			return;
		}
		case 4:
		{
			u_long idGuild = qry->GetInt("fText");
			g_dpCoreSrvr.SendDelPlayer(idPlayer, idGuild);
			break;
		}
		}
	}
	else
		return;

	CPlayerDataCenter::GetInstance()->DeletePlayerData(idPlayer);
	m_AddRemoveLock.Enter();
	m_AccountCacheMgr.RemoveMover(idPlayer, lpDbOverlappedPlus->AccountInfo.szAccount);
	m_AddRemoveLock.Leave();

	CDPTrans::GetInstance()->SendDeletePlayerData(idPlayer);
	for (vector<u_long>::iterator it = vecMessenger.begin(); it != vecMessenger.end(); it++)
	{
		qry->Execute("uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, (*it));
		g_dpCoreSrvr.SendRemovePlayerFriend(idPlayer, (*it));
	}
	g_dpLoginSrvr.SendPlayerList(lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, lpDbOverlappedPlus->dpid, dwAuthKey);

}

void CDbManager::ChangeBankPass(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	char szName[33];
	char szNewPass[5];
	u_long uidPlayer;
	arRead.ReadString(szName, 33);
	arRead.ReadString(szNewPass, 5);
	arRead >> uidPlayer;

	//BANK_STR 'U','000001','01','1234'
	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"BANK_STR 'U', '%07d', '%02d', '%s'", uidPlayer, g_appInfo.dwSys, szNewPass);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

}

void CDbManager::SendPlayerList(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
#ifdef __FIX_SQL_INJECTS
	if (IsInvalidAccountName(lpDbOverlappedPlus->AccountInfo.szAccount) || IsInvalidPasswordName(lpDbOverlappedPlus->AccountInfo.szPassword))
	{
		CString str;
		str.Format("CDbManager::SendPlayerList() err: %s - account, %s - password", lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword);
		Error(str);
		return;
	}
#endif

	BEFORESENDSOLE(ar, PACKETTYPE_PLAYER_LIST, lpDbOverlappedPlus->dpid);
	ar << lpDbOverlappedPlus->dwAuthKey;
	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "S2", 0, g_appInfo.dwSys, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s\r\n\t%s, %s", __FILE__, __LINE__, szQuery, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword);

		return;
	}

	u_long ulOffSet = ar.GetOffset();
	ar << (INT)0;

	int countPlayer = 0;
	u_long uidPlayer[5];
	BYTE nslot[5];
	BYTE MessengerSlot[5];
	DWORD	dwMessengerState[5];
	int countMessenger = 0;

	DWORD	adwWorldId[3];
	D3DXVECTOR3	avPos[3];
	memset(adwWorldId, 0, sizeof(adwWorldId));
	ACCOUNT_CACHE* pAccount = NULL;
	g_DbManager.m_AddRemoveLock.Enter();
	{
		pAccount = m_AccountCacheMgr.Find(lpDbOverlappedPlus->AccountInfo.szAccount);
		if (pAccount)
		{
			for (int i = 0; i < 3; i++)
			{
				if (pAccount->pMover[i])
				{
					adwWorldId[i] = pAccount->pMover[i]->m_dwWorldID;
					avPos[i] = pAccount->pMover[i]->m_vPos;
				}
			}
		}
	}
	g_DbManager.m_AddRemoveLock.Leave();

	int	nWhileCount = 0;

	while (qry->Fetch())
	{
		nWhileCount++;

		if (nWhileCount > 3)
			break;

		if (countPlayer > 2)
		{
			WriteLog("%s, %d\t%s : countPlayer > 2", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount);
			break;
		}

		int nError = 0;
		nError = qry->GetInt("fError");
		if (nError == 1 || nError == 2)
		{
			WriteLog("%s, %d\t%s, %s, %d", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount, lpDbOverlappedPlus->AccountInfo.szPassword, nError);
			return;
		}

		int islot = qry->GetInt("playerslot");

		if (islot < 0 || islot > 2)
		{
			WriteLog("%s, %d\t%s SlotError islot [%d]", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount, islot);
			continue;
		}

		for (int nSlotCount = 0; nSlotCount < countPlayer; nSlotCount++)
		{
			if (nslot[nSlotCount] == islot)
			{
				WriteLog("%s, %d\t%s nslot[nSlotCount] == islot", __FILE__, __LINE__, lpDbOverlappedPlus->AccountInfo.szAccount);
				continue;
			}
		}

		nslot[countPlayer] = (BYTE)islot;

		CMover mover;
		mover.InitProp();

		DWORD dwWorldID;
		int nBlock = 0;

		BOOL bRefresh = qry->GetInt("m_idCompany") > 0;

		if (adwWorldId[islot] > 0
			&& bRefresh == FALSE
			)
		{
			dwWorldID = adwWorldId[islot];
			mover.m_vPos = avPos[islot];
		}
		else
		{
			dwWorldID = qry->GetInt("dwWorldID");
			mover.m_vPos.x = qry->GetFloat("m_vPos_x");
			mover.m_vPos.y = qry->GetFloat("m_vPos_y");
			mover.m_vPos.z = qry->GetFloat("m_vPos_z");
		}

		mover.m_dwMode = 0;

		countPlayer++;

		qry->GetStr("m_szName", mover.m_szName);

		char strEnd_Time[13] = { 0, };
		qry->GetStr("BlockTime", strEnd_Time);
		CTime tEnd_Time;

		GetStrTime(&tEnd_Time, strEnd_Time);

		CTime NowTime = CTime::GetCurrentTime();
		if (NowTime > tEnd_Time)
			nBlock = 1;

		{
			mover.m_dwIndex = (DWORD)qry->GetInt("m_dwIndex");
			mover.m_idPlayer = (DWORD)qry->GetInt("m_idPlayer");
			mover.m_idparty = (DWORD)qry->GetInt("m_idparty");
			mover.m_idGuild = (DWORD)qry->GetInt("m_idGuild");

			mover.m_dwSkinSet = (DWORD)qry->GetInt("m_dwSkinSet");
			mover.m_dwHairMesh = (DWORD)qry->GetInt("m_dwHairMesh");
			mover.m_dwHeadMesh = (DWORD)qry->GetInt("m_dwHeadMesh");
			mover.m_dwHairColor = (DWORD)qry->GetInt("m_dwHairColor");
			mover.SetSex((BYTE)qry->GetInt("m_dwSex"));
			mover.m_nJob = (LONG)qry->GetInt("m_nJob");
			mover.m_nLevel = (LONG)qry->GetInt("m_nLevel");
			mover.m_nStr = qry->GetInt("m_nStr");
			mover.m_nSta = qry->GetInt("m_nSta");
			mover.m_nDex = qry->GetInt("m_nDex");
			mover.m_nInt = qry->GetInt("m_nInt");
#ifdef __MODEL_HIDEFASH
			int nHide = qry->GetInt("m_nHideCoat");
			for (int z = 0; z < 6; ++z)
				mover.m_bHideFashion[z] = (nHide >> z) & 1;
#endif

			ItemContainerStruct	ics;
			sprintf(ics.szItem, "m_Inventory");
			sprintf(ics.szIndex, "m_apIndex");
			GetContainer(qry, &mover.m_Inventory, &ics);

			CAr arbuf;
			arbuf << islot;
			arbuf << nBlock;
			arbuf << dwWorldID;
			arbuf << mover.m_dwIndex;
			arbuf.WriteString(mover.m_szName);
			arbuf << mover.m_vPos;
			arbuf << mover.m_idPlayer;
			arbuf << mover.m_idparty;
			arbuf << mover.m_idGuild;
			arbuf << mover.m_dwSkinSet;
			arbuf << mover.m_dwHairMesh;
			arbuf << mover.m_dwHairColor;
			arbuf << mover.m_dwHeadMesh;
			arbuf << mover.GetSex();
			arbuf << mover.m_nJob;
			arbuf << mover.m_nLevel;
			arbuf << (DWORD)0;
			arbuf << mover.m_nStr;
			arbuf << mover.m_nSta;
			arbuf << mover.m_nDex;
			arbuf << mover.m_nInt;
			arbuf << mover.m_dwMode;
#ifdef __MODEL_HIDEFASH
			for (int z = 0; z < 6; z++)
				arbuf << mover.m_bHideFashion[z];

#endif

			MessengerSlot[countMessenger] = islot;
			uidPlayer[countMessenger] = mover.m_idPlayer;
			dwMessengerState[countMessenger] = qry->GetInt("m_nMessengerState");
			countMessenger++;

			u_long uOffset = arbuf.GetOffset();
			arbuf << (int)0;	// reserve

			int countbuf = 0;
			CItemElem* pItemElem;
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				pItemElem = mover.m_Inventory.GetEquip(i);
				if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
				{
					ItemProp* pItemProp = prj.GetItemProp(pItemElem->m_dwItemId);
					if (pItemProp)
					{
						if (pItemProp->dwParts == i || pItemProp->dwPartsub == i ||
							(pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON))
						{
							arbuf << pItemElem->m_dwItemId;
#ifdef __MODEL_CHANGE
							arbuf << pItemElem->GetLook();
#endif
							countbuf++;
						}
					}
				}
			}
			int nBufsize;
			BYTE* pBuf = arbuf.GetBuffer(&nBufsize);
			*(pBuf + uOffset) = countbuf;
			ar.Write(pBuf, nBufsize);
		}
	} // while
	int nBufsize;
	BYTE* pBuf = ar.GetBuffer(&nBufsize);
	*(pBuf + ulOffSet) = countPlayer;


	ar << countMessenger;
	for (int i = 0; i < countMessenger; i++)
	{
		CMover cMoverBuf;
		cMoverBuf.InitProp();
		cMoverBuf.m_idPlayer = uidPlayer[i];
		LoadMessenger(&cMoverBuf, qry);
		cMoverBuf.m_RTMessenger.SetState(dwMessengerState[i]);
		ar << MessengerSlot[i];
		cMoverBuf.m_RTMessenger.Serialize(ar);
	}

	SEND(ar, &g_dpLoginSrvr, DPID_ALLPLAYERS);

}

BOOL CDbManager::OpenGuildCombat(void)
{
	m_nGuildCombatIndex = 0;
	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;
	m_uBestPlayer = 0;
	m_vecGuildCombat.clear();

	CQuery qry;

	if (qry.Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		AfxMessageBox("Error : Not Connect Character DB");
		return FALSE;
	}

	char szQuery[40960];

	sprintf(szQuery, "uspLoadMaxCombatID '%02d'", g_appInfo.dwSys);
	if (FALSE == qry.Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
	}

	if (qry.Fetch())
	{
		m_nGuildCombatIndex = qry.GetInt("MaxNum");


		sprintf(szQuery, "uspLoadCombatInfo %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
		if (FALSE == qry.Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
		}

		if (qry.Fetch())
		{
			int nClose = qry.GetInt("Status");

			if (nClose == 30 || nClose == -100)
			{
				++m_nGuildCombatIndex;
				sprintf(szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
				if (FALSE == qry.Exec(szQuery))
				{
					WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
				}
			}
		}
		else
		{
			sprintf(szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
			if (FALSE == qry.Exec(szQuery))
			{
				WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
			}
		}


		sprintf(szQuery, "uspLoadWinnerGuildInfo %d,'%02d'", m_nGuildCombatIndex - 1, g_appInfo.dwSys);
		if (FALSE == qry.Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);	return FALSE;
		}
		if (qry.Fetch())
		{
			m_uWinGuildId = qry.GetInt("GuildID");
			m_nWinGuildCount = qry.GetInt("StraightWin");
		}


		sprintf(szQuery, "uspLoadCombatBestPlayer %d,'%02d'", m_nGuildCombatIndex - 1, g_appInfo.dwSys);
		if (FALSE == qry.Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);	return FALSE;
		}
		if (qry.Fetch())
		{
			m_uBestPlayer = qry.GetInt("PlayerID");
		}


		sprintf(szQuery, "uspLoadCombatGuildList %d, '%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
		if (FALSE == qry.Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
		}

		while (qry.Fetch())
		{
			u_long GuildId = (u_long)(qry.GetInt("GuildID"));
			DWORD dwPenya = (DWORD)(qry.GetInt64("CombatFee"));
			int nStatus = qry.GetInt("Status");
			__GUILDCOMBATJOIN GCJoin;
			GCJoin.dwPenya = dwPenya;
			if (nStatus == 10 || nStatus == 12 || nStatus == 20)
				GCJoin.bRequest = TRUE;
			else
				GCJoin.bRequest = FALSE;
			GCJoin.uGuildId = GuildId;
			m_vecGuildCombat.push_back(GCJoin);
		}

		m_vecGCPlayerPoint.clear();
		sprintf(szQuery, "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys);
		if (FALSE == qry.Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery); return FALSE;
		}
		while (qry.Fetch())
		{
			__GCPLAYERPOINT GCPlayerPoint;
			GCPlayerPoint.uidPlayer = qry.GetInt("PlayerID");
			GCPlayerPoint.nJob = qry.GetInt("Job");
			GCPlayerPoint.nPoint = qry.GetInt("PointSummary");
			m_vecGCPlayerPoint.push_back(GCPlayerPoint);
		}
	}
	else
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
	}
	return TRUE;
}

BOOL	CDbManager::LoadPlayerData(void)
{
	CQuery	query;
	if (query.Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		Error("LoadPlayerData: can't connect character db");
		AfxMessageBox("can't connect character db");
	}
	char szQuery[100] = { 0, };

	sprintf(szQuery, "uspLoadPlayerData '%02d'", g_appInfo.dwSys);
	if (!query.Exec(szQuery))
	{
		Error("LoadPlayerData - query: %s", szQuery);
		return FALSE;
	}

	PlayerData	pd;
	u_long idPlayer = 0;
	while (query.Fetch())
	{
		idPlayer = query.GetInt("m_idPlayer");
		query.GetStr("m_szName", pd.szPlayer);
		pd.data.nSex = (int)query.GetInt("m_dwSex");
		pd.data.nJob = (int)query.GetInt("m_nJob");
		pd.data.nLevel = (int)query.GetInt("m_nLevel");
		pd.data.nVer = 1;
		pd.data.uLogin = 0;
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->AddPlayerData(idPlayer, pd);
		ASSERT(pPlayerData);
	}
	m_idPlayer = idPlayer;
	return TRUE;
}

BOOL CDbManager::GetPartyName(void)
{
	CQuery qry;

	if (qry.Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		return FALSE;
	}

	char szQuery[256], szPartyName[36];
	u_long id = 0;


	sprintf(szQuery,
		"PARTY_STR 'S2','','%02d'",
		g_appInfo.dwSys);

	if (FALSE == qry.Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return FALSE;
	}

	while (qry.Fetch())
	{
		qry.GetStr("partyname", szPartyName);
		id = qry.GetInt("m_idPlayer");
		m_2PartyNamePtr.insert(map<u_long, string>::value_type(id, szPartyName));
	}
	m_idPlayer = id;
	return TRUE;
}

BOOL CDbManager::GetMyPartyName(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, u_long uidPlayer, char* szPartyName)
{
	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"PARTY_STR 'S1','%07d','%02d'",
		uidPlayer, g_appInfo.dwSys);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return FALSE;
	}

	if (qry->Fetch())
		qry->GetStr("partyname", szPartyName);

	return TRUE;
}

void CDbManager::LogPlayConnect(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	int		iServer_No;
	DWORD	iMap_No;
	char	cIP[16] = { 0, };
	DWORD	dConnectTime;
	DWORD	dSeed;
	SYSTEMTIME	siIn;
	u_long	uidPlayer;
	char szAccount[MAX_ACCOUNT];
	int nLevel;
	int nJob;

	iServer_No = g_appInfo.dwSys;
	arRead >> iMap_No;
	arRead.Read(&siIn, sizeof(SYSTEMTIME));
	arRead.ReadString(cIP, 16);
	arRead >> dSeed;
	arRead >> uidPlayer;
	arRead.ReadString(szAccount, MAX_ACCOUNT);
	arRead >> nLevel;
	arRead >> nJob;

	CTime tIn(siIn), tOut = CTime::GetCurrentTime();
	CTimeSpan tConnect = tOut - tIn;

	CString strOut = tOut.Format("%Y%m%d%H%M%S");

	CString strIn = tIn.Format("%Y%m%d%H%M%S");
	dConnectTime = (DWORD)tConnect.GetTotalSeconds();
	char timebuf[15] = { 0, };
	strncpy(timebuf, (LPCTSTR)strIn, 14);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"uspLoggingLogin '%07d', '%02d', '%s', %d, %d, %d, %d, '%s', %d, '%s'"
		, uidPlayer, g_appInfo.dwSys, szAccount, iMap_No, dSeed, nLevel, nJob, timebuf, dConnectTime, cIP
	);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%d, %s", __FILE__, __LINE__, uidPlayer, szQuery);
		return;
	}
}

void CDbManager::AddPartyName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long uidPlayer;
	TCHAR	sParty[33];
	arRead >> uidPlayer;
	arRead.ReadString(sParty, 33);

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidGameName(sParty))
	{
		CString str;
		str.Format("CDbManager::AddPartyName() err: %s - partyname", sParty);
		Error(str);
		return;
	}
#endif

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery,
		"PARTY_STR 'A1','%07d','%02d','%s'",
		uidPlayer, g_appInfo.dwSys, sParty);

	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);

		return;
	}

	ULONG2STRING::iterator iter = m_2PartyNamePtr.find(uidPlayer);
	if (iter != m_2PartyNamePtr.end())
		m_2PartyNamePtr.erase(iter);

	m_2PartyNamePtr.insert(map<u_long, string>::value_type(uidPlayer, sParty));
}

BOOL CDbManager::SendItemtoCharacter(int nSlot, CMover* pMover, CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "S4", pMover->m_idPlayer, g_appInfo.dwSys);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return FALSE;
	}

	BOOL bResult = FALSE;
	while (qry->Fetch())
	{
		__SendItemContents SendItemContents;
		CItemElem* pItemElem = &SendItemContents.itemElem;
		pItemElem->SetRandomOptItemId(0);

		if (GetSendItem(qry, &SendItemContents) == FALSE)
		{
			WriteLog("SendItem::GetSendItem PlayerName = %s, Item_Count = %d Qry = %s", pMover->m_szName, SendItemContents.Item_Count, szQuery);
			continue;
		}

		if (0 == strcmp(SendItemContents.OneItem, "penya"))
		{
			if (SendPenya(qry1, szQuery, pMover, SendItemContents.Item_Count, SendItemContents.nNo) == FALSE)
			{
				WriteLog("SendItem::SendPenya PlayerName = %s, Item_Count = %d Qry = %s", pMover->m_szName, SendItemContents.Item_Count, szQuery);
				continue;
			}

			bResult = TRUE;

			LogItemInfo aLogItem;
			aLogItem.SendName = pMover->m_szName;
			aLogItem.RecvName = "DB";
			aLogItem.Gold = pMover->GetGold() - SendItemContents.Item_Count;
			aLogItem.Gold2 = pMover->GetGold();
			//aLogItem.ItemName = "SEED";
			_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
			aLogItem.itemNumber = SendItemContents.Item_Count;
			aLogItem.Action = "U";
			if (ItemLogQuery(qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem) == FALSE)
				continue;
		}
		else
		{
			SERIALNUMBER iSerialNumber = SendItem(qry1, szQuery, pMover, SendItemContents);
			if (iSerialNumber != 0)
			{
				bResult = TRUE;
				LogItemInfo aLogItem;
				aLogItem.SendName = pMover->m_szName;
				aLogItem.RecvName = "DB";
				aLogItem.Gold = pMover->GetGold();
				aLogItem.Gold2 = pMover->GetGold();
				_stprintf(aLogItem.szItemName, "%d", SendItemContents.itemElem.m_dwItemId);
				aLogItem.itemNumber = SendItemContents.Item_Count;
				aLogItem.ItemNo = iSerialNumber;
				aLogItem.nAbilityOption = pItemElem->GetOption();
				aLogItem.nItemResist = pItemElem->m_bItemResist;
				aLogItem.nResistAbilityOption = pItemElem->m_nResistAbilityOption;
				aLogItem.Action = "U";
				if (ItemLogQuery(qrylog, "LC", aLogItem, pMover->m_idPlayer, SendItemContents.nNo, SendItemContents.OneItem) == FALSE)
					continue;
			}
			else
				continue;
		}
	}

	return bResult;
}

BOOL CDbManager::GetSendItem(CQuery* pQry, __SendItemContents* pSendItemContents)
{
	pSendItemContents->nNo = pQry->GetInt("m_nNo");
	pQry->GetStr("Item_Name", pSendItemContents->OneItem);

	pSendItemContents->Item_Count = pQry->GetInt("Item_count");
	int nIndex = atoi(pSendItemContents->OneItem);
	ItemProp* pItemProp = NULL;
	if (nIndex > 0)
		pItemProp = prj.GetItemProp(nIndex);
	else
		pItemProp = prj.GetItemProp(pSendItemContents->OneItem);

	if (!pItemProp)
	{
		if (strcmp(pSendItemContents->OneItem, "penya") == 0)
			return TRUE;

		return FALSE;
	}

	CItemElem* pItemElem = &pSendItemContents->itemElem;
	pItemElem->m_dwItemId = pItemProp->dwID;
	pItemElem->m_nItemNum = pSendItemContents->Item_Count > 0 ? pSendItemContents->Item_Count : 1;
	pItemElem->SetOption(pQry->GetInt("m_nAbilityOption") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt("m_nAbilityOption"));

	DWORD dwItemResist = pQry->GetInt("m_bItemResist") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt("m_bItemResist");
	ItemProp* pVisPetProp = prj.GetItemProp((int)dwItemResist);
	if (pVisPetProp != NULL && pVisPetProp->dwItemKind3 == IK3_PET)
	{
		if (dwItemResist == pItemElem->m_dwItemId)
		{
			pItemElem->m_bTranformVisPet = TRUE;
			pItemElem->m_bItemResist = SAI79::NO_PROP;
		}
		else
		{
			Error("CDbManager::GetSendItem - TransformVisPet ItemId is Different. ItemId : [%d], ItemResist : [%d]", pItemElem->m_dwItemId, dwItemResist);
			pItemElem->m_bItemResist = SAI79::NO_PROP;
		}
	}
	else
		pItemElem->m_bItemResist = (BYTE)dwItemResist;

	pItemElem->m_nResistAbilityOption = pQry->GetInt("m_nResistAbilityOption") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt("m_nResistAbilityOption");

	pSendItemContents->m_dwKeepTime = pQry->GetInt64("m_dwKeepTime") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt64("m_dwKeepTime");

	pItemElem->SetRandomOptItemId(pQry->GetInt64("nRandomOptItemId") == CQuery::CQUERYNULL ? 0 : pQry->GetInt64("nRandomOptItemId"));

	pItemElem->SetPiercingSize(pQry->GetInt("nPiercedSize") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt("nPiercedSize"));
	for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
	{
		char szItem[32] = { 0, };
		sprintf(szItem, "adwItemId%d", i);
		pItemElem->SetPiercingItem(i, pQry->GetInt(szItem) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt(szItem));
	}
	pItemElem->SetUltimatePiercingSize(pQry->GetInt("nUMPiercedSize") <= CQuery::CQUERYNULL ? 0 : pQry->GetInt("nUMPiercedSize"));
	for (int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++)
	{
		char szItem[32] = { 0, };
		sprintf(szItem, "adwUMItemId%d", i);
		pItemElem->SetUltimatePiercingItem(i, pQry->GetInt(szItem) <= CQuery::CQUERYNULL ? 0 : pQry->GetInt(szItem));
	}
	return TRUE;
}

BOOL CDbManager::SendPenya(CQuery* pQry, char* szSql, CMover* pMover, int nPenya, int nNo)
{
	if (SendItemDeleteQuery(pQry, szSql, nNo) == FALSE)
		return FALSE;

	pMover->AddGold(nPenya, FALSE);
	return TRUE;
}

SERIALNUMBER CDbManager::SendItem(CQuery* pQry, char* szSql, CMover* pMover, __SendItemContents& SendItemContents)
{
	SERIALNUMBER iSerialNumber = 0;
	CItemElem* pItemElem = &SendItemContents.itemElem;

	iSerialNumber = CSerialNumber::GetInstance()->Get();


	DWORD dwKeepTime = 0;
	// if( pItemElem->m_dwKeepTime != 0 )
	if (SendItemContents.m_dwKeepTime != 0)
	{
		DWORD dwTemp;
		int nYear, nMonth, nDay, nHour, nMin;
		//		nYear = pItemElem->m_dwKeepTime / 100000000;	dwTemp = pItemElem->m_dwKeepTime % 100000000;
		nYear = (int)(SendItemContents.m_dwKeepTime / 100000000);	dwTemp = (DWORD)(SendItemContents.m_dwKeepTime % 100000000);
		nMonth = dwTemp / 1000000;		dwTemp = dwTemp % 1000000;
		nDay = dwTemp / 10000;			dwTemp = dwTemp % 10000;
		nHour = dwTemp / 100;			dwTemp = dwTemp % 100;
		nMin = dwTemp;

		CTime tKeepTime(nYear, nMonth, nDay, nHour, nMin, 0);
		dwKeepTime = (DWORD)(tKeepTime.GetTime());
		if ((time_t)dwKeepTime <= time(NULL))
		{
			FILEOUT("..\\SendItem.log", "Error dwKeepTime Over IdPlayer = %d, PlayerName = %s, ItemName : %s, KeepTime = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwKeepTime);
			SendItemDeleteQuery(pQry, szSql, SendItemContents.nNo);
			return 0;
		}
		pItemElem->m_dwKeepTime = dwKeepTime;
	}

	if (pItemElem->IsPiercedItem())
	{
		for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
		{
			if (pItemElem->GetPiercingItem(i) != 0)
			{
				ItemProp* pItemProp = prj.GetItemProp(pItemElem->GetPiercingItem(i));
				if (!pItemProp || !pItemElem->IsPierceAble(pItemProp->dwItemKind3))
				{
					FILEOUT("..\\SendItem.log", "Error Not IK3_SOCKETCARD or IK3_SOCKETCARD2 Piercing Over IdPlayer = %d, PlayerName = %s, ItemName : %s, PiercingItemId = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId);
					SendItemDeleteQuery(pQry, szSql, SendItemContents.nNo);
					return 0;
				}
			}
		}

		for (int i = 0; i < pItemElem->GetUltimatePiercingSize(); i++)
		{
			if (pItemElem->GetUltimatePiercingItem(i) != 0)
			{
				ItemProp* pItemProp = prj.GetItemProp(pItemElem->GetUltimatePiercingItem(i));
				if (!pItemProp || pItemProp->dwItemKind3 != IK3_ULTIMATE)
				{
					FILEOUT("..\\SendItem.log", "Error Not IK3_ULTIMATE Piercing Over IdPlayer = %d, PlayerName = %s, ItemName : %s, PiercingItemId = %d", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem, pItemElem->m_dwItemId);
					SendItemDeleteQuery(pQry, szSql, SendItemContents.nNo);
					return 0;
				}
			}
		}
	}

	if (!pMover->m_Inventory.Add(pItemElem))
	{
		FILEOUT("..\\SendItem.log", "Error Full IdPlayer = %d, PlayerName = %s, ItemName : %s", pMover->m_idPlayer, pMover->m_szName, SendItemContents.OneItem);
		return 0;
	}

	if (SendItemDeleteQuery(pQry, szSql, SendItemContents.nNo) == FALSE)
		return 0;

	pItemElem->SetSerialNumber(iSerialNumber);
	return pItemElem->GetSerialNumber();
}


BOOL CDbManager::SendItemDeleteQuery(CQuery* pQry, char* szSql, int nNo)
{
	DBQryCharacter(szSql, "S5", 0, 0, "", "", nNo);

	if (FALSE == pQry->Exec(szSql))
	{
		WriteLog("SendItemDeleteQuery :: nNo = %d", nNo);
		return FALSE;
	}

	if (pQry->Fetch())
	{
		int nError = pQry->GetInt("fError");
		if (nError == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbManager::ItemLogQuery(CQuery* pQryLog, char* szQueryState, const LogItemInfo& info, u_long uIdPlayer, int nNo, char* szItemName)
{
	char szQuerylog[4096] = { 0, };

	DBQryLog(szQuerylog, szQueryState, uIdPlayer, g_appInfo.dwSys, nNo, 0, 0, 0, 0, info.nItemResist, info.nResistAbilityOption, 0, 0, 0, 0, 'A', 0, '\0',
		0.0f, 0.0f, 0.0f, 0, 0, "\0", 0, 0, 0, szItemName, 0, info.itemNumber);

	if (FALSE == pQryLog->Exec(szQuerylog))
	{
		WriteLog("%s, %d\tSendItemLogQuery LC = %s", __FILE__, __LINE__, szQuerylog);
		return FALSE;
	}

	DBQryNewItemLog(szQuerylog, info);

	if (FALSE == pQryLog->Exec(szQuerylog))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuerylog);
		return FALSE;
	}
	return TRUE;
}

BOOL CDbManager::RemoveItemtoCharacter(CMover* pMover, CQuery* qry, CQuery* qry1, CQuery* qrylog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	BOOL	bResult = FALSE;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "S6", pMover->m_idPlayer, g_appInfo.dwSys);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return FALSE;
	}

	while (qry->Fetch())
	{
		char chState;
		char OneItem[MAX_PATH];
		int nNo, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption;
		SERIALNUMBER iSerialNumber;
		BOOL bIsPenya = FALSE;

		if (GetRemoveItem(qry, nNo, OneItem, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption, chState) == FALSE)
			continue;

		if (0 == strcmp(OneItem, "penya"))
		{
			if (RemovePenya(qry1, szQuery, pMover, Item_Count, nNo, chState) == FALSE)
			{
				WriteLog("RemoveItem::RemovePenya PlayerName = %s, Item_Count = %d, State = %c, Qry1 = %s", pMover->m_szName, Item_Count, chState, szQuery);
				continue;
			}
			bIsPenya = TRUE;
		}
		else
		{
			iSerialNumber = RemoveItem(qry1, szQuery, nNo, pMover, OneItem, Item_Count, m_nAbilityOption, nItemResist, nResistAbilityOption, chState);
		}

		if (bIsPenya)
		{
			//sprintf( OneItem, "SEED" );
			_stprintf(OneItem, "%d", II_GOLD_SEED1);
			iSerialNumber = 0;
		}
		else
		{
			if (iSerialNumber == 0)
				continue;
		}

		bResult = TRUE;

		LogItemInfo aLogItem;

		if (chState == 'I')
			aLogItem.Action = "I";
		else
			aLogItem.Action = "K";

		aLogItem.SendName = pMover->m_szName;
		aLogItem.RecvName = "DB";
		char szItemId[32] = { 0, };
		if (lstrlen(OneItem) > 0)
		{
			ItemProp* pItemProp = NULL;
			int nIndex = atoi(OneItem);
			if (nIndex > 0)
			{
				pItemProp = prj.GetItemProp(nIndex);
			}
			else
			{
				pItemProp = prj.GetItemProp(OneItem);
			}
			if (pItemProp != NULL)
			{
				_ultoa(pItemProp->dwID, szItemId, 10);
			}
			else
			{
				_stprintf(szItemId, "%d", -1);
			}
		}
		_tcscpy(aLogItem.szItemName, szItemId);
		// mirchang_100317 item name -> item Id
		aLogItem.Gold2 = pMover->GetGold();
		aLogItem.itemNumber = Item_Count;
		aLogItem.nItemResist = nItemResist;
		aLogItem.nResistAbilityOption = nResistAbilityOption;

		if (bIsPenya)
		{
			aLogItem.Gold = pMover->GetGold() + Item_Count;
		}
		else
		{
			aLogItem.Gold = pMover->GetGold();
			aLogItem.ItemNo = iSerialNumber;
			aLogItem.nAbilityOption = m_nAbilityOption;
		}
		if (ItemLogQuery(qrylog, "LD", aLogItem, pMover->m_idPlayer, nNo, OneItem) == FALSE)
			continue;
	}
	return bResult;
}

BOOL CDbManager::RemoveItemDeleteQuery(CQuery* pQry, char* szSql, int nNo)
{
	DBQryCharacter(szSql, "S7", 0, 0, "", "", nNo);

	if (FALSE == pQry->Exec(szSql))
	{
		WriteLog("RemoveItemDeleteQuery :: nNo = %d", nNo);
		return FALSE;
	}

	if (pQry->Fetch())
	{
		int nError = pQry->GetInt("fError");
		if (nError == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

HANDLE s_hHandle = (HANDLE)NULL;

BOOL CDbManager::CreateDbWorkers(void)
{
	s_hHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwThreadId;

	m_hIOCPGet = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);	// select
	ASSERT(m_hIOCPGet);
	for (int i = 0; i < MAX_GETTHREAD_SIZE; i++)
	{
		m_hThreadGet[i] = chBEGINTHREADEX(NULL, 0, _GetThread, (LPVOID)this, 0, &dwThreadId);
		ASSERT(m_hThreadGet[i]);
		SetThreadPriority(m_hThreadGet[i], THREAD_PRIORITY_BELOW_NORMAL);
		//		Sleep( 1000 );
		if (WaitForSingleObject(s_hHandle, SEC(3)) == WAIT_TIMEOUT)
			OutputDebugString("DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n");
	}
	m_hIOCPPut = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);	// insert log
	ASSERT(m_hIOCPPut);
	for (int i = 0; i < MAX_PUTTHREAD_SIZE; i++)
	{
		m_hThreadPut[i] = chBEGINTHREADEX(NULL, 0, _PutThread, (LPVOID)this, 0, &dwThreadId);
		ASSERT(m_hThreadPut[i]);
		SetThreadPriority(m_hThreadPut[i], THREAD_PRIORITY_BELOW_NORMAL);
		//		Sleep( 1000 );
		if (WaitForSingleObject(s_hHandle, SEC(3)) == WAIT_TIMEOUT)
			OutputDebugString("DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n");
	}

	m_hIOCPGuild = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	m_hThreadGuild = chBEGINTHREADEX(NULL, 0, _GuildThread, (LPVOID)this, 0, &dwThreadId);

	if (WaitForSingleObject(s_hHandle, SEC(3)) == WAIT_TIMEOUT)
		OutputDebugString("DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n");

	m_hCloseSPThread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSPThread = chBEGINTHREADEX(NULL, 0, _SPThread, (LPVOID)this, 0, &dwThreadId);	// update
	ASSERT(m_hSPThread);

#ifdef __S1108_BACK_END_SYSTEM
	m_hCloseWorker = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hWorker = chBEGINTHREADEX(NULL, 0, _BackSystem, this, 0, &dwThreadId);
	ASSERT(m_hWorker);
#endif // __S1108_BACK_END_SYSTEM

	ASSERT(m_hWorker);

	for (int i = 0; i < MAX_QUERY_RESERVED; i++)
	{
		m_apQuery[i] = new CQuery;
		if (FALSE == m_apQuery[i]->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
		{
			for (i = 0; i < MAX_QUERY_RESERVED; i++)
				SAFE_DELETE(m_apQuery[i]);
			OutputDebugString("MAX_QUERY_RESERVED");
			return FALSE;
		}
	}
	return TRUE;
}

void CDbManager::CloseDbWorkers(void)
{
	if (NULL != m_hIOCPGet)
	{
		for (int i = 0; i < MAX_GETTHREAD_SIZE; i++)
			PostQueuedCompletionStatus(m_hIOCPGet, 0, NULL, NULL);
		WaitForMultipleObjects(MAX_GETTHREAD_SIZE, m_hThreadGet, TRUE, INFINITE);
		CLOSE_HANDLE(m_hIOCPGet);
		for (int i = 0; i < MAX_GETTHREAD_SIZE; i++)
			CLOSE_HANDLE(m_hThreadGet[i]);
	}
	if (NULL != m_hIOCPPut)
	{
		for (int i = 0; i < MAX_PUTTHREAD_SIZE; i++)
			PostQueuedCompletionStatus(m_hIOCPPut, 0, NULL, NULL);
		WaitForMultipleObjects(MAX_PUTTHREAD_SIZE, m_hThreadPut, TRUE, INFINITE);
		CLOSE_HANDLE(m_hIOCPPut);
		for (int i = 0; i < MAX_PUTTHREAD_SIZE; i++)
			CLOSE_HANDLE(m_hThreadPut[i]);
	}

	if (m_hIOCPGuild)
	{
		PostQueuedCompletionStatus(m_hIOCPGuild, 0, NULL, NULL);
		WaitForSingleObject(m_hThreadGuild, INFINITE);
		CLOSE_HANDLE(m_hIOCPGuild);
		CLOSE_HANDLE(m_hThreadGuild);
	}

#ifdef __S1108_BACK_END_SYSTEM
	CLOSE_THREAD(m_hWorker, m_hCloseWorker);
#endif // __S1108_BACK_END_SYSTEM

	CLOSE_THREAD(m_hSPThread, m_hCloseSPThread);

	for (int i = 0; i < MAX_QUERY_RESERVED; i++)
		SAFE_DELETE(m_apQuery[i]);

	CLOSE_HANDLE(s_hHandle);
}

UINT CDbManager::_GetThread(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->GetThread();
	return 0;
}

UINT CDbManager::_PutThread(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->PutThread();
	return 0;
}

UINT CDbManager::_SPThread(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->SPThread();
	return 0;
}

UINT CDbManager::_UpdateThread(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->UpdateThread();
	return 0;
}

UINT CDbManager::_GuildThread(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->GuildThread();
	return 0;
}

#ifdef __S1108_BACK_END_SYSTEM
UINT CDbManager::_BackSystem(LPVOID pParam)
{
	CDbManager* pDbManager = (CDbManager*)pParam;
	pDbManager->BackSystem();
	return 0;
}
#endif // __S1108_BACK_END_SYSTEM

void CDbManager::GetThread(void)
{
	CQuery* pQuery = new CQuery;
	if (FALSE == pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d CharacterDB Connect", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
		return;
	}

	CQuery* pQuery1 = new CQuery;
	if (FALSE == pQuery1->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d CharacterDB Connect", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
		SAFE_DELETE(pQuery1);
		return;
	}

	CQuery* pQueryLog = new CQuery;
	if (FALSE == pQueryLog->Connect(3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG))
	{
		WriteLog("%s, %d LogDB Connect", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
		SAFE_DELETE(pQuery1);
		SAFE_DELETE(pQueryLog);
		return;
	}

	SetEvent(s_hHandle);

	BOOL bReturnValue = FALSE;
	DWORD dwBytesTransferred = 0;
	DWORD dwCompletionKey = 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL;

	while (true)
	{
		bReturnValue = GetQueuedCompletionStatus(m_hIOCPGet, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE);
		if (FALSE == bReturnValue)
		{
			Error("CDbManager::GetThread (1). Error:[%d]", ::GetLastError());
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			continue;
		}
		if (dwBytesTransferred == 0)
		{
			SAFE_DELETE(pQuery);
			SAFE_DELETE(pQuery1);
			SAFE_DELETE(pQueryLog);
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			return;
		}

		switch (lpDbOverlappedPlus->nQueryMode)
		{
		case SENDPLAYERLIST:
			SendPlayerList(pQuery, lpDbOverlappedPlus);
			break;
		case JOIN:
			Join(pQuery, pQuery1, pQueryLog, lpDbOverlappedPlus);
			break;
		case CREATEPLAYER:
			CreatePlayer(pQuery, lpDbOverlappedPlus);
			break;
		case REMOVEPLAYER:
			RemovePlayer(pQuery, lpDbOverlappedPlus);
			break;
		default:
		{
			Error("CDbManager::GetThread nQueryMode Invalid : [%d]", lpDbOverlappedPlus->nQueryMode);
			break;
		}
		}

		SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
	}
}

void CDbManager::PutThread(void)
{
	CQuery* pQueryChar = new CQuery;
	if (FALSE == pQueryChar->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		SAFE_DELETE(pQueryChar);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	CQuery* pQuery1 = new CQuery;
	if (FALSE == pQuery1->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		SAFE_DELETE(pQueryChar);
		SAFE_DELETE(pQuery1);
		return;
	}
#endif

	CQuery* pQueryLog = new CQuery;
	if (FALSE == pQueryLog->Connect(3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG))
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		SAFE_DELETE(pQueryChar);
#ifdef __FL_PACKED_ITEMS
		SAFE_DELETE(pQuery1);
#endif
		SAFE_DELETE(pQueryLog);
		return;
	}

	SetEvent(s_hHandle);

	BOOL bReturnValue = FALSE;
	DWORD dwBytesTransferred = 0;
	DWORD dwCompletionKey = 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL;

	while (true)
	{
		bReturnValue = GetQueuedCompletionStatus(m_hIOCPPut, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE);
		if (FALSE == bReturnValue)
		{
			Error("CDbManager::PutThread (1). Error:[%d]", ::GetLastError());
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			continue;
		}
		if (dwBytesTransferred == 0)
		{
			SAFE_DELETE(pQueryChar);
#ifdef __FL_PACKED_ITEMS
			SAFE_DELETE(pQuery1);
#endif
			SAFE_DELETE(pQueryLog);
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			return;
		}

		switch (lpDbOverlappedPlus->nQueryMode)
		{
		case SAVECONCURRENTUSERNUMBER:
			LogConcurrentUserNumber(pQueryLog, lpDbOverlappedPlus);
			break;
		case LOG_PLAY_CONNECT:
			LogPlayConnect(pQueryLog, lpDbOverlappedPlus);
			break;
		case LOG_ITEM:
			LogItem(pQueryLog, lpDbOverlappedPlus);
			break;
		case LOG_LEVELUP:
			LogLevelUp(pQueryLog, lpDbOverlappedPlus);
			break;
		case LOG_UNIQUEITEM:
			LogUniqueItem(pQueryLog, lpDbOverlappedPlus);
			break;
		case LOG_QUEST:
			LogQuest(pQueryLog, lpDbOverlappedPlus);
			break;
		case QM_SET_PLAYER_NAME:
			SetPlayerName(pQueryChar, pQueryLog, lpDbOverlappedPlus);
			break;
		case SAVE_PLAY_TIME:
			SavePlayTime(pQueryChar, lpDbOverlappedPlus);
			break;
		case QM_ADD_MESSENGER:
			AddMessenger(pQueryChar, lpDbOverlappedPlus);
			break;
		case QM_DELETE_MESSENGER:
			DeleteMessenger(pQueryChar, lpDbOverlappedPlus);
			break;
		case QM_UPDATE_MESSENGER:
			UpdateMessenger(pQueryChar, lpDbOverlappedPlus);
			break;
		case ADD_PARTYNAME:
			AddPartyName(pQueryChar, lpDbOverlappedPlus);
			break;
		case CHANGE_BANKPASS:
			ChangeBankPass(pQueryChar, lpDbOverlappedPlus);
			break;
		case QM_DELETE_REMOVE_GUILD_BANK_TBL:
			DeleteRemoveGuildBankTbl(pQueryChar, lpDbOverlappedPlus);
			break;
		case QM_CALL_USPLOGGINGQUEST:
			call_uspLoggingQuest(pQueryLog, lpDbOverlappedPlus);
			break;
		case QM_CALL_XXX_MULTI_SERVER:
			call_uspXXXMultiServer(pQueryChar, lpDbOverlappedPlus);
			break;
		case LOG_GUILDFURNITURE:
			LogGuildFurniture(pQueryChar, lpDbOverlappedPlus);
			break;
#ifdef __FL_GW_STATUE_V2
		case GW_STATUES_INSTALL:
			InstallStatues(pQueryChar, lpDbOverlappedPlus);
			break;
#endif
#ifdef __FL_EQUALIZED_MODE
		case TMP_EQUIP_SAVE:
			SaveTemporalEquipment(pQueryChar, lpDbOverlappedPlus);
			break;
		case TMP_EQUIP_GET:
			LoadTemporalEquipment(pQueryChar, lpDbOverlappedPlus);
			break;
#endif
#ifdef __FL_DAILY_REWARDS
		case UPDATE_DAILYGIFT:
			UpdateDailyGift(pQueryChar, lpDbOverlappedPlus);
			break;
#endif
#ifdef __FL_VENDORS
		case ADD_VENDOR:
			AddVendor(pQueryChar, lpDbOverlappedPlus);
			break;
		case UPDATE_VENDOR:
			UpdateVendor(pQueryChar, lpDbOverlappedPlus);
			break;
		case DELETE_VENDOR:
			DeleteVendor(pQueryChar, lpDbOverlappedPlus);
			break;
		case GET_VENDORS:
#ifndef __FL_PACKED_ITEMS
			GetVendors(pQueryChar, lpDbOverlappedPlus);
#else
			GetVendors(pQueryChar, lpDbOverlappedPlus, pQuery1);
#endif
			break;
#endif
		default:
			Error("CDbManager::PutThread nQueryMode Invalid : [%d]", lpDbOverlappedPlus->nQueryMode);
			break;
		} // end of switch

		SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);

	} // end of while
}

void CDbManager::UpdateThread(void)
{
	CQuery* pQuery = new CQuery;
	if (FALSE == pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		Error("CharacterDB Connect Failed");
		SAFE_DELETE(pQuery);
		return;
	}

	CQuery* pQueryLog = new CQuery;
	if (FALSE == pQueryLog->Connect(3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG))
	{
		Error("LogDB Connect Failed");
		SAFE_DELETE(pQuery);
		SAFE_DELETE(pQueryLog);
		return;
	}

	SetEvent(s_hHandle);

	BOOL bReturnValue = FALSE;
	DWORD dwBytesTransferred = 0;
	DWORD dwCompletionKey = 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL;
	char* szQuery = new char[40960];
	CMover* pMover = NULL;

	while (true)
	{
		bReturnValue = GetQueuedCompletionStatus(m_hIOCPUpdate, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE);
		if (FALSE == bReturnValue)
		{
			Error("CDbManager::UpdateThread (1). Error:[%d]", ::GetLastError());
			continue;
		}

		if (dwBytesTransferred == 0)
		{
			SAFE_DELETE(pQuery);
			SAFE_DELETE(pQueryLog);
			SAFE_DELETE_ARRAY(szQuery);
			return;
		}

		pMover = (CMover*)dwBytesTransferred;
		SavePlayer(pQuery, pQueryLog, pMover, szQuery);

#ifdef __INVALID_LOGIN_0612
		if (pMover->m_bLogout)
			g_DbManager.m_AccountCacheMgr.ChangeMultiServer(pMover->m_szAccount, 0);
#endif	// __INVALID_LOGIN_0612

		SAFE_DELETE(pMover);
	}
}

void CDbManager::SPThread(void)
{
	DWORD dwThreadId;
	m_hIOCPUpdate = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);	// insert log
	ASSERT(m_hIOCPUpdate);
	for (int i = 0; i < MAX_UPDATETHREAD_SIZE; i++)
	{
		m_hThreadUpdate[i] = chBEGINTHREADEX(NULL, 0, _UpdateThread, (LPVOID)this, 0, &dwThreadId);
		ASSERT(m_hThreadUpdate[i]);
		SetThreadPriority(m_hThreadUpdate[i], THREAD_PRIORITY_BELOW_NORMAL);
		if (WaitForSingleObject(s_hHandle, SEC(3)) == WAIT_TIMEOUT)
			OutputDebugString("DATABASESERVER.EXE\t// TIMEOUT\t// ODBC\n");
	}

	WaitForSingleObject(m_hCloseSPThread, INFINITE);

	for (int i = 0; i < MAX_UPDATETHREAD_SIZE; i++)
		PostQueuedCompletionStatus(m_hIOCPUpdate, 0, NULL, NULL);
	WaitForMultipleObjects(MAX_UPDATETHREAD_SIZE, m_hThreadUpdate, TRUE, INFINITE);
	CLOSE_HANDLE(m_hIOCPUpdate);
	for (int i = 0; i < MAX_UPDATETHREAD_SIZE; i++)
		CLOSE_HANDLE(m_hThreadUpdate[i]);
}

void CDbManager::CreateGuild(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	//	u_long idPlayer, idGuild;
	int nSize;
	GUILD_MEMBER_INFO info[MAX_PTMEMBER_SIZE];
	u_long idGuild;
	char szGuild[MAX_G_NAME];
	//	ar >> idPlayer >> idGuild;
	ar >> nSize;
	ar.Read(info, sizeof(GUILD_MEMBER_INFO) * nSize);
	ar >> idGuild;
	ar.ReadString(szGuild, MAX_G_NAME);

	GUILD_QUERYINFO qi1("A1");
	qi1.idGuild = idGuild;
	qi1.pszGuild = szGuild;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi1);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}
	if (pQuery->Fetch())
	{
		int nError = pQuery->GetInt("nError");
		if (nError != 1)
		{

			return;
		}
	}
	for (int i = 0; i < nSize; i++)
	{
		if (info[i].idPlayer != 0)
		{
			GUILD_QUERYINFO qi2("A2");
			//			qi2.idPlayer	= idPlayer;
			qi2.idPlayer = info[i].idPlayer;
			qi2.idGuild = idGuild;
			qi2.nLevel = (i == 0 ? GUD_MASTER : GUD_ROOKIE);
			DBQryGuild(szQuery, qi2);
			if (FALSE == pQuery->Exec(szQuery))
			{
				//				WriteLog( "CreateGuild(): A2" );
			}
		}
	}
	GUILDLOG_QUERYINFO qi("L3");
	qi.idGuild = idGuild;
	qi.idPlayer = info[0].idPlayer;
	qi.pszState = "S";
	qi.pszGuildBank = szGuild;

	DBQryGuildLog(szQuery, qi);
	if (FALSE == pQueryLog->Exec(szQuery))
	{

		return;
	}
	GUILDLOG_QUERYINFO LogQi("L1");
	LogQi.idGuild = idGuild;
	LogQi.idDoPlayer = info[0].idPlayer;
	LogQi.pszState = "G";

	for (int i = 0; i < nSize; i++)
	{
		if (info[i].idPlayer != 0)
		{
			LogQi.idPlayer = info[i].idPlayer;
			DBQryGuildLog(szQuery, LogQi);
			if (FALSE == pQueryLog->Exec(szQuery))
			{

				return;
			}
		}
	}


}

void CDbManager::DestroyGuild(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idGuild, idMaster;
	ar >> idGuild;
	ar >> idMaster;

	GUILD_QUERYINFO info("D1");
	info.idGuild = idGuild;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);

	if (FALSE == pQuery->Exec(szQuery))
		return;

#ifdef __FL_PACKED_ITEMS
	SavePackages(pQuery, nullptr, idGuild, PRT_GUILDBANK);
#endif

	GuildHouseDBMng->PostRequest(GUILDHOUSE_REMOVE, NULL, 0, idGuild);
	GUILDLOG_QUERYINFO qi("L3");
	qi.idGuild = idGuild;
	qi.idPlayer = idMaster;
	qi.pszState = "E";

	DBQryGuildLog(szQuery, qi);
	if (FALSE == pQueryLog->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::AddGuildMember(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;
	GUILD_QUERYINFO qi("A2");
	qi.idPlayer = idPlayer;
	qi.idGuild = idGuild;
	qi.nLevel = GUD_ROOKIE;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}
	GUILDLOG_QUERYINFO LogQi("L1");
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	LogQi.pszState = "J";

	DBQryGuildLog(szQuery, LogQi);
	if (FALSE == pQueryLog->Exec(szQuery))
	{

		return;
	}


}

void CDbManager::RemoveGuildMember(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer, idGuild, DoidPlayer;
	ar >> idPlayer >> idGuild >> DoidPlayer;

	GUILD_QUERYINFO qi("D2");
	qi.idPlayer = idPlayer;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}
	GUILDLOG_QUERYINFO LogQi("L1");
	LogQi.idGuild = idGuild;
	LogQi.idPlayer = idPlayer;
	LogQi.idDoPlayer = DoidPlayer;
	if (idPlayer == DoidPlayer)
		LogQi.pszState = "L";
	else
		LogQi.pszState = "R";

	DBQryGuildLog(szQuery, LogQi);
	if (FALSE == pQueryLog->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::UpdateGuildMemberLv(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer;
	int nMemberLv;
	ar >> idPlayer >> nMemberLv;

	GUILD_QUERYINFO qi("U2");
	qi.idPlayer = idPlayer;
	qi.nLevel = nMemberLv;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::UpdateGuildClass(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer;
	int nClass;
	ar >> idPlayer >> nClass;

	GUILD_QUERYINFO qi("U9");
	qi.idPlayer = idPlayer;
	qi.nClass = nClass;
	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::UpdateGuildNickName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer;
	char strNickName[MAX_GM_ALIAS] = { 0, };
	ar >> idPlayer;
	ar.ReadString(strNickName, MAX_GM_ALIAS);

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidGameName(strNickName))
	{
		CString str;
		str.Format("CDbManager::UpdateGuildNickName() err: %s - guildnickname", strNickName);
		Error(str);

		return;
	}
#endif

	GUILD_QUERYINFO qi("UA");
	qi.idPlayer = idPlayer;
	qi.pszGuild = strNickName;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::UpdateGuildMaster(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idPlayer, idPlayer2;
	ar >> idPlayer >> idPlayer2;

	GUILD_QUERYINFO qi("U2");
	qi.idPlayer = idPlayer;
	qi.nLevel = GUD_ROOKIE;
	qi.nClass = 0;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

	qi.idPlayer = idPlayer2;
	qi.nLevel = GUD_MASTER;
	DBQryGuild(szQuery, qi);

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}


}

void CDbManager::OpenGuild(void)
{
	//	0
	g_GuildMng.Clear();

	//	1
	CQuery* pQuery = new CQuery;
	if (pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		char s[128];
		sprintf(s, "DB open failed: %s", DSN_NAME_CHARACTER01);
		AfxMessageBox(s);
		SAFE_DELETE(pQuery);
		return;
	}

	char szSql[1024];

	DBQryGuild(szSql, "S1");	// ORDER BY
	if (FALSE == pQuery->Exec(szSql))
	{
		Error("sql error: %s", szSql);
		SAFE_DELETE(pQuery);
		return;
	}

	while (pQuery->Fetch())
	{
		CGuild* pGuild = new CGuild;

		pGuild->m_idGuild = (u_long)pQuery->GetInt("m_idGuild");
		pGuild->m_nLevel = pQuery->GetInt("m_nLevel");
		pGuild->m_dwContributionPxp = (DWORD)pQuery->GetInt("m_nGuildPxp");

		pGuild->m_adwPower[GUD_MASTER] = 0x000000FF;
		pGuild->m_adwPower[GUD_KINGPIN] = (DWORD)pQuery->GetInt("Lv_1");
		pGuild->m_adwPower[GUD_CAPTAIN] = (DWORD)pQuery->GetInt("Lv_2");
		pGuild->m_adwPower[GUD_SUPPORTER] = (DWORD)pQuery->GetInt("Lv_3");
		pGuild->m_adwPower[GUD_ROOKIE] = (DWORD)pQuery->GetInt("Lv_4");
		pGuild->m_bActive = (pQuery->GetChar("isuse") == 'F');

		pQuery->GetStr("m_szNotice", pGuild->m_szNotice);
		pQuery->GetStr("m_szGuild", pGuild->m_szGuild);
		StringTrimRight(pGuild->m_szGuild);

		pGuild->m_nGoldGuild = (DWORD)pQuery->GetInt("m_nGuildGold");
#ifdef __FL_GUILD_FINDER
		pGuild->m_bFinder = (BOOL)pQuery->GetInt("m_bFinder");
#endif

		g_GuildMng.AddGuild(pGuild);
	}

	DBQryGuild(szSql, "S2");
	if (FALSE == pQuery->Exec(szSql))
	{
		Error("sql error: %s", szSql);
		SAFE_DELETE(pQuery);
		return;
	}

	u_long idGuild;
	while (pQuery->Fetch())
	{
		CGuildMember* pMember = new CGuildMember;
		pMember->m_idPlayer = (u_long)pQuery->GetInt("m_idPlayer");
		pMember->m_nMemberLv = pQuery->GetInt("m_nMemberLv");
		pMember->m_nGiveGold = pQuery->GetInt("m_nGiveGold");
		pMember->m_dwGivePxpCount = (DWORD)pQuery->GetInt("m_nGivePxp");
		pMember->m_nPay = pQuery->GetInt("m_nPay");
		pMember->m_nClass = pQuery->GetInt("m_nClass");

		pQuery->GetStr("m_szAlias", pMember->m_szAlias);
		idGuild = (u_long)pQuery->GetInt("m_idGuild");
		CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
		if (pGuild)
		{
			pGuild->AddMember(pMember);
			if (pMember->m_nMemberLv == GUD_MASTER)
				pGuild->m_idMaster = pMember->m_idPlayer;
		}
		else
		{
			WriteLog("OpenGuild(): Player's guild not found - %d, %d", pMember->m_idPlayer, idGuild);
			SAFE_DELETE(pMember);
		}
	}

	SAFE_DELETE(pQuery);
}


void CDbManager::UpdateGuildContribution(CQuery* pQuery, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	BYTE nLevelUp;
	LONG nMemberLv;
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	CONTRIBUTION_CHANGED_INFO cci;
	ar >> cci;
	ar >> nLevelUp;
	ar >> nMemberLv;

	GUILD_QUERYINFO info("U4");
	info.idGuild = cci.idGuild;
	info.nGuildPxp = cci.dwGuildPxpCount;
	info.nGuildGold = cci.dwGuildPenya;
	info.nLevel = cci.nGuildLevel;
	info.idPlayer = cci.idPlayer;
	info.dwLv1 = cci.dwPenya;
	info.dwLv2 = cci.dwPxpCount;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);
	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}
	if (info.dwLv1 != 0)
	{

		GUILDLOG_QUERYINFO qi("L2");
		qi.idGuild = info.idGuild;
		qi.idPlayer = info.idPlayer;
		qi.nGuildGold = info.nGuildGold;
		qi.nItem = info.dwLv1;
		if (nLevelUp)
		{
			qi.pszState = "L";
		}
		else
		{
			qi.pszState = "I";
		}
		DBQryGuildLog(szQuery, qi);
		if (FALSE == pQueryLog->Exec(szQuery))
		{

			return;
		}
		GUILDLOG_QUERYINFO qi1("L4");
		qi1.idGuild = info.idGuild;
		qi1.idPlayer = info.idPlayer;
		qi1.nGuildGold = info.nGuildGold;
		qi1.nLevel = nMemberLv;
		qi1.nGuildLv = info.nLevel;
		qi1.nGuildPxp = info.nGuildPxp;
		if (nLevelUp)
		{
			qi1.pszState = "L";
		}
		else
		{
			qi1.pszState = "G";
		}
		DBQryGuildLog(szQuery, qi1);
		if (FALSE == pQueryLog->Exec(szQuery))
		{

			return;
		}
	}
	else
	{

		GUILDLOG_QUERYINFO qi2("L4");
		qi2.idGuild = info.idGuild;
		qi2.idPlayer = info.idPlayer;
		qi2.nGuildGold = info.nGuildGold;
		qi2.nLevel = nMemberLv;
		qi2.nGuildLv = info.nLevel;
		qi2.nGuildPxp = info.nGuildPxp;
		if (nLevelUp)
		{
			qi2.pszState = "L";
		}
		else
		{
			qi2.pszState = "P";
		}
		DBQryGuildLog(szQuery, qi2);
		if (FALSE == pQueryLog->Exec(szQuery))
		{

			return;
		}
	}

}

void CDbManager::UpdateGuildNotice(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idGuild;
	char szNotice[MAX_BYTE_NOTICE];
	ar >> idGuild;
	ar.ReadString(szNotice, MAX_BYTE_NOTICE);

	GUILD_QUERYINFO info("U5");
	info.idGuild = idGuild;
	info.pszNotice = szNotice;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);

	SQLINTEGER cbLen = SQL_NTS;
	if (pQuery->BindParameter(1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 127, 0, szNotice, 0, &cbLen) == FALSE)
	{

		return;
	}

	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}

void CDbManager::UpdateGuildAuthority(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idGuild;
	DWORD adwAuthority[MAX_GM_LEVEL];

	ar >> idGuild;
	ar.Read(adwAuthority, sizeof(DWORD) * MAX_GM_LEVEL);

	GUILD_QUERYINFO info("U1");
	info.idGuild = idGuild;
	info.dwLv1 = adwAuthority[GUD_KINGPIN];
	info.dwLv2 = adwAuthority[GUD_CAPTAIN];
	info.dwLv3 = adwAuthority[GUD_SUPPORTER];
	info.dwLv4 = adwAuthority[GUD_ROOKIE];

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);
	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}


#ifdef __FL_GUILD_FINDER
void CDbManager::UpdateGuildFinderState(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idGuild;
	BOOL bFinder;

	ar >> idGuild >> bFinder;

	GUILD_QUERYINFO info("E1");
	info.idGuild = idGuild;
	info.bFinder = bFinder;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);

	if (FALSE == pQuery->Exec(szQuery))
	{
		return;
	}

}
#endif

void CDbManager::UpdateGuildSetName(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long idGuild;
	char szName[MAX_G_NAME];
	ar >> idGuild;
	ar.ReadString(szName, MAX_G_NAME);

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidGameName(szName))
	{
		CString str;
		str.Format("CDbManager::UpdateGuildSetName() err: %s - guildname", szName);
		Error(str);

		return;
	}
#endif

	GUILD_QUERYINFO info("U8");
	info.idGuild = idGuild;
	info.pszGuild = szName;

	char szQuery[QUERY_SIZE] = { 0, };
	DBQryGuild(szQuery, info);
	if (FALSE == pQuery->Exec(szQuery))
	{

		return;
	}

}
void CDbManager::SetStrDBFormat(char* szDst, const char* szSrc)
{
	char* pCur = szDst + strlen(szDst);
	*pCur++ = ',';

	while (*szSrc)
	{
		sprintf(pCur, "%2X", (BYTE)*szSrc);
		pCur += 2;
		szSrc++;
	}
	*pCur = '\0';
}

inline void CDbManager::DBQryGuild(char* szSql, const char* szType, u_long idPlayer, u_long idGuild, const char* szGuild)
{
	sprintf(szSql, "GUILD_STR '%s', '%07d', '%02d', '%06d', '%s'", szType, idPlayer, g_appInfo.dwSys, idGuild, szGuild);
}

void CDbManager::DBQryGuild(char* szSql, const GUILD_QUERYINFO& info)
{
	if (info.pszType[0] == 'U' && info.pszType[1] == '5')
	{
		sprintf(szSql, "{ call GUILD_STR('%s',"
			"'%07d','%02d','%06d','%s',%d,%d,%d,%d,"
			"%d,%d,%d,"
#ifndef __FL_GUILD_FINDER
			"?,%d) }",
#else
			"?,%d,%d) }",
#endif
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild, info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp,
			info.nClass
#ifdef __FL_GUILD_FINDER
			, info.bFinder
#endif
		);

	}
	else
	{
		sprintf(szSql, "GUILD_STR '%s',"
			"@im_idPlayer='%07d',@iserverindex='%02d',@im_idGuild='%06d',@im_szGuild='%s',@iLv_1=%d,@iLv_2=%d,@iLv_3=%d,@iLv_4=%d,"
			"@im_nLevel=%d,@im_nGuildGold=%d,@im_nGuildPxp=%d,"
#ifndef __FL_GUILD_FINDER
			"@im_szNotice='%s',@im_nClass=%d",
#else
			"@im_szNotice='%s',@im_nClass=%d,@im_bFinder=%d ",
#endif
			info.pszType,
			info.idPlayer, g_appInfo.dwSys, info.idGuild, info.pszGuild, info.dwLv1, info.dwLv2, info.dwLv3, info.dwLv4,
			info.nLevel, info.nGuildGold, info.nGuildPxp,
			info.pszNotice, info.nClass
#ifdef __FL_GUILD_FINDER
			, info.bFinder
#endif
		);

	}
}
void CDbManager::DbQryMail(char* szSql, const MAIL_QUERYINFO& info)
{
	sprintf(szSql, "MAIL_STR '%s', @nMail=%d, @serverindex='%02d', @idReceiver='%07d', @idSender='%07d', @nGold=%d, @tmCreate=%d, @byRead=%d, @szTitle='%s', @szText='%s',"
		"@dwItemId=%d, @nItemNum=%d, @nHitPoint=%d, @nMaterial=%d, @byFlag=%d, @dwSerialNumber=%d,"
		"@nOption=%d, @bItemResist=%d, @nResistAbilityOption=%d, @nResistSMItemId=%d, @dwKeepTime=%d,"
		"@nRandomOptItemId=%I64d,"
		"@nPiercedSize=%d, @dwItemId1=%d, @dwItemId2=%d, @dwItemId3=%d, @dwItemId4=%d"
		", @bPet=%d, @nKind=%d, @nLevel=%d, @dwExp=%d, @wEnergy=%d, @wLife=%d,"
		"@anAvailLevel_D=%d, @anAvailLevel_C=%d, @anAvailLevel_B=%d, @anAvailLevel_A=%d, @anAvailLevel_S=%d"
		", @dwItemId5=%d"
#ifdef __FL_FIX_MAILVALUES
		", @szPetName='%s'"
		", @bTransformPet=%d"
		", @tmPetVis1=%d"
		", @tmPetVis2=%d"
		", @tmPetVis3=%d"
		", @tmPetVis4=%d"
		", @tmPetVis5=%d"
		", @tmPetVis6=%d"
		", @tmPetVis7=%d"
		", @tmPetVis8=%d"
		", @tmPetVis9=%d"
#ifdef __MODEL_CHANGE
		", @nItemModel=%d"
#endif
#endif
#ifdef __FL_PACKED_ITEMS
		", @szItem='%s'"
		", @szExt='%s'"
		", @szPiercing='%s'"
		", @szPet='%s'"
#endif
		, info.pszType, info.nMail, g_appInfo.dwSys, info.idReceiver, info.idSender, info.nGold, info.tmCreate, info.byRead, info.szTitle, info.szText,
		info.dwItemId, info.nItemNum, info.nHitPoint, info.nMaterial, info.byFlag, info.iSerialNumber,
		info.nOption, info.bItemResist, info.nResistAbilityOption, info.nResistSMItemId, info.dwKeepTime,
		info.iRandomOptItemId,
		info.nPiercedSize, info.dwItemId1, info.dwItemId2, info.dwItemId3, info.dwItemId4
		, info.bPet, info.nKind, info.nLevel, info.dwExp, info.wEnergy, info.wLife,
		info.anAvailLevel[PL_D], info.anAvailLevel[PL_C], info.anAvailLevel[PL_B], info.anAvailLevel[PL_A], info.anAvailLevel[PL_S]
		, info.dwItemId5
#ifdef __FL_FIX_MAILVALUES
		, info.szPetName
		, info.bTransformPet
		, info.tmPetVis1
		, info.tmPetVis2
		, info.tmPetVis3
		, info.tmPetVis4
		, info.tmPetVis5
		, info.tmPetVis6
		, info.tmPetVis7
		, info.tmPetVis8
		, info.tmPetVis9
#ifdef __MODEL_CHANGE
		, info.nItemModel
#endif
#endif
#ifdef __FL_PACKED_ITEMS
		, info.szItem
		, info.szExt
		, info.szPiercing
		, info.szPet
#endif
	);
}

void CDbManager::MakeQueryAddMail(char* szSql, CMail* pMail, u_long idReceiver)
{
	CItemElem item;
	CPet pet;
	BOOL bPet = FALSE;
#ifdef __FL_PACKED_ITEMS
	bool bPackage = false;
	ItemContainerStruct ics;
#endif

	if (pMail->m_pItemElem)
	{
		item = *pMail->m_pItemElem;
		if (pMail->m_pItemElem->m_pPet)
		{
			pet = *pMail->m_pItemElem->m_pPet;
			bPet = TRUE;
		}
#ifdef __FL_PACKED_ITEMS
		if (pMail->m_pItemElem->IsPackage() && !pMail->m_pItemElem->GetItemList()->empty())
		{
			CItemContainer<CItemElem> packageContent;
			packageContent.SetItemContainer(ITYPE_ITEM, MAX_ITEMS_IN_PACKAGE);

			g_DbManager.SaveOnePackage(pMail->m_pItemElem, &packageContent, &ics);

			bPackage = true;
		}
#endif
	}
	else
		item.m_nItemNum = 0;

#ifdef __FL_PACKED_ITEMS
	if (!bPackage)
	{
		strcat(ics.szItem, NullStr);
		strcat(ics.szExt, NullStr);
		strcat(ics.szPiercing, NullStr);
		strcat(ics.szPet, NullStr);
	}
#endif

	if (item.GetProp() && item.GetProp()->IsUltimate())
	{
		sprintf(szSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?"
			",%d, %d, %d, %d, %d, %d"
			",%d, %d, %d, %d, %d"
			",%I64d"
			",%d, %d, %d, %d, %d"
			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			",%d"
			",%d, %d, %d, %d, %d"
			",%d, %d, %d, %d, %d"
			",%d"
#ifdef __FL_FIX_MAILVALUES
			",'%s'"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
#ifdef __MODEL_CHANGE
			",%d"
#endif
#endif
#ifdef __FL_PACKED_ITEMS
			",'%s'"
			",'%s'"
			",'%s'"
			",'%s'"
#endif
			")}",

			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
			item.m_dwItemId, item.m_nItemNum, item.m_nHitPoint, 0, item.m_byFlag, item.GetSerialNumber(),
			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, 0, item.m_dwKeepTime,
			item.GetRandomOptItemId(),
			item.GetUltimatePiercingSize(), item.GetUltimatePiercingItem(0), item.GetUltimatePiercingItem(1), item.GetUltimatePiercingItem(2), item.GetUltimatePiercingItem(3),
			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
			pet.GetAvailLevel(PL_D), pet.GetAvailLevel(PL_C), pet.GetAvailLevel(PL_B), pet.GetAvailLevel(PL_A), pet.GetAvailLevel(PL_S)
			, item.GetUltimatePiercingItem(4)
			, item.GetPiercingItem(0), item.GetPiercingItem(1), item.GetPiercingItem(2), item.GetPiercingItem(3), item.GetPiercingItem(4)
			, item.GetPiercingItem(5), item.GetPiercingItem(6), item.GetPiercingItem(7), item.GetPiercingItem(8), item.GetPiercingItem(9)
			, item.GetPiercingSize()
#ifdef __FL_FIX_MAILVALUES
			, pet.GetName()
			, item.m_bTranformVisPet
			, item.GetVisKeepTime(0)
			, item.GetVisKeepTime(1)
			, item.GetVisKeepTime(2)
			, item.GetVisKeepTime(3)
			, item.GetVisKeepTime(4)
			, item.GetVisKeepTime(5)
			, item.GetVisKeepTime(6)
			, item.GetVisKeepTime(7)
			, item.GetVisKeepTime(8)
#ifdef __MODEL_CHANGE
			, item.GetLook()
#endif
#endif
#ifdef __FL_PACKED_ITEMS
			, ics.szItem
			, ics.szExt
			, ics.szPiercing
			, ics.szPet
#endif
		);
	}
	else
	{
		sprintf(szSql, "{call MAIL_STR('A1', %d, '%02d', '%07d', '%07d', %d, %d, %d, ?, ?,"
			"%d, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d,"
			"%I64d,"
			"%d, %d, %d, %d, %d"
			",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
			",%d"
			",%d, %d, %d, %d, %d"
			",%d, %d, %d, %d, %d"
			",%d"
#ifdef __FL_FIX_MAILVALUES
			",'%s'"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
			",%d"
#ifdef __MODEL_CHANGE
			",%d"
#endif
#endif
#ifdef __FL_PACKED_ITEMS
			",'%s'"
			",'%s'"
			",'%s'"
			",'%s'"
#endif
			")}",

			pMail->m_nMail, g_appInfo.dwSys, idReceiver, pMail->m_idSender, pMail->m_nGold, pMail->m_tmCreate, pMail->m_byRead,
			item.m_dwItemId, item.m_nItemNum, item.m_nHitPoint, 0, item.m_byFlag, item.GetSerialNumber(),
			item.GetOption(), item.m_bItemResist, item.m_nResistAbilityOption, 0, item.m_dwKeepTime,
			item.GetRandomOptItemId(),
			item.GetPiercingSize(), item.GetPiercingItem(0), item.GetPiercingItem(1), item.GetPiercingItem(2), item.GetPiercingItem(3),
			bPet, pet.GetKind(), pet.GetLevel(), pet.GetExp(), pet.GetEnergy(), pet.GetLife(),
			pet.GetAvailLevel(PL_D), pet.GetAvailLevel(PL_C), pet.GetAvailLevel(PL_B), pet.GetAvailLevel(PL_A), pet.GetAvailLevel(PL_S)
			, item.GetPiercingItem(4)
			, item.GetPiercingItem(5), item.GetPiercingItem(6), item.GetPiercingItem(7), item.GetPiercingItem(8), item.GetPiercingItem(9)
			, 0, 0, 0, 0, 0
			, 0
#ifdef __FL_FIX_MAILVALUES
			, pet.GetName()
			, item.m_bTranformVisPet
			, item.GetVisKeepTime(0)
			, item.GetVisKeepTime(1)
			, item.GetVisKeepTime(2)
			, item.GetVisKeepTime(3)
			, item.GetVisKeepTime(4)
			, item.GetVisKeepTime(5)
			, item.GetVisKeepTime(6)
			, item.GetVisKeepTime(7)
			, item.GetVisKeepTime(8)
#ifdef __MODEL_CHANGE
			, item.GetLook()
#endif
#endif
#ifdef __FL_PACKED_ITEMS
			, ics.szItem
			, ics.szExt
			, ics.szPiercing
			, ics.szPet
#endif
		);
	}
}

void CDbManager::DBQryGuildLog(char* szSql, const GUILDLOG_QUERYINFO& info)
{
	sprintf(szSql, "LOG_GUILD_STR '%s', @im_idGuild='%06d', @im_idPlayer='%07d', @iserverindex='%02d',"
		"@im_nGuildGold=%d, @im_nLevel=%d, @im_GuildLv=%d, @iGuildPoint=%d,"
		"@iDo_m_idPlayer='%07d', @iState='%s', @im_Item=%d, @im_GuildBank='%s',"
		"@im_nAbilityOption=%d, @iItem_count=%d, @iItem_UniqueNo=%d, @is_date='%s'",
		info.pszType, info.idGuild, info.idPlayer, g_appInfo.dwSys,
		info.nGuildGold, info.nLevel, info.nGuildLv, info.nGuildPxp,
		info.idDoPlayer, info.pszState, info.nItem, info.pszGuildBank,
		info.nAbilityOption, info.nItem_count, info.iUniqueNo, info.pszTime);

	if (strlen(szSql) > 8012)
		TRACE("%s\n", szSql);
}


void CDbManager::QueryGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	TRACE("PACKETTYPE_QUERYGUILDQUEST\n");
	//	GUILD_QUEST_STR 'S1', @im_idGuild,@iserverindex
	char	pszSql[128];
	sprintf(pszSql, "GUILD_QUEST_STR 'S1', @im_idGuild = '', @iserverindex = '%02d'", g_appInfo.dwSys);
	if (!pQuery->Exec(pszSql))
	{

		return;
	}

	int nCount = 0;
	u_long idGuild = 0, idCurr;
	GUILDQUEST pQuest[MAX_GUILD_QUEST];
	BYTE nQuestSize;

	BEFORESENDDUAL(ar, PACKETTYPE_QUERYGUILDQUEST, DPID_UNKNOWN, DPID_UNKNOWN);
	u_long uOffset = ar.GetOffset();
	ar << static_cast<int>(0);	// nCount

	while (pQuery->Fetch())
	{
		idCurr = (u_long)pQuery->GetInt("m_idGuild");
		if (idCurr != idGuild)
		{
			if (idGuild != 0)
			{
				ar << idGuild << nQuestSize;
				ar.Write((void*)pQuest, sizeof(GUILDQUEST) * nQuestSize);
				nCount++;
			}
			nQuestSize = 0;
			idGuild = idCurr;
		}
		pQuest[nQuestSize].nId = pQuery->GetInt("n_Id");
		pQuest[nQuestSize].nState = pQuery->GetInt("nState");
		nQuestSize++;
	}

	if (idGuild != 0)
	{
		ar << idGuild << nQuestSize;
		ar.Write((void*)pQuest, sizeof(GUILDQUEST) * nQuestSize);
		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer = ar.GetBuffer(&nBufferSize);
	*(UNALIGNED int*)(pBuffer + uOffset) = nCount;
	SEND(ar, CDPTrans::GetInstance(), lpDbOverlappedPlus->dpid);


}

void CDbManager::InsertGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	TRACE("PACKETTYPE_SETGUILDQUEST\n");
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idGuild;
	int nId;
	ar >> idGuild >> nId;

	char	pszSql[128];
	sprintf(pszSql, "GUILD_QUEST_STR 'A1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d",
		idGuild, g_appInfo.dwSys, nId);

	if (!pQuery->Exec(pszSql))
	{

		return;
	}

}

void CDbManager::UpdateGuildQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	TRACE("PACKETTYPE_UPDATEGUILDQUEST\n");
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idGuild;
	int nId, nState;
	ar >> idGuild >> nId >> nState;

	char	pszSql[128];
	sprintf(pszSql, "GUILD_QUEST_STR 'U1', @im_idGuild = '%06d', @iserverindex = '%02d', @in_Id = %d, @in_State = %d",
		idGuild, g_appInfo.dwSys, nId, nState);

	if (!pQuery->Exec(pszSql))
	{

		return;
	}

}


void CDbManager::GuildThread(void)
{
	CQuery* pQuery = new CQuery;
	if (FALSE == pQuery->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d GuildDB Connect", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
		return;
	}

#ifdef __FL_PACKED_ITEMS
	CQuery* pQuery1 = new CQuery;
	if (FALSE == pQuery1->Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01))
	{
		WriteLog("%s, %d GuildDB Connect", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
		SAFE_DELETE(pQuery1);
		return;
	}
#endif

	CQuery* pQueryLog = new CQuery;
	if (FALSE == pQueryLog->Connect(3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, DB_ADMIN_PASS_LOG))
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		SAFE_DELETE(pQuery);
#ifdef __FL_PACKED_ITEMS
		SAFE_DELETE(pQuery1);
#endif
		SAFE_DELETE(pQueryLog);
		return;
	}

	SetEvent(s_hHandle);

	BOOL bReturnValue = FALSE;
	DWORD dwBytesTransferred = 0;
	DWORD dwCompletionKey = 0;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = NULL;

	while (true)
	{
		bReturnValue = GetQueuedCompletionStatus(m_hIOCPGuild, &dwBytesTransferred, &dwCompletionKey, (LPOVERLAPPED*)&lpDbOverlappedPlus, INFINITE);
		if (FALSE == bReturnValue)
		{
			Error("CDbManager::GuildThread (1). Error:[%d]", ::GetLastError());
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			continue;
		}

		if (dwBytesTransferred == 0)
		{
			SAFE_DELETE(pQuery);
#ifdef __FL_PACKED_ITEMS
			SAFE_DELETE(pQuery1);
#endif
			SAFE_DELETE(pQueryLog);
			SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);
			return;
		}

		if (dwCompletionKey != 0 && lpDbOverlappedPlus == NULL)
		{
			CDPTrans::GetInstance()->SendAllGuildCombat((DPID)dwCompletionKey);
			continue;
		}
		switch (lpDbOverlappedPlus->nQueryMode)
		{
		case CREATE_GUILD:
			CreateGuild(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case DESTROY_GUILD:
			DestroyGuild(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case ADD_GUILD_MEMBER:
			AddGuildMember(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case REMOVE_GUILD_MEMBER:
			RemoveGuildMember(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_MEMBER_LEVEL:
			UpdateGuildMemberLv(pQuery, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_CLASS:
			UpdateGuildClass(pQuery, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_NICKNAME:
			UpdateGuildNickName(pQuery, lpDbOverlappedPlus);
			break;
		case QM_UPDATEGUILDMASTER:
			UpdateGuildMaster(pQuery, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_CONTRIBUTION:
			UpdateGuildContribution(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_NOTICE:
			UpdateGuildNotice(pQuery, lpDbOverlappedPlus);
			break;
		case UPDATE_GUILD_AUTHORITY:
			UpdateGuildAuthority(pQuery, lpDbOverlappedPlus);
			break;
#ifdef __FL_GUILD_FINDER
		case UPDATE_GUILD_FINDERSTATE:
			UpdateGuildFinderState(pQuery, lpDbOverlappedPlus);
			break;
#endif
		case GUILD_SETNAME:
			UpdateGuildSetName(pQuery, lpDbOverlappedPlus);
			break;
		case GUILD_BANK_UPDAATE:
			UpdateGuildBankUpdate(pQuery, pQueryLog, lpDbOverlappedPlus);
			break;
		case GUILD_BANK_QUERY:
#ifndef __FL_PACKED_ITEMS
			OpenQueryGuildBank(pQuery, lpDbOverlappedPlus);
#else
			OpenQueryGuildBank(pQuery, lpDbOverlappedPlus, pQuery1);
#endif
			break;

		case QM_QUERYGUILDQUEST:
			QueryGuildQuest(pQuery, lpDbOverlappedPlus);
			break;
		case QM_INSERTGUILDQUEST:
			InsertGuildQuest(pQuery, lpDbOverlappedPlus);
			break;
		case QM_UPDATEGUILDQUEST:
			UpdateGuildQuest(pQuery, lpDbOverlappedPlus);
			break;
		case START_GUILDCOMBAT:
			StartGuildCombat(pQuery, lpDbOverlappedPlus);
			break;
		case IN_GUILDCOMBAT:
			InGuildCombat(pQuery, lpDbOverlappedPlus);
			break;
		case OUT_GUILDCOMBAT:
			OutGuildCombat(pQuery, lpDbOverlappedPlus);
			break;
		case RESULT_GUILDCOMBAT:
			ResultGuildCombat(pQuery, lpDbOverlappedPlus);
			break;
		case CONTINUE_GUILDCOMBAT:
			ContinueGC(pQuery, lpDbOverlappedPlus);
			break;
		case ADD_MAIL:
			AddMail(pQuery, lpDbOverlappedPlus);
			break;
		case REMOVE_MAIL:
			RemoveMail(pQuery, lpDbOverlappedPlus);
			break;
		case REMOVE_MAIL_ITEM:
			RemoveMailItem(pQuery, lpDbOverlappedPlus);
			break;
		case REMOVE_MAIL_GOLD:
			RemoveMailGold(pQuery, lpDbOverlappedPlus);
			break;
		case READ_MAIL:
			ReadMail(pQuery, lpDbOverlappedPlus);
			break;
#ifdef __GETMAILREALTIME
		case QM_GETMAIL_REALTIME:
			QueryGetMailRealTime(pQuery);
			break;
#endif // __GETMAILREALTIME
#if __VER >= 11
		case QM_GUILDBANK_LOG_VIEW:
			GuildBankLogView(pQuery, lpDbOverlappedPlus);
			break;
#endif
#ifdef __JEFF_FIX_0
		case QM_ALL_PLAYER_DATA:
		{
			CDPTrans::GetInstance()->SendAllPlayerData(lpDbOverlappedPlus->dpid);
			CDPTrans::GetInstance()->SendAllMail(lpDbOverlappedPlus->dpid);
			break;
		}
		case QM_QUERY_MAIL_BOX:
		{
			CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
			u_long idReceiver;
			ar >> idReceiver;
			CPost* pPost = CPost::GetInstance();
			CMclAutoLock	Lock(pPost->m_csPost);

			CMailBox* pMailBox = NULL;
			pMailBox = pPost->GetMailBox(idReceiver);

			if (pMailBox != NULL)
			{
				CDPTrans::GetInstance()->SendMailBox(pMailBox, lpDbOverlappedPlus->dpid);
			}
			else
			{
				CDPTrans::GetInstance()->SendMailBoxReq(idReceiver, lpDbOverlappedPlus->dpid, FALSE, pMailBox);
			}


			break;
		}
#endif	// __JEFF_FIX_0
#ifdef __FL_CASH_SHOP
		case CSHOP_LOAD:
		{
			CDPTrans::GetInstance()->SendCShop(lpDbOverlappedPlus->dpid);
			break;
		}
		case CSHOP_BALANCE:
			GetCShopBalance(pQuery, lpDbOverlappedPlus);
			break;
		case CSHOP_ITEM:
			BuyCShopItem(pQuery, lpDbOverlappedPlus);
			break;
#endif
		default:
			Error("CDbManager::GuildThread nQueryMode Invalid : [%d]", lpDbOverlappedPlus->nQueryMode);
			break;
		}

		SAFE_DELETE_FREEREQUEST(lpDbOverlappedPlus);

	}
}

void CDbManager::SerializeGuildCombat(CAr& ar)
{
	ar << m_nGuildCombatIndex;
	ar << m_uWinGuildId;
	ar << m_nWinGuildCount;
	ar << m_uBestPlayer;

	ar << (u_long)m_vecGuildCombat.size();
	for (DWORD gci = 0; gci < m_vecGuildCombat.size(); ++gci)
	{
		__GUILDCOMBATJOIN GCJoin = m_vecGuildCombat[gci];
		ar << GCJoin.uGuildId;
		ar << GCJoin.dwPenya;
		ar << GCJoin.bRequest;
	}

	SerializePlayerPoint(ar);
}
void CDbManager::SerializePlayerPoint(CAr& ar)
{

	ar << (u_long)m_vecGCPlayerPoint.size();
	for (DWORD veci = 0; veci < m_vecGCPlayerPoint.size(); ++veci)
	{
		__GCPLAYERPOINT GCPlayerPoint = m_vecGCPlayerPoint[veci];
		ar << GCPlayerPoint.uidPlayer;
		ar << GCPlayerPoint.nJob;
		ar << GCPlayerPoint.nPoint;
	}
}

void CDbManager::InGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	u_long idGuild;
	DWORD dwPenya;
	DWORD dwExistingPenya;
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	ar >> idGuild;
	ar >> dwPenya;
	ar >> dwExistingPenya;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspJoinGuildToCombat %d,'%02d','%06d',%d,%d",
		m_nGuildCombatIndex, g_appInfo.dwSys, idGuild, dwPenya, 0);

	if (pQuery->Exec(szQuery))
	{
		__GUILDCOMBATJOIN GCJoin;
		GCJoin.dwPenya = dwPenya;
		GCJoin.bRequest = TRUE;
		GCJoin.uGuildId = idGuild;
		m_vecGuildCombat.push_back(GCJoin);
		CDPTrans::GetInstance()->SendInGuildCombat(idGuild, dwPenya, dwExistingPenya);
	}
	else
	{
		WriteLog("InGuildCombat()"); TRACE("ERROR: InGuildCombat()\n");
	}


}

void CDbManager::OutGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	u_long idGuild;
	u_long dwChannel;
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	ar >> idGuild;
	ar >> dwChannel;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspCancelGuildToCombat %d,'%02d','%06d'", m_nGuildCombatIndex, g_appInfo.dwSys, idGuild);

	if (pQuery->Exec(szQuery))
	{
		BOOL bFind = FALSE;
		__GUILDCOMBATJOIN GCJoin;
		for (DWORD gci = 0; gci < m_vecGuildCombat.size(); ++gci)
		{
			if (idGuild == m_vecGuildCombat[gci].uGuildId)
			{
				bFind = TRUE;
				GCJoin = m_vecGuildCombat[gci];
				break;
			}
		}
		if (bFind)
			GCJoin.bRequest = FALSE;
		CDPTrans::GetInstance()->SendOutGuildCombat(idGuild, dwChannel);
	}
	else
	{
		WriteLog("OutGuildCombat()");	TRACE("ERROR: OutGuildCombat()\n");
	}


}
void CDbManager::StartGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspStartCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("StartGuildCombat()");	TRACE("ERROR: StartGuildCombat()\n");
	}

}
void CDbManager::ResultGuildCombat(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);


	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspEndCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");
	}


	ar >> m_uWinGuildId;
	ar >> m_nWinGuildCount;
	ar >> m_uBestPlayer;

	int nGuildPoint, nWinCount;
	u_long uidGuild, uSizeGuild;
	__int64 nPrize;
	DWORD dwReturnPenya;

	ar >> uSizeGuild;
	for (DWORD i = 0; i < uSizeGuild; ++i)
	{
		ar >> uidGuild;
		ar >> nGuildPoint;
		ar >> nPrize;
		ar >> dwReturnPenya;
		ar >> nWinCount;
		char szQuery[QUERY_SIZE] = { 0, };
		sprintf(szQuery, "uspGradeCombatGuild %d,'%02d','%06d',%d,%d,%I64d,%d",
			m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, nGuildPoint, dwReturnPenya, nPrize, nWinCount);
		if (FALSE == pQuery->Exec(szQuery))
		{
			WriteLog("ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");
		}

		u_long uSizePlayer;
		u_long uidPlayer;
		int nPlayerPoint;
		__int64 nPrizePlyaer;
		ar >> uSizePlayer;
		for (DWORD j = 0; j < uSizePlayer; ++j)
		{
			ar >> uidPlayer;
			ar >> nPlayerPoint;
			ar >> nPrizePlyaer;
			if (m_uBestPlayer == uidPlayer)
				nPrizePlyaer = 1;

			char szQuery[QUERY_SIZE] = { 0, };
			sprintf(szQuery, "uspGradeCombatPlayer %d,'%02d','%06d','%07d',%d,%I64d",
				m_nGuildCombatIndex, g_appInfo.dwSys, uidGuild, uidPlayer, nPlayerPoint, nPrizePlyaer);
			if (FALSE == pQuery->Exec(szQuery))
			{
				WriteLog("ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");
			}
		}
	}

	m_vecGuildCombat.clear();
	++m_nGuildCombatIndex;


	sprintf(szQuery, "uspAddNewCombat %d,'%02d'", m_nGuildCombatIndex, g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");
	}

	m_vecGCPlayerPoint.clear();
	sprintf(szQuery, "uspRankGuildCombatPlayer '%02d'", g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("ResultGuildCombat()");	TRACE("ERROR: ResultGuildCombat()\n");
	}
	while (pQuery->Fetch())
	{
		__GCPLAYERPOINT GCPlayerPoint;
		GCPlayerPoint.uidPlayer = pQuery->GetInt("PlayerID");
		GCPlayerPoint.nJob = pQuery->GetInt("Job");
		GCPlayerPoint.nPoint = pQuery->GetInt("PointSummary");
		m_vecGCPlayerPoint.push_back(GCPlayerPoint);
	}

	CDPTrans::GetInstance()->SendResultGuildCombat();
	CDPTrans::GetInstance()->SendPlayerPointGuildCombat();


}
void CDbManager::ContinueGC(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	u_long uidGuild;
	int nGuildCombatID;
	int nWinCount;
	ar >> nGuildCombatID;
	ar >> uidGuild;
	ar >> nWinCount;

	m_uWinGuildId = 0;
	m_nWinGuildCount = 0;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspCombatContinue %d,'%02d','%06d',%d"
		, nGuildCombatID, g_appInfo.dwSys, uidGuild, nWinCount);
	if (FALSE == pQuery->Exec(szQuery))
	{
		WriteLog("GetPenyaGuildGuildCombat()");	TRACE("ERROR: GetPenyaGuildGuildCombat()\n");
	}

	CDPTrans::GetInstance()->SendContinueGC();


}

void CDbManager::PutExpLog(CQuery* pQueryLog, CMover* pMover, char ch)
{
	BOOL bPut = (ch != 'P');
	DWORD dwTick = GetTickCount();
	if (!bPut)
	{
		if (pMover->m_dwPeriodicTick < dwTick)
			bPut = TRUE;
	}
	if (bPut)
	{
		pMover->m_dwPeriodicTick = dwTick + MIN(120);

		char szSql[1024] = { 0, };
		DBQryLog(szSql, "L1", pMover->m_idPlayer, g_appInfo.dwSys,
			pMover->m_nExp1, pMover->m_nLevel,
			pMover->m_nJob, 0, pMover->GetFlightLv(),
			pMover->m_nStr, pMover->m_nDex, pMover->m_nInt, pMover->m_nSta,
			pMover->m_nRemainGP, 0,
			ch, 0, '\0', 0, 0, 0, pMover->GetGold(), 0);

		if (FALSE == pQueryLog->Exec(szSql))
			return;
	}
}

void CDbManager::SetPlayerName(CQuery* pQueryChar, CQuery* pQueryLog, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idPlayer;
	DWORD dwData;
	char lpszPlayer[MAX_PLAYER];
	ar >> idPlayer;
	ar.ReadString(lpszPlayer, MAX_PLAYER);
	ar >> dwData;

#ifdef __FIX_SQL_INJECTS
	if (IsInvalidGameName(lpszPlayer))
	{
		CString str;
		str.Format("CDbManager::SetPlayerName() err: %s - character", lpszPlayer);
		Error(str);

		return;
	}
#endif

	BOOL f = FALSE;
	char szQuery[QUERY_SIZE] = { 0, };
	DBQryCharacter(szQuery, "U4", idPlayer, g_appInfo.dwSys, NULL, lpszPlayer);
	if (pQueryChar->Exec(szQuery))
	{
		if (pQueryChar->Fetch())
			f = (BOOL)pQueryChar->GetInt("fError");
		char lpOutputString[128] = { 0, };
		sprintf(lpOutputString, "DATABASESERVER.EXE\t// QM_SET_PLAYER_NAME\t//1(%d)\n", f);
		OutputDebugString(lpOutputString);
	}
	else
	{
		OutputDebugString("DATABASESERVER.EXE\t// QM_SET_PLAYER_NAME\t//0(0)\n");
	}

	if (f)
	{
		char szOldPlayer[MAX_PLAYER] = { 0, };
		CPlayerDataCenter::GetInstance()->ReplacePlayerString(idPlayer, lpszPlayer, szOldPlayer);

		char szQuery[QUERY_SIZE] = { 0, };
		sprintf(szQuery, "uspChangeNameLog @pserverindex = '%02d', @pidPlayer = '%07d', @pOldName='%s', @pNewName='%s'",
			g_appInfo.dwSys, idPlayer, szOldPlayer, lpszPlayer);
		if (pQueryLog->Exec(szQuery) == FALSE)
			Error("query failed: %s", szQuery);
	}
	g_dpCoreSrvr.SendSetPlayerName(idPlayer, lpszPlayer, dwData, f);


}

void CDbManager::DBQryCharacter(char* qryCharacter, char* Gu, u_long idPlaeyr, int nserverindex, char* szAccount, char* szName,
	int nPlayerslot, DWORD dwWorldID, DWORD dwIndex, float vPos_x, float vPos_y,
	float vPos_z, char* szCharacterKey, DWORD dwSkinSet, DWORD dwHairMesh, DWORD dwHairColor,
	DWORD dwHeadMesh, DWORD dwSex, float vScale_x, DWORD dwMotion, float fAngle,
	int nHitPoint, int nManaPoint, int nFatiguePoint, DWORD dwRideItemIdx, DWORD dwGold,
	int nJob, char* szActMover, int nStr, int nSta, int nDex,
	int nInt, int nLevel, EXPINTEGER nExp1, EXPINTEGER nExp2, char* szJobSkill,
	char* szLicenseSkill, char* aJobLv, DWORD dwExpertLv, int nidMarkingWorld, float vMarkingPos_x,
	float vMarkingPos_y, float vMarkingPos_z, int nRemainGP, int nRemainLP, int nFlightLv,
	int nFxp, int nTxp, char* szQuestCntArray, char szAuthority, DWORD dwMode,
	int nidparty,
	DWORD dwFlyTime, DWORD dwMessengerState,
	int nTotalPlayTime, char* szCard, char* szIndex_Card, char* szObjIndexCard, char* szCube,
	char* szIndex_Cube, char* szObjIndex_Cube, char* szInventory, char* dzapIndex, char* szadwEquipment,
	char* szdwObjIndex, char* szSlotApplet, char* szSlotItem, char* szSlotQueue, int nActionPoint,
	char* szBank, char* m_apIndex_Bank, char* sadwObjIndex_Bank, DWORD dwGoldBank,
	int nFuel, int tmAccFuel, char* szSMTime, char* szSkillInfluence, DWORD m_dwSkillPoint
	, char* m_aCompleteQuest
	, char* ExtInven, char* PirecingInven, char* ExtBank, char* PirecingBank
	, DWORD dwReturnWorldID, float fReturnPosX, float fReturnPosY, float fReturnPosZ
	, char* szInventoryPet, char* szBankPet, DWORD dwPetId
	, int nHonor
	, int nLayer
	, char* m_aCheckedQuest
)
{
	sprintf(qryCharacter, "CHARACTER_STR '%s'", Gu);
	char strCharacter[40960];

	sprintf(strCharacter, ",@im_idPlayer='%07d',@iserverindex='%02d',@iaccount='%s'",
		idPlaeyr, nserverindex, szAccount);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	if (Gu[0] == 'S' && Gu[1] == '8')
		return;

	sprintf(strCharacter, ",@im_szName='%s',@iplayerslot=%d,@idwWorldID=%d,@im_dwIndex=%d,@im_vPos_x=%f,@im_vPos_y=%f",
		szName, nPlayerslot, dwWorldID, dwIndex, vPos_x, vPos_y);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_vPos_z=%f,@im_szCharacterKey='%s',@im_dwSkinSet=%d,@im_dwHairMesh=%d,@im_dwHairColor=%d,@im_dwHeadMesh=%d,@im_dwSex=%d,@im_vScale_x=%f,@im_dwMotion=%d,@im_fAngle=%f",
		vPos_z, szCharacterKey, dwSkinSet, dwHairMesh, dwHairColor, dwHeadMesh, dwSex, vScale_x, dwMotion, fAngle);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_nHitPoint=%d,@im_nManaPoint=%d,@im_nFatiguePoint=%d,@im_dwRideItemIdx=%d,@im_dwGold=%d,@im_nJob=%d,@im_pActMover='%s',@im_nStr=%d,@im_nSta=%d,@im_nDex=%d",
		nHitPoint, nManaPoint, nFatiguePoint, dwRideItemIdx, dwGold, nJob, szActMover, nStr, nSta, nDex);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_nInt=%d,@im_nLevel=%d,@im_nExp1=%I64d,@im_nExp2=%I64d,@im_aJobSkill='%s',@im_aLicenseSkill='%s',@im_aJobLv='%s',@im_dwExpertLv=%d,@im_idMarkingWorld=%d,@im_vMarkingPos_x=%f",
		nInt, nLevel, nExp1, (EXPINTEGER)0, szJobSkill, szLicenseSkill, aJobLv, dwExpertLv, nidMarkingWorld, vMarkingPos_x);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_vMarkingPos_y=%f,@im_vMarkingPos_z=%f,@im_nRemainGP=%d,@im_nRemainLP=%d,@im_nFxp=%d,@im_nTxp=%d,@im_lpQuestCntArray='%s',@im_chAuthority='%c',@im_dwMode=%d",
		vMarkingPos_y, vMarkingPos_z, nRemainGP, nRemainLP, nFxp, nTxp, szQuestCntArray, szAuthority, dwMode);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_idparty=%d,@im_dwFlyTime=%d,@im_nMessengerState=%d,@iTotalPlayTime=%d",
		nidparty, dwFlyTime, dwMessengerState, nTotalPlayTime);

	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_Card='%s',@im_Index_Card='%s',@im_ObjIndex_Card='%s',@im_Cube='%s',@im_Index_Cube='%s',@im_ObjIndex_Cube='%s',@im_Inventory='%s',@im_apIndex='%s',@im_dwObjIndex='%s'",
		szCard, szIndex_Card, szObjIndexCard, szCube, szIndex_Cube, szObjIndex_Cube, szInventory, dzapIndex, szdwObjIndex);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_aSlotApplet='%s',@im_aSlotItem='%s',@im_aSlotQueue='%s',@im_SkillBar=%d,@im_Bank='%s',@im_apIndex_Bank='%s',@im_dwObjIndex_Bank='%s',@im_dwGoldBank=%d,@im_nFuel=%d",
		szSlotApplet, szSlotItem, szSlotQueue, nActionPoint, szBank, m_apIndex_Bank, sadwObjIndex_Bank, dwGoldBank, nFuel);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_tmAccFuel=%d", tmAccFuel);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_dwSMTime='%s'", szSMTime);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@iSkillInfluence='%s'", szSkillInfluence);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_aCompleteQuest='%s'", m_aCompleteQuest);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_extInventory='%s',@im_InventoryPiercing='%s',@im_extBank='%s',@im_BankPiercing ='%s'", ExtInven, PirecingInven, ExtBank, PirecingBank);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_dwReturnWorldID=%d,@im_vReturnPos_x=%f,@im_vReturnPos_y=%f,@im_vReturnPos_z=%f",
		dwReturnWorldID, fReturnPosX, fReturnPosY, fReturnPosZ);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@iszInventoryPet='%s',@iszBankPet='%s', @im_dwPetId=%d",
		szInventoryPet, szBankPet, dwPetId);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	sprintf(strCharacter, ",@im_nHonor=%d",
		nHonor);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));
	sprintf(strCharacter, ",@im_nLayer=%d",
		nLayer);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));
	sprintf(strCharacter, ",@im_aCheckedQuest='%s'", m_aCheckedQuest);
	strncat(qryCharacter, strCharacter, sizeof(strCharacter));

	if (strlen(qryCharacter) > 40960)
	{
		ASSERT(0);
	}
}

BOOL CheckValidItem(DWORD dwItemId, short nNum)
{
	ItemProp* pItemProp = prj.GetItemProp(dwItemId);
	if (NULL == pItemProp)
	{
		return FALSE;
	}
	else
	{
		if (nNum <= 0 || nNum > (short)pItemProp->dwPackMax)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbManager::LoadPost(void)
{
	CPost* pPost = CPost::GetInstance();

	CMclAutoLock	Lock(pPost->m_csPost);
	if (m_qryPostProc.Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		WriteLog("%s, %d", __FILE__, __LINE__);
		AfxMessageBox("Error : Can't connect CHAR_DB");
		return FALSE;
	}

#ifdef __POST_DUP_1209
	set<int>	setnMail;
	int	nTotal = 0;
#endif	// __POST_DUP_1209

	MAIL_QUERYINFO info("S1");
	char szQuery[QUERY_SIZE] = { 0, };
	DbQryMail(szQuery, info);
	if (FALSE == m_qryPostProc.Exec(szQuery))
	{
		AfxMessageBox("QUERY: MAIL_STR 'S1'");
		return FALSE;
	}

	while (m_qryPostProc.Fetch())
	{
		u_long idReceiver = m_qryPostProc.GetInt("idReceiver");
		CMail* pMail = new CMail;
		pMail->m_nMail = m_qryPostProc.GetInt("nMail");
		pMail->m_idSender = m_qryPostProc.GetInt("idSender");

		int nGoldFlag = m_qryPostProc.GetInt("GoldFlag");
		pMail->m_nGold = 0;
		if (nGoldFlag == 0)
			pMail->m_nGold = m_qryPostProc.GetInt("nGold");
		pMail->m_tmCreate = (time_t)m_qryPostProc.GetInt("tmCreate");
		pMail->m_byRead = (BYTE)m_qryPostProc.GetInt("byRead");

		m_qryPostProc.GetStr("szTitle", pMail->m_szTitle);
		pMail->m_szTitle[MAX_MAILTITLE - 1] = '\0';

		m_qryPostProc.GetStr("szText", pMail->m_szText);

		int nItemFlag = m_qryPostProc.GetInt("ItemFlag");
		DWORD dwItemId = m_qryPostProc.GetInt("dwItemId");

		if (dwItemId && nItemFlag == 0)
		{
			pMail->m_pItemElem = new CItemElem;
			pMail->m_pItemElem->m_dwItemId = dwItemId;
			GetItemFromMail(&m_qryPostProc, pMail->m_pItemElem);

			if (CheckValidItem(dwItemId, pMail->m_pItemElem->m_nItemNum) == FALSE)
			{
#ifdef __POST_DUP_1209
				nTotal++;
#endif // __POST_DUP_1209
				SAFE_DELETE(pMail);
				continue;
			}
		}
#ifdef __POST_DUP_1209
		bool bResult = setnMail.insert(pMail->m_nMail).second;
		if (!bResult)
		{
			AfxMessageBox("CDbManager.LoadPost: duplicated");
			return FALSE;
		}
#endif	// __POST_DUP_1209

		if (FALSE == CPost::GetInstance()->AddMail(idReceiver, pMail))
		{
			AfxMessageBox("ERROR: LoadPost: AddMail");
			return FALSE;
		}
#ifdef __POST_DUP_1209
		nTotal++;
#endif	// __POST_DUP_1209
	}

	sprintf(szQuery, "uspLoadMaxMailID @pserverindex='%02d'", g_appInfo.dwSys);
	if (FALSE == m_qryPostProc.Exec(szQuery))
	{
		AfxMessageBox("QUERY: uspLoadMaxMailID");
		return FALSE;
	}
	if (m_qryPostProc.Fetch())
	{
		int nMaxMailID = m_qryPostProc.GetInt("MaxMailID");
#ifdef __POST_DUP_1209
		int nCount = m_qryPostProc.GetInt("nCount");
		if (nTotal != nCount)
		{
			char szTemp[255] = { 0, };
			sprintf(szTemp, "CDbManager.LoadPost: count -> count : %d, nTotal : %d", nCount, nTotal);
			AfxMessageBox(szTemp);
			return FALSE;
		}
#endif	// __POST_DUP_1209
		if ((int)(CMail::s_nMail) > nMaxMailID)
		{
			AfxMessageBox("MaxMailID is not valid");
			return FALSE;
		}
		CMail::s_nMail = nMaxMailID;
	}
	return TRUE;
}

void CDbManager::GetItemFromMail(CQuery* pQuery, CItemElem* pItemElem)
{
	pItemElem->m_bItemResist = pQuery->GetInt("bItemResist");
	pItemElem->m_byFlag = pQuery->GetInt("byFlag");
	pItemElem->m_dwKeepTime = pQuery->GetInt("dwKeepTime");
	pItemElem->SetSerialNumber(pQuery->GetSerialNumber("dwSerialNumber"));
	pItemElem->SetOption(pQuery->GetInt("nOption"));
	pItemElem->m_nHitPoint = pQuery->GetInt("nHitPoint");
	pItemElem->m_nItemNum = pQuery->GetInt("nItemNum");

	__int64 iRandomOptItemId = pQuery->GetInt64("nRandomOptItemId");
	if (iRandomOptItemId == -102)
		iRandomOptItemId = 0;
	pItemElem->SetRandomOptItemId(iRandomOptItemId);

	pItemElem->m_nResistAbilityOption = pQuery->GetInt("nResistAbilityOption");
	GetPiercingInfoFromMail(pQuery, pItemElem);

	BOOL bPet = pQuery->GetInt("bPet");
	if (bPet)
	{
		SAFE_DELETE(pItemElem->m_pPet);
		CPet* pPet = pItemElem->m_pPet = new CPet;
		BYTE nKind = (BYTE)pQuery->GetInt("nKind");
		BYTE nLevel = (BYTE)pQuery->GetInt("nLevel");
		DWORD dwExp = (DWORD)pQuery->GetInt("dwExp");
		WORD wEnergy = (WORD)pQuery->GetInt("wEnergy");
		WORD wLife = (WORD)pQuery->GetInt("wLife");
		BYTE anAvailLevel[PL_MAX];
		anAvailLevel[PL_D] = (BYTE)pQuery->GetInt("anAvailLevel_D");
		anAvailLevel[PL_C] = (BYTE)pQuery->GetInt("anAvailLevel_C");
		anAvailLevel[PL_B] = (BYTE)pQuery->GetInt("anAvailLevel_B");
		anAvailLevel[PL_A] = (BYTE)pQuery->GetInt("anAvailLevel_A");
		anAvailLevel[PL_S] = (BYTE)pQuery->GetInt("anAvailLevel_S");
#ifdef __PET_1024
		char szPetName[MAX_PET_NAME] = { 0, };
		pQuery->GetStr("szPetName", szPetName);
		if (strcmp(szPetName, "NULL") == 0)
			szPetName[0] = '\0';
#endif	// __PET_1024
		pPet->SetKind(nKind);
		pPet->SetLevel(nLevel);
		pPet->SetExp(dwExp);
		pPet->SetEnergy(wEnergy);
		pPet->SetLife(wLife);
		pPet->SetAvailLevel(PL_D, anAvailLevel[PL_D]);
		pPet->SetAvailLevel(PL_C, anAvailLevel[PL_C]);
		pPet->SetAvailLevel(PL_B, anAvailLevel[PL_B]);
		pPet->SetAvailLevel(PL_A, anAvailLevel[PL_A]);
		pPet->SetAvailLevel(PL_S, anAvailLevel[PL_S]);
#ifdef __PET_1024
		pPet->SetName(szPetName);
#endif	// __PET_1024
	}

#ifdef __FL_FIX_MAILVALUES
	pItemElem->m_bTranformVisPet = pQuery->GetInt("bTransformPet");

	if (pItemElem->IsVisPet())
	{
		pItemElem->SetVisKeepTimeSize(pItemElem->GetPiercingSize());
		for (int i = 0; i < pItemElem->GetPiercingSize(); i++)
		{
			char szItem[32];
			sprintf(szItem, "tmPetVis%d", i + 1);
			pItemElem->SetVisKeepTime(i, (time_t)pQuery->GetInt(szItem));
		}
	}

#ifdef __MODEL_CHANGE
	DWORD dwItemModel = (DWORD)pQuery->GetInt("nItemModel");
	if (dwItemModel <= 0)
		dwItemModel = NULL_ID;
	pItemElem->SetLook(dwItemModel);
#endif
#endif

#ifdef __FL_PACKED_ITEMS
	if (pItemElem->IsPackage())
	{
		CItemContainer<CItemElem> packageContainer;
		packageContainer.SetItemContainer(ITYPE_ITEM, MAX_ITEMS_IN_PACKAGE);

		GetOnePackage(pQuery, &packageContainer, pItemElem);
	}
#endif
}

void CDbManager::AddMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	CAr ar(pov->lpBuf, pov->uBufSize);

	u_long idReceiver = 0;
	u_long idSender = 0;
	BYTE byItem = 0;
	CItemElem kItemElem;
	int nGold = 0;
	TCHAR szMailTitle[MAX_MAILTITLE] = { 0, };
	TCHAR szMailText[MAX_MAILTEXT] = { 0, };

	ar >> idReceiver >> idSender >> byItem;

	if (byItem)
		kItemElem.Serialize(ar);

	ar >> nGold;
	ar.ReadString(szMailTitle, _countof(szMailTitle));
	ar.ReadString(szMailText, _countof(szMailText));

	PostMail(*pQuery, idReceiver, idSender, kItemElem, nGold, szMailTitle, szMailText);
}

void	CDbManager::PostMail(CQuery& kQuery, const u_long uReceiver, const u_long uSender, CItemElem& kItemElem, const int nGold, const char* szTitle, const char* szText)
{
	CMail* pMail = NULL;

	try
	{
		pMail = new CMail;
		pMail->m_tmCreate = time(NULL);
		pMail->m_idSender = uSender;
		if (kItemElem.IsEmpty() == FALSE)
		{
			pMail->m_pItemElem = new CItemElem;
			*pMail->m_pItemElem = kItemElem;
		}
		pMail->m_nGold = nGold;
		strcpy(pMail->m_szTitle, szTitle);
		strcpy(pMail->m_szText, szText);

		CPost* pPost = CPost::GetInstance();
		pPost->m_csPost.Enter();
		BOOL bResult = CPost::GetInstance()->AddMail(uReceiver, pMail) > 0;
		pPost->m_csPost.Leave();

		if (bResult)
		{
			char szQuery[QUERY_SIZE] = { 0, };
			CDbManager::MakeQueryAddMail(szQuery, pMail, uReceiver);

			SQLINTEGER cbLen = SQL_NTS;

			if (kQuery.BindParameter(1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 512, 0, (void*)pMail->m_szTitle, 0, &cbLen) == FALSE
				|| kQuery.BindParameter(2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 1024, 0, (void*)pMail->m_szText, 0, &cbLen) == FALSE)
			{
				Error("CDbManager::PostMail : %s - title, %s - text", pMail->m_szTitle, pMail->m_szText);
				CDPTrans::GetInstance()->SendPostMail(FALSE, uReceiver, pMail);
				pPost->m_csPost.Enter();
				CMailBox* pMailBox = pPost->GetMailBox(uReceiver);
				pMailBox->RemoveMail(pMail->m_nMail);
				pPost->m_csPost.Leave();
				return;
			}

			if (FALSE == kQuery.Exec(szQuery))
			{
				Error("CDbManager::PostMail : %s - szQuery", szQuery);
				CDPTrans::GetInstance()->SendPostMail(FALSE, uReceiver, pMail);
				pPost->m_csPost.Enter();
				CMailBox* pMailBox = pPost->GetMailBox(uReceiver);
				pMailBox->RemoveMail(pMail->m_nMail);
				pPost->m_csPost.Leave();
				return;
			}
		}
		else
		{
			Error("AddMail false. idSender : %d, pMail->m_nMail : %d", pMail->m_idSender, pMail->m_nMail);
			CDPTrans::GetInstance()->SendPostMail(FALSE, uReceiver, pMail);
			pPost->m_csPost.Enter();
			CMailBox* pMailBox = pPost->GetMailBox(uReceiver);
			pMailBox->RemoveMail(pMail->m_nMail);
			pPost->m_csPost.Leave();
			return;
		}

		CDPTrans::GetInstance()->SendPostMail(TRUE, uReceiver, pMail);
	}
	catch (...)
	{
		Error("[ CMail Alloc Failed. Sender(%07d), Receiver(%07d), ItemID(%d), ItemNum(%d), Gold(%d), Title(%s), Text(%s) ]"
			, uSender, uReceiver, kItemElem.m_dwItemId, kItemElem.m_nItemNum, nGold, szTitle, szText);
		SAFE_DELETE(pMail);
	}
}

void CDbManager::RemoveMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	CAr ar(pov->lpBuf, pov->uBufSize);

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost* pPost = CPost::GetInstance();
	pPost->m_csPost.Enter();

	CMailBox* pMailBox = pPost->GetMailBox(idReceiver);
	if (pMailBox)
	{
		pMailBox->RemoveMail(nMail);
		pPost->m_csPost.Leave();
		MAIL_QUERYINFO info("D1");
		info.nMail = nMail;
		char szQuery[QUERY_SIZE] = { 0, };
		DbQryMail(szQuery, info);
		if (FALSE == pQuery->Exec(szQuery))
			Error("QUERY: PACKETTYPE_QUERYREMOVEMAIL");
		else
			CDPTrans::GetInstance()->SendRemoveMail(idReceiver, nMail);
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::RemoveMail(list<CMail*>& lspMail)
{
	char szQuery[QUERY_SIZE] = { 0, };
	for (list<CMail*>::iterator i = lspMail.begin(); i != lspMail.end(); ++i)
	{
		CMail* pMail = *i;
		MAIL_QUERYINFO info("D1");
		info.nMail = pMail->m_nMail;
		DbQryMail(szQuery, info);
		if (m_qryPostProc.Exec(szQuery))
		{
			CMailBox* pMailBox = pMail->GetMailBox();
			pMailBox->RemoveMail(info.nMail);
			CDPTrans::GetInstance()->SendRemoveMail(pMailBox->m_idReceiver, info.nMail);
		}
	}
}

void CDbManager::RemoveMailItem(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	CAr ar(pov->lpBuf, pov->uBufSize);

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
	//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox = pPost->GetMailBox(idReceiver);
	if (pMailBox)
	{
		pMailBox->RemoveMailItem(nMail);
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info("U1");
		info.nMail = nMail;
		char szQuery[QUERY_SIZE] = { 0, };
		DbQryMail(szQuery, info);
		if (FALSE == pQuery->Exec(szQuery))
			Error("QUERY: PACKETTYPE_QUERYGETMAILITEM");
		else
			CDPTrans::GetInstance()->SendGetMailItem(idReceiver, nMail, uQuery);
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::RemoveMailGold(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	CAr ar(pov->lpBuf, pov->uBufSize);

	u_long idReceiver, nMail, uQuery;
	ar >> idReceiver >> nMail >> uQuery;

	CPost* pPost = CPost::GetInstance();
	//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox = pPost->GetMailBox(idReceiver);
	if (pMailBox)
	{
		pMailBox->RemoveMailGold(nMail);
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info("U2");
		info.nMail = nMail;
		char szQuery[QUERY_SIZE] = { 0, };
		DbQryMail(szQuery, info);
		if (FALSE == pQuery->Exec(szQuery))
			Error("QUERY: PACKETTYPE_QUERYGETMAILITEM");
		else
			CDPTrans::GetInstance()->SendGetMailGold(idReceiver, nMail, uQuery);
	}
	else
		pPost->m_csPost.Leave();	// u
}

void CDbManager::ReadMail(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	CAr ar(pov->lpBuf, pov->uBufSize);

	u_long idReceiver, nMail;
	ar >> idReceiver >> nMail;

	CPost* pPost = CPost::GetInstance();
	//	CMclAutoLock	Lock( pPost->m_csPost );
	pPost->m_csPost.Enter();	// l

	CMailBox* pMailBox = pPost->GetMailBox(idReceiver);
	if (pMailBox)
	{
		pMailBox->ReadMail(nMail);
		pPost->m_csPost.Leave();	// u
		MAIL_QUERYINFO info("U3");
		info.nMail = nMail;
		char szQuery[QUERY_SIZE] = { 0, };
		DbQryMail(szQuery, info);
		if (FALSE == pQuery->Exec(szQuery))
			Error("QUERY: PACKETTYPE_READMAIL");
		else
			CDPTrans::GetInstance()->SendReadMail(idReceiver, nMail);
	}
	else
	{
		pPost->m_csPost.Leave();	// u
		Error("CDbManager::ReadMail - pMailBox is NULL. idReceiver : %07d", idReceiver);
	}
}

int	CDbManager::GetTradeNo(void)
{
	time_t	t = time(NULL);
	t &= 0xffffffe0;
	LONG lCount = InterlockedIncrement(&m_cbTrade);
	lCount &= 0x0000001f;
	return (int)(t + lCount);
}

#ifdef __GETMAILREALTIME
BOOL CDbManager::QueryGetMailRealTime(CQuery* pQuery)
{
	if (NULL == pQuery)
		return FALSE;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "MAIL_STR_REALTIME 'S1', '%02d'", g_appInfo.dwSys);
	if (FALSE == pQuery->Exec(szQuery))
	{
		AfxMessageBox(szQuery);
		return FALSE;
	}

	__MAIL_REALTIME OneMail;
	vector< __MAIL_REALTIME > vecMailRT;

	while (pQuery->Fetch())
	{
		u_long idReceiver = pQuery->GetInt("idReceiver");
		CMail* pMail = new CMail;
		OneMail.nMail_Before = pQuery->GetInt("nMail");
		pMail->m_idSender = pQuery->GetInt("idSender");
		int nGoldFlag = pQuery->GetInt("GoldFlag");
		pMail->m_nGold = 0;
		if (nGoldFlag == 0)
			pMail->m_nGold = pQuery->GetInt("nGold");
		pMail->m_tmCreate = (time_t)pQuery->GetInt("tmCreate");
		pMail->m_byRead = (BYTE)pQuery->GetInt("byRead");

		//	mulcom	BEGIN100420
		//pQuery->GetStr( "szTitle", pMail->m_szTitle );
		char	szTempTitle[128];
		::memset(szTempTitle, 0, sizeof(szTempTitle));
		pQuery->GetStr("szTitle", szTempTitle);

		::memcpy(pMail->m_szTitle, szTempTitle, sizeof(pMail->m_szTitle));
		pMail->m_szTitle[MAX_MAILTITLE - 1] = '\0';
		//	mulcom	END100420

		pQuery->GetStr("szText", pMail->m_szText);

		int nItemFlag = pQuery->GetInt("ItemFlag");
		DWORD dwItemId = pQuery->GetInt("dwItemId");

		if (dwItemId && nItemFlag == 0)
		{
			ItemProp* pItemProp = prj.GetItemProp(dwItemId);
			if (!pItemProp)
			{
				Error("CDbManager::QueryGetMailRealTime: Not ItemProp = %d", dwItemId);
				continue;
			}
			pMail->m_pItemElem = new CItemElem;
			pMail->m_pItemElem->m_dwItemId = dwItemId;
			GetItemFromMail(pQuery, pMail->m_pItemElem);

			if (CheckValidItem(dwItemId, pMail->m_pItemElem->m_nItemNum) == FALSE)
			{
				SAFE_DELETE(pMail);
				continue;
			}

			CPost* pPost = CPost::GetInstance();

			pPost->m_csPost.Enter();
			OneMail.nMail_After = CPost::GetInstance()->AddMail(idReceiver, pMail);

			pPost->m_csPost.Leave();
			CDPTrans::GetInstance()->SendPostMail(TRUE, idReceiver, pMail);

			vecMailRT.push_back(OneMail);
		}
	}

	for (DWORD i = 0; i < vecMailRT.size(); ++i)
	{
		sprintf(szQuery, "MAIL_STR_REALTIME 'U1', '%02d', %d, %d, '%07d', %d, '%s', %d, %d, %d",
			g_appInfo.dwSys, vecMailRT[i].nMail_Before, vecMailRT[i].nMail_After, 0, 0, "", 0, vecMailRT[i].m_liSerialNumber, vecMailRT[i].m_nHitPoint);

		if (FALSE == pQuery->Exec(szQuery))
		{
			AfxMessageBox(szQuery);
			return FALSE;
		}
	}

	return TRUE;
}
#endif // __GETMAILREALTIME

BOOL CDbManager::QueryRemoveGuildBankTbl(void)
{
	if (NULL == m_apQuery[0])
		return FALSE;

	char pszQuery[100] = { 0, };
	sprintf(pszQuery, "uspLoadRemoveItemFromGuildBank @pserverindex = '%02d'", g_appInfo.dwSys);
	if (FALSE == m_apQuery[0]->Exec(pszQuery))
	{
		Error("query: %s", pszQuery);
		return FALSE;
	}
	while (m_apQuery[0]->Fetch())
	{
		int nNo = (int)m_apQuery[0]->GetInt("nNum");
		u_long idGuild = (u_long)m_apQuery[0]->GetInt("idGuild");

		char pszItemIndex[100] = { 0, };
		m_apQuery[0]->GetStr("ItemIndex", pszItemIndex);
		DWORD dwItemId = 0;
		if (lstrcmp(pszItemIndex, "penya"))	// item
			dwItemId = atoi(pszItemIndex);
		SERIALNUMBER iSerialNumber = (SERIALNUMBER)m_apQuery[0]->GetSerialNumber("ItemSerialNum");
		DWORD dwItemNum = (DWORD)m_apQuery[0]->GetInt("DeleteRequestCnt");
		CDPTrans::GetInstance()->SendQueryRemoveGuildBankTbl(nNo, idGuild, dwItemId, iSerialNumber, dwItemNum);
#ifdef _DEBUG
		char lpOutputString[200];
		sprintf(lpOutputString, "TRANS: QueryRemoveGuildBankTbl: nNo = %d, idGuild=%d, dwItemId=%d, liSerialNumber=%d, nItemNum=%d", nNo, idGuild, dwItemId, iSerialNumber, dwItemNum);
		OutputDebugString(lpOutputString);
#endif	// _DEBUG
	}
	return TRUE;
}

void CDbManager::DeleteRemoveGuildBankTbl(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpov)
{
	int nNo;
	DWORD dwRemoved;
	CAr ar(lpov->lpBuf, lpov->uBufSize);
	ar >> nNo >> dwRemoved;
	/*
		uspRemoveItemFromGuildBank:
			Parameter :
				@pserverindex char(2)
				@pNum int
			Return :
				0 : not exist
				1 : success
	*/
	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspRemoveItemFromGuildBank @pserverindex = '%02d', @pNum = %d, @pDeleteCnt=%d", g_appInfo.dwSys, nNo, dwRemoved);
#ifdef _DEBUG
	OutputDebugString(szQuery);
#endif	// _DEBUG
	if (FALSE == pQueryChar->Exec(szQuery))
		Error("%s", szQuery);
}

void CDbManager::call_uspLoggingQuest(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	u_long idPlayer;
	int nQuest, nState;
	char pszComment[100] = { 0, };
	CAr ar(pov->lpBuf, pov->uBufSize);
	ar >> idPlayer >> nQuest >> nState;
	ar.ReadString(pszComment, 100);
	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspLoggingQuest @pserverindex='%02d', @pidPlayer='%07d', @pQuestIndex=%d, @pState=%d, @pComment='%s'", g_appInfo.dwSys, idPlayer, nQuest, nState, pszComment);
	if (FALSE == pQuery->Exec(szQuery))
		Error("%s", szQuery);
}

void CDbManager::call_uspXXXMultiServer(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	u_long uKey, idPlayer;
	CAr ar(pov->lpBuf, pov->uBufSize);
	ar >> uKey >> idPlayer;

	char szQuery[QUERY_SIZE] = { 0, };

	if (uKey > 0)
	{
		int i = (int)uKey;// / 100;
		if (idPlayer > 0)	//			uspChangeMultiServer @pserverindex(char), @pidPlayer (char), @pMultiServer(int)
			sprintf(szQuery, "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, i);
		else	// uspInitMultiServer @pserverindex(char), @pMultiServer(int)
			sprintf(szQuery, "uspInitMultiServer @pserverindex='%02d', @pMultiServer=%d", g_appInfo.dwSys, i);
	}
	else
	{
		if (idPlayer > 0)	// o
			sprintf(szQuery, "uspChangeMultiServer @pserverindex='%02d', @pidPlayer='%07d', @pMultiServer=%d", g_appInfo.dwSys, idPlayer, 0);
	}
	if (FALSE == pQuery->Exec(szQuery))
		Error("%s", szQuery);
}

DB_OVERLAPPED_PLUS* CDbManager::AllocRequest(void)
{
	DB_OVERLAPPED_PLUS* ptr = m_pDbIOData->Alloc();
	memset(ptr, 0, sizeof(*ptr));
	return ptr;
}
void	CDbManager::FreeRequest_V21(DB_OVERLAPPED_PLUS* pOverlappedPlus)
{
	SAFE_DELETE_ARRAY(pOverlappedPlus->lpBuf);
	m_pDbIOData->Free(pOverlappedPlus);
}

void	CDbManager::MakeRequest(DB_OVERLAPPED_PLUS* pOverlappedPlus, LPBYTE lpBuf, u_long uBufSize)
{
	if (uBufSize < 0 || uBufSize >= (1024 * 1024))
	{
		Error(_T("MakeRequest uBufSize Error. [%d]"), uBufSize);
		uBufSize = 0;
	}

	pOverlappedPlus->lpBuf = new BYTE[uBufSize];
	::memset(pOverlappedPlus->lpBuf, 0, uBufSize);

	memcpy(pOverlappedPlus->lpBuf, lpBuf, uBufSize);
	pOverlappedPlus->uBufSize = uBufSize;

	return;
}

void	CDbManager::LoadMessenger(CMover* pMover, CQuery* pQuery)
{
	pMover->m_RTMessenger.clear();
	char szSQL[100] = { 0, };
	sprintf(szSQL, "uspLoadMessenger '%02d', '%07d'", g_appInfo.dwSys, pMover->m_idPlayer);
	if (!pQuery->Exec(szSQL))
		return;
	u_long idFriend;
	Friend f;
	int nSize = 0;
	while (pQuery->Fetch())
	{
		idFriend = (u_long)pQuery->GetInt("idFriend");
		f.bBlock = (DWORD)pQuery->GetInt("bBlock");
		if (++nSize > MAX_FRIEND)
		{
			Error("TOO MANY FRIEND: %07d", pMover->m_idPlayer);
			break;
		}
		pMover->m_RTMessenger.SetFriend(idFriend, &f);
	}
}

void CDbManager::AddMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	u_long idPlayer, idFriend;
	CAr ar(pov->lpBuf, pov->uBufSize);
	ar >> idPlayer >> idFriend;

	char szSQL[100] = { 0, };
	sprintf(szSQL, "uspAddMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend);
	if (!pQuery->Exec(szSQL))
	{
		//
	}
}

void CDbManager::DeleteMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	u_long idPlayer, idFriend;
	CAr ar(pov->lpBuf, pov->uBufSize);
	ar >> idPlayer >> idFriend;

	char szSQL[100] = { 0, };
	sprintf(szSQL, "uspDeleteMessenger '%02d', '%07d', '%07d'", g_appInfo.dwSys, idPlayer, idFriend);
	if (!pQuery->Exec(szSQL))
	{
		// error
	}
}

void CDbManager::UpdateMessenger(CQuery* pQuery, LPDB_OVERLAPPED_PLUS pov)
{
	u_long idPlayer, idFriend;
	BOOL bBlock;
	CAr ar(pov->lpBuf, pov->uBufSize);
	ar >> idPlayer >> idFriend >> bBlock;

	char szSQL[100] = { 0, };
	sprintf(szSQL, "uspUpdateMessenger '%02d', '%07d', '%07d', %d", g_appInfo.dwSys, idPlayer, idFriend, bBlock);
	if (!pQuery->Exec(szSQL))
	{
		// error
	}
}

#if __VER >= 11
void CDbManager::GuildBankLogView(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	BYTE	byListType;
	u_long	idReceiver, g_uIdofMulti;
	DWORD	idGuild;

	ar >> idReceiver;
	ar >> idGuild;
	ar >> byListType;
	ar >> g_uIdofMulti;

	DPID	dpidMulti = lpDbOverlappedPlus->dpid;


	char szQuery[QUERY_SIZE] = { 0, };
	switch (byListType)
	{
	case GI_LOG_VIEW_ITEM_ADD:
		sprintf(szQuery, "LOGGING_01_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
			"S1", idGuild, g_appInfo.dwSys);
		break;
	case GI_LOG_VIEW_ITEM_REMOVE:
		sprintf(szQuery, "LOGGING_01_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
			"S2", idGuild, g_appInfo.dwSys);
		break;
	case GI_LOG_VIEW_MONEY_ADD:
		sprintf(szQuery, "LOGGING_01_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
			"S3", idGuild, g_appInfo.dwSys);
		break;
	case GI_LOG_VIEW_MONEY_REMOVE:
		sprintf(szQuery, "LOGGING_01_DBF.dbo.usp_guildbank_log_view '%s','%06d','%02d'",
			"S4", idGuild, g_appInfo.dwSys);
		break;
	}
	if (pQuery->Exec(szQuery) == FALSE)
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
	}
	else
	{
		//		BEFORESENDSOLE( out, PACKETTYPE_GUILDLOG_VIEW, dpidMulti);
		BEFORESENDDUAL(out, PACKETTYPE_GUILDLOG_VIEW, DPID_UNKNOWN, DPID_UNKNOWN);
		int		nCount = 0, m_Item = 0, Item_count = 0, nAbilityOption = 0;
		CTime	date;

		u_long	idPlayer;

		u_long uOffset = out.GetOffset();
		out << nCount;
		out << byListType;
		out << idReceiver;

		char szDate[13] = { 0, };
		while (pQuery->Fetch())
		{
			idPlayer = pQuery->GetInt("m_idPlayer");
			char szSender[MAX_PLAYER] = "unknown";
			CPlayerDataCenter::GetInstance()->m_Access.Enter();
			const char* pszPlayer = CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer);
			if (pszPlayer)
				lstrcpy(szSender, pszPlayer);
			CPlayerDataCenter::GetInstance()->m_Access.Leave();

			pQuery->GetStr("s_date", szDate);
			m_Item = pQuery->GetInt("m_Item");
			nAbilityOption = pQuery->GetInt("m_nAbilityOption");
			Item_count = pQuery->GetInt("Item_count");

			GetStrTime(&date, szDate);

			out.WriteString(szSender);
			out << (long)date.GetTime();
			out << m_Item;
			out << nAbilityOption;
			out << Item_count;

			nCount++;
		}

		int nBufferSize;
		BYTE* pBuffer = out.GetBuffer(&nBufferSize);
		*(UNALIGNED int*)(pBuffer + uOffset) = nCount;
		SEND(out, CDPTrans::GetInstance(), dpidMulti);
	}


}
#endif

#ifdef __FL_GW_STATUE_V2
void CDbManager::InstallStatues(CQuery* pQueryChar, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);
	DPID dpidMulti = lpDbOverlappedPlus->dpid;

	u_long idMvp, idLeader;
	ar >> idMvp >> idLeader;

	if (idMvp == NULL || idMvp == NULL_ID || idLeader == NULL || idLeader == NULL_ID)
		return;

	BEFORESENDDUAL(out, PACKETTYPE_REQUEST_INSTALL_STATUES, DPID_UNKNOWN, DPID_UNKNOWN);
	u_long uOffset = out.GetOffset();
	int nCount = 0;
	out << nCount;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspLoadStatuesParameters '%07d', '%07d', '%02d'", idMvp, idLeader, g_appInfo.dwSys);
	if (!pQueryChar->Exec(szQuery))
	{
		Error("CDbManager::uspLoadStatuesParameters[Line:%u] Failed, Query: %s", __LINE__, szQuery);
		return;
	}

	while (pQueryChar->Fetch())
	{
		DWORD dwidPlayer = (DWORD)pQueryChar->GetInt("m_idPlayer");
		char        szName[64];
		pQueryChar->GetStr("m_szName", szName);
		DWORD dwJob = (DWORD)pQueryChar->GetInt("m_nJob");
		DWORD dwLevel = (DWORD)pQueryChar->GetInt("m_nLevel");
		DWORD dwSexIndex = (DWORD)pQueryChar->GetInt("m_dwSex");
		DWORD dwHairId = (DWORD)pQueryChar->GetInt("m_dwHairMesh");
		DWORD dwHairColorId = (DWORD)pQueryChar->GetInt("m_dwHairColor");
		DWORD dwHeadId = (DWORD)pQueryChar->GetInt("m_dwHeadMesh");

		CItemContainer< CItemElem > tmpInventoryOwner;
		tmpInventoryOwner.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);

		ItemContainerStruct	ics;
		sprintf(ics.szItem, "m_Inventory");
		sprintf(ics.szObjIndex, "m_dwObjIndex");
		GetContainer(pQueryChar, &tmpInventoryOwner, &ics);

		DWORD dwWhoIsThat = 0;
		if (dwidPlayer == idMvp && dwidPlayer == idLeader)
			dwWhoIsThat = 1; // both (SQL server will back only 1 result if it is the same person)
		else if (dwidPlayer == idMvp)
			dwWhoIsThat = 2; // mvp
		else if (dwidPlayer == idLeader)
			dwWhoIsThat = 3; // leader

		out << dwWhoIsThat;
		out << dwidPlayer << dwJob << dwLevel << dwSexIndex << dwHairId << dwHairColorId << dwHeadId;
		out.WriteString(szName);
		tmpInventoryOwner.Serialize(out);

		nCount++;
	}

	int nBufferSize;
	BYTE* pBuffer = out.GetBuffer(&nBufferSize);
	*(UNALIGNED int*)(pBuffer + uOffset) = nCount;
	SEND(out, CDPTrans::GetInstance(), dpidMulti);

}
#endif

#ifdef __FL_DAILY_REWARDS
void CDbManager::UpdateDailyGift(CQuery* qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr arRead(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	int nValue;
	u_long uidPlayer;
	arRead >> uidPlayer;
	arRead >> nValue;

	CTime CurrentTime = CTime::GetCurrentTime();
	CString str = CurrentTime.Format("%Y%m%d");

	char timebuf[9] = { 0, };
	strncpy(timebuf, (LPCTSTR)str, 8);

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "uspUpdateDailyGift '%07d', '%s', %d, '%02d'", uidPlayer, timebuf, nValue, g_appInfo.dwSys);

	if (FALSE == qry->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);

		return;
	}

}
#endif

#ifdef __FL_CASH_SHOP 
BOOL	CDbManager::LoadCShop()
{
	CQuery	query;
	if (query.Connect(3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, DB_ADMIN_PASS_CHARACTER01) == FALSE)
	{
		Error("LoadFlyFFShop: can't connect character db");
		AfxMessageBox("can't connect character db");
		return FALSE;
	}

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "SELECT * FROM dbo.tblCashShopItems");

	if (!query.Exec(szQuery))
	{
		Error("LoadFlyFFShop - not loaded");
		return FALSE;
	}

	CFlyFFShop::GetInstance()->GetItemVector()->clear();

	FLYFF_SHOP_ITEMS s;
	while (query.Fetch())
	{
		s.nItemIndex = query.GetInt("id");
		s.dwItemID = (DWORD)query.GetInt("item_id");
		s.nPurchaseMethod = query.GetInt("item_voteordonateorboth");
		s.nVotePrice = query.GetInt("item_voteprice");
		s.nDonatePrice = query.GetInt("item_donateprice");
		s.nCategory = query.GetInt("item_category");
		s.nAmount = query.GetInt("item_amount");

		CFlyFFShop::GetInstance()->GetItemVector()->push_back(s);
	}

	return TRUE;
}
void CDbManager::GetCShopBalance(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idPlayer;
	TCHAR szAccount[MAX_ACCOUNT] = { 0, };

	ar >> idPlayer;
	ar.ReadString(szAccount, _countof(szAccount));

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "SELECT * FROM ACCOUNT_DBF.dbo.ACCOUNT_TBL WHERE account = '%s'", szAccount);

	if (!pQuery->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	if (pQuery->Fetch())
	{
		int nDonate = pQuery->GetInt("cash");
		int nVote = pQuery->GetInt("votepoint");

		CDPTrans::GetInstance()->SendCShopBalance(lpDbOverlappedPlus->dpid, idPlayer, nDonate, nVote, FALSE);
	}
}
void CDbManager::BuyCShopItem(CQuery* pQuery, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus)
{
	CAr ar(lpDbOverlappedPlus->lpBuf, lpDbOverlappedPlus->uBufSize);

	u_long idPlayer;
	TCHAR szAccount[MAX_ACCOUNT] = { 0, };
	DWORD dwItemID;
	int nItemNum;
	int nTotalPrice;
	BOOL bDonate;
	u_long idReceiver;

	ar >> idPlayer;
	ar.ReadString(szAccount, _countof(szAccount));
	ar >> dwItemID >> nItemNum >> nTotalPrice >> bDonate;
	ar >> idReceiver;

	char szQuery[QUERY_SIZE] = { 0, };
	sprintf(szQuery, "SELECT * FROM ACCOUNT_DBF.dbo.ACCOUNT_TBL WHERE account = '%s'", szAccount);

	if (!pQuery->Exec(szQuery))
	{
		WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
		return;
	}

	if (pQuery->Fetch())
	{
		int nCurrentDonate = pQuery->GetInt("cash");
		int nCurrentVote = pQuery->GetInt("votepoint");

		int nNewDonate = nCurrentDonate;
		int nNewVote = nCurrentVote;

		BOOL bNotEnoughMoney = FALSE;
		if (bDonate)
		{
			nNewDonate = nCurrentDonate - nTotalPrice;
			if (nNewDonate < 0)
				bNotEnoughMoney = TRUE;
		}
		else
		{
			nNewVote = nCurrentVote - nTotalPrice;
			if (nNewVote < 0)
				bNotEnoughMoney = TRUE;
		}

		if (bNotEnoughMoney)
		{
			CDPTrans::GetInstance()->SendCShopBalance(lpDbOverlappedPlus->dpid, idPlayer, nCurrentDonate, nCurrentVote, TRUE);
			return;
		}

		sprintf(szQuery, "UPDATE ACCOUNT_DBF.dbo.ACCOUNT_TBL SET cash = %d, votepoint = %d WHERE account = '%s'", nNewDonate, nNewVote, szAccount);
		if (!pQuery->Exec(szQuery))
		{
			WriteLog("%s, %d\t%s", __FILE__, __LINE__, szQuery);
			return;
		}

		CDPTrans::GetInstance()->SendCShopItem(lpDbOverlappedPlus->dpid, idPlayer, idReceiver, nNewDonate, nNewVote, dwItemID, nItemNum);
	}
}
#endif