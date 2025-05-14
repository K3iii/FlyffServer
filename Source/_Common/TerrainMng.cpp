#include "stdafx.h"



CTerrainMng::CTerrainMng()
{
	m_aTerrain.clear();
	m_nWaterFrame = 0;
	m_pd3dDevice = NULL;
	m_nSize = 0;
#if __VER >= 14 // __WATER_EXT
	for(int i=0; i<MAX_WATER; i++)
		m_fWaterFrame[i] = 0.0f;
#endif //__WATER_EXT
}

CTerrainMng::~CTerrainMng()
{
	for ( int i = 0 ; i < m_nWaterFrame ; i++ )
	{
		SAFE_DELETE_ARRAY( m_pWaterIndexList[i].pList );
	}
	SAFE_DELETE_ARRAY( m_pWaterIndexList );
}
HRESULT CTerrainMng::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}
HRESULT CTerrainMng::RestoreDeviceObjects()
{
	return S_OK;
}
HRESULT CTerrainMng::InvalidateDeviceObjects()
{
	return S_OK;
}
HRESULT CTerrainMng::DeleteDeviceObjects()
{
	for (map<DWORD, TERRAIN>::iterator it = m_aTerrain.begin(); it != m_aTerrain.end(); ++it)
	{
		if (it->second.m_pTexture)
			SAFE_RELEASE(it->second.m_pTexture);
	}
	return S_OK;
}
BOOL CTerrainMng::LoadTexture( DWORD dwId )
{
	if( (int)( dwId ) >= m_nSize )
		return FALSE;
	LPTERRAIN lpTerrain = GetTerrain( dwId );

	if( lpTerrain && lpTerrain->m_pTexture == NULL )
	{
		CString strPath;
		strPath = MakePath( DIR_WORLDTEX, lpTerrain->m_szTextureFileName );
		D3DXIMAGE_INFO srcInfo;
		HRESULT hr = LoadTextureFromRes( m_pd3dDevice, strPath,
			D3DX_DEFAULT, D3DX_DEFAULT, 4, 0, D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, &srcInfo, NULL, &lpTerrain->m_pTexture );

		if( FAILED( hr ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CTerrainMng::LoadScript( LPCTSTR lpszFileName )
{
	CScanner scanner;
	if(scanner.Load(lpszFileName, FALSE )==FALSE)
		return FALSE;

	LPTERRAIN lpTerrain;
	m_nSize = 0;
	int nBrace = 1;
	scanner.SetMark();
	int i = scanner.GetNumber(); // folder or id




	int FrameCnt = 0, ImageCnt = 0, IdCnt = 0;
	FrameCnt = scanner.GetNumber();
	if ( nBrace == 1 && FrameCnt )
	{
		m_nWaterFrame = FrameCnt;
		m_pWaterIndexList = new WaterTexList[ m_nWaterFrame ];
		ZeroMemory( m_pWaterIndexList, m_nWaterFrame * sizeof( WaterTexList ) );
	}

	while( nBrace )
	{
		if( *scanner.token == '}' || scanner.tok == FINISHED )
		{
			nBrace--;
			if( nBrace > 0 )
			{
				scanner.SetMark();
				i = scanner.GetNumber();  // folder or id
				if( i > m_nSize ) m_nSize = i;

				FrameCnt = scanner.GetNumber();
				IdCnt = 0;
				if ( nBrace == 1 && FrameCnt )
				{
					m_nWaterFrame = FrameCnt;
					m_pWaterIndexList = new WaterTexList[ m_nWaterFrame ];
					ZeroMemory( m_pWaterIndexList, m_nWaterFrame * sizeof( WaterTexList ) );
					ImageCnt = 0;
				}
				else if ( nBrace == 2 && FrameCnt )
				{
					m_pWaterIndexList[ImageCnt].ListCnt = FrameCnt;
#if __VER >= 14 // __WATER_EXT
					m_pWaterIndexList[ImageCnt].fWaterFrame = 0.15f;
#endif //__WATER_EXT
					m_pWaterIndexList[ImageCnt].pList = new int[ FrameCnt ];
					ZeroMemory( m_pWaterIndexList[ImageCnt].pList, FrameCnt * sizeof(int) );
					ImageCnt++;
				}
				continue;
			}
			if( nBrace == 0 )
				continue;
		}
		//i = _ttoi( scanner.token );
		scanner.GetToken(); // { or filename

		if( *scanner.token == '{' )
		{
			nBrace++;
			scanner.SetMark();
			i = scanner.GetNumber(); // id


			FrameCnt = scanner.GetNumber();	//	Count
			if ( i == 0 )
			{
				if ( nBrace == 2 && FrameCnt )
				{
					m_pWaterIndexList[ImageCnt].ListCnt = FrameCnt;
#if __VER >= 14 // __WATER_EXT
					m_pWaterIndexList[ImageCnt].fWaterFrame = 0.15f;
#endif //__WATER_EXT
					m_pWaterIndexList[ImageCnt].pList = new int[ FrameCnt ];
					ImageCnt++;
				}
			}

			if( i > m_nSize ) m_nSize = i;
			continue;
		}
		else
		{
			scanner.GoMark();
			i = scanner.GetNumber(); // id
			if( i > m_nSize ) m_nSize = i;
			FrameCnt = scanner.GetNumber();	//	Count
			if ( nBrace == 3 )
			{
				m_pWaterIndexList[ImageCnt - 1].pList[IdCnt] = i;
				IdCnt++;
			}
		}
		lpTerrain = &m_aTerrain[ i ];
		lpTerrain->m_dwId = i;
		scanner.GetToken();  // texture fileName
		strcpy( lpTerrain->m_szTextureFileName, scanner.token );
		lpTerrain->m_bBlock = scanner.GetNumber(); // block
		scanner.GetToken(); // sound
		strcpy( lpTerrain->m_szSoundFileName, scanner.token );
		scanner.SetMark();
		i = scanner.GetNumber();  // texture fileName
		if( i > m_nSize ) m_nSize = i;
	}
/*

	int i = scanner.GetNumber( TRUE );  // id
	while( scanner.tok != FINISHED )
	{
		lpTerrain = &m_aTerrain[ i ];
		lpTerrain->m_dwId = i;
		scanner.GetToken( TRUE );  // texture fileName
		strcpy( lpTerrain->m_szTextureFileName, scanner.token );
		lpTerrain->m_bBlock = scanner.GetNumber( TRUE ); // block
		scanner.GetToken( TRUE ); // sound
		strcpy( lpTerrain->m_szSoundFileName, scanner.token );
		i = scanner.GetNumber( TRUE );  // texture fileName
		if( i > m_nSize ) m_nSize = i;
	}
*/
	m_nSize++;
	return TRUE;
}
