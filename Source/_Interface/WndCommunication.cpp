#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndCommand.h"
#include "party.h"

#include "WndManager.h"

#include "WndChatFilter.h"

#include "DPClient.h"
extern	CDPClient	g_DPlay;


#define		MAX_HISTORY_STRING		20

CWndEditChat::CWndEditChat()
{
}
CWndEditChat::~CWndEditChat()
{
}
void CWndEditChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*)m_pParentWnd;
	if (pWndChat->m_pWndCommand)
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnLButtonDown(nFlags, point);
}
void CWndEditChat::OnSetFocus(CWndBase* pOldWnd)
{
	if (m_string.IsEmpty())
	{
		CWndChat* pWndChat = (CWndChat*)m_pParentWnd;

		if (pWndChat->m_pWndCommand == NULL)
		{
			if (pWndChat->m_nChatChannel == CHATSTY_GENERAL)
			{
			}
			else if (pWndChat->m_nChatChannel == CHATSTY_WHISPER)
			{
				CString string;
				if (pWndChat->m_strCharName.IsEmpty())
					string.Format("/w ");
				else
					string.Format("/w %s ", pWndChat->m_strCharName);
				SetString(string);
			}
			else if (pWndChat->m_nChatChannel == CHATSTY_SHOUT)
			{
				SetString("/s ");
			}
			else if (pWndChat->m_nChatChannel == CHATSTY_PARTY)
			{
				SetString("/p ");
			}
			else if (pWndChat->m_nChatChannel == CHATSTY_GUILD)
			{
				SetString("/g ");
			}
		}
	}


	CWndEdit::OnSetFocus(pOldWnd);
}
void CWndEditChat::OnKillFocus(CWndBase* pNewWnd)
{
	CWndChat* pWndChat = (CWndChat*)m_pParentWnd;
	if (pWndChat->m_pWndCommand && this != pNewWnd)
		pWndChat->m_pWndCommand->Destroy();
	CWndEdit::OnKillFocus(pNewWnd);
}

CWndMacroChat::CWndMacroChat()
{
}
CWndMacroChat::~CWndMacroChat()
{
}
void CWndMacroChat::OnDraw(C2DRender* p2DRender)
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet(APP_COMMUNICATION_CHAT);
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if (pWndEdit->m_string.IsEmpty() == FALSE)
		p2DRender->RenderTexture(CPoint(4, 0), &m_texMacroChat);
}
void CWndMacroChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet(APP_COMMUNICATION_CHAT);
	CWndEditChat* pWndEdit = &pWndChat->m_wndEdit;//GetDlgItem( WIDC_EDIT );
	if (pWndEdit->m_string.IsEmpty() == FALSE)
	{
		CWndChat* pWndChat = (CWndChat*)g_WndMng.GetApplet(APP_COMMUNICATION_CHAT);
		m_GlobalShortcut.m_dwShortcut = SHORTCUT_CHAT;
		m_GlobalShortcut.m_pFromWnd = this;
		m_GlobalShortcut.m_pTexture = m_pTexMacro; //.m_pFromWnd   = this;
		m_GlobalShortcut.m_dwData = 0;

		CString strFormat;
		strFormat = pWndEdit->m_string;

		pWndEdit->m_string.GetTextFormat(strFormat);

		if (strFormat.GetLength() >= (MAX_SHORTCUT_STRING - 1))
		{
			memcpy(m_GlobalShortcut.m_szString, strFormat, MAX_SHORTCUT_STRING - 1);
		}
		else
		{
			strcpy(m_GlobalShortcut.m_szString, strFormat);
		}
	}
}

void CWndMacroChat::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndMacroChat::OnInitialUpdate()
{
	m_pTexMacro = m_textureMng.AddTexture(m_pApp->m_pd3dDevice, MakePath(DIR_ICON, "icon_MacroChat.dds"), 0xffff00ff);//, bMyLoader );
	m_texMacroChat.LoadTexture(m_pApp->m_pd3dDevice, MakePath(DIR_THEME, "wndButtMacroChat.bmp"), 0xffff00ff);
}
CWndTextChat::CWndTextChat()
{
}
CWndTextChat::~CWndTextChat()
{
}
BOOL CWndTextChat::IsPickupSpace(CPoint point)
{
	return FALSE;
}
int CWndChat::m_nChatChannel = CHATSTY_GENERAL;

