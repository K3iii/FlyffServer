#include "stdafx.h"
#include <stdio.h>
#include "model.h"
#include "Bone.h"



CBonesMng		g_BonesMng;

//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////
CBones :: CBones()
{
	Init();
}

CBones :: ~CBones()
{
	Destroy();
}

void	CBones :: Init( void )
{
	m_nID = 0;
	memset( m_szName, 0, sizeof(m_szName) );
	m_nMaxBone = 0;
	m_pBones = NULL;
	m_nRHandIdx = m_nLHandIdx = m_nRArmIdx = m_nLArmIdx = 0;

	D3DXMatrixIdentity( &m_mLocalRH );
	D3DXMatrixIdentity( &m_mLocalLH );
	D3DXMatrixIdentity( &m_mLocalShield );
	D3DXMatrixIdentity( &m_mLocalKnuckle );

	memset( m_vEvent, 0, sizeof(m_vEvent) );
//	memset( m_pEventParent, 0, sizeof(m_pEventParent) );
	memset( m_nEventParentIdx, 0, sizeof(m_nEventParentIdx) );

}

void	CBones :: Destroy( void )
{
	SAFE_DELETE_ARRAY( m_pBones );
	Init();
}




/*
BONE	*CBones :: FindBone( int nID )
{
	int		i;
	BONE	*pBone = m_pBones;

	i = m_nMaxBone;
	while( i-- )
	{
		if( pBone->m_nID == nID )	return pBone;
		pBone ++;
	}

	return NULL;
}
*/




int		CBones :: LoadBone( LPCTSTR szFileName )
{
	int		nNumBone;
	int		i;
	int		nLen;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )
		return	FAIL;

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer < VER_BONE )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_BONE );
		resFp.Close();
		return FAIL;
	}


	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}


	strcpy( m_szName, szFileName );

	resFp.Read( &nNumBone, 4, 1 );
	m_nMaxBone = nNumBone;
	m_pBones = new BONE[ nNumBone ];
	memset( m_pBones, 0, sizeof(BONE) * nNumBone );		// zero clear

	for( i = 0; i < nNumBone; i ++ )
	{
		resFp.Read( &nLen, 4, 1 );
		resFp.Read( m_pBones[i].m_szName,		nLen, 1 );
		resFp.Read( &m_pBones[i].m_mTM,			sizeof(D3DXMATRIX), 1 );			// World(Object) TM
		resFp.Read( &m_pBones[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		resFp.Read( &m_pBones[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		resFp.Read( &m_pBones[i].m_nParentIdx,	4, 1 );								// parent bone index
		if( strcmpi( m_pBones[i].m_szName + 6, "r hand") == 0 )
			m_nRHandIdx = i;
		if( strcmpi( m_pBones[i].m_szName + 6, "l hand") == 0 )
			m_nLHandIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "l forearm") == 0 )
			m_nLArmIdx = i;
		if( strcmpi(m_pBones[i].m_szName + 6, "r forearm") == 0 )
			m_nRArmIdx = i;
	}

	resFp.Read( &m_bSendVS, 4, 1 );


	resFp.Read( &m_mLocalRH, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalShield, sizeof(D3DXMATRIX), 1 );
	resFp.Read( &m_mLocalKnuckle, sizeof(D3DXMATRIX), 1 );


//	int		nParentIdx[4];
	if( nVer == 5 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * 4, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * 4, 1 );
	} else
	if( nVer >= 6 )
	{
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * MAX_MDL_EVENT, 1 );
		resFp.Read( m_nEventParentIdx, sizeof(int) * MAX_MDL_EVENT, 1 );
	}

	if( nVer == 7 )
	{
		resFp.Read( &m_mLocalLH, sizeof(D3DXMATRIX), 1 );
	}


//	for( i = 0; i < 4; i ++ )
//	{
//		m_pEventParent[i] = &m_pBones[ nParentIdx[i] ];
//	}

	resFp.Close();


	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBones[i].m_nParentIdx == -1 )
			m_pBones[i].m_pParent = NULL;
		else
			m_pBones[i].m_pParent = &m_pBones[ m_pBones[i].m_nParentIdx ];
	}

	return SUCCESS;
}







//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////
CBonesMng :: CBonesMng()
{
}

CBonesMng :: ~CBonesMng()
{
	Destroy();
}

void	CBonesMng :: Init( void )
{
}

