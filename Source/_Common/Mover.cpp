#include "stdafx.h"
#include "..\_AIInterface\ActionMover.h"
#include "Mover.h"
#include "ModelObject.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineObj.h"
#include "defineSound.h"
#include "defineSkill.h"
#include "lang.h"
#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "eveschool.h"

extern	CGuildCombat	g_GuildCombatMng;
#include "..\_aiinterface\aipet.h"

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#if __VER >= 9	// __PET_0410
#include "pet.h"
#endif	// __PET_0410

#if __VER >= 12 // __LORD
#ifdef __WORLDSERVER
#include "slord.h"
#endif	// __WORLDSERVER
#endif	// __LORD

#ifdef __SYS_TICKET
#ifdef __WORLDSERVER
#include "ticket.h"
#endif	// __WORLDSERVER
#endif	// __SYS_TICKET

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#endif	// __SYS_IDENTIFY

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 13
#include "honor.h"
#endif

#ifdef __CLIENT
#include "..\_Common\ParticleMng.h"
#include "Resdata.h"
#include "DPClient.h"
#include "DialogMsg.h"
extern	CDPClient	g_DPlay;
#include "GuildRender.h"
#else	// __CLIENT
#include "User.h"
#include "UserMacro.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#include "spevent.h"

extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient	g_DPCoreClient;

#endif	// __CLIENT

#include "..\_AIInterface\AIInterface.h"

#include "party.h"
extern	CPartyMng	g_PartyMng;

#include "guild.h"
extern	CGuildMng	g_GuildMng;
#include "guildwar.h"

#include "definequest.h"
#include "Ship.h"
#ifdef __WORLDSERVER
#include "User.h"
#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif
#endif

#ifdef __EVENT_MONSTER
#include "EventMonster.h"
#endif // __EVENT_MONSTER

#if __VER >= 12 // __SECRET_ROOM
#include "SecretRoom.h"
#endif // __SECRET_ROOM

#if __VER >= 12 // __NEW_ITEMCREATEMON_SERVER
#include "CreateMonster.h"
#endif // __NEW_ITEMCREATEMON_SERVER

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

extern	BOOL CanAdd(DWORD dwGold, int nPlus);

#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __WORLDSERVER
#ifdef __VM_0819
CMoverPool* CMover::m_pPool = new CMoverPool(1024, "CMover");
#else	// __VM_0819
CMoverPool* CMover::m_pPool = new CMoverPool(1024);
#endif	// __VM_0819
#else	// __WORLDSERVER
#ifdef __VM_0819
CMoverPool* CMover::m_pPool = new CMoverPool(128, "CMover");
#else	// __VM_0819
CMoverPool* CMover::m_pPool = new CMoverPool(128);
#endif	// __VM_0819
#endif	// __WORLDSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820


const int	MAX_DIALOGFILENAME = 32;

#ifdef __CLIENT
BOOL CMover::m_bQuestEmoticonAdd = TRUE;
FLOAT CMover::m_fQuestEmoticonScale = 1.0f;
#endif

#include "collecting.h"

#pragma warning ( disable : 4355 )

CMover::CMover()
#ifdef __BUFF_1107
	: m_buffs(this)
#endif	// __BUFF_1107
	, m_autoSynchronizer(this)
#ifdef __FL_SWITCH_EQ
	, m_Switches(this)
#endif
#ifdef __FL_MONSTER_ALBUM
	, m_MonsterAlbum(this)
#endif
{
	Init();
}

CMover::~CMover()
{
#ifdef __CLIENT
	SAFE_DELETE(m_pLadolf);
	SAFE_DELETE(m_pAngel);
#ifdef __EVE_BALLOON
	SAFE_DELETE(m_pBalloon);
#endif //__EVE_BALLOON
#endif //__CLIENT

#ifdef __WORLDSERVER
	SAFE_DELETE(m_pNpcProperty);
#endif

#ifdef __WORLDSERVER
	CCreateMonster::GetInstance()->RemoveInfo(this);
#endif // __WORLDSERVER

	CMover* pOther = m_vtInfo.GetOther();
	if (IsValidObj(pOther))
	{
		if (pOther->m_vtInfo.GetOther() == this)
			pOther->m_vtInfo.TradeClear();
	}

#ifdef __CLIENT
	m_vtInfo.VendorClose();
	g_DialogMsg.ClearMessage((CObj*)this);
#endif	// __CLIENT

	SAFE_DELETE(m_pActMover);

	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
		SAFE_DELETE(m_ShopInventory[i]);

	if (GetWorld())
	{
#if !defined(__CLIENT)
#ifdef __LAYER_1021
		GetWorld()->m_respawner.Increment(GetRespawn(), GetRespawnType(), m_bActiveAttack, GetLayer());
#else	// __LAYER_1021
		GetWorld()->m_respawner.Increment(GetRespawn(), GetRespawnType(), m_bActiveAttack);
#endif	// __LAYER_1021
#endif
	}

	SAFE_DELETE_ARRAY(m_aQuest);
	SAFE_DELETE_ARRAY(m_aCompleteQuest);
	SAFE_DELETE_ARRAY(m_aCheckedQuest);

	SetRide(NULL);

#ifdef __WORLDSERVER
	RemoveAllEnemies();
#endif 
}

void CMover::Init()
{
#ifdef __FL_BATTLE_PASS
	SetBattlePassLevel(0);
	SetBattlePassExp(0);
#endif
#ifdef __MODEL_HIDEFASH
	memset(m_bHideFashion, FALSE, sizeof(m_bHideFashion));
#endif
	m_bPositiveX = m_bPositiveZ = false;
	m_dwMotion = -1;
	m_dwMotionOption = 0;
	m_dwType = OT_MOVER;
	m_vMarkingPos = D3DXVECTOR3(0, 0, 0);
	m_nCount = xRandom(128);
	m_bPlayer = FALSE;
	m_dwTypeFlag = 0;
	m_pRide = NULL;
	m_pAIInterface = NULL;
	m_nHitPoint = 100;
	m_nManaPoint = 100;
	m_nFatiguePoint = 100;
	m_vDestPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_idDest = NULL_ID;
	m_fArrivalRange = 0.0f;
	SetSex(SEX_FEMALE);
	m_dwVirtItem = II_WEA_HAN_HAND;
	m_nHitPoint = 100;
	m_nManaPoint = 100;
	m_nFatiguePoint = 100;
	m_dwRideItemIdx = II_RID_RID_STI_MAGIC;
	m_nRemainGP = 0;
	m_nDefenseMin = 0;
	m_nDefenseMax = 0;
	m_nJob = -1;
	memset(m_szName, 0, sizeof(TCHAR));
	m_nAdjHitRate = 0;
	m_nAdjParry = 0;
	m_nStr = 15;
	m_nSta = 15;
	m_nDex = 15;
	m_nInt = 15;
	m_nLevel = 1;
	m_nExp1 = 0;
	m_dwFace = 0;
	m_dwSkinSet = 0;
	m_dwHairMesh = 0;
	m_dwHairColor = 0;
	m_dwHeadMesh = 0;
	m_nFxp = 0;
	m_szCharacterKey[0] = 0;
	{
		int i;
		for (i = 0; i < MAX_ADJPARAMARY; i++)		m_adjParamAry[i] = 0;
		for (i = 0; i < MAX_ADJPARAMARY; i++)		m_chgParamAry[i] = 0x7FFFFFFF;
	}

	ClearEquipInfo();

	m_vtInfo.Init(this);

	m_idPlayer = NULL_ID;
	m_dwAuthorization = AUTH_GENERAL;
	m_idMarkingWorld = NULL_ID;
	m_dwRegionAttr = 0;
	m_dwOldRegionAttr = 0;
	m_bForward = true;
	m_dwMode = 0;
	m_fHairColorR = 1.0f;
	m_fHairColorG = 1.0f;
	m_fHairColorB = 1.0f;
	m_dwHairColor = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
	m_nQuestSize = 0;
	m_aQuest = NULL;
	m_aCompleteQuest = NULL;
	m_nCompleteQuestSize = 0;
	m_aCheckedQuest = NULL;
	m_nCheckedQuestSize = 0;
#ifdef __WORLDSERVER
	m_timerQuestLimitTime.Set(MIN(1));
#else // __WORLDSERVER
	m_nQuestEmoticonIndex = -1;
	m_bShowQuestEmoticon = TRUE;
#endif // __WORLDSERVER
	m_idparty = 0;

	m_oaCmd = m_oaAct = OBJACT_NONE;
	memset(m_nCParam, 0, sizeof(m_nCParam));
	memset(m_nAParam, 0, sizeof(m_nAParam));
	m_dwFlag = 0;
	m_nAtkCnt = 0;
	m_idAttacker = NULL_ID;
	m_idTargeter = NULL_ID;
	m_idTracking = NULL_ID;
	m_tmActionPoint = timeGetTime();

	m_idLastHitMover = NULL_ID;
	m_nFuel = -1;
	m_tmAccFuel = 0;
	m_fAniSpeed = 1.0f;
	m_idPoisonAttacker = NULL_ID;
	m_tmPoisonUnit = 0;
	m_wPoisonDamage = 0;
	m_idBleedingAttacker = NULL_ID;
	m_tmBleedingUnit = 0;
	m_wBleedingDamage = 0;
	m_nMovePattern = 0;
	m_nMoveEvent = 0;
	m_nMoveEventCnt = 0;

	m_idGuild = 0;
	m_bGuildBank = FALSE;

	m_nPlusMaxHitPoint = 0;
	memset(m_dwSMTime, 0, sizeof(DWORD) * SM_MAX);

#ifdef __WORLDSERVER
	m_tGuildMember = CTime::GetCurrentTime();

	m_nQuestKeeping = 0;
	m_nPartyQuestKeeping = 0;
#endif // __WORLDSERVER

#ifdef __CLIENT
	m_bExclusiveLowBody = false;
	m_nWaterCircleCount = 0;
	m_idSfxHit = 0;
	m_dwReqFlag = 0;
	m_fDestScale = 1.0f;
	m_fDestScaleSlerp = 0;
#else // CLIENT
	m_nReflexDmg = 0;
#endif	// not Client

	m_pActMover = new CActionMover(this);
	m_pActMover->SetMover(this);

	m_dwTickRecovery = ::timeGetTime() + NEXT_TICK_RECOVERY;

	memset(&m_Resurrection_Data, 0, sizeof(m_Resurrection_Data));

	m_bActiveAttack = FALSE;
	m_dwGold = 0;

	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
		m_ShopInventory[i] = 0;

#ifdef	__CLIENT
	m_dwLadolfFlag = 0;
	m_pLadolf = NULL;
#if __VER >= 8 //__CSC_VER8_5
	m_pAngel = NULL;
	m_pAngelFlag = FALSE;
	m_dwAngelKind = 0;
#endif //__CSC_VER8_5

#ifdef __EVE_BALLOON
	m_pBalloonFlag = FALSE;
	m_pBalloon = NULL;
	m_dwBalloonKind = 0;
	m_fBalloonYPos = 1.0f;
	m_bBalloon = TRUE;
#endif	//__EVE_BALLOON

#if __VER >= 14 // __WING_ITEM
	m_eWingStateFlag = FLOATING;
	m_fOldLengthSq = 0.0f;
#endif // __WING_ITEM

#endif //__CLIENT
	m_dwStateMode = 0;
	m_nReadyTime = 0;
	m_dwUseItemId = 0;
	m_bItemFind = TRUE;
	m_dwTickCheer = 0;
	m_nCheerPoint = 0;
	SetEatPetId(NULL_ID);

	ZeroMemory(m_tmReUseDelay, sizeof(m_tmReUseDelay));

#ifdef __WORLDSERVER
	m_pNpcProperty = NULL;
	m_dwTickCreated = 0;
#endif

	m_nGuildCombatState = 0;

	m_nSkillLevel = 0;
	m_nSkillPoint = 0;

	m_dwMotionArrive = OBJMSG_STAND;

	m_dwPetId = NULL_ID;
	m_nHealCnt = 0;
	m_fSpeedFactor = 1.0F;

#ifdef __CLIENT
	m_pSfxBuffPet = NULL;
#endif //__CLIENT

#ifdef __JEFF_9_20
	m_dwMute = 0;
#endif	// __JEFF_9_20

	m_nHonor = -1;
	m_dwHonorCheckTime = 0;
	memset(m_aHonorTitle, 0, sizeof(int) * MAX_HONOR_TITLE);
	m_strTitle.Empty();
	m_objIdVisPet = NULL_ID;
	m_dwMoverSfxId = NULL_ID;

#ifdef __WORLDSERVER
	ApplyNpcItems();
#ifdef __FL_VENDORS
	SetVendorTime(0);
	SetVendorItemId(NULL_ID);
#endif
#endif

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
	SetNpcType(NPC_TYPE_NORMAL);
	SetNpcOwner(NULL_ID);
#endif
}


void CMover::SendAIMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (m_pAIInterface)
		m_pAIInterface->SendAIMsg(dwMsg, dwParam1, dwParam2);
}

void CMover::PostAIMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (m_pAIInterface)
		m_pAIInterface->PostAIMsg(dwMsg, dwParam1, dwParam2);
}

#ifdef __FL_PENYA_CONVERTER
#ifdef __WORLDSERVER
void CMover::PenyaToPerins()
{
	if (!IsPlayer())
		return;

	int nPerin = (int)(GetGold() / PERIN_VALUE);
	if (nPerin <= 0)
		return;

	CItemElem pItemElem;
	pItemElem.m_nItemNum = nPerin;
	pItemElem.m_dwItemId = II_SYS_SYS_SCR_PERIN;

	AddGold(-(nPerin * PERIN_VALUE));

	if (!CreateItem(&pItemElem))
	{
		g_dpDBClient.SendQueryPostMail(m_idPlayer, 0, pItemElem, 0, "Perin Converter", "FlyFF");

		CString str;
		str.Format("%d perin(s) sent to mail.", nPerin);
		((CUser*)this)->AddText(str, 0xffffffff);
	}
}
#endif
#endif

BOOL CMover::AddGold(int nGold, BOOL bSend)
{
	__int64 n64Gold = (__int64)nGold;

	if (n64Gold == 0)
		return TRUE;

	__int64 n64Total = static_cast<__int64>(GetGold()) + n64Gold;

#ifdef __FL_PENYA_CONVERTER // Try convert penya to perins firstly if result is more than INT value
#ifdef __WORLDSERVER 
	if (n64Total > static_cast<__int64>(INT_MAX))
	{
		PenyaToPerins();
		n64Total = static_cast<__int64>(GetGold()) + n64Gold;
	}
#endif
#endif

	if (n64Total > static_cast<__int64>(INT_MAX))
	{
#ifndef __CLIENT
		Error("Overflow. UserID:[%07d], Gold:[%d]", m_idPlayer, n64Gold);
#endif
		return FALSE;
	}
	else if (n64Total < 0)
	{
#ifndef __CLIENT
		Error("LackMoney. UserID:[%07d], Gold:[%d]", m_idPlayer, n64Gold);
#endif
		n64Total = 0;
	}

	SetGold(static_cast<int>(n64Total));


#ifdef __WORLDSERVER
	if (bSend)
		g_UserMng.AddSetPointParam(this, DST_GOLD, static_cast<int>(n64Total));
#endif	// __WORLDSERVER

	return TRUE;
}

void CMover::SetMovePattern(int nPattern)
{
	m_nMovePattern = nPattern;
	m_nMoveEvent = 0;
	m_nMoveEventCnt = 0;
	ClearDest();
}

int	CMover::GetHairCost(CMover* pMover, BYTE nR, BYTE nG, BYTE nB, BYTE nHair)
{
	BYTE nOrignalR = (BYTE)(pMover->m_fHairColorR * 255);
	BYTE nOrignalG = (BYTE)(pMover->m_fHairColorG * 255);
	BYTE nOrignalB = (BYTE)(pMover->m_fHairColorB * 255);

	int nHairCost = 0;
	int nHairColorCost = 0;

	if (nR != nOrignalR || nG != nOrignalG || nB != nOrignalB)
		nHairColorCost = HAIRCOLOR_COST;
	else
		nHairColorCost = 0;

	if (pMover->m_dwHairMesh + 1 != nHair + 1)
		nHairCost = HAIR_COST;
	else
		nHairCost = 0;

	return (nHairCost + nHairColorCost);
}

#ifdef __WORLDSERVER
void CMover::SubSMMode()
{
	time_t tmCur = (time_t)(CTime::GetCurrentTime().GetTime());
	for (int i = 0; i < SM_MAX; ++i)
	{
		if (m_dwSMTime[i] > 0)
		{
			if (g_AddSMMode.bSMModetime[i])
			{
				if (m_dwSMTime[i] < (DWORD)(tmCur))
				{
					m_dwSMTime[i] = 0;
				}
			}
			else
				--m_dwSMTime[i];

			if (m_dwSMTime[i] == 0)
			{
				ItemProp* aItemprop = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);
				if (aItemprop)
				{
					if (i == SM_MAX_HP)
					{
						ResetDestParam(aItemprop->dwDestParam[0], aItemprop->nAdjParamVal[1], 1);
					}
					else if (i == SM_MAX_HP50)
					{
						ResetDestParam(aItemprop->dwDestParam[0], m_nPlusMaxHitPoint, 1);
					}
					else if (i == SM_VELOCIJUMP)
					{
						if (aItemprop->dwDestParam1 != -1)
							ResetDestParam(aItemprop->dwDestParam1, aItemprop->nAdjParamVal1);
						if (aItemprop->dwDestParam2 != -1)
							ResetDestParam(aItemprop->dwDestParam2, aItemprop->nAdjParamVal2);
					}
					else if (i == SM_PARTYSKILL30 || i == SM_PARTYSKILL15 || i == SM_PARTYSKILL1)
						g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1);
				}
				else
				{
					WriteLog("%s, %d\r\n\tNo SMItem dwSMItemID[%d] : %d / Name : %s", __FILE__, __LINE__, i, g_AddSMMode.dwSMItemID[i], m_szName);
				}

				g_UserMng.AddSMMode(this, i, 0);

			}
		}
	}
}

void CMover::ClearAllSMMode()
{
	time_t tmCur = (time_t)(CTime::GetCurrentTime().GetTime());
	for (int i = 0; i < SM_MAX; ++i)
	{
		if (m_dwSMTime[i] > 0)
		{
			m_dwSMTime[i] = 0;
			ItemProp* aItemprop = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);
			if (aItemprop)
			{
				if (i == SM_MAX_HP)
				{
					ResetDestParam(aItemprop->dwDestParam[0], aItemprop->nAdjParamVal[1], 1);
				}
				else if (i == SM_MAX_HP50)
				{
					ResetDestParam(aItemprop->dwDestParam[0], m_nPlusMaxHitPoint, 1);
				}
				else if (i == SM_VELOCIJUMP)
				{
					if (aItemprop->dwDestParam1 != -1)
						ResetDestParam(aItemprop->dwDestParam1, aItemprop->nAdjParamVal1);
					if (aItemprop->dwDestParam2 != -1)
						ResetDestParam(aItemprop->dwDestParam2, aItemprop->nAdjParamVal2);
				}
				else if (i == SM_PARTYSKILL30 || i == SM_PARTYSKILL15 || i == SM_PARTYSKILL1)
					g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 0, 0, 1);
			}
			else
			{
				WriteLog("%s, %d\r\n\tNo SMItem dwSMItemID[%d] : %d / Name : %s", __FILE__, __LINE__, i, g_AddSMMode.dwSMItemID[i], m_szName);
			}

			g_UserMng.AddSMMode(this, i, 0);

		}
	}
}


BOOL CMover::SetSMMode(int nType, DWORD dwTime)
{
	if (g_AddSMMode.bSMModetime[nType])
	{
		CTime tRealtime = CTime::GetCurrentTime();
		CTimeSpan ctp(0, 0, 0, dwTime);
		tRealtime += ctp;
		m_dwSMTime[nType] = (time_t)(tRealtime.GetTime());
	}
	else
	{
		m_dwSMTime[nType] = dwTime;
	}

	g_UserMng.AddSMMode(this, nType, m_dwSMTime[nType]);

	return TRUE;
}

#endif // __WORLDSERVER

LPCHARACTER CMover::GetCharacter()
{
	return prj.GetCharacter(m_szCharacterKey);
}

#ifdef __WORLDSERVER
void CMover::SetMarkingPos()
{
	m_vMarkingPos = GetPos();
	m_idMarkingWorld = GetWorld()->GetID();
}
#endif


void CMover::InitNPCProperty()
{
#ifdef __WORLDSERVER
	if (m_pNpcProperty == NULL)
		m_pNpcProperty = new CNpcProperty;
#endif
}

#if defined(__WORLDSERVER)
void CMover::InitCharacter(LPCHARACTER lpCharacter)
{
	if (lpCharacter)
	{
#ifdef __OPT_MEM_0811
		if (lpCharacter->m_nEquipNum > 0)
			m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
#endif	// __OPT_MEM_0811
		strcpy(m_szName, lpCharacter->m_strName);
		for (int i = 0; i < lpCharacter->m_nEquipNum; i++)
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = lpCharacter->m_adwEquip[i];
			itemElem.m_nItemNum = 1;
			m_Inventory.Add(&itemElem);
		}

		CItemElem* pItem;
		for (DWORD i = 0; i < m_Inventory.m_dwItemMax; i++)
		{
			pItem = m_Inventory.GetAtId(i);
			if (pItem)
				m_Inventory.DoEquip(pItem->m_dwObjIndex, pItem->GetProp()->dwParts);
		}

		m_dwHairMesh = lpCharacter->m_dwHairMesh;
		m_dwHairColor = lpCharacter->m_dwHairColor;
		m_dwHeadMesh = lpCharacter->m_dwHeadMesh;
		m_bySex = (BYTE)lpCharacter->m_dwMoverIdx;

		AllocShopInventory(lpCharacter);
		ApplyNpcItems();
		LoadDialog();
#ifdef __OUTPUT_INFO_0803
		prj.OutputStore(lpCharacter->m_szKey, this);
#endif	// __OUTPUT_INFO_0803
	}
	m_nHitPoint = GetMaxHitPoint();
}

BOOL CMover::LoadDialog()
{
	if (m_pNpcProperty)
	{
		LPCHARACTER lpCharacter = GetCharacter();
		if (lpCharacter)
			return m_pNpcProperty->LoadDialog(lpCharacter);
	}

	return FALSE;
}
#endif // __WORLDSERVER



void CMover::ProcessQuest()
{
#ifdef __WORLDSERVER
	if (IsPlayer() == FALSE)
		return;
	BOOL bTimer = FALSE;
	if (m_timerQuestLimitTime.IsTimeOut())
	{
		m_timerQuestLimitTime.Reset();
		bTimer = TRUE;
	}
	for (int i = 0; i < m_nQuestSize; i++)
	{
		LPQUEST lpQuest = (LPQUEST)&m_aQuest[i];
		if (lpQuest)
		{
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(lpQuest->m_wId);
			if (pQuestProp)
			{
				D3DXVECTOR3 vPos = GetPos();

				if (pQuestProp->m_dwEndCondPatrolWorld == GetWorld()->m_dwWorldID && pQuestProp->m_rectEndCondPatrol.PtInRect(CPoint((int)vPos.x, (int)vPos.z)))
				{
					if (lpQuest->m_bPatrol == FALSE)
					{
						lpQuest->m_bPatrol = TRUE;



						((CUser*)this)->AddSetQuest(lpQuest);
					}
				}

				if (pQuestProp->m_nEndCondLimitTime && bTimer)
				{
					if (lpQuest->m_wTime && !(lpQuest->m_wTime & 0x8000))
					{
						lpQuest->m_wTime--;
						((CUser*)this)->AddSetQuest(lpQuest);
					}
				}
			}
		}
	}
#else // __WORLDSERVER
	if (IsPlayer())
		return;
	LPCHARACTER lpCharacter = GetCharacter();
	if (lpCharacter)
	{
		CMover* pMover = GetActiveMover();
		m_nQuestEmoticonIndex = -1;

		for (int i = 0; i < lpCharacter->m_awSrcQuest.GetSize(); i++)
		{
			int nQuest = lpCharacter->m_awSrcQuest.GetAt(i);
			int nItem = lpCharacter->m_anSrcQuestItem.GetAt(i);
			LPQUEST lpQuest = pMover->FindQuest(nQuest);


			if (lpQuest == NULL && pMover->IsCompleteQuest(nQuest) == FALSE)
			{
				if (__IsBeginQuestCondition(pMover, nQuest) && (nItem == 0 || pMover->GetItemNum(nItem)))
					m_nQuestEmoticonIndex = 1;
				else if (m_nQuestEmoticonIndex != 1 && __IsNextLevelQuest(pMover, nQuest) && (nItem == 0 || pMover->GetItemNum(nItem)))
					m_nQuestEmoticonIndex = 4;
			}

			if (lpQuest && prj.m_aPropQuest.GetAt(lpQuest->m_wId) && pMover->IsCompleteQuest(nQuest) == FALSE && lpQuest->m_nState != QS_END)
			{
				if (m_nQuestEmoticonIndex != 1 && (nItem == 0 || pMover->GetItemNum(nItem)))
					m_nQuestEmoticonIndex = 2;
			}
		}
		for (int i = 0; i < lpCharacter->m_awDstQuest.GetSize(); i++)
		{
			int nQuest = lpCharacter->m_awDstQuest.GetAt(i);
			int nItem = lpCharacter->m_anDstQuestItem.GetAt(i);
			LPQUEST lpQuest = pMover->FindQuest(nQuest);


			if (lpQuest && prj.m_aPropQuest.GetAt(lpQuest->m_wId) && pMover->IsCompleteQuest(nQuest) == FALSE && lpQuest->m_nState != QS_END)
			{

				if (__IsEndQuestCondition(pMover, nQuest) && (nItem == 0 || pMover->GetItemNum(nItem)))
					m_nQuestEmoticonIndex = 3;

				else if (m_nQuestEmoticonIndex != 3 && m_nQuestEmoticonIndex != 1 && (nItem == 0 || pMover->GetItemNum(nItem)))
					m_nQuestEmoticonIndex = 2;
			}
		}
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER
void CMover::ProcessAbnormalState()
{
	if (GetAdjParam(DST_CHRSTATE) & CHS_POISON)
	{
		if (IsLive())
		{
			int nUnitCnt = (int)((m_tmPoisonUnit / 1000.0f) * PROCESS_COUNT);
			if (nUnitCnt <= 0)
			{
				nUnitCnt = 1;
				SetPoison(FALSE);
			}
			else
			{
				if ((m_nCount % nUnitCnt) == 0)
				{
					CCtrl* pAttacker = prj.GetCtrl(m_idPoisonAttacker);
					if (IsValidObj(pAttacker) && pAttacker->GetType() == OT_MOVER)
					{
						m_pActMover->SendDamage(AF_FORCE,
							pAttacker->GetId(),
							m_wPoisonDamage);
					}
				}
			}
		}
	}

	if (GetAdjParam(DST_CHRSTATE) & CHS_BLEEDING)
	{
		if (IsLive())
		{
			int nUnitCnt = (int)((m_tmBleedingUnit / 1000.0f) * PROCESS_COUNT);
			if (nUnitCnt <= 0)
			{
				nUnitCnt = 1;
				SetBleeding(FALSE);
			}
			else
			{
				if ((m_nCount % nUnitCnt) == 0)
				{
					CCtrl* pAttacker = prj.GetCtrl(m_idBleedingAttacker);
					if (IsValidObj(pAttacker) && pAttacker->GetType() == OT_MOVER)
					{
						m_pActMover->SendDamage(AF_FORCE,
							pAttacker->GetId(),
							m_wBleedingDamage);
					}
				}
			}
		}
	}

	int nHeal = GetAdjParam(DST_HEAL);
	if (nHeal > 0)
	{
		if (m_nHealCnt > 0)
			m_nHealCnt--;

		if (m_nHealCnt == 0)
		{
			if (IsLive())
			{
				m_nHealCnt = (short)(PROCESS_COUNT * 2.0f);
				SetDestParam(DST_HP, nHeal, NULL_CHGPARAM, TRUE);
			}
		}
	}
}


void CMover::ProcessTarget()
{
	if (m_idAttacker != NULL_ID || m_idTargeter != NULL_ID)
	{
		CMover* pAttacker = GETMOVER(m_idAttacker);
		if (IsValidObj(pAttacker))
		{
			if (IsValidArea(pAttacker, 32.0f) == FALSE)
			{
				m_idAttacker = NULL_ID;
			}
		}
		else
		{
			m_idAttacker = NULL_ID;
		}

		CMover* pTargeter = GETMOVER(m_idTargeter);
		if (IsValidObj(pTargeter))
		{
			if (IsValidArea(pTargeter, 32.0f) == FALSE)
			{
				m_idTargeter = NULL_ID;
			}
		}
		else
		{
			m_idTargeter = NULL_ID;
		}
	}
}

void CMover::ProcessScript()
{
	if (IsPlayer())
		return;

	if (m_szCharacterKey[0] && m_pNpcProperty->IsTimeOut())
	{
		m_pNpcProperty->ResetTimer();
		m_pNpcProperty->RunDialog("#auto", NULL, 0, GetId(), GetId(), 0);
	}
}

void CMover::ApplyNpcItems()
{
	if (!IsNPC())
		return;

	LPCHARACTER pCharacter = GetCharacter();
	if (!pCharacter)
		return;

	LPVENDOR_ITEM pVendor;
	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
	{
		if (m_ShopInventory[i])
			m_ShopInventory[i]->Clear();

		if (pCharacter->m_venderItemAry[i].GetSize())
		{
			ItemProp* apItemProp[MAX_VENDOR_INVENTORY];
			int cbSize = 0;
			// generate
			for (int j = 0; j < pCharacter->m_venderItemAry[i].GetSize(); j++)
			{
				pVendor = (LPVENDOR_ITEM)pCharacter->m_venderItemAry[i].GetAt(j);

				if (!pVendor)
				{
					Error("Invalid pVendor NPC: %s", pCharacter->m_szKey);
					continue;
				}

				GenerateVendorItem(apItemProp, &cbSize, MAX_VENDOR_INVENTORY, pVendor);
			}
			// sort
			for (int j = 0; j < cbSize - 1; j++)
			{
				for (int k = j + 1; k < cbSize; k++)
				{
					if ((apItemProp[k]->dwItemKind1 < apItemProp[j]->dwItemKind1) ||
						(apItemProp[k]->dwItemKind1 == apItemProp[j]->dwItemKind1 && apItemProp[k]->dwItemRare < apItemProp[j]->dwItemRare))
					{
						ItemProp* ptmp = apItemProp[j];
						apItemProp[j] = apItemProp[k];
						apItemProp[k] = ptmp;
					}
				}
			}
			// add
			for (int j = 0; j < cbSize; j++)
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = apItemProp[j]->dwID;
				itemElem.m_nItemNum = (short)(apItemProp[j]->dwPackMax);
				itemElem.m_nHitPoint = apItemProp[j]->dwEndurance;

				if (m_ShopInventory[i]->Add(&itemElem) == FALSE)
					break;
			}
		}

		if (pCharacter->m_venderItemAry2[i].GetSize())
		{
			for (int j = 0; j < pCharacter->m_venderItemAry2[i].GetSize(); j++)
			{
				pVendor = (LPVENDOR_ITEM)pCharacter->m_venderItemAry2[i].GetAt(j);

				if (!pVendor)
				{
					Error("Invalid pVendor NPC: %s", pCharacter->m_szKey);
					continue;
				}

				ItemProp* pProp = prj.GetItemProp(pVendor->m_dwItemId);
				if (!pProp)
				{
					Error("Invalid NPC item with ID: [%d] in NPC: [%s]", pVendor->m_dwItemId, pCharacter->m_szKey);
					continue;
				}

				CItemElem itemElem;
				itemElem.m_dwItemId = pVendor->m_dwItemId;
				itemElem.m_nItemNum = (short)(pProp->dwPackMax);
				itemElem.m_nHitPoint = pProp->dwEndurance;

				DWORD adwItemObjID[MAX_VENDOR_INVENTORY] = { NULL_ID, };
				int anNum[MAX_VENDOR_INVENTORY] = { 0, };
				DWORD dwCount = 0;

				if (m_ShopInventory[i]->Add(&itemElem, adwItemObjID, anNum, &dwCount))
				{
					for (DWORD z = 0; z < dwCount; ++z)
					{
						CItemElem* pItemElem = m_ShopInventory[i]->GetAtId(adwItemObjID[z]);
						if (pItemElem)
							pItemElem->m_nShopCost = pVendor->m_nShopCost;
					}
				}
			}
		}
	}
}

#endif	// __WORLDSERVER

CModel* CMover::LoadModel(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex)
{
	if (m_dwIndex == MI_FEMALE || m_dwIndex == MI_MALE)
		return prj.m_modelMng.LoadModel(pd3dDevice, dwType, dwIndex, TRUE);
	return prj.m_modelMng.LoadModel(pd3dDevice, dwType, dwIndex);
}

void CMover::InitProp(BOOL bInitAI)
{
	MoverProp* pProp = GetProp();
	if (pProp == NULL)
	{
		Error("CMover::InitProp - GetProp(%d) return NULL \n", GetIndex());
		ASSERT(0);
		return;
	}

	if (m_szName[0] == '\0')
		_tcscpy(m_szName, pProp->szName);

#ifdef __WORLDSERVER
	if (bInitAI)
		SetAIInterface(pProp->dwAI);
#else
	UNUSED_ALWAYS(bInitAI);
#endif

	m_dwBelligerence = pProp->dwBelligerence;

#ifdef __OPT_MEM_0811
	if (IsPlayer())
		m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
#else	// __OPT_MEM_0811
	m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
#endif	// __OPT_MEM_0811

#ifdef __FL_SWITCH_EQ
	m_Inventory.m_bInventory = TRUE;
	m_Inventory.m_pMover = this;
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	if (IsPlayer())
		m_Chest.SetItemContainer(ITYPE_ITEM, MAX_CHEST);
#endif

	m_nSlot = 0;
	m_bBank = FALSE;
	for (int k = 0; k < 3; ++k)
	{
		if (IsPlayer())
			m_Bank[k].SetItemContainer(ITYPE_ITEM, MAX_BANK);
		m_dwGoldBank[k] = 0;
		m_idPlayerBank[k] = 0;
	}
	ZeroMemory(m_ShopInventory, sizeof(m_ShopInventory));

	ZeroMemory(m_aJobSkill, sizeof(m_aJobSkill));
	ZeroMemory(m_tmReUseDelay, sizeof(m_tmReUseDelay));


	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		m_aJobSkill[i].dwSkill = NULL_ID;
	}

	if (m_nJob != -1)
	{
		ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
		int nJobNum = prj.m_aJobSkillNum[m_nJob];

		LPSKILL lpSkill;
		for (int i = 0; i < nJobNum; i++)
		{
			ItemProp* pSkillProp = apSkillProp[i];
			lpSkill = &m_aJobSkill[i];
			lpSkill->dwSkill = pSkillProp->dwID;
		}
	}


	m_nLevel = pProp->dwLevel;
	if (m_nLevel < 1)
		m_nLevel = 1;

	if (IsPlayer())
	{
		if (m_nJob != -1)
		{
			m_nHitPoint = GetMaxHitPoint();
			m_nManaPoint = GetMaxManaPoint();
			m_nFatiguePoint = GetMaxFatiguePoint();
		}
		else
		{
			m_nHitPoint = 1;
			m_nManaPoint = 1;
			m_nFatiguePoint = 1;
		}

		m_pActMover->m_fSpeed = pProp->fSpeed;
	}
	else
	{
		m_nStr = pProp->dwStr;
		m_nSta = pProp->dwSta;
		m_nDex = pProp->dwDex;
		m_nInt = pProp->dwInt;

		m_nHitPoint = GetMaxHitPoint();
		m_nManaPoint = pProp->dwAddMp;
		m_nFatiguePoint = GetMaxFatiguePoint();
	}

	UpdateParam();
	m_dwVirtItem = pProp->dwAtk1;

	if (m_bPlayer == FALSE && pProp->dwFlying == 1)
		m_dwTypeFlag |= OBJTYPE_FLYING;

	if (IsPlayer())
	{
		m_aQuest = new QUEST[MAX_QUEST];
		m_aCompleteQuest = new WORD[MAX_COMPLETE_QUEST];
		m_aCheckedQuest = new WORD[MAX_CHECKED_QUEST];
	}
}