CWndChat::CWndChat()
{
#ifdef __FL_UPDATE_CHATTING
	m_nOldResizeDir = 1;
#endif

	m_pWndCommand = NULL;
	m_timerInsMsg.Set(MIN(2));
#ifdef __CSC_GAME_GRADE
	m_timerAlertGGrade.Set(MIN(60));
#endif //
	m_nInsMsgCnt = 0;
	m_bChatLock = FALSE;
	m_bMoveLock = FALSE;
	m_pWndChatFilter = NULL;
	m_nHistoryIndex = 0;
	m_bChatLog = FALSE;
}
CWndChat::~CWndChat()
{
	SAFE_DELETE(m_pWndCommand);
	SAFE_DELETE(m_pWndChatFilter);
}
void CWndChat::OnDraw(C2DRender* p2DRender)
{
	if (g_pPlayer == NULL)
		return;

#ifdef __FL_UPDATE_CHATTING
	p2DRender->RenderFillRect(CRect(GetWindowRect().left, GetWindowRect().top, GetWindowRect().right, GetWindowRect().bottom - 50), D3DCOLOR_ARGB(m_nAlphaCount >= 125 ? 125 : m_nAlphaCount, 0, 0, 0));
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	if (!m_nResizeDir && m_nOldResizeDir)
	{
		CRect rect = GetClientRect();
		rect.bottom -= 39;
		pWndText->SetWndRect(rect);

		pWndText->m_wndScrollBar.SetMaxScrollPos();
		pWndText->m_wndScrollBar.DoAlpha(255);
	}
	else if (m_nResizeDir && !m_nOldResizeDir)
		pWndText->m_wndScrollBar.DoAlpha(0);

	m_nOldResizeDir = m_nResizeDir;
#endif

	if (g_Option.m_nInstantHelp)
	{
		CString string;
		if (m_strArray.GetSize() && m_timerInsMsg.TimeOut())
		{
			m_timerInsMsg.Reset();

			string = prj.GetText(TID_SYS_HELP);
			string += " ";
			string += m_strArray.GetAt(m_nInsMsgCnt);
			g_WndMng.PutString(string, NULL, 0xffffff00, CHATSTY_HELP);
			m_nInsMsgCnt++;
			if (m_nInsMsgCnt == m_strArray.GetSize())
				m_nInsMsgCnt = 0;
		}
	}

#ifdef __CSC_GAME_GRADE
	if (m_timerAlertGGrade.TimeOut())
	{
		m_timerAlertGGrade.Reset();
		g_WndMng.PutString(prj.GetText(TID_SYS_ALERTGGRADE_NONPK), NULL, 0xffffff00, CHATSTY_HELP);
	}
#endif //

	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem(WIDC_RADIO4); // party
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem(WIDC_RADIO5); // guild

	pWndRadio4->EnableWindow(g_Party.IsMember(g_pPlayer->m_idPlayer));
	pWndRadio5->EnableWindow(g_pPlayer->GetGuild() == NULL ? FALSE : TRUE);

	if (::GetLanguage() == LANG_KOR || ::GetLanguage() == LANG_TWN || ::GetLanguage() == LANG_HK)
	{
		static TCHAR text[12] = { 0 };
		static wchar_t* wText = g_imeMgr.GetStringIndicator();

		if (wText)
		{
			CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);

			if (pWndStatic)
			{
				WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wText, wcslen(wText), text, 12, NULL, NULL);

				pWndStatic->m_dwColor = D3DCOLOR_ARGB(255, 50, 50, 220);

				if (PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_KOREAN)
				{
					if (g_imeMgr.GetImeState() == 0)
					{
						_tcscpy(text, "En");
					}
				}
				else if (PRIMARYLANGID(g_imeMgr.m_hkl) == LANG_CHINESE)
				{
					static wchar_t wTextCh[3][3] =
					{
						L"\x6CE8",
						L"\x65B0",
						L"\x5176",
					};

					if (g_imeMgr.IsNewPhonetic())
					{
						if (g_imeMgr.GetImeState() == 2)
							_tcscpy(text, "En");
						else
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[1], wcslen(wTextCh[1]), text, 12, NULL, NULL);
					}
					else if (g_imeMgr.IsPhonetic())
					{
						if (g_imeMgr.GetImeState() == 0)
							_tcscpy(text, "En");
						else
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[0], wcslen(wTextCh[0]), text, 12, NULL, NULL);
					}
					else
					{
						if (g_imeMgr.GetImeState() == 0)
							_tcscpy(text, "En");
						else
							WideCharToMultiByteEx(g_imeMgr.m_codePage, 0, wTextCh[2], wcslen(wTextCh[2]), text, 12, NULL, NULL);
					}
				}
				pWndStatic->SetTitle(text);
			}
		}
	}
}
void CWndChat::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_timerDobe.Set(SEC(5));

	LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDC_MACRO_CHAT);
	LPWNDCTRL lpWndText = GetWndCtrl(WIDC_CUSTOM1);
	m_wndMacroChat.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_MACRO_CHAT);
	m_wndMacroChat.AddWndStyle(WBS_NODRAWFRAME);

	lpWndCtrl = GetWndCtrl(WIDC_EDIT);
	m_wndEdit.Create(g_Neuz.GetSafeHwnd(), WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_EDIT);
	m_wndText.Create(WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem(WIDC_HEAD);
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);
	pWndEdit->SetNativeMode();

	CRect rectEdit = m_wndEdit.GetWndRect();
	rectEdit.left = rectEdit.left - 90;
	m_wndEdit.SetWndRect(rectEdit);

	CRect customRect = lpWndCtrl->rect;
	CWndCustom* pCustom = (CWndCustom*)GetDlgItem(WIDC_MACRO_CHAT);
	if (pCustom)
		pCustom->Move(customRect.TopLeft().x - 119, customRect.TopLeft().y);
	if (pWndHead)
	{
		pWndHead->EnableWindow(FALSE);
		pWndHead->SetVisible(FALSE);
	}

	if (g_Option.m_nInstantHelp)
	{
		CScript	s;
		s.Load(MakePath(DIR_CLIENT, "InstantHelp.inc"));
		s.GetToken();
		while (s.tok != FINISHED)
		{
			m_strArray.Add(s.Token);
			s.GetToken();
		}
	}

	SetChannel();
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem(WIDC_RADIO2);
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem(WIDC_RADIO3);
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem(WIDC_RADIO4);
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem(WIDC_RADIO5);
	if (m_nChatChannel == CHATSTY_GENERAL) pWndRadio1->SetCheck(TRUE);	else
		if (m_nChatChannel == CHATSTY_WHISPER) pWndRadio2->SetCheck(TRUE);	else
			if (m_nChatChannel == CHATSTY_SHOUT) pWndRadio3->SetCheck(TRUE);	else
				if (m_nChatChannel == CHATSTY_PARTY) pWndRadio4->SetCheck(TRUE);	else
					if (m_nChatChannel == CHATSTY_GUILD) pWndRadio5->SetCheck(TRUE);
	pWndText->m_nLineSpace = 2;

	LoadTGA(MakePath(DIR_THEME, "WndChatEdit00_1.tga"), &m_wndChatEdit[0]);
	LoadTGA(MakePath(DIR_THEME, "WndChatEdit01.tga"), &m_wndChatEdit[1]);
	LoadTGA(MakePath(DIR_THEME, "WndChatEdit02.tga"), &m_wndChatEdit[2]);

	if (m_wndChatEdit[2].lpData == NULL)
	{
		Error("CWndChat::OnInitialUpdate() m_wndChatEdit[2].lpData = NULL");
		return;
	}

	CWndButton* pWndLock = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	pWndLock->SetCheck(m_bChatLock);
	CWndButton* pWndMoveLock = (CWndButton*)GetDlgItem(WIDC_MOVELOCK);
	pWndMoveLock->SetCheck(m_bMoveLock);
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth = rect.Width();
	int nHeight = rect.Height();
	int x = 0;
	int y = rectRoot.bottom - nHeight;
	CPoint point(x, y);
	Move(point);

	if (g_WndMng.m_pWndChatLog == NULL)
	{
		g_WndMng.m_pWndChatLog = new CWndChatLog;

		if (g_WndMng.m_pWndChatLog)
		{
			g_WndMng.m_pWndChatLog->Initialize(NULL, APP_CHAT_LOG);

			CRect rectRoot = m_pWndRoot->GetLayoutRect();
			CRect rect = GetWindowRect(TRUE);
			CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);

			int x = 0;
			int y = rect.top;

			if ((rect.right + rect2.Width()) < rectRoot.right)
				x = rect.right;
			else
				x = (rect.left - rect2.Width());

			CPoint point(x, y);
			g_WndMng.m_pWndChatLog->Move(point);

			g_WndMng.m_pWndChatLog->SetVisible(FALSE);
		}
}

	if (g_WndMng.m_pWndChatLog)
	{
		CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		if (pWndCheck3)
			pWndCheck3->SetCheck(g_WndMng.m_pWndChatLog->m_bVisible);
	}
}

