// Theme.cpp: implementation of the CWndBase class.

////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include  "version.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013


char		g_szVersion[128];	// file scope global

CTheme::CTheme()
{
	m_pVBTexture = NULL;
	m_pVBGauge = NULL;
	m_bNudeSkin = FALSE;
	m_pd3dDevice = NULL;

	m_pFontStatus = NULL;
}
CTheme::~CTheme()
{
	DeleteTheme();
}
BOOL CTheme::LoadTheme(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR lpszFileName)
{
	m_pd3dDevice = pd3dDevice;

	CScanner scanner;
	if (scanner.Load(MakePath(DIR_THEME, lpszFileName)) == FALSE)
		return FALSE;

	scanner.GetToken();
	while (scanner.tok != FINISHED)
	{
		if (scanner.Token == "m_d3dcBackground")
		{
			scanner.GetToken();
			m_d3dcBackground = scanner.GetHex();
		}
		else
			if (scanner.Token == "m_texWallPaper")
			{
				scanner.GetToken();
				scanner.GetToken();

				m_texWallPaper.LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, scanner.token), 0xff0000);
				m_texWndPaper.LoadTexture(m_pd3dDevice, MakePath(DIR_THEME, "WindField.bmp"), 0xff0000);
			}
			else
				if (scanner.Token == "m_dwWallPaperType")
				{
					scanner.GetToken();
					m_dwWallPaperType = scanner.GetNumber();
				}
		scanner.GetToken();
	}

	PLANG_DATA pLangData = CLangMan::GetInstance()->GetLangData(::GetLanguage());
	CD3DFont* pFont;
	pFont = new CD3DFont(pLangData->font.afi[0].szFont, 9);
	m_mapFont.SetAt(_T("gulim9"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[1].szFont, 8);
	m_mapFont.SetAt(_T("gulim8"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[2].szFont, 13);
	m_mapFont.SetAt(_T("gulim13"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[3].szFont, 9);
	pFont->m_nOutLine = pLangData->font.afi[3].nOutLine;
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[3].dwBgColor;
	m_mapFont.SetAt(_T("Arial Black9"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[4].szFont, 9);
	pFont->m_nOutLine = pLangData->font.afi[4].nOutLine;
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[4].dwBgColor;
	m_mapFont.SetAt(_T("FontWorld"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[5].szFont, 15);
	pFont->m_nOutLine = pLangData->font.afi[5].nOutLine;
	pFont->m_dwColor = 0xffffffff;
	pFont->m_dwBgColor = pLangData->font.afi[5].dwBgColor;;
	pFont->m_dwFlags = pLangData->font.afi[5].dwFlags;
	m_mapFont.SetAt(_T("gulim20"), pFont);
#if __VER >= 12 // __SECRET_ROOM
	pFont = new CD3DFont(pLangData->font.afi[2].szFont, 11, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	m_mapFont.SetAt(_T("gulim11"), pFont);
	pFont = new CD3DFont(pLangData->font.afi[2].szFont, 9, D3DFONT_BOLD);
	pFont->m_nOutLine = 1;
	m_mapFont.SetAt(_T("gulim9_2"), pFont);
#endif //__SECRET_ROOM

	pFont = new CD3DFont(_T("Tahoma"), 9);
	m_mapFont.SetAt(_T("FontSomeText2"), pFont);

#ifdef __FL_CASH_SHOP
	pFont = new CD3DFont(_T("Tahoma"), 8, D3DFONT_BOLD);
	pFont->m_nOutLine = 2;
	m_mapFont.SetAt(_T("FontItemNumbers"), pFont);

	pFont = new CD3DFont(_T("Tahoma"), 9, D3DFONT_BOLD);
	m_mapFont.SetAt(_T("FontSomeText4"), pFont);
#endif

#ifdef __FL_UPDATE_TASKBAR
	pFont = new CD3DFont(_T("Tahoma"), 8, D3DFONT_BOLD);
	pFont->m_nOutLine = 2;
	pFont->m_dwBgColor = 0xffd46946;
	m_mapFont.SetAt(_T("FontTaskbarKeys"), pFont);
#endif

	SetVersion(::GetLanguage());


	m_mapFont.Lookup(_T("gulim9"), (void*&)m_pFontChat);
	m_mapFont.Lookup(_T("gulim9"), (void*&)m_pFontText);
	m_mapFont.Lookup(_T("gulim8"), (void*&)m_pFontStatus);
	m_mapFont.Lookup(_T("Arial Black9"), (void*&)m_pFontWndTitle);
	m_mapFont.Lookup(_T("FontWorld"), (void*&)m_pFontWorld);
	m_mapFont.Lookup(_T("gulim20"), (void*&)m_pFontCaption);
	m_mapFont.Lookup(_T("gulim13"), (void*&)m_pFontGuildCombatText);
#if __VER >= 12 // __SECRET_ROOM
	m_mapFont.Lookup(_T("gulim11"), (void*&)m_pFontSRMyGiuld);
	m_mapFont.Lookup(_T("gulim9_2"), (void*&)m_pFontSRGiuld);
#endif //__SECRET_ROOM

	m_mapFont.Lookup(_T("FontSomeText2"), (void*&)m_pFontSomeText2);

#ifdef __FL_CASH_SHOP
	m_mapFont.Lookup(_T("FontItemNumbers"), (void*&)m_pFontItemNumbers);
	m_mapFont.Lookup(_T("FontSomeText4"), (void*&)m_pFontSomeText4);
#endif

#ifdef __FL_UPDATE_TASKBAR
	m_mapFont.Lookup(_T("FontTaskbarKeys"), (void*&)m_pFontTaskbarKeys);
#endif

	return TRUE;
}
BOOL CTheme::SaveTheme(LPCTSTR lpszFileName)
{
	return TRUE;
}
void CTheme::DeleteTheme()
{
	DeleteDeviceObjects();
}
HRESULT CTheme::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while (pos)
	{
		m_mapFont.GetNextAssoc(pos, strFont, (void*&)pFont);
		pFont->InitDeviceObjects(pd3dDevice);
	}
	//	g_mesh.InitDeviceObjects( m_pd3dDevice );
		//g_mesh.OpenMesh( _T("obj_rideferriswheel.mes") );

	//	g_mesh.OpenMesh( _T("obj_starship.mes") );

	return TRUE;//m_pFontGameTitle->InitDeviceObjects( pd3dDevice );
}
HRESULT CTheme::InvalidateDeviceObjects()
{
#ifdef __YDEBUG
	m_texWallPaper.Invalidate();
	m_texWndPaper.Invalidate();
#endif //__YDEBUG

	HRESULT h = S_OK;
	SAFE_RELEASE(m_pVBTexture);
	SAFE_RELEASE(m_pVBGauge);
	//g_mesh.InvalidateDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while (pos)
	{
		m_mapFont.GetNextAssoc(pos, strFont, (void*&)pFont);
		pFont->InvalidateDeviceObjects();
	}
	return h;
}
HRESULT CTheme::DeleteDeviceObjects()
{
	HRESULT h = S_OK;
	m_texWallPaper.DeleteDeviceObjects();
	m_texWndPaper.DeleteDeviceObjects();
	//g_mesh.DeleteDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while (pos)
	{
		m_mapFont.GetNextAssoc(pos, strFont, (void*&)pFont);
		pFont->DeleteDeviceObjects();
		SAFE_DELETE(pFont);
	}
	m_mapFont.RemoveAll();

	return h;
}
HRESULT CTheme::RestoreDeviceObjects()
{
#ifdef __YDEBUG
	m_texWallPaper.SetInvalidate(m_pd3dDevice);
	m_texWndPaper.SetInvalidate(m_pd3dDevice);
#endif //__YDEBUG

	if (m_pVBTexture)
		return S_OK;
	//g_mesh.RestoreDeviceObjects();
	POSITION pos = m_mapFont.GetStartPosition();
	CString strFont; CD3DFont* pFont;
	while (pos)
	{
		m_mapFont.GetNextAssoc(pos, strFont, (void*&)pFont);
		pFont->RestoreDeviceObjects();
	}
	HRESULT hr = S_OK;
	m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX) * 24, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBTexture, NULL);
	m_pd3dDevice->CreateVertexBuffer(sizeof(DRAWVERTEX) * 42, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_DRAWVERTEX, D3DPOOL_SYSTEMMEM, &m_pVBGauge, NULL);
	return hr;
}

HRESULT CTheme::FrameMove()
{
	return S_OK;
}
void CTheme::RenderTitle(C2DRender* p2DRender)
{
}
void CTheme::RenderDesktop(C2DRender* p2DRender)
{
	int xOffset = 0;
	int rectWidth = 0;
	CTexture texture = m_texWallPaper;
	texture.SetAutoFree(FALSE);
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	CRect rectWindow = p2DRender->m_clipRect;
	texture.m_size.cx = rectWindow.Width();
	texture.m_size.cy = rectWindow.Height();
	p2DRender->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_d3dcBackground, 1.0f, 0);
	p2DRender->RenderTexture(CPoint(xOffset, 0), &texture);
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	p2DRender->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}


void CTheme::GradationRect(C2DRender* p2DRender, CRect* pRect, DWORD dwColor1t, DWORD dwColor1b, DWORD dwColor2b, int nMidPercent)
{
	int nFirstHeight = pRect->Height() * nMidPercent / 100;

	CRect rect1 = *pRect; rect1.bottom = rect1.top + nFirstHeight;
	CRect rect2 = *pRect; rect2.top = rect2.top + nFirstHeight;

	p2DRender->RenderFillRect(rect1, dwColor1t, dwColor1t, dwColor1b, dwColor1b);
	p2DRender->RenderFillRect(rect2, dwColor1b, dwColor1b, dwColor2b, dwColor2b);
}
void CTheme::RenderWndBaseTitleBar(C2DRender* p2DRender, CRect* pRect, LPCTSTR lpszTitle, DWORD dwColor)
{
	//DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );
	//DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );
	//DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );
	DWORD dwColor1 = D3DCOLOR_ARGB(255, 255, 255, 255);//D3DCOLOR_ARGB( 255, 130, 130, 230 );
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 150, 150, 150);//D3DCOLOR_ARGB( 255,  50,  50, 100 );
	DWORD dwColor3 = D3DCOLOR_ARGB(255, 230, 230, 230);//D3DCOLOR_ARGB( 255, 180, 180, 220 );

	GradationRect(p2DRender, pRect, dwColor1, dwColor2, dwColor3);
	//p2DRender->RenderLine( CPoint( pRect->left, pRect->top ), CPoint( pRect->right, pRect->top ), dwColor3 );

	p2DRender->TextOut(17, 7, lpszTitle, dwColor);
	//p2DRender->TextOut( 16, 6, lpszTitle, 0xffffffff);


}
void CTheme::RenderWndBaseFrame(C2DRender* p2DRender, CRect* pRect)
{
	//DWORD dwColor1 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );
	//DWORD dwColor2 = D3DCOLOR_ARGB( 100,  80,  80,  80 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );
	//DWORD dwColor3 = D3DCOLOR_ARGB( 150, 160, 160, 160 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );
	//DWORD dwColor4 = D3DCOLOR_ARGB( 100,   0,   0,   0 );//D3DCOLOR_ARGB( 255,  50,  50, 150 );
	DWORD dwColor1 = D3DCOLOR_ARGB(255, 0, 0, 0);//D3DCOLOR_ARGB( 255,  50,  50, 100 );
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 80, 80, 80);//D3DCOLOR_ARGB( 255, 180, 180, 220 );
	DWORD dwColor3 = D3DCOLOR_ARGB(255, 160, 160, 160);//D3DCOLOR_ARGB( 255, 180, 180, 220 );
	DWORD dwColor4 = D3DCOLOR_ARGB(255, 0, 0, 0);//D3DCOLOR_ARGB( 255,  50,  50, 150 );

	//CRect rectTemp = *pRect;
	//rectTemp.DeflateRect(2,2);
	//rectTemp.bottom = 20;
	if (m_bNudeSkin == FALSE)
	{
		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree(FALSE);

		FLOAT fu, fv;


		//	p2DRender->RenderTexture( CPoint( 0, 0), &texture, 0 );

			////////////////////////////

		CRect rect, rectOrg = *pRect;
		rectOrg += p2DRender->m_ptOrigin;


		TEXTUREVERTEX* pVertices;
		m_pVBTexture->Lock(0, sizeof(TEXTUREVERTEX) * 24, (void**)&pVertices, 0);

		// horizon

		rect.SetRect(rectOrg.left, rectOrg.top, rectOrg.right, rectOrg.top + 16 + 4);
		fu = (FLOAT)rect.Width() / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
		texture.m_fuRB = fu; texture.m_fvRB = fv;

		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.top, texture.m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;

		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.bottom, texture.m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture = m_texWndPaper;
		texture.SetAutoFree(FALSE);
		rect.SetRect(rectOrg.left, rectOrg.bottom - 4, rectOrg.right, rectOrg.bottom);
		fu = (FLOAT)rect.Width() / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
		texture.m_fuRB = fu; texture.m_fvRB = fv;

		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.top, texture.m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;

		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.bottom, texture.m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		// vertical
		texture = m_texWndPaper;
		texture.SetAutoFree(FALSE);
		rect.SetRect(rectOrg.left, rectOrg.top + 16 + 4, rectOrg.left + 4, rectOrg.bottom - 4);
		fu = (FLOAT)rect.Width() / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
		texture.m_fuRB = fu; texture.m_fvRB = fv;

		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.top, texture.m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;

		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.bottom, texture.m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		texture = m_texWndPaper;
		texture.SetAutoFree(FALSE);
		rect.SetRect(rectOrg.right - 4, rectOrg.top + 16 + 4, rectOrg.right, rectOrg.bottom - 4);
		fu = (FLOAT)rect.Width() / texture.m_size.cx;
		fv = (FLOAT)rect.Height() / texture.m_size.cy;
		texture.m_size.cx = rect.Width();
		texture.m_size.cy = rect.Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
		texture.m_fuRB = fu; texture.m_fvRB = fv;


		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.top, texture.m_fuLT + 0.000001f, texture.m_fvLT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;

		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.top, texture.m_fuRT + 0.000001f, texture.m_fvRT + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.left, (FLOAT)rect.bottom, texture.m_fuLB + 0.000001f, texture.m_fvLB + 0.000001f); pVertices++;
		SetTextureVertex(pVertices, (FLOAT)rect.right, (FLOAT)rect.bottom, texture.m_fuRB + 0.000001f, texture.m_fvRB + 0.000001f); pVertices++;

		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLT + 0.000001f; pVertices->v = texture.m_fvLT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;

		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.top   ; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRT + 0.000001f; pVertices->v = texture.m_fvRT + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.left ; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuLB + 0.000001f; pVertices->v = texture.m_fvLB + 0.000001f; pVertices++;
		//pVertices->x = (FLOAT)rect.right; pVertices->y = (FLOAT)rect.bottom; pVertices->z = 0.0f; pVertices->rhw = 1.0f; pVertices->u = texture.m_fuRB + 0.000001f; pVertices->v = texture.m_fvRB + 0.000001f; pVertices++;

		m_pVBTexture->Unlock();


		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, 1 );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_NONE );
		//p2DRender->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_NONE );
		p2DRender->m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		p2DRender->m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		p2DRender->m_pd3dDevice->SetTexture(0, texture.m_pTexture);
		p2DRender->m_pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX);
		p2DRender->m_pd3dDevice->SetStreamSource(0, m_pVBTexture, 0, sizeof(TEXTUREVERTEX));
		p2DRender->m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 8);


	}
	/////////////////////////////////////////
