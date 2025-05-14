#include "stdafx.h"
#include "defineText.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "dpcoreclient.h"
#include "DPSrvr.h"
#include "WorldMng.h"
#include "..\_Common\Ship.h"
#include "defineobj.h"
#ifdef __WORLDSERVER
#include "..\_aiinterface\AIPet.h"
#ifdef __SYS_TICKET
#include "ticket.h"
#endif	// __SYS_TICKET
#endif

#ifdef __FL_CASH_SHOP
#include "FlyFFShop.h"
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 11 // __SYS_COLLECTING
#include "collecting.h"
#include "definesound.h"
#include "defineitem.h"
#endif	// __SYS_COLLECTING

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __TAX
#include "Tax.h"
#endif // __TAX

#if __VER >= 12 // __LORD
#include "slord.h"
#include "lordskillexecutable.h"
#endif	// __LORD

#if __VER >= 15 // __PETVIS
#include "ItemUpgrade.h"
#endif // __PETVIS

#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif

extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;

#include "Environment.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#include "Chatting.h"
extern	CChattingMng	g_ChattingMng;

#include "Party.h"
extern	CPartyMng			g_PartyMng;
extern CGuildCombat g_GuildCombatMng;

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#include "UserMacro.h"
#include "post.h"

#if __VER >= 13 // __COUPLE_1117
#include "couplehelper.h"
#endif	// __COUPLE_1117

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
extern	CDPSrvr		g_DPSrvr;
extern	CWorldMng	g_WorldMng;
extern	CUserMng	g_UserMng;
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __INTERNALSERVER
#ifdef __VM_0819
CUserPool* CUser::m_pPool = new CUserPool(5, "CUser");
#else	// __VM_0819
CUserPool* CUser::m_pPool = new CUserPool(5);
#endif	// __VM_0819
#else	// __INTERNALSERVER
#ifdef __VM_0819
CUserPool* CUser::m_pPool = new CUserPool(512, "CUser");
#else	// __VM_0819
CUserPool* CUser::m_pPool = new CUserPool(512);
#endif	// __VM_0819
#endif	// __INTERNALSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820

CUserMng	g_UserMng;

CUser::CUser()
#ifdef __FL_EQUALIZED_MODE
	: m_buffsTmp(this)
#endif
{
	Init();
}

CUser::CUser(DPID dpidCache, DPID dpidUser)
#ifdef __FL_EQUALIZED_MODE
	: m_buffsTmp(this)
#endif
{
	Init(dpidCache, dpidUser);
}

CUser::~CUser()
{
	RemoveItFromView2();	// npc
}

BOOL CUser::IsValid()
{
	if (IsDelete())
		return FALSE;
	else
		return m_bValid;
}

void CUser::Init(DPID dpidCache, DPID dpidUser)
{
	CMover::m_bPlayer = TRUE;

#ifdef __FL_SWITCH_EQ
	m_dwNextSwitch = g_tmCurrent;
#endif

#ifdef __FL_DAILY_REWARDS
	m_nLastDailyReward = -1;
#endif

#ifdef __FL_EQUALIZED_MODE
	bTmpMode = FALSE;
#endif

#ifdef __FL_PET_FILTER
	m_nPetLootType = 0;
	m_nLootPlace = TYPE_INVENTORY;
#endif

#ifdef __AIO_PETS
	SetSavedEatPetId(NULL_ID);
#endif

#ifdef __FL_DUNGEON_TREASURE
	SetTreasureOpenState(FALSE);
#endif

#ifdef __FL_FIX_BLADEHACK
	SetLastAnimation(0);
#endif

#ifdef __FL_UPDATE_CHATTING
	SetLastChatMessage(GetTickCount());
#endif

	m_bCheckTransMailBox = FALSE;
	m_nCountFromClient = 0;
	m_dwTickFromClient = 0;

	m_dwSerial = 0;
	m_bValid = FALSE;
	m_Snapshot.dpidCache = dpidCache;
	m_Snapshot.dpidUser = dpidUser;

	m_dwAuthKey = 0;
	m_idSetTarget = NULL_ID;
	m_nOverHeal = 0;
	*m_szPartyName = '\0';

	memset((void*)m_szBankPass, 0, sizeof(char) * 5);
	memset(&m_playAccount, 0, sizeof(PLAY_ACCOUNT));

	m_idChatting = 0;
	memset(m_pWall, 0, sizeof(m_pWall));
	m_dwLeavePenatyTime = 0;
	m_dwDestroyTime = 0;
	m_tmEscape = 0;
	m_idSnoop = 0;

	m_dwReturnWorldID = 0;
	m_vReturnPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_lpszVillage = NULL;

	m_bPosting = FALSE;
	m_bInstantBank = FALSE;

	DWORD dwTick = ::timeGetTime();
	m_dwTickSFS = dwTick + 1000;
	m_dwTickNotify = dwTick + 133;
	m_dwTimeout4Save = SEC_SAVEPLAYER;

#ifdef __S_SERVER_UNIFY
	m_bAllAction = TRUE;
#endif // __S_SERVER_UNIFY
#ifdef __QUEST_1208
	m_tickScript = GetTickCount();
#endif	// __QUEST_1208

#ifdef __EVE_MINIGAME
	m_nKawibawiboWin = 0;
	m_nKawibawiboState = 3;
#endif // __EVE_MINIGAME

#if __VER >= 11 // __SYS_COLLECTING
	m_nCollecting = 0;
#endif	// __SYS_COLLECTING

#if __VER >= 12 // __LORD
	m_idElection = NULL_ID;
	m_bQuerying = FALSE;
#endif	// __LORD

#ifdef __AZRIA_1023
	m_szInput[0] = '\0';
#endif	// __AZRIA_1023

	m_idProposer = 0;
	m_cbProcessCouple = 0;

#ifdef __EVENTLUA_KEEPCONNECT
	m_dwTickKeepConnect = GetTickCount();
#endif // __EVENTLUA_KEEPCONNECT
}

void CUser::LevelUpSetting(void)
{
	g_UserMng.AddSetLevel(this, (short)GetLevel());
	AddSetGrowthLearningPoint(m_nRemainGP);
	g_dpDBClient.SendLogLevelUp(this, 1);
	g_dpDBClient.SendUpdatePlayerData(this);

	RefreshGrilledEel();

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::HP, GetMaxHitPoint(), false);
#endif
}

void CUser::RemoveItFromView2(BOOL bRemoveall)
{
	CCtrl* pCtrl;

	map<DWORD, CCtrl*>::iterator it = m_2npc.begin();
	for (; it != m_2npc.end(); ++it)
	{
		pCtrl = it->second;
		pCtrl->PCRemoveKey(GetId());
	}
	if (bRemoveall)
		m_2npc.clear();
}

#ifdef __MAP_SECURITY
void CUser::Open(DWORD dwWorldId)
#else // __MAP_SECURITY
void CUser::Open(void)
#endif // __MAP_SECURITY
{
	AddItToGlobalId();
	PCSetAt(GetId(), (CCtrl*)this);
	m_Snapshot.SetSnapshot(GetId(), PACKETTYPE_JOIN);

	AddEnvironmentSetting();

#ifdef __MAP_SECURITY
	AddWorldReadInfo(dwWorldId, GetPos());
#endif

	AddAddObj((CCtrl*)this);
	AddGameTimer(g_GameTimer.GetCurrentTime());
	AddTaskBar();
	AddEnvironment();
	AddPlayerData();
	AddFriendGameJoin();
	AddPartyName();
	ADDGameJoin();
	AddAllGuilds();
	AddMyGuild();
	AddFlyffEvent();
	AddLord();
	AddCouple();
}

void CUser::Process()
{
	if (!IsValid())
		return;

	DWORD dwTick = g_tmCurrent;

	if (IsMode(MODE_OUTOF_PARTYQUESTRGN))
	{
		SetNotMode(MODE_OUTOF_PARTYQUESTRGN);
		D3DXVECTOR3 vPos = D3DXVECTOR3(6968.0f, 0, 3328.8f);
		REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, vPos, REPLACE_NORMAL, nDefaultLayer);
		return;
	}

	if (m_dwDestroyTime && dwTick > m_dwDestroyTime)
	{
		g_UserMng.DestroyPlayer(this);
		return;
	}

	if (IsStateMode(STATE_BASEMOTION_MODE))
	{
		if ((int)(dwTick) >= m_nReadyTime)
		{
			m_nReadyTime = 0;
			if (m_bItemFind)
			{
				CItemElem* pItemElem = m_Inventory.GetAtId(m_dwUseItemId);
				if (pItemElem)
					DoUseItem(pItemElem->m_dwObjId);
				else
				{
					AddDefinedText(TID_PK_BLINK_LIMIT, "");
					m_nReadyTime = 0;
					m_dwUseItemId = 0;
					m_bItemFind = FALSE;
					SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
				}
			}
			else
			{
				ItemProp* pItemProp = prj.GetItemProp(m_dwUseItemId);
				if (pItemProp)
					DoUseItemVirtual(m_dwUseItemId, FALSE);
			}
		}
	}

#ifdef __SYS_TICKET
	if ((m_nCount & 15) == 0)
	{
		if (GetWorld() && CTicketProperty::GetInstance()->IsTarget(GetWorld()->GetID()))
		{
			CItemElem* pTicket = GetTicket();
			if (!pTicket)
			{
				RemoveIk3Buffs(IK3_TICKET);
				REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6971.984F, 100.0F, 3336.884F), REPLACE_FORCE, nDefaultLayer);
			}
		}
		else
			RemoveIk3Buffs(IK3_TICKET);

		ProcessCouple();
	}
#endif	// __SYS_TICKET

	CheckTickCheer();

	CMover::Process();

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
	if ((m_nCount & 15) == 0)
		ProcessInvites();
#endif

	if (IsLive())
	{
		if (m_nFuel > 0)
		{
			if ((m_nCount & 15) == 0)
			{
				if (m_pActMover->IsFly() && m_pActMover->IsStateFlag(OBJSTAF_ACC))
				{
					ItemProp* pItemProp = prj.GetItemProp(m_dwRideItemIdx);
					if (pItemProp)
						AddSetFuel(m_nFuel, m_tmAccFuel);
				}
			}
		}

		if (m_nOverHeal > 0)
			--m_nOverHeal;
	}

	if (dwTick > m_dwTickNotify)
	{
		m_dwTickNotify = dwTick + 133;
		Notify();
	}

	if (dwTick > m_dwTickSFS)
	{
		m_dwTickSFS += 1000;
		CheckFiniteItem();
#ifdef __EVENTLUA_KEEPCONNECT
		prj.m_EventLua.SetKeepConnectEvent(this, dwTick);
#endif // __EVENTLUA_KEEPCONNECT

		CWorld* pWorld = GetWorld();
		if (pWorld)
		{
			SubSMMode();
			if (--m_dwTimeout4Save == 0)
			{
				CheckHonorTime();
				g_dpDBClient.SavePlayer(this, pWorld->GetID(), GetPos(), GetLayer());
				AddEventLuaDesc();
			}
		}
	}

	if (HasActivatedEatPet())
	{
		CMover* pEatPet = prj.GetMover(GetEatPetId());
		if (IsValidObj(pEatPet))
		{
#ifdef __AIO_PETS
			if (!IsValidArea(pEatPet, 60))
#else
			if (!IsValidArea(pEatPet, 32))
#endif
			{
				CAIPet* pAIPet = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
				if (pAIPet)
				{
					CItemElem* pItemElem = m_Inventory.GetAtId(pAIPet->GetPetItemId());
					InactivateEatPet();
					ActivateEatPet(pItemElem);
				}
				else
					InactivateEatPet();
			}
		}
	}
}

int	CUser::Notify(void)
{
	if (IsValid() == FALSE)
		return	0;

	if (m_dwDestroyTime)
		return 0;

	static u_int uOffset = sizeof(DPID) + sizeof(DWORD) + sizeof(OBJID) + sizeof(short);
	LPBYTE lpBuf;
	int nBufSize;

	if (m_Snapshot.cb > 0)
	{
		lpBuf = m_Snapshot.ar.GetBuffer(&nBufSize);
		*(UNALIGNED WORD*)(lpBuf + sizeof(DPID) + sizeof(DWORD) + sizeof(OBJID)) = m_Snapshot.cb;
		g_DPSrvr.Send((LPVOID)lpBuf, nBufSize, m_Snapshot.dpidCache);
		*(UNALIGNED DWORD*)(lpBuf + sizeof(DPID)) = PACKETTYPE_SNAPSHOT;
		m_Snapshot.cb = 0;
		m_Snapshot.ar.ReelIn(uOffset);
		return nBufSize;
	}
	return 0;
}

#ifdef __S_SERVER_UNIFY
void CUser::SetAllAction()
{

	CString strName = GetName();
	LPCTSTR lpszString = strName;
	char c = strName[0];
	BOOL bCharacter = FALSE;
	m_bAllAction = TRUE;
	if ((c >= '0' && c <= '9') && !IsMultiByte(lpszString))
	{
		m_bAllAction = FALSE;
		bCharacter = TRUE;
	}

	else if (m_idGuild != 0)
	{
		CGuild* pGuild = g_GuildMng.GetGuild(m_idGuild);
		if (pGuild && pGuild->IsMaster(m_idPlayer))
		{
			CString strGuildName = pGuild->m_szGuild;
			LPCTSTR lpszGuildString = strGuildName;
			{
				c = strGuildName[0];
				if ((c >= '0' && c <= '9') && !IsMultiByte(lpszGuildString))
					m_bAllAction = FALSE;
			}
		}
	}
	AddAllAction(bCharacter);
}
#endif // __S_SERVER_UNIFY

void CUser::AddBlock(LPBYTE lpBlock, u_long uBlockSize)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar.Write(lpBlock, uBlockSize);
}

void CUser::AddHdr(OBJID objid, WORD wHdr)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << wHdr;
}

void CUser::AddAddObj(CCtrl* pCtrl)
{
	if (IsDelete())	return;

	if (pCtrl == NULL)
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pCtrl->GetId() << SNAPSHOTTYPE_ADD_OBJ << (BYTE)pCtrl->GetType() << pCtrl->GetIndex();
	CObj::SetMethod(pCtrl == this ? METHOD_NONE : METHOD_EXCLUDE_ITEM);
	pCtrl->Serialize(m_Snapshot.ar);

	if (pCtrl->GetType() == OT_MOVER)
	{
		CMover* pMover = static_cast<CMover*>(pCtrl);
		if (IsValidObj(pMover))
		{
			m_Snapshot.ar << pMover->GetDestPos();
			m_Snapshot.ar << pMover->GetDestId();
			m_Snapshot.ar << pMover->m_fArrivalRange;
		}
	}
}

void CUser::AddText(LPCSTR lpsz, DWORD dwColor)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
#ifdef __S_SERVER_UNIFY
	m_Snapshot.ar << TEXT_GENERAL;
#endif // __S_SERVER_UNIFY
	m_Snapshot.ar.WriteString(lpsz);
	m_Snapshot.ar << dwColor;
}


void CUser::AddDiagText(LPCSTR lpsz)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
#ifdef __S_SERVER_UNIFY
	m_Snapshot.ar << SNAPSHOTTYPE_TEXT;
	m_Snapshot.ar << TEXT_DIAG;
#else // __S_SERVER_UNIFY
	m_Snapshot.ar << SNAPSHOTTYPE_DIAG_TEXT;
#endif // __S_SERVER_UNIFY
	m_Snapshot.ar.WriteString(lpsz);
	m_Snapshot.ar << 0xffffffff;
}

#ifdef __S_SERVER_UNIFY
void CUser::AddAllAction(BOOL bCharacter)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ALLACTION;
	m_Snapshot.ar << m_bAllAction;
	m_Snapshot.ar << bCharacter;
}
#endif // __S_SERVER_UNIFY


void CUser::AddReplace(DWORD dwWorldID, D3DXVECTOR3& vPos)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_REPLACE;
	m_Snapshot.ar << dwWorldID;
	m_Snapshot.ar << vPos;
}

void CUser::AddCreateItem(CItemElem* pItemElem, int nType)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CREATEITEM;
	pItemElem->Serialize(m_Snapshot.ar);
	m_Snapshot.ar << nType;;
}

void CUser::AddMoveItem(BYTE nItemType, BYTE nSrcIndex, BYTE nDestIndex)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEITEM;
	m_Snapshot.ar << nItemType << nSrcIndex << nDestIndex;
}

void CUser::AddTrade(CUser* pTrader, u_long uidPlayer)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(pTrader);
	m_Snapshot.ar << SNAPSHOTTYPE_TRADE;
	m_Snapshot.ar << uidPlayer;
}

void CUser::AddComfirmTrade(OBJID objid)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADE;
}

void CUser::AddComfirmTradeCancel(OBJID objid)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMTRADECANCEL;
}


void CUser::AddCommonSkill(DWORD dwSkill, DWORD dwLevel)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COMMONSKILL;
	m_Snapshot.ar << dwSkill << dwLevel;
}

void CUser::AddTradePut(OBJID objid, BYTE byNth, CItemElem* pItemElem, int nItemNum)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUT;
	m_Snapshot.ar << byNth << nItemNum;
	pItemElem->Serialize(m_Snapshot.ar);
}

void CUser::AddTradePutError(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTERROR;
}

void CUser::AddTradePull(OBJID objid, BYTE byNth)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPULL;
	m_Snapshot.ar << byNth;
}

void CUser::AddTradePutGold(OBJID objid, DWORD dwGold)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADEPUTGOLD;
	m_Snapshot.ar << dwGold;
}

void CUser::AddTradeCancel(OBJID objid, u_long uidPlayer, int nMode)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_TRADECANCEL;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar << nMode;
}

void CUser::AddOpenShopWnd(CMover* pVendor)
{
	m_Snapshot.cb++;
	m_Snapshot.ar << pVendor->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_OPENSHOPWND;

	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
	{
		pVendor->m_ShopInventory[i]->Serialize(m_Snapshot.ar);

		// Serialize prices
		int nItemsCount = 0;
		u_long uOffset = m_Snapshot.ar.GetOffset();
		m_Snapshot.ar << nItemsCount;
		for (int z = 0; z < MAX_VENDOR_INVENTORY; z++)
		{
			CItemElem* pItemElem = pVendor->m_ShopInventory[i]->GetAtId(z);
			if (!pItemElem)
				continue;

			if (pItemElem->m_nShopCost == 0)
				continue;

			m_Snapshot.ar << (BYTE)z;
			m_Snapshot.ar << pItemElem->m_nShopCost;

			nItemsCount++;
		}

		int nBufSize1;
		LPBYTE lpBuf1 = m_Snapshot.ar.GetBuffer(&nBufSize1);
		*(UNALIGNED int*)(lpBuf1 + uOffset) = nItemsCount;
	}
}

void CUser::AddUnregisterPVendorItem(BYTE iIndex)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UNREGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << iIndex;
}

void CUser::AddPVendorClose(OBJID objidVendor)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << objidVendor;
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_CLOSE;
	m_Snapshot.ar << (BYTE)0;
}

void CUser::AddPVendorItem(CUser* pUser, BOOL bState)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pUser->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PVENDOR_ITEM;

	u_long uOffset = m_Snapshot.ar.GetOffset();
	BYTE nVendorItem = 0;

	m_Snapshot.ar << (BYTE)0;	// nVendorItem
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemElem = pUser->m_vtInfo.GetItem(i);
		if (pItemElem == NULL)
			continue;

		m_Snapshot.ar << (BYTE)i;
		pItemElem->Serialize(m_Snapshot.ar);
		m_Snapshot.ar << pItemElem->GetExtra();
		m_Snapshot.ar << pItemElem->m_nCost;
#ifdef __FL_VENDORS
		m_Snapshot.ar << pItemElem->m_nPerinCost;
#endif

		nVendorItem++;
	}
	m_Snapshot.ar << bState;

	GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
	*(UNALIGNED BYTE*)(lpBlock + uOffset) = nVendorItem;
}

void CUser::AddPutItemGuildBank(CItemElem* pItemElem)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)1;
	pItemElem->Serialize(m_Snapshot.ar);
}


