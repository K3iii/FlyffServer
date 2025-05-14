// WndBase.cpp: implementation of the CWndBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSound.h"
#include "defineText.h"
#include "AppDefine.h"
#include "clipboard.h"

#include "Exchange.h"

#include "playerdata.h"

#include "collecting.h"

#if __VER >= 15 // __GUILD_HOUSE
#include "GuildHouse.h"
#include "WndGuildHouse.h"
#endif

#ifdef __WIKI_ITEMS_V2
#include "WndWikiItems.h"
#endif

#ifdef __FL_TELEPORT
#include "Teleporter.h"
#endif

#define EDIT_HEIGHT 8
#define VSCROLL_WIDTH 16

#define GETRANGE ( ( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight() )

CWndStatic::CWndStatic()
{
	m_byWndType = WTYPE_STATIC;
	m_dwColor = 0xffffffff;
	m_strTexture = DEF_CTRL_GROUP;
	m_bTile = TRUE;

}
void CWndStatic::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	//rectWnd.DeflateRect( 2, 2 );
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
BOOL CWndStatic::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	if (lpszCaption && strlen(lpszCaption)) m_strTitle = lpszCaption;
	if (!(dwStyle & WSS_GROUPBOX))
	{
		m_strTexture = "";//WndEditTile200.tga";
		m_dwColor = 0xff2e70a9;
	}
	return CWndBase::Create(dwStyle | WBS_CHILD | WBS_NOFRAME, rect, pParentWnd, nID);
}
void CWndStatic::OnDraw(C2DRender* p2DRender)
{
	CPoint point(4, 4);

	if (IsWndStyle(WSS_GROUPBOX))
	{
		CD3DFont* pOldFont = p2DRender->GetFont();
		p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);

		int nFontMaxHeight = CWndBase::m_Theme.m_pFontText->GetMaxHeight();

		if (::GetLanguage() == LANG_FRE || ::GetLanguage() == LANG_GER)
			point.y -= 2;
		else
		{
			if (nFontMaxHeight > 14)
				point.y -= nFontMaxHeight - 14;
		}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if (IsWndStyle(WSS_ALIGNHRIGHT))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.x = (rect.Width() - sizeTitle.cx - 4);
			if (point.x < 2)
				point.x = 2;
		}
		if (IsWndStyle(WSS_ALIGNHCENTER))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.x = (rect.Width() - sizeTitle.cx) / 2;
			if (point.x < 2)
				point.x = 2;
		}
		if (IsWndStyle(WSS_ALIGNVBOTTOM))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.y = rect.Height() - sizeTitle.cy;
			if (point.y < 2)
				point.y = 2;
		}
		if (IsWndStyle(WSS_ALIGNVCENTER))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.y = (rect.Height() - sizeTitle.cy + 4) / 2;
			if (point.y < 2)
				point.y = 2;
		}
#endif // __IMPROVE_QUEST_INTERFACE

		p2DRender->TextOut(0 + point.x, point.y, m_strTitle, m_dwColor);//m_dwColor );
		p2DRender->TextOut(1 + point.x, point.y, m_strTitle, m_dwColor);
		p2DRender->SetFont(pOldFont);

	}
	else
	{
		CPoint point(2, 2);

#if __VER >= 14 // __STATIC_ALIGN
		if (IsWndStyle(WSS_ALIGNHRIGHT))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.x = (rect.Width() - sizeTitle.cx - 4);
			if (point.x < 2)
				point.x = 2;
		}
		if (IsWndStyle(WSS_ALIGNHCENTER))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.x = (rect.Width() - sizeTitle.cx) / 2;
			if (point.x < 2)
				point.x = 2;
		}
		if (IsWndStyle(WSS_ALIGNVBOTTOM))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.y = rect.Height() - sizeTitle.cy;
			if (point.y < 2)
				point.y = 2;
		}
		if (IsWndStyle(WSS_ALIGNVCENTER))
		{
			CSize sizeTitle = p2DRender->m_pFont->GetTextExtent(m_strTitle);
			CRect rect = GetWindowRect();
			point.y = (rect.Height() - sizeTitle.cy + 4) / 2;
			if (point.y < 2)
				point.y = 2;
		}
#endif // __STATIC_ALIGN

		if (IsWndStyle(WSS_MONEY))
		{
			CString str;
			str = GetNumberFormatEx(m_strTitle);
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);
			p2DRender->TextOut(0 + point.x, point.y, str, m_dwColor);
			p2DRender->TextOut(1 + point.x, point.y, str, m_dwColor);
			p2DRender->SetFont(pOldFont);
		}
		else
		{
			CD3DFont* pOldFont = p2DRender->GetFont();
			p2DRender->SetFont(CWndBase::m_Theme.m_pFontText);
			//p2DRender->TextOut( 6, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 6, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 0, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 7, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 8, 2, m_strTitle, D3DCOLOR_ARGB( 255, 217, 91, 51) );
			//p2DRender->TextOut( 1, 0, m_strTitle, 0xff10106f );//m_dwColor );
			//p2DRender->TextOut( 0, 0, m_strTitle, 0xff7070cf );//m_dwColor );

			p2DRender->TextOut(0 + point.x, point.y, m_strTitle, m_dwColor);//m_dwColor );
			p2DRender->TextOut(1 + point.x, point.y, m_strTitle, m_dwColor);//m_dwColor );

			p2DRender->SetFont(pOldFont);
		}
	}
}
BOOL CWndStatic::IsPickupSpace(CPoint point)
{
	if (IsWndStyle(WSS_GROUPBOX) || IsWndStyle(WSS_PICTURE))
	{
		return FALSE;
	}
	CRect rect = GetWindowRect();
	if (rect.PtInRect(point))
	{
		if (m_bLButtonDown)
		{
			int a = 0;
		}
		return TRUE;
	}
	return FALSE;
}
void CWndStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if (m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef __CLIENT
	if (m_pParentWnd)
		m_pParentWnd->SetFocus();
#endif
}
void CWndStatic::PaintFrame(C2DRender* p2DRender)
{
	if (m_strTexture.IsEmpty() == FALSE || IsWndStyle(WSS_PICTURE) == FALSE)
		return;
	CRect rect = GetWindowRect();
	p2DRender->RenderRect(rect, 0xff000000);
}


void CWndCustom::OnDraw(C2DRender* p2DRender)
{
	p2DRender->RenderFillRect(GetWindowRect(), 0xff808080);
	//return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndButton
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
WBS_TEXT
WBS_SPRITE
WBS_RADIO
WBS_CHECK

ex)
	WBS_TEXT
	WBS_TEXT | WBS_RADIO
	WBS_TEXT | WBS_CHECK
	WBS_SPRITE
	WBS_SPRITE | WBS_RADIO
	WBS_SPRITE | WBS_CHECK
*/

CWndButton::CWndButton()
{
	m_nFontColor = D3DCOLOR_ARGB(255, 64, 64, 64);
	m_nPushColor = D3DCOLOR_ARGB(255, 255, 255, 100);
	m_nDisableColor = D3DCOLOR_ARGB(255, 140, 140, 140);
	m_nHighlightColor = D3DCOLOR_ARGB(255, 64, 64, 255);
	//m_strSndEffect    = "sound\\sndsyspickupitems.wav";
	m_bCheck = FALSE;
	m_nTimePush = 0;
	m_bHighLight = FALSE;
	m_bGroup = FALSE;
	m_ptPush = CPoint(1, 1);
	m_pWndMenu = NULL;
	m_byWndType = WTYPE_BUTTON;
	m_cHotkey = 0;
	ZeroMemory(&m_shortcut, sizeof(m_shortcut));
}
CWndButton::~CWndButton()
{
	//SAFE_DELETE( m_pWndMenu );
}
void CWndButton::FitTextureSize()
{
	if (m_pTexture)
	{
		CRect rect = GetWindowRect(TRUE);
		if (IsWndStyle(WBS_RADIO) | IsWndStyle(WBS_CHECK))
			rect.right = rect.left + (m_pTexture->m_size.cx / 6);
		else
			rect.right = rect.left + (m_pTexture->m_size.cx / 4);
		SetWndRect(rect);
	}
}
void CWndButton::SetMenu(CWndMenu* pWndMenu)
{
	m_pWndMenu = pWndMenu;
}


BOOL CWndButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	//m_dwStyle = dwButtonStyle;// | WBS_NODRAW;
	//if(pSprPack)
//		dwStyle |= WBS_SPRITE;
	dwStyle |= WBS_HIGHLIGHT;
	if (lpszCaption && strlen(lpszCaption))
	{
		//dwButtonStyle |= WBS_TEXT;
		m_strTitle = lpszCaption;
	}
	BOOL b = CWndBase::Create(dwStyle | WBS_CHILD |/*WBS_NODRAWFRAME|*/WBS_NOFRAME, rect, pParentWnd, nID);//,pSprPack,nSprIdx,nColorTable);;
	//m_strTexture = "ButtRadio.bmp";
	if (IsWndStyle(WBS_RADIO))
		SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, DEF_CTRL_RADIO), 1);
	if (IsWndStyle(WBS_CHECK))
		SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, DEF_CTRL_CHECK), 1);

#ifdef __CLIENT
	//AddWndStyle( WBS_NODRAWFRAME );
	//DelWndStyle( WBS_NODRAWFRAME );
#else
	//DelWndStyle( WBS_NODRAWFRAME );
#endif
	return b;
}
void CWndButton::OnDraw(C2DRender* p2DRender)
{
}
void CWndButton::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();

	GET_CLIENT_POINT(m_pApp->GetSafeHwnd(), point);
	CString string;
#ifdef __CLIENT

#endif
	DWORD dwColor = D3DCOLOR_TEMP(200, 240, 240, 240);
	m_bHighLight = FALSE;
	int nFontHeight = GetFontHeight();
	int nFontColor = p2DRender->GetTextColor();

	//m_strTitle = "Button";


	if ((m_dwStyle & WBS_HIGHLIGHT) && m_GlobalShortcut.m_dwShortcut == SHORTCUT_NONE)
	{
		if (GetClientRect().PtInRect(m_ptMouse) && m_bEnable == TRUE)
		{
			m_bHighLight = TRUE;
		}
	}
	if (m_dwStyle & WBS_RADIO)
	{
		m_pTheme->RenderWndButtonRadio(p2DRender, this);
	}
	else
		if (m_dwStyle & WBS_CHECK)
		{
			m_pTheme->RenderWndButtonCheck(p2DRender, this);
		}
		else
			if (m_dwStyle & WBS_TEXT)
			{
				m_pTheme->RenderWndButtonText(p2DRender, this);
			}
			else
				if (m_dwStyle & WBS_MENUITEM)
				{
					CPoint pt(0);

					DWORD dwColor1 = (m_bHighLight == TRUE ? D3DCOLOR_ARGB(155, 200, 200, 200) : D3DCOLOR_ARGB(0, 0, 0, 0));
					DWORD dwColor2 = (m_bHighLight == TRUE ? D3DCOLOR_ARGB(155, 200, 200, 200) : D3DCOLOR_ARGB(0, 0, 0, 0));
					DWORD dwColor3 = (m_bHighLight == TRUE ? D3DCOLOR_ARGB(155, 200, 200, 200) : D3DCOLOR_ARGB(0, 0, 0, 0));
					CRect rect = GetClientRect();

					if (m_pWndMenu)
					{
						if (m_pWndMenu->IsVisible())
						{
							dwColor1 = D3DCOLOR_TEMP(155, 200, 200, 200);
							p2DRender->RenderFillRect(rect, dwColor1, dwColor1, dwColor1, dwColor1);
							dwColor = m_nPushColor;
						}
						p2DRender->RenderFillTriangle(
							CPoint(rect.right - 10, rect.top + 4),
							CPoint(rect.right - 10, rect.bottom - 4),
							CPoint(rect.right - 5, rect.top + rect.Height() / 2),
							0xffa0a0ff //dwColor
						);

						if (m_pTexture)
						{
							CSize size = m_pTexture->m_size;
							m_pTexture->m_size.cx = 20;
							m_pTexture->m_size.cy = 20;
							m_pTexture->Render(p2DRender, CPoint(pt.x, pt.y), m_nAlphaCount);
							m_pTexture->m_size = size;
						}
						p2DRender->TextOut(pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xff000000);
					}
					else
					{
						if (m_bHighLight)
						{
							p2DRender->RenderFillRect(rect, dwColor2, dwColor2, dwColor2, dwColor2);
							p2DRender->RenderRoundRect(rect, dwColor);
						}

						if (m_pTexture)
						{
							CSize size = m_pTexture->m_size;
							m_pTexture->m_size.cx = 20;
							m_pTexture->m_size.cy = 20;
							m_pTexture->Render(p2DRender, CPoint(pt.x, pt.y), m_nAlphaCount);
							m_pTexture->m_size = size;
						}

						if (GetCheck())
						{
							p2DRender->RenderLine(CPoint(2, 2), CPoint(5, 8), 0xff000000);
							p2DRender->RenderLine(CPoint(2, 2), CPoint(6, 8), 0xff000000);
							p2DRender->RenderLine(CPoint(5, 8), CPoint(8, 3), 0xff000000);
						}
						p2DRender->TextOut(pt.x + 4 + 20, pt.y + 4, m_strTitle, 0xff000000);
						if (m_cHotkey)
						{
							CString strHotkey = m_cHotkey;
							p2DRender->TextOut(m_rectClient.right - 32, pt.y + 4, strHotkey, 0xff000000);
						}
					}
				}
				else
				{
					m_pTheme->RenderWndButton(p2DRender, this);
				}
}
BOOL CWndButton::Process()
{
	return CWndBase::Process();
}
void CWndButton::SetPushTime(int nTime)
{
	m_nTimePush = nTime;
}
UINT CWndButton::GetState() const // Retrieves the check state, highlight state, and focus state of a button control.
{
	return 1;
}
void CWndButton::SetState(BOOL bHighlight) // Sets the highlighting state of a button control
{
}
int CWndButton::GetCheck() const // Retrieves the check state of a button control.
{
	return m_bCheck;
}
void CWndButton::SetCheck(int nCheck) // Sets the check state of a button control.
{
	m_bCheck = nCheck;
}
void CWndButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsPush())
	{
		if (m_dwStyle & WBS_CHECK)
			m_bCheck = !m_bCheck;

		CWndBase* pWnd = m_pParentWnd;
		//if( ! (pWnd->GetStyle() & WBS_CHILD ) )
		pWnd->OnChildNotify(WNM_CLICKED, m_nIdWnd, (LRESULT*)this);

		if ((m_dwStyle & WBS_RADIO) && m_pParentWnd)
		{
			CPtrArray* pWndArray = m_pParentWnd->GetWndArray();

			for (int i = 0; i < pWndArray->GetSize(); i++)
			{
				CWndButton* pWndBase = (CWndButton*)pWndArray->GetAt(i);
				if (pWndBase == this)
				{

					for (int i2 = i; i2 >= 0; i2--)
					{
						pWndBase = (CWndButton*)pWndArray->GetAt(i2);
						if (pWndBase->IsGroup() && pWndBase->IsWndStyle(WBS_RADIO))
						{

							for (int i3 = i2; i3 < pWndArray->GetSize(); i3++)
							{
								pWndBase = (CWndButton*)pWndArray->GetAt(i3);
								if (i3 > i2 && pWndBase->IsGroup())
									break;
								if (pWndBase->IsWndStyle(WBS_RADIO))
									((CWndButton*)pWndBase)->SetCheck(0);
								else
									break;
							}
							SetCheck(TRUE);
							return;
						}
					}
				}
			}
		}
	}
}
void CWndButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	PLAYSND(SND_INF_CLICK);
}
void CWndButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((m_dwStyle & WBS_MENUITEM) && m_pWndMenu == NULL && IsPush())
	{
		m_GlobalShortcut.m_dwShortcut = m_shortcut.m_dwShortcut;//SHORTCUT_APPLET;
		m_GlobalShortcut.m_dwId = m_nIdWnd;
		m_GlobalShortcut.m_pTexture = m_pTexture;
		_tcscpy(m_GlobalShortcut.m_szString, GetTitle());
	}
}

void CWndButton::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndButton::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}
void CWndButton::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
BOOL CWndButton::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
}
LRESULT CWndButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWndBase::DefWindowProc(message, wParam, lParam);
}

CWndTreeCtrl::CWndTreeCtrl()
{
	//m_nWndColor    = D3DCOLOR_TEMP( 255,  0x5 << 3,  0x5 << 3,  0x5 << 3 );
	m_byWndType = WTYPE_TREECTRL;

	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_nLineSpace = 3;
	m_nFontColor = D3DCOLOR_ARGB(255, 64, 64, 64);
	m_nSelectColor = D3DCOLOR_ARGB(255, 0, 0, 255);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_pFocusElem = NULL;
	m_nFontHeight = 0;
	m_pTexButtOpen = NULL;
	m_pTexButtClose = NULL;
	m_bMemberCheckingMode = FALSE;
	m_nMaxCheckNumber = 0;
	m_nTreeTabWidth = 16;
	m_nCategoryTextSpace = 16;
	m_nTreeItemsMaxWidth = 0;
#endif // __IMPROVE_QUEST_INTERFACE
}

CWndTreeCtrl::~CWndTreeCtrl()
{
	DeleteAllItems();
}
BOOL CWndTreeCtrl::Create(DWORD dwTextStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	return CWndBase::Create(WBS_CHILD | dwTextStyle, rect, pParentWnd, nID);
}
void CWndTreeCtrl::DeleteItemArray(void)
{
	for (int i = 0; i < m_treeItemArray.GetSize(); i++)
		SAFE_DELETE_CONST((LPTREEITEM)m_treeItemArray.GetAt(i));
	m_treeItemArray.RemoveAll();
}
BOOL CWndTreeCtrl::DeleteAllItems()
{
	FreeTree(m_treeElem.m_ptrArray);
	DeleteItemArray();
	m_pFocusElem = NULL;
	return TRUE;
}
LPTREEELEM CWndTreeCtrl::GetCurSel()
{
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::GetRootElem()
{
	return &m_treeElem;
}
LPTREEELEM CWndTreeCtrl::GetNextElem(LPTREEELEM pElem, int& nPos)
{
	LPTREEELEM pGetElem = NULL;
	if (nPos < pElem->m_ptrArray.GetSize())
	{
		pGetElem = (LPTREEELEM)pElem->m_ptrArray.GetAt(nPos);
		nPos++;
	}
	return pGetElem;
}
LPTREEELEM CWndTreeCtrl::FindTreeElem(DWORD dwData)
{
	return FindTreeElem(m_treeElem.m_ptrArray, dwData);
}
LPTREEELEM CWndTreeCtrl::FindTreeElem(CPtrArray& ptrArray, DWORD dwData)
{
	for (int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if (lpTreeElem->m_dwData == dwData)
			return lpTreeElem;
		if (lpTreeElem->m_ptrArray.GetSize())
		{
			lpTreeElem = FindTreeElem(lpTreeElem->m_ptrArray, dwData);
			if (lpTreeElem) return lpTreeElem;
		}
	}
	return NULL;
}
LPTREEELEM CWndTreeCtrl::SetCurSel(DWORD dwData)
{
	m_pFocusElem = FindTreeElem(m_treeElem.m_ptrArray, dwData);
	return m_pFocusElem;
}
LPTREEELEM CWndTreeCtrl::SetCurSel(LPCTSTR lpszKeyword)
{
	m_pFocusElem = FindTreeElem(m_treeElem.m_ptrArray, lpszKeyword);
	return m_pFocusElem;
}

void CWndTreeCtrl::FreeTree(CPtrArray& ptrArray)
{
	for (int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if (m_bMemberCheckingMode == TRUE)
			SAFE_DELETE(lpTreeElem->m_pWndCheckBox);
#endif // __IMPROVE_QUEST_INTERFACE
		if (lpTreeElem->m_ptrArray.GetSize())
			FreeTree(lpTreeElem->m_ptrArray);
		SAFE_DELETE(lpTreeElem);
	}
	ptrArray.RemoveAll();
}
LPTREEELEM CWndTreeCtrl::FindTreeElem(LPCTSTR lpszKeyword)
{
	return FindTreeElem(m_treeElem.m_ptrArray, lpszKeyword);
}
LPTREEELEM CWndTreeCtrl::FindTreeElem(CPtrArray& ptrArray, LPCTSTR lpszKeyword)
{
	for (int i = 0; i < ptrArray.GetSize(); i++)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		if (lpTreeElem->m_strKeyword == lpszKeyword)
			return lpTreeElem;
		if (lpTreeElem->m_ptrArray.GetSize())
		{
			lpTreeElem = FindTreeElem(lpTreeElem->m_ptrArray, lpszKeyword);
			if (lpTreeElem) return lpTreeElem;
		}
	}
	return NULL;
}

