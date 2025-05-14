
// author : gmpbigsun
// date : 2009_11_02
// music player for ogg and mp3 ( use miles lib )

#include "StdAfx.h"

#define _MUSIC_LIB_MILES	0		//use Miles lib
#define _MUSiC_LIB_FMOD		1		//use FMod Lib

#ifdef  __USE_SOUND_LIB_FMOD
#define __MUSIC_LIB  ( _MUSiC_LIB_FMOD ) 
#else
#define __MUSIC_LIB  ( _MUSIC_LIB_MILES ) 
#endif

#include "Music.h"
#include "mss.h"

#pragma comment( lib, "mss32.lib" )

CMusicMng g_MusicMng;

static BOOL g_bEnableMusic = TRUE;
static BOOL g_bFadeOutMusic = FALSE;
static TCHAR g_filename[128];
HSTREAM  g_Stream = NULL;
HDIGDRIVER  g_Dig = NULL;
TCHAR g_szFileName[64];

static float g_fMusicVolume = 1.0f;
static float g_fCurrVolume = 1.0f;

BOOL PlayMusic_(LPCTSTR lpszFileName, int nLoopCount)
{
	static BOOL sFORCELOCK = FALSE;

	if (sFORCELOCK)
		return FALSE;

	if (!g_Dig)
		return FALSE;

	if (g_bEnableMusic == FALSE)	
		return FALSE;

	if (_tcscmp(lpszFileName, g_filename) == 0 && IsPlayingMusic())
		return FALSE;

	if (g_Stream)
		AIL_close_stream(g_Stream);

	strcpy(g_filename, lpszFileName);

	g_Stream = AIL_open_stream(g_Dig, g_filename, 0);

	if (!g_Stream)
	{
		sFORCELOCK = TRUE;
		return FALSE;
	}

	if (nLoopCount < 0)
		nLoopCount = 0;

	AIL_set_stream_loop_count(g_Stream, nLoopCount);
	AIL_start_stream(g_Stream);
	AIL_service_stream(g_Stream, 0);

	SetVolume(g_fMusicVolume);

	return TRUE;
}

BOOL PlayMusic(LPCTSTR lpszFileName, int nLoopCount)
{
	return PlayMusic_(lpszFileName, nLoopCount);
}

BOOL PlayMusic(DWORD dwIdMusic, int nLoopCount)
{
	if (dwIdMusic == NULL_ID || dwIdMusic == 0)
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName(dwIdMusic);
	if (lpFileName[0])
	{
		if (PlayMusic(lpFileName, nLoopCount))
			return TRUE;
	}
	return FALSE;
}

BOOL PlayBGM(DWORD dwIdMusic)
{
	if (dwIdMusic == NULL_ID || dwIdMusic == 0)
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName(dwIdMusic);
	if (lpFileName[0])
		return PlayMusic(lpFileName, 1);

	return FALSE;
}

void LockMusic()
{

}

void StopMusic()
{
	if (!g_Stream)
		return;

	AIL_pause_stream(g_Stream, 1);	
}

void SetVolume(FLOAT fVolume)
{
	if (!g_Dig)
		return;

	g_fMusicVolume = fVolume;

	AIL_set_digital_master_volume_level(g_Dig, g_fMusicVolume);
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
}

BOOL IsPlayingMusic()
{
	if (!g_Stream)
		return FALSE;

	U32 status = AIL_stream_status(g_Stream);
	return (SMP_PLAYING == status);
}

BOOL IsStopMusic()
{
	if (!g_Stream)
		return TRUE;

	U32 status = AIL_stream_status(g_Stream);
	return (SMP_PLAYING != status);

}

void InitCustomSound(BOOL bEnable)
{
	AIL_startup();

	g_bEnableMusic = bEnable;

	g_Dig = AIL_open_digital_driver(44100, 16, MSS_MC_USE_SYSTEM_CONFIG, 0);
	if (!g_Dig)
	{
		MessageBox(0, AIL_last_error(), "Music initialize error", MB_OK);
		Error("AIL_open_digital_driver error, Music init error");
	}

}

void UnInitCustomSound()
{
	AIL_close_stream(g_Stream);
	AIL_close_digital_driver(g_Dig);
	AIL_shutdown();

	g_Stream = NULL;
	g_Dig = NULL;
}

BOOL CMusicMng::LoadScript(LPCTSTR lpszFileName)
{
	CScript script;
	if (script.Load(lpszFileName) == FALSE)
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		strcpy(music.szMusicFileName, script.token);
		m_aMusic.SetAtGrow(id, &music);
		id = script.GetNumber();
	} while (script.tok != FINISHED);
	m_aMusic.Optimize();

	return TRUE;
}

LPTSTR CMusicMng::GetFileName(DWORD dwId)
{
	MUSIC* pMusic = m_aMusic.GetAt(dwId);
	if (pMusic)
		return (LPTSTR)pMusic->szMusicFileName;
	return "";
}