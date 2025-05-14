#include "stdafx.h"
#include "Region.h"
#include "ModelGlobal.h"
#include "..\_Common\ParticleMng.h"
#include "..\_Common\TailEffectMng.h"

#include "defineskill.h"
#include "WorldMap.h"
#include "defineObj.h"
#if __VER >= 13 // __HOUSING
#include "housing.h"
#endif // __HOUSING

inline DWORD ObjTypeToObjFilter(DWORD dwType)
{
	static DWORD m_dwFilter[] = { OF_OBJ, OF_ANI, OF_CTRL, OF_SFX, OF_ITEM, OF_MOVER, OF_REGION, OF_SHIP };
	return m_dwFilter[dwType];
}


LPDIRECT3DINDEXBUFFER9 g_pIB;
LPDIRECT3DINDEXBUFFER9 g_pExIB;
LPDIRECT3DINDEXBUFFER9 g_pGridIB;
WORD g_anPrimitive[3] = { 32,8,2 };
WORD g_anPrimitiveA[3] = { 12,4,1 };
WORD g_anStartIndex[4] = { 0,(128 + 48) * 3,(128 + 48 + 32 + 16) * 3,(128 + 48 + 32 + 16 + 8 + 4) * 3 };

CCamera* CWorld::m_pCamera = NULL;
BOOL       CWorld::m_bViewLODTerrain = TRUE;
BOOL       CWorld::m_bMiniMapRender = FALSE;
#ifdef __CLIENT
CSkyBox    CWorld::m_skyBox;
BOOL	   CWorld::m_bZoomView = FALSE;
int		   CWorld::m_nZoomLevel = 0;
#endif

D3DCOLOR   CWorld::m_dwBgColor = D3DCOLOR_XRGB(0xe0, 0xe0, 0xff);
D3DXMATRIX CWorld::m_matProj;
D3DLIGHT9  CWorld::m_light;
D3DLIGHT9  CWorld::m_lightFogSky;
FLOAT CWorld::m_fFarPlane = 512.0f;
FLOAT CWorld::m_fNearPlane = 0.5f;



CULLSTATE CullObject(CULLINFO* pCullInfo, CObj* pObj);
BOOL EdgeIntersectsFace(D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, D3DXPLANE* pPlane);

float fDiv = 4.0f;

void CWorld::Projection(LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight)
{

	FLOAT fAspect = (FLOAT)nWidth / (FLOAT)nHeight;
	float fFov = D3DX_PI / fDiv;
	extern int g_nFlySpeed;

	float fNear = CWorld::m_fNearPlane;

	if (CMover::GetActiveMover())
	{
		if (CMover::GetActiveMover()->m_pActMover->IsFly() && (CMover::GetActiveMover()->m_pActMover->IsStateFlag(OBJSTAF_ACC)))
		{
			if (CMover::GetActiveMover()->m_pActMover->IsStateFlag(OBJSTAF_TURBO))
			{
				fDiv -= 0.2f;
				if (fDiv <= 2.0f)	fDiv = 2.0f;
			}
			else
			{
				fDiv += 0.05f;
				if (fDiv >= 4.0f)		fDiv = 4.0f;
			}
		}
		else
		{
			switch (m_nZoomLevel)
			{
			case 0:
				fDiv = 4.0f;
				break;
			case 1:
				fDiv = 10.0f;
				break;
			case 2:
				fDiv = 25.0f;
				break;
			case 3:
				fDiv = 50.0f;
				break;
			}
		}
	}

	D3DXMatrixPerspectiveFovLH(&m_matProj, fFov, fAspect, fNear - 0.01f, CWorld::m_fFarPlane);

	pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}



void CWorld::RenderBase(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont)
{
#ifdef __CLIENT

#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);


	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


	if (m_bViewWireframe)
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		m_pd3dDevice->SetTexture(0, NULL);
	}
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#ifdef __CLIENT

	CHECK1();
	if (m_bViewSkybox)
	{
		m_skyBox.Render(this, pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	CHECK2("  Render SkyBox");
#endif

	m_pCamera->Transform(pd3dDevice, this);

	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 );
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	float fBias = -0.0f;
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 2);
	//m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS , *((LPDWORD) (&fBias)));
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, FtoDW(fBias));
	//m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS , 0 );

	SetLight(TRUE);

	SetFogEnable(m_pd3dDevice, m_bViewFog);


	CHECK1();
	if (g_Option.m_nShadow < 2) SetStateShadowMap(m_pd3dDevice, 2, m_pCamera->m_matView);
	RenderTerrain();
	if (g_Option.m_nShadow < 2) ResetStateShadowMap(m_pd3dDevice, 2);
	CHECK2("  Render Terrain");

	if (m_bViewLight != TRUE)
		SetLight(m_bViewLight);


	RenderObject(pFont);
#endif
}


void CWorld::Render(LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont)
{
#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);


	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


	if (m_bViewWireframe)
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		m_pd3dDevice->SetTexture(0, NULL);
	}
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#ifdef __CLIENT

	CHECK1();
	if (m_bViewSkybox)
	{
		m_skyBox.Render(this, pd3dDevice);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	CHECK2("  Render SkyBox");
#endif

	m_pCamera->Transform(pd3dDevice, this);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	float fBias = -0.0f;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, FtoDW(fBias));

	SetLight(TRUE);
	SetFogEnable(m_pd3dDevice, m_bViewFog);

	CHECK1();
	if (g_Option.m_nShadow < 2) SetStateShadowMap(m_pd3dDevice, 2, m_pCamera->m_matView);
	RenderTerrain();
	if (g_Option.m_nShadow < 2) ResetStateShadowMap(m_pd3dDevice, 2);

	CHECK2("  Render Terrain");

	if (TRUE != m_bViewLight)
		SetLight(m_bViewLight);

	RenderGrid();

	RenderObject(pFont);

	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255)); //m_dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );

	g_GameTimer.GetMoonPercent();

	CHECK1();
	m_skyBox.RenderFall(pd3dDevice);
	if (m_bViewSkybox) //&& g_GameTimer.GetSunPercent() )
	{
		m_skyBox.DrawLensFlare(pd3dDevice);
	}
	CHECK2("  Render LensFlare");


	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	SetFogEnable(pd3dDevice, FALSE);
}

int ObjSortNearToFar(const VOID* arg1, const VOID* arg2)
{
	D3DXVECTOR3 vPos1 = (*(CObj**)arg1)->GetPos();
	D3DXVECTOR3 vPos2 = (*(CObj**)arg2)->GetPos();
	D3DXVECTOR3 vDir = (*(CObj**)arg1)->GetWorld()->m_pCamera->GetPos();//m_vPos2;

	//FLOAT d1 = vPos1.x * vDir.x + vPos1.z * vDir.z;
	//FLOAT d2 = vPos2.x * vDir.x + vPos2.z * vDir.z;

//	FLOAT d1 =  sqrt( (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z ) );
//	FLOAT d2 =  sqrt( (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z ) );
	FLOAT d1 = (vPos1.x - vDir.x) * (vPos1.x - vDir.x) + (vPos1.z - vDir.z) * (vPos1.z - vDir.z);
	FLOAT d2 = (vPos2.x - vDir.x) * (vPos2.x - vDir.x) + (vPos2.z - vDir.z) * (vPos2.z - vDir.z);

	if (d1 > d2)
		return +1;

	return -1;
}

int ObjSortFarToNear(const VOID* arg1, const VOID* arg2)
{
#ifndef __CSC_UPDATE_WORLD3D
	D3DXVECTOR3 vPos1 = (*(CObj**)arg1)->GetPos();
	D3DXVECTOR3 vPos2 = (*(CObj**)arg2)->GetPos();
	D3DXVECTOR3 vDir = (*(CObj**)arg1)->GetWorld()->m_pCamera->GetPos();//m_vPos2;

	//FLOAT d1 = vPos1.x * vDir.x + vPos1.z * vDir.z;
	//FLOAT d2 = vPos2.x * vDir.x + vPos2.z * vDir.z;

//	FLOAT d1 =  sqrt( (vPos1.x - vDir.x ) * ( vPos1.x - vDir.x ) + ( vPos1.z - vDir.z ) * ( vPos1.z - vDir.z ) );
//	FLOAT d2 =  sqrt( (vPos2.x - vDir.x ) * ( vPos2.x - vDir.x ) + ( vPos2.z - vDir.z ) * ( vPos2.z - vDir.z ) );



	FLOAT d1 = (vPos1.x - vDir.x) * (vPos1.x - vDir.x) + (vPos1.z - vDir.z) * (vPos1.z - vDir.z);
	FLOAT d2 = (vPos2.x - vDir.x) * (vPos2.x - vDir.x) + (vPos2.z - vDir.z) * (vPos2.z - vDir.z);

	if (d1 < d2)
		return +1;

	return -1;
#endif //__CSC_UPDATE_WORLD3D

#ifdef __CSC_UPDATE_WORLD3D
	float	fDist1 = (*(CObj**)arg1)->m_fDistCamera;
	float	fDist2 = (*(CObj**)arg2)->m_fDistCamera;

	if (fDist1 < fDist2)
		return 1;
	else if (fDist1 > fDist2)
		return -1;
	else
		return 0;
#endif //__CSC_UPDATE_WORLD3D
}


int ObjSortFarToNear2(const VOID* arg1, const VOID* arg2)
{
	float	fDist1 = (*(CObj**)arg1)->m_fDistCamera;
	float	fDist2 = (*(CObj**)arg2)->m_fDistCamera;

#ifdef __CSC_UPDATE_WORLD3D
	if (fDist1 < fDist2)
		return -1;
	else if (fDist1 > fDist2)
		return 1;
	else
		return 0;
#else //__CSC_UPDATE_WORLD3D
	if (fDist1 < fDist2)
		return 1;

	return -1;
#endif //__CSC_UPDATE_WORLD3D
}

void CWorld::RenderTerrain()
{
	int x, z, i, j;
	int px, pz;
	WorldPosToLand(m_pCamera->m_vPos, x, z);

	for (i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for (j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
		{
			int nOffset = i * m_nLandWidth + j;
			if (LandInWorld((int)j, (int)i) && m_apLand[nOffset])
			{
				m_apLand[nOffset]->UpdateCull();
			}
		}
	}

	if (m_bViewTerrain)
	{
		if (CWorld::m_bViewLODTerrain)
		{
			for (i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
			{
				for (j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
				{
					int nOffset = i * m_nLandWidth + j;
					if (LandInWorld((int)j, (int)i) && m_apLand[nOffset])
					{
						if (m_apLand[nOffset]->isVisibile())
						{
							if (j > 0 && LandInWorld((int)j - 1, (int)i) && m_apLand[nOffset - 1])
								for (pz = 0; pz < NUM_PATCHES_PER_SIDE; pz++)
									if (m_apLand[nOffset]->m_aPatches[pz][0].isVisibile())
										if (m_apLand[nOffset]->m_aPatches[pz][0].m_nLevel < m_apLand[nOffset - 1]->m_aPatches[pz][NUM_PATCHES_PER_SIDE - 1].m_nLevel)
											m_apLand[nOffset]->m_aPatches[pz][0].m_nRightLevel = m_apLand[nOffset - 1]->m_aPatches[pz][NUM_PATCHES_PER_SIDE - 1].m_nLevel;
							if (i > 0 && LandInWorld((int)j, (int)i - 1) && m_apLand[nOffset - m_nLandWidth])
								for (px = 0; px < NUM_PATCHES_PER_SIDE; px++)
									if (m_apLand[nOffset]->m_aPatches[0][px].isVisibile())
										if (m_apLand[nOffset]->m_aPatches[0][px].m_nLevel < m_apLand[nOffset - m_nLandWidth]->m_aPatches[NUM_PATCHES_PER_SIDE - 1][px].m_nLevel)
											m_apLand[nOffset]->m_aPatches[0][px].m_nTopLevel = m_apLand[nOffset - m_nLandWidth]->m_aPatches[NUM_PATCHES_PER_SIDE - 1][px].m_nLevel;
							if (j < m_nLandWidth - 1 && LandInWorld((int)j + 1, (int)i) && m_apLand[nOffset + 1])
								for (pz = 0; pz < NUM_PATCHES_PER_SIDE; pz++)
									if (m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE - 1].isVisibile())
										if (m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE - 1].m_nLevel < m_apLand[nOffset + 1]->m_aPatches[pz][0].m_nLevel)
											m_apLand[nOffset]->m_aPatches[pz][NUM_PATCHES_PER_SIDE - 1].m_nLeftLevel = m_apLand[nOffset + 1]->m_aPatches[pz][0].m_nLevel;
							if (i < m_nLandWidth - 1 && LandInWorld((int)j, (int)i + 1) && m_apLand[nOffset + m_nLandWidth])
								for (px = 0; px < NUM_PATCHES_PER_SIDE; px++)
									if (m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE - 1][px].isVisibile())
										if (m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE - 1][px].m_nLevel < m_apLand[nOffset + m_nLandWidth]->m_aPatches[0][px].m_nLevel)
											m_apLand[nOffset]->m_aPatches[NUM_PATCHES_PER_SIDE - 1][px].m_nBottomLevel = m_apLand[nOffset + m_nLandWidth]->m_aPatches[0][px].m_nLevel;
						}
					}
				}
			}
		}

		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		////////

		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


		float fBias = -.7f;

		for (i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
		{
			for (j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
			{
				int nOffset = i * m_nLandWidth + j;
				if (LandInWorld((int)j, (int)i) && m_apLand[nOffset])
				{
					if (m_apLand[nOffset]->isVisibile())
						m_apLand[nOffset]->Render(m_pd3dDevice);
				}
			}
		}
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		m_pd3dDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
}
void CWorld::RenderWater()
{
	if (!m_bViewWater)
		return;

	int x, z, i, j;
	//		int px,pz;
	WorldPosToLand(m_pCamera->m_vPos, x, z);

	for (i = z - m_nVisibilityLand; i <= z + m_nVisibilityLand; i++)
	{
		for (j = x - m_nVisibilityLand; j <= x + m_nVisibilityLand; j++)
		{
			int nOffset = i * m_nLandWidth + j;
			if (LandInWorld((int)j, (int)i) && m_apLand[nOffset])
			{
				if (m_apLand[nOffset]->isVisibile())
				{
					m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, m_bViewLight);
					m_apLand[nOffset]->RenderWater(m_pd3dDevice);
				}
			}
		}
	}
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, m_bViewLight);
}

