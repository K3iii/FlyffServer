#include "StdAfx.h"
#include "DPCacheSrvr.h"
#include "DPClient.h"
#include "DPCoreClient.h"
#include "ServerDesc.h"
#include "MyTrace.h"
#include "Player.h"

#ifdef __TRACE1027

#endif	// __TRACE1027

#ifdef __CRASH_0404
#include "crashstatus.h"
#endif	// __CRASH_0404

extern CDPCacheSrvr		g_DPCacheSrvr;
extern	CDPClientArray	g_DPClientArray;
extern	CMyTrace	g_MyTrace;

CDPCoreClient::CDPCoreClient()
{
	BEGIN_MSG;
	ON_MSG( PACKETTYPE_PROCSERVER_LIST, &CDPCoreClient::OnProcServerList );
	ON_MSG( PACKETTYPE_LOAD_WORLD, &CDPCoreClient::OnProcServer );
	ON_MSG( PACKETTYPE_JOIN, &CDPCoreClient::OnJoin );
	ON_MSG( PACKETTYPE_LEAVE, &CDPCoreClient::OnLeave );
	ON_MSG( PACKETTYPE_DESTROY_ALLPLAYERS, &CDPCoreClient::OnDestroyAllPlayers );
	ON_MSG( PACKETTYPE_KILLPLAYER, &CDPCoreClient::OnKillPlayer );
#ifdef __VALID_CONNECTION_CONFORMATION
	ON_MSG(PACKETTYPE_CONNECTION_CONFORMATION, &CDPCoreClient::OnConfirmPlayer);
#endif
#ifdef __V21_CLOSE_EXISTING_CONNECTION
	ON_MSG(PACKETTYPE_CLOSE_EXISTING_CONNECTION, &CDPCoreClient::OnCloseExistingConnection);
#endif
}

CDPCoreClient::~CDPCoreClient()
{

}

void CDPCoreClient::SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	switch( lpMsg->dwType )
	{
		case DPSYS_DESTROYPLAYERORGROUP:
			{
				g_MyTrace.Add( CMyTrace::Key( "main" ), TRUE, "destroy player or group." );
				break;
			}
	}
}

void CDPCoreClient::UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )
{
	static size_t	nSize	= sizeof(DPID);

	LPBYTE lpBuf	= (LPBYTE)lpMsg + nSize;
	ULONG	uBufSize	= dwMsgSize - nSize;
	DPID dpidUser	= *(UNALIGNED LPDPID)lpMsg;

	CAr ar( lpBuf, uBufSize );
	GETTYPE( ar );

	void ( theClass::*pfn )( theParameters )	=	GetHandler( dw );

	if( pfn )
		( this->*( pfn ) )( ar, dpidUser );
	else
	{
		switch( dw )
		{
			case PACKETTYPE_MODIFYMODE:
				{
					CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );
					g_DPClientArray.SendToServer( dpidUser, lpMsg, dwMsgSize );
					break;
				}
			default:
				g_DPCacheSrvr.Send( lpBuf, uBufSize, dpidUser );
				break;
		}
	}
}

void CDPCoreClient::SendDestroyPlayer( CPlayer* pPlayer )
{
	BEFORESENDSOLE( ar, PACKETTYPE_DESTROY_PLAYER, pPlayer->GetSerial() );
	SEND( ar, this, DPID_SERVERPLAYER );
}

#ifdef __VALID_CONNECTION_CONFORMATION
void CDPCoreClient::SendConfirmPlayer(CPlayer* pPlayer)
{
	BEFORESENDSOLE(ar, PACKETTYPE_CONNECTION_CONFORMATION, pPlayer->GetSerial());
	ar << pPlayer->GetNetworkId();
	SEND(ar, this, DPID_SERVERPLAYER);
}
void CDPCoreClient::OnConfirmPlayer(CAr& ar, DPID)
{
	DPID dpidUser;
	DWORD serial;
	bool state;
	ar >> dpidUser >> serial >> state;

	CMclAutoLock Lock(CPlayerMng::Instance()->m_AddRemoveLock);

	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayerBySerial(serial);
	if (pPlayer)
	{
		if (state)
			g_DPCacheSrvr.m_socketTimer[pPlayer->GetNetworkId()] = time(nullptr);
		else
		{
			g_DPCacheSrvr.DestroyPlayer(dpidUser);
			g_DPCacheSrvr.m_socketTimer.erase(dpidUser);
			Error("Destroyed Player DPID %d - IP %s", dpidUser, pPlayer->GetAddr());
		}
	}
	else
	{
		g_DPCacheSrvr.DestroyPlayer(dpidUser);
		g_DPCacheSrvr.m_socketTimer.erase(dpidUser);
		Error("Destroyed DPID %d", dpidUser);
	}
}