BOOL CWndChat::Process()
{
	return TRUE;
}

void CWndChat::SetChannel()
{
	m_wndText.SetString("");

	int nChannel = 0;
	switch (CWndChat::m_nChatChannel)
	{
	case CHATSTY_GENERAL: nChannel = 0; break;
	case CHATSTY_WHISPER: nChannel = 1; break;
	case CHATSTY_SHOUT: nChannel = 2; break;
	case CHATSTY_PARTY: nChannel = 3; break;
	case CHATSTY_GUILD: nChannel = 4; break;
	}
	DWORD dwChatFilter = g_Option.m_dwChatFilterEtc | g_Option.m_dwChatFilter[nChannel];

	for (int i = 0; i < g_WndMng.m_aChatString.GetSize(); i++)
	{
		if (g_WndMng.m_aChatStyle.GetAt(i) & dwChatFilter)
		{
			CString string = g_WndMng.m_aChatString.GetAt(i);
			m_wndText.m_string.AddParsingString(string, g_WndMng.m_aChatColor.GetAt(i), 0x00000000, 0, PS_NOT_MACRO);
			m_wndText.m_string.AddString("\n");
		}
	}
	m_wndText.ResetString();
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
}
void CWndChat::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChat::OnKillFocus(CWndBase* pNewWnd)
{

}
BOOL CWndChat::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height());
	SetTitle(GETTEXT(TID_APP_COMMUNICATION_CHAT));
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, WBS_KEY | WBS_THICKFRAME, CPoint(792, 130), pWndParent);
}

extern	CDPClient	g_DPlay;

void CWndChat::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChat::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (m_pWndCommand == pWndChild)
	{
		SAFE_DELETE(m_pWndCommand);
	}
	if (m_pWndChatFilter == pWndChild)
	{
		SAFE_DELETE(m_pWndChatFilter);

		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
		if (pWndWorld)
			pWndWorld->SetFocus();
	}
}
void CWndChat::SerializeRegInfo(CAr& ar, DWORD& dwVersion)
{
	CWndNeuz::SerializeRegInfo(ar, dwVersion);
	if (ar.IsLoading())
	{
		if (dwVersion == 0)
		{
		}
		else
		{
			ar >> m_bChatLock;
		}
		CWndButton* pWndLock = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		pWndLock->SetCheck(m_bChatLock);
	}
	else
	{
		dwVersion = 1;
		ar << m_bChatLock;
	}
}

