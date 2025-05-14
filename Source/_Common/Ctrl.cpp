// Ctrl.cpp: implementation of the CCtrl class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defineSkill.h"
#include "defineObj.h"
#include "defineText.h"
#include "ResData.h"
#ifdef __WORLDSERVER
#include "CtrlObjs.h"
#endif
#include "Ship.h"

#ifdef __CLIENT
#include "DPClient.h"
extern	CDPClient	g_DPlay;
#endif

#ifdef __WORLDSERVER
#ifdef __FL_FFA
#include "FFAManager.h"
#endif
#ifdef __FL_TDM
#include "TDMManager.h"
#endif
#include "user.h"
#include "dpcoreclient.h"
#include "dpsrvr.h"

#ifdef __WORLDSERVER
extern	CUserMng	g_UserMng;
extern	CDPCoreClient	g_DPCoreClient;
extern	CDPSrvr		g_DPSrvr;
#include "Party.h"
extern	CPartyMng	g_PartyMng;
#include "dpDatabaseClient.h"
extern	CDPDatabaseClient	g_dpDBClient;
#endif // Worldserver
#endif

#include "commonctrl.h"

extern map< string, DWORD > g_MapStrToObjId;

////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////

CCtrl::CCtrl()
{
	m_dwType = OT_CTRL;
	m_objid = NULL_ID;

#ifdef __WORLDSERVER
	m_lRespawn = -1;
	m_nRespawnType = 0;

	memset(m_nOldCenter, 0, sizeof(int) * MAX_LINKLEVEL);
#endif	// __WORLDSERVER

	D3DXMatrixIdentity(&m_mInvTM);
}

CCtrl::~CCtrl()
{
#ifdef __WORLDSERVER
	if (IsVirtual())
		return;

	RemoveItFromView();
#endif	// __WORLDSERVER

	RemoveItFromGlobalId();
}

void CCtrl::Process()
{
	CObj::Process();
#ifdef __WORLDSERVER
	ProcessDeleteRespawn();
#endif
}

void CCtrl::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	CObj::Render(pd3dDevice);
}

BOOL CCtrl::Read(CFileIO* pFile)
{
	return CObj::Read(pFile);
}


void CCtrl::AddItToGlobalId()
{
	prj.m_objmap.Add(this);

	if (OT_MOVER == GetType() && ((CMover*)this)->IsPlayer())
	{
		bool bResult = prj.m_idPlayerToUserPtr.insert(make_pair(((CMover*)this)->m_idPlayer, (CMover*)this)).second;
		if (bResult == false)
			Error("AddItToGlobalId id:%d duplicated.", ((CMover*)this)->m_idPlayer);
	}

#ifdef __WORLDSERVER
	if (GetType() == OT_CTRL)
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if (pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0')
			g_MapStrToObjId.insert(map< string, DWORD >::value_type(pCommonCtrl->m_CtrlElem.m_strCtrlKey, pCommonCtrl->GetId()));
	}
#endif // __WORLDSERVER
}

void CCtrl::RemoveItFromGlobalId()
{
	if (m_objid == NULL_ID)
		return;

	prj.m_objmap.RemoveKey(m_objid);

	if (OT_MOVER == GetType() && ((CMover*)this)->IsPlayer() && ((CMover*)this)->m_idPlayer != (u_long)0xffffffff)
		prj.m_idPlayerToUserPtr.erase(((CMover*)this)->m_idPlayer);

#ifdef __WORLDSERVER
	if (GetType() == OT_CTRL)
	{
		CCommonCtrl* pCommonCtrl = (CCommonCtrl*)this;
		if (pCommonCtrl->m_CtrlElem.m_strCtrlKey[0] != '\0')
			g_MapStrToObjId.erase(pCommonCtrl->m_CtrlElem.m_strCtrlKey);
	}
#endif // __WORLDSERVER
}

#ifdef __WORLDSERVER

BOOL CCtrl::ProcessDeleteRespawn()
{

#ifdef __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo(GetRespawn(), m_nRespawnType, GetLayer());
#else	// __LAYER_1021
	CRespawnInfo* pRespawnInfo = GetWorld()->m_respawner.GetRespawnInfo(GetRespawn(), m_nRespawnType);
#endif	// __LAYER_1021
	if (pRespawnInfo && pRespawnInfo->m_bRemove)
	{
		Delete();
		return TRUE;
	}
	return FALSE;
}

void CCtrl::RemoveItFromView(BOOL bRemoveall)
{
	if (!GetWorld())
		return;

	CUser* pUser;
	if (GetType() == OT_MOVER && ((CMover*)this)->IsPlayer())
	{
		map<DWORD, CUser* >::iterator it = m_2pc.begin();
		while (it != m_2pc.end())
		{
			pUser = it->second;
			if (pUser != this && pUser->PCRemoveKey(GetId()))
				pUser->AddRemoveObj(GetId());
			++it;
		}

		if (bRemoveall)
			m_2pc.clear();
	}
	else
	{
		map<DWORD, CUser* >::iterator it = m_2pc.begin();
		while (it != m_2pc.end())
		{
			pUser = it->second;
			if (pUser->NPCRemoveKey(GetId()))
				pUser->AddRemoveObj(GetId());
			++it;
		}

		if (bRemoveall)
			m_2pc.clear();
	}

}

BOOL CCtrl::IsNearPC(CUser* pUser)
{
	return IsNearPC(pUser->GetId());
}

BOOL CCtrl::IsNearPC(OBJID objid)
{
	map<DWORD, CUser* >::iterator it = m_2pc.find(objid);
	return (it != m_2pc.end());
}

#endif	// __WORLDSERVER

BOOL	CCtrl::GetSkillProp(ItemProp** ppSkillProp, AddSkillProp** ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr)
{
	ItemProp* pSkillProp = prj.GetSkillProp(nSkill);
	if (pSkillProp == NULL)
	{
		Error("%s : %d. 스킬(%d)의 프로퍼티가 없다.", szErr, GetIndex(), nSkill);
		return FALSE;
	}


	AddSkillProp* pAddSkillProp = prj.GetAddSkillProp(pSkillProp->dwSubDefine, dwLevel);
	if (pAddSkillProp == NULL)
	{
		Error("%s : %d. Add스킬(%d) Lv%d의 프로퍼티가 없다.", szErr, GetIndex(), nSkill, dwLevel);
		return FALSE;
	}

	if (pAddSkillProp->dwName != pSkillProp->dwID)
		Error("%s : %d. Add스킬(%d) AddSkill->dwName과 Skill->dwID가 다르다.", szErr, GetIndex(), nSkill);

	*ppSkillProp = pSkillProp;
	*ppAddSkillProp = pAddSkillProp;

	return TRUE;
}



