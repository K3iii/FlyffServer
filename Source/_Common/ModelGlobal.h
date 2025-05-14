#ifndef		__MODELGLOBAL_H__
#define		__MODELGLOBAL_H__

#define XE_NONE				0
#define XE_REFLECT			(0x00000001)
#define XE_OPACITY			(0x00000002)
#define XE_2SIDE			(0x00000004)
#define XE_SELF_ILLUMINATE	(0x00000008)
#define XE_SPECULAR			(0x00000010)
#define XE_BUMP				(0x00000020)
#define XE_HIDE				(0x00000040)

#define XE_ITEM_FIRE		(0x00000080)
#define XE_ITEM_ELEC		(0x00000100)
#define XE_ITEM_WATER		(0x00000200)
#define XE_ITEM_WIND		(0x00000400)
#define XE_ITEM_EARTH		(0x00000800)
#define XE_NOBUMP			(0x00001000)
#define XE_HIGHLIGHT_OBJ	(0x00002000)
#define XE_ITEM_GEN			(0x00004000)

#define XE_ITEM_FIRE_AL		(0x00008000)
#define XE_ITEM_ELEC_AL		(0x00010000)
#define XE_ITEM_WATER_AL	(0x00020000)
#define XE_ITEM_WIND_AL		(0x00040000)
#define XE_ITEM_EARTH_AL	(0x00080000)
#define XE_ITEM_GEN_AL		(0x00100000)


extern double		g_dUps;
extern double		g_dKps;
extern DWORD	g_tmCurrent;

void	InitUPS( void );
void	CalcUPS( void );


void	SetDiffuse( float r, float g, float b );
void	SetAmbient( float r, float g, float b );
void	SetLightVec( const D3DVECTOR &vLight );
void	SetLightPos( const D3DVECTOR &vLight );
void	SetTransformView( const D3DXMATRIX &mView );
void	SetTransformProj( const D3DXMATRIX &mProj );
void	SetLight( BOOL bLight );
void	SetNight( BOOL bNight );
void	SetFog( BOOL bFog );

// shadow
#define		SHADOWMAP_WIDTH		2048
#define		SHADOWMAP_HEIGHT	2048

#define		MAX_ELETEXTURE		18

extern LPD3DXRENDERTOSURFACE   g_pRenderToSurface;
extern LPDIRECT3DSURFACE9      g_pShadowSurface;
extern LPDIRECT3DTEXTURE9      g_pShadowTexture;

extern D3DXMATRIX g_mViewLight;
extern D3DXMATRIX g_mShadowProj;


BOOL CreateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat );
void DeleteShadowMap( LPDIRECT3DDEVICE9 pd3dDevice );
void RenderShadowMapInfo( LPDIRECT3DDEVICE9 pd3dDevice );
void SetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage, const D3DXMATRIX &mView );
void ResetStateShadowMap( LPDIRECT3DDEVICE9 pd3dDevice, int nShadowStage );

class CModelGlobal
{

public:
	D3DXVECTOR3		m_vCameraPos, m_vCameraDest, m_vCameraForward;
	LPDIRECT3DTEXTURE9	m_pElecTexture[MAX_ELETEXTURE];
	BOOL	m_bDepthBias;

	CModelGlobal()
	{
		memset( m_pElecTexture, 0, sizeof(m_pElecTexture) );
		m_bDepthBias = FALSE;
	}
	~CModelGlobal() {}

	void	SetCamera( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDest );

	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};

extern CModelGlobal g_ModelGlobal;

extern int		g_nRenderCnt;
extern int		g_nProcessCnt;


#define		PE_FIRE		1
#define		PE_ELEC		2
#define		PE_LASER	3
#define		PE_WATER	4
#define		PE_WIND		5
#define		PE_EARTH	6

#if __VER >= 9 // __Y_ADV_ENCHANT_EFFECT
#define		PE_FIRE_AL	7
#define		PE_ELEC_AL	8
#define		PE_LASER_AL	9
#define		PE_WATER_AL	10
#define		PE_WIND_AL	11
#define		PE_EARTH_AL	12
#define		PE_GEN		13
#define		PE_GEN_AL	14
#endif //__Y_ADV_ENCHANT_EFFECT

