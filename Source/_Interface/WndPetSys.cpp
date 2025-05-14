#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndPetSys.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;

#if __VER >= 15 // __PETVIS
#include "definesound.h"
#endif

#if __VER >= 12 // __PET_0519
CWndPetAwakCancel::CWndPetAwakCancel()
{

	m_pItemElem = NULL;
	m_pEItemProp = NULL;
	m_pTexture = NULL;

}
CWndPetAwakCancel::~CWndPetAwakCancel()
{
}
void CWndPetAwakCancel::OnDraw(C2DRender* p2DRender)
{
	ItemProp* pItemProp;

	if (m_pItemElem)
	{
		pItemProp = m_pItemElem->GetProp();
		LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);

		if (m_pTexture)
			m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
	}
}
void CWndPetAwakCancel::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);

	if (::GetLanguage() == LANG_FRE)
		pButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, _T("ButOk2.bmp")), TRUE);

	pButton->EnableWindow(FALSE);
	m_pText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	SetDescription();

	MoveParentCenter();
}

BOOL CWndPetAwakCancel::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_AWAK_CANCEL, 0, CPoint(0, 0), pWndParent);
}

void CWndPetAwakCancel::OnDestroy()
{
	if (m_pItemElem)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
	}
}

BOOL CWndPetAwakCancel::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetAwakCancel::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPetAwakCancel::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPetAwakCancel::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPetAwakCancel::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_BUTTON1)
	{
		if (m_pItemElem)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
			pButton->EnableWindow(FALSE);
			g_DPlay.SendPickupPetAwakeningCancel(m_pItemElem->m_dwObjId);
			Destroy();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetAwakCancel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_pItemElem) return;
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);
	rect = wndCtrl->rect;
	if (rect.PtInRect(point))
	{
		m_pItemElem->SetExtra(0);
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
		pButton->EnableWindow(FALSE);
		m_pItemElem = NULL;
		m_pEItemProp = NULL;
		m_pTexture = NULL;
	}
}

BOOL CWndPetAwakCancel::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	if (!g_pPlayer)
		return FALSE;

	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	if (!pTempElem->IsEatPet())
		return FALSE;

	if (m_pItemElem) 
		m_pItemElem->SetExtra(0);
	m_pItemElem = pTempElem;
	m_pEItemProp = m_pItemElem->GetProp();
	m_pItemElem->SetExtra(m_pItemElem->GetExtra() + 1);
	if (m_pEItemProp)
		m_pTexture = m_pItemElem->m_pTexture;

	CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	pButton->EnableWindow(TRUE);

	return TRUE;
}

void CWndPetAwakCancel::SetDescription()
{
	CScript scanner;
	BOOL checkflag;
	CHAR* szChar;

	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("PetAwakCancel.inc")));
	szChar = scanner.m_pProg;

	if (m_pText && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}
#endif

#if __VER >= 9 // __CSC_VER9_1
////////////////////////////////////////////////////////////////////
// CWndPetStatus
////////////////////////////////////////////////////////////////////

CWndPetStatus::CWndPetStatus()
{
	m_nDisplay = 2;
	m_nHPWidth = -1;
	m_nEXPWidth = -1;

#ifndef __AIO_PETS
	m_pVBHPGauge = NULL;
#endif
	m_pVBEXPGauge = NULL;

#ifndef __AIO_PETS
	m_bVBHPGauge = TRUE;
#endif
	m_bVBEXPGauge = TRUE;

	m_bHPVisible = TRUE;
	m_bExpVisible = TRUE;

	m_pPetElem = NULL;

	m_bExtension = FALSE;
	m_nLockLevel[0] = -1;
	m_nLockLevel[1] = -1;
	m_nBackUpLevel[0] = -1;
	m_nBackUpLevel[1] = -1;

	m_strPathLvImage[0] = MakePath(DIR_ICON, "Icon_PetLevel1_S.dds");
	m_strPathLvImage[1] = MakePath(DIR_ICON, "Icon_PetLevel2_S.dds");
	m_strPathLvImage[2] = MakePath(DIR_ICON, "Icon_PetLevel3_S.dds");
	m_strPathLvImage[3] = MakePath(DIR_ICON, "Icon_PetLevel4_S.dds");
	m_strPathLvImage[4] = MakePath(DIR_ICON, "Icon_PetLevel5_S.dds");
	m_strPathLvImage[5] = MakePath(DIR_ICON, "Icon_PetLevel6_S.dds");
	m_strPathLvImage[6] = MakePath(DIR_ICON, "Icon_PetLevel7_S.dds");
	m_strPathLvImage[7] = MakePath(DIR_ICON, "Icon_PetLevel8_S.dds");
	m_strPathLvImage[8] = MakePath(DIR_ICON, "Icon_PetLevel9_S.dds");

	m_nChoiceLevel = -1;
	m_pPetModel = NULL;

	for (int i = 0; i < 6; i++)
		m_pTexPetLv[i] = NULL;

	m_pTexPetLvBg = NULL;
	m_pTexPetStatusBg = NULL;
	memset(m_aPetLv, 0, sizeof(int) * 6);

	SetPetCamTable();
}

CWndPetStatus::~CWndPetStatus()
{
	DeleteDeviceObjects();
}

void CWndPetStatus::SetPetCamTable()
{

	m_PetCameTable[0].CamPos.x = 1.2f;
	m_PetCameTable[0].CamPos.y = 4.6f;
	m_PetCameTable[0].CamPos.z = -7.0f;
	m_PetCameTable[0].CamLook.x = -2.4f;
	m_PetCameTable[0].CamLook.y = 3.0f;
	m_PetCameTable[0].CamLook.z = 0.0f;
	m_PetCameTable[0].Scale = 6.0f;

	m_PetCameTable[1].CamPos.x = 0.7f;
	m_PetCameTable[1].CamPos.y = 3.0f;
	m_PetCameTable[1].CamPos.z = -5.2f;
	m_PetCameTable[1].CamLook.x = -2.2f;
	m_PetCameTable[1].CamLook.y = 2.5f;
	m_PetCameTable[1].CamLook.z = 1.0f;
	m_PetCameTable[1].Scale = 6.0f;

	m_PetCameTable[2].CamPos.x = 2.0f;
	m_PetCameTable[2].CamPos.y = 2.2f;
	m_PetCameTable[2].CamPos.z = -4.0f;
	m_PetCameTable[2].CamLook.x = -2.2f;
	m_PetCameTable[2].CamLook.y = 1.0f;
	m_PetCameTable[2].CamLook.z = 3.0f;
	m_PetCameTable[2].Scale = 7.0f;

	m_PetCameTable[3].CamPos.x = 1.4f;
	m_PetCameTable[3].CamPos.y = 4.2f;
	m_PetCameTable[3].CamPos.z = -8.0f;
	m_PetCameTable[3].CamLook.x = -2.2f;
	m_PetCameTable[3].CamLook.y = 2.0f;
	m_PetCameTable[3].CamLook.z = 3.0f;
	m_PetCameTable[3].Scale = 6.0f;

	m_PetCameTable[4].CamPos.x = 1.4f;
	m_PetCameTable[4].CamPos.y = 6.8f;
	m_PetCameTable[4].CamPos.z = -6.0f;
	m_PetCameTable[4].CamLook.x = -2.0f;
	m_PetCameTable[4].CamLook.y = 6.0f;
	m_PetCameTable[4].CamLook.z = 3.0f;
	m_PetCameTable[4].Scale = 5.5f;

	m_PetCameTable[5].CamPos.x = 3.0f;
	m_PetCameTable[5].CamPos.y = 6.0f;
	m_PetCameTable[5].CamPos.z = -12.0f;
	m_PetCameTable[5].CamLook.x = -2.0f;
	m_PetCameTable[5].CamLook.y = 3.0f;
	m_PetCameTable[5].CamLook.z = 3.0f;
	m_PetCameTable[5].Scale = 4.5f;

	m_PetCameTable[6].CamPos.x = 4.0f;
	m_PetCameTable[6].CamPos.y = 3.0f;
	m_PetCameTable[6].CamPos.z = -10.0f;
	m_PetCameTable[6].CamLook.x = -6.0f;
	m_PetCameTable[6].CamLook.y = 6.0f;
	m_PetCameTable[6].CamLook.z = 3.0f;
	m_PetCameTable[6].Scale = 5.0f;
}

