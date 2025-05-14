#include "StdAfx.h"
#include "Synchronizer.h"
#include "Mover.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#else
#include "User.h"
extern	CUserMng		g_UserMng;
#endif


AutoSynchronizer::AutoSynchronizer( CMover* pMover )
:	m_pMover( pMover )
,	m_vSyncPos( 0.0f, 0.0f, 0.0f )
,	m_fSyncAngle( 0.0f )
,	m_fSyncAngleX( 0.0f )
,	null_vec3( 0.0f, 0.0f, 0.0f )
,	m_collisionSynchronizeTime( 0 )
{
}

AutoSynchronizer::~AutoSynchronizer()
{
}

void AutoSynchronizer::Initialize()
{
	m_vSyncPos		= m_pMover->GetPos();
	m_fSyncAngle	= m_pMover->GetAngle();
	m_fSyncAngleX	= m_pMover->GetAngleX();
#ifdef __CLIENT
	UpdateGhost();
#endif
}

void AutoSynchronizer::Serialize( CAr & ar )
{
	if( ar.IsStoring() )	
	{
		ar << m_pMover->GetPos() << m_pMover->GetAngle() << m_pMover->GetAngleX() << m_pMover->m_pActMover->m_vDelta;
		ar << m_pMover->m_pActMover->m_fTurnAngle; // Flying only, todo
		ar << m_pMover->m_dwMotion << m_pMover->m_dwMotionOption << m_pMover->m_pActMover->m_nMotionEx << m_pMover->m_pModel->m_nLoop;
		ar << m_pMover->m_pActMover->GetState() << m_pMover->m_pActMover->GetStateFlag();
		ar << static_cast<bool>( m_pMover->IsEmptyDest() == TRUE );
	}
	else	
	{
		ar >> vSyncPos >> fAngle >> fAngleX >> vDelta;
		ar >> fTurnAngle; // Flying only, todo
		ar >> dwMotion >> dwMotionOption >> nMotionEx >> nLoop;
		ar >> dwState >> dwStateFlag;
		ar >> bEmptyDest;
		Synchronize();
	}
}

#ifdef __WORLDSERVER
AutoSynchronizer::Check_Result AutoSynchronizer::CheckValidSynchronize()
{
	if( ( m_pMover->IsFly() == TRUE ) != ( ( dwStateFlag & OBJSTAF_FLY ) == OBJSTAF_FLY ) )
		return invalid;
	
	D3DXVECTOR3 vDistance;
	float fDistance;
	float fLimitDistance;
	const float fAllowedFrame = 60.0f;

	if (m_pMover->IsFly())
	{
		vDistance = m_pMover->GetPos() - vSyncPos;
		fDistance = ::D3DXVec3LengthSq(&vDistance);
		fLimitDistance = ::pow((500000.0f / (60.0f * 60.0f * 60.0f)) * fAllowedFrame, 2);
	}
	else
	{
		// X/Z position based on speed, Y dont, so firstly compare it without Y
		vDistance = D3DXVECTOR3(m_pMover->GetPos().x, 0.0f, m_pMover->GetPos().z) - D3DXVECTOR3(vSyncPos.x, 0.0f, vSyncPos.z);
		fDistance = ::D3DXVec3LengthSq(&vDistance);
		fLimitDistance = ::pow(m_pMover->GetSpeed(m_pMover->m_pActMover->m_fSpeed) * fAllowedFrame, 2);

		/*if (fDistance > fLimitDistance)
			return replace;

		// Now check Y position difference
		vDistance = D3DXVECTOR3(0.0f, m_pMover->GetPos().y, 0.0f) - D3DXVECTOR3(0.0f, vSyncPos.y, 0.0f);
		fDistance = ::D3DXVec3LengthSq(&vDistance);
		fLimitDistance = 5.0f;*/
	}

	return fDistance <= fLimitDistance ? valid : replace;
}
#endif // __WORLDSERVER

void AutoSynchronizer::SendSynchronize()
{
#ifdef __WORLDSERVER
	g_UserMng.AddMoverBehavior( m_pMover );
#endif // __WORLDSERVER
#ifdef __CLIENT
	g_DPlay.SendPlayerBehavior();
#endif // __CLIENT
}

void AutoSynchronizer::CollisionState( BOOL bCollision )
{
#ifdef __WORLDSERVER
	if( m_pMover->IsPlayer() == TRUE )
		return;
#endif // __WORLDSERVER
#ifdef __CLIENT
	if( m_pMover->IsActiveMover() == FALSE )
		return;
#endif // __CLIENT
	if( bCollision )	
	{
		const DWORD	collisionSynchronizeInterval = SEC( 1 );
		if( m_collisionSynchronizeTime <= GetTickCount() - collisionSynchronizeInterval )	
		{
			SendSynchronize();
			m_collisionSynchronizeTime = GetTickCount();
		}
	}
	else	
	{
		if( m_collisionSynchronizeTime != 0 )
			SendSynchronize();
		m_collisionSynchronizeTime = 0;
	}
}

