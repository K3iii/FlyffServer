// AIPet.cpp :


#include "stdafx.h"
#include "AIPet.h"
#include "lang.h"
#include "User.h"
#include "dpcoreclient.h"

extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;

enum
{
	STATE_INIT = 1,
	STATE_IDLE,
	STATE_RAGE
};
BEGIN_AISTATE_MAP(CAIPet, CAIInterface)

	ON_STATE(STATE_INIT, &CAIPet::StateInit)
	ON_STATE(STATE_IDLE, &CAIPet::StateIdle)
	ON_STATE(STATE_RAGE, &CAIPet::StateRage)

END_AISTATE_MAP()

#define		PETSTATE_IDLE		0
#define		PETSTATE_TRACE		1


#ifndef __VM_0820
#ifndef __MEM_TRACE
#ifdef __VM_0819
MemPooler<CAIPet>* CAIPet::m_pPool = new MemPooler<CAIPet>(128, "CAIPet");
#else	// __VM_0819
MemPooler<CAIPet>* CAIPet::m_pPool = new MemPooler<CAIPet>(128);
#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

CAIPet::CAIPet()
{
	Init();
}
CAIPet::CAIPet(CObj* pObj) : CAIInterface(pObj)
{
	Init();
}

CAIPet::~CAIPet()
{
	Destroy();
}

void CAIPet::Init(void)
{
	m_idOwner = NULL_ID;
	m_bLootMove = FALSE;
	m_idLootItem = NULL_ID;
	m_nState = PETSTATE_IDLE;
	m_dwSkillId = NULL_ID;
	m_idPetItem = NULL_ID;
}

void CAIPet::Destroy(void)
{
	Init();
}

void CAIPet::InitAI()
{
	PostAIMsg(AIMSG_SETSTATE, STATE_IDLE);
}

void CAIPet::MoveToDst(D3DXVECTOR3 vDst)
{
	CMover* pMover = GetMover();
	pMover->SetDestPos(vDst);
	g_UserMng.AddSetDestPos(pMover, vDst, 1);

}

void CAIPet::MoveToDst(OBJID idTarget)
{
	CMover* pMover = GetMover();
	if (pMover->GetDestId() == idTarget && pMover->m_fArrivalRange == 0.0f)
		return;
	pMover->SetDestObj(idTarget, 0.0f, TRUE);
}

BOOL CAIPet::SubItemLoot(void)
{
	CMover* pMover = GetMover();
	CMover* pOwner = prj.GetMover(m_idOwner);
	CWorld* pWorld = GetWorld();

	if (!pMover || !pOwner || !pWorld)
		return FALSE;

	D3DXVECTOR3 vPos = pMover->GetPos();
	CObj* pObj = NULL;
	int nRange = 0;
	D3DXVECTOR3 vDist;
	FLOAT fDistSq, fMinDist = 9999999.0f;
	CObj* pMinObj = NULL;

	vDist = pOwner->GetPos() - pMover->GetPos();
	fDistSq = D3DXVec3LengthSq(&vDist);
#ifdef __AIO_PETS
	if (fDistSq > 60.0f * 60.0f)
		return FALSE;
#else
	if (fDistSq > 32.0f * 32.0f)
		return FALSE;
#endif

	if (pOwner && pOwner->IsFly())
		return FALSE;

	FOR_LINKMAP(pWorld, vPos, pObj, nRange, CObj::linkDynamic, pMover->GetLayer())
	{
		if (pObj->GetType() == OT_ITEM)
		{
			CItem* pItem = (CItem*)pObj;
			ItemProp* pItemProp = pItem->GetProp();
			if (pItem->IsDelete() == FALSE)
			{
				if (pItemProp)
				{
					if (pOwner->IsLoot(pItem, TRUE))
					{
#ifdef __FL_PET_FILTER
						if (pOwner->IsPlayer() && LootType(((CUser*)pOwner)->m_nPetLootType, pItemProp))
						{
#endif
							vDist = D3DXVECTOR3(pObj->GetPos().x, 0.0f, pObj->GetPos().z) - D3DXVECTOR3(pMover->GetPos().x, 0.0f, pMover->GetPos().z);
							fDistSq = D3DXVec3LengthSq(&vDist);
#ifndef __AIO_PETS
							if (fDistSq < 15 * 15 && fDistSq < fMinDist)
								pMinObj = pObj;
#else
							if (fDistSq < 45 * 45 && fDistSq < fMinDist)
							{
								pMinObj = pObj;
								fMinDist = fDistSq;
							}
#endif
#ifdef __FL_PET_FILTER
						}
#endif
					}
				}
			}
		}
	}
	END_LINKMAP

		if (pMinObj)
		{
#ifdef __AIO_PETS
			DWORD dwIdLootItem = NULL_ID;

			if (pMinObj->GetType() == OT_ITEM)
				dwIdLootItem = ((CItem*)pMinObj)->GetId();
#else
			DWORD dwIdLootItem = ((CItem*)pMinObj)->GetId();
#endif

			CCtrl* pCtrl = prj.GetCtrl(dwIdLootItem);
			if (IsValidObj(pCtrl))
			{
				MoveToDst(pMinObj->GetPos());
				m_idLootItem = dwIdLootItem;
				m_bLootMove = TRUE;
			}
		}

	return m_bLootMove;
}

