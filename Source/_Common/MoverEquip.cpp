#include "stdafx.h"
#include "defineText.h"
#include "defineSkill.h"
#include "defineSound.h"
#include "defineItem.h"
#include "ModelObject.h"
#include "authorization.h"
#include "CreateObj.h"
#include "lang.h"
#include "party.h"
#include "guild.h"
#include "guildwar.h"

#ifdef __CLIENT
#include "DPClient.h"
#include "DialogMsg.h"
extern	CDPClient	g_DPlay;
#include "GuildRender.h"
#else	// __CLIENT
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "dpdatabaseclient.h"
#include "ItemScript.h"

extern	CUserMng			g_UserMng;
extern	CWorldMng			g_WorldMng;
extern	CDPSrvr				g_DPSrvr;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
#endif	// __CLIENT

#if __VER >= 11 // __SYS_COLLECTING
#include "accessory.h"
#endif	// __SYS_COLLECTING

extern	CPartyMng		g_PartyMng;
extern	CGuildMng		g_GuildMng;


BYTE  nHairColor[10][3] = {
	{ 144,  99,  101 },
	{ 100, 107, 125 },
	{ 158,119,109 },
	{ 199,156,100 },
	{ 100,123,117 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 },
	{ 100,100,100 }
};


#define TEX_PART_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair01.dds"  ) : _T( "Part_femaleHair01.dds"  ) )
#define TEX_PART_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper01.dds" ) : _T( "Part_femaleUpper01.dds" ) )
#define TEX_PART_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower01.dds" ) : _T( "Part_femaleLower01.dds" ) )

#define PARTSMESH_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.o3d" ) : _T( "Part_femaleHair%02d.o3d" ) )
#define PARTSMESH_HEAD( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHead%02d.o3d" ) : _T( "Part_femaleHead%02d.o3d" ) )
#define PARTSMESH_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper.o3d"    ) : _T( "Part_femaleUpper.o3d"    ) )
#define PARTSMESH_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower.o3d"    ) : _T( "Part_femaleLower.o3d"    ) )
#define PARTSMESH_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.o3d"     ) : _T( "Part_femaleHand.o3d"     ) )
#define PARTSMESH_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.o3d"     ) : _T( "Part_femaleFoot.o3d"     ) )

#define PARTSTEX_HAIR( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHair%02d.dds"  ) : _T( "Part_femaleHair%02d.dds"  ) )
#define PARTSTEX_UPPER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleUpper%02d.dds" ) : _T( "Part_femaleUpper%02d.dds" ) )
#define PARTSTEX_LOWER( nSex ) ( nSex == SEX_MALE ? _T( "Part_maleLower%02d.dds" ) : _T( "Part_femaleLower%02d.dds" ) )
#define PARTSTEX_HAND( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleHand.dds"      ) : _T( "Part_femaleHand.dds"      ) )
#define PARTSTEX_FOOT( nSex )  ( nSex == SEX_MALE ? _T( "Part_maleFoot.dds"      ) : _T( "Part_femaleFoot.dds"      ) )

ItemProp* GetEquipItemProp(CItemContainer<CItemElem>* pInventory, PEQUIP_INFO pEquipInfo, int nParts)
{
	ItemProp* pItemProp = NULL;

	if (pInventory == NULL)
	{
		if (pEquipInfo[nParts].dwId != NULL_ID)
			pItemProp = prj.GetItemProp(pEquipInfo[nParts].dwId);
	}
	else
	{
		CItemElem* pItemElem = pInventory->GetEquip(nParts);
		if (pItemElem)
			pItemProp = pItemElem->GetProp();
	}

	return pItemProp;
}

int GetSelfDST(BYTE type)
{
	switch (type)
	{
	case SAI79::FIRE:
		return DST_RESIST_FIRE;
	case SAI79::WATER:
		return DST_RESIST_WATER;
	case SAI79::WIND:
		return DST_RESIST_WIND;
	case SAI79::ELECTRICITY:
		return DST_RESIST_ELECTRICITY;
	case SAI79::EARTH:
		return DST_RESIST_EARTH;
	}
	return DST_RESIST_FIRE;
}

int GetStrongDST(BYTE type)
{
	switch (type)
	{
	case SAI79::FIRE:
		return DST_RESIST_WIND;
	case SAI79::WATER:
		return DST_RESIST_FIRE;
	case SAI79::WIND:
		return DST_RESIST_EARTH;
	case SAI79::ELECTRICITY:
		return DST_RESIST_WATER;
	case SAI79::EARTH:
		return DST_RESIST_ELECTRICITY;
	}
	return DST_RESIST_FIRE;
}

int GetWeakDST(BYTE type)
{
	switch (type)
	{
	case SAI79::FIRE:
		return DST_RESIST_WATER;
	case SAI79::WATER:
		return DST_RESIST_ELECTRICITY;
	case SAI79::WIND:
		return DST_RESIST_FIRE;
	case SAI79::ELECTRICITY:
		return DST_RESIST_EARTH;
	case SAI79::EARTH:
		return DST_RESIST_WIND;
	}
	return DST_RESIST_FIRE;
}


void CMover::SetHair(int nHair)
{
	if (nHair >= 0 && nHair < MAX_HAIR)
	{
		m_dwHairMesh = nHair;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[64];
		// Hair
		_stprintf(lpszTemp, PARTSMESH_HAIR(GetSex()), m_dwHairMesh + 1);
		pModel->LoadElement(lpszTemp, PARTS_HAIR);
		_stprintf(lpszTemp, PARTSTEX_HAIR(GetSex()), 0 /*nHairColor*/ + 1);
		pModel->ChangeTexture(PARTS_HAIR, TEX_PART_HAIR(GetSex()), lpszTemp);
	}
}

void  CMover::SetHairColor(FLOAT r, FLOAT g, FLOAT b)
{
	if ((r == 1.0f && g == 1.0f && b == 1.0f) ||
		(fabs(r - 1.0f) < 0.001f && fabs(g - 1.0f) < 0.001f && fabs(b - 1.0f) < 0.001f))
	{
		m_fHairColorR = (nHairColor[m_dwHairMesh][0]) / 255.f;
		m_fHairColorG = (nHairColor[m_dwHairMesh][1]) / 255.f;
		m_fHairColorB = (nHairColor[m_dwHairMesh][2]) / 255.f;
	}
	else
	{
		m_fHairColorR = (r < 0.3f) ? 0.3f : r;
		m_fHairColorG = (g < 0.3f) ? 0.3f : g;
		m_fHairColorB = (b < 0.3f) ? 0.3f : b;
	}

	m_dwHairColor = D3DCOLOR_COLORVALUE(m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f);
}


void CMover::SetHairColor(DWORD dwHairColor)
{
	m_dwHairColor = dwHairColor;
	m_fHairColorR = ((dwHairColor >> 16) & 0xff) / 255.f;
	m_fHairColorG = ((dwHairColor >> 8) & 0xff) / 255.f;
	m_fHairColorB = ((dwHairColor) & 0xff) / 255.f;

	if ((m_fHairColorR == 1.0f && m_fHairColorG == 1.0f && m_fHairColorB == 1.0f) ||
		(fabs(m_fHairColorR - 1.0f) < 0.001f && fabs(m_fHairColorG - 1.0f) < 0.001f && fabs(m_fHairColorB - 1.0f) < 0.001f))
	{
		m_fHairColorR = (nHairColor[m_dwHairMesh][0]) / 255.f;
		m_fHairColorG = (nHairColor[m_dwHairMesh][1]) / 255.f;
		m_fHairColorB = (nHairColor[m_dwHairMesh][2]) / 255.f;
	}

	m_dwHairColor = D3DCOLOR_COLORVALUE(m_fHairColorR, m_fHairColorG, m_fHairColorB, 1.0f);
}


