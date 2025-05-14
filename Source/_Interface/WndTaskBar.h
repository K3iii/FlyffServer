// WndArcane.h: interface for the CWndNeuz class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDTASKBAR_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)
#define AFX_WNDTASKBAR_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndShortcut : public CWndButton
{
public:
	CWndShortcut();
	~CWndShortcut();

	virtual void OnDraw(C2DRender* p2DRender);
	virtual	BOOL Process();
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnMouseMove(UINT nFlags, CPoint point);

};

class CWndTaskBar : public CWndNeuz
{
	DWORD m_dwHighAlpha;
#ifdef __FL_UPDATE_TASKBAR
	DWORD m_dwHighAlphaClick[MAX_SLOT_ITEM];
#endif
public:

	BOOL		   IsShortcut( LPSHORTCUT lpShortcut, DWORD dwShortcut, DWORD dwId );
	CWndMenu       m_menuShortcut;
	CWndButton     m_wndMenu;
	CWndButton     m_wndQuickList;
	CWndButton     m_wndQuickListUp;
	CWndButton     m_wndQuickListDn;
	SHORTCUT       m_aSlotApplet[ MAX_SLOT_APPLET ]; // 1 ~ 20
	SHORTCUT       m_aSlotItem  [ MAX_SLOT_ITEM_COUNT ][ MAX_SLOT_ITEM ]; // 1 ~ 0(10)
	SHORTCUT       m_aSlotQueue [ MAX_SLOT_QUEUE  ];
	LPSHORTCUT     m_paSlotItem;
	LPSHORTCUT     m_pSelectShortcut;
	SHORTCUT       m_aSlotSkill;
	CTexture*      m_pTexture;
	int            m_nMaxSlotApplet;
	BOOL           m_bStartTimeBar;
	int            m_nUsedSkillQueue;
	int            m_nPosition;
	int            m_nSkillBar;
	int            m_nCurQueueNum;
	int            m_nCurQueue;
	CTimer         m_timer;
	int			   m_nExecute;
	OBJID		   m_idTarget;
	int            m_nSlotIndex;
	int		       m_nActionPoint;
	void RenderLordCollTime( CPoint pt, DWORD dwSkillId, C2DRender* p2DRender );
	void RenderCollTime( CPoint pt, DWORD dwSkillId, C2DRender* p2DRender );
	void UpdateItem();
	void PutTooTip( LPSHORTCUT pShortcut, CPoint point, CRect* pRect );
	void SetItemSlot( int nSlot );
	BOOL CheckAddSkill( int nSkillStyleSrc, int nQueueDest );
	BOOL UseSkillQueue( CCtrl* pTargetObj );
	LPSKILL GetCurrentSkillQueue();
	BOOL SetSkillQueue( int nIndex, DWORD dwType, DWORD dwId, CTexture* pTexture = NULL );
	BOOL SetShortcut(int nIndex, DWORD dwShortcut, DWORD dwType, DWORD dwId, CTexture* pTexture, int nWhere, int nLocation);
	void OnCancelSkill( void );
	LPSHORTCUT Select( CPoint point );
	void RemoveSkillQueue( int nIndex, BOOL bSend = TRUE );
	void InitTaskBar();

	CWndTaskBar();
	virtual ~CWndTaskBar();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual BOOL Process();
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual	void OnDestroyChildWnd( CWndBase* pWndChild );
	void	Serialize( CAr & ar );
	void	SetTaskBarTexture( LPSHORTCUT pShortcut );
	void Copy(CWndTaskBar* pWndOld);

#ifdef __FL_UPDATE_TASKBAR
	void RenderClicks(C2DRender* p2DRender);
	void DrawHotkeys(C2DRender* p2DRender, CPoint pt, CString str);
#endif

	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
protected:
	void RenderOutLineLamp(int x, int y, int num, DWORD size);
};


class CWndTaskMenu : public CWndMenu //Neuz
{
public:
	CWndTaskMenu();
	virtual ~CWndTaskMenu();
	void SetTexture( CWndButton* pWndButton );
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual CWndButton* AppendMenu( CWndMenu* pWndMenu, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem );
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL Process();
	virtual void OnKillFocus(CWndBase* pNewWnd);
private:
	CWndMenu* m_pMenu1;
	CWndMenu* m_pMenu2;
	CWndMenu* m_pMenu3;
	CWndMenu* m_pMenu4;
};

#endif // !defined(AFX_WNDTASKBAR_H__A93F3186_63D6_43C1_956F_EC8691E0C7D9__INCLUDED_)

