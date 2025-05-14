// stdafx.cpp : source file that includes just the standard includes
//	WorldServer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

char			g_szDBAddr[16];
CProject		prj;
CTickCount		g_TickCount;
int				g_fX3	= 1;
u_long			g_uIdofMulti;
char			g_sHeartbeat[32] = {0, };
CGameTimer		g_GameTimer;
CFlyffEvent		g_eLocal;
ADDSMMODE		g_AddSMMode;
ULONG			g_uKey;
DWORD			g_tmCurrent = 0;
time_t		g_tCurrent	= 0;
int				g_nProcessCnt = 0;


