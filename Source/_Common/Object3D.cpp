#include "stdafx.h"

#include <stdio.h>
#include <dxerr9.h>
#include "Object3D.h"
#include "Material.h"
//#include "..\_DirectX\dxutil.h"
#include "xUtil3D.h"
#include "ModelObject.h"

#ifdef __MODEL_CACHE
#include "BetterCache.h"
#endif

#include "Environment.h"

LPDIRECT3DTEXTURE9	g_pReflectMap = NULL;

LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
LPDIRECT3DVERTEXDECLARATION9 g_pShadowVertexDeclaration;
LPDIRECT3DVERTEXSHADER9 g_pSkiningVS;
LPDIRECT3DVERTEXSHADER9 g_pShadowVS;
//DWORD	g_dwSkiningVS = 0xFFFFFFFF;

D3DXMATRIX	g_mReflect = D3DXMATRIX(0.50f, 0.00f, 0.00f, 0.00f,
	0.00f, -0.50f, 0.00f, 0.00f,
	0.00f, 0.00f, 1.00f, 0.00f,
	0.50f, 0.50f, 0.00f, 1.00f);

extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;
extern D3DXVECTOR4 s_vLightPos;
extern D3DXMATRIX s_mViewProj;
extern D3DXMATRIX s_mView;
extern D3DXMATRIX s_mProj;
extern BOOL	s_bLight;
extern BOOL	s_bNight;
extern BOOL s_bFog;
extern float s_fFogStart;
extern float s_fFogEnd;


#ifndef	__WORLDSERVER
HRESULT		CreateSkinningVS(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName)
{
	HRESULT hr;
	LPD3DXBUFFER pCode;

	// Setup the vertex declaration
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0},
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0},
		{ 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0},
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0},
		{ 0,  36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0},
		D3DDECL_END()
	};


	if (FAILED(hr = pd3dDevice->CreateVertexDeclaration(decl, &g_pSkinVertexDeclaration)))
		return hr;

	// Assemble the vertex shader file
	if (FAILED(hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL)))
		return hr;
	// Create the vertex shader
	if (FAILED(hr = pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),
		&g_pSkiningVS)))
	{
		SAFE_RELEASE(pCode);
		return hr;
	}
	SAFE_RELEASE(pCode);

	return S_OK;
}

HRESULT		CreateShadowVS(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName)
{
	HRESULT hr;
	LPD3DXBUFFER pCode;

	// Setup the vertex declaration
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0},
		{ 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0},
		{ 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0},
		D3DDECL_END()
	};

#ifdef	_DEBUG
	// Shadow shader
	if (FAILED(hr = pd3dDevice->CreateVertexDeclaration(decl, &g_pShadowVertexDeclaration)))
		return hr;
	// Create vertex shader from a file
	if (FAILED(hr = D3DXAssembleShaderFromFile(szFileName, NULL, NULL, 0, &pCode, NULL)))
		return hr;
	if (FAILED(hr = pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &g_pShadowVS)))
	{
		SAFE_RELEASE(pCode);
		return hr;
	}
	SAFE_RELEASE(pCode);
#else
	if (FAILED(hr = pd3dDevice->CreateVertexDeclaration(decl, &g_pShadowVertexDeclaration)))
		return hr;

	// Assemble the vertex shader file
	if (FAILED(hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL)))
		return hr;
	// Create the vertex shader
	if (FAILED(hr = pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),
		&g_pShadowVS)))
	{
		SAFE_RELEASE(pCode);
		return hr;
	}
	SAFE_RELEASE(pCode);
#endif
	return S_OK;
}


void	DeleteVertexShader(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (g_pSkiningVS)
		SAFE_RELEASE(g_pSkiningVS);
	if (g_pShadowVS)
		SAFE_RELEASE(g_pShadowVS);
	if (g_pSkinVertexDeclaration)
		SAFE_RELEASE(g_pSkinVertexDeclaration);
	if (g_pShadowVertexDeclaration)
		SAFE_RELEASE(g_pShadowVertexDeclaration);
}

LPDIRECT3DVERTEXBUFFER9		g_pd3d_ShadowVB;

HRESULT		CreateShadowMask(LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight)
{
	HRESULT	hr;

	hr = pd3dDevice->CreateVertexBuffer(4 * sizeof(SHADOWMASK_VERTEX),
		D3DUSAGE_WRITEONLY, D3DFVF_SHADOWMASKVERTEX,
		D3DPOOL_MANAGED, &g_pd3d_ShadowVB, NULL);
	if (FAILED(hr))
		Error("CreateShadowMask : 버텍스 버퍼 생성 실패");

	SHADOWMASK_VERTEX* pVertex;
	hr = g_pd3d_ShadowVB->Lock(0, 4 * sizeof(SHADOWMASK_VERTEX), (void**)&pVertex, 0);
	if (FAILED(hr))
		return hr;
	memset(pVertex, 0x00, 4 * sizeof(SHADOWMASK_VERTEX));
	pVertex[0].x = 0.0f;	pVertex[0].y = 0.0f;
	pVertex[1].x = (float)nWidth; pVertex[1].y = 0.0f;
	pVertex[2].x = 0.0f; pVertex[2].y = (float)nHeight;
	pVertex[3].x = (float)nWidth; pVertex[3].y = (float)nHeight;

	pVertex[0].color = pVertex[1].color = pVertex[2].color = pVertex[3].color = 0x44000000;

	g_pd3d_ShadowVB->Unlock();

	return S_OK;
}

void	RenderShadowMask(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
	pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	pd3dDevice->SetRenderState(D3DRS_STENCILREF, 1);

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetVertexDeclaration(NULL);
	pd3dDevice->SetFVF(D3DFVF_SHADOWMASKVERTEX);
	pd3dDevice->SetStreamSource(0, g_pd3d_ShadowVB, 0, sizeof(SHADOWMASK_VERTEX));

	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// reset
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void	DeleteShadowMask(void)
{
	SAFE_RELEASE(g_pd3d_ShadowVB);
}

#endif // !__WORLDSERVER
//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////
CObject3DMng		g_Object3DMng;

CObject3DMng::CObject3DMng()
{
	Init();
}

CObject3DMng :: ~CObject3DMng()
{
	Destroy();
}

void	CObject3DMng::Init(void)
{
#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->ClearCacheEntities();
	CBetterCache::GetInstance()->ClearCacheObjects();
	CBetterCache::GetInstance()->ClearCacheOthers();
#else
	m_mapObject3D.clear();
#endif
}


void	CObject3DMng::Destroy(void)
{
#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->Clear();
#else
	for (map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i)
	{
		i->second->DeleteDeviceObjects();
		SAFE_DELETE(i->second);
	}
#endif

	Init();
}

HRESULT CObject3DMng::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}


HRESULT CObject3DMng::DeleteDeviceObjects()
{
#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->DeleteDeviceObjects();
#else
	for (map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i)
		i->second->DeleteDeviceObjects();
#endif
	return S_OK;
}



#ifdef __MODEL_CACHE
int CObject3DMng::DeleteObject3D(CObject3D* pObject3D, int nType)
#else
int CObject3DMng::DeleteObject3D(CObject3D* pObject3D)
#endif
{
	if (!pObject3D)
		return FALSE;
#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->DecreaseObjectCount(pObject3D, nType);
#else
	if (m_mapObject3D.size() == 0)
		return FALSE;
	if (pObject3D->m_nUseCnt > 1)
		pObject3D->m_nUseCnt--;
	else
	{
		m_mapObject3D.erase(pObject3D->m_szFileName);
		SAFE_DELETE(pObject3D);
		return TRUE;
	}
#endif
	return FALSE;
}

#ifdef __MODEL_CACHE
CObject3D* CObject3DMng::LoadObject3D(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType)
#else
CObject3D* CObject3DMng::LoadObject3D(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName)
#endif
{
	char sFile[MAX_PATH] = { 0, };
	strcpy(sFile, szFileName);
	strlwr(sFile);

#ifdef __MODEL_CACHE
	CObject3D* bResult = CBetterCache::GetInstance()->GetCacheModel(sFile, nType);
	if (bResult != NULL)
		return bResult;
#else
	map<string, CObject3D*>::iterator i = m_mapObject3D.find(sFile);
	if (i != m_mapObject3D.end())
	{
		i->second->m_nUseCnt++;
		return i->second;
	}
#endif
	CObject3D* pObject3D = new CObject3D;
	pObject3D->InitDeviceObjects(pd3dDevice);
	if (pObject3D->LoadObject(szFileName) == FAIL)
	{
		SAFE_DELETE(pObject3D);
		return NULL;
	}
	pObject3D->m_nUseCnt = 1;
#ifdef __MODEL_CACHE
	CBetterCache::GetInstance()->InsertCacheModel(sFile, pObject3D, nType);
#else
	bool bResult = m_mapObject3D.insert(map<string, CObject3D*>::value_type(sFile, pObject3D)).second;
#endif
	return pObject3D;
}



void CObject3DMng::Process(void)
{
}




//////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////
/////////
/////////
/////////
//////////////////////////////////////////////////////////////////////////////


CObject3D::CObject3D()
{
	Init();
}

CObject3D :: ~CObject3D()
{
	Destroy();
}

