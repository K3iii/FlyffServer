#ifndef __OBJ_H__
#define __OBJ_H__

#pragma once

#include <d3dx9math.h>
#include "Define.h"
#include "Data.h"
#include "Ar.h"
#include <string>
#include <map>
using	namespace	std;

#include "SkillInfluence.h"
#include "UserTaskBar.h"

#if defined(__DBSERVER) || defined(__VPW)
#include "..\DatabaseServer\Project.h"
extern	CProject	prj;
#endif	//__DBSERVER || __VPW

#if defined(__DBSERVER)
#include "dbmanager.h"
#endif

#ifdef __FL_SWITCH_EQ
#ifdef __DBSERVER
#include "Switches.h"
#endif
#endif

#ifdef __FL_MONSTER_ALBUM
#ifdef __DBSERVER
#include "MonsterAlbum.h"
#endif
#endif

#include "rtmessenger.h"

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

#if __VER >= 9	// __PET_0410
#include "pet.h"
#endif	// __PET_0410

#include "Piercing.h"

typedef	DWORD	OBJID;

#define ITYPE_ITEM      0
#define ITYPE_CARD      1
#define ITYPE_CUBE      2
#define ITYPE_PET		3

#ifdef __INVENTORY_168_SLOTS
#define	MAX_INVENTORY	168
#else
#define	MAX_INVENTORY	42
#endif


enum
{
	METHOD_NONE, METHOD_EXCLUDE_ITEM
};


