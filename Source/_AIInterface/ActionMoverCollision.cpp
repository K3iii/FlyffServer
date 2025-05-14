#include "stdafx.h"
#include "ActionMover.h"
#include "DefineObj.h"
#include "SecretRoom.h"

BOOL CanCollisionFly(CMover* pMover)
{
#ifdef __CLIENT
	if (CObj::m_bCollision && (pMover->IsActiveMover() || pMover->IsEmptyDest()))
		return TRUE;
#else
	if (pMover->IsNPC() || pMover->IsEmptyDest())
		return TRUE;
#endif
	return FALSE;
}




BOOL CActionMover::ProcessCollisionFly(D3DXVECTOR3* pPos)
{
	if (m_fAccPower == 0.0f && m_vDeltaAccu == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
		return FALSE;

	CMover* pMover = m_pMover;
	D3DXVECTOR3	vStart = *pPos;
	D3DXVECTOR3 vDelta = m_vDeltaAccu;
	CWorld* pWorld = GetWorld();

	vStart.y += 0.5f;

	BOOL fResult = FALSE;
	if (CanCollisionFly(pMover))
	{
		D3DXVECTOR3 vDir = m_vDeltaAccu;
#ifdef __WORLDSERVER
		vDir *= 0.25f;
#endif
		float fLen = D3DXVec3Length(&vDir);
		if (fLen > 0.001f)
		{
			FLOAT fR = 1.0f;
			if (fLen < 0.65f)
			{
				fR = 0.65f / fLen;
				vDir *= fR;
			}
			fResult = pWorld->ProcessCollisionReflection(&vDelta, vStart, vDir, 0);
			if (fResult)
			{
				vDelta /= fR;
				vDelta *= 0.7f;
				m_vDelta = vDelta;
			}
		}
	}


	pPos->y += vDelta.y;


	pPos->x += vDelta.x;
	pWorld->ClipX(pPos->x);
	int nAttr = pWorld->GetHeightAttribute(pPos->x, pPos->z);
	switch (nAttr)
	{
	case HATTR_NOMOVE:
		pPos->x -= vDelta.x;
		break;
	}


	pPos->z += vDelta.z;
	pWorld->ClipZ(pPos->z);
	if (pWorld->m_bFly)
		nAttr = pWorld->GetHeightAttribute(pPos->x, pPos->z);
	else
		nAttr = HATTR_NOFLY;

	switch (nAttr)
	{
	case HATTR_NOMOVE:
		pPos->z -= vDelta.z;
		break;
	case HATTR_DIE:
		break;
	case HATTR_NOFLY:
		pPos->x -= vDelta.x;
		pPos->z -= vDelta.z;
		pPos->y -= vDelta.y;
#ifdef __WORLDSERVER
		pMover->DoEquipItem(pMover->GetEquipItem(PARTS_RIDE));
#endif
		break;
	}

	m_fCurrentHeight = pWorld->GetFullHeight(D3DXVECTOR3(pPos->x, pPos->y + 1.0f, pPos->z));

	if (pPos->y > pWorld->m_fMaxHeight)
		pPos->y = pWorld->m_fMaxHeight;
	else
	{
		LPWATERHEIGHT pWaterHeight = pWorld->GetWaterHeight(*pPos);
		if (pWaterHeight && pWaterHeight->byWaterTexture == WTYPE_CLOUD && m_fCurrentHeight == 0.0f)
		{
			if (pPos->y < pWorld->m_fMinHeight)
				pPos->y = pWorld->m_fMinHeight;
		}
		else
		{
			if (pPos->y < m_fCurrentHeight)
			{
				pPos->y = m_fCurrentHeight;
				fResult = TRUE;
			}
		}
	}

	return fResult;
}





BOOL CActionMover::ProcessCollisionGround(D3DXVECTOR3* pPos)
{
	CMover* pMover = m_pMover;
	CModelObject* pModel = (CModelObject*)pMover->m_pModel;
	D3DXVECTOR3		vDelta = m_vDeltaAccu;
	BOOL			fResult = FALSE;
	CWorld* pWorld = GetWorld();

	if (m_bMove)
	{
		D3DXVECTOR3	vKneePos, vDeltaXZ;

		vKneePos = *pPos;
		vKneePos.y += 0.4f;
		vDeltaXZ = vDelta;
		vDeltaXZ.y = 0;


		//gmpbigsun : collision test-------------------------------------------------------------------------------
		float COLL_TEST_DIR = 6.5f;
#ifdef __AIO_POSI_RELATED
		if (pMover->IsPlayer())
			COLL_TEST_DIR = 1.5f;
#endif

		vDeltaXZ *= COLL_TEST_DIR;
#ifdef __X15
		vDeltaXZ /= 4.0f;
#endif

		BOOL bShouldHaveColl = FALSE;
		if (pMover->IsPlayer())
			bShouldHaveColl = TRUE;
		else
		{
			MoverProp* pProp = pMover->GetProp();
			if (pProp && pProp->dwAI != AII_EGG && pProp->dwAI != AII_PET)
			{
				DWORD dwWorldID = pWorld->GetID();
				if (dwWorldID >= WI_DUNGEON_SECRET_1 && dwWorldID <= WI_DUNGEON_SECRET_L)
					bShouldHaveColl = TRUE;
				else if (dwWorldID == WI_DUNGEON_SECRET_0
					|| dwWorldID == WI_INSTANCE_SANPRES || dwWorldID == WI_INSTANCE_SANPRES_1
					|| dwWorldID == WI_INSTANCE_OMINOUS || dwWorldID == WI_INSTANCE_OMINOUS_1
					|| dwWorldID == WI_INSTANCE_BEHAMAH
					)
					bShouldHaveColl = TRUE;
			}
		}

		if (bShouldHaveColl)
		{
			float fLen = D3DXVec3Length(&vDeltaXZ);
			if (fLen != 0.0f)
			{
#ifdef __LAYER_1015
				fResult = pWorld->ProcessCollision(&vDeltaXZ, vKneePos, vDeltaXZ, 0, pMover->GetLayer());
#else	// __LAYER_1015
				fResult = pWorld->ProcessCollision(&vDeltaXZ, vKneePos, vDeltaXZ, 0);
#endif	// __LAYER_1015
			}
		}

#ifdef __X15
		vDeltaXZ *= 4.0f;
#endif
		vDeltaXZ /= COLL_TEST_DIR;

#if __VER >= 11 // __FIX_COLLISION
		if (fResult && (D3DXVec3Length(&vDeltaXZ) > 0.15f))
		{
			vDeltaXZ.y = m_vDeltaAccu.y = m_vDelta.y = 0;
			m_vDelta.x = vDeltaXZ.x;
			m_vDelta.z = vDeltaXZ.z;
		}
#endif
		BOOL bCollision = FALSE;


		pPos->x += vDeltaXZ.x;
		pWorld->ClipX(pPos->x);
		int nAttr = pWorld->GetHeightAttribute(pPos->x, pPos->z);
		switch (nAttr)
		{
		case HATTR_NOWALK:
		case HATTR_NOMOVE:
			pPos->x -= vDeltaXZ.x;
			bCollision = TRUE;
			break;
		}


		pPos->z += vDeltaXZ.z;
		pWorld->ClipZ(pPos->z);
		nAttr = pWorld->GetHeightAttribute(pPos->x, pPos->z);
		switch (nAttr)
		{
		case HATTR_NOWALK:
		case HATTR_NOMOVE:
			pPos->z -= vDeltaXZ.z;
			bCollision = TRUE;
			break;
		case HATTR_DIE:
			break;
		}

#ifdef __WORLDSERVER
		if (bCollision)
			pMover->PostAIMsg(AIMSG_COLLISION);
#endif
	} // if( m_bMove )

	if (GetDmgState() == OBJSTA_DISAPPEAR)
		return fResult;

	if (vDelta.y < 0.0f)
	{
		if (m_bGround == 1)
			return fResult;

		D3DXVECTOR3 vSlide(0.0f, 0.0f, 0.0f);
		CObj* pObjColl = NULL;
		FLOAT h = pWorld->ProcessUnderCollision(&vSlide, &pObjColl, D3DXVECTOR3(pPos->x, pPos->y + 1.0f, pPos->z));
		m_fCurrentHeight = h;

		pPos->y += vDelta.y;

		BOOL bLanding = FALSE;
		if (GetJumpState() == OBJSTA_SJUMP3)
		{
			if (pPos->y < h)
				bLanding = TRUE;
		}
		else
		{
			if (pPos->y - 0.25f > h)
			{
				if (vDelta.y < -0.1f)
				{
#ifndef __AIO_POSI_RELATED
					if ((GetState() & OBJSTA_COLLECT) == 0)
						if (pMover->IsPlayer() && !(GetDmgState() & OBJSTA_DMG_FLY_ALL))
							SetJumpState(OBJSTA_SJUMP3);
#endif

					pMover->OnActDrop();
			}
		}
			else
				bLanding = TRUE;
	}

		if (bLanding)
		{
#ifdef __JEFF_11_6
			BOOL bReturn = pMover->IsNPC() && pMover->GetSpeedFactor() > 1.9F;
#endif	// __JEFF_11_6
			if (vSlide.y
#ifdef __JEFF_11_6
				&& !bReturn
#endif	// __JEFF_11_6
				)
			{
				*pPos = vSlide;
				m_bGround = 2;
#ifdef __WORLDSERVER
				pMover->PostAIMsg(AIMSG_COLLISION);
#endif
			}
			else
			{
				pPos->y = h;
				m_bGround = 1;
			}

			m_vDelta.y = 0;
		}
}
	else if (vDelta.y > 0)
	{
		FLOAT h = 65535.0f;

		D3DXVECTOR3 vTmp = *pPos;
		vTmp.y += 1.5f;
		if (pMover->IsPlayer())
		{
			h = pWorld->GetOverHeightForPlayer(vTmp, pMover);
		}
		else
		{
			h = pWorld->GetOverHeight(vTmp, pMover);
		}

		pPos->y += vDelta.y;
		if (pPos->y + 1.5f > h)
		{
			pPos->y = h - 1.5f;
			m_vDelta.y = -0.01f;
			fResult = TRUE;
		}
	}

	return fResult;
}