void CMover::SetHead(int nHead)
{
	if (nHead < MAX_HEAD)
	{
		m_dwHeadMesh = nHead;
		CModelObject* pModel = (CModelObject*)m_pModel;
		TCHAR lpszTemp[64];
		// Hair
		_stprintf(lpszTemp, PARTSMESH_HEAD(GetSex()), m_dwHeadMesh + 1);
		pModel->LoadElement(lpszTemp, PARTS_HEAD);
	}
}

void CMover::UpdateParts(BOOL bFakeParts)
{
	UpdateParts(GetSex(), m_dwHairMesh, m_dwHeadMesh, m_aEquipInfo, (CModelObject*)m_pModel, bFakeParts ? NULL : &m_Inventory, GetProp()->bIfParts, this);
	((CModelObject*)m_pModel)->RestoreDeviceObjects();
	UpdateBoundBox();
	UpdateParam();
}

void CMover::UpdateParts(int nSex, int nHairMesh, int nHeadMesh, PEQUIP_INFO pEquipInfo, CModelObject* pModel, CItemContainer< CItemElem  >* pInventory, BOOL bIfParts, CMover* pMover)
{
	if (pModel == NULL)
	{
		Error("CMover::UpdateParts : pModel is NULL");
		return;
	}

	TCHAR lpszTemp[64];
	if (bIfParts)
	{
		// Hair
		_stprintf(lpszTemp, PARTSMESH_HAIR(nSex), nHairMesh + 1);
		pModel->LoadElement(lpszTemp, PARTS_HAIR);
		// Head
		_stprintf(lpszTemp, PARTSMESH_HEAD(nSex), nHeadMesh + 1);
		pModel->LoadElement(lpszTemp, PARTS_HEAD);
	}

	BOOL abExclusiveParts[MAX_HUMAN_PARTS];
	ZeroMemory(abExclusiveParts, sizeof(abExclusiveParts));

#ifdef __CLIENT
	if (pMover)
		pMover->m_bExclusiveLowBody = false;
#endif

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (i == PARTS_HAIR || i == PARTS_HEAD)//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;

		ItemProp* pItemProp = GetEquipItemProp(pInventory, pEquipInfo, i);
		if (pItemProp)
		{
			if (pInventory)
			{
				CItemElem* pItemElem = pInventory->GetAt(i + pInventory->GetSize());
				if (pItemElem->IsFlag(CItemElem::expired))
					pItemProp = NULL;
			}
			else
			{
				if (pEquipInfo[i].byFlag & CItemElem::expired)
					pItemProp = NULL;
			}
		}


		if (pItemProp && pItemProp->dwExclusive != NULL_ID)
		{
#ifdef __CLIENT
			if (i == PARTS_CLOTH && pMover)
				pMover->m_bExclusiveLowBody = true;
#endif
			pModel->TakeOffParts(pItemProp->dwExclusive);
			abExclusiveParts[pItemProp->dwExclusive] = TRUE;
		}
	}

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (i == PARTS_HAIR || i == PARTS_HEAD)//|| i == PARTS_CAP || i == PARTS_HAND || i == PARTS_FOOT )
			continue;

		CItemElem* pItemElem = NULL;
		ItemProp* pItemProp = NULL;
		if (pInventory == NULL) // fake only
		{
			if (pEquipInfo[i].dwId != NULL_ID)
			{
				if ((pEquipInfo[i].byFlag & CItemElem::expired) != CItemElem::expired)
				{
#ifdef __MODEL_CHANGE
					pItemProp = pEquipInfo[i].isLook() ? prj.GetItemProp(pEquipInfo[i].dwLook) : prj.GetItemProp(pEquipInfo[i].dwId);
#else
					pItemProp = prj.GetItemProp(pEquipInfo[i].dwId);
#endif
				}
			}
		}
		else
		{
			pItemElem = pInventory->GetEquip(i);
			if (pItemElem)
			{
				if (!pItemElem->IsFlag(CItemElem::expired))
				{
#ifdef __MODEL_CHANGE
					pItemProp = pItemElem->IsChangedLook() ? prj.GetItemProp(pItemElem->GetLook()) : pItemElem->GetProp();
#else
					pItemProp = pItemElem->GetProp();
#endif
				}
			}
		}
		TCHAR szPartsName[128];
		if (pItemProp)
		{
			if (pItemProp->bPartsFile == TRUE)
				prj.m_modelMng.MakePartsName(szPartsName, _T("part"), pItemProp->dwID, nSex);
			else
				prj.m_modelMng.MakeModelName(szPartsName, OT_ITEM, pItemProp->dwID);

			if (bIfParts || (i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD))
			{
				pModel->LoadElement(szPartsName, i);
			}
		}


		if (abExclusiveParts[i] == FALSE)
		{
			switch (i)
			{
			case PARTS_RWEAPON:

				if (pItemProp && (pMover == NULL || (pMover && pMover->IsDisguise() == FALSE)))
				{
					switch (pItemProp->dwItemKind3)
					{
					case IK3_KNUCKLEHAMMER:
						((CModelObject*)pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)pModel)->GetRArmIdx());
						break;

					case IK3_BOW:
						((CModelObject*)pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)pModel)->GetLHandIdx());
						break;
					case IK3_YOYO:
						((CModelObject*)pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx());
						pModel->LoadElement(szPartsName, PARTS_LWEAPON);
						((CModelObject*)pModel)->SetParent(PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx());
						break;

					default:
						((CModelObject*)pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)pModel)->GetRHandIdx());
					}
				}
				else
					pModel->TakeOffParts(i);
				break;
			case PARTS_LWEAPON:

				if (pItemProp && (pMover == NULL || (pMover && pMover->IsDisguise() == FALSE)))
				{
					((CModelObject*)pModel)->SetParent(PARTS_LWEAPON, ((CModelObject*)pModel)->GetLHandIdx());
				}
				else
					pModel->TakeOffParts(i);
				break;
			case PARTS_SHIELD:

				if (pItemProp && (pMover == NULL || (pMover && pMover->IsDisguise() == FALSE)))
					((CModelObject*)pModel)->SetParent(PARTS_SHIELD, ((CModelObject*)pModel)->GetLArmIdx());
				else
					pModel->TakeOffParts(i);
				break;
			case PARTS_UPPER_BODY:
				if (bIfParts)
					if (pItemProp == NULL)
						pModel->LoadElement(PARTSMESH_UPPER(nSex), PARTS_UPPER_BODY);
				break;
			case PARTS_LOWER_BODY:
				if (bIfParts)
					if (pItemProp == NULL)
						pModel->LoadElement(PARTSMESH_LOWER(nSex), PARTS_LOWER_BODY);
				break;
			case PARTS_HAND:
				if (bIfParts)
					if (pItemProp == NULL)
						pModel->LoadElement(PARTSMESH_HAND(nSex), PARTS_HAND);
				break;
			case PARTS_FOOT:
				if (bIfParts)
					if (pItemProp == NULL)
						pModel->LoadElement(PARTSMESH_FOOT(nSex), PARTS_FOOT);
				break;
			default:
				if (bIfParts)
				{
					if (pItemProp == NULL)
						pModel->TakeOffParts(i);
				}

			}
		}
	}
}

void CMover::SetDefaultLowerBody(CModelObject* pModel)
{
	if (pModel != NULL)
		pModel->LoadElement(PARTSMESH_LOWER(m_bySex), PARTS_LOWER_BODY);
}

