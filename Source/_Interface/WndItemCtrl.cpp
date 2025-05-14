// WndBase.cpp: implementation of the CWndBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "DefineText.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;

#include "WndManager.h"
#include "WndText.h"
#include "WndBagEx.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndItemCtrl
//////////////////////////////////////////////////////////////////////////////////////////////////////

CTextureMng CWndItemCtrl::m_textureMng;

CWndItemCtrl::CWndItemCtrl()
{
	m_nWndColor = D3DCOLOR_TEMP(255, 0x5 << 3, 0x5 << 3, 0x5 << 3);
	m_nFontColor = D3DCOLOR_TEMP(255, 255, 255, 255);
	m_nSelectColor = D3DCOLOR_TEMP(255, 255, 255, 0);
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_pItemContainer = NULL;
	m_bDrag = FALSE;
	m_nOnSelect = -1;

	memset(m_pArrayItemElem, 0, sizeof(m_pArrayItemElem));
	m_nArrayCount = 0;
	m_bVisibleCount = TRUE;
}
CWndItemCtrl::~CWndItemCtrl()
{
}
void CWndItemCtrl::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(WBS_CHILD | dwListCtrlStyle, rect, pParentWnd, nID);
}
void CWndItemCtrl::InitItem(CItemContainer<CItemElem>* pItemContainer, DWORD SendToId)
{
	m_pItemContainer = pItemContainer;
	m_pFocusItem = NULL;
	m_dwFrame = SendToId;
}

void CWndItemCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);

	CheckTexture = m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, "WndItemCheck.bmp"), 0xffff00ff);

}

void CWndItemCtrl::UpdateTooltip(void)
{
	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	CPoint point = GetMousePoint();
	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
	{
		if (i < 0)
			continue;

		int x = (i - pt.y) % nWidth;
		int y = (i - pt.y) / nWidth;

		CItemElem* pItemElem = GetItem(i);
		if (pItemElem == NULL)
			continue;

		CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);
		if (rectHittest.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rectHittest);

			g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, m_dwFrame);
		}
	}
}


void CWndItemCtrl::OnMouseWndSurface(CPoint point)
{
	if (!m_pItemContainer)
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	CPoint pt;
	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	CWndBase* pWndBase = GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
	{
		if (i < 0)
			continue;

		int x = (i - pt.y) % nWidth;
		int y = (i - pt.y) / nWidth;
		CItemElem* pItemElem = GetItem(i);
		if (!pItemElem)
			continue;

		CRect rectHittest = CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2);

		if (rectHittest.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rectHittest);

			g_WndMng.PutToolTip_Item(pItemElem, point2, &rectHittest, m_dwFrame);
		}
	}

}

