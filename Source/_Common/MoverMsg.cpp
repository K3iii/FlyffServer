#include "stdafx.h"
#include "defineSkill.h"
#include "defineText.h"
#ifdef __CLIENT
#include "AppDefine.h"
#endif

#include "ModelObject.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#else	// __CLIENT
#ifdef __WORLDSERVER
#include "User.h"
#include "WorldMng.h"
#include "DPSrvr.h"
#include "DPCoreClient.h"
extern	CUserMng	g_UserMng;
extern	CWorldMng	g_WorldMng;
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
#endif	// __WORLDSERVER
#endif	// __CLIENT

#include "ship.h"

void	CMover::SetCmd(OBJACT cmd, int nParam1, int nParam2, int nParam3)
{
	m_oaCmd = cmd;
	m_nCParam[0] = nParam1;
	m_nCParam[1] = nParam2;
	m_nCParam[2] = nParam3;
}

void CMover::SetDestObj(OBJID objid, float fRange, BOOL bSend)
{
#ifndef __WORLDSERVER
	if (bSend && IsActiveMover() && (m_idDest != objid || m_fArrivalRange != fRange))
	{
		CMover* pTarget = prj.GetMover(m_idDest);
		if (pTarget)
		{
			if (pTarget->IsPlayer())
				g_DPlay.SendPlayerBehavior();
		}
		g_DPlay.SendPlayerDestObj(objid, fRange);
	}
#endif	// not __WORLDSERVER

#ifdef __WORLDSERVER
	if (bSend)
	{
		if (m_idDest != objid || m_fArrivalRange != fRange)
			g_UserMng.AddMoverSetDestObj(this, objid, fRange, FALSE);
	}
#endif	// __WORLDSERVER

	m_idDest = objid;
	m_fArrivalRange = fRange;
	ClearDestPos();

#ifdef __CLIENT
	m_oaCmd = OBJACT_NONE;
#endif // Client
}
void CMover::SetDestPos(const D3DXVECTOR3& vDestPos, bool bForward, BOOL fTransfer)
{
#ifndef __CLIENT
	UNREFERENCED_PARAMETER(fTransfer);
#endif

	m_vDestPos = vDestPos;

#ifdef __CLIENT	
	m_bForward = true;
#else //__CLIENT
	m_bForward = bForward;
#endif //__CLIENT

	D3DXVECTOR3	vDir = GetPos() - m_vDestPos;

	m_bPositiveX = (vDir.x > 0.0f);
	m_bPositiveZ = (vDir.z > 0.0f);

	ClearDestObj();

#ifdef __CLIENT
	if (IsActiveMover() && fTransfer)
		g_DPlay.PutPlayerDestPos(vDestPos, bForward);
#endif
}

FLOAT	CMover::GetAttackRange(DWORD dwAttackRange)
{
	float fAttRange = 0.0f;
	switch (dwAttackRange)
	{
	case AR_SHORT:	fAttRange = 2.0f;	break;
	case AR_LONG:	fAttRange = 3.0f;		break;
	case AR_FAR:	fAttRange = 4.0f;		break;
	case AR_RANGE:	fAttRange = 10.0f;		break;
	case AR_WAND:	fAttRange = 15.0f;		break;
	case AR_HRANGE: fAttRange = 6.0f;	break;
	case AR_HWAND:	fAttRange = 18.0f;		break;
	default:		fAttRange = 0.0f;		break;
	}

	ItemProp* pProp = GetActiveHandItemProp();
	if (pProp && pProp->dwItemKind3 == IK3_BOW)
	{
		int  nTmpATR = GetParam(DST_HAWKEYE_RATE, 0);
		if (nTmpATR > 0)
		{
			nTmpATR += 100;
			fAttRange = fAttRange * nTmpATR / 100;
		}
	}

	return fAttRange;
}


BOOL CMover::IsPossibleSetCmd(BOOL bControl, BOOL bCheckTaskbar)
{
	if (!m_pActMover)
		return FALSE;
	if (IsDie())
		return FALSE;
	if (m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL)
		return FALSE;
	if (m_dwMode & DONMOVE_MODE)
		return FALSE;
	if (GetAdjParam(DST_CHRSTATE) & CHS_SETSTONE)
		return FALSE;
	if (m_vtInfo.VendorIsVendor())
		return FALSE;
#ifdef __CLIENT
	if (!g_WndMng.m_bAllAction)
		return FALSE;
	if (g_WndMng.m_pLogOutWaitting)
		return FALSE;
	if (m_dwReqFlag & REQ_USESKILL)
		return FALSE;
	if (m_dwReqFlag & REQ_USEOBJECT)
		return FALSE;

	if (bCheckTaskbar)
	{
		if (g_WndMng.m_pWndTaskBar && g_WndMng.m_pWndTaskBar->m_nExecute)
			return FALSE;
	}
#endif

	if (!bControl)
	{
		if (m_pActMover->IsFly())
			return FALSE;

		if (m_pActMover->IsActJump())
			return FALSE;

		if (m_pActMover->IsActAttack())
			return FALSE;
	}


	return TRUE;
}