BOOL CMover::DoEquipMover(CItemElem* pItemElem, DWORD dwPart, BOOL bEquip, const EQUIP_INFO& rEquipInfo, CItemContainer< CItemElem  >* pInventory, PEQUIP_INFO pEquipInfo)
{
	ItemProp* pItemProp = pItemElem ? pItemElem->GetProp() : prj.GetItemProp(rEquipInfo.dwId);
	TCHAR lpszTemp[64];

	if (dwPart == NULL_ID)
		return FALSE;

	if (pItemProp && pItemProp->dwHanded == HD_ONE && dwPart == PARTS_RWEAPON)
	{
		if (bEquip)
		{
			ItemProp* pProp = GetEquipItemProp(pInventory, pEquipInfo, PARTS_RWEAPON);
			if (IsInteriorityJob(JOB_BLADE))
			{
				if (pProp && pProp->dwHanded == HD_ONE)
				{
					if (GetEquipItemProp(pInventory, pEquipInfo, PARTS_SHIELD) == NULL)
						dwPart = PARTS_LWEAPON;
				}
			}

			if (pProp && pProp->dwItemKind3 == IK3_YOYO)
				((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);
		}
	}

#ifndef __WORLDSERVER
	((CModelObject*)m_pModel)->TakeOffParts(dwPart);
#endif

	if (pItemProp && pItemProp->dwItemKind3 == IK3_YOYO)
		((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);

	if (bEquip)
	{
		ItemProp* pHandItemProp = GetEquipItemProp(pInventory, pEquipInfo, PARTS_RWEAPON);
		ItemProp* pLHandItemProp = GetEquipItemProp(pInventory, pEquipInfo, PARTS_LWEAPON);

		if (pItemProp && (pItemProp->dwItemKind3 == IK3_SHIELD
#if __VER >= 16
			|| pItemProp->dwItemKind3 == IK3_ZEMBARUNA
			|| pItemProp->dwItemKind3 == IK3_MAGICBARUNA
#endif
			))
		{
			if (pHandItemProp && pHandItemProp->dwHanded == HD_TWO)
			{
				if (pInventory)
				{
					CItemElem* pItemElemOld = pInventory->GetEquip(PARTS_RWEAPON);
					if (pInventory->UnEquip(PARTS_RWEAPON) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					if (pItemElemOld)
					{
						ItemProp* pOldItemProp = pItemElemOld->GetProp();
						ResetDestParamEquip(pOldItemProp, pItemElemOld);
						g_UserMng.AddDoEquip(this, PARTS_RWEAPON, pItemElemOld, FALSE);
					}
					prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElemOld, FALSE, PARTS_RWEAPON);
#endif 
				}

				if (pEquipInfo)
					pEquipInfo[PARTS_RWEAPON].dwId = NULL_ID;
				((CModelObject*)m_pModel)->TakeOffParts(PARTS_RWEAPON);

				if (pHandItemProp->dwItemKind3 == IK3_YOYO)
					((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);
			}
			else if (pLHandItemProp && pLHandItemProp->dwParts == PARTS_RWEAPON)
			{
				if (pInventory)
				{
					CItemElem* pItemElemOld = pInventory->GetEquip(PARTS_LWEAPON);
					if (pInventory->UnEquip(PARTS_LWEAPON) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					if (pItemElemOld)
					{
						ItemProp* pOldItemProp = pItemElemOld->GetProp();
						ResetDestParamEquip(pOldItemProp, pItemElemOld);
						g_UserMng.AddDoEquip(this, PARTS_LWEAPON, pItemElemOld, FALSE);
					}
#endif
				}

				if (pEquipInfo)
					pEquipInfo[PARTS_LWEAPON].dwId = NULL_ID;
				((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);
			}
		}

		if (pItemProp->dwHanded == HD_TWO)
		{
			if (pHandItemProp && pHandItemProp->dwItemKind3 == IK3_YOYO)
				((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);

			if (pInventory)
			{
				CItemElem* pItemElemOld = pInventory->GetEquip(PARTS_SHIELD);
				if (pItemElemOld)
				{
					if (pInventory->UnEquip(PARTS_SHIELD) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					ItemProp* pOldItemProp = pItemElemOld->GetProp();
					ResetDestParamEquip(pOldItemProp, pItemElemOld);
					g_UserMng.AddDoEquip(this, PARTS_SHIELD, pItemElemOld, FALSE);
#endif
				}

				pItemElemOld = pInventory->GetEquip(PARTS_LWEAPON);
				if (pItemElemOld)
				{
					if (pInventory->UnEquip(PARTS_LWEAPON) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					ItemProp* pOldItemProp = pItemElemOld->GetProp();
					ResetDestParamEquip(pOldItemProp, pItemElemOld);
					g_UserMng.AddDoEquip(this, PARTS_LWEAPON, pItemElemOld, FALSE);
#endif
				}

				pItemElemOld = pInventory->GetEquip(PARTS_RWEAPON);
				if (pItemElemOld)
				{
					if (pInventory->UnEquip(PARTS_RWEAPON) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					ItemProp* pOldItemProp = pItemElemOld->GetProp();
					ResetDestParamEquip(pOldItemProp, pItemElemOld);
					g_UserMng.AddDoEquip(this, PARTS_RWEAPON, pItemElemOld, FALSE);

					prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElemOld, FALSE, PARTS_RWEAPON);
#endif
				}
			}

			if (pEquipInfo)
				pEquipInfo[PARTS_SHIELD].dwId = NULL_ID;
			((CModelObject*)m_pModel)->TakeOffParts(PARTS_SHIELD);

			if (pEquipInfo)
				pEquipInfo[PARTS_LWEAPON].dwId = NULL_ID;
			((CModelObject*)m_pModel)->TakeOffParts(PARTS_LWEAPON);

			if (pEquipInfo)
				pEquipInfo[PARTS_RWEAPON].dwId = NULL_ID;
			((CModelObject*)m_pModel)->TakeOffParts(PARTS_RWEAPON);
		}

		if (pItemProp->dwExclusive != NULL_ID)
		{
			if (pInventory)
			{
				CItemElem* pExclusive = pInventory->GetEquip(pItemProp->dwExclusive);
				if (pExclusive)
				{
					if (pInventory->UnEquip(pItemProp->dwExclusive) == FALSE)
						return FALSE;
#ifdef __WORLDSERVER
					ItemProp* pProp = pExclusive->GetProp();
					ResetDestParamEquip(pProp, pExclusive);
					g_UserMng.AddDoEquip(this, pItemProp->dwExclusive, pExclusive, FALSE);
#endif
				}
			}

			if (pEquipInfo)
				pEquipInfo[pItemProp->dwExclusive].dwId = NULL_ID;
			((CModelObject*)m_pModel)->TakeOffParts(pItemProp->dwExclusive);
		}

		if (pInventory)
		{
			if (dwPart == PARTS_RING1 || dwPart == PARTS_EARRING1)
			{
				DWORD dwElem1 = NULL_ID, dwElem2 = NULL_ID;
				CItemElem* pItemElemTemp1 = pInventory->GetEquip(dwPart);
				CItemElem* pItemElemTemp2 = pInventory->GetEquip(dwPart + 1);
				if (pItemElemTemp1)
					dwElem1 = pItemElemTemp1->m_dwItemId;
				if (pItemElemTemp2)
					dwElem2 = pItemElemTemp2->m_dwItemId;
				if (dwElem1 != NULL_ID && dwElem2 == NULL_ID)
					dwPart = pItemProp->dwPartsub;
			}

			CItemElem* pItemElemOld = pInventory->GetEquip(dwPart);
			if (pItemElemOld)
			{
				if (pInventory->UnEquip(dwPart) == FALSE)
					return FALSE;
#ifdef __WORLDSERVER
				ItemProp* pOldItemProp = pItemElemOld->GetProp();
				ResetDestParamEquip(pOldItemProp, pItemElemOld);
				prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElemOld, FALSE, dwPart);
#endif
			}

			if (pInventory->DoEquip(pItemElem ? pItemElem->m_dwObjIndex : 0, dwPart))
			{
				if (pEquipInfo)
				{
					pEquipInfo[dwPart].dwId = pItemElem->m_dwItemId;
					pEquipInfo[dwPart].nOption = pItemElem->GetAttrOption();
				}

#ifdef __WORLDSERVER
				prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElem, TRUE, dwPart);
#endif
			}
			else
			{
				WriteError("%s %d", __FILE__, __LINE__);
				return FALSE;
			}
		}

		if (pEquipInfo)
		{
			if (dwPart == PARTS_RING1 || dwPart == PARTS_EARRING1)
			{
				DWORD dwElem1 = pEquipInfo[dwPart].dwId;
				DWORD dwElem2 = pEquipInfo[dwPart + 1].dwId;
				if (dwElem1 != NULL_ID && dwElem2 == NULL_ID)
					dwPart = pItemProp->dwPartsub;
			}

			memcpy(&pEquipInfo[dwPart], &rEquipInfo, sizeof(EQUIP_INFO));
		}

#ifndef __WORLDSERVER
		TCHAR szPartsName[128];

#ifdef __MODEL_CHANGE
		DWORD dwLook = (pItemElem ? pItemElem->GetLook() : rEquipInfo.dwLook);
		if (dwLook != NULL_ID)
		{
			ItemProp* pProp = prj.GetItemProp(dwLook);
			if (pProp->bPartsFile)
				prj.m_modelMng.MakePartsName(szPartsName, _T("part"), pProp->dwID, GetSex());
			else
				prj.m_modelMng.MakeModelName(szPartsName, OT_ITEM, pProp->dwID);
		}
		else
#endif
		{
			if (pItemProp->bPartsFile)
				prj.m_modelMng.MakePartsName(szPartsName, _T("part"), pItemProp->dwID, GetSex());
			else
				prj.m_modelMng.MakeModelName(szPartsName, OT_ITEM, pItemProp->dwID);

		}

		switch (dwPart)
		{
		case PARTS_RWEAPON:
			if (IsDisguise() == FALSE)
			{
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
				switch (pItemProp->dwItemKind3)
				{
				case IK3_KNUCKLEHAMMER:
					((CModelObject*)m_pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)m_pModel)->GetRArmIdx());
					break;
				case IK3_BOW:
					((CModelObject*)m_pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)m_pModel)->GetLHandIdx());
					break;
				case IK3_YOYO:
					((CModelObject*)m_pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)m_pModel)->GetRHandIdx());
					((CModelObject*)m_pModel)->LoadElement(szPartsName, PARTS_LWEAPON);
					((CModelObject*)m_pModel)->SetParent(PARTS_LWEAPON, ((CModelObject*)m_pModel)->GetLHandIdx());
					break;
				default:
					((CModelObject*)m_pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)m_pModel)->GetRHandIdx());
				}
			}
			break;
		case PARTS_LWEAPON:
			if (IsDisguise() == FALSE)
			{
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
				((CModelObject*)m_pModel)->SetParent(PARTS_LWEAPON, ((CModelObject*)m_pModel)->GetLHandIdx());
			}
			break;
		case PARTS_SHIELD:
			if (IsDisguise() == FALSE)
			{
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
				((CModelObject*)m_pModel)->SetParent(PARTS_SHIELD, ((CModelObject*)m_pModel)->GetLArmIdx());
			}
			break;
		case PARTS_UPPER_BODY:
			if (GetProp()->bIfParts)
			{
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
				_stprintf(lpszTemp, PARTSTEX_UPPER(GetSex()), m_dwSkinSet + 1);
				((CModelObject*)m_pModel)->ChangeTexture(PARTS_UPPER_BODY, TEX_PART_UPPER(GetSex()), lpszTemp);
			}
			break;
		case PARTS_LOWER_BODY:
			if (GetProp()->bIfParts)
			{
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
				_stprintf(lpszTemp, PARTSTEX_LOWER(GetSex()), m_dwSkinSet + 1);
				((CModelObject*)m_pModel)->ChangeTexture(PARTS_LOWER_BODY, TEX_PART_LOWER(GetSex()), lpszTemp);
			}
			break;
		default:
			if (GetProp()->bIfParts)
				((CModelObject*)m_pModel)->LoadElement(szPartsName, dwPart);
			break;

		}

		if (PARTS_LWEAPON == dwPart || PARTS_RWEAPON == dwPart || PARTS_SHIELD == dwPart)
		{
			if (m_pActMover && (!m_pActMover->IsActAttack() && !m_pActMover->IsActJump()))
				SetMotion(MTI_STAND, ANILOOP_1PLAY, MOP_NO_TRANS);
		}
