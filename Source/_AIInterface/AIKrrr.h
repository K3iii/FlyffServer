#ifndef __AI_KRRR_H
#define __AI_KRRR_H

#include "mempooler.h"

class CAIKrrr : public CAIInterface
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
//	CMover*	m_pEventTarget;
	OBJID	m_idEventTarget;
	int		m_nEventCount;

	OBJID	m_idLastAttacker;

	BOOL	m_bDefenseMode;

	BOOL	MoveProcessIdle();
	BOOL	MoveProcessRage();
	BOOL	MoveProcessRunaway();

	BOOL	StopProcessIdle();

	BOOL	SelectTarget( void );

	void Init( void );
	void Destroy( void );
	int GetEvent( void ) { return m_nEvent; }
	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateWander( const AIMSG & msg );
	BOOL	StateRunaway( const AIMSG & msg );
	BOOL	StateEvade( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

public:

	CAIKrrr();
	CAIKrrr( CObj* pObj );
	virtual ~CAIKrrr();

	virtual void InitAI();

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CAIKrrr>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CAIKrrr::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CAIKrrr::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CAIKrrr::m_pPool->Free( (CAIKrrr*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CAIKrrr::m_pPool->Free( (CAIKrrr*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820

	DECLARE_AISTATE()
};

#endif

