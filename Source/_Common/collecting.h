#ifndef __COLLECTING_H__
#define	__COLLECTING_H__

typedef	struct	_COLLECTING_ITEM
{
	DWORD	dwItemId;
#ifdef __WORLDSERVER
	DWORD	dwProb;
#endif
	DWORD	dwChance;
	_COLLECTING_ITEM()
	{
		dwItemId	= 0;
#ifdef __WORLDSERVER
		dwProb	= 0;
#endif
		dwChance = 0;
	}
}	COLLECTING_ITEM;

class CCollectingProperty
{
public:
	CCollectingProperty();
	virtual	~CCollectingProperty();
	static	CCollectingProperty*	GetInstance();
	BOOL	LoadScript( LPCTSTR szFile );

	int		GetCool( int nAbilityOption );
	int		GetMaxBattery( void )	{	return m_nMaxBattery;	}
#ifdef __WORLDSERVER
	DWORD	GetItem( void );
#endif
	int		GetEnchantProbability( int nAbilityOption );
	int		GetMaxCollectorLevel( void )	{	return m_anEnchant.size();	}

	vector<int>*	GetEnchantProbabilityPtr( void )	{	return &m_anEnchant;		}
	vector<COLLECTING_ITEM>*	GetItemPtr( void )	{	return &m_aItem;	}

private:
	vector<int>	m_anCool;
	vector<int>	m_anEnchant;
	vector<COLLECTING_ITEM>	m_aItem;
	int		m_nMaxBattery;
};
#endif	// __COLLECTING_H__