#endif
	}
	else // bEquip == FALSE
	{
		if (GetProp()->bIfParts)
		{
			if (pItemProp->dwExclusive != NULL_ID)
			{
				switch (pItemProp->dwExclusive)
				{
				case PARTS_UPPER_BODY:
					((CModelObject*)m_pModel)->LoadElement(PARTSMESH_UPPER(GetSex()), PARTS_UPPER_BODY);
					_stprintf(lpszTemp, PARTSTEX_UPPER(GetSex()), m_dwSkinSet + 1);
					((CModelObject*)m_pModel)->ChangeTexture(PARTS_UPPER_BODY, TEX_PART_UPPER(GetSex()), lpszTemp);
					break;
				case PARTS_LOWER_BODY:
				{
					ItemProp* pItemPropEquip = NULL;
					if (dwPart == PARTS_CLOTH)
						pItemPropEquip = GetEquipItemProp(pInventory, pEquipInfo, PARTS_UPPER_BODY);
					else
						pItemPropEquip = GetEquipItemProp(pInventory, pEquipInfo, PARTS_CLOTH);

					if (!pItemPropEquip)
					{
						((CModelObject*)m_pModel)->LoadElement(PARTSMESH_LOWER(GetSex()), PARTS_LOWER_BODY);
						_stprintf(lpszTemp, PARTSTEX_LOWER(GetSex()), m_dwSkinSet + 1);
						((CModelObject*)m_pModel)->ChangeTexture(PARTS_LOWER_BODY, TEX_PART_LOWER(GetSex()), lpszTemp);
					}
				}
				break;
				case PARTS_HAND:
					((CModelObject*)m_pModel)->LoadElement(PARTSMESH_HAND(GetSex()), PARTS_HAND);
					break;
				case PARTS_FOOT:
					((CModelObject*)m_pModel)->LoadElement(PARTSMESH_FOOT(GetSex()), PARTS_FOOT);
					break;
				}
			}
		}
		if (pInventory && pInventory->GetEquip(dwPart))
		{
			if (pInventory->UnEquip(dwPart) == FALSE)
			{
#ifdef __WORLDSERVER
				if (IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_LACKSPACE, "");
#endif
				return FALSE;
			}

#ifdef __WORLDSERVER
			prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElem, FALSE, dwPart);
#endif
		}

		if (pEquipInfo)
		{
			pEquipInfo[dwPart].dwId = NULL_ID;
			pEquipInfo[dwPart].nOption = 0;
			pEquipInfo[dwPart].byFlag = 0;
#ifdef __MODEL_CHANGE
			pEquipInfo[dwPart].dwLook = NULL_ID;
#endif
		}

#ifndef __WORLDSERVER
		if (GetProp()->bIfParts)
		{
			switch (dwPart)
			{
			case PARTS_UPPER_BODY:
				((CModelObject*)m_pModel)->LoadElement(PARTSMESH_UPPER(GetSex()), PARTS_UPPER_BODY);
				_stprintf(lpszTemp, PARTSTEX_UPPER(GetSex()), m_dwSkinSet + 1);
				((CModelObject*)m_pModel)->ChangeTexture(PARTS_UPPER_BODY, TEX_PART_UPPER(GetSex()), lpszTemp);
				break;
			case PARTS_LOWER_BODY:
				((CModelObject*)m_pModel)->LoadElement(PARTSMESH_LOWER(GetSex()), PARTS_LOWER_BODY);
				_stprintf(lpszTemp, PARTSTEX_LOWER(GetSex()), m_dwSkinSet + 1);
				((CModelObject*)m_pModel)->ChangeTexture(PARTS_LOWER_BODY, TEX_PART_LOWER(GetSex()), lpszTemp);
				break;
			case PARTS_HAND:
				((CModelObject*)m_pModel)->LoadElement(PARTSMESH_HAND(GetSex()), PARTS_HAND);
				break;
			case PARTS_FOOT:
				((CModelObject*)m_pModel)->LoadElement(PARTSMESH_FOOT(GetSex()), PARTS_FOOT);
				break;
			}
		}

		if (PARTS_LWEAPON == dwPart || PARTS_RWEAPON == dwPart || PARTS_SHIELD == dwPart)
		{
			if (m_pActMover && (!m_pActMover->IsActAttack() && !m_pActMover->IsActJump()))
				SetMotion(MTI_STAND, ANILOOP_1PLAY, MOP_FIXED);
		}
