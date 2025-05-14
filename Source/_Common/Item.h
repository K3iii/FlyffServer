#if !defined(AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_)
#define AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(__DBSERVER) || defined(__CORESERVER)
#include "..\_Network\Objects\Obj.h"
#else
#include "Ctrl.h"
#endif
#include "ProjectCmn.h"
#include "lang.h"

#if __VER >= 9	// __PET_0410
#include "pet.h"
#endif	// __PET_0410

#if __VER >= 11 // __SYS_COLLECTING
#include "accessory.h"
#endif	// __SYS_COLLECTING

#include "Piercing.h"

class CItemElem
{
public:
	OBJID       m_dwObjId;
	DWORD		m_dwObjIndex;
	DWORD		m_dwItemId;
	int 		m_nExtra;
	TCHAR       m_szItemText[32];
	int			m_nCost;
#ifdef __FL_VENDORS
	int			m_nPerinCost;
#endif
	int			m_nShopCost;
#ifdef __CLIENT
	CTexture* m_pTexture;
#endif

private:
	SERIALNUMBER		m_liSerialNumber;

public:
	BOOL IsEmpty() { return m_dwItemId ? FALSE : TRUE; }
	ItemProp* GetProp() { return prj.GetItemProp(m_dwItemId); }

	int				GetCost();
	void			SetExtra(int nExtra) { m_nExtra = nExtra; }
	int				GetExtra() { return m_nExtra; }
	DWORD			GetChipCost();
	BOOL			IsQuest();
	BOOL			IsOnePenyaPrice();

	void	SetSerialNumber(void);
	SERIALNUMBER	GetSerialNumber(void) { return m_liSerialNumber; }
	void	SetSerialNumber(SERIALNUMBER liSerialNumber) { m_liSerialNumber = liSerialNumber; }

private:
	CPiercing	m_piercing;
public:
	void	SetPiercingSize(int nSize) { m_piercing.SetPiercingSize(nSize); }
	int		GetPiercingSize() { return m_piercing.GetPiercingSize(); }
	void	SetPiercingItem(int nth, DWORD dwItem) { m_piercing.SetPiercingItem(nth, dwItem); }
	DWORD	GetPiercingItem(int nth) { return m_piercing.GetPiercingItem(nth); }
	void	CopyPiercing(const CPiercing& piercing) { m_piercing = piercing; }
	void	SerializePiercing(CAr& ar) { m_piercing.Serialize(ar); }
	BOOL	IsPiercedItem() { return m_piercing.IsPiercedItem(); }

	void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
	int		GetUltimatePiercingSize() { return m_piercing.GetUltimatePiercingSize(); }
	void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
	DWORD	GetUltimatePiercingItem(int nth) { return m_piercing.GetUltimatePiercingItem(nth); }

	BOOL IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE);

public:
	enum { expired = 0x01, binds = 0x02, isusing = 0x04, };

	int			m_nHitPoint;
	short		m_nItemNum;
	BYTE		m_byFlag;
	BYTE   	    m_bItemResist;
	int			m_nResistAbilityOption;
	int			m_nAbilityOption;
	DWORD		m_dwKeepTime;
#ifdef __WORLDSERVER
	BOOL		m_bQuery;
#endif	// __WORLDSERVER

private:
#ifdef __MODEL_CHANGE
	DWORD		m_dwLookId;
#endif 
	__int64		m_iRandomOptItemId;
	__int64		m_n64NewRandomOption;

public:
#ifdef __MODEL_CHANGE
	void SetLook(const DWORD dwId) { m_dwLookId = dwId; }
	DWORD GetLook() { return m_dwLookId; }
	bool IsChangedLook() { return m_dwLookId != NULL_ID; }
