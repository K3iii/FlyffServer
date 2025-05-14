// Neuz.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "2DRender.h"
#include "targa.h"



///////////////////////////////////////////////////////////////////////
// Class Name : CLipRect : public CRect
// Remarks    :




//////////////////////////////////////////////////////////////////////
// Func. Name : Clipping
// Parameters :
// Return     :
// Remarks    :

BOOL CRectClip::Clipping(CRect& rect) const
{
	if (rect.right <= left) return FALSE;
	if (rect.bottom <= top) return FALSE;
	if (rect.left >= right) return FALSE;
	if (rect.top >= bottom) return FALSE;
	if (rect.left < left) rect.left = left;
	if (rect.top < top) rect.top = top;
	if (rect.right > right) rect.right = right;
	if (rect.bottom > bottom) rect.bottom = bottom;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
// Func. Name : RectLapRect
// Parameters :

// Return     :

// Remarks    :


BOOL CRectClip::RectLapRect(CRect rect) const
{
	if (rect.right <= left) return FALSE;
	if (rect.bottom <= top) return FALSE;
	if (rect.left >= right) return FALSE;
	if (rect.top >= bottom) return FALSE;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Func. Name : RectInRect
// Parameters :

// Return     :

// Remarks    :


BOOL CRectClip::RectInRect(CRect rect) const
{
	if (rect.left < left) return FALSE;
	if (rect.top < top) return FALSE;
	if (rect.right > right) return FALSE;
	if (rect.bottom > bottom) return FALSE;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
// Class Name : C2DRender
// Remarks    :




/*
struct TextureSprite
{
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	LPDIRECT3DTEXTURE9      pTexture[8];
}
*/
#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline DRAWVERTEX Init2DVertex(int x, int y, D3DCOLOR color)
{
	DRAWVERTEX v;
	v.vec.x = (FLOAT)x;
	v.vec.y = (FLOAT)y;
	v.vec.z = 0;
	v.rhw = 1.0f;
	v.color = color;
	return v;
}
/*
inline FONT2DVERTEX Init2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
									  FLOAT tu, FLOAT tv )
{
	FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
	return v;
}
*/
C2DRender::C2DRender()
{
	m_pd3dDevice = NULL;
	m_pVBFillTriangle = NULL;
	m_pVBTriangle = NULL;
	m_pVBFillRect = NULL;
	m_pVBRect = NULL;
	m_pVBRoundRect = NULL;
	m_pVBLine = NULL;
	m_pVBPixel = NULL;
	//m_pVBTexture = NULL;
	m_pFont = NULL;
	m_ptOrigin = 0;
	m_clipRect.SetRect(0, 0, 800, 600);
	m_dwTextColor = D3DCOLOR_RGBA(255, 255, 255, 255);
}
C2DRender::~C2DRender()
{
	DeleteDeviceObjects();
}
HRESULT C2DRender::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;
	return hr;
}
HRESULT C2DRender::RestoreDeviceObjects(D3DSURFACE_DESC* pd3dsdBackBuffer)
{
	if (m_pd3dDevice == NULL) return S_OK;
	m_clipRect.SetRect(0, 0, pd3dsdBackBuffer->Width, pd3dsdBackBuffer->Height);
	HRESULT hr = S_OK;
	/*
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*4, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBFillRect     , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*5, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBRect         , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*8, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBRoundRect    , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*2, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBLine         , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*2, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBPixel        , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*3, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBFillTriangle , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(DRAWVERTEX   )*4, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX   , D3DPOOL_SYSTEMMEM, &m_pVBTriangle     , NULL);
	m_pd3dDevice->CreateVertexBuffer( sizeof(TEXTUREVERTEX)*4, D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBTexture      , NULL);
*/

	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 4, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBFillRect, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 5, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBRect, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 8, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBRoundRect, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 2, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBLine, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 2, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBPixel, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 3, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBFillTriangle, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 4, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_DEFAULT, &m_pVBTriangle, NULL);
	//m_pd3dDevice->CreateVertexBuffer( sizeof(TEXTUREVERTEX)*4, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_DEFAULT, &m_pVBTexture, NULL );


//D3DPOOL_SYSTEMMEM
//r( MAX_NUM_VERTICES*sizeof(FONT2DVERTEX),
  //                                                     D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
	//                                                   D3DPOOL_DEFAULT, &m_pVB ) ) )
	//{
	return hr;
}
HRESULT C2DRender::InvalidateDeviceObjects()
{
	return DeleteDeviceObjects();
}
HRESULT C2DRender::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pVBFillRect);
	SAFE_RELEASE(m_pVBRect);
	SAFE_RELEASE(m_pVBRoundRect);
	SAFE_RELEASE(m_pVBLine);
	SAFE_RELEASE(m_pVBPixel);
	//SAFE_RELEASE(m_pVBTexture  );
	SAFE_RELEASE(m_pVBFillTriangle);
	SAFE_RELEASE(m_pVBTriangle);
	return hr;
}