BOOL CWndChat::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	switch (nID)
	{
	case WIDC_CLOSE:
		nID = WTBID_CLOSE;
		break;
	case WIDC_HELP:
		nID = WTBID_HELP;
		break;
	case WIDC_BUTTON1:
	{
		if (m_pWndChatFilter == NULL)
		{
			m_pWndChatFilter = new CWndChatFilter;
			m_pWndChatFilter->Initialize(this);
		}
	}
	break;
	case WIDC_CHECK1:
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
		m_bChatLock = pWndCheck->GetCheck();
	}
	break;

	case WIDC_CHECK3:
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK3);
		BOOL bChatLog = pWndCheck->GetCheck();

		m_bChatLog = bChatLog;

		if (g_WndMng.m_pWndChatLog)
		{
			if (bChatLog)
			{
				CRect rectRoot = m_pWndRoot->GetLayoutRect();
				CRect rect = GetWindowRect(TRUE);
				CRect rect2 = g_WndMng.m_pWndChatLog->GetClientRect(TRUE);

				int x = 0;
				int y = rect.top;

				if ((rect.right + rect2.Width()) < rectRoot.right)
					x = rect.right;
				else
					x = (rect.left - rect2.Width());

				CPoint point(x, y);
				g_WndMng.m_pWndChatLog->Move(point);
			}

			g_WndMng.m_pWndChatLog->SetVisible(bChatLog);
		}
	}
	break;

	case WIDC_MOVELOCK:
	{
		CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_MOVELOCK);
		m_bMoveLock = pWndCheck->GetCheck();
		DWORD dwStyle = GetWndStyle();
		if (m_bMoveLock)
			dwStyle &= (~WBS_MOVE);
		else
			dwStyle |= WBS_MOVE;
		SetWndStyle(dwStyle);
	}
	break;
	case WIDC_RADIO1:
		m_nChatChannel = CHATSTY_GENERAL;
		m_wndEdit.SetString("");
		m_wndEdit.SetFocus();
		SetChannel();
		break;
	case WIDC_RADIO2:
		m_nChatChannel = CHATSTY_WHISPER;
		m_wndEdit.SetString("/w ");
		m_wndEdit.SetFocus();
		SetChannel();
		break;
	case WIDC_RADIO3:
		m_nChatChannel = CHATSTY_SHOUT;
		m_wndEdit.SetString("/s ");
		m_wndEdit.SetFocus();
		SetChannel();
		break;
	case WIDC_RADIO4:
		m_nChatChannel = CHATSTY_PARTY;
		m_wndEdit.SetString("/p ");
		m_wndEdit.SetFocus();
		SetChannel();
		break;
	case WIDC_RADIO5:
		m_nChatChannel = CHATSTY_GUILD;
		m_wndEdit.SetString("/g ");
		m_wndEdit.SetFocus();
		SetChannel();
		break;
	case APP_COMMAND:
	{
		if (message == WNM_DBLCLK)
		{
			CString string;
			m_pWndCommand->GetText(m_pWndCommand->GetCurSel(), string);
			string += " ";
			m_wndEdit.SetString("/");
			m_wndEdit.AddString(string);
			m_wndEdit.ResetString();
			m_pWndCommand->Destroy();
		}
		SetFocus();
	}
	case WIDC_HEAD:
		if (message == EN_RETURN)
		{
			g_WndMng.SetFocus();
		}
		break;
	case WIDC_EDIT:
		if (message == EN_UP)
		{
			if (m_strHistory.size() == 0)
				return FALSE;

			if (m_nHistoryIndex >= (int)(m_strHistory.size()))
			{
				m_strHistory.front();
				m_nHistoryIndex = 0;
			}

			m_nHistoryIndex++;
			m_wndEdit.m_string = m_strHistory[m_strHistory.size() - m_nHistoryIndex];

			return TRUE;
		}

#ifdef __FL_UPDATE_CHATTING
		if (message == EN_DOWN)
		{
			if (m_strHistory.size() == 0)
				return FALSE;

			if (m_nHistoryIndex <= 1)
			{
				m_strHistory.front();
				m_nHistoryIndex = (int)(m_strHistory.size() + 1);
			}

			m_nHistoryIndex--;
			m_wndEdit.m_string = m_strHistory[m_strHistory.size() - m_nHistoryIndex];

			return TRUE;
		}
