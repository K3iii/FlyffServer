#ifndef __PET_H__
#define	__PET_H__

#if __VER >= 9	// __PET_0410

#ifdef __CLIENT
#include "..\_AIInterface\AIInterface.h"
#endif	// __CLIENT

#include "defineitem.h"

enum	PETLEVEL	{	PL_EGG,	PL_D,	PL_C,	PL_B,	PL_A,	PL_S,	PL_MAX,	};

#define	MAX_PET_AVAIL_LEVEL	9
#define	MAX_ADD_LIFE	5

typedef	struct	_FEEDENERGY
{
	DWORD	dwCostMin;
	DWORD	dwCostMax;
	WORD	wEnergyMin;
	WORD	wEnergyMax;
}	FEEDENERGY,	*PFEEDENERGY;


#define	MAX_PET_SHAPE	3
typedef	struct	_PETAVAILPARAM
{
	DWORD	dwDstParam;
	DWORD	m_anParam[MAX_PET_AVAIL_LEVEL];
	DWORD	m_dwItemId;
	DWORD	m_adwIndex[MAX_PET_SHAPE];
	DWORD	m_dwProbability;
}	PETAVAILPARAM, *PPETAVAILPARAM;

class CPetProperty
{
public:
	CPetProperty();
	virtual	~CPetProperty();

	PPETAVAILPARAM	GetAvailParam( BYTE nKind );
	BYTE	GetLevelupAvailLevel( BYTE wLevel );
	WORD	GetFeedEnergy( DWORD dwCost, int nIndex = 0 );
	DWORD	GetIncrementExp( BYTE nLevel );
	WORD	GetMaxEnergy( BYTE nLevel );
	WORD	GetAddLife( void );

#ifdef __JEFF_11_3
	LPDWORD		GetLevelupAvailLevelProbabilityPtr( void )	{	return &m_adwLevelupAvailLevelProbability[0][0];	}
	LPBYTE	GetLevelupAvailLevelMaxPtr( void )		{	return m_anLevelupAvailLevelMax;	}
	vector<WORD>*	GetAddLifeProbabilityPtr( void )	{	return &m_awAddLifeProbability;		}
#endif	// __JEFF_11_3

	static	CPetProperty*	GetInstance( void );
	BOOL	LoadScript( LPCTSTR szFile );
	BYTE	Hatch( void );
private:
	PETAVAILPARAM	m_aPetAvailParam[PK_MAX];
	BYTE	m_anLevelupAvailLevelMax[PL_MAX];
	DWORD	m_adwLevelupAvailLevelProbability[PL_MAX][MAX_PET_AVAIL_LEVEL];
	vector<FEEDENERGY>	m_aFeedEnergy[2];
	DWORD	m_adwIncrementExp[PL_MAX];
	WORD	m_awMaxEnergy[PL_MAX];
	vector<WORD>	m_awAddLifeProbability;
};

#define	MAX_PET_LIFE	99
#define	MAX_PET_EGG_EXP		(DWORD)50000
#define	MAX_PET_EXP		(DWORD)100000

#define	PF_PET	0
#define	PF_PET_LEVEL_UP	1
#define	PF_PET_LEVEL_DOWN	2
#define	PF_PET_GET_AVAIL	3

#ifdef __PET_1024
#define	MAX_PET_NAME_FMT	33	// 16 * 2 + 1
#define	MAX_PET_NAME	17
#endif	// __PET_1024

class CPet
{
public:
//	Constructions
	CPet();
	virtual	~CPet();

//	Operations
	void	Serialize( CAr & ar );

//	Attributions
	void	SetAvailLevel( BYTE nLevel, BYTE nAvailLevel );

	DWORD	GetIndex( void );
	BYTE	GetAvailLevel( BYTE nLevel );
	LPBYTE	GetAvailLevel( void )	{	return m_anAvailLevel;	}

	void	GetAvailDestParam( DWORD & dwDestParam, int & nParam );
	BYTE	GetKind( void )	{	return m_nKind;	}
	void	SetKind( BYTE nKind )	{	m_nKind	= nKind;	}
	BYTE	GetLevel( void )	{	return m_nLevel;	}
	void	IncLevel( void )		{	m_nLevel++;	}
	void	SetLevel( BYTE nLevel )	{	m_nLevel	= nLevel;	}
	DWORD	GetExp( void )	{	return m_dwExp;		}
	void	SetExp( DWORD dwExp );
	WORD	GetLife( void );
	void	SetLife( WORD wLife )	{	m_wLife	= wLife;	}
	WORD	GetEnergy( void )	{	return m_wEnergy;	}
	void	SetEnergy( WORD wEnergy );
	WORD	GetMaxEnergy( void )	{	return CPetProperty::GetInstance()->GetMaxEnergy( m_nLevel );	}
	int		GetEnergyPercent( void )	{	return GetEnergy() * 100 / GetMaxEnergy();	}
	int		GetExpPercent( void )	{	return( GetExp() * 100 / GetMaxExp() );		}
	int		GetMaxExp( void )	{	return ( m_nLevel == PL_EGG? MAX_PET_EGG_EXP: MAX_PET_EXP );	}
#ifdef __PET_1024
	void	SetName( const char* szName );
	const char*	GetName()	{	return m_szName;	}
	BOOL	HasName()	{	return strlen( m_szName ) > 0;	}
#endif	// __PET_1024
	void	InitEgg( void );
	DWORD	GetItemId( void )
		{
			if( m_nLevel == PL_EGG )
				return II_PET_EGG;
			return( CPetProperty::GetInstance()->GetAvailParam( m_nKind )->m_dwItemId );
		}
	CPet&	operator=( CPet & pet );
private:
	BYTE	m_nKind;
	BYTE	m_nLevel;
	DWORD	m_dwExp;
	WORD	m_wEnergy;
	WORD	m_wLife;
	BYTE	m_anAvailLevel[PL_MAX];
#ifdef __PET_1024
	char	m_szName[MAX_PET_NAME];
#endif	// __PET_1024
};

#ifdef __CLIENT

class CAIEgg: public CAIInterface
{
private:
	OBJID m_idOwner;
	int		m_nState;
	void MoveToDst(	OBJID idTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );
	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );
	void Init( void );
	void Destroy( void );
public:
	CAIEgg( CObj* pObj, OBJID idOwner );
	virtual ~CAIEgg();

	virtual void InitAI();

	int		GetState( void )	{	return m_nState;	}
	OBJID	GetOwnerId( void )	{	return m_idOwner;	}

	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	DECLARE_AISTATE()
};
#endif	// __CLIENT

#endif	// __PET_0410

#endif	// __PET_H__