// ------------------------------------------


void	CCtrl::ApplySkillRegion(const D3DXVECTOR3& vPos, int nApplyType, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, CCtrl* pCenter, BOOL bControl)
{
#ifdef __WORLDSERVER
	int nRange = 4;	// 4m
	CObj* pObj;
	CCtrl* pSrc = this;
	D3DXVECTOR3 vDist;

	if (pSkillProp == NULL)
		Error("CCtrl::ApplySkillRegion : pSkillProp == NULL A:%d-%d", pSrc->GetType(), pSrc->GetIndex());

	FLOAT fRange = 4.0f;
	if (pAddSkillProp)
	{
		fRange = (float)pAddSkillProp->dwSkillRange;
		if (pAddSkillProp->dwSkillRange == NULL_ID)
			Error("경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pAddSkillProp->dwID);
	}
	else
	{
		fRange = (float)pSkillProp->_dwSkillRange;
		if (pSkillProp->_dwSkillRange == NULL_ID)
			Error("경고 : 스킬 %d의 dwSkillRange가 지정되지 않음", pSkillProp->dwID);
	}

	if (fRange <= 4.0f)		nRange = 4;
	else if (fRange <= 8.0f)	nRange = 8;
	else if (fRange <= 16.0f)	nRange = 16;
	else						nRange = 32;

	if (fRange <= 0)
		Error("CCtrl::ApplySkillRegion : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange);

	BOOL	bApply = FALSE;
	BOOL	bTarget = TRUE;


	if (nApplyType & OBJTYPE_PLAYER)
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer())
		{
			bApply = FALSE;

			if (pObj->GetType() != OT_MOVER)
			{
				continue;
			}

			if (pSrc->GetType() == OT_MOVER)
			{
				CMover* pAttacker = (CMover*)pSrc;
				if (pAttacker->IsPlayer())
				{
					bApply = TRUE;

					CMover* pDefender = (CMover*)pObj;
					if (pDefender->IsPlayer())
					{
						if (pCenter)
							bTarget = (pCenter == pDefender);
						else
							bTarget = FALSE;
					}
				}
				else
				{
					bApply = TRUE;
				}
			}
			else
			{
				bApply = TRUE;
			}

			if (bApply)
			{
				if (pObj != pSrc)
				{
					if (pObj->IsRangeObj(vPos, fRange))
					{
						CMover* pTarget = (CMover*)pObj;

						if (pCenter)
							bTarget = (pCenter == pTarget);
						else
							bTarget = FALSE;

						if (IsValidObj(pTarget) && pTarget->IsLive())
						{
							pTarget->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget);
						}
					}
				}

				bApply = FALSE;
			} // bApply
		}
		END_LINKMAP
	} // OBJTYPE_PLAYER


	if (nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL))
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer())
		{
			bApply = FALSE;
			if (pObj->GetType() == OT_MOVER)
			{
				CMover* pTarget = (CMover*)pObj;
				if (pTarget->IsPeaceful() == FALSE)
					bApply = TRUE;
			}
			else if (pObj->GetType() == OT_CTRL)
			{
				if (pSrc->GetType() == OT_MOVER)
					bApply = TRUE;
			}

			if (bApply)
			{
				if (pObj != pSrc)
				{
					if (pObj->IsRangeObj(vPos, fRange))
					{
						if (pObj->GetType() == OT_MOVER)
						{
							if (IsValidObj(pObj) && ((CMover*)pObj)->IsLive())
								((CCtrl*)pObj)->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg);
						}
						else
						{

							((CCtrl*)pObj)->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg);
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

#endif // WORLDSERVER
} // ApplySkillRegion()

void	CCtrl::ApplySkillAround(CCtrl* pSrc, int nApplyType, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb, BOOL bOnlyDmg, FLOAT fRangeCustom, BOOL bControl)
{
#ifdef __WORLDSERVER
	int nRange = 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if (pSkillProp == NULL)
		Error("CCtrl::ApplySkillAround : pSkillProp == NULL D:%d A:%d-%d", GetIndex(), pSrc->GetType(), pSrc->GetIndex());

	FLOAT fRange = 4.0f;
	if (pAddSkillProp)
		fRange = (float)pAddSkillProp->dwSkillRange;
	else
		fRange = (float)pSkillProp->_dwSkillRange;

	if (fRange <= 4.0f)		nRange = 4;
	else if (fRange <= 8.0f)	nRange = 8;
	else if (fRange <= 16.0f)	nRange = 16;
	else						nRange = 32;

	if (fRangeCustom != 0.0f)
	{
		fRange = fRangeCustom;
	}

	FLOAT fTargetRatio = 0.0f;
	if (pSkillProp->dwSpellRegion == SRO_REGION)
		fTargetRatio = 1.0f;

	if (fRange <= 0)
		Error("CCtrl::ApplySkillAround : D:%d A:%d-%d %d %f", GetIndex(), pSrc->GetType(), pSrc->GetIndex(), pSkillProp->dwID, fRange);

	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;


	if (nApplyType & OBJTYPE_PLAYER)
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer())
		{
			bApply = FALSE;

			if (pObj->GetType() != OT_MOVER)
			{
				continue;
			}

			if (pSrc->GetType() == OT_MOVER)
			{
				CMover* pAttacker = (CMover*)pSrc;
				if (pAttacker->IsPlayer())
				{
					bApply = TRUE;
					CMover* pDefender = (CMover*)pObj;
					if (pDefender->IsPlayer())
					{
						bTarget = (this == pDefender);
					}
				}
				else
					bApply = TRUE;
			}
			else
				bApply = TRUE;

			if (bApply)
			{
				if (pObj->IsRangeObj(this, fRange))
				{
					if (pObj == this && fTargetRatio == 0.0f)
					{
					}
					else
					{
						if (pObj != pSrc)
						{
							CMover* pTarget = (CMover*)pObj;
							bTarget = (this == pTarget);
							if (IsValidObj(pTarget) && pTarget->IsLive())
								pTarget->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg, bTarget);
						}
					}
				}

				bApply = FALSE;
			}
		}
		END_LINKMAP
	}


	if (nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL))
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer())
		{
			bApply = FALSE;
			if (pObj->GetType() == OT_MOVER)
			{
				CMover* pTarget = (CMover*)pObj;
				if (pTarget->IsPeaceful() == FALSE)
					bApply = TRUE;
			}
			else if (pObj->GetType() == OT_CTRL)
			{
				if (pSrc->GetType() == OT_MOVER)
					bApply = TRUE;
			}

			if (bApply)
			{
				if (pObj->IsRangeObj(this, fRange))
				{
					if (pObj == this && fTargetRatio == 0.0f)
					{
					}
					else
					{
						if (pObj != pSrc)
						{
							if (pObj->GetType() == OT_MOVER)
							{
								if (IsValidObj(pObj) && ((CMover*)pObj)->IsLive())
									((CCtrl*)pObj)->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg);
							}
							else
							{

								((CCtrl*)pObj)->ApplySkill(pSrc, pSkillProp, pAddSkillProp, bIgnoreProb, 0, bOnlyDmg);
							}
						}
					}
				}
			} // bApply
		}
		END_LINKMAP
	}

