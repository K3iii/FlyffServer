#ifndef __DPSOCK_H__
#define __DPSOCK_H__

#pragma once

#include "serversock.h"
#include "clientsock.h"
#include "serversocke.h"
#include "clientsocke.h"

template <class T>
class CDPSock
{
private:
	CSock*	m_pSock;
	u_short	m_uPort;
	char	m_lpAddr[32];
	u_long	m_uIoWorker;
	list<HANDLE>	m_listthread;
	CMclCritSec		m_locklistthread;
	HANDLE	m_hCompletionPort;
	WSAEVENT	m_hClose;
	HANDLE	m_hRecv;
	CSock*	m_pSockThreaded;
public:
	CBufferQueue<T>	m_lspRecvBuffer;
	BOOL	m_fServer;

//	DWORD	m_dwReadHeaderSize;
//	DWORD	m_dwDataSizeOffset;
private:
//	Operations
	BOOL	CreateIoWorker( u_long uIoWorker );
	BOOL	CloseIoWorker( void );
public:
//	Constructions
	CDPSock();
	virtual	~CDPSock();
//	Operations
	BOOL	Close( void );
	void	CloseConnection( DPID dpid );
	BOOL	Shutdown( DPID dpid );
	HANDLE	GetCompletionPort( void );
	HANDLE	GetCloseHandle( void );
	HANDLE	GetRecvHandle( void );
	CSock*	GetSockThreaded( void );
	int		GetIoWorkerCount( void );
	BOOL	CreateEventWorker( CSock* pSock );
	void	RemoveThread( HANDLE hThread );
	CClientSock<T>*		Get( SOCKET hSocket );

	BOOL	InitializeConnection( LPVOID lpConnection, DWORD dwFlags );

	BOOL	CreateServer( DWORD dwcrc );
	BOOL	JoinToServer( DWORD dwcrc, u_long uWaitingTime = 10000 );
	BOOL	CreateServerE( DWORD dwcrc );
	BOOL	JoinToServerE( DWORD dwcrc, u_long uWaitingTime = 10000 );

	BOOL	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );

	HRESULT	GetPlayerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	HRESULT	GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize );
	HRESULT	Receive( LPDPID lpdpidFrom, LPDPID lpdpidTo, DWORD dwFlags, T* &pBuffer );

	void	AddCreatePlayerOrGroupMsg( DPID dpid );
	void	AddDestroyPlayerOrGroupMsg( DPID dpid );

	static	UINT	_IoWorkerThread( LPVOID pParam )
		{
			CDPSock<T>* pDPSock	= (CDPSock<T>*)pParam;
			return pDPSock->IoWorkerThread();
		}
	static	UINT	_EventWorkerThread( LPVOID pParam )
		{
			CDPSock<T>* pDPSock	= (CDPSock<T>*)pParam;
			return pDPSock->EventWorkerThread();
		}
	int		IoWorkerThread( void );
	int		EventWorkerThread( void );
};

template <class T>
inline HANDLE CDPSock<T>::GetCompletionPort( void )
	{	return m_hCompletionPort;	}

template <class T>
inline HANDLE CDPSock<T>::GetCloseHandle( void )
	{	return m_hClose;	}

template <class T>
inline HANDLE CDPSock<T>::GetRecvHandle( void )
	{	return m_hRecv;	}

template <class T>
inline int	CDPSock<T>::GetIoWorkerCount( void )
	{	return (int)m_uIoWorker;	}

template <class T>
inline CSock* CDPSock<T>::GetSockThreaded( void )
	{	return m_pSockThreaded;	}

template <class T>
inline CClientSock<T>*	CDPSock<T>::Get( SOCKET hSocket )
	{	return ( m_pSock? (CClientSock<T>*)m_pSock->Get( hSocket ): NULL );	}

template <class T>
inline HRESULT	CDPSock<T>::GetHostAddr( LPVOID lpAddr, LPDWORD lpdwSize )
	{	return( m_pSock ? m_pSock->GetHostAddr( lpAddr, lpdwSize ):	DPERR_NOSESSIONS );	}

template <class T>
inline HRESULT CDPSock<T>::GetPlayerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize )
	{	return m_pSock->GetPeerAddr( dpid, lpAddr, lpdwSize );	}