#endif

	__int64 GetRandomOptItemId(void) { return m_iRandomOptItemId; }
	void	SetRandomOptItemId(__int64 iRandomOptItemId) { m_iRandomOptItemId = iRandomOptItemId; }
	int		GetRandomOpt(void) { return static_cast<int>(m_iRandomOptItemId & 0x00000000000000FF); }
	void	SetRandomOpt(int nRandomOpt) { m_iRandomOptItemId = (m_iRandomOptItemId & 0xFFFFFFFFFFFFFF00) | static_cast<__int64>(nRandomOpt & 0x00000000000000FF); }
	__int64* GetRandomOptItemIdPtr(void) { return &m_iRandomOptItemId; }

	__int64		GetNewRandomOption() { return m_n64NewRandomOption; }
	__int64* GetNewRandomOptionPtr() { return &m_n64NewRandomOption; }
	void		ResetNewRandomOption() { m_n64NewRandomOption = 0; }
	void		SelectNewRandomOption() { m_iRandomOptItemId = ((m_iRandomOptItemId & 0xC0000000000000FF) | m_n64NewRandomOption); }
	bool		SelectRandomOption(BYTE bySelectFlag);

	#ifdef __WEAPON_RARITY
	short		m_nWeaponRarity;
	#endif // __WEAPON_RARITY

	BOOL	IsPerin(void) { return m_dwItemId == II_SYS_SYS_SCR_PERIN; }
	BOOL	IsCollector(BOOL bRefinable = FALSE) { return(m_dwItemId == II_GEN_TOO_COL_NORMAL || (!bRefinable && m_dwItemId == II_GEN_TOO_COL_EVENT)); }
	BOOL	IsAccessory(void) { return CAccessoryProperty::GetInstance()->IsAccessory(m_dwItemId); }
	BOOL	IsActiveTicket(DWORD dwItemId);

	enum { eNoLevelDown, e5LevelDown, e10LevelDown, };
	int		GetLevelDown(void);	// 64|63
	void	SetLevelDown(int i = eNoLevelDown);
	DWORD	GetLimitLevel(void);
	BOOL	IsLimitLevel(CMover* pMover);

	CPet* m_pPet;
	BOOL	IsPet(void) { return GetProp()->dwItemKind3 == IK3_EGG; }
	BOOL	IsEatPet(void) { return GetProp()->dwItemKind3 == IK3_PET; }
	BOOL	IsFeed(void) { return GetProp()->dwItemKind3 == IK3_FEED; }
	BOOL	IsEgg(void);
	CString		GetName(void);

#ifdef __FL_PACKED_ITEMS
private:
	vector<CItemElem*> m_vecPackedItems;
public:
	void ClearPackedItems();
	bool AddToPack(CItemElem* pItem);
	bool DeleteFromPack(int nSel);
	vector<CItemElem*>* GetItemList() { return &m_vecPackedItems; }
	bool IsPackage() { return m_dwItemId == II_SYS_SYS_SCR_PACKAGE; }
#endif

public:
	//	Constructions
	CItemElem();
	~CItemElem();

	//	Attributes
	void Empty();
	int	GetAttrOption();
	int	GetAbilityOption() { return m_nAbilityOption; }
	int	GetItemResist(void) { return m_bItemResist; }
	int	GetResistAbilityOption(void) { return m_nResistAbilityOption; }
	int* GetAbilityOptionPtr() { return &m_nAbilityOption; }
	void SetAbilityOption(int nAbilityOption) { m_nAbilityOption = nAbilityOption; }
	int	GetGold();

#ifdef __CLIENT
	CTexture* GetTexture() { return m_pTexture; }
	void SetTexture(CTexture* pTexture) { m_pTexture = pTexture; }
	void SetTexture();
#endif

	void	GetPiercingAvail(PPIERCINGAVAIL pPiercingAvail);

	BOOL	IsExpiring(void)
	{
		if (m_dwKeepTime)
		{
			if (time_null() > (time_t)m_dwKeepTime)
				return TRUE;
		}
		return FALSE;
	}

	void	ResetFlag(BYTE byFlag) { m_byFlag &= ~byFlag; }
	void	SetFlag(BYTE byFlag) { m_byFlag |= byFlag; }
	BOOL	IsFlag(BYTE byFlag) { return (m_byFlag & byFlag) ? TRUE : FALSE; }
	BOOL	IsBinds();
	BOOL	IsUndestructable(void);
	BOOL	IsLogable();

	static BOOL	IsDiceRefineryAble(ItemProp* pProp);
	static BOOL	IsElementalCard(const DWORD dwItemID);

	//	Operations
	void	UseItem();
	CItemElem& operator = (CItemElem& ie);
	void	Serialize(CAr& ar);

	BOOL	IsVisPet() { return (GetProp() && GetProp()->IsVisPet()) || IsTransformVisPet(); }
	void	SetSwapVisItem(int nPos1, int nPos2);
	void	SetVisKeepTimeSize(int nSize) { m_piercing.SetVisKeepTimeSize(nSize); }
	void	SetVisKeepTime(int nth, time_t tmKeep) { m_piercing.SetVisKeepTime(nth, tmKeep); }
	time_t	GetVisKeepTime(int nth) { return m_piercing.GetVisKeepTime(nth); }
	DWORD	GetVisPetSfxId();
	BOOL	IsTransformVisPet() { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }

	BOOL	m_bTranformVisPet;
};

