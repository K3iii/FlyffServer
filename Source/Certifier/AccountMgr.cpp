// AccountMgr.cpp: implementation of the CAccountMgr class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccountMgr.h"
#include "..\Resource\lang.h"

////////////////////////////////////////////////////////////////

UINT HashKey( const char* key )
{
	UINT nHash = 0;
	while( *key )
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

////////////////////////////////////////////////////////////////

CAccountMgr::CAccountMgr()
{
}

CAccountMgr::~CAccountMgr()
{
	list< ACCOUNT_CACHE* >::iterator it;
	for( it = m_cache.begin(); it != m_cache.end(); ++it )
		SAFE_DELETE( *it );
}

////////////////////////////////////////////////////////////////

ACCOUNT_CHECK CAccountMgr::Check( DWORD dwIP )
{
	time_t	tmCur = time( NULL );


	list< ACCOUNT_CACHE* >::iterator it;
	for( it = m_cache.begin(); it != m_cache.end(); ++it )
	{
		ACCOUNT_CACHE* pInfo = *it;
		if( pInfo->m_dwIP == dwIP )
		{
			ACCOUNT_CHECK result = CHECK_OK;

			if (pInfo->m_nError >= 10)
			{
				long nSec = tmCur - pInfo->m_tmError;
				if( nSec <= (15 * 60) )
					result = CHECK_3TIMES_ERROR;
			}
			m_cache.erase( it );
			m_cache.push_front( pInfo );
			return result;
		}
	}



	ACCOUNT_CACHE* pInfo = NULL;
	if( m_cache.size() < 3 )
		pInfo = new ACCOUNT_CACHE;
	else
	{
		pInfo = m_cache.back();
		m_cache.pop_back();
	}

	pInfo->m_nError  = 0;
	pInfo->m_dwIP    = dwIP;
	pInfo->m_tmError = tmCur;

	m_cache.push_front( pInfo );

	return CHECK_OK;
}


void CAccountMgr::SetError( int nError )
{
	ACCOUNT_CACHE* pInfo = m_cache.front();		// MRU

	if( nError > 0 )
		++pInfo->m_nError;
	else
		pInfo->m_nError = 0;

	pInfo->m_tmError = time( NULL );
}