template <class T>
CDPSock<T>::CDPSock()
	{
		m_fServer	= FALSE;
		m_pSock		= NULL;
		m_hCompletionPort	= (HANDLE)NULL;
		m_hRecv		= CreateEvent( NULL, FALSE, FALSE, NULL );
		ASSERT( m_hRecv );
		m_hClose	= WSACreateEvent();
		ASSERT( m_hClose );
		m_uIoWorker		= 0;
		memset( m_lpAddr, 0, sizeof(char)*32 );
		m_pSockThreaded		= NULL;
		//	m_dwReadHeaderSize	= 0;
		//	m_dwDataSizeOffset	= 0;
	}

template <class T>
CDPSock<T>::~CDPSock()
	{
		CLOSE_HANDLE( m_hRecv );
		CLOSE_WSAEVENT( m_hClose );
		Close();
	}

template <class T>
BOOL CDPSock<T>::Close( void )
	{
		CloseIoWorker();
		if( m_pSock )
		{
			m_pSock->Close();
			SAFE_DELETE( m_pSock );
		}
		m_lspRecvBuffer.Clear( TRUE);	// delete memory
		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::InitializeConnection( LPVOID lpConnection, DWORD dwFlags )
	{
		if( lpConnection )
			strcpy( m_lpAddr, (char*)lpConnection );
		m_uPort		= (u_short)dwFlags;
		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::CreateIoWorker( u_long uIoWorker )
	{
		HANDLE hThread;



		m_uIoWorker	= uIoWorker;
		if( ( m_hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 ) ) == (HANDLE)NULL )
			return FALSE;

		DWORD dwThreadId;
		for( DWORD i = 0; i < m_uIoWorker; i++ )
		{
			hThread		= chBEGINTHREADEX( NULL, 0, _IoWorkerThread, this, 0, &dwThreadId );
			ASSERT( hThread );

			SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );

			m_listthread.push_back( hThread );
		}
		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::CloseIoWorker( void )
	{
		u_long cbThread;
		LPHANDLE lphThread;

		m_locklistthread.Enter();
		cbThread	= m_listthread.size();
		if( cbThread > 0 )
		{
			lphThread	= new HANDLE[cbThread];
			for( DWORD i = 0; i < cbThread; i++ )
			{
				lphThread[i]	= m_listthread.front();
				m_listthread.pop_front();
				if( !chWindows9x() )
					PostQueuedCompletionStatus( m_hCompletionPort, CLOSEIOWORKERMSG, TRUE, NULL );
			}
			if( chWindows9x() )
				WSASetEvent( m_hClose );
			WaitForMultipleObjects( cbThread, lphThread, TRUE, INFINITE );
			for( DWORD i = 0; i < cbThread; i++ ) {
				CloseHandle( (HANDLE)lphThread[i] );
			}
			SAFE_DELETE_ARRAY( lphThread );
		}
		m_uIoWorker	= 0;
		CLOSE_HANDLE( m_hCompletionPort );
		m_locklistthread.Leave();
		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::CreateEventWorker( CSock* pSock )
	{
		HANDLE hThread;
		m_uIoWorker++;
		m_pSockThreaded	= pSock;
		AddCreatePlayerOrGroupMsg( (DPID)pSock->GetHandle() );
	//	hThread		= (HANDLE)_beginthread( EventWorkerThread, 0, this );
		DWORD dwThreadId;
		hThread		= chBEGINTHREADEX( NULL, 0, _EventWorkerThread, this, 0, &dwThreadId );
		ASSERT( hThread );
		SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
		( (CClientSockE<T>*)pSock )->m_hWorker	= hThread;
		m_locklistthread.Enter();
		m_listthread.push_back( hThread );
		m_locklistthread.Leave();
		return TRUE;
	}

template <class T>
void CDPSock<T>::RemoveThread( HANDLE hThread )
	{
		HANDLE h;
		m_locklistthread.Enter();
		if( m_listthread.size() > 0 )
		{
			for( list<HANDLE>::iterator i = m_listthread.begin(); i != m_listthread.end(); ++i )
			{
				h	= *i;
				if( h == hThread )
				{
					m_listthread.erase( i );
					m_uIoWorker--;
					break;
				}
			}
		}
		m_locklistthread.Leave();
	}

template <class T>
BOOL CDPSock<T>::CreateServer( DWORD dwcrc )
	{
		Close();

		CServerSock<T>* pSock	= new CServerSock<T>( dwcrc );

		if( !pSock->Create( m_uPort ) )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}
		CSystemInfo si;
//		CreateIoWorker( si.dwNumberOfProcessors*2 );
		CreateIoWorker( (u_long)0x01 );
		pSock->StartServer( m_hCompletionPort, m_uIoWorker );

		if( !pSock->Listen() )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}
		pSock->SetID( DPID_SERVERPLAYER );
		m_pSock		= pSock;
		m_fServer	= TRUE;

		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::CreateServerE( DWORD dwcrc )
	{
		Close();

		CServerSockE<T>* pSock		= new CServerSockE<T>( dwcrc );

		if( !pSock->Create( m_uPort ) )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}
		pSock->StartServer( this );
		if( !pSock->Listen() )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}
		pSock->SetID( DPID_SERVERPLAYER );
		m_pSock		= pSock;
		m_fServer	= TRUE;

		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::JoinToServer( DWORD dwcrc, u_long uWaitingTime )
	{
		Close();

		CClientSock<T>* pSock	= new CClientSock<T>( dwcrc );

		if( !pSock->Create() )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}
		CreateIoWorker( (u_long)0x01 );
		if( !CreateIoCompletionPort( (HANDLE)pSock->GetHandle(), m_hCompletionPort, (DWORD)( pSock->GetHandle() ),  0 ) )
		{
			TRACE( "Can't create completion port with error %d\n", WSAGetLastError() );
			SAFE_DELETE( pSock );
			return FALSE;
		}
		if( !pSock->Connect( m_lpAddr, m_uPort ) )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}

		pSock->SetID( pSock->GetHandle() );
		pSock->SetPeerID( DPID_SERVERPLAYER );
		m_pSock		= pSock;

		int zero	= 0;
		setsockopt( pSock->GetHandle(), SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );
		setsockopt( pSock->GetHandle(), SOL_SOCKET, SO_RCVBUF, (char *)&zero, sizeof(zero) );

		PostQueuedCompletionStatus( m_hCompletionPort, NEWSOCKETMSG, (DWORD)pSock->GetHandle(), NULL );
		m_fServer	= FALSE;
		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::JoinToServerE( DWORD dwcrc, u_long uWaitingTime )
	{
		Close();

		CClientSockE<T>* pSock	= new CClientSockE<T>( dwcrc );

		if( !pSock->Create() )
		{
			SAFE_DELETE( pSock );
			return FALSE;
		}

		CreateEventWorker( (CClientSockE<T>*)pSock );
		if( !pSock->Connect( m_lpAddr, m_uPort ) )
		{
			SAFE_DELETE( m_pSock );
			return FALSE;
		}

		pSock->SetID( pSock->GetHandle() );
		pSock->SetPeerID( DPID_SERVERPLAYER );
		m_pSock		= pSock;

		int zero	= 0;
		setsockopt( pSock->GetHandle(), SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero) );
		setsockopt( pSock->GetHandle(), SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero) );

		InterlockedIncrement( &pSock->m_l );
		if( pSock->Recv() != 0 )
		{
			TRACE( "I/0 error %d\n", WSAGetLastError() );
	//		if( pSock->m_l == 0 ) {
				RemoveThread( pSock->m_hWorker );
				CloseConnection( pSock->GetID() );
				return FALSE;
	//		}
		}
		m_fServer	= FALSE;

		return TRUE;
	}

