// WndArcane.h: interface for the CWndNeuz class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDWORLD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __BUFF_1107
#include "buff.h"
#endif	// __BUFF_1107

typedef struct tagCAPTION
{
	TCHAR m_szCaption[512];
	CD3DFontAPI* m_pFont;

	CTexture m_texture;
	CSize m_size;
	FLOAT m_fXScale;
	FLOAT m_fYScale;
	FLOAT m_fAddScale;
	int m_nAlpha;

} CAPTION, * LPCAPTION;

class CCaption
{
public:
	BOOL m_bEnd;
	CTimer m_timer;
	int m_nCount;
	CPtrArray m_aCaption;
	CCaption();
	~CCaption();

	void RemoveAll();
	void Process();

	void Render(CPoint ptBegin, C2DRender* p2DRender);
	void AddCaption(LPCTSTR lpszCaption, CD3DFontAPI* pFont, BOOL bChatLog = TRUE, DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255));

	// Initializing and destroying device-dependent objects
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT DeleteDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
};

class CCapTime
{
public:
	CD3DFontAPI* m_pFont;
	CTexture m_texture;
	CSize m_size;

	FLOAT m_fXScale;
	FLOAT m_fYScale;
	FLOAT m_fAddScale;
	int m_nAlpha;
	int m_nTime;
	int m_nStep;
	BOOL m_bRender;
	CCapTime();
	~CCapTime();

	void Process();
	void Render(CPoint ptBegin, C2DRender* p2DRender);
	void SetTime(int nTime, CD3DFontAPI* pFont);
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT DeleteDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
};

extern CCapTime g_CapTime;
extern CCaption g_Caption1;
////////////////////////////////////////////////////////////////////////////////


#define NEXTSKILL_NONE				0xffffffff
#define NEXTSKILL_ACTIONSLOT		0x7fffffff


typedef struct BUFFSKILL
{
	BOOL m_bFlsh;
	int   m_nAlpha;
	CTexture* m_pTexture;

	BUFFSKILL() { Init(); };
	~BUFFSKILL() {};

	void Init()
	{
		m_bFlsh = 0;
		m_nAlpha = 202;
		m_pTexture = NULL;
	};

} BUFFSKILL;

typedef	struct	_SET_NAVIGATOR
{
	DWORD dwWorldId;
	D3DXVECTOR3 vPos;
	_SET_NAVIGATOR()
	{
		dwWorldId = 0;
	}
}
SET_NAVIGATOR;


struct BUFFICON_INFO
{
	CPoint	pt;
	int		nCount;
	int		nDelta;
};

struct BUFFICONRECT_INFO
{
	RECT    rc;
	DWORD	dwID;
};


#define MAX_ADVBUTTON	10

typedef struct BUTTON_INFO
{
	CWndButton* m_pwndButton;
	DWORD			m_dwRunWindow;
} BUTTON_INFO, * PBUTTON_INFO;

class CAdvMgr
{
	int						m_nIndex;
	CWndBase* m_pParentWnd;
	vector<BUTTON_INFO>		m_vecButton;

public:
	CAdvMgr();
	~CAdvMgr();

	void RemoveButtons();
	void Init(CWndBase* pParentWnd);
	void AddAdvButton(DWORD dwid);
	void SetVisible(BOOL bVisible);
	BOOL RunButton(DWORD dwID);
#ifdef __FL_WHEEL_OF_FORTUNE
	BOOL DeleteButton(DWORD dwID);
#endif
	void SortButton();
	BUTTON_INFO* FindRunWindowButton(DWORD dwID);

	void DestroyChildWnd(CWndBase* pWndChild);
};

typedef struct __GUILDRATE
{
	u_long m_uidPlayer;
	int    nLife;
	BOOL   bJoinReady;
} __GUILDRATE;

typedef struct __GCWARSTATE
{
	u_long m_uidPlayer;
	BOOL   m_bWar;
} __GCWARSTATE;

typedef struct __PGUEST_TIME_TEXT
{
	BOOL bFlag;
	int  nState;
	DWORD dwQuestTime;
} __PGUEST_TIME_TEXT;

#if __VER >= 12 // __SECRET_ROOM
typedef struct __SRGUILDINFO
{
	DWORD dwGuildId;
	int nWarState;
	int nKillCount;
} __SRGUILDINFO;

