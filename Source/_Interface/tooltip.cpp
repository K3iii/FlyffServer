// MainFrm.cpp : implementation of the CMainFrame class


#include "stdafx.h"


#include "DialogMsg.h"
extern CDialogMsg g_DialogMsg;

#if __VER >= 9 // __CSC_VER9_1
#include "defineText.h"
#endif //__CSC_VER9_1

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
CToolTip::CToolTip() :
	m_rectRender(0, 0, 0, 0),
	m_strToolTip(_T("")),
	m_nSubToolTipFlag(CWndMgr::TOOL_TIP_SWITCH_MAIN),
	m_nRevisedRect(0, 0, 0, 0),
	m_nSubToolTipNumber(CWndMgr::TOOL_TIP_SWITCH_MAIN)
#else // __IMPROVE_SYSTEM_VER15
CToolTip::CToolTip()
#endif // __IMPROVE_SYSTEM_VER15
{
	bElementt = 0;
	m_bReadyToopTip = FALSE;
	m_dwToolTipId = -1;
	m_ptToolTip = 0;
	m_ptOldToolTip = 0;
	m_bPutToolTip = FALSE;
	m_bEnable = TRUE;
	m_nPosition = 0;
	m_bToolTip = FALSE;
	m_nAlpha = 255;
	m_nAdded = 0;
	m_nSlot = 0;
	m_pUltimateTexture = NULL;
	m_pJewelBgTexture = NULL;
	m_vecMapMonsterID.clear();
#ifdef __FL_PACKED_ITEMS
	m_vecPackage.clear();
#endif

}
CToolTip::~CToolTip()
{
	Delete();
}
void CToolTip::Delete()
{
	int nloadTexture = 0;
	for (int i = 0; i < MAX_TT; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			m_apTextureToolTip[j].DeleteDeviceObjects();
			++nloadTexture;
		}
	}
}

void CToolTip::InitTexture()
{
	CString szTextName;
	CString szTextNamebuf;
	szTextNamebuf = "WndTooltipTile";

	char szBuf[32];
	int nloadTexture = 0;
	for (int i = 0; i < MAX_TT; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			szTextName = szTextNamebuf;
			sprintf(szBuf, "%02d", (i * 10) + j);
			szTextName += szBuf;
			szTextName += ".tga";
			m_apTextureToolTip[nloadTexture].LoadTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, szTextName), 0xffff00ff, TRUE);
			++nloadTexture;
		}
	}

	CString strPath;

	if (::GetLanguage() == LANG_FRE)
		strPath = MakePath(DIR_THEME, "Icon_Ultimate.dds");
	else
		strPath = MakePath(DIR_ICON, "Icon_Ultimate.dds");

	m_pUltimateTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, strPath, 0xffff00ff);
	m_pJewelBgTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "WndChgElemItem.bmp"), 0xffff00ff);
}

void CToolTip::CancelToolTip()
{
	if (FALSE == m_bEnable)
		return;

	m_bReadyToopTip = FALSE;
	m_bToolTip = FALSE;
	m_dwToolTipId = -1;
}

void CToolTip::PutToolTip(DWORD dwToolTipId, CString& string, CRect rect, CPoint pt, int nToolTipPos)
{
	if (-1 == nToolTipPos)
		return;

	PutToolTip(dwToolTipId, (LPCTSTR)string, rect, pt, nToolTipPos);
}

void CToolTip::PutToolTip(DWORD dwToolTipId, LPCTSTR lpszString, CRect rect, CPoint pt, int nToolTipPos)
{
	if (FALSE == m_bEnable ||
		FALSE == rect.PtInRect(pt))
		return;

	if (dwToolTipId != m_dwToolTipId || m_rect != rect)
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		//m_timerToopTip.Set(100);
		m_timerToopTip.Set(0);
		if (m_bToolTip)
			m_bToolTip = FALSE;
		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
	}

	if (NULL != lpszString)
	{
		m_strToolTip = lpszString;
		m_strToolTip.Init(CWndBase::m_Theme.m_pFontText, &CRect(0, 0, 315, 0));
	}

	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString(m_strToolTip);
	m_rectRender
		= CRect(
			0,
			0,
			size.cx + 6,
			size.cy + 3 + (2 * (size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight()))
		);
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
	m_nAdded = 0;
	m_nSlot = 0;

	m_vecMapMonsterID.clear();
