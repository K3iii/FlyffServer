#ifndef __SFXBASE_H
#define __SFXBASE_H

class CModel;

#define DEGREETORADIAN(x) ((2.0f*D3DX_PI*(x))/(360.0f))

class CSfxTexture;
class CSfxMeshMng;
class CSfxMng;

extern CSfxMng g_SfxMng;

extern CSfxTexture g_SfxTex;
extern CSfxMeshMng g_SfxMeshMng;

enum SFXPARTTYPE {
	SFXPARTTYPE_BILL=1,
	SFXPARTTYPE_PARTICLE=2,
	SFXPARTTYPE_MESH=3,
	SFXPARTTYPE_CUSTOMMESH=4,
};
enum SFXPARTBILLTYPE {
	SFXPARTBILLTYPE_BILL=1,
	SFXPARTBILLTYPE_BOTTOM=2,
	SFXPARTBILLTYPE_POLE=3,
	SFXPARTBILLTYPE_NORMAL=4,
};
enum SFXPARTALPHATYPE {
	SFXPARTALPHATYPE_BLEND=1,
	SFXPARTALPHATYPE_GLOW=2,
};



#define D3DFVF_D3DSFXVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)
#ifndef DIR_SFX
	#define DIR_SFX     _T( "SFX\\"  )
#endif

struct D3DSFXVERTEX
{
	D3DXVECTOR3 p;
	FLOAT       tu1, tv1;
};

struct SfxKeyFrame
{
	WORD nFrame;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vPosRotate;
	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vRotate;
	int nAlpha;
};

struct Particle
{
	WORD nFrame;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSpeed;
	D3DXVECTOR3 vScale;
//#ifdef __ATEST
	D3DXVECTOR3 vRotation;
	D3DXVECTOR3 vScaleStart;
	D3DXVECTOR3 vScaleEnd;
	D3DXVECTOR3 vScaleSpeed;
	BOOL        bSwScal;
//#endif
};

class CSfxPart
{
public:
//#ifdef __ATEST
	CString m_strName;
//#endif
	CString m_strTex;
	SFXPARTTYPE m_nType;
	SFXPARTBILLTYPE m_nBillType;
	SFXPARTALPHATYPE m_nAlphaType;
	WORD m_nTexFrame;
	WORD m_nTexLoop;
	BOOL m_bUseing;

	CPtrArray m_apKeyFrames;
	SfxKeyFrame* Key(BYTE nIndex) {
		if(nIndex>=m_apKeyFrames.GetSize()) return NULL;
		return (SfxKeyFrame*)(m_apKeyFrames[nIndex]);
	}
	SfxKeyFrame* KeyByFrame(WORD nFrame) {
		SfxKeyFrame* pKey;
		for(int i=0;i<m_apKeyFrames.GetSize();i++) {
			pKey=Key((BYTE)i);
			if(pKey->nFrame==nFrame) return pKey;
		}
		return NULL;
	}

	CSfxPart();
	virtual ~CSfxPart();
#ifndef __WORLDSERVER
	virtual void Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
#endif
	virtual void Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );

	void DeleteAllKeyFrame();
	void AddKeyFrame(WORD nFrame);
	void DeleteKeyFrame(WORD nFrame);
	SfxKeyFrame* GetPrevKey(WORD nFrame);
	SfxKeyFrame* GetNextKey(WORD nFrame, BOOL bSkip=TRUE);
	virtual void Load(CResFile& file);
	virtual void OldLoad(CResFile& file);
	virtual void Load2(CResFile& file);
	virtual void Load3(CResFile& file) {};

	void GetKey(WORD nFrame,SfxKeyFrame* pKey);

	void AdjustKeyFrame(WORD nFrame, SfxKeyFrame& key);
};

class CSfxPartBill : public CSfxPart
{
public:
	CSfxPartBill();
	~CSfxPartBill() {}
	virtual void Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
#ifndef __WORLDSERVER
	virtual void Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
#endif
	virtual void Load(CResFile& file);
	virtual void Load2(CResFile& file);
	virtual void OldLoad(CResFile& file);
};

class CSfxPartParticle: public CSfxPart
{
public:
	WORD m_nParticleCreate;
	WORD m_nParticleCreateNum;


	WORD m_nParticleFrameAppear;
	WORD m_nParticleFrameKeep;
	WORD m_nParticleFrameDisappear;


	FLOAT m_fParticleStartPosVar;
	FLOAT m_fParticleStartPosVarY;
	FLOAT m_fParticleYLow;
	FLOAT m_fParticleYHigh;
	FLOAT m_fParticleXZLow;
	FLOAT m_fParticleXZHigh;

	D3DXVECTOR3 m_vParticleAccel;

