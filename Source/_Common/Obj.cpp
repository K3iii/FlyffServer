#include "stdafx.h"
#include "CreateObj.h"
#include "..\_AIInterface\AIInterface.h"
#ifdef __WORLDSERVER
#include "..\_AIInterface\AIMonster.h"
#include "..\_AIInterface\AIMonster2.h"
#include "..\_AIInterface\AIClockWorks.h"
#include "..\_AIInterface\AIPet.h"
#include "..\_AIInterface\AIBigMuscle.h"
#include "..\_AIInterface\AIKrrr.h"
#include "..\_AIInterface\AIBear.h"
#include "..\_AIInterface\aimeteonyker.h"
#endif // __WORLDSERVER

BOOL		CObj::m_bCollision = TRUE;
BOOL		CObj::m_bAnimate = TRUE;
CObj* CObj::m_pObjActive = NULL;
CObj* CObj::m_pObjHighlight = NULL;
int			CObj::m_nMethod = METHOD_NONE;


CObj::CObj()
#ifdef __LAYER_1015
	:
m_nLayer(nDefaultLayer)
#endif	// __LAYER_1015
{
	m_dwType = OT_OBJ;
	m_dwIndex = 0;
	m_dwFlags = 0;
	m_dwObjAryIdx = 0xffffffff;
	m_pModel = NULL;
	m_fAngle = 0.0f;
	m_fAngX = 0.0f;
	m_fAngZ = 0.0f;

	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pWorld = NULL;

	m_dwAIInterface = 0;
	m_pAIInterface = NULL;
	m_dwAIInterfaceState = 2;

#ifdef __WORLDSERVER
	m_dwPatrolIndex = NULL_ID;
	m_nPatrolIndexCount = 0;
	m_bPatrolCycle = 0;
	m_bPatrolReverse = 0;
#endif

	SetUpdateMatrix(TRUE);
	SetPosChanged(TRUE);

	m_dwLinkLevel = 0;
	SetDelete(FALSE);

	D3DXMatrixScaling(&m_matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationY(&m_matRotation, D3DXToRadian(-m_fAngle));
	D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
	D3DXMatrixIdentity(&m_matWorld);

#ifdef __WORLDSERVER
	m_vRemoval = m_vLink = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SetVirtual(FALSE);
	m_nAddObjsArrayIndex = -1;
#else	// !__WORLDSERVER
	m_ppViewPtr = NULL;
	m_cullstate = CS_UNKNOWN;
	m_wBlendFactor = 255;
	m_pCamera = NULL;
	m_fDistCamera = 0.0f;
	m_dy = 0.0f;
	m_nEvent = 0;
	SetVisible(TRUE);

#endif	// !__WORLDSERVER
}

CObj::~CObj()
{
	if (m_pModel && m_pModel->IsAniable())
		SAFE_DELETE(m_pModel);

	SAFE_DELETE(m_pAIInterface);
#ifndef __WORLDSERVER
	SAFE_DELETE(m_pCamera);
#endif	// __WORLDSERVER
}

void CObj::InitProp(BOOL bInitAI)
{
}

ObjProp* CObj::GetProp()
{
	return prj.GetProp(m_dwType, m_dwIndex);
}

void CObj::UpdateLocalMatrix(void)
{
	D3DXMatrixScaling(&m_matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixRotationYawPitchRoll(&m_matRotation, D3DXToRadian(-m_fAngle), D3DXToRadian(-m_fAngX), D3DXToRadian(m_fAngZ));

	D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
	UpdateMatrix();
}

void CObj::UpdateMatrix()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matScale);
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matRotation);
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTrans);

	if (m_pModel)
	{

		m_OBB.UpdateMartix(m_pModel->m_vMin, m_pModel->m_vMax, m_matScale, m_matRotation, m_matWorld);
	}