#endif

		if (dwPart == PARTS_RWEAPON)
		{
			if (pInventory)
			{
				CItemElem* pItemLeft = pInventory->GetEquip(PARTS_LWEAPON);
				if (pItemLeft)
				{
					int nInvenSize = pInventory->GetSize();
					pInventory->Swap(nInvenSize + PARTS_LWEAPON, nInvenSize + PARTS_RWEAPON);
#ifdef __WORLDSERVER
					prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemLeft, TRUE, PARTS_RWEAPON);
#endif
				}
			}

			if (pEquipInfo)
			{
				if (pEquipInfo[PARTS_LWEAPON].dwId != NULL_ID)
				{
					memcpy(&pEquipInfo[PARTS_RWEAPON], &pEquipInfo[PARTS_LWEAPON], sizeof(EQUIP_INFO));
					pEquipInfo[PARTS_LWEAPON].dwId = NULL_ID;
					pEquipInfo[PARTS_LWEAPON].nOption = 0;
					pEquipInfo[PARTS_LWEAPON].byFlag = 0;
#ifdef __MODEL_CHANGE
					pEquipInfo[PARTS_LWEAPON].dwLook = NULL_ID;
#endif 
				}
			}

#ifdef __CLIENT
			((CModelObject*)m_pModel)->MovePart(PARTS_RWEAPON, PARTS_LWEAPON);
			((CModelObject*)m_pModel)->SetParent(PARTS_RWEAPON, ((CModelObject*)m_pModel)->GetRHandIdx());
#endif
		}
		}

#ifdef __CLIENT
	if (IsActiveMover()) // __FL_SWITCH_EQ : enable equip sounds only for active player, not for all near players. DoEquip() function in classic v15 source.
	{
		D3DXVECTOR3 vPos = GetPos();

		if (bEquip)
		{
			if (pItemProp)
			{
				if (pItemProp->dwItemKind1 == IK1_WEAPON)
					PLAYSND(SND_INF_EQUIPWEAPON, &vPos);
				else if (pItemProp->dwItemKind1 == IK1_GENERAL && pItemProp->dwItemKind2 == IK2_JEWELRY)
					PLAYSND(SND_INF_EQUIPACCESSORY, &vPos);
				else if (pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_CLOTH)
					PLAYSND(SND_INF_EQUIPCLOTH, &vPos);
				else if (pItemProp->dwItemKind1 == IK1_ARMOR && pItemProp->dwItemKind2 == IK2_ARMOR
					|| pItemProp->dwItemKind2 == IK2_ARMORETC)
				{
					PLAYSND(SND_INF_EQUIPARMOR, &vPos);
				}
			}
		}
		else
			PLAYSND(SND_INF_INVENTORYDROP, &vPos);
	}
#endif

	return TRUE;
	}

BOOL CMover::IsEquipAble(CItemElem* pItem, BOOL bIgnoreLevel)
{
	if (!pItem)
		return FALSE;

	ItemProp* pItemProp = pItem->GetProp();
	if (!pItemProp)
		return FALSE;

	OBJID dwObjid = pItem->m_dwObjId;

	if (pItemProp->dwParts == PARTS_RIDE)
	{
		int nLimitLv = pItemProp->dwFlightLimit;
		if (nLimitLv == NULL_ID)
			nLimitLv = 1;

		CWorld* pWorld = GetWorld();
		if (pWorld)
		{
			int nAttr = pWorld->GetHeightAttribute(GetPos().x, GetPos().z);

			if (GetFlightLv() < nLimitLv)
			{
#ifdef __WORLDSERVER
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_USEAIRCRAFT, "");
#else // __WORLDSERVER
				g_WndMng.PutString(prj.GetText(TID_GAME_USEAIRCRAFT), NULL, prj.GetTextColor(TID_GAME_USEAIRCRAFT));
#endif // __WORLDSERVER
				return FALSE;
			}

			if (!pWorld->m_bFly)
			{
#ifdef __WORLDSERVER
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_ERROR_NOFLY, "");
#else // __WORLDSERVER
				g_WndMng.PutString(prj.GetText(TID_ERROR_NOFLY), NULL, prj.GetTextColor(TID_ERROR_NOFLY));
#endif // __WORLDSERVER
				return FALSE;
			}

			if (HasBuffByIk3(IK3_TEXT_DISGUISE))
			{
#ifdef __WORLDSERVER
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_QUEST_DISQUISE_NOTFLY, "");
#else // __WORLDSERVER
				g_WndMng.PutString(prj.GetText(TID_QUEST_DISQUISE_NOTFLY), NULL, prj.GetTextColor(TID_QUEST_DISQUISE_NOTFLY));
#endif // __WORLDSERVER
				return FALSE;
			}

			if (nAttr == HATTR_NOFLY)
				return FALSE;

#ifdef __WORLDSERVER
			if (m_pActMover->IsState(OBJSTA_STAND) == FALSE)
				return FALSE;
#endif
		}

#ifdef __WORLDSERVER
#ifdef __AIO_PETS
		DisableAllPets();
#else
		if (HasActivatedEatPet() || HasActivatedSystemPet())
		{
			((CUser*)this)->AddDefinedText(TID_GAME_CANNOT_FLY_WITH_PET);
			return FALSE;
		}
