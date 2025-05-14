// WndArcane.cpp: implementation of the CWndNeuz class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "defineObj.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DPLoginClient.h"
#include "DPClient.h"
#include "dpCertified.h"
#include "wndCredit.h"
#include "..\_Common\Debug.h"
#include "webbox.h"

#include "WndManager.h"
#include "Network.h"


extern	CDPLoginClient	g_dpLoginClient;
extern	CDPClient	g_DPlay;
extern	CDPCertified	g_dpCertified;

extern BYTE  nHairColor[10][3];

#ifdef __CERTIFIER_COLLECTING_SYSTEM
#include "DPCollectClient.h"
#endif // __CERTIFIER_COLLECTING_SYSTEM

BOOL GetIePath(LPSTR lpPath)
{
	LONG result;
	HKEY hKey;
	DWORD dwType;
	char data[MAX_PATH];
	DWORD dataSize = MAX_PATH + 1;

	result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 0, KEY_QUERY_VALUE, &hKey);
	if (result == ERROR_SUCCESS)
	{
		result = ::RegQueryValueEx(hKey, "Path", NULL, &dwType, (unsigned char*)data, &dataSize);
		strcpy(lpPath, data);
		lpPath[lstrlen(lpPath) - 1] = '\0';
	}
	else
		return FALSE;

	RegCloseKey(hKey);
	return TRUE;
}

BOOL CWndConnectingBox::Initialize(CWndBase* pWndParent, DWORD nType)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);

	Create(_T(prj.GetText(TID_DIAG_0068)), /*MB_CANCEL*/0xFFFFFFFF, rect, APP_MESSAGEBOX);
	m_wndText.SetString(_T(prj.GetText(TID_DIAG_0064)));

	m_wndText.ResetString();
	return CWndMessageBox::Initialize(pWndParent, 0);
}
BOOL CWndConnectingBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	return TRUE;
}

BOOL CWndCharBlockBox::Initialize(CWndBase* pWndParent, DWORD nType)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);

	Create(_T(prj.GetText(TID_DIAG_0068)), MB_CANCEL, rect, APP_MESSAGEBOX);
	m_wndText.SetString(_T(prj.GetText(TID_DIAG_0073)));

	m_wndText.ResetString();
	return CWndMessageBox::Initialize(pWndParent, 0);
}
BOOL CWndCharBlockBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case IDCANCEL:
			//Destroy();
			break;
		}
	}
	return CWndMessageBox::OnChildNotify(message, nID, pLResult);
}
BOOL CWndAllCharBlockBox::Initialize(CWndBase* pWndParent, DWORD nType)
{
	CRect rect = m_pWndRoot->MakeCenterRect(250, 130);

	Create(_T(prj.GetText(TID_DIAG_0068)), MB_CANCEL, rect, APP_MESSAGEBOX);
	m_wndText.SetString(_T(prj.GetText(TID_DIAG_0074)));

	m_wndText.ResetString();
	return CWndMessageBox::Initialize(pWndParent, 0);
}
BOOL CWndAllCharBlockBox::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case IDCANCEL:
		{
			g_dpLoginClient.DeleteDPObject();
			CWndSelectChar* pWndSelectChar
				= (CWndSelectChar*)g_WndMng.GetWndBase(APP_SELECT_CHAR);
			if (pWndSelectChar)
			{
				pWndSelectChar->Destroy();
			}
			g_dpCertified.SendCertify();
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
			CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
		}
		//Destroy();
		break;
		}
	}
	return CWndMessageBox::OnChildNotify(message, nID, pLResult);
}

////////////////////////////////////////////////////////////////////////////
// Login
////////////////////////////////////////////////////////////////////////////

CWndLogin::CWndLogin()
{
	SetPutRegInfo(FALSE);
}
CWndLogin::~CWndLogin()
{
}
void CWndLogin::OnDraw(C2DRender* p2DRender)
{

}

BOOL CWndLogin::Process()
{
	if (g_Neuz.m_dwTimeOutDis < GetTickCount())
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();

		g_WndMng.OpenMessageBoxUpper(_T(prj.GetText(TID_DIAG_0043)));

		m_bDisconnect = TRUE;
	}

	if (g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect)
	{
		m_bDisconnect = FALSE;
		g_WndMng.CloseMessageBox();
		g_dpCertified.DeleteDPObject();
		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->EnableWindow(TRUE);
	}

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC4);
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 && GetAsyncKeyState(VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))
		pWndStatic->SetTitle("Caps Lock: ON");
	else
	{
		if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 || GetAsyncKeyState(VK_SHIFT) >> ((sizeof(SHORT) * 8) - 1))
			pWndStatic->SetTitle("Caps Lock: ON");
		else
			pWndStatic->SetTitle("Caps Lock: OFF");
	}

	return 1;
}

void CWndLogin::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();
	CRect rect = GetClientRect();

	CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
	CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);
	CWndButton* pSaveAccount = (CWndButton*)GetDlgItem(WIDC_CHECK1);

	pAccount->EnableModeChange(FALSE);
	pAccount->SetTabStop(TRUE);

	CWndButton* pPasswordStyle = (CWndButton*)GetDlgItem(WIDC_CHECK2);
	if (!pPasswordStyle->GetCheck())
		pPassword->AddWndStyle(EBS_PASSWORD);

	pPassword->SetTabStop(TRUE);
	pPassword->EnableModeChange(FALSE);

	CWndButton* pOk = (CWndButton*)GetDlgItem(WIDC_OK);
	CWndButton* pQuit = (CWndButton*)GetDlgItem(WIDC_QUIT);

	pOk->SetDefault(TRUE);

	pAccount->SetString(g_Option.m_szAccount);
	pSaveAccount->SetCheck(g_Option.m_bSaveAccount);

	if (g_Option.m_szAccount[0])
		pPassword->SetFocus();
	else
		pAccount->SetFocus();

	MoveParentCenter();

	g_Neuz.m_dwTimeOutDis = 0xffffffff;
	m_bDisconnect = FALSE;

	prj.LoadUserAccounts();

	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
	pWndList->ResetContent();

	for (auto& it : prj.mapUserAccounts)
		pWndList->AddString(it.first);

	CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
	if (pEdit)
		pEdit->SetStringBg("Account..");

	pEdit = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);
	if (pEdit)
		pEdit->SetStringBg("Password..");
}
BOOL CWndLogin::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_LOGIN, WBS_KEY, CPoint(0, 0), pWndParent);
}

void CWndLogin::Connected(long lTimeSpan)
{
	BOOL bSave = TRUE;

	auto data = prj.mapUserAccounts.find(g_Neuz.m_szAccount);
	if (data != prj.mapUserAccounts.end())
	{
		if (data->second != g_Neuz.m_szPassword)
			data->second = g_Neuz.m_szPassword;
		else
			bSave = FALSE;
	}
	else
	{
		if (g_Option.m_bSaveAccount)
			prj.mapUserAccounts.insert(make_pair(g_Neuz.m_szAccount, g_Neuz.m_szPassword));
		else
			bSave = FALSE;
	}

	if (bSave)
		prj.SaveUserAccounts();

	g_WndMng.CloseMessageBox();
	g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_SELECT_SERVER);
	Destroy();
}

