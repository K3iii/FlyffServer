#include "stdafx.h"
#include "mover.h"
#include "defineText.h"
#ifdef __WORLDSERVER
#include "user.h"
extern	CUserMng			g_UserMng;

#include "DPDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif	// WORLDSERVER

CMover* CVTInfo::GetOther()
{
	//	return m_pOther;
	if (m_objId == NULL_ID)
		return NULL;

	return prj.GetMover(m_objId);
}


void CVTInfo::SetOther(CMover* pMover)
{
	//	m_pOther = pMover;
	if (pMover)
	{
		m_objId = pMover->GetId();
	}
	else
	{
		m_objId = NULL_ID;
	}
}

CItemElem* CVTInfo::GetItem(BYTE i)
{
	return m_apItem_VT[i];
}

void CVTInfo::SetItem(BYTE i, CItemElem* pItemBase)
{
	m_apItem_VT[i] = pItemBase;
}

LPCTSTR	CVTInfo::GetTitle()
{
	return m_strTitle.c_str();
}

void CVTInfo::SetTitle(LPCTSTR szTitle)
{
	m_strTitle = szTitle;
}

BOOL CVTInfo::IsVendorOpen()
{
	return (m_strTitle.empty() != true);
}

void CVTInfo::Init(CMover* pOwner)
{
	m_pOwner = pOwner;
	ZeroMemory(m_apItem_VT, sizeof(m_apItem_VT));
	TradeSetGold(0);							// raiders.2006.11.28
	TradeClear();
	m_strTitle = "";
}


void CVTInfo::TradeClear()
{
	SetOther(NULL);
	for (int i = 0; i < MAX_TRADE; i++)
	{
		if (m_apItem_VT[i])
		{
			m_apItem_VT[i]->SetExtra(0);
			m_apItem_VT[i] = NULL;
		}
	}
#ifdef __WORLDSERVER
	int nGold = TradeGetGold();
	if (nGold > 0 && m_pOwner)
		m_pOwner->AddGold(nGold);

#endif

	TradeSetGold(0);
	TradeSetState(TRADE_STEP_ITEM);
}

void CVTInfo::TradeSetGold(DWORD dwGold)
{
	m_dwTradeGold = dwGold;
}

int CVTInfo::TradeGetGold()
{
	return m_dwTradeGold;
}

void CVTInfo::TradeSetItem(DWORD dwItemObjID, BYTE byNth, int nItemNum)
{
	CItemElem* pItemBase = m_pOwner->m_Inventory.GetAtId(dwItemObjID);
	if (pItemBase)
	{
		m_apItem_VT[byNth] = pItemBase;
		pItemBase->SetExtra(nItemNum);
	}
}

BOOL CVTInfo::TradeClearItem(BYTE byNth)
{
	CItemElem* pItemBase = m_apItem_VT[byNth];
	if (IsUsingItem(pItemBase))
	{
		pItemBase->SetExtra(0);
		m_apItem_VT[byNth] = NULL;

		return TRUE;
	}
	else
		return FALSE;
}

