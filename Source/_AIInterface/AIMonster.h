#ifndef __AI_MONSTER_H
#define __AI_MONSTER_H

#include "mempooler.h"
#include "Mover.h"
#include "PathObj.h"

class CAIMonster : public CAIInterface
{
protected:
	DWORD 			m_tmMove;
	DWORD			m_tmAttackDelay;
	DWORD			m_tmAttack;
	DWORD			m_tmSummon;
	DWORD			m_tmHelp;
	DWORD			m_tmReturnToBegin;

	D3DXVECTOR3		m_vPosBegin;
	D3DXVECTOR3		m_vDestPos;
	D3DXVECTOR3		m_vOldPos;
	D3DXVECTOR3		m_vPosDamage;

	_VECINFO		m_vecPatrolInfo;
	FLOAT			m_fAngleBegine;

	BOOL			m_bReturnToBegin;
#ifndef __FL_UPDATE_REAGGRO
	BOOL			m_bGoTarget;
#endif
	BOOL			m_bTargetNoMovePos;
	BOOL			m_bFirstRunaway;
	BOOL			m_bCallHelper;
	BOOL			m_bRangeAttack;

	DWORD			m_dwIdTarget;
	DWORD			m_dwAtkMethod;
	vector<OBJID>	m_vecIdSummon;

	DWORD			GetAtkMethod_Near();
	DWORD			GetAtkMethod_Far();
	DWORD			GetAtkRange( DWORD dwAtkMethod );
	ItemProp*		GetAtkProp( DWORD dwAtkMethod );
	DWORD			GetAtkItemId( DWORD dwAtkMethod );
	void			DoAttack( DWORD dwAtkMethod, CMover* pTarget );
	void			DoReturnToBegin( BOOL bReturn = TRUE );
	void			MoveToDst( OBJID dwIdTarget );
	void			MoveToDst( const D3DXVECTOR3& vDst );
	void			MoveToRandom( UINT nState );
	void			CallHelper( const MoverProp* pMoverProp );
	void			SetStop( DWORD dwTime );
	BOOL			IsEndStop();
	BOOL			IsMove() { return GetMover()->m_pActMover->IsMove(); }
	BOOL			IsInRange( D3DXVECTOR3& vDistant, FLOAT fRange );
	int				SelectAttackType( CMover *pTarget );
	BOOL			MoveProcessIdle( const AIMSG & msg );
	BOOL			MoveProcessRage( const AIMSG & msg );
	BOOL			SubAttackChance( const AIMSG & msg, CMover *pTarget );
	BOOL			MoveProcessRunaway();
	BOOL			StopProcessIdle();
	void			SubSummonProcess( CMover *pTarget );
	void			Init();
	BOOL			StateInit( const AIMSG & msg );
	BOOL			StateIdle( const AIMSG & msg );
	BOOL			StateWander( const AIMSG & msg );
	BOOL			StateRunaway( const AIMSG & msg );
	BOOL			StateEvade( const AIMSG & msg );
	BOOL			StateRage( const AIMSG & msg );

	BOOL			StateRagePatrol( const AIMSG & msg );
	BOOL			MoveProcessRagePatrol( const AIMSG & msg );
	BOOL			StateStand( const AIMSG & msg );
	BOOL			StatePatrol( const AIMSG & msg );
	BOOL			MoveProcessStand( const AIMSG & msg );
	BOOL			MoveProcessPatrol( const AIMSG & msg );

	void	SetTarget( OBJID dwIdTarget, u_long uParty );

#if __VER >= 9	//__AI_0509
public:
	virtual	BOOL	IsReturnToBegin( void )		{	return m_bReturnToBegin;	}
protected:
#endif	// __AI_0509

public:
	CAIMonster();
	CAIMonster( CObj* pObj );
	virtual ~CAIMonster();

	virtual void	InitAI();

#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIMonster>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIMonster::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIMonster::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIMonster::m_pPool->Free( (CAIMonster*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIMonster::m_pPool->Free( (CAIMonster*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820

	DECLARE_AISTATE()
};


#if __VER >= 12 // __MONSTER_SKILL
struct __MONSTERSKILL
{
	int		nAtkMethod;
	DWORD	dwSkillID;
	DWORD	dwSkillLV;
	int		nHitCount;
	int		nRange;
	int		nApplyProbabilty;
	DWORD	dwSkillTime;
	bool	bIgnoreSkillProb;
	__MONSTERSKILL()
	{
       nAtkMethod	= 0;
		dwSkillID	= NULL_ID;
		dwSkillLV	= 0;
		nHitCount	= 0;
		nRange		= 0;
		nApplyProbabilty = 0;
		dwSkillTime = 0;
		bIgnoreSkillProb = 0;
	}
};

typedef map<DWORD, vector<__MONSTERSKILL> > MAPMONSTERSKILL;

const DWORD ATK_MELEE = 1;
const DWORD ATK_RANGE = 2;

class CMonsterSkill
{
private:
	CMonsterSkill();
public:
	~CMonsterSkill();
	static CMonsterSkill* GetInstance( void );

	void	LoadScript();
	void	Clear();

	DWORD	GetMonsterSkillLevel( CMover* pAttacker, DWORD dwSkillId );
	BOOL	ApplySkill( CMover* pAttacker, CMover* pTarget, DWORD dwAtkMethod );
#if __VER >= 14 // __INSTANCE_DUNGEON
	BOOL	MonsterTransform( CMover* pMover, int nMoverHP );
#endif // __INSTANCE_DUNGEON

private:
	MAPMONSTERSKILL		m_mapMonsterSkill;
};
#endif // __MONSTER_SKILL

#endif