LPTREEELEM CWndTreeCtrl::InsertItem(LPTREEELEM lpParent, LPCTSTR lpString, DWORD dwData, BOOL bForbidChecking, BOOL bCheck, DWORD dwFontColor, DWORD dwSelectColor)
{
	LPTREEELEM lpTreeElem = new TREEELEM(&GetClientRect());
	lpTreeElem->m_lpParent = lpParent;
	lpTreeElem->m_dwColor = dwFontColor;
	lpTreeElem->m_dwSelectColor = dwSelectColor;
	lpTreeElem->m_strKeyword.Init(m_pFont, &GetClientRect());
	lpTreeElem->m_strKeyword.SetParsingString(lpString, dwFontColor, 0x00000000, 0, 0x00000001, TRUE);
	lpTreeElem->m_dwData = dwData;
	lpTreeElem->m_bOpen = FALSE;

	if (m_bMemberCheckingMode == TRUE && bForbidChecking == FALSE)
	{
		lpTreeElem->m_pWndCheckBox = new CWndButton;
		CWndButton* pWndCheckBox = lpTreeElem->m_pWndCheckBox;
		CRect rectCheckBox(0, 0, CHECK_BOX_SIZE_XY, CHECK_BOX_SIZE_XY);
		pWndCheckBox->Create("", WBS_CHECK, rectCheckBox, this, WIDC_CHECK);
		pWndCheckBox->SetTexture(D3DDEVICE, MakePath(DIR_THEME, _T(DEF_CTRL_CHECK)), 1);
		pWndCheckBox->FitTextureSize();
		pWndCheckBox->SetCheck(bCheck);
		pWndCheckBox->EnableWindow(FALSE);
		pWndCheckBox->SetVisible(FALSE);
	}
	if (lpParent)
		lpParent->m_ptrArray.Add(lpTreeElem);
	else
		m_treeElem.m_ptrArray.Add(lpTreeElem);
	return lpTreeElem;
}
void CWndTreeCtrl::LoadTreeScript(LPCTSTR lpFileName, bool bAllOpen)
{
	CScript script;
	if (script.Load(lpFileName) == FALSE)
		return;
	do
	{
		InterpriteScript(script, m_treeElem.m_ptrArray, bAllOpen);
		script.GetToken();
	} while (script.tok != FINISHED);

}
void CWndTreeCtrl::InterpriteScript(CScript& script, CPtrArray& ptrArray, bool bAllOpen)
{
	LPTREEELEM lpTreeElem = NULL;
	script.GetToken();	// keyword
	while (*script.token != '}' && script.tok != FINISHED)
	{
		lpTreeElem = new TREEELEM;
		lpTreeElem->m_strKeyword = script.Token;
		lpTreeElem->m_bOpen = bAllOpen ? TRUE : FALSE;
#ifdef __WIKI_ITEMS_V2
		if (lpTreeElem->m_strKeyword == "All Items")
			lpTreeElem->m_bOpen = TRUE;
#endif
		lpTreeElem->m_dwColor = m_nFontColor;
		ptrArray.Add(lpTreeElem);
		lpTreeElem->m_dwData = 0;
		script.GetToken();	// keyword
		if (*script.token == '{')
		{
			if (lpTreeElem)
				InterpriteScript(script, lpTreeElem->m_ptrArray, bAllOpen);
		}
	}
	if (script.tok == FINISHED)
		return;
	script.GetToken();
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
BOOL CWndTreeCtrl::CheckParentTreeBeOpened(LPTREEELEM lpTreeElem)
{
	if (lpTreeElem->m_bOpen == FALSE)
		return lpTreeElem->m_bOpen;
	else
	{
		if (lpTreeElem->m_lpParent)
			return CheckParentTreeBeOpened(lpTreeElem->m_lpParent);
		else
			return lpTreeElem->m_bOpen;
	}
}
CPtrArray* CWndTreeCtrl::GetTreeItemArray(void)
{
	return &m_treeItemArray;
}
void CWndTreeCtrl::SetFocusElem(const LPTREEELEM pFocusElem)
{
	m_pFocusElem = pFocusElem;
}
const LPTREEELEM CWndTreeCtrl::GetFocusElem(void) const
{
	return m_pFocusElem;
}
void CWndTreeCtrl::SetLineSpace(int nLineSpace)
{
	m_nLineSpace = nLineSpace;
}
int CWndTreeCtrl::GetLineSpace(void) const
{
	return m_nLineSpace;
}
void CWndTreeCtrl::SetMemberCheckingMode(BOOL bMemberCheckingMode)
{
	m_bMemberCheckingMode = bMemberCheckingMode;
}
BOOL CWndTreeCtrl::GetMemberCheckingMode(void) const
{
	return m_bMemberCheckingMode;
}
void CWndTreeCtrl::SetMaxCheckNumber(int nMaxCheckNumber)
{
	m_nMaxCheckNumber = nMaxCheckNumber;
}
int CWndTreeCtrl::GetMaxCheckNumber(void) const
{
	return m_nMaxCheckNumber;
}
void CWndTreeCtrl::SetTreeTabWidth(int nTreeTabWidth)
{
	m_nTreeTabWidth = nTreeTabWidth;
}
int CWndTreeCtrl::GetTreeTabWidth(void) const
{
	return m_nTreeTabWidth;
}
void CWndTreeCtrl::SetCategoryTextSpace(int nCategoryTextSpace)
{
	m_nCategoryTextSpace = nCategoryTextSpace;
}
int CWndTreeCtrl::GetCategoryTextSpace(void) const
{
	return m_nCategoryTextSpace;
}
int CWndTreeCtrl::GetTreeItemsNumber(void) const
{
	int nTreeItemsNumber = 0;
	CalculateTreeItemsNumber(nTreeItemsNumber, m_treeElem.m_ptrArray);
	return nTreeItemsNumber;
}
void CWndTreeCtrl::SetTreeItemsMaxWidth(int nTreeItemsMaxWidth)
{
	m_nTreeItemsMaxWidth = nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxWidth(void) const
{
	return m_nTreeItemsMaxWidth;
}
int CWndTreeCtrl::GetTreeItemsMaxHeight(void)
{
	int nTreeItemsMaxHeight = 6;
	CalculateTreeItemsMaxHeight(nTreeItemsMaxHeight, m_treeElem.m_ptrArray);
	return nTreeItemsMaxHeight;
}
void CWndTreeCtrl::SetTextColor(DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor)
{
	CalculateTextColor(dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, m_treeElem.m_ptrArray);
}
void CWndTreeCtrl::CalculateTreeItemsNumber(int& nSumTreeItemsNumber, const CPtrArray& rPtrArray) const
{
	for (int i = 0; i < rPtrArray.GetSize(); ++i)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)rPtrArray.GetAt(i);
		if (lpTreeElem)
		{
			++nSumTreeItemsNumber;
			if (lpTreeElem->m_ptrArray.GetSize() > 0)
				CalculateTreeItemsNumber(nSumTreeItemsNumber, lpTreeElem->m_ptrArray);
		}
	}
}
void CWndTreeCtrl::CalculateTreeItemsMaxHeight(int& nSumHeight, const CPtrArray& rPtrArray)
{
	for (int i = 0; i < rPtrArray.GetSize(); ++i)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)rPtrArray.GetAt(i);
		if (lpTreeElem)
		{
			nSumHeight += GetFontHeight();
			if (lpTreeElem->m_ptrArray.GetSize() > 0 && lpTreeElem->m_bOpen == TRUE)
				CalculateTreeItemsMaxHeight(nSumHeight, lpTreeElem->m_ptrArray);
		}
	}
}
void CWndTreeCtrl::CalculateTextColor(DWORD dwCategoryTextColor, DWORD dwNormalTextColor, DWORD dwSelectedCategoryTextColor, DWORD dwSelectedNormalTextColor, const CPtrArray& rPtrArray)
{
	for (int i = 0; i < rPtrArray.GetSize(); ++i)
	{
		LPTREEELEM lpTreeElem = (LPTREEELEM)rPtrArray.GetAt(i);
		if (lpTreeElem)
		{
			if (lpTreeElem->m_ptrArray.GetSize() > 0 || lpTreeElem->m_lpParent == NULL)
			{
				lpTreeElem->m_dwColor = dwCategoryTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedCategoryTextColor;
				CalculateTextColor(dwCategoryTextColor, dwNormalTextColor, dwSelectedCategoryTextColor, dwSelectedNormalTextColor, lpTreeElem->m_ptrArray);
			}
			else
			{
				lpTreeElem->m_dwColor = dwNormalTextColor;
				lpTreeElem->m_dwSelectColor = dwSelectedNormalTextColor;
			}
		}
	}
}
#endif // __IMPROVE_QUEST_INTERFACE
void CWndTreeCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	m_pTexButtOpen = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtTreeOpen.tga"), 0xffff00ff);
	m_pTexButtClose = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtTreeClose.tga"), 0xffff00ff);
	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);//,m_pSprPack,-1);
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
}
void CWndTreeCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(4, 4);
	if (IsWndStyle(WBS_VSCROLL))
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndTreeCtrl::PaintFrame(C2DRender* p2DRender)
{

}
void CWndTreeCtrl::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nFontHeight = GetFontHeight();
	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for (int i = 0; i < m_treeItemArray.GetSize(); i++)
		SAFE_DELETE_CONST((LPTREEITEM)m_treeItemArray.GetAt(i));
	m_treeItemArray.RemoveAll();
	m_nTreeItemsMaxWidth = 0;
	PaintTree(p2DRender, pt, m_treeElem.m_ptrArray);

	int nPage = GetClientRect().Height() / m_nFontHeight;
	int nRange = m_treeItemArray.GetSize();// - nPage;

	if (IsWndStyle(WBS_VSCROLL))
	{
		m_wndScrollBar.SetVisible(TRUE);
		m_wndScrollBar.SetScrollRange(0, nRange);
		m_wndScrollBar.SetScrollPage(nPage);

		if (nPage >= m_wndScrollBar.GetMaxScrollPos())
			m_wndScrollBar.SetScrollPos(0);
	}
	else
		m_wndScrollBar.SetVisible(FALSE);
}

void CWndTreeCtrl::PaintTree(C2DRender* p2DRender, CPoint& pt, CPtrArray& ptrArray)
{
	LPTREEELEM pTreeElem;
	LPTREEITEM pTreeItem;
	for (int i = 0; i < ptrArray.GetSize(); i++)
	{
		pTreeElem = (LPTREEELEM)ptrArray.GetAt(i);
		pTreeItem = new TREEITEM;
		CSize sizeStr;
		p2DRender->m_pFont->GetTextExtent(pTreeElem->m_strKeyword, &sizeStr);
		int nRectLeft = pt.x + m_nCategoryTextSpace;
		int nRectTop = pt.y;
		int nRectRight = pt.x + m_nCategoryTextSpace + sizeStr.cx;
		int nRectBottom = pt.y + sizeStr.cy;
		pTreeItem->m_rect.SetRect(nRectLeft, nRectTop, nRectRight, nRectBottom);
		m_nTreeItemsMaxWidth = (nRectRight > m_nTreeItemsMaxWidth) ? nRectRight : m_nTreeItemsMaxWidth;
		if (m_bMemberCheckingMode == TRUE && pTreeElem->m_pWndCheckBox)
		{
			CWndButton* pWndCheckBox = pTreeElem->m_pWndCheckBox;
			if (pTreeElem->m_ptrArray.GetSize() == 0)
			{
				pWndCheckBox->EnableWindow(TRUE);
				pWndCheckBox->SetVisible(TRUE);
				pWndCheckBox->SetWndRect(CRect(pt.x, pt.y - 1, pt.x + CHECK_BOX_SIZE_XY, pt.y + CHECK_BOX_SIZE_XY - 1));
			}
		}
		pTreeItem->m_lpTreeElem = pTreeElem;
		m_treeItemArray.Add(pTreeItem);
		if (pTreeElem->m_ptrArray.GetSize())
		{

			if (pTreeElem->m_bOpen)
				p2DRender->RenderTexture(pt, m_pTexButtOpen);
			else
				p2DRender->RenderTexture(pt, m_pTexButtClose);

		}
		if (m_pFocusElem == pTreeElem)
		{
			pTreeElem->m_strKeyword.SetColor(pTreeElem->m_dwSelectColor);
			p2DRender->TextOut_EditString(nRectLeft, nRectTop, pTreeElem->m_strKeyword);
		}
		else
		{
			pTreeElem->m_strKeyword.SetColor(pTreeElem->m_dwColor);
			p2DRender->TextOut_EditString(nRectLeft, nRectTop, pTreeElem->m_strKeyword);
		}

		pt.y += GetFontHeight();
		if (pTreeElem->m_ptrArray.GetSize() && pTreeElem->m_bOpen)
		{
			pt.x += m_nTreeTabWidth;
			PaintTree(p2DRender, pt, pTreeElem->m_ptrArray);
			pt.x -= m_nTreeTabWidth;
		}
	}
}
void CWndTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for (int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
		if (pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			CWndBase* pWnd = m_pParentWnd;
			while (pWnd->GetStyle() & WBS_CHILD)
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify(WNM_CLICKED, m_nIdWnd, (LRESULT*)m_pFocusElem);
			return;
		}
	}
}
void CWndTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for (int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if (m_bMemberCheckingMode == TRUE)
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if (pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened(pParentTreeElem) == FALSE)
			{
				if (pWndCheckBox->IsWindowEnabled() == TRUE)
					pWndCheckBox->EnableWindow(FALSE);
				if (pWndCheckBox->IsVisible() == TRUE)
					pWndCheckBox->SetVisible(FALSE);
			}
		}
		CRect rect = pTreeItem->m_rect;
		SIZE size = m_pTexButtOpen->m_size;
		rect.left -= m_nCategoryTextSpace;
		rect.SetRect(rect.left, rect.top, rect.left + size.cx, rect.top + size.cy);
		if (rect.PtInRect(point))
		{
			pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
#else // __IMPROVE_QUEST_INTERFACE
		if (pTreeItem->m_rect.PtInRect(point))
		{
			CRect rect = pTreeItem->m_rect;
			rect.SetRect(rect.left, rect.top, rect.left + m_nFontHeight, rect.top + m_nFontHeight);
			if (rect.PtInRect(point))
				pTreeItem->m_lpTreeElem->m_bOpen = !pTreeItem->m_lpTreeElem->m_bOpen;
			else
				m_pFocusElem = pTreeItem->m_lpTreeElem;
		}
#endif // __IMPROVE_QUEST_INTERFACE
	}
}

void CWndTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
}

void CWndTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LPTREEITEM pTreeItem;
	for (int i = 0; i < m_treeItemArray.GetSize(); i++)
	{
		pTreeItem = (LPTREEITEM)m_treeItemArray.GetAt(i);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
		if (m_bMemberCheckingMode == TRUE)
		{
			CWndButton* pWndCheckBox = pTreeItem->m_lpTreeElem->m_pWndCheckBox;
			LPTREEELEM pParentTreeElem = pTreeItem->m_lpTreeElem->m_lpParent;
			if (pWndCheckBox && pParentTreeElem && CheckParentTreeBeOpened(pParentTreeElem) == FALSE)
			{
				if (pWndCheckBox->IsWindowEnabled() == TRUE)
					pWndCheckBox->EnableWindow(FALSE);
				if (pWndCheckBox->IsVisible() == TRUE)
					pWndCheckBox->SetVisible(FALSE);
			}
		}
#endif // __IMPROVE_QUEST_INTERFACE
		if (pTreeItem->m_rect.PtInRect(point))
		{
			m_pFocusElem = pTreeItem->m_lpTreeElem;
			m_pFocusElem->m_bOpen = !m_pFocusElem->m_bOpen;
			CWndBase* pWnd = m_pParentWnd;
			while (pWnd->GetStyle() & WBS_CHILD)
				pWnd = pWnd->GetParentWnd();
			pWnd->OnChildNotify(WNM_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusElem);
		}
	}
}
void CWndTreeCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();
	if (IsWndStyle(WBS_VSCROLL))
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
	m_wndScrollBar.SetWndRect(rect);
	CWndBase::OnSize(nType, cx, cy);
}
BOOL CWndTreeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_wndScrollBar.GetScrollPage() < m_wndScrollBar.GetMaxScrollPos())
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
	}

	return TRUE;
}
BOOL CWndTreeCtrl::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
tagScriptElem::tagScriptElem(void) :
	m_lpParent(NULL),
	m_dwColor(D3DCOLOR_ARGB(255, 64, 64, 64)),
	m_dwSelectColor(D3DCOLOR_ARGB(255, 0, 0, 255)),
	m_strKeyword(_T("")),
	m_dwData(0),
	m_bOpen(FALSE),
	m_pWndCheckBox(NULL)
{}

tagScriptElem::tagScriptElem(CRect* pRect) :
	m_lpParent(NULL),
	m_dwColor(D3DCOLOR_ARGB(255, 64, 64, 64)),
	m_dwSelectColor(D3DCOLOR_ARGB(255, 0, 0, 255)),
	m_strKeyword(_T("")),
	m_dwData(0),
	m_bOpen(FALSE),
	m_pWndCheckBox(NULL)
{
	m_strKeyword.Init(CWndBase::m_Theme.m_pFontText, pRect);
}
#endif // __IMPROVE_QUEST_INTERFACE

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndScrollBar::CWndScrollBar()
{
	m_nScrollPos = 0;
	m_nScrollMinPos = 0;
	m_nScrollMaxPos = 0;
	m_nScrollPage = 0;
	m_byWndType = WTYPE_SCROLLBAR;
}
CWndScrollBar::~CWndScrollBar()
{
}
BOOL CWndScrollBar::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CRect rc = rect;

	if (dwStyle & WBS_HORI)
		rc.top = rc.bottom - 15;
	else
	{
		rc.left = rc.right - 15;
		dwStyle |= WBS_VERT;
	}

	return CWndBase::Create(dwStyle | WBS_CHILD | WBS_DOCKING, rc, pParentWnd, nID);
}
int CWndScrollBar::GetScrollPos() const // Retrieves the current position of a scroll box.
{
	if (m_nScrollPos < 0)
		return 0;

	return m_nScrollPos;
}
int CWndScrollBar::SetScrollPos(int nPos, BOOL bRedraw) // Sets the current position of a scroll box.
{
	if (nPos < 0)
		nPos = 0;

	m_nScrollPos = nPos;
	return 1;
}
void CWndScrollBar::SetMinScrollPos()
{
	m_nScrollPos = m_nScrollMinPos;
}
void CWndScrollBar::SetMaxScrollPos()
{
	m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	if (m_nScrollPos < 0) m_nScrollPos = 0;
}
int CWndScrollBar::GetMinScrollPos()
{
	return m_nScrollMinPos;
}
int CWndScrollBar::GetMaxScrollPos()
{
	return m_nScrollMaxPos;
}
int CWndScrollBar::GetScrollRange() const
{
	if ((m_nScrollMaxPos - m_nScrollMinPos) <= 0)
		return 1;
	else
		return m_nScrollMaxPos - m_nScrollMinPos;
}
int CWndScrollBar::GetScrollAbiliableRange()
{
	CRect rect = GetClientRect();
	if (IsWndStyle(WBS_HORI))
	{
		rect.left += m_wndArrow1.GetClientRect().Width();
		rect.right -= m_wndArrow2.GetClientRect().Width();
		int nWidth = rect.Width();
		int nPadWidth = (m_nScrollPage * nWidth / GetScrollRange());
		if (nPadWidth < 6) nPadWidth = 6;
		rect.right = rect.left + nPadWidth;
		return nWidth * m_nScrollPage / nPadWidth;
	}
	else
	{
		rect.top += m_wndArrow1.GetClientRect().Height();
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		int nHeight = rect.Height();
		int nPadHeight = (m_nScrollPage * nHeight / GetScrollRange());
		if (nPadHeight < 6) nPadHeight = 6;
		rect.bottom = rect.top + nPadHeight;
		return nHeight * m_nScrollPage / nPadHeight;
	}
}
int CWndScrollBar::GetScrollAbiliablePos()
{
	return m_nScrollPos * GetScrollAbiliableRange() / GetScrollRange();
}
int CWndScrollBar::GetScrollAbiliablePage()
{
	return m_nScrollPage * GetScrollAbiliableRange() / GetScrollRange();
}
void CWndScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const // Retrieves the current minimum and maximum scroll-bar positions for the given scroll bar.
{
	*lpMinPos = m_nScrollMinPos;
	*lpMinPos = m_nScrollMaxPos;
}
void CWndScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw) // Sets minimum and maximum position values for the given scroll bar.
{
	m_nScrollMinPos = nMinPos;
	m_nScrollMaxPos = nMaxPos;
}
void CWndScrollBar::ShowScrollBar(BOOL bShow) // Shows or hides a scroll bar.
{
}
BOOL CWndScrollBar::EnableScrollBar(UINT nArrowFlags) // Enables or disables one or both arrows of a scroll bar.
{
	return TRUE;
}
BOOL CWndScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw) // Sets information about the scroll bar.
{
	return TRUE;
}
BOOL CWndScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask) // Retrieves information about the scroll bar.
{
	return TRUE;
}
int CWndScrollBar::GetScrollLimit() // Retrieves the limit of the scroll bar
{
	return 1;
}
int CWndScrollBar::GetScrollPage() const
{
	return m_nScrollPage;
}
void CWndScrollBar::SetScrollPage(int nPage)
{
	m_nScrollPage = nPage;
}

void CWndScrollBar::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	//m_rectClient.DeflateRect( 1, 1 );
	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndScrollBar::PaintFrame(C2DRender* p2DRender)
{
	static const int TEXTURE_H_SCROLL_ARROW_SIZE_X = 16;
	static const int TEXTURE_H_SCROLL_BAR_SIZE_X = m_pTexButtHScrBar->m_size.cx;

	static const int TEXTURE_V_SCROLL_ARROW_SIZE_Y = 16;
	static const int TEXTURE_V_SCROLL_BAR_SIZE_Y = m_pTexButtVScrBar->m_size.cy;

	CRect rect = GetClientRect();

	CPoint point = rect.TopLeft();
	FLOAT fHeightSize = static_cast<FLOAT>((rect.Height() - (TEXTURE_V_SCROLL_ARROW_SIZE_Y * 2)) / TEXTURE_V_SCROLL_BAR_SIZE_Y);
	FLOAT fWidthSize = static_cast<FLOAT>((rect.Width() - (TEXTURE_H_SCROLL_ARROW_SIZE_X * 2)) / TEXTURE_H_SCROLL_BAR_SIZE_X);

	if (IsWndStyle(WBS_HORI))
		p2DRender->RenderTexture(CPoint(point.x + TEXTURE_H_SCROLL_ARROW_SIZE_X, point.y), m_pTexButtHScrBar, m_nAlphaCount, fWidthSize, 1.0F);
	else
		p2DRender->RenderTexture(CPoint(point.x, point.y + TEXTURE_V_SCROLL_ARROW_SIZE_Y), m_pTexButtVScrBar, m_nAlphaCount, 1.0F, fHeightSize);
}
void CWndScrollBar::OnDraw(C2DRender* p2DRender)
{
	if (GetScrollRange() && GetScrollRange() - m_nScrollPage > 0)
	{
		CRect rect = GetClientRect();
		int nRange = GetScrollRange();
		int nPos = GetScrollPos();
		int nPage = GetScrollPage();
		int nHeight = 0, nWidth = 0;
		if (IsWndStyle(WBS_HORI))
		{
			rect.left += m_wndArrow1.GetClientRect().Width();
			rect.right -= m_wndArrow2.GetClientRect().Width();

			nWidth = rect.Width();

			int nPadWidth = (nPage * nWidth / nRange);
			if (nPadWidth < 6)
			{
				nWidth -= 6 - nPadWidth;
				nPadWidth = 6;
			}

			rect.right = rect.left + nPadWidth;
			int nPadPos = (nPos * nWidth / nRange);

			rect.OffsetRect(nPadPos, 0);
		}
		else
		{
			rect.top += m_wndArrow1.GetClientRect().Height();
			rect.bottom -= m_wndArrow2.GetClientRect().Height();
			nHeight = rect.Height();


			int nPadHeight = (nPage * nHeight / nRange);
			if (nPadHeight < 6)
			{
				nHeight -= 6 - nPadHeight;
				nPadHeight = 6;
			}
			rect.bottom = rect.top + nPadHeight;
			int nPadPos = (nPos * nHeight / nRange);

			rect.OffsetRect(0, nPadPos);
		}

		int TEXTURE_SCROLL_P_LEFT_SIZE_X = m_pTexButtHScrPLeft->m_size.cx;
		int TEXTURE_SCROLL_P_BAR_SIZE_X = m_pTexButtHScrPBar->m_size.cx;
		int TEXTURE_SCROLL_P_RIGHT_SIZE_X = m_pTexButtHScrPRight->m_size.cx;

		FLOAT fWidthSize = static_cast<FLOAT>((/*nBarSizeOffset*/ +rect.Width() / TEXTURE_SCROLL_P_BAR_SIZE_X) - 1);

		static const int TEXTURE_SCROLL_P_UP_SIZE_Y = m_pTexButtVScrPUp->m_size.cy;
		static const int TEXTURE_SCROLL_P_BAR_SIZE_Y = m_pTexButtVScrPBar->m_size.cy;
		static const int TEXTURE_SCROLL_P_DOWN_SIZE_Y = m_pTexButtVScrPDown->m_size.cy;

		CPoint point = rect.TopLeft();
		FLOAT fHeightSize = static_cast<FLOAT>((rect.Height() / TEXTURE_SCROLL_P_BAR_SIZE_Y) - 1);

		if (IsWndStyle(WBS_HORI))
		{
			if (nWidth > 0)
			{
				p2DRender->RenderTexture(CPoint(point.x, point.y), m_pTexButtHScrPLeft, m_nAlphaCount);
				p2DRender->RenderTexture(CPoint(point.x + TEXTURE_SCROLL_P_LEFT_SIZE_X, point.y), m_pTexButtHScrPBar, m_nAlphaCount, fWidthSize, 1.0F);
				p2DRender->RenderTexture(CPoint(point.x + (rect.Width() - TEXTURE_SCROLL_P_RIGHT_SIZE_X), point.y), m_pTexButtHScrPRight, m_nAlphaCount);
			}
		}
		else
		{
			if (nHeight > 0)
			{
				p2DRender->RenderTexture(CPoint(point.x, point.y), m_pTexButtVScrPUp, m_nAlphaCount);
				p2DRender->RenderTexture(CPoint(point.x, point.y + TEXTURE_SCROLL_P_UP_SIZE_Y), m_pTexButtVScrPBar, m_nAlphaCount, 1.0F, fHeightSize);
				p2DRender->RenderTexture(CPoint(point.x, point.y + (rect.Height() - TEXTURE_SCROLL_P_DOWN_SIZE_Y)), m_pTexButtVScrPDown, m_nAlphaCount);
			}
		}
	}
}
//CTexturePack CWndScrollBar::m_texturePack;

