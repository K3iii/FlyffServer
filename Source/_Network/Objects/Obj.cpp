#include "stdafx.h"
#include "..\_Network\Objects\Obj.h"

#pragma warning ( disable : 4355 )

extern	CProject	prj;

#include "serialnumber.h"

int		CObj::m_nMethod = METHOD_NONE;

CObj::CObj()
#ifdef __LAYER_1015
	: m_nLayer(0)
#endif	// __LAYER_1015
{
	m_dwType = m_dwIndex = m_dwMotion = 0;
	m_vPos = D3DXVECTOR3(0, 0, 0);
	m_vScale = D3DXVECTOR3(1, 1, 1);
	m_fAngle = 0;
}

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid = NULL_ID;
}

CItemElem::CItemElem()
{
	memset(m_szItemText, 0, sizeof(m_szItemText));
	m_dwItemId = 0;
	m_dwObjId = NULL_ID;
	m_dwObjIndex = NULL_ID;
	m_liSerialNumber = 0;

	m_byFlag = 0;
	m_nItemNum = 1;
	m_nAbilityOption = 0;
	m_nHitPoint = 0;
	m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;
	m_dwKeepTime = 0;
	m_iRandomOptItemId = 0;
	m_pPet = NULL;
	m_bTranformVisPet = FALSE;
#ifdef __MODEL_CHANGE
	m_dwLookId = NULL_ID;
#endif

#ifdef __WEAPON_RARITY
	m_nWeaponRarity = 0;
#endif // __WEAPON_RARITY
}

CItemElem::~CItemElem()
{
	SAFE_DELETE(m_pPet);
#ifdef __FL_PACKED_ITEMS
	ClearPackedItems();
#endif
}

void	CItemElem::Empty(void)
{
	m_szItemText[0] = '\0';
	m_dwItemId = 0;

	SAFE_DELETE(m_pPet);
#ifdef __FL_PACKED_ITEMS
	ClearPackedItems();
#endif
	m_piercing.Clear();
}

CItemElem& CItemElem::operator =(CItemElem& ie)
{
	_tcscpy(m_szItemText, ie.m_szItemText);
	m_dwItemId = ie.m_dwItemId;
	m_liSerialNumber = ie.m_liSerialNumber;

	m_nItemNum = ie.m_nItemNum;
	m_nHitPoint = ie.m_nHitPoint;
	m_byFlag = ie.m_byFlag;
	m_nAbilityOption = ie.m_nAbilityOption;
	m_bItemResist = ie.m_bItemResist;
	m_nResistAbilityOption = ie.m_nResistAbilityOption;
	m_dwKeepTime = ie.m_dwKeepTime;
	m_piercing = ie.m_piercing;
	m_iRandomOptItemId = ie.GetRandomOptItemId();

	SAFE_DELETE(m_pPet);
	if (ie.m_pPet)
	{
		m_pPet = new CPet;
		*m_pPet = *ie.m_pPet;
	}

#ifdef __FL_PACKED_ITEMS
	ClearPackedItems();
	for (auto& it : ie.m_vecPackedItems)
		AddToPack(it);
#endif

	m_bTranformVisPet = ie.m_bTranformVisPet;
#ifdef __MODEL_CHANGE
	m_dwLookId = ie.GetLook();
#endif

#ifdef __WEAPON_RARITY
	m_nWeaponRarity = ie.m_nWeaponRarity;
#endif // __WEAPON_RARITY

	return *this;
}


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
}

