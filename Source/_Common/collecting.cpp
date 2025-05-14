#include "stdafx.h"
#include "collecting.h"

CCollectingProperty::CCollectingProperty()
{
	m_nMaxBattery	= 0;
}

CCollectingProperty::~CCollectingProperty()
{
}

CCollectingProperty* CCollectingProperty::GetInstance()
{
	static CCollectingProperty	sProperty;
	return &sProperty;
}

int CCollectingProperty::GetCool( int nAbilityOption )
{
	return( nAbilityOption <= (int)( m_anCool.size() ) ? m_anCool[nAbilityOption]: INT_MAX );
}

#ifdef __WORLDSERVER
DWORD CCollectingProperty::GetItem( void )
{
//	DWORD dwRand	= xRandom( 10000 );	// 0 ~ 9999
	DWORD dwRand	= xRandom( 1000000000 );	// 0 ~ 999999999
	for( int i = 0; i < (int)( m_aItem.size() ); i++ )
	{
		COLLECTING_ITEM item	= m_aItem[i];
		if( dwRand < item.dwProb )
			return item.dwItemId;
	}
	return 0;
}
#endif

bool SortCollectorItems(_COLLECTING_ITEM& first, _COLLECTING_ITEM& second)
{
	return (first.dwChance > second.dwChance);
}

BOOL CCollectingProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if (s.Token == _T("Collecting_Item"))
		{
			vector<COLLECTING_ITEM>* ptr = GetItemPtr();
			COLLECTING_ITEM item;
#ifdef __WORLDSERVER
			DWORD dwTotal = 0;
#endif
			s.GetToken();	// {
			item.dwItemId = s.GetNumber();
			while (*s.token != '}')
			{
				item.dwChance = s.GetNumber();
#ifdef __WORLDSERVER
				dwTotal += item.dwChance;
				item.dwProb = dwTotal;
#endif
				ptr->push_back(item);
				item.dwItemId = s.GetNumber();
			}
#ifdef __CLIENT
			std::sort(ptr->begin(), ptr->end(), SortCollectorItems);
#endif
		}
		else if (s.Token == _T("Collecting_Enchant"))
		{
			vector<int>* ptr = GetEnchantProbabilityPtr();
			// 1 / 1000
			s.GetToken();	// {
			int nProb = s.GetNumber();
			while (*s.token != '}')
			{
				ptr->push_back(nProb);
				nProb = s.GetNumber();
			}
		}
		else if( s.Token == _T( "Collecting_Time" ) )
		{

			s.GetToken();	// {
			int nCool	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_anCool.push_back( nCool );
				nCool	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "nMaxBattery" ) )
		{
			s.GetToken();	// =
			m_nMaxBattery	= s.GetNumber();
		}
		s.GetToken();
	}
	return TRUE;
}

int CCollectingProperty::GetEnchantProbability( int nAbilityOption )
{
	return	( nAbilityOption < (int)( m_anEnchant.size() ) ? m_anEnchant[nAbilityOption]: 0 );
}
