#include "stdafx.h"
#include <direct.h>
#include "xUtil.h"
#include "../../resource/lang.h"
//////////////////////////////////////////////////////////////////////////////
// global variable
//////////////////////////////////////////////////////////////////////////////

#ifdef __PROF
CProf			g_Prof;
#endif

int				g_nMaxTri = 0;
static BOOL		g_bShowMsgBox = TRUE;
LARGE_INTEGER	g_llFreq;
DWORD			g_next;
char			g_szWorkDir[256];
char			g_szExeDir[256];
OSTYPE			g_osVersion = WINDOWS_UNKNOWN;

static int		g_nLanguage = -1; // LANG_KOR;
static int		g_nSubLanguage = 0;	//LANG_SUB_DEFAULT

//////////////////////////////////////////////////////////////////////////////
// global functions
//////////////////////////////////////////////////////////////////////////////



DWORD xRandom(DWORD min, DWORD max /* max : max -1 */)
{
	if (max > min)
		return min + xRandom(max - min);
	else
		return min;
}


float xRandomF(float num)
{
	return ((float)rand() / (float)RAND_MAX) * (float)num;
}

void	xSRand(DWORD seed)
{
	g_next = seed;
}

#ifdef __CLIENT
LPCTSTR Error(LPCTSTR strFormat, ...)
{
	char szBuff[8192];
	static char szStr[8192];

	va_list args;
	va_start(args, strFormat);
	int n = _vsntprintf(szBuff, 8191, strFormat, args);
	va_end(args);

	if (n > 0)
	{
#if defined(_DEBUG)
		if (g_bShowMsgBox && MessageBox(NULL, szBuff, "¿À·ù", MB_OKCANCEL) == IDCANCEL)
			g_bShowMsgBox = FALSE;
#else
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf(szStr, "%d/%2d/%2d   %02d:%02d:%02d   %s",
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond,
			szBuff);

		DEBUGOUT2(szStr);
#ifdef __AIO_ADMIN_THINGS
		if (g_pPlayer && g_pPlayer->IsAuthHigher(AUTH_ADMINISTRATOR))
			g_WndMng.PutString(szStr);
#endif
#endif
	}
	else
		szStr[0] = '\0';

	return szStr;
}

#else // __CLIENT
LPCTSTR Error(LPCTSTR strFormat, ...)
{
	char szBuff[8192];
	va_list args;
	va_start(args, strFormat);
	int n = _vsntprintf(szBuff, 8191, strFormat, args);
	va_end(args);

	if (n > 0)
	{
		char szStr[8192];
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf(szStr, "%d/%2d/%2d   %02d:%02d:%02d   %s\n",
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond,
			szBuff);

		DEBUGOUT2(szStr);
	}

	return "";
}
#endif // !__CLIENT





void	GetFileName(LPCTSTR szSrc, LPTSTR szFileName)
{
	int	len, i;

	i = len = strlen(szSrc);
	while (i--)
	{
		if (szSrc[i] == '\\')
		{
			lstrcpy(szFileName, szSrc + i + 1);
			break;
		}
		if (i == 0)
			lstrcpy(szFileName, szSrc);
	}
}



void	GetFileTitle(LPCTSTR szSrc, LPTSTR szFileTitle)
{
	int		len, i;
	GetFileName(szSrc, szFileTitle);


	len = strlen(szFileTitle);
	for (i = 0; i < len; i++)
	{
		if (szFileTitle[i] == '.')
			break;
	}
	szFileTitle[i] = '\0';
}




void	GetFileExt(LPCTSTR szSrc, LPTSTR szFileExt)
{
	int		len, i;
	len = strlen(szSrc);
	i = len;
	while (i--)
	{
		if (szSrc[i] == '.')
		{
			lstrcpy(szFileExt, szSrc + i + 1);
			break;
		}
	}
}




void	GetFilePath(LPCTSTR szSrc, LPTSTR szFilePath)
{
	int len = strlen(szSrc);

	for (int i = len - 1; i >= 0; i--)
	{
		if (szSrc[i] == '\\')
			break;
	}
	strncpy(szFilePath, szSrc, len);
	szFilePath[len] = '\0';
}



int StringFind(const char* string, int c)
{
	char* pdest;
	return (pdest = (char*)strchr(string, c)) ? pdest - string : -1;
}

void StringTrimRight(char* szString)
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)
	LPTSTR lpsz = szString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
	}
}

CString GetNumberFormatEx(LPCTSTR szNumber)
{
	ASSERT(szNumber);

	CString str;

	int nLength = lstrlen(szNumber);
	for (int i = nLength - 1; i >= 0; --i)
	{
		str.Insert(0, szNumber[i]);
		if (((nLength - i) % 3) == 0 && i != 0)
			str.Insert(0, ',');
	}
	return str;
}

CString Separator(__int64 nNum, char cSeparator)
{
	CString szNumber;
	szNumber.Format("%I64d", nNum);
	ASSERT(szNumber);

	CString str;
	int nLength = lstrlen(szNumber);
	for (int i = nLength - 1; i >= 0; --i)
	{
		str.Insert(0, szNumber[i]);
		if (((nLength - i) % 3) == 0 && i != 0)
			str.Insert(0, cSeparator);
	}
	return str;
}

void MakeEven(long& x)
{
	x++;
	x &= (~0x01);
}



void SetLanguageInfo(int nLanguage, int nSubLanguage)
{
	g_nLanguage = nLanguage;
	g_nSubLanguage = nSubLanguage;
}

int GetLanguage()
{
	return g_nLanguage;
}

int GetSubLanguage()
{
	return g_nSubLanguage;
}

BOOL IsChatSpamString(LPCTSTR str)
{
	BOOL bIsSpamShit = FALSE;
	if (str[0] == '/')
	{
		CString str2 = str;
		if (str2.Find("/shout ") == 0 || str2.Find("/s ") == 0 || str2.Find("/say ") == 0)
			bIsSpamShit = TRUE; // if it's shout or say
	}
	else
		bIsSpamShit = TRUE; // if it's general chat

	return bIsSpamShit;
}