void CWndItemCtrl::RanderIcon(C2DRender* p2DRender, CItemElem* pItemElem, int x, int y, int nParent, int nalpha)
{
	int bRander = 0;

	if (nParent == APP_SHOP_)
	{
		ItemProp* pItemProp = pItemElem->GetProp();
		if (pItemProp == NULL) return;

		switch (pItemProp->dwItemKind2)
		{
		case IK2_WEAPON_DIRECT:
		case IK2_WEAPON_MAGIC:
		case IK2_ARMOR:
		case IK2_ARMORETC:
		case IK2_CLOTH:
		case IK2_CLOTHETC:
		{

			if (pItemProp->dwItemSex != 0xffffffff && pItemProp->dwItemSex != g_pPlayer->GetSex())
			{
				bRander = 1;
				break;
			}

			if (JOB_VAGRANT != pItemProp->dwItemJob)
			{
				if (pItemProp->dwItemJob != 0xffffffff && JOB_VAGRANT < pItemProp->dwItemJob && !(g_pPlayer->IsInteriorityJob(pItemProp->dwItemJob)))
				{
					bRander = 1;
					break;
				}
			}
			if (pItemProp->dwLimitLevel1 != 0xffffffff && pItemElem->IsLimitLevel(g_pPlayer))
			{
				bRander = 1;
				break;
			}
			break;
		}
		case IK2_RIDING:
		{
			if (g_pPlayer->GetFlightLv() < (int)(pItemProp->dwFlightLimit))
			{
				bRander = 1;
				break;
			}
			break;
		}
		}

		if (nParent == APP_SHOP_)
		{
			CWndShop* pWndBase = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
			LPCHARACTER lpCharacter = pWndBase->m_pMover->GetCharacter();

			if (lpCharacter)
			{
				if (lpCharacter->m_nVenderType == 0)
				{
					if (bRander == 0 && g_pPlayer->GetGold() < pItemElem->GetCost())
						bRander = 1;
				}
#ifndef __NPC_SETVENDORTYPE
				else if (lpCharacter->m_nVenderType == 1)
				{
					if (bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum(II_CHP_RED) < (int)pItemElem->GetChipCost())
						bRander = 1;
				}
#else
				else
				{
					if (bRander == 0 && g_pPlayer->m_Inventory.GetAtItemNum(lpCharacter->m_nVenderType) < (int)pItemElem->GetChipCost())
						bRander = 1;
				}
#endif
			}
		}
		else
		{
			if (bRander == 0 && g_pPlayer->GetGold() < (int)pItemProp->dwCost)
				bRander = 1;
		}

	}

#ifdef __FL_VENDORS
	if (pItemElem->IsFlag(CItemElem::expired) && pItemElem->GetProp() && pItemElem->GetProp()->dwItemKind3 != IK3_VENDOR)
#else
	if (pItemElem->IsFlag(CItemElem::expired))
#endif
	{
		bRander = 1;
	}

	CWndBase* pWndBase = GetParentWnd();
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
		{
			nParent = pWndBaseTemp->GetWndId();
			if (nParent == APP_INVENTORY || nParent == APP_GUILD_BANK || nParent == APP_SHOPEX || nParent == APP_COMMON_BANK
#ifdef __FL_CONSUMABLES_STORAGE
				|| nParent == APP_CHEST
#endif
				)
			{
				CWndEdit* pSearchBox = (CWndEdit*)pWndBaseTemp->GetDlgItem(WIDC_EDIT1);
				if (pSearchBox)
				{
					CString strFilter = pSearchBox->GetString();
					if (!strFilter.IsEmpty())
					{
						CString strName = pItemElem->GetName();
						if (!strName.IsEmpty())
						{
							if (pItemElem->GetAbilityOption())
								strName.Format("%s %+d", strName, pItemElem->GetAbilityOption());

							if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
								nalpha = 50;
						}
					}
				}
			}
		}
	}

	if (bRander == 0)
		pItemElem->GetTexture()->Render(p2DRender, CPoint(x, y), nalpha);
	else if (bRander == 1)
		pItemElem->GetTexture()->Render2(p2DRender, CPoint(x, y), D3DCOLOR_ARGB(nalpha, 255, 100, 100));
}