#ifdef __CLIENT
	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vecBoundsWorld[i], &m_vecBoundsLocal[i], &m_matWorld);

	D3DXPlaneFromPoints(&m_planeBoundsWorld[0], &m_vecBoundsWorld[0],
		&m_vecBoundsWorld[1], &m_vecBoundsWorld[2]); // Near
	D3DXPlaneFromPoints(&m_planeBoundsWorld[1], &m_vecBoundsWorld[6],
		&m_vecBoundsWorld[7], &m_vecBoundsWorld[5]); // Far
	D3DXPlaneFromPoints(&m_planeBoundsWorld[2], &m_vecBoundsWorld[2],
		&m_vecBoundsWorld[6], &m_vecBoundsWorld[4]); // Left
	D3DXPlaneFromPoints(&m_planeBoundsWorld[3], &m_vecBoundsWorld[7],
		&m_vecBoundsWorld[3], &m_vecBoundsWorld[5]); // Right
	D3DXPlaneFromPoints(&m_planeBoundsWorld[4], &m_vecBoundsWorld[2],
		&m_vecBoundsWorld[3], &m_vecBoundsWorld[6]); // Top
	D3DXPlaneFromPoints(&m_planeBoundsWorld[5], &m_vecBoundsWorld[1],
		&m_vecBoundsWorld[0], &m_vecBoundsWorld[4]); // Bottom
#endif // __CLIENT

	SetUpdateMatrix(FALSE);
}


void CObj::InsNextNode(CObj* pObj)
{
	if (pObj == this)
	{
		Error("CObj::InsNextNode() pObj == this");
	}

	pObj->m_pNext = m_pNext;
	pObj->m_pPrev = this;
	if (m_pNext)
		m_pNext->m_pPrev = pObj;
	m_pNext = pObj;
}

void CObj::DelNode()
{
	if (m_pPrev)
	{
		m_pPrev->m_pNext = m_pNext;
		if (m_pPrev->m_pNext == this)
			m_pPrev->m_pNext = NULL;
	}
	if (m_pNext)
	{
		m_pNext->m_pPrev = m_pPrev;
		if (m_pNext->m_pPrev == this)
			m_pNext->m_pPrev = NULL;
	}
	m_pNext = m_pPrev = NULL;
}

void CObj::Process()
{
#ifdef __CLIENT
	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem(m_dwType, m_dwIndex);
	if (lpModelElem == NULL)
		return;

	if (lpModelElem->m_dwModelType == MODELTYPE_SFX)
	{
		if (((CSfxModel*)m_pModel)->Process())
			((CSfxModel*)m_pModel)->m_nCurFrame = 0;
	}
	else
	{
		D3DXVECTOR3 vPos = GetPos();
		m_pModel->FrameMove(&vPos);
	}

	if (m_pModel->m_pModelElem->m_bTrans)
	{
		D3DXVECTOR3 vPosObj, vPosAct, vPosCam;
		float fLengthObj = 1.0f;
		float fLengthAct = 1.0f;
		if (GetActiveObj() != NULL)
		{
			vPosCam = m_pWorld->m_pCamera->m_vPos;
			vPosObj = GetPos() - vPosCam;
			vPosAct = GetActiveObj()->GetPos() - vPosCam;
			fLengthObj = D3DXVec3LengthSq(&vPosObj);
			fLengthAct = D3DXVec3LengthSq(&vPosAct);
		}

		if (fLengthObj < fLengthAct)
		{

			FLOAT fDist;
			D3DXVECTOR3 vIntersect;
			D3DXVECTOR3 vPickRayDir = vPosAct;
			D3DXVec3Normalize(&(vPickRayDir), &(vPickRayDir));
			if (Pick(&vPosCam, &vPickRayDir, &vIntersect, &fDist, FALSE, FALSE))
			{
				m_wBlendFactor -= 10;
				if (m_wBlendFactor < 64) m_wBlendFactor = 64;
			}
			else
			{
				m_wBlendFactor += 10;
				if (m_wBlendFactor > 255) m_wBlendFactor = 255;
			}
		}
		else
		{
			m_wBlendFactor += 5;
			if (m_wBlendFactor > 255) m_wBlendFactor = 255;
		}
	}
#endif // __CLIENT
}

void CObj::ProcessAI()
{
	if (m_pAIInterface && GetType() == OT_MOVER)
		m_pAIInterface->RouteMessage();
}

void CObj::SetOnLand()
{
	if (GetModel()->m_pModelElem->m_bFly == FALSE)
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y = m_pWorld->GetLandHeight(GetPos());
		SetPos(vPos);
	}
}