#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL + MAX_MASTER_SKILL + MAX_HERO_SKILL + MAX_LEGEND_HERO_SKILL )

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CObj
{
public:
	DWORD	m_dwType;
	DWORD	m_dwIndex;
	D3DXVECTOR3		m_vScale;
	DWORD	m_dwMotion;
	D3DXVECTOR3		m_vPos;
	FLOAT	m_fAngle;
public:
	//	Constructions
	CObj();
	virtual	~CObj() {}
	//	Operations
	virtual	void	Serialize(CAr& ar);
public:
	static int	m_nMethod;
	static int	GetMethod(void) { return m_nMethod; }

#ifdef __LAYER_1015
protected:
	int		m_nLayer;
public:
	void	SetLayer(int nLayer) { m_nLayer = nLayer; }
	int		GetLayer() { return m_nLayer; }
#endif	// __LAYER_1015
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CCtrl : public CObj
{
public:
	OBJID	m_objid;

public:
	//	Constructions
	CCtrl();
	virtual	~CCtrl() {}
	//	Operations
	void	SetId(OBJID objid) { m_objid = objid; }
	OBJID	GetId(void) { return m_objid; }
	virtual	void	Serialize(CAr& ar);
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItemElem
{
public:
	OBJID       m_dwObjId;
	DWORD		m_dwItemId;
	TCHAR       m_szItemText[32];
	DWORD		m_dwObjIndex;

private:
	SERIALNUMBER	m_liSerialNumber;

public:
	BOOL	IsEmpty() { return m_dwItemId ? FALSE : TRUE; }

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

#ifdef __DBSERVER
	void	SetUltimatePiercingSize(int nSize) { m_piercing.SetUltimatePiercingSize(nSize); }
	int		GetUltimatePiercingSize() { return m_piercing.GetUltimatePiercingSize(); }
	void	SetUltimatePiercingItem(int nth, DWORD dwItem) { m_piercing.SetUltimatePiercingItem(nth, dwItem); }
	DWORD	GetUltimatePiercingItem(int nth) { return m_piercing.GetUltimatePiercingItem(nth); }

	ItemProp* GetProp() { return prj.GetItemProp(m_dwItemId); }
	BOOL	IsPierceAble(DWORD dwTargetItemKind3 = NULL_ID, BOOL bSize = FALSE);
#endif // __DBSERVER

public:
	enum { expired = 0x01, binds = 0x02, isusing = 0x04, };

	int		m_nHitPoint;
	short	m_nItemNum;
	BYTE	m_byFlag;
	BYTE	m_bItemResist;
	int		m_nResistAbilityOption;
	int		m_nAbilityOption;
	DWORD	m_dwKeepTime;

private:
#ifdef __MODEL_CHANGE
	DWORD	m_dwLookId;
#endif
	__int64	m_iRandomOptItemId;

public:
#ifdef __MODEL_CHANGE
	void SetLook(const DWORD dwId) { m_dwLookId = dwId; }
	DWORD GetLook() { return m_dwLookId; }
	bool IsChangedLook() { return m_dwLookId != NULL_ID; }
#endif
	__int64		GetRandomOptItemId(void) { return m_iRandomOptItemId; }
	void	SetRandomOptItemId(__int64 iRandomOptItemId) { m_iRandomOptItemId = iRandomOptItemId; }
	int		GetRandomOpt(void) { return static_cast<int>(m_iRandomOptItemId & 0x00000000000000FF); }

	CPet* m_pPet;

#ifdef __WEAPON_RARITY
public:
	short m_nWeaponRarity;
#endif // __WEAPON_RARITY

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

	//	Operations
	void Empty();
#ifdef __FL_RECORD_BOOK
	int	GetAttrOption();
#endif
	int		GetOption(void) { return m_nAbilityOption; }
	void	SetOption(int nOption) { m_nAbilityOption = nOption; }

	void	ResetFlag(BYTE byFlag) { m_byFlag &= ~byFlag; }
	void	SetFlag(BYTE byFlag) { m_byFlag |= byFlag; }
	BOOL	IsFlag(BYTE byFlag) { return (m_byFlag & byFlag) ? TRUE : FALSE; }

	CItemElem& operator = (CItemElem& ie);
	void	Serialize(CAr& ar);

#ifdef __DBSERVER
	BOOL	IsVisPet() { return (GetProp() && GetProp()->IsVisPet()) || IsTransformVisPet(); }
	void	SetVisKeepTimeSize(int nSize) { m_piercing.SetVisKeepTimeSize(nSize); }
	void	SetVisKeepTime(int nth, time_t tmKeep) { m_piercing.SetVisKeepTime(nth, tmKeep); }
	time_t	GetVisKeepTime(int nth) { return m_piercing.GetVisKeepTime(nth); }
	BOOL	IsTransformVisPet() { return GetProp() && GetProp()->dwItemKind3 == IK3_PET && m_bTranformVisPet; }

	BOOL	m_bTranformVisPet;
#endif // __DBSERVER
};

#ifdef __FL_RECORD_BOOK
inline int CItemElem::GetAttrOption()
{
	int nAttr = (int)m_bItemResist;
	int nOption = m_nResistAbilityOption;
	int nRet = m_nAbilityOption;

	nRet |= (nAttr << 16);
	nRet |= (nOption << 24);
	return nRet;
}
#endif

typedef	struct	tagSKILL
{
	DWORD	dwSkill;
	DWORD	dwLevel;
	void Init() {};
}
SKILL, * LPSKILL;


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

template <class T> class CItemContainer
{
public:
	LPDWORD	m_apIndex;
	DWORD	m_dwIndexNum;
	T* m_apItem;
	DWORD	m_dwItemMax;
#ifdef __FL_SWITCH_EQ
	BOOL m_bInventory;
	CMover* m_pMover;
#endif

public:
	//	Constructions
	CItemContainer();
	virtual	~CItemContainer();
	//	Operations
	void Clear();
	void	SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff);

	BOOL	Add(T* pElem);
	BOOL	IsFull(ItemProp* pItemProp, short nNum);

	T* GetAt(DWORD dwIndex);
	T* GetAtId(OBJID dwObjId);
	T* GetItem(ItemProp* pItemProp, int nAbilityOption);
	T* GetItem(ItemProp* pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption);
	BOOL IsEquip(DWORD dwObjId);
	void RemoveAt(DWORD dwIndex);
	void	RemoveAtId(OBJID dwObjId);

	T* GetEquip(DWORD dwIndex);
	BOOL	UnEquip(DWORD dwIndex);
	void	DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex);
	void	Copy(CItemContainer<T>& rItemContainer);
	void	Serialize(CAr& ar);
};

template <class T> CItemContainer<T>::CItemContainer()
{
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
#ifndef __FL_PACKED_ITEMS
#if __VER >= 9
		SAFE_DELETE(m_apItem[i].m_pPet);
#endif	// __PET_0410
#endif
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum)
		{
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		}
		else
		{
			//			m_apItem[i].m_dwObjIndex	= NULL_ID:
			m_apIndex[i] = NULL_ID;
		}
	}
}