#endif // WORLDSERVER
} // ApplySkillAround()



void	CCtrl::ApplySkillLine(int nApplyType, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProb, BOOL bControl)
{
#ifdef __WORLDSERVER
	int nRange = 4;	// 4m
	CObj* pObj;
	D3DXVECTOR3 vPos = GetPos();
	D3DXVECTOR3 vDist;

	if (pSkillProp == NULL)
		Error("CCtrl::ApplySkillLine : pSkillProp == NULL %d", GetIndex());

	D3DXVECTOR3 vMin1 = D3DXVECTOR3(-fWidth / 2.0f, -fHeight / 2.0f, -fDepth);
	D3DXVECTOR3 vMax1 = D3DXVECTOR3(fWidth / 2.0f, fHeight / 2.0f, 0);

	D3DXMATRIX mInv;
	D3DXMATRIX m1, m2, m3;
	D3DXMatrixRotationY(&m1, D3DXToRadian(-GetAngle()));
	D3DXMatrixTranslation(&m2, GetPos().x, GetPos().y, GetPos().z);
	D3DXMatrixMultiply(&m3, &m1, &m2);	// world = rot * trans

	D3DXMatrixInverse(&mInv, NULL, &m3);

	D3DXVECTOR3 vDestLocal;

	if (fDepth <= 4.0f)
		nRange = 4;
	else if (fDepth <= 8.0f)
		nRange = 8;
	else if (fDepth <= 16.0f)
		nRange = 16;
	else
		nRange = 32;

	if (fDepth <= 0)
		Error("CCtrl::ApplySkillLine : %d %d", GetIndex(), fDepth);


	BOOL	bApply = FALSE;
	BOOL	bTarget = FALSE;

	if (nApplyType & OBJTYPE_PLAYER)
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkPlayer, GetLayer())
		{
			bApply = FALSE;

			if (pObj->GetType() != OT_MOVER)
			{
				continue;
			}

			bApply = TRUE;

			if (bApply && pObj != this)
			{
				CMover* pTarget = (CMover*)pObj;
				if (IsValidObj(pTarget) && pTarget->IsLive())
				{
					D3DXVec3TransformCoord(&vDestLocal, &pTarget->GetPos(), &mInv);

					D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
					D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
					if (::IsTouchAABB(vMin1, vMax1, vMin2, vMax2))
					{
						pTarget->ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb, 0, FALSE, bTarget);
					}
				}
			}
		}
		END_LINKMAP
	}


	if (nApplyType & (OBJTYPE_MONSTER | OBJTYPE_CTRL))
	{
		FOR_LINKMAP(GetWorld(), vPos, pObj, nRange, CObj::linkDynamic, GetLayer())
		{
			bApply = FALSE;
			if (pObj->GetType() == OT_MOVER)
			{
				CMover* pTarget = (CMover*)pObj;
				if (pTarget->IsPeaceful() == FALSE)
					bApply = TRUE;
			}
			else if (pObj->GetType() == OT_CTRL)
			{
				if (GetType() == OT_MOVER)
					bApply = TRUE;
			}


			if (bApply)
			{
				if (pObj->GetType() == OT_MOVER)
				{
					CMover* pTarget = (CMover*)pObj;

					if (IsValidObj(pTarget) && pTarget->IsLive())
					{

						D3DXVec3TransformCoord(&vDestLocal, &pTarget->GetPos(), &mInv);

						D3DXVECTOR3 vMin2 = vDestLocal + pTarget->m_pModel->m_vMin;
						D3DXVECTOR3 vMax2 = vDestLocal + pTarget->m_pModel->m_vMax;
						if (::IsTouchAABB(vMin1, vMax1, vMin2, vMax2))
						{
							((CCtrl*)pObj)->ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb);
						}
					}
				}
				else
					((CCtrl*)pObj)->ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb);
			}
		}
		END_LINKMAP
	}

#endif // WORLDSERVER
} // ApplySkillLine()




int		CCtrl::ApplySkillAroundTroupe(u_long idParty, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb)
{
	BOOL bRet = FALSE;
#ifdef __WORLDSERVER
	if (GetType() != OT_MOVER)		return bRet;
	if (((CMover*)this)->IsPlayer() == FALSE)		return bRet;

	CParty* pParty = g_PartyMng.GetParty(idParty);
	if (pParty)
	{
		int i;
		CUser* pMember;
		D3DXVECTOR3	vDist;
		float	fDist;
		float	fMaxDistSq = (float)pAddSkillProp->dwSkillRange;
		fMaxDistSq *= fMaxDistSq;
		FLOAT fTargetRatio = 0.0f;
		if (pSkillProp->dwSpellRegion == SRO_REGION)
			fTargetRatio = 1.0f;
		for (i = 0; i < pParty->m_nSizeofMember; i++)
		{
			pMember = g_UserMng.GetUserByPlayerID(pParty->m_aMember[i].m_uPlayerId);
			if (IsValidObj(pMember) && !pMember->IsDie()
				&& GetWorld() && pMember->GetWorld() && GetWorld()->GetID() == pMember->GetWorld()->GetID()
				&& GetLayer() == pMember->GetLayer())
			{
				if (pMember == this && fTargetRatio == 0.0f)
				{
				}
				else
				{
					vDist = pMember->GetPos() - GetPos();
					fDist = D3DXVec3LengthSq(&vDist);
					if (fDist < fMaxDistSq)
					{
						pMember->ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb);
						bRet = TRUE;
					}
				}
			}
		}
	}
	else
	{
		if (pSkillProp->dwExeTarget == EXT_TROUPEWITH)
		{
			ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb);
		}
	}