void CWndPetStatus::MakeGaugeVertex()
{
	LPWNDCTRL lpHP = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpExp = GetWndCtrl(WIDC_CUSTOM3);

	if (m_pPetElem != NULL && m_pPetElem->m_pPet != NULL)
	{
		CRect rect = GetClientRect();
		CRect rectTemp;

		int nWidthClient = lpHP->rect.Width();
		int nWidth;

#ifndef __AIO_PETS
		// HP
		nWidth = nWidthClient * m_pPetElem->m_pPet->GetEnergy() / m_pPetElem->m_pPet->GetMaxEnergy();
		if (m_nHPWidth != nWidth)
		{
			m_nHPWidth = nWidth;
			rect = lpHP->rect;
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBHPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x64ff0000, m_pVBHPGauge, &m_texGauFillNormal);
		}
#endif

		// Exp
		nWidth = (int)(((float)nWidthClient / m_pPetElem->m_pPet->GetMaxExp()) * m_pPetElem->m_pPet->GetExp());
		if (m_nEXPWidth != nWidth)
		{
			m_nEXPWidth = nWidth;
			rect = lpExp->rect;
			rectTemp = rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen(rectTemp);
			m_bVBEXPGauge = m_pTheme->MakeGaugeVertex(m_pApp->m_pd3dDevice, &rectTemp, 0x848c40ff, m_pVBEXPGauge, &m_texGauFillNormal);
		}
	}
}

void CWndPetStatus::PaintFrame(C2DRender* p2DRender)
{
	if (!IsValidObj(g_pPlayer))
		return;

	CRect rect = GetWindowRect();

	if (g_pPlayer->HasActivatedSystemPet())
	{
		m_pPetElem = g_pPlayer->GetPetItem();

		if (m_pTexture && m_pPetElem != NULL && m_pPetElem->m_pPet != NULL)
		{
			RenderWnd();

			if (IsWndStyle(WBS_CAPTION))
			{
				int y = 4;
				CD3DFont* pOldFont = p2DRender->GetFont();
				p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
				p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

				char szNameLevel[128] = { 0, };

				ItemProp* pProp = m_pPetElem->GetProp();
				CString strName = pProp->szName;
				if (m_pPetElem->m_pPet->GetLevel() > PL_EGG)
				{
					MoverProp* pMoverProp = prj.GetMoverProp(m_pPetElem->m_pPet->GetIndex());
					if (pMoverProp)
						strName = pMoverProp->szName;
				}
#ifdef __PET_1024
				if (m_pPetElem->m_pPet->HasName())
					strName = m_pPetElem->m_pPet->GetName();
#endif	// __PET_1024
				sprintf(szNameLevel, "%s", strName);
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
}

void CWndPetStatus::DrawPetInformation(C2DRender* p2DRender)
{
	// Draw Bg Text
	p2DRender->TextOut(74, 4, prj.GetText(TID_GAME_PET_STATUS_LEVEL), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
	p2DRender->TextOut(74, 18, prj.GetText(TID_GAME_PET_STATUS_ABILITY), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
#ifndef __AIO_PETS
	p2DRender->TextOut(74, 46, prj.GetText(TID_GAME_PET_STATUS_HP), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));
#endif
	p2DRender->TextOut(74, 61, prj.GetText(TID_GAME_PET_STATUS_EXP), D3DCOLOR_ARGB(255, 74, 74, 173), D3DCOLOR_ARGB(255, 160, 160, 240));

	DWORD dwColor = D3DCOLOR_ARGB(255, 255, 28, 174);

	if (m_pPetElem->m_pPet)
	{
		int nLevel = m_pPetElem->m_pPet->GetLevel();
		CString strTemp;
		DWORD dwLevelText;
		switch (nLevel)
		{
		case PL_EGG:
			dwLevelText = TID_GAME_PETGRADE_E;
			break;
		case PL_D:
			dwLevelText = TID_GAME_PETGRADE_D;
			break;
		case PL_C:
			dwLevelText = TID_GAME_PETGRADE_C;
			break;
		case PL_B:
			dwLevelText = TID_GAME_PETGRADE_B;
			break;
		case PL_A:
			dwLevelText = TID_GAME_PETGRADE_A;
			break;
		case PL_S:
			dwLevelText = TID_GAME_PETGRADE_S;
			break;
		}
		strTemp.Format("%s", prj.GetText(dwLevelText));
		p2DRender->TextOut(112, 4, strTemp, dwColor);


		CSize size = g_Neuz.m_2DRender.m_pFont->GetTextExtent(prj.GetText(TID_GAME_PET_STATUS_ABILITY));

		DWORD dwDstParam;
		int nParam;
		DWORD dwTooltip;
		m_pPetElem->m_pPet->GetAvailDestParam(dwDstParam, nParam);

		if (dwDstParam != 0 && nParam != 0)
		{
			if (g_WndMng.IsDst_Rate(dwDstParam))
			{
				if (dwDstParam == DST_ATTACKSPEED)
					strTemp.Format("%s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam / 2 / 10);
				else
					strTemp.Format("%s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam);
			}
			else
			{
				strTemp.Format("%s %+d", g_WndMng.FindDstString(dwDstParam), nParam);
			}
		}
		else
			strTemp.Format("%s", prj.GetText(TID_GAME_PET_EGG_ABILITY));

		p2DRender->TextOut(76 + size.cx, 18, strTemp, dwColor);

		//#ifndef __AIO_PETS
		int nLife = m_pPetElem->m_pPet->GetLife();
		strTemp.Format("%d", nLife);
		p2DRender->TextOut(100, 33, strTemp, dwColor);
		//#endif

		if (m_bExtension)
		{
			//Draw Level Background Image
			CPoint point;

			point = CPoint(GetWndCtrl(WIDC_CUSTOM4)->rect.left - 2, GetWndCtrl(WIDC_CUSTOM4)->rect.top - 2);

			if (m_pTexPetLvBg != NULL)
				m_pTexPetLvBg->Render(p2DRender, point);

			//Draw Level Image
			dwColor = D3DCOLOR_ARGB(255, 0, 0, 0);

			for (int i = PL_D; i <= nLevel; i++)
			{
				BYTE bLevel = m_pPetElem->m_pPet->GetAvailLevel(i);
				if (bLevel != 0)
				{
					if (m_aPetLv[i] != bLevel)
					{
						CString strPath;
						if (m_nLockLevel[0] == i)
							strPath = m_strPathLvImage[m_nBackUpLevel[0] - 1];
						else if (m_nLockLevel[1] == i)
							strPath = m_strPathLvImage[m_nBackUpLevel[1] - 1];
						else
							strPath = m_strPathLvImage[bLevel - 1];

						m_pTexPetLv[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
						m_aPetLv[i] = bLevel;
					}
					if (m_pTexPetLv[i] != NULL)
					{
						if (m_nChoiceLevel == i)
							m_pTexPetLv[i]->Render(p2DRender, point);
						else
							m_pTexPetLv[i]->Render(p2DRender, point, 120);
					}

					point.x += 35;
				}
			}
		}
	}
}

void CWndPetStatus::OnDraw(C2DRender* p2DRender)
{
	if (!g_pPlayer || !m_pPetElem || !m_pPetElem->m_pPet)
		return;

	//Draw Status Background Image
	CPoint point = CPoint(GetWndCtrl(WIDC_CUSTOM1)->rect.left - 12, GetWndCtrl(WIDC_CUSTOM1)->rect.top - 6);
	if (m_pTexPetStatusBg)
		m_pTexPetStatusBg->Render(p2DRender, point);

	CRect rect = GetClientRect();
	int nWidthClient = GetClientRect().Width() - 110;

	CRect rectTemp;
	LPWNDCTRL lpHP = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpExp = GetWndCtrl(WIDC_CUSTOM3);

	MakeGaugeVertex();

#ifndef __AIO_PETS
	if (m_bVBHPGauge)
	{
		int nMax = m_pPetElem->m_pPet->GetMaxEnergy();
		int nHitPercent = MulDiv(m_pPetElem->m_pPet->GetEnergy(), 100, nMax);

		m_bHPVisible = TRUE;

		if (m_bHPVisible)
			m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBHPGauge, &m_texGauFillNormal);
	}
#endif

	if (m_bVBEXPGauge)
		m_pTheme->RenderGauge(p2DRender->m_pd3dDevice, m_pVBEXPGauge, &m_texGauFillNormal);

	//Pet Information
	DrawPetInformation(p2DRender);

	if (m_nDisplay != 0)
	{
		DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

		char cbufHp[16] = { 0, };
		char cbufExp[16] = { 0, };

		int nCharHP, nCharEXP;

#ifndef __AIO_PETS
		if (m_nDisplay == 1)
		{
			int nMax = m_pPetElem->m_pPet->GetMaxEnergy();
			int nHitPercent = MulDiv(m_pPetElem->m_pPet->GetEnergy(), 100, nMax);

			wsprintf(cbufHp, "%d%%", nHitPercent);
			p2DRender->TextOut(lpHP->rect.right - 70, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000);

			nCharHP = wsprintf(cbufHp, "%dp", m_pPetElem->m_pPet->GetEnergy());

			int x = lpHP->rect.right - 7;
			p2DRender->TextOut(x - (int)(nCharHP * 5.8f), lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000);
		}
		else
		{
			nCharHP = wsprintf(cbufHp, "%d", m_pPetElem->m_pPet->GetEnergy());

			int x = lpHP->rect.right - 76 + 15;
			p2DRender->TextOut(x - (int)(nCharHP * 2.6f), lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000);

			nCharHP = wsprintf(cbufHp, "%d", m_pPetElem->m_pPet->GetMaxEnergy());

			x = lpHP->rect.right - 38 + 15;
			p2DRender->TextOut(x - nCharHP * 2, lpHP->rect.top - 0, cbufHp, dwColor, 0xff000000);

			p2DRender->TextOut(lpHP->rect.right - 42, lpHP->rect.top - 0, "/", dwColor, 0xff000000);
		}
#endif
		EXPINTEGER	nExpResult = m_pPetElem->m_pPet->GetExp() * (EXPINTEGER)10000 / m_pPetElem->m_pPet->GetMaxExp();
		float fExp = (float)nExpResult / 100.0f;

		if (fExp >= 99.99f)
			nCharEXP = sprintf(cbufExp, "99.99%%");
		else
			nCharEXP = sprintf(cbufExp, "%.2f%%", fExp);

		int x = lpHP->rect.right - 7; // -40
		p2DRender->TextOut(x - (int)(nCharEXP * 5.8f), lpExp->rect.top, cbufExp, dwColor, 0xff000000);
	}

	if (!m_pPetModel || !m_pPetModel->m_pModelElem)
		return;

	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecLookAt;
	float fScale = 1.0f;

	if (m_pPetElem->m_pPet->GetLevel() == PL_EGG)
	{
		vecPos.x = 0.0f;
		vecPos.y = 5.0f;
		vecPos.z = -4.0f;

		vecLookAt.x = 0.0f;
		vecLookAt.y = 0.0f;
		vecLookAt.z = 3.0f;
		fScale = 5.0f;
	}
	else
	{
		int petkind = m_pPetElem->m_pPet->GetKind();
		vecPos = m_PetCameTable[petkind].CamPos;
		vecLookAt = m_PetCameTable[petkind].CamLook;
		fScale = m_PetCameTable[petkind].Scale;
	}

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
	VIEWPORT tmp;
	tmp.vecPos = vecPos;
	tmp.vecLookAt = vecLookAt;
	tmp.vecMatrixScaling = D3DXVECTOR3(fScale, fScale, fScale);

	m_pPetModel->SetTextureEx(m_pPetModel->m_pModelElem->m_nTextureEx);
	g_WndMng.RenderObjectViewport(p2DRender, m_pPetModel, NULL, lpFace, &tmp, GetWndId());
}

HRESULT CWndPetStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
#ifndef __AIO_PETS
	if (m_pVBHPGauge == NULL)
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBHPGauge, NULL);
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL);

		m_nHPWidth = -1;
		m_nEXPWidth = -1;
	}
#else
	if (m_pVBEXPGauge == NULL)
	{
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBEXPGauge, NULL);
		m_nEXPWidth = -1;
	}
#endif
	return S_OK;
}

HRESULT CWndPetStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
#ifndef __AIO_PETS
	SAFE_RELEASE(m_pVBHPGauge);
#endif
	SAFE_RELEASE(m_pVBEXPGauge);
	SAFE_DELETE(m_pPetModel);
	return S_OK;
}

