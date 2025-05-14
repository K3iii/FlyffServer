#ifndef __ACTIONMOVER_H
#define __ACTIONMOVER_H


#include "Mover.h"
#include "Action.h"
#include "MoverMsg.h"
#ifdef __CLIENT
#include "TailEffectMng.h"
#endif
#include "mempooler.h"

#ifdef __CLIENT
extern CDamageNumMng g_DamageNumMng;
#endif	// __CLIENT

#define MAX_MOVE_HEIGHT		270.0f
#define MIN_MOVE_HEIGHT		85.0f

#define	CTRLMSG_FORWARD		0x00000001
#define	CTRLMSG_BACKWARD	0x00000002
#define	CTRLMSG_LEFT		0x00000004
#define	CTRLMSG_RIGHT		0x00000008
#define	CTRLMSG_LDOWN		0x00000010
#define	CTRLMSG_RDOWN		0x00000020

#define	AF_GENERIC			(DWORD)0x00000001
#define	AF_MISS				(DWORD)0x00000002
#define	AF_RESERVED			(DWORD)0x00000004
#define	AF_MAGIC			(DWORD)0x00000008
#define AF_MELEESKILL		(DWORD)0x00000010
#define AF_MAGICSKILL		(DWORD)0x00000020
#define AF_CRITICAL1		(DWORD)0x00000040
#define AF_CRITICAL2		(DWORD)0x00000080
#define AF_CRITICAL			(DWORD)0x000000c0
#define AF_PUSH				(DWORD)0x00000100
#define AF_PARRY			(DWORD)0x00000200
#define AF_RESIST			(DWORD)0x00000400
#define AF_STUN				(DWORD)0x00000800
#define AF_BLOCKING			(DWORD)0x00001000
#define AF_FORCE			(DWORD)0x00002000
#define AF_RANGE			(DWORD)0x00004000
#define AF_MONSTER_SP_CLIENT (DWORD)0x00008000
#define AF_FLYING			(DWORD)0x10000000


inline BOOL IsSkillAttack( DWORD dwAtkFlags )
{
	return (dwAtkFlags & (AF_MELEESKILL|AF_MAGICSKILL) );
}


#	ifdef __WORLDSERVER
#		define	MAX_CORR_SIZE_45	5
#		define	MAX_CORR_SIZE_150	9
#	else	// __WORLDSERVER
#		define	MAX_CORR_SIZE_45	22
#		define	MAX_CORR_SIZE_150	37
#	endif	// __WORLDSERVER


///////////////////////////////////////////////////////////////
// CActionMover
///////////////////////////////////////////////////////////////
class CActionMover;
typedef	MemPooler<CActionMover>	CActionMoverPool;

class CActionMover : public CAction
{
public:
	CActionMover();
	CActionMover( CMover* pMover );
	virtual	~CActionMover();

public:
	int				m_nCount;
	int				m_bGround;
	float			m_fSpeed;

	float			m_fAccPower;
	D3DXVECTOR3		m_vDelta;
	D3DXVECTOR3		m_vDeltaE;
	D3DXVECTOR3		m_vDeltaAccu;
	D3DXVECTOR3		m_vPosLast;
	float			m_fCurrentHeight;
	FLOAT			m_fTurnAngle;
	DWORD			m_idTarget;
	int				m_nMotionEx;
	DWORD			m_dwAtkFlags;
	int				m_nShootCnt;
	int				m_nMotionHitCount;
#ifdef __CLIENT
	DWORD			m_dwCtrlMsg;
#endif

private:
	OBJID			m_objidHit;
	int				m_nCastingTime;
#if __VER >= 10
	DWORD			m_dwCastingEndTick;
	int				m_nCastingSKillID;
	int				m_nCastingTick;
#endif
	int				m_bMove;

#ifdef __CLIENT
	CTailEffectBelt*	m_pTail;
#endif

public:
	virtual int		SendActMsg( OBJMSG dwMsg, int nParam1 = 0, int nParam2 = 0, int nParam3 = 0, int nParam4 = 0, int nParam5 = 0 );
	virtual	void	ProcessState( DWORD dwState, float fSpeed );


#if __VER >= 10
	int				GetCastingSKillID() { return m_nCastingSKillID;}
	DWORD			GetCastingEndTick() { return m_dwCastingEndTick;}
	void			SetCastingEndTick(DWORD dwCastingEnd) { m_dwCastingEndTick = dwCastingEnd;}
	int				GetCastingTick() { return m_nCastingTick;}
#endif