#endif
		if (message == EN_RETURN)
		{
			BOOL bFinished = TRUE;
			if (m_pWndCommand)
			{
				CString str1, str2;
				str1 = m_wndEdit.m_string.Right(m_wndEdit.m_string.GetLength() - 1);
				if (m_pWndCommand->GetCurSel() != -1)
				{
					m_pWndCommand->GetText(m_pWndCommand->GetCurSel(), str2);
					if (str1 != str2)
					{
						str2 += " ";
						m_wndEdit.SetString("/");
						m_wndEdit.AddString(str2);
						m_wndEdit.ResetString();
						bFinished = FALSE;
					}
				}
				m_pWndCommand->Destroy();
			}
			if (bFinished == TRUE)
			{
				CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
				CWndEdit* pWndHead = (CWndEdit*)GetDlgItem(WIDC_HEAD);
				CString string;
				if (pWndHead->m_string.IsEmpty() == FALSE &&
					(pWndHead->m_string[0] == '/' ||
						(m_wndEdit.m_string.IsEmpty() == FALSE && pWndHead->m_string[0] != '/' && m_wndEdit.m_string[0] != '/'))
					)
					string = pWndHead->m_string + " ";

				WORD wCodePage = g_codePage;
				const char* begin = m_wndEdit.m_string;
				const char* end = begin + m_wndEdit.m_string.GetLength();
				const char* iter = begin;
				while (*iter && iter < end)
				{
					if (*((BYTE*)iter) >= 0x80)
					{
						if (wCodePage != m_wndEdit.m_string.m_awCodePage[iter - begin])
						{
							wCodePage = m_wndEdit.m_string.m_awCodePage[iter - begin];

							char temp[16];

							if (wCodePage == g_codePage)
							{
								wsprintf(temp, "#nl");
							}
							else
							{
								wsprintf(temp, "#l%04d", wCodePage);
							}
							string += temp;
						}
					}
					const char* next = CharNextEx(iter, wCodePage);
					char temp[16];
					memcpy(temp, iter, next - iter);
					temp[next - iter] = 0;
					string += temp;
					iter = next;
				}
				if (m_strHistory.size() >= MAX_HISTORY_STRING)
				{
					m_nHistoryIndex = 0;
					m_strHistory.erase(m_strHistory.begin());
				}

				m_strHistory.push_back(string);
				m_nHistoryIndex = 0;

				int a = string.GetLength();
				if (!m_wndEdit.m_string.IsEmpty())
					m_wndEdit.m_string.GetTextFormat(string);

				g_WndMng.ParsingChat(string);
				CScanner scanner;
				scanner.SetProg(m_wndEdit.m_string.LockBuffer());
				scanner.GetToken();
				if (scanner.Token == "/")
				{
					scanner.GetToken();
					if (scanner.Token == "w" || scanner.Token == "whisper")
					{
						scanner.GetToken();
						m_strCharName = scanner.Token;
					}
				}
				m_wndEdit.m_string.UnlockBuffer();
				m_wndEdit.Empty();
				if (m_bChatLock == FALSE)
				{
					if (IsWndStyle(WBS_MODAL) == TRUE)
					{
						DelWndStyle(WBS_MODAL);
						if (g_Neuz.pOldModalWindow)
						{
							g_Neuz.pOldModalWindow->SetFocus();
						}
					}
					else
					{
						CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
						if (pWndWorld)
						{
							pWndWorld->SetFocus();
						}
						else
						{
							g_WndMng.SetFocus();
						}
					}
				}
				else
					m_wndEdit.SetFocus();
			}
		}
		else if (message == EN_CHANGE)
		{
			if (m_wndEdit.m_string.IsEmpty() == FALSE)
			{
				if (m_wndEdit.m_string[0] == '/' && m_wndEdit.GetOffset() == 1 && m_pWndCommand == NULL)
				{
					SAFE_DELETE(m_pWndCommand);
					m_pWndCommand = new CWndCommand;
					m_pWndCommand->m_pWndEdit = this;
					m_pWndCommand->Initialize(this);
					CRect rect = m_wndEdit.GetScreenRect();
					CPoint point = rect.TopLeft();
					point.y -= m_pWndCommand->GetWndRect().Height();
					m_pWndCommand->Move(point);
					m_wndEdit.SetFocus();
				}
				else
					if (m_pWndCommand)
					{
						CString string;
						string = m_wndEdit.m_string.Right(m_wndEdit.m_string.GetLength() - 1);

						if (string.Find(' ') == -1)
							m_pWndCommand->SelectString(0, string);
						else
							m_pWndCommand->Destroy();

						if (::GetLanguage() == LANG_KOR)
						{
							if (m_wndEdit.m_string == "/�� ")
							{
								m_wndEdit.SetString("/g ");
							}
							else if (m_wndEdit.m_string == "/�� ")
							{
								m_wndEdit.SetString("/p ");
							}
							else if (m_wndEdit.m_string == "/�� ")
							{
								m_wndEdit.SetString("/s ");
							}
							else if (m_wndEdit.m_string == "/�� ")
							{
								m_wndEdit.SetString("/w ");
							}
						}
					}
			}
			else if (m_pWndCommand)
				m_pWndCommand->Destroy();
		}
		break;
	case 10000:
		break;
	case 10001:
		break;
	case 10002:
		Destroy();
		break;
}


	if (nID == WIDC_CHECK1 || nID == WIDC_CHECK3 || nID == WIDC_MOVELOCK)
	{
		CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetApplet(APP_WORLD);
		if (pWndWorld)
			pWndWorld->SetFocus();
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndChat::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndChat::Parsing(CString string)
{

}
void CWndChat::PutString(LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle)
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);

	BOOL bLineEnd = FALSE;
	if (pWndText->m_wndScrollBar.GetScrollPos() == (pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage()))
		bLineEnd = TRUE;

	pWndText->AddString(lpszString, dwColor, dwPStyle);
	pWndText->AddString("\n", dwColor);// += '\n';


	if (pWndText->m_string.GetLineCount() >= 175)
	{
		pWndText->m_string.DeleteLine(0, 50);
		pWndText->UpdateScrollBar();
	}

	if (bLineEnd)
		pWndText->m_wndScrollBar.SetMaxScrollPos();
	}
