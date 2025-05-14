#include "stdafx.h"
#include <stdio.h>

#ifndef __MINIMIZE_USE_OPTION
#include "scanner.h"
#include "lang.h"
#endif

#include "HwOption.h"

#if __VER >= 9 // __CSC_VER9_RESOLUTION
#include "D3D9.h"
#pragma comment	(lib, "d3d9.lib")
#endif //__CSC_VER9_RESOLUTION


COption		g_Option;


#define CScannerEx	CScanner
#define GetTokenEx	GetToken

COption::COption()
{

}

COption::~COption()
{
}

void COption::Init()
{
	memset(m_szFileName, 0, sizeof(m_szFileName));
	m_nWeatherEffect = TRUE;
	m_bOperator = FALSE;
	m_nResWidth = 800;
	m_nResHeight = 600;

	m_bStartFullScreen = TRUE;
	m_nTextureQuality = 1;
	m_nObjectDistant = 1;
	m_nObjectDetail = 1;
	m_nShadow = 2;
	m_nWindowAlpha = 255;
	m_nWindowEffect = 0;
	m_nInstantHelp = 1;

	m_bSaveAccount = 0;
	m_szAccount[0] = 0;
	m_bNotice = TRUE;
	m_bTrade = TRUE;
	m_bSay = TRUE;
	m_bParty = TRUE;
	m_bMessengerJoin = TRUE;
	m_bMessenger = TRUE;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	m_bBattleBGM = FALSE;
#endif // __IMPROVE_SYSTEM_VER15
	m_nSer = m_nMSer = 0;

	m_nMonName = 1;
	m_nToolTipTexture = 0;

	m_fEffectVolume = 1.0f;
	m_fBGMVolume = 0.5f;

	m_dwChatFilter[0] = CHATSTY_GENERAL | CHATSTY_WHISPER | CHATSTY_SHOUT | CHATSTY_PARTY | CHATSTY_GUILD;
	m_dwChatFilter[1] = CHATSTY_GENERAL | CHATSTY_WHISPER;
	m_dwChatFilter[2] = CHATSTY_GENERAL | CHATSTY_SHOUT;
	m_dwChatFilter[3] = CHATSTY_GENERAL | CHATSTY_PARTY;
	m_dwChatFilter[4] = CHATSTY_GENERAL | CHATSTY_GUILD;
	m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;


	m_bDamageRender = FALSE;

	m_bSFXItemRenderOff = FALSE;
	m_bSFXRenderOff = FALSE;

	m_bCameraLock = TRUE;
	m_bAutoAttack = TRUE;
	m_nPlayerName = 1;
	m_nOtherPlayerName = 1;

#if __VER >= 8 //__CSC_VER8_3
	m_BuffStatusMode = 0;
#endif //__CSC_VER8_3

#if __VER >= 11 // __ADD_ZOOMOPT
	m_bZoomLimit = TRUE;
#endif

	m_nCurFold = 0;

#ifdef __FL_UPDATE_TASKBAR
	m_bRekeyTask = 0;
#endif

#ifdef __FL_PET_FILTER
	m_nLootType = 0;
	m_nLootPlace = TYPE_INVENTORY;
#endif

	m_bNightMode = 0;

#ifdef __AIO_PETS
	m_bHideWindowLooter = FALSE;
	m_bHideWindowPet = FALSE;
#endif

#ifdef __FL_SAVE_MESSAGES
	m_bBigMessenger = TRUE;
#endif
}