#endif
#endif	// __WORLDSERVER
	}

	{
		if (pItemProp->dwItemSex != NULL_ID && pItemProp->dwItemSex != GetSex())
		{
#ifdef __WORLDSERVER
			if (TRUE == IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGSEX, "\"%s\"", pItemProp->szName);
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if (pItemProp->dwItemKind1 != IK1_WEAPON)
	{
		if (pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob(pItemProp->dwItemJob)) {
#ifdef __WORLDSERVER
			if (TRUE == IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName);
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if (pItemProp->dwItemKind1 == IK1_WEAPON)
	{
		if (pItemProp->dwItemJob != NULL_ID && FALSE == IsInteriorityJob(pItemProp->dwItemJob))
		{
#ifdef __WORLDSERVER
			if (TRUE == IsPlayer())
				((CUser*)this)->AddDefinedText(TID_GAME_WRONGJOB, "\"%s\"", pItemProp->szName);
#endif	// __WORLDSERVER
			return FALSE;
		}
	}

	if (!bIgnoreLevel)
	{
		if (pItemProp->dwLimitLevel1 != 0xffffffff)
		{
			if (pItem->IsLimitLevel(this))
			{
#ifdef __WORLDSERVER
				if (TRUE == IsPlayer())
					((CUser*)this)->AddDefinedText(TID_GAME_REQLEVEL, "\"%d\"", pItem->GetLimitLevel());
#endif	// __WORLDSERVER
				return FALSE;
			}
		}

		if (pItemProp->dwItemKind2 == IK2_JEWELRY)
		{
			if (pItemProp->dwLimitLevel1 != 0xffffffff)
			{
				if (pItem->IsLimitLevel(this))
				{
#ifdef __WORLDSERVER
					if (TRUE == IsPlayer())
						((CUser*)this)->AddDefinedText(TID_GAME_REQLEVEL, "\"%d\"", pItem->GetLimitLevel());
#endif	// __WORLDSERVER
					return FALSE;
				}
			}
		}

	}

	ItemProp* pHandItemProp = NULL;

	CItemElem* pItemElem = m_Inventory.GetEquip(PARTS_RWEAPON);
	if (pItemElem)
		pHandItemProp = pItemElem->GetProp();

	if (pItemProp->dwItemKind3 == IK3_ARROW && (pHandItemProp == NULL || pHandItemProp->dwItemKind3 != IK3_BOW))
		return FALSE;

	return TRUE;
}

BOOL CMover::IsUnEquipAble(ItemProp* pItemProp)
{
	if (pItemProp->dwParts == PARTS_RIDE)
	{
		if (GetWorld())
		{
			int nAttr = GetWorld()->GetHeightAttribute(GetPos().x, GetPos().z);
			if ((nAttr == HATTR_NOFLY || nAttr == HATTR_NOWALK || nAttr == HATTR_NOMOVE))
				return FALSE;
		}
	}

	return TRUE;
}

// Equip item for pUser / g_pPlayer, so only active mover (me) with known m_Inventory
void CMover::DoEquipItem(CItemElem* pItemElem)
{
	if (IsDie())
		return;

	if (!pItemElem)
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	DWORD dwPart = pItemProp->dwParts;
	BOOL bEquip = !m_Inventory.IsEquip(pItemElem->m_dwObjId);
	if (!bEquip)
		dwPart = pItemElem->m_dwObjIndex - m_Inventory.m_dwIndexNum;

	if (bEquip)
	{
		if (dwPart == PARTS_RIDE)
		{
			if (m_Inventory.GetEquip(PARTS_RIDE))
				return;

			if (pItemElem->IsFlag(CItemElem::expired))
				return;
		}

#ifdef __WORLDSERVER
		if (!IsItemRedyTime(pItemElem->GetProp(), pItemElem->m_dwObjId, TRUE))
			return;
#endif

		if (IsEquipAble(pItemElem) == FALSE)
			return;
	}
	else
	{
		if (IsUnEquipAble(pItemProp) == FALSE)
			return;
	}

	EQUIP_INFO equipInfo;
	equipInfo.dwId = pItemElem->m_dwItemId;
	equipInfo.nOption = pItemElem->GetAttrOption();
	equipInfo.byFlag = pItemElem->m_byFlag;
	BOOL bResult = DoEquipMover(pItemElem, dwPart, bEquip, equipInfo, &m_Inventory, m_aEquipInfo);

	if (!bResult)
		return;

#ifdef __WORLDSERVER
	if (bEquip)
	{
		SetDestParamEquip(pItemProp, pItemElem);
		if ((pItemProp->dwFlag & IP_FLAG_EQUIP_BIND) && !pItemElem->IsFlag(CItemElem::binds))
		{
			pItemElem->SetFlag(CItemElem::binds);
			UpdateItem((BYTE)(pItemElem->m_dwObjId), UI_FLAG, MAKELONG(pItemElem->m_dwObjIndex, pItemElem->m_byFlag));
		}
	}
	else
	{
		ResetDestParamEquip(pItemProp, pItemElem);
		if (pItemProp->dwItemKind1 == IK1_WEAPON)
		{
			if (m_pActMover->IsStateFlag(OBJSTAF_COMBAT))
				SendActMsg(OBJMSG_MODE_PEACE);
		}

	}
#endif	// 

	if (dwPart == PARTS_RIDE)
	{
		if (bEquip)
		{
#ifdef __CLIENT
			if (IsPlayer())
			{
				PlayMusic(BGM_TH_FLYING);
				g_WndMng.PutString("Select player and press `Z` button to follow.");
				g_WndMng.m_bAutoRun = FALSE;
			}
#endif//
			SendActMsg(OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1);
		}
		else
			SendActMsg(OBJMSG_MODE_GROUND, 0, 0, 1);

		ClearDest();
	}

	UpdateBoundBox();
	UpdateParam();

#ifdef __WORLDSERVER
	g_UserMng.AddDoEquip(this, dwPart, pItemElem, bEquip);
#endif
}


#ifdef __CLIENT
BOOL CMover::DoEquipItemOther(const EQUIP_INFO& rEquipInfo, BOOL bEquip, DWORD dwPart)
{
	BOOL bResult = DoEquipMover(NULL, dwPart, bEquip, rEquipInfo, NULL, m_aEquipInfo);
	if (!bResult)
		return FALSE;

	ItemProp* pProp = prj.GetItemProp(rEquipInfo.dwId);
	if (pProp && dwPart == PARTS_RIDE)
	{
		if (bEquip)
			SendActMsg(OBJMSG_MODE_FLY, pProp->dwID, 0, 1);
		else
			SendActMsg(OBJMSG_MODE_GROUND, 0, 0, 1);

		ClearDest();
	}

	UpdateBoundBox();
	UpdateParam();

	return bResult;
}
#endif // CLIENT

void CMover::RedoEquip(BOOL fFakeParts, BOOL bDestParam)
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;
	BOOL bEquip;

	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		bEquip = fFakeParts ? (m_aEquipInfo[i].dwId != NULL_ID) : (m_Inventory.GetEquip(i) != NULL);
		if (bEquip)
		{
			pItemElem = NULL;
			pItemProp = NULL;

			if (!fFakeParts)
			{
				pItemElem = m_Inventory.GetEquip(i);
				if (NULL != pItemElem)
					pItemProp = pItemElem->GetProp();
			}
			else
				pItemProp = prj.GetItemProp(m_aEquipInfo[i].dwId);

			if ((pItemProp != NULL && pItemProp->dwID > 0 &&
				(pItemProp->dwParts == i || pItemProp->dwPartsub == i ||
					(pItemProp->dwParts == PARTS_RWEAPON && i == PARTS_LWEAPON))
				) &&
				(fFakeParts || m_Inventory.IsEquip(pItemElem->m_dwObjId)))
			{

				if (i == PARTS_RIDE)
				{
					if (FALSE == m_pActMover->IsFly())
						SendActMsg(OBJMSG_MODE_FLY, pItemProp->dwID, 0, 1);

					if (m_dwRideItemIdx == 0)
						m_dwRideItemIdx = pItemProp->dwID;
				}

				if (pItemElem)
					m_aEquipInfo[i].nOption = pItemElem->GetAttrOption();

				if (bDestParam)
				{
					SetDestParamEquip(pItemProp, pItemElem, TRUE);
					if (i == PARTS_RWEAPON)
						prj.m_UltimateWeapon.SetDestParamUltimate(this, pItemElem, TRUE, PARTS_RWEAPON);
				}
			}
			else
				m_aEquipInfo[i].dwId = NULL_ID;
		}
		else
		{
			if (i == PARTS_RIDE)
			{
				if (m_pActMover->IsFly())
					SendActMsg(OBJMSG_MODE_GROUND, 0, 0, 1);

				if (m_dwRideItemIdx != 0)
					m_dwRideItemIdx = 0;

				if (m_Inventory.GetEquip(i))
					m_Inventory.UnEquip(i);
			}
		}
	}


	pItemElem = m_Inventory.GetEquip(PARTS_RWEAPON);
	if (pItemElem)
		m_aEquipInfo[PARTS_RWEAPON].nOption = pItemElem->GetAttrOption();

	pItemElem = m_Inventory.GetEquip(PARTS_LWEAPON);
	if (pItemElem)
		m_aEquipInfo[PARTS_LWEAPON].nOption = pItemElem->GetAttrOption();

	UpdateParts(fFakeParts);

#ifdef __WORLDSERVER
	InvalidEquipOff();
#endif

	if (bDestParam)
	{
		int nAbilityOption = GetSetItem();
		if (nAbilityOption > 0)
			SetSetItemAvail(nAbilityOption);

		SetDestParamSetItem(NULL);
	}
}

#ifdef __WORLDSERVER
int	CMover::InvalidEquipOff()
{
	CItemElem* pItemElem = NULL;
	ItemProp* pItemProp = NULL;
	int		i;

	for (i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		pItemElem = NULL;
		pItemProp = NULL;

		pItemElem = m_Inventory.GetEquip(i);
		if (pItemElem)
			pItemProp = pItemElem->GetProp();

		if (pItemProp == NULL)
			continue;

		if (IsEquipAble(pItemElem, TRUE) == FALSE)
			DoEquipItem(pItemElem);
	}

	return FALSE;
}
#endif


void CMover::SetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem)
{
	if (pItemElem && pItemElem->IsFlag(CItemElem::expired))
		return;

#ifdef __WORLDSERVER
	RunItemScript(this, pItemProp->dwID, ITEM_OP_EQUIP, NULL);
#endif // __WORLDSERVER
#ifdef __WEAPON_RARITY
	// if (pItemElem && (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3)))
	// {
	// 	float fFactor = 0.0f;
	// 	int nFlatBonus = 0;
	// 	if (pItemElem->m_nWeaponRarity > 0)
	// 	{
	// 		switch (pItemElem->m_nWeaponRarity)
	// 		{
	// 			case 5: fFactor = 0.25f; nFlatBonus = 50; 
	// 					if(pItemProp->dwItemKind3 == IK3_SUIT)
	// 						SetDestParam(DST_HP_MAX_RATE, 50, NULL_CHGPARAM);
	// 					break; // Legendary
    //             case 4: fFactor = 0.2f;  nFlatBonus = 40; break; // Epic
    //             case 3: fFactor = 0.15f; nFlatBonus = 30; break; // Unique
    //             case 2: fFactor = 0.1f;  nFlatBonus = 20; break; // Rare
    //             case 1: fFactor = 0.05f; nFlatBonus = 10; break; // Common
	// 		}

	// 		// Apply general bonuses
	// 		for (int i = 0; i < PROP_BONUSES; i++)
	// 		{
	// 			if (pItemProp->dwDestParam[i] != -1)
	// 			{
	// 				int nStats = (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor);
	// 				if (nStats > 0)
	// 					SetDestParam(pItemElem->GetProp()->dwDestParam[i], nStats, NULL_CHGPARAM);
	// 			}
	// 		}

	// 		// Apply additional attribute bonuses
	// 		 int nTotalBonus = nFlatBonus; // You can add scaling here if you want

    //         // If you want to add scaling based on something, e.g. weapon level, add to nTotalBonus
	// 		#ifdef __WORLDSERVER
	// 		WriteLog("Applying flat bonus: %s, rarity=%d, STR+%d", pItemProp->szName, pItemElem->m_nWeaponRarity, nTotalBonus);
	// 		#endif
    //         // Apply to all stats at once, like DST_STAT_ALLUP
    //         SetDestParam(DST_STR, nTotalBonus, NULL_CHGPARAM);
    //         SetDestParam(DST_DEX, nTotalBonus, NULL_CHGPARAM);
    //         SetDestParam(DST_STA, nTotalBonus, NULL_CHGPARAM);
    //         SetDestParam(DST_INT, nTotalBonus, NULL_CHGPARAM);
	// 	}
	// }
	if (pItemElem && (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3)|| IsValidRarityItem2(pItemElem->GetProp()->dwItemKind3)))
	{
		float fFactor = 0.0f;
		int nFlatBonus = 0;
		if (pItemElem->m_nWeaponRarity > 0)
		{
			switch (pItemElem->m_nWeaponRarity)
			{
				case 5: fFactor = 0.25f; nFlatBonus = 50; break; // Legendary
                case 4: fFactor = 0.2f;  nFlatBonus = 40; break; // Epic
                case 3: fFactor = 0.15f; nFlatBonus = 30; break; // Unique
                case 2: fFactor = 0.1f;  nFlatBonus = 20; break; // Rare
                case 1: fFactor = 0.05f; nFlatBonus = 10; break; // Common
			}

			// Apply general bonuses
			for (int i = 0; i < PROP_BONUSES; i++)
			{
				if (pItemProp->dwDestParam[i] != -1)
				{
					int nStats = (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor);
					if (nStats > 0)
						SetDestParam(pItemElem->GetProp()->dwDestParam[i], nStats, NULL_CHGPARAM);
				}
			}

			// Apply additional attribute bonuses
			 int nTotalBonus = nFlatBonus; // You can add scaling here if you want

            // If you want to add scaling based on something, e.g. weapon level, add to nTotalBonus
#ifdef __WORLDSERVER
WriteLog("Applying flat bonus: %s, rarity=%d, STR+%d", pItemProp->szName, pItemElem->m_nWeaponRarity, nTotalBonus);
#endif
            // Apply to all stats at once, like DST_STAT_ALLUP
            SetDestParam(DST_STR, nTotalBonus, NULL_CHGPARAM);
            SetDestParam(DST_DEX, nTotalBonus, NULL_CHGPARAM);
            SetDestParam(DST_STA, nTotalBonus, NULL_CHGPARAM);
            SetDestParam(DST_INT, nTotalBonus, NULL_CHGPARAM);
		}
	}