void CUser::AddGetItemGuildBank(CItemElem* pItemElem, BOOL bItsMe)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;

	if (bItsMe)
		m_Snapshot.ar << (BYTE)1;
	else
		m_Snapshot.ar << (BYTE)3;

	pItemElem->Serialize(m_Snapshot.ar);
}

void CUser::AddGetGoldGuildBank(DWORD p_Gold, BYTE p_Mode, u_long playerID, BYTE cbCloak)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMGUILDBANK;
	m_Snapshot.ar << (BYTE)p_Mode;
	m_Snapshot.ar << p_Gold;
	m_Snapshot.ar << playerID;
	m_Snapshot.ar << cbCloak;
}


void CUser::AddPutItemBank(BYTE nSlot, CItemElem* pItemElem)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTITEMBANK;
	m_Snapshot.ar << nSlot;
	pItemElem->Serialize(m_Snapshot.ar);
}

void CUser::AddGetItemBank(CItemElem* pItemElem)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GETITEMBANK;
	pItemElem->Serialize(m_Snapshot.ar);
}

void CUser::AddPutGoldBank(BYTE nSlot, DWORD dwGold, DWORD dwGoldBank)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUTGOLDBANK;
	m_Snapshot.ar << nSlot << dwGold << dwGoldBank;
}

void CUser::AddMoveBankItem(BYTE nSrcIndex, BYTE nDestIndex)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MOVEBANKITEM;
	m_Snapshot.ar << nSrcIndex << nDestIndex;
}

void CUser::AddBankIsFull(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKISFULL;
}

void CUser::AddRemoveSkillInfluence(WORD wType, WORD wID)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	m_Snapshot.ar << wType;
	m_Snapshot.ar << wID;

}

void CUser::AddBankWindow(int nMode, DWORD dwId, DWORD dwItemId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_BANKWINDOW;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;

}

void CUser::AddGuildBankWindow(int nMode)
{
	if (IsDelete())	return;


	CGuild* pGuild = GetGuild();
	if (pGuild)
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_GUILD_BANK_WND;
		m_Snapshot.ar << nMode;

		m_Snapshot.ar << pGuild->m_nGoldGuild;
		pGuild->m_GuildBank.Serialize(m_Snapshot.ar);
	}
}

void CUser::AddChangeBankPass(int nMode, DWORD dwId, DWORD dwItemId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHANGEBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

void CUser::AddconfirmBankPass(int nMode, DWORD dwId, DWORD dwItemId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CONFIRMBANKPASS;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwId << dwItemId;
}

void CUser::AddUpdateItemEx(unsigned char id, char cParam, __int64 iValue)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM_EX;
	m_Snapshot.ar << id << cParam << iValue;
}

void CUser::AddUpdateItem(CHAR cType, BYTE nId, CHAR cParam, DWORD dwValue, DWORD dwTime, int nType)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_UPDATE_ITEM;
	m_Snapshot.ar << cType << nId << cParam << dwValue;
	m_Snapshot.ar << dwTime;
	m_Snapshot.ar << nType;
}

void CUser::AddSetExperience(EXPINTEGER nExp1, WORD wLevel, int nSkillPoint, int nSkillLevel)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETEXPERIENCE;
	m_Snapshot.ar << nExp1 << wLevel;
	m_Snapshot.ar << nSkillLevel << nSkillPoint;
}

void CUser::AddSetFxp(int nFxp, int nFlightLv)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFXP;
	m_Snapshot.ar << (WORD)nFxp << (WORD)nFlightLv;
}

void CUser::AddSetGrowthLearningPoint(long nRemainGP)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_GROWTH_LEARNING_POINT;
	m_Snapshot.ar << nRemainGP << (long)0;
}

void CUser::AddSetChangeJob(int nJob)
{
	DWORD dwJobLv[MAX_JOB] = { 0, };

	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_JOB_SKILL;
	m_Snapshot.ar << nJob;
	m_Snapshot.ar.Write((void*)&m_aJobSkill[0], sizeof(SKILL) * MAX_SKILL_JOB);
	m_Snapshot.ar.Write((void*)dwJobLv, sizeof(DWORD) * MAX_JOB);
}

void CUser::AddReturnSay(int ReturnFlag, const CHAR* lpszPlayer)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSAY;
	m_Snapshot.ar << ReturnFlag;
	m_Snapshot.ar.WriteString(lpszPlayer);

}

void CUser::AddGameTimer(double dCurrentTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMETIMER;
	m_Snapshot.ar << dCurrentTime;

}

void CUser::AddTaskBar()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TASKBAR;
	m_playTaskBar.Serialize(m_Snapshot.ar);

}

void CUser::AddSendErrorParty(DWORD dw, DWORD dwSkill)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ERRORPARTY;
	m_Snapshot.ar << dw;
	if (dw == ERROR_NOTTARGET)
	{
		m_Snapshot.ar << dwSkill;
	}

}

void CUser::AddSetPartyMemberParam(u_long idPlayer, BYTE nParam, int nVal)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SET_PARTY_MEMBER_PARAM;
	m_Snapshot.ar << idPlayer << nParam << nVal;

}

void CUser::AddPartyMember(CParty* pParty, u_long idPlayer, const char* pszLeader, const char* pszMember)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMEMBER;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString(pszLeader);
	m_Snapshot.ar.WriteString(pszMember);
	if (pParty)
	{
		m_Snapshot.ar << pParty->m_nSizeofMember;
		pParty->Serialize(m_Snapshot.ar);
	}
	else
	{
		m_Snapshot.ar << (int)0;
	}

}

void CUser::AddPartyExpLevel(int Exp, int Level, int nPoint)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYEXP;
	m_Snapshot.ar << Exp;
	m_Snapshot.ar << Level;
	m_Snapshot.ar << nPoint;

}
void CUser::AddSetPartyMode(int nMode, BOOL bOnOff, LONG nPoint, DWORD dwSkillTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETPARTYMODE;
	m_Snapshot.ar << nMode;
	m_Snapshot.ar << dwSkillTime;
	m_Snapshot.ar << bOnOff;
	if (bOnOff == TRUE)
	{
		m_Snapshot.ar << nPoint;
	}

}

void CUser::AddPartyChangeItemMode(int nItemMode)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEITEMMODE;
	m_Snapshot.ar << nItemMode;

}

void CUser::AddPartyChangeExpMode(int nExpMode)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGEEXPMODE;
	m_Snapshot.ar << nExpMode;

}

void CUser::AddPartyChangeName(const char* szPartyName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGENAME;
	m_Snapshot.ar.WriteString(szPartyName);

}

void CUser::AddPartyChangeTroup(const char* szPartyName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHANGETROUP;
	m_Snapshot.ar.WriteString(szPartyName);

}

void CUser::AddPartyRequest(CUser* pLeader, CUser* pMember, BOOL bTroup)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQEST;
	m_Snapshot.ar << pLeader->m_idPlayer;
	m_Snapshot.ar << pMember->m_idPlayer;
	m_Snapshot.ar.WriteString(pLeader->m_szName);
	m_Snapshot.ar << bTroup;

}

void CUser::AddPartyRequestCancel(u_long uLeaderid, u_long uMemberid, int nMode)
{
	// nMode


	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYREQESTCANCEL;
	m_Snapshot.ar << uLeaderid << uMemberid;
	m_Snapshot.ar << nMode;

}

void CUser::AddPartyName()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYNAME;
	m_Snapshot.ar.WriteString(m_szPartyName);

}



void CUser::AddPartySkillCall(const D3DXVECTOR3& vLeader)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_CALL;
	m_Snapshot.ar << vLeader;

}

void	CUser::AddPartySkillBlitz(OBJID idTarget)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYSKILL_BLITZ;
	m_Snapshot.ar << idTarget;

}

void CUser::AddFriendGameJoin()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDGAMEJOIN;
	m_RTMessenger.Serialize(m_Snapshot.ar);
}

void CUser::AddPartyChangeLeader(u_long uidChangeLeader)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYCHANGELEADER;
	m_Snapshot.ar << uidChangeLeader;

}

void CUser::AddCancelQuest(DWORD dwQuestCancelID)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)-1 << dwQuestCancelID;

}

void CUser::AddRemoveQuest(DWORD dwQuestCancelID)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)0 << dwQuestCancelID;

}

void CUser::AddRemoveAllQuest()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)1 << (DWORD)0;

}
void CUser::AddRemoveCompleteQuest()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEQUEST;
	m_Snapshot.ar << (int)2 << (DWORD)0;

}

void CUser::ADDGameJoin(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDGAMEJOIN;
	m_Snapshot.ar << m_dwSavePlayerTime;

}

void CUser::AddFriendReqest(u_long uLeader, LONG nJob, BYTE nSex, const char* szName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDREQEST;
	m_Snapshot.ar << uLeader;
	m_Snapshot.ar << nSex;
	m_Snapshot.ar << nJob;
	m_Snapshot.ar.WriteString(szName);

}

void CUser::AddFriendCancel()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDCANCEL;

}

void CUser::AddFriendError(BYTE nError, const char* szName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIENDERROR;
	m_Snapshot.ar << nError;
	m_Snapshot.ar.WriteString(szName);

}

void CUser::AddEnvironmentSetting()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ENVIRONMENTALL;
	m_Snapshot.ar << CEnvironment::GetInstance()->GetSeason();
}

void CUser::AddEnvironment()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ENVIRONMENT;

	CEnvironment::GetInstance()->Serialize(m_Snapshot.ar);
	m_Snapshot.ar.WriteString(prj.m_EventLua.GetWeatherEventTitle().c_str());
}
void CUser::AddPartyChat(const CHAR* lpName, const CHAR* lpString, OBJID objid)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYCHAT;
	m_Snapshot.ar << objid;

	m_Snapshot.ar.WriteString(lpName);
	m_Snapshot.ar.WriteString(lpString);

}

void CUser::AddAddFriend(u_long idPlayer, const char* lpszPlayer)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADDFRIEND;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString(lpszPlayer);

}

void CUser::AddRemoveFriend(u_long uidSender)
{
	if (IsDelete()) return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEFRIEND;
	m_Snapshot.ar << uidSender;

}


void CUser::AddQueryPlayerData(u_long idPlayer, PlayerData* pPlayerData)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	m_Snapshot.ar << idPlayer;
	m_Snapshot.ar.WriteString(pPlayerData->szPlayer);
	m_Snapshot.ar.Write(&pPlayerData->data, sizeof(sPlayerData));
}


void CUser::AddPlayerData(void)
{
	std::unordered_set<u_long> setPlayers;

	for (map<u_long, Friend>::iterator i1 = m_RTMessenger.begin(); i1 != m_RTMessenger.end(); ++i1)
		setPlayers.insert(i1->first);

	CGuild* pGuild = GetGuild();
	if (pGuild)
	{
		for (map<u_long, CGuildMember*>::iterator i2 = pGuild->m_mapPMember.begin(); i2 != pGuild->m_mapPMember.end(); ++i2)
			setPlayers.insert(i2->second->m_idPlayer);
	}

#ifdef __FL_FIX_PLAYERDATA
	// Add playerdata of all party-members when join the game
	CParty* pParty = g_PartyMng.GetParty(m_idparty);
	if (pParty)
	{
		for (int i = 0; i < pParty->m_nSizeofMember; i++)
			setPlayers.insert(pParty->m_aMember[i].m_uPlayerId);
	}

	// Add playerdata of couple
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple(m_idPlayer);
	if (pCouple)
		setPlayers.insert(pCouple->GetPartner(m_idPlayer));
#endif

	for (auto& it : setPlayers)
	{
		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(it);
		if (pPlayerData)
			AddQueryPlayerData(it, pPlayerData);
	}
}

void CUser::AddGuildInvite(u_long idGuild, u_long idMaster)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_INVITE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar << idMaster;

}

void CUser::AddAllGuilds(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ALL_GUILDS;
	g_GuildMng.Serialize(m_Snapshot.ar, TRUE);

}

void CUser::AddMyGuild(void)
{
	if (IsDelete())	return;

	CGuild* pGuild = GetGuild();
	if (!pGuild)
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD;
	m_Snapshot.ar << pGuild->m_idGuild;
	pGuild->Serialize(m_Snapshot.ar, FALSE);
}

void CUser::AddContribution(CONTRIBUTION_CHANGED_INFO& info)
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_CONTRIBUTION;
	m_Snapshot.ar << info;

}

void CUser::AddSetNotice(u_long idGuild, const char* szNotice)
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_NOTICE;
	m_Snapshot.ar << idGuild;
	m_Snapshot.ar.WriteString(szNotice);

}

void CUser::AddSetGuildAuthority(DWORD dwAuthority[])
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_AUTHORITY;
	m_Snapshot.ar.Write(dwAuthority, sizeof(DWORD) * MAX_GM_LEVEL);

}

void CUser::AddSMModeAll()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SM_MODE_ALL;

	u_long uOffset = m_Snapshot.ar.GetOffset();
	m_Snapshot.ar << (short)0;
	short nReal = 0;

	for (int i = 0; i < SM_MAX; ++i)
	{
		DWORD BufSMTime = 0;
		if (0 < m_dwSMTime[i])
		{
			if (g_AddSMMode.bSMModetime[i])
			{
				CTime tSMtime(m_dwSMTime[i]);
				CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
				BufSMTime = (DWORD)(ct.GetTotalSeconds());
			}
			else
			{
				BufSMTime = m_dwSMTime[i];
			}
			m_Snapshot.ar << i;
			m_Snapshot.ar << BufSMTime;
			++nReal;
		}
	}

	GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
	*(UNALIGNED short*)(lpBlock + uOffset) = nReal;


}

void CUser::AddFlyffEvent(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FLYFF_EVENT;
	g_eLocal.Serialize(m_Snapshot.ar);

}

#if __VER >= 9 // __EVENTLUA
void CUser::AddEventLuaDesc(int nState, string strDesc)
{
	if (strDesc.length() == 0) return;
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EVENTLUA_DESC;
	m_Snapshot.ar << nState;
	m_Snapshot.ar.WriteString(strDesc.c_str());
}
#endif // __EVENTLUA

void CUser::AddRemoveGuildBankItem(u_long idGuild, DWORD dwId, DWORD dwItemNum)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_GUILD_BANK_ITEM;
	m_Snapshot.ar << idGuild << dwId << dwItemNum;

}

void CUser::AddDefinedText(int dwText, LPCSTR lpszFormat, ...)
{
	if (IsDelete())	return;

	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 1, lpszFormat, args);
	va_end(args);

	if (nBuf < 0)
		return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString(szBuffer);

}

void CUser::AddChatText(int dwText, LPCSTR lpszFormat, ...)
{
	if (IsDelete())	return;

	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 1, lpszFormat, args);
	va_end(args);

	if (nBuf < 0)
		return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTEXT;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString(szBuffer);

}

void CUser::AddDefinedText(int dwText)
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDTEXT1;
	m_Snapshot.ar << dwText;

}

void CUser::AddDefinedCaption(BOOL bSmall, int dwText, LPCSTR lpszFormat, ...)
{
	if (IsDelete())	return;

	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 1, lpszFormat, args);
	va_end(args);

	if (nBuf < 0)
		return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DEFINEDCAPTION;
	m_Snapshot.ar << bSmall;
	m_Snapshot.ar << dwText;
	m_Snapshot.ar.WriteString(szBuffer);

}

void CUser::AddSetGuildQuest(int nQuestId, int nState)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETGUILDQUEST;
	m_Snapshot.ar << nQuestId << nState;

}

void CUser::AddRemoveGuildQuest(int nQuestId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEGUILDQUEST;
	m_Snapshot.ar << nQuestId;

}

void CUser::AddSetQuest(LPQUEST lpQuest)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETQUEST;
	m_Snapshot.ar.Write(lpQuest, sizeof(QUEST));//ar << nQuestIdx << nState;

}

void CUser::AddResurrectionMessage()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RESURRECTION_MESSAGE;

}


void CUser::AddCorrReq(CMover* pMover)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CORRREQ;
	m_Snapshot.ar << pMover->GetPos();
	m_Snapshot.ar << pMover->m_idAttacker;

}

void CUser::AddSetFuel(int nFuel, DWORD tmAccFuel)
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETFUEL;
	m_Snapshot.ar << nFuel << tmAccFuel;

}

void CUser::AddSetSkill(DWORD dwSkill, DWORD dwSkillLevel)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CMDSETSKILLLEVEL;
	m_Snapshot.ar << dwSkill << dwSkillLevel;

}

void CUser::AddSetSkillState(CMover* pMover, DWORD dwSkill, DWORD dwLevel, DWORD dwTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << pMover->GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SETSKILLSTATE;
	m_Snapshot.ar << dwSkill;
	m_Snapshot.ar << dwLevel;
	m_Snapshot.ar << dwTime;
}

void CUser::AddPlayMusic(u_long idMusic)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYMUSIC;
	m_Snapshot.ar << idMusic;

}

void CUser::AddPlaySound(u_long idSound)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)0;
	m_Snapshot.ar << idSound;

}

void CUser::AddPlaySound2(const char* szSound)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PLAYSOUND;
	m_Snapshot.ar << (BYTE)1;
	m_Snapshot.ar.WriteString(szSound);

}

void CUser::AddPartyMapInfo(int nIndex, D3DXVECTOR3 vPos)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYMAPINFO;
	m_Snapshot.ar << nIndex;
	m_Snapshot.ar << vPos;

}

void CUser::AddSetNaviPoint(const NaviPoint& nv, OBJID objid, const char* Name)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << objid;
	m_Snapshot.ar << SNAPSHOTTYPE_SETNAVIPOINT;
	m_Snapshot.ar << nv.Pos;// << nv.On;
	m_Snapshot.ar.WriteString(Name);

}

void CUser::AddSnoop(const char* lpString)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SNOOP;
	m_Snapshot.ar.WriteString(lpString);

}

void CUser::AddSetCheerParam(int nCheerPoint, DWORD dwRest, BOOL bAdd)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_SETCHEERPARAM;
	m_Snapshot.ar << nCheerPoint << dwRest;
	m_Snapshot.ar << bAdd; // chipi_080412

}

void CUser::AddQueryEquip(CUser* pUser)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(pUser);
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYEQUIP;

	u_long uOffset = m_Snapshot.ar.GetOffset();
	int cbEquip = 0;

	m_Snapshot.ar << cbEquip;

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		CItemElem* pItemElem = pUser->GetEquipItem(i);
		if (pItemElem)
		{
			m_Snapshot.ar << i;
			pItemElem->Serialize(m_Snapshot.ar);

			cbEquip++;
		}
	}

	GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
	*(UNALIGNED int*)(lpBlock + uOffset) = cbEquip;
}

void CUser::AddSummonFriendUse(CItemElem* pItemElem)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND;
	m_Snapshot.ar << pItemElem->m_dwObjId;
}
void CUser::AddSummonFriendConfirm(OBJID objid, DWORD dwData, const char* szName, const char* szWorldName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_FRIEND_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString(szName);
	m_Snapshot.ar.WriteString(szWorldName);

}
void CUser::AddSummonPartyConfirm(OBJID objid, DWORD dwData, const char* szWorldName)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_SUMMON;
	m_Snapshot.ar << SOMMON_PARTY_CONFIRM;
	m_Snapshot.ar << objid << dwData;
	m_Snapshot.ar.WriteString(szWorldName);

}

void CUser::SetReturnPos(const D3DXVECTOR3& vPos)
{
	ASSERT(GetWorld());

	m_dwReturnWorldID = GetWorld()->GetID();
	m_vReturnPos = vPos;
	TRACE("SetReturnPos: %f %f %f\n", m_vReturnPos.x, m_vReturnPos.y, m_vReturnPos.z);
}

D3DXVECTOR3& CUser::GetReturnPos(DWORD* pdwWorldID)
{
	*pdwWorldID = m_dwReturnWorldID;
	return m_vReturnPos;
}

