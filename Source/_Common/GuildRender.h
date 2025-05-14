#ifndef __GUILDRENDER_H__
#define __GUILDRENDER_H__

#define MAX_GUILD_TEXTURE	256


struct GUILD_TEXTURE
{
	DWORD	idGuild;
	LPDIRECT3DTEXTURE9	lpCloakTexture;

	DWORD	tmLoad;
};

class CGuildTexture
{
	int m_nMax;
	GUILD_TEXTURE	m_Texture[ MAX_GUILD_TEXTURE ];
public:
	CGuildTexture();
	~CGuildTexture();


	LPDIRECT3DTEXTURE9	LoadGuildTexture( DWORD idGuild );
};

extern CGuildTexture	g_GuildTexture;


#endif // __GUILDRENDER_H__