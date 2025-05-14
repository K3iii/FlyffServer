#include "stdafx.h"
#include "defineObj.h"
#include "CreateObj.h"

#if __VER >= 11 // __SYS_IDENTIFY
#include "randomoption.h"
#ifdef __WORLDSERVER
#include "ticket.h"
#endif	// __WORLDSERVER
#endif	// __SYS_IDENTIFY

#include "serialnumber.h"
#if __VER >= 9
#include "definetext.h"
#endif

BOOL IsUsableItem(CItemElem* pItem)
{
	if (pItem == NULL)
		return FALSE;

	if (pItem->IsEmpty() == TRUE)
		return FALSE;

	if (pItem->GetExtra() != 0)
		return FALSE;

	return TRUE;
}

BOOL IsUsingItem(CItemElem* pItem)
{
	if (pItem == NULL)
		return FALSE;

	if (pItem->GetExtra() != 0)
		return TRUE;
	else
		return FALSE;
}

CItemElem::CItemElem()
{
	m_szItemText[0] = '\0';
	m_dwItemId = 0;
	m_dwObjId = NULL_ID;
	m_dwObjIndex = NULL_ID;
	m_nExtra = 0;
	m_nCost = 0;
#ifdef __FL_VENDORS
	m_nPerinCost = 0;
#endif
	m_nShopCost = 0;
	m_liSerialNumber = 0;
#ifdef __CLIENT
	m_pTexture = nullptr;
#endif

	m_nItemNum = 1;
	m_nAbilityOption = 0;
	m_nHitPoint = -1;
	m_byFlag = 0;
	m_bItemResist = SAI79::NO_PROP;
	m_nResistAbilityOption = 0;
#ifdef __WORLDSERVER
	m_bQuery = FALSE;
#endif	// __WORLDSERVER
	m_dwKeepTime = 0;
	m_iRandomOptItemId = 0;
	m_n64NewRandomOption = 0;
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
	m_nExtra = 0;
	m_nCost = 0;
#ifdef __FL_VENDORS
	m_nPerinCost = 0;
#endif
	m_nShopCost = 0;
#ifdef __CLIENT
	m_pTexture = nullptr;
#endif

	SAFE_DELETE(m_pPet);
#ifdef __FL_PACKED_ITEMS
	ClearPackedItems();
#endif
	m_piercing.Clear();
}

CItemElem& CItemElem::operator = (CItemElem& ie)
{
	_tcscpy(m_szItemText, ie.m_szItemText);
	m_dwItemId = ie.m_dwItemId;
	m_liSerialNumber = ie.m_liSerialNumber;
#ifdef __CLIENT
	m_pTexture = ie.m_pTexture;
#endif

	m_nItemNum = ie.m_nItemNum;
	m_nHitPoint = ie.m_nHitPoint;
	m_byFlag = ie.m_byFlag;
	m_nAbilityOption = ie.m_nAbilityOption;
	m_bItemResist = ie.m_bItemResist;
	m_nResistAbilityOption = ie.m_nResistAbilityOption;
	m_dwKeepTime = ie.m_dwKeepTime;
	m_piercing = ie.m_piercing;
	m_iRandomOptItemId = ie.GetRandomOptItemId();
	m_n64NewRandomOption = ie.GetNewRandomOption();

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
	m_dwLookId = ie.m_dwLookId;
#endif

#ifdef __WEAPON_RARITY
	m_nWeaponRarity = ie.m_nWeaponRarity;
#endif // __WEAPON_RARITY

	return *this;
}

#ifdef __CLIENT
void CItemElem::SetTexture()
{
	ItemProp* pProp = prj.GetItemProp(m_dwItemId);
	if (pProp == NULL)
	{
		Error("GetProp() NULL Return ItemId:[%d]", m_dwItemId);
		return;
	}

	CString strIcon = pProp->szIcon;
	if (pProp->dwItemKind3 == IK3_EGG)
	{
		const BYTE nLevel = m_pPet ? m_pPet->GetLevel() : 0;

		switch (nLevel)
		{
		case PL_D:
		case PL_C:
			strIcon.Replace(".", "_00.");
			break;
		case PL_B:
		case PL_A:
			strIcon.Replace(".", "_01.");
			break;
		case PL_S:
			strIcon.Replace(".", "_02.");
			break;
		}
	}
	m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, strIcon), 0xffff00ff);
}
#endif