void CWorld::RenderObject(CD3DFont* pFont)
{
	int i = 0, nObjRegion = 0;
	static D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 100.0f);

	for (i = 0; i < m_nObjCullSize; i++)
		if (m_aobjCull[i])
			m_aobjCull[i]->m_ppViewPtr = NULL;
	for (i = 0; i < m_nSfxCullSize; i++)
		if (m_asfxCull[i])
			m_asfxCull[i]->m_ppViewPtr = NULL;
	//#endif
	m_nObjCullSize = 0;
	m_nSfxCullSize = 0;

	int	 nCount = 0;

	if (CDeployManager::GetInstance()->IsReady())
		CDeployManager::GetInstance()->Process();

	if (GuildDeploy()->IsReady())
		GuildDeploy()->Process();

	if (m_bViewAllObjects)
	{
		CHECK1();
		CObj* pObj;
		CLandscape* pLand;
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, m_bViewLight);
		int _nCount = 0, _nCount2 = 0;

		FOR_LAND(this, pLand, m_nVisibilityLand, FALSE)
		{
			FOR_OBJARRAY(pLand, pObj)
			{
				D3DXVECTOR3* pv, * pvCamera;
				float xDist, yDist, zDist;
				pvCamera = &(m_pCamera->GetPos());
				pv = &(pObj->GetPos());
				xDist = (pv->x - pvCamera->x);
				yDist = (pv->y - pvCamera->y);
				zDist = (pv->z - pvCamera->z);

				LPMODELELEM lpModelElem = prj.m_modelMng.GetModelElem(pObj->m_dwType, pObj->m_dwIndex);
				if (lpModelElem)
				{
					if (g_Option.m_bSFXRenderOff && lpModelElem->m_dwType == OT_SFX)
					{
						if (lpModelElem->m_bRenderFlag != 1)
							continue;
					}
				}

				if (pObj->GetType() == OT_MOVER)
				{
					CMover* pMover = (CMover*)pObj;
					if (pMover->m_dwMode & GCWAR_RENDER_SKIP_MODE)
						continue;
				}

				pObj->m_fDistCamera = sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
				{
					static FLOAT fDistant[3][4] = {
						{ 550, 300, 110, 450 },
						{ 450, 250, 80, 450 },
						{ 200, 125, 50, 425 } };
					if (m_bViewLODObj)
					{
						if (m_bMiniMapRender == FALSE && pObj->m_fDistCamera > fDistant[g_Option.m_nObjectDistant][pObj->GetModel()->m_pModelElem->m_dwDistant])
						{
							continue;
						}
					}
					else
						pObj->m_fDistCamera = 0;
				}

				BOOL bInside = FALSE;
				if (pObj == g_pPlayer)
					bInside = TRUE;

				if (!pLand->isVisibile() && pObj->GetRadius() > 28.0f)
				{
					D3DXVECTOR3 vDir = (*pv) - (*pvCamera);
					D3DXVECTOR3 vLookDir = m_pCamera->m_vLookAt - m_pCamera->GetPos();
					D3DXVec3Normalize(&vDir, &vDir);
					D3DXVec3Normalize(&vLookDir, &vLookDir);

					float fDot = D3DXVec3Dot(&vDir, &vLookDir);

					if (fDot >= -0.2f)
						bInside = TRUE;
				}
				if (pLand->isVisibile() || bInside)
				{
					if (!(m_bMiniMapRender == TRUE && pObj->GetType() != OT_OBJ))
					{
						if (pObj->IsUpdateMatrix())
							pObj->UpdateMatrix();
						pObj->SetCullState(CS_UNKNOWN);

						if (m_nObjCullSize < MAX_DISPLAYOBJ)
						{
							if (pObj->m_pModel != NULL && pObj->m_pModel->GetModelType() != MODELTYPE_SFX)
							{
								if (m_bCullObj)
								{
									_nCount++;
									pObj->m_cullstate
										= CullObject(&g_cullinfo, pObj);

									if (pObj->IsCull() == FALSE)
										m_aobjCull[m_nObjCullSize++] = pObj;
								}
								else
									m_aobjCull[m_nObjCullSize++] = pObj;
							}
							else
							{
								if (m_nSfxCullSize < MAX_DISPLAYSFX)
								{
									if (m_bCullObj)
									{
										_nCount++;
										pObj->m_cullstate = CullObject(&g_cullinfo, pObj);
										if (pObj->IsCull() == FALSE)
										{
											m_asfxCull[m_nSfxCullSize++] = pObj;
										}
									}
									else
									{
										m_asfxCull[m_nSfxCullSize++] = pObj;
									}
								}
							}
						}
					}
				}
			}
			END_OBJARRAY
		}
		END_LAND

			CHECK2("Assert Obj");

		CHECK1();
		qsort(m_aobjCull, m_nObjCullSize, sizeof(CObj*), ObjSortFarToNear2);
		for (i = 0; i < m_nObjCullSize; i++)
			m_aobjCull[i]->m_ppViewPtr = &m_aobjCull[i];

		CHECK2("Sort");
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		::SetLight(m_bViewLight);
		::SetFog(m_bViewFog);

		CHECK1();
		m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		{
			::SetTransformView(m_pCamera->m_matView);
			::SetTransformProj(m_matProj);

			SetDiffuse(m_light.Diffuse.r, m_light.Diffuse.g, m_light.Diffuse.b);
			SetAmbient(m_light.Ambient.r + 0.4f, m_light.Ambient.g + 0.4f, m_light.Ambient.b + 0.4f);

			SetLightVec(m_light.Direction);
			if (m_bViewFog)
			{
				m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bViewFog);
				m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
				m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
				m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
			}
			g_nMaxTri = 0;

			for (i = 0; i < m_nObjCullSize; i++)
			{
				pObj = m_aobjCull[i];

				if (pObj->GetType() == OT_MOVER)
					continue;

				if (g_Option.m_nShadow < 2)
				{
					if (pObj->GetType() == OT_OBJ)
						continue;
				}

				if (pObj->m_wBlendFactor < 255)
					continue;

				if (m_bViewFog)
				{
					if (pObj->m_pModel->m_bSkin)
					{
						vConst.w = (m_fFogEndValue - pObj->m_fDistCamera) / (m_fFogEndValue - m_fFogStartValue);
						m_pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
						SetLightVec(m_light.Direction);
					}
					else
						SetAmbient(m_light.Ambient.r + 0.3f, m_light.Ambient.g + 0.3f, m_light.Ambient.b + 0.3f);
				}

#ifdef __FL_UPDATE_GROUNDITEMS
				if (pObj->GetType() == OT_ITEM)
				{
					DWORD testDword;
					m_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &testDword);
					m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 175, 175, 175));
					pObj->Render(m_pd3dDevice);
					m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, testDword);
				}
				else
					pObj->Render(m_pd3dDevice);
#else
				pObj->Render(m_pd3dDevice);
#endif


				if (m_bViewBoundBox)
				{
					SetBoundBoxVertex(pObj);
					RenderBoundBoxVertex(pObj);
				}
			}

			if (g_Option.m_nShadow < 2)
			{
				bool bRenderedShadow = false;
				D3DXVECTOR3 kMyPos = g_pPlayer->GetPos();
				for (i = 0; i < m_nObjCullSize; i++)
				{
					pObj = m_aobjCull[i];

					if (!pObj)
						continue;

					if (pObj->GetType() != OT_OBJ)
						continue;

					if (pObj->m_wBlendFactor < 255)
						continue;

					if (pObj->IsRangeObj(kMyPos, 50.0f))
					{
						if (pObj->GetPos().y < (kMyPos.y + 0.5f))
						{
							if (!IsWorldGuildHouse())
							{
								SetStateShadowMap(m_pd3dDevice, 2, m_pCamera->m_matView);
								bRenderedShadow = true;
							}
						}
					}

					if (m_bViewFog)
					{
						if (pObj->m_pModel->m_bSkin)
						{
							vConst.w = (m_fFogEndValue - pObj->m_fDistCamera) / (m_fFogEndValue - m_fFogStartValue);
							m_pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
							SetLightVec(m_light.Direction);
						}
					}

					if (pObj->m_pModel->m_bSkin)
						ResetStateShadowMap(m_pd3dDevice, 2);

					pObj->Render(m_pd3dDevice);

					if (pObj->m_pModel->m_bSkin)
						SetStateShadowMap(m_pd3dDevice, 2, m_pCamera->m_matView);

					if (m_bViewBoundBox)
					{
						SetBoundBoxVertex(pObj);
						RenderBoundBoxVertex(pObj);
					}

					if (bRenderedShadow)
					{
						bRenderedShadow = false;
						ResetStateShadowMap(m_pd3dDevice, 2);
					}
				}

				ResetStateShadowMap(m_pd3dDevice, 2);
			}


			for (i = 0; i < m_nObjCullSize; i++)
			{
				pObj = m_aobjCull[i];
				if (!pObj)
					continue;

				if (pObj->GetType() != OT_MOVER)
					continue;

				if (pObj->m_wBlendFactor < 255)
					continue;

				if (pObj->IsActiveObj() && m_nZoomLevel != 0)
					continue;

				if (m_bViewFog)
				{
					if (pObj->m_pModel->m_bSkin)
					{
						vConst.w = (m_fFogEndValue - pObj->m_fDistCamera) / (m_fFogEndValue - m_fFogStartValue);
						m_pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
						SetLightVec(m_light.Direction);
					}
					else
						SetAmbient(m_light.Ambient.r + 0.3f, m_light.Ambient.g + 0.3f, m_light.Ambient.b + 0.3f);
				}

				pObj->Render(m_pd3dDevice);

				if (m_bViewBoundBox)
				{
					SetBoundBoxVertex(pObj);
					RenderBoundBoxVertex(pObj);
				}

				CMover* pMover = (CMover*)pObj;
				pMover->RenderPartsEffect(m_pd3dDevice);
			}

			DWORD dwAmbient;
			m_pd3dDevice->GetRenderState(D3DRS_AMBIENT, &dwAmbient);
			g_ParticleMng.Render(m_pd3dDevice);
			m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, dwAmbient);

			if (g_Option.m_nShadow == 2)
			{
				extern CModelObject g_Shadow;
				D3DXMATRIX mWorldShadow;
				g_Shadow.SetGroup(0);
				float bias = -0.0001f;
				LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
				if (g_ModelGlobal.m_bDepthBias)
					pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)(&bias)));
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
				pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

				g_Shadow.m_nNoEffect = 1;
				CMover* pMoverShadow = NULL;

				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				for (i = 0; i < m_nObjCullSize; i++)
				{
					pObj = m_aobjCull[i];
					if (pObj && pObj->GetType() == OT_MOVER)
					{
						pMoverShadow = (CMover*)pObj;
						if (!pMoverShadow->IsVisible())
							continue;
						if (pMoverShadow->IsMode(TRANSPARENT_MODE))
							continue;
						if (pMoverShadow->m_pActMover->m_fCurrentHeight < GetWaterHeight(pMoverShadow->GetPos())->byWaterHeight)
							continue;
						if (pMoverShadow->GetScrPos().y - pMoverShadow->m_pActMover->m_fCurrentHeight > 10.0f)
							continue;
						if (pMoverShadow->GetScrPos().y - pMoverShadow->m_pActMover->m_fCurrentHeight < 0)
							continue;

						////////////////////////////////////
						D3DXVECTOR3 v3Pos;
						D3DXVECTOR3 vec3Tri[3];

						D3DXMATRIX mRot;

						D3DXMatrixIdentity(&mRot);

						D3DXVECTOR3 vPos = pMoverShadow->GetScrPos();

						if (pMoverShadow->GetWorld())
						{
							pMoverShadow->GetWorld()->GetLandTri(vPos.x, vPos.z, vec3Tri);
							FLOAT ffHeight = pMoverShadow->GetWorld()->GetLandHeight(vPos.x, vPos.z);

							D3DXVECTOR3 vVector1 = vec3Tri[2] - vec3Tri[0];
							D3DXVECTOR3 vVector2 = vec3Tri[1] - vec3Tri[0];
							D3DXVECTOR3 vNormal;
							D3DXVec3Cross(&vNormal, &vVector1, &vVector2);
							D3DXVec3Normalize(&vNormal, &vNormal);

							D3DXVECTOR3 v3Up = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
							D3DXVECTOR3 v3Cross;
							FLOAT fDot;
							FLOAT fTheta;
							D3DXVec3Cross(&v3Cross, &v3Up, &vNormal);
							fDot = D3DXVec3Dot(&v3Up, &vNormal);
							fTheta = acos(fDot);

							D3DXQUATERNION qDirMap;
							D3DXQuaternionRotationAxis(&qDirMap, &v3Cross, fTheta);

							v3Pos = vPos;
							v3Pos.y = ffHeight + 0.1f;

							D3DXMatrixRotationQuaternion(&mRot, &qDirMap);
						}

						D3DXMatrixTranslation(&mWorldShadow, vPos.x, pMoverShadow->m_pActMover->m_fCurrentHeight, vPos.z);
						D3DXMatrixMultiply(&mWorldShadow, &mRot, &mWorldShadow);
						////////////////////////////////////

						g_Shadow.Render(pd3dDevice, &mWorldShadow);
					}
				}

				bias = 0;
				if (g_ModelGlobal.m_bDepthBias)
					pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)(&bias)));
			}
		}

		CHECK2("  Render Obj");
	}
	CHECK1();	// rener water
	RenderWater();
	CHECK2("  Render Water");