#endif // WorldServer
	return bRet;
} // ApplySkillAroundTroupe



// this : Target


BOOL CCtrl::ApplySkillHardCoding(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
#ifdef __WORLDSERVER
	switch (pSkillProp->dwID)
	{
	case SI_ASS_HEAL_RESURRECTION:
		if (GetType() == OT_MOVER)
		{
			CMover* pTarget = (CMover*)this;
			if (pTarget->IsPlayer() && pTarget->IsDie())
			{
				if (pTarget->m_pActMover->GetDmgState() == OBJSTA_RESURRECTION)
					return FALSE;
				else
				{
					if (((CUser*)this)->m_Resurrection_Data.bUseing != TRUE)
					{
						((CUser*)this)->AddResurrectionMessage();

						((CUser*)this)->m_Resurrection_Data.bUseing = TRUE;
						((CUser*)this)->m_Resurrection_Data.dwPlayerID = ((CMover*)pSrc)->m_idPlayer;
						((CUser*)this)->m_Resurrection_Data.pSkillProp = pSkillProp;
						((CUser*)this)->m_Resurrection_Data.pAddSkillProp = pAddSkillProp;
					}

					break;
				}
			}
		}
		break;
	} // switch
#endif // WorldServer
	return TRUE;
}

#ifdef __CLIENT
void CCtrl::CreateYoyoSkill(CSfx* pSfx, CCtrl* pTarget, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
	ItemProp* pItemProp = ((CMover*)this)->GetActiveHandItemProp();

	if (pItemProp)
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vLocal;
		D3DXVECTOR3 vPosSrc = GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 vPosDest = pTarget->GetPos() + D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		CModelObject* pModel = (CModelObject*)m_pModel;

		int nSkill = pSkillProp->dwID;

		switch (nSkill)
		{
		case SI_ACR_SUP_SLOWSTEP:
		case SI_JST_YOYO_HITOFPENYA:
		case SI_JST_YOYO_VATALSTAB:
		{
			pModel->GetHandPos(&vPos, PARTS_RWEAPON, GetMatrixWorld());
			pSfx = CreateSfxYoYo(D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest);
			((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);
		}
		break;
		case SI_ACR_YOYO_DEADLYSWING:
		case SI_ACR_YOYO_CROSSLINE:
		{
			pModel->GetHandPos(&vPos, PARTS_RWEAPON, GetMatrixWorld());
			pSfx = CreateSfxYoYo(D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest);
			((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_RWEAPON);

			pModel->GetHandPos(&vPos, PARTS_LWEAPON, GetMatrixWorld());
			pSfx = CreateSfxYoYo(D3DDEVICE, pItemProp->dwSfxObj2, vPos, GetId(), vPosDest);
			((CSfxItemYoyoAtk*)pSfx)->MakePath(PARTS_LWEAPON);
		}
		break;
		}
	}

}
#endif //__CLIENT




void CCtrl::CreateSkillSfx(CCtrl* pTarget, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
#ifdef __CLIENT

	//sun skill here
	{
		// 발사체 형태가 아니다.
		CSfx* pSfx = NULL;
		if ((int)pAddSkillProp->dwSkillTime > 0)	// 지속시간이 있는 스킬은
		{
			if (pSkillProp->dwSfxObj4 == NULL_ID)		// 이펙이 유지되는 스킬이 아닐경우에만.
			{
				if (pSkillProp->dwExeTarget == EXT_MELEEATK)
				{
					D3DXVECTOR3 vLocal;

					if (pSkillProp->dwLinkKind == IK3_YOYO)
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ(&vLocal, GetAngle(), 1.3f);	// 때리는방향 1미터앞.
						vLocal += GetPos();
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vLocal, pTarget->GetId(), 0);

					if (pSkillProp->dwSfxObj5 != NULL_ID)		// 시전자에게 발생
						pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), GetPos(), GetId(), 0);  // 5번 이펙트를 출력함.
				}
				else
				{
					if (pSkillProp->dwSfxObj3 != NULL_ID)		// 타겟에게 발생
						pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), pTarget->GetPos(), pTarget->GetId(), 0);  // 3번 이펙트를 출력함.
					if (pSkillProp->dwSfxObj5 != NULL_ID)		// 시전자에게 발생
						pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), GetPos(), GetId(), 0);  // 5번 이펙트를 출력함.
				}

				if (pSkillProp->dwLinkKind == IK3_YOYO)  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
					CreateYoyoSkill(pSfx, pTarget, pSkillProp, pAddSkillProp);

			}
			else
			{
				if (GetType() == OT_MOVER)
				{
					//gmpbigsun( 20100824 ) : CreateSfx의 Return값을 받아서 사운드 출력이 가능하게 함.
					if (pSkillProp->dwSkillType == NULL_ID)	// NULL_ID면 아이템들
					{
						if (((CMover*)this)->HasBuff(BUFF_ITEM, (WORD)(pSkillProp->dwID)) == FALSE)	// 이미 실행되고 있지 않을때만 이펙 출력
							pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj4, GetPos(), GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f));
					}
					else
					{
						if (pSkillProp->dwID == SI_ELE_FIRE_BURINGFIELD || pSkillProp->dwID == SI_RIN_SUP_MERKABAHANZELRUSHA || pSkillProp->dwID == SI_GEN_FRAMEARROW_BURN)
							pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj4, GetPos(), GetId(), pTarget->GetPos(), pTarget->GetId(), (int)(pAddSkillProp->dwSkillTime / 1000.0f));

						D3DXVECTOR3 vPos = GetPos();
						PLAYSND(pSkillProp->dwSndAttack1, &vPos);		// 발사 효과음.
					}
				}
			}
		}
		else
		{	// 지속시간 없는 스킬
			if (pSkillProp->dwExeTarget == EXT_MELEEATK)	// 근접공격 스킬의 타격 이펙트.
			{
				if (pSkillProp->dwSfxObj3 != NULL_ID)
				{
					D3DXVECTOR3 vLocal;

					if (pSkillProp->dwLinkKind == IK3_YOYO)
					{
						vLocal = pTarget->GetPos();
						vLocal.y += 1.0f;
					}
					else
					{
						AngleToVectorXZ(&vLocal, GetAngle(), 1.3f);	// 때리는방향 1미터앞.
						vLocal += GetPos();		// GetPos()를 this에서 pTarget으로 바꿨다. 2006/6/20 xuzhu.
						vLocal.y += 1.0f;
					}

					pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj3, vLocal, GetId(), vLocal, pTarget->GetId(), 0);	// 2006/6/20 xuzhu 가 바꿈.
				}
			}
			else
			{
				if (pSkillProp->dwSfxObj3 != NULL_ID)
				{
					D3DXVECTOR3 vPos = pTarget->GetPos();
					vPos.y += 1.0f;
					pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj3, GetPos(), GetId(), vPos, pTarget->GetId(), 0);
				}
			}

			if (pSkillProp->dwLinkKind == IK3_YOYO)  //요요는 아이템프로퍼티참조하여 이펙트 생성(예외처리)
			{
				CreateYoyoSkill(pSfx, pTarget, pSkillProp, pAddSkillProp);
			}
			else
				if (pSkillProp->dwSfxObj5 != NULL_ID)		// 시전자에게 발생
					pSfx = CreateSfx(D3DDEVICE, pSkillProp->dwSfxObj5, GetPos(), GetId(), GetPos(), GetId(), 0);  // 5번 이펙트는 시전자 자신에게 출력
		}
		if (pSfx)
		{
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND(pSkillProp->dwSndAttack1, &vPos);		// 발사 효과음.
		}

		//dwSkillRange
		if (pSkillProp->dwID == SI_GEN_FLASH && pSfx)
		{
			FLOAT fRange = 1.5f;
			pSfx->SetScale(D3DXVECTOR3(fRange, fRange, fRange));
		}
	}