template <class T>
BOOL CDPSock<T>::Send( char* lpData, DWORD dwDataSize, DPID dpidTo )
	{
		if( m_pSock )
		{
			m_pSock->Send( lpData, dwDataSize, dpidTo );
			return TRUE;
		}
		return FALSE;
	}

template <class T>
HRESULT CDPSock<T>::Receive( LPDPID lpdpidFrom, LPDPID lpdpidTo, DWORD dwFlags, T* & pBuffer )
	{
		CMclAutoLock	Lock( m_lspRecvBuffer.m_cs );

		if( !( pBuffer	= m_lspRecvBuffer.GetHead() ) )
			return DPERR_NOMESSAGES;

		*lpdpidFrom	= pBuffer->dpid;
		m_lspRecvBuffer.RemoveHead();
		return DP_OK;
	}

template <class T>
void CDPSock<T>::CloseConnection( DPID dpid )
	{
		if(  m_pSock->CloseConnection( dpid ) )
			AddDestroyPlayerOrGroupMsg( dpid );
		if( !m_fServer ) {
			SAFE_DELETE( m_pSock );
		}
	}

template <class T>
BOOL CDPSock<T>::Shutdown( DPID dpid )
	{	return m_pSock->Shutdown( dpid );	}

template <class T>
void CDPSock<T>::AddCreatePlayerOrGroupMsg( DPID dpid )
{
	DPMSG_CREATEPLAYERORGROUP createPlayer;
	T* pBuffer	= new T;
	pBuffer->crc( (CRC32*)m_pSock->IsCrcRead() );

	ZeroMemory( (void*)&createPlayer, sizeof(DPMSG_CREATEPLAYERORGROUP) );

	createPlayer.dwType	= DPSYS_CREATEPLAYERORGROUP;	// Is it a player or group
	createPlayer.dwPlayerType	= DPPLAYERTYPE_PLAYER;	// Is it a player or group
	createPlayer.dpId	= dpid;	// ID of the player or group
	createPlayer.dpIdParent	= DPID_SERVERPLAYER;
	pBuffer->AddSysMsg( (void*)&createPlayer, sizeof(DPMSG_CREATEPLAYERORGROUP) );

	m_lspRecvBuffer.AddTail( pBuffer );

	SetEvent( m_hRecv );
}

