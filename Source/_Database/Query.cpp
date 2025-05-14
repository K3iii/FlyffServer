#include "StdAfx.h"
#include "Query.h"

#ifdef __MONITORSRVR
#include "..\monitor\dbmanager.h"
#endif	// __MONITORSRVR

extern "C"
{
	#include "des.h"
}

void Decrpyt( unsigned char* pEncrypted, char* szResult )
{
	static unsigned char keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

    des3_context ctx3;
    des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );

    unsigned char* input = pEncrypted;
    unsigned char* result = (unsigned char*)szResult;
	for( int i=0; i<3; i++ )
		des3_decrypt( &ctx3, input + i*8, result + i*8 );
}

BOOL GetPWDFromToken( const char* szToken, char* szPWD )
{

	int nCount = strlen( szToken ) / 2;
	if( nCount > 0 )
	{
		BYTE byPWD[256];
		char szBuf[3] = {0, };
		for( int i=0; i<nCount; i++ )
		{
			memcpy( szBuf, szToken + i*2, 2 );
			sscanf( szBuf, "%02x", (byPWD + i) );
		}
		Decrpyt( byPWD, szPWD );
		return TRUE;
	}
	return FALSE;
/*
	int i;
	static unsigned char keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

    des3_context ctx3;
    des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );

	{
		// uncomment this block to make encrypted password
		// unsigned char input[24] = { '1', '2', };       // original password
		unsigned char input[24] = {0, };       // original password
		unsigned char output[24];

		strcpy( (char*)input, szToken );

		for( i=0; i<3; i++ )
		{
			unsigned char buf[8] = { 0, };
			memcpy( buf, input+i*8, 8 );
			des3_encrypt( &ctx3, buf, output + i*8 );
		}

		char szBuffer[256] = {0, };
		for( i=0; i<24; i++ )
			sprintf( szBuffer+i*2, "%02X", output[i] );
		OutputDebugString( szBuffer );

	}
	OutputDebugString( "\n" );
*/
	return TRUE;
}


CQuery::CQuery()
{
	hStmt	= NULL;
	hDbc	= NULL;
	hDbc	= NULL;
	hEnv	= NULL;

#ifdef __INVENTORY_168_SLOTS
	for (int i = 0; i < MAXCOL; i++)
		Col[i] = new char[51000];
#else
	for (int i = 0; i < MAXCOL; i++)
		Col[i] = new char[8192];
#endif
}


CQuery::~CQuery()
{
	if (hStmt)	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if (hDbc)	SQLDisconnect(hDbc);
	if (hDbc)	SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
	if (hEnv)	SQLFreeHandle(SQL_HANDLE_ENV,hEnv);

	for( int i = 0; i < MAXCOL; i++ )
		SAFE_DELETE_ARRAY( Col[i] );
}

void CQuery::DisConnect()
{
	if (hStmt)	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);	hStmt = NULL;
	if (hDbc)	SQLDisconnect(hDbc);
	if (hDbc)	SQLFreeHandle(SQL_HANDLE_DBC,hDbc);		hDbc = NULL;
	if (hEnv)	SQLFreeHandle(SQL_HANDLE_ENV,hEnv);		hEnv = NULL;
}



BOOL CQuery::EnableConnectionPooling()
{
	SQLRETURN result = SQLSetEnvAttr( NULL,  // make process level cursor pooling
					   			     SQL_ATTR_CONNECTION_POOLING,
						             (SQLPOINTER)SQL_CP_ONE_PER_DRIVER,
						             SQL_IS_INTEGER );

	return SQL_SUCCEEDED( result );
}







BOOL CQuery::Connect(int Type, char *ConStr, char *UID, const char *PWD)
{
	strcpy( DBName, ConStr );
	strcpy( DBId, UID );
	strcpy( DBPass, PWD );

	SQLCHAR InCon[255];
	SQLCHAR OutCon[255];
    SQLSMALLINT cbOutCon;

	SQLRETURN ret;


	SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&hEnv);
	SQLSetEnvAttr(hEnv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);


	SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hDbc);
	switch (Type)
	{
	case 1:
		wsprintf((char *)InCon,"DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;",ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 2:
		wsprintf((char *)InCon, "FileDsn=%s",ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 3:
		ret=SQLConnect(hDbc,(SQLCHAR *)ConStr,SQL_NTS,(SQLCHAR *)UID,SQL_NTS,
			(SQLCHAR *)PWD,SQL_NTS);
		break;
	}

	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		PrintDiag( ConStr );
		return FALSE;
	}


	ret=SQLAllocHandle(SQL_HANDLE_STMT,hDbc,&hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		PrintDiag( ConStr );
		hStmt=0;
		return FALSE;
	}


	return TRUE;
}


