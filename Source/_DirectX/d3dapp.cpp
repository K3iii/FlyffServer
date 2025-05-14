
#include "stdafx.h"
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#include <D3D9.h>
#include "DXUtil.h"
#include "D3DUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DRes.h"
#include "lang.h"

#ifdef __BUGSPLAT
#include "BugSplat.h"
#pragma comment	(lib, "BugSplat.lib")
#endif

#ifdef __MINIDUMP
#include "../WorldServer/MiniDumper.h"
#endif

#ifdef __AIO_NEUZAPP
#include "../../Resource/ResData.h"
#endif

DWORD FULLSCREEN_WIDTH = 800;//1024;
DWORD FULLSCREEN_HEIGHT = 600;

LARGE_INTEGER	g_fFreq;
LARGE_INTEGER	g_llTime, g_llLastTime;

void	InitTimer(void)
{
	QueryPerformanceFrequency(&g_fFreq);
}

void	ResetTimer(void)
{
	QueryPerformanceCounter(&g_llTime);
	g_llLastTime.QuadPart = g_llTime.QuadPart;
}

LONGLONG	GetTimer(void)
{
	LARGE_INTEGER	llCurr;
	QueryPerformanceCounter(&llCurr);
	return llCurr.QuadPart;
}

CD3DApplication* g_pD3DApp = NULL;

CD3DApplication::CD3DApplication()
{
	g_pD3DApp = this;
	m_pD3D = NULL;
	m_pd3dDevice = NULL;
	m_hWnd = NULL;
	m_hWndFocus = NULL;
	m_hMenu = NULL;
	m_bWindowed = true;
	m_bActive = false;
	m_bDeviceLost = false;
	m_bMinimized = false;
	m_bMaximized = false;
	m_bDeviceObjectsInited = false;
	m_bDeviceObjectsRestored = false;
	m_bCurrentlyResizing = false;
	m_dwCreateFlags = 0;
	m_bFrameMoving = true;
	m_bSingleStep = false;
	m_fTime = 0.0f;
	m_fElapsedTime = 0.0f;
	m_fFPS = 0.0f;
	m_strDeviceStats[0] = _T('\0');
	m_strFrameStats[0] = _T('\0');

	m_strWindowTitle = _T("D3D9 Application");
	m_dwCreationWidth = 400;
	m_dwCreationHeight = 300;
	m_bShowCursorWhenFullscreen = false;
	m_dwNextRenderTime = 0;

	Pause(true); // Pause until we're ready to render

	// When m_bClipCursorWhenFullscreen is true, the cursor is limited to
	// the device window when the app goes fullscreen.  This prevents users
	// from accidentally clicking outside the app window on a multimon system.
	// This flag is turned off by default for debug builds, since it makes
	// multimon debugging difficult.
	m_bClipCursorWhenFullscreen = true;

	InitTimer();

	ZeroMemory(&m_PrevDevMode, sizeof(DEVMODE));
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pD3DApp->MsgProc(hWnd, uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: ConfirmDeviceHelper()
// Desc: Static function used by D3DEnumeration
//-----------------------------------------------------------------------------
bool CD3DApplication::ConfirmDeviceHelper(D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType,
	D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat)
{
	DWORD dwBehavior;

	if (vertexProcessingType == SOFTWARE_VP)
		dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if (vertexProcessingType == MIXED_VP)
		dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
	else if (vertexProcessingType == HARDWARE_VP)
		dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if (vertexProcessingType == PURE_HARDWARE_VP)
		dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
	else
		dwBehavior = 0; // TODO: throw exception

	return SUCCEEDED(g_pD3DApp->ConfirmDevice(pCaps, dwBehavior, adapterFormat, backBufferFormat));
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Create(HINSTANCE hInstance)
{
	HRESULT hr;

	m_hInstance = hInstance;

	// Create the Direct3D object
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
		return DisplayErrorMsg(D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT);

	// Build a list of Direct3D adapters, modes and devices. The
	// ConfirmDevice() callback is used to confirm that only devices that
	// meet the app's requirements are considered.
	m_d3dEnumeration.SetD3D(m_pD3D);
	m_d3dEnumeration.ConfirmDeviceCallback = ConfirmDeviceHelper;
	if (FAILED(hr = m_d3dEnumeration.Enumerate()))
	{
		SAFE_RELEASE(m_pD3D);
		return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
	}

	// Unless a substitute hWnd has been specified, create a window to
	// render into
	if (m_hWnd == NULL)
	{
		WNDCLASS wndClass = { CS_DBLCLKS, WndProc, 0, 0, hInstance,
							  LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON)),
							  NULL,
							  (HBRUSH)GetStockObject(WHITE_BRUSH),
							  NULL, _T("D3D Window") };
		RegisterClass(&wndClass);

		// Set the window's initial style
		if (g_Option.m_bStartFullScreen)
			m_dwWindowStyle = WS_POPUP | WS_VISIBLE;
		else
			m_dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		// Set the window's initial width
		RECT rc;
		SetRect(&rc, 0, 0, m_dwCreationWidth, m_dwCreationHeight);
		AdjustWindowRect(&rc, m_dwWindowStyle, true);

		// Create the render window
		m_hWnd = CreateWindow(_T("D3D Window"), m_strWindowTitle, m_dwWindowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			(rc.right - rc.left), (rc.bottom - rc.top), 0,
			LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU)),
			hInstance, 0);

		if (!g_Option.m_bStartFullScreen)
		{
			RECT rc;
			GetWindowRect(m_hWnd, &rc);

			MONITORINFO target;
			target.cbSize = sizeof(MONITORINFO);

			HMONITOR hMon = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
			GetMonitorInfo(hMon, &target);

			int screenW = target.rcWork.right - target.rcWork.left;
			int screenH = target.rcWork.bottom - target.rcWork.top;
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			SetWindowPos(m_hWnd, 0, screenW / 2 - (width / 2), screenH / 2 - (height / 2), width, height, SWP_NOZORDER | SWP_NOSIZE);
		}
	}

	// The focus window can be a specified to be a different window than the
	// device window.  If not, use the device window as the focus window.
	if (m_hWndFocus == NULL)
		m_hWndFocus = m_hWnd;

	// Save window properties
	m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	::GetWindowRect(m_hWnd, &m_rcWindowBounds);
	::GetClientRect(m_hWnd, &m_rcWindowClient);

	ChangeFakeDisplayMode();

	if (FAILED(hr = ChooseInitialD3DSettings()))
	{
		SAFE_RELEASE(m_pD3D);
		return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
	}

	// Initialize the application timer
	DXUtil_Timer(TIMER_START);

	// Initialize the app's custom scene stuff
	if (FAILED(hr = OneTimeSceneInit()))
	{
		SAFE_RELEASE(m_pD3D);
		return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
	}

	// Initialize the 3D environment for the app
	if (FAILED(hr = Initialize3DEnvironment()))
	{
		SAFE_RELEASE(m_pD3D);
		return DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
	}