void C2DRender::SetViewport(CRect rect)
{
	D3DVIEWPORT9 viewport;
	viewport.X = rect.left;
	viewport.Y = rect.top;
	viewport.Width = rect.Width();
	viewport.Height = rect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);
	m_clipRect = rect;
	//SetViewportOrg(rect.left,rect.top);
}
void C2DRender::SetViewport(int nLeft, int nTop, int nRight, int nBottom)
{
	m_clipRect = CRect(nLeft, nTop, nRight, nBottom);
	D3DVIEWPORT9 viewport;
	viewport.X = m_clipRect.left;
	viewport.Y = m_clipRect.top;
	viewport.Width = m_clipRect.Width();
	viewport.Height = m_clipRect.Height();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	m_pd3dDevice->SetViewport(&viewport);
	//SetViewportOrg(nLeft,nTop);
}
BOOL C2DRender::ResizeFillRectVB(CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DTEXTURE9 m_pTexture)
{
	DRAWVERTEX* pDrawVertices;
	HRESULT hr = pVB->Lock(0, sizeof(DRAWVERTEX) * 4, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return FALSE;
	//	dwColor |=  0xff000000;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLB;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRB;
	pDrawVertices++;

	pVB->Unlock();
	return TRUE;
}
BOOL C2DRender::RenderFillRect(CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DTEXTURE9 m_pTexture)
{
	if (m_pd3dDevice == NULL)
		return FALSE;

	if (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0)
		return FALSE;

	rect += m_ptOrigin;	//rect.right; rect.bottom;
	ResizeFillRectVB(&rect, dwColorLT, dwColorRT, dwColorLB, dwColorRB, m_pVBFillRect, m_pTexture);

	if ((dwColorLT & 0xff000000) != 0xff000000 || (dwColorRT & 0xff000000) != 0xff000000 || (dwColorLB & 0xff000000) != 0xff000000 || (dwColorRB & 0xff000000) != 0xff000000) ////bSupportsAlphaBlend )
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
#endif

	m_pd3dDevice->SetTexture(0, m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBFillRect, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
#endif

	return TRUE;
}
BOOL C2DRender::RenderResizeRect(CRect rect, int nThick)
{
	CRect rectT(rect.left, rect.top - nThick, rect.right, rect.top);
	CRect rectB(rect.left, rect.bottom, rect.right, rect.bottom + nThick);
	CRect rectL(rect.left - nThick, rect.top, rect.left, rect.bottom);
	CRect rectR(rect.right, rect.top, rect.right + nThick, rect.bottom);
	CRect rectLT(rect.left - nThick, rect.top - nThick, rect.left, rect.top);
	CRect rectRT(rect.right, rect.top - nThick, rect.right + nThick, rect.top);
	CRect rectLB(rect.left - nThick, rect.bottom, rect.left, rect.bottom + nThick);
	CRect rectRB(rect.right, rect.bottom, rect.right + nThick, rect.bottom + nThick);

	RenderFillRect(rectT, 0xafff8080);
	RenderFillRect(rectB, 0xafff8080);
	RenderFillRect(rectL, 0xafff8080);
	RenderFillRect(rectR, 0xafff8080);
	RenderFillRect(rectLT, 0xff8080ff);
	RenderFillRect(rectRT, 0xff8080ff);
	RenderFillRect(rectLB, 0xff8080ff);
	RenderFillRect(rectRB, 0xff8080ff);

	return TRUE;
}
/*
		CRect rect = m_pWndFocusChild->GetWindowRect( TRUE );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0x80808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xaf808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0x80808080 );
		rect.InflateRect( 1, 1 );
		p2DRender->RenderRect( rect, 0xaf808080 );
*/

/////////////////////////////////////////////////////////////////////////////////////

BOOL C2DRender::ResizeRectVB(CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB)
{
	DRAWVERTEX* pDrawVertices;
	HRESULT hr = pVB->Lock(0, sizeof(DRAWVERTEX) * 5, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return FALSE;
	//dwColor |=  0xff000000;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRB;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLB;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLT;

	pVB->Unlock();
	return TRUE;
}
BOOL C2DRender::RenderRect(CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB)
{
	rect += m_ptOrigin;	rect.right--; rect.bottom--;
	ResizeRectVB(&rect, dwColorLT, dwColorRT, dwColorLB, dwColorRB, m_pVBRect);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
#endif

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBRect, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
#endif

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

BOOL C2DRender::ResizeRoundRectVB(CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DTEXTURE9 m_pTexture)
{
	DRAWVERTEX* pDrawVertices;
	HRESULT hr = pVB->Lock(0, sizeof(DRAWVERTEX) * 8, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return FALSE;
	//	dwColor |=  0xff000000;
		////////////////////////////////
	pDrawVertices->vec.x = (FLOAT)pRect->left + 1;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;// - 1;
	pDrawVertices->vec.y = (FLOAT)pRect->top;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRT;
	pDrawVertices++;
	////////////////////////////////
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->top + 1;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;// - 1;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRB;
	pDrawVertices++;
	////////////////////////////////
	pDrawVertices->vec.x = (FLOAT)pRect->left + 1;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLB;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->right;// - 1;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRB;
	pDrawVertices++;
	////////////////////////////////
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->top + 1;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorLT;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pRect->left;
	pDrawVertices->vec.y = (FLOAT)pRect->bottom;// - 1;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColorRB;
	pDrawVertices++;
	////////////////////////////////
	pVB->Unlock();
	return TRUE;
}
BOOL C2DRender::RenderRoundRect(CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DTEXTURE9 m_pTexture)
{
#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
#endif

	rect += m_ptOrigin;	rect.right--; rect.bottom--;
	ResizeRoundRectVB(&rect, dwColorLT, dwColorRT, dwColorLB, dwColorRB, m_pVBRoundRect);
	m_pd3dDevice->SetTexture(0, m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBRoundRect, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 4);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
#endif

	return TRUE;
}

BOOL C2DRender::RenderLine(CPoint pt1, CPoint pt2, DWORD dwColor1, DWORD dwColor2)
{
	pt1 += m_ptOrigin;
	pt2 += m_ptOrigin;

	DRAWVERTEX* pDrawVertices;
	HRESULT hr = m_pVBLine->Lock(0, sizeof(DRAWVERTEX) * 2, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return FALSE;
	pDrawVertices->vec.x = (FLOAT)pt1.x;
	pDrawVertices->vec.y = (FLOAT)pt1.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor1;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt2.x;
	pDrawVertices->vec.y = (FLOAT)pt2.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor2;
	pDrawVertices++;
	m_pVBLine->Unlock();

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
#endif

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBLine, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);

#ifdef __TEST_ANTIALIASING
	m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
#endif

	return TRUE;
}

void C2DRender::TextOut_EditString(int x, int y, CEditString& strEditString, int nPos, int nLines, int nLineSpace)
{
	SIZE size = m_pFont->GetTextExtent(strEditString);
	size.cy = m_pFont->GetMaxHeight() * strEditString.GetLineCount();
	CRect rect(x + m_ptOrigin.x, y + m_ptOrigin.y, x + m_ptOrigin.x + size.cx, y + m_ptOrigin.y + size.cy);
	if (m_clipRect.RectLapRect(rect))
	{
		TCHAR strHan[3];
		int nLength = strEditString.GetLength();
		DWORD dwCurOffset;
		x += m_ptOrigin.x;
		y += m_ptOrigin.y;
		int nBegin = x;
		m_pFont->DrawText((FLOAT)(x), (FLOAT)(y), 0, strEditString, nPos, nLines, nLineSpace);
		if (nLines == 0)
			nLines = strEditString.GetLineCount();
		int nMax = nPos + nLines;
		if (nMax > (int)(strEditString.GetLineCount()))
			nMax = strEditString.GetLineCount();

		//nLines = nLength;
	//for( int i = nPos; i < nPos + nLength; i++ )
		for (int i = nPos; i < nMax; i++)
		{
			CString string = strEditString.GetLine(i);
			DWORD dwOffset = strEditString.GetLineOffset(i);
			strHan[0] = 0;
			dwCurOffset = 0;
			const char* begin = string;
			const char* end = begin + string.GetLength();
			const char* iter = begin;

			while (*iter && iter < end)
			{
				DWORD dwColor = strEditString.m_adwColor[dwOffset + iter - begin];
				DWORD dwStyle = strEditString.m_abyStyle[dwOffset + iter - begin];
				WORD wCodePage = strEditString.m_awCodePage[dwOffset + iter - begin];

				const char* next = CharNextEx(iter, wCodePage);

				char temp[16];
				memcpy(temp, iter, next - iter);
				temp[next - iter] = 0;

				SIZE size;
				m_pFont->GetTextExtent(temp, &size, wCodePage);

				iter = next;

				if (*temp == 10)		//gmpbigsun(100414) : pass a character 'ENTER'
					continue;


				if (dwStyle & ESSTY_BLOCK)
				{
					DWORD dwBkgr = dwColor;
					dwColor = ~dwColor | 0xff000000;
					RenderFillRect(CRect(x - m_ptOrigin.x, y - m_ptOrigin.y, x + size.cx - m_ptOrigin.x, y + size.cy - m_ptOrigin.y), 0xff000000);
					m_pFont->DrawText((FLOAT)(x), (FLOAT)(y), dwColor, temp, 0, wCodePage);
				}
				if (dwStyle & ESSTY_SHADOW)
				{
					m_pFont->DrawText((FLOAT)(x + 2), (FLOAT)(y + 1), 0xffcfcfcf, temp, 0, wCodePage);
					m_pFont->DrawText((FLOAT)(x + 1), (FLOAT)(y), dwColor, temp, 0, wCodePage);
				}

				if (dwStyle & ESSTY_BOLD)
				{
					if (::GetLanguage() == LANG_THA)
						m_pFont->DrawText((FLOAT)(x), (FLOAT)(y), 0xff1010ff, temp, 0, wCodePage);
					else
						m_pFont->DrawText((FLOAT)(x + 1), (FLOAT)(y), dwColor, temp, 0, wCodePage);
				}

				if (dwStyle & ESSTY_UNDERLINE)
					RenderLine(CPoint(x - m_ptOrigin.x, y + size.cy - m_ptOrigin.y - 2), CPoint(x + size.cx - m_ptOrigin.x, y + size.cy - m_ptOrigin.y - 2), dwColor);
				if (dwStyle & ESSTY_STRIKETHROUGH)
					RenderLine(CPoint(x - m_ptOrigin.x, y + (size.cy / 2) - m_ptOrigin.y), CPoint(x + size.cx - m_ptOrigin.x, y + (size.cy / 2) - m_ptOrigin.y), dwColor);

				x += size.cx;
			}

			x = nBegin;
			y += m_pFont->GetMaxHeight() + nLineSpace;
		}
	}
}

void C2DRender::TextOut(int x, int y, LPCTSTR pszString, DWORD dwColor, DWORD dwShadowColor)
{
	if (m_pFont)
	{
		SIZE size = m_pFont->GetTextExtent(pszString);
		//size.cx = 8;
		CRect rect(x + m_ptOrigin.x, y + m_ptOrigin.y, x + m_ptOrigin.x + size.cx, y + m_ptOrigin.y + size.cy);
		if (m_clipRect.RectLapRect(rect))
		{
			if (dwShadowColor & 0xff000000)
				m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x + 1), (FLOAT)(y + m_ptOrigin.y + 1), dwShadowColor, (TCHAR*)pszString);
			m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x), (FLOAT)(y + m_ptOrigin.y), dwColor, (TCHAR*)pszString);
		}
	}
}
void C2DRender::TextOut(int x, int y, FLOAT fXScale, FLOAT fYScale, LPCTSTR pszString, DWORD dwColor, DWORD dwShadowColor)
{
	if (m_pFont)
	{
		SIZE size = m_pFont->GetTextExtent(pszString);
		CRect rect(x + m_ptOrigin.x, y + m_ptOrigin.y, x + m_ptOrigin.x + size.cx, y + m_ptOrigin.y + size.cy);
		if (m_clipRect.RectLapRect(rect))
		{
			if (dwShadowColor & 0xff000000)
				m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x + 1), (FLOAT)(y + m_ptOrigin.y + 1), dwShadowColor, (TCHAR*)pszString);
			m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x), (FLOAT)(y + m_ptOrigin.y), fXScale, fYScale, dwColor, (TCHAR*)pszString);
		}
	}
}
void C2DRender::TextOut(int x, int y, int nValue, DWORD dwColor, DWORD dwShadowColor)
{
	if (m_pFont)
	{
		TCHAR szString[32];
		_itot(nValue, szString, 10);
		SIZE size = m_pFont->GetTextExtent(szString);
		CRect rect(x + m_ptOrigin.x, y + m_ptOrigin.y, x + m_ptOrigin.x + size.cx, y + m_ptOrigin.y + size.cy);
		if (m_clipRect.RectLapRect(rect))
		{
			if (dwShadowColor & 0xff000000)
				m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x + 1), (FLOAT)(y + m_ptOrigin.y + 1), dwShadowColor, (TCHAR*)szString);
			m_pFont->DrawText((FLOAT)(x + m_ptOrigin.x), (FLOAT)(y + m_ptOrigin.y), dwColor, (TCHAR*)szString);
		}
	}
}

