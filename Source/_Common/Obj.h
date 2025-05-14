#if !defined(AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_22)
#define AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_22

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XUtil3D.h"


////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////
const DWORD		OBJ_FLAG_DELETE		  = 0x00000001;
const DWORD     OBJ_FLAG_VISIBLE      = 0x00000002;
const DWORD     OBJ_FLAG_VIRTUAL      = 0x00000004;
const DWORD     OBJ_FLAG_UPDATEMATRIX = 0x00000008;
const DWORD     OBJ_FLAG_POSCHANGED   = 0x00000010;

const DWORD		OBJ_FLAG_VISIBLE_COLLISION = 0x00000020;

#define		OBJID			DWORD
#define		MAX_LINKLEVEL	7


#define		ANILOOP_1PLAY	(0x00000001)
#define		ANILOOP_CONT	(0x00000002)
#define		ANILOOP_LOOP	(0x00000004)
#define		ANILOOP_RETURN	(0x00000008)
#define		ANILOOP_BACK	(0x00000010)

////////////////////////////////////////////////////////////////////////////////////
// enum
////////////////////////////////////////////////////////////////////////////////////

/// Represents the result of the culling calculation on an object.
enum CULLSTATE
{
    CS_UNKNOWN,      // cull state not yet computed
    CS_INSIDE,       // object bounding box is at least partly inside the frustum
    CS_OUTSIDE,      // object bounding box is outside the frustum
    CS_INSIDE_SLOW,  // OBB is inside frustum, but it took extensive testing to determine this
    CS_OUTSIDE_SLOW, // OBB is outside frustum, but it took extensive testing to determine this
};


enum
{
	METHOD_NONE,
	METHOD_EXCLUDE_ITEM
};

////////////////////////////////////////////////////////////////////////////////////
// class CObj
////////////////////////////////////////////////////////////////////////////////////

class		CCamera;
class		CLight;
class		CLandscape;
class		CWorld;
class		CAIInterface;
class		CModel;
struct		ObjProp;
class		CAr;


class CObj
{
	friend			CLandscape;

public:
	CObj();
	virtual ~CObj();

protected:
	DWORD			m_dwFlags;

private:
	DWORD			m_dwLinkLevel;
	D3DXVECTOR3		m_vScale;
	FLOAT			m_fAngle;
	float			m_fAngX;
	float			m_fAngZ;

protected:
	D3DXMATRIX		m_matTrans;
	D3DXMATRIX		m_matScale;
	D3DXMATRIX		m_matRotation;
	D3DXMATRIX		m_matWorld;

public:
	static int		m_nMethod;
	static CObj*	m_pObjHighlight;
	static CObj*	m_pObjActive;
	static BOOL		m_bCollision;
	static BOOL		m_bAnimate;


	BBOX			m_OBB;

	D3DXVECTOR3		m_vPos;
	CWorld*			m_pWorld;
	DWORD			m_dwType;
	DWORD			m_dwIndex;
	CModel*			m_pModel;
	CObj*			m_pPrev;
	CObj*			m_pNext;
	DWORD			m_dwObjAryIdx;

	CAIInterface*	m_pAIInterface;
	DWORD			m_dwAIInterface;		/// AI type
	DWORD			m_dwAIInterfaceState;	/// AI type sub

#ifdef __WORLDSERVER
		DWORD			m_dwPatrolIndex;
		int				m_nPatrolIndexCount;
		BYTE            m_bPatrolCycle: 1;
		BYTE            m_bPatrolReverse: 1;
	D3DXVECTOR3		m_vRemoval;
	D3DXVECTOR3		m_vLink;
	int				m_nAddObjsArrayIndex;
#else	// __WORLDSERVER
	CObj**			m_ppViewPtr;
    D3DXVECTOR3		m_vecBoundsLocal[8];	// bounding box coordinates (in local coord space)
    D3DXVECTOR3		m_vecBoundsWorld[8];	// bounding box coordinates (in world coord space)
    D3DXPLANE		m_planeBoundsWorld[6];	// bounding box planes (in world coord space)
    CULLSTATE		m_cullstate;			// whether object is in the view frustum

	WORD            m_wBlendFactor;
	float			m_dy;
	short			m_nEvent;
	short			m_nCnt;
	CCamera*		m_pCamera;
	float			m_fDistCamera;

	#ifdef __YSMOOTH_OBJ
		BOOL			m_bSmooth;
		BOOL			m_bSmoothOld;
		int				m_nSmoothCount;
	#endif //__YSMOOTH_OBJ
#endif	// __WORLDSERVER