#if __VER >= 9 // __CSC_VER9_RESOLUTION
void COption::CheckResolution()
{
	IDirect3D9* pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	int nMaxWidth = 800;
	int nMaxHeight = 600;

	D3DFORMAT allowedAdapterFormat = D3DFMT_X8R8G8B8;
	UINT numAdapterModes = pD3D9->GetAdapterModeCount(0, allowedAdapterFormat);
	for (UINT mode = 0; mode < numAdapterModes; mode++)
	{
		D3DDISPLAYMODE displayMode;
		pD3D9->EnumAdapterModes(0, allowedAdapterFormat, mode, &displayMode);

		if (displayMode.Width > nMaxWidth)
			nMaxWidth = displayMode.Width;
		if (displayMode.Height > nMaxHeight)
			nMaxHeight = displayMode.Height;
	}

	if (m_bStartFullScreen)
	{
		m_nResWidth = nMaxWidth;
		m_nResHeight = nMaxHeight;
	}
	else
	{
		if (m_nResWidth < 800)
			m_nResWidth = 800;
		if (m_nResWidth > nMaxWidth)
			m_nResWidth = nMaxWidth;
		if (m_nResHeight < 600)
			m_nResHeight = 600;
		if (m_nResHeight > nMaxHeight)
			m_nResHeight = nMaxHeight;

	}

	//SAFE_RELEASE( pD3D9 );
	pD3D9->Release();
}
#endif//__CSC_VER9_RESOLUTION

