#ifndef	__DPACCOUNTCLIENT_H__
#define	__DPACCOUNTCLIENT_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPAccountClient
#undef theParameters
#define theParameters	CAr & ar, LPBYTE lpBuf, u_long uBufSize

class CDPAccountClient : public CDPMng
{
public:
//	Constructions
	CDPAccountClient();
	virtual	~CDPAccountClient();
//	Overrides
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendAddIdofServer( DWORD dwIdofServer );
	void	SendRemoveAccount( char* lpString );
	void	SendRemoveAllAccounts( void );

//	Handlers
	USES_PFNENTRIES;
	void	OnGetPlayerList( CAr & ar, LPBYTE, u_long );
	void	OnCreatePlayer(CAr& ar, LPBYTE lpBuf, u_long uBufSize);
#ifdef __REMOVE_PLAYER_0221
	void	OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
#endif	// __REMOVE_PLAYER_0221
	void	OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnFail( CAr & ar, LPBYTE, u_long );

};

#endif	// __DPACCOUNTCLIENT_H__