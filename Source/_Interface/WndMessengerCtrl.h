#if __VER >= 11 // __CSC_VER11_4

#ifndef __WNDMESSENGERCTRL_H
#define __WNDMESSENGERCTRL_H

class C2DRender;

typedef struct __MESSENGER_PLAYER
{
	int		m_nChannel;
	DWORD	m_dwStatus;
	int		m_nLevel;
	int		m_nJob;
	DWORD	m_dwPlayerId;
	TCHAR	m_szName[ 64 ];
	BOOL	m_bBlock;
	BOOL	m_bVisitAllowed;
	__MESSENGER_PLAYER( void );
	void Initialize( void );
} __MESSENGER_PLAYER;

class CWndFriendCtrlEx : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	CWndScrollBar m_wndScrollBar;
	CWndMenu m_menu;
	vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	enum {SORT_BY_CHANNEL, SORT_BY_STATUS, SORT_BY_LEVEL, SORT_BY_JOB, SORT_BY_NAME};
	BOOL m_bSortbyChannel;
	BOOL m_bSortbyStatus;
	BOOL m_bSortbyLevel;
	BOOL m_bSortbyJob;
	BOOL m_bSortbyName;
	int m_nCurSort;

public:
	CWndFriendCtrlEx();
	~CWndFriendCtrlEx();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );

	// Overridables
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnRButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	// Sort Func.
	void SortbyChannel(BOOL bCheckbefore = TRUE);
	void SortbyStatus(BOOL bCheckbefore = TRUE);
	void SortbyLevel(BOOL bCheckbefore = TRUE);
	void SortbyJob(BOOL bCheckbefore = TRUE);
	void SortbyName(BOOL bCheckbefore = TRUE);
	void UpdatePlayerList();

	// UI Func.
	void SetScrollBar();
	int		GetSelect( CPoint point, u_long & idPlayer, Friend** ppFriend );
	void	GetSelectFriend( int SelectCount, u_long & idPlayer, Friend** ppFriend );
	u_long GetSelectId( int SelectCount );
	void ScrollBarPos( int nPos );
	int GetDrawCount( void );
};

class CWndGuildCtrlEx : public CWndBase
{
public:
	int m_nCurSelect;
	int m_nFontHeight;
	int m_nDrawCount;
	CWndScrollBar m_wndScrollBar;
	vector < __MESSENGER_PLAYER > m_vPlayerList;

private:
	enum {SORT_BY_CHANNEL, SORT_BY_STATUS, SORT_BY_LEVEL, SORT_BY_JOB, SORT_BY_NAME};
	BOOL m_bSortbyChannel;
	BOOL m_bSortbyStatus;
	BOOL m_bSortbyLevel;
	BOOL m_bSortbyJob;
	BOOL m_bSortbyName;
	int m_nCurSort;

public:
	CWndGuildCtrlEx();
	~CWndGuildCtrlEx();

	void Create( RECT& rect, CWndBase* pParentWnd, UINT nID );

	// Overridables
	virtual	void SetWndRect(CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnInitialUpdate();
	virtual void OnDraw( C2DRender* p2DRender );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	// Sort Func.
	void SortbyChannel(BOOL bCheckbefore = TRUE);
	void SortbyStatus(BOOL bCheckbefore = TRUE);
	void SortbyLevel(BOOL bCheckbefore = TRUE);
	void SortbyJob(BOOL bCheckbefore = TRUE);
	void SortbyName(BOOL bCheckbefore = TRUE);
	void UpdatePlayerList();

	// UI Func.
	void SetScrollBar();
	int		GetSelect( CPoint point, u_long & idPlayer, CGuildMember** lppGuildMember );
	u_long GetSelectId( int SelectCount );
	void ScrollBarPos( int nPos );
	int GetDrawCount( void );
};

#endif //__WNDMESSENGERCTRL_H

#endif //__CSC_VER11_4