BOOL CWndLogin::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_DBLCLK)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pWndList->GetCurSel() >= 0)
		{
			auto data = prj.mapUserAccounts.find(pWndList->GetString(pWndList->GetCurSel()));
			if (data != prj.mapUserAccounts.end())
			{
				if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite))
				{
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0043)));
					return FALSE;
				}

				CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
				pButton->EnableWindow(FALSE);

				CString strAccount, strPassword;
				strAccount = data->first;
				strPassword = data->second;

				if (IsInvalidAccountName(strAccount) || IsInvalidPasswordName(strPassword))
				{
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0013)));
					pButton->EnableWindow(TRUE);
					return TRUE;
				}

				g_Neuz.SetAccountInfo(strAccount, strPassword);
				g_dpCertified.SendCertify();
				g_WndMng.OpenCustomBox(NULL, new CWndConnectingBox);
			}
		}
	}
	else if (nID == WIDC_BUTTON1)
	{
		CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_LISTBOX1);
		if (pWndList->GetCurSel() >= 0)
		{
			prj.mapUserAccounts.erase(pWndList->GetString(pWndList->GetCurSel()));
			prj.SaveUserAccounts();

			pWndList->ResetContent();
			for (auto& it : prj.mapUserAccounts)
				pWndList->AddString(it.first);
		}
	}
	else if (nID == WIDC_BTN_SETUPPOWER)
	{
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_OPTIONEX);
	}

	switch (nID)
	{
	case WIDC_CHECK1:
	{
		g_Option.m_bSaveAccount = !g_Option.m_bSaveAccount;
		CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
		strcpy(g_Option.m_szAccount, pAccount->GetString());
	}
	break;

	case WIDC_CHECK2:
	{
		CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);
		CWndButton* pPasswordStyle = (CWndButton*)GetDlgItem(WIDC_CHECK2);
		if (!pPasswordStyle->GetCheck())
			pPassword->AddWndStyle(EBS_PASSWORD);
		else
			pPassword->DelWndStyle(EBS_PASSWORD);
	}
	break;

	case WIDC_OK:
	{
		if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite))
		{
			// Can't connect to server
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0043)));
			break;
		}

		CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pButton->EnableWindow(FALSE);

		CWndEdit* pAccount = (CWndEdit*)GetDlgItem(WIDC_ACCOUNT);
		CWndEdit* pPassword = (CWndEdit*)GetDlgItem(WIDC_PASSWORD);

		CString strAccount, strPassword;
		strAccount = pAccount->GetString();
		strPassword = pPassword->GetString();

		//__FIX_SQL_INJECTS
		if (IsInvalidAccountName(pAccount->GetString()))
		{
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0013)));
			pButton->EnableWindow(TRUE);
			return TRUE;
		}

		strcpy(g_Option.m_szAccount, pAccount->GetString());
		g_Neuz.SetAccountInfo(pAccount->GetString(), pPassword->GetString());
		g_dpCertified.SendCertify();
		g_WndMng.OpenCustomBox(NULL, new CWndConnectingBox);
		break;
	}

	case WIDC_QUIT:
	case WTBID_CLOSE:
		::PostMessage(g_Neuz.GetSafeHwnd(), WM_CLOSE, 0, 0);
		break;

	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndLogin::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndLogin::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndLogin::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

void CWndLogin::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;
}

CWndSelectServer::CWndSelectServer()
{
	m_atexPannel = NULL;
	m_dwChangeBannerTime = g_tmCurrent + SEC(10);
	m_vecStrBanner.clear();
	SetPutRegInfo(FALSE);
}
CWndSelectServer::~CWndSelectServer()
{
	if (m_atexPannel)
	{
		SAFE_DELETE(m_atexPannel);
	}
}
void CWndSelectServer::OnDraw(C2DRender* p2DRender)
{
	//p2DRender->TextOut( 5, 225, _T( "URL" ) );
	//p2DRender->TextOut( 5,  5, _T( "Clockworks Server List" ) );
}

void CWndSelectServer::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();

	//m_wndURL.Create( g_Neuz.GetSafeHwnd(), 0, CRect( 30, 220, 230, 240 ), this, 1000);
	//m_wndSearch.Create( _T( "Search" ), 0,  CRect( 235, 220, 290, 240 ), this, 100 );

	//m_wndServerList.Create( 0, CRect( rect.left + 5, rect.top + 40, rect.right - 5, rect.bottom - 35), this, 200 );


	CWndButton* pNext = (CWndButton*)GetDlgItem(WIDC_NEXT);
	pNext->SetDefault(TRUE);

	CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_CONTROL0);
	CRect ReRect = pWndList->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndList->SetWndRect(ReRect);

	CWndListBox* pWndListMulti = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);
	ReRect = pWndListMulti->GetWindowRect(TRUE);
	ReRect.bottom -= 5;
	pWndListMulti->SetWndRect(ReRect);

	pWndListMulti->m_nFontColor = 0xff000000;
	pWndListMulti->m_nSelectColor = 0xff0000ff;

	//pWndList->AddWndStyle( WBS_NODRAWFRAME );
	pWndList->m_nFontColor = 0xff000000;
	pWndList->m_nSelectColor = 0xff0000ff;

	int x = m_rectClient.Width() / 2;
	int y = m_rectClient.Height() - 30;
	CSize size = CSize(70, 25);//m_pSprPack->GetAt(9)->GetSize();

	CRect rect1_1(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);

	CRect rect2_1(x - size.cx - 10, y, (x - size.cx - 10) + size.cx, y + size.cy);
	CRect rect2_2(x + 10, y, (x + 10) + size.cx, y + size.cy);

	CRect rect3_1(x - (size.cx / 2) - size.cx - 10, y, (x - (size.cx / 2) - size.cx - 10) + size.cx, y + size.cy);
	CRect rect3_2(x - (size.cx / 2), y, (x - (size.cx / 2)) + size.cx, y + size.cy);
	CRect rect3_3(x + (size.cx / 2) + 10, y, (x + (size.cx / 2) + 10) + size.cx, y + size.cy);

	//	m_wndServerList.Create( WLVS_REPORT, CRect( rect.left + 5, rect.top + 20, rect.right - 5, rect.bottom - 65), this, 11 );

	TCHAR szTitle[3][10] = { _T("Server"), _T("Ping"), _T("Max") };

	BOOL bSeveServer = FALSE;
	for (int j = 0; j < (int)(g_dpCertified.m_dwSizeofServerset); j++)
	{
		char lpString[MAX_PATH] = { 0, };
		char lpStrtmp[32] = { 0, };
		long lCount = 0;
		long lMax = 0;
		if (g_dpCertified.m_aServerset[j].dwParent == NULL_ID)
		{
			if (g_dpCertified.m_aServerset[j].lEnable != 0L)
			{
				int nIndex = pWndList->AddString(g_dpCertified.m_aServerset[j].lpName);
				pWndList->SetItemData(nIndex, (DWORD)&g_dpCertified.m_aServerset[j]);

				if (nIndex == g_Option.m_nSer)
				{
					bSeveServer = TRUE;
					pWndListMulti->ResetContent();
				}
			}
		}
		else if (g_dpCertified.m_aServerset[j].lEnable != 0L)
		{
			if (pWndList->GetCount() > 0)
			{
				LPSERVER_DESC pServerDesc;
				if (bSeveServer)
				{
					pServerDesc = (LPSERVER_DESC)pWndList->GetItemData(g_Option.m_nSer);
				}
				else
				{
					pServerDesc = (LPSERVER_DESC)pWndList->GetItemData(0);
				}
				if (g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID)
				{
					lCount = g_dpCertified.m_aServerset[j].lCount;
					lMax = g_dpCertified.m_aServerset[j].lMax;

					long lBusy = (long)(lMax * 0.8);

					if (lCount < lBusy)
					{

						strcpy(lpStrtmp, prj.GetText(TID_GAME_NORMAL));
					}
					else
						if (lCount < lMax)
						{

							strcpy(lpStrtmp, prj.GetText(TID_GAME_BUSY));
						}
						else
						{
							strcpy(lpStrtmp, prj.GetText(TID_GAME_FULL));
						}
					sprintf(lpString, "%s(%s)", g_dpCertified.m_aServerset[j].lpName, lpStrtmp);
					//					sprintf( lpString, "%s(%d)", g_dpCertified.m_aServerset[j].lpName, g_dpCertified.m_aServerset[j].lCount );
					int nIndex = pWndListMulti->AddString(lpString);
					pWndListMulti->SetItemData(nIndex, (DWORD)&g_dpCertified.m_aServerset[j]);
				}
			}
		}
	}
	if (pWndListMulti->GetCount())
		pWndListMulti->SetCurSel(0);

	if (pWndList->GetCount())
		pWndList->SetCurSel(0);

	if (bSeveServer == FALSE)
	{
		g_Option.m_nSer = 0;
		g_Option.m_nMSer = 0;
	}
	else
	{
		if (g_Option.m_nMSer >= pWndListMulti->GetCount())
		{
			g_Option.m_nMSer = 0;
		}
	}
	if (pWndList->GetCount() > 0)
		pWndList->SetCurSel(g_Option.m_nSer);
	if (pWndListMulti->GetCount() > 0)
		pWndListMulti->SetCurSel(g_Option.m_nMSer);

	//	if( ::GetLanguage() == LANG_TWN )
	//	{
	//		CWndButton* pWndBack = (CWndButton*)GetDlgItem( WIDC_BACK );
	//		pWndBack->EnableWindow( FALSE );
	//	}

	MoveParentCenter();

	if (::GetLanguage() == LANG_JAP)
	{
		CRect rect2 = m_pWndRoot->GetLayoutRect();

		int width = (rect2.right - rect2.left) / 2;

		Move(width, m_rectWindow.top);
	}

	pNext->SetFocus();

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

	if (nCount > 0)
	{
		SAFE_DELETE(m_atexPannel);
		m_atexPannel = new IMAGE;
		LoadImage(MakePath(DIR_THEME, m_vecStrBanner[xRandom(nCount)]), m_atexPannel);
		AdjustWndBase();
	}
	///////////////////////////////////////////////////////////////////////////////////
}
BOOL CWndSelectServer::Process()
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
BOOL CWndSelectServer::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SELECT_SERVER, WBS_KEY, CPoint(0, 0), pWndParent);
}

