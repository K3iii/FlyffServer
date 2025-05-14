// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppDefine.h"
#include "Resdata.h"
#include "defineSound.h"
#include "definetext.h"
#include "DPClient.h"

#define		MAX_SLIDER		10

extern	CDPClient	g_DPlay;

CWndTotalOption::CWndTotalOption()
{
}
CWndTotalOption::~CWndTotalOption()
{
}

void CWndTotalOption::UpdateDataAll()
{

}

void CWndTotalOption::OnDraw(C2DRender* p2DRender)
{
}
void CWndTotalOption::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem(WIDC_TABCTRL1);
	CRect rect = GetClientRect();
	rect.left = 5;
	rect.top = 0;


	m_OptTabVideoSnd.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_AV12);
	m_OptTabGame.Create(WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_OPTEX_GAME12);
	WTCITEM tabTabItem;
	tabTabItem.mask = WTCIF_TEXT | WTCIF_PARAM;

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_VIDEOAUDIO);

	tabTabItem.pWndBase = &m_OptTabVideoSnd;
	pWndTabCtrl->InsertItem(0, &tabTabItem);

	tabTabItem.pszText = prj.GetText(TID_TOOLTIP_OPT_GAME);

	tabTabItem.pWndBase = &m_OptTabGame;
	pWndTabCtrl->InsertItem(1, &tabTabItem);
	MoveParentCenter();
}

BOOL CWndTotalOption::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndTotalOption::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndTotalOption::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndTotalOption::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTotalOption::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndTotalOption::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndOption::CWndOption()
{
}
CWndOption::~CWndOption()
{
}
void CWndOption::OnDraw(C2DRender* p2DRender)
{
}
void CWndOption::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndAlpha->SetCheck(g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndHelp->SetCheck(g_Option.m_nInstantHelp);

	CWndButton* pWndButton[4];

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_bCameraLock);

	CWndButton* pWndZoom = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	pWndZoom->SetCheck(!g_Option.m_bZoomLimit);
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}
BOOL CWndOption::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_ETC, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOption::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOption::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOption::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndOption::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndOption::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndHelp = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	CWndButton* pWndRoll = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem(WIDC_CHECK4);
	CWndButton* pWndZoomLimit = (CWndButton*)GetDlgItem(WIDC_CHECK5);

	switch (nID)
	{
	case WIDC_CHECK1:
		if (pWndAlpha->GetCheck())
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
		break;
	case WIDC_CHECK2:
		if (pWndHelp->GetCheck())
			g_Option.m_nInstantHelp = TRUE;
		else
			g_Option.m_nInstantHelp = FALSE;
		break;
	case WIDC_CHECK4:
	{
		if (pWndCamearaLock->GetCheck())
			g_Option.m_bCameraLock = TRUE;
		else
			g_Option.m_bCameraLock = FALSE;
	}
	break;

	case WIDC_CHECK5:
	{
		if (pWndZoomLimit->GetCheck())
			g_Option.m_bZoomLimit = FALSE;
		else
			g_Option.m_bZoomLimit = TRUE;
	}
	break;

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndOptSound::CWndOptSound()
{
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;

	m_nStep[0] = 0;
	m_nStep[1] = 0;
}
CWndOptSound::~CWndOptSound()
{

}
void CWndOptSound::OnDraw(C2DRender* p2DRender)
{
	CPoint pt;

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	p2DRender->RenderFillRect(lpWndCtrl->rect, 0);
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 10).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));

}
void CWndOptSound::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndButton* pWndButton[3];

	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_VOICEON);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_VOICEOFF);

	CWndStatic* pWndVoice = (CWndStatic*)GetDlgItem(WIDC_STATIC2);
	pWndVoice->SetVisible(FALSE);
	pWndButton[1]->SetVisible(FALSE);
	pWndButton[0]->SetVisible(FALSE);

	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "WndVolumeBar.tga"), 0xffff00ff, TRUE);
	m_TexturePt.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "ButtSpin.tga"), 0xffffffff, TRUE);
	m_nStep[0] = (int)(g_Option.m_fEffectVolume * 10);
	m_nStep[1] = (int)(g_Option.m_fBGMVolume * 10);

}