HRESULT CWndPetStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndPetStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();
	m_texGauEmptyNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);
	m_texGauFillNormal.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "GauEmptyNormal.bmp"), 0xffff00ff, TRUE);

	m_pTexPetLvBg = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "PetLevelBg.tga"), 0xffff00ff);
	m_pTexPetStatusBg = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "PetStatusBg.tga"), 0xffff00ff, TRUE);

	if (g_pPlayer->HasActivatedSystemPet())
		m_pPetElem = g_pPlayer->GetPetItem();

	//Size Control
	SetExtension(m_bExtension);

	//Position Control
	CWndStatus* pWndStatus = (CWndStatus*)GetWndBase(APP_STATUS1);
	if (pWndStatus != NULL)
	{
		CRect rectRoot = pWndStatus->GetWndRect();
		CRect rect = GetWindowRect();
		int x = rectRoot.right;
		int y = rectRoot.top;
		Move(CPoint(x, y));
	}
	else
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CPoint point(rectRoot.left + 216, rectRoot.top);
		Move(point);
	}
}

void CWndPetStatus::OnMouseWndSurface(CPoint point)
{
	if (!m_bExtension || m_pPetElem->m_pPet == NULL)
		return;

	CRect baseRect = GetWndCtrl(WIDC_CUSTOM4)->rect;
	CRect testRect;
	BOOL bCheckChoice = FALSE;

	testRect = baseRect;
	testRect.right = testRect.left + 34;

	int nLevel = m_pPetElem->m_pPet->GetLevel();

	for (int i = PL_D; i < nLevel + 1; i++)
	{
		if (testRect.PtInRect(point))
		{
			CPoint point2 = point;
			ClientToScreen(&point2);
			ClientToScreen(&testRect);

			DWORD dwDstParam;
			int nParam;
			DWORD dwTooltip;
			CString strTemp;
			CEditString strEdit;

			PPETAVAILPARAM pAvailParam = CPetProperty::GetInstance()->GetAvailParam(m_pPetElem->m_pPet->GetKind());
			dwDstParam = pAvailParam->dwDstParam;
			nParam = pAvailParam->m_anParam[m_pPetElem->m_pPet->GetAvailLevel(i) - 1];

			if (g_WndMng.IsDst_Rate(dwDstParam))
			{
				if (dwDstParam == DST_ATTACKSPEED)
					strTemp.Format("%s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam / 2 / 10);
				else
					strTemp.Format("%s %+d%%", g_WndMng.FindDstString(dwDstParam), nParam);
			}
			else
			{
				strTemp.Format("%s %+d", g_WndMng.FindDstString(dwDstParam), nParam);
			}

			strEdit.AddString(strTemp, D3DCOLOR_XRGB(255, 0, 0));

			g_toolTip.PutToolTip(m_pPetElem->m_dwItemId, strEdit, &testRect, point2, 0);
			bCheckChoice = TRUE;
			m_nChoiceLevel = i;
		}
		testRect.left = testRect.right;
		testRect.right = testRect.left + 34;
	}

	if (!bCheckChoice)
		m_nChoiceLevel = -1;
}

void CWndPetStatus::SetExtension(BOOL eflag)
{
	CRect rect = GetWindowRect(TRUE);

	if (eflag)
		rect.bottom += 48;
	else
		rect.bottom -= 48;

	SetWndRect(rect);
}

BOOL CWndPetStatus::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_STATUS);
}

BOOL CWndPetStatus::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndPetStatus::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndPetStatus::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_nHPWidth = -1;
	m_nEXPWidth = -1;

	CWndNeuz::SetWndRect(rectWnd, bOnSize);
}

void CWndPetStatus::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndPetStatus::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (++m_nDisplay > 2)
		m_nDisplay = 0;
}

BOOL CWndPetStatus::OnEraseBkgnd(C2DRender* p2DRender)
{
	return CWndBase::OnEraseBkgnd(p2DRender);
	CRect rect = GetClientRect();
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	p2DRender->RenderFillRect(rect, D3DCOLOR_ARGB(100, 0, 0, 0));

	return TRUE;
}

BOOL CWndPetStatus::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
#ifdef __AIO_PETS
	if (message == WNM_CLICKED)
	{
		if (nID == WTBID_CLOSE)
		{
			g_Option.m_bHideWindowPet = TRUE;
			g_WndMng.PutString("Window deactivated. It will not be opened in the future until you manually open it by clicking on the pet -> Pet State.");
		}
	}