BOOL CUser::DoUsePackItem(CItemElem* pItemElem, PPACKITEMELEM pPackItemElem, int nType, bool bBulk)
{
	time_t t = 0;
	if (pPackItemElem->nSpan)	// minutes
	{
		CTime time = CTime::GetCurrentTime() + CTimeSpan(0, 0, pPackItemElem->nSpan, 0);
		t = (time_t)(time.GetTime());
	}

	if (m_Inventory.GetEmptyCount() < pPackItemElem->nSize)
	{
		AddDefinedText(TID_GAME_LACKSPACE);
		return FALSE;
	}

	for (int i = 0; i < pPackItemElem->nSize; i++)
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = pPackItemElem->adwItem[i];
		itemElem.SetAbilityOption(pPackItemElem->anAbilityOption[i]);
		itemElem.m_nItemNum = pPackItemElem->anNum[i];
		itemElem.m_dwKeepTime = (DWORD)t;

		if (pItemElem->IsBinds())
			itemElem.SetFlag(CItemElem::binds);

		if (CreateItem(&itemElem))
		{
#ifndef __FL_FIX_USELESS_LOGS
			g_DPSrvr.PutCreateItemLog(this, &itemElem, "E", "PACK");
#endif
	}
}

	OnAfterUseItem(pItemElem->GetProp(), !bBulk);
	if (!bBulk)
		RemoveItem((BYTE)(pItemElem->m_dwObjId), 1, nType);

	return TRUE;

}

BOOL CUser::DoUseGiftbox(CItemElem* pItemElem, int nType, bool bBulk)
{
	GIFTBOXRESULT	result;
	if (!CGiftboxMan::GetInstance()->Open(pItemElem->m_dwItemId, &result))
		return FALSE;

	if (m_Inventory.GetEmptyCount() < 1)
	{
		AddDefinedText(TID_GAME_LACKSPACE);
		return FALSE;
	}

	CItemElem itemElem;
	itemElem.m_dwItemId = result.dwItem;
	itemElem.m_nItemNum = result.nNum;
	if (result.nFlag != 4)
		itemElem.m_byFlag = result.nFlag;

	time_t t = 0;
	if (result.nSpan)
	{
		CTime time = CTime::GetCurrentTime() + CTimeSpan(0, 0, result.nSpan, 0);
		t = (time_t)(time.GetTime());
	}
	itemElem.m_dwKeepTime = t;
	itemElem.SetAbilityOption(result.nAbilityOption);

	if (CreateItem(&itemElem))
	{
#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "E";
		aLogItem.SendName = GetName();
		aLogItem.RecvName = "GIFTBOX";
		aLogItem.WorldId = GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = GetGold();
		g_DPSrvr.OnLogItem(aLogItem, &itemElem, result.nNum);
#endif
}

	OnAfterUseItem(pItemElem->GetProp(), !bBulk);
	if (!bBulk)
		RemoveItem((BYTE)(pItemElem->m_dwObjId), 1, nType);

	return TRUE;
}

#if __VER >= 9	// __PET_0410
void CUser::AddPetState(DWORD dwPetId, WORD wLife, WORD wEnergy, DWORD dwExp)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_PET_STATE;
	m_Snapshot.ar << dwPetId << wLife << wEnergy << dwExp;
}

void	CUser::AddPetSetExp(DWORD dwExp)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_PET_SET_EXP;
	m_Snapshot.ar << dwExp;
}

void CUser::AddPet(CPet* pPet, BYTE nPetLevelup)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_PET;
	pPet->Serialize(m_Snapshot.ar);
	m_Snapshot.ar << nPetLevelup;
}

void CUser::AddPetFoodMill(int nResult, int nCount)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_PET_MILL;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nCount;
}
#endif	// __PET_0410

void	CUser::ProcessCollecting(void)
{
	CItemElem* pCol = GetCollector();
	if (pCol == NULL)
	{

		if (m_pActMover->GetActionState() == OBJSTA_COLLECT)
			StopCollecting();
	}
	else if (IsDisguise())
	{
		if (m_pActMover->GetActionState() == OBJSTA_COLLECT)
			StopCollecting();
	}
	else if (m_pActMover->GetActionState() == OBJSTA_COLLECT)
	{


		if (!HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_SILVERBATTERY)
			&& !HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_GOLDBATTERY)
			&& !HasBuff(BUFF_ITEM2, II_GEN_TOO_COL_BATTERY001))
		{
			if (--pCol->m_nHitPoint < 0)
				pCol->m_nHitPoint = 0;

			UpdateItem((BYTE)(pCol->m_dwObjId), UI_HP, pCol->m_nHitPoint);

			if (pCol->m_nHitPoint == 0)
			{
				StopCollecting();
				return;
			}
		}


		CCollectingProperty* pProperty = CCollectingProperty::GetInstance();
		if (++m_nCollecting >= pProperty->GetCool(pCol->GetAbilityOption()))
		{
			m_nCollecting = 0;

			DWORD dwItemId = pProperty->GetItem();
			if (dwItemId == 0)
				return;
			CItemElem itemElem;
			itemElem.m_dwItemId = dwItemId;
			itemElem.m_nItemNum = 1;
			itemElem.SetSerialNumber();
			if (CreateItem(&itemElem) == TRUE)
			{
				AddRestartCollecting(dwItemId);

#ifdef __FL_RECORD_BOOK
				CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::COLLECTOR_ITEMS, 1, true);
#endif

#ifndef __FL_FIX_USELESS_LOGS
				// log
				LogItemInfo	log;
				log.Action = "c";
				log.SendName = GetName();
				log.RecvName = "CreateItem";
				log.WorldId = GetWorld()->GetID();
				log.Gold = GetGold();
				log.Gold2 = GetGold();
				g_DPSrvr.OnLogItem(log, &itemElem, itemElem.m_nItemNum);
#endif
			}
		}

	}
}

void CUser::AddRestartCollecting(DWORD dwItemId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GETID(this);
	m_Snapshot.ar << SNAPSHOTTYPE_RESTART_COLLECTING;
	m_Snapshot.ar << dwItemId;
}

void CUser::StartCollecting(void)
{
	if (IsNoActionState())
		return;

	if (!IsRegionAttr(RA_COLLECTING))
	{
		AddDefinedText(TID_GAME_COLLECTING_REGION);
		return;
	}
	// state
	if (GetCollector() == NULL)
		return;

	CMover::StartCollecting();
	g_UserMng.AddStartCollecting(this);

#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo	log;
	log.Action = "c";
	log.SendName = GetName();
	log.RecvName = "StartCollecting";
	log.WorldId = GetWorld()->GetID();
	log.Gold = GetGold();
	log.Gold2 = GetGold();
	log.Gold_1 = GetCollector()->m_nHitPoint;
	g_DPSrvr.OnLogItem(log);
#endif

	m_dwHonorCheckTime = GetTickCount();
}

void CUser::StopCollecting(void)
{
	// state
	if (IsNoActionState())
		return;

	CMover::StopCollecting();
	m_nCollecting = 0;
	g_UserMng.AddStopCollecting(this);

#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo	log;
	log.Action = "c";
	log.SendName = GetName();
	log.RecvName = "StopCollecting";
	log.WorldId = GetWorld()->GetID();
	g_DPSrvr.OnLogItem(log);
#endif
}

BOOL CUser::DoUseItemBattery(void)
{
	CItemElem* pCol = GetCollector();
	if (!pCol || m_pActMover->GetActionState() == OBJSTA_COLLECT)
		return FALSE;

	UpdateItem((BYTE)(pCol->m_dwObjId), UI_HP, CCollectingProperty::GetInstance()->GetMaxBattery());

	return TRUE;
}

#ifdef __SYS_TICKET
CItemElem* CUser::FindActiveTicket(DWORD dwItemId)
{
	for (int i = 0; i < m_Inventory.GetMax(); i++)
	{
		CItemElem* pItem = m_Inventory.GetAtId(i);
		if (pItem && pItem->IsActiveTicket(dwItemId))
			return pItem;
	}
	return NULL;
}

void CUser::DoUseItemTicket(CItemElem* pItemElem)
{
	ItemProp* pItemProp = pItemElem->GetProp();
	if (HasBuffByIk3(pItemProp->dwItemKind3))
	{
		if (!HasBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId)))
		{
			AddDefinedText(TID_GAME_MUST_STOP_OTHER_TICKET);
			return;
		}
		if (pItemElem->m_dwKeepTime == 0)
		{
			AddDefinedText(TID_GAME_LIMITED_USE);
			return;
		}
		RemoveBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId));
		REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6971.984F, 100.0F, 3336.884F), REPLACE_FORCE, nDefaultLayer);
	}
	else
	{
		if (pItemElem->m_dwKeepTime == 0)
		{
			CItemElem* pTicket = FindActiveTicket(pItemElem->m_dwItemId);
			if (pTicket)
			{
				AddDefinedText(TID_GAME_LIMITED_USE);
				return;
			}
			else
			{
				UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_KEEPTIME, pItemElem->GetProp()->dwAbilityMin);

#ifndef __FL_FIX_USELESS_LOGS
				// log
				LogItemInfo	log;
				log.Action = "e";
				log.SendName = GetName();
				log.RecvName = "DoUseItemTicket";
				log.WorldId = GetWorld()->GetID();
				log.Gold = GetGold();
				log.Gold2 = GetGold();
				g_DPSrvr.OnLogItem(log, pItemElem, 1);
#endif
			}
		}
		CTicketProperty* pProperty = CTicketProperty::GetInstance();
		TicketProp* pTicketProp = pProperty->GetTicketProp(pItemElem->m_dwItemId);
		if (pTicketProp)
		{
#ifdef __AZRIA_1023
			int nLayer = ::atoi(GetInput());
			int nExpand = CTicketProperty::GetInstance()->GetExpanedLayer(pTicketProp->dwWorldId);
			if (nLayer <= 0 && nLayer >= -nExpand)
			{
				DoApplySkill((CCtrl*)this, pItemProp, NULL);
				REPLACE(g_uIdofMulti, pTicketProp->dwWorldId, pTicketProp->vPos, REPLACE_NORMAL, nLayer);
			}
#else	// __AZRIA_1023
			DoApplySkill((CCtrl*)this, pItemProp, NULL);
			REPLACE(g_uIdofMulti, pTicketProp->dwWorldId, pTicketProp->vPos, REPLACE_NORMAL, nTempLayer);
#endif	// __AZRIA_1023
			}
		}
	}
#endif	// __SYS_TICKET

#ifdef __JEFF_11
void	CUser::AddQuePetResurrectionResult(BOOL bResult)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_QUE_PETRESURRECTION_RESULT;
	m_Snapshot.ar << bResult;
}
#endif	// __JEFF_11

void CUser::ValidateItem(void)
{
	int nSize = m_Inventory.GetMax();
	ItemProp* pItemProp;
	for (int i = 0; i < nSize; i++)
	{
		CItemElem* pItemElem = m_Inventory.GetAtId(i);
		if (pItemElem)
		{
			pItemElem->SetExtra(0);
			if (pItemElem->GetSerialNumber() == 0)
				pItemElem->SetSerialNumber();

			pItemProp = pItemElem->GetProp();
			if (!pItemProp || pItemProp->dwItemKind3 == IK3_VIRTUAL)
			{
				WriteError("CUser::ValidateItem 1 - %d // %d", m_idPlayer, pItemElem->m_dwItemId);
				m_Inventory.RemoveAtId(i);
				continue;
			}
			if (pItemElem->m_nItemNum <= 0)
			{
				WriteError("CUser::ValidateItem 2 - %d // %d // %d // %d", m_idPlayer, pItemElem->m_dwItemId, pItemElem->m_nItemNum, pItemElem->GetSerialNumber());
				m_Inventory.RemoveAtId(i);
			}
		}
	}
}

void CUser::AdjustGuildQuest(DWORD dwWorldId)
{
	CGuildQuestProcessor* pProcessor = CGuildQuestProcessor::GetInstance();
	int nId = -1;
	if (dwWorldId == WI_WORLD_MADRIGAL)
		nId = pProcessor->PtInQuestRect(GetPos());

	if (nId > -1)
	{
		PGUILDQUESTELEM pElem = pProcessor->GetGuildQuest(nId);
		if (!pElem || pElem->idGuild != m_idGuild)
		{
			CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
			if (pWorld)
			{
				PRegionElem pRgnElem = NULL;

				if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
					pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);
				if (!pRgnElem)	// Find near revival pos
					pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), GetPos());

				if (pRgnElem)
					SetPos(pRgnElem->m_vPos);
			}
		}
	}
}

void CUser::AdjustMailboxState(void)
{
	CMailBox* pMailBox = CPost::GetInstance()->GetMailBox(m_idPlayer);
	if (pMailBox && pMailBox->IsStampedMailExists())
		SetMode(MODE_MAILBOX);
	else
		SetNotMode(MODE_MAILBOX);
}

void CUser::AddEventLuaDesc(void)
{
	vector<BYTE> vecList = prj.m_EventLua.GetEventList();
	for (int it = 0; it < (int)(vecList.size()); it++)
		AddEventLuaDesc(2, prj.m_EventLua.GetDesc(vecList[it]));
}

void CUser::AdjustPartyQuest(DWORD dwWorldId)
{
	CPartyQuestProcessor* pProc = CPartyQuestProcessor::GetInstance();
	int nId = pProc->PtInQuestRect(dwWorldId, GetPos());
	if (nId > -1)
	{
		PPARTYQUESTELEM pElem = pProc->GetPartyQuest(nId);
		if (pElem && pElem->idParty == m_idparty)
		{
			DWORD dwTime = pElem->dwEndTime - GetTickCount();
			AddQuestTextTime(TRUE, pElem->nProcess, dwTime);
		}
		else if (!pElem || pElem->idParty != m_idparty)
		{
			SetMode(MODE_OUTOF_PARTYQUESTRGN);
		}
	}
}

#if __VER >= 13 // __COUPLE_1117
void CUser::AddCouple()
{
	if (IsDelete())	return;
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple(m_idPlayer);
	if (pCouple)
	{
		m_Snapshot.cb++;
		m_Snapshot.ar << GetId();
		m_Snapshot.ar << SNAPSHOTTYPE_COUPLE;
		pCouple->Serialize(m_Snapshot.ar);
	}
}

void CUser::AddProposeResult(u_long idProposer, const char* pszProposer)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PROPOSE_RESULT;
	m_Snapshot.ar << idProposer;
	m_Snapshot.ar.WriteString(pszProposer);
}

void CUser::AddCoupleResult(u_long idPartner, const char* pszPartner)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_COUPLE_RESULT;
	m_Snapshot.ar << idPartner;
	m_Snapshot.ar.WriteString(pszPartner);
}

void CUser::AddDecoupleResult()
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DECOUPLE_RESULT;
}

#if __VER >= 13 // __COUPLE_1202

void CUser::ProcessCouple()
{
	BOOL bTick = FALSE;
	if (++m_cbProcessCouple > 60)
	{
		bTick = TRUE;
		m_cbProcessCouple = 0;
	}
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple(m_idPlayer);
	if (pCouple)
	{
		CUser* pPartner = static_cast<CUser*>(prj.GetUserByID(pCouple->GetPartner(m_idPlayer)));
		if (IsValidObj(pPartner))
		{
			if (bTick && m_idPlayer > pPartner->m_idPlayer && pCouple->GetLevel() < CCouple::eMaxLevel)
				g_dpDBClient.SendQueryAddCoupleExperience(m_idPlayer, 1);

#ifndef __FL_EQUALIZED_MODE
			if (!HasBuffByIk3(IK3_COUPLE_BUFF))
#else
			if (!HasBuffByIk3(IK3_COUPLE_BUFF) && !bTmpMode)
#endif
				ActiveCoupleBuff(pCouple->GetLevel());
		}
		else
		{
			RemoveIk3Buffs(IK3_COUPLE_BUFF);
		}
	}
	else
	{
		RemoveIk3Buffs(IK3_COUPLE_BUFF);
	}
}

void CUser::ActiveCoupleBuff(int nLevel)
{
	VS& vSkills = CCoupleProperty::Instance()->GetSkill(nLevel);
	for (int i = 0; i < (int)(vSkills.size()); i++)
	{
		ItemProp* pProp = prj.GetItemProp(vSkills[i]);
		if (pProp)
			DoApplySkill(this, pProp, NULL);
	}
}

void CUser::AddAddCoupleExperience(int nExperience)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ADD_COUPLE_EXPERIENCE;
	m_Snapshot.ar << nExperience;
}
#endif	// __COUPLE_1202

#endif	// __COUPLE_1117
/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CUserMng::CUserMng()
{
	m_lCount = 0;
}

CUserMng::~CUserMng()
{
}

void CUserMng::RemoveAllUsers()
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		DestroyPlayer(it->second);
	}

	m_users.clear();
	m_lCount = 0;
}


CUser* CUserMng::AddUser(DPID dpidCache, DPID dpidUser, DPID dpidSocket)
{
	CUser* pUser = GetUser(dpidCache, dpidUser, true);
	if (!pUser)
	{
		++m_lCount;

		pUser = new CUser(dpidCache, dpidSocket);
		pUser->m_dwSerial = dpidUser;
		m_users.insert(make_pair(dpidUser, pUser));
	}
	else
	{
		Error("CUserMng::AddUser: Duplicate User");
		pUser = NULL;
	}

	return pUser;
}

void CUserMng::RemoveUser(DWORD dwSerial)
{
	map<DWORD, CUser*>::iterator it = m_users.find(dwSerial);
	if (it == m_users.end())
		return;

	CUser* pUser = it->second;

	if (IsValidObj(pUser))
	{
		pUser->OnTradeRemoveUser();

		m_users.erase(it);
		--m_lCount;
		DestroyPlayer(pUser);
	}
}

CUser* CUserMng::GetUser(DPID dpidCache, DPID dpidUser, const bool checkValidity /* = false */)
{
	UNUSED_ALWAYS(dpidCache);

	std::map<DWORD, CUser*>::iterator it = m_users.find(dpidUser);
	if (it != m_users.end())
	{
		if (!checkValidity)
		{
			if (!it->second->m_bValid)
				return NULL;

			if (it->second->m_idPlayer == NULL_ID || it->second->m_idPlayer == 0)
				return NULL;
		}

		return it->second;
	}
	else
		return NULL;
}

CUser* CUserMng::GetUserByPlayerID(u_long idPlayer)
{
	return (CUser*)prj.GetUserByID(idPlayer);
}

#ifdef __VALID_CONNECTION_CONFORMATION
CUser* CUserMng::GetUserBySerial(u_long serial)
{
	for (auto& it : m_users)
		if (it.second->m_dwSerial == serial)
			return it.second;

	return nullptr;
}
#endif

BOOL CUserMng::AddPlayer(CUser* pUser, DWORD dwWorldID, int nLayer)
{
#ifdef __MAP_SECURITY
	pUser->Open(dwWorldID);
#else // 
	pUser->Open();
#endif // 
	BOOL bResult = g_WorldMng.AddObj(pUser, dwWorldID, FALSE, nLayer);
	pUser->SetValid(bResult);
	return bResult;
}