template <class T> void CItemContainer<T>::SetItemContainer(DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra)
{
	m_dwIndexNum = dwItemMax;
	m_dwItemMax = dwItemMax;
	if (dwExtra != 0xffffffff)
		m_dwItemMax += dwExtra;

	SAFE_DELETE_ARRAY(m_apItem);
	SAFE_DELETE_ARRAY(m_apIndex);

	m_apItem = new T[m_dwItemMax];
	m_apIndex = new DWORD[m_dwItemMax];
	for (u_long i = 0; i < m_dwItemMax; i++)
	{
		m_apItem[i].m_dwObjId = i;
		if (i < m_dwIndexNum)
		{
			m_apItem[i].m_dwObjIndex = i;
			m_apIndex[i] = i;
		}
		else
			m_apIndex[i] = NULL_ID;
	}
}

template <class T> BOOL CItemContainer<T>::Add(T* pElem)
{
	ItemProp* pItemProp = prj.GetItemProp(pElem->m_dwItemId);
	if (!pItemProp)
		return FALSE;

	short nNumtmp = (pElem)->m_nItemNum;
	if (IsFull(pItemProp, nNumtmp))
		return FALSE;

	BOOL bPackItem = TRUE;
	if (pItemProp->dwPackMax == 1)
		bPackItem = FALSE;

	CItemElem* pElemtmp;
	int nId;
	if (bPackItem)
	{
		for (DWORD i = 0; i < m_dwIndexNum; i++)
		{
			nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;

			pElemtmp = &m_apItem[nId];

			if (pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax
				&& pElemtmp->m_byFlag == 0)
			{
				if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				{
					nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;
				}
				else
				{
					pElemtmp->m_nItemNum += nNumtmp;
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
			nId = m_apIndex[i];
			if (nId < 0 || nId >= (int)(m_dwItemMax))
				continue;

			pElemtmp = &m_apItem[nId];
			if (pElemtmp->IsEmpty())
			{
				*pElemtmp = *(pElem);
				pElemtmp->m_dwObjId = nId;
				pElemtmp->m_dwObjIndex = i;

				if (nNumtmp > (short)pItemProp->dwPackMax)
				{
					pElemtmp->m_nItemNum = (short)pItemProp->dwPackMax;
					nNumtmp -= (short)pItemProp->dwPackMax;
				}
				else
				{
					pElemtmp->m_nItemNum = nNumtmp;
					nNumtmp = 0;
					break;
				}
			}
		}
	}

	return TRUE;
}


template <class T> BOOL CItemContainer<T>::IsFull(ItemProp* pItemProp, short nNum)
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
		else if (pElemtmp->m_dwItemId == pItemProp->dwID && pElemtmp->m_byFlag == 0)
		{
			if (pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax)
				nNumtmp -= ((short)pItemProp->dwPackMax - pElemtmp->m_nItemNum);
			else
				return FALSE;
		}
	}

	return TRUE;
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

template <class T> T* CItemContainer<T>::GetAtId(OBJID dwObjId)
{
	if (dwObjId >= m_dwItemMax)
		return NULL;

	T* pItemElem = &m_apItem[dwObjId];
	if (pItemElem->IsEmpty())
		return NULL;

	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetItem(ItemProp* pItemProp, int nAbilityOption)
{
	int nId;
	for (int i = 0; i < m_dwItemMax; i++)
	{
		nId = m_apIndex[i];
		if (0 <= nId && m_apItem[nId].IsEmpty() == FALSE)
		{
			T* pItemElem = &m_apItem[nId];
			if (pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption)
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> T* CItemContainer<T>::GetItem(ItemProp* pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption)
{
	int nId;
	for (DWORD i = 0; i < m_dwItemMax; i++)
	{
		nId = m_apIndex[i];
		if (0 <= nId && m_apItem[nId].IsEmpty() == FALSE)
		{
			T* pItemElem = &m_apItem[nId];
			if (pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption && pItemElem->m_bItemResist == nItemResist && pItemElem->m_nResistAbilityOption == nResistAbilityOption)
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> BOOL CItemContainer<T>::IsEquip(DWORD dwObjId)
{
	if (m_apItem[dwObjId].m_dwObjIndex >= m_dwIndexNum)
		return TRUE;
	return FALSE;
}
template <class T> void CItemContainer<T>::RemoveAt(DWORD dwIndex)
{
#ifdef __FL_FIX_SHORTCUTCRASH
#ifdef __CLIENT
	CItemElem* pElemt = &m_apItem[m_apIndex[dwIndex]];
	g_WndMng.CleanGlobalShortcut(pElemt);
#endif
#endif

	m_apItem[m_apIndex[dwIndex]].Empty();
	if (dwIndex >= m_dwIndexNum)
	{
		m_apItem[m_apIndex[dwIndex]].m_dwObjIndex = NULL_ID;
		m_apIndex[dwIndex] = NULL_ID;
	}
	//for( int i = dwIndex; i < m_dwIndexNum - 1; i++ )
	//	m_apIndex[ i ] = m_apIndex[ i + 1 ];
	//m_dwIndexNum--;
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

template <class T> void CItemContainer<T>::DoEquip(DWORD dwSrcIndex, DWORD dwDstIndex)
{
	dwDstIndex += m_dwIndexNum;
	for (int i = 0; i < m_dwItemMax; i++)
	{
		if (m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID)
		{
			m_apIndex[dwDstIndex] = m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex] = i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex = dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex = dwDstIndex;
			return;
		}
	}
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
	DWORD   adwObjIndex[(MAX_INVENTORY + MAX_HUMAN_PARTS)];
#else
	DWORD	adwObjIndex[128];
#endif

	unsigned char chSize = 0;

	if (ar.IsStoring())
	{
		ar.Write(m_apIndex, sizeof(DWORD) * m_dwItemMax);
		u_long uOffset = ar.GetOffset();
		ar << chSize;

		for (u_char ch = 0; ch < m_dwItemMax; ch++)	// 0-504
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
		// Clear
		for (u_int i = 0; i < m_dwItemMax; i++)
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for (u_char i = 0; i < chSize; i++)
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

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItem : public CCtrl
{
public:
	CItemElem* m_pItemBase;

	//	Constructions
	CItem();
	virtual ~CItem();
	//	Operations
	virtual void	Serialize(CAr& ar);
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CAction
{
public:
	DWORD	m_dwState;
	DWORD	m_dwStateFlag;

public:
	CAction()
	{
		m_dwState = 0;
		m_dwStateFlag = 0;
	}
	void	ClearStateFlag() { m_dwStateFlag = 0; }
	void	AddStateFlag(DWORD dwStateFlag) { m_dwStateFlag |= dwStateFlag; }
	DWORD	GetStateFlag() { return m_dwStateFlag; }
	virtual	~CAction() {}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CMover;
class CActionMover : public CAction
{
public:
	CActionMover() {}
	CActionMover(CMover* pMover) {}
	virtual	~CActionMover() {}
	DWORD	GetState(void) { return m_dwState; }
	void	__ForceSetState(DWORD dwState) { m_dwState = dwState; }
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;


	WORD		m_nKillNPCNum[2];
	BYTE		m_bPatrol : 1;
	BYTE		m_bReserve2 : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;
} QUEST, * LPQUEST;

//#ifdef __DBSERVER
#if defined(__DBSERVER)
typedef		MemPooler<CMover>	CMoverPool;
#endif	// __DBSERVER

typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
#ifdef __MODEL_CHANGE
	DWORD dwLook;
	_EQUIP_INFO() : dwId(NULL_ID), nOption(0), byFlag(0), dwLook(NULL_ID)
	{
	}
	bool isLook()
	{
		return dwLook != NULL_ID;
	}
#endif 
}	EQUIP_INFO, * PEQUIP_INFO;

#if __VER >= 11 // __SYS_POCKET
#include "pocket.h"
#endif	// __SYS_POCKET

#ifdef __FL_RECORD_BOOK
#ifdef __DBSERVER
#include "RecordParameters.h"
#endif
#endif

class CMover : public CCtrl
{
private:
	DWORD			m_dwGold;

public:

	TCHAR			m_szCharacterKey[32];
	BOOL			m_bActiveAttack;
	DWORD			m_dwBelligerence;

	int				m_nMovePattern;
	int				m_nMoveEvent;
	int				m_nMoveEventCnt;


	TCHAR			m_szName[32];
	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint;
	DWORD			m_dwSkinSet;
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;

public:
	LONG	GetFlightLv(void) { return (m_nLevel >= 20 ? 1 : 0); }
	BOOL	IsEquipableNPC() { return FALSE; }

	LONG			m_nFxp;
	BYTE			m_bySex;
	DWORD			m_dwRideItemIdx;
	int				m_nFuel;
	LONG			m_nJob;
	BYTE			m_nSlot;
	char			m_szBankPass[5];

	LONG			m_nStr, m_nSta, m_nDex, m_nInt;
	LONG			m_nLevel;
	EXPINTEGER		m_nExp1;
	SKILL			m_aJobSkill[MAX_SKILL_JOB];

	BYTE	m_abUpdateSkill[MAX_SKILL_JOB];

	OBJID			m_idMarkingWorld;
	D3DXVECTOR3		m_vMarkingPos;
	long			m_nRemainGP;
	DWORD			m_dwAuthorization;
	DWORD			m_dwMode;

	CUserTaskBar	m_UserTaskBar;
	CRTMessenger	m_RTMessenger;

	u_long			m_idparty;
	u_long			m_idGuild;

	int				m_nGuildCombatState;
	int				m_nSkillLevel;
	int				m_nSkillPoint;						// SP

	CActionMover* m_pActMover;
	CItemContainer< CItemElem  >	m_Inventory;
#ifdef __FL_CONSUMABLES_STORAGE
	CItemContainer< CItemElem  >	m_Chest;
#endif

	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[3];
	DWORD							m_dwGoldBank[3];

	D3DXVECTOR3		m_vReturnPos;
	DWORD			m_dwReturnWorldID;
	DWORD			m_tmAccFuel;
#ifdef __DBSERVER
#ifdef __JEFF_9_20
	DWORD	m_dwMute;
#else	// __JEFF_9_20
	DWORD			m_dwFlyTime;
#endif	// __JEFF_9_20
	DWORD			m_dwWorldID;
	CMclCritSec		m_AccessLock;
#endif // __DBSERVER

#if __VER >= 15 // __PETVIS
private:
	OBJID	m_objIdVisPet;
	DWORD	m_dwMoverSfxId;
#endif // __PETVIS
#if __VER >= 9	// __PET_0410
private:
	DWORD	m_dwPetId;
public:
	DWORD	GetPetId(void) { return m_dwPetId; }
	void	SetPetId(DWORD dwPetId) { m_dwPetId = dwPetId; }
	CItemElem* GetPetItem(void);
#endif	// __PET_0410
#if __VER >= 9	//__AI_0509
	FLOAT	m_fSpeedFactor;
#endif	// __AI_0509

#if __VER >= 13
	int				m_nHonor;
	int				m_aHonorTitle[MAX_HONOR_TITLE];
	void			SetHonorCount(int nIdx, int nCount) { m_aHonorTitle[nIdx] = nCount; }
#endif

	LONG			m_nPlusMaxHitPoint;
	DWORD			m_dwSMTime[SM_MAX];

#ifdef __BUFF_1107
	CBuffMgr	m_buffs;
#else	// __BUFF_1107
	CSkillInfluence m_SkillState;
#endif	// __BUFF_1107

	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
	void	ClearEquipInfo(void)
	{
		memset(m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS);
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __MODEL_CHANGE
			m_aEquipInfo[i].dwLook = NULL_ID;
#endif
		}
	}

	DWORD			m_dwStateMode;
	OBJID			m_dwUseItemId;
	LPQUEST			m_aQuest;
	LPWORD			m_aCompleteQuest;
	BYTE			m_nQuestSize;
	BYTE			m_nCompleteQuestSize;
	DWORD			m_dwPeriodicTick;
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	LPWORD			m_aCheckedQuest;
	BYTE			m_nCheckedQuestSize;
#endif // __IMPROVE_QUEST_INTERFACE

#ifdef __MODEL_HIDEFASH
	BOOL m_bHideFashion[6];
#endif

#ifdef __FL_SWITCH_EQ
#ifdef __DBSERVER
	CSwitches m_Switches;
#endif
#endif

#ifdef __FL_MONSTER_ALBUM
#ifdef __DBSERVER
	CMonsterAlbum m_MonsterAlbum;
#endif
#endif

#ifdef __FL_RECORD_BOOK
#ifdef __DBSERVER
	std::array<PlayerRecord_Own, MAX_RECORDTYPE> m_arrayRecords;
#endif
#endif


#ifdef __FL_BATTLE_PASS
#ifdef __DBSERVER
private:
	int m_nLastBattlePassID;
	int	m_nBattlePassLevel;
	int	m_nBattlePassExp;
public:
	int GetLastBattlePassID() const { return m_nLastBattlePassID; }
	int GetBattlePassLevel() const { return m_nBattlePassLevel; }
	int GetBattlePassExp() const { return m_nBattlePassExp; }

	void SetLastBattlePassID(const int nValue) { m_nLastBattlePassID = nValue; }
	void SetBattlePassLevel(const int nValue) { m_nBattlePassLevel = nValue; }
	void SetBattlePassExp(const int nValue) { m_nBattlePassExp = nValue; }
#endif
#endif

#if defined (__FL_GW_STATUE_V2) || defined (__FL_VENDORS)
	void SerializeAbnormalNpc(CAr& ar);
#endif

public:
	//	Constructions
	CMover();
	virtual	~CMover();

	//	Operations
	void	InitProp(void);
	int		InitSkillExp(void);
#if defined(__DBSERVER)
	MoverProp* GetProp(void) { return(prj.GetMoverProp(m_dwIndex)); }
#endif	// __DBSERVER

	BOOL	RemoveQuest(int nQuestId);
	virtual void	Serialize(CAr& ar);
	BOOL	IsAbnormalPlayerData();
	BOOL	NormalizePlayerData();
	BOOL	RemoveItemIK3();
	BYTE	GetSex() { return m_bySex; }
	void	SetSex(BYTE bySex) { m_bySex = bySex; }
	int		GetGold();
	void	SetGold(int nGold);
	BOOL	AddGold(int nGold, BOOL bSend = TRUE);

	void	Copy(CMover* pMover, BOOL bCopyAll);
	void	PeriodTick();


#if defined(__DBSERVER)
	void	Lock(void) { m_AccessLock.Enter(); }
	void	Unlock(void) { m_AccessLock.Leave(); }
#ifdef __INVALID_LOGIN_0612
	BOOL	m_bLogout;
	char	m_szAccount[MAX_ACCOUNT];
#endif	// __INVALID_LOGIN_0612
#ifndef __VM_0820
#ifndef __MEM_TRACE
public:
	static CMoverPool* m_pPool;
	void* operator new(size_t nSize) { return CMover::m_pPool->Alloc(); }
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine) { return CMover::m_pPool->Alloc(); }
	void	operator delete(void* lpMem) { CMover::m_pPool->Free((CMover*)lpMem); }
	void	operator delete(void* lpMem, LPCSTR lpszFileName, int nLine) { CMover::m_pPool->Free((CMover*)lpMem); }
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __DBSERVER
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

inline int CMover::GetGold()
{
	if (m_dwGold > INT_MAX)
		Error("GetGold(). UserId:[%07d], Gold:[%d]", m_idPlayer, m_dwGold);

	int nGold = m_dwGold;
	return nGold;
}

inline void CMover::SetGold(int nGold)
{
	if (nGold < 0)
	{
		Error("SetGold(). UserId:[%07d], Gold:[%d]", m_idPlayer, nGold);
		return;
	}
	m_dwGold = (DWORD)nGold;
}

inline BOOL CMover::AddGold(int nGold, BOOL bSend)
{
	__int64 n64Gold = (__int64)nGold;

	if (n64Gold == 0)
		return TRUE;

	__int64 n64Total = static_cast<__int64>(GetGold()) + n64Gold;

	if (n64Total > static_cast<__int64>(INT_MAX))
	{
#ifndef __CLIENT
		Error("Overflow. UserID:[%07d], Gold:[%d]", m_idPlayer, n64Gold);
#endif
		n64Total = static_cast<__int64>(INT_MAX);
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
#endif	// __OBJ_H__