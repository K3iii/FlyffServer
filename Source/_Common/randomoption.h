#ifndef __RANDOM_OPTION_EX_H__
#define	__RANDOM_OPTION_EX_H__

#include <vector>

#define	MAX_RANDOM_OPTION	3


#define _AWAKE_SAFE_FLAG	0x2000000000000000

#define _AWAKE_OLD_VALUE	1
#define _AWAKE_NEW_VALUE	2



typedef	struct	_AdjData
{
	short	nAdj;
	DWORD	dwProb;
	_AdjData( short n, DWORD dw )
	{
		nAdj	= n;
		dwProb	= dw;
	}
}	AdjData;

typedef struct	_RANDOM_OPTION
{
	int		nDst;
	int		nProb;
	vector<AdjData>	aAdjData;
	_RANDOM_OPTION()
	{
		nDst	= 0;
		nProb	= 0;
	}
	_RANDOM_OPTION( _RANDOM_OPTION* pRandomOption )
	{
		nDst	= pRandomOption->nDst;
		nProb	= pRandomOption->nProb;
		aAdjData.assign( pRandomOption->aAdjData.begin(), pRandomOption->aAdjData.end() );
	}
}	RANDOM_OPTION;

class CRandomOptionProperty
{
public:
#if __VER >= 12 // __PET_0519

	enum	{	eAwakening, eBlessing,	eSystemPet, eEatPet, eMaxRandomOptionKind	};
#else	// __PET_0519
	enum	{	eAwakening, eBlessing,	eMaxRandomOptionKind	};
#endif	// __PET_0519

#if __VER >= 12 // __J12_0
	enum	{	eAwakeningExtension	= 3	};
#endif	// __J12_0

	CRandomOptionProperty();
	virtual	~CRandomOptionProperty();
	static	CRandomOptionProperty*	GetInstance();

	BOOL	LoadScript( LPCTSTR szFile );
	void	LoadScriptBlock( CScript& s, int nRandomOptionKind );


	BOOL	GetParam( __int64 nRandomOptItemId, int i, int* pnDst, int* pnAdj );

	int		GetRandomOptionSize( __int64 nRandomOptItemId );


	int		GetViewRandomOptionSize( __int64 n64RandomOptItemId );



	int		GetRandomOptionKind( CItemElem* pItemElem );



	//BOOL	GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts );
	BOOL	GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts, bool bDecreaseFlag = false );



	void	InitializeRandomOption( __int64* pnRandomOptItemId );


	void	SetParam( __int64* pnRandomOptItemId, int nDst, int nAdj );
private:
	int		DetermineRandomOptionSize( int nRandomOptionKind );
	RANDOM_OPTION*	DetermineRandomOptionDst( int nRandomOptionKind, int nParts );


	//short DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption );
	short DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption, bool bDecreaseAdj = false );


	int		GetRandomOptionKindIndex( int nRandomOptionKind, int nParts );
	int		GetUpperProbability( int iRandomOptionKindIndex );
#if __VER >= 12 // __J12_0
	void	AwakeningExtension( void );
#endif	// __J12_0
private:
	int		m_anRandomOptionProb[eMaxRandomOptionKind][MAX_RANDOM_OPTION];
#if __VER >= 12 // __J12_0

	vector<RANDOM_OPTION>	m_aRandomOption[eMaxRandomOptionKind + eAwakeningExtension];
#else	// __J12_0
	vector<RANDOM_OPTION>	m_aRandomOption[eMaxRandomOptionKind];
#endif	// __J12_0



public:
	bool	IsCheckedSafeFlag( __int64 n64RandomeOption );
	void	SetSafeFlag( __int64* pn64RandomOption );
	void	ResetSafeFlag( __int64* pn64RandomOption );

};


#define g_xRandomOptionProperty		CRandomOptionProperty::GetInstance()

#endif	//__RANDOM_OPTION_EX_H__