template <class T>
void CDPSock<T>::AddDestroyPlayerOrGroupMsg( DPID dpid )
{
	DPMSG_DESTROYPLAYERORGROUP destroyPlayer;
	T* pBuffer	= new T;
	pBuffer->crc( (CRC32*)m_pSock->IsCrcRead() );

	ZeroMemory( (void*)&destroyPlayer, sizeof(DPMSG_DESTROYPLAYERORGROUP) );
	destroyPlayer.dwType	= DPSYS_DESTROYPLAYERORGROUP;	// Message type
	destroyPlayer.dwPlayerType	= DPPLAYERTYPE_PLAYER;	// Is it a player or group
	destroyPlayer.dpId	= dpid;		// player ID being deleted
	destroyPlayer.dpIdParent	= DPID_SERVERPLAYER;	// id of parent group
	pBuffer->AddSysMsg( (void*)&destroyPlayer, sizeof(DPMSG_DESTROYPLAYERORGROUP) );

	m_lspRecvBuffer.AddTail( pBuffer );

	SetEvent( m_hRecv );
}

template <class T>
int CDPSock<T>::IoWorkerThread( void )
	{
		SOCKET hSocket;
		DWORD dwBytes;
		CClientSock<T>* pClientSock;
		LPOVERLAPPED lpov;
		BOOL fOk;
		while( 1 )
		{
			fOk		= GetQueuedCompletionStatus( GetCompletionPort(), &dwBytes, (LPDWORD)&hSocket, &lpov, INFINITE );

			if( dwBytes == CLOSEIOWORKERMSG )
				return( 0 );

			if( dwBytes == NEWSOCKETMSG )
			{
				AddCreatePlayerOrGroupMsg( hSocket );
				pClientSock		= Get( hSocket );
				InterlockedIncrement( &pClientSock->m_l );
				if( pClientSock->Recv() != 0 )
				{
					TRACE( "I/0 error %d\n", WSAGetLastError() );
	//				if( pClientSock->m_l == 0 )
						CloseConnection( hSocket );
				}
				continue;
			}

			if( !( pClientSock = Get( hSocket ) ) )
				continue;

			if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or becoz of error
			{
				int err		= WSAGetLastError();
				switch( err )
				{
					case ERROR_NETNAME_DELETED:		//64
					case ERROR_OPERATION_ABORTED:
					case 0:
					case WSAENOTSOCK:	//10038
					default:
						{
							if( FALSE == fOk && NULL == lpov )	{}
							TRACE( "Completion status failed with error %d\n", err );
							TRACE( "Close socket %d, %x //REF:%d\n", hSocket, pClientSock, pClientSock->m_l - 1 );
							InterlockedDecrement( &pClientSock->m_l );
	//						if( pClientSock->m_l == 0 )
								CloseConnection( hSocket );
							break;
						}
				}
				continue;
			}

			if( &pClientSock->m_ovRecv == lpov )	// receive i/o completed
			{
				T* pBuffer	= pClientSock->Fetch( dwBytes );
				if( pBuffer )
				{
					if( pBuffer->cb > 0 ) {
						pBuffer->dpid	= hSocket;
						m_lspRecvBuffer.AddTail( pBuffer );
						SetEvent( GetRecvHandle() );
					}
					else {
						SAFE_DELETE( pBuffer );
					}
				}
				else if( WSAGetLastError()
					==  ERROR_BAD_NET_NAME )
				{
					CloseConnection( hSocket );
					continue;
				}

				if( pClientSock->Recv() != 0 )	// i/o error
				{
	//				if( pClientSock->m_l == 0 )
						CloseConnection( hSocket );
				}
			}
			else if( &pClientSock->m_ovSend == lpov )	// send i/o completed
			{
				if( pClientSock->SendRemnant( dwBytes ) != 0 )
				{
					if( pClientSock->m_l == 0 )
						CloseConnection( hSocket );
				}
			}
	//		Sleep( 0 );
		}
		return( 0 );
	}