void CUserMng::DestroyPlayer(CUser* pUser)
{
	CWorld* pWorld = pUser->GetWorld();
	if (pWorld == NULL)
	{
		SAFE_DELETE(pUser);
		return;
	}

#ifdef __FL_WHEEL_OF_FORTUNE
	CWheelOfFortune::GetInstance()->FinishProcess(pUser);
#endif

#ifdef __FL_EQUALIZED_MODE
	((CMover*)pUser)->TemporalOut(FALSE);
#endif

	if (pUser->IsCollecting())
		pUser->StopCollecting();

	if (pUser->HasActivatedEatPet())
		pUser->InactivateEatPet();

#ifdef __FL_SHOP_SEARCH
	prj.RemoveFromShopSearch(pUser->GetId());
#endif

	BOOL bDefaultPosition = FALSE;
	if (pUser->IsDie())
	{
		pUser->MakeCharacterAlive(TRUE, FALSE);
		bDefaultPosition = TRUE;
	}
	else
	{
		if (pUser->IsFly())
			pUser->SendActMsg(OBJMSG_STOP);
	}

	CChatting* pChatting = g_ChattingMng.GetChttingRoom(pUser->m_idChatting);
	if (pChatting)
	{
		CUser* pUserBuf;
		for (int i = 0; i < pChatting->GetChattingMember(); ++i)
		{
			pUserBuf = (CUser*)prj.GetUserByID(pChatting->m_idMember[i]);
			if (::IsValidObj(pUserBuf))
			{
				pUserBuf->AddRemoveChatting(pUser->m_idPlayer);
			}
		}
		pChatting->RemoveChattingMember(pUser->m_idPlayer);
		pUser->m_idChatting = 0;
	}

	CHousingMng::GetInstance()->DestroyHousing(pUser->m_idPlayer);

	if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pUser->GetWorld()->GetID()))
	{
		CInstanceDungeonHelper::GetInstance()->LeaveDungeon(pUser, pUser->GetWorld()->GetID());
		bDefaultPosition = TRUE;
	}

	DWORD dwWorld = pWorld->GetID();
	D3DXVECTOR3 vPos = pUser->GetPos();
	int nLayer = pUser->GetLayer();

	if (bDefaultPosition)
	{
		dwWorld = WI_WORLD_MADRIGAL;
		vPos = D3DXVECTOR3(6971.984f, 100.0f, 3336.884f);
		nLayer = nDefaultLayer;
	}

	g_dpDBClient.SavePlayer(pUser, dwWorld, vPos, nLayer, TRUE);
	g_dpDBClient.SendLogConnect(pUser);

#ifdef __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer(0, pUser);
#else	// __INVALID_LOGIN_0320
	g_dpDBClient.CalluspXXXMultiServer(0, pUser->m_idPlayer);
#endif	// __INVALID_LOGIN_0320

	pUser->ResetCheckClientReq();

	if (pUser->m_nAddObjsArrayIndex >= 0)
	{
		pWorld->removeObjectAtProcessArray(pUser);
		SAFE_DELETE(pUser);
	}
	else
		pUser->Delete();

	if (pUser)
	{
		if (g_eLocal.GetState(EVE_GUILDCOMBAT))
		{
			if (g_GuildCombatMng.m_nGCState == CGuildCombat::MAINTENANCE_STATE || g_GuildCombatMng.m_nGCState == CGuildCombat::WAR_WAR_STATE)
				g_GuildCombatMng.OutWar(pUser, NULL, TRUE);
		}
	}
}

void CUserMng::ModifyMode(DWORD dwMode)
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->m_dwAuthorization < AUTH_GAMEMASTER)
		{
			pUser->SetMode(dwMode);
			AddModifyMode(pUser);
		}
	}
}

void CUserMng::AddEventMessage(CCtrl* pCtrl, const TCHAR* szChat, int nFlag, DWORD dwItemId)
{
	CAr ar;

	ar << GETID(pCtrl) << SNAPSHOTTYPE_EVENTMESSAGE;
	ar.WriteString(szChat);
	ar << nFlag;
	ar << dwItemId;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCtrl)
}

void	CUserMng::AddDlgEmoticon(CCtrl* pCtrl, int nIdx)
{
	CHAR szString[32] = "!";
	CHAR szNum[32];
	itoa(nIdx, szNum, 10);
	strcat(szString, szNum);
	AddChat(pCtrl, szString);
}

#if __VER >= 11 // __SYS_COLLECTING
void CUserMng::AddStartCollecting(CUser* pUser)
{
	CAr ar;

	ar << GETID(pUser) << SNAPSHOTTYPE_START_COLLECTING;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
	{
		USERPTR->AddBlock(lpBuf, nBufSize);
	}
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddStopCollecting(CUser* pUser)
{
	CAr ar;

	ar << GETID(pUser) << SNAPSHOTTYPE_STOP_COLLECTING;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
	{
		USERPTR->AddBlock(lpBuf, nBufSize);
	}
	NEXT_VISIBILITYRANGE(pUser)
}
#endif	// __SYS_COLLECTING

void CUserMng::AddChat(CCtrl* pCtrl, const TCHAR* szChat)
{
	CAr ar;

	ar << GETID(pCtrl) << SNAPSHOTTYPE_CHAT;
	ar.WriteString(szChat);

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
	{
		USERPTR->AddBlock(lpBuf, nBufSize);

		if (USERPTR->m_idSnoop > 0 && pCtrl->GetType() == OT_MOVER && ((CMover*)pCtrl)->IsPlayer())
			g_DPCoreClient.SendChat(((CMover*)pCtrl)->m_idPlayer, USERPTR->m_idPlayer, szChat);
	}
	NEXT_VISIBILITYRANGE(pCtrl)
}

void CUserMng::AddDefinedText(CMover* pMover, int dwText, LPCSTR lpszFormat, ...)
{
	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int nBuf = _vsntprintf(szBuffer, sizeof(szBuffer) - 1, lpszFormat, args);
	va_end(args);

	if (nBuf <= 0)
		return;

	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_DEFINEDTEXT;
	ar << dwText;
	ar.WriteString(szBuffer);

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
	{
		USERPTR->AddBlock(lpBuf, nBufSize);
	}
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddUseObject(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_USEOBJECT;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddRemoveAllSkillInfluence(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_REMOVEALLSKILLINFULENCE;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddWorldShout(const TCHAR* szName, const TCHAR* szShout, D3DXVECTOR3 vPos, CWorld* pWorld)
{
	if (pWorld)
	{
		CAr arBlock;
		arBlock << NULL_ID << SNAPSHOTTYPE_SHOUT;
		arBlock << NULL_ID;
		arBlock.WriteString(szName);
		arBlock.WriteString(szShout);
#if __VER >= 12 // __LORD
		arBlock << (DWORD)0xffff99cc;
#endif	// __LORD
		GETBLOCK(arBlock, lpBlock, uBlockSize);

		AddBlock(pWorld, vPos, 0xff, lpBlock, uBlockSize);
	}
}

void CUserMng::AddDamage(CMover* pMover, OBJID objidAttacker, DWORD dwHit, DWORD dwAtkFlags)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_DAMAGE;
	ar << objidAttacker << dwHit;
	ar << dwAtkFlags;

	if (dwAtkFlags & AF_FLYING)
	{
		ar << pMover->GetPos();
		ar << pMover->GetAngle();
	}

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddDisguise(CMover* pMover, DWORD dwMoverIdx)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_DISGUISE << dwMoverIdx;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
void CUserMng::AddNoDisguise(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_NODISGUISE;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
void CUserMng::AddMoverDeath(CMover* pMover, CMover* pAttacker, DWORD dwMsg)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MOVERDEATH;
	ar << GETID(pAttacker) << dwMsg;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddUseSkill(CMover* pMover, DWORD dwSkill, DWORD dwLevel, OBJID objid, int nUseType, int nCastingTime)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_USESKILL;
	ar << dwSkill << dwLevel;
	ar << objid << nUseType << nCastingTime;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddDoEquip(CMover* pMover, DWORD dwPart, CItemElem* pItemElem, BOOL bEquip)
{
	EQUIP_INFO equipInfo;
	equipInfo.dwId = pItemElem->m_dwItemId;
	equipInfo.nOption = pItemElem->GetAttrOption();
	equipInfo.byFlag = pItemElem->m_byFlag;
#ifdef __MODEL_CHANGE
	equipInfo.dwLook = pItemElem->GetLook();
#endif

	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_DOEQUIP;

	ar << pItemElem->m_dwObjId << bEquip;
	ar.Write((void*)&equipInfo, sizeof(EQUIP_INFO));
	ar << dwPart;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddUpdateVendor(CMover* pVendor, CHAR cTab, BYTE nId, short nNum)
{
	CAr ar;
	ar << GETID(pVendor) << SNAPSHOTTYPE_UPDATE_VENDOR;

	ar << cTab << nId << nNum;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pVendor)
		if (USERPTR->m_vtInfo.GetOther() == pVendor)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pVendor)
}

void CUserMng::AddSetDestParam(CMover* pMover, int nDstParameter, int nAdjParameterValue, int nChgParameterValue)
{
	CAr ar;

	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_SETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue << nChgParameterValue;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddResetDestParam(CMover* pMover, int nDstParameter, int nAdjParameterValue)
{
	CAr ar;

	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_RESETDESTPARAM;

	ar << nDstParameter << nAdjParameterValue;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#ifdef __SPEED_SYNC_0108
void CUserMng::AddResetDestParamSync(CMover* pMover, int nDstParameter, int nAdjParameterValue, int nParameterValue)
{
	CAr ar;

	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_RESETDESTPARAM_SYNC;

	ar << nDstParameter << nAdjParameterValue << nParameterValue;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif

void CUserMng::AddSetPointParam(CMover* pMover, int nDstParameter, int nValue)
{
	CAr ar;

	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_SETPOINTPARAM;

	ar << nDstParameter << nValue;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddSetPos(CCtrl* pCtrl, D3DXVECTOR3& vPos)
{
	CAr ar;

	ar << GETID(pCtrl) << SNAPSHOTTYPE_SETPOS;
	ar << vPos;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCtrl)
}


void CUserMng::AddSetLevel(CMover* pMover, WORD wLevel)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_SETLEVEL;
	ar << wLevel;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddSetFlightLevel(CMover* pMover, int nFlightLv)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_SETFLIGHTLEVEL;
	ar << (WORD)nFlightLv;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}


void CUserMng::AddSetSkillLevel(CMover* pMover, DWORD dwSkill, DWORD dwLevel)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_SETSKILLLEVEL;
	ar << dwSkill << dwLevel;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}


void CUserMng::AddSetDestPos(CMover* pMover, CONST D3DXVECTOR3& vPos, BYTE fForward)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_DESTPOS;
	ar << vPos << fForward;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddSetMovePattern(CMover* pMover, int nPattern, const D3DXVECTOR3& vPos, FLOAT fAngle, FLOAT fAngleX)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_SETMOVEPATTERN;
	ar << nPattern << vPos << fAngle << fAngleX;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddMoverSetDestObj(CMover* pMover, OBJID objid, float fRange, BOOL fTransferToMe)
{
	if (pMover == NULL)
		return;

	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_MOVERSETDESTOBJ;
	ar << objid << fRange;

	GETBLOCK(ar, lpBuf, nBufSize);

	if (pMover->IsPlayer() == TRUE && fTransferToMe == TRUE)
	{
		static_cast<CUser*>(pMover)->AddBlock(lpBuf, nBufSize);
	}

	FOR_VISIBILITYRANGE(pMover)
		if (pMover->m_idPlayer != USERPTR->m_idPlayer)
		{
			USERPTR->AddBlock(lpBuf, nBufSize);
		}
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddMeleeAttack(CMover* pMover, OBJMSG dwAtkMsg, OBJID objid)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MELEE_ATTACK;
	ar << dwAtkMsg << objid;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddMeleeAttack2(CMover* pMover, OBJID objid)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MELEE_ATTACK2;
	ar << objid;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddMagicAttack(CMover* pMover, OBJID objid, int nMagicPower, int idSfxHit)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MAGIC_ATTACK;
	ar << objid << nMagicPower << idSfxHit;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddRangeAttack(CMover* pMover, OBJID objid, int nParam2, int idSfxHit)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_RANGE_ATTACK;
	ar << objid << nParam2 << idSfxHit;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
void CUserMng::AddAttackSP(CMover* pMover, DWORD dwAtkMsg, OBJID objid, int nParam2, int nParam3)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_SP_ATTACK;
	ar << dwAtkMsg << objid << nParam2 << nParam3;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddMoverBehavior(CMover* pMover, BOOL bTransferToMe)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MOVERBEHAVIOR;
	pMover->AutoSynchronizer()->Serialize(ar);

	GETBLOCK(ar, lpBuf, nBufSize);

	if (pMover->IsPlayer() == TRUE && bTransferToMe == TRUE)
	{
		static_cast<CUser*>(pMover)->AddBlock(lpBuf, nBufSize);
	}

	FOR_VISIBILITYRANGE(pMover)
		if (pMover->m_idPlayer != USERPTR->m_idPlayer)
		{
			USERPTR->AddBlock(lpBuf, nBufSize);
		}
	NEXT_VISIBILITYRANGE(pMover)
}
void CUserMng::AddCreateSfxObj(CCtrl* pCtrl, DWORD dwSfxObj, float x, float y, float z, BOOL bFlag)
{
	CAr ar;
	ar << GETID(pCtrl) << SNAPSHOTTYPE_CREATESFXOBJ;

	ar << dwSfxObj << x << y << z;
	ar << bFlag;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCtrl)
}


void CUserMng::AddMotionArrive(CMover* pMover, OBJMSG objmsg)
{
	if (pMover == NULL)
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MOTION_ARRIVE;

	ar << objmsg;

	GETBLOCK(ar, lpBuf, nBufSize);

	if (pMover->IsPlayer() == TRUE)
	{
		static_cast<CUser*>(pMover)->AddBlock(lpBuf, nBufSize);
	}

	FOR_VISIBILITYRANGE(pMover)
		if (pMover->m_idPlayer != USERPTR->m_idPlayer)
		{
			USERPTR->AddBlock(lpBuf, nBufSize);
		}
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddWorldCreateSfxObj(DWORD dwSfxObj, float x, float y, float z, BOOL bFlag, DWORD dwWorldId)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_CREATESFXOBJ << dwSfxObj << x << y << z << bFlag;
	GETBLOCK(arBlock, lpBlock, uBlockSize);
	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	AddBlockNoLock(lpBlock, uBlockSize, pWorld);
}

void	CUserMng::AddCreateSfxAllow(CMover* pMover, DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_CREATESFXALLOW;
	ar << dwSfxObjArrow << dwSfxObjHit << vPosDest.x << vPosDest.y << vPosDest.z << idTarget;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddNearSetChangeJob(CMover* pMover, int nJob, LPSKILL lpSkill)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_SET_NEAR_JOB_SKILL;
	ar << nJob;

	if (pMover->IsHero())
		ar.Write((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
	else if (pMover->IsLegendHero())
		ar.Write((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL));
	else if (pMover->IsMaster())
		ar.Write((void*)&pMover->m_aJobSkill[MAX_EXPERT_SKILL], sizeof(SKILL) * (MAX_PRO_SKILL + MAX_MASTER_SKILL));
	else
		ar.Write((void*)&pMover->m_aJobSkill[MAX_JOB_SKILL], sizeof(SKILL) * (MAX_EXPERT_SKILL));

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddModifyMode(CUser* pUser)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_MODIFYMODE;
	ar << pUser->m_dwMode;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddStateMode(CUser* pUser, BYTE nFlag)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_STATEMODE;
	ar << pUser->m_dwStateMode;
	ar << nFlag;
	if (nFlag == STATEMODE_BASEMOTION_ON)
	{
		ItemProp* pItemProp = NULL;
		if (pUser->m_bItemFind)
		{
			CItemElem* pItemElem = ((CMover*)pUser)->m_Inventory.GetAtId(pUser->m_dwUseItemId);
			if (pItemElem)
			{
				pItemProp = pItemElem->GetProp();
			}
		}
		else
		{
			pItemProp = prj.GetItemProp(pUser->m_dwUseItemId);
		}

		if (pItemProp)
		{
			ar << pItemProp->dwID;
		}
		else
		{
			Error("나오면 안됨.. 시전");
			return;
		}
	}

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}


void CUserMng::AddGameRate(FLOAT fRate, BYTE nFlag)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GAMERATE;
	ar << fRate;
	ar << nFlag;
	GETBLOCK(ar, lpBuf, uBufSize);

	AddBlock(lpBuf, uBufSize);	// all
}

void CUserMng::AddChangeFace(u_long uidPlayer, DWORD dwFace)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CHANGEFACE;
	ar << uidPlayer << dwFace;
	GETBLOCK(ar, lpBuf, uBufSize);

	AddBlock(lpBuf, uBufSize);	// all
}

void CUserMng::AddSexChange(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_SEX_CHANGE;
	ar << (u_short)pMover->GetIndex();
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddCreateGuild(u_long idPlayer, const char* lpszPlayer, u_long idGuild, const char* szGuild)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_CREATE_GUILD;
	ar << idPlayer << idGuild;
	ar.WriteString(lpszPlayer ? lpszPlayer : "");
	ar.WriteString(szGuild);
	GETBLOCK(ar, lpBuf, uBufSize);

	AddBlock(lpBuf, uBufSize);	// all
}

void CUserMng::AddDestroyGuild(const char* lpszPlayer, u_long idGuild)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_DESTROY_GUILD;
	ar.WriteString(lpszPlayer ? lpszPlayer : "");
	ar << idGuild;
	GETBLOCK(ar, lpBuf, uBufSize);

	AddBlock(lpBuf, uBufSize);	// all
}

void CUserMng::AddSetGuild(CUser* pUser, u_long idGuild)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_SET_GUILD;
	ar << idGuild;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddGetItemElem(CUser* pUser, CItemElem* pItemElem)
{
	CAr ar;

	ar << GETID(pUser) << SNAPSHOTTYPE_GETITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize(ar);
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		if (USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddPutItemElem(CUser* pUser, CItemElem* pItemElem)
{
	CAr ar;

	ar << GETID(pUser) << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize(ar);
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		if (USERPTR != pUser && USERPTR->m_bGuildBank == TRUE && USERPTR->m_idGuild == pUser->m_idGuild)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

#if __VER >= 8 // __GUILDCOMBAT_85
void CUserMng::AddPutItemElem(u_long uidGuild, CItemElem* pItemElem)
{
	CAr ar;

	ar << NULL_ID << SNAPSHOTTYPE_PUTITEMGUILDBANK;
	ar << (BYTE)3;
	pItemElem->Serialize(ar);
	GETBLOCK(ar, lpBuf, nBufSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUsertmp = it->second;
		if (pUsertmp->IsValid() == FALSE)
			continue;

		if (pUsertmp->m_idGuild == uidGuild)
			pUsertmp->AddBlock(lpBuf, nBufSize);
	}
}
#endif // __VER >= 8

void CUserMng::AddPVendorOpen(CUser* pUser)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_PVENDOR_OPEN;
	ar.WriteString(pUser->m_vtInfo.GetTitle());
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddPVendorClose(CUser* pUser)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_PVENDOR_CLOSE;
	ar << (BYTE)1;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddPVendorItemNum(CUser* pUser, BYTE nItem, short nVend, const char* sBuyer)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_PVENDOR_ITEM_NUM;
	ar << nItem << nVend;
	ar.WriteString(sBuyer);
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		if (USERPTR == pUser || USERPTR->m_vtInfo.GetOther() == pUser)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddSetHair(CUser* pUser, BYTE nHair, BYTE r, BYTE g, BYTE b)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_SET_HAIR;
	ar << nHair << r << g << b;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddHdr(CCtrl* pCtrl, WORD wHdr)
{
	CAr ar;
	ar << GETID(pCtrl) << wHdr;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCtrl)
}

void CUserMng::AddBlock(LPBYTE lpBlock, u_long uBlockSize)
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld())
			pUser->AddBlock(lpBlock, uBlockSize);
	}

}

void CUserMng::AddBlock(LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld)
{
	if (!pWorld)
		return;

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld() == pWorld)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddBlockNoLock(LPBYTE lpBlock, u_long uBlockSize)
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld())
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddBlockNoLock(LPBYTE lpBlock, u_long uBlockSize, CWorld* pWorld)
{
	if (!pWorld)
		return;

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld() == pWorld)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddBlock(CWorld* pWorld, const D3DXVECTOR3& vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize)
{
	if (!pWorld)
		return;

	float d = (float)(nRange * nRange);
	D3DXVECTOR3 vtmp;

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		vtmp = vPos - pUser->GetPos();
		if (pUser->GetWorld() == pWorld && D3DXVec3LengthSq(&vtmp) < d)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddGameSetting(void)
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		pUser->AddGameSetting();
	}
}
#if __VER >= 13
void CUserMng::AddShout(CUser* pUserSrc, int nRange, LPBYTE lpBlock, u_long uBlockSize)
#else // __VER >= 13
void CUserMng::AddShout(const D3DXVECTOR3& vPos, int nRange, LPBYTE lpBlock, u_long uBlockSize)
#endif // __VER >= 13
{
	float fRange = (float)(nRange * nRange);
	D3DXVECTOR3 v;

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;
		if (pUser->GetWorld() == NULL)
			continue;
#if __VER >= 13
		if (nRange > 0)
		{
			v = pUserSrc->GetPos() - pUser->GetPos();
			if (pUser->GetWorld() != pUserSrc->GetWorld() || pUser->GetLayer() != pUserSrc->GetLayer()
				|| D3DXVec3LengthSq(&v) > fRange
				)
				continue;
		}

		pUser->AddBlock(lpBlock, uBlockSize);
#else // __VER >= 13
		v = vPos - pUser->GetPos();
		if (nRange == 0 || D3DXVec3LengthSq(&v) < fRange)
		{
			pUser->AddBlock(lpBlock, uBlockSize);
	}
#endif // __VER >= 13
}
}