void CWndSelectServer::AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat)
{
	CPoint pt;
	LPWNDCTRL lpWndCtrl;

	CPoint pt2 = m_rectClient.TopLeft() - m_rectWindow.TopLeft();

	lpWndCtrl = GetWndCtrl(WIDC_COMMER_BANNER2);
	pt = lpWndCtrl->rect.TopLeft() + pt2;

	if (m_atexPannel)
		PaintTexture(pDest, m_atexPannel, pt, size);
}

void CWndSelectServer::Connected()
{
	DWORD dwSafeTime = 0;
	do
	{
		++dwSafeTime;
		Sleep(100);
	} while ((FALSE == prj.m_bLoadedMoverProp || FALSE == prj.m_bLoadedItemProp) && dwSafeTime < 2000);

	g_WndMng.CloseMessageBox();
	g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_SELECT_CHAR);
	Destroy();
}

void CWndSelectServer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWndListBox* pWndListServer = (CWndListBox*)GetDlgItem(WIDC_CONTROL0);
	CWndListBox* pWndListMulti = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);

	if (nChar == VK_UP)
	{
		DWORD dwIndex = pWndListMulti->GetCurSel();

		if (dwIndex > 0)
			pWndListMulti->SetCurSel(--dwIndex);
	}
	else
		if (nChar == VK_DOWN)
		{
			DWORD dwIndex = pWndListMulti->GetCurSel();

			if ((int)(dwIndex) < pWndListMulti->GetCount() - 1)
				pWndListMulti->SetCurSel(++dwIndex);
		}
}
BOOL CWndSelectServer::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_SELCHANGE)
	{
		switch (nID)
		{
		case 11:
		{
			TCHAR szTemp[32];// =  _T( "nnp://" );
			_tcscpy(szTemp, m_wndServerList.GetItemText(m_wndServerList.GetCurSel(), 0));
			m_wndURL.SetString(szTemp);
			break;
		}
		case 188:
		{
			char lpString[MAX_PATH] = { 0, };
			char lpStrtmp[32] = { 0, };
			long lCount = 0;
			long lMax = 0;
			CWndListBox* pWndListServer = (CWndListBox*)GetDlgItem(WIDC_CONTROL0);
			CWndListBox* pWndListMulti = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);

			pWndListMulti->ResetContent();
			LPSERVER_DESC pServerDesc = (LPSERVER_DESC)pWndListServer->GetItemData(pWndListServer->GetCurSel());
			for (int j = 0; j < (int)(g_dpCertified.m_dwSizeofServerset); j++)
			{
				if (g_dpCertified.m_aServerset[j].dwParent == pServerDesc->dwID && (g_dpCertified.m_aServerset[j].lEnable != 0L))
				{
					lCount = g_dpCertified.m_aServerset[j].lCount;
					lMax = g_dpCertified.m_aServerset[j].lMax;

					long lBusy = (long)(lMax * 0.8);

					if (lCount < lBusy)
						strcpy(lpStrtmp, prj.GetText(TID_GAME_NORMAL));
					else if (lCount < lMax)
						strcpy(lpStrtmp, prj.GetText(TID_GAME_BUSY));
					else
						lstrcpy(lpStrtmp, prj.GetText(TID_GAME_FULL));

					sprintf(lpString, "%s(%s)", g_dpCertified.m_aServerset[j].lpName, lpStrtmp);
					int nIndex = pWndListMulti->AddString(lpString);
					pWndListMulti->SetItemData(nIndex, (DWORD)&g_dpCertified.m_aServerset[j]);
				}
			}
			if (pWndListMulti->GetCount())
				pWndListMulti->SetCurSel(0);
			break;
		}
		}
	}
	else
		switch (nID)
		{
		case 10000: // close msg
		case WIDC_BACK: // Back
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
			Destroy();
			g_dpCertified.DeleteDPObject();
			break;
		case WIDC_NEXT: // accept
		{
			CWndListBox* pWnd = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);
			if (pWnd->GetCount() <= 0)
				break;
			LPSERVER_DESC pDesc = (LPSERVER_DESC)pWnd->GetItemData(pWnd->GetCurSel());
			if (pDesc)
			{
				if (pDesc->lCount > pDesc->lMax)
				{
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0041)));
					break;
				}
			}
		}

		if (FALSE == g_dpCertified.IsConnected())
		{
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
			Destroy();
			g_dpCertified.DeleteDPObject();
			g_dpLoginClient.DeleteDPObject();	// 2004^04^19
			break;
		}
		g_WndMng.OpenCustomBox(NULL, new CWndConnectingBox);

		CWndListBox* pWndList = (CWndListBox*)GetDlgItem(WIDC_CONTROL0);
		LPSERVER_DESC pServerDesc = (LPSERVER_DESC)pWndList->GetItemData(pWndList->GetCurSel());

		g_Option.m_nSer = pWndList->GetCurSel();
		g_Neuz.m_dwSys = pServerDesc->dwID;
		LPCSTR lpAddr = pServerDesc->lpAddr;
		pWndList = (CWndListBox*)GetDlgItem(WIDC_CONTROL1);
		pServerDesc = (LPSERVER_DESC)pWndList->GetItemData(pWndList->GetCurSel());
		g_Option.m_nMSer = pWndList->GetCurSel();
		g_Neuz.m_uIdofMulti = pServerDesc->dwID;
		g_Neuz.m_b18Server = pServerDesc->b18;


		if (pServerDesc->dwParent != g_Neuz.m_dwSys)
		{
			CWndListBox* pWndListBox = (CWndListBox*)GetDlgItem(WIDC_CONTROL0);
			for (int i = 0; i < pWndListBox->GetCount(); i++)
			{
				LPSERVER_DESC ptr = (LPSERVER_DESC)pWndListBox->GetItemData(i);
				if (ptr && ptr->dwID == pServerDesc->dwParent)
				{
					pWndListBox->SetCurSel(i);
					g_Option.m_nSer = i;
					g_Neuz.m_dwSys = ptr->dwID;
					lpAddr = ptr->lpAddr;
					break;
				}
			}
		}

#		ifdef __CRC
		if (!g_dpLoginClient.ConnectToServer(lpAddr, PN_LOGINSRVR, TRUE, CSock::crcWrite))
#		else	__CRC
		if (!g_dpLoginClient.ConnectToServer(lpAddr, PN_LOGINSRVR, TRUE))
#		endif	// __CRC
		{
			// Can't connect to server
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0043)));
			break;
		}

		g_dpLoginClient.QueryTickCount();
		g_dpLoginClient.SendGetPlayerList(pServerDesc->dwID, g_Neuz.m_szAccount, g_Neuz.m_szPassword);
		break;
		}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