#ifdef __FL_PACKED_ITEMS
	m_vecPackage.clear();
#endif
}

void CToolTip::PutToolTip(DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos)
{
	if (FALSE == m_bEnable)
		return;

	if (-1 == nToolTipPos)
		nToolTipPos = 0;
	else if (!rect.PtInRect(pt))
		return;

	if (dwToolTipId != m_dwToolTipId)
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		m_timerToopTip.Set(0);
		if (m_bToolTip)
			m_bToolTip = FALSE;
		m_nSubToolTipNumber = CWndMgr::TOOL_TIP_SWITCH_MAIN;
	}
	m_strToolTip = string;
	m_strToolTip.Init(CWndBase::m_Theme.m_pFontText, &CRect(0, 0, 315, 0));
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString(m_strToolTip);
	m_rectRender
		= CRect(
			0,
			0,
			size.cx + 6,
			size.cy + 3 + (2 * (size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight()))
		);
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
	m_nAdded = 0;
	m_nSlot = 0;

	m_vecMapMonsterID.clear();
#ifdef __FL_PACKED_ITEMS
	m_vecPackage.clear();
#endif
}

#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
void CToolTip::PutToolTipEx(DWORD dwToolTipId, CEditString& string, CRect rect, CPoint pt, int nToolTipPos, int nSubToolTipFlag)
{
	if (m_bEnable == FALSE)
		return;
	if (!rect.PtInRect(pt))
		return;

	if (m_rect == rect)
	{
		if (nSubToolTipFlag == 0)
		{
			if (m_bPutToolTip == FALSE)
			{
				m_nAlpha = 0;
				m_rect = rect;
				m_bReadyToopTip = TRUE;
				m_dwToolTipId = dwToolTipId;
				m_timerToopTip.Set(0);
				if (m_bToolTip)
					m_bToolTip = FALSE;
			}
		}
		else if (nSubToolTipFlag == 1 || nSubToolTipFlag == 2)
		{
			if (g_toolTip.GetReadyToolTipSwitch() == TRUE)
				m_bReadyToopTip = TRUE;
		}
	}
	else
	{
		m_nAlpha = 0;
		m_rect = rect;
		m_bReadyToopTip = TRUE;
		m_dwToolTipId = dwToolTipId;
		m_timerToopTip.Set(0);
		if (m_bToolTip)
			m_bToolTip = FALSE;
	}
	m_strToolTip = string;
	m_strToolTip.Init(CWndBase::m_Theme.m_pFontText, &CRect(0, 0, 315, 0));
	CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent_EditString(m_strToolTip);
	m_rectRender = CRect(0, 0, size.cx + 6, size.cy + 3 + (2 * (size.cy / CWndBase::m_Theme.m_pFontText->GetMaxHeight())));
	m_rect = rect;
	m_nPosition = nToolTipPos;
	m_bPutToolTip = TRUE;
	m_nAdded = 0;
	m_nSlot = 0;
	m_nSubToolTipFlag = nSubToolTipFlag;
	m_vecMapMonsterID.clear();
#ifdef __FL_PACKED_ITEMS
	m_vecPackage.clear();
#endif
}
#endif // __IMPROVE_SYSTEM_VER15