int	CMover::CMD_SetUseSkill(OBJID idTarget, int nSkillIdx, SKILLUSETYPE sutType)
{
	m_oaCmd = OBJACT_NONE;

	if (!IsPossibleSetCmd(FALSE, sutType == SUT_NORMAL))
		return 0;

	LPSKILL pSkill = GetSkill(0, nSkillIdx);
	if (!pSkill)
		return 0;

	ItemProp* pSkillProp = pSkill->GetProp();
	if (!pSkillProp)
		return 0;

	if (IsPlayer() && IsStateMode(STATE_BASEMOTION_MODE))
	{
#ifdef __CLIENT
		g_DPlay.SendStateModeCancel(STATE_BASEMOTION_MODE, STATEMODE_BASEMOTION_CANCEL);
#endif
		return 0;
	}

	float	fArrivalRange = 0.0f;
	fArrivalRange = GetAttackRange(pSkillProp->dwAttackRange);

	switch (pSkillProp->dwUseChance)
	{
	case WUI_NOW:
		idTarget = GetId();
		break;
	case WUI_TARGETINGOBJ:
	{
#ifdef __CLIENT
		CObj* pFocusObj = GetWorld()->GetObjFocus();
		if (pFocusObj && pFocusObj->GetType() == OT_MOVER)
			idTarget = ((CMover*)pFocusObj)->GetId();
#else
		if (IsPlayer())
			idTarget = ((CUser*)this)->m_idSetTarget;
#endif // __CLIENT
	}

	break;
#ifdef __CLIENT
	case WUI_TARGETCURSORPTZ:
	{
		idTarget = GetId();

		CRect rect;
		D3DXVECTOR3 vPos;
		CWndWorld* pWndWorld;

		pWndWorld = (CWndWorld*)g_WndMng.GetWndBase(APP_WORLD);

		rect = pWndWorld->GetClientRect();

		if (GetWorld()->ClientPointToVector(NULL, rect, pWndWorld->GetMousePoint(), &GetWorld()->m_matProj, &GetWorld()->GetCamera()->m_matView, &vPos, TRUE))
		{
#ifdef __SKILL0517
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, GetSkillLevel(pSkill));
#else	// __SKILL0517
			AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, pSkill->dwLevel);
#endif	// __SKILL0517
			if (!pAddSkillProp)
				return 0;

			FLOAT	fDist;
			FLOAT	fMaxDistSq;
			D3DXVECTOR3	vDist;
			fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
			fMaxDistSq *= fMaxDistSq;
			vDist = vPos - GetPos();
			fDist = D3DXVec3LengthSq(&vDist);

			SetAngle(GetDegree(vPos, GetPos()));


			if (fDist > fMaxDistSq)
			{
				FLOAT fLength;
				D3DXVECTOR3 vDirNor;
				D3DXVec3Normalize(&vDirNor, &vDist);
				fLength = (float)pAddSkillProp->dwSkillRange;
				float y = vPos.y;
				vPos = GetPos() + (vDirNor * fLength);
				vPos.y = y;

				int nAttr = GetWorld()->GetHeightAttribute(vPos.x, vPos.z);

				if (nAttr != HATTR_NONE)
				{
					while (nAttr != HATTR_NONE)
					{
						if (nAttr == HATTR_NOFLY)
							break;

						fLength -= 1.0f;
						vPos = GetPos() + (vDirNor * fLength);
						nAttr = GetWorld()->GetHeightAttribute(vPos.x, vPos.z);



						D3DXVECTOR3 vTemp = vPos - GetPos();
						float fTemp = D3DXVec3LengthSq(&vTemp);
						if (fTemp > fMaxDistSq)
						{
							vPos = GetPos();
							break;
						}
					}

					fLength -= 1.0f;
					vPos = GetPos() + (vDirNor * fLength);

					nAttr = GetWorld()->GetHeightAttribute(vPos.x, vPos.z);
					if (nAttr != HATTR_NONE)
					{
						vPos = GetPos();
					}
				}
			}
			else
			{
				int nAttr = GetWorld()->GetHeightAttribute(vPos.x, vPos.z);

				FLOAT fLength;
				D3DXVECTOR3 vDirNor;
				D3DXVec3Normalize(&vDirNor, &vDist);
				fLength = 0.0f;

				while (nAttr != HATTR_NONE)
				{
					if (nAttr == HATTR_NOFLY)
						break;

					fLength -= 1.0f;
					vPos = GetPos() + (vDirNor * fLength);

					nAttr = GetWorld()->GetHeightAttribute(vPos.x, vPos.z);

					D3DXVECTOR3 vTemp = vPos - GetPos();
					float fTemp = D3DXVec3LengthSq(&vTemp);
					if (fTemp > fMaxDistSq)
					{
						vPos = GetPos();
						break;
					}
				}
			}
			pWndWorld->m_vTelePos = vPos;
		}
		else
		{
			g_WndMng.m_pWndWorld->SetNextSkill(NEXTSKILL_NONE);
			g_WndMng.PutString(prj.GetText(TID_GAME_NOMOVING), NULL, prj.GetTextColor(TID_GAME_NOMOVING));
			return 0;
		}
	}
	break;