//return;

	p2DRender->RenderRoundRect(*pRect, dwColor1);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderRoundRect(*pRect, dwColor3);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderRect(*pRect, 0xffffffff);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderRect(*pRect, dwColor3);


	/*
	p2DRender->RenderRoundRect( *pRect, dwColor1 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor2 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor3 );
	pRect->DeflateRect(1,1);
	p2DRender->RenderRoundRect( *pRect, dwColor4 );
	*/
}
void CTheme::RenderEdge(C2DRender* p2DRender, CRect* pRect, BOOL bClient)
{
	DWORD dwColor1 = D3DCOLOR_ARGB(100, 0, 0, 0);//D3DCOLOR_TEMP( 255,   0,   0,  50 );
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 240, 240, 240);//D3DCOLOR_TEMP( 255,  80,  80, 120 );
	DWORD dwColor3 = D3DCOLOR_ARGB(100, 200, 200, 200);//D3DCOLOR_TEMP( 255,  80,  80, 120 );

	if (bClient)
		p2DRender->RenderFillRect(*pRect, dwColor1);
	p2DRender->RenderRoundRect(*pRect, dwColor2);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderRect(*pRect, dwColor2);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderRect(*pRect, dwColor3);

}
void CTheme::RenderWndBaseBkgr(C2DRender* p2DRender, CRect* pRect)
{
	if (m_bNudeSkin == FALSE)
	{

		DWORD dwColor1t = D3DCOLOR_ARGB(155, 100, 100, 100);//D3DCOLOR_ARGB( 255, 250, 250, 255 );
		DWORD dwColor1b = D3DCOLOR_ARGB(155, 70, 70, 70);//D3DCOLOR_ARGB( 255, 200, 200, 210 );
		DWORD dwColor2b = D3DCOLOR_ARGB(155, 43, 73, 45);//D3DCOLOR_ARGB( 255, 143, 173, 245 );
		//adationRect( p2DRender, pRect, dwColor1t, dwColor1b, dwColor2b );

		CTexture texture
			= m_texWndPaper;
		texture.SetAutoFree(FALSE);

		FLOAT fu = (FLOAT)pRect->Width() / texture.m_size.cx;
		FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
		texture.m_size.cx = pRect->Width();
		texture.m_size.cy = pRect->Height();
		texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
		texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
		texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
		texture.m_fuRB = fu; texture.m_fvRB = fv;
		p2DRender->RenderTexture(CPoint(0, 0), &texture);
		//	p2DRender->RenderFillRect( *pRect, 0xa0ffffff );
	}
	else
	{
		p2DRender->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		p2DRender->RenderFillRect(*pRect, D3DCOLOR_ARGB(100, 0, 0, 0));
	}
}
void CTheme::RenderWndEditFrame(C2DRender* p2DRender, CRect* pRect)
{
	DWORD dwColor1 = D3DCOLOR_ARGB(255, 200, 200, 200);//D3DCOLOR_ARGB(255,200,200,250);
	DWORD dwColor2 = D3DCOLOR_ARGB(255, 255, 255, 255);//D3DCOLOR_ARGB(255,150,150,200);

	p2DRender->RenderRoundRect(*pRect, dwColor1);
	pRect->DeflateRect(1, 1);
	p2DRender->RenderFillRect(*pRect, dwColor2);
}
void CTheme::RenderWndTextFrame(C2DRender* p2DRender, CRect* pRect)
{
	//DWORD dwColor1 = D3DCOLOR_ARGB(  88,  50,  50,  50 );//D3DCOLOR_ARGB( 255,   0,   0,  50 );
	DWORD dwColor1 = D3DCOLOR_ARGB(CWndBase::m_nAlpha - 32, 255, 255, 255);//D3DCOLOR_ARGB( 255,   0,   0,  50 );
	DWORD dwColor2 = D3DCOLOR_ARGB(CWndBase::m_nAlpha - 32, 226, 198, 181);//D3DCOLOR_ARGB( 255,  80,  80, 120 );

	p2DRender->RenderFillRect(*pRect, dwColor1);
	p2DRender->RenderRoundRect(*pRect, dwColor2);

}
////////////////////