void CToolTip::Process(CPoint pt, C2DRender* p2DRender)
{
	CD3DFont* pFont = p2DRender->m_pFont;

	if (m_bEnable == FALSE)
		return;

	if (m_bPutToolTip == FALSE)
	{
		CancelToolTip();
		return;
	}

	if (m_bPutToolTip == TRUE && m_bReadyToopTip == TRUE && m_timerToopTip.Over())
	{
		if (m_bToolTip == NULL)
			m_ptToolTip = pt;

		m_bToolTip = TRUE;
	}
	if (m_rect.PtInRect(pt) == FALSE)
		m_bPutToolTip = FALSE;

	m_nAlpha += 15;
	if (m_nAlpha > 255)
		m_nAlpha = 255;
}
void CToolTip::Paint(C2DRender* p2DRender)
{
	if (m_bEnable == FALSE)
		return;
	if (m_bToolTip)
	{
		CPoint pt = m_ptToolTip;
		int nPostion = m_nPosition;
		if (nPostion == 0) // top
		{
			pt = m_rect.TopLeft();
			pt.y -= m_rectRender.Height() + 10;
		}
		else if (nPostion == 1) // bottom
		{
			//			pt = CPoint(m_rect.left, m_rect.bottom);
			pt = CPoint(m_rect.left, m_rect.bottom + 20);		// 2006/7/5 -xuzhu-
			//rect.SetRect(pt.x,pt.y,pt.x+m_bToolTip->GetWidth(),pt.y+m_bToolTip->GetHeight());
		}
		else if (nPostion == 2) // left
		{
			pt = m_rect.TopLeft();
			pt.x -= m_rectRender.Width() + 2;
		}
		else if (nPostion == 3) // right
		{
			pt = CPoint(m_rect.right, m_rect.top);
			//pt.x += m_bToolTip->GetHeight() + 2;
		}
		else if (nPostion == 4) // left
		{
			pt = m_rect.TopLeft();
			pt.x -= m_rectRender.Width() + 7;
			pt.y += 25;
		}

		switch (m_nSubToolTipFlag)
		{
		case CWndMgr::TOOL_TIP_SWITCH_SUB1:
		{
			const CRect rectMain = g_toolTip.GetRevisedRect();
			const CRect rectRender = g_toolTip.GetRenderRect();
			pt.x = rectMain.left + rectRender.Width() + 16;
			break;
		}
		case CWndMgr::TOOL_TIP_SWITCH_SUB2:
		{
			const CRect rectSub1 = g_toolTipSub1.GetRevisedRect();
			const CRect rectRender = g_toolTipSub1.GetRenderRect();
			pt.x = rectSub1.left + rectRender.Width() + 16;
			break;
		}
		}
		static const int TOOL_TIP_WIDTH_FOR_SLOTS = 194;
		if (m_nSlot > 0 && m_rectRender.right < TOOL_TIP_WIDTH_FOR_SLOTS)
			m_rectRender.right = TOOL_TIP_WIDTH_FOR_SLOTS;
#ifdef __FL_PACKED_ITEMS
		const int TOOL_TIP_WIDTH_FOR_PACKAGE_SLOTS = m_vecPackage.size() * 32;
		if (m_rectRender.right < TOOL_TIP_WIDTH_FOR_PACKAGE_SLOTS)
			m_rectRender.right = TOOL_TIP_WIDTH_FOR_PACKAGE_SLOTS;
#endif
		CRect rect(pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height());

		switch (m_nSubToolTipFlag)
		{
		case CWndMgr::TOOL_TIP_SWITCH_MAIN:
		{
			static const int TOOL_TIP_MARGIN = 16;
			switch (m_nSubToolTipNumber)
			{
			case CWndMgr::TOOL_TIP_SWITCH_MAIN:
			{
				if (rect.right + TOOL_TIP_MARGIN > p2DRender->m_clipRect.right)
					pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 8;
				break;
			}
			case CWndMgr::TOOL_TIP_SWITCH_SUB1:
			{
				const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
				if (rect.right + rectToolTipSub1.Width() + (TOOL_TIP_MARGIN * 2) > p2DRender->m_clipRect.right)
					pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 224;
				break;
			}
			case CWndMgr::TOOL_TIP_SWITCH_SUB2:
			{
				const CRect rectToolTipSub1 = g_toolTipSub1.GetRevisedRect();
				const CRect rectToolTipSub2 = g_toolTipSub2.GetRevisedRect();
				if (rect.right + rectToolTipSub1.Width() + rectToolTipSub2.Width() + (TOOL_TIP_MARGIN * 3) > p2DRender->m_clipRect.right)
					pt.x = p2DRender->m_clipRect.Width() - rect.Width() - 470;
				break;
			}
			}
			if (rect.top < p2DRender->m_clipRect.top)
				pt.y = p2DRender->m_clipRect.top + 8;
			m_nRevisedRect = CRect(pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height());
			break;
		}
		case CWndMgr::TOOL_TIP_SWITCH_SUB1:
		case CWndMgr::TOOL_TIP_SWITCH_SUB2:
		{
			if (rect.top < p2DRender->m_clipRect.top)
				pt.y = p2DRender->m_clipRect.top + 8;
			m_nRevisedRect = CRect(pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height());
			break;
		}
		}

		if (m_nSlot > 0)
		{
			rect.SetRect(pt.x, pt.y - (36), pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height());
			if (rect.top < p2DRender->m_clipRect.top)
			{
				pt.y = p2DRender->m_clipRect.top + 8;
				rect.SetRect(pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height() + (36));
			}
		}
		else
			rect.SetRect(pt.x, pt.y, pt.x + m_rectRender.Width(), pt.y + m_rectRender.Height());

		int nPlusLow = 8;
		int nPlusColumn = 8;
		int nSetting = g_Option.m_nToolTipTexture * 9;

		float fNa = float(rect.bottom - rect.top + nPlusLow * 2) / 16.0f;
		int nlowDraw = (int)fNa;
		if (nlowDraw == 0 && 0 < fNa)
			nlowDraw = 1;

		fNa = float(rect.right - rect.left + nPlusColumn * 2) / 16.0f;
		int nColumnDraw = (int)fNa;

		if (nColumnDraw == 0 && 0 < fNa)
			nColumnDraw = 1;

		CRect PlusRect = rect;
		PlusRect.left = rect.left - nPlusLow;
		PlusRect.top = rect.top - nPlusColumn;
		PlusRect.right = rect.right - 16 + nPlusLow;
		PlusRect.bottom = rect.bottom - 16 + nPlusColumn;

		for (int i = 0; i < nlowDraw; ++i)
		{
			for (int j = 0; j < nColumnDraw; ++j)
			{

				if (i == 0 || j == 0)
				{
					if (j == 0 && i != 0)
					{
						if (i + 1 == nlowDraw)
						{
							m_apTextureToolTip[nSetting + 3].Render(p2DRender, CPoint(PlusRect.left, PlusRect.top + (i * 16)), CPoint(16, PlusRect.bottom - (PlusRect.top + (i * 16))));
							m_apTextureToolTip[nSetting + 5].Render(p2DRender, CPoint(PlusRect.right, PlusRect.top + (i * 16)), CPoint(16, PlusRect.bottom - (PlusRect.top + (i * 16))));
						}
						else
						{
							m_apTextureToolTip[nSetting + 3].Render(p2DRender, CPoint(PlusRect.left, PlusRect.top + (i * 16)));
							m_apTextureToolTip[nSetting + 5].Render(p2DRender, CPoint(PlusRect.right, PlusRect.top + (i * 16)));
						}

					}
					else if (i == 0 && j != 0)
					{
						if (j + 1 == nColumnDraw)
						{
							m_apTextureToolTip[nSetting + 1].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top), CPoint(PlusRect.right - (PlusRect.left + (j * 16)), 16));
							m_apTextureToolTip[nSetting + 7].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.bottom), CPoint(PlusRect.right - (PlusRect.left + (j * 16)), 16));
						}
						else
						{
							m_apTextureToolTip[nSetting + 1].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top));
							m_apTextureToolTip[nSetting + 7].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.bottom));
						}
					}
					continue;
				}


				if (i == nlowDraw - 1 || j == nColumnDraw - 1)
				{

					if (i == nlowDraw - 1 && j == nColumnDraw - 1)
					{
						m_apTextureToolTip[nSetting + 4].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top + (i * 16)),
							CPoint(PlusRect.right - (PlusRect.left + (j * 16)), PlusRect.bottom - (PlusRect.top + (i * 16))));

					}
					else if (i == nlowDraw - 1)
					{
						m_apTextureToolTip[nSetting + 4].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top + (i * 16)),
							CPoint(16, PlusRect.bottom - (PlusRect.top + (i * 16))));

					}
					else
					{
						m_apTextureToolTip[nSetting + 4].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top + (i * 16)),
							CPoint(PlusRect.right - (PlusRect.left + (j * 16)), 16));
					}
				}
				else
				{
					m_apTextureToolTip[nSetting + 4].Render(p2DRender, CPoint(PlusRect.left + (j * 16), PlusRect.top + (i * 16)));
				}
			}
		}

		m_apTextureToolTip[nSetting + 0].Render(p2DRender, CPoint(PlusRect.left, PlusRect.top));
		m_apTextureToolTip[nSetting + 2].Render(p2DRender, CPoint(PlusRect.right, PlusRect.top));
		m_apTextureToolTip[nSetting + 6].Render(p2DRender, CPoint(PlusRect.left, PlusRect.bottom));
		m_apTextureToolTip[nSetting + 8].Render(p2DRender, CPoint(PlusRect.right, PlusRect.bottom));

		p2DRender->TextOut_EditString(rect.TopLeft().x + 3, rect.TopLeft().y + 3, m_strToolTip, 0, 0, 2);//, D3DCOLOR_ARGB( m_nAlpha * 255 / 255, 0, 0, 0 ) );

		if (static_cast<int>(m_vecMapMonsterID.size()) > 0)
		{
			int nMonElementYPos = 0;
			int nStringLine = 0;
			for (vector< DWORD >::iterator Iterator = m_vecMapMonsterID.begin(); Iterator != m_vecMapMonsterID.end(); ++Iterator)
			{
				CString strTemp = m_strToolTip.GetLine(nStringLine);
				CString strLevel = _T("");
				CString strEnd = strTemp.GetAt(strTemp.GetLength() - 1);
				if (strTemp.Find(prj.GetText(TID_GAME_MONSTER_INFORMATION_LEVEL)) == 0)
				{
					int nLine = 1;
					if (strEnd != "\n")
					{
						nLine = 2;
						++nStringLine;
						strTemp = m_strToolTip.GetLine(nStringLine);
					}

					strTemp.TrimRight();
					CSize size = CWndBase::m_Theme.m_pFontText->GetTextExtent(strTemp);
					MoverProp* pMoverProp = prj.GetMoverProp(*Iterator);
					if (pMoverProp)
					{
						if (g_WndMng.m_pWndWorld && pMoverProp->eElementType)
						{
							if (Iterator == m_vecMapMonsterID.begin())
								nMonElementYPos = PlusRect.top + 8 + (size.cy + 2) * (nLine - 1);
							else
								nMonElementYPos += (size.cy + 2) * nLine;

							g_WndMng.m_pWndWorld->m_texAttrIcon.Render(p2DRender, CPoint(PlusRect.left + size.cx + 20, nMonElementYPos), pMoverProp->eElementType - 1, 255, 1.0f, 1.0f);
						}
					}

					++nStringLine;
				}
			}
		}

		if (m_nAdded == 1)
		{
			CTexture* pTexture;

			if (m_pUltimateTexture != NULL)
			{
				if (m_nSubToolTipFlag == CWndMgr::TOOL_TIP_SWITCH_MAIN)
					m_pUltimateTexture->Render(p2DRender, CPoint(PlusRect.left + 10, PlusRect.top + 8));
				else
					m_pUltimateTexture->Render(p2DRender, CPoint(PlusRect.left + 10, PlusRect.top + 24));
			}

			//Jewel Icon Added
			CPoint point;
			point.x = PlusRect.left + 14 + ((PlusRect.Width() - 194) / 2);
			point.y = PlusRect.bottom - 24;

			for (int i = 0; i < m_nSlot; i++)
			{
				if (m_pJewelBgTexture != NULL)
					m_pJewelBgTexture->RenderScal(p2DRender, point, 255, 1.0f, 1.0f);

				//Jewel Render
				if (m_nAddedJewel[i] != 0)
				{
					ItemProp* pItemProp;
					pItemProp = prj.GetItemProp(m_nAddedJewel[i]);
					if (pItemProp != NULL)
					{
						pTexture = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ITEM, pItemProp->szIcon), 0xffff00ff);
						if (pTexture != NULL)
							pTexture->RenderScal(p2DRender, point, 255, 1.0f, 1.0f);
					}
				}

				point.x += 38;
			}
		}
