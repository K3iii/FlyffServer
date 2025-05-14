#ifndef __DBMANAGER_H__
#define	__DBMANAGER_H__

#include "query.h"
#include "ar.h"
#include "mempool.h"
#include <dplay.h>
#include <map>
#include <string>
#include <set>
using	namespace	std;

const DWORD			DEFAULT_DB_WORKER_THREAD_NUM = 8;

enum QUERYMODE
{
	NEW_ACCOUNT,
	CERTIFY,
	CLOSE_EXISTING_CONNECTION,
};

typedef	struct	tagACCOUNT_INFO
{
	char	szAccount[MAX_ACCOUNT];
	char	szPassword[MAX_PASSWORD];
	char	szBak[MAX_ACCOUNT];
}	ACCOUNT_INFO, *LPACCOUNT_INFO;

typedef	struct tagDB_OVERLAPPED_PLUS
{
	int				nQueryMode;
	ACCOUNT_INFO	AccountInfo;
	DWORD			dpId;
	DWORD			dwIP;		//connected ip
}	DB_OVERLAPPED_PLUS, *LPDB_OVERLAPPED_PLUS;

class CAccountMgr;

class CDbManager
{
protected:
	HANDLE	m_hDbWorkerThreadTerminate[DEFAULT_DB_WORKER_THREAD_NUM];
	HANDLE	m_hIOCP[DEFAULT_DB_WORKER_THREAD_NUM];

public:
	CMemPool<DB_OVERLAPPED_PLUS>*	m_pDbIOData;
	char							m_szLoginPWD[256];

public:
	CDbManager();
	~CDbManager();

	BOOL	CreateDbWorkers();
	void	GetStrTime( CTime *time, char *strbuf );
	void	Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, CAccountMgr& accountMgr );
	void	CloseExistingConnection( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus );

	void	DBQryAccount( char* qryAccount, LPDB_OVERLAPPED_PLUS pData );
	void	OnCertifyQueryOK( CQuery & qry, LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus, const char* szCheck = "" );
	HANDLE	GetIOCPHandle( int n );
	void	PostQ( LPDB_OVERLAPPED_PLUS pData );
};

u_int	__stdcall	DbWorkerThread( LPVOID lpDbManager );	// DbWorkerThread

#endif	// __DBMANAGER_H__