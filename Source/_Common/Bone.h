#ifndef	__BONE_H__
#define	__BONE_H__

#include <d3dx9.h>
#include "file.h"

#define		MAX_VS_BONE		28

struct BONE;
struct BONE
{
	BONE		*m_pParent;


	int			m_nParentIdx;
	char		m_szName[32];
	D3DXMATRIX		m_mTM;
	D3DXMATRIX		m_mInverseTM;
	D3DXMATRIX		m_mLocalTM;
};

#define MAX_MDL_EVENT		8	// #event xx



class CBones
{
public:
	int		m_nID;
	char	m_szName[64];
	int		m_nMaxBone;
	BONE	*m_pBones;
	int		m_bSendVS;

	D3DXMATRIX	m_mLocalRH;
	D3DXMATRIX	m_mLocalLH;
	D3DXMATRIX	m_mLocalShield;
	D3DXMATRIX	m_mLocalKnuckle;
	int		m_nRHandIdx;
	int		m_nLHandIdx;
	int		m_nRArmIdx;
	int		m_nLArmIdx;
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];

	int		m_nEventParentIdx[MAX_MDL_EVENT];

private:
	void Init( void );
	void Destroy( void );

public:
	CBones();
	~CBones();


	BONE	*GetBone( int nIdx ) { return &m_pBones[ nIdx ]; }

	int		LoadBone( LPCTSTR szFileName );
};


#if __VER < 12 // __LEAK_0827
#define		MAX_BONES			128
#endif	// __LEAK_0827

// read only bone database
class CBonesMng
{
public:
#if __VER >= 12 // __LEAK_0827
	map<string, CBones*>	m_mapBones;
#else	// __LEAK_0827
	int			m_nSize;
	int			m_nMax;
	CBones *m_pBonesAry[ MAX_BONES ];
#endif	// __LEAK_0827

//#ifdef __WORLDSERVER
//	CRIT_SEC	m_AccessLock;
//#endif	// __WORLDSERVER

	CBonesMng();
	~CBonesMng();

	void	Init( void );
	void	Destroy( void );

	CBones *LoadBone( LPCTSTR szFileName );

};

extern CBonesMng		g_BonesMng;


////////////////////////////////////////////////////////////////////////////////////////////////
//////////
//////////
//////////
//////////
//////////
//////////
//////////
//////////
////////////////////////////////////////////////////////////////////////////////////////////////
#define		MA_HIT			(0x00000001)
#define		MA_SOUND		(0x00000002)
#define		MA_QUAKE		(0x00000004)
struct TM_ANIMATION
{
//	D3DXMATRIX			m_mAniTM;
	D3DXQUATERNION		m_qRot;
	D3DXVECTOR3			m_vPos;
};



struct	BONE_FRAME
{
	TM_ANIMATION	*m_pFrame;
	D3DXMATRIX		m_mLocalTM;
};


struct	MOTION_ATTR
{
	DWORD	m_dwAttr;
//	char	m_szFileName[32];
	int		m_nSndID;
	float	m_fFrame;
};

class CMotion
{
private:
	TM_ANIMATION	*m_pMotion;
	MOTION_ATTR		*m_pAttr;
	int		m_nMaxFrame;
public:
	int		m_nID;
	char	m_szName[64];
	float	m_fPerSlerp;
	int		m_nMaxBone;
	D3DXVECTOR3	m_vPivot;
	BONE_FRAME	*m_pBoneFrame;
	BONE		*m_pBoneInfo;
	int		m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[4];
	D3DXVECTOR3		*m_pPath;			// path

	CMotion();
	~CMotion();

	void Init( void );
	void Destroy( void );

	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }
	DWORD	GetAttr( int nNumFrm )
	{
		return m_pAttr[ nNumFrm ].m_dwAttr;
	}
//#if 0
	DWORD	IsAttrHit( float fOldFrm, float fNumFrm )
	{
#ifdef __ATTR_HIT_FIX
		int old = int(fOldFrm) + 1;
		while (old <= static_cast<int>(fNumFrm))
		{
			MOTION_ATTR* pAttr = &m_pAttr[old];
			if (pAttr->m_dwAttr & MA_HIT)
			{
				if (old <= pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm)
				{
					return pAttr->m_dwAttr;
				}
			}
			++old;
		}
#endif

		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )
				return pAttr->m_dwAttr;
			return 0;
	}
	void	SetAttr( float fNumFrm, DWORD dwAttr )
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	TM_ANIMATION*	GetFrame( int nNumFrm ) { return &m_pMotion[nNumFrm]; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }

	int		LoadMotion( LPCTSTR strFileName );
	void	ReadTM( CResFile *file, int nNumBone, int nNumFrame );

	void	AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight );

};

#if __VER < 12 // __LEAK_0827
#define		MAX_MOTION					2048
#define		MAX_MOTION_CACHE		16
#endif	// __LEAK_0827

// read only motion database
class CMotionMng
{
public:
#if __VER >= 12 // __LEAK_0827
	map<string, CMotion*>	m_mapMotions;
#else	// __LEAK_0827
	int		m_nSize;
	int		m_nMax;
	int		m_nCachePos;
	CMotion		*m_pCache[ MAX_MOTION_CACHE ];
	CMotion		*m_pMotionAry[ MAX_MOTION ];
#endif	// __LEAK_0827

	CMotionMng();
	~CMotionMng();

	void	Init( void );
	void	Destroy( void );

	CMotion	*LoadMotion( LPCTSTR strFileName );
};

extern CMotionMng		g_MotionMng;




#endif
