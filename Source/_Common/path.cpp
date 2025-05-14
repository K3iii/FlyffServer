// Neuz.cpp : Defines the entry point for the application.

#include "stdafx.h"

#include "lang.h"

#ifdef __LANG_1013
#include "langman.h"
#endif	// __LANG_1013


void MakePath( CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	strFullPath = lpszDirName;
	strFullPath += lpszDirName;
	strFullPath += lpszFileName;
}

CString MakePath( LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile )
{
	CString	strFullPath( lpDir );

	strFullPath		+= CLangMan::GetInstance()->GetLangData( dwLan )->szPath;

	strFullPath		+= lpFile;
	return strFullPath;
}


CString MakePath( LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	CString strFullPath( lpszDirName );
	strFullPath += lpszFileName;
	return strFullPath;
}

void MakePath( TCHAR* lpszFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName )
{
	_tcscpy( lpszFullPath, lpszDirName );
	_tcscat( lpszFullPath, lpszFileName );
}
