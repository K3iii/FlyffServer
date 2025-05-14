#include "stdafx.h"
#include "defineText.h"
#include "defineItem.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "dpcoreclient.h"
#include "dpdatabaseclient.h"
#include "dpsrvr.h"
#include "user.h"
#include "worldmng.h"
#include "misc.h"

#include "guild.h"
extern	CGuildMng	g_GuildMng;


#include "..\_Common\Ship.h"


#include "..\_aiinterface\AIPet.h"

#include "Party.h"
extern	CPartyMng			g_PartyMng;
extern	CUserMng			g_UserMng;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPCoreClient		g_DPCoreClient;
extern	CWorldMng			g_WorldMng;

#include "..\_Network\ErrorCode.h"


void CDPSrvr::OnUseSkill(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	WORD wType = 0;
	WORD wId = 0;
	OBJID objid = 0;
	int	 nUseType = 0;
	BOOL bControl = FALSE;

	ar >> wType >> wId >> objid >> nUseType >> bControl;
	wType = 0;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj((CObj*)pUser))
		return;

	if (pUser->IsFly()
		|| pUser->IsDie()
		|| pUser->m_vtInfo.VendorIsVendor()
		|| !pUser->m_bAllAction
		)
	{
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USESKILL);
		return;
	}

	const int nIdx = wId;
	if (nIdx < 0 || nIdx >= MAX_SKILL_JOB)
	{
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USESKILL);
		return;
	}

	LPSKILL pSkill = pUser->GetSkill(wType, nIdx);
	if (!pSkill)
		return;

	if (pSkill->dwSkill == DWORD(-1))
		return;

	ItemProp* pSkillProp = prj.GetSkillProp(pSkill->dwSkill);
	if (!pSkillProp)
		return;

	DWORD dwLevel = pSkill->dwLevel;
	if (dwLevel == 0 || dwLevel > pSkillProp->dwExpertMax)
	{
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USESKILL);
		return;
	}

	BOOL bSuccess = pUser->DoUseSkill(wType, nIdx, objid, (SKILLUSETYPE)nUseType, bControl);
	if (bSuccess)
	{
		if (nUseType == SUT_QUEUESTART)
			pUser->m_playTaskBar.m_nUsedSkillQueue = 0;
	}
	else
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USESKILL);
}

void CDPSrvr::OnUseObject(CAr& ar, DPID dpidCache, DPID dpidUser, LPBYTE lpBuf, u_long uBufSize)
{
	OBJID objid;
	ar >> objid;

	CUser* pUser = g_UserMng.GetUser(dpidCache, dpidUser);
	if (!IsValidObj((CObj*)pUser))
		return;

	if (pUser->IsFly()
		|| pUser->IsDie()
		|| pUser->m_vtInfo.VendorIsVendor()
		|| !pUser->m_bAllAction
		|| pUser->m_pActMover->IsActAttack()
		)
	{
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USEOBJECT);
		return;
	}

	BOOL bSuccess = pUser->DoUseObject(objid);
	if (!bSuccess)
		pUser->AddHdr(NULL_ID, SNAPSHOTTYPE_CLEAR_USEOBJECT);
}