int	CItemElem::GetCost(void)
{
	if (m_nShopCost > 0)
		return m_nShopCost;

	ItemProp* pProp = GetProp();
	if (!pProp)
		return -1;

	if (pProp->dwCost == NULL_ID)
		return -1;

	int nCost;
	if (GetAbilityOption())
		nCost = (int)(pProp->dwCost + pProp->dwCost * (0.1f + (GetAbilityOption() * GetAbilityOption()) / 18.0f));
	else
		nCost = (int)pProp->dwCost;

	return nCost;
}

DWORD CItemElem::GetChipCost()
{
	if (m_nShopCost > 0)
		return m_nShopCost;

	ItemProp* pProp = GetProp();
	if (!pProp)
		return NULL_ID;

	if (pProp->dwReferValue1 == NULL_ID)
		return NULL_ID;

	return pProp->dwReferValue1;
}
BOOL CItemElem::IsQuest()
{
	ItemProp* pProp = GetProp();
	if (pProp == NULL)
		return FALSE;

	if (pProp->dwItemKind3 != IK3_QUEST)
		return FALSE;

	return TRUE;
}

int	CItemElem::GetGold()
{
	ItemProp* pProp = GetProp();
	if (pProp == NULL)
		return 0;

	if (pProp->dwItemKind1 != IK1_GOLD)
		return 0;

	return m_nHitPoint;
}