class CPartsEffect
{
public:
	int		m_nType;
	DWORD m_dwItemKind3;

	CPartsEffect() { m_nType = 0; m_dwItemKind3 = 0; }
	virtual	~CPartsEffect() {}

	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { return S_OK; }
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) { return S_OK; }

};

#ifdef __CLIENT
#define MAX_PARTS_FIRE		28
#ifdef __CSC_ENCHANT_EFFECT_2
#define MAX_PARTS_FIRE_2	16
#endif //__CSC_ENCHANT_EFFECT_2

class CSfxModel;

class CPartsFire : public CPartsEffect
{
	D3DXVECTOR3	m_vList[ MAX_PARTS_FIRE ];
	CSfxModel *m_pList[ MAX_PARTS_FIRE ];
#ifdef __CSC_ENCHANT_EFFECT_2
	D3DXVECTOR3	m_vList_2[ MAX_PARTS_FIRE_2 ];
	CSfxModel *m_pList_2[ MAX_PARTS_FIRE_2 ];
#endif //
public:
	CPartsFire()
	{
		m_nType = PE_FIRE;
		memset( m_pList, 0, sizeof(m_pList) );
#ifdef __CSC_ENCHANT_EFFECT_2
		memset( m_pList_2, 0, sizeof(m_pList_2) );
#endif //__CSC_ENCHANT_EFFECT_2
	}
	CPartsFire( int nType )
	{
		m_nType = nType;
		memset( m_pList, 0, sizeof(m_pList) );
#ifdef __CSC_ENCHANT_EFFECT_2
		memset( m_pList_2, 0, sizeof(m_pList_2) );
#endif //__CSC_ENCHANT_EFFECT_2
	}
	virtual	~CPartsFire();
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale );
#ifdef __CSC_ENCHANT_EFFECT_2
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 vPos, DWORD dwSfx );
#endif //__CSC_ENCHANT_EFFECT_2
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
};

#if __VER >= 10 // __Y_DRAGON_FIRE

#define MAX_PARTS_FIRE_DRAGON		256
struct PARTICLE_DRAGON
{
    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity

    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression
	FLOAT		m_fGroundY;
	FLOAT       m_fScale;
	int			m_nAni;
};

struct FIREVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    D3DXVECTOR2				    m_v2Tex;

    enum {
        FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    };
};

#define BASE_VERTEX		6

class CPartsFireDragon : public CPartsEffect
{
	LPDIRECT3DTEXTURE9	m_pTexture;
	FIREVERTEX			m_vFan[BASE_VERTEX];
	LPDIRECT3DVERTEXBUFFER9 m_pParticleVB;
	PARTICLE_DRAGON*	m_pList[ MAX_PARTS_FIRE_DRAGON ];

public:
	CPartsFireDragon();
	~CPartsFireDragon() {}
	void	Create( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3 &vPos, DWORD dwSfx, const D3DXVECTOR3 &vScale, const D3DXVECTOR3 &vVel );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld );
	void	Process( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
};

#endif //__Y_DRAGON_FIRE

///////////////////////////////////////////////////////////////////////////////
struct FVF_BEAM
{
    D3DXVECTOR3 pos;	// The 3D position for the vertex
	FLOAT	tu, tv;
};
#define	D3DFVF_BEAM			(D3DFVF_XYZ | D3DFVF_TEX1)

#define MAX_BEAM	32

class CPartsBeam : public CPartsEffect
{
	BOOL	m_bActive;
	D3DXVECTOR3 m_vBeam[MAX_BEAM * 2];
	D3DXVECTOR3 m_vCenter[MAX_BEAM];
//	float		m_fScale[MAX_BEAM];
	float		m_fSize[MAX_BEAM];
	FVF_BEAM m_vList[MAX_BEAM * 2 * 2];
	int			m_nLevel[ MAX_BEAM ];

public:
	int		m_nMax;
	D3DXVECTOR3 m_vLast;

	CPartsBeam()
	{
		m_nType = PE_ELEC;
		m_bActive = FALSE; m_nMax = 0;
		memset( m_vBeam, 0, sizeof(m_vBeam) );
		memset( m_vList, 0, sizeof(m_vList) );
		memset( m_vCenter, 0, sizeof(m_vCenter) );
		memset( m_nLevel, 0, sizeof(m_nType) );
	}
	~CPartsBeam() {};

