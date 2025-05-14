#ifndef __TIMELIMIT_H__
#define __TIMELIMIT_H__


class CTimeLimit
{
private:
	DWORD	m_dwTick;
	int		m_nRequest;

public:
	CTimeLimit();
	CTimeLimit( int nLimit, DWORD dwPeriod );

	BOOL Check();
	static void	SetLimitation( int nLimit, DWORD dwPeriod );
};


#endif // __TIMELIMIT_H__