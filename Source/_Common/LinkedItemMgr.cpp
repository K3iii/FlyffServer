#include "stdafx.h"
#include "LinkedItemMgr.h"
#include "CmnHdr.h"
#ifdef __WORLDSERVER
#include <Network/DPCoreClient.h>
extern CDPCoreClient g_dpCoreClient;
#endif

#ifdef __ITEM_LINK
void CLinkedItemMgr::Serialize(CAr& ar, int& nIndex
#ifdef __CLIENT
	, int nLinkId
#endif
)
{
	if (ar.IsLoading())
	{
#ifdef __CLIENT
		char szSerialize[512];
		ar.ReadString(szSerialize, 512);
		if (SerializeFromWorld(szSerialize, nLinkId))
			nIndex = nLinkId;
		else
			nIndex = m_mapLinkedItems.size() - 1;
#endif
	}
	else
	{
		Error("CLinkedItemMgr::Serialize - Somehow Storing was called");
	}
}
#ifdef __CLIENT
bool CLinkedItemMgr::SerializeFromWorld(char* pString, int nLinkId)
{
	if (GetLinkedItem(nLinkId))
		return false;

	CItemElem tmpItem;
	int nCount = 0;
	int* pCount = &nCount;

	// Item ID
	tmpItem.m_dwItemId = GetIntFromStr(pString, pCount);

	// Upgrade Level
	tmpItem.SetAbilityOption(GetIntFromStr(pString, pCount));

	// Element
	tmpItem.m_bItemResist = GetIntFromStr(pString, pCount);
	tmpItem.m_nResistAbilityOption = GetIntFromStr(pString, pCount);

	// Vis Pet?
	tmpItem.m_bTranformVisPet = GetIntFromStr(pString, pCount);

	// Ultimate Piercing
	ItemProp* pProp = GetItemProp(tmpItem.m_dwItemId);
	if (pProp && pProp->IsUltimate() && pProp->IsWeapon())
	{
		tmpItem.SetUltimatePiercingSize(GetIntFromStr(pString, pCount));
		for (int i = 0; i < tmpItem.GetUltimatePiercingSize(); i++)
			tmpItem.SetUltimatePiercingItem(i, GetIntFromStr(pString, pCount));
	}

	// Regular Piercing
	int nPiercingSize = GetIntFromStr(pString, pCount);
	tmpItem.SetPiercingSize(nPiercingSize, 20);
	for (int i = 0; i < nPiercingSize; i++)
		tmpItem.SetPiercingItem(i, GetIntFromStr(pString, pCount));

	if (tmpItem.m_bTranformVisPet)
	{
		tmpItem.SetPiercingSize(nPiercingSize, 20);
		tmpItem.SetVisKeepTimeSize(nPiercingSize);
		for (int i = 0; i < nPiercingSize; i++)
			tmpItem.SetVisKeepTime(i, (time_t)GetIntFromStr(pString, pCount));
	}

	// Awake
	tmpItem.SetRandomOptItemId(GetInt64PaFromStr(pString, pCount));

	// Modelchange
#ifdef __SYS_MODELCHANGE
	tmpItem.SetLook(GetIntFromStr(pString, pCount));
#endif

	// Random Stats
	for (int i = 0; i < 3; i++)
	{
		DWORD dst = GetIntFromStr(pString, pCount);
		int adj = GetIntFromStr(pString, pCount);
		tmpItem.SetDestParam(i, dst, adj);
	}

	// Rune
#ifdef __SYS_RUNEFORGING
	tmpItem.m_dwRuneID = GetIntFromStr(pString, pCount);
	tmpItem.m_dwRuneLevel = GetIntFromStr(pString, pCount);
#endif

	LinkedItem li;
	li.item = tmpItem;
	li.timeCreated = GetTickCount();

	((CItemBase)tmpItem).SetTexture();

	m_mapLinkedItems.insert(std::make_pair(nLinkId, li));
	return true;
}
#endif

