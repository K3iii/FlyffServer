#include "stdafx.h"
#include ".\itemupgrade.h"

#if __VER >= 12 // __EXT_PIERCING
#include "defineObj.h"
#include "defineSound.h"
#include "defineText.h"

#if __VER >= 11 // __SYS_COLLECTING
#include "collecting.h"
#include "definesound.h"
#include "defineitem.h"
#endif	// __SYS_COLLECTING

#include "User.h"
extern CUserMng g_UserMng;
#include "DPSrvr.h"
extern CDPSrvr g_DPSrvr;
#include "DPDatabaseClient.h"
extern CDPDatabaseClient g_dpDBClient;

#if __VER >= 15 // __PETVIS
#include "AIPet.h"
#endif // __PETVIS

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

CItemUpgrade::CItemUpgrade(void)
#ifdef __SYS_ITEMTRANSY
	: m_nItemTransyLowLevel(1000000), m_nItemTransyHighLevel(2000000)
#endif // __SYS_ITEMTRANSY
{
	LoadScript();
}

CItemUpgrade::~CItemUpgrade(void)
{
	m_mapSuitProb.clear();
	m_mapWeaponProb.clear();
#if __VER >= 13 // __EXT_ENCHANT
	m_mapGeneralEnchant.clear();
	m_mapAttributeEnchant.clear();
#endif // __EXT_ENCHANT
}

CItemUpgrade* CItemUpgrade::GetInstance(void)
{
	static CItemUpgrade sItemUpgrade;
	return &sItemUpgrade;
}

void CItemUpgrade::LoadScript()
{
	CLuaBase	lua;
	if (lua.RunScript("ItemUpgrade.lua") != 0)
	{
		Error("CItemUpgrade::LoadScript() - ItemUpgrade.lua Run Failed!!!");
		return;
		ASSERT(0);
	}


	lua.GetGloabal("tSuitProb");
	lua.PushNil();
	while (lua.TableLoop(-2))
	{
		m_mapSuitProb.insert(make_pair(static_cast<int>(lua.ToNumber(-2)), static_cast<int>(lua.ToNumber(-1))));
		lua.Pop(1);
	}
	lua.Pop(0);


	lua.GetGloabal("tWeaponProb");
	lua.PushNil();
	while (lua.TableLoop(-2))
	{
		m_mapWeaponProb.insert(make_pair(static_cast<int>(lua.ToNumber(-2)), static_cast<int>(lua.ToNumber(-1))));
		lua.Pop(1);
	}
	lua.Pop(0);

#if __VER >= 13 // __EXT_ENCHANT

	lua.GetGloabal("tGeneral");
	lua.PushNil();
	while (lua.TableLoop(-2))
	{
		m_mapGeneralEnchant.insert(make_pair(static_cast<int>(lua.ToNumber(-2)), static_cast<int>(lua.ToNumber(-1))));
		lua.Pop(1);
	}
	lua.Pop(0);


	lua.GetGloabal("tAttribute");
	lua.PushNil();
	while (lua.TableLoop(-2))
	{
		__ATTRIBUTE_ENCHANT attrEnchant;
		attrEnchant.nProb = static_cast<int>(lua.GetFieldToNumber(-1, "nProb"));
		attrEnchant.nAddDamageRate = static_cast<int>(lua.GetFieldToNumber(-1, "nDamageRate"));
#if __VER >= 14 // __EXT_ATTRIBUTE
		attrEnchant.nDefenseRate = static_cast<int>(lua.GetFieldToNumber(-1, "nDefenseRate"));
		attrEnchant.nAddAtkDmgRate = static_cast<int>(lua.GetFieldToNumber(-1, "nAddAtkDmgRate"));
#endif // __EXT_ATTRIBUTE
		m_mapAttributeEnchant.insert(make_pair(static_cast<int>(lua.ToNumber(-2)), attrEnchant));
		lua.Pop(1);
#if __VER < 14 // __EXT_ATTRIBUTE
		if ((::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER) && m_mapAttributeEnchant.size() == 10)
			break;
#endif // __EXT_ATTRIBUTE
	}
	lua.Pop(0);
#endif // __EXT_ENCHANT
#ifdef __SYS_ITEMTRANSY
	m_nItemTransyLowLevel = static_cast<int>(lua.GetGlobalNumber("nItemTransyLowLevel"));
	m_nItemTransyHighLevel = static_cast<int>(lua.GetGlobalNumber("nItemTransyHighLevel"));
	lua.Pop(0);
#endif // __SYS_ITEMTRANSY
}


int CItemUpgrade::GetSizeProb(CItemElem* pItemElem)
{

	if (pItemElem->IsPierceAble(IK3_SOCKETCARD))
	{
		//return m_vecSuitProb.size() >= pItemElem->GetPiercingSize() ? m_vecSuitProb[pItemElem->GetPiercingSize()] : 0;
		map<int, int>::iterator it = m_mapSuitProb.find(pItemElem->GetPiercingSize() + 1);
		if (it != m_mapSuitProb.end())
			return it->second;
	}


	if (pItemElem->IsPierceAble(IK3_SOCKETCARD2))
	{
		//return m_vecWeaponProb.size() >= pItemElem->GetPiercingSize() ? m_vecWeaponProb[pItemElem->GetPiercingSize()] : 0;
		map<int, int>::iterator it = m_mapWeaponProb.find(pItemElem->GetPiercingSize() + 1);
		if (it != m_mapWeaponProb.end())
			return it->second;
	}

	return 0;
}