BOOL CVTInfo::TradeConsent()
{
	CMover* pTrader = GetOther();
	if (pTrader == NULL)
		return FALSE;

	int cbI = 0, cbYou = 0;
	CItemContainer<CItemElem> a;
	a.SetItemContainer(ITYPE_ITEM, MAX_TRADE);

	CItemElem* pItemBase;
	for (int i = 0; i < MAX_TRADE; i++)
	{
		pItemBase = m_apItem_VT[i];
		if (!pItemBase)
			continue;

		if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
			pItemBase->SetExtra(pItemBase->m_nItemNum);

		m_apItem_VT[i] = NULL;
		CItemElem* pItemElem = pItemBase;
		if (pItemElem->GetProp()->dwPackMax > 1)
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			a.Add(pItemElem);
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra(0);
			if (nTradeNum == 0)
				m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
		}
		else
		{
			a.Add(pItemElem);
			m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
		}
	}

	for (int i = 0; i < MAX_TRADE; i++)
	{
		pItemBase = pTrader->m_vtInfo.GetItem(i);
		if (pItemBase == NULL)
			continue;

		if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
			pItemBase->SetExtra(pItemBase->m_nItemNum);

		pTrader->m_vtInfo.SetItem(i, NULL);
		CItemElem* pItemElem = pItemBase;
		if (pItemElem->GetProp()->dwPackMax > 1)
		{
			short nTradeNum = pItemElem->m_nItemNum - (short)pItemBase->GetExtra();
			pItemElem->m_nItemNum = pItemBase->GetExtra();
			m_pOwner->m_Inventory.Add(pItemElem);
			pItemElem->m_nItemNum = nTradeNum;
			pItemElem->SetExtra(0);
			if (nTradeNum == 0)
				pTrader->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
		}
		else
		{
			m_pOwner->m_Inventory.Add(pItemElem);
			pTrader->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
		}
	}

	cbI = a.GetCount();
	for (int i = 0; i < cbI; i++)
	{
		pItemBase = a.GetAtId(i);
		pTrader->m_Inventory.Add(pItemBase);
	}


	int nThisGold = pTrader->m_vtInfo.TradeGetGold();
	int nTraderGold = TradeGetGold();


	TradeSetGold(0);
	TradeClear();
	pTrader->m_vtInfo.TradeSetGold(0);
	pTrader->m_vtInfo.TradeClear();


	m_pOwner->AddGold(nThisGold, FALSE);
	pTrader->AddGold(nTraderGold, FALSE);

	return TRUE;
}

DWORD CVTInfo::TradeSetItem2(DWORD dwItemObjID, BYTE byNth, int& nItemNum)
{
	CItemElem* pItemBase = m_pOwner->m_Inventory.GetAtId(dwItemObjID);
	if (IsUsableItem(pItemBase) == FALSE || m_apItem_VT[byNth] != NULL)
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if (m_pOwner->m_Inventory.IsEquip(pItemBase->m_dwObjId))
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if (pItemBase->IsQuest())
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if (pItemBase->IsBinds())
		return (DWORD)TID_GAME_CANNOTTRADE_ITEM;

	if (m_pOwner->IsUsing(pItemBase))
		return (DWORD)TID_GAME_CANNOT_DO_USINGITEM;

	if (nItemNum < 1)
		nItemNum = 1;

	if (nItemNum > pItemBase->m_nItemNum)
		nItemNum = pItemBase->m_nItemNum;

	TradeSetItem(dwItemObjID, byNth, nItemNum);

	return 0;
}

void TradeLog(CAr& ar, CItemElem* pItemBase, short nItemCount)
{
	ar << nItemCount;
	ar << pItemBase->GetAbilityOption();
	ar << int(pItemBase->m_bItemResist);
	ar << int(pItemBase->m_nResistAbilityOption);
	ar << pItemBase->m_nHitPoint;
	ar << pItemBase->m_dwKeepTime;
	ar << pItemBase->GetPiercingSize();
	for (int i = 0; i < pItemBase->GetPiercingSize(); i++)
		ar << pItemBase->GetPiercingItem(i);
	ar << pItemBase->GetUltimatePiercingSize();
	for (int i = 0; i < pItemBase->GetUltimatePiercingSize(); i++)
		ar << pItemBase->GetUltimatePiercingItem(i);
	ar << pItemBase->GetRandomOptItemId();
	if (pItemBase->m_pPet)
	{
		CPet* pPet = pItemBase->m_pPet;

		ar << pPet->GetKind();
		ar << pPet->GetLevel();
		ar << pPet->GetExp();
		ar << pPet->GetEnergy();
		ar << pPet->GetLife();
		ar << pPet->GetAvailLevel(PL_D);
		ar << pPet->GetAvailLevel(PL_C);
		ar << pPet->GetAvailLevel(PL_B);
		ar << pPet->GetAvailLevel(PL_A);
		ar << pPet->GetAvailLevel(PL_S);
	}
	else
	{
		if (pItemBase->IsTransformVisPet() == TRUE)
		{
			ar << (BYTE)100;
		}
		else
		{
			ar << (BYTE)0;
		}
		ar << (BYTE)0;
		ar << (DWORD)0;
		ar << (WORD)0;
		ar << (WORD)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
		ar << (BYTE)0;
	}
}