	#define	MAX_LINKTYPE	3
	enum	{	linkStatic	= 0, linkDynamic	= 1, linkPlayer	= 2	};

public:
	static BOOL		IsAnimate()						{ return m_bAnimate; }
	static void		AnimateEnable( BOOL bAnimate )	{ m_bAnimate = bAnimate; }
	static int		GetMethod()						{ return m_nMethod;	}
	static void		SetMethod( int nMethod )		{ m_nMethod	= nMethod;	}

	virtual	BOOL	SetMotion(DWORD dwMotion, int nLoop = ANILOOP_LOOP, DWORD dwOption = 0 ) { return FALSE; }
	virtual	BOOL	InitMotion(DWORD dwMotion) { return FALSE; }
	virtual	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex );
	virtual void	InitProp( BOOL bInitAI = TRUE );
	virtual	LPCTSTR GetName() { return _T( "hello" ); }
	virtual void	Process();
	virtual BOOL	Read( CFileIO* pFile );
	virtual void	Serialize( CAr & ar );
	virtual void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
	virtual void	RenderName( LPDIRECT3DDEVICE9 pd3dDevice, CD3DFont* pFont, DWORD dwColor = 0xffffffff );
	virtual	BOOL	SetIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwIndex, BOOL bInitProp = FALSE ) { return SetTypeIndex( pd3dDevice, m_dwType, dwIndex, bInitProp ); }

	BOOL			IsUpdateMatrix() { 	return (m_dwFlags & OBJ_FLAG_UPDATEMATRIX); }
	void			SetUpdateMatrix( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_UPDATEMATRIX ); }
	BOOL			IsVisible() { return (m_dwFlags & OBJ_FLAG_VISIBLE);  }
	void			SetVisible( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_VISIBLE ); }
	BOOL			IsVirtual() { return (m_dwFlags & OBJ_FLAG_VIRTUAL);  }
	void			SetVirtual( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_VIRTUAL ); }
	BOOL			IsDelete()	{ return (m_dwFlags & OBJ_FLAG_DELETE);  }
	void			SetDelete( BOOL bUpdate ) { SetFlag( bUpdate, OBJ_FLAG_DELETE ); }
	BOOL			IsPosChanged()  { return (m_dwFlags & OBJ_FLAG_POSCHANGED); }
	void			SetPosChanged( BOOL bUpdate) { SetFlag( bUpdate, OBJ_FLAG_POSCHANGED ); }

	void			AddAngleX( FLOAT fAngle );
	FLOAT			GetAngleX() { return m_fAngX; }
	void			SetAngleX( FLOAT fAngX );

	void			UpdateLocalMatrix();
	void			UpdateMatrix();
	BOOL			IsRangeObj( CObj *pSrc, float fRange );
	BOOL			IsRangeObj( const D3DXVECTOR3 &vDest, float fRange );

	CObj* GetNextNode() { return m_pNext; }
	void			InsNextNode(CObj* pBkgr);
	void			DelNode();

	DWORD			GetLinkLevel() { return m_dwLinkLevel; }
	void			ResetScale();
	BOOL			UpdateBoundBox();
	void			SetOnLand();

	void			SetAIInterface( DWORD dwAIInterface, DWORD dwState = 2 );
	ObjProp*		GetProp();
	DWORD			GetType() { return m_dwType; }
	DWORD			GetIndex() { return m_dwIndex; }
	BOOL			SetTypeIndex( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwType, DWORD dwIndex, BOOL bInitProp = FALSE );
	void			ProcessAI();
	DWORD			GetLinkType();
	void			Delete();

	void			SetPos( const D3DXVECTOR3& vPos );
	D3DXVECTOR3		GetPos()	{ return m_vPos; }
	void			SetAngle( FLOAT fAngle );
	FLOAT			GetAngle()	{ return m_fAngle; }
	void			AddAngle( FLOAT fAngle );
	void			SetScale( const D3DXVECTOR3& vScale );
	D3DXVECTOR3		GetScale()	{ return m_vScale; }
	CWorld*			GetWorld()	{ return m_pWorld; }
	void			SetWorld( CWorld* pWorld );
	CModel*			GetModel()	{ return m_pModel; }
	BOOL			Pick( D3DXVECTOR3* pPickRayOrg, D3DXVECTOR3* pPickRayDir, D3DXVECTOR3* pvIntersect,
		                  FLOAT* pfDist, BOOL bBoundBox = FALSE, BOOL bColl = FALSE );
	BOOL			IsDynamicObj()			{ return m_dwType != OT_OBJ && m_dwType != OT_SFX; }

	D3DXMATRIX		GetMatrixTrans() const		{ return m_matTrans; }
	D3DXMATRIX		GetMatrixScale() const		{ return m_matScale; }
	D3DXMATRIX		GetMatrixRotation()	const	{ return m_matRotation; }
	D3DXMATRIX		GetMatrixWorld() const		{ return m_matWorld; }
	D3DXMATRIX*		GetMatrixWorldPtr()		    { return &m_matWorld; }

	void			SetMatrixTrans(const D3DXMATRIX& matTrans) { m_matTrans = matTrans; }
	void			SetMatrixRotation(const D3DXMATRIX& matRotation) { m_matRotation = matRotation; }

	FLOAT			GetRadius();
	FLOAT			GetRadiusXZ();