void CUserMng::AddWorldMsg(const CRect* pRect, LPCTSTR lpszString)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString(lpszString);
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	D3DXVECTOR3 vtmp;

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		vtmp = pUser->GetPos();
		if (pUser->GetWorld())
		{
			if (pRect == NULL)
				pUser->AddBlock(lpBlock, uBlockSize);
			else if (pRect->PtInRect(CPoint((int)vtmp.x, (int)vtmp.y)))
				pUser->AddBlock(lpBlock, uBlockSize);
		}
	}
}


// ex)


void CUserMng::AddSetScale(CMover* pMover, float fScalePercent)
{
	CAr ar;

	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_SETSCALE;
	ar << fScalePercent;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}



void CUserMng::AddSetSkillState(CMover* pCenter, CMover* pTarget, WORD wType, WORD wID, DWORD dwLevel, DWORD dwTime)
{
	CAr ar;

	ar << GETID(pCenter);
	ar << SNAPSHOTTYPE_SETSKILLSTATE;
	ar << pTarget->GetId() << wType << wID;
#if __VER >= 11 // __SYS_COLLECTING
	if (wType == BUFF_ITEM2)
	{
		time_t t = (time_t)dwLevel - time_null();
		ar << t;
	}
	else
		ar << dwLevel;
#else	// __SYS_COLLECTING
	ar << dwLevel;
#endif	// __SYS_COLLECTING
	ar << dwTime;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCenter)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCenter)
}

#ifdef __S1108_BACK_END_SYSTEM
void CUserMng::AddMonsterProp()
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_MONSTERPROP;

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

	ar << prj.m_nRemoveMonsterPropSize;
	for (int i = 0; i < prj.m_nRemoveMonsterPropSize; ++i)
	{
		ar.WriteString(prj.m_aRemoveProp[i]);
	}

	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);	// all
}

void CUserMng::AddGMChat(int nSize)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GMCHAT;
	ar << nSize;
	for (int i = 0; i < nSize; ++i)
	{
		ar.WriteString(prj.m_chGMChat[i]);
	}

	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);	// all
}
#endif // __S1108_BACK_END_SYSTEM

void CUserMng::AddGuildCombatUserState(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover);
	ar << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_USERSTATE;
	ar << pMover->GetId() << pMover->m_nGuildCombatState;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddGCWinGuild(void)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_WINGUILD;
	ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;

	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlockNoLock(lpBuf, uBufSize);	// all
}
void CUserMng::AddGCBestPlayer(void)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT << GC_BESTPLAYER;
	ar << g_GuildCombatMng.m_uBestPlayer;

	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlockNoLock(lpBuf, uBufSize);	// all
}

void CUserMng::AddGuildCombatState(void)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	ar << GC_GCSTATE;
	ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;

	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlockNoLock(lpBuf, uBufSize);	// all
}

void CUserMng::AddGuildCombatNextTimeWorld(DWORD dwTime, DWORD dwState)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_NEXTTIMESTATE;
	arBlock << dwTime << dwState;
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		pUser->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGuildCombatEnterTime(DWORD dwTime)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ENTERTIME;
	arBlock << dwTime;
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddGCWarPlayerlist(u_long uidGuild, CUser* pSendUser)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList(arBlock);
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	if (pSendUser == NULL)
	{
		map<DWORD, CUser*>::iterator it;
		for (it = m_users.begin(); it != m_users.end(); ++it)
		{
			CUser* pUser = it->second;
			if (pUser->IsValid() == FALSE)
				continue;

			if (uidGuild)
			{
				if (uidGuild == pUser->m_idGuild)
				{
					pUser->AddBlock(lpBlock, uBlockSize);
				}
			}
			else if (pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
			{
				pUser->AddBlock(lpBlock, uBlockSize);
			}
		}
	}
	else
	{
		pSendUser->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGCLogWorld(void)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG;
	arBlock << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for (int veci = 0; veci < (int)(g_GuildCombatMng.m_vecGCGetPoint.size()); ++veci)
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint[veci];
		arBlock << GCGetPoint.uidGuildAttack;
		arBlock << GCGetPoint.uidGuildDefence;
		arBlock << GCGetPoint.uidPlayerAttack;
		arBlock << GCGetPoint.uidPlayerDefence;
		arBlock << GCGetPoint.nPoint;
		arBlock << GCGetPoint.bKillDiffernceGuild;
		arBlock << GCGetPoint.bMaster;
		arBlock << GCGetPoint.bDefender;
		arBlock << GCGetPoint.bLastLife;
	}
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGCLogRealTimeWorld(CGuildCombat::__GCGETPOINT GCGetPoint)
{
	LPCSTR szAttacker, szDefender;
	szAttacker = CPlayerDataCenter::GetInstance()->GetPlayerString(GCGetPoint.uidPlayerAttack);
	szDefender = CPlayerDataCenter::GetInstance()->GetPlayerString(GCGetPoint.uidPlayerDefence);
	if (szAttacker == NULL || szDefender == NULL)
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_LOG_REALTIME;
	arBlock << GCGetPoint.uidGuildAttack;
	arBlock << GCGetPoint.uidGuildDefence;
	arBlock.WriteString(szAttacker);
	arBlock.WriteString(szDefender);
	arBlock << GCGetPoint.uidPlayerAttack;
	arBlock << GCGetPoint.uidPlayerDefence;
	arBlock << GCGetPoint.nPoint;
	arBlock << GCGetPoint.bKillDiffernceGuild;
	arBlock << GCGetPoint.bMaster;
	arBlock << GCGetPoint.bDefender;
	arBlock << GCGetPoint.bLastLife;
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::ReplaceWorld(DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, int nLayer)
{
	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	if (pWorld)
	{
		map<DWORD, CUser*>::iterator it;
		for (it = m_users.begin(); it != m_users.end(); ++it)
		{
			CUser* pUser = it->second;
			if (pUser->IsValid() == FALSE)
				continue;

			if (pUser->GetWorld() == pWorld)
			{
				D3DXVECTOR3 v3Pos = pUser->GetPos();
				((CMover*)pUser)->REPLACE(g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3(fReplaceX, 0.0f, fReplaceZ), REPLACE_NORMAL, nLayer);
				pUser->m_vtInfo.SetOther(NULL);
			}
		}
	}
}

void CUserMng::ReplaceWorldArea(u_long idParty, DWORD dwWorldId, DWORD dwReplaceWorldId, float fReplaceX, float fReplaceZ, float fArea, BOOL bDieFlag, int nLayer)
{
	CWorld* pWorld = g_WorldMng.GetWorld(dwWorldId);
	if (pWorld)
	{
		for (int i = 0; i < (int)(pWorld->m_dwObjNum); i++)
		{
			CObj* pObj = pWorld->m_apObject[i];
			if (IsValidObj(pObj) && pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsPlayer() && ((CMover*)pObj)->m_idparty == idParty)
			{
				CUser* pUser = (CUser*)pObj;
				if (bDieFlag)
				{
					if (pUser->IsDie())
						continue;
				}

				D3DXVECTOR3 vPos = pUser->GetPos();
				float fNewArea = fArea * 2.0f;
				fReplaceX += (-fArea) + xRandomF(fNewArea);
				fReplaceZ += (-fArea) + xRandomF(fNewArea);
				pUser->REPLACE(g_uIdofMulti, dwReplaceWorldId, D3DXVECTOR3(fReplaceX, 0.0f, fReplaceZ), REPLACE_NORMAL, nLayer);
			}
		}
	}
}

void CUserMng::AddWorldMsg(DWORD dwWorldId, LPCTSTR lpszString)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_WORLDMSG;
	arBlock.WriteString(lpszString);
	GETBLOCK(arBlock, lpBlock, uBlockSize);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;
		if (pUser->GetWorld()->GetID() == dwWorldId)
			pUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUserMng::AddGuildMsg(u_long idGuild, LPCSTR lpsz)
{
	CGuild* pGuild = g_GuildMng.GetGuild(idGuild);
	if (pGuild)
		AddGuildMsg(pGuild, lpsz);
}

void CUserMng::AddGuildMsg(CGuild* pGuild, LPCSTR lpsz)
{
	CGuildMember* pMember;
	CUser* pUsertmp;

	for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
		i != pGuild->m_mapPMember.end(); ++i)
	{
		pMember = i->second;
		pUsertmp = (CUser*)prj.GetUserByID(pMember->m_idPlayer);
		if (IsValidObj(pUsertmp))
			pUsertmp->AddText(lpsz);
	}
}

void CUserMng::AddGCIsRequest(u_long uidGuild, BOOL bRequest)
{
	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_ISREQUEST;
	arBlock << bRequest;

	GETBLOCK(arBlock, lpBlock, uBlockSize);
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUsertmp = it->second;
		if (pUsertmp->IsValid() == FALSE)
			continue;

		if (pUsertmp->m_idGuild == uidGuild)
			pUsertmp->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGCGuildStatus(u_long uidGuild, CUser* pSendUser)
{
	if (g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDSTATUS;

	CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.FindGuildCombatMember(uidGuild);
	if (pGCMember != NULL)
	{

		list<CGuildCombat::__JOINPLAYER*> lspPlyaerList;
		lspPlyaerList.clear();

		CGuildCombat::__JOINPLAYER* pJoinPlayer;

		for (list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1)
		{
			pJoinPlayer = *i1;
			lspPlyaerList.push_back(pJoinPlayer);
		}


		for (int veci = pGCMember->vecGCSelectMember.size() - 1; veci >= 0; --veci)
		{
			pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			if (0 < pJoinPlayer->nlife)
			{
				BOOL bFind = FALSE;
				for (list<CGuildCombat::__JOINPLAYER*>::iterator i1 = pGCMember->lspFifo.begin(); i1 != pGCMember->lspFifo.end(); ++i1)
				{
					if (pJoinPlayer == *i1)
					{
						bFind = TRUE;
						break;
					}
				}
				if (bFind == FALSE)
				{
					lspPlyaerList.push_front(pJoinPlayer);
				}
			}
			else
			{
				lspPlyaerList.push_back(pJoinPlayer);
			}
		}


		if (0 < pGCMember->lspFifo.size())
			arBlock << g_GuildCombatMng.m_nMaxWarPlayer;
		else
			arBlock << (int)0;

		arBlock << (int)lspPlyaerList.size();
		for (list<CGuildCombat::__JOINPLAYER*>::iterator i1 = lspPlyaerList.begin(); i1 != lspPlyaerList.end(); ++i1)
		{
			pJoinPlayer = *i1;
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nlife;
		}
	}
	else
	{
		arBlock << (int)0;
		arBlock << (int)0;
	}

	GETBLOCK(arBlock, lpBlock, uBlockSize);
	if (pSendUser == NULL)
	{
		map<DWORD, CUser*>::iterator it;
		for (it = m_users.begin(); it != m_users.end(); ++it)
		{
			CUser* pUser = it->second;
			if (pUser->IsValid() == FALSE)
				continue;

			if (pUser->m_idGuild == uidGuild)
			{
				pUser->AddBlock(lpBlock, uBlockSize);
			}
		}
	}
	else
	{
		pSendUser->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGCGuildPrecedence(CUser* pSendUser)
{
	if (g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_GUILDPRECEDENCE;

	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for (int gcmi = 0; gcmi < (int)(g_GuildCombatMng.m_vecGuildCombatMem.size()); ++gcmi)
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];
		if (0 < pGCMember->vecGCSelectMember.size())
		{
			arBlock << (BOOL)TRUE; // bSend;
			CGuild* pGuild = g_GuildMng.GetGuild(pGCMember->uGuildId);
			if (pGuild)
				arBlock.WriteString(pGuild->m_szGuild);
			else
				arBlock.WriteString("Not Guild");
			arBlock << pGCMember->nGuildPoint;
		}
		else
		{
			arBlock << (BOOL)FALSE; // bSend;
		}
	}

	GETBLOCK(arBlock, lpBlock, uBlockSize);
	if (pSendUser == NULL)
	{
		map<DWORD, CUser*>::iterator it;
		for (it = m_users.begin(); it != m_users.end(); ++it)
		{
			CUser* pUser = it->second;
			if (pUser->IsValid() == FALSE)
				continue;
			if (pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
				pUser->AddBlock(lpBlock, uBlockSize);
		}
	}
	else
	{
		pSendUser->AddBlock(lpBlock, uBlockSize);
	}
}
void CUserMng::AddGCPlayerPrecedence(CUser* pSendUser)
{
	if (g_GuildCombatMng.m_nState == CGuildCombat::CLOSE_STATE)
		return;

	CAr arBlock;
	arBlock << NULL_ID << SNAPSHOTTYPE_GUILDCOMBAT;
	arBlock << GC_PLAYERPRECEDENCE;

	arBlock << (int)g_GuildCombatMng.m_vecGuildCombatMem.size();
	for (int gcmi = 0; gcmi < (int)(g_GuildCombatMng.m_vecGuildCombatMem.size()); ++gcmi)
	{
		CGuildCombat::__GuildCombatMember* pGCMember = g_GuildCombatMng.m_vecGuildCombatMem[gcmi];
		arBlock << (int)pGCMember->vecGCSelectMember.size();
		for (int veci = 0; veci < (int)(pGCMember->vecGCSelectMember.size()); ++veci)
		{
			CGuildCombat::__JOINPLAYER* pJoinPlayer = pGCMember->vecGCSelectMember[veci];
			arBlock << pJoinPlayer->uidPlayer;
			arBlock << pJoinPlayer->nPoint;
		}
	}

	GETBLOCK(arBlock, lpBlock, uBlockSize);

	if (pSendUser == NULL)
	{
		map<DWORD, CUser*>::iterator it;
		for (it = m_users.begin(); it != m_users.end(); ++it)
		{
			CUser* pUser = it->second;
			if (pUser->IsValid() == FALSE)
				continue;

			if (pUser->GetWorld()->GetID() == WI_WORLD_GUILDWAR)
			{
				pUser->AddBlock(lpBlock, uBlockSize);
			}
		}
	}
	else
	{
		pSendUser->AddBlock(lpBlock, uBlockSize);
	}
}

void CUser::AddRunScriptFunc(const RunScriptFunc& runScriptFunc)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RUNSCRIPTFUNC;
	m_Snapshot.ar << runScriptFunc.wFuncType;
	switch (runScriptFunc.wFuncType)
	{
	case FUNCTYPE_ADDKEY:
	case FUNCTYPE_ADDANSWER:
	{
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal1);
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal2);
		m_Snapshot.ar << runScriptFunc.dwVal1;
		m_Snapshot.ar << runScriptFunc.dwVal2;
		break;
	}
	case FUNCTYPE_REMOVEKEY:
	{
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal1);
		break;
	}
	case FUNCTYPE_SAY:
	case FUNCTYPE_SAYQUEST:
	{
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal1);
		m_Snapshot.ar << runScriptFunc.dwVal2;
		break;
	}
	case FUNCTYPE_SETNAVIGATOR:
	{
		m_Snapshot.ar << runScriptFunc.dwVal1;
		m_Snapshot.ar << runScriptFunc.vPos;
		break;
	}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	case FUNCTYPE_NEWQUEST:
	case FUNCTYPE_CURRQUEST:
	{
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal1);
		m_Snapshot.ar.WriteString(runScriptFunc.lpszVal2);
		m_Snapshot.ar << runScriptFunc.dwVal1;
		m_Snapshot.ar << runScriptFunc.dwVal2;
		break;
	}
#endif // __IMPROVE_QUEST_INTERFACE
	default:
		break;
	}

}

void CUser::AddEnterChatting(CUser* pUser)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_ENTERMEMBER;
	m_Snapshot.ar << pUser->m_idPlayer;
	m_Snapshot.ar.WriteString(pUser->GetName());

}

void CUser::AddNewChatting(CChatting* pChatting)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_NEWCHATTING;
	m_Snapshot.ar << pChatting->GetChattingMember();
	for (int i = 0; i < pChatting->GetChattingMember(); ++i)
	{
		m_Snapshot.ar << pChatting->m_idMember[i];
#if __VER >= 11 // __SYS_PLAYER_DATA
		if (NULL == CPlayerDataCenter::GetInstance()->GetPlayerString(pChatting->m_idMember[i]))
#else	// __SYS_PLAYER_DATA
		if (NULL == prj.GetPlayerString(pChatting->m_idMember[i]))
#endif	// __SYS_PLAYER_DATA
		{
			Error("AddNewChatting : MAX_ChattingMember = %d, GetMember = %d", pChatting->GetChattingMember(), pChatting->m_idMember[i]);
			m_Snapshot.ar.WriteString("");
		}
		else
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			m_Snapshot.ar.WriteString(CPlayerDataCenter::GetInstance()->GetPlayerString(pChatting->m_idMember[i]));
#else	//__SYS_PLAYER_DATA
			m_Snapshot.ar.WriteString(prj.GetPlayerString(pChatting->m_idMember[i]));
#endif	// __SYS_PLAYER_DATA
		}
	}
}

void CUser::AddRemoveChatting(u_long uidPlayer)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_REMOVEMEMBER;
	m_Snapshot.ar << uidPlayer;

}

void CUser::AddDeleteChatting()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_DELETECHATTING;

}

void CUser::AddChatting(u_long uidPlayer, char* pszChat)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTING;
	m_Snapshot.ar << uidPlayer;
	m_Snapshot.ar.WriteString(pszChat);

}

void CUser::AddChttingRoomState(BOOL bState)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_CHATTING;
	m_Snapshot.ar << CHATTING_CHATTINGROOMSTATE;
	m_Snapshot.ar << bState;

}

void CUser::AddGameRate(FLOAT fRate, BYTE nFlag)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GAMERATE;
	m_Snapshot.ar << fRate;
	m_Snapshot.ar << nFlag;

}


void CUser::AddGameSetting()
{
	if (IsDelete())	return;

	AddGameRate(prj.m_fShopCost, GAME_RATE_SHOPCOST);
	AddGameRate((float)(prj.m_dwVagSP), GAME_SKILL_VAGSP);
	AddGameRate((float)(prj.m_dwExpertSP), GAME_SKILL_EXPERTSP);
	AddGameRate((float)(prj.m_dwProSP), GAME_SKILL_PROSP);
#ifdef __SHOP_COST_RATE
	AddGameRate(prj.m_EventLua.GetShopBuyFactor(), GAME_RATE_SHOP_BUY);
	AddGameRate(prj.m_EventLua.GetShopSellFactor(), GAME_RATE_SHOP_SELL);
#endif // __SHOP_COST_RATE
	if (((CMover*)this)->IsAuthHigher(AUTH_GAMEMASTER))
	{
		AddGameRate(prj.m_fItemDropRate, GAME_RATE_ITEMDROP);
		AddGameRate(prj.m_fGoldDropRate, GAME_RATE_GOLDDROP);
		AddGameRate(prj.m_fMonsterExpRate, GAME_RATE_MONSTEREXP);
		AddGameRate(prj.m_fMonsterHitRate, GAME_RATE_MONSTERHIT);

#ifdef __S1108_BACK_END_SYSTEM
		AddGameRate(prj.m_fMonsterRebirthRate, GAME_RATE_REBIRTH);
		AddGameRate(prj.m_fMonsterHitpointRate, GAME_RATE_HITPOINT);
		AddGameRate(prj.m_fMonsterAggressiveRate, GAME_RATE_AGGRESSIVE);
#endif // __S1108_BACK_END_SYSTEM
	}
}