void CWndScrollBar::OnInitialUpdate()
{
	//m_pTexButtVScrUp    = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrUp.bmp"    ), 0xffff00ff );
	//m_pTexButtVScrDown  = m_texturePack.LoadTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "ButtVScrDown.bmp"  ), 0xffff00ff );
	m_pTexButtVScrBar = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrBar.bmp"), 0xffff00ff);
	m_pTexButtVScrPUp = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrPUp.bmp"), 0xffff00ff);
	m_pTexButtVScrPDown = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrPDown.bmp"), 0xffff00ff);
	m_pTexButtVScrPBar = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrPBar.bmp"), 0xffff00ff);

	m_pTexButtHScrBar = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrBar.bmp"), 0xffff00ff);
	m_pTexButtHScrPLeft = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrPLeft.bmp"), 0xffff00ff);
	m_pTexButtHScrPRight = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrPRight.bmp"), 0xffff00ff);
	m_pTexButtHScrPBar = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrPBar.bmp"), 0xffff00ff);

	CRect rect = GetClientRect();
	if (IsWndStyle(WBS_HORI))
	{
		rect.right = rect.right / 4 * 4;

		m_wndArrow1.Create("", 0, CRect(rect.left, rect.top, rect.left + rect.Height(), rect.bottom), this, 1000);
		m_wndArrow1.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrLeft.tga"), TRUE);
		m_wndArrow1.FitTextureSize();

		rect.left = 0;
		m_wndArrow2.Create("", 0, CRect(rect.right - rect.Height(), rect.top, rect.right, rect.bottom), this, 1001);
		m_wndArrow2.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtHScrRight.tga"), TRUE);
		m_wndArrow2.FitTextureSize();
	}
	else
	{
		rect.bottom = rect.bottom / 4 * 4;
		m_wndArrow1.Create("", 0, CRect(rect.left, rect.top, rect.right, rect.top + rect.Width()), this, 1000);//,m_pSprPack,13+0);
		m_wndArrow1.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrUp.tga"), TRUE);
		m_wndArrow1.FitTextureSize();

		rect.top = 0;
		m_wndArrow2.Create("", 0, CRect(rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom), this, 1001);//,m_pSprPack,13+2);
		m_wndArrow2.SetTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtVScrDown.tga"), TRUE);
		m_wndArrow2.FitTextureSize();
	}

	m_wndArrow1.SetPushTime(500);
	m_wndArrow2.SetPushTime(500);

	m_wndArrow1.DelWndStyle(WBS_NODRAWFRAME);
	m_wndArrow2.DelWndStyle(WBS_NODRAWFRAME);
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bPushPad = FALSE;
}
void CWndScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}
void CWndScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if (GetScrollPage() >= GetMaxScrollPos())
		return;
#endif // __IMPROVE_QUEST_INTERFACE

	SetCapture();
	CRect rect = GetClientRect();
	if (IsWndStyle(WBS_HORI))
	{
		rect.left += m_wndArrow1.GetClientRect().Width();
		rect.right -= m_wndArrow2.GetClientRect().Width();
		if (rect.PtInRect(point) && GetScrollRange())
		{
			rect = GetClientRect();
			rect.left += m_wndArrow1.GetClientRect().Width();
			rect.right -= m_wndArrow2.GetClientRect().Width();
			int nWidth = rect.Width();
			int nRange = GetScrollRange();
			int nPos = GetScrollPos();
			int nPage = GetScrollPage();
			int nPadWidth = (nPage * nWidth / nRange);
			if (nPadWidth < 6)
			{
				nWidth -= 6 - nPadWidth;
				nPadWidth = 6;
			}
			rect.right = rect.left + nPadWidth;
			int nPadPos = (nPos * nWidth / nRange);
			rect.OffsetRect(nPadPos, 0);

			if (rect.PtInRect(point))
			{
				SetCapture();
				m_nPos = point.x - rect.left;
				m_bPushPad = TRUE;
			}
			else
				if (point.x < rect.left)
				{
					m_nScrollPos -= m_nScrollPage;
					if (m_nScrollPos < m_nScrollMinPos)
						m_nScrollPos = m_nScrollMinPos;
				}
				else
					if (point.x > rect.right)
					{
						m_nScrollPos += m_nScrollPage;
						if (m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
							m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
					}
		}
	}
	else
	{
		rect.top += m_wndArrow1.GetClientRect().Height();
		rect.bottom -= m_wndArrow2.GetClientRect().Height();
		if (rect.PtInRect(point) && GetScrollRange())
		{
			rect = GetClientRect();
			rect.top += m_wndArrow1.GetClientRect().Height();
			rect.bottom -= m_wndArrow2.GetClientRect().Height();
			int nHeight = rect.Height();
			int nRange = GetScrollRange();
			int nPos = GetScrollPos();
			int nPage = GetScrollPage();
			int nPadHeight = (nPage * nHeight / nRange);
			if (nPadHeight < 6)
			{
				nHeight -= 6 - nPadHeight;
				nPadHeight = 6;
			}
			rect.bottom = rect.top + nPadHeight;
			int nPadPos = (nPos * nHeight / nRange);
			rect.OffsetRect(0, nPadPos);

			if (rect.PtInRect(point))
			{
				SetCapture();
				m_nPos = point.y - rect.top;
				m_bPushPad = TRUE;
			}
			else
				if (point.y < rect.top)
				{
					m_nScrollPos -= m_nScrollPage;
					if (m_nScrollPos < m_nScrollMinPos)
						m_nScrollPos = m_nScrollMinPos;
				}
				else
					if (point.y > rect.bottom)
					{
						m_nScrollPos += m_nScrollPage;
						if (m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
							m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
					}
		}
	}
}
void CWndScrollBar::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bPushPad)
	{
		CRect rect = GetClientRect();
		int nHeight, nPadHeight;
		int nWidth, nPadWidth;
		if (IsWndStyle(WBS_HORI))
		{
			point.x -= (m_wndArrow1.GetClientRect().Width() + m_nPos);
			rect.left += m_wndArrow1.GetClientRect().Width();
			rect.right -= m_wndArrow2.GetClientRect().Width();

			nWidth = rect.Width();
			// rect.Height() : pt.y = GetScrollRange() : x
			int nPadWidth = (m_nScrollPage * nWidth / GetScrollRange());
			if (nPadWidth < 6)
				nWidth -= 6 - nPadWidth;
			if (nWidth != 0)
				m_nScrollPos = point.x * GetScrollRange() / nWidth;
		}
		else
		{
			point.y -= (m_wndArrow1.GetClientRect().Height() + m_nPos);
			rect.top += m_wndArrow1.GetClientRect().Height();
			rect.bottom -= m_wndArrow2.GetClientRect().Height();

			int nHeight = rect.Height();
			// rect.Height() : pt.y = GetScrollRange() : x
			int nPadHeight = (m_nScrollPage * nHeight / GetScrollRange());
			if (nPadHeight < 6)
				nHeight -= 6 - nPadHeight;
			if (nHeight != 0)
				m_nScrollPos = point.y * GetScrollRange() / nHeight;
		}


		if (m_nScrollPos < m_nScrollMinPos)
			m_nScrollPos = m_nScrollMinPos;
		if (m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
			m_nScrollPos = m_nScrollMaxPos - m_nScrollPage;
	}
}
BOOL CWndScrollBar::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	switch (nID)
	{
	case 1000:
		if (--m_nScrollPos < m_nScrollMinPos)
			m_nScrollPos = m_nScrollMinPos;
		//TRACE("%d\n",m_nScrollPos);
		break;
	case 1001:
		if (++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
			m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
		//TRACE("%d\n",m_nScrollPos);
		break;
	}
	return TRUE;
}
void CWndScrollBar::SetScrollDown()
{
	if (++m_nScrollPos > m_nScrollMaxPos - m_nScrollPage)
		m_nScrollPos--;// = m_nScrollMaxPos - m_nScrollPage;
}
void CWndScrollBar::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	if (IsWndStyle(WBS_HORI))
	{
		rect.right = rect.right / 4 * 4;

		m_wndArrow1.SetWndRect(CRect(rect.left, rect.top, rect.left + rect.Height(), rect.bottom));
		m_wndArrow2.SetWndRect(CRect(rect.right - rect.Height(), rect.top, rect.right, rect.bottom));

		CWndBase::OnSize(nType, cx, cy);
	}
	else
	{
		rect.bottom = rect.bottom / 4 * 4;

		m_wndArrow1.SetWndRect(CRect(rect.left, rect.top, rect.right, rect.top + rect.Width()));
		m_wndArrow2.SetWndRect(CRect(rect.left, rect.bottom - rect.Width(), rect.right, rect.bottom));

		CWndBase::OnSize(nType, cx, cy);
	}
}
void CWndScrollBar::DoAlpha(int doAlpha)
{
	m_nAlphaCount = m_wndArrow1.m_nAlphaCount = m_wndArrow2.m_nAlphaCount = doAlpha;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListBox
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListBox::CWndListBox()
{
	m_nWndColor = D3DCOLOR_ARGB(255, 0x5 << 3, 0x5 << 3, 0x5 << 3);
	m_nFontColor = D3DCOLOR_ARGB(255, 64, 64, 64);
	//m_nSelectColor = D3DCOLOR_ARGB( 255, 64, 64, 0   );
	m_nSelectColor = D3DCOLOR_ARGB(255, 64, 64, 255);
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	m_dwOnMouseColor = D3DCOLOR_ARGB(255, 255, 128, 0);
	m_dwInvalidColor = D3DCOLOR_ARGB(255, 170, 170, 170);
	m_nLeftMargin = 0;
#endif // __IMPROVE_QUEST_INTERFACE
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType = WTYPE_LISTBOX;
	m_nLineSpace = 1;
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_nALPHAPos = -1;
	m_clkALPHADelay = clock();
#ifdef __FL_NEW_BOXES
	m_bOwnRender = FALSE;
#endif

}
CWndListBox::~CWndListBox()
{
	ResetContent();
}
void CWndListBox::Create(DWORD dwListBoxStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	//m_dwListBoxStyle = dwListBoxStyle;
	CWndBase::Create(dwListBoxStyle | WBS_CHILD, rect, pParentWnd, nID);
}
void CWndListBox::LoadListBoxScript(LPCTSTR lpFileName)
{
	CScanner scanner;
	if (scanner.Load(lpFileName) == FALSE)
		return;
	scanner.GetToken(); // keyword
	do {
		InterpriteScript(scanner, m_listItemArray);
		scanner.GetToken(); // keyword
	} while (scanner.tok != FINISHED);

}
void CWndListBox::InterpriteScript(CScanner& scanner, CPtrArray& ptrArray)
{
	do {
		LPLISTITEM lpListItem = new LISTITEM;
		lpListItem->m_strWord = scanner.Token;
		ptrArray.Add(lpListItem);
		scanner.GetToken();
	} while (*scanner.token != '}' && scanner.tok != FINISHED);
	if (scanner.tok == FINISHED)
		return;
	scanner.GetToken();
}
void CWndListBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();

	if (IsWndStyle(WBS_VSCROLL))
		m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	else if (IsWndStyle(WBS_HSCROLL))
		m_wndScrollBar.Create(WBS_DOCKING | WBS_HORI, rect, this, 1000);
	else
		m_wndScrollBar.SetVisible(FALSE);
}
#ifdef __FL_PACKED_ITEMS
BOOL CWndListBox::OnDropIcon(LPSHORTCUT pShortcut, CPoint point)
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify(WIN_ITEMDROP, m_nIdWnd, (LRESULT*)pShortcut);
}
#endif
void CWndListBox::OnDraw(C2DRender* p2DRender)
{
#ifdef __FL_NEW_BOXES
	if (m_bOwnRender)
		return;
#endif

	CWndBase* pWnd = GetParentWnd();
	CPoint pt(3, 3);

#ifdef __WIKI_ITEMS_V2
	if (pWnd && pWnd->GetWndId() == APP_WIKI_ITEMS)
	{
		CWndWikiItems* pWndWiki = (CWndWikiItems*)pWnd;
		m_nFontHeight = 40;
		CPoint pt(10, 3);

		vector<ItemProp*>* pVecItems = pWndWiki->GetItemList();
		CRect rectClient = GetClientRect();

		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pVecItems->size());
		m_wndScrollBar.SetScrollPage(nPage);

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)m_wndScrollBar.GetMaxScrollPos(); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			ItemProp* pProp = pVecItems->at(i);
			if (!pProp)
				continue;

			DWORD dwColor = 0xFF000000;
			p2DRender->RenderLine(CPoint(-2, pt.y + 37), CPoint(rectClient.right - 3, pt.y + 37), dwColor);
			if (i == m_nCurSelect)
			{
				CRect DrawRect = CRect(0, pt.y - 2, rectClient.right, pt.y + 36);
				p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
			}
			if (strlen(pProp->szIcon))
			{
				CString szIcon = pProp->szIcon;
				if (pProp->dwItemKind3 == IK3_EGG && pProp->dwID != II_PET_EGG)
					szIcon.Replace(".", "_00.");
				CTexture* pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, szIcon), 0xffff00ff);
				if (pIcon)
					pIcon->Render(p2DRender, pt);
			}
			CRect rectToolTip(pt.x, pt.y, pt.x + 35, pt.y + 35);
			if (rectToolTip.PtInRect(m_ptMouse))
			{
				CPoint pt2 = m_ptMouse;
				ClientToScreen(&pt2);
				ClientToScreen(&rectToolTip);
				CItemElem Item;
				Item.m_dwItemId = pProp->dwID;
				g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, APP_WIKI_ITEMS);
			}
			pt.x += 40;
			pt.y += 10;
			CString strName = pProp->szName;
			if (pProp->dwItemJob != NULL_ID)
				g_WndMng.StringSize(strName, 29);
			p2DRender->TextOut(pt.x, pt.y, strName, dwColor);

			pt.x += 235;
			if (pProp->dwItemJob < MAX_JOB)
				p2DRender->TextOut(pt.x, pt.y, prj.m_aJob[pProp->dwItemJob].szName, dwColor);


			int nLevel = (pProp->dwLimitLevel1 == NULL_ID ? 1 : pProp->dwLimitLevel1);
			strName.Format("%d", nLevel);
			pt.x += 129 - strName.GetLength() * 4;
			p2DRender->TextOut(pt.x, pt.y, strName, dwColor);
			pt.x = 10;
			pt.y += m_nFontHeight - 10;
		}
	}
#endif
	else if (pWnd->GetWndId() == APP_DIALOG_EVENT)
	{
		m_nFontHeight = 35;
		int nGapX = 40;
		CPoint pt(10, 0);
		CExchange::PSETLIST pSet = prj.m_Exchange.FindExchange(((CWndDialogEvent*)pWnd)->m_nMMI);
		CTexture* pArrow = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_THEME, "ExchangeArrow.tga"), 0xFFFF0000);

		if (!pSet)
			return;
		unsigned nPos = m_wndScrollBar.GetScrollPos();
		unsigned nEnd = nPos + 7;
		if (nEnd > pSet->vecSet.size())
			nEnd = pSet->vecSet.size();
		if (clock() > m_clkALPHADelay)
		{
			m_nALPHAPos++;
			m_clkALPHADelay = clock() + SEC(2);
			if (m_nALPHAPos < 0)
				m_nALPHAPos = 0;
		}

		for (unsigned i = nPos; i < nEnd; ++i)
		{
			if (pt.y > GetWndRect().Height())
				break;
			if (i == m_nCurSelect)
			{
				CRect DrawRect = CRect(0, pt.y + 1, 434, pt.y + 34);
				p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
			}
			for (unsigned j = 0; j < pSet->vecSet[i].vecCondItem.size(); j++)
			{
				ItemProp* pPropItem = prj.GetItemProp(pSet->vecSet[i].vecCondItem[j].dwItemId);
				if (pPropItem)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pPropItem->szIcon), 0xffff00ff);
					DWORD dwBlend = 255;

					if (pPropItem->dwID == II_GOLD_SEED1)
					{
						if (g_pPlayer->GetGold() < pSet->vecSet[i].vecCondItem[j].nItemNum)
							dwBlend = 60;
					}
					else
					{
						if (g_pPlayer->m_Inventory.GetItemCount(pPropItem->dwID) < pSet->vecSet[i].vecCondItem[j].nItemNum)
							dwBlend = 60;
					}

					if (pTex)
						pTex->Render(p2DRender, CPoint(pt.x, pt.y + 2), dwBlend);

					TCHAR szTemp[32];
					_stprintf(szTemp, "%d", pSet->vecSet[i].vecCondItem[j].nItemNum);
					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
					CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
					m_p2DRender->TextOut(pt.x + 30 - size.cx, pt.y + 30 - size.cy + 4, szTemp, 0xffeaebdd);
					p2DRender->SetFont(pOldFont);

					CRect rectTT = CRect(pt.x, pt.y, pt.x + 32, pt.y + 32);
					if (rectTT.PtInRect(m_ptMouse))
					{
						CItemElem elem;
						CPoint pt2 = m_ptMouse;
						ClientToScreen(&pt2);
						ClientToScreen(&rectTT);
						elem.m_dwItemId = pSet->vecSet[i].vecCondItem[j].dwItemId;
						elem.m_byFlag = pSet->vecSet[i].vecCondItem[j].byFalg;
						g_WndMng.PutToolTip_Item(&elem, pt2, &rectTT);
					}

					pt.x += nGapX;
				}
			}
			pArrow->Render(p2DRender, pt);
			pt.x += nGapX;
			int nALPHA = m_nALPHAPos % pSet->vecSet[i].vecPayItem.size();
			for (unsigned j = 0; j < pSet->vecSet[i].vecPayItem.size(); j++)
			{
				ItemProp* pPropItem = prj.GetItemProp(pSet->vecSet[i].vecPayItem[j].dwItemId);
				if (pPropItem)
				{
					CTexture* pTex = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pPropItem->szIcon), 0xffff00ff);
					DWORD dwBlend = 60;
					if (j == nALPHA)
						dwBlend = 255;

					if (pTex)
						pTex->Render(p2DRender, CPoint(pt.x, pt.y + 2), dwBlend);

					TCHAR szTemp[32];
					_stprintf(szTemp, "%d", pSet->vecSet[i].vecPayItem[j].nItemNum);
					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
					CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
					m_p2DRender->TextOut(pt.x + 30 - size.cx, pt.y + 30 - size.cy + 4, szTemp, 0xffeaebdd);
					p2DRender->SetFont(pOldFont);

					CRect rectTT = CRect(pt.x, pt.y, pt.x + 32, pt.y + 32);
					if (rectTT.PtInRect(m_ptMouse))
					{
						CItemElem elem;
						CPoint pt2 = m_ptMouse;
						ClientToScreen(&pt2);
						ClientToScreen(&rectTT);
						elem.m_dwItemId = pSet->vecSet[i].vecPayItem[j].dwItemId;
						elem.m_byFlag = pSet->vecSet[i].vecPayItem[j].byFalg;
						g_WndMng.PutToolTip_Item(&elem, pt2, &rectTT);
					}

					pt.x += nGapX;
				}
			}
			pt.x = 10;
			pt.y += m_nFontHeight + 1;

			p2DRender->RenderLine(CPoint(0, pt.y), CPoint(GetWindowRect().Width() - 10, pt.y), 0xFF776655);
		}
		int nRange = pSet->vecSet.size();
		int nPage = GetClientRect().Height() / m_nFontHeight;
		if (IsWndStyle(WBS_VSCROLL))
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
		else
			m_wndScrollBar.SetVisible(FALSE);
	}
