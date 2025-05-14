#ifndef __ACCOUNT_H__
#define	__ACCOUNT_H__

#include "mempooler.h"
#include <map>
#include <string>
using	namespace	std;

#define	MAX_CERTIFIER			32
const int LEFTIME_NOTIFIED		= 1;


enum ACCOUNT_STATUS
{
	ACCOUNT_STATUS_INITIAL,
	ACCOUNT_STATUS_NOTIFIED,
	ACCOUNT_STATUS_SECONDQUERY,
};

class CAccount
{
public:
//	Constructions
	CAccount();
	CAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, BYTE b18, int fCheck );
	virtual	~CAccount();
	void	SetTimeOverDays(  const CTime* pOverDays );

//	Attributes
public:
	DWORD			m_dpid1;
	DWORD			m_dpid2;
	DWORD			m_dwIdofServer;
	BYTE			m_cbAccountFlag;
	CTime			m_TimeOverDays;
	TCHAR			m_lpszAccount[MAX_ACCOUNT];
	BOOL			m_fRoute;
	DWORD			m_dwPing;
	DWORD			m_dwAuthKey;
	int				m_cbRef;
	int				m_fCheck;
	ACCOUNT_STATUS	m_nStatus;
	u_long			m_uIdofMulti;
	BOOL			m_bCertify;

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CAccount>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAccount::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAccount::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAccount::m_pPool->Free( (CAccount*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAccount::m_pPool->Free( (CAccount*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

class CAccountMng
{
private:
	map<string, CAccount*>	m_stringToPAccount;
	map<DWORD, int>			m_dpidToIndex;
	map<DWORD, CAccount*>	m_adpidToPAccount[MAX_CERTIFIER];
	int		m_nSizeof;
	int		m_nOldHour;

public:
	int						m_nCount;
	map<DWORD, DWORD>		m_2IdofServer;

	int		m_nYear;
	int		m_nMonth;
	int		m_nDay;
	int		m_nHour;
	int		m_nMinute;
public:
//	Constructions
	CAccountMng();
	virtual	~CAccountMng();
public:
//	Attributes
	int			GetIndex( DWORD dpid1 );
	int			GetIdofServer( DWORD dpid );
	CAccount*	GetAccount( LPCTSTR lpszAccount );
	CAccount*	GetAccount( DWORD dpid1, DWORD dpid2 );
#ifdef __LOG_PLAYERCOUNT_CHANNEL
	map<string, CAccount*> GetMapAccount();
#endif // __LOG_PLAYERCOUNT_CHANNEL

//	Operations
	void	Clear( void );
	void	AddConnection( DWORD dpid );
	void	RemoveConnection( DWORD dpid );
	void	AddIdofServer( DWORD dpid, DWORD dwIdofServer );
#ifdef __SERVERLIST0911
	DWORD	RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection = TRUE );
#else	// __SERVERLIST0911
	void	RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection = TRUE );
#endif	// __SERVERLIST0911

	BYTE	AddAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, DWORD* pdwAuthKey, BYTE b18, int fCheck );
	void	RemoveAccount( LPCTSTR lpszAccount );
	void	RemoveAccount( DWORD dpid1, DWORD dpid2 );
	BOOL	IsTimeCheckAddr();

public:
	CMclCritSec		m_AddRemoveLock;

};

#endif	// __ACCOUNT_H__