BOOL CheckTradeGold(CMover* pMover, int nMinus, int nPlus)
{
	if (nMinus >= 0)
	{
		if (pMover->CheckUserGold(nMinus, false) == true)
		{
			int nGold = pMover->GetGold() - nMinus;
			int nResult = nGold + nPlus;
			if (nPlus >= 0)
			{
				if (nResult >= nGold)
					return TRUE;
			}
			else
			{
				if (nResult >= 0)
					return TRUE;
			}

		}
	}

	return FALSE;
}

TRADE_CONFIRM_TYPE CVTInfo::TradeLastConfirm(CAr& ar)
{
	TRADE_CONFIRM_TYPE		result = TRADE_CONFIRM_ERROR;
	CMover* pTrader = GetOther();

	int nTraderGold = pTrader->m_vtInfo.TradeGetGold();
	int nUserGold = TradeGetGold();

	int nThisAdd = nTraderGold;
	int nTraderAdd = nUserGold;

	if (CheckTradeGold(m_pOwner, 0, nTraderGold) == FALSE ||
		CheckTradeGold(pTrader, 0, nUserGold) == FALSE)
	{
		TradeClear();
		pTrader->m_vtInfo.TradeClear();
		return result;
	}

	int nPlayers = 0;
	int nTraders = 0;

	for (int i = 0; i < MAX_TRADE; i++)
	{
		if (GetItem(i))
			nPlayers++;

		if (pTrader->m_vtInfo.GetItem(i))
			nTraders++;
	}

	if ((pTrader->m_Inventory.GetSize() - pTrader->m_Inventory.GetCount()) < nPlayers)
		result = TRADE_CONFIRM_ERROR;
	else if ((m_pOwner->m_Inventory.GetSize() - m_pOwner->m_Inventory.GetCount()) < nTraders)
		result = TRADE_CONFIRM_ERROR;
	else
		result = TRADE_CONFIRM_OK;

	if (result == TRADE_CONFIRM_OK)
	{
		CItemContainer<CItemElem> u;
		u.SetItemContainer(ITYPE_ITEM, MAX_TRADE);

		m_pOwner->AddGold(nThisAdd, FALSE);
		pTrader->AddGold(nTraderAdd, FALSE);

		pTrader->m_vtInfo.TradeSetGold(0);
		TradeSetGold(0);

		ar.WriteString("T");
		ar.WriteString(pTrader->GetName());
		ar.WriteString(m_pOwner->GetName());
		ar << m_pOwner->GetWorld()->GetID();
		ar << nTraderGold << nUserGold;
		ar << pTrader->GetGold() << m_pOwner->GetGold();
		ar << m_pOwner->m_idPlayer << m_pOwner->GetLevel() << m_pOwner->GetJob();
		ar << pTrader->m_idPlayer << pTrader->GetLevel() << pTrader->GetJob();
#ifdef __WORLDSERVER
		ar.WriteString(((CUser*)m_pOwner)->m_playAccount.lpAddr);
		ar.WriteString(((CUser*)pTrader)->m_playAccount.lpAddr);
#endif	// __WORLDSERVER
		u_long uSize1 = 0;
		u_long uOffset1 = ar.GetOffset();
		ar << (DWORD)0;
		u_long uSize2 = 0;
		u_long uOffset2 = ar.GetOffset();
		ar << (DWORD)0;

		CItemElem* pItemBase;
		for (int i = 0; i < MAX_TRADE; i++)
		{
			pItemBase = m_apItem_VT[i];
			if (pItemBase == NULL)
				continue;

			if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
				pItemBase->SetExtra(pItemBase->m_nItemNum);

			m_apItem_VT[i] = NULL;
			CItemElem* pItemElem = pItemBase;
			if (pItemElem->GetProp()->dwPackMax > 1)
			{
				short nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				u.Add(pItemElem);
				pItemElem->m_nItemNum = nTradeNum;
				pItemElem->SetExtra(0);
				if (nTradeNum == 0)
					m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
			}
			else
			{
				u.Add(pItemElem);
				m_pOwner->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
			}
		}

		for (int i = 0; i < MAX_TRADE; i++)
		{
			pItemBase = pTrader->m_vtInfo.GetItem(i);
			if (pItemBase == NULL)
				continue;

			if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
				pItemBase->SetExtra(pItemBase->m_nItemNum);

			pTrader->m_vtInfo.SetItem(i, NULL);
			uSize1++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();

			char szItemId[32] = { 0, };
			_stprintf(szItemId, "%d", pItemBase->GetProp()->dwID);
			ar.WriteString(szItemId);

			CItemElem* pItemElem = pItemBase;
			if (pItemElem->GetProp()->dwPackMax > 1)
			{
				int nTradeNum = pItemElem->m_nItemNum - pItemBase->GetExtra();
				pItemElem->m_nItemNum = pItemBase->GetExtra();
				m_pOwner->m_Inventory.Add(pItemElem);
				pItemElem->m_nItemNum = (short)nTradeNum;

				TradeLog(ar, pItemBase, pItemBase->GetExtra());

				pItemElem->SetExtra(0);
				if (nTradeNum == 0)
					pTrader->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
			}
			else
			{
				TradeLog(ar, pItemBase, 1);

				m_pOwner->m_Inventory.Add(pItemElem);
				pTrader->m_Inventory.RemoveAtId(pItemBase->m_dwObjId);
			}
		}

		nPlayers = u.GetCount();
		for (int i = 0; i < nPlayers; i++)
		{
			pItemBase = u.GetAtId(i);
			pTrader->m_Inventory.Add(pItemBase);
			uSize2++;
			ar << pItemBase->m_dwItemId;
			ar << pItemBase->GetSerialNumber();
			char szItemId[32] = { 0, };
			_stprintf(szItemId, "%d", pItemBase->GetProp()->dwID);
			ar.WriteString(szItemId);
			TradeLog(ar, pItemBase, pItemBase->m_nItemNum);
		}

#ifdef __WORLDSERVER
		if (m_pOwner->IsPlayer())
		{
			if (((CUser*)m_pOwner)->GetWorld())
				g_dpDBClient.SavePlayer((CUser*)m_pOwner, ((CUser*)m_pOwner)->GetWorld()->GetID(), ((CUser*)m_pOwner)->GetPos(), ((CUser*)m_pOwner)->GetLayer());
		}
		if (pTrader->IsPlayer())
		{
			if (((CUser*)pTrader)->GetWorld())
				g_dpDBClient.SavePlayer((CUser*)pTrader, ((CUser*)pTrader)->GetWorld()->GetID(), ((CUser*)pTrader)->GetPos(), ((CUser*)pTrader)->GetLayer());
		}
#endif

		int nBufSize;
		LPBYTE lpBlock = ar.GetBuffer(&nBufSize);

		*(UNALIGNED u_long*)(lpBlock + uOffset1) = uSize1;
		*(UNALIGNED u_long*)(lpBlock + uOffset2) = uSize2;
	}


	TradeClear();
	pTrader->m_vtInfo.TradeClear();
	return result;
}

