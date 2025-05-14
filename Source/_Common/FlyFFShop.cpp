#include "StdAfx.h"
#include "FlyFFShop.h"

#ifdef __FL_CASH_SHOP
#ifdef __CLIENT
#include "ResData.h"
#include "DPClient.h"
#include "MsgHdr.h"
#include "defineSound.h"
extern	CDPClient	g_DPlay;

// General Parameters
constexpr int nMaxShopTabs = 11;
constexpr int nMinWindowX = 16 * 50;
constexpr int nMinWindowY = 16 * 23;
constexpr int nOneSlotWidth = 116;
constexpr int nOneSlotHeight = 80;
constexpr int nOffsetBetweenSlots = 8;
constexpr int nItemSlotSize = 32;
constexpr float fTabTextureSize = 0.6f;
constexpr float fCurrencyTextureSize = 0.5f;
constexpr float fButtonsTextureSize = 0.55f;

// Categories Parameters
constexpr DWORD dwCategoriesGeneralColor = D3DCOLOR_ARGB(255, 122, 122, 187);
constexpr DWORD dwCategoriesSelectedColor = D3DCOLOR_ARGB(255, 85, 85, 155);
constexpr DWORD dwCategoriesHighlatedColor = D3DCOLOR_ARGB(50, 255, 255, 255);
constexpr DWORD dwCategoriesLineColor = D3DCOLOR_ARGB(255, 0, 0, 0);
constexpr DWORD dwCategoriesTextColor = D3DCOLOR_ARGB(255, 255, 255, 255);

// Item Slots Parameters
constexpr DWORD dwItemsGeneralColor = D3DCOLOR_ARGB(200, 240, 240, 235);
constexpr DWORD dwItemsFrameDefaultColor = D3DCOLOR_ARGB(175, 0, 0, 0);
constexpr DWORD dwItemsLineColor = D3DCOLOR_ARGB(200, 0, 0, 0);
constexpr DWORD dwItemsSlotFrameColor = D3DCOLOR_ARGB(200, 0, 0, 0);
constexpr DWORD dwItemsSlotGeneralColor = D3DCOLOR_ARGB(255, 255, 255, 255);
constexpr DWORD dwItemsNameColor = D3DCOLOR_ARGB(150, 0, 0, 0);
constexpr DWORD dwItemsCurrenciesColor = D3DCOLOR_ARGB(200, 0, 0, 0);
constexpr auto strPurchaseTexture = "flyffshop_btn_purchase.png";
constexpr auto strPreviewTexture = "flyffshop_btn_preview.png";

// Donate/Vote URL
constexpr auto strDonateURL = "https://www.google.com"; // Test URL
constexpr auto strVoteURL = "https://www.google.com"; // Test URL

CString CWndFlyFFShop::GetTabName(int nTab)
{
	switch (nTab)
	{
	case 0: return "All";
	case 1: return "Special Sale";
	case 2: return "Premium";
	case 3: return "Functional";
	case 4: return "Consumables";
	case 5: return "Chanceboxes";
	case 6: return "Furniture";
	case 7: return "Pets";
	case 8: return "Transport";
	case 9: return "Fashion";
	case 10: return "Skins";
	default: return "Empty";
	}

	return "Empty";
}
CString CWndFlyFFShop::GetTabTexture(int nTab)
{
	switch (nTab)
	{
	case 0: return "";
	case 1: return "Itm_PetPaang.dds";
	case 2: return "Itm_SysTicketPaidworld-32.dds";
	case 3: return "Itm_SysSysScrCancel.dds";
	case 4: return "Itm_SysSysEveChocolate06.dds";
	case 5: return "Itm_EventPackbox01-32.dds";
	case 6: return "Itm_HouFurBedBasic02.dds";
	case 7: return "itm_PetDog1.dds";
	case 8: return "Itm_RidRidWinFeather02.dds";
	case 9: return "itm_mCloSnowboarder01Suit.dds";
	case 10: return "Item_WeaSwdColoGrade_3.dds";
	default: return "";
	}

	return "";
}
#endif

CFlyFFShop::CFlyFFShop()
{

}
CFlyFFShop::~CFlyFFShop()
{
	m_vecCShopItems.clear();
}
CFlyFFShop* CFlyFFShop::GetInstance()
{
	static CFlyFFShop sTP;
	return &sTP;
}
void CFlyFFShop::SerializeFlyffShop(CAr& ar)
{
	if (ar.IsStoring())
	{
		ar << m_vecCShopItems.size();
		for (auto& it : m_vecCShopItems)
		{
			ar << it.nItemIndex;
			ar << it.dwItemID;
			ar << it.nPurchaseMethod;
			ar << it.nVotePrice;
			ar << it.nDonatePrice;
			ar << it.nCategory;
			ar << it.nAmount;
		}
	}
	else
	{
		m_vecCShopItems.clear();
		size_t vecSize;

		ar >> vecSize;
		for (size_t i = 0; i < vecSize; i++)
		{
			FLYFF_SHOP_ITEMS tmpStruc;
			ar >> tmpStruc.nItemIndex;
			ar >> tmpStruc.dwItemID;
			ar >> tmpStruc.nPurchaseMethod;
			ar >> tmpStruc.nVotePrice;
			ar >> tmpStruc.nDonatePrice;
			ar >> tmpStruc.nCategory;
			ar >> tmpStruc.nAmount;

			m_vecCShopItems.push_back(tmpStruc);
		}
	}
}

