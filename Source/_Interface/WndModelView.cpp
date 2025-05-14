#include "stdafx.h"
#ifdef __MODEL_VIEW
#include "WndModelView.h"
#include "ResData.h"
#include "defineObj.h"

CWndModelView::CWndModelView(void)
{
	m_pPet = NULL;
	m_pEgg = NULL;

	m_bLButtonDownRot = false;
	m_fRot = 0.0f;
	m_fWheel = 6.0f;
}
CWndModelView::~CWndModelView(void)
{
	SAFE_DELETE(m_pPet);
	SAFE_DELETE(m_pEgg);
}
BOOL CWndModelView::Process()
{
	if (m_Mover.m_pModel)
		m_Mover.m_pModel->FrameMove();
	if (m_pPet)
		m_pPet->FrameMove();
	if (m_pEgg)
		m_pEgg->FrameMove();

	return TRUE;
}
BOOL CWndModelView::Initialize(CWndBase* pWndParent, DWORD dwType)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MODEL_VIEW, 0, CPoint(0, 0), pWndParent);
}
void CWndModelView::OnDraw(C2DRender* p2DRender)
{
	CWndModelView* pWndView = (CWndModelView*)g_WndMng.GetWndBase(APP_MODEL_VIEW);
	CRect rect = pWndView->GetClientRect();

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
	lpFace->rect = rect;

	VIEWPORT tmp;
	tmp.m_fFov = m_fWheel;
	tmp.m_fRot = m_fRot;
	tmp.vecMatrixScaling = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

	if (m_pPet)
	{
		m_pPet->SetTextureEx(m_pPet->m_pModelElem->m_nTextureEx);
		g_WndMng.RenderObjectViewport(p2DRender, m_pPet, NULL, lpFace, &tmp, GetWndId());
	}
	else if (m_pEgg)
	{
		m_pEgg->SetTextureEx(m_pEgg->m_pModelElem->m_nTextureEx);
		g_WndMng.RenderObjectViewport(p2DRender, m_pEgg, NULL, lpFace, &tmp, GetWndId());
	}
	else if (m_Mover.m_pModel)
		g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)m_Mover.m_pModel, &m_Mover, lpFace, &tmp, GetWndId());
}
void CWndModelView::OnInitialUpdate(void)
{
	CWndNeuz::OnInitialUpdate();
	RestoreDeviceObjects();
	MoveParentCenter();

	SetSex(g_pPlayer->GetSex());
}
void CWndModelView::SetSex(BOOL bSex)
{
	if (bSex == g_pPlayer->m_bySex)
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
		{
			if (g_pPlayer->GetEquipItem(i))
			{
				m_Mover.m_aEquipInfo[i].dwId = g_pPlayer->GetEquipItem(i)->m_dwItemId;
#ifdef __MODEL_CHANGE
				m_Mover.m_aEquipInfo[i].dwLook = g_pPlayer->GetEquipItem(i)->GetLook();
#endif
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; ++i)
		{
			if (g_pPlayer->GetEquipItem(i))
			{
				if (i == PARTS_RWEAPON || i == PARTS_LWEAPON || i == PARTS_SHIELD)
				{
					m_Mover.m_aEquipInfo[i].dwId = g_pPlayer->GetEquipItem(i)->m_dwItemId;
#ifdef __MODEL_CHANGE
					m_Mover.m_aEquipInfo[i].dwLook = g_pPlayer->GetEquipItem(i)->GetLook();
#endif
				}
				else
				{
					m_Mover.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __MODEL_CHANGE
					m_Mover.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
				}
			}
		}
	}

	int nMover = (bSex ? MI_FEMALE : MI_MALE);

	m_Mover.m_bySex = bSex;

	SAFE_DELETE(m_Mover.m_pModel);
	m_Mover.m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
	prj.m_modelMng.LoadMotion(m_Mover.m_pModel, OT_MOVER, nMover, MTI_STAND);

	UpdMover();

	m_Mover.m_pModel->InitDeviceObjects(g_Neuz.GetDevice());
}
BOOL CWndModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pWndRoot->GetWindowRect().PtInRect(pt))
	{
		if (zDelta < 0)
			m_fWheel -= 0.1f;
		else
			m_fWheel += 0.1f;

		if (m_fWheel < 1.0f)
			m_fWheel = 6.0f;
		if (m_fWheel > 18.0f)
			m_fWheel = 6.0f;
	}
	return true;
}
void CWndModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
	{
		m_ptOld = point;
		m_bLButtonDownRot = TRUE;
	}
}
void CWndModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bLButtonDownRot = FALSE;
}
void CWndModelView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (GetWndCtrl(WIDC_CUSTOM1)->rect.PtInRect(point))
		m_ptOld1 = point;
}
void CWndModelView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
}
void CWndModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonDownRot)
	{
		m_fRot += (m_ptOld.x - point.x) * 0.75f;
		m_ptOld = point;
	}
}
void CWndModelView::UpdItem(DWORD dwParts, DWORD dwID)
{
	int nArryEquip1[4] = { PARTS_CAP, PARTS_UPPER_BODY, PARTS_HAND, PARTS_FOOT };
	int nArryEquip2[4] = { PARTS_HAT, PARTS_CLOTH, PARTS_GLOVE, PARTS_BOOTS };
	for (int i = 0; i < 4; i++)
	{
		if (dwParts == nArryEquip1[i])
		{
			m_Mover.m_aEquipInfo[nArryEquip2[i]].dwId = NULL_ID;
#ifdef __MODEL_CHANGE
			m_Mover.m_aEquipInfo[nArryEquip2[i]].dwLook = NULL_ID;
#endif
			break;
		}
		if (dwParts == nArryEquip2[i])
		{
			m_Mover.m_aEquipInfo[nArryEquip1[i]].dwId = NULL_ID;
#ifdef __MODEL_CHANGE
			m_Mover.m_aEquipInfo[nArryEquip1[i]].dwLook = NULL_ID;
#endif
			break;
		}
	}
	m_Mover.m_aEquipInfo[dwParts].dwId = dwID;
#ifdef __MODEL_CHANGE
	m_Mover.m_aEquipInfo[dwParts].dwLook = NULL_ID;
#endif
}
void CWndModelView::UpdMover()
{
	m_Mover.m_fHairColorR = g_pPlayer->m_fHairColorR;
	m_Mover.m_fHairColorG = g_pPlayer->m_fHairColorG;
	m_Mover.m_fHairColorB = g_pPlayer->m_fHairColorB;

	if (m_Mover.m_bySex == g_pPlayer->m_bySex)
		CMover::UpdateParts(g_pPlayer->GetSex(), g_pPlayer->m_dwHairMesh, g_pPlayer->m_dwHeadMesh, m_Mover.m_aEquipInfo, (CModelObject*)m_Mover.m_pModel, NULL);
	else
		CMover::UpdateParts(m_Mover.m_bySex, m_Mover.m_dwHairMesh, m_Mover.m_dwHeadMesh, m_Mover.m_aEquipInfo, (CModelObject*)m_Mover.m_pModel, NULL);
}
bool CWndModelView::SetPart(CItemElem* pItemElem)
{
	if (!pItemElem)
		return false;

	ItemProp* pProp = pItemElem->GetProp();
	if (!pProp)
		return false;

	DWORD dwID = pProp->dwID;
#ifdef __MODEL_CHANGE
	if (pItemElem->GetLook() != NULL_ID)
		dwID = pItemElem->GetLook();
#endif

	SAFE_DELETE(m_pPet);
	SAFE_DELETE(m_pEgg);

	if (pProp->dwItemSex != NULL_ID)
	{
		if (pProp->dwItemSex != m_Mover.m_bySex)
			SetSex(pProp->dwItemSex);
	}
	else
	{
		if (pProp->dwItemKind1 != IK1_WEAPON)
			SetSex(g_pPlayer->m_bySex);
	}

	if (pProp->dwParts > 1 && pProp->dwParts < MAX_HUMAN_PARTS && pProp->dwParts != PARTS_RIDE)
	{
		if (pProp->dwParts >= PARTS_NECKLACE1 && pProp->dwParts <= PARTS_BULLET)
			return false;

		UpdItem(pProp->dwParts, dwID);
	}
	else
	{
		if (pProp->dwItemKind3 == IK3_PET)
		{
			m_pPet = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_MOVER, pProp->dwLinkKind);
			m_pPet->LoadMotion(m_pPet->GetMotionFileName("stand"));
		}
		else if (pProp->dwItemKind3 == IK3_EGG)
		{
			int dwEggID = MI_PET_EGG;
			switch (dwID)
			{
			case II_PET_WHITETIGER01: dwEggID = MI_PET_WHITETIGER01; break;
			case II_PET_BARBARYLION01: dwEggID = MI_PET_BARBARYLION01; break;
			case II_PET_RABBIT02: dwEggID = MI_PET_RABBIT02; break;
			case II_PET_DRAGON01: dwEggID = MI_PET_DRAGON01; break;
			case II_PET_UNICORN01: dwEggID = MI_PET_UNICORN01; break;
			case II_PET_NINEFOX01: dwEggID = MI_PET_NINEFOX01; break;
			case II_PET_EAGLE01: dwEggID = MI_PET_EAGLE01; break;
			}
			m_pEgg = (CModelObject*)prj.m_modelMng.LoadModel(D3DDEVICE, OT_MOVER, dwEggID);
			m_pEgg->LoadMotion(m_pEgg->GetMotionFileName(dwID == II_PET_EGG ? "idle" : "stand"));
		}
		else
			return false;
	}

	UpdMover();
	return true;
}
void CWndModelView::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 200, 300);
	AdjustMaxRect(&rectWnd, 495, 590);
	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}
#endif