BOOL CWndSelectServer::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{

	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndSelectServer::OnSize(UINT nType, int cx, int cy)
{
	/*
	CRect rect = GetClientRect();
	rect.bottom = rect.bottom - 40; //20;
	rect.right -= 50;
	rect.DeflateRect( 1, 1 );
	m_wndText.SetWndRect( rect );

	rect = GetClientRect();
	rect.top = rect.bottom - 37; //20;
	rect.right -= 50;
	rect.DeflateRect( 1, 1 );
	m_wndChat.SetWndRect( rect );

	rect = GetClientRect();
	rect.left = rect.right - 47; rect.right -= 3;	rect.top += 3; rect.bottom = rect.top + 20;
	m_wndLogin.SetWndRect( rect ); rect.OffsetRect( 0, 25 );
	m_wndRegist.SetWndRect( rect ); rect.OffsetRect( 0, 25 );
	m_wndQuit.SetWndRect( rect );
*/
	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndSelectServer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsWndRoot())
		return;

	//if(IsWndStyle(WBS_CAPTION) && m_bPickup)
	{
		//	m_wndTitleBar.m_wndMinimize.SetVisible(TRUE);
			//m_wndTitleBar.m_wndMaximize.SetVisible(TRUE);
	}
}
void CWndSelectServer::OnLButtonDown(UINT nFlags, CPoint point)
{
	//	CWndBase::OnLButtonDown(nFlags,point
	if (IsWndRoot())
		return;
	//	return;

}

///////////////////////////////////////////////////////////////////////////////
// Delete Character
///////////////////////////////////////////////////////////////////////////////

CWndDeleteChar::CWndDeleteChar()
{
}
CWndDeleteChar::~CWndDeleteChar()
{
}
void CWndDeleteChar::OnDraw(C2DRender* p2DRender)
{
}
void CWndDeleteChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pWndEdit)
		pWndEdit->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_READONLY | EBS_NUMBER);

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
	if (pWndButton)
	{
		pWndButton->SetDefault(TRUE);
		pWndButton->EnableWindow(FALSE);
	}

	MoveParentCenter();
}

void CWndDeleteChar::AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat)
{
	CWndNeuz::AdditionalSkinTexture(pDest, sizeSurface, d3dFormat);
}

BOOL CWndDeleteChar::Initialize(CWndBase* pWndParent, DWORD dwWndId)
{
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_DELETE_CHAR, WBS_MODAL, CPoint(0, 0), pWndParent);
}

BOOL CWndDeleteChar::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}

void CWndDeleteChar::OnSize(UINT nType, int cx, int cy) \
{
	CWndNeuz::OnSize(nType, cx, cy);
}

void CWndDeleteChar::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CWndDeleteChar::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndDeleteChar::DeletePlayer(int nSelect, LPCTSTR szNo)
{
	g_dpLoginClient.SendDeletePlayer(nSelect, szNo);
	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndButton->EnableWindow(FALSE);
	pWndButton = (CWndButton*)GetDlgItem(WIDC_CANCEL);
	pWndButton->EnableWindow(FALSE);
}

BOOL CWndDeleteChar::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (nID == WIDC_OK)
	{
		CWndSelectChar* pWnd = (CWndSelectChar*)g_WndMng.GetWndBase(APP_SELECT_CHAR);
		if (pWnd == NULL)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		if (pWndEdit == NULL)
			return CWndNeuz::OnChildNotify(message, nID, pLResult);

		BOOL bOK = FALSE;
		LPCTSTR szNo = pWndEdit->GetString();
		if (strlen(szNo) == 4)
		{
			BOOL bisdigit = TRUE;
			for (int i = 0; i < 4; i++)
			{
				if ((isdigit2(szNo[i]) == FALSE))
				{
					bisdigit = FALSE;
					break;
				}
			}

			if (bisdigit)
			{
				DeletePlayer(pWnd->m_nSelectCharacter, szNo);
				bOK = TRUE;
			}
		}


		if (bOK == FALSE)
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0044)));
	}
	else if (nID == WIDC_CANCEL)
	{
		Destroy();
	}
	switch (nID)
	{
	case WIDC_BUTTON1:
	{
		InsertPassword(0);
		break;
	}
	case WIDC_BUTTON2:
	{
		InsertPassword(1);
		break;
	}
	case WIDC_BUTTON3:
	{
		InsertPassword(2);
		break;
	}
	case WIDC_BUTTON4:
	{
		InsertPassword(3);
		break;
	}
	case WIDC_BUTTON5:
	{
		InsertPassword(4);
		break;
	}
	case WIDC_BUTTON6:
	{
		InsertPassword(5);
		break;
	}
	case WIDC_BUTTON7:
	{
		InsertPassword(6);
		break;
	}
	case WIDC_BUTTON8:
	{
		InsertPassword(7);
		break;
	}
	case WIDC_BUTTON9:
	{
		InsertPassword(8);
		break;
	}
	case WIDC_BUTTON10:
	{
		InsertPassword(9);
		break;
	}
	case WIDC_BUTTON11:
	{
		CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
		if (pWndEdit)
			pWndEdit->Empty();

		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
		if (pWndButton)
			pWndButton->EnableWindow(FALSE);
		break;
	}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndDeleteChar::InsertPassword(int nNumber)
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem(WIDC_EDIT1);
	if (pWndEdit && strlen(pWndEdit->GetString()) < 4)
	{
		CString strNumber;
		strNumber.Format("%d", nNumber);
		pWndEdit->AddString(strNumber);
	}

	if (strlen(pWndEdit->GetString()) == 4)
	{
		CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_OK);
		pWndButton->EnableWindow(TRUE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Select Character
///////////////////////////////////////////////////////////////////////////////

int CWndSelectChar::m_nSelectCharacter = 0;

CWndSelectChar::CWndSelectChar()
{
	m_pWndDeleteChar = NULL;
	m_dwMotion[0] = MTI_SITSTAND;
	m_dwMotion[1] = MTI_SITSTAND;
	m_dwMotion[2] = MTI_SITSTAND;
	SetPutRegInfo(FALSE);
	m_CreateApply = TRUE;

	m_bFlash = TRUE;
	m_nFlashAlpha = 0;
	m_nChars = 0;
}
CWndSelectChar::~CWndSelectChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
	SAFE_DELETE(m_pWndDeleteChar);
}

void CWndSelectChar::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!g_Neuz.m_timerConnect.Over())
		return;

	int nSelectCharBuf = m_nSelectCharacter;
	if (nChar == VK_LEFT)
	{
		--nSelectCharBuf;
		if (0 > nSelectCharBuf)
		{
			nSelectCharBuf = MAX_CHARACTER_LIST - 1;
		}
		SelectCharacter(nSelectCharBuf);
	}
	else if (nChar == VK_RIGHT)
	{
		++nSelectCharBuf;
		if (nSelectCharBuf >= MAX_CHARACTER_LIST)
		{
			nSelectCharBuf = 0;
		}
		SelectCharacter(nSelectCharBuf);
	}
}

void CWndSelectChar::OnDestroyChildWnd(CWndBase* pWndChild)
{
	if (pWndChild == m_pWndDeleteChar)
		SAFE_DELETE(m_pWndDeleteChar);
}
HRESULT CWndSelectChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	return S_OK;
}
HRESULT CWndSelectChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndSelectChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndSelectChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return S_OK;
}
BOOL CWndSelectChar::Process()
{
	if (m_bFlash == TRUE)
	{
		m_nFlashAlpha += 4;
		if (m_nFlashAlpha > 150)
		{
			m_nFlashAlpha = 150;
			m_bFlash = FALSE;
		}
	}
	else
	{
		m_nFlashAlpha -= 4;
		if (m_nFlashAlpha < 25)
		{
			m_nFlashAlpha = 25;
			m_bFlash = TRUE;
		}
	}

	m_nChars = 0;

	if (g_Neuz.m_apPlayer[0] != NULL)
		m_nChars++;
	if (g_Neuz.m_apPlayer[1] != NULL)
		m_nChars++;
	if (g_Neuz.m_apPlayer[2] != NULL)
		m_nChars++;

	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		CMover* pMover = g_Neuz.m_apPlayer[i];
		if (!pMover)
			continue;

		if (!pMover->m_pModel)
			continue;

		CRect rect = m_aRect[i];

		int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
		if (m_nSelectCharacter == i)
		{

			if (m_dwMotion[i] == MTI_GETUP)
			{
				if (pMover->m_pModel->IsEndFrame() && pMover->m_pModel->m_nLoop == ANILOOP_1PLAY)
				{
					SetMotion((CModelObject*)pMover->m_pModel, nMover, MTI_STAND, ANILOOP_LOOP, 0);
					m_dwMotion[i] = MTI_STAND;
				}
			}
		}
		else
		{

			if (m_dwMotion[i] != MTI_SITSTAND)
			{
				if (pMover->m_pModel->IsEndFrame() && pMover->m_pModel->m_nLoop == ANILOOP_1PLAY)
				{
					SetMotion((CModelObject*)pMover->m_pModel, nMover, MTI_SITSTAND, ANILOOP_LOOP, 0);
					m_dwMotion[i] = MTI_SITSTAND;
				}
				else
					if (m_dwMotion[i] != MTI_SIT)
					{
						SetMotion((CModelObject*)pMover->m_pModel, nMover, MTI_SIT, ANILOOP_1PLAY, 0);
						m_dwMotion[i] = MTI_SIT;
					}
			}

		}
		pMover->m_pModel->FrameMove();
	}

#ifndef _DEBUG
	if (g_Neuz.m_dwTimeOutDis < GetTickCount())
	{
		g_Neuz.m_dwTimeOutDis = 0xffffffff;
		g_dpCertified.DeleteDPObject();
		g_dpLoginClient.DeleteDPObject();
		g_DPlay.DeleteDPObject();

		g_WndMng.OpenMessageBoxUpper(_T(prj.GetText(TID_DIAG_0043)));

		m_bDisconnect = TRUE;
	}

	if (g_WndMng.m_pWndMessageBoxUpper == NULL && m_bDisconnect)
	{
		Destroy();
		g_WndMng.CloseMessageBox();
		g_dpCertified.SendCertify();
		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
		return 0;
	}
#endif //_DEBUG

	return CWndNeuz::Process();
}

void CWndSelectChar::OnDraw(C2DRender* p2DRender)
{
	CWndButton* pWndAccept = (CWndButton*)GetDlgItem(WIDC_ACCEPT);
	CWndButton* pWndCreate = (CWndButton*)GetDlgItem(WIDC_CREATE);
	CWndButton* pWndDelete = (CWndButton*)GetDlgItem(WIDC_DELETE);

	CWndStatic* pWndStaticChar[4];
	pWndStaticChar[1] = (CWndStatic*)GetDlgItem(WIDC_ST_CHARNAME01);
	pWndStaticChar[2] = (CWndStatic*)GetDlgItem(WIDC_ST_CHARNAME02);
	pWndStaticChar[3] = (CWndStatic*)GetDlgItem(WIDC_ST_CHARNAME03);

	LPWNDCTRL lpCustom[3];
	lpCustom[0] = GetWndCtrl(WIDC_CUSTOM1);
	lpCustom[1] = GetWndCtrl(WIDC_CUSTOM2);
	lpCustom[2] = GetWndCtrl(WIDC_CUSTOM3);

	LPWNDCTRL lpStatic[3];
	lpStatic[0] = GetWndCtrl(WIDC_ST_CHARNAME01);
	lpStatic[1] = GetWndCtrl(WIDC_ST_CHARNAME02);
	lpStatic[2] = GetWndCtrl(WIDC_ST_CHARNAME03);

	CRect rect;
	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		rect = m_aRect[i];

		if (g_Neuz.m_apPlayer[i] != NULL)
		{
			if (g_WndMng.GetWndBase(APP_2ND_PASSWORD_NUMBERPAD) == NULL)
			{
				POINT point = GetMousePoint();
				if (m_aRect[i].PtInRect(point))
				{
					CRect rectHittest = m_aRect[i];
					CPoint point2 = point;
					ClientToScreen(&point2);
					ClientToScreen(rectHittest);
					g_WndMng.PutToolTip_Character(i, point2, &rectHittest);
				}
			}
			if (m_nSelectCharacter == i)
			{
				CRect rectNew;
				CRect rectFlash;

				rectNew.top = lpCustom[i]->rect.top;
				rectNew.left = lpCustom[i]->rect.left;
				rectNew.bottom = lpCustom[i]->rect.bottom + 2;
				rectNew.right = lpCustom[i]->rect.right;

				rectFlash.top = lpStatic[i]->rect.top + 3;
				rectFlash.left = lpStatic[i]->rect.left + 2;
				rectFlash.bottom = lpStatic[i]->rect.bottom - 3;
				rectFlash.right = lpStatic[i]->rect.right - 2;

				p2DRender->RenderFillRect(rectNew, D3DCOLOR_ARGB(15, 64, 51, 154));
				p2DRender->RenderFillRect(rectFlash, D3DCOLOR_ARGB(m_nFlashAlpha, 109, 80, 255));

				pWndStaticChar[i + 1]->SetTitle(g_Neuz.m_apPlayer[i]->GetName());

				switch (i)
				{
				case 0: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 1: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 2: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				}
			}
			else
				pWndStaticChar[i + 1]->SetTitle(g_Neuz.m_apPlayer[i]->GetName());

			if (!g_Neuz.m_apPlayer[i]->m_pModel)
				continue;

			VIEWPORT tmp;
			tmp.m_fFov = 3.5f;
			tmp.vecMatrixScaling = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
			tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -0.6f, 0.0f);

			g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)g_Neuz.m_apPlayer[i]->m_pModel, g_Neuz.m_apPlayer[i], lpCustom[i], &tmp, GetWndId());
		}
		else
			pWndStaticChar[i + 1]->SetTitle("");

		if (m_nSelectCharacter == i)
		{
			if (g_Neuz.m_apPlayer[i])
			{
				pWndCreate->EnableWindow(FALSE);
				pWndAccept->EnableWindow(TRUE);
				pWndDelete->EnableWindow(TRUE);
			}
			else
			{
				pWndCreate->EnableWindow(TRUE);
				pWndAccept->EnableWindow(FALSE);
				pWndDelete->EnableWindow(FALSE);

				switch (i)
				{
				case 0: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 1: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				case 2: pWndStaticChar[1]->m_dwColor = 0xFF666666, pWndStaticChar[2]->m_dwColor = 0xFF666666, pWndStaticChar[3]->m_dwColor = 0xFF666666; break;
				}
			}
		}
	}
}
void CWndSelectChar::UpdateCharacter()
{
	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		CMover* pMover = g_Neuz.m_apPlayer[i];
		if (pMover)
		{
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			pMover->m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);
			if (i == m_nSelectCharacter)
			{
				prj.m_modelMng.LoadMotion((CModelObject*)pMover->m_pModel, OT_MOVER, nMover, MTI_STAND);
				m_dwMotion[i] = MTI_STAND;
			}
			else
			{
				prj.m_modelMng.LoadMotion((CModelObject*)pMover->m_pModel, OT_MOVER, nMover, MTI_SITSTAND);
				m_dwMotion[i] = MTI_SITSTAND;
			}
			CMover::UpdateParts(pMover->GetSex(), pMover->m_dwHairMesh, pMover->m_dwHeadMesh, pMover->m_aEquipInfo, (CModelObject*)pMover->m_pModel, NULL/*&pMover->m_Inventory*/);
		}
	}
}
void CWndSelectChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	CRect rect = GetClientRect();

	LPWNDCTRL lpText1 = GetWndCtrl(WIDC_CUSTOM1);
	LPWNDCTRL lpText2 = GetWndCtrl(WIDC_CUSTOM2);
	LPWNDCTRL lpText3 = GetWndCtrl(WIDC_CUSTOM3);

	LPWNDCTRL lpCustom[3];
	lpCustom[0] = GetWndCtrl(WIDC_CUSTOM1);
	lpCustom[1] = GetWndCtrl(WIDC_CUSTOM2);
	lpCustom[2] = GetWndCtrl(WIDC_CUSTOM3);

	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem(WIDC_ST_ACCOUNTNAME);
	CString str;
	str.Format("Account: %s", g_Neuz.m_szAccount);
	pWndStatic->SetTitle(str);

	CWndButton* pWndButton = (CWndButton*)GetDlgItem(WIDC_DELETE);
	CWndButton* pWndAccept = (CWndButton*)GetDlgItem(WIDC_ACCEPT);
	pWndAccept->SetDefault(TRUE);
	CWndButton* pWndBack = (CWndButton*)GetDlgItem(WIDC_BACK);
	pWndBack->SetFocus();
	m_bDisconnect = FALSE;

	rect = CRect(16, 16, 174, 254);
	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
		m_aRect[i] = lpCustom[i]->rect;

	MoveParentCenter();
}