void CWndItemCtrl::OnDraw(C2DRender* p2DRender)
{
#ifdef __SYS_TICKET
	CMover* pPlayer = g_pPlayer;
	if (!pPlayer)
		return;
	CWorld* pWorld = pPlayer->GetWorld();
	if (!pWorld)
		return;
#endif	// __SYS_TICKET

	if (!m_pItemContainer)
		return;

	DWORD dwCur = GetTickCount();
	CPoint pt(3, 3);

	CWndBase* pWndBase = GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;

	int nPage = nHeight;
	int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
	if (m_pItemContainer->m_dwIndexNum % nWidth)
		nRange++;
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);

	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	BOOL bNotAvailable = FALSE;
	int nType = g_WndMng.GetContainerTypeByWindowId(nParent, pWndBase->GetWndId());
	switch (nType)
	{
	case TYPE_BANK_0:
	case TYPE_BANK_1:
	case TYPE_BANK_2:
	{
		CWndBank* pWnd = (CWndBank*)GetWndBase(APP_COMMON_BANK);
		if (pWnd)
		{
			switch (nType)
			{
			case TYPE_BANK_0: if (!pWnd->bUse[0]) bNotAvailable = TRUE; break;
			case TYPE_BANK_1: if (!pWnd->bUse[1]) bNotAvailable = TRUE; break;
			case TYPE_BANK_2: if (!pWnd->bUse[2]) bNotAvailable = TRUE; break;
			}
		}
	}
	break;
	default: break;
	}

	for (int i = pt.y; i < m_pItemContainer->GetSize(); i++)
	{
		if (i < 0)
			continue;

		int nMaxItem = (nPage + m_wndScrollBar.GetScrollPos()) * nWidth;
		if (i >= nMaxItem)
			break;

		int x = (i - pt.y) % nWidth;
		int y = (i - pt.y) / nWidth;

		CPoint point;
		point = CPoint(x * 32, y * 32);
		CRect crBoard;
		crBoard.left = point.x;
		crBoard.top = point.y;
		crBoard.right = point.x + 31;
		crBoard.bottom = crBoard.top + 31;

		if (crBoard.PtInRect(GetMousePoint()) && !bNotAvailable)
		{
			m_nOnSelect = i;
			p2DRender->RenderRect(crBoard, 0xff118701);
		}
		else
		{
			m_nOnSelect = -1;
			p2DRender->RenderRect(crBoard, D3DCOLOR_ARGB(175, 198, 179, 172));
		}

		CRect crRect = crBoard;
		crRect.DeflateRect(1, 1, 1, 1);
		p2DRender->RenderFillRect(crRect, D3DCOLOR_ARGB(255, 255, 255, 255));

		CItemElem* pItemElem = GetItem(i);
		if (pItemElem == NULL)
			continue;

		if (pItemElem->GetTexture())
		{
			if (IsUsingItem(pItemElem) || (nParent == APP_INVENTORY && pPlayer->IsUsing(pItemElem)))
			{
				RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, 60);

				if (CheckTexture)
					CheckTexture->Render(p2DRender, CPoint(x * 32, y * 32), 255);
			}
			else if (m_nOnSelect == i)
			{
				pItemElem->GetTexture()->m_size.cx += 4;
				pItemElem->GetTexture()->m_size.cy += 4;
				if ((pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0)
#ifdef __SYS_TICKET
					|| pPlayer->GetTicket() == pItemElem
#endif	// __SYS_TICKET
					)
					RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent, 100);
				else
					RanderIcon(p2DRender, pItemElem, x * 32 - 2, y * 32 - 2, nParent);
				pItemElem->GetTexture()->m_size.cx -= 4;
				pItemElem->GetTexture()->m_size.cy -= 4;
			}
			else
			{
				if (pItemElem->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET && pItemElem->m_dwKeepTime <= 0)
					RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent, 100);
				else
					RanderIcon(p2DRender, pItemElem, x * 32, y * 32, nParent);
			}


			if (pItemElem->GetProp()->dwPackMax > 1 && nParent != APP_SHOP_)
			{
				short m_nItemNum = pItemElem->m_nItemNum;
				if (pItemElem->GetExtra() > 0)
					m_nItemNum -= (short)pItemElem->GetExtra();
				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", m_nItemNum);

				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
				CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
				m_p2DRender->TextOut(x * 32 + 32 - size.cx - 2, y * 32 + 34 - size.cy, szTemp, 0xffeaebdd);
				p2DRender->SetFont(pOldFont);
			}

			DWORD dwGroup = g_pPlayer->m_cooltimeMgr.GetGroup(pItemElem->GetProp());
			if (dwGroup)
			{
				DWORD dwEnd = g_pPlayer->m_cooltimeMgr.GetTime(dwGroup);
				if (dwEnd != 0 && dwEnd > dwCur)
				{
					DWORD dwBase = g_pPlayer->m_cooltimeMgr.GetBase(dwGroup);
					CPoint pt(x * 32, y * 32);
					RenderRadar(p2DRender, pt, dwCur - dwBase, dwEnd - dwBase);
				}
			}

			CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
			if (pWndInventory)
			{
				if (pWndInventory->m_pUpgradeItem && pItemElem)
				{
					if (pWndInventory->m_pUpgradeItem == pItemElem)
					{
						if (pWndInventory->m_dwEnchantWaitTime != 0xffffffff)
						{
							CPoint pt(x * 32, y * 32);
							RenderEnchant(p2DRender, pt);
						}
					}
				}
			}
		}
	}

	if (bNotAvailable)
		p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB(80, 255, 0, 0));
}
void CWndItemCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!g_pPlayer)
		return;

	if (!m_pItemContainer)
		return;

	CPoint pt(3, 3);
	CRect rect;
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		return;
	if (CWndBase::m_GlobalShortcut.IsEmpty() == FALSE)
		return;

	int nItem = HitTest(point);
	if (nItem <= -1)
		return;

	CWndBase* pWndBase = GetParentWnd();
	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	CItemElem* pItemElem = NULL;
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);

	if (nParent == APP_INVENTORY)
	{
		if (pWndInventory && pWndInventory->m_dwEnchantWaitTime != 0xffffffff || GetWndBase(APP_SMELT_SAFETY_CONFIRM) != NULL)
		{
			g_WndMng.PutString(prj.GetText(TID_MMI_NOTUPGRADE), NULL, prj.GetTextColor(TID_MMI_NOTUPGRADE));
			m_pFocusItem = NULL;
			return;
		}
	}

	if (GetWndBase(APP_EQUIP_BIND_CONFIRM) != NULL)
	{
		g_WndMng.PutString(prj.GetText(TID_TOOLTIP_EQUIPBIND_ERROR01), NULL, prj.GetTextColor(TID_TOOLTIP_EQUIPBIND_ERROR01));
		m_pFocusItem = NULL;
		return;
	}

	if (GetWndBase(APP_COMMITEM_DIALOG) != NULL)
	{
		g_WndMng.PutString(prj.GetText(TID_TOOLTIP_ITEM_USING_ERROR), NULL, prj.GetTextColor(TID_TOOLTIP_ITEM_USING_ERROR));
		m_pFocusItem = NULL;
		return;
	}

	pItemElem = GetItem(nItem);
	if (pItemElem)
	{
		if (IsUsableItem(pItemElem))
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add(nItem);
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
			m_bDrag = TRUE;
		}

		if (nParent == APP_INVENTORY && pWndInventory)
		{
			CPoint pt(3, 3);

			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
			if (m_pItemContainer->m_dwIndexNum % nWidth)
				nRange++;
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);

			pt.y = 0;
			pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

			int x = (nItem - pt.y) % nWidth;
			int y = (nItem - pt.y) / nWidth;

			if (pWndInventory->m_bIsUpgradeMode && pWndInventory->m_pUpgradeMaterialItem != NULL && m_pFocusItem != NULL)
			{
				if (!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT)
					&& pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM01
					&& m_pFocusItem->GetAbilityOption() >= 3 && CItemElem::IsDiceRefineryAble(m_pFocusItem->GetProp()))
				{
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm);
					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR1);
					CWndBase::m_GlobalShortcut.Empty();
					m_bDrag = FALSE;
					g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
					g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
				}
				else if (!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT3) && pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02 &&
					m_pFocusItem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
				{
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm);
					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR2);
					CWndBase::m_GlobalShortcut.Empty();
					m_bDrag = FALSE;
					g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
					g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
				}
				else if (!g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT4) && m_pFocusItem->GetAbilityOption() >= 3
					&& pWndInventory->m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE
					&& m_pFocusItem->GetProp()->dwItemKind2 == IK2_JEWELRY)
				{
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm);
					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR3);
					CWndBase::m_GlobalShortcut.Empty();
					m_bDrag = FALSE;
					g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
					g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
				}
				else if (g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_SMELPROT) == FALSE &&
					m_pFocusItem->GetResistAbilityOption() >= 3 &&
					CItemElem::IsElementalCard(pWndInventory->m_pUpgradeMaterialItem->m_dwItemId) == TRUE &&
					(m_pFocusItem->GetProp() && m_pFocusItem->GetProp()->IsEleRefineryAble()) == TRUE)
				{
					SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm);
					g_WndMng.m_pWndSmeltSafetyConfirm = new CWndSmeltSafetyConfirm(CWndSmeltSafetyConfirm::WND_ERROR1);
					CWndBase::m_GlobalShortcut.Empty();
					m_bDrag = FALSE;
					g_WndMng.m_pWndSmeltSafetyConfirm->SetWndMode(m_pFocusItem);
					g_WndMng.m_pWndSmeltSafetyConfirm->Initialize(NULL);
				}
				else
					pWndInventory->RunUpgrade(m_pFocusItem);
			}
		}
	}
	else
	{
		m_dwSelectAry.RemoveAll();
		m_nCurSelect = nItem;
		m_pFocusItem = NULL;
	}
}

void CWndItemCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_pItemContainer)
		return;

	m_bDrag = FALSE;
	int nItem = HitTest(point);
	if (nItem <= -1) return;

	CWndBase* pWndBase = this->GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	CItemElem* pItemElem = NULL;
	pItemElem = GetItem(nItem);

	if (pItemElem)
	{
		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000))
		{
			int j = NULL;
			for (; j < m_dwSelectAry.GetSize(); j++)
			{
				if (m_dwSelectAry.GetAt(j) == nItem)
					break;
			}
			if (j == m_dwSelectAry.GetSize())
				m_dwSelectAry.Add(nItem);
			else
				if (m_nCurSelect != nItem)
					m_dwSelectAry.RemoveAt(j);
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}
		else if (IsSelectedItem(nItem))
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add(nItem);
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}
	}
}
void CWndItemCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_pItemContainer)
		return;

	if (m_bDrag == FALSE)
		return;

	m_bDrag = FALSE;
	CPoint pt(3, 3);
	CRect rect;

	CWndBase* pWndBase = GetParentWnd();

	int nParent;
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (pWndBaseTemp)
			nParent = pWndBaseTemp->GetWndId();
		else
			return;
	}
	else
		return;

	CItemElem* pItemElem = NULL;
	pItemElem = GetItem(m_nCurSelect);

	if (IsUsableItem(pItemElem))
	{
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
		m_GlobalShortcut.m_dwIndex = m_nCurSelect;
		m_GlobalShortcut.m_dwType = 0;
		m_GlobalShortcut.m_dwId = pItemElem->m_dwObjId;
		m_GlobalShortcut.m_pTexture = pItemElem->GetTexture();
		m_GlobalShortcut.m_dwData = (DWORD)pItemElem;
		m_GlobalShortcut.m_nLocation = g_WndMng.GetContainerTypeByWindowId(nParent, pWndBase->GetWndId());
		_tcscpy(m_GlobalShortcut.m_szString, pItemElem->GetName());
	}
}
CItemElem* CWndItemCtrl::GetItem(DWORD dwIndex)
{
	return m_pItemContainer->GetAt(dwIndex);
}

