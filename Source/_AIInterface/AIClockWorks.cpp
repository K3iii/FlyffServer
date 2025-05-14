#include "stdafx.h"

#include "AIClockWorks.h"
#include "User.h"
#include "dpcoreclient.h"
#include "Party.h"

extern	CUserMng		g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CPartyMng		g_PartyMng;

// Event Pattern Type
#define		EPT_READY			0
#define		EPT_MOVING			1
#define		EPT_ATTACKING		2
#define		EPT_TRACKING		3

// ClockWorks Attack Pattern
#define		CAT_NONE			0
#define		CAT_SLASH			1
#define		CAT_SHOOT			2
#define		CAT_LASER			3
#define		CAT_QUAKE			4

enum
{
	STATE_INIT = 1,
	STATE_APPEAR,
	STATE_IDLE,
	STATE_RAGE,
	STATE_SUPER_RAGE
};
BEGIN_AISTATE_MAP( CAIClockWorks, CAIInterface )

	ON_STATE( STATE_INIT   , &CAIClockWorks::StateInit   )
	ON_STATE( STATE_IDLE   , &CAIClockWorks::StateIdle   )
	ON_STATE( STATE_RAGE   , &CAIClockWorks::StateRage   )
	ON_STATE( STATE_SUPER_RAGE   , &CAIClockWorks::StateRage   )

END_AISTATE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __VM_0820
#ifndef __MEM_TRACE
	#ifdef __VM_0819
	MemPooler<CAIClockWorks>*	CAIClockWorks::m_pPool		= new MemPooler<CAIClockWorks>( 4, "CAIClockWorks" );
	#else	// __VM_0819
	MemPooler<CAIClockWorks>*	CAIClockWorks::m_pPool		= new MemPooler<CAIClockWorks>( 4 );
	#endif	// __VM_0819
#endif	// __MEM_TRACE
#endif	// __VM_0820

CAIClockWorks::CAIClockWorks()
{
	Init();
}
CAIClockWorks::CAIClockWorks( CObj* pObj ) : CAIInterface( pObj )
{
	Init();
}

CAIClockWorks::~CAIClockWorks()
{
	Destroy();
}

void CAIClockWorks::Init( void )
{
	m_vPosBegin.x = m_vPosBegin.y = m_vPosBegin.z = 0;
	m_nEvent = EPT_READY;
	m_nAttackType = 0;
	m_tmReattack = m_tmAddReattack = m_tmTrace = m_tmTimeOver = timeGetTime();
	m_idTarget = NULL_ID;
	m_vTarget.x = m_vTarget.y = m_vTarget.z = 0;
	m_idLastAttacker = NULL_ID;
	m_bDefenseMode = FALSE;
	m_nAppearCnt = 0;
}

void CAIClockWorks::Destroy( void )
{
}

void CAIClockWorks::InitAI()
{
	m_vPosBegin = GetMover()->GetPos();
	PostAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
#ifdef __INTERNALSERVER
	DEBUGOUT2( "ClockWorks start" );
#endif
}

BOOL CAIClockWorks::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIClockWorks::MoveProcessIdle()
{
#ifdef __MOVER_STATE_EFFECT
	CMover* pMover = GetMover();

	if( IsInvalidObj( pMover ) == TRUE && pMover->m_pActMover->IsStateFlag( OBJSTAF_COMBAT ) == TRUE )
	{
		if( pMover->m_pActMover->SendActMsg( OBJMSG_MODE_PEACE ) == TRUE )
		{
			g_UserMng.AddSendActMsg( pMover, OBJMSG_MODE_PEACE );
		}
	}
#endif // __MOVER_STATE_EFFECT

	return TRUE;
}

BOOL CAIClockWorks::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )
		SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );
	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessIdle();

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )
		SendAIMsg( AIMSG_SETSTATE, STATE_RAGE, NULL_ID );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

		/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )

	EndAIHandler( )
			return TRUE;
}