void CObj::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifndef __WORLDSERVER
	if (!IsVisible() || (IsCull() && GetType() != 1))
		return;

	if (IsUpdateMatrix())
		UpdateMatrix();

	LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem(m_dwType, m_dwIndex);
	if (lpModelElem == NULL)
		return;
	if (lpModelElem->m_dwModelType == MODELTYPE_SFX)
	{
		((CSfxModel*)m_pModel)->m_vPos = GetPos();
		((CSfxModel*)m_pModel)->m_vPos.y += 0.2f;
		((CSfxModel*)m_pModel)->m_vRotate.y = GetAngle();
		((CSfxModel*)m_pModel)->m_vScale = GetScale();
		((CSfxModel*)m_pModel)->m_matScale = m_matScale;
		((CSfxModel*)m_pModel)->Render(pd3dDevice, NULL);
		return;
	}

	CModel* pModel = m_pModel;
	DWORD dwModelType = pModel->GetModelType();

	pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	int wBlendFactor = m_wBlendFactor;
	if (m_pModel->m_nNoEffect)
		wBlendFactor = 255;

	m_pModel->SetBlendFactor(wBlendFactor);

	if (pModel->m_nNoEffect == 0)
	{
		if (m_pWorld->m_bIsIndoor == 1)
			pModel->SetGroup(0);
		else
			pModel->SetGroup(g_Option.m_nObjectDetail);
	}
	else
		pModel->SetGroup(2);

	if (pModel->GetType() == MODELTYPE_MESH || pModel->GetType() == MODELTYPE_ANIMATED_MESH)
	{
		if (((CModelObject*)pModel)->m_pModelElem->m_nTextureEx > 0)
			((CModelObject*)pModel)->SetTextureEx(((CModelObject*)pModel)->m_pModelElem->m_nTextureEx);
		else
			((CModelObject*)pModel)->SetTextureEx(0);
	}

	pModel->Render(pd3dDevice, &m_matWorld);
#endif
}


CModel* CObj::LoadModel(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex)
{
	return prj.m_modelMng.LoadModel(pd3dDevice, dwType, dwIndex);
}

void CObj::ResetScale()
{
	m_vScale.x = m_pModel->m_pModelElem->m_fScale;
	m_vScale.y = m_pModel->m_pModelElem->m_fScale;
	m_vScale.z = m_pModel->m_pModelElem->m_fScale;
}

BOOL CObj::SetTypeIndex(LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex, BOOL bInitProp)
{
	m_dwIndex = dwIndex;
	m_pModel = LoadModel(pd3dDevice, dwType, dwIndex);
	if (m_pModel)
	{
		if (bInitProp)
		{
			ResetScale();
			UpdateBoundBox();
			InitProp();
		}
		else
		{
#ifdef __WORLDSERVER
			if (dwType != OT_OBJ)
				ResetScale();
#endif //__WORLDSERVER
			UpdateBoundBox();
		}

		return TRUE;
		}

	return FALSE;
	}

BOOL CObj::UpdateBoundBox()
{
	if (m_pModel == NULL)
		return FALSE;

	const BOUND_BOX* pBB = m_pModel->GetBBVector();
	if (pBB)
	{
#ifdef __CLIENT
		m_vecBoundsLocal[0] = D3DXVECTOR3(m_pModel->m_vMax.x, m_pModel->m_vMax.y, m_pModel->m_vMax.z);
		m_vecBoundsLocal[1] = D3DXVECTOR3(m_pModel->m_vMax.x, m_pModel->m_vMax.y, m_pModel->m_vMin.z);
		m_vecBoundsLocal[2] = D3DXVECTOR3(m_pModel->m_vMax.x, m_pModel->m_vMin.y, m_pModel->m_vMax.z);
		m_vecBoundsLocal[3] = D3DXVECTOR3(m_pModel->m_vMax.x, m_pModel->m_vMin.y, m_pModel->m_vMin.z);
		m_vecBoundsLocal[4] = D3DXVECTOR3(m_pModel->m_vMin.x, m_pModel->m_vMax.y, m_pModel->m_vMax.z);
		m_vecBoundsLocal[5] = D3DXVECTOR3(m_pModel->m_vMin.x, m_pModel->m_vMax.y, m_pModel->m_vMin.z);
		m_vecBoundsLocal[6] = D3DXVECTOR3(m_pModel->m_vMin.x, m_pModel->m_vMin.y, m_pModel->m_vMin.z);
		m_vecBoundsLocal[7] = D3DXVECTOR3(m_pModel->m_vMin.x, m_pModel->m_vMin.y, m_pModel->m_vMax.z);
#endif // __CLIENT
		UpdateLocalMatrix();
	}

#if !defined(__WORLDSERVER)
	CalcLinkLevel();
#endif	// !defined(__WORLDSERVER)

	return TRUE;
}