BOOL CWndSelectChar::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	//CRect rect = m_pWndRoot->MakeCenterRect(250,130);
	CRect rect = m_pWndRoot->MakeCenterRect(590, 400);
	SetTitle(_T("Select Character"));
	//return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_THICKFRAME | WBS_MAXIMIZEBOX, rect, pWndParent, APP_SELECT_CHAR );
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_SELECT_CHAR, WBS_KEY, CPoint(0, 0), pWndParent);

	//return CWndNeuz::Create(dwStyle|WBS_MOVE|WBS_SOUND|WBS_CAPTION|WBS_THICKFRAME|WBS_MAXIMIZEBOX,rect,pWndParent,10);
}

void CWndSelectChar::Connected()
{
	g_Neuz.WaitLoading();

	if (m_nSelectCharacter < 0 || m_nSelectCharacter >= MAX_CHARACTER_LIST)
	{
		LPCTSTR szErr = Error("CWndSelectChar::Connected : 범위초과 %d", m_nSelectCharacter);
		int* p = NULL;
		*p = 1;
	}

	g_dpLoginClient.SendPreJoin(g_Neuz.m_szAccount, g_Neuz.m_apPlayer[m_nSelectCharacter]->m_idPlayer, g_Neuz.m_apPlayer[m_nSelectCharacter]->GetName(), m_nSelectCharacter);

	if (::GetLanguage() == LANG_ENG && ::GetSubLanguage() == LANG_SUB_USA)
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC(30);
	else
		g_Neuz.m_dwTimeOutDis = GetTickCount() + SEC(15);
}

BOOL CWndSelectChar::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (!g_Neuz.m_timerConnect.Over())
		return CWndNeuz::OnChildNotify(message, nID, pLResult);

	switch (nID)
	{
	case 10000: // close msg
	case WIDC_BACK: // Back
	{
#ifdef __CERTIFIER_COLLECTING_SYSTEM
		DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM
		g_dpLoginClient.DeleteDPObject();
		Sleep(1000);
#			ifdef __CRC
		if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE, CSock::crcWrite))
#			else	// __CRC
		if (!g_dpCertified.ConnectToServer(g_Neuz.m_lpCertifierAddr, PN_CERTIFIER, TRUE))
#			endif	// __CRC
		{
			// Can't connect to server
			g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0043)));
			break;
		}
		Destroy();
		g_dpCertified.SendCertify();

		g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
		CWndBase* pWndBase = g_WndMng.GetWndBase(APP_LOGIN);
		//				pWndBase->SetVisible( FALSE );
		break;
	}
	case WIDC_CREATE: // Create
		if (m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter] == NULL)
		{
			if (m_CreateApply)
			{
				u_short uSlot = (u_short)m_nSelectCharacter;
				Destroy();
				g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_CREATE_CHAR);
				CWndCreateChar* pWndCreateChar = (CWndCreateChar*)g_WndMng.GetWndBase(APP_CREATE_CHAR);
				if (pWndCreateChar) {
					pWndCreateChar->m_Player.m_nSlot = uSlot;
				}
			}
			else
			{

			}
		}
		break;
	case WIDC_DELETE: // Delete
		if (m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter])
		{
			SAFE_DELETE(m_pWndDeleteChar);
			m_pWndDeleteChar = new CWndDeleteChar;
			m_pWndDeleteChar->Initialize(this);
		}
		break;
	case WIDC_ACCEPT: // Accept
	{
		CWndButton* pWndAccept = (CWndButton*)GetDlgItem(WIDC_ACCEPT);
		pWndAccept->EnableWindow(FALSE);
	}
#ifdef __CERTIFIER_COLLECTING_SYSTEM
	DPCollectClient->DeleteDPObject();
#endif // __CERTIFIER_COLLECTING_SYSTEM

	if (g_Neuz.m_nCharacterBlock[m_nSelectCharacter] == 0)
	{
		g_WndMng.OpenCustomBox(NULL, new CWndCharBlockBox);
	}
	else
	{
		if (FALSE == g_dpLoginClient.IsConnected())
		{
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_LOGIN);
			Destroy();
			g_dpLoginClient.DeleteDPObject();
			break;
		}

		g_WndMng.OpenCustomBox(_T(prj.GetText(TID_DIAG_0064)), new CWndConnectingBox);

		if (g_DPlay.Connect(g_Neuz.m_lpCacheAddr, g_Neuz.m_uCachePort))
		{
			if (m_nSelectCharacter != -1 && g_Neuz.m_apPlayer[m_nSelectCharacter])
			{
				g_Neuz.m_dwTempMessage = 1;
				g_Neuz.m_timerConnect.Set(SEC(1));
			}
		}
	}
	break;
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndSelectChar::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndSelectChar::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType, cx, cy);
}
BOOL CWndSelectChar::SetMotion(CModelObject* pModel, DWORD dwIndex, DWORD dwMotion, int nLoop, DWORD dwOption)
{
	DWORD	dwOrigMotion = dwMotion;
	prj.m_modelMng.LoadMotion(pModel, OT_MOVER, dwIndex, dwMotion);
	pModel->m_bEndFrame = FALSE;
	pModel->SetLoop(nLoop);
	if (dwOption & MOP_NO_TRANS) pModel->SetMotionBlending(FALSE);
	else pModel->SetMotionBlending(TRUE);

	return TRUE;
}
void CWndSelectChar::SelectCharacter(int i)
{
	if (m_nSelectCharacter != i)
	{
		m_nSelectCharacter = i;
		CMover* pMover = g_Neuz.m_apPlayer[i];
		if (pMover)
		{
			int nMover = (pMover->GetSex() == SEX_MALE ? MI_MALE : MI_FEMALE);
			CModelObject* pModel = (CModelObject*)pMover->m_pModel;
			if (pModel)
				SetMotion(pModel, nMover, MTI_GETUP, ANILOOP_1PLAY, 0);

			m_dwMotion[i] = MTI_GETUP;
		}
	}
}
void CWndSelectChar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!g_Neuz.m_timerConnect.Over())
		return;

	for (int i = 0; i < MAX_CHARACTER_LIST; i++)
	{
		if ( /*m_pBipedMesh[ i ] &&*/ m_aRect[i].PtInRect(point))
		{
			if (g_Neuz.m_nCharacterBlock[i] == 0)
			{
				g_WndMng.OpenCustomBox(NULL, new CWndCharBlockBox);

			}
			else
			{
				SelectCharacter(i);
			}
		}
	}
}
//pMover->SetMotion( MTI_STAND );


void CWndSelectChar::OnLButtonDown(UINT nFlags, CPoint point)
{
}

