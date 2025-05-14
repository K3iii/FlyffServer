#ifndef __WNDRANKGUILD__H
#define __WNDRANKGUILD__H


class CWndRankTabUnited : public CWndNeuz
{
public:
	CWndRankTabUnited();
	~CWndRankTabUnited();

	int		m_nCurrentList;
	int		m_nMxOld, m_nMyOld;

	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

class CWndRankTabPenya : public CWndNeuz
{
public:
	CWndRankTabPenya();
	~CWndRankTabPenya();

	int		m_nCurrentList;
	int		m_nMxOld, m_nMyOld;

	virtual BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	virtual void OnMouseMove(UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

class CWndRankGuild : public CWndNeuz
{
public:
	CWndRankGuild();
	~CWndRankGuild();

	CWndRankTabUnited	m_WndRankTabUnited;
	CWndRankTabPenya	m_WndRankTabPenya;

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};

#endif