void CItemElem::UseItem()
{
	if (GetProp()->bPermanence != TRUE)
		m_nItemNum--;
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

void CItemElem::GetPiercingAvail(PPIERCINGAVAIL pPiercingAvail)
{
	for (int i = 0; i < GetPiercingSize(); i++)
	{
		PPIERCINGAVAIL ptr = CPiercingAvail::GetInstance()->GetPiercingAvail(GetPiercingItem(i));
		if (ptr)
		{
			for (int j = 0; j < ptr->nSize; j++)
			{
				int nFind = -1;
				for (int k = 0; k < pPiercingAvail->nSize; k++)
				{
					if (pPiercingAvail->anDstParam[k] == ptr->anDstParam[j])
					{
						nFind = k;
						break;
					}
				}
				if (nFind < 0)
					nFind = pPiercingAvail->nSize++;
				pPiercingAvail->anDstParam[nFind] = ptr->anDstParam[j];
				pPiercingAvail->anAdjParam[nFind] += ptr->anAdjParam[j];
			}
		}
	}
}


BOOL CItemElem::IsDiceRefineryAble(ItemProp* pProp)
{
	if (!pProp)
		return FALSE;

	if (pProp->dwItemKind2 == IK2_ARMOR ||
		pProp->dwItemKind2 == IK2_WEAPON_MAGIC ||
		pProp->dwItemKind2 == IK2_WEAPON_DIRECT ||
		pProp->dwItemKind2 == IK2_ARMORETC)
		return TRUE;

	return FALSE;
}

BOOL CItemElem::IsElementalCard(const DWORD dwItemID)
{
	switch (dwItemID)
	{
	case II_GEN_MAT_ELE_FLAME: case II_GEN_MAT_ELE_RIVER: case II_GEN_MAT_ELE_GENERATOR: case II_GEN_MAT_ELE_DESERT: case II_GEN_MAT_ELE_CYCLON:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CItemElem::IsBinds(void)
{
	ItemProp* pProperty = GetProp();

	if (!pProperty)
		return FALSE;

	if (m_dwKeepTime && pProperty->dwItemKind2 != IK2_WARP)
		return TRUE;

	if ((pProperty->dwFlag & IP_FLAG_BINDS) == IP_FLAG_BINDS)
		return TRUE;

	if (IsFlag(CItemElem::binds))
		return TRUE;

	if (g_xRandomOptionProperty->GetRandomOptionSize(GetRandomOptItemId()) > 0
		&& (g_xRandomOptionProperty->GetRandomOptionKind(this) == CRandomOptionProperty::eBlessing || g_xRandomOptionProperty->GetRandomOptionKind(this) == CRandomOptionProperty::eEatPet))
		return TRUE;

	if (GetLevelDown() < 0)
		return TRUE;

	return FALSE;
}

BOOL CItemElem::IsUndestructable(void)
{
	ItemProp* pProperty = GetProp();
	if ((pProperty->dwFlag & IP_FLAG_UNDESTRUCTABLE) == IP_FLAG_UNDESTRUCTABLE)
		return TRUE;
	return FALSE;
}

BOOL CItemElem::IsLogable(void)
{
	if (GetProp()->nLog != -1 || GetRandomOptItemId() != 0)
		return TRUE;

	return FALSE;
}

void	CItemElem::SetSerialNumber(void)
{
	m_liSerialNumber = CSerialNumber::GetInstance()->Get();
}


#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __WORLDSERVER
#ifdef __VM_0819
CItemPool* CItem::m_pPool = new CItemPool(512, "CItem");
#else	// __VM_0819
CItemPool* CItem::m_pPool = new CItemPool(512);
#endif	// __VM_0819
#else	// __WORLDSERVER
#ifdef __VM_0819
CItemPool* CItem::m_pPool = new CItemPool(128, "CItem");
#else	// __VM_0819
CItemPool* CItem::m_pPool = new CItemPool(128);
#endif	// __VM_0819
#endif	// __WORLDSERVER
#endif	// __MEM_TRACE
#endif	// __VM_0820

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
CItem::CItem()
{
	m_dwType = OT_ITEM;
	m_pItemBase = NULL;
	m_idHolder = 0;
	m_idOwn = NULL_ID;
	m_dwDropTime = 0;
	m_bDropMob = 0;
	m_dwDropTime = timeGetTime();
#ifdef __CLIENT
	m_fGroundY = 0;
	m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
#endif //__CLIENT
#ifdef __EVENT_MONSTER
	m_IdEventMonster = NULL_ID;
#endif // __EVENT_MONSTER
}

void CItem::SetOwner(OBJID id)
{
	m_idOwn = id;
	m_dwDropTime = ::timeGetTime();
	m_bDropMob = TRUE;
}

CItem::~CItem()
{
	SAFE_DELETE(m_pItemBase);
	if (GetWorld())
	{
#if !defined(__CLIENT)
#ifdef __LAYER_1021
		GetWorld()->m_respawner.Increment(GetRespawn(), m_nRespawnType, FALSE, GetLayer());
#else	// __LAYER_1021
		GetWorld()->m_respawner.Increment(GetRespawn(), m_nRespawnType, FALSE);
#endif	// __LAYER_1021
#endif
	}
}

BOOL CItem::Read(CFileIO* pFile)
{
	CObj::Read(pFile);
	return TRUE;
}

#ifdef __CLIENT
void CItem::SetDelta(float fGroundY, D3DXVECTOR3& vDelta)
{
	m_fGroundY = fGroundY;
	m_vDelta = vDelta;
}
#endif // __CLIENT

void CItem::Process()
{
	CCtrl::Process();
#ifdef __CLIENT
	AddAngle(0.5f);
	D3DXVECTOR3 vPos = GetPos();


	if (m_fGroundY)
	{
		if (vPos.y > m_fGroundY)
		{
			m_vDelta.y -= 0.0075f;
		}
		else
		{
			vPos.y = m_fGroundY;
			m_vDelta.x = m_vDelta.y = m_vDelta.z = 0;
			SetPos(vPos);
		}


		if (m_vDelta.x == 0 && m_vDelta.y == 0 && m_vDelta.z == 0)
		{

		}
		else
		{
			m_vDelta.x = m_vDelta.z = 0;
			vPos += m_vDelta;
			SetPos(vPos);
		}
	}
#endif // __CLIENT

#ifdef __WORLDSERVER
	if ((int)g_tmCurrent - (int)m_dwDropTime > MIN(1))
		Delete();
#endif	// __WORLDSERVER
}

void CItem::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifndef __WORLDSERVER
	ItemProp* pItemProp = GetProp();
	if (pItemProp && pItemProp->nReflect > 0)
		((CModelObject*)m_pModel)->SetEffect(0, XE_REFLECT);

	CObj::Render(pd3dDevice);

#ifndef __FL_UPDATE_GROUNDITEMS
	if (xRandom(50) == 1)
		CreateSfx(pd3dDevice, XI_GEN_ITEM_SHINE01, GetPos());
#else
	if (xRandom(150) == 1)
		CreateSfx(pd3dDevice, XI_GEN_ITEM_SHINE01, GetPos());
#endif

#endif
}

void CItem::RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor)
{
#ifndef __WORLDSERVER
#ifdef __FL_UPDATE_GROUNDITEMS
	ItemProp* pProp = GetProp();
	if (!pProp)
		return;
#endif

	D3DXVECTOR3 vOut, vPos = GetPos(), vPosHeight;
	D3DVIEWPORT9 vp;
	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	pd3dDevice->GetViewport(&vp);
	D3DXMATRIX matTrans;
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &m_matRotation);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matTrans);

	vPosHeight = pBB->m_vPos[0];
	vPosHeight.x = 0;
	vPosHeight.z = 0;

	D3DXVec3Project(&vOut, &vPosHeight, &vp, &GetWorld()->m_matProj,
		&GetWorld()->m_pCamera->m_matView, &matWorld);