///////////////////////////////////////////////////////////////////////////////
// Select Character
///////////////////////////////////////////////////////////////////////////////
CWndCreateChar::CWndCreateChar()
{
	SetPutRegInfo(FALSE);
	memset(m_ColorRect, 0, sizeof(CRect) * 3);
	memset(m_fColor, 0, sizeof(FLOAT) * 3);
	m_bLButtonClick = FALSE;
	m_ChoiceBar = -1;
}
CWndCreateChar::~CWndCreateChar()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}
HRESULT CWndCreateChar::InitDeviceObjects()
{
	CWndBase::InitDeviceObjects();
	if (m_Player.m_pModel)
		m_Player.m_pModel->InitDeviceObjects(m_pApp->m_pd3dDevice);
	return S_OK;
}
HRESULT CWndCreateChar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if (m_Player.m_pModel)
		m_Player.m_pModel->RestoreDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	if (m_Player.m_pModel)
		m_Player.m_pModel->InvalidateDeviceObjects();
	return S_OK;
}
HRESULT CWndCreateChar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	//m_pModel->DeleteDeviceObjects();
	return S_OK;
}
void CWndCreateChar::OnDraw(C2DRender* p2DRender)
{
	if (!m_Player.m_pModel)
		return;

	m_Player.m_pModel->FrameMove();

	LPWNDCTRL lpFace = GetWndCtrl(WIDC_CUSTOM4);
	VIEWPORT tmp;
	tmp.vecLookAt = D3DXVECTOR3(0.0f, 0.0f, 3.0f);
	tmp.vecPos = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	tmp.vecMatrixScaling = D3DXVECTOR3(1.4f, 1.4f, 1.4f);
	tmp.vecMatrixTranslation = D3DXVECTOR3(0.0f, -1.1f, 0.0f);

	m_Player.m_fHairColorR = m_fColor[0];
	m_Player.m_fHairColorG = m_fColor[1];
	m_Player.m_fHairColorB = m_fColor[2];

	g_WndMng.RenderObjectViewport(p2DRender, (CModelObject*)m_Player.m_pModel, &m_Player, lpFace, &tmp, GetWndId());

	LPWNDCTRL lpWndCtrl[3];
	lpWndCtrl[0] = GetWndCtrl(WIDC_CUSTOM);
	lpWndCtrl[1] = GetWndCtrl(WIDC_CUSTOM1);
	lpWndCtrl[2] = GetWndCtrl(WIDC_CUSTOM2);
	CTexture* pTex;
	for (int i = 0; i < 3; i++)
	{
		CString str;
		str.Format("Poloska_%d.bmp", i + 1);
		pTex = CWndBase::m_textureMng.AddTexture(p2DRender->m_pd3dDevice, MakePath(DIR_THEME, str), 0xffff00ff);
		if (pTex)
			p2DRender->RenderTexture(CPoint(lpWndCtrl[i]->rect.left, lpWndCtrl[i]->rect.top), pTex);
		CPoint pt = CPoint(m_ColorScrollBar[i].x - (m_Texture.m_size.cx / 2), m_ColorScrollBar[i].y);
		m_Texture.Render(p2DRender, pt);
	}
}
void CWndCreateChar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	m_Player.m_nJob = JOB_VAGRANT;

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem(WIDC_STATIC7);
	if (pStatic)
		pStatic->EnableWindow(FALSE);

	CWndButton* pWndOk = (CWndButton*)GetDlgItem(WIDC_OK);
	pWndOk->SetDefault(TRUE);


	SetSex(SEX_FEMALE);
	MoveParentCenter();

	CWndEdit* pWndName = (CWndEdit*)GetDlgItem(WIDC_NAME);
	pWndName->SetFocus();

	m_Player.m_dwHairMesh = (char)(xRandom(0, MAX_BASE_HAIR));
	m_Player.m_dwHeadMesh = (char)(xRandom(0, MAX_DEFAULT_HEAD));
	CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);

	CWndStatic* pWnd2ndPasswordText = (CWndStatic*)GetDlgItem(WIDC_STATIC_2ND_PASSWORD_TEXT);
	assert(pWnd2ndPasswordText);

	pWnd2ndPasswordText->m_dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	CWndEdit* pWnd2ndPassword = (CWndEdit*)GetDlgItem(WIDC_EDIT_2ND_PASSWORD);
	assert(pWnd2ndPassword);
	pWnd2ndPassword->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);
	pWnd2ndPassword->SetMaxStringNumber(MAX_2ND_PASSWORD_NUMBER);
	CWndEdit* pWnd2ndPasswordConfirm = (CWndEdit*)GetDlgItem(WIDC_EDIT_2ND_PASSWORD_CONFIRM);
	assert(pWnd2ndPasswordConfirm);
	pWnd2ndPasswordConfirm->AddWndStyle(EBS_PASSWORD | EBS_AUTOHSCROLL | EBS_NUMBER);
	pWnd2ndPasswordConfirm->SetMaxStringNumber(MAX_2ND_PASSWORD_NUMBER);

	CRect rect = GetWindowRect(TRUE);
	m_fColor[0] = (nHairColor[m_Player.m_dwHairMesh][0]) / 255.f;
	m_fColor[1] = (nHairColor[m_Player.m_dwHairMesh][1]) / 255.f;
	m_fColor[2] = (nHairColor[m_Player.m_dwHairMesh][2]) / 255.f;

	LPWNDCTRL lpWndCtrl;
	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM);
	rect = lpWndCtrl->rect;
	m_ColorRect[0].left = rect.left;
	m_ColorRect[0].top = rect.top;
	m_ColorRect[0].right = rect.right;
	m_ColorRect[0].bottom = rect.bottom;
	m_ColorRect[0] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM1);
	rect = lpWndCtrl->rect;
	m_ColorRect[1].left = rect.left;
	m_ColorRect[1].top = rect.top;
	m_ColorRect[1].right = rect.right;
	m_ColorRect[1].bottom = rect.bottom;
	m_ColorRect[1] = rect;

	lpWndCtrl = GetWndCtrl(WIDC_CUSTOM2);
	rect = lpWndCtrl->rect;
	m_ColorRect[2].left = rect.left;
	m_ColorRect[2].top = rect.top;
	m_ColorRect[2].right = rect.right;
	m_ColorRect[2].bottom = rect.bottom;
	m_ColorRect[2] = rect;

	ReSetBar(m_fColor[0], m_fColor[1], m_fColor[2]);
	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), 0xffff00ff, TRUE);
}

void CWndCreateChar::SetSex(int nSex)
{
	m_Player.m_bySex = nSex;

	int nMover = m_Player.m_bySex == SEX_MALE ? MI_MALE : MI_FEMALE;

	SAFE_DELETE(m_Player.m_pModel);
	m_Player.m_pModel = (CModelObject*)prj.m_modelMng.LoadModel(g_Neuz.m_pd3dDevice, OT_MOVER, nMover, TRUE);

	if (nSex == SEX_MALE)
		prj.m_modelMng.LoadMotion(m_Player.m_pModel, OT_MOVER, nMover, MTI_STAND);
	else
		prj.m_modelMng.LoadMotion(m_Player.m_pModel, OT_MOVER, nMover, MTI_STAND2);

	memset(m_Player.m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS);
	{
		for (int i = 0; i < MAX_HUMAN_PARTS; i++)
		{
			m_Player.m_aEquipInfo[i].dwId = NULL_ID;
#ifdef __MODEL_CHANGE
			m_Player.m_aEquipInfo[i].dwLook = NULL_ID;
#endif
		}
	}

	for (int i = 0; i < MAX_BEGINEQUIP; i++)
	{
		DWORD dwEquip = prj.m_jobItem[m_Player.m_nJob].adwMale[i][m_Player.m_bySex];
		if (dwEquip != NULL_ID)
		{
			ItemProp* pItemProp = prj.GetItemProp(dwEquip);
			m_Player.m_aEquipInfo[pItemProp->dwParts].dwId = dwEquip;
#ifdef __MODEL_CHANGE
			m_Player.m_aEquipInfo[pItemProp->dwParts].dwLook = dwEquip;
#endif
		}
	}
	CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);
}

BOOL CWndCreateChar::Initialize(CWndBase* pWndParent, DWORD dwStyle)
{
	CRect rect = m_pWndRoot->MakeCenterRect(590, 400);
	m_Texture.LoadTexture(g_Neuz.GetDevice(), MakePath(DIR_THEME, "yellowbuttten.tga"), 0xffff00ff, TRUE);
	return CWndNeuz::InitDialog(g_Neuz.GetSafeHwnd(), APP_CREATE_CHAR, WBS_KEY, CPoint(0, 0), pWndParent);
}