void	CObject3D::Init(void)
{
	m_pGroup = &m_Group[0];
	memset(m_Group, 0, sizeof(LOD_GROUP) * MAX_GROUP);
	//	m_pObject	  = NULL;
	//	_mUpdate	  = NULL;
	m_pmExternBone = m_pmExternBoneInv = NULL;
	m_pBaseBone = NULL;
	m_pBaseBoneInv = NULL;
	m_pMotion = NULL;
	memset(&m_CollObject, 0, sizeof(GMOBJECT));
	m_CollObject.m_Type = GMT_ERROR;
	m_nID = 0;
	m_nHavePhysique = FALSE;
	//	m_nMaxObject = 0;
	m_nMaxBone = 0;
	m_bSendVS = 0;
	m_bLOD = 0;
	memset(m_szFileName, 0, sizeof(m_szFileName));

	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;
#if __VER >= 9 // __CSC_VER9_5
	m_vForce3.x = m_vForce3.y = m_vForce3.z = 0;
	m_vForce4.x = m_vForce4.y = m_vForce4.z = 0;
#endif //__CSC_VER9_5
	m_vBBMin.x = m_vBBMin.y = m_vBBMin.z = 65535.0;
	m_vBBMax.x = m_vBBMax.y = m_vBBMax.z = -65535.0;
	m_nMaxFace = 0;
	m_nNoTexture = 0;
	m_nNoEffect = 0;

	m_nMaxEvent = 0;
	memset(m_vEvent, 0, sizeof(m_vEvent));

	m_fScrlU = m_fScrlV = 0.0f;

	m_nUseCnt = 0;

	m_fAmbient[0] = 1.0f;
	m_fAmbient[1] = 1.0f;
	m_fAmbient[2] = 1.0f;

	m_nTextureEx = 0;
	m_pAttr = NULL;
}

void	CObject3D::Destroy(void)
{
	int		i, j, k;

	for (j = 0; j < MAX_GROUP; j++)
	{
		GMOBJECT* pObject = m_Group[j].m_pObject;
		for (i = 0; i < m_Group[j].m_nMaxObject; i++)
		{
			SAFE_DELETE_ARRAY(pObject[i].m_pVertexList);
			SAFE_DELETE_ARRAY(pObject[i].m_pPhysiqueVertex);
			for (k = 0; k < pObject[i].m_nMaxMtrlBlk; k++)
			{
#if !defined(__WORLDSERVER)
				g_TextureMng.DeleteMaterial(pObject[i].m_pMtrlBlkTexture[k]);
#endif // !__WORLDSERVER
			}
			SAFE_DELETE_ARRAY(pObject[i].m_pMtrlBlk);
			SAFE_DELETE_ARRAY(pObject[i].m_pMtrlBlkTexture);
			SAFE_DELETE_ARRAY(pObject[i].m_pFrame);

			SAFE_DELETE_ARRAY(pObject[i].m_pVB);
			SAFE_DELETE_ARRAY(pObject[i].m_pIB);
			pObject[i].m_pIIB = NULL;
		}
	}

	SAFE_DELETE_ARRAY(m_CollObject.m_pVertexList);
	SAFE_DELETE_ARRAY(m_CollObject.m_pPhysiqueVertex);
	SAFE_DELETE_ARRAY(m_CollObject.m_pMtrlBlk);
	SAFE_DELETE_ARRAY(m_CollObject.m_pMtrlBlkTexture);
	SAFE_DELETE_ARRAY(m_CollObject.m_pFrame);


	SAFE_DELETE_ARRAY(m_pAttr);

	SAFE_DELETE_ARRAY(m_CollObject.m_pVB);
	SAFE_DELETE_ARRAY(m_CollObject.m_pIB);
	m_CollObject.m_pIIB = NULL;

	DeleteDeviceObjects();	// m_pObject[].m_pd3d_VB


	SAFE_DELETE_ARRAY(m_Group[0].m_pObject);
	m_Group[1].m_pObject = NULL;
	m_Group[2].m_pObject = NULL;
	SAFE_DELETE_ARRAY(m_Group[0]._mUpdate);
	m_Group[1]._mUpdate = NULL;
	m_Group[2]._mUpdate = NULL;

	SAFE_DELETE_ARRAY(m_pBaseBone);
	SAFE_DELETE(m_pMotion);

	Init();
}

int		CObject3D::GetMaxVertex(void)
{
	int		i, nMax = 0;
	for (i = 0; i < m_pGroup->m_nMaxObject; i++)
		nMax += m_pGroup->m_pObject[i].m_nMaxVertexList;
	return nMax;
}

int		CObject3D::GetMaxFace(void)
{
	int		i, nMax = 0;
	for (i = 0; i < m_pGroup->m_nMaxObject; i++)
		nMax += m_pGroup->m_pObject[i].m_nMaxFaceList;
	return nMax;
}

int		CObject3D::GetMaxMtrlBlk(void)
{
	int		i, nMax = 0;
	for (i = 0; i < m_pGroup->m_nMaxObject; i++)
		nMax += m_pGroup->m_pObject[i].m_nMaxMtrlBlk;
	return nMax;
}





HRESULT CObject3D::RestoreDeviceObjects(LPDIRECT3DVERTEXBUFFER9* ppd3d_VB, D3DPOOL pool)
{
	int		i, j;
	GMOBJECT* pObj;

	for (j = 0; j < MAX_GROUP; j++)
	{
		GMOBJECT* pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for (i = 0; i < nMaxObject; i++)
		{
			pObj = &pObject[i];
			if (pObj->m_Type == GMT_SKIN)
			{
				CreateDeviceBuffer(pObj, &ppd3d_VB[i], pool);

			}
			else
			{
				if (pObj->m_pd3d_VB)
					continue;

				CreateDeviceBuffer(pObj, NULL, pool);

			}
		}
		ppd3d_VB += m_Group[j].m_nMaxObject;
	}


	return S_OK;
}

HRESULT CObject3D::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}

HRESULT CObject3D::DeleteDeviceObjects()
{
	int		i, j;
	GMOBJECT* pObj;

	for (j = 0; j < MAX_GROUP; j++)
	{
		GMOBJECT* pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for (i = 0; i < nMaxObject; i++)
		{
			pObj = &pObject[i];
			SAFE_RELEASE(pObj->m_pd3d_VB);
			SAFE_RELEASE(pObj->m_pd3d_IB);
		}
	}

	return  S_OK;
}

int		g_MaxVB = 0;



HRESULT	CObject3D::CreateDeviceBuffer(GMOBJECT* pObject, LPDIRECT3DVERTEXBUFFER9* ppd3d_VB, D3DPOOL pool)
{
#ifdef	__WORLDSERVER
	return S_OK;
#else
	//	pool = D3DPOOL_MANAGED;

	HRESULT		hr;
	int			nVertexSize;
	DWORD		dwFVF;
	DWORD		dwUsage = D3DUSAGE_WRITEONLY;

	if (g_bUsableVS == FALSE && pObject->m_pPhysiqueVertex)
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;

	nVertexSize = pObject->m_nVertexSize;
	dwFVF = pObject->m_dwFVF;

	if (ppd3d_VB == NULL)
	{
		// create vertexbuffer
		if (pObject->m_pd3d_VB == NULL)
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);
			hr = m_pd3dDevice->CreateVertexBuffer(pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, &pObject->m_pd3d_VB, NULL);

			if (FAILED(hr))
			{
				LPCTSTR szError = Error("1 %s Object3D 버텍스 버퍼 생성 실패(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x",
					m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB,
					nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice);
				//ADDERRORMSG( szError );

				if (FAILED(hr = m_pd3dDevice->TestCooperativeLevel()))
				{
					LPCTSTR szErr = Error("%s CObject3D 디바이스실패 %08x", m_szFileName, (int)hr);
					//ADDERRORMSG( szErr );
				}
				int* p = NULL;
				*p = 1;
				return E_FAIL;
			}
			SendVertexBuffer(pObject, pObject->m_pd3d_VB);
		}
	}
	else
	{
		if (*ppd3d_VB == NULL)
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);

			hr = m_pd3dDevice->CreateVertexBuffer(pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, ppd3d_VB, NULL);

			if (FAILED(hr))
			{
				LPCTSTR szError = Error("2 %s Object3D 버텍스 버퍼 생성 실패(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x",
					m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB,
					nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice);
				//ADDERRORMSG( szError );
				if (FAILED(hr = m_pd3dDevice->TestCooperativeLevel()))
				{
					LPCTSTR szErr = Error("%s CObject3D 디바이스실패 %08x", m_szFileName, (int)hr);
					//ADDERRORMSG( szErr );
				}
				int* p = NULL;
				*p = 1;
				return E_FAIL;
			}
			SendVertexBuffer(pObject, *ppd3d_VB);
		}
	}

	// create indexbuffer
	if (pObject->m_pd3d_IB == NULL)
	{
		g_MaxVB += (pObject->m_nMaxIB * sizeof(WORD));

		hr = m_pd3dDevice->CreateIndexBuffer(pObject->m_nMaxIB * sizeof(WORD),
			dwUsage, D3DFMT_INDEX16,
			D3DPOOL_MANAGED, &pObject->m_pd3d_IB, NULL);
		if (FAILED(hr))
		{
			Error("%s Object3D 인덱스 버퍼 생성 실패(%s) : m_nMaxIB=%d",
				m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxIB);
			int* p = NULL;
			*p = 1;
			return E_FAIL;
		}
		SendIndexBuffer(pObject);
	}


	return S_OK;
#endif // !__WORLDSERVER
}


int CObject3D::IsAnimateFile(LPCTSTR szFileName)
{
	CResFile resFp;

	BOOL bRet = resFp.Open(szFileName, "rb");
	if (bRet == FALSE)
	{
		Error("IsAnimateFile : %s 읽기 실패", szFileName);
		return FAIL;
	}
	char c0;
	int d0;
	char buff[64];
	D3DXVECTOR3 v;
	float f0;

	resFp.Read(&c0, 1, 1);
	resFp.Read(buff, c0, 1);
	resFp.Read(&d0, 4, 1);

	resFp.Read(&d0, 4, 1);		// Serial ID
	resFp.Read(&v, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&v, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&f0, sizeof(float), 1);
	resFp.Read(&f0, sizeof(float), 1);
	resFp.Seek(16, SEEK_CUR);		// reserved

	resFp.Read(&v, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&v, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&f0, sizeof(float), 1);		// per slerp
	resFp.Read(&d0, 4, 1);

	BOOL bAnimate = FALSE;
	if (d0 > 0)
		bAnimate = TRUE;
	else
		bAnimate = FALSE;

	resFp.Close();

	return bAnimate;
}


