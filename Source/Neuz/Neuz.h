#ifndef __NEUZAPP_H
#define __NEUZAPP_H

#define WM_LOGOUT ( WM_USER + 10 )

#define CUR_ATTACK       0
#define CUR_BASE         1
#define CUR_CAMERA       2
#define CUR_CONTROL      3
#define CUR_DELAY        4
#define CUR_EDIT         5
#define CUR_GETITEM      6
#define CUR_HELP         7
#define CUR_NO           8
#define CUR_NPC          9
#define CUR_SELECT       10
#define CUR_RESIZE_HORI  11
#define CUR_RESIZE_VERT  12
#define CUR_RESIZE_HV1   13
#define CUR_RESIZE_HV2   14
#define CUR_DOOR	     15
#define CUR_GATE	     16
#define CUR_LEVER        17
#define CUR_HAMMER       18

#ifdef __DISCORD
enum Discord
{
	DIS_START, // Start 
	DIS_END, // End
	DIS_DEFAULT_DETAILS, // Default details
	DIS_CHARACTER_DETAILS // Character details
};
#endif

#ifdef __FL_SAVE_MESSAGES
struct PRIVATE_MESSAGES
{
	CString str;
	BOOL bIsFromMe;
	CTime cTime;
};
#endif

#include "NeuzEnemy.h"

class CNeuzApp : public CD3DApplication
{
public:
	CNeuzApp();

public:
#ifdef __FL_CASH_SHOP
	int m_nDonatePoints;
	int m_nVotePoints;
#endif
	int m_nRealWeather;
#ifdef __FL_GW_STATUE_V2
	CTexture				m_IconStatueQuestion;
#endif
	DWORD					m_dwTimeOutDis;
	int						m_nTexAlpha;
	BOOL					m_bTexLoad;
	BOOL					m_bTexLoadAlpha;
	CTexture				m_TexLoading;
	BOOL					m_bActiveNeuz;
	CNeuzEnemy				m_NeuzEnemy;
	BOOL					m_bQuit;
	CTimer					m_timerQuit;//.Set( SEC( 1 ) );
	CTexturePack			m_texQuestEmoticon;
	CTexture				m_texChar;
	C2DRender				m_2DRender;
	HCURSOR					m_ahCursor[20];
	TCHAR					m_szCaptureFileName[64];
	BOOL					m_bCapture;
	int						m_nCaptureCnt;
	CTimer					m_timerConnect;
	DWORD					m_dwTempMessage;
	BOOL					m_bLButtonDown;
	BOOL					m_bRButtonDown;
	CBackCamera				m_camera;
	BOOL					m_bFrameSkip;
	CTexture				m_TextureGauge[5];
	CTexture				m_TextureTurboGauge[2];
	CTexture				m_TextureCastingGauge[2];
	CStringArray			m_strArray;
	TCHAR					m_lpCertifierAddr[64];
	TCHAR					m_lpCacheAddr[16];
	u_short					m_uCachePort;						// 15400, 5400
	TCHAR					m_szAccount[MAX_ACCOUNT];			// account
	TCHAR					m_szPassword[MAX_PASSWORD];			// password

	DWORD					m_dwSys;
	DWORD					m_dwAuthKey;
	CMover* m_apPlayer[MAX_CHARACTER_LIST];
	DWORD					m_adwWorldID[MAX_CHARACTER_LIST];
	int						m_nCharacterBlock[MAX_CHARACTER_LIST];
	CRTMessenger	m_aRTMessenger[MAX_CHARACTER_LIST];

	u_long					m_idPlayer;
	DWORD					m_dwPingTime;
	short					m_nPingCount;
	BYTE					m_cbAccountFlag;
	UINT					m_nLeftTimeNotifyStatus;
	DWORD					m_dwShoutLimitSecond;
	int						m_nShoutLimitCount;
	u_long					m_uIdofMulti;
	BOOL					m_b18Server;
	HANDLE					m_hThread;
	DWORD					m_dwSummonPartyData;
	OBJID					m_dwSummonPartyObjid;
	char					m_szSummonPartyWorldName[128];
	CTexture				m_TextureHPGauge[3];
	CTexture* m_pMasterIcon[6];
	CTexture* m_pHeroIcon;
#if __VER >= 11 //	__SYS_COLLECTING
	CTexture				m_TexCltGauge[2];
#endif
#if __VER >= 11 // __MOD_VENDOR
	CItemElem				m_aSavedInven[MAX_VENDITEM];
#endif

#if __VER >= 12 // __ITEMCREATEMON_S0602
	D3DXVECTOR3				m_vCursorPos;
	CItemElem* m_pCreateMonItem;
#endif // __ITEMCREATEMON_S0602

#ifdef __FL_SAVE_MESSAGES
	map<string, vector<PRIVATE_MESSAGES>> m_mapPrivateMessages;
#endif

	CWndBase* pOldModalWindow;

	struct
	{
		char	chLeft;		// 'A'
		char	chUp;		// 'W'
		char	chTrace;	// 'Z'
		char	chQuest;	// 'Q'
		char	chWalk;	// 'X'
	}	Key;


public:
	void UpdateCursor();

protected:
	HRESULT		OneTimeSceneInit();
	HRESULT		InitDeviceObjects();
	HRESULT		RestoreDeviceObjects();
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();
	HRESULT		FinalCleanup();
	HRESULT		Render();
	HRESULT		FrameMove();
	HRESULT		ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior,
		D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat);
	void		HashMD5(char* szResult, LPCTSTR szPWD);

public:

#ifdef __DISCORD
	void ManageDiscord(int nType);
#endif

	void		ResetStaticValues();

	LRESULT		MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT		CaptureBitmap();
	void		SetAccountInfo(LPCTSTR lpszAccount, LPCTSTR lpszpw);
	BOOL		LoadOption();
	void		ApplyWidthHeightChanges();
	BOOL		KillWindow(int wParam);
	void		BeginLoadThread();
	void		WaitLoading();

#ifdef __FL_FIX_LOADING
	BOOL IsAlreadyRunning();
	void StartDelay(BOOL bStart);
#endif
};

extern LPCTSTR GetProtocolVersion();
extern void SetMouseCursor(DWORD dwCursor);

extern CNeuzApp g_Neuz;
#ifdef __LINK_PORTAL
extern BOOL	g_bBuddyFlag;
#endif //__LINK_PORTAL

#endif