#ifdef __FL_SHOP_SEARCH
	else if (pWnd->GetWndId() == APP_SHOP_SEARCH)
	{
		CWndShopSearch* pWndMain = (CWndShopSearch*)pWnd;
		m_nFontHeight = 40;
		CPoint pt(10, 3);

		vector<CWndShopSearch::SHOP_ITEMS>* pVecItems = pWndMain->GetItemList();
		CRect rectClient = GetClientRect();

		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pVecItems->size());
		m_wndScrollBar.SetScrollPage(nPage);

		CTexture* pPerinIcon = nullptr;
		CTexture* pPenyaIcon = nullptr;

		ItemProp* pPerinProp = prj.GetItemProp(II_SYS_SYS_SCR_PERIN);
		ItemProp* pPenyaProp = prj.GetItemProp(II_GOLD_SEED4);

		if (strlen(pPerinProp->szIcon))
			pPerinIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pPerinProp->szIcon), 0xffff00ff);
		if (strlen(pPenyaProp->szIcon))
			pPenyaIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pPenyaProp->szIcon), 0xffff00ff);

		CItemElem* pItemElem = nullptr;
		ItemProp* pProp = nullptr;

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVecItems->size()); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			pItemElem = pVecItems->at(i).pItem;
			pProp = pItemElem->GetProp();

			// Render line and rect
			DWORD dwColor = 0xFF000000;
			p2DRender->RenderLine(CPoint(-2, pt.y + 37), CPoint(rectClient.right - 3, pt.y + 37), dwColor);
			if (i == m_nCurSelect)
			{
				CRect DrawRect = CRect(0, pt.y - 2, rectClient.right, pt.y + 36);
				p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
			}

			// Item icon
			if (pItemElem->m_pTexture)
			{
				pItemElem->m_pTexture->Render(p2DRender, pt);

				if (pProp->dwPackMax > 1)
				{
					short nItemNum = pItemElem->GetExtra();
					TCHAR szTemp[32];
					_stprintf(szTemp, "%d", nItemNum);
					CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);

					CD3DFont* pOldFont = p2DRender->GetFont();
					p2DRender->SetFont(CWndBase::m_Theme.m_pFontItemNumbers);
					m_p2DRender->TextOut(pt.x + 32 - size.cx, pt.y + 34 - size.cy, szTemp, 0xffeaebdd);
					p2DRender->SetFont(pOldFont);
				}
			}
			CRect rectToolTip(pt.x, pt.y, pt.x + 35, pt.y + 35);
			if (rectToolTip.PtInRect(m_ptMouse))
			{
				CPoint pt2 = m_ptMouse;
				ClientToScreen(&pt2);
				ClientToScreen(&rectToolTip);
				g_WndMng.PutToolTip_Item(pItemElem, pt2, &rectToolTip, APP_SHOP_SEARCH);
			}

			// Item name
			pt.x += 40;
			pt.y += 10;
			CString strTmp = pItemElem->GetName();
			if (pItemElem->GetAbilityOption())
				strTmp.Format("%s %+d", strTmp, pItemElem->GetAbilityOption());
			g_WndMng.StringSize(strTmp, 18);
			p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

			// Render item level
			pt.x += 150;
			int nLevel = (pProp->dwLimitLevel1 == NULL_ID ? 1 : pProp->dwLimitLevel1);
			strTmp.Format("%d", nLevel);
			p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

			// Owner name
			pt.x += 45;
			strTmp = pVecItems->at(i).strOwnerName;
			p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

			// Vendor title
			pt.x += 150;
			strTmp = pVecItems->at(i).strVendorTitle;
			g_WndMng.StringSize(strTmp, 12);
			p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

			LONG tmpY = pt.y;
			pt.x += 120;

			// Currency Icons
			BOOL bBoth = FALSE;
			if (pItemElem->m_nPerinCost && pItemElem->m_nCost)
				bBoth = TRUE;

			if (bBoth)
			{
				pt.x += 4;
				pt.y -= 10;
				p2DRender->RenderTexture(pt, pPerinIcon, 255, 0.6f, 0.6f);
				pt.y += 18;
				p2DRender->RenderTexture(pt, pPenyaIcon, 255, 0.6f, 0.6f);
			}
			else
			{
				pt.y -= 6;
				p2DRender->RenderTexture(pt, pItemElem->m_nPerinCost ? pPerinIcon : pPenyaIcon, 255, 0.8f, 0.8f);
			}


			// Prices values
			pt.x += 28;

			if (bBoth)
			{
				pt.x -= 4;
				pt.y -= 16;
				p2DRender->TextOut(pt.x, pt.y, Separator(pItemElem->m_nPerinCost), dwColor);
				pt.y += 18;
				p2DRender->TextOut(pt.x, pt.y, Separator(pItemElem->m_nCost), dwColor);
			}
			else
			{
				pt.y += 5;
				p2DRender->TextOut(pt.x, pt.y, pItemElem->m_nPerinCost ? Separator(pItemElem->m_nPerinCost) : Separator(pItemElem->m_nCost), dwColor);
			}

			pt.y = tmpY;

			pt.x = 10;
			pt.y += m_nFontHeight - 10;
		}
	}
#endif
#ifdef __FL_PARTY_FINDER
	else if (pWnd->GetWndId() == APP_PARTY_FINDER)
	{
		CWndPartyFinder* pWndMain = (CWndPartyFinder*)pWnd;
		m_nFontHeight = 22;
		CPoint pt(5, 3);

		vector<PARTY_LIST>* pVec = pWndMain->GetList();
		CRect rectClient = GetClientRect();

		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pVec->size());
		m_wndScrollBar.SetScrollPage(nPage);

		const float fTexSizes = 0.5f;
		const int nLineY = m_nFontHeight - 4;
		int nLeaderJob;
		int nLeaderLevel;
		CString strAll;

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVec->size()); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			DWORD dwColor = 0xFF000000;

			// Render line and rect
			p2DRender->RenderLine(CPoint(-2, pt.y + nLineY), CPoint(rectClient.right - 3, pt.y + nLineY), dwColor);
			if (i == m_nCurSelect)
			{
				CRect DrawRect = CRect(0, pt.y - 3, rectClient.right, pt.y + nLineY);
				p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
			}

			// Render leader information
			nLeaderJob = pVec->at(i).m_nLeaderJob;
			nLeaderLevel = pVec->at(i).m_nLeaderLevel;
			strAll = pVec->at(i).m_strLeaderName;

			if (prj.m_aJob[nLeaderJob].dwJobType == JTYPE_PRO)
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(24, pt.y), (19 + nLeaderJob - 6), 0xffffffff, 0.9f, 0.85f);
			else if (prj.m_aJob[nLeaderJob].dwJobType == JTYPE_MASTER)
			{
				int nMasterIndex = 27;
				if (nLeaderLevel < 70)
					nMasterIndex = 27;
				else if (nLeaderLevel >= 70 && nLeaderLevel < 80)
					nMasterIndex = 28;
				else if (nLeaderLevel >= 80 && nLeaderLevel < 90)
					nMasterIndex = 29;
				else if (nLeaderLevel >= 90 && nLeaderLevel < 100)
					nMasterIndex = 30;
				else if (nLeaderLevel >= 100 && nLeaderLevel < 110)
					nMasterIndex = 31;
				else if (nLeaderLevel >= 110 && nLeaderLevel <= 120)
					nMasterIndex = 32;

				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(5, pt.y), nMasterIndex, 0xffffffff, 0.9f, 0.85f);
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(24, pt.y), (19 + nLeaderJob - 16), 0xffffffff, 0.9f, 0.85f);
			}
			else if (prj.m_aJob[nLeaderJob].dwJobType == JTYPE_HERO)
			{
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(5, pt.y), 33, 0xffffffff, 0.9f, 0.9f);
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(24, pt.y), (19 + nLeaderJob - 24), 0xffffffff, 0.9f, 0.85f);
			}
			else if (prj.m_aJob[nLeaderJob].dwJobType == JTYPE_LEGEND_HERO)
			{
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(5, pt.y), 33, 0xffffffff, 0.9f, 0.9f);
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(24, pt.y), (19 + nLeaderJob - 32), 0xffffffff, 0.9f, 0.85f);
			}
			else
				g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(24, pt.y), 14 + nLeaderJob, 0xffffffff, 0.9f, 0.85f);

			g_WndMng.StringSize(strAll, 10);

			if (MAX_PROFESSIONAL <= nLeaderJob && nLeaderJob < MAX_MASTER)
				strAll.Format("%s (%d-M)", strAll, nLeaderLevel);
			else if (MAX_MASTER <= nLeaderJob)
				strAll.Format("%s (%d-H)", strAll, nLeaderLevel);
			else if (MAX_HERO <= nLeaderJob)
				strAll.Format("%s (%d-H)", strAll, nLeaderLevel);
			else
				strAll.Format("%s (%d)", strAll, nLeaderLevel);

			p2DRender->TextOut(43, pt.y, strAll, dwColor);

			// Members in party
			strAll.Format("%d/%d", pVec->at(i).m_nPartyMembers, MAX_PTMEMBER_SIZE_SPECIAL);
			p2DRender->TextOut(190, pt.y, strAll, dwColor);

			// Points
			strAll.Format("%d", pVec->at(i).m_nPartyPoint);
			p2DRender->TextOut(263, pt.y, strAll, dwColor);

			// Level
			strAll.Format("%d", pVec->at(i).m_nPartyLevel);
			p2DRender->TextOut(318, pt.y, strAll, dwColor);

			// Scrolls
			if (pWndMain->m_pTexRed)
				p2DRender->RenderTexture(CPoint(365, pt.y), pWndMain->m_pTexRed, pVec->at(i).m_bRedScroll ? 255 : 100, fTexSizes, fTexSizes);
			if (pWndMain->m_pTexOrange)
				p2DRender->RenderTexture(CPoint(385, pt.y), pWndMain->m_pTexOrange, pVec->at(i).m_bOrangeScroll ? 255 : 100, fTexSizes, fTexSizes);
			if (pWndMain->m_pTexGreen)
				p2DRender->RenderTexture(CPoint(405, pt.y), pWndMain->m_pTexGreen, pVec->at(i).m_bGreenScroll ? 255 : 100, fTexSizes, fTexSizes);

			// Skills
			if (pWndMain->m_pTexLink)
				p2DRender->RenderTexture(CPoint(425, pt.y), pWndMain->m_pTexLink, pVec->at(i).m_bLink ? 255 : 100, fTexSizes, fTexSizes);
			if (pWndMain->m_pTexDrop)
				p2DRender->RenderTexture(CPoint(445, pt.y), pWndMain->m_pTexDrop, pVec->at(i).m_bDrop ? 255 : 100, fTexSizes, fTexSizes);
			if (pWndMain->m_pTexGift)
				p2DRender->RenderTexture(CPoint(465, pt.y), pWndMain->m_pTexGift, pVec->at(i).m_bGift ? 255 : 100, fTexSizes, fTexSizes);
			if (pWndMain->m_pTextResting)
				p2DRender->RenderTexture(CPoint(485, pt.y), pWndMain->m_pTextResting, pVec->at(i).m_bResting ? 255 : 100, fTexSizes, fTexSizes);

			pt.y += m_nFontHeight;
		}
	}
#endif
#ifdef __FL_GUILD_FINDER
	else if (pWnd->GetWndId() == APP_GUILD_FINDER)
	{
		CWndGuildFinder* pWndMain = (CWndGuildFinder*)pWnd;

		m_nFontHeight = 22;
		CPoint pt(10, 3);
		CRect rectClient = GetClientRect();
		DWORD dwColor = 0xFF000000;
		const int nLineY = m_nFontHeight - 4;

		int nJob;
		int nLevel;
		CString strAll;

		if (m_nIdWnd == WIDC_LISTBOX1)
		{
			vector<CWndGuildFinder::GUILD_LIST>* pVec = pWndMain->GetList();

			int nPage = rectClient.Height() / m_nFontHeight;
			m_wndScrollBar.SetScrollRange(0, pVec->size());
			m_wndScrollBar.SetScrollPage(nPage);

			ItemProp* pProp;
			CTexture* pTexture;

			for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVec->size()); ++i)
			{
				if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
					break;

				// Render line and rect
				p2DRender->RenderLine(CPoint(-2, pt.y + nLineY), CPoint(rectClient.right - 3, pt.y + nLineY), dwColor);
				if (i == m_nCurSelect)
				{
					CRect DrawRect = CRect(0, pt.y - 3, rectClient.right, pt.y + nLineY);
					p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
				}

				CWndGuildFinder::GUILD_LIST* pGuild = &pVec->at(i);

				// Render house texture
				if (pGuild->m_bGuildHouse && pWndMain->m_pTexHouse)
					p2DRender->RenderTexture(CPoint(pt.x, pt.y - 2), pWndMain->m_pTexHouse, 255, 0.7f, 0.7f);

				// Render guild level
				strAll.Format("%d", pGuild->m_nLevel);
				pt.x += 45;
				p2DRender->TextOut(pt.x, pt.y, strAll, dwColor);

				// Render guild name
				strAll.Format("%s", pGuild->m_szGuild);
				g_WndMng.StringSize(strAll, 10);
				pt.x += 45;
				p2DRender->TextOut(pt.x, pt.y, strAll, dwColor);

				// Render amount of members
				strAll.Format("%d / %02d", pGuild->m_nMembers, pGuild->m_nMaxMembers);
				pt.x += 85;
				p2DRender->TextOut(pt.x, pt.y, strAll, dwColor);

				// Render leader information
				nJob = pGuild->m_nLeaderJob;
				nLevel = pGuild->m_nLeaderLevel;
				strAll = pGuild->m_strLeaderName;

				pt.x += 60;
				if (prj.m_aJob[nJob].dwJobType == JTYPE_PRO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 6), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_MASTER)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 16), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_HERO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 24), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_LEGEND_HERO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 32), 0xffffffff, 0.9f, 0.85f);
				else
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), 14 + nJob, 0xffffffff, 0.9f, 0.85f);

				g_WndMng.StringSize(strAll, 8);
				strAll.Format("Lv.%d %s", nLevel, strAll);
				pt.x += 18;
				p2DRender->TextOut(pt.x, pt.y, strAll, dwColor);

				pt.x += 114;
				for (auto& it : pGuild->m_vecFurniture)
				{
					pProp = prj.GetItemProp(it);
					if (!pProp)
						continue;

					if (!strlen(pProp->szIcon))
						continue;

					pTexture = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
					if (!pTexture)
						continue;

					p2DRender->RenderTexture(pt, pTexture, 255, 0.4, 0.4);

					CRect rectItem = CRect(pt.x, pt.y, pt.x + 14, pt.y + 15);
					if (rectItem.PtInRect(m_ptMouse))
					{
						CPoint pt2 = m_ptMouse;
						ClientToScreen(&pt2);
						ClientToScreen(&rectItem);

						CItemElem elem;
						elem.m_dwItemId = it;
						g_WndMng.PutToolTip_Item(&elem, pt2, &rectItem);
					}

					pt.x += 14;
				}

				pt.x = 10;
				pt.y += m_nFontHeight;
			}
		}
		else if (m_nIdWnd == WIDC_LISTBOX2)
		{
			vector<CWndGuildFinder::GUILD_MEMBER>* pVecMembers = pWndMain->GetListMembers();
			if (!pVecMembers)
				return;

			int nPage = rectClient.Height() / m_nFontHeight;
			m_wndScrollBar.SetScrollRange(0, pVecMembers->size());
			m_wndScrollBar.SetScrollPage(nPage);

			for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVecMembers->size()); ++i)
			{
				if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
					break;

				// Render line and rect
				p2DRender->RenderLine(CPoint(-2, pt.y + nLineY), CPoint(rectClient.right - 3, pt.y + nLineY), dwColor);
				if (i == m_nCurSelect)
				{
					CRect DrawRect = CRect(0, pt.y - 3, rectClient.right, pt.y + nLineY);
					p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
				}

				CWndGuildFinder::GUILD_MEMBER* pMember = &pVecMembers->at(i);

				// Render leader information
				strAll = pMember->m_strName;
				nLevel = pMember->m_nLevel;
				nJob = pMember->m_nJob;

				if (prj.m_aJob[nJob].dwJobType == JTYPE_PRO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 6), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_MASTER)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 16), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_HERO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 24), 0xffffffff, 0.9f, 0.85f);
				else if (prj.m_aJob[nJob].dwJobType == JTYPE_LEGEND_HERO)
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), (19 + nJob - 32), 0xffffffff, 0.9f, 0.85f);
				else
					g_WndMng.m_pWndWorld->m_texPlayerDataIcon.Render(p2DRender, CPoint(pt.x, pt.y), 14 + nJob, 0xffffffff, 0.9f, 0.85f);

				//g_WndMng.StringSize(strAll, 8);
				strAll.Format("Lv.%d %s", nLevel, strAll);
				pt.x += 18;
				p2DRender->TextOut(pt.x, pt.y, strAll, dwColor);

				pt.x = 10;
				pt.y += m_nFontHeight;
			}
		}
	}
#endif
	else if (pWnd && pWnd->GetWndId() == APP_COLLECTING_INFO)
	{
		m_nFontHeight = 35;
		CPoint pt(10, 3);

		CCollectingProperty* pProperty = CCollectingProperty::GetInstance();
		CRect rectClient = GetClientRect();
		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pProperty->GetItemPtr()->size());
		m_wndScrollBar.SetScrollPage(nPage);

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pProperty->GetItemPtr()->size()); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			p2DRender->RenderLine(CPoint(-2, pt.y + 35), CPoint(rectClient.right - 6, pt.y + 35), 0xFF776655);

			DWORD dwItem = pProperty->GetItemPtr()->at(i).dwItemId;
			ItemProp* pItemProp = prj.GetItemProp(dwItem);
			if (pItemProp)
			{
				DWORD dwColor = 0xFF000000;
				if (strlen(pItemProp->szIcon))
				{
					CString szIcon = pItemProp->szIcon;
					if (pItemProp->dwItemKind3 == IK3_EGG && pItemProp->dwID != II_PET_EGG)
						szIcon.Replace(".", "_00.");
					CTexture* pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, szIcon), 0xffff00ff);
					pIcon->Render(p2DRender, CPoint(pt.x, pt.y + 3));
				}

				CRect rectToolTip(pt.x, pt.y, pt.x + 35, pt.y + 35);
				if (rectToolTip.PtInRect(m_ptMouse))
				{
					CPoint pt2 = m_ptMouse;
					ClientToScreen(&pt2);
					ClientToScreen(&rectToolTip);
					CItemElem Item;
					Item.m_dwItemId = pItemProp->dwID;
					g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, APP_COLLECTING_INFO);
				}

				pt.y += 10;
				pt.x += 50;
				CString strTmp = pItemProp->szName;
				g_WndMng.StringSize(strTmp, 20);
				p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

				DWORD dwProb = pProperty->GetItemPtr()->at(i).dwChance;
				strTmp.Format("%.4f%%", (float)dwProb / 10000000.0f);
				pt.x += 170;
				p2DRender->TextOut(pt.x, pt.y, strTmp, dwColor);

				pt.x = 10;
				pt.y += m_nFontHeight - 10;
			}
		}
	}
#ifdef __FL_FFA
	else if (pWnd && pWnd->GetWndId() == APP_FFA_RESULT)
	{
		CWndFFAResult* pWndMain = (CWndFFAResult*)pWnd;
		vector<FFA_PLAYER>* pVec = pWndMain->GetPlayerList();
		size_t size = pVec->size();
		if (!size)
			return;

		m_nFontHeight = 35;
		CPoint pt(10, 3);

		CRect rectClient = GetClientRect();
		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, size);
		m_wndScrollBar.SetScrollPage(nPage);

		CString strFormat;
		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(size); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			p2DRender->RenderLine(CPoint(-2, pt.y + 35), CPoint(rectClient.right - 6, pt.y + 35), 0xFF776655);

			pt.y += 10;

			DWORD dwColor = 0xFF000000;

			// Place
			strFormat.Format("%d.", i + 1);
			p2DRender->TextOut(10, pt.y, strFormat, dwColor);

			// Name
			strFormat = CPlayerDataCenter::GetInstance()->GetPlayerString(pVec->at(i).idPlayer);
			g_WndMng.StringSize(strFormat, 12);
			p2DRender->TextOut(35, pt.y, strFormat, dwColor);

			// Kills
			strFormat.Format("%d", pVec->at(i).nKills);
			p2DRender->TextOut(135, pt.y, strFormat, dwColor);

			// Deaths
			strFormat.Format("%d", pVec->at(i).nDeaths);
			p2DRender->TextOut(165, pt.y, strFormat, dwColor);

			// K/D
			strFormat.Format("%.2f", static_cast<float>(pVec->at(i).nKills) / (pVec->at(i).nDeaths == 0 ? 1 : pVec->at(i).nDeaths));
			p2DRender->TextOut(200, pt.y, strFormat, dwColor);

			pt.y += m_nFontHeight - 10;
		}
	}
#endif
#ifdef __FL_TDM
	else if (pWnd && pWnd->GetWndId() == APP_TDM_RESULT)
	{
		CWndTDMResult* pWndMain = (CWndTDMResult*)pWnd;
		vector<TDM_PLAYER>* pVec = pWndMain->GetPlayerList();
		size_t size = pVec->size();
		if (!size)
			return;

		m_nFontHeight = 35;
		CPoint pt(10, 3);

		CRect rectClient = GetClientRect();
		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, size);
		m_wndScrollBar.SetScrollPage(nPage);

		CString strFormat;
		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(size); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			p2DRender->RenderLine(CPoint(-2, pt.y + 35), CPoint(rectClient.right - 6, pt.y + 35), 0xFF776655);

			pt.y += 10;

			DWORD dwColor = 0xFF000000;
			if (g_WndMng.m_pWndWorld)
				dwColor = g_WndMng.m_pWndWorld->GetColorByTeam(pVec->at(i).team);

			// Place
			strFormat.Format("%d.", i + 1);
			p2DRender->TextOut(10, pt.y, strFormat, dwColor);

			// Name
			strFormat = CPlayerDataCenter::GetInstance()->GetPlayerString(pVec->at(i).idPlayer);
			g_WndMng.StringSize(strFormat, 12);
			p2DRender->TextOut(35, pt.y, strFormat, dwColor);

			// Kills
			strFormat.Format("%d", pVec->at(i).nKills);
			p2DRender->TextOut(135, pt.y, strFormat, dwColor);

			// Deaths
			strFormat.Format("%d", pVec->at(i).nDeaths);
			p2DRender->TextOut(165, pt.y, strFormat, dwColor);

			// K/D
			strFormat.Format("%.2f", static_cast<float>(pVec->at(i).nKills) / (pVec->at(i).nDeaths == 0 ? 1 : pVec->at(i).nDeaths));
			p2DRender->TextOut(200, pt.y, strFormat, dwColor);

			pt.y += m_nFontHeight - 10;
		}
	}
	else if (pWnd && pWnd->GetWndId() == APP_TDM_REG)
		((CWndTDMRegister*)pWnd)->DrawListBox(p2DRender);