void CMover::InitLevel(int nJob, LONG nLevel, BOOL bGamma)
{
#ifdef __WORLDSERVER
	if (nJob < JOB_VAGRANT || nJob >= MAX_JOB || nLevel <= 0 || nLevel > MAX_3RD_LEGEND_LEVEL)
	{
		nJob = JOB_VAGRANT;
		nLevel = 1;
	}

	MoverProp* pProp = GetProp();
	if (pProp)
	{
		m_nLevel = 1;
		m_nStr = m_nSta = m_nDex = m_nInt = 15;
		m_nRemainGP = 0;
		m_nSkillLevel = 0;
		m_nSkillPoint = 0;

		ZeroMemory(m_aJobSkill, sizeof(m_aJobSkill));

		for (int i = 0; i < MAX_SKILL_JOB; i++)
		{
			m_aJobSkill[i].dwSkill = NULL_ID;
			m_aJobSkill[i].dwLevel = 0;
		}

		AddChangeJob(JOB_VAGRANT);

		for (int i = 1; i < nLevel; i++)
		{
			m_nLevel = i + 1;
			m_nRemainGP += prj.m_aExpCharacter[m_nLevel].dwLPPoint;

			if (nJob >= MAX_PROFESSIONAL && i > 59)
				m_nRemainGP++;

			if (i == MAX_GENERAL_LEVEL)
				m_nRemainGP += 12;

			if (nJob < MAX_PROFESSIONAL)
			{
				if ((i + 1) == MAX_JOB_LEVEL)
				{
					if (nJob < MAX_EXPERT)
						AddChangeJob(nJob);
					else
					{
						if (nJob % 2 != 0)
							AddChangeJob((nJob - 5) / 2);
						else
							AddChangeJob((nJob - 4) / 2);
					}
				}
				else if ((i + 1) == MAX_JOB_LEVEL + MAX_EXP_LEVEL)
					AddChangeJob(nJob);
			}
			else
			{
				if ((i + 1) == MAX_JOB_LEVEL)
				{
					if (nJob < MAX_MASTER)
					{
						if (nJob % 2 != 0)
							AddChangeJob((nJob - 15) / 2);
						else
							AddChangeJob((nJob - 14) / 2);
					}
					else if (nJob < MAX_HERO)
					{
						if (nJob % 2 != 0)
							AddChangeJob((nJob - 23) / 2);
						else
							AddChangeJob((nJob - 22) / 2);
					}
					else
					{
						if (nJob % 2 != 0)
							AddChangeJob((nJob - 31) / 2);
						else
							AddChangeJob((nJob - 30) / 2);
					}
				}
				else if ((i + 1) == MAX_JOB_LEVEL + MAX_EXP_LEVEL)
				{
					if (nJob < MAX_MASTER)
					{
						AddChangeJob(nJob - 10);
						AddChangeJob(nJob);
					}
					else if (nJob < MAX_HERO)
					{
						AddChangeJob(nJob - 18);
						AddChangeJob(nJob - 8);
						AddChangeJob(nJob);
					}
					else
					{
						AddChangeJob(nJob - 26);
						AddChangeJob(nJob - 16);
						AddChangeJob(nJob - 8);
						AddChangeJob(nJob);
					}
				}
			}
		}

		const int nSkillPoint = GetTotalSkillPoint(nJob, m_nLevel);
		m_nSkillLevel = m_nSkillPoint = nSkillPoint;

		if (IsMaster())
		{
			int dwTmpSkLevel = 1;//60, 72, 84, 96, 108
			if (nLevel > 59 && nLevel < 72)
				dwTmpSkLevel = 1;
			else if (nLevel > 71 && nLevel < 84)
				dwTmpSkLevel = 2;
			else if (nLevel > 83 && nLevel < 96)
				dwTmpSkLevel = 3;
			else if (nLevel > 95 && nLevel < 108)
				dwTmpSkLevel = 4;
			else if (nLevel > 107 && nLevel < 120)
				dwTmpSkLevel = 5;
			for (int i = 0; i < MAX_SKILL_JOB; i++)
			{
				LPSKILL lpSkill = &(m_aJobSkill[i]);
				if (lpSkill && lpSkill->dwSkill != NULL_ID)
				{
					ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
					if (pSkillProp == NULL)
						continue;
					if (pSkillProp->dwItemKind1 != JTYPE_MASTER)
						continue;
					lpSkill->dwLevel = dwTmpSkLevel;
				}
			}
		}
		else if (IsHero() || IsLegendHero())
		{
			for (int i = 0; i < MAX_SKILL_JOB; i++)
			{
				LPSKILL lpSkill = &(m_aJobSkill[i]);
				if (lpSkill && lpSkill->dwSkill != NULL_ID)
				{
					ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
					if (pSkillProp == NULL)
						continue;
					if (pSkillProp->dwItemKind1 != JTYPE_MASTER)
						continue;
					lpSkill->dwLevel = 5;
				}
			}
		}

		if (bGamma)
		{
			m_nExp1 = 0;
		}

		((CUser*)this)->AddSetChangeJob(nJob);
		g_UserMng.AddNearSetChangeJob(this, nJob, &((CUser*)this)->m_aJobSkill[MAX_JOB_SKILL]);

		((CUser*)this)->LevelUpSetting();
		((CUser*)this)->AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);

		((CUser*)this)->m_playTaskBar.InitTaskBarShorcutKind(SHORTCUT_SKILL);
		((CUser*)this)->AddTaskBar();

		g_UserMng.AddStatInfo(this, TRUE);

		((CUser*)this)->CheckHonorStat();
		((CUser*)this)->AddHonorListAck();
		g_UserMng.AddHonorTitleChange(this, m_nHonor);
	}
#endif // __WORLDSERVER
}



BOOL CMover::InitSkillExp()
{
	for (int i = 0; i < MAX_SKILL_JOB; ++i)
	{
		LPSKILL pSkill = &m_aJobSkill[i];
		if (pSkill != NULL && pSkill->dwSkill != NULL_ID)
		{
			ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
			if (pSkillProp == NULL)
				return FALSE;

			if (0 < pSkill->dwLevel && pSkillProp->dwItemKind1 != JTYPE_MASTER && pSkillProp->dwItemKind1 != JTYPE_HERO)
			{
				m_nSkillPoint += (pSkill->dwLevel * prj.GetSkillPoint(pSkillProp));
				pSkill->dwLevel = 0;
			}
		}
	}

#ifdef __WORLDSERVER
	((CUser*)this)->m_playTaskBar.InitTaskBarShorcutKind(SHORTCUT_SKILL);
	((CUser*)this)->AddTaskBar();
#endif //__WORLDSERVER
	return TRUE;
}

#ifdef __CLIENT
int CMover::GetCurrentMaxSkillPoint()
{
	int nCurrentMaxSkillPoint = m_nSkillPoint;
	for (int i = 0; i < MAX_SKILL_JOB; ++i)
	{
		LPSKILL pSkill = &m_aJobSkill[i];
		if (pSkill != NULL && pSkill->dwSkill != NULL_ID)
		{
			ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
			if (pSkillProp != NULL)
			{
				if (0 < pSkill->dwLevel && pSkillProp->dwItemKind1 != JTYPE_MASTER && pSkillProp->dwItemKind1 != JTYPE_HERO)
					nCurrentMaxSkillPoint += (pSkill->dwLevel * prj.GetSkillPoint(pSkillProp));
			}
		}
	}

	return nCurrentMaxSkillPoint;
}

void CMover::SetSfxBuffPet(const DWORD idEffect)
{
	if (m_pSfxBuffPet)
	{
		g_WorldMng.Get()->RemoveObj(m_pSfxBuffPet);
		m_pSfxBuffPet = NULL;
	}

	if (NULL_ID != idEffect)
		m_pSfxBuffPet = CreateSfx(g_Neuz.m_pd3dDevice, idEffect, GetPos(), GetId(), GetPos(), GetId(), -1);
}

#endif //__CLIENT
void CMover::ReState()
{
#ifdef __WORLDSERVER
	int nAdd = 0;

	nAdd += (m_nStr - 15);
	m_nStr = 15;

	nAdd += (m_nSta - 15);
	m_nSta = 15;

	nAdd += (m_nDex - 15);
	m_nDex = 15;

	nAdd += (m_nInt - 15);
	m_nInt = 15;

	m_nRemainGP += nAdd;

	RefreshGrilledEel();
	g_UserMng.AddStatInfo(this, TRUE);

	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange(this, m_nHonor);
#endif // __WORLDSERVER
}

#ifdef __S_ADD_RESTATE
void CMover::ReStateOne(int nKind)
{
#ifdef __WORLDSERVER
	int nAdd = 0;
	switch (nKind)
	{
	case 0:		// Str
	{
		nAdd = m_nStr - 15;
		m_nStr = 15;
	}
	break;
	case 1:		// Sta
	{
		nAdd = m_nSta - 15;
		m_nSta = 15;
	}
	break;
	case 2:		// Dex
	{
		nAdd = m_nDex - 15;
		m_nDex = 15;
	}
	break;
	case 3:		// Int
	{
		nAdd = m_nInt - 15;
		m_nInt = 15;
	}
	break;
	}
	m_nRemainGP += nAdd;

	RefreshGrilledEel();
	g_UserMng.AddStatInfo(this, TRUE);

	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange(this, m_nHonor);
#endif // __WORLDSERVER
}
#endif // __S_ADD_RESTATE

#ifdef __ADD_RESTATE_LOW
void CMover::ReStateOneLow(int nKind)
{
#ifdef __WORLDSERVER
	int nAdd = 0;
	switch (nKind)
	{
	case 0:		// Str
	{
		if (m_nStr >= RESTATE_LOW + 15)
		{
			nAdd = RESTATE_LOW;
			m_nStr -= RESTATE_LOW;
		}
		else
		{
			nAdd = m_nStr - 15;
			m_nStr = 15;
		}
	}
	break;
	case 1:		// Sta
	{
		if (m_nSta >= RESTATE_LOW + 15)
		{
			nAdd = RESTATE_LOW;
			m_nSta -= RESTATE_LOW;
		}
		else
		{
			nAdd = m_nSta - 15;
			m_nSta = 15;
		}
	}
	break;
	case 2:		// Dex
	{
		if (m_nDex >= RESTATE_LOW + 15)
		{
			nAdd = RESTATE_LOW;
			m_nDex -= RESTATE_LOW;
		}
		else
		{
			nAdd = m_nDex - 15;
			m_nDex = 15;
		}
	}
	break;
	case 3:		// Int
	{
		if (m_nInt >= RESTATE_LOW + 15)
		{
			nAdd = RESTATE_LOW;
			m_nInt -= RESTATE_LOW;
		}
		else
		{
			nAdd = m_nInt - 15;
			m_nInt = 15;
		}
	}
	break;
	}
	m_nRemainGP += nAdd;

	RefreshGrilledEel();
	g_UserMng.AddStatInfo(this, TRUE);

	((CUser*)this)->CheckHonorStat();
	((CUser*)this)->AddHonorListAck();
	g_UserMng.AddHonorTitleChange(this, m_nHonor);
#endif // __WORLDSERVER
}
#endif // __ADD_RESTATE_LOW

#ifdef __WORLDSERVER
BOOL CMover::ReplaceInspection(REGIONELEM* pPortkey)
{
	BOOL bResult = TRUE;
	if (bResult != FALSE && pPortkey->m_uItemId != 0xffffffff)
	{
		int nCount = GetItemNum(pPortkey->m_uItemId);
		if ((DWORD)(nCount) <= pPortkey->m_uiItemCount)
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}

		TRACE("ReplaceInspection Item %d Count %d/%d Inspection %d \n", pPortkey->m_uItemId, nCount, pPortkey->m_uiItemCount, bResult);
	}
	if (bResult != FALSE && pPortkey->m_uiMinLevel != 0xffffffff)
	{
		if (pPortkey->m_uiMinLevel <= (UINT)(m_nLevel) && (UINT)(m_nLevel) <= pPortkey->m_uiMaxLevel)
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}

		TRACE("ReplaceInspection Level (%d~%d) %d Inspection %d \n", pPortkey->m_uiMinLevel, pPortkey->m_uiMaxLevel, m_nLevel, bResult);
	}
	if (bResult != FALSE && pPortkey->m_iQuest != 0xffffffff)
	{
		LPQUEST pCurQuest = FindQuest(pPortkey->m_iQuest);
		if (pCurQuest && (pPortkey->m_iQuestFlag == 0 || pPortkey->m_iQuestFlag == pCurQuest->m_nState))
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE("ReplaceInspection Quest %d Flag %d/%d Inspection %d \n", pPortkey->m_iQuest, pPortkey->m_iQuestFlag, pCurQuest->m_nState, bResult);
	}
	if (bResult != FALSE && pPortkey->m_iJob != 0xffffffff)
	{
		if (pPortkey->m_iJob == m_nJob)
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE("ReplaceInspection Job %d/%d Inspection %d \n", pPortkey->m_iJob, m_nJob, bResult);
	}
	if (bResult != FALSE && pPortkey->m_iGender != 0xffffffff)
	{
		if (pPortkey->m_iGender == GetSex())
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE("ReplaceInspection Gender %d/%d Inspection %d \n", pPortkey->m_iGender, GetSex(), bResult);
	}
	if (bResult != FALSE && pPortkey->m_bCheckParty != FALSE)
	{
		CParty* pParty = g_PartyMng.GetParty(m_idparty);
		if (pParty != NULL && pParty->IsMember(m_idPlayer))
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE("ReplaceInspection Party Inspection %d \n", bResult);
	}
	if (bResult != FALSE && pPortkey->m_bCheckGuild != FALSE)
	{
		CGuild* pGuild = g_GuildMng.GetGuild(m_idGuild);
		if (pGuild && pGuild->IsMember(m_idPlayer))
			bResult = TRUE;
		else
			bResult = FALSE;
		TRACE("ReplaceInspection Guild Inspection %d \n", bResult);
	}

	return bResult;
}


BOOL CMover::Replace(u_long uIdofMulti, DWORD dwWorldID, D3DXVECTOR3& vPos, REPLACE_TYPE type, int nLayer)
{
	CWorld* pWorld = GetWorld();
	if (!pWorld)
		return FALSE;

	if (dwWorldID == 0 || dwWorldID == NULL_ID)
		return FALSE;

	CWorld* pTargetWorld = g_WorldMng.GetWorld(dwWorldID);
	if (!pTargetWorld || !pTargetWorld->m_linkMap.GetLinkMap(nLayer))
	{
		Error("Layer not found");
		return FALSE;
	}

	if (type == REPLACE_NORMAL)
	{
		if (IsAuthHigher(AUTH_GAMEMASTER) == FALSE && IsFly())
			return FALSE;
	}

	if (IsCollecting())
		StopCollecting();

#ifndef __AIO_PETS
	if (HasActivatedEatPet())
		InactivateEatPet();
#endif

	LPREPLACEOBJ lpReplaceObj = NULL;

	for (int i = 0; i < pWorld->m_cbModifyLink; i++)
	{
		if (pWorld->m_apModifyLink[i] == this)
		{
			pWorld->m_apModifyLink[i] = NULL;
			m_vRemoval = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			break;
		}
	}

	for (int i = 0; i < pWorld->m_cbReplaceObj; i++)
	{
		if (pWorld->m_aReplaceObj[i].pObj == this)
		{
			lpReplaceObj = &pWorld->m_aReplaceObj[i];
			break;
		}
	}
	if (lpReplaceObj == NULL)
	{
		ASSERT(pWorld->m_cbReplaceObj < MAX_REPLACEOBJ);
		lpReplaceObj = &pWorld->m_aReplaceObj[pWorld->m_cbReplaceObj++];
		lpReplaceObj->pObj = this;
	}

	lpReplaceObj->dwWorldID = dwWorldID;
	lpReplaceObj->vPos = vPos;
	lpReplaceObj->uIdofMulti = uIdofMulti;
	lpReplaceObj->nLayer = nLayer;

	return TRUE;
}

BOOL CMover::CreateItem(CItemElem* pItemElem, int nType)
{
	if (!pItemElem)
		return	FALSE;

	if (pItemElem->m_dwItemId == 0)
		return FALSE;

	if (pItemElem->m_nItemNum <= 0)
	{
		Error(("CMover::CreateItem : m_idPlayer [%d], ItemId [%d], ItemNum [%d], Type [%d]"), m_idPlayer, pItemElem->m_dwItemId, pItemElem->m_nItemNum, nType);
		return FALSE;
	}

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
	{
		Error(("CMover::CreateItem : Item Property Invalid. m_idPlayer [%d], ItemId [%d], Type [%d]"), m_idPlayer, pItemElem->m_dwItemId, nType);
		return FALSE;
	}

	CItemContainer<CItemElem>* pContainer = GetContainerByType(nType);
	if (!pContainer)
	{
		//Error("CreateItem() no container: m_idPlayer [%d], ItemId [%d], Type [%d]", m_idPlayer, pItemElem->m_dwItemId, nType);
		return FALSE;
	}

	if (pItemElem->m_nHitPoint <= 0)
		pItemElem->m_nHitPoint = (pItemProp->dwEndurance == -1) ? 0 : pItemProp->dwEndurance;

	#ifdef __WEAPON_RARITY
	if (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3) || IsValidRarityItem2(pItemElem->GetProp()->dwItemKind3))
	{
		if (pItemElem->m_nWeaponRarity < 1)
			pItemElem->m_nWeaponRarity = 1;
	}
	else
		pItemElem->m_nWeaponRarity = 0;

	//NOTICE
	if (IsPlayer() && pItemElem->m_nWeaponRarity > 1)
	{
		CString strWeapon;
		BOOL bCheck = FALSE;
		if (pItemElem->m_nWeaponRarity == 5)
		{
			strWeapon.Format("[Legendary] %s", pItemElem->GetName());
			bCheck = TRUE;
		}
		else if (pItemElem->m_nWeaponRarity == 4)
		{
			strWeapon.Format("[Mythical] %s", pItemElem->GetName());
			bCheck = TRUE;
		}
		//else if (pItemElem->m_nWeaponRarity == 3)
		//{
		//	strWeapon.Format("[Rare] %s", pItemElem->GetName());
		//	bCheck = TRUE;
		//}
		//else if (pItemElem->m_nWeaponRarity == 2)
		//{
		//	strWeapon.Format("[Uncommon] %s", pItemElem->GetName());
		//	bCheck = TRUE;
		//}

		if (bCheck && strWeapon != "")
		{
			//char szMsg[500];
			//sprintf(szMsg, "[Notice] %s obtained %s!", ((CUser*)this)->GetName(), strWeapon);

			CString str;
			str.Format("[Notice] %s obtained %s!", ((CUser*)this)->GetName(), strWeapon);

#ifdef _ANNOUNCE_RENDERER
			g_UserMng.SendAnnouncement(str.GetBuffer(0), AnnounceRenderer::RendererType::TYPE_CENTER, 0);
#endif

			//g_DPCoreClient.SendCaption(szMsg, 0, 1);
		}
	}
#endif // __WEAPON_RARITY

	if (pItemElem->GetSerialNumber() == 0)
		pItemElem->SetSerialNumber();

#ifdef __FL_EQUALIZED_MODE
	if (IsPlayer() && nType == TYPE_INVENTORY)
	{
		CUser* pUser = (CUser*)this;
		if (pUser->bTmpMode && !pUser->IsAuthHigher(AUTH_ADMINISTRATOR))
		{
			g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, *pItemElem, 0, pItemProp->szName, "");
			return TRUE;
		}
	}
#endif

	BOOL fSuccess = pContainer->Add(pItemElem);
	if (fSuccess && IsPlayer())
		((CUser*)this)->AddCreateItem(pItemElem, nType);

	return fSuccess;
}

void CMover::RemoveItem(BYTE nId, short nNum, int nType)
{
	CItemContainer<CItemElem>* pContainer = GetContainerByType(nType);
	if (!pContainer)
	{
		Error("RemoveItem() no container: m_idPlayer [%d], type [%d]", m_idPlayer, nType);
		return;
	}

	CItemElem* pItemBase = pContainer->GetAtId(nId);
	if (!pItemBase)
	{
		Error("RemoveItem() no item: m_idPlayer [%d], type [%d]", m_idPlayer, nType);
		return;
	}

	if (nNum < 1 || nNum > pItemBase->m_nItemNum)
	{
		Error(("RemoveItem() wrong number: m_idPlayer [%d], m_dwItemId [%d], m_nItemNum [%d], Count [%d]"), m_idPlayer, pItemBase->m_dwItemId, pItemBase->m_nItemNum, nNum);
		nNum = pItemBase->m_nItemNum;
	}

	UpdateItem(nId, UI_NUM, pItemBase->m_nItemNum - nNum, 0, nType);
}
#endif	// __WORLDSERVER

CItem* CMover::_DropItemNPC(DWORD dwItemType, DWORD dwID, short nDropNum, const D3DXVECTOR3& vPos)
{
#ifdef __WORLDSERVER
	if (IsInvalidObj(this))
		return NULL;

	CWorld* pWorld = GetWorld();
	if (pWorld == NULL)
		return NULL;

	CItemElem* pItemBase = m_Inventory.GetAtId(dwID);
	if (NULL == pItemBase)
		return NULL;

	if (m_Inventory.IsEquip(pItemBase->m_dwObjId))
		return NULL;

	short nNum = pItemBase->m_nItemNum;
	if (nDropNum == 0)
		nDropNum = nNum;

	if (nDropNum < 1 || nDropNum > nNum)
		return NULL;

	CItem* pItem = new CItem;
	pItem->m_pItemBase = new CItemElem;
	*(pItem->m_pItemBase) = *pItemBase;
	pItem->m_pItemBase->m_nItemNum = nDropNum;

	if (pItemBase->m_dwItemId == 0)
		Error("_DropItemNPC SetIndex: %s \n", GetName());

	pItem->SetIndex(D3DDEVICE, pItemBase->m_dwItemId);
	if (pItem->m_pModel)
	{
		D3DXVECTOR3 v = vPos;
		v.y += (pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y) / 2.0f;
		pItem->SetPos(v);
		pItem->SetAngle((float)(xRandom(360)));
		pItem->m_dwDropTime = timeGetTime();

		if (pWorld->ADDOBJ((CObj*)pItem, TRUE, GetLayer()))
		{
			RemoveItem((BYTE)(dwID), nDropNum);
			return pItem;
		}
		else
		{
			SAFE_DELETE(pItem);
		}
	}
	else
	{
		SAFE_DELETE(pItem);
	}
#endif // WORLDSERVER
	return NULL;
}


BOOL CMover::IsDropableState(BOOL bPK)
{
	if (IsInvalidObj(this))
		return FALSE;

#ifdef __WORLDSERVER
	CWorld* pWorld = GetWorld();
	if (pWorld == NULL)
		return FALSE;

	if (m_pActMover->m_bGround == 0 || m_pActMover->IsFly())
	{
		if (bPK == FALSE)
		{
			((CUser*)this)->AddDefinedText(TID_GAME_FLIGHTDROP, "");
			return FALSE;
		}
	}

	if (m_vtInfo.GetOtherID() != NULL_ID)
	{
		((CUser*)this)->AddDefinedText(TID_GAME_TRADELIMITITEM, "");
		return FALSE;
	}

	if (m_vtInfo.VendorIsVendor())
		return FALSE;

	if (IsMode(ITEM_MODE))
		return FALSE;

#endif // __WORLDSERVER
	return TRUE;
}

CItem* CMover::DropItem(DWORD dwID, short nDropNum, const D3DXVECTOR3& vPos, BOOL bPK)
{
#ifdef __WORLDSERVER
	if (IsPlayer() == FALSE)
		return _DropItemNPC(0, dwID, nDropNum, vPos);

	CItemElem* pItemBase = m_Inventory.GetAtId(dwID);
	if (IsDropable(pItemBase, bPK) == FALSE)
		return NULL;

	short nNum = pItemBase->m_nItemNum;
	if (nDropNum == 0)
		nDropNum = nNum;

	if (nDropNum < 1 || nDropNum > nNum)
		return NULL;

	CItem* pItem = new CItem;
	pItem->m_pItemBase = new CItemElem;
	*(pItem->m_pItemBase) = *pItemBase;
	pItem->m_pItemBase->m_nItemNum = nDropNum;

	if (pItemBase->IsQuest())
		pItem->m_idHolder = m_idPlayer;

#ifndef __FL_FIX_USELESS_LOGS
	if (pItemBase->IsLogable())
	{
		LogItemInfo aLogItem;
		aLogItem.Action = "D";
		aLogItem.SendName = GetName();
		aLogItem.RecvName = "GROUND";
		aLogItem.WorldId = GetWorld()->GetID();
		aLogItem.Gold = aLogItem.Gold2 = GetGold();
		g_DPSrvr.OnLogItem(aLogItem, pItemBase, nDropNum);
	}
#endif

	pItem->SetIndex(D3DDEVICE, pItemBase->m_dwItemId);

	if (pItem->m_pModel)
	{
		D3DXVECTOR3 v = vPos;
		v.y += (pItem->m_pModel->m_vMax.y - pItem->m_pModel->m_vMin.y) / 2.0f;
		pItem->SetPos(v);
		pItem->SetAngle((float)(xRandom(360)));
		pItem->m_dwDropTime = timeGetTime();
		UNUSED_ALWAYS(bPK);

		if (GetWorld()->ADDOBJ((CObj*)pItem, TRUE, GetLayer()))
		{
			RemoveItem((BYTE)(dwID), nDropNum);
			return pItem;
		}
		else
		{
			SAFE_DELETE(pItem);
		}
	}
	else
	{
		SAFE_DELETE(pItem);
	}

#endif // WORLDSERVER
	return NULL;
}


REGIONELEM* CMover::UpdateRegionAttr()
{
	REGIONELEM* pPortkey = NULL;
	const DWORD dwCheck = (RA_SAFETY | RA_FIGHT);

	D3DXVECTOR3 vPos = GetPos();
	POINT pt = { (LONG)(vPos.x), (LONG)(vPos.z) };
	LPREGIONELEM lpRegionElem;
	int nSize = GetWorld()->m_aRegion.GetSize();
	DWORD	dwRegionAttr = 0;
	for (int i = 0; i < nSize; i++)
	{
		lpRegionElem = GetWorld()->m_aRegion.GetAt(i);
		if (!lpRegionElem)
			continue;

		if (lpRegionElem->m_rect.PtInRect(pt))
		{
			dwRegionAttr |= lpRegionElem->m_dwAttribute;
			if (lpRegionElem->m_dwIdTeleWorld != WI_WORLD_NONE)
				pPortkey = lpRegionElem;
		}
	}

	if (dwRegionAttr & dwCheck)
	{
		m_dwOldRegionAttr = m_dwRegionAttr;
		m_dwRegionAttr = dwRegionAttr;
	}
	else
	{
		m_dwOldRegionAttr = m_dwRegionAttr;
		m_dwRegionAttr = GetWorld()->m_nPKMode;;
	}

	return pPortkey;
}

void CMover::SetStateMode(DWORD dwMode, BYTE nFlag)
{
	m_dwStateMode |= dwMode;
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode(((CUser*)this), nFlag);
#endif // __WORLDSERVER
}

void CMover::SetStateNotMode(DWORD dwMode, BYTE nFlag)
{
	m_dwStateMode &= (~dwMode);
#ifdef __WORLDSERVER
	g_UserMng.AddStateMode(((CUser*)this), nFlag);
#endif // __WORLDSERVER
}

#ifdef __CLIENT
BOOL CMover::IsUseItemReadyTime(ItemProp* pItemProp, OBJID dwObjItemId, int nType)
{
	if (nType == TYPE_INVENTORY)
	{
		if (m_Inventory.IsEquip(dwObjItemId))
		{
			return TRUE;
		}
	}

	if (pItemProp->dwParts == PARTS_RIDE)
	{
		CWorld* pWorld = GetWorld();
		if (!pWorld)
			return FALSE;

		int nLimitLv = pItemProp->dwFlightLimit;
		if (nLimitLv == NULL_ID)
			nLimitLv = 1;

		if (GetFlightLv() < nLimitLv)
		{
			PrintString(this, TID_GAME_USEAIRCRAFT);
			return FALSE;
		}

		int nAttr = pWorld->GetHeightAttribute(GetPos().x, GetPos().z);
		if (!pWorld->m_bFly || nAttr == HATTR_NOFLY)
		{
			PrintString(this, TID_ERROR_NOFLY);
			return FALSE;
		}
	}

	if (pItemProp->dwSkillReadyType != 0 &&
		pItemProp->dwSkillReadyType != 0xffffffff)
	{
		if (m_vtInfo.IsVendorOpen())
		{
			return FALSE;
		}
		else if (IsStateMode(STATE_BASEMOTION_MODE))
		{
			PrintString(this, TID_PK_BLINK_LIMIT);
			return FALSE;
		}
		else if (m_pActMover->IsState(OBJSTA_STAND) == FALSE)
		{
			PrintString(this, TID_PK_STAND_LIMIT);
			return FALSE;
		}
		else if (IsFly())
		{
			g_WndMng.PutString(prj.GetText(TID_PK_FLIGHT_NOUSE), NULL, prj.GetTextColor(TID_PK_FLIGHT_NOUSE));
			return FALSE;
		}
	}
	return TRUE;
}
#endif

void CMover::ProcessRegion()
{
	if (FALSE == IsPlayer())
		return;

	REGIONELEM* pPortkey = NULL;
	if (IsPosChanged())
	{
		pPortkey = UpdateRegionAttr();
		SetPosChanged(FALSE);
	}

#ifdef __WORLDSERVER
	if (pPortkey)
	{
		if (IsFly())
			return;

		if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(pPortkey->m_dwIdTeleWorld))
		{
			if (!CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(static_cast<CUser*>(this)->GetWorld()->GetID()))
			{
				const BOOL bRet = CInstanceDungeonHelper::GetInstance()->EnteranceDungeon(static_cast<CUser*>(this), pPortkey->m_dwIdTeleWorld);
				if (bRet == FALSE)
				{
					if (IsPlayer() && GetWorld())
					{
						D3DXVECTOR3	vDeltaE;
						AngleToVectorXZ(&vDeltaE, GetAngle() + 180.0f, 4.5f);
						REPLACE(g_uIdofMulti, GetWorld()->GetID(), GetPos() + vDeltaE, REPLACE_NORMAL, GetLayer());
					}
				}
			}
			else
				REPLACE(g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld, REPLACE_NORMAL, static_cast<CUser*>(this)->GetLayer());

			return;
		}

		if (ReplaceInspection(pPortkey) == FALSE)
			return;

		int nLayer = nTempLayer;
		if (GetWorld() && GetWorld()->GetID() == pPortkey->m_dwIdTeleWorld)
			nLayer = GetLayer();

		REPLACE(g_uIdofMulti, pPortkey->m_dwIdTeleWorld, pPortkey->m_vTeleWorld, REPLACE_NORMAL, nLayer);
	}
#endif	// __WORLDSERVER
}

BOOL CMover::Read(CFileIO* pFile)
{
	CObj::Read(pFile);

	char szDialogFile[MAX_DIALOGFILENAME];
	DWORD dwExtraFlag = 0;

	pFile->Read(m_szName, sizeof(m_szName));
	pFile->Read(szDialogFile, sizeof(szDialogFile));
	pFile->Read(m_szCharacterKey, sizeof(m_szCharacterKey));
	pFile->Read(&m_dwBelligerence, sizeof(m_dwBelligerence));
	pFile->Read(&dwExtraFlag, sizeof(dwExtraFlag));

	UpdateParam();

#if defined(__WORLDSERVER)
	InitCharacter(GetCharacter());
#endif

	return TRUE;
}

CItemElem* CMover::GetLWeaponItem()
{
	return GetWeaponItem(PARTS_LWEAPON);
}

CItemElem* CMover::GetWeaponItem(int nParts)
{
	if (IsPlayer())
		return m_Inventory.GetEquip(nParts);
	return NULL;
}

