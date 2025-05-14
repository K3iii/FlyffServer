#include "stdafx.h"

#if defined(__DBSERVER) || defined(__CORESERVER) || defined(__ACTIVEXPLAYER) || defined(__VPW)
#include "..\_Network\Objects\Obj.h"
#else
#include "Mover.h"
#endif


void CCtrl::Serialize( CAr & ar )
{
	CObj::Serialize( ar );

	if( ar.IsStoring() )
		ar << m_objid;
	else
		ar >> m_objid;
}


void CItemElem::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_dwObjId << m_dwItemId;
		ar << m_liSerialNumber;
		ar.WriteString(m_szItemText);

		ar << m_nItemNum;
		ar << m_nHitPoint;
		ar << m_byFlag;
		ar << m_nAbilityOption;
		ar << m_bItemResist;
		ar << m_nResistAbilityOption;
		m_piercing.Serialize(ar);
		ar << m_iRandomOptItemId;
		ar << m_dwKeepTime;
		if (m_dwKeepTime)
		{
			time_t t = m_dwKeepTime - time_null();
			ar << t;
		}

		if (m_pPet)
		{
			ar << (BYTE)0x01;
			m_pPet->Serialize(ar);
		}
		else
		{
			ar << (BYTE)0x00;
		}

#ifdef __FL_PACKED_ITEMS
		ar << (u_long)m_vecPackedItems.size();
		for (auto& it : m_vecPackedItems)
			it->Serialize(ar);
#endif

		ar << m_bTranformVisPet;
#ifdef __MODEL_CHANGE
		ar << m_dwLookId;
#endif 
#ifdef __WEAPON_RARITY
		ar << m_nWeaponRarity;
#endif // __WEAPON_RARITY
	}
	else
	{
		ar >> m_dwObjId >> m_dwItemId;
		ar >> m_liSerialNumber;
		ar.ReadString(m_szItemText, 32);

		ar >> m_nItemNum;
		ar >> m_nHitPoint;
		ar >> m_byFlag;
		ar >> m_nAbilityOption;
		ar >> m_bItemResist;
		ar >> m_nResistAbilityOption;
		m_piercing.Serialize(ar);
		ar >> m_iRandomOptItemId;
		ar >> m_dwKeepTime;

		if (m_dwKeepTime)
		{
			time_t t;
			ar >> t;
#ifdef __CLIENT
			m_dwKeepTime = time_null() + t;
#endif	// __CLIENT
		}

		SAFE_DELETE(m_pPet);
		BYTE bPet;
		ar >> bPet;
		if (bPet)
		{
			m_pPet = new CPet;
			m_pPet->Serialize(ar);
		}

#ifdef __FL_PACKED_ITEMS
		ClearPackedItems();
		u_long nSize;
		ar >> nSize;

		for (u_long i = 0; i < nSize; i++)
		{
			CItemElem itemElem;
			itemElem.Serialize(ar);
			AddToPack(&itemElem);
		}
#endif

		ar >> m_bTranformVisPet;
#ifdef __MODEL_CHANGE
		ar >> m_dwLookId;
#endif
#ifdef __WEAPON_RARITY
		ar >> m_nWeaponRarity;
#endif // __WEAPON_RARITY

#ifdef __CLIENT
		SetTexture();
#endif
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void CItem::Serialize( CAr & ar )	// 45
{
	CCtrl::Serialize( ar );		// 33

	if( ar.IsStoring() )
	{
	}
	else
	{
		SAFE_DELETE( m_pItemBase );
		m_pItemBase	= new CItemElem;
	}

	m_pItemBase->Serialize( ar );	
}