//		Load Object

int		CObject3D::LoadObject(LPCTSTR szFileName)
{
	int		j, nTemp;
	int		nVer;
	D3DXVECTOR3	vTemp;
	BOOL	bNormalObj = FALSE;

	CResFile resFp;

	BOOL bRet = resFp.Open(MakePath(DIR_MODEL, szFileName), "rb");
	if (bRet == FALSE)
	{
		Error("LoadObject : %s 읽기 실패", szFileName);
		return FAIL;
	}
	Init();


	char szName[MAX_PATH];
	strcpy(m_szFileName, szFileName);
	strlwr(m_szFileName);
	//	strcpy( szName, GetFileName(szFileName) );
	GetFileName(szFileName, szName);

	char buff[MAX_PATH];
	char cLen;
	resFp.Read(&cLen, 1, 1);
	resFp.Read(buff, cLen, 1);
	for (j = 0; j < cLen; j++)
		buff[j] = buff[j] ^ (char)0xcd;

	if (cLen >= 64)
	{
		Error("LoadObject3D : %s 파일명이 너무 길다 %d", szFileName, (int)cLen);
		return 0;
	}

	buff[cLen] = 0;
	if (strcmpi(szName, buff) != 0)
	{
		Error("잘못된 파일 : %s, %s, %s", szFileName, szName, buff);
		return 0;
	}


	resFp.Read(&nVer, 4, 1);
	if (nVer < VER_MESH)
	{
		Error("%s의 버전은 %d.  최신버전은 %d", szFileName, nVer, VER_MESH);
		resFp.Close();
		return FAIL;
	}

	resFp.Read(&m_nID, 4, 1);		// Serial ID
	resFp.Read(&m_vForce1, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&m_vForce2, sizeof(D3DXVECTOR3), 1);
#if __VER >= 9 // __CSC_VER9_5
	if (nVer >= 22)
	{
		resFp.Read(&m_vForce3, sizeof(D3DXVECTOR3), 1);
		resFp.Read(&m_vForce4, sizeof(D3DXVECTOR3), 1);
	}
#endif //__CSC_VER9_5
	resFp.Read(&m_fScrlU, sizeof(float), 1);
	resFp.Read(&m_fScrlV, sizeof(float), 1);
	resFp.Seek(16, SEEK_CUR);		// reserved

	resFp.Read(&m_vBBMin, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&m_vBBMax, sizeof(D3DXVECTOR3), 1);
	resFp.Read(&m_fPerSlerp, sizeof(float), 1);		// per slerp
	resFp.Read(&m_nMaxFrame, 4, 1);

	resFp.Read(&m_nMaxEvent, 4, 1);
	if (m_nMaxEvent > 0)
		resFp.Read(m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1);

	resFp.Read(&nTemp, 4, 1);
	if (nTemp)
	{
		m_CollObject.m_Type = GMT_NORMAL;
		LoadGMObject(&resFp, &m_CollObject);
	}
	resFp.Read(&m_bLOD, 4, 1);


	resFp.Read(&m_nMaxBone, 4, 1);

	if (m_nMaxBone > 0)
	{
		m_pBaseBone = new D3DXMATRIX[m_nMaxBone * 2];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;
		resFp.Read(m_pBaseBone, sizeof(D3DXMATRIX) * m_nMaxBone, 1);
		resFp.Read(m_pBaseBoneInv, sizeof(D3DXMATRIX) * m_nMaxBone, 1);
		if (m_nMaxFrame > 0)
		{
			m_pMotion = new CMotion;
			m_pMotion->ReadTM(&resFp, m_nMaxBone, m_nMaxFrame);
		}
		resFp.Read(&m_bSendVS, 4, 1);
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;
	LOD_GROUP* pGroup;
	int nPoolSize, nDebugSize = 0;
	resFp.Read(&nPoolSize, 4, 1);
	GMOBJECT* pPool = new GMOBJECT[nPoolSize];
	if (pPool == NULL)
		Error("메모리 할당 실패:CObject3D::LoadObject( %s ) %d", m_szFileName, nPoolSize);
	memset(pPool, 0, sizeof(GMOBJECT) * nPoolSize);
	for (int i = 0; i < nMaxGroup; i++)
	{
		pGroup = &m_Group[i];
		resFp.Read(&pGroup->m_nMaxObject, 4, 1);

		pGroup->m_pObject = pPool;
		pPool += pGroup->m_nMaxObject;

		nDebugSize += pGroup->m_nMaxObject;
		if (nDebugSize > nPoolSize)
			Error("LoadObject : 메모리 오버플로우 %d, %d", nPoolSize, nDebugSize);

		memset(pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject);
		for (j = 0; j < pGroup->m_nMaxObject; j++)		pGroup->m_pObject[j].m_nID = -1;

		GMOBJECT* pObject;
		int		nParentIdx;
		BOOL	bAnimate = FALSE;
		int		nType;
		//------  Geometry Object
		for (j = 0; j < pGroup->m_nMaxObject; j++)
		{
			pObject = &pGroup->m_pObject[j];

			resFp.Read(&nType, 4, 1);	// Type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if (nType & 0x80000000)
				pObject->m_bLight = TRUE;

			resFp.Read(&pObject->m_nMaxUseBone, 4, 1);
			if (pObject->m_nMaxUseBone > 0)
				resFp.Read(pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1);

			if (pObject->m_Type == GMT_SKIN)
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
				pObject->m_dwFVF = D3DFVF_SKINVERTEX;
			}
			else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
				pObject->m_dwFVF = D3DFVF_NORMALVERTEX;
			}

			resFp.Read(&pObject->m_nID, 4, 1);	// Object ID
			resFp.Read(&nParentIdx, 4, 1);			// parent idx
			pObject->m_nParentIdx = nParentIdx;


			if (nParentIdx != -1)
			{
				resFp.Read(&pObject->m_ParentType, 4, 1);

				switch (pObject->m_ParentType)
				{
				case GMT_BONE:		pObject->m_pParent = NULL;	break;
				case GMT_NORMAL:	pObject->m_pParent = &pGroup->m_pObject[nParentIdx];		break;
					break;
				}
			}


			resFp.Read(&pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1);

			// load geometry
			LoadGMObject(&resFp, pObject);

			if (pObject->m_Type == GMT_SKIN)
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
				pObject->m_dwFVF = D3DFVF_SKINVERTEX;
			}
			else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
				pObject->m_dwFVF = D3DFVF_NORMALVERTEX;
			}

			if (pObject->m_pPhysiqueVertex == NULL)	bNormalObj = TRUE;
			else										m_nHavePhysique = TRUE;

			// load TM animation -
			if (pObject->m_Type == GMT_NORMAL)
			{
				if (m_nMaxFrame > 0)	LoadTMAni(&resFp, pObject);
			}

		}
	} // LOD_GROUP

	if (bNormalObj == TRUE)
	{
		D3DXMATRIX* pmPool = new D3DXMATRIX[nPoolSize];
		for (int i = 0; i < nMaxGroup; i++)
		{
			m_Group[i]._mUpdate = pmPool;
			pmPool += m_Group[i].m_nMaxObject;
			for (j = 0; j < m_Group[i].m_nMaxObject; j++)
				D3DXMatrixIdentity(&m_Group[i]._mUpdate[j]);
		}
	}


	if (nVer >= 21)
	{
		int nAttr = 0;

		resFp.Read(&nAttr, 4, 1);
		if (nAttr == m_nMaxFrame)
		{

			if (m_nMaxFrame > 0)
				m_pAttr = new MOTION_ATTR[m_nMaxFrame];
			else
				m_pAttr = NULL;

			resFp.Read(m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1);
		}
	}


	resFp.Close();

	return SUCCESS;
}

//	Save

int		CObject3D::SaveObject(LPCTSTR szFileName)
{

	return SUCCESS;
}


