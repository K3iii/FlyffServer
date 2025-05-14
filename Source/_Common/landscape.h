#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#ifndef __MAP_SIZE

#define MAP_SIZE (128)
#define NUM_PATCHES_PER_SIDE (16)
#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
#define LIGHTMAP_SIZE ((PATCH_SIZE-1)*NUM_PATCHES_PER_SIDE)

#ifdef __CLIENT
#define LANDREALSCALE ( MAP_SIZE * MPU )
#define MPU g_MPU
#define MPU_INV ( 1.0f / float( MPU ) )
#else
//#define MPU 4												// METER PER UNIT
#define MPU OLD_MPU
#endif

//const float MPU_INV = 1.0f / float(MPU);

#define INIT_HEIGHT		0
#define MAX_LAYER		20

#define D3DFVF_D3DLANDSCAPEVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2)
#define D3DFVF_WATERVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_HGTATTRVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

#define HGT_NOWALK 1000.0f
#define HGT_NOFLY  2000.0f
#define HGT_NOMOVE 3000.0f
#define HGT_DIE    4000.0f

#define HATTR_NONE   0
#define HATTR_NOWALK 1
#define HATTR_NOFLY  2
#define HATTR_NOMOVE 3
#define HATTR_DIE    4

#define WTYPE_NONE  0x00
#define WTYPE_CLOUD 0x01
#define WTYPE_WATER 0x02

struct WATERVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	DWORD color;
	FLOAT u, v;
};

struct HGTATTRVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	DWORD color;
};

struct D3DLANDSCAPEVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT       tu1, tv1;
	FLOAT       tu2, tv2;
};

class CObj;
class CWorld;




struct CLandLayer
{
public:
	BOOL m_bVisible;
	WORD m_nTex;
	BOOL m_aPatchEnable[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	LPDIRECT3DTEXTURE9 m_pLightMap;

	CLandLayer(LPDIRECT3DDEVICE9 pd3dDevice,WORD nTex);
	~CLandLayer();
	BOOL GetPatchEnable(int x, int z) { return m_aPatchEnable[x+z*NUM_PATCHES_PER_SIDE]; }
};



typedef struct
{
	BYTE byWaterHeight;
	BYTE byWaterTexture;

} WATERHEIGHT,* LPWATERHEIGHT;

typedef struct
{
	DWORD					WaterVertexNum;
	LPDIRECT3DVERTEXBUFFER9	pVB;
} WATERVERTEXBUFFER, * LPWATERVB;

#define MASK_WATERFRAME	0xfc



class CLandscape
{
	friend CWorld;

public:
	CLandscape();
	~CLandscape();

protected:
	static FLOAT	m_fCloud_u1, m_fCloud_v1 ;
	static FLOAT	m_fCloud_u2, m_fCloud_v2 ;
#if __VER < 14 // __WATER_EXT
	static FLOAT	m_fWaterFrame;
#endif //__WATER_EXT

	LPDIRECT3DDEVICE9		m_pd3dDevice;
	CWorld*					m_pWorld;
	FLOAT*					m_pHeightMap;
	WATERHEIGHT				m_aWaterHeight[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BYTE					m_aLandAttr[ NUM_PATCHES_PER_SIDE * NUM_PATCHES_PER_SIDE ];
	BOOL					m_bDirty;
	BOOL					m_bVisible;
	D3DXVECTOR3				m_avBounds[8];
	D3DXPLANE				m_aplaneBounds[6];
	int						m_nWorldX,m_nWorldY;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	WATERVERTEXBUFFER		*m_pWaterVB;

	DWORD					m_nCloudVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pCloudVertexBuffer;
	DWORD					m_nWaterVertexNum;
	LPDIRECT3DVERTEXBUFFER9 m_pWaterVertexBuffer;


	FLOAT				GetHeightMap( int nOffset );
	void				OptimizeLayer();
	CLandLayer*			NewLayer( WORD nTex );
	int					GetHeightAttribute( int x, int z );
	LPWATERHEIGHT		GetWaterHeight(int x, int z );
	HRESULT				MakeWaterVertexBuffer();
	HRESULT				MakeLandAttrVertexBuffer();
	HRESULT				MakeHgtAttrVertexBuffer();

public:
	static	int			m_nWidthLinkMap[ MAX_LINKLEVEL ];

	DWORD				m_dwVersion;
	CPtrArray			m_aLayer;
	BOOL				m_abPatchRendered[NUM_PATCHES_PER_SIDE*NUM_PATCHES_PER_SIDE];
	CObj**				m_apObject [ MAX_OBJARRAY ];
	DWORD				m_adwObjNum[ MAX_OBJARRAY ];
	CDWordStack			m_aObjStack[ MAX_OBJARRAY ];
	CTexture			m_texMiniMap;
	CPatch				m_aPatches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];
	CObj**				m_apObjLink[MAX_LINKTYPE][MAX_LINKLEVEL];

	CObj***				GetObjLink( DWORD dwLinkType )	{	return( m_apObjLink[dwLinkType] );	}
	CObj**				GetObjLink( DWORD dwLinkType, DWORD dwLinkLevel )	{	return( m_apObjLink[dwLinkType][dwLinkLevel] );		}
	HRESULT				InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, CWorld* pWorld );
	HRESULT				RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT				InvalidateDeviceObjects();
	HRESULT				DeleteDeviceObjects();
	int					isVisibile( ) { return m_bVisible; }
	void				FreeTerrain();
	void				ResetTerrain( DWORD dwInitHeight ,BYTE* pHeight=NULL);
	void				NewLandscape( DWORD dwTextureId );
	void				SetVertices();
	void				RenderPatches();
	HRESULT				Render( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bLod = TRUE );
	HRESULT				RenderWater( LPDIRECT3DDEVICE9 pd3dDevice );
#if __VER >= 13 // __HOUSING
	BOOL				ForceTexture(LPDIRECT3DTEXTURE9 pNewTex);
#endif	// __HOUSING
	void				CalculateBound();
	void				UpdateCull(void);
	void				Cull();
	FLOAT				GetHeight_Fast( float x, float z );
	FLOAT				GetHeight( FLOAT x, FLOAT z );
	FLOAT				GetHeight( DWORD x, DWORD z );
	FLOAT				GetHeight( POINT pos );
	void				ReadLandscape(FILE* fp);
	BOOL				LoadLandscape( LPCTSTR lpszFileName, int x = 0, int y = 0  );
	void				SetUsedAllObjects();
	void				AddObjArray( CObj* pObj );
	void				RemoveObjArray( CObj* pObj );
	void				RemoveObjArray( int nIndex );
	BOOL				InsertObjLink( CObj* pObj );
	BOOL				RemoveObjLink( CObj* pObj );
	BOOL				SetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel, CObj* pObj );
	CObj*				GetObjInLinkMap( D3DXVECTOR3 vPos, DWORD dwLinkType, int nLinkLevel );
	int					GetObjArraySize();
};

void SetLODDetail(int nDetail);

#else	// not __MAP_SIZE

 #define MAP_SIZE (128)
 #define NUM_PATCHES_PER_SIDE (16)
 #define MPU 4 // METER PER UNIT

#endif	// __MAP_SIZE
#endif	// LANDSCAPE_H