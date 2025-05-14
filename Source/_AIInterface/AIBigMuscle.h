#ifndef __AI_BIGMUSCLE_H
#define __AI_BIGMUSCLE_H

#include "mempooler.h"

class CAIBigMuscle : public CAIInterface
{
	BOOL	m_bEventFlag[5];
	D3DXVECTOR3 m_vPosBegin;
	int		m_nEvent;
	int		m_nAttackType;
	DWORD	m_tmReattack;
	DWORD	m_tmAddReattack;
	DWORD	m_tmTrace;
	DWORD	m_tmTimeOver;
	OBJID	m_idTarget;
	D3DXVECTOR3	m_vTarget;
	int		m_nAppearCnt;

	OBJID	m_idLastAttacker;

	BOOL	m_bDefenseMode;

	BOOL	MoveProcessIdle();
	BOOL	MoveProcessRage();
	BOOL	MoveProcessRunaway();

	BOOL	StopProcessIdle();

	BOOL	SelectTarget( void );

	void Init( void );
	void Destroy( void );

public:

	CAIBigMuscle();
	CAIBigMuscle( CObj* pObj );
	virtual ~CAIBigMuscle();

	virtual void InitAI();

	int GetEvent( void ) { return m_nEvent; }
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIBigMuscle>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIBigMuscle::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIBigMuscle::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIBigMuscle::m_pPool->Free( (CAIBigMuscle*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIBigMuscle::m_pPool->Free( (CAIBigMuscle*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820

	DECLARE_AISTATE()
};

#endif