int		CObject3D::LoadGMObject(CResFile* file, GMOBJECT* pObject)
{
	DWORD	dwTemp;
	int		i;

	file->Read(&pObject->m_vBBMin, sizeof(D3DXVECTOR3), 1);
	file->Read(&pObject->m_vBBMax, sizeof(D3DXVECTOR3), 1);

	file->Read(&pObject->m_bOpacity, 4, 1);
	file->Read(&pObject->m_bBump, 4, 1);
	file->Read(&pObject->m_bRigid, 4, 1);
	file->Seek(28, SEEK_CUR);		// reserved

	// size of list
	file->Read(&pObject->m_nMaxVertexList, 4, 1);
	file->Read(&pObject->m_nMaxVB, 4, 1);
	file->Read(&pObject->m_nMaxFaceList, 4, 1);
	file->Read(&pObject->m_nMaxIB, 4, 1);

	m_nMaxFace += pObject->m_nMaxFaceList;

	// Vertex list
	if (pObject->m_Type == GMT_SKIN)
	{
		pObject->m_pVertexList = new D3DXVECTOR3[pObject->m_nMaxVertexList];
		SKINVERTEX* pVB = new SKINVERTEX[pObject->m_nMaxVB];	// Indexed Vertex buffer -
		pObject->m_pVB = pVB;
	}
	else
	{
		pObject->m_pVertexList = new D3DXVECTOR3[pObject->m_nMaxVertexList];
		NORMALVERTEX* pVB = new NORMALVERTEX[pObject->m_nMaxVB];	// Indexed Vertex buffer

		pObject->m_pVB = pVB;
	}
	pObject->m_pIB = new WORD[pObject->m_nMaxIB + pObject->m_nMaxVB];
	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;

	file->Read(pObject->m_pVertexList, sizeof(D3DXVECTOR3) * pObject->m_nMaxVertexList, 1);
	if (pObject->m_Type == GMT_SKIN)
		file->Read(pObject->m_pVB, sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1);
	else
		file->Read(pObject->m_pVB, sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1);


	file->Read(pObject->m_pIB, sizeof(WORD) * pObject->m_nMaxIB, 1);
	file->Read(pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1);

	file->Read(&dwTemp, 4, 1);
	if (dwTemp)
	{
		// Physique
		pObject->m_pPhysiqueVertex = new int[pObject->m_nMaxVertexList];
		file->Read(pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1);
	}

	// Material
	D3DMATERIAL9	mMaterial;
	char			szBitmap[256];
	int				nLen;
	MATERIAL* mMaterialAry[16];
	//nt				nIdx = 0;
	int				bIsMaterial;

	file->Read(&bIsMaterial, 4, 1);
	pObject->m_bMaterial = bIsMaterial;
	if (bIsMaterial)
	{
		memset(mMaterialAry, 0, sizeof(mMaterialAry));
		//		for( i = 0; i < 16; i ++ )	mMaterialAry[i] = NULL;

		file->Read(&pObject->m_nMaxMaterial, 4, 1);

		if (pObject->m_nMaxMaterial == 0)	pObject->m_nMaxMaterial = 1;

		for (i = 0; i < pObject->m_nMaxMaterial; i++)
		{
			file->Read(&mMaterial, sizeof(D3DMATERIAL9), 1);
			file->Read(&nLen, 4, 1);
			if (nLen > sizeof(szBitmap))
				Error("CObject3D::LoadGMObject : %s 텍스쳐 파일명이 너무길다 : 길이 = %d", m_szFileName, nLen);
			file->Read(szBitmap, nLen, 1);
			strlwr(szBitmap);

			pObject->m_MaterialAry[i].m_Material = mMaterial;

			if (strlen(szBitmap) + 1 > sizeof(pObject->m_MaterialAry[i].strBitMapFileName))
				Error("CObject3D::LoadGeoMesh() : %s의 길이가 너무 길다", szBitmap);

			strcpy(pObject->m_MaterialAry[i].strBitMapFileName, szBitmap);
#if	!defined(__WORLDSERVER)
			if (IsEmpty(szBitmap) == FALSE)
				mMaterialAry[i] = g_TextureMng.AddMaterial(m_pd3dDevice, &mMaterial, szBitmap);
#endif
		}
	}

	// vertex buffer Material block
	file->Read(&pObject->m_nMaxMtrlBlk, 4, 1);

	if (pObject->m_nMaxMtrlBlk >= 32)
	{
		Error("LoadGMObject (m_nMaxMtrlBlk) %s", m_szFileName);
		return FAIL;
	}
	if (pObject->m_nMaxMtrlBlk > 0)
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[pObject->m_nMaxMtrlBlk];
		pObject->m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[pObject->m_nMaxMtrlBlk * 8];
		memset(pObject->m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk * 8));

		file->Read(pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1);
#ifndef __WORLDSERVER

		for (i = 0; i < pObject->m_nMaxMtrlBlk; i++)
		{
			if (bIsMaterial)
			{
				if (mMaterialAry[pObject->m_pMtrlBlk[i].m_nTextureID])
				{
					pObject->m_pMtrlBlkTexture[i] = mMaterialAry[pObject->m_pMtrlBlk[i].m_nTextureID]->m_pTexture;

				}


			}
		}
#endif	// __WORLDSERVER
	}

	return SUCCESS;
}


//	TM Animation data load

int		CObject3D::LoadTMAni(CResFile* file, GMOBJECT* pObject)
{
	int	bFrame;
	file->Read(&bFrame, 4, 1);
	if (bFrame == 0)	return FAIL;

	pObject->m_pFrame = new TM_ANIMATION[m_nMaxFrame];
	file->Read(pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1);

	return SUCCESS;
}





void	CObject3D::ChangeTexture(LPCTSTR szSrc, LPCTSTR szDest)
{
#ifdef __WORLDSERVER
	return;
#else
	int		i, j, k;
	GMOBJECT* pObject;
	char	szBuff[128];

	strcpy(szBuff, szSrc);
	strlwr(szBuff);
	TCHAR szBitMapFileName[16][128];
	for (k = 0; k < MAX_GROUP; k++)
	{
		for (i = 0; i < m_Group[k].m_nMaxObject; i++)
		{
			pObject = &m_Group[k].m_pObject[i];




			for (j = 0; j < pObject->m_nMaxMtrlBlk; j++)
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				_tcscpy(szBitMapFileName[j], pObject->m_MaterialAry[nID].strBitMapFileName);
			}
			for (j = 0; j < pObject->m_nMaxMtrlBlk; j++)
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				if (strcmp(szBitMapFileName[j], szBuff) == 0)
				{
					MATERIAL* pMtrl;
					D3DMATERIAL9	mMtrl;
					pMtrl = g_TextureMng.AddMaterial(m_pd3dDevice, &mMtrl, szDest);
					pObject->m_pMtrlBlkTexture[j] = pMtrl->m_pTexture;
					strcpy(pObject->m_MaterialAry[nID].strBitMapFileName, szDest);
				}
			}
		}
	} // LOD_GROUP
#endif //!__WORLDSERVER
}




int		CObject3D::SaveGMObject(FILE* fp, GMOBJECT* pObject)
{
	return SUCCESS;
}






int		CObject3D::SaveTMAni(FILE* fp, GMOBJECT* pObject)
{
	int		bFrame;
	if (pObject->m_pFrame)	bFrame = 1;
	else					bFrame = 0;
	fwrite(&bFrame, 4, 1, fp);
	if (bFrame == 0)	return FAIL;

	if (m_nMaxFrame > 0)
	{
		fwrite(pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1, fp);
	}

	return SUCCESS;
}

//static int	_nSlideCnt = 0;







int		CObject3D::SlideVectorXZ(D3DXVECTOR3* pOut, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vEnd, const D3DXMATRIX& mWorld, BOOL bCollObj, int nSlideCnt)
{
	D3DXVECTOR3* v1, * v2, * v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX* pVB;
	WORD* pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP* pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if (bCollObj == FALSE)
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if (pGroup->_mUpdate == NULL)
			return FAIL;
	}

	if (pObject->m_Type == GMT_ERROR)		return 0;

	for (i = 0; i < nMaxObj; i++)
	{
		if (pObject->m_Type == GMT_SKIN)
			Error("SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName);
		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);

		if (bCollObj == FALSE)
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse(&mInv, NULL, &mTM);
		D3DXVec3TransformCoord(&vInvPos, &vPos, &mInv);
		D3DXVec3TransformCoord(&vInvEnd, &vEnd, &mInv);
		vInvDir = vInvEnd - vInvPos;
		for (j = 0; j < nMaxFace; j++)
		{
			v1 = &pVB[*pIB++].position;
			v2 = &pVB[*pIB++].position;
			v3 = &pVB[*pIB++].position;

			//			FLOAT fU, fV;
			//			bRet = D3DXIntersectTri( v1, v2, v3, &vInvPos, &vInvDir, &fU, &fV, &fDist );
			bRet = IsTouchRayTri(v1, v2, v3, &vInvPos, &vInvDir, &fDist);

			if (bRet && fDist >= 0.0f)
			{


				if (fDist < 1.0f)
				{

					vDir = vEnd - vPos;
					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;

					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross(&vN, &vA, &vB);
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord(&vN, &vN, &mInv);

					if (vN.x == 0 && vN.z == 0)
						Error("CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다");

					vN.y = 0;
					D3DXVec3Normalize(&vN, &vN);


					vTemp = vEnd - vIntersect;
					CalcSlideVec(&vTemp, vTemp, vN);
					//					vTemp += vIntersect;
					//					*pOut = vTemp - vPos;
					*pOut = vTemp;

					vTemp += vPos;

					if (++nSlideCnt < 3)
						SlideVectorXZ(pOut, vPos, vTemp, mWorld, bCollObj, nSlideCnt);

					//					nSlideCnt = 0;
					return 1;
				}
			}
		}
	}
	//	_nSlideCnt = 0;
	return 0;
}




int		CObject3D::SlideVectorXZ2(D3DXVECTOR3* pOut, D3DXVECTOR3* pIntersect, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vEnd, const D3DXMATRIX& mWorld, BOOL bCollObj)
{
	D3DXVECTOR3* v1, * v2, * v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	NORMALVERTEX* pVB;
	WORD* pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP* pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if (bCollObj == FALSE)
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if (pGroup->_mUpdate == NULL)		return FAIL;
	}


	if (pObject->m_Type == GMT_ERROR)		return 0;

	for (i = 0; i < nMaxObj; i++)
	{
		if (pObject->m_Type == GMT_SKIN)
			Error("SlideVectorXZ:스킨오브젝트 발견. 에러%s", m_szFileName);

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		if (bCollObj == FALSE)
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse(&mInv, NULL, &mTM);
		D3DXVec3TransformCoord(&vInvPos, &vPos, &mInv);
		D3DXVec3TransformCoord(&vInvEnd, &vEnd, &mInv);
		vInvDir = vInvEnd - vInvPos;
		for (j = 0; j < nMaxFace; j++)
		{
			v1 = &pVB[*pIB++].position;
			v2 = &pVB[*pIB++].position;
			v3 = &pVB[*pIB++].position;

			bRet = IsTouchRayTri(v1, v2, v3, &vInvPos, &vInvDir, &fDist);
#if __VER >= 11 //  __FIX_COLLISION
			if (bRet)
			{
				vDir = vEnd - vPos;
				float fDirLength = D3DXVec3Length(&vDir);


				if (fDirLength > 3.0f)
				{
					if (fabs(fDist) < fDirLength)
					{


						//					fDist *= 0.5f;
						if (fDist < 0.0f)
							vIntersect = vPos - fDist * vDir;
						else
							vIntersect = vPos + fDist * vDir;
						vA = *v2 - *v1;
						vB = *v3 - *v1;
						D3DXVec3Cross(&vN, &vA, &vB);
						mInv = mTM;
						mInv._41 = mInv._42 = mInv._43 = 0;
						D3DXVec3TransformCoord(&vN, &vN, &mInv);

						if (vN.x == 0 && vN.z == 0)
							Error("CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다");

						vN.y = 0;
						D3DXVec3Normalize(&vN, &vN);


						vTemp = vEnd - vIntersect;
						*pIntersect = vIntersect;

						vInvDir.x = -vDir.x;	vInvDir.y = -vDir.y;	vInvDir.z = -vDir.z;	// -D
						FLOAT fLenN = D3DXVec3Dot(&vInvDir, &vN);		// -D dot N
						vN *= (fLenN * 2.0f);
						vTemp = vDir + vN;
						D3DXVec3Scale(&vTemp, &vTemp, 0.2f);

						*pOut = vTemp;

						return 1;
					}
				}
				else
				{
					if (fDist >= 0.0f)
					{
						if (fDist < 1.0f)
						{

							vDir = vEnd - vPos;
							vIntersect = vPos + fDist * vDir;

							vA = *v2 - *v1;
							vB = *v3 - *v1;
							D3DXVec3Cross(&vN, &vA, &vB);
							mInv = mTM;
							mInv._41 = mInv._42 = mInv._43 = 0;
							D3DXVec3TransformCoord(&vN, &vN, &mInv);

							if (vN.x == 0 && vN.z == 0)
								Error("CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다");

							vN.y = 0;
							D3DXVec3Normalize(&vN, &vN);


							vTemp = vEnd - vIntersect;
							*pIntersect = vIntersect;
							CalcSlideVec(&vTemp, vTemp, vN);

							*pOut = vTemp;

							return 1;
						}
					}
				}

			}
		}
	}
	return 0;