void CItemUpgrade::OnPiercing(CUser* pUser, DWORD dwItemId, DWORD dwSocketCard)
{

	CItemElem* pItemElem0 = pUser->m_Inventory.GetAtId(dwItemId);
	CItemElem* pItemElem1 = pUser->m_Inventory.GetAtId(dwSocketCard);
	if (IsUsableItem(pItemElem0) == FALSE || IsUsableItem(pItemElem1) == FALSE)
		return;


	if (pUser->m_Inventory.IsEquip(dwItemId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT, "");
		return;
	}


	if (!pItemElem0->IsPierceAble())
	{
		pUser->AddDefinedText(TID_PIERCING_POSSIBLE_ITEM, "");
		return;
	}


	if (!pItemElem0->IsPierceAble(pItemElem1->GetProp()->dwItemKind3))
	{
		pUser->AddDefinedText(TID_UPGRADE_ERROR_WRONGUPLEVEL, "");
		return;
	}


	int nSize = pItemElem0->GetPiercingSize();

	int nCount = 0;
	for (int j = 0; j < nSize; j++)
	{
		if (pItemElem0->GetPiercingItem(j) != 0)
			nCount++;
	}


	if (nCount == nSize)
	{
		pUser->AddDefinedText(TID_PIERCING_ERROR_NOPIERCING, "");
		return;
	}


	if (pUser->m_vtInfo.GetOther())
		return;
	if (pUser->m_vtInfo.VendorIsVendor())
		return;

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "PIERCING";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
#endif

	if (pUser->Pierce(pItemElem0, pItemElem1->m_dwItemId))
	{
#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "$";
		g_DPSrvr.OnLogItem(aLogItem, pItemElem0, pItemElem0->m_nItemNum);
		aLogItem.Action = "!";
		g_DPSrvr.OnLogItem(aLogItem, pItemElem1, pItemElem1->m_nItemNum);
#endif

		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->RemoveItem((BYTE)(dwSocketCard), (short)1);
	}

}

void CItemUpgrade::OnPiercingRemove(CUser* pUser, DWORD objId, int nSlot)
{
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElem = pUser->m_Inventory.GetAtId(objId);
	if (!IsUsableItem(pItemElem) || !pItemElem->IsPierceAble())
		return;

	if (pUser->m_Inventory.IsEquip(objId))
		return;

#ifdef __AIO_PETS
	if (pItemElem->IsEatPet())
	{
		pUser->AddDefinedText(TID_PIERCING_POSSIBLE_ITEM);
		return;
	}
#endif

	ItemProp* pItemProp1 = pItemElem->GetProp();
	if (!pItemProp1)
		return;

	BOOL bValid = FALSE;
	int nMaxSlots = pItemProp1->dwItemKind3 == IK3_SUIT ? MAX_PIERCING_SUIT : MAX_PIERCING_WEAPON;
	if (nSlot >= 0 && nSlot < nMaxSlots)
		bValid = TRUE;

	if (!bValid)
	{
		pUser->AddDefinedText(TID_GAME_COUPLE_E11);
		return;
	}

	if (pItemElem->GetPiercingSize() == 0)
	{
		pUser->AddDefinedText(TID_GAME_REMOVE_PIERCING_ERROR);
		return;
	}

	int nPayPenya = 1000000;
	if (pUser->CheckUserGold(nPayPenya, false) == false)
	{
		pUser->AddDefinedText(TID_GAME_LACKMONEY);
		return;
	}

	if (pItemElem->GetPiercingItem(nSlot) != 0)
	{
		pUser->AddGold(-nPayPenya);
		pUser->AddDefinedText(TID_GAME_REMOVE_PIERCING_SUCCESS);
		pUser->UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_PIERCING, MAKELONG(nSlot, 0));

#ifndef __FL_FIX_USELESS_LOGS
		LogItemInfo aLogItem;
		aLogItem.Action = "$";
		aLogItem.SendName = pUser->GetName();
		aLogItem.RecvName = "PIERCING_REMOVE";
		aLogItem.WorldId = pUser->GetWorld()->GetID();
		aLogItem.Gold = pUser->GetGold() + nPayPenya;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Gold_1 = -nPayPenya;
		g_DPSrvr.OnLogItem(aLogItem, pItemElem, 1);
#endif
	}
}
#endif // 

#if __VER >= 13 // __EXT_ENCHANT
void	CItemUpgrade::OnEnchant(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial)
{
	if (!IsUsableItem(pItemMain) || !IsUsableItem(pItemMaterial))
		return;

	if (pUser->m_Inventory.IsEquip(pItemMain->m_dwObjId))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT, "");
		return;
	}

	switch (pItemMaterial->GetProp()->dwItemKind3)
	{
	case IK3_ELECARD:
		EnchantAttribute(pUser, pItemMain, pItemMaterial);
		break;

	case IK3_ENCHANT:
		EnchantGeneral(pUser, pItemMain, pItemMaterial);
		break;

	default:
		if (pItemMain->IsAccessory())
			RefineAccessory(pUser, pItemMain, pItemMaterial);

		else if (pItemMain->IsCollector())
			RefineCollector(pUser, pItemMain, pItemMaterial);
		break;
	}
}