void CWndChat::OnDestroy()
{
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	g_WndMng.m_ChatString = pWndText->m_string;
}
void CWndChat::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 16 * 29, 16 * 5);
	AdjustMaxRect(&rectWnd, 16 * 49, 16 * 35);

	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(0, 4, 3, 6);

	m_rectLayout = m_rectClient;
	if (bOnSize && (rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height()))
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
	MakeVertexBuffer();
}
void CWndChat::OnSize(UINT nType, int cx, int cy)
{
	if (m_pWndCommand)
		m_pWndCommand->Destroy();

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);
	CWndEdit* pWndHead = (CWndEdit*)GetDlgItem(WIDC_HEAD);
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT);

	CRect rectEdit;

	CRect rect = GetClientRect();
#ifndef __FL_UPDATE_CHATTING
	rect.top = 0;
	rect.left = 0;
	rect.right -= 0;
	rect.bottom -= 39;

	pWndText->SetWndRect(rect);
	pWndText->m_wndScrollBar.SetMaxScrollPos();
#endif

	rect = GetClientRect();
	rectEdit = pWndHead->GetWndRect();
	rectEdit.top = rect.bottom - 18;
	rectEdit.bottom = rect.bottom;
	rect.DeflateRect(1, 1);
	pWndHead->SetWndRect(rectEdit);

	rect = GetClientRect();
	rectEdit = m_wndEdit.GetWndRect();
	rectEdit.top = rect.bottom - 18;
	rectEdit.bottom = rect.bottom;
	rectEdit.right = rect.right - 20;
	rect.DeflateRect(1, 1);
	m_wndEdit.SetWndRect(rectEdit);

	rect = GetClientRect();
	rectEdit = m_wndMacroChat.GetWndRect();
	rectEdit.top = rect.bottom - 19;
	rectEdit.bottom = rect.bottom;

	m_wndMacroChat.SetWndRect(rectEdit);
	CWndButton* pWndRadio1 = (CWndButton*)GetDlgItem(WIDC_RADIO1);
	CWndButton* pWndRadio2 = (CWndButton*)GetDlgItem(WIDC_RADIO2);
	CWndButton* pWndRadio3 = (CWndButton*)GetDlgItem(WIDC_RADIO3);
	CWndButton* pWndRadio4 = (CWndButton*)GetDlgItem(WIDC_RADIO4);
	CWndButton* pWndRadio5 = (CWndButton*)GetDlgItem(WIDC_RADIO5);
	CWndButton* pWndCheck = (CWndButton*)GetDlgItem(WIDC_CHECK1);
	CWndButton* pWndCheck2 = (CWndButton*)GetDlgItem(WIDC_MOVELOCK);
	CWndButton* pWndButton1 = (CWndButton*)GetDlgItem(WIDC_BUTTON1);
	CWndButton* pWndButton2 = (CWndButton*)GetDlgItem(WIDC_HELP);
	CWndButton* pWndButton3 = (CWndButton*)GetDlgItem(WIDC_CLOSE);

	CWndButton* pWndCheck3 = (CWndButton*)GetDlgItem(WIDC_CHECK3);

	rect = GetClientRect();
	rectEdit = pWndRadio1->GetWndRect();
	rectEdit.top = rect.bottom - 38;
	rectEdit.left = 2;
	pWndRadio1->Move(rectEdit.TopLeft());
	rectEdit.left = 50;
	pWndRadio2->Move(rectEdit.TopLeft());
	rectEdit.left = 100;
	pWndRadio3->Move(rectEdit.TopLeft());
	rectEdit.left = 150;
	pWndRadio4->Move(rectEdit.TopLeft());
	rectEdit.left = 200;
	pWndRadio5->Move(rectEdit.TopLeft());
	rectEdit.left = rect.right - 200;
	pWndCheck->Move(rectEdit.TopLeft());
	rectEdit.left = rect.right - 150;
	pWndButton1->Move(rectEdit.TopLeft());

	rectEdit.left = rect.right - 100;
	pWndCheck3->Move(rectEdit.TopLeft());
	rectEdit.left = rect.right - 49;

	CRect rectEdit2 = rectEdit;

	rectEdit2.top += 1;
	pWndCheck2->Move(rectEdit2.TopLeft());
	rectEdit.left = rect.right - 32;
	rectEdit.top += 1;
	pWndButton2->Move(rectEdit.TopLeft());
	rectEdit.left = rect.right - 15;
	pWndButton3->Move(rectEdit.TopLeft());

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC1);

	if (pWndStatic)
	{
		rectEdit.top += 20;
		rectEdit.left -= 5;
		pWndStatic->Move(rectEdit.TopLeft());
	}

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndChat::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

void CWndChat::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pWndCommand)
		m_pWndCommand->Destroy();
}