BOOL C2DRender::RenderTexture2(CPoint pt, CTexture* pTexture, FLOAT fScaleX, FLOAT fScaleY, D3DCOLOR color)
{
	pt += m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = pt.x + (fScaleX * pTexture->m_size.cx);
	FLOAT bottom = pt.y + (fScaleY * pTexture->m_size.cy);

	TEXTUREVERTEX2 vertex[4];
	TEXTUREVERTEX2* pVertices = vertex;

	SetTextureVertex2(pVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT, color);
	pVertices++;
	SetTextureVertex2(pVertices, left, bottom, pTexture->m_fuLB, pTexture->m_fvLB, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB, color);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX2));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
};

BOOL C2DRender::RenderTextureColor(CPoint pt, CTexture* pTexture, FLOAT fScaleX, FLOAT fScaleY, D3DCOLOR color)
{
	pt += m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = pt.x + (fScaleX * pTexture->m_size.cx);
	FLOAT bottom = pt.y + (fScaleY * pTexture->m_size.cy);

	TEXTUREVERTEX2 vertex[4];
	TEXTUREVERTEX2* pVertices = vertex;

	SetTextureVertex2(pVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT, color);
	pVertices++;
	SetTextureVertex2(pVertices, left, bottom, pTexture->m_fuLB, pTexture->m_fvLB, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB, color);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif


	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX2));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
};

BOOL C2DRender::RenderTextureEx(CPoint pt, CPoint pt2, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX, FLOAT fScaleY, BOOL bAnti)
{
	pt += m_ptOrigin;
	if (!pTexture) return FALSE;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = pt.x + (fScaleX * pt2.x);
	FLOAT bottom = pt.y + (fScaleY * pt2.y);

	TEXTUREVERTEX vertex[4];
	TEXTUREVERTEX* pVertices = vertex;

	SetTextureVertex(pVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT);
	pVertices++;
	SetTextureVertex(pVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT);
	pVertices++;
	SetTextureVertex(pVertices, left, bottom, pTexture->m_fuLB, pTexture->m_fvLB);
	pVertices++;
	SetTextureVertex(pVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);

#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwBlendFactorAlhpa, 0, 0, 0));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
};

BOOL C2DRender::RenderTextureEx2(CPoint pt, CPoint pt2, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX, FLOAT fScaleY, D3DCOLOR color)
{
	pt += m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = pt.x + (fScaleX * pt2.x);
	FLOAT bottom = pt.y + (fScaleY * pt2.y);

	TEXTUREVERTEX2 vertex[4];
	TEXTUREVERTEX2* pVertices = vertex;

	SetTextureVertex2(pVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT, color);
	pVertices++;
	SetTextureVertex2(pVertices, left, bottom, pTexture->m_fuLB, pTexture->m_fvLB, color);
	pVertices++;
	SetTextureVertex2(pVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB, color);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwBlendFactorAlhpa, 0, 0, 0));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, NULL /*pTexture->m_pTexture*/);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX2));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
};


BOOL C2DRender::RenderTexture(CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX, FLOAT fScaleY)
{
	try
	{
		if (!pTexture)			//gmpbigsun
			return FALSE;

		pt += m_ptOrigin;
		CPoint ptCenter = pTexture->m_ptCenter;
		ptCenter.x = (LONG)(ptCenter.x * fScaleX);
		ptCenter.y = (LONG)(ptCenter.y * fScaleY);
		pt -= ptCenter;

		FLOAT left = (FLOAT)(pt.x);
		FLOAT top = (FLOAT)(pt.y);
		FLOAT right = pt.x + (fScaleX * pTexture->m_size.cx);
		FLOAT bottom = pt.y + (fScaleY * pTexture->m_size.cy);

		TEXTUREVERTEX vertex[4];
		TEXTUREVERTEX* pVertices = vertex;

		SetTextureVertex(pVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT);
		pVertices++;
		SetTextureVertex(pVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT);
		pVertices++;
		SetTextureVertex(pVertices, left, bottom, pTexture->m_fuLB, pTexture->m_fvLB);
		pVertices++;
		SetTextureVertex(pVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB);
		pVertices++;

		m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwBlendFactorAlhpa, 0, 0, 0));
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
		m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));
		m_pd3dDevice->SetTexture(0, NULL);
		return TRUE;

	}
	catch (...)
	{
		return false;
	}
};