BOOL CQuery::BindParameter(SQLUSMALLINT parameterNumber,
                           SQLSMALLINT inputOutputType,
                           SQLSMALLINT valueType,
                           SQLSMALLINT parameterType,
                           SQLUINTEGER columnSize,
                           SQLSMALLINT decimalDigits,
                           SQLPOINTER  parameterValuePtr,
                           SQLINTEGER bufferLength,
                           SQLINTEGER *strLen_or_IndPtr)
{
	SQLRETURN result = ::SQLBindParameter( hStmt, parameterNumber, inputOutputType, valueType, parameterType, columnSize,
		                                decimalDigits, parameterValuePtr, bufferLength, strLen_or_IndPtr );
	if( result != SQL_SUCCESS )
	{
		TRACE( "CQuery::BindParameter\n" );
		WriteLog( "CQuery::BindParameter - result : %d, ThreadID : %d", result, ::GetCurrentThreadId() );
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CQuery::Exec(LPCTSTR szSQL)
{
	Clear();

	SQLRETURN ret = SQLExecDirect( hStmt, (SQLCHAR*)szSQL, lstrlen( szSQL ) );
	switch( ret )
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUG
		PrintDiag( szSQL, SQL_HANDLE_STMT );
#endif

	case SQL_SUCCESS:
	case SQL_NO_DATA_FOUND:
		break;
	default:
		{
			PrintDiag( szSQL, SQL_HANDLE_STMT );
			DisConnect();
			if( Connect( 3, DBName, DBId, DBPass ) )
			{
				ret = SQLExecDirect( hStmt, (SQLCHAR *)szSQL, SQL_NTS );
				if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA))
					return FALSE;
			}
			else
				return FALSE;
		}
	}

	return PrepareFetch();
}

BOOL CQuery::MoreResults()
{


	SQLRETURN ret = SQLMoreResults( hStmt );
	switch( ret ) {
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		break;
	default:
		PrintDiag( "SQLMoreResults", SQL_HANDLE_STMT );
		return FALSE;
	}

	return PrepareFetch();
}


BOOL CQuery::Exec(LPCTSTR szSQL, int nCount, QUERY_BINDINFO infos[])
{
	Clear();

	SQLRETURN ret = SQLExecDirect( hStmt, (SQLCHAR*)szSQL, lstrlen( szSQL ) );
	switch( ret )
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUG
		PrintDiag( szSQL, SQL_HANDLE_STMT );
#endif

	case SQL_SUCCESS:
	case SQL_NO_DATA_FOUND:
		break;
	default:
		{
			PrintDiag( szSQL, SQL_HANDLE_STMT );
			DisConnect();
			if( Connect( 3, DBName, DBId, DBPass ) )
			{
				ret = SQLExecDirect( hStmt, (SQLCHAR *)szSQL, SQL_NTS );
				if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA))
					return FALSE;
			}
			else
				return FALSE;
		}
	}

	return PrepareFetch();
}



BOOL CQuery::Fetch()
{
	SQLRETURN result = SQLFetch( hStmt );

	switch( result )
	{
	case SQL_SUCCESS_WITH_INFO:
		#ifdef _DEBUG
			PrintDiag( NULL, SQL_HANDLE_STMT );
		#endif
	case SQL_SUCCESS:
	case SQL_NEED_DATA:
		return TRUE;

	case SQL_NO_DATA_FOUND:		// == SQL_NO_DATA
		return FALSE;
	}

#ifdef __INTERNALSERVER
	PrintDiag( NULL, SQL_HANDLE_STMT );
#endif

	return FALSE;
}