CItem::~CItem()
{
	SAFE_DELETE(m_pItemBase);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//#ifdef __DBSERVER
#if defined(__DBSERVER)
#ifndef __VM_0820
#ifndef __MEM_TRACE
CMoverPool* CMover::m_pPool = new CMoverPool(1024);
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __DBSERVER

CMover::CMover()
#ifdef __BUFF_1107
	: m_buffs(this)
#endif	// __BUFF_1107
#ifdef __FL_SWITCH_EQ
	, m_Switches(this)
#endif
#ifdef __FL_MONSTER_ALBUM
	, m_MonsterAlbum(this)
#endif
{
	memset(m_aJobSkill, 0, sizeof(SKILL) * MAX_SKILL_JOB);
	m_dwBelligerence = 0;
#ifdef __JEFF_9_20
	m_dwMute = 0;
#else	// __JEFF_9_20
	m_dwFlyTime = 0;
#endif	// __JEFF_9_20
	m_dwReturnWorldID = 0;
	memset(m_szName, 0, sizeof(m_szName));
	m_vMarkingPos = D3DXVECTOR3(0, 0, 0);
	m_vReturnPos = D3DXVECTOR3(0, 0, 0);
	m_dwType = OT_MOVER;
	m_bPlayer = FALSE;
	m_idPlayer = (u_long)0xffffffff;
	//	memset( m_szName, 0, sizeof(m_szName) );
	memset(m_szCharacterKey, 0, sizeof(m_szCharacterKey));
	m_nHitPoint = m_nManaPoint = m_nFatiguePoint = 0;
	m_pActMover = new CActionMover(this);
	m_Inventory.SetItemContainer(ITYPE_ITEM, MAX_INVENTORY, MAX_HUMAN_PARTS);
#ifdef __FL_SWITCH_EQ
	m_Inventory.m_bInventory = TRUE;
	m_Inventory.m_pMover = this;
#endif

#ifdef __FL_CONSUMABLES_STORAGE
	m_Chest.SetItemContainer(ITYPE_ITEM, MAX_CHEST);
#endif
	m_nSlot = 0;
	for (int i = 0; i < 3; ++i)
	{
		m_Bank[i].SetItemContainer(ITYPE_ITEM, MAX_BANK);
		m_dwGoldBank[i] = 0;
		m_idPlayerBank[i] = 0;
	}
	m_nPlusMaxHitPoint = 0;
	memset(m_dwSMTime, 0, sizeof(DWORD) * SM_MAX);
#ifndef __BUFF_1107
	m_SkillState.Init();
	m_SkillState.SetMover(this);
#endif	// __BUFF_1107
	ClearEquipInfo();

	m_dwSkinSet = 0;//SKINSET_00;
	m_dwHairMesh = 0;//HAIRMESH_00;
	m_dwHairColor = 0xffffffff;
	m_dwHeadMesh = 0;
	m_nFxp = 0;
	//	m_nTxp			= 0;
	m_bySex = SEX_FEMALE;
	m_dwRideItemIdx = 0;

	m_dwGold = 0;

	m_nJob = -1;

	m_nStr = m_nSta = m_nDex = m_nInt = 15;
	m_nLevel = 1;
	m_nExp1 = 0;

	m_idMarkingWorld = NULL_ID;

	m_dwAuthorization = AUTH_GENERAL;
	m_dwMode = 0;
	m_dwStateMode = 0;
	m_dwUseItemId = 0;
	m_nRemainGP = 0;

	m_aQuest = new QUEST[MAX_QUEST];
	m_aCompleteQuest = new WORD[MAX_COMPLETE_QUEST];
	m_nQuestSize = 0;
	m_nCompleteQuestSize = 0;
	memset(m_aQuest, 0, sizeof(QUEST) * MAX_QUEST);
	memset(m_aCompleteQuest, 0, sizeof(WORD) * MAX_COMPLETE_QUEST);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_aCheckedQuest = new WORD[MAX_CHECKED_QUEST];
	m_nCheckedQuestSize = 0;
	memset(m_aCheckedQuest, 0, sizeof(WORD) * MAX_CHECKED_QUEST);
#endif // __IMPROVE_QUEST_INTERFACE

	m_idparty = 0;
	m_idGuild = 0;

#ifdef __DBSERVER
	m_dwWorldID = 1;
#endif // __DBSERVER
	memset(m_szBankPass, 0, sizeof(m_szBankPass));

	m_nFuel = -1;
	m_bActiveAttack = FALSE;
	m_nMovePattern = m_nMoveEvent = m_nMoveEventCnt = 0;
	m_tmAccFuel = 0;
	m_dwPeriodicTick = 0;
	m_dwTickCheer = 0;
	m_nCheerPoint = 0;
	m_nGuildCombatState = 0;
	m_nSkillLevel = 0;
	m_nSkillPoint = 0;

#ifdef __DBSERVER
#ifdef __INVALID_LOGIN_0612
	m_bLogout = FALSE;
	*m_szAccount = '\0';
#endif	// __INVALID_LOGIN_0612
#endif	// __DBSERVER

	memset(m_abUpdateSkill, 0, sizeof(m_abUpdateSkill));

#if __VER >= 9	// __PET_0410
	m_dwPetId = NULL_ID;
#endif	// __PET_0410
#if __VER >= 9	//__AI_0509
	m_fSpeedFactor = 1.0F;
#endif	// __AI_0509

#ifdef __JEFF_9_20
	m_dwMute = 0;
#endif	// __JEFF_9_20
#if __VER >= 13
	m_nHonor = -1;
	memset(m_aHonorTitle, 0, sizeof(int) * MAX_HONOR_TITLE);
#endif
#if __VER >= 15 // __PETVIS
	m_objIdVisPet = NULL_ID;
	m_dwMoverSfxId = NULL_ID;
#endif // __PETVIS
#ifdef __FL_BATTLE_PASS
	SetLastBattlePassID(0);
	SetBattlePassLevel(0);
	SetBattlePassExp(0);
#endif
#ifdef __MODEL_HIDEFASH
	memset(m_bHideFashion, FALSE, sizeof(m_bHideFashion));
#endif
}

CMover::~CMover()
{
	SAFE_DELETE(m_pActMover);

	SAFE_DELETE_ARRAY(m_aQuest);
	SAFE_DELETE_ARRAY(m_aCompleteQuest);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	SAFE_DELETE_ARRAY(m_aCheckedQuest);
#endif // __IMPROVE_QUEST_INTERFACE
}

void CMover::InitProp(void)
{
#ifdef __DBSERVER
	MoverProp* pMvrProp = GetProp();
	if (pMvrProp)
	{
		if (FALSE == m_bPlayer)
			_tcscpy(m_szName, pMvrProp->szName);

		memset(m_aJobSkill, 0, sizeof(SKILL) * (MAX_SKILL_JOB));

		LPSKILL lpSkill;
		for (int i = 0; i < MAX_SKILL_JOB; i++)
		{
			m_aJobSkill[i].dwSkill = NULL_ID;
		}

		if (m_nJob != -1)
		{
			ItemProp** apSkillProp = prj.m_aJobSkill[m_nJob];
			int nJobNum = prj.m_aJobSkillNum[m_nJob];
			for (int i = 0; i < nJobNum; i++)
			{
				ItemProp* pSkillProp = apSkillProp[i];
				lpSkill = &m_aJobSkill[i];
				lpSkill->dwSkill = pSkillProp->dwID;
			}
		}
		m_nHitPoint = 77; //GetMaxHitPoint();
		m_nManaPoint = 77; //GetMaxManaPoint();
		m_nFatiguePoint = 77; //GetMaxFatiguePoint();
		memset(m_szBankPass, 0, sizeof(m_szBankPass));
		m_dwBelligerence = pMvrProp->dwBelligerence;
	}
#endif	// __DBSERVER
}

BOOL CMover::IsAbnormalPlayerData()
{
#ifndef __CORESERVER
	DWORD dwId;
	int nMax = m_Inventory.m_dwItemMax;
	int nNumberofIndex = m_Inventory.m_dwIndexNum;

	for (int i = 0; i < nMax; i++)
	{
		dwId = m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
		{
			if (m_Inventory.m_apItem[dwId].m_dwObjIndex != i)
				return TRUE;
		}
	}

	DWORD dwObjIndex;
	for (int i = 0; i < nMax; i++)
	{
		dwObjIndex = m_Inventory.m_apItem[i].m_dwObjIndex;
		if (dwObjIndex != NULL_ID)
		{
			if (m_Inventory.m_apIndex[dwObjIndex] != i)
				return TRUE;
		}
		if (FALSE == m_Inventory.m_apItem[i].IsEmpty())
		{
			ItemProp* pItemProp
				= prj.GetItemProp(m_Inventory.m_apItem[i].m_dwItemId);
			if (!pItemProp	/*It have not property*/
				|| pItemProp->dwItemKind3 == IK3_VIRTUAL/*It is monster's virtual weapon*/)
				return TRUE;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset(adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS);
	for (int i = nNumberofIndex; i < nMax; i++)
	{
		dwId = m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
			adwEquipment[i - nNumberofIndex] = dwId;
	}
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (adwEquipment[i] != m_aEquipInfo[i].dwId)
			return TRUE;
	}
#endif // __CORESERVER
	return FALSE;
}

BOOL CMover::NormalizePlayerData()
{
	int nMax = m_Inventory.m_dwItemMax;
	int nNumberofIndex = m_Inventory.m_dwIndexNum;
	DWORD dwId;
	for (int i = 0; i < nMax; i++)
	{
		dwId = m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
		{
			if (m_Inventory.m_apItem[dwId].m_dwObjIndex != i)
			{

				DWORD dwIndex = NULL_ID;
				for (int j = 0; j < nMax; j++)
				{
					if (m_Inventory.m_apItem[j].m_dwObjIndex == i)
					{
						dwIndex = j;
						break;
					}
				}
				if (dwIndex != NULL_ID)
				{
					m_Inventory.m_apIndex[i] = dwIndex;
				}
				else
				{
					if (i >= nNumberofIndex)
					{
						m_Inventory.m_apIndex[i] = NULL_ID;
					}
					else
					{
						int j = NULL;
						for (; j < nMax; j++)
						{
							if (m_Inventory.m_apItem[j].m_dwObjIndex == NULL_ID)
							{
								m_Inventory.m_apItem[j].m_dwObjIndex = i;
								m_Inventory.m_apIndex[i] = j;
								break;
							}
						}
						if (j == nMax)
						{
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
		dwObjIndex = m_Inventory.m_apItem[i].m_dwObjIndex;
		if (dwObjIndex != NULL_ID)
		{
			if (m_Inventory.m_apIndex[dwObjIndex] != i)
				m_Inventory.m_apItem[i].m_dwObjIndex = NULL_ID;
		}
	}

	DWORD adwEquipment[MAX_HUMAN_PARTS];
	memset(adwEquipment, 0xff, sizeof(DWORD) * MAX_HUMAN_PARTS);
	for (int i = nNumberofIndex; i < nMax; i++)
	{
		dwId = m_Inventory.m_apIndex[i];
		if (dwId != NULL_ID)
			adwEquipment[i - nNumberofIndex] = dwId;
	}
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (m_aEquipInfo[i].dwId != adwEquipment[i])
			m_aEquipInfo[i].dwId = adwEquipment[i];
	}
	return TRUE;
}
#if !defined(__CORESERVER)

BOOL CMover::RemoveItemIK3()
{
	int	nSize = m_Inventory.m_dwItemMax;
	CItemElem* pItemElem;
	BOOL bRemove;
	BOOL bResult = FALSE;
	for (int i = 0; i < nSize; i++)
	{
		bRemove = FALSE;
		pItemElem = m_Inventory.GetAtId(i);
		if (pItemElem)
		{
			ItemProp* pItemProp = prj.GetItemProp(pItemElem->m_dwItemId);
			if (pItemProp)
			{
				if (pItemProp->dwItemKind3 == IK3_DELETE)
				{
					bRemove = TRUE;
				}

				if (bRemove)
				{
					if (m_Inventory.IsEquip(pItemElem->m_dwObjId))
					{
						if (m_Inventory.UnEquip(pItemProp->dwParts))
						{
							m_aEquipInfo[pItemProp->dwParts].dwId = NULL_ID;
							m_Inventory.RemoveAt(pItemElem->m_dwObjIndex);
							bResult = TRUE;
						}
						else
						{
							WriteLog("Not UnEquip RemoveItemIK3  : %s, %d, %d", m_szName, pItemElem->m_dwObjId, pItemProp->dwID);
						}
					}
					else
					{
						m_Inventory.RemoveAt(pItemElem->m_dwObjIndex);
						bResult = TRUE;
					}
				}
			}
			else
			{
				WriteLog("Not ItemProp RemoveItemIK3  : %s, %d ", m_szName, pItemElem->m_dwItemId);
			}
		}
	}
	return bResult;
}
#endif // !__CORESERVER

#ifdef __CONV_SKILL_11_MONTH_JOB1
int CMover::InitSkillExp()
{
	int dwSkillPoint = 0;
	for (int i = MAX_JOB_SKILL; i < MAX_SKILL_JOB; ++i)
	{
		LPSKILL pSkill = &m_aJobSkill[i];
		if (pSkill != NULL)
		{
			if (pSkill->dwSkill != NULL_ID)
			{
				ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
				if (pSkillProp == NULL)
				{
					return 0;	// property not found
				}

				for (int j = 1; j < pSkill->dwLevel; ++j)
				{
					AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, j);
					if (pAddSkillProp == NULL)
					{
						return 0;	// property not found
					}
					if (prj.m_aExpSkill[j + 1] != 0)
						dwSkillPoint += prj.m_aExpSkill[j + 1] * pAddSkillProp->dwSkillExp;
					else
						dwSkillPoint += pAddSkillProp->dwSkillExp;
				}

				AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, pSkill->dwLevel);
				if (pAddSkillProp == NULL)
				{
					return 0;	// property not found
				}
				pSkill->dwLevel = 1;
			}
		}
	}
	return dwSkillPoint;
}
#endif // __CONV_SKILL_11_MONTH_JOB1

BOOL CItemElem::IsPierceAble(DWORD dwTargetItemKind3, BOOL bSize)
{
	if (!GetProp())
		return FALSE;

	int nPiercedSize = GetPiercingSize();
	if (bSize)
		nPiercedSize++;

	if (GetProp()->dwItemKind3 == IK3_SUIT)
	{
		if (nPiercedSize <= MAX_PIERCING_SUIT)
		{
			if (dwTargetItemKind3 == NULL_ID)
				return TRUE;
			else if (dwTargetItemKind3 == IK3_SOCKETCARD)
				return TRUE;
		}
	}

	else if (GetProp()->dwItemKind3 == IK3_SHIELD
		|| GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT
		|| GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC
#if __VER >= 16
		|| GetProp()->dwItemKind3 == IK3_MAGICBARUNA
		|| GetProp()->dwItemKind3 == IK3_ZEMBARUNA
#endif
		)
	{
		if (nPiercedSize <= MAX_PIERCING_WEAPON)
		{
			if (dwTargetItemKind3 == NULL_ID)
				return TRUE;
			else if (dwTargetItemKind3 == IK3_SOCKETCARD2)
				return TRUE;
		}
	}

	else if (IsVisPet())
	{
		if (nPiercedSize <= MAX_VIS)
		{
			if (dwTargetItemKind3 == NULL_ID)
				return TRUE;
			else if (dwTargetItemKind3 == IK3_VIS)
				return TRUE;
		}
	}

	return FALSE;
}


#ifdef __FL_PACKED_ITEMS
void CItemElem::ClearPackedItems()
{
	for (auto& it : m_vecPackedItems)
		SAFE_DELETE(it);

	m_vecPackedItems.clear();
}
bool CItemElem::AddToPack(CItemElem* pItem)
{
	// If this item is not a package
	if (!IsPackage())
		return false;

	// If this item have not prop
	ItemProp* pThisProp = GetProp();
	if (!pThisProp)
		return false;

	// If item which we adding into current item is package
	if (!pItem || pItem->IsPackage())
		return false;

	// If item which we adding into current item have not prop
	ItemProp* pItemProp = pItem->GetProp();
	if (!pItemProp)
		return false;

	if (m_vecPackedItems.size() >= MAX_ITEMS_IN_PACKAGE)
		return false;

	CItemElem* pItemElem = new CItemElem;
	*pItemElem = *pItem;
	m_vecPackedItems.push_back(pItemElem);

	return true;
}
bool CItemElem::DeleteFromPack(int nSel)
{
	if (!IsPackage())
		return false;

	if (nSel < 0 || nSel >= static_cast<int>(m_vecPackedItems.size()))
		return false;

	CItemElem* pItemElem = m_vecPackedItems.at(nSel);
	SAFE_DELETE(pItemElem);
	m_vecPackedItems.erase(m_vecPackedItems.begin() + nSel);

	return true;
}
#endif

void CMover::Copy(CMover* pMover, BOOL bAll)
{
	m_nSlot = pMover->m_nSlot;
	m_dwIndex = pMover->m_dwIndex;
	m_idPlayer = pMover->m_idPlayer;
	m_dwAuthorization = pMover->m_dwAuthorization;
	m_idGuild = pMover->m_idGuild;
	m_dwMode = pMover->m_dwMode;
	lstrcpy(m_szBankPass, pMover->m_szBankPass);
	m_dwPeriodicTick = pMover->m_dwPeriodicTick;
	m_RTMessenger = pMover->m_RTMessenger;
#ifdef __BUFF_1107
	m_buffs = pMover->m_buffs;
#endif	// __BUFF_1107

	if (bAll)
	{
#ifdef __FL_CONSUMABLES_STORAGE
		m_Chest.Copy(pMover->m_Chest);
#endif
		m_Inventory.Copy(pMover->m_Inventory);
		memcpy(m_aEquipInfo, pMover->m_aEquipInfo, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS);
		memcpy(m_UserTaskBar.m_aSlotApplet, pMover->m_UserTaskBar.m_aSlotApplet, sizeof(m_UserTaskBar.m_aSlotApplet));
		memcpy(m_UserTaskBar.m_aSlotItem, pMover->m_UserTaskBar.m_aSlotItem, sizeof(m_UserTaskBar.m_aSlotItem));
		memcpy(m_UserTaskBar.m_aSlotQueue, pMover->m_UserTaskBar.m_aSlotQueue, sizeof(m_UserTaskBar.m_aSlotQueue));
		m_UserTaskBar.m_nActionPoint = pMover->m_UserTaskBar.m_nActionPoint;
		lstrcpy(m_szName, pMover->m_szName);
		m_vScale = pMover->m_vScale;
		m_dwMotion = pMover->m_dwMotion;
#ifdef __DBSERVER
		m_dwWorldID = pMover->m_dwWorldID;
#endif	// __DBSERVER
#ifdef __LAYER_1015
		SetLayer(pMover->GetLayer());
#endif	// __LAYER_1015
		m_vPos = pMover->m_vPos;
		m_fAngle = pMover->m_fAngle;
		lstrcpy(m_szCharacterKey, pMover->m_szCharacterKey);
		m_nHitPoint = pMover->m_nHitPoint;
		m_nManaPoint = pMover->m_nManaPoint;
		m_nFatiguePoint = pMover->m_nFatiguePoint;
		m_dwSkinSet = pMover->m_dwSkinSet;
		m_dwHairMesh = pMover->m_dwHairMesh;
		m_dwHairColor = pMover->m_dwHairColor;
		m_dwHeadMesh = pMover->m_dwHeadMesh;
		SetSex(pMover->GetSex());
		m_dwRideItemIdx = pMover->m_dwRideItemIdx;
		SetGold(pMover->GetGold());
		m_nJob = pMover->m_nJob;
		m_idparty = pMover->m_idparty;
		m_pActMover->m_dwState = pMover->m_pActMover->GetState();
		m_pActMover->m_dwStateFlag = pMover->m_pActMover->GetStateFlag();
		m_nStr = pMover->m_nStr;
		m_nSta = pMover->m_nSta;
		m_nDex = pMover->m_nDex;
		m_nInt = pMover->m_nInt;
		m_nLevel = pMover->m_nLevel;
		m_nExp1 = pMover->m_nExp1;
		m_nFuel = pMover->m_nFuel;
		m_tmAccFuel = pMover->m_tmAccFuel;
		m_nSkillLevel = pMover->m_nSkillLevel;
		m_nSkillPoint = pMover->m_nSkillPoint;

		m_idMarkingWorld = pMover->m_idMarkingWorld;
		m_vMarkingPos = pMover->m_vMarkingPos;
		m_nRemainGP = pMover->m_nRemainGP;
		m_nFxp = pMover->m_nFxp;
		m_bPlayer = TRUE;
		memcpy(m_aJobSkill, pMover->m_aJobSkill, sizeof(m_aJobSkill));
		memcpy(m_abUpdateSkill, pMover->m_abUpdateSkill, sizeof(m_abUpdateSkill));
		memcpy(m_aQuest, pMover->m_aQuest, sizeof(QUEST) * MAX_QUEST);
		m_nQuestSize = pMover->m_nQuestSize;
		memcpy(m_aCompleteQuest, pMover->m_aCompleteQuest, sizeof(WORD) * MAX_COMPLETE_QUEST);
		m_nCompleteQuestSize = pMover->m_nCompleteQuestSize;
		memcpy(m_aCheckedQuest, pMover->m_aCheckedQuest, sizeof(WORD) * MAX_CHECKED_QUEST);
		m_nCheckedQuestSize = pMover->m_nCheckedQuestSize;
		memcpy(m_dwSMTime, pMover->m_dwSMTime, sizeof(m_dwSMTime));
		m_nPlusMaxHitPoint = pMover->m_nPlusMaxHitPoint;
#ifndef __BUFF_1107
		memcpy(m_SkillState.m_aSkillInfluence, pMover->m_SkillState.m_aSkillInfluence, sizeof(m_SkillState.m_aSkillInfluence));
#endif	// __BUFF_1107
		m_dwReturnWorldID = pMover->m_dwReturnWorldID;
		m_vReturnPos = pMover->m_vReturnPos;

		for (int i = 0; i < 3; i++)
			m_Bank[i].Copy(pMover->m_Bank[i]);
		memcpy(m_idPlayerBank, pMover->m_idPlayerBank, sizeof(m_idPlayerBank));
		memcpy(m_dwGoldBank, pMover->m_dwGoldBank, sizeof(m_dwGoldBank));

#if __VER >= 9	// __PET_0410
		SetPetId(pMover->GetPetId());
#endif	// __PET_0410
#if __VER >= 9	//__AI_0509
		m_fSpeedFactor = pMover->m_fSpeedFactor;
#endif	// __AI_0509

#ifdef __JEFF_9_20
		m_dwMute = pMover->m_dwMute;
#endif	// __JEFF_9_20
#if __VER >= 13
		m_nHonor = pMover->m_nHonor;
		memcpy(m_aHonorTitle, pMover->m_aHonorTitle, sizeof(int) * MAX_HONOR_TITLE);
#endif

#ifdef __FL_BATTLE_PASS
		SetLastBattlePassID(pMover->GetLastBattlePassID());
		SetBattlePassLevel(pMover->GetBattlePassLevel());
		SetBattlePassExp(pMover->GetBattlePassExp());
#endif

#ifdef __MODEL_HIDEFASH
		memcpy(m_bHideFashion, pMover->m_bHideFashion, sizeof(m_bHideFashion));
#endif
		m_nCheerPoint = pMover->m_nCheerPoint;

#ifdef __FL_SWITCH_EQ
		m_Switches = pMover->m_Switches;
#endif
#ifdef __FL_RECORD_BOOK
		m_arrayRecords = pMover->m_arrayRecords;
#endif
#ifdef __FL_MONSTER_ALBUM
		m_MonsterAlbum = pMover->m_MonsterAlbum;
#endif
	}
}

void CMover::PeriodTick(void)
{
	char ch = m_dwPeriodicTick == 0 ? 'O' : 'P';
	BOOL bPut = (ch != 'P');
	DWORD dwTick = GetTickCount();
	if (!bPut)
	{
		if (m_dwPeriodicTick < dwTick)
			bPut = TRUE;
	}
	if (bPut)
		m_dwPeriodicTick = dwTick + MIN(120);
}


BOOL CMover::RemoveQuest(int nQuestId)
{
	BOOL	bRemove = FALSE;
	for (int i = 0; i < m_nQuestSize; i++)
	{
		if (m_aQuest[i].m_wId == nQuestId)
		{
			for (; i < m_nQuestSize - 1; i++)
				m_aQuest[i] = m_aQuest[i + 1];
			m_nQuestSize--;
			bRemove = TRUE;
			break;
		}
	}
	for (int i = 0; i < m_nCompleteQuestSize; i++)
	{
		if (m_aCompleteQuest[i] == nQuestId)
		{
			for (; i < m_nCompleteQuestSize - 1; i++)
				m_aCompleteQuest[i] = m_aCompleteQuest[i + 1];
			m_nCompleteQuestSize--;
			bRemove = TRUE;
			break;
		}
	}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	for (int i = 0; i < m_nCheckedQuestSize; ++i)
	{
		if (m_aCheckedQuest[i] == nQuestId)
		{
			for (; i < m_nCheckedQuestSize - 1; ++i)
				m_aCheckedQuest[i] = m_aCheckedQuest[i + 1];
			m_aCheckedQuest[--m_nCheckedQuestSize] = 0;
			bRemove = TRUE;
			break;
		}
	}
#endif // __IMPROVE_QUEST_INTERFACE
	return bRemove;
}

CItemElem* CMover::GetPetItem(void)
{
	return m_Inventory.GetAtId(m_dwPetId);
}