TRADE_STATE CVTInfo::TradeGetState()
{
	return m_state;
}

void CVTInfo::TradeSetState(TRADE_STATE state)
{
	m_state = state;
}

BOOL CVTInfo::VendorClearItem(BYTE i)
{
	if (m_apItem_VT[i])
	{
		m_apItem_VT[i]->SetExtra(0);
		m_apItem_VT[i]->m_nCost = 0;
#ifdef __FL_VENDORS
		m_apItem_VT[i]->m_nPerinCost = 0;
#endif
		m_apItem_VT[i] = NULL;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CVTInfo::VendorItemNum(BYTE i, short nNum)
{
	if (m_apItem_VT[i])
	{
		m_apItem_VT[i]->SetExtra(nNum);
		if (nNum <= 0)
		{
			m_apItem_VT[i]->m_nCost = 0;
#ifdef __FL_VENDORS
			m_apItem_VT[i]->m_nPerinCost = 0;
#endif
#ifdef __CLIENT
			if (m_pOwner->IsActiveMover() == FALSE)
				SAFE_DELETE(m_apItem_VT[i]);
#endif
			m_apItem_VT[i] = NULL;
		}
	}
}

void CVTInfo::VendorCopyItems(CItemElem** ppItemVd)
{
	memcpy((void*)m_apItem_VT, ppItemVd, sizeof(m_apItem_VT));
}

void CVTInfo::VendorClose(BOOL bClearTitle)
{
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		if (m_apItem_VT[i])
		{
			m_apItem_VT[i]->SetExtra(0);
			m_apItem_VT[i]->m_nCost = 0;
#ifdef __FL_VENDORS
			m_apItem_VT[i]->m_nPerinCost = 0;
#endif
#ifdef __CLIENT
			if (FALSE == m_pOwner->IsActiveMover())
				SAFE_DELETE(m_apItem_VT[i]);
#endif	// __CLIENT
			m_apItem_VT[i] = NULL;
		}
	}

	if (bClearTitle)
		m_strTitle = "";

	SetOther(NULL);
}

#ifndef __FL_VENDORS
void CVTInfo::VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost)
{
	CItemElem* pItemBase = m_pOwner->m_Inventory.GetAtId(nId);
	if (pItemBase)
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra(nNum);
		pItemBase->m_nCost = nCost;
	}
}
#else 
void CVTInfo::VendorSetItem(BYTE nId, BYTE i, short nNum, int nCost, int nPerinCost)
{
	CItemElem* pItemBase = m_pOwner->m_Inventory.GetAtId(nId);
	if (pItemBase)
	{
		m_apItem_VT[i] = pItemBase;
		pItemBase->SetExtra(nNum);
		pItemBase->m_nCost = nCost;
		pItemBase->m_nPerinCost = nPerinCost;
	}
}
#ifdef __WORLDSERVER
void CVTInfo::PlayerToVendorAll(CUser* pUserPlayer)
{
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		CItemElem* pItemBase = pUserPlayer->m_vtInfo.GetItem(i);
		if (!pItemBase)
			continue;

		PlayerToVendorSingle(pUserPlayer, pItemBase, i, pItemBase->GetExtra(), pItemBase->m_nCost, pItemBase->m_nPerinCost);
	}

	pUserPlayer->m_vtInfo.VendorClose();
	g_UserMng.AddPVendorClose(pUserPlayer);
}
void CVTInfo::PlayerToVendorSingle(CUser* pUserPlayer, CItemElem* pItemBase, BYTE i, short nNum, int nCost, int nPerinCost)
{
	CItemElem* pItemElem = pItemBase;

	CItemElem itemElem;
	itemElem = *pItemElem;
	itemElem.m_nItemNum = nNum;

	m_pOwner->m_Inventory.m_apItem[i] = itemElem;
	VendorSetItem(i, i, nNum, nCost, nPerinCost);
	pUserPlayer->RemoveItem((BYTE)pItemBase->m_dwObjId, nNum);
}
void CVTInfo::VendorToPlayerSingle(CUser* pUserPlayer, BYTE i)
{
	CItemElem* pItemElem = GetItem(i);
	if (!pItemElem)
		return;

	VendorClearItem(i);

	CItemElem itemElem;
	itemElem = *pItemElem;

	if (IsValidObj(pUserPlayer))
	{
		if (!pUserPlayer->CreateItem(&itemElem))
			g_dpDBClient.SendQueryPostMail(pUserPlayer->m_idPlayer, 0, itemElem, 0, "Offline Vendor", "This item was not sold, so it is back to you!");
	}
	else
		g_dpDBClient.SendQueryPostMail(m_pOwner->GetNpcOwner(), 0, itemElem, 0, "Offline Vendor", "This item was not sold, so it is back to you!");

	m_pOwner->RemoveItem((BYTE)pItemElem->m_dwObjId, pItemElem->m_nItemNum);
}
#endif
#endif