ItemProp* CMover::GetActiveHandItemProp(int nParts)
{
	if (IsPlayer())
	{
#ifdef __CLIENT
		if (IsActiveMover())
		{
			CItemElem* pItemElem = m_Inventory.GetEquip(nParts);
			if (pItemElem)
				return pItemElem->GetProp();
			return prj.GetItemProp(II_WEA_HAN_HAND);
		}
		else
		{
			DWORD dwWeaponId = m_aEquipInfo[nParts].dwId;
			if (dwWeaponId != NULL_ID)
				return prj.GetItemProp(dwWeaponId);
			return prj.GetItemProp(II_WEA_HAN_HAND);
		}
#else	// __CLIENT
		CItemElem* pItemElem = m_Inventory.GetEquip(nParts);
		if (pItemElem)
			return pItemElem->GetProp();
		return prj.GetItemProp(II_WEA_HAN_HAND);
#endif	// __CLIENT
	}

	if (m_dwVirtItem != NULL_ID)
		return prj.GetItemProp(m_dwVirtItem);

	return NULL;
}
ItemProp* CMover::GetTransyItem(ItemProp* pItemProp, BOOL bCheck, LPCTSTR lpszFileName)
{
	ItemProp* pItemPropChange = NULL;
	CString szMsg;
	int nCount = 0;
	BOOL bSetIteFirst = FALSE;

	if (pItemProp && (pItemProp->dwItemKind2 == IK2_ARMOR || pItemProp->dwItemKind2 == IK2_ARMORETC)
		&& (pItemProp->dwItemSex == SEX_MALE || pItemProp->dwItemSex == SEX_FEMALE))
	{
		for (int j = 0; j < prj.m_aPropItem.GetSize(); j++)
		{
			BOOL bSetIteSecond = FALSE;

			ItemProp* pItemProp1 = prj.GetItemProp(j);
			if (pItemProp1 && pItemProp->dwID != pItemProp1->dwID
				&& (pItemProp1->dwItemKind2 == IK2_ARMOR || pItemProp1->dwItemKind2 == IK2_ARMORETC)
				&& (pItemProp1->dwItemSex == SEX_MALE || pItemProp1->dwItemSex == SEX_FEMALE)
				&& pItemProp->dwItemSex != pItemProp1->dwItemSex)
			{
				if (pItemProp->dwItemKind1 == pItemProp1->dwItemKind1 && pItemProp->dwItemKind2 == pItemProp1->dwItemKind2 && pItemProp->dwItemKind3 == pItemProp1->dwItemKind3
					&& pItemProp->dwItemJob == pItemProp1->dwItemJob && pItemProp->dwHanded == pItemProp1->dwHanded && pItemProp->dwParts == pItemProp1->dwParts
					&& pItemProp->dwItemLV == pItemProp1->dwItemLV //&& pItemProp->dwCost == pItemProp1->dwCost//&& pItemProp->dwItemRare == pItemProp1->dwItemRare
					&& pItemProp->dwAbilityMin == pItemProp1->dwAbilityMin && pItemProp->dwAbilityMax == pItemProp1->dwAbilityMax && pItemProp->fAttackSpeed == pItemProp1->fAttackSpeed
					&& pItemProp->dwLimitLevel1 == pItemProp1->dwLimitLevel1
					)
				{
					if (CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemProp->dwID))
						bSetIteFirst = TRUE;

					if (CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemProp1->dwID))
						bSetIteSecond = TRUE;


					if (bSetIteFirst == bSetIteSecond)
					{
						pItemPropChange = pItemProp1;
						++nCount;

						if (bCheck)
						{
							if (0 < nCount)
								szMsg.Format("%s -> %s (%d)", pItemProp->szName, pItemProp1->szName, nCount);
							else
								szMsg.Format("%s -> %s", pItemProp->szName, pItemProp1->szName);
							FILEOUT(lpszFileName, szMsg);
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		if (bCheck && nCount == 0)
		{
			szMsg.Format("%s -> %s", pItemProp->szName, "NULL");
			FILEOUT(lpszFileName, szMsg);
		}
	}

	return pItemPropChange;
}

CItemElem* CMover::GetEquipItem(int nParts)
{
	return m_Inventory.GetEquip(nParts);
}

BOOL CMover::IsDualWeapon()
{
#ifdef __CLIENT
	if (IsActiveMover())
#else
	if (IsPlayer())
#endif
	{
		CItemElem* pItemElemR = m_Inventory.GetEquip(PARTS_RWEAPON);
		CItemElem* pItemElemL = m_Inventory.GetEquip(PARTS_LWEAPON);
		if (pItemElemR && pItemElemL)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (m_aEquipInfo[PARTS_RWEAPON].dwId != NULL_ID && m_aEquipInfo[PARTS_LWEAPON].dwId != NULL_ID)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}


void CMover::ProcessAniSpeed()
{
	switch (m_pActMover->GetState() & OBJSTA_ATK_ALL)
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
#ifndef __ATTR_HIT_FIX
	case OBJSTA_ATK_MAGIC1:
	case OBJSTA_ATK_RANGE1:
#endif
		m_fAniSpeed = GetAttackSpeed();
		break;
#ifdef __ATTR_HIT_FIX
	case OBJSTA_ATK_MAGIC1:
	case OBJSTA_ATK_RANGE1:
		m_fAniSpeed = GetAttackSpeed() * 1.1f;
		break;
#endif
	case OBJSTA_ATK_CASTING1:
	case OBJSTA_ATK_CASTING2:
	case OBJSTA_ATK_MAGICSKILL:
		m_fAniSpeed = GetCastingAniSpeed();
		break;
	default:
	{
		m_fAniSpeed = 1.0f;
	}
	}
}

#ifdef __CLIENT
void CMover::ProcessScaleSlerp()
{
	if (m_fDestScaleSlerp > 0)
	{
		MoverProp* pMoverProp = GetProp();
		if (pMoverProp)
		{
			D3DXVECTOR3 vScale = GetScale();
			if (pMoverProp->dwClass != RANK_MATERIAL && pMoverProp->dwClass != RANK_SUPER && pMoverProp->dwClass != RANK_MIDBOSS)
			{
				LPCTSTR szErr = Error("CMover::Process : �ڿ����� �ƴѵ� ���Դ�.%s %f, %f, %f %f %f", GetName(), m_fDestScaleSlerp, m_fDestScale, vScale.x, vScale.y, vScale.z);
				//ADDERRORMSG( szErr );
				m_fDestScaleSlerp = 0;
			}
			else
			{
				D3DXVECTOR3 vDestScale = D3DXVECTOR3(m_fDestScale, m_fDestScale, m_fDestScale);
				D3DXVec3Lerp(&vScale, &vDestScale, &vScale, m_fDestScaleSlerp);
				SetScale(vScale);
				m_fDestScaleSlerp -= 0.001f;
			}
		}
	}
}

void CMover::ProcessWaterCircle(const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vPos)
{
	if (!IsPlayer())
		return;

	if (!IsMode(TRANSPARENT_MODE))
	{
		LPWATERHEIGHT pWaterHeight = GetWorld()->GetWaterHeight(GetPos());
		if (pWaterHeight && (pWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
		{
			FLOAT fHeight = (FLOAT)pWaterHeight->byWaterHeight;
			if (vPosTemp.y > fHeight && vPos.y <= fHeight && m_pActMover->IsActJump())
			{
				CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_WATERCROWN01, D3DXVECTOR3(vPos.x, fHeight, vPos.z));
			}
			if (vPos != vPosTemp && vPos.y < fHeight && vPos.y >(fHeight - 1.5f) && m_nWaterCircleCount > 7)
			{
				CreateSfx(g_Neuz.m_pd3dDevice, XI_GEN_WATERCIRCLE01, D3DXVECTOR3(vPos.x, fHeight + .01f, vPos.z));
				m_nWaterCircleCount = 0;
			}
		}
		m_nWaterCircleCount++;
	}
}

void CMover::ProcessETC()
{
	int nAbilityOption = GetSetItemClient();
	if (nAbilityOption >= 3 && !(m_dwFlag & MVRF_SETITEMSFX))
	{
		m_dwFlag |= MVRF_SETITEMSFX;
		CreateAbilityOption_SetItemSFX(nAbilityOption);
	}

	CModelObject* pModel = (CModelObject*)m_pModel;
	if (pModel->m_SparkInfo.m_bUsed)
	{
		if (pModel->m_SparkInfo.m_nCnt != 0)
		{
			pModel->m_SparkInfo.m_fLerp -= 0.05f;

			if (pModel->m_SparkInfo.m_fLerp < 0.0f)
			{
				pModel->m_SparkInfo.m_bUsed = FALSE;
			}
		}

		pModel->m_SparkInfo.m_nCnt++;
	}

	if (GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;

		if (IsPlayer() && g_pPlayer && pWndWorld)
		{
			BOOL bFlag = FALSE;
			int nActivePlayer = pWndWorld->IsGCStatusPlayerWar(g_pPlayer->m_idPlayer);
			int nPlayer = pWndWorld->IsGCStatusPlayerWar(m_idPlayer);


			if (nActivePlayer == 1)
			{

				if (nPlayer != 1)
				{
					bFlag = TRUE;
				}
			}
			else

				if (nActivePlayer == 0)
				{

					if (nPlayer == -1)
					{
						bFlag = TRUE;
					}
				}

			if (bFlag)
			{
				m_dwMode |= GCWAR_NOT_CLICK_MODE;
				m_dwMode |= GCWAR_RENDER_SKIP_MODE;
			}
			else
			{
				m_dwMode &= ~GCWAR_NOT_CLICK_MODE;
				m_dwMode &= ~GCWAR_RENDER_SKIP_MODE;
			}
		}
	}
	else
	{
		m_dwMode &= ~GCWAR_NOT_CLICK_MODE;
		m_dwMode &= ~GCWAR_RENDER_SKIP_MODE;
	}
}

extern 	void CreateFlyParticle(CMover* pMover, float fAngX, int nType);

#endif // __CLIENT

void CMover::ProcessActionPoint()
{
	if ((int)(g_tmCurrent - m_tmActionPoint) > (1000 * 3))
	{
		FLOAT fTime = (g_tmCurrent - m_tmActionPoint) / 1000.0f;
		fTime /= 3.0f;
		if (fTime < 0)	fTime = 0;

#ifdef __WORLDSERVER
		((CUser*)this)->m_playTaskBar.m_nActionPoint += (int)(fTime * 2.0f);
		if (((CUser*)this)->m_playTaskBar.m_nActionPoint > 100)
			((CUser*)this)->m_playTaskBar.m_nActionPoint = 100;
#else
		if (IsActiveMover())
		{
			g_WndMng.m_pWndTaskBar->m_nActionPoint += (int)(fTime * 2.0f);
			if (g_WndMng.m_pWndTaskBar->m_nActionPoint > 100)
				g_WndMng.m_pWndTaskBar->m_nActionPoint = 100;

			g_Neuz.m_NeuzEnemy.CheckInvalid();
		}
#endif // CLIENT

		m_tmActionPoint = g_tmCurrent;
	}
}


#ifdef __WORLDSERVER
#define	CF_SEC	15
#else	// __WORLDSERVER
#define	CF_SEC	63
#endif	// __WORLDSERVER

void CMover::Process()
{
	if (IsDelete())
		return;

	CModelObject* pModel = (CModelObject*)m_pModel;
	D3DXVECTOR3		vPos, vPosTemp;
	BOOL			bCollision;


#ifdef __WORLDSERVER
	if (m_2pc.size() == 0 && m_dwAIInterface != AII_PET)
		return;
#endif	// __WORLDSERVER


#ifdef __CLIENT
	if (IsNPC() && IsFlyingNPC())
		CreateFlyParticle(this, GetAngleX(), 0);

	ProcessScaleSlerp();
	AutoSynchronizer()->UpdateGhost();

	if (m_pRide)
	{
		if (m_pRide->m_pBone)
			WingMotionSetting(pModel);

		m_pRide->FrameMove(NULL, GetRideFrameSpeed());
	}

	if (m_pLadolf)
		m_pLadolf->FrameMove();

#if __VER >= 8 //__CSC_VER8_5
	if (IsPlayer() && HasBuffByIk3(IK3_ANGEL_BUFF))
	{
		if (m_pAngel == NULL)
		{
			m_pAngel = new CModelObject;
			m_pAngel->InitDeviceObjects(g_Neuz.m_pd3dDevice);

			IBuff* pBuff = m_buffs.GetBuffByIk3(IK3_ANGEL_BUFF);
			ItemProp* pItemProp = NULL;
			if (pBuff)
				pItemProp = pBuff->GetProp();

			if (pItemProp)
			{
				switch (pItemProp->dwSfxElemental)
				{
				case ELEMENTAL_ANGEL_BLUE: m_dwAngelKind = 13; break;
				case ELEMENTAL_ANGEL_RED:  m_dwAngelKind = 16; break;
				case ELEMENTAL_ANGEL_WHITE: m_dwAngelKind = 19; break;
				case ELEMENTAL_ANGEL_GREEN: m_dwAngelKind = 22; break;
				}

				CString textFile;
				textFile.Format("%s.chr", pItemProp->szTextFileName);
				m_pAngel->LoadBone(textFile);
				textFile.Format("%s.o3d", pItemProp->szTextFileName);
				m_pAngel->LoadElement(textFile, 0);
				textFile.Format("%s_stand01.ani", pItemProp->szTextFileName);
				m_pAngel->LoadMotion(textFile);

				m_AngelPos = GetPos();

				m_AngelWorldM = m_matWorld;
				D3DXMATRIX mScal;
				D3DXMatrixScaling(&mScal, 0.2f, 0.2f, 0.2f);

				m_AngelWorldM = mScal * m_matRotation;

				m_AngelPos.x += 1.0f;
				m_AngelWorldM._41 = m_AngelPos.x;
				m_AngelWorldM._42 = m_AngelPos.y + 1.0f;
				m_AngelWorldM._43 = m_AngelPos.z;

				m_pAngelFlag = TRUE;
			}
		}
	}
	else
	{
		m_pAngelFlag = FALSE;
		SAFE_DELETE(m_pAngel);
	}
#endif //__CSC_VER8_5

#ifdef __EVE_BALLOON
	if (IsPlayer() && HasBuffByIk3(IK3_BALLOON))
	{
		if (m_pBalloon == NULL)
		{
			m_pBalloon = new CModelObject;
			m_pBalloon->InitDeviceObjects(g_Neuz.m_pd3dDevice);

			IBuff* pBuff = m_buffs.GetBuffByIk3(IK3_BALLOON);
			if (pBuff != NULL)
			{
				ItemProp* pItemProp = pBuff->GetProp();
				if (pItemProp)
				{
					TCHAR szModelName[MAX_PATH];
					prj.m_modelMng.MakeModelName(szModelName, OT_ITEM, pItemProp->dwID);
					m_pBalloon->LoadElement(szModelName, 0);

					m_BalloonPos = GetPos();

					m_BalloonWorldM = m_matWorld;
					D3DXMATRIX mScal;
					D3DXMatrixScaling(&mScal, 1.5f, 1.5f, 1.5f);

					m_BalloonWorldM = mScal;// * m_matRotation;

					m_BalloonPos.x += 1.0f;
					m_BalloonWorldM._41 = m_BalloonPos.x;
					m_BalloonWorldM._42 = m_BalloonPos.y + 1.0f;
					m_BalloonWorldM._43 = m_BalloonPos.z;

					m_pBalloonFlag = TRUE;
				}
			}
		}
	}
	else
	{
		m_pBalloonFlag = FALSE;
		SAFE_DELETE(m_pBalloon);
		m_fBalloonYPos = 1.0f;
	}

	int nAddonsState = 0;
	if (!IsMode(TRANSPARENT_MODE))
		nAddonsState = 1;
	else if (IsActiveMover())
		nAddonsState = 2;

	if (m_pBalloon)
	{
		if (nAddonsState > 0)
		{
			m_pBalloonFlag = TRUE;
			m_pBalloon->SetBlendFactor(nAddonsState == 1 ? 255 : 80);
			m_pBalloon->FrameMove();
			BalloonMoveProcess();
		}
		else
			m_pBalloonFlag = FALSE;
	}
#endif //__EVE_BALLOON

	if (m_pAngel)
	{
		if (nAddonsState > 0)
		{
			m_pAngelFlag = TRUE;
			m_pAngel->SetBlendFactor(nAddonsState == 1 ? 255 : 80);
			m_pAngel->FrameMove();
			AngelMoveProcess();
		}
		else
			m_pAngelFlag = FALSE;
	}

#endif // __CLIENT


	if (IsPlayer())
	{
#ifdef __MAP_SECURITY
#ifdef __CLIENT
		if (IsActiveMover() && ((m_nCount & 31) == 0))
			g_WorldMng.CheckMapKey();
#endif // __CLIENT
#endif // __MAP_SECURITY

		if ((m_nCount & CF_SEC) == 0)
		{
			ProcessPet();
#ifdef __WORLDSERVER
			ProcessCollecting();
#else
			if (IsActiveMover())
				ProcessCollecting();
#endif
#ifdef __JEFF_9_20
			if (m_dwMute > 0)
				m_dwMute--;
#endif	// __JEFF_9_20
		}
	}

#ifdef __CLIENT
	ProcessBuff();
#else
	if ((m_nCount & CF_SEC) == 0)
	{
		ProcessBuff();
#ifdef __FL_VENDORS
		if (GetNpcType() == NPC_TYPE_VENDOR_HUMAN || GetNpcType() == NPC_TYPE_VENDOR_NPC)
		{
			if ((DWORD)(time(0)) > GetVendorTime())
			{
				ShutdownVendor();
				return;
			}
		}
#endif
	}
#endif

	{
		ProcessMove();
	}

	{
		ProcessAI();
	}

	vPosTemp = vPos = GetPos();

	{
		m_pActMover->ProcessPreAction();
		ProcessAniSpeed();
	}

	{

#ifdef __X15
		for (int i = 0; i < 4; ++i)
#endif
		{
			ProcessMovePattern();

			m_pActMover->ProcessAction(&vPos);

			if (IsAnimate())
			{
				FLOAT fFactor = 1.0F;

				if (IsFly() == FALSE
					&& m_pActMover->IsState(OBJSTA_ATK_ALL) == FALSE
					&& m_pActMover->IsState(OBJSTA_FMOVE)
					)
				{
					int nMaxFrame = pModel->GetMaxFrame();
					FLOAT fSpeed = GetSpeed(m_pActMover->m_fSpeed);
					MoverProp* pProp = GetProp();
					if (IsNPC())
					{
						switch (pProp->dwAI)
						{
						case AII_MONSTER:
						case AII_PET:
						case AII_EGG:
						{
							fFactor = fSpeed * nMaxFrame / 1.1F;
							break;
						}
						}
					}
					else
						fFactor = fSpeed * nMaxFrame / 2.2F;

					if (fabs(pProp->fFrame - 0.0F) < 0.000001F)
						fFactor = 1.0F;
					else if (pProp->dwFlying == 1)
						fFactor = 1.0F;
					else
						fFactor *= pProp->fFrame;
					if (fabs(fFactor - 0.0F) < 0.000001F)
						fFactor = 1.0F;
				}
				pModel->FrameMove(&vPos, m_fAniSpeed * fFactor);
			}
		}

		if (m_nAtkCnt > 0)
			++m_nAtkCnt;
	}
	{
		bCollision = m_pActMover->ProcessCollision(&vPos);
	}

#ifdef __CLIENT
	ProcessWaterCircle(vPosTemp, vPos);
#endif

	{
#ifdef __CLIENT
		if (GetWorld()->GetLandscape(vPos))
#endif
		{
			SetPos(vPos);
			if (bCollision)
				OnActCollision();
			AutoSynchronizer()->CollisionState(bCollision);
		}
	}

	{
		ProcessRegion();
		ProcessQuest();
#ifdef __WORLDSERVER
		ProcessDeleteRespawn();
#endif // __WORLDSERVER
	}

#ifdef __WORLDSERVER
	{
		ProcessScript();
	}
#endif	// __WORLDSERVER

	{
#ifdef __WORLDSERVER
		ProcessRecovery();
		ProcessAbnormalState();

		if (IsNPC())
		{
			if ((m_nCount & 127) == 0)
			{
				ProcessTarget();
				ProcInstantBanker();
			}
		}
#endif // __WORLDSERVER

		if (IsPlayer() && IsLive())
			ProcessActionPoint();
	}
#ifdef __WORLDSERVER
	ProcessSFXDamage();
#endif	// __WORLDSERVER

#if defined(__CLIENT)
	ProcessETC();

	if (NULL_ID != m_dwMoverSfxId && !m_pSfxBuffPet)		//gmpbigsun: buffpet effect
		SetSfxBuffPet(m_dwMoverSfxId);
#endif //defined(__CLIENT)

	++m_nCount;

}

#ifdef __CLIENT

#if __VER >= 8 //__CSC_VER8_5
void CMover::AngelMoveProcess()
{
	if (m_pAngelFlag && m_pAngel != NULL)
	{
		D3DXVECTOR3 vDist = GetPos() - m_AngelPos;
		FLOAT fDistSq = D3DXVec3LengthSq(&vDist);

		if (fDistSq > 100.0f * 100.0f)
		{
			SAFE_DELETE(m_pAngel);
			m_pAngelFlag = FALSE;
			return;
		}
		if (fDistSq > 1.0f * 1.0f)
		{
			D3DXVECTOR3 vf1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 vf2 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			D3DXVECTOR3 vf3;

			FLOAT fSerp = fDistSq / 10.0f;

			D3DXVec3Lerp(&vf3, &vf1, &vf2, fSerp);

			FLOAT fSpeed = 0.0f;
			m_AngelWorldM = m_matWorld;
			D3DXMATRIX mScal;
			D3DXMatrixScaling(&mScal, 0.2f, 0.2f, 0.2f);

			m_AngelWorldM = mScal * m_matRotation;

			D3DXVec3Normalize(&vDist, &vDist);

			fSpeed = GetSpeed(m_pActMover->m_fSpeed) * vf3.x;

			vDist = vDist * fSpeed;

			m_AngelPos.x += vDist.x;
			m_AngelPos.y += vDist.y;
			m_AngelPos.z += vDist.z;

			m_AngelWorldM._41 = m_AngelPos.x;
			m_AngelWorldM._42 = m_AngelPos.y + 1.0f;
			m_AngelWorldM._43 = m_AngelPos.z;
		}

		CreateAngelParticle(m_AngelPos);
	}
}
#endif //__CSC_VER8_5

#ifdef __EVE_BALLOON
void CMover::BalloonMoveProcess()
{
	if (m_pBalloonFlag && m_pBalloon != NULL)
	{
		D3DXVECTOR3 vDist = GetPos() - m_BalloonPos;
		FLOAT fDistSq = D3DXVec3LengthSq(&vDist);

		D3DXMATRIX mScal, mRot;
		if (fDistSq > 100.0f * 100.0f)
		{
			SAFE_DELETE(m_pBalloon);
			m_pBalloonFlag = FALSE;
			return;
		}
		if (fDistSq > 1.0f * 1.0f)
		{
			D3DXVECTOR3 vf1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 vf2 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			D3DXVECTOR3 vf3;

			FLOAT fSerp = fDistSq / 10.0f;

			D3DXVec3Lerp(&vf3, &vf1, &vf2, fSerp);

			FLOAT fSpeed = 0.0f;
			m_BalloonWorldM = m_matWorld;
			D3DXMatrixScaling(&mScal, 1.5f, 1.5f, 1.5f);

			m_BalloonWorldM = mScal;// * m_matRotation;

			D3DXVec3Normalize(&vDist, &vDist);

			fSpeed = GetSpeed(m_pActMover->m_fSpeed) * vf3.x;

			vDist = vDist * fSpeed;

			m_BalloonPos.x += vDist.x;
			m_BalloonPos.y += vDist.y;
			m_BalloonPos.z += vDist.z;

			m_BalloonWorldM._41 = m_BalloonPos.x;
			m_BalloonWorldM._42 = m_BalloonPos.y + m_fBalloonYPos;
			m_BalloonWorldM._43 = m_BalloonPos.z;

			if (m_bBalloon == TRUE)
			{
				m_fBalloonYPos += 0.006f;

				if (m_fBalloonYPos > 1.5)
					m_bBalloon = FALSE;
			}
			else
			{
				m_fBalloonYPos -= 0.006f;

				if (m_fBalloonYPos < 0.5)
					m_bBalloon = TRUE;
			}
		}
		else
		{
			m_BalloonWorldM = m_matWorld;
			D3DXMATRIX mScal;
			D3DXMatrixScaling(&mScal, 1.5f, 1.5f, 1.5f);

			m_BalloonWorldM = mScal;// * m_matRotation;

			m_BalloonWorldM._41 = m_BalloonPos.x;
			m_BalloonWorldM._42 = m_BalloonPos.y + m_fBalloonYPos;
			m_BalloonWorldM._43 = m_BalloonPos.z;

			if (m_bBalloon == TRUE)
			{
				m_fBalloonYPos += 0.002f;

				if (m_fBalloonYPos > 1.3)
					m_bBalloon = FALSE;
			}
			else
			{
				m_fBalloonYPos -= 0.002f;

				if (m_fBalloonYPos < 0.7)
					m_bBalloon = TRUE;
			}
		}

		D3DXMatrixRotationY(&mRot, timeGetTime() / 3000.0f);
		m_BalloonWorldM = mRot * m_BalloonWorldM;
	}
}
#endif //__EVE_BALLOON

void CMover::CreateAbilityOption_SetItemSFX(int nAbilityOption)
{
	DWORD dwSfx = 0;

	switch (nAbilityOption)
	{
	case 3: dwSfx = XI_GEN_ITEM_SETITEM03; break;
	case 4: dwSfx = XI_GEN_ITEM_SETITEM04; break;
	case 5: dwSfx = XI_GEN_ITEM_SETITEM05; break;
	case 6: dwSfx = XI_GEN_ITEM_SETITEM06; break;
	case 7: dwSfx = XI_GEN_ITEM_SETITEM07; break;
	case 8: dwSfx = XI_GEN_ITEM_SETITEM08; break;
	case 9: dwSfx = XI_GEN_ITEM_SETITEM09; break;
	case 10: dwSfx = XI_GEN_ITEM_SETITEM10; break;
	}

	CSfxPartsLinkShoulder* pSfx = NULL;
	D3DXVECTOR3 v3Scal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	if (nAbilityOption == 3)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 4)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 5)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 6)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 3;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 7)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 3;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 8;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 9;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 8)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 3;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 8;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 9;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 9)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 2;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 3;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 8;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 9;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 26;
		pSfx->SetScale(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 29;
		pSfx->SetScale(D3DXVECTOR3(1.5f, 1.5f, 1.5f));
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}
	else if (nAbilityOption == 10)
	{

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 0;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 1;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 4;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 5;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 6;
		pSfx->SetScale(D3DXVECTOR3(3.0f, 4.5f, 3.0f));
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 7;
		pSfx->SetScale(D3DXVECTOR3(3.0f, 3.0f, 3.0f));
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 8;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 9;
		pSfx->m_v3Offset.y = 0.1f;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 2;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 3;
		pSfx->m_nOldAbilityOption = nAbilityOption;

		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 26;
		pSfx->m_nOldAbilityOption = nAbilityOption;
		pSfx = (CSfxPartsLinkShoulder*)CreateSfx(D3DDEVICE, dwSfx, GetPos(), GetId(), GetPos(), GetId(), -1);
		pSfx->m_nPartsLink = 29;
		pSfx->m_nOldAbilityOption = nAbilityOption;
	}

}

#endif //__CLIENT

void CMover::GenerateVendorItem(ItemProp** apItemProp, int* pcbSize, int nMax, LPVENDOR_ITEM pVendor)
{
	CPtrArray* pItemKindAry = prj.GetItemKindAry(pVendor->m_nItemkind3);
	ItemProp* pItemProp = NULL;
	int cbSizeOld = *pcbSize;

	ASSERT(pVendor->m_nUniqueMin >= 0);
	ASSERT(pVendor->m_nUniqueMax >= 0);

	if (*pcbSize >= nMax)
		return;

	int nMinIdx = -1, nMaxIdx = -1;

	for (int j = pVendor->m_nUniqueMin; j <= pVendor->m_nUniqueMax; j++)
	{
		nMinIdx = prj.GetMinIdx(pVendor->m_nItemkind3, j);
		if (nMinIdx != -1)
			break;
	}
	for (int j = pVendor->m_nUniqueMax; j >= pVendor->m_nUniqueMin; j--)
	{
		nMaxIdx = prj.GetMaxIdx(pVendor->m_nItemkind3, j);
		if (nMaxIdx != -1)
			break;
	}
	if (nMinIdx < 0)
	{
		WriteError("VENDORITEM//%s//%d-%d//%d", GetName(), pVendor->m_nUniqueMin, pVendor->m_nUniqueMax, pVendor->m_nItemkind3);
		return;
	}

	for (int k = nMinIdx; k <= nMaxIdx; k++)
	{
		pItemProp = (ItemProp*)pItemKindAry->GetAt(k);

		if ((NULL == pItemProp) ||
			(pItemProp->dwShopAble == (DWORD)-1) ||
			(pVendor->m_nItemJob != -1 && (DWORD)pItemProp->dwItemJob != pVendor->m_nItemJob))
			continue;

		if (*pcbSize >= nMax)
			break;

		apItemProp[*pcbSize] = pItemProp;
		(*pcbSize)++;
	}
}

void CMover::OnArrive(DWORD dwParam1, DWORD dwParam2)
{
#ifdef __WORLDSERVER
	if (!IsPlayer())
	{
		PostAIMsg(AIMSG_ARRIVAL, dwParam1, dwParam2);
		ClearDestObj();
		g_UserMng.AddMoverBehavior(this);
	}
#endif // __WORLDSERVER
}

void CMover::ClearDestPos()
{
	m_vDestPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CMover::ClearDestObj(BOOL bCancelTaskbar)
{
	DWORD idDestOld = m_idDest;
	m_idDest = NULL_ID; m_fArrivalRange = 0.0f;

#ifdef __CLIENT
	if (IsActiveMover())
	{
		if (bCancelTaskbar)
		{
			CWndTaskBar* pTaskBar = (CWndTaskBar*)g_WndMng.m_pWndTaskBar;
			if (pTaskBar->m_nExecute)
			{
				pTaskBar->OnCancelSkill();
				g_DPlay.SendEndSkillQueue();
			}
		}

		// We should be 100% sure we will be able to SendPlayerBehavior with clear bDest in ControlGround() function after we cleared DestObj
		// For example, if we cleared dest object while moving to it, we will have OBJSTA_FMOVE state, so normally we should OBJMSG_STAND in ControlGround()
		// But in some situations it skipped, for example, if we had key "W" pressed, as soon as we come to ControlGround() we will not be able to OBJMSG_FORWARD (because we already have OBJSTA_FMOVE), 
		// and also we will not be able to OBJMSG_STAND, because bUp is active, it means SendPlayerBehaviour() will never be called till we stop pressing "W"
		if (idDestOld != NULL_ID
			&& m_pActMover
			&& (m_pActMover->GetState() == 0 || m_pActMover->GetMoveState() == OBJSTA_FMOVE || m_pActMover->GetMoveState() == OBJSTA_BMOVE || m_pActMover->GetMoveState() == OBJSTA_STAND))
		{
			SendActMsg(OBJMSG_WAIT);
		}
	}
#endif
}

void CMover::OnArriveAtPos()
{
	SendActMsg(m_dwMotionArrive);
	OnArrive(NULL_ID, 1);
	ClearDestPos();
}

BOOL CMover::SetMotion(DWORD dwMotion, int nLoop, DWORD dwOption)
{
	CModelObject* pModel = (CModelObject*)m_pModel;
	DWORD	dwOrigMotion = dwMotion;
	ItemProp* pItemProp = GetActiveHandItemProp();
	if (m_pActMover->IsStateFlag(OBJSTAF_COMBAT) && (dwOption & MOP_FIXED) == 0)
	{
#if defined(__CLIENT)
		if (pItemProp == NULL)
		{
			if (IsPlayer())
			{
				LPCTSTR szErr = Error("CMover::SetMotion : Player %s %d %d", GetName(), IsActiveMover(), m_aEquipInfo[PARTS_RWEAPON].dwId);
				//ADDERRORMSG( szErr );
			}
			else
			{
				LPCTSTR szErr = Error("CMover::SetMotion : NPC %s %d", GetName(), m_dwVirtItem);
				//ADDERRORMSG( szErr );
			}

			return FALSE;
		}
#endif // __CLIENT



		int nIndex = GetIndex();
		if (!IsPlayer() || (IsPlayer() &&
			(nIndex == MI_MALE ||
				nIndex == MI_FEMALE ||
				nIndex == MI_CROWNIBLIS ||
				nIndex == MI_CROWNSHADE ||
				nIndex == MI_CROWNBUBBLE ||
				nIndex == MI_DWARPETMAS)))
		{
			BOOL bMasterPlayer = FALSE;

			if (nIndex == MI_CROWNIBLIS || nIndex == MI_CROWNSHADE || nIndex == MI_CROWNBUBBLE || nIndex == MI_DWARPETMAS)
				bMasterPlayer = TRUE;

			if (IsDualWeapon() && !bMasterPlayer)
			{
				dwMotion += 900;
			}
			else
			{
				BOOL bHanded = (pItemProp->dwHanded == HD_TWO) ? TRUE : FALSE;
				switch (pItemProp->dwItemKind3)
				{
				case IK3_YOYO:
					if (!bMasterPlayer) dwMotion += MTI_STAND_14;
					else dwMotion += 200;
					break;
				case IK3_BOW:
					if (!bMasterPlayer) dwMotion += MTI_STAND_13;
					else dwMotion += 200;
					break;
				case IK3_SWD:
					if (bHanded)	dwMotion += 800;
					else			dwMotion += 200;
					break;
				case IK3_AXE:
					if (bHanded)	dwMotion += 800;
					else			dwMotion += 500;
					break;
				case IK3_WAND:
					if (GetJob() == JOB_PSYCHIKEEPER
						|| GetJob() == JOB_PSYCHIKEEPER_MASTER
						|| GetJob() == JOB_PSYCHIKEEPER_HERO
						|| GetJob() == JOB_MENTALIST_HERO
						)
					{
						dwMotion += MTI_STAND_11;
					}
					else
						dwMotion += MTI_STAND_02;
					break;
				case IK3_CHEERSTICK: dwMotion += 400; break;
				case IK3_STAFF:
					dwMotion += MTI_STAND_05;
					break;
				case IK3_KNUCKLEHAMMER: dwMotion += 700; break;
				default:
					if (IsPlayer()) dwMotion += 200;
					break;
				}
			}
		}
	}
	if (m_dwMotion == dwOrigMotion && m_dwMotionOption == dwOption)
	{
		if (nLoop == ANILOOP_LOOP)		return FALSE;
		if (pModel->m_bEndFrame == FALSE)
			return FALSE;
		if (pModel->m_bEndFrame && nLoop == ANILOOP_CONT)
			return FALSE;
	}

	prj.m_modelMng.LoadMotion(m_pModel, m_dwType, m_dwIndex, dwMotion);
	m_dwMotion = dwOrigMotion;
	m_dwMotionOption = dwOption;

	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop(nLoop);


	if (m_pModel && m_pModel->m_pModelElem && (m_pModel->m_pModelElem->m_dwIndex == MI_NPC_SNOWMAN01 || m_pModel->m_pModelElem->m_dwIndex == MI_NPC_SNOWMAN02))
		dwOption |= MOP_NO_TRANS;

	if (dwOption & MOP_NO_TRANS)
		pModel->SetMotionBlending(FALSE);
	else
		pModel->SetMotionBlending(TRUE);

	if (dwOption & MOP_SWDFORCE)
	{
		if (IsPlayer())
		{
			int nOption = 0;
			DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
			if (IsActiveMover())
			{
				CItemElem* pItemElem = GetWeaponItem();
				if (pItemElem)
					nOption = pItemElem->GetAbilityOption();
			}
			else
				nOption = m_aEquipInfo[PARTS_RWEAPON].nOption & 0xFF;

			if (nOption >= 10)
				dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
			else if (nOption == 9)
				dwColor = D3DCOLOR_ARGB(255, 255, 150, 0);
			else if (nOption >= 7)
				dwColor = D3DCOLOR_ARGB(255, 198, 0, 255);
			else if (nOption >= 5)
				dwColor = D3DCOLOR_ARGB(255, 123, 82, 255);
			else if (nOption >= 3)
				dwColor = D3DCOLOR_ARGB(255, 129, 221, 251);
			else if (nOption >= 1)
				dwColor = D3DCOLOR_ARGB(255, 201, 251, 190);
			else
				dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

			pModel->MakeSWDForce(PARTS_RWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed);
			pModel->MakeSWDForce(PARTS_LWEAPON, pItemProp->dwItemKind3, (dwOption & MOP_HITSLOW) ? TRUE : FALSE, dwColor, m_fAniSpeed);
		}
	}
	else
	{
		if (pModel->m_pForce)
			pModel->m_pForce->Clear();
		if (pModel->m_pForce2)
			pModel->m_pForce2->Clear();
	}

	return TRUE;
}
BOOL CMover::InitMotion(DWORD dwMotion)
{
	prj.m_modelMng.LoadMotion(m_pModel, m_dwType, m_dwIndex, dwMotion);
	m_dwMotion = dwMotion;
	return TRUE;
}

#ifdef __WORLDSERVER
OBJID CMover::GetMaxEnemyHitID()
{
	OBJID objId = NULL_ID;
	int	nHitPoint = 0;

	for (SET_OBJID::iterator i = m_idEnemies.begin(); i != m_idEnemies.end(); ++i)
	{
		HIT_INFO info = (*i).second;
		if (nHitPoint < info.nHit)
		{
			objId = (*i).first;
			nHitPoint = info.nHit;
		}
	}
	return objId;
}

int CMover::GetEnemyHit(OBJID objid, DWORD* pdwTick)
{
	int nHit = 0;
	DWORD dwTick = 0;

	SET_OBJID::iterator i = m_idEnemies.find(objid);
	if (i != m_idEnemies.end())
	{
		HIT_INFO info = (*i).second;
		nHit = info.nHit;
		dwTick = info.dwTick;
	}

	if (pdwTick)
		*pdwTick = dwTick;

	return nHit;
}


DWORD CMover::AddEnemy(OBJID objid, int nHit)
{
	DWORD dwLast = 0;
	SET_OBJID::iterator i = m_idEnemies.find(objid);
	if (i == m_idEnemies.end())
	{
		HIT_INFO info;
		info.nHit = nHit;
		info.dwTick = GetTickCount();
		m_idEnemies.insert(make_pair(objid, info));
	}
	else
	{
		HIT_INFO& info = (*i).second;
		dwLast = info.dwTick;
		info.dwTick = GetTickCount();
		info.nHit += nHit;
	}
	return dwLast;
}

void CMover::RemoveEnemy(OBJID objid)
{
	SET_OBJID::iterator i = m_idEnemies.find(objid);
	if (i != m_idEnemies.end())
	{
		if (m_pActMover->GetObjHit() == objid)
			m_pActMover->SetObjHit(NULL_ID);

		PostAIMsg(AIMSG_DSTDIE, objid);

		m_idEnemies.erase(i);
	}
}

void CMover::RemoveAllEnemies()
{
	vector<OBJID>	adwEnemy;
	for (SET_OBJID::iterator it = m_idEnemies.begin(); it != m_idEnemies.end(); ++it)
		adwEnemy.push_back(it->first);
	m_idEnemies.clear();

	CMover* pEnemy;
	while (adwEnemy.size() > 0)
	{
		OBJID objid = adwEnemy.back();
		pEnemy = prj.GetMover(objid);
		if (IsValidObj((CObj*)pEnemy))
			pEnemy->RemoveEnemy(GetId());
		adwEnemy.pop_back();
	}
	adwEnemy.clear();
}
#endif

int CMover::DoDie(CCtrl* pAttackCtrl, DWORD dwMsg)
{
#ifdef __WORLDSERVER
	if (IsDie())
		return 0;

	CMover* pAttacker = NULL;

	if (pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER)
		pAttacker = (CMover*)pAttackCtrl;

	if (pAttacker && pAttacker != this && pAttacker->IsPlayer())
	{
		if (IsNPC())
		{
			((CUser*)pAttacker)->SetHonorAdd(GetIndex(), HI_HUNT_MONSTER);

#ifdef __FL_RECORD_BOOK
			MoverProp* pMoverProp = GetProp();
			if (pMoverProp)
			{
				if (pMoverProp->dwClass == RANK_BOSS || pMoverProp->dwClass == RANK_SUPER)
					CRecordBook::GetInstance()->AddPlayerRecord(pAttacker, RecordType::BOSSES_KILLED, 1, true);
				else
					CRecordBook::GetInstance()->AddPlayerRecord(pAttacker, RecordType::MOBS_KILLED, 1, true);
			}
#endif
		}
		else
		{
#ifdef __FL_RECORD_BOOK
			CRecordBook::GetInstance()->AddPlayerRecord(pAttacker, RecordType::PLAYERS_KILLED, 1, true);
#endif
		}
	}

	m_nAtkCnt = 0;

	if (IsPlayer())
	{
		if (IsStateMode(STATE_BASEMOTION_MODE))
		{
			SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF);
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
		}

		SetStun(FALSE);
		SetPoison(FALSE);
		SetBleeding(FALSE);
	}

	BOOL fValid = IsValidObj(this);

	if (IsPlayer() && m_pActMover->IsFly())
		DoEquipItem(GetEquipItem(PARTS_RIDE));

	SendActMsg(OBJMSG_STOP);
	SendActMsg(OBJMSG_DIE, dwMsg, (int)pAttacker);

	if (IsPlayer())
	{
		CMover* pLastHit = prj.GetMover(m_idLastHitMover);
		if (IsValidObj(pLastHit))
		{
			if (pLastHit->m_idAttacker == GetId())
				pLastHit->m_idAttacker = NULL_ID;
			if (pLastHit->m_idTargeter == GetId())
				pLastHit->m_idTargeter = NULL_ID;
		}


		if (fValid)
		{
			if (IsValidObj(pAttacker) == FALSE || pAttacker->IsNPC())
			{
				CPet* pPet = GetPet();
				if (pPet && pPet->GetLevel() != PL_EGG)
				{
					//#ifndef __AIO_PETS
					CItemElem* pItemElem = GetPetItem();
					int nLife = (int)pPet->GetLife();
					if (--nLife >= 0)
					{
						pPet->SetLife(nLife);
						pPet->SetEnergy(CPetProperty::GetInstance()->GetMaxEnergy(pPet->GetLevel()));
					}
					else
					{
						pPet->SetLife(0);
						pPet->SetEnergy(0);
						pPet->SetExp(0);
						pItemElem->SetFlag(CItemElem::expired);
						UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_FLAG, MAKELONG(pItemElem->m_dwObjIndex, pItemElem->m_byFlag));
					}
					PetRelease();
					((CUser*)this)->AddPetState(pItemElem->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp());
				}
			}
		}
	}

	if (fValid)
	{
		g_UserMng.AddMoverDeath(this, pAttacker, dwMsg);
		RemoveAllEnemies();
	}	// fValid

	if (pAttacker && pAttacker->m_pActMover->GetObjHit() == GetId())
		pAttacker->m_pActMover->SetObjHit(NULL_ID);

	if (IsValidObj(pAttacker) && pAttacker->IsNPC())
	{
		pAttacker->SendAIMsg(AIMSG_INIT_TARGETCLEAR);
	}

	if (IsPlayer() == FALSE && pAttacker && pAttacker->IsPlayer())
	{
#ifdef __FL_BATTLE_PASS
		MoverProp* pMoverProp = GetProp();
		if (pMoverProp)
		{
			if (pMoverProp->dwClass == RANK_SUPER)
				CBattlePass::GetInstance()->AddProgress(((CUser*)pAttacker), EXP_BOSS);
			else if (GetLevel() >= pAttacker->GetLevel())
				CBattlePass::GetInstance()->AddProgress(((CUser*)pAttacker), EXP_MOB);
		}
#endif
		for (int i = 0; i < pAttacker->m_nQuestSize; i++)
		{
			LPQUEST lpQuest = (LPQUEST)&pAttacker->m_aQuest[i];
			if (!lpQuest)
				continue;
			QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(lpQuest->m_wId);
			if (!pQuestProp)
				continue;

			for (int j = 0; j < 2; ++j)
			{

				if (pQuestProp->m_nEndCondKillNPCIdx[j] != GetIndex())
					continue;

				if (pQuestProp->m_nBeginCondParty == 2)
				{
					CParty* pParty = g_PartyMng.GetParty(pAttacker->m_idparty);

					if (pParty && pParty->IsMember(pAttacker->m_idPlayer))
					{
						for (int k = 0; k < pParty->GetSizeofMember(); ++k)
						{
							PartyMember* pPartyMember = &pParty->m_aMember[k];
							CMover* pMember = prj.GetUserByID(pPartyMember->m_uPlayerId);
							if (IsValidObj(pMember) && IsValidArea(pMember, 64.0f))
							{
								LPQUEST pMemberQuest = pMember->FindQuest(lpQuest->m_wId);
								if (pMemberQuest && pMemberQuest->m_nKillNPCNum[j] < pQuestProp->m_nEndCondKillNPCNum[j])
								{
									++pMemberQuest->m_nKillNPCNum[j];
									((CUser*)pMember)->AddSetQuest(pMemberQuest);
								}
							}
						}
					}
				}
				else if (lpQuest->m_nKillNPCNum[j] < pQuestProp->m_nEndCondKillNPCNum[j])
				{
					++lpQuest->m_nKillNPCNum[j];
					((CUser*)pAttacker)->AddSetQuest(lpQuest);
				}
			}
		}
	}