typedef struct __KILLCOUNTCIPHERS
{
	BOOL bDrawMyGuildKillCount;
	char szMyGuildKillCount;
	CPoint ptPos;
	float fScaleX;
	float fScaleY;
	int nAlpha;
} __KILLCOUNTCIPHERS;

#define MAX_KILLCOUNT_CIPHERS 3
#endif //__SECRET_ROOM

#ifdef __FL_FFA
struct FFA_PLAYER
{
	u_long idPlayer;
	int nKills;
	int nDeaths;
};
#endif

#ifdef __FL_TDM
struct TDM_PLAYER
{
	u_long idPlayer;
	TDM_TEAMS team;
	int nKills;
	int nDeaths;

	TDM_PLAYER()
	{
		idPlayer = NULL_ID;
		team = TDM_TEAM_NONE;
		nKills = 0;
		nDeaths = 0;
	}
};
#endif

class CWndWorld : public CWndNeuz
{
	FLOAT m_fHigh;
	FLOAT m_fLow;
	CPoint m_ptMouseOld;
	CPoint m_ptMouseSpot;
	BOOL m_bFreeMove;
	BOOL m_bFlyMove;
	BOOL m_bSelectTarget;

public:
	CAdvMgr		m_AdvMgr;
	CObj* m_pSelectRenderObj;


	CAdvMgr* GetAdvMgr() { return &m_AdvMgr; }

	BOOL	m_IsMailTexRender;
	BOOL	m_bCtrlPushed;
	BOOL	m_bRenderFPS;

	BOOL	m_bShiftPushed;
	BOOL	m_bAngelFinish;

	CObj* m_pNextTargetObj;
	CObj* m_pRenderTargetObj;

	DWORD m_dwIdBgmMusic;
	CTexturePack m_texTarget;
	CTexturePack m_texTargetFly;
	CTexturePack m_texTargetArrow;
	CTexturePack m_texGauFlight;

	BOOL s_bAccKeyed;
	CTimer m_timerFocusAttack;
	CTimer m_timerLButtonDown;
	CTimer m_timerRButtonDown;
	DWORD  m_dwPowerTick;

	CTimer m_timerAutoRun;
	CTimer m_timerAutoRunPush;
	CTimer m_timerAutoRunBlock;
	BOOL   m_nDubleUp;

	CWndMenu m_wndMenuMover;
	BOOL m_bSetQuestNPCDest;
	D3DXVECTOR3 m_vQuestNPCDest;
	D3DXVECTOR3 m_vDestinationArrow;


	CTexture m_texLvUp;
	CTexture m_texLvDn;
	CTexture m_texLvUp2;
	CTexture m_texLvDn2;
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge;

	int nWidthOld;
	__int64 m_nMoverHPOld;
	OBJID objRenderOld;

	int		ControlPlayer(DWORD dwMessage, CPoint point);
	int		ControlGround(DWORD dwMessage, CPoint point);
	int		ControlFlying(DWORD dwMessage, CPoint point);

	//CTexture m_texTargetGauge;
	CModelObject m_meshArrow;

	DWORD			m_dwRenderArrowTime;
	D3DXVECTOR3		m_v3Dest;

	void RenderArrow();
	BOOL UseSkillToFocusObj(CCtrl* pFocusObj);
	void GetBoundRect(CObj* pObj, CRect* pRect);

	void RenderArrow_Text(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR3& vDest, const D3DXMATRIX& mat);	//gmpbigsun : refactoring

	CDWordArray		m_aFlyTarget;

	DWORD	m_dwNextSkill;
	vector <BUFFICONRECT_INFO> m_rcCheck;

