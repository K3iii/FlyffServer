#ifndef	__OBJECT3D_H__
#define	__OBJECT3D_H__

#include <d3dx9.h>
#include "Material.h"
#include "Bone.h"

#define D3DFVF_SKINVERTEX_BUMP (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SKINVERTEX (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX_BUMP (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SHADOWMASKVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct SHADOWMASK_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

// Mesh Type
enum GMTYPE
{
	GMT_ERROR = -1,
	GMT_NORMAL,
	GMT_SKIN,
	GMT_BONE
};

// Vertex Buffer Type
enum VBTYPE
{
	VBT_ERROR = -1,
	VBT_NORMAL,
	VBT_BUMP,
};


struct	INDEX_BUFFER
{
	int		nIdx;
	D3DXVECTOR3		normal;
};

struct SKINVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

struct NORMALVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

struct	MATERIAL_BLOCK
{
	int		m_nStartVertex;
	int		m_nPrimitiveCount;
	int		m_nTextureID;
//	int		m_n2Side;				// 2 side render block
//	int		m_nReflect;
//	int		m_nOpacity;
	DWORD	m_dwEffect;
	int		m_nAmount;
	int		m_nMaxUseBone;
	int		m_UseBone[MAX_VS_BONE];
};

struct GMOBJECT;


// Geometry Object

struct GMOBJECT		// base object
{
	GMTYPE	m_Type;					// GMOBJECT Type
	int			m_nVertexSize;		// sizeof(FVF)
	DWORD		m_dwFVF;
	int			m_nMaxUseBone;
	int			m_UseBone[ MAX_VS_BONE ];
	int			m_nID;
	int			m_bOpacity;
	int			m_bBump;
	int			m_bRigid;
	int			m_bLight;
	int			m_nMaxVertexList;			// read
	int			m_nMaxFaceList;				// read
	int			m_nMaxMtrlBlk;				// read
	int			m_nMaxMaterial;				// read
	GMTYPE		m_ParentType;
	int			m_nParentIdx;
	GMOBJECT*	m_pParent;
	D3DXMATRIX		m_mLocalTM;			// read
	D3DXMATRIX		m_mInverseTM;

	D3DXVECTOR3		*m_pVertexList;
//	INDEX_BUFFER	*m_pIndexBuffer;		// read
	int				*m_pPhysiqueVertex;
	MATERIAL_BLOCK	*m_pMtrlBlk;			// read
	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;

	TM_ANIMATION	*m_pFrame;

	D3DXVECTOR3		m_vBBMin, m_vBBMax;			// read bound box min, max
//	D3DXVECTOR3		m_vBBVList[8];				// read bound box vlist

	int				m_bMaterial;
	MATERIAL		m_MaterialAry[16];


//	NORMALVERTEX	*m_ptempVB;
	void			*m_pVB;

	WORD			*m_pIB;
	WORD			*m_pIIB;
	int				m_nMaxVB;
	int				m_nMaxIB;


	LPDIRECT3DVERTEXBUFFER9		m_pd3d_VB;
	LPDIRECT3DINDEXBUFFER9		m_pd3d_IB;

};

#define		MAX_GROUP		3

#define		LOD_HIGH	0
#define		LOD_MEDIUM	1
#define		LOD_LOW		2

struct LOD_GROUP
{
	int			m_nMaxObject;
	GMOBJECT*	m_pObject;		// sizeof(GMOBJECT) * m_nMaxObject


	D3DXMATRIX	*_mUpdate;
};

class CObject3D
{
private:
	LOD_GROUP		*m_pGroup;
	LOD_GROUP		m_Group[ MAX_GROUP ];
	D3DXMATRIX	*m_pmExternBone;
	D3DXMATRIX	*m_pmExternBoneInv;

	int			m_nMaxBone;
	D3DXMATRIX	*m_pBaseBone;
	D3DXMATRIX	*m_pBaseBoneInv;
	CMotion		*m_pMotion;

	int		m_nMaxEvent;

	int		m_nTextureEx;
	MOTION_ATTR		*m_pAttr;

public:
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	int		m_bSendVS;
	int		m_bLOD;
	int		m_nID;
	int		m_nHavePhysique;
	char	m_szFileName[64];
	GMOBJECT	m_CollObject;