#endif
#ifdef __FL_TELEPORT
	else if (pWnd && pWnd->GetWndId() == APP_TELEPORT && m_nIdWnd == WIDC_LISTBOX1)
	{
		CWndTeleport* pWnd2 = (CWndTeleport*)pWnd;
		m_nFontHeight = 40;
		CPoint pt(10, 3);

		vector<DWORD>* pVec = pWnd2->GetList();
		if (pVec == NULL)
			return;

		CRect rectClient = GetClientRect();

		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pVec->size());
		m_wndScrollBar.SetScrollPage(nPage);

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVec->size()); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			ItemProp* pProp = prj.GetItemProp(pVec->at(i));
			if (!pProp)
				continue;

			DWORD dwColor = 0xFF000000;
			p2DRender->RenderLine(CPoint(-2, pt.y + 37), CPoint(rectClient.right - 2, pt.y + 37), dwColor);
			if (i == m_nCurSelect)
			{
				CRect DrawRect = CRect(0, pt.y - 2, rectClient.right, pt.y + 36);
				p2DRender->RenderFillRect(DrawRect, 0xFFbef5b0);
			}

			if (strlen(pProp->szIcon))
			{
				CString szIcon = pProp->szIcon;
				if (pProp->dwItemKind3 == IK3_EGG && pProp->dwID != II_PET_EGG)
					szIcon.Replace(".", "_00.");
				CTexture* pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, szIcon), 0xffff00ff);
				if (pIcon)
					pIcon->Render(p2DRender, pt);
			}
			CRect rectToolTip(pt.x, pt.y, pt.x + 35, pt.y + 35);
			if (rectToolTip.PtInRect(m_ptMouse))
			{
				CPoint pt2 = m_ptMouse;
				ClientToScreen(&pt2);
				ClientToScreen(&rectToolTip);
				CItemElem Item;
				Item.m_dwItemId = pProp->dwID;
				g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, APP_TELEPORT);
			}
			pt.x += 40;
			pt.y += 10;

			CString str; str.Format("%s", pProp->szName);
			g_WndMng.StringSize(str, 24);
			p2DRender->TextOut(pt.x, pt.y, str, dwColor);
			pt.x = 10;
			pt.y += m_nFontHeight - 10;
		}
	}
#endif
#ifdef __FL_RECYCLE
	else if (pWnd && pWnd->GetWndId() == APP_RECYCLE)
	{
		CWndRecycle* pWnd2 = (CWndRecycle*)pWnd;
		m_nFontHeight = 40;
		CPoint pt(10, 3);

		vector<CWndRecycle::RECYCLE_ITEMS>* pVec = pWnd2->GetItemList();

		CRect rectClient = GetClientRect();

		int nPage = rectClient.Height() / m_nFontHeight;
		m_wndScrollBar.SetScrollRange(0, pVec->size());
		m_wndScrollBar.SetScrollPage(nPage);

		for (unsigned i = (unsigned)m_wndScrollBar.GetScrollPos(); i < (unsigned)(pVec->size()); ++i)
		{
			if (i > (unsigned)(nPage + m_wndScrollBar.GetScrollPos()))
				break;

			ItemProp* pProp = pVec->at(i).pItemProp;
			if (!pProp)
				continue;

			DWORD dwColor = 0xFF000000;
			p2DRender->RenderLine(CPoint(-2, pt.y + 37), CPoint(rectClient.right - 2, pt.y + 37), dwColor);

			if (strlen(pProp->szIcon))
			{
				CString szIcon = pProp->szIcon;
				if (pProp->dwItemKind3 == IK3_EGG && pProp->dwID != II_PET_EGG)
					szIcon.Replace(".", "_00.");
				CTexture* pIcon = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_ITEM, szIcon), 0xffff00ff);
				if (pIcon)
					pIcon->Render(p2DRender, pt);
			}
			CRect rectToolTip(pt.x, pt.y, pt.x + 35, pt.y + 35);
			if (rectToolTip.PtInRect(m_ptMouse))
			{
				CPoint pt2 = m_ptMouse;
				ClientToScreen(&pt2);
				ClientToScreen(&rectToolTip);
				CItemElem Item;
				Item.m_dwItemId = pProp->dwID;
				g_WndMng.PutToolTip_Item(&Item, pt2, &rectToolTip, APP_RECYCLE);
			}
			pt.x += 40;
			pt.y += 10;
			CString strName = pProp->szName;
			g_WndMng.StringSize(strName, 20);
			p2DRender->TextOut(pt.x, pt.y, strName, dwColor);

			// Reward count
			pt.x += 170;
			strName.Format("%d", pVec->at(i).nPrice);
			p2DRender->TextOut(pt.x, pt.y, strName, dwColor);

			pt.x = 10;
			pt.y += m_nFontHeight - 10;
		}
	}
#endif	
#ifdef __FL_PACKED_ITEMS
	else if (pWnd && pWnd->GetWndId() == APP_PACKED_ITEMS)
		((CWndPackedItems*)pWnd)->DrawItemlist(p2DRender);
	else if (pWnd && pWnd->GetWndId() == APP_PACKED_ITEMS_VIEW)
		((CWndPackedItemsPreview*)pWnd)->DrawItemlist(p2DRender);
#endif
#ifdef __FL_WHEEL_OF_FORTUNE
	else if (pWnd && pWnd->GetWndId() == APP_WOF)
		((CWndWof*)pWnd)->DrawChances(p2DRender);
#endif
#ifdef __FL_RECORD_BOOK
	else if (pWnd && pWnd->GetWndId() == APP_RECORD_BOOK)
		((CWndRecordBook*)pWnd)->DrawListBox(p2DRender);
#endif
#ifdef __FL_MONSTER_ALBUM
	else if (pWnd && pWnd->GetWndId() == APP_MONSTER_ALBUM)
		((CWndMonsterAlbum*)pWnd)->DrawListBox(p2DRender);
#endif
	else
	{
		m_nFontHeight = GetFontHeight() + m_nLineSpace;

		int nPage = 0;
		if (IsWndStyle(WBS_HSCROLL))
			nPage = GetClientRect().Width() / m_nFontHeight;
		else
			nPage = GetClientRect().Height() / m_nFontHeight;
		const int nRange = m_listItemArray.GetSize();

		if (IsWndStyle(WBS_VSCROLL) || IsWndStyle(WBS_HSCROLL))
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
		else
			m_wndScrollBar.SetVisible(FALSE);

		const int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;
		const int nScrollBarHeight = IsWndStyle(WBS_HSCROLL) ? m_wndScrollBar.GetClientRect().Height() : 0;

		CPoint pt(3, 3);
		for (int i = m_wndScrollBar.GetScrollPos(); i < static_cast<unsigned>(m_listItemArray.GetSize()); i++)
		{
			if (i > nPage + m_wndScrollBar.GetScrollPos())
				break;

			tagITEM* pListItem = static_cast<tagITEM*>(m_listItemArray.GetAt(i));
			if (!pListItem->m_bIsVisible)
				continue;

			if (pListItem->m_bIsValid)
			{
				pListItem->m_rect.left = pt.x;
				pListItem->m_rect.top = pt.y;

				if (IsWndStyle(WBS_HSCROLL))
				{
					pListItem->m_rect.right = pt.x + m_nFontHeight;
					pListItem->m_rect.bottom = pt.y + m_rectWindow.Height() - nScrollBarWidth;
				}
				else
				{
					pListItem->m_rect.right = pt.x + m_rectWindow.Width() - nScrollBarHeight;
					pListItem->m_rect.bottom = pt.y + m_nFontHeight;
				}

				if (i == m_nCurSelect)
					pListItem->m_strWord.SetColor(m_nSelectColor);
				else
				{
					CPoint point = GetMousePoint();
					if (pListItem->m_rect.PtInRect(point))
						pListItem->m_strWord.SetColor(m_dwOnMouseColor);
					else
						pListItem->m_strWord.SetColor(m_nFontColor);
				}
			}
			else
				pListItem->m_strWord.SetColor(m_dwInvalidColor);

			p2DRender->TextOut_EditString(static_cast<int>(m_nLeftMargin + pt.x), pt.y, pListItem->m_strWord);

			if (IsWndStyle(WBS_HSCROLL))
				pt.x += m_nFontHeight;
			else
				pt.y += m_nFontHeight;
		}
	}
}

void CWndListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);

	if (IsWndStyle(WBS_HSCROLL))
		pt.x -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	else
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();

	CRect rect;
	LPLISTITEM pListItem;
	for (int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);

		if (pListItem->m_bIsValid == FALSE)
		{
			if (IsWndStyle(WBS_HSCROLL))
				pt.x += m_nFontHeight;
			else
				pt.y += m_nFontHeight;
			continue;
		}
		int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;
		int nScrollBarHeight = IsWndStyle(WBS_HSCROLL) ? m_wndScrollBar.GetClientRect().Height() : 0;

		if (IsWndStyle(WBS_HSCROLL))
			rect.SetRect(pt.x, pt.y, pt.x + m_nFontHeight, pt.y + m_rectWindow.Height() - nScrollBarHeight);
		else
			rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight);

		if (rect.PtInRect(point))
		{
			m_nCurSelect = i;
			m_pFocusItem = pListItem;

			CWndBase* pWnd = m_pParentWnd;
			pWnd->OnChildNotify(WNM_SELCHANGE, m_nIdWnd, (LRESULT*)pListItem);
			return;
		}

		if (IsWndStyle(WBS_HSCROLL))
			pt.x += m_nFontHeight;
		else
			pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);

	if (IsWndStyle(WBS_HSCROLL))
		pt.x -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	else
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();

	CRect rect;
	LPLISTITEM pListItem;
	for (int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);
		if (pListItem->m_bIsValid == FALSE)
		{
			pt.y += m_nFontHeight;
			continue;
		}
		int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;
		int nScrollBarHeight = IsWndStyle(WBS_HSCROLL) ? m_wndScrollBar.GetClientRect().Height() : 0;

		if (IsWndStyle(WBS_HSCROLL))
			rect.SetRect(pt.x, pt.y, pt.x + m_nFontHeight, pt.y + m_rectWindow.Height() - nScrollBarHeight);
		else
			rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight);

		if (rect.PtInRect(point))
		{
			if (m_pFocusItem == pListItem)
			{
				CWndBase* pWnd = m_pParentWnd;
				pListItem->m_rect.left = point.x;
				pListItem->m_rect.top = point.y;
				pWnd->OnChildNotify(WNM_SELCANCEL, m_nIdWnd, (LRESULT*)pListItem);//m_pFocusItem);
				return;
			}
		}

		if (IsWndStyle(WBS_HSCROLL))
			pt.x += m_nFontHeight;
		else
			pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);

	if (IsWndStyle(WBS_HSCROLL))
		pt.x -= m_nFontHeight * m_wndScrollBar.GetScrollPos();
	else
		pt.y -= m_nFontHeight * m_wndScrollBar.GetScrollPos();

	CRect rect;
	LPLISTITEM pListItem;
	for (int i = 0; i < m_listItemArray.GetSize(); i++)
	{
		pListItem = (LPLISTITEM)m_listItemArray.GetAt(i);

		int nScrollBarWidth = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetClientRect().Width() : 0;
		int nScrollBarHeight = IsWndStyle(WBS_HSCROLL) ? m_wndScrollBar.GetClientRect().Height() : 0;

		if (IsWndStyle(WBS_HSCROLL))
			rect.SetRect(pt.x, pt.y, pt.x + m_nFontHeight, pt.y + m_rectWindow.Height() - nScrollBarHeight);
		else
			rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - nScrollBarWidth, pt.y + m_nFontHeight);

		if (rect.PtInRect(point))
		{
			if (m_pFocusItem == pListItem)
			{
				CWndBase* pWnd = m_pParentWnd;
				while (pWnd->GetStyle() & WBS_CHILD)
					pWnd = pWnd->GetParentWnd();
				pWnd->OnChildNotify(WNM_DBLCLK, m_nIdWnd, (LRESULT*)m_pFocusItem);
				return;
			}
		}

		if (IsWndStyle(WBS_HSCROLL))
			pt.x += m_nFontHeight;
		else
			pt.y += m_nFontHeight;
	}
}
void CWndListBox::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
BOOL CWndListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos())
		return TRUE;

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

int CWndListBox::GetCount() const
{
	return m_listItemArray.GetSize();
}
DWORD CWndListBox::GetItemData(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_dwData;
}

void* CWndListBox::GetItemDataPtr(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return (void*)lpListItem->m_dwData;
}

int CWndListBox::SetItemData(int nIndex, DWORD dwItemData)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = dwItemData;
	return 0;
}

int CWndListBox::SetItemDataPtr(int nIndex, void* pData)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData = (DWORD)pData;
	return 0;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
DWORD CWndListBox::GetItemData2(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_dwData2;
}

void* CWndListBox::GetItemData2Ptr(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return (void*)lpListItem->m_dwData2;
}

BOOL CWndListBox::GetItemValidity(int nIndex)
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_bIsValid;
}

BOOL CWndListBox::GetItemVisibility(int nIndex)
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_bIsVisible;
}

int CWndListBox::SetItemData2(int nIndex, DWORD dwItemData)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData2 = dwItemData;
	return 0;
}

int CWndListBox::SetItemData2Ptr(int nIndex, void* pData)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_dwData2 = (DWORD)pData;
	return 0;
}

int CWndListBox::SetItemValidity(int nIndex, BOOL bValidity)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_bIsValid = bValidity;
	return 0;
}

int CWndListBox::SetItemVisibility(int nIndex, BOOL bIsVisible)
{
	if (nIndex < 0 || nIndex >= m_listItemArray.GetSize())
		return LB_ERR;
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_bIsVisible = bIsVisible;
	return 0;
}
#endif // __IMPROVE_QUEST_INTERFACE

const CRect& CWndListBox::GetItemRect(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_rect;
}

int CWndListBox::GetSel(int nIndex) const
{
	return 1;
}

int CWndListBox::GetText(int nIndex, LPSTR lpszBuffer) const
{
	return 1;
}

void CWndListBox::GetText(int nIndex, CString& rString) const
{
	if (nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		rString = lpListItem->m_strWord;
	}
}

int CWndListBox::GetTextLen(int nIndex) const
{
	if (nIndex >= 0 && nIndex < m_listItemArray.GetSize())
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		return lpListItem->m_strWord.GetLength();
	}
	return 0;
}

int CWndListBox::GetCurSel() const
{
	return m_nCurSelect;
}

int CWndListBox::SetCurSel(int nSelect)
{
	if (-1 == nSelect)
	{
		m_nCurSelect = nSelect;
		return 0;
	}

	if (nSelect >= 0 && nSelect < m_listItemArray.GetSize())
	{
		m_pFocusItem = (LPLISTITEM)m_listItemArray.GetAt(nSelect);
		m_nCurSelect = nSelect;
		return m_nCurSelect;
	}

	Error("CWndListBox::SetCurSel(int nSelect) 범위 넘어섬 %d", nSelect);

	return 0;
}

int CWndListBox::SetSel(int nIndex, BOOL bSelect)
{
	return 1;
}

int CWndListBox::GetSelCount() const
{
	return 1;
}

int CWndListBox::GetSelItems(int nMaxItems, LPINT rgIndex) const
{
	return 1;
}


int CWndListBox::AddString(LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = new LISTITEM;
	lpListItem->m_strWord = lpszItem;
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	lpListItem->m_strWord.Init(m_pFont, &GetClientRect());
	lpListItem->m_strWord.SetParsingString(lpszItem, m_nFontColor, 0x00000000, 0, 0x00000001, TRUE);
#endif // __IMPROVE_QUEST_INTERFACE
	m_listItemArray.Add(lpListItem);
	return m_listItemArray.GetSize() - 1;
}

int CWndListBox::DeleteString(UINT nIndex)
{
	SAFE_DELETE_CONST((LPLISTITEM)m_listItemArray.GetAt(nIndex));
	m_listItemArray.RemoveAt(nIndex);
	if (nIndex == m_nCurSelect)
	{
		m_nCurSelect = -1;
		m_pFocusItem = NULL;
	}
	return 1;
}

void CWndListBox::SetString(int nIndex, LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_strWord = lpszItem;
}

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
const CString& CWndListBox::GetString(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_strWord;
}

void CWndListBox::SetListStringAlpha(int nIndex, BYTE byAlpha)
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_strWord.SetAlpha(byAlpha);
}

void CWndListBox::SetKeyString(int nIndex, LPCTSTR lpszItem)
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	lpListItem->m_strKey = lpszItem;
}

const CString& CWndListBox::GetKeyString(int nIndex) const
{
	LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
	return lpListItem->m_strKey;
}

void CWndListBox::SetOnMouseColor(DWORD dwOnMouseColor)
{
	m_dwOnMouseColor = dwOnMouseColor;
}

DWORD CWndListBox::GetOnMouseColor(void) const
{
	return m_dwOnMouseColor;
}

void CWndListBox::SetInvalidColor(DWORD dwInvalidColor)
{
	m_dwInvalidColor = dwInvalidColor;
}

DWORD CWndListBox::GetInvalidColor(void) const
{
	return m_dwInvalidColor;
}

void CWndListBox::SetLeftMargin(int nLeftMargin)
{
	m_nLeftMargin = nLeftMargin;
}

int CWndListBox::GetLeftMargin(void) const
{
	return m_nLeftMargin;
}
#endif // __IMPROVE_QUEST_INTERFACE

int CWndListBox::GetItemIndex(const CString& strItem) const
{
	for (int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex)
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		if (lpListItem == NULL)
		{
			continue;
		}

		if (strcmp(strItem, lpListItem->m_strWord) == 0)
		{
			return nIndex;
		}
	}

	return -1;
}

int CWndListBox::GetItemIndex(DWORD dwItem) const
{
	for (int nIndex = 0; nIndex < m_listItemArray.GetSize(); ++nIndex)
	{
		LPLISTITEM lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nIndex);
		if (lpListItem == NULL)
		{
			continue;
		}

		if (dwItem == lpListItem->m_dwData)
		{
			return nIndex;
		}
	}

	return -1;
}

int CWndListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	return 1;
}

void CWndListBox::ResetContent()
{
	for (int i = 0; i < m_listItemArray.GetSize(); i++)
		SAFE_DELETE_CONST((LPLISTITEM)m_listItemArray.GetAt(i));
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_listItemArray.RemoveAll();
	m_wndScrollBar.SetScrollPos(0, FALSE);
}

int CWndListBox::FindString(int nStartAfter, LPCTSTR lpszItem) const
{
	if (m_listItemArray.GetSize() == 0)
		return -1;

	LPLISTITEM lpListItem;
	int nCmp = 0;
	int nBegin = 0, nEnd = m_listItemArray.GetSize() - 1;
	int nDiv = nBegin + ((nEnd - nBegin) / 2);
	do
	{
		if (nCmp == 1) nEnd = nDiv;
		if (nCmp == -1) nBegin = nDiv;
		nDiv = nBegin + ((nEnd - nBegin) / 2);
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nDiv);
		nCmp = strcmp(lpListItem->m_strWord, lpszItem);
		if (nCmp == 0) return nDiv;
	} while ((nEnd - nBegin) > 1);
	int nResult;
	lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nBegin);
	nCmp = strcmp(lpListItem->m_strWord, lpszItem);
	if (nCmp >= 0)
		nResult = nBegin;
	else
	{
		lpListItem = (LPLISTITEM)m_listItemArray.GetAt(nEnd);
		nResult = nEnd;
	}
	if (lpszItem[0] != lpListItem->m_strWord[0])
		nResult = -1;
	return nResult;
}
/*

input d

0 5 = 2
2 5 = 3
2 3 = 2

0 a
1 b
2 c
3 f
4 f
5 f

0 5 = 2
2 5 = 3
3 5 = 4
3 4 = 3

0 a
1 b
2 c
3 c
4 f
5 f
*/
int QSortListBox(const VOID* arg1, const VOID* arg2)
{
	//return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
	LPCTSTR pSrc = (*(CWndListBox::LISTITEM**)arg1)->m_strWord;
	LPCTSTR pDst = (*(CWndListBox::LISTITEM**)arg2)->m_strWord;

	return _stricmp(pSrc, pDst);
}
void CWndListBox::SortListBox()
{
	qsort(m_listItemArray.GetData(), m_listItemArray.GetSize(), sizeof(LPLISTITEM), QSortListBox);
}
int CWndListBox::FindStringExact(int nIndexStart, LPCTSTR lpszItem) const
{
	return 1;
}

int CWndListBox::SelectString(int nStartAfter, LPCTSTR lpszItem)
{
	m_nCurSelect = FindString(nStartAfter, lpszItem);
	if (m_nCurSelect == -1)
		return -1;
	int nPos = m_wndScrollBar.GetScrollPos();
	int nRange = m_wndScrollBar.GetScrollRange();
	int nPage = m_wndScrollBar.GetScrollPage();
	if ((m_nCurSelect < nPos || m_nCurSelect > nPos + nPage))
	{
		if (m_nCurSelect < nRange - nPage)
			m_wndScrollBar.SetScrollPos(m_nCurSelect < 0 ? 0 : m_nCurSelect);
		else
			m_wndScrollBar.SetScrollPos(nRange - nPage);
	}
	return m_nCurSelect;
}
void CWndListBox::OnSize(UINT nType, int cx, int cy)
{
	//	m_string.Reset( m_pFont, &GetClientRect() );

	CRect rect = GetWindowRect();
	if (IsWndStyle(WBS_VSCROLL))
		rect.left = rect.right - VSCROLL_WIDTH;
	else if (IsWndStyle(WBS_HSCROLL))
		rect.top = rect.bottom - VSCROLL_WIDTH;
	else
		m_wndScrollBar.SetVisible(FALSE);

	m_wndScrollBar.SetWndRect(rect);
	CWndBase::OnSize(nType, cx, cy);
}
void CWndListBox::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(4, 4);

	if (IsWndStyle(WBS_VSCROLL))
		m_rectClient.right -= 15;
	else if (IsWndStyle(WBS_HSCROLL))
		m_rectClient.bottom -= 15;
	else
		m_wndScrollBar.SetVisible(FALSE);

	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndListBox::PaintFrame(C2DRender* p2DRender)
{
	CWndBase::PaintFrame(p2DRender);
}
BOOL CWndListBox::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
void CWndListBox::OnSetFocus(CWndBase* pOldWnd)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndScrollBar
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndText::CWndText()
{
	m_dwOffset = 0;
	m_dwBlockBegin = 0;
	m_dwBlockEnd = 0;
	m_bLButtonDown = FALSE;
	m_timerCaret.Set(1000);
	m_bCaretVisible = FALSE;
	m_ptCaret = 0;
	m_nLineSpace = 5;
	if (GetLanguage() == LANG_FRE)
		m_nLineSpace = 0;
	m_byWndType = WTYPE_TEXT;
	m_bEnableClipboard = TRUE;
	memset(m_szCaret, 0, sizeof(char) * 3);

	m_szCaret[0] = 0;
	m_strTexture = DEF_CTRL_TEXT;
	m_bTile = TRUE;
	m_ptDeflateRect = CPoint(6, 6);
	m_strBg = "";
}
CWndText::~CWndText()
{
}
BOOL CWndText::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	return CWndBase::Create(dwStyle | WBS_CHILD, rect, pParentWnd, nID);
}
void CWndText::OnInitialUpdate()
{
	m_wndMenu.CreateMenu(this);

	if (GetLanguage() == LANG_FRE)
	{
		CRect rect = GetWndRect();
		rect.bottom += 2;
		SetWndRect(rect, TRUE);
	}

	m_string.Init(m_pFont, &GetClientRect());
	CRect rect = GetClientRect();
	m_wndScrollBar.Create(WBS_DOCKING | WBS_VERT, rect, this, 1000);
	UpdateScrollBar();
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));

	m_bScr = TRUE;
	m_nLineRefresh = 0;
}
void CWndText::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect();
	m_string.Init(m_pFont, &rect);
	rect = GetWindowRect();
	if (IsWndStyle(WBS_VSCROLL))
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
	m_wndScrollBar.SetWndRect(rect);
	UpdateScrollBar();
	CWndBase::OnSize(nType, cx, cy);
}
void CWndText::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	if (!IsWndStyle(WBS_NOFRAME))
		m_rectClient.DeflateRect(m_ptDeflateRect.x, m_ptDeflateRect.y);

	if (IsWndStyle(WBS_VSCROLL))
		m_rectClient.right -= 15;
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}
}