void CWndChat::OnRButtonDown(UINT nFlags, CPoint point)
{
	int nLineCount = m_wndText.m_string.GetLineCount();
	int nPos = m_wndText.IsWndStyle(WBS_VSCROLL) ? m_wndText.m_wndScrollBar.GetScrollPos() : 0;
	DWORD dwMaxHeight = m_wndText.GetFontHeight();
	for (int i = nPos; i < nLineCount; ++i)
	{
		if ((int)(((i - nPos) * dwMaxHeight)) <= point.y && point.y < (int)((dwMaxHeight * (i - nPos + 1))))
		{
			CString str = m_wndText.m_string.GetLine(i);
			char szFind[MAX_NAME] = { 0, };

			BOOL bChat = FALSE;


			int nFindName = str.Find(" :");
			if (0 < nFindName && nFindName < MAX_NAME)
			{
				int nFindSpace = str.Find(' ');
				if (nFindSpace == -1 || nFindName <= nFindSpace)
				{
					strncpy(szFind, str, nFindName);
					if (strcmp(szFind, g_pPlayer->GetName()) != 0)
						bChat = TRUE;
				}
			}


			if (bChat == FALSE)
			{
				int nFindName1 = str.Find("<- [");
				int nFindName2 = str.Find("]");
				if (0 == nFindName1)
				{
					str.Replace("<- [", "");
				}
				else
				{
					nFindName1 = str.Find("-> [");
					str.Replace("-> [", "");
				}

				if (0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 4)
				{
					nFindName2 = str.Find("]");
					strncpy(szFind, str, nFindName2);
					int nFindSpace = strspn(" ", szFind);
					if (nFindSpace < 1 && strcmp(szFind, g_pPlayer->GetName()) != 0)
						bChat = TRUE;
				}
			}


			if (bChat == FALSE)
			{
				int nFindName1 = str.Find("[");
				int nFindName2 = str.Find("]");
				if (0 == nFindName1 && 0 < nFindName2 && nFindName2 - nFindName1 < MAX_NAME + 1)
				{
					str.Replace("[", "");
					nFindName2 = str.Find("]");
					strncpy(szFind, str, nFindName2);
					int nFindSpace = strspn(" ", szFind);
					if (nFindSpace < 1 && strcmp(szFind, g_pPlayer->GetName()) != 0)
						bChat = TRUE;

					CString strSystem = prj.GetText(TID_ADMIN_ANNOUNCE);
					strSystem.Replace("[", "");
					int nFindSystem = strSystem.Find("]");
					if (0 < nFindSystem)
					{
						char szFindSystem[MAX_NAME] = { 0, };
						strncpy(szFindSystem, strSystem, nFindSystem);
						if (strcmp(szFind, szFindSystem) == 0)
							bChat = FALSE;
					}
				}
			}

			if (bChat)
			{
				CString strWhisper;
				if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
					strWhisper.Format("/whisper %s ", szFind);
				else
					strWhisper.Format("/say %s ", szFind);

				m_wndEdit.SetString(strWhisper);
				m_wndEdit.SetFocus();
			}
			break;
			}
		}
	}
void CWndChat::OnRButtonUp(UINT nFlags, CPoint point)
{

}

void CWndChat::OnMouseMove(UINT nFlags, CPoint point)
{
}



BOOL CWndChat::OnEraseBkgnd(C2DRender* p2DRender)
{
	return TRUE;
}

void CWndChat::AdditionalSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat)
{

#ifndef __FL_UPDATE_CHATTING
	CWndNeuz::AdditionalSkinTexture(pDest, size, d3dFormat);
#endif

	CRect rect = GetWindowRect();
	CPoint point = CPoint(0, rect.Height() - m_wndChatEdit[0].size.cy);
	PaintTexture(pDest, &m_wndChatEdit[0], point, size);
	int nWidth = (rect.Width() - m_wndChatEdit[0].size.cx) / m_wndChatEdit[1].size.cx;
	point.x += m_wndChatEdit[0].size.cx;
	for (int i = 0; i < nWidth; i++)
	{
		if (i == nWidth - 2)
			PaintTexture(pDest, &m_wndChatEdit[2], point, size);
		else
			if (i < nWidth - 2)
				PaintTexture(pDest, &m_wndChatEdit[1], point, size);
		point.x += m_wndChatEdit[1].size.cx;
	}
}


CWndChatLog::CWndChatLog()
{
#ifdef __FL_UPDATE_CHATTING
	m_nOldResizeDir = 1;
#endif
}
CWndChatLog::~CWndChatLog()
{
}
void CWndChatLog::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL lpWndText = GetWndCtrl(WIDC_CUSTOM1);
	m_wndText.Create(WBS_NOFRAME | WBS_NODRAWFRAME | WBS_VSCROLL, lpWndText->rect, this, WIDC_TEXT);
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);

	pWndText->m_nLineSpace = 2;

	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rect = GetWindowRect();
	int nWidth = rect.Width();
	int nHeight = rect.Height();
	int x = 0;//rectRoot.left + (rectRoot.Width()  / 2) - (nWidth  / 2);
	int y = rectRoot.bottom - nHeight;
	CPoint point(x, y);
	Move(point);

	m_wndMenuPlace.CreateMenu(this);
	m_wndMenuPlace.AppendMenu(0, 0, prj.GetText(TID_GAME_GUIDE_HIDE));
}

