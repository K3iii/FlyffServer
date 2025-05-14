#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <stdio.h>
#include "xUtil3d.h"
#include "defineObj.h"
#include "defineNeuz.h"
#include "Material.h"
#include "ModelGlobal.h"
#include "ModelObject.h"




#ifdef	_DEBUG
extern LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
extern LPDIRECT3DVERTEXSHADER9	g_pSkiningVS;
#endif

BOOL	g_bUsableVS = TRUE;
DWORD	g_dwUsablePS_Level = 0;

extern float g_fHairLight;
extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;
extern D3DXVECTOR4 s_vLightPos;
extern D3DXMATRIX s_mViewProj;
extern D3DXMATRIX s_mView;
extern D3DXMATRIX s_mProj;

extern BOOL s_bLight;


char   g_szLastFile[512];

///////////////////////////////////////////////////////////////////////////////
//////
//////
//////
//////
//////
///////////////////////////////////////////////////////////////////////////////
CModelObject::CModelObject()
{
	Init();
}

CModelObject::~CModelObject()
{
	Destroy();
}

void CModelObject::Init(void)
{
	int		i;

	for (i = 0; i < MAX_ELEMENT; i++)
	{
		Init(i);
	}

	m_pBone = NULL;
	m_pForce = m_pForce2 = NULL;
	m_pMotion = m_pMotionOld = NULL;
	m_mUpdateBone = NULL;
	m_nModelType = MODELTYPE_NONE;
	m_bMotionBlending = FALSE;
	m_fBlendWeight = 0;
	m_bCollObj = 0;
	m_pBaseBoneInv = NULL;

	m_nFrameOld = 0;

	m_vMin.x = m_vMin.y = m_vMin.z = 65535.0f;
	m_vMax.x = m_vMax.y = m_vMax.z = -65535.0f;
	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0.0f;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0.0f;

	m_vForce3.x = m_vForce3.y = m_vForce3.z = 0.0f;
	m_vForce4.x = m_vForce4.y = m_vForce4.z = 0.0f;


#ifdef __CLIENT
	m_pm_pPartsEffectRegular[0] = m_pm_pPartsEffectRegular[1] = NULL;
	m_pm_pPartsEffectElement[0] = m_pm_pPartsEffectElement[1] = NULL;
#endif // __CLIENT
}

void CModelObject::Init(int nIdx)
{
	if (nIdx < 0)	return;
	memset(&m_Element[nIdx], 0, sizeof(m_Element[nIdx]));
	m_Element[nIdx].m_nParentIdx = -1;
	m_Element[nIdx].m_nPartsIdx = -1;
	D3DXMatrixIdentity(&m_Element[nIdx].m_mLocalRH);
	D3DXMatrixIdentity(&m_Element[nIdx].m_mLocalLH);
	D3DXMatrixIdentity(&m_Element[nIdx].m_mLocalShield);
	D3DXMatrixIdentity(&m_Element[nIdx].m_mLocalKnuckle);
	D3DXMatrixIdentity(&m_Element[nIdx].m_mLocalTM);

	m_nFrameOld = 0;
}


void CModelObject::Destroy(void)
{
	int		i;
	for (i = 0; i < MAX_ELEMENT; i++)
		Destroy(i);

	SAFE_DELETE_ARRAY(m_mUpdateBone);

	SAFE_DELETE(m_pForce);
	SAFE_DELETE(m_pForce2);

#ifdef __CLIENT
	SAFE_DELETE(m_pm_pPartsEffectRegular[0]);
	SAFE_DELETE(m_pm_pPartsEffectRegular[1]);
	SAFE_DELETE(m_pm_pPartsEffectElement[0]);
	SAFE_DELETE(m_pm_pPartsEffectElement[1]);
#endif // __CLIENT

	Init();
}

#if __VER >= 14 // __WING_ITEM
CString CModelObject::GetMotionFileName(const CString& rstrMotion) const
{
	static TCHAR s_szRoot[MAX_OBJTYPE][32] = { "obj", "ani", "ctrl", "sfx", "item", "mvr", "region", "obj", "path" };

	CString strBoneFileName;
	if (m_pBone == NULL || m_pModelElem == NULL)
		return strBoneFileName;

	strBoneFileName.Format(_T("%s_%s_%s.ani"), s_szRoot[m_pModelElem->m_dwType], m_pModelElem->m_szName, rstrMotion);
	return strBoneFileName;
}
#endif // __WING_ITEM


// BOOL bNeedCollObject = TRUE
BOOL CModelObject::TestIntersectionOBB_Line(const Segment3& segment, const CObj* pObj, BOOL bNeedCollObject)
{
	CObject3D* pObject3D = m_Element[0].m_pObject3D;
	if (pObject3D->m_CollObject.m_Type == GMT_ERROR && bNeedCollObject)
	{
		return FALSE;
	}


	char* pString = ((CModelObject*)pObj->m_pModel)->GetObject3D()->m_szFileName;
	if (strcmp("obj_guildcombatground.o3d", pString) == 0)
		return TRUE;


#ifdef __CPU_UTILDOWN_060502	//sun!!
	return IntrSegment3Box3_Test(segment, pObj->m_OBB);
#else
	const D3DXMATRIX& mWorld = pObj->GetMatrixWorld();

	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 vStart, vEnd;
	vStart = segment.Origin - segment.Extent * segment.Direction;
	vEnd = segment.Origin + segment.Extent * segment.Direction;
	return ::IsTouchOBB_Line(m_vMin, m_vMax, mWorld, vStart, vEnd, &vTemp);
#endif
}


BOOL CModelObject::Destroy(int nIdx)
{
	if (nIdx < 0)	return FALSE;
	O3D_ELEMENT* pElem = &m_Element[nIdx];
	int		i;

	if (pElem->m_pObject3D)
	{
		if (pElem->m_ppd3d_VB)
		{
			pElem->m_pObject3D->SetLOD(0);
			for (i = 0; i < pElem->m_pObject3D->GetMaxObject(); i++)
				SAFE_RELEASE(pElem->m_ppd3d_VB1[i]);
			pElem->m_pObject3D->SetLOD(1);
			for (i = 0; i < pElem->m_pObject3D->GetMaxObject(); i++)
				SAFE_RELEASE(pElem->m_ppd3d_VB2[i]);
			pElem->m_pObject3D->SetLOD(2);
			for (i = 0; i < pElem->m_pObject3D->GetMaxObject(); i++)
				SAFE_RELEASE(pElem->m_ppd3d_VB3[i]);
			pElem->m_pObject3D->SetLOD(0);

			SAFE_DELETE_ARRAY(pElem->m_ppd3d_VB);
		}
#ifdef __MODEL_CACHE
		g_Object3DMng.DeleteObject3D(pElem->m_pObject3D, GetType());
#else
		g_Object3DMng.DeleteObject3D(pElem->m_pObject3D);
#endif

		Init(nIdx);
		return TRUE;
	}
	Init(nIdx);
	return FALSE;
}


void	CModelObject::CreateForce(int nParts)
{
#ifdef __CLIENT
	if (nParts == PARTS_RWEAPON)
	{
		if (m_pForce)	return;
		m_pForce = new CSwordForce;
	}
	else
		if (nParts == PARTS_LWEAPON)
		{
			if (m_pForce2)	return;
			m_pForce2 = new CSwordForce;
		}
		else
		{
			ASSERT(0);
		}
#endif
}