#endif // __WEAPON_RARITY

	for (int i = 0; i < PROP_BONUSES; i++)
	{
		if (pItemProp->dwDestParam[i] != -1)
			SetDestParam(i, pItemProp);
	}

	UpdateElementResist(pItemElem, TRUE);

	CAccessoryProperty* pProperty = CAccessoryProperty::GetInstance();
	if (pItemElem && pItemElem->IsAccessory())
	{
		vector<SINGLE_DST>* pDst = pProperty->GetDst(pItemElem->m_dwItemId, pItemElem->GetAbilityOption());
		for (DWORD i = 0; i < pDst->size(); i++)
			SetDestParam((*pDst)[i].nDst, (*pDst)[i].nAdj, NULL_CHGPARAM);
	}

#ifdef __WORLDSERVER
	if (pItemProp->IsGiftBoxItem())
		static_cast<CUser*>(this)->AddBuff(BUFF_EQUIP, (WORD)(pItemProp->dwID), 1, 999999999);
#endif // __WORLDSERVER

	if (!bIgnoreSetItem)
	{
		if (pItemElem)
		{
			int nAbilityOption = GetSetItem(pItemElem);
			if (nAbilityOption > 0)
				SetSetItemAvail(nAbilityOption);
			SetDestParamSetItem(pItemElem);
		}
	}

	if (pItemElem && !pItemElem->IsFlag(CItemElem::expired))
	{
		DestParamPiercingAvail(pItemElem, TRUE);
		SetDestParamRandomOpt(pItemElem);
	}
}

