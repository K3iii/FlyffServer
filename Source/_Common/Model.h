#ifndef __MODEL_H
#define __MODEL_H

#include "ModelMng.h"
#include "bone.h"
#include "xUtil3d.h"
#include "Obj.h"

#ifndef __MODELTYPE_NONE
#define MODELTYPE_NONE               0
#define MODELTYPE_MESH               1
#define	MODELTYPE_ANIMATED_MESH		 2
#define MODELTYPE_BILLBOARD          3
#define MODELTYPE_SFX                4		// ANIMATED_BILLBOARD 4
#define MODELTYPE_ASE				 5
#endif


//     - z
//   3 | 2
// - --+-- + x
//   0 | 1

//   7 | 6
// - --+-- + x
//   4 | 5
struct BOUND_BOX
{
	D3DXVECTOR3		m_vPos[8];
};

// Model Render Effect
#define REFF_NONE    0
#define REFF_ALPHA   1
#define REFF_SFX     2

// file version
#define		VER_BONE			4
#define		VER_MESH			20
#define		VER_MOTION			10

#define		COL_WHITE		D3DCOLOR_ARGB(255, 255, 255, 255)
#define		COL_RED			D3DCOLOR_ARGB(255, 255,   0,   0)
#define		COL_GREEN		D3DCOLOR_ARGB(255,   0, 255,   0)
#define		COL_BLUE		D3DCOLOR_ARGB(255,   0,   0, 255)
#define		COL_GRAY		D3DCOLOR_ARGB(255, 128, 128, 128)
#define		COL_DARKGRAY	D3DCOLOR_ARGB(255,  96,  96,  96)
#define		COL_LIGHTGRAY	D3DCOLOR_ARGB(255, 192, 192, 192)


#define		BS_NONE			0
#define		BS_MODEL		1
#define		BS_OBJECT		2
#define		BS_MTRLBLK		3

class CObj;
class Segment3;

#if defined(__CLIENT)
typedef struct _SPARKINFO
{
	D3DXVECTOR3		m_v3Color;
	D3DXVECTOR3		m_v3SparkDir;
	BOOL			m_bUsed;
	FLOAT			m_fLerp;
	int				m_nCnt;

	_SPARKINFO() { m_nCnt = 0; m_fLerp = 1.0f; }
} _SPARKINFO, *_PSPARKINFO;
#endif //defined(__CLIENT)

class CModel
{
protected:
	LPDIRECT3DDEVICE9	m_pd3dDevice;        // The D3D rendering device
	int					m_nModelType;
	BOUND_BOX			m_BB;
	DWORD				m_dwRenderEffect;
	DWORD				m_dwBlendFactor;

public:
	DWORD				m_dwColor;
	D3DXVECTOR3			m_vMin, m_vMax;
	LPMODELELEM			m_pModelElem;

	int					m_nPause;
	int					m_bSlow;
	BOOL				m_bEndFrame;
	float				m_fFrameCurrent;
	float				m_fFrameOld;
	int					m_nFrameMax;
	float				m_fPerSlerp;
	float				m_fSlp;
	int					m_nLoop;
	BOOL				m_bSkin;
	int					m_nNoEffect;

#ifdef __CLIENT
	_SPARKINFO			m_SparkInfo;
#endif

	CModel() { Init(); }
	CModel(int nModelType) { Init(); m_nModelType = nModelType; }
	virtual ~CModel();

	void Init( void )
	{
		m_nModelType = MODELTYPE_MESH;
		m_dwRenderEffect = REFF_NONE;
		InitAnimate();
		m_vMin.x = m_vMin.y = m_vMin.z = 0.0f;
		m_vMax.x = m_vMax.y = m_vMax.z = 0.0f;
		m_dwBlendFactor = 255;
		m_dwColor = 0;
		m_pd3dDevice = NULL;
		m_bSkin = FALSE;
	#if defined(__CLIENT)
		memset( &m_SparkInfo, 0, sizeof(_SPARKINFO) );
	#endif //defined(__CLIENT)

		m_pModelElem = NULL;
		m_nNoEffect = 0;
		m_bSlow = FALSE;
	}
	void InitAnimate( void )
	{
		m_fFrameCurrent = 0.0f;
		m_fFrameOld = -1.0f;
		m_nFrameMax = 0;
		m_nPause = 0;
		m_bEndFrame = FALSE;
		m_nLoop = ANILOOP_LOOP;
		m_fSlp = 0.0f;
		m_fPerSlerp = 0.5f;
		m_bSlow = FALSE;
	}