#ifndef __FL_UPDATE_GROUNDITEMS
	vOut.x -= pFont->GetTextExtent(m_pItemBase->GetProp()->szName).cx / 2;
	pFont->DrawText(vOut.x + 1, vOut.y + 1, 0xff000000, m_pItemBase->GetProp()->szName);
	pFont->DrawText(vOut.x, vOut.y, dwColor, m_pItemBase->GetProp()->szName);
#else
	CItemElem* pItemElem = m_pItemBase;
	if (pItemElem)
	{
		CString strTemp;
		if (pProp->dwItemKind3 != IK3_GOLD)
			strTemp.Format("%s (x%d)", pProp->szName, pItemElem->m_nItemNum);
		else
			strTemp.Format("%s", pProp->szName);

		vOut.x -= pFont->GetTextExtent(strTemp).cx / 2;
		pFont->DrawText(vOut.x, vOut.y - 25, dwColor, strTemp);
}
#endif
#endif	// __WORLDSERVER
}

CString CItemElem::GetName(void)
{
	ItemProp* pProp = GetProp();
	CString strName = pProp->szName;
	if (pProp->dwItemKind3 == IK3_EGG && m_pPet)
	{
		MoverProp* pMoverProp = prj.GetMoverProp(m_pPet->GetIndex());
		if (pMoverProp)
		{
#ifdef __PET_1024
			if (m_pPet->HasName())
				strName.Format(prj.GetText(TID_GAME_CAGE_STRING), m_pPet->GetName());
			else
#endif	// __PET_1024
				strName.Format(prj.GetText(TID_GAME_CAGE_STRING), pMoverProp->szName);
		}
	}
	return strName;
}

int	CItemElem::GetLevelDown(void)
{
	if (m_iRandomOptItemId & 0x8000000000000000)
		return -10;
	else if (m_iRandomOptItemId & 0x4000000000000000)
		return -5;
	return 0;
}

void CItemElem::SetLevelDown(int i)
{
	//	0x8000000000000000
	//	0x4000000000000000
	m_iRandomOptItemId &= ~0xC000000000000000;
	if (i == e5LevelDown)
		m_iRandomOptItemId |= 0x4000000000000000;
	else if (i == e10LevelDown)
		m_iRandomOptItemId |= 0x8000000000000000;
}

DWORD CItemElem::GetLimitLevel(void)
{
	if (GetProp()->dwLimitLevel1 == 0xFFFFFFFF)
		return 0xFFFFFFFF;
	int nLimitLevel = static_cast<int>(GetProp()->dwLimitLevel1) + GetLevelDown();
	if (nLimitLevel < 0)
		nLimitLevel = 0;

	return (DWORD)nLimitLevel;
}

BOOL CItemElem::IsLimitLevel(CMover* pMover)
{
	if (pMover == NULL)
		return FALSE;

	if (pMover->GetJobType() >= JTYPE_MASTER && pMover->GetJobType() > pMover->GetJobType(GetProp()->dwItemJob))
		return FALSE;

	if ((DWORD)(pMover->GetLevel()) >= GetLimitLevel())
		return FALSE;

	return TRUE;
}


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