BOOL	CAIClockWorks::SelectTarget( void )
{
	CMover	*pMover = GetMover();
	CWorld	*pWorld = GetWorld();
	CModelObject *pModel = (CModelObject *)pMover->GetModel();

	FLOAT fRadius = pMover->GetRadiusXZ();
	FLOAT fRadiusSq = fRadius * fRadius;

	CMover *pLastAttacker = prj.GetMover( m_idLastAttacker );
	if( pLastAttacker == NULL )
	{
		m_idLastAttacker = NULL_ID;
	} else
	{
		D3DXVECTOR3 vDist = pLastAttacker->GetPos() - pMover->GetPos();
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq >= fRadiusSq * 10.0f )
		{

#ifdef __INTERNALSERVER
			CString str;
			str.Format( "LastAtker(%s)가 너무 멀어서(%5.2f) 타겟포기함", pLastAttacker->GetName(), sqrt(fDistSq) );
			DEBUGOUT2( str, "boss.txt" );
#endif

			m_idLastAttacker = NULL_ID;
			pLastAttacker = NULL;
		}
	}


	m_idTarget = NULL_ID;
	m_vTarget.x = m_vTarget.y = m_vTarget.z = 0;

	if( m_idLastAttacker == NULL_ID )
	{

		CMover* pTarget = ScanTarget( pMover, 80, JOB_ALL );
		if( pTarget )
		{

			if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )
			{
				m_idTarget = pTarget->GetId();
#ifdef __INTERNALSERVER
				CString str;
				str.Format( "scanTarget:%s", pTarget->GetName() );
				DEBUGOUT2( str, "boss.txt" );
#endif

			}
			else
				return FALSE;
		} else
			return FALSE;
	} else

	{
		DWORD dwNum = xRandom( 100 );
		if( dwNum < 50 )
		{

			m_idTarget = m_idLastAttacker;
		} else
		if( dwNum < 75 )
		{

			CMover *pTarget = ScanTargetStrong( pMover, 50 );
			if( pTarget )
			{

				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )
				{
					m_idTarget = pTarget->GetId();
#ifdef __INTERNALSERVER
					CString str;
					str.Format( "Leader:%s %d", pTarget->GetName(), pTarget->GetLevel() );
					DEBUGOUT2( str, "boss.txt" );
#endif
				}
				else
					m_idTarget = m_idLastAttacker;
			} else
				m_idTarget = m_idLastAttacker;
		} else
		if( dwNum < 100 )
		{

			CMover *pTarget = ScanTargetOverHealer( pMover, 50 );
			if( pTarget )
			{

				if( pMover->IsFlyingNPC() || pTarget->m_pActMover->IsFly() == FALSE )
				{
					m_idTarget = pTarget->GetId();
#ifdef __INTERNALSERVER
					CString str;
					str.Format( "OverHealer:%s", pTarget->GetName() );
					DEBUGOUT2( str, "boss.txt" );
#endif
				}
				else
					m_idTarget = m_idLastAttacker;
			} else
				m_idTarget = m_idLastAttacker;
		}
#ifdef __INTERNALSERVER
		{
			CMover *pTarget = prj.GetMover( m_idTarget );
			CMover *pMover = prj.GetMover( m_idLastAttacker );
			CString str;
			if( pMover )
			{
				if( pTarget )
					str.Format( "rand:%d LastAttacker:%s SelectTarget:%s", dwNum, pMover->GetName(), pTarget->GetName() );
				else
					str.Format( "rand:%d LastAttacker:%s SelectTarget:%s", dwNum, pMover->GetName(), "없음" );
				DEBUGOUT2( str, "boss.txt" );
			} else
			{
				if( pTarget )
					str.Format( "rand:%d LastAttacker:%s SelectTarget:%s", dwNum, "없음.", pTarget->GetName() );
				else
					str.Format( "rand:%d LastAttacker:%s SelectTarget:%s", dwNum, "없음.", "없음" );

				DEBUGOUT2( str, "boss.txt" );
			}

		}
#endif
	}







	return TRUE;
}


//	Rage Processs