#ifdef __CLIENT
	g_TailEffectMng.Render(m_pd3dDevice);
	//			g_Tail.Render( m_pd3dDevice );
#endif


	D3DXMATRIX matWorld;
	D3DXMATRIX mat, matView;
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_pCamera)
	{
		LPWATERHEIGHT lpWaterHeight = GetWaterHeight((int)(m_pCamera->m_vPos.x), (int)(m_pCamera->m_vPos.z));
		if (lpWaterHeight && m_pCamera->m_vPos.y < lpWaterHeight->byWaterHeight &&
			(lpWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
		{
			D3DXMatrixLookAtLH(&mat, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			m_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
			m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			//m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x7fffffff);
			m_pd3dDevice->SetTexture(0, prj.m_terrainMng.GetTerrain(20)->m_pTexture);
			m_pd3dDevice->SetFVF(D3DFVF_D3DSFXVERTEX);
			m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			D3DSFXVERTEX pVertices[4];
			//			D3DSFXVERTEX* pVertices = new D3DSFXVERTEX[ 4 ];
			pVertices[0].p = D3DXVECTOR3(-2.0f, -2.0f, 1.0f);
			pVertices[0].tu1 = .0f;
			pVertices[0].tv1 = .0f;
			pVertices[1].p = D3DXVECTOR3(-2.0f, 2.0f, 1.0f);
			pVertices[1].tu1 = .0f;
			pVertices[1].tv1 = .0f;
			pVertices[2].p = D3DXVECTOR3(2.0f, 2.0f, 1.0f);
			pVertices[2].tu1 = .0f;
			pVertices[2].tv1 = .0f;
			pVertices[3].p = D3DXVECTOR3(2.0f, -2.0f, 1.0f);
			pVertices[3].tu1 = .0f;
			pVertices[2].tv1 = .0f;
			m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, pVertices, sizeof(D3DSFXVERTEX));
			m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
			m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			//			delete pVertices;
		}
	}
	CHECK1();
	qsort(m_asfxCull, m_nSfxCullSize, sizeof(CObj*), ObjSortFarToNear);

	for (i = 0; i < m_nSfxCullSize; i++)
		m_asfxCull[i]->m_ppViewPtr = &m_asfxCull[i];

	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);////_bViewLight );
	CObj* pObj;

	for (i = 0; i < m_nSfxCullSize; i++)
	{
		pObj = m_asfxCull[i];

		if (pObj->GetType() == OT_SFX)
		{
			CMover* pMover = prj.GetMover(((CSfx*)pObj)->m_idSrc);
			if (IsValidObj(pMover))
			{
				if (pMover->m_dwMode & GCWAR_RENDER_SKIP_MODE)
					continue;
			}
		}

		if (pObj->GetType() == OT_SFX)
		{
			bool bRender = true;
			if (g_Option.m_bSFXItemRenderOff && !((CSfx*)pObj)->GetSkill())
				bRender = false;
			else if (g_Option.m_bSFXRenderOff && ((CSfx*)pObj)->GetSkill())
				bRender = false;

			if (!bRender)
			{
				CMover* pMover = prj.GetMover(((CSfx*)pObj)->m_idSrc);
				if (IsValidObj(pMover) && pMover->IsPlayer())
					continue;
			}
		}

		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);//m_bViewLight );
		pObj->Render(m_pd3dDevice);
	}

	for (i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[i];
		if (pObj->m_wBlendFactor < 255)
		{
			if (m_bViewFog)
			{
				if (pObj->m_pModel->m_bSkin)
				{
					vConst.w = (m_fFogEndValue - pObj->m_fDistCamera) / (m_fFogEndValue - m_fFogStartValue);
					m_pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);
					{
						SetLightVec(m_light.Direction);
					}
				}
			}
			pObj->Render(m_pd3dDevice);
			if (m_bViewBoundBox)
			{
				SetBoundBoxVertex(pObj);
				RenderBoundBoxVertex(pObj);
			}
		}
	}

	for (i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[i];
		if (pObj && m_bViewName && pFont)
		{
			if (pObj->GetType() == OT_MOVER)
			{
				CMover* pMover = (CMover*)pObj;

				if (pMover->IsActiveMover() && m_nZoomLevel != 0)
					continue;

				if (pMover->IsDie() && !pMover->IsPlayer())
					continue;

				float fDistLimit = 60.0f;
				if (pMover->IsPlayer() || (pMover->IsNPC() && pMover->IsPeaceful()))
					fDistLimit = 100.0f;
				else if (pMover->m_bActiveAttack)
					fDistLimit = 80.0f;

				if (pObj->m_fDistCamera < fDistLimit)
				{
					CMover* pMover = (CMover*)pObj;
					if (pMover->IsMode(TRANSPARENT_MODE) == 0)
						pMover->RenderName(m_pd3dDevice, pFont);
				}

				pMover->RenderChrState(m_pd3dDevice);
			}
#ifdef __FL_UPDATE_GROUNDITEMS
			else if (pObj->GetType() == OT_ITEM)
			{
				const float fDistLimit = 60.0f;
				if (pObj->m_fDistCamera < fDistLimit)
				{
					DWORD dwColor = 0xff7d7d7d;
					ItemProp* pItemProp = prj.GetItemProp(pObj->m_dwIndex);
					if (pItemProp)
					{
						if (pItemProp->dwItemKind3 == IK3_GOLD || pItemProp->dwID == II_SYS_SYS_SCR_PERIN)
						{
							dwColor = 0xffcfa846;
						}
						else if (pItemProp->dwItemKind1 == IK1_WEAPON)
						{
							if (pItemProp->dwReferStat1 == WEAPON_UNIQUE)
							{
								dwColor = D3DCOLOR_XRGB(0, 130, 0);
							}
							else if (pItemProp->dwReferStat1 == WEAPON_ULTIMATE)
							{
								dwColor = D3DCOLOR_XRGB(127, 75, 255);
							}
#ifdef __NEW_ITEM_VARUNA
							else if (pItemProp->dwReferStat1 >= BARUNA_D && pItemProp->dwReferStat1 <= BARUNA_S)
							{
								dwColor = D3DCOLOR_XRGB(0, 90, 255);
							}
#endif //__NEW_ITEM_VARUNA
							else
							{
								dwColor = D3DCOLOR_XRGB(46, 155, 255);
							}
						}
						else if (pItemProp->dwItemKind1 == IK1_ARMOR)
						{
							if (pItemProp->dwReferStat1 == ARMOR_SET)
							{
								dwColor = D3DCOLOR_XRGB(0, 130, 0);
							}
#ifdef __NEW_ITEM_VARUNA
							else if (pItemProp->dwReferStat1 >= BARUNA_D && pItemProp->dwReferStat1 <= BARUNA_S)
							{
								dwColor = D3DCOLOR_XRGB(0, 90, 255);
							}
#endif //__NEW_ITEM_VARUNA
							else
							{
								dwColor = D3DCOLOR_XRGB(46, 155, 255);
							}
						}
						else if (pItemProp->dwItemKind3 == IK3_VIS)
						{
							dwColor = 0xff2fba64;
						}
						else if (pItemProp->dwID == II_CHP_RED)
						{
							dwColor = 0xfff23f63;
						}
						else if (pItemProp->dwItemKind3 == IK3_PET || pItemProp->dwItemKind3 == IK3_EGG)
						{
							dwColor = 0xff779100;
						}
						else if (pItemProp->dwItemKind3 == IK3_SOCKETCARD || pItemProp->dwItemKind3 == IK3_ELECARD)
						{
							dwColor = 0xff779100;
						}
						else if (pItemProp->dwItemKind3 == IK3_QUEST)
							dwColor = 0xff3be3d8;

						if (dwColor != NULL_ID)
							pObj->RenderName(m_pd3dDevice, pFont, dwColor);
					}
				}
			}
#endif
		}
	}
	m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_bViewFog);
	CHECK2("  Render Effect");

}
void	_DrawRect(LPDIRECT3DDEVICE9 pd3dDevice, int x, int y, int w, int h, DWORD dwColor)
{
#define D3DFVF_2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

	struct FVF_2DVERTEX
	{
		D3DXVECTOR4 vPos;	// The 3D position for the vertex
		DWORD		dwColor;		// The vertex color
	};
	FVF_2DVERTEX	m_aVertex[8];

	for (int i = 0; i < 8; i++)
	{
		m_aVertex[i].vPos = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
		m_aVertex[i].dwColor = 0xffffffff;
	}


	FVF_2DVERTEX* aVertex = m_aVertex;
	aVertex[0].vPos.x = (float)x;
	aVertex[0].vPos.y = (float)y;
	aVertex[0].vPos.z = 0.0f;
	aVertex[0].vPos.w = 1.0f;
	aVertex[0].dwColor = dwColor;

	aVertex[1].vPos.x = (float)(x + (w - 1));
	aVertex[1].vPos.y = (float)y;
	aVertex[1].vPos.z = 0.0f;
	aVertex[1].vPos.w = 1.0f;
	aVertex[1].dwColor = dwColor;

	aVertex[2].vPos.x = (float)(x + (w - 1));
	aVertex[2].vPos.y = (float)(y + (h - 1));
	aVertex[2].vPos.z = 0.0f;
	aVertex[2].vPos.w = 1.0f;
	aVertex[2].dwColor = dwColor;

	aVertex[3].vPos.x = (float)x;
	aVertex[3].vPos.y = (float)(y + (h - 1));
	aVertex[3].vPos.z = 0.0f;
	aVertex[3].vPos.w = 1.0f;
	aVertex[3].dwColor = dwColor;

	aVertex[4].vPos.x = (float)x;
	aVertex[4].vPos.y = (float)y;
	aVertex[4].vPos.z = 0.0f;
	aVertex[4].vPos.w = 1.0f;
	aVertex[4].dwColor = dwColor;

	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetVertexDeclaration(NULL);
	pd3dDevice->SetFVF(D3DFVF_2DVERTEX);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, aVertex, sizeof(FVF_2DVERTEX));
}