BOOL C2DRender::RenderTextureForMinimap(const CRect& kRect, CTexture* pTexture, CTexture* pAlphaTex, const CSize& kSize)
{
	if (NULL == pTexture || NULL == pAlphaTex)
		return FALSE;

	static CTexture kTexFrame;
	if (NULL == kTexFrame.m_pTexture)
		kTexFrame.LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "map.tga"), 0xffff00ff, TRUE);

	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	CPoint pt = m_ptOrigin;
	CPoint ptCenter = pAlphaTex->m_ptCenter;
	pt -= ptCenter;

	FLOAT left = (FLOAT)pt.x;
	FLOAT top = (FLOAT)pt.y;
	FLOAT right = left + (fScaleX * kSize.cx);
	FLOAT bottom = top + (fScaleY * kSize.cy);

	TEXTUREVERTEX vertex[4];
	TEXTUREVERTEX* pVertices = vertex;

	SetTextureVertex(pVertices, left, top, pAlphaTex->m_fuLT, pAlphaTex->m_fvLT);
	pVertices++;
	SetTextureVertex(pVertices, right, top, pAlphaTex->m_fuRT, pAlphaTex->m_fvRT);
	pVertices++;
	SetTextureVertex(pVertices, left, bottom, pAlphaTex->m_fuLB, pAlphaTex->m_fvLB);
	pVertices++;
	SetTextureVertex(pVertices, right, bottom, pAlphaTex->m_fuRB, pAlphaTex->m_fvRB);
	pVertices++;

	m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	m_pd3dDevice->SetTexture(1, pAlphaTex->m_pTexture);


	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));

	m_pd3dDevice->SetTexture(0, kTexFrame.m_pTexture);
	m_pd3dDevice->SetTexture(1, NULL);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));

	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetTexture(1, NULL);

	return TRUE;
}

BOOL C2DRender::RenderTextureRotate(CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX, FLOAT fScaleY, FLOAT fRadian)
{
	pt += m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT _left = (FLOAT)(pt.x);
	FLOAT _top = (FLOAT)(pt.y);
	FLOAT _right = pt.x + (fScaleX * pTexture->m_size.cx);
	FLOAT _bottom = pt.y + (fScaleY * pTexture->m_size.cy);


	//	FLOAT cy = _top  + ((_bottom - _top) / 2);
	FLOAT cx = _left + ptCenter.x;
	FLOAT cy = _top + ptCenter.y;

	_left -= cx;
	_right -= cx;
	_top -= cy;
	_bottom -= cy;

	D3DXVECTOR2	v1, v2, v3, v4;
	D3DXMATRIX	mRot;

	v1.x = _left;	v1.y = _top;
	v2.x = _right;	v2.y = _top;
	v3.x = _left;	v3.y = _bottom;
	v4.x = _right;	v4.y = _bottom;

	D3DXMatrixRotationZ(&mRot, fRadian);
	D3DXVec2TransformCoord(&v1, &v1, &mRot);
	D3DXVec2TransformCoord(&v2, &v2, &mRot);
	D3DXVec2TransformCoord(&v3, &v3, &mRot);
	D3DXVec2TransformCoord(&v4, &v4, &mRot);


	TEXTUREVERTEX vertex[4];
	TEXTUREVERTEX* pVertices = vertex;

	SetTextureVertex(pVertices, cx + v1.x, cy + v1.y, pTexture->m_fuLT, pTexture->m_fvLT);
	pVertices++;
	SetTextureVertex(pVertices, cx + v2.x, cy + v2.y, pTexture->m_fuRT, pTexture->m_fvRT);
	pVertices++;
	SetTextureVertex(pVertices, cx + v3.x, cy + v3.y, pTexture->m_fuLB, pTexture->m_fvLB);
	pVertices++;
	SetTextureVertex(pVertices, cx + v4.x, cy + v4.y, pTexture->m_fuRB, pTexture->m_fvRB);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif


	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwBlendFactorAlhpa, 0, 0, 0));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
}

BOOL C2DRender::RenderTextureRotate(CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fRadian, BOOL bCenter, FLOAT fScaleX, FLOAT fScaleY)
{

	// if bCenter is TRUE, Axis is texture center
	// else Axis is left-top

	pt += m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	ptCenter.x = (LONG)(ptCenter.x * fScaleX);
	ptCenter.y = (LONG)(ptCenter.y * fScaleY);
	pt -= ptCenter;

	FLOAT _left = (FLOAT)(pt.x);
	FLOAT _top = (FLOAT)(pt.y);
	FLOAT _right = pt.x + (fScaleX * pTexture->m_size.cx);
	FLOAT _bottom = pt.y + (fScaleY * pTexture->m_size.cy);

	FLOAT cx = 0.0f;
	FLOAT cy = 0.0f;
	if (bCenter)
	{
		cx = _left + ((_right - _left) / 2);
		cy = _top + ((_bottom - _top) / 2);
	}
	else
	{
		cx = _left + ptCenter.x;
		cy = _top + ptCenter.y;
	}

	_left -= cx;
	_right -= cx;
	_top -= cy;
	_bottom -= cy;

	D3DXVECTOR2	v1, v2, v3, v4;
	D3DXMATRIX	mRot;

	v1.x = _left;	v1.y = _top;
	v2.x = _right;	v2.y = _top;
	v3.x = _left;	v3.y = _bottom;
	v4.x = _right;	v4.y = _bottom;

	D3DXMatrixRotationZ(&mRot, fRadian);
	D3DXVec2TransformCoord(&v1, &v1, &mRot);
	D3DXVec2TransformCoord(&v2, &v2, &mRot);
	D3DXVec2TransformCoord(&v3, &v3, &mRot);
	D3DXVec2TransformCoord(&v4, &v4, &mRot);


	TEXTUREVERTEX vertex[4];
	TEXTUREVERTEX* pVertices = vertex;

	SetTextureVertex(pVertices, cx + v1.x, cy + v1.y, pTexture->m_fuLT, pTexture->m_fvLT);
	pVertices++;
	SetTextureVertex(pVertices, cx + v2.x, cy + v2.y, pTexture->m_fuRT, pTexture->m_fvRT);
	pVertices++;
	SetTextureVertex(pVertices, cx + v3.x, cy + v3.y, pTexture->m_fuLB, pTexture->m_fvLB);
	pVertices++;
	SetTextureVertex(pVertices, cx + v4.x, cy + v4.y, pTexture->m_fuRB, pTexture->m_fvRB);
	pVertices++;

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
#ifndef __TEST_ANISOTROPIC
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwBlendFactorAlhpa, 0, 0, 0));
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(TEXTUREVERTEX));
	m_pd3dDevice->SetTexture(0, NULL);
	return TRUE;
}