#ifdef __FL_PACKED_ITEMS
		else if (m_nAdded == 4)
		{
			CPoint point;
			point.x = PlusRect.left + 10;
			point.y = PlusRect.bottom - 24;

			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);

			for (auto& it : m_vecPackage)
			{
				if (m_pJewelBgTexture)
					m_pJewelBgTexture->RenderScal(p2DRender, point, 255, 0.8f, 0.8f);

				if (it.pTexture)
					it.pTexture->RenderScal(p2DRender, point, 255, 0.8f, 0.8f);

				if (it.nItemCount > 1)
				{
					TCHAR szTemp[32];
					_stprintf(szTemp, "%d", it.nItemCount);
					CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
					p2DRender->TextOut(point.x + 24 - size.cx, point.y + 24 - size.cy + 9, szTemp, 0xffeaebdd);
				}
				else if (it.nAbilityOption > 0)
				{
					TCHAR szTemp[32];
					_stprintf(szTemp, "%+d", it.nAbilityOption);
					CSize size = p2DRender->m_pFont->GetTextExtent(szTemp);
					p2DRender->TextOut(point.x + 24 - size.cx, point.y - 6, szTemp, 0xffeaebdd);
				}

				point.x += 31;
			}

			p2DRender->SetFont(pOldFont);
		}
