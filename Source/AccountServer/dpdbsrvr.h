#ifndef __DPDBSRVR_H__
#define	__DPDBSRVR_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPDBSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize

class CAccount;

class CDPDBSrvr : public CDPMng<CBuffer>
{
public:
//	Constructions
	CDPDBSrvr();
	virtual	~CDPDBSrvr();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

// Operations
	void	SendOneHourNotify( CAccount* pAccount );
	void	SendPlayerCount( void );
	void	OnRemoveConnection( DPID dpid );
	void	SendCloseExistingConnection( const char*lpszAccount, LONG lError );
	void	SendFail( long lError, const char* lpszAccount, DPID dpid );

#ifdef __LOG_PLAYERCOUNT_CHANNEL
	vector<CString>	m_vecstrChannelAccount;
#endif // 

//	Handlers
	USES_PFNENTRIES;
	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAccount( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCreatePlayer(CAr& ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize);
#ifdef __REMOVE_PLAYER_0221
	void	OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
#endif	// 
	void	OnJoin( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAllAccounts( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnServerEnable( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
};

#endif	// __DPDBSRVR_H__