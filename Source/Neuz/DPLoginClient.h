#ifndef __DPLOGINCLIENT_H__
#define __DPLOGINCLIENT_H__

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define	theClass	CDPLoginClient
#undef theParameters
#define theParameters	CAr & ar

class CDPLoginClient : public CDPMng
{
private:
	BOOL	m_fDestroy;
	BOOL	m_fConn;
	CTimer	m_timer;
	BOOL	m_bShowDisconnectMsg;
	LONG	m_lError;
public:
	int		m_nSlot;

//	Constructions
	CDPLoginClient();
	virtual	~CDPLoginClient();
//	Override
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
#ifdef __TEST_PACKET_CRYPT
	virtual BOOL Send(LPVOID pData, DWORD dwDataSize, DPID dpidTo);
#endif

//	Operations
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();
	void	Destroy( BOOL fDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	void	QueryTickCount( void );
	void	SendGetPlayerList( DWORD dwID, LPCSTR lpszAccount, LPCSTR lpszpw );
	void	SendCreatePlayer( BYTE nSlot, LPCSTR lpszPlayer/*, LPDWORD adwEquipment*/, BYTE nFace, BYTE nCostume, BYTE nSkinSet, BYTE nHairMesh, DWORD dwHairColor, BYTE nSex, BYTE nJob, BYTE nHeadMesh, int nBankPW = 0 );
	void	SendDeletePlayer( BYTE nSlot, LPCTSTR szNo );
	void	SendPreJoin(const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer, int nSlot);
	void	OnPreJoin( CAr & ar );

//	Handlers
	USES_PFNENTRIES;
	void	OnPlayerList( CAr & ar );
	void	OnError( CAr & ar );
	void	OnCacheAddr( CAr & ar );
	void	OnQueryTickCount( CAr & ar );
	void	OnOneHourNotify( CAr & ar );

	void	OnPing( CAr & ar );
};

inline void CDPLoginClient::Destroy( BOOL fDestroy )
	{	m_fDestroy	= fDestroy;	}
inline BOOL CDPLoginClient::IsDestroyed( void )
	{	return( m_fDestroy );	}
inline BOOL CDPLoginClient::IsConnected( void )
	{	return( m_fConn );	}

inline void CDPLoginClient::Ping( void )
{
	if( m_timer.IsTimeOut() ) {
		m_timer.Reset();
		BEFORESENDSOLE( ar, PACKETTYPE_PING, DPID_UNKNOWN );
		DWORD dwPingTime = GetCurrentTime();
		ar << dwPingTime;
		++g_Neuz.m_nPingCount;
		if( g_Neuz.m_nPingCount > 1)
		{
			g_Neuz.m_dwPingTime = 0;
		}
		SEND( ar, this, DPID_SERVERPLAYER );
	}

	if( m_fDestroy ) {
		DeleteDPObject();
		m_fDestroy	= FALSE;
	}
}
#endif	// __DPLOGINCLIENT_H__