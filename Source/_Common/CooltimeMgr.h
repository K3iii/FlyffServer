#ifndef	__COOLTIMEMGR_H__
#define __COOLTIMEMGR_H__

#ifndef __AIO_PETS
const int MAX_COOLTIME_TYPE = 3;
#else
const int MAX_COOLTIME_TYPE = 5;
#endif

struct ItemProp;


class CCooltimeMgr
{
public:
	CCooltimeMgr();
	virtual ~CCooltimeMgr();

	static DWORD		GetGroup( ItemProp* pItemProp );

	BOOL				CanUse( DWORD dwGroup );
	void				SetTime( DWORD dwGroup, DWORD dwCoolTime );
	DWORD				GetTime( DWORD dwGroup )
	{
		ASSERT( dwGroup > 0 );
		return m_times[ dwGroup - 1];
	}

	DWORD				GetBase( DWORD dwGroup )
	{
		ASSERT( dwGroup > 0 );
		return m_bases[ dwGroup - 1];
	}

protected:
	DWORD				m_times[MAX_COOLTIME_TYPE];
	DWORD				m_bases[MAX_COOLTIME_TYPE];
};

#endif	__COOLTIMEMGR_H__