int COption::Load(LPCTSTR szFileName)
{
	_tcscpy(m_szFileName, szFileName);

	CScannerEx scan;
	if (scan.Load(szFileName) == FALSE)
		return 0;

	do {
		scan.GetToken();
		if (scan.Token == _T("resolution"))
		{
			m_nResWidth = scan.GetNumber();
			m_nResHeight = scan.GetNumber();
		}
		else if (scan.Token == _T("fullscreen"))
			m_bStartFullScreen = scan.GetNumber();
		else if (scan.Token == _T("texture"))
			m_nTextureQuality = scan.GetNumber();
		else if (scan.Token == _T("detail"))
			m_nObjectDetail = scan.GetNumber();
		else if (scan.Token == _T("distant"))
			m_nObjectDistant = scan.GetNumber();
		else if (scan.Token == _T("shadow"))
			m_nShadow = scan.GetNumber();
		else if (scan.Token == _T("WindowAlpha"))
			m_nWindowAlpha = scan.GetNumber();
		else if (scan.Token == _T("InstantHelp"))
			m_nInstantHelp = scan.GetNumber();
		else if (scan.Token == _T("saveAccount"))
			m_bSaveAccount = scan.GetNumber();
		else if (scan.Token == _T("account"))
		{
			scan.GetTokenEx();
			strcpy(m_szAccount, scan.Token);
		}
		else if (scan.Token == _T("notice"))
			m_bNotice = scan.GetNumber();
		else if (scan.Token == _T("Trade"))
			m_bTrade = scan.GetNumber();
		else if (scan.Token == _T("Say"))
			m_bSay = scan.GetNumber();
		else if (scan.Token == _T("Party"))
			m_bParty = scan.GetNumber();
		else if (scan.Token == _T("MessengerJoin"))
			m_bMessengerJoin = scan.GetNumber();
		else if (scan.Token == _T("Messenger"))
			m_bMessenger = scan.GetNumber();
		else if (scan.Token == _T("BattleBGM"))
			m_bBattleBGM = scan.GetNumber();
		else if (scan.Token == _T("Server"))
		{
			m_nSer = scan.GetNumber();
			m_nMSer = scan.GetNumber();
		}
		else if (scan.Token == _T("Sound"))
		{
			m_fEffectVolume = scan.GetFloat();
			m_fBGMVolume = scan.GetFloat();
		}
		else if (scan.Token == _T("ChatFilter0"))
		{
			m_dwChatFilter[0] = scan.GetNumber();
			m_dwChatFilter[0] &= ~(CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME);
		}
		else if (scan.Token == _T("ChatFilter1"))
		{
			m_dwChatFilter[1] = scan.GetNumber();
			m_dwChatFilter[1] &= ~(CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME);
		}
		else if (scan.Token == _T("ChatFilter2"))
		{
			m_dwChatFilter[2] = scan.GetNumber();
			m_dwChatFilter[2] &= ~(CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME);
		}
		else if (scan.Token == _T("ChatFilter3"))
		{
			m_dwChatFilter[3] = scan.GetNumber();
			m_dwChatFilter[3] &= ~(CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME);
		}
		else if (scan.Token == _T("ChatFilter4"))
		{
			m_dwChatFilter[4] = scan.GetNumber();
			m_dwChatFilter[4] &= ~(CHATSTY_HELP | CHATSTY_SYSTEM | CHATSTY_GAME);
		}
		else if (scan.Token == _T("ChatFilterEtc"))
		{
			m_dwChatFilterEtc = CHATSTY_HELP_CHAT | CHATSTY_SYSTEM_CHAT | CHATSTY_GAME_CLIENT;
		}
		else if (scan.Token == _T("DamageRender"))
		{
			m_bDamageRender = scan.GetNumber();
		}
		else if (scan.Token == _T("SFXRenderOff"))
			m_bSFXRenderOff = scan.GetNumber();
		else if (scan.Token == _T("CameraLock"))
			m_bCameraLock = scan.GetNumber();
		else if (scan.Token == _T("AutoAttack"))
			m_bAutoAttack = scan.GetNumber();
		else if (scan.Token == _T("PRenderName"))
			m_nPlayerName = scan.GetNumber();
		else if (scan.Token == _T("OPRenderName"))
			m_nOtherPlayerName = scan.GetNumber();
		else if (scan.Token == _T("NRenderName"))
			m_nMonName = scan.GetNumber();
		else if (scan.Token == _T("BUFFSTATUSMODE"))
			m_BuffStatusMode = scan.GetNumber();
		else if (scan.Token == _T("ZOOMLIMIT"))
			m_bZoomLimit = scan.GetNumber();
		else if (scan.Token == _T("SFXItemRenderOff"))
			m_bSFXItemRenderOff = scan.GetNumber();
		else if (scan.Token == _T("WEATHEREFFECT"))
			m_nWeatherEffect = scan.GetNumber();
		else if (scan.Token == _T("nCurFold"))
			m_nCurFold = scan.GetNumber();
#ifdef __FL_UPDATE_TASKBAR
		else if (scan.Token == _T("RekeyTask"))
			m_bRekeyTask = scan.GetNumber();
#endif
#ifdef __FL_PET_FILTER
		else if (scan.Token == _T("LOOTTYPE"))
			m_nLootType = scan.GetNumber();
		else if (scan.Token == _T("LOOTPLACE"))
		{
			m_nLootPlace = scan.GetNumber();
			if (m_nLootPlace < TYPE_INVENTORY || m_nLootPlace >= TYPE_MAX)
				m_nLootPlace = TYPE_INVENTORY;
		}
#endif
		else if (scan.Token == _T("NIGHT"))
			m_bNightMode = scan.GetNumber();
#ifdef __AIO_PETS
		else if (scan.Token == _T("LooterWindow"))
			m_bHideWindowLooter = scan.GetNumber();
		else if (scan.Token == _T("PetWindow"))
			m_bHideWindowPet = scan.GetNumber();
#endif
#ifdef __FL_SAVE_MESSAGES
		else if (scan.Token == _T("BigMessenger"))
			m_bBigMessenger = scan.GetNumber();
#endif

	} while (scan.tok != FINISHED);

	CheckResolution();
	return 1;
}