#ifdef __CLIENT
CWndFlyFFShop::CWndFlyFFShop(void)
	: m_nSelectedTab(0)
	, m_nSelectedPage(0)
	, m_nMaxItemsX(1)
	, m_nMaxItemsY(1)
	, m_pIconDonate(NULL)
	, m_pIconVote(NULL)
	, m_pIconPurchase(NULL)
	, m_pIconPreview(NULL)
	, m_bClickCategory(FALSE)
	, m_bClickItem(FALSE)
	, m_pSearchBox(NULL)
	, m_pCheckBoxDonate(NULL)
	, m_pCheckBoxVote(NULL)
	, m_pComboSex(NULL)
	, m_pWndPurchaseItem(NULL)
{

}

CWndFlyFFShop::~CWndFlyFFShop(void)
{
	m_vecFilteredShopItems.clear();
	resetItemRects();
	arOriginalSizes.clear();
	SAFE_DELETE(m_pWndPurchaseItem);
}

BOOL CWndFlyFFShop::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CASH_SHOP, 0, CPoint(0, 0), pWndParent);
}

void CWndFlyFFShop::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	CWndStatic* pWndDonate = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CWndStatic* pWndVote = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pIconDonate = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, pWndDonate->m_strTexture), 0xffff00ff);
	m_pIconVote = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, pWndVote->m_strTexture), 0xffff00ff);

	m_pIconPurchase = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, strPurchaseTexture), 0xffff00ff);
	m_pIconPreview = CWndBase::m_textureMng.AddTexture(D3DDEVICE, MakePath(DIR_THEME, strPreviewTexture), 0xffff00ff);

	m_pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pSearchBox->SetStringBg("Search..");
	m_pCheckBoxDonate = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	m_pCheckBoxVote = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	m_pComboSex = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);

	m_pComboSex->AddWndStyle(EBS_READONLY);
	m_pComboSex->AddString("Any Gender");
	m_pComboSex->AddString("Only Male");
	m_pComboSex->AddString("Only Female");
	m_pComboSex->SetCurSel(0);

	UpdatePoints();
	UpdateContent();
	saveOriginRects();
	MoveParentCenter();
}
void CWndFlyFFShop::RefreshMaxItemsValues()
{
	LPWNDCTRL lpItemsZone = GetWndCtrl(WIDC_CUSTOM2);

	const int nAddX = nOneSlotWidth + nOffsetBetweenSlots;
	const int nAddY = nOneSlotHeight + nOffsetBetweenSlots;

	const int nTmpMaxX = m_nMaxItemsX;
	const int nTmpMaxY = m_nMaxItemsY;

	m_nMaxItemsX = lpItemsZone->rect.Width() / nAddX;
	m_nMaxItemsY = lpItemsZone->rect.Height() / nAddY;

	if (nTmpMaxX != m_nMaxItemsX || nTmpMaxY != m_nMaxItemsY)
		SetPage(PAGE_DEFAULT);
}
void CWndFlyFFShop::OnSize(UINT nType, int cx, int cy)
{
	CPoint cpSizeDiff;
	cpSizeDiff.x = (cx - arOriginalSizes[0].rect.Width());
	cpSizeDiff.y = (cy - arOriginalSizes[0].rect.Height());

	for (auto& it : arOriginalSizes)
	{
		switch (it.type)
		{
		case PARENT:
			break;

		case STATIC:
			GetDlgItem(it.id)->SetWndRect(getItemRect(it.id, cpSizeDiff));
			break;

		case CUSTOM:
			GetWndCtrl(it.id)->rect = getItemRect(it.id, cpSizeDiff);
			break;
		}
	}

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndFlyFFShop::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndPurchaseItem)
		SAFE_DELETE(m_pWndPurchaseItem);
}
void CWndFlyFFShop::OnDraw(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontSomeText4);

	CSize sizeFont = p2DRender->m_pFont->GetTextExtent("1");
	CPoint pointMouse = GetMousePoint();
	CString str;
	ItemProp* pProp;
	CTexture* pIconItem;

	// Render Categories Background
	LPWNDCTRL lpTabs = GetWndCtrl(WIDC_CUSTOM1);
	CRect rectTabs = lpTabs->rect;
	p2DRender->RenderRect(rectTabs, dwCategoriesLineColor);
	rectTabs.DeflateRect(1, 1, 1, 1);
	p2DRender->RenderFillRect(rectTabs, dwCategoriesGeneralColor);

	// Render Categories Tabs
	const int nOneCategoryHeight = rectTabs.Height() / nMaxShopTabs;
	const int nTextureX = rectTabs.left + 6;
	const int nTextX = rectTabs.left + 30;

	CRect rectTab = rectTabs;
	rectTab.bottom = rectTab.top + nOneCategoryHeight;

	int nLineY = rectTab.top + rectTab.Height() - 1;
	int nTextY = rectTab.top + rectTab.Height() / 2 - sizeFont.cy / 2;

	for (int i = 0; i < nMaxShopTabs; i++)
	{
		// Selected and highlighted category
		if (i == m_nSelectedTab)
			p2DRender->RenderFillRect(rectTab, dwCategoriesSelectedColor);
		else
		{
			if (rectTab.PtInRect(pointMouse))
			{
				p2DRender->RenderFillRect(rectTab, dwCategoriesHighlatedColor);

				if (m_bClickCategory)
				{
					PLAYSND(SND_INF_CLICK);
					m_nSelectedTab = i;
					UpdateContent();
				}
			}
		}

		// Category Texture
		str = GetTabTexture(i);
		if (!str.IsEmpty())
		{
			pIconItem = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, str), 0xffff00ff);
			if (pIconItem)
				p2DRender->RenderTexture(CPoint(nTextureX, rectTab.top + rectTab.Height() / 2 - pIconItem->m_size.cy * fTabTextureSize / 2), pIconItem, 255, fTabTextureSize, fTabTextureSize);
		}

		// Category name
		str = GetTabName(i);
		p2DRender->TextOut(nTextX, nTextY, str, dwCategoriesTextColor);

		// Line
		p2DRender->RenderLine(CPoint(rectTab.left, nLineY), CPoint(rectTab.right, nLineY), dwCategoriesLineColor);

		nLineY += nOneCategoryHeight;
		nTextY += nOneCategoryHeight;
		rectTab.top += nOneCategoryHeight;
		rectTab.bottom += nOneCategoryHeight;
	}

	// Render Item Slots
	LPWNDCTRL lpItemsZone = GetWndCtrl(WIDC_CUSTOM2);

	int nPosX = lpItemsZone->rect.left;
	int nPosY = lpItemsZone->rect.top;

	for (int i = 0; i < m_nMaxItemsX * m_nMaxItemsY; i++)
	{
		int nItemBasedByPage = i + (m_nMaxItemsX * m_nMaxItemsY) * m_nSelectedPage;
		if (nItemBasedByPage >= (int)m_vecFilteredShopItems.size())
			break;

		FLYFF_SHOP_ITEMS tmp = m_vecFilteredShopItems[nItemBasedByPage];
		pProp = prj.GetItemProp(tmp.dwItemID);

		// Item zone frame
		CRect rectFrame = CRect(nPosX, nPosY, nPosX + nOneSlotWidth, nPosY + nOneSlotHeight);
		p2DRender->RenderRoundRect(rectFrame, dwItemsFrameDefaultColor);

		// Item zone
		CRect rectZone = rectFrame;
		rectZone.DeflateRect(1, 1, 1, 1);
		p2DRender->RenderFillRect(rectZone, dwItemsGeneralColor);

		// Item name
		str.Format("%s", pProp->szName);
		g_WndMng.StringSize(str, 14);
		sizeFont = p2DRender->m_pFont->GetTextExtent(str);
		CPoint pointItemName = CPoint(rectZone.left + rectZone.Size().cx / 2 - sizeFont.cx / 2, rectZone.top + 2);
		p2DRender->TextOut(pointItemName.x, pointItemName.y, str, dwItemsNameColor);

		// Line
		CPoint pointLine1 = CPoint(rectZone.left, pointItemName.y + sizeFont.cy);
		CPoint pointLine2 = CPoint(rectZone.right, pointItemName.y + sizeFont.cy);
		p2DRender->RenderLine(pointLine1, pointLine2, dwItemsLineColor);

		// Render Slot Background Frame
		CRect rectItemFrame = CRect(rectZone.left + 5, pointLine2.y + 5, 0, 0);
		rectItemFrame.right = rectItemFrame.left + nItemSlotSize;
		rectItemFrame.bottom = rectItemFrame.top + nItemSlotSize;
		p2DRender->RenderRoundRect(rectItemFrame, dwItemsSlotFrameColor);

		// Render Slot Background
		CRect rectItem = rectItemFrame;
		rectItem.DeflateRect(1, 1, 1, 1);
		p2DRender->RenderRect(rectItem, dwItemsSlotGeneralColor);

		// Render item tooltip
		if (rectItemFrame.PtInRect(pointMouse))
		{
			if (m_pWndOnMouseMove == this && rectItemFrame.PtInRect(pointMouse))
			{
				CItemElem itemElem;
				itemElem.m_dwItemId = pProp->dwID;
				itemElem.m_nItemNum = tmp.nAmount;

				CPoint point2 = pointMouse;
				ClientToScreen(&point2);
				ClientToScreen(&rectItemFrame);
				g_WndMng.PutToolTip_Item(&itemElem, point2, &rectItemFrame, GetWndId());
				ScreenToClient(&rectItemFrame);
			}
		}

		// Render Item Icon
		pIconItem = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		if (pIconItem)
			p2DRender->RenderTexture(rectItem.TopLeft(), pIconItem, 255, nItemSlotSize / 32.0f, nItemSlotSize / 32.0f);

		// Render Item Amount
		CD3DFont* pOldFont2 = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
		str.Format("x%d", tmp.nAmount);
		sizeFont = p2DRender->m_pFont->GetTextExtent(str);
		p2DRender->TextOut(rectItemFrame.right - sizeFont.cx - 2, rectItemFrame.bottom - sizeFont.cy + 3, str, 0xffeaebdd);
		p2DRender->SetFont(pOldFont2);

		// Render Donate Icon/Price
		CPoint pointTexture = CPoint(rectItem.right + 5, rectItem.top);
		p2DRender->RenderTexture(pointTexture, m_pIconDonate, 255, fCurrencyTextureSize, fCurrencyTextureSize);
		CPoint pointPrice = CPoint(pointTexture.x + m_pIconDonate->m_size.cx * fCurrencyTextureSize + 5, pointTexture.y);
		p2DRender->TextOut(pointPrice.x, pointPrice.y, tmp.nPurchaseMethod == CS_METHOD_DP || tmp.nPurchaseMethod == CS_METHOD_BOTH ? Separator(tmp.nDonatePrice) : "-", dwItemsCurrenciesColor);

		// Render Vote Icon/Price
		pointTexture = CPoint(pointTexture.x, pointTexture.y + m_pIconDonate->m_size.cy * fCurrencyTextureSize);
		p2DRender->RenderTexture(pointTexture, m_pIconVote, 255, fCurrencyTextureSize, fCurrencyTextureSize);
		pointPrice = CPoint(pointPrice.x, pointTexture.y);
		p2DRender->TextOut(pointPrice.x, pointPrice.y, tmp.nPurchaseMethod == CS_METHOD_VP || tmp.nPurchaseMethod == CS_METHOD_BOTH ? Separator(tmp.nVotePrice) : "-", dwItemsCurrenciesColor);

		// Line
		CPoint pointLine3 = CPoint(rectZone.left, rectItemFrame.bottom + 5);
		CPoint pointLine4 = CPoint(rectZone.right, rectItemFrame.bottom + 5);
		p2DRender->RenderLine(pointLine3, pointLine4, dwItemsLineColor);

		// Buttons zone
		CRect rectButton = CRect(rectZone.left, pointLine3.y + 1, rectZone.right, rectZone.bottom);
		p2DRender->RenderFillRect(rectButton,
			D3DCOLOR_ARGB(200, 249, 230, 90), D3DCOLOR_ARGB(200, 249, 230, 90),
			D3DCOLOR_ARGB(200, 253, 211, 24), D3DCOLOR_ARGB(200, 253, 211, 24));

		int nOneButtonWidth = rectButton.Size().cx / 2;

		// Render purchase button		
		CRect rectButtonPurchase = rectButton;
		rectButtonPurchase.right = rectButtonPurchase.left + nOneButtonWidth;

		if (rectButtonPurchase.PtInRect(pointMouse))
		{
			p2DRender->RenderFillRect(rectButtonPurchase, D3DCOLOR_ARGB(80, 255, 255, 255));

			if (m_pWndOnMouseMove == this)
			{
				CPoint point2 = pointMouse;
				ClientToScreen(&point2);
				ClientToScreen(&rectButtonPurchase);
				g_toolTip.PutToolTip(i, "Purchase/Gift an item", rectButtonPurchase, point2);
				ScreenToClient(&rectButtonPurchase);
			}

			if (m_bClickItem)
			{
				PLAYSND(SND_INF_CLICK);
				SAFE_DELETE(m_pWndPurchaseItem);
				m_pWndPurchaseItem = new CWndFlyFFShopPurchase(tmp);
				m_pWndPurchaseItem->Initialize(this);
			}
		}

		pointTexture = CPoint(rectButtonPurchase.right - rectButtonPurchase.Size().cx / 2 - m_pIconPurchase->m_size.cx * fButtonsTextureSize / 2, rectButtonPurchase.bottom - rectButtonPurchase.Size().cy / 2 - m_pIconPurchase->m_size.cy * fButtonsTextureSize / 2);
		p2DRender->RenderTexture(pointTexture, m_pIconPurchase, 255, fButtonsTextureSize, fButtonsTextureSize);

		// Render vertical line
		p2DRender->RenderLine(CPoint(rectButtonPurchase.right, rectButton.top), CPoint(rectButtonPurchase.right, rectButton.bottom), dwItemsLineColor);

		// Render preview button
		CRect rectButtonPreview = rectButton;
		rectButtonPreview.left = rectButtonPurchase.right;

		if (rectButtonPreview.PtInRect(pointMouse))
		{
			p2DRender->RenderFillRect(rectButtonPreview, D3DCOLOR_ARGB(80, 255, 255, 255));

			if (m_pWndOnMouseMove == this)
			{
				CPoint point2 = pointMouse;
				ClientToScreen(&point2);
				ClientToScreen(&rectButtonPreview);
				g_toolTip.PutToolTip(i, "Preview item", rectButtonPreview, point2);
				ScreenToClient(&rectButtonPreview);
			}

#ifdef __MODEL_VIEW
			if (m_bClickItem)
			{
				PLAYSND(SND_INF_CLICK);
				CItemElem itemElem;
				itemElem.m_dwItemId = pProp->dwID;
				itemElem.m_nItemNum = tmp.nAmount;
				if (!g_WndMng.PreviewModel(&itemElem))
					g_WndMng.PutString("You can't preview this item.");
			}
#endif
		}

		pointTexture = CPoint(rectButtonPreview.right - rectButtonPreview.Size().cx / 2 - m_pIconPreview->m_size.cx * fButtonsTextureSize / 2, rectButtonPreview.bottom - rectButtonPreview.Size().cy / 2 - m_pIconPreview->m_size.cy * fButtonsTextureSize / 2);
		p2DRender->RenderTexture(pointTexture, m_pIconPreview, 255, fButtonsTextureSize, fButtonsTextureSize);

		// Find position for next item
		if ((i + 1) % m_nMaxItemsX)
			nPosX += nOneSlotWidth + nOffsetBetweenSlots;
		else
		{
			nPosY += nOneSlotHeight + nOffsetBetweenSlots;
			nPosX = lpItemsZone->rect.left;
		}
	}

	p2DRender->SetFont(pOldFont);

	m_bClickCategory = FALSE;
	m_bClickItem = FALSE;

	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndFlyFFShop::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndFlyFFShop::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1:
			m_nSelectedTab = 0;
			m_pSearchBox->SetString("");
			m_pCheckBoxDonate->SetCheck(FALSE);
			m_pCheckBoxVote->SetCheck(FALSE);
			m_pComboSex->SetCurSel(0);
			UpdateContent();
			break;
		case WIDC_BUTTON2:
			ShellExecute(0, 0, strDonateURL, 0, 0, SW_SHOW);
			break;
		case WIDC_BUTTON3:
			ShellExecute(0, 0, strVoteURL, 0, 0, SW_SHOW);
			break;
		case WIDC_BUTTON4:
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
			pButton->EnableWindow(FALSE);

			g_DPlay.SendHdr(PACKETTYPE_CSHOP_BALANCE);
			break;
		}
		case WIDC_BUTTON_1:
			KillFocus(m_pSearchBox, NULL);
			m_pSearchBox->Empty();
			break;
		case WIDC_BTN_START:
			SetPage(PAGE_DEFAULT);
			break;
		case WIDC_BUTTON_MINUS:
			SetPage(PAGE_PREV);
			break;
		case WIDC_BUTTON_PLUS:
			SetPage(PAGE_NEXT);
			break;
		case WIDC_BTN_END:
			SetPage(PAGE_MAX);
			break;
		case WIDC_CHECK1:
		case WIDC_CHECK2:
			UpdateContent();
			break;
		default:
			break;
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1)
			UpdateContent();

	}
	else if (message == EN_CHANGE)
	{
		if (nID == WIDC_EDIT1)
			UpdateContent();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndFlyFFShop::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPWNDCTRL lpTabs = GetWndCtrl(WIDC_CUSTOM1);
	if (lpTabs->rect.PtInRect(point))
		m_bClickCategory = TRUE;

	CWndStatic* pWndBackground = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	if (pWndBackground->GetWndRect().PtInRect(point))
		m_bClickItem = TRUE;
}
void CWndFlyFFShop::UpdatePoints()
{
	CWndStatic* pWndPoints = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	pWndPoints->m_strTitle = Separator(g_Neuz.m_nDonatePoints);
	pWndPoints = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
	pWndPoints->m_strTitle = Separator(g_Neuz.m_nVotePoints);

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	pButton->EnableWindow(TRUE);
}
void CWndFlyFFShop::UpdateContent()
{
	m_vecFilteredShopItems.clear();

	for (auto& it : *CFlyFFShop::GetInstance()->GetItemVector())
	{
		if (it.nCategory != m_nSelectedTab && m_nSelectedTab != 0)
			continue;

		ItemProp* pItemProp = prj.GetItemProp(it.dwItemID);
		if (!pItemProp)
			continue;

		// Search bar
		CString strFilter = m_pSearchBox->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = pItemProp->szName;
			if (!strName.IsEmpty())
			{
				if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
					continue;
			}
		}

		// Available for donate points
		if (m_pCheckBoxDonate->GetCheck())
		{
			if (it.nPurchaseMethod != CS_METHOD_DP && it.nPurchaseMethod != CS_METHOD_BOTH)
				continue;
		}

		// Available for vote points
		if (m_pCheckBoxVote->GetCheck())
		{
			if (it.nPurchaseMethod != CS_METHOD_VP && it.nPurchaseMethod != CS_METHOD_BOTH)
				continue;
		}

		// Gender Filter
		if (m_pComboSex->GetCurSel() == 1)
		{
			if (pItemProp->dwItemSex != SEX_MALE && !strstr(pItemProp->szName, "(M)"))
				continue;
		}
		else if (m_pComboSex->GetCurSel() == 2)
		{
			if (pItemProp->dwItemSex != SEX_FEMALE && !strstr(pItemProp->szName, "(F)"))
				continue;
		}

		m_vecFilteredShopItems.insert(m_vecFilteredShopItems.begin(), it);
	}

	SetPage(PAGE_DEFAULT);
}
void CWndFlyFFShop::SetPage(int nMode)
{
	int nTotalItems = m_vecFilteredShopItems.size();
	int nMaxPage = -1;
	while (nTotalItems > 0)
	{
		nMaxPage++;
		nTotalItems -= m_nMaxItemsX * m_nMaxItemsY;
	}

	if (nMode == PAGE_DEFAULT)
	{
		m_nSelectedPage = 0;
	}
	else if (nMode == PAGE_PREV)
	{
		m_nSelectedPage--;
		if (m_nSelectedPage < 0)
			m_nSelectedPage = nMaxPage;
	}
	else if (nMode == PAGE_NEXT)
	{
		m_nSelectedPage++;
		if (m_nSelectedPage > nMaxPage)
			m_nSelectedPage = 0;
	}
	else if (nMode == PAGE_MAX)
	{
		m_nSelectedPage = nMaxPage;
	}

	UpdatePageStatic(nMaxPage);
}
void CWndFlyFFShop::UpdatePageStatic(int nMaxPage)
{
	CWndButton* pMinus = (CWndButton*)GetDlgItem(WIDC_BUTTON_MINUS);
	CWndButton* pPlus = (CWndButton*)GetDlgItem(WIDC_BUTTON_PLUS);
	CWndButton* pFirst = (CWndButton*)GetDlgItem(WIDC_BTN_START);
	CWndButton* pLast = (CWndButton*)GetDlgItem(WIDC_BTN_END);

	if (nMaxPage <= 0)
	{
		pMinus->EnableWindow(FALSE);
		pPlus->EnableWindow(FALSE);
		pFirst->EnableWindow(FALSE);
		pLast->EnableWindow(FALSE);
	}
	else
	{
		pMinus->EnableWindow(TRUE);
		pPlus->EnableWindow(TRUE);
		pFirst->EnableWindow(TRUE);
		pLast->EnableWindow(TRUE);
	}

	if (nMaxPage == -1)
		nMaxPage = 0;

	CString str;
	str.Format("%d/%d", m_nSelectedPage, nMaxPage);
	CWndStatic* pWndPage = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
	pWndPage->SetTitle(str);

}
void CWndFlyFFShop::saveOriginRects()
{
	if (arOriginalSizes.size() > 0)
		return;

	arOriginalSizes.push_back(ctrlhelp(0, PARENT, GetClientRect()));

	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM1, CUSTOM, GetWndCtrl(WIDC_CUSTOM1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM2, CUSTOM, GetWndCtrl(WIDC_CUSTOM2)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC6, STATIC, GetDlgItem(WIDC_STATIC6)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_EDIT1, STATIC, GetWndCtrl(WIDC_EDIT1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON_1, STATIC, GetWndCtrl(WIDC_BUTTON_1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BTN_START, STATIC, GetWndCtrl(WIDC_BTN_START)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON_MINUS, STATIC, GetWndCtrl(WIDC_BUTTON_MINUS)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC5, STATIC, GetDlgItem(WIDC_STATIC5)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON_PLUS, STATIC, GetWndCtrl(WIDC_BUTTON_PLUS)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BTN_END, STATIC, GetWndCtrl(WIDC_BTN_END)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC1, STATIC, GetDlgItem(WIDC_STATIC1)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC2, STATIC, GetDlgItem(WIDC_STATIC2)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC3, STATIC, GetDlgItem(WIDC_STATIC3)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC4, STATIC, GetDlgItem(WIDC_STATIC4)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CHECK1, STATIC, GetWndCtrl(WIDC_CHECK1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CHECK2, STATIC, GetWndCtrl(WIDC_CHECK2)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC7, STATIC, GetDlgItem(WIDC_STATIC7)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_COMBOBOX1, STATIC, GetWndCtrl(WIDC_COMBOBOX1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON1, STATIC, GetWndCtrl(WIDC_BUTTON1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON2, STATIC, GetWndCtrl(WIDC_BUTTON2)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON3, STATIC, GetWndCtrl(WIDC_BUTTON3)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON4, STATIC, GetWndCtrl(WIDC_BUTTON4)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC8, STATIC, GetDlgItem(WIDC_STATIC8)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC9, STATIC, GetDlgItem(WIDC_STATIC9)->GetWndRect()));
}
void CWndFlyFFShop::resetItemRects()
{
	for (auto& it : arOriginalSizes)
	{
		switch (it.type)
		{
		case STATIC:
			GetDlgItem(it.id)->SetWndRect(it.rect);
			break;

		case CUSTOM:
			GetWndCtrl(it.id)->rect = it.rect;
			break;
		}
	}
}

CRect CWndFlyFFShop::getItemRect(int id, CPoint sizediff, ctrltype type)
{
	for (auto& it : arOriginalSizes)
	{
		if ((type == PARENT || it.type == type) && it.id == id)
		{
			switch (id)
			{
			case WIDC_CUSTOM1:
				return CRect(it.rect.left, it.rect.top, it.rect.right, it.rect.bottom + sizediff.y);

			case WIDC_BUTTON_1:
				return it.rect;

			case WIDC_EDIT1:
				return CRect(it.rect.left, it.rect.top, it.rect.right + sizediff.x, it.rect.bottom);

			case WIDC_STATIC1:
			case WIDC_STATIC2:
			case WIDC_STATIC3:
			case WIDC_STATIC4:
			case WIDC_CHECK1:
			case WIDC_CHECK2:
			case WIDC_STATIC7:
			case WIDC_COMBOBOX1:
			case WIDC_BUTTON1:
			case WIDC_BUTTON2:
			case WIDC_BUTTON3:
			case WIDC_BUTTON4:
			case WIDC_STATIC8:
				return CRect(it.rect.left + sizediff.x, it.rect.top, it.rect.right + sizediff.x, it.rect.bottom);

			case WIDC_BTN_START:
			case WIDC_BUTTON_MINUS:
			case WIDC_STATIC5:
			case WIDC_BUTTON_PLUS:
			case WIDC_BTN_END:
				return CRect(it.rect.left + (sizediff.x / 2), it.rect.top + sizediff.y, it.rect.right + (sizediff.x / 2), it.rect.bottom + sizediff.y);

			case WIDC_CUSTOM2:
			case WIDC_STATIC6:
				return CRect(it.rect.left, it.rect.top, it.rect.right + sizediff.x, it.rect.bottom + sizediff.y);

			case WIDC_STATIC9:
				return CRect(it.rect.left + sizediff.x, it.rect.top + sizediff.y, it.rect.right + sizediff.x, it.rect.bottom + sizediff.y);
			}
		}
	}

	return CRect(0, 0, 0, 0);
}

void CWndFlyFFShop::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, nMinWindowX, nMinWindowY);

	if (bOnSize)
		RefreshMaxItemsValues();

	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}

/////////////////////////////////
//////// Purchase window ////////
/////////////////////////////////

CWndFlyFFShopPurchase::CWndFlyFFShopPurchase(FLYFF_SHOP_ITEMS itm)
	: m_pComboFriends(NULL)
	, m_pEditNum(NULL)
	, m_pTexture(NULL)
{
	item = itm;

	for (int i = 0; i < 2; i++)
		m_pButtonsCurrency[i] = NULL;
}

CWndFlyFFShopPurchase::~CWndFlyFFShopPurchase()
{
	m_vecFriends.clear();
}

BOOL CWndFlyFFShopPurchase::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CASH_SHOP_PURCHASE, 0, CPoint(0, 0), pWndParent);
}