#if __VER >= 14 // __SMELT_SAFETY
BYTE	CItemUpgrade::OnSmeltSafety(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr)
{
	switch (pItemMaterial->GetProp()->dwItemKind3)
	{
	case IK3_ENCHANT:

		return SmeltSafetyNormal(pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr);


	case IK3_PIERDICE:

		if (pItemMain->IsAccessory())
			return SmeltSafetyAccessory(pUser, pItemMain, pItemMaterial, pItemProtScr);

		else if (pItemMain->IsPierceAble(NULL_ID, TRUE))
			return SmeltSafetyPiercingSize(pUser, pItemMain, pItemMaterial, pItemProtScr);
	case IK3_ELECARD:
		return SmeltSafetyAttribute(pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr);

	default:
		break;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyNormal(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr)
{

	switch (pItemMaterial->GetProp()->dwID)
	{
	case II_GEN_MAT_ORICHALCUM01:
		return SmeltSafetyGeneral(pUser, pItemMain, pItemMaterial, pItemProtScr, pItemSmeltScr);


	case II_GEN_MAT_ORICHALCUM02:
		return prj.m_UltimateWeapon.SmeltSafetyUltimate(pUser, pItemMain, pItemMaterial, pItemProtScr);

	default:
		break;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyGeneral(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr)
{

	if (!CItemElem::IsDiceRefineryAble(pItemMain->GetProp()))
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}


	if (pItemMain->GetProp()->dwReferStat1 == WEAPON_ULTIMATE || pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT)
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}


	if (pItemMain->GetAbilityOption() >= GetMaxGeneralEnchantSize())
	{
		//pUser->AddDefinedText( TID_UPGRADE_MAXOVER );
		return 3;
	}


	int nPercent = GetGeneralEnchantProb(pItemMain->GetAbilityOption());


	if (pItemSmeltScr != NULL)
	{

		if (IsUsableItem(pItemSmeltScr) && pItemSmeltScr->GetProp()->dwID == II_SYS_SYS_SCR_SMELTING)
		{

			if (pItemMain->GetAbilityOption() < 7)
			{
				nPercent += 1000;
				pUser->RemoveItem((BYTE)(pItemSmeltScr->m_dwObjId), 1);
			}
		}

		else
			return 0;
	}

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.Action = "N";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), 1);
	pUser->RemoveItem((BYTE)(pItemProtScr->m_dwObjId), 1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::NORMAL_UPGRADES, 1, true);
#endif

	if ((int)(xRandom(10000)) > nPercent)
	{

		//pUser->AddDefinedText( TID_UPGRADE_FAIL );
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "F";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 2;
	}
	else
	{

		//pUser->AddDefinedText( TID_UPGRADE_SUCCEEFUL );
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 1;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyAccessory(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr)
{

	if (pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE)
	{
		return 0;
	}

	if (pItemProtScr->GetProp()->dwID != II_SYS_SYS_SCR_SMELPROT4)
	{
		return 0;
	}

	if (pItemMain->GetAbilityOption() >= MAX_AAO)	// 20
	{
		return 3;
	}


#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), 1);
	pUser->RemoveItem((BYTE)(pItemProtScr->m_dwObjId), 1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::ACCE_UPGRADES, 1, true);
#endif

	DWORD dwProbability = CAccessoryProperty::GetInstance()->GetProbability(pItemMain->GetAbilityOption());

	if (xRandom(10000) > dwProbability)
	{
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "L";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 2;
	}
	else
	{
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 1;
	}
	return 0;
}

BYTE	CItemUpgrade::SmeltSafetyPiercingSize(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr)
{
	if (pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE)
		return 0;

	if (pItemProtScr->m_dwItemId != II_SYS_SYS_SCR_PIEPROT)
		return 0;

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "PIERCING_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
#endif

	int nCost = 100000;

	if (0 < nCost)
	{
		if (pUser->CheckUserGold(nCost, false) == false)
		{
			pUser->AddDefinedText(TID_GAME_LACKMONEY);
			return 0;
		}

		pUser->AddGold(-(nCost));

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Gold = pUser->GetGold() + nCost;
		aLogItem.Gold2 = pUser->GetGold();
		aLogItem.Action = "!";
		_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
		aLogItem.itemNumber = nCost;
		g_DPSrvr.OnLogItem(aLogItem);
#endif

	}
	else
	{
		return 0;
	}

#ifndef __FL_FIX_USELESS_LOGS
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	aLogItem.Action = "!";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), 1);
	pUser->RemoveItem((BYTE)(pItemProtScr->m_dwObjId), 1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::PIERCING_UPGRADES, 1, true);
#endif

	int nPercent = GetSizeProb(pItemMain);

	if (nPercent < (int)(xRandom(10000)))
	{
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "!";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 2;
	}
	else
	{
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_PIERCING_SIZE, pItemMain->GetPiercingSize() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "#";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 1;
	}
	return 0;
}

void	CItemUpgrade::RefineAccessory(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial)
{

	if (pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE)
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}
	if (pItemMain->GetAbilityOption() >= MAX_AAO)	// 20
	{
		pUser->AddDefinedText(TID_GAME_ACCESSORY_MAX_AAO);
		return;
	}

