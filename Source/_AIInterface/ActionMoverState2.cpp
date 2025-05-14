#include "stdafx.h"
#include "defineObj.h"
#include "ActionMover.h"
#include "..\_Common\ParticleMng.h"
#ifdef __CLIENT
#include "..\_Common\TailEffectMng.h"
#include "DPClient.h"
#endif
#ifdef __WORLDSERVER
#include "User.h"
#include "DPCoreClient.h"
#include "DPDatabaseClient.h"
#include "DPSrvr.h"
#endif


const float FRIC_AIR = 0.011f;


#ifdef __CLIENT
extern	CDPClient	g_DPlay;
extern int			g_nFlySpeed;
#endif // CLIENT

#ifdef __WORLDSERVER
extern	CUserMng			g_UserMng;
extern	CDPCoreClient		g_DPCoreClient;
extern	CDPDatabaseClient	g_dpDBClient;
extern	CDPSrvr				g_DPSrvr;
#endif // __WORLDSERVER

#ifdef __CLIENT
void CreateFlyParticle(CMover* pMover, float fAngX, int nType)
{
	FLOAT fAngXZ, fAngH, fDist, fSpeed;
	D3DXVECTOR3 vVel;
	D3DXVECTOR3 vPos = pMover->GetScrPos();
	if (nType == 0)
		vPos.y += 0.8f;

	fAngXZ = pMover->GetScrAngle();
	fAngXZ += 180.0f;
	fAngXZ = D3DXToRadian(fAngXZ);

	vPos.x += (xRandom(100) / 100.0f) - 0.50f;
	vPos.y += (xRandom(100) / 100.0f) - 0.50f;
	vPos.z += (xRandom(100) / 100.0f) - 0.50f;

	for (int i = 0; i < 2; i++)
	{
		fAngXZ = (float)(-45.0f + xRandomF(90));
		fAngXZ += pMover->GetScrAngle();
		fAngXZ += 180.0f;

		fAngXZ = D3DXToRadian(fAngXZ);
		fAngH = (float)(fAngX + xRandomF(10));
		fAngH = D3DXToRadian(fAngH);
		fSpeed = xRandomF(0.02f);

		fDist = cosf(-fAngH) * fSpeed;
		vVel.x = sinf(fAngXZ) * fDist;
		vVel.z = -cosf(fAngXZ) * fDist;
		vVel.y = -sinf(-fAngH) * fSpeed;
		if (nType == 1)
			g_ParticleMng.CreateParticle(10 + xRandom(3), vPos, vVel, CMover::GetActiveMover()->GetPos().y + 0.5f);
		else
			g_ParticleMng.CreateParticle(2 + xRandom(3), vPos, vVel, CMover::GetActiveMover()->GetPos().y + 0.5f);
	}

}

