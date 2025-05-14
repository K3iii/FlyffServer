#ifndef __AIMETEONYKER_H__
#define	__AIMETEONYKER_H__

class CAIMeteonyker : public CAIInterface
{

	OBJID	m_dwIdTarget;
	u_long	m_uParty;



	int		m_cbRetaliation;
	map<OBJID, int>	m_mapDamage;
	void	AddDamage( OBJID idAttacker, int nDamage );
	void	ResetDamage( void );
	OBJID	GetDealer( void );
	BOOL	ProcessRetaliation( void );


	BOOL	ProcessSummon( BOOL bUnconditional = FALSE );



	BOOL	ProcessBurn( BOOL bUnconditional = FALSE );

//	BOOL	m_bHeal;


	int		m_cbAttack;
	int		m_nSPAttackType;
	BOOL	ProcessSPAttack( void );

	BOOL	ProcessHeal( void );
	time_t	m_tmRecovery;
	void	ProcessRecovery( void );

	DWORD	m_dwHPOld;
	BOOL	m_bCritical;
	time_t	m_tmAttackDelay;
	time_t	m_tmTrace;
	DWORD	m_dwAtk;
	BOOL	SelectTarget( void );
	OBJID	m_idLastAttacker;
	D3DXVECTOR3	m_vPosBegin;
	BOOL	m_bReturnToBegin;
	time_t	m_tmReturnToBegin;
	time_t	m_tmAttack;
	D3DXVECTOR3	m_vOldPos;
	time_t	m_tmMove;
	D3DXVECTOR3	m_vPosDamage;

	void Init( void );
	void Destroy( void );
	BOOL	MoveProcessIdle( void );
	BOOL	MoveProcessRage( void );
	BOOL	MoveProcessRunaway( void );
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	void	SetStop( DWORD dwTime );
	BOOL	IsEndStop( void );
	BOOL	IsMove() {	return GetMover()->m_pActMover->IsMove();	}
	void	MoveToDst( const D3DXVECTOR3& vDst );
	void	MoveToDst( OBJID dwIdTarget );
	void	MoveToRandom( UINT nState );
	BOOL	IsInRange( const D3DXVECTOR3& vDistant, FLOAT fRange );
	void	DoReturnToBegin( BOOL bReturn = TRUE );
private:
	CMover*		GetTarget( OBJID objid );
public:
	CAIMeteonyker();
	CAIMeteonyker( CObj* pObj );
	virtual	~CAIMeteonyker();
	virtual	void	InitAI( void );
	virtual	BOOL	IsReturnToBegin( void )		{	return m_bReturnToBegin;	}

	void	SetTarget( OBJID dwIdTarget, u_long uParty );

	DECLARE_AISTATE();
};

#endif	// __AIMETEONYKER_H__