#ifdef __S1108_BACK_END_SYSTEM
void CUser::AddMonsterProp()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MONSTERPROP;

	m_Snapshot.ar << prj.m_nMonsterPropSize;
	for (int i = 0; i < prj.m_nMonsterPropSize; ++i)
	{
		m_Snapshot.ar.WriteString(prj.m_aMonsterProp[i].szMonsterName);
		m_Snapshot.ar << prj.m_aMonsterProp[i].nHitPoint;
		m_Snapshot.ar << prj.m_aMonsterProp[i].nAttackPower;
		m_Snapshot.ar << prj.m_aMonsterProp[i].nDefence;
		m_Snapshot.ar << prj.m_aMonsterProp[i].nExp;
		m_Snapshot.ar << prj.m_aMonsterProp[i].nItemDrop;
		m_Snapshot.ar << prj.m_aMonsterProp[i].nPenya;
	}

	m_Snapshot.ar << prj.m_nRemoveMonsterPropSize;
	for (int i = 0; i < prj.m_nRemoveMonsterPropSize; ++i)
	{
		m_Snapshot.ar.WriteString(prj.m_aRemoveProp[i]);
	}


}
#endif // __S1108_BACK_END_SYSTEM

void CUser::AddInitSkill()
{

	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_INITSKILLPOINT;
	m_Snapshot.ar << m_nSkillPoint;
}


void CUser::AddDoUseSkillPoint(SKILL aJobSkill[], int nSkillPoint)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_DOUSESKILLPOINT;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill;
		lpSkill = &aJobSkill[i];

		m_Snapshot.ar << lpSkill->dwSkill << lpSkill->dwLevel;
	}
	m_Snapshot.ar << nSkillPoint;
}

void CUserMng::AddCommonPlace(CCtrl* pCtrl, BYTE nType)
{
	CAr ar;

	ar << GETID(pCtrl) << SNAPSHOTTYPE_COMMONPlACE;
	ar << nType;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pCtrl)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pCtrl)
}

void CUser::AddReturnScroll()
{
	if (IsDelete())	return;


	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_RETURNSCORLL;

}

void CUserMng::OutputStatistics(void)
{
	int	acbUser[MAX_3RD_LEGEND_LEVEL];
	int cb = 0;
	int nTotal = 0;

	memset(acbUser, 0, sizeof(int) * MAX_3RD_LEGEND_LEVEL);
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;

		if (pUser->GetLevel() >= 1 && pUser->GetLevel() <= MAX_3RD_LEGEND_LEVEL)
		{
			acbUser[pUser->GetLevel() - 1]++;
			cb++;
			nTotal += pUser->GetLevel();
		}
	}

	static char lpOutputString[4096];
	*lpOutputString = '\0';
	for (int i = 0; i < MAX_3RD_LEGEND_LEVEL; i++)
	{
		if (acbUser[i] == 0)
			continue;
		char lpString[32] = { 0, };
		sprintf(lpString, "%d\t%d\n", i + 1, acbUser[i]);
		lstrcat(lpOutputString, lpString);
	}

	if (cb > 0)
	{
		char lpOverview[32] = { 0, };
		sprintf(lpOverview, "U=%d, A=%d", cb, nTotal / cb);
		lstrcat(lpOutputString, lpOverview);
	}

	CTime time = CTime::GetCurrentTime();
	FILEOUT(time.Format("../statistics%Y%m%d%H%M%S.txt"), lpOutputString);
}

void CUser::AddPostMail(CMail* pMail)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_POSTMAIL;
	pMail->Serialize(m_Snapshot.ar);

}

void CUser::AddRemoveMail(u_long nMail, int nType)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVEMAIL;
	m_Snapshot.ar << nMail << nType;

}

void CUser::AddMailBox(CMailBox* pMailBox)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX;
	pMailBox->Serialize(m_Snapshot.ar);

}

void CUser::SendCheckMailBoxReq(BOOL bCheckTransMailBox)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUERYMAILBOX_REQ;
	m_Snapshot.ar << bCheckTransMailBox;
}


void CUser::CheckTransMailBox(BOOL bCheckTransMailBox)
{
	m_bCheckTransMailBox = bCheckTransMailBox;
}

BOOL CUser::GetCheckTransMailBox()
{
	return m_bCheckTransMailBox;
}

bool CUser::CheckClientReq()
{
	DWORD dwTick = GetTickCount();
	if (dwTick >= m_dwTickFromClient + CHECK_TICK_FROM_CLIENT)
	{
		m_dwTickFromClient = dwTick;
		++m_nCountFromClient;
		return true;
	}
	return false;
}

void CUser::ResetCheckClientReq()
{
	m_dwTickFromClient = 0;
	m_nCountFromClient = 0;
}

int CUser::GetCountClientReq()
{
	return m_nCountFromClient;
}


void CUser::AddGCWindow(__int64 nPrizePenya, DWORD dwRequstPenya, DWORD dwMinRequestPenya)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_IN_WINDOW;
	m_Snapshot.ar << nPrizePenya << dwRequstPenya << dwMinRequestPenya;

}

void CUser::AddGCRequestStatus(__int64 nPrizePenya, vector<CGuildCombat::__REQUESTGUILD> vecRequestStatus)
{
	DWORD dwRequstPenya = 0;
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_REQUEST_STATUS;
	CTime tNextTime = g_GuildCombatMng.GetNextGuildCobmatTime();
	time_t d = (time_t)(tNextTime.GetTime() - time_null());
	m_Snapshot.ar << d;
	m_Snapshot.ar << nPrizePenya;
	int nSize = vecRequestStatus.size();
	m_Snapshot.ar << nSize;
	for (int i = 0; i < nSize; ++i)
	{
		CGuildCombat::__REQUESTGUILD RequestGuild = vecRequestStatus[i];
		CGuild* pGuild = g_GuildMng.GetGuild(RequestGuild.uidGuild);
		if (pGuild)
			m_Snapshot.ar.WriteString(pGuild->m_szGuild);
		else
			m_Snapshot.ar.WriteString("NotGuild");

		if (m_idGuild == RequestGuild.uidGuild)
			dwRequstPenya = RequestGuild.dwPenya;
	}
	m_Snapshot.ar << dwRequstPenya;

}

void CUser::AddGCSelectPlayerWindow(vector<CGuildCombat::__JOINPLAYER>& vecSelectPlayer, u_long uidDefender, BOOL bWindow, BOOL bRequestWar)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxJoinMember;
	m_Snapshot.ar << g_GuildCombatMng.m_nMaxWarPlayer;

	m_Snapshot.ar << bRequestWar;
	if (bRequestWar)
	{
		m_Snapshot.ar << bWindow;
		if (bWindow)
		{
			m_Snapshot.ar << uidDefender;
			int nSize = vecSelectPlayer.size();
			m_Snapshot.ar << nSize;
			for (int i = 0; i < nSize; ++i)
			{
				CGuildCombat::__JOINPLAYER JoinPlayer = vecSelectPlayer[i];
				m_Snapshot.ar << JoinPlayer.uidPlayer;
			}
		}
	}

}

void CUser::AddGCJoinWarWindow(int nTelTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_SELECTWARPOS;
	m_Snapshot.ar << nTelTime;

}
void CUser::AddGCWinGuild(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WINGUILD;
	m_Snapshot.ar << g_GuildCombatMng.m_nGuildCombatIndex << g_GuildCombatMng.m_uWinGuildId << g_GuildCombatMng.m_nWinGuildCount;

}
void CUser::AddGCBestPlayer(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_BESTPLAYER;
	m_Snapshot.ar << g_GuildCombatMng.m_uBestPlayer;

}
void CUser::AddGCIsRequest(BOOL bRequest)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ISREQUEST;
	m_Snapshot.ar << bRequest;

}
void CUser::AddGCDiagMessage(const char* str)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_DIAGMESSAGE;
	m_Snapshot.ar.WriteString(str);

}
void CUser::AddGCTele(const char* str)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_TELE;
	m_Snapshot.ar.WriteString(str);

}
void CUser::AddGuildCombatNextTime(DWORD dwTime, DWORD dwState)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_NEXTTIMESTATE;
	m_Snapshot.ar << dwTime << dwState;

}
void CUser::AddGuildCombatEnterTime(DWORD dwTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_ENTERTIME;
	m_Snapshot.ar << dwTime;

}

void CUser::AddGCWarPlayerlist()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_WARPLAYERLIST;
	g_GuildCombatMng.SerializeGCWarPlayerList(m_Snapshot.ar);

}


void CUser::AddGuildCombatState(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_GCSTATE;
	m_Snapshot.ar << g_GuildCombatMng.m_nState << g_GuildCombatMng.m_nGCState;

}
void CUser::AddGCLog(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_LOG;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCGetPoint.size();
	for (int veci = 0; veci < (int)(g_GuildCombatMng.m_vecGCGetPoint.size()); ++veci)
	{
		CGuildCombat::__GCGETPOINT GCGetPoint = g_GuildCombatMng.m_vecGCGetPoint[veci];
		m_Snapshot.ar << GCGetPoint.uidGuildAttack;
		m_Snapshot.ar << GCGetPoint.uidGuildDefence;
		m_Snapshot.ar << GCGetPoint.uidPlayerAttack;
		m_Snapshot.ar << GCGetPoint.uidPlayerDefence;
		m_Snapshot.ar << GCGetPoint.nPoint;
		m_Snapshot.ar << GCGetPoint.bKillDiffernceGuild;
		m_Snapshot.ar << GCGetPoint.bMaster;
		m_Snapshot.ar << GCGetPoint.bDefender;
		m_Snapshot.ar << GCGetPoint.bLastLife;
	}

}
void CUser::AddGCPlayerPoint(void)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDCOMBAT;
	m_Snapshot.ar << GC_PLAYERPOINT;
	m_Snapshot.ar << (u_long)g_GuildCombatMng.m_vecGCPlayerPoint.size();
	for (int veci = 0; veci < (int)(g_GuildCombatMng.m_vecGCPlayerPoint.size()); ++veci)
	{
		CGuildCombat::__GCPLAYERPOINT GCPlayerPoint = g_GuildCombatMng.m_vecGCPlayerPoint[veci];
		m_Snapshot.ar << GCPlayerPoint.uidPlayer;
		m_Snapshot.ar << GCPlayerPoint.nJob;
		m_Snapshot.ar << GCPlayerPoint.nPoint;
	}
}
void CUser::AddQuestTextTime(BOOL bFlag, int nState, DWORD dwTime)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_TEXT_TIME;
	m_Snapshot.ar << bFlag;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwTime;

}

void CUser::AddFocusObj(OBJID objid)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_FOCUSOBJ;
	m_Snapshot.ar << objid;

}

void CUser::CheckFiniteItem()
{
	for (int i = 0; i < m_Inventory.GetMax(); i++)
	{
		CItemElem* pItemElem = m_Inventory.GetAtId(i);
		if (IsUsableItem(pItemElem) && pItemElem->IsExpiring() && !pItemElem->IsFlag(CItemElem::expired))
		{
			if (pItemElem->IsEatPet())
			{
				if (IsUsingEatPet(pItemElem))
					InactivateEatPet();

				RemoveItem((BYTE)(pItemElem->m_dwObjId), pItemElem->m_nItemNum);
			}
			else
			{
				ItemProp* pItemProp = pItemElem->GetProp();
				if ((int)(pItemElem->m_dwObjIndex) >= (int)(m_Inventory.GetSize()))	// equiped item
					ResetDestParamEquip(pItemProp, pItemElem);

				pItemElem->SetFlag(CItemElem::expired);
				UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_FLAG, MAKELONG(pItemElem->m_dwObjIndex, pItemElem->m_byFlag));

				if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime > 0 && HasBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId)))
					RemoveBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId));
			}
		}
	}
}

#ifdef __EVE_MINIGAME
void CUser::AddKawibawiboResult(int nResult, int nWinCount, DWORD dwItemId, int nItemCount, DWORD dwNextItemId, int nNextItemCount)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_MINIGAME;
	m_Snapshot.ar << MINIGAME_KAWIBAWIBO_RESUTLT;

	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nWinCount;
	if (nResult == CMiniGame::KAWIBAWIBO_WIN)
	{
		m_Snapshot.ar << dwItemId << dwNextItemId;
		m_Snapshot.ar << nItemCount << nNextItemCount;
		m_Snapshot.ar << prj.m_MiniGame.nKawiBawiBo_MaxWin;
	}
	else if (nResult == CMiniGame::KAWIBAWIBO_GETITEM)
	{
		m_Snapshot.ar << dwItemId;
		m_Snapshot.ar << nItemCount;
	}
}
#endif // __EVE_MINIGAME

#if __VER >= 9 // __ULTIMATE
void CUser::AddUltimateMakeItem(int nResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEITEM;
	m_Snapshot.ar << nResult;
}

void CUser::AddUltimateMakeGem(int nResult, int nNum)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << ULTIMATE_MAKEGEM;
	m_Snapshot.ar << nResult;
	m_Snapshot.ar << nNum;
}

void CUser::AddUltimateWeapon(BYTE state, int nResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ULTIMATE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}
#endif // __ULTIMATE

#ifdef __TRADESYS
void CUser::AddExchangeResult(BYTE state, int nResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_EXCHANGE;
	m_Snapshot.ar << state;
	m_Snapshot.ar << nResult;
}
#endif // __TRADESYS

#if __VER >= 10 // __REMOVE_ATTRIBUTE
void CUser::AddRemoveAttribute(BOOL bSuccess)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REMOVE_ATTRIBUTE;
	m_Snapshot.ar << bSuccess;
}
#endif // __REMOVE_ATTRIBUTE

#if __VER >= 9	// __PET_0410
#ifdef __PET_1024
void CUserMng::AddSetPetName(CUser* pUser, const char* szPetName)
{
	CAr ar;

	ar << GETID(pUser) << SNAPSHOTTYPE_SET_PET_NAME;
	ar.WriteString(szPetName);

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pUser)
}
#endif	// __PET_1024

#ifdef __PET_1024
void CUserMng::AddPetCall(CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel, const char* szPetName)
#else	// __PET_1024
void CUserMng::AddPetCall(CMover* pMover, DWORD dwPetId, DWORD dwIndex, BYTE nPetLevel)
#endif	// __PET_1024
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_PET_CALL;
	ar << dwPetId << dwIndex << nPetLevel;
#ifdef __PET_1024
	ar.WriteString(szPetName);
#endif	// __PET_1024

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddPetRelease(CMover* pMover)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_PET_RELEASE;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddPetLevelup(CMover* pMover, DWORD dwPetId)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_PET_LEVELUP;
	ar << dwPetId;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

void CUserMng::AddPetFeed(CMover* pMover, WORD wEnergy)
{

	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_PET_FEED;
	ar << wEnergy;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif	// __PET_0410

#if __VER >= 13
void CUserMng::AddHonorTitleChange(CMover* pMover, int nChange)
{

	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_HONOR_CHANGE_ACK;
	ar << nChange;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

#endif

#if __VER >= 9	//__AI_0509
void CUserMng::AddSetSpeedFactor(CMover* pMover, FLOAT fSpeedFactor)
{
	CAr ar;

	ar << GETID(pMover) << SNAPSHOTTYPE_SET_SPEED_FACTOR;
	ar << fSpeedFactor;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif	// __AI_0509

#if __VER >= 10
void CUser::AddLegendSkillResult(int nResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_LEGENDSKILLUP_RESULT;
	//	m_Snapshot.ar << LEGENDSKILL_RESULT;
	m_Snapshot.ar << nResult;
}
#endif

#if __VER >= 9 // __EVENTLUA
void CUserMng::AddEventLua(BYTE nId, BOOL bState)
{
	string strDesc = prj.m_EventLua.GetDesc(nId);

	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;
		pUser->AddEventLuaDesc((int)bState, strDesc);
	}
}
#endif // __EVENTLUA


#if __VER >= 11
void CUser::AddGuildBankLogView(BYTE byListType, short nLogCount, const __GUILDBANKLOG_ENTRY* logs)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDLOG_VIEW;
	m_Snapshot.ar << byListType;
	m_Snapshot.ar << nLogCount;

	for (short i = 0; i < nLogCount; ++i)
	{
		TRACE("GuildBankLogView:%s %d %d %d %d\n", logs[i].szPlayer, logs[i].nDate, logs[i].nItemID, logs[i].nItemAbilityOption, logs[i].nItemCount);
		m_Snapshot.ar.WriteString(logs[i].szPlayer);
		m_Snapshot.ar << logs[i].nDate;
		m_Snapshot.ar << logs[i].nItemID;					//  nItemAbilityOption
		if (byListType == GI_LOG_VIEW_ITEM_ADD || byListType == GI_LOG_VIEW_ITEM_REMOVE)
		{
			m_Snapshot.ar << logs[i].nItemAbilityOption;
			m_Snapshot.ar << logs[i].nItemCount;
		}
	}
}
#endif

#if __VER >= 13
void CUser::AddHonorListAck()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HONOR_LIST_ACK;

	for (int i = 0; i < MAX_HONOR_TITLE; ++i)
	{
		m_Snapshot.ar << m_aHonorTitle[i];
	}
}
#endif

#if __VER >= 12 // __SECRET_ROOM
void CUser::AddSecretRoomMngState(int nState, DWORD dwRemainTime)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_MNG_STATE;
	m_Snapshot.ar << nState;
	m_Snapshot.ar << dwRemainTime;
}

void CUser::AddSecretRoomContInfo(BYTE nContinent, BYTE nType, vector<__SECRETROOM_TENDER>& vecSecreetRoomTender, int nIndex)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_INFO;
	m_Snapshot.ar << nContinent;
	m_Snapshot.ar << nType;

	switch (nType)
	{
	case SECRETROOM_GUILDLIST:
	{
		m_Snapshot.ar << vecSecreetRoomTender.size();

		for (int i = 0; i < (int)(vecSecreetRoomTender.size()); i++)
		{
			m_Snapshot.ar << vecSecreetRoomTender[i].dwGuildId;
			m_Snapshot.ar << vecSecreetRoomTender[i].nWarState;
			m_Snapshot.ar << vecSecreetRoomTender[i].nKillCount;

			if (m_idGuild == vecSecreetRoomTender[i].dwGuildId)
			{
				int nSize = vecSecreetRoomTender[i].vecLineUpMember.size();
				m_Snapshot.ar << nSize;

				for (int j = 0; j < nSize; j++)
				{
					m_Snapshot.ar << vecSecreetRoomTender[i].vecLineUpMember[j];
				}
			}
		}

		map<int, int> mapMonsterNum = CSecretRoomMng::GetInstance()->m_mapMonsterNum;
		map<int, int>::iterator it = mapMonsterNum.begin();
		m_Snapshot.ar << mapMonsterNum.size();
		for (; it != mapMonsterNum.end(); it++)
		{
			m_Snapshot.ar << it->first;
			m_Snapshot.ar << it->second;
		}
	}
	break;

	case SECRETROOM_KILLCOUNT:
	{
		m_Snapshot.ar << nIndex;
		m_Snapshot.ar << vecSecreetRoomTender[nIndex].nKillCount;
	}
	break;

	case SECRETROOM_WARSTATE:
	{
		m_Snapshot.ar << nIndex;
		m_Snapshot.ar << vecSecreetRoomTender[nIndex].nWarState;
	}
	break;
	}
}

void CUser::AddSecretRoomTenderOpenWnd(int nTenderPenya)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDEROPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinPenya;
	m_Snapshot.ar << nTenderPenya;
}

void CUser::AddSecretRoomLineUpOpenWnd(vector<DWORD>& vecLineUpMember)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_LINEUPOPENWND;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum;
	m_Snapshot.ar << CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum;
	m_Snapshot.ar << vecLineUpMember.size();

	for (int i = 0; i < (int)(vecLineUpMember.size()); i++)
		m_Snapshot.ar << vecLineUpMember[i];
}