BOOL C2DRender::RenderFillTriangle(CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3)
{
#ifdef __FL_WHEEL_OF_FORTUNE
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
#endif

	pt1 += m_ptOrigin;
	pt2 += m_ptOrigin;
	pt3 += m_ptOrigin;

	DRAWVERTEX* pDrawVertices;
	HRESULT hr = m_pVBFillTriangle->Lock(0, sizeof(DRAWVERTEX) * 3, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	pDrawVertices->vec.x = (FLOAT)pt1.x;
	pDrawVertices->vec.y = (FLOAT)pt1.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor1;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt2.x;
	pDrawVertices->vec.y = (FLOAT)pt2.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor2;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt3.x;
	pDrawVertices->vec.y = (FLOAT)pt3.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor3;
	pDrawVertices++;
	m_pVBFillTriangle->Unlock();

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBFillTriangle, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////

BOOL C2DRender::RenderTriangle(CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3)
{
#ifdef __FL_WHEEL_OF_FORTUNE
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
#endif

	pt1 += m_ptOrigin;
	pt2 += m_ptOrigin;
	pt3 += m_ptOrigin;

	DRAWVERTEX* pDrawVertices;
	HRESULT hr = m_pVBTriangle->Lock(0, sizeof(DRAWVERTEX) * 4, (void**)&pDrawVertices, D3DLOCK_DISCARD);
	pDrawVertices->vec.x = (FLOAT)pt1.x;
	pDrawVertices->vec.y = (FLOAT)pt1.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor1;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt2.x;
	pDrawVertices->vec.y = (FLOAT)pt2.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor2;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt3.x;
	pDrawVertices->vec.y = (FLOAT)pt3.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor3;
	pDrawVertices++;
	pDrawVertices->vec.x = (FLOAT)pt1.x;
	pDrawVertices->vec.y = (FLOAT)pt1.y;
	pDrawVertices->vec.z = 0.0f;
	pDrawVertices->rhw = 1.0f;
	pDrawVertices->color = dwColor1;
	m_pVBTriangle->Unlock();

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF(D3DFVF_DRAWVERTEX);
	m_pd3dDevice->SetStreamSource(0, m_pVBTriangle, 0, sizeof(DRAWVERTEX));
	m_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 3);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
	m_pTexture = NULL;
	m_bAutoFree = FALSE;
	m_ptCenter = CPoint(0, 0);
}
CTexture::~CTexture()
{
	if (m_bAutoFree)
		DeleteDeviceObjects();
}

BOOL CTexture::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pTexture);
	return TRUE;
}
BOOL CTexture::CreateTexture(LPDIRECT3DDEVICE9 pd3dDevice,
	int nWidth, int nHeight, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool)
{
#ifdef __FL_FIX_MEMORY_LEAKS
	DeleteDeviceObjects();
#endif

	HRESULT hr = pd3dDevice->CreateTexture(nWidth, nHeight, 1, Usage, Format, Pool, &m_pTexture, NULL);
	if (hr != D3D_OK)
	{
		CString string;
		string.Format("CreateTexture Error = %d, cx = %d, cy = %d", hr, nWidth, nHeight);
		ASSERT(0);
		return FALSE;
	}
	m_size = CSize(nWidth, nHeight);
	m_fuLT = 0.0f;
	m_fvLT = 0.0f;
	m_fuRT = 1.0f;
	m_fvRT = 0.0f;
	m_fuLB = 0.0f;
	m_fvLB = 1.0f;
	m_fuRB = 1.0f,
	m_fvRB = 1.0f;
	m_ptCenter = CPoint(0, 0);
	m_bAutoFree = TRUE;

	return TRUE;
}

BOOL CTexture::LoadTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader)
{
#ifdef __FL_FIX_MEMORY_LEAKS
	DeleteDeviceObjects();
#endif

#ifndef __WORLDSERVER
	SIZE size;
	TCHAR szFileName[MAX_PATH];
	DWORD dwOffsetDest = 0;
	DWORD dwOffsetSrc = 0;
	_tcscpy(szFileName, pFileName);
	_tcslwr(szFileName);

	if (bMyLoader == FALSE) // dds only
	{
		// item, icon, A1R5G5B5
		//dKeyColor = 0;
		D3DXIMAGE_INFO imageInfo;
		// Create the texture using D3DX
		HRESULT hr = ::LoadTextureFromRes(pd3dDevice, pFileName,
			D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5,//D3DFMT_UNKNOWN,//D3DFMT_A1R5G5B5,
			D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
			D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture);
		if (hr != S_OK)
		{
			LPCTSTR szErr = Error("CTexture::LoadTexture : %s read error", pFileName);
			//ADDERRORMSG( szErr );
			return FALSE;
		}
		size.cx = imageInfo.Width;
		size.cy = imageInfo.Height;
		m_size = size;
	}
	else if (strstr(szFileName, ".bmp"))
	{
		IMAGE image;
		LPBYTE lpData = NULL;
		if (LoadBMP(pFileName, &image) == TRUE)
		{
			lpData = image.lpData;
			m_size = image.size;
			size = m_size;
			AdjustSize(&m_size);
			int nDestLine = m_size.cx - size.cx;

			//#define BMP32BIT
#ifdef BMP32BIT
			CreateTexture(pd3dDevice, m_size.cx, m_size.cy, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
#else
			CreateTexture(pd3dDevice, m_size.cx, m_size.cy, D3DX_DEFAULT, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED);
#endif
			HRESULT hr;
			D3DLOCKED_RECT lockedRect;
			if ((hr = m_pTexture->LockRect(0, &lockedRect, 0, 0)) == D3D_OK)// Lock
			{
#ifdef BMP32BIT
				LPDWORD pDest = (LPDWORD)lockedRect.pBits;
				LPDWORD pSrc = (LPDWORD)lpData;
#else
				LPWORD pDest = (LPWORD)lockedRect.pBits;
#endif
				for (int y = 0; y < size.cy; y++, dwOffsetDest += nDestLine)
				{
					for (int x = 0; x < size.cx; x++, dwOffsetSrc += 3, dwOffsetDest++)
					{
						BYTE byData1 = lpData[dwOffsetSrc + 0];
						BYTE byData2 = lpData[dwOffsetSrc + 1];
						BYTE byData3 = lpData[dwOffsetSrc + 2];
#ifdef BMP32BIT
						if (byData1 == 255 && byData2 == 0 && byData3 == 255)
							pDest[dwOffsetDest] = 0;
						else
							pDest[dwOffsetDest] = 0xff000000 | (byData3 << 16) | (byData2 << 8) | byData1;
#else
						if (byData1 == 255 && byData2 == 0 && byData3 == 255)
							pDest[dwOffsetDest] = 0x0000;
						else
							pDest[dwOffsetDest] = 0x8000 | ((byData3 >> 3) << 10) | ((byData2 >> 3) << 5) | (byData1 >> 3);
#endif
					}
				}
				m_pTexture->UnlockRect(0);
			}
			else
			{
#ifndef _DEBUG
				CString string;
				string.Format("Lock 실패 : Error = %d", hr);
				//ADDERRORMSG( string );
				ASSERT(0);
#endif
			}
		}
		else
			Error("CTexture::LoadTexture : %s read error", pFileName);
	}
	else if (strstr(szFileName, ".tga"))
	{
		LPBYTE lpData = NULL;
		CTarga targa;
		if (targa.Load(pFileName, &lpData, &m_size) == TRUE)
		{
			size = m_size;
			AdjustSize(&m_size);
			int nDestLine = m_size.cx - size.cx;
#define BMP32BIT
#ifdef BMP32BIT
			CreateTexture(pd3dDevice, m_size.cx, m_size.cy, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);// == FALSE )
#else
			CreateTexture(pd3dDevice, m_size.cx, m_size.cy, D3DX_DEFAULT, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED);
#endif
			HRESULT hr;
			D3DLOCKED_RECT lockedRect;
			if ((hr = m_pTexture->LockRect(0, &lockedRect, 0, 0)) == D3D_OK)// Lock
			{
#ifdef BMP32BIT
				LPDWORD pDest = (LPDWORD)lockedRect.pBits;
				LPDWORD pSrc = (LPDWORD)lpData;
#else
				LPWORD pDest = (LPWORD)lockedRect.pBits;
#endif
				for (int y = 0; y < size.cy; y++, dwOffsetDest += nDestLine)
				{
					for (int x = 0; x < size.cx; x++, dwOffsetSrc++, dwOffsetDest++)
					{
#ifdef BMP32BIT
						pDest[dwOffsetDest] = pSrc[dwOffsetSrc];
#else
						BYTE byData1 = lpData[dwOffsetSrc + 0] >> 4;
						BYTE byData2 = lpData[dwOffsetSrc + 1] >> 4;
						BYTE byData3 = lpData[dwOffsetSrc + 2] >> 4;
						BYTE byData4 = lpData[dwOffsetSrc + 3] >> 4;

						if (byData1 == 0xf && byData2 == 0 && byData3 == 0xf)
							pDest[dwOffsetDest] = 0x0000;
						else
							pDest[dwOffsetDest] = (byData4 << 12) | (byData3 << 8) | (byData2 << 4) | byData1;
#endif
					}
				}
				m_pTexture->UnlockRect(0);
			}
			else
			{
#ifndef _DEBUG
				CString string;
				string.Format("Lock 실패 : Error = %d", hr);
				//ADDERRORMSG( string );
				ASSERT(0);
#endif
			}
		}
		else
			Error("CTexture::LoadTexture : %s read error", pFileName);

		SAFE_DELETE(lpData);
	}

	if (m_pTexture == NULL)
		return FALSE;

#undef BMP32BIT
	m_fuLT = 0.0f;
	m_fvLT = 0.0f;

	m_fuRT = (FLOAT)size.cx / m_size.cx;
	m_fvRT = 0.0f;

	m_fuLB = 0.0f;
	m_fvLB = (FLOAT)size.cy / m_size.cy;

	m_fuRB = (FLOAT)size.cx / m_size.cx;
	m_fvRB = (FLOAT)size.cy / m_size.cy;

	m_ptCenter.x = 0;
	m_ptCenter.y = 0;

	m_sizePitch = m_size;
	m_size = size;

	m_bAutoFree = TRUE;
#endif	// __WORLDSERVER
	return 1;
}



CTexturePack::CTexturePack()
{
	m_dwNumber = 0;
	m_pTexture = NULL;
	m_ap2DTexture = NULL;
}
CTexturePack::~CTexturePack()
{
	DeleteDeviceObjects();
}
BOOL CTexturePack::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_DELETE_ARRAY(m_ap2DTexture);
	return TRUE;
}