#ifdef __FL_EQUALIZED_MODE
void CMover::ResetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem, BOOL bIgnoreSetItem)
#else
void CMover::ResetDestParamEquip(ItemProp* pItemProp, CItemElem* pItemElem)
#endif
{
	if (pItemElem && pItemElem->IsFlag(CItemElem::expired))
		return;

#ifdef __WORLDSERVER
	RunItemScript(this, pItemProp->dwID, ITEM_OP_UNEQUIP, NULL);
#endif // __WORLDSERVER

#ifdef __WEAPON_RARITY
	// if (pItemElem && (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3)))
	// {
	// 	float fFactor = 0.0f;
	// 	int nFlatBonus = 0;
	// 	if (pItemElem->m_nWeaponRarity > 0)
	// 	{
	// 		switch (pItemElem->m_nWeaponRarity)
	// 		{
	// 			case 5: fFactor = 0.25f; nFlatBonus = 50; 
	// 					if(pItemProp->dwItemKind3 == IK3_ARMOR)
	// 						ResetDestParam(DST_HP_MAX_RATE, 50);
	// 					break; // Legendary
    //             case 4: fFactor = 0.2f;  nFlatBonus = 40; break; // Epic
    //             case 3: fFactor = 0.15f; nFlatBonus = 30; break; // Unique
    //             case 2: fFactor = 0.1f;  nFlatBonus = 20; break; // Rare
    //             case 1: fFactor = 0.05f; nFlatBonus = 10; break; // Common
	// 		}

	// 		for (int i = 0; i < PROP_BONUSES; i++)
	// 		{
	// 			if (pItemProp->dwDestParam[i] != -1)
	// 			{
	// 				int nStats = (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor);
	// 				if (nStats > 0)
	// 					ResetDestParam(pItemElem->GetProp()->dwDestParam[i], nStats);
	// 			}
	// 		}

	// 		int nTotalBonus = nFlatBonus;

    //         ResetDestParam(DST_STR, nTotalBonus);
    //         ResetDestParam(DST_DEX, nTotalBonus);
    //         ResetDestParam(DST_STA, nTotalBonus);
    //         ResetDestParam(DST_INT, nTotalBonus);
	// 	}
	// }
	if (pItemElem && (IsValidRarityItem(pItemElem->GetProp()->dwItemKind3) || IsValidRarityItem2(pItemElem->GetProp()->dwItemKind3)))
	{
		float fFactor = 0.0f;
		int nFlatBonus = 0;
		if (pItemElem->m_nWeaponRarity > 0)
		{
			switch (pItemElem->m_nWeaponRarity)
			{
				case 5: fFactor = 0.25f; nFlatBonus = 50; break; // Legendary
                case 4: fFactor = 0.2f;  nFlatBonus = 40; break; // Epic
                case 3: fFactor = 0.15f; nFlatBonus = 30; break; // Unique
                case 2: fFactor = 0.1f;  nFlatBonus = 20; break; // Rare
                case 1: fFactor = 0.05f; nFlatBonus = 10; break; // Common
			}

			for (int i = 0; i < PROP_BONUSES; i++)
			{
				if (pItemProp->dwDestParam[i] != -1)
				{
					int nStats = (int)((pItemElem->GetProp()->nAdjParamVal[i]) * fFactor);
					if (nStats > 0)
						ResetDestParam(pItemElem->GetProp()->dwDestParam[i], nStats);
				}
			}

			int nTotalBonus = nFlatBonus;

            ResetDestParam(DST_STR, nTotalBonus);
            ResetDestParam(DST_DEX, nTotalBonus);
            ResetDestParam(DST_STA, nTotalBonus);
            ResetDestParam(DST_INT, nTotalBonus);
		}
	}
#endif // __WEAPON_RARITY

	for (int i = 0; i < PROP_BONUSES; i++)
	{
		if (pItemProp->dwDestParam[i] != -1)
			ResetDestParam(pItemProp->dwDestParam[i], pItemProp->nAdjParamVal[i]);
	}

	UpdateElementResist(pItemElem, FALSE);

	if (pItemElem)
	{
		CAccessoryProperty* pProperty = CAccessoryProperty::GetInstance();
		if (pItemElem->IsAccessory())
		{
			vector<SINGLE_DST>* pDst = pProperty->GetDst(pItemElem->m_dwItemId, pItemElem->GetAbilityOption());
			for (DWORD i = 0; i < pDst->size(); i++)
				ResetDestParam((*pDst)[i].nDst, (*pDst)[i].nAdj);
		}

#ifdef __WORLDSERVER
		if (pItemProp->IsGiftBoxItem())
			((CUser*)this)->RemoveBuff(BUFF_EQUIP, (WORD)(pItemProp->dwID));
#endif // __WORLDSERVER

#ifndef __FL_EQUALIZED_MODE
		int nAbilityOption = GetSetItem(pItemElem);
		if (nAbilityOption > 0)
			ResetSetItemAvail(nAbilityOption);
		DestParamPiercingAvail(pItemElem, FALSE);
		ResetDestParamSetItem(pItemElem);
		ResetDestParamRandomOpt(pItemElem);
#else
		if (!bIgnoreSetItem)
		{
			int nAbilityOption = GetSetItem(pItemElem);
			if (nAbilityOption > 0)
				ResetSetItemAvail(nAbilityOption);

			ResetDestParamSetItem(pItemElem);
		}
		DestParamPiercingAvail(pItemElem, FALSE);
		ResetDestParamRandomOpt(pItemElem);
#endif
	}
	}

void CMover::PickupGold(int nGold, BOOL bDropMob)
{
	ASSERT(nGold > 0);
	if (nGold <= 0)
	{
		Error("%s PickupGold - %d", GetName(), nGold);
		return;
	}

#ifdef __WORLDSERVER
	CParty* pParty = g_PartyMng.GetParty(m_idparty);
	if (pParty && pParty->IsMember(m_idPlayer) && bDropMob)
	{
		int nNumInsideMember = 1;
		CUser* pListMember[MAX_PTMEMBER_SIZE];
		int	nMaxListMember = 0;
		CUser* pMember = NULL;
		int i;
		D3DXVECTOR3	vDist;


		memset(pListMember, 0, sizeof(pListMember));
		for (i = 0; i < pParty->m_nSizeofMember; i++)
		{
			pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);

			if (IsValidArea(pMember, 32.0f))
				pListMember[nMaxListMember++] = pMember;
		}

		if (nMaxListMember > 0)
		{
			int nShare = nGold / nMaxListMember;
			int nRest = nGold % nMaxListMember;
			if (nShare)
			{
				for (i = 0; i < nMaxListMember; i++)
				{
					pMember = pListMember[i];

					if (i == 0)
						pMember->PickupGoldCore(nShare + nRest);
					else
						pMember->PickupGoldCore(nShare);
				}
			}
		}
	}
	else
#endif // WORLDSERVER
	{
		PickupGoldCore(nGold);
	}
}


void CMover::PickupGoldCore(int nGold)
{
	if (AddGold(nGold) == FALSE)
		return;

#ifndef __FL_FIX_USELESS_LOGS
#ifdef __WORLDSERVER
	if (IsPlayer())
	{
		if (nGold >= 500)
		{
			LogItemInfo aLogItem;
			aLogItem.Action = "R";
			aLogItem.SendName = "GROUND";
			aLogItem.RecvName = GetName();
			aLogItem.WorldId = GetWorld()->GetID();
			aLogItem.Gold = GetGold() - nGold;
			aLogItem.Gold2 = GetGold();
			//aLogItem.ItemName = "SEED";
			_stprintf(aLogItem.szItemName, "%d", II_GOLD_SEED1);
			aLogItem.itemNumber = nGold;
			g_DPSrvr.OnLogItem(aLogItem);
		}
	}
#endif	// __WORLDSERVER
#endif
}

#ifdef __CLIENT
BOOL CMover::GetEquipFlag(int nParts, BYTE* pbyFlag)
{
	if (IsActiveMover())
	{
		CItemElem* pItemElem = GetEquipItem(nParts);
		if (pItemElem)
		{
			*pbyFlag = pItemElem->m_byFlag;
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		if (nParts < 0 || nParts >= MAX_HUMAN_PARTS)
			return FALSE;

		if (m_aEquipInfo[nParts].dwId != NULL_ID)
		{
			*pbyFlag = m_aEquipInfo[nParts].byFlag;
			return TRUE;
		}
		return FALSE;
	}
}
#endif	// __CLIENT

void CMover::ClearEquipInfo()
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

void CMover::UpdateElementResist(CItemElem* pItemElem, BOOL bSet)
{
	if (!pItemElem)
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	if (pItemProp->dwItemKind1 != IK1_ARMOR)
		return;

	if (pItemProp->dwParts != PARTS_UPPER_BODY)
		return;

	if (pItemElem->m_bItemResist == SAI79::NO_PROP)
		return;

	if (pItemElem->m_nResistAbilityOption <= 0)
		return;

	if (bSet) // Set
	{
		int nStrong = GetStrongDST(pItemElem->m_bItemResist);
		int nWeak = GetWeakDST(pItemElem->m_bItemResist);

		SetDestParam(nWeak, -pItemElem->m_nResistAbilityOption * 2, NULL_CHGPARAM);
		SetDestParam(nStrong, pItemElem->m_nResistAbilityOption * 2, NULL_CHGPARAM);
	}
	else // Reset
	{
		int nStrong = GetStrongDST(pItemElem->m_bItemResist);
		int nWeak = GetWeakDST(pItemElem->m_bItemResist);

		ResetDestParam(nWeak, -pItemElem->m_nResistAbilityOption * 2);
		ResetDestParam(nStrong, pItemElem->m_nResistAbilityOption * 2);
	}
}