#ifdef __BS_ADDOBJATTR_INVISIBLE
enum FD_OBJ_ATTR
{
	FOA_Invisible_Collision = 0x00010000,
	FOA_Invisible = 0x00020000,
};
#endif

BOOL CObj::Read(CFileIO* pFile)
{
	D3DXVECTOR3		vAxis;
	DWORD			dwMotion;
	DWORD			dwAIIntreface;
	DWORD			dwAI2;

	pFile->Read(&m_fAngle, sizeof(m_fAngle));
	pFile->Read(&vAxis, sizeof(vAxis));
	pFile->Read(&m_vPos, sizeof(m_vPos));
	pFile->Read(&m_vScale, sizeof(m_vScale));
	pFile->Read(&m_dwType, sizeof(m_dwType));



#ifdef __BS_ADDOBJATTR_INVISIBLE
	if (m_dwType & FOA_Invisible_Collision)
	{
		m_dwType &= ~FOA_Invisible_Collision;
		SetVisible(FALSE);
	}

	if (m_dwType & FOA_Invisible)
	{
		m_dwType &= ~FOA_Invisible;
		SetVisible(FALSE);
	}
#endif

	pFile->Read(&m_dwIndex, sizeof(m_dwIndex));
	pFile->Read(&dwMotion, sizeof(dwMotion));
	pFile->Read(&dwAIIntreface, sizeof(dwAIIntreface));
	pFile->Read(&dwAI2, sizeof(dwAI2));

	m_fAngX = vAxis.x;
	m_fAngZ = vAxis.z;

	InitProp(FALSE);
	if (m_dwType == OT_MOVER)
	{
		((CMover*)this)->m_dwMotion = dwMotion;
#ifdef __WORLDSERVER
		SetAIInterface(dwAIIntreface, dwAI2);
#endif
	}

	m_vPos.x *= FLOAT(OLD_MPU);
	m_vPos.z *= FLOAT(OLD_MPU);

#ifdef __WORLDSERVER
	m_vLink = m_vPos;
#endif	// __WORLDSERVER

	return TRUE;
	}