	BOOL	IsNullBoundBox() { if( m_vMin == m_vMax || m_vMin.x == 65535.0f ) return TRUE; return FALSE; }
	void    SetBlendFactor( DWORD dwBlendFactor ) { m_dwBlendFactor = dwBlendFactor; }
	DWORD   GetBlendFactor( ) { return m_dwBlendFactor; }
	BOOL    IsAniable();
	BOOL	IsEndFrame() { return m_bEndFrame; }
	int     GetModelType() { return m_nModelType; }
	int		GetType() { return m_nModelType; }
	void	SetModelType(int nModelType) { m_nModelType = nModelType; }
	void	SetLoop( int nLoop ) { m_nLoop = nLoop; }
	void	SetSpeed( float fPerSlerp ) { m_fPerSlerp = fPerSlerp; }
	const BOUND_BOX*	GetBBVector( void ) { return &m_BB; }
	float	GetRadius( void );

	DWORD	GetRenderEffect() { return m_dwRenderEffect; }
	int		GetMaxFrame() { return m_nFrameMax; }
	int		GetNextFrame();

#ifdef __MODEL_CACHE
	virtual int		LoadModel(LPCTSTR szFileName, int nPart, int nType) { return 1; }
#else
	virtual int		LoadModel(LPCTSTR szFileName) { return 1; }
#endif
	virtual int		SaveModel( LPCTSTR szFileName ) { return 1; }
	virtual void	ClearNormal( void ) {}

	virtual void	AddFrame( FLOAT fAddFrame );
	virtual void	SetFrame( FLOAT fFrame );


	virtual BOOL	IsLoadAttr( void ) { return FALSE; }
	virtual MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return NULL; }
	virtual DWORD	GetAttr( int nNumFrm ) { return 0; }
	virtual DWORD	IsAttrHit( float fNumFrm ) { return 0; }
	virtual DWORD	IsAttrHit( void ) { return 0; }
	virtual MOTION_ATTR *IsAttrSound( float fNumFrm ) { return NULL; }
	virtual MOTION_ATTR *IsAttrSound( void ) { return 0; }
	virtual DWORD	IsAttrQuake( float fNumFrm ) { return 0; }
	virtual DWORD	IsAttrQuake( void ) { return 0; }
	virtual void	SetAttr( float fNumFrm, DWORD dwAttr ) {}
	virtual void	ResetAttr( int nNumFrm, DWORD dwAttr ) {}
	virtual	void	SetGroup( int nNum ) {}		// Set LOD Group
	virtual int		IsHaveCollObj( void ) { return 0; }

	// Rendering
	virtual BOOL	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
#ifdef __CLIENT
	virtual void	RenderEffect(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL, DWORD dwItemKind3 = NULL_ID, DWORD dwItemKind3Right = NULL_ID, int nLevelL = 0, int nLeveR = 0) {}
#endif // __CLIENT
	virtual void	FrameMove( D3DXVECTOR3 *pvSndPos = NULL, float fSpeed = 1.0f );
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice; return S_OK; }
	virtual HRESULT RestoreDeviceObjects() { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects() { return S_OK; }
	virtual HRESULT DeleteDeviceObjects() { return S_OK; }

#ifdef __CLIENT
	BOOL			IntersectBB( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist );
#endif

	virtual BOOL	Intersect( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) { return FALSE; }
	virtual D3DXVECTOR3 *IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE ) { return NULL; }
	virtual	BOOL	IsTouchOBB_Line( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, BOOL bNeedCollObject = TRUE ) { return FALSE;}
	virtual	BOOL	TestIntersectionOBB_Line( const Segment3& segment, const CObj* pObj, BOOL bNeedCollObject = TRUE ) { return FALSE;}

	FLOAT GetMaxWidth() { return ( m_vMax.x - m_vMin.x > m_vMax.z - m_vMin.z ) ? m_vMax.x - m_vMin.x : m_vMax.z - m_vMin.z; }
	FLOAT GetMaxHeight() { return m_vMax.y - m_vMin.y; }
};

#endif