void RenderShadowMap(LPDIRECT3DDEVICE9 pd3dDevice, CObj** pList, int nMax)
{
	extern BOOL g_bShadow;
	if (g_bShadow == FALSE)
		return;
	int		i;
	CObj* pObj;
	static D3DXVECTOR4 vConst(1.0f, 1.0f, 1.0f, 100.0f);
	pd3dDevice->SetVertexShaderConstantF(95, (float*)&vConst, 1);


	g_pRenderToSurface->BeginScene(g_pShadowSurface, NULL);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 255, 255, 255), 0, 0);

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetTexture(1, NULL);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 192, 192, 192));
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	D3DXVECTOR3 vLightPos(0.5f, 1.0f, -0.5f);
	//D3DXVec3Normalize(&vLightPos, &vLightPos);

	D3DXVECTOR3 playerPos = g_pPlayer->GetPos();
	vLightPos += playerPos;


	D3DXMatrixLookAtLH(&g_mViewLight, &vLightPos, &playerPos, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	int clipSize = 40;
	D3DXMatrixOrthoOffCenterLH(&g_mShadowProj, -clipSize, clipSize, -clipSize, clipSize, -100.0f, 100.0f);
	//D3DXMatrixPerspectiveFovLH(&g_mShadowProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);

	pd3dDevice->SetTransform(D3DTS_VIEW, &g_mViewLight);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_mShadowProj);
	::SetTransformView(g_mViewLight);
	::SetTransformProj(g_mShadowProj);

	for (i = 0; i < nMax; i++)
	{
		pObj = *pList++;
		if (pObj)
		{
			if (g_Option.m_nShadow == 1 && pObj->GetType() != OT_MOVER)
				continue;

			if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsMode(TRANSPARENT_MODE))
				continue;

#ifdef __FL_UPDATE_GROUNDITEMS
			if (pObj->GetType() == OT_ITEM)
				continue;
#endif

			if (pObj->m_pModel && pObj->m_pModel->m_pModelElem->m_bShadow)
			{
				pObj->m_pModel->m_nNoEffect = 2;
				pObj->Render(pd3dDevice);
				pObj->m_pModel->m_nNoEffect = 0;
			}
		}
	}

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	_DrawRect(pd3dDevice, 0, 0, 2048, 2048, D3DCOLOR_ARGB(255, 255, 255, 255));

	extern BOOL s_bLight;
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, s_bLight);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pRenderToSurface->EndScene(D3DX_FILTER_POINT);

}

void CWorld::RenderBoundBoxVertex(CObj* pObj)
{
	CModel* pModel = pObj->m_pModel;
	D3DXVECTOR3 vPos = pObj->GetPos();
	D3DXVECTOR3 vScale = pObj->GetScale();

	D3DXMATRIX mat, matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&mat, vScale.x, vScale.y, vScale.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
	FLOAT fTheta = D3DXToRadian(-pObj->GetAngle());
	D3DXMatrixRotationY(&mat, fTheta);
	D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
	D3DXMatrixTranslation(&mat, vPos.x, vPos.y, vPos.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &mat);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetFVF(D3DFVF_BOUNDBOXVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pBoundBoxVertexBuffer, 0, sizeof(BOUNDBOXVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_nBoundBoxVertexNum / 2);

}
void CWorld::SetBoundBoxVertex(CObj* pObj)
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	const BOUND_BOX* pBB;
	CModel* pModel = pObj->m_pModel;
	pBB = pModel->GetBBVector();
	/*
	switch( pModel->GetModelType() )
	{
		case MODELTYPE_BILLBOARD:
			return;
		case MODELTYPE_STATIC_MESH:
			pBB = ((CModelObject*)pModel)->GetBBVector();
			break;
		case MODELTYPE_BIPED_MESH:
			pBB = ((CModelObject*)pModel)->GetBBVector();
			break;
	}
	*/
	D3DXVECTOR3 vBoundBox;
	//     - z
	//   3 | 2
	// - --+-- + x
	//   0 | 1

	//   7 | 6
	// - --+-- + x
	//   4 | 5
	BOUNDBOXVERTEX* pVertices;
	m_pBoundBoxVertexBuffer->Lock(0, m_nBoundBoxVertexNum * sizeof(BOUNDBOXVERTEX), (void**)&pVertices, 0);

	// 0
	vBoundBox = pBB->m_vPos[0];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[1];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 1
	vBoundBox = pBB->m_vPos[1];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[2];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 2
	vBoundBox = pBB->m_vPos[2];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[3];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 3
	vBoundBox = pBB->m_vPos[3];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[0];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;

	// 4
	vBoundBox = pBB->m_vPos[0];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[4];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 5
	vBoundBox = pBB->m_vPos[1];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[5];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 6
	vBoundBox = pBB->m_vPos[3];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[7];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 7
	vBoundBox = pBB->m_vPos[2];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[6];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;

	// 8
	vBoundBox = pBB->m_vPos[4];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[5];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 9
	vBoundBox = pBB->m_vPos[5];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[6];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 10
	vBoundBox = pBB->m_vPos[6];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[7];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	// 11
	vBoundBox = pBB->m_vPos[7];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;
	vBoundBox = pBB->m_vPos[4];//avBoundBox[ 0 ];
	pVertices->p = vBoundBox; pVertices->color = 0xffffffff;	pVertices++;

	m_pBoundBoxVertexBuffer->Unlock();
}


BOOL CWorld::CheckBound(D3DXVECTOR3* vPos, D3DXVECTOR3* vDest, D3DXVECTOR3* vOut, FLOAT* fLength)
{
	D3DXVECTOR3 tempVec = (*(vPos)-*(vDest));// /10.0f;
	float length = D3DXVec3Length(&tempVec);
	D3DXVec3Normalize(&tempVec, &tempVec);
	tempVec /= 10;
	D3DXVECTOR3 tempVec2 = *(vDest);
	D3DXVECTOR3 vDist1, vDist2;
	float tempheight;
	BOOL	b1, b2;
	b1 = FALSE;	b2 = FALSE;
	tempVec2 += (tempVec * 10);

	BOOL  bWaterChkDn = FALSE;
	BOOL  bWaterChkUp = FALSE;
	FLOAT fWaterHeight = 0.0f;
	LPWATERHEIGHT pWaterHeight = GetWaterHeight(*vDest);
	if (pWaterHeight &&
		(pWaterHeight->byWaterTexture & (byte)(~MASK_WATERFRAME)) == WTYPE_WATER)
	{
		fWaterHeight = (FLOAT)pWaterHeight->byWaterHeight;
		//vDest->y += 0.5f;

		if (fWaterHeight < vDest->y)
		{
			bWaterChkDn = TRUE;
		}
		else
		{
			bWaterChkUp = TRUE;
		}
	}

	for (int i = 0; i < int(length * 10); i++)
	{
		tempVec2 += tempVec;
		tempheight = GetLandHeight(tempVec2.x, tempVec2.z);
#if __VER >= 11 // __FIX_PICKING

		if (tempVec2.y < tempheight + 0.6f)
#else
		if (tempVec2.y < tempheight + 0.3f)
#endif
		{
			b1 = TRUE;
			vDist1 = tempVec2 - *vDest;
			break;
		}

		D3DXVECTOR3 vWaterVec = tempVec2;
		//vWaterVec.y+=0.5f;
		if (bWaterChkDn)
		{
			if (vWaterVec.y < fWaterHeight + 0.3f)
			{
				b1 = TRUE;
				vDist1 = vWaterVec - *vDest;
				break;
			}
		}
	}
	D3DXVECTOR3 vIntersect;
	BOOL bRet = IntersectObjLine2(&vIntersect, *vDest, *vPos, TRUE);
	if (bRet)
	{
		b2 = TRUE;

		if (vIntersect.y < vPos->y)
			vIntersect.y -= .6f;
		else
			vIntersect.y += .6f;

		vDist2 = vIntersect - *vDest;
	}
	FLOAT	fDist1, fDist2;
	if (b1 == TRUE && b2 == TRUE)
	{
		fDist1 = D3DXVec3LengthSq(&vDist1);
		fDist2 = D3DXVec3LengthSq(&vDist2);
		if (fDist1 < fDist2)
		{
			*fLength = D3DXVec3Length(&vDist1);
			*vOut = tempVec2;
		}
		else
		{
			*fLength = D3DXVec3Length(&vDist2);
			*vOut = vIntersect;
		}
		return TRUE;
	}
	else
		if (b1)
		{
			*fLength = D3DXVec3Length(&vDist1);
			*vOut = tempVec2;
			return TRUE;
		}
		else
			if (b2)
			{
				*fLength = D3DXVec3Length(&vDist2);
				*vOut = vIntersect;
				return TRUE;
			}


	*fLength = length;
	return FALSE;
}
void CWorld::SetLight(BOOL bLight)
{
	//ACE("SetLight %d \n", bLight);

#ifndef  __WORLDSERVER
	DWORD dwAmbient = D3DCOLOR_ARGB(0, 0, 0, 0);
	CLight* pLight = NULL;

	D3DXVECTOR3 vPos = (CObj::GetActiveObj() != NULL ? CObj::GetActiveObj()->GetPos() : D3DXVECTOR3(0, 0, 0));

	pLight = GetLight("direction");

#if __VER >= 15 // __BS_CHANGING_ENVIR
	ENVIR_INFO* pInfo = GetInContinent(g_pPlayer->GetPos());

	if (pInfo && m_kCurContinent._bUseEnvir)
	{
		if (pLight)
		{
			pLight->Ambient.r = pInfo->_fAmbient[0];
			pLight->Ambient.g = pInfo->_fAmbient[1];
			pLight->Ambient.b = pInfo->_fAmbient[2];

			pLight->Specular.r = 2.0f;
			pLight->Specular.g = 2.0f;
			pLight->Specular.b = 2.0f;

			pLight->Diffuse.r = pInfo->_fDiffuse[0];
			pLight->Diffuse.g = pInfo->_fDiffuse[1];
			pLight->Diffuse.b = pInfo->_fDiffuse[2];

			HookUpdateLight(pLight);

			memcpy(&m_lightFogSky, pLight, sizeof(m_lightFogSky));

			pLight->Diffuse.r *= 1.2f;
			pLight->Diffuse.g *= 1.2f;
			pLight->Diffuse.b *= 1.2f;

			pLight->Ambient.r *= 0.8f;
			pLight->Ambient.g *= 0.8f;
			pLight->Ambient.b *= 0.8f;

			memcpy(&m_light, pLight, sizeof(m_light));

			D3DXVECTOR3 vecSun = D3DXVECTOR3(0.0f, 0.5f, 0.5f);
			D3DXVec3Normalize(&(vecSun), &(vecSun));
			pLight->SetDir(-vecSun.x, -vecSun.y, -vecSun.z);
			pLight->Appear(m_pd3dDevice, TRUE);

			DWORD dwR, dwG, dwB;
			dwR = (DWORD)(pLight->Ambient.r * 255);
			dwG = (DWORD)(pLight->Ambient.g * 255);
			dwB = (DWORD)(pLight->Ambient.b * 255);
			dwAmbient = D3DCOLOR_ARGB(255, dwR, dwG, dwB);
		}
	}
	else
#endif

		if (m_bIsIndoor)
		{
			if (pLight)
			{

				pLight->Diffuse.r = ((m_dwDiffuse >> 16) & 0xff) / 255.f;
				pLight->Diffuse.g = ((m_dwDiffuse >> 8) & 0xff) / 255.f;
				pLight->Diffuse.b = ((m_dwDiffuse) & 0xff) / 255.f;


				pLight->Specular.r = 1.0f;
				pLight->Specular.g = 1.0f;
				pLight->Specular.b = 1.0f;

				pLight->Ambient.r = ((m_dwAmbient >> 16) & 0xff) / 255.f;
				pLight->Ambient.g = ((m_dwAmbient >> 8) & 0xff) / 255.f;
				pLight->Ambient.b = ((m_dwAmbient) & 0xff) / 255.f;

				HookUpdateLight(pLight);
				memcpy(&m_lightFogSky, pLight, sizeof(m_lightFogSky));

				pLight->Diffuse.r += 0.1f;
				pLight->Diffuse.g += 0.1f;
				pLight->Diffuse.b += 0.1f;

				pLight->Specular.r = 2.0f;
				pLight->Specular.g = 2.0f;
				pLight->Specular.b = 2.0f;

				pLight->Ambient.r *= 0.9f;
				pLight->Ambient.g *= 0.9f;
				pLight->Ambient.b *= 0.9f;

				memcpy(&m_light, pLight, sizeof(m_light));

				pLight->SetDir(m_v3LightDir.x, m_v3LightDir.y, m_v3LightDir.z);
				pLight->Appear(m_pd3dDevice, TRUE);

				DWORD dwR, dwG, dwB;
				dwR = (DWORD)(pLight->Ambient.r * 255);
				dwG = (DWORD)(pLight->Ambient.g * 255);
				dwB = (DWORD)(pLight->Ambient.b * 255);
				dwAmbient = D3DCOLOR_ARGB(255, dwR, dwG, dwB);
			}
		}
		else
		{
			if (pLight)
			{

				int nHour = 8, nMin = 0;
				nHour = g_GameTimer.m_nHour;
				nMin = g_GameTimer.m_nMin;
				nHour--;
				if (nHour < 0) nHour = 0;
				if (nHour > 23) nHour = 23;

				LIGHTCOLOR lightColorPrv = m_k24Light[(nHour - 1 == -1) ? 23 : nHour - 1];
				LIGHTCOLOR lightColor = m_k24Light[nHour];

				//m_lightColor = lightColorPrv;
				lightColorPrv.r1 += (lightColor.r1 - lightColorPrv.r1) * nMin / 60;
				lightColorPrv.g1 += (lightColor.g1 - lightColorPrv.g1) * nMin / 60;
				lightColorPrv.b1 += (lightColor.b1 - lightColorPrv.b1) * nMin / 60;
				lightColorPrv.r2 += (lightColor.r2 - lightColorPrv.r2) * nMin / 60;
				lightColorPrv.g2 += (lightColor.g2 - lightColorPrv.g2) * nMin / 60;
				lightColorPrv.b2 += (lightColor.b2 - lightColorPrv.b2) * nMin / 60;
				// 60(minMax) : 15(curMin) = 0.5(colorDistant) : x(curCol)


				pLight->Diffuse.r = lightColorPrv.r1;
				pLight->Diffuse.g = lightColorPrv.g1;
				pLight->Diffuse.b = lightColorPrv.b1;

				pLight->Specular.r = 1.0f;
				pLight->Specular.g = 1.0f;
				pLight->Specular.b = 1.0f;

				pLight->Ambient.r = lightColorPrv.r2;
				pLight->Ambient.g = lightColorPrv.g2;
				pLight->Ambient.b = lightColorPrv.b2;

				HookUpdateLight(pLight);
				memcpy(&m_lightFogSky, pLight, sizeof(m_lightFogSky));

				pLight->Diffuse.r *= 1.1f;
				pLight->Diffuse.g *= 1.1f;
				pLight->Diffuse.b *= 1.1f;

				pLight->Specular.r = 2.0f;
				pLight->Specular.g = 2.0f;
				pLight->Specular.b = 2.0f;

				pLight->Ambient.r *= 0.9f;
				pLight->Ambient.g *= 0.9f;
				pLight->Ambient.b *= 0.9f;

				memcpy(&m_light, pLight, sizeof(m_light));

				D3DXVECTOR3 vecSun = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
				D3DXMATRIX  matTemp;
				static const float CONS_VAL = 3.1415926f / 180.f;

				D3DXMatrixRotationX(&matTemp, (m_skyBox.m_fSunAngle + 180) * CONS_VAL);
				D3DXVec3TransformCoord(&vecSun, &vecSun, &matTemp);
				pLight->SetDir(vecSun.x, vecSun.y, vecSun.z);
				pLight->Appear(m_pd3dDevice, TRUE);

				DWORD dwR, dwG, dwB;
				dwR = (DWORD)(pLight->Ambient.r * 255);
				dwG = (DWORD)(pLight->Ambient.g * 255);
				dwB = (DWORD)(pLight->Ambient.b * 255);
				dwAmbient = D3DCOLOR_ARGB(255, dwR, dwG, dwB);
			}
		}

	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, bLight);//m_bViewLight );
	m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, dwAmbient);//D3DCOLOR_ARGB( 0,0,0,0) ); //m_dwAmbient );//D3DCOLOR_ARGB( 255,128,128,128) );//D3DCOLOR_ARGB( 255,50,50,70) );
	::SetLight(bLight);


	m_pd3dDevice->SetMaterial(&m_baseMaterial);