#else
			if (bRet && fDist >= 0.0f)
			{


				if (fDist < 1.0f)
				{

					vDir = vEnd - vPos;
					//					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;
					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross(&vN, &vA, &vB);
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord(&vN, &vN, &mInv);

					if (vN.x == 0 && vN.z == 0)
						Error("CActionMover::ProcessCollisionGround : 충돌한 면의 노말이 완전 수직이다");

					vN.y = 0;
					D3DXVec3Normalize(&vN, &vN);


					vTemp = vEnd - vIntersect;
					*pIntersect = vIntersect;
					CalcSlideVec(&vTemp, vTemp, vN);

					*pOut = vTemp;
					//					*pOut = vTemp;

					return 1;
				}
			}
}
}
	return 0;
#endif




}








#if defined( __SLIDE_060502 )

extern BOOL CanSlide(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
	float fCosine, D3DXVECTOR3* pOut);

int	CObject3D::SlideVectorUnder(D3DXVECTOR3* pOut, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vEnd, const D3DXMATRIX& mWorld, D3DXVECTOR3* pIntersect)
{
	D3DXVECTOR3* v1, * v2, * v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir;
	D3DXMATRIX	mInv;
	FLOAT	    fDist;
	BOOL	    bRet;
	NORMALVERTEX* pVB;
	WORD* pIB;

	GMOBJECT* pObject = &m_CollObject;
	if (pObject->m_Type != GMT_NORMAL)
		return 0;

	pIB = pObject->m_pIB;
	pVB = (NORMALVERTEX*)(pObject->m_pVB);

	D3DXMatrixInverse(&mInv, NULL, &mWorld);
	D3DXVec3TransformCoord(&vInvPos, &vPos, &mInv);
	D3DXVec3TransformCoord(&vInvEnd, &vEnd, &mInv);
	vInvDir = vInvEnd - vInvPos;

	for (int j = 0; j < pObject->m_nMaxFaceList; ++j)
	{
		v1 = &pVB[*pIB++].position;
		v2 = &pVB[*pIB++].position;
		v3 = &pVB[*pIB++].position;

		bRet = IsTouchRayTri(v1, v2, v3, &vInvPos, &vInvDir, &fDist);
		if (bRet && fDist >= 0.0f)
		{


			if (fDist < 1.0f)
			{

				D3DXVECTOR3 vDir = vEnd - vPos;
				*pIntersect = vPos + fDist * vDir;


				D3DXVECTOR3 vSlide;
				if (CanSlide(*v1, *v2, *v3, -0.19080f, &vSlide) == FALSE)	// -0.19080f = cos(101)
				{
					*pOut = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					return 1;
				}


				D3DXVec3TransformNormal(&vSlide, &vSlide, &mWorld);
				*pOut = *pIntersect + vSlide;
				return 1;
			}
		}
	}
	return 0;
}
#else // __SLIDE_060502

int		CObject3D::SlideVectorUnder(D3DXVECTOR3* pOut, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vEnd, const D3DXMATRIX& mWorld, D3DXVECTOR3* pIntersect)
{
	D3DXVECTOR3* v1, * v2, * v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX* pVB;
	WORD* pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	//	if( _mUpdate == NULL )		return FAIL;

	if (pObject->m_Type == GMT_ERROR)
		return 0;

	//	for( i = 0; i < nMaxObj; i++ )
	{
		if (pObject->m_Type == GMT_SKIN)
			Error("SlideVectorUnder:스킨오브젝트 발견. 에러%s", m_szFileName);

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pObject++;
		mTM = mWorld;
		//		mTM = m_Group[0].m_pObject[0].m_mLocalTM * mWorld;

		D3DXMatrixInverse(&mInv, NULL, &mTM);
		D3DXVec3TransformCoord(&vInvPos, &vPos, &mInv);
		D3DXVec3TransformCoord(&vInvEnd, &vEnd, &mInv);
		vInvDir = vInvEnd - vInvPos;
		for (j = 0; j < nMaxFace; j++)
		{
			v1 = &pVB[*pIB++].position;
			v2 = &pVB[*pIB++].position;
			v3 = &pVB[*pIB++].position;

			bRet = IsTouchRayTri(v1, v2, v3, &vInvPos, &vInvDir, &fDist);

			if (bRet && fDist >= 0.0f)
			{


				if (fDist < 1.0f)
				{

					vDir = vEnd - vPos;
					*pIntersect = vPos + fDist * vDir;

					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross(&vTemp, &vA, &vB);
					D3DXVec3Normalize(&vN, &vTemp);

					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord(&vTemp, &vN, &mInv);
					vN = vTemp;

					vDir.x = vDir.z = 0;	vDir.y = -1.0f;


					if (D3DXVec3Dot(&vN, &vDir) < -0.19080f)		// -0.19080f = cos(101)
						return 1;
					vDir.y = -0.01f;


					CalcSlideVec(&vTemp, vDir, vN);
					*pOut = *pIntersect + vTemp;
					return 1;
				}
			}
		}
	}
	return 0;
}
#endif // not __SLIDE_060502






void CObject3D::FindTouchTriLine(D3DXVECTOR3** pTriOut, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vEnd, const D3DXMATRIX& mWorld, FLOAT* pDist, BOOL bCollObj)
{
	D3DXVECTOR3* v1, * v2, * v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	NORMALVERTEX* pVB;

	int i;
	LOD_GROUP* pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if (bCollObj == FALSE)
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if (pGroup->_mUpdate == NULL)
			return;
	}

	if (pObject->m_Type == GMT_ERROR)
	{
		*pTriOut = NULL;
		return;
	}

	WORD* pIB;
	int		j;
	BOOL	bRet;

	for (i = 0; i < nMaxObj; i++)
	{
		if (pObject->m_Type == GMT_SKIN)
			Error("FindTouchTriLine:스킨오브젝트 발견. 에러%s", m_szFileName);

		nMaxFace = pObject->m_nMaxFaceList;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pIB = pObject->m_pIB;
		pObject++;
		if (bCollObj == FALSE)
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		D3DXMatrixInverse(&mInv, NULL, &mTM);
		D3DXVec3TransformCoord(&vInvPos, &vPos, &mInv);
		D3DXVec3TransformCoord(&vInvEnd, &vEnd, &mInv);
		vInvDir = vInvEnd - vInvPos;
		for (j = 0; j < nMaxFace; j++)
		{
			v1 = &pVB[*pIB++].position;
			v2 = &pVB[*pIB++].position;
			v3 = &pVB[*pIB++].position;

			bRet = IsTouchRayTri(v1, v2, v3, &vInvPos, &vInvDir, pDist);

			if (bRet && *pDist >= 0.0f)
			{


				if (*pDist < 1.0f)
				{


					pTriOut[0] = v1;
					pTriOut[1] = v2;
					pTriOut[2] = v3;
					return;
				}
			}
		}
	}

	*pTriOut = NULL;
}


#ifdef __CLIENT

static D3DXVECTOR3	s_vLastPickTri[3];
D3DXVECTOR3* GetLastPickTri(void)
{
	return s_vLastPickTri;
}
#endif // __CLIENT


constexpr std::size_t poolsize = 10969;
static D3DXVECTOR3	_vPool[poolsize];

