#ifndef __WNDGUILD_TAB_APP__H
#define __WNDGUILD_TAB_APP__H

#include "guild.h"
extern	CGuildMng	g_GuildMng;

class CWndGuildTabApp : public CWndNeuz
{
public:
	void UpdateData();
	void EnableButton( BOOL bEnable );
	CWndGuildTabApp();
	~CWndGuildTabApp();

	DWORD	m_adwPower[MAX_GM_LEVEL];

	void SetData( DWORD dwPower[] );
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