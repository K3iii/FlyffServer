#ifndef __HWOPTION_H__
#define	__HWOPTION_H__

class COption
{
public:
	TCHAR	m_szFileName[MAX_PATH];
	int		m_nResWidth;
	int		m_nResHeight;

	BOOL	m_bStartFullScreen;
	int		m_nTextureQuality;
	int		m_nObjectDetail;
	int     m_nObjectDistant;
	int     m_nShadow;
	BOOL    m_bOperator;
	DWORD   m_dwChatFilter[5];
	DWORD   m_dwChatFilterEtc;
	int		m_nMonName;
	int		m_nPlayerName;
	int		m_nOtherPlayerName;
	int		m_nToolTipTexture;

	BOOL	m_nWeatherEffect;
	float	m_fEffectVolume;
	float	m_fBGMVolume;
	int     m_nWindowAlpha; // 0 ~ 255
	int     m_nWindowEffect;
	int     m_nInstantHelp;


	BOOL	m_bSay;
	BOOL	m_bTrade;
	BOOL	m_bParty;
	BOOL	m_bMessengerJoin;
	BOOL	m_bMessenger;
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	BOOL	m_bBattleBGM;
#endif // __IMPROVE_SYSTEM_VER15

	int		m_nSer;
	int		m_nMSer;

	BOOL    m_bSaveAccount;
	TCHAR   m_szAccount[ 64 ];
	BOOL    m_bNotice;
	int		m_nCurFold;
#ifdef __FL_UPDATE_TASKBAR
	BOOL m_bRekeyTask;
#endif
#ifdef __FL_PET_FILTER
	int		m_nLootType;
	BYTE	m_nLootPlace;
#endif
	BOOL m_bNightMode;
#ifdef __AIO_PETS
	BOOL m_bHideWindowLooter;
	BOOL m_bHideWindowPet;
#endif
#ifdef __FL_SAVE_MESSAGES
	BOOL m_bBigMessenger;
#endif

	BOOL	m_bDamageRender;
	BOOL	m_bSFXItemRenderOff;
	BOOL	m_bSFXRenderOff;
	BOOL	m_bCameraLock;
	BOOL	m_bAutoAttack;

	int m_BuffStatusMode;
	BOOL	m_bZoomLimit;
	COption();
	~COption();

	void Init();
	int Load( LPCTSTR szFileName );
	int Save( LPCTSTR szFileName );
	void CheckResolution();
};

extern COption		g_Option;

#endif
