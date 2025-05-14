#include "stdafx.h"
#include "TailEffectMng.h"


#ifdef __CLIENT


const DWORD TAILVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;


CTailModelMng::CTailModelMng()
{
	m_ListTailModel.clear();
}
CTailModelMng::~CTailModelMng()
{
}





CTailEffectBelt::CTailEffectBelt()
{
	Init();
}

CTailEffectBelt::~CTailEffectBelt()
{
	Destroy();
}

void CTailEffectBelt::Init( void )
{
	m_bActive	= FALSE;
	m_pPool		= NULL;
	m_nPoolPtr	= 0;
	m_pTexture = NULL;
	m_pVB		= NULL;
	m_pTails	= NULL;
	m_pTailsFree = NULL;
    m_dwBase		= 0;
    m_dwFlush		= 0;
	m_dwDiscard		= 0;
	m_nMaxTail		= 0;
	m_nType			= 0;
	m_fFadeSpeed    = 0.0f;
}


void CTailEffectBelt::Clear( void )
{
	m_bActive	= FALSE;
	m_nPoolPtr	= 0;
	m_pTails	= NULL;
	m_pTailsFree	= NULL;
	m_dwBase = 0;
	m_dwFlush = 0;
	m_dwDiscard = 0;
	m_nMaxTail = 0;
	m_nType = 0;
}

void CTailEffectBelt::Destroy( void )
{
	InvalidateDeviceObjects();
	SAFE_RELEASE( m_pTexture );
	SAFE_DELETE_ARRAY( m_pPool );
	Init();
}

#define		MAX_TAIL	512

void CTailEffectBelt::Create( int nType, FLOAT fFadeSpeed )
{
	if( m_pPool == NULL )
		Init();

	m_bActive = TRUE;
    m_dwBase         = MAX_TAIL;
    m_dwFlush        = 512;
	m_dwDiscard      = MAX_TAIL;
	m_nType			= nType;
	m_fFadeSpeed     = fFadeSpeed;

	if( m_pPool == NULL )
		m_pPool = new TAIL[ MAX_TAIL ];
	memset( m_pPool, 0, sizeof(TAIL) * MAX_TAIL );

}




int	CTailEffectBelt::CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 )
{
	if( m_bActive == FALSE )	return 1;
	if( m_pTexture == NULL )	return 1;
	TAIL *pTail;


	if( m_nMaxTail >= MAX_TAIL )		return 0;

    if( m_pTailsFree )
    {
        pTail = m_pTailsFree;
        m_pTailsFree = pTail->m_pNext;
    }
    else
    {
		pTail = &m_pPool[ m_nPoolPtr++ ];
		if( m_nPoolPtr >= MAX_TAIL )
			m_nPoolPtr = 0;
    }

    pTail->m_pNext = m_pTails;
    m_pTails = pTail;
    m_nMaxTail++;


    pTail->m_vPos1 = vPos1;
    pTail->m_vPos2 = vPos2;		// 2

	pTail->m_clrDiffuse = D3DXCOLOR( 1.0f,   1.0f,   1.0f,   1.0f );
	pTail->m_clrFade    = D3DXCOLOR( 1.0f,   1.0f,  1.0f,  0.0f );
    pTail->m_fFade      = 0.0f;

	return 1;
}




HRESULT CTailEffectBelt::FrameMove( void )
{
	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture == NULL )	return S_OK;

    TAIL *pTail, **ppTail;

    ppTail = &m_pTails;

    while( *ppTail )
    {
        pTail = *ppTail;

        // Calculate new position
//		pTail->m_fFade += 0.030f;
		pTail->m_fFade += m_fFadeSpeed;



        if( pTail->m_fFade > 2.0f )
            pTail->m_fFade = 2.0F;

        // Kill old Tails

        if( pTail->m_fFade >= 2.0f )
        {
            // Kill Tail
            *ppTail = pTail->m_pNext;
            pTail->m_pNext = m_pTailsFree;
            m_pTailsFree = pTail;
			m_nMaxTail--;
        }
        else
        {
            ppTail = &pTail->m_pNext;
        }
    }


    return S_OK;
}



//---------------------------------------------------------------------

//---------------------------------------------------------------------
HRESULT CTailEffectBelt::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;

	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture )			return S_OK;

	// Create the texture using D3DX
	hr = LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, szFileName ),
								D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
								D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
								D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &m_pTexture );
	if( hr == E_FAIL )
	{
		Error( "%s 읽기 실패", MakePath( DIR_MODELTEX, szFileName ) );
		m_bActive = FALSE;
	}

	return hr;
}

