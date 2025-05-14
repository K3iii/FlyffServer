#ifdef __EVE_MINIGAME

#ifndef __WNDMINIGAME__H
#define __WNDMINIGAME__H

class CWndKawiBawiBoGameWin;

class CWndKawiBawiBoGame : public CWndNeuz
{
	enum MyPosition {KAWI, BAWI, BO};
public:
	int m_nWinningCount;
	int m_nWinningMaxCount;
	int m_nMyChoice;
	int m_nComChoice;
	int m_nCount;
	int m_nDelay;
	int m_nResult;
	int m_nPrevResult;
	int m_nStatus;
	int m_nItemCount;
	int m_nNextItemCount;
	BOOL m_bCheckCoupon;

	DWORD m_dwRItemId;
	DWORD m_dwRNextItemId;

	CString m_strChoice[3];

	CWndStatic*	m_pStWinningCount;
	CWndStatic*	m_pStMyChoice;
	CWndStatic*	m_pStComChoice;

	CWndKawiBawiBoGameWin* m_pWndGameWin;
public:
	CWndKawiBawiBoGame();
	virtual ~CWndKawiBawiBoGame();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual BOOL Process ();
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	void ReceiveResult(int nResult, int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId);
	void CreateWinningWnd();
	void RefreshInfo();
	void DisableAllBtn();
	void EnableAllBtn();
};

class CWndKawiBawiBoGameWin : public CWndNeuz
{
public:
	CWndKawiBawiBoGame* m_pWndGame;

	int m_nItemCount;
	int m_nNextItemCount;
	DWORD m_dwItemID;
	DWORD m_dwNextItemID;

public:
	CWndKawiBawiBoGameWin();
	virtual ~CWndKawiBawiBoGameWin();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnMouseWndSurface( CPoint point );

	void SetInfo(int nItemCount, int nNextItemCount, DWORD dwItemId, DWORD dwNextItemId, int nWinningCount, int nWinningMaxCount);
};

class CWndKawiBawiBoGameConfirm : public CWndNeuz
{
public:
	CWndButton m_wndButton1;
	CWndButton m_wndButton2;
protected:
	CString m_strText;
	CWndText m_wndText;

public:
	CWndKawiBawiBoGameConfirm();
	~CWndKawiBawiBoGameConfirm();

	virtual void OnDestroy();
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};
#endif //__WNDMINIGAME__H
#endif //__EVE_MINIGAME