#ifdef __MINIDUMP
	InitDumper(DUMP_LEVEL_2);
#endif
#ifdef __BUGSPLAT
	MiniDmpSender* mpSender = new MiniDmpSender(L"safemyface_gmail_com", L"Neuz", L"1.0", NULL, MDSF_USEGUARDMEMORY | MDSF_LOGFILE | MDSF_PREVENTHIJACKING);
	SetGlobalCRTExceptionBehavior();
	SetPerThreadCRTExceptionBehavior();
	mpSender->setGuardByteBufferSize(20 * 1024 * 1024);
	mpSender->setDefaultUserName(L"Put your character name here");
	mpSender->setDefaultUserEmail(L"If you want, put your discord ID or mail here");
	mpSender->setDefaultUserDescription(L"Explain to us everything what you can about this crash. It can help us to improve the game.");
#endif

	// The app is ready to go
	Pause(false);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FindBestWindowedMode()
// Desc: Sets up m_d3dSettings with best available windowed mode, subject to
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool CD3DApplication::FindBestWindowedMode(bool bRequireHAL, bool bRequireREF)
{
	// Get display mode of primary adapter (which is assumed to be where the window
	// will appear)
	D3DDISPLAYMODE primaryDesktopDisplayMode;
	m_pD3D->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);

	D3DAdapterInfo* pBestAdapterInfo = NULL;
	D3DDeviceInfo* pBestDeviceInfo = NULL;
	D3DDeviceCombo* pBestDeviceCombo = NULL;

	for (UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++)
	{
		D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
		for (UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++)
		{
			D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
			if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
				continue;
			if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
				continue;
			for (UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++)
			{
				D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
				bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
				if (!pDeviceCombo->IsWindowed)
					continue;
				if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
					continue;
				// If we haven't found a compatible DeviceCombo yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it
				if (pBestDeviceCombo == NULL ||
					pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB)
				{
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
					if (pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB)
					{
						// This windowed device combo looks great -- take it
						goto EndWindowedDeviceComboSearch;
					}
					// Otherwise keep looking for a better windowed device combo
				}
			}
		}
	}
EndWindowedDeviceComboSearch:
	if (pBestDeviceCombo == NULL)
		return false;

	m_d3dSettings.pWindowed_AdapterInfo = pBestAdapterInfo;
	m_d3dSettings.pWindowed_DeviceInfo = pBestDeviceInfo;
	m_d3dSettings.pWindowed_DeviceCombo = pBestDeviceCombo;
	m_d3dSettings.IsWindowed = true;
	m_d3dSettings.Windowed_DisplayMode = primaryDesktopDisplayMode;
	m_d3dSettings.Windowed_Width = m_rcWindowClient.right - m_rcWindowClient.left;
	m_d3dSettings.Windowed_Height = m_rcWindowClient.bottom - m_rcWindowClient.top;
	if (m_d3dEnumeration.AppUsesDepthBuffer)
		m_d3dSettings.Windowed_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
	m_d3dSettings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
	m_d3dSettings.Windowed_MultisampleQuality = 0;
	m_d3dSettings.Windowed_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
	m_d3dSettings.Windowed_PresentInterval = *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
	return true;
}


//-----------------------------------------------------------------------------
// Name: FindBestFullscreenMode()
// Desc: Sets up m_d3dSettings with best available fullscreen mode, subject to
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool CD3DApplication::FindBestFullscreenMode(bool bRequireHAL, bool bRequireREF)
{
	// For fullscreen, default to first HAL DeviceCombo that supports the current desktop
	// display mode, or any display mode if HAL is not compatible with the desktop mode, or
	// non-HAL if no HAL is available
	D3DDISPLAYMODE adapterDesktopDisplayMode;
	D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
	D3DDISPLAYMODE bestDisplayMode;
	bestAdapterDesktopDisplayMode.Width = 0;
	bestAdapterDesktopDisplayMode.Height = 0;
	bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
	bestAdapterDesktopDisplayMode.RefreshRate = 0;

	D3DAdapterInfo* pBestAdapterInfo = NULL;
	D3DDeviceInfo* pBestDeviceInfo = NULL;
	D3DDeviceCombo* pBestDeviceCombo = NULL;

	for (UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++)
	{
		D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
		m_pD3D->GetAdapterDisplayMode(pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode);
		for (UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++)
		{
			D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
			if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
				continue;
			if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
				continue;
			for (UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++)
			{
				D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
				bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
				bool bAdapterMatchesDesktop = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);
				if (pDeviceCombo->IsWindowed)
					continue;
				// If we haven't found a compatible set yet, or if this set
				// is better (because it's a HAL, and/or because formats match better),
				// save it
				if (pBestDeviceCombo == NULL ||
					pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bAdapterMatchesDesktop ||
					pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
				{
					bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
					if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
					{
						// This fullscreen device combo looks great -- take it
						goto EndFullscreenDeviceComboSearch;
					}
					// Otherwise keep looking for a better fullscreen device combo
				}
			}
		}
	}
EndFullscreenDeviceComboSearch:
	if (pBestDeviceCombo == NULL)
		return false;

	// Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
	// and is as close to bestAdapterDesktopDisplayMode's res as possible
	bestDisplayMode.Width = 0;
	bestDisplayMode.Height = 0;
	bestDisplayMode.Format = D3DFMT_UNKNOWN;
	bestDisplayMode.RefreshRate = 0;
	for (UINT idm = 0; idm < pBestAdapterInfo->pDisplayModeList->Count(); idm++)
	{
		D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
		if (pdm->Format != pBestDeviceCombo->AdapterFormat)
			continue;
		if (pdm->Width == bestAdapterDesktopDisplayMode.Width &&
			pdm->Height == bestAdapterDesktopDisplayMode.Height &&
			pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate)
		{
			// found a perfect match, so stop
			bestDisplayMode = *pdm;
			break;
		}
		else if (pdm->Width == bestAdapterDesktopDisplayMode.Width &&
			pdm->Height == bestAdapterDesktopDisplayMode.Height &&
			pdm->RefreshRate > bestDisplayMode.RefreshRate)
		{
			// refresh rate doesn't match, but width/height match, so keep this
			// and keep looking
			bestDisplayMode = *pdm;
		}
		else if (pdm->Width == bestAdapterDesktopDisplayMode.Width)
		{
			// width matches, so keep this and keep looking
			bestDisplayMode = *pdm;
		}
		else if (bestDisplayMode.Width == 0)
		{
			// we don't have anything better yet, so keep this and keep looking
			bestDisplayMode = *pdm;
		}
	}
	bestDisplayMode.Width = FULLSCREEN_WIDTH;
	bestDisplayMode.Height = FULLSCREEN_HEIGHT;

	m_d3dSettings.pFullscreen_AdapterInfo = pBestAdapterInfo;
	m_d3dSettings.pFullscreen_DeviceInfo = pBestDeviceInfo;
	m_d3dSettings.pFullscreen_DeviceCombo = pBestDeviceCombo;
	m_d3dSettings.IsWindowed = false;
	m_d3dSettings.Fullscreen_DisplayMode = bestDisplayMode;
	if (m_d3dEnumeration.AppUsesDepthBuffer)
		m_d3dSettings.Fullscreen_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
	m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
	m_d3dSettings.Fullscreen_MultisampleQuality = 0;
	m_d3dSettings.Fullscreen_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
	m_d3dSettings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
	return true;
}