#endif // not WORLDSERVER
}
#ifdef __CLIENT
HRESULT CWorld::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	if (prj.m_terrainMng.GetTerrain(10)->m_pTexture == NULL)
		prj.m_terrainMng.LoadTexture(10);

	for (int i = 0; i < prj.m_terrainMng.m_nWaterFrame; i++)
	{
		for (int j = 0; j < prj.m_terrainMng.m_pWaterIndexList[i].ListCnt; j++)
			prj.m_terrainMng.LoadTexture(prj.m_terrainMng.m_pWaterIndexList[i].pList[j]);
	}


	CLight* pLight;
	pLight = new CLight;
	pLight->SetLight("direction", D3DLIGHT_DIRECTIONAL, 0, 150, 0);
	pLight->Attenuation0 = 0.0f;
	pLight->Range = 40;//FLT_MAX;//127.0f;
	pLight->Diffuse.r = 1.0f;
	pLight->Diffuse.g = 1.0f;
	pLight->Diffuse.b = 1.0f;
	pLight->Specular.r = 1.0f;
	pLight->Specular.g = 1.0f;
	pLight->Specular.b = 1.0f;
	pLight->Ambient.r = 0.0f;
	pLight->Ambient.g = 0.0f;
	pLight->Ambient.b = 0.0f;
	pLight->Direction = D3DXVECTOR3(0.0f, 0.5f, 0.5f);
	AddLight(pLight);


	ZeroMemory(&m_baseMaterial, sizeof(m_baseMaterial));
	m_baseMaterial.Diffuse.r = 1.0f;
	m_baseMaterial.Diffuse.g = 1.0f;
	m_baseMaterial.Diffuse.b = 1.0f;
	m_baseMaterial.Diffuse.a = 1.0f;
	m_baseMaterial.Ambient.r = 1.0f;
	m_baseMaterial.Ambient.g = 1.0f;
	m_baseMaterial.Ambient.b = 1.0f;
	m_baseMaterial.Ambient.a = 1.0f;
	m_baseMaterial.Specular.r = 1.0f;
	m_baseMaterial.Specular.g = 1.0f;
	m_baseMaterial.Specular.b = 1.0f;
	m_baseMaterial.Specular.a = 1.0f;
	m_baseMaterial.Power = 50.0f;

	return hr;
}
#endif

HRESULT CWorld::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_pBoundBoxVertexBuffer)
		return S_OK;
	HRESULT hr;
	SetFogEnable(pd3dDevice, m_bViewFog);
	for (int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
	{
		if (m_apLand[i])
			m_apLand[i]->RestoreDeviceObjects(pd3dDevice);
	}

	m_nBoundBoxVertexNum = 12 * 2;
	hr = m_pd3dDevice->CreateVertexBuffer(
		m_nBoundBoxVertexNum * sizeof(BOUNDBOXVERTEX),
		D3DUSAGE_WRITEONLY, D3DFVF_BOUNDBOXVERTEX,
		D3DPOOL_MANAGED, &m_pBoundBoxVertexBuffer, NULL);

	return hr;
}

HRESULT CWorld::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;
	for (int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
	{
		if (m_apLand[i])
			m_apLand[i]->InvalidateDeviceObjects();
	}
	SAFE_RELEASE(m_pBoundBoxVertexBuffer);

	return hr;
}
HRESULT CWorld::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	for (int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
	{
		if (m_apLand[i])
		{
			m_apLand[i]->DeleteDeviceObjects();
		}
	}
	if (m_apLand != NULL)
	{
		for (int i = 0; i < m_nLandWidth * m_nLandHeight; i++)
		{
			SAFE_DELETE(m_apLand[i]);
		}
		SAFE_DELETE_ARRAY(m_apLand);
	}
	return hr;
}
HRESULT CWorld::StaticInitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifdef __CLIENT
	m_skyBox.InitDeviceObjects(pd3dDevice);
#endif
	return S_OK;
}
HRESULT CWorld::StaticRestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
#ifdef __CLIENT
	m_skyBox.RestoreDeviceObjects(pd3dDevice);
#endif
	if (g_pExIB == NULL)
	{
		pd3dDevice->CreateIndexBuffer((PATCH_SIZE * PATCH_SIZE) * 6 * sizeof(WORD),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pExIB, NULL);
		WORD* pIB;
		int count = 0;
		g_pExIB->Lock(0, 0, (void**)&pIB, 0);
		for (int py = 0; py < PATCH_SIZE; py++) {
			for (int px = 0; px < PATCH_SIZE; px++) {
				if ((px + py) % 2 == 0) {
					pIB[count++] = px + py * (PATCH_SIZE + 1);
					pIB[count++] = (px + 1) + (py + 1) * (PATCH_SIZE + 1);
					pIB[count++] = px + 1 + py * (PATCH_SIZE + 1);

					pIB[count++] = px + py * (PATCH_SIZE + 1);
					pIB[count++] = px + (py + 1) * (PATCH_SIZE + 1);
					pIB[count++] = (px + 1) + (py + 1) * (PATCH_SIZE + 1);
				}
				else {
					pIB[count++] = px + py * (PATCH_SIZE + 1);
					pIB[count++] = px + (py + 1) * (PATCH_SIZE + 1);
					pIB[count++] = (px + 1) + py * (PATCH_SIZE + 1);

					pIB[count++] = px + (py + 1) * (PATCH_SIZE + 1);
					pIB[count++] = (px + 1) + (py + 1) * (PATCH_SIZE + 1);
					pIB[count++] = (px + 1) + py * (PATCH_SIZE + 1);
				}
			}
		}
	}
	if (g_pGridIB == NULL)
	{
		pd3dDevice->CreateIndexBuffer(NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE * (PATCH_SIZE + 1) * (PATCH_SIZE + 1) * 2 * 2 * sizeof(WORD),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pGridIB, NULL);
	}
	if (g_pIB == NULL)
	{
		pd3dDevice->CreateIndexBuffer((128 + 48 + 32 + 16 + 8 + 4 + 2) * 3 * sizeof(WORD),
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIB, NULL);
		WORD* pIB;
		g_pIB->Lock(0, 0, (void**)&pIB, 0);

		WORD tempTriList[] = {

			0,10,1, 1,10,2, 2,10,11, 2,11,12, 2,12,3, 3,12,4, 4,12,13, 4,13,14, 4,14,5, 5,14,6, 6,14,15, 6,15,16, 6,16,7, 7,16,8,
			10,20,11, 11,20,12, 12,20,21, 12,21,22, 12,22,13, 13,22,14, 14,22,23, 14,23,24, 14,24,15, 15,24,16,
			20,30,21, 21,30,22, 22,30,31, 22,31,32, 22,32,23, 23,32,24, 30,40,31, 31,40,32,

			8,16,17, 16,24,25, 16,25,26, 16,26,17, 24,32,33, 24,33,34, 24,34,25, 25,34,26, 26,34,35,
			32,40,41, 32,41,42, 32,42,33, 33,42,34, 34,42,43, 34,43,44, 34,44,35, 40,50,41, 41,50,42, 42,50,51, 42,51,52, 42,52,43, 43,52,44, 44,52,53,
			50,60,51, 51,60,52, 52,60,61, 52,61,62, 52,62,53, 60,70,61, 61,70,62, 62,70,71, 70,80,71,

			0,9,10, 9,18,10, 10,18,19, 10,19,20, 18,27,28, 18,28,19, 19,28,20, 20,28,29, 20,29,30,
			27,36,28, 28,36,37, 28,37,38, 28,38,29, 29,38,30, 30,38,39, 30,39,40, 36,45,46, 36,46,37, 37,46,38, 38,46,47, 38,47,48, 38,48,39, 39,48,40,
			45,54,46, 46,54,55, 46,55,56, 46,56,47, 47,56,48, 54,63,64, 54,64,55, 55,64,56, 63,72,64,

			48,49,40, 40,49,50, 48,56,57, 48,57,58, 48,58,49, 49,58,50, 50,58,59, 50,59,60,
			56,64,65, 56,65,66, 56,66,57, 57,66,58, 58,66,67, 58,67,68, 58,68,59, 59,68,60, 60,68,69, 60,69,70,
			64,72,73, 64,73,74, 64,74,65, 65,74,66, 66,74,75, 66,75,76, 66,76,67, 67,76,68, 68,76,77, 68,77,78, 68,78,69, 69,78,70, 70,78,79, 70,79,80,

			0,10,2, 2,10,20, 2,20,4, 4,20,22, 4,22,24, 4,24,6, 6,24,16, 6,16,8,
			20,30,22, 22,30,40, 22,40,32, 22,32,24,

			8,16,26, 16,24,26, 32,40,42, 32,42,24, 24,42,44, 24,44,26,
			40,50,42, 42,50,60, 42,60,44, 44,60,62, 60,70,62, 62,70,80,

			0,18,10, 10,18,20, 20,18,36, 20,36,38, 20,38,30, 30,38,40,
			36,54,56, 36,56,38, 38,56,48, 38,48,40, 54,72,64, 54,64,56,

			48,56,58, 48,58,40, 50,40,58, 50,58,60,
			64,72,74, 64,74,56, 56,74,76, 58,56,76, 58,76,60, 60,76,78, 60,78,70, 70,78,80,


			0,20,2, 2,20,4, 4,20,22, 4,22,24, 4,24,6, 6,24,8, 20,40,22, 22,40,24,

			8,24,26, 24,40,42, 24,42,44, 24,44,26, 40,60,42, 42,60,44, 44,60,62, 60,80,62,

			0,18,20, 18,36,20, 20,36,38, 20,38,40, 36,54,56, 36,56,38, 38,56,40, 54,72,56,

			40,56,58, 40,58,60, 56,72,74, 56,74,76, 56,76,58, 58,76,60, 60,76,78, 60,78,80,

			4,0,20, 4,20,40, 4,40,24, 4,24,8,

			44,8,24, 44,24,40, 44,40,60, 44,60,80,

			36,72,56, 36,56,40, 36,40,20, 36,20,0,

			76,80,60, 76,60,40, 76,40,56, 76,56,72,


			4,0,40, 4,40,8,

			44,8,40, 44,40,80,

			36,72,40, 36,40,0,

			76,80,40, 76,40,72,

			0,40,8,

			8,40,80,

			0,72,40,

			40,72,80,


			0,72,8, 8,72,80
		};
		memcpy(pIB, tempTriList, (128 + 48 + 32 + 16 + 8 + 4 + 2) * 3 * sizeof(WORD));
		g_pIB->Unlock();
	}
	return S_OK;
}
HRESULT CWorld::StaticInvalidateDeviceObjects()
{
#ifdef __CLIENT
	m_skyBox.InvalidateDeviceObjects();
#endif
	SAFE_RELEASE(g_pIB);
	SAFE_RELEASE(g_pExIB);
	SAFE_RELEASE(g_pGridIB);
	return S_OK;
}
HRESULT CWorld::StaticDeleteDeviceObjects()
{
#ifdef __CLIENT
	m_skyBox.DeleteDeviceObjects();
#endif
	return S_OK;
}