BOOL CWndChatLog::Process()
{
	return TRUE;
}
void CWndChatLog::OnSetFocus(CWndBase* pOldWnd)
{
}
void CWndChatLog::OnKillFocus(CWndBase* pNewWnd)
{
}
BOOL CWndChatLog::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	CRect rectWindow = m_pWndRoot->GetWindowRect();
	CRect rect(154, rectWindow.Height() - 100, 154 + rectWindow.Width() - 300, rectWindow.Height());
	SetTitle(GETTEXT(TID_APP_COMMUNICATION_CHAT));
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), dwWndId, WBS_THICKFRAME, CPoint(792, 130), pWndParent);
	//	return CWndNeuz::InitDialog( g_Neuz.GetSafeHwnd(), dwWndId, 0, CPoint( 792, 130 ), pWndParent );
}

void CWndChatLog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
void CWndChatLog::OnDestroyChildWnd(CWndBase* pWndChild)
{
}
BOOL CWndChatLog::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	CWndBase* pWndBase = (CWndBase*)pLResult;

	if (pWndBase->m_pParentWnd == &m_wndMenuPlace)
	{
		SetFocus();
		switch (nID)
		{
		case 0:
		{
			CWndChat* pWndChat = (CWndChat*)g_WndMng.GetWndBase(APP_COMMUNICATION_CHAT);

			if (pWndChat)
			{
				CWndButton* pWndCheck = (CWndButton*)pWndChat->GetDlgItem(WIDC_CHECK3);

				if (pWndCheck)
					pWndCheck->SetCheck(FALSE);
			}

			SetVisible(FALSE);
		}
		break;
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndChatLog::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndChatLog::PutString(LPCTSTR lpszString, DWORD dwColor, DWORD dwPStyle)
{
	CString strNew;
	CTime time = CTime::GetCurrentTime();
	strNew.Format("[%02d:%02d:%02d] %s", time.GetHour(), time.GetMinute(), time.GetSecond(), lpszString);
	lpszString = strNew;

	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT);

	BOOL bLineEnd = FALSE;
	if (pWndText->m_wndScrollBar.GetScrollPos() == (pWndText->m_wndScrollBar.GetMaxScrollPos() - pWndText->m_wndScrollBar.GetScrollPage()))
		bLineEnd = TRUE;

	pWndText->AddString(lpszString, dwColor, dwPStyle);
	pWndText->AddString("\n", dwColor);// += '\n';

	if (pWndText->m_string.GetLineCount() >= 350)
	{
		pWndText->m_string.DeleteLine(0, 50);
		pWndText->UpdateScrollBar();
	}

	if (bLineEnd)
		pWndText->m_wndScrollBar.SetMaxScrollPos();
}
void CWndChatLog::OnDestroy()
{
}
void CWndChatLog::OnSize(UINT nType, int cx, int cy)
{
#ifndef __FL_UPDATE_CHATTING
	CRect rect = GetClientRect();
	rect.top = 0;
	rect.left = 0;
	rect.right -= 0;
	rect.bottom -= 0;

	m_wndText.SetWndRect(rect);
	m_wndText.m_wndScrollBar.SetMaxScrollPos();
#endif

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndChatLog::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndChatLog::OnRButtonDown(UINT nFlags, CPoint point)
{
	CRect rect = GetWindowRect(TRUE);
	CRect menurect = m_wndMenuPlace.GetWindowRect();

	rect.top += point.y;
	rect.left += point.x;

	m_wndMenuPlace.Move(CPoint(rect.left, rect.top));

	m_wndMenuPlace.SetVisible(!m_wndMenuPlace.IsVisible());
	m_wndMenuPlace.SetFocus();
}
void CWndChatLog::OnRButtonUp(UINT nFlags, CPoint point)
{
}
void CWndChatLog::OnMouseMove(UINT nFlags, CPoint point)
{
}
void CWndChatLog::AdditionalSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat)
{
#ifndef __FL_UPDATE_CHATTING
	CWndNeuz::AdditionalSkinTexture(pDest, size, d3dFormat);
#endif
}

void CWndChatLog::SetWndRect(CRect rectWnd, BOOL bOnSize)
{
	AdjustMinRect(&rectWnd, 16 * 17, 16 * 3);
	AdjustMaxRect(&rectWnd, 16 * 49, 16 * 35);

	CRect rectOld = m_rectClient;
	m_rectWindow = rectWnd;
	m_rectClient = m_rectWindow;
	m_rectClient.DeflateRect(0, 4, 3, 6);
	m_rectLayout = m_rectClient;
	if (bOnSize && (rectOld.Width() != m_rectClient.Width() || rectOld.Height() != m_rectClient.Height()))
		OnSize(0, m_rectClient.Width(), m_rectClient.Height());
	MakeVertexBuffer();
}

#ifdef __FL_UPDATE_CHATTING
void CWndChatLog::OnDraw(C2DRender* p2DRender)
{
	p2DRender->RenderFillRect(GetWindowRect(), D3DCOLOR_ARGB(m_nAlphaCount >= 125 ? 125 : m_nAlphaCount, 0, 0, 0));
	if (!m_nResizeDir && m_nOldResizeDir)
	{
		m_wndText.SetWndRect(GetClientRect());

		m_wndText.m_wndScrollBar.SetMaxScrollPos();
		m_wndText.m_wndScrollBar.DoAlpha(255);
	}
	else if (m_nResizeDir && !m_nOldResizeDir)
		m_wndText.m_wndScrollBar.DoAlpha(0);

	m_nOldResizeDir = m_nResizeDir;
}
#endif