#ifndef __AI_INTERFACE_H
#define __AI_INTERFACE_H

#include "MoverMsg.h"

class CAIInterface;

struct AIMSG
{
	DWORD dwMessage;
	DWORD dwParam1;
	DWORD dwParam2;
};

typedef BOOL (CAIInterface::*FUNCAISTATE)( const AIMSG & );

struct STATEMAP_ENTRIES
{
	DWORD dwAIStateId;
	FUNCAISTATE pStateFunc;
};

struct STATEMAP
{
	STATEMAP* lpBaseStateMap;
	STATEMAP_ENTRIES* lpStateMapEntries;
};

enum
{
	AIMSG_INIT = 1,
	AIMSG_PROCESS,
	AIMSG_TIMEOUT,
	AIMSG_SETSTATE,
	AIMSG_SETPROCESS,
	AIMSG_DSTDAMAGE,
	AIMSG_DSTDIE,
	AIMSG_DAMAGE,
	AIMSG_DIE,
	AIMSG_COLLISION,
	AIMSG_ARRIVAL,
	AIMSG_NOMOVE,
	AIMSG_BEGINMOVE,
	AIMSG_ATTACK_MELEE,
	AIMSG_END_MELEEATTACK,
	AIMSG_END_APPEAR,
	AIMSG_INIT_TARGETCLEAR,
	AIMSG_EXIT,
	AIMSG_TARGET_FIND,
	AIMSG_REATTACK_TIMEOUT,
	AIMSG_INVALID_TARGET,
};

enum AI2_STATE
{
	AI2_IDLE,
	AI2_MOVE,
	AI2_RAGE,
	AI2_SEARCH,
	AI2_TRACKING,
	AI2_ATTACK,
};

////////////////////////////////////////////////////////////////////////

#define BeginAIHandler() if( 0 ) {
#define OnMessage( x ) return TRUE; } else if( msg.dwMessage == x )	{
#define EndAIHandler() return TRUE; }


#define BEGIN_AISTATE_MAP( theClass, baseClass ) \
	const STATEMAP* theClass::GetStateMap() const  \
		{ return (STATEMAP*)&theClass::stateMap; } \
	const STATEMAP theClass::stateMap = \
		{ (STATEMAP*)&baseClass::stateMap, (STATEMAP_ENTRIES*)&theClass::stateMapEntries[0] }; \
	const STATEMAP_ENTRIES theClass::stateMapEntries[] = { \

#define END_AISTATE_MAP() 0, NULL };
#define ON_STATE( id, pStateFunc ) id, (FUNCAISTATE)pStateFunc,
#define DECLARE_AISTATE() \
	static const STATEMAP_ENTRIES stateMapEntries[]; \
	static const STATEMAP stateMap; \
	virtual const STATEMAP* GetStateMap() const;

////////////////////////////////////////////////////////////////////////

class CAIInterface
{
	FUNCAISTATE GetAIStateFunc( DWORD dwState );

protected:
	queue< AIMSG >	m_MsgQueue;
//	CRIT_SEC		m_AddRemoveLock;
	FUNCAISTATE		m_pStateFunc;
	CObj*			m_pObj;

public:
	DECLARE_AISTATE()

	CAIInterface();
	CAIInterface( CObj* pObj );
	virtual	~CAIInterface();


	virtual void	RouteMessage();
	virtual void	SendAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	virtual void	PostAIMsg( DWORD dwMessage, DWORD dwParam1 = 0, DWORD dwParam2 = 0 );
	virtual void	InitAI() { }
#if __VER >= 9	//__AI_0509
	virtual		BOOL	IsReturnToBegin( void )		{	return FALSE;	}
#endif	// __AI_0509

protected:
	CMover* GetMover() { return (CMover*)m_pObj; }
	CWorld* GetWorld() { return m_pObj->GetWorld(); }


	CMover* ScanTarget( CObj* pObjCenter, int nRangeMeter = 5, int nJobCond = 0, DWORD dwQuest = 0, DWORD dwItem = 0, int nChao = 0 );
	CMover* ScanTargetStrong( CObj* pObjCenter, FLOAT fRangeMeter );
	CMover* ScanTargetOverHealer( CObj* pObjCenter, FLOAT fRangeMeter  );
#if __VER >= 9	//__AI_0509
	CMover*	ScanTargetNext( CObj* pObjCenter, int nRange,  OBJID dwIdTarget, u_long uParty );
	u_long	m_uParty;
#endif	// __AI_0509
};
////////////////////////////////////////////////////////////////////////


#endif