void CActionMover::ProcessFlyParticle(float fLenSq)
{
	CMover* pMover = m_pMover;


	if ((pMover->IsMode(TRANSPARENT_MODE)) == 0)
	{
		ItemProp* pRideProp = prj.GetItemProp(m_pMover->m_dwRideItemIdx);

		if (fLenSq > 0.01f)
		{
			if ((pMover->IsActiveMover() && g_Neuz.m_camera.m_fZoom > 2.0f) || pMover->IsActiveMover() == FALSE)
			{
				int nType = 0;
				if (pRideProp && pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD)
					nType = 1;
				CreateFlyParticle(pMover, pMover->GetScrAngleX(), nType);
			}
		}

		if (fLenSq > 0.001f)
		{
			if ((pMover->IsActiveMover() && g_Neuz.m_camera.m_fZoom > 1.0f) || pMover->IsActiveMover() == FALSE)
			{
				if (pRideProp && pRideProp->dwItemKind3 == IK3_BOARD)
				{
					if (m_pTail)
					{
						if (pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD)
						{
							if (m_pTail->GetType() != 2)
								m_pTail->ChangeTexture(D3DDEVICE, "etc_Tail2.bmp", 2);
						}
						else
						{
							if (m_pTail->GetType() != 1)
								m_pTail->ChangeTexture(D3DDEVICE, "etc_Tail1.bmp", 1);
						}


					}
					if (m_pTail == NULL)
					{
						if (pRideProp->dwID == II_RID_RID_BOR_RIDINGCLOUD)
						{
							m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect(g_Neuz.m_pd3dDevice, "etc_Tail2.bmp", 2);
						}
						else
						{
							m_pTail = (CTailEffectBelt*)g_TailEffectMng.AddEffect(g_Neuz.m_pd3dDevice, "etc_Tail1.bmp", 1);
						}
					}
					D3DXVECTOR3	vPos1, vPos2;
					D3DXVECTOR3	vLocal;
					FLOAT		fAngXZ = pMover->GetScrAngle();
					FLOAT		fAngH = pMover->GetScrAngleX();

					AngleToVectorXZ(&vLocal, fAngXZ, -1.0f);

					fAngXZ -= 90.0f;
					if (fAngXZ < 0)
						fAngXZ += 360.0f;
					AngleToVector(&vPos1, fAngXZ, -fAngH, 0.5f);
					vPos1 += pMover->GetScrPos();

					vPos1 += vLocal;

					fAngXZ = pMover->GetScrAngle();
					fAngH = pMover->GetScrAngleX();

					AngleToVectorXZ(&vLocal, fAngXZ, -1.0f);

					fAngXZ += 90.0f;
					if (fAngXZ > 360.0f)
						fAngXZ -= 360.0f;
					AngleToVector(&vPos2, fAngXZ, -fAngH, 0.5f);
					vPos2 += pMover->GetScrPos();
					vPos2 += vLocal;

					if (m_pTail)
						m_pTail->CreateTail(vPos1, vPos2);
				}
			}
		}
	}
}

void CActionMover::ProcessFlyTracking()
{
	CMover* pMover = m_pMover;


	if (pMover->IsActiveMover() && (pMover->m_dwFlag & MVRF_TRACKING))
	{
		static float s_fTurnAcc = 0, s_fTurnAccH = 0;
		{
			CMover* pTarget = prj.GetMover(pMover->m_idTracking);
			if (pTarget)
			{
				D3DXVECTOR3	vDist = pTarget->GetPos() - pMover->GetPos();
				FLOAT fAngXZ, fAngH;

				xGetDegree(&fAngXZ, &fAngH, vDist);


				FLOAT fMoverAng = pMover->GetAngle();
				if (fMoverAng > 180.0f)
					fMoverAng -= 360.0f;
				FLOAT fSubAng = fAngXZ - fMoverAng;
				if (fSubAng > 180.0f)
					fSubAng -= 360.0f;
				else if (fSubAng < -180.0f)
					fSubAng += 360.0f;

				if (fSubAng < -3.0f)
					s_fTurnAcc = -2.5f;
				else
					if (fSubAng > 3.0f)
						s_fTurnAcc = 2.5f;
					else
						s_fTurnAcc = 0;

				FLOAT fMoverAngX = pMover->GetAngleX();
				FLOAT fSubAngH = fAngH - fMoverAngX;
				if (fSubAngH > 180.0f)
					fSubAngH -= 360.0f;
				else if (fSubAngH < -180.0f)
					fSubAngH += 360.0f;

				if (fSubAngH < -3.0f)
					s_fTurnAccH = -1.5f;
				else
					if (fSubAngH > 3.0f)
						s_fTurnAccH = 1.5f;
					else
						s_fTurnAccH = 0;

				pMover->SetAngle(pMover->GetAngle() + s_fTurnAcc);
				pMover->SetAngleX(pMover->GetAngleX() + s_fTurnAccH);

				if (s_fTurnAcc || s_fTurnAccH)
					g_DPlay.PostPlayerAngle(TRUE);
			}
		}
	}
}
#endif // __CLIENT

