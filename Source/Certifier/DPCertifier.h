#ifndef __DPCERTIFIER_H__
#define	__DPCERTIFIER_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

using	namespace	std;

#undef	theClass
#define theClass	CDPCertifier
#undef theParameters
#define theParameters	CAr & ar, DPID, LPBYTE, u_long


class CLoginLimit
{
private:
	int		m_nLimitation;
	DWORD	m_dwTick;
	int		m_nRequestPerSecond;

public:
	CLoginLimit();
	bool	Check();
	void	SetLimitation( int nLimitation )
	{
		m_nLimitation = nLimitation;
	}
};

class CDPCertifier : public CDPMng
{
public:
	CLoginLimit		m_limit;
	DWORD			m_dwSizeofServerset;
	SERVER_DESC		m_aServerset[128];
	char	m_szVer[32];
#ifdef __SECURITY_0628
	char	m_szResVer[100];
#endif	// __SECURITY_0628
	map<u_long, LPSERVER_DESC>	m_2ServersetPtr;
public:
//	Constructions
	CDPCertifier();
	virtual	~CDPCertifier();
// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
#ifdef __TEST_PACKET_CRYPT
	virtual BOOL	Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo);
#endif
	void	SendServerList(DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szBak);
	void	SendError( LONG lError, DPID dpid );
	void	SendHdr( DWORD dwHdr, DPID dpid );
	void	SetLimitation( int nLimit );

	USES_PFNENTRIES;

	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnPing( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
};

inline void CDPCertifier::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, dpid );
}

#endif	// __DPCERTIFIER_H__