void CWndText::PaintFrame(C2DRender* p2DRender)
{
}
void CWndText::OnDraw(C2DRender* p2DRender)
{
#ifdef __FL_UPDATE_CHATTING
	if (GetParentWnd())
	{
		if (GetParentWnd()->GetWndId() == APP_COMMUNICATION_CHAT || GetParentWnd()->GetWndId() == APP_CHAT_LOG)
		{
			if (GetParentWnd()->m_nResizeDir)
				return;
		}
	}
#endif

	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine(m_dwOffset);
	CPoint ptCaret = 0;
	DWORD dwLineCount = m_string.GetLineCount();
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont(m_pFont);

	int nPos = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetScrollPos() : 0;
	int nLines = IsWndStyle(WBS_VSCROLL) ? m_wndScrollBar.GetScrollPage() : GETRANGE;// GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();

	int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
	int nRange = m_string.GetLineCount();

	if (IsWndStyle(WBS_VSCROLL))
	{
		m_wndScrollBar.SetVisible(TRUE);
		m_wndScrollBar.SetScrollRange(0, nRange);
		m_wndScrollBar.SetScrollPage(nPage);

		if (GetParentWnd()->m_nIdWnd == APP_MESSAGE_NOTE)
		{
			SetWndRect(GetWindowRect(TRUE), FALSE);
			if (m_nLineRefresh < (int)(m_string.GetLineCount()))
			{
				m_wndScrollBar.SetScrollDown();
				m_nLineRefresh = m_string.GetLineCount();
			}
		}
	}
	else
		m_wndScrollBar.SetVisible(FALSE);

	if (IsWndStyle(EBS_PASSWORD))
	{
		for (int i = nPos, y = 0; i < (int)(dwLineCount); i++, y++)
		{
			dwOffset = m_string.GetLineOffset(i);
			string = m_string.GetLine(i);
			dwBegin = 0;
			int nLength = string.GetLength();
			const char* begin = string;
			const char* end = begin + string.GetLength();
			const char* iter = begin;

			while (*iter) {

				WORD wCodePage = m_string.m_awCodePage[dwCurOffset];

				dwCurOffset = dwOffset + (iter - begin);

				const char* next = CharNextEx(iter, wCodePage);

				strHan[0] = '*';
				strHan[1] = 0;

				CSize size;
				p2DRender->m_pFont->GetTextExtent("*", &size);
				size.cx *= next - iter;

				DWORD dwColor = m_string.m_adwColor[dwCurOffset];
				DWORD dwStyle = m_string.m_abyStyle[dwCurOffset];

				if (m_bEnableClipboard)
				{
					DWORD dwBlockBegin;
					DWORD dwBlockEnd;
					if (m_dwBlockBegin > m_dwBlockEnd)
						dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
					else
						dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;


					if (dwCurOffset >= dwBlockBegin && dwCurOffset < dwBlockEnd)
					{
						DWORD dwBkgr = dwColor;
						dwColor = ~dwColor | 0xff000000;
						p2DRender->RenderFillRect(CRect(dwBegin, 0 + y * dwMaxHeight, dwBegin + size.cx, 0 + y * dwMaxHeight + size.cy), dwBkgr);
					}
				}
				p2DRender->TextOut(dwBegin, 0 + y * dwMaxHeight, strHan, dwColor);
				if (dwStyle & ESSTY_BOLD)
					p2DRender->TextOut(dwBegin + 1, 0 + y * dwMaxHeight, strHan, dwColor);
				if (dwStyle & ESSTY_UNDERLINE)
					p2DRender->RenderLine(CPoint(dwBegin, y * dwMaxHeight + size.cy), CPoint(dwBegin + size.cx, y * dwMaxHeight + size.cy), dwColor);



				if (dwCurOffset == m_dwOffset)
				{
					strcpy(m_szCaret, strHan);
					ptCaret = CPoint(dwBegin, 0 + y * dwMaxHeight);
				}
				dwBegin += size.cx;

				iter = next;
			}
		}
	}
	else
	{
		if (nLines == 6)
		{
			int a = 0;
		}
		BlockSetStyle(ESSTY_BLOCK);
		p2DRender->TextOut_EditString(0, 0, m_string, nPos, nLines, m_nLineSpace);
	}

	DrawCaret(p2DRender);

	if (strlen(m_string) == 0 && m_strBg.GetLength())
		p2DRender->TextOut(2, 0, m_strBg, D3DCOLOR_ARGB(115, 0, 0, 0));

	p2DRender->SetFont(pOldFont);
}

void CWndText::BlockSetStyle(DWORD dwStyle)
{
	if (m_bEnableClipboard)
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if (m_dwBlockBegin > m_dwBlockEnd)
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if (dwBlockEnd - dwBlockBegin)
		{
			m_string.SetStyle(dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle);
		}
	}
}

void CWndText::BlockSetColor(DWORD dwColor)
{
	if (m_bEnableClipboard)
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if (m_dwBlockBegin > m_dwBlockEnd)
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if (dwBlockEnd - dwBlockBegin)
		{
			m_string.SetColor(dwBlockBegin, dwBlockEnd - dwBlockBegin, dwColor);
		}
	}
}


void CWndText::BlockClearStyle(DWORD dwStyle)
{
	if (m_bEnableClipboard)
	{
		DWORD dwBlockBegin;
		DWORD dwBlockEnd;
		if (m_dwBlockBegin > m_dwBlockEnd)
			dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
		else
			dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
		if (dwBlockEnd - dwBlockBegin)
		{
			m_string.ClearStyle(dwBlockBegin, dwBlockEnd - dwBlockBegin, dwStyle);
		}
	}
}

void CWndText::UpdateScrollBar()
{
	if (IsWndStyle(WBS_VSCROLL))
	{

		int nPage = GETRANGE;//( GetClientRect().Height() + m_nLineSpace ) / GetFontHeight();
		int nRange = m_string.GetLineCount();// - nPage;

		if (IsWndStyle(WBS_VSCROLL))
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
		else
			m_wndScrollBar.SetVisible(FALSE);

		if (m_wndScrollBar.GetScrollPos() > (m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage()))
			m_wndScrollBar.SetMaxScrollPos();
	}

}
CPoint CWndText::OffsetToPoint(DWORD dwSetOffset, TCHAR* pszStr)
{
	CString string;
	DWORD dwMaxHeight = GetFontHeight();
	DWORD dwOffset = 0;
	TCHAR strHan[3];
	DWORD dwBegin = 0;
	DWORD dwCurOffset = 0;
	DWORD dwOffsetLine = m_string.OffsetToLine(m_dwOffset);
	CPoint ptCaret(0, 0);
	for (int i = m_wndScrollBar.GetScrollPos(); i < (int)(m_string.GetLineCount()); i++)
	{
		dwOffset = m_string.GetLineOffset(i);
		string = m_string.GetLine(i);
		dwBegin = 0;
		strHan[0] = 0;
		const char* begin = string;
		const char* end = begin + string.GetLength();
		const char* iter = begin;

		while (*iter && iter < end) {

			WORD wCodePage = m_string.m_awCodePage[dwOffset + (iter - begin)];

			const char* next = CharNextEx(iter, wCodePage);

			SIZE size;

			if (IsWndStyle(EBS_PASSWORD)) {
				m_pFont->GetTextExtent("*", &size);
				size.cx *= next - iter;
			}
			else {
				char temp[16];
				memcpy(temp, iter, next - iter);
				temp[next - iter] = 0;
				m_pFont->GetTextExtent(temp, &size, wCodePage);
			}

			dwCurOffset = dwOffset + (iter - begin);


			if (dwCurOffset == dwSetOffset)
			{
				if (pszStr)
					strcpy(pszStr, strHan);
				return CPoint(dwBegin, 0 + i * dwMaxHeight);// + GetScreenRect().TopLeft();
			}
			dwBegin += size.cx;
			iter = next;
		}


		if (dwSetOffset == m_string.GetLength())
		{
			if (!m_dwOffset || m_string.GetAt(m_dwOffset - 1) != '\n')
			{
				if (pszStr)
					pszStr[0] = 0;
				ptCaret = CPoint(dwBegin, i * dwMaxHeight);// + GetScreenRect().TopLeft();
			}
			else
			{
				if (pszStr)
					pszStr[0] = 0;
				ptCaret = CPoint(0, (i + 1) * dwMaxHeight);// + GetScreenRect().TopLeft();
			}
		}
	}
	return ptCaret;
}

void CWndText::DrawCaret(C2DRender* p2DRender)
{
}
LONG CWndText::GetOffset(CPoint point)
{
	CString string;
	int dwMaxHeight = GetFontHeight();
	CPoint pt = point;
	pt.y /= dwMaxHeight;
	pt.y += m_wndScrollBar.GetScrollPos();
	DWORD dwOffset1 = m_string.GetLineOffset(pt.y);
	DWORD dwOffset2 = m_string.GetLineOffset(pt.y + 1);
	DWORD dwBegin = 0;
	const char* begin = m_string;
	const char* end = begin + dwOffset2;
	const char* iter = begin + dwOffset1;

	while (*iter && iter < end) {

		if (*iter == '\r' || *iter == '\n')
		{
			return iter - begin;
		}

		WORD wCodePage = m_string.m_awCodePage[iter - begin];

		const char* next = CharNextEx(iter, wCodePage);

		SIZE size;

		if (IsWndStyle(EBS_PASSWORD))
		{
			m_pFont->GetTextExtent("*", &size);
			size.cx *= next - iter;
		}
		else
		{
			char temp[16];
			memcpy(temp, iter, next - iter);
			temp[next - iter] = 0;
			m_pFont->GetTextExtent(temp, &size, wCodePage);
		}

		if ((int)(dwBegin + size.cx) > pt.x)
		{
#ifdef __AIO
			if (pt.x < 0)
				return iter - begin;
#endif
			else if (pt.x - dwBegin < (dwBegin + size.cx) - pt.x)
				return iter - begin;
			else
				return next - begin;
		}

		dwBegin += size.cx;
		iter = next;
	}
	return iter - begin;
}
void CWndText::OnChar(UINT nChar)
{
	if (nChar == 3) // CTRL-C : copy
	{
		if (m_dwBlockBegin != m_dwBlockEnd)
		{
			DWORD dwBlockBegin;
			DWORD dwBlockEnd;
			if (m_dwBlockBegin > m_dwBlockEnd)
				dwBlockBegin = m_dwBlockEnd, dwBlockEnd = m_dwBlockBegin;
			else
				dwBlockBegin = m_dwBlockBegin, dwBlockEnd = m_dwBlockEnd;
			CString strClipboard;
			strClipboard = m_string.Mid(dwBlockBegin, dwBlockEnd - dwBlockBegin);
			if (m_bEnableClipboard)
				CClipboard::SetText(strClipboard);
		}
	}
}
void CWndText::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_string.ClearStyle(ESSTY_BLOCK);
	LONG lOffset = GetOffset(point);
	if (lOffset != -1)
		m_dwBlockBegin = m_dwBlockEnd = m_dwOffset = lOffset;
	m_bLButtonDown = TRUE;
	SetCapture();

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}
}
void CWndText::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_string.ClearStyle(ESSTY_BLOCK);
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	CWndBase* pWnd = m_pParentWnd;
	pWnd->OnChildNotify(STN_CLICKED, m_nIdWnd, (LRESULT*)this);
}
void CWndText::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (g_pPlayer && g_pPlayer->HasBuff(BUFF_ITEM, II_SYS_SYS_SCR_FONTEDIT))
	{
		m_wndMenu.DeleteAllMenu();
		m_wndMenu.AppendMenu(0, MGI_APPELL_UP, prj.GetText(TID_GAME_FONT_EDIT));
		m_wndMenu.Move(CPoint(m_rectCurrentWindow.left, m_rectCurrentWindow.top) + point);
		m_wndMenu.SetVisible(TRUE);//!m_wndMenuMover.IsVisible() );
		m_wndMenu.SetFocus();
	}
}

BOOL CWndText::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	//	m_wndMenu.SetVisible( FALSE );
	return CWndBase::OnChildNotify(message, nID, pLResult);
}

BOOL CWndText::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	if (pWndBase == &m_wndMenu)
	{
		switch (nID)
		{
		case MGI_APPELL_UP:
			SAFE_DELETE(g_WndMng.m_pWndFontEdit);
			g_WndMng.m_pWndFontEdit = new CWndFontEdit;
			g_WndMng.m_pWndFontEdit->Initialize();
			g_WndMng.m_pWndFontEdit->m_pWndText = this;
			break;
		}
	}

	m_wndMenu.SetVisible(FALSE);

	return CWndBase::OnCommand(nID, dwMessage, pWndBase);
}
void CWndText::OnMouseMove(UINT nFlags, CPoint point)
{
	//return;
	if (m_bLButtonDown == TRUE)
	{
		m_string.ClearStyle(ESSTY_BLOCK);
		LONG lOffset = GetOffset(point);
		if (lOffset != -1)
			m_dwBlockEnd = m_dwOffset = lOffset;
	}
	else
	{
		int dwOffset;
		int dwBlockBegin;
		int dwBlockEnd;
		LONG lOffset = GetOffset(point);
		BYTE chr1, chr2;

		if (lOffset >= 0 && lOffset < m_string.GetLength())
		{
			if (!IsWhite(m_string.GetAt(lOffset)))
			{
				dwBlockBegin = lOffset;
				dwBlockEnd = lOffset;

				while (dwBlockBegin > 1)
				{
					chr1 = m_string.GetAt(dwBlockBegin - 1);
					chr2 = m_string.GetAt(dwBlockBegin - 2);

					if (IsDBCSLeadByte(chr1) && IsDBCSLeadByte(chr2))
						dwBlockBegin -= 2;
					else
						if (!IsWhite(chr1))
							dwBlockBegin -= 1;
						else
							break;
				}
				/*
				while(dwBlockEnd < m_string.GetLength() )
				{
					chr1 = m_string.GetAt ( dwBlockEnd );

					if( IsDBCSLeadByte( chr1 ) )
						dwBlockEnd += 2;
					else
					if( !IsWhite( chr1 ) )
						dwBlockEnd += 1;
					else
						break;
				}
				*/
				dwOffset = dwBlockEnd;

				CString strFind = m_string.Mid(dwBlockBegin, dwBlockEnd - dwBlockBegin);
				/*
				CStringArray* pStrArray = prj.GetHelp( strFind );
				if(pStrArray)
				{
					GET_CLIENT_POINT( m_pApp->GetSafeHwnd(), point);
					CPoint pt1 = OffsetToPoint(dwBlockBegin);
					CPoint pt2 = OffsetToPoint(dwBlockEnd);
					g_toolTip.PutToolTip( 100, pStrArray->GetAt(0), CRect
						(	pt1.x, pt1.y, pt2.x, pt2.y + m_pFont->GetMaxHeight() )
						, point);
				}
				*/
			}
		}
	}
}
void CWndText::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LONG lOffset = GetOffset(point);
	BYTE chr1;

	if (lOffset >= 0 && lOffset < m_string.GetLength())
	{
		if (!IsWhite(m_string.GetAt(lOffset)))
		{
			chr1 = 0;
			const char* begin = m_string;
			const char* end = begin + m_string.GetLength();
			const char* iter = begin;

			m_dwBlockBegin = 0;

			// Get m_dwBlockBegin
			while (*iter && iter < end) {

				if (IsWhite(*iter))
					m_dwBlockBegin = iter - begin;

				WORD wCodePage = m_string.m_awCodePage[iter - begin];

				iter = CharNextEx(iter, wCodePage);
				if (iter - begin > lOffset) break;
			}

			m_dwBlockEnd = m_dwBlockBegin;

			while (*iter && iter < end) {

				if (IsWhite(*iter)) {
					m_dwBlockEnd = iter - begin;
					break;
				}

				WORD wCodePage = m_string.m_awCodePage[iter - begin];

				iter = CharNextEx(iter, wCodePage);
				if (iter - begin > lOffset) break;
			}

			m_dwOffset = m_dwBlockEnd;
		}
	}
}
BOOL CWndText::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	if (m_wndScrollBar.GetScrollPage() >= m_wndScrollBar.GetMaxScrollPos())
		return TRUE;
#endif // __IMPROVE_QUEST_INTERFACE
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

void CWndText::OnSetFocus(CWndBase* pOldWnd)
{
	CWndBase::OnSetFocus(pOldWnd);

	if (g_WndMng.m_pWndFontEdit)
	{
		g_WndMng.m_pWndFontEdit->m_pWndText = this;
	}
}
void CWndText::OnKillFocus(CWndBase* pNewWnd)
{
	CWndBase::OnKillFocus(pNewWnd);
}
void CWndText::SetString(LPCTSTR pszString, DWORD dwColor)
{

	m_string.Init(m_pFont, &GetClientRect());

	m_string.SetParsingString(pszString, dwColor);
	if (IsWndStyle(WBS_VSCROLL))
		UpdateScrollBar();
	m_dwOffset = m_string.GetLength();

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}

	m_dwBlockBegin = m_dwBlockEnd = 0;
}
void CWndText::AddString(LPCTSTR pszString, DWORD dwColor, DWORD dwPStyle)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.AddParsingString(pszString, dwColor, 0x00000000, 0, dwPStyle);//+= pszString;
	//m_string.Align( m_pFont, nLine );
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}
}

void CWndText::Insert(int nIndex, LPCTSTR pstr)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.Insert(nIndex, pstr);
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}
}

void CWndText::BlockInsert(LPCTSTR pstr, LPCTSTR pstr2)
{
	DWORD dwBlockTemp;

	if (m_dwBlockBegin > m_dwBlockEnd)
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd = dwBlockTemp;
	}

	if (m_dwBlockBegin >= 0)
	{
		m_string.Insert(m_dwBlockBegin, pstr);

		m_dwBlockBegin += strlen(pstr);

		if ((int)(m_dwBlockEnd + strlen(pstr)) < m_string.GetLength())
			m_dwBlockEnd += strlen(pstr);
		else
			m_dwBlockEnd = m_string.GetLength();

		m_string.Insert(m_dwBlockEnd, pstr2);

		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();

		if (m_bCaretVisible)
		{
			CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
			SetCaretPos(ptCaret);
		}
	}
}

void CWndText::BlockDelete(LPCTSTR pstr, LPCTSTR pstr2)
{
	DWORD dwBlockTemp;

	if (m_dwBlockBegin > m_dwBlockEnd)
	{
		dwBlockTemp = m_dwBlockBegin;
		m_dwBlockBegin = m_dwBlockEnd;
		m_dwBlockEnd = dwBlockTemp;
	}

	if (m_dwBlockBegin >= 0)
	{
		int nColor = stricmp(pstr, "#c");

		TCHAR ch;
		int nCount = m_dwBlockEnd;
		int nIndex = 0;

		nIndex = m_string.Find(pstr2, nCount);

		if (nIndex != -1)
			Delete(nIndex, strlen(pstr2));
		/////////////////////////////////////

		nCount = m_dwBlockBegin;

		while (nCount--)
		{
			ch = m_string.GetAt(nCount);

			if (ch == '#')
			{
				nIndex = m_string.Find(pstr, nCount);

				if (nIndex != -1)
				{
					Delete(nIndex, strlen(pstr));

					if (nColor != -1)
					{
						Delete(nIndex, 8);
					}

					break;
				}
			}
		}

		m_dwBlockBegin -= strlen(pstr);
		m_dwBlockEnd -= strlen(pstr);

		if (nColor != -1)
		{
			m_dwBlockBegin -= 8;
			m_dwBlockEnd -= 8;
		}


		if ((int)m_dwBlockBegin < 0)
			m_dwBlockBegin = 0;

		if ((int)m_dwBlockEnd < 0)
			m_dwBlockEnd = 0;

		UpdateScrollBar();
		m_dwOffset = m_string.GetLength();

		if (m_bCaretVisible)
		{
			CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
			SetCaretPos(ptCaret);
		}
	}
}

void CWndText::Delete(int nIndex, int nLen)
{
	int nLine = m_string.GetLineCount() - 1;
	m_string.Delete(nIndex, nLen);
	UpdateScrollBar();
	m_dwOffset = m_string.GetLength();

	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}
}

void CWndText::ResetString()
{
	if (m_bCaretVisible)
	{
		CPoint ptCaret = OffsetToPoint(m_dwOffset, m_szCaret);
		SetCaretPos(ptCaret);
	}

	UpdateScrollBar();
}

//////////////////////



CWndMenu::CWndMenu()
{
	m_nLargeWidth = 0;
	m_byWndType = WTYPE_MENU;
}
CWndMenu::~CWndMenu()
{
	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
		SAFE_DELETE_CONST(GetMenuItem(i));
}
BOOL CWndMenu::CreateMenu(CWndBase* pWndParent)
{
	BOOL bResult = CWndBase::Create(WBS_POPUP/*|WBS_NOFRAME*/, CRect(0, 0, 0, 0), pWndParent, 10);
	SetVisible(FALSE);
	return bResult;
}
void CWndMenu::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(3, 3);
	m_rectLayout = m_rectClient;
	if (bOnSize && (rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height()))
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
	MakeVertexBuffer();
}
BOOL CWndMenu::CreatePopupMenu()
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenu(UINT nIDResource)
{
	return TRUE;
}
BOOL CWndMenu::LoadMenuIndirect(const void* lpMenuTemplate)
{
	return TRUE;
}
BOOL CWndMenu::DestroyMenu()
{
	return TRUE;
}
void CWndMenu::DeleteAllMenu()
{
	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
		SAFE_DELETE_CONST(GetMenuItem(i));
	m_awndMenuItem.RemoveAll();
}