#endif
		else if (m_nAdded == 3)
		{
			CTexture* pTex;
			BYTE bWeak, bStrong;
			switch (bElementt)
			{
			case 0: //FIRE, WATER, ELECTRICITY, WIND, EARTH
			{
				bWeak = 3;
				bStrong = 1;
				break;
			}
			case 1:
			{
				bWeak = 0;
				bStrong = 2;
				break;
			}
			case 2:
			{
				bWeak = 1;
				bStrong = 4;
				break;
			}
			case 3:
			{
				bWeak = 4;
				bStrong = 0;
				break;
			}
			case 4:
			{
				bWeak = 2;
				bStrong = 3;
				break;
			}

			}
			pTex = CWndBase::m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_THEME, "ExchangeArrow.tga"), 0xffff00ff);
			if (pTex)
			{
				p2DRender->RenderTexture(CPoint(PlusRect.left + 30, PlusRect.top + 8), pTex, 255, 0.7f, 0.7f);
				p2DRender->RenderTexture(CPoint(PlusRect.left + 75, PlusRect.top + 8), pTex, 255, 0.7f, 0.7f);
			}

			g_WndMng.m_pWndWorld->m_texAttrIcon.Render(p2DRender, CPoint(PlusRect.left + 10, PlusRect.top + 9), bStrong, 255, 1.2f, 1.2f);
			g_WndMng.m_pWndWorld->m_texAttrIcon.Render(p2DRender, CPoint(PlusRect.left + 55, PlusRect.top + 9), bElementt, 255, 1.2f, 1.2f);
			g_WndMng.m_pWndWorld->m_texAttrIcon.Render(p2DRender, CPoint(PlusRect.left + 100, PlusRect.top + 9), bWeak, 255, 1.2f, 1.2f);
		}
	}
}