#ifdef __WORLDSERVER
void CLinkedItemMgr::AddItem(CItemElem* pItemElem, int& nIndex)
{
	if (!pItemElem)
		return;

	LinkedItem li;
	li.item = *pItemElem;
	li.timeCreated = GetTickCount();

	m_mapLinkedItems.insert(std::make_pair(++m_nCount, li));
	nIndex = m_nCount;
}
void CLinkedItemMgr::SerializeToClient(CItemElem* item, char* pszTarget)
{
	if (!item)
		return;

	char szSerialize[512] = { 0, };
	char szTemp[64] = { 0, };

	// ID, AbilityOption, ItemResist, ResistOption
	sprintf(szSerialize, "%d,%d,%d,%d,%d,",
		item->m_dwItemId,
		item->GetAbilityOption(),
		item->GetItemResist(),
		item->GetResistAbilityOption(),
		item->IsTransformVisPet()
	);

	// Ultimate Piercing
	ItemProp* pProp = GetItemProp(item->m_dwItemId);
	if (pProp && pProp->IsUltimate() && pProp->IsWeapon())
	{
		sprintf(szTemp, "%d,", item->GetUltimatePiercingSize());
		strncat(szSerialize, szTemp, sizeof(szTemp));

		for (int i = 0; i < item->GetUltimatePiercingSize(); i++)
		{
			sprintf(szTemp, "%d,", item->GetUltimatePiercingItem(i));
			strncat(szSerialize, szTemp, sizeof(szTemp));
		}
	}

	// General Piercing
	sprintf(szTemp, "%d,", item->GetPiercingSize());
	strncat(szSerialize, szTemp, sizeof(szTemp));

	for (int i = 0; i < item->GetPiercingSize(); i++)
	{
		sprintf(szTemp, "%d,", item->GetPiercingItem(i));
		strncat(szSerialize, szTemp, sizeof(szTemp));
	}

	if (item->IsVisPet())
		for (int i = 0; i < item->GetPiercingSize(); i++)
		{
			sprintf(szTemp, "%d,", item->GetVisKeepTime(i));
			strncat(szSerialize, szTemp, sizeof(szTemp));
		}

	// Awake x64
	sprintf(szTemp, "%I64d,", item->GetRandomOptItemId());
	strncat(szSerialize, szTemp, sizeof(szTemp));

	// Model
	sprintf(szTemp, "%d,", item->GetLook());
	strncat(szSerialize, szTemp, sizeof(szTemp));

	// Random Stats
	for (int i = 0; i < 3; i++)
	{
		sprintf(szTemp, "%d,%d,", item->GetDestParam(i), item->GetAdjParam(i));
		strncat(szSerialize, szTemp, sizeof(szTemp));
	}

	// Rune
#ifdef __SYS_RUNEFORGING
	sprintf(szTemp, "%d,%d", item->GetRune(), item->GetRuneLevel());
#endif
	strncat(szSerialize, szTemp, sizeof(szTemp));

	sprintf(pszTarget, szSerialize, sizeof(szSerialize));
}
#endif
void CLinkedItemMgr::ProcessTimestamps()
{
	if (GetTickCount() < (DWORD)(m_LastProcessTick + PROCESS_TICK))
		return;

	auto now = GetTickCount();

	for (auto it = m_mapLinkedItems.begin(); it != m_mapLinkedItems.end(); it++)
	{
		if ((DWORD)(it->second.timeCreated + LINK_KEEPTIME) < now)
			it = m_mapLinkedItems.erase(it);
	}
}

CItemElem* CLinkedItemMgr::GetLinkedItem(int nIndex)
{
	ProcessTimestamps();

	auto finditem = m_mapLinkedItems.find(nIndex);
	if (finditem != m_mapLinkedItems.end())
		return &(finditem->second.item);

	return nullptr;
}
#endif