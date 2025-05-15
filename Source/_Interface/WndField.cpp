// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineObj.h"
#include "DefineItem.h"
#include "defineText.h"
#include "defineskill.h"
#include "defineSound.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "Party.h"
#include "WndManager.h"
#include <afxdisp.h>
#include "defineNeuz.h"

#include "collecting.h"

#ifdef __WIKI_ITEMS_V2
#include "WndWikiItems.h"
#endif

#include "DPClient.h"
#include "MsgHdr.h"
extern	CDPClient	g_DPlay;

#ifdef __FL_PARTY_FINDER
#include "party.h"
#endif

#include "wndvendor.h"
#include "wndwebbox.h"
#if __VER >= 11 // __SYS_POCKET
#include "WndBagEx.h"
#endif

#if __VER >= 11 // __SYS_PLAYER_DATA
#include "playerdata.h"
#endif	// __SYS_PLAYER_DATA

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#endif

#ifdef __FL_BATTLE_PASS
#include "BattlePass.h"
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
#include "WheelOfFortune.h"
#endif

#ifdef __FL_RECORD_BOOK
#include "RecordBook.h"
#endif

extern int g_nSkillCurSelect;
extern float g_fHairLight;

#include "eveschool.h"
extern CGuildCombat g_GuildCombatMng;

#include "dpcertified.h"
extern	CDPCertified	g_dpCertified;


#define WND_WIDTH 210
#define TASKBAR_HEIGHT 44

extern DWORD FULLSCREEN_WIDTH;
extern DWORD FULLSCREEN_HEIGHT;


#define MAX_GUILDCOMBAT_LIST_PER_PAGE 11
#define MAX_MAIL_LIST_PER_PAGE 6
#define MAX_GUILDCOMBAT_LIST		  100

CWndDropItem::CWndDropItem()
{
	m_pItemElem = NULL;
	m_pEdit = NULL;
}
CWndDropItem::~CWndDropItem()
{
}
void CWndDropItem::OnDraw(C2DRender* p2DRender)
{
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi(szNumber);

	if (m_pItemElem->m_nItemNum == 1)
	{
		m_pEdit->SetString("1");
	}
	else
	{
		if (m_pItemElem->m_nItemNum < nNumber)
		{
			char szNumberbuf[16] = { 0, };
			nNumber = m_pItemElem->m_nItemNum;
			_itoa(m_pItemElem->m_nItemNum, szNumberbuf, 10);
			m_pEdit->SetString(szNumberbuf);
		}
		for (int i = 0; i < 8; i++)
		{
			char szNumberbuf[8] = { 0, };
			strncpy(szNumberbuf, szNumber, 8);


			if (47 >= szNumberbuf[i] || szNumberbuf[i] >= 58)
			{
				if (szNumberbuf[i] != 0)
				{
					nNumber = m_pItemElem->m_nItemNum;
					_itoa(m_pItemElem->m_nItemNum, szNumberbuf, 10);
					m_pEdit->SetString(szNumberbuf);
					break;
				}
			}
		}
	}
}
void CWndDropItem::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndOk->SetDefault(TRUE);
	m_pEdit->SetFocus();

	if (m_pItemElem->m_nItemNum == 1)
	{
		m_pEdit->SetString("1");
		//		m_pEdit->SetVisible( FALSE );
	}
	else
	{
		TCHAR szNumber[64];
		_itot(m_pItemElem->m_nItemNum, szNumber, 10);
		m_pEdit->SetString(szNumber);
	}


	MoveParentCenter();
}

BOOL CWndDropItem::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DROP_ITEM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndDropItem::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndDropItem::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndDropItem::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndDropItem::OnLButtonDown(UINT nFlags, CPoint point)
{
}

#if __VER >= 14 // __DROP_CONFIRM_BUG
BOOL CWndDropItem::Process(void)
{
	if (m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
#endif // __DROP_CONFIRM_BUG

BOOL CWndDropItem::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK || message == EN_RETURN)
	{
		short DropNum = 0;

		if (m_pItemElem->m_nItemNum >= 1)
		{
			DropNum = atoi(m_pEdit->GetString());
		}
		if (DropNum > 0)
		{
			g_DPlay.SendDropItem(0, m_pItemElem->m_dwObjId, DropNum, m_vPos);
		}

		Destroy();
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
CWndDropConfirm::CWndDropConfirm()
{
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}
CWndDropConfirm::~CWndDropConfirm()
{
}
void CWndDropConfirm::OnDraw(C2DRender* p2DRender)
{

}
void CWndDropConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_YES);
	pWndOk->SetDefault(TRUE);


	MoveParentCenter();
}

BOOL CWndDropConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DROP_CONFIRM, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndDropConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndDropConfirm::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndDropConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndDropConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

#if __VER >= 14 // __DROP_CONFIRM_BUG
BOOL CWndDropConfirm::Process(void)
{
	if (m_pItemElem->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
#endif // __DROP_CONFIRM_BUG

BOOL CWndDropConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES || message == EN_RETURN)
	{
		g_DPlay.SendDropItem(0, m_pItemElem->m_dwObjId, 1, m_vPos);
		Destroy();
	}
	else if (nID == WIDC_NO)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndRandomScrollConfirm::CWndRandomScrollConfirm()
{
	bFlag = FALSE;
	objid = objid1 = NULL_ID;
}
CWndRandomScrollConfirm::~CWndRandomScrollConfirm()
{
}
void CWndRandomScrollConfirm::SetItem(OBJID objidBuf, OBJID objid1Buf, BOOL bFlagBuf)
{
	objid = objidBuf;
	objid1 = objid1Buf;
	bFlag = bFlagBuf;

}
void CWndRandomScrollConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndRandomScrollConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CItemElem* pItemElem0 = g_pPlayer->m_Inventory.GetAtId(objid);
	CItemElem* pItemElem1 = g_pPlayer->m_Inventory.GetAtId(objid1);

	if (pItemElem0 && pItemElem1)
	{
		CString strMessage;
		if (bFlag)
			strMessage.Format(prj.GetText(TID_GAME_RANDOMSCROLL_CONFIRM1), pItemElem0->GetProp()->szName);
		else
			strMessage.Format(prj.GetText(TID_GAME_RANDOMSCROLL_CONFIRM), pItemElem0->GetProp()->szName);

		CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
		pWndText->SetString(strMessage);
		pWndText->EnableWindow(FALSE);
	}
	else
	{
		Destroy();
	}


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndRandomScrollConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_RANDOMSCROLL_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndRandomScrollConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRandomScrollConfirm::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRandomScrollConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRandomScrollConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndRandomScrollConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		g_DPlay.SendRandomScroll(objid, objid1);
	}
	Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndQuestItemWarning::CWndQuestItemWarning()
{
	m_pItemElem = NULL;
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}
CWndQuestItemWarning::~CWndQuestItemWarning()
{
}
void CWndQuestItemWarning::OnDraw(C2DRender* p2DRender)
{
}
void CWndQuestItemWarning::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_CONTEXT);

	if (pWndEdit)
	{
		pWndEdit->SetString(_T(prj.GetText(TID_GAME_QUEITMWARNING)));
		pWndEdit->EnableWindow(FALSE);
	}


	MoveParentCenter();
}

BOOL CWndQuestItemWarning::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndQuestItemWarning::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndQuestItemWarning::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndQuestItemWarning::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndQuestItemWarning::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndQuestItemWarning::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BTN_YES || message == EN_RETURN)
	{
		if (m_pItemElem->m_nItemNum == 1)
		{
			SAFE_DELETE(g_WndMng.m_pWndDropConfirm);
			g_WndMng.m_pWndDropConfirm = new CWndDropConfirm;
			g_WndMng.m_pWndDropConfirm->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropConfirm->m_vPos = m_vPos;
			g_WndMng.m_pWndDropConfirm->Initialize(NULL, APP_DROP_CONFIRM);
		}
		else
		{
			SAFE_DELETE(g_WndMng.m_pWndDropItem);
			g_WndMng.m_pWndDropItem = new CWndDropItem;
			g_WndMng.m_pWndDropItem->m_pItemElem = m_pItemElem;
			g_WndMng.m_pWndDropItem->m_vPos = m_vPos;
			g_WndMng.m_pWndDropItem->Initialize(NULL, APP_DROP_ITEM);
		}
		Destroy();
	}
	else if (nID == WIDC_BTN_NO)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


#include "WndShop.h"

CWndGold::CWndGold()
{
}
CWndGold::~CWndGold()
{
}
void CWndGold::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_pTexture = &m_texture; //.m_pFromWnd   = this;
	m_GlobalShortcut.m_dwData = 0;
}
void CWndGold::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndGold::OnInitialUpdate()
{
	m_texture.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, "itm_GolGolSeed.dds"), 0xffff00ff);
}

CWndQueryEquip::CWndQueryEquip()
{
	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);

	m_OldPos = CPoint(0, 0);
}
CWndQueryEquip::~CWndQueryEquip()
{

}

BOOL CWndQueryEquip::Process()
{
	if (m_Mover.m_pModel)
		m_Mover.m_pModel->FrameMove();

	return TRUE;
}
void CWndQueryEquip::OnMouseWndSurface(CPoint point)
{
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		CRect DrawRect = m_InvenRect[i];

		CPoint point = GetMousePoint();

		if (DrawRect.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);

			CItemElem itemElem;
			itemElem = m_aEquipInfoAdd[i];

			g_WndMng.PutToolTip_Item(&itemElem, point2, &DrawRect, APP_QUERYEQUIP);
		}
	}
}
void CWndQueryEquip::OnDraw(C2DRender* p2DRender)
{
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		FLOAT sx = 1.0f, sy = 1.0f;

		DWORD dwAlpha = 255;

		if (m_aEquipInfoAdd[i].m_pTexture == NULL)
			continue;

		CRect DrawRect = m_InvenRect[i];
		CPoint cpAdd = CPoint(6, 6);

		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2)
		{
			cpAdd = CPoint(0, 0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
		{
			cpAdd = CPoint(0, 0);

			sx = 0.9f;
			sy = 0.9f;
		}

		if (m_aEquipInfoAdd[i].IsFlag(CItemElem::expired))
			p2DRender->RenderTexture2(DrawRect.TopLeft() + cpAdd, m_aEquipInfoAdd[i].m_pTexture, sx, sy, D3DCOLOR_XRGB(255, 100, 100));
		else
			p2DRender->RenderTexture(DrawRect.TopLeft() + cpAdd, m_aEquipInfoAdd[i].m_pTexture, dwAlpha, sx, sy);
	}

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM20);
	VIEWPORT tmp;
	tmp.m_fRot = m_fRot;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)m_Mover.m_pModel, &m_Mover, lpFace, &tmp, GetWndId());
}
void CWndQueryEquip::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
		m_fRot += (m_OldPos.x - point.x) * 0.5f;

	m_OldPos = point;
}
void CWndQueryEquip::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();

}
void CWndQueryEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM20);
	if (lpWndCtrl->rect.PtInRect(point))
	{
		m_OldPos = point;
		m_bLButtonDownRot = TRUE;
	}
}

void CWndQueryEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM6);
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM7);
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM8);
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM9);
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl(WIDC_CUSTOM10);

	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM11);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM12);
	lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM13);
	lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM14);
	lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM15);
	lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM16);
	lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM17);
	lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM18);
	lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM19);

	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	MoveParentCenter();
}

void CWndQueryEquip::SetModel(CMover* pMover)
{
	if (!pMover)
		return;

	SAFE_DELETE(m_Mover.m_pModel);
	int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_Mover.m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_Mover.m_pModel, OT_MOVER, nMover, MTI_STAND);
	m_Mover.m_pModel->InitDeviceObjects(g_Neuz.GetDevice());

	memcpy(m_Mover.m_aEquipInfo, pMover->m_aEquipInfo, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS);
	m_Mover.m_fHairColorR = pMover->m_fHairColorR;
	m_Mover.m_fHairColorG = pMover->m_fHairColorG;
	m_Mover.m_fHairColorB = pMover->m_fHairColorB;
	CMover::UpdateParts(pMover->GetSex(), pMover->m_dwHairMesh, pMover->m_dwHeadMesh, m_Mover.m_aEquipInfo, (CModelObject*)m_Mover.m_pModel, NULL);
}

void CWndQueryEquip::SetEquipInfoAdd(CItemElem* aEquipInfoAdd)
{
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		m_aEquipInfoAdd[i] = aEquipInfoAdd[i];
}

BOOL CWndQueryEquip::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, 0, pWndParent);
}

////////////////////////////////////////
CWndInventory::CWndInventory()
{
	m_bReport = FALSE;
	m_pSelectItem = NULL;
#ifdef __FL_BOXES_V2
	m_pSelectedBox = NULL;
#endif
	m_pWndConfirmBuy = NULL;

	m_bIsUpgradeMode = FALSE;
	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_pSfxUpgrade = NULL;
	m_dwEnchantWaitTime = 0xffffffff;
	m_TexRemoveItem = NULL;

	m_pModel = NULL;
	m_OldPos = CPoint(0, 0);

	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);

#ifdef __INVENTORY_RESIZE
	arOriginalSizes.clear();
#endif
}
CWndInventory::~CWndInventory()
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmBuy);

	DeleteUpgradeSfx();

#ifdef __INVENTORY_RESIZE
	resetItemRects();
	arOriginalSizes.clear();
#endif
}
void CWndInventory::DeleteUpgradeSfx()
{
	if (m_pSfxUpgrade)
	{
		m_pSfxUpgrade->Delete();
		m_pSfxUpgrade = NULL;
	}
}
void CWndInventory::OnDestroy(void)
{
	SAFE_DELETE(m_pModel);
	SAFE_DELETE(m_pWndConfirmBuy);

#if __VER >= 9 // __CSC_VER9_1
	CWndMixJewel* pWndMixJewel = (CWndMixJewel*)GetWndBase(APP_SMELT_MIXJEWEL);
	if (pWndMixJewel != NULL)
		pWndMixJewel->Destroy();

	CWndExtraction* pWndExtraction = (CWndExtraction*)GetWndBase(APP_SMELT_EXTRACTION);
	if (pWndExtraction != NULL)
		pWndExtraction->Destroy();
#endif //__CSC_VER9_1

#ifdef __WINDOW_INTERFACE_BUG
	CWndRemoveAttribute* pWndRemoveAttribute = (CWndRemoveAttribute*)GetWndBase(APP_REMOVE_ATTRIBUTE);
	if (pWndRemoveAttribute != NULL)
		pWndRemoveAttribute->Destroy();
	CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)GetWndBase(APP_SMELT_REMOVE_PIERCING_EX);
	if (pWndRemovePiercing != NULL)
		pWndRemovePiercing->Destroy();
	CWndLvReqDown* pWndLvReqDown = (CWndLvReqDown*)GetWndBase(APP_LVREQDOWN);
	if (pWndLvReqDown != NULL)
		pWndLvReqDown->Destroy();
	CWndBlessingCancel* pWndBlessingCancel = (CWndBlessingCancel*)GetWndBase(APP_CANCEL_BLESSING);
	if (pWndBlessingCancel != NULL)
		pWndBlessingCancel->Destroy();
	CWndUpgradeBase* pWndUpgradeBase = (CWndUpgradeBase*)GetWndBase(APP_TEST);
	if (pWndUpgradeBase != NULL)
		pWndUpgradeBase->Destroy();
#endif // __WINDOW_INTERFACE_BUG

#if __VER >= 14 // __SMELT_SAFETY
	CWndSmeltSafety* pWndSmeltSafety = (CWndSmeltSafety*)GetWndBase(APP_SMELT_SAFETY);
	if (pWndSmeltSafety != NULL)
		pWndSmeltSafety->Destroy();

	CWndSmeltSafetyConfirm* pWndSmeltSafetyConfirm = (CWndSmeltSafetyConfirm*)GetWndBase(APP_SMELT_SAFETY_CONFIRM);
	if (pWndSmeltSafetyConfirm != NULL)
		pWndSmeltSafetyConfirm->Destroy();
#endif //__SMELT_SAFETY

#if __VER >= 14 // __RESTATE_CONFIRM
	CWndRestateConfirm* pWndRestateConfirm = (CWndRestateConfirm*)GetWndBase(APP_RESTATE_CONFIRM);
	if (pWndRestateConfirm != NULL)
		pWndRestateConfirm->Destroy();
#endif // __RESTATE_CONFIRM

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	CWndPetFoodMill* pWndPetFoodMill = (CWndPetFoodMill*)GetWndBase(APP_PET_FOODMILL);
	if (pWndPetFoodMill != NULL)
		pWndPetFoodMill->Destroy();
#endif // __IMPROVE_SYSTEM_VER15

	CWndShop* pWndShop = (CWndShop*)GetWndBase(APP_SHOP_);
	if (pWndShop != NULL)
		pWndShop->Destroy();

	CWndBank* pWndBank = (CWndBank*)GetWndBase(APP_COMMON_BANK);
	if (pWndBank != NULL)
		pWndBank->Destroy();

#ifdef __MODEL_CHANGE
	if (g_WndMng.m_pWndLookChange)
		g_WndMng.m_pWndLookChange->Destroy();
#endif

#ifdef __INVENTORY_RESIZE
	resetItemRects();
#endif
}
void CWndInventory::OnMouseWndSurface(CPoint point)
{
#ifdef __INVENTORY_RESIZE
	if (m_nResizeDir)
		return;
#endif

	int nTemp = 0;
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		CItemElem* pItemBase = g_pPlayer->GetEquipItem(i);
		CRect DrawRect = m_InvenRect[i];

		CPoint point = GetMousePoint();
		if (DrawRect.PtInRect(point))
		{
			nTemp = i;

			if (i == PARTS_LWEAPON)
			{
				pItemBase = g_pPlayer->GetEquipItem(PARTS_SHIELD);
				if (pItemBase)
					nTemp = PARTS_SHIELD;
				else
					pItemBase = g_pPlayer->GetEquipItem(PARTS_LWEAPON);
			}

			if (pItemBase)
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				g_toolTip.SetSubToolTipNumber(0);
				g_WndMng.PutToolTip_Item(pItemBase, point2, &DrawRect, APP_INVENTORY);
				break;
			}
			else
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&DrawRect);

				CString strText;

				switch (nTemp)
				{
				case PARTS_UPPER_BODY:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_SUIT);
					break;
				case PARTS_HAND:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_GAUNTLET);
					break;
				case PARTS_FOOT:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_BOOTS);
					break;
				case PARTS_CAP:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_HELMET);
					break;
				case PARTS_CLOAK:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLO);
					break;
				case PARTS_LWEAPON:
				case PARTS_RWEAPON:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_WEAPON);
					break;
				case PARTS_SHIELD:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_GUARD);
					break;
				case PARTS_MASK:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_MASK);
					break;
				case PARTS_RIDE:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_RID);
					break;
				case PARTS_NECKLACE1:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_NECKLACE);
					break;
				case PARTS_RING1:
				case PARTS_RING2:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_RING);
					break;
				case PARTS_EARRING1:
				case PARTS_EARRING2:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_EARRING);
					break;
				case PARTS_BULLET:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_BULLET);
					break;
				case PARTS_HAT:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_01);
					break;
				case PARTS_CLOTH:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_02);
					break;
				case PARTS_GLOVE:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_03);
					break;
				case PARTS_BOOTS:
					strText = prj.GetText(TID_TOOLTIP_INVENTORY_CLOTHES_04);
					break;
				}

				g_toolTip.PutToolTip(100000, strText, DrawRect, point2);
				break;
			}
		}
	}
}
void CWndInventory::OnDraw(C2DRender* p2DRender)
{
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;

	CString strGoldNum;
	CWndStatic* pGoldNum = (CWndStatic*)GetDlgItem(WIDC_GOLD_NUM);
	CWndTrade* pWndTrade = (CWndTrade*)g_WndMng.GetWndBase(APP_TRADE);
	strGoldNum.Format(_T("%d"), g_pPlayer->GetGold());

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM21);
	if (lpWndCtrl1 && m_TexRemoveItem)
	{
		CPoint point = GetMousePoint();
		float fScal = 1.0f;
		if (lpWndCtrl1->rect.PtInRect(point))
		{
			fScal = 1.2f;
			CPoint point2 = point;
			CRect DrawRect = lpWndCtrl1->rect;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);
			g_toolTip.PutToolTip(100, prj.GetText(TID_GAME_INVEONTORY_REMOVE_TOOL), DrawRect, point2, 0);
		}
		if (m_TexRemoveItem)
			m_TexRemoveItem->Render(p2DRender, lpWndCtrl1->rect.TopLeft(), CPoint(27, 27), 255, fScal, fScal);
	}

	pGoldNum->SetTitle(strGoldNum);

	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);//D3DCOLOR_TEMP( 255,   0,   0,  50 );
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);//D3DCOLOR_TEMP( 255,  80,  80, 120 );
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);//D3DCOLOR_TEMP( 255,  80,  80, 120 );

	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		DWORD dwAlpha = 255;
		CItemElem* pItemBase = g_pPlayer->GetEquipItem(i);
		if (i == PARTS_LWEAPON)
		{
			CItemElem* pRWeapon = g_pPlayer->GetEquipItem(PARTS_RWEAPON);
			if (pRWeapon && pRWeapon->GetProp()->dwHanded == HD_TWO)
			{
				pItemBase = pRWeapon;
				dwAlpha = 100;
			}
		}

		FLOAT sx = 1.0f;
		FLOAT sy = 1.0f;

		CPoint cpAdd = CPoint(6, 6);
		CRect DrawRect = m_InvenRect[i];

		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2)
		{
			cpAdd = CPoint(0, 0);

			sx = 0.8f;
			sy = 0.8f;
		}
		else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
		{
			cpAdd = CPoint(0, 0);

			sx = 0.9f;
			sy = 0.9f;
		}


#ifdef __MODEL_HIDEFASH
		if (i == PARTS_CLOAK)
		{
			if (!g_pPlayer->IsOptionRenderCostume(4))
				dwAlpha = 140;
		}
		else if (i == PARTS_MASK)
		{
			if (!g_pPlayer->IsOptionRenderCostume(5))
				dwAlpha = 140;
		}
		else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
		{
			if (!g_pPlayer->IsOptionRenderCostume(i - PARTS_HAT))
				dwAlpha = 140;
		}
#endif

		if (pItemBase && pItemBase->GetTexture())
		{
			if (pItemBase->IsFlag(CItemElem::expired))
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_XRGB(255, 100, 100), sx, sy);
			}
			else
			{
				pItemBase->GetTexture()->Render2(p2DRender, DrawRect.TopLeft() + cpAdd, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255), sx, sy);
			}

			CItemElem* pItemElem = pItemBase;
			if (pItemElem->GetProp()->dwPackMax > 1)
			{
				short nItemNum = pItemElem->m_nItemNum;

				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", nItemNum);
				CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
				int x = DrawRect.left;
				int y = DrawRect.top;

				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
				m_p2DRender->TextOut(DrawRect.right - size.cx, y + 41 - size.cy, szTemp, 0xffeaebdd);
				p2DRender->SetFont(pOldFont);
			}
		}
	}

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM20);
	VIEWPORT tmp;
	tmp.m_fRot = m_fRot;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pModel, g_pPlayer, lpFace, &tmp, GetWndId());
}

void CWndInventory::UpDateModel()
{
	SAFE_DELETE(m_pModel);

	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_pModel, OT_MOVER, nMover, MTI_STAND);
	UpdateParts();
	m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}


void CWndInventory::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
#ifdef __FL_BOXES_V2
	m_wndMenuBoxOpen.CreateMenu(this);
#endif
	m_bLButtonDownRot = FALSE;
	m_fRot = 0.0f;

#ifndef __INVENTORY_RESIZE
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM6);
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM7);
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM8);
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM9);
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl(WIDC_CUSTOM10);

	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM11);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM12);
	lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM13);
	lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM14);
	lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM15);
	lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM16);
	lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM17);
	lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM18);
	lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM19);

	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;
#else
	refreshItemRects();
#endif

#ifdef __MODEL_HIDEFASH
	int nArryEquip[6] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS, PARTS_CLOAK, PARTS_MASK };
	for (int i = 0; i < 6; ++i)
	{
		CRect rect = m_InvenRect[nArryEquip[i]];
		rect.right = rect.left + 16;
		rect.bottom = rect.top + 16;
		m_awndCheckHide[i].Create("", WBS_CHECK, rect, this, 100000 + i);
		m_awndCheckHide[i].SetCheck(g_pPlayer->IsOptionRenderCostume(i));
	}
#endif

	UpDateModel();

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_INVENTORY);
	m_wndItemCtrl.Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, 11);
	m_wndItemCtrl.InitItem(&g_pPlayer->m_Inventory, APP_INVENTORY);

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT(TID_GAME_ITEM);
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem(0, &tabTabItem);

#ifdef __INVENTORY_RESIZE
	CRect rectGold = GetWndCtrl(WIDC_STATIC1)->rect;
	m_wndGold.Create("g", WBS_NODRAWFRAME, rectGold, this, WIDC_GOLD);
	m_wndGold.AddWndStyle(WBS_NODRAWFRAME);
#else
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_GOLD);
	m_wndGold.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_GOLD);
	m_wndGold.AddWndStyle(WBS_NODRAWFRAME);
#endif

	CWndStatic* pGoldNum = (CWndStatic*)GetDlgItem(WIDC_GOLD_NUM);
	pGoldNum->AddWndStyle(WSS_MONEY);

	m_pUpgradeItem = NULL;
	m_pUpgradeMaterialItem = NULL;
	m_bIsUpgradeMode = FALSE;
	m_dwEnchantWaitTime = 0xffffffff;

	m_TexRemoveItem = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndInventoryGarbage.dds"), 0xffff00ff);

#ifdef __INVENTORY_RESIZE
	saveOriginRects();
#endif

	CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pSearchBox)
		pSearchBox->SetStringBg("Search..");

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 112 + 48);
	Move(point);
}
BOOL CWndInventory::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(792, 130, 792 + 232, 130 + 405 + 20); // 1024 768


	memset(m_InvenRect, 0, sizeof(CRect) * MAX_HUMAN_PARTS);
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(792, 130), pWndParent);
	//	return CWndNeuz::Create( WBS_VIEW | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME, rect, pWndParent, dwWndId );
}

BOOL CWndInventory::Process()
{
	if (m_pModel)
		m_pModel->FrameMove();

	if (m_dwEnchantWaitTime < g_tmCurrent)
	{
		m_dwEnchantWaitTime = 0xffffffff;

		DeleteUpgradeSfx();

		CItemElem* pItemElem = m_pUpgradeItem;
		if (pItemElem && m_pUpgradeMaterialItem)
		{
			CItemElem* pItemMaterialElem = m_pUpgradeMaterialItem;
			if (pItemMaterialElem && pItemMaterialElem->GetProp())
			{
				if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ELECARD
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_ENCHANT
					|| pItemMaterialElem->GetProp()->dwItemKind3 == IK3_PIERDICE
					)
				{
					if (pItemMaterialElem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
						g_DPlay.SendUltimateEnchantWeapon(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
					else if (pItemMaterialElem->m_dwItemId == II_GEN_MAT_MOONSTONE)
					{
						if (pItemElem->IsCollector(TRUE) || pItemElem->GetProp()->dwItemKind2 == IK2_JEWELRY)
							g_DPlay.SendEnchant(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
					}
					else
						g_DPlay.SendEnchant(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
				}
				else if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD || pItemMaterialElem->GetProp()->dwItemKind3 == IK3_SOCKETCARD2)
					g_DPlay.SendPiercing(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
				else if (pItemMaterialElem->GetProp()->IsNeedTarget())
				{
					g_DPlay.SendDoUseItemTarget(m_pUpgradeMaterialItem->m_dwObjId, pItemElem->m_dwObjId);
				}
				else
					if (pItemMaterialElem->GetProp()->dwItemKind3 == IK3_RANDOM_SCROLL)
					{
						SAFE_DELETE(g_WndMng.m_pWndRandomScrollConfirm);
						g_WndMng.m_pWndRandomScrollConfirm = new CWndRandomScrollConfirm;
						if (0 < pItemElem->GetRandomOpt())
							g_WndMng.m_pWndRandomScrollConfirm->SetItem(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId, TRUE);
						else
							g_WndMng.m_pWndRandomScrollConfirm->SetItem(pItemElem->m_dwObjId, m_pUpgradeMaterialItem->m_dwObjId);
						g_WndMng.m_pWndRandomScrollConfirm->Initialize();
					}
			}
		}
	}
	return TRUE;
}

BOOL CWndInventory::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WIN_ITEMDROP)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if (!lpShortcut->m_pFromWnd)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		if (!pWndFrame)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		BOOL bForbid = TRUE;
		if (nID == 11)
		{
			if (pWndFrame->GetWndId() == GetWndId() && lpShortcut->m_dwData == 0)
			{
				SetForbid(TRUE);
				return FALSE;
			}

			if (pWndFrame->GetWndId() == APP_SHOP_)
			{
				CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
				for (int i = 0; i < (int)(pWndItemCtrl->GetSelectedCount()); i++)
				{
					int nItem = pWndItemCtrl->GetSelectedItem(i);
					pWndItemCtrl->GetItem(nItem);
				}

				CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
				if (!itemElem || !itemElem->GetProp())
					return FALSE;

				CWndShop* pwndShop = (CWndShop*)g_WndMng.GetWndBase(APP_SHOP_);
				if (pwndShop)
				{
					LPCHARACTER lpCharacter = pwndShop->m_pMover->GetCharacter();

#ifndef __NPC_SETVENDORTYPE
					if (lpCharacter->m_nVenderType == 1)
					{
						if (g_pPlayer->m_Inventory.GetAtItemNum(II_CHP_RED) - (int)(lpShortcut->m_dwData)->GetChipCost() >= 0)
						{
							SAFE_DELETE(m_pWndConfirmBuy);
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_nBuyType = 1;
							m_pWndConfirmBuy->Initialize(this);
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox(_T(prj.GetText(TID_GAME_CANNT_BY_REDCHIP)));
					}
#else
					if (lpCharacter->m_nVenderType != 0)
					{
						if (g_pPlayer->m_Inventory.GetAtItemNum(lpCharacter->m_nVenderType) - (int)((CItemElem*)lpShortcut->m_dwData)->GetChipCost() >= 0)
						{
							SAFE_DELETE(m_pWndConfirmBuy);
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->m_nBuyType = lpCharacter->m_nVenderType;
							m_pWndConfirmBuy->Initialize(this);
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0048)));
					}
#endif
					else if (lpCharacter->m_nVenderType == 0)
					{
#ifdef __SHOP_COST_RATE
						if (g_pPlayer->GetGold() - (((CItemElem*)lpShortcut->m_dwData)->GetCost() * prj.m_fShopBuyRate) >= 0)
#else // __SHOP_COST_RATE
						if (g_pPlayer->GetGold() - ((CItemElem*)lpShortcut->m_dwData)->GetCost() >= 0)
#endif // __SHOP_COST_RATE
						{
							SAFE_DELETE(m_pWndConfirmBuy);
							m_pWndConfirmBuy = new CWndConfirmBuy;
							m_pWndConfirmBuy->m_pItemElem = (CItemElem*)lpShortcut->m_dwData;
							m_pWndConfirmBuy->Initialize(this);
							bForbid = FALSE;
						}
						else
							g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0048)));
					}
				}
			}
			else if (pWndFrame->GetWndId() == APP_VENDOR_REVISION)
			{
				CWndVendor* pWndVendor = (CWndVendor*)pWndFrame;
				if (pWndVendor->m_pVendor == g_pPlayer)
				{
					g_DPlay.SendUnregisterPVendorItem((BYTE)(lpShortcut->m_dwIndex));
				}
				else
				{
					SAFE_DELETE(pWndVendor->m_pWndVendorBuy);
					pWndVendor->m_pWndVendorBuy = new CWndVendorBuy((CItemElem*)lpShortcut->m_dwData, lpShortcut->m_dwIndex/*nItem*/);
					pWndVendor->m_pWndVendorBuy->Initialize(pWndVendor);
				}
				bForbid = FALSE;
			}
			else if (pWndFrame->GetWndId() == APP_COMMON_BANK)
			{
				CWndBase* pWndTaget = pWndFrame->m_pCurFocus;
				BYTE nSlot;

				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					if (pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[0]))
					{
						nSlot = 0;
					}
					else if (pWndTaget == &(g_WndMng.m_pWndBank->m_wndItemCtrl[1]))
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}

					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;

					for (int i = 0; i < (int)(pWndItemCtrl->GetSelectedCount()); i++)
					{
						int nItem = pWndItemCtrl->GetSelectedItem(i);
						pWndItemCtrl->GetItem(nItem);
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if (!itemElem || !itemElem->GetProp())
						return FALSE;

					if (itemElem->m_nItemNum > 1)
					{
						SAFE_DELETE(g_WndMng.m_pWndTradeGold);
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD;
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;

						g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
						CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
						CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
					{
						g_DPlay.SendGetItemBank(nSlot, (BYTE)(lpShortcut->m_dwId), 1);
					}
				}
				else
				{
					if (pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[0]))
					{
						nSlot = 0;
					}
					else if (pWndTaget == &(g_WndMng.m_pWndBank->m_wndGold[1]))
					{
						nSlot = 1;
					}
					else
					{
						nSlot = 2;
					}

					SAFE_DELETE(g_WndMng.m_pWndTradeGold);
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nSlot];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_GOLD;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;

					g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
					CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
					CString strMain = prj.GetText(TID_GAME_MOVEPENYA);
					CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
					pStatic->m_strTitle = strMain;
					pStaticCount->m_strTitle = strCount;
				}
				bForbid = FALSE;
			}
#ifdef __FL_CONSUMABLES_STORAGE // Chest -> Inventory
			if (pWndFrame->GetWndId() == APP_CHEST)
			{
				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					if (!pWndItemCtrl)
						return FALSE;

					for (int i = 0; i < (int)(pWndItemCtrl->GetSelectedCount()); i++)
					{
						int nItem = pWndItemCtrl->GetSelectedItem(i);
						pWndItemCtrl->GetItem(nItem);
					}

					CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
					if (!itemElem || !itemElem->GetProp())
						return FALSE;

					if (itemElem->m_nItemNum > 1)
					{
						SAFE_DELETE(g_WndMng.m_pWndTradeGold);
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
						g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_CHEST;
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;
						g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
						g_WndMng.m_pWndTradeGold->MoveParentCenter();

						CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
						CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
						CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
						CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
					else
						g_DPlay.SendMoveItem_Chest(lpShortcut->m_dwId, 1, FALSE);
				}

				bForbid = FALSE;
			}
#endif
			else if (pWndFrame->GetWndId() == APP_POST_READ)
			{
				CWndPostRead* pWndPostRead = (CWndPostRead*)pWndFrame;

				if (pWndPostRead)
				{
					if (lpShortcut->m_dwData != 0)
					{
						pWndPostRead->MailReceiveItem();
					}
					else
					{
						pWndPostRead->MailReceiveGold();
					}

					bForbid = FALSE;
				}
			}
			else if (pWndFrame->GetWndId() == APP_GUILD_BANK)
			{
				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if (pGuild && pGuild->IsGetItem(g_pPlayer->m_idPlayer))
					{
						CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;

						for (int i = 0; i < (int)(pWndItemCtrl->GetSelectedCount()); i++)
						{
							int nItem = pWndItemCtrl->GetSelectedItem(i);
							pWndItemCtrl->GetItem(nItem);
						}
						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if (!itemElem || !itemElem->GetProp())
							return FALSE;

						if (itemElem->m_nItemNum > 1)
						{
							SAFE_DELETE(g_WndMng.m_pWndTradeGold);
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;

							g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
							CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
							CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
							CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
							pStatic->m_strTitle = strMain;
							pStaticCount->m_strTitle = strCount;
						}
						else
						{
							g_DPlay.SendGetItemGuildBank((BYTE)(lpShortcut->m_dwId), 1, 1);
						}
					}
				}
				else
				{
					CGuild* pGuild = g_pPlayer->GetGuild();
					if (pGuild && pGuild->IsGetPenya(g_pPlayer->m_idPlayer))
					{
						SAFE_DELETE(g_WndMng.m_pWndTradeGold);
						g_WndMng.m_pWndTradeGold = new CWndTradeGold;
						memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
						g_WndMng.m_pWndTradeGold->m_dwGold = pGuild->m_nGoldGuild;
						g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_MESSENGER;
						g_WndMng.m_pWndTradeGold->m_pWndBase = this;

						g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
						g_WndMng.m_pWndTradeGold->MoveParentCenter();
						CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
						CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
						CString strMain = prj.GetText(TID_GAME_MOVEPENYA);
						CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
						pStatic->m_strTitle = strMain;
						pStaticCount->m_strTitle = strCount;
					}
				}
				bForbid = FALSE;
			}
		}
		SetForbid(bForbid);
	}
	else if (message == WIN_DBLCLK)
	{
		if (nID == 11) // item
		{
			CItemElem* pItemBase = (CItemElem*)pLResult;
			if (!pItemBase)
				return 0;

			g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
		}
	}
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case 100:
			m_wndItemCtrl.SetWndRect(m_wndItemCtrl.GetWindowRect(TRUE));
			break;
		case WTBID_REPORT:
			break;
#ifdef __CONTAINER_SORTING
		case WIDC_BUTTON1:
		{
			CInventorySort0* pInvSort = new CInventorySort0;
			vector <sItem> vItem;
			vItem.resize(MAX_INVENTORY);

			for (int i = 0; i < MAX_INVENTORY; i++)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(i);
				if (!IsUsableItem(pItemElem))
				{
					vItem[i].dwKind2 = 0xffffffff;
					vItem[i].dwItemId = 0xffffffff;
					vItem[i].nIndex = i;
				}
				else
				{
					ItemProp* pProp = pItemElem->GetProp();
					vItem[i].dwKind2 = pProp->dwItemKind2;
					vItem[i].dwItemId = pItemElem->m_dwItemId;
					vItem[i].nIndex = i;
				}
			}

			sort(vItem.begin(), vItem.end());

			for (size_t i = 0; i < vItem.size(); i++)
				pInvSort->Add(vItem[i].nIndex);

			BYTE nDestPos = 0;
			for (int i = 0; i < MAX_INVENTORY; i++)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(nDestPos);
				if (IsUsableItem(pItemElem))
				{
					if (IsUsingItem(pItemElem))
						nDestPos++;
				}

				BYTE nSrc = pInvSort->GetItemSrc(i);
				pItemElem = g_pPlayer->m_Inventory.GetAt(nSrc);
				if (IsUsableItem(pItemElem))
				{
					if (IsUsingItem(pItemElem))
						continue;
				}
				else
					continue;

				if (nSrc == nDestPos)
				{
					nDestPos++;
					continue;
				}
				pInvSort->MoveItem(i, nDestPos);
				g_DPlay.SendMoveItem(0, nSrc, nDestPos);
				Sleep(3);
				nDestPos++;
			}

			SAFE_DELETE(pInvSort);
		}
		break;
#endif
		case WIDC_BUTTON_1:
		{
			CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
			KillFocus(pSearchBox, NULL);
			pSearchBox->Empty();
		}
		break;
#ifdef __FL_PENYA_CONVERTER
		case WIDC_BTN_NUM1: g_DPlay.SendHdr(PACKETTYPE_CONVERT_PENYA); break;
#endif
		}

#ifdef __MODEL_HIDEFASH
		for (int i = 0; i < 6; ++i)
		{
			if (nID == m_awndCheckHide[i].GetWndId())
				g_DPlay.SendHideCoat(i, !m_awndCheckHide[i].GetCheck());
		}
#endif
	}
#ifdef __FL_BOXES_V2
	else if (message == WNM_TRACK)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (IsUsableItem(pItem))
		{
			m_pSelectedBox = pItem;

			CPoint point = GetMousePoint();
			ClientToScreen(&point);
			m_wndMenuBoxOpen.DeleteAllMenu();
			m_wndMenuBoxOpen.AppendMenu(0, 0, "Open All");
			m_wndMenuBoxOpen.Move(point);
			m_wndMenuBoxOpen.SetVisible(TRUE);
			m_wndMenuBoxOpen.SetFocus();
		}
	}
#endif
#ifdef __CONTAINER_RCLICKS
	else if (message == WNM_ITEMCLICK)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (IsUsableItem(pItem) && pItem->GetProp())
		{
			if (g_WndMng.GetWndBase(APP_GUILD_BANK))
			{
				g_DPlay.SendPutItemGuildBank((BYTE)pItem->m_dwObjId, pItem->m_nItemNum, 1);
			}
			else if (g_WndMng.GetWndBase(APP_CHEST))
			{
				g_DPlay.SendMoveItem_Chest((BYTE)pItem->m_dwObjId, pItem->m_nItemNum, TRUE);
			}
			else if (g_WndMng.GetWndBase(APP_COMMON_BANK))
			{
				for (int i = 0; i < MAX_CHARACTER_LIST; ++i)
				{
					if (g_pPlayer->m_nSlot == i || (g_pPlayer->m_idPlayerBank[i] != 0 && g_pPlayer->IsCommBank()))
					{
						if (g_pPlayer->m_Bank[i].IsFull(pItem, pItem->GetProp(), pItem->m_nItemNum) == FALSE)
						{
							g_DPlay.SendPutItemBank(i, (BYTE)pItem->m_dwObjId, pItem->m_nItemNum);
							break;
						}
					}
				}
			}
			else if (g_WndMng.GetWndBase(APP_TRADE))
			{
				for (int i = 0; i < MAX_TRADE; ++i)
				{
					if (g_pPlayer->m_vtInfo.GetItem(i) == NULL)
					{
						g_DPlay.SendTradePut(i, pItem->m_dwObjId, pItem->m_nItemNum);
						break;
					}
				}
			}
			else if (g_WndMng.GetWndBase(APP_SHOP_))
			{
				SHORTCUT shortcut;
				shortcut.m_pFromWnd = this;
				shortcut.m_dwData = (DWORD)pLResult;
				shortcut.m_dwType = ITYPE_ITEM;
				g_WndMng.GetWndBase(APP_SHOP_)->OnChildNotify(WIN_ITEMDROP, 11, (LRESULT*)&shortcut);
			}
		}
	}
#endif

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndInventory::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
#ifdef __FL_BOXES_V2
	if (pWndBase == &m_wndMenuBoxOpen)
	{
		if (IsUsableItem(m_pSelectedBox))
		{
			switch (nID)
			{
			case 0:
			{
				g_DPlay.SendBulkOpenBox(m_pSelectedBox->m_dwObjId);
				break;
			}
			}
		}

		m_pSelectedBox = NULL;
		m_wndMenuBoxOpen.SetVisible(FALSE);
	}
#endif
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndInventory::OnSize(UINT nType, int cx, int cy)
{
#ifdef __INVENTORY_RESIZE
	CPoint cpSizeDiff;

	cpSizeDiff.x = (cx - arOriginalSizes[0].rect.Width());
	cpSizeDiff.y = (cy - arOriginalSizes[0].rect.Height());

	m_wndItemCtrl.OnMouseWheel(0, 0, CPoint(0, 0));
	m_wndItemCtrl.SetMinScrollPos();

	for (size_t i = 1; i < arOriginalSizes.size(); i++)
	{
		switch (arOriginalSizes[i].type)
		{
		case STATIC:
			GetDlgItem(arOriginalSizes[i].id)->SetWndRect(getItemRect(arOriginalSizes[i].id, cpSizeDiff));
			break;

		case CUSTOM:
			GetWndCtrl(arOriginalSizes[i].id)->rect = getItemRect(arOriginalSizes[i].id, cpSizeDiff);
			break;

		case CHECKBOX:
#ifdef __MODEL_HIDEFASH
			m_awndCheckHide[arOriginalSizes[i].id].SetWndRect(getItemRect(arOriginalSizes[i].id, cpSizeDiff, CHECKBOX));
#endif
			break;
		}
	}

	refreshItemRects();
#endif

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndInventory::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = FALSE;
	ReleaseCapture();

}
void CWndInventory::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM20);
	if (lpWndCtrl->rect.PtInRect(point))
	{
		SetCapture();
		m_OldPos = point;
		m_bLButtonDownRot = TRUE;
	}

	if (m_bLButtonDown)
	{
		for (int i = 2; i < MAX_HUMAN_PARTS; i++)
		{
			CRect DrawRect = m_InvenRect[i];

			if (DrawRect.PtInRect(point))
			{
				CItemElem* pItemBase = g_pPlayer->GetEquipItem(i);
				if (pItemBase)
				{
					m_pSelectItem = pItemBase;
					return;
				}
			}
		}
	}
	m_pSelectItem = NULL;
}

void CWndInventory::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		CRect DrawRect = m_InvenRect[i];

		if (DrawRect.PtInRect(point))
		{
			CItemElem* pItemElem = g_pPlayer->GetEquipItem(i);
			if (pItemElem)
				g_DPlay.SendDoUseItem(pItemElem->m_dwObjId, "", TYPE_INVENTORY);
		}
	}
}

void CWndInventory::OnRButtonDown(UINT nFlags, CPoint point)
{
	BaseMouseCursor();
}

BOOL CWndInventory::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	if (m_bIsUpgradeMode)
		SetMouseCursor(CUR_HAMMER);
	else
		CWndNeuz::OnSetCursor(pWndBase, nHitTest, message);

	return TRUE;
}

void CWndInventory::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef __INVENTORY_RESIZE
	if (m_nResizeDir)
		return;
#endif

	if (m_bLButtonDownRot)
	{
		m_fRot += (m_OldPos.x - point.x) * 0.5f;
	}

	m_OldPos = point;

	if (m_bLButtonDown && IsPush())
	{
		if (m_bPickup == TRUE)
		{
			m_pSelectItem = NULL;
			return;
		}

		if (m_pSelectItem && m_pSelectItem->GetProp())
		{
			m_GlobalShortcut.m_pFromWnd = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex = 0xffffffff;
			m_GlobalShortcut.m_dwType = 0;
			m_GlobalShortcut.m_dwId = m_pSelectItem->m_dwObjId;
			m_GlobalShortcut.m_pTexture = m_pSelectItem->GetTexture();
			m_GlobalShortcut.m_dwData = (DWORD)m_pSelectItem;
			m_GlobalShortcut.m_nLocation = TYPE_INVENTORY;
			_tcscpy(m_GlobalShortcut.m_szString, m_pSelectItem->GetProp()->szName);
		}
	}
}
BOOL CWndInventory::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();

	if (pWndFrame == NULL)
		return FALSE;

	if (g_WndMng.GetWndBase(APP_SHOP_) ||
		g_WndMng.GetWndBase(APP_BANK) ||
		g_WndMng.GetWndBase(APP_TRADE))
	{
		if (pWndFrame->GetWndId() == APP_INVENTORY)
		{
			SetForbid(TRUE);

			g_WndMng.PutString(prj.GetText(TID_GAME_TRADELIMITUSING), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));

			return FALSE;
		}
	}
	else if (g_WndMng.GetWndBase(APP_REPAIR))
	{
		if (pWndFrame->GetWndId() == APP_INVENTORY)
		{
			SetForbid(TRUE);

			g_WndMng.PutString(prj.GetText(TID_GAME_REPAIR_NOTUSE), NULL, prj.GetTextColor(TID_GAME_TRADELIMITUSING));

			return FALSE;
		}
	}
	else if (g_WndMng.GetWndBase(APP_INVENTORY))
	{
		if (pWndFrame->GetWndId() == APP_INVENTORY)
		{
			LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM21);
			if (lpWndCtrl1->rect.PtInRect(point))
			{
				if (pShortcut->m_dwType == ITYPE_ITEM && pShortcut->m_dwData)
				{
					CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
					if (!IsUsableItem(pItemElem))
						return FALSE;
					if (pItemElem->GetExtra() > 0)
						return FALSE;
					if (g_pPlayer->m_Inventory.IsEquip(pShortcut->m_dwId))
						return FALSE;
					if (g_pPlayer->IsUsing(pItemElem))
						return FALSE;
					if (pItemElem->IsUndestructable() == TRUE)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM), NULL, prj.GetTextColor(TID_GAME_ERROR_UNDESTRUCTABLE_ITEM));
						return FALSE;
					}

					SAFE_DELETE(g_WndMng.m_pWndInvenRemoveItem);
					g_WndMng.m_pWndInvenRemoveItem = new CWndInvenRemoveItem;
					g_WndMng.m_pWndInvenRemoveItem->Initialize(&g_WndMng, APP_INVEN_REMOVE_ITEM);
					g_WndMng.m_pWndInvenRemoveItem->InitItem(pItemElem);
					return TRUE;
				}
			}
		}
	}

	return CWndBase::OnDropIcon(pShortcut, point);
}
void CWndInventory::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndConfirmBuy)
		SAFE_DELETE(m_pWndConfirmBuy);
}
#ifdef __INVENTORY_RESIZE
void CWndInventory::saveOriginRects()
{
	if (arOriginalSizes.size() > 0)
		return;

	arOriginalSizes.push_back(ctrlhelp(0, PARENT, GetClientRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_INVENTORY, STATIC, GetDlgItem(WIDC_INVENTORY)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_GOLD, STATIC, GetDlgItem(WIDC_GOLD)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_GOLD_NUM, STATIC, GetDlgItem(WIDC_GOLD_NUM)->GetWndRect()));
#ifdef __FL_PENYA_CONVERTER
	arOriginalSizes.push_back(ctrlhelp(WIDC_BTN_NUM1, STATIC, GetWndCtrl(WIDC_BTN_NUM1)->rect));
#endif
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC1, STATIC, GetDlgItem(WIDC_STATIC1)->GetWndRect()));
#ifdef __CONTAINER_SORTING
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON1, STATIC, GetWndCtrl(WIDC_BUTTON1)->rect));
#endif
	arOriginalSizes.push_back(ctrlhelp(WIDC_BUTTON_1, STATIC, GetWndCtrl(WIDC_BUTTON_1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_EDIT1, STATIC, GetDlgItem(WIDC_EDIT1)->GetWndRect()));
#ifdef __MODEL_HIDEFASH
	for (size_t i = 0; i < (sizeof(m_awndCheckHide) / sizeof(CWndButton)); i++)
		arOriginalSizes.push_back(ctrlhelp(i, CHECKBOX, m_awndCheckHide[i].GetWndRect()));
#endif
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC11, STATIC, GetDlgItem(WIDC_STATIC11)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC12, STATIC, GetDlgItem(WIDC_STATIC12)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC13, STATIC, GetDlgItem(WIDC_STATIC13)->GetWndRect()));
	arOriginalSizes.push_back(ctrlhelp(WIDC_STATIC14, STATIC, GetDlgItem(WIDC_STATIC14)->GetWndRect()));

	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM1, CUSTOM, GetWndCtrl(WIDC_CUSTOM1)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM2, CUSTOM, GetWndCtrl(WIDC_CUSTOM2)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM3, CUSTOM, GetWndCtrl(WIDC_CUSTOM3)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM4, CUSTOM, GetWndCtrl(WIDC_CUSTOM4)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM5, CUSTOM, GetWndCtrl(WIDC_CUSTOM5)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM6, CUSTOM, GetWndCtrl(WIDC_CUSTOM6)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM7, CUSTOM, GetWndCtrl(WIDC_CUSTOM7)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM8, CUSTOM, GetWndCtrl(WIDC_CUSTOM8)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM9, CUSTOM, GetWndCtrl(WIDC_CUSTOM9)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM10, CUSTOM, GetWndCtrl(WIDC_CUSTOM10)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM11, CUSTOM, GetWndCtrl(WIDC_CUSTOM11)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM12, CUSTOM, GetWndCtrl(WIDC_CUSTOM12)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM13, CUSTOM, GetWndCtrl(WIDC_CUSTOM13)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM14, CUSTOM, GetWndCtrl(WIDC_CUSTOM14)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM15, CUSTOM, GetWndCtrl(WIDC_CUSTOM15)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM16, CUSTOM, GetWndCtrl(WIDC_CUSTOM16)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM17, CUSTOM, GetWndCtrl(WIDC_CUSTOM17)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM18, CUSTOM, GetWndCtrl(WIDC_CUSTOM18)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM19, CUSTOM, GetWndCtrl(WIDC_CUSTOM19)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM20, CUSTOM, GetWndCtrl(WIDC_CUSTOM20)->rect));
	arOriginalSizes.push_back(ctrlhelp(WIDC_CUSTOM21, CUSTOM, GetWndCtrl(WIDC_CUSTOM21)->rect));
}

void CWndInventory::resetItemRects()
{
	for (size_t i = 1; i < arOriginalSizes.size(); i++)
	{
		switch (arOriginalSizes[i].type)
		{
		case STATIC:
			GetDlgItem(arOriginalSizes[i].id)->SetWndRect(arOriginalSizes[i].rect);
			break;

		case CUSTOM:
			GetWndCtrl(arOriginalSizes[i].id)->rect = arOriginalSizes[i].rect;
			break;

		case CHECKBOX:
#ifdef __MODEL_HIDEFASH
			m_awndCheckHide[arOriginalSizes[i].id].SetWndRect(arOriginalSizes[i].rect);
#endif
			break;
		}
	}
}

CRect CWndInventory::getItemRect(int id, CPoint sizediff, ctrltype type)
{
	for (size_t i = 0; i < arOriginalSizes.size(); i++)
	{
		if ((type == PARENT || arOriginalSizes[i].type == type) && arOriginalSizes[i].id == id)
		{
			if (type == CHECKBOX)
			{
				switch (id)
				{
				case 4: // cloak
					return arOriginalSizes[i].rect;
				case 5: // Mask
					return arOriginalSizes[i].rect;


				default:
					return CRect(arOriginalSizes[i].rect.left + (sizediff.x / 2), arOriginalSizes[i].rect.top, arOriginalSizes[i].rect.right + (sizediff.x / 2), arOriginalSizes[i].rect.bottom);
				}
			}

			switch (id)
			{
			case WIDC_STATIC11: // left
			{
				CRect t1 = getItemRect(WIDC_CUSTOM5, sizediff);
				CRect t2 = getItemRect(WIDC_CUSTOM9, sizediff);
				return CRect(t1.left, t1.top, t2.right, t2.bottom);
			}

			case WIDC_STATIC12: // top
			{
				CRect t1 = getItemRect(WIDC_CUSTOM11, sizediff);
				CRect t2 = getItemRect(WIDC_CUSTOM15, sizediff);
				return CRect(t1.left, t1.top, t2.right, t2.bottom);
			}

			case WIDC_STATIC13: // right
			{
				CRect t1 = getItemRect(WIDC_CUSTOM1, sizediff);
				CRect t2 = getItemRect(WIDC_CUSTOM10, sizediff);
				return CRect(t1.left, t1.top, t2.right, t2.bottom);
			}

			case WIDC_STATIC14: // bottom
			{
				CRect t1 = getItemRect(WIDC_CUSTOM16, sizediff);
				CRect t2 = getItemRect(WIDC_CUSTOM19, sizediff);
				return CRect(t1.left, t1.top, t2.right, t2.bottom);
			}

			case WIDC_CUSTOM5:
			case WIDC_CUSTOM6:
			case WIDC_CUSTOM7:
			case WIDC_CUSTOM8:
			case WIDC_CUSTOM9:
			case WIDC_BUTTON_1:
				return arOriginalSizes[i].rect;

			case WIDC_CUSTOM1:
			case WIDC_CUSTOM2:
			case WIDC_CUSTOM3:
			case WIDC_CUSTOM4:
			case WIDC_CUSTOM10:
				return CRect(arOriginalSizes[i].rect.left + sizediff.x, arOriginalSizes[i].rect.top, arOriginalSizes[i].rect.right + sizediff.x, arOriginalSizes[i].rect.bottom);

			case WIDC_CUSTOM11:
			case WIDC_CUSTOM12:
			case WIDC_CUSTOM13:
			case WIDC_CUSTOM14:
			case WIDC_CUSTOM15:
			case WIDC_CUSTOM16:
			case WIDC_CUSTOM17:
			case WIDC_CUSTOM18:
			case WIDC_CUSTOM19:
			case WIDC_CUSTOM20:
				return CRect(arOriginalSizes[i].rect.left + (sizediff.x / 2), arOriginalSizes[i].rect.top, arOriginalSizes[i].rect.right + (sizediff.x / 2), arOriginalSizes[i].rect.bottom);

			case WIDC_GOLD:
			case WIDC_GOLD_NUM:
			case WIDC_STATIC1:
#ifdef __FL_PENYA_CONVERTER
			case WIDC_BTN_NUM1:
#endif
				return CRect(arOriginalSizes[i].rect.left + sizediff.x, arOriginalSizes[i].rect.top + sizediff.y, arOriginalSizes[i].rect.right + sizediff.x, arOriginalSizes[i].rect.bottom + sizediff.y);

			case WIDC_CUSTOM21:
#ifdef __CONTAINER_SORTING
			case WIDC_BUTTON1:
#endif
				return CRect(arOriginalSizes[i].rect.left, arOriginalSizes[i].rect.top + sizediff.y, arOriginalSizes[i].rect.right, arOriginalSizes[i].rect.bottom + sizediff.y);

			case WIDC_INVENTORY:
				return CRect(arOriginalSizes[i].rect.left, arOriginalSizes[i].rect.top, arOriginalSizes[i].rect.right + sizediff.x, arOriginalSizes[i].rect.bottom + sizediff.y);

			case WIDC_EDIT1:
				return CRect(arOriginalSizes[i].rect.left, arOriginalSizes[i].rect.top, arOriginalSizes[i].rect.right + sizediff.x, arOriginalSizes[i].rect.bottom);
			}
		}
	}

	return CRect(0, 0, 0, 0);
}

void CWndInventory::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	if (m_nResizeDir)
	{
		int width = abs(m_rectWindow.Width() - rectWnd.Width()), height = abs(m_rectWindow.Height() - rectWnd.Height());

		if ((width < nGridTexSize || (width % nGridTexSize) > 0) && (height < nGridTexSize || (height % nGridTexSize) > 0)) // no half resizes (only full item size)
		{
			return;
		}

		if (width < nGridTexSize || (width % nGridTexSize) > 0)
		{
			rectWnd.left = m_rectWindow.left;
			rectWnd.right = m_rectWindow.right;
		}

		if (height < nGridTexSize || (height % nGridTexSize) > 0)
		{
			rectWnd.top = m_rectWindow.top;
			rectWnd.bottom = m_rectWindow.bottom;
		}
	}

	AdjustMinRect(&rectWnd, 16 * 15, 16 * 25);
	AdjustMaxRect(&rectWnd, 16 * 27, 16 * 41);

	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}


void CWndInventory::refreshItemRects()
{
	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM6);
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM7);
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM8);
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM9);
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl(WIDC_CUSTOM10);

	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM11);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM12);
	lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM13);
	lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM14);
	lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM15);
	lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM16);
	lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM17);
	lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM18);
	lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM19);

	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;
}
#endif


#if __VER >= 13 // __RENEW_CHARINFO
CWndCharInfo::CWndCharInfo()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;
	m_nSpeedRun = 0;
	m_nCriticalRate = 0;
	m_nSpellCastSpeed = 0;
	m_nPvpDamage = 0;
	m_nHitRate = 0;
	m_nBlockMelee = 0;
	m_nBlockRange = 0;
	m_nDodgeRate = 0;

	m_bExpert = FALSE;
	m_fWaitingConfirm = FALSE;
	m_nDisplay = 1;

}


CWndCharInfo::~CWndCharInfo()
{
}

void CWndCharInfo::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	CRect rect = GetClientRect();
	rect.bottom -= 30;
	int			y = 0, nNext = 15, nyAdd3 = 21;
	DWORD		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	BYTE		checkhero = g_pPlayer->GetLegendChar();
	int			xpos = 0;
	int			ypos = 0;
	CTexture* pTexture;
	CString		strPath;
	CPoint		point;
	y = 10;
	p2DRender->TextOut(90, y, prj.GetText((TID_APP_CHARACTER_BASE)), dwColor);


	y = 10 + nyAdd3;
	if (TRUE)
	{
		p2DRender->TextOut(80, y, g_pPlayer->GetName(), dwColor); y += nNext;
		p2DRender->TextOut(80, y, g_pPlayer->GetJobString(), dwColor); y += nNext;
		ypos = y;
		if (checkhero == LEGEND_CLASS_MASTER)
		{
			if (g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
		if (checkhero == LEGEND_CLASS_HERO)
			xpos = 103;
		if (checkhero == LEGEND_CLASS_LEGENDHERO)
			xpos = 103;

		p2DRender->TextOut(80, y, g_pPlayer->GetLevel(), dwColor); y += nNext;
		y = 81 + nyAdd3;
	}
	else
	{
		p2DRender->TextOut(50, y, g_pPlayer->GetName(), dwColor); y += nNext;
		p2DRender->TextOut(50, y, g_pPlayer->GetJobString(), dwColor); y += nNext;
		ypos = y;
		if (checkhero == LEGEND_CLASS_MASTER)
		{
			if (g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
		if (checkhero == LEGEND_CLASS_HERO)
			xpos = 73;
		if (checkhero == LEGEND_CLASS_LEGENDHERO)
			xpos = 73;

		p2DRender->TextOut(50, y, g_pPlayer->GetLevel(), dwColor); y += nNext;
		y = 81 + nyAdd3;
	}

	point.x = xpos + 3;
	point.y = ypos;
	if (checkhero == LEGEND_CLASS_MASTER)
	{
		if (/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark1.dds");
		else if (g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark2.dds");
		else if (g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark3.dds");
		else if (g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark4.dds");
		else if (g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark5.dds");
		else if (g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, point);

	}

	else if (g_pPlayer->IsHero() || g_pPlayer->IsLegendHero())
	{
		strPath = MakePath(DIR_ICON, "icon_hero_small.dds");
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, point);
	}
	y = 55 + nyAdd3;
	CString strServerName;

	strServerName.Format("%s", g_dpCertified.GetServerName(g_Option.m_nSer));
	if (TRUE)
	{
		p2DRender->TextOut(80, y, strServerName, dwColor);
		y += nNext;
	}
	else
	{
		p2DRender->TextOut(50, y, strServerName, dwColor);
		y += nNext;
	}


	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for (int j = 0; j < (int)(g_dpCertified.m_dwSizeofServerset); j++)
	{
		if (g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if (nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if (g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if (pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format("%s", g_dpCertified.m_aServerset[j + g_Option.m_nMSer].lpName);
				if (TRUE)
					p2DRender->TextOut(80, y, strServerName, dwColor);
				else
					p2DRender->TextOut(50, y, strServerName, dwColor);

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}


	y = 10 + nyAdd3;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor); y += nNext;

	p2DRender->TextOut(88, 111, prj.GetText((TID_APP_CHARACTER_DETAIL)), D3DCOLOR_ARGB(255, 0, 0, 0));

	/////////////////////////// detail begin ////////////////////////////
	int nyAdd = 121;
	int x = 5, nNextX = 100;
	x = 55; y = 15 + nyAdd;
	nNext = 15;

	RenderATK(p2DRender, x, y);
	y += nNext;

	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	if (m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense(FALSE))
	{
		if ((g_nRenderCnt / 8) & 1)
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

		p2DRender->TextOut(x, y, GetVirtualDEF(), dwColor);
	}
	else
		p2DRender->TextOut(x, y, g_pPlayer->GetShowDefense(FALSE), dwColor); y += nNext;

	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	CString strMsg;
	float fAttackSpeed;
	if (m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed())
	{
		if ((g_nRenderCnt / 8) & 1)
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format("%d%%", int(fAttackSpeed * 100.0f) / 2);
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;


	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	int nCritical = 0;
	if (m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb())
	{
		if ((g_nRenderCnt / 8) & 1)
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

		nCritical = GetVirtualCritical();
	}
	else
		nCritical = g_pPlayer->GetCriticalProb();

	if (nCritical > 100)
		nCritical = 100;
	strMsg.Format("%d%%", nCritical);
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	strMsg.Format("%d%%", g_pPlayer->GetSpeedRun());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	strMsg.Format("%d%%", g_pPlayer->GetSpellCastSpeed());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	x = 167; y = 15 + nyAdd;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	strMsg.Format("%d%%", g_pPlayer->GetCriticalRate());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	if (GetVirtualDodgeRate() != g_pPlayer->GetDodgeRate())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		strMsg.Format("%d%%", GetVirtualDodgeRate());
	}
	else
		strMsg.Format("%d%%", g_pPlayer->GetDodgeRate());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	int nHitRate = 0;
	if (GetVirtualHitRate() != g_pPlayer->GetHitRate())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		nHitRate = GetVirtualHitRate();
	}
	else
		nHitRate = g_pPlayer->GetHitRate();

	strMsg.Format("%d%%", nHitRate);

	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	if (GetVirtualBlockMelee() != g_pPlayer->GetBlockMelee())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		strMsg.Format("%d%%", GetVirtualBlockMelee());
	}
	else
		strMsg.Format("%d%%", g_pPlayer->GetBlockMelee());

	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	if (GetVirtualBlockRange() != g_pPlayer->GetBlockRange())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		strMsg.Format("%d%%", GetVirtualBlockRange());
	}
	else
		strMsg.Format("%d%%", g_pPlayer->GetBlockRange());

	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;


	strMsg.Format("%d%%", g_pPlayer->GetPvpDamage());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	x = 15; nNextX = 60;

	y = 115 + nyAdd;
	int StatYPos = 50;

	if (g_pPlayer->m_nStr == g_pPlayer->GetStr())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	}
	else if (g_pPlayer->m_nStr < g_pPlayer->GetStr())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
	}
	else
	{
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	}

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetStr(), dwColor); y += nNext;

	if (g_pPlayer->m_nSta == g_pPlayer->GetSta())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	}
	else if (g_pPlayer->m_nSta < g_pPlayer->GetSta())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
	}
	else
	{
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	}

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetSta(), dwColor); y += nNext;

	if (g_pPlayer->m_nDex == g_pPlayer->GetDex())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	}
	else if (g_pPlayer->m_nDex < g_pPlayer->GetDex())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
	}
	else
	{
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	}

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetDex(), dwColor); y += nNext;

	if (g_pPlayer->m_nInt == g_pPlayer->GetInt())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	}
	else if (g_pPlayer->m_nInt < g_pPlayer->GetInt())
	{
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
	}
	else
	{
		dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	}

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetInt(), dwColor); y += nNext;

	if (m_nGpPoint)
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
	}

	p2DRender->TextOut(110, y + 4, m_nGpPoint, dwColor); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect(10, 52 + nyAdd, 80, 65 + nyAdd);
	rectHittest[1].SetRect(10, 67 + nyAdd, 80, 80 + nyAdd);
	rectHittest[2].SetRect(10, 82 + nyAdd, 80, 95 + nyAdd);
	rectHittest[3].SetRect(10, 97 + nyAdd, 80, 110 + nyAdd);
	rectHittest[4].SetRect(10, 112 + nyAdd, 160, 125 + nyAdd);

	CRect rectTemp;
	CPoint ptTemp;
	// ( Str, Sta, Dex, Int, GP )
	CPoint ptMouse = GetMousePoint();
	y = 15 + nyAdd;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_CRT_RATE), dwColor); y += nNext;

	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_DODGE), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_HIT_RATE), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_M_BLOCK), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_SPD_RUN), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_R_BLOCK), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_SPL_SPEED), dwColor);
	p2DRender->TextOut(105, y, prj.GetText(TID_PVP_DAMAGE), dwColor); y += nNext;
	y += 10;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STR), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STA), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_INT), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor); y += nNext;
	//////////////// pvp ///////////////////////// deleted 24.12.19
}




void CWndCharInfo::OnInitialUpdate()
{

	CWndBase::OnInitialUpdate();

	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("WndNewCharacterWhite.tga")), TRUE);

	int nyAdd = 121;
	int posY = 114 + nyAdd;
	int posX = 136;

	m_editStrCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY - 2, posX - 4, posY + 16), this, 100);
	m_wndStrPlus.Create("<", 0, CRect(posX + 6, posY + 2, posX + 20, posY + 18), this, 101);
	m_wndStrMinus.Create("<", 0, CRect(posX + 22, posY + 2, posX + 36, posY + 18), this, 102); posY += 15;

	m_editStaCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY - 2, posX - 4, posY + 16), this, 103);
	m_wndStaPlus.Create("<", 0, CRect(posX + 6, posY + 2, posX + 20, posY + 18), this, 104);
	m_wndStaMinus.Create("<", 0, CRect(posX + 22, posY + 2, posX + 36, posY + 18), this, 105); posY += 15;
	m_editDexCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY - 2, posX - 4, posY + 16), this, 106);
	m_wndDexPlus.Create("<", 0, CRect(posX + 6, posY + 2, posX + 20, posY + 18), this, 107);
	m_wndDexMinus.Create("<", 0, CRect(posX + 22, posY + 2, posX + 36, posY + 18), this, 108); posY += 15;

	m_editIntCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY - 2, posX - 4, posY + 16), this, 109);

	m_wndIntPlus.Create("<", 0, CRect(posX + 6, posY + 2, posX + 20, posY + 18), this, 110);
	m_wndIntMinus.Create("<", 0, CRect(posX + 22, posY + 2, posX + 36, posY + 18), this, 111);
	m_wndStrPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStrMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndStaPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStaMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndDexPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndDexMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndIntPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndIntMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);


	posY += 50;

	m_wndApply.Create("Ok", 0, CRect(posX - 85, posY, posX - 35, posY + 22), this, 112);
	m_wndReset.Create("Cancel", 0, CRect(posX - 15, posY, posX + 35, posY + 22), this, 113);
	if (::GetLanguage() == LANG_FRE)
	{
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateOk.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateCancel.tga")), TRUE);
	}
	else
	{
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharApply.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharReset.tga")), TRUE);
	}


	m_nGpPoint = g_pPlayer->GetRemainGP();

	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if (g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}
	int nyAdd2 = 280;
	int y = 105 + nyAdd2;

	RefreshStatPoint();
	MakeVertexBuffer();
}


BOOL CWndCharInfo::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rect = m_pWndRoot->GetWindowRect();
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);

}


BOOL CWndCharInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}


BOOL CWndCharInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int editnum = 0;

	if (nID == 100 || nID == 103 || nID == 106 || nID == 109)
	{
		CString tempnum = m_editStrCount.GetString();
		for (int i = 0; i < tempnum.GetLength(); i++)
		{
			if (isdigit2(tempnum.GetAt(i)) == FALSE)
				return FALSE;
		}
	}
	switch (nID)
	{
	case 100:
		editnum = atoi(m_editStrCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
		m_nStrCount = editnum;
		break;
	case 103:
		editnum = atoi(m_editStaCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
		m_nStaCount = editnum;
		break;
	case 106:
		editnum = atoi(m_editDexCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
		m_nDexCount = editnum;
		break;
	case 109:
		editnum = atoi(m_editIntCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
		m_nIntCount = editnum;
		break;
	}

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case 101: //Str Plus
			m_nStrCount++;
			break;
		case 102: //Str Minus
			m_nStrCount--;
			break;
		case 104: //Sta Plus
			m_nStaCount++;
			break;
		case 105: //Sta Minus
			m_nStaCount--;
			break;
		case 107: //Dex Plus
			m_nDexCount++;
			break;
		case 108: //Dex Minus
			m_nDexCount--;
			break;
		case 110: //Int Plus
			m_nIntCount++;
			break;
		case 111: //Int Minus
			m_nIntCount--;
			break;
		case 112: //Apply
			if (m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			{
				SAFE_DELETE(g_WndMng.m_pWndStateConfirm);
				g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
				g_WndMng.m_pWndStateConfirm->Initialize();
			}
			break;
		case 113: //Reset
			m_nStrCount = 0;
			m_nStaCount = 0;
			m_nDexCount = 0;
			m_nIntCount = 0;
			RefreshStatPoint();
			break;

		case JOB_MERCENARY:
		case JOB_ACROBAT:
		case JOB_ASSIST:
		case JOB_MAGICIAN:
		case JOB_PUPPETEER:
		{
			if (nID != g_pPlayer->GetJob())
			{
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0037)));
			}
			break;
		}
		}
	}

	if (nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if (g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if (g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}

	return CWndBase::OnChildNotify(message, nID, pLResult);
}


void CWndCharInfo::OnSize(UINT nType, int cx, int cy)
{

}


void CWndCharInfo::OnLButtonUp(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnLButtonDown(UINT nFlags, CPoint point)
{

}


void CWndCharInfo::OnMouseWndSurface(CPoint point)
{

}


BOOL CWndCharInfo::Process()
{
	//Control Button Plus or Minus
	if (m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if (m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if (m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if (m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if (m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);

		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);

		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if (m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}


void CWndCharInfo::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}


void CWndCharInfo::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;

	*pnMin = 0;
	*pnMax = 0;

	if (g_pPlayer)
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp(nParts);
		if (pItemProp == NULL)
			return;

		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;

		*pnMin = g_pPlayer->GetParam(DST_ABILITY_MIN, *pnMin);
		*pnMax = g_pPlayer->GetParam(DST_ABILITY_MAX, *pnMax);

		int nATK = 0;
		switch (pItemProp->dwWeaponType)
		{
		case WT_MELEE_SWD:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_AXE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MELEE_STAFF:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_STICK:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)));
			break;
		case WT_MELEE_KNUCKLE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MAGIC_WAND:
			nATK = (int)((g_pPlayer->GetInt() + m_nIntCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f);
			break;
		case WT_MELEE_YOYO:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_RANGE_BOW:
			nATK = (int)((((g_pPlayer->GetDex() + m_nDexCount - 14) * 4.0f + (g_pPlayer->GetLevel() * 1.3f) + ((g_pPlayer->GetStr() + m_nStrCount) * 0.2f)) * 0.7f));
			break;
		}

		nATK += g_pPlayer->GetPlusWeaponATK(pItemProp->dwWeaponType);

		int nPlus = nATK + g_pPlayer->GetParam(DST_CHR_DMG, 0);

		*pnMin += nPlus;
		*pnMax += nPlus;

		CItemElem* pWeapon = g_pPlayer->GetWeaponItem(nParts);
		if (pWeapon && pWeapon->GetProp())
		{
			float f = g_pPlayer->GetItemMultiplier(pWeapon);
			*pnMin = (int)(*pnMin * f);
			*pnMax = (int)(*pnMax * f);
		}

		if (pWeapon)
		{
			int nOption = pWeapon->GetAbilityOption();
			if (nOption > 0)
			{
				int nValue = (int)pow((float)(nOption), 1.5f);

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}


int CWndCharInfo::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset(&info, 0x00, sizeof(info));
	info.dwAtkFlags = AF_GENERIC;

	float fFactor = 1.0f;
	if (g_pPlayer)
	{
		JobProp* pProperty = prj.GetJobProp(g_pPlayer->GetJob());
		assert(pProperty);
		fFactor = pProperty->fFactorDef;
	}
	nDefense = (int)(((((g_pPlayer->GetLevel() * 2) + ((g_pPlayer->GetSta() + m_nStaCount) / 2)) / 2.8f) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor));
	nDefense = nDefense + (g_pPlayer->GetDefenseByItem(FALSE) / 4);
	nDefense = nDefense + (g_pPlayer->GetParam(DST_ADJDEF, 0));

	nDefense = (int)(nDefense * g_pPlayer->GetDEFMultiplier(&info));

#ifdef __JEFF_11
	if (nDefense < 0)
		nDefense = 0;
#endif	// __JEFF_11
	return nDefense;
}


int CWndCharInfo::GetVirtualCritical()
{
	int nCritical;
	nCritical = ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical = (int)(nCritical * g_pPlayer->GetJobPropFactor(JOB_PROP_CRITICAL));
	nCritical = g_pPlayer->GetParam(DST_CHR_CHANCECRITICAL, nCritical);
#ifdef __JEFF_11
	if (nCritical < 0)
		nCritical = 0;
#endif	// __JEFF_11

	if (g_pPlayer)
	{
		if (g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL))
		{
			if (g_Party.IsMember(g_pPlayer->m_idPlayer))
				nCritical += (g_Party.m_nSizeofMember / 2);
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}
	}
	return nCritical;
}

int CWndCharInfo::GetVirtualSpeedRun()
{
	int nSpeedRun;
	nSpeedRun = g_pPlayer->GetParam(DST_SPEED, 0);	// ???? ??? ???? ????
	return nSpeedRun;
}
int CWndCharInfo::GetVirtualCriticalRate()
{
	int nCriticalRate;
	nCriticalRate = g_pPlayer->GetParam(DST_CRITICAL_BONUS, 0);	// ???? ??? ???? ????
	return nCriticalRate;
}
int CWndCharInfo::GetVirtualSpellCastSpeed()
{
	int nSpellCastSpeed;
	nSpellCastSpeed = g_pPlayer->GetParam(DST_SPELL_RATE, 0);	// ???? ??? ???? ????
	return nSpellCastSpeed;
}

int CWndCharInfo::GetVirtualPvpDamage()
{
	int nPvpDamage;
	nPvpDamage = g_pPlayer->GetParam(DST_PVP_DMG, 0);	// ???? ??? ???? ????
#ifdef __JEFF_11
	if (nPvpDamage < 0)
		nPvpDamage = 0;
#endif	// __JEFF_11
	return nPvpDamage;
}

int CWndCharInfo::GetVirtualHitRate()
{
	int nHitRate;
	nHitRate = g_pPlayer->GetParam(DST_ADJ_HITRATE, 0);	// ???? ??? ???? ????
	return nHitRate;
}

int CWndCharInfo::GetVirtualBlockMelee()
{
	int nBlockMelee;
	nBlockMelee = g_pPlayer->GetParam(DST_BLOCK_MELEE, 0);	// ???? ??? ???? ????
#ifdef __JEFF_11
	if (nBlockMelee < 0)
		nBlockMelee = 0;
	if (nBlockMelee > 96)
		nBlockMelee = 96;
#endif	// __JEFF_11
	return nBlockMelee;
}

int CWndCharInfo::GetVirtualBlockRange()
{
	int nBlockRange;
	nBlockRange = g_pPlayer->GetParam(DST_BLOCK_RANGE, 0);	// ???? ??? ???? ????
#ifdef __JEFF_11
	if (nBlockRange < 0)
		nBlockRange = 0;
	if (nBlockRange > 96)
		nBlockRange = 96;
#endif	// __JEFF_11
	return nBlockRange;
}

int CWndCharInfo::GetVirtualDodgeRate()
{
	int nDodgeRate;
	nDodgeRate = (int)((g_pPlayer->GetDex() * 0.5));
	nDodgeRate = g_pPlayer->GetParam(DST_PARRY, 0);	// ???? ??? ???? ????
#ifdef __JEFF_11
	if (nDodgeRate < 0)
		nDodgeRate = 0;
#endif	// __JEFF_11
	return nDodgeRate;
}

float CWndCharInfo::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;

	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if (pWeaponProp)
		fItem = pWeaponProp->fAttackSpeed;

	JobProp* pProperty = prj.GetJobProp(g_pPlayer->GetJob());
	ASSERT(pProperty);

	int A = int(pProperty->fAttackSpeed + (fItem * (4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f)) - 3.0f);

	if (187.5f <= A)
		A = (int)(187.5f);

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;

	int nIndex = A / 10;
	nIndex = max(nIndex, 0);
	nIndex = min(nIndex, (MAX_ATTACK_SPEED_PLUSVALUE - 1));

	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f, 0.40f,
		0.48f, 0.56f, 0.64f, 0.72f, 0.80f,
		0.88f, 0.96f, 1.04f, 1.12f, 1.20f,
		1.30f, 1.38f, 1.50f
	};

	fSpeed = ((50.0f / (200.f - A)) / 2.0f) + fPlusValue[nIndex];

	float fDstParam = g_pPlayer->GetParam(DST_ATTACKSPEED, 0) / 1000.0f;
	fSpeed += fDstParam;

	if (0 < g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0))
		fSpeed = fSpeed + (fSpeed * g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0) / 100);

	if (fSpeed < 0.0f)
		fSpeed = 0.1f;
	if (fSpeed > 2.0f)
		fSpeed = 2.0f;

	return fSpeed;
}


void CWndCharInfo::RenderATK(C2DRender* p2DRender, int x, int y)
{
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax(&nMin, &nMax);

	if ((m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax))
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax(&nMin, &nMax);

	int nATK = (nMin + nMax) / 2;

	if (g_pPlayer->IsSMMode(SM_ATTACK_UP))
		nATK = (int)(nATK * 1.2f);

	if (0 < g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0))
		nATK = nATK + (nATK * g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0) / 100);

	nATK += g_pPlayer->GetWeaponPlusDamage(1, FALSE);
	nATK += g_pPlayer->GetParam(DST_ATKPOWER, 0);

#ifdef __JEFF_11
	if (nATK < 0)
		nATK = 0;
#endif	// __JEFF_11

	wsprintf(szText, "%d", nATK);

	p2DRender->TextOut(x, y, szText, dwColor);
}

CWndHonor::CWndHonor()
{
	m_nSelectedId = 0;
	m_vecTitle.clear();
}

CWndHonor::~CWndHonor()
{

}

void CWndHonor::OnDraw(C2DRender* p2DRender)
{
}

void CWndHonor::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	RefreshList();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndListBox->m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CWndHonor::RefreshList()
{
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndListBox->ResetContent();
	pWndButton1->EnableWindow(FALSE);

	m_vecTitle.clear();
	m_vecTitle = CTitleManager::Instance()->m_vecEarned;

	if (g_pPlayer)
		m_nSelectedId = g_pPlayer->m_nHonor;

	if (m_vecTitle.size() > 0)
	{
		vector<EarnedTitle>::iterator	iter;
		pWndListBox->AddString(prj.GetText(TID_GAME_NOT_SELECTED_TITLE));
		for (iter = m_vecTitle.begin(); iter != m_vecTitle.end(); ++iter)
		{
			pWndListBox->AddString(iter->strTitle.GetBuffer(0));
		}
	}
	else
		pWndListBox->AddString(prj.GetText(TID_GAME_NO_TITLE));
}


BOOL CWndHonor::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HONOR, 0, CPoint(0, 0), pWndParent);
}


BOOL CWndHonor::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{

	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}


BOOL CWndHonor::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
#if __VER >= 13 // __HONORABLE_TITLE
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	switch (nID)
	{
	case WIDC_LISTBOX1: // view ctrl
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pWndListBox->GetCurSel() == 0)
		{
			m_nSelectedId = -1;
			pWndButton1->EnableWindow(TRUE);
		}
		else
		{
			m_nSelectedId = m_vecTitle[pWndListBox->GetCurSel() - 1].nId;
			pWndButton1->EnableWindow(TRUE);
		}
		break;
	}
	case WIDC_BUTTON1:
		if (g_pPlayer)
		{
			pWndButton1->EnableWindow(FALSE);
			g_DPlay.SendReqHonorTitleChange(m_nSelectedId);
			break;
		}
	};
#endif	// __HONORABLE_TITLE
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


void CWndHonor::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType, cx, cy);
}


void CWndHonor::OnLButtonUp(UINT nFlags, CPoint point)
{


}


void CWndHonor::OnLButtonDown(UINT nFlags, CPoint point)
{


}
#endif // __RENEW_CHARINFO


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CWndCharacter::CWndCharacter()
{
}
CWndCharacter::~CWndCharacter()
{
}
void CWndCharacter::OnDraw(C2DRender* p2DRender)
{
}
void CWndCharacter::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	CWndTabCtrl* lpTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	if (ar.IsLoading())
	{
		int nCurSel;
		ar >> nCurSel;
#if __VER >= 13 // __RENEW_CHARINFO
		if (nCurSel > 1) nCurSel = 0;
#endif // __RENEW_CHARINFO
		lpTabCtrl->SetCurSel(nCurSel);
	}
	else
	{
		ar << lpTabCtrl->GetCurSel();
	}
}
void CWndCharacter::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 15;

	CWndTabCtrl* lpTapCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	m_wndCharInfo.Create(WBS_CHILD | WBS_NOFRAME, rect, lpTapCtrl, 100000);
	m_wndHonor.Create(WBS_CHILD | WBS_NOFRAME, rect, lpTapCtrl, APP_HONOR);
	m_wndHonor.AddWndStyle(WBS_NOFRAME);
	m_wndHonor.AddWndStyle(WBS_NODRAWFRAME);
	m_wndCharInfo.AddWndStyle(WBS_NOFRAME);
	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_CHAR_INFO);
	tabTabItem.pWndBase = &m_wndCharInfo;
	lpTapCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_TITLE_HONOR);
	tabTabItem.pWndBase = &m_wndHonor;
	lpTapCtrl->InsertItem(1, &tabTabItem);
	lpTapCtrl->SetCurSel(0);
}
BOOL CWndCharacter::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(0, 90, WND_WIDTH, 260);
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndCharacter::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndCharacter::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCharacter::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndCharacter::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacter::OnLButtonDown(UINT nFlags, CPoint point)
{
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CWndStateConfirm::CWndStateConfirm()
{
	m_nId = 0;
}
CWndStateConfirm::~CWndStateConfirm()
{
}
void CWndStateConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndStateConfirm::OnSetState(UINT nId)
{
	m_nId = nId;
}
void CWndStateConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	//	strTemp.Format( prj.GetText(TID_GAME_TOOLTIP_SHELLCOST)
	CString strMessage;

#if __VER >= 9 // __CSC_VER9_2
	strMessage.Format(prj.GetText(TID_GAME_CHARSTATUS_APPLY_Q));
#else //__CSC_VER9_2
	switch (m_nId)
	{
	case 100:	// str
		strMessage.Format(prj.GetText(TID_DIAG_0076));
		break;
	case 101:	// sta
		strMessage.Format(prj.GetText(TID_DIAG_0077));
		break;
	case 102:	// dex
		strMessage.Format(prj.GetText(TID_DIAG_0078));
		break;
	case 103:	// int
		strMessage.Format(prj.GetText(TID_DIAG_0079));
		break;
	default:
		break;
	}
#endif //__CSC_VER9_2

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);


	if (g_pPlayer->IsBaseJob() == TRUE)
	{

		CString strAddMessage;
		strAddMessage.Format(prj.GetText(TID_DIAG_0082));
		strMessage += '\n';
		strMessage += strAddMessage;
	}
	pWndText->SetString(strMessage);
	pWndText->EnableWindow(FALSE);


	MoveParentCenter();
}

BOOL CWndStateConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_STATE_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndStateConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndStateConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndStateConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndStateConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndStateConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		CWndCharacter* pWndBase = (CWndCharacter*)g_WndMng.GetWndBase(APP_CHARACTER3);
		if (pWndBase)
		{
			CWndCharInfo* pInfo = &pWndBase->m_wndCharInfo;
			if (pInfo)
			{
				g_DPlay.SendModifyStatus(pInfo->m_nStrCount, pInfo->m_nStaCount, pInfo->m_nDexCount, pInfo->m_nIntCount);
				pInfo->m_nStrCount = pInfo->m_nStaCount = pInfo->m_nDexCount = pInfo->m_nIntCount = 0;
				pInfo->RefreshStatPoint();
				Destroy();
			}
		}
	}
	else if (nID == WIDC_NO || nID == WTBID_CLOSE)
	{
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_CHARACTER3);
		if (pWndBase)
		{
			((CWndCharacter*)pWndBase)->m_wndCharInfo.m_fWaitingConfirm = FALSE;
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


CWndCharacterBase::CWndCharacterBase()
{
}
CWndCharacterBase::~CWndCharacterBase()
{
}
void CWndCharacterBase::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();
	rect.bottom -= 30;
	//	p2DRender->RenderFillRect( rect, 0xff00ffff );
	int y = 0, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
#if __VER >= 10 // __CSC_VER9_1
	BYTE checkhero = g_pPlayer->GetLegendChar();
	int xpos = 0;
	int ypos = 0;
	CTexture* pTexture;
	CString strPath;
	CPoint point;
#endif //__CSC_VER9_1
	y = 10;

	if (::GetLanguage() == LANG_JAP)
	{
		p2DRender->TextOut(80, y, g_pPlayer->GetName(), dwColor); y += nNext;
		p2DRender->TextOut(80, y, g_pPlayer->GetJobString(), dwColor); y += nNext;

		ypos = y;
		if (checkhero == LEGEND_CLASS_MASTER)
		{
			if (g_pPlayer->GetLevel() < 100)
				xpos = 97;
			else
				xpos = 103;
		}
		p2DRender->TextOut(80, y, g_pPlayer->GetLevel(), dwColor); y += nNext;


		y = 81;
	}
	else
	{
		p2DRender->TextOut(50, y, g_pPlayer->GetName(), dwColor); y += nNext;
		p2DRender->TextOut(50, y, g_pPlayer->GetJobString(), dwColor); y += nNext;
		ypos = y;
		if (checkhero == LEGEND_CLASS_MASTER)
		{
			if (g_pPlayer->GetLevel() < 100)
				xpos = 67;
			else
				xpos = 73;
		}
		p2DRender->TextOut(50, y, g_pPlayer->GetLevel(), dwColor); y += nNext;

		y = 81;
	}

	point.x = xpos;
	point.y = ypos - 2;
	if (checkhero == LEGEND_CLASS_MASTER)
	{
		if (/*g_pPlayer->m_nLevel >= 60 && */g_pPlayer->m_nLevel < 70)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark1.dds");
		else if (g_pPlayer->m_nLevel >= 70 && g_pPlayer->m_nLevel < 80)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark2.dds");
		else if (g_pPlayer->m_nLevel >= 80 && g_pPlayer->m_nLevel < 90)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark3.dds");
		else if (g_pPlayer->m_nLevel >= 90 && g_pPlayer->m_nLevel < 100)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark4.dds");
		else if (g_pPlayer->m_nLevel >= 100 && g_pPlayer->m_nLevel < 110)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark5.dds");
		else if (g_pPlayer->m_nLevel >= 110 && g_pPlayer->m_nLevel <= 120)
			strPath = MakePath(DIR_ICON, "Icon_MasterMark6.dds");

		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, point);

	}
	else if (checkhero == LEGEND_CLASS_HERO || checkhero == LEGEND_CLASS_LEGENDHERO)
	{
		strPath = MakePath(DIR_ICON, "icon_Hero.dds");
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, point);
	}

	y = 55;
	CString strServerName;
	strServerName.Format("%s", g_dpCertified.GetServerName(g_Option.m_nSer));
	if (::GetLanguage() == LANG_JAP)
	{
		p2DRender->TextOut(80, y, strServerName, dwColor);
		y += nNext;
	}
	else
	{
		p2DRender->TextOut(50, y, strServerName, dwColor);
		y += nNext;
	}


	LPSERVER_DESC pServerDesc = NULL;
	int nCount = 0;
	for (int j = 0; j < (int)(g_dpCertified.m_dwSizeofServerset); j++)
	{
		if (g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if (nCount++ == g_Option.m_nSer)
				pServerDesc = g_dpCertified.m_aServerset + j;
		}
		if (g_dpCertified.m_aServerset[j].dwParent != NULL_ID && g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if (pServerDesc != NULL && g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
			{
				strServerName.Format("%s", g_dpCertified.m_aServerset[j + g_Option.m_nMSer].lpName);
				if (::GetLanguage() == LANG_JAP)
					p2DRender->TextOut(80, y, strServerName, dwColor);
				else
					p2DRender->TextOut(50, y, strServerName, dwColor);

				j = g_dpCertified.m_dwSizeofServerset;
			}
		}
	}

	y = 96;

	p2DRender->TextOut(96, y, g_pPlayer->GetFlightLv(), dwColor); y += nNext;
	{
		char szBuff[64];
		int nMaxFxp = prj.m_aFxpCharacter[g_pPlayer->GetFlightLv() + 1].dwFxp;
		sprintf(szBuff, "%5.2f%%", (float)g_pPlayer->GetFxp() * 100.0f / (float)nMaxFxp);
		p2DRender->TextOut(90, y, szBuff, dwColor); y += nNext;
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_01), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_APP_CHARACTER_JOB), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_02), dwColor); y += nNext;

	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVER), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHAR_SERVERNAME), dwColor); y += nNext;

	y += 10;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_04), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_05), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_06), dwColor);
}
void CWndCharacterBase::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("WndCharacter2_1.tga")), TRUE);
	FitTextureSize();

	MakeVertexBuffer();
}

BOOL CWndCharacterBase::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(240, 0, 240 + 330, 255 - 135);
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);
}
BOOL CWndCharacterBase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndCharacterBase::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCharacterBase::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndCharacterBase::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCharacterBase::OnLButtonDown(UINT nFlags, CPoint point)
{
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CWndCharacterDetail::CWndCharacterDetail()
{
	if (!g_WndMng.m_pWndStateConfirm)
		m_fWaitingConfirm = FALSE;

	m_nCnt = 0;
}
CWndCharacterDetail::~CWndCharacterDetail()
{
}

void CWndCharacterDetail::RenderATK(C2DRender* p2DRender, int x, int y)
{
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	char szText[32];
	int nMin, nMax;
	g_pPlayer->GetHitMinMax(&nMin, &nMax);
	int nATK = (nMin + nMax) / 2;

	if (g_pPlayer->IsSMMode(SM_ATTACK_UP))
		nATK = (int)(nATK * 1.2f);

	if (0 < g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0))
		nATK = nATK + (nATK * g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0) / 100);

	nATK += g_pPlayer->GetWeaponPlusDamage(1, FALSE);

	//#ifdef __PET_0410
	nATK += g_pPlayer->GetParam(DST_ATKPOWER, 0);
	//#endif	// __PET_0410

#ifdef __JEFF_11
	if (nATK < 0)
		nATK = 0;
#endif	// __JEFF_11

	wsprintf(szText, "%d", nATK);

	p2DRender->TextOut(x, y, szText, dwColor);
}

void CWndCharacterDetail::OnDraw(C2DRender* p2DRender)
{
	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;
	RenderATK(p2DRender, x, y);
	y += nNext;

	p2DRender->TextOut(x, y, g_pPlayer->GetShowDefense(FALSE), dwColor); y += nNext;
	x = 100; y = 5;

	x = 130; y = 10;
	CString strMsg;
	strMsg.Format("%d%%", g_pPlayer->GetCriticalProb());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	float fAttackSpeed = g_pPlayer->GetAttackSpeed();
	strMsg.Format("%d%%", int(fAttackSpeed * 100.0f) / 2);
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

#ifdef _KWCSC_UPDATE
	x = 50;
	int fHitRate = g_pPlayer->GetAdjHitRate();
	strMsg.Format("%d%%", fHitRate);
	p2DRender->TextOut(x, y, strMsg, dwColor);
#endif
	x = 15; y = 40; nNextX = 60;

#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	if (g_pPlayer->m_nStr == g_pPlayer->GetStr())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nStr < g_pPlayer->GetStr())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(65, y, g_pPlayer->GetStr(), dwColor); y += nNext;

	if (g_pPlayer->m_nSta == g_pPlayer->GetSta())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nSta < g_pPlayer->GetSta())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(65, y, g_pPlayer->GetSta(), dwColor); y += nNext;

	if (g_pPlayer->m_nDex == g_pPlayer->GetDex())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nDex < g_pPlayer->GetDex())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(65, y, g_pPlayer->GetDex(), dwColor); y += nNext;

	if (g_pPlayer->m_nInt == g_pPlayer->GetInt())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nInt < g_pPlayer->GetInt())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(65, y, g_pPlayer->GetInt(), dwColor); y += nNext;

	if (m_fWaitingConfirm == FALSE)
	{
		m_wndStr.EnableWindow(((int)g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
		m_wndSta.EnableWindow(((int)g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
		m_wndDex.EnableWindow(((int)g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
		m_wndInt.EnableWindow(((int)g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
	}
	else
	{
		m_wndStr.EnableWindow(FALSE);
		m_wndSta.EnableWindow(FALSE);
		m_wndDex.EnableWindow(FALSE);
		m_wndInt.EnableWindow(FALSE);
	}
#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
#ifdef __S_ADD_RESTATE
	p2DRender->TextOut(135, y, 1, dwColor); y += nNext;
	p2DRender->TextOut(135, y, 1, dwColor); y += nNext;
	p2DRender->TextOut(135, y, 1, dwColor); y += nNext;
	p2DRender->TextOut(135, y, 1, dwColor); y += nNext;
#else // __S_ADD_RESTATE
	p2DRender->TextOut(135, y, pJobAbillity->dwStr, dwColor); y += nNext;
	p2DRender->TextOut(135, y, pJobAbillity->dwSta, dwColor); y += nNext;
	p2DRender->TextOut(135, y, pJobAbillity->dwDex, dwColor); y += nNext;
	p2DRender->TextOut(135, y, pJobAbillity->dwInt, dwColor); y += nNext;
#endif // __S_ADD_RESTATE






	if (g_pPlayer->GetRemainGP())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
			m_nCnt = 0;
		}
	}

	p2DRender->TextOut(105, y, g_pPlayer->GetRemainGP(), dwColor); y += nNext;

	CRect rectHittest[5];
#ifdef _KWCSC_UPDATE
	rectHittest[0].SetRect(10, 67, 160, 65);
	rectHittest[1].SetRect(10, 82, 160, 80);
	rectHittest[2].SetRect(10, 97, 160, 95);
	rectHittest[3].SetRect(10, 112, 160, 110);
	rectHittest[4].SetRect(10, 127, 160, 125);
#else
	rectHittest[0].SetRect(10, 52, 160, 65);
	rectHittest[1].SetRect(10, 67, 160, 80);
	rectHittest[2].SetRect(10, 82, 160, 95);
	rectHittest[3].SetRect(10, 97, 160, 110);
	rectHittest[4].SetRect(10, 112, 160, 125);
#endif
	CRect rectTemp;
	CPoint ptTemp;

	CPoint ptMouse = GetMousePoint();
	for (int iC = 0; iC < 5; ++iC)
	{
		if (rectHittest[iC].PtInRect(ptMouse))
		{
			ClientToScreen(&ptMouse);
			ClientToScreen(&rectHittest[iC]);
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB(0, 93, 0);
			DWORD dwColorCommand = D3DCOLOR_XRGB(180, 0, 0);
			if (iC == 0)	// STR
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_STR));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetStr());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR0));
				strEdit.AddString("(");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR1), dwColorCommand);
				strEdit.AddString(")");
			}
			else if (iC == 1) // STA
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_STA));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetSta());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA1));
			}
			else if (iC == 2) // DEX
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_DEX));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetDex());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX1));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX2));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX3), dwColorCommand);
			}
			else if (iC == 3) // INT
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_INT));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetInt());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT1));
			}
			else	// GP
			{
				szString.Format("GP  %d\n ", g_pPlayer->GetRemainGP());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_GP0));
			}

			g_toolTip.PutToolTip(100, strEdit, rectHittest[iC], ptMouse, 3);
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor);
	p2DRender->TextOut(75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor); y += nNext;

	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor);
	p2DRender->TextOut(75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor); y += nNext;
#ifdef _KWCSC_UPDATE
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_HITRATE), dwColor); y += nNext;
#endif
	y += 12;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STR), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STA), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_INT), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor); y += nNext;
}
void CWndCharacterDetail::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("wndCharacter2.tga")), TRUE);
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);



#ifdef _KWCSC_UPDATE
	y = 67;
#else
	y = 52;
#endif
	m_wndStr.Create("<", 0, CRect(100, y, 145 + 20, y + 16), this, 100); y += 14;
	m_wndSta.Create("<", 0, CRect(100, y, 145 + 20, y + 16), this, 101); y += 14;
	m_wndDex.Create("<", 0, CRect(100, y, 145 + 20, y + 16), this, 102); y += 14;
	m_wndInt.Create("<", 0, CRect(100, y, 145 + 20, y + 16), this, 103); y += 14;

	m_wndStr.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndSta.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndDex.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndInt.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);

#ifdef __S_ADD_RESTATE
	m_wndStr.EnableWindow((g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
	m_wndSta.EnableWindow((g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
	m_wndDex.EnableWindow((g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
	m_wndInt.EnableWindow((g_pPlayer->GetRemainGP() > 0) ? TRUE : FALSE);
#else // __S_ADD_RESTATE
	LPJOBABILITY pJobAbillity;
	if (g_pPlayer->IsExpert())
		pJobAbillity = &prj.m_aJobAbillity[g_pPlayer->GetJob()];
	else
		pJobAbillity = &prj.m_aJobAbillity[JOB_VAGRANT];
	m_wndStr.EnableWindow((g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr) ? TRUE : FALSE);
	m_wndSta.EnableWindow((g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta) ? TRUE : FALSE);
	m_wndDex.EnableWindow((g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex) ? TRUE : FALSE);
	m_wndInt.EnableWindow((g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt) ? TRUE : FALSE);
#endif // __S_ADD_RESTATE
}
BOOL CWndCharacterDetail::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(240, 0, 240 + 330, 255 - 135);
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);
}
BOOL CWndCharacterDetail::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (m_fWaitingConfirm == FALSE)
		{
			m_fWaitingConfirm = TRUE;
			SAFE_DELETE(g_WndMng.m_pWndStateConfirm);
			g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
			g_WndMng.m_pWndStateConfirm->OnSetState(nID);
			g_WndMng.m_pWndStateConfirm->Initialize();
		}
	}
	return CWndBase::OnChildNotify(message, nID, pLResult);
}
BOOL CWndCharacterDetail::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCharacterDetail::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndCharacterDetail::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}
void CWndCharacterDetail::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}
void CWndCharacterDetail::OnMouseWndSurface(CPoint point)
{

	/*	CRect rectHittest( 10, 52, 100, 64 );
		if( rectHittest.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &rectHittest );
			CString szEditString;
			szEditString.Format( "%d, %d", point.x, point.y );
			g_toolTip.PutToolTip( 100, szEditString, rectHittest, point2 );
			g_WndMng.PutString( szEditString );
	//		g_toolTip.PutToolTip( (DWORD)this, m_strTooltip, rectHittest, point2, 0 );
		}
	*/
}

////////////////////////////////////////////////////////////////////
// CWndCharacterDetail2 Class
////////////////////////////////////////////////////////////////////

#if __VER >= 9 // __CSC_VER9_2

CWndCharacterDetail2::CWndCharacterDetail2()
{
	m_nStrCount = 0;
	m_nStaCount = 0;
	m_nDexCount = 0;
	m_nIntCount = 0;
	m_nGpPoint = 0;

	m_nATK = 0;
	m_nDEF = 0;
	m_nCritical = 0;
	m_nATKSpeed = 0;
}

CWndCharacterDetail2::~CWndCharacterDetail2()
{
}

void CWndCharacterDetail2::RenderATK(C2DRender* p2DRender, int x, int y)
{
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	char szText[32];
	int nMin, nMax;
	int nTemp1, nTemp2;

	GetVirtualATK(&nTemp1, &nTemp2);
	g_pPlayer->GetHitMinMax(&nMin, &nMax);

	if ((m_nStrCount != 0 || m_nDexCount != 0 || m_nIntCount != 0) && (nTemp1 != nMin || nTemp2 != nMax))
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}

		GetVirtualATK(&nMin, &nMax);
	}
	else
		g_pPlayer->GetHitMinMax(&nMin, &nMax);

	int nATK = (nMin + nMax) / 2;

	if (g_pPlayer->IsSMMode(SM_ATTACK_UP))
		nATK = (int)(nATK * 1.2f);

	if (0 < g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0))
		nATK = nATK + (nATK * g_pPlayer->GetParam(DST_ATKPOWER_RATE, 0) / 100);

	nATK += g_pPlayer->GetWeaponPlusDamage(1, FALSE);
	nATK += g_pPlayer->GetParam(DST_ATKPOWER, 0);

#ifdef __JEFF_11
	if (nATK < 0)
		nATK = 0;
#endif	// __JEFF_11

	wsprintf(szText, "%d", nATK);

	p2DRender->TextOut(x, y, szText, dwColor);
}

void CWndCharacterDetail2::OnDraw(C2DRender* p2DRender)
{
	if (g_pPlayer == NULL)
		return;

	int y = 5, nNext = 15;
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

	y = 15;
	int x = 5, nNextX = 100;

	x = 50; y = 10;

	RenderATK(p2DRender, x, y);
	y += nNext;


	if (m_nStaCount != 0 && GetVirtualDEF() != g_pPlayer->GetShowDefense(FALSE))
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		p2DRender->TextOut(x, y, GetVirtualDEF(), dwColor); y += nNext;
	}
	else
		p2DRender->TextOut(x, y, g_pPlayer->GetShowDefense(FALSE), dwColor); y += nNext;

	x = 130; y = 10;


	CString strMsg;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	if (m_nDexCount != 0 && GetVirtualCritical() != g_pPlayer->GetCriticalProb())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		strMsg.Format("%d%%", GetVirtualCritical());
	}
	else
		strMsg.Format("%d%%", g_pPlayer->GetCriticalProb());
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;


	float fAttackSpeed;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	if (m_nDexCount != 0 && GetVirtualATKSpeed() != g_pPlayer->GetAttackSpeed())
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
		fAttackSpeed = GetVirtualATKSpeed();
	}
	else
		fAttackSpeed = g_pPlayer->GetAttackSpeed();

	strMsg.Format("%d%%", int(fAttackSpeed * 100.0f) / 2);
	p2DRender->TextOut(x, y, strMsg, dwColor); y += nNext;

	x = 15; y = 40; nNextX = 60;

	y = 52;
	int StatYPos = 50;

	if (g_pPlayer->m_nStr == g_pPlayer->GetStr())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nStr < g_pPlayer->GetStr())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetStr(), dwColor); y += nNext;

	if (g_pPlayer->m_nSta == g_pPlayer->GetSta())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nSta < g_pPlayer->GetSta())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetSta(), dwColor); y += nNext;

	if (g_pPlayer->m_nDex == g_pPlayer->GetDex())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nDex < g_pPlayer->GetDex())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetDex(), dwColor); y += nNext;

	if (g_pPlayer->m_nInt == g_pPlayer->GetInt())
		dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	else
		if (g_pPlayer->m_nInt < g_pPlayer->GetInt())
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
		else
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	p2DRender->TextOut(StatYPos, y, g_pPlayer->GetInt(), dwColor); y += nNext;
	/*
		LPJOBABILITY pJobAbillity;
		if( g_pPlayer->IsExpert() )
			pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
		else
			pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

		y = 52;

		p2DRender->TextOut( 135, y, pJobAbillity->dwStr, dwColor ); y += nNext;
		p2DRender->TextOut( 135, y, pJobAbillity->dwSta, dwColor ); y += nNext;
		p2DRender->TextOut( 135, y, pJobAbillity->dwDex, dwColor ); y += nNext;
		p2DRender->TextOut( 135, y, pJobAbillity->dwInt, dwColor ); y += nNext;
	*/
	if (m_nGpPoint)
	{
		if ((g_nRenderCnt / 8) & 1)
		{
			dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		}
	}

	p2DRender->TextOut(105, y, m_nGpPoint, dwColor); y += nNext;

	CRect rectHittest[5];

	rectHittest[0].SetRect(10, 52, 80, 65);
	rectHittest[1].SetRect(10, 67, 80, 80);
	rectHittest[2].SetRect(10, 82, 80, 95);
	rectHittest[3].SetRect(10, 97, 80, 110);
	rectHittest[4].SetRect(10, 112, 160, 125);

	CRect rectTemp;
	CPoint ptTemp;

	CPoint ptMouse = GetMousePoint();
	for (int iC = 0; iC < 5; ++iC)
	{
		if (rectHittest[iC].PtInRect(ptMouse))
		{
			ClientToScreen(&ptMouse);
			ClientToScreen(&rectHittest[iC]);
			CEditString strEdit;
			CString szString;
			DWORD dwColorName = D3DCOLOR_XRGB(0, 93, 0);
			DWORD dwColorCommand = D3DCOLOR_XRGB(180, 0, 0);
			if (iC == 0)	// STR
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_STR));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetStr());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR0));
				strEdit.AddString("(");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STR1), dwColorCommand);
				strEdit.AddString(")");
			}
			else if (iC == 1) // STA
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_STA));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetSta());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_STA1));
			}
			else if (iC == 2) // DEX
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_DEX));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetDex());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX1));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX2));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_DEX3), dwColorCommand);
			}
			else if (iC == 3) // INT
			{
				szString.Format("%s", prj.GetText(TID_TOOLTIP_INT));
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				szString.Format("  %d\n ", g_pPlayer->GetInt());
				strEdit.AddString(szString, dwColorName, ESSTY_BOLD);
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT0));
				strEdit.AddString("\n ");
				strEdit.AddString(prj.GetText(TID_TOOLTIP_COMMAND_INT1));
			}
			else // GP
				strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_GPPOINT));

			g_toolTip.PutToolTip(100, strEdit, rectHittest[iC], ptMouse, 3);
			break;
		}
	}

	y = 10;
	dwColor = D3DCOLOR_ARGB(255, 0, 0, 180);
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_13), dwColor);
	p2DRender->TextOut(75, y, prj.GetText(TID_GAME_CHARACTER_14), dwColor); y += nNext;

	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEFENCE), dwColor);
	p2DRender->TextOut(75, y, prj.GetText(TID_GAME_CHARACTER_15), dwColor); y += nNext;
	y += 12;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STR), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_STA), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_DEX), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_TOOLTIP_INT), dwColor); y += nNext;
	p2DRender->TextOut(7, y, prj.GetText(TID_GAME_CHARACTER_07), dwColor); y += nNext;


	CRect rect;
	rect.SetRect(7, 10, 160, 38);
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		CEditString strEdit;
		if (m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_STATUS1));
		else
			strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_STATUS2));
		g_toolTip.PutToolTip(100, strEdit, rect, ptMouse, 3);
	}
	//Edit Tooltip
	rect.SetRect(90, 52, 160, 110);
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		CEditString strEdit;
		strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_EDIT));
		g_toolTip.PutToolTip(100, strEdit, rect, ptMouse, 3);
	}
	//Button Tooltip
	rect = m_wndApply.m_rectWindow;
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		CEditString strEdit;
		strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_APPLY));
		g_toolTip.PutToolTip(100, strEdit, rect, ptMouse, 3);
	}
	rect = m_wndReset.m_rectWindow;
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		CEditString strEdit;
		strEdit.AddString(prj.GetText(TID_TOOLTIP_CHARSTATUS_RESET));
		g_toolTip.PutToolTip(100, strEdit, rect, ptMouse, 3);
	}
}

void CWndCharacterDetail2::OnInitialUpdate()
{
	CWndBase::OnInitialUpdate();

	SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("WndCharacter2_2.tga")), TRUE);
	FitTextureSize();

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);


	int posY = 49;
	int posX = 128;

	m_editStrCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 100);
	m_wndStrPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 101);
	m_wndStrMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 102); posY += 15;

	m_editStaCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 103);
	m_wndStaPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 104);
	m_wndStaMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 105); posY += 15;

	m_editDexCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 106);
	m_wndDexPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 107);
	m_wndDexMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 108); posY += 15;

	m_editIntCount.Create(g_Neuz.GetSafeHwnd(), 0, CRect(posX - 38, posY, posX - 4, posY + 16), this, 109);
	m_wndIntPlus.Create("<", 0, CRect(posX, posY + 2, posX + 14, posY + 18), this, 110);
	m_wndIntMinus.Create("<", 0, CRect(posX + 16, posY + 2, posX + 30, posY + 18), this, 111);

	m_wndStrPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStrMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndStaPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndStaMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndDexPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndDexMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);
	m_wndIntPlus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharPlus.bmp")), TRUE);
	m_wndIntMinus.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharMinus.bmp")), TRUE);

	posY += 36;
	m_wndApply.Create("Apply", 0, CRect(posX - 108, posY, posX - 58, posY + 22), this, 112);
	m_wndReset.Create("Reset", 0, CRect(posX - 30, posY, posX + 20, posY + 22), this, 113);

	if (::GetLanguage() == LANG_FRE)
	{
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateOk.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButStateCancel.tga")), TRUE);
	}
	else
	{
		m_wndApply.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharApply.tga")), TRUE);
		m_wndReset.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("ButtCharReset.tga")), TRUE);
	}


	/*
	LPJOBABILITY pJobAbillity;
	if( g_pPlayer->IsExpert() )
		pJobAbillity = &prj.m_aJobAbillity[ g_pPlayer->GetJob() ];
	else
		pJobAbillity = &prj.m_aJobAbillity[ JOB_VAGRANT ];

	m_wndStr.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwStr ) ? TRUE : FALSE );
	m_wndSta.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwSta ) ? TRUE : FALSE );
	m_wndDex.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwDex ) ? TRUE : FALSE );
	m_wndInt.EnableWindow( ( g_pPlayer->GetRemainGP() >= pJobAbillity->dwInt ) ? TRUE : FALSE );
	*/

	m_nGpPoint = g_pPlayer->GetRemainGP();

	m_wndStrMinus.EnableWindow(FALSE);
	m_wndStaMinus.EnableWindow(FALSE);
	m_wndDexMinus.EnableWindow(FALSE);
	m_wndIntMinus.EnableWindow(FALSE);

	if (g_pPlayer->GetRemainGP() <= 0)
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);
	}

	RefreshStatPoint();
}

BOOL CWndCharacterDetail2::Process()
{
	//Control Button Plus or Minus
	if (m_nStrCount > 0)
		m_wndStrMinus.EnableWindow(TRUE);
	else
		m_wndStrMinus.EnableWindow(FALSE);

	if (m_nStaCount > 0)
		m_wndStaMinus.EnableWindow(TRUE);
	else
		m_wndStaMinus.EnableWindow(FALSE);

	if (m_nDexCount > 0)
		m_wndDexMinus.EnableWindow(TRUE);
	else
		m_wndDexMinus.EnableWindow(FALSE);

	if (m_nIntCount > 0)
		m_wndIntMinus.EnableWindow(TRUE);
	else
		m_wndIntMinus.EnableWindow(FALSE);

	m_nGpPoint = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount);

	if (m_nGpPoint > 0)
	{
		m_wndStrPlus.EnableWindow(TRUE);
		m_wndStaPlus.EnableWindow(TRUE);
		m_wndDexPlus.EnableWindow(TRUE);
		m_wndIntPlus.EnableWindow(TRUE);

		m_editStrCount.EnableWindow(TRUE);
		m_editStaCount.EnableWindow(TRUE);
		m_editDexCount.EnableWindow(TRUE);
		m_editIntCount.EnableWindow(TRUE);
	}
	else
	{
		m_wndStrPlus.EnableWindow(FALSE);
		m_wndStaPlus.EnableWindow(FALSE);
		m_wndDexPlus.EnableWindow(FALSE);
		m_wndIntPlus.EnableWindow(FALSE);

		m_editStrCount.EnableWindow(FALSE);
		m_editStaCount.EnableWindow(FALSE);
		m_editDexCount.EnableWindow(FALSE);
		m_editIntCount.EnableWindow(FALSE);
	}

	if (m_nStrCount > 0 || m_nStaCount > 0 || m_nDexCount > 0 || m_nIntCount > 0)
	{
		m_wndApply.EnableWindow(TRUE);
		m_wndReset.EnableWindow(TRUE);
	}
	else
	{
		m_wndApply.EnableWindow(FALSE);
		m_wndReset.EnableWindow(FALSE);
	}

	RefreshStatPoint();
	return TRUE;
}

void CWndCharacterDetail2::RefreshStatPoint()
{
	CString tempstr;
	tempstr.Format("%d", m_nStrCount);
	m_editStrCount.SetString(tempstr);
	tempstr.Format("%d", m_nStaCount);
	m_editStaCount.SetString(tempstr);
	tempstr.Format("%d", m_nDexCount);
	m_editDexCount.SetString(tempstr);
	tempstr.Format("%d", m_nIntCount);
	m_editIntCount.SetString(tempstr);
}

void CWndCharacterDetail2::GetVirtualATK(int* pnMin, int* pnMax)
{
	int nParts = PARTS_RWEAPON;

	*pnMin = 0;
	*pnMax = 0;

	if (g_pPlayer)
	{
		ItemProp* pItemProp = g_pPlayer->GetActiveHandItemProp(nParts);
		if (pItemProp == NULL)
			return;

		*pnMin = pItemProp->dwAbilityMin * 2;
		*pnMax = pItemProp->dwAbilityMax * 2;

		*pnMin = g_pPlayer->GetParam(DST_ABILITY_MIN, *pnMin);
		*pnMax = g_pPlayer->GetParam(DST_ABILITY_MAX, *pnMax);

		int nATK = 0;
		switch (pItemProp->dwWeaponType)
		{
		case WT_MELEE_SWD:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_SWD)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_AXE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_AXE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MELEE_STAFF:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STAFF)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_MELEE_STICK:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_STICK)) + (float(g_pPlayer->GetLevel() * 1.3f)));
			break;
		case WT_MELEE_KNUCKLE:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_KNUCKLE)) + (float(g_pPlayer->GetLevel() * 1.2f)));
			break;
		case WT_MAGIC_WAND:
			nATK = (int)((g_pPlayer->GetInt() + m_nIntCount - 10) * g_pPlayer->GetJobPropFactor(JOB_PROP_WAND) + g_pPlayer->GetLevel() * 1.2f);
			break;
		case WT_MELEE_YOYO:
			nATK = (int)(float((g_pPlayer->GetStr() + m_nStrCount - 12) * g_pPlayer->GetJobPropFactor(JOB_PROP_YOYO)) + (float(g_pPlayer->GetLevel() * 1.1f)));
			break;
		case WT_RANGE_BOW:
			nATK = (int)((((g_pPlayer->GetDex() + m_nDexCount - 14) * 4.0f + (g_pPlayer->GetLevel() * 1.3f) + ((g_pPlayer->GetStr() + m_nStrCount) * 0.2f)) * 0.7f));
			break;
		}

		nATK += g_pPlayer->GetPlusWeaponATK(pItemProp->dwWeaponType);

		int nPlus = nATK + g_pPlayer->GetParam(DST_CHR_DMG, 0);

		*pnMin += nPlus;
		*pnMax += nPlus;

		CItemElem* pWeapon = g_pPlayer->GetWeaponItem(nParts);
		if (pWeapon && pWeapon->GetProp())
		{
			float f = g_pPlayer->GetItemMultiplier(pWeapon);
			*pnMin = (int)(*pnMin * f);
			*pnMax = (int)(*pnMax * f);
		}

		if (pWeapon)
		{
			int nOption = pWeapon->GetAbilityOption();
			if (nOption > 0)
			{
				int nValue = (int)pow((float)(nOption), 1.5f);

				*pnMin += nValue;
				*pnMax += nValue;
			}
		}
	}
}

int CWndCharacterDetail2::GetVirtualDEF()
{
	int nDefense = 0;

	ATTACK_INFO info;
	memset(&info, 0x00, sizeof(info));
	info.dwAtkFlags = AF_GENERIC;

	float fFactor = 1.0f;
	if (g_pPlayer)
	{
		JobProp* pProperty = prj.GetJobProp(g_pPlayer->GetJob());
		assert(pProperty);
		fFactor = pProperty->fFactorDef;
	}
	nDefense = (int)(((((g_pPlayer->GetLevel() * 2) + ((g_pPlayer->GetSta() + m_nStaCount) / 2)) / 2.8f) - 4) + ((g_pPlayer->GetSta() + m_nStaCount - 14) * fFactor));
	nDefense += (g_pPlayer->GetDefenseByItem(FALSE) / 4);
	nDefense += g_pPlayer->GetParam(DST_ADJDEF, 0);
	nDefense = (int)(nDefense * g_pPlayer->GetDEFMultiplier(&info));

#ifdef __JEFF_11
	if (nDefense < 0)
		nDefense = 0;
#endif	// __JEFF_11
	return nDefense;
}

int CWndCharacterDetail2::GetVirtualCritical()
{
	int nCritical;
	nCritical = ((g_pPlayer->GetDex() + m_nDexCount) / 10);
	nCritical = (int)(nCritical * g_pPlayer->GetJobPropFactor(JOB_PROP_CRITICAL));
	nCritical = g_pPlayer->GetParam(DST_CHR_CHANCECRITICAL, nCritical);
#ifdef __JEFF_11
	if (nCritical < 0)
		nCritical = 0;
#endif	// __JEFF_11

	if (g_pPlayer)
	{
		if (g_pPlayer->m_idparty && (g_pPlayer->m_dwFlag & MVRF_CRITICAL))
		{
			if (g_Party.IsMember(g_pPlayer->m_idPlayer))
				nCritical += (g_Party.m_nSizeofMember / 2);
			g_pPlayer->m_dwFlag &= (~MVRF_CRITICAL);
		}
	}
	return nCritical;
}

float CWndCharacterDetail2::GetVirtualATKSpeed()
{
	float fSpeed = 1.0f;
	float fItem = 1.0f;

	ItemProp* pWeaponProp = g_pPlayer->GetActiveHandItemProp();
	if (pWeaponProp)
		fItem = pWeaponProp->fAttackSpeed;

	JobProp* pProperty = prj.GetJobProp(g_pPlayer->GetJob());
	ASSERT(pProperty);



	int A = int(pProperty->fAttackSpeed + (fItem * (4.0f * (g_pPlayer->GetDex() + m_nDexCount) + g_pPlayer->GetLevel() / 8.0f)) - 3.0f);

	if (187.5f <= A)
		A = (int)(187.5f);

	const int MAX_ATTACK_SPEED_PLUSVALUE = 18;

	int nIndex = A / 10;
	nIndex = max(nIndex, 0);
	nIndex = min(nIndex, (MAX_ATTACK_SPEED_PLUSVALUE - 1));

	float fPlusValue[MAX_ATTACK_SPEED_PLUSVALUE] = {
		0.08f, 0.16f, 0.24f, 0.32f,	0.40f,
		0.48f, 0.56f, 0.64f, 0.72f,	0.80f,
		0.88f, 0.96f, 1.04f, 1.12f,	1.20f,
		1.30f, 1.38f, 1.50f
	};

	fSpeed = ((50.0f / (200.f - A)) / 2.0f) + fPlusValue[nIndex];

	float fDstParam = g_pPlayer->GetParam(DST_ATTACKSPEED, 0) / 1000.0f;
	fSpeed += fDstParam;

	if (0 < g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0))
		fSpeed = fSpeed + (fSpeed * g_pPlayer->GetParam(DST_ATTACKSPEED_RATE, 0) / 100);

	if (fSpeed < 0.0f)
		fSpeed = 0.1f;
	if (fSpeed > 2.0f)
		fSpeed = 2.0f;

	return fSpeed;
}

BOOL CWndCharacterDetail2::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(240, 0, 240 + 330, 255 - 135);
	//SetTitle( GETTEXT( TID_APP_CHARACTER )  );
	return CWndBase::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_EXTENSION, rect, pWndParent, dwWndId);
}

BOOL CWndCharacterDetail2::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int editnum = 0;

	if (nID == 100 || nID == 103 || nID == 106 || nID == 109)
	{
		CString tempnum = m_editStrCount.GetString();
		for (int i = 0; i < tempnum.GetLength(); i++)
		{
			if (isdigit2(tempnum.GetAt(i)) == FALSE)
				return FALSE;
		}
	}
	switch (nID)
	{
	case 100:
		editnum = atoi(m_editStrCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStaCount + m_nDexCount + m_nIntCount);
		m_nStrCount = editnum;
		break;
	case 103:
		editnum = atoi(m_editStaCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nDexCount + m_nIntCount);
		m_nStaCount = editnum;
		break;
	case 106:
		editnum = atoi(m_editDexCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nIntCount);
		m_nDexCount = editnum;
		break;
	case 109:
		editnum = atoi(m_editIntCount.GetString());
		if (editnum > g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount))
			editnum = g_pPlayer->GetRemainGP() - (m_nStrCount + m_nStaCount + m_nDexCount);
		m_nIntCount = editnum;
		break;
	}

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case 101: //Str Plus
			m_nStrCount++;
			break;
		case 102: //Str Minus
			m_nStrCount--;
			break;
		case 104: //Sta Plus
			m_nStaCount++;
			break;
		case 105: //Sta Minus
			m_nStaCount--;
			break;
		case 107: //Dex Plus
			m_nDexCount++;
			break;
		case 108: //Dex Minus
			m_nDexCount--;
			break;
		case 110: //Int Plus
			m_nIntCount++;
			break;
		case 111: //Int Minus
			m_nIntCount--;
			break;
		case 112: //Apply
			if (m_nStrCount != 0 || m_nStaCount != 0 || m_nDexCount != 0 || m_nIntCount != 0)
			{
				SAFE_DELETE(g_WndMng.m_pWndStateConfirm);
				g_WndMng.m_pWndStateConfirm = new CWndStateConfirm;
				g_WndMng.m_pWndStateConfirm->Initialize();
			}
			break;
		case 113: //Reset
			m_nStrCount = 0;
			m_nStaCount = 0;
			m_nDexCount = 0;
			m_nIntCount = 0;
			RefreshStatPoint();
			break;
		}
	}

	if (nID >= 100 && nID <= 111) //If use Full GpPoint
	{
		if (g_pPlayer->GetRemainGP() == (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(FALSE);
			m_wndStaPlus.EnableWindow(FALSE);
			m_wndDexPlus.EnableWindow(FALSE);
			m_wndIntPlus.EnableWindow(FALSE);
		}
		else if (g_pPlayer->GetRemainGP() > (m_nStrCount + m_nStaCount + m_nDexCount + m_nIntCount))
		{
			m_wndStrPlus.EnableWindow(TRUE);
			m_wndStaPlus.EnableWindow(TRUE);
			m_wndDexPlus.EnableWindow(TRUE);
			m_wndIntPlus.EnableWindow(TRUE);
		}
	}
	return CWndBase::OnChildNotify(message, nID, pLResult);
}

BOOL CWndCharacterDetail2::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}

void CWndCharacterDetail2::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CWndBase::OnSize(nType, cx, cy);
}

void CWndCharacterDetail2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

void CWndCharacterDetail2::OnMouseWndSurface(CPoint point)
{
}

#endif //__CSC_VER9_2

CWndTradeGold::CWndTradeGold()
{
	m_dwGold = 0;
	m_nIdWndTo = APP_INVENTORY;
	m_pWndBase = NULL;
	m_nSlot = 0;
	m_nPutSlot = 100;
}
CWndTradeGold::~CWndTradeGold()
{
}
BOOL CWndTradeGold::Process(void)
{
	CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_Shortcut.m_dwId);
	if (pItemBase == NULL)
		return TRUE;

	if (pItemBase->GetExtra() > 0)
	{
		Destroy();
	}
	return TRUE;
}
void CWndTradeGold::OnDraw(C2DRender* p2DRender)
{
}
void CWndTradeGold::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndOk->SetDefault(TRUE);
	pWndEdit->SetFocus();

	if (m_dwGold > INT_MAX)
		m_dwGold = INT_MAX;

	TCHAR szNumber[64];
	_itot(m_dwGold, szNumber, 10);
	pWndEdit->SetString(szNumber);
}

BOOL CWndTradeGold::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_TRADE_GOLD, WBS_MODAL, 0, pWndParent);
}
BOOL CWndTradeGold::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndTradeGold::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTradeGold::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTradeGold::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndTradeGold::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK || message == EN_RETURN)
	{
		CRect rect = GetClientRect();
		CWorld* pWorld = g_WorldMng.Get();
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);

		CString string = pWndEdit->GetString();

		if (strlen(pWndEdit->GetString()) <= 0)
			return FALSE;

		int nCost = _ttoi(string);
		if (nCost < 1)
			nCost = 1;
		if (nCost > INT_MAX)
			nCost = 1;

		if (m_nIdWndTo == APP_INVENTORY)
		{
			if (nCost > g_pPlayer->GetGold())
				nCost = g_pPlayer->GetGold();

			g_DPlay.SendDropGold(nCost, g_pPlayer->GetPos(), m_vPos);
		}
		else if (m_nIdWndTo == APP_TRADE)
		{
			if (m_Shortcut.m_dwData == 0)
			{
				if (nCost > g_pPlayer->GetGold())
					nCost = g_pPlayer->GetGold();

				if (nCost > 0)
					g_DPlay.SendTradePutGold(nCost);
			}
			else
			{
				CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_Shortcut.m_dwId);
				if (pItemBase)
				{
					if (nCost > pItemBase->m_nItemNum)
					{
						nCost = pItemBase->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendTradePut((BYTE)(m_Shortcut.m_dwData), m_Shortcut.m_dwId, nCost);
				}
			}
		}
		else if (m_nIdWndTo == APP_BANK)
		{
			if (m_Shortcut.m_dwData == 0)
			{
				if (nCost > g_pPlayer->GetGold())
					nCost = g_pPlayer->GetGold();

				g_DPlay.SendPutGoldBank(m_nSlot, nCost);
			}
			else
			{
				CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_Shortcut.m_dwId);
				if (pItemBase)
				{
					if (nCost > pItemBase->m_nItemNum)
					{
						nCost = pItemBase->m_nItemNum;
					}
					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemBank(m_nSlot, (BYTE)(m_Shortcut.m_dwId), nCost);
				}
			}
		}
#ifdef __FL_CONSUMABLES_STORAGE
		else if (m_nIdWndTo == APP_CHEST)
		{
			if (m_Shortcut.m_dwData != 0)
			{
				if (nCost > (int)((m_dwGold)))
					nCost = m_dwGold;
				m_Shortcut.m_dwData -= 100;

				if (m_nPutSlot == -1)
					g_DPlay.SendMoveItem_Chest(m_Shortcut.m_dwId, nCost, TRUE); // Inventory -> Chest
				else
					g_DPlay.SendMoveItem_Chest(m_Shortcut.m_dwId, nCost, FALSE); // Chest -> Inventory

			}
		}
#endif
		else if (m_nIdWndTo == APP_GUILD_BANK)
		{
			if (m_Shortcut.m_dwData != 0)
			{
				CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(m_Shortcut.m_dwId);
				if (pItemBase)
				{
					if (nCost > pItemBase->m_nItemNum)
					{
						nCost = pItemBase->m_nItemNum;
					}

					m_Shortcut.m_dwData -= 100;
					g_DPlay.SendPutItemGuildBank((BYTE)(m_Shortcut.m_dwId), nCost, 1);

				}
			}
		}
		else if (m_nIdWndTo == APP_MESSENGER)
		{
			if (g_pPlayer->GetGuild())
			{
				if (m_Shortcut.m_dwData == 0)
				{
					if ((DWORD)nCost > (g_pPlayer->GetGuild()->m_nGoldGuild))
						nCost = g_pPlayer->GetGuild()->m_nGoldGuild;

					if (nCost > 0)
						g_DPlay.SendGetItemGuildBank((BYTE)(m_Shortcut.m_dwId), nCost, 0);
				}
				else
				{
					CItemElem* pItemBase = NULL;
					if (m_Shortcut.m_dwType == ITYPE_ITEM)
						pItemBase = g_pPlayer->GetGuild()->m_GuildBank.GetAtId(m_Shortcut.m_dwId);

					if (pItemBase)
					{
						if (nCost > pItemBase->m_nItemNum)
						{
							nCost = pItemBase->m_nItemNum;
						}

						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemGuildBank((BYTE)(m_Shortcut.m_dwId), nCost, 1);
					}
				}
			}
		}
		else if (m_nIdWndTo == APP_GOLD)
		{
			if (m_Shortcut.m_dwData == 0)
			{
				if (nCost > (int)(g_pPlayer->m_dwGoldBank[m_nSlot]))
					nCost = g_pPlayer->m_dwGoldBank[m_nSlot];

				if (nCost > 0)
				{
					g_DPlay.SendGetGoldBank(m_nSlot, nCost);
				}
				else
				{
					// error message
				}
			}
			else
			{
				CItemElem* pItemBase = NULL;
				if (m_Shortcut.m_dwType == ITYPE_ITEM)
				{
					pItemBase = g_pPlayer->m_Bank[m_nSlot].GetAtId(m_Shortcut.m_dwId);

					if (pItemBase)
					{
						if (nCost > pItemBase->m_nItemNum)
						{
							nCost = pItemBase->m_nItemNum;
						}

						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendGetItemBank(m_nSlot, (BYTE)(m_Shortcut.m_dwId), nCost);
					}
				}
				else
				{
					//					assert(0);
				}
			}
		}
		else if (m_nIdWndTo == APP_COMMON_BANK)
		{
			if (m_Shortcut.m_dwData == 0)
			{
				if (nCost > (int)(g_pPlayer->m_dwGoldBank[m_nPutSlot]))
					nCost = g_pPlayer->m_dwGoldBank[m_nPutSlot];

				if (nCost > 0)
				{
					g_DPlay.SendPutGoldBankToBank(m_nPutSlot, m_nSlot, nCost);
				}
				else
				{
					// error message
				}
			}
			else
			{
				CItemElem* pItemBase = NULL;
				if (m_Shortcut.m_dwType == ITYPE_ITEM)
				{
					pItemBase = g_pPlayer->m_Bank[m_nPutSlot].GetAtId(m_Shortcut.m_dwId);
					if (pItemBase)
					{
						if (nCost > pItemBase->m_nItemNum)
						{
							nCost = pItemBase->m_nItemNum;
						}

						m_Shortcut.m_dwData -= 100;
						g_DPlay.SendPutItemBankToBank(m_nPutSlot, m_nSlot, (BYTE)(m_Shortcut.m_dwId), nCost);
					}
				}
				else
				{
					//					assert(0);
				}
			}
		}
		else
			if (m_nIdWndTo == APP_POST_SEND)
			{
				if (m_Shortcut.m_dwData == 0)
				{
					if (nCost > g_pPlayer->GetGold())
						nCost = g_pPlayer->GetGold();

					if (nCost > 0)
					{

						if (g_WndMng.m_pWndPost)
						{
							g_WndMng.m_pWndPost->m_PostTabSend.SetCost(nCost);
						}
					}
					else
					{
						// error message
					}
				}
				else
				{
					CItemElem* pItemBase = NULL;
					if (m_Shortcut.m_dwType == ITYPE_ITEM)
					{
						pItemBase = g_pPlayer->m_Inventory.GetAtId(m_Shortcut.m_dwId);
						if (pItemBase)
						{
							if (nCost > pItemBase->m_nItemNum)
							{
								nCost = pItemBase->m_nItemNum;
							}

							pItemBase->SetExtra(nCost);

							m_Shortcut.m_dwData -= 100;

							if (g_WndMng.m_pWndPost)
							{
								g_WndMng.m_pWndPost->m_PostTabSend.SetItemId((BYTE)(m_Shortcut.m_dwId));
								g_WndMng.m_pWndPost->m_PostTabSend.SetCount(nCost);
							}
						}
					}
					else
					{
						//					assert(0);
					}
				}
			}
		Destroy();
	}
	if (nID == WIDC_CANCEL)
	{
		if (m_nIdWndTo == APP_POST_SEND)
		{
			g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
			g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
		}
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmTrade::CWndConfirmTrade()
{
	m_objid = 0;
}
CWndConfirmTrade::~CWndConfirmTrade()
{
}
void CWndConfirmTrade::OnSetName(const char* szName, OBJID objid)
{
	m_objid = objid;
	CWndStatic* m_pName = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CString sName;
	sName = szName;
	sName.Format(prj.GetText(TID_GAME_FROM), szName);
	m_pName->SetTitle(sName);
}
void CWndConfirmTrade::OnDraw(C2DRender* p2DRender)
{
}
void CWndConfirmTrade::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);
}

BOOL CWndConfirmTrade::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	m_objid = 0;

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CONFIRM_TRADE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndConfirmTrade::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndConfirmTrade::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndConfirmTrade::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndConfirmTrade::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndConfirmTrade::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		CMover* pTrader = prj.GetMover(m_objid);
		if (pTrader != NULL)
		{
			g_DPlay.SendTrade(pTrader);
		}
		Destroy();
	}
	else
		if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		{
			g_DPlay.SendConfirmTradeCancel(m_objid);
			Destroy();
		}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CWndTradeConfirm::CWndTradeConfirm()
{
}
CWndTradeConfirm::~CWndTradeConfirm()
{
}
void CWndTradeConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndTradeConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pWndButtonOk = (CWndButton*)GetDlgItem(WIDC_YES);
	pWndButtonOk->SetVisible(TRUE);
	CWndButton* pWndButtonNO = (CWndButton*)GetDlgItem(WIDC_NO);
	pWndButtonNO->SetVisible(TRUE);

	bMsg = FALSE;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	pWndStatic->SetTitle(prj.GetText(TID_DIAG_0083));


	MoveParentCenter();
}

BOOL CWndTradeConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TRADE_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndTradeConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndTradeConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTradeConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTradeConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndTradeConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (bMsg)
	{
		return(TRUE);
	}

	if (nID == WIDC_NO || nID == WTBID_CLOSE)
	{
		g_DPlay.SendTradeCancel();
	}
	else if (nID == WIDC_YES)
	{
		g_DPlay.SendTradeConfirm();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CWndTrade::CWndTrade()
{
	SetPutRegInfo(FALSE);
}
CWndTrade::~CWndTrade()
{
	g_DPlay.SendTradeCancel();
	SAFE_DELETE(g_WndMng.m_pWndTradeGold);
}

void CWndTrade::OnDraw(C2DRender* p2DRender)
{
	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover = g_pPlayer->m_vtInfo.GetOther();
	if (pMover && pMover->GetType() == OT_MOVER)
	{
		CWndStatic* pWndNameYou = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
		CWndStatic* pWndNameI = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		CWndStatic* pWndGoldYou = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
		CWndStatic* pWndGoldI = (CWndStatic*)GetDlgItem(WIDC_STATIC4);

		pWndNameYou->SetTitle(pMover->GetName(TRUE));
		pWndNameI->SetTitle(g_pPlayer->GetName());

		CString string;
		int nLength;
		string.Format("%d", m_nGoldYou);

		nLength = string.GetLength();
		while (nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldYou->SetTitle(string);

		string.Format("%d", m_nGoldI);
		nLength = string.GetLength();
		while (nLength - 3 > 0)
		{
			nLength -= 3;
			string.Insert(nLength, ',');
		}
		pWndGoldI->SetTitle(string);

	}
	else
	{
		g_pPlayer->m_vtInfo.TradeClear();
	}
}
void CWndTrade::OnInitialUpdate()
{
	m_wndItemCtrlYou.Create(WLVS_ICON | WBS_NODRAWFRAME, CRect(5, 25 + 15, 5 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15), this, 10001);
	m_wndItemCtrlI.Create(WLVS_ICON | WBS_NODRAWFRAME, CRect(175, 25 + 15, 175 + 32 * 5 + 5, 25 + 32 * 5 + 5 + 15), this, 10002);

	m_nGoldI = 0;
	m_nGoldYou = 0;

	CWorld* pWorld = g_WorldMng.Get();
	CMover* pMover = g_pPlayer->m_vtInfo.GetOther();

	if (pMover)
	{
		pMover->m_vtInfo.TradeClear();
		pMover->m_vtInfo.SetOther(g_pPlayer);
		m_wndItemCtrlYou.InitItem(pMover);
		m_wndItemCtrlI.InitItem(g_pPlayer);
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndNeuz::OnInitialUpdate();

#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif

	if (g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
	{
		Destroy();
		return;
	}
}


BOOL CWndTrade::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TRADE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndTrade::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndTradeConfirm* pWndTradeConfirm = (CWndTradeConfirm*)g_WndMng.GetWndBase(APP_TRADE_CONFIRM);
	if (pWndTradeConfirm)
	{
		return(TRUE);
	}
	if (message == WIN_ITEMDROP && (nID == 10001 || nID == 10002))
	{
		BOOL TradeGold = FALSE;
		SAFE_DELETE(g_WndMng.m_pWndTradeGold);
		g_WndMng.m_pWndTradeGold = new CWndTradeGold;
		memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
		if (g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwData == 0)
		{
			TradeGold = TRUE;
			g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
		}
		else
		{
			CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(g_WndMng.m_pWndTradeGold->m_Shortcut.m_dwId);
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemBase->m_nItemNum;
		}
		g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_TRADE;
		g_WndMng.m_pWndTradeGold->m_pWndBase = this;

		if (TradeGold)
		{
			if (m_nGoldI == 0)
			{
				g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
				g_WndMng.m_pWndTradeGold->MoveParentCenter();
				CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
				CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
				g_WndMng.m_pWndTradeGold->SetTitle("Penya");
				CString strMain = prj.GetText(TID_GAME_MOVEPENYA);
				CString strCount = prj.GetText(TID_GAME_PENYACOUNT);//" SEED : ";
				pStatic->m_strTitle = strMain;
				pStaticCount->m_strTitle = strCount;
			}
			else
			{
				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
			}
		}
		else
		{
			g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
			g_WndMng.m_pWndTradeGold->MoveParentCenter();
			CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
			CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
			g_WndMng.m_pWndTradeGold->SetTitle("Item");
			CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
		}
	}
	else
	{
		if (nID == WIDC_OK_I)
		{
			g_DPlay.SendTradeOk();
		}
		else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		{
			g_DPlay.SendTradeCancel();
			return(TRUE);
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndTrade::DoCancel()
{
	CMover* pTrader = g_pPlayer->m_vtInfo.GetOther();
	if (pTrader)
		pTrader->m_vtInfo.TradeClear();
	g_pPlayer->m_vtInfo.TradeClear();

	Destroy();
}

BOOL CWndTrade::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndTrade::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTrade::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}
void CWndTrade::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

#define OBJFILTER_PLAYER   0
#define OBJFILTER_PARTY    1
#define OBJFILTER_NPC      2
#define OBJFILTER_MONSTER  3

CWndStatus::CWndStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;

	m_pVBHPGauge = NULL;
	m_pVBMPGauge = NULL;
	m_pVBFPGauge = NULL;
	m_pVBEXPGauge = NULL;

	m_bVBHPGauge = TRUE;
	m_bVBMPGauge = TRUE;
	m_bVBFPGauge = TRUE;

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;

	nWidthOldHp = 0;
	nWidthOldMp = 0;
	nWidthOldFp = 0;
	nWidthOldExp = 0;
}

CWndStatus::~CWndStatus()
{
	DeleteDeviceObjects();
}
void CWndStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpMP = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpFP = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpExp = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpPxp = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM6);
	CMover* pMover = CMover::GetActiveMover();

	if (pMover)
	{
		CRect rect = GetClientRect();
		CRect rectTemp;

		//	pMover->m_nHitPoint = 50;
		int nWidthClient = lpHP->rect.Width();
		//100 : rect = hp : x
		int nWidth;

		// HP
		nWidth = (int)((__int64)nWidthClient * (__int64)pMover->GetHitPoint() / (__int64)pMover->GetMaxHitPoint());
		if (nWidth > nWidthClient)
			nWidth = nWidthClient;

		if (nWidthOldHp > nWidthClient || nWidthOldHp < 0)
			nWidthOldHp = nWidthOldHp > nWidthClient ? nWidthClient : 0;

		if (nWidthOldHp < nWidth)
		{
			int nWidthMinusOld = nWidth - nWidthOldHp;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldHp + (nWidth - nWidthOldHp > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOldHp);
			nWidthOldHp = nWidth;
		}
		else if (nWidthOldHp > nWidth)
		{
			int nWidthMinusOld = nWidthOldHp - nWidth;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldHp - (nWidthOldHp - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOldHp - nWidth);
			nWidthOldHp = nWidth;
		}

		if (m_nHPWidth != nWidth)
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;//.SetRect( 105, 6, 105 + nWidthClient, 6 + 12 );
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal);
		}

		nWidth = (int)((__int64)nWidthClient * (__int64)pMover->GetManaPoint() / (__int64)pMover->GetMaxManaPoint());
		if (nWidth > nWidthClient)
			nWidth = nWidthClient;

		if (nWidthOldMp > nWidthClient || nWidthOldMp < 0)
			nWidthOldMp = nWidthOldMp > nWidthClient ? nWidthClient : 0;

		if (nWidthOldMp < nWidth)
		{
			int nWidthMinusOld = nWidth - nWidthOldMp;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldMp + (nWidth - nWidthOldMp > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOldMp);
			nWidthOldMp = nWidth;
		}
		else if (nWidthOldMp > nWidth)
		{
			int nWidthMinusOld = nWidthOldMp - nWidth;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldMp - (nWidthOldMp - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOldMp - nWidth);
			nWidthOldMp = nWidth;
		}

		if (m_nMPWidth != nWidth)
		{
			m_nMPWidth = nWidth;
			rect = lpMP->rect;
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBMPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x640000ff, m_pVBMPGauge, &m_texGauFillNormal);
		}
		// FP
		nWidth = (int)((__int64)nWidthClient * (__int64)pMover->GetFatiguePoint() / (__int64)pMover->GetMaxFatiguePoint());
		if (nWidth > nWidthClient)
			nWidth = nWidthClient;

		if (nWidthOldFp > nWidthClient || nWidthOldFp < 0)
			nWidthOldFp = nWidthOldFp > nWidthClient ? nWidthClient : 0;

		if (nWidthOldFp < nWidth)
		{
			int nWidthMinusOld = nWidth - nWidthOldFp;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldFp + (nWidth - nWidthOldFp > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOldFp);
			nWidthOldFp = nWidth;
		}
		else if (nWidthOldFp > nWidth)
		{
			int nWidthMinusOld = nWidthOldFp - nWidth;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldFp - (nWidthOldFp - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOldFp - nWidth);
			nWidthOldFp = nWidth;
		}

		if (m_nFPWidth != nWidth)
		{
			m_nFPWidth = nWidth;
			rect = lpFP->rect;//rect.SetRect( 105, 32, 105 + nWidthClient, 32 + 12 );
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBFPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x6400ff00, m_pVBFPGauge, &m_texGauFillNormal);
		}

		nWidth = (int)(((float)nWidthClient / pMover->GetMaxExp1()) * pMover->GetExp1());
		if (nWidth > nWidthClient)
			nWidth = nWidthClient;

		if (nWidthOldExp > nWidthClient || nWidthOldExp < 0)
			nWidthOldExp = nWidthOldExp > nWidthClient ? nWidthClient : 0;

		if (nWidthOldExp < nWidth)
		{
			int nWidthMinusOld = nWidth - nWidthOldExp;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldExp + (nWidth - nWidthOldExp > nWidthMinusOld ? nWidthMinusOld : nWidth - nWidthOldExp);
			nWidthOldExp = nWidth;
		}
		else if (nWidthOldExp > nWidth)
		{
			int nWidthMinusOld = nWidthOldExp - nWidth;
			nWidthMinusOld = nWidthMinusOld / 6;
			if (nWidthMinusOld <= 1)
			{
				nWidthMinusOld = 1;
			}

			nWidth = nWidthOldExp - (nWidthOldExp - nWidth > nWidthMinusOld ? nWidthMinusOld : nWidthOldExp - nWidth);
			nWidthOldExp = nWidth;
		}
		if (m_nEXPWidth != nWidth)
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;//rect.SetRect( 105, 48, 105 + nWidthClient, 48 + 8 );
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x848c40ff, m_pVBEXPGauge, &m_texGauFillSmall);
		}
	}
}
void CWndStatus::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	if (m_pTexture && g_pPlayer)
	{
		RenderWnd();
		if (IsWndStyle(WBS_CAPTION))
		{
			int y = 4;
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
			p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

			char szNameLevel[128] = { 0, };
			sprintf(szNameLevel, prj.GetText(TID_GAME_WND_STATUS_PLAYER_INFORMATION), g_pPlayer->GetName(), g_pPlayer->GetLevel());
			SetTitle(szNameLevel);

			p2DRender->SetFont(pOldFont);
		}
	}
	else
	{
		m_pTheme->RenderWndBaseFrame(p2DRender, &rect);
		if (IsWndStyle(WBS_CAPTION))
		{

			rect.bottom = 21;
			{
				m_pTheme->RenderWndBaseTitleBar(p2DRender, &rect, m_strTitle, m_dwColor);
			}
		}
	}
}

void CWndStatus::OnDraw(C2DRender* p2DRender)
{
	CMover* pMover = CMover::GetActiveMover();
	if (!pMover)
		return;

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;

	CRect rectTemp;
	LPWNDCTRL lpHP = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpMP = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpFP = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpExp = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM6);

	if (pMover)
	{
		MakeGaugeVertex();

		if (m_bVBHPGauge)
		{
			m_bHPVisible = TRUE;
			if (m_bHPVisible)
				m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal);
		}
		if (m_bVBMPGauge)
			m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBMPGauge, &m_texGauFillNormal);
		if (m_bVBFPGauge)
			m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBFPGauge, &m_texGauFillNormal);
		else
			m_bExpVisible = TRUE;

		if (m_bVBEXPGauge)
			m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillSmall);
	}

	if (m_nDisplay != 0)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

		char cbufHp[16] = { 0, };
		char cbufMp[16] = { 0, };
		char cbufFp[16] = { 0, };
		char cbufExp[16] = { 0, };
		char cbufPxp[16] = { 0, };

		int nCharHP, nCharMP, nCharFP, nCharEXP; //, nCharPXP;
		CSize size = p2DRender->m_pFont->GetTextExtent("8");
		int nMaxHeight = p2DRender->m_pFont->GetMaxHeight();
		int nTopGap = 0;

		if (::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER)
			nTopGap = 2;
		else
		{
			if (nMaxHeight > 14)
				nTopGap = nMaxHeight - 14;
		}

		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontSomeText2);

		if (m_nDisplay == 1)
		{
			wsprintf(cbufHp, "%d%%", pMover->GetHitPointPercent());
			wsprintf(cbufMp, "%d%%", pMover->GetManaPointPercent());
			wsprintf(cbufFp, "%d%%", pMover->GetFatiguePointPercent());

			p2DRender->TextOut(lpHP->rect.right - 100, lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000);
			p2DRender->TextOut(lpMP->rect.right - 100, lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000);
			p2DRender->TextOut(lpFP->rect.right - 100, lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000);

			nCharHP = wsprintf(cbufHp, prj.GetText(TID_GAME_WND_STATUS_PERCENT_POINT), pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, prj.GetText(TID_GAME_WND_STATUS_PERCENT_POINT), pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, prj.GetText(TID_GAME_WND_STATUS_PERCENT_POINT), pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 17; // -40
			p2DRender->TextOut(x - (int)(nCharHP * 5.8f), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(nCharMP * 5.8f), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(nCharFP * 5.8f), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000);
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", pMover->GetHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetFatiguePoint());

			int x = lpHP->rect.right - 78;
			p2DRender->TextOut(x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000);

			nCharHP = wsprintf(cbufHp, "%d", pMover->GetMaxHitPoint());
			nCharMP = wsprintf(cbufMp, "%d", pMover->GetMaxManaPoint());
			nCharFP = wsprintf(cbufFp, "%d", pMover->GetMaxFatiguePoint());

			x = lpHP->rect.right - 26;
			p2DRender->TextOut(x - (int)(((float)nCharHP / 2.0f) * size.cx), lpHP->rect.top - nTopGap, cbufHp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(((float)nCharMP / 2.0f) * size.cx), lpMP->rect.top - nTopGap, cbufMp, dwColor, 0xff000000);
			p2DRender->TextOut(x - (int)(((float)nCharFP / 2.0f) * size.cx), lpFP->rect.top - nTopGap, cbufFp, dwColor, 0xff000000);

			LPWNDCTRL lpHP = GetWndCtrl(WIDC_CUSTOM1);
			int nWidthCustom = lpHP->rect.Width();
			int nGap = (int)(((nWidthCustom / 2.0f) + (size.cx / 2.0f)));

			p2DRender->TextOut(lpHP->rect.right - nGap, lpHP->rect.top - nTopGap, "/", dwColor, 0xff000000);
			p2DRender->TextOut(lpMP->rect.right - nGap, lpMP->rect.top - nTopGap, "/", dwColor, 0xff000000);
			p2DRender->TextOut(lpFP->rect.right - nGap, lpFP->rect.top - nTopGap, "/", dwColor, 0xff000000);
		}

		p2DRender->SetFont(pOldFont);

		EXPFLOAT	nExpResult = pMover->GetExp1() * (EXPFLOAT)10000 / pMover->GetMaxExp1();
		float fExp = (float)nExpResult / 100.0f;
		if (fExp >= 99.99f)
			nCharEXP = sprintf(cbufExp, "99.999%%");
		else if (pMover->IsMode(MODE_EXPUP_STOP))
			nCharEXP = sprintf(cbufExp, "Stopped");
		else
			nCharEXP = sprintf(cbufExp, "%.3f%%", fExp);

		CSize size2 = m_p2DRender->m_pFont->GetTextExtent(cbufExp);
		p2DRender->TextOut(lpExp->rect.right - size2.cx - 5, lpExp->rect.top - 1, cbufExp, dwColor, 0xff000000);
	}

	VIEWPORT tmp;
	tmp.m_fRot = -20.0f;
	tmp.vecMatrixScaling = D3DXVECTOR3(6.0f, 6.0f, 6.0f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -7.8f, 0.0f);

	g_WndMng.RenderObjectViewport(p2DRender, g_pBipedMesh, g_pPlayer, lpFace, &tmp, GetWndId());
}
HRESULT CWndStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (m_pVBHPGauge == NULL)
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL);
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBMPGauge, NULL);
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBFPGauge, NULL);
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL);
		m_nHPWidth = -1;
		m_nMPWidth = -1;
		m_nFPWidth = -1;
		m_nEXPWidth = -1;
		m_nPXPWidth = -1;
	}


	return S_OK;
}
HRESULT CWndStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBHPGauge);
	SAFE_RELEASE(m_pVBMPGauge);
	SAFE_RELEASE(m_pVBFPGauge);
	SAFE_RELEASE(m_pVBEXPGauge);

	return S_OK;//return S_OK;
}
HRESULT CWndStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauEmptySmall.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptySmall.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillSmall.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptySmall.bmp"), 0xffff00ff, TRUE);

	if (!g_pBipedMesh)
	{
		int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		g_pBipedMesh = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
		prj.m_modelMng.LoadMotion(g_pBipedMesh, OT_MOVER, nMover, 0);
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, g_pBipedMesh, &g_pPlayer->m_Inventory);
	}
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point(rectRoot.left, rectRoot.top);
	Move(point);

}

BOOL CWndStatus::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_STATUS1);
}

BOOL CWndStatus::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	switch (nID)
	{
	case 100: // wnd2
		break;
	case 101: // wnd1
		g_SoundMng.m_nSoundVolume = !g_SoundMng.m_nSoundVolume;
		break;
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndStatus::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_nHPWidth = -1;
	m_nMPWidth = -1;
	m_nFPWidth = -1;
	m_nEXPWidth = -1;
	m_nPXPWidth = -1;

	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndStatus::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (++m_nDisplay > 2)
		m_nDisplay = 0;
}
BOOL CWndStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd(p2DRender);
	//oint pt = m_rectClient.TopLeft() - m_rectWindow.TopLeft();
	CRect rect = GetClientRect();
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	//p2DRender->RenderFillRect( rect, D3DCOLOR_ARGB( 255, 70, 70, 170 ) );
	p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB(100, 0, 0, 0));

	return TRUE;
}

CWndEmotion::CWndEmotion()
{
	m_lpSelectTreeElem = NULL;
}
CWndEmotion::~CWndEmotion()
{
}
void CWndEmotion::OnDraw(C2DRender* p2DRender)
{
	CRect rect = GetClientRect();

	p2DRender->TextOut(5, rect.Height() - 50, _T("ї¬°б ґл»з"));
}
void CWndEmotion::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
}
BOOL CWndEmotion::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(50, 50, 300, 300);

	return CWndNeuz::Create(WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId);
}
BOOL CWndEmotion::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndEmotion::OnSize(UINT nType, int cx, int cy)
{
	m_wndViewCtrl.SetWndRect(CRect(5, 5, m_rectClient.Width() - 5, m_rectClient.Height() - 60));

	CRect rect = GetClientRect();
	rect.top = rect.bottom - 55;
	rect.bottom = rect.top + 20;
	rect.left = 70;
	rect.right -= 5;
	m_wndEdit.SetWndRect(rect);

	rect = GetClientRect();
	rect.top = rect.bottom - 30;
	rect.bottom = rect.top + 25;
	rect.left = rect.right - 80;
	rect.right -= 5;
	m_wndDefault.SetWndRect(rect);

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndEmotion::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndEmotion::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndEmotion::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndLogOut::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);

	Create(_T(prj.GetText(TID_DIAG_0068)), MB_OKCANCEL, rect, APP_MESSAGEBOX);//dwWndId );
	m_wndText.SetString(_T(prj.GetText(TID_DIAG_0069)));
	m_wndText.ResetString();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);

	return CWndMessageBox::Initialize(pWndParent, dwWndId);
}
BOOL CWndLogOut::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case IDCANCEL:
			Destroy();
			break;
		case IDOK:
			g_Neuz.ResetStaticValues();

			if (g_pPlayer->EndMotion())
				g_Neuz.m_timerQuit.Set(SEC(3));

			if (g_pPlayer->IsRegionAttr(RA_SAFETY) || g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER))
			{
				::PostMessage(g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0);
			}
			else
			{
				if (g_WndMng.m_pLogOutWaitting == NULL)
				{
					g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
					g_WndMng.m_pLogOutWaitting->Initialize(NULL);
					g_WndMng.m_pLogOutWaitting->SetIsLogOut(TRUE);
					SetVisible(FALSE);
				}
			}

			CString strTitle;
			strTitle.Format(__NEUZ_TITLE);
			SetWindowText(CWndBase::m_hWnd, strTitle);

			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndLogOutWaitting::CWndLogOutWaitting()
{
}
CWndLogOutWaitting::~CWndLogOutWaitting()
{
}

BOOL CWndLogOutWaitting::Initialize(CWndBase* pWndParent)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_MESSAGE_TEXT);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);
	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_MESSAGE_TEXT);
}


BOOL CWndLogOutWaitting::Process()
{
	DWORD dwCurrentTime = g_tmCurrent;
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	DWORD dwEndWaitTime = dwTime - dwCurrentTime;
	dwEndWaitTime /= 1000;

	CString str;
	str.Format(prj.GetText(TID_PK_TIME_END), dwEndWaitTime);

	if (dwEndWaitTime > 0)
	{
		pWndText->SetString(str);
	}

	if (dwCurrentTime > dwTime)
	{
		if (m_bIsLogOut)
		{
			::PostMessage(g_Neuz.GetSafeHwnd(), WM_LOGOUT, 0, 0);
		}
		else
		{
			::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0);
		}

		Destroy();
	}
	return TRUE;
}

BOOL CWndLogOutWaitting::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndLogOutWaitting::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_wndTitleBar.SetVisible(FALSE);

	m_bIsLogOut = FALSE;

	dwTime = g_tmCurrent + SEC(TIMEWAIT_CLOSE);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);


	g_DPlay.SendReqLeave();
}


BOOL CWndQuit::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);
	Create(_T(prj.GetText(TID_DIAG_0068)), MB_OKCANCEL, rect, APP_MESSAGEBOX);//dwWndId );
	m_wndText.SetString(_T(prj.GetText(TID_DIAG_0070)));
	m_wndText.ResetString();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);

	m_bFlag = FALSE;

	return CWndMessageBox::Initialize(pWndParent, dwWndId);
}
BOOL CWndQuit::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case IDCANCEL:
			Destroy();
			break;
		case IDOK:
			if (m_bFlag)
				return TRUE;

			m_bFlag = TRUE;

			if (g_pPlayer->EndMotion())
				g_Neuz.m_timerQuit.Set(SEC(3));

			if (g_pPlayer->IsRegionAttr(RA_SAFETY))
				::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0);
			else
			{
				if (g_WndMng.m_pLogOutWaitting == NULL)
				{
					g_WndMng.m_pLogOutWaitting = new CWndLogOutWaitting;
					g_WndMng.m_pLogOutWaitting->Initialize(NULL);
					g_WndMng.m_pLogOutWaitting->SetIsLogOut(FALSE);
					SetVisible(FALSE);
				}
			}

			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndRevival::CWndRevival()
{
	m_pLodeStar = NULL;
	m_pRevival = NULL;
}
CWndRevival::~CWndRevival()
{
}
void CWndRevival::OnDraw(C2DRender* p2DRender)
{
#ifdef __JEFF_11_4
	BOOL	bArena = g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena();
#endif	// __JEFF_11_4
	if (NULL == g_pPlayer->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_RESURRECTION)
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow(FALSE);
	else
		m_pRevival->EnableWindow(TRUE);
}
void CWndRevival::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pLodeStar = (CWndButton*)GetDlgItem(WIDC_REVIVAL_TO_LODESTAR);
	m_pRevival = (CWndButton*)GetDlgItem(WIDC_REVIVAL_STAND);
	m_pLodeStar->m_strToolTip = _T(prj.GetText(TID_GAME_TOOLTIP_LODESTAR));
	m_pRevival->m_strToolTip = _T(prj.GetText(TID_GAME_TOOLTIP_OGNPOINT));

#ifdef __JEFF_11_4
	BOOL	bArena = g_pPlayer->GetWorld() && g_pPlayer->GetWorld()->IsArena();
#endif	// __JEFF_11_4
	if (NULL == g_pPlayer->m_Inventory.GetAtItemId(II_SYS_SYS_SCR_RESURRECTION)
#ifdef __JEFF_11_4
		|| bArena
#endif	// __JEFF_11_4
		)
		m_pRevival->EnableWindow(FALSE);
	else
		m_pRevival->EnableWindow(TRUE);

	m_wndTitleBar.SetVisible(FALSE);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);
}
BOOL CWndRevival::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), APP_REVIVAL, WBS_MODAL);

	return TRUE;
}
BOOL CWndRevival::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRevival::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRevival::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRevival::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndRevival::EnableButton(int nButton, BOOL bEnable)
{
	switch (nButton)
	{
	case WIDC_REVIVAL_TO_LODESTAR:
	{
		m_pLodeStar->EnableWindow(bEnable);
		break;
	}
	}
}

BOOL CWndRevival::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_REVIVAL_TO_LODESTAR:
	{
		g_DPlay.SendHdr(PACKETTYPE_REVIVAL_TO_LODESTAR);

		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_RESURRECTION);
		if (pWndBase)
			g_DPlay.SendResurrectionCancel();

		Destroy();
		break;
	}
	case WIDC_REVIVAL_STAND:
	{
		g_DPlay.SendHdr(PACKETTYPE_REVIVAL);

		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_RESURRECTION);
		if (pWndBase)
			g_DPlay.SendResurrectionCancel();

		Destroy();
		break;
	}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndResurrectionConfirm::CWndResurrectionConfirm()
{
}
CWndResurrectionConfirm::~CWndResurrectionConfirm()
{
}
void CWndResurrectionConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndResurrectionConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_wndTitleBar.SetVisible(FALSE);


	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point((rectRoot.right - rectWindow.Width()) / 2, 70);
	Move(point);
}
BOOL CWndResurrectionConfirm::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), APP_RESURRECTION, WBS_MODAL);
	return TRUE;
}
BOOL CWndResurrectionConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndResurrectionConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndResurrectionConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndResurrectionConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndResurrectionConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case  WIDC_BUTTON1:
	{
		g_DPlay.SendResurrectionOK();
		Destroy();
	}
	break;
	case  10000:
	case  WIDC_BUTTON2:
	{
		g_DPlay.SendResurrectionCancel();
		Destroy();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}




CWndCommItemDlg::CWndCommItemDlg()
{
	m_pWndEdit = NULL;
	m_dwObjId = NULL_ID;
}
CWndCommItemDlg::~CWndCommItemDlg()
{
}
void CWndCommItemDlg::OnDraw(C2DRender* p2DRender)
{
}
void CWndCommItemDlg::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_COMMUSE);
	if (m_pWndEdit)
		m_pWndEdit->EnableWindow(FALSE);


	MoveParentCenter();
}
void CWndCommItemDlg::SetItem(DWORD dwDefindText, DWORD dwObjId)
{
	m_pWndEdit->AddString(prj.GetText(dwDefindText));
	m_dwObjId = dwObjId;
}

BOOL CWndCommItemDlg::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COMMITEM_DIALOG, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCommItemDlg::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCommItemDlg::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndCommItemDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndCommItemDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndCommItemDlg::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON_OK || message == EN_RETURN)
	{
		CItemElem* pItem = static_cast<CItemElem*>(g_pPlayer->m_Inventory.GetAtId(m_dwObjId));
		if (pItem)
		{
			if (pItem->m_dwItemId == II_SYS_SYS_SCR_PET_TAMER_MISTAKE)
				g_DPlay.SendPetTamerMistake(m_dwObjId);
			else
			{
				if (pItem->GetProp()->dwItemKind3 == IK3_TICKET)
					g_DPlay.SendDoUseItem(m_dwObjId, "0");
				else
					g_DPlay.SendDoUseItem(m_dwObjId);
			}
		}
		Destroy();
	}
	else if (nID == WIDC_BUTTON_CANCEL || nID == WTBID_CLOSE)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndInventory::RunUpgrade(CItemElem* pItem)
{
	if (m_bIsUpgradeMode)
	{
		m_bIsUpgradeMode = FALSE;

		if (pItem == NULL || m_pUpgradeMaterialItem == NULL)
		{
			g_WndMng.PutString(prj.GetText(TID_UPGRADE_CANCLE), NULL, prj.GetTextColor(TID_UPGRADE_CANCLE));
			BaseMouseCursor();
			return;
		}

		ItemProp* pItemProp = m_pUpgradeMaterialItem->GetProp();
		if (!pItemProp)
			return;

		if (pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_SOCKETCARD2)
		{
			if (!(pItem)->IsPierceAble(pItemProp->dwItemKind3))
			{
				g_WndMng.PutString(prj.GetText(TID_PIERCING_POSSIBLE_ITEM));
				BaseMouseCursor();
				return;
			}

			int nCount = 0;
			for (int j = 0; j < (pItem)->GetPiercingSize(); j++)
			{
				if ((pItem)->GetPiercingItem(j) != 0)
					nCount++;
			}


			if (nCount == (pItem)->GetPiercingSize())
			{
				g_WndMng.PutString(prj.GetText(TID_PIERCING_ERROR_NOPIERCING), NULL, prj.GetTextColor(TID_PIERCING_ERROR_NOPIERCING));
				BaseMouseCursor();
				return;
			}
		}
		else if (pItemProp->dwItemKind3 == IK3_ELECARD)
		{
			ItemProp* pProp = pItem->GetProp();
			if (!pProp)
				return;

			if (!pProp->IsEleRefineryAble())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_NOTEQUALITEM), NULL, prj.GetTextColor(TID_GAME_NOTEQUALITEM));
				BaseMouseCursor();
				return;
			}

			CItemElem* pItemElem = pItem;

			if (pItemElem->m_bItemResist != SAI79::NO_PROP && pItemElem->m_bItemResist != pItemProp->eItemType)
			{
				g_WndMng.PutString(prj.GetText(TID_UPGRADE_ERROR_TWOELEMENT), NULL, prj.GetTextColor(TID_UPGRADE_ERROR_TWOELEMENT));
				BaseMouseCursor();
				return;
			}

			DWORD dwWantedCard = 0;
			DWORD dwItemAO = pItemElem->m_nResistAbilityOption;

			switch (pItemProp->eItemType)
			{
			case SAI79::FIRE:
				dwWantedCard = II_GEN_MAT_ELE_FLAME;	break;
			case SAI79::WATER:
				dwWantedCard = II_GEN_MAT_ELE_RIVER;	break;
			case SAI79::ELECTRICITY:
				dwWantedCard = II_GEN_MAT_ELE_GENERATOR;	break;
			case SAI79::EARTH:
				dwWantedCard = II_GEN_MAT_ELE_DESERT;	break;
			case SAI79::WIND:
				dwWantedCard = II_GEN_MAT_ELE_CYCLON; break;
			default:
				dwWantedCard = 0;	break;
			}

			if (pItemProp->dwID != dwWantedCard)
			{
				g_WndMng.PutString(prj.GetText(TID_UPGRADE_ERROR_WRONGUPLEVEL), NULL, prj.GetTextColor(TID_UPGRADE_ERROR_WRONGUPLEVEL));
				BaseMouseCursor();
				return;
			}
		}
		else  if (pItemProp->dwItemKind3 == IK3_ENCHANT)
		{
			if (m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_ORICHALCUM02)
			{
				CWndSmeltJewel* pWndSmeltJewel = (CWndSmeltJewel*)g_WndMng.GetWndBase(APP_SMELT_JEWEL);
				if (pWndSmeltJewel != NULL)
				{
					if (pWndSmeltJewel->m_pItemElem && pWndSmeltJewel->m_pItemElem->m_dwObjId == pItem->m_dwObjId)
					{
						g_WndMng.PutString(prj.GetText(TID_GAME_NOTEQUALITEM), NULL, prj.GetTextColor(TID_GAME_NOTEQUALITEM));
						BaseMouseCursor();
						return;
					}
				}

				if (pItem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
				{
					g_WndMng.PutString(prj.GetText(TID_GAME_NOTEQUALITEM), NULL, prj.GetTextColor(TID_GAME_NOTEQUALITEM));
					BaseMouseCursor();
					return;
				}
			}
			else if (!CItemElem::IsDiceRefineryAble(pItem->GetProp()))
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_NOTEQUALITEM), NULL, prj.GetTextColor(TID_GAME_NOTEQUALITEM));
				BaseMouseCursor();
				return;
			}
		}
		else if (pItemProp->dwItemKind3 == IK3_RANDOM_SCROLL)
		{
			if (!(pItem->GetProp()->dwItemKind1 == IK1_WEAPON || pItem->GetProp()->dwItemKind2 == IK2_ARMOR || pItem->GetProp()->dwItemKind2 == IK2_ARMORETC))
			{
				BaseMouseCursor();
				g_WndMng.PutString(prj.GetText(TID_GAME_RANDOMSCROLL_ERROR), NULL, prj.GetTextColor(TID_GAME_RANDOMSCROLL_ERROR));
				return;
			}
		}
		else if (pItemProp->IsNeedTarget())
		{
			m_pUpgradeItem = pItem;
			m_dwEnchantWaitTime = g_tmCurrent + 100;
			return;
		}
		else if (pItemProp->dwItemKind3 == IK3_PIERDICE)
		{
			if (m_pUpgradeMaterialItem && m_pUpgradeMaterialItem->m_dwItemId == II_GEN_MAT_MOONSTONE)
			{
				if ((static_cast<CItemElem*>(pItem))->IsCollector(TRUE) || pItem->GetProp()->dwItemKind2 == IK2_JEWELRY)
				{
					m_pUpgradeItem = pItem;
					m_dwEnchantWaitTime = g_tmCurrent + 100;
					return;
				}
				g_WndMng.PutString(prj.GetText(TID_GAME_NOTEQUALITEM), NULL, prj.GetTextColor(TID_GAME_NOTEQUALITEM));
				BaseMouseCursor();
				return;
			}
		}

		m_pUpgradeItem = pItem;
		m_dwEnchantWaitTime = g_tmCurrent + 100;
	}
}

void CWndInventory::BaseMouseCursor()
{
	m_bIsUpgradeMode = FALSE;
	DeleteUpgradeSfx();
}

void CWndInventory::UpdateParts()
{
	if (m_pModel)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, g_pPlayer->m_aEquipInfo, m_pModel, &g_pPlayer->m_Inventory);
}


CWndReSkillControl::CWndReSkillControl()
{
}
CWndReSkillControl::~CWndReSkillControl()
{
}
void CWndReSkillControl::OnDraw(C2DRender* p2DRender)
{

}
void CWndReSkillControl::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(prj.GetText(TID_RESKILLPOINT_CONTROL1));
	m_wndTitleBar.SetVisible(FALSE);
	pWndText->ResetString();
}

BOOL CWndReSkillControl::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_RESKILL_CONTROL1, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndReSkillControl::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndReSkillControl::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndReSkillControl::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndReSkillControl::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndReSkillControl::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
		if (pSkillTree)
			pSkillTree->InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE);
	}
	else if (nID == WIDC_BUTTON2)
	{
		SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
		g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
		g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
	}
	else if (nID == 10000)
	{
		return TRUE;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndReSkillWarning::CWndReSkillWarning()
{
}
CWndReSkillWarning::~CWndReSkillWarning()
{
	Destroy();
}
void CWndReSkillWarning::InitItem(BOOL bParentDestroy)
{
	m_bParentDestroy = bParentDestroy;
}
void CWndReSkillWarning::OnDestroy()
{
	if (m_bParentDestroy)
	{
		CWndBase* pWndBase1 = (CWndBase*)g_WndMng.GetWndBase(APP_SKILL4);
		pWndBase1->Destroy();
	}
}
void CWndReSkillWarning::OnDraw(C2DRender* p2DRender)
{
}
void CWndReSkillWarning::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_CONTEXT);

	if (pWndEdit)
	{
		pWndEdit->SetString(_T(prj.GetText(TID_GAME_SKILLLEVEL_CHANGE)));
		pWndEdit->EnableWindow(FALSE);
	}

	m_bParentDestroy = FALSE;


	MoveParentCenter();
}
BOOL CWndReSkillWarning::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_QUEITMWARNING, WBS_MODAL, CPoint(0, 0), pWndParent);
}
BOOL CWndReSkillWarning::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndReSkillWarning::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndReSkillWarning::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndReSkillWarning::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndReSkillWarning::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BTN_YES || message == EN_RETURN)
	{
		CWndSkill_16* pSkillTree = (CWndSkill_16*)g_WndMng.GetWndBase(APP_SKILL4);
		if (pSkillTree)
			g_DPlay.SendDoUseSkillPoint(pSkillTree->GetSkill());

		Destroy();
	}
	else if (nID == WIDC_BTN_NO)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndPostItemWarning::CWndPostItemWarning()
{
	m_nMailIndex = -1;
}
CWndPostItemWarning::~CWndPostItemWarning()
{
}
void CWndPostItemWarning::OnDraw(C2DRender* p2DRender)
{
}
void CWndPostItemWarning::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	MoveParentCenter();
}

void CWndPostItemWarning::SetString(char* string)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_CONTEXT);

	if (pWndEdit)
	{
		pWndEdit->SetString(string);
		pWndEdit->EnableWindow(FALSE);
	}
}


BOOL CWndPostItemWarning::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_QUEITMWARNING);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);

	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_QUEITMWARNING);
}
BOOL CWndPostItemWarning::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPostItemWarning::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPostItemWarning::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPostItemWarning::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPostItemWarning::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BTN_YES || message == EN_RETURN)
	{
		g_DPlay.SendQueryGetMailItem(m_nMailIndex);
		Destroy();
	}
	else if (nID == WIDC_BTN_NO)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}






CWndPost::CWndPost()
{
#ifdef __MAIL_REQUESTING_BOX
	m_pWndMailRequestingBox = NULL;
#endif // __MAIL_REQUESTING_BOX
}
CWndPost::~CWndPost()
{
	SAFE_DELETE(m_pWndMailRequestingBox)
}
void CWndPost::OnDraw(C2DRender* p2DRender)
{
}
void CWndPost::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	EnableWindow(FALSE);


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

#ifdef __FL_CONSUMABLES_STORAGE
	if (GetWndBase(APP_CHEST))
		GetWndBase(APP_CHEST)->Destroy();
#endif
	m_PostTabSend.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND);
	m_PostTabReceive.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE);

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_APP_POST_SEND);
	tabTabItem.pWndBase = &m_PostTabSend;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_APP_POST_RECEIVE);
	tabTabItem.pWndBase = &m_PostTabReceive;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	pWndTabCtrl->SetCurSel(1);

	MoveParentCenter();


	g_DPlay.SendQueryMailBox();
}

void CWndPost::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndMailRequestingBox)
		SAFE_DELETE(m_pWndMailRequestingBox);
}

BOOL CWndPost::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPost::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPost::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);

#ifdef __MAIL_REQUESTING_BOX
	SAFE_DELETE(m_pWndMailRequestingBox);
	m_pWndMailRequestingBox = new CWndMailRequestingBox;
	m_pWndMailRequestingBox->Initialize(this);
#endif // __MAIL_REQUESTING_BOX
}
void CWndPost::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPost::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPost::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_SELCHANGE)
	{
		CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);

		if (pWndTabCtrl->GetCurSel() == 0)
		{
		}
		else
		{
#ifndef __MAIL_REQUESTING_BOX
			m_PostTabSend.ClearData();
#endif // __MAIL_REQUESTING_BOX
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#ifdef __MAIL_REQUESTING_BOX
void CWndPost::LoadLastMailBox(void)
{
	CString strFileName = _T("");
	if (g_pPlayer == NULL)
	{
		return;
	}
	strFileName.Format("%s_MailData.Temp", g_pPlayer->GetName());
	FILE* fp = fopen(strFileName, "rb");
	if (fp == NULL)
	{
		Error("LoadLastMailBox : fp == NULL");
		return;
	}

	while (feof(fp) == 0)
	{
		CMail* pMail = new CMail;
		fread(&(pMail->m_nGold), sizeof(DWORD), 1, fp);
		fread(&(pMail->m_byRead), sizeof(BYTE), 1, fp);
		fread(&(pMail->m_szTitle), sizeof(char), MAX_MAILTITLE, fp);
		CMailBox::GetInstance()->AddMail(pMail);
	}

	fclose(fp);
}

void CWndPost::SaveLastMailBox(void)
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	if (pMailBox == NULL)
	{
		return;
	}

	CMailBox& MailBox = *pMailBox;
	if (MailBox.empty() == true)
	{
		CString strFileName = _T("");
		if (g_pPlayer == NULL)
		{
			return;
		}
		strFileName.Format("%s_MailData.Temp", g_pPlayer->GetName());
		DeleteFile(strFileName);

		return;
	}

	CString strFileName = _T("");
	if (g_pPlayer == NULL)
	{
		return;
	}
	strFileName.Format("%s_MailData.Temp", g_pPlayer->GetName());
	FILE* fp = fopen(strFileName.GetBuffer(0), "wb");
	if (fp == NULL)
	{
		Error("SaveLastMailBox : fp == NULL");
		return;
	}

	for (size_t i = 0; i < MailBox.size(); ++i)
	{
		fwrite(&(MailBox[i]->m_nGold), sizeof(DWORD), 1, fp);
		fwrite(&(MailBox[i]->m_byRead), sizeof(BYTE), 1, fp);
		fwrite(&(MailBox[i]->m_szTitle), sizeof(char), MAX_MAILTITLE, fp);
	}

	fclose(fp);
}
#endif // __MAIL_REQUESTING_BOX


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox
{
public:
	CWndText	m_wndText;

	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];
	char		m_szText[MAX_MAILTEXT];
	void		SetValue(BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

BOOL CWndPostSendMessageBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CString str;
	str.Format(prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY);

	return CWndMessageBox::Initialize(str,
		pWndParent,
		MB_OKCANCEL);
}

void CWndPostSendMessageBox::SetValue(BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText)
{
	m_nItem = nItem;
	m_nItemNum = nItemNum;
	strcpy(m_szReceiver, lpszReceiver);
	strcpy(m_szTitle, lpszTitle);
	strcpy(m_szText, lpszText);
	m_nGold = nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == 1)
	{
		if (g_pPlayer)
		{
			g_DPlay.SendQueryPostMail(m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText);

			CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

			if (pWndPost)
			{
				pWndPost->m_PostTabSend.ClearData();
			}
		}

		Destroy();
	}
	else if (nID == 2)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndPostSend::CWndPostSend() :m_nItem(0xff), m_nCost(0), m_nCount(0)
{
	//	m_pWndPostSendConfirm = NULL;
}
CWndPostSend::~CWndPostSend()
{
	ClearData();
	//	SAFE_DELETE(m_pWndPostSendConfirm);
}

void CWndPostSend::ClearData()
{
	CItemElem* pItemElem;
	pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);

	if (pItemElem)
	{

		if (!g_pPlayer->m_vtInfo.IsTrading(pItemElem))
			pItemElem->SetExtra(0);
	}

	m_nItem = 0xff;
	m_nCost = 0;
	m_nCount = 0;
	/*
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );

	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
	*/
}

void CWndPostSend::SetReceive(char* pchar)
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pWndCombo->SetString(pchar);
}
void CWndPostSend::SetTitle(char* pchar)
{
	CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

	pWndEdit1->SetString(pchar);
}
void CWndPostSend::SetText(char* pchar)
{
	CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem(WIDC_EDIT3);

	pWndEdit1->SetString(pchar);
}
void CWndPostSend::OnDraw(C2DRender* p2DRender)
{
	if (m_nItem != 0xff)
	{
		CItemElem* pItemElem;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);

		if (pItemElem)
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl(WIDC_STATIC3);

			pItemElem->GetTexture()->Render(p2DRender, pCustom->rect.TopLeft(), 255);

			if (pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0)
			{
				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", pItemElem->GetExtra());
				CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
				p2DRender->TextOut(pCustom->rect.right - size.cx - 2, pCustom->rect.bottom - size.cy + 2, szTemp, 0xffeaebdd);
				p2DRender->SetFont(pOldFont);
			}

			//*
			CRect hitrect = pCustom->rect;
			CPoint point = GetMousePoint();
			if (hitrect.PtInRect(point))
			{
				CPoint point2 = point;
				ClientToScreen(&point2);
				ClientToScreen(&hitrect);

				g_WndMng.PutToolTip_Item(pItemElem, point2, &hitrect);
			}
			/**/
		}
	}
}
void CWndPostSend::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pNotice = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	pNotice->AddWndStyle(EBS_WANTRETURN);
	pNotice->AddWndStyle(EBS_AUTOVSCROLL);




	LPWNDCTRL pWndCtrl = GetWndCtrl(WIDC_EDIT3);
	if (pWndCtrl)
		pNotice->SetWndRect(pWndCtrl->rect);

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	CWndEdit* pWndEdit3 = (CWndEdit*)GetDlgItem(WIDC_EDIT3);

	pWndCombo->SetTabStop(TRUE);
	pWndEdit2->SetTabStop(TRUE);
	pWndEdit3->SetTabStop(TRUE);

	for (map<u_long, Friend>::iterator i = g_WndMng.m_RTMessenger.begin(); i != g_WndMng.m_RTMessenger.end(); ++i)
	{
		u_long idPlayer = i->first;
		Friend* pFriend = &i->second;
		//		if( pFriend )
		{
			pWndCombo->AddString(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
		}
	}

	if (::GetLanguage() != LANG_KOR)
	{
		CWndStatic* pStatic2 = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
		CRect rect = pStatic2->GetWndRect();
		rect.left -= 14;
		rect.right -= 14;
		pStatic2->SetWndRect(rect, TRUE);

		CWndEdit* pEdit4 = (CWndEdit*)GetDlgItem(WIDC_EDIT4);
		rect = pEdit4->GetWndRect();
		rect.left -= 14;
		rect.right += 10;
		pEdit4->SetWndRect(rect, TRUE);
	}

	pWndCombo->SetFocus();
}

BOOL CWndPostSend::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_POST_SEND, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPostSend::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPostSend::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPostSend::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPostSend::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndPostSend::SetItemId(BYTE nId)
{

	if (nId != m_nItem && m_nItem != 0xff)
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);
		if (pItemElem)
		{
			pItemElem->SetExtra(0);
		}
	}

	m_nItem = nId;
}

BOOL CWndPostSend::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!(pShortcut->m_dwShortcut == SHORTCUT_ITEM) || !(pWndFrame->GetWndId() == APP_INVENTORY))
		return FALSE;

	if (pShortcut->m_dwData == 0)
	{
		SetForbid(TRUE);
		return FALSE;
	}

	if (g_pPlayer->m_Inventory.IsEquip(pShortcut->m_dwId))
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT));
		SetForbid(TRUE);
		return FALSE;
	}

	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(pShortcut->m_dwIndex);
	if (!IsUsableItem(pItemElem))
		return FALSE;

	LPWNDCTRL pCustom = GetWndCtrl(WIDC_STATIC3);
	if (PtInRect(&(pCustom->rect), point))
	{
		if (pItemElem->m_nItemNum > 1)
		{
			SetItemId((BYTE)(pItemElem->m_dwObjId));

			SAFE_DELETE(g_WndMng.m_pWndTradeGold);
			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT));
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND;
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;

			g_WndMng.m_pWndTradeGold->Initialize(&g_WndMng, APP_TRADE_GOLD);
			g_WndMng.m_pWndTradeGold->AddWndStyle(WBS_MODAL);
			g_WndMng.m_pWndTradeGold->MoveParentCenter();

			CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
			CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
			CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
			g_WndMng.m_pWndTradeGold->SetTitle("");

			if (pItemElem->GetProp())
			{
				CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

				if (stricmp(pWndEdit2->GetString(), pItemElem->GetProp()->szName) != 0)
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId((BYTE)(pItemElem->m_dwObjId));
			pItemElem->SetExtra(1);

			if (pItemElem->GetProp())
			{
				CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

				if (stricmp(pWndEdit2->GetString(), pItemElem->GetProp()->szName) != 0)
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
	}

	return TRUE;
}

void CWndPostSend::OnRButtonUp(UINT nFlags, CPoint point)
{
	LPWNDCTRL pCustom = GetWndCtrl(WIDC_STATIC3);
	if (PtInRect(&(pCustom->rect), point))
	{
		CItemElem* pItemElem;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);

		if (pItemElem)
		{
			pItemElem->SetExtra(0);
		}

		m_nItem = 0xff;
	}
}

void CWndPostSend::AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat)
{
	CWndNeuz::AdditionalSkinTexture(pDest, sizeSurface, d3dFormat);
}

#define MAX_BYTE_POST		128

BOOL CWndPostSend::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_SEND:
	{
		CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
		CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
		CWndEdit* pWndEdit3 = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
		CWndEdit* pWndEdit4 = (CWndEdit*)GetDlgItem(WIDC_EDIT4);

		LPCTSTR szstr1 = pWndCombo->GetString();
		LPCTSTR szstr2 = pWndEdit2->GetString();
		LPCTSTR szstr3 = pWndEdit3->GetString();
		LPCTSTR szstr4 = pWndEdit4->GetString();


		if (strlen(szstr1) <= 0 || strlen(szstr2) <= 0)
		{
			return FALSE;
		}


		if (m_nItem == NULL_ID && strlen(szstr4) <= 0)
		{
			return FALSE;
		}


		int nlen = strlen(szstr4);
		BOOL bisdigit = TRUE;

		if (nlen > 0)
		{
			for (int i = 0; i < nlen; i++)
			{
				if ((isdigit2(szstr4[i]) == FALSE))
				{
					bisdigit = FALSE;
					break;
				}
			}
		}

		if (bisdigit == FALSE)
		{
			//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
			return FALSE;
		}

		__int64 liGold = _atoi64(szstr4);
		if (liGold < 0 || liGold > 1000000000)
		{
			g_WndMng.OpenMessageBoxUpper(prj.GetText(TID_GAME_LIMITPENYA));
			return FALSE;
		}

		if (strlen(szstr1) < 3 || strlen(szstr1) > 16)
		{

			g_WndMng.OpenMessageBoxUpper(prj.GetText(TID_DIAG_0057));
			return FALSE;
		}

		if (strlen(szstr3) > (MAX_MAILTEXT - 1))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT);
			g_WndMng.OpenMessageBoxUpper(str);
			return FALSE;
		}

		if (strlen(szstr2) > (MAX_MAILTITLE - 1))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE);
			g_WndMng.OpenMessageBoxUpper(str);
			return FALSE;
		}

		CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;

		if (pBox)
		{
			g_WndMng.OpenCustomBox("", pBox);

			CItemElem* pItemElem;
			pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);

			if (pItemElem)
			{
				pBox->SetValue(m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3);
			}
			else
			{
				pBox->SetValue(-1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3);
			}
		}

		break;
	}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


/////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead() :m_nMailIndex(-1), m_bDrag(0)
{
	m_pDeleteConfirm = NULL;
	m_pWndPostItemWarning = NULL;
}
CWndPostRead::~CWndPostRead()
{
	ClearData();
	SAFE_DELETE(m_pDeleteConfirm);
	SAFE_DELETE(m_pWndPostItemWarning);
}

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
	m_bDrag = FALSE;
}

void CWndPostRead::OnDraw(C2DRender* p2DRender)
{
	if (m_nMailIndex == -1)
		return;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	if (!mailbox[m_nMailIndex])
		return;

	CItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if (m_pItemElem && m_pItemElem->GetTexture())
	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl(WIDC_STATIC3);

		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if (hitrect.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&hitrect);

			p2DRender->RenderRoundRect(pCustom->rect, D3DCOLOR_XRGB(150, 0, 0));
			g_WndMng.PutToolTip_Item(m_pItemElem, point2, &hitrect);
		}

		m_pItemElem->GetTexture()->Render(p2DRender, pCustom->rect.TopLeft(), 255);

		if (m_pItemElem->GetProp()->dwPackMax > 1)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", m_pItemElem->m_nItemNum);

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
			CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
			p2DRender->TextOut(pCustom->rect.right - size.cx, pCustom->rect.bottom - 14, szTemp, 0xffeaebdd);
			p2DRender->SetFont(pOldFont);
		}
	}

	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl(WIDC_STATIC4);
		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if (hitrect.PtInRect(point) && mailbox[m_nMailIndex]->m_nGold > 0)
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&hitrect);

			p2DRender->RenderRoundRect(pCustom->rect, D3DCOLOR_XRGB(150, 0, 0));
		}
	}
}
void CWndPostRead::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	CWndEdit* pWndEdit3 = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CRect rect = pWndEdit3->GetWndRect();
	rect.right += 32;
	pWndEdit3->SetWndRect(rect, TRUE);

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON3);


	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
	//	pWndText->EnableWindow(FALSE);

	pWndButton->SetFocus();
	MoveParentCenter();

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC4);
	m_wndGold.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_STATIC4);
	m_wndGold.AddWndStyle(WBS_NODRAWFRAME);

}


BOOL CWndPostRead::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_POST_READ);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);

	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_READ);
}

BOOL CWndPostRead::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPostRead::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPostRead::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}
void CWndPostRead::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMailIndex == -1)
		return;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (!pMail)
		return;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl(WIDC_STATIC3);


	if (PtInRect(&(pCustom->rect), point))
	{
		m_bDrag = TRUE;
	}
}
void CWndPostRead::SetValue(int nMailIndex)
{
	m_nMailIndex = nMailIndex;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (pMail == NULL)
		return;

	CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	CWndEdit* pWndEdit3 = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CString str;
	if (pMail->m_idSender == 0)
		str.Format("FLYFF");
	else
		str.Format("%s", CPlayerDataCenter::GetInstance()->GetPlayerString(pMail->m_idSender));
	g_WndMng.StringSize(str, 16);

	if (!str.IsEmpty())
		pWndEdit1->SetString(str);
	else
		pWndEdit1->SetString("Unknown");

	//
	str.Format("%s", pMail->m_szTitle);
	g_WndMng.StringSize(str, 13);

	if (!str.IsEmpty())
	{
		pWndEdit2->SetString(str);
		pWndText->SetString(pMail->m_szText);
	}
	else
	{
		pWndEdit2->SetString("");
		pWndText->SetString("");
	}

	g_DPlay.SendQueryReadMail(pMail->m_nMail);

	char szbuffer[128] = { 0 };
	sprintf(szbuffer, "%d", pMail->m_nGold);
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
}
BOOL CWndPostRead::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDrag)
	{
		if (m_nMailIndex == -1)
			return;

		CMailBox* pMailBox = CMailBox::GetInstance();
		CMailBox& mailbox = *pMailBox;

		CMail* pMail = mailbox[m_nMailIndex];

		if (pMail && pMail->m_pItemElem && pMail->m_pItemElem->GetProp())
		{
			m_GlobalShortcut.m_pFromWnd = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
			m_GlobalShortcut.m_dwIndex = 0xffffffff;
			m_GlobalShortcut.m_dwType = ITYPE_ITEM;
			m_GlobalShortcut.m_dwId = pMail->m_pItemElem->m_dwObjId;
			m_GlobalShortcut.m_pTexture = pMail->m_pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData = (DWORD)pMail->m_pItemElem;
			_tcscpy(m_GlobalShortcut.m_szString, pMail->m_pItemElem->GetProp()->szName);
		}
	}
}

void CWndPostRead::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_nMailIndex == -1)
		return;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (!pMail)
		return;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl(WIDC_STATIC3);


	if (PtInRect(&(pCustom->rect), point))
	{
		MailReceiveItem();
	}

	pCustom = GetWndCtrl(WIDC_STATIC4);

	if (PtInRect(&(pCustom->rect), point))
	{
		MailReceiveGold();
	}
}
void CWndPostRead::MailReceiveItem()
{

	if (m_nMailIndex == -1)
		return;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (pMail && pMail->m_pItemElem)
	{

		int nDay = 0;
		DWORD dwTime = 0;
		pMail->GetMailInfo(&nDay, &dwTime);


		if ((MAX_KEEP_MAX_DAY * 24) - dwTime > (MAX_KEEP_BASIC_DAY * 24))
		{
			FLOAT fCustody = 0.0f;
			FLOAT fPay = 0.0f;
			fCustody = (FLOAT)((FLOAT)(MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay) / (FLOAT)(MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY));

			m_bDrag = FALSE;
			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize(this);
			m_pWndPostItemWarning->SetIndex(pMail->m_nMail);

			fPay = pMail->m_pItemElem->GetCost() * fCustody;
			if (fPay < 0.0f)
				fPay = 0.0f;

			TCHAR szChar[256] = { 0 };
			sprintf(szChar, prj.GetText(TID_GAME_MAILBOX_KEEP_PAY), MAX_KEEP_BASIC_DAY, (int)fPay);

			m_pWndPostItemWarning->SetString(szChar);
		}
		else
		{
			g_DPlay.SendQueryGetMailItem(pMail->m_nMail);
		}
	}
}
void CWndPostRead::MailReceiveGold()
{

	if (m_nMailIndex == -1)
		return;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (pMail && pMail->m_nMail > 0 && pMail->m_nGold > 0)
	{
		g_DPlay.SendQueryGetMailGold(pMail->m_nMail);

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
		pWndEdit->SetString("0");
	}
}
void CWndPostRead::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pDeleteConfirm)
		SAFE_DELETE(m_pDeleteConfirm);
	if (pWndChild == m_pWndPostItemWarning)
		SAFE_DELETE(m_pWndPostItemWarning);
}

BOOL CWndPostRead::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];

	if (!pMail)
		return FALSE;

	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if (pWndPost == NULL)
		return FALSE;

	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		SAFE_DELETE(m_pDeleteConfirm);
		m_pDeleteConfirm = new CWndPostDeleteConfirm;
		m_pDeleteConfirm->Initialize(this);
		m_pDeleteConfirm->SetValue(m_nMailIndex);
	}
	break;
	case WIDC_BUTTON2:
	{
		CWndEdit* pWndEdit1 = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		CWndEdit* pWndEdit2 = (CWndEdit*)GetDlgItem(WIDC_EDIT2);

		if (pWndEdit1 && pWndEdit2)
		{
			if (stricmp(pWndEdit1->GetString(), "Unknow") == 0)
			{
				g_WndMng.OpenMessageBoxUpper(prj.GetText(TID_MAIL_UNKNOW));
				return FALSE;
			}

			pWndPost->m_PostTabSend.SetItemId(0xff);
			pWndPost->m_PostTabSend.SetReceive("");
			pWndPost->m_PostTabSend.SetText("");
			pWndPost->m_PostTabSend.SetTitle("");

			CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)pWndPost->GetDlgItem(WIDC_TABCTRL1);
			pWndTabCtrl->SetCurSel(0);

#if __VER >= 11 // __SYS_PLAYER_DATA
			LPCSTR lpszPlayerString = CPlayerDataCenter::GetInstance()->GetPlayerString(pMail->m_idSender);
			char	lpszPlayer[MAX_PLAYER] = { 0, };
			if (pMail->m_idSender == 0)
				lstrcpy(lpszPlayer, "FLYFF");
			else
				lstrcpy(lpszPlayer, lpszPlayerString);
#else	// __SYS_PLAYER_DATA
			LPCSTR lpszPlayerString = prj.GetPlayerString(pMail->m_idSender);
			char	lpszPlayer[MAX_PLAYER] = { 0, };
			if (pMail->m_idSender == 0)
				lstrcpy(lpszPlayer, "FLYFF");
			else if (lpszPlayerString)
				lstrcpy(lpszPlayer, lpszPlayerString);
#endif	// __SYS_PLAYER_DATA

			pWndPost->m_PostTabSend.SetReceive((char*)lpszPlayer);

			CString str = "Re:";
			str += pWndEdit2->GetString();
			pWndPost->m_PostTabSend.SetTitle(str.GetBuffer(0));
			Destroy();
		}
	}
	break;
	case WIDC_BUTTON3:
	{
		Destroy();
	}
	break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndPostDeleteConfirm::CWndPostDeleteConfirm() :m_nIndex(0)
{
}

CWndPostDeleteConfirm::~CWndPostDeleteConfirm()
{

}

void CWndPostDeleteConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndPostDeleteConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	CWndText* pWndText = (CWndText*)(GetDlgItem(WIDC_TEXT1));
	pWndText->SetString(_T(prj.GetText(TID_MAIL_DELETE_CONFIRM)));

	MoveParentCenter();
}

BOOL CWndPostDeleteConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_POST_DELETE_CONFIRM);
	CRect rect = CRect(0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy);

	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_DELETE_CONFIRM);
}

BOOL CWndPostDeleteConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndPostDeleteConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPostDeleteConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPostDeleteConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndPostDeleteConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		CMailBox* pMailBox = CMailBox::GetInstance();
		CMailBox& mailbox = *pMailBox;

		CMail* pMail = mailbox[m_nIndex];

		if (pMail)
		{
			g_DPlay.SendQueryRemoveMail(pMail->m_nMail);
			Destroy();

			CWndPostRead* pWndPost = (CWndPostRead*)g_WndMng.GetWndBase(APP_POST_READ);
			if (pWndPost)
				pWndPost->Destroy();

		}
	}
	else if (nID == WIDC_NO)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
/////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostReceive::CWndPostReceive()
{
	m_nMax = 0;
	m_nSelect = -1;
	m_pWndPostRead = NULL;
	m_pStaticCount = nullptr;
}
CWndPostReceive::~CWndPostReceive()
{
#ifdef __FIX_WND_1109
	DeleteDeviceObjects();
#endif	// __FIX_WND_1109
	SAFE_DELETE(m_pWndPostRead);
}
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

	return TRUE;
}
int CWndPostReceive::GetSelectIndex(const CPoint& point)
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 23) / 40;

	if (0 <= nIndex && nIndex < MAX_MAIL_LIST_PER_PAGE)
	{
		int nSelect = nBase + nIndex;
		if (0 <= nSelect && nSelect < m_nMax)
			return nSelect;
	}
	return -1;
}
void CWndPostReceive::UpdateScroll()
{
	CMailBox* pMailBox = CMailBox::GetInstance();
	m_nMax = pMailBox->size();

	m_pStaticCount->m_strTitle.Format("Total mails: %d", m_nMax);

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_MAIL_LIST_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_MAIL_LIST_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}
}
void CWndPostReceive::OnDraw(C2DRender* p2DRender)
{
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;

	sx = 9;
	sy = 30;

	CRect rc(sx, 5, sx + 310, 7);
	rc += CPoint(0, 20);

	int nBase = m_wndScrollBar.GetScrollPos();
	if (nBase < 0)
	{
		nBase = 0;
		TRACE("aa\n");
	}

	TCHAR szTemp[32];

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	if (mailbox.size() != m_nMax)
		UpdateScroll();

	p2DRender->RenderTexture(CPoint(sx + 4, sy - 4), &m_Texture[0], 150);


	D3DXVECTOR2 v2_1 = D3DXVECTOR2(255, 0);
	D3DXVECTOR2 v2_2 = D3DXVECTOR2(0, 150);
	D3DXVECTOR2 v2Result;

	COleDateTime dt;
	CString strDateTime;

	for (int i = nBase; i < nBase + MAX_MAIL_LIST_PER_PAGE; ++i)
	{
		if (i >= m_nMax)
			break;

		if ((int)(mailbox.size()) <= i)
			continue;

		if (mailbox[i] == NULL)
			continue;

		if (m_nSelect >= 0 && i == m_nSelect)
		{
			rc.SetRect(sx + 4, sy - 4, sx + 240, sy + 36);
			p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(32, 190, 0, 0));
		}


		if (mailbox[i]->m_pItemElem && mailbox[i]->m_pItemElem->m_pTexture)
		{
			p2DRender->RenderTexture(CPoint(sx + 10, sy + 2), mailbox[i]->m_pItemElem->m_pTexture);

			if (mailbox[i]->m_pItemElem->GetProp()->dwPackMax > 1)
			{
				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

				_stprintf(szTemp, "%d", mailbox[i]->m_pItemElem->m_nItemNum);
				CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
				p2DRender->TextOut(sx + 40 - size.cx, sy + 20, szTemp, 0xffeaebdd);
				p2DRender->SetFont(pOldFont);
			}
		}
		else if (mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0)
			p2DRender->RenderTexture(CPoint(sx + 10, sy + 2), &m_Texture[2]);

		if (mailbox[i]->m_byRead)
			p2DRender->RenderTexture(CPoint(sx + 10, sy + 1), &m_Texture[1], 110);

		const char* lpszPlayerString = CPlayerDataCenter::GetInstance()->GetPlayerString(mailbox[i]->m_idSender);
		CString str;
		if (mailbox[i]->m_idSender == 0)
			str.Format("FLYFF");
		else
			str.Format("%s", lpszPlayerString);
		g_WndMng.StringSize(str, 16);

		if (!str.IsEmpty())
			p2DRender->TextOut(sx + 60, sy + 3, str, D3DCOLOR_XRGB(0, 0, 0));
		else
			p2DRender->TextOut(sx + 60, sy + 3, "Unknown", D3DCOLOR_XRGB(0, 0, 0));

		int nDay = 0;
		DWORD dwTime = 0;
		mailbox[i]->GetMailInfo(&nDay, &dwTime);

		if (nDay <= 1)
			str.Format(prj.GetText(TID_PK_LIMIT_HOUR), dwTime);
		else
			str.Format(prj.GetText(TID_PK_LIMIT_DAY), nDay);

		FLOAT f = (FLOAT)((FLOAT)nDay / (FLOAT)MAX_KEEP_BASIC_DAY);
		D3DXVec2Lerp(&v2Result, &v2_1, &v2_2, f);

		if (nDay <= 0)
			p2DRender->TextOut(sx + 190, sy + 3, str, D3DCOLOR_XRGB((int)v2Result.x, (int)v2Result.y, 0));
		else
			p2DRender->TextOut(sx + 193, sy + 3, str, D3DCOLOR_XRGB((int)v2Result.x, (int)v2Result.y, 0));

		str.Format("%s", mailbox[i]->m_szTitle);
		g_WndMng.StringSize(str, 13);

		if (!str.IsEmpty())
			p2DRender->TextOut(sx + 60, sy + 18, str, D3DCOLOR_XRGB(0, 0, 190));
		else
			p2DRender->TextOut(sx + 60, sy + 18, "", D3DCOLOR_XRGB(0, 0, 190));

		sy += 40;
	}
}

void CWndPostReceive::OnMouseWndSurface(CPoint point)
{
	int	sx, sy;

	sx = 15;
	sy = 30;

	CRect rc(sx, 5, sx + 310, 7);
	rc += CPoint(0, 20);

	int nBase = m_wndScrollBar.GetScrollPos();
	if (nBase < 0)
	{
		nBase = 0;
		TRACE("aa\n");
	}

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	CString strDateTime;
	CPoint cpoint = point;

	for (int i = nBase; i < nBase + MAX_MAIL_LIST_PER_PAGE; ++i)
	{
		if (i >= m_nMax)
			break;

		if ((int)(mailbox.size()) <= i)
			continue;

		if (mailbox[i] == NULL)
			continue;

		//		if( GetFocusWnd() == this )
		{
			rc.SetRect(sx + 4, sy - 4, sx + 240, sy + 36);
			if (PtInRect(rc, cpoint))
			{
				ClientToScreen(&cpoint);
				ClientToScreen(&rc);

				CTime cTime(mailbox[i]->m_tmCreate);
				strDateTime.Format("%.2d.%.2d.%.2d %.2d:%.2d", cTime.GetMonth(), cTime.GetDay(), cTime.GetYear(), cTime.GetHour(), cTime.GetMinute());
				strDateTime.Format(prj.GetText(TID_GAME_MAIL_RECEIVE_DATE), strDateTime);

				g_toolTip.PutToolTip((DWORD)this, strDateTime, rc, cpoint);
				break;
			}
		}

		sy += 40;
	}
}

void CWndPostReceive::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_nMax <= 0)
		return;

	int nSelect = GetSelectIndex(point);
	if (nSelect == -1)
		return;

	m_nSelect = nSelect;

	CMailBox* pMailBox = CMailBox::GetInstance();
	CMailBox& mailbox = *pMailBox;

	if (mailbox[m_nSelect] == NULL)
		return;

	SAFE_DELETE(m_pWndPostRead);
	m_pWndPostRead = new CWndPostRead;
	m_pWndPostRead->Initialize(this);
	m_pWndPostRead->SetValue(m_nSelect);
}

void CWndPostReceive::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndPostRead)
		SAFE_DELETE(m_pWndPostRead);
}

void CWndPostReceive::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pStaticCount = (CWndStatic*)GetDlgItem(WIDC_STATIC1);

	CRect rect = GetWindowRect();
	rect.top += 15;
	rect.bottom -= 50;
	rect.left = rect.right - 40;
	rect.right -= 30;

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_MAIL_LIST_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_MAIL_LIST_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);


	MoveParentCenter();

	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndPostTable.tga"), 0xffff00ff);
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndNotUse.tga"), 0xffff00ff);
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, "itm_GolGolSeed.dds"), 0xffff00ff);

}

BOOL CWndPostReceive::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_POST_RECEIVE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPostReceive::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPostReceive::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPostReceive::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPostReceive::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMax <= 0)
		return;

	int nSelect = GetSelectIndex(point);
	if (nSelect != -1)
		m_nSelect = nSelect;
}
BOOL CWndPostReceive::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndPostReceive::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

/////////////////////////////////////////////////////////////////////
// CWndGuildCombatOfferMessageBox
////////////////////////////////////////////////////////////////////////
class CWndGuildCombatOfferMessageBox : public CWndMessageBox
{
public:
	DWORD m_nCost;
	void	SetValue(CString str, DWORD nCost);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
};

BOOL CWndGuildCombatOfferMessageBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize("",
		pWndParent,
		MB_OKCANCEL);
}

void CWndGuildCombatOfferMessageBox::SetValue(CString str, DWORD nCost)
{
	m_wndText.SetString(str);
	m_nCost = nCost;
}

BOOL CWndGuildCombatOfferMessageBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
	{
		g_DPlay.SendGuildCombatApp(m_nCost);
		SAFE_DELETE(g_WndMng.m_pWndGuildCombatOffer);
		Destroy();
	}
	break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}




CWndGuildCombatOffer::CWndGuildCombatOffer()
{
	m_dwMinGold = 0;
	m_dwBackupGold = 0;
	m_dwReqGold = 0;
}

CWndGuildCombatOffer::~CWndGuildCombatOffer()
{

}

void CWndGuildCombatOffer::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildCombatOffer::EnableAccept(BOOL bFlag)
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_ACCEPT);

	if (pWndButton)
	{
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndGuildCombatOffer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString(prj.GetText(TID_GAME_GUILDCOMBAT_PRESENT_TEXT));
	pWndText->EnableWindow(TRUE);

	MoveParentCenter();
}

BOOL CWndGuildCombatOffer::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_OFFER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildCombatOffer::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildCombatOffer::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildCombatOffer::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatOffer::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatOffer::SetGold(DWORD nCost)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

	if (pWndEdit)
	{
		CString str;
		str.Format("%d", nCost);
		pWndEdit->SetString(str);
	}
}

void CWndGuildCombatOffer::SetTotalGold(__int64 nCost)
{
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	pStatic->AddWndStyle(WSS_MONEY);

	if (pStatic)
	{
		CString str;
		str.Format("%I64d", nCost);
		pStatic->SetTitle(str);
	}
}

BOOL CWndGuildCombatOffer::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_ACCEPT)
	{
		if (g_pPlayer)
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);

			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi(str);

			if (m_dwReqGold != 0)
			{
				if (nCost <= m_dwBackupGold)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_MORE_CURRENT_REQUEST));
					return FALSE;
				}

			}
			else
			{
				if (nCost < m_dwMinGold)
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_MIN));
					return FALSE;
				}
			}


			CWndGuildCombatOfferMessageBox* pMsg = new CWndGuildCombatOfferMessageBox;
			if (pMsg)
			{
				g_WndMng.OpenCustomBox("", pMsg, this);
				CString str;

				if (m_dwReqGold == 0)
				{
					str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), 0, nCost);
				}
				else
				{
					str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_MORE_REQUEST), m_dwBackupGold, nCost - m_dwBackupGold);
				}

				pMsg->SetValue(str, nCost);
			}
		}

		//		Destroy();
	}
	else if (nID == WIDC_CLOSE)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CGuildCombatInfoMessageBox::CGuildCombatInfoMessageBox()
{
}

CGuildCombatInfoMessageBox::~CGuildCombatInfoMessageBox()
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CGuildCombatInfoMessageBox::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	RenderWnd();

	if (IsWndStyle(WBS_CAPTION))
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
		p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

		char szNameLevel[128] = { 0, };

		sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE));

		SetTitle(szNameLevel);

		p2DRender->SetFont(pOldFont);
	}
}
#endif //__GUILD_COMBAT_1TO1

BOOL CGuildCombatInfoMessageBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG, 0, CPoint(0, 0), pWndParent);
}

BOOL CGuildCombatInfoMessageBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_YES:
	{
		g_DPlay.SendGCTele();
		Destroy();
	}
	break;
	case WIDC_NO:
	{
		Destroy();
	}
	case 10000:
	{
		Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CGuildCombatInfoMessageBox::SetString(CHAR* szChar)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(szChar);
	pWndText->ResetString();
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CGuildCombatInfoMessageBox::SetString(CString strMsg)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(strMsg);
	pWndText->ResetString();
}
#endif //__GUILD_COMBAT_1TO1

void CGuildCombatInfoMessageBox::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();
}

//////////////////////////////////////////////////////////////////////////////////////////////


CGuildCombatInfoMessageBox2::CGuildCombatInfoMessageBox2()
{
}
CGuildCombatInfoMessageBox2::~CGuildCombatInfoMessageBox2()
{
}
void CGuildCombatInfoMessageBox2::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();
}

BOOL CGuildCombatInfoMessageBox2::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_MSG2, 0, CPoint(0, 0), pWndParent);
}

BOOL CGuildCombatInfoMessageBox2::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_OK:
	{
		Destroy();
	}
	break;
	case 10000:
	{
		Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CGuildCombatInfoMessageBox2::SetString(CHAR* szChar)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	pWndText->m_string.AddParsingString(szChar);
	pWndText->ResetString();
}
//////////////////////////////////////////////////////////////////////////////////////////////////





BOOL CGuildCombatSelectionClearMessageBox::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize(prj.GetText(TID_GAME_GUILDCOMBAT_REMAKE_MAKEUP),
		pWndParent,
		MB_OKCANCEL);
}

BOOL CGuildCombatSelectionClearMessageBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
	{
		CWndGuildCombatSelection* pGuildCombatSelection = (CWndGuildCombatSelection*)g_WndMng.GetWndBase(APP_GUILDCOMBAT_SELECTION);

		if (pGuildCombatSelection)
			pGuildCombatSelection->Reset();

		Destroy();
	}
	break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
//////////////////////////////////////////////////////////////////////////////////////////////





CWndGuildCombatSelection::CWndGuildCombatSelection()
{
	m_vecGuildList.clear();
	m_mapSelectPlayer.clear();
	m_vecSelectPlayer.clear();
	m_uidDefender = -1;

	nMaxJoinMember = 0;
	nMaxWarMember = 0;
}

CWndGuildCombatSelection::~CWndGuildCombatSelection()
{

}

void CWndGuildCombatSelection::SetDefender(u_long uiPlayer)
{
	m_uidDefender = uiPlayer;
}

void CWndGuildCombatSelection::AddCombatPlayer(u_long uiPlayer)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	m_vecSelectPlayer.push_back(uiPlayer);

	CGuild* pGuild = g_pPlayer->GetGuild();

	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find(uiPlayer);
	CGuildMember* pMember = i->second;

	CString str;
#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
	str.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
#else	// __SYS_PLAYER_DATA
	str.Format("Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString(pMember->m_idPlayer), prj.m_aJob[pMember->m_nJob].szName);
#endif	// __SYS_PLAYER_DATA
	pWndList->AddString(str);
}

void CWndGuildCombatSelection::SetMemberSize(int nMaxJoin, int nMaxWar)
{
	nMaxJoinMember = nMaxJoin;
	nMaxWarMember = nMaxWar;

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);

	CString str;
	str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_OFFER_INFO), nMaxWarMember, nMaxJoinMember);
	pWndStatic->SetTitle(str);
}


void CWndGuildCombatSelection::AddGuildPlayer(u_long uiPlayer)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_vecGuildList.push_back(uiPlayer);

	CGuild* pGuild = g_pPlayer->GetGuild();

	map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.find(uiPlayer);
	CGuildMember* pMember = i->second;

	CString str;
#if __VER >= 11 // __SYS_PLAYER_DATA
	PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
	str.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
#else	// __SYS_PLAYER_DATA
	str.Format("Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString(pMember->m_idPlayer), prj.m_aJob[pMember->m_nJob].szName);
#endif	// __SYS_PLAYER_DATA
	pWndList->AddString(str);
}

void CWndGuildCombatSelection::RemoveGuildPlayer(int nIndex)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	pWndList->DeleteString(nIndex);
	m_vecGuildList.erase(m_vecGuildList.begin() + nIndex);
}

void CWndGuildCombatSelection::RemoveCombatPlayer(int nIndex)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

	pWndList->DeleteString(nIndex);

	if (m_vecSelectPlayer[nIndex] == m_uidDefender)
		SetDefender(-1);

	m_vecSelectPlayer.erase(m_vecSelectPlayer.begin() + nIndex);
}


void CWndGuildCombatSelection::UpDateGuildListBox()
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	if (pWndList)
	{
		pWndList->ResetContent();

		m_vecGuildList.clear();
		m_mapSelectPlayer.clear();

		CGuild* pGuild = g_pPlayer->GetGuild();
		if (pGuild)
		{

			CGuildMember* pMember;
			for (map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin(); i != pGuild->m_mapPMember.end(); ++i)
			{
				pMember = i->second;
#if __VER >= 11 // __SYS_PLAYER_DATA
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
				if (pPlayerData->data.uLogin > 0)
					m_mapSelectPlayer.insert(make_pair(pPlayerData->data.nLevel, pMember));
#else	// __SYS_PLAYER_DATA
				if (pMember->m_nLogin)
					m_mapSelectPlayer.insert(make_pair(pMember->m_nLevel, pMember));
#endif	// __SYS_PLAYER_DATA
			}


			CString str;
			for (multimap<int, CGuildMember*>::iterator j = m_mapSelectPlayer.begin(); j != m_mapSelectPlayer.end(); ++j)
			{
				pMember = j->second;
#if __VER >= 11 // __SYS_PLAYER_DATA
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
				if (pPlayerData->data.uLogin > 0)
#else	// __SYS_PLAYER_DATA
				if (pMember->m_nLogin)
#endif	// __SYS_PLAYER_DATA
				{
#if __VER >= 11 // __SYS_PLAYER_DATA
					str.Format("Lv%.2d	%.16s %.10s", pPlayerData->data.nLevel, pPlayerData->szPlayer, prj.m_aJob[pPlayerData->data.nJob].szName);
#else	// __SYS_PLAYER_DATA
					str.Format("Lv%.2d	%.16s %.10s", pMember->m_nLevel, prj.GetPlayerString(pMember->m_idPlayer), prj.m_aJob[pMember->m_nJob].szName);
#endif	// __SYS_PLAYER_DATA
					pWndList->AddString(str);
					m_vecGuildList.push_back(pMember->m_idPlayer);
				}
			}
		}
	}
}

u_long CWndGuildCombatSelection::FindCombatPlayer(u_long uiPlayer)
{
	for (int i = 0; i < (int)(m_vecSelectPlayer.size()); i++)
	{
		if (m_vecSelectPlayer[i] == uiPlayer)
			return m_vecSelectPlayer[i];
	}

	return -1;
}

u_long CWndGuildCombatSelection::FindGuildPlayer(u_long uiPlayer)
{
	for (int i = 0; i < (int)(m_vecGuildList.size()); i++)
	{
		if (m_vecGuildList[i] == uiPlayer)
			return m_vecGuildList[i];
	}

	return -1;
}

void CWndGuildCombatSelection::OnDraw(C2DRender* p2DRender)
{
	CWndButton* pWndStatic = (CWndButton*)GetDlgItem(WIDC_BUTTON8);

	if (pWndStatic)
	{
		pWndStatic->EnableWindow(FALSE);

		if (m_uidDefender <= 0)
		{
			pWndStatic->SetVisible(FALSE);
		}
		else
		{
			pWndStatic->SetVisible(TRUE);

			int i = NULL;
			for (; i < (int)(m_vecSelectPlayer.size()); i++)
			{
				if (m_uidDefender == m_vecSelectPlayer[i])
					break;
			}

			pWndStatic->Move(330, (i * 17) + 75);
		}
	}

	CRect crect;
	crect.left = 349;
	crect.top = 74;
	crect.right = 587;
	crect.bottom = crect.top + (nMaxWarMember * 17);
	p2DRender->RenderFillRect(crect, D3DCOLOR_ARGB(40, 0, 0, 220));

	crect.left = 349;
	crect.top = crect.bottom;
	crect.right = 587;
	crect.bottom = 377;
	p2DRender->RenderFillRect(crect, D3DCOLOR_ARGB(40, 220, 0, 0));

}

void CWndGuildCombatSelection::EnableFinish(BOOL bFlag)
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_FINISH);

	if (pWndButton)
	{
		pWndButton->EnableWindow(bFlag);
	}
}

void CWndGuildCombatSelection::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE)
	{
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP));
		Destroy();
		return;
	}

	MoveParentCenter();

	UpDateGuildListBox();
}

BOOL CWndGuildCombatSelection::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELECTION, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildCombatSelection::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildCombatSelection::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildCombatSelection::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatSelection::OnLButtonDown(UINT nFlags, CPoint point)
{
}


void CWndGuildCombatSelection::Reset()
{
	m_uidDefender = 0;
	UpDateGuildListBox();
	CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	pWndListBox->ResetContent();
	m_vecSelectPlayer.clear();
}

BOOL CWndGuildCombatSelection::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	if (nID == WIDC_BUTTON1)
	{
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel == -1)
			return FALSE;

		if (nMaxJoinMember < (int)(m_vecSelectPlayer.size()))
		{
			CString str;
			str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_SELECTION_MAX), nMaxJoinMember);
			g_WndMng.OpenMessageBox(str);
			return FALSE;
		}

		CGuild* pGuild = g_pPlayer->GetGuild();

		if (pGuild)
		{
			CGuildMember* pGuildMember = pGuild->GetMember(m_vecGuildList[nCurSel]);

			if (pGuildMember)
			{
#if __VER >= 11 // __SYS_PLAYER_DATA
				if (CPlayerDataCenter::GetInstance()->GetPlayerData(pGuildMember->m_idPlayer)->data.nLevel < 30)
#else	// __SYS_PLAYER_DATA
				if (pGuildMember->m_nLevel < 30)
#endif	// __SYS_PLAYER_DATA
				{
					g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_LIMIT_LEVEL_NOTICE));
					return FALSE;
				}
			}
			else
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_MEMBER));
				return FALSE;
			}
		}

		u_long uiPlayer;
		uiPlayer = FindCombatPlayer(m_vecGuildList[nCurSel]);

		if (uiPlayer != -1)
		{
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_ALREADY_ENTRY));
			return FALSE;
		}

		AddCombatPlayer(m_vecGuildList[nCurSel]);
		RemoveGuildPlayer(nCurSel);
	}
	else

		if (nID == WIDC_BUTTON2)
		{
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

			int nCurSel = pWndListBox->GetCurSel();
			if (nCurSel == -1)
				return FALSE;

			u_long uiPlayer;
			uiPlayer = FindGuildPlayer(m_vecSelectPlayer[nCurSel]);

			if (uiPlayer == -1)
			{

				AddGuildPlayer(m_vecSelectPlayer[nCurSel]);
				RemoveCombatPlayer(nCurSel);
			}
			else
			{
				RemoveCombatPlayer(nCurSel);
			}
		}
		else

			if (nID == WIDC_BUTTON3)
			{
				CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

				int nCurSel = pWndListBox->GetCurSel();
				if (nCurSel == -1 || nCurSel == 0)
					return FALSE;

				CString temp1, temp2;
				pWndListBox->GetText(nCurSel - 1, temp1);
				pWndListBox->GetText(nCurSel, temp2);

				pWndListBox->SetString(nCurSel - 1, temp2);
				pWndListBox->SetString(nCurSel, temp1);

				u_long uiTemp = 0;

				uiTemp = m_vecSelectPlayer[nCurSel];
				m_vecSelectPlayer[nCurSel] = m_vecSelectPlayer[nCurSel - 1];
				m_vecSelectPlayer[nCurSel - 1] = uiTemp;

				m_nDefenderIndex = nCurSel - 1;
			}
			else

				if (nID == WIDC_BUTTON4)
				{
					CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

					int nCurSel = pWndListBox->GetCurSel();
					if (nCurSel == -1 || nCurSel == pWndListBox->GetCount() - 1)
						return FALSE;

					CString temp1, temp2;
					pWndListBox->GetText(nCurSel, temp1);
					pWndListBox->GetText(nCurSel + 1, temp2);

					pWndListBox->SetString(nCurSel, temp2);
					pWndListBox->SetString(nCurSel + 1, temp1);

					u_long uiTemp = 0;

					uiTemp = m_vecSelectPlayer[nCurSel];
					m_vecSelectPlayer[nCurSel] = m_vecSelectPlayer[nCurSel + 1];
					m_vecSelectPlayer[nCurSel + 1] = uiTemp;

					m_nDefenderIndex = nCurSel + 1;
				}
				else
					if (nID == WIDC_RESET)
					{
						CGuildCombatSelectionClearMessageBox* pBox = new CGuildCombatSelectionClearMessageBox;
						g_WndMng.OpenCustomBox("", pBox);
					}
					else
						if (nID == WIDC_FINISH)
						{
#ifndef _DEBUG
							if (m_uidDefender == -1 || m_uidDefender == 0)
							{
								g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_ASSIGN_DEFENDER));
								return FALSE;
							}
#endif //_DEBUG


							if (g_GuildCombatMng.m_nGCState != CGuildCombat::NOTENTER_COUNT_STATE)
							{
								g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_CANNOT_MAKEUP));
								Destroy();
								return FALSE;
							}

							if (m_vecSelectPlayer.size() == 0)
							{
								g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_PLAYER));
								return FALSE;
							}

							CGuild* pGuild = g_pPlayer->GetGuild();
							CGuildMember* pGuildMemberl;

							if (pGuild)
							{
								BOOL bSkip = FALSE;



								for (int i = 0; i < (int)(m_vecSelectPlayer.size()); i++)
								{
									pGuildMemberl = pGuild->GetMember(m_vecSelectPlayer[i]);

									if (pGuildMemberl)
									{
										if (pGuildMemberl->m_nMemberLv == GUD_MASTER || pGuildMemberl->m_nMemberLv == GUD_KINGPIN)
										{
											bSkip = TRUE;
											break;
										}
									}
								}

								if (bSkip)
								{
									g_DPlay.SendGCSelectPlayer(m_vecSelectPlayer, m_uidDefender);
									Destroy();
								}
								else
								{
									g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_HAVENOT_MASTER));
									return FALSE;
								}
							}
						}
						else

							if (nID == WIDC_BUTTON7)
							{
								CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);

								int nCurSel = pWndListBox->GetCurSel();
								if (nCurSel == -1)
									return FALSE;

								CGuild* pGuild = g_pPlayer->GetGuild();

								if (pGuild)
								{
									if (m_vecSelectPlayer.size() > 1 && pGuild->IsMaster(m_vecSelectPlayer[nCurSel]))
									{
										g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_NOT_ASSIGN_DEFENDER));
										return FALSE;
									}
								}

								m_nDefenderIndex = nCurSel;
								SetDefender(m_vecSelectPlayer[nCurSel]);
							}
							else
								if (nID == WIDC_CLOSE)
								{
									Destroy();
								}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndGuildCombatState::CWndGuildCombatState()
{
	Init(0);

	m_tEndTime = 0;
	m_tCurrentTime = 0;
}

CWndGuildCombatState::~CWndGuildCombatState()
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildCombatState::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	RenderWnd();

	if (IsWndStyle(WBS_CAPTION))
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
		p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

		char szNameLevel[128] = { 0, };

		sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_OFFERSTATE));

		SetTitle(szNameLevel);

		p2DRender->SetFont(pOldFont);
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildCombatState::Init(time_t lTime)
{
}

void CWndGuildCombatState::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	MoveParentCenter();
}

void CWndGuildCombatState::InsertTitle(const char szTitle[])
{
	CString strTitle;
	strTitle.Format("%s - %s", GetTitle(), szTitle);
	SetTitle(strTitle);
}

BOOL CWndGuildCombatState::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_OFFERSTATE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildCombatState::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildCombatState::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildCombatState::OnLButtonUp(UINT nFlags, CPoint point)
{
}


int CWndGuildCombatState::GetSelectIndex(const CPoint& point)
{
	return -1;
}

void CWndGuildCombatState::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatState::OnRButtonUp(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildCombatState::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_CLOSE)
	{
		Destroy();
	}
	else
		if (nID == WIDC_BUTTON1)
		{
			Destroy();
			g_DPlay.SendGuildCombatWindow();
		}
		else
			if (nID == WIDC_BUTTON2)
			{
				g_DPlay.SendGCRequestStatusWindow();
			}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildCombatState::InsertGuild(const char szGuild[], const char szName[], int nNum)
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	CString str;
	str.Format("No.%.2d %s", nNum, szGuild);
	pWndList->AddString(str);
}

void CWndGuildCombatState::OnDraw(C2DRender* p2DRender)
{
	if (m_tCurrentTime > 0)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;

		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()));
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours());
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes());
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds());

		str.Format("%s %s %s %s", str1, str2, str3, str4);
		pStatic->SetTitle(str);
	}
}

void CWndGuildCombatState::SetRate(int nRate)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);

	CString str;
	str.Format("%d", nRate);

	if (nRate == -1)
		str = " ";

	pWndStatic->SetTitle(str);
	if (8 < nRate)
		pWndStatic->m_dwColor = 0xFFFF0000;
}

BOOL CWndGuildCombatState::Process()
{
	m_tCurrentTime = (m_tEndTime - time_null());

	if (m_tEndTime && m_tCurrentTime > 0)
	{
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
	}

	return TRUE;
}

#if __VER < 11 // __GUILD_COMBAT_1TO1
void CWndGuildCombatState::SetTotalGold(__int64 nGold)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC8);
	pWndStatic->AddWndStyle(WSS_MONEY);

	CString str;
	str.Format("%I64d", nGold);
	pWndStatic->SetTitle(str);

}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildCombatState::SetGold(int nGold)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC6);
	pWndStatic->AddWndStyle(WSS_MONEY);

	CString str;
	str.Format("%d", nGold);
	pWndStatic->SetTitle(str);
}




CWndGuildCombatJoinSelection::CWndGuildCombatJoinSelection()
{
	m_dwOldTime = 0xffffffff;
}

CWndGuildCombatJoinSelection::~CWndGuildCombatJoinSelection()
{

}

void CWndGuildCombatJoinSelection::OnDraw(C2DRender* p2DRender)
{
	DWORD dwLeftTime = m_dwOldTime - g_tmCurrent;
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	CString str;
	str.Format(prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_MESSAGE), (dwLeftTime / 1000) + 1);
	pStatic->SetTitle(str);
}

void CWndGuildCombatJoinSelection::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	MoveParentCenter();

	m_wndTitleBar.SetVisible(FALSE);
}

BOOL CWndGuildCombatJoinSelection::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_SELSTART, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildCombatJoinSelection::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildCombatJoinSelection::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildCombatJoinSelection::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatJoinSelection::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildCombatJoinSelection::Process()
{
	if (m_dwOldTime <= g_tmCurrent)
		Destroy();

	return TRUE;
}

BOOL CWndGuildCombatJoinSelection::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWndGuildWarAppConfirm::CWndGuildWarAppConfirm()
{
}

CWndGuildWarAppConfirm::~CWndGuildWarAppConfirm()
{
}

void CWndGuildWarAppConfirm::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildWarAppConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	m_wndText.Create(WBS_NODRAWFRAME, rect, this, 0);
	m_wndText.SetString(_T(prj.GetText(TID_GAME_GUILDWAR_APP)), 0xff000000);//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();

	MoveParentCenter();
}

BOOL CWndGuildWarAppConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildWarAppConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildWarAppConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildWarAppConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildWarAppConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildWarAppConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (g_pPlayer)
			g_DPlay.SendGuildCombatApp(80000);

		Destroy();
	}
	else if (nID == WIDC_NO)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndGuildWarCancelConfirm::CWndGuildWarCancelConfirm()
{
}

CWndGuildWarCancelConfirm::~CWndGuildWarCancelConfirm()
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildWarCancelConfirm::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	RenderWnd();

	if (IsWndStyle(WBS_CAPTION))
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
		p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

		char szNameLevel[128] = { 0, };

		sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_CANCEL_CONFIRM));

		SetTitle(szNameLevel);

		p2DRender->SetFont(pOldFont);
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildWarCancelConfirm::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildWarCancelConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	m_wndText.Create(WBS_NODRAWFRAME, rect, this, 0);
	m_wndText.SetString(_T(prj.GetText(TID_GAME_GUILDWAR_CANCEL)), 0xff000000);//prj.GetTextColor(TID_DIAG_0065) );
	m_wndText.ResetString();

	MoveParentCenter();
}

BOOL CWndGuildWarCancelConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_CANCEL_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildWarCancelConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildWarCancelConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildWarCancelConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildWarCancelConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildWarCancelConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (g_pPlayer)
			g_DPlay.SendGuildCombatCancel();

		Destroy();
	}
	else if (nID == WIDC_NO)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndGuildWarJoinConfirm::CWndGuildWarJoinConfirm()
{
}

CWndGuildWarJoinConfirm::~CWndGuildWarJoinConfirm()
{
}

#if __VER >= 11 // __GUILD_COMBAT_1TO1
void CWndGuildWarJoinConfirm::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	RenderWnd();

	if (IsWndStyle(WBS_CAPTION))
	{
		int y = 4;
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
		p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

		char szNameLevel[128] = { 0, };
		sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_GUILDCOMBAT_ENTRANCE));

		SetTitle(szNameLevel);

		p2DRender->SetFont(pOldFont);
	}
}
#endif //__GUILD_COMBAT_1TO1

void CWndGuildWarJoinConfirm::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildWarJoinConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	rect.left += 8;
	rect.right -= 4;
	rect.top += 8;
	rect.bottom /= 2;

	m_wndText.Create(WBS_NODRAWFRAME, rect, this, 0);
	m_wndText.SetString(_T(prj.GetText(TID_GAME_GUILDWAR_JOIN)), prj.GetTextColor(TID_GAME_GUILDWAR_JOIN));
	m_wndText.ResetString();

	MoveParentCenter();
}

BOOL CWndGuildWarJoinConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CLOSE_EXISTING_CONNECTION, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildWarJoinConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildWarJoinConfirm::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildWarJoinConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndGuildWarJoinConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildWarJoinConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_YES)
	{
		if (g_pPlayer)
		{
			if (g_pPlayer->IsBaseJob())
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GUILDCOMBAT_NOTENTER_VAGRANT));
				Destroy();
				return FALSE;
			}
			g_DPlay.SendGCJoin();
		}

		Destroy();
	}
	else if (nID == WIDC_NO)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}




CWndGuildWarState::CWndGuildWarState()
{
	Init(0);
}

CWndGuildWarState::~CWndGuildWarState()
{
}

void CWndGuildWarState::Init(time_t lTime)
{
	memset(m_aList, 0, sizeof(m_aList));
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildWarState::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetWindowRect();
	rect.top += 53;
	rect.bottom -= 10;
	rect.left = rect.right - 30;
	rect.right -= 10;

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_GUILDCOMBAT_LIST_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);


	MoveParentCenter();
}

void CWndGuildWarState::InsertTitle(const char szTitle[])
{
	CString strTitle;
	strTitle.Format("%s - %s", GetTitle(), szTitle);
	SetTitle(strTitle);
}

BOOL CWndGuildWarState::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_WAR_STATE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildWarState::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildWarState::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildWarState::OnLButtonUp(UINT nFlags, CPoint point)
{
}


int CWndGuildWarState::GetSelectIndex(const CPoint& point)
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if (0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE)
	{
		int nSelect = nBase + nIndex;
		if (0 <= nSelect && nSelect < m_nMax)
			return nSelect;
	}
	return -1;
}

void CWndGuildWarState::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMax <= 0)
		return;

	int nSelect = GetSelectIndex(point);
	if (nSelect != -1)
		m_nSelect = nSelect;
}

void CWndGuildWarState::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnRButtonUp(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildWarState::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildWarState::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndGuildWarState::OnMouseWndSurface(CPoint point)
{
}
void CWndGuildWarState::InsertGuild(const char szGuild[], const char szName[], int nNum)
{
	if (m_nMax >= MAX_GUILDCOMBAT_LIST)
	{
		Error("CWndGuildWarState::InsertGuild - range over");
		return;
	}

	if (nNum == 0)
		return;

	CString str;
	str.Format("%s", szGuild);
	g_WndMng.StringSize(str, 8);
	_tcscpy(m_aList[m_nMax].szGuild, str);
	_tcscpy(m_aList[m_nMax].szName, szName);
	m_aList[m_nMax].nNum = nNum;

	m_nMax++;
}

void CWndGuildWarState::OnDraw(C2DRender* p2DRender)
{
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;

	CRect rc(sx, 5, sx + 310, 7);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	rc += CPoint(0, 20);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	int nBase = m_wndScrollBar.GetScrollPos();
	if (nBase < 0)
	{
		nBase = 0;
		TRACE("aa\n");
	}

	for (int i = nBase; i < nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i)
	{
		if (i >= m_nMax)
			break;

		sprintf(szNum, "%3d", i + 1);
		sprintf(szCount, "%2d", m_aList[i].nNum);

		if (m_nSelect >= 0 && i == m_nSelect)
		{
			rc.SetRect(sx, sy - 4, sx + 295, sy + 16);
			p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(64, 0, 0, 0));
		}

		p2DRender->TextOut(sx + 4, sy, szNum, dwColor);
		p2DRender->TextOut(sx + 30, sy, m_aList[i].szGuild, dwColor);
		p2DRender->TextOut(sx + 160, sy, m_aList[i].szName, dwColor);
		p2DRender->TextOut(sx + 260, sy, szCount, dwColor);

		sy += 18;
	}
}
BOOL CWndGuildWarState::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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






CWndGuildCombatRanking::CWndGuildCombatRanking()
{
	Init(0);
}

CWndGuildCombatRanking::~CWndGuildCombatRanking()
{
}

void CWndGuildCombatRanking::Init(time_t lTime)
{
	m_multimapRanking.clear();
	m_nMax = 0;
	m_nSelect = -1;
}

void CWndGuildCombatRanking::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetWindowRect();
	rect.top += 53;
	rect.bottom -= 10;
	rect.left = rect.right - 30;
	rect.right -= 10;

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_GUILDCOMBAT_LIST_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_GUILDCOMBAT_LIST_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);


	MoveParentCenter();
}

BOOL CWndGuildCombatRanking::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKING, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndGuildCombatRanking::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndGuildCombatRanking::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndGuildCombatRanking::OnLButtonUp(UINT nFlags, CPoint point)
{
}


int CWndGuildCombatRanking::GetSelectIndex(const CPoint& point)
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if (0 <= nIndex && nIndex < MAX_GUILDCOMBAT_LIST_PER_PAGE)
	{
		int nSelect = nBase + nIndex;
		if (0 <= nSelect && nSelect < m_nMax)
			return nSelect;
	}
	return -1;
}

void CWndGuildCombatRanking::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMax <= 0)
		return;

	int nSelect = GetSelectIndex(point);
	if (nSelect != -1)
		m_nSelect = nSelect;
}

void CWndGuildCombatRanking::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnRButtonDown(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnRButtonUp(UINT nFlags, CPoint point)
{
}

BOOL CWndGuildCombatRanking::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndGuildCombatRanking::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRanking::OnMouseWndSurface(CPoint point)
{
}
void CWndGuildCombatRanking::InsertGuild(const char szGuild[], int nWinCount)
{
	if (m_nMax >= MAX_GUILDCOMBAT_LIST)
	{
		Error("CWndGuildWarState::InsertGuild - range over");
		return;
	}

	CString str;
	str.Format("%s", szGuild);
	g_WndMng.StringSize(str, 8);

	GUILDNAME p;
	memset(&p, 0, sizeof(GUILDNAME));
	_tcscpy(p.szGuild, str);

	m_multimapRanking.insert(make_pair(nWinCount, p));

	m_nMax++;
}

void CWndGuildCombatRanking::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;

	CRect rc(sx, 5, sx + 230, 7);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	rc += CPoint(0, 20);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	int nBase = m_wndScrollBar.GetScrollPos();
	if (nBase < 0)
	{
		nBase = 0;
		TRACE("aa\n");
	}

	int nRanking = 0;
	int nOldRanking = -1;
	multimap< int, GUILDNAME >::reverse_iterator iterRobotBegin = m_multimapRanking.rbegin();

	for (int i = nBase; i < nBase + MAX_GUILDCOMBAT_LIST_PER_PAGE; ++i)
	{
		if (i >= m_nMax)
			break;

		multimap< int, GUILDNAME >::value_type& refValue = *iterRobotBegin;

		if (nOldRanking != refValue.first)
			nRanking++;

		sprintf(szNum, "%3d", nRanking);
		sprintf(szCount, "%2d", refValue.first);

		if (m_nSelect >= 0 && i == m_nSelect)
		{
			rc.SetRect(sx, sy - 4, sx + 215, sy + 16);
			p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(64, 0, 0, 0));
		}

		if (i == 0)
			dwColor = D3DCOLOR_XRGB(200, 0, 0);
		else
			dwColor = D3DCOLOR_XRGB(0, 0, 0);

		if (nOldRanking != refValue.first)
		{
			p2DRender->TextOut(sx + 4, sy, szNum, dwColor);
		}
		else
		{
			p2DRender->TextOut(sx + 5, sy, "   ", dwColor);
		}

		if (i == 0)
		{
			p2DRender->TextOut(sx + 40, sy, refValue.second.szGuild, D3DCOLOR_XRGB(200, 0, 0));
			p2DRender->TextOut(sx + 180, sy, szCount, D3DCOLOR_XRGB(200, 0, 0));
		}
		else
		{
			p2DRender->TextOut(sx + 40, sy, refValue.second.szGuild, dwColor);
			p2DRender->TextOut(sx + 180, sy, szCount, dwColor);
		}


		sy += 18;

		iterRobotBegin++;
		nOldRanking = refValue.first;
	}
}
BOOL CWndGuildCombatRanking::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CWndGuildCombatRanking::SortRanking()
{

}



CWndGuildCombatResult::CWndGuildCombatResult()
{
}
CWndGuildCombatResult::~CWndGuildCombatResult()
{
}
void CWndGuildCombatResult::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildCombatResult::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );

	pWndTabCtrl->SetButtonLength(100);

	m_WndGuildCombatTabResultRate.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT);
	m_WndGuildCombatTabResultLog.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_TAB_RESULT_LOG);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultRate;
	pWndTabCtrl->InsertItem(0, &tabTabItem);


	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG2);
	tabTabItem.pWndBase = &m_WndGuildCombatTabResultLog;
	pWndTabCtrl->InsertItem(1, &tabTabItem);


	MoveParentCenter();

	{
		//		CRect r = GetWindowRect( TRUE );
		//		m_WndGuildTabMember.m_rectParent = r;
	}

	CWndListBox* pWndList1 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem(WIDC_LISTBOX1));
	CWndListBox* pWndList2 = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem(WIDC_LISTBOX2));
	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem(WIDC_TEXT1));

	pWndList1->ResetContent();
	pWndList2->ResetContent();
	pWndText->ResetString();
}
BOOL CWndGuildCombatResult::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RESULT_POINT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildCombatResult::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildCombatResult::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildCombatResult::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildCombatResult::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildCombatResult::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndGuildCombatResult::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatResult::InsertGuildRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem(WIDC_LISTBOX1));
	pWndList->AddString(str);
}

void CWndGuildCombatResult::InsertPersonRate(CString str)
{
	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultRate.GetDlgItem(WIDC_LISTBOX2));
	pWndList->AddString(str);
}

void CWndGuildCombatResult::InsertLog(CString str)
{
	//	CWndListBox* pWndList = (CWndListBox*)(m_WndGuildCombatTabResultLog.GetDlgItem( WIDC_LISTBOX1 ));
	//	pWndList->AddString( str );

	CWndText* pWndText = (CWndText*)(m_WndGuildCombatTabResultLog.GetDlgItem(WIDC_TEXT1));
	pWndText->AddString(str);
	//	pWndList->AddString( str );

}


/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT
****************************************************/
CWndGuildCombatTabResultRate::CWndGuildCombatTabResultRate()
{
}
CWndGuildCombatTabResultRate::~CWndGuildCombatTabResultRate()
{
}
void CWndGuildCombatTabResultRate::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildCombatTabResultRate::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();



	MoveParentCenter();
}
BOOL CWndGuildCombatTabResultRate::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildCombatTabResultRate::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndGuildCombatTabResultRate::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

/****************************************************
  WndId : APP_GUILDCOMBAT_TAB_RESULT_LOG
****************************************************/
CWndGuildCombatTabResultLog::CWndGuildCombatTabResultLog()
{
}
CWndGuildCombatTabResultLog::~CWndGuildCombatTabResultLog()
{
}
void CWndGuildCombatTabResultLog::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildCombatTabResultLog::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	MoveParentCenter();
}
BOOL CWndGuildCombatTabResultLog::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_TAB_RESULT_LOG, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildCombatTabResultLog::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndGuildCombatTabResultLog::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		CTime time = CTime::GetCurrentTime();
		CString str;

		str.Format("GuildLog_%d%.2d%.2d.txt", time.GetYear(), time.GetMonth(), time.GetDay());

		CFileIO file;

		if (file.Open(str, _T("wt")) == FALSE)
			return FALSE;

		CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

		file.PutString(_T(pWndText->m_string));

		CString strTemp;
		strTemp.Format(prj.GetText(TID_GAME_GUILDCOMBAT_LOG_SAVE), str);

		g_WndMng.OpenMessageBox(strTemp);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



CWndGuildCombatRank_Person::CWndGuildCombatRank_Person()
{
	m_mTotalRanking.clear();
}
CWndGuildCombatRank_Person::~CWndGuildCombatRank_Person()
{
}
void CWndGuildCombatRank_Person::OnDraw(C2DRender* p2DRender)
{
}

void CWndGuildCombatRank_Person::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 0;
	rect.top = 0;
	//pWndTabCtrl->Create( WBS_NOFRAME, rect, this, 12 );

	pWndTabCtrl->SetButtonLength(100);

	m_WndGuildCombatTabClass_Tot.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS);
	m_WndGuildCombatTabClass_Mer.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS);
	m_WndGuildCombatTabClass_Mag.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS);
	m_WndGuildCombatTabClass_Acr.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS);
	m_WndGuildCombatTabClass_Ass.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_GUILDCOMBAT_RANKINGCLASS);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_TOOLTIP_LOG1);
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Tot;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.m_aJob[1].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mer;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	tabTabItem.pszText = prj.m_aJob[4].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Mag;
	pWndTabCtrl->InsertItem(2, &tabTabItem);

	tabTabItem.pszText = prj.m_aJob[2].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Acr;
	pWndTabCtrl->InsertItem(3, &tabTabItem);

	tabTabItem.pszText = prj.m_aJob[3].szName;
	tabTabItem.pWndBase = &m_WndGuildCombatTabClass_Ass;
	pWndTabCtrl->InsertItem(4, &tabTabItem);



	MoveParentCenter();
}
BOOL CWndGuildCombatRank_Person::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANK_P, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildCombatRank_Person::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndGuildCombatRank_Person::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndGuildCombatRank_Person::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndGuildCombatRank_Person::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndGuildCombatRank_Person::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_CLOSE)
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndGuildCombatRank_Person::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CWndGuildCombatRank_Person::InsertRank(int nJob, u_long	uidPlayer, int nPoint)
{
	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	GcRankInfo.nJob = nJob;
	GcRankInfo.uidPlayer = uidPlayer;
	m_mTotalRanking.insert(make_pair(nPoint, GcRankInfo));
}

void CWndGuildCombatRank_Person::DivisionList()
{
	if (m_mTotalRanking.size() <= 0)
		return;

	int nPoint;

	__GUILDCOMBAT_RANK_INFO GcRankInfo;
	CWndGuildCombatRank_Class* pRankTot;
	CWndGuildCombatRank_Class* pRank;


	pRankTot = &(m_WndGuildCombatTabClass_Tot);
	multimap< int, __GUILDCOMBAT_RANK_INFO >::reverse_iterator i;


	pRank = NULL;
	for (i = m_mTotalRanking.rbegin(); i != m_mTotalRanking.rend(); ++i)
	{
		nPoint = i->first;
		GcRankInfo = i->second;

		pRank = __GetJobKindWnd(GcRankInfo.nJob);

		if (pRank == NULL)
			continue;

		pRank->InsertRank(GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint);
		pRankTot->InsertRank(GcRankInfo.nJob, GcRankInfo.uidPlayer, nPoint);
	}
}

void CWndGuildCombatRank_Person::UpdateList()
{

}

void CWndGuildCombatRank_Person::UpdatePlayer(int nJob, u_long idPlayer)
{

}

CWndGuildCombatRank_Class* CWndGuildCombatRank_Person::__GetJobKindWnd(int nJob)
{
	switch (nJob)
	{
	case JOB_MERCENARY:
	case JOB_KNIGHT:
	case JOB_BLADE:
	case JOB_KNIGHT_MASTER:
	case JOB_BLADE_MASTER:
	case JOB_KNIGHT_HERO:
	case JOB_BLADE_HERO:
	case JOB_LORDTEMPLER_HERO:
	case JOB_STORMBLADE_HERO:
		return &m_WndGuildCombatTabClass_Mer;
		break;

	case JOB_ACROBAT:
	case JOB_JESTER:
	case JOB_RANGER:
	case JOB_JESTER_MASTER:
	case JOB_RANGER_MASTER:
	case JOB_JESTER_HERO:
	case JOB_RANGER_HERO:
	case JOB_WINDLURKER_HERO:
	case JOB_CRACKSHOOTER_HERO:
		return &m_WndGuildCombatTabClass_Acr;
		break;

	case JOB_ASSIST:
	case JOB_RINGMASTER:
	case JOB_BILLPOSTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_RINGMASTER_HERO:
	case JOB_BILLPOSTER_HERO:
	case JOB_FLORIST_HERO:
	case JOB_FORCEMASTER_HERO:
		return &m_WndGuildCombatTabClass_Ass;
		break;

	case JOB_MAGICIAN:
	case JOB_PSYCHIKEEPER:
	case JOB_ELEMENTOR:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_ELEMENTOR_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
	case JOB_ELEMENTOR_HERO:
	case JOB_MENTALIST_HERO:
	case JOB_ELEMENTORLORD_HERO:
		return &m_WndGuildCombatTabClass_Mag;
		break;

	default:
		break;
	}

	return NULL;
}

/****************************************************
  WndId : APP_GUILDCOMBAT_RANKINGCLASS
****************************************************/
CWndGuildCombatRank_Class::CWndGuildCombatRank_Class()
{
	m_nRate = 0;
	m_nOldRate = -1;
	m_nMax = 0;
	m_nSelect = -1;
}
CWndGuildCombatRank_Class::~CWndGuildCombatRank_Class()
{
}
void CWndGuildCombatRank_Class::OnDraw(C2DRender* p2DRender)
{
	DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	char szNum[8], szCount[8];

	sx = 8;
	sy = 35;

	CRect rc(sx, 5, sx + 330, 7);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	rc += CPoint(0, 20);
	p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(128, 0, 0, 0));

	int nBase = m_wndScrollBar.GetScrollPos();
	if (nBase < 0)
	{
		nBase = 0;
		TRACE("aa\n");
	}

	int nRanking = 0;
	int nOldRanking = -1;

	__GUILDCOMBAT_RANK_INFO2 GCRankInfo;
	__GUILDCOMBAT_RANK_INFO2 GCRankInfoMy;
	int	nMyRanking = 0;

	CString szName;
	CString szJob;

	BOOL	bMyRanking = FALSE;

	p2DRender->TextOut(sx + 4, 10, prj.GetText(TID_GAME_RATE), dwColor);
	p2DRender->TextOut(sx + 40, 10, prj.GetText(TID_GAME_NAME), dwColor);
	p2DRender->TextOut(sx + 180, 10, prj.GetText(TID_GAME_JOB), dwColor);
	p2DRender->TextOut(sx + 275, 10, prj.GetText(TID_GAME_POINT), dwColor);

	for (int k = 0; k < nBase; k++)
	{
		GCRankInfo = m_listRank[k];

		if (nOldRanking != GCRankInfo.nPoint)
			nRanking++;

		if (GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer)
		{
			bMyRanking = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking = nRanking;
		}

		nOldRanking = GCRankInfo.nPoint;
	}

	for (int i = nBase; i < nBase + MAX_GUILDCOMBAT_RANK_PER_PAGE; ++i)
	{
		if (i >= m_nMax)
			break;

		GCRankInfo = m_listRank[i];

		if (nOldRanking != GCRankInfo.nPoint)
			nRanking++;

		if (GCRankInfo.uidPlayer == g_pPlayer->m_idPlayer)
		{
			bMyRanking = TRUE;
			GCRankInfoMy = GCRankInfo;
			nMyRanking = nRanking;
		}

		sprintf(szNum, "%3d", nRanking);
		sprintf(szCount, "%2d", GCRankInfo.nPoint);

		if (m_nSelect >= 0 && i == m_nSelect)
		{
			rc.SetRect(sx, sy - 4, sx + 320, sy + 16);
			p2DRender->RenderFillRect(rc, D3DCOLOR_ARGB(64, 0, 0, 0));
		}

		if (i == 0)
			dwColor = D3DCOLOR_XRGB(200, 0, 0);
		else
			dwColor = D3DCOLOR_XRGB(0, 0, 0);

		if (nOldRanking != GCRankInfo.nPoint)
		{
			p2DRender->TextOut(sx + 4, sy, szNum, dwColor);
		}
		else
		{
			p2DRender->TextOut(sx + 5, sy, "   ", dwColor);
		}


#if __VER >= 11 // __SYS_PLAYER_DATA
		CString strName;
		const char* lpName = CPlayerDataCenter::GetInstance()->GetPlayerString(GCRankInfo.uidPlayer);
		if (lpName)
			strName = lpName;
#endif // __SYS_PLAYER_DATA

		if (i == 0)
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 40, sy, strName, D3DCOLOR_XRGB(200, 0, 0));
#else // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 40, sy, GCRankInfo.strName, D3DCOLOR_XRGB(200, 0, 0));
#endif // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 180, sy, GCRankInfo.strJob, D3DCOLOR_XRGB(200, 0, 0));
		}
		else
		{
#if __VER >= 11 // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 40, sy, strName, dwColor);
#else // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 40, sy, GCRankInfo.strName, dwColor);
#endif // __SYS_PLAYER_DATA
			p2DRender->TextOut(sx + 180, sy, GCRankInfo.strJob, dwColor);
		}

		p2DRender->TextOut(sx + 275, sy, szCount, dwColor);

		sy += 18;

		nOldRanking = GCRankInfo.nPoint;
	}


	if (bMyRanking)
	{
		sprintf(szNum, "%3d", nMyRanking);
		sprintf(szCount, "%2d", GCRankInfoMy.nPoint);

		dwColor = D3DCOLOR_XRGB(0, 0, 255);

		p2DRender->TextOut(sx + 4, 250, szNum, dwColor);

		p2DRender->TextOut(sx + 40, 250, GCRankInfoMy.strName, dwColor);
		p2DRender->TextOut(sx + 180, 250, GCRankInfoMy.strJob, dwColor);
		p2DRender->TextOut(sx + 275, 250, szCount, dwColor);
	}
}

int CWndGuildCombatRank_Class::GetSelectIndex(const CPoint& point)
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y - 32) / 18;

	if (0 <= nIndex && nIndex < MAX_GUILDCOMBAT_RANK_PER_PAGE)
	{
		int nSelect = nBase + nIndex;
		if (0 <= nSelect && nSelect < m_nMax)
			return nSelect;
	}
	return -1;
}
BOOL CWndGuildCombatRank_Class::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

void CWndGuildCombatRank_Class::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nMax <= 0)
		return;

	int nSelect = GetSelectIndex(point);
	if (nSelect != -1)
		m_nSelect = nSelect;
}

void CWndGuildCombatRank_Class::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CRect rect = GetWindowRect();
	rect.top += 30;
	rect.bottom -= 80;
	rect.left = rect.right - 30;
	rect.right -= 30;

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_GUILDCOMBAT_RANK_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}

	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);


	MoveParentCenter();
}
BOOL CWndGuildCombatRank_Class::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILDCOMBAT_RANKINGCLASS, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildCombatRank_Class::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndGuildCombatRank_Class::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndGuildCombatRank_Class::InsertRank(int nJob, u_long uidPlayer, int nPoint)
{
	if (m_nMax >= MAX_GUILDCOMBAT_RANK)
		return;

#if __VER >= 11 // __SYS_PLAYER_DATA
	m_listRank[m_nMax].strName = CPlayerDataCenter::GetInstance()->GetPlayerString(uidPlayer);
#else	// __SYS_PLAYER_DATA
	m_listRank[m_nMax].strName = prj.GetPlayerString(uidPlayer);
#endif	// __SYS_PLAYER_DATA
	m_listRank[m_nMax].strJob = prj.m_aJob[nJob].szName;
	m_listRank[m_nMax].uidPlayer = uidPlayer;
	m_listRank[m_nMax].nPoint = nPoint;

	m_nMax++;

	if (m_nMax > 0)
	{
		m_wndScrollBar.SetScrollRange(0, m_nMax);
		if (m_nMax < MAX_GUILDCOMBAT_RANK_PER_PAGE)
			m_wndScrollBar.SetScrollPage(m_nMax);
		else
			m_wndScrollBar.SetScrollPage(MAX_GUILDCOMBAT_RANK_PER_PAGE);
	}
	else
	{
		m_wndScrollBar.SetScrollRange(0, 1);
		m_wndScrollBar.SetScrollPage(1);
	}
}


/****************************************************
  WndId : APP_FONTEDIT
****************************************************/
CWndFontEdit::CWndFontEdit()
{
	m_pTexture = NULL;
	m_pWndText = NULL;
	memset(m_ColorRect, 0, sizeof(CRect) * 3);
}
CWndFontEdit::~CWndFontEdit()
{
}
void CWndFontEdit::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "yellowbuttten.tga"), 0xffff00ff, TRUE);

	m_ColorRect[0].left = 38;
	m_ColorRect[0].top = 46;
	m_ColorRect[0].right = 156;
	m_ColorRect[0].bottom = 63;

	m_ColorRect[1].left = 38;
	m_ColorRect[1].top = 68;
	m_ColorRect[1].right = 156;
	m_ColorRect[1].bottom = 83;

	m_ColorRect[2].left = 38;
	m_ColorRect[2].top = 89;
	m_ColorRect[2].right = 156;
	m_ColorRect[2].bottom = 103;

	m_ColorScrollBar[0] = 0;
	m_ColorScrollBar[1] = 0;
	m_ColorScrollBar[2] = 0;

	m_bLButtonClick = FALSE;

	m_fColor[0] = m_fColor[1] = m_fColor[2] = 0.0f;

	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);


	MoveParentCenter();
}
BOOL CWndFontEdit::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_FONTEDIT, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndFontEdit::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndFontEdit::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (m_pWndText == NULL)
		return FALSE;

	CWndButton* pWndCheck = NULL;

	switch (nID)
	{
	case WIDC_CHECK1:
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);

		if (pWndCheck->GetCheck())
		{
			m_pWndText->BlockSetStyle(ESSTY_BOLD);
			//				m_pWndText->BlockInsert( "#b", "#nb" );
		}
		else
		{
			m_pWndText->BlockClearStyle(ESSTY_BOLD);
			//				m_pWndText->BlockDelete( "#b", "#nb" );
		}
		break;
	case WIDC_CHECK2:
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);

		if (pWndCheck->GetCheck())
		{
			m_pWndText->BlockSetStyle(ESSTY_UNDERLINE);
			//				m_pWndText->BlockInsert( "#u", "#nu" );
		}
		else
		{
			m_pWndText->BlockClearStyle(ESSTY_UNDERLINE);
			//				m_pWndText->BlockDelete( "#u", "#nu" );
		}
		break;
	case WIDC_CHECK3:
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);

		if (pWndCheck->GetCheck())
		{
			m_pWndText->BlockSetStyle(ESSTY_STRIKETHROUGH);
			//				m_pWndText->BlockInsert( "#s", "#ns" );
		}
		else
		{
			m_pWndText->BlockClearStyle(ESSTY_STRIKETHROUGH);
			//				m_pWndText->BlockDelete( "#s", "#ns" );
		}
		break;
	case WIDC_CHECK4:
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);

		if (pWndCheck->GetCheck())
		{
			DWORD dwR, dwG, dwB, dwColor;
			dwR = (DWORD)(m_fColor[0] * 255);
			dwG = (DWORD)(m_fColor[1] * 255);
			dwB = (DWORD)(m_fColor[2] * 255);

			dwColor = D3DCOLOR_XRGB(dwR, dwG, dwB);

			CString str;
			str.Format("#c%x", dwColor);

			m_pWndText->BlockSetColor(dwColor);
			//				m_pWndText->BlockInsert( str, "#nc" );
		}
		else
		{
			m_pWndText->BlockSetColor(0xff000000);
			//				m_pWndText->BlockDelete( "#c", "#nc" );
		}

		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndFontEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonClick = FALSE;
}
void CWndFontEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonClick = TRUE;
}
void CWndFontEdit::OnMouseWndSurface(CPoint point)
{
	if (g_pPlayer == NULL)
		return;

	CRect rect = CRect(17, 17, 186, 148);

	if (!rect.PtInRect(point))
		m_bLButtonClick = FALSE;

	for (int i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];

		DrawRect.top -= 22;
		DrawRect.bottom -= 22;

		if (DrawRect.PtInRect(point) && m_bLButtonClick)
		{
			point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;

			LONG Width = DrawRect.right - DrawRect.left;
			LONG Pos = point.x - DrawRect.left;

			FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

			D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
			D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
			D3DXVECTOR2 vec3;

			D3DXVec2Lerp(&vec3, &vec1, &vec2, p);

			m_fColor[i] = vec3.x;

			m_ColorScrollBar[i].x = point.x;
		}
	}
}

void CWndFontEdit::OnDraw(C2DRender* p2DRender)
{
	for (int i = 0; i < 3; i++)
	{
		CPoint pt = CPoint(m_ColorScrollBar[i].x - (m_pTexture->m_size.cx / 2), m_ColorScrollBar[i].y);

		m_pTexture->Render(p2DRender, pt);
	}
	/*
		for( int j=0; j<3; j++ )
		{
			if( m_ColorScrollBar[j].x != m_OriginalColorScrollBar[j].x )
				m_pTexture->Render( p2DRender, CPoint( m_OriginalColorScrollBar[j].x - ( m_pTexture->m_size.cx / 2 ), m_OriginalColorScrollBar[j].y ), 160 );
		}
	*/

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);

	DWORD dwR, dwG, dwB;
	dwR = (DWORD)(m_fColor[0] * 255);
	dwG = (DWORD)(m_fColor[1] * 255);
	dwB = (DWORD)(m_fColor[2] * 255);

	p2DRender->RenderFillRect(lpFace->rect, D3DCOLOR_XRGB(dwR, dwG, dwB));
}

void CWndFontEdit::ReSetBar(FLOAT r, FLOAT g, FLOAT b)
{
	FLOAT fR = ((r - 0.0f) / (1.0f - 0.0f)) * 100.0f;
	FLOAT fG = ((g - 0.0f) / (1.0f - 0.0f)) * 100.0f;
	FLOAT fB = ((b - 0.0f) / (1.0f - 0.0f)) * 100.0f;

	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top - 20;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top - 20;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top - 20;
}

#if __VER >= 8 // __CSC_VER8_3

CWndBuffStatus::CWndBuffStatus()
{
	m_BuffIconViewOpt = g_Option.m_BuffStatusMode;
}

CWndBuffStatus::~CWndBuffStatus()
{
}

void CWndBuffStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	this->DelWndStyle(WBS_CAPTION);

	m_wndTitleBar.SetVisible(FALSE);

	SetBuffIconInfo();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CPoint point((rectRoot.right - rectRoot.left) / 3, (rectRoot.bottom - rectRoot.top) / 3);
	Move(point);
}
BOOL CWndBuffStatus::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BUFF_STATUS, WBS_NOFOCUS, CPoint(0, 0), pWndParent);
}

BOOL CWndBuffStatus::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

BOOL CWndBuffStatus::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndBuffStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->m_pWndFocus = this;
}

void CWndBuffStatus::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!GetHitTestResult())
	{
		if (g_WndMng.m_pWndWorld)
		{
			CRect rect = GetWindowRect(TRUE);
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDown(nFlags, p);
			this->m_pWndFocus = g_WndMng.m_pWndWorld;
		}
	}
}

void CWndBuffStatus::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndBuffStatus::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!GetHitTestResult())
	{
		if (g_WndMng.m_pWndWorld)
		{
			CRect rect = GetWindowRect(TRUE);
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDown(nFlags, p);
		}
	}
}

void CWndBuffStatus::OnMouseWndSurface(CPoint point)
{
}

void CWndBuffStatus::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (GetHitTestResult())
	{
		if (m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if (m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if (g_WndMng.m_pWndWorld)
		{
			CRect rect = GetWindowRect(TRUE);
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bLButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnLButtonDblClk(nFlags, p);
		}
	}
}

void CWndBuffStatus::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if (GetHitTestResult())
	{
		if (m_BuffIconViewOpt == 0)
			m_BuffIconViewOpt = 1;
		else if (m_BuffIconViewOpt == 1)
			m_BuffIconViewOpt = 0;
		g_Option.m_BuffStatusMode = m_BuffIconViewOpt;
		SetBuffIconInfo();
	}
	else
	{
		if (g_WndMng.m_pWndWorld)
		{
			CRect rect = GetWindowRect(TRUE);
			CPoint p;
			p.x = point.x + rect.left;
			p.y = point.y + rect.top;
			g_WndMng.m_pWndWorld->m_bRButtonDown = TRUE;
			g_WndMng.m_pWndWorld->OnRButtonDblClk(nFlags, p);
		}
	}
}

BOOL CWndBuffStatus::GetHitTestResult()
{
	BOOL rtn_val = FALSE;
	RECT rectHittest;
	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);
	BUFFICON_INFO buffinfo;

	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for (auto& i : g_pPlayer->m_buffs.m_mapBuffs)
	{
		IBuff* pBuff = i.second;
		if (pBuff->GetType() == BUFF_SKILL)
		{
			buffinfo = *it;
			SetRect(&rectHittest, buffinfo.pt.x, buffinfo.pt.y, buffinfo.pt.x + 30, buffinfo.pt.y + 30);
			ClientToScreen(&rectHittest);
			if (PtInRect(&rectHittest, ptMouse))
				rtn_val = TRUE;
			it++;
		}
	}
	return rtn_val;
}

void CWndBuffStatus::SetBuffIconInfo()
{
	BUFFICON_INFO buffinfo;
	int x = 0;
	int y = 0;
	int i;
	CRect rect;
	rect = GetWindowRect(TRUE);

	if (!m_pBuffIconInfo.empty())
		m_pBuffIconInfo.clear();

	if (m_BuffIconViewOpt == 0)
	{
		for (i = 0; i < MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint(x, y);
			m_pBuffIconInfo.push_back(buffinfo);
			x += 30;
			if (((i + 1) % 7) == 0)
			{
				x = 0;
				y += 30;
			}
		}
		rect.bottom = 150 + rect.top;
		rect.right = 263 + rect.left;
	}
	else if (m_BuffIconViewOpt == 1)
	{
		for (i = 0; i < MAX_SKILLBUFF_COUNT; i++)
		{
			buffinfo.pt = CPoint(x, y);
			m_pBuffIconInfo.push_back(buffinfo);
			y += 30;
			if (((i + 1) % 7) == 0)
			{
				y = 0;
				x += 30;
			}
		}
		//widht 54, heigth = 238
		rect.bottom = 263 + rect.top;
		rect.right = 150 + rect.left;
	}
	SetWndRect(rect);
	AdjustWndBase();
}

void CWndBuffStatus::PaintFrame(C2DRender* p2DRender)
{
}

void CWndBuffStatus::OnDraw(C2DRender* p2DRender)
{
	if (g_pPlayer == NULL)
		return;

	CPoint ptMouse = GetMousePoint();
	ClientToScreen(&ptMouse);
	BUFFICON_INFO buffinfo;

	std::list<BUFFICON_INFO>::iterator it = m_pBuffIconInfo.begin();
	for (auto& i : g_pPlayer->m_buffs.m_mapBuffs)
	{
		IBuff* pBuff = i.second;
		if (pBuff->GetType() == BUFF_SKILL)
		{
			buffinfo = *it;
			RenderBuffIcon(p2DRender, pBuff, TRUE, &buffinfo, ptMouse);
			it++;
		}
	}
}

void CWndBuffStatus::RenderBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse)
{
	int nTexture;
	RECT rectHittest;
	multimap< DWORD, BUFFSKILL >::iterator iter;

	if (bPlayer)
		nTexture = 0;
	else
		nTexture = 1;

	ItemProp* pItem = NULL;

	WORD wID = pBuff->GetId();
	if (pBuff->GetType() == BUFF_SKILL)
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].find(pBuff->GetId());

		if (iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[0].end())
			return;

		if (bPlayer)
			pItem = prj.GetSkillProp(pBuff->GetId());
		else
			pItem = prj.GetPartySkill(pBuff->GetId());
	}
	else
	{
		iter = ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].find(pBuff->GetId());

		if (iter == ((CWndWorld*)g_WndMng.m_pWndWorld)->m_pBuffTexture[2].end())
			return;

		pItem = prj.GetItemProp(wID);
	}

	multimap< DWORD, BUFFSKILL >::value_type* pp;
	pp = &(*iter);

	ASSERT(pItem);
	if (pp->second.m_pTexture == NULL)
		return;

	BOOL bFlash = FALSE;
	DWORD dwOddTime = 0;

	if (pBuff->GetTotal() > 0)
	{
		dwOddTime = pBuff->GetTotal() - (g_tmCurrent - pBuff->GetInst());
		bFlash = (dwOddTime < 20 * 1000);

		if (pBuff->GetTotal() < (g_tmCurrent - pBuff->GetInst()))
			dwOddTime = 0;
	}

	D3DXCOLOR color;

	if (bFlash)
	{
		if (pp->second.m_bFlsh == TRUE)
		{
			pp->second.m_nAlpha += 6;

			if (pp->second.m_nAlpha > 202)
			{
				pp->second.m_nAlpha = 202;
				pp->second.m_bFlsh = FALSE;
			}
		}
		else
		{
			pp->second.m_nAlpha -= 6;

			if (pp->second.m_nAlpha < 64)
			{
				pp->second.m_nAlpha = 64;
				pp->second.m_bFlsh = TRUE;
			}
		}

		if (pItem->nEvildoing < 0)
			color = D3DCOLOR_ARGB(pp->second.m_nAlpha, 255, 120, 255);
		else
			color = D3DCOLOR_ARGB(pp->second.m_nAlpha, 255, 255, 255);

		p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1.0f, 1.0f, color);
	}
	else
	{
		if (pItem->nEvildoing < 0)
			color = D3DCOLOR_ARGB(202, 255, 120, 255);
		else
			color = D3DCOLOR_ARGB(202, 255, 255, 255);

		p2DRender->RenderTexture2(pInfo->pt, pp->second.m_pTexture, 1.0f, 1.0f, color);
	}

	SetRect(&rectHittest, pInfo->pt.x, pInfo->pt.y, pInfo->pt.x + 28, pInfo->pt.y + 28);
	ClientToScreen(&rectHittest);

	CEditString strEdit;
	CString strLevel;
	strLevel.Format("   Lvl %d", pBuff->GetLevel());

	if (pItem->dwItemRare == 102)
	{
		strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
		strEdit.AddString(strLevel, D3DCOLOR_XRGB(0, 93, 0), ESSTY_BOLD);
	}
	else if (pItem->dwItemRare == 103)
	{
		strEdit.AddString(pItem->szName, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
		strEdit.AddString(strLevel, D3DCOLOR_XRGB(182, 0, 255), ESSTY_BOLD);
	}
	else
	{
		strEdit.AddString(pItem->szName, 0xff2fbe6d, ESSTY_BOLD);
		strEdit.AddString(strLevel, 0xff2fbe6d, ESSTY_BOLD);
	}

	CString str;

	if (pBuff->GetTotal() > 0)
	{
		CTimeSpan ct((long)(dwOddTime / 1000.0f));

		if (ct.GetDays() != 0)
		{
			str.Format("\n%.2d:%.2d:%.2d:%.2d", static_cast<int>(ct.GetDays()), ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
		}
		else
		{
			if (ct.GetHours() >= 1)
				str.Format("\n%.2d:%.2d:%.2d", ct.GetHours(), ct.GetMinutes(), ct.GetSeconds());
			else
				str.Format("\n%.2d:%.2d", ct.GetMinutes(), ct.GetSeconds());
		}
		RenderOptBuffTime(p2DRender, pInfo->pt, ct, D3DCOLOR_XRGB(255, 255, 255));
	}

	if (PtInRect(&rectHittest, ptMouse))
	{
		BOOL bPutDescription = TRUE;
		if (pBuff->GetType() == BUFF_SKILL &&
			pBuff->GetId() != SI_RIG_MASTER_BLESSING &&
			pBuff->GetId() != SI_ASS_CHEER_STONEHAND &&
			pBuff->GetId() != SI_MAG_EARTH_LOOTING &&
			pBuff->GetId() != SI_ASS_HEAL_PREVENTION &&
			pBuff->GetId() != SI_FLO_CHEER_PATTERS &&
			pBuff->GetId() != SI_STORM_DOUBLE_HOLDINGSTORM
			)
		{
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pItem->dwSubDefine, pBuff->GetLevel());
			if (pAddSkillProp)
			{
				if (g_WndMng.PutDestParam(NULL, pAddSkillProp, &strEdit))
					bPutDescription = FALSE;
			}
		}

		if (bPutDescription && strlen(pItem->szCommand))
		{
			CString strTemp;
			strTemp.Format("\n%s", pItem->szCommand);
			strEdit.AddString(strTemp);
		}
	}

	strEdit.AddString(str);

	g_toolTip.PutToolTip(wID, strEdit, rectHittest, ptMouse, 1);

	++pInfo->nCount;
}

void CWndBuffStatus::RenderOptBuffTime(C2DRender* p2DRender, CPoint& point, CTimeSpan& ct, DWORD dwColor)
{
	CString str;
	int nTime[3];
	nTime[0] = (int)(ct.GetTotalSeconds());
	nTime[1] = (int)(ct.GetTotalMinutes());
	nTime[2] = (int)(ct.GetTotalHours());

	if (nTime[0] > 0 && nTime[0] <= 59)
		str.Format("%ds", nTime[0]);

	else if (nTime[1] > 0 && nTime[1] <= 59)
		str.Format("%dm", nTime[1]);

	else if (nTime[2] > 0 && nTime[2] <= 23)
		str.Format("%dh", nTime[2]);

	else if (nTime[2] > 23)
		str.Format("%dd", nTime[2] / 24);

	CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontWorld);
	p2DRender->TextOut((point.x + 14) - size.cx / 2, (point.y + 28) - size.cy / 2 - 6, str, dwColor, 200);
	p2DRender->SetFont(pOldFont);
}

#endif //__CSC_VER8_3

#if __VER >= 9 // __CSC_VER9_1
CWndMixJewel::CWndMixJewel()
	: m_pItemSun(NULL)
	, m_pItemMoon(NULL)
	, wndCtrlSun(NULL)
	, wndCtrlMoon(NULL)
	, pTexSun(NULL)
	, pTexMoon(NULL)
{

}

CWndMixJewel::~CWndMixJewel()
{
	if (m_pItemSun)
	{
		m_pItemSun->SetExtra(0);
		m_pItemSun = NULL;
	}

	if (m_pItemMoon)
	{
		m_pItemMoon->SetExtra(0);
		m_pItemMoon = NULL;
	}
}
void CWndMixJewel::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();

	wndCtrlSun = GetWndCtrl(WIDC_STATIC1);
	wndCtrlMoon = GetWndCtrl(WIDC_STATIC2);

	ItemProp* pItemProp = prj.GetItemProp(II_GEN_MAT_ORICHALCUM01);
	if (pItemProp)
		pTexSun = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
	pItemProp = prj.GetItemProp(II_GEN_MAT_MOONSTONE);
	if (pItemProp)
		pTexMoon = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);

	UpdateButtonState(FALSE);
	SetDescription(NULL);
}

void CWndMixJewel::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SmeltMixJewel.inc")));
	szChar = scanner.m_pProg;

	CWndText* m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (m_pText && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}
BOOL CWndMixJewel::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_MIXJEWEL, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndMixJewel::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!pWndFrame)
		return FALSE;

	if (pWndFrame->GetWndId() != APP_INVENTORY)
	{
		SetForbid(TRUE);
		return FALSE;
	}

	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	SetJewel(pTempElem);

	return TRUE;
}
void CWndMixJewel::OnDraw(C2DRender* p2DRender)
{
	// Icons
	if (pTexSun)
	{
		int nAlphaBlend = m_pItemSun != NULL ? 255 : 75;
		pTexSun->Render(p2DRender, CPoint(wndCtrlSun->rect.left, wndCtrlSun->rect.top), nAlphaBlend);
	}

	if (pTexMoon)
	{
		int nAlphaBlend = m_pItemMoon != NULL ? 255 : 75;
		pTexMoon->Render(p2DRender, CPoint(wndCtrlMoon->rect.left, wndCtrlMoon->rect.top), nAlphaBlend);
	}

	// Text
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

	CString str;
	CSize size;

	if (m_pItemSun)
	{
		str.Format("%d", m_pItemSun->GetExtra());
		size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(wndCtrlSun->rect.right - size.cx - 2, wndCtrlSun->rect.bottom - size.cy, str, 0xffeaebdd);
	}

	if (m_pItemMoon)
	{
		str.Format("%d", m_pItemMoon->GetExtra());
		size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(wndCtrlMoon->rect.right - size.cx - 2, wndCtrlMoon->rect.bottom - size.cy, str, 0xffeaebdd);
	}

	p2DRender->SetFont(pOldFont);
}
void CWndMixJewel::UpdateButtonState(BOOL bEnabled)
{
	if (!m_pItemSun || m_pItemSun->GetExtra() < 5)
		bEnabled = FALSE;
	if (!m_pItemMoon || m_pItemMoon->GetExtra() < 5)
		bEnabled = FALSE;

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
	pButton->EnableWindow(bEnabled);

}
BOOL CWndMixJewel::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_START)
	{
		if (m_pItemSun && m_pItemSun->GetExtra() >= 5 && m_pItemMoon && m_pItemMoon->GetExtra() >= 5)
		{
			g_DPlay.SendUltimateMakeItem(m_pItemSun->m_dwObjId, m_pItemMoon->m_dwObjId);
			UpdateButtonState(FALSE);
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndMixJewel::ReceiveResult(BOOL bResult)
{
	if (bResult)
	{
		if (m_pItemSun)
		{
			int nNewSunExtra = m_pItemSun->GetExtra() - 5;
			if (nNewSunExtra > 0)
				m_pItemSun->SetExtra(nNewSunExtra);
			else
				m_pItemSun = NULL;
		}

		if (m_pItemMoon)
		{
			int nNewMoonExtra = m_pItemMoon->GetExtra() - 5;
			if (nNewMoonExtra > 0)
				m_pItemMoon->SetExtra(nNewMoonExtra);
			else
				m_pItemMoon = NULL;
		}

		g_WndMng.PutString(prj.GetText(TID_GAME_MIXJEWEL_SUCCESS));
	}

	UpdateButtonState(TRUE);
}
void CWndMixJewel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (wndCtrlSun->rect.PtInRect(point))
	{
		if (m_pItemSun)
		{
			m_pItemSun->SetExtra(0);
			m_pItemSun = NULL;
		}
	}
	else if (wndCtrlMoon->rect.PtInRect(point))
	{
		if (m_pItemMoon)
		{
			m_pItemMoon->SetExtra(0);
			m_pItemMoon = NULL;
		}
	}

	UpdateButtonState(TRUE);
}
void CWndMixJewel::SetJewel(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	if (pItemProp->dwID == II_GEN_MAT_ORICHALCUM01)
	{
		if (m_pItemSun)
		{
			m_pItemSun->SetExtra(0);
			m_pItemSun = NULL;
		}

		m_pItemSun = pItemElem;
		m_pItemSun->SetExtra(m_pItemSun->m_nItemNum);
	}
	else if (pItemProp->dwID == II_GEN_MAT_MOONSTONE)
	{
		if (m_pItemMoon)
		{
			m_pItemMoon->SetExtra(0);
			m_pItemMoon = NULL;
		}

		m_pItemMoon = pItemElem;
		m_pItemMoon->SetExtra(m_pItemMoon->m_nItemNum);
	}

	UpdateButtonState(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndExtraction Class
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndExtraction::CWndExtraction()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
}

CWndExtraction::~CWndExtraction()
{
}

void CWndExtraction::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

void CWndExtraction::OnDraw(C2DRender* p2DRender)
{
	ItemProp* pItemProp;
	CTexture* pTexture;

	if (m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
		if (pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			if (pTexture != NULL)
				pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
		}
	}
}

void CWndExtraction::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if (::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("ButOk2.bmp")), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	SetDescription(NULL);

	MoveParentCenter();
}

BOOL CWndExtraction::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_EXTRACTION, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndExtraction::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndExtraction::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndExtraction::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndExtraction::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndExtraction::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		if (m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
		}
	}
}

BOOL CWndExtraction::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	CRect rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pTempElem))
			return FALSE;

		if (m_pItemElem == NULL)
		{
			ItemProp* pItemProp = pTempElem->GetProp();
			if (pItemProp && pItemProp->dwItemKind1 == IK1_WEAPON)
			{
				if ((pItemProp->dwReferStat1 == WEAPON_GENERAL && pItemProp->dwLimitLevel1 >= 60) || pItemProp->dwReferStat1 == WEAPON_UNIQUE)
				{
					m_pItemElem = pTempElem;
					m_pEItemProp = m_pItemElem->GetProp();
					m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
					CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
					pButton->EnableWindow(TRUE);
				}
			}
		}
	}
	return TRUE;
}

void CWndExtraction::SetWeapon(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	if (m_pItemElem == NULL && pItemElem != NULL)
	{
		ItemProp* pProp = pItemElem->GetProp();

		if (pProp->dwItemKind1 == IK1_WEAPON)
		{
			if ((pProp->dwReferStat1 == WEAPON_GENERAL && pProp->dwLimitLevel1 >= 60) || pProp->dwReferStat1 == WEAPON_UNIQUE)
			{
				m_pItemElem = pItemElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
			}
		}
	}
}

BOOL CWndExtraction::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_START)
	{

		if (m_pItemElem != NULL)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);

			g_DPlay.SendUltimateMakeGem(m_pItemElem->m_dwObjId);
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndExtraction::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SmeltExtraction.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

void CWndExtraction::ReceiveResult(int result, int nCount)
{













	switch (result)
	{
	case CUltimateWeapon::ULTIMATE_SUCCESS:
	{
		CString message;
		ItemProp* pItemProp = prj.GetItemProp(prj.m_UltimateWeapon.GetGemKind(m_pEItemProp->dwLimitLevel1));
		message.Format(prj.GetText(TID_GAME_EXTRACTION_SUCCESS), pItemProp->szName, nCount);
		g_WndMng.OpenMessageBox(message);
		m_pItemElem = NULL;
	}
	break;
	case CUltimateWeapon::ULTIMATE_FAILED:
	case CUltimateWeapon::ULTIMATE_CANCEL:
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_EXTRACTION_FAILED));
		Destroy();
		break;
	case CUltimateWeapon::ULTIMATE_INVENTORY:
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_EXTRACTION_ERROR));

		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(TRUE);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSmeltJewel Class
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndSmeltJewel::CWndSmeltJewel()
{
	m_pMoonstone = NULL;
	m_pTextureMoon = NULL;

	m_pMainItem = NULL;
	m_pJewelElem = NULL;
	m_pItemElem = NULL;
	m_pText = NULL;
	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_objJewelId = -1;

	for (int i = 0; i < 5; i++)
	{
		m_nJewelSlot[i] = -1;
		m_dwJewel[i] = -1;
	}
	m_fRotate = 0.0f;
	m_fAddRot = 2.0f;

	m_nStatus = 0;
	m_nCount = 0;
	m_nDelay = 25;
	m_nAlpha = 0;
	m_nEyeYPos = 0.0f;
	m_bFlash = FALSE;
}

CWndSmeltJewel::~CWndSmeltJewel()
{

	//	if(m_pMainItem != NULL)
	//		SAFE_DELETE(m_pMainItem);
}

void CWndSmeltJewel::OnDestroy()
{
	if (m_pJewelElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pJewelElem))
			m_pJewelElem->SetExtra(0);
	}
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}

	if (m_pMoonstone)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pMoonstone))
			m_pMoonstone->SetExtra(0);
	}

}

void CWndSmeltJewel::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_nInfo[0] = WIDC_JEWEL_SLOT_INFO1;
	m_nInfo[1] = WIDC_JEWEL_SLOT_INFO2;
	m_nInfo[2] = WIDC_JEWEL_SLOT_INFO3;
	m_nInfo[3] = WIDC_JEWEL_SLOT_INFO4;
	m_nInfo[4] = WIDC_JEWEL_SLOT_INFO5;

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButton[3] = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	pWndButton[4] = (CWndButton*)GetDlgItem(WIDC_BUTTON5);

	ItemProp* pItemProp = prj.GetItemProp(II_GEN_MAT_MOONSTONE);
	if (pItemProp)
		m_pTextureMoon = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);


	m_nJewelSlot[0] = WIDC_STATIC1;
	m_nJewelSlot[1] = WIDC_STATIC2;
	m_nJewelSlot[2] = WIDC_STATIC3;
	m_nJewelSlot[3] = WIDC_STATIC4;
	m_nJewelSlot[4] = WIDC_STATIC5;

	for (int i = 0; i < 5; i++)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem(m_nJewelSlot[i]);
		pStatic->EnableWindow(FALSE);
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
	if (::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("ButOk2.bmp")), TRUE);

	pButton->EnableWindow(FALSE);

	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	SetDescription(NULL);

	MoveParentCenter();
}


BOOL CWndSmeltJewel::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_JEWEL, 0, CPoint(0, 0), pWndParent);
}


BOOL CWndSmeltJewel::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSmeltJewel::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndSmeltJewel::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndSmeltJewel::OnLButtonDown(UINT nFlags, CPoint point)
{
}

BOOL CWndSmeltJewel::Process()
{
	UpdateButtons();

	if (m_pItemElem != NULL)
	{
		CWndButton* pButton;
		pButton = (CWndButton*)GetDlgItem(WIDC_START);

		if (m_nStatus != 1)
		{
			if ((m_nUsableSlot >= 0 && m_nUsableSlot < 5) && m_dwJewel[m_nUsableSlot] != -1)
				pButton->EnableWindow(TRUE);
			else
				pButton->EnableWindow(FALSE);
		}
		else
			pButton->EnableWindow(FALSE);
	}

	if (m_nStatus == 1) //Start!
	{
		if (m_nStatus == 1)
		{
			if (m_nCount > m_nDelay)
			{
				m_fAddRot += 4.0f;
				//( m_fRotate < 1 ) ? m_fRotate = 1 : m_fRotate;

				if (m_nDelay <= 25 && m_nDelay > 16)
					m_bFlash = TRUE;

				if (m_nDelay < 10)
				{
					m_nEyeYPos -= 0.4f;
					m_nAlpha += 18;
					(m_nAlpha > 230) ? m_nAlpha = 230 : m_nAlpha;
				}

				m_nDelay -= 1;
				if (m_nDelay < 0)
				{
					m_nStatus = 0;
					m_nDelay = 25;
					m_fRotate = 0.0f;
					m_fAddRot = 2.0f;
					m_nAlpha = 0;
					m_nEyeYPos = 0.0f;

					if (m_pItemElem != NULL && m_objJewelId != -1)
						g_DPlay.SendUltimateSetGem(m_pItemElem->m_dwObjId, m_objJewelId);
				}

				m_nCount = 0;
			}
			m_nCount++;
		}
	}

	return TRUE;
}

void CWndSmeltJewel::OnDraw(C2DRender* p2DRender)
{
	if (m_pTextureMoon)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT1);
		if (m_pMoonstone)
			m_pTextureMoon->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
		else
			m_pTextureMoon->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top), 50);
	}

	if (!m_pItemElem)
		return;

	D3DXVECTOR3 vEyePt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);

	if (m_pItemElem->GetProp()->dwItemKind3 == IK3_AXE ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_SWD)
	{
		if (m_pItemElem->GetProp()->dwHanded == HD_ONE)
		{
			vEyePt.x = 0.0f;
			vEyePt.y = 3.0f;
			vEyePt.z = 0.0f;

			vLookatPt.x = 0.6f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
		else if (m_pItemElem->GetProp()->dwHanded == HD_TWO)
		{
			vEyePt.x = 1.0f;
			vEyePt.y = 5.0f;
			vEyePt.z = 0.0f;

			vLookatPt.x = 1.2f;
			vLookatPt.y = -0.2f;
			vLookatPt.z = 0.0f;
		}
	}
	else if (m_pItemElem->GetProp()->dwItemKind3 == IK3_YOYO ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_BOW)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;

		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if (m_pItemElem->GetProp()->dwItemKind3 == IK3_WAND)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 3.0f;
		vEyePt.z = 0.0f;

		vLookatPt.x = 0.4f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}
	else if (m_pItemElem->GetProp()->dwItemKind3 == IK3_CHEERSTICK ||
		m_pItemElem->GetProp()->dwItemKind3 == IK3_STAFF)
	{
		vEyePt.x = 0.0f;
		vEyePt.y = 4.0f;
		vEyePt.z = 0.0f;

		vLookatPt.x = 0.01f;
		vLookatPt.y = -0.2f;
		vLookatPt.z = 0.0f;
	}

	if (m_nStatus == 1)
		vEyePt.y += m_nEyeYPos;

	m_fRotate += m_fAddRot;

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
	VIEWPORT tmp;
	tmp.m_fRot = m_fRotate;
	tmp.vecPos = vEyePt;
	tmp.vecLookAt = vLookatPt;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.5f, 1.5f, 1.5f);

	g_WndMng.RenderObjectViewport(p2DRender, m_pMainItem, NULL, lpFace, &tmp, GetWndId());

	//Jewel Rendering
	CTexture* pTexture;
	ItemProp* pItemProp;
	if (m_pItemElem != NULL)
	{
		for (int i = 0; i < 5; i++)
		{
			CString DstText = "";

			if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND01 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ01)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DST_HP_MAX));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND02 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ02)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DST_ATKPOWER));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND03 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ03)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DST_ADJDEF));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND04 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ04)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DST_MELEE_STEALHP));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND05 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ05)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DST_PVP_DMG));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND06 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ06)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_STR));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND07 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ07)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_STA));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND08 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ08)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_DEX));
			else if (m_dwJewel[i] >= II_GEN_MAT_DIAMOND09 && m_dwJewel[i] <= II_GEN_MAT_TOPAZ09)
				DstText.Format("%s", prj.GetText(TID_TOOLTIP_INT));

			LPWNDCTRL wndCtrl = GetWndCtrl(m_nInfo[i]);
			if (wndCtrl)
				p2DRender->TextOut(wndCtrl->rect.left + 10, wndCtrl->rect.top + 10, DstText, D3DCOLOR_ARGB(255, 0, 0, 0));

			LPWNDCTRL pWndCtrl = GetWndCtrl(m_nJewelSlot[i]);
			if (i < m_pItemElem->GetUltimatePiercingSize())
			{
				if (m_dwJewel[i] != -1)
				{
					if (i != m_nUsableSlot)
					{
						pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndDisableBlue.bmp"), 0xffff00ff);
						if (pTexture != NULL)
							pTexture->Render(p2DRender, CPoint(pWndCtrl->rect.left, pWndCtrl->rect.top));
					}

					pItemProp = prj.GetItemProp(m_dwJewel[i]);
					if (pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
						if (pTexture != NULL)
							pTexture->Render(p2DRender, CPoint(pWndCtrl->rect.left, pWndCtrl->rect.top));
					}
				}
				else
				{
					if (i != m_nUsableSlot)
					{
						pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndDisableBlack.bmp"), 0xffff00ff);
						if (pTexture != NULL)
							pTexture->Render(p2DRender, CPoint(pWndCtrl->rect.left, pWndCtrl->rect.top));
					}
				}
			}
			else
			{
				pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndDisableRed.bmp"), 0xffff00ff);
				if (pTexture != NULL)
					pTexture->Render(p2DRender, CPoint(pWndCtrl->rect.left, pWndCtrl->rect.top));
			}
		}
	}

	if (m_nStatus == 1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;

		lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
		rect = lpWndCtrl->rect;
		p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB(m_nAlpha, 240, 255, 255));
	}
}

BOOL CWndSmeltJewel::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (m_nStatus != 0)
		return FALSE;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CUSTOM1);

	//Set Weapon
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pItemElem))
		return FALSE;

	if ((pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_DIRECT || pItemElem->GetProp()->dwItemKind2 == IK2_WEAPON_MAGIC) &&
		pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
	{
		if (wndCtrl->rect.PtInRect(point))
		{
			if (m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			InitializeJewel(pItemElem);

			if (m_pItemElem != NULL)
				m_pItemElem->SetExtra(pItemElem->GetExtra() + 1);

			m_pMainItem = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_ITEM, m_pItemElem->m_dwItemId);
			m_pMainItem->InitDeviceObjects(g_Neuz.GetDevice());
		}
	}

	//SetJewel
	if (m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if (pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY ||
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			int checkslot = -1;
			for (int i = 0; i < 5; i++)
			{
				LPWNDCTRL wndCtrl = GetWndCtrl(m_nJewelSlot[i]);
				if (wndCtrl->rect.PtInRect(point))
				{
					checkslot = i;
					i = 5;
				}
			}
			if (m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1 && checkslot == m_nUsableSlot)
			{
				m_dwJewel[checkslot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra() + 1);
				m_pJewelElem = pItemElem;
			}
		}
	}

	wndCtrl = GetWndCtrl(WIDC_PIC_SLOT1);
	CRect rect = wndCtrl->rect;

	if (rect.PtInRect(point))
	{
		if (m_pMoonstone == NULL)
		{
			ItemProp* pItemProp = pItemElem->GetProp();
			if (pItemProp->dwID == II_GEN_MAT_MOONSTONE)
			{
				m_pMoonstone = pItemElem;
				m_pMoonstone->SetExtra(m_pMoonstone->GetExtra() + 1);
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_REMOVE_JEWEL_ERROR1), NULL, prj.GetTextColor(TID_GAME_REMOVE_JEWEL_ERROR1));
		}
	}

	return TRUE;
}

void CWndSmeltJewel::SetJewel(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	//SetJewel
	if (m_nStatus == 0 && m_pItemElem != NULL && m_pItemElem->GetUltimatePiercingSize() > 0)
	{
		if (pItemElem && (pItemElem->m_dwItemId == II_GEN_MAT_RUBY ||
			pItemElem->m_dwItemId == II_GEN_MAT_DIAMOND ||
			pItemElem->m_dwItemId == II_GEN_MAT_EMERALD ||
			pItemElem->m_dwItemId == II_GEN_MAT_SAPPHIRE ||
			pItemElem->m_dwItemId == II_GEN_MAT_TOPAZ))
		{
			if (m_nUsableSlot > -1 && m_dwJewel[m_nUsableSlot] == -1)
			{
				m_dwJewel[m_nUsableSlot] = pItemElem->m_dwItemId;
				m_objJewelId = pItemElem->m_dwObjId;
				pItemElem->SetExtra(pItemElem->GetExtra() + 1);
				m_pJewelElem = pItemElem;
			}
		}
	}
}

void CWndSmeltJewel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(m_nJewelSlot[m_nUsableSlot]);
	if (m_nStatus == 0 && wndCtrl != NULL && wndCtrl->rect.PtInRect(point))
	{
		if (m_dwJewel[m_nUsableSlot] != -1)
		{
			if (m_pJewelElem)
				m_pJewelElem->SetExtra(0);
			m_dwJewel[m_nUsableSlot] = -1;
		}
	}

	wndCtrl = GetWndCtrl(WIDC_PIC_SLOT1);
	CRect rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		if (m_pMoonstone)
		{
			if (g_pPlayer && !g_pPlayer->m_vtInfo.IsTrading(m_pMoonstone))
			{
				m_pMoonstone->SetExtra(0);
				m_pMoonstone = NULL;
			}
		}
	}

}

void CWndSmeltJewel::SetItem(CItemElem* pItemElem)
{
	if (!m_pItemElem)
		return;

	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (m_pMoonstone == NULL && pItemProp->dwItemKind2 == IK2_MATERIAL)
	{
		if (pItemProp->dwID == II_GEN_MAT_MOONSTONE)
		{
			m_pMoonstone = pItemElem;
			m_pMoonstone->SetExtra(m_pMoonstone->GetExtra() + 1);
		}
	}
}
void CWndSmeltJewel::UpdateButtons()
{
	for (int i = 0; i < 5; i++)
	{
		if (m_pItemElem && m_pMoonstone && m_pItemElem->GetUltimatePiercingItem(i) != 0 && m_nStatus != 1)
			pWndButton[i]->EnableWindow(TRUE);
		else
			pWndButton[i]->EnableWindow(FALSE);
	}
}

void CWndSmeltJewel::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SmeltJewel.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

BOOL CWndSmeltJewel::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_START)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(FALSE);

			m_nStatus = 1;
			PLAYSND("PcSkillD-Counter01.wav");
		}
	}

	if (m_pItemElem)
	{
		int nSlot = -1;
		switch (nID)
		{
		case WIDC_BUTTON1: nSlot = 0; break;
		case WIDC_BUTTON2: nSlot = 1; break;
		case WIDC_BUTTON3: nSlot = 2; break;
		case WIDC_BUTTON4: nSlot = 3; break;
		case WIDC_BUTTON5: nSlot = 4; break;
		}

		if (nSlot != -1 && m_pItemElem->GetUltimatePiercingItem(nSlot) != 0)
		{
			if (m_pMoonstone)
			{
				if (m_nStatus != 1)
					g_DPlay.SendUltimateRemoveGem(m_pItemElem->m_dwObjId, m_pMoonstone->m_dwObjId, nSlot);
				else
					g_WndMng.PutString("You cant remove jewel right now");
			}
			else
				g_WndMng.PutString("You should insert Moostone firstly");
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSmeltJewel::ReceiveResult(int result)
{

	switch (result)
	{
	case CUltimateWeapon::ULTIMATE_SUCCESS:
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELTJEWEL_SUCCESS), NULL, prj.GetTextColor(TID_GAME_SMELTJEWEL_SUCCESS));
		PLAYSND("InfUpgradeSuccess.wav");
		InitializeJewel(m_pItemElem);
		break;
	case CUltimateWeapon::ULTIMATE_FAILED:
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELTJEWEL_FAIL), NULL, prj.GetTextColor(TID_GAME_SMELTJEWEL_FAIL));
		InitializeJewel(m_pItemElem);
		break;
	case CUltimateWeapon::ULTIMATE_CANCEL:
		Destroy();
		break;
	}
}

void CWndSmeltJewel::InitializeJewel(CItemElem* pItemElem)
{

	m_nJewelCount = 0;
	m_nUsableSlot = -1;
	m_dwJewel[m_nUsableSlot] = -1;
	m_objJewelId = -1;

	m_pItemElem = pItemElem;

	BOOL bSlot = true;

	for (int i = 0; i < 5; i++)
	{
		m_dwJewel[i] = -1;
		if (i < m_pItemElem->GetUltimatePiercingSize())
		{
			if (m_pItemElem->GetUltimatePiercingItem(i) != 0)
			{
				m_dwJewel[i] = m_pItemElem->GetUltimatePiercingItem(i);
				m_nJewelCount++;
			}
			else if (bSlot)
			{
				if (m_dwJewel[i] == (DWORD)-1)
				{
					m_nUsableSlot = i;
					bSlot = false;
				}
			}
		}
	}

	if (m_nUsableSlot == -1)
	{
		int m_nSlot = pItemElem->GetUltimatePiercingSize();
		if (m_nJewelCount < m_nSlot)
			m_nUsableSlot = m_nJewelCount;
		else
			m_nUsableSlot = -1;
	}
	if (m_pJewelElem)
		m_pJewelElem->SetExtra(0);
}
#endif


#if __VER >= 10 // __CSC_VER9_1 -> __LEGEND
CWndHeroSkillUp::CWndHeroSkillUp()
{
	for (int i = 0; i < 5; i++)
	{
		m_pItemElem[i] = NULL;
		m_JewelID[i] = -1;
		m_SlotID[i] = -1;
		m_PicJewel[i] = -1;
	}
}

CWndHeroSkillUp::~CWndHeroSkillUp()
{
}
void CWndHeroSkillUp::OnDestroy()
{
	for (int i = 0; i < 5; i++)
	{
		if (m_pItemElem[i] != NULL)
		{
			if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem[i]))
				m_pItemElem[i]->SetExtra(0);
		}
	}
}

void CWndHeroSkillUp::OnDraw(C2DRender* p2DRender)
{
	//Render Icon
	ItemProp* pItemProp;
	CTexture* pTexture;
	BOOL bCheckSlot = TRUE;

	CPoint point = GetMousePoint();
	int testnum = HitTest(point);
	if (testnum != -1)
	{
		if (CWndBase::m_GlobalShortcut.m_dwData)
		{
			CRect rect;
			LPWNDCTRL wndCtrl = GetWndCtrl(m_SlotID[testnum]);
			rect = wndCtrl->rect;
			p2DRender->RenderFillRect(rect, 0x60ffff00);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		//Default Jewel
		pItemProp = prj.GetItemProp(m_JewelID[i]);
		LPWNDCTRL wndCtrl = GetWndCtrl(m_PicJewel[i]);
		if (pItemProp != NULL)
		{
			pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			if (pTexture != NULL)
				pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
		}

		//Slot Jewel
		if (m_pItemElem[i] != NULL)
		{
			pItemProp = m_pItemElem[i]->GetProp();
			wndCtrl = GetWndCtrl(m_SlotID[i]);
			if (pItemProp != NULL)
			{
				pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
				if (pTexture != NULL)
					pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
			}

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

			CString str;
			CSize size;

			str.Format("%d", m_pItemElem[i]->GetExtra());
			size = m_p2DRender->m_pFont->GetTextExtent(str);
			m_p2DRender->TextOut(wndCtrl->rect.right - size.cx - 2, wndCtrl->rect.bottom - size.cy, str, 0xffeaebdd);

			p2DRender->SetFont(pOldFont);
		}
		else
			bCheckSlot = FALSE;
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(bCheckSlot);
}

void CWndHeroSkillUp::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(FALSE);

	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	SetDescription(NULL);

	m_SlotID[0] = WIDC_PIC_SLOT1;
	m_SlotID[1] = WIDC_PIC_SLOT2;
	m_SlotID[2] = WIDC_PIC_SLOT3;
	m_SlotID[3] = WIDC_PIC_SLOT4;
	m_SlotID[4] = WIDC_PIC_SLOT5;

	m_PicJewel[0] = WIDC_CUSTOM1;
	m_PicJewel[1] = WIDC_CUSTOM2;
	m_PicJewel[2] = WIDC_CUSTOM3;
	m_PicJewel[3] = WIDC_CUSTOM4;
	m_PicJewel[4] = WIDC_CUSTOM5;

	m_JewelID[0] = II_GEN_MAT_DIAMOND;
	m_JewelID[1] = II_GEN_MAT_EMERALD;
	m_JewelID[2] = II_GEN_MAT_SAPPHIRE;
	m_JewelID[3] = II_GEN_MAT_RUBY;
	m_JewelID[4] = II_GEN_MAT_TOPAZ;

	MoveParentCenter();
}

BOOL CWndHeroSkillUp::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HERO_SKILLUP, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndHeroSkillUp::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndHeroSkillUp::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndHeroSkillUp::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndHeroSkillUp::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndHeroSkillUp::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int choicenum = HitTest(point);
	if (choicenum > -1 && m_pItemElem[choicenum] != NULL)
	{
		m_pItemElem[choicenum]->SetExtra(0);
		m_pItemElem[choicenum] = NULL;
	}
}

BOOL CWndHeroSkillUp::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	int nSelect = HitTest(point);
	if (nSelect > -1)
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pItemElem))
			return FALSE;

		if (m_pItemElem[nSelect] == NULL)
		{
			if (pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->m_nItemNum);
			}
		}
	}

	return TRUE;
}

BOOL CWndHeroSkillUp::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		BOOL checkall = TRUE;
		OBJID itemobjId[5];
		for (int i = 0; i < 5; i++)
		{
			if (m_pItemElem[i] == NULL)
				checkall = FALSE;
			else
				itemobjId[i] = m_pItemElem[i]->m_dwObjId;
		}

		if (checkall)
		{
			g_DPlay.SendLegendSkillUp(itemobjId, 5);
		}
	}
	else if (nID == WIDC_CANCEL)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

int CWndHeroSkillUp::HitTest(CPoint point)
{
	int rtn_val = -1;
	CRect rect;
	for (int i = 0; i < 5; i++)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl(m_SlotID[i]);
		rect = wndCtrl->rect;
		if (rect.PtInRect(point))
		{
			rtn_val = i;
			i = MAX_JEWEL;
		}
	}
	return rtn_val;
}

void CWndHeroSkillUp::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("HeroSkillUp.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

void CWndHeroSkillUp::SetJewel(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pProp = pItemElem->GetProp();
	int nSelect = -1;

	switch (pProp->dwID)
	{
	case II_GEN_MAT_DIAMOND:
		nSelect = 0;
		break;
	case II_GEN_MAT_EMERALD:
		nSelect = 1;
		break;
	case II_GEN_MAT_SAPPHIRE:
		nSelect = 2;
		break;
	case II_GEN_MAT_RUBY:
		nSelect = 3;
		break;
	case II_GEN_MAT_TOPAZ:
		nSelect = 4;
		break;
	}

	if (nSelect > -1)
	{
		if (m_pItemElem[nSelect] == NULL)
		{
			if (pItemElem->GetProp()->dwID == m_JewelID[nSelect])
			{
				m_pItemElem[nSelect] = pItemElem;
				m_pItemElem[nSelect]->SetExtra(m_pItemElem[nSelect]->m_nItemNum);
			}
		}
	}
}

void CWndHeroSkillUp::ReceiveResult(int nresult)
{
	if (nresult == 0 || nresult == 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (m_pItemElem[i] != NULL)
			{
				m_pItemElem[i]->SetExtra(m_pItemElem[i]->GetExtra() - 1);
				if (m_pItemElem[i]->GetExtra() <= 0)
				{
					m_pItemElem[i]->SetExtra(0);
					m_pItemElem[i] = NULL;
				}
			}
		}
	}

	switch (nresult)
	{
	case -1:
		g_WndMng.PutString(prj.GetText(TID_GAME_HEROSKILLUP_MAX));
		break;
	case 0:
		g_WndMng.PutString(prj.GetText(TID_GAME_HEROSKILLUP_FAIL));
		break;
	case 1:
		g_WndMng.PutString(prj.GetText(TID_GAME_HEROSKILLUP_SUCCESS));
		break;
	}
}
#endif //__CSC_VER9_1 -> __LEGEND




#ifdef __TRADESYS

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndDialogEvent Class
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndDialogEvent::CWndDialogEvent()
{
	m_nChoiceNum = -1;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nMMI = -1;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent("123");

	m_nGap = size.cy + 2;

	if (m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndDialogEvent::~CWndDialogEvent()
{
}

void CWndDialogEvent::OnDestroy()
{
}

void CWndDialogEvent::OnDraw(C2DRender* p2DRender)
{

}

void CWndDialogEvent::SetMMI(int mmi)
{
	m_nMMI = mmi;
}

void CWndDialogEvent::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	if (m_nMMI != -1)
	{
		m_nVecList = prj.m_Exchange.GetListTextId(m_nMMI);

		CWndListBox* pWndListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		pWndListItems->ResetContent();
		for (auto& it : m_nVecList)
			pWndListItems->AddString("");

		SetDescription(NULL);
	}
	else
		Destroy();

	MoveParentCenter();
}

BOOL CWndDialogEvent::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DIALOG_EVENT, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndDialogEvent::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
		//Text Page Down
		if (m_nCurrentPage < m_nDescCount - 1)
		{
			m_nCurrentPage++;
			CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
			pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));

			if (m_nCurrentPage == m_nDescCount - 1)
			{
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				pButton->SetVisible(FALSE);
				pButton->EnableWindow(FALSE);
			}
		}
		break;
	case WIDC_OK:
		if (CheckBeforeSending())
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
			pButton->EnableWindow(FALSE);
			g_DPlay.SendExchange(m_nMMI, m_nChoiceNum);
		}
		break;

	case WIDC_CANCEL:
		Destroy();
		break;

	case WIDC_LISTBOX1:
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel != -1)
			m_nChoiceNum = nCurSel;
		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndDialogEvent::SetDescription(CHAR* szChar)
{
	//Set List
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	//Set Description
	m_nDescList = prj.m_Exchange.GetDescId(m_nMMI);
	m_nDescCount = m_nDescList.size();
	if (!m_nDescCount)
		return;

	if (m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString(prj.GetText(m_nDescList[m_nCurrentPage]));
	pText->ResetString();

	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	lpWndCtrl = GetWndCtrl(WIDC_BUTTON1);
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

void CWndDialogEvent::ReceiveResult(int result)
{
	vector<int> vResult = prj.m_Exchange.GetResultMsg(m_nMMI, m_nChoiceNum);

	switch (result)
	{
	case CExchange::EXCHANGE_SUCCESS:

		if (vResult.size() == 2)
			g_WndMng.OpenMessageBox(prj.GetText(vResult[0]));
		break;
	case CExchange::EXCHANGE_INVENTORY_FAILED:
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_LACKSPACE));
		break;
	case CExchange::EXCHANGE_CONDITION_FAILED:

		if (vResult.size() == 2)
			g_WndMng.OpenMessageBox(prj.GetText(vResult[1]));
		break;
	case CExchange::EXCHANGE_FAILED:
		g_WndMng.OpenMessageBox("FAILED");
		break;
	}

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pButton->EnableWindow(TRUE);
}
BOOL CWndDialogEvent::CheckBeforeSending()
{
	if (!g_pPlayer)
		return FALSE;

	if (m_nChoiceNum <= -1)
		return FALSE;

	CExchange::PSETLIST pSet = prj.m_Exchange.FindExchange(m_nMMI);
	if (!pSet)
		return FALSE;

	for (unsigned j = 0; j < pSet->vecSet[m_nChoiceNum].vecCondItem.size(); j++)
	{
		ItemProp* pPropItem = prj.GetItemProp(pSet->vecSet[m_nChoiceNum].vecCondItem[j].dwItemId);
		if (pPropItem)
		{
			if (pPropItem->dwID == II_GOLD_SEED1)
			{
				if (g_pPlayer->GetGold() < pSet->vecSet[m_nChoiceNum].vecCondItem[j].nItemNum)
					return FALSE;
			}
			else
			{
				if (g_pPlayer->m_Inventory.GetItemCount(pPropItem->dwID) < pSet->vecSet[m_nChoiceNum].vecCondItem[j].nItemNum)
					return FALSE;

				if (!CheckExtra(pPropItem->dwID))
					return FALSE;
			}
		}
	}

	return TRUE;
}
BOOL CWndDialogEvent::CheckExtra(DWORD dwId)
{
	CItemElem* pItemElem;
	for (int i = 0; i < g_pPlayer->m_Inventory.GetMax(); i++)
	{
		pItemElem = g_pPlayer->m_Inventory.GetAtId(i);

		if (pItemElem && pItemElem->m_dwItemId == dwId)
		{
			if (!IsUsableItem(pItemElem))
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif //__TRADESYS

#if __VER >= 12 // __HEAVEN_TOWER

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndHeavenTower Class
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndHeavenTower::CWndHeavenTower()
{
	m_nChoiceNum = -1;
	m_nListCount = 0;
	m_nDescCount = 0;
	m_nCurrentPage = 0;
	m_nListOffset = 0;

	CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent("123");

	m_nGap = size.cy + 2;

	if (m_nGap >= 16)
		m_nListOffset = m_nGap - 16;
	else
		m_nListOffset = 0;
}

CWndHeavenTower::~CWndHeavenTower()
{
}

void CWndHeavenTower::OnDestroy()
{
}

void CWndHeavenTower::OnDraw(C2DRender* p2DRender)
{
	if (m_nChoiceNum > -1)
	{
		CRect rect;
		LPWNDCTRL lpWndCtrl;
		D3DXCOLOR color = D3DCOLOR_ARGB(60, 240, 0, 0);

		lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
		rect = lpWndCtrl->rect;
		rect.top += m_nChoiceNum * m_nGap + m_nListOffset;
		rect.bottom += m_nChoiceNum * m_nGap + m_nListOffset;

		rect.right += 80;
		p2DRender->RenderFillRect(rect, color);
	}
}

void CWndHeavenTower::InitText()
{
	m_nDescList.push_back(TID_TOOLTIP_HEAVENTOWER);

	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR01);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR02);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR03);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR04);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR05);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR06);
	m_nVecList.push_back(TID_GAME_HEAVETOWER_FLOOR07);

	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR01_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR02_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR03_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR04_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR05_MSG);
	m_nMsgList.push_back(TID_GAME_HEAVETOWER_FLOOR06_MSG);

	m_nCost.push_back(10000);
	m_nCost.push_back(30000);
	m_nCost.push_back(50000);
	m_nCost.push_back(70000);
	m_nCost.push_back(100000);

	m_nListCount = m_nVecList.size();

	//Set List
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);

	for (int i = 0; i < m_nListCount; i++)
	{
		CString strText;
		strText.Format("%d. %s", i + 1, prj.GetText(m_nVecList[i]));
		pWndList->AddString(strText);
	}

	//Set Description
	m_nDescCount = m_nDescList.size();
	if (m_nDescCount == 0)
		return;

	if (m_nDescCount == 1)
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->SetVisible(FALSE);
		pButton->EnableWindow(FALSE);
	}

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_nCurrentPage = 0;
	pText->m_string.AddParsingString(prj.GetText(m_nDescList[m_nCurrentPage]));
	pText->ResetString();
}

void CWndHeavenTower::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	InitText();

	if (m_nListCount == 0)
	{
		Destroy();
		return;
	}
	InitWnd();

	MoveParentCenter();
}

BOOL CWndHeavenTower::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_HEAVEN_TOWER, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndHeavenTower::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
		//Text Page Down
		if (m_nCurrentPage < m_nDescCount - 1)
		{
			m_nCurrentPage++;
			CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
			pText->m_string.SetString(prj.GetText(m_nDescList[m_nCurrentPage]));

			if (m_nCurrentPage == m_nDescCount - 1)
			{
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				pButton->SetVisible(FALSE);
				pButton->EnableWindow(FALSE);
			}
		}
		break;
	case WIDC_LISTBOX1:
		CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		int nCurSel = pWndListBox->GetCurSel();
		if (nCurSel != -1)
		{
			m_nChoiceNum = nCurSel;

			if (m_nChoiceNum == 6)
			{
				Destroy();
				break;
			}

			CString strMsg;
			CWndHeavenTowerEntranceConfirm* pWndEntranceConfirm = new CWndHeavenTowerEntranceConfirm;
			if (pWndEntranceConfirm)
			{
				g_WndMng.OpenCustomBox("", pWndEntranceConfirm, this);

				if (m_nChoiceNum != 5)
				{
					strMsg.Format(prj.GetText(m_nMsgList[m_nChoiceNum]), m_nCost[m_nChoiceNum]);
					pWndEntranceConfirm->SetValue(strMsg, m_nChoiceNum + 1);
				}
				else
				{
					strMsg.Format(prj.GetText(m_nMsgList[m_nChoiceNum]));
					pWndEntranceConfirm->SetValue(strMsg, m_nChoiceNum + 1);
				}

			}
		}
		break;
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndHeavenTower::InitWnd()
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;

	//Size Control
	if (m_nListCount <= MAX_FLOOR_COUNT)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		lpWndCtrl = GetWndCtrl(WIDC_LISTBOX1);
		rect = lpWndCtrl->rect;

		int nListBoxBottom = 0;
		int nListBoxHeight = 0;
		int nButtonHeight = 0;
		//ListBox
		rect.bottom = rect.top + m_nListCount * m_nGap + 10;
		nListBoxBottom = rect.bottom;
		nListBoxHeight = rect.bottom - rect.top;

		rect.right += 80;

		pWndList->SetWndRect(rect);
		//Window
		rect = GetWindowRect(TRUE);
		lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
		CRect rectText = lpWndCtrl->rect;
		rect.bottom = rect.top + nListBoxHeight + (rectText.bottom - rectText.top) + 60;
		SetWndRect(rect);
	}


	//Window
	rect = GetWindowRect(TRUE);
	rect.right += 80;
	SetWndRect(rect);

	//Button
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	lpWndCtrl = GetWndCtrl(WIDC_BUTTON1);
	rect = lpWndCtrl->rect;
	rect.left += 80;
	rect.right += 80;
	pWndButton->SetWndRect(rect);

	//Text
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	lpWndCtrl = GetWndCtrl(WIDC_TEXT1);
	rect = lpWndCtrl->rect;
	rect.right += 80;
	pWndText->SetWndRect(rect);
}

////////////////////////////////////////////////////////////////////
// CWndHeavenTowerEntranceConfirm Class
////////////////////////////////////////////////////////////////////

BOOL CWndHeavenTowerEntranceConfirm::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	m_nFloor = 0;
	return CWndMessageBox::Initialize("", pWndParent, MB_OKCANCEL);
}

void CWndHeavenTowerEntranceConfirm::SetValue(CString str, DWORD nFloor)
{
	m_wndText.SetString(str);
	m_nFloor = nFloor;
}

BOOL CWndHeavenTowerEntranceConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case IDOK:
		if (m_nFloor > 0 && m_nFloor != 6)
		{
			g_DPlay.SendTeleportToHeavenTower(m_nFloor);
			SAFE_DELETE(g_WndMng.m_pWndHeavenTower);
		}
		Destroy();
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //__HEAVEN_TOWER

#if __VER >= 10 // __REMOVE_ATTRIBUTE
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndRemoveAttribute Class
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndRemoveAttribute::CWndRemoveAttribute()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pWndConfirm = NULL;
	m_pTexture = NULL;
}

CWndRemoveAttribute::~CWndRemoveAttribute()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}

	if (m_pWndConfirm != NULL)
		m_pWndConfirm->Destroy();

	SAFE_DELETE(m_pWndConfirm);
}

void CWndRemoveAttribute::OnDestroy()
{
}

void CWndRemoveAttribute::OnDraw(C2DRender* p2DRender)
{
	ItemProp* pItemProp;

	if (m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
		if (pItemProp != NULL)
		{
			if (m_pTexture != NULL)
				m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
		}
	}
}

void CWndRemoveAttribute::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);

	if (::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("ButOk2.bmp")), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	SetDescription(NULL);

	MoveParentCenter();
}

BOOL CWndRemoveAttribute::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndRemoveAttribute::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRemoveAttribute::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndRemoveAttribute::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndRemoveAttribute::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndRemoveAttribute::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem)
		return;

	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;
	}
}

BOOL CWndRemoveAttribute::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pTempElem))
			return FALSE;

		if (pTempElem->m_nResistAbilityOption > 0)
		{
			ItemProp* pItemProp = pTempElem->GetProp();
			if (pItemProp && pItemProp->IsEleRefineryAble())
			{
				if (m_pItemElem != NULL)
					m_pItemElem->SetExtra(0);

				m_pItemElem = pTempElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);
				m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
				pButton->EnableWindow(TRUE);
			}
		}
		else
			g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_REMOVE_ERROR)); //Error Message
	}
	return TRUE;
}

void CWndRemoveAttribute::SetWeapon(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	if (pItemElem->m_nResistAbilityOption > 0)
	{
		ItemProp* pProp = pItemElem->GetProp();
		if (pProp && pProp->IsEleRefineryAble())
		{
			if (m_pItemElem != NULL)
				m_pItemElem->SetExtra(0);

			m_pItemElem = pItemElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_START);
			pButton->EnableWindow(TRUE);
		}
	}
	else
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_REMOVE_ERROR)); //Error Message
}

BOOL CWndRemoveAttribute::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_START)
	{
		if (m_pWndConfirm == NULL)
		{
			m_pWndConfirm = new CWndRemoveAttributeConfirm;
			m_pWndConfirm->Initialize(this);
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndRemoveAttribute::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndConfirm)
		SAFE_DELETE(m_pWndConfirm);
}

void CWndRemoveAttribute::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("RemoveAttribute.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

void CWndRemoveAttribute::ReceiveResult(BOOL result)
{
	if (result)
		g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_REMOVE_ATTRIBUTE_CONFIRM));

	Destroy();
}

////////////////////////////////////////////////////////////////////
//	CWndRemoveAttributeConfirm
////////////////////////////////////////////////////////////////////

CWndRemoveAttributeConfirm::CWndRemoveAttributeConfirm()
{
}

CWndRemoveAttributeConfirm::~CWndRemoveAttributeConfirm()
{
}

void CWndRemoveAttributeConfirm::OnDestroy()
{
}

void CWndRemoveAttributeConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndRemoveAttributeConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pText->m_string.AddParsingString(prj.GetText(TID_GAME_REMOVE_ATTRIBUTE));
	pText->ResetString();
	MoveParentCenter();
}

BOOL CWndRemoveAttributeConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_REMOVE_ATTRIBUTE_CONFIRM, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndRemoveAttributeConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRemoveAttributeConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRemoveAttributeConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRemoveAttributeConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndRemoveAttributeConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndRemoveAttribute* pParentwnd = (CWndRemoveAttribute*)this->m_pParentWnd;

	if (nID == WIDC_YES)
	{
		//SEND to Server
		if (pParentwnd && pParentwnd->m_pItemElem)
			g_DPlay.SendRemoveAttribute(pParentwnd->m_pItemElem->m_dwObjId);
	}
	else if (nID == WIDC_NO || nID == WTBID_CLOSE)
	{
		pParentwnd->m_pWndConfirm = NULL;
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__REMOVE_ATTRIBUTE


#if __VER >= 11 // __PIERCING_REMOVE
CWndRemovePiercing::CWndRemovePiercing()
{
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
	m_pSlot = -1;
}

CWndRemovePiercing::~CWndRemovePiercing()
{
}

void CWndRemovePiercing::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

void CWndRemovePiercing::OnDraw(C2DRender* p2DRender)
{
	ItemProp* pItemProp;

	if (m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
		if (pItemProp != NULL)
		{
			if (m_pTexture != NULL)
				m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
			int nMaxPiercing;
			CRect rect;
			LPWNDCTRL lpWndCtrl;

			CPoint ptMouse = GetMousePoint();
			if (pItemProp->dwItemKind3 != IK3_SUIT)
			{
				static DWORD dwStatic[] = { WIDC_STATIC_PIERCING1, WIDC_STATIC_PIERCING2, WIDC_STATIC_PIERCING3, WIDC_STATIC_PIERCING4, WIDC_STATIC_PIERCING5,
					WIDC_STATIC_PIERCING6, WIDC_STATIC_PIERCING7, WIDC_STATIC_PIERCING8, WIDC_STATIC_PIERCING9, WIDC_STATIC_PIERCING10 };

				for (int i = 0; i < sizeof(dwStatic) / sizeof(dwStatic[0]); i++)
				{
					CRect rect2 = GetWndCtrl(dwStatic[i])->rect;
					if (rect2.PtInRect(ptMouse))
					{
						p2DRender->RenderFillRect(rect2, 0xFFbef5b0);
						p2DRender->RenderRect(rect2, 0xFF666100);
						break;
					}
				}
			}
			else
			{
				static DWORD dwStatic2[] = { WIDC_STATIC_PIERCING1, WIDC_STATIC_PIERCING2, WIDC_STATIC_PIERCING3, WIDC_STATIC_PIERCING4 };

				for (int i = 0; i < sizeof(dwStatic2) / sizeof(dwStatic2[0]); i++)
				{
					CRect rect2 = GetWndCtrl(dwStatic2[i])->rect;
					if (rect2.PtInRect(ptMouse))
					{
						p2DRender->RenderFillRect(rect2, 0xFFbef5b0);
						p2DRender->RenderRect(rect2, 0xFF666100);
						break;
					}
				}
			}

			if (pItemProp->dwItemKind3 == IK3_SUIT)
			{
				nMaxPiercing = 4;
				D3DXCOLOR color = D3DCOLOR_ARGB(60, 240, 0, 0);

				for (int i = 4; i < 10; i++)
				{
					lpWndCtrl = GetWndCtrl(m_nInfoSlot[i]);
					rect = lpWndCtrl->rect;
					p2DRender->RenderFillRect(rect, color);
				}
			}
			else
				nMaxPiercing = 10;

			CString textOpt;
			CString strTemp;

			int nPiercingSize = m_pItemElem->GetPiercingSize();

			for (int i = 0; i < nPiercingSize; i++)
			{
				if (nPiercingSize > nMaxPiercing)
					break;

				PPIERCINGAVAIL ptr = NULL;
				ptr = CPiercingAvail::GetInstance()->GetPiercingAvail(m_pItemElem->GetPiercingItem(i));

				if (ptr != NULL)
				{
					for (int j = 0; j < ptr->nSize; j++)
					{
						int nDst = (int)ptr->anDstParam[j];
						int nAdj = (int)ptr->anAdjParam[j];

						if (g_WndMng.IsDstRate(nDst))
						{
							if (nDst == DST_ATTACKSPEED)
								strTemp.Format("%s%+d%%  ", g_WndMng.FindDstString(nDst), nAdj / 2 / 10);
							else
								strTemp.Format("%s%+d%%  ", g_WndMng.FindDstString(nDst), nAdj);
						}
						else
							strTemp.Format("%s+%d  ", g_WndMng.FindDstString(nDst), nAdj);

						textOpt += strTemp;
					}

					lpWndCtrl = GetWndCtrl(m_nInfoSlot[i]);
					p2DRender->TextOut(lpWndCtrl->rect.left + 10, lpWndCtrl->rect.top + 6, textOpt, D3DCOLOR_ARGB(255, 0, 0, 0));

					textOpt = "";
					strTemp = "";
				}
			}
		}
	}
}

void CWndRemovePiercing::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	SetDescription(NULL);

	m_nInfoSlot[0] = WIDC_STATIC_PIERCING1;
	m_nInfoSlot[1] = WIDC_STATIC_PIERCING2;
	m_nInfoSlot[2] = WIDC_STATIC_PIERCING3;
	m_nInfoSlot[3] = WIDC_STATIC_PIERCING4;
	m_nInfoSlot[4] = WIDC_STATIC_PIERCING5;
	m_nInfoSlot[5] = WIDC_STATIC_PIERCING6;
	m_nInfoSlot[6] = WIDC_STATIC_PIERCING7;
	m_nInfoSlot[7] = WIDC_STATIC_PIERCING8;
	m_nInfoSlot[8] = WIDC_STATIC_PIERCING9;
	m_nInfoSlot[9] = WIDC_STATIC_PIERCING10;

	MoveParentCenter();
}

BOOL CWndRemovePiercing::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

#if __VER >= 12 // __CSC_VER12_4
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_PIERCING_EX, 0, CPoint(0, 0), pWndParent);
#else //__CSC_VER12_4
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_REMOVE_PIERCING, 0, CPoint(0, 0), pWndParent);
#endif //__CSC_VER12_4
}
BOOL CWndRemovePiercing::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRemovePiercing::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRemovePiercing::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRemovePiercing::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndRemovePiercing::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		if (m_pItemElem)
		{
			m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
			m_pEItemProp = NULL;
			m_pTexture = NULL;
		}
	}

	static DWORD dwStatic[] = { WIDC_STATIC_PIERCING1, WIDC_STATIC_PIERCING2, WIDC_STATIC_PIERCING3, WIDC_STATIC_PIERCING4, WIDC_STATIC_PIERCING5,
		WIDC_STATIC_PIERCING6, WIDC_STATIC_PIERCING7, WIDC_STATIC_PIERCING8, WIDC_STATIC_PIERCING9, WIDC_STATIC_PIERCING10 };

	for (int i = 0; i < sizeof(dwStatic) / sizeof(dwStatic[0]); i++)
	{
		CRect rect2 = GetWndCtrl(dwStatic[i])->rect;
		if (rect2.PtInRect(point))
		{
			m_pSlot = i;
			if (m_pItemElem && m_pItemElem->GetPiercingItem(i) != 0)
				g_WndMng.OpenCustomBox(NULL, new CWndAccept1);

			break;
		}
	}
}

void CWndRemovePiercing::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		ClientToScreen(&point);
		ClientToScreen(&rect);
		g_toolTip.PutToolTip((DWORD)this, prj.GetText(TID_GAME_TOOLTIP_PIERCINGITEM), rect, point);
	}
}

BOOL CWndRemovePiercing::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pTempElem))
			return FALSE;

		if (m_pItemElem == NULL)
		{
#ifdef __AIO_PETS
			if (pTempElem->IsEatPet())
			{
				g_WndMng.PutString(prj.GetText(TID_GAME_REMOVE_PIERCING_ERROR_EX), NULL, prj.GetTextColor(TID_GAME_REMOVE_PIERCING_ERROR_EX));
				return FALSE;
			}
#endif
			if (pTempElem->IsPierceAble())
			{
				m_pItemElem = pTempElem;
				m_pEItemProp = m_pItemElem->GetProp();
				m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);
				m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_REMOVE_PIERCING_ERROR_EX), NULL, prj.GetTextColor(TID_GAME_REMOVE_PIERCING_ERROR_EX));
		}
	}
	return TRUE;
}

void CWndRemovePiercing::SetItem(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	if (m_pItemElem == NULL)
	{
		ItemProp* pProp = pItemElem->GetProp();
#ifdef __AIO_PETS
		if (pItemElem->IsEatPet())
		{
			g_WndMng.PutString(prj.GetText(TID_GAME_REMOVE_PIERCING_ERROR_EX), NULL, prj.GetTextColor(TID_GAME_REMOVE_PIERCING_ERROR_EX));
			return;
		}
#endif
		if (pItemElem->IsPierceAble())
		{
			m_pItemElem = pItemElem;
			m_pEItemProp = m_pItemElem->GetProp();
			m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
		}
		else
			g_WndMng.PutString(prj.GetText(TID_GAME_REMOVE_PIERCING_ERROR_EX), NULL, prj.GetTextColor(TID_GAME_REMOVE_PIERCING_ERROR_EX));
	}
}

void CWndRemovePiercing::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("SmeltRemovePiercing.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

#if __VER >= 13 // __CSC_VER13_2
BOOL CWndCoupleMessage::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndMessageBox::Initialize(m_strText, pWndParent, MB_OKCANCEL);
}

BOOL CWndCoupleMessage::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (m_nMode)
	{
	case CM_SENDPROPOSE:
	{
		if (nID == IDOK)
		{
			if (m_pTargetObj)
				g_DPlay.SendPropose(((CMover*)m_pTargetObj)->GetName());
			Destroy();
		}
		else if (nID == IDCANCEL)
			Destroy();
	}
	break;
	case CM_RECEIVEPROPOSE:
	{
		if (nID == IDOK)
		{
			g_DPlay.SendCouple();
			Destroy();
		}
		else if (nID == IDCANCEL)
		{
			g_DPlay.SendRefuse();
			Destroy();
		}
	}
	break;
	case CM_CANCELCOUPLE:
	{
		if (nID == IDOK)
		{
			g_DPlay.SendDecouple();
			Destroy();
		}
		else if (nID == IDCANCEL)
			Destroy();
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndCoupleMessage::SetMessageMod(CString strText, int nMode, CObj* pTargetObj)
{
	m_strText = strText;
	m_nMode = nMode;
	m_pTargetObj = pTargetObj;
}

////////////////////////////////////////////////////////////////////
// Couple Manager Tab Information Window
////////////////////////////////////////////////////////////////////
CWndCoupleTabInfo::CWndCoupleTabInfo()
{
	m_pCouple = NULL;
	m_pVBGauge = NULL;
}

CWndCoupleTabInfo::~CWndCoupleTabInfo()
{
	DeleteDeviceObjects();
}

HRESULT CWndCoupleTabInfo::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();

	if (m_pVBGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL);

	return S_OK;
}
HRESULT CWndCoupleTabInfo::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndCoupleTabInfo::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}

void CWndCoupleTabInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauFillNormal.bmp"), 0xffff00ff, TRUE);

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	if (pWndButton)
	{
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtBreakUp.bmp"), 0xffff00ff);
		else
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtBreakUp.bmp"), 0xffff00ff);
	}

	MoveParentCenter();
}

BOOL CWndCoupleTabInfo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_INFO, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCoupleTabInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		if (m_pCouple)
		{
			SAFE_DELETE(g_WndMng.m_pWndCoupleMessage);
			g_WndMng.m_pWndCoupleMessage = new CWndCoupleMessage;
			u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
			PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);
			if (pData)
			{
				CString strText;
				strText.Format(prj.GetText(TID_GAME_COUPLECANCEL), pData->szPlayer);
				g_WndMng.m_pWndCoupleMessage->SetMessageMod(strText, CWndCoupleMessage::CM_CANCELCOUPLE);
				g_WndMng.m_pWndCoupleMessage->Initialize(NULL);
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndCoupleTabInfo::OnDraw(C2DRender* p2DRender)
{
	TEXTUREVERTEX2 pVertex[6];
	TEXTUREVERTEX2* pVertices = pVertex;

	//Couple Info & Exp Gauge Draw
	CRect rect, rectTemp;
	LPWNDCTRL lpStatic = GetWndCtrl(WIDC_STATIC4);
	rect.TopLeft().y = lpStatic->rect.top + 36;
	rect.TopLeft().x = lpStatic->rect.left + 70;
	rect.BottomRight().y = lpStatic->rect.top + 48;
	rect.BottomRight().x = lpStatic->rect.right - 10;

	m_pCouple = CCoupleHelper::Instance()->GetCouple();
	if (m_pCouple)
	{
		u_long idPartner = m_pCouple->GetPartner(g_pPlayer->m_idPlayer);
		PlayerData* pData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPartner);

		if (pData)
		{
			CString strTemp;
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(pData->szPlayer);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			pWndStatic->SetTitle(prj.m_aJob[pData->data.nJob].szName);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", pData->data.nLevel);
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);
			pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
			pWndStatic->m_dwColor = 0xff3f3f3f;
			strTemp.Format("%d", m_pCouple->GetLevel());
			pWndStatic->SetTitle(strTemp);

			// Draw Master/Hero Icon
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

			if (pWndWorld)
			{
				LPWNDCTRL lpCtrl = GetWndCtrl(WIDC_LEVEL);
				CPoint ptJobType;
				ptJobType.x = lpCtrl->rect.right - 60;
				ptJobType.y = lpCtrl->rect.top - 1;

				if (prj.m_aJob[pData->data.nJob].dwJobType == JTYPE_MASTER)
				{
					int nMasterIndex = 27;
					if (/*m_nLevel >= 60 && */pData->data.nLevel < 70)
						nMasterIndex = 27;
					else if (pData->data.nLevel >= 70 && pData->data.nLevel < 80)
						nMasterIndex = 28;
					else if (pData->data.nLevel >= 80 && pData->data.nLevel < 90)
						nMasterIndex = 29;
					else if (pData->data.nLevel >= 90 && pData->data.nLevel < 100)
						nMasterIndex = 30;
					else if (pData->data.nLevel >= 100 && pData->data.nLevel < 110)
						nMasterIndex = 31;
					else if (pData->data.nLevel >= 110 && pData->data.nLevel <= 120)
						nMasterIndex = 32;

					pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, ptJobType, nMasterIndex, &pVertices, 0xffffffff);
				}
				else if (prj.m_aJob[pData->data.nJob].dwJobType == JTYPE_HERO || prj.m_aJob[pData->data.nJob].dwJobType == JTYPE_LEGEND_HERO)
					pWndWorld->m_texPlayerDataIcon.MakeVertex(p2DRender, ptJobType, 33, &pVertices, 0xffffffff);

				pWndWorld->m_texPlayerDataIcon.Render(m_pApp->m_pd3dDevice, pVertex, ((int)pVertices - (int)pVertex) / sizeof(TEXTUREVERTEX2));
			}
		}

		float fRate = CCoupleProperty::Instance()->GetExperienceRate(m_pCouple->GetLevel(), m_pCouple->GetExperience());


		if (fRate > 0.00f && fRate < 0.04f)
			fRate = 0.05f;
		else if (fRate >= 0.04f && fRate <= 0.075f)
			fRate = 0.075f;

		int nWidth = (int)(rect.Width() * fRate);
		rectTemp = rect;
		rectTemp.right = rectTemp.left + nWidth;
		if (rect.right < rectTemp.right)
			rectTemp.right = rect.right;
	}
	else
	{
		CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NAME);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_JOB);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
		pWndStatic = (CWndStatic*)GetDlgItem(WIDC_COUPLE_LEVEL);
		pWndStatic->SetTitle("");
	}

	if (m_pVBGauge)
	{
		m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal);
		m_pTheme->RenderGauge(p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal);
	}
}

////////////////////////////////////////////////////////////////////
// Couple Manager Tab Skill Window
////////////////////////////////////////////////////////////////////
CWndCoupleTabSkill::CWndCoupleTabSkill()
{
	m_pText = NULL;
	m_pSkillBgTexture = NULL;
}

CWndCoupleTabSkill::~CWndCoupleTabSkill()
{
}

void CWndCoupleTabSkill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("CoupleSkillInfo.inc")));
	CHAR* szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}

	m_pSkillBgTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "Bg_Couple_Skill.tga"), 0xffff00ff);

	MoveParentCenter();
}

BOOL CWndCoupleTabSkill::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COUPLE_TAB_SKILL, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCoupleTabSkill::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndCoupleTabSkill::OnDraw(C2DRender* p2DRender)
{
	CPoint point;

	LPWNDCTRL lpWndStatic = GetWndCtrl(WIDC_STATIC1);

	if (lpWndStatic)
	{
		point.x = lpWndStatic->rect.left + 4;
		point.y = lpWndStatic->rect.top + 2;
	}

	// Draw Background Image
	if (m_pSkillBgTexture)
		p2DRender->RenderTexture(point, m_pSkillBgTexture);

	// Draw Active SKill
	int nLevel = 0;
	CCouple* pCouple = CCoupleHelper::Instance()->GetCouple();
	if (pCouple)
		nLevel = pCouple->GetLevel();

	VS& vSkills = CCoupleProperty::Instance()->GetSkill(nLevel);
	VSK& vSkillKinds = CCoupleProperty::Instance()->GetSKillKinds();

	if (vSkills.size() == vSkillKinds.size())
	{
		for (int i = 0; i < (int)(vSkills.size()); i++)
		{
			if (vSkills[i] > 0)
			{
				ItemProp* pItemProp = prj.GetItemProp(vSkillKinds[i]);
				if (pItemProp)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pItemProp->szIcon), 0xffff00ff);

					if (pTex)
						p2DRender->RenderTexture(point, pTex);
				}
			}
			point.x += 32;
		}
	}

	// Make Tooltip
	CPoint ptMouse = GetMousePoint();
	CRect rect = lpWndStatic->rect;

	rect.left += 4;
	rect.top += 2;
	rect.right = rect.left + 32;
	rect.bottom = rect.top + 32;

	for (int i = 0; i < (int)(vSkillKinds.size()); i++)
	{
		if (rect.PtInRect(ptMouse))
		{
			// Tooltip
			CString strSkillName;
			ItemProp* pItemProp = prj.GetItemProp(vSkillKinds[i]);

			if (pItemProp)
				strSkillName.Format("%s", pItemProp->szName);

			ClientToScreen(&ptMouse);
			ClientToScreen(&rect);
			g_toolTip.PutToolTip(10000, strSkillName, rect, ptMouse, 1);
		}

		rect.left += 32;
		rect.right += 32;
	}
}

////////////////////////////////////////////////////////////////////
// Couple Manager Window
////////////////////////////////////////////////////////////////////
CWndCoupleManager::CWndCoupleManager()
{
}

CWndCoupleManager::~CWndCoupleManager()
{
}

void CWndCoupleManager::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;

	m_wndCoupleTabInfo.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_INFO);
	m_wndCoupleTabSkill.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_COUPLE_TAB_SKILL);

	WTCITEM tabTabItem;

	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_INFO);
	tabTabItem.pWndBase = &m_wndCoupleTabInfo;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_GAME_COUPLE_SKILL);
	tabTabItem.pWndBase = &m_wndCoupleTabSkill;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	MoveParentCenter();
}

BOOL CWndCoupleManager::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COUPLE_MAIN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndCoupleManager::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__CSC_VER13_2

#endif //__PIERCING_REMOVE

CWndSmeltSafety::CWndSmeltSafety(CWndSmeltSafety::WndMode eWndMode)
	: m_eWndMode(eWndMode)
	, m_bStart(FALSE)
	, m_bRealStart(FALSE)
	, m_dwEnchantTimeStart(NULL_ID)
	, m_dwEnchantTimeEnd(NULL_ID)
	, m_pItemElem(NULL)
	, m_pItemElemMaterial(NULL)
	, m_pItemElemScroll_1(NULL)
	, m_pItemElemScroll_2(NULL)
	, m_pItemElemTexture(NULL)
	, m_pSuccessTexture(NULL)
	, m_pFailureTexture(NULL)
	, m_pSelectedElementalCardItemProp(NULL)
	, nTotalAttempts(0)
	, nSuccessAttempts(0)
	, nFailureAttempts(0)
	, m_pNowGaugeTexture(NULL)
	, m_pVertexBufferGauge(NULL)
	, nLastResult(0)
{

}
CWndSmeltSafety::~CWndSmeltSafety()
{
	ResetItems();
	DeleteDeviceObjects();
}
void CWndSmeltSafety::StartProcess(BOOL bManual)
{
	if (m_bStart)
		return;

	if (!IsPossibleStart())
	{
		g_WndMng.PutString("Not enough resources or penya, or upgrade goal is done !");
		StopProcess();
		return;
	}

	if (bManual)
	{
		if (!m_bRealStart)
			m_dwEnchantTimeStart = g_tmCurrent;
	}
	else
		m_dwEnchantTimeStart = g_tmCurrent + 200;

	m_bRealStart = TRUE;
}
void CWndSmeltSafety::StopProcess(BOOL bFake)
{
	if (!bFake)
		m_bRealStart = FALSE;

	m_bStart = FALSE;
	m_dwEnchantTimeStart = NULL_ID;
	m_dwEnchantTimeEnd = NULL_ID;
	nLastResult = 0;
}
void CWndSmeltSafety::SetResult(int nResult)
{
	if (nResult != 0)
	{
		if (m_pItemElemMaterial)
		{
			m_pItemElemMaterial->SetExtra(m_pItemElemMaterial->GetExtra() - 1);
			if (m_pItemElemMaterial->GetExtra() <= 0)
			{
				m_pItemElemMaterial->SetExtra(0);
				m_pItemElemMaterial = NULL;
			}
		}

		if (m_pItemElemScroll_1)
		{
			m_pItemElemScroll_1->SetExtra(m_pItemElemScroll_1->GetExtra() - 1);
			if (m_pItemElemScroll_1->GetExtra() <= 0)
			{
				m_pItemElemScroll_1->SetExtra(0);
				m_pItemElemScroll_1 = NULL;
			}
		}

		if (m_pItemElemScroll_2)
		{
			m_pItemElemScroll_2->SetExtra(m_pItemElemScroll_2->GetExtra() - 1);
			if (m_pItemElemScroll_2->GetExtra() <= 0)
			{
				m_pItemElemScroll_2->SetExtra(0);
				m_pItemElemScroll_2 = NULL;
			}
		}

		if (m_bRealStart)
			StartProcess(FALSE);
	}

	if (m_bRealStart)
		nLastResult = nResult;
}
void CWndSmeltSafety::ResetItems()
{
	if (m_pItemElem)
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}
	m_pItemElemTexture = NULL;

	if (m_pItemElemMaterial)
	{
		m_pItemElemMaterial->SetExtra(0);
		m_pItemElemMaterial = NULL;
	}
	if (m_pItemElemScroll_1)
	{
		m_pItemElemScroll_1->SetExtra(0);
		m_pItemElemScroll_1 = NULL;
	}
	if (m_pItemElemScroll_2)
	{
		m_pItemElemScroll_2->SetExtra(0);
		m_pItemElemScroll_2 = NULL;
	}

	nTotalAttempts = nSuccessAttempts = nFailureAttempts = 0;

	StopProcess();
	RefreshText();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	pWndEdit->SetString(TEXT(""));

}
void CWndSmeltSafety::RefreshText(void)
{
	CString str;
	if (m_pItemElem)
		str.Format("%d", GetNowSmeltValue());
	else
		str.Format(TEXT("--"));

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_NOW_GRADE);
	pWndStatic->SetTitle(str);

	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC63);
	str.Format("Total: %d | Success: %d | Failure: %d", nTotalAttempts, nSuccessAttempts, nFailureAttempts);
	pWndStatic->SetTitle(str);
}
BOOL CWndSmeltSafety::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY, 0, CPoint(0, 0), pWndParent);
}
void CWndSmeltSafety::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();

	wndCtrlElemMaterial = GetWndCtrl(WIDC_STATIC11);
	wndElemScroll_1 = GetWndCtrl(WIDC_STATIC41);
	wndElemScroll_2 = GetWndCtrl(WIDC_STATIC61);

	m_pNowGaugeTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyGauge.bmp"), 0xffff00ff);
	m_pSuccessTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetySuccess.bmp"), 0xffff00ff);
	m_pFailureTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "SafetyFailure.bmp"), 0xffff00ff);

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_NOW_GRADE);
	pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_NOW_GRADE));

	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_TITLE_MAX_GRADE);
	pWndStatic->SetTitle(prj.GetText(TID_GAME_SMELT_SAFETY_MAX_GRADE));

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	pWndEdit->AddWndStyle(EBS_NUMBER);
	pWndEdit->EnableWindow(FALSE);
	pWndEdit->SetString(TEXT(""));

	RefreshText();

	switch (m_eWndMode)
	{
	case WND_NORMAL:
		SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_NORMAL));
		break;
	case WND_ACCESSARY:
		SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_ACCESSARY));
		break;
	case WND_PIERCING:
		SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_PIERCING));
		break;
	case WND_ELEMENT:
		SetTitle(prj.GetText(TID_GAME_SMELTSAFETY_ELEMENT));
		break;
	case WND_UNIQUE:
		SetTitle(prj.GetText(TID_MMI_SMELT_CHANGEGWEAPON));
		break;
	case WND_ULTIMATE:
		SetTitle(prj.GetText(TID_MMI_SMELT_CHANGEUWEAPON));
		break;
	}

	g_WndMng.CreateApplet(APP_INVENTORY);
}
BOOL CWndSmeltSafety::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON_START:
	{
		StartProcess(TRUE);
		break;
	}
	case WIDC_BUTTON_STOP:
	{
		StopProcess();
		break;
	}
	case WIDC_BUTTON_RESET:
	{
		if (m_bStart || m_bRealStart)
			break;

		ResetItems();
		break;
	}
	case WIDC_BUTTON_PLUS:
	{
		if (m_pItemElem != NULL)
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
			nMaxSmeltNumber = (++nMaxSmeltNumber < GetDefaultMaxSmeltValue()) ? nMaxSmeltNumber : GetDefaultMaxSmeltValue();
			CString strMaxSmeltNumber;
			strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
			pWndEdit->SetString(strMaxSmeltNumber);
		}
		break;
	}
	case WIDC_BUTTON_MINUS:
	{
		if (m_pItemElem != NULL)
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
			nMaxSmeltNumber = (--nMaxSmeltNumber > 0) ? nMaxSmeltNumber : 0;
			CString strMaxSmeltNumber;
			strMaxSmeltNumber.Format("%d", nMaxSmeltNumber);
			pWndEdit->SetString(strMaxSmeltNumber);
		}
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndSmeltSafety::Process()
{
	if (CMover::GetActiveMover()->m_vtInfo.GetOther() || CMover::GetActiveMover()->m_vtInfo.VendorIsVendor())
		Destroy();

	if (m_bStart)
	{
		if (m_dwEnchantTimeEnd < g_tmCurrent)
		{
			if (!IsPossibleStart())
			{
				g_WndMng.PutString("Not enough resources or penya, or upgrade goal is done !");
				StopProcess();
				return FALSE;
			}

			if (m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE)
			{
				g_DPlay.SendUltimateTransWeapon(
					m_pItemElem->m_dwObjId,
					m_pItemElemScroll_1->m_dwObjId,
					m_pItemElemMaterial->m_dwObjId,
					m_pItemElemScroll_2->m_dwObjId);
			}
			else
			{
				g_DPlay.SendSmeltSafety(
					m_pItemElem->m_dwObjId,
					m_pItemElemMaterial->m_dwObjId,
					m_pItemElemScroll_1->m_dwObjId,
					m_pItemElemScroll_2 != NULL ? m_pItemElemScroll_2->m_dwObjId : NULL_ID);
			}

			StopProcess(TRUE);
		}

	}
	else
	{
		if (m_dwEnchantTimeStart && g_tmCurrent >= m_dwEnchantTimeStart)
		{
			m_bStart = TRUE;
			m_dwEnchantTimeEnd = m_dwEnchantTimeStart + 100;
		}
	}

	return TRUE;
}
BOOL CWndSmeltSafety::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!pWndFrame)
		return FALSE;

	if (pWndFrame->GetWndId() != APP_INVENTORY)
	{
		SetForbid(TRUE);
		return FALSE;
	}

	CRect WndRect = GetClientRect();
	if (WndRect.PtInRect(point))
	{
		CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		SetItem(pTempElem);
	}

	return TRUE;
}
void CWndSmeltSafety::OnDraw(C2DRender* p2DRender)
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CRect rectSmeltItem = pWndStatic->GetWndRect();

	if (m_pItemElem && m_pItemElemTexture)
	{
		m_pItemElemTexture->Render(p2DRender, CPoint(rectSmeltItem.left, rectSmeltItem.top));

		if (m_bStart)
			RenderEnchant(p2DRender, CPoint(rectSmeltItem.left, rectSmeltItem.top));
	}

	CPoint pointMouse = GetMousePoint();
	if (rectSmeltItem.PtInRect(pointMouse) != FALSE)
	{
		if (m_pItemElem)
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectSmeltItem);
			g_WndMng.PutToolTip_Item(m_pItemElem, pointMouse, &rectSmeltItem);
		}
		else
		{
			ClientToScreen(&pointMouse);
			ClientToScreen(&rectSmeltItem);
			CString strEmptyTooltip;
			strEmptyTooltip.Format("%s", prj.GetText(TID_GAME_TOOLTIP_SMELT_SAFETY_ITEM));
			g_toolTip.PutToolTip(reinterpret_cast<DWORD>(this), strEmptyTooltip, rectSmeltItem, pointMouse);
		}
	}

	DrawListItem(p2DRender);

	// Draw last result image
	if (!m_bStart)
	{
		CRect forimagerect = GetWndCtrl(WIDC_CUSTOM2)->rect;
		if (nLastResult == 1 && m_pSuccessTexture)
			m_pSuccessTexture->Render(p2DRender, CPoint(forimagerect.left, forimagerect.top), 255);
		else if (nLastResult == 2 && m_pFailureTexture)
			m_pFailureTexture->Render(p2DRender, CPoint(forimagerect.left, forimagerect.top), 255);
	}
	else // Gauge
	{
		float m_fGaugeRate = static_cast<float>(g_tmCurrent - m_dwEnchantTimeStart) / static_cast<float>(m_dwEnchantTimeEnd - m_dwEnchantTimeStart);
		const int nExtensionPixel = 0;
		static CRect rectStaticTemp;
		LPWNDCTRL lpStatic = GetWndCtrl(WIDC_CUSTOM2);
		rectStaticTemp.TopLeft().y = lpStatic->rect.top;
		rectStaticTemp.TopLeft().x = lpStatic->rect.left + nExtensionPixel;
		rectStaticTemp.BottomRight().y = lpStatic->rect.bottom;
		int nGaugeWidth(lpStatic->rect.left + static_cast<int>(static_cast<float>(lpStatic->rect.right - lpStatic->rect.left) * m_fGaugeRate));
		nGaugeWidth = nGaugeWidth < lpStatic->rect.right ? nGaugeWidth : lpStatic->rect.right;
		rectStaticTemp.BottomRight().x = nGaugeWidth + nExtensionPixel;
		assert(m_pVertexBufferGauge != NULL);
		m_pTheme->RenderGauge(p2DRender, &rectStaticTemp, NULL_ID, m_pVertexBufferGauge, m_pNowGaugeTexture);
	}


	// Draw material and scrolls number
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

	CString str;
	CSize size;

	if (m_pItemElemMaterial)
	{
		str.Format("%d", m_pItemElemMaterial->GetExtra());
		size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(wndCtrlElemMaterial->rect.right - size.cx - 2, wndCtrlElemMaterial->rect.bottom - size.cy, str, 0xffeaebdd);
	}

	if (m_pItemElemScroll_1)
	{
		str.Format("%d", m_pItemElemScroll_1->GetExtra());
		size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(wndElemScroll_1->rect.right - size.cx - 2, wndElemScroll_1->rect.bottom - size.cy, str, 0xffeaebdd);
	}

	if (m_pItemElemScroll_2)
	{
		str.Format("%d", m_pItemElemScroll_2->GetExtra());
		size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(wndElemScroll_2->rect.right - size.cx - 2, wndElemScroll_2->rect.bottom - size.cy, str, 0xffeaebdd);
	}

	p2DRender->SetFont(pOldFont);
}
void CWndSmeltSafety::DrawListItem(C2DRender* p2DRender)
{
	if ((m_eWndMode == WND_NORMAL || m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE) && (m_pItemElem == NULL || m_pItemElemTexture == NULL))
		return;

	static const int NORMAL_ALPHA(255);
	static const int TRANSLUCENT_ALPHA(75);
	static int nAlphaBlend(NORMAL_ALPHA);
	ItemProp* pItemProp = NULL;
	CTexture* pTexture = NULL;

	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_GEN_MAT_ORICHALCUM02) : prj.GetItemProp(II_GEN_MAT_ORICHALCUM01);
		break;
	}
	case WND_ACCESSARY:
	{
		pItemProp = prj.GetItemProp(II_GEN_MAT_MOONSTONE);
		break;
	}
	case WND_PIERCING:
	{
		pItemProp = prj.GetItemProp(II_GEN_MAT_MOONSTONE);
		break;
	}
	case WND_ELEMENT:
	{
		pItemProp = m_pSelectedElementalCardItemProp;
		if (m_pItemElem)
		{
			switch (m_pItemElem->GetItemResist())
			{
			case SAI79::FIRE:
			{
				pItemProp = prj.GetItemProp(II_GEN_MAT_ELE_FLAME);
				break;
			}
			case SAI79::WATER:
			{
				pItemProp = prj.GetItemProp(II_GEN_MAT_ELE_RIVER);
				break;
			}
			case SAI79::ELECTRICITY:
			{
				pItemProp = prj.GetItemProp(II_GEN_MAT_ELE_GENERATOR);
				break;
			}
			case SAI79::WIND:
			{
				pItemProp = prj.GetItemProp(II_GEN_MAT_ELE_CYCLON);
				break;
			}
			case SAI79::EARTH:
			{
				pItemProp = prj.GetItemProp(II_GEN_MAT_ELE_DESERT);
				break;
			}
			}
		}
		break;
	}
	case WND_UNIQUE:
	case WND_ULTIMATE:
	{
		pItemProp = prj.GetItemProp(II_GEN_MAT_ORICHALCUM02);
		break;
	}
	}
	if (m_eWndMode != WND_ELEMENT || pItemProp)
	{
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		nAlphaBlend = m_pItemElemMaterial != NULL ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render(p2DRender, CPoint(wndCtrlElemMaterial->rect.left, wndCtrlElemMaterial->rect.top), nAlphaBlend);
	}


	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		pItemProp = (m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE) ? prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT3) : prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT);
		break;
	}
	case WND_ACCESSARY:
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT4);
		break;
	}
	case WND_PIERCING:
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_PIEPROT);
		break;
	}
	case WND_ELEMENT:
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT);
		break;
	}
	case WND_UNIQUE:
	case WND_ULTIMATE:
	{
		int nLevel = m_pItemElem->GetProp()->dwLimitLevel1;
		if (nLevel >= 60 && nLevel <= 70)
			pItemProp = prj.GetItemProp(II_GEN_MAT_TOPAZ);
		else if (nLevel >= 71 && nLevel <= 85)
			pItemProp = prj.GetItemProp(II_GEN_MAT_RUBY);
		else if (nLevel >= 86 && nLevel <= 100)
			pItemProp = prj.GetItemProp(II_GEN_MAT_SAPPHIRE);
		else if (nLevel >= 101 && nLevel <= 119)
			pItemProp = prj.GetItemProp(II_GEN_MAT_EMERALD);
		else
			pItemProp = prj.GetItemProp(II_GEN_MAT_DIAMOND);

		break;
	}
	}
	pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
	nAlphaBlend = m_pItemElemScroll_1 != NULL ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
	pTexture->Render(p2DRender, CPoint(wndElemScroll_1->rect.left, wndElemScroll_1->rect.top), nAlphaBlend);

	if (m_eWndMode == WND_NORMAL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE && m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL)
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELTING);
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		nAlphaBlend = m_pItemElemScroll_2 != NULL ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render(p2DRender, CPoint(wndElemScroll_2->rect.left, wndElemScroll_2->rect.top), nAlphaBlend);
	}
	if (m_eWndMode == WND_ELEMENT && m_pItemElem && m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL)
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELTING2);
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		nAlphaBlend = m_pItemElemScroll_2 != NULL ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render(p2DRender, CPoint(wndElemScroll_2->rect.left, wndElemScroll_2->rect.top), nAlphaBlend);
	}

	if ((m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE) && m_pItemElem)
	{
		pItemProp = prj.GetItemProp(II_SYS_SYS_SCR_SMELPROT3);
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
		nAlphaBlend = m_pItemElemScroll_2 != NULL ? NORMAL_ALPHA : TRANSLUCENT_ALPHA;
		pTexture->Render(p2DRender, CPoint(wndElemScroll_2->rect.left, wndElemScroll_2->rect.top), nAlphaBlend);
	}
}
void CWndSmeltSafety::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bStart || m_bRealStart)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR12), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR12));
		return;
	}

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CRect rect = pWndStatic->GetWndRect();

	if (rect.PtInRect(point))
		ResetItems();
	else if (wndCtrlElemMaterial->rect.PtInRect(point))
	{
		if (m_pItemElemMaterial)
		{
			m_pItemElemMaterial->SetExtra(0);
			m_pItemElemMaterial = NULL;
		}
	}
	else if (wndElemScroll_1->rect.PtInRect(point))
	{
		if (m_pItemElemScroll_1)
		{
			m_pItemElemScroll_1->SetExtra(0);
			m_pItemElemScroll_1 = NULL;
		}
	}
	else if (wndElemScroll_2->rect.PtInRect(point))
	{
		if (m_pItemElemScroll_2)
		{
			m_pItemElemScroll_2->SetExtra(0);
			m_pItemElemScroll_2 = NULL;
		}
	}
}
void CWndSmeltSafety::SetItem(CItemElem* pItemElem)
{
	if (m_bStart || m_bRealStart)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR12), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR12));
		return;
	}

	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pItemProp = pItemElem->GetProp();
	if (!pItemProp)
		return;

	if (!m_pItemElem)
	{
		BOOL bAcceptableItem = FALSE;
		switch (m_eWndMode)
		{
		case WND_NORMAL:
		{
			if (CItemElem::IsDiceRefineryAble(pItemProp))
				bAcceptableItem = TRUE;
			break;
		}
		case WND_ACCESSARY:
		{
			if (pItemProp->dwItemKind2 == IK2_JEWELRY)
				bAcceptableItem = TRUE;
			break;
		}
		case WND_PIERCING:
		{
			if (pItemElem->IsPierceAble() && !pItemElem->IsEatPet())
				bAcceptableItem = TRUE;
			break;
		}
		case WND_ELEMENT:
		{
			if (pItemProp->IsEleRefineryAble())
				bAcceptableItem = TRUE;
			break;
		}
		case WND_UNIQUE:
		{
			if (pItemProp->dwItemKind1 == IK1_WEAPON && pItemProp->dwLimitLevel1 >= 60)
			{
				if (pItemProp->dwReferStat1 == WEAPON_GENERAL)
					bAcceptableItem = TRUE;
			}
			break;
		}
		case WND_ULTIMATE:
		{
			if (pItemProp->dwItemKind1 == IK1_WEAPON && pItemProp->dwLimitLevel1 >= 60)
			{
				if (pItemProp->dwReferStat1 == WEAPON_UNIQUE && pItemElem->GetAbilityOption() >= 10)
					bAcceptableItem = TRUE;
			}
			break;
		}
		}



		if (!bAcceptableItem)
			g_WndMng.PutString("You cant use this item");
		else
		{
			m_pItemElem = pItemElem;
			m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
			m_pItemElemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);

			RefreshText();

			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
			CString strDefaultSmeltMaxValue;
			strDefaultSmeltMaxValue.Format("%d", GetDefaultMaxSmeltValue());
			pWndEdit->SetString(strDefaultSmeltMaxValue);
		}
	}
	else
	{
		if (IsAcceptableMaterial(pItemProp))
		{
			if (m_pItemElemMaterial)
			{
				m_pItemElemMaterial->SetExtra(0);
				m_pItemElemMaterial = NULL;
			}

			m_pItemElemMaterial = pItemElem;
			m_pItemElemMaterial->SetExtra(m_pItemElemMaterial->m_nItemNum);
		}
		else if (IsAcceptableScroll1(pItemProp))
		{
			if (m_pItemElemScroll_1)
			{
				m_pItemElemScroll_1->SetExtra(0);
				m_pItemElemScroll_1 = NULL;
			}

			m_pItemElemScroll_1 = pItemElem;
			m_pItemElemScroll_1->SetExtra(m_pItemElemScroll_1->m_nItemNum);
		}
		else if ((m_eWndMode == WND_NORMAL || m_eWndMode == WND_ELEMENT || m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE) && IsAcceptableScroll2(pItemProp))
		{
			if (m_pItemElemScroll_2)
			{
				m_pItemElemScroll_2->SetExtra(0);
				m_pItemElemScroll_2 = NULL;
			}

			m_pItemElemScroll_2 = pItemElem;
			m_pItemElemScroll_2->SetExtra(m_pItemElemScroll_2->m_nItemNum);
		}
		else
		{
			if (m_eWndMode == WND_ELEMENT && CItemElem::IsElementalCard(pItemProp->dwID) == TRUE)
			{
				if (m_pItemElem->GetItemResist() != SAI79::NO_PROP)
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR18), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR18));
				else if (pItemProp != m_pSelectedElementalCardItemProp)
					g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR19), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR19));
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_SMELT_SAFETY_ERROR06), NULL, prj.GetTextColor(TID_GAME_SMELT_SAFETY_ERROR06));
		}
	}
}
BOOL CWndSmeltSafety::IsAcceptableMaterial(ItemProp* pItemProp)
{
	if (!m_pItemElem)
		return FALSE;

	BOOL bAcceptableItem(FALSE);
	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		if (m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
		{
			if (pItemProp->dwID == II_GEN_MAT_ORICHALCUM01)
				bAcceptableItem = TRUE;
		}
		else
		{
			if (pItemProp->dwID == II_GEN_MAT_ORICHALCUM02)
				bAcceptableItem = TRUE;
		}
		break;
	}
	case WND_ACCESSARY:
	case WND_PIERCING:
	{
		if (pItemProp->dwID == II_GEN_MAT_MOONSTONE)
			bAcceptableItem = TRUE;
		break;
	}
	case WND_ELEMENT:
	{
		switch (m_pItemElem->GetItemResist())
		{
		case SAI79::FIRE:
		{
			if (pItemProp->dwID == II_GEN_MAT_ELE_FLAME)
				bAcceptableItem = TRUE;
			break;
		}
		case SAI79::WATER:
		{
			if (pItemProp->dwID == II_GEN_MAT_ELE_RIVER)
				bAcceptableItem = TRUE;
			break;
		}
		case SAI79::ELECTRICITY:
		{
			if (pItemProp->dwID == II_GEN_MAT_ELE_GENERATOR)
				bAcceptableItem = TRUE;
			break;
		}
		case SAI79::WIND:
		{
			if (pItemProp->dwID == II_GEN_MAT_ELE_CYCLON)
				bAcceptableItem = TRUE;
			break;
		}
		case SAI79::EARTH:
		{
			if (pItemProp->dwID == II_GEN_MAT_ELE_DESERT)
				bAcceptableItem = TRUE;
			break;
		}
		default:
		{
			if (CItemElem::IsElementalCard(pItemProp->dwID) == TRUE)
			{
				m_pSelectedElementalCardItemProp = pItemProp;
				bAcceptableItem = TRUE;
			}
		}
		}
		break;
	}
	case WND_UNIQUE:
	case WND_ULTIMATE:
	{
		if (pItemProp->dwID == II_GEN_MAT_ORICHALCUM02)
			bAcceptableItem = TRUE;

		break;
	}
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll1(ItemProp* pItemProp)
{
	if (!m_pItemElem)
		return FALSE;

	BOOL bAcceptableItem(FALSE);
	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		if (m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
		{
			if (pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT)
				bAcceptableItem = TRUE;
		}
		else
		{
			if (pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT3)
				bAcceptableItem = TRUE;
		}
		break;
	}
	case WND_ACCESSARY:
	{
		if (pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT4)
			bAcceptableItem = TRUE;
		break;
	}
	case WND_PIERCING:
	{
		if (pItemProp->dwID == II_SYS_SYS_SCR_PIEPROT)
			bAcceptableItem = TRUE;
		break;
	}
	case WND_ELEMENT:
	{
		if (pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT)
			bAcceptableItem = TRUE;
		break;
	}
	case WND_UNIQUE:
	case WND_ULTIMATE:
	{
		DWORD dwID = NULL_ID;
		int nLevel = m_pItemElem->GetProp()->dwLimitLevel1;
		if (nLevel >= 60 && nLevel <= 70)
			dwID = II_GEN_MAT_TOPAZ;
		else if (nLevel >= 71 && nLevel <= 85)
			dwID = II_GEN_MAT_RUBY;
		else if (nLevel >= 86 && nLevel <= 100)
			dwID = II_GEN_MAT_SAPPHIRE;
		else if (nLevel >= 101 && nLevel <= 119)
			dwID = II_GEN_MAT_EMERALD;
		else
			dwID = II_GEN_MAT_DIAMOND;

		if (pItemProp->dwID == dwID)
			bAcceptableItem = TRUE;

		break;
	}
	}
	return bAcceptableItem;
}

BOOL CWndSmeltSafety::IsAcceptableScroll2(ItemProp* pItemProp)
{
	if (!m_pItemElem)
		return FALSE;

	BOOL bAcceptableItem(FALSE);
	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		if (m_pItemElem->GetAbilityOption() < GENERAL_NON_USING_SCROLL2_LEVEL && m_pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE)
		{
			if (pItemProp->dwID == II_SYS_SYS_SCR_SMELTING)
				bAcceptableItem = TRUE;
		}
		break;
	}
	case WND_ELEMENT:
	{
		if (m_pItemElem->GetResistAbilityOption() < ELEMENTAL_NON_USING_SCROLL2_LEVEL)
		{
			if (pItemProp->dwID == II_SYS_SYS_SCR_SMELTING2)
				bAcceptableItem = TRUE;
		}
		break;
	}
	case WND_UNIQUE:
	case WND_ULTIMATE:
	{
		if (pItemProp->dwID == II_SYS_SYS_SCR_SMELPROT3)
			bAcceptableItem = TRUE;

		break;
	}
	}
	return bAcceptableItem;
}
int CWndSmeltSafety::GetNowSmeltValue(void)
{
	int nNowSmeltValue(0);
	if (m_pItemElem)
	{
		if (m_eWndMode == WND_NORMAL || m_eWndMode == WND_ACCESSARY)
			nNowSmeltValue = m_pItemElem->GetAbilityOption();
		else if (m_eWndMode == WND_PIERCING)
			nNowSmeltValue = m_pItemElem->GetPiercingSize();
		else if (m_eWndMode == WND_ELEMENT)
			nNowSmeltValue = m_pItemElem->GetResistAbilityOption();
	}
	return nNowSmeltValue;
}
int CWndSmeltSafety::GetDefaultMaxSmeltValue(void)
{
	if (!m_pItemElem)
		return FALSE;

	if (m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE)
		return TRUE;

	int nDefaultMaxSmeltValue(0);
	switch (m_eWndMode)
	{
	case WND_NORMAL:
	{
		if(m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)
			nDefaultMaxSmeltValue = 20;
		else
		nDefaultMaxSmeltValue = 10;
		break;
	}
	case WND_ACCESSARY:
	{
		nDefaultMaxSmeltValue = 20;
		break;
	}
	case WND_PIERCING:
	{
		if (m_pItemElem->GetProp()->dwItemKind3 == IK3_SUIT)
			nDefaultMaxSmeltValue = 4;
		else
			nDefaultMaxSmeltValue = 10;
		break;
	}
	case WND_ELEMENT:
	{
		nDefaultMaxSmeltValue = 20;
		break;
	}
	}
	return nDefaultMaxSmeltValue;
}
void CWndSmeltSafety::DisableScroll2(void)
{
	if (!m_pItemElem)
		return;

	if ((m_eWndMode == WND_NORMAL && (m_pItemElem->GetAbilityOption() >= GENERAL_NON_USING_SCROLL2_LEVEL || m_pItemElem->GetProp()->dwReferStat1 == WEAPON_ULTIMATE)) ||
		(m_eWndMode == WND_ELEMENT && m_pItemElem->GetResistAbilityOption() >= ELEMENTAL_NON_USING_SCROLL2_LEVEL))
	{
		if (m_pItemElemScroll_2)
		{
			m_pItemElemScroll_2->SetExtra(0);
			m_pItemElemScroll_2 = NULL;
		}
	}
}
HRESULT CWndSmeltSafety::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (m_pVertexBufferGauge == NULL)
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			D3DFVF_TEXTUREVERTEX2,
			D3DPOOL_DEFAULT,
			&m_pVertexBufferGauge,
			NULL);
	assert(m_pVertexBufferGauge != NULL);
	return S_OK;
}
HRESULT CWndSmeltSafety::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVertexBufferGauge);
	return S_OK;
}
HRESULT CWndSmeltSafety::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVertexBufferGauge);
	return S_OK;
}
BOOL CWndSmeltSafety::IsPossibleStart()
{
	if (!m_pItemElem || !m_pItemElemMaterial || !m_pItemElemScroll_1)
		return FALSE;

	if (m_eWndMode == WND_UNIQUE || m_eWndMode == WND_ULTIMATE)
	{
		if (!m_pItemElemScroll_2)
			return FALSE;
	}
	else
	{
		if (GetNowSmeltValue() == GetDefaultMaxSmeltValue())
			return FALSE;
	}

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT_MAX_GRADE);
	int nMaxSmeltNumber(atoi(pWndEdit->GetString()));
	if (GetNowSmeltValue() >= nMaxSmeltNumber)
		return FALSE;

	if (m_eWndMode == WND_PIERCING)
	{
		if (g_pPlayer && g_pPlayer->GetGold() < 100000)
			return FALSE;
	}

	return TRUE;
}

CWndSmeltSafetyConfirm::CWndSmeltSafetyConfirm(ErrorMode eErrorMode) :
	m_eErrorMode(eErrorMode),
	m_pItemElem(NULL)
{
}

CWndSmeltSafetyConfirm::~CWndSmeltSafetyConfirm()
{
}

BOOL CWndSmeltSafetyConfirm::Initialize(CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SMELT_SAFETY_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

void CWndSmeltSafetyConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_CONFIRM_TEXT);
	assert(pWndText != NULL);

	switch (m_eErrorMode)
	{
	case WND_ERROR1:
	{
		pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_NORMAL));
		break;
	}
	case WND_ERROR2:
	{
		pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_HIGHEST));
		break;
	}
	case WND_ERROR3:
	{
		pWndText->AddString(prj.GetText(TID_GAME_SMELT_SAFETY_CONFIRM_ACCESSARY));
		break;
	}
	}

	MoveParentCenter();
}

BOOL CWndSmeltSafetyConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);

	if (pWndInventory != NULL)
	{
		if (nID == WIDC_SMELT_YES)
		{
			if (m_pItemElem != NULL)
				pWndInventory->RunUpgrade(m_pItemElem);

			Destroy();
		}
		else if (nID == WIDC_SMELT_NO)
		{
			pWndInventory->BaseMouseCursor();
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndSmeltSafetyConfirm::OnDestroy()
{
	CWndInventory* pWndInventory = (CWndInventory*)GetWndBase(APP_INVENTORY);
	if (pWndInventory != NULL)
		pWndInventory->BaseMouseCursor();
}

void CWndSmeltSafetyConfirm::SetWndMode(CItemElem* pItemElem)
{
	m_pItemElem = pItemElem;
}


#if __VER >= 14 // __RESTATE_CONFIRM
CWndRestateConfirm::CWndRestateConfirm(DWORD dwItemID) :
	m_dwItemID(dwItemID)
{
}

CWndRestateConfirm::~CWndRestateConfirm(void)
{
}

BOOL CWndRestateConfirm::Initialize(CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_RESTATE_CONFIRM, 0, CPoint(0, 0), pWndParent);
}

void CWndRestateConfirm::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_RESTATE_CONFIRM_TEXT);
	assert(pWndText != NULL);
	switch (m_dwItemID)
	{
	case II_CHR_SYS_SCR_RESTATE:
	{
		pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_ALL_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_STR:
	{
		pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STR_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_STA:
	{
		pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_STA_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_DEX:
	{
		pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_DEX_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_INT:
	{
		pWndText->AddString(prj.GetText(TID_TOOLTIP_RESTATE_INT_CONFIRM));
		break;
	}
#ifdef __ADD_RESTATE_LOW
	case II_CHR_SYS_SCR_RESTATE_STR_LOW:
	{
		pWndText->AddString(prj.GetText(TID_GAME_RESTATE_STR_LOW_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_STA_LOW:
	{
		pWndText->AddString(prj.GetText(TID_GAME_RESTATE_STA_LOW_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_DEX_LOW:
	{
		pWndText->AddString(prj.GetText(TID_GAME_RESTATE_DEX_LOW_CONFIRM));
		break;
	}
	case II_CHR_SYS_SCR_RESTATE_INT_LOW:
	{
		pWndText->AddString(prj.GetText(TID_GAME_RESTATE_INT_LOW_CONFIRM));
		break;
	}
#endif // __ADD_RESTATE_LOW
	}

	MoveParentCenter();
}

BOOL CWndRestateConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_RESTATE_CONFIRM_YES:
	{
		g_DPlay.SendDoUseItem(m_dwItemObjID);
		Destroy();
		break;
	}
	case WIDC_RESTATE_CONFIRM_NO:
	{
		Destroy();
		break;
	}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndRestateConfirm::SetInformation(DWORD dwItemObjID)
{
	m_dwItemObjID = dwItemObjID;
}
#endif //__RESTATE_CONFIRM

#ifdef __FL_DUNGEON_TREASURE
CWndDungeonTreasure::CWndDungeonTreasure()
	: dwCloseTime(NULL)
{
	m_bNoCloseButton = TRUE;
}

CWndDungeonTreasure::~CWndDungeonTreasure(void)
{
}

BOOL CWndDungeonTreasure::Initialize(CWndBase* pWndParent, DWORD nType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DUNGEON_TREASURE, 0, CPoint(0, 0), pWndParent);
}

void CWndDungeonTreasure::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndButton[3] = (CWndButton*)GetDlgItem(WIDC_BUTTON3);

	for (int i = 0; i < 4; i++)
		pWndButton[i]->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("dungeontreasureclosed.tga")), TRUE);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString("You have a chance to try your luck and get a reward for completing this dungeon! Select one of the 4 treasures by clicking on it 1 time. Please note: if you leave the dungeon or exit the game, you will lose this chance!");

	MoveParentCenter();
}

BOOL CWndDungeonTreasure::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int nSelectedTreasure = -1;
	switch (nID)
	{
	case WIDC_BUTTON: nSelectedTreasure = 1; break;
	case WIDC_BUTTON1: nSelectedTreasure = 2; break;
	case WIDC_BUTTON2: nSelectedTreasure = 3; break;
	case WIDC_BUTTON3: nSelectedTreasure = 4; break;
	default: nSelectedTreasure = -1; break;
	}

	if (nSelectedTreasure != -1)
		g_DPlay.SendDungeonTreasureSelect(nSelectedTreasure);

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndDungeonTreasure::Process(void)
{
	if (dwCloseTime != NULL)
	{
		if (GetTickCount() - SEC(10) > dwCloseTime)
			Destroy();
	}

	return TRUE;
}
void CWndDungeonTreasure::ReceiveResult(int nTreasureWin, BOOL bUserWin)
{
	for (int i = 0; i < 4; i++)
	{
		pWndButton[i]->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("dungeontreasureempty.tga")), TRUE);
		pWndButton[i]->EnableWindow(FALSE);
	}
	pWndButton[nTreasureWin - 1]->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("dungeontreasurereward.tga")), TRUE);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if (bUserWin)
		pWndText->SetString("Congratulations! Your choice turned out to be correct. Your reward will appear in the inventory or will be sent to the mail. This window will be closed automatically in 10 sec.");
	else
		pWndText->SetString("Sorry, but your choice turned out to be wrong. This window will be closed automatically in 10 sec.");

	dwCloseTime = GetTickCount();
}
#endif

#ifdef __FL_BATTLE_PASS
CWndBattlePass::CWndBattlePass(void)
	: m_nPage(0)
	, m_nBPnextEXP(1)
{

}

CWndBattlePass::~CWndBattlePass(void)
{

}
void CWndBattlePass::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(m_Theme.m_pFontWorld);

	CString str;
	CPoint point = GetMousePoint();

	// Progress Bar Rendering (current level -> next level)
	CRect rectBar = GetWndCtrl(WIDC_CUSTOM1)->rect;
	int nWidthClient = rectBar.Width();
	int nWidth = nWidthClient * g_pPlayer->GetBattlePassExp() / m_nBPnextEXP;
	if (nWidth > nWidthClient)
		nWidth = nWidthClient;

	CRect rectColor = rectBar;
	rectColor.right = rectColor.left + nWidth;

	CRect rectWhite = rectBar;
	rectWhite.left = rectColor.right;

	p2DRender->RenderFillRect(rectColor, D3DCOLOR_ARGB(225, 67, 215, 67), D3DCOLOR_ARGB(225, 67, 215, 67), D3DCOLOR_ARGB(225, 41, 71, 39), D3DCOLOR_ARGB(225, 41, 71, 39));
	p2DRender->RenderFillRect(rectWhite, D3DCOLOR_ARGB(200, 255, 255, 255));
	p2DRender->RenderRect(rectBar, D3DCOLOR_ARGB(150, 0, 0, 0));

	// Render % progress numbers
	if (!rectBar.PtInRect(point))
		str.Format(("%.2f%%"), (float)g_pPlayer->GetBattlePassExp() * 10000 / m_nBPnextEXP / 100.0f);
	else
		str.Format(("%d / %d"), g_pPlayer->GetBattlePassExp(), m_nBPnextEXP);

	CSize sz = p2DRender->m_pFont->GetTextExtent(str);
	p2DRender->TextOut(rectBar.left + rectBar.Width() / 2 - sz.cx / 2, rectBar.top + rectBar.Height() / 2 - sz.cy / 2, str);

	// Render rewards
	p2DRender->SetFont(m_Theme.m_pFontItemNumbers);

	const static int nStaticSlot[] =
	{
	  WIDC_STATIC3, WIDC_STATIC4, WIDC_STATIC5, WIDC_STATIC6, WIDC_STATIC7
	, WIDC_STATIC8, WIDC_STATIC9, WIDC_STATIC10, WIDC_STATIC11, WIDC_STATIC12
	, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16, WIDC_STATIC17
	, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20, WIDC_STATIC21, WIDC_STATIC22
	};

	ItemProp* pProp;
	CTexture* pIcon;
	int	nItemNum;
	BOOL bBind;
	BOOL bReceivedItem;

	for (int i = 0; i < 20; i++)
	{
		int nBpLevelBasedByPage = i + (20 * m_nPage) + 1;
		if (nBpLevelBasedByPage > CBattlePass::GetInstance()->GetMaxBattlePassLevel())
			break;

		pProp = prj.GetItemProp(CBattlePass::GetInstance()->GetRewardIdForThisLevel(nBpLevelBasedByPage));
		nItemNum = CBattlePass::GetInstance()->GetRewardNumForThisLevel(nBpLevelBasedByPage);
		bBind = CBattlePass::GetInstance()->GetRewardBindsForThisLevel(nBpLevelBasedByPage);
		bReceivedItem = nBpLevelBasedByPage <= g_pPlayer->GetBattlePassLevel();

		if (!pProp)
			continue;

		pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		if (!pIcon)
			continue;

		// Render reward icon
		CRect rectReward = ((CWndStatic*)GetDlgItem(nStaticSlot[i]))->GetWindowRect(TRUE);
		p2DRender->RenderTexture(rectReward.TopLeft(), pIcon, bReceivedItem ? 100 : 255, 1.0f, 1.0f);

		// Render reward amount
		str.Format("%d", nItemNum);
		CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
		p2DRender->TextOut(GetWndCtrl(nStaticSlot[i])->rect.right - size.cx - 2, GetWndCtrl(nStaticSlot[i])->rect.bottom - size.cy, str, 0xffeaebdd);

		// Render received rewards texture
		if (bReceivedItem)
		{
			pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_THEME, "WndItemCheck.bmp"), 0xffff00ff);
			if (pIcon)
				pIcon->Render(p2DRender, rectReward.TopLeft());
		}

		// Render reward tooltip
		if (rectReward.PtInRect(point))
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = pProp->dwID;
			itemElem.m_nItemNum = nItemNum;
			if (bBind)
				itemElem.SetFlag(CItemElem::binds);

			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rectReward);
			g_WndMng.PutToolTip_Item(&itemElem, point2, &rectReward, GetWndId());
		}
	}

	// Render "ticket" texture
	pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_THEME, CBattlePass::GetInstance()->IsNotFree() ? "bpticketyes.png" : "bpticketno.png"), 0xffff00ff);
	if (pIcon)
	{
		CRect rectTicket = ((CWndStatic*)GetDlgItem(WIDC_STATIC52))->GetWindowRect(TRUE);
		p2DRender->RenderTexture(rectTicket.TopLeft(), pIcon, 255, 1.0f, 1.0f);
	}

	// Render general information tooltip
	if (g_pPlayer->GetBattlePassLevel() < CBattlePass::GetInstance()->GetMaxBattlePassLevel())
	{
		CRect rectTooltipStatic = ((CWndStatic*)GetDlgItem(WIDC_STATIC56))->GetWindowRect(TRUE);
		if (rectTooltipStatic.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&rectTooltipStatic);
			g_toolTip.PutToolTip(10000, strEdit, rectTooltipStatic, point2, 0);
		}
	}

	// Time Update
	if (CBattlePass::GetInstance()->GetCurrentTimeT() >= 0)
	{
		str.Format(" Season Ends: %.1d Days (%.1dh %.1dm %.1ds)"
			, static_cast<int>(CBattlePass::GetInstance()->GetTimeSpan().GetDays())
			, CBattlePass::GetInstance()->GetTimeSpan().GetHours()
			, CBattlePass::GetInstance()->GetTimeSpan().GetMinutes()
			, CBattlePass::GetInstance()->GetTimeSpan().GetSeconds()
		);
	}
	else
		str.Format(_T(" The season is over."));

	((CWndStatic*)GetDlgItem(WIDC_STATIC1))->SetTitle(str);

	p2DRender->SetFont(pOldFont);
}

void CWndBattlePass::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();

	if (CBattlePass::GetInstance()->GetBattlePassID() == 0)
	{
		g_WndMng.PutString(prj.GetText(TID_GAME_STILLNOTUSE));
		Destroy();
		return;
	}

	CString str;
	str.Format("Battle Pass | Season %d", CBattlePass::GetInstance()->GetBattlePassID());
	SetTitle(str);
	UpdateStaticParams();
	UpdateStatics();
}
void CWndBattlePass::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
BOOL CWndBattlePass::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_BATTLE_PASS);
}

BOOL CWndBattlePass::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndBattlePass::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndBattlePass::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1: UpdatePage(FALSE); break;
		case WIDC_BUTTON2: UpdatePage(TRUE); break;
		default: break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndBattlePass::UpdateStaticParams()
{
	if (!g_pPlayer)
		return;

	// Find exp value which needed to receive next level
	if (g_pPlayer->GetBattlePassLevel() < CBattlePass::GetInstance()->GetMaxBattlePassLevel())
		m_nBPnextEXP = CBattlePass::GetInstance()->GetExpForThisLevel(g_pPlayer->GetBattlePassLevel() + 1);
	else
		m_nBPnextEXP = 1;

	// Set current/max level info
	CString str;
	str.Format("%d / %d", g_pPlayer->GetBattlePassLevel(), CBattlePass::GetInstance()->GetMaxBattlePassLevel());
	((CWndStatic*)GetDlgItem(WIDC_STATIC54))->SetTitle(str);

	// Generate tooltip text
	DWORD dwColor = 0xff000000;
	strEdit.SetString("");

	strEdit.AddString("Mob Hunting: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpPerMonster() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);

	strEdit.AddString("\nBoss Hunting: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpPerBoss() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);

	strEdit.AddString("\nGW Kills: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpGW_Kill() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);

	strEdit.AddString("\nGW MvP: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpGW_MvP() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);

	strEdit.AddString("\nGW Win: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpGW_Win() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);

	strEdit.AddString("\nSR Win: ", dwColor, ESSTY_BOLD);
	str.Format(("%.2f%%"), (float)CBattlePass::GetInstance()->GetExpSR_Win() * 10000 / m_nBPnextEXP / 100.0f);
	strEdit.AddString(str);
}
void CWndBattlePass::UpdatePage(BOOL bNext)
{
	int nTotalLevels = CBattlePass::GetInstance()->GetMaxBattlePassLevel();
	int nMaxPage = -1;
	while (nTotalLevels > 0)
	{
		nMaxPage++;
		nTotalLevels -= 20;
	}

	if (bNext)
	{
		m_nPage++;
		if (m_nPage > nMaxPage)
			m_nPage = 0;
	}
	else
	{
		m_nPage--;
		if (m_nPage < 0)
			m_nPage = nMaxPage;
	}

	UpdateStatics();
}
void CWndBattlePass::UpdateStatics()
{
	const static int nStaticText[] =
	{
	  WIDC_STATIC31, WIDC_STATIC32, WIDC_STATIC33, WIDC_STATIC34, WIDC_STATIC35
	, WIDC_STATIC36, WIDC_STATIC37, WIDC_STATIC38, WIDC_STATIC39, WIDC_STATIC40
	, WIDC_STATIC41, WIDC_STATIC42, WIDC_STATIC43, WIDC_STATIC44, WIDC_STATIC45
	, WIDC_STATIC46, WIDC_STATIC47, WIDC_STATIC48, WIDC_STATIC49, WIDC_STATIC50
	};

	CString str;
	for (int i = 0; i < 20; i++)
	{
		str.Format("Lv. %d", i + (20 * m_nPage) + 1);
		((CWndStatic*)GetDlgItem(nStaticText[i]))->SetTitle(str);
	}
}
#endif

#ifdef __FL_CONSUMABLES_STORAGE
CWndChest::CWndChest()
{

}
CWndChest::~CWndChest()
{

}
void CWndChest::OnDraw(C2DRender* p2DRender)
{
}
void CWndChest::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	// Korean-memes
	if (g_WndMng.GetWndBase(APP_TRADE) || g_WndMng.GetWndBase(APP_POST) || g_WndMng.m_pWndShop || g_WndMng.m_pWndBank || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndBase(APP_VENDOR_REVISION))
	{
		Destroy();
		return;
	}

	CWndTabCtrl* pTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	m_wndItemCtrl.Create(WLVS_ICON, CRect(0, 0, 250, 250), pTabCtrl, 11);
	m_wndItemCtrl.InitItem(&g_pPlayer->m_Chest, APP_CHEST);

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;
	tabTabItem.pszText = GETTEXT(TID_GAME_ITEM);
	tabTabItem.pWndBase = &m_wndItemCtrl;
	pTabCtrl->InsertItem(0, &tabTabItem);

	CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pSearchBox)
		pSearchBox->SetStringBg("Search..");

	MoveParentCenter();
}
BOOL CWndChest::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(792, 130), pWndParent);
}
BOOL CWndChest::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			BYTE nMode = 0;
			for (int i = 0; i < MAX_INVENTORY; i++)
			{
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(i);
				if (!pItemElem)
					continue;

				ItemProp* pProp = pItemElem->GetProp();
				if (!pProp)
					continue;

				if (!g_pPlayer->CanBeUsedOutsideInventory(pProp))
					continue;

				if (g_pPlayer->m_Chest.IsFull(pItemElem, pProp, pItemElem->m_nItemNum))
					continue;

				if (!IsUsableItem(pItemElem) || g_pPlayer->IsUsing(pItemElem))
				{
					nMode = 1;
					break;
				}
				nMode = 2;
			}

			switch (nMode)
			{
			case 0: g_WndMng.PutString("You don't have any suitable items to move."); break;
			case 1: g_WndMng.PutString(prj.GetText(TID_GAME_BANKNOTRADE), NULL, prj.GetTextColor(TID_GAME_BANKNOTRADE)); break;
			case 2:
			{
				g_DPlay.SendHdr(PACKETTYPE_MOVE_ALL_ITEMS_CHEST);
				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
				pButton->EnableWindow(FALSE);
			}
			break;
			}
		}
		else if (nID == WIDC_BUTTON_1)
		{
			CWndEdit* pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
			KillFocus(pSearchBox, NULL);
			pSearchBox->Empty();
		}
	}
	else if (message == WIN_ITEMDROP)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if (!lpShortcut || !lpShortcut->m_pFromWnd)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		if (!pWndFrame)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		BOOL bForbid = TRUE;
		if (nID == 11)
		{
			if (pWndFrame->GetWndId() == APP_INVENTORY)
			{
				SAFE_DELETE(g_WndMng.m_pWndTradeGold);
				if (lpShortcut->m_dwData != 0)
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					if (!pWndItemCtrl)
						return FALSE;

					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if (SelectCount != 1)
						g_WndMng.PutString(prj.GetText(TID_GAME_EQUIPPUT), NULL, prj.GetTextColor(TID_GAME_EQUIPPUT));
					else
					{
						for (int i = 0; i < (int)(SelectCount); i++)
						{
							int nItem = pWndItemCtrl->GetSelectedItem(i);
							pWndItemCtrl->GetItem(nItem);
						}

						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if (!itemElem)
							return FALSE;

						ItemProp* pProp = itemElem->GetProp();
						if (!pProp)
							return FALSE;

						if (pProp->dwItemKind3 == IK3_QUEST)
							g_WndMng.PutString(prj.GetText(TID_GAME_ERROR_DONT_MOVE_QUEST_ITEM_TO_BAG_EX), NULL, prj.GetTextColor(TID_GAME_ERROR_DONT_MOVE_QUEST_ITEM_TO_BAG_EX));
						else
						{
							if (itemElem->m_nItemNum > 1)
							{
								if (itemElem->GetExtra() == 0)
								{
									SAFE_DELETE(g_WndMng.m_pWndTradeGold);
									g_WndMng.m_pWndTradeGold = new CWndTradeGold;
									memcpy(&g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT));
									g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
									g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_CHEST;
									g_WndMng.m_pWndTradeGold->m_pWndBase = this;
									g_WndMng.m_pWndTradeGold->m_nPutSlot = -1;
									g_WndMng.m_pWndTradeGold->Initialize(NULL, APP_TRADE_GOLD);
									g_WndMng.m_pWndTradeGold->MoveParentCenter();
									CWndStatic* pStatic = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_STATIC);
									CWndStatic* pStaticCount = (CWndStatic*)g_WndMng.m_pWndTradeGold->GetDlgItem(WIDC_CONTROL1);
									CString strMain = prj.GetText(TID_GAME_MOVECOUNT);
									CString strCount = prj.GetText(TID_GAME_NUMCOUNT);
									pStatic->m_strTitle = strMain;
									pStaticCount->m_strTitle = strCount;
								}
							}
							else
								g_DPlay.SendMoveItem_Chest(lpShortcut->m_dwId, 1, TRUE);
						}
					}
				}
			}
		}
	}
	else if (message == WIN_DBLCLK)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (pItem)
		{
			CWndBase* pWndPut = m_pCurFocus;
			if (pWndPut)
				g_WndMng.UseItem(pItem, TYPE_CHEST);
		}
	}

#ifdef __CONTAINER_RCLICKS
	else if (message == WNM_ITEMCLICK)
	{
		CItemElem* pItem = (CItemElem*)pLResult;
		if (IsUsableItem(pItem))
		{
			if (g_WndMng.GetWndBase(APP_INVENTORY))
				g_DPlay.SendMoveItem_Chest((BYTE)pItem->m_dwObjId, pItem->m_nItemNum, FALSE);
		}
	}
#endif

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif

#ifdef __FL_DPS_DISPLAY
bool SortDpsFromHighToLow(CWndDpsDisplay::DPS_MEMBERS& first, CWndDpsDisplay::DPS_MEMBERS& second)
{
	return (first.nStaticDpsValue > second.nStaticDpsValue);
}

CWndDpsDisplay::CWndDpsDisplay()
	: dwTargetID(NULL_ID)
	, dwLastDpsUpdate(0)
{
	m_bNoCloseButton = TRUE;
	m_nAlphaCount = 0;
	vecDpsMembers.clear();
}

CWndDpsDisplay::~CWndDpsDisplay()
{
	vecDpsMembers.clear();
}

void CWndDpsDisplay::OnDraw(C2DRender* p2DRender)
{
	// General window rendering
	CRect rectApplet = GetWindowRect();
	p2DRender->RenderRect(rectApplet, D3DCOLOR_ARGB(200, 0, 0, 0));
	p2DRender->RenderFillRect(rectApplet, D3DCOLOR_ARGB(80, 0, 0, 0));

	// Title bar
	CRect rectMain = rectApplet;
	rectMain.left += 1;
	rectMain.top += 1;
	rectMain.right -= 1;
	rectMain.bottom = rectMain.top + WINDOWY;
	p2DRender->RenderFillRect(rectMain, D3DCOLOR_ARGB(130, 0, 0, 0));

	CRect rectColoredTitleBar = rectMain;
	rectColoredTitleBar.bottom = rectColoredTitleBar.top + 1;
	float m_fGaugeRate = static_cast<float>(g_tmCurrent - dwLastDpsUpdate) / static_cast<float>(dwLastDpsUpdate + SEC(UPDATE_DELAY) - dwLastDpsUpdate);
	int nGaugeWidth = rectMain.left + static_cast<int>(static_cast<float>(rectMain.right - rectMain.left) * m_fGaugeRate);
	rectColoredTitleBar.right = nGaugeWidth < rectMain.right ? nGaugeWidth : rectMain.right;
	p2DRender->RenderFillRect(rectColoredTitleBar, D3DCOLOR_ARGB(195, 67, 215, 67), D3DCOLOR_ARGB(195, 67, 215, 67), D3DCOLOR_ARGB(195, 41, 71, 39), D3DCOLOR_ARGB(195, 41, 71, 39));

	CString strTitle;
	strTitle.Format("Damage Stats %d/%d", vecDpsMembers.size() > MAX_DISPLAY_LIST ? MAX_DISPLAY_LIST : vecDpsMembers.size(), MAX_DISPLAY_LIST);
	CSize size;
	size = m_p2DRender->m_pFont->GetTextExtent(strTitle);
	int yTextOffset = rectMain.bottom / 2 - size.cy / 2;
	p2DRender->TextOut(rectApplet.right / 2 - size.cx / 2, yTextOffset + 1, strTitle, D3DCOLOR_ARGB(255, 255, 255, 255));

	// Main info
	CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

	int nMaxDpsNumber = 0;
	int nAmountOfMembers = 0;
	int xIcon = 3;
	int xName = 23;
	int xDps = 0;
	int yLine = 0;

	for (auto it = vecDpsMembers.begin(); it != vecDpsMembers.end(); ++it)
	{
		if (nAmountOfMembers >= MAX_DISPLAY_LIST)
			break;
		else
			nAmountOfMembers++;

		yLine += WINDOWY;
		yTextOffset += WINDOWY;
		rectMain.top += WINDOWY;
		rectMain.bottom += WINDOWY;

		// Colored rects
		if (nMaxDpsNumber > 0)
			rectMain.right = rectApplet.right * it->nStaticDpsValue / nMaxDpsNumber;
		else
			nMaxDpsNumber = it->nStaticDpsValue;

		if (nMaxDpsNumber == 0 || rectMain.right < rectMain.left)
			rectMain.right = rectMain.left;

		p2DRender->RenderFillRect(rectMain, D3DCOLOR_ARGB(175, 215, 67, 67), D3DCOLOR_ARGB(175, 215, 67, 67), D3DCOLOR_ARGB(175, 71, 41, 39), D3DCOLOR_ARGB(175, 71, 41, 39));

		// Job icons
		if (pWndWorld)
		{
			int nJob = it->nMemberJob;
			if (prj.m_aJob[nJob].dwJobType == JTYPE_PRO)
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(xIcon, yTextOffset), (19 + nJob - 6), 0xffffffff, 0.95f, 0.95f);
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_MASTER)
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(xIcon, yTextOffset), (19 + nJob - 16), 0xffffffff, 0.95f, 0.95f);
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_HERO)
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(xIcon, yTextOffset), (19 + nJob - 24), 0xffffffff, 0.95f, 0.95f);
			else if (prj.m_aJob[nJob].dwJobType == JTYPE_LEGEND_HERO)
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(xIcon, yTextOffset), (19 + nJob - 32), 0xffffffff, 0.95f, 0.95f);
			else
				pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(xIcon, yTextOffset), 14 + nJob, 0xffffffff, 0.95f, 0.95f);
		}

		// Name
		strTitle.Format("%s", it->strMemberName);
		p2DRender->TextOut(xName, yTextOffset, strTitle, D3DCOLOR_ARGB(255, 255, 255, 255));

		// Dps
		strTitle.Format("%d", it->nStaticDpsValue);
		strTitle.Format("%s/s", GetNumberFormatEx(strTitle));
		xDps = rectApplet.right - m_p2DRender->m_pFont->GetTextExtent(strTitle).cx - 5;
		p2DRender->TextOut(xDps, yTextOffset, strTitle, D3DCOLOR_ARGB(255, 255, 255, 255));

		// Line
		p2DRender->RenderLine(CPoint(rectApplet.left, yLine), CPoint(rectApplet.right, yLine), D3DCOLOR_ARGB(220, 0, 0, 0));
	}

}

BOOL CWndDpsDisplay::Process()
{
	if (!g_pPlayer)
		return TRUE;

	CWorld* pWorld = g_WorldMng.Get();
	if (pWorld)
	{
		CObj* pFocus = pWorld->GetObjFocus();
		if (IsValidObj(pFocus) && pFocus->GetType() == OT_MOVER && ((CMover*)pFocus)->GetId() != dwTargetID)
		{
			dwTargetID = ((CMover*)pFocus)->GetId();
			dwLastDpsUpdate = GetTickCount();
			vecDpsMembers.clear();
			UpdateSize();
		}
	}

	BOOL bShouldUpdate = FALSE;
	if (GetTickCount() - SEC(UPDATE_DELAY) >= dwLastDpsUpdate)
		bShouldUpdate = TRUE;

	for (auto it = vecDpsMembers.begin(); it != vecDpsMembers.end(); ++it)
	{
		if (bShouldUpdate)
		{
			it->nStaticDpsValue = it->nTotalDamageForDps / UPDATE_DELAY;
			it->nTotalDamageForDps = 0;
		}

		if (GetTickCount() - SEC(REMOVE_DELAY) >= it->dwLastHitTime)
		{
			vecDpsMembers.erase(it);
			UpdateSize();
			break;
		}
	}

	if (bShouldUpdate)
	{
		dwLastDpsUpdate = GetTickCount();
		std::sort(vecDpsMembers.begin(), vecDpsMembers.end(), SortDpsFromHighToLow);
	}

	return TRUE;
}

void CWndDpsDisplay::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	int nAddedValue = vecDpsMembers.size() > MAX_DISPLAY_LIST ? MAX_DISPLAY_LIST : vecDpsMembers.size();
	AdjustMinRect(&rectWnd, WINDOWX, WINDOWY + nAddedValue * WINDOWY + 2);
	AdjustMaxRect(&rectWnd, WINDOWX, WINDOWY + nAddedValue * WINDOWY + 2);
	CWndNeuz::SetWndRect(rectWnd, bOnSize);

}
void CWndDpsDisplay::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	UpdateSize();
	m_ButtonsMenu.CreateMenu(this);
	MoveParentCenter();
}

BOOL CWndDpsDisplay::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_DPS_DISPLAY);
}

void CWndDpsDisplay::OnRButtonDown(UINT nFlags, CPoint point)
{
	CPoint point2 = GetMousePoint();
	ClientToScreen(&point2);

	m_ButtonsMenu.DeleteAllMenu();
	m_ButtonsMenu.AppendMenu(0, 0, "Close window");
	m_ButtonsMenu.Move(point2);
	m_ButtonsMenu.SetVisible(TRUE);
	m_ButtonsMenu.SetFocus();
}
BOOL CWndDpsDisplay::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_ButtonsMenu)
	{
		if (nID == 0)
			Destroy();

		m_ButtonsMenu.SetVisible(FALSE);
	}

	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndDpsDisplay::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndDpsDisplay::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndDpsDisplay::Add(CString str, int nJob, DWORD nNum)
{
	BOOL bFound = FALSE;
	for (auto it = vecDpsMembers.begin(); it != vecDpsMembers.end(); ++it)
	{
		if (it->strMemberName == str)
		{
			it->nTotalDamageForDps += nNum;
			it->dwLastHitTime = GetTickCount();

			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		DPS_MEMBERS members;
		members.strMemberName = str;
		members.nMemberJob = nJob;
		members.nTotalDamageForDps = nNum;
		members.nStaticDpsValue = 0;
		members.dwLastHitTime = GetTickCount();

		vecDpsMembers.push_back(members);
		UpdateSize();
	}
}
void CWndDpsDisplay::UpdateSize()
{
	int nAddedValue = vecDpsMembers.size() > MAX_DISPLAY_LIST ? MAX_DISPLAY_LIST : vecDpsMembers.size();
	CRect rect = GetWindowRect(TRUE);
	rect.bottom = WINDOWY + nAddedValue * WINDOWY + 2;
	SetWndRect(rect);
}
#endif

CWndSkill_16::CWndSkill_16()
{
	m_pFocusItem = NULL;
	m_bDrag = FALSE;
	m_apSkill = NULL;
	m_cOldJob = JOB_VAGRANT;
	m_cActiveSlot = 0;
	m_cActiveJobType = JTYPE_BASE;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
		m_atexSkill[i] = NULL;

	m_atexTopDown[0] = NULL;
	m_atexTopDown[1] = NULL;

	m_texSelectSkill = NULL;

	m_aTexSkillTrees[0] = NULL;
	m_aTexSkillTrees[1] = NULL;
	m_aTexSkillTrees[2] = NULL;
	m_aTexSkillTrees[3] = NULL;

	m_nCurrSkillPoint = 0;
}

CWndSkill_16::~CWndSkill_16()
{
	DeleteDeviceObjects();
}

void CWndSkill_16::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	if (ar.IsLoading())
	{
		ar >> m_cActiveSlot;
		SetSlot(m_cActiveSlot);
	}
	else
	{
		ar << m_cActiveSlot;
	}
}

BOOL CWndSkill_16::IsDownPoint(DWORD skill)
{
	LPSKILL pSkill = GetdwSkill(skill);
	if (pSkill == NULL)
		return FALSE;

	LPSKILL pSkillMy = g_pPlayer->GetSkill(pSkill->dwSkill);
	if (pSkillMy == NULL)
		return FALSE;

	if (pSkill->dwLevel <= pSkillMy->dwLevel)
		return FALSE;

	return TRUE;
}

void CWndSkill_16::SubSkillPointDown(LPSKILL lpSkill)
{
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_apSkill[i].dwSkill == NULL_ID)
			continue;

		LPSKILL pSkill2 = &m_apSkill[i];

		if (pSkill2 == NULL)
			continue;

		if (pSkill2->GetProp()->dwReSkill1 == lpSkill->dwSkill)
		{
			if (pSkill2->GetProp()->dwReSkillLevel1)
			{
				if (lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel1)
				{
					if (pSkill2->dwLevel > 0)
					{
						m_nCurrSkillPoint += (prj.GetSkillPoint(pSkill2->GetProp()) * pSkill2->dwLevel);
						pSkill2->dwLevel = 0;
						SubSkillPointDown(pSkill2);
					}
				}
			}
		}

		if (pSkill2->GetProp()->dwReSkill2 == lpSkill->dwSkill)
		{
			if (pSkill2->GetProp()->dwReSkillLevel2)
			{
				if (lpSkill->dwLevel < pSkill2->GetProp()->dwReSkillLevel2)
				{
					if (pSkill2->dwLevel > 0)
					{
						m_nCurrSkillPoint += (prj.GetSkillPoint(pSkill2->GetProp()) * pSkill2->dwLevel);
						pSkill2->dwLevel = 0;
						SubSkillPointDown(pSkill2);
					}
				}
			}
		}
	}
}

LPSKILL CWndSkill_16::GetdwSkill(DWORD dwSkill)
{
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		if (m_apSkill[i].dwSkill == dwSkill)
			return &m_apSkill[i];
	}
	return NULL;
}

LPSKILL CWndSkill_16::GetSkill(int i)
{
	LPSKILL pSkill = &m_apSkill[i];

	if (pSkill->dwSkill == NULL_ID)
		return NULL;
	if (pSkill->GetProp() == NULL)
	{
		Error("GetSkill Error Not Prop : %d", i);
		return NULL;
	}

	DWORD dwReqDislLV = pSkill->GetProp()->dwReqDisLV;
	return pSkill;
}

void CWndSkill_16::OnSize(UINT nType, int cx, int cy)
{
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 50;
	CSize size = CSize(70, 25);

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	CRect rect = GetClientRect();
	rect.DeflateRect(4, 4);

	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndSkill_16::CheckSkill(int i)
{
	LPSKILL pSkill = &m_apSkill[i];

	if (pSkill->dwSkill == NULL_ID || g_pPlayer == NULL)
		return FALSE;

	DWORD dwSkill = pSkill->dwSkill;

	ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);

	if (pSkillProp == NULL || pSkillProp->nLog == 1)
		return FALSE;

	if (g_pPlayer->GetLevel() < (int)(pSkillProp->dwReqDisLV) && !g_pPlayer->IsMaster() && !g_pPlayer->IsHero() && !g_pPlayer->IsLegendHero())
		return FALSE;

	if (pSkillProp->dwReSkill1 != 0xffffffff)
	{
		LPSKILL pSkillBuf = GetdwSkill(pSkillProp->dwReSkill1);
		if (pSkillBuf)
		{
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel1)
			{
				return FALSE;
			}
		}
	}

	if (pSkillProp->dwReSkill2 != 0xffffffff)
	{
		LPSKILL pSkillBuf = GetdwSkill(pSkillProp->dwReSkill2);
		if (pSkillBuf)
		{
			if (pSkillBuf->dwLevel < pSkillProp->dwReSkillLevel2)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

HRESULT CWndSkill_16::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}

HRESULT CWndSkill_16::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}

HRESULT CWndSkill_16::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	if (g_pPlayer && g_pPlayer->m_nSkillPoint > 0)
	{
		SAFE_DELETE_ARRAY(m_apSkill);
	}
	else
	{
		m_apSkill = NULL;
	}

	return S_OK;
}

void CWndSkill_16::InitItem(int nJob, LPSKILL apSkill, BOOL bReset)
{
	if (g_pPlayer == NULL)
		return;

	if (g_pPlayer->m_nSkillPoint > 0)
	{
		m_nCurrSkillPoint = g_pPlayer->m_nSkillPoint;
		m_apSkill = new SKILL[MAX_SKILL_JOB];
		memcpy(m_apSkill, apSkill, sizeof(SKILL) * MAX_SKILL_JOB);
	}
	else
	{
		m_nCurrSkillPoint = 0;
		m_apSkill = apSkill;
	}

	m_pFocusItem = NULL;
	if (m_apSkill == NULL)
		return;

	CString strTex[4];
	CWndStatic* pWndStaticVagrant = (CWndStatic*)GetDlgItem(WIDC_STATIC_C1);
	CWndStatic* pWndStatic1stJob = (CWndStatic*)GetDlgItem(WIDC_STATIC_C2);
	CWndStatic* pWndStatic2ndJob = (CWndStatic*)GetDlgItem(WIDC_STATIC_C3);
	CWndStatic* pWndStatic3rdJob = (CWndStatic*)GetDlgItem(WIDC_STATIC_C4);

	CWndButton* pWndBtnVagrant = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);
	CWndButton* pWndBtn1stJob = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2);
	CWndButton* pWndBtn2ndJob = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3);
	CWndButton* pWndBtn3rdJob = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4);

	CWndStatic* pWndStaticMaster = (CWndStatic*)GetDlgItem(WIDC_STATIC_MASTER);
	CWndStatic* pWndStaticHero = (CWndStatic*)GetDlgItem(WIDC_STATIC_HERO);

	LPWNDCTRL wndCtrlMaster = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL wndCtrlHero = GetWndCtrl(WIDC_CUSTOM3);

	CString firstJob = prj.GetText(TID_MAIL_OTHERWORK);
	CString secondJob = prj.GetText(TID_MAIL_OTHERWORK);
	CString thirdJob = prj.GetText(TID_MAIL_OTHERWORK);

	strTex[0] = "ImgSkillVagrant.tga";
	pWndStaticVagrant->SetTitle(prj.m_aJob[JOB_VAGRANT].szName);
	pWndBtnVagrant->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);

	if (nJob >= MAX_JOBBASE)
	{
		pWndBtn1stJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn1stJob->EnableWindow(FALSE);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		pWndStatic1stJob->SetTitle(firstJob);
	}

	//has 2nd job?
	if (nJob >= MAX_EXPERT)
	{
		pWndBtn2ndJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn2ndJob->EnableWindow(FALSE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		pWndStatic2ndJob->SetTitle(secondJob);
	}
	// master
	if (nJob >= MAX_PROFESSIONAL)
	{
		pWndStaticMaster->SetVisible(TRUE);
	}
	else
	{
		pWndStaticMaster->SetVisible(FALSE);
	}
	//hero
	if (nJob >= MAX_MASTER)
	{
		pWndStaticHero->SetVisible(TRUE);

		if (nJob == JOB_KNIGHT_HERO || nJob == JOB_LORDTEMPLER_HERO)
			m_strHeroSkillIcon = "Back_Hero_KntDrawing.tga";
		else if (nJob == JOB_BLADE_HERO || nJob == JOB_STORMBLADE_HERO)
			m_strHeroSkillIcon = "Back_Hero_BldDefence.tga";
		else if (nJob == JOB_JESTER_HERO || nJob == JOB_WINDLURKER_HERO)
			m_strHeroSkillIcon = "Back_Hero_JstSilence.tga";
		else if (nJob == JOB_RANGER_HERO || nJob == JOB_CRACKSHOOTER_HERO)
			m_strHeroSkillIcon = "Back_Hero_RagHawkeye.tga";
		else if (nJob == JOB_RINGMASTER_HERO || nJob == JOB_FLORIST_HERO)
			m_strHeroSkillIcon = "Back_Hero_RigReturn.tga";
		else if (nJob == JOB_BILLPOSTER_HERO || nJob == JOB_FORCEMASTER_HERO)
			m_strHeroSkillIcon = "Back_Hero_BilDisenchant.tga";
		else if (nJob == JOB_PSYCHIKEEPER_HERO || nJob == JOB_MENTALIST_HERO)
			m_strHeroSkillIcon = "Back_Hero_PsyStone.tga";
		else if (nJob == JOB_ELEMENTOR_HERO || nJob == JOB_ELEMENTORLORD_HERO)
			m_strHeroSkillIcon = "Back_Hero_EleCursemind.tga";
	}
	else
	{
		pWndStaticHero->SetVisible(FALSE);
	}

	if (nJob >= MAX_HERO)
	{
		pWndBtn3rdJob->EnableWindow(TRUE);
	}
	else
	{
		pWndBtn3rdJob->EnableWindow(FALSE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		pWndStatic3rdJob->SetTitle(thirdJob);
	}

	switch (nJob)
	{
	case JOB_KNIGHT_HERO:
	case JOB_KNIGHT_MASTER:
	case JOB_KNIGHT:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Night.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_KNIGHT].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMer.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotKnight.bmp"), TRUE);
		break;

	case JOB_LORDTEMPLER_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Night.TGA";
		strTex[3] = "SkillTreeLord.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_KNIGHT].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_LORDTEMPLER_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMer.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotKnight.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotLord.bmp"), TRUE);
		break;

	case JOB_BLADE:
	case JOB_BLADE_MASTER:
	case JOB_BLADE_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Blade.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_BLADE].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMer.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBlad.bmp"), TRUE);
		break;

	case JOB_STORMBLADE_HERO:
		strTex[1] = "Back_Me.TGA";
		strTex[2] = "Back_Blade.TGA";
		strTex[3] = "SkillTreeStormb.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_BLADE].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_STORMBLADE_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMer.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBlad.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotStormb.bmp"), TRUE);
		break;

	case JOB_MERCENARY:
		strTex[1] = "Back_Me.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MERCENARY].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMer.bmp"), TRUE);
		break;
	case JOB_BILLPOSTER:
	case JOB_BILLPOSTER_MASTER:
	case JOB_BILLPOSTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Bill.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_BILLPOSTER].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotAs.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBillfor.bmp"), TRUE);
		break;

	case JOB_FORCEMASTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Bill.TGA";
		strTex[3] = "SkillTreeForcm.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_BILLPOSTER].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_FORCEMASTER_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotAs.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBillfor.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotForcem.bmp"), TRUE);
		break;

	case JOB_RINGMASTER:
	case JOB_RINGMASTER_MASTER:
	case JOB_RINGMASTER_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Ring.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_RINGMASTER].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotAs.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRingm.bmp"), TRUE);
		break;

	case JOB_FLORIST_HERO:
		strTex[1] = "Back_As.TGA";
		strTex[2] = "Back_Ring.TGA";
		strTex[3] = "SkillTreeFlor.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_RINGMASTER].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_FLORIST_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotAs.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRingm.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotFlor.bmp"), TRUE);
		break;

	case JOB_ASSIST:
		strTex[1] = "Back_As.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ASSIST].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotAs.bmp"), TRUE);
		break;
	case JOB_ELEMENTOR:
	case JOB_ELEMENTOR_MASTER:
	case JOB_ELEMENTOR_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Ele.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_ELEMENTOR].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMag.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElem.bmp"), TRUE);
		break;

	case JOB_ELEMENTORLORD_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Ele.TGA";
		strTex[3] = "SkillTreeElel.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_ELEMENTOR].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_ELEMENTORLORD_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMag.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElem.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElel.bmp"), TRUE);
		break;

	case JOB_PSYCHIKEEPER:
	case JOB_PSYCHIKEEPER_MASTER:
	case JOB_PSYCHIKEEPER_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Psy.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_PSYCHIKEEPER].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMag.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotPsy.bmp"), TRUE);
		break;

	case JOB_MENTALIST_HERO:
		strTex[1] = "Back_Ma.TGA";
		strTex[2] = "Back_Psy.TGA";
		strTex[3] = "SkillTreeMent.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_PSYCHIKEEPER].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_MENTALIST_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMag.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotPsy.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMent.bmp"), TRUE);
		break;

	case JOB_MAGICIAN:
		strTex[1] = "Back_Ma.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_MAGICIAN].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMag.bmp"), TRUE);
		break;
	case JOB_ACROBAT:
		strTex[1] = "Back_Acr.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotArc.bmp"), TRUE);
		break;
	case JOB_JESTER:
	case JOB_JESTER_MASTER:
	case JOB_JESTER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Jst.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_JESTER].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotArc.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotJes.bmp"), TRUE);
		break;
	case JOB_WINDLURKER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Jst.TGA";
		strTex[3] = "SkillTreeWindI.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_JESTER].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_WINDLURKER_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotArc.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotJes.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotWindI.bmp"), TRUE);
		break;

	case JOB_RANGER:
	case JOB_RANGER_MASTER:
	case JOB_RANGER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Rag.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_RANGER].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotArc.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRan.bmp"), TRUE);
		break;
	case JOB_CRACKSHOOTER_HERO:
		strTex[1] = "Back_Acr.TGA";
		strTex[2] = "Back_Rag.TGA";
		strTex[3] = "SkillTreeCracks.TGA";
		pWndStatic1stJob->SetTitle(prj.m_aJob[JOB_ACROBAT].szName);
		pWndStatic2ndJob->SetTitle(prj.m_aJob[JOB_RANGER].szName);
		pWndStatic3rdJob->SetTitle(prj.m_aJob[JOB_CRACKSHOOTER_HERO].szName);
		pWndBtn1stJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotArc.bmp"), TRUE);
		pWndBtn2ndJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRan.bmp"), TRUE);
		pWndBtn3rdJob->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotCracks.bmp"), TRUE);
		break;
	}

	if (!strTex[0].IsEmpty()) // vagrant bg
	{
		m_aTexSkillTrees[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, strTex[0]), 0xffff00ff);
		if (!m_aTexSkillTrees[0])
			Error("CWndSkillTreeEx::InitItem failed to load %s.", strTex[0]);
	}

	if (!strTex[1].IsEmpty()) // 1st job bg
	{
		m_aTexSkillTrees[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, strTex[1]), 0xffff00ff);
		if (!m_aTexSkillTrees[1])
			Error("CWndSkillTreeEx::InitItem failed to load %s.", strTex[1]);
	}

	if (!strTex[2].IsEmpty()) // 2nd job bg
	{
		m_aTexSkillTrees[2] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, strTex[2]), 0xffff00ff);
		if (!m_aTexSkillTrees[2])
			Error("CWndSkillTreeEx::InitItem failed to load %s.", strTex[2]);
	}

	if (!strTex[3].IsEmpty()) // 3rd job bg
	{
		m_aTexSkillTrees[3] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, strTex[3]), 0xffff00ff);
		if (!m_aTexSkillTrees[3])
			Error("CWndSkillTreeEx::InitItem failed to load %s.", strTex[3]);
	}

	AdjustWndBase();
	LoadTextureSkillicon();
	g_nSkillCurSelect = -1;
	m_dwMouseSkill = NULL_ID;
	if (m_cOldJob != nJob)
	{
		SetSlot(0);
		m_cOldJob = nJob;
	}

	if (g_nSkillCurSelect >= 0)
		m_pFocusItem = &m_apSkill[g_nSkillCurSelect];
}

BOOL CWndSkill_16::GetSkillPoint(DWORD dwSkillID, CRect& rect)
{
	ItemProp* pPropSkill = prj.GetSkillProp(dwSkillID);

	if (pPropSkill)
	{
		int nRectX, nRectY;
		nRectX = nRectY = 0;

		switch (dwSkillID)
		{
		case SI_VAG_ONE_CLEANHIT:				nRectX = 50, nRectY = 55; break;
		case SI_VAG_ONE_BRANDISH:				nRectX = 100, nRectY = 55; break;
		case SI_VAG_ONE_OVERCUTTER:				nRectX = 150, nRectY = 55; break;
		case SI_MER_ONE_SPLMASH:				nRectX = 25, nRectY = 16; break;
		case SI_MER_SHIELD_PROTECTION:			nRectX = 25, nRectY = 68; break;
		case SI_MER_ONE_KEENWHEEL:				nRectX = 75, nRectY = 3; break;
		case SI_MER_ONE_BLOODYSTRIKE:			nRectX = 75, nRectY = 29; break;
		case SI_MER_SHIELD_PANBARRIER:			nRectX = 75, nRectY = 55; break;
		case SI_MER_SUP_IMPOWERWEAPON:			nRectX = 75, nRectY = 81; break;
		case SI_MER_ONE_BLINDSIDE:				nRectX = 125, nRectY = 3; break;
		case SI_MER_ONE_REFLEXHIT:				nRectX = 125, nRectY = 29; break;
		case SI_MER_ONE_SNEAKER:				nRectX = 125, nRectY = 55; break;
		case SI_MER_SUP_SMITEAXE:				nRectX = 125, nRectY = 81; break;
		case SI_MER_SUP_BLAZINGSWORD:			nRectX = 125, nRectY = 107; break;
		case SI_MER_ONE_SPECIALHIT:				nRectX = 175, nRectY = 3; break;
		case SI_MER_ONE_GUILOTINE:				nRectX = 175, nRectY = 29; break;
		case SI_MER_SUP_AXEMASTER:				nRectX = 175, nRectY = 81; break;
		case SI_MER_SUP_SWORDMASTER:			nRectX = 175, nRectY = 107; break;
		case SI_KNT_TWOSW_CHARGE:				nRectX = 25, nRectY = 29; break;
		case SI_KNT_TWOAX_PAINDEALER:			nRectX = 25, nRectY = 55; break;
		case SI_KNT_SUP_GUARD:					nRectX = 25, nRectY = 81; break;
		case SI_KNT_TWOSW_EARTHDIVIDER:			nRectX = 75, nRectY = 29; break;
		case SI_KNT_TWOAX_POWERSTUMP:			nRectX = 75, nRectY = 55; break;
		case SI_KNT_SUP_RAGE:					nRectX = 75, nRectY = 81; break;
		case SI_KNT_TWO_POWERSWING:				nRectX = 125, nRectY = 42; break;
		case SI_KNT_SUP_PAINREFLECTION:			nRectX = 125, nRectY = 81; break;
		case SI_BLD_DOUBLESW_SILENTSTRIKE:		nRectX = 25, nRectY = 29; break;
		case SI_BLD_DOUBLEAX_SPRINGATTACK:		nRectX = 25, nRectY = 55; break;
		case SI_BLD_DOUBLE_ARMORPENETRATE:		nRectX = 25, nRectY = 81; break;
		case SI_BLD_DOUBLESW_BLADEDANCE:		nRectX = 75, nRectY = 29; break;
		case SI_BLD_DOUBLEAX_HAWKATTACK:		nRectX = 75, nRectY = 55; break;
		case SI_BLD_SUP_BERSERK:				nRectX = 75, nRectY = 81; break;
		case SI_BLD_DOUBLE_CROSSSTRIKE:			nRectX = 125, nRectY = 42; break;
		case SI_BLD_DOUBLE_SONICBLADE:			nRectX = 125, nRectY = 81; break;
		case SI_ASS_HEAL_HEALING:				nRectX = 8, nRectY = 42; break;
		case SI_ASS_HEAL_PATIENCE:				nRectX = 54, nRectY = 16; break;
		case SI_ASS_CHEER_QUICKSTEP:			nRectX = 54, nRectY = 42; break;
		case SI_ASS_CHEER_MENTALSIGN:			nRectX = 54, nRectY = 68; break;
		case SI_ASS_KNU_TAMPINGHOLE:			nRectX = 54, nRectY = 94; break;
		case SI_ASS_HEAL_RESURRECTION:			nRectX = 100, nRectY = 16; break;
		case SI_ASS_CHEER_HASTE:				nRectX = 100, nRectY = 42; break;
		case SI_ASS_CHEER_HEAPUP:				nRectX = 100, nRectY = 68; break;
		case SI_ASS_CHEER_STONEHAND:			nRectX = 100, nRectY = 94; break;
		case SI_ASS_CHEER_CIRCLEHEALING:		nRectX = 146, nRectY = 16; break;
		case SI_ASS_CHEER_CATSREFLEX:			nRectX = 146, nRectY = 42; break;
		case SI_ASS_CHEER_BEEFUP:				nRectX = 146, nRectY = 68; break;
		case SI_ASS_KNU_BURSTCRACK:				nRectX = 146, nRectY = 94; break;
		case SI_ASS_HEAL_PREVENTION:			nRectX = 192, nRectY = 16; break;
		case SI_ASS_CHEER_CANNONBALL:			nRectX = 192, nRectY = 42; break;
		case SI_ASS_CHEER_ACCURACY:				nRectX = 192, nRectY = 68; break;
		case SI_ASS_KNU_POWERFIST:				nRectX = 192, nRectY = 94; break;
		case SI_RIN_SUP_PROTECT:				nRectX = 25, nRectY = 29; break;
		case SI_RIN_SUP_HOLYCROSS:				nRectX = 25, nRectY = 55; break;
		case SI_RIN_HEAL_GVURTIALLA:			nRectX = 25, nRectY = 81; break;
		case SI_RIN_SUP_HOLYGUARD:				nRectX = 75, nRectY = 29; break;
		case SI_RIN_SUP_SPIRITUREFORTUNE:		nRectX = 75, nRectY = 55; break;
		case SI_RIN_HEAL_HEALRAIN:				nRectX = 75, nRectY = 81; break;
		case SI_RIN_SQU_GEBURAHTIPHRETH:		nRectX = 125, nRectY = 42; break;
		case SI_RIN_SUP_MERKABAHANZELRUSHA:		nRectX = 125, nRectY = 81; break;
		case SI_BIL_KNU_BELIALSMESHING:			nRectX = 25, nRectY = 29; break;
		case SI_BIL_PST_ASMODEUS:				nRectX = 25, nRectY = 55; break;
		case SI_BIL_KNU_BLOODFIST:				nRectX = 75, nRectY = 29; break;
		case SI_BIL_PST_BARAQIJALESNA:			nRectX = 75, nRectY = 55; break;
		case SI_BIL_KNU_PIERCINGSERPENT:		nRectX = 125, nRectY = 29; break;
		case SI_BIL_PST_BGVURTIALBOLD:			nRectX = 125, nRectY = 55; break;
		case SI_BIL_KNU_SONICHAND:				nRectX = 175, nRectY = 42; break;
		case SI_BIL_PST_ASALRAALAIKUM:			nRectX = 175, nRectY = 81; break;
		case SI_MAG_MAG_MENTALSTRIKE:			nRectX = 25, nRectY = 3; break;
		case SI_MAG_MAG_BLINKPOOL: 				nRectX = 25, nRectY = 55; break;
		case SI_MAG_FIRE_BOOMERANG:				nRectX = 75, nRectY = 3; break;
		case SI_MAG_WIND_SWORDWIND:				nRectX = 75, nRectY = 29; break;
		case SI_MAG_WATER_ICEMISSILE:			nRectX = 75, nRectY = 55; break;
		case SI_MAG_ELECTRICITY_LIGHTINGBALL:	nRectX = 75, nRectY = 81; break;
		case SI_MAG_EARTH_SPIKESTONE:			nRectX = 75, nRectY = 107; break;
		case SI_MAG_FIRE_HOTAIR:				nRectX = 125, nRectY = 3; break;
		case SI_MAG_WIND_STRONGWIND:			nRectX = 125, nRectY = 29; break;
		case SI_MAG_WATER_WATERBALL:			nRectX = 125, nRectY = 55; break;
		case SI_MAG_ELECTRICITY_LIGHTINGPARM:	nRectX = 125, nRectY = 81; break;
		case SI_MAG_EARTH_ROCKCRASH:			nRectX = 125, nRectY = 107; break;
		case SI_MAG_FIRE_FIRESTRIKE:			nRectX = 175, nRectY = 3; break;
		case SI_MAG_WIND_WINDCUTTER:			nRectX = 175, nRectY = 29; break;
		case SI_MAG_WATER_SPRINGWATER:			nRectX = 175, nRectY = 55; break;
		case SI_MAG_ELECTRICITY_LIGHTINGSHOCK:	nRectX = 175, nRectY = 81; break;
		case SI_MAG_EARTH_LOOTING:				nRectX = 175, nRectY = 107; break;
		case SI_ELE_FIRE_FIREBIRD:				nRectX = 25, nRectY = 3; break;
		case SI_ELE_EARTH_STONESPEAR:			nRectX = 25, nRectY = 29; break;
		case SI_ELE_WIND_VOID:					nRectX = 25, nRectY = 55; break;
		case SI_ELE_ELECTRICITY_THUNDERSTRIKE:	nRectX = 25, nRectY = 81; break;
		case SI_ELE_WATER_ICESHARK:				nRectX = 25, nRectY = 107; break;
		case SI_ELE_FIRE_BURINGFIELD:			nRectX = 75, nRectY = 3; break;
		case SI_ELE_EARTH_EARTHQUAKE:			nRectX = 75, nRectY = 29; break;
		case SI_ELE_WIND_WINDFIELD:				nRectX = 75, nRectY = 55; break;
		case SI_ELE_ELECTRICITY_ELETRICSHOCK:	nRectX = 75, nRectY = 81; break;
		case SI_ELE_WATER_POISONCLOUD:			nRectX = 75, nRectY = 107; break;
		case SI_ELE_MULTY_METEOSHOWER:			nRectX = 125, nRectY = 16; break;
		case SI_ELE_MULTY_SANDSTORM:			nRectX = 125, nRectY = 42; break;
		case SI_ELE_MULTY_LIGHTINGSTORM:		nRectX = 125, nRectY = 68; break;
		case SI_ELE_MULTY_AVALANCHE:			nRectX = 125, nRectY = 107; break;
		case SI_ELE_FIRE_FIREMASTER:			nRectX = 175, nRectY = 3; break;
		case SI_ELE_EARTH_EARTHMASTER:			nRectX = 175, nRectY = 29; break;
		case SI_ELE_WIND_WINDMASTER:			nRectX = 175, nRectY = 55; break;
		case SI_ELE_ELECTRICITY_LIGHTINGMASTER:	nRectX = 175, nRectY = 81; break;
		case SI_ELE_WATER_WATERMASTER:			nRectX = 175, nRectY = 107; break;
		case SI_PSY_NLG_DEMONOLGY:				nRectX = 25, nRectY = 29; break;
		case SI_PSY_PSY_PSYCHICBOMB:			nRectX = 25, nRectY = 55; break;
		case SI_PSY_NLG_CRUCIOSPELL:			nRectX = 25, nRectY = 81; break;
		case SI_PSY_NLG_SATANOLGY:				nRectX = 75, nRectY = 29; break;
		case SI_PSY_PSY_SPRITBOMB:				nRectX = 75, nRectY = 55; break;
		case SI_PSY_PSY_MAXIMUMCRISIS:			nRectX = 75, nRectY = 81; break;
		case SI_PSY_PSY_PSYCHICWALL:			nRectX = 125, nRectY = 42; break;
		case SI_PSY_PSY_PSYCHICSQUARE:			nRectX = 125, nRectY = 81; break;
		case SI_ACR_YOYO_PULLING:				nRectX = 25, nRectY = 4; break;
		case SI_ACR_SUP_SLOWSTEP:				nRectX = 25, nRectY = 30; break;
		case SI_ACR_BOW_JUNKBOW:				nRectX = 25, nRectY = 81; break;
		case SI_ACR_SUP_FASTWALKER:				nRectX = 75, nRectY = 4; break;
		case SI_ACR_SUP_YOYOMASTER:				nRectX = 75, nRectY = 30; break;
		case SI_ACR_SUP_BOWMASTER: 				nRectX = 75, nRectY = 81; break;
		case SI_ACR_SUP_DARKILLUSION:			nRectX = 125, nRectY = 4; break;
		case SI_ACR_YOYO_SNITCH:				nRectX = 125, nRectY = 30; break;
		case SI_ACR_YOYO_CROSSLINE:				nRectX = 125, nRectY = 56; break;
		case SI_ACR_BOW_SILENTSHOT:				nRectX = 125, nRectY = 81; break;
		case SI_ACR_BOW_AIMEDSHOT:				nRectX = 125, nRectY = 107; break;
		case SI_ACR_YOYO_ABSOLUTEBLOCK:			nRectX = 175, nRectY = 4; break;
		case SI_ACR_YOYO_DEADLYSWING:			nRectX = 175, nRectY = 30; break;
		case SI_ACR_YOYO_COUNTER:				nRectX = 175, nRectY = 56; break;
		case SI_ACR_BOW_AUTOSHOT:				nRectX = 175, nRectY = 81; break;
		case SI_ACR_BOW_ARROWRAIN:				nRectX = 175, nRectY = 107; break;
		case SI_JST_SUP_POISON:					nRectX = 25, nRectY = 29; break;
		case SI_JST_SUP_BLEEDING:				nRectX = 25, nRectY = 55; break;
		case SI_JST_YOYO_ESCAPE:				nRectX = 25, nRectY = 81; break;
		case SI_JST_YOYO_CRITICALSWING:			nRectX = 75, nRectY = 29; break;
		case SI_JST_YOYO_BACKSTAB:				nRectX = 75, nRectY = 55; break;
		case SI_JST_SUP_ABSORB:					nRectX = 75, nRectY = 81; break;
		case SI_JST_YOYO_VATALSTAB:				nRectX = 125, nRectY = 42; break;
		case SI_JST_YOYO_HITOFPENYA:			nRectX = 125, nRectY = 81; break;
		case SI_RAG_BOW_ICEARROW:				nRectX = 25, nRectY = 29; break;
		case SI_RAG_BOW_FLAMEARROW:				nRectX = 25, nRectY = 55; break;
		case SI_RAG_BOW_POISONARROW:			nRectX = 25, nRectY = 81; break;
		case SI_RAG_SUP_FASTATTACK:				nRectX = 75, nRectY = 29; break;
		case SI_RAG_BOW_PIRCINGARROW:			nRectX = 75, nRectY = 55; break;
		case SI_RAG_SUP_NATURE:					nRectX = 75, nRectY = 81; break;
		case SI_RAG_BOW_TRIPLESHOT:				nRectX = 125, nRectY = 42; break;
		case SI_RAG_BOW_SILENTARROW:			nRectX = 125, nRectY = 81; break;
		case SI_LOD_SUP_PULLING:				nRectX = 25, nRectY = 29; break;
		case SI_LOD_ONE_GRANDRAGE:				nRectX = 25, nRectY = 55; break;
		case SI_LOD_SUP_HOLYARMOR:				nRectX = 25, nRectY = 81; break;
		case SI_LOD_SUP_SCOPESTRIKE:			nRectX = 75, nRectY = 55; break;
		case SI_LOD_SUP_ANGER:					nRectX = 75, nRectY = 81; break;
		case SI_LOD_ONE_SHILDSTRIKE:			nRectX = 125, nRectY = 55; break;
		case SI_STORM_DOUBLE_CROSSBLOOD:		nRectX = 25, nRectY = 29; break;
		case SI_STORM_SUP_POWERINCREASE:		nRectX = 25, nRectY = 55; break;
		case SI_STORM_DOUBLE_STORMBLAST:		nRectX = 75, nRectY = 29; break;
		case SI_STORM_DOUBLE_HOLDINGSTORM:		nRectX = 75, nRectY = 55; break;
		case SI_WIN_YOYO_MADHURRICANE:			nRectX = 25, nRectY = 55; break;
		case SI_WIN_SUP_EVASIONINCREASE:		nRectX = 25, nRectY = 29; break;
		case SI_WIN_SUP_CONTROLINCREASE:		nRectX = 75, nRectY = 29; break;
		case SI_WIN_YOYO_BACKSTEP:				nRectX = 75, nRectY = 55; break;
		case SI_CRA_SUP_POWERINCREASE:			nRectX = 25, nRectY = 29; break;
		case SI_CRA_CRBOW_RANGESTRIKE:			nRectX = 25, nRectY = 55; break;
		case SI_CRA_SUP_HAWKEYE:				nRectX = 125, nRectY = 29; break;
		case SI_CRA_SUP_CONTROLINCREASE:		nRectX = 75, nRectY = 29; break;
		case SI_FLO_HEAL_DOT:					nRectX = 25, nRectY = 42; break;
		case SI_FLO_SQU_BLESSSTEP:				nRectX = 71, nRectY = 16; break;
		case SI_FLO_SQU_BLESSBODY:				nRectX = 71, nRectY = 42; break;
		case SI_FLO_SQU_BLESSARMOR:				nRectX = 71, nRectY = 68; break;
		case SI_FLO_SUP_ABSOLUTE:				nRectX = 117, nRectY = 94; break;
		case SI_FLO_CHEER_PATTERS:				nRectX = 71, nRectY = 94; break;
		case SI_FOR_SQU_FORCETENACITY:			nRectX = 25, nRectY = 29; break;
		case SI_FOR_SQU_FORCEANGER:				nRectX = 75, nRectY = 29; break;
		case SI_FOR_SQU_FORCESPEED:				nRectX = 25, nRectY = 55; break;
		case SI_FOR_SQU_FORCEMAD:				nRectX = 75, nRectY = 55; break;
		case SI_MEN_WAN_DARKNESSLAKE:			nRectX = 71, nRectY = 94; break;
		case SI_MEN_WAN_ATKDECREASE:			nRectX = 71, nRectY = 16; break;
		case SI_MEN_WAN_FEARSCREAM:				nRectX = 25, nRectY = 42; break;
		case SI_MEN_WAN_DEFDECREASE:			nRectX = 71, nRectY = 42; break;
		case SI_MEN_WAN_SPEDECREASE:			nRectX = 71, nRectY = 68; break;
		case SI_ELE_STF_FINALSPEAR:				nRectX = 25, nRectY = 29; break;
		case SI_ELE_STF_COSMICELEMENT:			nRectX = 25, nRectY = 55; break;
		case SI_ELE_STF_SLIPPING:				nRectX = 75, nRectY = 81; break;
		case SI_ELE_STF_THUNDERBOLT:			nRectX = 75, nRectY = 42; break;
		default:
			if (pPropSkill->dwItemKind1 == JTYPE_MASTER)
			{
				rect = CRect(0, 0, 24, 24);
				LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
				if (lpWndCtrl)
					rect += lpWndCtrl->rect.TopLeft();

				return TRUE;
			}
			else
				if (pPropSkill->dwItemKind1 == JTYPE_HERO)
				{
					rect = CRect(0, 0, 24, 24);
					LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM3);
					if (lpWndCtrl)
						rect += lpWndCtrl->rect.TopLeft();

					return TRUE;
				}
				else
					return FALSE;
		}

		nRectY += 4;
		rect = CRect(nRectX, nRectY, nRectX + 24, nRectY + 24);

		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC_PANNEL);
		if (lpWndCtrl)
		{
			rect += lpWndCtrl->rect.TopLeft();
		}
	}
	return TRUE;
}

void CWndSkill_16::LoadTextureSkillicon()
{
	if (m_apSkill == NULL)
		return;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill;
		lpSkill = &m_apSkill[i];
		if (lpSkill->dwSkill != NULL_ID)
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt(lpSkill->dwSkill);
			if (pSkillProp)
				m_atexSkill[i] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
		}
	}
	m_textPackNum.LoadScript(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("icon_IconSkillLevel.inc")));
}

void CWndSkill_16::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_apSkill == NULL)
		return;

	if (m_bDrag == FALSE)
		return;

	CPoint pt(3, 3);
	CRect rect;
	if (CheckSkill(g_nSkillCurSelect))
	{
		m_bDrag = FALSE;
		DWORD dwSkill = m_apSkill[g_nSkillCurSelect].dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);

		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_SKILL;
		m_GlobalShortcut.m_dwType = 0;
		m_GlobalShortcut.m_dwIndex = dwSkill;
		m_GlobalShortcut.m_dwData = 0;
		m_GlobalShortcut.m_dwId = g_nSkillCurSelect;
		m_GlobalShortcut.m_pTexture = m_atexSkill[g_nSkillCurSelect];
		_tcscpy(m_GlobalShortcut.m_szString, pSkillProp->szName);
	}
}

BOOL CWndSkill_16::Process()
{
	if (m_apSkill == NULL || g_pPlayer == NULL)
		return FALSE;
	CWndButton* pWndBtn;
	switch (m_cActiveSlot)
	{
	case 0: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1); break;
	case 1: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2); break;
	case 2: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3); break;
	case 3: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4); break;
	}

	if (pWndBtn)
		pWndBtn->m_bPush = true;

	m_pWndButton[0]->EnableWindow(FALSE);
	m_pWndButton[1]->EnableWindow(FALSE);
	m_pWndButton[2]->EnableWindow(FALSE);
	m_pWndButton[3]->EnableWindow(FALSE);
	m_pWndButton[4]->EnableWindow(FALSE);

	if (m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint)
	{
		LPSKILL lpSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if (pSkillProp == NULL || lpSkillUser == NULL)
			return TRUE;
		if (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)
		{
			CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem(WIDC_STATIC_NEEDSP);
			lpWndStaticNeedSP->SetTitle("0");
			return TRUE;
		}
		int nPoint = prj.GetSkillPoint(pSkillProp);
		if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax && nPoint <= m_nCurrSkillPoint)
		{
			m_pWndButton[0]->EnableWindow(TRUE);
			m_pWndButton[4]->EnableWindow(TRUE);
		}
		if (m_pFocusItem->dwLevel != lpSkillUser->dwLevel)
			m_pWndButton[1]->EnableWindow(TRUE);

		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
		{
			m_pWndButton[2]->EnableWindow(TRUE);
			m_pWndButton[3]->EnableWindow(TRUE);
		}
	}

	if (m_pFocusItem)
	{
		CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem(WIDC_STATIC_NEEDSP);
		ItemProp* pSkillProp = m_pFocusItem->GetProp();
		if (pSkillProp)
		{
			int nPoint = prj.GetSkillPoint(pSkillProp);
			CString strSPN;
			strSPN.Format("%d", nPoint);
			lpWndStaticNeedSP->SetTitle(strSPN);
		}
	}
	else
	{
		CWndStatic* lpWndStaticNeedSP = (CWndStatic*)GetDlgItem(WIDC_STATIC_NEEDSP);
		lpWndStaticNeedSP->SetTitle("0");
	}

	return TRUE;
}

void CWndSkill_16::OnMouseWndSurface(CPoint point)
{
	DWORD dwMouseSkill = NULL_ID;
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;

		DWORD dwSkill = pSkill->dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
		if (dwSkill != NULL_ID && (pSkillProp->dwItemKind1 == m_cActiveJobType || (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)))
		{
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect))
			{
				rect.top -= 2;
				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;

				if (rect.PtInRect(point))
				{
					dwMouseSkill = dwSkill;

					ClientToScreen(&point);
					ClientToScreen(&rect);
					g_WndMng.PutToolTip_Skill(dwSkill, pSkill->dwLevel, point, &rect, CheckSkill(i));
					break;
				}
			}
		}
	}
	m_dwMouseSkill = dwMouseSkill;
}

void CWndSkill_16::OnDraw(C2DRender* p2DRender)
{
	m_aTexSkillTrees[m_cActiveSlot]->Render(p2DRender, m_pointTreePos);

	if (m_apSkill == NULL)
		return;

	if (!m_strHeroSkillIcon.IsEmpty())
	{
		CTexture* pTexture;
		CRect rect;
		CPoint point;
		LPWNDCTRL lpWndCtrl;

		lpWndCtrl = GetWndCtrl(WIDC_CUSTOM3);
		rect = lpWndCtrl->rect;
		point = rect.TopLeft();
		point.y -= 2;
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, m_strHeroSkillIcon), 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, point, CPoint(27, 27));
	}

	m_nAlphaCount = m_nAlpha;
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;

		DWORD dwSkill = pSkill->dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
		if (pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID)
		{
			if (m_atexSkill[i] && CheckSkill(i) && 0 < pSkill->dwLevel && (pSkillProp->dwItemKind1 == m_cActiveJobType || (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)))
			{
				CRect rect;
				if (GetSkillPoint(pSkillProp->dwID, rect))
				{
					rect.top -= 2;
					m_atexSkill[i]->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
					int nAddNum = 0;
					LPSKILL pSkillUser = g_pPlayer->GetSkill(pSkill->dwSkill);
					if (pSkillUser && pSkill->dwLevel != pSkillUser->dwLevel)
						nAddNum = 20;
					if (pSkill->dwLevel < pSkillProp->dwExpertMax)
						m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), pSkill->dwLevel - 1 + nAddNum);
					else
						m_textPackNum.Render(p2DRender, rect.TopLeft() - CPoint(2, 2), 19 + nAddNum);
				}
			}
			else if (m_dwMouseSkill == dwSkill && m_atexSkill[i] && CheckSkill(i))
			{
				CRect rect;
				if (GetSkillPoint(pSkillProp->dwID, rect))
				{
					rect.top -= 2;
					m_atexSkill[i]->Render(p2DRender, rect.TopLeft(), CPoint(27, 27));
				}
			}
			if (m_pFocusItem && m_pFocusItem->dwSkill == dwSkill)
			{
				CRect rect;
				if (GetSkillPoint(pSkillProp->dwID, rect))
				{
					rect.top -= 3;
					rect.left -= 3;
					if (pSkillProp->dwItemKind1 == m_cActiveJobType || (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO))
						m_texSelectSkill->Render(p2DRender, rect.TopLeft(), CPoint(27, 27), 255UL, 1.1f, 1.1f);
				}
			}
		}
	}

	CWndStatic* lpWndStatic9 = (CWndStatic*)GetDlgItem(WIDC_STATIC_SP);
	CString strSP;
	strSP.Format("%d", m_nCurrSkillPoint);
	lpWndStatic9->SetTitle(strSP);

	if (m_pFocusItem && g_nSkillCurSelect > -1)
	{
		if (m_pFocusItem->dwSkill == -1)
		{
			CString str;
			str.Format("Impossible Skill!!! Player Job %d, Focused Skill %d , Level %d, CurSelect %d", g_pPlayer->GetJob(), m_pFocusItem->dwSkill, m_pFocusItem->dwLevel, g_nSkillCurSelect);
			//(str);
		}

		lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
		m_atexSkill[g_nSkillCurSelect]->Render(p2DRender, lpWndCtrl->rect.TopLeft(), CPoint(30, 30), 255, 1.2f, 1.2f);


		ItemProp* pSkillProp = prj.GetSkillProp(m_pFocusItem->dwSkill);

		if (pSkillProp && 0 < m_pFocusItem->dwLevel)
		{
			int nAddNum = 0;
			LPSKILL pSkillUser = g_pPlayer->GetSkill(m_pFocusItem->dwSkill);
			if (pSkillUser && m_pFocusItem->dwLevel != pSkillUser->dwLevel)
				nAddNum = 20;

			CPoint pt = lpWndCtrl->rect.TopLeft();
			pt.x += 5;
			pt.y += 5;

			if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax)
				m_textPackNum.Render(p2DRender, pt, m_pFocusItem->dwLevel - 1 + nAddNum);
			else
				m_textPackNum.Render(p2DRender, pt, 19 + nAddNum);
		}
	}
}
void CWndSkill_16::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();

	InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill);

	m_atexTopDown[0] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "LvUp.bmp"), 0xffff00ff);
	m_atexTopDown[1] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "LvDn.bmp"), 0xffff00ff);
	m_texSelectSkill = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndPosMark.tga"), 0xffff00ff);

	if (g_nSkillCurSelect >= 0)
		m_pFocusItem = &m_apSkill[g_nSkillCurSelect];

	m_pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON_PLUS);
	m_pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON_MINUS);
	m_pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_BUTTON_BACK);
	m_pWndButton[3] = (CWndButton*)GetDlgItem(WIDC_BUTTON_OK);
	m_pWndButton[4] = (CWndButton*)GetDlgItem(WIDC_BTN_NUM1);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int x = rectRoot.right - rect.Width();
	int y = rectRoot.bottom - rect.Height();

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC_PANNEL);
	if (lpWndCtrl)
	{
		m_pointTreePos = lpWndCtrl->rect.TopLeft();
		m_pointTreePos.y += 4;
	}

	CPoint point(x, y);
	Move(point);
}

BOOL CWndSkill_16::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(792, 130), pWndParent);
}

BOOL CWndSkill_16::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (m_pFocusItem && 0 < g_pPlayer->m_nSkillPoint)
	{
		ItemProp* pSkillProp = prj.GetSkillProp(m_pFocusItem->dwSkill);
		if (pSkillProp)
		{
			int nPoint = prj.GetSkillPoint(pSkillProp);
			switch (nID)
			{
			case WIDC_BUTTON_PLUS:	// + skill level
			{
				if (nPoint <= m_nCurrSkillPoint)
				{
					if (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax)
					{
						m_nCurrSkillPoint -= nPoint;
						++m_pFocusItem->dwLevel;
					}
				}
			}
			break;
			case WIDC_BTN_NUM1:	// + skill level
			{
				while (m_pFocusItem->dwLevel < pSkillProp->dwExpertMax)
				{
					if (nPoint > m_nCurrSkillPoint)
						break;

					m_nCurrSkillPoint -= nPoint;
					++m_pFocusItem->dwLevel;
				}
			}
			break;
			case WIDC_BUTTON_MINUS:
			{
				if (IsDownPoint(m_pFocusItem->dwSkill))
				{
					m_nCurrSkillPoint += nPoint;
					--m_pFocusItem->dwLevel;
					SubSkillPointDown(m_pFocusItem);
				}
			}
			break;
			case WIDC_BUTTON_BACK:
			{
				if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
					InitItem(g_pPlayer->GetJob(), g_pPlayer->m_aJobSkill, TRUE);
				m_pFocusItem = NULL;
			}
			break;
			case WIDC_BUTTON_OK:
			{
				if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
				{
					SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
					g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
					g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				}
			}
			break;
			}
		}
	}

	switch (nID)
	{
	case WIDC_BUTTON_C1:SetSlot(0); break;
	case WIDC_BUTTON_C2:SetSlot(1); break;
	case WIDC_BUTTON_C3:SetSlot(2); break;
	case WIDC_BUTTON_C4:SetSlot(3); break;
	}

	if (nID == 10000)
	{
		if (m_nCurrSkillPoint != g_pPlayer->m_nSkillPoint)
		{
			CWndBase* pWndBase = (CWndBase*)g_WndMng.GetWndBase(APP_QUEITMWARNING);
			if (pWndBase == NULL)
			{
				SAFE_DELETE(g_WndMng.m_pWndReSkillWarning);
				g_WndMng.m_pWndReSkillWarning = new CWndReSkillWarning;
				g_WndMng.m_pWndReSkillWarning->Initialize(NULL);
				g_WndMng.m_pWndReSkillWarning->InitItem(TRUE);
			}
			return TRUE;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndSkill_16::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndSkill_16::SetSlot(char cSlot)
{
	if (cSlot > 3)
	{
		cSlot = 0;
	}
	CWndButton* pWndBtn;
	switch (m_cActiveSlot)
	{
	case 0: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1); break;
	case 1: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2); break;
	case 2: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3); break;
	case 3: pWndBtn = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4); break;
	}

	if (pWndBtn)
		pWndBtn->m_bPush = false;

	m_cActiveSlot = cSlot;
	char cSelJob = JOB_VAGRANT;
	switch (cSlot)
	{
	case 0:
		m_cActiveJobType = JTYPE_BASE;
		break;
	case 1:
		m_cActiveJobType = JTYPE_EXPERT;
		switch (g_pPlayer->GetJob())
		{
		case JOB_MERCENARY:
		case JOB_KNIGHT:
		case JOB_KNIGHT_MASTER:
		case JOB_KNIGHT_HERO:
		case JOB_BLADE:
		case JOB_BLADE_MASTER:
		case JOB_BLADE_HERO:
		case JOB_LORDTEMPLER_HERO:
		case JOB_STORMBLADE_HERO:
			cSelJob = JOB_MERCENARY;
			break;
		case JOB_ASSIST:
		case JOB_RINGMASTER:
		case JOB_RINGMASTER_MASTER:
		case JOB_RINGMASTER_HERO:
		case JOB_BILLPOSTER:
		case JOB_BILLPOSTER_MASTER:
		case JOB_BILLPOSTER_HERO:
		case JOB_FLORIST_HERO:
		case JOB_FORCEMASTER_HERO:
			cSelJob = JOB_ASSIST;
			break;
		case JOB_MAGICIAN:
		case JOB_ELEMENTOR:
		case JOB_ELEMENTOR_MASTER:
		case JOB_ELEMENTOR_HERO:
		case JOB_PSYCHIKEEPER:
		case JOB_PSYCHIKEEPER_MASTER:
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:
		case JOB_ELEMENTORLORD_HERO:
			cSelJob = JOB_MAGICIAN;
			break;
		case JOB_ACROBAT:
		case JOB_RANGER:
		case JOB_RANGER_MASTER:
		case JOB_RANGER_HERO:
		case JOB_JESTER:
		case JOB_JESTER_MASTER:
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			cSelJob = JOB_ACROBAT;
			break;
		}
		break;
	case 2:
		m_cActiveJobType = JTYPE_PRO;
		switch (g_pPlayer->GetJob())
		{
		case JOB_KNIGHT:
		case JOB_KNIGHT_MASTER:
		case JOB_KNIGHT_HERO:
		case JOB_LORDTEMPLER_HERO:
			cSelJob = JOB_KNIGHT;
			break;
		case JOB_BLADE:
		case JOB_BLADE_MASTER:
		case JOB_BLADE_HERO:
		case JOB_STORMBLADE_HERO:
			cSelJob = JOB_BLADE;
			break;
		case JOB_RINGMASTER:
		case JOB_RINGMASTER_MASTER:
		case JOB_RINGMASTER_HERO:
		case JOB_FLORIST_HERO:
			cSelJob = JOB_RINGMASTER;
			break;
		case JOB_BILLPOSTER:
		case JOB_BILLPOSTER_MASTER:
		case JOB_BILLPOSTER_HERO:
		case JOB_FORCEMASTER_HERO:
			cSelJob = JOB_BILLPOSTER;
			break;
		case JOB_ELEMENTOR:
		case JOB_ELEMENTOR_MASTER:
		case JOB_ELEMENTOR_HERO:
		case JOB_ELEMENTORLORD_HERO:
			cSelJob = JOB_ELEMENTOR;
			break;
		case JOB_PSYCHIKEEPER:
		case JOB_PSYCHIKEEPER_MASTER:
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:
			cSelJob = JOB_PSYCHIKEEPER;
			break;
		case JOB_RANGER:
		case JOB_RANGER_MASTER:
		case JOB_RANGER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			cSelJob = JOB_RANGER;
			break;
		case JOB_JESTER:
		case JOB_JESTER_MASTER:
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:
			cSelJob = JOB_JESTER;
			break;
		}
		break;
	case 3:
		m_cActiveJobType = JTYPE_LEGEND_HERO;
		cSelJob = g_pPlayer->GetJob();

		break;
	}

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC_CLASSNAME);
	if (pStatic)
		pStatic->SetTitle(prj.m_aJob[cSelJob].szName);
}

void CWndSkill_16::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrag = FALSE;
}

void CWndSkill_16::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_apSkill == NULL)
		return;

	CPoint ptMouse = GetMousePoint();
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;

		DWORD dwSkill = pSkill->dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
		if (pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID && (pSkillProp->dwItemKind1 == m_cActiveJobType || (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)))
		{
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && CheckSkill(i))
			{
				rect.top -= 2;
				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;
				if (rect.PtInRect(ptMouse))
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[i];
					m_bDrag = TRUE;
					break;
				}
			}
		}
	}
}

void CWndSkill_16::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_apSkill == NULL)
		return;

	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL pSkill = GetSkill(i);
		if (pSkill == NULL)
			continue;

		DWORD dwSkill = pSkill->dwSkill;
		ItemProp* pSkillProp = prj.GetSkillProp(dwSkill);
		if (pSkillProp && pSkillProp->nLog != 1 && dwSkill != NULL_ID && (pSkillProp->dwItemKind1 == m_cActiveJobType || (pSkillProp->dwItemKind1 == JTYPE_MASTER || pSkillProp->dwItemKind1 == JTYPE_HERO)))
		{
			CRect rect;
			if (GetSkillPoint(pSkillProp->dwID, rect) && CheckSkill(i))
			{
				rect.top -= 2;
				rect.right = rect.left + 27;
				rect.bottom = rect.top + 27;
				CPoint ptMouse = GetMousePoint();
				if (rect.PtInRect(ptMouse))
				{
					g_nSkillCurSelect = i;
					m_pFocusItem = &m_apSkill[i];

					if (g_pPlayer->CheckSkill(m_pFocusItem->dwSkill) == FALSE)
						return;

					CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
					if (pTaskBar->m_nExecute == 0)
						pTaskBar->SetSkillQueue(pTaskBar->m_nCurQueueNum, 0, i, m_atexSkill[i]);
					break;
				}
			}
		}
	}
}

#ifdef __FL_FAST_ELEMENT
CWndChangeAttributeNew::CWndChangeAttributeNew()
	: m_pItemElemScroll(NULL)
	, m_pTextureScroll(NULL)
{
	ClearMainItem();
}

CWndChangeAttributeNew::~CWndChangeAttributeNew()
{
	ClearScroll();
}

void CWndChangeAttributeNew::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	if (pComboBox)
	{
		pComboBox->AddWndStyle(EBS_READONLY);
		int nIdx = pComboBox->AddString(("%s", "Right-hand weapon"));
		pComboBox->SetItemData(nIdx, 1);
		nIdx = pComboBox->AddString(("%s", "Left-hand weapon"));
		pComboBox->SetItemData(nIdx, 2);
		nIdx = pComboBox->AddString(("%s", "Suit"));
		pComboBox->SetItemData(nIdx, 3);
		pComboBox->SetCurSel(0);
	}

	m_nAttributeStaticID[0] = WIDC_CUSTOM1;
	m_nAttributeStaticID[1] = WIDC_CUSTOM2;
	m_nAttributeStaticID[2] = WIDC_CUSTOM3;
	m_nAttributeStaticID[3] = WIDC_CUSTOM4;
	m_nAttributeStaticID[4] = WIDC_CUSTOM5;

	m_nTooltipTextIndx[0] = TID_TOOLTIP_CA_INSLOT;
	m_nTooltipTextIndx[1] = TID_TOOLTIP_CA_FIRE;
	m_nTooltipTextIndx[2] = TID_TOOLTIP_CA_WATER;
	m_nTooltipTextIndx[3] = TID_TOOLTIP_CA_ELEC;
	m_nTooltipTextIndx[4] = TID_TOOLTIP_CA_WIND;
	m_nTooltipTextIndx[5] = TID_TOOLTIP_CA_EARTH;

	ItemProp* pProp = prj.GetItemProp(__FL_FAST_ELEMENT);
	if (pProp)
		m_pTextureScroll = CWndBase::m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

	MoveParentCenter();
}

BOOL CWndChangeAttributeNew::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_FAST_ELEMENT, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndChangeAttributeNew::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndChangeAttributeNew::OnDraw(C2DRender* p2DRender)
{
	if (m_pTexture)
		m_pTexture->Render(p2DRender, CPoint(GetWndCtrl(WIDC_STATIC1)->rect.left, GetWndCtrl(WIDC_STATIC1)->rect.top));

	if (m_pTextureScroll)
		m_pTextureScroll->Render(p2DRender, CPoint(GetWndCtrl(WIDC_STATIC2)->rect.left, GetWndCtrl(WIDC_STATIC2)->rect.top), m_pItemElemScroll ? 255 : 100);

	if (m_pItemElemScroll)
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

		CString str;
		str.Format("%d", m_pItemElemScroll->GetExtra());
		CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
		m_p2DRender->TextOut(GetWndCtrl(WIDC_STATIC2)->rect.right - size.cx - 2, GetWndCtrl(WIDC_STATIC2)->rect.bottom - size.cy, str, 0xffeaebdd);

		p2DRender->SetFont(pOldFont);
	}

	for (int i = 0; i < 5; i++)
	{
		CRect rect = GetWndCtrl(m_nAttributeStaticID[i])->rect;
		g_WndMng.m_pWndWorld->m_texAttrIcon.Render(p2DRender, CPoint(rect.left, rect.top), i, 255, 1.5f, 1.5f);

		if (i == m_nAttributeNum)
			FillRect(p2DRender, GetWndCtrl(m_nAttributeStaticID[i])->rect, 0xffbb00ff, 0x00bb00ff);
	}
}

BOOL CWndChangeAttributeNew::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (!g_pPlayer)
		return FALSE;

	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	if (pTempElem->m_dwItemId == __FL_FAST_ELEMENT)
		SetScroll(pTempElem);

	return TRUE;
}

BOOL CWndChangeAttributeNew::Process()
{
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);

	if (m_pItemElem)
	{
		m_nAttributeNum = m_pItemElem->m_bItemResist - 1;

		BOOL bValidPart = FALSE;
		switch (pComboBox->GetSelectedItemData())
		{
		case 1:
		{
			if (m_pItemElem == g_pPlayer->GetEquipItem(PARTS_RWEAPON))
				bValidPart = TRUE;

			break;
		}
		case 2:
		{
			if (g_pPlayer->GetEquipItem(PARTS_LWEAPON))
			{
				if (m_pItemElem == g_pPlayer->GetEquipItem(PARTS_LWEAPON))
					bValidPart = TRUE;
			}
			else if (g_pPlayer->GetEquipItem(PARTS_SHIELD))
			{
				if (m_pItemElem == g_pPlayer->GetEquipItem(PARTS_SHIELD))
					bValidPart = TRUE;
			}
			break;
		}
		case 3:
		{
			if (m_pItemElem == g_pPlayer->GetEquipItem(PARTS_UPPER_BODY))
				bValidPart = TRUE;

			break;
		}
		}

		if (!bValidPart)
			ClearMainItem();
	}
	else
	{
		switch (pComboBox->GetSelectedItemData())
		{
		case 1:
		{
			if (g_pPlayer->GetEquipItem(PARTS_RWEAPON))
			{
				m_pItemElem = g_pPlayer->GetEquipItem(PARTS_RWEAPON);
				m_nInvPosition = PARTS_RWEAPON;
			}
			break;
		}
		case 2:
		{
			if (g_pPlayer->GetEquipItem(PARTS_LWEAPON))
			{
				if (m_pItemElem != g_pPlayer->GetEquipItem(PARTS_LWEAPON))
				{
					m_pItemElem = g_pPlayer->GetEquipItem(PARTS_LWEAPON);
					m_nInvPosition = PARTS_LWEAPON;
				}
			}
			else if (g_pPlayer->GetEquipItem(PARTS_SHIELD))
			{
				if (m_pItemElem != g_pPlayer->GetEquipItem(PARTS_SHIELD))
				{
					m_pItemElem = g_pPlayer->GetEquipItem(PARTS_SHIELD);
					m_nInvPosition = PARTS_SHIELD;
				}
			}
			break;
		}
		case 3:
		{
			if (g_pPlayer->GetEquipItem(PARTS_UPPER_BODY))
			{
				m_pItemElem = g_pPlayer->GetEquipItem(PARTS_UPPER_BODY);
				m_nInvPosition = PARTS_UPPER_BODY;
			}
			break;
		}
		}

		ItemProp* pProp = NULL;
		if (m_pItemElem)
			pProp = m_pItemElem->GetProp();

		if (!m_pItemElem || !m_pItemElem->m_bItemResist || !pProp || !pProp->IsEleRefineryAble())
			ClearMainItem();
		else
		{
			if (pProp)
				m_pTexture = CWndBase::m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		}
	}

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_STATIC1);
	CRect rect = lpWndCtrl->rect;
	CPoint ptMouse = GetMousePoint();
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		if (m_pItemElem)
			g_WndMng.PutToolTip_Item(m_pItemElem, ptMouse, &rect);
	}

	lpWndCtrl = GetWndCtrl(WIDC_STATIC2);
	rect = lpWndCtrl->rect;
	if (rect.PtInRect(ptMouse))
	{
		ClientToScreen(&ptMouse);
		ClientToScreen(&rect);
		if (m_pItemElemScroll)
			g_WndMng.PutToolTip_Item(m_pItemElemScroll, ptMouse, &rect);
	}

	for (int i = 0; i < 5; i++)
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if (rect.PtInRect(ptMouse))
		{
			ClientToScreen(&ptMouse);
			ClientToScreen(&rect);
			g_toolTip.PutToolTip(10000, prj.GetText(m_nTooltipTextIndx[i + 1]), rect, ptMouse, 1);
			break;
		}
	}

	return TRUE;
}

void CWndChangeAttributeNew::ClearMainItem()
{
	m_pItemElem = NULL;
	m_pTexture = NULL;

	m_nInvPosition = -1;
	m_nAttributeNum = -1;
}
void CWndChangeAttributeNew::ClearScroll()
{
	if (g_pPlayer && m_pItemElemScroll)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElemScroll))
			m_pItemElemScroll->SetExtra(0);

		m_pItemElemScroll = NULL;
	}
}
void CWndChangeAttributeNew::SetScroll(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	ClearScroll();
	m_pItemElemScroll = pItemElem;
	m_pItemElemScroll->SetExtra(m_pItemElemScroll->m_nItemNum);
}
void CWndChangeAttributeNew::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);

	for (int i = 0; i < 5; i++)
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if (rect.PtInRect(point))
		{
			if (!m_pItemElemScroll)
			{
				g_WndMng.PutString(prj.GetText(TID_UPGRADE_ERROR_WRONGSUPITEM));
				break;
			}
			if (m_pItemElem && m_nAttributeNum != i)
			{
				m_nAttributeNum = i;
				g_DPlay.SendSelectElement(m_pItemElemScroll->m_dwObjId, m_nAttributeNum + 1, m_nInvPosition);

				m_pItemElemScroll->SetExtra(m_pItemElemScroll->GetExtra() - 1);
				if (m_pItemElemScroll->GetExtra() <= 0)
					ClearScroll();
			}
			break;
		}
	}
}

BOOL CWndChangeAttributeNew::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	CRect rect;
	LPWNDCTRL lpWndCtrl;
	BOOL bOnTitle = FALSE;
	CPoint point = GetMousePoint();

	for (int i = 0; i < 5; i++)
	{
		lpWndCtrl = GetWndCtrl(m_nAttributeStaticID[i]);
		rect = lpWndCtrl->rect;
		if (rect.PtInRect(point))
		{
			bOnTitle = TRUE;
			break;
		}
	}


	if (bOnTitle)
		SetMouseCursor(CUR_SELECT);
	else
	{
		SetMouseCursor(CUR_BASE);
		CWndBase::OnSetCursor(pWndBase, nHitTest, message);
	}

	return TRUE;
}
void CWndChangeAttributeNew::FillRect(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend)
{
	CPoint Point = CPoint(rectBg.TopLeft().x, rectBg.TopLeft().y);
	float fVar = 0.04f;

	D3DXCOLOR dwColorDest2 = dwColorstart;
	D3DXCOLOR dwColor = dwColorend;
	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB(0, 255, 255, 255);
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB(0, 255, 255, 255);

	float fLerp = 1.0f;

	D3DXColorLerp(&dwColor2, &dwColor, &dwColorDest2, fLerp);
	int nthick = 8;
	CRect Rect = CRect(0, 0, rectBg.right - rectBg.left, rectBg.bottom - rectBg.top);

	p2DRender->RenderFillRect(CRect(Point.x + Rect.left - 2, Point.y + Rect.top - 2, Point.x + Rect.left + nthick - 2, Point.y + Rect.bottom + 2),
		dwColor2, dwColor1, dwColor2, dwColor1);

	p2DRender->RenderFillRect(CRect(Point.x + Rect.left - 2, Point.y + Rect.top + nthick - 2, Point.x + Rect.right + 2, Point.y + Rect.top - 2),
		dwColor1, dwColor1, dwColor2, dwColor2);

	p2DRender->RenderFillRect(CRect(Point.x + Rect.right + 2, Point.y + Rect.top - 2, Point.x + Rect.right - nthick + 2, Point.y + Rect.bottom + 2),
		dwColor2, dwColor1, dwColor2, dwColor1);

	p2DRender->RenderFillRect(CRect(Point.x + Rect.left - 2, Point.y + Rect.bottom + 2, Point.x + Rect.right + 2, Point.y + Rect.bottom - nthick + 2),
		dwColor2, dwColor2, dwColor1, dwColor1);
}

#endif

#ifdef __FL_SHOP_SEARCH
CWndShopSearch::CWndShopSearch(void)
{

}

CWndShopSearch::~CWndShopSearch(void)
{
	ClearItems();
	vecClientShopsItems.clear();
}
BOOL CWndShopSearch::Process(void)
{
	if (m_pListItems->GetCurSel() >= 0)
	{
		pWndButtonTeleport->EnableWindow(TRUE);
		pWndButtonPM->EnableWindow(TRUE);
	}
	else
	{
		pWndButtonTeleport->EnableWindow(FALSE);
		pWndButtonPM->EnableWindow(FALSE);
	}

	return TRUE;
}
void CWndShopSearch::OnDraw(C2DRender* p2DRender)
{

}
void CWndShopSearch::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	g_DPlay.SendHdr(PACKETTYPE_REQUEST_SHOP_SEARCH);

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndButtonRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pWndButtonTeleport = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	pWndButtonReset = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButtonPM = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	m_pNameFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pMinLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pMaxLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	m_pComboSex = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pComboJob = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);

	m_pMinLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMaxLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMinLevelFilter->SetString("1");
	m_pMaxLevelFilter->SetString("200");

	m_pComboSex->AddString("~");
	m_pComboSex->AddString("Male");
	m_pComboSex->AddString("Female");
	m_pComboSex->SetCurSel(0);
	m_pComboSex->AddWndStyle(EBS_READONLY);

	m_pComboJob->AddString("~");
	for (int i = 0; i < MAX_JOB; ++i)
	{
		if (i == 5 || i == 14 || i == 15)
			continue;
		m_pComboJob->AddString(prj.m_aJob[i].szName);
	}
	m_pComboJob->SetCurSel(0);
	m_pComboJob->AddWndStyle(EBS_READONLY);

	MoveParentCenter();
}
void CWndShopSearch::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
BOOL CWndShopSearch::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_SHOP_SEARCH);
}

BOOL CWndShopSearch::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndShopSearch::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndShopSearch::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			g_DPlay.SendHdr(PACKETTYPE_REQUEST_SHOP_SEARCH);
		}
		else if (nID == WIDC_BUTTON2)
		{
			if (m_pListItems->GetCurSel() >= 0 && m_pListItems->GetCurSel() <= (int)(vecClientShopsItems.size()))
				g_DPlay.SendTeleportToShop(vecClientShopsItems.at(m_pListItems->GetCurSel()).dwOwnerID);

		}
		else if (nID == WIDC_BUTTON3)
		{
			m_pNameFilter->SetString("");
			m_pMinLevelFilter->SetString("1");
			m_pMaxLevelFilter->SetString("200");
			m_pComboSex->SetCurSel(0);
			m_pComboJob->SetCurSel(0);
			UpdateData();
		}
		else if (nID == WIDC_BUTTON4)
		{
			if (m_pListItems->GetCurSel() >= 0 && m_pListItems->GetCurSel() <= (int)(vecClientShopsItems.size()))
			{
				CWndMessage* pWndMessage = g_WndMng.GetMessagge(vecClientShopsItems.at(m_pListItems->GetCurSel()).strOwnerName);
				if (!pWndMessage)
					pWndMessage = g_WndMng.OpenMessage(vecClientShopsItems.at(m_pListItems->GetCurSel()).strOwnerName);
			}
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1 || nID == WIDC_COMBOBOX2)
			UpdateData();

	}
	else if (message == EN_CHANGE)
	{
		if (nID == WIDC_EDIT1 || nID == WIDC_EDIT2 || nID == WIDC_EDIT3)
			UpdateData();
	}
	else if (message == WNM_DBLCLK)
	{
		if (nID == WIDC_LISTBOX1)
		{
			if (m_pListItems->GetCurSel() != -1)
			{
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				{
#ifdef __FL_PACKED_ITEMS
					if (vecClientShopsItems.at(m_pListItems->GetCurSel()).pItem->IsPackage())
						g_WndMng.PreviewPackage(vecClientShopsItems.at(m_pListItems->GetCurSel()).pItem);
#endif
#ifdef __MODEL_VIEW
					else
						g_WndMng.PreviewModel(vecClientShopsItems.at(m_pListItems->GetCurSel()).pItem);
#endif
				}
			}
		}
	}


	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndShopSearch::AcceptResult(CAr& ar)
{
	ClearItems();

	DWORD dwOwnerID = NULL_ID;
	char strOwnerName[MAX_NAME], strShopName[MAX_NAME];
	BYTE nVendorItem;
	int nExtra;
	SHOP_ITEMS sh;

	u_long nSize;
	ar >> nSize;

	for (u_long i = 0; i < nSize; i++)
	{
		ar >> dwOwnerID;
		ar.ReadString(strOwnerName, MAX_NAME);
		ar.ReadString(strShopName, MAX_NAME);

		ar >> nVendorItem;
		for (int z = 0; z < nVendorItem; z++)
		{
			CItemElem* pItemElem = new CItemElem;

			pItemElem->Serialize(ar);
			ar >> nExtra;
			ar >> pItemElem->m_nCost;
#ifdef __FL_VENDORS
			ar >> pItemElem->m_nPerinCost;
#endif
			pItemElem->SetExtra(nExtra);

			sh.dwOwnerID = dwOwnerID;
			sh.pItem = pItemElem;
			sh.strOwnerName.Format("%s", strOwnerName);
			sh.strVendorTitle.Format("%s", strShopName);
			vecServerShopsItems.push_back(sh);
		}
	}

	UpdateData();
}
void CWndShopSearch::UpdateData()
{
	vecClientShopsItems.clear();

	for (auto& it : vecServerShopsItems)
	{
		// Prop Check
		ItemProp* pItemProp = it.pItem->GetProp();
		if (!pItemProp)
			continue;

		// Name Filter
		CString strFilter = m_pNameFilter->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = it.pItem->GetName();
			if (!strName.IsEmpty())
			{
				if (it.pItem->GetAbilityOption())
					strName.Format("%s %+d", strName, it.pItem->GetAbilityOption());

				if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
					continue;
			}
		}

		// Level Filters
		DWORD dwLimitLevel = 1;
		if (pItemProp->dwLimitLevel1 && pItemProp->dwLimitLevel1 != 0xFFFFFFFF)
			dwLimitLevel = pItemProp->dwLimitLevel1;

		int nMinLevel = atoi(m_pMinLevelFilter->GetString());
		if (nMinLevel <= 0)
			nMinLevel = 1;

		if ((int)dwLimitLevel < nMinLevel)
			continue;

		int nMaxLevel = atoi(m_pMaxLevelFilter->GetString());
		if ((int)dwLimitLevel > nMaxLevel)
			continue;

		// Sex Filters
		if (m_pComboSex->GetCurSel() != 0)
		{
			if (pItemProp->dwItemSex != SEX_FEMALE && m_pComboSex->GetCurSel() == 2)
				continue;

			if (pItemProp->dwItemSex != SEX_MALE && m_pComboSex->GetCurSel() == 1)
				continue;
		}

		// Job Filter
		if (m_pComboJob->GetCurSel() >= 1)
		{
			DWORD dwJob = JOB_VAGRANT;
			switch (m_pComboJob->GetCurSel())
			{
			case 2: dwJob = JOB_MERCENARY; break;
			case 3: dwJob = JOB_ACROBAT; break;
			case 4: dwJob = JOB_ASSIST; break;
			case 5: dwJob = JOB_MAGICIAN; break;
			case 6: dwJob = JOB_KNIGHT; break;
			case 7: dwJob = JOB_BLADE; break;
			case 8: dwJob = JOB_JESTER; break;
			case 9: dwJob = JOB_RANGER; break;
			case 10: dwJob = JOB_RINGMASTER; break;
			case 11: dwJob = JOB_BILLPOSTER; break;
			case 12: dwJob = JOB_PSYCHIKEEPER; break;
			case 13: dwJob = JOB_ELEMENTOR; break;
			case 14: dwJob = JOB_KNIGHT_MASTER; break;
			case 15: dwJob = JOB_BLADE_MASTER; break;
			case 16: dwJob = JOB_JESTER_MASTER; break;
			case 17: dwJob = JOB_RANGER_MASTER; break;
			case 18: dwJob = JOB_RINGMASTER_MASTER; break;
			case 19: dwJob = JOB_BILLPOSTER_MASTER; break;
			case 20: dwJob = JOB_PSYCHIKEEPER_MASTER; break;
			case 21: dwJob = JOB_ELEMENTOR_MASTER; break;
			case 22: dwJob = JOB_KNIGHT_HERO; break;
			case 23: dwJob = JOB_BLADE_HERO; break;
			case 24: dwJob = JOB_JESTER_HERO; break;
			case 25: dwJob = JOB_RANGER_HERO; break;
			case 26: dwJob = JOB_RINGMASTER_HERO; break;
			case 27: dwJob = JOB_BILLPOSTER_HERO; break;
			case 28: dwJob = JOB_PSYCHIKEEPER_HERO; break;
			case 29: dwJob = JOB_ELEMENTOR_HERO; break;
			case 30: dwJob = JOB_LORDTEMPLER_HERO; break;
			case 31: dwJob = JOB_STORMBLADE_HERO; break;
			case 32: dwJob = JOB_WINDLURKER_HERO; break;
			case 33: dwJob = JOB_CRACKSHOOTER_HERO; break;
			case 34: dwJob = JOB_FLORIST_HERO; break;
			case 35: dwJob = JOB_FORCEMASTER_HERO; break;
			case 36: dwJob = JOB_MENTALIST_HERO; break;
			case 37: dwJob = JOB_ELEMENTORLORD_HERO; break;
			default: dwJob = JOB_VAGRANT; break;
			}

			if (pItemProp->dwItemJob != dwJob)
				continue;
		}

		vecClientShopsItems.push_back(it);
	}

	m_pListItems->ResetContent();
	for (auto& it : vecClientShopsItems)
		m_pListItems->AddString("");

}
void CWndShopSearch::ClearItems()
{
	for (auto& it : vecServerShopsItems)
		SAFE_DELETE(it.pItem);

	vecServerShopsItems.clear();
}
#endif


#ifdef __FL_PET_FILTER
CWndPetLootOption::CWndPetLootOption()
	: dwSaveTime(NULL)
	, m_nLootType(g_Option.m_nLootType)
	, m_nLootPlace(g_Option.m_nLootPlace)
{
}
CWndPetLootOption::~CWndPetLootOption()
{
	SaveData();
}
void CWndPetLootOption::OnDraw(C2DRender* p2DRender)
{
}
void CWndPetLootOption::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	// Container
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	pComboBox->AddWndStyle(EBS_READONLY);
	int nIdx = 0;

	for (int i = TYPE_INVENTORY; i < TYPE_MAX; i++)
	{
		nIdx = pComboBox->AddString(g_WndMng.GetStringNameByContainerType(i));
		pComboBox->SetItemData(nIdx, i);
	}

	pComboBox->SetCurSel(m_nLootPlace);

	ReloadPresets();
	UpdateCheckboxes();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
BOOL CWndPetLootOption::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_LOOT_OPTION, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndPetLootOption::Process(void)
{
	if (dwSaveTime != NULL)
	{
		if (GetTickCount() - 1250 > dwSaveTime)
		{
			SaveData();
			dwSaveTime = NULL;
		}
	}

	return TRUE;
}
BOOL CWndPetLootOption::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetLootOption::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPetLootOption::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPetLootOption::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPetLootOption::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndOption1 = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndOption2 = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	CWndButton* pWndOption3 = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	CWndButton* pWndOption4 = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	CWndButton* pWndOption5 = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	CWndButton* pWndOption6 = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	CWndButton* pWndOption7 = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	CWndButton* pWndOption8 = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	CWndButton* pWndOption9 = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	CWndButton* pWndOption10 = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	CWndButton* pWndOption11 = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	CWndButton* pWndOption12 = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	CWndButton* pWndOption13 = (CWndButton*)GetDlgItem(WIDC_CHECK13);
	CWndButton* pWndOption14 = (CWndButton*)GetDlgItem(WIDC_CHECK14);
	CWndButton* pWndOption15 = (CWndButton*)GetDlgItem(WIDC_CHECK15);
	CWndButton* pWndOption16 = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	CWndButton* pWndOption17 = (CWndButton*)GetDlgItem(WIDC_CHECK17);
	CWndButton* pWndOption18 = (CWndButton*)GetDlgItem(WIDC_CHECK18);
	CWndButton* pWndOption19 = (CWndButton*)GetDlgItem(WIDC_CHECK19);
	CWndButton* pWndOption20 = (CWndButton*)GetDlgItem(WIDC_CHECK20);
	CWndButton* pWndOption21 = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	CWndButton* pWndOption22 = (CWndButton*)GetDlgItem(WIDC_CHECK22);
	CWndButton* pWndOption23 = (CWndButton*)GetDlgItem(WIDC_CHECK23);
	CWndButton* pWndOption24 = (CWndButton*)GetDlgItem(WIDC_CHECK24);

	switch (nID)
	{
	case WIDC_CHECK1:
		if (pWndOption1->GetCheck())		m_nLootType = m_nLootType | 0x1;
		else								m_nLootType = m_nLootType & ~0x1;
		break;
	case WIDC_CHECK2:
		if (pWndOption2->GetCheck())		m_nLootType = m_nLootType | 0x2;
		else								m_nLootType = m_nLootType & ~0x2;
		break;
	case WIDC_CHECK3:
		if (pWndOption3->GetCheck())		m_nLootType = m_nLootType | 0x4;
		else								m_nLootType = m_nLootType & ~0x4;
		break;
	case WIDC_CHECK4:
		if (pWndOption4->GetCheck())		m_nLootType = m_nLootType | 0x8;
		else								m_nLootType = m_nLootType & ~0x8;
		break;
	case WIDC_CHECK5:
		if (pWndOption5->GetCheck())		m_nLootType = m_nLootType | 0x10;
		else								m_nLootType = m_nLootType & ~0x10;
		break;
	case WIDC_CHECK6:
		if (pWndOption6->GetCheck())		m_nLootType = m_nLootType | 0x20;
		else								m_nLootType = m_nLootType & ~0x20;
		break;
	case WIDC_CHECK7:
		if (pWndOption7->GetCheck())		m_nLootType = m_nLootType | 0x40;
		else								m_nLootType = m_nLootType & ~0x40;
		break;
	case WIDC_CHECK8:
		if (pWndOption8->GetCheck())		m_nLootType = m_nLootType | 0x80;
		else								m_nLootType = m_nLootType & ~0x80;
		break;
	case WIDC_CHECK9:
		if (pWndOption9->GetCheck())		m_nLootType = m_nLootType | 0x100;
		else								m_nLootType = m_nLootType & ~0x100;
		break;
	case WIDC_CHECK10:
		if (pWndOption10->GetCheck())		m_nLootType = m_nLootType | 0x200;
		else								m_nLootType = m_nLootType & ~0x200;
		break;
	case WIDC_CHECK11:
		if (pWndOption11->GetCheck())		m_nLootType = m_nLootType | 0x400;
		else								m_nLootType = m_nLootType & ~0x400;
		break;
	case WIDC_CHECK12:
		if (pWndOption12->GetCheck())		m_nLootType = m_nLootType | 0x800;
		else								m_nLootType = m_nLootType & ~0x800;
		break;
	case WIDC_CHECK13:
		if (pWndOption13->GetCheck())		m_nLootType = m_nLootType | 0x1000;
		else								m_nLootType = m_nLootType & ~0x1000;
		break;
	case WIDC_CHECK14:
		if (pWndOption14->GetCheck())		m_nLootType = m_nLootType | 0x2000;
		else								m_nLootType = m_nLootType & ~0x2000;
		break;

	case WIDC_CHECK15:
		if (pWndOption15->GetCheck())		m_nLootType = m_nLootType | 0x4000;
		else								m_nLootType = m_nLootType & ~0x4000;
		break;
	case WIDC_CHECK16:
		if (pWndOption16->GetCheck())		m_nLootType = m_nLootType | 0x8000;
		else								m_nLootType = m_nLootType & ~0x8000;
		break;
	case WIDC_CHECK17:
		if (pWndOption17->GetCheck())		m_nLootType = m_nLootType | 0x10000;
		else								m_nLootType = m_nLootType & ~0x10000;
		break;
	case WIDC_CHECK18:
		if (pWndOption18->GetCheck())		m_nLootType = m_nLootType | 0x20000;
		else								m_nLootType = m_nLootType & ~0x20000;
		break;
	case WIDC_CHECK19:
		if (pWndOption19->GetCheck())		m_nLootType = m_nLootType | 0x40000;
		else								m_nLootType = m_nLootType & ~0x40000;
		break;
	case WIDC_CHECK20:
		if (pWndOption20->GetCheck())		m_nLootType = m_nLootType | 0x80000;
		else								m_nLootType = m_nLootType & ~0x80000;
		break;
	case WIDC_CHECK21:
		if (pWndOption21->GetCheck())		m_nLootType = m_nLootType | 0x100000;
		else								m_nLootType = m_nLootType & ~0x100000;
		break;
	case WIDC_CHECK22:
		if (pWndOption22->GetCheck())		m_nLootType = m_nLootType | 0x200000;
		else								m_nLootType = m_nLootType & ~0x200000;
		break;


		// All or nothing
	case WIDC_CHECK23:
	{
		if (pWndOption23->GetCheck())
		{
			m_nLootType = m_nLootType | 0x400000;

			if (pWndOption24->GetCheck())
			{
				pWndOption24->SetCheck(FALSE);
				m_nLootType = m_nLootType & ~0x800000;
			}
		}
		else
			m_nLootType = m_nLootType & ~0x400000;
	}
	break;
	case WIDC_CHECK24:
	{
		if (pWndOption24->GetCheck())
		{
			m_nLootType = m_nLootType | 0x800000;

			if (pWndOption23->GetCheck())
			{
				pWndOption23->SetCheck(FALSE);
				m_nLootType = m_nLootType & ~0x400000;
			}
		}
		else
			m_nLootType = m_nLootType & ~0x800000;
	}
	break;

	// Close
	case WIDC_CANCEL:
		Destroy();
		break;

		// Save & Delete presets
	case WIDC_BUTTON1:
	{
		CWndEdit* pType = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		if (CheckPresetValidName(pType->GetString()))
		{
			auto iter = prj.mapPetFilterPresets.find(pType->GetString());
			if (iter != prj.mapPetFilterPresets.end())
				iter->second = m_nLootType;
			else
				prj.mapPetFilterPresets.insert(make_pair(pType->GetString(), m_nLootType));

			ReloadPresets();
			prj.SavePetFilterPresets();
			pType->Empty();
			g_WndMng.PutString("Preset saved or updated successfully.");
		}
		else
			g_WndMng.PutString("Preset name is invalid. It should have 3-16 letters and should not contain special symbols.");
	}
	break;

	case WIDC_BUTTON2:
	{
		CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
		if (pComboBox->GetCurSel() >= 0 && pComboBox->GetCurSel() <= prj.mapPetFilterPresets.size())
		{
			prj.mapPetFilterPresets.erase(pComboBox->GetString());
			ReloadPresets();
			prj.SavePetFilterPresets();
		}
	}
	break;
	}

	if (message == WNM_SELCHANGE)
	{
		switch (nID)
		{
		case WIDC_COMBOBOX1:
		{
			CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
			if (pComboBox->GetSelectedItemData() >= TYPE_INVENTORY && pComboBox->GetSelectedItemData() < TYPE_MAX)
				m_nLootPlace = pComboBox->GetSelectedItemData();
		}
		break;
		case WIDC_COMBOBOX2:
		{
			CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
			if (pComboBox->GetCurSel() >= 0 && pComboBox->GetCurSel() <= prj.mapPetFilterPresets.size())
			{
				auto iter = prj.mapPetFilterPresets.find(pComboBox->GetString());
				if (iter != prj.mapPetFilterPresets.end())
				{
					m_nLootType = iter->second;
					UpdateCheckboxes();
				}
			}
		}
		break;
		}
	}

	dwSaveTime = GetTickCount();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPetLootOption::UpdateCheckboxes()
{
	CWndButton* pWndOption1 = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndOption2 = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	CWndButton* pWndOption3 = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	CWndButton* pWndOption4 = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	CWndButton* pWndOption5 = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	CWndButton* pWndOption6 = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	CWndButton* pWndOption7 = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	CWndButton* pWndOption8 = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	CWndButton* pWndOption9 = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	CWndButton* pWndOption10 = (CWndButton*)GetDlgItem(WIDC_CHECK10);
	CWndButton* pWndOption11 = (CWndButton*)GetDlgItem(WIDC_CHECK11);
	CWndButton* pWndOption12 = (CWndButton*)GetDlgItem(WIDC_CHECK12);
	CWndButton* pWndOption13 = (CWndButton*)GetDlgItem(WIDC_CHECK13);
	CWndButton* pWndOption14 = (CWndButton*)GetDlgItem(WIDC_CHECK14);
	CWndButton* pWndOption15 = (CWndButton*)GetDlgItem(WIDC_CHECK15);
	CWndButton* pWndOption16 = (CWndButton*)GetDlgItem(WIDC_CHECK16);
	CWndButton* pWndOption17 = (CWndButton*)GetDlgItem(WIDC_CHECK17);
	CWndButton* pWndOption18 = (CWndButton*)GetDlgItem(WIDC_CHECK18);
	CWndButton* pWndOption19 = (CWndButton*)GetDlgItem(WIDC_CHECK19);
	CWndButton* pWndOption20 = (CWndButton*)GetDlgItem(WIDC_CHECK20);
	CWndButton* pWndOption21 = (CWndButton*)GetDlgItem(WIDC_CHECK21);
	CWndButton* pWndOption22 = (CWndButton*)GetDlgItem(WIDC_CHECK22);
	CWndButton* pWndOption23 = (CWndButton*)GetDlgItem(WIDC_CHECK23);
	CWndButton* pWndOption24 = (CWndButton*)GetDlgItem(WIDC_CHECK24);

	if (m_nLootType & 0x1) pWndOption1->SetCheck(TRUE); else pWndOption1->SetCheck(FALSE);
	if (m_nLootType & 0x2) pWndOption2->SetCheck(TRUE); else pWndOption2->SetCheck(FALSE);
	if (m_nLootType & 0x4) pWndOption3->SetCheck(TRUE); else pWndOption3->SetCheck(FALSE);
	if (m_nLootType & 0x8) pWndOption4->SetCheck(TRUE); else pWndOption4->SetCheck(FALSE);
	if (m_nLootType & 0x10) pWndOption5->SetCheck(TRUE); else pWndOption5->SetCheck(FALSE);
	if (m_nLootType & 0x20) pWndOption6->SetCheck(TRUE); else pWndOption6->SetCheck(FALSE);
	if (m_nLootType & 0x40) pWndOption7->SetCheck(TRUE); else pWndOption7->SetCheck(FALSE);
	if (m_nLootType & 0x80) pWndOption8->SetCheck(TRUE); else pWndOption8->SetCheck(FALSE);
	if (m_nLootType & 0x100) pWndOption9->SetCheck(TRUE); else pWndOption9->SetCheck(FALSE);
	if (m_nLootType & 0x200) pWndOption10->SetCheck(TRUE); else pWndOption10->SetCheck(FALSE);
	if (m_nLootType & 0x400) pWndOption11->SetCheck(TRUE); else pWndOption11->SetCheck(FALSE);
	if (m_nLootType & 0x800) pWndOption12->SetCheck(TRUE); else pWndOption12->SetCheck(FALSE);
	if (m_nLootType & 0x1000) pWndOption13->SetCheck(TRUE); else pWndOption13->SetCheck(FALSE);
	if (m_nLootType & 0x2000) pWndOption14->SetCheck(TRUE); else pWndOption14->SetCheck(FALSE);
	if (m_nLootType & 0x4000) pWndOption15->SetCheck(TRUE); else pWndOption15->SetCheck(FALSE);
	if (m_nLootType & 0x8000) pWndOption16->SetCheck(TRUE); else pWndOption16->SetCheck(FALSE);
	if (m_nLootType & 0x10000) pWndOption17->SetCheck(TRUE); else pWndOption17->SetCheck(FALSE);
	if (m_nLootType & 0x20000) pWndOption18->SetCheck(TRUE); else pWndOption18->SetCheck(FALSE);
	if (m_nLootType & 0x40000) pWndOption19->SetCheck(TRUE); else pWndOption19->SetCheck(FALSE);
	if (m_nLootType & 0x80000) pWndOption20->SetCheck(TRUE); else pWndOption20->SetCheck(FALSE);
	if (m_nLootType & 0x100000) pWndOption21->SetCheck(TRUE); else pWndOption21->SetCheck(FALSE);
	if (m_nLootType & 0x200000) pWndOption22->SetCheck(TRUE); else pWndOption22->SetCheck(FALSE);
	if (m_nLootType & 0x400000) pWndOption23->SetCheck(TRUE); else pWndOption23->SetCheck(FALSE);
	if (m_nLootType & 0x800000) pWndOption24->SetCheck(TRUE); else pWndOption24->SetCheck(FALSE);
}
void CWndPetLootOption::ReloadPresets()
{
	CWndComboBox* pComboBox = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	pComboBox->AddWndStyle(EBS_READONLY);
	pComboBox->ResetContent();

	int nIdx = 0;
	for (auto& it : prj.mapPetFilterPresets)
	{
		nIdx = pComboBox->AddString(it.first);
		pComboBox->SetItemData(nIdx, it.second);
	}
}
BOOL CWndPetLootOption::CheckPresetValidName(CString str)
{
	if (strlen(str) < 3 || strlen(str) > 16)
		return FALSE;

	int nLen = lstrlen(str);
	for (int i = 0; i < nLen; i++)
	{
		char chLetter = str[i];
		if (/*chLetter == ' ' || chLetter == ' ' || */chLetter == '"')
			return FALSE;
	}

	return TRUE;
}
void CWndPetLootOption::SaveData()
{
	if (g_Option.m_nLootType != m_nLootType || g_Option.m_nLootPlace != m_nLootPlace)
	{
		g_Option.m_nLootType = m_nLootType;
		g_Option.m_nLootPlace = m_nLootPlace;
		g_Option.Save("Neuz.ini");
		g_DPlay.SendPetLootOption(m_nLootType, m_nLootPlace);
	}
}
#endif

#ifdef __FL_EQUALIZED_MODE
CWndSelectTmpClass::CWndSelectTmpClass()
{
	nSelectedClass = -1;
}
CWndSelectTmpClass::~CWndSelectTmpClass()
{
}
void CWndSelectTmpClass::OnDraw(C2DRender* p2DRender)
{
}
void CWndSelectTmpClass::SetClass(int nClassType)
{
	CWndButton* pButton1 = NULL;
	if (nClassType == 0)
	{
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotBlad.bmp")), TRUE);
		pButton1->SetToolTip("Blade");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotKnight.bmp")), TRUE);
		pButton1->SetToolTip("Knight");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotJes.bmp")), TRUE);
		pButton1->SetToolTip("Jester");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotRan.bmp")), TRUE);
		pButton1->SetToolTip("Ranger");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotPsy.bmp")), TRUE);
		pButton1->SetToolTip("Psykeeper");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON6);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotElem.bmp")), TRUE);
		pButton1->SetToolTip("Elementor");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON7);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotBillfor.bmp")), TRUE);
		pButton1->SetToolTip("Billposter");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON8);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotRingm.bmp")), TRUE);
		pButton1->SetToolTip("Ringmaster");

	}
	else if (nClassType == 1)
	{
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotStormb.bmp")), TRUE);
		pButton1->SetToolTip("Slayer");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotLord.bmp")), TRUE);
		pButton1->SetToolTip("Templar");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotWindI.bmp")), TRUE);
		pButton1->SetToolTip("Harlequin");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotCracks.bmp")), TRUE);
		pButton1->SetToolTip("Crackshooter");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotMent.bmp")), TRUE);
		pButton1->SetToolTip("Mentalist");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON6);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotElel.bmp")), TRUE);
		pButton1->SetToolTip("Arcanist");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON7);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotForcem.bmp")), TRUE);
		pButton1->SetToolTip("Force Master");
		pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON8);
		pButton1->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, _T("SlotFlor.bmp")), TRUE);
		pButton1->SetToolTip("Seraph");
	}
}
void CWndSelectTmpClass::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}
BOOL CWndSelectTmpClass::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TMP_CLASS, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndSelectTmpClass::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndSelectTmpClass::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndSelectTmpClass::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndSelectTmpClass::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndSelectTmpClass::Process()
{
	if (g_pPlayer && g_pPlayer->GetWorld())
	{
		BOOL bFound = FALSE;
		for (auto& it : prj.m_vecTmpWorlds)
		{
			if (it.dwWorldID == g_pPlayer->GetWorld()->GetID())
			{
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
			Destroy();
	}
	else
		Destroy();

	return TRUE;
}
BOOL CWndSelectTmpClass::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndButton = NULL;

	const static int szButtons[] = { WIDC_BUTTON1, WIDC_BUTTON2, WIDC_BUTTON3, WIDC_BUTTON4, WIDC_BUTTON5, WIDC_BUTTON6, WIDC_BUTTON7, WIDC_BUTTON8 };
	for (int i = 0, j = sizeof(szButtons) / sizeof(szButtons[0]); i < j; i++)
	{
		pWndButton = (CWndButton*)GetDlgItem(szButtons[i]);
		pWndButton->SetCheck(FALSE);

		if (nID == szButtons[i])
		{
			nSelectedClass = i;
			pWndButton->SetCheck(TRUE);
		}
	}

	if (nID == WIDC_BUTTON9)
	{
		if (nSelectedClass >= 0 && nSelectedClass <= 7)
		{
			g_DPlay.SendSelectTmpClass(nSelectedClass);
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif

BOOL CWndAccept1::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);
	Create(_T("Confirmation of deletion"), MB_OKCANCEL, rect, APP_MESSAGEBOX);
	m_wndText.SetString(_T("You are going to remove this card from the equipment. This action cannot be undone. Continue?"));
	m_wndText.ResetString();

	return CWndMessageBox::Initialize(pWndParent, dwWndId);
}
BOOL CWndAccept1::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case IDCANCEL:
			Destroy();
			break;
		case IDOK:
			CWndRemovePiercing* pWndRemovePiercing = (CWndRemovePiercing*)g_WndMng.GetWndBase(APP_SMELT_REMOVE_PIERCING_EX);
			if (pWndRemovePiercing)
				g_DPlay.SendPiercingRemove(pWndRemovePiercing->m_pItemElem->m_dwObjId, pWndRemovePiercing->m_pSlot);

			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndCollectingItems::CWndCollectingItems(void)
{
	m_pListbox = NULL;
	m_pTexMoon = NULL;
	m_pTexLevel = NULL;
}

CWndCollectingItems::~CWndCollectingItems(void)
{

}

BOOL CWndCollectingItems::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_COLLECTING_INFO, 0, CPoint(0, 0), pWndParent);
}

void CWndCollectingItems::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
	m_pListbox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pTexMoon = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, _T("Itm_GenMatMoonstone.dds")), 0xffff00ff);
	m_pTexLevel = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("Icon_ColLevel.tga")), 0xffff00ff);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CString str;
	for (int i = 0; i < CCollectingProperty::GetInstance()->GetMaxCollectorLevel(); i++)
	{
		str.Format("Level %d -> Level %d = %.f%%\n", i, i + 1, float(100 / (1000 / CCollectingProperty::GetInstance()->GetEnchantProbability(i))));
		pWndText->AddString(str);
	}

	MoveParentCenter();
}

void CWndCollectingItems::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl(WIDC_PIC1);
	LPWNDCTRL wndCtrlPic2 = GetWndCtrl(WIDC_PIC2);

	if (m_pTexMoon)
		m_pTexMoon->Render(p2DRender, wndCtrlPic2->rect.TopLeft());
	if (m_pTexLevel)
		m_pTexLevel->Render(p2DRender, wndCtrlPic1->rect.TopLeft());

	CWndNeuz::OnDraw(p2DRender);
}

void CWndCollectingItems::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndCollectingItems::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

#ifdef __FL_GW_STATUE_V2
CWndStatueConfiguration::CWndStatueConfiguration(void)
	: m_nMode(-1)
	, m_pTexture(NULL)
	, m_pEdit(NULL)
{

}

CWndStatueConfiguration::~CWndStatueConfiguration(void)
{

}

BOOL CWndStatueConfiguration::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_STATUE_CONFIGURATION, 0, CPoint(0, 0), pWndParent);
}

void CWndStatueConfiguration::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEdit->AddWndStyle(EBS_NUMBER);
	m_pEdit->SetStringBg("Type price here..");

	MoveParentCenter();
}

void CWndStatueConfiguration::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl(WIDC_PIC1);
	if (m_pTexture)
		m_pTexture->Render(p2DRender, wndCtrlPic1->rect.TopLeft());

	CWndNeuz::OnDraw(p2DRender);
}

void CWndStatueConfiguration::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndStatueConfiguration::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndStatueConfiguration::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			int nCost = _ttoi(m_pEdit->GetString());
			int nMaxCost = 2000000000;
			if (nCost < 1 || nCost > nMaxCost)
			{
				CString str;
				str.Format("Price should be more than 1 and less than %d!", nMaxCost);
				g_WndMng.PutString(str);
			}
			else
			{
				g_DPlay.SendStatueSettings(m_nMode, nCost);
				Destroy();
			}
		}
		else if (nID == WIDC_BUTTON2)
			Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndStatueConfiguration::UpdateStaticParams()
{
	if (m_nMode == NPC_TYPE_MVPSTATUE)
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("TexGuildCombatBest.bmp")), 0xffff00ff);
	else
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("TexGuildCombatWinner.bmp")), 0xffff00ff);

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString("");
	pWndText->AddString("Set the cost of the buff for your statue to allow other players purchase it.");
}

/////////////////////////////////////////
////////////////////////////////////////

CWndStatuePurchase::CWndStatuePurchase(void)
	: m_nMode(-1)
	, m_pTextureWnd(NULL)
	, m_pTextureItem(NULL)
	, m_nBuffPrice(0)
{

}

CWndStatuePurchase::~CWndStatuePurchase(void)
{

}

BOOL CWndStatuePurchase::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_STATUE_PURCHASE, 0, CPoint(0, 0), pWndParent);
}

void CWndStatuePurchase::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	pWndEdit->AddWndStyle(EBS_READONLY);
	pWndEdit->SetString("Loading..");

	MoveParentCenter();
}

void CWndStatuePurchase::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl(WIDC_PIC1);
	if (m_pTextureWnd)
		m_pTextureWnd->Render(p2DRender, wndCtrlPic1->rect.TopLeft());

	LPWNDCTRL wndCtrlPic2 = GetWndCtrl(WIDC_PIC2);
	if (m_pTextureItem)
	{
		CRect rect = wndCtrlPic2->rect;
		CPoint point = GetMousePoint();

		m_pTextureItem->Render(p2DRender, rect.TopLeft());

		ItemProp* pProp = NULL;
		if (m_nMode == NPC_TYPE_MVPSTATUE)
			pProp = prj.GetItemProp(dwMvpStatueBuff);
		else if (m_nMode == NPC_TYPE_GUILDLEADERSTATUE)
			pProp = prj.GetItemProp(dwLeaderStatueBuff);

		if (pProp)
		{
			if (rect.PtInRect(point))
			{
				ClientToScreen(&point);
				ClientToScreen(&rect);

				CItemElem pItemElem;
				pItemElem.m_dwItemId = pProp->dwID;
				g_WndMng.PutToolTip_Item(&pItemElem, point, &rect, GetWndId());
			}
		}
	}

	CWndNeuz::OnDraw(p2DRender);
}

void CWndStatuePurchase::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndStatuePurchase::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndStatuePurchase::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
			g_DPlay.SendReceiveStatueBuff(m_nMode == NPC_TYPE_MVPSTATUE, m_nBuffPrice);
		else if (nID == WIDC_BUTTON2)
			Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndStatuePurchase::UpdateStaticParams()
{
	g_DPlay.SendRequestStatueParams(m_nMode == NPC_TYPE_MVPSTATUE);

	CString str;
	if (m_nMode == NPC_TYPE_MVPSTATUE)
	{
		m_pTextureWnd = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("TexGuildCombatBest.bmp")), 0xffff00ff);
		ItemProp* pProp = prj.GetItemProp(dwMvpStatueBuff);
		if (pProp)
			m_pTextureItem = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

		str.Format("Click the Accept button if you want to purchase the specified buff at the specified price. The owner will receive payment by mail. For owner of this statue buff provided free of charge.");
	}
	else
	{
		m_pTextureWnd = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("TexGuildCombatWinner.bmp")), 0xffff00ff);
		ItemProp* pProp = prj.GetItemProp(dwLeaderStatueBuff);
		if (pProp)
			m_pTextureItem = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

		str.Format("Click the Accept button if you want to purchase the specified buff at the specified price. The owner will receive payment by mail. For owner of this statue and his guild mates buff provided free of charge.");
	}

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString("");
	pWndText->AddString(str);
}
void CWndStatuePurchase::SetBuff(int nPrice)
{
	m_nBuffPrice = nPrice;

	CString str;
	if (m_nBuffPrice > 0)
		str.Format("Price: %s Penya", Separator(m_nBuffPrice));
	else
		str.Format("Statue not activated yet!");

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	pWndEdit->SetString(str);
}
#endif

#ifdef __FL_FFA
CWndFFARegister::CWndFFARegister(void)
	: m_pItemTexture(NULL)
	, m_dwRewardID(NULL_ID)
	, m_nRewardAmount(0)
	, m_bLastFFA(FALSE)
	, m_ct(0)
	, m_tEndTime(0)
	, m_tCurrentTime(0)
	, m_nNextRefresh(0)
{

}

CWndFFARegister::~CWndFFARegister(void)
{

}

BOOL CWndFFARegister::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_FFA_REG, 0, CPoint(0, 0), pWndParent);
}

void CWndFFARegister::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	pEdit->AddWndStyle(EBS_READONLY);
	pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	pEdit->AddWndStyle(EBS_READONLY);
	pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	pEdit->AddWndStyle(EBS_READONLY);

	LoadData();
	MoveParentCenter();
}

void CWndFFARegister::OnDraw(C2DRender* p2DRender)
{
	// Item
	if (m_pItemTexture && m_dwRewardID)
	{
		CWndStatic* pItemStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC5);
		CRect rect = pItemStatic->GetWndRect();
		CPoint point = GetMousePoint();

		m_pItemTexture->Render(p2DRender, rect.TopLeft());

		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
		CString str;
		str.Format("%d", m_nRewardAmount);
		CSize size = p2DRender->m_pFont->GetTextExtent(str);
		p2DRender->TextOut(rect.right - size.cx - 2, rect.bottom - size.cy + 2, str, 0xffeaebdd);
		p2DRender->SetFont(pOldFont);

		if (rect.PtInRect(point))
		{
			ClientToScreen(&point);
			ClientToScreen(&rect);

			CItemElem pItem;
			pItem.m_dwItemId = m_dwRewardID;
			pItem.m_nItemNum = m_nRewardAmount;
			g_WndMng.PutToolTip_Item(&pItem, point, &rect, APP_FFA_REG);
		}
	}

	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndFFARegister::Process(void)
{
	// Time
	m_tCurrentTime = m_tEndTime - time_null();
	CString str;
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (m_tCurrentTime >= 0 && !m_bLastFFA)
	{
		m_ct = m_tCurrentTime;
		str.Format("%.2dh:%.2dm:%.2ds", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds());
		pEdit->SetString(str);
	}
	else
	{
		if (m_bLastFFA)
		{
			str.Format("FFA is over today. Refresh in: %d hour(s).", m_nNextRefresh);
			pEdit->SetString(str);
		}
	}

	return TRUE;
}
void CWndFFARegister::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndFFARegister::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndFFARegister::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
			g_DPlay.SendHdr(PACKETTYPE_REQUEST_FFA_REGISTER);
		else if (nID == WIDC_BUTTON2)
		{
			CWndActivities* pWndActivities = (CWndActivities*)g_WndMng.GetWndBase(APP_PVP_ACTIVITIES);
			if (pWndActivities)
				pWndActivities->Destroy();
		}
		else if (nID == WIDC_BUTTON3)
			LoadData();
		else if (nID == WIDC_BUTTON4)
			g_DPlay.SendHdr(PACKETTYPE_REQUEST_FFA_JOIN);
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndFFARegister::LoadData()
{
	g_DPlay.SendHdr(PACKETTYPE_REQUEST_FFA);

	CWndButton* pWndButtRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButtRefresh->EnableWindow(FALSE);
}
void CWndFFARegister::AcceptData(CAr& ar)
{
	u_long nPlayerCount;
	int nMaxPlayerCount, nJoinFee;
	time_t timeStart;

	ar >> nPlayerCount;
	ar >> nMaxPlayerCount;
	ar >> nJoinFee;
	ar >> m_nRewardAmount;
	ar >> m_dwRewardID;
	ar >> timeStart;
	ar >> m_nNextRefresh;

	SetTime(timeStart);
	m_bLastFFA = timeStart == 0;

	CString str;
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	str.Format("%d/%d", nPlayerCount, nMaxPlayerCount);
	pEdit->SetString(str);

	pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	str.Format("%s", Separator(nJoinFee));
	pEdit->SetString(str);

	ItemProp* pRewardProp = prj.GetItemProp(m_dwRewardID);
	if (pRewardProp)
		m_pItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pRewardProp->szIcon), 0xffff00ff);

	CWndButton* pWndButtRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButtRefresh->EnableWindow(TRUE);
}

//////////////////////////
CWndFFAResult::CWndFFAResult(void)
{

}

CWndFFAResult::~CWndFFAResult(void)
{
	vecPlayerFFA.clear();
}

BOOL CWndFFAResult::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_FFA_RESULT, 0, CPoint(0, 0), pWndParent);
}

void CWndFFAResult::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->vecPlayerFFA.size())
		vecPlayerFFA = g_WndMng.m_pWndWorld->vecPlayerFFA;
	else
		Destroy();

	MoveParentCenter();
}

void CWndFFAResult::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndFFAResult::Process(void)
{
	return TRUE;
}
void CWndFFAResult::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndFFAResult::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndFFAResult::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
			Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif

#ifdef __FL_DAILY_REWARDS
CWndDailyGift::CWndDailyGift(void)
	: m_nDay(0)
{
	m_bNoCloseButton = TRUE;

	for (int i = 0; i < 2; i++)
	{
		m_dwItemID[i] = NULL_ID;
		m_nItemNum[i] = 0;
		m_bBind[i] = FALSE;
		m_pItemTexture[i] = NULL;
	}
}

CWndDailyGift::~CWndDailyGift(void)
{

}

BOOL CWndDailyGift::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DAILY_REWARD, 0, CPoint(0, 0), pWndParent);
}

void CWndDailyGift::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

void CWndDailyGift::OnDraw(C2DRender* p2DRender)
{
	for (int i = 0; i < 2; i++)
	{
		if (m_pItemTexture[i] && m_dwItemID[i])
		{
			const static int nElement[] = { WIDC_STATIC2, WIDC_STATIC3 };
			CWndStatic* pItemStatic = (CWndStatic*)GetDlgItem(nElement[i]);
			CRect rect = pItemStatic->GetWndRect();
			CPoint point = GetMousePoint();

			m_pItemTexture[i]->Render(p2DRender, rect.TopLeft());

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
			CString str;
			str.Format("%d", m_nItemNum[i]);
			CSize size = p2DRender->m_pFont->GetTextExtent(str);
			p2DRender->TextOut(rect.right - size.cx - 2, rect.bottom - size.cy + 2, str, 0xffeaebdd);
			p2DRender->SetFont(pOldFont);

			if (rect.PtInRect(point))
			{
				ClientToScreen(&point);
				ClientToScreen(&rect);

				CItemElem pItem;
				pItem.m_dwItemId = m_dwItemID[i];
				pItem.m_nItemNum = m_nItemNum[i];
				if (m_bBind)
					pItem.SetFlag(CItemElem::binds);

				g_WndMng.PutToolTip_Item(&pItem, point, &rect, APP_DAILY_REWARD);
			}
		}
	}

	CWndNeuz::OnDraw(p2DRender);
}

void CWndDailyGift::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndDailyGift::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndDailyGift::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			g_DPlay.SendHdr(PACKETTYPE_COLLECT_DAILY_GIFT);
			Destroy();
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndDailyGift::SetData(CAr& ar)
{
	ar >> m_nDay;
	ar >> m_dwItemID[0];
	ar >> m_nItemNum[0];
	ar >> m_bBind[0];
	ar >> m_dwItemID[1];
	ar >> m_nItemNum[1];
	ar >> m_bBind[1];

	for (int i = 0; i < 2; i++)
	{
		if (m_dwItemID[i] == NULL_ID)
			continue;

		ItemProp* pTestProp = prj.GetItemProp(m_dwItemID[i]);
		if (pTestProp)
			m_pItemTexture[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pTestProp->szIcon), 0xffff00ff);
	}

	CString str;
	str.Format("Welcome back! You can receive reward for day %d.", m_nDay + 1);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	pWndText->SetString(str);
}
#endif

#ifdef __MODEL_CHANGE
CWndLookChange::CWndLookChange()
{
	m_pItemDest = NULL;
	m_pItemSource = NULL;
	m_pRectDest = NULL;
	m_pRectSource = NULL;
	m_pRectResult = NULL;
	m_pStart = NULL;
}

void CWndLookChange::OnDestroy()
{
	if (m_pItemDest)
		m_pItemDest->SetExtra(0);
	if (m_pItemSource)
		m_pItemSource->SetExtra(0);
}

void CWndLookChange::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pRectDest = &(GetWndCtrl(WIDC_STATIC1)->rect);
	m_pRectSource = &(GetWndCtrl(WIDC_STATIC2)->rect);
	m_pRectResult = &(GetWndCtrl(WIDC_STATIC3)->rect);
	m_pStart = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pStart->EnableWindow(FALSE);

	g_WndMng.CreateApplet(APP_INVENTORY);

	MoveParentCenter();
}

BOOL CWndLookChange::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_LOOKCHANGE, 0, 0, pWndParent);
}

BOOL CWndLookChange::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (m_pRectDest->PtInRect(point))
	{
		CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (SetItem(pItem))
			return TRUE;
	}
	else if (m_pRectSource->PtInRect(point))
	{
		if (!m_pItemDest)
		{
			g_WndMng.PutDefinedString(TID_MMI_LOOKCHANGE03);
			return FALSE;
		}
		CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (SetItem(pItem))
			return TRUE;
	}
	return CWndNeuz::OnDropIcon(pShortcut, point);
}

void CWndLookChange::OnMouseWndSurface(CPoint point)
{
	if (m_pRectDest->PtInRect(point) && m_pItemDest)
	{
		PUT_TOOL_TIP(*m_pRectDest, point, m_pItemDest);
	}
	else if (m_pRectSource->PtInRect(point) && m_pItemSource)
	{
		PUT_TOOL_TIP(*m_pRectSource, point, m_pItemSource);
	}
	else if (m_pRectResult->PtInRect(point) && m_pItemSource && m_pItemDest)
	{
		CItemElem itemElem = *m_pItemDest;
		itemElem.SetLook(m_pItemSource->m_dwItemId);
		PUT_TOOL_TIP(*m_pRectResult, point, &itemElem);
	}
	CWndNeuz::OnMouseWndSurface(point);
}

void CWndLookChange::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemDest)
	{
		CTexture* pTex = m_pItemDest->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_pRectDest->TopLeft());
	}
	if (m_pItemSource)
	{
		CTexture* pTex = m_pItemSource->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_pRectSource->TopLeft());
	}
	if (m_pItemDest && m_pItemSource)
	{
		CTexture* pTex = m_pItemDest->GetTexture();
		if (pTex)
			pTex->Render(p2DRender, m_pRectResult->TopLeft());
	}
	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndLookChange::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		if (m_pItemSource && m_pItemDest)
		{
			g_DPlay.SendLookChange(m_pItemSource->m_dwObjId, m_pItemDest->m_dwObjId);
			m_pItemSource->SetExtra(0);
			m_pItemDest->SetExtra(0);
			m_pStart->EnableWindow(FALSE);
			m_pItemDest = NULL;
			m_pItemSource = NULL;
			Destroy();
		}
		break;
	}
	case WIDC_BUTTON2:
	{
		Destroy();
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndLookChange::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_pRectDest->PtInRect(point))
	{
		if (m_pItemDest)
			m_pItemDest->SetExtra(0);
		if (m_pItemSource)
			m_pItemSource->SetExtra(0);
		m_pItemSource = NULL;
		m_pItemDest = NULL;
		m_pStart->EnableWindow(FALSE);
	}
	if (m_pRectSource->PtInRect(point))
	{
		if (m_pItemSource)
			m_pItemSource->SetExtra(0);
		m_pItemSource = NULL;
		m_pStart->EnableWindow(FALSE);
	}
	CWndNeuz::OnRButtonUp(nFlags, point);
}

int CWndLookChange::IsValidItem(CItemElem* pItem)
{
	ItemProp* prop = pItem->GetProp();
	if (!prop)
		return TID_MMI_LOOKCHANGE01;
	if (pItem->IsChangedLook())
		return TID_MMI_LOOKCHANGE01;
	if (!IsUsableItem(pItem))
		return TID_MMI_LOOKCHANGE06;

	return -1;
}

BOOL CWndLookChange::SetItem(CItemElem* pItem)
{
	if (!IsUsableItem(pItem))
		return FALSE;

	CItemElem** ppItemTarget = NULL;
	int nMsg = IsValidItem(pItem);
	if (m_pItemDest)
	{
		if (pItem->m_dwItemId == m_pItemDest->m_dwItemId)
			return FALSE;
		if (nMsg == -1)
		{
			if (pItem->GetProp()->dwItemKind3 == IK3_SWD
				|| pItem->GetProp()->dwItemKind3 == IK3_AXE
				|| pItem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER
				|| pItem->GetProp()->dwItemKind3 == IK3_CHEERSTICK
				|| pItem->GetProp()->dwItemKind3 == IK3_WAND
				|| pItem->GetProp()->dwItemKind3 == IK3_STAFF
				|| pItem->GetProp()->dwItemKind3 == IK3_BOW
#ifdef __CROSSBOW
				|| pItem->GetProp()->dwItemKind3 == IK3_CROSSBOW
#endif // 
				|| pItem->GetProp()->dwItemKind3 == IK3_YOYO
				|| pItem->GetProp()->dwItemKind3 == IK3_SHIELD
				|| pItem->GetProp()->dwItemKind3 == IK3_HELMET
				|| pItem->GetProp()->dwItemKind3 == IK3_SUIT
				|| pItem->GetProp()->dwItemKind3 == IK3_GAUNTLET
				|| pItem->GetProp()->dwItemKind3 == IK3_BOOTS
				|| pItem->GetProp()->dwItemKind3 == IK3_HAT
				|| pItem->GetProp()->dwItemKind3 == IK3_CLOTH
				|| pItem->GetProp()->dwItemKind3 == IK3_GLOVE
				|| pItem->GetProp()->dwItemKind3 == IK3_SHOES
				|| pItem->GetProp()->dwItemKind3 == IK3_CLOAK
				|| pItem->GetProp()->dwItemKind3 == IK3_MASK
				|| pItem->GetProp()->dwItemKind3 == IK3_PET
				|| pItem->GetProp()->dwItemKind3 == IK3_MAGICBARUNA
				|| pItem->GetProp()->dwItemKind3 == IK3_ZEMBARUNA
#ifdef __ANI_WING
				|| pItem->GetProp()->dwID == IK3_ANI_WING
#endif // __ANI_WING			
				)
			{
				if ((pItem->GetProp()->dwItemKind3 == IK3_SHIELD || pItem->GetProp()->dwItemKind3 == IK3_MAGICBARUNA || pItem->GetProp()->dwItemKind3 == IK3_ZEMBARUNA) &&
					(m_pItemDest->GetProp()->dwItemKind3 == IK3_SHIELD || m_pItemDest->GetProp()->dwItemKind3 == IK3_MAGICBARUNA || m_pItemDest->GetProp()->dwItemKind3 == IK3_ZEMBARUNA))
				{
					ppItemTarget = &m_pItemSource;
					m_pStart->EnableWindow();
				}
				else if (pItem->GetProp()->dwItemKind3 == m_pItemDest->GetProp()->dwItemKind3 && (m_pItemDest->GetProp()->dwItemSex == pItem->GetProp()->dwItemSex || (m_pItemDest->GetProp()->dwItemSex == -1 || pItem->GetProp()->dwItemSex == -1)))
				{
					ppItemTarget = &m_pItemSource;
					m_pStart->EnableWindow();
				}
				else
				{
					nMsg = TID_MMI_LOOKCHANGE07;
				}
			}
			else
			{
				nMsg = TID_GAME_PET_IS_NOT_FEED;
			}
		}
		else
			nMsg = TID_MMI_LOOKCHANGE08;
	}
	else
	{
		if (nMsg == -1)
		{
			if (pItem->GetProp()->dwItemKind3 == IK3_SWD
				|| pItem->GetProp()->dwItemKind3 == IK3_AXE
				|| pItem->GetProp()->dwItemKind3 == IK3_KNUCKLEHAMMER
				|| pItem->GetProp()->dwItemKind3 == IK3_CHEERSTICK
				|| pItem->GetProp()->dwItemKind3 == IK3_WAND
				|| pItem->GetProp()->dwItemKind3 == IK3_STAFF
				|| pItem->GetProp()->dwItemKind3 == IK3_BOW
#ifdef __CROSSBOW
				|| pItem->GetProp()->dwItemKind3 == IK3_CROSSBOW
#endif // 
				|| pItem->GetProp()->dwItemKind3 == IK3_YOYO
				|| pItem->GetProp()->dwItemKind3 == IK3_SHIELD
				|| pItem->GetProp()->dwItemKind3 == IK3_HELMET
				|| pItem->GetProp()->dwItemKind3 == IK3_SUIT
				|| pItem->GetProp()->dwItemKind3 == IK3_GAUNTLET
				|| pItem->GetProp()->dwItemKind3 == IK3_BOOTS
				|| pItem->GetProp()->dwItemKind3 == IK3_HAT
				|| pItem->GetProp()->dwItemKind3 == IK3_CLOTH
				|| pItem->GetProp()->dwItemKind3 == IK3_GLOVE
				|| pItem->GetProp()->dwItemKind3 == IK3_SHOES
				|| pItem->GetProp()->dwItemKind3 == IK3_CLOAK
				|| pItem->GetProp()->dwItemKind3 == IK3_MASK
				|| pItem->GetProp()->dwItemKind3 == IK3_PET
				|| pItem->GetProp()->dwItemKind3 == IK3_MAGICBARUNA
				|| pItem->GetProp()->dwItemKind3 == IK3_ZEMBARUNA
				)
			{
				ppItemTarget = &m_pItemDest;
				if (m_pItemSource)
				{
					m_pItemSource->SetExtra(0);
					m_pItemSource = NULL;
				}
			}
			else
				nMsg = TID_GAME_PET_IS_NOT_FEED;
		}
	}
	if (ppItemTarget)
	{
		if (*ppItemTarget)
			(*ppItemTarget)->SetExtra(0);
		*ppItemTarget = pItem;
		(*ppItemTarget)->SetExtra(1);
	}
	if (nMsg != -1)
	{
		g_WndMng.PutDefinedString(nMsg);
		return FALSE;
	}
	return TRUE;
}
#endif

#ifdef __JOB_CHANGE
CWndNewChangeJob::CWndNewChangeJob()
{
	m_bNoCloseButton = TRUE;
	m_nJob = -1;
	m_pWndButtonOK = nullptr;

	for (int i = 0; i < 4; i++)
		m_pWndButton[i] = nullptr;
}

CWndNewChangeJob::~CWndNewChangeJob()
{
}

BOOL CWndNewChangeJob::Process()
{
	if (m_nJob == -1)
		m_pWndButtonOK->EnableWindow(FALSE);
	else
		m_pWndButtonOK->EnableWindow(TRUE);

	return TRUE;
}

void CWndNewChangeJob::OnDraw(C2DRender* p2DRender)
{

}

void CWndNewChangeJob::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);
	m_pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2);
	m_pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C3);
	m_pWndButton[3] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C4);
	m_pWndButtonOK = (CWndButton*)GetDlgItem(WIDC_OK);

	for (int i = 0; i < 4; i++)
		m_pWndButton[i]->SetToolTip(prj.m_aJob[i + 1].szName);

	MoveParentCenter();
}

BOOL CWndNewChangeJob::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGE_JOB_01, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndNewChangeJob::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndNewChangeJob::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndNewChangeJob::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		g_DPlay.SendJobChange(m_nJob);
		Destroy();
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			m_pWndButton[i]->SetCheck(FALSE);

			if (nID == m_pWndButton[i]->GetWndId())
			{
				m_nJob = i + 1;
				m_pWndButton[i]->SetCheck(TRUE);
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
CWndNewChangeJob2::CWndNewChangeJob2()
{
	m_bNoCloseButton = TRUE;
	m_nJob = -1;
	m_pWndButtonOK = nullptr;

	for (int i = 0; i < 2; i++)
		m_pWndButton[i] = nullptr;
}

CWndNewChangeJob2::~CWndNewChangeJob2()
{
}

BOOL CWndNewChangeJob2::Process()
{
	if (m_nJob == -1)
		m_pWndButtonOK->EnableWindow(FALSE);
	else
		m_pWndButtonOK->EnableWindow(TRUE);

	return TRUE;
}
void CWndNewChangeJob2::OnDraw(C2DRender* p2DRender)
{
}

void CWndNewChangeJob2::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);
	m_pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_BUTTON_C2);
	m_pWndButtonOK = (CWndButton*)GetDlgItem(WIDC_OK);

	const int nNewJob1 = GetNewJob();
	switch (nNewJob1)
	{
	case JOB_KNIGHT:
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotKnight.bmp"), TRUE);
		m_pWndButton[1]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBlad.bmp"), TRUE);
		break;
	case JOB_JESTER:
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotJes.bmp"), TRUE);
		m_pWndButton[1]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRan.bmp"), TRUE);
		break;
	case JOB_RINGMASTER:
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRingm.bmp"), TRUE);
		m_pWndButton[1]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBillfor.bmp"), TRUE);
		break;
	case JOB_PSYCHIKEEPER:
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotPsy.bmp"), TRUE);
		m_pWndButton[1]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElem.bmp"), TRUE);
		break;
	default:
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		m_pWndButton[0]->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		m_pWndButton[1]->EnableWindow(FALSE);
		m_pWndButton[1]->EnableWindow(FALSE);
		break;
	}

	if (nNewJob1 != -1)
	{
		for (int i = 0; i < 2; i++)
			m_pWndButton[i]->SetToolTip(prj.m_aJob[nNewJob1 + i].szName);
	}

	MoveParentCenter();
}

BOOL CWndNewChangeJob2::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGE_JOB_02, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndNewChangeJob2::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndNewChangeJob2::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndNewChangeJob2::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		g_DPlay.SendJobChange(m_nJob);
		Destroy();
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			m_pWndButton[i]->SetCheck(FALSE);

			if (nID == m_pWndButton[i]->GetWndId())
			{
				const int nNewJob = GetNewJob();
				m_nJob = nNewJob == -1 ? nNewJob : nNewJob + i;
				m_pWndButton[i]->SetCheck(TRUE);
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

int CWndNewChangeJob2::GetNewJob()
{
	if (g_pPlayer->GetLevel() == (MAX_JOB_LEVEL + MAX_EXP_LEVEL))
	{
		switch (g_pPlayer->GetJob())
		{
		case JOB_MERCENARY: return JOB_KNIGHT;
		case JOB_ACROBAT: return JOB_JESTER;
		case JOB_ASSIST: return JOB_RINGMASTER;
		case JOB_MAGICIAN: return JOB_PSYCHIKEEPER;
		default: return -1;
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
CWndNewChangeJob3::CWndNewChangeJob3()
{
	m_bNoCloseButton = TRUE;
	m_nJob = -1;
	m_pWndButtonOK = nullptr;
	m_pWndButton = nullptr;
}

CWndNewChangeJob3::~CWndNewChangeJob3()
{
}
BOOL CWndNewChangeJob3::Process()
{
	if (m_nJob == -1)
		m_pWndButtonOK->EnableWindow(FALSE);
	else
		m_pWndButtonOK->EnableWindow(TRUE);

	return TRUE;
}
void CWndNewChangeJob3::OnDraw(C2DRender* p2DRender)
{
}

void CWndNewChangeJob3::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_C1);
	m_pWndButtonOK = (CWndButton*)GetDlgItem(WIDC_OK);

	const int nNewJob = GetNewJob();
	switch (nNewJob)
	{
	case JOB_KNIGHT_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotKnight.bmp"), TRUE);
		break;
	case JOB_BLADE_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBlad.bmp"), TRUE);
		break;
	case JOB_JESTER_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotJes.bmp"), TRUE);
		break;
	case JOB_RANGER_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRan.bmp"), TRUE);
		break;
	case JOB_RINGMASTER_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRingm.bmp"), TRUE);
		break;
	case JOB_BILLPOSTER_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBillfor.bmp"), TRUE);
		break;
	case JOB_PSYCHIKEEPER_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotPsy.bmp"), TRUE);
		break;
	case JOB_ELEMENTOR_MASTER:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElem.bmp"), TRUE);
		break;
	case JOB_KNIGHT_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotKnight.bmp"), TRUE);
		break;
	case JOB_BLADE_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBlad.bmp"), TRUE);
		break;
	case JOB_JESTER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotJes.bmp"), TRUE);
		break;
	case JOB_RANGER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRan.bmp"), TRUE);
		break;
	case JOB_RINGMASTER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotRingm.bmp"), TRUE);
		break;
	case JOB_BILLPOSTER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotBillfor.bmp"), TRUE);
		break;
	case JOB_PSYCHIKEEPER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotPsy.bmp"), TRUE);
		break;
	case JOB_ELEMENTOR_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElem.bmp"), TRUE);
		break;
	case JOB_LORDTEMPLER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotLord.bmp"), TRUE);
		break;
	case JOB_STORMBLADE_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotStormb.bmp"), TRUE);
		break;
	case JOB_WINDLURKER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotWindI.bmp"), TRUE);
		break;
	case JOB_CRACKSHOOTER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotCracks.bmp"), TRUE);
		break;
	case JOB_FLORIST_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotFlor.bmp"), TRUE);
		break;
	case JOB_FORCEMASTER_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotForcem.bmp"), TRUE);
		break;
	case JOB_MENTALIST_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotMent.bmp"), TRUE);
		break;
	case JOB_ELEMENTORLORD_HERO:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotElel.bmp"), TRUE);
		break;
	default:
		m_pWndButton->SetTexture(D3DDEVICE, MakePath(DIR_THEME, "SlotVan.bmp"), TRUE);
		m_pWndButton->EnableWindow(FALSE);
		break;
	}

	if (nNewJob != -1)
		m_pWndButton->SetToolTip(prj.m_aJob[nNewJob].szName);

	MoveParentCenter();
}

BOOL CWndNewChangeJob3::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CHANGE_JOB_03, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndNewChangeJob3::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndNewChangeJob3::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndNewChangeJob3::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON_C1:
		m_nJob = GetNewJob();
		m_pWndButton->SetCheck(TRUE);
		break;
	case WIDC_OK:
		g_DPlay.SendJobChange(m_nJob);
		Destroy();
		break;
	default: break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

int CWndNewChangeJob3::GetNewJob()
{
	if (g_pPlayer->GetLevel() == MAX_GENERAL_LEVEL && g_pPlayer->IsPro() && g_pPlayer->GetExpPercent() >= 9999)
	{
		switch (g_pPlayer->GetJob())
		{
		case JOB_KNIGHT: return JOB_KNIGHT_MASTER; break;
		case JOB_BLADE: return JOB_BLADE_MASTER; break;
		case JOB_JESTER: return JOB_JESTER_MASTER; break;
		case JOB_RANGER: return JOB_RANGER_MASTER; break;
		case JOB_RINGMASTER: return JOB_RINGMASTER_MASTER; break;
		case JOB_BILLPOSTER: return JOB_BILLPOSTER_MASTER; break;
		case JOB_PSYCHIKEEPER: return JOB_PSYCHIKEEPER_MASTER; break;
		case JOB_ELEMENTOR: return JOB_ELEMENTOR_MASTER; break;
		default: return -1; break;
		}
	}
	if (g_pPlayer->GetLevel() == MAX_GENERAL_LEVEL && g_pPlayer->IsMaster() && g_pPlayer->GetExpPercent() >= 9999)
	{
		switch (g_pPlayer->GetJob())
		{
		case JOB_KNIGHT_MASTER: return JOB_KNIGHT_HERO; break;
		case JOB_BLADE_MASTER: return JOB_BLADE_HERO; break;
		case JOB_JESTER_MASTER: return JOB_JESTER_HERO; break;
		case JOB_RANGER_MASTER: return JOB_RANGER_HERO; break;
		case JOB_RINGMASTER_MASTER: return JOB_RINGMASTER_HERO; break;
		case JOB_BILLPOSTER_MASTER: return JOB_BILLPOSTER_HERO; break;
		case JOB_PSYCHIKEEPER_MASTER: return JOB_PSYCHIKEEPER_HERO; break;
		case JOB_ELEMENTOR_MASTER: return JOB_ELEMENTOR_HERO; break;
		default: return -1; break;
		}
	}
	if (g_pPlayer->GetLevel() == MAX_LEGEND_LEVEL && g_pPlayer->IsHero())
	{
		switch (g_pPlayer->GetJob())
		{
		case JOB_KNIGHT_HERO: return JOB_LORDTEMPLER_HERO; break;
		case JOB_BLADE_HERO: return JOB_STORMBLADE_HERO; break;
		case JOB_JESTER_HERO: return JOB_WINDLURKER_HERO; break;
		case JOB_RANGER_HERO: return JOB_CRACKSHOOTER_HERO; break;
		case JOB_RINGMASTER_HERO: return JOB_FLORIST_HERO; break;
		case JOB_BILLPOSTER_HERO: return JOB_FORCEMASTER_HERO; break;
		case JOB_PSYCHIKEEPER_HERO: return JOB_MENTALIST_HERO; break;
		case JOB_ELEMENTOR_HERO: return JOB_ELEMENTORLORD_HERO; break;
		default: return -1; break;
		}
	}
	return -1;
}
#endif

#ifdef __FL_RECYCLE
CWndRecycle::CWndRecycle()
	: m_pItemElem(NULL)
	, m_pEItemProp(NULL)
	, m_pText(NULL)
	, m_pTexture(NULL)
	, m_pButtonStart(NULL)
	, m_pButtonCheckbox(NULL)
	, m_pListBox(NULL)
	, m_pNameFilter(nullptr)
	, m_pWndComboFilter(nullptr)
{

}
CWndRecycle::~CWndRecycle()
{
	m_vecItems.clear();
}
void CWndRecycle::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	if (m_pTexture)
		m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));

}
void CWndRecycle::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pButtonStart = (CWndButton*)GetDlgItem(WIDC_START);
	m_pButtonStart->EnableWindow(FALSE);
	m_pButtonCheckbox = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	m_pText = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	m_pText->AddWndStyle(EBS_READONLY);

	m_pNameFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pNameFilter->SetStringBg("Search by item name..");

	m_pWndComboFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pWndComboFilter->AddWndStyle(EBS_READONLY);
	m_pWndComboFilter->AddString("Sort by value (Highest)");
	m_pWndComboFilter->AddString("Sort by value (Lowest)");
	m_pWndComboFilter->AddString("Sort by item level (Highest)");
	m_pWndComboFilter->AddString("Sort by item level (Lowest)");
	m_pWndComboFilter->SetCurSel(1);

	ApplyFilters();

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndRecycle::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_RECYCLE, 0, CPoint(0, 0), pWndParent);
}

void CWndRecycle::OnDestroy()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndRecycle::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRecycle::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRecycle::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRecycle::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndRecycle::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_START)
		SendToServer();

	if (message == EN_CHANGE)
	{
		if (nID == WIDC_EDIT1)
			ApplyFilters();
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1)
			ApplyFilters();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void  CWndRecycle::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rect = GetWndCtrl(WIDC_CHANGE)->rect;
	if (rect.PtInRect(point))
		ClearItem();
}

BOOL  CWndRecycle::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	SetItem(pTempElem);

	return TRUE;
}
BOOL CWndRecycle::SetItem(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return FALSE;

	auto it = prj.mapRecycle.find(pItemElem->m_dwItemId);
	if (it == prj.mapRecycle.end())
		return FALSE;

	if (m_pItemElem)
		m_pItemElem->SetExtra(0);
	m_pItemElem = pItemElem;
	m_pEItemProp = m_pItemElem->GetProp();
	m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
	m_pButtonStart->EnableWindow(TRUE);

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	if (m_pEItemProp)
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, m_pEItemProp->szIcon), 0xffff00ff);

	CString str;
	str.Format("Quantity: %d", it->second);
	m_pText->SetString(str);

	if (IsPossibleForAuto(pItemElem))
	{
		if (m_pButtonCheckbox->GetCheck())
			SendToServer();
	}
	else
		m_pButtonCheckbox->EnableWindow(FALSE);

	return TRUE;
}
BOOL CWndRecycle::IsPossibleForAuto(CItemElem* pItemElem)
{
	if (!pItemElem)
		return FALSE;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return FALSE;

	if (pProp->dwItemKind1 == IK1_WEAPON)
	{
		if (pProp->dwReferStat1 == WEAPON_UNIQUE || pProp->dwReferStat1 == WEAPON_ULTIMATE)
			return FALSE;
	}

	return TRUE;
}
void CWndRecycle::SendToServer()
{
	if (!m_pItemElem)
		return;

	m_pButtonStart->EnableWindow(FALSE);
	g_DPlay.SendRecycleRequest(m_pItemElem->m_dwObjId);
	ClearItem();

}
void CWndRecycle::ClearItem()
{
	if (m_pItemElem)
		m_pItemElem->SetExtra(0);

	m_pButtonStart->EnableWindow(FALSE);
	m_pButtonCheckbox->EnableWindow(TRUE);
	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;
	m_pText->SetString("");
}
bool SortByValueHighest(CWndRecycle::RECYCLE_ITEMS s1, CWndRecycle::RECYCLE_ITEMS s2)
{
	return (s1.nPrice > s2.nPrice);
}
bool SortByValueLowest(CWndRecycle::RECYCLE_ITEMS s1, CWndRecycle::RECYCLE_ITEMS s2)
{
	return (s1.nPrice < s2.nPrice);
}
bool SortByItemLevelHighest(CWndRecycle::RECYCLE_ITEMS s1, CWndRecycle::RECYCLE_ITEMS s2)
{
	return (s1.pItemProp->dwLimitLevel1 > s2.pItemProp->dwLimitLevel1);
}
bool SortByItemLevelLowest(CWndRecycle::RECYCLE_ITEMS s1, CWndRecycle::RECYCLE_ITEMS s2)
{
	return (s1.pItemProp->dwLimitLevel1 < s2.pItemProp->dwLimitLevel1);
}
void CWndRecycle::SortByValue(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByValueHighest);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByValueLowest);
}

void CWndRecycle::SortByItemLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemLevelHighest);
	else
		std::sort(m_vecItems.begin(), m_vecItems.end(), SortByItemLevelLowest);
}

void CWndRecycle::ApplyFilters()
{
	m_vecItems.clear();

	RECYCLE_ITEMS tmpStruct;
	for (auto& it : prj.mapRecycle)
	{
		ItemProp* pItemProp = prj.GetItemProp(it.first);
		if (!pItemProp)
			continue;

		tmpStruct.pItemProp = pItemProp;
		tmpStruct.nPrice = it.second;

		// Name Filter
		CString strFilter = m_pNameFilter->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = pItemProp->szName;
			if (!strName.IsEmpty())
			{
				if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
					continue;
			}
		}

		m_vecItems.push_back(tmpStruct);
	}

	// Sort
	switch (m_pWndComboFilter->GetCurSel())
	{
	case 0: SortByValue(TRUE); break;
	case 1: SortByValue(FALSE); break;
	case 2: SortByItemLevel(TRUE); break;
	case 3: SortByItemLevel(FALSE); break;
	default: break;
	}

	m_pListBox->ResetContent();
	for (auto& it : m_vecItems)
		m_pListBox->AddString("");
}
#endif

#ifdef __FL_VENDORS
CWndVendorItemSet::CWndVendorItemSet(CString str)
	: m_strNameVendor(str)
	, m_pItemElem(NULL)
	, m_pItemTexture(NULL)
{
}
CWndVendorItemSet::~CWndVendorItemSet()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}
void CWndVendorItemSet::OnDraw(C2DRender* p2DRender)
{
	if (m_pItemElem && m_pItemTexture)
	{
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_PIC_SLOT);
		m_pItemTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
	}
}
void CWndVendorItemSet::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_BUTTON);
	pWndOk->EnableWindow(FALSE);

	MoveParentCenter();
}
BOOL CWndVendorItemSet::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, 0, pWndParent);
}
BOOL CWndVendorItemSet::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndVendorItemSet::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndVendorItemSet::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON)
	{
		CWndVendor* pWndVendor = (CWndVendor*)g_WndMng.GetWndBase(APP_VENDOR_REVISION);
		if (pWndVendor && m_pItemElem && g_pPlayer && g_pPlayer->m_vtInfo.VendorIsVendor() && !g_pPlayer->m_vtInfo.IsVendorOpen())
		{
			g_DPlay.SendPVendorOpen((const char*)m_strNameVendor, FALSE, m_pItemElem->m_dwObjId);
			pWndVendor->Destroy();
		}

		Destroy();
	}
	else if (nID == WIDC_BUTTON1)
		Destroy();

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndVendorItemSet::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	SetItem(pItemElem);
	return TRUE;
}
void CWndVendorItemSet::SetItem(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem))
		return;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp || pProp->dwItemKind3 != IK3_VENDOR)
		return;

	DWORD dwSkillTime = pItemElem->m_dwKeepTime ? pItemElem->m_dwKeepTime : pProp->dwSkillTime;
	if (dwSkillTime == NULL || dwSkillTime == NULL_ID)
		return;

	if (m_pItemElem)
		m_pItemElem->SetExtra(0);
	m_pItemElem = pItemElem;
	m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
	m_pItemTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_BUTTON);
	pWndOk->EnableWindow(TRUE);
}

CWndVendorManagement::CWndVendorManagement()
{
}
CWndVendorManagement::~CWndVendorManagement()
{
}
void CWndVendorManagement::OnDraw(C2DRender* p2DRender)
{
}
void CWndVendorManagement::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	if (!CheckVendorBuff())
	{
		g_WndMng.PutString("You have not activated vendor.");
		Destroy();
	}

	MoveParentCenter();
}
BOOL CWndVendorManagement::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_VENDOR_MANAGE, 0, 0, pWndParent);
}
BOOL CWndVendorManagement::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndVendorManagement::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndVendorManagement::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (!CheckVendorBuff())
	{
		Destroy();
		return CWndNeuz::OnChildNotify(message, nID, pLResult);
	}

	switch (nID)
	{
	case WIDC_BUTTON: g_DPlay.SendVendorAction(1); Destroy(); break;
	case WIDC_BUTTON1: g_DPlay.SendVendorAction(2); break;
	default: break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndVendorManagement::CheckVendorBuff()
{
	if (!g_pPlayer || !g_pPlayer->HasBuffByIk3(IK3_VENDOR))
		return FALSE;

	return TRUE;
}
#endif


#ifdef __FL_NEW_BOXES
CWndNewBoxOpen::CWndNewBoxOpen(CItemElem* pItemElem, vector<NEWBOXELEM>* vec)
	: m_pWndListItems(NULL)
	, m_pWndButton(NULL)
{
	m_pItemElem = pItemElem;
	m_vecElements = vec;

	m_pItemElem->SetExtra(m_pItemElem->m_nItemNum);
	ItemProp* pProp = pItemElem->GetProp();
	if (pProp)
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
}

CWndNewBoxOpen::~CWndNewBoxOpen()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

void CWndNewBoxOpen::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	if (m_pTexture && m_pItemElem)
	{
		m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));

		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
		TCHAR szTemp[32];
		_stprintf(szTemp, "%d", m_pItemElem->GetExtra());
		CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
		p2DRender->TextOut(wndCtrl->rect.right - size.cx - 2, wndCtrl->rect.bottom - size.cy + 2, szTemp, 0xffeaebdd);
		p2DRender->SetFont(pOldFont);
	}

	// Render Items List
	CRect rectList = m_pWndListItems->GetWndRect();
	CRect rectItem(rectList.left + 3, rectList.top + 8, rectList.right - 19, rectList.top + 8 + m_pWndListItems->m_nFontHeight);

	CSize fontSize = m_p2DRender->m_pFont->GetTextExtent("1");
	CPoint pointTexture = CPoint(rectItem.left + 5, 0);
	CPoint pointItemName = CPoint(rectItem.left + 45, rectItem.bottom - rectItem.Height() / 2 - fontSize.cy / 2);
	CPoint pointItemAmount = CPoint(0, pointItemName.y);

	for (unsigned i = (unsigned)m_pWndListItems->m_wndScrollBar.GetScrollPos(); i < (unsigned)(m_vecElements->size()); ++i)
	{
		if (i >= (unsigned)(m_pWndListItems->m_wndScrollBar.GetScrollPage() + m_pWndListItems->m_wndScrollBar.GetScrollPos()))
			break;

		ItemProp* pProp = prj.GetItemProp(m_vecElements->at(i).dwReward);
		if (!pProp)
			break;

		if (i == m_pWndListItems->GetCurSel())
			p2DRender->RenderFillRect(rectItem, 0xFFbef5b0);

		p2DRender->RenderLine(CPoint(rectItem.left, rectItem.bottom - 1), CPoint(rectItem.right, rectItem.bottom - 1), 0xff000000);

		if (strlen(pProp->szIcon))
		{
			CTexture* pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
			if (pIcon)
			{
				pointTexture.y = rectItem.bottom - rectItem.Height() / 2 - pIcon->m_size.cy / 2;
				pIcon->Render(p2DRender, pointTexture);
			}
		}

		TCHAR szTemp[32];
		_stprintf(szTemp, "%d", m_vecElements->at(i).nItemCount);
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
		CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
		m_p2DRender->TextOut(pointTexture.x + 30 - size.cx, pointTexture.y + 30 - size.cy + 4, szTemp, 0xffeaebdd);
		p2DRender->SetFont(pOldFont);

		CRect rectToolTip = rectItem;
		rectToolTip.right = rectToolTip.left + 35;
		if (rectToolTip.PtInRect(m_ptMouse) && m_pWndOnMouseMove == m_pWndListItems)
		{
			CPoint pt2 = m_ptMouse;
			ClientToScreen(&pt2);
			ClientToScreen(&rectToolTip);
			CItemElem Item;
			Item.m_dwItemId = pProp->dwID;
			Item.m_nItemNum = m_vecElements->at(i).nItemCount;
			if (m_vecElements->at(i).bBinded)
				Item.SetFlag(CItemElem::binds);
			g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, GetWndId());
		}

		p2DRender->TextOut(pointItemName.x, pointItemName.y, pProp->szName, 0xff000000);

		rectItem.top += m_pWndListItems->m_nFontHeight;
		rectItem.bottom += m_pWndListItems->m_nFontHeight;
		pointItemName.y += m_pWndListItems->m_nFontHeight;
		pointItemAmount.y += m_pWndListItems->m_nFontHeight;
	}
}

void CWndNewBoxOpen::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pWndButton->EnableWindow(FALSE);

	m_pWndListItems->m_bOwnRender = TRUE;
	m_pWndListItems->m_nFontHeight = 40;
	for (auto& it : *m_vecElements)
		m_pWndListItems->AddString("");

	MoveParentCenter();
}

BOOL CWndNewBoxOpen::Initialize(CWndBase* pWndParent, DWORD)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_NEWBOX_OPEN, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndNewBoxOpen::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndNewBoxOpen::OnSize(UINT nType, int cx, int cy)
{
	int nPage = m_pWndListItems->GetWndRect().Height() / m_pWndListItems->m_nFontHeight;
	m_pWndListItems->m_wndScrollBar.SetScrollRange(0, m_vecElements->size());
	m_pWndListItems->m_wndScrollBar.SetScrollPage(nPage);

	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndNewBoxOpen::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_SELCHANGE)
	{
		if (nID == m_pWndListItems->GetWndId())
		{
			if (m_pWndListItems->GetCurSel() >= 0)
				m_pWndButton->EnableWindow(TRUE);
		}
	}
	else if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			g_DPlay.SendUseNewBox(m_pItemElem->m_dwObjId, m_pWndListItems->GetCurSel());
			int nNewExtra = m_pItemElem->GetExtra() - 1;
			if (nNewExtra > 0)
				m_pItemElem->SetExtra(nNewExtra);
			else
				Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndNewBoxOpen::OnDestroy()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}
void CWndNewBoxOpen::OnMouseWndSurface(CPoint point)
{
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	CRect DrawRect = wndCtrl->rect;

	if (DrawRect.PtInRect(point))
	{
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);

		g_WndMng.PutToolTip_Item(m_pItemElem, point2, &DrawRect, GetWndId());
	}
}
#endif

#ifdef __FL_SWITCH_EQ
CWndSwitchEquip::CWndSwitchEquip()
	: m_pWndListSwitches(NULL)
	, m_pButtonUse(NULL)
	, m_pButtonDelete(NULL)
	, m_pButtonModify(NULL)
	, m_pButtonCreate(NULL)
	, m_pWndNewSwitch(NULL)
	, m_pStaticShortcut(NULL)
	, m_pTextureDark(NULL)
	, m_pTextureNormal(NULL)
{

}
CWndSwitchEquip::~CWndSwitchEquip()
{
	SAFE_DELETE(m_pWndNewSwitch);
}

BOOL CWndSwitchEquip::Process()
{
	BOOL bSelected = IsSelected();
	m_pButtonUse->EnableWindow(bSelected);
	m_pButtonDelete->EnableWindow(bSelected);
	m_pButtonModify->EnableWindow(bSelected);

	if (g_pPlayer->m_Switches.GetTotalSwitches() >= MAX_SWITCHES)
		m_pButtonCreate->EnableWindow(FALSE);
	else
		m_pButtonCreate->EnableWindow(TRUE);

	if (m_Mover.m_pModel)
		m_Mover.m_pModel->FrameMove();

	return TRUE;
}
void CWndSwitchEquip::OnMouseWndSurface(CPoint point)
{
	EQ_SWITCH* pSwitch = NULL;
	if (IsSelected())
		pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_pWndListSwitches->GetCurSel());

	CRect DrawRect = m_pStaticShortcut->GetWndRect();
	if (DrawRect.PtInRect(point))
	{
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);

		CString str;
		if (pSwitch)
			str.Format("You can drag and move this shortcut to any taskbar slot!");
		else
			str.Format("You should select any of your presets to be able to add it's shortcut to your taskbar!");

		g_toolTip.PutToolTip(100000, str, DrawRect, point2);
		return;
	}

	if (!pSwitch)
		return;

	CItemElem* pItemElem = NULL;
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		if (!m_InvenRect[i].PtInRect(point))
			continue;

		DrawRect = m_InvenRect[i];
		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwItem[i]);

		if (i == PARTS_LWEAPON)
		{
			pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwItem[PARTS_SHIELD]);
			if (!pItemElem)
				pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwItem[PARTS_LWEAPON]);
		}

		break;
	}

	if (m_RectLooter.PtInRect(point))
	{
		DrawRect = m_RectLooter;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwLooterPet);
	}
	else if (m_RectPet.PtInRect(point))
	{
		DrawRect = m_RectPet;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwPet);
	}
	else if (m_RectRegen.PtInRect(point))
	{
		DrawRect = m_RectRegen;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwRegen);
	}

	if (pItemElem)
	{
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);
		g_WndMng.PutToolTip_Item(pItemElem, point2, &DrawRect, GetWndId());
	}
}
void CWndSwitchEquip::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM20);
	VIEWPORT tmp;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);
	g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)m_Mover.m_pModel, &m_Mover, lpFace, &tmp, GetWndId());

	EQ_SWITCH* pSwitch = NULL;
	if (IsSelected())
		pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_pWndListSwitches->GetCurSel());

	CItemElem* pItemElem = NULL;
	for (int i = 2; i < MAX_HUMAN_PARTS; i++)
	{
		if (!pSwitch)
			break;

		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwItem[i]);
		if (!pItemElem)
			continue;

		if (!pItemElem->m_pTexture)
			continue;

		CPoint cpAdd = CPoint(6, 6);
		FLOAT sx = 1.0f, sy = 1.0f;

		if (i >= PARTS_NECKLACE1 && i <= PARTS_EARRING2)
		{
			cpAdd = CPoint(0, 0);
			sx = 0.8f;
			sy = 0.8f;
		}
		else if (i >= PARTS_HAT && i <= PARTS_BOOTS)
		{
			cpAdd = CPoint(0, 0);
			sx = 0.9f;
			sy = 0.9f;
		}

		p2DRender->RenderTexture(m_InvenRect[i].TopLeft() + cpAdd, pItemElem->m_pTexture, 255, sx, sy);
	}

	if (pSwitch)
	{
		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwLooterPet);
		if (pItemElem && pItemElem->m_pTexture)
			p2DRender->RenderTexture(m_RectLooter.TopLeft() + CPoint(6, 6), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwPet);
		if (pItemElem && pItemElem->m_pTexture)
			p2DRender->RenderTexture(m_RectPet.TopLeft() + CPoint(6, 6), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwRegen);
		if (pItemElem && pItemElem->m_pTexture)
		{
			p2DRender->RenderTexture(m_RectRegen.TopLeft() + CPoint(6, 6), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", pItemElem->m_nItemNum);
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(m_RectRegen.TopLeft().x + 36 - size.cx, m_RectRegen.TopLeft().y + 36 - size.cy + 4, szTemp, 0xffeaebdd);
			p2DRender->SetFont(pOldFont);
		}

		p2DRender->RenderTexture(m_pStaticShortcut->GetWndRect().TopLeft(), m_pTextureNormal, 255, 1.0f, 1.0f);
	}
	else
		p2DRender->RenderTexture(m_pStaticShortcut->GetWndRect().TopLeft(), m_pTextureDark, 255, 1.0f, 1.0f);
}

void CWndSwitchEquip::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM3);
	LPWNDCTRL lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM4);
	LPWNDCTRL lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM5);
	LPWNDCTRL lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM6);
	LPWNDCTRL lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM7);
	LPWNDCTRL lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM8);
	LPWNDCTRL lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM9);
	LPWNDCTRL lpWndCtrl10 = GetWndCtrl(WIDC_CUSTOM10);

	m_InvenRect[6] = m_InvenRect[14] = lpWndCtrl1->rect;
	m_InvenRect[2] = m_InvenRect[15] = lpWndCtrl2->rect;
	m_InvenRect[4] = m_InvenRect[17] = lpWndCtrl3->rect;
	m_InvenRect[5] = m_InvenRect[18] = lpWndCtrl4->rect;

	m_InvenRect[10] = lpWndCtrl5->rect;
	m_InvenRect[9] = m_InvenRect[11] = lpWndCtrl6->rect;
	m_InvenRect[25] = lpWndCtrl7->rect;
	m_InvenRect[8] = lpWndCtrl8->rect;
	m_InvenRect[12] = lpWndCtrl9->rect;
	m_InvenRect[13] = lpWndCtrl10->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_CUSTOM11);
	lpWndCtrl2 = GetWndCtrl(WIDC_CUSTOM12);
	lpWndCtrl3 = GetWndCtrl(WIDC_CUSTOM13);
	lpWndCtrl4 = GetWndCtrl(WIDC_CUSTOM14);
	lpWndCtrl5 = GetWndCtrl(WIDC_CUSTOM15);
	lpWndCtrl6 = GetWndCtrl(WIDC_CUSTOM16);
	lpWndCtrl7 = GetWndCtrl(WIDC_CUSTOM17);
	lpWndCtrl8 = GetWndCtrl(WIDC_CUSTOM18);
	lpWndCtrl9 = GetWndCtrl(WIDC_CUSTOM19);

	m_InvenRect[20] = lpWndCtrl1->rect;
	m_InvenRect[22] = lpWndCtrl2->rect;
	m_InvenRect[19] = lpWndCtrl3->rect;
	m_InvenRect[23] = lpWndCtrl4->rect;
	m_InvenRect[21] = lpWndCtrl5->rect;

	m_InvenRect[26] = lpWndCtrl6->rect;
	m_InvenRect[27] = lpWndCtrl7->rect;
	m_InvenRect[28] = lpWndCtrl8->rect;
	m_InvenRect[29] = lpWndCtrl9->rect;

	lpWndCtrl1 = GetWndCtrl(WIDC_COST_NUM1);
	lpWndCtrl2 = GetWndCtrl(WIDC_COST_NUM2);
	lpWndCtrl3 = GetWndCtrl(WIDC_COST_NUM3);

	m_RectLooter = lpWndCtrl1->rect;
	m_RectPet = lpWndCtrl2->rect;
	m_RectRegen = lpWndCtrl3->rect;

	m_pButtonUse = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pButtonDelete = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	m_pButtonCreate = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	m_pButtonModify = (CWndButton*)GetDlgItem(WIDC_BUTTON4);

	m_pStaticShortcut = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_pStaticShortcut->EnableWindow(FALSE);
	m_pTextureDark = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_PvpDark.dds"), 0xffff00ff);
	m_pTextureNormal = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_Pvp.dds"), 0xffff00ff);

	m_pWndListSwitches = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	ResetListbox();

	UpdateModel();
	MoveParentCenter();
}

BOOL CWndSwitchEquip::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_EQ_SWITCH, 0, 0, pWndParent);
}
void CWndSwitchEquip::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void CWndSwitchEquip::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndNewSwitch)
		SAFE_DELETE(m_pWndNewSwitch);
}
void CWndSwitchEquip::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pStaticShortcut->GetWndRect().PtInRect(point) && IsSelected())
	{
		EQ_SWITCH* pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_pWndListSwitches->GetCurSel());
		if (pSwitch)
		{
			m_GlobalShortcut.m_pFromWnd = this;
			m_GlobalShortcut.m_dwShortcut = SHORTCUT_EQ_SWITCH;
			m_GlobalShortcut.m_dwType = 0;
			m_GlobalShortcut.m_dwIndex = 0;
			m_GlobalShortcut.m_dwData = 0;
			m_GlobalShortcut.m_dwId = m_pWndListSwitches->GetCurSel();
			m_GlobalShortcut.m_pTexture = m_pTextureNormal;
			_tcscpy(m_GlobalShortcut.m_szString, pSwitch->szSwitchName);
		}
	}
}

BOOL CWndSwitchEquip::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_LISTBOX1)
		{
			UpdateParts();
			UpdateStatic();
		}
	}
	else if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON3)
		{
			SAFE_DELETE(m_pWndNewSwitch);
			m_pWndNewSwitch = new CWndSwitchEquip_Create(-1);
			m_pWndNewSwitch->Initialize(this);
		}

		if (IsSelected())
		{
			if (nID == WIDC_BUTTON2)
				g_DPlay.SendSwitchDelete(m_pWndListSwitches->GetCurSel());
			else if (nID == WIDC_BUTTON1)
				g_DPlay.SendSwitchUse(m_pWndListSwitches->GetCurSel());
			else if (nID == WIDC_BUTTON4)
			{
				g_DPlay.SendSwitchUse(m_pWndListSwitches->GetCurSel());
				SAFE_DELETE(m_pWndNewSwitch);
				m_pWndNewSwitch = new CWndSwitchEquip_Create(m_pWndListSwitches->GetCurSel());
				m_pWndNewSwitch->Initialize(this);
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndSwitchEquip::ResetListbox()
{
	m_pWndListSwitches->ResetContent();
	for (auto& it : *g_pPlayer->m_Switches.GetSwitches())
		m_pWndListSwitches->AddString(it.szSwitchName);

	UpdateParts();
	UpdateStatic();
}
void CWndSwitchEquip::UpdateStatic()
{
	EQ_SWITCH* pSwitch = NULL;
	if (IsSelected())
		pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_pWndListSwitches->GetCurSel());

	CString str;
	if (pSwitch)
		str.Format("%s", pSwitch->szSwitchName);
	else
		str.Format("~");

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	pStatic->SetTitle(str);
}
void CWndSwitchEquip::UpdateModel()
{
	if (!g_pPlayer)
		return;

	SAFE_DELETE(m_Mover.m_pModel);
	int nMover = (g_pPlayer->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
	m_Mover.m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_Mover.m_pModel, OT_MOVER, nMover, MTI_STAND);
	UpdateParts();
	m_Mover.m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}
void CWndSwitchEquip::UpdateParts()
{
	if (!g_pPlayer || !m_Mover.m_pModel)
		return;

	m_Mover.ClearEquipInfo();

	EQ_SWITCH* pSwitch = NULL;
	if (IsSelected())
		pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_pWndListSwitches->GetCurSel());

	CItemElem* pItemElem = NULL;
	for (int i = 0; i < MAX_HUMAN_PARTS; i++)
	{
		if (!pSwitch)
			break;

		pItemElem = g_pPlayer->m_Inventory.GetAtId(pSwitch->dwItem[i]);
		if (!pItemElem)
			continue;

		m_Mover.m_aEquipInfo[i].dwId = pItemElem->m_dwItemId;
#ifdef __MODEL_CHANGE
		m_Mover.m_aEquipInfo[i].dwLook = pItemElem->GetLook();
#endif
	}

	m_Mover.m_fHairColorR = g_pPlayer->m_fHairColorR;
	m_Mover.m_fHairColorG = g_pPlayer->m_fHairColorG;
	m_Mover.m_fHairColorB = g_pPlayer->m_fHairColorB;

	CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, (CModelObject*)m_Mover.m_pModel, NULL);
}
BOOL CWndSwitchEquip::IsSelected()
{
	if (m_pWndListSwitches->GetCurSel() >= 0 && m_pWndListSwitches->GetCurSel() < g_pPlayer->m_Switches.GetSwitches()->size())
		return TRUE;

	return FALSE;
}
void CWndSwitchEquip::SetUsedPreset(int nSel)
{
	if (m_pWndListSwitches->GetCurSel() == nSel)
		return;

	m_pWndListSwitches->SetCurSel(nSel);
	UpdateParts();
	UpdateStatic();
}

CWndSwitchEquip_Create::CWndSwitchEquip_Create(int nExisting)
	: m_pEdit(NULL)
	, m_dwLooter(NULL_ID)
	, m_dwPet(NULL_ID)
	, m_dwRegen(NULL_ID)
{
	m_nSelected = nExisting;
}

CWndSwitchEquip_Create::~CWndSwitchEquip_Create()
{
}

BOOL CWndSwitchEquip_Create::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_EQ_SWITCH_CREATE, 0, CPoint(0, 0), pWndParent);
}

void CWndSwitchEquip_Create::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEdit->SetStringBg("Type name for new macros here..");

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	m_RectLooter = pStatic->GetWndRect();
	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	m_RectPet = pStatic->GetWndRect();
	pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_RectRegen = pStatic->GetWndRect();

	if (m_nSelected != -1)
	{
		if (m_nSelected < 0 || m_nSelected >= g_pPlayer->m_Switches.GetSwitches()->size())
			Destroy();
		else
		{
			EQ_SWITCH* pSwitch = &g_pPlayer->m_Switches.GetSwitches()->at(m_nSelected);
			m_pEdit->SetString(pSwitch->szSwitchName);
			m_dwLooter = pSwitch->dwLooterPet;
			m_dwPet = pSwitch->dwPet;
			m_dwRegen = pSwitch->dwRegen;
			m_pEdit->EnableWindow(FALSE);
			SetTitle("Edit Preset");
		}
	}

	g_WndMng.PutString("Your current equipment will be used for this preset!");

	MoveParentCenter();
}
void CWndSwitchEquip_Create::OnDraw(C2DRender* p2DRender)
{
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwLooter);
	if (pItemElem && pItemElem->m_pTexture)
		p2DRender->RenderTexture(m_RectLooter.TopLeft() + CPoint(7, 7), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

	pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwPet);
	if (pItemElem && pItemElem->m_pTexture)
		p2DRender->RenderTexture(m_RectPet.TopLeft() + CPoint(7, 7), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

	pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwRegen);
	if (pItemElem && pItemElem->m_pTexture)
	{
		p2DRender->RenderTexture(m_RectRegen.TopLeft() + CPoint(7, 7), pItemElem->m_pTexture, 255, 1.0f, 1.0f);

		TCHAR szTemp[32];
		_stprintf(szTemp, "%d", pItemElem->m_nItemNum);
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
		CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
		m_p2DRender->TextOut(m_RectRegen.TopLeft().x + 37 - size.cx, m_RectRegen.TopLeft().y + 37 - size.cy + 4, szTemp, 0xffeaebdd);
		p2DRender->SetFont(pOldFont);
	}

	CWndNeuz::OnDraw(p2DRender);
}
void CWndSwitchEquip_Create::OnMouseWndSurface(CPoint point)
{
	CItemElem* pItemElem = NULL;
	CRect DrawRect;

	if (m_RectLooter.PtInRect(point))
	{
		DrawRect = m_RectLooter;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwLooter);
	}

	if (m_RectPet.PtInRect(point))
	{
		DrawRect = m_RectPet;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwPet);
	}

	if (m_RectRegen.PtInRect(point))
	{
		DrawRect = m_RectRegen;
		pItemElem = g_pPlayer->m_Inventory.GetAtId(m_dwRegen);
	}

	if (pItemElem)
	{
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);
		g_WndMng.PutToolTip_Item(pItemElem, point2, &DrawRect, GetWndId());
	}
}
void CWndSwitchEquip_Create::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_RectLooter.PtInRect(point))
		m_dwLooter = NULL_ID;

	if (m_RectPet.PtInRect(point))
		m_dwPet = NULL_ID;

	if (m_RectRegen.PtInRect(point))
		m_dwRegen = NULL_ID;
}
BOOL CWndSwitchEquip_Create::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndSwitchEquip_Create::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
		{
			if (!g_pPlayer->m_Switches.AddSwitch(m_nSelected, m_pEdit->GetString(), m_dwLooter, m_dwPet, m_dwRegen, FALSE))
			{
				g_WndMng.PutString("Name should not contain spaces and special symbols. Number of letters: 4-16.");
				Clear();
			}
			else
			{
				g_DPlay.SendSwitchAdd(m_nSelected, (LPSTR)(LPCSTR)m_pEdit->GetString(), m_dwLooter, m_dwPet, m_dwRegen);
				Destroy();
			}
		}
		else if (nID == WIDC_BUTTON2)
			Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndSwitchEquip_Create::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!pWndFrame)
		return FALSE;

	if (pWndFrame->GetWndId() != APP_INVENTORY)
	{
		SetForbid(TRUE);
		return FALSE;
	}

	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!pTempElem)
		return FALSE;

	ItemProp* pProp = pTempElem->GetProp();
	if (!pProp)
		return FALSE;

	if (pProp->dwItemKind3 == IK3_PET)
		m_dwLooter = pTempElem->m_dwObjId;
	else if (pProp->dwItemKind3 == IK3_EGG)
		m_dwPet = pTempElem->m_dwObjId;
	else if (pProp->dwItemKind2 == IK2_FOOD
		|| pProp->dwItemKind2 == IK2_POTION
		|| pProp->dwItemKind2 == IK2_REFRESHER)
		m_dwRegen = pTempElem->m_dwObjId;
	else
		return FALSE;

	return TRUE;
}
void CWndSwitchEquip_Create::Clear()
{
	if (m_nSelected != -1)
		m_pEdit->SetString("");

	m_dwLooter = m_dwPet = m_dwRegen = NULL_ID;
}
void CWndSwitchEquip_Create::FindAndRemove(DWORD dwObjId)
{
	if (m_dwLooter == dwObjId)
		m_dwLooter = NULL_ID;

	if (m_dwPet == dwObjId)
		m_dwPet = NULL_ID;

	if (m_dwRegen == dwObjId)
		m_dwRegen = NULL_ID;
}
#endif

#ifdef __AI_SYSTEM
int partySkill[] = { 6,8 };
CWndRefresher::CWndRefresher()
{
	m_dwsfxtime = -1;
	bStart = FALSE;
	for (int i = 0; i < 4; i++)
	{
		m_pItemElem[i] = NULL;
		m_pTexture[i] = NULL;
		m_dwitemid[i] = NULL;

	}

}
CWndRefresher::~CWndRefresher()
{

}


BOOL CWndRefresher::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_REFRESHER, 0, CPoint(0, 0), pWndParent);
}


BOOL CWndRefresher::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndRefresher::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndRefresher::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndRefresher::OnLButtonDown(UINT nFlags, CPoint point)
{
}
void CWndRefresher::OnDraw(C2DRender* p2DRender)
{
	//if (bStart)
	//{
	//	CString str;
	//	str.Format("杀怪：%d",g_pPlayer->n_KillNumber);
	//	p2DRender->TextOut(16,164,1,1,str,0xffff0000);
	//}
	int j = 0;
	for (unsigned int i = 0; i < Skill.size(); i++)
	{
		LPSKILL lpSkill;
		lpSkill = g_pPlayer->GetSkill(0, Skill[i]);
		ItemProp* pSkillProp = prj.GetSkillProp(lpSkill->dwSkill);
		if (pSkillProp)
		{
			CTexture* pIcon = NULL;
			pIcon = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pSkillProp->szIcon), 0xffff00ff);
			pIcon->Render(p2DRender, CPoint(m_rect[j].left + 11, m_rect[j].top + 1), 255);
			j++;
		}
	}
	for (unsigned int i = 0; i < BuffItem.size(); i++)
	{
		ItemProp* pItemProp = prj.GetItemProp(BuffItem[i]);
		if (pItemProp)
		{
			CTexture* pIcon = NULL;
			pIcon = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
			pIcon->Render(p2DRender, CPoint(m_rect[j].left + 11, m_rect[j].top + 1), 255);
			j++;
		}
	}
	//剧团
	for (int i = 0; i < 2; i++)
	{
		ItemProp* pItemProp = prj.GetPartySkill(partySkill[i]);

		if (pItemProp)
		{
			CTexture* pIcon = NULL;
			pIcon = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pItemProp->szIcon), 0xffff00ff, FALSE);
			pIcon->Render(p2DRender, CPoint(m_rect[j].left + 11, m_rect[j].top + 1), 255);
			j++;
		}
		if (g_Party.m_nSizeofMember != 0 && g_Party.IsLeader(g_pPlayer->m_idPlayer))
		{
			if (g_Party.GetLevel() - pItemProp->dwReqDisLV >= 0)
			{
				CWndButton* pButtn1 = (CWndButton*)GetDlgItem(400 + i);
				pButtn1->EnableWindow(TRUE);
			}

		}
		else
		{
			CWndButton* pButtn1 = (CWndButton*)GetDlgItem(400 + i);
			pButtn1->SetCheck(FALSE);
			pButtn1->EnableWindow(FALSE);

		}
	}


}
HRESULT CWndRefresher::RestoreDeviceObjects()
{
	CWndNeuz::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndRefresher::InvalidateDeviceObjects()
{
	CWndNeuz::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndRefresher::DeleteDeviceObjects()
{
	CWndNeuz::DeleteDeviceObjects();
	InvalidateDeviceObjects();
	return S_OK;
}
void CWndRefresher::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_START);
	CWndButton* pButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_STOP);
	// pButton->EnableWindow(FALSE);
	pButton2->EnableWindow(FALSE);

	///////////////插入药品////////////////////
	CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
	CWndComboBox* pCombo1 = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	CWndComboBox* pCombo2 = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
	int nIndex = -1;
	CString str;
	pCombo->ResetContent();
	nIndex = pCombo->AddString("select Hp");
	pCombo->SetItemData(nIndex, 0);
	pCombo->SetCurSel(nIndex);
	pCombo1->ResetContent();
	nIndex = pCombo1->AddString("select Mp");
	pCombo1->SetItemData(nIndex, 0);
	pCombo1->SetCurSel(nIndex);
	pCombo2->ResetContent();
	nIndex = pCombo2->AddString("select Fp");
	pCombo2->SetItemData(nIndex, 0);
	pCombo2->SetCurSel(nIndex);
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(i);
		if (!pItemElem)
			continue;
		ItemProp* pItemProp = pItemElem->GetProp();
		if (!pItemProp)
			continue;
		if (pItemProp->dwItemKind2 != IK2_FOOD && pItemProp->dwItemKind2 != IK2_REFRESHER && pItemProp->dwItemKind2 != IK2_POTION)
			continue;
		BOOL bCunZai = FALSE;
		if (pItemProp->dwDestParam[0] == DST_HP)
		{
			for (int i = 0; i < pCombo->GetListBoxSize(); i++)
			{
				if (pCombo->GetItemData(i) == pItemProp->dwID)
				{
					bCunZai = TRUE;
					break;
				}
			}
			if (bCunZai == FALSE)
			{
				str.Format("%s", pItemProp->szName);
				nIndex = pCombo->AddString(str);
				pCombo->SetItemData(nIndex, pItemProp->dwID);
				pCombo->SetCurSel(nIndex);
				m_dwitemid[0] = pItemProp->dwID;
			}

		}
		else if (pItemProp->dwDestParam[0] == DST_MP)
		{
			for (int i = 0; i < pCombo1->GetListBoxSize(); i++)
			{
				if (pCombo1->GetItemData(i) == pItemProp->dwID)
				{
					bCunZai = TRUE;
					break;
				}
			}
			if (bCunZai == FALSE)
			{
				str.Format("%s", pItemProp->szName);
				nIndex = pCombo1->AddString(str);
				pCombo1->SetItemData(nIndex, pItemProp->dwID);
				pCombo1->SetCurSel(nIndex);
				m_dwitemid[1] = pItemProp->dwID;
			}
		}
		else if (pItemProp->dwDestParam[0] == DST_FP)
		{
			for (int i = 0; i < pCombo2->GetListBoxSize(); i++)
			{
				if (pCombo2->GetItemData(i) == pItemProp->dwID)
				{
					bCunZai = TRUE;
					break;
				}
			}
			if (bCunZai == FALSE)
			{
				str.Format("%s", pItemProp->szName);
				nIndex = pCombo2->AddString(str);
				pCombo2->SetItemData(nIndex, pItemProp->dwID);
				pCombo2->SetCurSel(nIndex);
				m_dwitemid[2] = pItemProp->dwID;
			}
		}
	}





	///////////////////////////////////////////////////////////////////////
	CWndButton* pWndButton;
	pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO);
	pWndButton->SetCheck(FALSE);
	pWndButton = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	pWndButton->SetCheck(TRUE);//默认选择无限范围

	CWndEdit* LVmin = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndEdit* LVmax = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	LVmin->SetString("1");
	LVmax->SetString("999");
	///////////////////////////////////////////////////////////////////////////////////////////
	if (g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
	{
		CWndButton* pWndButtonRang = new CWndButton;
		pWndButtonRang->Create("bow double hit", WBS_CHECK, CRect(75, 145, 145, 161), this, 300);
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	CWndButton* pWndButtonRang = new CWndButton;
	pWndButtonRang->Create("check ascription", WBS_CHECK, CRect(5, 145, 75, 161), this, 301);



	CPoint pt;
	int index = 0;
	//CWndButton* pWndButtonSkill =  new CWndButton;
	for (int x = 0; x < 10; x++)//技能框
	{
		for (int y = 0; y < 6; y++)
		{
			pt = CPoint(x + 155, y + 8);

			pt.x += (x * 42);
			pt.y += (y * 32);

			SetRect(m_rect[index], pt.x, pt.y, pt.x + 32, pt.y + 32);
			index++;
		}
	}
	index = 0;
	Skill.clear();
	for (int i = 0; i < MAX_SKILL_JOB; i++)
	{
		LPSKILL lpSkill;
		lpSkill = &g_pPlayer->m_aJobSkill[i];

		if (lpSkill->dwSkill != NULL_ID && lpSkill->dwLevel > 0)
		{
			ItemProp* pSkillProp = prj.m_aPropSkill.GetAt(lpSkill->dwSkill);
			if (pSkillProp && pSkillProp->nEvildoing == 1 && pSkillProp->dwReferValue1 > 0)
			{
				CWndButton* pWndButtonSkill = new CWndButton;
				Skill.push_back(i);
				pWndButtonSkill->Create("", WBS_CHECK, m_rect[index], this, 100 + index);
				index++;
			}
			else if (pSkillProp && pSkillProp->dwID == SI_RIN_SUP_MERKABAHANZELRUSHA)
			{
				CWndButton* pWndButtonSkill = new CWndButton;
				Skill.push_back(i);
				pWndButtonSkill->Create("", WBS_CHECK, m_rect[index], this, 100 + index);
				index++;
			}
		}

	}

	BuffItem.clear();
	int index2 = 0;
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt(i);
		if (pItemElem && IsBuffItem(pItemElem->m_dwItemId))
		{
			BuffItem.push_back(pItemElem->m_dwItemId);
			CWndButton* pWndButtonSkill = new CWndButton;
			pWndButtonSkill->Create("", WBS_CHECK, m_rect[index + index2], this, 200 + index2);
			index2++;
		}
	}
	//剧团技能
	for (int i = 0; i < 2; i++)
	{
		CWndButton* pWndButtonSkill = new CWndButton;
		pWndButtonSkill->Create("", WBS_CHECK, m_rect[index + index2 + i], this, 400 + i);
		pWndButtonSkill->EnableWindow(FALSE);
	}

	CWndButton* pWndButtonF1 = new CWndButton;
	pWndButtonF1->Create("F1", WBS_CHECK, m_rect[index + index2 + 2], this, 500);
	//pWndButtonF1->EnableWindow(FALSE);
	int n = (index + index2 + 2) % 6 == 0 ? 0 : 1;
	SetWndRect(CRect(5, 5, 176 + int((index + index2 + 2) / 6 + n) * 42, 240));



	MoveParentCenter();
}
BOOL CWndRefresher::IsBuffItem(DWORD itemId)
{
	switch (itemId)
	{
	case II_CHR_SYS_SCR_UPCUTSTONE:
	case II_SYS_SYS_SCR_ICEATKSPEED:
	case II_SYS_SYS_SCR_VELOCIJUMP:
	case II_CHR_SYS_SCR_ACTIVITION:
	case II_SYS_SYS_SCR_AMPESS:
		return TRUE;
		break;
	default:
		return FALSE;
		break;

	}
}
BOOL CWndRefresher::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	//LPWNDCTRL wndCtrl[4];
	//wndCtrl[0] = GetWndCtrl( WIDC_CUSTOM7 );
	//wndCtrl[1] = GetWndCtrl( WIDC_CUSTOM1 );
	//wndCtrl[2] = GetWndCtrl( WIDC_CUSTOM2 ); 
	//wndCtrl[3] = GetWndCtrl( WIDC_CUSTOM8); 
	//for( int i = 0; i < 4; i++ )
	//{
	//	if( !wndCtrl[i] )
	//	{
	//		continue;
	//	}
	//	if( wndCtrl[i]->rect.PtInRect( point ))
	//	{

	//		CItemElem* pTempElem;
	//		CItemElem* pItemBase = g_pPlayer->GetItemId( pShortcut->m_dwId );
	//		pTempElem =  pItemBase;
	//		ItemProp* pItemProp = pTempElem->GetProp();


	//		if( pItemProp->dwItemKind2 == IK2_FOOD || pItemProp->dwItemKind2 == IK2_POTION || pItemProp->dwItemKind2 == IK2_REFRESHER || pItemProp->dwUseable == 1)
	//		{
	//			if( m_pItemElem[i])
	//			{
	//				m_pItemElem[i]->SetExtra(0);
	//				m_pItemElem[i] = NULL;
	//				m_dwitemid[i]=NULL;
	//			}
	//			m_pItemElem[i] = pTempElem;
	//			m_pTexture[i] = CWndBase::m_textureMng.AddTexture( D3DDEVICE, MakePath( DIR_ITEM, m_pItemElem[i]->GetProp()->szIcon ), 0xffff00ff );
	//			if( m_pTexture[i])
	//			{
	//				m_dwitemid[i]=m_pItemElem[i]->m_dwItemId;
	//			//	m_pItemElem[i]->SetExtra(1);
	//			}
	//		}else{
	//			return FALSE;
	//		}

	//	}
	//}
	return TRUE;
}
DWORD timeEndInter = 0;
DWORD s_CoolTime = 0;
DWORD nPowtime = 0;
BOOL CWndRefresher::Process()
{
	if (g_pPlayer->GetHitPoint() == 0)
	{
		return FALSE;
	}
	////////////////吃药////////////////////////////
	CWndButton* hp = (CWndButton*)GetDlgItem(WIDC_HP);
	CWndButton* mp = (CWndButton*)GetDlgItem(WIDC_MP);
	CWndButton* fp = (CWndButton*)GetDlgItem(WIDC_FP);
	//for( int i = 0; i < 3; ++i )
	{
		DWORD dwId = g_pPlayer->m_Inventory.Find(m_dwitemid[0]);
		if (bStart && dwId != NULL_ID && hp->GetCheck())
		{
			CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);

			if (pItemBase && (((FLOAT)g_pPlayer->GetMaxHitPoint() / g_pPlayer->GetHitPoint()) > 2.0F || pItemBase->GetProp()->nAdjParamVal1 + g_pPlayer->GetHitPoint() <= g_pPlayer->GetMaxHitPoint()))
			{

				if (g_pPlayer->m_cooltimeMgr.CanUse(g_pPlayer->m_cooltimeMgr.GetGroup(pItemBase->GetProp())))
				{
					g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
				}
			}
		}
		dwId = g_pPlayer->m_Inventory.Find(m_dwitemid[1]);
		if (bStart && dwId != NULL_ID && mp->GetCheck())
		{
			CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
			//if( ((FLOAT)g_pPlayer->GetMaxManaPoint() / g_pPlayer->GetManaPoint() ) > 2.0F || m_pItemElem[1]->GetProp()->nAdjParamVal1 + g_pPlayer->GetManaPoint() <= g_pPlayer->GetMaxManaPoint() )
			if (pItemBase && g_pPlayer->GetManaPoint() < 500 && g_pPlayer->GetManaPoint() < g_pPlayer->GetMaxManaPoint())
			{

				if (g_tmCurrent - m_CoolTime > 2000)
				{
					g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
					m_CoolTime = g_tmCurrent;

				}
			}
		}
		dwId = g_pPlayer->m_Inventory.Find(m_dwitemid[2]);
		if (bStart && dwId != NULL_ID && fp->GetCheck())
		{
			CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
			//if( ((FLOAT)g_pPlayer->GetMaxFatiguePoint() / g_pPlayer->GetFatiguePoint() ) > 2.0F || m_pItemElem[2]->GetProp()->nAdjParamVal1 + g_pPlayer->GetFatiguePoint() <= g_pPlayer->GetMaxFatiguePoint() )
			if (pItemBase && g_pPlayer->GetFatiguePoint() < 200 && g_pPlayer->GetFatiguePoint() < g_pPlayer->GetMaxFatiguePoint())
			{
				if (g_tmCurrent - m_CoolTime > 2000)
				{
					g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
					m_CoolTime = g_tmCurrent;

				}
			}
		}
		//dwId = g_pPlayer->m_Inventory.Find( m_dwitemid[3] );
		//if( bStart && dwId != NULL_ID && Ex->GetCheck() )
		//{
		//	CItemElem* pItemBase = g_pPlayer->GetItemId(dwId);
		//	if(pItemBase && !g_pPlayer->HasBuff( BUFF_ITEM, pItemBase->m_dwItemId ))
		//	{
		//		if( g_tmCurrent-m_CoolTime>2000)
		//		{
		//			g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pItemBase->m_dwObjId ), NULL_ID, pItemBase->GetProp()->dwParts );
		//			m_CoolTime = g_tmCurrent;
		//		}
		//	}

		//}


	}
	////////////////////////////以下是自动打怪//////////////////////////
	if (g_pPlayer->IsFly())
	{
		return FALSE;
	}


	CWndButton* fanwei = (CWndButton*)GetDlgItem(WIDC_RADIO);
	CWndButton* dafanwei = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	//取得怪物等级范围
	CWndEdit* LVmin = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	CWndEdit* LVmax = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndButton* F1 = (CWndButton*)GetDlgItem(500);
	int MinLv = _ttoi(LVmin->GetString());
	int MaxLv = _ttoi(LVmax->GetString());
	if (MinLv > 999 || MinLv < 1 || MaxLv > 999 || MaxLv < 1)
	{
		return FALSE;
	}
	if (bStart)
	{
		//if(fanwei->GetCheck())
	//		CreateSfx(g_Neuz.m_pd3dDevice,XI_NEIGUA,g_pPlayer->GetPos(),g_pPlayer->GetId());
		CWorld* pWorld = g_WorldMng.Get();
		float fDist;
		CObj* pSelectOld = pWorld->GetObjFocus();
		CObj* pSelectNEW = NULL;
		CMover* pMover = (CMover*)pSelectOld;
		if (pSelectOld && pSelectOld->GetType() != OT_MOVER)
		{
			return FALSE;
			//bStart=false;
			//CWndButton* pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_START);
			//pButton1->EnableWindow(TRUE);
		}
		if (pMover && pMover->GetHitPoint() > 0)
		{

			CWndButton* guishu = (CWndButton*)GetDlgItem(301);//判断归属
			if (guishu->GetCheck() && pMover->m_idAttacker != NULL_ID && pMover->m_idAttacker != g_pPlayer->GetId())
			{
				CMover* pTmp = prj.GetMover(pMover->m_idAttacker);
				if (g_pPlayer->m_idparty > 0 && pTmp && pTmp->GetPartyId() == g_pPlayer->m_idparty)
				{

				}
				else
				{
					pWorld->SetObjFocus(NULL);
					return FALSE;
				}
			}
			D3DXVECTOR3 vStart = g_pPlayer->GetPos();			vStart.y += 0.5f;
			D3DXVECTOR3 vEnd = pMover->GetPos();	vEnd.y += 0.5f;
			if (g_pPlayer->GetWorld()->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE))
			{
				if (g_tmCurrent - timeEndInter > SEC(5))
				{
					pWorld->SetObjFocus(NULL);
					return FALSE;

				}

			}
			else
			{
				timeEndInter = g_tmCurrent;
				//判断连续技是否存在
				CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
				if (pTaskBar->m_aSlotQueue[0].IsEmpty())
				{
					if (g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
					{
						CWndButton* range = (CWndButton*)GetDlgItem(300);
						if (range->GetCheck() && g_pPlayer->GetWeaponItem()->GetProp()->dwItemKind3 == IK3_BOW)
						{
							if (F1->GetCheck())
							{
								LPSHORTCUT lpShortcut = &pTaskBar->m_paSlotItem[0];
								if (lpShortcut->IsEmpty() || lpShortcut->m_dwShortcut != SHORTCUT_SKILL)
								{
									g_pPlayer->CMD_SetRangeAttack(pMover->GetId(), 4);
								}
								else
								{
									//LPSKILL pSkill = g_pPlayer->GetSkill(0,lpShortcut->m_dwIndex);
									//ItemProp* pSkillProp = prj.GetSkillProp( pSkill->dwSkill );
									if (g_pPlayer->GetReuseDelay(lpShortcut->m_dwId) <= 0)
									{
										g_pPlayer->CMD_SetUseSkill(pMover->GetId(), lpShortcut->m_dwId);
									}
									else
									{
										g_pPlayer->CMD_SetRangeAttack(pMover->GetId(), 4);
									}
									//g_WndMng.UseSkillShortCut(lpShortcut->m_dwType ,lpShortcut->m_dwIndex);

								}

								//g_WndMng.ObjectExecutor( lpShortcut );
							}
							else
								g_pPlayer->CMD_SetRangeAttack(pMover->GetId(), 4);
						}
						else
						{
							if (F1->GetCheck())
							{
								LPSHORTCUT lpShortcut = &pTaskBar->m_paSlotItem[0];
								if (lpShortcut->IsEmpty() || lpShortcut->m_dwShortcut != SHORTCUT_SKILL)
								{
									((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
								}
								else
								{
									//LPSKILL pSkill = g_pPlayer->GetSkill(0,lpShortcut->m_dwIndex);
									//ItemProp* pSkillProp = prj.GetSkillProp( pSkill->dwSkill );
									if (g_pPlayer->GetReuseDelay(lpShortcut->m_dwId) <= 0)
									{
										g_pPlayer->CMD_SetUseSkill(pMover->GetId(), lpShortcut->m_dwId);
									}
									else
									{
										((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
									}
									//g_WndMng.UseSkillShortCut(lpShortcut->m_dwType ,lpShortcut->m_dwIndex);

								}

								//g_WndMng.ObjectExecutor( lpShortcut );
							}
							else
								((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
						}
					}
					else
					{
						if (F1->GetCheck())
						{
							LPSHORTCUT lpShortcut = &pTaskBar->m_paSlotItem[0];
							if (lpShortcut->IsEmpty() || lpShortcut->m_dwShortcut != SHORTCUT_SKILL)
							{
								((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
							}
							else
							{
								if (g_pPlayer->GetReuseDelay(lpShortcut->m_dwId) <= 0)
								{
									//g_pPlayer->CMD_SetUseSkill( pMover->m_objid, lpShortcut->m_dwId );
									g_WndMng.ObjectExecutor(lpShortcut);
								}
							}

						}
						else
							((CWndWorld*)g_WndMng.m_pWndWorld)->m_bAutoAttack = TRUE;
					}
				}
				else
					g_WndMng.m_pWndWorld->UseSkill();
			}

		}
		else
		{
			//寻怪之前判断辅助状态
			int j = 0;
			for (unsigned int i = 0; i < Skill.size(); i++)
			{
				LPSKILL pSkill = g_pPlayer->GetSkill(0, Skill[i]);
				ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
				if (pSkillProp)
				{
					CWndButton* pButton = (CWndButton*)GetDlgItem(100 + j);
					if (pButton->GetCheck() && !g_pPlayer->HasBuff(BUFF_SKILL, pSkillProp->dwID) &&
						g_pPlayer->GetReuseDelay(Skill[i]) <= 0 && !g_pPlayer->HasBuff(BUFF_SKILL, SI_BLD_SUP_BERSERK)
						)
					{
						if (g_tmCurrent - m_CoolTime > 2000)
						{
							g_pPlayer->CMD_SetUseSkill(g_pPlayer->GetId(), Skill[i]);
							m_CoolTime = g_tmCurrent;
							//使用技能后暂停2秒寻怪
							if (pSkillProp->dwID == SI_RIN_SUP_MERKABAHANZELRUSHA)
							{
								s_CoolTime = -1;
							}
							else if (s_CoolTime != -1)
							{
								s_CoolTime = g_tmCurrent;
							}


							//CString str;
							//str.Format("发送技能:%s ID:%d time:%d",pSkillProp->szName,pSkillProp->dwID,s_CoolTime);
							//g_WndMng.PutString(str);
						}


						//g_DPlay.SendUseSkill(0,Skill[i],g_pPlayer->m_objid);
					}
					j++;
				}
			}
			if (s_CoolTime != -1 && g_tmCurrent - s_CoolTime > 2000)
			{
				CObj* pObj;
				CCtrl* pSrc = g_pPlayer;
				D3DXVECTOR3 vPos = g_pPlayer->GetPos();
				BOOL	bApply;
				BOOL	lApply;
				FLOAT fRange = 140.0f;
				FLOAT nRange = 140.0f;
				FLOAT nline = 140.0f;
				fDist = 0.0f;
				D3DXVECTOR3	vDist;
				pSelectNEW = NULL;
				//	if( nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL) )
				{
					FOR_LINKMAP(g_pPlayer->GetWorld(), vPos, pObj, (int)nRange, CObj::linkDynamic, GetLayer())	// linkDynamic
					{
						D3DXVECTOR3 vStart = g_pPlayer->GetPos();			vStart.y += 0.5f;
						D3DXVECTOR3 vEnd = pObj->GetPos();	vEnd.y += 0.5f;
						float ftemp;

						bApply = TRUE;
						lApply = TRUE;

						if (pObj->GetType() == OT_MOVER && g_pPlayer->GetWorld()->IntersectObjLine(NULL, vStart, vEnd, FALSE, FALSE) == FALSE)
						{

							CMover* pTarget = (CMover*)pObj;
							if (fanwei->GetCheck())//挂机范围选择
							{
								bApply = pTarget->IsRangeObj(vPos, fRange);
							}
							CWndButton* guishu = (CWndButton*)GetDlgItem(301);
							if (guishu->GetCheck() && pTarget->m_idAttacker != NULL_ID && pTarget->m_idAttacker != g_pPlayer->GetId())
							{
								CMover* pTmp = prj.GetMover(pTarget->m_idAttacker);
								if (g_pPlayer->m_idparty && pTmp && pTmp->GetPartyId() == g_pPlayer->m_idparty)
								{

								}
								else
								{
									bApply = FALSE;
								}

							}

							//寻怪等级选择
							if (pTarget->GetLevel() >= MinLv && pTarget->GetLevel() <= MaxLv)
							{
								lApply = TRUE;
							}
							else
								lApply = FALSE;
							if (pTarget->GetHitPoint() > 0 && pTarget->IsPeaceful() == FALSE && bApply && lApply)		// 不是和平模式的NPC
							{

								vDist = vStart - vEnd;
								ftemp = D3DXVec3LengthSq(&vDist);
								if (fDist > 0)//找到更近的怪物
								{
									if (fDist > ftemp)
									{
										fDist = ftemp;
										pSelectNEW = (CMover*)pObj;
									}
								}
								else
								{
									fDist = ftemp;
									pSelectNEW = (CMover*)pObj;
								}
							}

						}


					}
					END_LINKMAP

						if (pSelectNEW)
						{
							pWorld->SetObjFocus(pSelectNEW);
						}
				}

			}
		}

		////////////////////////以下使用物品和团技能///////////////////


		for (unsigned int i = 0; i < BuffItem.size(); i++)
		{
			CWndButton* pButton = (CWndButton*)GetDlgItem(200 + i);
			DWORD dwId = g_pPlayer->m_Inventory.Find(BuffItem[i]);
			if (dwId != NULL_ID && pButton->GetCheck())
			{
				switch (BuffItem[i])
				{
				case II_CHR_SYS_SCR_UPCUTSTONE:
				{
					if (!g_pPlayer->IsSMMode(SM_ATTACK_UP))
					{
						CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
						g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
					}

				}break;
				case II_SYS_SYS_SCR_VELOCIJUMP:
				{
					if (!(g_pPlayer->IsSMMode(SM_VELOCIJUMP)))
					{
						CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
						g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
					}
				}break;
				case II_CHR_SYS_SCR_ACTIVITION:
				{
					if (!(g_pPlayer->IsSMMode(SM_ACTPOINT)))
					{
						CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
						g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
					}
				}break;
				//case II_GEN_FOO_INS_STARCANDYA:
				//case II_GEN_FOO_INS_STARCANDYB:
				//case II_GEN_FOO_INS_STARCANDYC:
				//	{
				//		if (!(g_pPlayer->HasBuff(BUFF_ITEM3, BuffItem[i])))
				//		{
				//			CItemElem* pItemBase = g_pPlayer->GetItemId(dwId);
				//			g_DPlay.SendDoUseItem( MAKELONG( ITYPE_ITEM, pItemBase->m_dwObjId ), NULL_ID );
				//		}
				//	}break;

				default:if (!g_pPlayer->HasBuff(BUFF_ITEM, BuffItem[i]))
				{
					CItemElem* pItemBase = g_pPlayer->m_Inventory.GetAtId(dwId);
					g_WndMng.UseItem(pItemBase, TYPE_INVENTORY);
				}break;
				}

			}
		}
		if (g_Party.GetSizeofMember() > 0 && g_Party.IsLeader(g_pPlayer->m_idPlayer) && g_Party.m_nKindTroup == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				CWndButton* pButton = (CWndButton*)GetDlgItem(400 + i);
				if (pButton->GetCheck())
				{
					ItemProp* pItemProp = prj.GetPartySkill(partySkill[i]);
					if (pItemProp)
					{
						if ((g_Party.GetLevel() - pItemProp->dwReqDisLV) >= 0 && int(g_Party.GetPoint() - pItemProp->dwExp) >= 0)
						{
							DWORD	dwSkillTimeTmp = 0;
							CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.m_pWndWorld;

							for (MAPBUFF::iterator t = pWndWorld->m_buffs.m_mapBuffs.begin(); t != pWndWorld->m_buffs.m_mapBuffs.end(); ++t)
							{
								IBuff* ptr = t->second;
								if (ptr->GetId() == partySkill[i])
									dwSkillTimeTmp = ptr->GetTotal();
							}

							if (dwSkillTimeTmp == 0)
							{
								g_DPlay.SendPartySkillUse(partySkill[i]);
							}
						}
					}

				}

			}
		}
	}
	return TRUE;
}


BOOL CWndRefresher::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_BUTTON_REFRESH_START:// Start Refreshing
	{
		bStart = TRUE;
		CWndButton* pButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_STOP);
		pButton2->EnableWindow(TRUE);
		CWndButton* pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_START);
		pButton1->EnableWindow(FALSE);
		break;
	}
	case WIDC_BUTTON_REFRESH_STOP:// Stop Refreshing
	{
		bStart = FALSE;
		CWndButton* pButton2 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_STOP);
		pButton2->EnableWindow(FALSE);
		CWndButton* pButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON_REFRESH_START);
		pButton1->EnableWindow(TRUE);

		break;
	}
	case WIDC_COMBOBOX1:
	{
		CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
		if (pCombo->GetCurSel() != -1)
		{
			m_dwitemid[1] = pCombo->GetItemData(pCombo->GetCurSel());
		}

		break;
	}
	case WIDC_COMBOBOX2:
	{
		CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX2);
		if (pCombo->GetCurSel() != -1)
		{
			m_dwitemid[2] = pCombo->GetItemData(pCombo->GetCurSel());
		}

		break;
	}
	case WIDC_COMBOBOX:
	{
		CWndComboBox* pCombo = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX);
		if (pCombo->GetCurSel() != -1)
		{
			m_dwitemid[0] = pCombo->GetItemData(pCombo->GetCurSel());
		}

		break;
	}

	case WIDC_RADIO:
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_RADIO);
		pButton->SetCheck(TRUE);
		pButton = (CWndButton*)GetDlgItem(WIDC_RADIO1);
		pButton->SetCheck(FALSE);
		break;
	}
	case WIDC_RADIO1:
	{
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_RADIO1);
		pButton->SetCheck(TRUE);
		pButton = (CWndButton*)GetDlgItem(WIDC_RADIO);
		pButton->SetCheck(FALSE);
		break;
	}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


#endif

#ifdef __FL_PARTY_FINDER
CWndPartyFinder::CWndPartyFinder()
	: m_pListItems(NULL)
	, m_pWndComboFilter(NULL)
	, m_pSearchBox(NULL)
	, m_pTexRed(NULL)
	, m_pTexOrange(NULL)
	, m_pTexGreen(NULL)
	, m_pTexLink(NULL)
	, m_pTexDrop(NULL)
	, m_pTexGift(NULL)
	, m_pTextResting(NULL)
{

}
CWndPartyFinder::~CWndPartyFinder()
{
	m_vecPartyListOriginal.clear();
	m_vecPartyListFiltered.clear();
}
void CWndPartyFinder::OnDraw(C2DRender* p2DRender)
{
}
void CWndPartyFinder::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SendRefreshRequest();

	m_pListItems = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pSearchBox->SetStringBg("Search by leader name..");
	m_pWndComboFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pWndComboFilter->AddWndStyle(EBS_READONLY);
	m_pWndComboFilter->AddString("Leader Lvl (Highest)");
	m_pWndComboFilter->AddString("Leader Lvl (Lowest)");
	m_pWndComboFilter->AddString("Members (Highest)");
	m_pWndComboFilter->AddString("Members (Lowest)");
	m_pWndComboFilter->AddString("Points (Highest)");
	m_pWndComboFilter->AddString("Points (Lowest)");
	m_pWndComboFilter->AddString("Party Lvl (Highest)");
	m_pWndComboFilter->AddString("Party Lvl (Lowest)");
	m_pWndComboFilter->SetCurSel(2);

	// Pre-load scroll textures
	ItemProp* pPropTmp = prj.GetItemProp(II_CHR_SYS_SCR_PSKILLFULL30);
	if (pPropTmp)
		m_pTexRed = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pPropTmp->szIcon), 0xffff00ff);
	pPropTmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYEXPUP02);
	if (pPropTmp)
		m_pTexOrange = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pPropTmp->szIcon), 0xffff00ff);
	pPropTmp = prj.GetItemProp(II_SYS_SYS_SCR_PARTYSKILLUP02);
	if (pPropTmp)
		m_pTexGreen = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pPropTmp->szIcon), 0xffff00ff);

	// Pre-load skill textures
	pPropTmp = prj.GetPartySkill(ST_LINKATTACK);
	if (pPropTmp)
		m_pTexLink = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pPropTmp->szIcon), 0xffff00ff);
	pPropTmp = prj.GetPartySkill(ST_FORTUNECIRCLE);
	if (pPropTmp)
		m_pTexDrop = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pPropTmp->szIcon), 0xffff00ff);
	pPropTmp = prj.GetPartySkill(ST_GIFTBOX);
	if (pPropTmp)
		m_pTexGift = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pPropTmp->szIcon), 0xffff00ff);
	pPropTmp = prj.GetPartySkill(ST_STRETCHING);
	if (pPropTmp)
		m_pTextResting = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pPropTmp->szIcon), 0xffff00ff);

	m_Menu.CreateMenu(this);
	MoveParentCenter();
}
BOOL CWndPartyFinder::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PARTY_FINDER, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndPartyFinder::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_Menu)
	{
		if (m_pListItems->GetCurSel() != -1)
		{
			PARTY_LIST* list = &m_vecPartyListFiltered.at(m_pListItems->GetCurSel());

			switch (nID)
			{
			case 0: g_DPlay.SendJoinPartyFinder(list->m_dwPartyId); break;
			case 1:
			{
				if (g_WndMng.m_RTMessenger.GetFriend(list->m_dwLeaderId))
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0051)));
				else
				{
					g_DPlay.SendAddFriendReqest(list->m_dwLeaderId);
					CString str;
					str.Format(prj.GetText(TID_GAME_MSGINVATE), list->m_strLeaderName);
					g_WndMng.PutString(str, NULL, prj.GetTextColor(TID_GAME_MSGINVATE));
				}
			}
			break;
			case 2: g_WndMng.OpenMessage(list->m_strLeaderName); break;
			default: break;
			}
		}
	}
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndPartyFinder::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON_1:
		{
			KillFocus(m_pSearchBox, NULL);
			m_pSearchBox->Empty();
			Filter();
		}
		break;
		case WIDC_BUTTON_2:
		{
			SendRefreshRequest();
		}
		break;
		case WIDC_CHECK1:
		case WIDC_CHECK2:
		case WIDC_CHECK3:
		case WIDC_CHECK4:
		{
			Filter();
		}
		break;
		}
	}
	else if (message == EN_CHANGE)
	{
		if (nID == WIDC_EDIT1)
			Filter();
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == WIDC_COMBOBOX1)
			Filter();
		else if (nID == WIDC_LISTBOX1)
		{
			if (m_pListItems->GetCurSel() != -1)
			{
				if (g_Party.FindMember(m_vecPartyListFiltered.at(m_pListItems->GetCurSel()).m_dwLeaderId) == -1)
				{
					CPoint point = GetMousePoint();
					ClientToScreen(&point);
					m_Menu.DeleteAllMenu();
					m_Menu.AppendMenu(0, 0, prj.GetText(TID_APP_JOIN)); // join
					m_Menu.AppendMenu(0, 1, prj.GetText(TID_MMI_ADD_MESSENGER)); // add messenger
					m_Menu.AppendMenu(0, 2, prj.GetText(TID_GAME_TAGSEND)); // send message
					m_Menu.Move(point);
					m_Menu.SetVisible(TRUE);
					m_Menu.SetFocus();
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPartyFinder::ReceiveList(CAr& ar)
{
	m_vecPartyListOriginal.clear();

	u_long nSize;
	ar >> nSize;

	char strLeaderName[MAX_NAME];

	for (u_long i = 0; i < nSize; i++)
	{
		PARTY_LIST party;

		// Leader info
		ar >> party.m_dwLeaderId;
		ar >> party.m_nLeaderLevel;
		ar >> party.m_nLeaderJob;
		ar.ReadString(strLeaderName, MAX_NAME);
		party.m_strLeaderName = strLeaderName;

		// Party info
		ar >> party.m_dwPartyId;
		ar >> party.m_nPartyMembers;
		ar >> party.m_nPartyLevel;
		ar >> party.m_nPartyPoint;

		// Skills/Scrolls
		ar >> party.m_bLink;
		ar >> party.m_bDrop;
		ar >> party.m_bGift;
		ar >> party.m_bResting;
		ar >> party.m_bRedScroll;
		ar >> party.m_bOrangeScroll;
		ar >> party.m_bGreenScroll;

		m_vecPartyListOriginal.push_back(party);
	}

	Filter();
}
void CWndPartyFinder::Filter()
{
	m_vecPartyListFiltered.clear();

	CWndButton* pWndCheck = NULL;
	for (auto& it : m_vecPartyListOriginal)
	{
		// Filter by name
		CString strFilter = m_pSearchBox->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = it.m_strLeaderName;
			if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
				continue;
		}

		// Hide parties without scrolls
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		if (pWndCheck->GetCheck())
		{
			if (!it.m_bRedScroll && !it.m_bOrangeScroll && !it.m_bGreenScroll)
				continue;
		}

		// Hide parties without skills
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		if (pWndCheck->GetCheck())
		{
			if (!it.m_bLink && !it.m_bDrop && !it.m_bGift && !it.m_bResting)
				continue;
		}

		// Hide parties with non-friend leader
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		if (pWndCheck->GetCheck())
		{
			if (g_WndMng.m_RTMessenger.GetFriend(it.m_dwLeaderId) == NULL)
				continue;
		}

		// Show full parties
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		if (!pWndCheck->GetCheck())
		{
			if (it.m_nPartyMembers == MAX_PTMEMBER_SIZE_SPECIAL)
				continue;
		}

		m_vecPartyListFiltered.push_back(it);
	}

	switch (m_pWndComboFilter->GetCurSel())
	{
	case 0: SortByLeaderLevel(TRUE); break;
	case 1: SortByLeaderLevel(FALSE); break;
	case 2: SortByMembers(TRUE); break;
	case 3: SortByMembers(FALSE); break;
	case 4: SortByPoints(TRUE); break;
	case 5: SortByPoints(FALSE); break;
	case 6: SortByPartyLevel(TRUE); break;
	case 7: SortByPartyLevel(FALSE); break;
	default: break;
	}

	m_pListItems->ResetContent();
	for (auto& it : m_vecPartyListFiltered)
		m_pListItems->AddString("");
}
void CWndPartyFinder::SendRefreshRequest()
{
	g_DPlay.SendHdr(PACKETTYPE_REQUEST_PARTY_FINDER);
}
bool SortByLeaderLevelHighest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nLeaderLevel > s2.m_nLeaderLevel);
}
bool SortByLeaderLevelLowest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nLeaderLevel < s2.m_nLeaderLevel);
}
bool SortByMembersHighest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyMembers > s2.m_nPartyMembers);
}
bool SortByMembersLowest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyMembers < s2.m_nPartyMembers);
}
bool SortByPointsHighest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyPoint > s2.m_nPartyPoint);
}
bool SortByPointsLowest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyPoint < s2.m_nPartyPoint);
}
bool SortByPartyLevelHighest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyLevel > s2.m_nPartyLevel);
}
bool SortByPartyLevelLowest(PARTY_LIST s1, PARTY_LIST s2)
{
	return (s1.m_nPartyLevel < s2.m_nPartyLevel);
}
void CWndPartyFinder::SortByLeaderLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByLeaderLevelHighest);
	else
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByLeaderLevelLowest);
}
void CWndPartyFinder::SortByMembers(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByMembersHighest);
	else
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByMembersLowest);
}
void CWndPartyFinder::SortByPoints(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByPointsHighest);
	else
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByPointsLowest);
}
void CWndPartyFinder::SortByPartyLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByPartyLevelHighest);
	else
		std::sort(m_vecPartyListFiltered.begin(), m_vecPartyListFiltered.end(), SortByPartyLevelLowest);
}
#endif

#ifdef __FL_GUILD_FINDER
CWndGuildFinder::CWndGuildFinder()
	: m_pListGuilds(NULL)
	, m_pListMembers(NULL)
	, m_pTextDetails(NULL)
	, m_pSearchBox(NULL)
	, m_pMinLevelFilter(NULL)
	, m_pMaxLevelFilter(NULL)
	, m_pWndComboFilter(NULL)
	, m_pWndButtonShowFull(NULL)
	, m_pWndButtonHouse(NULL)
	, m_pWndButtonFurni(NULL)
	, m_pWndButtonJoin(NULL)
	, m_pWndButtonCancel(NULL)
	, m_pWndButtonSort(NULL)
	, m_pTexHouse(NULL)
{

}
CWndGuildFinder::~CWndGuildFinder()
{
	m_vecGuildListOriginal.clear();
	m_vecGuildListFiltered.clear();
}
void CWndGuildFinder::OnDraw(C2DRender* p2DRender)
{
}
BOOL CWndGuildFinder::Process(void)
{
	if (IsSelected())
	{
		m_pWndButtonJoin->EnableWindow(TRUE);
		m_pWndButtonCancel->EnableWindow(TRUE);
	}
	else
	{
		m_pWndButtonJoin->EnableWindow(FALSE);
		m_pWndButtonCancel->EnableWindow(FALSE);
	}

	if (m_pWndButtonFurni->GetCheck())
		m_pWndButtonHouse->EnableWindow(FALSE);
	else
		m_pWndButtonHouse->EnableWindow(TRUE);

	return TRUE;
}
void CWndGuildFinder::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	SendRefreshRequest();

	m_pListGuilds = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	m_pListMembers = (CWndListBox*)GetDlgItem(WIDC_LISTBOX2);
	m_pTextDetails = (CWndText*)GetDlgItem(WIDC_TEXT1);
	m_pSearchBox = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pSearchBox->SetStringBg("Search by guild name..");
	m_pMinLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pMaxLevelFilter = (CWndEdit*)GetDlgItem(WIDC_EDIT3);
	m_pWndComboFilter = (CWndComboBox*)GetDlgItem(WIDC_COMBOBOX1);
	m_pWndButtonShowFull = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	m_pWndButtonHouse = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	m_pWndButtonFurni = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	m_pWndButtonJoin = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pWndButtonCancel = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	m_pWndButtonSort = (CWndButton*)GetDlgItem(WIDC_BUTTON3);

	CString str;
	str.Format("%d", MAX_GUILD_LEVEL);

	m_pMinLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMaxLevelFilter->AddWndStyle(EBS_NUMBER);
	m_pMinLevelFilter->SetString("1");
	m_pMaxLevelFilter->SetString(str);

	m_pWndComboFilter->AddWndStyle(EBS_READONLY);
	m_pWndComboFilter->AddString("Guild Lvl (Highest)");
	m_pWndComboFilter->AddString("Guild Lvl (Lowest)");
	m_pWndComboFilter->AddString("Members (Highest)");
	m_pWndComboFilter->AddString("Members (Lowest)");
	m_pWndComboFilter->AddString("Master Lvl (Highest)");
	m_pWndComboFilter->AddString("Master Lvl (Lowest)");
	m_pWndComboFilter->SetCurSel(0);

	m_pTexHouse = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "guildfinderhouse.png"), 0xffff00ff);

	MoveParentCenter();
}
BOOL CWndGuildFinder::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_GUILD_FINDER, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndGuildFinder::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndGuildFinder::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON3:
		{
			SendRefreshRequest();
			break;
		}
		case WIDC_BUTTON4:
		{
			KillFocus(m_pSearchBox, NULL);
			m_pSearchBox->Empty();
			Filter();
			break;
		}
		case WIDC_CHECK1:
		case WIDC_CHECK2:
		case WIDC_CHECK3: Filter(); break;
		case WIDC_BUTTON1:
		{
			if (IsSelected())
				g_DPlay.SendGuildFinderJoin(m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_dwGuildId);
			break;
		}
		case WIDC_BUTTON2:
		{
			m_pListGuilds->SetCurSel(-1);
			ResetAdditionalInfo();
			break;
		}
		default: break;
		}
	}
	else if (message == EN_CHANGE)
	{
		switch (nID)
		{
		case WIDC_EDIT1:
		case WIDC_EDIT2:
		case WIDC_EDIT3:
		{
			int nMaxLevel = atoi(m_pMaxLevelFilter->GetString());
			if (nMaxLevel > MAX_GUILD_LEVEL)
			{
				CString str;
				str.Format("%d", MAX_GUILD_LEVEL);
				m_pMaxLevelFilter->SetString(str);
			}

			Filter();
			break;
		}
		default: break;
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		switch (nID)
		{
		case WIDC_COMBOBOX1: Filter(); break;
		case WIDC_LISTBOX1:
		{
			ResetAdditionalInfo();
			if (IsSelected())
			{
				if (m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_vecMembers.size() == 0)
					g_DPlay.SendRequestGuildFinderAddon(m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_dwGuildId);
				else
					SetAdditionalInfo(m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_dwGuildId);
			}
			break;
		}
		default: break;
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndGuildFinder::SendRefreshRequest()
{
	g_DPlay.SendHdr(PACKETTYPE_REQUEST_GUILD_FINDER);
}
void CWndGuildFinder::ReceiveList(CAr& ar)
{
	m_vecGuildListOriginal.clear();

	int nSize;
	ar >> nSize;

	char strLeaderName[MAX_NAME];
	CGuild* pGuild;
	int nSize2;
	DWORD dwItemId;

	for (int i = 0; i < nSize; i++)
	{
		GUILD_LIST guild;

		// Guild info (serialization)
		ar >> guild.m_dwGuildId;
		ar >> guild.m_bGuildHouse;
		ar >> guild.m_nMembers;
		ar >> guild.m_nLevel;

		// Leader info (serialization)
		ar >> guild.m_nLeaderLevel;
		ar >> guild.m_nLeaderJob;
		ar.ReadString(strLeaderName, MAX_NAME);
		guild.m_strLeaderName = strLeaderName;

		// Furniture info
		ar >> nSize2;
		for (int i = 0; i < nSize2; i++)
		{
			ar >> dwItemId;
			guild.m_vecFurniture.push_back(dwItemId);
		}

		pGuild = g_GuildMng.GetGuild(guild.m_dwGuildId);
		if (!pGuild)
			continue;

		// Guild info (existing)
		lstrcpy(guild.m_szGuild, pGuild->m_szGuild);
		/*guild.m_nMaxMembers = pGuild->GetMaxMemberSize();*/
		guild.m_nMaxMembers = (int)CGuildTable::GetInstance().GetMaxMemeber(guild.m_nLevel);

		// Leader info (existing)
		guild.m_dwLeaderId = pGuild->m_idMaster;

		m_vecGuildListOriginal.push_back(guild);
	}

	Filter();
}
void CWndGuildFinder::Filter()
{
	m_vecGuildListFiltered.clear();

	CWndButton* pWndCheck = NULL;

	for (auto& it : m_vecGuildListOriginal)
	{
		// Filter by name
		CString strFilter = m_pSearchBox->GetString();
		if (!strFilter.IsEmpty())
		{
			CString strName = it.m_szGuild;
			if (!strstr(strName.MakeLower(), strFilter.MakeLower()))
				continue;
		}

		// Show full guilds
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		if (!pWndCheck->GetCheck())
		{
			if (it.m_nMembers == it.m_nMaxMembers)
				continue;
		}

		// Hide guilds without house
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		if (pWndCheck->GetCheck())
		{
			if (!it.m_bGuildHouse)
				continue;
		}

		// Hide guilds without furniture
		pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		if (pWndCheck->GetCheck())
		{
			if (!it.m_vecFurniture.size())
				continue;
		}

		// Filter by min/max level
		int nMinLevel = atoi(m_pMinLevelFilter->GetString());
		if (it.m_nLevel < nMinLevel)
			continue;

		int nMaxLevel = atoi(m_pMaxLevelFilter->GetString());
		if (it.m_nLevel > nMaxLevel)
			continue;

		m_vecGuildListFiltered.push_back(it);
	}

	switch (m_pWndComboFilter->GetCurSel())
	{
	case 0: SortByGuildLevel(TRUE); break;
	case 1: SortByGuildLevel(FALSE); break;
	case 2: SortByMembers(TRUE); break;
	case 3: SortByMembers(FALSE); break;
	case 4: SortByMasterLevel(TRUE); break;
	case 5: SortByMasterLevel(FALSE); break;
	default: break;
	}

	m_pListGuilds->ResetContent();
	for (auto& it : m_vecGuildListFiltered)
		m_pListGuilds->AddString("");

	ResetAdditionalInfo();
}
BOOL CWndGuildFinder::IsSelected()
{
	if (m_pListGuilds->GetCurSel() >= 0 && m_pListGuilds->GetCurSel() < m_vecGuildListFiltered.size())
		return TRUE;

	return FALSE;
}
vector<CWndGuildFinder::GUILD_MEMBER>* CWndGuildFinder::GetListMembers()
{
	if (!IsSelected())
		return NULL;

	return &m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_vecMembers;
}
void CWndGuildFinder::ResetAdditionalInfo()
{
	m_pTextDetails->SetString("");
	m_pListMembers->ResetContent();
}
void CWndGuildFinder::ReceiveAdditionalInfo(CAr& ar)
{
	u_long idGuild;
	char m_szNotice[MAX_BYTE_NOTICE];
	size_t nMembers;

	ar >> idGuild;
	ar.ReadString(m_szNotice, MAX_BYTE_NOTICE);
	ar >> nMembers;

	vector<GUILD_MEMBER> vecMembers;
	GUILD_MEMBER member;

	char strName[MAX_NAME];
	int nMemberLv;
	int nMemberJob;
	for (size_t n = 0; n < nMembers; n++)
	{
		ar.ReadString(strName, MAX_NAME);
		member.m_strName = strName;
		ar >> member.m_nLevel;
		ar >> member.m_nJob;
		vecMembers.push_back(member);
	}

	for (auto& it : m_vecGuildListOriginal)
	{
		if (it.m_dwGuildId != idGuild)
			continue;

		it.m_strGuildDesc = m_szNotice;
		it.m_vecMembers.clear();
		it.m_vecMembers = vecMembers;
		break;
	}

	for (auto& it : m_vecGuildListFiltered)
	{
		if (it.m_dwGuildId != idGuild)
			continue;

		it.m_strGuildDesc = m_szNotice;
		it.m_vecMembers.clear();
		it.m_vecMembers = vecMembers;
		break;
	}

	if (IsSelected())
		SetAdditionalInfo(m_vecGuildListFiltered.at(m_pListGuilds->GetCurSel()).m_dwGuildId);
}
void CWndGuildFinder::SetAdditionalInfo(u_long idGuild)
{
	ResetAdditionalInfo();

	CString str;
	for (auto& it : m_vecGuildListFiltered)
	{
		if (it.m_dwGuildId != idGuild)
			continue;

		str.Format("Guild: %s\nLevel: %d\nMembers: %d / %d\nDescription: %s", it.m_szGuild, it.m_nLevel, it.m_nMembers, it.m_nMaxMembers, it.m_strGuildDesc);
		m_pTextDetails->AddString(str);

		for (auto& it2 : it.m_vecMembers)
			m_pListMembers->AddString("");

		break;
	}

}
bool SortByGuildLevelHighest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nLevel > s2.m_nLevel);
}
bool SortByGuildLevelLowest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nLevel < s2.m_nLevel);
}
bool SortByGuildMembersHighest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nMembers > s2.m_nMembers);
}
bool SortByGuildMembersLowest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nMembers < s2.m_nMembers);
}
bool SortByMasterLevelHighest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nLeaderLevel > s2.m_nLeaderLevel);
}
bool SortByMasterLevelLowest(CWndGuildFinder::GUILD_LIST s1, CWndGuildFinder::GUILD_LIST s2)
{
	return (s1.m_nLeaderLevel < s2.m_nLeaderLevel);
}
void CWndGuildFinder::SortByGuildLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByGuildLevelHighest);
	else
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByGuildLevelLowest);
}
void CWndGuildFinder::SortByMembers(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByGuildMembersHighest);
	else
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByGuildMembersLowest);
}
void CWndGuildFinder::SortByMasterLevel(BOOL bHighest)
{
	if (bHighest)
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByMasterLevelHighest);
	else
		std::sort(m_vecGuildListFiltered.begin(), m_vecGuildListFiltered.end(), SortByMasterLevelLowest);
}
#endif

#ifdef __FL_FARM_DISPLAY
CWndFarmDisplay::CWndFarmDisplay()
	: m_nWndState(STATE_OFF)
	, m_nTotalKills(0)
	, m_nTotalKillsOld(0)
	, m_iTotalDamage(0)
	, m_iTotalDamageOld(0)
	, m_iTotalPenya(0)
	, m_nPenyaOld(0)
	, m_pWndBtnStartStop(nullptr)
	, m_pWndBtnPause(nullptr)
	, m_pWndBtnRefresh(nullptr)
	, m_pWndBtnWiki(nullptr)
	, m_pWndBtnSize(nullptr)
	, m_pWndBtnClose(nullptr)
	, m_pTextureStartTime(nullptr)
	, m_pTextureTotalKills(nullptr)
	, m_pTextureTotalDamage(nullptr)
	, m_pTextureTotalPenya(nullptr)
	, m_pTextureTotalPerins(nullptr)
	, m_bSmall(FALSE)
	, m_tTimer(0)
	, m_tPauseTimer(0)
	, m_pWndFarmDisplaySet(nullptr)
	, m_pTexBtnStart(nullptr)
	, m_pTexBtnStop(nullptr)
{
	m_bNoCloseButton = TRUE;
	m_nAlphaCount = 0;
}

CWndFarmDisplay::~CWndFarmDisplay()
{
	prj.SaveFarmDisplay();
	SAFE_DELETE(m_pWndFarmDisplaySet);
}

void CWndFarmDisplay::OnDraw(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(m_Theme.m_pFontItemNumbers);

	CRect rectApplet = GetWindowRect();
	CRect rectFrame = rectApplet;
	rectApplet.DeflateRect(1, 1, 1, 1);

	CPoint ptLine1(rectApplet.left, rectApplet.top);
	CPoint ptLine2(rectApplet.right, 0);
	CPoint ptIcon(7, 5);
	CTexture* pTex = nullptr;
	CString strValue = "";
	ItemProp* pProp = nullptr;

	int nPerinNum = (int)(m_iTotalPenya / PERIN_VALUE);
	int nPenyaNum = (int)(m_iTotalPenya - (nPerinNum * PERIN_VALUE));

	// General window rendering
	p2DRender->RenderRect(rectFrame, 0xffa69500);
	p2DRender->RenderFillRect(rectApplet, D3DCOLOR_ARGB(180, 0, 0, 0));

	for (int i = 1; i < DEFAULTELEMENTS + prj.m_vecFarmDisplay.size(); i++)
	{
		ptLine1.y += ONEHEIGHT;
		ptIcon.y += ONEHEIGHT;

		if (m_bSmall && i == 2)
			break;

		p2DRender->RenderLine(ptLine1, CPoint(ptLine2.x, ptLine1.y), m_dwLineColor);

		switch (i)
		{
		case 1:
		{
			pTex = m_pTextureStartTime;

			if (m_tTimer > 0)
			{
				if (m_nWndState == STATE_ON)
				{
					time_t t = m_tTimer - time_null();
					if (t > 0)
					{
						CTimeSpan time(t);
						strValue.Format("%.2d:%.2d:%.2d", time.GetHours(), time.GetMinutes(), time.GetSeconds());
					}
					else
						InitState(STATE_OFF);
				}
				else if (m_nWndState == STATE_PAUSE)
				{
					if (m_tPauseTimer > 0)
					{
						CTimeSpan time(m_tPauseTimer);
						strValue.Format("%.2d:%.2d:%.2d (Paused)", time.GetHours(), time.GetMinutes(), time.GetSeconds());
					}
					else
						strValue = "(Paused)";
				}
			}
			else
				strValue.Format("-");

			break;
		}
		case 2:
		{
			pTex = m_pTextureTotalKills;

			int nKills = m_nTotalKills;
			g_WndMng.MakeNumberSmooth(&nKills, &m_nTotalKillsOld);
			strValue.Format("%s", Separator(nKills));
			break;
		}
		case 3:
		{
			pTex = m_pTextureTotalDamage;

			__int64 nDmg = m_iTotalDamage;
			g_WndMng.MakeNumberSmooth64(&nDmg, &m_iTotalDamageOld);
			strValue.Format("%s", Separator(nDmg));
			break;
		}
		case 4:
		{
			pTex = m_pTextureTotalPenya;

			g_WndMng.MakeNumberSmooth(&nPenyaNum, &m_nPenyaOld);
			strValue.Format("%s", Separator(nPenyaNum));
			break;
		}
		case 5:
		{
			pTex = m_pTextureTotalPerins;
			strValue.Format("%s", Separator(nPerinNum));
			break;
		}
		default:
		{
			pProp = prj.GetItemProp(prj.m_vecFarmDisplay.at(i - DEFAULTELEMENTS).dwItem);
			if (!pProp)
				continue;

			pTex = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
			strValue.Format("%s", Separator(prj.m_vecFarmDisplay.at(i - DEFAULTELEMENTS).nCount));
			break;
		}
		}

		if (pTex)
			p2DRender->RenderTexture(ptIcon, pTex, 255, 0.5f, 0.5f);

		p2DRender->TextOut(28, ptIcon.y, strValue, m_dwTextColor);
	}

	// Last line
	ptLine1.y += ONEHEIGHT;
	ptIcon.y += ONEHEIGHT;
	p2DRender->RenderLine(ptLine1, CPoint(ptLine2.x, ptLine1.y), m_dwLineColor);

	p2DRender->SetFont(pOldFont);
}

BOOL CWndFarmDisplay::Process()
{
	m_pWndBtnPause->EnableWindow(TRUE);
	if (m_nWndState == STATE_OFF)
	{
		m_pWndBtnPause->EnableWindow(FALSE);
		m_pWndBtnStartStop->SetTexture(D3DDEVICE, NULL, m_pTexBtnStart);
	}
	else
		m_pWndBtnStartStop->SetTexture(D3DDEVICE, NULL, m_pTexBtnStop);

	return TRUE;
}
void CWndFarmDisplay::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndBtnStartStop = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pWndBtnPause = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	m_pWndBtnRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	m_pWndBtnWiki = (CWndButton*)GetDlgItem(WIDC_BUTTON4);
	m_pWndBtnSize = (CWndButton*)GetDlgItem(WIDC_BUTTON5);
	m_pWndBtnClose = (CWndButton*)GetDlgItem(WIDC_BUTTON6);

	m_pTextureStartTime = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "farmtimer.png"), 0xffff00ff);
	m_pTextureTotalKills = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "killcount.png"), 0xffff00ff);
	m_pTextureTotalDamage = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "cntdamage.png"), 0xffff00ff);
	m_pTextureTotalPenya = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, "itm_GolGolSeed.dds"), 0xffff00ff);
	m_pTextureTotalPerins = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, "Itm_SysSysScrPerin.png"), 0xffff00ff);

	m_pTexBtnStart = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ButtPlay.tga"), 0xffff00ff);
	m_pTexBtnStop = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ButtPlayStop.tga"), 0xffff00ff);

	UpdateSize();
	MoveParentCenter();
}

BOOL CWndFarmDisplay::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_FARM_DISPLAY);
}
void CWndFarmDisplay::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndFarmDisplay::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1:
		{
			if (m_nWndState == STATE_OFF)
			{
				SAFE_DELETE(m_pWndFarmDisplaySet);
				m_pWndFarmDisplaySet = new CWndFarmDisplaySet();
				m_pWndFarmDisplaySet->Initialize(this);
			}
			else
				InitState(STATE_OFF);

			break;
		}
		case WIDC_BUTTON2:
		{
			if (m_nWndState == STATE_ON)
				InitState(STATE_PAUSE);
			else if (m_nWndState == STATE_PAUSE)
				InitState(STATE_ON);

			break;
		}
		case WIDC_BUTTON3:
		{
			CWndWikiItems* pWndWiki = (CWndWikiItems*)g_WndMng.CreateApplet(APP_WIKI_ITEMS);
			if (pWndWiki)
			{
				pWndWiki->SetMode(1);
				g_WndMng.PutString("Double click on items which you want to add.");
			}

			break;
		}
		case WIDC_BUTTON4:
		{
			prj.m_vecFarmDisplay.clear();
			UpdateSize();

			g_WndMng.PutString("All tracked items was removed from the list.");
			break;
		}
		case WIDC_BUTTON5: UpdateSize(TRUE); break;
		case WIDC_BUTTON6: Destroy(); break;
		default: break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndFarmDisplay::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, rectWnd.Width(), rectWnd.Height());
	AdjustMaxRect(&rectWnd, rectWnd.Width(), rectWnd.Height());
	CWndNeuz::SetWndRect(rectWnd, bOnSize);

}
void CWndFarmDisplay::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndFarmDisplaySet)
		SAFE_DELETE(m_pWndFarmDisplaySet);
}
void CWndFarmDisplay::UpdateSize(BOOL bManual)
{
	if (bManual)
		m_bSmall = !m_bSmall;
	else
		m_bSmall = FALSE;

	CRect rect = GetWindowRect(TRUE);
	if (m_bSmall)
		rect.bottom = rect.top + ONEHEIGHT * 2;
	else
	{
		const int nStaticDetails = DEFAULTELEMENTS * ONEHEIGHT;
		const int nDynamicDetails = prj.m_vecFarmDisplay.size() * ONEHEIGHT;
		rect.bottom = rect.top + nStaticDetails + nDynamicDetails;
	}

	SetWndRect(rect);
}
void CWndFarmDisplay::AddItem(DWORD dwItem, int nVal)
{
	if (m_nWndState != STATE_ON)
		return;

	for (auto& it : prj.m_vecFarmDisplay)
	{
		if (it.dwItem != dwItem)
			continue;

		it.nCount += nVal;
		break;
	}
}
void CWndFarmDisplay::AddFromWiki(DWORD dwItem)
{
	if (prj.m_vecFarmDisplay.size() >= 10)
	{
		g_WndMng.PutString("You cannot add more than 10 items to the track list.");
		return;
	}

	for (auto& it : prj.m_vecFarmDisplay)
	{
		if (it.dwItem == dwItem)
			return;
	}

	FARM_DISPLAY tmp;
	tmp.dwItem = dwItem;
	tmp.nCount = 0;
	prj.m_vecFarmDisplay.push_back(tmp);

	UpdateSize();
}
void CWndFarmDisplay::ResetValues()
{
	m_nTotalKills = 0;
	m_iTotalDamage = 0;
	m_iTotalPenya = 0;
	m_tTimer = 0;
	m_tPauseTimer = 0;

	for (auto& it : prj.m_vecFarmDisplay)
		it.nCount = 0;
}
void CWndFarmDisplay::InitState(int nState, int nHour, int nMin, int nSec)
{
	if (m_nWndState == nState)
		return;

	const int nOldState = m_nWndState;
	m_nWndState = nState;

	switch (m_nWndState)
	{
	case STATE_ON:
	{
		if (nOldState == STATE_OFF)
		{
			ResetValues();
			CTime tm = CTime::GetCurrentTime() + CTimeSpan(0, nHour, nMin, nSec);
			m_tTimer = (time_t)(tm.GetTime());
		}
		else
		{
			CTime tm = CTime::GetCurrentTime() + m_tPauseTimer;
			m_tTimer = (time_t)(tm.GetTime());
			m_tPauseTimer = 0;
		}
		g_WndMng.PutString("Farm counter activated.");
		break;
	}
	case STATE_OFF:
	{
		m_tTimer = 0;
		m_tPauseTimer = 0;
		g_WndMng.PutString("Farm counter deactivated.");
		break;
	}
	case STATE_PAUSE:
	{
		m_tPauseTimer = m_tTimer - time_null();
		g_WndMng.PutString("Farm counter paused.");
		break;
	}
	default: break;
	}
}

CWndFarmDisplaySet::CWndFarmDisplaySet()
	: m_pEditHour(NULL)
	, m_pEditMin(NULL)
	, m_pEditSec(NULL)
{
}

CWndFarmDisplaySet::~CWndFarmDisplaySet()
{
}

BOOL CWndFarmDisplaySet::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_FARM_DISPLAY_SET, 0, CPoint(0, 0), pWndParent);
}

void CWndFarmDisplaySet::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pEditHour = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	m_pEditHour->AddWndStyle(EBS_NUMBER);
	m_pEditHour->SetString("1");

	m_pEditMin = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	m_pEditMin->AddWndStyle(EBS_NUMBER);
	m_pEditMin->SetString("0");

	m_pEditSec = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	m_pEditSec->AddWndStyle(EBS_NUMBER);
	m_pEditSec->SetString("0");

	MoveParentCenter();
}
void CWndFarmDisplaySet::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
}
BOOL CWndFarmDisplaySet::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	int nNumber = atoi(m_pEditHour->GetString());
	if (nNumber > 23)
		m_pEditHour->SetString("23");

	nNumber = atoi(m_pEditMin->GetString());
	if (nNumber > 59)
		m_pEditMin->SetString("59");

	nNumber = atoi(m_pEditSec->GetString());
	if (nNumber > 59)
		m_pEditSec->SetString("59");

	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON)
		{
			CWndFarmDisplay* pWnd = (CWndFarmDisplay*)GetWndBase(APP_FARM_DISPLAY);
			if (pWnd)
			{
				pWnd->InitState(CWndFarmDisplay::STATE_ON, atoi(m_pEditHour->GetString()), atoi(m_pEditMin->GetString()), atoi(m_pEditSec->GetString()));
				Destroy();
			}
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif
#ifdef __FL_PACKED_ITEMS
CWndPackedItems::CWndPackedItems()
	: m_pItemElem(nullptr)
	, m_pListBox(nullptr)
	, m_pButtonUse(nullptr)
	, m_pButtonRestore(nullptr)
	, m_dwHighAlpha(0)
{

}

CWndPackedItems::~CWndPackedItems()
{
	ClearItem();
}

void CWndPackedItems::OnDraw(C2DRender* p2DRender)
{
	if (!m_pItemElem)
		return;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	if (m_pItemElem->m_pTexture)
		m_pItemElem->m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));

	if (IsListboxBgState())
	{
		CRect rectListBox = m_pListBox->GetWndRect();
		rectListBox.right -= 4;
		rectListBox.bottom -= 16;

		DWORD dwColor;
		if (m_dwHighAlpha < 128)
			dwColor = D3DCOLOR_ARGB(m_dwHighAlpha, 0, 255, 0);
		else
			dwColor = D3DCOLOR_ARGB(128 - (m_dwHighAlpha - 128), 0, 255, 0);

		p2DRender->RenderFillRect(rectListBox, dwColor);
	}
}
void CWndPackedItems::DrawItemlist(C2DRender* p2DRender)
{
	if (!m_pItemElem)
		return;

	const auto pVec = m_pItemElem->GetItemList();

	const CRect rectClient = m_pListBox->GetClientRect();
	const int nPage = rectClient.Width() / m_pListBox->m_nFontHeight;
	m_pListBox->m_wndScrollBar.SetScrollRange(0, pVec->size());
	m_pListBox->m_wndScrollBar.SetScrollPage(nPage);

	CRect rectItem(3, rectClient.top, 3 + m_pListBox->m_nFontHeight, rectClient.bottom);
	CPoint pointTexture = CPoint(0, 0);

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

	for (int i = m_pListBox->m_wndScrollBar.GetScrollPos(); i < (int)pVec->size(); ++i)
	{
		if (i > (m_pListBox->m_wndScrollBar.GetScrollPage() + m_pListBox->m_wndScrollBar.GetScrollPos()))
			break;

		CItemElem* pItemElem = pVec->at(i);
		ItemProp* pProp = pItemElem->GetProp();

		p2DRender->RenderLine(CPoint(rectItem.right, rectItem.top), CPoint(rectItem.right, rectItem.bottom), 0xff000000);

		if (i == m_pListBox->GetCurSel() && !IsListboxBgState())
			p2DRender->RenderFillRect(CRect(rectItem.left + 1, rectItem.top, rectItem.right, rectItem.bottom), 0xFFbef5b0);

		if (pItemElem->m_pTexture)
		{
			pointTexture.x = rectItem.right - rectItem.Width() / 2 - pItemElem->m_pTexture->m_size.cx / 2;
			pointTexture.y = rectItem.bottom - rectItem.Height() / 2 - pItemElem->m_pTexture->m_size.cy / 2;
			pItemElem->m_pTexture->Render(p2DRender, pointTexture);
		}

		if (pProp && pProp->dwPackMax > 1)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", pItemElem->m_nItemNum);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(pointTexture.x + 30 - size.cx, pointTexture.y + 30 - size.cy + 4, szTemp, 0xffeaebdd);
		}
		else if (pItemElem->GetAbilityOption() > 0)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%+d", pItemElem->GetAbilityOption());
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(pointTexture.x + 30 - size.cx, pointTexture.y - 2, szTemp, 0xffeaebdd);
		}

		CRect rectToolTip = rectItem;
		if (rectToolTip.PtInRect(m_pListBox->m_ptMouse) && m_pWndOnMouseMove == m_pListBox)
		{
			CPoint pt2 = m_pListBox->m_ptMouse;
			m_pListBox->ClientToScreen(&pt2);
			m_pListBox->ClientToScreen(&rectToolTip);
			g_WndMng.PutToolTip_Item(pItemElem, pt2, &rectToolTip, GetWndId());
		}

		rectItem.left += m_pListBox->m_nFontHeight;
		rectItem.right += m_pListBox->m_nFontHeight;
	}

	p2DRender->SetFont(pOldFont);
}
BOOL CWndPackedItems::Process()
{
	if (!m_pItemElem)
	{
		m_pButtonUse->EnableWindow(FALSE);
		m_pButtonRestore->EnableWindow(FALSE);
		return TRUE;
	}

	BOOL bUseButtonState, bRestoreButtonState;
	bUseButtonState = bRestoreButtonState = TRUE;

	if (m_pItemElem->GetItemList()->empty())
		bUseButtonState = FALSE;

	if (m_pListBox->GetCurSel() == -1)
		bRestoreButtonState = FALSE;

	m_pButtonUse->EnableWindow(bUseButtonState);
	m_pButtonRestore->EnableWindow(bRestoreButtonState);

	if (IsListboxBgState())
	{
		m_dwHighAlpha += 5;
		if (m_dwHighAlpha >= 256)
			m_dwHighAlpha = 0;
	}

	return TRUE;
}
void CWndPackedItems::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pListBox->m_nFontHeight = 40;
	m_pButtonUse = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	m_pButtonRestore = (CWndButton*)GetDlgItem(WIDC_BUTTON3);

	MoveParentCenter();
}

BOOL CWndPackedItems::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_PACKED_ITEMS);
}
void CWndPackedItems::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndPackedItems::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (!m_pItemElem)
		return CWndNeuz::OnChildNotify(message, nID, pLResult);

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON1: g_DPlay.SendPackageOpen(m_pItemElem->m_dwObjId); break;
		case WIDC_BUTTON3:
		{
			if (m_pListBox->GetCurSel() != -1)
			{
				g_DPlay.SendPackageRestore(m_pItemElem->m_dwObjId, m_pListBox->GetCurSel());
				m_pListBox->SetCurSel(-1);
			}
		}
		break;
		default: break;
		}
	}
	else if (message == WIN_ITEMDROP)
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		if (!lpShortcut || !lpShortcut->m_pFromWnd)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		if (!pWndFrame || pWndFrame->GetWndId() != APP_INVENTORY)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		if (nID == WIDC_LISTBOX)
		{
			CItemElem* pItemElem = (CItemElem*)lpShortcut->m_dwData;
			if (IsUsableItem(pItemElem) && IsListboxBgState())
				g_DPlay.SendPackageAdd(m_pItemElem->m_dwObjId, pItemElem->m_dwObjId);
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndPackedItems::OnMouseWndSurface(CPoint point)
{
	if (!m_pItemElem)
		return;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_CHANGE);
	CRect DrawRect = wndCtrl->rect;
	if (DrawRect.PtInRect(point))
	{
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);
		g_WndMng.PutToolTip_Item(m_pItemElem, point2, &DrawRect, GetWndId());
	}
}
void CWndPackedItems::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
void  CWndPackedItems::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rect = GetWndCtrl(WIDC_CHANGE)->rect;
	if (rect.PtInRect(point))
		ClearItem();
}
BOOL  CWndPackedItems::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	SetItem(pTempElem);

	return TRUE;
}
bool CWndPackedItems::IsListboxBgState()
{
	if (!m_pItemElem)
		return false;

	if (m_pItemElem->GetItemList()->size() >= MAX_ITEMS_IN_PACKAGE)
		return false;

	if (m_GlobalShortcut.IsEmpty())
		return false;

	if (m_GlobalShortcut.m_dwShortcut != SHORTCUT_ITEM)
		return false;

	return true;
}
bool CWndPackedItems::SetItem(CItemElem* pItemElem)
{
	if (!IsUsableItem(pItemElem) || !pItemElem->IsPackage())
		return false;

	ClearItem();
	m_pItemElem = pItemElem;
	m_pItemElem->SetExtra(m_pItemElem->m_nItemNum);
	RefreshListBox();

	return true;
}
void CWndPackedItems::ClearItem()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}

	m_pItemElem = nullptr;
	RefreshListBox();
}
void CWndPackedItems::RefreshListBox()
{
	if (!m_pItemElem)
		return;

	m_pListBox->ResetContent();
	for (auto& it : *m_pItemElem->GetItemList())
		m_pListBox->AddString("");
}



CWndPackedItemsPreview::CWndPackedItemsPreview()
	: m_pListBox(nullptr)
{

}

CWndPackedItemsPreview::~CWndPackedItemsPreview()
{

}

void CWndPackedItemsPreview::OnDraw(C2DRender* p2DRender)
{

}
void CWndPackedItemsPreview::DrawItemlist(C2DRender* p2DRender)
{
	const auto pVec = m_ItemElem.GetItemList();

	const CRect rectClient = m_pListBox->GetClientRect();
	const int nPage = rectClient.Width() / m_pListBox->m_nFontHeight;
	m_pListBox->m_wndScrollBar.SetScrollRange(0, pVec->size());
	m_pListBox->m_wndScrollBar.SetScrollPage(nPage);

	CRect rectItem(3, rectClient.top, 3 + m_pListBox->m_nFontHeight, rectClient.bottom);
	CPoint pointTexture = CPoint(0, 0);

	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

	for (int i = m_pListBox->m_wndScrollBar.GetScrollPos(); i < (int)pVec->size(); ++i)
	{
		if (i > (m_pListBox->m_wndScrollBar.GetScrollPage() + m_pListBox->m_wndScrollBar.GetScrollPos()))
			break;

		CItemElem* pItemElem = pVec->at(i);
		ItemProp* pProp = pItemElem->GetProp();

		p2DRender->RenderLine(CPoint(rectItem.right, rectItem.top), CPoint(rectItem.right, rectItem.bottom), 0xff000000);

		if (i == m_pListBox->GetCurSel())
			p2DRender->RenderFillRect(CRect(rectItem.left + 1, rectItem.top, rectItem.right, rectItem.bottom), 0xFFbef5b0);

		if (pItemElem->m_pTexture)
		{
			pointTexture.x = rectItem.right - rectItem.Width() / 2 - pItemElem->m_pTexture->m_size.cx / 2;
			pointTexture.y = rectItem.bottom - rectItem.Height() / 2 - pItemElem->m_pTexture->m_size.cy / 2;
			pItemElem->m_pTexture->Render(p2DRender, pointTexture);
		}

		if (pProp && pProp->dwPackMax > 1)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", pItemElem->m_nItemNum);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(pointTexture.x + 30 - size.cx, pointTexture.y + 30 - size.cy + 4, szTemp, 0xffeaebdd);
		}
		else if (pItemElem->GetAbilityOption() > 0)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%+d", pItemElem->GetAbilityOption());
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(pointTexture.x + 30 - size.cx, pointTexture.y - 2, szTemp, 0xffeaebdd);
		}

		CRect rectToolTip = rectItem;
		if (rectToolTip.PtInRect(m_pListBox->m_ptMouse) && m_pWndOnMouseMove == m_pListBox)
		{
			CPoint pt2 = m_pListBox->m_ptMouse;
			m_pListBox->ClientToScreen(&pt2);
			m_pListBox->ClientToScreen(&rectToolTip);
			g_WndMng.PutToolTip_Item(pItemElem, pt2, &rectToolTip, GetWndId());
		}

		rectItem.left += m_pListBox->m_nFontHeight;
		rectItem.right += m_pListBox->m_nFontHeight;
	}

	p2DRender->SetFont(pOldFont);
}
void CWndPackedItemsPreview::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pListBox = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pListBox->m_nFontHeight = 40;

	MoveParentCenter();
}

BOOL CWndPackedItemsPreview::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_PACKED_ITEMS_VIEW);
}
bool CWndPackedItemsPreview::SetItem(CItemElem* pItemElem)
{
	if (!pItemElem || !pItemElem->IsPackage())
		return false;

	m_ItemElem = *pItemElem;

	return true;
}
#endif

#ifdef __FL_WHEEL_OF_FORTUNE
CWndWof::CWndWof()
	: m_nWndState(STATE_NONE)
	, m_pWndBtnInit(nullptr)
	, m_pWndBtnClose(nullptr)
	, m_pTexFLGray(nullptr)
	, m_pTexFLYellow(nullptr)
	, m_pTexArrow(nullptr)
	, m_pTexItemBg(nullptr)
	, m_arrowAngle(D3DXToRadian(static_cast<double>(rand() % 360)))
	, m_arrowDistance(0.0)
	, m_initialArrowDistance(0.0)
	, m_nLampStateNoneRenderMode(0)
	, m_dwTimerChangeLampMode(0)
	, m_nTotalLampModeChanges(0)
	, m_bLampGoToNext(true)
	, m_nCurrentYellowLamp(0)
	, m_dwTimerChangeLamp(0)
	, m_bWinItemFlash(true)
	, m_dwTimerChangeWinItemFlash(0)
	, m_nTotalItemFlashChanges(0)
	, m_pWndListChances(nullptr)
{
	SetPutRegInfo(FALSE);
}

CWndWof::~CWndWof()
{
	g_DPlay.SendHdr(PACKETTYPE_WOF_FINISH);
	m_vecSortedItem.clear();
}

void CWndWof::OnDraw(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();

	// Render window bg
	CRect rect = GetWindowRect();
	CRect rectFrame = rect;
	rect.DeflateRect(1, 1, 1, 1);


	// Render title for listbox chances
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	rect = pWndStatic->GetWndRect();
	rectFrame = rect;
	rect.DeflateRect(1, 1, 1, 1);


	DrawWheel(p2DRender);
	DrawTicket(p2DRender);
	DrawHistory(p2DRender);

	// Render arrow
	CPoint arrowPoint = CENTER;
	arrowPoint.x -= m_pTexArrow->m_size.cx / 2;
	arrowPoint.y -= m_pTexArrow->m_size.cy / 2;
	m_pTexArrow->RenderRotate(p2DRender, arrowPoint, m_arrowAngle, TRUE);

	p2DRender->SetFont(pOldFont);
}
void CWndWof::DrawWheel(C2DRender* p2DRender)
{
	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

	const auto pVec = CWheelOfFortune::GetInstance()->GetItemList();
	const CPoint mousePoint = GetMousePoint();

	bool bSkipNextTex = false;

	for (int i = 0; i < pVec->size(); i++)
	{
		WOF_ITEM* item = &pVec->at(i);

		// Render triangles
		double angle1 = GetAngle(i);
		double angle2 = GetAngle(i + 1);

		CPoint point1(CENTER.x + RADIUS * cos(angle1), CENTER.y + RADIUS * sin(angle1));
		CPoint point2(CENTER.x + RADIUS * cos(angle2), CENTER.y + RADIUS * sin(angle2));

		p2DRender->RenderFillTriangle(CENTER, point1, point2,
			CWheelOfFortune::GetInstance()->GetColor(true, item->dwColor),
			CWheelOfFortune::GetInstance()->GetColor(false, item->dwColor),
			CWheelOfFortune::GetInstance()->GetColor(false, item->dwColor));

		p2DRender->RenderTriangle(CENTER, point1, point2,
			D3DCOLOR_ARGB(55, 0, 0, 0), D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR_ARGB(255, 0, 0, 0));

		bool bArrowInThisZone = false;
		if (m_arrowAngle >= angle1 && m_arrowAngle < angle2)
			bArrowInThisZone = true;

		// Render lamps
		double angleTex1 = angle1 - 0.035;
		double angleTex2 = angle2 - 0.035;

		CPoint pointTex1(CENTER.x + RADIUS * cos(angleTex1), CENTER.y + RADIUS * sin(angleTex1));
		CPoint pointTex2(CENTER.x + RADIUS * cos(angleTex2), CENTER.y + RADIUS * sin(angleTex2));

		if (m_nWndState == STATE_NONE)
		{
			bool bYellow = false;
			if (m_nLampStateNoneRenderMode == 0)
			{
				if (i == m_nCurrentYellowLamp)
					bYellow = true;
			}
			else if (m_nLampStateNoneRenderMode == 1)
			{
				if (i % 2)
					bYellow = true;
			}
			else
			{
				if (!(i % 2))
					bYellow = true;
			}

			if (bYellow)
				m_pTexFLYellow->RenderRotate(p2DRender, pointTex1, angleTex1);
			else
				m_pTexFLGray->RenderRotate(p2DRender, pointTex1, angleTex1);
		}
		else
		{
			if (bArrowInThisZone)
			{
				m_pTexFLYellow->RenderRotate(p2DRender, pointTex1, angleTex1);
				m_pTexFLYellow->RenderRotate(p2DRender, pointTex2, angleTex2);
				bSkipNextTex = true;
			}
			else
			{
				if (!bSkipNextTex)
					m_pTexFLGray->RenderRotate(p2DRender, pointTex1, angleTex1);

				bSkipNextTex = false;
			}
		}

		// Render item bg, texture, count and tooltip
		DWORD dwAlpha = 255;
		if (m_nWndState != STATE_NONE && !bArrowInThisZone)
			dwAlpha = 125;

		if (m_nWndState == STATE_FINISH)
		{
			if (bArrowInThisZone && m_bWinItemFlash)
				dwAlpha = 255;
			else
				dwAlpha = 125;
		}

		double angleItemBg = angle2 - ((angle2 - angle1) / 2.0);
		CPoint pointItemBg(CENTER.x + RADIUS_ITEMS * cos(angleItemBg), CENTER.y + RADIUS_ITEMS * sin(angleItemBg));
		pointItemBg.x -= m_pTexItemBg->m_size.cx / 2;
		pointItemBg.y -= m_pTexItemBg->m_size.cy / 2;
		p2DRender->RenderTexture(pointItemBg, m_pTexItemBg, dwAlpha);

		if (item->pTexture)
			p2DRender->RenderTexture(pointItemBg, item->pTexture, dwAlpha);
		else
		{
			ItemProp* pProp = prj.GetItemProp(item->dwItemId);
			if (pProp)
				item->pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		}

		if (item->nItemCount > 1)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", item->nItemCount);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(pointItemBg.x + m_pTexItemBg->m_size.cx - size.cx - 4, pointItemBg.y + m_pTexItemBg->m_size.cy - size.cy, szTemp, D3DCOLOR_ARGB(dwAlpha, 234, 235, 221));
		}

		CRect rectItem = CRect(pointItemBg.x, pointItemBg.y, pointItemBg.x + m_pTexItemBg->m_size.cx, pointItemBg.y + m_pTexItemBg->m_size.cy);
		if (rectItem.PtInRect(mousePoint))
		{
			CItemElem itemElem;
			itemElem.m_dwItemId = item->dwItemId;
			itemElem.m_nItemNum = item->nItemCount;
			if (item->bBinded)
				itemElem.SetFlag(CItemElem::binds);

			CPoint point2 = mousePoint;
			ClientToScreen(&point2);
			ClientToScreen(&rectItem);
			g_WndMng.PutToolTip_Item(&itemElem, point2, &rectItem, GetWndId());
		}
	}
}
void CWndWof::DrawTicket(C2DRender* p2DRender)
{
	const CPoint mousePoint = GetMousePoint();

	CRect rectButton = m_pWndBtnInit->GetWndRect();
	CRect rectTicket(rectButton.left - m_pTexItemBg->m_size.cx - 6, rectButton.top - 17, rectButton.left - 6, rectButton.top - 17 + m_pTexItemBg->m_size.cy);
	CPoint ptTicket = rectTicket.TopLeft();

	if (CWheelOfFortune::GetInstance()->GetTicketTexture())
		p2DRender->RenderTexture(ptTicket, CWheelOfFortune::GetInstance()->GetTicketTexture());
	else
	{
		ItemProp* pProp = prj.GetItemProp(CWheelOfFortune::GetInstance()->GetTicket());
		if (pProp)
			CWheelOfFortune::GetInstance()->SetTicketTexture(CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff));
	}

	p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
	CString str; str.Format("%d", CWheelOfFortune::GetInstance()->GetTicketCount());
	CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
	m_p2DRender->TextOut(ptTicket.x + m_pTexItemBg->m_size.cx - size.cx - 4, ptTicket.y + m_pTexItemBg->m_size.cy - size.cy, str, D3DCOLOR_ARGB(255, 234, 235, 221));

	if (rectTicket.PtInRect(mousePoint))
	{
		CItemElem itemElem;
		itemElem.m_dwItemId = CWheelOfFortune::GetInstance()->GetTicket();
		itemElem.m_nItemNum = CWheelOfFortune::GetInstance()->GetTicketCount();

		CPoint point2 = mousePoint;
		ClientToScreen(&point2);
		ClientToScreen(&rectTicket);
		g_WndMng.PutToolTip_Item(&itemElem, point2, &rectTicket, GetWndId());
	}

	p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);
	str.Format("attempts: %d", g_pPlayer->GetItemNum(CWheelOfFortune::GetInstance()->GetTicket()) / CWheelOfFortune::GetInstance()->GetTicketCount());
	size = m_p2DRender->m_pFont->GetTextExtent(str);
	m_p2DRender->TextOut(rectButton.left, rectButton.top - size.cy - 3, str, 0xff000000);

}
void CWndWof::DrawChances(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();

	const CRect rectClient = m_pWndListChances->GetClientRect();
	const CPoint mousePoint = m_pWndListChances->GetMousePoint();

	m_pWndListChances->m_nFontHeight = 40;
	const int nPage = rectClient.Height() / m_pWndListChances->m_nFontHeight;
	m_pWndListChances->m_wndScrollBar.SetScrollRange(0, m_vecSortedItem.size());
	m_pWndListChances->m_wndScrollBar.SetScrollPage(nPage);

	DWORD dwColor = 0xff000000;
	CString str;
	CRect rc = CRect(3, 3, 3 + rectClient.Width(), 3 + m_pWndListChances->m_nFontHeight);
	CRect rcItem = rc;
	rcItem.right = rcItem.left + 32;
	rcItem.bottom = rcItem.top + 32;

	CSize sizeText = m_p2DRender->m_pFont->GetTextExtent("123");
	CPoint ptName(rcItem.right + 5, rcItem.bottom - rcItem.Height() / 2 - sizeText.cy / 2);
	CPoint ptChance = CPoint(ptName.x + 90, ptName.y);

	for (int i = m_pWndListChances->m_wndScrollBar.GetScrollPos(); i < m_pWndListChances->m_wndScrollBar.GetMaxScrollPos(); ++i)
	{
		if (i > (nPage + m_pWndListChances->m_wndScrollBar.GetScrollPos()))
			break;

		WOF_ITEM* pItem = &m_vecSortedItem.at(i);

		if (pItem->pTexture)
			p2DRender->RenderTexture(CPoint(rcItem.left, rcItem.top), pItem->pTexture);
		else
		{
			ItemProp* pProp = prj.GetItemProp(pItem->dwItemId);
			if (pProp)
				pItem->pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		}

		if (pItem->nItemCount > 1)
		{
			str.Format("%d", pItem->nItemCount);
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(str);
			m_p2DRender->TextOut(rcItem.right - size.cx - 4, rcItem.bottom - size.cy, str, D3DCOLOR_ARGB(255, 234, 235, 221));
		}

		CRect rectToolTip = rcItem;
		if (rectToolTip.PtInRect(mousePoint))
		{
			CPoint pt2 = mousePoint;
			m_pWndListChances->ClientToScreen(&pt2);
			m_pWndListChances->ClientToScreen(&rectToolTip);
			CItemElem Item;
			Item.m_dwItemId = pItem->dwItemId;
			Item.m_nItemNum = pItem->nItemCount;
			if (pItem->bBinded)
				Item.SetFlag(CItemElem::binds);

			g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, GetWndId());
		}

		ItemProp* pProp = prj.GetItemProp(pItem->dwItemId);
		if (pProp)
		{
			str.Format("%s", pProp->szName);
			g_WndMng.StringSize(str, 8);
			p2DRender->SetFont(pOldFont);
			m_p2DRender->TextOut(ptName.x, ptName.y, str, dwColor);
		}

		DWORD dwProb = pItem->dwChance;
		str.Format("%.4f%%", (float)dwProb / 10000000.0f);
		m_p2DRender->TextOut(ptChance.x, ptChance.y, str, dwColor);

		rc.top += m_pWndListChances->m_nFontHeight;
		rc.bottom += m_pWndListChances->m_nFontHeight;
		rcItem.top += m_pWndListChances->m_nFontHeight;
		rcItem.bottom += m_pWndListChances->m_nFontHeight;
		ptName.y += m_pWndListChances->m_nFontHeight;
		ptChance.y += m_pWndListChances->m_nFontHeight;
	}

	p2DRender->SetFont(pOldFont);
}
void CWndWof::DrawHistory(C2DRender* p2DRender)
{
	const CPoint mousePoint = GetMousePoint();

	// Title
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	CRect rect = pWndStatic->GetWndRect();
	CRect rectFrame = rect;
	rect.DeflateRect(1, 1, 1, 1);


	// Items icons and count
	pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	rect = pWndStatic->GetWndRect();

	const auto pList = CWheelOfFortune::GetInstance()->GetRecentRewardsList();
	const int nWidth = rect.Width() / 32;

	CPoint ptItem(rect.left + 5, rect.top + 3);

	int i = 0;
	for (auto& it : *pList)
	{
		if (it.pTexture)
			p2DRender->RenderTexture(ptItem, it.pTexture);
		else
		{
			ItemProp* pProp = prj.GetItemProp(it.dwItemId);
			if (pProp)
				it.pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
		}

		if (it.nItemCount > 1)
		{
			TCHAR szTemp[32];
			_stprintf(szTemp, "%d", it.nItemCount);
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(ptItem.x + m_pTexItemBg->m_size.cx - size.cx - 4, ptItem.y + m_pTexItemBg->m_size.cy - size.cy, szTemp, D3DCOLOR_ARGB(255, 234, 235, 221));
		}

		CRect rcItem(ptItem.x, ptItem.y, ptItem.x + 32, ptItem.y + 32);
		if (rcItem.PtInRect(mousePoint))
		{
			CPoint pt2 = mousePoint;
			ClientToScreen(&pt2);
			ClientToScreen(&rcItem);
			CItemElem Item;
			Item.m_dwItemId = it.dwItemId;
			Item.m_nItemNum = it.nItemCount;
			if (it.bBinded)
				Item.SetFlag(CItemElem::binds);

			g_WndMng.PutToolTip_Item(&Item, pt2, &rcItem, GetWndId());
		}

		i++;

		if (i % nWidth)
			ptItem.x += 32;
		else
		{
			ptItem.x = rect.left + 5;
			ptItem.y += 32;
		}
	}
}
BOOL CWndWof::Process()
{
	ProcessLamp();
	ProcessGame();
	ProcessItemFlash();

	BOOL bBtnInitState = TRUE;
	BOOL bBtnCloseState = TRUE;

	if (m_nWndState != STATE_NONE)
	{
		bBtnInitState = FALSE;
		bBtnCloseState = FALSE;
	}

	if (g_pPlayer->GetItemNum(CWheelOfFortune::GetInstance()->GetTicket()) < CWheelOfFortune::GetInstance()->GetTicketCount())
		bBtnInitState = FALSE;

	m_pWndBtnInit->EnableWindow(bBtnInitState);
	m_pWndBtnClose->EnableWindow(bBtnCloseState);

	return TRUE;
}
void CWndWof::ProcessLamp()
{
	if (m_nWndState != STATE_NONE)
		return;

	// Lamp Mode Process
	if (m_nLampStateNoneRenderMode)
	{
		if (GetTickCount() >= m_dwTimerChangeLampMode)
		{
			if (m_nTotalLampModeChanges < 5)
			{
				m_nLampStateNoneRenderMode = m_nLampStateNoneRenderMode == 1 ? 2 : 1;
				m_dwTimerChangeLampMode = GetTickCount() + 700;
				m_nTotalLampModeChanges++;
			}
			else
			{
				m_nLampStateNoneRenderMode = 0;
				m_nTotalLampModeChanges = 0;
				m_dwTimerChangeLamp = GetTickCount() + 100;
			}
		}
	}
	else
	{
		// Lamp Process
		if (GetTickCount() >= m_dwTimerChangeLamp)
		{
			bool bSwitchMode = false;
			if (m_bLampGoToNext)
			{
				m_nCurrentYellowLamp += 1;
				if (m_nCurrentYellowLamp >= (int)CWheelOfFortune::GetInstance()->GetItemList()->size())
				{
					m_nCurrentYellowLamp = CWheelOfFortune::GetInstance()->GetItemList()->size() - 1;
					m_bLampGoToNext = false;
					bSwitchMode = true;
				}
			}
			else
			{
				m_nCurrentYellowLamp -= 1;
				if (m_nCurrentYellowLamp < 0)
				{
					m_nCurrentYellowLamp = 0;
					m_bLampGoToNext = true;
					bSwitchMode = true;
				}
			}

			if (bSwitchMode)
			{
				m_nLampStateNoneRenderMode = 1;
				m_dwTimerChangeLampMode = GetTickCount() + 700;
			}
			else
				m_dwTimerChangeLamp = GetTickCount() + 100;
		}
	}
}
void CWndWof::ProcessGame()
{
	if (m_nWndState != STATE_INIT)
		return;

	if (m_arrowDistance > 0.0)
	{
		double speed = CalculateSpeed(0.31, m_arrowDistance, m_initialArrowDistance);
		if (speed > m_arrowDistance)
			speed = m_arrowDistance;

		m_arrowAngle += speed;
		m_arrowDistance -= speed;

		if (m_arrowAngle > (2.0 * D3DX_PI))
			m_arrowAngle = m_arrowAngle - (2.0 * D3DX_PI);

		if (m_arrowDistance == 0.0)
		{
			g_DPlay.SendHdr(PACKETTYPE_WOF_FINISH);
			SetState(STATE_WAIT_FINISH);
		}
	}
}
void CWndWof::ProcessItemFlash()
{
	if (m_nWndState != STATE_FINISH)
		return;

	if (GetTickCount() >= m_dwTimerChangeWinItemFlash)
	{
		m_bWinItemFlash = !m_bWinItemFlash;
		m_nTotalItemFlashChanges++;
		m_dwTimerChangeWinItemFlash = GetTickCount() + 333;

		if (m_nTotalItemFlashChanges > 6)
			SetState(STATE_NONE);
	}
}
void CWndWof::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	if (!CWheelOfFortune::GetInstance()->IsActive())
	{
		g_WndMng.PutString("Wheel of Fortune not available at this moment.");
		Destroy();
	}

	m_pWndBtnInit = (CWndButton*)GetDlgItem(WIDC_BUTTON);
	m_pWndBtnClose = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	m_pTexFLGray = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "wof1.png"), 0xffff00ff);
	m_pTexFLYellow = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "wof2.png"), 0xffff00ff);
	m_pTexArrow = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "wof3.png"), 0xffff00ff);
	m_pTexItemBg = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndChgElemItem.bmp"), 0xffff00ff);

	m_pWndListChances = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	ResetListChances();

	MoveParentCenter();
}

BOOL CWndWof::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_WOF);
}
BOOL CWndWof::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON)
		{
			g_DPlay.SendHdr(PACKETTYPE_WOF_INIT);
			SetState(STATE_WAIT_INIT);
		}
		else if (nID == WIDC_BUTTON1)
			Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndWof::SetState(int nState, int nVal)
{
	m_nWndState = nState;

	if (nState == STATE_INIT)
	{
		int nZones = CWheelOfFortune::GetInstance()->GetItemList()->size();

		const double prizeAngle = GetAngle(nVal);
		const double nextPrizeAngle = GetAngle(nVal + 1);

		const double r = (double)rand() / RAND_MAX;
		const double randomInBetween = prizeAngle + r * (nextPrizeAngle - prizeAngle - 0.01);

		double distanceBetweenCurrentAndFinish = 0.0;
		if (randomInBetween > m_arrowAngle)
			distanceBetweenCurrentAndFinish = randomInBetween - m_arrowAngle;
		else
			distanceBetweenCurrentAndFinish = (2.0 * D3DX_PI) - m_arrowAngle + randomInBetween;

		m_arrowDistance = static_cast<double>(4 + rand() % 3) * (2.0 * D3DX_PI) + distanceBetweenCurrentAndFinish;
		m_initialArrowDistance = m_arrowDistance;
	}
	else if (nState == STATE_FINISH)
	{
		m_bWinItemFlash = true;
		m_dwTimerChangeWinItemFlash = GetTickCount() + 333;
		m_nTotalItemFlashChanges = 0;
	}
	else if (nState == STATE_NONE)
	{
		ResetLamps();
	}
}
double CWndWof::GetAngle(int nZone)
{
	const int zones = CWheelOfFortune::GetInstance()->GetItemList()->size();
	double angle = 2.0 * D3DX_PI * nZone / zones;
	return angle;
}
double CWndWof::CalculateSpeed(double initialSpeed, double remainingDistance, double totalDistance)
{
	double currentSpeed = initialSpeed * sqrt(remainingDistance / totalDistance);
	return currentSpeed;
}
void CWndWof::ResetLamps()
{
	m_nLampStateNoneRenderMode = 0;
	m_dwTimerChangeLampMode = 0;
	m_nTotalLampModeChanges = 0;
	m_bLampGoToNext = true;
	m_nCurrentYellowLamp = 0;
	m_dwTimerChangeLamp = 0;
}
bool SortByChance(WOF_ITEM& first, WOF_ITEM& second)
{
	return (first.dwChance > second.dwChance);
}

void CWndWof::ResetListChances()
{
	m_vecSortedItem.clear();
	m_vecSortedItem = *CWheelOfFortune::GetInstance()->GetItemList();
	std::sort(m_vecSortedItem.begin(), m_vecSortedItem.end(), SortByChance);

	m_pWndListChances->ResetContent();
	for (auto& it : m_vecSortedItem)
		m_pWndListChances->AddString("");
}
#endif

#ifdef __FL_RECORD_BOOK
CWndRecordBook::CWndRecordBook()
	: m_pWndTreeCategories(nullptr)
	, m_pListBoxRecords(nullptr)
	, m_pStaticPlayerName(nullptr)
	, m_fRot(0.0f)
	, m_bLButtonDownRot(false)
	, m_OldPos(CPoint(0, 0))
{
	for (int i = 0; i < 3; i++)
		m_pTexTop[i] = nullptr;
}

CWndRecordBook::~CWndRecordBook()
{
	m_mapKeyword.clear();
	SAFE_DELETE(m_Mover.m_pModel);
	CRecordBook::GetInstance()->Clear();
}

void CWndRecordBook::OnDraw(C2DRender* p2DRender)
{
	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM);
	VIEWPORT tmp;
	tmp.m_fRot = m_fRot;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);
	g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)m_Mover.m_pModel, &m_Mover, lpFace, &tmp, GetWndId());
}
BOOL CWndRecordBook::Process()
{
	// Cuz of playerdata it should be done in Process()
	if (m_pStaticPlayerName->GetTitle() == "")
	{
		if (m_pListBoxRecords->GetCurSel() != -1)
		{
			auto pVec = CRecordBook::GetInstance()->GetVectorRecords(GetCategoryByKeyword());
			if (pVec && m_pListBoxRecords->GetCurSel() < (int)pVec->size())
				m_pStaticPlayerName->SetTitle(CPlayerDataCenter::GetInstance()->GetPlayerString(pVec->at(m_pListBoxRecords->GetCurSel()).idPlayer));
		}
	}

	if (m_Mover.m_pModel)
		m_Mover.m_pModel->FrameMove();

	return TRUE;
}
void CWndRecordBook::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pWndTreeCategories = (CWndTreeCtrl*)GetDlgItem(WIDC_TREECTRL);
	m_pWndTreeCategories->LoadTreeScript(MakePath(DIR_CLIENT, "RecordBook.inc"));
	m_pListBoxRecords = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pStaticPlayerName = (CWndStatic*)GetDlgItem(WIDC_STATIC3);
	m_pStaticPlayerName->SetTitle("");

	m_mapKeyword["Most STR"] = RecordType::STR;
	m_mapKeyword["Most STA"] = RecordType::STA;
	m_mapKeyword["Most DEX"] = RecordType::DEX;
	m_mapKeyword["Most INT"] = RecordType::INTT;
	m_mapKeyword["Most ADOCH"] = RecordType::ADOCH;
	m_mapKeyword["Most HP"] = RecordType::HP;
	m_mapKeyword["Most DMG (PvP)"] = RecordType::PVP_DMG;
	m_mapKeyword["Most Asal (PvP)"] = RecordType::PVP_ASAL;
	m_mapKeyword["Most HoP (PvP)"] = RecordType::PVP_HOP;
	m_mapKeyword["Most Charged Bow (PvP)"] = RecordType::PVP_CHARGEBOW;
	m_mapKeyword["Most DMG (PvE)"] = RecordType::PVE_DMG;
	m_mapKeyword["Most Asal (PvE)"] = RecordType::PVE_ASAL;
	m_mapKeyword["Most HoP (PvE)"] = RecordType::PVE_HOP;
	m_mapKeyword["Most Charged Bow (PvE)"] = RecordType::PVE_CHARGEBOW;

	m_mapKeyword["Most Awakes"] = RecordType::AWAKES_COUNT;
	m_mapKeyword["Most Normal Upgrades"] = RecordType::NORMAL_UPGRADES;
	m_mapKeyword["Most Accessory Upgrades"] = RecordType::ACCE_UPGRADES;
	m_mapKeyword["Most Piercings"] = RecordType::PIERCING_UPGRADES;
	m_mapKeyword["Most Element Upgrades"] = RecordType::ELEMENT_UPGRADES;

	m_mapKeyword["Most Mobs Killed"] = RecordType::MOBS_KILLED;
	m_mapKeyword["Most Bosses Killed"] = RecordType::BOSSES_KILLED;
	m_mapKeyword["Most Players Killed"] = RecordType::PLAYERS_KILLED;
	m_mapKeyword["Most Deaths"] = RecordType::DEATHS;
	m_mapKeyword["Most MvP Earned"] = RecordType::MVP;
	m_mapKeyword["Most GW Won"] = RecordType::GW_WIN;
	m_mapKeyword["Most SR Won"] = RecordType::SR_WIN;
	m_mapKeyword["Most FFA Kills"] = RecordType::FFA_KILLS;
	m_mapKeyword["Most TDM Kills"] = RecordType::TDM_KILLS;

	m_mapKeyword["Most Playtime"] = RecordType::TOTAL_PLAYTIME;
	m_mapKeyword["Most Items Collected"] = RecordType::COLLECTOR_ITEMS;
	m_mapKeyword["Most Quests Finished"] = RecordType::QUESTS;
	m_mapKeyword["Most Dungeon Runs"] = RecordType::DUNGEON_RUNS;
	m_mapKeyword["Most Cheers Given"] = RecordType::CHEERS_GIVEN;
	m_mapKeyword["Most Cheers Received"] = RecordType::CHEERS_RECEIVED;
	m_mapKeyword["Most Wheel Attempts"] = RecordType::WOF_ATTEMPTS;


	m_pWndTreeCategories->FindTreeElem("Character")->m_bOpen = TRUE;
	m_pWndTreeCategories->FindTreeElem("Upgrade")->m_bOpen = TRUE;
	m_pWndTreeCategories->FindTreeElem("PvP & PvE")->m_bOpen = TRUE;
	m_pWndTreeCategories->FindTreeElem("Miscellaneous")->m_bOpen = TRUE;
	m_strKeyword = m_pWndTreeCategories->SetCurSel("Most STR")->m_strKeyword;
	RefreshListBox();

	g_DPlay.SendHdr(PACKETTYPE_RECORD_BOOK);

	CString str;
	for (int i = 0; i < 3; i++)
	{
		str.Format("recordbook_top%d.dds", i + 1);
		m_pTexTop[i] = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, str), 0xffff00ff);
	}

	MoveParentCenter();
}

BOOL CWndRecordBook::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_RECORD_BOOK);
}
BOOL CWndRecordBook::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == m_pWndTreeCategories->GetWndId())
		{
			LPTREEELEM lpTreeElem = (LPTREEELEM)pLResult;
			if (lpTreeElem)
			{
				if (lpTreeElem->m_strKeyword == m_strKeyword.c_str())
					return FALSE;

				m_strKeyword = lpTreeElem->m_strKeyword;
				RefreshListBox();
			}
		}
	}
	else if (message == WNM_SELCHANGE)
	{
		if (nID == m_pListBoxRecords->GetWndId())
			RefreshSelectedCharacter();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndRecordBook::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
		m_fRot += (m_OldPos.x - point.x) * 0.5f;

	m_OldPos = point;
}
void CWndRecordBook::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDownRot = false;
	ReleaseCapture();

}
void CWndRecordBook::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM);
	if (lpWndCtrl->rect.PtInRect(point))
	{
		m_OldPos = point;
		m_bLButtonDownRot = true;
	}
}
int CWndRecordBook::GetCategoryByKeyword()
{
	auto it = m_mapKeyword.find(m_strKeyword);
	if (it != m_mapKeyword.end())
		return it->second;

	return -1;
}
void CWndRecordBook::RefreshListBox()
{
	m_pListBoxRecords->ResetContent();

	auto pVec = CRecordBook::GetInstance()->GetVectorRecords(GetCategoryByKeyword());
	if (pVec)
	{
		for (auto& it : *pVec)
			m_pListBoxRecords->AddString("");

		if (m_pListBoxRecords->GetCount() > 0)
			m_pListBoxRecords->SetCurSel(0);
	}

	RefreshSelectedCharacter();
}
void CWndRecordBook::DrawListBox(C2DRender* p2DRender)
{
	const int nCategory = GetCategoryByKeyword();
	auto pVec = CRecordBook::GetInstance()->GetVectorRecords(nCategory);
	if (!pVec)
		return;

	const CRect rectClient = m_pListBoxRecords->GetClientRect();

	m_pListBoxRecords->m_nFontHeight = 22;
	const int nPage = rectClient.Height() / m_pListBoxRecords->m_nFontHeight;
	m_pListBoxRecords->m_wndScrollBar.SetScrollRange(0, pVec->size());
	m_pListBoxRecords->m_wndScrollBar.SetScrollPage(nPage);

	DWORD dwColor = 0xff000000;
	CString str;
	CRect rc = CRect(3, 3, 3 + rectClient.Width(), 3 + m_pListBoxRecords->m_nFontHeight);

	CSize sizeText = m_p2DRender->m_pFont->GetTextExtent("123");
	CPoint ptText(3, rc.bottom - rc.Height() / 2 - sizeText.cy / 2);

	CPoint ptLine(0, rc.bottom - 1);

	for (int i = m_pListBoxRecords->m_wndScrollBar.GetScrollPos(); i < m_pListBoxRecords->m_wndScrollBar.GetMaxScrollPos(); ++i)
	{
		if (i > (nPage + m_pListBoxRecords->m_wndScrollBar.GetScrollPos()))
			break;

		PlayerRecord* pItem = &pVec->at(i);

		if (i == m_pListBoxRecords->GetCurSel())
			p2DRender->RenderFillRect(rc, 0xFFbef5b0);
		p2DRender->RenderLine(CPoint(0, ptLine.y), CPoint(rectClient.right, ptLine.y), dwColor);

		// Rank
		if (i >= 0 && i < 3)
		{
			if (m_pTexTop[i])
				p2DRender->RenderTexture(CPoint(ptText.x, ptText.y - 2), m_pTexTop[i], 255, 0.6f, 0.6f);
		}
		else
		{
			str.Format("#%d", i + 1);
			p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);
		}

		// Name
		ptText.x = 35;
		str.Format("%s", CPlayerDataCenter::GetInstance()->GetPlayerString(pItem->idPlayer));
		//g_WndMng.StringSize(str, 10);
		p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);

		// Level
		ptText.x = 150;
		if (MAX_PROFESSIONAL <= pItem->nJob && pItem->nJob < MAX_MASTER)
			str.Format("%d-M", pItem->nLevel);
		else if (MAX_MASTER <= pItem->nJob)
			str.Format("%d-H", pItem->nLevel);
		else if (MAX_HERO <= pItem->nJob)
			str.Format("%d-H", pItem->nLevel);
		else
			str.Format("%d", pItem->nLevel);
		p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);

		// Job
		ptText.x = 195;
		if (prj.m_aJob[pItem->nJob].dwJobType == JTYPE_PRO)
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pItem->nJob - 6), 0xffffffff, 0.9f, 0.85f);
		else if (prj.m_aJob[pItem->nJob].dwJobType == JTYPE_MASTER)
		{
			int nMasterIndex = 27;
			if (pItem->nLevel < 70)
				nMasterIndex = 27;
			else if (pItem->nLevel >= 70 && pItem->nLevel < 80)
				nMasterIndex = 28;
			else if (pItem->nLevel >= 80 && pItem->nLevel < 90)
				nMasterIndex = 29;
			else if (pItem->nLevel >= 90 && pItem->nLevel < 100)
				nMasterIndex = 30;
			else if (pItem->nLevel >= 100 && pItem->nLevel < 110)
				nMasterIndex = 31;
			else if (pItem->nLevel >= 110 && pItem->nLevel <= 120)
				nMasterIndex = 32;

			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, nMasterIndex, 0xffffffff, 0.9f, 0.85f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pItem->nJob - 16), 0xffffffff, 0.9f, 0.85f);
		}
		else if (prj.m_aJob[pItem->nJob].dwJobType == JTYPE_HERO)
		{
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 33, 0xffffffff, 0.9f, 0.9f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pItem->nJob - 24), 0xffffffff, 0.9f, 0.85f);
		}
		else if (prj.m_aJob[pItem->nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 33, 0xffffffff, 0.9f, 0.9f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pItem->nJob - 32), 0xffffffff, 0.9f, 0.85f);
		}
		else
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 14 + pItem->nJob, 0xffffffff, 0.9f, 0.85f);

		// Value
		if (!IsTimeValue(nCategory))
			str.Format("%s", Separator(pItem->n64Value));
		else
		{
			CTimeSpan time(pItem->n64Value);
			if (time.GetDays() != 0)
				str.Format("%.2d Day. %.2d Hr.", static_cast<int>(time.GetDays()), time.GetHours());
			else
			{
				if (time.GetHours() >= 1)
					str.Format("%.2d Hr. %.2d Min.", time.GetHours(), time.GetMinutes());
				else
					str.Format("%.2d Min. %.2d Sec.", time.GetMinutes(), time.GetSeconds());
			}
		}

		sizeText = m_p2DRender->m_pFont->GetTextExtent(str);
		ptText.x = rc.right - 10 - sizeText.cx;
		p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);

		rc.top += m_pListBoxRecords->m_nFontHeight;
		rc.bottom += m_pListBoxRecords->m_nFontHeight;
		ptLine.y += m_pListBoxRecords->m_nFontHeight;
		ptText.y += m_pListBoxRecords->m_nFontHeight;
		ptText.x = 3;
	}
}
bool CWndRecordBook::IsTimeValue(int nRecordType)
{
	switch (nRecordType)
	{
	case RecordType::TOTAL_PLAYTIME: return true;
	default: return false;
	}

	return false;
}
void CWndRecordBook::RefreshSelectedCharacter()
{
	m_pStaticPlayerName->SetTitle("");
	SAFE_DELETE(m_Mover.m_pModel);

	auto pVectorRecords = CRecordBook::GetInstance()->GetVectorRecords(GetCategoryByKeyword());
	if (!pVectorRecords)
		return;

	if (m_pListBoxRecords->GetCurSel() == -1 || m_pListBoxRecords->GetCurSel() >= (int)pVectorRecords->size())
		return;

	auto it = CRecordBook::GetInstance()->GetPlayerEquipments()->find(pVectorRecords->at(m_pListBoxRecords->GetCurSel()).idPlayer);
	if (it == CRecordBook::GetInstance()->GetPlayerEquipments()->end())
		return;

	int nMover = it->second.nSex == SEX_MALE ? MI_MALE : MI_FEMALE;
	m_Mover.m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_Mover.m_pModel, OT_MOVER, nMover, MTI_STAND);

	m_Mover.SetSex(it->second.nSex);
	m_Mover.m_nJob = it->second.nJob;
	m_Mover.SetHair(it->second.dwHairMesh);
	m_Mover.SetHairColor(it->second.dwHairColor);
	m_Mover.SetHead(it->second.dwHeadMesh);
	memcpy(m_Mover.m_aEquipInfo, it->second.aEquipInfo, sizeof(m_Mover.m_aEquipInfo));

	CMover::UpdateParts(m_Mover.GetSex(), m_Mover.m_dwHairMesh, m_Mover.m_dwHeadMesh, m_Mover.m_aEquipInfo, (CModelObject*)m_Mover.m_pModel, NULL);

	m_Mover.m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}
#endif
#ifdef __FL_MONSTER_ALBUM
CWndMonsterAlbum::CWndMonsterAlbum()
	: m_pListBoxMonsters(nullptr)
	, m_pWndBtnHideUnlocked(nullptr)
	, m_wndCtrlProgressBar(NULL)
	, m_pLockTexture(nullptr)
	, m_pRewardTexture(nullptr)
	, m_dwHighAlpha(0)
	//, m_pWndTextStats(nullptr)
	, m_pWndTextDescription(nullptr)
{
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		m_wndCtrl[i] = nullptr;
		m_pBossCardTexture[i] = nullptr;
	}
}

CWndMonsterAlbum::~CWndMonsterAlbum()
{
	for (auto& it : m_mapMonsterModels)
		SAFE_DELETE(it.second.pModelObject);

	m_mapMonsterModels.clear();
	m_vecSortedMonsters.clear();
}
void CWndMonsterAlbum::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);

	if (ar.IsLoading())
	{
		int nCheck;
		ar >> nCheck;

		int nOldCheck = m_pWndBtnHideUnlocked->GetCheck();
		m_pWndBtnHideUnlocked->SetCheck(nCheck);

		if (nOldCheck != nCheck)
			UpdateMonsterList();
	}
	else
	{
		ar << m_pWndBtnHideUnlocked->GetCheck();
	}
}
void CWndMonsterAlbum::OnDraw(C2DRender* p2DRender)
{
	DrawProgressBar(p2DRender);

	ItemProp* pBossCardProp = nullptr;
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		const DWORD dwBossCard = g_pPlayer->m_MonsterAlbum.GetBossCards()->at(i);
		if (dwBossCard == NULL_ID)
		{
			if (m_pLockTexture)
				m_pLockTexture->Render(p2DRender, CPoint(m_wndCtrl[i]->rect.left, m_wndCtrl[i]->rect.top));
		}
		else if (dwBossCard > 0)
		{
			if (m_pBossCardTexture[i])
				m_pBossCardTexture[i]->Render(p2DRender, CPoint(m_wndCtrl[i]->rect.left, m_wndCtrl[i]->rect.top));

			if (IsBossCardSlotBgState())
			{
				CRect rectGreen = m_wndCtrl[i]->rect;
				rectGreen.right -= 2;
				rectGreen.bottom -= 2;

				DWORD dwColor;
				if (m_dwHighAlpha < 128)
					dwColor = D3DCOLOR_ARGB(m_dwHighAlpha, 0, 255, 0);
				else
					dwColor = D3DCOLOR_ARGB(128 - (m_dwHighAlpha - 128), 0, 255, 0);

				p2DRender->RenderFillRect(rectGreen, dwColor);
			}
		}
	}
}
void CWndMonsterAlbum::DrawProgressBar(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();
	CPoint ptMouse = GetMousePoint();
	DWORD dwTextColor = 0xff000000;
	CString str;

	const size_t nMySize = g_pPlayer->m_MonsterAlbum.GetCompletedMonstersCount();
	const size_t nAllSize = CMonsterAlbumMng::GetInstance()->GetMonsters()->size();

	CRect rectBar = m_wndCtrlProgressBar->rect;
	int nTotalWidth = rectBar.Width();
	int nCurrentWidth = nTotalWidth * nMySize / nAllSize;

	CRect rectColor = rectBar;
	rectColor.right = rectColor.left + nCurrentWidth;

	CRect rectWhite = rectBar;
	rectWhite.left = rectColor.right;

	p2DRender->RenderFillRect(rectColor, D3DCOLOR_ARGB(200, 67, 215, 67), D3DCOLOR_ARGB(200, 67, 215, 67), D3DCOLOR_ARGB(200, 41, 71, 39), D3DCOLOR_ARGB(200, 41, 71, 39));
	p2DRender->RenderFillRect(rectWhite, 0xffdededd);
	p2DRender->RenderRect(rectBar, D3DCOLOR_ARGB(255, 0, 0, 0));

	if (m_pRewardTexture)
	{
		const float fTextureSize = 0.8f;
		for (auto& it : *CMonsterAlbumMng::GetInstance()->GetBonuses())
		{
			const size_t nTotalUnlocked = it.first;

			CPoint pointVertLine1 = CPoint(rectBar.left + nTotalWidth * nTotalUnlocked / nAllSize, rectBar.top);
			CPoint pointVertLine2 = CPoint(pointVertLine1.x, rectBar.bottom);
			p2DRender->RenderLine(pointVertLine1, pointVertLine2, 0xff000000);

			CPoint pointTexture = pointVertLine1;
			pointTexture.x -= m_pRewardTexture->m_size.cx * fTextureSize / 2;
			pointTexture.y -= m_pRewardTexture->m_size.cy * fTextureSize + 2;
			p2DRender->RenderTexture(pointTexture, m_pRewardTexture, 255, fTextureSize, fTextureSize);

			// Render tooltip with info
			CRect rectToolTip = CRect(pointTexture.x, pointTexture.y, pointTexture.x + m_pRewardTexture->m_size.cx, pointTexture.y + m_pRewardTexture->m_size.cy);
			if (rectToolTip.PtInRect(ptMouse) && m_pWndOnMouseMove == this)
			{
				CPoint pt2 = ptMouse;
				ClientToScreen(&pt2);
				ClientToScreen(&rectToolTip);

				CEditString strEdit;

				strEdit.AddString("Rewards: ", dwTextColor, ESSTY_BOLD);

				if (it.second.vecItems.empty())
					strEdit.AddString("\n~");
				else
				{
					ItemProp* pItemProp = nullptr;
					for (auto& it2 : it.second.vecItems)
					{
						pItemProp = prj.GetItemProp(it2.dwItemID); // shouldnt be nullptr

						if (it2.bBinds)
							str.Format("\n%s (x%d) (Binded)", pItemProp->szName, it2.nItemCount);
						else
							str.Format("\n%s (x%d)", pItemProp->szName, it2.nItemCount);
						strEdit.AddString(str);
					}
				}

				strEdit.AddString("\nPermanent stats: ", dwTextColor, ESSTY_BOLD);

				if (it.second.vecDst.empty())
					strEdit.AddString("\n~");
				else
				{
					for (auto& it2 : it.second.vecDst)
					{
						if (g_WndMng.IsDstRate(it2.dwDest))
						{
							if (it2.dwDest == DST_ATTACKSPEED)
								str.Format("\n%s%+d%%  ", g_WndMng.FindDstString(it2.dwDest), it2.nVal / 2 / 10);
							else
								str.Format("\n%s%+d%%  ", g_WndMng.FindDstString(it2.dwDest), it2.nVal);
						}
						else
							str.Format("\n%s+%d  ", g_WndMng.FindDstString(it2.dwDest), it2.nVal);

						strEdit.AddString(str, g_WndMng.dwItemColor.dwAwakening);
					}
				}

				strEdit.AddString("\nDescription: ", dwTextColor, ESSTY_BOLD);
				str.Format("\nReward for %d unlocked monsters.", it.first);
				strEdit.AddString(str);

				g_toolTip.PutToolTip(10000, strEdit, rectToolTip, pt2, 0);
			}

		}
	}

	// Render progress info inside progress bar
	str.Format("Unlocked: %d / %d", nMySize, nAllSize);
	CSize sz = p2DRender->m_pFont->GetTextExtent(str);
	p2DRender->TextOut(rectBar.left + rectBar.Width() / 2 - sz.cx / 2, rectBar.top + rectBar.Height() / 2 - sz.cy / 2 + 1, str, 0xff000000);
	p2DRender->TextOut(rectBar.left + rectBar.Width() / 2 - sz.cx / 2 + 1, rectBar.top + rectBar.Height() / 2 - sz.cy / 2 + 1, str, 0xff000000);

	p2DRender->SetFont(pOldFont);
}
void CWndMonsterAlbum::DrawListBox(C2DRender* p2DRender)
{
	CD3DFont* pOldFont = p2DRender->GetFont();

	const int nOneZoneSize = 88;
	const CRect rectClient = m_pListBoxMonsters->GetClientRect();
	const int nWidth = rectClient.Width() / nOneZoneSize;
	const int nHeight = rectClient.Height() / nOneZoneSize;

	int nPage = nHeight;
	int nRange = m_vecSortedMonsters.size() / nWidth;
	if (m_vecSortedMonsters.size() % nWidth)
		nRange++;

	m_pListBoxMonsters->m_wndScrollBar.SetScrollRange(0, nRange);
	m_pListBoxMonsters->m_wndScrollBar.SetScrollPage(nPage);

	CPoint pt(3, m_pListBoxMonsters->m_wndScrollBar.GetScrollPos() * nWidth);

	CString str;
	const DWORD dwTextColor = 0xff000000;

	for (int i = pt.y; i < m_vecSortedMonsters.size(); i++)
	{
		if (i < 0)
			continue;

		int nMaxItem = (nPage + m_pListBoxMonsters->m_wndScrollBar.GetScrollPos()) * nWidth;
		if (i >= nMaxItem)
			break;

		ALBUM_STRUCT* pStruc = &m_vecSortedMonsters.at(i);

		int x = (i - pt.y) % nWidth;
		int y = (i - pt.y) / nWidth;

		// Render zone
		CPoint point(x * nOneZoneSize, y * nOneZoneSize);
		CRect crBoard;
		crBoard.left = point.x;
		crBoard.top = point.y;
		crBoard.right = point.x + nOneZoneSize - 1;
		crBoard.bottom = crBoard.top + nOneZoneSize - 1;
		p2DRender->RenderRect(crBoard, D3DCOLOR_ARGB(255, 0, 0, 0));

		CRect crRect = crBoard;
		crRect.DeflateRect(1, 1, 1, 1);
		p2DRender->RenderFillRect(crRect, D3DCOLOR_ARGB(255, 255, 255, 255));

		CRect crModel = crRect;
		crModel.bottom -= 14;

		// Render Model
		auto it = m_mapMonsterModels.find(pStruc->pMoverProp->dwID);
		if (it != m_mapMonsterModels.end())
		{
			CModelObject* pModelObject = it->second.pModelObject;
			float fRadius = it->second.fRadius;

			if (pModelObject && pModelObject->m_pModelElem)
			{
				//if (pStruc->nProgress == CARDS_NEEDED_TO_UNLOCK)
				pModelObject->FrameMove();

				CObject3D* pObj3D = pModelObject->GetObject3D();
				if (pObj3D)
				{
					D3DXVECTOR3 vecPos = pObj3D->m_vEvent[0];

					float fModelScale = pModelObject->m_pModelElem->m_fScale;
					if (fModelScale < 1.0f && fModelScale > 0.001f)
						vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));
					else if (fModelScale > 1.0f)
						vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

					vecPos.y += 2.2f;
					vecPos.z -= 3.0f;

					float fScale = (1 / fRadius) * 1.5f;

					VIEWPORT tmp;
					tmp.vecPos = vecPos;
					tmp.vecLookAt = D3DXVECTOR3(0.0f, 0.28f, 1.0f);
					tmp.vecMatrixScaling = D3DXVECTOR3(fScale, fScale, fScale);

					WNDCTRL tmpWndCtrl;
					tmpWndCtrl.rect = crModel;

					pModelObject->SetTextureEx(pModelObject->m_pModelElem->m_nTextureEx);
					g_WndMng.RenderObjectViewport(p2DRender, pModelObject, NULL, &tmpWndCtrl, &tmp, GetWndId());
				}
			}
		}

		// Render lock bg and texture
		if (pStruc->nProgress != CARDS_NEEDED_TO_UNLOCK)
		{
			p2DRender->RenderFillRect(crModel, D3DCOLOR_ARGB(80, 255, 0, 0));

			if (m_pLockTexture)
			{
				CPoint ptLockTexture;
				ptLockTexture.x = crModel.right - 2 - m_pLockTexture->m_size.cx;
				ptLockTexture.y = crModel.top + 2;
				m_pLockTexture->Render(p2DRender, ptLockTexture);
			}
		}

		// Render progress bar
		CRect crProgress = crRect;
		crProgress.top = crModel.bottom;

		const int nCurrent = pStruc->nProgress;
		const int nMax = CARDS_NEEDED_TO_UNLOCK;

		int nWidthClient = crProgress.Width();
		int nWidth = nWidthClient * nCurrent / nMax;

		CRect rectColor = crProgress;
		rectColor.right = rectColor.left + nWidth;

		CRect rectWhite = crProgress;
		rectWhite.left = rectColor.right;

		p2DRender->RenderFillRect(rectColor, D3DCOLOR_ARGB(200, 67, 215, 67), D3DCOLOR_ARGB(200, 67, 215, 67), D3DCOLOR_ARGB(200, 41, 71, 39), D3DCOLOR_ARGB(200, 41, 71, 39));
		p2DRender->RenderFillRect(rectWhite, 0xffdededd);
		p2DRender->RenderLine(crProgress.TopLeft(), CPoint(crProgress.TopLeft().x + crProgress.Width(), crProgress.TopLeft().y), 0xff000000);

		str.Format("%d / %d", nCurrent, nMax);
		CSize sz = p2DRender->m_pFont->GetTextExtent(str);
		p2DRender->TextOut(crProgress.left + crProgress.Width() / 2 - sz.cx / 2, crProgress.top + crProgress.Height() / 2 - sz.cy / 2 + 1, str, 0xff000000);
		p2DRender->TextOut(crProgress.left + crProgress.Width() / 2 - sz.cx / 2 + 1, crProgress.top + crProgress.Height() / 2 - sz.cy / 2 + 1, str, 0xff000000);

		// Render tooltip with info
		CRect rectToolTip = crBoard;
		if (rectToolTip.PtInRect(m_pListBoxMonsters->m_ptMouse) && m_pWndOnMouseMove == m_pListBoxMonsters)
		{
			CPoint pt2 = m_pListBoxMonsters->m_ptMouse;
			m_pListBoxMonsters->ClientToScreen(&pt2);
			m_pListBoxMonsters->ClientToScreen(&rectToolTip);

			CEditString strEdit;

			strEdit.AddString("Monster: ", dwTextColor, ESSTY_BOLD);
			str.Format("%s (Lvl: %d)", pStruc->pMoverProp->szName, pStruc->pMoverProp->dwLevel);
			strEdit.AddString(str);

			strEdit.AddString("\nLocation: ", dwTextColor, ESSTY_BOLD);
			LPWORLD pWorldStruc = g_WorldMng.GetWorldStruct(pStruc->dwWorldID);
			if (pWorldStruc)
			{
				str.Format("%s", pWorldStruc->m_szWorldName);
				strEdit.AddString(str);
			}

			strEdit.AddString("\nCard piece name: ", dwTextColor, ESSTY_BOLD);
			str.Format("%s", pStruc->pCardPieceProp ? pStruc->pCardPieceProp->szName : "~");
			strEdit.AddString(str);

			strEdit.AddString("\nChance to obtain card piece: ", dwTextColor, ESSTY_BOLD);
			str.Format("%f%%", (static_cast<float>(pStruc->dwChanceToObtain) / 3000000000) * 100.0);
			strEdit.AddString(str);

			strEdit.AddString("\nProgress: ", dwTextColor, ESSTY_BOLD);
			str.Format("%d / %d", pStruc->nProgress, CARDS_NEEDED_TO_UNLOCK);
			strEdit.AddString(str);

			g_toolTip.PutToolTip(10000, strEdit, rectToolTip, pt2, 0);
		}
	}

	p2DRender->SetFont(pOldFont);
}
BOOL CWndMonsterAlbum::Process()
{
	if (IsBossCardSlotBgState())
	{
		m_dwHighAlpha += 5;
		if (m_dwHighAlpha >= 256)
			m_dwHighAlpha = 0;
	}

	return TRUE;
}
void CWndMonsterAlbum::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pListBoxMonsters = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);
	m_pWndBtnHideUnlocked = (CWndButton*)GetDlgItem(WIDC_CHECK);
	m_wndCtrlProgressBar = GetWndCtrl(WIDC_CUSTOM1);
	m_pLockTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_Lock.dds"), 0xffff00ff);
	m_pRewardTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "monsteralbumreward.dds"), 0xffff00ff);
	//m_pWndTextStats = (CWndText*)GetDlgItem(WIDC_TEXT);
	m_pWndTextDescription = (CWndText*)GetDlgItem(WIDC_TEXT1);

	m_wndCtrl[0] = GetWndCtrl(WIDC_CHANGE);
	m_wndCtrl[1] = GetWndCtrl(WIDC_CHANGE1);
	m_wndCtrl[2] = GetWndCtrl(WIDC_CHANGE2);
	m_wndCtrl[3] = GetWndCtrl(WIDC_CHANGE3);
	m_wndCtrl[4] = GetWndCtrl(WIDC_CHANGE4);
	m_wndCtrl[5] = GetWndCtrl(WIDC_CHANGE5);

	for (int i = 0; i < MAX_BOSS_CARDS; i++)
		UpdateBossCardTexture(i);

	for (auto& it : *CMonsterAlbumMng::GetInstance()->GetMonsters())
	{
		CModelObject* pModelObject = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_MOVER, it.first);
		if (pModelObject)
		{
			prj.m_modelMng.LoadMotion(pModelObject, OT_MOVER, it.first, MTI_STAND);
			pModelObject->InitDeviceObjects(g_Neuz.GetDevice());

			MODEL_STRUCT tmp;
			tmp.pModelObject = pModelObject;
			tmp.fRadius = pModelObject->GetRadius();
			m_mapMonsterModels.insert(make_pair(it.first, tmp));
		}
	}

	UpdateMonsterList();
	UpdateStatSum();
	UpdateDescription();

	MoveParentCenter();
}

BOOL CWndMonsterAlbum::Initialize(CWndBase* pWndParent, DWORD)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_MONSTER_ALBUM);
}
BOOL CWndMonsterAlbum::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_CHECK)
			UpdateMonsterList();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
bool CWndMonsterAlbum::IsBossCardSlotBgState()
{
	if (m_GlobalShortcut.IsEmpty())
		return false;

	if (m_GlobalShortcut.m_dwShortcut != SHORTCUT_ITEM)
		return false;

	CItemContainer<CItemElem>* pContainer = g_pPlayer->GetContainerByType(m_GlobalShortcut.m_nLocation);
	if (!pContainer)
		return false;

	CItemElem* pTempElem = pContainer->GetAtId(m_GlobalShortcut.m_dwId);
	if (!IsUsableItem(pTempElem))
		return false;

	ItemProp* pItemProp = pTempElem->GetProp();
	if (!pItemProp || pItemProp->dwID != II_SYS_SYS_MA_SLOT_RESTORE)
		return false;

	return true;
}
BOOL  CWndMonsterAlbum::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		if (PtInRect(m_wndCtrl[i]->rect, point))
		{
			const DWORD dwBossCard = g_pPlayer->m_MonsterAlbum.GetBossCards()->at(i);
			if (dwBossCard == NULL_ID || dwBossCard == 0)
				return FALSE;

			if (!IsBossCardSlotBgState())
				return FALSE;

			g_DPlay.SendMABossCardSlotRestore(pShortcut->m_dwId, pShortcut->m_nLocation, i);
			break;
		}
	}

	return TRUE;
}
void CWndMonsterAlbum::OnMouseWndSurface(CPoint point)
{
	for (int i = 0; i < MAX_BOSS_CARDS; i++)
	{
		if (PtInRect(m_wndCtrl[i]->rect, point))
		{
			const DWORD dwBossCard = g_pPlayer->m_MonsterAlbum.GetBossCards()->at(i);
			if (dwBossCard == NULL_ID || dwBossCard == 0)
				return;

			CRect DrawRect = m_wndCtrl[i]->rect;
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&DrawRect);

			CItemElem itemElem;
			itemElem.m_dwItemId = dwBossCard;

			g_WndMng.PutToolTip_Item(&itemElem, point2, &DrawRect, GetWndId());
			break;
		}
	}
}
void CWndMonsterAlbum::UpdateMonsterList()
{
	m_pListBoxMonsters->ResetContent();

	m_vecSortedMonsters.clear();

	ALBUM_STRUCT tmp;

	for (auto& it : *CMonsterAlbumMng::GetInstance()->GetMonsters())
	{
		int nProgress = g_pPlayer->m_MonsterAlbum.GetMonsterProgress(it.first);

		if (m_pWndBtnHideUnlocked->GetCheck())
		{
			if (nProgress == CARDS_NEEDED_TO_UNLOCK)
				continue;
		}

		tmp.pMoverProp = prj.GetMoverProp(it.first);
		tmp.pCardPieceProp = it.second.dwCardPieceID == NULL_ID ? nullptr : prj.GetItemProp(it.second.dwCardPieceID);
		tmp.dwWorldID = it.second.dwWorldID;
		tmp.dwChanceToObtain = it.second.dwChanceToObtain;
		tmp.nProgress = nProgress;

		auto it = std::lower_bound(m_vecSortedMonsters.begin(), m_vecSortedMonsters.end(), tmp);
		m_vecSortedMonsters.insert(it, tmp);
	}
}
void CWndMonsterAlbum::UpdateProgress(DWORD dwID)
{
	for (auto it = m_vecSortedMonsters.begin(); it != m_vecSortedMonsters.end(); it++)
	{
		if (it->pMoverProp->dwID != dwID)
			continue;

		it->nProgress = g_pPlayer->m_MonsterAlbum.GetMonsterProgress(dwID);

		if (it->nProgress == CARDS_NEEDED_TO_UNLOCK)
		{
			UpdateStatSum();

			if (m_pWndBtnHideUnlocked->GetCheck())
				m_vecSortedMonsters.erase(it);
		}

		break;
	}
}
void CWndMonsterAlbum::UpdateBossCardTexture(int nSlot)
{
	const DWORD dwBossCard = g_pPlayer->m_MonsterAlbum.GetBossCards()->at(nSlot);
	if (dwBossCard == NULL_ID || dwBossCard == 0)
		m_pBossCardTexture[nSlot] = nullptr;
	else
	{
		ItemProp* pBossCardProp = prj.GetItemProp(dwBossCard);
		if (pBossCardProp)
			m_pBossCardTexture[nSlot] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pBossCardProp->szIcon), 0xffff00ff);
	}
}
void CWndMonsterAlbum::UpdateStatSum()
{
	//m_pWndTextStats->SetString("");
}
void CWndMonsterAlbum::UpdateDescription()
{
	CScript	scanner;
	if (!scanner.Load(MakePath(DIR_CLIENT, _T("MonsterAlbum.inc"))))
		return;

	CString str;
	str.Format(scanner.m_pProg, CARDS_NEEDED_TO_UNLOCK, prj.GetItemProp(II_SYS_SYS_MA_SLOT_UNLOCK) ? prj.GetItemProp(II_SYS_SYS_MA_SLOT_UNLOCK)->szName : "error"
		, prj.GetItemProp(II_SYS_SYS_MA_SLOT_RESTORE) ? prj.GetItemProp(II_SYS_SYS_MA_SLOT_RESTORE)->szName : "error"
	);

	m_pWndTextDescription->SetString(str);
}
#endif

#ifdef __FL_TDM
CWndActivities::CWndActivities()
{
}
CWndActivities::~CWndActivities()
{
}

void CWndActivities::OnDraw(C2DRender* p2DRender)
{
}
void CWndActivities::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();

#ifdef __FL_FFA
	m_wndFFA.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_FFA_REG);
#endif
#ifdef __FL_TDM
	m_wndTDM.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_TDM_REG);
#endif

	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = "FFA";
	tabTabItem.pWndBase = &m_wndFFA;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = "TDM";
	tabTabItem.pWndBase = &m_wndTDM;
	pWndTabCtrl->InsertItem(1, &tabTabItem);

	MoveParentCenter();
}

BOOL CWndActivities::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PVP_ACTIVITIES, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndActivities::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndActivities::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndActivities::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndTDMRegister::CWndTDMRegister(void)
	: m_pListPlayers(nullptr)
	, m_bLastTDM(false)
	, m_ct(0)
	, m_tEndTime(0)
	, m_tCurrentTime(0)
	, m_nNextRefresh(0)
	, m_nMaxPlayerCount(0)
{

}

CWndTDMRegister::~CWndTDMRegister(void)
{
	m_vecRegisteredPlayers.clear();
}

BOOL CWndTDMRegister::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TDM_REG, 0, CPoint(0, 0), pWndParent);
}

void CWndTDMRegister::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	m_pListPlayers = (CWndListBox*)GetDlgItem(WIDC_LISTBOX);

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	pEdit->AddWndStyle(EBS_READONLY);
	pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	pEdit->AddWndStyle(EBS_READONLY);

	LoadData();

	MoveParentCenter();
}

void CWndTDMRegister::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
}

BOOL CWndTDMRegister::Process(void)
{
	m_tCurrentTime = m_tEndTime - time_null();
	CString str;
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (m_tCurrentTime >= 0 && !m_bLastTDM)
	{
		m_ct = m_tCurrentTime;
		str.Format("%.2dh:%.2dm:%.2ds", m_ct.GetHours(), m_ct.GetMinutes(), m_ct.GetSeconds());
		pEdit->SetString(str);
	}
	else
	{
		if (m_bLastTDM)
		{
			str.Format("TDM is over today. Refresh in: %d hour(s).", m_nNextRefresh);
			pEdit->SetString(str);
		}
	}

	return TRUE;
}
void CWndTDMRegister::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndTDMRegister::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndTDMRegister::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
			g_DPlay.SendHdr(PACKETTYPE_REQUEST_TDM_REGISTER);
		else if (nID == WIDC_BUTTON2)
		{
			CWndActivities* pWndActivities = (CWndActivities*)g_WndMng.GetWndBase(APP_PVP_ACTIVITIES);
			if (pWndActivities)
				pWndActivities->Destroy();
		}
		else if (nID == WIDC_BUTTON3)
			LoadData();
		else if (nID == WIDC_BUTTON4)
			g_DPlay.SendHdr(PACKETTYPE_REQUEST_TDM_JOIN);
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
void CWndTDMRegister::LoadData()
{
	g_DPlay.SendHdr(PACKETTYPE_REQUEST_TDM);

	CWndButton* pWndButtRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButtRefresh->EnableWindow(FALSE);
}
void CWndTDMRegister::AcceptData(CAr& ar)
{
	m_vecRegisteredPlayers.clear();
	m_pListPlayers->ResetContent();

	time_t timeStart;
	size_t nPlayerCount;

	ar >> timeStart;
	ar >> m_nNextRefresh;
	ar >> m_nMaxPlayerCount;

	ar >> nPlayerCount;
	u_long idPlayer;
	for (size_t i = 0; i < nPlayerCount; i++)
	{
		ar >> idPlayer;
		m_vecRegisteredPlayers.push_back(idPlayer);
		m_pListPlayers->AddString("");
	}

	SetTime(timeStart);
	m_bLastTDM = timeStart == 0;

	UpdateStatic();

	CWndButton* pWndButtRefresh = (CWndButton*)GetDlgItem(WIDC_BUTTON3);
	pWndButtRefresh->EnableWindow(TRUE);
}
void CWndTDMRegister::RegisterPlayer(u_long idPlayer)
{
	m_vecRegisteredPlayers.push_back(idPlayer);
	m_pListPlayers->AddString("");
	UpdateStatic();
}
void CWndTDMRegister::UpdateStatic()
{
	CString str;
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT2);
	str.Format("%d/%d", m_vecRegisteredPlayers.size(), m_nMaxPlayerCount);
	pEdit->SetString(str);
}
void CWndTDMRegister::DrawListBox(C2DRender* p2DRender)
{
	const CRect rectClient = m_pListPlayers->GetClientRect();

	m_pListPlayers->m_nFontHeight = 22;
	const int nPage = rectClient.Height() / m_pListPlayers->m_nFontHeight;
	m_pListPlayers->m_wndScrollBar.SetScrollRange(0, m_vecRegisteredPlayers.size());
	m_pListPlayers->m_wndScrollBar.SetScrollPage(nPage);

	DWORD dwColor = 0xff000000;
	CString str;
	CRect rc = CRect(3, 3, 3 + rectClient.Width(), 3 + m_pListPlayers->m_nFontHeight);

	CSize sizeText = m_p2DRender->m_pFont->GetTextExtent("123");
	CPoint ptText(3, rc.bottom - rc.Height() / 2 - sizeText.cy / 2);

	CPoint ptLine(0, rc.bottom - 1);

	for (int i = m_pListPlayers->m_wndScrollBar.GetScrollPos(); i < m_pListPlayers->m_wndScrollBar.GetMaxScrollPos(); ++i)
	{
		if (i > (nPage + m_pListPlayers->m_wndScrollBar.GetScrollPos()))
			break;

		u_long idPlayer = m_vecRegisteredPlayers.at(i);

		PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(idPlayer);
		if (!pPlayerData)
			continue;

		if (i == m_pListPlayers->GetCurSel())
			p2DRender->RenderFillRect(rc, 0xFFbef5b0);
		p2DRender->RenderLine(CPoint(0, ptLine.y), CPoint(rectClient.right, ptLine.y), dwColor);

		// Job
		ptText.x = 5;
		if (prj.m_aJob[pPlayerData->data.nJob].dwJobType == JTYPE_PRO)
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pPlayerData->data.nJob - 6), 0xffffffff, 0.9f, 0.85f);
		else if (prj.m_aJob[pPlayerData->data.nJob].dwJobType == JTYPE_MASTER)
		{
			int nMasterIndex = 27;
			if (pPlayerData->data.nLevel < 70)
				nMasterIndex = 27;
			else if (pPlayerData->data.nLevel >= 70 && pPlayerData->data.nLevel < 80)
				nMasterIndex = 28;
			else if (pPlayerData->data.nLevel >= 80 && pPlayerData->data.nLevel < 90)
				nMasterIndex = 29;
			else if (pPlayerData->data.nLevel >= 90 && pPlayerData->data.nLevel < 100)
				nMasterIndex = 30;
			else if (pPlayerData->data.nLevel >= 100 && pPlayerData->data.nLevel < 110)
				nMasterIndex = 31;
			else if (pPlayerData->data.nLevel >= 110 && pPlayerData->data.nLevel <= 120)
				nMasterIndex = 32;

			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, nMasterIndex, 0xffffffff, 0.9f, 0.85f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pPlayerData->data.nJob - 16), 0xffffffff, 0.9f, 0.85f);
		}
		else if (prj.m_aJob[pPlayerData->data.nJob].dwJobType == JTYPE_HERO)
		{
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 33, 0xffffffff, 0.9f, 0.9f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pPlayerData->data.nJob - 24), 0xffffffff, 0.9f, 0.85f);
		}
		else if (prj.m_aJob[pPlayerData->data.nJob].dwJobType == JTYPE_LEGEND_HERO)
		{
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 33, 0xffffffff, 0.9f, 0.9f);
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, (19 + pPlayerData->data.nJob - 32), 0xffffffff, 0.9f, 0.85f);
		}
		else
			g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, ptText, 14 + pPlayerData->data.nJob, 0xffffffff, 0.9f, 0.85f);

		// Level
		ptText.x = 25;
		if (MAX_PROFESSIONAL <= pPlayerData->data.nJob && pPlayerData->data.nJob < MAX_MASTER)
			str.Format("%d-M", pPlayerData->data.nLevel);
		else if (MAX_MASTER <= pPlayerData->data.nJob)
			str.Format("%d-H", pPlayerData->data.nLevel);
		else if (MAX_HERO <= pPlayerData->data.nJob)
			str.Format("%d-H", pPlayerData->data.nLevel);
		else
			str.Format("%d", pPlayerData->data.nLevel);
		p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);

		// Name
		ptText.x = 75;
		str.Format("%s", pPlayerData->szPlayer);
		p2DRender->TextOut(ptText.x, ptText.y, str, dwColor);

		rc.top += m_pListPlayers->m_nFontHeight;
		rc.bottom += m_pListPlayers->m_nFontHeight;
		ptLine.y += m_pListPlayers->m_nFontHeight;
		ptText.y += m_pListPlayers->m_nFontHeight;
		ptText.x = 3;
	}
}

CWndTDMResult::CWndTDMResult(void)
{

}

CWndTDMResult::~CWndTDMResult(void)
{
	m_vecPlayersTDM.clear();
}

BOOL CWndTDMResult::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_TDM_RESULT, 0, CPoint(0, 0), pWndParent);
}

void CWndTDMResult::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();

	if (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->m_vecPlayersTDM.size())
		m_vecPlayersTDM = g_WndMng.m_pWndWorld->m_vecPlayersTDM;
	else
		Destroy();

	MoveParentCenter();
}

void CWndTDMResult::OnDraw(C2DRender* p2DRender)
{
	CWndNeuz::OnDraw(p2DRender);
}

void CWndTDMResult::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

BOOL CWndTDMResult::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
BOOL CWndTDMResult::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON1)
			Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif