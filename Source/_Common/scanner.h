#ifndef __SCANNER_H
#define __SCANNER_H

#include "data.h"

#ifndef NULL_ID
#define NULL_ID 0xffffffff
#endif

#define MAX_TOKENSTR 2048

/*
 *  Add additional C keyword tokens here
 */
enum Toke
{
	NONE_,ARG,VAR,IF,ELSE,FOR,DO,WHILE,BREAK,SWITCH,ANSWER,SELECT,YESNO,
	CASE,DEFAULT,GOTO,RETURN,EOL,DEFINE,INCLUDE,ENUM,FINISHED,END
};
/*
 *  Add additional double operatoes here (such as->)
 */
enum DoubleOps
{
	LT = 1, LE,GT,GE,EQ,NE,NT
};

enum TokenType
{
	TEMP,DELIMITER,IDENTIFIER,NUMBER,HEX,KEYWORD,STRING,BLOCK
};


//////////////////////////////////////////////////////////////////////////



class CScanner
{
public:
	CScanner( BOOL bComma = FALSE );
	CScanner( LPVOID p, BOOL bComma = FALSE );
	virtual ~CScanner();

protected:
	BOOL		m_bComma;
	BYTE		m_bMemFlag;
	int			m_nProgSize;
	DWORD		m_dwDef;
	BOOL		Read( CFileIO* pFile, BOOL );

public:
	CHAR*		m_lpMark;
	CHAR*		m_pProg;
	CHAR*		m_pBuf;

	CString		Token;
	char*		token;
	char		m_mszToken[ MAX_TOKENSTR ];
	int			tokenType;
	int			tok;
	int			m_nDoubleOps;
	DWORD		dwValue;
	CString 	m_strFileName;
	BOOL		m_bErrorCheck;

	virtual void	SetMark() { m_lpMark = m_pProg; }
	virtual void	GoMark() { if( m_lpMark ) m_pProg = m_lpMark; }
	virtual int		GetLineNum( LPVOID lpProg = NULL );
	virtual void	PutBack();
	virtual	int		GetToken( BOOL bComma = FALSE );

	void			SetErrorCheck( BOOL bErrorCheck ) { m_bErrorCheck = bErrorCheck; }
	BOOL			GetErrorCheck() { return m_bErrorCheck; }
	void			GetLastFull();
	void			Free();
	BOOL			Load( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE );
	BOOL			Load_FileIO( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE );
	void			SetProg( LPVOID pProg );
	int				GetNumber( BOOL bComma = FALSE );
	__int64			GetInt64( BOOL bComma = FALSE );
	FLOAT			GetFloat( BOOL bComma = FALSE );
	DWORD			GetHex( BOOL bComma = FALSE );
	SERIALNUMBER	GetSerialNumber( BOOL bComma = FALSE );
	EXPINTEGER		GetExpInteger(  BOOL bComma = FALSE )	{ return GetInt64( bComma ); }
};


///////////////////////////////////////////////////////////////////////

extern WORD		g_codePage;

bool	IsComposibleTh(BYTE prev, BYTE curr, int mode);
const char*	CharNextEx(const char* strText, WORD codePage = g_codePage);

///////////////////////////////////////////////////////////////////////
int WideCharToMultiByteEx(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCWSTR  lpWideCharStr,
    int      cchWideChar,
    LPSTR    lpMultiByteStr,
    int      cchMultiByte,
    LPCSTR   lpDefaultChar,
    LPBOOL   lpUsedDefaultChar);

int MultiByteToWideCharEx(
    UINT     CodePage,
    DWORD    dwFlags,
    LPCSTR   lpMultiByteStr,
    int      cchMultiByte,
    LPWSTR   lpWideCharStr,
    int      cchWideChar);


BOOL IsCyrillic( const char chSrc );

#endif