void	CBonesMng :: Destroy( void )
{
	for( map<string, CBones*>::iterator i = m_mapBones.begin(); i != m_mapBones.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapBones.clear();
}

CBones *CBonesMng :: LoadBone( LPCTSTR szFileName )
{
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

	map<string, CBones*>::iterator i	= m_mapBones.find( sFile );
	if( i != m_mapBones.end() )
		return i->second;

	CBones* pBones	= new CBones;
	if( pBones->LoadBone( szFileName ) == FAIL )
	{
		SAFE_DELETE(pBones);
		return NULL;
	}
	bool bResult = m_mapBones.insert(map<string, CBones*>::value_type(sFile, pBones)).second;
	if (bResult == false)
	{
		SAFE_DELETE(pBones);
		return NULL;
	}

	return pBones;
}

//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////
CMotionMng		g_MotionMng;


CMotionMng :: CMotionMng()
{

}

CMotionMng :: ~CMotionMng()
{
	Destroy();
}

void	CMotionMng :: Init( void )
{

}

void	CMotionMng :: Destroy( void )
{
	for( map<string, CMotion*>::iterator i = m_mapMotions.begin(); i != m_mapMotions.end(); ++i )
		SAFE_DELETE( i->second );
	m_mapMotions.clear();
}

CMotion *CMotionMng :: LoadMotion( LPCTSTR szFileName )
{
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

	map<string, CMotion*>::iterator i	= m_mapMotions.find( sFile );
	if( i != m_mapMotions.end() )
		return i->second;


	CMotion* pMotion	= new CMotion;
	if( pMotion->LoadMotion( szFileName ) == FAIL )
	{
		SAFE_DELETE(pMotion);
		return NULL;
	}
	bool bResult	= m_mapMotions.insert( map<string, CMotion*>::value_type( sFile, pMotion ) ).second;
	if (!bResult)
	{
		SAFE_DELETE(pMotion);
		return NULL;
	}
	return pMotion;
}


//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////

CMotion :: CMotion()
{
	Init();
}

CMotion :: ~CMotion()
{
	Destroy();
}

void	CMotion :: Init( void )
{
	m_pMotion = NULL;
	m_pPath = NULL;
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

void	CMotion :: Destroy( void )
{
	int		i;

	SAFE_DELETE_ARRAY( m_pAttr );
	SAFE_DELETE_ARRAY( m_pMotion );
	SAFE_DELETE_ARRAY( m_pPath );
	if( m_pBoneFrame )
	{
		for( i = 0; i < m_nMaxBone; i ++ )
			m_pBoneFrame[i].m_pFrame = NULL;
	}
	SAFE_DELETE_ARRAY( m_pBoneFrame );
	SAFE_DELETE_ARRAY( m_pBoneInfo );

	Init();
}




int		CMotion :: LoadMotion( LPCTSTR szFileName )
{
	int		nNumBone;
	int		nNumFrame;
//	int		nNumSize;
//	int		i, j;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )
	{
		LPCTSTR szStr = Error( "%s : 찾을 수 없음", szFileName );
		//ADDERRORMSG( szStr );
		resFp.Close();
		return FAIL;
	}

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer != VER_MOTION )
	{
		Error( "%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MOTION );
		resFp.Close();
		return FAIL;
	}


	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		Error( "%s : file name is too long", szFileName );
		resFp.Close();
		return FAIL;
	}


	strcpy( m_szName, szFileName );

	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );
	resFp.Seek( 32, SEEK_CUR );		// reserved

	resFp.Read( &nNumBone, 4, 1 );
	resFp.Read( &nNumFrame, 4, 1 );
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if( nNumFrame <= 0 )
	{
		Error( "CMotion::LoadMotion : %s read MaxFrame = %d", szFileName, nNumFrame );
		resFp.Close();
		return FAIL;
	}

	// path
	int nTemp;
	resFp.Read( &nTemp, 4, 1 );
	if( nTemp )
	{
		m_pPath = new D3DXVECTOR3[ nNumFrame ];
		resFp.Read( m_pPath, sizeof(D3DXVECTOR3) * nNumFrame, 1 );
	}



	ReadTM( &resFp, nNumBone, nNumFrame );


	resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1 );

	resFp.Read( &m_nMaxEvent, 4, 1 );
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );


	resFp.Close();



	return SUCCESS;
}




void	CMotion :: ReadTM( CResFile *file, int nNumBone, int nNumFrame )
{
	int		nNumSize;
	int		nFrame;
	int		i;

	m_nMaxBone = nNumBone;
	m_nMaxFrame = nNumFrame;


	m_pBoneInfo = new BONE[ nNumBone ];
	memset( m_pBoneInfo, 0, sizeof(BONE) * nNumBone );		// zero clear

	int		nLen;
	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nLen, 4, 1 );
		if( nLen > 32 )
			Error("CMotion::ReadTM - %s bonename is too long", m_szName );

		file->Read( m_pBoneInfo[i].m_szName,	nLen, 1 );
		file->Read( &m_pBoneInfo[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		file->Read( &m_pBoneInfo[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		file->Read( &m_pBoneInfo[i].m_nParentIdx,	4, 1 );								// parent bone index
	}


	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBoneInfo[i].m_nParentIdx == -1 )
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[ m_pBoneInfo[i].m_nParentIdx ];
	}


	file->Read( &nNumSize, 4, 1 );

	m_pMotion		= new TM_ANIMATION[ nNumSize ];
	m_pBoneFrame	= new BONE_FRAME[ nNumBone ];
	m_pAttr			= new MOTION_ATTR[ nNumFrame ];
	memset( m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame );
	TM_ANIMATION	*p = m_pMotion;
	int		nCnt = 0;


	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nFrame, 4, 1 );
		if( nFrame == 1 )
		{
			m_pBoneFrame[i].m_pFrame = p;
			file->Read( m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1 );
			p += nNumFrame;
			nCnt += nNumFrame;
//			for( j = 0; j < nNumFrame; j ++ )
//			{
//				file->Read( &(p->m_mAniTM), sizeof(D3DXMATRIX), 1 );
//				file->Read( &(p->m_qRot), sizeof(D3DXQUATERNION), 1 );
//				file->Read( &(p->m_vPos), sizeof(D3DXVECTOR3), 1 );
//				p ++;
//				nCnt ++;
//			}
		} else
		{
			file->Read( &(m_pBoneFrame[i].m_mLocalTM), sizeof(D3DXMATRIX), 1 );
			m_pBoneFrame[i].m_pFrame = NULL;

		}
	}

	if( nCnt != nNumSize )
	{
		Error( "%s : frame size error", m_szName );
	}
}