inline int CItemElem::GetAttrOption()
{
	int nAttr = (int)m_bItemResist;
	int nOption = m_nResistAbilityOption;
	int nRet = m_nAbilityOption;

	nRet |= (nAttr << 16);
	nRet |= (nOption << 24);
	return nRet;
}

typedef struct tagSkill
{
	DWORD dwSkill;
	DWORD dwLevel;
	ItemProp* GetProp() { return prj.GetSkillProp(dwSkill); }

} SKILL, * LPSKILL;

template <class T> class CItemContainer
{
public:
	LPDWORD    m_apIndex;
	DWORD      m_dwIndexNum;
	DWORD      m_dwItemMax;
	T* m_apItem;
#ifdef __FL_SWITCH_EQ
	BOOL m_bInventory;
	CMover* m_pMover;
#endif

	void SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = NULL_ID);
	CItemContainer();
	virtual ~CItemContainer();

	void Clear();

	T* GetAt(DWORD dwIndex);
#ifdef __CLIENT
	DWORD	Find(DWORD dwItemId)
	{
		if (dwItemId == 0)
			return NULL_ID;
		for (int i = 0; i < (int)(m_dwIndexNum); i++)
		{
			DWORD nId = m_apIndex[i];
			if (nId < 0 || nId >= m_dwItemMax)
				continue;
			if (m_apItem[nId].m_dwItemId == dwItemId)
				return nId;
		}
		return NULL_ID;
	}
	DWORD	GetItemCount(DWORD dwItemId)
	{
		if (dwItemId == 0)
			return 0;
		int nCount = 0;
		for (int i = 0; i < (int)(m_dwItemMax); i++)
		{
			CItemElem* pItemElem = &m_apItem[i];
			if (pItemElem->m_dwItemId == dwItemId)
				nCount += pItemElem->m_nItemNum;
		}
		return(nCount);
	}
#endif	// __CLIENT


	BOOL DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex);
	BOOL UnEquip(DWORD dwIndex);
	BOOL IsEquip(DWORD dwObjId);
	T* GetEquip(DWORD dwIndex);

	void	Swap(DWORD dwSrcIndex, DWORD dwDstIndex);
	BOOL	Add(T* pElem, DWORD* pItemObjID = NULL, int* pItemQuantity = NULL, DWORD* pCount = NULL);
	BOOL	IsFull(T* pElem, ItemProp* pItemProp, short nNum);
	void	SetAtId(OBJID dwObjId, T* pItemElem);

	T* GetAtId(OBJID dwObjId);
	T* GetAtItemId(DWORD dwItemId);
	int		GetAtItemNum(DWORD dwItemId);
	void	RemoveAtId(OBJID dwObjId);

	int		GetSize() { return int(m_dwIndexNum); }
	int		GetMax() { return int(m_dwItemMax); }
	void	Copy(CItemContainer<T>& rItemContainer);
	void	Serialize(CAr& ar);

	int		GetEmptyCount()
	{
		int nCount = 0;
		for (DWORD i = 0; i < m_dwItemMax; i++)
		{
			if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex < m_dwIndexNum)
				nCount++;
		}
		return(nCount);
	}
	int		GetCount()
	{
		int nCount = 0;
		for (DWORD i = 0; i < m_dwItemMax; i++)
		{
			if (m_apItem[i].IsEmpty() == FALSE && m_apItem[i].m_dwObjIndex < m_dwIndexNum)
				nCount++;
		}
		return(nCount);
	}
	int		GetCountByIK3(DWORD dwItemKind3)
	{
		int nCount = 0;
		for (int i = 0; i < m_dwItemMax; i++)
		{
			if (m_apItem[i].IsEmpty() == FALSE && m_apItem[i].GetProp()->dwItemKind3 == dwItemKind3)
				nCount++;
		}
		return(nCount);
	}
	int		GetCount(DWORD dwItemId)
	{
		int nCount = 0;
		for (DWORD i = 0; i < m_dwItemMax; i++)
		{
			if (m_apItem[i].IsEmpty() == FALSE && m_apItem[i].m_dwItemId == dwItemId)
				nCount++;
		}
		return(nCount);
	}