void CWorld::SetFogEnable(LPDIRECT3DDEVICE9 pd3dDevice, BOOL bEnable)
{
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, bEnable);
	if (bEnable)
	{
		DWORD dwColor = CWorld::GetDiffuseColor();
		pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, dwColor);//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
		pd3dDevice->SetRenderState(D3DRS_FOGSTART, FtoDW(m_fFogStartValue));
		pd3dDevice->SetRenderState(D3DRS_FOGEND, FtoDW(m_fFogEndValue));
		pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, FtoDW(m_fFogDensity));

		m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		m_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);//m_dwFogMode );
		m_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);//m_bRangeBasedFog );
	}
}

static CObj* s_pLastPickObj = NULL;

CObj* GetLastPickObj(void)
{
	return s_pLastPickObj;
}


BOOL CWorld::ClientPointToVector(D3DXVECTOR3* pOut, RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, D3DXVECTOR3* pVector, BOOL bObject)
{
	if (m_pCamera == NULL)
		return FALSE;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayAdd;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;
	CObj* pObj = NULL;
	D3DXVECTOR3 vPickObj = D3DXVECTOR3(0, 0, 0);


	GetPickRay(rect, point, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir);

	if (bObject)
	{
		pObj = CWorld::PickObject(rect, point, pmatProj, pmatView, 0xffffffff, CMover::GetActiveObj(), pVector, TRUE);
		if (pObj)
		{
			vPickObj = *pVector;
		}
	}


	vPickRayDir.x *= (FLOAT)m_iMPU * 2;
	vPickRayDir.z *= (FLOAT)m_iMPU * 2;
	vPickRayDir.y *= (FLOAT)m_iMPU * 2;
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;// * MPU;

	FLOAT fFarPlane = m_fFarPlane;
#ifdef __CLIENT
	fFarPlane /= 2;
#endif
	do
	{
		vPickRayCur += vPickRayAdd;
		if (D3DXVec3Length(&vPickRayAdd) <= 0.000001f) return FALSE;
		if (VecInWorld(vPickRayCur))
		{
			CLandscape* pLand = GetLandscape(vPickRayCur);
			if (pLand && pLand->isVisibile())
			{
				for (int z = -m_iMPU; z <= m_iMPU; z += m_iMPU)
				{
					for (int x = -m_iMPU; x <= m_iMPU; x += m_iMPU)
					{
						int tempx = (int)(vPickRayCur.x - x);//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
						int tempy = (int)(vPickRayCur.z - z);//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;
						v1 = D3DXVECTOR3((FLOAT)(tempx), GetLandHeight((FLOAT)tempx, (FLOAT)tempy), (FLOAT)(tempy));
						v2 = D3DXVECTOR3((FLOAT)(tempx + m_iMPU), GetLandHeight((FLOAT)tempx + m_iMPU, (FLOAT)tempy), (FLOAT)(tempy));
						v3 = D3DXVECTOR3((FLOAT)(tempx), GetLandHeight((FLOAT)tempx, (FLOAT)tempy + m_iMPU), (FLOAT)(tempy + m_iMPU));
						v4 = D3DXVECTOR3((FLOAT)(tempx + m_iMPU), GetLandHeight((FLOAT)tempx + m_iMPU, (FLOAT)tempy + m_iMPU), (FLOAT)(tempy + m_iMPU));
						if (IntersectTriangle(v1, v2, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist))
						{
							if (fDist < fNearDist)
							{
#if __VER >= 11 // __FIX_PICKING

								D3DXVECTOR3 vNormal;
								D3DXVECTOR3	vRay;
								D3DXVECTOR3	vLine1 = v1 - v3;
								D3DXVECTOR3	vLine2 = v2 - v3;
								D3DXVec3Cross(&vNormal, &vLine1, &vLine2);
								D3DXVec3Normalize(&vNormal, &vNormal);
								D3DXVec3Normalize(&vRay, &vPickRayDir);
								if (D3DXVec3Dot(&vNormal, &vRay) < 0.0f) break;
#endif
								fNearDist = fDist; *pVector = vecIntersect; bTriangle1 = TRUE;
							}
						}
						else
							if (IntersectTriangle(v2, v4, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist))
							{
								if (fDist < fNearDist)
								{
#if __VER >= 11 // __FIX_PICKING

									D3DXVECTOR3 vNormal;
									D3DXVECTOR3	vRay;
									D3DXVECTOR3	vLine1 = v2 - v3;
									D3DXVECTOR3	vLine2 = v4 - v3;
									D3DXVec3Cross(&vNormal, &vLine1, &vLine2);
									D3DXVec3Normalize(&vNormal, &vNormal);
									D3DXVec3Normalize(&vRay, &vPickRayDir);
									if (D3DXVec3Dot(&vNormal, &vRay) < 0.0f) break;
#endif
									fNearDist = fDist; *pVector = vecIntersect;	bTriangle1 = TRUE;
								}
							}
					}
				}
			}
		}
		vLength = vPickRayOrig - vPickRayCur;
	} while (/*PickRayCur.y >= 0.0f && vPickRayCur.y < 100.0f &&*/ D3DXVec3Length(&vLength) < (fFarPlane));

	if (bTriangle1 || pObj)
	{

		if (pObj)
		{
			D3DXVECTOR3 vDist = vPickObj - vPickRayOrig;
			FLOAT fDistSq1 = D3DXVec3LengthSq(&vDist);
			vDist = *pVector - vPickRayOrig;
			FLOAT fDistSq2 = D3DXVec3LengthSq(&vDist);
			if (fDistSq1 < fDistSq2)
			{
				*pVector = vPickObj;
				D3DXVECTOR3* pTri = ::GetLastPickTri();
				if (pOut)
				{
					D3DXVec3TransformCoord(&pOut[0], &pTri[0], pObj->GetMatrixWorldPtr());
					D3DXVec3TransformCoord(&pOut[1], &pTri[1], pObj->GetMatrixWorldPtr());
					D3DXVec3TransformCoord(&pOut[2], &pTri[2], pObj->GetMatrixWorldPtr());
				}
				s_pLastPickObj = pObj;
			}
			else
			{
				if (pOut)
					GetLandTri(pVector->x, pVector->z, pOut);
				s_pLastPickObj = NULL;
			}
		}
		else
		{
			if (pOut)
				GetLandTri(pVector->x, pVector->z, pOut);
			s_pLastPickObj = NULL;
		}
		return TRUE;
	}

	return FALSE;
}




BOOL CWorld::IsPickTerrain(RECT rect, POINT point, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView)
{
	if (m_pCamera == NULL)
		return FALSE;
	D3DXVECTOR3 vector;
	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayAdd;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 _v1, _v2, _v3, _v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;
	FLOAT fFarPlaneSq = m_fFarPlane * m_fFarPlane;

	GetPickRay(rect, point, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir);
	vPickRayDir.x *= (FLOAT)MPU * 2;
	vPickRayDir.z *= (FLOAT)MPU * 2;
	vPickRayDir.y *= (FLOAT)MPU * 2;
	vPickRayCur = vPickRayOrig;
	vPickRayAdd = vPickRayDir;// * MPU;
	do
	{
		vPickRayCur += vPickRayAdd;
		if (VecInWorld(vPickRayCur))
		{
			CLandscape* pLand = GetLandscape(vPickRayCur);
			if (pLand && pLand->isVisibile())
			{
				int tempx = (int)(vPickRayCur.x);//( (int)vPickRayCur.x / MPU ) * MPU;//(pLand->m_nWorldX+x*PATCH_SIZE+px)*MPU;
				int tempy = (int)(vPickRayCur.z);//( (int)vPickRayCur.z / MPU ) * MPU;//(pLand->m_nWorldY+y*PATCH_SIZE+py)*MPU;


				v1 = D3DXVECTOR3((FLOAT)(tempx), GetLandHeight((FLOAT)tempx, (FLOAT)tempy), (FLOAT)(tempy));
				v2 = D3DXVECTOR3((FLOAT)(tempx + m_iMPU), GetLandHeight((FLOAT)tempx + m_iMPU, (FLOAT)tempy), (FLOAT)(tempy));
				v3 = D3DXVECTOR3((FLOAT)(tempx), GetLandHeight((FLOAT)tempx, (FLOAT)tempy + m_iMPU), (FLOAT)(tempy + m_iMPU));
				v4 = D3DXVECTOR3((FLOAT)(tempx + m_iMPU), GetLandHeight((FLOAT)tempx + m_iMPU, (FLOAT)tempy + m_iMPU), (FLOAT)(tempy + m_iMPU));
				if (IntersectTriangle(v1, v2, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist))
				{
					if (fDist < fNearDist) { fNearDist = fDist; bTriangle1 = TRUE; }
				}
				else
					if (IntersectTriangle(v2, v4, v3, vPickRayOrig, vPickRayDir, &vecIntersect, &fDist))
					{
						if (fDist < fNearDist) { fNearDist = fDist; bTriangle1 = TRUE; }
					}
			}
		}
		vLength = vPickRayOrig - vPickRayCur;
	} while (D3DXVec3LengthSq(&vLength) < fFarPlaneSq);
	if (bTriangle1)
		return TRUE;
	return FALSE;
}