BOOL CVTInfo::VendorIsVendor()
{
	for (int i = 0; i < MAX_VENDITEM; ++i)
	{
		if (m_apItem_VT[i])
			return TRUE;
	}

	return FALSE;
}

BOOL CVTInfo::IsTrading(CItemElem* pItemElem)
{
	for (int i = 0; i < MAX_VENDITEM; i++)
	{
		if (m_apItem_VT[i] == pItemElem)
			return TRUE;
	}
	return FALSE;
}

//CDPSrvr::OnBuyPVendorItem
#ifdef __WORLDSERVER
BOOL CVTInfo::VendorSellItem(CMover* pBuyer, BYTE i, short nNum, VENDOR_SELL_RESULT& result)
{
	result.nRemain = 0;
	result.nErrorCode = 0;

	if (!IsVendorOpen())
		return FALSE;

	CItemElem* pItemBase = m_apItem_VT[i];
	if (!IsUsingItem(pItemBase))
		return FALSE;

	if (pItemBase->GetExtra() > pItemBase->m_nItemNum)
		pItemBase->SetExtra(pItemBase->m_nItemNum);

	if (nNum < 1)
		nNum = 1;
	if (nNum > pItemBase->GetExtra())
		nNum = (short)pItemBase->GetExtra();

#ifndef __FL_VENDORS
	const __int64 nnPenya = (__int64)((float)nNum * (float)pItemBase->m_nCost);
	if (nnPenya < 0 || nnPenya > INT_MAX)
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}
	const int nPenya = (int)nnPenya;

	if (pItemBase->m_nCost > 0 && pBuyer->CheckUserGold(nPenya, false) == false)
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	if (m_pOwner->CheckUserGold(nPenya, true) == false)
	{
		result.nErrorCode = TID_GAME_LIMITPENYA;
		return FALSE;
	}

	CItemElem* pItemElem = pItemBase;
	CItemElem itemElem;
	itemElem = *pItemElem;
	itemElem.m_nItemNum = nNum;

	if (pBuyer->CreateItem(&itemElem) == FALSE)
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	int nCost = pItemBase->m_nCost;
	pBuyer->AddGold(-(pItemBase->m_nCost * nNum));
	m_pOwner->AddGold(pItemBase->m_nCost * nNum);