#endif // Client
} // CreateSkillSfx



int		CCtrl::ApplyDamage(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, int nParam, BOOL bTarget)
{
	int nDamage = 0;

	switch (pSkillProp->dwExeTarget)
	{
	case EXT_MELEEATK:
	case EXT_MAGICATK:
	case EXT_MAGICATKSHOT:
	{
		if (pSkillProp->dwID == SI_MAG_FIRE_HOTAIR)
		{
			nDamage = SendDamage(AF_MAGICSKILL, pSrc->GetId(), nParam, bTarget);
		}
		else
		{
			int nAtkFlag = 0;
			if (pSkillProp->dwSkillType == KT_MAGIC)
				nAtkFlag = AF_MAGICSKILL;
			else
				nAtkFlag = AF_MELEESKILL;

			if (pSkillProp->dwComboStyle == CT_FINISH)		nDamage = SendDamageForce(nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget);
			else											nDamage = SendDamage(nAtkFlag, pSrc->GetId(), (pSkillProp->dwID << 16) | pAddSkillProp->dwSkillLvl, bTarget);
		}
	}
	break;
	}

	return nDamage;
}


BOOL CCtrl::IsPossibleApplySkill(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
	return TRUE;
}

void	CCtrl::ApplySkill(CCtrl* pSrc, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bTarget)
{
#ifdef __PK_PVP_SKILL_REGION
	if (pSrc->GetType() == OT_MOVER && GetType() == OT_MOVER)
	{
		if (((CMover*)pSrc)->IsPlayer() && ((CMover*)this)->IsPlayer() && pSkillProp->dwSkillType != NULL_ID)
		{
			if (pSkillProp->nEvildoing < 0)
			{
				CMover* pAttacker = (CMover*)pSrc;
				CMover* pDefender = (CMover*)this;
				if (!(pAttacker->IsGuildCombatTarget(pDefender)
#ifdef __JEFF_11_4
					|| pAttacker->IsArenaTarget(pDefender)
#endif
#ifdef __FL_FFA
#ifdef __CLIENT
					|| (pDefender->GetWorld() && pDefender->GetWorld()->IsFFA())
#else
					|| (CFFAManager::GetInstance()->IsFFAEnjoyer(pAttacker) && CFFAManager::GetInstance()->IsFFAEnjoyer(pDefender))
#endif
#endif
#ifdef __FL_TDM
#ifdef __CLIENT
					|| (g_WndMng.m_pWndWorld && g_WndMng.m_pWndWorld->IsCanKillTDM(pDefender))
#else
					|| CTDMManager::GetInstance()->IsCanKill(pAttacker, pDefender)
#endif
#endif
					))
					return;
			}
		}
	}
#endif // __PK_PVP_SKILL_REGION

	if (IsPossibleApplySkill(pSrc, pSkillProp, pAddSkillProp) == FALSE)
		return;

	int nDamage = ApplyDamage(pSrc, pSkillProp, pAddSkillProp, nParam, bTarget);
	if (bOnlyDmg == TRUE)
		return;

	if (pSkillProp->dwID != SI_RIN_SUP_HOLYCROSS && pSkillProp->dwID != SI_GEN_SWORDCROSS)
	{
		if (GetType() == OT_MOVER)
		{
			MoverProp* pMoverProp = ((CMover*)this)->GetProp();
			if (pMoverProp)
			{
				if (pMoverProp->dwClass == RANK_MIDBOSS || pMoverProp->dwClass == RANK_BOSS || pMoverProp->dwClass == RANK_SUPER)
					return;
			}
		}
	}

#if defined(__WORLDSERVER)
	if (this != pSrc && pSrc->GetType() == OT_MOVER && GetType() == OT_MOVER)
	{
		if (((CMover*)this)->IsDie() == FALSE)
		{
			if (pSkillProp->nEvildoing < 0)
				if (((CMover*)pSrc)->GetHitType2(((CMover*)this), bTarget, FALSE) == HITTYPE_FAIL)
					return;
#ifdef __JEFF_11_4
			CWorld* pWorld = pSrc->GetWorld();
			if (pWorld && pWorld->IsArena() == FALSE)
#endif	// __JEFF_11_4
			{
				if (pSkillProp->nEvildoing > 0)
					if (((CMover*)pSrc)->GetHitType2(((CMover*)this), bTarget, TRUE) != HITTYPE_FAIL)
						return;
			}
		}
		else
		{
			if (pSkillProp->dwExeTarget != EXT_MAGICATKSHOT)
				if (pSkillProp->dwSfxObj2 != NULL_ID)
					g_UserMng.AddCreateSfxObj(this, pSkillProp->dwSfxObj2);

			if (pSkillProp->nEvildoing < 0)
				return;
		}
	}
#endif //defined(__WORLDSERVER)

	int nProb = pSkillProp->nProbability;
	if (pAddSkillProp)
	{
		nProb = pAddSkillProp->nProbability;

		BOOL bPVP = (pSrc->GetType() == OT_MOVER && ((CMover*)pSrc)->IsPlayer() && GetType() == OT_MOVER && ((CMover*)this)->IsPlayer());
		if (bPVP)
			nProb = pAddSkillProp->nProbabilityPVP;

	}

	if (bIgnoreProb)
		nProb = NULL_ID;

	if (nProb == NULL_ID || (int)(xRandom(100)) < nProb)
	{
		if (GetType() == OT_MOVER)
		{
			int nSkillTime = 0;
			if (pAddSkillProp)
				nSkillTime = (int)pAddSkillProp->dwSkillTime;
			else
				nSkillTime = (int)pSkillProp->dwSkillTime;

			BOOL bNew = TRUE;
			if (nSkillTime >= 0)
			{
				DWORD dwTime1 = 0, dwTime2 = 0;
				if (pSkillProp->dwReferTarget1 == RT_TIME || pSkillProp->dwReferTarget2 == RT_TIME)
				{
					if (pSrc->GetType() == OT_MOVER)
						((CMover*)pSrc)->SubReferTime(&dwTime1, &dwTime2, pSkillProp, pAddSkillProp);
				}

				WORD wType;
				if (pSkillProp->dwSkillType == NULL_ID)
					wType = BUFF_ITEM;
				else
					wType = BUFF_SKILL;


#if defined(__WORLDSERVER)
				int nLevel = 0;
				if (pAddSkillProp)
					nLevel = pAddSkillProp->dwSkillLvl;
				else
					nLevel = 0;

				if (pSkillProp->dwID == SI_ELE_FIRE_BURINGFIELD)
					bNew = FALSE;
				else
#ifdef __PVPDEBUFSKILL
					bNew = static_cast<CMover*>(this)->AddBuff(wType, (WORD)(pSkillProp->dwID), nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2, pSrc->GetId());
#else // __PVPDEBUFSKILL
					bNew = ((CMover*)this)->m_SkillState.Set(wType, pSkillProp->dwID, nLevel, nSkillTime + (int)dwTime1 + (int)dwTime2);
#endif // __PVPDEBUFSKILL


#elif defined(__CLIENT)
				bNew = ((CMover*)this)->HasBuff(wType, (WORD)(pSkillProp->dwID)) ? FALSE : TRUE;
#endif // Client
			}

			if (bNew)
			{
#ifdef __WORLDSERVER

				if (ApplySkillHardCoding(pSrc, pSkillProp, pAddSkillProp) == FALSE)
					return;

				if (pSkillProp->dwID == SI_ASS_HEAL_RESURRECTION)
					return;

				switch (pSkillProp->dwExeTarget)
				{
				case EXT_SELFCHGPARAMET:
					if (pSrc->GetType() == OT_MOVER)
						((CMover*)pSrc)->ApplyParam((CMover*)pSrc, pSkillProp, pAddSkillProp, TRUE, nDamage);
					break;
				case EXT_OBJCHGPARAMET:
				case EXT_MAGIC:
				case EXT_ANOTHER:
				case EXT_ANOTHERWITH:
				default:
					if (this->GetType() == OT_MOVER)
						((CMover*)this)->ApplyParam((CMover*)pSrc, pSkillProp, pAddSkillProp, TRUE, nDamage);
					break;
				}

				if (this->GetType() == OT_MOVER)
				{
					CMover* pTarget = (CMover*)this;

					DWORD dwDmgShift = NULL_ID;
					if (pAddSkillProp)
						dwDmgShift = pAddSkillProp->dwDmgShift;
					else
						dwDmgShift = pSkillProp->dwDmgShift;

					if (dwDmgShift != NULL_ID)
						pTarget->pushedByAnotherPower(GetDegree(pTarget->GetPos(), pSrc->GetPos()), 0.85f);

					if (pSkillProp->dwID == SI_ACR_YOYO_PULLING || pSkillProp->dwID == SI_LOD_SUP_PULLING)
					{
						if (pAddSkillProp && pAddSkillProp->nDestData1[2] != NULL_ID)
						{
							FLOAT fPullingLen = (FLOAT)pAddSkillProp->nDestData1[2] * 0.001f;

							D3DXVECTOR3 v3Len = pTarget->GetPos() - pSrc->GetPos();
							FLOAT fTotalLen = D3DXVec3LengthSq(&v3Len) * 0.1f;
							{
								if (fTotalLen > ((CMover*)this)->GetAttackRange(AR_HRANGE))
									fTotalLen = ((CMover*)this)->GetAttackRange(AR_HRANGE);

								FLOAT fDest = fTotalLen / ((CMover*)this)->GetAttackRange(AR_HRANGE);
								pTarget->pushedByAnotherPower(GetDegree(pTarget->GetPos(), pSrc->GetPos()), fPullingLen * -fDest);
							}
						}
					}
				}
#endif // __WORLDSERVER
			} // if( bNew )

#ifdef __WORLDSERVER
			if (pSkillProp->dwID == SI_GEN_FLASH && GetType() == OT_MOVER && ((CMover*)this)->IsPlayer())
			{
				((CUser*)this)->AddCommonSkill(pSkillProp->dwID, pAddSkillProp->dwSkillLvl);
			}
#endif // __WORLDSERVER

		}
	}

#ifdef __WORLDSERVER
	switch (pSkillProp->dwSpellRegion)
	{
	case SRO_AROUND:
	case SRO_REGION:
		if (pSkillProp->dwExeTarget == EXT_TROUPE || pSkillProp->dwExeTarget == EXT_TROUPEWITH)
			if (pSkillProp->dwSfxObj3 != NULL_ID)
				g_UserMng.AddCreateSfxObj(this, pSkillProp->dwSfxObj3);
		break;
	}


	if (pSkillProp->dwID == SI_ACR_YOYO_SNITCH)
	{
		CUser* pUser = (CUser*)pSrc;
		CUser* pDest = (CUser*)this;
		if (pDest->GetType() == OT_MOVER && pDest->GetGold() > 0)
		{
			int nPlus = (pDest->GetLevel() * 20) - (pUser->GetLevel() * 10);

			if (pUser->GetLevel() <= 20)
			{
			}
			else if (pUser->GetLevel() <= 30)
			{
				nPlus = (int)(nPlus * 0.7f);
			}
			else if (pUser->GetLevel() <= 40)
			{
				nPlus = (int)(nPlus * 0.5f);
			}
			else
			{
				nPlus = (int)(nPlus * 0.3f);
			}

			if (nPlus > 0)
			{
				nPlus = min(nPlus, pDest->GetGold());
				pUser->AddGold(nPlus);
				pDest->AddGold(-nPlus);
			}
		}
	}
#endif	//__WORLDSERVER
} // ApplySkill


