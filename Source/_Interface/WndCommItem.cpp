#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCommItem.h"
#include "DPClient.h"
extern	CDPClient	g_DPlay;




CWndCommItem::CWndCommItem()
{
}
CWndCommItem::~CWndCommItem()
{
}
void CWndCommItem::OnDraw(C2DRender* p2DRender)
{
}
void CWndCommItem::OnInitialUpdate()
{
#ifdef __TAIWAN__
	Destroy();
#endif // __TAIWAN__

	CWndNeuz::OnInitialUpdate();


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;
	m_wndCommItemCtrl.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COMM_ITEM);


	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_ITEMTIME);
	tabTabItem.pWndBase = &m_wndCommItemCtrl;
	pWndTabCtrl->InsertItem(0, &tabTabItem);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndCommItem::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COMM_ITEM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCommItem::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCommItem::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndCommItem::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCommItem::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndCommItem::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}






/////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPartyCtrl
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndCommItemCtrl::CWndCommItemCtrl()
{
	m_pVBGauge = NULL;
	pWndWorld = NULL;
	memset(m_dwDraw, 0, sizeof(int) * (SM_MAX + MAX_SKILLINFLUENCE));
	m_nMaxDraw = 0;
}
CWndCommItemCtrl::~CWndCommItemCtrl()
{
	DeleteDeviceObjects();
}

void CWndCommItemCtrl::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	m_dwListCtrlStyle = dwListCtrlStyle;
	CWndBase::Create(WBS_CHILD, rect, pParentWnd, nID);
}

void CWndCommItemCtrl::LoadListBoxScript(LPCTSTR lpFileName)
{
}
void CWndCommItemCtrl::InterpriteScript(CScanner& scanner, CPtrArray& ptrArray)
{
}
HRESULT CWndCommItemCtrl::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if (m_pVBGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);

	return S_OK;
}
HRESULT CWndCommItemCtrl::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndCommItemCtrl::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
void CWndCommItemCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);//,m_pSprPack,-1);
	m_nFontHeight = 30;

	pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptySmall.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauFillSmall.bmp"), 0xffff00ff, TRUE);
}
void CWndCommItemCtrl::OnMouseWndSurface(CPoint point)
{
	CPoint pt(3, 3);

	if (NULL == pWndWorld)
	{
		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		return;
	}

	int nCountinueCount = 0;
	for (int i = 0; i < m_nMaxDraw; i++)
	{
		int x = 0, nWidth = m_rectClient.Width();// - 1;
		CRect rectHittest = CRect(x + 3, pt.y, x + 3 + nWidth, pt.y + 32);// pt.x, pt.y+ 12, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight+ 12 );

		if (rectHittest.PtInRect(point))
		{
			ClientToScreen(&point);
			ClientToScreen(&rectHittest);
			ItemProp* pItem = prj.GetItemProp(m_dwDraw[i]);
			if (pItem == NULL)
				continue;

			CEditString str;
			CString strTemp;
			CString strEnter = '\n';
			str.AddString(pItem->szName, 0xff0000ff, ESSTY_BOLD);
			str += strEnter;
			if (pItem->dwID == II_SYS_SYS_SCR_SMELPROT || pItem->dwID == II_SYS_SYS_SCR_SMELTING
				|| pItem->dwID == II_SYS_SYS_SCR_SUPERSMELTING
				|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT3
				|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT4
				|| pItem->dwID == II_SYS_SYS_SCR_SMELTING2
				)
			{
				strTemp.Format("%s\n", prj.GetText(TID_GAME_DEMOL_USE));
				str += strTemp;
			}
			else
			{
#ifdef __BS_ITEM_UNLIMITEDTIME
				if (pItem->dwSkillTime == 999999999 && pItem->dwCircleTime == (DWORD)-1 && pItem->dwAbilityMin == (DWORD)-1)
				{
					strTemp.Format("%s\n", prj.GetText(TID_GAME_TOOLTIP_PERMANENTTIME_1));
					str += strTemp;
				}
#endif //
			}

			strTemp.Format("%s", pItem->szCommand);
			str += strTemp;
			g_toolTip.PutToolTip(m_dwDraw[i], str, rectHittest, point, 0);
		}
		pt.y += m_nFontHeight;
	}
}

int CWndCommItemCtrl::GetMaxBuff()
{
	int nMaxCount = 0;
	ItemProp* pItem = NULL;
	int i = 0;

	for (i = 0; i < SM_MAX; ++i)
	{
		if (0 < g_pPlayer->m_dwSMTime[i])
			++nMaxCount;
	}

	nMaxCount += g_pPlayer->m_buffs.GetCount();
	return nMaxCount;
}

