#include "stdafx.h"
#include "account.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"
#include "MsgHdr.h"
#include "dbmanager.h"

extern	CDbManager	g_DbManager;
extern	CDPSrvr		g_dpSrvr;
extern	CDPDBSrvr	g_dpDbSrvr;

CAccountMng			g_AccountMng;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __VM_0820
MemPooler<CAccount>*	CAccount::m_pPool	= new MemPooler<CAccount>( 1024 );
#endif	// __VM_0820

CAccount::CAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, BYTE cbAccountFlag, int fCheck )
{
	_tcscpy( m_lpszAccount, lpszAccount );
	m_dpid1				= dpid1;
	m_dpid2				= dpid2;
	m_cbAccountFlag		= cbAccountFlag;
	m_fRoute			= FALSE;
	m_dwIdofServer		= 0;
	m_dwPing			= timeGetTime();
	m_uIdofMulti		= NULL_ID;
	m_dwAuthKey			= 0;
	m_cbRef				= 0;

	m_TimeOverDays		= 0;
	m_nStatus			= ACCOUNT_STATUS_INITIAL;
	m_fCheck			= fCheck;

	m_bCertify = FALSE;
}

CAccount::~CAccount()
{
}

CAccountMng::CAccountMng()
{
	m_nSizeof	= 0;
	m_nCount	= 0;
	m_nOldHour  = 0;

	CTime tTime = CTime::GetCurrentTime();
	m_nYear = tTime.GetYear() - 1;
	m_nMonth = tTime.GetMonth();
	m_nDay = tTime.GetDay();
	m_nHour = tTime.GetHour();
	m_nMinute = tTime.GetMinute();
}

CAccountMng::~CAccountMng()
{
	Clear();
}

void CAccountMng::Clear( void )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( map<string, CAccount*>::iterator i	= m_stringToPAccount.begin(); i != m_stringToPAccount.end(); ++i )
		SAFE_DELETE( i->second );
	m_stringToPAccount.clear();

	for( int j = 0; j < m_nSizeof; j++ )
		m_adpidToPAccount[j].clear();
	m_dpidToIndex.clear();
}

int CAccountMng::GetIndex( DWORD dpid1 )
{
// locked
	map<DWORD, int>::iterator i		= m_dpidToIndex.find( dpid1 );
	if( i  != m_dpidToIndex.end() )
		return i->second;
	return( -1 );
}

int CAccountMng::GetIdofServer( DWORD dpid )
{
// locked
	map<DWORD, DWORD>::iterator i	= m_2IdofServer.find( dpid );
	if( i != m_2IdofServer.end() )
		return i->second;
	return( -1 );
}

BYTE CAccountMng::AddAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, DWORD* pdwAuthKey, BYTE cbAccountFlag, int fCheck )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	map<string, CAccount*>::iterator i1	= m_stringToPAccount.find( lpszAccount );
	if( i1 == m_stringToPAccount.end() )
	{
		int nIndex	= GetIndex( dpid1 );
		if( nIndex >= 0 && nIndex < MAX_CERTIFIER )
		{
			map<DWORD, CAccount*>::iterator i2	= m_adpidToPAccount[nIndex].find( dpid2 );
			if( i2 == m_adpidToPAccount[nIndex].end() )
			{
				CAccount* pAccount	= new CAccount( lpszAccount, dpid1, dpid2, cbAccountFlag, fCheck );
				m_stringToPAccount.insert( map<string, CAccount*>::value_type( lpszAccount, pAccount ) );
				m_adpidToPAccount[nIndex].insert( map<DWORD, CAccount*>::value_type( dpid2, pAccount ) );
				m_nCount++;
				*pdwAuthKey = pAccount->m_dwAuthKey = xRandom(0x00000001UL, ULONG_MAX);
				pAccount->m_bCertify = TRUE;
				return ACCOUNT_CHECK_OK;
			}
			return ACCOUNT_DUPLIACTE;
		}
		return ACCOUNT_DUPLIACTE;
	}
	else
	{
		CAccount* pAccount	= GetAccount( lpszAccount );
		if( pAccount && FALSE == pAccount->m_fRoute )
		{
			if( timeGetTime() - pAccount->m_dwPing > MIN( 2 ) )
			{
				g_dpSrvr.DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
				RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
				return AddAccount( lpszAccount, dpid1, dpid2, pdwAuthKey, cbAccountFlag, fCheck );
			}
		}
		return ACCOUNT_DUPLIACTE;
	}
}

void CAccountMng::RemoveAccount( LPCTSTR lpszAccount )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

#ifdef __INTERNALSERVER
	time_t cur = time(NULL);
	WriteLog( "CAccountMng::RemoveAccount(%s) - %s\n", lpszAccount, ctime( &cur ));