	D3DXVECTOR3		m_vBBMin, m_vBBMax;			// read bound box min, max
	D3DXVECTOR3		m_vForce1, m_vForce2;
#if __VER >= 9 // __CSC_VER9_5
	D3DXVECTOR3		m_vForce3, m_vForce4;
#endif //__CSC_VER9_5
	float	m_fScrlU, m_fScrlV;
	int		m_nMaxFace;
	int		m_nNoTexture;
	int		m_nNoEffect;


//	double		m_dKPS;
	float		m_fPerSlerp;
	int			m_nMaxFrame;
//	D3DXVECTOR3		m_vBBVList[8];				// read bound box vlist

	int		m_nUseCnt;

	FLOAT	m_fAmbient[3];

private:
	void	Init( void );
	void	Destroy( void );

	int		LoadGMObject( CResFile *file, GMOBJECT *pObject );
	int		LoadTMAni	( CResFile *file, GMOBJECT *pObject );
	int		SaveGMObject( FILE *fp, GMOBJECT *pObject );
	int		SaveTMAni	( FILE *fp, GMOBJECT *pObject );

public:
	CObject3D();
	~CObject3D();

	const int	GetMaxEvent( )		{ return m_nMaxEvent; }
	GMOBJECT*	GetGMOBJECT( int nIdx = 0 ) { return &m_pGroup->m_pObject[ nIdx ]; }
	int		GetMaxObject( void ) { return m_pGroup->m_nMaxObject; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }
	int		GetMaxBone( void )	{ return m_nMaxBone; }
	D3DXMATRIX *GetBaseBone( void ) { return m_pBaseBone; }
	void	SetExternBone( D3DXMATRIX *pmBone, D3DXMATRIX *pmBoneInv ) { if(m_pBaseBone == NULL) { m_pmExternBone = pmBone; m_pmExternBoneInv = pmBoneInv; } }
//	int		Find( int nID );
	void	SetTexture( LPDIRECT3DTEXTURE9 pTexture );
	void	SetTextureEx( GMOBJECT *pObj, int nNumEx );
	void	SetTextureEx( int nNumEx )
	{
		for( int i = 0; i < m_pGroup->m_nMaxObject; i ++ )
			SetTextureEx( &(m_pGroup->m_pObject[i]), nNumEx );
	}
	void	LoadTextureEx( int nNumEx, GMOBJECT *pObj, MATERIAL *pmMaterial[16] );
	void	ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest );
	void	SetBone( D3DXMATRIX *pmBone ) { m_pmExternBone = pmBone; }
	int		GetMaxVertex( void );
	int		GetMaxFace( void );
	int		GetMaxMtrlBlk( void );
	BOOL	IsHaveCollObj( void ) { return (m_CollObject.m_Type != GMT_ERROR)? TRUE : FALSE; }
	BOOL	IsUseExternBone( void ) { if( m_nHavePhysique && m_pBaseBone == NULL ) return TRUE; else return FALSE; }
	void	SetLOD( int nLevel )
	{
		if( nLevel < 0 || nLevel >= MAX_GROUP )
		{
			Error( "SetLOD : lv=%d, %s", nLevel, m_szFileName );
			nLevel = 0;
		}
		if( m_bLOD == 0 ) nLevel = 0;
		m_pGroup = &m_Group[nLevel];
	}	// 0:high,  1:medium,   2:low
	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }
	DWORD	GetAttr( int nNumFrm )
	{
		if( m_pAttr == NULL )	return 0;
		return m_pAttr[ nNumFrm ].m_dwAttr;
	}
	DWORD	IsAttrHit( float fNumFrm )
	{
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fNumFrm )
	{
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fNumFrm )
	{
		if( m_pAttr == NULL )	return 0;
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( pAttr->m_fFrame == fNumFrm )
				return pAttr->m_dwAttr;
			return 0;
	}


	HRESULT	CreateDeviceBuffer( GMOBJECT *pObject, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool = D3DPOOL_MANAGED );
	HRESULT SendVertexBuffer( GMOBJECT *pObj, LPDIRECT3DVERTEXBUFFER9 pd3d_VB );
	HRESULT SendIndexBuffer( GMOBJECT *pObj );
	int		LoadObject( LPCTSTR szFileName );