CObj* CWorld::PickObject(RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter, CObj* pExceptionObj, D3DXVECTOR3* pVector, BOOL bOnlyTopPick, BOOL bOnlyNPC)
{
	if (m_pCamera == NULL)
		return FALSE;

	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayDir2;
	D3DXVECTOR3 vPickRayAdd;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vIntersect;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;
	CObj* pSelectObj = NULL;

	GetPickRay(rectClient, ptClient, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir);
	vPickRayDir2 = vPickRayDir;
	vPickRayDir2.y = 0.0f;
	D3DXVec3Normalize(&vPickRayDir2, &vPickRayDir2);
	int nCount = 0;

	CObj* pObj;
	CObj* pNonCullObjs[10000];
	int nNonCullNum = 0;
	for (int i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[i];
		if (pObj)
		{
			if (bOnlyTopPick && pObj->GetModel()->m_pModelElem->m_bPick == FALSE)
				continue;
			if (pObj->IsCull() == FALSE)
			{
				if (pObj != pExceptionObj && (ObjTypeToObjFilter(pObj->GetType()) & dwObjectFilter))
				{
					if (bOnlyNPC && pObj->GetType() == OT_MOVER)
						if (((CMover*)pObj)->IsPlayer())	continue;

					pNonCullObjs[nNonCullNum++] = pObj;
				}
			}
		}
	}

	BOOL bPick = FALSE;
	for (int i = nNonCullNum - 1; i >= 0; i--)
	{
		CObj* pObj = (CObj*)pNonCullObjs[i];
		nCount++;
		if (pObj->GetType() == OT_MOVER && ((CMover*)pObj)->IsDie())
			bPick = pObj->m_pModel->Intersect(vPickRayOrig, vPickRayDir, pObj->GetMatrixWorld(), &vIntersect, &fDist);
		else
			bPick = pObj->Pick(&vPickRayOrig, &vPickRayDir, &vIntersect, &fDist);
		if (bPick)
		{
			if (fDist < fNearDist)
			{
				fNearDist = fDist;
				if (pVector)
					*pVector = vIntersect;
				return pObj;
			}
		}
	}
	return NULL;
}
CObj* CWorld::PickObject_Fast(RECT rectClient, POINT ptClient, D3DXMATRIX* pmatProj, D3DXMATRIX* pmatView, DWORD dwObjectFilter, CObj* pExceptionObj, BOOL bBoundBox, BOOL bOnlyNPC)
{
	if (m_pCamera == NULL)
		return NULL;

	D3DXVECTOR3 vPickRayOrig;
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayDir2;
	D3DXVECTOR3 vPickRayAdd;
	D3DXVECTOR3 vPickRayCur;
	D3DXVECTOR3 vIntersect;
	D3DXVECTOR3 vLength;
	D3DXVECTOR3 v1, v2, v3, v4;
	BOOL bTriangle1 = FALSE;
	D3DXVECTOR3 vecIntersect;
	FLOAT fDist;               // Ray-Intersection Parameter Distance
	FLOAT fNearDist = m_fFarPlane;
	CObj* pSelectObj = NULL;

	GetPickRay(rectClient, ptClient, pmatProj, pmatView, &vPickRayOrig, &vPickRayDir);
	vPickRayDir2 = vPickRayDir;
	vPickRayDir2.y = 0.0f;
	D3DXVec3Normalize(&vPickRayDir2, &vPickRayDir2);
	int nCount = 0;

	CObj* pObj;
	CObj* pNonCullObjs[10000];
	int nNonCullNum = 0;
	for (int i = 0; i < m_nObjCullSize; i++)
	{
		pObj = m_aobjCull[i];
		if (pObj)
		{
			if (pObj->IsCull() == FALSE)
			{
				if (pObj != pExceptionObj && (ObjTypeToObjFilter(pObj->GetType()) & dwObjectFilter))
				{
					if (pObj->GetType() == OT_MOVER)
					{
						if (((CMover*)pObj)->IsPlayer())
						{
							if (bOnlyNPC)
								continue;
						}
						else
						{
							if (((CMover*)pObj)->IsDie())
								continue;
							if (((CMover*)pObj)->GetId() == NULL_ID && pObj != g_pPlayer->m_pet.GetObj())
								continue;
						}
					}
					pNonCullObjs[nNonCullNum++] = pObj;
				}
			}
		}
	}

	BOOL bAABB = bBoundBox;
	for (int i = nNonCullNum - 1; i >= 0; i--)
	{
		CObj* pObj = (CObj*)pNonCullObjs[i];
		bAABB = bBoundBox;
		if (pObj->GetType() == OT_CTRL)
			bAABB = FALSE;
		if (pObj->Pick(&vPickRayOrig, &vPickRayDir, &vIntersect, &fDist, bAABB))
		{
			if (fDist < fNearDist)
			{
				fNearDist = fDist;
				return pObj;
			}
		}
	}
	return NULL;
}


void CWorld::UpdateCullInfo(D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj)
{
	D3DXMATRIX mat;

	D3DXMatrixMultiply(&mat, pMatView, pMatProj);
	D3DXMatrixInverse(&mat, NULL, &mat);

	g_cullinfo.vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); // xyz
	g_cullinfo.vecFrustum[1] = D3DXVECTOR3(1.0f, -1.0f, 0.0f); // Xyz
	g_cullinfo.vecFrustum[2] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f); // xYz
	g_cullinfo.vecFrustum[3] = D3DXVECTOR3(1.0f, 1.0f, 0.0f); // XYz
	g_cullinfo.vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f); // xyZ
	g_cullinfo.vecFrustum[5] = D3DXVECTOR3(1.0f, -1.0f, 1.0f); // XyZ
	g_cullinfo.vecFrustum[6] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f); // xYZ
	g_cullinfo.vecFrustum[7] = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // XYZ

	for (INT i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&g_cullinfo.vecFrustum[i], &g_cullinfo.vecFrustum[i], &mat);

	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[0], &g_cullinfo.vecFrustum[0],
		&g_cullinfo.vecFrustum[1], &g_cullinfo.vecFrustum[2]); // Near
	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[1], &g_cullinfo.vecFrustum[6],
		&g_cullinfo.vecFrustum[7], &g_cullinfo.vecFrustum[5]); // Far
	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[2], &g_cullinfo.vecFrustum[2],
		&g_cullinfo.vecFrustum[6], &g_cullinfo.vecFrustum[4]); // Left
	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[3], &g_cullinfo.vecFrustum[7],
		&g_cullinfo.vecFrustum[3], &g_cullinfo.vecFrustum[5]); // Right
	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[4], &g_cullinfo.vecFrustum[2],
		&g_cullinfo.vecFrustum[3], &g_cullinfo.vecFrustum[6]); // Top
	D3DXPlaneFromPoints(&g_cullinfo.planeFrustum[5], &g_cullinfo.vecFrustum[1],
		&g_cullinfo.vecFrustum[0], &g_cullinfo.vecFrustum[4]); // Bottom
}


//-----------------------------------------------------------------------------
// Name: CullObject()
// Desc: Determine the cullstate for an object bounding box (OBB).
//       The algorithm is:
//       1) If any OBB corner pt is inside the frustum, return CS_INSIDE
//       2) Else if all OBB corner pts are outside a single frustum plane,
//          return CS_OUTSIDE
//       3) Else if any frustum edge penetrates a face of the OBB, return
//          CS_INSIDE_SLOW
//       4) Else if any OBB edge penetrates a face of the frustum, return
//          CS_INSIDE_SLOW
//       5) Else if any point in the frustum is outside any plane of the
//          OBB, return CS_OUTSIDE_SLOW
//       6) Else return CS_INSIDE_SLOW
//-----------------------------------------------------------------------------
CULLSTATE CullObject(CULLINFO* pCullInfo, CObj* pObj)
{
	//pVecBounds      , pPlaneBounds
	//m_vecBoundsWorld, m_planeBoundsWorld

	D3DXVECTOR3 pVecBounds[8];
	D3DXPLANE pPlaneBounds[6];

	//convert to stack for mem auto destroy 
	for (int i = 0; i < 8; i++) {
		pVecBounds[i] = pObj->m_vecBoundsWorld[i];
	}

	for (int i = 0; i < 6; i++) {
		pPlaneBounds[i] = pObj->m_planeBoundsWorld[i];
	}

	float cullOffset = 0.f;

	//We offset the cull only for objects
	if (pObj->GetType() == OT_OBJ) {
		cullOffset = 30.f;
	}

	BYTE bOutside[8];
	ZeroMemory(&bOutside, sizeof(bOutside));

	// Check boundary vertices against all 6 frustum planes,
	// and store result (1 if outside) in a bitfield
	for (int iPoint = 0; iPoint < 8; iPoint++)
	{
		for (int iPlane = 0; iPlane < 6; iPlane++)
		{
			if (pCullInfo->planeFrustum[iPlane].a * pVecBounds[iPoint].x +
				pCullInfo->planeFrustum[iPlane].b * pVecBounds[iPoint].y +
				pCullInfo->planeFrustum[iPlane].c * pVecBounds[iPoint].z +
				pCullInfo->planeFrustum[iPlane].d + cullOffset < 0)
			{
				bOutside[iPoint] |= (1 << iPlane);
			}
	}
		// If any point is inside all 6 frustum planes, it is inside
		// the frustum, so the object must be rendered.
		if (bOutside[iPoint] == 0)
			return CS_INSIDE;
}

	// If all points are outside any single frustum plane, the object is
	// outside the frustum
	if ((bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] &
		bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7]) != 0)
	{
		return CS_OUTSIDE;
	}
#ifdef __CSC_UPDATE_WORLD3D
	return CS_INSIDE;
#else
	//    return CS_OUTSIDE;

		// Now see if any of the frustum edges penetrate any of the faces of
		// the bounding box
	D3DXVECTOR3 edge[12][2] =
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[1], // front bottom
		pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], // front top
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], // front left
		pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], // front right
		pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], // back bottom
		pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], // back top
		pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], // back left
		pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], // back right
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], // left bottom
		pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], // left top
		pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[5], // right bottom
		pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], // right top
	};
	D3DXVECTOR3 face[6][4] =
	{
		pVecBounds[0], pVecBounds[2], pVecBounds[3], pVecBounds[1], // front
		pVecBounds[4], pVecBounds[5], pVecBounds[7], pVecBounds[6], // back
		pVecBounds[0], pVecBounds[4], pVecBounds[6], pVecBounds[2], // left
		pVecBounds[1], pVecBounds[3], pVecBounds[7], pVecBounds[5], // right
		pVecBounds[2], pVecBounds[6], pVecBounds[7], pVecBounds[3], // top
		pVecBounds[0], pVecBounds[4], pVecBounds[5], pVecBounds[1], // bottom
	};
	D3DXVECTOR3* pEdge;
	D3DXVECTOR3* pFace;
	pEdge = &edge[0][0];
	for (INT iEdge = 0; iEdge < 12; iEdge++)
	{
		pFace = &face[0][0];
		for (INT iFace = 0; iFace < 6; iFace++)
		{
			if (EdgeIntersectsFace(pEdge, pFace, &pPlaneBounds[iFace]))
			{
				return CS_INSIDE_SLOW;
			}
			pFace += 4;
		}
		pEdge += 2;
	}

	// Now see if any of the bounding box edges penetrate any of the faces of
	// the frustum
	D3DXVECTOR3 edge2[12][2] =
	{
		pVecBounds[0], pVecBounds[1], // front bottom
		pVecBounds[2], pVecBounds[3], // front top
		pVecBounds[0], pVecBounds[2], // front left
		pVecBounds[1], pVecBounds[3], // front right
		pVecBounds[4], pVecBounds[5], // back bottom
		pVecBounds[6], pVecBounds[7], // back top
		pVecBounds[4], pVecBounds[6], // back left
		pVecBounds[5], pVecBounds[7], // back right
		pVecBounds[0], pVecBounds[4], // left bottom
		pVecBounds[2], pVecBounds[6], // left top
		pVecBounds[1], pVecBounds[5], // right bottom
		pVecBounds[3], pVecBounds[7], // right top
	};
	D3DXVECTOR3 face2[6][4] =
	{
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[1], // front
		pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[6], // back
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[2], // left
		pCullInfo->vecFrustum[1], pCullInfo->vecFrustum[3], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[5], // right
		pCullInfo->vecFrustum[2], pCullInfo->vecFrustum[6], pCullInfo->vecFrustum[7], pCullInfo->vecFrustum[3], // top
		pCullInfo->vecFrustum[0], pCullInfo->vecFrustum[4], pCullInfo->vecFrustum[5], pCullInfo->vecFrustum[1], // bottom
	};
	pEdge = &edge2[0][0];
	for (int iEdge = 0; iEdge < 12; iEdge++)
	{
		pFace = &face2[0][0];
		for (INT iFace = 0; iFace < 6; iFace++)
		{
			if (EdgeIntersectsFace(pEdge, pFace, &pCullInfo->planeFrustum[iFace]))
			{
				return CS_INSIDE_SLOW;
			}
			pFace += 4;
		}
		pEdge += 2;
	}

	// Now see if frustum is contained in bounding box
	// If any frustum corner point is outside any plane of the bounding box,
	// the frustum is not contained in the bounding box, so the object
	// is outside the frustum
	for (INT iPlane = 0; iPlane < 6; iPlane++)
	{
		if (pPlaneBounds[iPlane].a * pCullInfo->vecFrustum[0].x +
			pPlaneBounds[iPlane].b * pCullInfo->vecFrustum[0].y +
			pPlaneBounds[iPlane].c * pCullInfo->vecFrustum[0].z +
			pPlaneBounds[iPlane].d < 0)
		{
			return CS_OUTSIDE_SLOW;
		}
	}

	// Bounding box must contain the frustum, so render the object
	return CS_INSIDE_SLOW;
