#ifndef __DPCERTIFIED_H__
#define	__DPCERTIFIED_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"

#undef	theClass
#define theClass	CDPCertified
#undef theParameters
#define theParameters	CAr & ar, DPID

class CDPCertified : public CDPMng
{
private:
	BOOL	m_fConn;
	CTimer	m_timer;
	LONG	m_lError;					// protocol error code

public:
	DWORD		m_dwSizeofServerset;
	SERVER_DESC	m_aServerset[128];

public:
//	Constructions
	CDPCertified();
	virtual	~CDPCertified();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
#ifdef __TEST_PACKET_CRYPT
	virtual BOOL	Send(LPVOID lpData, DWORD dwDataSize, DPID dpidTo);
#endif

//	Operations
	LONG	GetNetError();
	LONG	GetErrorCode()	{ return m_lError; }
//	void	Destroy( BOOL bDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	LPCTSTR GetServerName( int nServerIndex );
	void	SendHdr( DWORD dwHdr );
#ifndef __V21_CLOSE_EXISTING_CONNECTION
	void	SendCloseExistingConnection( const char* lpszAccount, const char* lpszpw );
#else
	void	SendCloseExistingConnection();
#endif
	BOOL	CheckNofityDisconnected();
	void	SendCertify();

	USES_PFNENTRIES;

	// Handlers
	void	OnSrvrList( CAr & ar, DPID );
	void	OnError( CAr & ar, DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID );
};

inline void CDPCertified::SendHdr( DWORD dwHdr )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, DPID_SERVERPLAYER );
}
inline void CDPCertified::Ping( void )
{
	if( m_timer.IsTimeOut() )
	{
		m_timer.Reset();
		SendHdr( PACKETTYPE_PING );
	}
}

inline BOOL CDPCertified::IsConnected( void )
{
	return m_fConn;
}

#endif	// __DPCERTIFIED_H__