BOOL CItemElem::IsOnePenyaPrice()
{
	ItemProp* pProp = GetProp();
	if (!pProp)
		return TRUE;

	if (pProp->dwItemKind3 == IK3_TICKET)
		return TRUE;

	if (m_dwKeepTime > 0 || IsFlag(CItemElem::expired))
		return TRUE;

	if (IsFlag(CItemElem::binds))
		return TRUE;

	return FALSE;
}

#ifdef __WORLDSERVER
BOOL CItemElem::IsActiveTicket(DWORD dwItemId)
{
	ItemProp* pProp = GetProp();
	if (!pProp)
		return FALSE;

	if (!IsFlag(expired) && pProp->dwItemKind3 == IK3_TICKET && m_dwKeepTime > 0)
	{
		TicketProp* pProp1 = CTicketProperty::GetInstance()->GetTicketProp(m_dwItemId);
		TicketProp* pProp2 = CTicketProperty::GetInstance()->GetTicketProp(dwItemId);
		return (pProp1->dwWorldId == pProp2->dwWorldId);
	}
	return FALSE;
}
#endif	// __WORLDSERVER
BOOL	IsNeedTarget(ItemProp* pProp)
{
	return(pProp->dwExeTarget == EXT_ITEM);
}

#ifdef __WEAPON_RARITY
BOOL IsValidRarityItem(DWORD dwIdIk3)
{
	switch (dwIdIk3)
	{
	case IK3_SWD:
	case IK3_AXE:
	case IK3_CHEERSTICK:
	case IK3_KNUCKLEHAMMER:
	case IK3_WAND:
	case IK3_STAFF:
	case IK3_THSWD:
	case IK3_THAXE:
	case IK3_YOYO:
	case IK3_BOW:
	case IK3_SHIELD:
	case IK3_MAGICBARUNA:
	case IK3_ZEMBARUNA:
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}
BOOL IsValidRarityItem2(DWORD dwIdIk3)
{
	switch (dwIdIk3)
	{
	case IK3_HELMET:
	case IK3_SUIT:
	case IK3_GAUNTLET:
	case IK3_BOOTS:
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}
#endif // __WEAPON_RARITY

BOOL CItemElem::IsEgg()
{
	if (!IsPet())
		return FALSE;
	return (!m_pPet || m_pPet->GetLevel() == PL_EGG);
}

#if __VER >= 15 // __PETVIS
void  CItemElem::SetSwapVisItem(int nPos1, int nPos2)
{
	int nSize = GetPiercingSize();
	if (nPos1 >= nSize || nPos2 >= nSize)
		return;

	DWORD dwTempId = GetPiercingItem(nPos1);
	time_t tmTemp = GetVisKeepTime(nPos1);

	SetPiercingItem(nPos1, GetPiercingItem(nPos2));
	SetVisKeepTime(nPos1, GetVisKeepTime(nPos2));
	SetPiercingItem(nPos2, dwTempId);
	SetVisKeepTime(nPos2, tmTemp);
}

DWORD CItemElem::GetVisPetSfxId()
{
	int nLevel = 0;
	for (int i = 0; i < GetPiercingSize(); i++)
	{
		ItemProp* pProp = prj.GetItemProp(GetPiercingItem(i));
		if (pProp && pProp->dwAbilityMax > (DWORD)(nLevel))
			nLevel = pProp->dwAbilityMax;
	}

	switch (nLevel)
	{
	case 1:	return XI_BUFFPET_GRADE1;
	case 2: return XI_BUFFPET_GRADE2;
	case 3: return XI_BUFFPET_GRADE3;
	}

	return NULL_ID;
}
#endif // __PETVIS

bool		CItemElem::SelectRandomOption(BYTE bySelectFlag)
{
	bool	bRetValue = true;

	if (bySelectFlag == _AWAKE_OLD_VALUE)
	{
		ResetNewRandomOption();
	}
	else if (bySelectFlag == _AWAKE_NEW_VALUE)
	{
		SelectNewRandomOption();
		ResetNewRandomOption();
	}
	else
	{
		WriteLog("bySelectFlag is invalid value. bySelectFlag : [%d]", (int)(bySelectFlag));

		bRetValue = false;
	}

	return	bRetValue;
}