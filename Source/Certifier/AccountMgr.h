// AccountMgr.h: interface for the CAccountMgr class.

////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_)
#define AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum ACCOUNT_CHECK {
	CHECK_OK,
	CHECK_3TIMES_ERROR,
};

struct ACCOUNT_CACHE
{
	DWORD	m_dwIP;
	int		m_nError;
	time_t	m_tmError;
};


class CAccountMgr
{
public:
	list< ACCOUNT_CACHE* >	m_cache;

public:
	CAccountMgr();
	~CAccountMgr();

	ACCOUNT_CHECK	Check( DWORD dwIP );
	void			SetError( int nError );
};

#endif // !defined(AFX_ACCOUNTMGR_H__57A5D903_C611_4722_99DD_AAA9FB5A8D57__INCLUDED_)