#else // WORLDSERVER
#ifndef __CLIENT
	CMover* pAttacker = NULL;
	if (pAttackCtrl && pAttackCtrl->GetType() == OT_MOVER)
		pAttacker = (CMover*)pAttackCtrl;
	SendActMsg(OBJMSG_DIE, dwMsg, (int)pAttacker);
#endif
#endif // not WORLDSERVER


#if defined(__WORLDSERVER)
	if (GetType() == OT_MOVER)
	{
		RemoveCommonBuffs();
		g_UserMng.AddRemoveAllSkillInfluence(this);
	}

	if (IsPlayer() && m_nGuildCombatState)
	{
		if (pAttacker && pAttacker->IsPlayer() && GetWorld() && GetWorld()->GetID() == WI_WORLD_GUILDWAR)
		{
			g_GuildCombatMng.OutWar((CUser*)this, NULL);
			g_GuildCombatMng.GetPoint((CUser*)pAttacker, (CUser*)this);
		}
	}

	if (CSecretRoomMng::GetInstance()->IsInTheSecretRoom(this))
	{
		if (IsNPC() && pAttacker->IsPlayer())
			CSecretRoomMng::GetInstance()->SetSecretRoomKill(GetWorld()->GetID(), GetProp()->dwID);
		if (IsPlayer() && CSecretRoomMng::GetInstance()->IsGuildMaster((CUser*)this))
			CSecretRoomMng::GetInstance()->SetFailGuild((CUser*)this);
	}

	if (IsNPC())
		CCreateMonster::GetInstance()->SetState(GetId(), 'D');


	if (CInstanceDungeonHelper::GetInstance()->IsInstanceDungeon(GetWorld()->GetID()))
	{
		if (!IsPlayer() && IsValidObj(pAttacker) && pAttacker->IsPlayer())
			CInstanceDungeonHelper::GetInstance()->SetInstanceDungeonKill(GetWorld()->GetID(), static_cast<DWORD>(this->GetLayer()), GetProp()->dwID);
	}

#ifdef __EVENTLUA_SPAWN
	if (IsNPC())
		prj.m_EventLua.RemoveSpawnedMonster(GetId());
#endif // __EVENTLUA_SPAWN

#ifdef __FL_FFA
	CFFAManager::GetInstance()->Kill(pAttacker, this);
#endif
#ifdef __FL_TDM
	CTDMManager::GetInstance()->Kill(pAttacker, this);
#endif

#ifdef __FL_RECORD_BOOK
	if (IsPlayer())
		CRecordBook::GetInstance()->AddPlayerRecord(this, RecordType::DEATHS, 1, true);
#endif

#endif //__WORLDSERVER

	return 1;
}


#ifdef __WORLDSERVER
void CMover::AddPartyMemberExperience(CUser* pUser, EXPINTEGER nExp, int nFxp)
{
	if (nFxp)
	{
		if (pUser->AddFxp(nFxp))
		{
			g_UserMng.AddSetFlightLevel(pUser, pUser->GetFlightLv());
		}
		pUser->AddSetFxp(pUser->m_nFxp, pUser->GetFlightLv());
	}

	if (nExp > prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp)
		nExp = prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp;

	if (pUser->AddExperience(nExp, TRUE, TRUE, TRUE))
		pUser->LevelUpSetting();

	pUser->AddSetExperience(pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel);
}



void CMover::SubAroundExp(CMover* pAttacker, float fRange)
{
	D3DXVECTOR3	vPos = GetPos();
	D3DXVECTOR3 vDist;
	FLOAT		fDistSq;
	CUser* pUser;
	CUser* pList[512], ** ptr;
	int		nMaxList = 0;
	int		i;

	fRange *= fRange;

	memset(pList, 0, sizeof(pList));
	ptr = pList;


	FOR_VISIBILITYRANGE(this)
	{
		pUser = USERPTR;
		vDist = vPos - pUser->GetPos();
		fDistSq = D3DXVec3LengthSq(&vDist);
		if (fDistSq <= fRange)
		{
			*ptr++ = pUser;
			nMaxList++;
		}
	}
	NEXT_VISIBILITYRANGE(this)

		if (nMaxList == 0)
			return;

	EXPINTEGER nExp = GetProp()->nExpValue / (EXPINTEGER)nMaxList;

	ptr = pList;
	for (i = 0; i < nMaxList; i++)
	{
		pUser = *ptr++;

		if (nExp > prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp)
			nExp = prj.m_aExpCharacter[pUser->m_nLevel].nLimitExp;

		if (pUser->AddExperience(nExp, TRUE, TRUE, TRUE))
			pUser->LevelUpSetting();

		((CUser*)pUser)->AddSetExperience(pUser->GetExp1(), (WORD)pUser->m_nLevel, pUser->m_nSkillPoint, pUser->m_nSkillLevel);
	}
}

void CMover::AddKillRecovery()
{
	if (IsPlayer() == FALSE)	return;

	int nHPPoint = GetParam(DST_KILL_HP, 0);
	int nMPPoint = GetParam(DST_KILL_MP, 0);
	int nFPPoint = GetParam(DST_KILL_FP, 0);

	BOOL bSfx = FALSE;
	if (nHPPoint)
	{
		int nHP = GetPointParam(DST_HP);
		int nHPMax = GetMaxPoint(DST_HP);
		if (nHP + nHPPoint >= nHPMax)
			nHP = nHPMax;
		else
			nHP = nHP + nHPPoint;
		SetPointParam(DST_HP, nHP);
		bSfx = TRUE;
	}

	if (nMPPoint)
	{
		int nMP = GetPointParam(DST_MP);
		int nMPMax = GetMaxPoint(DST_MP);
		if (nMP + nMPPoint >= nMPMax)
			nMP = nMPMax;
		else
			nMP = nMP + nMPPoint;
		SetPointParam(DST_MP, nMP);
		bSfx = TRUE;
	}

	if (nFPPoint)
	{
		int nFP = GetPointParam(DST_FP);
		int nFPMax = GetMaxPoint(DST_FP);
		if (nFP + nFPPoint >= nFPMax)
			nFP = nFPMax;
		else
			nFP = nFP + nFPPoint;
		SetPointParam(DST_FP, nFP);
		bSfx = TRUE;
	}

	if (bSfx)
		g_UserMng.AddCreateSfxObj(this, XI_KILL_RECOVERY);
}


int CMover::SubExperience(CMover* pDead)
{
	if (IsPlayer() == FALSE)	return 0;
	if (pDead->IsPlayer())	return 0;

	MoverProp* pMoverProp = pDead->GetProp();
	ASSERT(pMoverProp);

	EXPFLOAT fExpValue = 0;
	float	fFxpValue = 0.0f;
	fExpValue = pMoverProp->nExpValue * static_cast<EXPFLOAT>(pMoverProp->m_fExp_Rate);
	fFxpValue = pMoverProp->nFxpValue * pMoverProp->m_fExp_Rate;

	if (IsMode(MODE_EXPUP_STOP))
		fExpValue = 0;

	const int nMobLevel = pDead->GetLevel();
	const int nPlayerLevel = GetLevel();

	if ((nMobLevel - nPlayerLevel) > 14)
	{
		if (fExpValue != 0)
			((CUser*)this)->AddText("You cannot gain experience from a monster that is 15 or more levels higher than you.", 0xffffffff);

		return 0;
	}

	AddExperienceKillMember(pDead, fExpValue, pMoverProp, fFxpValue);
	return 1;
}

BOOL CMover::IsValidArea(CMover* pMover, float fLength)
{
	float fDist;
	D3DXVECTOR3	vDist;
	if (IsValidObj((CObj*)this) && IsValidObj((CObj*)pMover) && GetWorld() == pMover->GetWorld()
#ifdef __LAYER_1015
		&& GetLayer() == pMover->GetLayer()
#endif // __LAYER_1015
		)
	{
		vDist = pMover->GetPos() - GetPos();
		fDist = D3DXVec3LengthSq(&vDist);
		if (fDist < fLength * fLength)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::GetPartyMemberFind(CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize)
{
	CUser* pUsertmp = NULL;
	D3DXVECTOR3	vDist;

	for (int i = 0; i < pParty->m_nSizeofMember; i++)
	{
		pUsertmp = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
		if (IsValidArea((CMover*)pUsertmp, 64.0f))
		{
			apMember[(*nMemberSize)++] = pUsertmp;
			(*nTotalLevel) += pUsertmp->GetLevel();
			if ((*nMaxLevel10) < pUsertmp->GetLevel())
			{
				(*nMaxLevel) = (*nMaxLevel10) = pUsertmp->GetLevel();
			}
		}
	}

	if (0 < (*nMaxLevel10) - 20)
	{
		(*nMaxLevel10) -= 20;
	}
	else
	{
		(*nMaxLevel10) = 0;
	}

	if ((*nMemberSize) == 0 || (*nTotalLevel) == 0)
	{
		return FALSE;
	}

	return TRUE;
}

void CMover::AddExperienceKillMember(CMover* pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue)
{
	vector<OBJID>	adwEnemy;
	vector<int>		anHitPoint;
	DWORD	dwMaxEnemyHit = 0;
	for (SET_OBJID::iterator it = pDead->m_idEnemies.begin(); it != pDead->m_idEnemies.end(); ++it)
	{
		adwEnemy.push_back((*it).first);
		anHitPoint.push_back((*it).second.nHit);
		dwMaxEnemyHit += (*it).second.nHit;
	}

	if (adwEnemy.size() > 1024)
	{
		Error("CMover::AddExperienceKillMember - enemy size is too big");
	}

	if (dwMaxEnemyHit == 0)
		return;

	for (DWORD j = 0; j < adwEnemy.size(); j++)
	{
		if (adwEnemy[j] == 0)
			continue;
		CMover* pEnemy = prj.GetMover(adwEnemy[j]);
		if (IsValidObj(pEnemy) && pDead->IsValidArea(pEnemy, 64.0f) && pEnemy->IsPlayer())
		{
			DWORD dwHitPointParty = 0;
			CParty* pParty = g_PartyMng.GetParty(pEnemy->m_idparty);
			if (pParty && pParty->IsMember(pEnemy->m_idPlayer))
			{
				dwHitPointParty = anHitPoint[j];
				for (DWORD k = j + 1; k < adwEnemy.size(); k++)
				{
					if (adwEnemy[k] == 0)
						continue;
					CMover* pEnemy2 = prj.GetMover(adwEnemy[k]);
					if (IsValidObj(pEnemy2) && pDead->IsValidArea(pEnemy2, 64.0f) && pEnemy2->IsPlayer())
					{
						if (pEnemy->m_idparty == pEnemy2->m_idparty && pParty->IsMember(pEnemy2->m_idPlayer))
						{
							dwHitPointParty += anHitPoint[k];
							adwEnemy[k] = 0;
						}
					}
					else
					{
						adwEnemy[k] = 0;
					}
				}
			}
			if (dwHitPointParty > 0)
				anHitPoint[j] = dwHitPointParty;
			float fExpValuePerson = (float)(fExpValue * (float(anHitPoint[j]) / float(dwMaxEnemyHit)));
			if (dwHitPointParty)
			{
				int nTotalLevel = 0;
				int nMaxLevel10 = 0;
				int nMaxLevel = 0;
				int nMemberSize = 0;
				CUser* apMember[MAX_PTMEMBER_SIZE];
				memset(apMember, 0, sizeof(apMember));

				if (pEnemy->GetPartyMemberFind(pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize) == FALSE)
					break;

				if (1 < nMemberSize)
					pEnemy->AddExperienceParty(pDead, fExpValuePerson, pMoverProp, fFxpValue, pParty, apMember, &nTotalLevel, &nMaxLevel10, &nMaxLevel, &nMemberSize);
				else
					pEnemy->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, TRUE);
			}
			else
			{
				pEnemy->AddExperienceSolo(fExpValuePerson, pMoverProp, fFxpValue, FALSE);
			}
		}
	}
}

void CMover::AddExperienceSolo(EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, BOOL bParty)
{
	int dw_Level = GetLevel() - (int)pMoverProp->dwLevel;
	if (dw_Level > 0)
	{
		if (1 == dw_Level || dw_Level == 2)
		{
			fExpValue *= static_cast<EXPFLOAT>(0.7f);
			fFxpValue *= 0.7f;
		}
		else if (3 == dw_Level || dw_Level == 4)
		{
			fExpValue *= static_cast<EXPFLOAT>(0.4f);
			fFxpValue *= 0.4f;
		}
		else
		{
			fExpValue *= static_cast<EXPFLOAT>(0.1f);
			fFxpValue *= 0.1f;
		}
	}

	if (fExpValue > static_cast<EXPFLOAT>(prj.m_aExpCharacter[m_nLevel].nLimitExp))
		fExpValue = static_cast<EXPFLOAT>(prj.m_aExpCharacter[m_nLevel].nLimitExp);


	if (fFxpValue)
	{
		if (AddFxp((int)fFxpValue))
		{
#ifdef __WORLDSERVER
			g_UserMng.AddSetFlightLevel(this, GetFlightLv());
#endif	// __WORLDSERVER
		}
		else
		{

		}
#ifdef __WORLDSERVER
		((CUser*)this)->AddSetFxp(m_nFxp, GetFlightLv());
#endif	// __WORLDSERVER
	}


	if (AddExperience(static_cast<EXPINTEGER>(fExpValue), TRUE, TRUE, TRUE))	// lv up
#		ifdef __WORLDSERVER
	((CUser*)this)->LevelUpSetting();
#		endif	// __WORLDSERVER

#		ifdef __WORLDSERVER
	((CUser*)this)->AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);
#		endif	// __WORLDSERVER
}

void CMover::AddExperienceParty(CMover* pDead, EXPFLOAT fExpValue, MoverProp* pMoverProp, float fFxpValue, CParty* pParty, CUser* apMember[], int* nTotalLevel, int* nMaxLevel10, int* nMaxLevel, int* nMemberSize)
{

	float fFactor = GetExperienceReduceFactor((int)pMoverProp->dwLevel, *nMaxLevel);
	fExpValue *= static_cast<EXPFLOAT>(fFactor);
	fFxpValue *= fFactor;


	pParty->GetPoint((*nTotalLevel), (*nMemberSize), pDead->GetLevel());

	if (pParty->m_nKindTroup)
	{
		switch (pParty->m_nTroupsShareExp)
		{
		case 1:
		{
			AddExperiencePartyContribution(pDead, apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10));
		}
		break;
		case 2:
		{

		}
		break;
		default:
		{
			AddExperiencePartyLevel(apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10));
		}
		break;
		}
	}
	else
	{

		AddExperiencePartyLevel(apMember, pParty, fExpValue, fFxpValue, (*nMemberSize), (*nMaxLevel10));
	}
}