BOOL CWndCreateChar::OnChildNotify(UINT message, UINT nID, LRESULT* pLResult)
{
	if (message == WNM_CLICKED)
	{
		switch (nID)
		{
		case 10000:
			return FALSE;
		case WIDC_MALE: // male
			SetSex(SEX_MALE);
			break;
		case WIDC_FEMALE: // female
			SetSex(SEX_FEMALE);
			break;
		case WIDC_HAIRSTYLE_LEFT: // hair
			m_Player.m_dwHairMesh--;
			if ((int)m_Player.m_dwHairMesh < 0)
				m_Player.m_dwHairMesh = MAX_BASE_HAIR - 1;
			CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);
			break;
		case WIDC_HAIRSTYLE_RIGHT: // hair
			m_Player.m_dwHairMesh++;
			if ((int)m_Player.m_dwHairMesh >= MAX_BASE_HAIR)
				m_Player.m_dwHairMesh = 0;
			CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);
			break;
		case WIDC_FACE_LEFT: // head
			m_Player.m_dwHeadMesh--;
			if ((int)m_Player.m_dwHeadMesh < 0)
				m_Player.m_dwHeadMesh = MAX_DEFAULT_HEAD - 1;
			CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);
			break;
		case WIDC_FACE_RIGHT: // head
			m_Player.m_dwHeadMesh++;
			if ((int)m_Player.m_dwHeadMesh >= MAX_DEFAULT_HEAD)
				m_Player.m_dwHeadMesh = 0;
			CMover::UpdateParts(m_Player.m_bySex, m_Player.m_dwHairMesh, m_Player.m_dwHeadMesh, m_Player.m_aEquipInfo, (CModelObject*)m_Player.m_pModel, NULL);
			break;

		case WIDC_CANCEL: // Cancel
		{
			Destroy();
			g_WndMng.ObjectExecutor(SHORTCUT_APPLET, APP_SELECT_CHAR);
			CWndSelectChar* pWndSelectChar
				= (CWndSelectChar*)g_WndMng.GetWndBase(APP_SELECT_CHAR);
			if (pWndSelectChar)
				pWndSelectChar->UpdateCharacter();
			break;
		}
		case WIDC_OK: // Create
		{
			CWndEdit* pEdit = (CWndEdit*)GetDlgItem(WIDC_NAME);
			CString string = pEdit->m_string;

			//__FIX_SQL_INJECTS
			if (IsInvalidGameName(pEdit->GetString()))
			{
				g_WndMng.PutString(prj.GetText(TID_DIAG_0013));
				return TRUE;
			}

			CWndEdit* pWnd2ndPassword = (CWndEdit*)GetDlgItem(WIDC_EDIT_2ND_PASSWORD);
			assert(pWnd2ndPassword);
			CWndEdit* pWnd2ndPasswordConfirm = (CWndEdit*)GetDlgItem(WIDC_EDIT_2ND_PASSWORD_CONFIRM);
			assert(pWnd2ndPasswordConfirm);
			if (strcmp(pWnd2ndPassword->GetString(), _T("")) == 0)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR01));
				return TRUE;
			}
			if (static_cast<int>(strlen(pWnd2ndPassword->GetString())) < MAX_2ND_PASSWORD_NUMBER)
			{
				CString strError = _T("");
				strError.Format(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR02), MAX_2ND_PASSWORD_NUMBER);
				g_WndMng.OpenMessageBox(strError);
				return TRUE;
			}
			if (strcmp(pWnd2ndPassword->GetString(), _T("0000")) == 0)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR03));
				return TRUE;
			}
			if (strcmp(pWnd2ndPasswordConfirm->GetString(), _T("")) == 0)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR04));
				return TRUE;
			}
			if (strcmp(pWnd2ndPassword->GetString(), pWnd2ndPasswordConfirm->GetString()) != 0)
			{
				g_WndMng.OpenMessageBox(prj.GetText(TID_2ND_PASSWORD_INPUT_ERROR05));
				return TRUE;
			}

			CWndButton* pButton = (CWndButton*)GetDlgItem(WIDC_OK);
			pButton->EnableWindow(FALSE);
			pButton = (CWndButton*)GetDlgItem(WIDC_CANCEL);
			pButton->EnableWindow(FALSE);
			DWORD dwHairColor = 0xffffffff;

			BYTE nColorR = (BYTE)((m_fColor[0] * 255));
			BYTE nColorG = (BYTE)((m_fColor[1] * 255));
			BYTE nColorB = (BYTE)((m_fColor[2] * 255));
			dwHairColor = D3DCOLOR_ARGB(255, nColorR, nColorG, nColorB);

			g_dpLoginClient.SendCreatePlayer((BYTE)(m_Player.m_nSlot), string, m_Player.m_dwFace, 0, m_Player.m_dwSkinSet, m_Player.m_dwHairMesh, dwHairColor, m_Player.m_bySex, m_Player.m_nJob, m_Player.m_dwHeadMesh, atoi(pWnd2ndPassword->GetString()));
		}
		break;
		case 10002: // Accept
		{
			g_WndMng.OpenCustomBox(NULL, new CWndConnectingBox);
			g_Neuz.m_dwTempMessage = 1;
			g_Neuz.m_timerConnect.Set(1);
		}
		break;
		}
	}
	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
BOOL CWndCreateChar::OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase)
{
	return CWndNeuz::OnCommand(nID, dwMessage, pWndBase);
}
void CWndCreateChar::OnSize(UINT nType, int cx, int cy)
{

	CWndNeuz::OnSize(nType, cx, cy);
}
void CWndCreateChar::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_ChoiceBar = -1;
	m_bLButtonClick = FALSE;
}

void CWndCreateChar::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i = 0; i < 3; i++)
	{
		CRect DrawRect = m_ColorRect[i];
		if (DrawRect.PtInRect(point))
			m_ChoiceBar = i;
	}
	m_bLButtonClick = TRUE;
}

void CWndCreateChar::OnMouseWndSurface(CPoint point)
{
	if (m_ChoiceBar != -1 && m_bLButtonClick)
	{
		CRect DrawRect = m_ColorRect[m_ChoiceBar];
		point.x = (point.x > DrawRect.right) ? DrawRect.right : point.x;
		point.x = (point.x < DrawRect.left) ? DrawRect.left : point.x;
		LONG Width = DrawRect.right - DrawRect.left;
		LONG Pos = point.x - DrawRect.left;
		FLOAT p = ((FLOAT)((FLOAT)Pos / (FLOAT)Width));

#ifdef __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.0f, 1.0f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(1.0f, 1.0f);
#else // __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec1 = D3DXVECTOR2(0.3f, 0.998f);
		D3DXVECTOR2 vec2 = D3DXVECTOR2(0.998f, 0.998f);
#endif // __Y_HAIR_BUG_FIX
		D3DXVECTOR2 vec3;
		D3DXVec2Lerp(&vec3, &vec1, &vec2, p);
		m_fColor[m_ChoiceBar] = vec3.x;
		m_ColorScrollBar[m_ChoiceBar].x = point.x;
	}
}

void CWndCreateChar::ReSetBar(FLOAT r, FLOAT g, FLOAT b)
{
#ifdef __Y_HAIR_BUG_FIX
	FLOAT fR = (r / 1.0f) * 100.0f;
	FLOAT fG = (g / 1.0f) * 100.0f;
	FLOAT fB = (b / 1.0f) * 100.0f;
#else // __Y_HAIR_BUG_FIX
	FLOAT fR = ((r - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fG = ((g - 0.3f) / (1.0f - 0.3f)) * 100.0f;
	FLOAT fB = ((b - 0.3f) / (1.0f - 0.3f)) * 100.0f;
#endif // __Y_HAIR_BUG_FIX

	m_ColorScrollBar[0].x = (LONG)((((m_ColorRect[0].right - m_ColorRect[0].left) * fR) / 100.0f) + m_ColorRect[0].left);
	m_ColorScrollBar[0].y = m_ColorRect[0].top;
	m_ColorScrollBar[1].x = (LONG)((((m_ColorRect[1].right - m_ColorRect[1].left) * fG) / 100.0f) + m_ColorRect[1].left);
	m_ColorScrollBar[1].y = m_ColorRect[1].top;
	m_ColorScrollBar[2].x = (LONG)((((m_ColorRect[2].right - m_ColorRect[2].left) * fB) / 100.0f) + m_ColorRect[2].left);
	m_ColorScrollBar[2].y = m_ColorRect[2].top;
}