BOOL CWndOptSound::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTEX_SOUND, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptSound::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOptSound::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptSound::OnLButtonUp(UINT nFlags, CPoint point)
{
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
}
void CWndOptSound::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	int nResult = 0;
	nResult = GetSliderStep(WIDC_CUSTOM1, m_nStep[0], point);
	if (nResult == -1)
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_fEffectVolume = m_nStep[0] * 0.1f;
			g_SoundMng.m_nSoundVolume = (int)(-(1.0f - g_Option.m_fEffectVolume) * 5000);
		}
	///////////////////////////////////
	nResult = GetSliderStep(WIDC_CUSTOM2, m_nStep[1], point);
	if (nResult == -1)
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_fBGMVolume = m_nStep[1] * 0.1f;
			SetVolume(g_Option.m_fBGMVolume);
		}

}

void CWndOptSound::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonClick)
	{
		GetRangeSlider(WIDC_CUSTOM1, m_nStep[0], point);

		g_Option.m_fEffectVolume = m_nStep[0] * 0.1f;
		g_SoundMng.m_nSoundVolume = (int)(-(1.0f - g_Option.m_fEffectVolume) * 5000);
	}

	if (m_bLButtonClick2)
	{
		GetRangeSlider(WIDC_CUSTOM2, m_nStep[1], point);

		g_Option.m_fBGMVolume = m_nStep[1] * 0.1f;
		SetVolume(g_Option.m_fBGMVolume);
	}
}
BOOL CWndOptSound::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CPoint CWndOptSound::GetStepPos(int nStep, int nWidth, int nDivision)
{
	return nStep * (nWidth / (nDivision + 1));
}

int CWndOptSound::GetSliderStep(DWORD dwWndId, int& nStep, CPoint point)
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
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptSound::GetRangeSlider(DWORD dwWndId, int& nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

	if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep += 1;//nWidth / 10;
	if (point.x < lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep -= 1;//nWidth / 10;

	if (nStep < 0)
		nStep = 0;

	if (nStep > MAX_SLIDER)
		nStep = MAX_SLIDER;
}

HRESULT CWndOptSound::InvalidateDeviceObjects()
{
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptSound::RestoreDeviceObjects()
{
	return CWndNeuz::RestoreDeviceObjects();
}


CWndOptVideo::CWndOptVideo()
{
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;

	m_nStep[0] = 0;
	m_nStep[1] = 0;
}
CWndOptVideo::~CWndOptVideo()
{
	g_Option.Save("neuz.ini");
}
HRESULT CWndOptVideo::InvalidateDeviceObjects()
{
	return CWndNeuz::InvalidateDeviceObjects();
}
HRESULT CWndOptVideo::RestoreDeviceObjects()
{
	return CWndNeuz::RestoreDeviceObjects();
}

CPoint CWndOptVideo::GetStepPos(int nStep, int nWidth, int nDivision)
{
	if (nDivision == 1 && nStep)
		return nStep * nWidth - 16;
	else
		return nStep * (nWidth / (nDivision + 1));
}

void CWndOptVideo::OnDraw(C2DRender* p2DRender)
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
	CPoint pt1 = lpWndCtrl->rect.TopLeft();
	CPoint pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[0], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 1).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM3);
	pt1 = lpWndCtrl->rect.TopLeft();
	pt2 = lpWndCtrl->rect.BottomRight() - lpWndCtrl->rect.TopLeft();
	m_Texture.Render(p2DRender, pt1, pt2);
	pt = pt1;
	pt.x += GetStepPos(m_nStep[1], lpWndCtrl->rect.right - lpWndCtrl->rect.left, 1).x;
	m_TexturePt.Render(p2DRender, pt, CPoint(16, 16));


}
void CWndOptVideo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	CWndButton* pWndButton[3];

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_OBJECT_LOD_HIGH);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_OBJECT_LOD_MID);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_OBJECT_LOD_LOW);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[g_Option.m_nObjectDetail]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_OBJECT_DISTANT_HIGH);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_OBJECT_DISTANT_MID);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_OBJECT_DISTANT_LOW);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[g_Option.m_nObjectDistant]->SetCheck(TRUE);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_SHADOW_HIGH);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_SHADOW_MID);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_SHADOW_LOW);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[g_Option.m_nShadow]->SetCheck(TRUE);
	if (g_Neuz.m_d3dCaps.MaxSimultaneousTextures <= 2)
	{
		pWndButton[0]->EnableWindow(FALSE);
		pWndButton[1]->EnableWindow(FALSE);
		pWndButton[2]->EnableWindow(FALSE);
	}

	if (::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_JAP)
	{
		pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK4);
		if (pWndButton[0])
			pWndButton[0]->SetVisible(FALSE);
	}

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK6);
	pWndButton[0]->SetCheck(g_Option.m_nPlayerName);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK7);
	pWndButton[0]->SetCheck(g_Option.m_nOtherPlayerName);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK8);
	pWndButton[0]->SetCheck(g_Option.m_nMonName);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	pWndButton[0]->SetCheck(g_Option.m_bDamageRender);
	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndButton[0]->SetCheck(!g_Option.m_bZoomLimit);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK5);
	if (pWndButton[0])
		pWndButton[0]->SetCheck(g_Option.m_bCameraLock);
	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "WndVolumeBar.tga"), 0xffff00ff, TRUE);
	m_TexturePt.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "ButtSpin.tga"), 0xffffffff, TRUE);

	m_nStep[0] = !g_Option.m_bSFXItemRenderOff;
	m_nStep[1] = !g_Option.m_bSFXRenderOff;

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK3);
	if (pWndButton[0]) pWndButton[0]->SetCheck(g_Option.m_nWeatherEffect);

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_CHECK9);
	pWndButton[0]->SetCheck(g_Option.m_bNightMode);


}
BOOL CWndOptVideo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_VIDEO, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndOptVideo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOptVideo::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptVideo::OnLButtonUp(UINT nFlags, CPoint point)
{
	PLAYSND(SND_NONE);
	ReleaseCapture();
	m_bLButtonClick = FALSE;
	m_bLButtonClick2 = FALSE;
}
void CWndOptVideo::OnLButtonDown(UINT nFlags, CPoint point)
{

	SetCapture();

	int nResult = 0;

	nResult = GetSliderStep(WIDC_CUSTOM2, m_nStep[0], point, 1);
	if (nResult == -1)
	{
		m_bLButtonClick = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_bSFXItemRenderOff = !m_nStep[0];
		}

	nResult = GetSliderStep(WIDC_CUSTOM3, m_nStep[1], point, 1);
	if (nResult == -1)
	{
		m_bLButtonClick2 = TRUE;
		return;
	}
	else
		if (nResult == 1)
		{
			g_Option.m_bSFXRenderOff = !m_nStep[1];
		}

}