void CUser::AddSecretRoomTenderView(int nTenderPenya, int nRanking, time_t t, vector<__SECRETROOM_TENDER>& vecSRTender)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SECRETROOM_TENDERVIEW;
	m_Snapshot.ar << nTenderPenya;
	m_Snapshot.ar << nRanking;
	m_Snapshot.ar << t;
	m_Snapshot.ar << vecSRTender.size();
	for (int i = 0; i < (int)(vecSRTender.size()); i++)
		m_Snapshot.ar << vecSRTender[i].dwGuildId;
}
#endif // __SECRET_ROOM

#if __VER >= 12 // __LORD
void CUser::AddLord(void)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD;
	CSLord::Instance()->Serialize(m_Snapshot.ar);
}

int CUser::GetMuteText(void)
{
	if (m_dwMute > 0)
		return TID_GAME_STATE_MUTE;
	if (HasBuff(BUFF_ITEM, II_SYS_SYS_LS_RAGE))
		return TID_GAME_LORD_SKILL_RAGE_E001;
	return 0;
}

void CUser::AddLordSkillTick(CLordSkill* pSkills)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LORD_SKILL_TICK;
	pSkills->SerializeTick(m_Snapshot.ar);
}
#endif	// __LORD

#if __VER >= 12 // __LORD
void CUserMng::AddElectionBeginCandidacy(void)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_CANDIDACY;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddElectionBeginVote(int nRequirement)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_BEGIN_VOTE;
	ar << nRequirement;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddElectionEndVote(u_long idPlayer)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_END_VOTE;
	ar << idPlayer;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddElectionAddDeposit(u_long idPlayer, __int64 iDeposit, time_t tCreate)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_ADD_DEPOSIT;
	ar << idPlayer << iDeposit << tCreate;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddElectionSetPledge(u_long idPlayer, const char* szPledge)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_SET_PLEDGE;
	ar << idPlayer;
	ar.WriteString(szPledge);
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddElectionIncVote(u_long idPlayer, u_long idElector)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_ELECTION_INC_VOTE;
	ar << idPlayer << idElector;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddLEventCreate(CLEComponent* pComponent)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_CREATE;
	pComponent->Serialize(ar);
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddLEventInitialize(void)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_INITIALIZE;
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddLEventTick(ILordEvent* pEvent)
{
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_L_EVENT_TICK;
	pEvent->SerializeTick(ar);
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}

void CUserMng::AddLordSkillUse(CUser* pUser, u_long idTarget, int nSkill)
{
	CAr ar;
	ar << GETID(pUser) << SNAPSHOTTYPE_LORD_SKILL_USE << idTarget << nSkill;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pUser)
	{
		USERPTR->AddBlock(lpBuf, nBufSize);
	}
	NEXT_VISIBILITYRANGE(pUser)
}

void CUserMng::AddQueryPlayerData(u_long idPlayer)
{
	if (idPlayer == NULL_ID)
		return;
	PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
	if (!pData)
		return;
	CAr ar;
	ar << NULL_ID << SNAPSHOTTYPE_QUERY_PLAYER_DATA;
	ar << idPlayer;
	ar.WriteString(pData->szPlayer);
	ar.Write(&pData->data, sizeof(sPlayerData));
	GETBLOCK(ar, lpBuf, uBufSize);
	AddBlock(lpBuf, uBufSize);
}
#endif	// __LORD

#if __VER >= 12 // __TAX
void CUser::AddTaxInfo(void)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_ALLINFO;
	CTax::GetInstance()->Serialize(m_Snapshot.ar);
}

void CUser::AddTaxSetTaxRateOpenWnd(BYTE nCont)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_TAX_SETTAXRATE_OPENWND;
	m_Snapshot.ar << nCont;
	m_Snapshot.ar << CTax::GetInstance()->m_nMinTaxRate << CTax::GetInstance()->m_nMaxTaxRate;
}

void CUserMng::AddTaxInfo(void)
{
	map<DWORD, CUser*>::iterator it;
	for (it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() == FALSE)
			continue;
		pUser->AddTaxInfo();
	}
}
#endif // __TAX

#if __VER >= 13 // __HOUSING
void CUser::AddHousingAllInfo(CHousing* pHousing)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_ALLINFO;
	pHousing->Serialize(m_Snapshot.ar);
}

void CUser::AddHousingSetFurnitureList(HOUSINGINFO& housingInfo, BOOL bAdd)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_FURNITURELIST;
	housingInfo.Serialize(m_Snapshot.ar);
	m_Snapshot.ar << bAdd;
}

void CUser::AddHousingSetupFurniture(HOUSINGINFO* pHousingInfo)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETUPFURNITURE;
	pHousingInfo->Serialize(m_Snapshot.ar);
}

void CUser::AddHousingPaperingInfo(DWORD dwItemId, BOOL bSetup)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_PAPERINGINFO;
	m_Snapshot.ar << dwItemId;
	m_Snapshot.ar << bSetup;
}

void CUserMng::AddHousingPaperingInfo(DWORD dwItemId, BOOL bSetup, DWORD dwMasterId)
{
	for (map<DWORD, CUser*>::iterator it = m_users.begin(); it != m_users.end(); ++it)
	{
		CUser* pUser = it->second;
		if (pUser->IsValid() && pUser->GetWorld() && pUser->GetWorld()->GetID() == WI_WORLD_MINIROOM)
		{
			if (pUser->GetLayer() == static_cast<int>(dwMasterId))
				pUser->AddHousingPaperingInfo(dwItemId, bSetup);
		}
	}
}

void CUser::AddHousingSetVisitAllow(DWORD dwTargetId, BOOL bAllow)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_SETVISITALLOW;
	m_Snapshot.ar << dwTargetId << bAllow;
}

void CUser::AddHousingVisitableList(vector<DWORD>& vecVisitable)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_HOUSING_VISITABLELIST;
	m_Snapshot.ar << vecVisitable.size();
	for (int i = 0; i < (int)(vecVisitable.size()); i++)
		m_Snapshot.ar << vecVisitable[i];
}
#endif // __HOUSING

#if __VER >= 13 // __QUEST_HELPER
void CUser::AddNPCPos(const D3DXVECTOR3& vPos)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUESTHELPER_NPCPOS;
	m_Snapshot.ar << vPos;
}
#endif // __QUEST_HELPER

BOOL CUser::IsGuildMaster()
{
	if (GetGuild() && GetGuild()->IsMaster(m_idPlayer))
		return TRUE;

	return FALSE;
}

#ifdef __AZRIA_1023
int CUser::DoUseItemInput(ItemProp* pProp, CItemElem* pItem)
{
	switch (pProp->dwItemKind3)
	{
	case IK3_TICKET:
		DoUseItemTicket(pItem);
		return 0;
	default:	break;
	}
	switch (pProp->dwID)
	{
#ifdef __PET_1024
	case II_SYS_SYS_SCR_PET_NAMING:
		return DoUseItemPetNaming();
#endif	// __PET_1024
	default:
		return 1;
	}
	return 1;
}
#endif	// __AZRIA_1023

#ifdef __PET_1024
int CUser::DoUseItemPetNaming()
{
	CPet* pPet = GetPet();
	if (!pPet)
	{
		AddDefinedText(TID_GAME_NAME_PET_E00);
		return 1;
	}

	if (IsInvalidGameName(GetInput()))
	{
		AddDefinedText(TID_GAME_NAME_PET_E01);
		return 1;
	}

	pPet->SetName(GetInput());
	g_UserMng.AddSetPetName(this, pPet->GetName());
	return 0;
}
#endif	// __PET_1024

#if __VER >= 14 // __SMELT_SAFETY
void CUser::AddSmeltSafety(BYTE nResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_SMELT_SAFETY;
	m_Snapshot.ar << nResult;
}
#endif // __SMELT_SAFETY

#ifdef __MAP_SECURITY
void CUser::AddWorldReadInfo(DWORD dwWorldId, D3DXVECTOR3 vPos)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_WORLD_READINFO;
	m_Snapshot.ar << dwWorldId << vPos;
}
#endif // __MAP_SECURITY

#if __VER >= 15 // __PETVIS
void CUser::AddActivateVisPet(OBJID objIdVisPetItem, OBJID objIdVisPetId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_VISPET_ACTIVATE;
	m_Snapshot.ar << objIdVisPetItem << objIdVisPetId;
}

void CUserMng::AddChangeMoverSfxId(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_MOVER_CHANGESFX;
	ar << pMover->m_dwMoverSfxId;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif // __PETVIS

#if __VER >= 15 // __GUILD_HOUSE
void CUser::AddGuildHousePakcet(int nPacketType, GH_Fntr_Info& gfi, int nIndex)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_PACKET;
	m_Snapshot.ar << nPacketType << nIndex;
	gfi.Serialize(m_Snapshot.ar);
}

void CUser::AddGuildHouseAllInfo(CGuildHouseBase* pGuildHouse)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_ALLINFO;
	m_Snapshot.ar << static_cast<BOOL>(g_eLocal.GetState(ENABLE_GUILD_INVENTORY));
	if (pGuildHouse)
	{
		m_Snapshot.ar << TRUE;
		pGuildHouse->SerializeAllInfo(m_Snapshot.ar);
	}
	else
		m_Snapshot.ar << FALSE;
}

void CUser::AddGuildHouseRemvoe()
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDHOUSE_REMOVE;
}

BOOL CUserMng::HasUserSameWorldnLayer(CUser* pUserSrc)
{
	if (!IsValidObj(pUserSrc) || !pUserSrc->GetWorld())
		return FALSE;

	for (map<DWORD, CUser*>::iterator it = m_users.begin(); it != m_users.end(); it++)
	{
		if ((pUserSrc != it->second) && (pUserSrc->GetWorld() && it->second->GetWorld())
			&& (pUserSrc->GetWorld()->GetID() == it->second->GetWorld()->GetID()) && (pUserSrc->GetLayer() && it->second->GetLayer()))
			return TRUE;
	}

	return FALSE;
}
#endif // __GUILD_HOUSE

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void CUser::AddCheckedQuest()
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_QUEST_CHECKED;
	m_Snapshot.ar << this->m_nCheckedQuestSize;
	if (this->m_nCheckedQuestSize)
		m_Snapshot.ar.Write(this->m_aCheckedQuest, sizeof(WORD) * this->m_nCheckedQuestSize);
}
#endif // __IMPROVE_QUEST_INTERFACE


void	CUser::SelectAwakeningValue(DWORD dwItemObjID, DWORD dwSerialNumber, BYTE bySelectFlag)
{
	CItemElem* pItem = NULL;
	pItem = (m_Inventory.GetAtId(dwItemObjID));

	if (pItem == NULL)
	{
		WriteLog("pItem is NULL in SelectAwakeningValue function. ItemObjID : [%d]", dwItemObjID);

		return;
	}

	DWORD	dwItemSerialNumber = 0;
	dwItemSerialNumber = (DWORD)(pItem->GetSerialNumber());

	if (dwItemSerialNumber != dwSerialNumber)
	{
		WriteLog("Item SerialNumber is different. ItemSerialNumber : [%d] [%d]", dwItemSerialNumber, dwSerialNumber);

		return;
	}

	if (pItem->GetNewRandomOption() == 0)
	{
		WriteLog("This item have not new random option.");

		return;
	}

	bool	bRetValue = false;

	bRetValue = pItem->SelectRandomOption(bySelectFlag);

	if (bRetValue == true)
	{
		UpdateItemEx((BYTE)(pItem->m_dwObjId), UI_RANDOMOPTITEMID, pItem->GetRandomOptItemId());


		//WriteLog( "UserID [%d] : ItemSerialNumber[%d] SelectAwakeningValue bySelectFlag[%d]", (int)( GetId() ), (int)( pItem->GetSerialNumber() ), (int)( bySelectFlag ) );

		TCHAR	szSelectValue[128];
		::memset(szSelectValue, 0, sizeof(szSelectValue));

		if (bySelectFlag == 1)			//	_AWAKE_OLD_VALUE
		{
			::_stprintf(szSelectValue, "SELECT _AWAKE_OLD_VALUE");
		}
		else if (bySelectFlag == 2)	//	_AWAKE_NEW_VALUE
		{
			::_stprintf(szSelectValue, "SELECT _AWAKE_NEW_VALUE");
		}
		else
		{
			::_stprintf(szSelectValue, "SELECT ERROR");
		}

#ifndef __FL_FIX_USELESS_LOGS
		g_DPSrvr.PutItemLog(this, "z", szSelectValue, pItem, 1);
#endif

	}

	return;
}

void	CUser::SendNewRandomOption(unsigned char id, DWORD dwSerialNumber, __int64 n64NewRandomOption)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_ITEM_SELECT_AWAKENING_VALUE;
	m_Snapshot.ar << id << dwSerialNumber << n64NewRandomOption;
}


#ifdef __FL_DUNGEON_TREASURE
void CUser::AddDungeonTreasureWnd(BOOL bOpen)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_DUNGEONTREASURE_WND;
	m_Snapshot.ar << bOpen;
}
void CUser::AddDungeonTreasureResult(int nSelected, BOOL bResult)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_DUNGEONTREASURE_RESULT;
	m_Snapshot.ar << nSelected;
	m_Snapshot.ar << bResult;
}
#endif

#ifdef __FL_BATTLE_PASS
void CUser::AddBattlePassTime()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_BATTLE_PASS_TIME;

	CTime tEndTime = CBattlePass::GetInstance()->GetTimeEnd();
	time_t dEnd = (time_t)(tEndTime.GetTime() - time_null());

	m_Snapshot.ar << dEnd;
}
void CUser::AddBattlePassState(int nBPLvl, int nBPExp)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_BATTLE_PASS;
	m_Snapshot.ar << nBPLvl << nBPExp;
}
#endif

#ifdef __FL_CONSUMABLES_STORAGE
void CUser::AddPutItemToChest(CItemElem* pItemElem)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PUT_ITEM_TO_CHEST;
	pItemElem->Serialize(m_Snapshot.ar);
}
void CUser::AddMoveItemInsideChest(BYTE nSrcIndex, BYTE nDestIndex)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MOVE_ITEM_INSIDECHEST;
	m_Snapshot.ar << nSrcIndex << nDestIndex;
}
#endif

void CUserMng::AddSMMode(CMover* pMover, int nType, DWORD dwTime)
{
	if (!IsValidObj(pMover))
		return;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_SM_MODE;

	DWORD BufSMTime = 0;
	if (dwTime != 0)
	{
		if (g_AddSMMode.bSMModetime[nType])
		{
			CTime tSMtime(pMover->m_dwSMTime[nType]);
			CTimeSpan ct = tSMtime - CTime::GetCurrentTime();
			BufSMTime = (DWORD)(ct.GetTotalSeconds());
		}
		else
			BufSMTime = pMover->m_dwSMTime[nType];
	}
	else
		BufSMTime = 0;

	ar << nType;
	ar << BufSMTime;

	if (nType < 0 || SM_MAX <= nType)
		WriteLog("SMMODE OVERFLOW nType : %d", nType);

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}


void CUserMng::AddStatInfo(CMover* pMover, BOOL bRegen)
{
	if (!IsValidObj(pMover))
		return;

	if (bRegen)
		pMover->SetHitPoint(pMover->GetMaxHitPoint());

	if (bRegen || pMover->IsSMMode(SM_MAINTAIN_MP))
		pMover->SetManaPoint(pMover->GetMaxManaPoint());

	if (bRegen || pMover->IsSMMode(SM_MAINTAIN_FP))
		pMover->SetFatiguePoint(pMover->GetMaxFatiguePoint());

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_UPDATE_STATINFO;
	ar << pMover->m_nStr << pMover->m_nSta << pMover->m_nDex << pMover->m_nInt;
	ar << pMover->m_nRemainGP;
	ar << bRegen;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}

#ifdef __FL_FAST_ELEMENT
void	CUserMng::AddElementChange(CMover* pMover, int nPosition, int nOpt)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_ADDREDOEQUIP;
	ar << nPosition << nOpt;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)

}
#endif
#ifdef __FL_SHOP_SEARCH
void CUser::AddShopSearch()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ADD_SHOP_SEARCH;

	m_Snapshot.ar << (u_long)(prj.m_setPrivateShops.size());
	for (auto& it : prj.m_setPrivateShops)
	{
		CMover* pShopOwner = prj.GetMover(it);
		if (!IsValidObj(pShopOwner))
		{
			Error("Invalid shop owner in shop search system, return");
			return;
		}

		m_Snapshot.ar << it;
		m_Snapshot.ar.WriteString(pShopOwner->GetName());
		m_Snapshot.ar.WriteString(pShopOwner->m_vtInfo.GetTitle());

		u_long uOffset = m_Snapshot.ar.GetOffset();
		BYTE nVendorItem = 0;

		m_Snapshot.ar << nVendorItem;
		for (int i = 0; i < MAX_VENDITEM; i++)
		{
			CItemElem* pItemElem = pShopOwner->m_vtInfo.GetItem(i);
			if (!pItemElem)
				continue;

			pItemElem->Serialize(m_Snapshot.ar);
			m_Snapshot.ar << pItemElem->GetExtra();
			m_Snapshot.ar << pItemElem->m_nCost;
#ifdef __FL_VENDORS
			m_Snapshot.ar << pItemElem->m_nPerinCost;
#endif
			nVendorItem++;
		}

		GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
		*(UNALIGNED BYTE*)(lpBlock + uOffset) = nVendorItem;
	}
}
#endif


void	CUser::SummonPlayer(CUser* pDestUser)
{
	if (IsDie() == TRUE)
	{
		return;
	}

	if (IsValidObj(pDestUser) == FALSE)
	{
		return;
	}

	CWorld* pWorld = GetWorld();
	if (pWorld == NULL)
	{
		return;
	}

	const int nLayer = GetLayer();
	if (pWorld->m_linkMap.GetLinkMap(nLayer) == NULL)
	{
		return;
	}

	const REPLACE_TYPE nReplaceType = IsAuthHigher(AUTH_GAMEMASTER) ? REPLACE_FORCE : REPLACE_NORMAL;
	pDestUser->REPLACE(g_uIdofMulti, pWorld->GetID(), GetPos(), nReplaceType, nLayer);
}

#ifdef __FL_EQUALIZED_MODE
void CUser::AddOpenTempWindow(int nType)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SELECT_TMP_CLASS;
	m_Snapshot.ar << nType;

}
void CUser::AddInvAndBuffsSerialize(bool bWithBuffs)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SERIALIZE_INVENTORY;
	((CMover*)this)->m_Inventory.Serialize(m_Snapshot.ar);

	m_Snapshot.ar << bWithBuffs;
	if (bWithBuffs)
		((CMover*)this)->m_buffs.Serialize(m_Snapshot.ar);

}
void CUserMng::AddUpdateEquipInfo(CMover* pMover)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_ADDUPDATEEQUIPFORNEAR;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		ar << pMover->m_aEquipInfo[i].dwId;
		ar << pMover->m_aEquipInfo[i].nOption;
#ifdef __MODEL_CHANGE
		ar << pMover->m_aEquipInfo[i].dwLook;
#endif // 
	}

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR != pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif

#ifdef __FL_FIX_IDCHANGE_EXPLOIT
void CUser::ProcessInvites()
{
	for (auto j = vecPartyInvites.begin(); j != vecPartyInvites.end();)
	{
		if (GetTickCount() > j->dwTimeEnd)
		{
			CUser* pUserTarget = (CUser*)prj.GetUserByID(j->dwInvieMember);
			if (pUserTarget)
			{
				CString str;
				str.Format("Your party invite for player %s was expired.", pUserTarget->GetName());
				AddText(str, 0xffffffff);
			}

			j = vecPartyInvites.erase(j);
		}
		else
			++j;
	}

	for (auto j = vecGuildInvites.begin(); j != vecGuildInvites.end();)
	{
		if (GetTickCount() > j->dwTimeEnd)
		{
			CUser* pUserTarget = (CUser*)prj.GetUserByID(j->dwInvieMember);
			if (pUserTarget)
			{
				CString str;
				str.Format("Your guild invite for player %s was expired.", pUserTarget->GetName());
				AddText(str, 0xffffffff);
			}

			j = vecGuildInvites.erase(j);
		}
		else
			++j;
	}

	for (auto j = vecFriendInvites.begin(); j != vecFriendInvites.end();)
	{
		if (GetTickCount() > j->dwTimeEnd)
		{
			CUser* pUserTarget = (CUser*)prj.GetUserByID(j->dwInvieMember);
			if (pUserTarget)
			{
				CString str;
				str.Format("Your friend invite for player %s was expired.", pUserTarget->GetName());
				AddText(str, 0xffffffff);
			}

			j = vecFriendInvites.erase(j);
		}
		else
			++j;
	}
}
#endif