	CTexture		m_TexGuildWinner;
	CTexture		m_TexGuildBest;
	multimap< int, CString >	m_mmapGuildCombat_GuildPrecedence;
	multimap< int, u_long >		m_mmapGuildCombat_PlayerPrecedence;
	vector  < __GUILDRATE >		m_vecGuildCombat_GuildStatus;
	map< u_long, vector<__GCWARSTATE> >  m_mapGC_GuildStatus;
	CWndBase* m_pWndBuffStatus;

public:
	void SetWorldMusic(BOOL bForceStop = FALSE);
	void AddGuildPrecedence(int, CString);
	void AddPlayerPrecedence(int, u_long);
	void AddGuildStatus(u_long uidPlayer, int nLife, BOOL bJoinReady);
	void AddGCStatus(u_long uidDefender, u_long uidPlayer, BOOL bWar);
	u_long GetGCStatusDefender(u_long uidDefender);
	int  IsGCStatusPlayerWar(u_long uidPlayer);
	void ClearGuildPrecedence() { m_mmapGuildCombat_GuildPrecedence.clear(); }
	void ClearPlayerPrecedence() { m_mmapGuildCombat_PlayerPrecedence.clear(); }
	void ClearGuildStatus() { m_vecGuildCombat_GuildStatus.clear(); }
	BOOL GetBuffIconRect(DWORD dwID, const CPoint& point);
	int  GetBuffTimeGap();
	void RenderOptBuffTime(C2DRender* p2DRender, CPoint& point, CTimeSpan& ct, DWORD dwColor);
	void RenderMoverBuff(CMover* pMover, C2DRender* p2DRender);

	CD3DFontAPI* m_pFontAPICaption;
	CD3DFontAPI* m_pFontAPITitle;
	CD3DFontAPI* m_pFontAPITime;

	CTexturePack	m_texMsgIcon;
	SET_NAVIGATOR	m_stnv;
	CTexturePack	m_texAttrIcon;
	CTexturePack	m_texPlayerDataIcon;

	DWORD				m_dwOneSecCount;
	ADDSMMODE			m_AddSMMode;
	CTexture* m_dwSMItemTexture[SM_MAX];
	BOOL				m_bSMFlsh[SM_MAX];
	int					m_nSMAlpha[SM_MAX];

	void UseSkill();
	CTexturePack m_texFontDigital;

	static D3DXVECTOR3 m_vTerrainPoint;

	BOOL m_bAutoAttack;
	OBJID	m_idLastTarget;
	BOOL	IsLastTarget(OBJID objid) { return(m_idLastTarget == objid); }
	void	SetLastTarget(OBJID objid) { m_idLastTarget = objid; }
	DWORD	GetNextSkill(void) { return m_dwNextSkill; }
	void	SetNextSkill(DWORD dwNextSkill)
	{
		m_dwNextSkill = dwNextSkill;
	}


	float	m_fRollAng;

	DWORD	m_dwGuildCombatTime;

	__PGUEST_TIME_TEXT m_QuestTime;

	char	m_szGuildCombatStr[64];
	struct __GuildCombatJoin
	{
		u_long uidGuild;
		char szJoinGuildName[MAX_NAME];
		int nJoinSize;
		int nOutSize;
	};

	CWndButton m_wndMenu;
	D3DXVECTOR3		m_vTelePos;

	void Projection(LPDIRECT3DDEVICE9 pd3dDevice);

	CObj* PickObj(POINT point, BOOL bOnlyNPC = FALSE);
	CObj* SelectObj(POINT point);
	CObj* HighlightObj(POINT point);

	int		m_nSelect;
	void	ClearFlyTarget(void)
	{
		m_aFlyTarget.RemoveAll();
	}

	void	AddFlyTarget(OBJID idMover)
	{
		m_aFlyTarget.Add(idMover);
	}

	OBJID	m_objidTracking;
	void	ClearTracking();

	CWndWorld();
	virtual ~CWndWorld();
	void RenderAltimeter();
	void RenderFocusObj(CObj* pObj, CRect rect, DWORD dwColor1, DWORD dwColor2);
	void SetNextTarget();
	void RenderFocusArrow(CPoint pt);
	void RenderGauFlight(C2DRender* p2DRender);

	int   m_nLimitBuffCount;
	CBuffMgr	m_buffs;
	vector< multimap<DWORD, BUFFSKILL> >	m_pBuffTexture;

	DWORD	GetSystemPetTextureKey(IBuff* pBuff);

	void RenderBuff(C2DRender* p2DRender);
	void RenderSelectObj(C2DRender* pRender, CObj* pObj);