CItemElem* CWndItemCtrl::GetItemFromArr(DWORD dwIndex)
{
	ASSERT(dwIndex < MAX_VENDOR_INVENTORY);
	return m_pArrayItemElem[dwIndex];
}

int CWndItemCtrl::HitTest(CPoint point)
{
	if (!m_pItemContainer)
		return -1;

	int nDstId = -1;

	CRect rect = GetClientRect();
	int nWidth = rect.Width() / 32;
	int nHeight = rect.Height() / 32;
	CPoint pt(3, 3);

	pt.y = 0;
	pt.y += m_wndScrollBar.GetScrollPos() * nWidth;

	CWndBase* pWndBase = GetParentWnd();
	if (pWndBase)
	{
		CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
		if (!pWndBaseTemp)
			return nDstId;
	}
	else
		return nDstId;


	for (int i = pt.y; i < (int)(m_pItemContainer->m_dwIndexNum); i++)
	{
		int x = (i - pt.y) % nWidth;
		int y = (i - pt.y) / nWidth;
		rect.SetRect(x * 32, y * 32, x * 32 + 32, y * 32 + 32);
		if (rect.PtInRect(point))
		{
			nDstId = i;
			break;
		}
	}
	return nDstId;
}
BOOL CWndItemCtrl::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (!m_pItemContainer)
		return FALSE;

	if (pShortcut->m_dwShortcut == SHORTCUT_ITEM)
	{
		int nDstIndex = HitTest(point);
		if (nDstIndex == -1)
			nDstIndex = m_pItemContainer->m_dwIndexNum;

		if (pShortcut->m_pFromWnd == this)
		{
			CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
			if (pWndFrame->GetWndId() == APP_BANK)
			{
				g_DPlay.SendMoveBankItem((BYTE)(pShortcut->m_dwIndex), (BYTE)((DWORD)nDstIndex));
			}
			else if (pWndFrame->GetWndId() == APP_INVENTORY)
			{
				CItemElem* pItemElem = m_pItemContainer->GetAt(pShortcut->m_dwIndex);
				CItemElem* pItemElem2 = m_pItemContainer->GetAt((DWORD)nDstIndex);
				if (IsUsableItem(pItemElem) && (!pItemElem2 || IsUsableItem(pItemElem2)))
				{
					g_DPlay.SendMoveItem(0, (BYTE)(pShortcut->m_dwIndex), (BYTE)((DWORD)nDstIndex));
				}
			}
#ifdef __FL_CONSUMABLES_STORAGE
			else if (pWndFrame->GetWndId() == APP_CHEST)
			{
				CItemElem* pItemElem = m_pItemContainer->GetAt(pShortcut->m_dwIndex);
				if (pItemElem)
					g_DPlay.SendMoveItemInsideChest((BYTE)(pShortcut->m_dwIndex), (BYTE)((DWORD)nDstIndex));
			}
#endif
		}
		else
		{
			CWndBase* pParent = (CWndBase*)GetParentWnd();
			pParent->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CWndItemCtrl::IsSelectedItem(int iItem)
{
	for (int i = 0; i < m_dwSelectAry.GetSize(); i++)
		if (m_dwSelectAry.GetAt(i) == iItem)
			return TRUE;
	return FALSE;
}
int CWndItemCtrl::GetSelectedItem(int i)
{
	return m_dwSelectAry.GetAt(i);
}
UINT CWndItemCtrl::GetSelectedCount()
{
	return m_dwSelectAry.GetSize();
}

void CWndItemCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (!m_pItemContainer)
		return;

	CWndBase* pWndBase = GetParentWnd();
	if (!pWndBase)
		return;

	CWndBase* pWndBaseTemp = pWndBase->GetParentWnd();
	if (!pWndBaseTemp)
		return;

	int nItem = HitTest(point);
	if (nItem <= -1)
		return;

	CItemElem* pItem = GetItem(nItem);
	if (!IsUsableItem(pItem))
		return;

	if (IsSelectedItem(nItem) == FALSE)
	{
		m_dwSelectAry.RemoveAll();
		m_dwSelectAry.Add(nItem);
		m_nCurSelect = nItem;
		m_pFocusItem = pItem;
	}

#ifdef __FL_BOXES_V2
	if (pWndBaseTemp->GetWndId() == APP_INVENTORY)
	{
		ItemProp* pProp = pItem->GetProp();
		if (pProp && (pProp->nBoxType == BOX_TYPE_PACKITEM || pProp->nBoxType == BOX_TYPE_GIFTBOX))
		{
			pWndBase->OnChildNotify(WNM_TRACK, m_nIdWnd, (LRESULT*)pItem);
			return;
		}
	}
#endif
#ifdef __CONTAINER_RCLICKS
	pWndBaseTemp->OnChildNotify(WNM_ITEMCLICK, m_nIdWnd, (LRESULT*)pItem);
#endif
}

void CWndItemCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if (pWndInventory && pWndInventory->m_dwEnchantWaitTime == 0xffffffff)
	{
		pWndInventory->BaseMouseCursor();
		if (g_WndMng.m_pWndSmeltSafetyConfirm != NULL)
			SAFE_DELETE(g_WndMng.m_pWndSmeltSafetyConfirm)
	}

	if (!m_pItemContainer)
		return;
}