void	CModelObject::TakeOffParts(int nParts)
{
	if (nParts < 0)
		Error("CModelObject::TakeOffParts : nParts = %d", nParts);



	O3D_ELEMENT* pElem;
	int nIdx = GetPartsIdx(nParts);
	pElem = GetParts(nParts);
	if (pElem && pElem->m_ppd3d_VB)
	{
		pElem->m_pObject3D->SetLOD(0);
		for (int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
			SAFE_RELEASE(pElem->m_ppd3d_VB1[j]);
		pElem->m_pObject3D->SetLOD(1);
		for (int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
			SAFE_RELEASE(pElem->m_ppd3d_VB2[j]);
		pElem->m_pObject3D->SetLOD(2);
		for (int j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
			SAFE_RELEASE(pElem->m_ppd3d_VB3[j]);
		pElem->m_pObject3D->SetLOD(0);
		SAFE_DELETE_ARRAY(pElem->m_ppd3d_VB);
	}

	if (nIdx != -1)
	{
		if (nParts == PARTS_LWEAPON || nParts == PARTS_RWEAPON)
		{
#ifdef __CLIENT
			SAFE_DELETE(m_pm_pPartsEffectRegular[0]);
			SAFE_DELETE(m_pm_pPartsEffectRegular[1]);
			SAFE_DELETE(m_pm_pPartsEffectElement[0]);
			SAFE_DELETE(m_pm_pPartsEffectElement[1]);
#endif // __CLIENT
		}

		Init(nIdx);
	}
}


//		LoadBone

int		CModelObject::LoadBone(LPCTSTR szFileName)
{
	if (m_pBone)
		Error("CModelObject::LoadBone ; 이미 본이 로딩되어 있다. 0x%08x", (int)m_pBone);


	m_pBone = g_BonesMng.LoadBone(szFileName);


	if (m_pBone == NULL)
	{
		LPCTSTR szStr = Error("%s : 찾을 수 없음", szFileName);
		//ADDERRORMSG( szStr );
		return FAIL;
	}

	m_bSkin = TRUE;
#ifdef _DEBUG
	if (m_mUpdateBone)
		Error("CModelObject::LoadBone : %s 이미 읽었는데 또 읽은것 같다.", szFileName);
#endif


	m_mUpdateBone = new D3DXMATRIX[m_pBone->m_nMaxBone * 2];
	m_pBaseBoneInv = m_mUpdateBone + m_pBone->m_nMaxBone;

	int		i;
	for (i = 0; i < m_pBone->m_nMaxBone; i++)
	{
		m_mUpdateBone[i] = m_pBone->m_pBones[i].m_mTM;
		m_pBaseBoneInv[i] = m_pBone->m_pBones[i].m_mInverseTM;
	}
	return SUCCESS;
}



//	LoadMotion


int		CModelObject::LoadMotion(LPCTSTR szFileName)
{
	int		i;

	if (m_pBone == NULL)
	{
		Error("%s : 본을 로딩하지 않고 모션을 읽음", szFileName);
		return FAIL;
	}
	m_pMotionOld = m_pMotion;
	m_nFrameOld = (int)m_fFrameCurrent;

	m_pMotion = g_MotionMng.LoadMotion(szFileName);
	if (m_pMotion == NULL)
	{
		return FAIL;
	}
	if (m_pMotionOld == NULL)
		m_pMotionOld = m_pMotion;

	InitAnimate();

	m_nFrameMax = m_pMotion->GetMaxFrame();
	m_fPerSlerp = (float)((int)(m_pMotion->m_fPerSlerp * 1000.0f)) / 1000.0f;

	if (m_pMotionOld->m_nMaxBone != m_pMotion->m_nMaxBone)
	{
		LPCTSTR szErr = Error("CModelObject::LoadElement - %s bone count(%d) - old count(%d)", szFileName, m_pMotion->m_nMaxBone, m_pMotionOld->m_nMaxBone);
		//ADDERRORMSG( szErr );
	}


	for (i = 0; i < MAX_ELEMENT; i++)
	{
		if (m_Element[i].m_pObject3D)
			m_Element[i].m_pObject3D->SetExternBone(m_mUpdateBone, m_pBaseBoneInv);
	}
	return SUCCESS;
}



int	CModelObject::MovePart(int nDstPart, int nSrcPart)
{
	O3D_ELEMENT* pDestPart = GetParts(nDstPart);
	O3D_ELEMENT* pSrcPart = GetParts(nSrcPart);

	if (!pDestPart && pSrcPart)
	{
		LoadElement(pSrcPart->m_pObject3D->m_szFileName, nDstPart);
		memset(pSrcPart, 0, sizeof(*pSrcPart));
		return 1;
	}

	if (!pDestPart || !pSrcPart)
		return 0;

	if (pDestPart->m_pObject3D)
		Error("CModelObject::MovePart : m_Element[%d].m_pObject3D == 0x%08x", nDstPart, (int)pDestPart->m_pObject3D);

	memcpy(pDestPart, pSrcPart, sizeof(O3D_ELEMENT));	// 카피하고
	memset(pSrcPart, 0, sizeof(*pSrcPart));		// 소스측은 클리어.	
	return 1;
}

#ifdef __MODEL_CACHE
int		CModelObject::LoadElement(LPCTSTR szFileName, int nParts, int nType)
#else
int		CModelObject::LoadElement(LPCTSTR szFileName, int nParts)
#endif
{
	CObject3D* pObject3D;
	O3D_ELEMENT* pElem;

	if (nParts < 0)
	{
		Error("CModelObject::LoadElement : nParts 에러 - nParts = %d, %s", nParts, szFileName);
		return FAIL;
	}

	if (nParts == PARTS_RIDE)
		return FAIL;

	if (nParts >= PARTS_LOWER2 && nParts <= PARTS_BULLET)
		return FAIL;

	int nIdx = GetPartsIdx(nParts);
	Destroy(nIdx);

#ifdef __CLIENT
	HRESULT hr;

	if (FAILED(hr = m_pd3dDevice->TestCooperativeLevel()))
	{
		LPCTSTR szErr = Error("%s 디바이스실패 %08x", szFileName, (int)hr);
		//ADDERRORMSG( szErr );
	}
#endif
#ifdef __MODEL_CACHE
	pObject3D = g_Object3DMng.LoadObject3D(m_pd3dDevice, szFileName, nType);
#else
	pObject3D = g_Object3DMng.LoadObject3D(m_pd3dDevice, szFileName);
#endif
	if (pObject3D == NULL)
	{
		LPCTSTR szStr = Error("%s : 찾을 수 없음", MakePath(DIR_MODEL, szFileName));
		//ADDERRORMSG( szStr );
		return FAIL;
	}

	pObject3D->InitDeviceObjects(m_pd3dDevice);


	if (pObject3D->IsUseExternBone())
	{
		if (m_pBone == NULL)
		{
#ifdef __CLIENT
			Error("%s : bone not loaded.", szFileName);
#endif // __CLIENT
			return FAIL;
		}
	}

	int i = NULL;
	for (; i < MAX_ELEMENT; i++)
	{
		if (m_Element[i].m_nPartsIdx == -1)
			break;
	}

	if (i == MAX_ELEMENT)
		return FAIL;

	pElem = &m_Element[i];
	memset(pElem, 0, sizeof(*pElem));
	pElem->m_nPartsIdx = nParts;
	pElem->m_nParentIdx = -1;
	D3DXMatrixIdentity(&(pElem->m_mLocalRH));
	D3DXMatrixIdentity(&(pElem->m_mLocalLH));
	D3DXMatrixIdentity(&(pElem->m_mLocalShield));
	D3DXMatrixIdentity(&(pElem->m_mLocalKnuckle));


	pElem->m_pObject3D = pObject3D;

	memset(g_szLastFile, 0, sizeof(g_szLastFile));
	strcpy(g_szLastFile, szFileName);

	if (pObject3D->GetGMOBJECT() == NULL)
	{
		LPCTSTR szStr = Error("CModelObject::LoadElement에서 튕김 : 읽은 파일명 %s", MakePath(DIR_MODEL, szFileName));
		//ADDERRORMSG( szStr );
		return FAIL;
	}

	if (pObject3D->GetGMOBJECT()->m_bOpacity)
		m_dwRenderEffect = REFF_ALPHA;
	m_bCollObj = pObject3D->IsHaveCollObj();
	if (pObject3D->m_nHavePhysique)	m_bSkin = TRUE;

	if (m_nFrameMax > 0)
	{
		if (pObject3D->GetMaxFrame())
			if (m_nFrameMax != pObject3D->GetMaxFrame())
			{
				LPCTSTR szErr = Error("CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax);
				//ADDERRORMSG( szErr );
			}
	}
	if (m_nFrameMax == 0)
		m_nFrameMax = pObject3D->GetMaxFrame();


	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;



	if (m_pBone)
		pObject3D->SetExternBone(m_mUpdateBone, m_pBaseBoneInv);




	if (pObject3D->m_vBBMin.x < m_vMin.x)	m_vMin.x = pObject3D->m_vBBMin.x;
	if (pObject3D->m_vBBMin.y < m_vMin.y)	m_vMin.y = pObject3D->m_vBBMin.y;
	if (pObject3D->m_vBBMin.z < m_vMin.z)	m_vMin.z = pObject3D->m_vBBMin.z;
	if (pObject3D->m_vBBMax.x > m_vMax.x)	m_vMax.x = pObject3D->m_vBBMax.x;
	if (pObject3D->m_vBBMax.y > m_vMax.y)	m_vMax.y = pObject3D->m_vBBMax.y;
	if (pObject3D->m_vBBMax.z > m_vMax.z)	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB(m_BB.m_vPos, m_vMin, m_vMax);



	pElem->m_nParentIdx = -1;

	if (pObject3D->m_nHavePhysique)
	{
		if (pElem->m_ppd3d_VB)
			Error("CModelObject::LoadElement: %s pElem->m_ppd3d_VB is not NULL", szFileName);

		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);

		if (nTotal <= 0)
			Error("%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal);

		pElem->m_ppd3d_VB = new LPDIRECT3DVERTEXBUFFER9[nTotal];
		memset(pElem->m_ppd3d_VB, 0, sizeof(LPDIRECT3DVERTEXBUFFER9) * nTotal);
		LPDIRECT3DVERTEXBUFFER9* ppd3d_VB = pElem->m_ppd3d_VB;
		pObject3D->SetLOD(0);		pElem->m_ppd3d_VB1 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_ppd3d_VB2 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_ppd3d_VB3 = ppd3d_VB;
		pObject3D->SetLOD(0);
		pElem->m_ppd3d_VBSel = pElem->m_ppd3d_VB1;
	}

	pObject3D->RestoreDeviceObjects(pElem->m_ppd3d_VB);

	return SUCCESS;
}

#if __VER >= 15 // __GUILD_HOUSE
#ifdef __CLIENT

int CModelObject::LoadClonedElement(LPCTSTR szFileName)
{

	CObject3D* pObject3D = NULL;
	O3D_ELEMENT* pElem = NULL;

	pObject3D = new CObject3D;
	pObject3D->InitDeviceObjects(m_pd3dDevice);
	if (pObject3D->LoadObject(szFileName) == FAIL)
	{
		assert(0);
		SAFE_DELETE(pObject3D);
		return -1;
	}

	pObject3D->InitDeviceObjects(m_pd3dDevice);

	pElem = &m_Element[0];
	memset(pElem, 0, sizeof(*pElem));
	pElem->m_nPartsIdx = 0;
	pElem->m_nParentIdx = -1;
	D3DXMatrixIdentity(&(pElem->m_mLocalRH));
	D3DXMatrixIdentity(&(pElem->m_mLocalLH));
	D3DXMatrixIdentity(&(pElem->m_mLocalShield));
	D3DXMatrixIdentity(&(pElem->m_mLocalKnuckle));

	pElem->m_pObject3D = pObject3D;

	m_bCollObj = pObject3D->IsHaveCollObj();

	memset(g_szLastFile, 0, sizeof(g_szLastFile));
	strcpy(g_szLastFile, szFileName);
	if (pObject3D->m_nHavePhysique)
		m_bSkin = TRUE;

	if (m_nFrameMax == 0)
		m_nFrameMax = pObject3D->GetMaxFrame();

	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;

	if (m_nFrameMax > 0)
	{
		if (pObject3D->GetMaxFrame())
			if (m_nFrameMax != pObject3D->GetMaxFrame())
			{
				LPCTSTR szErr = Error("CModelObject::LoadElement - %s의 MaxFrame(%d)과 이전에 읽은 MaxFrame(%d)이 다르다.", szFileName, pObject3D->GetMaxFrame(), m_nFrameMax);
				//ADDERRORMSG( szErr );
			}
	}
	if (m_nFrameMax == 0)
		m_nFrameMax = pObject3D->GetMaxFrame();


	m_fPerSlerp = (float)((int)(pObject3D->m_fPerSlerp * 1000.0f)) / 1000.0f;



	if (m_pBone)
		pObject3D->SetExternBone(m_mUpdateBone, m_pBaseBoneInv);




	if (pObject3D->m_vBBMin.x < m_vMin.x)	m_vMin.x = pObject3D->m_vBBMin.x;
	if (pObject3D->m_vBBMin.y < m_vMin.y)	m_vMin.y = pObject3D->m_vBBMin.y;
	if (pObject3D->m_vBBMin.z < m_vMin.z)	m_vMin.z = pObject3D->m_vBBMin.z;
	if (pObject3D->m_vBBMax.x > m_vMax.x)	m_vMax.x = pObject3D->m_vBBMax.x;
	if (pObject3D->m_vBBMax.y > m_vMax.y)	m_vMax.y = pObject3D->m_vBBMax.y;
	if (pObject3D->m_vBBMax.z > m_vMax.z)	m_vMax.z = pObject3D->m_vBBMax.z;
	SetBB(m_BB.m_vPos, m_vMin, m_vMax);



	pElem->m_nParentIdx = -1;

	if (pObject3D->m_nHavePhysique)
	{
		if (pElem->m_ppd3d_VB)
			Error("CModelObject::LoadElement: %s pElem->m_ppd3d_VB is not NULL", szFileName);

		int nTotal = 0;
		pObject3D->SetLOD(0);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);	nTotal += pObject3D->GetMaxObject();
		pObject3D->SetLOD(0);

		if (nTotal <= 0)
			Error("%s LoadElement : nTotal=%d", pObject3D->m_szFileName, nTotal);

		pElem->m_ppd3d_VB = new LPDIRECT3DVERTEXBUFFER9[nTotal];
		memset(pElem->m_ppd3d_VB, 0, sizeof(LPDIRECT3DVERTEXBUFFER9) * nTotal);
		LPDIRECT3DVERTEXBUFFER9* ppd3d_VB = pElem->m_ppd3d_VB;
		pObject3D->SetLOD(0);		pElem->m_ppd3d_VB1 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(1);		pElem->m_ppd3d_VB2 = ppd3d_VB;		ppd3d_VB += pObject3D->GetMaxObject();
		pObject3D->SetLOD(2);		pElem->m_ppd3d_VB3 = ppd3d_VB;
		pObject3D->SetLOD(0);
		pElem->m_ppd3d_VBSel = pElem->m_ppd3d_VB1;
	}

	pObject3D->RestoreDeviceObjects(pElem->m_ppd3d_VB);

	return 1;

}
#endif //__CLIENT

#endif //__GUILD_HOUSE

// SetParent( PARTS_LWEAPON, BONE_RHAND );


void	CModelObject::SetParent(int nParts, int nBoneIdx)
{
	O3D_ELEMENT* pElem = GetParts(nParts);
	if (!pElem)
		return;


	pElem->m_nParentIdx = nBoneIdx;
	pElem->m_mLocalTM = pElem->m_pObject3D->GetGMOBJECT()->m_mLocalTM;



	if (nBoneIdx == GetRHandIdx())
		pElem->m_mLocalRH = m_pBone->m_mLocalRH;

	if (nBoneIdx == GetLHandIdx())
		pElem->m_mLocalLH = m_pBone->m_mLocalLH;

	if (nBoneIdx == GetLArmIdx())
		pElem->m_mLocalShield = m_pBone->m_mLocalShield;

	if (nBoneIdx == GetRArmIdx())
		pElem->m_mLocalKnuckle = m_pBone->m_mLocalKnuckle;
}


void	CModelObject::ClearTextureEx(int nParts)
{
	O3D_ELEMENT* pParts = GetParts(nParts);

	if (pParts)
		pParts->m_pTextureEx = NULL;
}


void	CModelObject::ChangeTexture(int nParts, LPCTSTR szSrc, LPCTSTR szDest)
{
#ifdef __WORLDSERVER
	return;
#endif

	O3D_ELEMENT* pParts = GetParts(nParts);
	if (pParts)
		pParts->m_pObject3D->ChangeTexture(szSrc, szDest);
}

#ifndef __WORLDSERVER
//---------------------------------------------------------------------------------------------





FLOAT   g_fDiffuse[4];
FLOAT   g_fAmbient[4];

int		CModelObject::Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* mWorld)
{
#ifdef	__WORLDSERVER
	return 1;
#else
	CObject3D* pObject3D;
	O3D_ELEMENT* pElem;
	int		i;
	int		nNextFrame;
	D3DXMATRIX	m1;

#ifndef __CLIENT
	if (IsEmptyElement() == TRUE)
		return 1;
#endif

#ifdef	_DEBUG
	if (m_mUpdateBone && g_pSkiningVS == NULL)
		Error("CMyD3DApplication::RestoreDeviceObjects()에 CreateSkiningVS()를 호출하시오.\r\nCMyD3DApplication::InvalidateDeviceObjects()에는 DeleteSkiningVS()를 호출하시오.\r\n혹은 *.vsh파일을 읽지 못했다.");
	if (m_nLoop == 0)
		Error("경고 : %s : CModelObject::m_nLoop가 지정되지 않았다.", m_pMotion->m_szName);
#endif

	if (m_pMotion)
		m_pMotion->AnimateBone(m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight);

	if (m_pBone)
	{
		D3DXMATRIX* pmBones;
		D3DXMATRIX mWorldTranspose;
		D3DXMATRIX* pmBonesInv = m_pBaseBoneInv;
		BONE* pBoneList = m_pBone->m_pBones;
		pmBones = m_mUpdateBone;

		if (m_pBone->m_bSendVS)
		{
			int		nMaxBone = m_pBone->m_nMaxBone;

			if (nMaxBone > MAX_VS_BONE)
				Error("CModelObject::Render : overflow bone count - %d", nMaxBone);

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
		D3DXVECTOR4 vLightPos = s_vLightPos;

		mViewProj = *mWorld * s_mView * s_mProj;

		D3DXMatrixTranspose(&mViewProjTranspose, &mViewProj);
		D3DXMatrixTranspose(&mWorldTranspose, mWorld);

		D3DXMatrixInverse(&mInvWorld, NULL, mWorld);
		D3DXVec4Transform(&vLight, &vLight, &mInvWorld);
		D3DXVec4Normalize(&vLight, &vLight);
		D3DXVec4Transform(&vLightPos, &vLightPos, &mInvWorld);

		m_pd3dDevice->SetVertexShaderConstantF(84, (float*)&mViewProjTranspose, 4);
		m_pd3dDevice->SetVertexShaderConstantF(91, (float*)&vLightPos, 1);
		m_pd3dDevice->SetVertexShaderConstantF(92, (float*)&vLight, 1);
		m_pd3dDevice->SetVertexShaderConstantF(93, (float*)&s_fDiffuse, 1);
		m_pd3dDevice->SetVertexShaderConstantF(94, (float*)&s_fAmbient, 1);
	}

	if (m_nNoEffect == 0)
	{

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	}
	DWORD dwBlendFactor = m_dwColor | (m_dwBlendFactor << 24);

	if (m_dwBlendFactor < 255 || m_dwColor)
	{
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwBlendFactor);
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	}

	nNextFrame = GetNextFrame();
	pd3dDevice->SetMaterial(g_TextureMng.GetMaterial(pd3dDevice, 0));

	D3DXVECTOR3 vec3LightBackup = D3DXVECTOR3(s_vLight[0], s_vLight[1], s_vLight[2]);
	D3DXVECTOR4 vec4Diffuse = D3DXVECTOR4(s_fDiffuse[0], s_fDiffuse[1], s_fDiffuse[2], s_fDiffuse[3]);;


	for (i = 0; i < MAX_ELEMENT; i++)
	{
		pElem = &m_Element[i];

		pObject3D = pElem->m_pObject3D;
		if (pObject3D == NULL)
			continue;

		if (pElem->m_nEffect & XE_HIDE)
			continue;

		pObject3D->m_nNoTexture = 0; // m_nNoTexture;
		pObject3D->m_nNoEffect = m_nNoEffect;

		if (pElem->m_nParentIdx != -1)
		{

			m1 = m_mUpdateBone[pElem->m_nParentIdx] * *mWorld;
			if (pElem->m_nParentIdx == GetRHandIdx())
				m1 = pElem->m_mLocalRH * m1;
			else if (pElem->m_nParentIdx == GetLHandIdx())
				m1 = pElem->m_mLocalLH * m1;
			else if (pElem->m_nParentIdx == GetLArmIdx())
				m1 = pElem->m_mLocalShield * m1;
			else if (pElem->m_nParentIdx == GetRArmIdx())
				m1 = pElem->m_mLocalKnuckle * m1;
		}
		else
			m1 = *mWorld;

		if (pElem->m_nPartsIdx == PARTS_HAIR)
		{
			memcpy(g_fDiffuse, s_fDiffuse, sizeof(FLOAT) * 4);
			memcpy(g_fAmbient, s_fAmbient, sizeof(FLOAT) * 4);

			SetDiffuse(0, 0, 0);

#ifdef __CLIENT
			if (s_bLight)
			{
				float fblack = (CWorld::m_light.Diffuse.r * g_fHairLight) + (CWorld::m_light.Diffuse.g * g_fHairLight) + (CWorld::m_light.Diffuse.b * g_fHairLight);
				fblack = fblack / 3.0f;
				fblack *= 0.6f;

#if __VER >= 14
				if (fblack < 0.3f)
					fblack = 0.3f;
#endif // __BS_FIX_HAIR_AMBIENT
				SetAmbient(min(pObject3D->m_fAmbient[0] * fblack, 1.0f), min(pObject3D->m_fAmbient[1] * fblack, 1.0f), min(pObject3D->m_fAmbient[2] * fblack, 1.0f));
			}
			else
#endif // CLIENT
				SetAmbient(pObject3D->m_fAmbient[0], pObject3D->m_fAmbient[1], pObject3D->m_fAmbient[2]);

			pd3dDevice->SetVertexShaderConstantF(93, (float*)&s_fDiffuse, 1);
			pd3dDevice->SetVertexShaderConstantF(94, (float*)&s_fAmbient, 1);
		}

		if (m_pBone)
			pObject3D->SetExternBone(m_mUpdateBone, m_pBaseBoneInv);

		if (m_SparkInfo.m_bUsed)
		{
			SetLightVec(m_SparkInfo.m_v3SparkDir);

			D3DXVECTOR3 vec3Diffuse;
			D3DXVec3Lerp(&vec3Diffuse, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &m_SparkInfo.m_v3Color, m_SparkInfo.m_fLerp);
			SetDiffuse(vec3Diffuse.x, vec3Diffuse.y, vec3Diffuse.z);
		}

		pObject3D->Render(pd3dDevice, pElem->m_ppd3d_VBSel, m_fFrameCurrent, nNextFrame, &m1, pElem->m_nEffect, dwBlendFactor);

		pObject3D->m_nNoEffect = 0;

		if (pElem->m_nPartsIdx == PARTS_HAIR)
		{
			memcpy(s_fDiffuse, g_fDiffuse, sizeof(FLOAT) * 4);
			memcpy(s_fAmbient, g_fAmbient, sizeof(FLOAT) * 4);

			pd3dDevice->SetVertexShaderConstantF(93, (float*)&s_fDiffuse, 1);
			pd3dDevice->SetVertexShaderConstantF(94, (float*)&s_fAmbient, 1);
		}
	}

#ifdef __ATTACH_MODEL
	RenderAttachModel(pd3dDevice, mWorld);
#endif //__ATTACH_MODEL


	if (m_dwBlendFactor < 255 || m_dwColor)
	{
		if (m_nNoEffect == 0)
		{
			pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		}
	}

	SetLightVec(vec3LightBackup);
	SetDiffuse(vec4Diffuse.x, vec4Diffuse.y, vec4Diffuse.z);

	return 1;
#endif // !__WORLDSERVER
}

#ifdef __CLIENT
void CModelObject::RenderEffect(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* mWorld, DWORD dwItemKind3, DWORD dwItemKind3Right, int nLevelL, int nLeveR)
#endif // __CLIENT
{
	if (g_Option.m_bSFXItemRenderOff && dwItemKind3 != IK3_YOYO)
		return;

	O3D_ELEMENT* pElem;
	int	i;
	BOOL bYoYoAttakLEquip = FALSE;
	if (m_nNoEffect == 0)
	{

		if (m_pForce && m_pForce->m_nMaxSpline > 0)	m_pForce->Draw(pd3dDevice, mWorld);
		if (m_pForce2 && m_pForce2->m_nMaxSpline > 0)	m_pForce2->Draw(pd3dDevice, mWorld);

#ifdef __CLIENT
		if (CObj::GetActiveObj())
		{
			int nEffect, nLevel, nLevel2;
			nLevel2 = nLeveR;

			pElem = GetParts(PARTS_RWEAPON);

			if (!g_Option.m_bSFXItemRenderOff && pElem && (pElem->m_nEffect & XE_HIDE) == 0)
			{
				nEffect = pElem->m_nEffect & 0x00FFFFFF;
				nLevel = (pElem->m_nEffect >> 24);
#ifdef __CLIENT
				if (nEffect & XE_ITEM_FIRE)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_FIRE, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_ELEC)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_ELEC, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WATER)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WATER, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WIND)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WIND, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_EARTH)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_EARTH, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_FIRE_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_FIRE_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_ELEC_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_ELEC_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WATER_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WATER_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WIND_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_WIND_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_EARTH_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_EARTH_AL, nLevel, nLevel2);

				if (nEffect & XE_ITEM_GEN)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_GEN, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_GEN_AL)
					CreateWeaponEffect(PARTS_RWEAPON, dwItemKind3Right, mWorld, PE_GEN_AL, nLevel, nLevel2);