#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
#endif

	DWORD dwProbability = CAccessoryProperty::GetInstance()->GetProbability(pItemMain->GetAbilityOption());

	BOOL bSmelprot = FALSE;
	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4))
	{
		bSmelprot = TRUE;
		pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4);
	}

	if (xRandom(10000) < dwProbability)
	{
		pUser->AddDefinedText(TID_UPGRADE_SUCCEEFUL);
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
	}
	else
	{
		pUser->AddDefinedText(TID_UPGRADE_FAIL);
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		if (!bSmelprot)
		{
			if (pItemMain->GetAbilityOption() >= 3)
			{
#ifndef __FL_FIX_USELESS_LOGS
				aLogItem.Action = "L";
				g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
				pUser->RemoveItem((BYTE)(pItemMain->m_dwObjId), pItemMain->m_nItemNum);
			}
		}
	}

#ifndef __FL_FIX_USELESS_LOGS
	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif
	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), 1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::ACCE_UPGRADES, 1, true);
#endif
}

void	CItemUpgrade::RefineCollector(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial)
{

	if (pItemMaterial->GetProp()->dwID != II_GEN_MAT_MOONSTONE)
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}

	CCollectingProperty* pProperty = CCollectingProperty::GetInstance();
	if (pItemMain->GetAbilityOption() >= pProperty->GetMaxCollectorLevel())
	{
		pUser->AddDefinedText(TID_GAME_MAX_COLLECTOR_LEVEL);
		return;
	}

	int nProb = pProperty->GetEnchantProbability(pItemMain->GetAbilityOption());
	if (nProb == 0)
		return;

#ifndef __FL_FIX_USELESS_LOGS
	// log
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
#endif

	DWORD dwRand = xRandom(1000);	// 0 - 999
	if ((int)(dwRand) < nProb)
	{
		pUser->AddDefinedText(TID_UPGRADE_SUCCEEFUL);
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
	}
	else
	{
		pUser->AddDefinedText(TID_UPGRADE_FAIL);
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);
		if (pUser->IsMode(TRANSPARENT_MODE) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);
		// log
	}

#ifndef __FL_FIX_USELESS_LOGS
	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), 1);
}

#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
BYTE	CItemUpgrade::SmeltSafetyAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr)
{
	ItemProp* pProp = pItemMain->GetProp();
	if (!pProp)
		return 0;

	if (!pProp->IsEleRefineryAble())
		return 0;

	if (pItemMain->m_bItemResist != SAI79::NO_PROP)
	{
		if (pItemMain->m_bItemResist != pItemMaterial->GetProp()->eItemType)
			return 0;
	}


	if (pItemMaterial->GetProp()->dwID != WhatEleCard(pItemMaterial->GetProp()->eItemType))
		return 0;


	if (pItemMain->m_nResistAbilityOption >= GetMaxAttributeEnchantSize())
		return 3;


	int nPercent = GetAttributeEnchantProb(pItemMain->m_nResistAbilityOption);


	if (pItemSmeltScr != NULL)
	{

		if (IsUsableItem(pItemSmeltScr) && pItemSmeltScr->GetProp()->dwID == II_SYS_SYS_SCR_SMELTING2)
		{

			if (pItemMain->m_nResistAbilityOption < 10)
			{
				nPercent += 1000;
				pUser->RemoveItem((BYTE)(pItemSmeltScr->m_dwObjId), 1);
			}
		}

		else
			return 0;
	}

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.Action = "N";
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM_SMELTSAFETY";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();

	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	DWORD dwValue = pItemMaterial->GetProp()->eItemType;
	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), (short)1);
	pUser->RemoveItem((BYTE)(pItemProtScr->m_dwObjId), 1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::ELEMENT_UPGRADES, 1, true);
#endif

	if ((int)(xRandom(10000)) > nPercent)
	{

		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);

		if (!pUser->IsMode(TRANSPARENT_MODE))
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "J";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif

		return 2;
	}
	else
	{

		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if (!pUser->IsMode(TRANSPARENT_MODE))
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_IR, dwValue);
		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_RAO, pItemMain->m_nResistAbilityOption + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "O";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
		return 1;
	}
	return 0;
}
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
#endif // __SMELT_SAFETY

void	CItemUpgrade::EnchantGeneral(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial)
{
	int* pAbilityOption = pItemMain->GetAbilityOptionPtr();
	if (pAbilityOption == NULL)
		return;

	if (pItemMain->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}

	if (pItemMaterial->GetProp()->dwID != II_GEN_MAT_ORICHALCUM01)
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}

	if (!CItemElem::IsDiceRefineryAble(pItemMain->GetProp()))
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}

	if (*pAbilityOption >= GetMaxGeneralEnchantSize())
	{
		pUser->AddDefinedText(TID_UPGRADE_MAXOVER);
		return;
	}


	int nPercent = GetGeneralEnchantProb(*pAbilityOption);

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
#endif

	BOOL bSmelprot = FALSE;
	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT))
	{
		bSmelprot = TRUE;
		pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT);
	}

	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELTING))
	{
		if (*pAbilityOption < 7)
		{
			nPercent += 1000;
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELTING);
		}
	}

	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING))
	{
		if (*pAbilityOption < 7)
		{
			nPercent += 10000;
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING);
		}
	}


	if ((int)(xRandom(10000)) > nPercent)
	{

		pUser->AddDefinedText(TID_UPGRADE_FAIL);
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);


		if (*pAbilityOption >= 3)
		{
			if (!bSmelprot)
			{
#ifndef __FL_FIX_USELESS_LOGS
				aLogItem.Action = "L";
				g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
				pUser->RemoveItem((BYTE)(pItemMain->m_dwObjId), (short)1);
			}
			else
			{
#ifndef __FL_FIX_USELESS_LOGS
				aLogItem.Action = "F";
				g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
			}
		}
	}
	else
	{

		pUser->AddDefinedText(TID_UPGRADE_SUCCEEFUL);
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "H";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
	}