void CTexturePack::MakeVertex(C2DRender* p2DRender, CPoint point, int nIndex, TEXTUREVERTEX** ppVertices)
{
	CTexture* pTexture = GetAt(nIndex);

	point += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;

	point -= ptCenter;

	FLOAT left = (FLOAT)(point.x);
	FLOAT top = (FLOAT)(point.y);
	FLOAT right = (FLOAT)(point.x + ( /*fScaleX **/ pTexture->m_size.cx));
	FLOAT bottom = (FLOAT)(point.y + ( /*fScaleY **/ pTexture->m_size.cy));

	SetTextureVertex(*ppVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT);
	(*ppVertices)++;
	SetTextureVertex(*ppVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT);
	(*ppVertices)++;
	SetTextureVertex(*ppVertices, (FLOAT)(point.x), bottom, pTexture->m_fuLB, pTexture->m_fvLB);
	(*ppVertices)++;

	SetTextureVertex(*ppVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT);
	(*ppVertices)++;
	SetTextureVertex(*ppVertices, (FLOAT)(point.x), bottom, pTexture->m_fuLB, pTexture->m_fvLB);
	(*ppVertices)++;
	SetTextureVertex(*ppVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB);
	(*ppVertices)++;
}
void CTexturePack::MakeVertex(C2DRender* p2DRender, CPoint point, int nIndex, TEXTUREVERTEX2** ppVertices, DWORD dwColor)
{
	CTexture* pTexture = GetAt(nIndex);

	point += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;

	point -= ptCenter;

	FLOAT left = (FLOAT)(point.x);
	FLOAT top = (FLOAT)(point.y);
	FLOAT right = (FLOAT)(point.x + ( /*fScaleX **/ pTexture->m_size.cx));
	FLOAT bottom = (FLOAT)(point.y + ( /*fScaleY **/ pTexture->m_size.cy));

	SetTextureVertex2(*ppVertices, left, top, pTexture->m_fuLT, pTexture->m_fvLT, dwColor);
	(*ppVertices)++;
	SetTextureVertex2(*ppVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT, dwColor);
	(*ppVertices)++;
	SetTextureVertex2(*ppVertices, (FLOAT)(point.x), bottom, pTexture->m_fuLB, pTexture->m_fvLB, dwColor);
	(*ppVertices)++;

	SetTextureVertex2(*ppVertices, right, top, pTexture->m_fuRT, pTexture->m_fvRT, dwColor);
	(*ppVertices)++;
	SetTextureVertex2(*ppVertices, (FLOAT)(point.x), bottom, pTexture->m_fuLB, pTexture->m_fvLB, dwColor);
	(*ppVertices)++;
	SetTextureVertex2(*ppVertices, right, bottom, pTexture->m_fuRB, pTexture->m_fvRB, dwColor);
	(*ppVertices)++;
}
void CTexturePack::Render(LPDIRECT3DDEVICE9 pd3dDevice, TEXTUREVERTEX* pVertices, int nVertexNum)
{
	if (nVertexNum == 0)
		return;
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));

	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetTexture(0, m_pTexture);
	pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nVertexNum / 3, pVertices, sizeof(TEXTUREVERTEX));
}
void CTexturePack::Render(LPDIRECT3DDEVICE9 pd3dDevice, TEXTUREVERTEX2* pVertices, int nVertexNum)
{
	if (nVertexNum == 0)
		return;
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetTexture(0, m_pTexture);
	pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nVertexNum / 3, pVertices, sizeof(TEXTUREVERTEX2));
}
BOOL CTexturePack::LoadScript(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pszFileName)
{
	CScanner scanner;
	if (scanner.Load(pszFileName) == FALSE)
	{
		Error("%s not found", pszFileName);
		return FALSE;
	}

	DWORD dwCount = 0;
	CTexture* pTexture;
	BOOL bMultiLang = FALSE;

	do
	{
		scanner.GetToken();
		if (scanner.Token == "number")
		{
			m_dwNumber = scanner.GetNumber();
			m_ap2DTexture = new CTexture[m_dwNumber];
		}
		else if (scanner.Token == "MULTI_LANGUAGE")
		{
			bMultiLang = TRUE;
		}
		else if (scanner.Token == "texture")
		{
			scanner.GetToken();

			TCHAR strFileName[MAX_PATH];
			strcpy(strFileName, scanner.token);
			D3DCOLOR d3dKeyColor = scanner.GetHex();

			D3DXIMAGE_INFO imageInfo;

			if (bMultiLang)
			{
				LoadTextureFromRes(pd3dDevice, MakePath(DIR_THEME, strFileName),
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4,
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture);
			}
			else
			{
				LoadTextureFromRes(pd3dDevice, strFileName,
					D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4,
					D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
					D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, d3dKeyColor, &imageInfo, NULL, &m_pTexture);
			}

			m_size.cx = imageInfo.Width;
			m_size.cy = imageInfo.Height;
		}
		else if (scanner.Token == "data")
		{
			if (dwCount >= m_dwNumber)
			{
			}
			pTexture = &m_ap2DTexture[dwCount++];
			pTexture->m_size.cx = scanner.GetNumber();
			pTexture->m_size.cy = scanner.GetNumber();
			pTexture->m_fuLT = (FLOAT)scanner.GetNumber() / m_size.cx;
			pTexture->m_fvLT = (FLOAT)scanner.GetNumber() / m_size.cy;
			pTexture->m_fuRT = (FLOAT)scanner.GetNumber() / m_size.cx;
			pTexture->m_fvRT = (FLOAT)scanner.GetNumber() / m_size.cy;
			pTexture->m_fuLB = (FLOAT)scanner.GetNumber() / m_size.cx;
			pTexture->m_fvLB = (FLOAT)scanner.GetNumber() / m_size.cy;
			pTexture->m_fuRB = (FLOAT)scanner.GetNumber() / m_size.cx;
			pTexture->m_fvRB = (FLOAT)scanner.GetNumber() / m_size.cy;
			pTexture->m_pTexture = m_pTexture;
		}
		else if (scanner.Token == "pos")
		{
			if (dwCount >= m_dwNumber)
			{
			}
			pTexture = &m_ap2DTexture[dwCount++];
			pTexture->m_size.cx = scanner.GetNumber();
			pTexture->m_size.cy = scanner.GetNumber();
			int x = scanner.GetNumber();
			int y = scanner.GetNumber();
			scanner.GetToken();
			if (scanner.Token == "h")
			{
				pTexture->m_fuRT = (FLOAT)x / m_size.cx;
				pTexture->m_fvRT = (FLOAT)y / m_size.cy;
				pTexture->m_fuLT = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvLT = (FLOAT)y / m_size.cy;
				pTexture->m_fuRB = (FLOAT)x / m_size.cx;
				pTexture->m_fvRB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
				pTexture->m_fuLB = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvLB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
			}
			else if (scanner.Token == "v")
			{
				pTexture->m_fuLB = (FLOAT)x / m_size.cx;
				pTexture->m_fvLB = (FLOAT)y / m_size.cy;
				pTexture->m_fuRB = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvRB = (FLOAT)y / m_size.cy;
				pTexture->m_fuLT = (FLOAT)x / m_size.cx;
				pTexture->m_fvLT = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
				pTexture->m_fuRT = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvRT = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
			}
			else if (scanner.Token == "hv")
			{
				pTexture->m_fuRB = (FLOAT)x / m_size.cx;
				pTexture->m_fvRB = (FLOAT)y / m_size.cy;
				pTexture->m_fuLB = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvLB = (FLOAT)y / m_size.cy;
				pTexture->m_fuRT = (FLOAT)x / m_size.cx;
				pTexture->m_fvRT = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
				pTexture->m_fuLT = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvLT = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
			}
			else
			{
				pTexture->m_fuLT = (FLOAT)x / m_size.cx;
				pTexture->m_fvLT = (FLOAT)y / m_size.cy;
				pTexture->m_fuRT = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvRT = (FLOAT)y / m_size.cy;
				pTexture->m_fuLB = (FLOAT)x / m_size.cx;
				pTexture->m_fvLB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
				pTexture->m_fuRB = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
				pTexture->m_fvRB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
			}
			pTexture->m_pTexture = m_pTexture;
		}
		else if (scanner.Token == "datauv")
		{
			if (dwCount >= m_dwNumber)
			{
			}
			pTexture = &m_ap2DTexture[dwCount++];
			pTexture->m_size.cx = scanner.GetNumber();
			pTexture->m_size.cy = scanner.GetNumber();
			pTexture->m_fuLT = scanner.GetFloat();
			pTexture->m_fvLT = scanner.GetFloat();
			pTexture->m_fuRT = scanner.GetFloat();
			pTexture->m_fvRT = scanner.GetFloat();
			pTexture->m_fuLB = scanner.GetFloat();
			pTexture->m_fvLB = scanner.GetFloat();
			pTexture->m_fuRB = scanner.GetFloat();
			pTexture->m_fvRB = scanner.GetFloat();
			pTexture->m_pTexture = m_pTexture;
		}
		else if (scanner.Token == "serialize")
		{
			if (dwCount >= m_dwNumber)
			{
				Error("%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount);
				return FALSE;
			}
			int nCnt = 0;
			int nFrame = scanner.GetNumber();
			SIZE size;
			size.cx = scanner.GetNumber();
			size.cy = scanner.GetNumber();
			POINT start;
			start.x = scanner.GetNumber();
			start.y = scanner.GetNumber();
			POINT center;
			center.x = scanner.GetNumber();
			center.y = scanner.GetNumber();

			for (int i = start.y; i < m_size.cy; i += size.cy)
			{
				for (int j = start.x; j < m_size.cx; j += size.cx, nCnt++)
				{
					if (nCnt < nFrame)
					{

						if (dwCount >= m_dwNumber)
						{
							Error("%s 에러, 할당 :%d, 실제갯수 : %d", pszFileName, m_dwNumber, dwCount);
							return FALSE;
						}

						pTexture = &m_ap2DTexture[dwCount];
						dwCount++;

						pTexture->m_size = size;
						pTexture->m_ptCenter = center;
						int x = j;
						int y = i;
						pTexture->m_fuLT = (FLOAT)x / m_size.cx;
						pTexture->m_fvLT = (FLOAT)y / m_size.cy;
						pTexture->m_fuRT = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
						pTexture->m_fvRT = (FLOAT)y / m_size.cy;
						pTexture->m_fuLB = (FLOAT)x / m_size.cx;
						pTexture->m_fvLB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
						pTexture->m_fuRB = (FLOAT)((FLOAT)x + pTexture->m_size.cx - 0) / (FLOAT)m_size.cx;
						pTexture->m_fvRB = (FLOAT)((FLOAT)y + pTexture->m_size.cy - 0) / (FLOAT)m_size.cy;
						pTexture->m_pTexture = m_pTexture;
					}
				}
			}
		}
	} while (scanner.tok != FINISHED);
	return TRUE;
}

CTextureMng::CTextureMng()
{
}
CTextureMng::~CTextureMng()
{
	DeleteDeviceObjects();
}

BOOL CTextureMng::DeleteDeviceObjects()
{
#ifdef __VS2003
	if (m_mapTexture.size() <= 0)
		return TRUE;
#endif

	for (MapTexItor i = m_mapTexture.begin(); i != m_mapTexture.end(); ++i)
		SAFE_DELETE((*i).second);
	m_mapTexture.clear();
	return TRUE;
}
BOOL CTextureMng::RemoveTexture(LPCTSTR pKey)
{
	if (m_mapTexture.empty())
		return TRUE;

	MapTexItor mapTexItor = m_mapTexture.find(pKey);
	if (mapTexItor != m_mapTexture.end())
	{
		SAFE_DELETE((*mapTexItor).second);
		m_mapTexture.erase(pKey);
	}
	return TRUE;
}
CTexture* CTextureMng::AddTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader)
{
	CTexture* pTexture = NULL;
	MapTexItor mapTexItor;

	mapTexItor = m_mapTexture.find(pFileName);
	if (mapTexItor != m_mapTexture.end())
		return (*mapTexItor).second;
	pTexture = new CTexture;
	if (pTexture->LoadTexture(pd3dDevice, pFileName, d3dKeyColor, bMyLoader))
	{
		m_mapTexture.insert(MapTexType(pFileName, pTexture));
		return pTexture;
	}
	SAFE_DELETE(pTexture);
	m_mapTexture.insert(MapTexType(pFileName, NULL));
	return NULL;
}
CTexture* CTextureMng::AddTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pKey, CTexture* pTexture)
{
	MapTexItor mapTexItor = m_mapTexture.find(pKey);
	if (mapTexItor != m_mapTexture.end())
		return (*mapTexItor).second;
	m_mapTexture.insert(MapTexType(pKey, pTexture));
	return pTexture;
}
CTexture* CTextureMng::GetAt(LPCTSTR pFileName)
{
	MapTexItor mapTexItor = m_mapTexture.find(pFileName);
	if (mapTexItor != m_mapTexture.end())
		return (CTexture*)(*mapTexItor).second;
	return NULL;
}