void	CObject3D::ComputeInterval(float fVV0, float fVV1, float fVV2, float fD0, float fD1, float fD2, float fD0D1, float fD0D2, float& fA, float& fB, float& fC, float& fX0, float& fX1)
{
	if (fD0D1 > 0.0f)
	{
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1;
	}
	else if (fD0D2 > 0.0f)
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = (fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if (fD1 * fD2 > 0.0f || fD0 != 0.0f)
	{
		fA = fVV0; fB = (fVV1 - fVV0) * fD0; fC = (fVV2 - fVV0) * fD0; fX0 = fD0 - fD1; fX1 = fD0 - fD2;
	}
	else if (fD1 != 0.0f)
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = (fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if (fD2 != 0.0f)
	{
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1;
	}
	else
	{
		return;
	}

}

BOOL	CObject3D::SimpleTriIntersect(D3DXMATRIX mWorld, GMOBJECT* pTargetObj, D3DXMATRIX mTargetWorld)
{

	D3DXVECTOR3	v1, v2, v3, tv1, tv2, tv3;
	D3DXMATRIX	mTM;
	int			nMax, nMax2;
	int			nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT* pObj = &m_Group[0].m_pObject[0];
	WORD* pIB, * pIB2;
	int			i, j, k;
	NORMALVERTEX* pVB, * pVB2;

	if (m_CollObject.m_Type != GMT_ERROR)
	{
		pObj = &m_CollObject;
		nMaxObj = 1;
	}


	for (i = 0; i < nMaxObj; i++)
	{

		mTM = m_Group[0]._mUpdate[i] * mWorld;

		nMax = pObj->m_nMaxFaceList;
		pVB = (NORMALVERTEX*)(pObj->m_pVB);
		pIB = pObj->m_pIB;
		for (j = 0; j < nMax; j++)
		{

			D3DXVec3TransformCoord(&v1, &pVB[*pIB++].position, &mTM);
			D3DXVec3TransformCoord(&v2, &pVB[*pIB++].position, &mTM);
			D3DXVec3TransformCoord(&v3, &pVB[*pIB++].position, &mTM);

			D3DXVECTOR3	vw1, vw2, vNormal;
			float		fD;

			D3DXVec3Subtract(&vw1, &v2, &v1);
			D3DXVec3Subtract(&vw2, &v3, &v1);
			D3DXVec3Cross(&vNormal, &vw1, &vw2);

			fD = -D3DXVec3Dot(&vNormal, &v1);


			nMax2 = pTargetObj->m_nMaxFaceList;
			pVB2 = (NORMALVERTEX*)(pTargetObj->m_pVB);
			pIB2 = pTargetObj->m_pIB;
			for (k = 0; k < nMax2; k++)
			{


				float fdu0, fdu1, fdu2, ftdu0, ftdu1, ftdu2, fdu01, fdu02, ftdu01, ftdu02;
				D3DXVECTOR3	vtw1, vtw2, vtNormal, vCross;
				float		fD2;

				float		fXLen, fYLen, fZLen, fCood1, fCood2, fCood3, ftCood1, ftCood2, ftCood3;

				D3DXVec3TransformCoord(&tv1, &pVB2[*pIB2++].position, &mTargetWorld);
				D3DXVec3TransformCoord(&tv2, &pVB2[*pIB2++].position, &mTargetWorld);
				D3DXVec3TransformCoord(&tv3, &pVB2[*pIB2++].position, &mTargetWorld);


				fdu0 = D3DXVec3Dot(&vNormal, &tv1) + fD;
				fdu1 = D3DXVec3Dot(&vNormal, &tv2) + fD;
				fdu2 = D3DXVec3Dot(&vNormal, &tv3) + fD;

				fdu01 = fdu0 * fdu1;
				fdu02 = fdu0 * fdu2;
				if (fdu01 > 0.0f && fdu02 > 0.0f)
					continue;

				D3DXVec3Subtract(&vtw1, &tv2, &tv1);
				D3DXVec3Subtract(&vtw2, &tv3, &tv1);
				D3DXVec3Cross(&vtNormal, &vtw1, &vtw2);

				fD2 = -D3DXVec3Dot(&vtNormal, &tv1);


				ftdu0 = D3DXVec3Dot(&vtNormal, &v1) + fD2;
				ftdu1 = D3DXVec3Dot(&vtNormal, &v2) + fD2;
				ftdu2 = D3DXVec3Dot(&vtNormal, &v3) + fD2;

				ftdu01 = ftdu0 * ftdu1;
				ftdu02 = ftdu0 * ftdu2;
				if (ftdu01 > 0.0f && ftdu02 > 0.0f)
					continue;


				D3DXVec3Cross(&vCross, &vNormal, &vtNormal);


				fXLen = fabs(vCross.x);
				fYLen = fabs(vCross.y);
				fZLen = fabs(vCross.z);


				if (fYLen > fXLen && fYLen > fZLen) // y
				{
					fCood1 = v1.y;
					fCood2 = v2.y;
					fCood3 = v3.y;
					ftCood1 = tv1.y;
					ftCood2 = tv2.y;
					ftCood3 = tv3.y;
				}
				if (fZLen > fXLen && fZLen > fYLen) // z
				{
					fCood1 = v1.z;
					fCood2 = v2.z;
					fCood3 = v3.z;
					ftCood1 = tv1.z;
					ftCood2 = tv2.z;
					ftCood3 = tv3.z;
				}
				else								// x
				{
					fCood1 = v1.x;
					fCood2 = v2.x;
					fCood3 = v3.x;
					ftCood1 = tv1.x;
					ftCood2 = tv2.x;
					ftCood3 = tv3.x;
				}

				float a, b, c, x0, x1, d, e, f, y0, y1;
				float xx, yy, xxyy, tmp;
				float isect1[2], isect2[2];


				ComputeInterval(fCood1, fCood2, fCood3, fdu0, fdu1, fdu2, fdu01, fdu02, a, b, c, x0, x1);


				ComputeInterval(ftCood1, ftCood2, ftCood3, ftdu0, ftdu1, ftdu2, ftdu01, ftdu02, d, e, f, y0, y1);

				xx = x0 * x1;
				yy = y0 * y1;
				xxyy = xx * yy;

				tmp = a * xxyy;
				isect1[0] = tmp + b * x1 * yy;
				isect1[1] = tmp + c * x0 * yy;

				tmp = d * xxyy;
				isect2[0] = tmp + e * xx * y1;
				isect2[1] = tmp + f * xx * y0;

				if (isect1[0] > isect1[1])
					tmp = isect1[0];
				isect1[0] = isect1[1];
				isect1[1] = tmp;

				if (isect2[0] > isect2[1])
					tmp = isect2[0];
				isect2[0] = isect2[1];
				isect2[1] = tmp;

				if (isect1[1] < isect2[0] || isect2[1] < isect1[0]) continue;

				return TRUE;
			}
		}
		pObj++;
	}

	return FALSE;
}

D3DXVECTOR3* CObject3D::IntersectRayTri(const D3DXVECTOR3& vRayOrig, const D3DXVECTOR3& vRayDir, const D3DXMATRIX& mWorld,
	D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl)
{
	D3DXVECTOR3* v1, * v2, * v3, * v4;
	D3DXVECTOR3	vInvRayOrig, vInvRayDir, vw1, vw2;
	D3DXMATRIX	mTM, mInv, * pBone, * pBoneInv;
	int		nMax;
	int		nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT* pObj = &m_Group[0].m_pObject[0];
	WORD* pIB, * pIIB;
	int		i, j, nVIdx;
	FLOAT	fDist, fMinDist = 65535.0f;
	BOOL	bRet, bFlag = FALSE;

	if (bColl)
	{
		if (m_CollObject.m_Type != GMT_ERROR)
		{
			pObj = &m_CollObject;
			nMaxObj = 1;
		}
	}

	for (i = 0; i < nMaxObj; i++)
	{
		if (pObj->m_Type == GMT_SKIN)
		{
			SKINVERTEX* pVB;
			mTM = /*_mUpdate[i] * */mWorld;
			D3DXMatrixInverse(&mInv, NULL, &mTM);
			D3DXVec3TransformCoord(&vInvRayOrig, &vRayOrig, &mInv);

			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;
			D3DXVec3TransformCoord(&vInvRayDir, &vRayDir, &mInv);
			int* pPhysique = pObj->m_pPhysiqueVertex;
			D3DXVECTOR3* pVList = pObj->m_pVertexList;
			pIB = pObj->m_pIB;
			pIIB = pObj->m_pIIB;
			pVB = (SKINVERTEX*)pObj->m_pVB;
			if (m_pmExternBone)
			{
				pBone = m_pmExternBone;
				pBoneInv = m_pmExternBoneInv;
			}
			else
			{
				pBone = m_pBaseBone;
				pBoneInv = m_pBaseBoneInv;
			}
			nMax = pObj->m_nMaxVB;

			if (nMax > poolsize)
				continue;

			int	matIdx;
			v4 = _vPool;
			for (j = 0; j < nMax; j++)
			{
				nVIdx = *pIIB++;

				if (nVIdx >= pObj->m_nMaxVertexList)
				{
					Error("%s model is broken", m_szFileName);
					continue;
				}

				matIdx = pPhysique[nVIdx];

				if (matIdx < 0)
				{
					Error("%s model is broken (2)", m_szFileName);
					continue;
				}

				mTM = pBoneInv[matIdx] * pBone[matIdx];
				D3DXVec3TransformCoord(v4, &pVB[j].position, &mTM);
				v4++;
			}
			nMax = pObj->m_nMaxFaceList;
			v4 = _vPool;	// pVB
			for (j = 0; j < nMax; j++)
			{
				v1 = &v4[*pIB++];
				v2 = &v4[*pIB++];
				v3 = &v4[*pIB++];

				bRet = IsTouchRayTri(v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist);
				if (bRet && fDist >= 0.0f)
				{
					if (fDist < fMinDist)
					{
						fMinDist = fDist;
						bFlag = TRUE;
#ifdef __CLIENT
						s_vLastPickTri[0] = *v1;	s_vLastPickTri[1] = *v2;	s_vLastPickTri[2] = *v3;
#endif
					}
				}
			}
		}
		else
		{  // non-skin object
			NORMALVERTEX* pVB;
			mTM = m_Group[0]._mUpdate[i] * mWorld;
			D3DXMatrixInverse(&mInv, NULL, &mTM);
			D3DXVec3TransformCoord(&vInvRayOrig, &vRayOrig, &mInv);

			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;
			D3DXVec3TransformCoord(&vInvRayDir, &vRayDir, &mInv);
			nMax = pObj->m_nMaxFaceList;
			pVB = (NORMALVERTEX*)(pObj->m_pVB);
			pIB = pObj->m_pIB;
			for (j = 0; j < nMax; j++)
			{
				v1 = &pVB[*pIB++].position;
				v2 = &pVB[*pIB++].position;
				v3 = &pVB[*pIB++].position;

				bRet = IsTouchRayTri(v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist);

				if (bRet && fDist >= 0.0f)
				{
					if (fDist < fMinDist)
					{
						fMinDist = fDist;
						bFlag = TRUE;
#ifdef __CLIENT
						D3DXVec3TransformCoord(&s_vLastPickTri[0], v1, &m_Group[0]._mUpdate[i]);
						D3DXVec3TransformCoord(&s_vLastPickTri[1], v2, &m_Group[0]._mUpdate[i]);
						D3DXVec3TransformCoord(&s_vLastPickTri[2], v3, &m_Group[0]._mUpdate[i]);
#endif	// __CLIENT
					}
				}
			}
		}
		pObj++;

	}
	if (bFlag)
	{
		*pvIntersect = vRayOrig + fMinDist * vRayDir;
		return _vPool;
	}

	return NULL;
}


void	CObject3D::SetTexture(LPDIRECT3DTEXTURE9 pTexture)
{
#if !defined(__WORLDSERVER)
	m_Group[0].m_pObject[0].m_pMtrlBlkTexture[0] = pTexture;
#endif
}

void	CObject3D::LoadTextureEx(int nNumEx, GMOBJECT* pObj, MATERIAL* pmMaterial[16])
{
#if !defined(__WORLDSERVER)
	int		i;
	char	szTexture[MAX_PATH];
	char	szFileExt[MAX_PATH];
	char	szNum[16];
	D3DMATERIAL9	mMaterial;

	memset(&mMaterial, 0, sizeof(mMaterial));


	for (i = 0; i < pObj->m_nMaxMaterial; i++)
	{
		if (IsEmpty(pObj->m_MaterialAry[i].strBitMapFileName))	continue;
		strcpy(szTexture, pObj->m_MaterialAry[i].strBitMapFileName);		// mvr_bang.dds
		//		strcpy( szTexture, ::GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName ) );	// mvr_bang
		GetFileTitle(pObj->m_MaterialAry[i].strBitMapFileName, szTexture);	// mvr_bang
		strcat(szTexture, "-et");
		sprintf(szNum, "%02d.", nNumEx);
		strcat(szTexture, szNum);			// -et01 ~ 07
		//		strcat( szTexture, ::GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName ) );		// mvr_bang-et.dds
		GetFileExt(pObj->m_MaterialAry[i].strBitMapFileName, szFileExt);	// mvr_bang-et.dds
		lstrcat(szTexture, szFileExt);

		if (IsEmpty(szTexture) == FALSE)
			pmMaterial[i] = g_TextureMng.AddMaterial(m_pd3dDevice, &mMaterial, szTexture);
	}
#endif // !__WORLDSERVER
}




void	CObject3D::SetTextureEx(GMOBJECT* pObj, int nNumEx)
{
#if !defined(__WORLDSERVER)
	LPDIRECT3DTEXTURE9* pTextureEx;
	int		i;
	MATERIAL* mMaterial[16];
	int		nID;

	if (nNumEx >= 8)
	{
		Error("CObject3D::SetTextureEx : nNumEx = %d", nNumEx);
		return;
	}
	pTextureEx = pObj->m_pMtrlBlkTexture + (pObj->m_nMaxMtrlBlk * nNumEx);

	memset(mMaterial, 0, sizeof(mMaterial));

	if (pTextureEx[0] == NULL)
	{
		LoadTextureEx(nNumEx, pObj, mMaterial);
		for (i = 0; i < pObj->m_nMaxMtrlBlk; i++)
		{
			nID = pObj->m_pMtrlBlk[i].m_nTextureID;
			if (mMaterial[nID])
				pTextureEx[i] = mMaterial[nID]->m_pTexture;
		}
	}

	m_nTextureEx = nNumEx;
#endif
}

void	CObject3D::ClearNormal(void)
{
	/*	int		i;
		D3DXVECTOR3		n = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

		for( int j = 0; j < m_nMaxObject; j++ )
		{
			GMOBJECT* pObject = &m_pObject[j];
			if( pObject->m_Type == GMT_SKIN )
			{
				SKINVERTEX *pVB = (SKINVERTEX*)(pObject->m_pVB);
				for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
			}
			else
			{
				NORMALVERTEX *pVB = (NORMALVERTEX*)(pObject->m_pVB);
				for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
			}

			SendVertexBuffer( pObject, NULL );
		}
		*/
}







void	CObject3D::Animate(FLOAT fFrameCurrent, int nNextFrame)
{
#if !defined(__WORLDSERVER)
	int		i;
	TM_ANIMATION* pFrame = NULL;
	TM_ANIMATION* pNext = NULL;
	D3DXQUATERNION	qSlerp;
	D3DXVECTOR3		vSlerp;
	int		nCurrFrame;
	float	fSlp;
	D3DXMATRIX	m1, m2;
	D3DXMATRIX* mUpdate = m_pGroup->_mUpdate;
	D3DXMATRIX* mParent;

	GMOBJECT* pObj;
	int nMaxObj = m_pGroup->m_nMaxObject;
	for (i = 0; i < nMaxObj; i++)
	{
		pObj = &m_pGroup->m_pObject[i];
		if (pObj->m_ParentType == GMT_BONE)
		{
			if (m_pmExternBone)	mParent = m_pmExternBone;
			else					mParent = m_pBaseBone;
		}
		else
			mParent = m_pGroup->_mUpdate;
		if (m_nMaxFrame > 0)
		{
			if (pObj->m_pFrame)
			{
				nCurrFrame = (int)fFrameCurrent;
				fSlp = fFrameCurrent - (float)nCurrFrame;

				pFrame = &pObj->m_pFrame[nCurrFrame];
				pNext = &pObj->m_pFrame[nNextFrame];

				D3DXQuaternionSlerp(&qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp);
				D3DXVec3Lerp(&vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp);

				D3DXMatrixTranslation(&m1, vSlerp.x, vSlerp.y, vSlerp.z);

				D3DXMatrixRotationQuaternion(&m2, &qSlerp);
				mUpdate[i] = m2 * m1;

				if (pObj->m_nParentIdx != -1)
					mUpdate[i] *= mParent[pObj->m_nParentIdx];

			}
			else
			{
				if (pObj->m_nParentIdx != -1)
					mUpdate[i] = pObj->m_mLocalTM * mParent[pObj->m_nParentIdx];
				else
					mUpdate[i] = pObj->m_mLocalTM;

			}
		}
		else
		{
			if (pObj->m_nParentIdx != -1)
				mUpdate[i] = pObj->m_mLocalTM * mParent[pObj->m_nParentIdx];
			else
				mUpdate[i] = pObj->m_mLocalTM;
		}
	}
#endif // __WORLDSERVER
}

void	CObject3D::Skining(GMOBJECT* pObj, const D3DXMATRIX* mBones)
{

}




HRESULT		CObject3D::SetVertexBuffer(GMOBJECT* pObj)
{

	return S_OK;
}



HRESULT CObject3D::SendVertexBuffer(GMOBJECT* pObj, LPDIRECT3DVERTEXBUFFER9 pd3d_VB)
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	HRESULT	hr;
	VOID* pVertices;
	int		nMax;
	int		nVertexSize;

	nVertexSize = pObj->m_nVertexSize;


	nMax = pObj->m_nMaxVB * nVertexSize;
	/*
		char buff[256] = { 0 };
		sprintf( buff, "%s\t%d,%d : TYPE : %d\n", m_szFileName, pObj->m_nMaxVB, pObj->m_nVertexSize, pObj->m_VBType );
		OutputDebugString( buff );
	*/

	if (FAILED(hr = pd3d_VB->Lock(0, nMax, (void**)&pVertices, 0)))		// send vertex buffer
		return hr;
	memcpy(pVertices, pObj->m_pVB, nMax);

	pd3d_VB->Unlock();

	return S_OK;
}




HRESULT CObject3D::SendIndexBuffer(GMOBJECT* pObj)
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	VOID* pVertices;
	int		nMax;


	nMax = pObj->m_nMaxIB * sizeof(WORD);
	if (FAILED(pObj->m_pd3d_IB->Lock(0, nMax, (void**)&pVertices, 0)))	// send index buffer
		return FAIL;

	memcpy(pVertices, pObj->m_pIB, nMax);
	pObj->m_pd3d_IB->Unlock();

	return S_OK;
}