#endif

	map<string, CAccount*>::iterator i	= m_stringToPAccount.find( lpszAccount );
	if( i != m_stringToPAccount.end() )
	{
		CAccount* pAccount = i->second;

		if( pAccount->m_fRoute && pAccount->m_cbRef == 2 )
		{
			u_long uId	= pAccount->m_dwIdofServer * 100 + pAccount->m_uIdofMulti;
			map<u_long, LPSERVER_DESC>::iterator i2	= g_dpSrvr.m_2ServersetPtr.find( uId );

			if( i2 != g_dpSrvr.m_2ServersetPtr.end() )
			{
				long lCount = InterlockedDecrement( &i2->second->lCount );
				g_dpSrvr.SendPlayerCount( uId, lCount );
			}
		}

		g_DbManager.UpdateTracking( FALSE, lpszAccount );

		SAFE_DELETE( pAccount );
		m_stringToPAccount.erase( i );
		m_nCount--;
	}
}

void CAccountMng::RemoveAccount( DWORD dpid1, DWORD dpid2 )
{
	CAccount* pAccount;
	int nIndex;
	CMclAutoLock	Lock( m_AddRemoveLock );

	nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		map<DWORD, CAccount*>::iterator i	= m_adpidToPAccount[nIndex].find( dpid2 );
		if( i != m_adpidToPAccount[nIndex].end() )
		{
			pAccount	= i->second;
			m_adpidToPAccount[nIndex].erase( i );

			if( FALSE == pAccount->m_fRoute )
			{
				RemoveAccount( pAccount->m_lpszAccount );	// erase & delete
			}
		}
	}
}

CAccount* CAccountMng::GetAccount( LPCTSTR lpszAccount )
{
// locked
	map<string, CAccount*>::iterator i	= m_stringToPAccount.find( lpszAccount );
	if( i != m_stringToPAccount.end() )
		return i->second;
	return NULL;
}

CAccount* CAccountMng::GetAccount( DWORD dpid1, DWORD dpid2 )
{
// locked
	int nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		map<DWORD, CAccount*>::iterator i	= m_adpidToPAccount[nIndex].find( dpid2 );
		if( i != m_adpidToPAccount[nIndex].end() )
		{
			return i->second;
		}
	}
	return NULL;
}

void CAccountMng::AddConnection( DWORD dpid1 )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	map<DWORD, int>::iterator i	= m_dpidToIndex.find( dpid1 );
	if( i == m_dpidToIndex.end() )
	{
		m_dpidToIndex.insert( map<DWORD, int>::value_type( dpid1, m_nSizeof ) );
		m_nSizeof++;
	}
//	else
//	{
//	}
}

void CAccountMng::RemoveConnection( DWORD dpid1 )
{
	CAccount* pAccount;
	int nIndex;

	CMclAutoLock	Lock( m_AddRemoveLock );
	nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		map<DWORD, CAccount*>::iterator	i1	= m_adpidToPAccount[nIndex].begin();
		while( i1 != m_adpidToPAccount[nIndex].end() )
		{
			pAccount	= i1->second;
			map<DWORD, CAccount*>::iterator	i2	= i1;
			++i1;
			m_adpidToPAccount[nIndex].erase( i2 );
			RemoveAccount( pAccount->m_lpszAccount );	// erase & delete
		}
	}
}

void CAccountMng::AddIdofServer( DPID dpid, DWORD dwIdofServer )
{
	int nIdofServer;
	CMclAutoLock	Lock( m_AddRemoveLock );

	nIdofServer		= GetIdofServer( dpid );
	if( nIdofServer < 0 )
		m_2IdofServer.insert( map<DPID, DWORD>::value_type( dpid, dwIdofServer ) );
}

#ifdef __SERVERLIST0911
DWORD		CAccountMng::RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection )
#else	// __SERVERLIST0911
void CAccountMng::RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection )
#endif	// __SERVERLIST0911
{
	CAccount* pAccount;
	CMclAutoLock	Lock( m_AddRemoveLock );
	DWORD dwIdofServer	= GetIdofServer( dpid );

	map<string, CAccount*>::iterator i	= m_stringToPAccount.begin();
	while( i != m_stringToPAccount.end() )
	{
		pAccount	= i->second;
		++i;
		if( pAccount->m_dwIdofServer == dwIdofServer )
		{
			if( pAccount->m_fRoute )
				RemoveAccount( pAccount->m_lpszAccount );
			else
				RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
	if( bRemoveConnection )
		m_2IdofServer.erase( dpid );

#ifdef __SERVERLIST0911
	return dwIdofServer;
#endif	// __SERVERLIST0911
}

BOOL CAccountMng::IsTimeCheckAddr()
{
	CTime tTime = CTime::GetCurrentTime();
	if( m_nYear == tTime.GetYear() && m_nMonth == tTime.GetMonth() && m_nDay == tTime.GetDay() &&
		m_nHour == tTime.GetHour() && m_nMinute == tTime.GetMinute() )
	{
		return TRUE;
	}
	return FALSE;
}

#ifdef __LOG_PLAYERCOUNT_CHANNEL
map<string, CAccount*> CAccountMng::GetMapAccount()
{
	return m_stringToPAccount;
}
#endif // __LOG_PLAYERCOUNT_CHANNEL