#ifdef __CLIENT
#ifndef __VM_0820
MemPooler<CDamageNum>* CDamageNum::m_pPool = new MemPooler<CDamageNum>(32);
#endif	// __VM_0820

void CDamageNum::Process()
{
	switch (m_nState)
	{
	case 0:	// init
	{
		D3DXVECTOR3 vTemp;
		g_DamageNumMng.m_viewport.X = 0;
		g_DamageNumMng.m_viewport.Y = 0;
		D3DXVec3Project(&vTemp, &(m_vPos + D3DXVECTOR3(0, 1.0f, 0)), &g_DamageNumMng.m_viewport, &g_DamageNumMng.m_matProj, &g_DamageNumMng.m_matView, &g_DamageNumMng.m_matWorld);
		m_fY = vTemp.y;
		m_fDy = 5.0f;
		m_nState++;
	}
	break;
	case 1:
		//		m_fDy -= 2.0f;
		if (m_nCnt >= 8)
		{
			m_nState++;
			m_nCnt = 0;
			m_fDy = 0;
		}
		break;
	case 2:
		if (g_nRenderCnt & 1)
			m_fDy = -3.0f;
		else
			m_fDy = 3.0f;

		if (m_nCnt >= 10)
		{
			m_nState++;
			m_fDy = 0;
			m_nCnt = 0;
		}
		break;
	case 3:
		if (m_nCnt >= 15)
		{
			m_nCnt = 0;
			m_fDy = 2.0f;
			m_nState++;
		}
		break;
	case 4:
		break;


	}

	m_fY -= m_fDy;
	m_nCnt++;

	m_nFrame++;
}