int		CCtrl::ShootSkill(CCtrl* pTarget, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp)
{
#ifdef __CLIENT

	if (pSkillProp->dwExeTarget != EXT_MAGICATKSHOT
		&& pSkillProp->dwExeTarget != EXT_MAGICSHOT
		)
		return 0;

	DWORD dwShootObj = pSkillProp->dwSfxObj2;
	if (dwShootObj != NULL_ID)
	{
		D3DXVECTOR3 vPos = GetPos();

		if (((CMover*)this)->IsPlayer())
		{
			if (pSkillProp->dwLinkKind == IK3_BOW)
			{

				CModelObject* pModel = (CModelObject*)m_pModel;
				pModel->GetHandPos(&vPos, PARTS_LWEAPON, GetMatrixWorld());
				vPos.y -= 1.0f;
			}
		}

		CSfxShoot* pShootSfx = CreateShootSfx(D3DDEVICE, dwShootObj, vPos, GetId(), D3DXVECTOR3(0, 0, 0), pTarget->GetId());
		if (pShootSfx)
		{
			D3DXVECTOR3 vPos = GetPos();
			PLAYSND(pSkillProp->dwSndAttack1, &vPos);

			if (GetType() == OT_MOVER)
			{
				if (((CMover*)this)->IsActiveMover())
				{
					int idSfxHit = ++(((CMover*)this)->m_idSfxHit);
					int		nMaxDmgCnt = 1;
					if (pSkillProp->tmContinuousPain != NULL_ID)
						nMaxDmgCnt = (pAddSkillProp->dwSkillTime / pAddSkillProp->dwPainTime) + 1;

					if (pSkillProp->dwSkillType == KT_SKILL)
						g_DPlay.SendSfxID(pTarget->GetId(), idSfxHit, AF_MELEESKILL, NULL_ID, nMaxDmgCnt);
					else
						g_DPlay.SendSfxID(pTarget->GetId(), idSfxHit, AF_MAGICSKILL, NULL_ID, nMaxDmgCnt);
					pShootSfx->m_idSfxHit = idSfxHit;
				}
			}
			pShootSfx->SetSkill(pSkillProp->dwID);
			pShootSfx->m_nMagicPower = pAddSkillProp->dwSkillLvl;
			pShootSfx->m_dwSfxHit = pSkillProp->dwSfxObj3;

		}

	}
	else
		Error("ShootSkill MAGICATKSHOT : %s dwSfxObj2가 지정되지 않음", pSkillProp->szName);
#endif // CLIENT
	return 1;
}