void CObj::SetPos(const D3DXVECTOR3& vPos)
{
	if (m_vPos == vPos)
		return;

	SetPosChanged(TRUE);

	CWorld* pWorld = GetWorld();
#ifdef __WORLDSERVER
	if (pWorld)
	{
		if (pWorld->VecInWorld(vPos) == FALSE)
			return;

		D3DXVECTOR3	vOld = GetLinkPos() / (float)(pWorld->m_iMPU);
		D3DXVECTOR3 vCur = vPos / (float)(pWorld->m_iMPU);
		if ((int)vOld.x != (int)vCur.x || (int)vOld.z != (int)vCur.z)
		{
			if (m_vRemoval == D3DXVECTOR3(0, 0, 0))
			{
				pWorld->m_apModifyLink[pWorld->m_cbModifyLink++] = this;
				if (pWorld->m_cbModifyLink >= MAX_MODIFYLINK)
					Error("MODIFYLINK//MAX//%d", pWorld->m_cbModifyLink);
			}

			m_vRemoval = vPos;
			m_vPos = vPos;
		}
		else
		{
			m_vPos = vPos;
			m_vLink = vPos;

			if (m_vRemoval != D3DXVECTOR3(0, 0, 0))
			{
				for (int i = 0; i < pWorld->m_cbModifyLink; i++)
				{
					if (pWorld->m_apModifyLink[i] == this)
					{
						pWorld->m_apModifyLink[i] = NULL;
						m_vRemoval = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
						break;
					}
				}
			}
		}
	}
	else
	{
		m_vPos = vPos;
		m_vLink = m_vPos;
	}
#else	// __WORLDSERVER
	if (pWorld)
	{
		if (pWorld->VecInWorld(vPos) == FALSE)
			return;

		D3DXVECTOR3	vOld = GetPos() / (FLOAT)(MPU);
		D3DXVECTOR3 vCur = vPos / (FLOAT)(MPU);
		if ((int)vOld.x != (int)vCur.x || (int)vOld.z != (int)vCur.z)
		{
			int nOldX = int(vOld.x / MAP_SIZE);
			int nOldZ = int(vOld.z / MAP_SIZE);
			int nCurX = int(vCur.x / MAP_SIZE);
			int nCurZ = int(vCur.z / MAP_SIZE);
			if (nOldX != nCurX || nOldZ != nCurZ)
			{
				CLandscape* pOldLandscape = pWorld->m_apLand[nOldX + nOldZ * pWorld->m_nLandWidth];
				if (NULL != pOldLandscape)
				{
					pOldLandscape->RemoveObjLink(this);
					pOldLandscape->RemoveObjArray(this);
				}
				m_vPos = vPos;
				CLandscape* pCurLandscape = pWorld->m_apLand[nCurX + nCurZ * pWorld->m_nLandWidth];
				if (NULL != pCurLandscape)
				{
					pCurLandscape->InsertObjLink(this);
					pCurLandscape->AddObjArray(this);
				}
			}
			else
			{

				int nWidth = 0;
				if (m_pModel)
					nWidth = (int)(m_pModel->GetMaxWidth());
				DWORD dwLinkType = GetLinkType();
				if (pWorld->GetObjInLinkMap(m_vPos, dwLinkType, m_dwLinkLevel) == this)
					pWorld->SetObjInLinkMap(m_vPos, dwLinkType, m_dwLinkLevel, m_pNext);
				DelNode();
				CObj* pObj = pWorld->GetObjInLinkMap(vPos, dwLinkType, m_dwLinkLevel);
				if (pObj)
					pObj->InsNextNode(this);
				else
					pWorld->SetObjInLinkMap(vPos, dwLinkType, m_dwLinkLevel, this);
				m_vPos = vPos;
			}
		}
		else
		{
			m_vPos = vPos;
		}
	}
	else
	{
		m_vPos = vPos;
	}
#endif	// __WORLDSERVER

#ifndef __CLIENT
	D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
	SetUpdateMatrix(TRUE);
#else
	if (GetType() != OT_MOVER)
	{
		D3DXMatrixTranslation(&m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
		SetUpdateMatrix(TRUE);
}
#endif

	return;
	}

void CObj::AddAngle(FLOAT fAngle)
{
	SetAngle(m_fAngle + fAngle);
}

void CObj::SetAngle(FLOAT fAngle)
{
	m_fAngle = (float)((360.0 / 65536) * ((int)(fAngle * (65536 / 360.0)) & 65535));

	FLOAT fAngZ = 0.0f;
	fAngZ = -m_fAngZ;

#if !defined(__CLIENT)
	D3DXMatrixRotationYawPitchRoll(&m_matRotation, D3DXToRadian(-m_fAngle), D3DXToRadian(-m_fAngX), D3DXToRadian(fAngZ));
	SetUpdateMatrix(TRUE);
#else


	if (GetType() != OT_MOVER)
	{
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, D3DXToRadian(-m_fAngle), D3DXToRadian(-m_fAngX), D3DXToRadian(fAngZ));
		SetUpdateMatrix(TRUE);
	}
#endif
}


void CObj::AddAngleX(FLOAT fAngle)
{
	m_fAngX += fAngle;
	SetAngleX(m_fAngX);
}

void CObj::SetAngleX(FLOAT fAngX)
{
	if (((CMover*)this)->IsPlayer() && ((CMover*)this)->IsFly() == FALSE)
		fAngX = 0;

	m_fAngX = fAngX;

	FLOAT fAngZ = 0.0f;
	fAngZ = -m_fAngZ;

#if !defined(__CLIENT)
	D3DXMatrixRotationYawPitchRoll(&m_matRotation, D3DXToRadian(-m_fAngle), D3DXToRadian(-m_fAngX), D3DXToRadian(fAngZ));
	SetUpdateMatrix(TRUE);
#else
	if (GetType() != OT_MOVER)
	{
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, D3DXToRadian(-m_fAngle), D3DXToRadian(-m_fAngX), D3DXToRadian(fAngZ));
		SetUpdateMatrix(TRUE);
}
#endif	
}

