#ifndef __FUNCTEXTCMD_H
#define __FUNCTEXTCMD_H

struct TextCmdFunc
{
	BOOL (*m_pFunc)( CScanner& scanner );
	TCHAR* m_pCommand;
	TCHAR* m_pAbbreviation;
	TCHAR* m_pKrCommand;
	TCHAR* m_pKrAbbreviation;
	DWORD  m_nServer;
	DWORD m_dwAuthorization;
	TCHAR* m_pszDesc;
};
extern TextCmdFunc m_textCmdFunc[];

#endif