DWORD CTheme::GetButtonFontColor(CWndButton* pWndButton)
{
	if (pWndButton->IsWindowEnabled())
	{
		if (pWndButton->IsHighLight())
			return pWndButton->GetHighLightColor();
		return pWndButton->IsPush() ? pWndButton->GetPushColor() : pWndButton->GetFontColor();
	}
	return pWndButton->GetDisableColor();
}
POINT CTheme::GetButtonTextPos(C2DRender* p2DRender, CWndButton* pWndButton)
{
	if (!(pWndButton->GetStyle() & WBS_NOCENTER))
		return pWndButton->GetStrCenter(p2DRender, pWndButton->GetTitle());
	return CPoint(0, 0);
}
void CTheme::RenderWndButton(C2DRender* p2DRender, CWndButton* pWndButton)
{
	int   nCheck = pWndButton->GetCheck();
	BOOL  bHighLight = pWndButton->IsHighLight();
	BOOL  bEnable = pWndButton->IsWindowEnabled();
	BOOL  bPush = pWndButton->IsPush();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	POINT pt = GetButtonTextPos(p2DRender, pWndButton);

	//#ifdef __NEWINTERFACE
	if (pWndButton->m_pTexture)
	{
		RenderWndButton_4Texture(p2DRender, pWndButton);
		return;
	}
	if (nCheck)
	{
		dwColor = D3DCOLOR_ARGB(0, 0, 0, 0);

	}
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 200, 200, 250 ) : D3DCOLOR_ARGB( 255, 150, 150, 250 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB( 255, 130, 130, 200 ) : D3DCOLOR_ARGB( 255,  30,  30, 100 ) );
	//DWORD dwColor1 = ( bHighLight ?	D3DCOLOR_ARGB( 100, 100, 100, 150 ) : D3DCOLOR_ARGB( 200,  200,  200, 200 ) );
	//DWORD dwColor2 = ( bHighLight ?	D3DCOLOR_ARGB(  55,  30,  30, 100 ) : D3DCOLOR_ARGB(  55,   0,   0,  00 ) );


	//DWORD dwColor1 = D3DCOLOR_ARGB( 250, 255, 255, 255 );//D3DCOLOR_ARGB( 255, 130, 130, 230 );
	//DWORD dwColor2 = D3DCOLOR_ARGB( 50,    0,   0,  00 );//D3DCOLOR_ARGB( 255,  50,  50, 100 );
	//DWORD dwColor3 = D3DCOLOR_ARGB( 200, 150, 150, 150 );//D3DCOLOR_ARGB( 255, 180, 180, 220 );

	DWORD dwColor1 = (bPush ? D3DCOLOR_ARGB(255, 100, 255, 255) : D3DCOLOR_ARGB(250, 255, 255, 255));
	DWORD dwColor2 = (bPush ? D3DCOLOR_ARGB(50, 0, 0, 0) : D3DCOLOR_ARGB(50, 0, 0, 0));
	DWORD dwColor3 = (bPush ? D3DCOLOR_ARGB(200, 0, 150, 150) : D3DCOLOR_ARGB(200, 150, 150, 150));
	//	DWORD dwColor  = ( bHighLight ? D3DCOLOR_ARGB( 250, 0, 255, 255 ) : D3DCOLOR_ARGB( 250, 255, 255, 255 ) );

		//p2DRender->TextOut( 17, 7, lpszTitle, 0xff000000);
		//p2DRender->TextOut( 16, 6, lpszTitle, 0xffffffff);

	p2DRender->TextOut(pt.x + 1, pt.y + 1, pWndButton->GetTitle(), 0xff000000);
	p2DRender->TextOut(pt.x, pt.y, pWndButton->GetTitle(), dwColor);

	//	if( pWndButton->IsWndStyle( WBS_NODRAWFRAME ) )
	//		return;

	CRect rect = pWndButton->GetClientRect();
	rect.DeflateRect(2, 2);
	GradationRect(p2DRender, &rect, dwColor1, dwColor2, dwColor3);
	p2DRender->RenderLine(CPoint(rect.left, rect.top), CPoint(rect.right, rect.top), dwColor3);

	//CRect rect = pWndButton->GetClientRect();
	rect.InflateRect(1, 1);
	p2DRender->RenderRoundRect(rect, D3DCOLOR_ARGB(155, 200, 200, 200));
	rect.InflateRect(1, 1);
	p2DRender->RenderRoundRect(rect, D3DCOLOR_ARGB(155, 50, 50, 50));
	/*
		if( bPush )
			p2DRender->RenderFillRect( rect, dwColor2, dwColor2, dwColor1, dwColor1 );
		else
			p2DRender->RenderFillRect( rect, dwColor1, dwColor1, dwColor2, dwColor2 );
	*/
}
void CTheme::RenderWndButton_4Texture(C2DRender* p2DRender, CWndButton* pWndButton)
{
	int   nCheck = pWndButton->GetCheck();
	BOOL  bHighLight = pWndButton->IsHighLight();
	BOOL  bEnable = pWndButton->IsWindowEnabled();
	BOOL  bPush = pWndButton->IsPush();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if (pWndButton->m_pTexture == FALSE)
		return;

	CTexture* pTexture = pWndButton->m_pTexture;
	CSize sizeOld = pTexture->m_size;
	pTexture->m_size.cx /= 4;

	if (bEnable == FALSE)
	{
		pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
		pTexture->m_fvLT = 0.0f;

		pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
		pTexture->m_fvRT = 0.0f;

		pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
		pTexture->m_fvLB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;

		pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
		pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
	}
	else
		if (bPush == FALSE && nCheck == 0)
		{
			if (bHighLight)
			{
				pTexture->m_fuLT = 0.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = (FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = 0.0f;
				pTexture->m_fvLB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = (FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx;
				pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 1.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 1.0f;
				pTexture->m_fvLB = ((FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy);

				pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
				pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
		{

			pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
			pTexture->m_fvLT = 0.0f;

			pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
			pTexture->m_fvRT = 0.0f;

			pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
			pTexture->m_fvLB = ((FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy);

			pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
			pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;

		}
	pTexture->Render(p2DRender, CPoint(0, 0), pWndButton->m_nAlphaCount);
	pTexture->m_size = sizeOld;
}

void CTheme::RenderWndButton_6Texture(C2DRender* p2DRender, CWndButton* pWndButton)
{
	int   nCheck = pWndButton->GetCheck();
	BOOL  bHighLight = pWndButton->IsHighLight();
	BOOL  bEnable = pWndButton->IsWindowEnabled();
	BOOL  bPush = pWndButton->IsPush();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	if (pWndButton->m_pTexture)
	{
		CTexture* pTexture = pWndButton->m_pTexture;
		CSize sizeOld = pTexture->m_size;
		pTexture->m_size.cx /= 6;

		if (bEnable == FALSE)
		{
			if (bPush == FALSE && nCheck == 0)
			{
				pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 5.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
				pTexture->m_fvLB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 5.0f;
				pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
			else
			{
				pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 5.0f;
				pTexture->m_fvLT = 0.0f;

				pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 6.0f;
				pTexture->m_fvRT = 0.0f;

				pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 5.0f;
				pTexture->m_fvLB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;

				pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 6.0f;
				pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
			}
		}
		else
			if (bPush == FALSE && nCheck == 0)
			{
				if (bHighLight)
				{
					pTexture->m_fuLT = 0.0f;
					pTexture->m_fvLT = 0.0f;

					pTexture->m_fuRT = (FLOAT)pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
					pTexture->m_fvRT = 0.0f;

					pTexture->m_fuLB = 0.0f;
					pTexture->m_fvLB = (FLOAT)pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;

					pTexture->m_fuRB = (FLOAT)pTexture->m_size.cx / (FLOAT)pTexture->m_sizePitch.cx;
					pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / (FLOAT)pTexture->m_sizePitch.cy;
				}
				else
				{
					pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 1.0f;
					pTexture->m_fvLT = 0.0f;

					pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
					pTexture->m_fvRT = 0.0f;

					pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 1.0f;
					pTexture->m_fvLB = ((FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy);

					pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
					pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
				}
			}
			else
			{
				if (bHighLight)
				{
					pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
					pTexture->m_fvLT = 0.0f;

					pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
					pTexture->m_fvRT = 0.0f;

					pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 2.0f;
					pTexture->m_fvLB = ((FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy);

					pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
					pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
				}
				else
				{
					pTexture->m_fuLT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
					pTexture->m_fvLT = 0.0f;

					pTexture->m_fuRT = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
					pTexture->m_fvRT = 0.0f;

					pTexture->m_fuLB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 3.0f;
					pTexture->m_fvLB = ((FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy);

					pTexture->m_fuRB = ((FLOAT)pTexture->m_size.cx / pTexture->m_sizePitch.cx) * 4.0f;
					pTexture->m_fvRB = (FLOAT)pTexture->m_size.cy / pTexture->m_sizePitch.cy;
				}
			}
		pTexture->Render(p2DRender, CPoint(0, 0), pWndButton->m_nAlphaCount);
		pTexture->m_size = sizeOld;
	}

}

void CTheme::RenderWndButtonCheck(C2DRender* p2DRender, CWndButton* pWndButton)
{
	int nFontHeight = pWndButton->GetFontHeight();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	if (pWndButton->m_pTexture) // pWndButton->IsWndStyle( WBS_PUSHLIKE ) )
	{
		RenderWndButton_6Texture(p2DRender, pWndButton);
		p2DRender->TextOut(pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor);
	}
	else
	{
		BOOL  bHighLight = pWndButton->IsHighLight();
		BOOL  bEnable = pWndButton->IsWindowEnabled();
		BOOL  bPush = pWndButton->IsPush();
		//		POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

		int nFontColor = p2DRender->GetTextColor();

		p2DRender->RenderRect(CRect(0, 0, 10, 10), dwColor);
		if (pWndButton->GetCheck())
		{
			p2DRender->RenderLine(CPoint(2, 2), CPoint(5, 8), dwColor);
			p2DRender->RenderLine(CPoint(2, 2), CPoint(6, 8), dwColor);
			p2DRender->RenderLine(CPoint(5, 8), CPoint(8, 3), dwColor);
		}
		p2DRender->TextOut(nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor);
	}
}
void CTheme::RenderWndButtonRadio(C2DRender* p2DRender, CWndButton* pWndButton)
{
	BOOL  bHighLight = pWndButton->IsHighLight();
	BOOL  bEnable = pWndButton->IsWindowEnabled();
	BOOL  bPush = pWndButton->IsPush();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	//	POINT pt          = GetButtonTextPos( p2DRender, pWndButton );

	int nFontHeight = pWndButton->GetFontHeight();
	int nFontColor = p2DRender->GetTextColor();
	if (pWndButton->m_pTexture) // pWndButton->IsWndStyle( WBS_PUSHLIKE ) )
	{
		RenderWndButton_6Texture(p2DRender, pWndButton);
		p2DRender->TextOut(pWndButton->m_pTexture->m_size.cx / 6 + 2, p2DRender->m_clipRect.Height() / 2 - nFontHeight / 2, pWndButton->GetTitle(), dwColor);
	}
	else
	{
		CRect rect(0, 0, 10, 10);

		p2DRender->RenderRoundRect(rect, dwColor);
		if (pWndButton->GetCheck())
		{
			rect.DeflateRect(2, 2, 2, 2);
			p2DRender->RenderFillRect(rect, dwColor);
		}
		p2DRender->TextOut(nFontHeight + 5, 0, pWndButton->GetTitle(), dwColor);
	}
	/*
	if(m_bCheck && m_bEnable == TRUE)
		;//p2DRender->PaintImage(lpRadioBitmap2,CPtSz(1,1,14,14),0);
	else
		;//p2DRender->PaintImage(lpRadioBitmap1,CPtSz(1,1,14,14),0);
	p2DRender->TextOut( nFontHeight, 0, m_strTitle );
	*/

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTheme::RenderWndButtonText(C2DRender* p2DRender, CWndButton* pWndButton)
{
	BOOL  bHighLight = pWndButton->IsHighLight();
	BOOL  bEnable = pWndButton->IsWindowEnabled();
	BOOL  bPush = pWndButton->IsPush();
	DWORD dwColor = GetButtonFontColor(pWndButton);
	POINT pt = GetButtonTextPos(p2DRender, pWndButton);

	p2DRender->TextOut(pt.x, pt.y, pWndButton->GetTitle(), dwColor);
}

void CTheme::RenderWndTaskBar(C2DRender* p2DRender, CRect* pRect)
{
	CRect rect = *pRect;

	CTexture texture
		= m_texWndPaper;
	texture.SetAutoFree(FALSE);

	FLOAT fu = (FLOAT)pRect->Width() / texture.m_size.cx;
	FLOAT fv = (FLOAT)pRect->Height() / texture.m_size.cy;
	texture.m_size.cx = pRect->Width();
	texture.m_size.cy = pRect->Height();
	texture.m_fuLT = 0.0f; texture.m_fvLT = 0.0f;
	texture.m_fuRT = fu; texture.m_fvRT = 0.0f;
	texture.m_fuLB = 0.0f; texture.m_fvLB = fv;
	texture.m_fuRB = fu; texture.m_fvRB = fv;
	p2DRender->RenderTexture(CPoint(0, 0), &texture);

	DWORD dwColor0 = D3DCOLOR_ARGB(150, 100, 100, 100);
	DWORD dwColor1 = D3DCOLOR_ARGB(250, 255, 255, 255);
	DWORD dwColor2 = D3DCOLOR_ARGB(50, 0, 0, 00);
	DWORD dwColor3 = D3DCOLOR_ARGB(200, 150, 150, 150);

	GradationRect(p2DRender, &rect, dwColor1, dwColor2, dwColor3, 40);

	rect = *pRect;//GetWindowRect();
	rect.DeflateRect(3, 3);
	rect.left = 65;
	rect.right -= 90;
	rect.DeflateRect(1, 1);

}
void CTheme::RenderWndMenuTask(C2DRender* p2DRender, CRect* pRect)
{
}
void CTheme::RenderWndMenu(C2DRender* p2DRender, CRect* pRect)
{
	RenderWndBaseFrame(p2DRender, pRect);
}
void CTheme::RenderWndMenuItem(C2DRender* p2DRender, CWndButton* pWndButton)
{
}
BOOL CTheme::MakeGaugeVertex(LPDIRECT3DDEVICE9 pd3dDevice, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture)
{
	CPoint pt = pRect->TopLeft();
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	/////////////////////////////////////////////////////////
	int nTexWidth = (pTexture->m_size.cx) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = (FLOAT)(pt.x + nTexWidth);//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)(pt.y + nTexHeight);//( pTexture->m_size.cy );

	int nWidth = (pRect->Width() / nTexWidth);// - 2;


	if (nWidth < 2)
		return FALSE;

	int nTileNum = 3;
	int nVertexNum = 3 * 6;

	TEXTUREVERTEX2* pVertices, * pVertices_;
	HRESULT hr = pVB->Lock(0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**)&pVertices_, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return FALSE;
	{
		SIZE size = pTexture->m_size;
		SIZE sizePitch = pTexture->m_sizePitch;
		size.cx /= 3;

		pVertices = pVertices_;
		for (int i = 0; i < nVertexNum; i++)
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left = (FLOAT)(pt.x);
		right = (FLOAT)(pt.x + nTexWidth);

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		////////////////////////////

		left = (FLOAT)(pt.x + nTexWidth);
		right = (FLOAT)(pt.x + nTexWidth + ((nWidth - 2) * nTexWidth));

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		////////////////////////////
		left = (FLOAT)(pt.x + ((nWidth - 1) * nTexWidth));
		right = (FLOAT)(pt.x + ((nWidth)*nTexWidth));

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

	}
	pVB->Unlock();
	return TRUE;
}
void CTheme::RenderGauge(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture)
{
	/////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif


	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	int nVertexNum = 3 * 6;
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(TEXTUREVERTEX2));
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nVertexNum / 3);
}

void CTheme::RenderGauge(C2DRender* p2DRender, CRect* pRect, DWORD dwColor, LPDIRECT3DVERTEXBUFFER9 pVB, CTexture* pTexture)
{
	//	pTexture = &m_texGauEmptyNormal;
	LPDIRECT3DDEVICE9 pd3dDevice = p2DRender->m_pd3dDevice;
	CPoint pt = pRect->TopLeft();
	pt += p2DRender->m_ptOrigin;
	CPoint ptCenter = pTexture->m_ptCenter;
	pt -= ptCenter;

	/////////////////////////////////////////////////////////
	int nTexWidth = (pTexture->m_size.cx) / 3;
	int nTexHeight = pTexture->m_size.cy;

	FLOAT left = (FLOAT)(pt.x);
	FLOAT top = (FLOAT)(pt.y);
	FLOAT right = (FLOAT)(pt.x + nTexWidth);//( pTexture->m_size.cx );
	FLOAT bottom = (FLOAT)(pt.y + nTexHeight);//( pTexture->m_size.cy );

	int nWidth = (pRect->Width() / nTexWidth);// - 2;


	if (nWidth < 2)
		return;

	int nTileNum = 3;
	int nVertexNum = 3 * 6;

	TEXTUREVERTEX2* pVertices, * pVertices_;
	HRESULT hr = pVB->Lock(0, sizeof(TEXTUREVERTEX2) * nVertexNum, (void**)&pVertices_, D3DLOCK_DISCARD);
	if (hr != D3D_OK) return;
	{
		SIZE size = pTexture->m_size;
		SIZE sizePitch = pTexture->m_sizePitch;
		size.cx /= 3;

		pVertices = pVertices_;
		for (int i = 0; i < nVertexNum; i++)
		{
			pVertices->vec.z = 0;
			pVertices->rhw = 1.0f;
			pVertices->color = dwColor;
			pVertices++;
		}
		pVertices = pVertices_;

		left = (FLOAT)(pt.x);
		right = (FLOAT)(pt.x + nTexWidth);

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = 0.0f;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = (FLOAT)size.cx / sizePitch.cx;
		pVertices->v = (FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		////////////////////////////

		left = (FLOAT)(pt.x + nTexWidth);
		right = (FLOAT)(pt.x + nTexWidth + ((nWidth - 2) * nTexWidth));

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 1.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		////////////////////////////
		left = (FLOAT)(pt.x + ((nWidth - 1) * nTexWidth));
		right = (FLOAT)(pt.x + ((nWidth)*nTexWidth));

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = 0.0f;//(FLOAT)size.cy / sizePitch.cy;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)top - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = 0.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)left - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 2.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

		pVertices->vec.x = (FLOAT)right - 0.5f;
		pVertices->vec.y = (FLOAT)bottom - 0.5f;
		pVertices->u = ((FLOAT)size.cx / sizePitch.cx) * 3.0f;
		pVertices->v = ((FLOAT)size.cy / sizePitch.cy) * 1.0f;
		pVertices++;

	}
	pVB->Unlock();

	/////////////////////////////////////////////////////////

	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, 1);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
#ifndef __TEST_ANISOTROPIC
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
#endif

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	//pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	//pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	//pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	//pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
	//	D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
	//	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	//pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetTexture(0, pTexture->m_pTexture);
	pd3dDevice->SetFVF(D3DFVF_TEXTUREVERTEX2);
	pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(TEXTUREVERTEX2));
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nVertexNum / 3);
}

void CTheme::SetVersion(int Lang)
{
	sprintf(g_szVersion, "%s %s %d", __DATE__, __TIME__, Lang);
}
