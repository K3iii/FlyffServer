#ifndef __DPACCOUNTCLIENT_H__
#define	__DPACCCOUNTCLIENT_H__

#include "dpmng.h"
#include "msghdr.h"

#undef	theClass
#define theClass	CDPAccountClient
#undef theParameters
#define theParameters	CAr & ar, DPID

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

	void	SendAddAccount(char* lpAddr, LPCTSTR lpszAccount, BYTE cbAccountFlag, DPID idFrom, int fCheck, const char* szBak);

	void	SendRemoveAccount( DPID idFrom );
	void	SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom );
	void	SendPing( DPID idFrom );
	void	SendCloseExistingConnection( const char* lpszAccount );

	USES_PFNENTRIES;
//	Handlers
	void	OnAddAccount( CAr & ar, DPID dpid );
	void	OnDestroyPlayer( CAr & ar, DPID dpid );
	void	OnServersetList( CAr & ar, DPID dpid );
	void	OnPlayerCount( CAr & ar, DPID dpid );

	void	OnEnableServer( CAr & ar, DPID dpid );
};

#endif	// __DPACCOUNTCLIENT_H__