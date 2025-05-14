#ifndef __WNDMESSENGER__H
#define __WNDMESSENGER__H

#include "WndFriendCtrl.h"
#include "WndPartyCtrl.h"
#include "WndGuildCtrl.h"
#include "WndMessengerCtrl.h"

class CWndMessengerEx : public CWndNeuz
{
public:
	CWndMessengerEx();
	~CWndMessengerEx();

	CWndMenu			m_menuState;
	CWndFriendCtrlEx	m_wndFriend;
	CWndGuildCtrlEx		m_wndGuild;
	CTexture			m_TexMail;
	int					m_nFlashCounter;
	int					m_nSwitch;

	virtual	BOOL Process();
//	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );

	void UpdateFriendList();
	void UpdateGuildMemberList();

#ifdef __FL_SAVE_MESSAGES
	CWndMenu m_PostMessagesMenu;
	void UpdateList();
	void SetExtension(BOOL eflag);
#endif
};

class CWndInstantMsg : public CWndNeuz
{
public:
	CTimer m_timer;
	CString m_strMessage;
	CWndInstantMsg();
	~CWndInstantMsg();

	CString m_strPlayer;

	void AddMessageJoin( LPCTSTR lpszJoinName );
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage );
	void AddPostMessage( LPCTSTR lpszSendName );

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize( UINT nType, int cx, int cy );
	virtual void OnLButtonUp( UINT nFlags, CPoint point );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
};
class CWndMessage : public CWndNeuz
{
public:
	CWndMessage();
	~CWndMessage();

	CString m_strPlayer;

	void InitSize( void );
#ifdef __FL_SAVE_MESSAGES
	void AddMessage( LPCTSTR lpszFrom, LPCTSTR lpszMessage, CTime time = 0);
#else
	void AddMessage(LPCTSTR lpszFrom, LPCTSTR lpszMessage);
#endif
	void OnInputString();

	virtual void SetWndRect( CRect rectWnd, BOOL bOnSize );
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