int	COption::Save(LPCTSTR szFileName)
{
	FILE* fp = fopen(szFileName, _T("wt"));
	if (fp == NULL)
		return 0;

	_ftprintf(fp, _T("resolution %d %d\n"), m_nResWidth, m_nResHeight);
	_ftprintf(fp, _T("fullscreen %d\n"), m_bStartFullScreen);
	_ftprintf(fp, _T("texture %d\n"), m_nTextureQuality);
	_ftprintf(fp, _T("detail %d\n"), m_nObjectDetail);
	_ftprintf(fp, _T("distant %d\n"), m_nObjectDistant);
	_ftprintf(fp, _T("shadow %d\n"), m_nShadow);
	_ftprintf(fp, _T("WindowAlpha %d\n"), m_nWindowAlpha);
	_ftprintf(fp, _T("InstantHelp %d\n"), m_nInstantHelp);
	_ftprintf(fp, _T("saveAccount %d\n"), m_bSaveAccount);

	if (m_bSaveAccount)
		_ftprintf(fp, _T("account \"%s\"\n"), m_szAccount);
	else
		_ftprintf(fp, _T("account \"%s\"\n"), "");

	_ftprintf(fp, _T("notice %d\n"), m_bNotice);
	_ftprintf(fp, _T("Trade %d\n"), m_bTrade);
	_ftprintf(fp, _T("Say %d\n"), m_bSay);
	_ftprintf(fp, _T("Party %d\n"), m_bParty);
	_ftprintf(fp, _T("MessengerJoin %d\n"), m_bMessengerJoin);
	_ftprintf(fp, _T("Messenger %d\n"), m_bMessenger);
	_ftprintf(fp, _T("BattleBGM %d\n"), m_bBattleBGM);
	_ftprintf(fp, _T("Server %d %d\n"), m_nSer, m_nMSer);
	_ftprintf(fp, _T("Sound %.2f %.2f\n"), m_fEffectVolume, m_fBGMVolume);
	_ftprintf(fp, _T("ChatFilter0 %d\n"), m_dwChatFilter[0]);
	_ftprintf(fp, _T("ChatFilter1 %d\n"), m_dwChatFilter[1]);
	_ftprintf(fp, _T("ChatFilter2 %d\n"), m_dwChatFilter[2]);
	_ftprintf(fp, _T("ChatFilter3 %d\n"), m_dwChatFilter[3]);
	_ftprintf(fp, _T("ChatFilter4 %d\n"), m_dwChatFilter[4]);
	_ftprintf(fp, _T("ChatFilterEtc %d\n"), m_dwChatFilterEtc);
	_ftprintf(fp, _T("DamageRender %d\n"), m_bDamageRender);
	_ftprintf(fp, _T("SFXRenderOff %d\n"), m_bSFXRenderOff);
	_ftprintf(fp, _T("CameraLock %d\n"), m_bCameraLock);
	_ftprintf(fp, _T("AutoAttack %d\n"), m_bAutoAttack);
	_ftprintf(fp, _T("PRenderName %d\n"), m_nPlayerName);
	_ftprintf(fp, _T("OPRenderName %d\n"), m_nOtherPlayerName);
	_ftprintf(fp, _T("NRenderName %d\n"), m_nMonName);
	_ftprintf(fp, _T("BUFFSTATUSMODE %d\n"), m_BuffStatusMode);
	_ftprintf(fp, _T("ZOOMLIMIT %d\n"), m_bZoomLimit);
	_ftprintf(fp, _T("SFXItemRenderOff %d\n"), m_bSFXItemRenderOff);
	_ftprintf(fp, _T("WEATHEREFFECT %d\n"), m_nWeatherEffect);
	_ftprintf(fp, _T("nCurFold %d\n"), m_nCurFold);
#ifdef __FL_UPDATE_TASKBAR
	_ftprintf(fp, _T("RekeyTask %d\n"), m_bRekeyTask);
#endif
#ifdef __FL_PET_FILTER
	_ftprintf(fp, _T("LOOTTYPE %d\n"), m_nLootType);
	_ftprintf(fp, _T("LOOTPLACE %d\n"), m_nLootPlace);
#endif
	_ftprintf(fp, _T("NIGHT %d\n"), m_bNightMode);
#ifdef __AIO_PETS
	_ftprintf(fp, _T("LooterWindow %d\n"), m_bHideWindowLooter);
	_ftprintf(fp, _T("PetWindow %d\n"), m_bHideWindowPet);
#endif
#ifdef __FL_SAVE_MESSAGES
	_ftprintf(fp, _T("BigMessenger %d\n"), m_bBigMessenger);
#endif


	fclose(fp);
	return 1;
}