void CObj::SetScale(const D3DXVECTOR3& vScale)
{
	m_vScale = vScale;
	D3DXMatrixScaling(&m_matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	SetUpdateMatrix(TRUE);
}

#ifdef __WORLDSERVER
void CObj::SetAIInterface(DWORD dwAIInterface, DWORD dwState)
{
	if (m_dwAIInterface == dwAIInterface && m_dwAIInterfaceState == dwState)
		return;

	SAFE_DELETE(m_pAIInterface);
	m_pAIInterface = NULL;

	switch (dwAIInterface)
	{
	case AII_MONSTER:		m_pAIInterface = new CAIMonster(this);	break;
	case AII_CLOCKWORKS:	m_pAIInterface = new CAIClockWorks(this);	break;
	case AII_PET:			m_pAIInterface = new CAIPet(this);		break;
	case AII_BIGMUSCLE:		m_pAIInterface = new CAIBigMuscle(this);	break;
	case AII_KRRR:			m_pAIInterface = new CAIKrrr(this);		break;
	case AII_BEAR:			m_pAIInterface = new CAIBear(this);		break;
	case AII_VER2_TYPE0:	m_pAIInterface = new CAIMonster2(this);	break;
	case AII_METEONYKER:	m_pAIInterface = new CAIMeteonyker(this);	break;
	}

	m_dwAIInterface = dwAIInterface;
	m_dwAIInterfaceState = dwState;
}
#endif // __WORLDSERVER


void CObj::Delete()
{
	if (m_pWorld)
		m_pWorld->DeleteObj(this);
}

#ifdef __CLIENT
BOOL CObj::Pick(D3DXVECTOR3* pvPickRayOrig, D3DXVECTOR3* pvPickRayDir, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bOnlyBoundBox, BOOL bColl)
{
	if (!m_pModel->IntersectBB(*pvPickRayOrig, *pvPickRayDir, GetMatrixWorld(), pvIntersect, pfDist))
		return FALSE;

	if (bOnlyBoundBox == TRUE)
		return TRUE;

	return m_pModel->Intersect(*pvPickRayOrig, *pvPickRayDir, GetMatrixWorld(), pvIntersect, pfDist, bColl);
}
#endif

FLOAT	CObj::GetRadiusXZ(void)
{
	float	fLenX = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	float	fLenZ = fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);

	float	fRadius = ((fLenX + fLenZ) / 2.0f) / 2.0f;
	fRadius *= m_vScale.x;

	return fRadius;
}

const float INV_6 = 1.0f / 6.0f;

FLOAT	CObj::GetRadius(void)
{
	float fRadius = fabs(m_pModel->m_vMax.x - m_pModel->m_vMin.x);
	fRadius += fabs(m_pModel->m_vMax.y - m_pModel->m_vMin.y);
	fRadius += fabs(m_pModel->m_vMax.z - m_pModel->m_vMin.z);

	fRadius *= INV_6;
	fRadius *= m_vScale.x;

	return fRadius;
}

BOOL	CObj::IsRangeObj(CObj* pOther, float fRange)
{
	if (m_pModel == NULL)
		return FALSE;

	if (fRange == 0.0f)
		fRange = 1.0f;

	float fRadius = GetRadius();
	fRadius *= 0.8f;

	float fOther = pOther->GetRadius();
	fOther *= 0.8f;

	fRadius += (fOther + fRange);

	D3DXVECTOR3 vDist = GetPos() - pOther->GetPos();
	float fDistSq = D3DXVec3LengthSq(&vDist);

	if (fDistSq < (fRadius * fRadius))
		return TRUE;

	return FALSE;
}