#ifndef __WORLDSERVER
void CObject3D::SetState(MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor)
{
	// set
	if (pBlock->m_dwEffect & XE_2SIDE)
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	if (m_nNoEffect == 1)	return;

	if (dwBlendFactor != 0xff000000)
		//if( nBlendFactor < 255 )
	{
		if (m_nNoEffect)	return;
		DWORD dwBlendF = dwBlendFactor;
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );

		//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		//pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nBlendF, 0, 0, 0) );
		m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwBlendF);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	}
	else
		if ((pBlock->m_dwEffect & XE_OPACITY))
		{
			m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xb0);
			//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS   );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			if (m_nNoEffect)	return;

			if (pBlock->m_nAmount < 255)
			{
				m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(pBlock->m_nAmount, 0, 0, 0));
				m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
				m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

				m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			}

		}

	// Set Reflection
	if ((pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT))
	{
		if (m_nNoEffect)	return;
		m_pd3dDevice->SetTransform(D3DTS_TEXTURE1, &g_mReflect);
		extern LPDIRECT3DTEXTURE9 g_pReflectMap;
		m_pd3dDevice->SetTexture(1, g_pReflectMap);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	else
		if (pBlock->m_dwEffect & XE_SELF_ILLUMINATE)
		{
			m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		}
		else
			// Set Specular
			if ((pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR))
			{
				if (m_nNoEffect)	return;
				int a = 0;
			}
			else
				if ((pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP))
				{
					if (m_nNoEffect)	return;
					int a = 0;
				}
				else
					if ((pBlock->m_dwEffect & XE_HIGHLIGHT_OBJ) || (nEffect & XE_HIGHLIGHT_OBJ))
					{
						if (m_nNoEffect)	return;
						//		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
						m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
						m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
						m_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
					}

	//	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	//	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}




void CObject3D::ResetState(MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor)
{
	// reset
	if (pBlock->m_dwEffect & XE_2SIDE)
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (m_nNoEffect == 1)	return;

	//if( nBlendFactor == 255 )
	if (dwBlendFactor == 0xff000000)
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		if (m_nNoEffect)	return;
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		if (m_nNoEffect)	return;
		m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xb0);
		m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	}

	if ((pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT))
	{
		if (m_nNoEffect)	return;
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	else if (pBlock->m_dwEffect & XE_SELF_ILLUMINATE)
	{
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, s_bLight);
	}
	else if ((pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR))
	{
		if (m_nNoEffect)	return;
	}
	else if ((pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP))
	{
		if (m_nNoEffect)	return;
		int a = 0;
	}


	m_pd3dDevice->SetTexture(0, NULL);
}