#ifndef __FL_FIX_USELESS_LOGS
	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), (short)1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::NORMAL_UPGRADES, 1, true);
#endif
}

int		CItemUpgrade::GetGeneralEnchantProb(int nAbilityOption)
{
	int nProb = 0;

	map<int, int>::iterator it = m_mapGeneralEnchant.find(nAbilityOption + 1);
	if (it != m_mapGeneralEnchant.end())
		nProb = it->second;

	return nProb;
}

void	CItemUpgrade::EnchantAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial)
{
	int* pAbilityOption = &(pItemMain->m_nResistAbilityOption);
	if (pAbilityOption == NULL)
		return;

	if (pItemMain->m_bItemResist != SAI79::NO_PROP)
	{
		if (pItemMain->m_bItemResist != pItemMaterial->GetProp()->eItemType)
		{
			pUser->AddDefinedText(TID_UPGRADE_ERROR_TWOELEMENT);
			return;
		}
	}

	ItemProp* pProp = pItemMain->GetProp();
	if (!pProp)
		return;

	if (!pProp->IsEleRefineryAble())
	{
		pUser->AddDefinedText(TID_GAME_NOTEQUALITEM);
		return;
	}

	DWORD dwReqCard = WhatEleCard(pItemMaterial->GetProp()->eItemType);
	if (pItemMaterial->GetProp()->dwID != dwReqCard)
	{
		pUser->AddDefinedText(TID_UPGRADE_ERROR_WRONGUPLEVEL);
		return;
	}


	if (*pAbilityOption >= GetMaxAttributeEnchantSize())
	{
		pUser->AddDefinedText(TID_UPGRADE_MAXOVER);
		return;
	}

	int nPercent = GetAttributeEnchantProb(*pAbilityOption);

#ifndef __FL_FIX_USELESS_LOGS
	LogItemInfo aLogItem;
	aLogItem.SendName = pUser->GetName();
	aLogItem.RecvName = "UPGRADEITEM";
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
#endif

	BOOL bSmelprot = FALSE;
	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT))
	{
		bSmelprot = TRUE;
		pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT);
	}

	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELTING2))
	{
		if (*pAbilityOption < 10)
		{
			nPercent += 1000;
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELTING2);
		}
	}

	if (pUser->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING))
	{
		if (*pAbilityOption < 7)
		{
			nPercent += 10000;
			pUser->RemoveBuff(BUFF_ITEM, II_SYS_SYS_SCR_SUPERSMELTING);
		}
	}


	if ((int)(xRandom(10000)) > nPercent)
	{

		pUser->AddDefinedText(TID_UPGRADE_FAIL);
		pUser->AddPlaySound(SND_INF_UPGRADEFAIL);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_FAIL, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);


		if (*pAbilityOption >= 3)
		{
			if (!bSmelprot)
			{
#ifndef __FL_FIX_USELESS_LOGS
				aLogItem.Action = "L";
				g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
				pUser->RemoveItem((BYTE)(pItemMain->m_dwObjId), (short)1);
			}
		}
		else
		{
#ifndef __FL_FIX_USELESS_LOGS
			aLogItem.Action = "J";
			g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
		}
	}
	else
	{

		pUser->AddDefinedText(TID_UPGRADE_SUCCEEFUL);
		pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);

		if ((pUser->IsMode(TRANSPARENT_MODE)) == 0)
			g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);

		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_IR, pItemMaterial->GetProp()->eItemType);
		pUser->UpdateItem((BYTE)pItemMain->m_dwObjId, UI_RAO, pItemMain->m_nResistAbilityOption + 1);

#ifndef __FL_FIX_USELESS_LOGS
		aLogItem.Action = "O";
		g_DPSrvr.OnLogItem(aLogItem, pItemMain, pItemMain->m_nItemNum);
#endif
	}

#ifndef __FL_FIX_USELESS_LOGS
	aLogItem.Action = "N";
	g_DPSrvr.OnLogItem(aLogItem, pItemMaterial, pItemMaterial->m_nItemNum);
#endif

	pUser->RemoveItem((BYTE)(pItemMaterial->m_dwObjId), (short)1);

#ifdef __FL_RECORD_BOOK
	CRecordBook::GetInstance()->AddPlayerRecord(pUser, RecordType::ELEMENT_UPGRADES, 1, true);
#endif
}

int CItemUpgrade::GetAttributeEnchantProb(int nAbilityOption)
{
	map<int, __ATTRIBUTE_ENCHANT>::iterator it = m_mapAttributeEnchant.find(nAbilityOption + 1);
	if (it != m_mapAttributeEnchant.end())
		return it->second.nProb;

	return 0;
}