BOOL CAIClockWorks::MoveProcessRage()
{
	CMover	*pMover = GetMover();
	CWorld	*pWorld = GetWorld();
	CModelObject *pModel = (CModelObject *)pMover->GetModel();

	if( m_bDefenseMode == FALSE && pMover->GetHitPointPercent() <= 100 )
	{
		m_bDefenseMode = TRUE;
		pMover->SetDestParam( DST_ADJDEF_RATE, 800, NULL_CHGPARAM );
	}


	if( m_nEvent == EPT_READY )
	{
		if( pMover->m_pActMover->m_bGround == 1 && pMover->m_pActMover->GetActionState() != OBJSTA_APPEAR )
			pMover->SendActMsg( OBJMSG_APPEAR );
		if( ++m_nAppearCnt >= PROCESS_COUNT * 5 )
		{
			PostAIMsg( AIMSG_END_APPEAR );
			pMover->m_pActMover->ResetState( OBJSTA_ACTION_ALL );
			m_nAppearCnt = 0;
		}

	} else

	if( m_nEvent == EPT_ATTACKING )
	{
		BOOL bEnd = FALSE;

		if( pMover->m_dwMotion == MTI_STAND || pModel->IsEndFrame()	)
			bEnd = TRUE;
		if( timeGetTime() >= m_tmTimeOver + SEC(15) )
			bEnd = TRUE;
		if( bEnd )
		{
			m_nEvent = EPT_MOVING;
			m_tmReattack = timeGetTime();
			m_tmAddReattack = 2000;
			m_idTarget = NULL_ID;
		}
	} else

	if( m_nEvent == EPT_TRACKING )
	{
		if( timeGetTime() >= m_tmTrace + 3000 )
		{
			m_nEvent = EPT_MOVING;
			m_idTarget = NULL;
			m_vTarget.x = m_vTarget.y = m_vTarget.z = 0;
		}
	} else

	{

		if( timeGetTime() >= m_tmReattack + (2000 - m_tmAddReattack) )
		{
			FLOAT fRadius = pMover->GetRadiusXZ();
			FLOAT fRadiusSq = fRadius * fRadius;

			if( m_idTarget == NULL_ID &&
				(m_vTarget.x == 0 && m_vTarget.y == 0 && m_vTarget.z == 0) )
			{

				if( SelectTarget() == FALSE )
					return FALSE;
			}

			D3DXVECTOR3	vTarget;
			FLOAT	fDistSq = 0;
			CMover *pTarget = NULL;
			if( m_idTarget != NULL_ID )
			{
				pTarget = prj.GetMover( m_idTarget );
				if( IsValidObj(pTarget) )
					vTarget = pTarget->GetPos();
				else
				{
					m_idTarget = NULL_ID;
					return FALSE;
				}
			} else
			if( m_vTarget.x && m_vTarget.y && m_vTarget.z )
			{
				vTarget = m_vTarget;
			} else
			{

				return FALSE;
			}

			D3DXVECTOR3 vDist = vTarget - pMover->GetPos();
			fDistSq = D3DXVec3LengthSq( &vDist );
			FLOAT fArrivalRange = fRadius;

			if( fDistSq < fRadiusSq * 1.5f )
			{
				m_nAttackType = CAT_QUAKE;
				fArrivalRange = 0;
			} else
			if( fDistSq < fRadiusSq * 3.0f )
			{
				if( xRandom(100) < 50 )
					m_nAttackType = CAT_SLASH;
				else
					m_nAttackType = CAT_LASER;
				fArrivalRange = fRadius;

			} else
			if( fDistSq < fRadiusSq * 6.0f )
			{

				m_nAttackType = CAT_SHOOT;
				fArrivalRange = 30.0f;
			} else
			{

				m_idTarget = NULL_ID;
				return FALSE;
			}




			m_tmTrace = timeGetTime();
			m_nEvent = EPT_TRACKING;
			pMover->CMD_SetMeleeAttack( m_idTarget, fArrivalRange );

#ifdef __INTERNALSERVER
			{
				CString strType;
				switch( m_nAttackType )
				{
				case CAT_SLASH:		strType = "베기";	break;
				case CAT_SHOOT:		strType = "쏘기";	break;
				case CAT_LASER:		strType = "레이저";	break;
				case CAT_QUAKE:		strType = "밟기";	break;
				}
				CString str;
				str.Format( "AtkType:%s Dist:%5.2f", strType, sqrt(fDistSq) );
				DEBUGOUT2( str, "boss.txt" );
			}
#endif


		} else
		{

		}
	}

	return TRUE;
}




BOOL CAIClockWorks::StateRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS )
		MoveProcessRage();

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE )

		m_idLastAttacker = msg.dwParam1;

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE )
		SendAIMsg( AIMSG_EXIT );

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )
	{
		OBJMSG	dwMsg = OBJMSG_NONE;
		DWORD	dwItemID = 0;
		MoverProp	*pProp = pMover->GetProp();

		switch( m_nAttackType )
		{
		case CAT_SLASH:		dwMsg = OBJMSG_ATK1;	dwItemID = pProp->dwAtk1;	break;
		case CAT_LASER:		dwMsg = OBJMSG_ATK3;	dwItemID = pProp->dwAtk2;	break;
		case CAT_SHOOT:		dwMsg = OBJMSG_ATK2;	dwItemID = pProp->dwAtk3;	break;
		case CAT_QUAKE:		dwMsg = OBJMSG_ATK4;	dwItemID = pProp->dwAtk4;	break;
		default:
			Error( "CAIClockWorks::StateRage - AIMSG_ARRIVAL m_nAttackType=%d", m_nAttackType );
		}

		if( dwMsg )
		{
			if( m_idTarget != NULL_ID )
			{
				if( pMover->DoAttackMelee( m_idTarget, dwMsg, dwItemID ) == 1 )
				{
					m_nEvent = EPT_ATTACKING;
					m_tmTimeOver = timeGetTime();
				} else
					m_nEvent = EPT_MOVING;



			}
		} else
			m_nEvent = EPT_MOVING;
	}



	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ATTACK_MELEE )
		switch( m_nAttackType )
		{
		case CAT_SLASH:



			break;
		case CAT_LASER:

			{


			}
			break;
		case CAT_SHOOT:

			break;
		case CAT_QUAKE:

			break;
		default:
			Error( "CAIClockWorks::StateRage : AIMSG_ATTACK_MELEE %d", m_nAttackType );
			break;
		}

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_END_APPEAR )
		m_nEvent = EPT_MOVING;

	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_END_MELEEATTACK )
		m_nEvent = EPT_MOVING;
		m_tmReattack = timeGetTime();
		m_tmAddReattack = xRandom( 2000 );
		m_idTarget = NULL_ID;


	/////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )

	EndAIHandler( )
	return TRUE;
}