template <class T>
int CDPSock<T>::EventWorkerThread( void )
	{
		CClientSockE<T>* pClientSock	= (CClientSockE<T>*)GetSockThreaded();
		SOCKET hSocket	= pClientSock->GetHandle();

		WSAEVENT aEvent[4];
		aEvent[0]	= pClientSock->GetRecvEvent();
		aEvent[1]	= pClientSock->GetSendEvent();
		aEvent[2]	= GetCloseHandle();
		aEvent[3]	= NULL;

		DWORD dwBytes	= 0, dw;
		BOOL fOk;

		while( 1 )
		{
			dw	= WSAWaitForMultipleEvents( 3, aEvent, FALSE, INFINITE, FALSE );
			switch( dw )
			{
				case WSA_WAIT_EVENT_0+2:
					return 0;
				case WSA_WAIT_FAILED:
					TRACE( "WSA_WAIT_FAILED with error %d\n", WSAGetLastError() );
					continue;
					break;
				case WSA_WAIT_EVENT_0:
					{
						fOk		= GetOverlappedResult( (HANDLE)hSocket, &pClientSock->m_ovRecv, &dwBytes, TRUE );
						if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or because of error
						{
							int err		= WSAGetLastError();
							switch( err )
							{
								case ERROR_NETNAME_DELETED:
								case ERROR_OPERATION_ABORTED:
								case 0:
								case WSAENOTSOCK:
								default:
									{
										TRACE( "Close socket %d, %x //REF:%d\n", hSocket, pClientSock, pClientSock->m_l-1 );
										InterlockedDecrement( &pClientSock->m_l );
	//									if( pClientSock->m_l == 0 ) {
	//										RemoveThread( pClientSock->m_hWorker );
											CloseConnection( hSocket );
											return 0;
	//									}
										break;
									}
							}
						}

						T* pBuffer	= pClientSock->Fetch( dwBytes );
						if( pBuffer )
						{
							if( pBuffer->cb > 0 ) {
								pBuffer->dpid	= hSocket;
								m_lspRecvBuffer.AddTail( pBuffer );
								SetEvent( GetRecvHandle() );
							}
							else {
								SAFE_DELETE( pBuffer );
							}
						}
						else if( WSAGetLastError()
							==  ERROR_BAD_NET_NAME )
						{
							CloseConnection( hSocket );
							return 0;
						}

						if( pClientSock->Recv() != 0 )	// i/o error
						{
	//						if( pClientSock->m_l == 0 ) {
								RemoveThread( pClientSock->m_hWorker );
								CloseConnection( hSocket );
								return 0;
	//						}
						}
						break;
					}
				case WSA_WAIT_EVENT_0+1:
					{
						fOk		= GetOverlappedResult( (HANDLE)hSocket, &pClientSock->m_ovSend, &dwBytes, TRUE );
						WSAResetEvent( aEvent[1] );
						if( !fOk || dwBytes == 0 )	// When Connection Closed by peer gracefully or because of error
						{
							int err		= WSAGetLastError();
							switch( err )
							{
								case ERROR_NETNAME_DELETED:
								case ERROR_OPERATION_ABORTED:
								case 0:
								case WSAENOTSOCK:
								default:
									{
										TRACE( "Close socket %d, %x //REF:%d\n", hSocket, pClientSock, pClientSock->m_l-1 );
										InterlockedDecrement( &pClientSock->m_l );
	//									if( pClientSock->m_l == 0 ) {
											RemoveThread( pClientSock->m_hWorker );
											CloseConnection( hSocket );
											return 0;
	//									}
										break;
									}
							}
						}

						if( pClientSock->SendRemnant( dwBytes ) != 0 )
						{
	//						if( pClientSock->m_l == 0 ) {
								RemoveThread( pClientSock->m_hWorker );
								CloseConnection( hSocket );
								return 0;
	//						}
						}
						break;
					}
			}
		}
		return 0;
	}

#endif //__DPSOCK_H__