private:
	void	Swap2(DWORD dwSrcIndex, DWORD dwDstIndex);
};

template <class T> CItemContainer<T>::CItemContainer()
{
	m_dwIndexNum = 0;
	m_dwItemMax = 0;

	m_apItem = NULL;
	m_apIndex = NULL;
#ifdef __FL_SWITCH_EQ
	m_bInventory = FALSE;
	m_pMover = NULL;
#endif
}

template <class T> CItemContainer<T>::~CItemContainer()
{
	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);
}

template <class T> void CItemContainer<T>::Clear()
{
	for (DWORD i = 0; i < m_dwItemMax; i++)
	{
		m_apItem[i].Empty();
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum)
		{
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		}
		else
		{
			m_apIndex[i] = NULL_ID;
		}
	}
}

template <class T> void CItemContainer<T>::SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra)
{
	m_dwIndexNum = dwItemMax;
	m_dwItemMax = dwItemMax;
	if (dwExtra != NULL_ID)
		m_dwItemMax += dwExtra;

	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);

	m_apItem = new T[m_dwItemMax];
	m_apIndex = new DWORD[m_dwItemMax];
	Clear();
}

template <class T> T* CItemContainer<T>::GetAtId(DWORD dwId)
{
	if (dwId >= (DWORD)(GetMax()))
		return NULL;

	T* pItemElem = &m_apItem[dwId];
	if (pItemElem->IsEmpty())
		return NULL;

	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetAtItemId(DWORD dwItemId)
{
	for (DWORD i = 0; i < m_dwItemMax; i++)
	{
		if (m_apItem[i].m_dwItemId == dwItemId)
			return &m_apItem[i];
	}
	return NULL;
}

template <class T> int CItemContainer<T>::GetAtItemNum(DWORD dwItemId)
{
	int nResult = 0;
	for (DWORD i = 0; i < m_dwItemMax; i++)
	{
		if (m_apItem[i].m_dwItemId == dwItemId)
			nResult += m_apItem[i].m_nItemNum;
	}
	return nResult;
}

template <class T> BOOL CItemContainer<T>::DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex)
{
	dwDstIndex += m_dwIndexNum;

	if (dwSrcIndex == dwDstIndex)
		return FALSE;

	if (dwSrcIndex >= m_dwItemMax || dwDstIndex >= m_dwItemMax)
		return FALSE;

	for (DWORD i = 0; i < m_dwItemMax; i++)
	{
		if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID)
		{
			if (m_apIndex[dwSrcIndex] >= m_dwItemMax)
				return FALSE;
			m_apIndex[dwDstIndex] = m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex] = i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex = dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex = dwDstIndex;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> BOOL CItemContainer<T>::UnEquip(DWORD dwIndex)
{
	dwIndex += m_dwIndexNum;
	if (dwIndex >= m_dwItemMax)
		return FALSE;

	DWORD dwId = m_apIndex[dwIndex];

	if (dwId >= m_dwItemMax)
		return FALSE;

	for (DWORD i = 0; i < m_dwIndexNum; i++)
	{
		if (m_apIndex[i] >= m_dwItemMax)
			continue;

		if (m_apItem[m_apIndex[i]].IsEmpty())
		{
			m_apItem[m_apIndex[i]].m_dwObjIndex = NULL_ID;
			m_apIndex[dwIndex] = NULL_ID;
			m_apItem[dwId].m_dwObjIndex = i;
			m_apIndex[i] = dwId;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> BOOL CItemContainer<T>::IsEquip(DWORD dwObjId)
{
	if (!m_apItem)
	{
		WriteError("CONTAINER//0");
		return FALSE;
	}

	if (dwObjId >= m_dwItemMax)
		return FALSE;

	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum)
		return TRUE;
	return FALSE;
}

template <class T> T* CItemContainer<T>::GetEquip(DWORD dwIndex)
{
	if (m_dwIndexNum)
	{
		if (dwIndex < 0 || dwIndex >= (m_dwItemMax - m_dwIndexNum))
		{
			WriteError("GETEQUIP//%d, %d, %d", m_dwIndexNum, m_dwItemMax - m_dwIndexNum, dwIndex);
			return NULL;
		}
		return GetAt(m_dwIndexNum + dwIndex);
	}
	return NULL;
}

template <class T> BOOL CItemContainer<T>::IsFull(T* pElem, ItemProp* pItemProp, short nNum)
{
	int nId;
	short nNumtmp = nNum;
	CItemElem* pElemtmp;
	for (DWORD i = 0; i < m_dwIndexNum; i++)
	{
		nId = m_apIndex[i];
		if (nId < 0 || nId >= (int)(m_dwItemMax))
			continue;
		pElemtmp = &m_apItem[nId];
		if (pElemtmp->IsEmpty())
		{
			if (nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= (short)pItemProp->dwPackMax;
			else
				return FALSE;
		}
		else if (pElem->m_dwItemId == pElemtmp->m_dwItemId
			&& pElem->m_byFlag == pElemtmp->m_byFlag
			&& pElem->m_dwKeepTime == 0 && pElemtmp->m_dwKeepTime == 0
			)
		{
			if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
			else
				return FALSE;
		}
	}

	return TRUE;
}

template <class T> BOOL CItemContainer<T>::Add(T* pElem, DWORD* pItemObjID, int* pItemQuantity, DWORD* pCount)
{
	if (pElem == NULL)
		return FALSE;

	ItemProp* pItemProp = pElem->GetProp();
	if (!pItemProp)
		return FALSE;

	short nNumtmp = pElem->m_nItemNum;
	if (IsFull(pElem, pItemProp, nNumtmp))
		return FALSE;

	BOOL bPackItem = TRUE;
	if (pItemProp->dwPackMax == 1)
		bPackItem = FALSE;

	CItemElem* pElemtmp = NULL;

	if (pItemObjID)
		*pCount = 0;

	if (bPackItem)
	{
		for (DWORD i = 0; i < m_dwIndexNum; i++)
		{
			int nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;

			pElemtmp = &m_apItem[nId];

			if (pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElem->m_byFlag == pElemtmp->m_byFlag
				&& pElem->m_dwKeepTime == 0 && pElemtmp->m_dwKeepTime == 0
				)
			{
				if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				{
					nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;

					if (pItemObjID)
					{
						pItemObjID[*pCount] = nId;
						pItemQuantity[*pCount] = pElemtmp->m_nItemNum;
						++(*pCount);
					}
				}
				else
				{
					pElemtmp->m_nItemNum += nNumtmp;
					if (pItemObjID)
					{
						pItemObjID[*pCount] = nId;
						pItemQuantity[*pCount] = pElemtmp->m_nItemNum;
						++(*pCount);
					}
					nNumtmp = 0;
					break;
				}
			}

		}
	}

	if (nNumtmp > 0)
	{
		for (DWORD i = 0; i < m_dwIndexNum; i++)
		{
			int nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;

			pElemtmp = &m_apItem[nId];
			if (pElemtmp->IsEmpty())
			{
				*pElemtmp = *(pElem);
				pElemtmp->m_dwObjId = nId;
				pElemtmp->m_dwObjIndex = i;
#ifdef __CLIENT
				pElemtmp->SetTexture();
#endif
				if (nNumtmp > (short)pItemProp->dwPackMax)
				{
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;
					nNumtmp -= (short)pItemProp->dwPackMax;

					if (pItemObjID)
					{
						pItemObjID[*pCount] = nId;
						pItemQuantity[*pCount] = pElemtmp->m_nItemNum;
						++(*pCount);
					}
				}
				else
				{
					pElemtmp->m_nItemNum = nNumtmp;
					nNumtmp = 0;

					if (pItemObjID)
					{
						pItemObjID[*pCount] = nId;
						pItemQuantity[*pCount] = pElemtmp->m_nItemNum;
						++(*pCount);
					}
					break;
				}
			}
		}
	}
	if (nNumtmp > 0)
		Error("template <class T> BOOL CItemContainer<T>::Add( T* pElem... : nNumtmp > 0");

	return TRUE;
}

template <class T> void CItemContainer<T>::RemoveAtId(OBJID dwObjId)
{
	if (dwObjId >= m_dwItemMax)
		return;
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwItemMax)
		return;

#ifdef __FL_FIX_SHORTCUTCRASH
#ifdef __CLIENT
	CItemElem* pElemt = &m_apItem[dwObjId];
	g_WndMng.CleanGlobalShortcut(pElemt);
#endif
#endif

#ifdef __FL_SWITCH_EQ
	if (m_bInventory && m_pMover)
		m_pMover->m_Switches.FindAndRemove(dwObjId);
#endif

	m_apItem[dwObjId].Empty();
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum)
	{
		m_apIndex[m_apItem[dwObjId].m_dwObjIndex] = NULL_ID;
		m_apItem[dwObjId].m_dwObjIndex = NULL_ID;
	}
}


template <class T> void CItemContainer<T>::SetAtId(OBJID dwObjId, T* pItemElem)
{
	if (dwObjId >= m_dwItemMax)
		return;
	m_apItem[dwObjId] = *pItemElem;
	m_apItem[dwObjId].m_dwObjId = dwObjId;
}

template <class T> T* CItemContainer<T>::GetAt(DWORD dwIndex)
{
	DWORD dwIdx = m_apIndex[dwIndex];
	if (dwIdx == NULL_ID)
		return NULL;

	T* pItemElem = &m_apItem[dwIdx];
	if (pItemElem->IsEmpty())
		return NULL;

	return pItemElem;
}

template<class T> void CItemContainer<T>::Swap2(DWORD dwSrcIndex, DWORD dwDstIndex)
{
	DWORD dwItem = m_apIndex[dwSrcIndex];
	m_apIndex[dwSrcIndex] = m_apIndex[dwDstIndex];
	m_apIndex[dwDstIndex] = dwItem;

	DWORD dwSrcItem = m_apIndex[dwSrcIndex];
	DWORD dwDstItem = m_apIndex[dwDstIndex];
	if (dwSrcItem != NULL_ID)
		m_apItem[dwSrcItem].m_dwObjIndex = dwSrcIndex;
	if (dwDstItem != NULL_ID)
		m_apItem[dwDstItem].m_dwObjIndex = dwDstIndex;
}

template <class T> void CItemContainer<T>::Swap(DWORD dwSrcIndex, DWORD dwDstIndex)
{
	if (dwSrcIndex == dwDstIndex || dwSrcIndex >= m_dwItemMax || dwDstIndex >= m_dwItemMax)
		return;

	CItemElem* pItemElemSrc, * pItemElemDst;
	pItemElemSrc = GetAtId(m_apIndex[dwSrcIndex]);
	pItemElemDst = GetAtId(m_apIndex[dwDstIndex]);

	if (pItemElemSrc && pItemElemDst
		&& pItemElemSrc->m_dwItemId == pItemElemDst->m_dwItemId
		&& pItemElemSrc->m_byFlag == pItemElemDst->m_byFlag
		&& pItemElemSrc->m_dwKeepTime == 0 && pItemElemDst->m_dwKeepTime == 0
		)
	{
		ItemProp* pItemProp;
		int nPackMax;
		if ((pItemProp = pItemElemSrc->GetProp()) && (nPackMax = pItemProp->dwPackMax) > 1)
		{
			short nRemnant = (short)nPackMax - pItemElemDst->m_nItemNum;
			if (nRemnant >= pItemElemSrc->m_nItemNum)
			{
				pItemElemDst->m_nItemNum += pItemElemSrc->m_nItemNum;
				RemoveAtId(m_apIndex[dwSrcIndex]);
			}
			else
			{
				pItemElemDst->m_nItemNum += nRemnant;
				pItemElemSrc->m_nItemNum -= nRemnant;
			}
			return;
		}
	}
	Swap2(dwSrcIndex, dwDstIndex);
}

template <class T> void CItemContainer<T>::Copy(CItemContainer<T>& rItemContainer)
{
	m_dwIndexNum = rItemContainer.m_dwIndexNum;
	memcpy((void*)m_apIndex, (void*)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwItemMax);
	for (u_long i = 0; i < m_dwItemMax; i++)
	{
		m_apItem[i].m_dwObjId = rItemContainer.m_apItem[i].m_dwObjId;
		m_apItem[i].m_dwObjIndex = rItemContainer.m_apItem[i].m_dwObjIndex;
		if (rItemContainer.m_apItem[i].IsEmpty() == FALSE)
			m_apItem[i] = rItemContainer.m_apItem[i];
		else
			m_apItem[i].Empty();
	}
}

template <class T> void CItemContainer<T>::Serialize(CAr& ar)
{
#ifdef __INVENTORY_168_SLOTS
	DWORD    adwObjIndex[(MAX_INVENTORY + MAX_HUMAN_PARTS)];
#else
	DWORD	adwObjIndex[128];
#endif

	unsigned char chSize = 0;

	if (ar.IsStoring())
	{

		ar.Write(m_apIndex, sizeof(DWORD) * m_dwItemMax);

		u_long uOffset = ar.GetOffset();
		ar << chSize;

		for (u_char ch = 0; ch < m_dwItemMax; ch++)
		{
			if (m_apItem[ch].IsEmpty() == FALSE)
			{
				ar << ch;
				m_apItem[ch].Serialize(ar);
				chSize++;
			}
			adwObjIndex[ch] = m_apItem[ch].m_dwObjIndex;
		}

		ar.Write(adwObjIndex, sizeof(DWORD) * m_dwItemMax);

		int nBufSize;
		LPBYTE lpBuf = ar.GetBuffer(&nBufSize);
		*(lpBuf + uOffset) = chSize;
	}
	else
	{
		ar.Read(m_apIndex, sizeof(DWORD) * m_dwItemMax);

		for (u_long i = 0; i < m_dwItemMax; i++)
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for (int i = 0; i < chSize; i++)
		{
			ar >> ch;
			m_apItem[ch].Serialize(ar);
		}

		ar.Read(adwObjIndex, sizeof(DWORD) * m_dwItemMax);
		for (DWORD i = 0; i < m_dwItemMax; i++)
		{
			m_apItem[i].m_dwObjIndex = adwObjIndex[i];
		}
	}
}

#include "mempooler.h"
typedef MemPooler<CItem> CItemPool;


class CItem : public CCtrl
{
private:
#ifdef __CLIENT
	float	m_fGroundY;
	D3DXVECTOR3	 m_vDelta;
#endif // __CLIENT

public:
	CItemElem* m_pItemBase;
	u_long	m_idHolder;
	OBJID	m_idOwn;
	DWORD	m_dwDropTime;
	BOOL	m_bDropMob;
#ifdef __EVENT_MONSTER
	BOOL	m_IdEventMonster;
#endif // __EVENT_MONSTER


public:
	CItem();
	virtual ~CItem();

	void SetOwner(OBJID id);
	DWORD GetItemType() const { return 0; }
	void SetItemBase(CItemElem* pItemBase) { m_pItemBase = pItemBase; }

	CItemElem* GetItemBase() const { return m_pItemBase; }
	ItemProp* GetProp() { return prj.GetItemProp(GetIndex()); }

	virtual void Serialize(CAr& ar);
	virtual void Process();
	virtual void Render(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual void RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff);
	virtual BOOL Read(CFileIO* pFile);

#ifdef __CLIENT
	void SetDelta(float fGroundY, D3DXVECTOR3& vDelta);
#endif // __CLIENT

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	CItemPool* m_pPool;
	void* operator new(size_t nSize) { return CItem::m_pPool->Alloc(); }
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine) { return CItem::m_pPool->Alloc(); }
	void	operator delete(void* lpMem) { CItem::m_pPool->Free((CItem*)lpMem); }
	void	operator delete(void* lpMem, LPCSTR lpszFileName, int nLine) { CItem::m_pPool->Free((CItem*)lpMem); }
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

extern BOOL IsUsableItem(CItemElem* pItem);
extern BOOL IsUsingItem(CItemElem* pItem);
extern BOOL IsNeedTarget(ItemProp* pProp);

#ifdef __WEAPON_RARITY
extern BOOL IsValidRarityItem(DWORD dwIdIk3);
extern BOOL IsValidRarityItem2(DWORD dwIdIk3);
#endif // __WEAPON_RARITY

#endif // !defined(AFX_ITEM_H__80E88B36_BD6B_449B_BE76_34F2B5B77552__INCLUDED_)

