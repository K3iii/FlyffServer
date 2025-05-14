#pragma once
#if __VER >= 12 // __EXT_PIERCING

#if __VER >= 13 // __EXT_ENCHANT
struct __ATTRIBUTE_ENCHANT
{
	int nProb;
	int nAddDamageRate;
#if __VER >= 14 // __EXT_ATTRIBUTE
	int nDefenseRate;
	int nAddAtkDmgRate;
#endif // __EXT_ATTRIBUTE
	__ATTRIBUTE_ENCHANT()
	{
		nProb = 0;
		nAddDamageRate = 0;
#if __VER >= 14 // __EXT_ATTRIBUTE
		nDefenseRate = 0;
		nAddAtkDmgRate = 0;
#endif // __EXT_ATTRIBUTE
	}
};
#endif // __EXT_ENCHANT

class CItemUpgrade
{
public:
	CItemUpgrade(void);
	~CItemUpgrade(void);
	static CItemUpgrade*	GetInstance( void );

	void	LoadScript();
	int		GetSizeProb( CItemElem* pItemElem );
	void	OnPiercing( CUser* pUser, DWORD dwItemId, DWORD dwSocketCard );
	void	OnPiercingRemove(CUser* pUser, DWORD objId, int nSlot);

#if __VER >= 14 // __SMELT_SAFETY
	BYTE	OnSmeltSafety( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
#endif // __SMELT_SAFETY
#if __VER >= 13 // __EXT_ENCHANT
	void	OnEnchant( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeDamageFactor( int nAbilityOption );
#if __VER >= 14 // __EXT_ATTRIBUTE
	int		GetAttributeDefenseFactor( int nAbilityOption );
	int		GetAttributeAddAtkDmgFactor( int nAbilityOption );
#endif // __EXT_ATTRIBUTE
	int		GetMaxGeneralEnchantSize() { return m_mapGeneralEnchant.size(); }
	int		GetMaxAttributeEnchantSize() { return m_mapAttributeEnchant.size(); }
#endif // __EXT_ENCHANT
#ifdef __SYS_ITEMTRANSY
	void	OnItemTransy( CUser* pUser, OBJID objidTarget, OBJID objidTransy, DWORD dwChangeId, BOOL bCash );
private:
	BOOL	RunItemTransy( CUser* pUser, CItemElem* pItemElemTarget, DWORD dwChangeId );
	int		m_nItemTransyLowLevel, m_nItemTransyHighLevel;
#endif // __SYS_ITEMTRANSY

private:
	map<int, int>	m_mapSuitProb;
	map<int, int>	m_mapWeaponProb;

#if __VER >= 14 // __SMELT_SAFETY
private:
	void	RefineAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	void	RefineCollector( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	BYTE	SmeltSafetyNormal( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
	BYTE	SmeltSafetyAccessory( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
	BYTE	SmeltSafetyPiercingSize( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr );
#if __VER >= 15 // __15_5TH_ELEMENTAL_SMELT_SAFETY
	BYTE	SmeltSafetyAttribute(CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial, CItemElem* pItemProtScr, CItemElem* pItemSmeltScr );
#endif // __15_5TH_ELEMENTAL_SMELT_SAFETY
public:
#endif // __SMELT_SAFETY

#if __VER >= 13 // __EXT_ENCHANT
	void	EnchantGeneral( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetGeneralEnchantProb( int nAbilityOption );
	void	EnchantAttribute( CUser* pUser, CItemElem* pItemMain, CItemElem* pItemMaterial );
	int		GetAttributeEnchantProb( int nAbilityOption );

	DWORD	WhatEleCard( DWORD dwItemType );

	map<int, int>	m_mapGeneralEnchant;
	map<int, __ATTRIBUTE_ENCHANT> m_mapAttributeEnchant;
#endif // __EXT_ENCHANT

public:
	void	PetVisSize( CUser* pUser, OBJID objIdMaterial, int nType = TYPE_INVENTORY );
	void	SetPetVisItem(CUser* pUser, OBJID objIdVis, int nType = TYPE_INVENTORY);
	void	RemovePetVisItem( CUser* pUser, int nPosition, BOOL bExpired = FALSE );
#ifndef __FL_FIX_USELESS_LOGS
	void	PutPetVisItemLog( CUser* pUser, const char* szAction, const char* szContext, CItemElem* pItem, int nPosition );
#endif
	void	SwapVis( CUser* pUser, int nPos1, int nPos2 );
	void	ChangeVisPetSfx( CUser* pUser, CItemElem* pItemElemPet );
	void	TransFormVisPet( CUser* pUser, OBJID objIdMaterial, int nType = TYPE_INVENTORY);
};

#endif // __EXT_PIERCING