#endif
	if (nID == WIDC_BUTTON1)
	{
		m_bExtension = !m_bExtension;
		SetExtension(m_bExtension);
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndPetStatus::Process()
{
	if (!IsValidObj(g_pPlayer))
		return FALSE;

	if (g_pPlayer->HasActivatedSystemPet())
	{
		m_pPetElem = g_pPlayer->GetPetItem();

		if (m_pPetElem)
		{
			if (!g_pPlayer->GetPetModel() && m_pPetModel)
			{
				SAFE_DELETE(m_pPetModel);
			}
			else if (m_pPetElem->m_pPet && g_pPlayer->GetPetModel() && !m_pPetModel)
			{
				m_pPetModel = new CModelObject;
				m_pPetModel->InitDeviceObjects(g_Neuz.m_pd3dDevice);

				CString textFile;
				textFile.Format("Mvr_%s.chr", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				m_pPetModel->LoadBone(textFile);
				textFile.Format("Mvr_%s.o3d", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				m_pPetModel->LoadElement(textFile, 0);

				m_pPetModel->m_pModelElem = g_pPlayer->GetPetModel()->m_pModelElem;

				if (m_pPetElem->m_pPet->GetLevel() == PL_EGG)
					textFile.Format("Mvr_%s_idle.ani", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);
				else
					textFile.Format("Mvr_%s_stand.ani", g_pPlayer->GetPetModel()->m_pModelElem->m_szName);

				m_pPetModel->LoadMotion(textFile);
			}
		}
	}

	if (m_pPetModel)
		m_pPetModel->FrameMove();

	return TRUE;
}

void CWndPetStatus::LockShowLevel(BOOL lFlag, int nLevel, int nPos)
{
	if (lFlag)
	{
		m_nLockLevel[nPos] = nLevel;
		if (m_pPetElem->m_pPet)
			m_nBackUpLevel[nPos] = m_pPetElem->m_pPet->GetAvailLevel(nLevel);
	}
	else
	{
		m_nLockLevel[nPos] = -1;
		m_nBackUpLevel[nPos] = -1;

		CString strPath;
		BYTE bLevel = m_pPetElem->m_pPet->GetAvailLevel(nLevel);
		if (bLevel != 0)
		{
			strPath = m_strPathLvImage[bLevel - 1];
			m_pTexPetLv[nLevel] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		}
	}
}

////////////////////////////////////////////////////////////////////
//	CWndFoodConfirm Class
////////////////////////////////////////////////////////////////////

CWndFoodConfirm::CWndFoodConfirm()
{
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_nParent = 0;
}

CWndFoodConfirm::CWndFoodConfirm(int nParent)
{
	m_pItemElem = NULL;
	m_pEdit = NULL;
	m_nParent = nParent;
}

CWndFoodConfirm::~CWndFoodConfirm()
{
}

void CWndFoodConfirm::SetItem(DWORD dwObjId)
{
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId(dwObjId);
	if (!IsUsableItem(pItemElem))
		return;

	m_pItemElem = pItemElem;
}

void CWndFoodConfirm::OnDraw(C2DRender* p2DRender)
{
	LPCTSTR szNumber;
	szNumber = m_pEdit->GetString();
	int nNumber = atoi(szNumber);

	if (m_pItemElem == NULL)
		return;

	if (m_pItemElem->m_nItemNum == 1)
	{
		m_pEdit->SetString("1");
	}
	else
	{
		if (nNumber > 1000)	// 0723
		{
			char szNumberbuf[16] = { 0, };
			nNumber = 1000;
			_itoa(1000, szNumberbuf, 10);
			m_pEdit->SetString(szNumberbuf);
		}
		else if (m_pItemElem->m_nItemNum < nNumber)
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
void CWndFoodConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndOk->SetDefault(TRUE);
	m_pEdit->SetFocus();

	if (m_pItemElem == NULL)
	{
		Destroy();
		return;
	}

	if (m_pItemElem->m_nItemNum == 1)
	{
		m_pEdit->SetString("1");
	}
	else
	{
		TCHAR szNumber[64];
		if (m_pItemElem->m_nItemNum > 1000)	// 0723
			_itot(1000, szNumber, 10);
		else
			_itot(m_pItemElem->m_nItemNum, szNumber, 10);
		m_pEdit->SetString(szNumber);
	}

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	if (m_nParent == 1)
		pStatic->SetTitle(prj.GetText(TID_GAME_PETFOOD_COUNTCONFIRM));
	else if (m_nParent == 2)
		pStatic->SetTitle(prj.GetText(TID_GAME_ITEMCOUNT_QUESTION));


	MoveParentCenter();
}

BOOL CWndFoodConfirm::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_ITEM, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndFoodConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndFoodConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndFoodConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndFoodConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndFoodConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK || message == EN_RETURN)
	{
		short DropNum = 0;

		if (m_pItemElem != NULL)
		{
			if (m_pItemElem->m_nItemNum >= 1)
			{
				DropNum = atoi(m_pEdit->GetString());
			}
			if (DropNum != 0 && DropNum <= 1000)	// 0723
			{
				if (m_nParent == 1) //Pet Food
				{
					//Send to Server...
#if __VER < 12 // __PET_0519
					g_DPlay.SendUsePetFeed(m_pItemElem->m_dwObjId, DropNum);
#endif	// __PET_0519
					//PLAYSND( "Feed.wav" );
				}
				else if (m_nParent == 2) //Food Mill
				{
					CWndPetFoodMill* pWndPetFoodMill = (CWndPetFoodMill*)g_WndMng.GetWndBase(APP_PET_FOODMILL);
					if (pWndPetFoodMill != NULL)
						pWndPetFoodMill->SetItemForFeed(m_pItemElem, DropNum);
				}
		}
	}

		Destroy();
}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndFoodConfirm::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	if (m_pTexture)
	{
		RenderWnd();

		if (IsWndStyle(WBS_CAPTION))
		{
			int y = 4;
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontWndTitle);
			p2DRender->TextOut(10, y, m_strTitle, m_dwColor);

			char szNameLevel[128] = { 0, };
			if (m_nParent == 1)
				sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_PETFOOD_CONFIRM));
			else if (m_nParent == 2)
				sprintf(szNameLevel, "%s", prj.GetText(TID_GAME_ITEMCOUNT_CONFIRM));

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

////////////////////////////////////////////////////////////////////
//	CWndPetMiracle Class
////////////////////////////////////////////////////////////////////
CWndPetMiracle::CWndPetMiracle()
{
	m_nCount[0] = 0;
	m_nDelay[0] = 1;
	m_nStatus[0] = -1;
	m_nCount[1] = 0;
	m_nDelay[1] = 1;
	m_nStatus[1] = -1;

	m_nPreLvCount = -1;
	m_nCurLvCount = -1;
	m_nResPreLevel = -1;
	m_nResCurLevel = -1;
	m_nPetLevel = -1;
	m_dwObjId = -1;

	m_bReciveResult[0] = FALSE;
	m_bReciveResult[1] = FALSE;

	m_bEnd = FALSE;
	m_pText = NULL;
	m_bLocked[0] = FALSE;
	m_bLocked[1] = FALSE;

	m_strPathLvImage[0] = MakePath(DIR_ICON, "Icon_PetLevel1_L.dds");
	m_strPathLvImage[1] = MakePath(DIR_ICON, "Icon_PetLevel2_L.dds");
	m_strPathLvImage[2] = MakePath(DIR_ICON, "Icon_PetLevel3_L.dds");
	m_strPathLvImage[3] = MakePath(DIR_ICON, "Icon_PetLevel4_L.dds");
	m_strPathLvImage[4] = MakePath(DIR_ICON, "Icon_PetLevel5_L.dds");
	m_strPathLvImage[5] = MakePath(DIR_ICON, "Icon_PetLevel6_L.dds");
	m_strPathLvImage[6] = MakePath(DIR_ICON, "Icon_PetLevel7_L.dds");
	m_strPathLvImage[7] = MakePath(DIR_ICON, "Icon_PetLevel8_L.dds");
	m_strPathLvImage[8] = MakePath(DIR_ICON, "Icon_PetLevel9_L.dds");
}

CWndPetMiracle::~CWndPetMiracle()
{
}

void CWndPetMiracle::OnDestroy()
{
	CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase(APP_PET_STATUS);
	if (pWndStatus != NULL && m_bLocked[0])
	{
		pWndStatus->LockShowLevel(FALSE, m_nPetLevel - 1, 0);
		m_bLocked[0] = FALSE;
	}
	if (pWndStatus != NULL && m_bLocked[1])
	{
		pWndStatus->LockShowLevel(FALSE, m_nPetLevel, 1);
		m_bLocked[1] = FALSE;
	}
}

void CWndPetMiracle::OnDraw(C2DRender* p2DRender)
{
	CTexture* pTexture;
	CString strPath;
	CEditString strEdit;


	strEdit.SetString(m_strPetLevel[0], D3DCOLOR_XRGB(255, 255, 255), ESSTY_BOLD);
	p2DRender->TextOut_EditString(GetWndCtrl(WIDC_STATIC1)->rect.left + 8, GetWndCtrl(WIDC_STATIC1)->rect.top + 3, strEdit);
	strEdit.SetString(m_strPetLevel[1], D3DCOLOR_XRGB(255, 255, 255), ESSTY_BOLD);
	p2DRender->TextOut_EditString(GetWndCtrl(WIDC_STATIC2)->rect.left + 8, GetWndCtrl(WIDC_STATIC2)->rect.top + 3, strEdit);


	if (m_bReciveResult[0])
		strPath = m_strPathLvImage[m_nPreLvCount];
	else
		strPath = m_strPathLvImage[m_nMiracleLv[0] - 1];
	if (strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(GetWndCtrl(WIDC_STATIC3)->rect.left, GetWndCtrl(WIDC_STATIC3)->rect.top + 4));
	}

	if (m_bReciveResult[1])
		strPath = m_strPathLvImage[m_nCurLvCount];
	else
		strPath = m_strPathLvImage[m_nMiracleLv[1] - 1];

	if (strPath.GetLength() > 0)
	{
		pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
		if (pTexture != NULL)
			pTexture->Render(p2DRender, CPoint(GetWndCtrl(WIDC_STATIC4)->rect.left, GetWndCtrl(WIDC_STATIC4)->rect.top + 4));
	}
}

void CWndPetMiracle::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	if (pWndButton)
	{
		if (::GetLanguage() == LANG_ENG || ::GetLanguage() == LANG_VTN)
			pWndButton->SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButChance.bmp"), 0xffff00ff);
	}


	if (g_pPlayer->HasActivatedSystemPet())
	{
		CItemElem* m_pPetElem = g_pPlayer->GetPetItem();
		m_nPetLevel = m_pPetElem->m_pPet->GetLevel();
		if (m_pPetElem->m_pPet != NULL && (m_nPetLevel == PL_B || m_nPetLevel == PL_A || m_nPetLevel == PL_S))
		{
			switch (m_nPetLevel)
			{
			case PL_B:
				m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_C); //"Lv. C"
				m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_B); //"Lv. B"
				break;
			case PL_A:
				m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_B); //"Lv. B"
				m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_A); //"Lv. A"
				break;
			case PL_S:
				m_strPetLevel[0] = prj.GetText(TID_GAME_PET_GRADE_A); //"Lv. A"
				m_strPetLevel[1] = prj.GetText(TID_GAME_PET_GRADE_S); //"Lv. S"
				break;
			}

			m_nMiracleLv[0] = m_pPetElem->m_pPet->GetAvailLevel(m_nPetLevel - 1);
			m_nMiracleLv[1] = m_pPetElem->m_pPet->GetAvailLevel(m_nPetLevel);
			m_nPreLvCount = m_nMiracleLv[0] - 1;
			m_nCurLvCount = m_nMiracleLv[1] - 1;

			m_pText = (CWndText*)GetDlgItem(WIDC_TEXT_DESC);
			SetDescription(NULL);

			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_OK);
			pButton->EnableWindow(FALSE);
			pButton->SetVisible(FALSE);


			CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase(APP_PET_STATUS);
			if (pWndStatus != NULL)
			{
				pWndStatus->LockShowLevel(TRUE, m_nPetLevel - 1, 0);
				pWndStatus->LockShowLevel(TRUE, m_nPetLevel, 1);
				m_bLocked[0] = m_bLocked[1] = TRUE;
			}

			MoveParentCenter();
		}
		else
			Destroy();
	}
	else
		Destroy();
}

