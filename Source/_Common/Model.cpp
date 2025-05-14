#include "stdafx.h"
#include "Model.h"
#include "..\_DirectX\dxutil.h"


CModel::~CModel()
{
//	SAFE_DELETE( m_pBillboard );
	DeleteDeviceObjects();
}

void CModel::FrameMove(D3DXVECTOR3* pvSndPos, float fSpeed)
{
	if (m_nPause != 0 || m_nFrameMax <= 0)
		return;

	m_fFrameOld = m_fFrameCurrent;
	const float fAddFrame = m_bSlow == TRUE ? 0.1f : m_fPerSlerp * fSpeed;

	m_fFrameCurrent += fAddFrame;

	if (m_fFrameOld + fAddFrame > static_cast<float>(m_nFrameMax))
		m_fFrameOld -= static_cast<float>(m_nFrameMax);

	if (m_fFrameOld < 0.0f)
		m_fFrameOld = 0.0f;

	int nCurrFrame = static_cast<int>(m_fFrameCurrent);

	m_fSlp = m_fFrameCurrent - static_cast<float>(nCurrFrame);

	if (m_fFrameCurrent + fAddFrame > static_cast<float>(m_nFrameMax))
	{
		m_bEndFrame = TRUE;

		if (m_nLoop & ANILOOP_LOOP)
			nCurrFrame %= m_nFrameMax;
		else
		{
			m_fSlp = 0.0f;
			nCurrFrame = m_nFrameMax - 1;
		}
	}

	m_fFrameCurrent = static_cast<float>(nCurrFrame) + m_fSlp;
}

void CModel::AddFrame( FLOAT fAddFrame )
{
}
void CModel::SetFrame( FLOAT fFrame )
{
}
BOOL CModel::Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld )
{
	return TRUE;
}
BOOL CModel::IsAniable()
{
	if( m_nModelType == MODELTYPE_MESH || m_nModelType == MODELTYPE_BILLBOARD )
		return FALSE;
	return TRUE;
}




int	CModel::GetNextFrame( void )
{
	int		nCurrFrm = (int)m_fFrameCurrent;
	int		nNextFrm;

	nNextFrm = nCurrFrm + 1;
	if( nNextFrm >= m_nFrameMax )
	{
		if( m_nLoop & ANILOOP_LOOP )
			nNextFrm = 0;
		else
			nNextFrm = m_nFrameMax - 1;
	}

	return nNextFrm;
}


float CModel::GetRadius( void )
{
	D3DXVECTOR3 vLen = m_vMax - m_vMin;
	float	fLen = D3DXVec3Length( &vLen );
	return fLen / 2.0f;
}

#ifdef __CLIENT
BOOL CModel::IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist )
{
	static constexpr int anIntexTable[ 12 * 3 ] =
	{
		0, 1, 2,  0, 2, 3,
		0, 4, 5,  0, 5, 1,
		4, 5, 6,  4, 6, 7,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 2, 6,  1, 6, 5
	};

	D3DXVECTOR3 bbTransformedVertices[8];
	for (size_t i = 0; i != 8; ++i) 
		D3DXVec3TransformCoord(&i[bbTransformedVertices], i + m_BB.m_vPos, &mWorld);

	for (int i = 0; i < 12 * 3; i += 3)
	{
		const D3DXVECTOR3& v1 = bbTransformedVertices[anIntexTable[i + 0]];
		const D3DXVECTOR3& v2 = bbTransformedVertices[anIntexTable[i + 1]];
		const D3DXVECTOR3& v3 = bbTransformedVertices[anIntexTable[i + 2]];

		if (IntersectTriangle(v1, v2, v3, vRayOrig, vRayDir, pvIntersect, pfDist))
			return TRUE;
	}

	return FALSE;
}
#endif