void CMover::AddExperiencePartyContribution(CMover* pDead, CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10)
{
	EXPINTEGER	nMemberExp;
	int nAttackMember = 0;
	float fMaxMemberLevel = 0.0f;
	for (int i = 0; i < nMemberSize; i++)
	{
		float fContribution = 0;
		int nHit = pDead->GetEnemyHit(apMember[i]->GetId());
		if (nHit)
		{
			++nAttackMember;
		}
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}
	float fAddExp = 0.0f;
	fAddExp = (float)((fExpValue * 0.2f) * (pParty->m_nSizeofMember - 1));
	float fFullParty = 0.0f;
	if (nMemberSize == MAX_PTMEMBER_SIZE_SPECIAL)
	{
		fFullParty = (float)((fExpValue * 0.1f));
	}
	float fOptionExp = 0.0f;
	if (1 < nAttackMember)
	{
		fOptionExp = (float)((fExpValue * (float)nAttackMember / 100.0f));
	}

	for (int i = 0; i < nMemberSize; i++)
	{
		float fContribution = 0;
		int nHit = pDead->GetEnemyHit(apMember[i]->GetId());
		if (nHit)
		{
			fContribution = (float)nHit * 100 / (float)pDead->GetMaxHitPoint();
			if (100 < fContribution)
			{
				fContribution = 100.0f;
			}
			if (nMaxLevel10 < apMember[i]->GetLevel())
			{
				nMemberExp = static_cast<EXPINTEGER>((fExpValue * (fContribution / 100.0f)) + (fAddExp * (((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel)) + fOptionExp + fFullParty);
				AddPartyMemberExperience(apMember[i], nMemberExp, 0);
			}
		}
		else
		{
			if (nMaxLevel10 < apMember[i]->GetLevel())
			{
				nMemberExp = static_cast<EXPINTEGER>((fAddExp * (((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel)) + fOptionExp + fFullParty);
				AddPartyMemberExperience(apMember[i], nMemberExp, 0);
			}
		}
	}
}

void CMover::AddExperiencePartyLevel(CUser* apMember[], CParty* pParty, EXPFLOAT fExpValue, float fFxpValue, int nMemberSize, int nMaxLevel10)
{
	EXPINTEGER nMemberExp;
	float fMaxMemberLevel = 0.0f;
	for (int i = 0; i < nMemberSize; i++)
	{
		fMaxMemberLevel += ((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel());
	}
	float fAddExp = 0.0f;
	fAddExp = (float)((fExpValue * 0.2f) * (nMemberSize - 1));

	for (int i = 0; i < nMemberSize; i++)
	{
		if (nMaxLevel10 < apMember[i]->GetLevel())
		{
			nMemberExp = static_cast<EXPINTEGER>(((fExpValue + fAddExp) * (((float)apMember[i]->GetLevel() * (float)apMember[i]->GetLevel()) / fMaxMemberLevel))); //+ fFullAddExp );
			AddPartyMemberExperience(apMember[i], nMemberExp, 0);
		}
	}
}


float CMover::GetExperienceReduceFactor(int nLevel, int nMaxLevel)
{
	float fFactor = 1.0f;
	int nDelta = nMaxLevel - nLevel;
	if (nDelta > 0)
	{
		nDelta = min(nDelta, 9);

		if (::GetLanguage() == LANG_KOR)
		{
			float fFactors[10] = { 0.7f, 0.7f, 0.7f, 0.4f, 0.4f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };
			return fFactors[nDelta];
		}
		else
		{
			float fFactors[10] = { 0.8f, 0.8f, 0.6f, 0.35f, 0.2f, 0.12f, 0.08f, 0.04f, 0.02f, 0.01f };
			return fFactors[nDelta];
		}
	}

	return fFactor;
}
#endif	// __WORLDSERVER not client





BOOL CMover::IsAttackAble(CObj* pObj)
{
	if (HasBuffByIk3(IK3_TEXT_DISGUISE))
		return FALSE;

	BOOL bAble = FALSE;

	CWorld* pWorld = GetWorld();
	if (pObj)
	{
		if (IsStateMode(STATE_BASEMOTION_MODE))
		{
#ifdef __CLIENT
			g_DPlay.SendStateModeCancel(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
#else
			SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
			m_nReadyTime = 0;
			m_dwUseItemId = 0;
#endif
			return FALSE;
		}

		if (pObj->GetType() == OT_MOVER)
		{
#ifdef __CLIENT
			CMover* pMover = (CMover*)pObj;
			if (pMover->IsPlayer())
			{
#ifdef __JEFF_11_4
				if (IsArenaTarget(pMover))
					bAble = TRUE;
#endif	// __JEFF_11_4
#ifdef __FL_FFA
				else if (pMover->GetWorld() && pMover->GetWorld()->IsFFA())
					bAble = TRUE;
#endif
#ifdef __FL_TDM
				else if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->IsCanKillTDM(pMover))
					bAble = TRUE;
#endif

				if (g_eLocal.GetState(EVE_GUILDCOMBAT))
				{
					if (pWorld->GetID() == WI_WORLD_GUILDWAR)
					{
						if (IsGuildCombatTarget(pMover)) // GuildCombat
							bAble = TRUE;
					}
				}
			}
			else
				bAble = IsAttackAbleNPC(pMover);
#else // __CLIENT
			if (GetHitType((CMover*)pObj, TRUE, 0) == HITTYPE_FAIL)
				bAble = FALSE;
			else
				bAble = TRUE;
#endif // __CLIENT
		}
		else
			bAble = TRUE;

#ifdef __CLIENT
		if (!bAble)
			((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = FALSE;
#endif
	}

	return bAble;
}

BOOL CMover::IsAttackAbleNPC(CMover* pNPC)
{
	if (HasBuffByIk3(IK3_TEXT_DISGUISE))
		return FALSE;

	BOOL bAble = TRUE;
	if (pNPC->IsDie())
	{
#ifdef __CLIENT
		g_WorldMng()->SetObjFocus(NULL);
#else
		if (IsPlayer())
			((CUser*)this)->AddSetTarget(NULL_ID);
#endif
		bAble = FALSE;
	}
	else
	{
		MoverProp* pMoverProp = pNPC->GetProp();

		if (pMoverProp->bKillable == 0)
			return FALSE;


		if (pNPC->IsPeaceful() == TRUE)
		{
			bAble = FALSE;
		}
		else
		{
			if (m_pActMover->IsFly() && pMoverProp->dwFlying == 0)
				bAble = FALSE;
			else if (m_pActMover->IsFly() == FALSE && pMoverProp->dwFlying == 1)
				bAble = FALSE;
		}
	}

#ifdef __CLIENT
	if (bAble == TRUE)
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
#endif // Client

	return bAble;
}

void CMover::PrintString(CMover* pMover, DWORD dwId)
{
#ifdef __CLIENT
	g_WndMng.PutString(prj.GetText(dwId), NULL, prj.GetTextColor(dwId));
#endif // __CLIENT
#ifdef __WORLDSERVER
	((CUser*)this)->AddDefinedText(dwId, "");
	if (IsValidObj(pMover))
		((CUser*)pMover)->AddDefinedText(dwId, "");
#endif // __WORLDSERVER
}
#ifdef __WORLDSERVER
int CMover::IsSteal(CMover* pTarget)
{
	if (IsNPC() || ::GetLanguage() == LANG_JAP)
		return 0;

	if (pTarget->GetIndex() == MI_DEMIAN5 || pTarget->GetIndex() == MI_KEAKOON5 || pTarget->GetIndex() == MI_MUFFRIN5)
		return 0;

	if (pTarget->IsNPC())
	{
		BOOL bStealCheck = TRUE;
		if (pTarget->m_idTargeter == NULL_ID)
		{
			return 0;
		}
		CMover* pOtherPlayer = prj.GetMover(pTarget->m_idTargeter);
		if (IsValidObj(pOtherPlayer))
		{

			if (m_idparty && (pOtherPlayer->m_idparty == m_idparty))
				bStealCheck = FALSE;
			if (pTarget->GetProp()->dwClass == RANK_SUPER)
				bStealCheck = FALSE;
		}
		else
			bStealCheck = FALSE;

		if (bStealCheck &&
			pTarget->m_idTargeter != NULL_ID && pTarget->m_idTargeter != GetId())
		{
			((CUser*)this)->AddDefinedText(TID_GAME_PRIORITYMOB, "");
			return 1;
		}
	}

	return 0;
}

BOOL CMover::DropItemByDied(CMover* pAttacker)
{
	BOOL bResult;
	OBJID objid = GetMaxEnemyHitID();
	if (objid != NULL_ID)
	{
		CMover* pMover = prj.GetMover(objid);
		if (IsValidObj(pMover))
			bResult = DropItem(pMover);
		else
			objid = NULL_ID;
	}

	if (objid == NULL_ID)
		bResult = DropItem(pAttacker);

	return bResult;
}
#endif // WORLDSERVER

BOOL CMover::DropItem(CMover* pAttacker)
{
	MoverProp* lpMoverProp = GetProp();
#ifdef __WORLDSERVER
	if (pAttacker->IsPlayer() && IsNPC())
	{
		if (!lpMoverProp)
			return FALSE;

		if (m_nQuestKeeping > 0)
		{
			CGuildQuestProcessor* pProcessor = CGuildQuestProcessor::GetInstance();
			GUILDQUESTELEM* pElem = pProcessor->GetGuildQuest(m_nQuestKeeping);
			if (pElem && pElem->objidWormon == GetId())
			{
				CGuild* pGuild = pAttacker->GetGuild();
				if (pGuild)
				{
					pGuild->SetQuest(pElem->nId, pElem->ns);
					g_dpDBClient.SendUpdateGuildQuest(pGuild->m_idGuild, pElem->nId, pElem->ns);
					pElem->nProcess = GQP_GETITEM;
					pElem->dwEndTime = GetTickCount() + MIN(20);
					pElem->ns = pElem->nf = 0;
					pElem->nState = 0;
					pElem->objidWormon = NULL_ID;
				}
			}
		}

		if (m_nPartyQuestKeeping > 0)
		{
			CPartyQuestProcessor* pProcessor = CPartyQuestProcessor::GetInstance();
			PARTYQUESTELEM* pElem = pProcessor->GetPartyQuest(m_nPartyQuestKeeping);
			if (pElem && pElem->objidWormon == GetId())
			{
				CParty* pParty = g_PartyMng.GetParty(pAttacker->m_idparty);
				if (pParty)
				{
					pElem->nProcess = PQP_GETITEM;
					pElem->dwEndTime = GetTickCount() + MIN(20);
					pElem->ns = pElem->nf = 0;
					pElem->nState = 0;
					pElem->objidWormon = NULL_ID;

					pProcessor->SendQuestLimitTime(PQP_GETITEM, MIN(20), pAttacker->m_idparty);
				}

			}
		}

		if ((pAttacker->m_nLevel - (int)lpMoverProp->dwLevel) < 10)
		{
			map<DWORD, int> mapItemList = prj.m_EventLua.GetItem(lpMoverProp->dwLevel);
			for (map<DWORD, int>::iterator it = mapItemList.begin(); it != mapItemList.end(); it++)
			{
#ifdef __BUGFIX_0326
				if (lpMoverProp->dwFlying)
				{
					CItemElem itemElem;
					itemElem.m_dwItemId = it->first;
					itemElem.m_nItemNum = it->second;
					itemElem.SetSerialNumber();
					if (pAttacker->CreateItem(&itemElem) == TRUE)
					{
					}
				}
				else
#endif	// __BUGFIX_0326
				{
					CItemElem* pItemElem = new CItemElem;
					pItemElem->m_dwItemId = it->first;
					pItemElem->m_nItemNum = it->second;
					pItemElem->m_nHitPoint = -1;
					pItemElem->SetSerialNumber();
					CItem* pItem = new CItem;
					pItem->m_pItemBase = pItemElem;
					pItem->m_idOwn = pAttacker->GetId();
					pItem->m_dwDropTime = timeGetTime();
					pItem->m_bDropMob = TRUE;
					pItem->SetIndex(D3DDEVICE, pItem->m_pItemBase->m_dwItemId);
					D3DXVECTOR3 vPos = GetPos();
					vPos.x += (xRandomF(2.0f) - 1.0f);
					vPos.z += (xRandomF(2.0f) - 1.0f);
					pItem->SetPos(vPos);
					if (GetWorld()->ADDOBJ(pItem, TRUE, GetLayer()) == FALSE)
					{
						SAFE_DELETE(pItem);
					}
				}
			}
		}

		QUESTITEM* pQuestItem;
		short nNum;
		u_long uSizeOfQuestItem = lpMoverProp->m_QuestItemGenerator.GetSize();
		for (u_long i = 0; i < uSizeOfQuestItem; i++)
		{
			pQuestItem = lpMoverProp->m_QuestItemGenerator.GetAt(i);

#ifndef __FL_MONSTER_ALBUM
			LPQUEST lpQuest = pAttacker->FindQuest(pQuestItem->dwQuest);
			if (lpQuest && lpQuest->m_nState == pQuestItem->dwState)
			{
#else
			bool bGood = false;
			if (pQuestItem->dwQuest == QUEST_SPECIAL_MONSTERALBUM)
				bGood = true;
			else
			{
				LPQUEST lpQuest = pAttacker->FindQuest(pQuestItem->dwQuest);
				if (lpQuest && lpQuest->m_nState == pQuestItem->dwState)
					bGood = true;
			}

			if (bGood)
			{
#endif
				DWORD dwProbability = pQuestItem->dwProbability;

				if (pQuestItem->dwQuest == QUEST_VALENTINE || pQuestItem->dwQuest == QUEST_WHITEDAY)
				{
					float f;
					int  d = pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
					if (d <= 1)	f = 1.0f;
					else if (d <= 2)	f = 0.8f;
					else if (d <= 4)	f = 0.6f;
					else if (d <= 7)	f = 0.3f;
					else	f = 0.1f;
					dwProbability = (DWORD)(dwProbability * f);
				}

				if (xRandom(3000000000) <= dwProbability)
				{
					if (pQuestItem->dwNumber == 0)
						Error("CMover::DropItem : %s�� quest item drop %d��°�� dwNumber�� 0", GetName(), i);
					nNum = (short)(xRandom(pQuestItem->dwNumber) + 1);
					if (pQuestItem->dwIndex == 0)
					{
						WriteLog("%s, %d\r\n\t%s", __FILE__, __LINE__, lpMoverProp->szName);
						break;
					}
					CParty* pParty = g_PartyMng.GetParty(pAttacker->m_idparty);
#ifndef __FL_MONSTER_ALBUM
					if (pParty && pParty->IsMember(pAttacker->m_idPlayer))	// party
#else
					if (pQuestItem->dwQuest != QUEST_SPECIAL_MONSTERALBUM && pParty && pParty->IsMember(pAttacker->m_idPlayer))
#endif
					{
						for (int j = 0; j < pParty->GetSizeofMember(); ++j)
						{
							PartyMember* pPartyMember = &pParty->m_aMember[j];
							CMover* pMember = prj.GetUserByID(pPartyMember->m_uPlayerId);
							if (!IsValidObj(pMember) || !IsValidArea(pMember, 64.0f))
								continue;

							LPQUEST pMemberQuest = pMember->FindQuest(pQuestItem->dwQuest);
							if (pMemberQuest && pMemberQuest->m_nState == pQuestItem->dwState)
							{
								CItemElem itemElem;
								itemElem.m_dwItemId = pQuestItem->dwIndex;
								itemElem.m_nItemNum = nNum;
								itemElem.m_nHitPoint = -1;
								if (pMember->CreateItem(&itemElem) == FALSE)
								{
									CItemElem* pItemElem = new CItemElem;
									pItemElem->m_dwItemId = pQuestItem->dwIndex;
									pItemElem->m_nItemNum = nNum;
									ItemProp* pItemProp = pItemElem->GetProp();
									if (pItemProp)
										pItemElem->m_nHitPoint = pItemProp->dwEndurance;
									pItemElem->SetSerialNumber();
									CItem* pItem = new CItem;
									pItem->m_pItemBase = pItemElem;
									if (pItemElem->m_dwItemId == 0)
										Error("DropItem:1st %s\n", GetName());
									pItem->SetIndex(D3DDEVICE, pItemElem->m_dwItemId);

									D3DXVECTOR3 vPosMember = pMember->GetPos();
									pItem->SetPos(vPosMember);

									pItem->SetAngle((float)(xRandom(360)));
									pItem->m_idHolder = pMember->m_idPlayer;
									pItem->m_dwDropTime = timeGetTime();
									if (pMember->GetWorld()->ADDOBJ(pItem, TRUE, GetLayer()) == FALSE)
									{
										SAFE_DELETE(pItem);
									}
								}
								else
									((CUser*)pMember)->AddDefinedText(TID_EVE_REAPITEM, "\"%s\"", prj.GetItemProp(pQuestItem->dwIndex)->szName);
							}
						}
					}
					else	// solo
					{
						CItemElem itemElem;
						itemElem.m_dwItemId = pQuestItem->dwIndex;
						itemElem.m_nItemNum = nNum;
						itemElem.m_nHitPoint = -1;
						if (pAttacker->CreateItem(&itemElem) == FALSE)
						{
							CItemElem* pItemElem = new CItemElem;
							pItemElem->m_dwItemId = pQuestItem->dwIndex;
							pItemElem->m_nItemNum = nNum;
							ItemProp* pItemProp = pItemElem->GetProp();
							if (pItemProp)
								pItemElem->m_nHitPoint = pItemProp->dwEndurance;
							pItemElem->SetSerialNumber();
							CItem* pItem = new CItem;
							pItem->m_pItemBase = pItemElem;
							if (pItemElem->m_dwItemId == 0)
								Error("DropItem:1st %s\n", GetName());
							pItem->SetIndex(D3DDEVICE, pItemElem->m_dwItemId);

							D3DXVECTOR3 vPosAttacker = pAttacker->GetPos();
							pItem->SetPos(vPosAttacker);

							pItem->SetAngle((float)(xRandom(360)));
							pItem->m_idHolder = pAttacker->m_idPlayer;
							pItem->m_dwDropTime = timeGetTime();
							if (pAttacker->GetWorld()->ADDOBJ(pItem, TRUE, GetLayer()) == FALSE)
							{
								SAFE_DELETE(pItem);
							}
						}
						else
							((CUser*)pAttacker)->AddDefinedText(TID_EVE_REAPITEM, "\"%s\"", prj.GetItemProp(pQuestItem->dwIndex)->szName);
					}
				}
			}
		}

		int nloop = 1;
		BOOL bUnique = FALSE;
		if (pAttacker->m_idparty)
		{
			CParty* pParty = g_PartyMng.GetParty(pAttacker->m_idparty);
			if (pParty)
			{
				if (pParty->m_nKindTroup == 1)
				{
					if (pParty->m_nModeTime[PARTY_GIFTBOX_MODE] || pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
					{
						if (pParty->m_nModeTime[PARTY_PARSKILL_MODE])
						{
							if (pParty->m_nModeTime[PARTY_GIFTBOX_MODE])
								nloop = 2;
							if (pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
								bUnique = TRUE;
						}
						else
						{
							CUser* pLeader = g_UserMng.GetUserByPlayerID(pParty->m_aMember[0].m_uPlayerId);
							if (IsValidObj((CObj*)pLeader)/* && pLeader->IsNearPC( (CUser*)pAttacker ) && IsOrigin()*/)
							{
								if (pLeader->IsValidArea(pAttacker, 255.0f))
								{
									if (pParty->m_nModeTime[PARTY_GIFTBOX_MODE])
										nloop = 2;
									if (pParty->m_nModeTime[PARTY_FORTUNECIRCLE_MODE])
										bUnique = TRUE;
								}
							}
						}
					}
				}
			}
		}
		if (pAttacker->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_GET01))
			nloop += 1;
		if (pAttacker->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_GET02))
			nloop += 2;
#ifdef __DST_GIFTBOX
		nloop += pAttacker->GetAdjParam(DST_GIFTBOX);
#endif // __DST_GIFTBOX

#ifdef __EVENT_MONSTER
		if (CEventMonster::GetInstance()->SetEventMonster(lpMoverProp->dwID))
			if (!CEventMonster::GetInstance()->IsGiftBoxAble())
				nloop = 1;
#endif // __EVENT_MONSTER

		D3DXVECTOR3 vPos;
		for (int k = 0; k < nloop; k++)
		{
			int nProbability = 100, nPenyaRate = 100;
			BOOL bAdjDropRate = TRUE;

			if (GetIndex() == MI_CLOCKWORK1)
				bAdjDropRate = FALSE;
#ifdef __EVENT_MONSTER
			if (CEventMonster::GetInstance()->SetEventMonster(lpMoverProp->dwID))
				bAdjDropRate = FALSE;
#endif // 
#ifdef __EVENTLUA_SPAWN
			if (prj.m_EventLua.IsEventSpawnMonster(lpMoverProp->dwID))
				bAdjDropRate = FALSE;
#endif // 
			if (bAdjDropRate)
			{
				int d = pAttacker->m_nLevel - (int)lpMoverProp->dwLevel;
				if (d <= 1) { nProbability = 100;	nPenyaRate = 100; }
				else if (d <= 2) { nProbability = 80;	nPenyaRate = 100; }
				else if (d <= 4) { nProbability = 60;	nPenyaRate = 80; }
				else if (d <= 7) { nProbability = 30;	nPenyaRate = 65; }
				else { nProbability = 10;	nPenyaRate = 50; }
			}

			FLOAT fItemDropRate = nProbability * GetItemDropRateFactor(pAttacker);

			if (xRandom(100) < fItemDropRate)
			{
				int nSize = lpMoverProp->m_DropItemGenerator.GetSize();
				int nNumber = 0;
				DROPITEM* lpDropItem;

				for (int i = 0; i < nSize; i++)
				{
					if ((lpDropItem = lpMoverProp->m_DropItemGenerator.GetAt(i, bUnique, GetPieceItemDropRateFactor(pAttacker))) != NULL)
					{
						if (lpDropItem->dtType == DROPTYPE_NORMAL)
						{
							ItemProp* pProp = prj.GetItemProp(lpDropItem->dwIndex);
							if (!pProp)
							{
								Error("CMover::DropItemByDied() [ItemID:%d][MoverID:%d]", lpDropItem->dwIndex, GetProp()->dwID);
								continue;
							}
							DWORD dwNum = lpDropItem->dwNumber;
							if (dwNum == (DWORD)-1)
								dwNum = 1;

							if (lpMoverProp->dwFlying)
							{
								CItemElem itemElem;
								itemElem.m_dwItemId = lpDropItem->dwIndex;
								itemElem.m_nItemNum = (short)(xRandom(dwNum) + 1);
								itemElem.SetAbilityOption(lpDropItem->dwLevel);
								#ifdef __WEAPON_RARITY
								if (IsValidRarityItem(itemElem.GetProp()->dwItemKind3) || IsValidRarityItem2(itemElem.GetProp()->dwItemKind3))
								{
									BOOL bCheck = FALSE;
									if (!bCheck && xRandom(0, 10000) < 10)
									{
										itemElem.m_nWeaponRarity = 5;
										bCheck = TRUE;
									}
									if (!bCheck && xRandom(0, 10000) < 15)
									{
										itemElem.m_nWeaponRarity = 4;
										bCheck = TRUE;
									}
									if (!bCheck && xRandom(0, 10000) < 20)
									{
										itemElem.m_nWeaponRarity = 3;
										bCheck = TRUE;
									}
									if (!bCheck && xRandom(0, 10000) < 25)
									{
										itemElem.m_nWeaponRarity = 2;
										bCheck = TRUE;
									}
									if (!bCheck)
										itemElem.m_nWeaponRarity = 1;
								}
								else
									itemElem.m_nWeaponRarity = 0;
								#endif // __WEAPON_RARITY

								if (pAttacker->CreateItem(&itemElem) == TRUE)
								{	// log
									ItemProp* pItemProp = itemElem.GetProp();
									if (pItemProp)
									{
#ifndef __FL_FIX_USELESS_LOGS
										if (pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || (pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY))
										{
											switch (pItemProp->nLog)
											{
											case 1:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption());	break;
											case 2:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), 200);	break;
											case 3:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), 100);	break;
											}
										}
#endif
									}
									if (lpDropItem->dwNumber != (DWORD)-1)
										nNumber++;
									if ((DWORD)(nNumber) >= lpMoverProp->m_DropItemGenerator.m_dwMax)
										break;
								}
								continue;
							}
							CItemElem* pItemElem = new CItemElem;
							pItemElem->m_dwItemId = lpDropItem->dwIndex;
							pItemElem->m_nItemNum = (short)(xRandom(dwNum) + 1);
							ItemProp* pItemProp = pItemElem->GetProp();
							if (pItemProp)
							{
								pItemElem->m_nHitPoint = pItemProp->dwEndurance;
								pItemElem->SetRandomOpt(CRandomOptItemGen::GetInstance()->GenRandomOptItem(lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass));
							}
							pItemElem->SetAbilityOption(lpDropItem->dwLevel);
							#ifdef __WEAPON_RARITY
							if (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3) || IsValidRarityItem2(pItemElem->GetProp()->dwItemKind3))
							{
								BOOL bCheck = FALSE;
								if (!bCheck && xRandom(0, 10000) < 10)
								{
									pItemElem->m_nWeaponRarity = 5;
									bCheck = TRUE;
								}
								if (!bCheck && xRandom(0, 10000) < 15)
								{
									pItemElem->m_nWeaponRarity = 4;
									bCheck = TRUE;
								}
								if (!bCheck && xRandom(0, 10000) < 20)
								{
									pItemElem->m_nWeaponRarity = 3;
									bCheck = TRUE;
								}
								if (!bCheck && xRandom(0, 10000) < 25)
								{
									pItemElem->m_nWeaponRarity = 2;
									bCheck = TRUE;
								}
								if (!bCheck)
									pItemElem->m_nWeaponRarity = 1;
							}
							else
								pItemElem->m_nWeaponRarity = 0;
							#endif // __WEAPON_RARITY
							pItemElem->SetSerialNumber();
							CItem* pItem = new CItem;
							pItem->m_pItemBase = pItemElem;
							BOOL bJJim = TRUE;
							if (lpMoverProp->dwClass == RANK_SUPER)
								bJJim = FALSE;
							if (GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5)
								bJJim = FALSE;
							if (bJJim)
							{
								pItem->m_idOwn = pAttacker->GetId();
								pItem->m_dwDropTime = timeGetTime();
							}
							pItem->m_bDropMob = TRUE;
							if (pItem->m_pItemBase->m_dwItemId == 0) Error("DropItem:2nd %s\n", GetName());
							pItem->SetIndex(D3DDEVICE, pItem->m_pItemBase->m_dwItemId);

							vPos = GetPos();
							vPos.x += (xRandomF(2.0f) - 1.0f);
							vPos.z += (xRandomF(2.0f) - 1.0f);
							vPos.y = GetPos().y;
#ifdef __EVENT_MONSTER
							if (CEventMonster::GetInstance()->SetEventMonster(lpMoverProp->dwID))
							{
								pItem->m_idOwn = pAttacker->GetId();
								pItem->m_dwDropTime = timeGetTime();

								pItem->m_IdEventMonster = lpMoverProp->dwID;
								float fItemDropRange = CEventMonster::GetInstance()->GetItemDropRange();
								vPos = GetPos();
								vPos.x += (xRandomF(fItemDropRange) - (fItemDropRange / 2.0f));
								vPos.z += (xRandomF(fItemDropRange) - (fItemDropRange / 2.0f));
							}
#endif // __EVENT_MONSTER
							pItem->SetPos(vPos);

							if (GetWorld()->ADDOBJ(pItem, TRUE, GetLayer()) == TRUE)
							{
								if (lpDropItem->dwNumber != (DWORD)-1)
									nNumber++;

#ifndef __FL_FIX_USELESS_LOGS
								if (pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || (pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY))
								{
									switch (pItemProp->nLog)
									{
									case 1:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, pItemElem->GetAbilityOption());
										break;
									case 2:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, 200);
										break;
									case 3:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, 100);
										break;
									}
								}
#endif

								if (nNumber == lpMoverProp->m_DropItemGenerator.m_dwMax)
									break;
							}
							else
							{
								SAFE_DELETE(pItem);
							}
						}
						else if (lpDropItem->dtType == DROPTYPE_SEED && k == 0)
						{
							int	nSeedID = 0;
							int nNumGold = lpDropItem->dwNumber + xRandom(lpDropItem->dwNumber2 - lpDropItem->dwNumber);
							nNumGold = nNumGold * nPenyaRate / 100;
#ifdef __S1108_BACK_END_SYSTEM
							nNumGold = (int)(nNumGold * prj.m_fGoldDropRate * lpMoverProp->m_fPenya_Rate);
							if (nNumGold == 0)
								continue;
#else // 
							nNumGold *= prj.m_fGoldDropRate;
#endif // 

							nNumGold = (int)(nNumGold * prj.m_EventLua.GetGoldDropFactor());
#ifdef __FL_UPDATE_LORDEVENT
							nNumGold = (int)(nNumGold * CSLord::Instance()->GetEvent()->GetPFactor());
#endif
							pAttacker->PickupGold(nNumGold, TRUE);
						}
					}
				}

				nSize = lpMoverProp->m_DropKindGenerator.GetSize();
				DROPKIND* pDropKind;
				CPtrArray* pItemKindAry;
				int nAbilityOption; //, nDropLuck;
				BOOL bDrop = FALSE;
				for (int i = 0; i < nSize; i++)
				{
					bDrop = FALSE;
					pDropKind = lpMoverProp->m_DropKindGenerator.GetAt(i);
					pItemKindAry = prj.GetItemKindAry(pDropKind->dwIK3);
					int nMinIdx = -1, nMaxIdx = -1;
					for (int j = pDropKind->nMinUniq; j <= pDropKind->nMaxUniq; j++)
					{
						nMinIdx = prj.GetMinIdx(pDropKind->dwIK3, j);
						if (nMinIdx != -1)
							break;
					}
					for (int j = pDropKind->nMaxUniq; j >= pDropKind->nMinUniq; j--)
					{
						nMaxIdx = prj.GetMaxIdx(pDropKind->dwIK3, j);
						if (nMaxIdx != -1)
							break;
					}
					if (nMinIdx < 0 || nMaxIdx < 0)
					{
						continue;
					}
					ItemProp* pItemProp = (ItemProp*)pItemKindAry->GetAt(nMinIdx + xRandom(nMaxIdx - nMinIdx + 1));
					if (NULL == pItemProp)
					{
						continue;
					}

					nAbilityOption = xRandom(11);	// 0 ~ 10

					DWORD dwAdjRand;

					for (int k = nAbilityOption; k >= 0; k--)
					{
						DWORD dwPrabability = (DWORD)(prj.m_adwExpDropLuck[(pItemProp->dwItemLV > 120 ? 119 : pItemProp->dwItemLV - 1)][k]
							* ((float)lpMoverProp->dwCorrectionValue / 100.0f));

						dwAdjRand = xRandom(3000000000);
						if (bUnique && dwPrabability <= 10000000)
							dwAdjRand /= 2;

						if (dwAdjRand < dwPrabability)
						{
							if (lpMoverProp->dwFlying)
							{
								CItemElem itemElem;
								itemElem.m_dwItemId = pItemProp->dwID;
								itemElem.m_nItemNum = 1;
								itemElem.SetAbilityOption(k);
								itemElem.SetRandomOpt(CRandomOptItemGen::GetInstance()->GenRandomOptItem(lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass));

								if (pAttacker->CreateItem(&itemElem) == TRUE)
								{	// log
#ifndef __FL_FIX_USELESS_LOGS
									if (pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || (pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY))
									{
										switch (pItemProp->nLog)
										{
										case 1:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), itemElem.GetAbilityOption());	break;
										case 2:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), 200);	break;
										case 3:		g_dpDBClient.SendLogUniqueItem2(pAttacker, itemElem.GetProp(), 100);	break;
										}
									}
#endif

									break;
								}
							}

							CItemElem* pItemElem = new CItemElem;
							pItemElem->m_dwItemId = pItemProp->dwID;
							pItemElem->m_nItemNum = 1;
							pItemElem->m_nHitPoint = pItemProp->dwEndurance;
							pItemElem->SetAbilityOption(k);
							pItemElem->SetRandomOpt(CRandomOptItemGen::GetInstance()->GenRandomOptItem(lpMoverProp->dwLevel, (FLOAT)nProbability / 100.0f, pItemProp, lpMoverProp->dwClass));
							pItemElem->SetSerialNumber();
							CItem* pItem = new CItem;
							pItem->m_pItemBase = pItemElem;
							BOOL bJJim = TRUE;

							if (lpMoverProp->dwClass == RANK_SUPER)
								bJJim = FALSE;
							if (GetIndex() == MI_DEMIAN5 || GetIndex() == MI_KEAKOON5 || GetIndex() == MI_MUFFRIN5)
								bJJim = FALSE;
							if (bJJim)
							{
								pItem->m_idOwn = pAttacker->GetId();
								pItem->m_dwDropTime = timeGetTime();
							}
							pItem->m_bDropMob = TRUE;
#ifdef __EVENT_MONSTER
							if (CEventMonster::GetInstance()->SetEventMonster(lpMoverProp->dwID))
							{

								pItem->m_idOwn = pAttacker->GetId();
								pItem->m_dwDropTime = timeGetTime();

								pItem->m_IdEventMonster = lpMoverProp->dwID;
							}
#endif // __EVENT_MONSTER
							if (pItem->m_pItemBase->m_dwItemId == 0) Error("DropItem: 4th %s\n", GetName());
							pItem->SetIndex(D3DDEVICE, pItem->m_pItemBase->m_dwItemId);

							D3DXVECTOR3 vPos = GetPos();
							pItem->SetPos(vPos);

							if (GetWorld()->ADDOBJ(pItem, TRUE, GetLayer()) == TRUE)
							{
								bDrop = TRUE;

#ifndef __FL_FIX_USELESS_LOGS
								if (pItemProp->dwItemKind1 == IK1_WEAPON || pItemProp->dwItemKind1 == IK1_ARMOR || (pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY))
								{
									switch (pItemProp->nLog)
									{
									case 1:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, pItemElem->GetAbilityOption());	break;
									case 2:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, 200);	break;
									case 3:		g_dpDBClient.SendLogUniqueItem(pAttacker, pItem, 100);	break;
									}
								}
#endif
							}
							else
							{
								SAFE_DELETE(pItem);
							}
							break;
						}
					}

					if (GetProp()->dwClass == RANK_SUPER)
					{
						if (bDrop)
							break;
					}
				}
			}
		}
	}
#endif	// __WORLDSERVER

	return TRUE;
}



void CMover::SetJJim(CMover * pJJimer)
{
	if (GetProp()->dwClass == RANK_SUPER)
		return;

#ifdef __EVENT_MONSTER
	if (CEventMonster::GetInstance()->IsEventMonster(GetProp()->dwID))
		return;
#endif // __EVENT_MONSTER


	if (pJJimer->IsPlayer() && IsNPC())
	{
		m_idTargeter = pJJimer->GetId();
	}
}


CGuild* CMover::GetGuild()
{
#ifdef __WORLDSERVER
	//	locked
	CGuild* pGuild = g_GuildMng.GetGuild(m_idGuild);
	if (pGuild && pGuild->IsMember(m_idPlayer))
		return pGuild;
	return NULL;
#else	// __WORLDSERVER
#ifdef __CLIENT
#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
	if (GetNpcType() != NPC_TYPE_NORMAL)
	{
		if (m_idGuild > 0)
		{
			CGuild* pGuild = g_GuildMng.GetGuild(m_idGuild);
			if (pGuild)
				return pGuild;
		}
		return NULL;
	}
#endif
	if (m_idGuild > 0 && CMover::GetActiveMover() &&
		m_idGuild == CMover::GetActiveMover()->m_idGuild)
	{
		CGuild* pGuild = g_GuildMng.GetGuild(m_idGuild);
		if (pGuild && pGuild->IsMember(m_idPlayer))
			return pGuild;
		return NULL;
	}
	else
	{
		return g_GuildMng.GetGuild(m_idGuild);
	}
#endif	// __CLIENT
#endif	// __WORLDSERVER
}

#ifdef  __WORLDSERVER
void CMover::ProcessRecovery()
{
	if (IsPlayer() == FALSE || IsDie())
		return;

	if (IsAttackMode())
		return;

	DWORD dwCurTick = g_tmCurrent;
	if (dwCurTick <= m_dwTickRecovery)
		return;

	float fRecovery = 1.0f;

	m_dwTickRecovery = dwCurTick + NEXT_TICK_RECOVERY;

	CParty* pParty = g_PartyMng.GetParty(m_idparty);
	if (pParty && pParty->IsMember(m_idPlayer))
	{
		if (pParty->m_nKindTroup == 1 && pParty->m_nModeTime[PARTY_STRETCHING_MODE])
		{
			BOOL nMItem = FALSE;
			CUser* pLeader = g_UserMng.GetUserByPlayerID(pParty->m_aMember[0].m_uPlayerId);
			if (pParty->m_nModeTime[PARTY_PARSKILL_MODE])
			{
				nMItem = TRUE;
			}
			else
			{
				if (IsValidObj((CObj*)pLeader))
				{
					if (IsValidArea(pLeader, 255.0f))
						nMItem = TRUE;
				}
			}
			if (nMItem)
			{
				if (IsValidObj(pLeader) && pLeader->IsInteriorityJob(JOB_ASSIST)) //ty SPSquonK
					fRecovery = 1.8f;
				else
					fRecovery = 1.5f;
			}
		}
	}
	else
	{
		m_idparty = 0;
	}
	IncHitPoint((int)(GetHPRecovery() * fRecovery));
	IncManaPoint((int)(GetMPRecovery() * fRecovery));
	IncFatiguePoint((int)(GetFPRecovery() * fRecovery));
}
#endif // __WORLDSERVER

void CMover::AllocShopInventory(LPCHARACTER pCharacter)
{
	if (IsVendorNPC())
	{
		m_ShopInventory[0] = new CItemContainer< CItemElem  >; m_ShopInventory[0]->SetItemContainer(ITYPE_ITEM, MAX_VENDOR_INVENTORY);
		m_ShopInventory[1] = new CItemContainer< CItemElem  >; m_ShopInventory[1]->SetItemContainer(ITYPE_ITEM, MAX_VENDOR_INVENTORY);
		m_ShopInventory[2] = new CItemContainer< CItemElem  >; m_ShopInventory[2]->SetItemContainer(ITYPE_ITEM, MAX_VENDOR_INVENTORY);
		m_ShopInventory[3] = new CItemContainer< CItemElem  >; m_ShopInventory[3]->SetItemContainer(ITYPE_ITEM, MAX_VENDOR_INVENTORY);
	}
}

BOOL CMover::IsVendorNPC()
{
	LPCHARACTER pCharacter = GetCharacter();
	if (!pCharacter)
		return FALSE;
	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
	{
		if (pCharacter->m_venderItemAry[i].GetSize())
			return TRUE;
	}

	for (int i = 0; i < MAX_VENDOR_INVENTORY_TAB; i++)
	{
		if (pCharacter->m_venderItemAry2[i].GetSize())
			return TRUE;
	}

	return FALSE;
}

#ifdef __CLIENT
BOOL CMover::EndMotion()
{
	if (!m_pActMover->IsActAttack() &&
		!m_pActMover->IsActDamage() &&
		!m_pActMover->IsActJump() &&
		!m_pActMover->IsFly() &&
		!m_pActMover->IsDie())
	{
		SetAngle(GetDegree(GetWorld()->GetCamera()->m_vPos, GetPos()));
		SetMotion(MTI_LOGOUT, ANILOOP_CONT, MOP_FIXED);
		return TRUE;
	}
	return FALSE;
}
#endif //__CLIENT

BOOL CMover::IsBullet(ItemProp * pItemProp)
{

// 	if (pItemProp->dwLinkKindBullet != NULL_ID)
// 	{
// 		DWORD dwTip = TID_TIP_NEEDSKILLITEM;
// 		if (pItemProp->dwItemKind3 == IK3_BOW || pItemProp->dwLinkKind == IK3_BOW)
// 		{
// 			dwTip = TID_TIP_NEEDSATTACKITEM;
// 		}

// 		ItemProp* pProp = GetActiveHandItemProp(PARTS_BULLET);
// 		if (pProp)
// 		{
// 			if (pProp->dwItemKind2 != pItemProp->dwLinkKindBullet)
// 			{
// #ifdef __CLIENT
// 				g_WndMng.PutString(prj.GetText(dwTip), NULL, prj.GetTextColor(dwTip));
// 				((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = FALSE;
// #endif // __CLIENT
// #ifdef __WORLDSERVER
// 				((CUser*)this)->AddDefinedText(dwTip, "");
// #endif // __WORLDSERVER
// 				return FALSE;
// 			}
// 		}
// 		else
// 		{
// #ifdef __CLIENT
// 			g_WndMng.PutString(prj.GetText(dwTip), NULL, prj.GetTextColor(dwTip));
// #endif // __CLIENT
// #ifdef __WORLDSERVER
// 			((CUser*)this)->AddDefinedText(dwTip, "");
// #endif // __WORLDSERVER
// 			return FALSE;
// 		}
// 	}
	return TRUE;
}

#ifdef __WORLDSERVER
void CMover::ArrowDown()
{
	// if (IsNPC())
	// 	return;

	// CItemElem* pItemElem = m_Inventory.GetEquip(PARTS_BULLET);
	// if (!IsUsableItem(pItemElem))
	// 	return;

	// int nNewNum = pItemElem->m_nItemNum - 1;
	// if (nNewNum <= 0)
	// 	DoEquipItem(pItemElem);

	// RemoveItem((BYTE)(pItemElem->m_dwObjId), 1);
}
#endif // __WORLDSERVER