void CWndOptVideo::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLButtonClick)
	{
		GetRangeSlider(WIDC_CUSTOM2, m_nStep[0], point, 1);
		g_Option.m_bSFXItemRenderOff = !m_nStep[1];
	}

	if (m_bLButtonClick2)
	{
		GetRangeSlider(WIDC_CUSTOM3, m_nStep[1], point, 1);
		g_Option.m_bSFXRenderOff = !m_nStep[1];
	}
}

void CWndOptVideo::OnMouseWndSurface(CPoint point)
{
}
BOOL CWndOptVideo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*)*pLResult;
	CWndButton* pWndZoomLimit = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndCamearaLock = (CWndButton*)GetDlgItem(WIDC_CHECK5);

	switch (nID)
	{
	case WIDC_OBJECT_LOD_HIGH:
		g_Option.m_nObjectDetail = 0;
		break;
	case WIDC_OBJECT_LOD_MID:
		g_Option.m_nObjectDetail = 1;
		break;
	case WIDC_OBJECT_LOD_LOW:
		g_Option.m_nObjectDetail = 2;
		break;

	case WIDC_OBJECT_DISTANT_HIGH:
		g_Option.m_nObjectDistant = 0;
		break;
	case WIDC_OBJECT_DISTANT_MID:
		g_Option.m_nObjectDistant = 1;
		break;
	case WIDC_OBJECT_DISTANT_LOW:
		g_Option.m_nObjectDistant = 2;
		break;
	case WIDC_SHADOW_HIGH:
		g_Option.m_nShadow = 0;
		CreateShadowMap(m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat);
		break;
	case WIDC_SHADOW_MID:
		g_Option.m_nShadow = 1;
		CreateShadowMap(m_pApp->m_pd3dDevice, g_Neuz.m_d3dpp.BackBufferFormat);
		break;
	case WIDC_SHADOW_LOW:
		g_Option.m_nShadow = 2;
		break;
	case WIDC_CHECK4:
		if (::GetLanguage() != LANG_KOR)
		{
			pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK4);
			if (pWndButton)
			{
				g_Option.m_bStartFullScreen = (pWndButton->GetCheck());
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_GAME_RESETTING), MB_OK, this);
			}
		}
		break;
	case WIDC_CHECK2:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		g_Option.m_bDamageRender = pWndButton->GetCheck();
		break;

	case WIDC_CHECK6:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK6);
		g_Option.m_nPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK7:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK7);
		g_Option.m_nOtherPlayerName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK8:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK8);
		g_Option.m_nMonName = pWndButton->GetCheck();
		break;
	case WIDC_CHECK1:
	{
		if (pWndZoomLimit->GetCheck())
			g_Option.m_bZoomLimit = FALSE;
		else
		{
			g_Option.m_bZoomLimit = TRUE;
			g_Neuz.m_camera.m_fZoom = 7.0f;
		}
	}
	break;

	case WIDC_CHECK5:
	{
		if (pWndCamearaLock->GetCheck())
			g_Option.m_bCameraLock = TRUE;
		else
			g_Option.m_bCameraLock = FALSE;
	}
	break;

	case WIDC_CHECK3:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		if (pWndButton) g_Option.m_nWeatherEffect = pWndButton->GetCheck();
		break;

	case WIDC_CHECK9:
		pWndButton = (CWndButton*)GetDlgItem(WIDC_CHECK9);
		g_Option.m_bNightMode = pWndButton->GetCheck();

		if (g_pPlayer && g_pPlayer->GetWorld())
			g_pPlayer->GetWorld()->InitWorldEnvir();

		break;

	case WIDC_OK:
		Destroy();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