void CDamageNum::Render(CTexturePack* textPackNum)
{

	FLOAT fX, fY;
	D3DXVECTOR3 vTemp;
	D3DXVec3TransformCoord(&vTemp, &m_vPos, &g_DamageNumMng.m_matView);
	if (vTemp.z < 0)	return;
	g_DamageNumMng.m_viewport.X = 0;
	g_DamageNumMng.m_viewport.Y = 0;
	D3DXVec3Project(&vTemp, &(m_vPos + D3DXVECTOR3(0, 1.0f, 0)), &g_DamageNumMng.m_viewport, &g_DamageNumMng.m_matProj, &g_DamageNumMng.m_matView, &g_DamageNumMng.m_matWorld);

	fX = vTemp.x;

	fY = m_fY;
	FLOAT fScaleX = 2.0f;
	FLOAT fScaleY = 2.0f;
	int nAlpha = 255;
	if (m_nState == 4)
	{
		fScaleX = fScaleY = 3.0f + (m_nCnt * 0.05f);
		nAlpha = 75 - (m_nCnt * 2);
		if (nAlpha < 0)	nAlpha = 0;
	}

	fScaleX /= 4;
	fScaleY /= 4;

	if (m_nAttribute < 4)
	{
		char strTemp[64];
		sprintf(strTemp, "%d", m_nNumber);
		int nLength = strlen(strTemp);
		fX -= fScaleX * textPackNum->m_ap2DTexture[0].m_size.cx * (1.0f + 0.5f * (nLength - 1)) / 2.0f;

		const int spacePosition = nLength % 3;
		for (int i = 0; i < nLength; ++i)
		{
			const unsigned long nIndex = strTemp[i] - '0' + m_nAttribute * 16;
			if (static_cast<int>(nIndex) >= 0)
			{
				if (i % 3 == spacePosition && i)
				{
					textPackNum->Render(&g_Neuz.m_2DRender, CPoint(static_cast<int>(fX) + 1.0f, static_cast<int>(fY) - 2.0f), m_nAttribute * 16 + 11, static_cast<DWORD>(nAlpha), fScaleX, fScaleY);
					fX = static_cast<FLOAT>(fX + (textPackNum->m_ap2DTexture[m_nAttribute * 16 + 11].m_size.cx * 0.4 * fScaleX));
				}
				textPackNum->Render(&g_Neuz.m_2DRender, CPoint(static_cast<int>(fX), static_cast<int>(fY)), nIndex, static_cast<DWORD>(nAlpha), fScaleX, fScaleY);
				fX = static_cast<FLOAT>(fX + (textPackNum->m_ap2DTexture[nIndex].m_size.cx * 0.5 * fScaleX));
			}
		}
	}
	else if (m_nAttribute == 4)
	{
		fX = (FLOAT)(fX - (textPackNum->m_ap2DTexture[10].m_size.cx * 0.5 / 2 * fScaleX));
		textPackNum->Render(&g_Neuz.m_2DRender, CPoint((int)(fX), (int)(fY)), 10, nAlpha, fScaleX, fScaleY);
	}
	else if (m_nAttribute == 5)
	{
		fX = (FLOAT)(fX - (textPackNum->m_ap2DTexture[11].m_size.cx * 0.5 / 2 * fScaleX));
		textPackNum->Render(&g_Neuz.m_2DRender, CPoint((int)(fX), (int)(fY)), 11, nAlpha, fScaleX, fScaleY);
	}
	else if (m_nAttribute == 6)
	{
		fX = (FLOAT)(fX - (textPackNum->m_ap2DTexture[12].m_size.cx * 0.5 / 2 * fScaleX));
		textPackNum->Render(&g_Neuz.m_2DRender, CPoint((int)(fX), (int)(fY)), 12, nAlpha, fScaleX, fScaleY);
	}
	else if (m_nAttribute == 7)
	{
		fX = (FLOAT)(fX - (textPackNum->m_ap2DTexture[13].m_size.cx * 0.5 / 2 * fScaleX));
		textPackNum->Render(&g_Neuz.m_2DRender, CPoint((int)(fX), (int)(fY)), 13, nAlpha, fScaleX, fScaleY);
	}

	m_nFrame++;

}

void CDamageNumMng::LoadTexture(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_textPackNum.LoadScript(pd3dDevice, MakePath(DIR_SFX, _T("DmgEffect.inc")));
}

CDamageNumMng::~CDamageNumMng()
{
	DeleteDeviceObjects();
}
BOOL CDamageNumMng::DeleteDeviceObjects()
{
	CDamageNum* pDamagenum;
	for (int i = 0; i < m_Array.GetSize(); i++)
	{
		pDamagenum = (CDamageNum*)m_Array.GetAt(i);
		SAFE_DELETE(pDamagenum);
	}
	m_Array.RemoveAll();
	m_textPackNum.DeleteDeviceObjects();
	return TRUE;
}
void CDamageNumMng::AddNumber(D3DXVECTOR3 vPos, DWORD nNumber, DWORD nAttribute)
{
	CDamageNum* pDamagenum = new CDamageNum(vPos, nNumber, nAttribute);
	m_Array.Add(pDamagenum);
}

void CDamageNumMng::Process()
{
	CDamageNum* pDamagenum;

	for (int i = 0; i < m_Array.GetSize(); i++)
	{
		pDamagenum = (CDamageNum*)m_Array.GetAt(i);
		if (pDamagenum->m_nFrame >= 100)
		{
			SAFE_DELETE(pDamagenum);
			m_Array.RemoveAt(i);
			i--;
		}
		else
		{
			pDamagenum->Process();
		}
	}
}
void CDamageNumMng::Render()
{
	CDamageNum* pDamagenum;

	for (int i = 0; i < m_Array.GetSize(); i++)
	{
		pDamagenum = (CDamageNum*)m_Array.GetAt(i);
		pDamagenum->Render(&m_textPackNum);
	}
}
#endif


