#endif // __CLIENT
			}

			if (dwItemKind3Right != IK3_YOYO)
			{
				pElem = GetParts(PARTS_LWEAPON);
				if (!pElem)
					pElem = GetParts(PARTS_SHIELD);
			}
			else
			{
				O3D_ELEMENT* pRweaponElem = GetParts(PARTS_RWEAPON);
				O3D_ELEMENT* pLweaponElem = GetParts(PARTS_LWEAPON);
				if ((pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) == 0) &&
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) == 0))
					pElem = GetParts(PARTS_RWEAPON);
				else if ((pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) == 0) &&
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) != 0))
					pElem = GetParts(PARTS_LWEAPON);
				else if ((pRweaponElem && (pRweaponElem->m_nEffect & XE_HIDE) != 0) &&
					(pLweaponElem && (pLweaponElem->m_nEffect & XE_HIDE) == 0))
				{
					pElem = GetParts(PARTS_RWEAPON);
					bYoYoAttakLEquip = TRUE;
				}
			}
			if (!g_Option.m_bSFXItemRenderOff && pElem && (bYoYoAttakLEquip || (pElem->m_nEffect & XE_HIDE) == 0))
			{
				nEffect = pElem->m_nEffect & 0x00FFFFFF;
				nLevel = (pElem->m_nEffect >> 24);


				if (dwItemKind3Right == IK3_YOYO)
				{
					dwItemKind3 = dwItemKind3Right;
					nLevel2 = nLeveR;
				}
				else
					nLevel2 = nLevelL;

				if (nEffect & XE_ITEM_FIRE)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_FIRE, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_ELEC)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_ELEC, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WATER)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_WATER, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WIND)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_WIND, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_EARTH)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_EARTH, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_FIRE_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_FIRE_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_ELEC_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_ELEC_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WATER_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_WATER_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_WIND_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_WIND_AL, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_EARTH_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_EARTH_AL, nLevel, nLevel2);
				// reg
				if (nEffect & XE_ITEM_GEN)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_GEN, nLevel, nLevel2);
				else if (nEffect & XE_ITEM_GEN_AL)
					CreateWeaponEffect(PARTS_LWEAPON, dwItemKind3, mWorld, PE_GEN_AL, nLevel, nLevel2);

			}
		}