int CItemUpgrade::GetAttributeDamageFactor(int nAbilityOption)
{
	if (nAbilityOption > GetMaxAttributeEnchantSize())
		nAbilityOption = GetMaxAttributeEnchantSize();

	map<int, __ATTRIBUTE_ENCHANT>::iterator it = m_mapAttributeEnchant.find(nAbilityOption);
	if (it != m_mapAttributeEnchant.end())
		return it->second.nAddDamageRate;

	return 0;
}

#if __VER >= 14 // __EXT_ATTRIBUTE
int CItemUpgrade::GetAttributeDefenseFactor(int nAbilityOption)
{
	if (nAbilityOption > GetMaxAttributeEnchantSize())
		nAbilityOption = GetMaxAttributeEnchantSize();

	map<int, __ATTRIBUTE_ENCHANT>::iterator it = m_mapAttributeEnchant.find(nAbilityOption);
	if (it != m_mapAttributeEnchant.end())
		return it->second.nDefenseRate;

	return 0;
}

int CItemUpgrade::GetAttributeAddAtkDmgFactor(int nAbilityOption)
{
	if (nAbilityOption > GetMaxAttributeEnchantSize())
		nAbilityOption = GetMaxAttributeEnchantSize();

	map<int, __ATTRIBUTE_ENCHANT>::iterator it = m_mapAttributeEnchant.find(nAbilityOption);
	if (it != m_mapAttributeEnchant.end())
		return it->second.nAddAtkDmgRate;

	return 0;
}
#endif // __EXT_ATTRIBUTE

DWORD CItemUpgrade::WhatEleCard(DWORD dwItemType)
{

	switch (dwItemType)
	{
	case SAI79::FIRE:
		return II_GEN_MAT_ELE_FLAME;
	case SAI79::WATER:
		return II_GEN_MAT_ELE_RIVER;
	case SAI79::ELECTRICITY:
		return II_GEN_MAT_ELE_GENERATOR;
	case SAI79::EARTH:
		return II_GEN_MAT_ELE_DESERT;
	case SAI79::WIND:
		return II_GEN_MAT_ELE_CYCLON;
	default:
		return 0;
	}
}
#endif // __EXT_ENCHANT

#ifdef __SYS_ITEMTRANSY
void CItemUpgrade::OnItemTransy(CUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash)
{
	CItemElem* pItemElemTarget = pUser->m_Inventory.GetAtId(objidTarget);
	if (!IsUsableItem(pItemElemTarget))
		return;

	if (pUser->m_Inventory.IsEquip(objidTarget))
	{
		pUser->AddDefinedText(TID_GAME_EQUIPPUT, "");
		return;
	}

#ifdef __MODEL_CHANGE
	if (pItemElemTarget->IsChangedLook())
	{
		pUser->AddDefinedText(TID_MMI_LOOKCHANGE01);
		return;
	}
#endif

	if (bCash)
	{
		CItemElem* pItemElemTransy = pUser->m_Inventory.GetAtId(objidTransy);
		if (!IsUsableItem(pItemElemTransy))
			return;


		if (pItemElemTransy->GetProp()->dwID != II_CHR_SYS_SCR_ITEMTRANSY_A && pItemElemTransy->GetProp()->dwID != II_CHR_SYS_SCR_ITEMTRANSY_B)
			return;


		if (pItemElemTransy->GetProp()->dwID == II_CHR_SYS_SCR_ITEMTRANSY_A)
		{
			if (pItemElemTarget->GetProp()->dwLimitLevel1 > 60)
				return;
		}
		else
		{
			if (pItemElemTarget->GetProp()->dwLimitLevel1 < 61)
				return;
		}

		if (RunItemTransy(pUser, pItemElemTarget, dwChangeId))
		{
			pUser->RemoveItem((BYTE)(objidTransy), (short)1);
		}
	}
}

BOOL CItemUpgrade::RunItemTransy(CUser* pUser, CItemElem* pItemElemTarget, DWORD dwChangeId)
{
	ItemProp* pItemProp = pItemElemTarget->GetProp();
	ItemProp* pItemPropChange = prj.GetItemProp(dwChangeId);


	if (!pItemProp || !pItemPropChange || pItemProp->dwID == pItemPropChange->dwID
		|| (pItemProp->dwItemKind2 != IK2_ARMOR && pItemProp->dwItemKind2 != IK2_ARMORETC)
		|| (pItemProp->dwItemSex != SEX_MALE && pItemProp->dwItemSex != SEX_FEMALE)
		|| (pItemPropChange->dwItemKind2 != IK2_ARMOR && pItemPropChange->dwItemKind2 != IK2_ARMORETC)
		|| (pItemPropChange->dwItemSex != SEX_MALE && pItemPropChange->dwItemSex != SEX_FEMALE)
		|| pItemProp->dwItemSex == pItemPropChange->dwItemSex
		|| pItemProp->dwItemKind1 != pItemPropChange->dwItemKind1 || pItemProp->dwItemKind2 != pItemPropChange->dwItemKind2
		|| pItemProp->dwItemKind3 != pItemPropChange->dwItemKind3 || pItemProp->dwItemJob != pItemPropChange->dwItemJob
		|| pItemProp->dwHanded != pItemPropChange->dwHanded || pItemProp->dwParts != pItemPropChange->dwParts
		|| pItemProp->dwItemLV != pItemPropChange->dwItemLV || pItemProp->dwAbilityMin != pItemPropChange->dwAbilityMin
		|| pItemProp->dwAbilityMax != pItemPropChange->dwAbilityMax || pItemProp->fAttackSpeed != pItemPropChange->fAttackSpeed
		)
		return FALSE;



	pUser->AddPlaySound(SND_INF_UPGRADESUCCESS);
	g_UserMng.AddCreateSfxObj((CMover*)pUser, XI_INT_SUCCESS, pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z);


	CItemElem ItemElemSend;
	ItemElemSend = *pItemElemTarget;
	ItemElemSend.m_dwItemId = pItemPropChange->dwID;
	ItemElemSend.m_nHitPoint = pItemPropChange->dwEndurance;

	pUser->AddDefinedText(TID_GAME_ITEM_TRANSY_SUCCESS, "\"%s\" \"%s\"", pItemElemTarget->GetProp()->szName, ItemElemSend.GetProp()->szName);
	pUser->RemoveItem((BYTE)(pItemElemTarget->m_dwObjId), (short)1);
	pUser->CreateItem(&ItemElemSend);

	return TRUE;
}
#endif // __SYS_ITEMTRANSY