void CWndCommItemCtrl::DrawSM(C2DRender* p2DRender, CPoint* pPoint, int x, int& nScroll)
{
	for (int i = 0; i < SM_MAX; ++i)
	{
		if (g_pPlayer->m_dwSMTime[i] <= 0)
			continue;

		++nScroll;
		if ((m_wndScrollBar.GetScrollPos() >= nScroll))
			continue;

		ItemProp* pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[i]);

		if (pItem != NULL && pWndWorld->m_dwSMItemTexture[i] != NULL)
		{
			p2DRender->RenderTexture(CPoint(2, pPoint->y), pWndWorld->m_dwSMItemTexture[i], 192);
			m_dwDraw[m_nMaxDraw] = g_AddSMMode.dwSMItemID[i];
			++m_nMaxDraw;
		}

		DWORD dwColor = 0xff000000;
		CString string;
		string.Format("#cff0000ff%s#nc", pItem->szName);
		CEditString strEdit;
		strEdit.SetParsingString(string);
		p2DRender->TextOut_EditString(x + 40, pPoint->y + 3, strEdit, 0, 0, 2);

		CTimeSpan ct(g_pPlayer->m_dwSMTime[i]);
		string.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
		p2DRender->TextOut(x + 40, pPoint->y + 18, string, dwColor);
		pPoint->y += m_nFontHeight;// + 3;
	}
}

void CWndCommItemCtrl::DrawSkill(C2DRender* p2DRender, CPoint* pPoint, int x, int& nScroll)
{
	ItemProp* pItem = NULL;
	multimap<DWORD, BUFFSKILL>::value_type* pp = NULL;
	BOOL bExpRander[6];

	for (int iRander = 0; iRander < 6; ++iRander)
		bExpRander[iRander] = TRUE;
	for (auto & it1 : g_pPlayer->m_buffs.m_mapBuffs)
	{
		IBuff* ptr1 = it1.second;
		if (ptr1->GetType() == BUFF_SKILL || ptr1->GetType() == BUFF_EQUIP)
			continue;
		DWORD dwSkillID = ptr1->GetId();
		pItem = prj.GetItemProp(dwSkillID);

		if (pItem->dwItemKind3 == IK3_EGG)
			continue;

		if (pItem->dwItemKind1 == IK1_HOUSING)
			continue;

		auto it = pWndWorld->m_pBuffTexture[2].find(dwSkillID);
		if (it != pWndWorld->m_pBuffTexture[2].end())
			pp = &(*it);

		if (pp == NULL)
			continue;

		if (pp->second.m_pTexture == NULL)
			continue;

		++nScroll;
		if ((m_wndScrollBar.GetScrollPos() >= nScroll))
			continue;

		p2DRender->RenderTexture(CPoint(2, pPoint->y), pp->second.m_pTexture, 192);

		DWORD dwColor = 0xff000000;
		CString string;
		string.Format("#cff0000ff%s#nc", pItem->szName);
		CEditString strEdit;
		strEdit.SetParsingString(string);
		p2DRender->TextOut_EditString(x + 40, pPoint->y + 3, strEdit, 0, 0, 2);

		if (pItem->dwID == II_SYS_SYS_SCR_SMELPROT || pItem->dwID == II_SYS_SYS_SCR_SMELTING
			|| pItem->dwID == II_SYS_SYS_SCR_SUPERSMELTING
			|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT3
			|| pItem->dwID == II_SYS_SYS_SCR_SMELPROT4
			|| pItem->dwID == II_SYS_SYS_SCR_SMELTING2
			)
		{
			string.Format("%s", prj.GetText(TID_GAME_DEMOL_USE));
		}
		else if (pItem->dwID == II_SYS_SYS_SCR_RETURN)
		{
			string.Format("%s", prj.GetText(TID_TOOLTIP_RETURN_USE));
		}
		else
		{
			if (pItem->dwItemKind2 == IK2_BUFF2)
			{
				time_t	t = (time_t)ptr1->GetLevel() - time_null();
				if (t < 0)
					t = 0;
				CTimeSpan ts(t);

				CString str;
				str.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(ts.GetDays()), ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
				string = str;
			}
			else if (pItem->dwItemKind3 == IK3_TICKET)
			{
				CItemElem* pTicket = g_pPlayer->GetTicket();
				CString str;
				if (pTicket)
				{
					time_t t = pTicket->m_dwKeepTime - time_null();
					if (t > 0)
					{
						CTimeSpan time(t);
						str.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(time.GetDays()), time.GetHours(), time.GetMinutes(), time.GetSeconds());
					}
					string = str;
				}
				else
				{
					string = "";
				}
			}
			else if (pItem->dwID == II_SYS_SYS_SCR_PET_FEED_POCKET)
			{

				CItemElem* ptr;
				CItemElem* pItemElem = NULL;

				int nMax = g_pPlayer->m_Inventory.GetMax();
				for (int i = 0; i < nMax; i++)
				{
					ptr = g_pPlayer->m_Inventory.GetAtId(i);
					if (IsUsableItem(ptr) && ptr->m_dwItemId == II_SYS_SYS_SCR_PET_FEED_POCKET &&
						ptr->m_dwKeepTime > 0 && !ptr->IsFlag(CItemElem::expired))
					{
						pItemElem = ptr;
						i = nMax;
					}
				}

				if (pItemElem != NULL)
				{
					CString strTemp;

					time_t t = pItemElem->m_dwKeepTime - time_null();
					//					if( pItemElem->m_dwKeepTime && !pItemElem->IsFlag( CItemElem::expired ) )
					{
						if (t > 0)
						{
							CTimeSpan ct(t);
							string.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
						}
					}
				}
			}
			else
			{

#ifdef __BS_ITEM_UNLIMITEDTIME
				if (pItem->dwSkillTime == 999999999)
				{
					string = prj.GetText(TID_GAME_TOOLTIP_PERMANENTTIME_1);
				}
				else
#endif //__BS_ITEM_UNLIMITEDTIME
				{
					DWORD dwOddTime = ptr1->GetTotal() - (g_tmCurrent - ptr1->GetInst());
					CTimeSpan ct((dwOddTime / 1000));
					string = prj.GetText(TID_TOOLTIP_DATE);
					string.Format(prj.GetText(TID_TOOLTIP_DATE), static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
				}
			}
		}

		p2DRender->TextOut(x + 40, pPoint->y + 18, string, dwColor);

		pPoint->y += m_nFontHeight;// + 3;
		m_dwDraw[m_nMaxDraw] = pItem->dwID;
		++m_nMaxDraw;
	}
}

