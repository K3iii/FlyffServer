#ifndef __RANGDA_H__
#define	__RANGDA_H__


typedef	struct	_RANGDA_POS
{
	DWORD	dwWorldId;
	D3DXVECTOR3 vPos;
}	RANGDA_POS,	*PRANGDA_POS;

typedef	vector<RANGDA_POS>	VRP;

class CRangda
{
public:
	CRangda( DWORD dwMonster );
	CRangda( DWORD dwMonster, int nInterval, int nReplace, BOOL bActiveAttack );
	virtual	~CRangda();
	void	AddPos( const RANGDA_POS & vPos );
	void	OnTimer();
	void	SetInterval( int nInterval )	{	m_nInterval	= nInterval;	}
	void	SetReplace( int nReplace )	{	m_nReplace	= nReplace;		}
	void	SetActiveAttack( BOOL bActiveAttack )	{	m_bActiveAttack		= bActiveAttack;	}

private:
	RANGDA_POS	GetRandomPos( void );
	void	SetObj( OBJID objid );
	BOOL	HavetoGenerate( void );
	BOOL	HavetoReplace( void );
	void	CreateMonster( void );
	BOOL	IsDamaged( CMover* pMonster );
	void	ProcessReplace( CMover* pMonster );
	void	ProcessGenerate( void );
	CMover*		GetMonster( void );
private:
	const DWORD	m_dwMonster;
	int		m_nInterval;
	int		m_nReplace;
	BOOL	m_bActiveAttack;
	VRP	m_vvPos;
	int		m_nGenerateCountdown;
	int		m_nReplaceCountdown;
	BOOL	m_bReplaceable;
	int		m_nOldPos;
	OBJID	m_objid;
};

typedef	vector<CRangda*>	VR;

class CRangdaController
{
public:
	virtual	~CRangdaController();
	static	CRangdaController*	Instance( void );
	void	AddRangda( CRangda* pRangda );
	void	OnTimer( void );
	BOOL	LoadScript( const char* szFile );
private:
	CRangdaController();
private:
	VR	m_vRangda;
};
#endif	// __RANGDA_H__