BOOL CWndItemCtrl::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if (pWndInventory)
		pWndInventory->OnSetCursor(pWndBase, nHitTest, message);
	else
		CWndBase::OnSetCursor(pWndBase, nHitTest, message);

	return TRUE;
}

void CWndItemCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!g_pPlayer)
		return;

	if (!m_pItemContainer)
		return;

	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		return;

	if (CWndBase::m_GlobalShortcut.IsEmpty() == FALSE)
		return;

	int nItem = HitTest(point);
	if (nItem <= -1)
		return;

	CWndBase* pParent = (CWndBase*)GetParentWnd();
	if (!pParent)
		return;

	CItemElem* pItemElem = GetItem(nItem);
	if (!pItemElem)
		return;

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
#ifdef __FL_PACKED_ITEMS
		if (pItemElem->IsPackage())
			g_WndMng.PreviewPackage(pItemElem);
#endif
#ifdef __MODEL_VIEW
		else
			g_WndMng.PreviewModel(pItemElem);
#endif
		return;
	}

	if (IsUsableItem(pItemElem))
	{
		if (IsSelectedItem(nItem) == FALSE)
		{
			m_dwSelectAry.RemoveAll();
			m_dwSelectAry.Add(nItem);
			m_nCurSelect = nItem;
			m_pFocusItem = pItemElem;
		}

		pParent->OnChildNotify(WIN_DBLCLK, m_nIdWnd, (LRESULT*)pItemElem);
	}
}
void CWndItemCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndItemCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect(rect);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndItemCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(4, 0);

	if (m_pItemContainer)
	{
		if (1)
		{
			CRect rect = GetClientRect();
			int nWidth = rect.Width() / 32;
			int nHeight = rect.Height() / 32;
			int nPage = nHeight;
			int nRange = m_pItemContainer->m_dwIndexNum / nWidth;
			m_rectClient.right -= 15;
		}
	}
	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndItemCtrl::PaintFrame(C2DRender* p2DRender)
{
	return;
}