BOOL CWndPetMiracle::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_MIRACLE, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndPetMiracle::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		if (nID == WIDC_BUTTON2)
		{
			CWndButton* pButton;
			pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
			pButton->EnableWindow(FALSE);

			//Send to Server
			if (m_dwObjId != -1)
				g_DPlay.SendPetTamerMiracle(m_dwObjId);
		}
		else if (nID == WIDC_OK || nID == WIDC_CLOSE)
			Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetMiracle::ReceiveResult(int nPreLevel, int nCurLevel)
{
	m_nResPreLevel = nPreLevel;
	m_nResCurLevel = nCurLevel;
	m_nMiracleLv[0] = m_nResPreLevel;
	m_nMiracleLv[1] = m_nResCurLevel;

	m_nStatus[0] = 0;
	m_nStatus[1] = 0;
	m_bReciveResult[0] = TRUE;
	m_bReciveResult[1] = TRUE;
}

void CWndPetMiracle::SetItem(DWORD dwObjId)
{
	m_dwObjId = dwObjId;
}

BOOL CWndPetMiracle::Process()
{
	PreLevelImgProcess();
	CurLevelImgProcess();
	return TRUE;
}

void CWndPetMiracle::PreLevelImgProcess()
{
	if (m_nStatus[0] == 0)
	{
		if (m_nCount[0] % 4 == 0)
		{
			if (m_nCount[0] > 60)
			{
				m_nCount[0] = 0;
				m_nStatus[0] = 1;
			}
			else
			{
				PLAYSND("InfOpen.wav");
				int randnum = rand() % 9;
				if (randnum == m_nPreLvCount)
				{
					randnum++;
					(randnum > 8) ? randnum = 0 : randnum;

				}
				m_nPreLvCount = randnum;
			}
		}
		m_nCount[0]++;
	}
	else if (m_nStatus[0] == 1)
	{
		if (m_nCount[0] > m_nDelay[0])
		{
			if (m_nDelay[0] < 10)
			{
				PLAYSND("InfOpen.wav");
				m_nDelay[0] += 1;
			}
			else
			{
				PLAYSND("InfOpen.wav");

				m_nPreLvCount = m_nResPreLevel;
				m_nStatus[0] = -1;
				m_nDelay[0] = 1;
				m_nCount[0] = 0;
				m_bReciveResult[0] = FALSE;
				m_bEnd = TRUE;

				CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase(APP_PET_STATUS);
				if (pWndStatus != NULL && m_bLocked[0])
				{
					pWndStatus->LockShowLevel(FALSE, m_nPetLevel - 1, 0);
					m_bLocked[0] = FALSE;
				}
			}

			int randnum = rand() % 9;
			if (randnum == m_nPreLvCount)
			{
				randnum++;
				(randnum > 8) ? randnum = 0 : randnum;
			}
			m_nPreLvCount = randnum;
			m_nCount[0] = 0;
		}
		m_nCount[0]++;
	}
}

void CWndPetMiracle::CurLevelImgProcess()
{
	if (m_nStatus[1] == 0)
	{
		if (m_nCount[1] % 4 == 0)
		{
			if (m_nCount[1] > 80)
			{
				m_nCount[1] = 0;
				m_nStatus[1] = 1;
			}
			else
			{
				PLAYSND("InfOpen.wav");
				int randnum = rand() % 9;
				if (randnum == m_nCurLvCount)
				{
					randnum++;
					(randnum > 8) ? randnum = 0 : randnum;

				}
				m_nCurLvCount = randnum;
			}
		}
		m_nCount[1]++;
	}
	else if (m_nStatus[1] == 1)
	{
		if (m_nCount[1] > m_nDelay[1])
		{
			if (m_nDelay[1] < 20)
			{
				PLAYSND("InfOpen.wav");
				m_nDelay[1] += 1;
			}
			else
			{
				PLAYSND("InfOpen.wav");

				m_nCurLvCount = m_nResCurLevel;
				m_nStatus[1] = -1;
				m_nDelay[1] = 1;
				m_nCount[1] = 0;
				m_bReciveResult[1] = FALSE;
				m_bEnd = TRUE;

				CWndPetStatus* pWndStatus = (CWndPetStatus*)g_WndMng.GetWndBase(APP_PET_STATUS);
				if (pWndStatus != NULL && m_bLocked[1])
				{
					pWndStatus->LockShowLevel(FALSE, m_nPetLevel, 1);
					m_bLocked[1] = FALSE;
				}
				CWndButton* pButton;
				pButton = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
				pButton->SetVisible(FALSE);

				pButton = (CWndButton*)GetDlgItem(WIDC_OK);
				pButton->SetVisible(TRUE);
				pButton->EnableWindow(TRUE);
			}

			int randnum = rand() % 9;
			if (randnum == m_nCurLvCount)
			{
				randnum++;
				(randnum > 8) ? randnum = 0 : randnum;
			}
			m_nCurLvCount = randnum;
			m_nCount[1] = 0;
		}
		m_nCount[1]++;
	}
}