void CWndCommItemCtrl::OnDraw(C2DRender* p2DRender)
{
	if (NULL == pWndWorld)
	{
		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		return;
	}
	if (NULL == g_pPlayer)
		return;

	CPoint pt(3, 3);
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	int nMaxCount = GetMaxBuff();
	int nMax = nMaxCount;


	if (nMax - m_wndScrollBar.GetScrollPos() > m_wndScrollBar.GetScrollPage())
		nMax = m_wndScrollBar.GetScrollPage() + m_wndScrollBar.GetScrollPos();
	if (nMax < m_wndScrollBar.GetScrollPos())
		nMax = 0;

	for (int i = 0; i < m_wndScrollBar.GetScrollPos() && i < nMaxCount; ++i);

	int x = 0, nWidth = m_rectClient.Width();// - 1;
	CRect rect(x, pt.y, x + nWidth, pt.y + m_nFontHeight);
	rect.SetRect(x + 3, pt.y + 6, x + 3 + 32, pt.y + 6 + 32);

	memset(m_dwDraw, 0, sizeof(int) * (SM_MAX + MAX_SKILLINFLUENCE));
	m_nMaxDraw = 0;

	int nScroll = 0;
	DrawSM(p2DRender, &pt, x, nScroll);
	DrawSkill(p2DRender, &pt, x, nScroll);

}

void CWndCommItemCtrl::SetScrollBar()
{
	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	nRange = g_WndMng.m_RTMessenger.size();
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
}

void CWndCommItemCtrl::OnSize(UINT nType, int cx, int cy)
{
	ItemProp* pItem = NULL;
	CRect rect = GetWindowRect();
	rect.left = rect.right - 15;
	m_wndScrollBar.SetWndRect(rect);

	int nPage, nRange;
	nPage = GetClientRect().Height() / m_nFontHeight;
	int nCount = 0;
	for (int k = 0; k < SM_MAX; ++k)
	{
		if (0 < g_pPlayer->m_dwSMTime[k])
		{
			++nCount;
		}
	}

	nCount += g_pPlayer->m_buffs.GetCount();
	nRange = nCount;
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndCommItemCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndCommItemCtrl::PaintFrame(C2DRender* p2DRender)
{
	int nPage, nRange;
	ItemProp* pItem = NULL;
	if (1)
	{
		nPage = GetClientRect().Height() / (m_nFontHeight);
		int nCount = 0;
		for (int k = 0; k < SM_MAX; ++k)
		{
			if (0 < g_pPlayer->m_dwSMTime[k])
			{
				++nCount;
			}
		}

		nCount += g_pPlayer->m_buffs.GetCount();
		nRange = nCount;
	}
	m_wndScrollBar.SetScrollRange(0, nRange);
	m_wndScrollBar.SetScrollPage(nPage);
}
BOOL CWndCommItemCtrl::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
BOOL CWndCommItemCtrl::SetItem(const LVITEM* pItem)
{
	return TRUE;
}
int CWndCommItemCtrl::InsertItem(const LVITEM* pItem)
{
	return TRUE;
}
