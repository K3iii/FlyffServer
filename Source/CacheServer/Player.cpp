#include "StdAfx.h"
#include "Player.h"
#include "dpcachesrvr.h"
#include "dpcoreclient.h"

extern	CDPCoreClient	g_DPCoreClient;
extern	CDPCacheSrvr	g_DPCacheSrvr;

CPlayer::CPlayer(  DPID dpidUser, DWORD dwSerial )
:
m_dwAuthKey( 0 ),
m_bAlive( TRUE ),
m_dpid( dpidUser ),
m_idPlayer( 0 ),
m_pClient( NULL ),
m_dwSerial( dwSerial ),
m_dwWorldId( 0 ),
m_uChannel( 0 ),
m_idParty( 0 ),
m_idGuild( 0 ),
m_nSlot( 0 ),
m_bConnected(false),
m_dwCreation( GetTickCount() )
{
	*m_szPlayer	= '\0';
	*m_szAccount	= '\0';
	*m_szPass	= '\0';
	*m_lpAddr	= '\0';
}

CPlayer::~CPlayer()
{
}

void CPlayer::Join( CAr & ar )
{
	ar >> m_dwWorldId >> m_idPlayer >> m_dwAuthKey;
	ar >> m_idParty >> m_idGuild >> m_uChannel >> m_nSlot;
	ar.ReadString( m_szPlayer, MAX_PLAYER );
	ar.ReadString( m_szAccount, MAX_ACCOUNT );
	ar.ReadString( m_szPass, MAX_PASSWORD );
}

CPlayerMng::CPlayerMng()
:
m_lSerial( 0 )
{
}

CPlayerMng::~CPlayerMng()
{
	Clear();
}

void CPlayerMng::Clear( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	for( MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapPlayers.clear();
}

BOOL CPlayerMng::AddPlayer( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	CPlayer* pPlayer	= new CPlayer( dpid, InterlockedIncrement( &m_lSerial ) );
	bool bResult	= m_mapPlayers.insert( MPP::value_type( dpid, pPlayer ) ).second;
	if( !bResult )
		SAFE_DELETE( pPlayer );
	return bResult;
}

CPlayer* CPlayerMng::GetPlayer( DPID dpid )
{
	MPP::iterator i	= m_mapPlayers.find( dpid );
	if( i != m_mapPlayers.end() )
		return i->second;
	return NULL;
}

#ifdef __V21_CLOSE_EXISTING_CONNECTION
CPlayer* CPlayerMng::GetPlayer(const char* szAccount)
{
	for (MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i)
	{
		CPlayer* pPlayer = i->second;
		if (pPlayer != NULL)
		{
			if (_stricmp(pPlayer->GetAccount(), szAccount) == 0)
			{
				return pPlayer;
			}
		}
	}

	return NULL;
}
#endif

BOOL CPlayerMng::RemovePlayer( DPID dpid )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	MPP::iterator i	= m_mapPlayers.find( dpid );
	if( i != m_mapPlayers.end() )
	{
		SAFE_DELETE( i->second );
		m_mapPlayers.erase( i );
#ifdef __VALID_CONNECTION_CONFORMATION
		g_DPCacheSrvr.m_socketTimer.erase(dpid);
#endif
		return TRUE;
	}
	return FALSE;
}

void CPlayerMng::DestroyPlayer( CDPClient* pClient )
{
	CMclAutoLock Lock( m_AddRemoveLock );
	if( pClient )
		DestroyPlayersOnChannel( pClient );
	else
		DestroyAllPlayers();
}

void CPlayerMng::DestroyPlayersOnChannel( CDPClient* pClient )
{
	for( MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
	{
		CPlayer* pPlayer	= i->second;
		if( pPlayer->GetClient() == pClient )
		{
			g_DPCoreClient.SendDestroyPlayer( pPlayer );
			g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );
		}
	}
}

void CPlayerMng::DestroyAllPlayers( void )
{
	for( MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
		g_DPCacheSrvr.DestroyPlayer( i->second->GetNetworkId() );
}

CPlayer* CPlayerMng::GetPlayerBySerial( DWORD dwSerial )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i )
	{
		CPlayer* pPlayer	= i->second;
		if( pPlayer->GetSerial() == dwSerial )
			return pPlayer;
	}
	return NULL;
}

void CPlayerMng::DestroyGarbage(void)
{
	DWORD dwTick = GetTickCount() - SEC(60);
	CMclAutoLock	Lock(m_AddRemoveLock);

#ifdef __VALID_CONNECTION_CONFORMATION
	std::vector<DPID> invalid_sockets;

	for (auto & it : g_DPCacheSrvr.m_socketTimer)
	{
		if (it.second + 120 < time(nullptr))
		{
			auto player = GetPlayer(it.first);
			if (player)
			{
				g_DPCoreClient.SendConfirmPlayer(player);
			}
			else
			{
				g_DPCacheSrvr.DestroyPlayer(it.first);
				invalid_sockets.push_back(it.first);
			}
		}
	}

	for (auto& id : invalid_sockets)
		g_DPCacheSrvr.m_socketTimer.erase(id);
#endif

	for (MPP::iterator i = m_mapPlayers.begin(); i != m_mapPlayers.end(); ++i)
	{
		CPlayer* pPlayer = i->second;
		if (pPlayer->IsTimeover(dwTick))
		{
			//Error("CPlayerMng::DestroyGarbage (timeover): Account %s Player %s", pPlayer->GetAccount(), pPlayer->GetPlayer());
			g_DPCacheSrvr.DestroyPlayer(pPlayer->GetNetworkId());
		}
		else if (!pPlayer->IsAlive())
		{
			//Error("CPlayerMng::DestroyGarbage (!alive): Account %s Player %s", pPlayer->GetAccount(), pPlayer->GetPlayer());
			DestroyGarbagePlayer(pPlayer);
		}
		else
			SendKeepAlive(pPlayer);
	}
}

void CPlayerMng::DestroyGarbagePlayer( CPlayer* pPlayer )
{
	pPlayer->SetAlive( TRUE );
	g_DPCacheSrvr.DestroyPlayer( pPlayer->GetNetworkId() );
}

void CPlayerMng::SendKeepAlive( CPlayer* pPlayer )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );
	pPlayer->SetAlive( FALSE );
	SEND( ar, &g_DPCacheSrvr, pPlayer->GetNetworkId() );
}

CPlayerMng*	CPlayerMng::Instance( void )
{
	static	CPlayerMng	sPlayerMng;
	return &sPlayerMng;
}
