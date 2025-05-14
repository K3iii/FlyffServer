#include "stdafx.h"

#include "Ship.h"


D3DXMATRIX g_matView;
D3DXMATRIX g_matInvView;

CCamera::CCamera()
{
	m_vPos = D3DXVECTOR3(50.0f, 110.0f, 50.0f);
	m_vLookAt = D3DXVECTOR3(m_vPos.x + 1.0f, 100.0f, m_vPos.z + 1.0f);
	m_vOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_nQuakeSec = 0;
	m_fQuakeSize = 0.06f;
	m_dwCamStyle = 0;
}
void CCamera::Process(LPDIRECT3DDEVICE9 pd3dDevice, float fFactor)
{
}
void CCamera::Transform(LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld)
{
	D3DXVECTOR3	vTempPos = m_vPos;
	if (m_nQuakeSec > 0)
	{
		float x = (m_fQuakeSize / 2.0f) - xRandomF(m_fQuakeSize);
		float y = (m_fQuakeSize / 2.0f) - xRandomF(m_fQuakeSize);
		float z = (m_fQuakeSize / 2.0f) - xRandomF(m_fQuakeSize);

		vTempPos.x += x;
		vTempPos.y += y;
		vTempPos.z += z;
		--m_nQuakeSec;
	}
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1.0f, 0);
	D3DXMatrixLookAtLH(&m_matView, &vTempPos, &m_vLookAt, &vUp);

	pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);
	pWorld->UpdateCullInfo(&m_matView, &pWorld->m_matProj);
	D3DXMatrixInverse(&m_matInvView, NULL, &m_matView);
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;
}
void CCamera::SetQuake(int nSec, float fSize)
{
	if (m_nQuakeSec <= 0)
	{
		m_nQuakeSec = nSec;
		m_fQuakeSize = fSize;
	}
}
void CCamera::Reset()
{

}
void CCamera::ControlCamera(DWORD dwStyle)
{
	m_dwCamStyle = dwStyle;
}
void CBackCamera::Lock()
{
#ifdef __CLIENT

	m_bLock = TRUE;
	if (g_WorldMng.Get())
	{
		CMover* pMoverTarget = (CMover*)g_WorldMng.Get()->GetObjFocus();
		if (!(pMoverTarget && CMover::GetActiveMover() && CMover::GetActiveMover()->m_pActMover->IsFly())) {
			FLOAT fAngle = CMover::GetActiveMover()->GetAngle();
			m_fRotx = -fAngle;
		}
	}
#endif
}
void CBackCamera::Unlock()
{
	m_bLock = FALSE;
	m_fRotx = 0;
	m_fRoty = 0;
	m_nCamMode = CM_NORMAL;
}

CBackCamera::CBackCamera()
{
	m_fZoom = 6.0f;
	m_bLock = FALSE;
	m_fRoty = -12.0f;

	m_fLength1 = 0.0f;
	m_fLength2 = 0.0f;
	m_bOld = FALSE;
	m_bStart = FALSE;
}
D3DXVECTOR3 m_vPosVal;
void CBackCamera::ControlCamera(DWORD dwStyle)
{
	return;
	CCamera::ControlCamera(dwStyle);
	D3DXVECTOR3 vPos = CMover::GetActiveMover()->GetPos();
	D3DXVECTOR3 vTemp = vPos - m_vPos;

	switch (dwStyle)
	{
	case CAMSTY_HIT:
		m_vPosVal = vTemp;
		break;

	case CAMSTY_DAMAGE:
		m_vOffset += vTemp;
		break;
	}
}

int  CBackCamera::GetAnglePie(FLOAT fAngle)
{
	FLOAT fAngTemp = fabs(fAngle);

	if (fAngTemp > 0.0f && fAngTemp <= 90.0f)
	{
		return 4;
	}

	if (fAngTemp > 90.0f && fAngTemp <= 180.0f)
	{
		return 3;
	}

	if (fAngTemp > 180.0f && fAngTemp <= 270.0f)
	{
		return 2;
	}

	if (fAngTemp > 270.0f && fAngTemp <= 360.0f)
	{
		return 1;
	}

	return 0;
}

#ifdef __HOUSING
BOOL CBackCamera::SetCamMode(int nType)
{

	m_nCamMode = nType;

	return m_nCamMode;
}
#endif