#endif

void CDPCoreClient::SendAddPlayer( CPlayer* pPlayer, CRTMessenger & rtmessenger )
{
	BEFORESENDSOLE( ar, PACKETTYPE_JOIN, pPlayer->GetSerial() );
	ar << pPlayer->GetAuthKey();
	ar.WriteString(pPlayer->GetAccount());
	ar << pPlayer->GetPlayerId();
	ar << pPlayer->GetNetworkId();
	ar << pPlayer->GetParty();
	ar << pPlayer->GetGuild();
	ar << pPlayer->GetChannel();
	rtmessenger.Serialize(ar);
	ar.WriteString(pPlayer->GetPlayer());
	ar.WriteString(pPlayer->GetAddr());
	SEND( ar, this, DPID_SERVERPLAYER );
}

void CDPCoreClient::OnProcServerList( CAr & ar, DPID )
{
	CServerDesc *pServer;
	short nSize;

	ar >> nSize;
	for( int i = 0; i < nSize; i++ )
	{
		pServer		= new CServerDesc;
		pServer->Serialize( ar );

		if( !g_DPClientArray.Connect( pServer ) ) {
			SAFE_DELETE( pServer );
		}
	}
	g_MyTrace.AddLine( '-' );
}

void CDPCoreClient::OnProcServer( CAr & ar, DPID )
{
	CServerDesc* pServer	= new CServerDesc;
	pServer->Serialize( ar );
	if( !g_DPClientArray.Connect( pServer ) ) {
		SAFE_DELETE( pServer );
	}
}

void CDPCoreClient::OnJoin( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	BYTE byData;
	ar >> dwSerial;
	ar >> byData;

	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );
	if( pPlayer == NULL )
	{
		Error( "CDPCoreClient::OnJoin - player not found" );
		return;
	}

	CDPClient* pClient = g_DPClientArray.GetClient( pPlayer->GetChannel(), pPlayer->GetWorld(), D3DXVECTOR3(0.0f, 0.0f, 0.0f) );
	if( pClient )
	{
		pPlayer->SetClient( pClient );
		pClient->SendJoin( pPlayer );
	}
	else
	{
		Error( "CDPCoreClient.OnJoin: server not found id: %d account: %s player: %s world: %d",
			      pPlayer->GetPlayerId(), pPlayer->GetAccount(), pPlayer->GetPlayer(), pPlayer->GetWorld() );
		g_DPCacheSrvr.DestroyPlayer( dpid );
	}
}

void CDPCoreClient::OnLeave( CAr & ar, DPID dpid )
{
	CMclAutoLock	Lock( CPlayerMng::Instance()->m_AddRemoveLock );

	DWORD dwSerial;
	ar >> dwSerial;
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayerBySerial( dwSerial );
	if (pPlayer)
	{
		g_DPCacheSrvr.DestroyPlayer(dpid);
#ifdef __VALID_CONNECTION_CONFORMATION
		g_DPCacheSrvr.m_socketTimer.erase(dpid);
#endif
	}
}

void CDPCoreClient::OnDestroyAllPlayers( CAr & ar, DPID dpid )
{
	CPlayerMng::Instance()->DestroyPlayer( NULL );
}

void CDPCoreClient::OnKillPlayer( CAr & ar, DPID dpid )
{
#ifdef __VALID_CONNECTION_CONFORMATION
	g_DPCacheSrvr.m_socketTimer.erase(dpid);
#endif
	g_DPCacheSrvr.DestroyPlayer( dpid );
}

#ifdef __V21_CLOSE_EXISTING_CONNECTION
void	CDPCoreClient::OnCloseExistingConnection(CAr& ar, DPID /*dpid*/)
{
	char lpszAccount[MAX_ACCOUNT] = { 0, };
	LONG lError = 0;

	ar.ReadString(lpszAccount, _countof(lpszAccount));
	ar >> lError;

	CMclAutoLock	Lock(CPlayerMng::Instance()->m_AddRemoveLock);

	DWORD dwSerial;
	ar >> dwSerial;
	CPlayer* pPlayer = CPlayerMng::Instance()->GetPlayer(lpszAccount);
	if (pPlayer)
		g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
}
#endif

void CDPCoreClient::SendToServer( DPID dpidUser, LPVOID lpMsg, DWORD dwMsgSize )
{
	CPlayer* pPlayer	= CPlayerMng::Instance()->GetPlayer( dpidUser );
	if( !pPlayer )
		return;
	*reinterpret_cast<UNALIGNED DPID*>( lpMsg )		= pPlayer->GetSerial();
	Send( lpMsg, dwMsgSize, DPID_SERVERPLAYER );
}

CDPCoreClient	g_DPCoreClient;