#endif //__CSC_UPDATE_WORLD3D
}

//-----------------------------------------------------------------------------
// Name: EdgeIntersectsFace()
// Desc: Determine if the edge bounded by the two vectors in pEdges intersects
//       the quadrilateral described by the four vectors in pFacePoints.
//       Note: pPlane could be derived from pFacePoints using
//       D3DXPlaneFromPoints, but it is precomputed in advance for greater
//       speed.
//-----------------------------------------------------------------------------
#ifndef __CSC_UPDATE_WORLD3D
BOOL EdgeIntersectsFace(D3DXVECTOR3* pEdges, D3DXVECTOR3* pFacePoints, D3DXPLANE* pPlane)
{
	// If both edge points are on the same side of the plane, the edge does
	// not intersect the face
	FLOAT fDist1;
	FLOAT fDist2;
	fDist1 = pPlane->a * pEdges[0].x + pPlane->b * pEdges[0].y +
		pPlane->c * pEdges[0].z + pPlane->d;
	fDist2 = pPlane->a * pEdges[1].x + pPlane->b * pEdges[1].y +
		pPlane->c * pEdges[1].z + pPlane->d;
	if (fDist1 > 0 && fDist2 > 0 ||
		fDist1 < 0 && fDist2 < 0)
	{
		return FALSE;
	}

	// Find point of intersection between edge and face plane (if they're
	// parallel, edge does not intersect face and D3DXPlaneIntersectLine
	// returns NULL)
	D3DXVECTOR3 ptIntersection;
	if (NULL == D3DXPlaneIntersectLine(&ptIntersection, pPlane, &pEdges[0], &pEdges[1]))
		return FALSE;

	// Project onto a 2D plane to make the pt-in-poly test easier
	FLOAT fAbsA = (pPlane->a > 0 ? pPlane->a : -pPlane->a);
	FLOAT fAbsB = (pPlane->b > 0 ? pPlane->b : -pPlane->b);
	FLOAT fAbsC = (pPlane->c > 0 ? pPlane->c : -pPlane->c);
	D3DXVECTOR2 facePoints[4];
	D3DXVECTOR2 point;
	if (fAbsA > fAbsB && fAbsA > fAbsC)
	{
		// Plane is mainly pointing along X axis, so use Y and Z
		for (INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].y;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.y;
		point.y = ptIntersection.z;
	}
	else if (fAbsB > fAbsA && fAbsB > fAbsC)
	{
		// Plane is mainly pointing along Y axis, so use X and Z
		for (INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].z;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.z;
	}
	else
	{
		// Plane is mainly pointing along Z axis, so use X and Y
		for (INT i = 0; i < 4; i++)
		{
			facePoints[i].x = pFacePoints[i].x;
			facePoints[i].y = pFacePoints[i].y;
		}
		point.x = ptIntersection.x;
		point.y = ptIntersection.y;
	}

	// If point is on the outside of any of the face edges, it is
	// outside the face.
	// We can do this by taking the determinant of the following matrix:
	// | x0 y0 1 |
	// | x1 y1 1 |
	// | x2 y2 1 |
	// where (x0,y0) and (x1,y1) are points on the face edge and (x2,y2)
	// is our test point.  If this value is positive, the test point is
	// "to the left" of the line.  To determine whether a point needs to
	// be "to the right" or "to the left" of the four lines to qualify as
	// inside the face, we need to see if the faces are specified in
	// clockwise or counter-clockwise order (it could be either, since the
	// edge could be penetrating from either side).  To determine this, we
	// do the same test to see if the third point is "to the right" or
	// "to the left" of the line formed by the first two points.
	// See http://forum.swarthmore.edu/dr.math/problems/scott5.31.96.html
	FLOAT x0, x1, x2, y0, y1, y2;
	x0 = facePoints[0].x;
	y0 = facePoints[0].y;
	x1 = facePoints[1].x;
	y1 = facePoints[1].y;
	x2 = facePoints[2].x;
	y2 = facePoints[2].y;
	BOOL bClockwise = FALSE;
	if (x1 * y2 - y1 * x2 - x0 * y2 + y0 * x2 + x0 * y1 - y0 * x1 < 0)
		bClockwise = TRUE;
	x2 = point.x;
	y2 = point.y;
	for (INT i = 0; i < 4; i++)
	{
		x0 = facePoints[i].x;
		y0 = facePoints[i].y;
		if (i < 3)
		{
			x1 = facePoints[i + 1].x;
			y1 = facePoints[i + 1].y;
		}
		else
		{
			x1 = facePoints[0].x;
			y1 = facePoints[0].y;
		}
		if ((x1 * y2 - y1 * x2 - x0 * y2 + y0 * x2 + x0 * y1 - y0 * x1 > 0) == bClockwise)
			return FALSE;
	}

	// If we get here, the point is inside all four face edges,
	// so it's inside the face.
	return TRUE;
}
#endif //__CSC_UPDATE_WORLD3D

void CWorld::CalculateBound()
{
	for (int i = 0; i < m_nLandHeight; i++)
	{
		for (int j = 0; j < m_nLandWidth; j++)
		{
			if (m_apLand[j + i * m_nLandWidth])
				m_apLand[j + i * m_nLandWidth]->CalculateBound();
		}
	}
}


void CWorld::RenderGrid()
{
	//if( m_bViewGrid || m_bViewGridPatch || m_bViewGridLandscape )
	{
		if (m_bViewGrid)
			RenderGrids(CRect(0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU), 1, 0xff808080);
		if (m_bViewGridPatch)
			RenderGrids(CRect(0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU), MAP_SIZE / NUM_PATCHES_PER_SIDE, 0xffffffff);
		if (m_bViewGridLandscape)
			RenderGrids(CRect(0, 0, WORLD_WIDTH * MPU, WORLD_HEIGHT * MPU), MAP_SIZE, 0xffffff00);
	}

#if __VER >= 15 // __BS_CHANGING_ENVIR
#ifdef _DEBUG
	RenderContinentLines();
#endif
#endif
}

void CWorld::RenderGrids(CRect rect, WORD dx, DWORD color)
{
	rect.NormalizeRect();
	int startx = rect.left / MPU;
	int starty = rect.top / MPU;
	int endx = rect.right / MPU;
	int endy = rect.bottom / MPU;

	//if(startx>endx) swap(startx,endx);
	//if(starty>endy) swap(starty,endy);
	if (startx < 0) startx = 0;
	if (starty < 0) starty = 0;
	if (endx > m_nLandWidth * MAP_SIZE) endx = m_nLandWidth * MAP_SIZE;
	if (endy > m_nLandHeight * MAP_SIZE) endy = m_nLandHeight * MAP_SIZE;


	int startworldx = startx / MAP_SIZE;
	int startworldy = starty / MAP_SIZE;
	int endworldx = endx / MAP_SIZE;
	int endworldy = endy / MAP_SIZE;

	if (startworldx < 0) startworldx = 0;
	if (startworldy < 0) startworldy = 0;
	if (endworldx >= m_nLandWidth) endworldx = m_nLandWidth - 1;
	if (endworldy >= m_nLandHeight) endworldy = m_nLandHeight - 1;

	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	//	m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,1);

	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//	D3DXMATRIX matWorld,matTemp;
	//	D3DXMatrixIdentity( &matWorld );
	//	matWorld._42=0.05;
	//	m_pd3dDevice->GetTransform( D3DTS_WORLD, &matTemp);
	//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TFACTOR);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetFVF(D3DFVF_D3DLANDSCAPEVERTEX);

	for (int wy = startworldy; wy < endworldy + 1; wy++)
	{
		for (int wx = startworldx; wx < endworldx + 1; wx++)
		{
			RenderWorldGrids(wx, wy, CPoint(startx, starty), CPoint(endx, endy), dx, color);
		}
	}
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//m_pd3dDevice->SetRenderState( D3DRS_ZBIAS ,0);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
}

void CWorld::RenderWorldGrids(int wx, int wy, CPoint ptLT, CPoint ptRB, WORD dx, DWORD color)
{
	CLandscape* pLand = GetLandscape(wx, wy);
	if (!pLand) return;
	if (!pLand->isVisibile()) return;
	int startx = ptLT.x - wx * MAP_SIZE;
	int starty = ptLT.y - wy * MAP_SIZE;
	int endx = ptRB.x - wx * MAP_SIZE;
	int endy = ptRB.y - wy * MAP_SIZE;

	if (startx < 0) startx = 0;
	if (starty < 0) starty = 0;
	if (endx >= MAP_SIZE) endx = MAP_SIZE - 1;
	if (endy >= MAP_SIZE) endy = MAP_SIZE - 1;

	int startpatchx = startx / PATCH_SIZE;
	int startpatchy = starty / PATCH_SIZE;
	int endpatchx = endx / PATCH_SIZE;
	int endpatchy = endy / PATCH_SIZE;

	WORD* pIB;
	int nCount = 0;
	g_pGridIB->Lock(0, 0, (void**)&pIB, 0);
	for (int py = startpatchy; py <= endpatchy; py++) {
		for (int px = startpatchx; px <= endpatchx; px++) {
			if (!pLand->m_aPatches[py][px].isVisibile()) continue;

			int indexoffset = (px + py * NUM_PATCHES_PER_SIDE) * (PATCH_SIZE + 1) * (PATCH_SIZE + 1);

			int startgridx = startx - px * PATCH_SIZE;
			int startgridy = starty - py * PATCH_SIZE;
			int endgridx = endx - px * PATCH_SIZE;
			int endgridy = endy - py * PATCH_SIZE;

			if (startgridx < 0) startgridx = 0;
			if (startgridy < 0) startgridy = 0;
			if (endgridx >= PATCH_SIZE) endgridx = PATCH_SIZE - 1;
			if (endgridy >= PATCH_SIZE) endgridy = PATCH_SIZE - 1;

			if (endgridx + px * PATCH_SIZE + wx * MAP_SIZE == ptRB.x) endgridx--;
			if (endgridy + py * PATCH_SIZE + wy * MAP_SIZE == ptRB.y) endgridy--;

			if (dx == 0) {
				for (int gy = startgridy; gy <= endgridy + 1; gy++) {
					if (gy + py * PATCH_SIZE + wy * MAP_SIZE == ptLT.y || gy + py * PATCH_SIZE + wy * MAP_SIZE == ptRB.y) {
						for (int gx = startgridx; gx < endgridx + 1; gx++) {
							pIB[nCount++] = gx + (gy * (PATCH_SIZE + 1)) + indexoffset;
							pIB[nCount++] = (gx + 1) + (gy * (PATCH_SIZE + 1)) + indexoffset;
						}
					}
				}
				for (int gx = startgridx; gx <= endgridx + 1; gx++) {
					if (gx + px * PATCH_SIZE + wx * MAP_SIZE == ptLT.x || gx + px * PATCH_SIZE + wx * MAP_SIZE == ptRB.x) {
						for (int gy = startgridy; gy < endgridy + 1; gy++) {
							pIB[nCount++] = gx + (gy * (PATCH_SIZE + 1)) + indexoffset;
							pIB[nCount++] = gx + ((gy + 1) * (PATCH_SIZE + 1)) + indexoffset;
						}
					}
				}
			}
			else {
				for (int gy = startgridy; gy <= endgridy + 1; gy++) {
					if ((gy + py * PATCH_SIZE + wy * MAP_SIZE - ptLT.y) % dx == 0) {
						for (int gx = startgridx; gx < endgridx + 1; gx++) {
							pIB[nCount++] = gx + (gy * (PATCH_SIZE + 1)) + indexoffset;
							pIB[nCount++] = (gx + 1) + (gy * (PATCH_SIZE + 1)) + indexoffset;
						}
					}
				}
				for (int gx = startgridx; gx <= endgridx + 1; gx++) {
					if ((gx + px * PATCH_SIZE + wx * MAP_SIZE - ptLT.x) % dx == 0) {
						for (int gy = startgridy; gy < endgridy + 1; gy++) {
							pIB[nCount++] = gx + (gy * (PATCH_SIZE + 1)) + indexoffset;
							pIB[nCount++] = gx + ((gy + 1) * (PATCH_SIZE + 1)) + indexoffset;
						}
					}
				}
			}
		}
	}

	g_pGridIB->Unlock();
	m_pd3dDevice->SetIndices(g_pGridIB);
	m_pd3dDevice->SetStreamSource(0, pLand->m_pVB, 0, sizeof(D3DLANDSCAPEVERTEX));

	if (nCount > 0)
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, (PATCH_SIZE + 1) * (PATCH_SIZE + 1) * (NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE), 0, nCount / 2);
}

