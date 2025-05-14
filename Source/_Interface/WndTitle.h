// WndArcane.h: interface for the CWndNeuz class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDTITLE_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDTITLE_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DECLARE_WNDMESSAGEBOX( CWndConnectingBox )
DECLARE_WNDMESSAGEBOX( CWndDisconnectedBox )
DECLARE_WNDMESSAGEBOX( CWndCharBlockBox )
DECLARE_WNDMESSAGEBOX( CWndAllCharBlockBox )

#ifdef __NPKCRYPT
#include "npkcrypt.h"
#endif	// __NPKCRYPT

class CWndLogin : public CWndNeuz
{
	//CTexture   m_Texture;
public:
#ifdef __NPKCRYPT
	HKCRYPT		m_hKCrypt;
#endif	// __NPKCRYPT

	BOOL m_bDisconnect;

	CWndEdit   m_wndAccount     ;
	CWndEdit   m_wndPassword    ;
	CWndButton m_wndSaveAccount ;
	CWndButton m_wndSavePassword;
	CWndButton m_wndFindAccount ;

	CWndButton m_wndLogin    ;
	CWndButton m_wndRegist   ;
	CWndButton m_wndQuit     ;

	void Connected( long lTimeSpan );

	CWndLogin();
	virtual ~CWndLogin();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwStyle = 0);
	virtual BOOL Process ();
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

#ifdef __CON_AUTO_LOGIN
	void SetAccountAndPassword( const CString& account, const CString& pass );
#endif
};

BEGIN_WNDCLASS( CWndSelectServer )
public:
	vector<CString> m_vecStrBanner;
	DWORD		 m_dwChangeBannerTime;
	LPIMAGE		 m_atexPannel;

	CWndEdit     m_wndURL       ;
	CWndButton   m_wndSearch    ;
	CWndListCtrl	m_wndServerList;
	CWndButton   m_wndBack      ;
	CWndButton   m_wndAccept    ;

	virtual BOOL Process ();
	void AfterSkinTexture( LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	void Connected();
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
END_WNDCLASS


BEGIN_WNDCLASS( CWndDeleteChar )
public:
/*
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
*/
	void DeletePlayer( int nSelect, LPCTSTR szNo );
	virtual void AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4 );
	void InsertPassword(int nPasswordNumber);

END_WNDCLASS

class CWndSelectChar : public CWndNeuz
{
public:

	CWndSelectChar();
	virtual ~CWndSelectChar();

	CWndText m_wndText1;
	CWndText m_wndText2;
	CWndText m_wndText3;

	CWndDeleteChar* m_pWndDeleteChar;
	static int m_nSelectCharacter;
	CRect m_aRect[MAX_CHARACTER_LIST];
	DWORD m_dwMotion[MAX_CHARACTER_LIST];
	BOOL m_bDisconnect;

private:
	BOOL m_CreateApply;
	int		m_nFlashAlpha;
	bool	m_bFlash;
	int		m_nChars;

public:
	void UpdateCharacter();
	void Connected();

	BOOL SetMotion(CModelObject* pModel, DWORD dwIndex, DWORD dwMotion, int nLoop, DWORD dwOption);

	void SelectCharacter(int i);

	virtual BOOL    Process();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual void	OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
};

BEGIN_WNDCLASS( CWndCreateChar )
	CWndButton   m_wndAccept   ;

	CMover m_Player;

	virtual void OnMouseWndSurface(CPoint point);
	void 			ReSetBar(FLOAT r, FLOAT g, FLOAT b);
	CRect			m_ColorRect[3];
	FLOAT			m_fColor[3];
	CPoint			m_ColorScrollBar[3];
	CPoint			m_OriginalColorScrollBar[3];
	CTexture        m_Texture;
	int				m_ChoiceBar;
	BOOL			m_bLButtonClick;

	void SetSex( int nSex );
	void Connected();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	enum { MAX_2ND_PASSWORD_NUMBER = 4 };

END_WNDCLASS

#endif // !defined(AFX_WNDTITLE_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
