#include "stdafx.h"
#include "AppDefine.h"
#include "WndNotice.h"




CWndInfoNotice::CWndInfoNotice()
{
	m_vecStrBanner.clear();
	m_dwChangeBannerTime = g_tmCurrent + SEC(10);
	m_atexPannel = NULL;
}
CWndInfoNotice::~CWndInfoNotice()
{
	SAFE_DELETE(m_atexPannel);
}
void CWndInfoNotice::OnDraw(C2DRender* p2DRender)
{
}
void CWndInfoNotice::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();


	CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndCheck->SetCheck(!g_Option.m_bNotice);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);

	CFileStatus fileStatus;

	CString strnotice = "Notice.inc";
	if (CFile::GetStatus("Client\\" + strnotice, fileStatus) == TRUE)
	{
		CFileIO file;
		if (file.Open("Client\\" + strnotice, "rb"))
		{
			int nLength = file.GetLength();
			TCHAR* pChar = new TCHAR[nLength + 1];
			file.Read(pChar, nLength);
			pChar[nLength] = 0;
			pWndText->m_string.AddParsingString(pChar);
			pWndText->ResetString();
			SAFE_DELETE(pChar);
		}
	}

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point(rectRoot.right - rectWindow.Width(), 110);
	Move(point);
	MoveParentCenter();

	///////////////////////////////////////////////////////////////////////////////////
	int nCount = 0;
	CScript script;
	if (script.Load(MakePath(DIR_THEME, "TexBannerList.inc")))
	{
		int nLang;
		nLang = script.GetNumber();
		do
		{
			if (nLang == ::GetLanguage())
			{
				script.GetToken();

				nCount = atoi(script.token);

				script.GetToken();

				for (int i = 0; i < nCount; i++)
				{
					CString addStr = script.token;
					m_vecStrBanner.push_back(addStr);
					script.GetToken();
				}

				if (nCount <= 0)
				{
					Error("TexBannerList.inc의 갯수가 0이다");
					return;
				}

				break;
			}
			else
				script.GetLastFull();

			nLang = script.GetNumber();
		} while (script.tok != FINISHED);
	}

	SAFE_DELETE(m_atexPannel);
	if (nCount > 0)
	{
		m_atexPannel = new IMAGE;
		LoadImage(MakePath(DIR_THEME, m_vecStrBanner[xRandom(nCount)]), m_atexPannel);
		AdjustWndBase();
	}
	///////////////////////////////////////////////////////////////////////////////////
}

void CWndInfoNotice::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat)
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();

	lpWndCtrl = GetWndCtrl(WIDC_COMMER_BANNER1);
	pt = lpWndCtrl->rect.TopLeft() + pt2;

	if (m_atexPannel)
		PaintTexture(pDest, m_atexPannel, pt, size);
}

BOOL CWndInfoNotice::Process()
{
	if (g_tmCurrent > m_dwChangeBannerTime)
	{
		m_dwChangeBannerTime = g_tmCurrent + SEC(10);
		SAFE_DELETE(m_atexPannel);
		m_atexPannel = new IMAGE;
		LoadImage(MakePath(DIR_THEME, m_vecStrBanner[xRandom(m_vecStrBanner.size())]), m_atexPannel);
		AdjustWndBase();
	}

	return TRUE;
}


BOOL CWndInfoNotice::Initialize(CWndBase* pWndParent, DWORD /*dwWndId*/)
{

	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_INFO_NOTICE, 0, CPoint(0, 0), pWndParent);
}
BOOL CWndInfoNotice::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndInfoNotice::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndInfoNotice::OnLButtonUp(UINT nFlags, CPoint point)
{
}
BOOL CWndInfoNotice::OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message)
{
	if (::GetLanguage() == LANG_KOR)
	{
		DWORD dwCursor = 0xffffffff;

		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl(WIDC_COMMER_BANNER1);
		if (PtInRect(&pCustom->rect, GetMousePoint()))
		{
			dwCursor = CUR_SELECT;
		}

		if (dwCursor == 0xffffffff)
			CWndNeuz::OnSetCursor(pWndBase, nHitTest, message);
		else
			SetMouseCursor(dwCursor);
	}
	return TRUE;
}
void CWndInfoNotice::OnLButtonDown(UINT nFlags, CPoint point)
{

}
BOOL CWndInfoNotice::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_CHECK1:
		g_Option.m_bNotice = !g_Option.m_bNotice;
		Destroy();
		break;
	case WIDC_CLOSE:
		Destroy();
		return TRUE;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