void CQuery::Clear()
{
	SQLCloseCursor(hStmt);
	SQLFreeStmt(hStmt, SQL_UNBIND);
}


int CQuery::FindCol(char *name)
{
	int i;
	for (i=0;i<nCol;i++)
	{
		if (stricmp(name,(LPCTSTR)ColName[i])==0)
			return i+1;
	}
	return -1;
}


int CQuery::GetInt(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA)
	{
		return CQUERYNULL;
	} else
	{
		return atoi(Col[nCol-1]);
	}
}


int CQuery::GetInt(char *sCol)
{
	int n;
	n=FindCol(sCol);
	if (n==-1)
	{
		return CQUERYNOCOL;
	} else
	{
		return GetInt(n);
	}
}


__int64 CQuery::GetInt64(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA)
	{
		return CQUERYNULL;
	} else
	{
		return _atoi64(Col[nCol-1]);
	}
}


__int64 CQuery::GetInt64(char *sCol )
{
	int n;
	n=FindCol(sCol);
	if (n==-1)
	{
		return CQUERYNOCOL;
	} else
	{
		return GetInt64( n );
	}
}


float CQuery::GetFloat(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA)
	{
		return CQUERYNULL;
	} else
	{
		return	( (float)( atof(Col[nCol-1]) ) );
	}
}


float CQuery::GetFloat(char *sCol)
{
	int n;
	n = FindCol(sCol);
	if (n==-1)
		return CQUERYNOCOL;
	else
		return GetFloat(n);
}


char CQuery::GetChar(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA)
	{
		return CQUERYNULL;
	} else
	{
		return Col[nCol-1][0];
	}
}

char CQuery::GetChar(char *sCol)
{
	int n;
	n = FindCol(sCol);
	if (n==-1)
		return CQUERYNOCOL;
	else
		return GetChar(n);
}



void CQuery::GetStr(int nCol, char *buf)
{
	if (nCol > this->nCol)
	{
		strcpy(buf, "CQUERYNOCOL");
		return;
	}

	if (lCol[nCol-1]==SQL_NULL_DATA)
	{
		lstrcpy(buf,"NULL");
	} else {
		 strcpy(buf,Col[nCol-1]);
	}
}


void CQuery::GetStr(char *sCol, char *buf)
{
	int n;
	n=FindCol(sCol);
	if (n==-1)
	{
		lstrcpy(buf,"ERROR:Colume not found");
	} else
	{
		GetStr(n, buf);
	}
}


void CQuery::PrintDiag( LPCTSTR szSQL, SQLSMALLINT type )
{

/*
	SQLINTEGER NativeError;
	SQLCHAR SqlState[6], Msg[1024];
	SQLSMALLINT MsgLen;

	int ii=1;
	SQLRETURN Ret = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);
	if( Ret != SQL_NO_DATA )
	{
		if( szSQL )
			WriteLogFile("query:%s\nSQLSTATE:%s error:%s", szSQL, (LPCTSTR)SqlState,(LPCTSTR)Msg );
		else
			WriteLogFile("SQLSTATE:%s error:%s", (LPCTSTR)SqlState,(LPCTSTR)Msg );
	}
*/
	UCHAR szMsg[SQL_MAX_MESSAGE_LENGTH];
	UCHAR szState[SQL_SQLSTATE_SIZE+1];
	SQLSMALLINT nLen;
	SQLINTEGER nError;
	SQLRETURN ret;

	SQLHANDLE handle = hDbc;
	if( type == SQL_HANDLE_STMT )
		handle = hStmt;

	int nIndex=1;
	while( TRUE )
	{
		ret = SQLGetDiagRec( type, handle, nIndex, szState, &nError, szMsg, SQL_MAX_MESSAGE_LENGTH-1, &nLen );
		if( (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) &&	lstrcmp((LPCTSTR)szState, "00000") != 0 )
		{
			if( szSQL )
				WriteLogFile("query:%s\nSQLSTATE:%s error:%s", szSQL, (LPCTSTR)szState,(LPCTSTR)szMsg );
			else
				WriteLogFile("SQLSTATE:%s error:%s", (LPCTSTR)szState,(LPCTSTR)szMsg );
			nIndex++;
		}
		else
			break;
	}
}