int		CCtrl::TargetSelecter(CCtrl* pTarget)
{
	int nApplyType = 0;
	CCtrl* pSrc = this;

	if (pTarget->GetType() == OT_MOVER)
	{
		nApplyType = OBJTYPE_PLAYER | OBJTYPE_MONSTER;
	}
	else
	{

		if (pSrc->GetType() == OT_MOVER)
		{
			if (((CMover*)pSrc)->IsPlayer())
				nApplyType = OBJTYPE_PLAYER | OBJTYPE_CTRL | OBJTYPE_MONSTER;
		}
		else
		{
			nApplyType = OBJTYPE_PLAYER;
		}
	}

	return nApplyType;
}

int		CCtrl::DoApplySkill(CCtrl* pTarget, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bControl)
{
	int nResult = DoApplySkillEx(pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, nParam, bOnlyDmg, bControl);
	if (nResult == 1)
	{
#if defined(__WORLDSERVER)
		if (this != pTarget && GetType() == OT_MOVER && pTarget->GetType() == OT_MOVER)
		{
			if (pSkillProp->nEvildoing < 0)
				((CMover*)pTarget)->OnAttacked((CMover*)this, 0, TRUE, 0);
		}
#endif
	}

	return nResult;
}

int		CCtrl::DoApplySkillEx(CCtrl* pTarget, ItemProp* pSkillProp, AddSkillProp* pAddSkillProp, bool bIgnoreProb, int nParam, BOOL bOnlyDmg, BOOL bControl)
{
	int		nRet = 1;
	switch (pSkillProp->dwSpellRegion)
	{
	case SRO_AROUND:	// 대상 0% 주위 100%
	case SRO_REGION:	// 대상 100% 주위 100%
	{
		if (pSkillProp->dwExeTarget != EXT_TROUPE && pSkillProp->dwExeTarget != EXT_TROUPEWITH)	// 극단대상이 아닐때.
		{
			int nApplyType = 0;
			nApplyType = TargetSelecter(pTarget);
			pTarget->ApplySkillAround(this, nApplyType, pSkillProp, pAddSkillProp, bIgnoreProb, bOnlyDmg, 0.0f, bControl);
		}
		else if (pSkillProp->dwExeTarget == EXT_TROUPE || pSkillProp->dwExeTarget == EXT_TROUPEWITH)	// 극단 대상(자기 자신 포함)
		{
			if (GetType() == OT_MOVER && ((CMover*)this)->IsPlayer())
			{
				ApplySkillAroundTroupe(((CMover*)this)->m_idparty, pSkillProp, pAddSkillProp, bIgnoreProb);
			}
		}

#ifdef __WORLDSERVER
		// chipi - 범위 ctrl 생성 스킬로 인해 듀얼 종료 후 카오되는 현상 수정
		if (pSkillProp->tmContinuousPain != NULL_ID)	// 지속데미지 형태냐?
		{
			CContDamageCtrl* pDmgCtrl = new CContDamageCtrl;	// 지속데미지를 주는 가상오브젝트 생성 - 메모리풀링 처리 할것
			if (pDmgCtrl)
			{
				pDmgCtrl->m_pSkillProp = pSkillProp;
				pDmgCtrl->m_pAddSkillProp = pAddSkillProp;
				pDmgCtrl->m_idSrc = GetId();
				pDmgCtrl->m_idTarget = pTarget->GetId();
				pDmgCtrl->SetPos(pTarget->GetPos());

				if (GetWorld()->ADDOBJ(pDmgCtrl, FALSE, GetLayer()) == FALSE)
				{
					SAFE_DELETE(pDmgCtrl);
				}
			}
		}
#endif
	}
	break;
	case SRO_LINE:		// this를 중심으로 m_fAngle라인.
	{
		int nApplyType = 0;
		nApplyType = TargetSelecter(pTarget);
		ApplySkillLine(nApplyType, pSkillProp, pAddSkillProp, 2.5f, 2.5f, (float)pAddSkillProp->dwSkillRange, bIgnoreProb, bControl);
	}
	break;
	case SRO_DIRECT:	// 대상 하나에만 적용
	default:
		if (pSkillProp->dwID != SI_PSY_PSY_PSYCHICWALL)		// 사이킥월은 처리 안함.
			pTarget->ApplySkill(this, pSkillProp, pAddSkillProp, bIgnoreProb, nParam);

		break;
	}

#ifdef __WORLDSERVER		
	// 싸이킥월의 벽을 만들어냄.
	if (pSkillProp->dwID == SI_PSY_PSY_PSYCHICWALL)
	{
		BOOL bCreate = FALSE;
		int i = 0;
		if (GetType() == OT_MOVER)
		{
			if (((CMover*)this)->IsPlayer())
			{
				CUser* pUser = (CUser*)this;
				for (i = 0; i < 2; i++)		// 한번에 2개이상 생성되지 않게함.
				{
					if (pUser->m_pWall[i] == NULL)
					{
						CObj* pObj = CreateObj(D3DDEVICE, OT_CTRL, CI_PSYCHICWALL);
						if (pObj)
						{
							pObj->SetPos(GetPos());
							pObj->SetAngle(-(GetAngle()));
							((CCommonCtrl*)pObj)->m_pSkillProp = pSkillProp;
							((CCommonCtrl*)pObj)->m_pAddSkillProp = pAddSkillProp;
							((CCommonCtrl*)pObj)->m_idAttacker = GetId();
							((CCommonCtrl*)pObj)->m_bControl = bControl;

							if (GetWorld()->ADDOBJ(pObj, TRUE, GetLayer()) == TRUE)
							{
								pUser->m_pWall[i] = pObj;
								bCreate = TRUE;
							}
							else
							{
								SAFE_DELETE(pObj);
								bCreate = FALSE;
							}
							break;
						}
					}
				}
			}
		}
		if (bCreate == FALSE)	// 생성못했으면
			nRet = 0;			// 실패 리턴

	}


	if (GetType() == OT_MOVER)
	{
		if (!((CMover*)this)->IsInvisibleSkill(pSkillProp->dwID) && pAddSkillProp)
			((CMover*)this)->RemoveInvisibleSkill();

		if (pSkillProp->dwID == SI_JST_SUP_POISON)
		{
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_BLEEDING);
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_ABSORB);
		}
		else if (pSkillProp->dwID == SI_JST_SUP_BLEEDING)
		{
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_POISON);
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_ABSORB);
		}
		else if (pSkillProp->dwID == SI_JST_SUP_ABSORB)
		{
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_BLEEDING);
			((CMover*)this)->RemoveBuff(BUFF_SKILL, SI_JST_SUP_POISON);
		}

	}