BOOL CWndMenu::DeleteMenu(UINT nPosition, UINT nFlags)
{
	SAFE_DELETE_CONST(GetMenuItem(nPosition));
	m_awndMenuItem.RemoveAt(nPosition);
	for (int i = nPosition; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem = GetMenuItem(i);
		CRect rect = pWndMenuItem->GetWindowRect(TRUE);
		rect.OffsetRect(0, -22);
		pWndMenuItem->SetWndRect(rect);
	}
	CRect rect = GetWindowRect(TRUE);
	rect.OffsetRect(0, 22);
	rect.bottom -= 22;
	SetWndRect(rect);
	return TRUE;
}
CWndButton* CWndMenu::AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = new CWndButton;
	int nCount = m_awndMenuItem.GetSize();
	CSize size = m_pFont->GetTextExtent(lpszNewItem);
	if (size.cx + 60 > m_nLargeWidth)
		m_nLargeWidth = size.cx + 60;
	pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT/* | WBS_NOMENUICON*/, CRect(2, 2 + (nCount * 22), m_nLargeWidth, 2 + (nCount * 22) + 20), this, nIDNewItem);
	pWndButton->DelWndStyle(WBS_NODRAWFRAME);
	m_awndMenuItem.Add(pWndButton);
	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		pWndButton = (CWndButton*)m_awndMenuItem.GetAt(i);
		CRect rect = pWndButton->GetWindowRect(TRUE);
		rect.right = rect.left + m_nLargeWidth;
		pWndButton->SetWndRect(rect);
	}
	SetWndRect(CRect(0, 0, m_nLargeWidth, 5 + 5 + ((nCount + 1) * 22)));
	return pWndButton;
}
UINT CWndMenu::CheckMenuItem(UINT nIDCheckItem, UINT nCheck)
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt(nIDCheckItem);
	pWndButton->SetCheck(nCheck);
	return TRUE;
}
UINT CWndMenu::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
{
	return TRUE;
}
UINT CWndMenu::GetMenuItemCount() const
{
	return m_awndMenuItem.GetSize();
}
UINT CWndMenu::GetMenuItemID(int nPos) const
{
	return TRUE;
}
UINT CWndMenu::GetMenuState(UINT nID, UINT nFlags) const
{
	CWndButton* pWndButton = (CWndButton*)m_awndMenuItem.GetAt(nID);
	return pWndButton->GetCheck();//( nCheck );
	//return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount, UINT nFlags) const
{
	return TRUE;
}
int  CWndMenu::GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const
{
	return NULL;
}
CWndMenu* CWndMenu::GetSubMenu(int nPos) const
{
	return NULL;
}
BOOL CWndMenu::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	CWndButton* pWndButton = new CWndButton;
	pWndButton->Create(lpszNewItem, WBS_MENUITEM | WBS_HIGHLIGHT, CRect(5, 50 + 5 + (nPosition * 22), 175, 50 + 5 + (nPosition * 22) + 20), this, nIDNewItem);
	pWndButton->DelWndStyle(WBS_NODRAWFRAME);
	m_awndMenuItem.InsertAt(nPosition, pWndButton);
	for (int i = nPosition + 1; i < m_awndMenuItem.GetSize(); i++)
	{
		CWndButton* pWndMenuItem = GetMenuItem(i);
		CRect rect = pWndMenuItem->GetWindowRect(TRUE);
		rect.OffsetRect(0, 22);
		pWndMenuItem->SetWndRect(rect);
	}
	CRect rect = GetWindowRect(TRUE);
	rect.OffsetRect(0, -22);
	rect.bottom += 22;
	SetWndRect(rect);
	return TRUE;
}
BOOL CWndMenu::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	return TRUE;
}
BOOL CWndMenu::RemoveMenu(UINT nPosition, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags)
{
	return TRUE;
}
BOOL CWndMenu::SetDefaultItem(UINT uItem, BOOL fByPos)
{
	return TRUE;
}
UINT CWndMenu::GetDefaultItem(UINT gmdiFlags, BOOL fByPos)
{
	return TRUE;
}
CWndButton* CWndMenu::GetMenuItem(int nPos)
{
	return (CWndButton*)m_awndMenuItem.GetAt(nPos);
}
void CWndMenu::SetVisibleSub(BOOL bVisible)
{
	CWndBase::SetVisible(bVisible);
	for (int i = 0; i < m_awndMenuItem.GetSize(); i++)
	{
		if (GetMenuItem(i)->m_pWndMenu)
			GetMenuItem(i)->m_pWndMenu->SetVisibleSub(bVisible);
	}
}
void CWndMenu::SetVisibleAllMenu(BOOL bVisible)
{
	CWndMenu* pMenu = this;
	CWndMenu* pMenu2 = this;
	while (pMenu2 && pMenu2->IsWndStyle(WBS_POPUP))
	{
		pMenu = pMenu2;
		pMenu2 = (CWndMenu*)pMenu->GetParentWnd();
	}
	if (pMenu)
	{
		pMenu->CWndBase::SetVisible(bVisible);
		for (int i = 0; i < pMenu->m_awndMenuItem.GetSize(); i++)
		{
			if (pMenu->GetMenuItem(i)->m_pWndMenu)
				pMenu->GetMenuItem(i)->m_pWndMenu->SetVisibleSub(bVisible);
		}
	}
}
void CWndMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if (pNewWnd == NULL)
		SetVisibleAllMenu(FALSE);
	else
		if (pNewWnd->IsWndStyle(WBS_POPUP) == FALSE)
			SetVisibleAllMenu(FALSE);
}

void CWndMenu::OnDraw(C2DRender* p2DRender)
{

}
BOOL CWndMenu::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	BOOL bResult = FALSE;
	if (m_pParentWnd)
	{
		bResult = m_pParentWnd->OnChildNotify(message, nID, pLResult);
		m_pParentWnd->SetFocus();
	}

	return bResult;
}
BOOL CWndMenu::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
#if __VER >= 15 // __GUILD_HOUSE

	if (MMI_GHOUSE_INFO == nID)
	{
		CWndGuildHousing* pWnd = (CWndGuildHousing*)g_WndMng.GetApplet(APP_GH_FURNITURE_STORAGE);
		if (!pWnd)
		{
			g_WndMng.CreateApplet(APP_GH_FURNITURE_STORAGE);
			pWnd = (CWndGuildHousing*)g_WndMng.GetApplet(APP_GH_FURNITURE_STORAGE);
			assert(pWnd);

			pWnd->InitBySection(CWndGuildHousing::GS_FURNITURE);
		}

	}
	else if (MMI_GHOUSE_REINSTALL == nID)
	{
		GuildHouse->Reset();
		return TRUE;
	}
	else if (MMI_GHOUSE_RECALL == nID)
	{
		GuildHouse->Remove();
		return TRUE;
	}
#endif

	BOOL bResult = FALSE;
	if (m_pParentWnd)
	{
		bResult = m_pParentWnd->OnCommand(nID, dwMessage, this);
		if (bResult != -1)
			m_pParentWnd->SetFocus();
	}
	return bResult;
}
void CWndMenu::PaintFrame(C2DRender* p2DRender)
{
	p2DRender->RenderFillRect(GetWindowRect(), D3DCOLOR_ARGB(255, 226, 198, 181));

}
BOOL CWndMenu::OnEraseBkgnd(C2DRender* p2DRender)
{
	p2DRender->RenderFillRect(GetClientRect(), D3DCOLOR_ARGB(255, 255, 255, 255));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndListCtrl
//////////////////////////////////////////////////////////////////////////////////////////////////////

CWndListCtrl::CWndListCtrl()
{
	m_nWndColor = D3DCOLOR_TEMP(255, 0x5 << 3, 0x5 << 3, 0x5 << 3);
	m_nFontColor = D3DCOLOR_ARGB(255, 64, 64, 64);
	m_nSelectColor = D3DCOLOR_ARGB(255, 255, 0, 0);
	m_nCurSelect = -1;
	m_pFocusItem = NULL;
	m_byWndType = WTYPE_LISTCTRL;
	m_nLineSpace = 3;
}
CWndListCtrl::~CWndListCtrl()
{
	LVCOLUMN* pColumn;
	LVITEM* pItems;
	for (int i = 0; i < m_aColumns.GetSize(); i++)
	{
		pColumn = (LVCOLUMN*)m_aColumns.GetAt(i);
		SAFE_DELETE(pColumn->pszText);
		SAFE_DELETE(pColumn);
	}
	for (int i = 0; i < m_aItems.GetSize(); i++)
	{
		pItems = (LVITEM*)m_aItems.GetAt(i);
		for (int j = 0; j < m_aColumns.GetSize(); j++)
		{
			if (pItems[j].pszText)
				SAFE_DELETE(pItems[j].pszText);
		}
		SAFE_DELETE_ARRAY(pItems);
	}
	//	ResetContent();
}
void CWndListCtrl::Create(DWORD dwListCtrlStyle, RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndBase::Create(dwListCtrlStyle | WBS_CHILD, rect, pParentWnd, nID);
}
void CWndListCtrl::LoadListBoxScript(LPCTSTR lpFileName)
{
}
void CWndListCtrl::InterpriteScript(CScanner& scanner, CPtrArray& ptrArray)
{
}
void CWndListCtrl::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	//rect.right += 5;
	m_wndScrollBar.AddWndStyle(WBS_DOCKING);
	m_wndScrollBar.Create(WBS_VERT, rect, this, 1000);//,m_pSprPack,-1);
}


void CWndListCtrl::OnDraw(C2DRender* p2DRender)
{
	CPoint pt(3, 3);
	m_nFontHeight = GetFontHeight();



	//	if( IsWndStyle( WLVS_REPORT ) )
	{
		pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
		for (int i = 0; i < m_aItems.GetSize(); i++)
		{
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
			for (int i2 = 0, x = 0; i2 < m_aColumns.GetSize(); i2++)
			{
				if (i == m_nCurSelect)
					p2DRender->TextOut(x + 2, pt.y, pItems[i2].pszText, m_nSelectColor);
				else
					p2DRender->TextOut(x + 2, pt.y, pItems[i2].pszText, m_nFontColor);
				LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt(i2);
				x += pColumn->cx + 7;
			}
			pt.y += m_nFontHeight + 3;
		}

		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;

		if (IsWndStyle(WBS_VSCROLL))
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
		else
			m_wndScrollBar.SetVisible(FALSE);
	}


	// CCtrllist
	if (IsWndStyle(WLVS_ICON))
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for (int i = pt.y; i < m_aItems.GetSize(); i++)
		{
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
			if (i == m_nCurSelect)
				p2DRender->RenderFillRect(CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2), 0xff707070);
			else
				p2DRender->RenderFillRect(CRect(x * 32, y * 32, x * 32 + 32 - 2, y * 32 + 32 - 2), 0xffffffff);
			//p2DRender->TextOut( x * 32, y * 32, pItems->pszText, 0xff000000 );
		}

		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if (nRange - nPage <= 0)
			m_wndScrollBar.SetVisible(FALSE);
		else
		{
			m_wndScrollBar.SetVisible(TRUE);
			m_wndScrollBar.SetScrollRange(0, nRange);
			m_wndScrollBar.SetScrollPage(nPage);
		}
	}
}
int CWndListCtrl::GetCurSel() const
{
	return m_nCurSelect;
}
int CWndListCtrl::SetCurSel(int nSelect)
{
	m_pFocusItem = (LVITEM*)m_aItems.GetAt(nSelect);
	m_nCurSelect = nSelect;
	return m_nCurSelect;
}
CString CWndListCtrl::GetItemText(int nItem, int nSubItem) const
{
	LVITEM* pItem = (LVITEM*)m_aItems.GetAt(nItem);
	return pItem[nSubItem].pszText;
}
void CWndListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);
	CRect rect;

	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for (int i = 0; i < m_aItems.GetSize(); i++)
	{
		LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			if (m_pFocusItem == pItems)
			{

				CWndBase* pWnd = m_pParentWnd;
				pWnd->OnChildNotify(WNM_SELCHANGE, m_nIdWnd, (LRESULT*)m_pFocusItem);
				return;
			}
		}
		pt.y += m_nFontHeight + 3;
	}

	// CCtrllist
	if (IsWndStyle(WLVS_ICON))
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for (int i = pt.y; i < m_aItems.GetSize(); i++)
		{
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
			rect.SetRect(x * 32, y * 32, x * 32 + 32, y * 32 + 32);
			if (rect.PtInRect(point))
			{
				if (m_pFocusItem == pItems)
				{

					CWndBase* pWnd = m_pParentWnd;
					pWnd->OnChildNotify(WNM_SELCHANGE, m_nIdWnd, (LRESULT*)m_pFocusItem);
					return;
				}
			}
		}
	}
}
void CWndListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(3, 3);
	CRect rect;

	pt.y -= (m_nFontHeight + 3) * m_wndScrollBar.GetScrollPos();
	for (int i = 0; i < m_aItems.GetSize(); i++)
	{
		LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
		rect.SetRect(pt.x, pt.y, pt.x + m_rectWindow.Width() - m_wndScrollBar.GetClientRect().Width(), pt.y + m_nFontHeight);
		if (rect.PtInRect(point))
		{
			m_nCurSelect = i;
			m_pFocusItem = pItems;
			break;
		}
		pt.y += m_nFontHeight + 3;
	}

	if (IsWndStyle(WLVS_ICON))
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		pt.y = 0;
		pt.y += m_wndScrollBar.GetScrollPos() * nWidth;
		for (int i = pt.y; i < m_aItems.GetSize(); i++)
		{
			int x = (i - pt.y) % nWidth;
			int y = (i - pt.y) / nWidth;
			LVITEM* pItems = (LVITEM*)m_aItems.GetAt(i);
			rect.SetRect(x * 32, y * 32, x * 32 + 32, y * 32 + 32);
			if (rect.PtInRect(point))
			{
				m_nCurSelect = i;
				m_pFocusItem = pItems;
				break;
			}
		}
	}
}
void CWndListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
}
void CWndListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetWindowRect();

	if (IsWndStyle(WBS_VSCROLL))
		rect.left = rect.right - VSCROLL_WIDTH;
	m_wndScrollBar.SetVisible(IsWndStyle(WBS_VSCROLL));
	m_wndScrollBar.SetWndRect(rect);

	CWndBase::OnSize(nType, cx, cy);
}
void CWndListCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(4, 4);

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		int nPage = GETRANGE;//GetClientRect().Height() / (m_nFontHeight + 3);
		int nRange = m_aItems.GetSize();// - nPage;
		if (!(nRange - nPage <= 0))
			m_rectClient.right -= 15;
		m_rectClient.top += 15;
	}
	if (IsWndStyle(WLVS_ICON))
	{
		CRect rect = GetClientRect();
		int nWidth = rect.Width() / 32;
		int nHeight = rect.Height() / 32;
		int nPage = nHeight;
		int nRange = m_aItems.GetSize() / nWidth;// - nPage;
		if (!(nRange - nPage <= 0))
			m_rectClient.right -= 15;
	}
	if (bOnSize)
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
}
void CWndListCtrl::PaintFrame(C2DRender* p2DRender)
{
	CRect rect = GetWindowRect();
	m_pTheme->RenderWndTextFrame(p2DRender, &rect);

	//if( IsWndStyle( WLVS_REPORT ) )
	{
		int i, x = NULL;
		for (; i < m_aColumns.GetSize(); i++)
		{
			LVCOLUMN* pColumn = (LVCOLUMN*)m_aColumns.GetAt(i);
			p2DRender->TextOut(x + 4, 4, pColumn->pszText);
			m_pTheme->GradationRect(p2DRender, &CRect(x, 0, x + pColumn->cx + 5, 20),
				D3DCOLOR_ARGB(50, 100, 100, 100),
				D3DCOLOR_ARGB(90, 255, 255, 255),
				D3DCOLOR_ARGB(50, 000, 000, 000), 20);
			x += pColumn->cx + 6;
		}
		m_pTheme->GradationRect(p2DRender, &CRect(x, 0, rect.Width(), 20),
			D3DCOLOR_ARGB(50, 100, 100, 100),
			D3DCOLOR_ARGB(90, 255, 255, 255),
			D3DCOLOR_ARGB(50, 000, 000, 000), 20);
	}
}
/*
typedef struct tagLVCOLUMNA
{
	UINT mask;
	int fmt;
	int cx;
	LPSTR pszText;
	int cchTextMax;
	int iSubItem;
#if (_WIN32_IE >= 0x0300)
	int iImage;
	int iOrder;
#endif
} LVCOLUMNA, FAR* LPLVCOLUMNA;
typedef struct tagLVITEMA
{
	UINT mask;
	int iItem;
	int iSubItem;
	UINT state;
	UINT stateMask;
	LPSTR pszText;
	int cchTextMax;
	int iImage;
	LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
	int iIndent;
#endif
} LVITEMA, FAR* LPLVITEMA;
*/
BOOL CWndListCtrl::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}
BOOL CWndListCtrl::SetItem(const LVITEM* pItem)
{
	if (pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt(pItem->iItem) == NULL)
		return FALSE;
	LVITEM* pItems = (LVITEM*)m_aItems.GetAt(pItem->iItem);
	if (pItems)
	{
		if (pItems[pItem->iSubItem].pszText)
			SAFE_DELETE(pItems[pItem->iSubItem].pszText);
		memcpy(&pItems[pItem->iSubItem], pItem, sizeof(LVITEM));
		pItems[pItem->iSubItem].pszText = new _TCHAR[_tcslen(pItem->pszText) + sizeof(_TCHAR)];
		_tcscpy(pItems[pItem->iSubItem].pszText, pItem->pszText);
	}
	return TRUE;
}
int CWndListCtrl::InsertItem(const LVITEM* pItem)
{
	//	if( pItem->iItem < m_aItems.GetSize() && m_aItems.GetAt( pItem->iItem ) )
	if (pItem->iItem > m_aItems.GetSize())
		return -1;
	LVITEM* pNewItems = new LVITEM[m_aColumns.GetSize()];
	ZeroMemory(pNewItems, sizeof(LVITEM) * m_aColumns.GetSize());
	memcpy(&pNewItems[pItem->iSubItem], pItem, sizeof(LVITEM));
	pNewItems[pItem->iSubItem].pszText = new _TCHAR[_tcslen(pItem->pszText) + sizeof(_TCHAR)];
	_tcscpy(pNewItems[pItem->iSubItem].pszText, pItem->pszText);
	m_aItems.SetAtGrow(pItem->iItem, (void*)pNewItems);
	return pItem->iItem;
}
int CWndListCtrl::GetItemCount() const
{
	return m_aItems.GetSize();
}
int CWndListCtrl::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	LVCOLUMN* pNewColumn = new LVCOLUMN;
	memcpy(pNewColumn, pColumn, sizeof(LVCOLUMN));
	pNewColumn->pszText = new _TCHAR[_tcslen(pColumn->pszText) + sizeof(_TCHAR)];
	_tcscpy(pNewColumn->pszText, pColumn->pszText);
	if (nCol < m_aColumns.GetSize() && m_aColumns.GetAt(nCol))
		return -1;
	m_aColumns.SetAtGrow(nCol, (void*)pNewColumn);

	/*
	CPtrArray m_aWndColumns;
	CRect rect;// = GetWindowRect();
	CWndButton* pWndButton = new CWndButton;
	pWndButton->AddWndStyle( WBS_DOCKING );
	rect = CRect( 0, 0, pColumn->cx, 20 );
	if( nCol )
	{
		LVCOLUMN* pNewColumn = (LVCOLUMN*)m_aColumns.GetAt( nCol - 1 );
		if(
		i
	}
	pWndButton->Create( pColumn->pszText, 0, rect , this, nCol );
	m_aWndColumns.SetAtGrow( nCol, pWndButton );
	*/
	return nCol;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
tagWTCITEM::tagWTCITEM(void) :
	mask(0),
	dwState(0),
	dwStateMask(0),
	pszText(_T("")),
	cchTextMax(0),
	iImage(0),
	lParam(0),
	pWndBase(NULL)
{
}
#endif // __IMPROVE_QUEST_INTERFACE
CWndTabCtrl::CWndTabCtrl()
{
	m_nTabButtonLength = 0;
	m_nCurSelect = 0;
	m_aTab.reserve(10);
	m_eTabTitleAlign = ALIGN_CENTER;
}
CWndTabCtrl::~CWndTabCtrl()
{
	vector< LPWTCITEM >::iterator itor;
	for (itor = m_aTab.begin(); itor != m_aTab.end(); itor++)
		SAFE_DELETE(*itor);

	DeleteDeviceObjects();
}
BOOL CWndTabCtrl::Create(DWORD dwStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	return CWndBase::Create(dwStyle | WBS_CHILD | WBS_NOFRAME /*| WBS_NODRAWFRAME*/, rect, pParentWnd, nID);//,pSprPack,nSprIdx,nColorTable);
}
HRESULT CWndTabCtrl::InitDeviceObjects()
{
	return CWndBase::InitDeviceObjects();
}
HRESULT CWndTabCtrl::RestoreDeviceObjects()
{
	return CWndBase::RestoreDeviceObjects();
}
HRESULT CWndTabCtrl::InvalidateDeviceObjects()
{
	return CWndBase::InvalidateDeviceObjects();
}
HRESULT CWndTabCtrl::DeleteDeviceObjects()
{
	return CWndBase::DeleteDeviceObjects();
}
void CWndTabCtrl::PaintFrame(C2DRender* p2DRender)
{

}
int CWndTabCtrl::GetRenderMethod()
{
	// -1 = no render
	// 0 = render only bottom side
	// 1 = render everything

	int nWndId = GetParentWnd()->GetWndId();
	switch (nWndId)
	{
	case APP_CHEST: return -1;
	case APP_INVENTORY: return -1;
	case APP_GUILD_BANK: return -1;
	case APP_COMMON_BANK: return -1;
	case APP_SHOPEX: return 0;
	default: return 1;
	}

	return 1;
}
void CWndTabCtrl::OnDraw(C2DRender* p2DRender)
{
	int nRenderMethod = GetRenderMethod();
	if (nRenderMethod == -1)
		return;

	int nTabSize = m_aTab.size();

	if (m_nCurSelect >= 0)
	{
		CRect rect = GetWindowRect();
		rect.right -= 15;
		int nHeight = (rect.Height() / EDIT_HEIGHT) - 2;
		rect.top += (nHeight * EDIT_HEIGHT);

		if (nTabSize == 0)
			nTabSize = 1;
		int nWidth = rect.Width() / 8;
		int nSize = (nWidth / nTabSize);
		int nCurPos = nSize * m_nCurSelect;

		static const int TEXTURE_TAB_SIZE_X = m_aTexture[1].m_size.cx;
		static const int TEXTURE_TAB_SIZE_Y = m_aTexture[1].m_size.cy;

		CPoint pointStart = rect.TopLeft();
		pointStart.x += (TEXTURE_TAB_SIZE_X * nCurPos);

		int nFirstTabTextureIndex = (m_nCurSelect == 0) ? 0 : 1;
		m_aTexture[nFirstTabTextureIndex].Render(p2DRender, pointStart, CWndBase::m_nAlpha);
		m_aTexture[1].Render(p2DRender,
			CPoint(pointStart.x + TEXTURE_TAB_SIZE_X, pointStart.y),
			CPoint(TEXTURE_TAB_SIZE_X, TEXTURE_TAB_SIZE_Y),
			CWndBase::m_nAlpha,
			static_cast<FLOAT>(nSize - 3));
		m_aTexture[2].Render(p2DRender, CPoint(pointStart.x + (TEXTURE_TAB_SIZE_X * (nSize - 2)), pointStart.y), CWndBase::m_nAlpha);
		if (m_nCurSelect == nTabSize - 1)
			m_aTexture[5].Render(p2DRender, CPoint(pointStart.x + (TEXTURE_TAB_SIZE_X * (nSize - 1)), pointStart.y), CWndBase::m_nAlpha);
		else
		{
			m_aTexture[3].Render(p2DRender, CPoint(pointStart.x + (TEXTURE_TAB_SIZE_X * (nSize - 1)), pointStart.y), CWndBase::m_nAlpha);
			m_aTexture[4].Render(p2DRender, CPoint(pointStart.x + (TEXTURE_TAB_SIZE_X * (nSize)), pointStart.y), CWndBase::m_nAlpha);
		}
	}

	CRect rectClient = GetWindowRect();
	rectClient.right -= 15;
	int y = rectClient.bottom - 17;

	if (nTabSize > 0)
	{
		int nLength = rectClient.Width() / nTabSize;
		for (int i = 0; i < nTabSize; ++i)
		{
			LPWTCITEM pItem = m_aTab[i];
			if (pItem)
			{
				switch (m_eTabTitleAlign)
				{
				case ALIGN_LEFT:
				{
					DWORD dwTextColor = (i != m_nCurSelect) ? 0xffffffff : 0xff000000;
					p2DRender->TextOut((i * nLength) + 2, y, pItem->pszText, dwTextColor);
					break;
				}
				case ALIGN_RIGHT:
				{
					DWORD dwTextColor = (i != m_nCurSelect) ? 0xffffffff : 0xff000000;
					CSize size = p2DRender->m_pFont->GetTextExtent(pItem->pszText);
					if (size.cx < nLength)
					{
						int nRenderingPositionX = nLength - size.cx;
						p2DRender->TextOut((i * nLength) + nRenderingPositionX, y, pItem->pszText, dwTextColor);
					}
					else
						p2DRender->TextOut((i * nLength) + 2, y, pItem->pszText, dwTextColor);
					break;
				}
				case ALIGN_CENTER:
				{
					DWORD dwTextColor = (i != m_nCurSelect) ? 0xffffffff : 0xff000000;
					CSize size = p2DRender->m_pFont->GetTextExtent(pItem->pszText);
					if (size.cx < nLength)
					{
						int nRenderingPositionX = (nLength - size.cx) / 2;
						p2DRender->TextOut((i * nLength) + nRenderingPositionX, y, pItem->pszText, dwTextColor);
					}
					else
						p2DRender->TextOut((i * nLength) + 2, y, pItem->pszText, dwTextColor);
					break;
				}
				default:
				{
					DWORD dwTextColor = (i != m_nCurSelect) ? 0xffffffff : 0xff000000;
					p2DRender->TextOut((i * nLength) + 2, y, pItem->pszText, dwTextColor);
				}
				}
			}
		}
	}
}
void CWndTabCtrl::OnInitialUpdate()
{
	m_aTexture[0].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile10.bmp"), 0xffff00ff, TRUE);
	m_aTexture[1].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile11.bmp"), 0xffff00ff, TRUE);
	m_aTexture[2].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile12.bmp"), 0xffff00ff, TRUE);
	m_aTexture[3].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile13.bmp"), 0xffff00ff, TRUE);
	m_aTexture[4].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile14.bmp"), 0xffff00ff, TRUE);
	m_aTexture[5].LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "WndTabTile15.bmp"), 0xffff00ff, TRUE);
}