#endif

		for (i = 0; i < MAX_ELEMENT; i++)
		{
			pElem = &m_Element[i];
			pElem->m_nEffect = 0;
		}
	}
}
#endif // WORLDSERVER



HRESULT CModelObject::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

#ifdef __ATTACH_MODEL
	InitAttachModelDeviceObjects(pd3dDevice);
#endif //__ATTACH_MODEL
	return S_OK;
}

HRESULT CModelObject::RestoreDeviceObjects()
{
	int		i;
	O3D_ELEMENT* pElem;
	CObject3D* pObject3D;

	SetGroup(0);
	for (i = 0; i < MAX_ELEMENT; i++)
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if (pObject3D)
		{

			pObject3D->RestoreDeviceObjects(pElem->m_ppd3d_VB);
		}
	}

#ifdef __CLIENT
	if (m_pm_pPartsEffectRegular[0])
		m_pm_pPartsEffectRegular[0]->RestoreDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectRegular[1])
		m_pm_pPartsEffectRegular[1]->RestoreDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectElement[0])
		m_pm_pPartsEffectElement[0]->RestoreDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectElement[1])
		m_pm_pPartsEffectElement[1]->RestoreDeviceObjects(m_pd3dDevice);
#endif // __CLIENT

#ifdef __ATTACH_MODEL
	RestoreAttachModelDeviceObjects();