void CWndPetMiracle::SetDescription(CHAR* szChar)
{
	CScript scanner;
	BOOL checkflag;
	checkflag = scanner.Load(MakePath(DIR_CLIENT, _T("PetMiracle.inc")));
	szChar = scanner.m_pProg;

	if (m_pText != NULL && checkflag)
	{
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndPetFoodMill Class
//////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPetFoodMill::CWndPetFoodMill()
{
	m_dwObjId = NULL_ID;
	m_pItemElem = NULL;
	m_pWndFoodConfrim = NULL;
	m_pTexture = NULL;
}

CWndPetFoodMill::~CWndPetFoodMill()
{
	SAFE_DELETE(m_pWndFoodConfrim);
}

void CWndPetFoodMill::OnDestroy()
{
	if (m_pItemElem != NULL)
	{
		if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
			m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}
}

void CWndPetFoodMill::OnDraw(C2DRender* p2DRender)
{
	//Draw Food
	ItemProp* pItemProp;

	if (m_pItemElem != NULL)
	{
		pItemProp = m_pItemElem->GetProp();
		if (pItemProp != NULL)
		{
			CPoint point;
			point.x = GetWndCtrl(WIDC_STATIC2)->rect.left;
			point.y = GetWndCtrl(WIDC_STATIC2)->rect.top;

			if (m_pTexture != NULL)
				m_pTexture->Render(p2DRender, point);

			TCHAR szTemp[32];
			_stprintf(szTemp, prj.GetText(TID_GAME_KWAIBAWIBO_PRESENT_NUM), m_nItemCount);
			CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
			m_p2DRender->TextOut(point.x + 36 - size.cx, point.y + 48 - size.cy, szTemp, 0xff0000ff);
		}
	}
}

void CWndPetFoodMill::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);
	pStatic->SetTitle(prj.GetText(TID_GAME_PETFOODMILL_DESC));

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	CWndInventory* pWndInventory = (CWndInventory*)g_WndMng.CreateApplet(APP_INVENTORY);
#endif // __IMPROVE_SYSTEM_VER15

	MoveParentCenter();
}

BOOL CWndPetFoodMill::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return InitDialog(g_Neuz.GetSafeHwnd(), APP_PET_FOODMILL);
}

BOOL CWndPetFoodMill::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		if (m_pItemElem != NULL)
		{
			ItemProp* pItemProp = m_pItemElem->GetProp();
			if (pItemProp->dwItemKind3 == IK3_GEM && pItemProp->dwCost > 0)
			{
				g_DPlay.SendMakePetFeed(m_pItemElem->m_dwObjId, (short)m_nItemCount, m_dwObjId);
			}
			else
				g_WndMng.PutString(prj.GetText(TID_GAME_NOTFOOD), NULL, prj.GetTextColor(TID_GAME_NOTFOOD));
		}
	}
	else if (nID == WIDC_CANCEL || nID == WTBID_CLOSE)
		Destroy();
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPetFoodMill::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC2);
	rect = wndCtrl->rect;
	if (m_pItemElem != NULL && rect.PtInRect(point))
	{
		m_pItemElem->SetExtra(0);
		m_pItemElem = NULL;
	}
}

void CWndPetFoodMill::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndFoodConfrim)
		SAFE_DELETE(m_pWndFoodConfrim);

}
BOOL CWndPetFoodMill::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!pWndFrame || pWndFrame->GetWndId() != APP_INVENTORY)
		return FALSE;

	CItemElem* pTempElem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
	if (!IsUsableItem(pTempElem))
		return FALSE;

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC2);
	if (wndCtrl->rect.PtInRect(point))
	{
		if (pTempElem != NULL)
		{
			if (m_pItemElem != NULL)
			{
				if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
					m_pItemElem->SetExtra(0);
				m_pItemElem = NULL;
			}

			SAFE_DELETE(m_pWndFoodConfrim);
			m_pWndFoodConfrim = new CWndFoodConfirm(2);
			m_pWndFoodConfrim->m_pItemElem = pTempElem;
			m_pWndFoodConfrim->Initialize(this);
		}
	}

	return TRUE;
}

void CWndPetFoodMill::SetItem(DWORD dwObjId)
{
	m_dwObjId = dwObjId;
}

void CWndPetFoodMill::SetItemForFeed(CItemElem* pItemElem, int nCount)
{
	if (!IsUsableItem(pItemElem))
		return;

	m_pItemElem = pItemElem;
	m_nItemCount = nCount;
	m_pItemElem->SetExtra(nCount);

	ItemProp* pItemProp = m_pItemElem->GetProp();

	if (pItemProp != NULL)
		m_pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
}

void CWndPetFoodMill::ReceiveResult(int nResult, int nCount)
{
	switch (nResult)
	{
	case 0:
		g_WndMng.PutString(prj.GetText(TID_GAME_LACKSPACE), NULL, prj.GetTextColor(TID_GAME_LACKSPACE));
		break;
	case 1:
		CString strtmp;
		strtmp.Format(prj.GetText(TID_GAME_PETFEED_MAKE), nCount);
		g_WndMng.PutString(strtmp, NULL, prj.GetTextColor(TID_GAME_PETFEED_MAKE));
		if (m_pItemElem != NULL)
		{
			if (!g_pPlayer->m_vtInfo.IsTrading(m_pItemElem))
				m_pItemElem->SetExtra(0);
			m_pItemElem = NULL;
		}
		if (m_pTexture != NULL)
			m_pTexture = NULL;
		break;
	}
}

////////////////////////////////////////////////////////////////////
//	CWndPetLifeConfirm
////////////////////////////////////////////////////////////////////

CWndPetLifeConfirm::CWndPetLifeConfirm()
{
	m_nId = -1;
}

CWndPetLifeConfirm::~CWndPetLifeConfirm()
{
}

void CWndPetLifeConfirm::OnDestroy()
{
}

void CWndPetLifeConfirm::SetItem(int nId)
{
	m_nId = nId;
}

void CWndPetLifeConfirm::OnDraw(C2DRender* p2DRender)
{
}
void CWndPetLifeConfirm::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();
	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(60, 25);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	rect.DeflateRect(10, 10, 10, 35);
	m_wndText.AddWndStyle(WBS_VSCROLL);
	m_wndText.Create(WBS_NODRAWFRAME, rect, this, 0);
	m_strText = prj.GetText(TID_GAME_PET_USELIFE);
	m_wndText.SetString(m_strText, 0xff000000);
	m_wndText.ResetString();

	m_wndButton1.Create("YES", 0, rect2_1, this, IDYES);
	m_wndButton2.Create("NO", 0, rect2_2, this, IDNO);
	m_wndButton1.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtYes.tga"));
	m_wndButton2.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtNo.tga"));
	m_wndButton1.FitTextureSize();
	m_wndButton2.FitTextureSize();

	MoveParentCenter();
}

BOOL CWndPetLifeConfirm::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_MESSAGEBOX, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndPetLifeConfirm::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndPetLifeConfirm::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndPetLifeConfirm::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndPetLifeConfirm::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndPetLifeConfirm::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == IDYES)
	{
		if (m_nId != -1)
			g_DPlay.SendDoUseItem(m_nId);
	}
	else if (nID == IDNO)
	{

	}

	Destroy();
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

#endif //__CSC_VER9_1


#if __VER >= 15 // __PETVIS

//////////////////////////////////////////////////////////////////////////////////////////////
// CWndBuffPetStatus
//////////////////////////////////////////////////////////////////////////////////////////////
CWndBuffPetStatus::CWndBuffPetStatus()
{
	m_pPetModel = NULL;
	m_pWndConfirmVis = NULL;

	for (int i = 0; i < MAX_VIS; ++i)
		m_pItemElem[i] = NULL;

	m_pTexPetStatusBg = NULL;
	m_fRadius = 0.0f;
}

CWndBuffPetStatus::~CWndBuffPetStatus()
{
	DeleteDeviceObjects();
}

HRESULT CWndBuffPetStatus::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}

HRESULT CWndBuffPetStatus::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_DELETE(m_pWndConfirmVis);
	for (int i = 0; i < MAX_VIS; ++i)
	{
		SAFE_DELETE(m_pItemElem[i]);
	}
	return S_OK;
}

