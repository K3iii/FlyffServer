#include "stdafx.h"

#include "GuildRender.h"


#ifndef __WORLDSERVER
#ifdef __XCLOAK




CGuildTexture	g_GuildTexture;




CGuildTexture::CGuildTexture()
{
	m_nMax = 0;
	memset( m_Texture, 0, sizeof(m_Texture) );

//	m_pFree = NULL;
}

CGuildTexture::~CGuildTexture()
{

	m_nMax = 0;
	memset( m_Texture, 0, sizeof(m_Texture) );

//	m_pFree = NULL;
}




LPDIRECT3DTEXTURE9 CGuildTexture::LoadGuildTexture( DWORD idGuild )
{
#ifndef _DEBUG
	return NULL;
#endif
	int	i;
	GUILD_TEXTURE *pList = m_Texture, *pNode;
	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMaterial;
	char	szName[128];

	memset( szName, 0, sizeof(szName) );

	for( i = 0; i < MAX_GUILD_TEXTURE; i ++ )
	{
		pNode = pList++;
		if( pNode->idGuild == 0 )	continue;
		if( pNode->idGuild == idGuild )
			return pNode->lpCloakTexture;
	}

	pList = m_Texture;
	for( i = 0; i < MAX_GUILD_TEXTURE; i ++ )
	{
		pNode = pList++;
		if( pNode->idGuild )		continue;

		pNode->idGuild = idGuild;


		sprintf( szName, "CT%06d.jpg", idGuild );
		pMtrl = g_TextureMng.AddMaterial( D3DDEVICE, &mMaterial, szName, DIR_LOGO );

		pNode->lpCloakTexture = pMtrl->m_pTexture;
		pNode->tmLoad = timeGetTime();
		return pNode->lpCloakTexture;
	}

	LPCTSTR szErr = Error( "CGuildTexture::LoadGuildTexture : ²ËÂù°æ¿ì »ý±è" );
	//ADDERRORMSG( szErr );

	return NULL;
}













#endif
#endif // not WORLDSERVER