	void RenderExpBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BUFFICON_INFO* pInfo, CPoint ptMouse, DWORD dwItemID);
	void RenderBuffIcon(C2DRender* p2DRender, IBuff* pBuff, BOOL bPlayer, BUFFICON_INFO* pInfo, CPoint ptMouse);

	void RenderSMBuff(C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse);
	void RenderCasting(C2DRender* p2DRender);

	void	RenderEventIcon(C2DRender* p2DRender, BUFFICON_INFO* pInfo, CPoint ptMouse);

	void PutPetTooltipInfo(CItemElem* pItemElem, CEditString* pEdit);

	char m_szSecretRoomStr[256];
	vector<__SRGUILDINFO> m_vecGuildList;
	__KILLCOUNTCIPHERS m_stKillCountCiphers[MAX_KILLCOUNT_CIPHERS];
	BOOL m_bFlashBackground;

	void DrawSecretRoomGuildInfo(C2DRender* p2DRender, BOOL bIsMyGuild, int nRank, __SRGUILDINFO stGuildInfo, CPoint ptRank, CPoint ptLogo,
		CPoint ptGName, CPoint ptHypoon, CPoint ptState, float fLogoScaleX, float fLogoScaleY, CRect rectBg);
	void DrawSecretRoomInfo(C2DRender* p2DRender);
	void DrawMyGuildKillCount(C2DRender* p2DRender, __SRGUILDINFO stGuildInfo, CPoint ptState, int nMax);
	void DrawOutLineLamp(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend, int nState, int nRank, BOOL bIsMyGuild);
	void DrawOutLineFlash(C2DRender* p2DRender, CRect rectBg, DWORD dwColorstart, DWORD dwColorend);


#ifdef __FL_FFA
	BYTE m_nStateFFA;
	CTimeSpan m_ctFFA;
	time_t m_tEndTimeFFA;
	time_t m_tCurrentTimeFFA;
	vector<FFA_PLAYER> vecPlayerFFA;

	void SetTimeFFA(time_t tTime) { m_tCurrentTimeFFA = 0; m_tEndTimeFFA = time_null() + tTime; }
	void SetStateFFA(BYTE nState) { m_nStateFFA = nState; };
	void RenderFFA(C2DRender* p2DRender);
	void ProcessFFA();
	void ClearFFA();
	void SortFFA();
#endif

#ifdef __FL_TDM
	BYTE m_nStateTDM;
	CTimeSpan m_ctTDM;
	time_t m_tEndTimeTDM;
	time_t m_tCurrentTimeTDM;

	vector<TDM_PLAYER> m_vecPlayersTDM;
	unordered_map<u_long, TDM_PLAYER> m_mapPlayersTDM;
	unordered_map<TDM_TEAMS, int> m_mapTeamsTDM;

	void SetTimeTDM(time_t tTime) { m_tCurrentTimeTDM = 0; m_tEndTimeTDM = time_null() + tTime; }
	void SetStateTDM(BYTE nState) { m_nStateTDM = nState; };
	void RenderTDM(C2DRender* p2DRender);
	void RenderTDM_Members(C2DRender* p2DRender);
	void RenderTDM_Teams(C2DRender* p2DRender);
	void ProcessTDM();
	void ClearTDM();
	void SortTDM();

	bool IsCanKillTDM(CMover* pDefender);
	TDM_TEAMS GetTeamByPlayerId(u_long idPlayer);
	DWORD GetColorByTeam(TDM_TEAMS team);

	CTexture m_TextureBlueTeam;
	CTexture m_TextureRedTeam;
	CTexture m_TextureFightTDM;
#endif

	void InviteParty(u_long uidPlayer);
	void InviteCompany(OBJID objId);

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwWndId = 0);
	// message
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnSetCursor(CWndBase* pWndBase, UINT nHitTest, UINT message);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnMButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnMButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL Process();
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual LRESULT WndMsgProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnDropIcon(LPSHORTCUT pShortcut, CPoint point = 0);
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	BOOL UseFocusObj(CCtrl* pFocusObj);

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

private:
	void	ShowMoverMenu(CMover* pTarget);
	int		GetGaugePower(int* pnValue);
	void	RefreshLimitBuffCount();

#if __VER >= 15 // __GUILD_HOUSE
	void	ShowCCtrlMenu(CCtrl* pCCtrl);
#endif
#if __VER >= 15 // __IMPROVE_SYSTEM_VER15
	BOOL MenuException(CPoint point);
#endif // __IMPROVE_SYSTEM_VER15
};
#endif // !defined(AFX_WNDFIELD_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