void CWndFlyFFShopPurchase::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, prj.GetItemProp(item.dwItemID)->szIcon), 0xffff00ff);

	// Add friendlist
	m_pComboFriends = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pComboFriends->AddWndStyle(EBS_READONLY);
	m_vecFriends.push_back(g_pPlayer->m_idPlayer);
	m_pComboFriends->AddString(g_pPlayer->GetName());
	for (auto& it : g_WndMng.m_RTMessenger)
	{
		m_vecFriends.push_back(it.first);
		m_pComboFriends->AddString(CPlayerDataCenter::GetInstance()->GetPlayerString(it.first));
	}
	m_pComboFriends->SetCurSel(0);

	// Item count edit zone
	m_pEditNum = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEditNum->AddWndStyle(EBS_NUMBER);
	m_pEditNum->SetString("1");

	// Currency zone
	m_pButtonsCurrency[0] = (CWndButton*)GetDlgItem(WIDC_RADIO);
	m_pButtonsCurrency[1] = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	m_pButtonsCurrency[0]->SetGroup(TRUE);

	if (item.nPurchaseMethod == CS_METHOD_VP)
	{
		m_pButtonsCurrency[0]->EnableWindow(FALSE);
		m_pButtonsCurrency[0]->SetCheck(FALSE);
		m_pButtonsCurrency[1]->SetCheck(TRUE);
	}
	else if (item.nPurchaseMethod == CS_METHOD_DP)
	{
		m_pButtonsCurrency[1]->EnableWindow(FALSE);
		m_pButtonsCurrency[1]->SetCheck(FALSE);
		m_pButtonsCurrency[0]->SetCheck(TRUE);
	}
	else
	{
		m_pButtonsCurrency[0]->SetCheck(TRUE);
		m_pButtonsCurrency[1]->SetCheck(FALSE);
	}

	MoveParentCenter();
}
void CWndFlyFFShopPurchase::OnDraw(C2DRender* p2DRender)
{
	CWndFlyFFShop* pWndMain = (CWndFlyFFShop*)g_WndMng.GetWndBase(APP_CASH_SHOP);
	if (!pWndMain)
		return;

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontSomeText4);

	// Render item icon
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT1);
	if (m_pTexture)
		m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));

	CD3DFont* pOldFont2 = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
	CString str;
	str.Format("x%d", item.nAmount);
	CSize sizeFont = p2DRender->m_pFont->GetTextExtent(str);
	p2DRender->TextOut(wndCtrl->rect.right - sizeFont.cx - 2, wndCtrl->rect.bottom - sizeFont.cy + 3, str, 0xffeaebdd);
	p2DRender->SetFont(pOldFont2);

	int nItems = atoi(m_pEditNum->GetString());

	// Render Donate Icon/Price
	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_RADIO);
	CRect rect = pButton->GetWndRect();

	CPoint pointTexture = CPoint(rect.left + 20, rect.bottom - rect.Size().cy / 2 - pWndMain->m_pIconDonate->m_size.cy * fCurrencyTextureSize / 2);
	p2DRender->RenderTexture(pointTexture, pWndMain->m_pIconDonate, 255, fCurrencyTextureSize, fCurrencyTextureSize);
	CPoint pointPrice = CPoint(pointTexture.x + pWndMain->m_pIconDonate->m_size.cx * fCurrencyTextureSize + 5, pointTexture.y);
	p2DRender->TextOut(pointPrice.x, pointPrice.y, item.nPurchaseMethod == CS_METHOD_DP || item.nPurchaseMethod == CS_METHOD_BOTH ? Separator(item.nDonatePrice * nItems) : "-", dwItemsCurrenciesColor);

	// Render Vote Icon/Price
	pButton = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	rect = pButton->GetWndRect();

	pointTexture = CPoint(rect.left + 20, rect.bottom - rect.Size().cy / 2 - pWndMain->m_pIconVote->m_size.cy * fCurrencyTextureSize / 2);
	p2DRender->RenderTexture(pointTexture, pWndMain->m_pIconVote, 255, fCurrencyTextureSize, fCurrencyTextureSize);
	pointPrice = CPoint(pointTexture.x + pWndMain->m_pIconVote->m_size.cx * fCurrencyTextureSize + 5, pointTexture.y);
	p2DRender->TextOut(pointPrice.x, pointPrice.y, item.nPurchaseMethod == CS_METHOD_VP || item.nPurchaseMethod == CS_METHOD_BOTH ? Separator(item.nVotePrice * nItems) : "-", dwItemsCurrenciesColor);


	p2DRender->SetFont(pOldFont);

	CWndNeuz::OnDraw(p2DRender);
}
void CWndFlyFFShopPurchase::OnMouseWndSurface(CPoint point)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT1);
	CRect rectItem = wndCtrl->rect;

	if (rectItem.PtInRect(point))
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = item.dwItemID;
		itemElem.m_nItemNum = item.nAmount;

		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&rectItem);
		g_WndMng.PutToolTip_Item(&itemElem, point2, &rectItem, GetWndId());
	}
}
BOOL CWndFlyFFShopPurchase::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndFlyFFShopPurchase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1:
		case WIDC_BUTTON2:
		{
			int nNumber = atoi(m_pEditNum->GetString());

			if (nID == WIDC_BUTTON1)
				nNumber -= 1;
			else
				nNumber += 1;

			CString str;
			str.Format("%d", nNumber);
			m_pEditNum->SetString(str);
		}
		break;
		case WIDC_BUTTON3:
		{
			BOOL bNotEnough = FALSE;
			if (m_pButtonsCurrency[0]->GetCheck())
			{
				if (g_Neuz.m_nDonatePoints < item.nDonatePrice * atoi(m_pEditNum->GetString()))
					bNotEnough = TRUE;
			}
			else
			{
				if (g_Neuz.m_nVotePoints < item.nVotePrice * atoi(m_pEditNum->GetString()))
					bNotEnough = TRUE;
			}

			if (bNotEnough)
				g_WndMng.PutString("Not enough money to purchase this item.", NULL, 0xffff0000);
			else
			{
				g_DPlay.SendBuyCShopItem(item.nItemIndex, atoi(m_pEditNum->GetString()), m_pButtonsCurrency[0]->GetCheck(), m_vecFriends[m_pComboFriends->GetCurSel()]);
				Destroy();
			}
		}
		break;
		default: break;
		}
	}

	int nNumber = atoi(m_pEditNum->GetString());
	if (nNumber < 1)
		m_pEditNum->SetString("1");
	else if (nNumber > 99)
		m_pEditNum->SetString("99");

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif
#endif
