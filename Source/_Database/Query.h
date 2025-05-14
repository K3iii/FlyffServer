#ifndef _QUERY_H_
#define _QUERY_H_

#include <sql.h>
#include <sqlext.h>

struct QUERY_BINDINFO
{
	SQLSMALLINT targetType;
	SQLPOINTER	targetValue;
	SQLINTEGER	bufferLength;
	SQLINTEGER*	strLen_or_Ind;
};

class CQuery
{
public:
//	char *DBName, *DBId, *DBPass;
	char DBName[256];
	char DBId[256];
	char DBPass[256];


	enum { MAXCOL=256, BLOBBATCH=10000, CQUERYNULL=-100, CQUERYEOF=-101,
		CQUERYNOCOL=-102, CQUERYERROR=-103 };
private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	char*	Col[MAXCOL];

	int FindCol(char *name);

public:
	SQLHSTMT hStmt;
	SQLSMALLINT nCol;
	SQLCHAR ColName[MAXCOL][50];
	SQLINTEGER lCol[MAXCOL];

	CQuery();
	~CQuery();

	void PrintDiag( LPCTSTR szSQL, SQLSMALLINT type = SQL_HANDLE_DBC );
	BOOL Connect(int Type, char *ConStr, char *UID=NULL, const char *PWD=NULL);
	void DisConnect();
	BOOL Exec(LPCTSTR szSQL);
	BOOL Exec(LPCTSTR szSQL, int nCount, QUERY_BINDINFO info[]);
	BOOL PrepareFetch();

	BOOL	Execute( LPCTSTR lpszFormat, ... );

	int ExecGetInt(LPCTSTR szSQL);
	void ExecGetStr(LPCTSTR szSQL, char *buf);
	BOOL Fetch();
	void Clear();
	int GetInt(int nCol);
	int GetInt(char *sCol);

	__int64		GetInt64( int nCol );
	__int64		GetInt64( char* sCol );

	EXPINTEGER	GetExpInteger( int nCol )	{ return GetInt64( nCol );	}
	EXPINTEGER	GetExpInteger(	char* sCol ) { return GetInt64( sCol ); 	}
	SERIALNUMBER	GetSerialNumber( int nCol )
		{
			return GetInt( nCol );
		}
	SERIALNUMBER	GetSerialNumber( char* sCol )
		{
			return GetInt( sCol );
		}

	float GetFloat(int nCol);
	float GetFloat(char *sCol);
	char GetChar(int nCol);
	char GetChar(char *sCol);
	void GetStr(int nCol, char *buf);
	void GetStr(char *sCol, char *buf);
	int ReadBlob(LPCTSTR szSQL, void *buf);
	void WriteBlob(LPCTSTR szSQL, void *buf, int size);
	BOOL BindParameter(SQLUSMALLINT parameterNumber,
                           SQLSMALLINT inputOutputType,
                           SQLSMALLINT valueType,
                           SQLSMALLINT parameterType,
                           SQLUINTEGER columnSize,
                           SQLSMALLINT decimalDigits,
                           SQLPOINTER  parameterValuePtr,
                           SQLINTEGER bufferLength,
                           SQLINTEGER *strLen_or_IndPtr);

	BOOL MoreResults( void );


	void WriteLogFile(char *strLog,...);
	static BOOL EnableConnectionPooling();
};



extern BOOL GetPWDFromToken( const char* szToken, char* szPWD );
#endif