#else
	// Price per item
	int nCost = pItemBase->m_nCost;
	int nPerinCost = pItemBase->m_nPerinCost;
	if (nPerinCost <= 0 && nCost <= 0)
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	// Price for all items
	__int64 nTotal64Penya = nCost;
	nTotal64Penya *= nNum;
	int nTotalPerins = nPerinCost;
	nTotalPerins *= nNum;

	// Convert penya price to perins value if possible
	int nPerinNum = (int)(nTotal64Penya / PERIN_VALUE);
	int nPenyaNum = (int)(nTotal64Penya - (nPerinNum * PERIN_VALUE));
	nPerinNum += nTotalPerins;

	if (pBuyer->GetPerinNum() < nPerinNum)
	{
		result.nErrorCode = TID_GAME_LACKMONEY;
		return FALSE;
	}

	if (pBuyer->GetGold() < nPenyaNum)
	{
		if (pBuyer->GetPerinNum() < (nPerinNum + 1))
		{
			result.nErrorCode = TID_GAME_LACKMONEY;
			return FALSE;
		}
	}

	CItemElem* pItemElem = pItemBase;
	CItemElem itemElem;
	itemElem = *pItemElem;
	itemElem.m_nItemNum = nNum;

	if (pBuyer->CreateItem(&itemElem) == FALSE)
	{
		result.nErrorCode = TID_GAME_LACKSPACE;
		return FALSE;
	}

	char szText[512];
	sprintf_s<512>(szText, "[%s] bought [%d x %s]. Total: [%s] Perins, [%s] Penya.", pBuyer->GetName(), nNum, pItemBase->GetProp()->szName, Separator(nPerinNum), Separator(nPenyaNum));

	u_long idRealSeller = NULL_ID;
	if (m_pOwner->IsPlayer())
	{
		idRealSeller = m_pOwner->m_idPlayer;
		((CUser*)m_pOwner)->AddText(szText);
	}
	else
	{
		idRealSeller = m_pOwner->GetNpcOwner();
		CUser* pUser = (CUser*)prj.GetUserByID(idRealSeller);
		if (IsValidObj(pUser))
			pUser->AddText(szText);
	}

	// Send perins & penya to seller
	CItemElem itemElem2;
	itemElem2.m_dwItemId = II_SYS_SYS_SCR_PERIN;

	int nPerinNumTmp = nPerinNum;
	while (nPerinNumTmp > 9999)
	{
		itemElem2.m_nItemNum = 9999;
		g_dpDBClient.SendQueryPostMail(idRealSeller, 0, itemElem2, 0, "Sold item", szText);
		nPerinNumTmp -= 9999;
	}

	if (nPerinNumTmp > 0)
	{
		itemElem2.m_nItemNum = nPerinNumTmp;
		g_dpDBClient.SendQueryPostMail(idRealSeller, 0, itemElem2, nPenyaNum, "Sold item", szText);
	}
	else
		g_dpDBClient.SendQueryPostMail(idRealSeller, 0, CItemElem(), nPenyaNum, "Sold item", szText);

	// Remove perin & penya from buyer
	if (nPerinNum > 0)
		pBuyer->RemovePerin(nPerinNum);

	if (nPenyaNum > 0)
	{
		if (pBuyer->GetGold() < nPenyaNum)
		{
			pBuyer->RemovePerin(1);
			int nGivePenyaBack = (int)(PERIN_VALUE - nPenyaNum);
			pBuyer->AddGold(nGivePenyaBack);
		}
		else
			pBuyer->AddGold(-(nPenyaNum));
	}