	void			SetExternalForce( const D3DXVECTOR3 &vForce ) { m_vDeltaE = vForce; }
	int				MakeParamDamage( int nMagicPower, int nSkill, int nDmgCnt = 0 );
	int				SendDamage( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE, int nReflect = 0 )	{	return SendActMsg( OBJMSG_DAMAGE, dwAtkFlags, idSender, nParam, bTarget, nReflect );	}
	int				SendDamageForce( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE );


	DWORD			_ProcessMsgDmg( DWORD dwMsg, CMover* pSender, DWORD dwAtkFlags, int nParam = 0, BOOL bTarget = TRUE, int nReflect = 0 );
	void			_ProcessStateAttack( DWORD dwState, int nParam = 0 );
	void			_ProcessStateAttack2( DWORD dwState, int nParam = 0 );

	void			ProcessStateAttackSP( DWORD dwState, int nParam = 0 );
	void			ProcessStateAttackMagic( DWORD dwState, int nParam = 0 );
	void			ProcessStateAttackRange( DWORD dwState, int nParam = 0 );
	void			ProcessStateMeleeSkill( DWORD dwState, int nParam );
	void			ProcessStateMagicSkill( DWORD dwState, int nParam );
	void			ProcessStateMagicCasting( DWORD dwState, int nParam );
	void			ProcessPreAction();
	int				ProcessAction( const D3DXVECTOR3 *vPos );
	int				ProcessActMsg1( CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5 );
	int				ProcessActMsg2( CMover* pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nparam3, int nParam4, int nParam5 );
	void			ProcessState1( CMover* pMover, DWORD dwState, float fSpeed );
	void			ProcessState2( CMover* pMover, DWORD dwState, float fSpeed );
	void			ProcessFlyMove();
	BOOL			ProcessCollisionFly( D3DXVECTOR3 *vPos );
	BOOL			ProcessCollisionGround( D3DXVECTOR3 *vPos );
	BOOL			ProcessCollision( D3DXVECTOR3 *vPos );


	void			SetObjHit( OBJID idHit ) {	m_objidHit = idHit;	}
	OBJID			GetObjHit() {	return m_objidHit; }
	CMover*			PreProcessDamage( CMover* pMover, OBJID idAttacker, BOOL bTarget, int nReflect );

#ifdef __CLIENT
	DWORD			OnDamageMsgC( DWORD dwMsg, CMover* pAttacker, DWORD dwAtkFlags, int nParam );
#endif	// __CLIENT

#ifndef __VM_0820
#ifndef __MEM_TRACE
	static			CActionMoverPool*	m_pPool;
	void*			operator new( size_t nSize )	{	return CActionMover::m_pPool->Alloc();	}
	void*			operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CActionMover::m_pPool->Alloc();	}
	void			operator delete( void* lpMem )	{	CActionMover::m_pPool->Free( (CActionMover*)lpMem );	}
	void			operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CActionMover::m_pPool->Free( (CActionMover*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820

protected:
	void			Init();
	void			Destroy();
	void			ProcessFlyParticle( float fLenSq );
	void			ProcessFlyTracking();
};

inline int CActionMover::MakeParamDamage( int nMagicPower, int nSkill, int nDmgCnt)
{
	int nParam = nMagicPower;	// D0~D7
	nParam |= (nDmgCnt << 8);	// D7~D15
	nParam |= (nSkill << 16);	// D16~D31
	return nParam;
}



#endif	// __ACTIONMOVER_H