BOOL	CObj::IsRangeObj(const D3DXVECTOR3& vDest, float fRange)
{
	if (m_pModel == NULL)	return FALSE;

	float	fRadius = 1.0f;
	fRadius *= 0.8f;

	float fRadiusSrc = GetRadius();
	fRadiusSrc *= 0.8f;

	fRadiusSrc += fRange;
	fRadius += fRadiusSrc;

	D3DXVECTOR3 vDist = vDest - GetPos();
	float fDistSq = D3DXVec3LengthSq(&vDist);

	if (fDistSq < fRadius * fRadius)
		return TRUE;

	return FALSE;
}

DWORD CObj::GetLinkType(void)
{
	switch (m_dwType)
	{
	case OT_OBJ:
		return CObj::linkStatic;
	case OT_MOVER:
		if (((CMover*)this)->IsPlayer() == TRUE)
			return CObj::linkPlayer;
	case OT_CTRL:
	case OT_SFX:
	case OT_ITEM:
		return CObj::linkDynamic;
	}

	Error("CObj::GetLinkType() -1 for %d m_dwType, %d m_dwIndex, %d world, %d x, %d z", m_dwType, m_dwIndex, GetWorld() ? GetWorld()->GetID() : 0, static_cast<int>(GetPos().x), static_cast<int>(GetPos().z));
	return static_cast<DWORD>(-1);
}

void CObj::RenderName(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor)
{
	return;
}

void CObj::SetWorld(CWorld* pWorld)
{
	m_pWorld = pWorld;
#ifdef __WORLDSERVER
	if (m_pWorld == NULL)
		return;

	CalcLinkLevel();
#endif //  __WORLDSERVER
}


void CObj::CalcLinkLevel()
{
	if (m_pModel == NULL)
	{
		m_dwLinkLevel = 0;
		return;
	}

	float fThisWidth = 0.0f;
	if (m_pModel->IsNullBoundBox() == FALSE)
	{
		D3DXMATRIX mWorld = GetMatrixWorld();
		D3DXVECTOR4 vMin, vMax;
		D3DXVec3Transform(&vMin, &m_pModel->m_vMin, &mWorld);
		D3DXVec3Transform(&vMax, &m_pModel->m_vMax, &mWorld);
		fThisWidth = (fabs(vMax.x - vMin.x) > fabs(vMax.z - vMin.z)) ? vMax.x - vMin.x : vMax.z - vMin.z;
		fThisWidth = fabs(fThisWidth);
	}

#ifdef __WORLDSERVER
#ifdef __LAYER_1015
	m_dwLinkLevel = m_pWorld->m_linkMap.CalcLinkLevel(this, fThisWidth, GetLayer());
#else	// __LAYER_1015
	m_dwLinkLevel = m_pWorld->m_linkMap.CalcLinkLevel(this, fThisWidth);
#endif	// __LAYER_1015
#else

	int nWidthLink, nWidthPerGrid;
	nWidthLink = MAP_SIZE * 2;
	int i = NULL;
	for (; i < MAX_LINKLEVEL; i++)
	{
		nWidthLink /= 2;
		if (nWidthLink == 0)
			nWidthLink = 1;

		nWidthPerGrid = MAP_SIZE * MPU / nWidthLink;
		if (fThisWidth < (float)nWidthPerGrid)
			break;
	}

	if (i == MAX_LINKLEVEL)
		i = MAX_LINKLEVEL - 1;

#ifdef __FL_FIX_LINKMAP_LEVEL
	const DWORD dwOldLinkLevel = m_dwLinkLevel;
	m_dwLinkLevel = i;

	if (dwOldLinkLevel != m_dwLinkLevel)
	{
		CWorld* pWorld = GetWorld();
		if (pWorld)
		{
			DWORD dwLinkType = GetLinkType();
			if (pWorld->GetObjInLinkMap(m_vPos, dwLinkType, dwOldLinkLevel) == this)
				pWorld->SetObjInLinkMap(m_vPos, dwLinkType, dwOldLinkLevel, m_pNext);
			DelNode();
			CObj* pObj = pWorld->GetObjInLinkMap(m_vPos, dwLinkType, m_dwLinkLevel);
			if (pObj)
				pObj->InsNextNode(this);
			else
				pWorld->SetObjInLinkMap(m_vPos, dwLinkType, m_dwLinkLevel, this);
		}
	}
#else
	m_dwLinkLevel = i;
#endif

#endif // !__WORLDSERVER
}