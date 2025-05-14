#include "stdafx.h"
#include "defineText.h"
#include "User.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#include "WorldMng.h"

extern	CUserMng	g_UserMng;

#ifndef __MEM_TRACE
#ifdef _DEBUG
#define new new( __FILE__, __LINE__ )
#endif
#endif	// __MEM_TRACE

#include "UserMacro.h"

extern	CDPSrvr		g_DPSrvr;
extern	CWorldMng	g_WorldMng;

void CUser::AddEscape( DWORD tmMaxEscape )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_DO_ESCAPE;
	m_Snapshot.ar << tmMaxEscape;
}


void CUser::AddSetActionPoint( int nAP )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETACTIONPOINT;
	m_Snapshot.ar << nAP;
}

void CUser::AddSetTarget( OBJID idTarget )
{
	if( IsDelete() )	return;

	m_Snapshot.cb++;
	m_Snapshot.ar << NULL_ID;
	m_Snapshot.ar << SNAPSHOTTYPE_SETTARGET;
	m_Snapshot.ar << idTarget;
}

// ----------------------------------------------------------------------------------

//   CUserMng Lux

// ----------------------------------------------------------------------------------
/*
void	CUserMng::AddDoCollect( CUser* pUser, OBJID idTarget )
{
	CAr ar;
	ar << GETID( pUser ) << SNAPSHOTTYPE_DO_COLLECT;
	ar << idTarget;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pUser )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pUser )
}
*/

void	CUserMng::AddCreateSkillEffect( CMover *pAttacker, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	CAr ar;
	ar << GETID( pAttacker ) << SNAPSHOTTYPE_ACTIVESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pAttacker )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pAttacker )
}


void	CUserMng::AddSetStun( CMover *pMover, BOOL bApply)
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SETSTUN;
	ar << bApply;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void	CUserMng::AddDoApplySkill( CCtrl *pCtrl, OBJID idTarget, DWORD dwSkill, DWORD dwLevel )
{
	CAr ar;
	ar << GETID( pCtrl ) << SNAPSHOTTYPE_DOAPPLYUSESKILL;
	ar << idTarget << dwSkill << dwLevel;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}


void	CUserMng::AddSendActMsg( CMover *pMover, OBJMSG dwMsg, int nParam1, int nParam2, int nParam3 )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_SENDACTMSG;
	ar << (int)dwMsg;
	ar << pMover->IsFly();
	ar << nParam1 << nParam2 << nParam3;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void	CUserMng::AddPushPower( CMover *pMover, D3DXVECTOR3 vPos, FLOAT fPushAngle, FLOAT fPower )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_PUSHPOWER;
	ar << vPos;
	ar << fPushAngle << fPower;
	GETBLOCK( ar, lpBuf, nBufSize );

	if (pMover->IsPlayer() == TRUE)
	{
		static_cast<CUser*>(pMover)->AddBlock(lpBuf, nBufSize);
	}

	FOR_VISIBILITYRANGE(pMover)
		if (pMover->m_idPlayer != USERPTR->m_idPlayer)
		{
			USERPTR->AddBlock(lpBuf, nBufSize);
		}
	NEXT_VISIBILITYRANGE(pMover)
}

void	CUserMng::AddRemoveSkillInfluence( CMover *pMover, WORD wType, WORD wID )
{
	CAr ar;
	ar << GETID( pMover ) << SNAPSHOTTYPE_REMOVESKILLINFULENCE;
	ar << wType;
	ar << wID;
	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pMover )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pMover )
}

void CUserMng::AddSetPosAngle( CCtrl* pCtrl, const D3DXVECTOR3 &vPos, FLOAT fAngle )
{
	CAr ar;

	ar << GETID( pCtrl ) << SNAPSHOTTYPE_SETPOSANGLE;
	ar << vPos << fAngle;

	GETBLOCK( ar, lpBuf, nBufSize );

	FOR_VISIBILITYRANGE( pCtrl )
		USERPTR->AddBlock( lpBuf, nBufSize );
	NEXT_VISIBILITYRANGE( pCtrl )
}