void	CActionMover::ProcessFlyMove(void)
{
	float fLenSq = D3DXVec3LengthSq(&m_vDelta);
	if (fLenSq == 0.0f && (GetStateFlag() & OBJSTAF_ACC) == 0)
		return;

	CMover* pMover = m_pMover;
	FLOAT fAccPwr = m_fAccPower;

#ifdef __CLIENT
	ProcessFlyTracking();
#endif // client


	if ((GetStateFlag() & OBJSTAF_TURBO) && (GetStateFlag() & OBJSTAF_ACC))
	{
#ifdef __WORLDSERVER
		pMover->m_tmAccFuel = (int)(pMover->m_tmAccFuel - (1000.0f / (float)FRAME_PER_SEC));
#endif
		if (pMover->m_tmAccFuel <= 0)
		{
			pMover->m_tmAccFuel = 0;
			SendActMsg(OBJMSG_MODE_TURBO_OFF);
#ifdef __WORLDSERVER
			g_UserMng.AddSendActMsg(pMover, OBJMSG_MODE_TURBO_OFF);
#endif
		}
		else
			fAccPwr *= 1.6f;
	}

#ifdef __CLIENT
	ProcessFlyParticle(fLenSq);
#endif

	BOOL bMaxSpeed = FALSE;
	if (fAccPwr > 0.0f)
	{

		FLOAT fAngX = D3DXToRadian(pMover->GetAngleX());
		FLOAT fAng = D3DXToRadian(pMover->GetAngle());
		FLOAT fDist = cosf(fAngX) * fAccPwr;

		D3DXVECTOR3	vAcc;
		vAcc.x = sinf(fAng) * fDist;
		vAcc.z = -cosf(fAng) * fDist;
		vAcc.y = -sinf(fAngX) * fAccPwr;


		if (fLenSq > 0.01f)
		{
			D3DXVECTOR3 vDeltaNorm, vAccNorm;
			D3DXVec3Normalize(&vDeltaNorm, &m_vDelta);
			D3DXVec3Normalize(&vAccNorm, &vAcc);
			float fDot = D3DXVec3Dot(&vDeltaNorm, &vAccNorm);

			if (fDot < 0.633319f)
			{
				vAcc *= 2.0f;
				m_vDelta *= 0.985f;
#ifdef __CLIENT
				if (!(pMover->IsMode(TRANSPARENT_MODE)))
				{
					if ((g_nProcessCnt & 3) == 0)
						CreateSfx(g_Neuz.m_pd3dDevice, XI_NAT_DUST_RUN, pMover->GetPos());
				}
#endif
			}
		}

		fLenSq = D3DXVec3LengthSq(&m_vDelta);
		float fMaxSpeed = 0.3f;
		if (GetStateFlag() & OBJSTAF_TURBO)
			fMaxSpeed *= 1.5f;

		if (fLenSq < (fMaxSpeed * fMaxSpeed))
			m_vDelta += vAcc;
		else
			bMaxSpeed = TRUE;
	}

	if (!bMaxSpeed)
		m_vDelta *= (1.0f - FRIC_AIR);

	fLenSq = D3DXVec3LengthSq(&m_vDelta);
	if (m_fAccPower == 0.0f && fLenSq < 0.0002f * 0.0002f)
	{
		fLenSq = 0;
		m_vDelta = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		RemoveStateFlag(OBJSTAF_ACC);
	}

#ifdef __CLIENT
	if (pMover->IsActiveMover())
	{
		g_nFlySpeed = (int)((sqrt(fLenSq) * 60.0f) * 60.0f * 60.0f);
		g_nFlySpeed = (int)(g_nFlySpeed / 200.0f);
	}
#endif
}