void CWndTabCtrl::AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat)
{
	int nRenderMethod = GetRenderMethod();
	if (nRenderMethod == -1)
		return;

	/////////////////////////////////////////////////
	CRect rect = GetWindowRect(TRUE);
	rect += (m_pParentWnd->GetClientRect(TRUE).TopLeft() - m_pParentWnd->GetWindowRect(TRUE).TopLeft());
	CRect rect2 = rect;
	//ect = GetClientRect();
	rect.right -= 15;
	int nWidth = rect.Width() / 8;
	int nHeight = (rect.Height() / EDIT_HEIGHT);// - 2;
	CPoint point;

	int nImgBit = IMGBIT_32;
	CString m_strTile = "WndEditTile06.tga";
	CString strFileName;
	LPIMAGE lpImage[20];

	if (nRenderMethod != 0)
	{
		for (int i = 0; i < 9; i++)
		{
			CString strTemp1 = m_strTile.Left(m_strTile.GetLength() - 6);
			CString strTemp2 = m_strTile.Right(4);
			strFileName.Format("%s%02d%s", strTemp1, i, strTemp2);
			if (m_strWndTileMap.Lookup(strFileName, (void*&)lpImage[i]) == FALSE)
			{
				lpImage[i] = new IMAGE;

				if (LoadImage(MakePath(DIR_THEME, strFileName), lpImage[i]) == FALSE)
					Error("CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName);

				m_strWndTileMap.SetAt(strFileName, lpImage[i]);
			}
		}

		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				point = CPoint(j * 8, i * EDIT_HEIGHT) + rect.TopLeft();
				if (i == 0)
				{
					if (j == 0)
						PaintTexture(pDest, lpImage[0], point, sizeSurface, d3dFormat);
					else if (j == nWidth - 1)
						PaintTexture(pDest, lpImage[2], point, sizeSurface, d3dFormat);
					else
						PaintTexture(pDest, lpImage[1], point, sizeSurface, d3dFormat);
				}
				else if (i == nHeight - 1)
				{
					if (j == 0)
						PaintTexture(pDest, lpImage[6], point, sizeSurface, d3dFormat);
					else if (j == nWidth - 1)
						PaintTexture(pDest, lpImage[8], point, sizeSurface, d3dFormat);
					else
						PaintTexture(pDest, lpImage[7], point, sizeSurface, d3dFormat);
				}
				else
				{
					if (j == 0)
						PaintTexture(pDest, lpImage[3], point, sizeSurface, d3dFormat);
					else if (j == nWidth - 1)
						PaintTexture(pDest, lpImage[5], point, sizeSurface, d3dFormat);
					else
						PaintTexture(pDest, lpImage[4], point, sizeSurface, d3dFormat);
				}
			}
		}
	}

	m_strTile = "WndTabTile00.bmp";
	nImgBit = IMGBIT_32;
	for (int i = 0; i < 6; i++)
	{
		CString strTemp1 = m_strTile.Left(m_strTile.GetLength() - 6);
		CString strTemp2 = m_strTile.Right(4);
		strFileName.Format("%s%02d%s", strTemp1, i, strTemp2);
		if (m_strWndTileMap.Lookup(strFileName, (void*&)lpImage[i]) == FALSE)
		{
			lpImage[i] = new IMAGE;
			if (LoadImage(MakePath(DIR_THEME, strFileName), lpImage[i]) == FALSE)
				Error("CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName);

			m_strWndTileMap.SetAt(strFileName, lpImage[i]);
		}
	}

	for (int i = 10; i < 16; i++)
	{
		CString strTemp1 = m_strTile.Left(m_strTile.GetLength() - 6);
		CString strTemp2 = m_strTile.Right(4);
		strFileName.Format("%s%02d%s", strTemp1, i, strTemp2);
		if (m_strWndTileMap.Lookup(strFileName, (void*&)lpImage[i]) == FALSE)
		{
			lpImage[i] = new IMAGE;
			if (LoadImage(MakePath(DIR_THEME, strFileName), lpImage[i]) == FALSE)
				Error("CWndTabCtrl::AdditionalSkinTexture에서 %s Open1 실패", strFileName);

			m_strWndTileMap.SetAt(strFileName, lpImage[i]);
		}
	}
	/////////////////////////////////////////////////
	nHeight -= 2;
	CRect rectTab = GetWindowRect(TRUE);
	rectTab += (m_pParentWnd->GetClientRect(TRUE).TopLeft() - m_pParentWnd->GetWindowRect(TRUE).TopLeft());
	rectTab.top = rectTab.top + nHeight * EDIT_HEIGHT;//rect.Height() / 8 rect.bottom - 20;
	//ect = GetClientRect();
	rectTab.right -= 15;
	nWidth = rectTab.Width() / 8;
	//nHeight = rect.Height() / 10;// ) - 2;
	int nTapSize = m_aTab.size();
	if (nTapSize == 0)
		nTapSize = 1;
	int nSize = (nWidth) / nTapSize;//m_aTab.size() ;
	int nCurSize = 0;//e;
	for (int i = 0; i < nWidth; i++)
	{
		point = CPoint(i * 8, 0) + rectTab.TopLeft();
		if (i == nCurSize - 2)
		{
			PaintTexture(pDest, lpImage[2], point, sizeSurface, d3dFormat);
			i++;
			point = CPoint(i * 8, 0) + rectTab.TopLeft();
			if (i == nWidth - 1)
				PaintTexture(pDest, lpImage[5], point, sizeSurface, d3dFormat);
			else
				PaintTexture(pDest, lpImage[3], point, sizeSurface, d3dFormat);
		}
		else if (i == nCurSize)
		{
			if (i == 0)
				PaintTexture(pDest, lpImage[0], point, sizeSurface, d3dFormat);
			else
			{
				PaintTexture(pDest, lpImage[4], point, sizeSurface, d3dFormat);
			}
			nCurSize += nSize;
		}
		else
			PaintTexture(pDest, lpImage[1], point, sizeSurface, d3dFormat);
	}
	for (int i = 0; i < m_wndArray.GetSize(); i++)
	{
		CWndBase* pWndBase = (CWndBase*)m_wndArray.GetAt(i);
		CRect rectOld = m_rectWindow;
		m_rectWindow.OffsetRect(rect.TopLeft());
		if (pWndBase->IsDestroy() == FALSE && pWndBase->IsVisible())
		{
			pWndBase->AdditionalSkinTexture(pDest, sizeSurface, d3dFormat);
			for (int i = 0; i < pWndBase->m_wndArray.GetSize(); i++)
			{
				CWndBase* pWndChild = (CWndBase*)pWndBase->m_wndArray.GetAt(i);
				CRect rectOldChild = pWndChild->m_rectWindow;
				pWndChild->m_rectWindow.OffsetRect(rect.TopLeft());
				if (pWndChild->IsDestroy() == FALSE && pWndChild->IsVisible())
					pWndChild->AdditionalSkinTexture(pDest, sizeSurface, d3dFormat);
				pWndChild->m_rectWindow = rectOldChild;
			}
			pWndBase->AfterSkinTexture(pDest, sizeSurface);
		}
		m_rectWindow = rectOld;
	}
}


void CWndTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_aTab.size() == 0) return;

	CRect rectClient = GetClientRect();
	CRect rect;
	CRect rect2 = rectClient;
	rect2.bottom -= 18;
	int y = rectClient.bottom - 18;

	LPWTCITEM pItem;
	int nSelect = -1;
	if (m_aTab.size())
	{
		for (int i = 0; i < (int)(m_aTab.size()); i++)
		{
			pItem = m_aTab[i];
			if (pItem)
			{
				rect.SetRect(i * m_nTabButtonLength, y, m_nTabButtonLength + i * m_nTabButtonLength, rectClient.bottom);
				if (rect.PtInRect(point))
					nSelect = i;
			}
		}
	}
	if (nSelect != -1)
	{
		SetCurSel(nSelect);
		CWndBase* pParent = (CWndBase*)GetParentWnd();
		pParent->OnChildNotify(WNM_SELCHANGE, GetWndId(), (LRESULT*)this);
	}
}
BOOL CWndTabCtrl::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndBase* pParent = (CWndBase*)GetParentWnd();
	return pParent->OnChildNotify(message, nID, pLResult);
}

int CWndTabCtrl::GetCurSel() const
{
	return m_nCurSelect;
}
int CWndTabCtrl::SetCurSel(int nItem)
{
	int nOldSelect = m_nCurSelect;
	LPWTCITEM pItem;
	CRect rect;
	m_nCurSelect = nItem;
	if (m_aTab[m_nCurSelect]->pWndBase)
	{
		pItem = m_aTab[nOldSelect];
		pItem->pWndBase->SetVisible(FALSE);

		if (m_nCurSelect >= (int)(m_aTab.size()))
		{
			CString string;
			string.Format("CWndTabCtrl::SetCurSel에서 nItem이 범위를 넘어섬 : nItem(%d) m_aTabSize(%d)", m_nCurSelect, m_aTab.size());
			//ADDERRORMSG( string );
		}

		pItem = m_aTab[m_nCurSelect];
		pItem->pWndBase->SetVisible(TRUE);
		rect = GetWindowRect(TRUE);
		if (IsWndStyle(WBS_VSCROLL))
			rect.right -= 15;
		rect.bottom -= 18;
		for (int i = 0; i < (int)(m_aTab.size()); i++)
		{
			LPWTCITEM pItem = m_aTab[i];
			if (pItem && pItem->pWndBase)
			{
				rect.OffsetRect(-rect.TopLeft());
				pItem->pWndBase->SetWndRect(rect);
			}
		}
		GetFrameWnd()->AdjustWndBase();
		m_aTab[m_nCurSelect]->pWndBase->SetFocus();
	}
	else
		m_nCurSelect = nOldSelect;
	return nItem;
}
BOOL CWndTabCtrl::GetItem(int nItem, WTCITEM* pTabCtrlItem) const
{
	if (nItem >= (int)(m_aTab.size()))
		return FALSE;
	memcpy(pTabCtrlItem, m_aTab[nItem], sizeof(WTCITEM));
	return TRUE;
}
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
LPWTCITEM CWndTabCtrl::GetTabItem(int nItemNumber) const
{
	if (nItemNumber < 0 || nItemNumber >= (int)(m_aTab.size()))
		return NULL;
	return m_aTab[nItemNumber];
}

LPWTCITEM CWndTabCtrl::GetSelectedTab(void) const
{
	return m_aTab[m_nCurSelect];
}

void CWndTabCtrl::SetTabTitleAlign(const CWndTabCtrl::TabTitleAlign eTabTitleAlign)
{
	m_eTabTitleAlign = eTabTitleAlign;
}

const CWndTabCtrl::TabTitleAlign CWndTabCtrl::GetTabTitleAlign(void) const
{
	return m_eTabTitleAlign;
}
#endif // __IMPROVE_QUEST_INTERFACE
BOOL CWndTabCtrl::InsertItem(int nItem, WTCITEM* pTabCtrlItem)
{
	if (nItem >= (int)(m_aTab.size()))
		m_aTab.resize(nItem + 1);//serve( nItem + 1 );
	LPWTCITEM pItem = new WTCITEM;
	memcpy(pItem, pTabCtrlItem, sizeof(WTCITEM));
	m_aTab[nItem] = pItem;

	if (pTabCtrlItem->pWndBase)
	{
		if (nItem == m_nCurSelect)
			pTabCtrlItem->pWndBase->SetVisible(TRUE);
		else
			pTabCtrlItem->pWndBase->SetVisible(FALSE);
		CRect rect = GetClientRect(TRUE);
		rect.bottom -= 18;
		rect.OffsetRect(-rect.TopLeft());
		if (IsWndStyle(WBS_VSCROLL))
			rect.right -= 15;

		pItem->pWndBase->SetWndRect(rect);
		pItem->pWndBase->AddWndStyle(WBS_CHILDFRAME);

		if (m_aTab[m_nCurSelect])
			m_aTab[m_nCurSelect]->pWndBase->SetFocus();
	}
	m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();// - 10;


	return TRUE;
}
BOOL CWndTabCtrl::InsertTexture(int nItem, LPCTSTR lpszFileName)
{
	return TRUE;
}
void CWndTabCtrl::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndBase::SetWndRect(rectWnd, bOnSize);
}
BOOL CWndTabCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	return TRUE;
}

void CWndTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CRect rect = GetClientRect(TRUE);
	rect.bottom -= 18;

	for (int i = 0; i < (int)(m_aTab.size()); i++)
	{
		LPWTCITEM pItem = m_aTab[i];
		if (pItem && pItem->pWndBase)
		{
			rect.OffsetRect(-rect.TopLeft());
			pItem->pWndBase->SetWndRect(rect);
		}
	}
	if (m_aTab.size())
		m_nTabButtonLength = GetClientRect().Width() / m_aTab.size();// - 10;

	CWndBase::OnSize(nType, cx, cy);
}

void CWndComboListBox::PaintFrame(C2DRender* p2DRender)
{
	m_pTheme->RenderWndEditFrame(p2DRender, &GetWindowRect());
}

void CWndComboListBox::OnKillFocus(CWndBase* pNewWnd)
{
	m_pParentWnd->OnChildNotify(WNM_KILLFOCUS, m_nIdWnd, (LRESULT*)this);

	CWndBase::OnKillFocus(pNewWnd);
}

CWndComboBox::CWndComboBox()
{
	m_byWndType = WTYPE_COMBOBOX;
	m_bOpen = FALSE;
}
CWndComboBox::~CWndComboBox()
{
}
void CWndComboBox::OnInitialUpdate()
{
	CRect rect = GetWindowRect();
	rect.left = rect.right - 20;
	m_wndButton.AddWndStyle(WBS_DOCKING);
	m_wndButton.Create(_T("V"), WBS_CHILD, rect, this, 0);
	m_wndButton.m_pTexture = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "ButtQuickListDn.tga"), 0xffff00ff, TRUE);
	rect = GetWindowRect();
	ClientToScreen(&rect);
	rect.top = rect.bottom;
	rect.bottom += 100;
	m_wndListBox.CWndBase::Create(WBS_POPUP | WBS_VSCROLL, rect, this, 1);
	m_wndListBox.m_strTitle = _T("Combo ListBox");
	m_wndListBox.m_strTexture = m_strTexture;
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.RestoreDeviceObjects();
	m_wndListBox.SetVisible(FALSE);

}

void CWndComboBox::Create(DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID)
{
	CWndEdit::Create(pParentWnd->m_pApp->GetSafeHwnd(), dwComboBoxStyle | WBS_CHILD, rect, pParentWnd, nID);
}
void CWndComboBox::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	CWndEdit::SetWndRect(rectWnd, bOnSize);
}

void CWndComboBox::PaintFrame(C2DRender* p2DRender)
{
	m_pTheme->RenderWndEditFrame(p2DRender, &GetWindowRect());
}

void CWndComboBox::OnDraw(C2DRender* p2DRender)
{
	CWndEdit::OnDraw(p2DRender);
}

void CWndComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bOpen == TRUE)
		m_bOpen = FALSE;
	else
	{
		m_bOpen = TRUE;
		OpenListBox();
	}
}

void CWndComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
}
BOOL CWndComboBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == 0 && message == WNM_CLICKED) // list
	{
		if (m_bOpen == TRUE)
			m_bOpen = FALSE;
		else
		{
			m_bOpen = TRUE;
			OpenListBox();
		}
	}
	if (nID == 1 && message == WNM_SELCHANGE) // list
	{
		if (m_bOpen == TRUE)
		{
			SetSelectedItemInformation();
			SetFocus();
			m_wndListBox.SetVisible(FALSE);
			m_bOpen = FALSE;
		}
	}
	if (nID == 1 && message == WNM_KILLFOCUS)
	{
		CPoint point = GetMousePoint();
		CRect rtComboBox = GetClientRect();
		CRect rtButton = m_wndButton.GetClientRect();
		if (rtComboBox.PtInRect(point) == TRUE || rtButton.PtInRect(point) == TRUE)
			m_bOpen = TRUE;
		else
		{
			if (m_bOpen == TRUE)
				m_bOpen = FALSE;
		}
	}
	return TRUE;
}
// manipulating listbox items
int CWndComboBox::AddString(LPCTSTR lpszString)
{
	int nNum = m_wndListBox.AddString(lpszString);
	CRect rect = m_wndListBox.GetWindowRect(TRUE);

	if (nNum > 10)
		nNum = 10;

	rect.bottom = rect.top + ((nNum + 1) * (m_pFont->GetMaxHeight() + 3)) + 8;
	m_wndListBox.SetWndRect(rect);
	return nNum;
}
int CWndComboBox::DeleteString(UINT nIndex)
{
	return m_wndListBox.DeleteString(nIndex);
}
int CWndComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	return 1;
}
void CWndComboBox::ResetContent()
{
	SetString(_T(""));
	m_wndListBox.ResetContent();
}

void CWndComboBox::SelectItem(const CString& strItem)
{
	int nIndex = m_wndListBox.GetItemIndex(strItem);
	if (nIndex == -1)
	{
		return;
	}

	m_wndListBox.SetCurSel(nIndex);
	SetSelectedItemInformation();
}
void CWndComboBox::SelectItem(DWORD dwItem)
{
	int nIndex = m_wndListBox.GetItemIndex(dwItem);
	if (nIndex == -1)
	{
		return;
	}

	m_wndListBox.SetCurSel(nIndex);
	SetSelectedItemInformation();
}
void CWndComboBox::SetSelectedItemInformation(void)
{
	CString strSelectedItem = _T("");
	m_wndListBox.GetText(m_wndListBox.GetCurSel(), strSelectedItem);
	SetString(strSelectedItem);
	CWndBase* pWnd = m_pParentWnd;
	if (pWnd)
	{
		pWnd->OnChildNotify(WNM_SELCHANGE, m_nIdWnd, (LRESULT*)this);
	}
}

DWORD CWndComboBox::GetItemData(int nIndex) const
{
	return m_wndListBox.GetItemData(nIndex);
}
int CWndComboBox::SetItemData(int nIndex, DWORD dwItemData)
{
	return m_wndListBox.SetItemData(nIndex, dwItemData);
}
int CWndComboBox::GetCurSel() const
{
	return m_wndListBox.GetCurSel();
}
int CWndComboBox::SetCurSel(int nSelect)
{

	assert(nSelect >= 0 && nSelect < m_wndListBox.GetCount());

	CString string;
	m_wndListBox.GetText(nSelect, string);
	SetString(string);
	return m_wndListBox.SetCurSel(nSelect);
}

void CWndComboBox::OpenListBox(void)
{
	CRect rect = GetScreenRect();
	rect.top = rect.bottom;
	rect.bottom += 200;

	m_wndListBox.Move(rect.TopLeft());
	m_wndListBox.SetVisible(TRUE);
	m_wndListBox.m_bTile = m_bTile;
	m_wndListBox.AdjustWndBase();
	m_wndListBox.SetFocus();
}

DWORD CWndComboBox::GetSelectedItemData(void) const
{
	int nSelectedListNumber = m_wndListBox.GetCurSel();
	if (nSelectedListNumber == -1)
	{
		return 0;
	}
	return m_wndListBox.GetItemData(nSelectedListNumber);
}
void CWndComboBox::GetListBoxText(int nIndex, CString& strString) const
{
	m_wndListBox.GetText(nIndex, strString);
}
int CWndComboBox::GetListBoxTextLength(int nIndex) const
{
	return m_wndListBox.GetTextLen(nIndex);
}
int CWndComboBox::GetListBoxSize(void) const
{
	return m_wndListBox.GetCount();
}