//-----------------------------------------------------------------------------
// Name: ChooseInitialD3DSettings()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::ChooseInitialD3DSettings()
{
	bool bFoundFullscreen = FindBestFullscreenMode(false, false);
	bool bFoundWindowed = FindBestWindowedMode(false, false);

	if (!bFoundWindowed && bFoundFullscreen)
		m_d3dSettings.IsWindowed = false;

	if (!bFoundFullscreen && !bFoundWindowed)
		return D3DAPPERR_NOCOMPATIBLEDEVICES;

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message handling function.
//-----------------------------------------------------------------------------
LRESULT CD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		// Handle paint messages when the app is paused
		if (m_pd3dDevice && !m_bActive && m_bWindowed &&
			m_bDeviceObjectsInited && m_bDeviceObjectsRestored)
		{
			Render();
			m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		}
		break;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
		return 0;

	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		m_bCurrentlyResizing = true;
		Pause(true);
		break;

	case WM_SIZE:
		// Pick up possible changes to window style due to maximize, etc.
		if (m_bWindowed && m_hWnd != NULL)
			m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);

		if (SIZE_MINIMIZED == wParam)
		{
			ClipCursor();
			m_bMinimized = true;
			m_bMaximized = false;
		}
		else if (SIZE_MAXIMIZED == wParam)
		{
			m_bMinimized = false;
			m_bMaximized = true;
			HandlePossibleSizeChange();
		}
		else if (SIZE_RESTORED == wParam)
		{
			if (m_bMaximized)
			{
				m_bMaximized = false;
				HandlePossibleSizeChange();
			}
			else if (m_bMinimized)
			{
				m_bMinimized = false;
				HandlePossibleSizeChange();
			}
			else if (!m_bCurrentlyResizing)
				HandlePossibleSizeChange();
		}
		break;

	case WM_EXITSIZEMOVE:
		Pause(false);
		m_bCurrentlyResizing = false;
		HandlePossibleSizeChange();
		break;

#ifdef __AIO_NEUZAPP
	case WM_SETFOCUS:
	{
		memset(&g_bKeyTable, 0, sizeof(g_bKeyTable));
		g_Neuz.m_bActiveNeuz = TRUE;
		SetVolume(g_Option.m_fBGMVolume);
	}
	break;
	case WM_KILLFOCUS:
	{
		memset(&g_bKeyTable, 0, sizeof(g_bKeyTable));
		g_Neuz.m_bActiveNeuz = FALSE;
		SetVolume(0.0f);

		g_Neuz.m_bLButtonDown = FALSE;
		g_Neuz.m_bRButtonDown = FALSE;

		if (g_WndMng.m_pWndWorld)
		{
			g_WndMng.m_pWndWorld->m_bCtrlPushed = FALSE;
			g_WndMng.m_pWndWorld->m_bShiftPushed = FALSE;
			g_WndMng.m_pWndWorld->m_bLButtonDown = FALSE;
			g_WndMng.m_pWndWorld->m_bRButtonDown = FALSE;
		}
	}
	break;