static 	BOOL IsAnimateFile( LPCTSTR szFileName );
	int		SaveObject( LPCTSTR szFileName );
	void	ClearNormal( void );


	int		SlideVectorXZ( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj/* = TRUE*/, int nSlideCnt );
	int		SlideVectorXZ2( D3DXVECTOR3 *pOut, D3DXVECTOR3 *pIntersect, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj = TRUE);
	int		SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect );
	void	FindTouchTriLine( D3DXVECTOR3 **pTriOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, FLOAT *pDist, BOOL bCollObj = TRUE );
	D3DXVECTOR3 *IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE );
#if __VER >= 13 // __HOUSING
	BOOL	SimpleTriIntersect(D3DXMATRIX mWorld, GMOBJECT* TargetObj, D3DXMATRIX mTargetWorld);
	void	ComputeInterval(float fVV0,float fVV1,float fVV2,float fD0,float fD1,float fD2,float fD0D1,float fD0D2,float &fA,float &fB,float &fC,float &fX0,float &fX1);
#endif // __HOUSING
	void	Skining( GMOBJECT *pObj, const D3DXMATRIX *mBones );
	HRESULT	SetVertexBuffer( GMOBJECT *pObj );
	void	Animate( FLOAT fFrameCurrent, int nNextFrame );
#ifndef __WORLDSERVER
	void    SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void    ResetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void	SetShader( const D3DXMATRIX *mWorld );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nEffect = 0, DWORD dwBlendFactor = 0xff000000 );
	void	RenderSkin( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pd3d_VB, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor = 0xff000000 );
	void	RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, int nBlendFactor = 255 );

	void	RenderBB( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
#ifdef __SHADOW
	void	RenderNormalShadow( LPDIRECT3DDEVICE9 pd3dDevice, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nBlendFactor );
	void	RenderShadow( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nBlendFactor = 255 );
//	void	RenderShadow( LPDIRECT3DDEVICE9 pd3dDevice, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nBlendFactor );
#endif
#endif

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice;  return S_OK; }
	HRESULT RestoreDeviceObjects( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool = D3DPOOL_MANAGED );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};

#define		MAX_OBJECT3D			2048
#define		MAX_OBJECT3D_CACHE		32

class	CObject3DMng
{
private:
#ifdef __JEFF_11_5
#ifndef __MODEL_CACHE
	map<string, CObject3D*>	m_mapObject3D;
#endif
#else	// __JEFF_11_5
	int			m_nCachePos;
	int			m_nSize;
	int			m_nMax;
//	DWORD		m_tmTimer;
//#ifdef __WORLDSERVER
//	CRIT_SEC	m_AccessLock;
//#endif	// __WORLDSERVER
	CObject3D		*m_pCache[ MAX_OBJECT3D_CACHE ];
	CObject3D		*m_pObject3DAry[ MAX_OBJECT3D ];
#endif	// __JEFF_11_5

	void	Init( void );

public:
	void	Destroy( void );
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	CObject3DMng();
	~CObject3DMng();

#ifdef __MODEL_CACHE
	int			DeleteObject3D(CObject3D* pObject3D, int nType);
	CObject3D* LoadObject3D(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType = 0);
#else
	int			DeleteObject3D( CObject3D *pObject3D );
	CObject3D	*LoadObject3D( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
#endif
	void	Process( void );

	//HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; }

};

extern CObject3DMng		g_Object3DMng;
extern D3DXMATRIX	g_mReflect;

HRESULT		CreateShadowVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
HRESULT		CreateSkinningVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
void		DeleteVertexShader( LPDIRECT3DDEVICE9 pd3dDevice );


HRESULT		CreateShadowMask( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight );
void	RenderShadowMask( LPDIRECT3DDEVICE9 pd3dDevice );
void	DeleteShadowMask( void );

void	SetTransformViewProj( const D3DXMATRIX &mViewProj );

D3DXVECTOR3 *GetLastPickTri( void );



#endif // OBJECT3D