int CQuery::ReadBlob(LPCTSTR szSQL, void *buf)
{
	SQLCHAR BinaryPtr[BLOBBATCH];
	SQLINTEGER LenBin;
	char *p;
	int nGet;
	int TotalGet=0;

	SQLRETURN ret=SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	if (ret!=SQL_SUCCESS) {
		PrintDiag( szSQL );
		return -1;
	}

	while ((ret=SQLFetch(hStmt)) != SQL_NO_DATA)
	{
		p=(char *)buf;
		while ((ret=SQLGetData(hStmt,1,SQL_C_BINARY,BinaryPtr,sizeof(BinaryPtr),
			&LenBin))!=SQL_NO_DATA) {
			if (LenBin==SQL_NULL_DATA)
			{
				Clear();
				return 0;
			}
			if (ret==SQL_SUCCESS)
				nGet=LenBin;
			else
				nGet=BLOBBATCH;
			TotalGet+=nGet;
			memcpy(p,BinaryPtr,nGet);
			p+=nGet;
		}
	}
	Clear();
	return TotalGet;
}



void CQuery::WriteBlob(LPCTSTR szSQL, void *buf, int size)
{
	SQLINTEGER cbBlob;
	char tmp[BLOBBATCH],*p;
	SQLPOINTER pToken;
	int nPut;

	cbBlob=SQL_LEN_DATA_AT_EXEC(size);
	SQLBindParameter(hStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_LONGVARBINARY,
		size,0,(SQLPOINTER)1,0,&cbBlob);
	SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	SQLRETURN ret=SQLParamData(hStmt, &pToken);
	while (ret==SQL_NEED_DATA)
	{
		if (ret==SQL_NEED_DATA)
		{
			if ((int)pToken==1)
			{
				for (p=(char *)buf;p<(char *)buf+size;p+=BLOBBATCH)
				{
					nPut=min(BLOBBATCH,(char *)buf+size-p);
					memcpy(tmp,p,nPut);
					SQLPutData(hStmt,(PTR)tmp,nPut);
				}
			}
		}
		ret=SQLParamData(hStmt, &pToken);
	}
	Clear();
}

void CQuery::WriteLogFile(char* lpszFormat,...)
{
	TCHAR szBuffer[1024* 64];

	va_list args;
	va_start( args, lpszFormat );
	int n = _vsntprintf( szBuffer, sizeof(szBuffer)-1, lpszFormat, args );
	va_end( args );

	if( n == -1 ) // if the number of bytes to write exceeds buffer, then count bytes are written and -1 is returned
		return;

	OutputDebugString( szBuffer );

	FILE* fp = fopen(MakeFileNameDate("..\\Program\\Logs\\CQuery-LogFile.txt"), "a");
	if( fp == NULL )
		return;

	// time
	SYSTEMTIME	time;
	GetLocalTime( &time );
	fprintf( fp, "%d/%02d/%02d\t%02d:%02d:%02d\n%s\n",
		     time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
		     szBuffer );
	fclose( fp );
}


BOOL CQuery::PrepareFetch()
{
	SQLNumResultCols(hStmt,&nCol);
	if (nCol > MAXCOL)
		return FALSE;



	if (nCol == 0)
	{
		Clear();
		return TRUE;
	}

	SWORD	nActualLen;
	SWORD	m_nSQLType;
	UDWORD	m_nPrecision;
	SWORD	m_nScale;
	SWORD	m_nNullability;



	for (int c=0;c<nCol;c++)
	{
#ifdef __INVENTORY_168_SLOTS
		SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 51000, &lCol[c]);
#else
		SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 8192, &lCol[c]);
#endif

		SQLDescribeCol(hStmt,c+1,ColName[c],30,
			&nActualLen,
			&m_nSQLType,
			&m_nPrecision,
			&m_nScale,
			&m_nNullability);
	}

	return TRUE;
}

BOOL CQuery::Execute( LPCTSTR lpszFormat, ... )
{
	char szQuery[1024]		= { 0,};
	va_list	args;
	va_start( args, lpszFormat );
	int n	= _vsntprintf( szQuery, 1024, lpszFormat, args );
	va_end( args );
	return n > 0 && Exec( szQuery );
}