HRESULT CWndBuffPetStatus::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CWndBuffPetStatus::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	RestoreDeviceObjects();

	//Position Control
	CWndStatus* pWndStatus = (CWndStatus*)GetWndBase(APP_STATUS1);
	if (pWndStatus != NULL)
	{
		CRect rectRoot = pWndStatus->GetWndRect();
		CRect rect = GetWindowRect();
		int x = rectRoot.right;
		int y = rectRoot.top;

		CRect kWRect;
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);
		if (pWndWorld)
		{
			kWRect = pWndWorld->GetClientRect();
			if ((x + rect.Width()) > kWRect.right)
				x = rectRoot.left - rect.Width();
		}

		Move(CPoint(x, y));
	}
	else
	{
		CRect rectRoot = m_pWndRoot->GetLayoutRect();
		CPoint point(rectRoot.left + 192, rectRoot.top);
		Move(point);
	}

	m_nCtrlId[0] = WIDC_BUFFPET_SLOT1;
	m_nCtrlId[1] = WIDC_BUFFPET_SLOT2;
	m_nCtrlId[2] = WIDC_BUFFPET_SLOT3;
	m_nCtrlId[3] = WIDC_BUFFPET_SLOT4;
	m_nCtrlId[4] = WIDC_BUFFPET_SLOT5;
	m_nCtrlId[5] = WIDC_BUFFPET_SLOT6;
	m_nCtrlId[6] = WIDC_BUFFPET_SLOT7;
	m_nCtrlId[7] = WIDC_BUFFPET_SLOT8;
	m_nCtrlId[8] = WIDC_BUFFPET_SLOT9;

	m_pTexPetStatusBg = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "BuffpetStatusBg .tga"), 0xffff00ff, TRUE);
}

BOOL CWndBuffPetStatus::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_BUFFPET_STATUS, 0, CPoint(0, 0), pWndParent);
}

void CWndBuffPetStatus::OnDraw(C2DRender* p2DRender)
{
	DrawSlotItems(p2DRender);

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM1);
	CPoint point = CPoint(lpFace->rect.left - 12, lpFace->rect.top - 22);
	if (m_pTexPetStatusBg)
		m_pTexPetStatusBg->Render(p2DRender, point);

	if (!m_pPetModel || !m_pPetModel->m_pModelElem)
		return;

	CObject3D* pObj3D = m_pPetModel->GetObject3D();
	if (!pObj3D)
		return;

	D3DXVECTOR3 vecPos = pObj3D->m_vEvent[0];
	D3DXVECTOR3 vecLookAt;
	float fScale = 1.0f;

	float fModelScale = m_pPetModel->m_pModelElem->m_fScale;
	if (fModelScale < 1.0f && fModelScale > 0.001f)
		vecPos *= (fModelScale - fModelScale * (0.5f + (1.0f - fModelScale) * 0.01f));
	else if (fModelScale > 1.0f)
		vecPos *= (fModelScale - fModelScale * (0.9f + fModelScale * 0.01f));

	vecPos.x += 0.5f;
	vecPos.y += 1.8f;
	vecPos.z -= (3.0f);

	vecLookAt.x = -0.24f;
	vecLookAt.y = 0.28f;
	vecLookAt.z = 1.0f;
	fScale = (1 / m_fRadius) * 1.5f;

	VIEWPORT tmp;
	tmp.vecPos = vecPos;
	tmp.vecLookAt = vecLookAt;
	tmp.vecMatrixScaling = D3DXVECTOR3(fScale, fScale, fScale);

	m_pPetModel->SetTextureEx(m_pPetModel->m_pModelElem->m_nTextureEx);
	g_WndMng.RenderObjectViewport(p2DRender, m_pPetModel, NULL, lpFace, &tmp, GetWndId());
}

BOOL CWndBuffPetStatus::Process()
{
#ifndef __AIO_PETS
	if (!IsValidObj(g_pPlayer))
		return FALSE;

	if (g_pPlayer->HasActivatedVisPet())
	{
		CMover* pMyBuffPet = NULL;

		if (!m_pPetModel)
		{
			pMyBuffPet = prj.GetMover(g_pPlayer->GetEatPetId());
			if (pMyBuffPet)
			{
				lstrcpy(pMyBuffPet->m_szCharacterKey, "MaFl_BuffPet");
				m_pPetModel = (CModelObject*)pMyBuffPet->GetModel();


				m_strTitle = pMyBuffPet->GetName();
			}
			else
				return TRUE;
}
	}
#endif

	return TRUE;
}

#ifdef __AIO_PETS
void CWndBuffPetStatus::SetPet(CMover* pPet)
{
	if (!m_pPetModel)
	{
		m_pPetModel = (CModelObject*)pPet->GetModel();
		m_strTitle = pPet->GetName();

		if (m_pPetModel)
			m_fRadius = m_pPetModel->GetRadius();
	}
}
#endif

void CWndBuffPetStatus::UpdateVisState()
{
	if (g_pPlayer)
		m_cVisStates = g_pPlayer->GetValidVisTable(g_pPlayer->GetVisPetItem());
}

BOOL CWndBuffPetStatus::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pWndFrame = pShortcut->m_pFromWnd->GetFrameWnd();
	if (!pWndFrame)
	{
		assert(0);
		return FALSE;
	}

	int selectedSlot = GetSlotIndexByPoint(point);
	if (selectedSlot < 0)
		return FALSE;

	if (!IsFreeSlot(selectedSlot))
		return FALSE;

	if (APP_INVENTORY == pWndFrame->GetWndId())
	{
		CItemElem* pItem = g_pPlayer->m_Inventory.GetAtId(pShortcut->m_dwId);
		if (!IsUsableItem(pItem))
			return FALSE;

		if (!pItem->GetProp()->IsVis())
			return FALSE;
	}

	if (pWndFrame->GetWndId() != APP_INVENTORY)
	{
		if (APP_BUFFPET_STATUS == pWndFrame->GetWndId())
		{
			int selectedSlot = GetSlotIndexByPoint(point);
			if (selectedSlot < 0)
				return FALSE;

			if (pShortcut->m_dwData == selectedSlot)
				return FALSE;


			g_DPlay.SendSwapVis(pShortcut->m_dwData, selectedSlot);
		}

		return TRUE;
	}



	if (APP_INVENTORY == pWndFrame->GetWndId())
		DoModal_ConfirmQuestion(pShortcut->m_dwId, g_pPlayer->GetId(), ((CItemElem*)pShortcut->m_dwData)->m_dwItemId);
	else
		DoModal_ConfirmQuestion(pShortcut->m_dwId, g_pPlayer->GetId(), pShortcut->m_dwIndex);

	return TRUE;
}

BOOL CWndBuffPetStatus::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

BOOL CWndBuffPetStatus::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
#ifdef __AIO_PETS
	if (message == WNM_CLICKED)
	{
		if (nID == WTBID_CLOSE)
		{
			g_Option.m_bHideWindowLooter = TRUE;
			g_WndMng.PutString("Window deactivated. It will not be opened in the future until you manually open it by clicking on the pet -> Toggle Buff Pet Status.");
		}
	}
#endif

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndBuffPetStatus::OnDestroy()
{
	for (int i = 0; i < MAX_VIS; i++)
	{
		SAFE_DELETE(m_pItemElem[i]);
	}
}

void CWndBuffPetStatus::DrawSlotItems(C2DRender* p2DRender)
{
	if (!g_pPlayer)
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if (!pItem)
		return;

	int nAvailableSlot = pItem->GetPiercingSize();
	if (nAvailableSlot > MAX_VIS)
		return;

	UpdateVisState();

	LPWNDCTRL wndCtrl = NULL;
	for (int i = 0; i < MAX_VIS; ++i)
	{
		if (i < nAvailableSlot)
		{
			DWORD dwItemIndex = pItem->GetPiercingItem(i);

			ItemProp* pProp = prj.GetItemProp(dwItemIndex);
			if (!pProp)
				continue;

			m_pTexture[i] = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);

			if (m_pTexture[i] != NULL)
			{

				DWORD color = (m_cVisStates[i] == SUCCSESS_NEEDVIS ? 0xffffffff : 0x4fffffff);

				wndCtrl = GetWndCtrl(m_nCtrlId[i]);
				if (wndCtrl)
				{
					CPoint pt2 = CPoint(wndCtrl->rect.left, wndCtrl->rect.top);
					m_pTexture[i]->Render2(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top), color);
				}
			}
		}
		else
		{

			wndCtrl = GetWndCtrl(m_nCtrlId[i]);
			CTexture* pTexClosed = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, "Icon_Lock.dds"), 0xffff00ff);
			if (pTexClosed)
				pTexClosed->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));

		}
	}
}