CWndOptWindow::CWndOptWindow()
{
}
CWndOptWindow::~CWndOptWindow()
{
}
void CWndOptWindow::OnDraw(C2DRender* p2DRender)
{
}
void CWndOptWindow::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndButton* pWndEffect = (CWndButton*)GetDlgItem(WIDC_CHECK);
	pWndEffect->SetCheck(g_Option.m_nWindowAlpha == 255 ? FALSE : TRUE);

	CWndButton* pWndButton[3];

	pWndButton[0] = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	pWndButton[1] = (CWndButton*)GetDlgItem(WIDC_RADIO2);
	pWndButton[2] = (CWndButton*)GetDlgItem(WIDC_RADIO3);
	pWndButton[0]->SetGroup(TRUE);
	pWndButton[g_Option.m_nWindowEffect]->SetCheck(TRUE);

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndOptWindow::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_WINDOW, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptWindow::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOptWindow::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndOptWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndOptWindow::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndButton* pWndAlpha = (CWndButton*)GetDlgItem(WIDC_CHECK);
	CWndButton* pWndEffect = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	switch (nID)
	{
	case WIDC_CHECK:
		if (pWndAlpha->GetCheck())
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 128;
		else
			CWndBase::m_nAlpha = g_Option.m_nWindowAlpha = 255;
	case WIDC_RADIO1:
		break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

CWndOptMyInfo::CWndOptMyInfo()
{
}
CWndOptMyInfo::~CWndOptMyInfo()
{
}
void CWndOptMyInfo::OnDraw(C2DRender* p2DRender)
{
}
void CWndOptMyInfo::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();
}

BOOL CWndOptMyInfo::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_OPTION_MYINFO, 0, CPoint(0, 0), pWndParent);
}

BOOL CWndOptMyInfo::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndOptMyInfo::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndOptMyInfo::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndOptMyInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
}
BOOL CWndOptMyInfo::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}



int CWndOptVideo::GetSliderStep(DWORD dwWndId, int& nStep, CPoint point)
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
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

int CWndOptVideo::GetSliderStep(DWORD dwWndId, int& nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	CRect crect = lpWndCtrl->rect;

	crect.left += GetStepPos(nStep, lpWndCtrl->rect.right - lpWndCtrl->rect.left, nDivision).x;
	crect.right = crect.left + 16;

	if (PtInRect(crect, point))
	{
		return -1;
	}

	crect = lpWndCtrl->rect;
	if (PtInRect(crect, point))
	{
		int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

		if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, nDivision).x)
			nStep += 1;//nWidth / 10;
		else
			nStep -= 1;//nWidth / 10;

		return 1;
	}

	return 0;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int& nStep, CPoint point, int nDivision)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

	if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, nDivision).x)
		nStep += 1;//nWidth / 10;
	if (point.x < lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, nDivision).x)
		nStep -= 1;//nWidth / 10;

	if (nStep < 0)
		nStep = 0;

	if (nStep > nDivision)
		nStep = nDivision;
}

void CWndOptVideo::GetRangeSlider(DWORD dwWndId, int& nStep, CPoint point)
{
	LPWNDCTRL lpWndCtrl = GetWndCtrl(dwWndId);
	int nWidth = lpWndCtrl->rect.right - lpWndCtrl->rect.left;

	if (point.x > lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep += 1;//nWidth / 10;
	if (point.x < lpWndCtrl->rect.left + GetStepPos(nStep, nWidth, MAX_SLIDER).x)
		nStep -= 1;//nWidth / 10;

	if (nStep < 0)
		nStep = 0;

	if (nStep > MAX_SLIDER)
		nStep = MAX_SLIDER;
}