#ifdef __FL_PET_FILTER
BOOL CAIPet::LootType(int nType, ItemProp* pItemProp)
{
	// All or nothing
	if ((nType & 0x400000))
		return TRUE;
	else if ((nType & 0x800000))
		return FALSE;

	DWORD dwKind1 = pItemProp->dwItemKind1;
	DWORD dwKind2 = pItemProp->dwItemKind2;
	DWORD dwKind3 = pItemProp->dwItemKind3;

	if (dwKind3 == IK3_GOLD)
		return TRUE;

	bool bType[22] = { 0, };
	// Weapon
	bType[0] = (dwKind1 == IK1_WEAPON && pItemProp->dwReferStat1 == WEAPON_ULTIMATE);
	bType[1] = (dwKind1 == IK1_WEAPON && pItemProp->dwReferStat1 == WEAPON_UNIQUE);
	bType[2] = (dwKind1 == IK1_WEAPON && (pItemProp->dwReferStat1 == WEAPON_GENERAL || pItemProp->dwReferStat1 == NULL_ID));
	bType[3] = (dwKind3 == IK3_SHIELD
#if __VER >= 16
		|| dwKind3 == IK3_MAGICBARUNA || dwKind3 == IK3_ZEMBARUNA
#endif
		);

	// Armor
	bType[4] = (dwKind1 == IK1_ARMOR && pItemProp->dwReferStat1 == ARMOR_SET);
	bType[5] = (dwKind1 == IK1_ARMOR && pItemProp->dwReferStat1 == NULL_ID);

	// Acce
	bType[6] = (dwKind3 == IK3_NECKLACE);
	bType[7] = (dwKind3 == IK3_EARRING);
	bType[8] = (dwKind3 == IK3_RING);

	// Cards
	bType[9] = (dwKind3 == IK3_ELECARD);
	bType[10] = (dwKind3 == IK3_SOCKETCARD2);
	bType[11] = (dwKind3 == IK3_SOCKETCARD);

	// Food
	bType[12] = (dwKind3 == IK3_COOKING || dwKind3 == IK3_INSTANT || dwKind3 == IK3_ICECEARM || dwKind3 == IK3_PILL);
	bType[13] = (dwKind3 == IK3_REFRESHER);
	bType[14] = (dwKind3 == IK3_DRINK);

	// Beads
	bType[15] = (dwKind3 == IK3_VIS && pItemProp->dwAbilityMax == 1);
	bType[16] = (dwKind3 == IK3_VIS && pItemProp->dwAbilityMax == 2);
	bType[17] = (dwKind3 == IK3_VIS && pItemProp->dwAbilityMax == 3);

	// QI
	bType[18] = (dwKind3 == IK3_GEM);

	// Jewels
	bType[19] = (dwKind3 == IK3_ULTIMATE || dwKind3 == IK3_ENCHANT || dwKind3 == IK3_PIERDICE || dwKind3 == IK3_DICE);

	// Eggs
	bType[20] = (dwKind3 == IK3_EGG);

	// Scrolls
	bType[21] = (dwKind2 == IK2_BUFF || dwKind2 == IK2_BUFF2 || dwKind3 == IK3_SCROLL);


	if ((nType & 0x1) && bType[0])
		return TRUE;
	if ((nType & 0x2) && bType[1])
		return TRUE;
	if ((nType & 0x4) && bType[2])
		return TRUE;
	if ((nType & 0x8) && bType[3])
		return TRUE;

	if ((nType & 0x10) && bType[4])
		return TRUE;
	if ((nType & 0x20) && bType[5])
		return TRUE;

	if ((nType & 0x40) && bType[6])
		return TRUE;
	if ((nType & 0x80) && bType[7])
		return TRUE;
	if ((nType & 0x100) && bType[8])
		return TRUE;

	if ((nType & 0x200) && bType[9])
		return TRUE;
	if ((nType & 0x400) && bType[10])
		return TRUE;
	if ((nType & 0x800) && bType[11])
		return TRUE;

	if ((nType & 0x1000) && bType[12])
		return TRUE;
	if ((nType & 0x2000) && bType[13])
		return TRUE;
	if ((nType & 0x4000) && bType[14])
		return TRUE;

	if ((nType & 0x8000) && bType[15])
		return TRUE;
	if ((nType & 0x10000) && bType[16])
		return TRUE;
	if ((nType & 0x20000) && bType[17])
		return TRUE;

	if ((nType & 0x40000) && bType[18])
		return TRUE;
	if ((nType & 0x80000) && bType[19])
		return TRUE;
	if ((nType & 0x100000) && bType[20])
		return TRUE;
	if ((nType & 0x200000) && bType[21])
		return TRUE;

	return FALSE;
}
#endif