#endif

	case WM_SETCURSOR:
		if (m_bActive)
		{
			switch (LOWORD(lParam))
			{
			case HTTOP:
			case HTBOTTOM:
				SetMouseCursor(CUR_RESIZE_VERT);
				break;
			case HTLEFT:
			case HTRIGHT:
				SetMouseCursor(CUR_RESIZE_HORI);
				break;
			case HTTOPLEFT:
			case HTBOTTOMRIGHT:
				SetMouseCursor(CUR_RESIZE_HV1);
				break;
			case HTTOPRIGHT:
			case HTBOTTOMLEFT:
				SetMouseCursor(CUR_RESIZE_HV2);
				break;
			case HTCLIENT:
				break;
			default:
				SetMouseCursor(CUR_BASE);
				break;
			}
			return TRUE;
		}
		break;

	case WM_MOUSEMOVE:
		if (m_bActive && m_pd3dDevice != NULL)
		{
			POINT ptCursor;
			GetCursorPos(&ptCursor);
			if (!m_bWindowed)
				ScreenToClient(m_hWnd, &ptCursor);
			m_pd3dDevice->SetCursorPosition(ptCursor.x, ptCursor.y, 0);
		}
		break;

	case WM_ENTERMENULOOP:
		// Pause the app when menus are displayed
		Pause(true);
		break;

	case WM_EXITMENULOOP:
		Pause(false);
		break;

	case WM_NCHITTEST:
		// Prevent the user from selecting the menu in fullscreen mode
		if (!m_bWindowed)
			return HTCLIENT;
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	{
		if (LOWORD(wParam) == HTMENU
			|| LOWORD(wParam) == HTSYSMENU)
			return 0;
	}
	break;

	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
		return 0;

	case WM_CONTEXTMENU:
		return 0;

	case WM_POWERBROADCAST:
		switch (wParam)
		{
#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND 0x0000
#endif
		case PBT_APMQUERYSUSPEND:
			// At this point, the app should save any data for open
			// network connections, files, etc., and prepare to go into
			// a suspended mode.
			return true;

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif
		case PBT_APMRESUMESUSPEND:
			// At this point, the app should recover any data, network
			// connections, files, etc., and resume running from when
			// the app was suspended.
			return true;
		}
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing and power loss in fullscreen mode
		switch (wParam)
		{
		case SC_KEYMENU:
			return 0;

		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MONITORPOWER:
			if (false == m_bWindowed)
				return 1;
			break;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_TOGGLESTART:
			// Toggle frame movement
			m_bFrameMoving = !m_bFrameMoving;
			DXUtil_Timer(m_bFrameMoving ? TIMER_START : TIMER_STOP);
			break;

		case IDM_SINGLESTEP:
			// Single-step frame movement
			if (false == m_bFrameMoving)
				DXUtil_Timer(TIMER_ADVANCE);
			else
				DXUtil_Timer(TIMER_STOP);
			m_bFrameMoving = false;
			m_bSingleStep = true;
			break;

		case IDM_CHANGEDEVICE:
			return 0;

		case IDM_TOGGLEFULLSCREEN:
			return 0;

		case IDM_EXIT:
			break;
		}
		break;
	case WM_CLOSE:
	{
#ifdef __DISCORD // Close
		g_Neuz.ManageDiscord(DIS_END);
#endif
		Cleanup3DEnvironment();
		SAFE_RELEASE(m_pD3D);
		FinalCleanup();
		HMENU hMenu = GetMenu(hWnd);
		if (hMenu != NULL)
			DestroyMenu(hMenu);
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		m_hWnd = NULL;
		return 0;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: HandlePossibleSizeChange()
// Desc: Reset the device if the client area size has changed.
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::HandlePossibleSizeChange()
{
	HRESULT hr = S_OK;

	RECT rcBoundsOld = m_rcWindowBounds;
	RECT rcClientOld = m_rcWindowClient;
	::GetWindowRect(m_hWnd, &m_rcWindowBounds);
	::GetClientRect(m_hWnd, &m_rcWindowClient);

	const int nWidthOld = rcClientOld.right - rcClientOld.left;
	const int nHeightOld = rcClientOld.bottom - rcClientOld.top;

	const int nWidth = m_rcWindowClient.right - m_rcWindowClient.left;
	const int nHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

	if (nWidthOld != nWidth || nHeightOld != nHeight)
	{
		// A new window size will require a new backbuffer
		// size, so the 3D structures must be changed accordingly.
		Pause(true);

		m_d3dpp.BackBufferWidth = nWidth;
		m_d3dpp.BackBufferHeight = nHeight;

		if (m_pd3dDevice != NULL)
		{
			// Reset the 3D environment
			if (FAILED(hr = Reset3DEnvironment()))
			{
				if (hr != D3DERR_OUTOFVIDEOMEMORY)
					hr = D3DAPPERR_RESETFAILED;
				DisplayErrorMsg(hr, MSGERR_APPMUSTEXIT);
			}
		}

		Pause(false);

		g_Option.m_nResWidth = nWidth;
		g_Option.m_nResHeight = nHeight;
		g_Option.Save("neuz.ini");

		g_Neuz.ApplyWidthHeightChanges();

		g_WndMng.SetWndRect(CRect(0, 0, nWidth, nHeight));
		g_WndMng.AlighWindow(rcBoundsOld, m_rcWindowBounds, rcClientOld, m_rcWindowClient);

		if (g_WndMng.m_pWndTaskBar)
			g_WndMng.OpenTaskBar();
	}

	return hr;
}




//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Initialize3DEnvironment()
{
	HRESULT hr;

	D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
	D3DDeviceInfo* pDeviceInfo = m_d3dSettings.PDeviceInfo();

	m_bWindowed = m_d3dSettings.IsWindowed;

	// Prepare window for possible windowed/fullscreen change
	AdjustWindowForChange();

	// Set up the presentation parameters
	BuildPresentParamsFromSettings();

	if (pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE)
	{
		// Warn user about null ref device that can't render anything
		DisplayErrorMsg(D3DAPPERR_NULLREFDEVICE, 0);
	}

	DWORD behaviorFlags;
	if (m_d3dSettings.GetVertexProcessingType() == SOFTWARE_VP)
		behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if (m_d3dSettings.GetVertexProcessingType() == MIXED_VP)
		behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
	else if (m_d3dSettings.GetVertexProcessingType() == HARDWARE_VP)
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if (m_d3dSettings.GetVertexProcessingType() == PURE_HARDWARE_VP)
		behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
	else
		behaviorFlags = 0; // TODO: throw exception


#ifdef __TEST_ANTIALIASING
	DWORD MSQuality = 0;
	D3DMULTISAMPLE_TYPE MSType = D3DMULTISAMPLE_NONE;
	if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, TRUE, D3DMULTISAMPLE_8_SAMPLES, &MSQuality)))
		MSType = D3DMULTISAMPLE_8_SAMPLES;
	else if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, TRUE, D3DMULTISAMPLE_4_SAMPLES, &MSQuality)))
		MSType = D3DMULTISAMPLE_4_SAMPLES;
	else if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, TRUE, D3DMULTISAMPLE_2_SAMPLES, &MSQuality)))
		MSType = D3DMULTISAMPLE_2_SAMPLES;
	int MSQ = MSQuality - 1;

	if (MSType != D3DMULTISAMPLE_NONE)
	{
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.MultiSampleType = MSType;
		m_d3dpp.MultiSampleQuality = MSQ;
		m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		m_d3dpp.Flags = 0;
		m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}

	hr = m_pD3D->CreateDevice(m_d3dSettings.AdapterOrdinal(), pDeviceInfo->DevType,
		m_hWndFocus, behaviorFlags, &m_d3dpp,
		&m_pd3dDevice);


	if (MSType != D3DMULTISAMPLE_NONE)
	{
		m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)8);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
#else
	hr = m_pD3D->CreateDevice(m_d3dSettings.AdapterOrdinal(), pDeviceInfo->DevType,
		m_hWndFocus, behaviorFlags, &m_d3dpp,
		&m_pd3dDevice);
