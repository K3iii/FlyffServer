#ifndef __DPCACHESRVR_H__
#define __DPCACHESRVR_H__

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define theClass	CDPCacheSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID, LPBYTE, u_long

class CDPCacheSrvr : public CDPMng
{
private:

public:
#ifdef __VALID_CONNECTION_CONFORMATION
	std::map<DPID, time_t> m_socketTimer;
#endif

	// Constructions
	CDPCacheSrvr();
	virtual	~CDPCacheSrvr();

	// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
#ifdef __TEST_PACKET_CRYPT
	virtual BOOL	Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo);
#endif

	USES_PFNENTRIES;

	// Handlers
	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveConnection( DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE, u_long );
};


#endif