BOOL CAIPet::StateInit(const AIMSG& msg)
{
	return TRUE;
}

BOOL CAIPet::MoveProcessIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	CMover* pOwner = prj.GetMover(m_idOwner);

	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	if (NotOwnedPetInactivated())
		return FALSE;

	if (m_bLootMove == FALSE)
	{
		if (m_nState == PETSTATE_IDLE)
		{
#ifdef __AIO_PETS
			if (pOwner->IsRangeObj(pMover, 2.0f) == FALSE)
#else
			D3DXVECTOR3 vDist = pOwner->GetPos() - pMover->GetPos();
			FLOAT fDistSq = D3DXVec3LengthSq(&vDist);
			if (fDistSq > 1.0f * 1.0f)
#endif
			{
				MoveToDst(m_idOwner);
				m_nState = PETSTATE_TRACE;
			}
		}
		else if (m_nState == PETSTATE_TRACE)
		{
			if (pOwner->IsRangeObj(pMover, 0) == TRUE)
			{
				m_nState = PETSTATE_IDLE;
				pMover->SendActMsg(OBJMSG_STOP);
				pMover->ClearDest();
			}
			if (pMover->IsEmptyDest())
			{
				m_bLootMove = FALSE;
				m_idLootItem = NULL_ID;
				m_nState = PETSTATE_IDLE;
			}

		}
	}

	if ((pMover->GetCount() & 15) == 0)
	{
		if (m_bLootMove == FALSE)
			if (SubItemLoot())
				m_nState = PETSTATE_IDLE;
	}

	if (m_bLootMove == TRUE)
	{
		CCtrl* pCtrl = prj.GetCtrl(m_idLootItem);
		if (IsInvalidObj(pCtrl))
		{
			MoveToDst(pMover->GetPos());
			m_bLootMove = FALSE;
			m_idLootItem = NULL_ID;
			m_nState = PETSTATE_IDLE;
		}
		else
		{
			if (pMover->IsEmptyDest())
			{
				m_bLootMove = FALSE;
				m_idLootItem = NULL_ID;
				m_nState = PETSTATE_IDLE;
			}
		}
	}

	return TRUE;
}

BOOL CAIPet::StateIdle(const AIMSG& msg)
{
	CMover* pMover = GetMover();

	BeginAIHandler()

		OnMessage(AIMSG_INIT)
		OnMessage(AIMSG_INIT_TARGETCLEAR)
		OnMessage(AIMSG_PROCESS)
		MoveProcessIdle(msg);

	OnMessage(AIMSG_DAMAGE)

		OnMessage(AIMSG_DIE)
		SendAIMsg(AIMSG_EXIT);

	OnMessage(AIMSG_COLLISION)

		OnMessage(AIMSG_ARRIVAL)
	{
		if (m_bLootMove)
		{
			CMover* pOwner = prj.GetMover(m_idOwner);
			if (IsValidObj(pOwner))
			{
				BOOL bSuccess = FALSE;
				CCtrl* pCtrl = prj.GetCtrl(m_idLootItem);
#ifdef __AIO_PETS
				CItem* pItem = (CItem*)pCtrl;
#endif
				if (IsValidObj(pCtrl))
				{
#ifndef __AIO_PETS
					CItem* pItem = (CItem*)pCtrl;
#endif
					D3DXVECTOR3 vDist = D3DXVECTOR3(pCtrl->GetPos().x, 0.0f, pCtrl->GetPos().z) - D3DXVECTOR3(pMover->GetPos().x, 0.0f, pMover->GetPos().z);
					FLOAT fDistSq = D3DXVec3LengthSq(&vDist);
					if (fDistSq < 5.0f * 5.0f)
					{
						if (pItem->IsDelete())
							return TRUE;
						bSuccess = pOwner->DoLoot(pItem);
					}
				}
				if (bSuccess)
				{
#ifdef __AIO_PETS
					if (pMover->GetWorld())
					{
						D3DXVECTOR3 vPos = pMover->GetPos();
						CObj* pObj;

						FOR_LINKMAP(pMover->GetWorld(), vPos, pObj, 0, CObj::linkDynamic, pMover->GetLayer())
						{
							if (pObj->GetType() == OT_ITEM)
							{
								D3DXVECTOR3 vDist = D3DXVECTOR3(pObj->GetPos().x, 0.0f, pObj->GetPos().z) - D3DXVECTOR3(pMover->GetPos().x, 0.0f, pMover->GetPos().z);
								FLOAT fDistSq = D3DXVec3LengthSq(&vDist);
								if (fDistSq < 3.0f * 3.0f)
								{
									CItem* pItem2 = (CItem*)pObj;
									ItemProp* pItemProp = pItem2->GetProp();
									if (pItemProp && pOwner->IsLoot(pItem2, TRUE))
									{
										if (pItemProp->dwID == pItem->GetProp()->dwID && (pItemProp->dwPackMax > 1 || pItemProp->dwItemKind3 == IK3_GOLD) && pOwner->DoLoot(pItem2) == FALSE)
										{
											continue;
										}
									}
								}
							}
						}
						END_LINKMAP
					}
#endif
					if (SubItemLoot() == FALSE)
					{
						m_bLootMove = FALSE;
						m_idLootItem = NULL_ID;
					}
				}
				else
				{
					m_bLootMove = FALSE;
					m_idLootItem = NULL_ID;
				}
			}
		}
	}

	OnMessage(AIMSG_EXIT)

		EndAIHandler()

		return TRUE;
}