HRESULT CTailEffectBelt::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return S_OK;
	if( m_pTexture == NULL )	return S_OK;
	if( m_pVB )
		return S_OK;
    HRESULT hr;

    if(FAILED(hr = pd3dDevice->CreateVertexBuffer( (MAX_TAIL * 2) *
													sizeof(TAILVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
													TAILVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
        return E_FAIL;
	}

    return S_OK;
}

HRESULT CTailEffectBelt::ChangeTexture( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType )
{
	SAFE_RELEASE( m_pTexture );
	m_nType = nType;
	return InitDeviceObjects( pd3dDevice, szFileName );
}

HRESULT CTailEffectBelt::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    return S_OK;
}


HRESULT CTailEffectBelt::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_bActive == FALSE )	return E_FAIL;
	if( m_pTexture == NULL )	return E_FAIL;
	if( m_nMaxTail < 2 )		return E_FAIL;

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &mWorld );		// Set World Transform

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,   D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
//    pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	pd3dDevice->SetTexture(0, m_pTexture );

    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(TAILVERTEX) );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
    pd3dDevice->SetFVF( TAILVERTEX::FVF );

    HRESULT hr;

    TAIL*    pTail = m_pTails;
    TAILVERTEX* pVertices;
    DWORD        dwNumTailsToRender = 0;

	m_dwBase += m_dwFlush;

	if(m_dwBase >= m_dwDiscard)
		m_dwBase = 0;

	if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(TAILVERTEX), m_dwFlush * sizeof(TAILVERTEX),
		(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
    {
		return hr;
	}

	D3DXCOLOR clrDiffuse;
	D3DXVECTOR3	vPos1, vPos2;
	int		nCount = 0;
	float	fTy;
	float	fFade;

    while( pTail )
	{
        vPos1 = pTail->m_vPos1;
		vPos2 = pTail->m_vPos2;

		if( pTail->m_fFade > 1.0f )
			fFade = 2.0f - pTail->m_fFade;
		else
			fFade = pTail->m_fFade;
        D3DXColorLerp( &clrDiffuse, &pTail->m_clrFade, &pTail->m_clrDiffuse, fFade );
        DWORD dwDiffuse = (DWORD) clrDiffuse;

		if( nCount & 1 )	fTy = 1.0f;
		else				fTy = 0.0f;


		pVertices->v     = vPos1;
		pVertices->color = dwDiffuse;
		pVertices->tx	= 0.0f;
		pVertices->ty	= fTy;
		pVertices++;

		pVertices->v     = vPos2;
		pVertices->color = dwDiffuse;
		pVertices->tx	= 1.0f;
		pVertices->ty	= fTy;
		pVertices++;

		if( ++dwNumTailsToRender == m_dwFlush )
		{
			m_pVB->Unlock();

			if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_dwBase, (dwNumTailsToRender * 2) - 2 )))
				return hr;

			m_dwBase += m_dwFlush;

			if(m_dwBase >= m_dwDiscard)
				m_dwBase = 0;

			if( FAILED( hr = m_pVB->Lock( m_dwBase * sizeof(TAILVERTEX), m_dwFlush * sizeof(TAILVERTEX),
				(void**) &pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) ) )
			{
				return hr;
			}

			dwNumTailsToRender = 0;
		}

        pTail = pTail->m_pNext;
		nCount ++;
	}

    m_pVB->Unlock();

#ifdef _DEBUG
	if( dwNumTailsToRender == 1 )
		Error( "TailRender : 렌더할 꼬리개수가 넘 작은경우 발생. %d", dwNumTailsToRender );
#endif

    // Render any remaining particles
    if( dwNumTailsToRender )
    {
		if(FAILED(hr = pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_dwBase, (dwNumTailsToRender * 2) - 2 )))
			return hr;
    }


	pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,  TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTexture(0, NULL );


    return S_OK;
}


CTailEffectMng	g_TailEffectMng;

CTailEffectMng::CTailEffectMng()
{
	Init();
}

CTailEffectMng::~CTailEffectMng()
{
	Destroy();
}

void CTailEffectMng::Init( void )
{
	m_bActive = TRUE;
	m_nMaxType = 0;

	for( int i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		SAFE_DELETE( m_TailEffects[i] );
	}
}

void CTailEffectMng::Destroy( void )
{


	Init();
}

HRESULT CTailEffectMng::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		m_TailEffects[i]->RestoreDeviceObjects( pd3dDevice );
	}

	return S_OK;
}

HRESULT CTailEffectMng::InvalidateDeviceObjects( void )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		m_TailEffects[i]->InvalidateDeviceObjects();
	}

	return S_OK;
}


CTailEffect *CTailEffectMng::AddEffect( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType, FLOAT fFadeSpeed )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] != NULL ) 
			continue;

		m_TailEffects[ i ] = new CTailEffectBelt;
		m_TailEffects[ i ]->Create( nType, fFadeSpeed );
		m_TailEffects[ i ]->InitDeviceObjects( pd3dDevice, szFileName );
		m_TailEffects[ i ]->RestoreDeviceObjects( pd3dDevice );
		m_nMaxType ++;
		return m_TailEffects[ i ];
	}

	return NULL;
}


int		CTailEffectMng::Delete( CTailEffect *pTail )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[i] == pTail )
		{
			m_TailEffects[i]->Destroy();
			SAFE_DELETE( m_TailEffects[i] );
			m_nMaxType --;
			return 1;
		}
	}

	return 0;
}

void CTailEffectMng::Process( void )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		if( m_TailEffects[i]->IsActive() == FALSE )	continue;
		m_TailEffects[i]->FrameMove();
	}
}

void CTailEffectMng::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	int		i;
	for( i = 0; i < MAX_TAILEFFECT; i ++ )
	{
		if( m_TailEffects[ i ] == NULL ) continue;
		if( m_TailEffects[i]->IsActive() == FALSE )	continue;
		m_TailEffects[i]->Render( pd3dDevice );
	}
}


#endif // CLIENT
