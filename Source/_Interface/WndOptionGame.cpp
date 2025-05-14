#include "stdafx.h"
#include "AppDefine.h"
#include "WndOptionGame.h"
#include "DPClient.h"
#include "defineSound.h"

extern	CDPClient	g_DPlay;

#define		MAX_SLIDER		100


/****************************************************
  WndId : APP_OPTION_GAME - Game Option
****************************************************/

CWndOptionGame::CWndOptionGame()
{
#if __VER >= 12 // __UPDATE_OPT
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;

	m_nStep[0] = 0;
	m_nStep[1] = 0;
#endif
}
CWndOptionGame::~CWndOptionGame()
{
}
void CWndOptionGame::OnDraw(C2DRender* p2DRender)
{
#if __VER >= 12 // __UPDATE_OPT
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect(lpWndCtrl->rect, 0);
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 100).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 100).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));
#endif
}

void CWndOptionGame::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pWndButton[2];

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO2);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[!(g_Option.m_bTrade)]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO3);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO4);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[!(g_Option.m_bSay)]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO5);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO6);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[!(g_Option.m_bParty)]->SetCheck(TRUE);

	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "WndVolumeBar.tga"), 0xffff00ff, TRUE);
	m_TexturePt.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "ButtSpin.tga"), 0xffffffff, TRUE);
	m_nStep[0] = (int)(g_Option.m_fEffectVolume * 100);
	m_nStep[1] = (int)(g_Option.m_fBGMVolume * 100);

	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	pWndAlpha->SetCheck(g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

	CWndButton* pWndCheckBattleBGM = (CWndButton*)GetDlgItem(WIDC_CHECK_BATTLE_BGM);
	pWndCheckBattleBGM->SetCheck(g_Option.m_bBattleBGM ? TRUE : FALSE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO7);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO8);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[!(g_Option.m_bMessengerJoin)]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_MESSENGER_ON);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_MESSENGER_OFF);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[!(g_Option.m_bMessenger)]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO12);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO13);
	if (g_pPlayer)
	{
		pWndButton[0]->SetGroup(TRUE);
		if (g_pPlayer->IsMode(EQUIP_DENIAL_MODE))
			pWndButton[1]->SetCheck(TRUE);
		else
			pWndButton[0]->SetCheck(TRUE);
	}
	else
	{
		pWndButton[0]->EnableWindow(FALSE);
		pWndButton[1]->EnableWindow(FALSE);
	}


	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO14);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO15);
	pWndButton[0]->SetGroup(TRUE);

	if (g_Option.m_bAutoAttack)
		pWndButton[0]->SetCheck(TRUE);
	else
		pWndButton[1]->SetCheck(TRUE);

#ifdef __FL_UPDATE_TASKBAR
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	pWndButton[0]->SetCheck(g_Option.m_bRekeyTask);
#endif

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
BOOL CWndOptionGame::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_GAME, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptionGame::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOptionGame::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptionGame::OnLButtonUp(UINT nFlags, CPoint point)
{
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
}
void CWndOptionGame::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	int nResult = 0;
	nResult = GetSliderStep(WIDC_CUSTOM2, m_nStep[0], point);
	if (nResult == -1)
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_fEffectVolume = m_nStep[0] * 0.01f;
			g_SoundMng.m_nSoundVolume = (int)(-(1.0f - g_Option.m_fEffectVolume) * 5000);
		}

	nResult = GetSliderStep(WIDC_CUSTOM1, m_nStep[1], point);
	if (nResult == -1)
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_fBGMVolume = m_nStep[1] * 0.01f;
			SetVolume(g_Option.m_fBGMVolume);
		}
}
BOOL CWndOptionGame::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	CWndButton* pWndCheckBattleBGM = (CWndButton*)GetDlgItem(WIDC_CHECK_BATTLE_BGM);

	switch (nID)
	{
	case WIDC_RADIO1:
		g_Option.m_bTrade = 1;
		break;
	case WIDC_RADIO2:
		g_Option.m_bTrade = 0;
		break;
	case WIDC_RADIO3:
		g_Option.m_bSay = 1;
		break;
	case WIDC_RADIO4:
		g_Option.m_bSay = 0;
		break;
	case WIDC_RADIO5:
		g_Option.m_bParty = 1;
		break;
	case WIDC_RADIO6:
		g_Option.m_bParty = 0;
		break;
	case WIDC_CHECK3:
		if (pWndAlpha->GetCheck())
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
	case WIDC_CHECK_BATTLE_BGM:
	{
		g_Option.m_bBattleBGM = (pWndCheckBattleBGM->GetCheck() == TRUE) ? TRUE : FALSE;
		break;
	}
	case WIDC_RADIO7:
		g_Option.m_bMessengerJoin = 1;
		break;
	case WIDC_RADIO8:
		g_Option.m_bMessengerJoin = 0;

	case WIDC_MESSENGER_ON:
		g_Option.m_bMessenger = 1;
		break;
	case WIDC_MESSENGER_OFF:
		g_Option.m_bMessenger = 0;
		break;
	case WIDC_RADIO12:
		if (g_pPlayer)
			g_DPlay.SendQueryEquipSetting(TRUE);
		break;
	case WIDC_RADIO13:
		if (g_pPlayer)
			g_DPlay.SendQueryEquipSetting(FALSE);
		break;
	case WIDC_RADIO14:
		g_Option.m_bAutoAttack = 1;
		break;
	case WIDC_RADIO15:
		g_Option.m_bAutoAttack = 0;
		break;
#ifdef __FL_UPDATE_TASKBAR
	case WIDC_CHECK4:
	{
		CWndButton* pWndCheck4 = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		g_Option.m_bRekeyTask = (pWndCheck4->GetCheck() == TRUE) ? TRUE : FALSE;
		break;
	}
#endif
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


void CWndOptionGame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonClick)
	{
		GetRangeSlider(WIDC_CUSTOM2, m_nStep[0], point);

		g_Option.m_fEffectVolume = m_nStep[0] * 0.01f;
		g_SoundMng.m_nSoundVolume = (int)(-(1.0f - g_Option.m_fEffectVolume) * 5000);
	}

	if (m_bLButtonClick2)
	{
		GetRangeSlider(WIDC_CUSTOM1, m_nStep[1], point);

		g_Option.m_fBGMVolume = m_nStep[1] * 0.01f;
		SetVolume(g_Option.m_fBGMVolume);
	}
}

CPoint CWndOptionGame::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision + 1));
}

int CWndOptionGame::GetSliderStep(DWORD dwWndId, int& nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	CRect crect = lpWndCtrl->rect;

	crect.left += GetStepPos(nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, MAX_SLIDER).x;
	crect.right = crect.left + 16;

	if (PtInRect(crect, point))
	{
		return -1;
	}

	crect = lpWndCtrl->rect;
	if (PtInRect(crect, point))
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

		if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
			nStep += 2;//nWidth / 10;
		else
			nStep -= 2;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptionGame::GetRangeSlider(DWORD dwWndId, int& nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

	if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep += 5;//nWidth / 10;
	if (point.x < lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep -= 5;//nWidth / 10;

	if (nStep < 0)
		nStep = 0;

	if (nStep > MAX_SLIDER)
		nStep = MAX_SLIDER;
}

HRESULT CWndOptionGame::InvalidateDeviceObjects()
{
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptionGame::RestoreDeviceObjects()
{
	return CWndNeuz::RestoreDeviceObjects();
}