#ifdef __WORLDSERVER
	D3DXVECTOR3		GetLinkPos()							{	return m_vLink;	}
	D3DXVECTOR3		GetRemovalPos()							{	return m_vRemoval;	}
	void			SetLinkPos( const D3DXVECTOR3 & vPos )		{	m_vLink		= vPos;	}
	void			SetRemovalPos( const D3DXVECTOR3 & vPos )	{	m_vRemoval	= vPos;	}
#ifdef __LAYER_1015
private:
	int		m_nLayer;
public:
	void	SetLayer( int nLayer )	{	m_nLayer	= nLayer;	}
	int		GetLayer()	{	return m_nLayer;	}
#endif	// __LAYER_1015
#else	// __WORLDSERVER
	D3DXVECTOR3		GetLinkPos()					{	return m_vPos;	}

	static void		SetActiveObj( CObj* pObj )		{ m_pObjActive = pObj; }
	static CObj*	GetActiveObj()					{ return m_pObjActive; }
	BOOL			IsActiveObj()					{ return m_pObjActive == this; }

	BOOL			IsCull() { return m_cullstate == CS_OUTSIDE; }
	void			SetCullState( CULLSTATE cullState ) { m_cullstate = cullState; }

	virtual D3DXVECTOR3		GetScrPos() { return GetPos(); }
#endif	// !__WORLDSERVER

private:
	void			CalcLinkLevel();
	void			SetFlag( BOOL bUpdate, DWORD dwValue );
};

inline void CObj::SetFlag( BOOL bUpdate, DWORD dwValue )
{
	if( bUpdate )
		m_dwFlags |= dwValue;
	else
		m_dwFlags &= (~dwValue);
}

inline D3DXVECTOR3 VelocityToVec( FLOAT fAngle, FLOAT fVelocity )
{
	FLOAT fTheta = D3DXToRadian( fAngle );
	FLOAT fx = sinf( fTheta ) * fVelocity;
	FLOAT fz = -cosf( fTheta ) * fVelocity;
	return D3DXVECTOR3( fx, 0.0f, fz );
}


//inline FLOAT GetDegree( D3DXVECTOR3 vDestPos, D3DXVECTOR3 vSrcPos )
inline FLOAT GetDegree( const D3DXVECTOR3& vDestPos, const D3DXVECTOR3& vSrcPos )
{
	D3DXVECTOR3 vDir1 = VelocityToVec(0.0f, 1.0f);
	D3DXVECTOR3 vDir2 = vDestPos - vSrcPos;
	vDir2.y = 0.0f;

	D3DXVec3Normalize(&vDir1, &vDir1);
	D3DXVec3Normalize(&vDir2, &vDir2);
	FLOAT fDot = D3DXVec3Dot(&vDir1, &vDir2);
	FLOAT fRadian = acosf(fDot);
	FLOAT fDegree = D3DXToDegree(fRadian);
	if (vDir2.x < 0.0f)
		fDegree = 360.0f - fDegree;

	return fDegree;
}

inline FLOAT GetDegreeX( const D3DXVECTOR3 &vDestPos, const D3DXVECTOR3 &vSrcPos )
{
	D3DXVECTOR3 vXZ = vDestPos - vSrcPos;
	float	yDist = vXZ.y;
	vXZ.y = 0.0f;
	float	fLenXZ  = D3DXVec3Length( &vXZ );
	float	zDist = fLenXZ;

	float r = (float)atan2( yDist, -zDist );
	float a = D3DXToDegree( r );
	a = -a;

	if( a > 90.0f )
		a	= 180.0f - a;
	else if( a < -90.0f )
		a	= -180.0f - a;
	return a;
}

inline BOOL IsValidObj( CObj* pObj )
{
	return (pObj && ( pObj->IsDelete() == FALSE ));
}

inline BOOL IsInvalidObj( CObj* pObj )
{
	return !IsValidObj( pObj );
}

inline BOOL IsValidObjID( OBJID id )			//added by gmpbigsun
{
	return ( NULL_ID != id && 0 != id );
}


#endif // !defined(AFX_OBJ_H__2D16F42E_646A_4AEE_9046_504AB9747375__INCLUDED_)