#if __VER >= 15 // __PETVIS
void CItemUpgrade::PetVisSize(CUser* pUser, OBJID objIdMaterial, int nType)
{
	if (!IsValidObj(pUser))
		return;

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemMaterial = pContainer->GetAtId(objIdMaterial);
	if (!IsUsableItem(pItemElemMaterial))
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	if (!IsUsableItem(pItemElemPet) || !pItemElemPet->IsVisPet())
	{
		pUser->AddDefinedText(TID_GAME_BUFFPET_NOSUMMON02);
		return;
	}

	if (pItemElemMaterial->m_dwItemId != II_SYS_SYS_VIS_KEY01)
		return;

	if (!pItemElemPet->IsPierceAble(NULL_ID, TRUE))
	{
		pUser->AddDefinedText(TID_GAME_BUFFPET_EXPANSION);
		return;
	}

#ifndef __FL_FIX_USELESS_LOGS
	g_DPSrvr.PutItemLog(pUser, "!", "VIS_SLOT_MATERIAL", pItemElemMaterial);
#endif

	pUser->UpdateItem((BYTE)(pItemElemPet->m_dwObjId), UI_PETVIS_SIZE, pItemElemPet->GetPiercingSize() + 1);

#ifndef __FL_FIX_USELESS_LOGS
	g_DPSrvr.PutItemLog(pUser, "#", "VIS_SLOT_SIZE", pItemElemPet);
#endif

	pUser->RemoveItem((BYTE)(objIdMaterial), 1, nType);
}

void CItemUpgrade::SetPetVisItem(CUser* pUser, OBJID objIdVis, int nType)
{
	if (!IsValidObj(pUser))
		return;

	CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
	if (!pContainer)
		return;

	CItemElem* pItemElemVis = pContainer->GetAtId(objIdVis);
	if (!IsUsableItem(pItemElemVis))
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	if (!IsUsableItem(pItemElemPet))
	{
		pUser->AddDefinedText(TID_GAME_BUFFPET_NOSUMMON01);
		return;
	}

	ItemProp* pVisProp = prj.GetItemProp(pItemElemVis->m_dwItemId);
	if (!pVisProp)
		return;

	if (!pItemElemPet->IsPierceAble(pVisProp->dwItemKind3))
		return;

	int nFirstEmptySlot = NULL_ID;
	time_t t = 0;

	for (int i = 0; i < pItemElemPet->GetPiercingSize(); i++)
	{
		DWORD dwVisId = pItemElemPet->GetPiercingItem(i);
		if (dwVisId == pVisProp->dwID)
		{
			t = pItemElemPet->GetVisKeepTime(i);
			nFirstEmptySlot = i;
			break;
		}

		if (nFirstEmptySlot == NULL_ID && dwVisId == 0)
			nFirstEmptySlot = i;
	}

	if (nFirstEmptySlot == NULL_ID)
	{
		pUser->AddDefinedText(TID_GAME_BUFFPET_LACKSLOT);
		return;
	}

	if (pUser->IsSatisfyNeedVis(pItemElemPet, pVisProp) != SUCCSESS_NEEDVIS)
	{
		pUser->AddDefinedText(TID_GAME_BUFFPET_REQVIS);
		return;
	}

	BOOL bNewBead = TRUE;
	if (t != 0)
	{
		CTimeSpan	ts((t + pVisProp->dwAbilityMin) - time(NULL));
		if (ts.GetDays() >= 30)
		{
			pUser->AddText("Maximal possible time: 30 days.", 0xffffffff);
			return;
		}
		else
			bNewBead = FALSE;
	}

	pUser->ResetPetVisDST(pItemElemPet);
	if (bNewBead)
	{
		pUser->UpdateItem((BYTE)(pItemElemPet->m_dwObjId), UI_PETVIS_ITEM, MAKELONG(nFirstEmptySlot, pItemElemVis->m_dwItemId), pVisProp->dwAbilityMin);
		pUser->AddText("Bead added!", 0xffffffff);
	}
	else
	{
		pUser->UpdateItem((BYTE)(pItemElemPet->m_dwObjId), UI_STACKBEAD, MAKELONG(nFirstEmptySlot, pItemElemVis->m_dwItemId), pVisProp->dwAbilityMin * 60 + t);
		pUser->AddText("Bead time added!", 0xffffffff);
	}

	pUser->RemoveItem((BYTE)(objIdVis), 1, nType);

	pUser->SetPetVisDST(pItemElemPet);
	ChangeVisPetSfx(pUser, pItemElemPet);
}