#ifdef __FL_PARTY_SORT
void CUser::AddPartyChangePosition(u_long uidChangeMember, BOOL bUpDown)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ADDPARTYCHANGEPOS;
	m_Snapshot.ar << uidChangeMember;
	m_Snapshot.ar << bUpDown;
}
#endif

#ifdef __FL_GW_STATUE_V2
void CUser::AddStatueParams(int nCost)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_STATUE_PARAMS;
	m_Snapshot.ar << nCost;
}
#endif

#ifdef __FL_FFA
void CUser::AddFFAData()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FFA_DATA;

	auto pManager = CFFAManager::GetInstance();
	m_Snapshot.ar << pManager->GetPlayerCount();
	m_Snapshot.ar << pManager->GetMaxPlayers();
	m_Snapshot.ar << pManager->GetJoinFee();
	m_Snapshot.ar << pManager->GetRewardAmount();
	m_Snapshot.ar << pManager->GetRewardID();
	m_Snapshot.ar << pManager->GetNextFFA();
	m_Snapshot.ar << (int)(24 - CTime::GetCurrentTime().GetHour());
}
void CUser::AddFFAState(BOOL bPlayerList)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FFA_STATE;

	auto pManager = CFFAManager::GetInstance();
	m_Snapshot.ar << pManager->GetCurrentState();
	m_Snapshot.ar << pManager->GetNextStateTime() - time_null();
	m_Snapshot.ar << bPlayerList;

	if (bPlayerList)
	{
		auto pMapPlayers = pManager->GetPlayerList();
		m_Snapshot.ar << pMapPlayers->size();
		for (auto& it : *pMapPlayers)
		{
			m_Snapshot.ar << it.first;
			m_Snapshot.ar << it.second.nKils;
			m_Snapshot.ar << pManager->GetMaxLifes() - it.second.nLifes;
		}
	}
}
void CUser::AddFFAKill(u_long idKiller, u_long idDead)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_FFA_KILL;

	m_Snapshot.ar << idKiller;
	m_Snapshot.ar << idDead;
}
#endif

#ifdef __FL_DAILY_REWARDS
void CUser::AddDailyReward()
{
	if (IsDelete())	return;

	auto itToday = prj.mapDailyRewards.find(m_nLastDailyReward);
	if (itToday == prj.mapDailyRewards.end())
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_ADD_DAILY_REWARD;
	m_Snapshot.ar << itToday->first;

	// Today reward
	m_Snapshot.ar << itToday->second.dwItemID;
	m_Snapshot.ar << itToday->second.nItemNum;
	m_Snapshot.ar << itToday->second.bItemBind;

	// Tomorrow reward
	DWORD dwTomorrowReward = NULL_ID;
	int nTomorrowAmount = 0;
	BOOL bTomorrowBind = FALSE;

	auto itTomorrow = prj.mapDailyRewards.find(m_nLastDailyReward + 1);
	if (itTomorrow == prj.mapDailyRewards.end())
		itTomorrow = prj.mapDailyRewards.find(0);

	if (itTomorrow != prj.mapDailyRewards.end())
	{
		dwTomorrowReward = itTomorrow->second.dwItemID;
		nTomorrowAmount = itTomorrow->second.nItemNum;
		bTomorrowBind = itTomorrow->second.bItemBind;
	}

	m_Snapshot.ar << dwTomorrowReward;
	m_Snapshot.ar << nTomorrowAmount;
	m_Snapshot.ar << bTomorrowBind;
}
#endif

#ifdef __MODEL_HIDEFASH
void CUserMng::AddHideFashionUpdate(CMover* pMover, int nPos, BOOL bState)
{
	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_HIDE_FASH;
	ar << nPos;
	ar << bState;

	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
#endif

#ifdef __MODEL_CHANGE
void CUser::AddLookChange(DWORD dwID, DWORD dwLook)
{
	if (IsDelete())
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_LOOKCHANGE;
	m_Snapshot.ar << dwID << dwLook;
}
#endif

#ifdef __FL_PARTY_FINDER
void CUser::AddPartyFinder(vector<PARTY_LIST>* vecParties)
{
	if (IsDelete())
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYFINDER;

	m_Snapshot.ar << (u_long)(vecParties->size());
	for (auto& it : *vecParties)
	{
		// Leader info
		m_Snapshot.ar << it.m_dwLeaderId;
		m_Snapshot.ar << it.m_nLeaderLevel;
		m_Snapshot.ar << it.m_nLeaderJob;
		m_Snapshot.ar.WriteString(it.m_strLeaderName);

		// Party info
		m_Snapshot.ar << it.m_dwPartyId;
		m_Snapshot.ar << it.m_nPartyMembers;
		m_Snapshot.ar << it.m_nPartyLevel;
		m_Snapshot.ar << it.m_nPartyPoint;

		// Skills/Scrolls
		m_Snapshot.ar << it.m_bLink;
		m_Snapshot.ar << it.m_bDrop;
		m_Snapshot.ar << it.m_bGift;
		m_Snapshot.ar << it.m_bResting;
		m_Snapshot.ar << it.m_bRedScroll;
		m_Snapshot.ar << it.m_bOrangeScroll;
		m_Snapshot.ar << it.m_bGreenScroll;
	}
}
void CUser::AddPartyChangeJoinMode(BOOL bMode)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_PARTYJOINMODE;
	m_Snapshot.ar << bMode;
}
#endif

#ifndef __FL_VENDORS
void CUser::AddRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << iIndex << nType << nId << nNum << nCost;
}
#else 
void CUser::AddRegisterPVendorItem(BYTE iIndex, BYTE nType, BYTE nId, short nNum, int nCost, int nPerinCost)
{
	if (IsDelete())	return;
	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_REGISTER_PVENDOR_ITEM;
	m_Snapshot.ar << iIndex << nType << nId << nNum << nCost << nPerinCost;
}
void CUserMng::AddNewItemToLiveVendor(CMover* pMover, BYTE iIndex)
{
	CItemElem* pItemBase = pMover->m_vtInfo.GetItem(iIndex);
	if (!pItemBase)
		return;

	CItemElem* pItemElem = pItemBase;

	CAr ar;
	ar << GETID(pMover) << SNAPSHOTTYPE_VENDOR_ADDNEWITEM;
	ar << iIndex;
	pItemElem->Serialize(ar);
	ar << pItemElem->GetExtra();
	ar << pItemElem->m_nCost;
	ar << pItemElem->m_nPerinCost;
	GETBLOCK(ar, lpBuf, nBufSize);

	FOR_VISIBILITYRANGE(pMover)
		if (USERPTR == pMover || USERPTR->m_vtInfo.GetOther() == pMover)
			USERPTR->AddBlock(lpBuf, nBufSize);
	NEXT_VISIBILITYRANGE(pMover)
}
void CUser::ActivateClientVendor()
{
	if (IsDelete())
		return;

	auto it = prj.m_mapPlayerShops.find(m_idPlayer);
	if (it == prj.m_mapPlayerShops.end())
		return;

	CMover* pMoverVendor = prj.GetMover(it->second);
	if (!IsValidObj(pMoverVendor))
		return;

	DWORD dwVendorTime = pMoverVendor->GetVendorTime();
	DWORD dwVendorItemId = pMoverVendor->GetVendorItemId();
	dwVendorTime -= time(0);

	m_Snapshot.cb++;
	m_Snapshot.ar << GetId();
	m_Snapshot.ar << SNAPSHOTTYPE_VENDOR_ACTIVATE;
	m_Snapshot.ar << dwVendorTime;
	m_Snapshot.ar << dwVendorItemId;
}
void CUser::AddVendorPreview(CMover* pMover)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_VENDOR_PREVIEW;

	u_long uOffset = m_Snapshot.ar.GetOffset();
	BYTE nVendorItem = 0;

	m_Snapshot.ar << nVendorItem;
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemElem = pMover->m_vtInfo.GetItem(i);
		if (pItemElem == NULL)
			continue;

		m_Snapshot.ar << (BYTE)i;
		pItemElem->Serialize(m_Snapshot.ar);
		m_Snapshot.ar << pItemElem->GetExtra();
		m_Snapshot.ar << pItemElem->m_nCost;
		m_Snapshot.ar << pItemElem->m_nPerinCost;

		nVendorItem++;
	}
	m_Snapshot.ar.WriteString(pMover->m_vtInfo.GetTitle());

	GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
	*(UNALIGNED BYTE*)(lpBlock + uOffset) = nVendorItem;
}
#endif

#ifdef __FL_CASH_SHOP
void CUser::AddCShop()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_CSHOP_ADD;
	CFlyFFShop::GetInstance()->SerializeFlyffShop(m_Snapshot.ar);
}
void CUser::AddCShopBalance(int nDonate, int nVote)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_CSHOP_BALANCE;
	m_Snapshot.ar << nDonate << nVote;
}
#endif

#ifdef __FL_SWITCH_EQ
void CUser::AddSwitchAdd(int nSel, const char* lpszName, DWORD dwLooter, DWORD dwPet, DWORD dwRegen)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SWITCH_ADD;
	m_Snapshot.ar << nSel;
	m_Snapshot.ar.WriteString(lpszName);
	m_Snapshot.ar << dwLooter;
	m_Snapshot.ar << dwPet;
	m_Snapshot.ar << dwRegen;
}
void CUser::AddSwitchDelete(int nSel)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SWITCH_DELETE;
	m_Snapshot.ar << nSel;
}
void CUser::AddSwitches()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SWITCHES_ALL;
	m_Switches.Serialize(m_Snapshot.ar);
}
#endif

#ifdef __FL_GUILD_FINDER
void CUser::AddGuildFinder()
{
	if (IsDelete())
		return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILDFINDER;

	u_long uOffset = m_Snapshot.ar.GetOffset();
	int nTotal = 0;
	m_Snapshot.ar << nTotal;

	CGuildHouseBase* pGHouse = NULL;
	CUser* pLeader = NULL;
	int nLeaderLevel = 1;
	int nLeaderJob = 1;
	CString strLeaderName = "Empty";

	for (auto& it : g_GuildMng.m_mapPGuild)
	{
		CGuild* pGuild = (CGuild*)it.second;
		if (!pGuild)
			continue;

		if (!pGuild->m_bFinder)
			continue;

		m_Snapshot.ar << pGuild->GetGuildId();

		pGHouse = CGuildHouseMng::GetInstance()->GetGuildHouse(pGuild->GetGuildId());
		if (pGHouse)
			m_Snapshot.ar << (BOOL)TRUE;
		else
			m_Snapshot.ar << (BOOL)FALSE;

		m_Snapshot.ar << pGuild->GetSize();
		m_Snapshot.ar << pGuild->m_nLevel;

		pLeader = g_UserMng.GetUserByPlayerID(pGuild->m_idMaster);
		if (IsValidObj(pLeader))
		{
			nLeaderLevel = pLeader->GetLevel();
			nLeaderJob = pLeader->GetJob();
			strLeaderName = pLeader->GetName();
		}
		else
		{
			PlayerData* pPlayerDataLeader = CPlayerDataCenter::GetInstance()->GetPlayerData(pGuild->m_idMaster);
			if (pPlayerDataLeader)
			{
				nLeaderLevel = pPlayerDataLeader->data.nLevel;
				nLeaderJob = pPlayerDataLeader->data.nJob;
				strLeaderName = pPlayerDataLeader->szPlayer;
			}
			else
			{
				nLeaderLevel = 1;
				nLeaderJob = JOB_VAGRANT;
				strLeaderName = "Empty";
			}
		}

		m_Snapshot.ar << nLeaderLevel;
		m_Snapshot.ar << nLeaderJob;
		m_Snapshot.ar.WriteString(strLeaderName);

		// Furniture info
		u_long uOffset2 = m_Snapshot.ar.GetOffset();
		int nTotal2 = 0;
		m_Snapshot.ar << nTotal2;

		if (pGHouse)
		{
			ItemProp* pProp;
			for (int i = 0; i < pGHouse->GetFurnitureListSize(); i++)
			{
				GH_Fntr_Info* pInfo = pGHouse->GetFurnitureInfoPtr(i);
				if (!pInfo)
					continue;

				if (!pInfo->bSetup)
					continue;

				pProp = prj.GetItemProp(pInfo->dwItemId);
				if (!pProp || pProp->dwItemKind2 == IK2_GUILDHOUSE_NPC)
					continue;

				m_Snapshot.ar << pInfo->dwItemId;

				nTotal2++;
			}
		}

		GETBLOCK(m_Snapshot.ar, lpBlock2, nBlockSize2);
		*(UNALIGNED int*)(lpBlock2 + uOffset2) = nTotal2;
		// Furniture info end

		nTotal++;
	}

	GETBLOCK(m_Snapshot.ar, lpBlock, nBlockSize);
	*(UNALIGNED int*)(lpBlock + uOffset) = nTotal;
}
void CUser::AddSetGuildFinderState(BOOL bFinder)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_FINDERSTATE;
	m_Snapshot.ar << bFinder;

}
void CUser::AddGuildFinderAddon(CGuild* pGuild)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_GUILD_FINDERADDON;
	m_Snapshot.ar << pGuild->GetGuildId();
	m_Snapshot.ar.WriteString(pGuild->m_szNotice);
	m_Snapshot.ar << pGuild->m_mapPMember.size();

	CUser* pUserMember;
	CString strMemberName;
	int nMemberLv;
	int nMemberJob;
	for (auto& it : pGuild->m_mapPMember)
	{
		pUserMember = g_UserMng.GetUserByPlayerID(it.second->m_idPlayer);
		if (IsValidObj(pUserMember))
		{
			strMemberName = pUserMember->GetName();
			nMemberLv = pUserMember->GetLevel();
			nMemberJob = pUserMember->GetJob();
		}
		else
		{
			PlayerData* pPlayerDataLeader = CPlayerDataCenter::GetInstance()->GetPlayerData(it.second->m_idPlayer);
			if (pPlayerDataLeader)
			{
				strMemberName = pPlayerDataLeader->szPlayer;
				nMemberLv = pPlayerDataLeader->data.nLevel;
				nMemberJob = pPlayerDataLeader->data.nJob;
			}
			else
			{
				strMemberName = "Empty";
				nMemberLv = 1;
				nMemberJob = JOB_VAGRANT;
			}
		}

		m_Snapshot.ar.WriteString(strMemberName);
		m_Snapshot.ar << nMemberLv;
		m_Snapshot.ar << nMemberJob;
	}
}
#endif

#ifdef __FL_PACKED_ITEMS
void CUser::AddPackageOpen(DWORD dwObjId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PACKAGE_OPEN;
	m_Snapshot.ar << dwObjId;
}
void CUser::AddPackageAdd(DWORD dwPackObjId, DWORD dwItemObjId)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PACKAGE_ADD;
	m_Snapshot.ar << dwPackObjId << dwItemObjId;
}
void CUser::AddPackageRestore(DWORD dwObjId, int nCurSel)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_PACKAGE_RESTORE;
	m_Snapshot.ar << dwObjId << nCurSel;
}
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
void CUser::AddWof()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_WOF;
	CWheelOfFortune::GetInstance()->Serialize(m_Snapshot.ar);
}
void CUser::AddWofInit(int nPrize)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_WOF_INIT;
	m_Snapshot.ar << nPrize;
}
void CUser::AddWofFinish(WOF_ITEM* pItem)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_WOF_FINISH;
	m_Snapshot.ar << pItem->dwItemId;
	m_Snapshot.ar << pItem->nItemCount;
}
#endif

void CUserMng::KickAllPlayers()
{
	g_DPCoreClient.SendSystem("All players disconnected.");

	for (auto& it : g_UserMng.m_users)
	{
		CUser* pUser = it.second;
		if (!IsValidObj(pUser))
			continue;

		if (!pUser->m_bValid)
			continue;

		if (pUser->m_idPlayer == NULL_ID || pUser->m_idPlayer == 0)
			continue;

		if (pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			if (pUser->GetWorld())
				g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

			continue;
		}

		g_DPSrvr.QueryDestroyPlayer(pUser->m_Snapshot.dpidCache, pUser->m_Snapshot.dpidUser, pUser->m_dwSerial, pUser->m_idPlayer);
	}
}
#ifdef __FL_RECORD_BOOK
void CUser::AddRecordBook()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_RECORD_BOOK;
	CRecordBook::GetInstance()->SerializeAllRecords(m_Snapshot.ar);
}
#endif
#ifdef __FL_MONSTER_ALBUM
void CUser::AddMAAddCardPiece(DWORD dwMobID, int nAddedPieces)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MA_ADDCARDPIECE;
	m_Snapshot.ar << dwMobID;
	m_Snapshot.ar << nAddedPieces;
}
void CUser::AddMABossCardSlotUnlock(int nSlot)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MA_BOSSCARDSLOT_UNLOCK;
	m_Snapshot.ar << nSlot;
}
void CUser::AddMABossCardSlotInsert(int nSlot, DWORD dwBossCardID)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MA_BOSSCARDSLOT_INSERT;
	m_Snapshot.ar << nSlot;
	m_Snapshot.ar << dwBossCardID;
}
void CUser::AddMABossCardSlotRestore(int nSlot)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_MA_BOSSCARDSLOT_RESTORE;
	m_Snapshot.ar << nSlot;
}
#endif

#ifdef __FL_TDM
void CUser::AddTDMData()
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TDM_DATA;

	auto pManager = CTDMManager::GetInstance();
	m_Snapshot.ar << pManager->GetNextTDM();
	m_Snapshot.ar << (int)(24 - CTime::GetCurrentTime().GetHour());
	m_Snapshot.ar << pManager->GetMaxPlayers();

	auto pVecRegisteredPlayers = pManager->GetRegisteredPlayerList();
	m_Snapshot.ar << pVecRegisteredPlayers->size();
	for (auto& it : *pVecRegisteredPlayers)
		m_Snapshot.ar << it;
}
void CUser::AddTDMRegister(CUser* pUser)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TDM_REGISTER;
	m_Snapshot.ar << pUser->m_idPlayer;
	m_Snapshot.ar.WriteString(pUser->GetName());
}
void CUser::AddTDMState(bool bPlayerList)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TDM_STATE;

	auto pManager = CTDMManager::GetInstance();
	m_Snapshot.ar << pManager->GetCurrentState();
	m_Snapshot.ar << pManager->GetNextStateTime() - time_null();
	m_Snapshot.ar << bPlayerList;

	if (bPlayerList)
	{
		auto pMapPlayers = pManager->GetPlayerList();
		m_Snapshot.ar << pMapPlayers->size();
		for (auto& it : *pMapPlayers)
		{
			m_Snapshot.ar << it.first;
			m_Snapshot.ar << it.second.team;
			m_Snapshot.ar << it.second.nPoints;
			m_Snapshot.ar << pManager->GetMaxLifes() - it.second.nLifes;
		}

		auto pMapTeams = pManager->GetTeamList();
		m_Snapshot.ar << pMapTeams->size();
		for (auto& it : *pMapTeams)
		{
			m_Snapshot.ar << it.first;
			m_Snapshot.ar << it.second;
		}
	}
}
void CUser::AddTDMKill(u_long idKiller, u_long idDead)
{
	if (IsDelete())	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_TDM_KILL;

	m_Snapshot.ar << idKiller;
	m_Snapshot.ar << idDead;
}
#endif