BOOL CAIPet::MoveProcessRage(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	if (pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL))
		return FALSE;

	return TRUE;
}

BOOL CAIPet::StateRage(const AIMSG& msg)
{
	CMover* pMover = GetMover();
	if (IsInvalidObj(pMover))	return FALSE;

	BeginAIHandler()

		OnMessage(AIMSG_INIT)
		OnMessage(AIMSG_INIT_TARGETCLEAR)
		OnMessage(AIMSG_PROCESS)
		MoveProcessRage(msg);

	OnMessage(AIMSG_DAMAGE)

		OnMessage(AIMSG_COLLISION)
		OnMessage(AIMSG_DIE)
		OnMessage(AIMSG_DSTDIE)
		SendAIMsg(AIMSG_SETSTATE, STATE_IDLE);

	OnMessage(AIMSG_BEGINMOVE)

		OnMessage(AIMSG_ARRIVAL)
		OnMessage(AIMSG_EXIT)

		EndAIHandler()

		return TRUE;
}

BOOL CAIPet::NotOwnedPetInactivated(void)
{
	CMover* pEatPet = GetMover();
	CMover* pOwner = prj.GetMover(m_idOwner);

	if (IsInvalidObj(pOwner))
	{
		pEatPet->Delete();
		return TRUE;
	}

#ifndef __AIO_PETS
	if (pOwner->IsDie())
	{
		pOwner->InactivateEatPet();
		return TRUE;
	}

#ifndef __AIO_PETS
	else if (!pOwner->IsValidArea(pEatPet, 32))
#else
	else if (!pOwner->IsValidArea(pEatPet, 60))
#endif
	{
		pOwner->InactivateEatPet();
		return TRUE;
	}
#endif
	return FALSE;
}

void CAIPet::SetItem(CMover* pPlayer, CItemElem* pItem)
{
	m_idPetItem = pItem->m_dwObjId;
	pPlayer->SetDestParamRandomOptExtension(pItem);
	SetSkill(pPlayer, pItem->GetProp());

}

void CAIPet::SetSkill(CMover* pPlayer, ItemProp* Prop)
{
	if (Prop->dwActiveSkill != NULL_ID)
	{
		SetSkillId(Prop->dwActiveSkill);
		pPlayer->DoActiveSkill(Prop->dwActiveSkill, 1, pPlayer);
	}
}

void CAIPet::ResetItem(void)
{
	CMover* pPlayer = prj.GetMover(m_idOwner);
	if (IsValidObj(pPlayer))
	{
		CItemElem* pItem = static_cast<CItemElem*>(pPlayer->m_Inventory.GetAtId(m_idPetItem));
		if (pItem && pItem->IsEatPet())
		{
			pPlayer->ResetDestParamRandomOptExtension(pItem);
			ResetSkill(pPlayer);
		}
	}
	m_idPetItem = NULL_ID;
}

void CAIPet::ResetSkill(CMover* pPlayer)
{
	if (GetSkillId() != NULL_ID && pPlayer->HasBuff(BUFF_SKILL, (WORD)(GetSkillId())))
		pPlayer->RemoveBuff(BUFF_SKILL, (WORD)(GetSkillId()));
}