#endif // WorldServer

#ifdef __CLIENT
	CreateSkillSfx(pTarget, pSkillProp, pAddSkillProp);
#endif

	if (pSkillProp->dwActiveSkill != NULL_ID)
	{
		if (pSkillProp->dwActiveSkillRate == NULL_ID || xRandom(100) <= pSkillProp->dwActiveSkillRate)	// 발동확률에 걸렸는가.
			DoActiveSkill(pSkillProp->dwActiveSkill, 1, pTarget, false, bControl);		// dwActiveSkill Lv1이 발동됨.

	}

	return nRet;
}

void CCtrl::DoActiveSkill(DWORD dwSkill, int nLevel, CCtrl* pTarget, bool bIgnoreProb, BOOL bControl)
{
	if (IsInvalidObj(pTarget))
		return;

	ItemProp* pSkillProp;
	AddSkillProp* pAddSkillProp;
	GetSkillProp(&pSkillProp, &pAddSkillProp, dwSkill, nLevel, "DoActiveSkill");

	int nRet;
	if (pSkillProp->dwExeTarget == EXT_MAGICATKSHOT ||
		pSkillProp->dwExeTarget == EXT_MAGICSHOT)
		nRet = ShootSkill(pTarget, pSkillProp, pAddSkillProp);
	else
	{
		nRet = DoApplySkill(pTarget, pSkillProp, pAddSkillProp, bIgnoreProb, 0, FALSE, bControl);
#ifdef __WORLDSERVER
		g_UserMng.AddDoApplySkill(this, pTarget->GetId(), dwSkill, nLevel);
#endif
	}
}


