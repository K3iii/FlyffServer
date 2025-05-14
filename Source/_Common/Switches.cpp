#include "StdAfx.h"
#include "Switches.h"
#ifdef __CLIENT
#include "ResData.h"
#endif
#ifdef __WORLDSERVER
#include "User.h"
#endif

#ifdef __FL_SWITCH_EQ
CSwitches::CSwitches(CMover* pMover)
	: m_pMover(pMover)
{

}
CSwitches::~CSwitches()
{
	m_vecSwitches.clear();
}
void CSwitches::Serialize(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_vecSwitches.size();

		for (auto& it : m_vecSwitches)
		{
			ar.WriteString(it.szSwitchName);

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
				ar << it.dwItem[i];

			ar << it.dwLooterPet;
			ar << it.dwPet;
			ar << it.dwRegen;
		}
	}
	else
	{
		m_vecSwitches.clear();

		size_t nSize;
		ar >> nSize;

		for (size_t n = 0; n < nSize; n++)
		{
			EQ_SWITCH tmp;

			ar.ReadString(tmp.szSwitchName, 32);

			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
				ar >> tmp.dwItem[i];

			ar >> tmp.dwLooterPet;
			ar >> tmp.dwPet;
			ar >> tmp.dwRegen;

			m_vecSwitches.push_back(tmp);
		}
	}
}
BOOL CSwitches::AddSwitch(int nSelected, LPCTSTR szText, DWORD dwLooter, DWORD dwPet, DWORD dwRegen, BOOL bAdd)
{
	EQ_SWITCH* pSwitch = NULL;

	if (nSelected == -1)
	{
		if (GetTotalSwitches() >= MAX_SWITCHES)
			return FALSE;

		if (IsInvalidGameName(szText))
			return FALSE;
	}
	else
	{
		if (nSelected < 0 || nSelected >= GetTotalSwitches())
			return FALSE;

		pSwitch = &GetSwitches()->at(nSelected);
		if (!pSwitch)
			return FALSE;
	}

	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;

	if (dwLooter != NULL_ID)
	{
		pItemElem = m_pMover->m_Inventory.GetAtId(dwLooter);
		if (pItemElem)
		{
			pItemProp = pItemElem->GetProp();
			if (!pItemProp || pItemProp->dwItemKind3 != IK3_PET)
				dwLooter = NULL_ID;
		}
		else
			dwLooter = NULL_ID;
	}

	if (dwPet != NULL_ID)
	{
		pItemElem = m_pMover->m_Inventory.GetAtId(dwPet);
		if (pItemElem)
		{
			pItemProp = pItemElem->GetProp();
			if (!pItemProp || pItemProp->dwItemKind3 != IK3_EGG)
				dwPet = NULL_ID;
		}
		else
			dwPet = NULL_ID;
	}

	if (dwRegen != NULL_ID)
	{
		pItemElem = m_pMover->m_Inventory.GetAtId(dwRegen);
		if (pItemElem)
		{
			pItemProp = pItemElem->GetProp();
			if (!pItemProp || (pItemProp->dwItemKind2 != IK2_FOOD
				&& pItemProp->dwItemKind2 != IK2_POTION
				&& pItemProp->dwItemKind2 != IK2_REFRESHER))
				dwRegen = NULL_ID;
		}
		else
			dwRegen = NULL_ID;
	}

	if (bAdd)
	{
		if (!pSwitch)
		{
			EQ_SWITCH tmp;
			sprintf(tmp.szSwitchName, "%s", szText);
			tmp.dwLooterPet = dwLooter;
			tmp.dwPet = dwPet;
			tmp.dwRegen = dwRegen;

#ifndef __DBSERVER
			CItemElem* pItemElem = NULL;
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				pItemElem = m_pMover->GetEquipItem(i);
				if (!pItemElem)
					continue;

				tmp.dwItem[i] = pItemElem->m_dwObjId;
			}
#endif
			m_vecSwitches.push_back(tmp);
		}
		else
		{
			pSwitch->dwLooterPet = dwLooter;
			pSwitch->dwPet = dwPet;
			pSwitch->dwRegen = dwRegen;

#ifndef __DBSERVER
			CItemElem* pItemElem = NULL;
			for (int i = 0; i < MAX_HUMAN_PARTS; i++)
			{
				pItemElem = m_pMover->GetEquipItem(i);
				if (!pItemElem)
				{
					pSwitch->dwItem[i] = NULL_ID;
					continue;
				}

				pSwitch->dwItem[i] = pItemElem->m_dwObjId;
			}
#endif
		}
	}

	return TRUE;
}
BOOL CSwitches::DeleteSwitch(int nSel)
{
	if (nSel < 0 || nSel >= GetTotalSwitches())
		return FALSE;

	m_vecSwitches.erase(m_vecSwitches.begin() + nSel);
	return TRUE;
}
#ifdef __WORLDSERVER
int CSwitches::GetRequiredEmptySpace(EQ_SWITCH* pSwitch)
{
	if (!pSwitch)
		return 1;

	int nSpaceNeeded = 1;

	CItemElem* pItemElem = NULL;
	for (int i = PARTS_LWEAPON; i <= PARTS_SHIELD; i++)
	{
		pItemElem = m_pMover->GetEquipItem(i);
		if (pItemElem)
			nSpaceNeeded++;
	}

	for (int i = PARTS_RING1; i <= PARTS_EARRING2; i++)
	{
		pItemElem = m_pMover->GetEquipItem(i);
		if (pItemElem && pSwitch->dwItem[i] != NULL_ID && pItemElem->m_dwObjId != pSwitch->dwItem[i])
			nSpaceNeeded++;
	}

	return nSpaceNeeded;
}
void CSwitches::UseSwitch(int nSel)
{
	if (!m_pMover->IsPossibleUseItem())
		return;

	if (m_pMover->m_pActMover && m_pMover->m_pActMover->IsActAttack())
		return;

	if (nSel < 0 || nSel >= GetTotalSwitches())
		return;

	CUser* pUser = (CUser*)m_pMover;
	if (g_tmCurrent < pUser->m_dwNextSwitch)
	{
		pUser->AddText("Can be used every 0.4 sec!", 0xffffffff);
		return;
	}
	pUser->m_dwNextSwitch = g_tmCurrent + 400;

	EQ_SWITCH* pSwitch = &GetSwitches()->at(nSel);
	if (!pSwitch)
		return;

	int nRequiredEmptySpace = GetRequiredEmptySpace(pSwitch);
	if (m_pMover->m_Inventory.GetEmptyCount() < nRequiredEmptySpace)
	{
		CString str;
		str.Format("You should have at least %d free slots in your inventory to switch equipment!", nRequiredEmptySpace);
		pUser->AddText(str, 0xffffffff);
		return;
	}

	// Unequip all weapons and shield
	CItemElem* pItemElem = NULL;
	for (int i = PARTS_LWEAPON; i <= PARTS_SHIELD; i++)
	{
		pItemElem = m_pMover->GetEquipItem(i);
		if (pItemElem)
			m_pMover->DoUseItem(pItemElem->m_dwObjId);
	}

	// Unequip rings & earrings
	for (int i = PARTS_RING1; i <= PARTS_EARRING2; i++)
	{
		pItemElem = m_pMover->GetEquipItem(i);
		if (pItemElem && pSwitch->dwItem[i] != NULL_ID && pItemElem->m_dwObjId != pSwitch->dwItem[i])
			m_pMover->DoUseItem(pItemElem->m_dwObjId);
	}

	int nPart = 0;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		nPart = i;

		// Equip firstly right hand weapon, after that equip left hand weapon
		if (nPart == PARTS_LWEAPON)
			nPart = PARTS_RWEAPON;
		else if (nPart == PARTS_RWEAPON)
			nPart = PARTS_LWEAPON;

		pItemElem = m_pMover->m_Inventory.GetAtId(pSwitch->dwItem[nPart]);
		if (!IsUsableItem(pItemElem))
			continue;

		if (m_pMover->m_Inventory.IsEquip(pSwitch->dwItem[nPart]))
			continue;

		m_pMover->DoUseItem(pItemElem->m_dwObjId);
	}

	pItemElem = m_pMover->m_Inventory.GetAtId(pSwitch->dwLooterPet);
	if (IsUsableItem(pItemElem))
	{
		if (!m_pMover->IsUsingEatPet(pItemElem))
		{
			m_pMover->InactivateEatPet();
			m_pMover->DoUseItem(pItemElem->m_dwObjId);
		}
	}

	pItemElem = m_pMover->m_Inventory.GetAtId(pSwitch->dwPet);
	if (IsUsableItem(pItemElem))
	{
		if (m_pMover->GetPetItem() != pItemElem)
		{
			m_pMover->PetRelease();
			m_pMover->DoUseItem(pItemElem->m_dwObjId);
		}
	}

	pItemElem = m_pMover->m_Inventory.GetAtId(pSwitch->dwRegen);
	if (IsUsableItem(pItemElem))
		m_pMover->DoUseItem(pItemElem->m_dwObjId);
}
#endif
void CSwitches::FindAndRemove(DWORD dwObjId)
{
	BOOL bFound = FALSE;

	for (auto& it : m_vecSwitches)
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			if (it.dwItem[i] == dwObjId)
			{
				it.dwItem[i] = NULL_ID;
				bFound = TRUE;
			}
		}

		if (it.dwLooterPet == dwObjId)
		{
			it.dwLooterPet = NULL_ID;
			bFound = TRUE;
		}

		if (it.dwPet == dwObjId)
		{
			it.dwPet = NULL_ID;
			bFound = TRUE;
		}

		if (it.dwRegen == dwObjId)
		{
			it.dwRegen = NULL_ID;
			bFound = TRUE;
		}
	}

#ifdef __CLIENT
	if (m_pMover->IsActiveMover())
	{
		CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
		if (pWnd)
		{
			if (bFound)
				pWnd->UpdateParts();

			if (pWnd->m_pWndNewSwitch)
				pWnd->m_pWndNewSwitch->FindAndRemove(dwObjId);
		}

		if (bFound)
			g_WndMng.PutString("Item was removed from all equipment presets.");
	}
#endif
}
void CSwitches::ClearAllSwitches()
{
	m_vecSwitches.clear();

#ifdef __CLIENT
	if (m_pMover->IsActiveMover())
	{
		CWndSwitchEquip* pWnd = (CWndSwitchEquip*)g_WndMng.GetWndBase(APP_EQ_SWITCH);
		if (pWnd)
		{
			pWnd->UpdateModel();
			pWnd->ResetListbox();
		}

		g_WndMng.PutString("All equipment presets was deleted.");
	}
#endif
}
#endif