#endif

	if (SUCCEEDED(hr))
	{
		// When moving from fullscreen to windowed mode, it is important to
		// adjust the window size after recreating the device rather than
		// beforehand to ensure that you get the window size you want.  For
		// example, when switching from 640x480 fullscreen to windowed with
		// a 1000x600 window on a 1024x768 desktop, it is impossible to set
		// the window size to 1000x600 until after the display mode has
		// changed to 1024x768, because windows cannot be larger than the
		// desktop.
		if (m_bWindowed)
		{
			SetWindowPos(m_hWnd, HWND_NOTOPMOST,
				m_rcWindowBounds.left, m_rcWindowBounds.top,
				(m_rcWindowBounds.right - m_rcWindowBounds.left),
				(m_rcWindowBounds.bottom - m_rcWindowBounds.top),
				SWP_SHOWWINDOW);
		}

		// Store device Caps
		m_pd3dDevice->GetDeviceCaps(&m_d3dCaps);
		m_dwCreateFlags = behaviorFlags;

		// Store device description
		if (pDeviceInfo->DevType == D3DDEVTYPE_REF)
			lstrcpy(m_strDeviceStats, TEXT("REF"));
		else if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
			lstrcpy(m_strDeviceStats, TEXT("HAL"));
		else if (pDeviceInfo->DevType == D3DDEVTYPE_SW)
			lstrcpy(m_strDeviceStats, TEXT("SW"));

		if (behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
			behaviorFlags & D3DCREATE_PUREDEVICE)
		{
			if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
				lstrcat(m_strDeviceStats, TEXT(" (pure hw vp)"));
			else
				lstrcat(m_strDeviceStats, TEXT(" (simulated pure hw vp)"));
		}
		else if (behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		{
			if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
				lstrcat(m_strDeviceStats, TEXT(" (hw vp)"));
			else
				lstrcat(m_strDeviceStats, TEXT(" (simulated hw vp)"));
		}
		else if (behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
		{
			if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
				lstrcat(m_strDeviceStats, TEXT(" (mixed vp)"));
			else
				lstrcat(m_strDeviceStats, TEXT(" (simulated mixed vp)"));
		}
		else if (behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
		{
			lstrcat(m_strDeviceStats, TEXT(" (sw vp)"));
		}

		if (pDeviceInfo->DevType == D3DDEVTYPE_HAL)
		{
			// Be sure not to overflow m_strDeviceStats when appending the adapter
			// description, since it can be long.  Note that the adapter description
			// is initially CHAR and must be converted to TCHAR.
			lstrcat(m_strDeviceStats, TEXT(": "));
			const int cchDesc = sizeof(pAdapterInfo->AdapterIdentifier.Description);
			TCHAR szDescription[cchDesc];
			DXUtil_ConvertAnsiStringToGenericCch(szDescription,
				pAdapterInfo->AdapterIdentifier.Description, cchDesc);
			int maxAppend = sizeof(m_strDeviceStats) / sizeof(TCHAR) -
				lstrlen(m_strDeviceStats) - 1;
			_tcsncat(m_strDeviceStats, szDescription, maxAppend);
		}

		// Store render target surface desc
		LPDIRECT3DSURFACE9 pBackBuffer = NULL;
		m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		pBackBuffer->GetDesc(&m_d3dsdBackBuffer);
		pBackBuffer->Release();

		if (m_bShowCursorWhenFullscreen && !m_bWindowed)
		{
#ifdef _WIN64
			HCURSOR hCursor = (HCURSOR)GetClassLongPtr(m_hWnd, GCLP_HCURSOR);
#else
			HCURSOR hCursor = (HCURSOR)ULongToHandle(GetClassLong(m_hWnd, GCL_HCURSOR));
#endif
			D3DUtil_SetDeviceCursor(m_pd3dDevice, hCursor, true);
			m_pd3dDevice->ShowCursor(true);
		}

		// Confine cursor to fullscreen window
		ClipCursor();

		// Initialize the app's device-dependent objects
		hr = InitDeviceObjects();
		if (FAILED(hr))
		{
			DeleteDeviceObjects();
		}
		else
		{
			m_bDeviceObjectsInited = true;
			hr = RestoreDeviceObjects();
			if (FAILED(hr))
			{
				InvalidateDeviceObjects();
			}
			else
			{
				m_bDeviceObjectsRestored = true;
				return S_OK;
			}
		}

		// Cleanup before we try again
		Cleanup3DEnvironment();
	}

	// If that failed, fall back to the reference rasterizer
	if (hr != D3DAPPERR_MEDIANOTFOUND &&
		hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) &&
		pDeviceInfo->DevType == D3DDEVTYPE_HAL)
	{
		if (FindBestWindowedMode(false, true))
		{
			m_bWindowed = true;
			AdjustWindowForChange();
			// Make sure main window isn't topmost, so error message is visible
			SetWindowPos(m_hWnd, HWND_NOTOPMOST,
				m_rcWindowBounds.left, m_rcWindowBounds.top,
				(m_rcWindowBounds.right - m_rcWindowBounds.left),
				(m_rcWindowBounds.bottom - m_rcWindowBounds.top),
				SWP_SHOWWINDOW);

			// Let the user know we are switching from HAL to the reference rasterizer
			DisplayErrorMsg(hr, MSGWARN_SWITCHEDTOREF);

			hr = Initialize3DEnvironment();
		}
	}
	return hr;
}




//-----------------------------------------------------------------------------
// Name: BuildPresentParamsFromSettings()
// Desc:
//-----------------------------------------------------------------------------
void CD3DApplication::BuildPresentParamsFromSettings()
{
	m_d3dpp.Windowed = m_d3dSettings.IsWindowed;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.MultiSampleType = m_d3dSettings.MultisampleType();
	m_d3dpp.MultiSampleQuality = m_d3dSettings.MultisampleQuality();
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.EnableAutoDepthStencil = m_d3dEnumeration.AppUsesDepthBuffer;
	m_d3dpp.hDeviceWindow = m_hWnd;
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if (m_d3dEnumeration.AppUsesDepthBuffer)
	{
		m_d3dpp.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_d3dpp.AutoDepthStencilFormat = m_d3dSettings.DepthStencilBufferFormat();
	}

	if (m_bWindowed)
	{
		m_d3dpp.BackBufferWidth = m_rcWindowClient.right - m_rcWindowClient.left;
		m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
		m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{
		m_d3dpp.BackBufferWidth = m_d3dSettings.DisplayMode().Width;
		m_d3dpp.BackBufferHeight = m_d3dSettings.DisplayMode().Height;
		m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
		m_d3dpp.FullScreen_RefreshRateInHz = m_d3dSettings.Fullscreen_DisplayMode.RefreshRate;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

}

void CD3DApplication::ClipCursor()
{
	if (m_bClipCursorWhenFullscreen)
	{
		if (!m_bWindowed)
		{
			RECT rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);
			::ClipCursor(&rcWindow);
		}
		else
		{
			::ClipCursor(NULL);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Reset3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::Reset3DEnvironment()
{
	HRESULT hr;

	// Release all vidmem objects
	if (m_bDeviceObjectsRestored)
	{
		m_bDeviceObjectsRestored = false;
		InvalidateDeviceObjects();
	}
	// Reset the device
	if (FAILED(hr = m_pd3dDevice->Reset(&m_d3dpp)))
	{
		if (D3DERR_DEVICELOST == hr)
		{

		}

		return hr;
	}

	// Store render target surface desc
	LPDIRECT3DSURFACE9 pBackBuffer;
	m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	pBackBuffer->GetDesc(&m_d3dsdBackBuffer);
	pBackBuffer->Release();

	if (m_bShowCursorWhenFullscreen && !m_bWindowed)
	{
		HCURSOR hCursor;
#ifdef _WIN64
		hCursor = (HCURSOR)GetClassLongPtr(m_hWnd, GCLP_HCURSOR);
#else
		hCursor = (HCURSOR)ULongToHandle(GetClassLong(m_hWnd, GCL_HCURSOR));
#endif
		D3DUtil_SetDeviceCursor(m_pd3dDevice, hCursor, true);
		m_pd3dDevice->ShowCursor(true);
	}

	// Confine cursor to fullscreen window
	ClipCursor();

	// Initialize the app's device-dependent objects
	hr = RestoreDeviceObjects();
	if (FAILED(hr))
	{
		InvalidateDeviceObjects();
		return hr;
	}
	m_bDeviceObjectsRestored = true;

	// If the app is paused, trigger the rendering of the current frame
	if (false == m_bFrameMoving)
	{
		m_bSingleStep = true;
		DXUtil_Timer(TIMER_START);
		DXUtil_Timer(TIMER_STOP);
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::AdjustWindowForChange()
{
	SetWindowLong(m_hWnd, GWL_STYLE, m_dwWindowStyle);

	if (m_hMenu != NULL)
	{
		SetMenu(m_hWnd, m_hMenu);
		m_hMenu = NULL;
	}

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Run()
// Desc:
//-----------------------------------------------------------------------------
INT CD3DApplication::Run()
{
	// Load keyboard accelerators
	HACCEL hAccel = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_MAIN_ACCEL));

	// Now we're ready to recieve and process Windows messages.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		if (m_bActive)
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		else
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			if (hAccel == NULL || m_hWnd == NULL ||
				0 == TranslateAccelerator(m_hWnd, hAccel, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (m_bDeviceLost)
			{
				// Yield some CPU time to other processes
				Sleep(100); // 100 milliseconds
			}
			// Render a frame during idle time (no messages are waiting)
			if (m_bActive)
			{
				if (FAILED(Render3DEnvironment()))
					SendMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
		}
	}
	if (hAccel != NULL)
		DestroyAcceleratorTable(hAccel);

	return (INT)msg.wParam;
}

//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
#ifdef __CLIENT
#include "DPLoginClient.h"
#include "dpcertified.h"
extern	CDPLoginClient	g_dpLoginClient;
extern	CDPCertified	g_dpCertified;
#endif
HRESULT CD3DApplication::Render3DEnvironment()
{
	HRESULT hr;

	g_dpLoginClient.Ping();
	g_dpCertified.Ping();

	if (m_bDeviceLost)
	{
		// Test the cooperative level to see if it's okay to render
		if (FAILED(hr = m_pd3dDevice->TestCooperativeLevel()))
		{
			// If the device was lost, do not render until we get it back
			if (D3DERR_DEVICELOST == hr)
			{
				//CString str;
				//str.Format( );

				return S_OK;
			}

			// Check if the device needs to be reset.
			if (D3DERR_DEVICENOTRESET == hr)
			{
				// If we are windowed, read the desktop mode and use the same format for
				// the back buffer
				if (m_bWindowed)
				{
					D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
					m_pD3D->GetAdapterDisplayMode(pAdapterInfo->AdapterOrdinal, &m_d3dSettings.Windowed_DisplayMode);
					m_d3dpp.BackBufferFormat = m_d3dSettings.Windowed_DisplayMode.Format;
				}

				if (FAILED(hr = Reset3DEnvironment()))
					return hr;
			}
			return hr;
		}
		m_bDeviceLost = false;
	}


	// Get the app's time, in seconds. Skip rendering if no time elapsed
	FLOAT fAppTime = DXUtil_Timer(TIMER_GETAPPTIME);
	FLOAT fElapsedAppTime = DXUtil_Timer(TIMER_GETELAPSEDTIME);
	if ((0.0f == fElapsedAppTime) && m_bFrameMoving)
		return S_OK;


	// FrameMove (animate) the scene
	if (m_bFrameMoving || m_bSingleStep)
	{
		// Store the time for the app
		m_fTime = fAppTime;
		m_fElapsedTime = fElapsedAppTime;

		// Frame move the scene
		if (FAILED(hr = FrameMove()))
			return hr;

		m_bSingleStep = false;
	}

	if (IsDrawTiming() && IsRenderState())
	{
		// Render the scene as normal
		if (FAILED(hr = Render()))
			return hr;

		UpdateStats();

		// Show the frame on the primary surface.
		hr = m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (D3DERR_DEVICELOST == hr)
			m_bDeviceLost = true;
	}

	SetFST();

	return S_OK;
}


#ifdef __AIO_NEUZAPP
BOOL CD3DApplication::IsRenderState()
{
	// Window and this window not minimised
	if (!g_Option.m_bStartFullScreen && !IsMinimised())
		return TRUE;

	// Fullscreen and this fullscreen in focus
	if (g_Option.m_bStartFullScreen && g_Neuz.m_bActiveNeuz)
		return TRUE;

	// Actually should be enough to just disable rendering completely, BUT...
	// But in flyff some processing (mostly UI) done in rendering, so to avoid possible troubles better keep it like this for now
	if (m_dwNextRenderTime < GetTickCount())
	{
		m_dwNextRenderTime = GetTickCount() + SEC(1);
		return TRUE;
	}

	return FALSE;
}
#endif

//-----------------------------------------------------------------------------
// Name: UpdateStats()
// Desc:
//-----------------------------------------------------------------------------
void CD3DApplication::UpdateStats()
{
	// Keep track of the frame count
	static FLOAT fLastTime = 0.0f;
	static DWORD dwFrames = 0;
	FLOAT fTime = DXUtil_Timer(TIMER_GETABSOLUTETIME);
	++dwFrames;

	// Update the scene stats once per second
	if (fTime - fLastTime > 1.0f)
	{
		m_fFPS = dwFrames / (fTime - fLastTime);

		fLastTime = fTime;
		dwFrames = 0;

		TCHAR strFmt[100];
		D3DFORMAT fmtAdapter = m_d3dSettings.DisplayMode().Format;
		if (fmtAdapter == m_d3dsdBackBuffer.Format)
		{
			lstrcpyn(strFmt, D3DUtil_D3DFormatToString(fmtAdapter, false), 100);
		}
		else
		{
			_sntprintf(strFmt, 100, TEXT("backbuf %s, adapter %s"),
				D3DUtil_D3DFormatToString(m_d3dsdBackBuffer.Format, false),
				D3DUtil_D3DFormatToString(fmtAdapter, false));
		}
		strFmt[99] = TEXT('\0');

		TCHAR strDepthFmt[100];
		if (m_d3dEnumeration.AppUsesDepthBuffer)
		{
			_sntprintf(strDepthFmt, 100, TEXT(" (%s)"),
				D3DUtil_D3DFormatToString(m_d3dSettings.DepthStencilBufferFormat(), false));
			strDepthFmt[99] = TEXT('\0');
		}
		else
		{
			// No depth buffer
			strDepthFmt[0] = TEXT('\0');
		}

		TCHAR* pstrMultiSample;
		switch (m_d3dSettings.MultisampleType())
		{
		case D3DMULTISAMPLE_NONMASKABLE:  pstrMultiSample = TEXT(" (Nonmaskable Multisample)"); break;
		case D3DMULTISAMPLE_2_SAMPLES:  pstrMultiSample = TEXT(" (2x Multisample)"); break;
		case D3DMULTISAMPLE_3_SAMPLES:  pstrMultiSample = TEXT(" (3x Multisample)"); break;
		case D3DMULTISAMPLE_4_SAMPLES:  pstrMultiSample = TEXT(" (4x Multisample)"); break;
		case D3DMULTISAMPLE_5_SAMPLES:  pstrMultiSample = TEXT(" (5x Multisample)"); break;
		case D3DMULTISAMPLE_6_SAMPLES:  pstrMultiSample = TEXT(" (6x Multisample)"); break;
		case D3DMULTISAMPLE_7_SAMPLES:  pstrMultiSample = TEXT(" (7x Multisample)"); break;
		case D3DMULTISAMPLE_8_SAMPLES:  pstrMultiSample = TEXT(" (8x Multisample)"); break;
		case D3DMULTISAMPLE_9_SAMPLES:  pstrMultiSample = TEXT(" (9x Multisample)"); break;
		case D3DMULTISAMPLE_10_SAMPLES: pstrMultiSample = TEXT(" (10x Multisample)"); break;
		case D3DMULTISAMPLE_11_SAMPLES: pstrMultiSample = TEXT(" (11x Multisample)"); break;
		case D3DMULTISAMPLE_12_SAMPLES: pstrMultiSample = TEXT(" (12x Multisample)"); break;
		case D3DMULTISAMPLE_13_SAMPLES: pstrMultiSample = TEXT(" (13x Multisample)"); break;
		case D3DMULTISAMPLE_14_SAMPLES: pstrMultiSample = TEXT(" (14x Multisample)"); break;
		case D3DMULTISAMPLE_15_SAMPLES: pstrMultiSample = TEXT(" (15x Multisample)"); break;
		case D3DMULTISAMPLE_16_SAMPLES: pstrMultiSample = TEXT(" (16x Multisample)"); break;
		default:                        pstrMultiSample = TEXT(""); break;
		}

		const int cchMaxFrameStats = sizeof(m_strFrameStats) / sizeof(TCHAR);
		_sntprintf(m_strFrameStats, cchMaxFrameStats, _T("%.02f fps (%dx%d), %s%s%s"), m_fFPS,
			m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height,
			strFmt, strDepthFmt, pstrMultiSample);
		m_strFrameStats[cchMaxFrameStats - 1] = TEXT('\0');
	}
}




//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
void CD3DApplication::Pause(bool bPause)
{
	static DWORD dwAppPausedCount = 0;

	dwAppPausedCount += (bPause ? +1 : -1);
	m_bActive = (dwAppPausedCount ? false : true);

	// Handle the first pause request (of many, nestable pause requests)
	if (bPause && (1 == dwAppPausedCount))
	{
		// Stop the scene from animating
		if (m_bFrameMoving)
			DXUtil_Timer(TIMER_STOP);
	}

	if (0 == dwAppPausedCount)
	{
		// Restart the timers
		if (m_bFrameMoving)
			DXUtil_Timer(TIMER_START);
	}
}

//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
void CD3DApplication::Cleanup3DEnvironment()
{
	if (m_pd3dDevice != NULL)
	{
		if (m_bDeviceObjectsRestored)
		{
			m_bDeviceObjectsRestored = false;
			InvalidateDeviceObjects();
		}
		if (m_bDeviceObjectsInited)
		{
			m_bDeviceObjectsInited = false;
			DeleteDeviceObjects();
		}

		// __LOCAL_TESTS
		/*int nCount = m_pd3dDevice->Release();
		if (nCount > 0)
		{
			TCHAR szString[256];
			sprintf(szString, "Not released : %d", nCount);
			MessageBox(NULL, szString, "MASQUERADE", MB_ICONERROR | MB_OK);
			//DisplayErrorMsg( D3DAPPERR_NONZEROREFCOUNT, MSGERR_APPMUSTEXIT );
		}*/

		m_pd3dDevice = NULL;
	}
}


//-----------------------------------------------------------------------------
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT CD3DApplication::DisplayErrorMsg(HRESULT hr, DWORD dwType)
{
	static bool s_bFatalErrorReported = false;
	TCHAR strMsg[512];

	// If a fatal error message has already been reported, the app
	// is already shutting down, so don't show more error messages.
	if (s_bFatalErrorReported)
		return hr;

	BOOL bDefault = FALSE;
	switch (hr)
	{
	case D3DAPPERR_NODIRECT3D:
		_tcscpy(strMsg, _T("Could not initialize Direct3D. You may\n")
			_T("want to check that the latest version of\n")
			_T("DirectX is correctly installed on your\n")
			_T("system.  Also make sure that this program\n")
			_T("was compiled with header files that match\n")
			_T("the installed DirectX DLLs."));
		break;

	case D3DAPPERR_NOCOMPATIBLEDEVICES:
		if (::GetLanguage() == LANG_KOR)
		{
			_tcscpy(strMsg, _T("Could not find any compatible Direct3D\n")
				_T("devices.\n")
				_T("그래픽 드라이버의 버전이 낮거나 그래픽카드가 Direct3D를 지원하지 않습니다.\n")
				_T("www.flyff.com에서 화면드라이버를 받아 설치하거나\n")
				_T("제조사의 홈페이지에서 다운로드 받아 설치하시기 바랍니다.")
			);
		}
		else
		{
			_tcscpy(strMsg, _T("Could not find any compatible Direct3D\n")
				_T("devices."));
		}
		break;

	case D3DAPPERR_NOWINDOWABLEDEVICES:
		_tcscpy(strMsg, _T("This sample cannot run in a desktop\n")
			_T("window with the current display settings.\n")
			_T("Please change your desktop settings to a\n")
			_T("16- or 32-bit display mode and re-run this\n")
			_T("sample."));
		break;

	case D3DAPPERR_NOHARDWAREDEVICE:
		_tcscpy(strMsg, _T("No hardware-accelerated Direct3D devices\n")
			_T("were found."));
		break;

	case D3DAPPERR_HALNOTCOMPATIBLE:
		_tcscpy(strMsg, _T("This sample requires functionality that is\n")
			_T("not available on your Direct3D hardware\n")
			_T("accelerator."));
		break;

	case D3DAPPERR_NOWINDOWEDHAL:
		_tcscpy(strMsg, _T("Your Direct3D hardware accelerator cannot\n")
			_T("render into a window.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes."));
		break;

	case D3DAPPERR_NODESKTOPHAL:
		_tcscpy(strMsg, _T("Your Direct3D hardware accelerator cannot\n")
			_T("render into a window with the current\n")
			_T("desktop display settings.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes."));
		break;

	case D3DAPPERR_NOHALTHISMODE:
		_tcscpy(strMsg, _T("This sample requires functionality that is\n")
			_T("not available on your Direct3D hardware\n")
			_T("accelerator with the current desktop display\n")
			_T("settings.\n")
			_T("Press F2 while the app is running to see a\n")
			_T("list of available devices and modes."));
		break;

	case D3DAPPERR_MEDIANOTFOUND:
	case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
		_tcscpy(strMsg, _T("Could not load required media."));
		break;

	case D3DAPPERR_RESETFAILED:
		_tcscpy(strMsg, _T("Could not reset the Direct3D device."));
		break;

	case D3DAPPERR_NONZEROREFCOUNT:
		_tcscpy(strMsg, _T("A D3D object has a non-zero reference\n")
			_T("count (meaning things were not properly\n")
			_T("cleaned up)."));
		break;

	case D3DAPPERR_NULLREFDEVICE:
		_tcscpy(strMsg, _T("Warning: Nothing will be rendered.\n")
			_T("The reference rendering device was selected, but your\n")
			_T("computer only has a reduced-functionality reference device\n")
			_T("installed.  Install the DirectX SDK to get the full\n")
			_T("reference device.\n"));
		break;

	case E_OUTOFMEMORY:
		_tcscpy(strMsg, _T("Not enough memory."));
		break;

	case D3DERR_OUTOFVIDEOMEMORY:
		_tcscpy(strMsg, _T("Not enough video memory."));
		break;

	case D3DERR_DRIVERINTERNALERROR:
		_tcscpy(strMsg, _T("A serious problem occured inside the display driver."));
		dwType = MSGERR_APPMUSTEXIT;
		break;

	default:
		_tcscpy(strMsg, _T("Generic application error. Enable\n")
			_T("debug output for detailed information."));
		char ErrorCode[12];
		sprintf(ErrorCode, "%d", (int)hr);
		_tcscat(strMsg, _T(ErrorCode));
		bDefault = TRUE;
	}

	if (MSGERR_APPMUSTEXIT == dwType)
	{
		s_bFatalErrorReported = true;
		_tcscat(strMsg, _T("\n\nThis sample will now exit."));
#ifdef __CLIENT
		MessageBox(GetSafeHwnd(), strMsg, m_strWindowTitle, MB_ICONERROR | MB_OK);
#else
		MessageBox(NULL, strMsg, m_strWindowTitle, MB_ICONERROR | MB_OK);
#endif

		// Close the window, which shuts down the app
#ifdef __CLIENT
		Error("%s", strMsg);
#endif
		if (hr != D3DAPPERR_RESETFAILED && bDefault == FALSE)
		{
			//ADDERRORMSG_NOHEAD( strMsg );
			int* p = NULL;
			*p = 1;
		}
		if (m_hWnd)
			SendMessage(m_hWnd, WM_CLOSE, 0, 0);
	}
	else
	{
		if (MSGWARN_SWITCHEDTOREF == dwType)
			_tcscat(strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
				_T("a software device that implements the entire\n")
				_T("Direct3D feature set, but runs very slowly."));
		MessageBox(NULL, strMsg, m_strWindowTitle, MB_ICONWARNING | MB_OK);
	}

	return hr;
}

void CD3DApplication::ChangeFakeDisplayMode()
{
	if (g_Option.m_bStartFullScreen)
	{
		ZeroMemory(&m_PrevDevMode, sizeof(DEVMODE));
		m_PrevDevMode.dmSize = sizeof(DEVMODE);
		m_PrevDevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_PrevDevMode);


		DEVMODE dm;
		ZeroMemory(&dm, sizeof(DEVMODE));
		dm.dmSize = sizeof(DEVMODE);
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		dm.dmBitsPerPel = 32;
		dm.dmPelsWidth = FULLSCREEN_WIDTH;
		dm.dmPelsHeight = FULLSCREEN_HEIGHT;
		dm.dmDisplayFrequency = m_PrevDevMode.dmDisplayFrequency;

		if (ChangeDisplaySettings(&dm, CDS_TEST) == DISP_CHANGE_SUCCESSFUL) {
			ChangeDisplaySettings(&dm, CDS_FULLSCREEN);


			SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION);
		}
	}
}

void CD3DApplication::RestoreFakeDisplayMode()
{
	m_PrevDevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	if (ChangeDisplaySettings(&m_PrevDevMode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
		ChangeDisplaySettings(&m_PrevDevMode, CDS_UPDATEREGISTRY);
}

#ifdef __AIO_NEUZAPP
void CD3DApplication::Flash()
{
	FLASHWINFO fInfo;
	fInfo.cbSize = sizeof(fInfo);
	fInfo.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
	fInfo.hwnd = m_hWnd;
	fInfo.uCount = 15;
	fInfo.dwTimeout = 500;
	FlashWindowEx(&fInfo);
}
#endif