void AutoSynchronizer::Synchronize()
{
	if( ::IsValidObj( m_pMover ) == FALSE || m_pMover->m_pActMover == NULL )
		return;

#ifdef __WORLDSERVER
	switch( CheckValidSynchronize() )	
	{
		case valid:
#endif // __WORLDSERVER
			m_pMover->SetPos( vSyncPos );
			m_pMover->SetAngle( fAngle );
			m_pMover->SetAngleX( fAngleX );
			m_pMover->m_pActMover->m_vDelta = vDelta;
			m_pMover->m_pActMover->m_fTurnAngle = fTurnAngle; // Flying only, todo

			m_pMover->m_pActMover->__ForceSetState( dwState );	
			m_pMover->m_pActMover->ClearStateFlag();
			m_pMover->m_pActMover->AddStateFlag( dwStateFlag );
			m_pMover->m_pActMover->m_nMotionEx = nMotionEx;
			m_pMover->SetMotion( dwMotion, nLoop, dwMotionOption );
			if( bEmptyDest )	
			{
#ifdef __WORLDSERVER
				if( m_pMover->GetCmd() == OBJACT_USESKILL )
					m_pMover->ProcessMoveArrivalBySynchronizer();									
#endif // __WORLDSERVER
				m_pMover->ClearDest();
			}
#ifdef __WORLDSERVER
			SendSynchronize();
			break;
		case replace:
			m_pMover->REPLACE( g_uIdofMulti, m_pMover->GetWorld()->GetID(), m_pMover->GetPos(), REPLACE_FORCE, m_pMover->GetLayer() );
			break;
		case invalid:
		default:
			break;
	}
#endif // __WORLDSERVER
}

#ifdef __CLIENT
void AutoSynchronizer::UpdateGhostPos()
{
	if( m_pMover->IsPlayer() )	
	{
	//	if( m_pMover->m_pActMover->IsActAttack() && m_pMover->m_pActMover->m_vDeltaE == null_vec3 )	
	//		return;
	}
	else	
	{
		if( m_pMover->GetProp() && m_pMover->GetProp()->dwAI == AII_PET && !m_pMover->m_pActMover->IsMove() )	
			return;
	}
				
	D3DXVECTOR3 vPos = m_pMover->GetPos();
	vPos.y += 0.07f;
	
	const float criteria = 0.2f * 0.2f;	
	const float distance = ::D3DXVec3LengthSq( &D3DXVECTOR3( vPos - m_vSyncPos ) );
	const bool bGhostUpdate =	(	!m_pMover->IsActiveMover() 
								&&	( m_pMover->m_pActMover->IsMove() || distance > criteria )
								&&	m_vSyncPos != null_vec3
								);
								
	if( bGhostUpdate )	
	{
		const float rate = m_pMover->IsFly() ? 0.05f : 0.2f;		
		::D3DXVec3Lerp( &m_vSyncPos, &m_vSyncPos, &vPos, rate );	
	}
	else	
		m_vSyncPos	= vPos;

	D3DXMATRIX matTrans;
	::D3DXMatrixTranslation( &matTrans, m_vSyncPos.x, m_vSyncPos.y, m_vSyncPos.z );
	m_pMover->SetMatrixTrans( matTrans );
	m_pMover->SetUpdateMatrix( TRUE );
}

void AutoSynchronizer::UpdateGhostAngle()
{
	const float fAngle = m_pMover->GetAngle();
	const float fAngleX = m_pMover->GetAngleX();
	D3DXMATRIX	matRotation = m_pMover->GetMatrixRotation();
	if( m_pMover->IsActiveMover() )	
	{
		if( m_pMover->IsFly() )
			m_fSyncAngle = fAngle;
		m_fSyncAngleX = fAngleX;
	}

	const float rate = m_pMover->IsFly() ? 16.0f : 4.0f;
	// Angle
	if( ::fabs( fAngle - m_fSyncAngle ) > 0.001f ) 	
	{
		float d = fAngle - m_fSyncAngle;
		if( ::fabs( d ) < 4.0f )
			m_fSyncAngle = fAngle;
		if( d < -180.0f )
			d += 360.0f;
		else if( d > 180.0f )
			d -= 360;

		m_fSyncAngle += d / rate;

		if( m_fSyncAngle > 360.0f )
			m_fSyncAngle -= 360.0f;
		else if( m_fSyncAngle < 0.0f )
			m_fSyncAngle += 360.0f;
	}

	// AngleX
	if( fabs( fAngleX - m_fSyncAngleX ) > 0.001f )	
	{
		float d = fAngleX - m_fSyncAngleX;
		if( fabs( d ) < 4.0f )
			m_fSyncAngleX = fAngleX;
		m_fSyncAngleX += d / rate;

		if( m_fSyncAngleX > 45.0f )
			m_fSyncAngleX = 45.0f;
		else if( m_fSyncAngleX < -45.0f )
			m_fSyncAngleX = -45.0f;
	}

	::D3DXMatrixRotationY( &matRotation, D3DXToRadian( -m_fSyncAngle ) );
	D3DXMATRIX matRotX;
	::D3DXMatrixRotationX( &matRotX, D3DXToRadian( m_fSyncAngleX ) );
	matRotation *= matRotX;
	::D3DXMatrixRotationYawPitchRoll( &matRotation, D3DXToRadian( -m_fSyncAngle ), D3DXToRadian( -m_fSyncAngleX ), 0.0f );
	m_pMover->SetMatrixRotation( matRotation );
	m_pMover->SetUpdateMatrix( TRUE );
}

void AutoSynchronizer::UpdateGhost()
{
	UpdateGhostPos();
	UpdateGhostAngle();
}
#endif