void CToolTip::SetUltimateToolTip(CItemElem* pItemBase)
{
	m_nAdded = 1;
	m_nSlot = pItemBase->GetUltimatePiercingSize();
	for (int i = 0; i < m_nSlot; i++)
		m_nAddedJewel[i] = pItemBase->GetUltimatePiercingItem(i);
}

#ifdef __FL_PACKED_ITEMS
void CToolTip::SetPackageToolTip(CItemElem* pItemBase)
{
	m_nAdded = 4;

	m_vecPackage.clear();

	PACKAGE_ITEMS tmp;
	for (auto& it : *pItemBase->GetItemList())
	{
		tmp.pTexture = it->m_pTexture;
		tmp.nItemCount = it->m_nItemNum;
		tmp.nAbilityOption = it->GetAbilityOption();
		m_vecPackage.push_back(tmp);
	}

	m_nSlot = (int)m_vecPackage.size();
}
#endif

//-----------------------------------------------------------------------------
void CToolTip::ResizeMapMonsterToolTip(void)
{
	m_rectRender.right += 18;
}
//-----------------------------------------------------------------------------
void CToolTip::InsertMonsterID(DWORD dwMonsterID)
{
	m_vecMapMonsterID.push_back(dwMonsterID);
}
const CPoint& CToolTip::GetPointToolTip(void) const
{
	return m_ptToolTip;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRect(void) const
{
	return m_rect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetRenderRect(const CRect& rectRender)
{
	m_rectRender = rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRenderRect(void) const
{
	return m_rectRender;
}
//-----------------------------------------------------------------------------
const CRect& CToolTip::GetRevisedRect(void) const
{
	return m_nRevisedRect;
}
//-----------------------------------------------------------------------------
void CToolTip::SetSubToolTipNumber(int nSubToolTipNumber)
{
	m_nSubToolTipNumber = nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
int CToolTip::GetSubToolTipNumber(void) const
{
	return m_nSubToolTipNumber;
}
//-----------------------------------------------------------------------------
BOOL CToolTip::GetReadyToolTipSwitch(void) const
{
	return m_bReadyToopTip;
}

void CToolTip::SetElementTooltip(DWORD dwToolTipId, BYTE bElement)
{
	bElementt = bElement;
	m_nAdded = 3;
}