void CItemUpgrade::RemovePetVisItem(CUser* pUser, int nPosition, BOOL bExpired)
{
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	if (!IsUsableItem(pItemElemPet))
		return;

#ifdef __AIO_PETS
	if (nPosition < 0 || nPosition >= pItemElemPet->GetPiercingSize())
		return;
#endif

	DWORD dwItemId = pItemElemPet->GetPiercingItem(nPosition);
	if (dwItemId == 0)
		return;

	pUser->ResetPetVisDST(pItemElemPet);

#ifndef __FL_FIX_USELESS_LOGS
	if (bExpired)
		PutPetVisItemLog(pUser, "$", "VIS_REMOVE_EXPIRED", pItemElemPet, nPosition);
	else
		PutPetVisItemLog(pUser, "$", "VIS_REMOVE_BYUSER", pItemElemPet, nPosition);
#endif

	pUser->UpdateItem((BYTE)(pItemElemPet->m_dwObjId), UI_PETVIS_ITEM, MAKELONG(nPosition, 0), 0);
	ItemProp* pItemProp = prj.GetItemProp(dwItemId);
	if (pItemProp)
		pUser->AddDefinedText(TID_GAME_BUFFPET_REMOVEVIS, "\"%s\"", pItemProp->szName);
	pUser->SetPetVisDST(pItemElemPet);
	ChangeVisPetSfx(pUser, pItemElemPet);
}

#ifndef __FL_FIX_USELESS_LOGS
void CItemUpgrade::PutPetVisItemLog(CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nPosition)
{
	LogItemInfo	log;
	log.Action = szAction;
	log.SendName = pUser->GetName();
	log.RecvName = szContext;
	log.WorldId = pUser->GetWorld() ? pUser->GetWorld()->GetID() : WI_WORLD_NONE;
	log.Gold = pUser->GetGold();
	log.Gold2 = pItem->GetVisKeepTime(nPosition) - time_null();
	g_DPSrvr.OnLogItem(log, pItem, pItem->m_nItemNum);
}
#endif

void CItemUpgrade::SwapVis(CUser* pUser, int nPos1, int nPos2)
{
	if (!IsValidObj(pUser))
		return;

	CItemElem* pItemElemPet = pUser->GetVisPetItem();
	if (!IsUsableItem(pItemElemPet))
		return;

	pUser->UpdateItem((BYTE)(pItemElemPet->m_dwObjId), UI_PETVIS_ITEMSWAP, MAKELONG(nPos1, nPos2));
}


void CItemUpgrade::ChangeVisPetSfx(CUser* pUser, CItemElem* pItemElemPet)
{
	CMover* pVisPet = prj.GetMover(pUser->GetEatPetId());
	if (IsValidObj(pUser) && IsValidObj(pVisPet))
	{
		DWORD dwSfxId = pItemElemPet->GetVisPetSfxId();
		if (pVisPet->m_dwMoverSfxId != dwSfxId)
		{
			pVisPet->m_dwMoverSfxId = dwSfxId;
			g_UserMng.AddChangeMoverSfxId(pVisPet);
		}
	}
}

void CItemUpgrade::TransFormVisPet(CUser* pUser, OBJID objIdMaterial, int nType)
{
	CMover* pEatPet = prj.GetMover(pUser->GetEatPetId());
	if (IsValidObj(pEatPet))
	{
		CAIPet* pAI = static_cast<CAIPet*>(pEatPet->m_pAIInterface);
		if (pAI)
		{
			CItemContainer<CItemElem>* pContainer = pUser->GetContainerByType(nType);
			if (!pContainer)
				return;

			CItemElem* pItemEatPet = pUser->m_Inventory.GetAtId(pAI->GetPetItemId());
			CItemElem* pItemMaterial = pContainer->GetAtId(objIdMaterial);
			if (IsUsableItem(pItemEatPet) && IsUsableItem(pItemMaterial))
			{
				if (pItemEatPet->IsVisPet())
				{
					pUser->AddDefinedText(TID_GAME_PET_TRAN_FAILURE);
					return;
				}

				pUser->RemoveItem((BYTE)(objIdMaterial), 1, nType);
				pUser->UpdateItem((BYTE)(pItemEatPet->m_dwObjId), UI_TRANSFORM_VISPET, TRUE);

#ifndef __FL_FIX_USELESS_LOGS
				g_DPSrvr.PutItemLog(pUser, "!", "TRANSFORM_VISPET", pItemEatPet);
#endif
				pUser->AddDefinedText(TID_GAME_PET_TRAN_SUCCESS, "\"%s\"", pItemEatPet->GetProp()->szName);
			}
		}
	}
	else
	{
		pUser->AddDefinedText(TID_GAME_PET_TRAN_FAILURE);
	}
}
#endif // __PETVIS