void	CMotion :: AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight )
{
	int		i;
	BONE	*pBone = m_pBoneInfo;
	TM_ANIMATION	*pFrame = NULL;
	TM_ANIMATION	*pNext = NULL;
	TM_ANIMATION	*pFrameOld = NULL;
//	D3DXMATRIX	*pMatBone = m_mUpdateBone;
	D3DXMATRIX	*pMatBones = pmUpdateBone;
	int			nCurrFrame;
	float		fSlp;
	D3DXQUATERNION		qSlerp;
	D3DXVECTOR3			vSlerp;
	BONE_FRAME	*pBoneFrame = NULL;
	BONE_FRAME	*pBoneFrameOld = NULL;
	D3DXMATRIX	m1, m2;

	if( pMotionOld == NULL )
		pMotionOld = this;


	i = m_nMaxBone;

	nCurrFrame = (int)fFrameCurrent;
	fSlp = fFrameCurrent - (float)nCurrFrame;
	///////////////////////

//	int	nNextFrame = GetNextFrame();
	pBoneFrame	  = m_pBoneFrame;
	pBoneFrameOld = pMotionOld->m_pBoneFrame;

	while( i-- )
	{
		if( pBoneFrameOld->m_pFrame )
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld];
		else
			pFrameOld = NULL;
		if( pBoneFrame->m_pFrame )
		{
			pFrame = &pBoneFrame->m_pFrame[ nCurrFrame ];
			pNext  = &pBoneFrame->m_pFrame[ nNextFrame ];

			D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );
			D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );

			if( bMotionTrans )
			{
				float	fBlendSlp = fBlendWeight;
				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qSlerp, fBlendSlp );
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vSlerp, fBlendSlp );
				} else
				{
					D3DXQUATERNION	qLocal;
					D3DXVECTOR3		vLocal;
					D3DXMATRIX		*pmLocalTM = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
					vLocal.x = pmLocalTM->_41;
					vLocal.y = pmLocalTM->_42;
					vLocal.z = pmLocalTM->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal, &qSlerp, fBlendSlp );
					D3DXVec3Lerp( &vSlerp, &vLocal, &vSlerp, fBlendSlp );
				}
			}



			D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );


			D3DXMatrixRotationQuaternion( &m2, &qSlerp );
			m2 *= m1;

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];
//				else


			// WorldTM = LocalTM * ParentTM;

		} else

		{
			if( bMotionTrans )
			{
				float	fBlendSlp = fBlendWeight;
				D3DXQUATERNION	qLocal;
				D3DXVECTOR3		vLocal;
				D3DXMATRIX		*pmLocalTM = &pBoneFrame->m_mLocalTM;
				D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
				vLocal.x = pmLocalTM->_41;
				vLocal.y = pmLocalTM->_42;
				vLocal.z = pmLocalTM->_43;

				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qLocal, fBlendSlp );
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vLocal, fBlendSlp );
				} else
				{
					D3DXQUATERNION	qLocal2;
					D3DXVECTOR3		vLocal2;
					D3DXMATRIX		*pmLocalTM2 = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal2, pmLocalTM2 );
					vLocal2.x = pmLocalTM2->_41;
					vLocal2.y = pmLocalTM2->_42;
					vLocal2.z = pmLocalTM2->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal2, &qLocal, fBlendSlp );
					D3DXVec3Lerp( &vSlerp, &vLocal2, &vLocal, fBlendSlp );
				}
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );


				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				m2 *= m1;
			} else
				m2 = pBoneFrame->m_mLocalTM;

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];
//				else
//					m2 = m2 * *pmWorld;
		}

		*pmUpdateBone = m2;

		pBoneFrame ++;
		pBoneFrameOld ++;
		pBone ++;
		pmUpdateBone ++;
	}
}