void CBackCamera::Process(LPDIRECT3DDEVICE9 pd3dDevice, float fFactor)
{
	CMover* pMover = CMover::GetActiveMover();
	if (!pMover)	
		return;

	CWorld* pWorld = pMover->GetWorld();
	if (!pWorld)
		return;

	D3DXVECTOR3 vPos = pMover->GetPos();
	vPos.y += 0.9f;

	FLOAT fAngle = pMover->GetAngle();
	if (m_bLock)
		fAngle = 0;
	FLOAT fAngleY = 90.0f;

	if (m_nCamMode == CM_MYROOM)
	{
		if (m_fZoom <= 0.5f)
			m_fZoom = 0.5f;
	}

	m_vLookAt = vPos;
	m_vLookAt.y += 0.4f;

	m_fCurRoty = m_fRoty + m_fZoom * 4;

	float fAdjAng = (1.0f - fAngleY / 90.0f) * 45.0f;
	m_fCurRoty += fAdjAng;
	m_fCurRoty += pMover->GetAngleX();

	if (m_fCurRoty > 80.0f)
		m_fCurRoty = 80.0f;

	if (m_nCamMode == CM_MYROOM)
	{
		if (m_fCurRoty <= 10.0f)
		{
			m_fCurRoty = 10.0f;
			if (m_fRoty > 0.0f)
				m_fRoty = 0.0f;
			if (m_fRoty < -30.0f)
				m_fRoty = -30.0f;
		}
	}

	m_fCurRotx = m_fRotx;

	fAngle = m_fCurRotx - fAngle + 180.0f;

	extern float fDiv;
	D3DXVECTOR3 vTemp;

	if (fDiv == 2.0f)
		vTemp = D3DXVECTOR3(0.0f, 0.0f, -0.0f - 2.0f);
	else
		vTemp = D3DXVECTOR3(0.0f, 0.0f, -4.0f - m_fZoom * 2.0f);

	D3DXMATRIX matTemp;
	D3DXMatrixRotationX(&matTemp, D3DXToRadian(m_fCurRoty / 1.0f));
	D3DXVec3TransformCoord(&vTemp, &vTemp, &matTemp);
	D3DXMatrixRotationY(&matTemp, D3DXToRadian(fAngle));
	D3DXVec3TransformCoord(&m_vOffsetDest, &vTemp, &matTemp);

	D3DXVECTOR3 vecOffsetDelta = ((m_vOffsetDest - m_vOffset) + m_vPosVal) / fFactor;

	m_vOffset += vecOffsetDelta;

	m_vPosVal /= 2;

	m_vPos = vPos + m_vOffset;

	FLOAT fLength;
	static D3DXVECTOR3 m_vLimitPos;
	D3DXVECTOR3 m_vOutPos = m_vPos;

	BOOL bCrash = pWorld->CheckBound(&m_vPos, &m_vLookAt, &m_vOutPos, &fLength);
	if (bCrash)
		m_fLength2 = fLength;

	if (m_bOld && bCrash == FALSE)
	{
		m_fLength1 = fLength;
		m_bStart = TRUE;
	}

	if (m_bStart)
	{
		D3DXVECTOR3 vCPos = vPos + m_vOffset;
		D3DXVECTOR3 vDir = vCPos - m_vLookAt;
		D3DXVec3Normalize(&vDir, &vDir);

		m_fLength2 += 0.37f;
		if (m_fLength2 > fLength)
			m_bStart = FALSE;

		m_vOutPos = m_vLookAt + (vDir * m_fLength2);
	}

	m_bOld = bCrash;

	g_ModelGlobal.SetCamera(m_vOutPos, m_vLookAt);

	m_vPos = m_vOutPos;
}
CToolCamera::CToolCamera()
{
	m_vPos = D3DXVECTOR3(0.0f, 110.0f, 0.0f);
	m_vVelocity = D3DXVECTOR3(0, 0, 0);
	m_fYaw = 0;
	m_fYawVelocity = 0;
	m_fPitch = 0;
	m_fPitchVelocity = 0;
	//m_matOrientation = 0;
}
void CToolCamera::Process(LPDIRECT3DDEVICE9 pd3dDevice, float fFactor)
{
}
void CToolCamera::Transform(LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld)
{
	D3DXQUATERNION qR;
	D3DXQuaternionRotationYawPitchRoll(&qR, m_fYaw, m_fPitch, 0.0f);
	//D3DXMatrixAffineTransformation( &m_matOrientation, 1.25f, NULL, &qR, &m_vPos );
	D3DXMatrixAffineTransformation(&m_matOrientation, 1.0f, NULL, &qR, &m_vPos);
	D3DXMatrixInverse(&m_matView, NULL, &m_matOrientation);
	pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);
	pWorld->UpdateCullInfo(&m_matView, &pWorld->m_matProj);
	D3DXMatrixInverse(&m_matInvView, NULL, &m_matView);
	m_matInvView._41 = 0.0f; m_matInvView._42 = 0.0f; m_matInvView._43 = 0.0f;
	g_matInvView = m_matInvView;
	g_matView = m_matView;

}