#if defined(__WORLDSERVER)
BOOL CMover::IsItemRedyTime(ItemProp * pItemProp, OBJID dwObjid, BOOL bItemFind)
{
	if (pItemProp->dwSkillReadyType != 0 &&
		pItemProp->dwSkillReadyType != 0xffffffff)
	{
		if (pItemProp->dwItemKind2 == IK2_RIDING)
		{
			DisableAllPets();

			if (m_dwFlag & MVRF_NOACTION)
				return FALSE;
		}

		if (IsStateMode(STATE_BASEMOTION_MODE))
		{
			if (m_nReadyTime != 0)
			{
				((CUser*)this)->AddDefinedText(TID_PK_BLINK_LIMIT, "");
				return FALSE;
			}
			else
			{
				SetStateNotMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_OFF);
			}
		}
		else
		{
			m_nReadyTime = timeGetTime() + pItemProp->dwSkillReadyType;
			m_dwUseItemId = dwObjid;
			m_bItemFind = bItemFind;
			SetStateMode(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_ON);
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __WORLDSERVER

#ifdef __CLIENT
CSfx* CMover::CreateSfxArrow(DWORD dwSfxObjArrow, DWORD dwSfxObjHit, D3DXVECTOR3 vPosDest, int idTarget)
{
	CSfx* pSfx = NULL;


	D3DXVECTOR3 vPos;
	CModelObject* pModel = (CModelObject*)m_pModel;
	pModel->GetHandPos(&vPos, PARTS_LWEAPON, GetMatrixWorld());

	pSfx = CreateSfx(D3DDEVICE, dwSfxObjArrow, vPos, GetId(), vPosDest, idTarget);

	if (dwSfxObjHit != NULL_ID)
	{
		if (pSfx)
		{
			((CSfxShoot*)pSfx)->m_dwSfxHit = dwSfxObjHit;
		}
	}

	return pSfx;

}
#endif
int CMover::GetSetItem(CItemElem * pItemElem)
{
	static	DWORD adwParts[4] = { PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP };
	if (!pItemElem)
	{
		pItemElem = GetEquipItem(PARTS_UPPER_BODY);
	}
	else
	{
		if (!IsSetItemPart(pItemElem->GetProp()->dwParts))
			return 0;
	}

	if (!pItemElem)
		return 0;

	if (pItemElem->IsFlag(CItemElem::expired))
		return 0;

	int nAbilityOption = pItemElem->GetAbilityOption();
	DWORD dwParts = pItemElem->GetProp()->dwParts;

	for (int i = 0; i < 4; i++)
	{
		if (dwParts != adwParts[i])
		{
			pItemElem = GetEquipItem(adwParts[i]);
			if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
			{
				if (nAbilityOption > pItemElem->GetAbilityOption())
					nAbilityOption = pItemElem->GetAbilityOption();
				continue;
			}
			return 0;
		}
	}
	return nAbilityOption;
}

int CMover::GetSetItemParts(DWORD dwParts)
{
	int nAbilityOption = 0;

	if (IsActiveMover())
	{
		CItemElem* pItemElem = GetEquipItem(dwParts);
		if (pItemElem)
			nAbilityOption = pItemElem->GetAbilityOption();
	}
	else
		nAbilityOption = (m_aEquipInfo[dwParts].nOption & 0xFF);

	return nAbilityOption;
}

int CMover::GetSetItemClient()
{
	static	DWORD adwParts[4] = { PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT, PARTS_CAP };

	int nAbilityOption = 10;

	for (int i = 0; i < 4; i++)
	{
		int nValue = GetSetItemParts(adwParts[i]);
		if (nValue == 0)
			return 0;
		if (nAbilityOption > nValue)
			nAbilityOption = nValue;
	}

	return nAbilityOption;
}

BOOL CMover::IsSetItemPart(DWORD dwParts)
{
	switch (dwParts)
	{
	case PARTS_UPPER_BODY:
	case PARTS_HAND:
	case PARTS_FOOT:
	case PARTS_CAP:
		return TRUE;
	}
	return FALSE;
}

void CMover::SetSetItemAvail(int nAbilityOption)
{
	PSETITEMAVAIL pSetItemAvail = prj.GetSetItemAvail(nAbilityOption);
	if (pSetItemAvail)
	{
		if (pSetItemAvail->nHitRate != 0)
			SetDestParam(DST_ADJ_HITRATE, pSetItemAvail->nHitRate, NULL_CHGPARAM);
		if (pSetItemAvail->nBlock != 0)
		{
			SetDestParam(DST_BLOCK_RANGE, pSetItemAvail->nBlock, NULL_CHGPARAM);
			SetDestParam(DST_BLOCK_MELEE, pSetItemAvail->nBlock, NULL_CHGPARAM);
		}
		if (pSetItemAvail->nMaxHitPointRate != 0)
			SetDestParam(DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, NULL_CHGPARAM);
		if (pSetItemAvail->nAddMagic != 0)
			SetDestParam(DST_ADDMAGIC, pSetItemAvail->nAddMagic, NULL_CHGPARAM);
		if (pSetItemAvail->nAdded != 0)
		{
			SetDestParam(DST_STR, pSetItemAvail->nAdded, NULL_CHGPARAM);
			SetDestParam(DST_DEX, pSetItemAvail->nAdded, NULL_CHGPARAM);
			SetDestParam(DST_INT, pSetItemAvail->nAdded, NULL_CHGPARAM);
			SetDestParam(DST_STA, pSetItemAvail->nAdded, NULL_CHGPARAM);
		}
	}
}

void CMover::ResetSetItemAvail(int nAbilityOption)
{
	PSETITEMAVAIL pSetItemAvail = prj.GetSetItemAvail(nAbilityOption);
	if (pSetItemAvail)
	{
		if (pSetItemAvail->nHitRate != 0)
			ResetDestParam(DST_ADJ_HITRATE, pSetItemAvail->nHitRate, TRUE);
		if (pSetItemAvail->nBlock != 0)
		{
			ResetDestParam(DST_BLOCK_RANGE, pSetItemAvail->nBlock, TRUE);
			ResetDestParam(DST_BLOCK_MELEE, pSetItemAvail->nBlock, TRUE);
		}
		if (pSetItemAvail->nMaxHitPointRate != 0)
			ResetDestParam(DST_HP_MAX_RATE, pSetItemAvail->nMaxHitPointRate, TRUE);
		if (pSetItemAvail->nAddMagic != 0)
			ResetDestParam(DST_ADDMAGIC, pSetItemAvail->nAddMagic, TRUE);
		if (pSetItemAvail->nAdded != 0)
		{
			ResetDestParam(DST_STR, pSetItemAvail->nAdded, TRUE);
			ResetDestParam(DST_DEX, pSetItemAvail->nAdded, TRUE);
			ResetDestParam(DST_INT, pSetItemAvail->nAdded, TRUE);
			ResetDestParam(DST_STA, pSetItemAvail->nAdded, TRUE);
		}
	}
}

void CMover::DestParamPiercingAvail(CItemElem * pItemElem, BOOL bSET)
{
	if (!pItemElem->IsPierceAble())
		return;

	PIERCINGAVAIL piercingAvail;
	memset(&piercingAvail, 0, sizeof(piercingAvail));

	pItemElem->GetPiercingAvail(&piercingAvail);

	for (int i = 0; i < piercingAvail.nSize; i++)
	{
		if (bSET)
			SetDestParam(piercingAvail.anDstParam[i], piercingAvail.anAdjParam[i], NULL_CHGPARAM);
		else
			ResetDestParam(piercingAvail.anDstParam[i], piercingAvail.anAdjParam[i], TRUE);
	}
}

BOOL CMover::Pierce(CItemElem * pSuit, DWORD dwItemId)
{
	for (int i = 0; i < pSuit->GetPiercingSize(); i++)
	{
		if (pSuit->GetPiercingItem(i) == 0)
		{

			UpdateItem((BYTE)(pSuit->m_dwObjId), UI_PIERCING, MAKELONG(i, dwItemId));
			return TRUE;
		}

	}
	return FALSE;
}

#ifdef __WORLDSERVER
void CMover::CheckTickCheer()
{
	if (m_nCheerPoint >= MAX_CHEERPOINT)
		return;

	DWORD dwTickCount = GetTickCount();
	if (dwTickCount > m_dwTickCheer)
		SetCheerParam(m_nCheerPoint + 1, dwTickCount, TICK_CHEERPOINT);
}
#endif	// __WORLDSERVER

void CMover::SetCheerParam(int nCheerPoint, DWORD dwTickCount, DWORD dwRest)
{
#ifdef __WORLDSERVER	// chipi_080411
	BOOL bAdd = FALSE;
	if (m_nCheerPoint < nCheerPoint)
		bAdd = TRUE;
#endif // __WORLDSERVER
	m_nCheerPoint = nCheerPoint;
	m_dwTickCheer = dwTickCount + dwRest;
#ifdef __WORLDSERVER
	((CUser*)this)->AddSetCheerParam(nCheerPoint, dwRest, /* chipi_080411 */ bAdd);
#endif	// __WORLDSERVER
}


BOOL CMover::NoDisguise(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifdef __CLIENT

	if (GetIndex() == MI_MALE || GetIndex() == MI_FEMALE)
		return FALSE;

	DWORD dwIndex = (GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	SetIndex(pd3dDevice, dwIndex, FALSE, FALSE);
#endif  //__CLIENT
	return TRUE;
}
BOOL CMover::Disguise(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex)
{
#ifdef __CLIENT
	return SetIndex(pd3dDevice, dwMoverIndex);
#endif // __CLIENT
	return TRUE;
}
BOOL CMover::SetIndex(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwMoverIndex, BOOL bInitProp, BOOL bDestParam)
{
	MoverProp* pMoverProp = prj.GetMoverProp(dwMoverIndex);
	if (pMoverProp == NULL)
	{
		Error("CMover.SetIndex: property not found: %d", dwMoverIndex);
		return FALSE;
	}

	if (m_pModel && m_pModel->IsAniable())
		SAFE_DELETE(m_pModel);
	SetTypeIndex(pd3dDevice, OT_MOVER, dwMoverIndex, bInitProp);
	m_dwMotion = -1;
	SetMotion(MTI_STAND);

	if (m_bPlayer)
	{
		if (dwMoverIndex == MI_MALE || dwMoverIndex == MI_FEMALE)
		{
#if defined( __CLIENT )
			RedoEquip(!IsActiveMover(), bDestParam);
#else
			RedoEquip(FALSE);
#endif
			SetHairColor(m_dwHairColor);
		}
	}
	ResetScale();
	UpdateLocalMatrix();
	return TRUE;
}
BOOL CMover::IsDisguise()
{
	if (IsPlayer())
	{
		return (GetIndex() != MI_MALE && GetIndex() != MI_FEMALE);
	}
	return FALSE;
}


LPCTSTR CMover::GetName(BOOL bNickname)
{
	if (!bNickname)
		return (LPCTSTR)m_szName;

	if (m_dwAuthorization >= AUTH_GAMEMASTER && (GetIndex() == MI_CROWNIBLIS || GetIndex() == MI_CROWNSHADE || GetIndex() == MI_CROWNBUBBLE))
	{
		MoverProp* pMoverProp = GetProp();
		if (pMoverProp)
			return (LPCTSTR)pMoverProp->szName;
	}
	return (LPCTSTR)m_szName;
}

void CMover::SetDestParamSetItem(CItemElem * pItemElem)
{
	ITEMAVAIL itemAvail;
	memset(&itemAvail, 0, sizeof(itemAvail));

	if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
	{
		CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
		if (pSetItem)
		{
			int nEquiped = GetEquipedSetItemNumber(pSetItem);
			pSetItem->GetItemAvail(&itemAvail, nEquiped, FALSE);
		}
	}
	else
	{
		map<CSetItem*, int>	mapSetItem;
		map<CSetItem*, int>::iterator i;
		for (int nParts = 0; nParts < MAX_HUMAN_PARTS; nParts++)
		{
			pItemElem = GetEquipItem(nParts);
			if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
			{
				CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
				if (pSetItem)
				{
					i = mapSetItem.find(pSetItem);
					if (i != mapSetItem.end())
						i->second++;
					else
						mapSetItem.insert(map<CSetItem*, int>::value_type(pSetItem, 1));
				}
			}
		}
		for (i = mapSetItem.begin(); i != mapSetItem.end(); ++i)
		{
			CSetItem* pSetItem = i->first;
			int nEquiped = i->second;
			pSetItem->GetItemAvail(&itemAvail, nEquiped, TRUE);
		}
	}

	for (int i = 0; i < itemAvail.nSize; i++)
		SetDestParam(itemAvail.anDstParam[i], itemAvail.anAdjParam[i], NULL_CHGPARAM);
}

void CMover::ResetDestParamSetItem(CItemElem * pItemElem)
{
	if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
	{
		CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
		if (pSetItem)
		{
			ITEMAVAIL itemAvail;
			memset(&itemAvail, 0, sizeof(itemAvail));
			int nEquiped = GetEquipedSetItemNumber(pSetItem);
			pSetItem->GetItemAvail(&itemAvail, nEquiped + 1, FALSE);
			for (int i = 0; i < itemAvail.nSize; i++)
				ResetDestParam(itemAvail.anDstParam[i], itemAvail.anAdjParam[i], TRUE);
		}
	}
}

int CMover::GetEquipedSetItemNumber(CSetItem * pSetItem)
{
	int nEquiped = 0;
	for (int i = 0; i < pSetItem->m_nElemSize; i++)
	{
		CItemElem* pItemElem = GetEquipItem(pSetItem->m_anParts[i]);
		if (pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] && !pItemElem->IsFlag(CItemElem::expired))
			nEquiped++;
	}
	return nEquiped;
}

void CMover::GetEquipedSetItem(int nSetItemId, BOOL * pbEquiped, int* pnEquip)
{
	*pnEquip = 0;
	CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItem(nSetItemId);
	if (pSetItem)
	{
		for (int i = 0; i < pSetItem->m_nElemSize; i++)
		{
			CItemElem* pItemElem = GetEquipItem(pSetItem->m_anParts[i]);
			if (pItemElem && pItemElem->m_dwItemId == pSetItem->m_adwItemId[i] && !pItemElem->IsFlag(CItemElem::expired))
			{
				pbEquiped[i] = TRUE;
				(*pnEquip)++;
			}
		}
	}
}

void CMover::SetDestParamRandomOptOrigin(CItemElem * pItemElem)
{
	if (pItemElem->GetRandomOpt() > 0)
	{
		PRANDOMOPTITEM pRandomOptItem = CRandomOptItemGen::GetInstance()->GetRandomOptItem(pItemElem->GetRandomOpt());
		if (pRandomOptItem)
		{
			for (int i = 0; i < pRandomOptItem->ia.nSize; i++)
				SetDestParam(pRandomOptItem->ia.anDstParam[i], pRandomOptItem->ia.anAdjParam[i], NULL_CHGPARAM);
		}
	}
}

void CMover::SetDestParamRandomOptExtension(CItemElem * pItemElem)
{
	int cbOption = g_xRandomOptionProperty->GetRandomOptionSize(pItemElem->GetRandomOptItemId());
	for (int i = 0; i < cbOption; i++)
	{
		int nDst, nAdj;
		if (g_xRandomOptionProperty->GetParam(pItemElem->GetRandomOptItemId(), i, &nDst, &nAdj))
			SetDestParam(nDst, nAdj, NULL_CHGPARAM);
	}
}

void CMover::SetDestParamRandomOpt(CItemElem * pItemElem)
{
	SetDestParamRandomOptOrigin(pItemElem);
	SetDestParamRandomOptExtension(pItemElem);
}

void CMover::ResetDestParamRandomOptOrigin(CItemElem * pItemElem)
{
	if (pItemElem->GetRandomOpt() > 0)
	{
		PRANDOMOPTITEM pRandomOptItem = CRandomOptItemGen::GetInstance()->GetRandomOptItem(pItemElem->GetRandomOpt());
		if (pRandomOptItem)
		{
			for (int i = 0; i < pRandomOptItem->ia.nSize; i++)
				ResetDestParam(pRandomOptItem->ia.anDstParam[i], pRandomOptItem->ia.anAdjParam[i], TRUE);
		}
	}
}

void CMover::ResetDestParamRandomOptExtension(CItemElem * pItemElem)
{
	int cbOption = g_xRandomOptionProperty->GetRandomOptionSize(pItemElem->GetRandomOptItemId());
	for (int i = 0; i < cbOption; i++)
	{
		int nDst, nAdj;
		if (g_xRandomOptionProperty->GetParam(pItemElem->GetRandomOptItemId(), i, &nDst, &nAdj))
			ResetDestParam(nDst, nAdj, TRUE);
	}
}

void CMover::ResetDestParamRandomOpt(CItemElem * pItemElem)
{
	ResetDestParamRandomOptOrigin(pItemElem);
	ResetDestParamRandomOptExtension(pItemElem);
}

#ifdef __CLIENT
void CMover::OverCoatItemRenderCheck(CModelObject * pModel)
{
	if (!pModel)
		return;

	int nArryEquip1[4] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT };
	int nArryEquip2[4] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS };

	int nTemp[4];

	for (int i = 0; i < 4; i++)
	{
		if (!IsOptionRenderCostume(i))
		{
			nTemp[i] = nArryEquip1[i];
			nArryEquip1[i] = nArryEquip2[i];
			nArryEquip2[i] = nTemp[i];
		}
	}

	if (IsActiveMover())
	{
		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		pItemElem = m_Inventory.GetEquip(PARTS_UPPER_BODY);

		if (IsOptionRenderCostume(1))
		{
			pItemElem = m_Inventory.GetEquip(PARTS_CLOTH);
		}

		if (pItemElem)
		{
			if (!pItemElem->IsFlag(CItemElem::expired))
			{
				if (pItemElem->GetLook() != NULL_ID)
					pItemProp = prj.GetItemProp(pItemElem->GetLook());
				else
					pItemProp = pItemElem->GetProp();
			}
		}

		O3D_ELEMENT* pElemLowerBody = pModel->GetParts(PARTS_LOWER_BODY);
		if (!IsOptionRenderCostume(1) && pItemElem == NULL)
		{
			if (pElemLowerBody == NULL)
				SetDefaultLowerBody(pModel);
			else
				pElemLowerBody->m_nEffect &= ~XE_HIDE;
		}
		else
		{
			if (pItemProp && pItemProp->dwExclusive != NULL_ID)
			{
				if (pElemLowerBody)
					pElemLowerBody->m_nEffect |= XE_HIDE;
			}
		}
	}
	else
	{
		O3D_ELEMENT* pElemLowerBody = pModel->GetParts(PARTS_LOWER_BODY);
		if (!IsOptionRenderCostume(1) && m_aEquipInfo[PARTS_UPPER_BODY].dwId == NULL_ID)
			SetDefaultLowerBody(pModel);
		else
		{
			if (m_bExclusiveLowBody)
			{
				if (pElemLowerBody)
					pElemLowerBody->m_nEffect |= XE_HIDE;
			}
		}
	}


	O3D_ELEMENT* pElem = NULL;
	O3D_ELEMENT* pElem2 = NULL;

	for (int j = 0; j < 4; j++)
	{
		pElem = pModel->GetParts(nArryEquip1[j]);
		pElem2 = pModel->GetParts(nArryEquip2[j]);

		if (pElem2)
			pElem2->m_nEffect &= ~XE_HIDE;

		if (!pElem)
			continue;

		pElem->m_nEffect &= ~XE_HIDE;

		if (pElem2)
		{
			if (IsActiveMover())
			{
				CItemElem* pItemElemOvercoat = GetEquipItem(nArryEquip2[j]);
				if (pItemElemOvercoat)
				{
					if (pItemElemOvercoat->IsFlag(CItemElem::expired))
					{
						pElem->m_nEffect &= ~XE_HIDE;
						pElem2->m_nEffect |= XE_HIDE;
						continue;
					}
				}
			}
			else
			{
				if ((m_aEquipInfo[nArryEquip2[j]].byFlag & CItemElem::expired))
				{
					pElem->m_nEffect &= ~XE_HIDE;
					pElem2->m_nEffect |= XE_HIDE;
					continue;
				}
			}

			pElem->m_nEffect |= XE_HIDE;
		}
		else
		{
			if (!IsOptionRenderCostume(0) && j == 0)
				pElem->m_nEffect |= XE_HIDE;
		}
	}


	pElem = pModel->GetParts(PARTS_CLOAK);
	if (pElem)
	{
		if (!IsOptionRenderCostume(4))
			pElem->m_nEffect |= XE_HIDE;
		else
			pElem->m_nEffect &= ~XE_HIDE;
	}

	pElem = pModel->GetParts(PARTS_MASK);
	if (pElem)
	{
		if (!IsOptionRenderCostume(5))
			pElem->m_nEffect |= XE_HIDE;
		else
			pElem->m_nEffect &= ~XE_HIDE;
	}

}
#endif	// __CLIENT

#ifdef __WORLDSERVER
void CMover::ProcInstantBanker()
{
	if (m_dwTickCreated && m_dwIndex == MI_INFO_PENG)
	{
		LPCHARACTER lpCharacter = GetCharacter();
		if (lpCharacter && lstrcmp(lpCharacter->m_szKey, "MaFl_InstantBank") == 0)
		{
			if (GetTickCount() > m_dwTickCreated + TIMEWAIT_INSTANT_BANKER)
				Delete();
		}
	}
}
#endif	// __WORLDSERVER

CItemElem* CMover::GetVendorItem()
{
	return NULL;
}

void CMover::AddSkillPoint(int nPoint)
{
	m_nSkillPoint += nPoint;
	m_nSkillLevel += nPoint;
}


BOOL CMover::IsAttackMode()
{
	return (m_nAtkCnt && m_nAtkCnt < (SEC1 * 10));
}

BOOL CMover::IsDropable(CItemElem * pItemElem, BOOL bPK)
{
	return (IsDropableState(bPK)
		&& IsUsableItem(pItemElem)
		&& !pItemElem->IsBinds()
		&& !m_Inventory.IsEquip(pItemElem->m_dwObjId)
		&& !IsUsing(pItemElem)
#if __VER >= 9	// __PET_0410
		&& pItemElem->GetProp()->dwItemKind3 != IK3_EGG
#endif	// __PET_0410
#ifdef __JEFF_11
		&& pItemElem->m_dwItemId != II_CHP_RED
#endif	// __JEFF_11

		);
}

#ifdef __CLIENT
BOOL CMover::IsStateDbuff()
{
	int nAdjParam = GetAdjParam(DST_CHRSTATE);
	if (nAdjParam & CHS_STUN || nAdjParam & CHS_POISON || nAdjParam & CHS_GROGGY
		|| nAdjParam & CHS_SILENT || nAdjParam & CHS_BLEEDING
		)
	{
		return TRUE;
	}

	return FALSE;
}
#endif


int __IsEndQuestCondition(CMover * pMover, int nQuestId)
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(nQuestId);

	if (pQuestProp)
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->FindQuest(nQuestId);
		if (pCurQuest == NULL || pCurQuest->m_nState == QS_END)
			return 0;

		if (pQuestProp->m_nEndCondLimitTime == 0 || pCurQuest->m_wTime)
			nResult++;

		if (pQuestProp->m_dwEndCondPatrolWorld == 0 || pCurQuest->m_bPatrol)
			nResult++;

		for (int i = 0; i < 2; i++)
		{
			if (pQuestProp->m_nEndCondKillNPCIdx[i] == 0 || pCurQuest->m_nKillNPCNum[i] >= pQuestProp->m_nEndCondKillNPCNum[i])
				nResult++;
		}

		if (pQuestProp->m_nEndCondSkillIdx == 0)
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill(pQuestProp->m_nEndCondSkillIdx);
			if (lpSkill)
			{
				if (pMover->CheckSkill(pQuestProp->m_nEndCondSkillIdx) && lpSkill->dwLevel >= (DWORD)(pQuestProp->m_nEndCondSkillLvl))
					nResult++;
			}
		}

		if (pQuestProp->m_nEndCondGold == 0)
			nResult++;
		else
		{
			if (pMover->CheckUserGold(pQuestProp->m_nEndCondGold, false) == true)
				nResult++;
		}

		if (pQuestProp->m_nEndCondLevelMin == 0 || (pMover->GetLevel() >= pQuestProp->m_nEndCondLevelMin && pMover->GetLevel() <= pQuestProp->m_nEndCondLevelMax))
			nResult++;
		if (pQuestProp->m_nEndCondExpPercentMin == 0 || (pMover->GetExpPercent() >= pQuestProp->m_nEndCondExpPercentMin && pMover->GetExpPercent() <= pQuestProp->m_nEndCondExpPercentMax))
			nResult++;

		if (pQuestProp->m_nEndCondPetExp == 0)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetExpPercent() >= pQuestProp->m_nEndCondPetExp)
				nResult++;
		}
		if (pQuestProp->m_nEndCondPetLevel == -1)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetLevel() == pQuestProp->m_nEndCondPetLevel)
				nResult++;
		}

		if (pQuestProp->m_nEndCondDisguiseMoverIndex == 0)
			nResult++;
		else
		{
			if (pQuestProp->m_nEndCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE)
				nResult++;
			else if (pQuestProp->m_nEndCondDisguiseMoverIndex == pMover->GetIndex())
				nResult++;
		}

		if (pQuestProp->m_nEndCondState == 0)
		{
			int i = QS_BEGIN + 1;
			for (; i < QS_END; i++)
			{
				if (pQuestProp->m_questState[i] == NULL)
					break;
			}
			if ((i - 1) == pCurQuest->m_nState)
				nResult++;
		}
		else
			if (pQuestProp->m_nEndCondState == pCurQuest->m_nState)
				nResult++;

		if (pQuestProp->m_nEndCondCompleteQuestOper == 0)
			nResult += 6;
		else if (pQuestProp->m_nEndCondCompleteQuestOper == 1)
		{
			int i = NULL;
			for (; i < 6; i++)
			{
				if (pMover->IsCompleteQuest(pQuestProp->m_nEndCondCompleteQuest[i]))
					break;
			}
			if (i != 6)
				nResult += 6;
		}
		else if (pQuestProp->m_nEndCondCompleteQuestOper == 2)
		{
			for (int i = 0; i < 6; i++)
			{
				if (pQuestProp->m_nEndCondCompleteQuest[i] == 0 || pMover->IsCompleteQuest(pQuestProp->m_nEndCondCompleteQuest[i]))
					nResult++;
			}
		}

		if (pQuestProp->m_nEndCondParty == 0)
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty(pMover->m_idparty);
			if (pParty)
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();
				bLeader = pParty->m_aMember[0].m_uPlayerId == pMover->m_idPlayer;
			}
#else
			bParty = g_Party.IsMember(g_pPlayer->m_idPlayer);
			nSize = g_Party.GetSizeofMember();
			bLeader = (g_Party.m_aMember[0].m_uPlayerId == pMover->m_idPlayer);
#endif
			if (pQuestProp->m_nEndCondParty == 1 && bParty == FALSE)
				nResult++;
			else if (pQuestProp->m_nEndCondParty == 2 && bParty == TRUE)
			{
				if (pQuestProp->m_nEndCondPartyLeader == 0 || (pQuestProp->m_nEndCondPartyLeader - 1) == bLeader)
				{
					if (pQuestProp->m_nEndCondPartyNum == 0)
						nResult++;
					else if (pQuestProp->m_nEndCondPartyNumComp == 0)
					{
						if (nSize == pQuestProp->m_nEndCondPartyNum)
							nResult++;
					}
					else if (pQuestProp->m_nEndCondPartyNumComp == 1)
					{
						if (nSize >= pQuestProp->m_nEndCondPartyNum)
							nResult++;
					}
					else if (pQuestProp->m_nEndCondPartyNumComp == -1)
					{
						if (nSize <= pQuestProp->m_nEndCondPartyNum)
							nResult++;
					}
				}
			}
		}

		if (pQuestProp->m_nEndCondGuild == 0)
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			CGuild* pGuild = g_GuildMng.GetGuild(pMover->m_idPlayer);
			if (pGuild)
			{
				bGuild = TRUE;
				nSize = pGuild->GetSize();
				bLeader = (pGuild->m_idMaster == pMover->m_idPlayer);
			}
			if (pQuestProp->m_nEndCondGuild == 1 && bGuild == FALSE)
				nResult++;
			else if (pQuestProp->m_nEndCondGuild == 2 && bGuild == TRUE)
			{
				if (pQuestProp->m_nEndCondGuildLeader == 0 || (pQuestProp->m_nEndCondGuildLeader - 1) == bLeader)
				{
					if (pQuestProp->m_nEndCondGuildNum == 0)
						nResult++;
					else if (pQuestProp->m_nEndCondGuildNumComp == 0)
					{
						if (nSize == pQuestProp->m_nEndCondGuildNum)
							nResult++;
					}
					else if (pQuestProp->m_nEndCondGuildNumComp == 1)
					{
						if (nSize >= pQuestProp->m_nEndCondGuildNum)
							nResult++;
					}
					else if (pQuestProp->m_nEndCondGuildNumComp == -1)
					{
						if (nSize <= pQuestProp->m_nEndCondGuildNum)
							nResult++;
					}
				}
			}
		}

		if (pQuestProp->m_nEndCondItemNum == 0)
			nResult += MAX_QUESTCONDITEM;
		else
		{
			for (int i = 0; i < MAX_QUESTCONDITEM; i++)
			{
				if (i < pQuestProp->m_nEndCondItemNum)
				{
					QuestPropItem* pEndCondItem = &pQuestProp->m_paEndCondItem[i];
					if (pEndCondItem->m_nSex == -1 || pEndCondItem->m_nSex == pMover->GetSex())
					{
						if (pEndCondItem->m_nType == 0)
						{
							if (pEndCondItem->m_nJobOrItem == -1 || pEndCondItem->m_nJobOrItem == pMover->GetJob())
							{
								if (pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pEndCondItem->m_nItemIdx) >= pEndCondItem->m_nItemNum)
									nResult++;
							}
							else
								nResult++;
						}
						else if (pEndCondItem->m_nType == 1)
						{
							if (pEndCondItem->m_nJobOrItem == -1 || pMover->GetItemNum(pEndCondItem->m_nJobOrItem))
							{
								if (pEndCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pEndCondItem->m_nItemIdx) >= pEndCondItem->m_nItemNum)
									nResult++;
							}
							else
								nResult++;
						}
					}
				}
				else
					nResult++;
			}
		}

		if (nResult == 20 + MAX_QUESTCONDITEM)
		{
			if (pCurQuest->m_wTime)
				pCurQuest->m_wTime |= 0x8000;

			return 1;
		}
	}

	return 0;
}

int __IsBeginQuestCondition(CMover * pMover, int nQuestId)
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(nQuestId);
	if (pQuestProp)
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->FindQuest(nQuestId);
		BOOL bComplete = pMover->IsCompleteQuest(nQuestId);

		if (pCurQuest || bComplete)
			return 0;

#ifdef __WORLDSERVER

		int nItemCount = 0;
		for (int j = 0; j < 4; j++)
			if (pQuestProp->m_nBeginSetAddItemIdx[j])
				nItemCount++;

		if (IsValidObj(pMover) && static_cast<CUser*>(pMover)->m_Inventory.GetEmptyCount() < nItemCount)
		{
			static_cast<CUser*>(pMover)->AddDefinedText(TID_GAME_LACKSPACE);
			return 0;
		}

#endif // __WORLDSERVER

		for (int i = 0; i < 6; i++)
		{
			if (pQuestProp->m_anBeginCondPreviousQuest[i] == 0)
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->FindQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				BOOL bPreComplete = pMover->IsCompleteQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				if (pQuestProp->m_nBeginCondPreviousQuestType == 0)
				{
					if (pPreQuest || bPreComplete)
						nResult++;
				}
				else
					if (pQuestProp->m_nBeginCondPreviousQuestType == 1)
					{
						if (pPreQuest == NULL && bPreComplete)
							nResult++;
					}
					else
						if (pQuestProp->m_nBeginCondPreviousQuestType == 2)
						{
							if (pPreQuest && bPreComplete == FALSE)
								nResult++;
						}
			}
		}

		for (int i = 0; i < 6; i++)
		{
			if (pQuestProp->m_anBeginCondExclusiveQuest[i] == 0)
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->FindQuest(pQuestProp->m_anBeginCondExclusiveQuest[i]);
				BOOL bPreComplete = pMover->IsCompleteQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				if (pPreQuest == NULL && bPreComplete == FALSE)
					nResult++;
			}
		}

		if (pQuestProp->m_nBeginCondJobNum == 0)
			nResult++;
		else
			for (int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++)
			{
				if (pQuestProp->m_nBeginCondJob[i] == pMover->GetJob())
				{
					nResult++;
					break;
				}
			}

		if (pQuestProp->m_nBeginCondLevelMin == 0 || (pMover->GetLevel() >= pQuestProp->m_nBeginCondLevelMin && pMover->GetLevel() <= pQuestProp->m_nBeginCondLevelMax))
			nResult++;

		if (pQuestProp->m_nBeginCondParty == 0)
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty(pMover->m_idparty);
			if (pParty)
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();

			}
#else
			bParty = g_Party.IsMember(g_pPlayer->m_idPlayer);
			nSize = g_Party.GetSizeofMember();
#endif
			if (pQuestProp->m_nBeginCondParty == 1 && bParty == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondParty == 2 && bParty == TRUE)
				{
					if (pQuestProp->m_nBeginCondPartyLeader == -1 || pQuestProp->m_nBeginCondPartyLeader == bLeader)
					{
						if (pQuestProp->m_nBeginCondPartyNum == 0)
							nResult++;
						else
							if (pQuestProp->m_nBeginCondPartyNumComp == 0)
							{
								if (nSize == pQuestProp->m_nBeginCondPartyNum)
									nResult++;
							}
							else
								if (pQuestProp->m_nBeginCondPartyNumComp == 1)
								{
									if (nSize >= pQuestProp->m_nBeginCondPartyNum)
										nResult++;
								}
								else
									if (pQuestProp->m_nBeginCondPartyNumComp == -1)
									{
										if (nSize <= pQuestProp->m_nBeginCondPartyNum)
											nResult++;
									}
					}
				}
		}

		if (pQuestProp->m_nBeginCondGuild == 0)
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
			if (pQuestProp->m_nBeginCondGuild == 1 && bGuild == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondGuild == 2 && bGuild == TRUE)
				{
					if (pQuestProp->m_nBeginCondGuildLeader == -1 || pQuestProp->m_nBeginCondGuildLeader == bLeader)
					{
						if (pQuestProp->m_nBeginCondGuildNum == 0)
							nResult++;
						else
							if (pQuestProp->m_nBeginCondGuildNumComp == 0)
							{
								if (nSize == pQuestProp->m_nBeginCondGuildNum)
									nResult++;
							}
							else
								if (pQuestProp->m_nBeginCondGuildNumComp == 1)
								{
									if (nSize >= pQuestProp->m_nBeginCondGuildNum)
										nResult++;
								}
								else
									if (pQuestProp->m_nBeginCondGuildNumComp == -1)
									{
										if (nSize <= pQuestProp->m_nBeginCondGuildNum)
											nResult++;
									}
					}
				}
		}

		if (pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pMover->GetSex())
			nResult++;

		if (pQuestProp->m_nBeginCondSkillIdx == 0)
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill(pQuestProp->m_nBeginCondSkillIdx);
			if (lpSkill)
			{
				if (pMover->CheckSkill(pQuestProp->m_nBeginCondSkillLvl) && lpSkill->dwLevel >= (DWORD)(pQuestProp->m_nBeginCondSkillLvl))
					nResult++;
			}
		}

		if (pQuestProp->m_nBeginCondDisguiseMoverIndex == 0)
			nResult++;
		else
		{
			if (pQuestProp->m_nBeginCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondDisguiseMoverIndex == pMover->GetIndex())
					nResult++;
		}


		if (pQuestProp->m_nBeginCondPetExp == 0)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetExpPercent() >= pQuestProp->m_nBeginCondPetExp)
				nResult++;
		}
		if (pQuestProp->m_nBeginCondPetLevel == -1)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetLevel() == pQuestProp->m_nBeginCondPetLevel)
				nResult++;
		}

		if (pQuestProp->m_nBeginCondItemNum == 0)
			nResult += MAX_QUESTCONDITEM;
		else
			for (int i = 0; i < MAX_QUESTCONDITEM; i++)
			{
				if (i < pQuestProp->m_nBeginCondItemNum)
				{
					QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[i];
					if (pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex())
					{
						if (pBeginCondItem->m_nType == 0)
						{
							if (pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob())
							{
								if (pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pBeginCondItem->m_nItemIdx) >= pBeginCondItem->m_nItemNum)
									nResult++;
							}
							else
								nResult++;
						}
						else
							if (pBeginCondItem->m_nType == 1)
							{
								if (pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum(pBeginCondItem->m_nJobOrItem))
								{
									if (pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pBeginCondItem->m_nItemIdx) >= pBeginCondItem->m_nItemNum)
										nResult++;
								}
								else
									nResult++;
							}
					}
				}
				else
					nResult++;
			}

		if (nResult == 21 + MAX_QUESTCONDITEM)
			return 1;
	}

	return 0;
}

int __IsNextLevelQuest(CMover * pMover, int nQuestId)
{
	QuestProp* pQuestProp = prj.m_aPropQuest.GetAt(nQuestId);
	if (pQuestProp)
	{
		int nResult = 0;
		LPQUEST pCurQuest = pMover->FindQuest(nQuestId);
		BOOL bComplete = pMover->IsCompleteQuest(nQuestId);

		if (pCurQuest || bComplete)
			return 0;

		for (int i = 0; i < 6; i++)
		{
			if (pQuestProp->m_anBeginCondPreviousQuest[i] == 0)
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->FindQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				BOOL bPreComplete = pMover->IsCompleteQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				if (pQuestProp->m_nBeginCondPreviousQuestType == 0)
				{
					if (pPreQuest || bPreComplete)
						nResult++;
				}
				else
					if (pQuestProp->m_nBeginCondPreviousQuestType == 1)
					{
						if (pPreQuest == NULL && bPreComplete)
							nResult++;
					}
					else
						if (pQuestProp->m_nBeginCondPreviousQuestType == 2)
						{
							if (pPreQuest && bPreComplete == FALSE)
								nResult++;
						}
			}
		}

		for (int i = 0; i < 6; i++)
		{
			if (pQuestProp->m_anBeginCondExclusiveQuest[i] == 0)
				nResult++;
			else
			{
				LPQUEST pPreQuest = pMover->FindQuest(pQuestProp->m_anBeginCondExclusiveQuest[i]);
				BOOL bPreComplete = pMover->IsCompleteQuest(pQuestProp->m_anBeginCondPreviousQuest[i]);
				if (pPreQuest == NULL && bPreComplete == FALSE)
					nResult++;
			}
		}

		if (pQuestProp->m_nBeginCondJobNum == 0)
			nResult++;
		else
			for (int i = 0; i < pQuestProp->m_nBeginCondJobNum; i++)
			{
				if (pQuestProp->m_nBeginCondJob[i] == pMover->GetJob())
				{
					nResult++;
					break;
				}
			}

		if (pMover->GetLevel() < pQuestProp->m_nBeginCondLevelMin && pMover->GetLevel() + 5 >= pQuestProp->m_nBeginCondLevelMin)
			nResult++;

		if (pQuestProp->m_nBeginCondParty == 0)
			nResult++;
		else
		{
			BOOL bParty = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;
#ifdef __WORLDSERVER
			CParty* pParty = g_PartyMng.GetParty(pMover->m_idparty);
			if (pParty)
			{
				bParty = TRUE;
				nSize = pParty->GetSizeofMember();

			}
#else
			bParty = g_Party.IsMember(g_pPlayer->m_idPlayer);
			nSize = g_Party.GetSizeofMember();
#endif
			if (pQuestProp->m_nBeginCondParty == 1 && bParty == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondParty == 2 && bParty == TRUE)
				{
					if (pQuestProp->m_nBeginCondPartyLeader == -1 || pQuestProp->m_nBeginCondPartyLeader == bLeader)
					{
						if (pQuestProp->m_nBeginCondPartyNum == 0)
							nResult++;
						else
							if (pQuestProp->m_nBeginCondPartyNumComp == 0)
							{
								if (nSize == pQuestProp->m_nBeginCondPartyNum)
									nResult++;
							}
							else
								if (pQuestProp->m_nBeginCondPartyNumComp == 1)
								{
									if (nSize >= pQuestProp->m_nBeginCondPartyNum)
										nResult++;
								}
								else
									if (pQuestProp->m_nBeginCondPartyNumComp == -1)
									{
										if (nSize <= pQuestProp->m_nBeginCondPartyNum)
											nResult++;
									}
					}
				}
		}

		if (pQuestProp->m_nBeginCondGuild == 0)
			nResult++;
		else
		{
			BOOL bGuild = FALSE;
			BOOL bLeader = FALSE;
			int nSize = 0;

			if (pQuestProp->m_nBeginCondGuild == 1 && bGuild == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondGuild == 2 && bGuild == TRUE)
				{
					if (pQuestProp->m_nBeginCondGuildLeader == -1 || pQuestProp->m_nBeginCondGuildLeader == bLeader)
					{
						if (pQuestProp->m_nBeginCondGuildNum == 0)
							nResult++;
						else if (pQuestProp->m_nBeginCondGuildNumComp == 0)
						{
							if (nSize == pQuestProp->m_nBeginCondGuildNum)
								nResult++;
						}
						else if (pQuestProp->m_nBeginCondGuildNumComp == 1)
						{
							if (nSize >= pQuestProp->m_nBeginCondGuildNum)
								nResult++;
						}
						else if (pQuestProp->m_nBeginCondGuildNumComp == -1)
						{
							if (nSize <= pQuestProp->m_nBeginCondGuildNum)
								nResult++;
						}
					}
				}
		}

		if (pQuestProp->m_nBeginCondSex == -1 || pQuestProp->m_nBeginCondSex == pMover->GetSex())
			nResult++;

		if (pQuestProp->m_nBeginCondSkillIdx == 0)
			nResult++;
		else
		{
			LPSKILL lpSkill = pMover->GetSkill(pQuestProp->m_nBeginCondSkillIdx);
			if (lpSkill)
			{
				if (pMover->CheckSkill(pQuestProp->m_nBeginCondSkillLvl) && (int)(lpSkill->dwLevel) >= pQuestProp->m_nBeginCondSkillLvl)
					nResult++;
			}
		}


		if (pQuestProp->m_nBeginCondDisguiseMoverIndex == 0)
			nResult++;
		else
		{
			if (pQuestProp->m_nBeginCondDisguiseMoverIndex == -1 && pMover->IsDisguise() == FALSE)
				nResult++;
			else
				if (pQuestProp->m_nBeginCondDisguiseMoverIndex == pMover->GetIndex())
					nResult++;
		}


		if (pQuestProp->m_nBeginCondPetExp == 0)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetExpPercent() >= pQuestProp->m_nBeginCondPetExp)
				nResult++;
		}
		if (pQuestProp->m_nBeginCondPetLevel == -1)
			nResult++;
		else
		{
			CPet* pPet = pMover->GetPet();
			if (pPet && pPet->GetLevel() == pQuestProp->m_nBeginCondPetLevel)
				nResult++;
		}

		if (pQuestProp->m_nBeginCondItemNum == 0)
			nResult += MAX_QUESTCONDITEM;
		else
		{
			for (int i = 0; i < MAX_QUESTCONDITEM; i++)
			{
				if (i < pQuestProp->m_nBeginCondItemNum)
				{
					QuestPropItem* pBeginCondItem = &pQuestProp->m_paBeginCondItem[i];
					if (pBeginCondItem->m_nSex == -1 || pBeginCondItem->m_nSex == pMover->GetSex())
					{
						if (pBeginCondItem->m_nType == 0)
						{
							if (pBeginCondItem->m_nJobOrItem == -1 || pBeginCondItem->m_nJobOrItem == pMover->GetJob())
							{
								if (pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pBeginCondItem->m_nItemIdx) >= pBeginCondItem->m_nItemNum)
									nResult++;
							}
							else
								nResult++;
						}
						else if (pBeginCondItem->m_nType == 1)
						{
							if (pBeginCondItem->m_nJobOrItem == -1 || pMover->GetItemNum(pBeginCondItem->m_nJobOrItem))
							{
								if (pBeginCondItem->m_nItemIdx == 0 || pMover->GetItemNum(pBeginCondItem->m_nItemIdx) >= pBeginCondItem->m_nItemNum)
									nResult++;
							}
							else
								nResult++;
						}
					}
				}
				else
					nResult++;
			}
		}

		if (nResult == 21 + MAX_QUESTCONDITEM)
			return 1;
	}

	return 0;
}