#endif

	const int nRemain = pItemBase->GetExtra() - nNum;
	VendorItemNum(i, nRemain);
	g_UserMng.AddPVendorItemNum((CUser*)m_pOwner, i, nRemain, pBuyer->GetName());

	m_pOwner->RemoveItem((BYTE)pItemBase->m_dwObjId, nNum);

	if (pBuyer->IsPlayer())
	{
		if (((CUser*)pBuyer)->GetWorld())
			g_dpDBClient.SavePlayer((CUser*)pBuyer, ((CUser*)pBuyer)->GetWorld()->GetID(), ((CUser*)pBuyer)->GetPos(), ((CUser*)pBuyer)->GetLayer());
	}
	if (m_pOwner->IsPlayer())
	{
		if (((CUser*)m_pOwner)->GetWorld())
			g_dpDBClient.SavePlayer((CUser*)m_pOwner, ((CUser*)m_pOwner)->GetWorld()->GetID(), ((CUser*)m_pOwner)->GetPos(), ((CUser*)m_pOwner)->GetLayer());
	}
#ifdef __FL_VENDORS
	else
		g_dpDBClient.SendUpdateVendor(m_pOwner);
#endif

	result.item = itemElem;
	result.item.m_nCost = nCost;
	result.nRemain = nRemain;
	return TRUE;
}
#endif // __WORLDSERVER