void	CActionMover::ProcessState2(CMover* pMover, DWORD dwState, float fSpeed)
{
	int nParam = 0;
	float fAng = pMover->GetAngleX();
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	ItemProp* pItemProp = prj.GetItemProp(m_pMover->m_dwRideItemIdx);
	if (!pItemProp)
	{
		Error("ProcessState2(): %s, m_dwRideItemIdx == %d", m_pMover->m_szName, m_pMover->m_dwRideItemIdx);
		return;
	}

	switch (dwState)
	{

	case OBJSTA_STAND:
		m_fAccPower = 0;
		if (GetState() & OBJSTA_ATK_ALL)	break;
		if (GetState() & OBJSTA_TURN_ALL)	break;
		if (GetState() & OBJSTA_DMG_ALL)	break;
		{
			if (pMover->SetMotion(pItemProp->dwUseMotion + MTA_FSTAND1, ANILOOP_LOOP, MOP_FIXED) == TRUE)
				if (pMover->m_pRide)
					pMover->m_pRide->m_fFrameCurrent = 0;
		}
		break;

	case OBJSTA_FMOVE:

		m_fAccPower = pItemProp->fFlightSpeed * 0.75f;

		if (GetState() & OBJSTA_ATK_ALL)	break;
		if (GetState() & OBJSTA_TURN_ALL)	break;
		{
			if (pMover->SetMotion(pItemProp->dwUseMotion + MTA_FRUNNING1, ANILOOP_LOOP, MOP_FIXED) == TRUE)
				if (pMover->m_pRide)
					pMover->m_pRide->m_fFrameCurrent = 0;
		}
		break;

	case OBJSTA_WAIT:
		break;

	case OBJSTA_LTURN:
	{
		float fTurnAng = m_fTurnAngle;
		if ((GetStateFlag() & OBJSTAF_ACC) == 0)
			fTurnAng *= 2.5f;
		pMover->AddAngle(fTurnAng);
	}
	if (IsActAttack())	break;
	pMover->SetMotion(pItemProp->dwUseMotion + MTA_FLTURN1, ANILOOP_LOOP, MOP_FIXED);

	break;
	case OBJSTA_RTURN:
	{
		float fTurnAng = m_fTurnAngle;
		if ((GetStateFlag() & OBJSTAF_ACC) == 0)
			fTurnAng *= 2.5f;
		pMover->AddAngle(-fTurnAng);
	}
	if (IsActAttack())	break;
	pMover->SetMotion(pItemProp->dwUseMotion + MTA_FRTURN1, ANILOOP_LOOP, MOP_FIXED);
	break;
	case OBJSTA_LOOKUP:
		if (fAng > -45.0f)
			pMover->AddAngleX(-pItemProp->fFlightTBAngle);
		break;
	case OBJSTA_LOOKDOWN:
		if (fAng < 45.0f)
			pMover->AddAngleX(pItemProp->fFlightTBAngle);
		break;

	case OBJSTA_ATK1:
	case OBJSTA_ATK2:
	case OBJSTA_ATK3:
	case OBJSTA_ATK4:
		_ProcessStateAttack2(dwState, nParam);
		break;
	case OBJSTA_ATK_MAGIC1:
		ProcessStateAttackMagic(dwState, nParam);
		break;


	case OBJSTA_DMG:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_DMG_ALL);
		break;
	case OBJSTA_DEAD:
#ifdef __WORLDSERVER
		if (!pMover->IsPlayer())
		{
			SetState(OBJSTA_DMG_ALL, OBJSTA_DISAPPEAR);
			m_nCount = 0;
		}
#endif
		break;
	}
}




void	CActionMover::_ProcessStateAttack2(DWORD dwState, int nParam)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;

	switch (dwState)
	{
	case OBJSTA_ATK1:
		if (pModel->IsEndFrame())
			ResetState(OBJSTA_ATK_ALL);

		if (pModel->m_nPause > 0)
		{
			--pModel->m_nPause;
		}
		else
		{
			if (pModel->IsAttrHit())
			{
				CMover* pHitObj = prj.GetMover(GetObjHit());
				if (IsInvalidObj(pHitObj) || pHitObj->IsDie())
					return;

				BOOL bSuccess = pHitObj->m_pActMover->SendDamage(AF_GENERIC, pMover->GetId());
				if (bSuccess == FALSE)
					return;
#ifdef __CLIENT
				if (pMover->IsActiveMover())
				{
					if (pModel)
						pModel->m_nPause = 5;
				}

				ItemProp* pItemProp = pMover->GetActiveHandItemProp();
				if (pItemProp)
				{
					D3DXVECTOR3 v = pMover->GetPos();
					PLAYSND(pItemProp->dwSndAttack1, &v);
				}
#endif
			}
		}
		break;
	}
}