#ifdef __WORLDSERVER
void CMover::ProcessSFXDamage(void)
{
	ProcessSFXExpire();

	map<OBJID, queue<SFXHIT_INFO> >::iterator iInfo = m_mapSFXInfo.begin();
	while (iInfo != m_mapSFXInfo.end())
	{
		map<OBJID, queue<SFXHIT_INFO> >::iterator iInfo2 = iInfo;
		++iInfo;
		OBJID idTarget = iInfo2->first;
		queue<SFXHIT_INFO>& qInfo = iInfo2->second;
		map<OBJID, queue<SFXHIT_COUNT> >::iterator iCount = m_mapSFXCount.find(idTarget);
		if (iCount == m_mapSFXCount.end())
			continue;
		queue<SFXHIT_COUNT>& qCount = iCount->second;
		// qInfo, qCount
		while (qInfo.size() > 0 && qCount.size() > 0)
		{
			SFXHIT_INFO& si = qInfo.front();
			SFXHIT_COUNT& sc = qCount.front();
			if (si.dwSkill == sc.dwSkill)
			{
				if (AttackBySFX(this, si) == FALSE)
				{
					m_mapSFXInfo.erase(idTarget);
					m_mapSFXCount.erase(idTarget);
					break;
				}
				qInfo.pop();
				qCount.pop();
			}
			else
			{
				TRACE("���� ó��\n");
				if (si.dwTickCount > sc.dwTickCount)
				{
					while (qCount.size() > 0)
					{
						SFXHIT_COUNT& sc1 = qCount.front();
						if (si.dwSkill == sc1.dwSkill)
							break;
						qCount.pop();
					}
				}
				else if (si.dwTickCount < sc.dwTickCount)
				{
					while (qInfo.size() > 0)
					{
						SFXHIT_INFO& si1 = qInfo.front();
						if (si1.dwSkill == sc.dwSkill)
							break;
						qInfo.pop();
					}
				}
				else
				{
					qInfo.pop();
					qCount.pop();
				}
			}
		}
	}

}

void CMover::ProcessSFXExpire(void)
{
	DWORD dwTickCount = GetTickCount();

	map<OBJID, queue<SFXHIT_INFO> >::iterator iInfo = m_mapSFXInfo.begin();
	while (iInfo != m_mapSFXInfo.end())
	{
		map<OBJID, queue<SFXHIT_INFO> >::iterator i2 = iInfo;
		++iInfo;
		OBJID idTarget = i2->first;
		queue<SFXHIT_INFO>& q = i2->second;

		while (q.size() > 0)
		{
			SFXHIT_INFO& si = q.front();
			if (si.dwTickCount + SEC(10) < dwTickCount)
			{
				q.pop();
				continue;
			}
			else
				break;
		}
		if (q.size() == 0)
		{
			m_mapSFXInfo.erase(idTarget);
		}
	}
	// 1.2
	map<OBJID, queue<SFXHIT_COUNT> >::iterator iCount = m_mapSFXCount.begin();
	while (iCount != m_mapSFXCount.end())
	{
		map<OBJID, queue<SFXHIT_COUNT> >::iterator i2 = iCount;
		++iCount;
		OBJID idTarget = i2->first;
		queue<SFXHIT_COUNT>& q = i2->second;

		while (q.size() > 0)
		{
			SFXHIT_COUNT& sc = q.front();
			if (sc.dwTickCount + SEC(10) < dwTickCount)
			{
				q.pop();
				continue;
			}
			else
				break;
		}
		if (q.size() == 0)
		{
			m_mapSFXCount.erase(idTarget);
		}
	}
}
#endif	// __WORLDSERVER

#ifdef __CLIENT
void CMover::WingMotionSetting(const CModelObject * pModel)
{
	if (m_pRide == NULL && m_pRide->m_pBone == NULL)
		return;

	if ((m_pActMover->GetState() & OBJSTA_STAND) && (D3DXVec3LengthSq(&m_pActMover->m_vDelta) <= 0.000f) && pModel->m_fFrameCurrent == 0.0f)
	{
		if (m_eWingStateFlag != FLOATING)
		{
			CString strMotion = m_pRide->GetMotionFileName(_T("stand"));
			assert(strMotion != _T(""));
			m_pRide->LoadMotion(strMotion);
			m_pRide->SetMotionBlending(TRUE);
			m_pRide->SetLoop(ANILOOP_LOOP);
			m_eWingStateFlag = FLOATING;
		}
	}
	else if (m_pActMover->GetStateFlag() & OBJSTAF_ACC)
	{
		if (m_eWingStateFlag != FLYING)
		{
			CString strMotion = m_pRide->GetMotionFileName(_T("walk"));
			assert(strMotion != _T(""));
			m_pRide->LoadMotion(strMotion);
			m_pRide->SetMotionBlending(TRUE);
			m_pRide->SetLoop(ANILOOP_LOOP);
			m_eWingStateFlag = FLYING;
		}
	}
	else if ((m_pActMover->GetState() & OBJSTA_LTURN) || (m_pActMover->GetState() & OBJSTA_RTURN))
	{
		if (m_eWingStateFlag != TURNING)
			m_eWingStateFlag = TURNING;
	}
}

float CMover::GetRideFrameSpeed(void)
{
	float fFrameSpeed = 1.0f;
	if (m_pRide)
	{
		float fNowLengthSq = D3DXVec3LengthSq(&m_pActMover->m_vDelta);
		fFrameSpeed = 1.0f + (fabs(fNowLengthSq - m_fOldLengthSq) * 7000.0f);
		m_fOldLengthSq = fNowLengthSq;

		if ((m_pActMover->GetStateFlag() & OBJSTAF_ACC) && (m_pActMover->GetStateFlag() & OBJSTAF_TURBO))
			fFrameSpeed += 1.2f;

		if (fFrameSpeed < 1.0f)
			fFrameSpeed = 1.0f;

		if (fFrameSpeed > 4.0f)
			fFrameSpeed = 4.0f;
	}
	return fFrameSpeed;
}
#endif //__CLIENT

#ifdef __CLIENT
void CMover::CreateAngelParticle(D3DXVECTOR3 vPos)
{
	FLOAT fAngXZ, fAngH, fDist, fSpeed;
	D3DXVECTOR3 vVel;

	fAngXZ = GetAngle();
	fAngXZ += 180.0f;
	fAngXZ = D3DXToRadian(fAngXZ);

	vPos.y += 1.1f;

	for (int j = 0; j < 2; j++)
	{
		vPos.y -= 0.03f;

		{
			fAngXZ = (float)(-45.0f + xRandomF(90));
			fAngXZ += GetAngle();
			fAngXZ += 180.0f;

			fAngXZ = D3DXToRadian(fAngXZ);
			fAngH = (float)(GetAngleX() + xRandomF(-30) + xRandomF(30));
			fAngH = D3DXToRadian(fAngH);
			fSpeed = xRandomF(0.02f);

			fDist = cosf(-fAngH) * fSpeed;
			vVel.x = sinf(fAngXZ) * fDist;
			vVel.z = -cosf(fAngXZ) * fDist;
			vVel.y = -sinf(-fAngH) * fSpeed;

			g_ParticleMng.CreateParticle(m_dwAngelKind + xRandom(3), vPos, vVel, GetPos().y + 0.5f);
		}
	}
}
#endif //__CLIENT

#if __VER >= 9	// __PET_0410
void CMover::ProcessPet(void)
{
#ifdef __CLIENT

#ifdef __AIO_PETS
	if (IsActiveMover() && HasActivatedVisPet())
	{
		CMover* pMyBuffPet = prj.GetMover(GetEatPetId());
		if (pMyBuffPet)
		{
			lstrcpy(pMyBuffPet->m_szCharacterKey, "MaFl_BuffPet");

			if (g_WndMng.m_pWndBuffPetStatus)
				g_WndMng.m_pWndBuffPetStatus->SetPet(pMyBuffPet);

		}
	}
#endif

	if (HasActivatedSystemPet())
	{
		if (m_pet.GetObj() == NULL)
		{
			DWORD dwIndex = 0;

			if (IsActiveMover())
			{
				CItemElem* pItemElem = GetPetItem();
				if (pItemElem && pItemElem->m_pPet)
					dwIndex = pItemElem->m_pPet->GetIndex();
			}
			else
				dwIndex = (DWORD)LOWORD(GetPetId());

			if (dwIndex > 0)
			{
				CMover* pPet = (CMover*)CreateObj(D3DDEVICE, OT_MOVER, dwIndex);
#ifdef __PET_1024
				if (m_pet.HasName())
					pPet->SetName(m_pet.GetName());
#endif	// __PET_1024
				pPet->InitMotion(MTI_STAND);
				pPet->UpdateLocalMatrix();
				pPet->m_pAIInterface = new CAIEgg(pPet, GetId());
				pPet->m_dwAIInterface = AII_EGG;
				pPet->m_dwAIInterfaceState = 2;
				pPet->SetPos(GetPos());
				GetWorld()->AddObj(pPet, FALSE);
				m_pet.SetObj(pPet);

				pPet->AutoSynchronizer()->Initialize();

				if (IsActiveMover())
					lstrcpy(pPet->m_szCharacterKey, "MaFl_Pet");
				CreatePetSfx();
				m_pet.SetLevelup(0);

#ifndef __AIO_PETS
				if (FALSE == g_WndMng.GetWndBase(APP_PET_STATUS) && this->IsActiveMover())
#else
				if (/*!g_WndMng.GetWndBase(APP_PET_STATUS) && */this->IsActiveMover() && !g_Option.m_bHideWindowPet) // __FL_SWITCH_EQ
#endif
				{
					SAFE_DELETE(g_WndMng.m_pWndPetStatus);
					g_WndMng.m_pWndPetStatus = new CWndPetStatus;
					g_WndMng.m_pWndPetStatus->Initialize();
				}
			}
		}
	}
	else
	{
		if (m_pet.GetObj())
		{
			m_pet.GetObj()->Delete();
			m_pet.SetObj(NULL);

			if (g_WndMng.GetWndBase(APP_PET_STATUS) && this->IsActiveMover())
				g_WndMng.m_pWndPetStatus->Destroy();
		}
	}

#else	// __CLIENT
	ProcessPetAvail();
	ProcessPetEnergy();
	ProcessPetExp();
#if __VER >= 15 // __PETVIS
	ProcessVisPet();
#endif // __PETVIS
#endif	// __CLIENT
}

CItemElem* CMover::GetPetItem(void)
{
	if (HasActivatedSystemPet())
	{
		CItemElem* pItemElem = static_cast<CItemElem*>(m_Inventory.GetAtId(m_dwPetId));
		if (pItemElem && pItemElem->IsPet())
			return pItemElem;
	}
	return NULL;
}

CPet* CMover::GetPet(void)
{
	CItemElem* pItemElem = GetPetItem();
	if (pItemElem)
		return pItemElem->m_pPet;
	return NULL;
}

void CMover::PetLevelup(void)
{
#ifdef __CLIENT
	if (m_pet.GetObj())
	{
		m_pet.SetLevelup(PF_PET_LEVEL_UP);
		m_pet.GetObj()->Delete();
		m_pet.SetObj(NULL);
	}
#else	// __CLIENT
	CItemElem* pItemElem = GetPetItem();
	CPet* pPet = pItemElem->m_pPet;

	if (pPet->GetLevel() == PL_EGG)
	{
		pPet->SetKind(CPetProperty::GetInstance()->Hatch());
		pPet->SetLife(1);
#if __VER >= 13
		((CUser*)this)->SetHonorAdd(HS_HATCHING_EGG, HI_COUNT_CHECK);
#endif
	}

	pPet->IncLevel();
	pItemElem->m_dwItemId = pPet->GetItemId();
	pPet->SetExp(0);
	pPet->SetEnergy(pPet->GetMaxEnergy());

	BYTE nAvailLevel = CPetProperty::GetInstance()->GetLevelupAvailLevel(pPet->GetLevel());
	pPet->SetAvailLevel(pPet->GetLevel(), nAvailLevel);
	if (HasPet())
		RemovePet();

	//	g_dpDBClient.CalluspPetLog(m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_LEVELUP, pPet);

	((CUser*)this)->AddPet(pPet, PF_PET_LEVEL_UP);
	g_UserMng.AddPetLevelup(this, MAKELONG((WORD)pPet->GetIndex(), (WORD)pPet->GetLevel()));
#endif	// __CLIENT
}

#ifdef __WORLDSERVER
void CMover::ProcessPetAvail(void)
{
	CPet* pPet = GetPet();
	if (pPet)
	{
		if (HasPet() == FALSE)
		{
			CItemElem* pItemElem = GetPetItem();
			DWORD dwDestParam;
			int nParam;
			pPet->GetAvailDestParam(dwDestParam, nParam);
			if (dwDestParam > 0)
			{
				// wID: dwItemId
				// dwLevel: dwDestParam(H) + nParam(L)
				AddBuff(BUFF_PET, (WORD)(pItemElem->m_dwItemId), MAKELONG((WORD)nParam, (WORD)dwDestParam), 0);
				SetDestParam(dwDestParam, nParam, NULL_CHGPARAM, TRUE);
			}
#if __VER >= 12 // __PET_0519
			SetDestParamRandomOptExtension(pItemElem);
#endif	// __PET_0519
		}
	}
	else
	{
		PetRelease();
	}
}

void CMover::ProcessPetEnergy(void)
{
#ifdef __AIO_PETS
	return;
#endif

	if ((m_nCount & 15) != 0)
		return;

	CPet* pPet = GetPet();
	if (pPet == NULL)
		return;

	if (pPet->GetLevel() == PL_EGG)
		return;

	if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_A)
		|| HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B)
		)
		return;
#if __VER >= 12
	if (HasBuff(BUFF_ITEM2, II_SYS_SYS_SCR_SPETGOOD)
		&& pPet->GetLevel() == PL_S
		)
		return;
#endif


	if (HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET)
#ifdef __JEFF_11_1
		|| HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_FEED_POCKET02)
#endif	// __JEFF_11_1
		)
	{
		CItemElem* ptr;
		int nMax = m_Inventory.GetMax();
		for (int i = 0; i < nMax; i++)
		{
			ptr = m_Inventory.GetAtId(i);
			if (IsUsableItem(ptr) && ptr->m_dwItemId == II_SYS_SYS_FEED_01)
			{
				if ((m_nCount & 0X1F) == 0)
					RemoveItem((BYTE)(ptr->m_dwObjId), 1);

				return;
			}
		}
	}

	CItemElem* pItemElem = GetPetItem();
	int nEnergy = (int)pPet->GetEnergy();
	if (--nEnergy <= 0)
	{
		int nLife = (int)pPet->GetLife();
		if (--nLife >= 0)
		{

			pPet->SetLife(nLife);
			pPet->SetEnergy(CPetProperty::GetInstance()->GetMaxEnergy(pPet->GetLevel()));
		}
		else
		{

			pPet->SetLife(0);
			pPet->SetEnergy(0);
			pPet->SetExp(0);

			pItemElem->SetFlag(CItemElem::expired);
			UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_FLAG, MAKELONG(pItemElem->m_dwObjIndex, pItemElem->m_byFlag));

			PetRelease();
		}
	}
	else
	{
		pPet->SetEnergy(nEnergy);
	}
	((CUser*)this)->AddPetState(pItemElem->m_dwObjId, pPet->GetLife(), pPet->GetEnergy(), pPet->GetExp());
}

void CMover::ProcessPetExp(void)
{
	if ((m_nCount & 1023) != 0)
		return;

	CPet* pPet = GetPet();
	if (pPet == NULL)
		return;


	if (pPet->GetLevel() != PL_EGG && pPet->GetLevel() != PL_S)
	{
		if (pPet->GetExp() == MAX_PET_EXP)
			return;

		float fFactor = HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_PET_TONIC_B) ? 1.5F : 1.0F;

		// 100000 = 100%
		DWORD dwExp = (DWORD)(pPet->GetExp() + CPetProperty::GetInstance()->GetIncrementExp(pPet->GetLevel()) * fFactor);
		if (dwExp > MAX_PET_EXP)
			dwExp = MAX_PET_EXP;
		pPet->SetExp(dwExp);
		((CUser*)this)->AddPetSetExp(dwExp);
	}
}

#endif	// __WORLDSERVER

#ifdef __CLIENT
void	CMover::CreatePetSfx(void)
{
	CMover* pPet = m_pet.GetObj();
	if (!pPet)
		return;

	switch (m_pet.GetLevelup())
	{
	case PF_PET_LEVEL_UP:
		//level up Sound
		PLAYSND("PCSkillCastCheer.wav");
		//level up Effect
		break;
	case PF_PET_LEVEL_DOWN:

		break;
	case PF_PET_GET_AVAIL:
		if (IsActiveMover())
		{
			CWndPetMiracle* pWndMiracle = (CWndPetMiracle*)g_WndMng.GetWndBase(APP_PET_MIRACLE);
			if (pWndMiracle != NULL)
			{
				CPet* pCPet = GetPet();
				int nLevel = pCPet->GetLevel();
				pWndMiracle->ReceiveResult(pCPet->GetAvailLevel(nLevel - 1), pCPet->GetAvailLevel(nLevel));
			}
		}
		break;
	}
}
#endif	// __CLIENT

void CMover::PetRelease(void)
{
#ifdef __CLIENT
	if (m_pet.GetObj())
	{
		m_pet.GetObj()->Delete();
		m_pet.SetObj(NULL);
	}
	SetPetId(NULL_ID);

	if (IsActiveMover())
		PLAYSND("ItemGnCollector.wav");

#else	// __CLIENT
	CItemElem* pItemElem = GetPetItem();
	if (!(pItemElem && pItemElem->m_pPet))
		return;

	if (HasPet())
		RemovePet();

	//	g_dpDBClient.CalluspPetLog(m_idPlayer, pItemElem->GetSerialNumber(), 0, PETLOGTYPE_RELEASE, pItemElem->m_pPet);
	SetPetId(NULL_ID);
	g_UserMng.AddPetRelease(this);
#endif	// __CLIENT
}

#endif	// __PET_0410

BOOL CMover::IsUsingEatPet(CItemElem * pItemElem)
{
	if (HasActivatedEatPet())
	{
		CMover* pEatPet = prj.GetMover(GetEatPetId());
		if (IsValidObj(pEatPet))
		{
			CAIPet* pAIPet = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
			if (pAIPet && pAIPet->GetPetItemId() == pItemElem->m_dwObjId)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CMover::IsUsing(CItemElem * pItemElem)
{
#ifdef __AIO_PETS
	if (pItemElem->m_dwObjId == m_objIdVisPet)
		return TRUE;
#endif

	if (IsUsingEatPet(pItemElem))
		return TRUE;

	if (GetPetId() == pItemElem->m_dwObjId)
		return TRUE;

	if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime > 0
		&& HasBuff(BUFF_ITEM, (WORD)(pItemElem->m_dwItemId)) && !pItemElem->IsFlag(CItemElem::expired))
		return TRUE;

#ifdef __SYS_TICKET
	if (pItemElem == GetTicket())
		return TRUE;
#endif	// __SYS_TICKET

	return FALSE;
}

void	CMover::SetHonorCount(int nIdx, int nCount)
{
	if (nIdx < 0 || nIdx >= MAX_HONOR_TITLE)
		return;

	if (nCount < INT_MAX)
		m_aHonorTitle[nIdx] = nCount;
}
void	CMover::SetHonorAdd(int nSmallIdx, int nLargeGroup, int nDefault)
{
#ifdef __WORLDSERVER
	ASSERT(IsPlayer());
	HonorData* pHonorData = CTitleManager::Instance()->GetHonorDataByID(nSmallIdx, nLargeGroup);
	if (pHonorData)
	{
		if (nDefault == 0)
			SetHonorCount(pHonorData->nID, m_aHonorTitle[pHonorData->nID] + 1);
		else
			SetHonorCount(pHonorData->nID, nDefault);
		if (pHonorData->nNeed == GetHonorTitle(pHonorData->nID))
		{
			((CUser*)this)->AddHonorListAck();
		}

	}
#endif	// __WORLDSERVER
}

void	CMover::CheckHonorStat()
{
#ifdef __WORLDSERVER
	if (m_nHonor < -1 || m_nHonor >= MAX_HONOR_TITLE)
		m_nHonor = -1;

	//////////////////////////////////////////////////////////////////////////
	// initialize
	for (int job_index = MAX_PROFESSIONAL; job_index < MAX_JOB; ++job_index) {
		const int honor_index = CTitleManager::Instance()->GetIdx(job_index, HI_EARN_TITLE);
		SetHonorCount(honor_index, 0);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// set
	for (int sub_job_index = m_nJob; sub_job_index >= MAX_PROFESSIONAL && sub_job_index < MAX_JOB; ) {
		const int honor_index = CTitleManager::Instance()->GetIdx(sub_job_index, HI_EARN_TITLE);
		SetHonorCount(honor_index, 1);
		sub_job_index = GetSubLineJob(sub_job_index);
	}
	//////////////////////////////////////////////////////////////////////////

	int nIdx = CTitleManager::Instance()->GetIdx(HS_STR, HI_COUNT_CHECK);
	SetHonorCount(nIdx, m_nStr);

	nIdx = CTitleManager::Instance()->GetIdx(HS_STA, HI_COUNT_CHECK);
	SetHonorCount(nIdx, m_nSta);

	nIdx = CTitleManager::Instance()->GetIdx(HS_DEX, HI_COUNT_CHECK);
	SetHonorCount(nIdx, m_nDex);

	nIdx = CTitleManager::Instance()->GetIdx(HS_INT, HI_COUNT_CHECK);
	SetHonorCount(nIdx, m_nInt);

	nIdx = CTitleManager::Instance()->GetIdx(HS_LORD, HI_EARN_TITLE);
	if (CSLord::Instance()->IsLord(m_idPlayer))
		SetHonorCount(nIdx, 1);
	else
		SetHonorCount(nIdx, 0);


	if (GetHonorTitle(m_nHonor) < CTitleManager::Instance()->GetNeedCount(m_nHonor, -1))
		m_nHonor = -1;

#endif	// __WORLDSERVER
}

void	CMover::CheckHonorTime()
{
#ifdef __WORLDSERVER
	// m_dwHonorCheckTime
	if (m_pActMover->GetActionState() == OBJSTA_COLLECT)
	{
		DWORD dwTick = GetTickCount();
		if ((m_dwHonorCheckTime + 3600000) < dwTick)
		{
			m_dwHonorCheckTime = dwTick;
			((CUser*)this)->SetHonorAdd(HS_COLLECT, HI_ELASPED_TIME);
		}
	}

	if (m_vtInfo.VendorIsVendor() || m_vtInfo.IsVendorOpen())
	{
		DWORD dwTick = GetTickCount();
		if ((m_dwHonorCheckTime + 3600000) < dwTick)
		{
			m_dwHonorCheckTime = dwTick;
			((CUser*)this)->SetHonorAdd(HS_TRADE, HI_ELASPED_TIME);
		}
	}
#endif	// __WORLDSERVER
}

#ifdef __WORLDSERVER
BOOL CMover::IsReturnToBegin(void)
{
	if (m_pAIInterface)
		return m_pAIInterface->IsReturnToBegin();
	return FALSE;
}
#endif	// __WORLDSERVER

void CMover::SetSpeedFactor(FLOAT fSpeedFactor)
{
	if (fabs(m_fSpeedFactor - fSpeedFactor) > 0.000001F)
	{
		m_fSpeedFactor = fSpeedFactor;
#ifdef __WORLDSERVER
		g_UserMng.AddSetSpeedFactor(this, fSpeedFactor);
#endif	// __WORLDSERVER
	}
}

void	CMover::ProcessCollecting(void)
{
#ifdef __CLIENT
	CItemElem* pCol = GetCollector();

	if (pCol == NULL)
	{

		g_WndMng.CloseCollecting();
	}
	else if (m_pActMover->GetActionState() == OBJSTA_COLLECT)
	{

		// SFX
		if ((m_dwFlag & MVRF_COLLECT) == 0)
		{
			ItemProp* pHandProp = GetActiveHandItemProp();
			if (pHandProp->dwSfxObj2 != NULL_ID)
			{
				D3DXVECTOR3 vSrc, vLocal = D3DXVECTOR3(0, 0.5f, 0);
				((CModelObject*)m_pModel)->GetForcePos(&vLocal, 0, PARTS_RWEAPON, GetMatrixWorld());
				vSrc = vLocal;
				CSfx* pSfx = CreateSfx(D3DDEVICE, pHandProp->dwSfxObj2, vSrc, GetId(), D3DXVECTOR3(0, 0, 0), NULL_ID, -1);
				if (pSfx)
				{
					pSfx->SetAngle(-GetAngle() + 90.0f);
					//pSfx->SetAngleX(90.0f);
					m_dwFlag |= MVRF_COLLECT;
				}
			}
			if (g_WndMng.m_pWndCollecting) g_WndMng.m_pWndCollecting->SetButtonCaption(true);
		}	// SFX
		if (g_WndMng.m_pWndCollecting) g_WndMng.m_pWndCollecting->Update();
		if (m_nCltTime < m_nMaxCltTime) ++m_nCltTime;
		else if (m_nCltTime >= m_nMaxCltTime) m_nCltTime = m_nMaxCltTime;
	}
	else
	{

		g_WndMng.OpenCollecting();
		g_WndMng.m_pWndCollecting->SetButtonCaption(false);
		g_WndMng.m_pWndCollecting->Update();
	}
#endif	// __CLIENT
}

void	CMover::StartCollecting(void)
{
	ClearDest();
	m_dwFlag |= MVRF_NOACTION;
	m_dwMode |= DONMOVE_MODE;
	SendActMsg(OBJMSG_STOP);
	SendActMsg(OBJMSG_COLLECT);
#ifdef __CLIENT
	CItemElem* pCollector = GetCollector();
	CCollectingProperty* pProperty = CCollectingProperty::GetInstance();
	if (pCollector) m_nMaxCltTime = pProperty->GetCool(pCollector->GetAbilityOption()) + 1;
	m_nCltTime = 0;
#endif	// __CLIENT
}

void	CMover::StopCollecting(void)
{
	m_dwFlag &= (~MVRF_NOACTION);
	m_dwMode &= (~DONMOVE_MODE);
	m_pActMover->ResetState(OBJSTA_ACTION_ALL);
	m_pActMover->SetMoveState(OBJSTA_STAND);
	m_pActMover->RemoveStateFlag(OBJSTAF_ETC);
	SetMotion(MTI_STAND);
}

CItemElem* CMover::GetCollector(void)
{
	CItemElem* pCol = GetWeaponItem();
	if (pCol && (!pCol->IsCollector() || pCol->IsFlag(CItemElem::expired)))
		pCol = NULL;
	return pCol;
}

#ifdef __SYS_TICKET
CItemElem* CMover::GetTicket(void)
{
	IBuff* pBuff = m_buffs.GetBuffByIk3(IK3_TICKET);
	if (!pBuff)
		return NULL;

	CItemElem* pTicket;
	for (DWORD i = 0; i < m_Inventory.m_dwItemMax; i++)
	{
		pTicket = m_Inventory.GetAtId(i);
		if (pTicket && pTicket->m_dwKeepTime > 0 && pTicket->GetProp()->dwItemKind3 == IK3_TICKET
			&& pTicket->m_dwItemId == (DWORD)pBuff->GetId()
			&& !pTicket->IsFlag(CItemElem::expired))
			return pTicket;
	}
	return NULL;
}
#endif	// __SYS_TICKET

BOOL CMover::IsShoutFull(void)
{
	return	(IsSMMode(SM_SHOUT15) || IsSMMode(SM_SHOUT30) || IsSMMode(SM_SHOUT001));
}

BOOL CMover::IsCommBank(void)
{
	return	(IsSMMode(SM_BANK15) || IsSMMode(SM_BANK30) || IsSMMode(SM_BANK001));
}

int CMover::GetPerinNum(void)
{
	int nPerin = 0;
	for (int i = 0; i < m_Inventory.GetMax(); i++)
	{
		CItemElem* pItem = static_cast<CItemElem*>(m_Inventory.GetAtId(i));
		if (IsUsableItem(pItem) && pItem->IsPerin())
			nPerin += pItem->m_nItemNum;
	}
	return nPerin;
}


__int64 CMover::GetTotalGold(void)
{
	return static_cast<__int64>(GetPerinNum()) * PERIN_VALUE + static_cast<__int64>(GetGold());
}

#ifdef __WORLDSERVER
int CMover::RemovePerin(int nPerin)
{
	int nRest = nPerin;
	for (int i = 0; i < m_Inventory.GetMax() && nRest > 0; i++)
	{
		CItemElem* pItem = static_cast<CItemElem*>(m_Inventory.GetAtId(i));
		if (IsUsableItem(pItem) && pItem->IsPerin())
		{
			int nRemove = nRest >= pItem->m_nItemNum ? pItem->m_nItemNum : nRest;
			RemoveItem((BYTE)(i), nRemove);
			nRest -= nRemove;
		}
	}
	return nPerin - nRest;
}



int CMover::RemoveTotalGold(__int64 iGold)
{
	ASSERT(iGold <= GetTotalGold());
	int nPerin = (int)(RemovePerin((int)(iGold / PERIN_VALUE)));
	__int64 iRest = iGold - (static_cast<__int64>(nPerin) * PERIN_VALUE);
	if (iRest > GetGold())
	{
		RemovePerin(1);
		iRest = -(PERIN_VALUE - iRest);
	}
	AddGold(static_cast<int>(-iRest), TRUE);
	return nPerin;
}
#endif	// __WORLDSERVER

#ifdef __CLIENT
CClientPet::CClientPet()
	:
	m_pObj(NULL),
	m_nLevelup(0)
{
#ifdef __PET_1024
	m_szName[0] = '\0';
#endif	// __PET_1024
}

CClientPet::~CClientPet()
{
}

#ifdef __PET_1024
void CClientPet::SetName(char* szName)
{
	strncpy(m_szName, szName, MAX_PET_NAME - 1);
	m_szName[MAX_PET_NAME - 1] = '\0';
}
#endif	// __PET_1024
#endif	// __CLIENT

CItemContainer<CItemElem>* CMover::GetContainerByType(int nType)
{
	switch (nType)
	{
	case TYPE_INVENTORY: return &m_Inventory;
	case TYPE_CHEST: return &m_Chest;
	case TYPE_BANK_0:
	{
		if (m_nSlot == 0 || (m_idPlayerBank[0] != 0 && IsCommBank()))
			return &m_Bank[0];
	}
	break;
	case TYPE_BANK_1:
	{
		if (m_nSlot == 1 || (m_idPlayerBank[1] != 0 && IsCommBank()))
			return &m_Bank[1];
	}
	break;
	case TYPE_BANK_2:
	{
		if (m_nSlot == 2 || (m_idPlayerBank[2] != 0 && IsCommBank()))
			return &m_Bank[2];
	}
	break;
	default: return NULL;
	}

	return NULL;
}

BOOL CMover::CanBeUsedOutsideInventory(ItemProp * pItemProp)
{
#ifdef __FL_EQUALIZED_MODE
#ifdef __WORLDSERVER
	if (IsPlayer() && ((CUser*)this)->bTmpMode)
		return FALSE;
#endif
#endif

	if (!pItemProp)
		FALSE;

	if (pItemProp->dwParts != NULL_ID)
		return FALSE;

	if (pItemProp->dwItemKind3 == IK3_EGG || pItemProp->dwItemKind3 == IK3_PET)
		return FALSE;

	if ((pItemProp->dwDestParam1 != -1 && pItemProp->nAdjParamVal1 != -1)
		|| (pItemProp->dwDestParam2 != -1 && pItemProp->nAdjParamVal2 != -1)
		|| (pItemProp->dwDestParam3 != -1 && pItemProp->nAdjParamVal3 != -1))
		return TRUE;

	if (pItemProp->dwItemKind3 == IK3_VIS
		|| pItemProp->dwID == II_SYS_SYS_VIS_KEY01
		|| pItemProp->dwID == II_SYS_SYS_SCR_PET_MAGIC)
		return TRUE;

	if (pItemProp->dwItemKind1 == IK1_HOUSING)
		return TRUE;

	if (pItemProp->dwItemKind3 == IK3_EXP_RATE)
		return TRUE;

#ifdef __FL_MONSTER_ALBUM
	if (pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM
		|| pItemProp->dwItemKind3 == IK3_MONSTER_ALBUM_BOSS_CARD
		|| pItemProp->dwID == II_SYS_SYS_MA_SLOT_UNLOCK
		)
		return TRUE;
#endif

	if (pItemProp->nBoxType == BOX_TYPE_PACKITEM || pItemProp->nBoxType == BOX_TYPE_GIFTBOX)
		return TRUE;

	for (int i = 0; i < SM_MAX; ++i)
	{
		ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);
		if (pItem && pItem == pItemProp)
			return TRUE;
	}

	return FALSE;
}

bool CMover::CheckUserGold(int nGold, bool bAdd)
{
	__int64 n64Gold = (__int64)nGold;

	if (n64Gold < 0 || n64Gold > INT_MAX)
		return false;

	__int64 n64UserGold = (__int64)m_dwGold;

	if (bAdd == true)
	{
		if (n64UserGold + n64Gold > INT_MAX)
			return false;
	}
	else
	{
		if (nGold < 0)
			return false;

		if (n64UserGold - n64Gold < 0)
			return false;
	}

	return true;
}

#ifdef __MODEL_HIDEFASH
#ifdef __CLIENT
BOOL CMover::IsOptionRenderCostume(int nCostumeIdx)
{
	if (nCostumeIdx < 0 || nCostumeIdx > 5)
		return FALSE;

	return !m_bHideFashion[nCostumeIdx];
}
#endif
#endif

#ifdef __FL_EQUALIZED_MODE
#ifdef __WORLDSERVER
BOOL CMover::TemporalPreCheck(DWORD dwWorld)
{
	CUser* pUser = (CUser*)this;
	if (!pUser)
		return FALSE;

	if (dwWorld == WI_WORLD_GUILDWAR)
	{
		CTime tCurrentTime = CTime::GetCurrentTime();
		int nDayofWeek = tCurrentTime.GetDayOfWeek();
		BOOL bIsDay = FALSE;

		switch (nDayofWeek)
		{
		case 1: bIsDay = TRUE; break; // SUNDAY
		case 2: bIsDay = TRUE; break; // Monday
		case 3: bIsDay = TRUE; break; // Tuesday
		case 4: bIsDay = TRUE; break; // Wednesday
		case 5: bIsDay = TRUE; break; // Thursday
		case 6: bIsDay = TRUE; break; // Friday
		case 7: bIsDay = TRUE; break; // Saturday

		default: bIsDay = FALSE; break;
		}

		return bIsDay;
	}
	else
		return TRUE;

}
void CMover::TemporalEnter(int nType, int nClass)
{
	CUser* pUser = (CUser*)this;
	if (pUser->bTmpMode)
		return;

	if (pUser->GetWorld())
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());

	pUser->bTmpMode = TRUE;

#ifdef __FL_MONSTER_ALBUM
	pUser->m_MonsterAlbum.ResetApplyDST();
#endif

	// Clear Pets //
#ifdef __AIO_PETS
	pUser->SetSavedEatPetId(NULL_ID);
#endif
	DisableAllPets();

	// Save & Clear Buffs //
	memcpy(pUser->m_dwSMTimeTmp, m_dwSMTime, sizeof(pUser->m_dwSMTimeTmp));
	pUser->m_buffsTmp = m_buffs;
	pUser->RemoveAllBuff();
	pUser->ClearAllSMMode();

	// Save & Clear Taskbar //
	memcpy(pUser->m_UserTaskBarTemp.m_aSlotApplet, pUser->m_playTaskBar.m_aSlotApplet, sizeof(pUser->m_UserTaskBarTemp.m_aSlotApplet));
	memcpy(pUser->m_UserTaskBarTemp.m_aSlotItem, pUser->m_playTaskBar.m_aSlotItem, sizeof(pUser->m_UserTaskBarTemp.m_aSlotItem));
	memcpy(pUser->m_UserTaskBarTemp.m_aSlotQueue, pUser->m_playTaskBar.m_aSlotQueue, sizeof(pUser->m_UserTaskBarTemp.m_aSlotQueue));
	pUser->m_playTaskBar.InitTaskBar();

#ifdef __SWITCH_EQUIP
	// Save & Clear Switch Equips
	pUser->m_vEquipSwitchInfoTmp.clear();
	pUser->m_vEquipSwitchInfoTmp = m_vEquipSwitchInfo;
	m_vEquipSwitchInfo.clear();
	pUser->SendEquipSwitch();
#endif
#ifdef __FL_SWITCH_EQ
	// Save & Clear Switch Equips
	pUser->m_vecSwitchesTmp.clear();
	pUser->m_vecSwitchesTmp = *pUser->m_Switches.GetSwitches();
	pUser->m_Switches.ClearAllSwitches();
	pUser->AddSwitches();
#endif

	// Save: Skills | Skill Points //
	memcpy(pUser->m_aJobSkillTmp, m_aJobSkill, sizeof(pUser->m_aJobSkillTmp));
	pUser->m_nSkillLevelTmp = m_nSkillLevel;
	pUser->m_nSkillPointTmp = m_nSkillPoint;

	// Save: Stats | Stat Points //
	pUser->m_nRemainGPTmp = m_nRemainGP;
	pUser->m_nStrTmp = m_nStr;
	pUser->m_nStaTmp = m_nSta;
	pUser->m_nDexTmp = m_nDex;
	pUser->m_nIntTmp = m_nInt;

	// Save & Clear Equipment //
	DestroyEquipBonuses();
	pUser->m_InventoryTemp.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
	pUser->m_InventoryTemp.Copy(m_Inventory);
	m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
	RedoEquip(FALSE, FALSE);
	ClearEquipInfo();

	// Save & Init: Level | Class | Exp //
	pUser->m_nTmpJob = GetJob();
	pUser->m_nTmpLv = GetLevel();
	pUser->m_nTmpExp = GetExp1();
	if (nType == 0)
		nType = 120; // Level for TYPE ID 0
	else
		nType = 150; // Level for TYPE ID 1
	InitLevel(nClass, nType);

	// Init Max Skills //
	LPSKILL pSkill = NULL;
	ItemProp* pSkillProp = NULL;
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		pSkill = &(pUser->m_aJobSkill[i]);

		if (pSkill == NULL || pSkill->dwSkill == 0xffffffff)
			continue;

		pSkillProp = prj.GetSkillProp(pSkill->dwSkill);

		if (pSkillProp == NULL)
			continue;

		pSkill->dwLevel = pSkillProp->dwExpertMax;
		pUser->AddSetSkill(pSkill->dwSkill, pSkill->dwLevel);
	}

	// Init Temporal Equipment //
	BOOL bFound = FALSE;
	DWORD dwSex = pUser->GetSex();
	DWORD dwWorldID = pUser->GetWorld()->GetID();

	for (auto& it : g_DPSrvr.m_vecTemporalConfig)
	{
		if (it.dwWorldID == dwWorldID)
		{
			if (it.dwJob == nClass && it.dwSex == dwSex)
			{
				m_Inventory.Copy(*it.m_InventoryPreset);
				bFound = TRUE;
				break;
			}
		}
	}

	if (!bFound)
		pUser->AddText("Cant find equipment for selected world/gender/job..");

	for (int i = 0; i < m_Inventory.GetMax(); i++)
	{
		CItemElem* pItem = static_cast<CItemElem*>(m_Inventory.GetAtId(i));
		if (pItem && pItem->IsVisPet())
		{
			for (int j = 0; j < pItem->GetPiercingSize(); j++)
				pItem->SetVisKeepTime(j, 10800 + time_null()); // Beads time for all pets in temporal equipment (now 3 hours)
		}
	}

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		CItemElem* pItemElem = GetEquipItem(i);
		if (pItemElem)
		{
			m_aEquipInfo[i].dwId = pItemElem->m_dwItemId;
			m_aEquipInfo[i].nOption = pItemElem->GetAttrOption();
#ifdef __MODEL_CHANGE
			m_aEquipInfo[i].dwLook = pItemElem->GetLook();
#endif
		}
	}
	RedoEquip(FALSE);
	pUser->AddInvAndBuffsSerialize(false);
	g_UserMng.AddUpdateEquipInfo(this);
	UpdateTemporalCounter(TRUE); // add
}
void CMover::TemporalOut(BOOL bSave)
{
	CUser* pUser = (CUser*)this;
	if (!pUser->bTmpMode)
		return;

	UpdateTemporalCounter(FALSE); // remove

	// Clear Pets //
#ifdef __AIO_PETS
	pUser->SetSavedEatPetId(NULL_ID);
#endif
	DisableAllPets();

	// Clear Equipment //
	DestroyEquipBonuses();

	// Back: Level | Class | Exp //
	InitLevel(pUser->m_nTmpJob, pUser->m_nTmpLv);
	SetExperience(pUser->m_nTmpExp, pUser->m_nTmpLv);

	// Back: Stats | Stat Points //
	pUser->m_nRemainGP = pUser->m_nRemainGPTmp;
	m_nStr = pUser->m_nStrTmp;
	m_nSta = pUser->m_nStaTmp;
	m_nDex = pUser->m_nDexTmp;
	m_nInt = pUser->m_nIntTmp;

	// Clear & Back Buffs //
	pUser->RemoveAllBuff();
	pUser->ClearAllSMMode();
	m_buffs = pUser->m_buffsTmp;
	for (MAPBUFF::iterator i = m_buffs.m_mapBuffs.begin(); i != m_buffs.m_mapBuffs.end(); ++i)
	{
		IBuff* pBuff = i->second;
		if (pBuff)
			pBuff->Apply(this);
	}
	memcpy(m_dwSMTime, pUser->m_dwSMTimeTmp, sizeof(m_dwSMTime));
	for (int i = 0; i < SM_MAX; ++i)
	{
		if (m_dwSMTime[i] > 0)
		{
			if (i == SM_PARTYSKILL1 || i == SM_PARTYSKILL15 || i == SM_PARTYSKILL30)
				g_DPCoreClient.SendUserPartySkill(m_idPlayer, PARTY_PARSKILL_MODE, 1000, 0, 1);
			else if (i == SM_VELOCIJUMP)
			{
				ItemProp* pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_VELOCIJUMP);
				if (pItemProp)
				{
					if (pItemProp->dwDestParam1 != -1)
						SetDestParam(pItemProp->dwDestParam1, pItemProp->nAdjParamVal1, pItemProp->nAdjParamVal1);
					if (pItemProp->dwDestParam2 != -1)
						SetDestParam(pItemProp->dwDestParam2, pItemProp->nAdjParamVal2, pItemProp->nAdjParamVal2);
				}
			}
			else if (i == SM_MAX_HP)
			{
				ItemProp* pItemProp = prj.GetItemProp(II_CHR_FOO_COO_BULLHAMS);
				if (pItemProp)
					SetDestParam(pItemProp->dwDestParam1, pItemProp->nAdjParamVal2, pItemProp->nAdjParamVal2);
			}
			else if (i == SM_MAX_HP50)
			{
				ItemProp* pItemProp = prj.GetItemProp(II_CHR_FOO_COO_GRILLEDEEL);
				if (pItemProp)
				{
					DWORD nAdjParamVal;
					m_nPlusMaxHitPoint = nAdjParamVal = int(GetMaxOriginHitPoint() / (100 / pItemProp->nAdjParamVal1));
					SetDestParam(pItemProp->dwDestParam1, nAdjParamVal, nAdjParamVal);
				}
			}
		}
	}
