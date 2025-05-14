#include "StdAfx.h"
#include "dploginsrvr.h"
extern	CDPLoginSrvr	g_dpLoginSrvr;

#include "dpcoresrvr.h"
extern	CDPCoreSrvr		g_dpCoreSrvr;

extern	APP_INFO	g_appInfo;

#include "dpaccountclient.h"
#include "serialnumber.h"

CDPAccountClient::CDPAccountClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_GETPLAYERLIST, &CDPAccountClient::OnGetPlayerList );
	ON_MSG(PACKETTYPE_CREATE_PLAYER, &CDPAccountClient::OnCreatePlayer);
#ifdef __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_DEL_PLAYER, &CDPAccountClient::OnRemovePlayer );
#endif	// __REMOVE_PLAYER_0221
	ON_MSG( PACKETTYPE_JOIN, &CDPAccountClient::OnJoin );
	ON_MSG( PACKETTYPE_PLAYER_COUNT, &CDPAccountClient::OnPlayerCount );
	ON_MSG( PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPAccountClient::OnCloseExistingConnection );
	ON_MSG( PACKETTYPE_ONE_HOUR_NOTIFY, &CDPAccountClient::OnOneHourNotify )
	ON_MSG( PACKETTYPE_FAIL, &CDPAccountClient::OnFail );
}

CDPAccountClient::~CDPAccountClient()
{
}

void CDPAccountClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
				break;
	}
}

void CDPAccountClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	CAr ar( (LPBYTE)lpMsg, dwMsgSize );
	GETTYPE( ar );
	void ( theClass::*pfn )( theParameters )
		=	GetHandler( dw );
	ASSERT( pfn );
	( this->*( pfn ) )( ar, (LPBYTE)lpMsg + sizeof(DWORD), dwMsgSize - sizeof(DWORD) );
}

void CDPAccountClient::SendAddIdofServer( DWORD dwIdofServer )
{
	BEFORESEND( ar, PACKETTYPE_MYREG );
	ar << dwIdofServer;
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAccount( char* lpszAccount )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ACCOUNT );
	ar.WriteString( lpszAccount );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::SendRemoveAllAccounts( void )
{
	BEFORESEND( ar, PACKETTYPE_REMOVE_ALLACCOUNTS );
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPAccountClient::OnCreatePlayer(CAr& ar, LPBYTE lpBuf, u_long uBufSize)
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest(lpDbOverlappedPlus, lpBuf, uBufSize);
	lpDbOverlappedPlus->nQueryMode = CREATEPLAYER;
	PostQueuedCompletionStatus(g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped);
}

#ifdef __REMOVE_PLAYER_0221
void CDPAccountClient::OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode = REMOVEPLAYER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}
#endif	// __REMOVE_PLAYER_0221

void CDPAccountClient::OnGetPlayerList( CAr & ar, LPBYTE, u_long )
{
	DWORD dwAuthKey;
	ar >> dwAuthKey;
	u_long uIdofMulti;
	ar >> uIdofMulti;
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szAccount, MAX_ACCOUNT );
	ar.ReadString( lpDbOverlappedPlus->AccountInfo.szPassword, MAX_PASSWORD );
	ar >> lpDbOverlappedPlus->dpid;
	lpDbOverlappedPlus->nQueryMode	= SENDPLAYERLIST;
	lpDbOverlappedPlus->dwAuthKey	= dwAuthKey;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= JOIN;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPGet, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus		= g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, lpBuf, uBufSize );
	lpDbOverlappedPlus->nQueryMode	= SAVECONCURRENTUSERNUMBER;
	PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );
}

void CDPAccountClient::OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	LONG lError = 0;

	ar.ReadString( lpszAccount, MAX_ACCOUNT );

	g_dpLoginSrvr.SendCloseExistingConnection( lpszAccount, lError );
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	g_dpCoreSrvr.SendCloseExistingConnection(lpszAccount, lError);
#endif
}


void CDPAccountClient::OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize )
{
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	g_dpLoginSrvr.SendOneHourNotify( lpszAccount );
}

void CDPAccountClient::OnFail( CAr & ar, LPBYTE, u_long )
{
	long lError;
	char lpszAccount[MAX_ACCOUNT]	= { 0, };
	ar >> lError;
	ar.ReadString( lpszAccount, MAX_ACCOUNT );
	switch( lError )
	{
		case ERROR_FLYFF_ACCOUNT:
			{
				g_dpLoginSrvr.SendFail( lError, lpszAccount );
				break;
			}
	}
}


CDPAccountClient	g_dpAccountClient;