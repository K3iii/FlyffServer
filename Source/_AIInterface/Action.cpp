// Billboard.cpp : implementation of the CBeastView class


#include "stdafx.h"
#include "Action.h"


CAction::CAction()
{
	Init();
}
CAction::CAction( CMover* pMover )
{
	Init();
	m_pMover = pMover;
}

CAction::~CAction()
{
}

void CAction::Init( void )
{
	m_pMover		= NULL;
	m_dwState		= 0;
	m_dwStateFlag	= 0;
}

void	CAction::AddStateFlag( DWORD dwFlag )
{
	m_dwStateFlag |= dwFlag;
}

void	CAction::RemoveStateFlag( DWORD dwFlag )
{
	m_dwStateFlag &= (~dwFlag);
}

void CAction::ProcessAtkMsg( void )
{
	if( !m_qMeleeAtkMsg.Process( this ) )
		m_qMagicAtkMsg.Process( GetMover() );
}


CWorld* CAction::GetWorld( )
{
	return m_pMover->GetWorld();
}


void	CAction::OnEndMoveState( void )
{

}

void	CAction::OnEndTurnState( void )
{

}

void	CAction::OnEndLookState( void )
{

}

void	CAction::OnEndJumpState( DWORD dwState )
{

	switch( dwState & OBJSTA_JUMP_ALL )
	{
	case OBJSTA_SJUMP4:
		m_pMover->OnActEndJump4();
		break;
	}
}

void	CAction::OnEndAttackState( DWORD dwState )
{

	switch( dwState & OBJSTA_ATK_ALL )
	{
	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
	case OBJSTA_ATK_MAGIC1:
	case OBJSTA_ATK_RANGE1:
	case OBJSTA_RANGE3:
	case OBJSTA_RANGE4:
#ifdef __WORLDSERVER
		if( m_pMover->IsPlayer() && m_pMover->IsFly() )
			return;

		if( !IsAtkMsgEmpty() )
			ProcessAtkMsg();
#else // worldserver
		if( m_pMover->IsActiveMover() == FALSE )
		{
			if( !IsAtkMsgEmpty() )
				ProcessAtkMsg();
		}
#endif // not worldserver
		break;
	case OBJSTA_ATK_MELEESKILL:
		m_pMover->OnActEndMeleeSkill();
		break;
	case OBJSTA_ATK_MAGICSKILL:
		m_pMover->OnActEndMagicSkill();
		break;
	}
}

void	CAction::OnEndDamageState( void )
{

}

void	CAction::OnEndActionState( void )
{

}

void	CAction::ClearState( void )
{
	DWORD dwState = m_dwState;

//	ResetState( OBJSTA_MOVE_ALL | OBJSTA_TURN_ALL | OBJSTA_LOOK_ALL | OBJSTA_JUMP_ALL | OBJSTA_ATK_ALL | OBJSTA_DMG_ALL );
	ResetState( OBJSTA_ALL );

	if( m_dwState )
	{
		Error( "CAction::ClearState : 아직도 뭔가 클리어되지 않은 값이 있다. %08x, %08x", dwState, m_dwState );
		m_dwState = 0;
	}
}



void	CAction::ResetState( DWORD dwState )
{
	DWORD dw	= m_dwState;
	if( dwState & OBJSTA_MOVE_ALL )
	{
		OnEndMoveState();
		m_dwState &= (~dwState);
	}

	if( dwState & OBJSTA_TURN_ALL )
	{
		OnEndTurnState();
		m_dwState &= (~dwState);
	}

	if( dwState & OBJSTA_LOOK_ALL )
	{
		OnEndLookState();
		m_dwState &= (~dwState);
	}

	if( dwState & OBJSTA_JUMP_ALL )
	{
		m_dwState &= (~dwState);
		OnEndJumpState( dw );
	}

	if( dwState &	OBJSTA_ATK_ALL )
	{
		m_dwState &= (~dwState);
		OnEndAttackState(dw);
	}

	if( dwState &	OBJSTA_DMG_ALL )
	{
		OnEndDamageState();
		m_dwState &= (~dwState);
	}

	if( dwState & OBJSTA_ACTION_ALL )
	{
		OnEndActionState();
		m_dwState &= (~dwState);
	}
}