#ifdef __WBQT_GUILD_BUFFS
	pUser->RemoveIk3Buffs(IK3_GUILD_BUFF);
#endif

	// Back Inventory //
	m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
	m_Inventory.Copy(pUser->m_InventoryTemp);
	pUser->m_InventoryTemp.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
	ClearEquipInfo();
	RedoEquip(FALSE);
	pUser->AddInvAndBuffsSerialize(true);

	// Back: Skills | Skill Points //
	memcpy(m_aJobSkill, pUser->m_aJobSkillTmp, sizeof(m_aJobSkill));
	ZeroMemory(pUser->m_aJobSkillTmp, sizeof(pUser->m_aJobSkillTmp));
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		pUser->m_aJobSkillTmp[i].dwSkill = NULL_ID;
		pUser->m_aJobSkillTmp[i].dwLevel = 0;
	}
	m_nSkillLevel = pUser->m_nSkillLevelTmp;
	m_nSkillPoint = pUser->m_nSkillPointTmp;

#ifdef __SWITCH_EQUIP
	// Clear & Back Switch Equips
	m_vEquipSwitchInfo.clear();
	m_vEquipSwitchInfo = pUser->m_vEquipSwitchInfoTmp;
	pUser->m_vEquipSwitchInfoTmp.clear();
	pUser->SendEquipSwitch();
#endif

#ifdef __FL_SWITCH_EQ
	pUser->m_Switches.ClearAllSwitches();
	*pUser->m_Switches.GetSwitches() = pUser->m_vecSwitchesTmp;
	pUser->m_vecSwitchesTmp.clear();
	pUser->AddSwitches();
#endif

	// Clear & Back Taskbar //
	pUser->m_playTaskBar.InitTaskBar();
	memcpy(pUser->m_playTaskBar.m_aSlotApplet, pUser->m_UserTaskBarTemp.m_aSlotApplet, sizeof(pUser->m_playTaskBar.m_aSlotApplet));
	memcpy(pUser->m_playTaskBar.m_aSlotItem, pUser->m_UserTaskBarTemp.m_aSlotItem, sizeof(pUser->m_playTaskBar.m_aSlotItem));
	memcpy(pUser->m_playTaskBar.m_aSlotQueue, pUser->m_UserTaskBarTemp.m_aSlotQueue, sizeof(pUser->m_playTaskBar.m_aSlotQueue));
	pUser->m_UserTaskBarTemp.InitTaskBar();
	pUser->AddTaskBar();

#ifdef __FL_MONSTER_ALBUM
	pUser->m_MonsterAlbum.SetApplyDST();
#endif

	pUser->bTmpMode = FALSE;

	if (bSave && pUser->GetWorld())
		g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
}
void CMover::DisableAllPets()
{
	if (HasActivatedSystemPet())
		PetRelease();

	if (HasActivatedEatPet())
	{
		CMover* pEatPet = prj.GetMover(GetEatPetId());
		if (IsValidObj(pEatPet))
			InactivateEatPet();
	}

	if (HasPet())
		RemovePet();
}
void CMover::DestroyEquipBonuses()
{
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		CItemElem* pItemElemOld = m_Inventory.GetEquip(i);
		if (pItemElemOld && pItemElemOld->GetProp())
		{
			ResetDestParamEquip(pItemElemOld->GetProp(), pItemElemOld, TRUE);
			prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElemOld, FALSE, i);
		}
	}

	UpdateParts(FALSE);
	InvalidEquipOff();

	int nAbilityOption = GetSetItem();
	if (nAbilityOption > 0)
		ResetSetItemAvail(nAbilityOption);

	CItemElem* pItemElem;
	ITEMAVAIL itemAvail;
	memset(&itemAvail, 0, sizeof(itemAvail));

	map<CSetItem*, int>	mapSetItem;
	map<CSetItem*, int>::iterator i;
	for (int nParts = 0; nParts < MAX_HUMAN_PARTS; nParts++)
	{
		pItemElem = GetEquipItem(nParts);
		if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
		{
			CSetItem* pSetItem = CSetItemFinder::GetInstance()->GetSetItemByItemId(pItemElem->m_dwItemId);
			if (pSetItem)
			{
				i = mapSetItem.find(pSetItem);
				if (i != mapSetItem.end())
					i->second++;
				else
					mapSetItem.insert(map<CSetItem*, int>::value_type(pSetItem, 1));
			}
		}
	}

	for (i = mapSetItem.begin(); i != mapSetItem.end(); ++i)
	{
		CSetItem* pSetItem = i->first;
		int nEquiped = i->second;
		pSetItem->GetItemAvail(&itemAvail, nEquiped, TRUE);
	}

	for (int i = 0; i < itemAvail.nSize; i++)
		ResetDestParam(itemAvail.anDstParam[i], itemAvail.anAdjParam[i], TRUE);
}

void CMover::UpdateTemporalCounter(BOOL bAdd)
{
	int nValue = GetJob();
	if (nValue == JOB_BLADE || nValue == JOB_BLADE_MASTER || nValue == JOB_BLADE_HERO || nValue == JOB_STORMBLADE_HERO)
		nValue = 0;
	else if (nValue == JOB_KNIGHT || nValue == JOB_KNIGHT_MASTER || nValue == JOB_KNIGHT_HERO || nValue == JOB_LORDTEMPLER_HERO)
		nValue = 1;
	else if (nValue == JOB_JESTER || nValue == JOB_JESTER_MASTER || nValue == JOB_JESTER_HERO || nValue == JOB_WINDLURKER_HERO)
		nValue = 2;
	else if (nValue == JOB_RANGER || nValue == JOB_RANGER_MASTER || nValue == JOB_RANGER_HERO || nValue == JOB_CRACKSHOOTER_HERO)
		nValue = 3;
	else if (nValue == JOB_PSYCHIKEEPER || nValue == JOB_PSYCHIKEEPER_MASTER || nValue == JOB_PSYCHIKEEPER_HERO || nValue == JOB_MENTALIST_HERO)
		nValue = 4;
	else if (nValue == JOB_ELEMENTOR || nValue == JOB_ELEMENTOR_MASTER || nValue == JOB_ELEMENTOR_HERO || nValue == JOB_ELEMENTORLORD_HERO)
		nValue = 5;
	else if (nValue == JOB_BILLPOSTER || nValue == JOB_BILLPOSTER_MASTER || nValue == JOB_BILLPOSTER_HERO || nValue == JOB_FORCEMASTER_HERO)
		nValue = 6;
	else if (nValue == JOB_RINGMASTER || nValue == JOB_RINGMASTER_MASTER || nValue == JOB_RINGMASTER_HERO || nValue == JOB_FLORIST_HERO)
		nValue = 7;

	map<u_long, vector<TEMPORAL_LIMITS>>::iterator it = prj.map_DynamicTemporalCounter.find(GetWorld()->GetID());
	if (it != prj.map_DynamicTemporalCounter.end())
	{
		for (auto& it2 : it->second)
		{
			if (it2.nJob == nValue)
			{
				if (bAdd)
					it2.nCurrentAmountOfPlayers++;
				else
					it2.nCurrentAmountOfPlayers--;

				break;
			}
		}
	}
}

#endif // ws
#endif


#ifdef __JOB_CHANGE
void CMover::ChangeJob(int nJob)
{
#ifdef __WORLDSERVER
	MoverProp* pProp = GetProp();
	if (pProp)
	{
		if (nJob != JOB_PUPPETEER && nJob != JOB_GATEKEEPER && nJob != JOB_DOPPLER && nJob >= 0
			&& nJob < MAX_LEGEND_HERO
			)
		{
			AddChangeJob(nJob);
			int nLevel = 0;
			if (nJob >= JOB_MERCENARY && nJob <= JOB_MAGICIAN)
			{
				nLevel = 15;
				m_nStr = m_nSta = m_nDex = m_nInt = 15;
				m_nRemainGP = 28;
			}
			if (nJob >= JOB_KNIGHT && nJob <= JOB_ELEMENTOR)
			{
				nLevel = 60;
				m_nStr = m_nSta = m_nDex = m_nInt = 15;
				m_nRemainGP = 118;
			}
			if (nJob >= JOB_KNIGHT_MASTER && nJob <= JOB_ELEMENTOR_MASTER)
			{
				nLevel = 60;
				m_nStr = m_nSta = m_nDex = m_nInt = 15;
				m_nRemainGP = 118;
			}
			if (nJob >= JOB_KNIGHT_HERO && nJob <= JOB_ELEMENTOR_HERO)
			{
				nLevel = 121;
				m_nStr = m_nSta = m_nDex = m_nInt = 15;
				m_nRemainGP = 313;
			}
			if (nJob >= JOB_LORDTEMPLER_HERO && nJob <= JOB_ELEMENTORLORD_HERO)
			{
				nLevel = 130;
				m_nStr = m_nSta = m_nDex = m_nInt = 15;
				m_nRemainGP = 340;
			}

			m_nLevel = nLevel;
			m_nJob = nJob;

			// Find total points for this job/level
			int nSkillPoint = GetTotalSkillPoint(nJob, m_nLevel);

			// Find all skills which not level 0 and calculate their total points
			int nPointsAtSkills = 0;
			for (int i = 0; i < MAX_SKILL_JOB; ++i)
			{
				LPSKILL pSkill = &m_aJobSkill[i];
				if (pSkill != NULL && pSkill->dwSkill != NULL_ID)
				{
					ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
					if (pSkillProp != NULL)
					{
						if (0 < pSkill->dwLevel && pSkillProp->dwItemKind1 != JTYPE_MASTER && pSkillProp->dwItemKind1 != JTYPE_HERO)
							nPointsAtSkills += (pSkill->dwLevel * prj.GetSkillPoint(pSkillProp));
					}
				}
			}

			// We not wipe skills, so we should give to the player only free points, but not all of them
			nSkillPoint -= nPointsAtSkills;

			m_nSkillLevel = m_nSkillPoint = nSkillPoint;

			m_nExp1 = 0;

			((CUser*)this)->AddSetChangeJob(nJob);
			g_UserMng.AddNearSetChangeJob(this, nJob, &((CUser*)this)->m_aJobSkill[MAX_JOB_SKILL]);

			((CUser*)this)->LevelUpSetting();
			((CUser*)this)->AddSetExperience(GetExp1(), (WORD)m_nLevel, m_nSkillPoint, m_nSkillLevel);

			g_UserMng.AddStatInfo(this, TRUE);

			((CUser*)this)->CheckHonorStat();
			((CUser*)this)->AddHonorListAck();
			g_UserMng.AddHonorTitleChange(this, m_nHonor);
		}
	}
#endif // __WORLDSERVER
}
#endif

BOOL CMover::IsPossibleUseItem()
{
	if (!IsPlayer())
		return FALSE;

	if (IsDie())
		return FALSE;

	if (m_vtInfo.GetOther())
		return FALSE;

	if (m_vtInfo.VendorIsVendor())
		return FALSE;

#ifdef __S_SERVER_UNIFY
#ifdef __WORLDSERVER
	if (!((CUser*)this)->m_bAllAction)
		return FALSE;
#else
	if (!g_WndMng.m_bAllAction)
		return FALSE;
#endif
#endif

	if (GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return FALSE;

	return TRUE;
}

#ifdef __WORLDSERVER
void CMover::MakeCharacterAlive(BOOL bSamePlace, BOOL bScroll)
{
	if (!IsPlayer())
		return;

	if (!IsDie())
		return;

	CWorld* pWorld = GetWorld();
	if (!pWorld)
		return;

	if (bScroll)
	{
		CItemElem* pItemElem = m_Inventory.GetAtItemId(II_SYS_SYS_SCR_RESURRECTION);
		if (!IsUsableItem(pItemElem))
			return;

		RemoveItem((BYTE)pItemElem->m_dwObjId, 1);

		ItemProp* pItemProp = pItemElem->GetProp();
		if (pItemProp && pItemProp->dwSfxObj3 != -1)
			g_UserMng.AddCreateSfxObj(this, pItemProp->dwSfxObj3);
	}

	CUser* pUser = (CUser*)this;

	// Change position
	if (pWorld->GetID() == WI_WORLD_GUILDWAR)
		g_GuildCombatMng.JoinObserver(pUser, FALSE);
#ifdef __FL_FFA
	else if (pWorld->IsFFA())
		CFFAManager::GetInstance()->TeleportToSpawn(pUser);
#endif
#ifdef __FL_TDM
	else if (pWorld->IsTDM())
		CTDMManager::GetInstance()->TeleportToSpawn(pUser);
#endif
	else
	{
		if (!bSamePlace)
		{
			if (CSecretRoomMng::GetInstance()->IsInTheSecretRoom(pUser))
				pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, CSecretRoomMng::GetInstance()->GetRevivalPos(pUser), REPLACE_NORMAL, nDefaultLayer);
			else if (pWorld->GetID() == WI_DUNGEON_MUSCLE || pWorld->GetID() == WI_DUNGEON_KRRR || pWorld->GetID() == WI_DUNGEON_BEAR)
				pUser->REPLACE(g_uIdofMulti, WI_WORLD_MADRIGAL, D3DXVECTOR3(6968.0f, 0.0f, 3328.8f), REPLACE_NORMAL, nDefaultLayer);
			else
			{
				PRegionElem	pRgnElem = NULL;
				if (pWorld->GetID() != pWorld->m_dwIdWorldRevival && pWorld->m_dwIdWorldRevival != 0)
					pRgnElem = g_WorldMng.GetRevivalPos(pWorld->m_dwIdWorldRevival, pWorld->m_szKeyRevival);

				if (!pRgnElem)
					pRgnElem = g_WorldMng.GetNearRevivalPos(pWorld->GetID(), pUser->GetPos());

				if (pRgnElem)
					pUser->REPLACE(g_uIdofMulti, pRgnElem->m_dwWorldId, pRgnElem->m_vPos, REPLACE_FORCE, nRevivalLayer);
				else
					pUser->REPLACE(g_uIdofMulti, pWorld->GetID(), pUser->GetPos(), REPLACE_FORCE, pUser->GetLayer());
			}
		}
	}

	// Respawn
	m_pActMover->SendActMsg(OBJMSG_RESURRECTION);
	g_UserMng.AddHdr(pUser, SNAPSHOTTYPE_RESURRECTION);

	// Init HP/MP/FP
	float fHpRate = 0.2f;
	float fMpRate = 0.2f;
	float fFpRate = 0.2f;

	if (pWorld->GetID() == WI_WORLD_GUILDWAR
#ifdef __FL_FFA
		|| pWorld->IsFFA()
#endif
#ifdef __FL_TDM
		|| pWorld->IsTDM()
#endif
		)
	{
		fHpRate = 1.0f;
		fMpRate = 1.0f;
		fFpRate = 1.0f;
	}

	if (IsSMMode(SM_MAINTAIN_MP))
		fMpRate = 1.0f;

	if (IsSMMode(SM_MAINTAIN_FP))
		fFpRate = 1.0f;

	// HP
	SetPointParam(DST_HP, static_cast<int>(GetMaxHitPoint() * fHpRate));

	// MP
	const int recoveryMP = static_cast<int>(GetMaxManaPoint() * fMpRate);
	if (recoveryMP > GetManaPoint())
		SetPointParam(DST_MP, recoveryMP);

	// FP
	const int recoveryFP = static_cast<int>(GetMaxFatiguePoint() * fFpRate);
	if (recoveryFP > GetFatiguePoint())
		SetPointParam(DST_FP, recoveryFP);
}
#endif

bool	CMover::CheckAttackDistance(CObj * pTarget, const DWORD AttackRangeType)
{
#ifdef __WORLDSERVER
	if (IsPlayer() == FALSE)
		return true;
	if (pTarget->GetType() != OT_MOVER || ::IsInvalidObj(pTarget))
		return false;

	CMover* pTargetMover = static_cast<CMover*>(pTarget);

	/*	const float fAllowedFrame = 120.0f;	// 2.0 sec
		const float fMoverSpeed = pTargetMover->GetSpeed(pTargetMover->m_pActMover->m_fSpeed);
		float fAllowedRange = fMoverSpeed * fAllowedFrame;
		if (fAllowedRange < 1.0f)
			fAllowedRange = 1.0f;

		if (IsRangeObj(pTarget, GetAttackRange(AttackRangeType) + fAllowedRange) == FALSE)
			return false;*/

	const float fAllowedExtraRange = 4.5f;
	const float fRealRange = GetAttackRange(AttackRangeType);

	if (IsRangeObj(pTarget, fRealRange + fAllowedExtraRange) == FALSE)
		return false;
#endif // __WORLDSERVER
	return true;
}

int		CMover::GetGeneralSkillPoint(const DWORD dwJob, const DWORD dwLevel)
{
	int nSkillPoint = 0;
	int nGeneralLevel = dwLevel;

	if (dwJob >= MAX_PROFESSIONAL)
	{
		nGeneralLevel = MAX_GENERAL_LEVEL;
	}

	for (int i = 2; i <= nGeneralLevel; ++i)
	{
		nSkillPoint += ((i - 1) / SKILLPOINT_DIVIDE) + SKILLPOINT_BASE;
	}

	return nSkillPoint;
}

int		CMover::GetTotalSkillPoint(const DWORD dwJob, const DWORD dwLevel)
{
	int nSkillPoint = 0;
	nSkillPoint = GetGeneralSkillPoint(dwJob, dwLevel);

	switch (dwJob)
	{
	case JOB_MERCENARY:
		nSkillPoint += 40;
		break;

	case JOB_ACROBAT:
		nSkillPoint += 50;
		break;

	case JOB_ASSIST:
		nSkillPoint += 60;
		break;

	case JOB_MAGICIAN:
		nSkillPoint += 90;
		break;

	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
		nSkillPoint += 40;
		nSkillPoint += 80;
		break;

	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
		nSkillPoint += 50;
		nSkillPoint += 100;
		break;

	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		nSkillPoint += 60;
		nSkillPoint += 100;
		break;

	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		nSkillPoint += 60;
		nSkillPoint += 120;
		break;

	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		nSkillPoint += 90;
		nSkillPoint += 90;
		break;

	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		nSkillPoint += 90;
		nSkillPoint += 300;
		break;

	default:
		break;
	}

	if (dwJob >= MAX_PROFESSIONAL)
		nSkillPoint += 400;

	if (dwJob >= MAX_HERO)
	{
		int nRemainLevel = dwLevel - MAX_LEGEND_LEVEL;

		if (nRemainLevel > 0)
		{
			if (dwJob == JOB_LORDTEMPLER_HERO
				|| dwJob == JOB_FLORIST_HERO
				)
				nSkillPoint += nRemainLevel * SKILLPOINT_LEGENDHERO_EXCEPTION;
			else
				nSkillPoint += nRemainLevel * SKILLPOINT_LEGENDHERO_BASE;
		}

		switch (dwJob)
		{
		case JOB_LORDTEMPLER_HERO:
			nSkillPoint += 40;
			nSkillPoint += 80;
			nSkillPoint += 24;
			break;

		case JOB_STORMBLADE_HERO:
			nSkillPoint += 40;
			nSkillPoint += 80;
			nSkillPoint += 16;
			break;

		case JOB_WINDLURKER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			nSkillPoint += 50;
			nSkillPoint += 100;
			nSkillPoint += 16;
			break;

		case JOB_FLORIST_HERO:
			nSkillPoint += 60;
			nSkillPoint += 100;
			nSkillPoint += 24;
			break;

		case JOB_FORCEMASTER_HERO:
			nSkillPoint += 60;
			nSkillPoint += 120;
			nSkillPoint += 16;
			break;

		case JOB_MENTALIST_HERO:
			nSkillPoint += 90;
			nSkillPoint += 90;
			nSkillPoint += 20;
			break;

		case JOB_ELEMENTORLORD_HERO:
			nSkillPoint += 90;
			nSkillPoint += 300;
			nSkillPoint += 16;
			break;

		default:
			break;
		}
	}


	return nSkillPoint;
}

DWORD	CMover::GetSubLineJob(const DWORD dwJob)
{
	if (dwJob < MAX_JOBBASE || dwJob > MAX_JOB)
	{
		return NULL_ID;
	}

	else if (dwJob >= MAX_JOBBASE && dwJob < MAX_EXPERT)
	{
		return JOB_VAGRANT;
	}

	else if (dwJob >= MAX_EXPERT && dwJob < MAX_PROFESSIONAL)
	{
		return (dwJob / 2 - 2);
	}

	else if (dwJob >= MAX_PROFESSIONAL && dwJob < MAX_MASTER)
	{
		return (dwJob - 10);
	}

	else if (dwJob >= MAX_MASTER && dwJob < MAX_HERO)
	{
		return (dwJob - 8);
	}

	else if (dwJob >= MAX_HERO && dwJob < MAX_LEGEND_HERO)
	{
		return (dwJob - 8);
	}

	else
	{
		return NULL_ID;
	}
}

void CMover::ClearCastingSkills(BOOL bSend)
{
	DWORD	dwTmpTick = GetTickCount();
	int	nTmpSkillID = m_pActMover->GetCastingSKillID();
	if (m_pActMover->GetCastingEndTick() > dwTmpTick && (nTmpSkillID == SI_KNT_HERO_DRAWING || nTmpSkillID == SI_RIG_HERO_RETURN))
	{
		m_pActMover->SetCastingEndTick(0);
		if (!IsDie())
			m_pActMover->ClearState();
#ifdef __WORLDSERVER
		if (bSend)
			g_UserMng.AddHdr(this, SNAPSHOTTYPE_CLEARCASTING);
#endif
	}
}

#ifdef __FL_VENDORS
#ifdef __WORLDSERVER 
void CMover::ShutdownVendor(BOOL bForceAll)
{
	if (GetNpcType() != NPC_TYPE_VENDOR_HUMAN && GetNpcType() != NPC_TYPE_VENDOR_NPC)
		return;

	auto it = prj.m_mapPlayerShops.find(GetNpcOwner());
	if (it == prj.m_mapPlayerShops.end())
		return;

#ifdef __FL_SHOP_SEARCH
	prj.RemoveFromShopSearch(GetId());
#endif

	CUser* pUser = (CUser*)prj.GetUserByID(it->first);

	// Send items from vendor back to the player
	for (int i = 0; i < MAX_VENDITEM; i++)
		m_vtInfo.VendorToPlayerSingle(pUser, i);

	// Send offline vendor item back to the player
	if ((DWORD)(time(0)) < GetVendorTime())
	{
		CItemElem itemVendor;
		itemVendor.m_dwItemId = GetVendorItemId();
		itemVendor.m_nItemNum = 1;
		itemVendor.SetSerialNumber();
		itemVendor.m_dwKeepTime = GetVendorTime() - time(0);

		if (IsValidObj(pUser))
		{
			if (!pUser->CreateItem(&itemVendor))
				g_dpDBClient.SendQueryPostMail(pUser->m_idPlayer, 0, itemVendor, 0, "Offline Vendor", "You can continue use this vendor.");
		}
		else
			g_dpDBClient.SendQueryPostMail(it->first, 0, itemVendor, 0, "Offline Vendor", "You can continue use this vendor.");

	}

	if (IsValidObj(pUser))
	{
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_VENDOR_DEACTIVATE);
		if (pUser->GetWorld())
			g_dpDBClient.SavePlayer(pUser, pUser->GetWorld()->GetID(), pUser->GetPos(), pUser->GetLayer());
	}

	g_dpDBClient.SendDeleteVendor(it->first);

	if (!bForceAll)
		prj.m_mapPlayerShops.erase(it);

	Delete();
}
#endif
#endif

void CMover::SetRide(CModel * pModel, int nRideItemIdx)
{
	if (m_pRide && m_pRide->IsAniable())
		SAFE_DELETE(m_pRide);

	m_pRide = (CModelObject*)pModel;
	m_dwRideItemIdx = nRideItemIdx;
}

BOOL CMover::IsIgnoreCollisionSkill(DWORD dwSkill)
{
	switch (dwSkill)
	{
	case SI_ASS_HEAL_HEALING: return TRUE;
	case SI_ASS_HEAL_RESURRECTION: return TRUE;
	default: return FALSE;
	}

	return FALSE;
}
#ifdef __WORLDSERVER
void CMover::RefreshGrilledEel()
{
	if (IsSMMode(SM_MAX_HP50))
	{
		ItemProp* pItemProp = prj.GetItemProp(g_AddSMMode.dwSMItemID[SM_MAX_HP50]);
		if (pItemProp)
		{
			const LONG nOldPlusMaxHitPoint = m_nPlusMaxHitPoint;
			m_nPlusMaxHitPoint = int(GetMaxOriginHitPoint() / (100 / pItemProp->nAdjParamVal1));

			if (nOldPlusMaxHitPoint > m_nPlusMaxHitPoint)
				ResetDestParam(pItemProp->dwDestParam1, nOldPlusMaxHitPoint - m_nPlusMaxHitPoint);
			else if (nOldPlusMaxHitPoint < m_nPlusMaxHitPoint)
				SetDestParam(pItemProp->dwDestParam1, m_nPlusMaxHitPoint - nOldPlusMaxHitPoint, m_nPlusMaxHitPoint - nOldPlusMaxHitPoint);
		}
	}
}
#endif