BOOL CWndItemCtrl::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
BOOL CWndItemCtrl::SetItem(const LVITEM* pItem)
{
	if (pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt(pItem->iItem) == NULL)
		return FALSE;
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt(pItem->iItem);
	memcpy(&pItems[pItem->iSubItem], pItem, sizeof(LVITEM));
	pItems[pItem->iSubItem].pszText = new _TCHAR[_tcslen(pItem->pszText) + sizeof(_TCHAR)];
	_tcscpy(pItems[pItem->iSubItem].pszText, pItem->pszText);
	return TRUE;
}

int CWndItemCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndItemCtrl::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy(pNewColumn, pColumn, sizeof(LVCOLUMN));
	pNewColumn->pszText = new _TCHAR[_tcslen(pColumn->pszText)];
	_tcscpy(pNewColumn->pszText, pColumn->pszText);
	if (nCol < m_aColumns.GetSize() && m_aColumns.GetAt(nCol))
		return -1;
	m_aColumns.SetAtGrow(nCol, (void*)pNewColumn);
	return nCol;
}
BOOL CWndItemCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0)
	{
		if (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() + 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage());
	}
	else
	{
		if (m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos())
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetScrollPos() - 1);
		else
			m_wndScrollBar.SetScrollPos(m_wndScrollBar.GetMinScrollPos());
	}

	return TRUE;
}

#ifdef __INVENTORY_RESIZE
int CWndItemCtrl::getScrollbarPos()
{
	return (m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos());
}

void CWndItemCtrl::SetMinScrollPos()
{
	return m_wndScrollBar.SetMinScrollPos();
}

int CWndItemCtrl::getScrollbarMax()
{
	return m_wndScrollBar.GetMaxScrollPos();
}
#endif