void	CObject3D::RenderSkin(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pd3d_VB, GMOBJECT* pObj, const D3DXMATRIX* mWorld, int nEffect, DWORD dwBlendFactor)
{

	if (g_bUsableVS == FALSE)
		pd3dDevice->SetSoftwareVertexProcessing(TRUE);

	MATERIAL_BLOCK* pBlock;
	int		nMaxMtrl, nMaxVB, nMaxBone;
	D3DXMATRIX* pBone;
	D3DXMATRIX* pmBonesInv;
	int		i, idx;

	if (m_nNoEffect == 0)
	{
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	D3DXMATRIX	m1;

	if (m_pmExternBone)
	{
		pBone = m_pmExternBone;
		pmBonesInv = m_pmExternBoneInv;
	}
	else
	{
		pBone = m_pBaseBone;
		pmBonesInv = m_pBaseBoneInv;
	}

	if (pObj->m_nMaxUseBone)
	{
		nMaxBone = pObj->m_nMaxUseBone;
		for (i = 0; i < nMaxBone; i++)
		{
			idx = pObj->m_UseBone[i];
			m1 = pmBonesInv[idx] * pBone[idx];

			D3DXMatrixTranspose(&m1, &m1);
			pd3dDevice->SetVertexShaderConstantF(i * 3, (float*)&m1, 3);

		}
	}

	pd3dDevice->SetVertexDeclaration(g_pSkinVertexDeclaration);

	HRESULT hr = pd3dDevice->SetVertexShader(g_pSkiningVS);

	{
		pd3dDevice->SetStreamSource(0, pd3d_VB, 0, sizeof(SKINVERTEX));
	}


	pd3dDevice->SetIndices(pObj->m_pd3d_IB);

	pd3dDevice->SetTransform(D3DTS_WORLD, mWorld);

	LPDIRECT3DTEXTURE9* pBlkTexture = pObj->m_pMtrlBlkTexture;

	if (m_nTextureEx)
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	pBlock = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB = pObj->m_nMaxVB;
	while (nMaxMtrl--)
	{
		if (pBlock->m_nMaxUseBone)
		{
			nMaxBone = pBlock->m_nMaxUseBone;
			for (i = 0; i < nMaxBone; i++)
			{
				idx = pBlock->m_UseBone[i];
				m1 = pmBonesInv[idx] * pBone[idx];

				D3DXMatrixTranspose(&m1, &m1);
				pd3dDevice->SetVertexShaderConstantF(i * 3, (float*)&m1, 3);
			}
		}

		SetState(pBlock, nEffect, dwBlendFactor);

		if (m_nNoTexture && pObj->m_bOpacity == 0)
			pd3dDevice->SetTexture(0, NULL);
		else
			pd3dDevice->SetTexture(0, *pBlkTexture);

		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount);

		ResetState(pBlock, nEffect, dwBlendFactor);

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock++;
		pBlkTexture++;
	}


	//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, s_bFog );
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetVertexDeclaration(NULL);

#ifdef __CLIENT
	if (g_Neuz.m_d3dSettings.GetVertexProcessingType() != SOFTWARE_VP && g_bUsableVS == FALSE)
		pd3dDevice->SetSoftwareVertexProcessing(FALSE);
#endif

}







void	CObject3D::RenderNormal(LPDIRECT3DDEVICE9 pd3dDevice, GMOBJECT* pObj, const D3DXMATRIX* mWorld, int nEffect, int nBlendFactor)
{
	MATERIAL_BLOCK* pBlock;
	int		nMaxMtrl, nMaxVB;
	pd3dDevice->SetVertexShader(NULL);

	pd3dDevice->SetVertexDeclaration(NULL);

	if (m_nNoEffect == 0)
	{
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		if (CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING)
		{
			if (stricmp(m_szFileName, "obj_macoprtr16.o3d") == 0 || stricmp(m_szFileName, "obj_macoprtr17.o3d") == 0)
			{
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
			}
			else
			{
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}
		}

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}

	LPDIRECT3DTEXTURE9* pBlkTexture = pObj->m_pMtrlBlkTexture;
	if (m_nTextureEx)
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);

	{
		pd3dDevice->SetFVF(D3DFVF_NORMALVERTEX);
		pd3dDevice->SetStreamSource(0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX));
	}

	pd3dDevice->SetIndices(pObj->m_pd3d_IB);

	pd3dDevice->SetTransform(D3DTS_WORLD, mWorld);

	pBlock = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB = pObj->m_nMaxVB;

	while (nMaxMtrl--)
	{
		if (pObj->m_bLight)
		{
			if (m_nNoEffect == 0)
			{
				pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}
		}
		else
			SetState(pBlock, nEffect, nBlendFactor);

		{
			if (m_nNoTexture && pObj->m_bOpacity == 0)
				pd3dDevice->SetTexture(0, NULL);
			else
				pd3dDevice->SetTexture(0, *pBlkTexture);
		}

		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount);

		if (pObj->m_bLight)
		{
			if (m_nNoEffect == 0)
			{
				pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

				pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_LIGHTING, s_bLight);
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
		}
		else
			ResetState(pBlock, nEffect, nBlendFactor);

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock++;
		pBlkTexture++;
	}
}

void	CObject3D::SetShader(const D3DXMATRIX* mWorld)
{
	int		i;
	if (m_pBaseBone == NULL)	return;

	D3DXMATRIX* pmBones = m_pBaseBone;
	D3DXMATRIX* pmBonesInv = m_pBaseBoneInv;
	D3DXMATRIX mWorldTranspose;

	if (m_bSendVS)
	{
		int		nMaxBone = m_nMaxBone;
		if (nMaxBone > MAX_VS_BONE)
			Error("CModelObject::Render : 뼈대개수가 최대치를 초과했다. %d", nMaxBone);

		for (i = 0; i < nMaxBone; i++)
		{
			mWorldTranspose = pmBonesInv[i] * pmBones[i];

			D3DXMatrixTranspose(&mWorldTranspose, &mWorldTranspose);
			m_pd3dDevice->SetVertexShaderConstantF(i * 3, (float*)&mWorldTranspose, 3);
		}
	}

	D3DXMATRIX	mView, mProj;
	D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;
	D3DXVECTOR4 vLight = s_vLight;
	mViewProj = *mWorld * s_mView * s_mProj;
	D3DXMatrixTranspose(&mViewProjTranspose, &mViewProj);
	D3DXMatrixTranspose(&mWorldTranspose, mWorld);

	D3DXMatrixInverse(&mInvWorld, NULL, mWorld);
	D3DXVec4Transform(&vLight, &vLight, &mInvWorld);
	//	D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );
	D3DXVec4Normalize(&vLight, &vLight);

	m_pd3dDevice->SetVertexShaderConstantF(84, (float*)&mViewProjTranspose, 4);
	m_pd3dDevice->SetVertexShaderConstantF(92, (float*)&vLight, 1);
	m_pd3dDevice->SetVertexShaderConstantF(93, (float*)&s_fDiffuse, 1);
	m_pd3dDevice->SetVertexShaderConstantF(94, (float*)&s_fAmbient, 1);
}

void	CObject3D::Render(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9* ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX* mWorld, int nEffect, DWORD dwBlendFactor)
{
	int			i;
	int			nMax = m_pGroup->m_nMaxObject;
	GMOBJECT* pObj;
	D3DXMATRIX	m1;
	D3DXMATRIX* pBone = NULL;

	if (m_pGroup->_mUpdate)
		Animate(fFrameCurrent, nNextFrame);
	if (m_nMaxFrame > 0)
	{
		if (m_pmExternBone == NULL)
		{
			if (m_pMotion)
				m_pMotion->AnimateBone(m_pBaseBone, NULL, fFrameCurrent, nNextFrame, 0, FALSE, 0.0f);
		}
	}
	if (m_pBaseBone)
		SetShader(mWorld);

	for (i = 0; i < nMax; i++)
	{
		pObj = &m_pGroup->m_pObject[i];

		if (pObj->m_nVertexSize == 0 || pObj->m_dwFVF == 0)
			Error("%s : FVF가 기술되지 않았다. VertexSize=%d, FVF=%d", m_szFileName, pObj->m_nVertexSize, pObj->m_dwFVF);

		if (pObj->m_Type == GMT_SKIN)
		{
			if (ppd3d_VB == NULL)
				Error("CObject3D::Render : %s 스킨오브젝트인데 ppd3d_VB가 없다.", m_szFileName);

			RenderSkin(pd3dDevice, ppd3d_VB[i], pObj, mWorld, nEffect, dwBlendFactor);

		}
		else
		{
			m1 = m_pGroup->_mUpdate[i] * *mWorld;
			if (pObj->m_bLight && s_bNight == FALSE)
				continue;
			RenderNormal(pd3dDevice, pObj, &m1, nEffect, dwBlendFactor);
		}
	}
	if (m_nNoTexture == 0)
	{
		pd3dDevice->SetTexture(1, NULL);
		pd3dDevice->SetTexture(0, NULL);
	}

}

#endif // __WORLDSERVER