	void	Create( const D3DXVECTOR3 &v );
	void	Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize = 1.0f, int nLevel = 0 );
	void	Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice,
					const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward,
					const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, int nLevel );
};




class CPartsLaser : public CPartsEffect
{
	D3DXVECTOR3 m_vBeam[2];

	float		m_fSize;
	FVF_BEAM m_vList[2 * 2];
	int		m_nCount;
	FLOAT	m_fRadian;
	LPDIRECT3DTEXTURE9	m_pTexture;

public:
	BOOL	m_bActive;

	CPartsLaser()
	{
		m_nType = PE_LASER;
		m_bActive = FALSE;
		m_nCount = 0;
		m_fRadian = 0;
		m_pTexture = NULL;
		memset( m_vBeam, 0, sizeof(m_vBeam) );
		memset( m_vList, 0, sizeof(m_vList) );
//		memset( m_vCenter, 0, sizeof(m_vCenter) );
	}
	~CPartsLaser() {}

	HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );

	void	Create( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize = 1.0f );
	void	SetPos( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fSize );
//	void	Generate( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2 );
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld, const D3DXVECTOR3 &vEye, const D3DXVECTOR3 &vForward/*, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, FLOAT fSize = 0*/ );
};

#endif // __CLIENT




class CTextureSurface
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPD3DXRENDERTOSURFACE   m_pRenderToSurface;
	LPDIRECT3DSURFACE9      m_pSurface;
	LPDIRECT3DVERTEXBUFFER9		m_pd3dVB;

	void Init();
	void Destroy();

public:
	LPDIRECT3DTEXTURE9      m_pTexture;
	int		m_nWidth, m_nHeight;
	BOOL	bTest;

	CTextureSurface();
	~CTextureSurface();

	HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight, BOOL bDepth = FALSE );
	void DeleteDeviceObjects( void );

	void BeginScene( void );
	void EndScene( void );

	void SetTexture( void );

	void	RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst = NULL, BOOL bBlend = FALSE );
	void	DownSampling( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst, float fOffsetU, float fOffsetV );
	void	RenderTargetBlur( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );
	void	RenderTargetBlurH( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );
	void	RenderTargetBlurV( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pDst );

};





class CGlareLevel
{
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	int		m_nWidth, m_nHeight;

	CTextureSurface		m_Surface[5];
	CTextureSurface		m_Black;

	void Init();
	void Destroy();
public:
	BOOL	m_bActive;
	CTextureSurface		m_Src;

	CGlareLevel();
	~CGlareLevel();

	void Create( LPDIRECT3DDEVICE9 pd3dDevice, D3DFORMAT backBufferFormat, int nWidth, int nHeight );
	void DeleteDeviceObjects( void );
	void SetState( LPDIRECT3DDEVICE9 pd3dDevice );
	void ResetState( LPDIRECT3DDEVICE9 pd3dDevice );
	void RenderWorld( LPDIRECT3DDEVICE9 pd3dDevice, CObj **pList, int nMax );
//	void DownSampling( LPDIRECT3DDEVICE9 pd3dDevice, CTextureSurface *pSrc, float fOffsetU, float fOffsetV );
	void RenderGlareEffect( LPDIRECT3DDEVICE9 pd3dDevice );
	void Blur( LPDIRECT3DDEVICE9 pd3dDevice );

};

extern CGlareLevel		g_Glare;

#ifdef __CLIENT
class CPartsWeaponEffect : public CPartsEffect
{
	D3DXVECTOR3 m_vEffect;
	CSfxModel* m_pEffect;

public:
	CPartsWeaponEffect();
	CPartsWeaponEffect(int nType, DWORD dwKind3)
	{
		m_nType = nType;
		m_dwItemKind3 = dwKind3;
		m_pEffect = NULL;
	}
	virtual	~CPartsWeaponEffect();

	void	Create(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXVECTOR3& vPos, DWORD dwSfx);
	void	Render(LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* mWorld);
};
#endif // __CLIENT
#endif

