#include "stdafx.h"
#include "CtrlObjs.h"
#include "defineSkill.h"
#include "user.h"
#include "dpsrvr.h"
#include "Party.h"
#include "dpDatabaseClient.h"


extern	CUserMng	g_UserMng;
extern	CDPSrvr		g_DPSrvr;
extern	CPartyMng	g_PartyMng;
extern	CDPDatabaseClient	g_dpDBClient;


void CContDamageCtrl::Init( void )
{
	m_tmStart = m_tmUpdate = timeGetTime();
	m_pSkillProp = NULL;
	m_pAddSkillProp = NULL;
	m_idTarget = m_idSrc = NULL_ID;
}

void CContDamageCtrl::Destroy( void )
{
	Init();
}

void CContDamageCtrl::ApplyDamage( void )
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;

	CCtrl *pSrc = prj.GetCtrl( m_idSrc );
	if( IsInvalidObj(pSrc) )
	{

		Delete();
		return;
	}

	CCtrl *pCenter = prj.GetCtrl( m_idTarget );
	D3DXVECTOR3 vPos = GetPos();
	int nApplyType;
	nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;
	pSrc->ApplySkillRegion(vPos, nApplyType, pSkillProp, pAddSkillProp, false, TRUE, pCenter, FALSE );
}


void CContDamageCtrl::Process()
{
	ItemProp *pSkillProp = m_pSkillProp;
	AddSkillProp *pAddSkillProp = m_pAddSkillProp;

	if( pSkillProp->tmContinuousPain == NULL_ID )
		Error( "%s의 SkillProp tmContinuousPain의 값이 -1", pSkillProp->dwID );

	if( (int)(g_tmCurrent - m_tmUpdate) > (int)pSkillProp->tmContinuousPain )
	{
		m_tmUpdate = g_tmCurrent;

		ApplyDamage();
	}

	if( (int)(g_tmCurrent - m_tmStart) > (int)m_pAddSkillProp->dwSkillTime )
		Delete();
}