#endif //__ATTACH_MODEL

	return S_OK;
}
HRESULT CModelObject::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();

#ifdef __CLIENT
	if (m_pm_pPartsEffectRegular[0])
		m_pm_pPartsEffectRegular[0]->InvalidateDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectRegular[1])
		m_pm_pPartsEffectRegular[1]->InvalidateDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectElement[0])
		m_pm_pPartsEffectElement[0]->InvalidateDeviceObjects(m_pd3dDevice);
	if (m_pm_pPartsEffectElement[1])
		m_pm_pPartsEffectElement[1]->InvalidateDeviceObjects(m_pd3dDevice);
#endif // __CLIENT

	return  S_OK;
}
HRESULT CModelObject::DeleteDeviceObjects()
{
	int		i, j;
	O3D_ELEMENT* pElem;
	CObject3D* pObject3D;

	for (i = 0; i < MAX_ELEMENT; i++)
	{
		pElem = &m_Element[i];
		pObject3D = pElem->m_pObject3D;
		if (pObject3D)
		{
			if (pElem->m_ppd3d_VB)
			{
				pElem->m_pObject3D->SetLOD(0);
				for (j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
					SAFE_RELEASE(pElem->m_ppd3d_VB1[j]);
				pElem->m_pObject3D->SetLOD(1);
				for (j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
					SAFE_RELEASE(pElem->m_ppd3d_VB2[j]);
				pElem->m_pObject3D->SetLOD(2);
				for (j = 0; j < pElem->m_pObject3D->GetMaxObject(); j++)
					SAFE_RELEASE(pElem->m_ppd3d_VB3[j]);
				pElem->m_pObject3D->SetLOD(0);

			}
			//			pObject3D->DeleteDeviceObjects();
		}
	}

#ifdef __ATTACH_MODEL
	DeleteAttachModelDeviceObjects();
#endif //__ATTACH_MODEL

	return  S_OK;
}

void	CModelObject::FrameMove(D3DXVECTOR3* pvSndPos, float fSpeed)
{

#ifndef __CLIENT
	UNREFERENCED_PARAMETER(pvSndPos);
#endif

#ifdef __CLIENT
	if (m_pMotion)
	{
		MOTION_ATTR* pAttr = IsAttrSound();
		if (pAttr && pAttr->m_nSndID > 0 && m_nPause == 0)
			PLAYSND(pAttr->m_nSndID, pvSndPos);

		int nMaxFrame = GetMaxFrame();
		if (nMaxFrame > 0)
		{
			int i = (m_nFrameOld + 1) % nMaxFrame;
			int nFrame = (static_cast<int>(m_fFrameCurrent) + 1) % nMaxFrame;
			while (i != nFrame)
			{
				BOOL bAttr = FALSE;
				BOOL bQuake = IsAttrQuake((float)(i));
				if (bQuake && m_nPause == 0)
				{
					CMover* pMover = CMover::GetActiveMover();
					if (pMover)
					{
						ItemProp* pItemProp = pMover->GetActiveHandItemProp();
						if (pItemProp && pItemProp->dwItemKind3 != IK3_YOYO && !pMover->IsActiveMover())
						{
							pMover->GetWorld()->m_pCamera->SetQuake(15);
							bAttr = TRUE;
						}
					}
				}
				if (bAttr)
					break;
				i = (i + 1) % nMaxFrame;
			}
		}

	}
	else
	{
		CObject3D* pObject = GetObject3D();
		if (pObject->GetMaxFrame())
		{
			MOTION_ATTR* pAttr = pObject->IsAttrSound(m_fFrameCurrent);
			if (pAttr && pAttr->m_nSndID > 0 && m_nPause == 0)
				PLAYSND(pAttr->m_nSndID, pvSndPos);

			int i = (m_nFrameOld + 1) % pObject->GetMaxFrame();
			int nFrame = (static_cast<int>(m_fFrameCurrent) + 1) % pObject->GetMaxFrame();
			while (i != nFrame)
			{
				BOOL bAttr = FALSE;
				BOOL bQuake = pObject->IsAttrQuake((float)(i));
				if (bQuake && m_nPause == 0)
				{
					CMover* pMover = CMover::GetActiveMover();
					if (pMover && m_nPause == 0)
					{
						pMover->GetWorld()->m_pCamera->SetQuake(30, 1.0f);
						bAttr = TRUE;
					}
				}
				if (bAttr)
					break;

				int nMaxFrame = GetMaxFrame();
				if (nMaxFrame > 0)
					i = (i + 1) % nMaxFrame;
			}
		}
	}
#endif

	CModel::FrameMove(NULL, fSpeed);

#if defined(__CPU_UTILDOWN_060502)
#if defined(__WORLDSERVER)
	return;
#endif
#endif

	if (m_pForce)
	{
		if ( /*!m_bEndFrame &&*/ m_pForce->m_nMaxSpline && !m_nPause)
		{
			m_pForce->m_nMaxDraw += (int)((MAX_SF_SLERP * 2 + 2) /** fSpeed*/);
			if (m_pForce->m_nMaxDraw > m_pForce->m_nMaxSpline)
			{
				if (m_nLoop & ANILOOP_LOOP)
					m_pForce->m_nMaxDraw = 0;
				else
					m_pForce->m_nMaxDraw = m_pForce->m_nMaxSpline;
			}

		}

		m_pForce->Process();
	}

	if (m_pForce2)
	{
		if (m_pForce2->m_nMaxSpline && !m_nPause)
		{
			m_pForce2->m_nMaxDraw += (int)((MAX_SF_SLERP * 2 + 2) /** fSpeed*/);
			if (m_pForce2->m_nMaxDraw > m_pForce2->m_nMaxSpline)
			{
				if (m_nLoop & ANILOOP_LOOP)
					m_pForce2->m_nMaxDraw = 0;
				else
					m_pForce2->m_nMaxDraw = m_pForce2->m_nMaxSpline;
			}

		}

		m_pForce2->Process();
	}

	if (m_bMotionBlending)
	{
		m_fBlendWeight += 0.05f;
		if (m_fBlendWeight >= 1.0f)
			SetMotionBlending(FALSE);
	}

#ifdef __ATTACH_MODEL
	FrameMoveAttachModel();
#endif //__ATTACH_MODEL
}




void	CModelObject::MakeSWDForce(int nParts, DWORD dwItemKind3, BOOL bSlow, DWORD dwColor, float fSpeed)
{
#ifdef __CLIENT
	D3DXVECTOR3		v1, v2;
	O3D_ELEMENT* pElem = GetParts(nParts);
	D3DXMATRIX	m1;
	if (!pElem)
		return;

	if (pElem->m_pObject3D == NULL)		return;

	if (pElem->m_pObject3D->m_vForce1.x == 0)	return;
	if (pElem->m_pObject3D->m_vForce2.x == 0)	return;


	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	int nLoop = m_nLoop;
	m_nLoop = ANILOOP_1PLAY;

	CreateForce(nParts);
	CSwordForce* pForce;
	if (nParts == PARTS_RWEAPON)
		pForce = m_pForce;
	else
		pForce = m_pForce2;

	pForce->Clear();
	pForce->m_dwColor = dwColor;

	D3DXMATRIX* pmLocal = NULL;
	if (pElem->m_nParentIdx == GetRHandIdx())
		pmLocal = &pElem->m_mLocalRH;
	else if (pElem->m_nParentIdx == GetLHandIdx())
		pmLocal = &pElem->m_mLocalLH;
	else if (pElem->m_nParentIdx == GetLArmIdx())
		pmLocal = &pElem->m_mLocalShield;
	else if (pElem->m_nParentIdx == GetRArmIdx())
		pmLocal = &pElem->m_mLocalKnuckle;

	if (!pmLocal)
		return;

	while (1)
	{
		if (m_pMotion)
			m_pMotion->AnimateBone(m_mUpdateBone, m_pMotionOld, m_fFrameCurrent, GetNextFrame(), m_nFrameOld, m_bMotionBlending, m_fBlendWeight);


		D3DXMatrixMultiply(&m1, pmLocal, &m_mUpdateBone[pElem->m_nParentIdx]);


		D3DXVec3TransformCoord(&v1, &(pElem->m_pObject3D->m_vForce1), &m1);
		D3DXVec3TransformCoord(&v2, &(pElem->m_pObject3D->m_vForce2), &m1);
		pForce->Add(v1, v2);

		CModel::FrameMove(NULL, fSpeed);
		if (bSlow && IsAttrHit(m_fFrameCurrent))
			m_bSlow = TRUE;
		if (m_bEndFrame)
			break;
	}

	pForce->MakeSpline();


	// rewind
	m_fFrameCurrent = 0.0f;
	m_nPause = 0;
	m_bEndFrame = FALSE;
	m_nLoop = nLoop;
	m_bSlow = FALSE;
#endif
}

void	CModelObject::SetMotionBlending(BOOL bFlag)
{
	if (bFlag == m_bMotionBlending)	return;
	m_bMotionBlending = bFlag;
	m_fBlendWeight = 0.0f;
}


void	CModelObject::GetForcePos(D3DXVECTOR3* vOut, int nIdx, int nParts, const D3DXMATRIX& mWorld)
{
	D3DXMATRIX* pmLocal = NULL;
	O3D_ELEMENT* pElem = GetParts(nParts);
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	if (!pElem)
		return;

	if (pElem->m_pObject3D == NULL)		return;

	if (pElem->m_pObject3D->m_vForce1.x == 0)	return;
	if (pElem->m_pObject3D->m_vForce2.x == 0)	return;

	if (pElem->m_nParentIdx == GetRHandIdx())
		pmLocal = &pElem->m_mLocalRH;
	else if (pElem->m_nParentIdx == GetLHandIdx())
		pmLocal = &pElem->m_mLocalLH;
	else
	{
		D3DXMatrixIdentity(&m1);
		pmLocal = &m1;
	}

	if (!pmLocal)
		return;


	//	if( m_pMotion )


	D3DXMatrixMultiply(&m1, pmLocal, &m_mUpdateBone[pElem->m_nParentIdx]);
	if (nIdx == 0)
		D3DXVec3TransformCoord(&v1, &(pElem->m_pObject3D->m_vForce1), &m1);
	else
		D3DXVec3TransformCoord(&v1, &(pElem->m_pObject3D->m_vForce2), &m1);

	D3DXVec3TransformCoord(&v1, &v1, &mWorld);
	*vOut = v1;

}


void	CModelObject::GetForcePos(D3DXVECTOR3* vOut, int nIdx, int nParts, const D3DXMATRIX& mWorld, float fScale)
{
	D3DXMATRIX mLocal;
	O3D_ELEMENT* pElem = GetParts(nParts);
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	if (!pElem)
		return;

	if (pElem->m_pObject3D == NULL)		return;

	if (pElem->m_pObject3D->m_vForce1.x == 0)	return;
	if (pElem->m_pObject3D->m_vForce2.x == 0)	return;

	if (pElem->m_nParentIdx == GetRHandIdx())
	{
		mLocal = pElem->m_mLocalRH;
		D3DXMatrixScaling(&mLocal, fScale, fScale, fScale);
	}
	else if (pElem->m_nParentIdx == GetLHandIdx())
	{
		mLocal = pElem->m_mLocalLH;
		D3DXMatrixScaling(&mLocal, fScale, fScale, fScale);
	}
	else
	{
		D3DXMatrixIdentity(&m1);
		mLocal = m1;
	}


	//	if( m_pMotion )


	D3DXMatrixMultiply(&m1, &mLocal, &m_mUpdateBone[pElem->m_nParentIdx]);
	if (nIdx == 0)
		D3DXVec3TransformCoord(&v1, &(pElem->m_pObject3D->m_vForce1), &m1);
	else
		D3DXVec3TransformCoord(&v1, &(pElem->m_pObject3D->m_vForce2), &m1);

	D3DXVec3TransformCoord(&v1, &v1, &mWorld);
	*vOut = v1;

}



void	CModelObject::GetHandPos(D3DXVECTOR3* vOut, int nParts, const D3DXMATRIX& mWorld)
{
	D3DXMATRIX* pmLocal = NULL;
	D3DXMATRIX	m1;
	D3DXVECTOR3		v1;

	int nParentIdx = 0;
	if (m_pBone == NULL)	return;
	if (nParts == PARTS_RWEAPON)
	{
		nParentIdx = GetRHandIdx();
		pmLocal = &m_pBone->m_mLocalRH;
	}
	else if (nParts == PARTS_LWEAPON)
	{
		nParentIdx = GetLHandIdx();
		pmLocal = &m_pBone->m_mLocalLH;
	}
	else
	{
		D3DXMatrixIdentity(&m1);
		pmLocal = &m1;
	}

	if (!pmLocal)
		return;

	//	if( m_pMotion )



	D3DXMatrixMultiply(&m1, pmLocal, &m_mUpdateBone[nParentIdx]);
	v1.x = m1._41;		v1.y = m1._42;		v1.z = m1._43;

	D3DXVec3TransformCoord(&v1, &v1, &mWorld);
	*vOut = v1;

}


D3DXVECTOR3		SplineSlerp(D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3, D3DXVECTOR3* v4, float fSlerp)
{
	D3DXVECTOR3		vResult;

	float	t = fSlerp;
	float	t2 = t * t;
	float	t3 = t2 * t;

	D3DXVECTOR3		m0, m1;
	const float alpha = 0.0f;

	m0 = ((1 - alpha) / 2.0f) *
		((*v2 - *v1) + *v3 - *v2);
	m1 = ((1 - alpha) / 2.0f) *
		((*v3 - *v2) + *v4 - *v3);

	vResult = (((2 * t3) - (3 * t2) + 1) * *v2) +
		((t3 - (2 * t2) + t) * m0) +
		((t3 - t2) * m1) +
		(((-2 * t3) + (3 * t2)) *
			*v3);

	return vResult;
}

void	CSwordForce::Add(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	if (m_nMaxVertex >= MAX_SF_SWDFORCE)
	{
		LPCTSTR szErr = Error("SWDForceAdd : 범위 초과 %d", m_nMaxVertex);
		//ADDERRORMSG( szErr );
		return;
	}
	FVF_SWDFORCE* pList = &m_aList[m_nMaxVertex];

	pList->position = v1;
	pList->color = 0xffffffff;

	pList = &m_aList2[m_nMaxVertex];
	pList->position = v2;
	pList->color = 0xffffffff;

	m_nMaxVertex++;
}



void	CSwordForce::MakeSpline(void)
{
	FVF_SWDFORCE* pKey;
	int		i, j;
	D3DXVECTOR3 vSlp;
	int		a, b, c, d;
	int		nMaxVertex = m_nMaxVertex;

	m_nMaxSpline = 0;
	for (i = 0; i < nMaxVertex - 1; i++)
	{

		a = i - 1;		if (a < 0)	a = 0;
		b = i;
		c = i + 1;
		d = i + 2;		if (d >= nMaxVertex)	d = nMaxVertex - 1;
		for (j = 0; j < MAX_SF_SLERP + 1; j++)
		{
			if (m_nMaxSpline >= MAX_SF_SPLINE)
			{
				Error("SWDForceAdd Spline : %d", m_nMaxSpline);
				break;
			}
			pKey = m_aList;
			vSlp = SplineSlerp(&pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP);
			m_aSpline[m_nMaxSpline].position = vSlp;
			m_aSpline[m_nMaxSpline].color = 0xffffffff;
			m_nMaxSpline++;
			pKey = m_aList2;
			vSlp = SplineSlerp(&pKey[a].position, &pKey[b].position, &pKey[c].position, &pKey[d].position, (float)j / MAX_SF_SLERP);
			m_aSpline[m_nMaxSpline].position = vSlp;
			m_aSpline[m_nMaxSpline].color = 0xffffffff;
			m_nMaxSpline++;
		}
	}
}

void	CSwordForce::Process(void)
{
	FVF_SWDFORCE* pList = m_aSpline;
	int		i;
	int		nRed1 = 0;
	int		nGreen1 = 0;
	int		nBlue1 = 0;
	int		nRed2 = (m_dwColor >> 16) & 0xff;
	int		nGreen2 = (m_dwColor >> 8) & 0xff;
	int		nBlue2 = m_dwColor & 0xff;

	if (m_nMaxSpline == 0)		return;

	for (i = m_nMaxDraw - 1; i >= 0; i--)
	{
		if (i % 2)
			pList[i].color = D3DCOLOR_ARGB(255, nRed1, nGreen1, nBlue1);
		else
			pList[i].color = D3DCOLOR_ARGB(255, nRed2, nGreen2, nBlue2);

		nRed1 -= 3; if (nRed1 < 0)	nRed1 = 0;
		nGreen1 -= 3; if (nGreen1 < 0) nGreen1 = 0;
		nBlue1 -= 3; if (nBlue1 < 0)	nBlue1 = 0;

		nRed2 -= 3; if (nRed2 < 0)	nRed2 = 0;
		nGreen2 -= 3; if (nGreen2 < 0) nGreen2 = 0;
		nBlue2 -= 3; if (nBlue2 < 0)	nBlue2 = 0;
	}

}




void	CSwordForce::Draw(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* mWorld)
{
	pd3dDevice->SetTransform(D3DTS_WORLD, mWorld);
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetFVF(D3DFVF_SWDFORCE);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	{
		int		nShare, nRest;
		int		i;
		FVF_SWDFORCE* p = m_aSpline;
		int		nMaxDraw = m_nMaxDraw;

		nMaxDraw -= 2;
		nShare = nMaxDraw / 64;
		nRest = nMaxDraw % 64;
		for (i = 0; i < nShare; i++)
		{
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 64, p, sizeof(FVF_SWDFORCE));
			p += 64;
		}

		if (nRest > 0)
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, nRest, p, sizeof(FVF_SWDFORCE));

	}

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}