	D3DXVECTOR3 m_vScale;
	D3DXVECTOR3 m_vScaleSpeed;
	D3DXVECTOR3 m_vRotation;
	D3DXVECTOR3 m_vRotationLow;
	D3DXVECTOR3 m_vRotationHigh;

//#ifdef __ATEST
	BOOL		m_bRepeatScal;
	FLOAT		m_fScalSpeedXLow;
	FLOAT		m_fScalSpeedXHigh;
	FLOAT		m_fScalSpeedYLow;
	FLOAT		m_fScalSpeedYHigh;
	FLOAT		m_fScalSpeedZLow;
	FLOAT		m_fScalSpeedZHigh;
	D3DXVECTOR3 m_vScaleSpeed2;
	D3DXVECTOR3 m_vScaleEnd;
//#endif


	BOOL        m_bRepeat;


	CSfxPartParticle();
	~CSfxPartParticle();

#ifndef __WORLDSERVER
	virtual void Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
#endif
	virtual void Load(CResFile& file);
	virtual void Load2(CResFile& file);
	virtual void Load3(CResFile& file);
	virtual void OldLoad(CResFile& file);
};
class CSfxPartMesh: public CSfxPart
{
public:
	CSfxPartMesh();
	~CSfxPartMesh();
#ifndef __WORLDSERVER
	virtual void Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
#endif
	virtual void Load(CResFile& file);
	virtual void Load2(CResFile& file);
	virtual void Load3(CResFile& file) {};
	virtual void OldLoad(CResFile& file);
};
class CSfxPartCustomMesh: public CSfxPart
{
public:
	int m_nPoints;
	CSfxPartCustomMesh();
	~CSfxPartCustomMesh();
#ifndef __WORLDSERVER
	virtual void Render( D3DXVECTOR3 vPos, WORD nFrame, FLOAT fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
#endif
	virtual void Render2( D3DXVECTOR3 vPos, WORD nFrame, D3DXVECTOR3 fAngle, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f )  );
	virtual void Load(CResFile& file);
	virtual void Load2(CResFile& file);
	virtual void Load3(CResFile& file) {};
	virtual void OldLoad(CResFile& file);
};


class CSfxBase
{
public:
	BOOL    LoadMerge();
	CString m_strName;

	CPtrArray m_apParts;
	CSfxPart* Part(BYTE nIndex) {
		if(m_apParts.GetSize()>nIndex) return (CSfxPart*)(m_apParts[nIndex]);
		return NULL;
	}

	CSfxBase();
	~CSfxBase();

	CSfxPart* AddPart(SFXPARTTYPE nType);
	void DeletePart(BYTE nIndex);
	void AdjustPart(BYTE nIndex);

	BOOL Load(void);
};


class CSfxMng
{
public:
	static LPDIRECT3DDEVICE9 m_pd3dDevice;
	static LPDIRECT3DVERTEXBUFFER9 m_pSfxVB;

	FLOAT m_fScale;

	CPtrArray m_apSfxBase;

	CSfxMng() {	m_fScale=0.5f; }
	~CSfxMng();

	void AddSfxBase(CSfxBase* pSfxBase);
	void DeleteSfxBase(BYTE nIndex);
	CSfxBase* GetSfxBase(BYTE nIndex);
	CSfxBase* GetSfxBase(CString strSfxName);

	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};


class CSfxModel : public CModel
{
public:

	WORD m_nSfxBase;
	CSfxBase* m_pSfxBase;

	D3DXMATRIX  m_matScale;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vRotate;
	D3DXVECTOR3 m_vScale;
	WORD m_nCurFrame;

	CPtrArray m_apParticles;

	CSfxModel();
	~CSfxModel();
	void DeleteAll(void);
	void RemovePart( int nIndex );
	void SetSfx(CSfxBase* pSfxBase);
	void SetSfx(DWORD dwIndex);
	void SetSfx(LPCTSTR strSfxName);
#ifndef __WORLDSERVER
	virtual BOOL Render( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
	BOOL RenderZ( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
	void RenderParticles( D3DXVECTOR3 vPos,WORD nFrame,FLOAT fAngle,CSfxPartParticle* pPartParticle,CPtrArray* pParticles, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
#endif
	virtual BOOL Render2( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX* pmWorld = NULL );
	void RenderParticles2( D3DXVECTOR3 vPos,WORD nFrame,D3DXVECTOR3 fAngle,CSfxPartParticle* pPartParticle,CPtrArray* pParticles, D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	//void Render(void);
	BOOL Process(void);
	BOOL SetFrame( int nFrame );
#ifdef __CLIENT
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxModel>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxModel::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxModel::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxModel::m_pPool->Free( (CSfxModel*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxModel::m_pPool->Free( (CSfxModel*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
#endif	// __CLIENT
};


class CSfxTexture
{
public:
	CMapStringToPtr m_apTexture;

	CSfxTexture();
	~CSfxTexture();

	LPDIRECT3DTEXTURE9 AddTex(CString str);
	void DeleteTex(CString str);
	LPDIRECT3DTEXTURE9 Tex(CString str);
	void DeleteAll(void);
};

class CSfxMeshMng
{
public:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	CMapStringToPtr m_apMesh;

	CSfxMeshMng();
	~CSfxMeshMng();

	CModelObject* AddMesh(CString str);
	void DeleteMesh(CString str);
	CModelObject* Mesh(CString str);
	void DeleteAll(void);

	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};
#endif