BOOL CWndBuffPetStatus::DoModal_ConfirmQuestion(DWORD dwItemId, OBJID dwObjid, DWORD dwIndex, int nSlot, CWndConfirmVis::ConfirmVisSection eSection)
{
	SAFE_DELETE(m_pWndConfirmVis);
	m_pWndConfirmVis = new CWndConfirmVis;
	m_pWndConfirmVis->m_dwItemId = dwItemId;
	m_pWndConfirmVis->m_objid = dwObjid;
	m_pWndConfirmVis->m_nSlot = nSlot;
	m_pWndConfirmVis->m_eSection = eSection;
	m_pWndConfirmVis->m_dwItemIndex = dwIndex;

	m_pWndConfirmVis->Initialize(this, APP_CONFIRM_ENTER);

	return TRUE;
}

void CWndBuffPetStatus::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndConfirmVis)
		SAFE_DELETE(m_pWndConfirmVis);
}

int CWndBuffPetStatus::GetSlotIndexByPoint(const CPoint& point)
{
	LPWNDCTRL wndCtrl = NULL;
	CRect rect;
	for (int i = 0; i < MAX_VIS; ++i)
	{
		wndCtrl = GetWndCtrl(m_nCtrlId[i]);
		rect = wndCtrl->rect;

		if (rect.PtInRect(point))
			return i;
	}

	return -1;
}

void CWndBuffPetStatus::OnLButtonDown(UINT nFlags, CPoint point)
{


	LPWNDCTRL wndCtrl = NULL;
	CRect rect;

	int selectedSlot = GetSlotIndexByPoint(point);
	if (selectedSlot < 0)
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if (!pItem)
		return;

	DWORD dwItemIndex = pItem->GetPiercingItem(selectedSlot);
	if (0 == dwItemIndex)
		return;

	assert(selectedSlot >= 0 && selectedSlot < MAX_VIS);
	assert(m_pTexture[selectedSlot]);

	m_GlobalShortcut.m_pFromWnd = this;
	m_GlobalShortcut.m_dwShortcut = SHORTCUT_ITEM;
	m_GlobalShortcut.m_dwType = ITYPE_ITEM;
	m_GlobalShortcut.m_dwId = 0;
	m_GlobalShortcut.m_dwIndex = dwItemIndex;
	m_GlobalShortcut.m_pTexture = m_pTexture[selectedSlot];
	m_GlobalShortcut.m_dwData = selectedSlot;
}

void CWndBuffPetStatus::OnLButtonDblClk(UINT nFlags, CPoint point)
{

	int selectedSlot = GetSlotIndexByPoint(point);
	if (selectedSlot < 0)
		return;

	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if (!pItem)
		return;

	DWORD dwItemIndex = pItem->GetPiercingItem(selectedSlot);
	if (0 == dwItemIndex)
		return;

	DoModal_ConfirmQuestion(0, 0, dwItemIndex, selectedSlot, CWndConfirmVis::CVS_UNEQUIP_VIS);
}

BOOL CWndBuffPetStatus::IsVisItem(DWORD index)
{
	ItemProp* pProp = prj.GetItemProp(index);
	if (!pProp)
		return FALSE;

	return pProp->IsVis();
}

BOOL CWndBuffPetStatus::IsFreeSlot(const int index)
{
	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	if (!pItem)
		return FALSE;

	int nAvailableSlot = pItem->GetPiercingSize();
	return (index < nAvailableSlot);
}

void CWndBuffPetStatus::OnMouseWndSurface(CPoint point)
{
	CRect rect;
	LPWNDCTRL wndCtrl = NULL;
	CItemElem* pItem = g_pPlayer->GetVisPetItem();
	assert(pItem);
	if (pItem->GetPiercingSize() == 0)
		return;

	for (int i = 0; i < MAX_VIS; i++)
	{
		wndCtrl = GetWndCtrl(m_nCtrlId[i]);
		rect = wndCtrl->rect;
		if (rect.PtInRect(point))
		{
			ClientToScreen(&point);
			ClientToScreen(&rect);

			DWORD dwIndex = pItem->GetPiercingItem(i);
			if (0 != dwIndex)
			{
				if (NULL == m_pItemElem[i])
					m_pItemElem[i] = new CItemElem;

				m_pItemElem[i]->m_dwItemId = dwIndex;

				time_t endTime = pItem->GetVisKeepTime(i);
				//DWORD remainTime = endTime - timeGetTime( );
				m_pItemElem[i]->m_dwKeepTime = endTime;

			}
			else
			{
				SAFE_DELETE(m_pItemElem[i]);
				continue;
			}

			g_WndMng.PutToolTip_Item(m_pItemElem[i], point, &rect);
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
CWndConfirmVis::CWndConfirmVis()
{
	m_dwItemId = NULL_ID;
	m_objid = NULL_ID;
	m_nSlot = 0;
	m_dwItemIndex = 0;
	m_eSection = CVS_EQUIP_VIS;
}

CWndConfirmVis::~CWndConfirmVis()
{

}


void CWndConfirmVis::OnDraw(C2DRender* p2DRender)
{

}

void CWndConfirmVis::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pOk = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* pCancel = (CWndButton*)GetDlgItem(WIDC_BUTTON2);
	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	AddWndStyle(WBS_MODAL);

	pOk->SetDefault(TRUE);

	ItemProp* pProp = prj.GetItemProp(m_dwItemIndex);
	assert(pProp);

	if (!pProp)
		return;

	CWndText* pText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	CString strTitle;
	switch (m_eSection)
	{
	case CVS_EQUIP_VIS: strTitle.Format(GETTEXT(TID_GAME_BUFFPET_EQUIP), pProp->szName); break;
	case CVS_UNEQUIP_VIS: strTitle.Format(GETTEXT(TID_GAME_BUFFPET_CANCEL), pProp->szName); break;
	case CVS_EQUIP_VISKEY:
	{
		MoverProp* pProp = NULL;
		if (g_pPlayer)
		{
			CMover* pPet = prj.GetMover(g_pPlayer->GetEatPetId());
			if (pPet)
				pProp = pPet->GetProp();
		}

		if (pProp)
			strTitle.Format(GETTEXT(TID_GAME_BUFFPET_KET01), pProp->szName);
	}break;

	case CVS_PICKUP_TO_BUFF:
		strTitle = GETTEXT(TID_GAME_PET_TRAN);
		break;

#ifdef __PROTECT_AWAKE
	case ETC_PROTECT_AWAKE:
		strTitle = GETTEXT(TID_GAME_REGARDLESS_USE01);
		break;
#endif //AWAKE_PROTECT
	}

	pText->SetString(strTitle);


	pEdit->Move(-100, -100);
	pEdit->SetFocus();
}

BOOL CWndConfirmVis::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	InitDialog(g_Neuz.GetSafeHwnd(), APP_CONFIRM_ENTER, WBS_KEY, 0, pWndParent);
	MoveParentCenter();
	return TRUE;
}
BOOL CWndConfirmVis::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndConfirmVis::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndConfirmVis::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndConfirmVis::SendEquipPacket()
{
	switch (m_eSection)
	{
	case CVS_EQUIP_VIS:			g_DPlay.SendDoUseItem(m_dwItemId); break;
	case CVS_UNEQUIP_VIS:		g_DPlay.SendRemoveVis(m_nSlot);	break;
	case CVS_EQUIP_VISKEY:		g_DPlay.SendDoUseItem(m_dwItemId); break;
	case CVS_PICKUP_TO_BUFF:	g_DPlay.SendDoUseItem(m_dwItemId); break;
#ifdef __PROTECT_AWAKE
	case ETC_PROTECT_AWAKE: g_DPlay.SendDoUseItem(m_dwItemId); break;
#endif //__PROTECT_AWAKE

	default: break;
	}
}

BOOL CWndConfirmVis::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{

	if (WIDC_EDIT1 == nID && EN_RETURN == message)
	{
		SendEquipPacket();
		Destroy();
		return CWndNeuz::OnChildNotify(message, nID, pLResult);
	}

	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case WIDC_BUTTON2:
			Destroy();
			break;
		case WIDC_BUTTON1:
			SendEquipPacket();
			Destroy();
			break;
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
#endif //#ifdef __PETVIS