#ifdef __CLIENT
void	CModelObject::CreateWeaponEffect(int nParts, DWORD dwItemKind3, const D3DXMATRIX* pmWorld, int nType, int nLevel, int nLevel2)
{
	if (nType >= PE_GEN && nLevel < 3)
		return;
	if (nType < PE_EARTH_AL && nLevel2 < 3 && nLevel < 3)
		return;

	short nElementLevel = 0;
	if (nLevel2 >= 20)	nElementLevel = 5;
	else if (nLevel2 >= 16)	nElementLevel = 4;
	else if (nLevel2 >= 11)	nElementLevel = 3;
	else if (nLevel2 >= 6)	nElementLevel = 2;
	else if (nLevel2 >= 3)	nElementLevel = 1;

	short nRegularLevel = 0;
	if (nLevel >= 10)	nRegularLevel = 5;
	else if (nLevel >= 9)		nRegularLevel = 4;
	else if (nLevel >= 7)		nRegularLevel = 3;
	else if (nLevel >= 5)		nRegularLevel = 2;
	else if (nLevel >= 3)		nRegularLevel = 1;

	O3D_ELEMENT* pElem = GetParts(nParts);
	if (!pElem)
	{
		if (nParts == 9)
		{
			pElem = GetParts(PARTS_SHIELD);
			if (!pElem)
				return;
		}
		else
			return;
	}

	if (pElem->m_pObject3D == NULL)
		return;

	// getmatrix
	D3DXMATRIX* pmLocal = NULL;
	if (pElem->m_nParentIdx == GetRHandIdx())
		pmLocal = &pElem->m_mLocalRH;
	else if (pElem->m_nParentIdx == GetLHandIdx())
		pmLocal = &pElem->m_mLocalLH;
	else if (pElem->m_nParentIdx == GetLArmIdx())
		pmLocal = &pElem->m_mLocalShield;
	else if (pElem->m_nParentIdx == GetRArmIdx())
		pmLocal = &pElem->m_mLocalKnuckle;

	if (!pmLocal)
		return;

	assert(pmLocal);

	D3DXMATRIX m1;
	D3DXMatrixIdentity(&m1);
	D3DXMATRIX m2;
	D3DXMatrixIdentity(&m2);

	D3DXMatrixMultiply(&m1, pmLocal, &m_mUpdateBone[pElem->m_nParentIdx]);
	m2 = m1 * *pmWorld;


	int dwSfxRegular = XI_WEA_SWORD01;
	int dwSfxElement = XI_WEA_SWORD01;
	float fOffset = 0;

	switch (dwItemKind3)
	{
	case IK3_SWD: dwSfxRegular = XI_WEA_SWORD01;	fOffset = 0.10f; break;
	case IK3_THSWD: dwSfxRegular = XI_WEA_TWOSWORD01;	fOffset = 0.00f; break;
	case IK3_AXE: dwSfxRegular = XI_WEA_AXE01;		fOffset = 0.10f; break;
	case IK3_THAXE: dwSfxRegular = XI_WEA_TWOAXE01;	fOffset = 0.10f; break;
	case IK3_YOYO: dwSfxRegular = XI_WEA_YOYO01;		fOffset = 0.00f; break;
	case IK3_KNUCKLEHAMMER: dwSfxRegular = XI_WEA_KNUCK01;	fOffset = 0.10f; break;
	case IK3_CHEERSTICK: dwSfxRegular = XI_WEA_STICK01;	fOffset = 0.00f; break;
	case IK3_STAFF: dwSfxRegular = XI_WEA_STAFF01;	fOffset = 0.00f; break;
#ifdef __CROSSBOW
	case IK3_CROSSBOW: dwSfxRegular = XI_WEA_CROSSBOW01;	fOffset = 0.10f; break;
#endif // __CROSSBOW
	case IK3_BOW: dwSfxRegular = XI_WEA_BOW01;		fOffset = 0.00f; break;
	case IK3_WAND: dwSfxRegular = XI_WEA_WAND01;		fOffset = 0.10f; break;
	case IK3_SHIELD: dwSfxRegular = XI_WEA_SHI01;		fOffset = 0.00f; break;
#if __VER >= 16
	case IK3_MAGICBARUNA: dwSfxRegular = XI_WEA_SHI01; fOffset = -0.45f; break;
	case IK3_ZEMBARUNA: dwSfxRegular = XI_WEA_SHI01;	fOffset = 0.00f; break;
#endif
	default: return;
	}
	dwSfxElement = dwSfxRegular;

	switch (nType)
	{
	case PE_FIRE: dwSfxElement += 5;  break;
	case PE_FIRE_AL: dwSfxElement += 5;  break;
	case PE_WATER: dwSfxElement += 10; break;
	case PE_WATER_AL: dwSfxElement += 10; break;
	case PE_WIND: dwSfxElement += 15; break;
	case PE_WIND_AL: dwSfxElement += 15; break;
	case PE_ELEC: dwSfxElement += 20; break;
	case PE_ELEC_AL: dwSfxElement += 20; break;
	case PE_EARTH: dwSfxElement += 25; break;
	case PE_EARTH_AL: dwSfxElement += 25; break;
	default: break;
	}

	int nPart = 0;
	if (nParts == PARTS_LWEAPON)
		nPart = 0;
	else
		nPart = 1;

	D3DXVECTOR3 v3(0.0f + fOffset, 0.0f, 0.0f);
	if (nElementLevel > 0)
	{
		dwSfxElement += (nElementLevel - 1);

		if (m_pm_pPartsEffectElement[nPart])
		{
			if (m_pm_pPartsEffectElement[nPart]->m_nType != nType || m_pm_pPartsEffectElement[nPart]->m_dwItemKind3 != dwItemKind3)
				SAFE_DELETE(m_pm_pPartsEffectElement[nPart]);
		}

		if (m_pm_pPartsEffectElement[nPart] == NULL)
			m_pm_pPartsEffectElement[nPart] = new CPartsWeaponEffect(nType, dwItemKind3);

		CPartsWeaponEffect* pEffectElement = (CPartsWeaponEffect*)m_pm_pPartsEffectElement[nPart];
		pEffectElement->Create(m_pd3dDevice, v3, dwSfxElement);
		pEffectElement->Render(m_pd3dDevice, &m2);
	}

	if (nRegularLevel > 0)
	{
		dwSfxRegular += (nRegularLevel - 1);

		if (m_pm_pPartsEffectRegular[nPart])
		{
			if (m_pm_pPartsEffectRegular[nPart]->m_nType != nType || m_pm_pPartsEffectRegular[nPart]->m_dwItemKind3 != dwItemKind3)
				SAFE_DELETE(m_pm_pPartsEffectRegular[nPart]);
		}

		if (m_pm_pPartsEffectRegular[nPart] == NULL)
			m_pm_pPartsEffectRegular[nPart] = new CPartsWeaponEffect(nType, dwItemKind3);

		CPartsWeaponEffect* pEffectRegular = (CPartsWeaponEffect*)m_pm_pPartsEffectRegular[nPart];
		pEffectRegular->Create(m_pd3dDevice, v3, dwSfxRegular);
		pEffectRegular->Render(m_pd3dDevice, &m2);
	}
}
#endif // __CLIENT