#endif // __CLIENT
	}


	if (idTarget != GetId())
	{
		CMover* pTarget = prj.GetMover(idTarget);
		if (IsValidObj(pTarget))
		{
			if (pSkillProp->nEvildoing < 0)
				if (IsAttackAble(pTarget) == FALSE)
					return 0;

			if (pSkill->dwSkill == SI_ASS_HEAL_RESURRECTION)
			{
				if (pTarget->IsNPC() || pTarget->IsDie() == FALSE)
					return 0;
			}
			else
			{
				if (pTarget->IsDie())
					return 0;
			}
		}
	}


	bool bSetDestObj = false;
	switch (pSkillProp->dwExeTarget)
	{
	case EXT_SELFCHGPARAMET:
		idTarget = GetId();
		bSetDestObj = true;
		break;
	case EXT_MAGICATKSHOT:
	case EXT_MAGICATK:
	case EXT_MAGICSHOT:
		if (idTarget == NULL_ID)	return 0;
		bSetDestObj = true;
		break;
	case EXT_ANOTHERWITH:
		if (idTarget == NULL_ID)
			idTarget = GetId();
		bSetDestObj = true;
		break;
	case EXT_AROUNDATK:
		idTarget = GetId();
		bSetDestObj = true;
		break;
	case EXT_OBJCHGPARAMET:
	default:
		if (idTarget == NULL_ID)
			return 0;
		bSetDestObj = true;
		break;
	}

	if (bSetDestObj)
	{
#ifdef __WORLDSERVER
		SetDestObj(idTarget, fArrivalRange, FALSE);
		g_UserMng.AddMoverSetDestObj(this, idTarget, fArrivalRange, TRUE);
#else // __WORLDSERVER
		SetDestObj(idTarget, fArrivalRange, TRUE);
#endif // __WORLDSERVER
	}

	ClearActParam();
	SetCmd(OBJACT_USESKILL, nSkillIdx, idTarget, sutType);

	return 1;
}

class CWndWorld;

void	CMover::CMD_SetMeleeAttack(OBJID idTarget, FLOAT fRange)
{
	if (!IsPossibleSetCmd())
		return;

	CMover* pTarget = prj.GetMover(idTarget);
	if (!IsValidObj(pTarget))
		return;

	if (pTarget->IsDie() || pTarget->m_pActMover->IsFly())
		return;

	ItemProp* pProp = GetActiveHandItemProp();
	if (!pProp || pProp->dwItemKind2 == IK2_TOOLS)
		return;

	if (pProp->dwItemKind3 == IK3_YOYO)
		fRange = GetAttackRange(pProp->dwAttackRange);

#ifdef __CLIENT
	ClearActParam();
	SetDestObj(idTarget, fRange);
	SetCmd(OBJACT_MELEE_ATTACK, idTarget);
#else
	SetDestObj(idTarget, fRange, TRUE);
#endif
}

void	CMover::CMD_SetRangeAttack(OBJID idTarget, int nPower)
{
	if (!IsPossibleSetCmd())
		return;

	CMover* pTarget = prj.GetMover(idTarget);
	if (!IsValidObj(pTarget))
		return;

	if (pTarget->IsDie() || pTarget->m_pActMover->IsFly())
		return;

	ItemProp* pItemProp = GetActiveHandItemProp();
	if (!pItemProp || pItemProp->dwItemKind3 != IK3_BOW)
		return;

	if (!IsBullet(pItemProp))
		return;

	ClearActParam();
	SetDestObj(idTarget, GetAttackRange(pItemProp->dwAttackRange), TRUE);
	SetCmd(OBJACT_RANGE_ATTACK, idTarget, nPower, 0);
}

void	CMover::CMD_SetMagicAttack(OBJID idTarget, int nMagicPower)
{
	if (!IsPossibleSetCmd())
		return;

	CMover* pTarget = prj.GetMover(idTarget);
	if (!IsValidObj(pTarget))
		return;

	if (pTarget->IsDie() || pTarget->m_pActMover->IsFly())
		return;

	ItemProp* pItemProp = GetActiveHandItemProp();
	if (!pItemProp || pItemProp->dwItemKind3 != IK3_WAND)
		return;

	ClearActParam();
	SetDestObj(idTarget, GetAttackRange(pItemProp->dwAttackRange));
	SetCmd(OBJACT_MAGIC_ATTACK, idTarget, nMagicPower);
}

void	CMover::CMD_SetUseObject(CCtrl* pCtrl)
{
	if (!IsPossibleSetCmd())
		return;

	if (!IsValidObj((CObj*)pCtrl))
		return;

	ClearActParam();
	SetDestObj(pCtrl->GetId());
	SetCmd(OBJACT_USEOBJECT, pCtrl->GetId());
}

void	CMover::SetActParam(OBJACT act, int nParam1, int nParam2, int nParam3, int nParam4, int nParam5)
{
	m_oaAct = act;
	m_nAParam[0] = nParam1;
	m_nAParam[1] = nParam2;
	m_nAParam[2] = nParam3;
	m_nAParam[3] = nParam4;
	m_nAParam[4] = nParam5;
}
