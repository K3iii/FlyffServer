#include "stdafx.h"
#include "User.h"
#include "ScriptHelper.h"

#if !defined(__REMOVE_SCIRPT_060712)









char	CScriptDialog::s_pszName[64];
char	CScriptDialog::s_pszKey[260];
char	CScriptDialog::s_pszBeforeRun[260];
char	CScriptDialog::s_pszAfterRun[260];

CScriptDialog::CScriptDialog()
{
	ZeroMemory( m_anGlobalVal, sizeof( m_anGlobalVal ) );
}
CScriptDialog::~CScriptDialog()
{
	Free();
}
void CScriptDialog::Free()
{
	LPSCRIPT lpScript;
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		lpScript = (LPSCRIPT)m_scriptAry.GetAt(i);
		SAFE_DELETE( lpScript->m_pScript );
		SAFE_DELETE( lpScript );
	}
	m_scriptAry.RemoveAll();
}
BOOL CScriptDialog::IsEmpty()
{
	return m_scriptAry.GetSize() ? FALSE : TRUE;
}
BOOL CScriptDialog::InterpretScript( CScript& smc )
{
	LPSTR pProg;
	TCHAR szSubKey[ 64 ];
	LPSCRIPT lpScript;
	LPSTR pScript;
	int nBlockCount;
	{
		int nTokenType = smc.GetToken(); // first word

		while(*smc.token != '}' && smc.tok != FINISHED )
		{
			if( nTokenType != STRING )
			{
				CString string;
				string.Format( "%s(%d) : CScriptDialog::InterpretScript에서 스트링 형태가 아닌 Key(%s)가 발견되었습니다.", smc.m_strFileName, smc.GetLineNum(), smc.token );
				Error( string );
			}
			_tcscpy( szSubKey, smc.token );
			pProg = smc.m_pProg;
			if(smc.IsCompilerMode())
				nBlockCount = 8; // "# " + "main" + "()"
			else
				nBlockCount = 6; // "main" + "()"
			int nBlock = -1;
			while( nBlock )
			{
				if( IsDBCSLeadByte( *pProg ) == FALSE )
				{
					if( *pProg == '"' )
					{
						do
						{
							if( IsDBCSLeadByte( *pProg ) == FALSE )
							{
								nBlockCount++; pProg++;
							}
							else
							{
								nBlockCount += 2; pProg += 2;
							}

						} while( *pProg != '"' );
						nBlockCount++; pProg++;
					}
					if( *pProg == '{' )
					{
						if( nBlock == -1 )
							nBlock = 1;
						else
							nBlock++;
					}
					if( *pProg == '}' )
						nBlock--;
					nBlockCount++; pProg++;
				}
				else
				{
					nBlockCount += 2; pProg += 2;
				}
			}
			lpScript = new SCRIPT;
			ZeroMemory( lpScript, sizeof( SCRIPT ) );
			pScript = new CHAR[ nBlockCount + 10 ];
			lpScript->m_pScript = pScript;
			_tcscpy( lpScript->m_szKey, szSubKey );

//			lpScript->m_bHidden = FALSE;
			m_scriptAry.Add( lpScript );
			if( smc.IsCompilerMode() )
			{
				strcpy(pScript,"# main()");
				memcpy(pScript+8,smc.m_pProg,nBlockCount-8);
			}
			else
			{
				strcpy(pScript,"main()");
				memcpy(pScript+6,smc.m_pProg,nBlockCount-6);
			}
			*(pScript + nBlockCount) = 0;
			smc.m_pProg = pProg;
			nTokenType = smc.GetToken(); // next
		}
		smc.GetToken(); // next word
	}
	return TRUE;
}


BOOL CScriptDialog::LoadScript(LPCTSTR lpszFileName)
{
	int nProcess = CScript::COMPILER;
	CScript smc;

	if( smc.Load( lpszFileName, TRUE, nProcess ) == TRUE )
	{
		strcpy( m_szFileName, lpszFileName );

		smc.SetErrorCheck( FALSE );
		while( smc.tokenType != STRING )
			smc.GetToken();
		smc.PutBack();
		smc.SetErrorCheck( TRUE );
		InterpretScript( smc );
		smc.Free();
	}
	struct _finddata_t c_file;
	long hFile;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
	_splitpath( lpszFileName, drive, dir, name, ext );
	CString string;
	string.Format( "%s%s-*%s", dir, name, ext );
	/* Find first .c file in current directory */
	if( (hFile = _findfirst( string, &c_file )) != -1L )
	{
		if( !(c_file.attrib & _A_SUBDIR ) )
		{
			string.Format( "%s%s", dir, c_file.name );
			if( smc.Load( string, TRUE, nProcess ) == FALSE )
				return FALSE;
			smc.SetErrorCheck( FALSE );
			while( smc.tokenType != STRING )
				smc.GetToken();
			smc.PutBack();
			smc.SetErrorCheck( TRUE );
			InterpretScript( smc );
			smc.Free();
		}
		/* Find the rest of the .c files */
		while( _findnext( hFile, &c_file ) == 0 )
		{
			//strcpy( name, dir );
			//strcat( name, c_file.name );
			if( !(c_file.attrib & _A_SUBDIR ) )
			{
				string.Format( "%s%s", dir, c_file.name );
				if( smc.Load( string, TRUE, nProcess ) == FALSE )
					return FALSE;
				smc.SetErrorCheck( FALSE );
				while( smc.tokenType != STRING )
					smc.GetToken();
				smc.PutBack();
				smc.SetErrorCheck( TRUE );
				InterpretScript( smc );
				smc.Free();
			}
		}
		_findclose( hFile );
	}
	return TRUE;
}
LPSCRIPT CScriptDialog::Lookup(LPCTSTR pKey)
{
	for(int i = 0; i < m_scriptAry.GetSize(); i++)
	{
		LPSCRIPT lpScript = (LPSCRIPT)m_scriptAry.GetAt(i);
		if( _tcscmp( lpScript->m_szKey, pKey ) == 0 )
			return lpScript;
	}
	return NULL;
}

BOOL CScriptDialog::Run( LPCTSTR pKey, int* nResult, int nValue,int nDstId, int nSrcId, int nQuestId, int nCaller )
{
	CScript script;
	script.m_paTimer = m_aTimer;
	script.m_panGlobalVal = m_anGlobalVal;
	script.m_strFileName = m_szFileName;
	script.SetIValue( nValue, nDstId, nSrcId, nCaller, int( this ), nQuestId, -1 );

	if( strcmp( pKey, "QUEST_END" ) == 0 )
	{
		__QuestEnd( script.GetPcId(), script.GetNpcId(), script.m_panGlobalVal[ 15 ] );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_BEGIN" ) == 0 )
	{
		__QuestBegin( script.GetPcId(), script.GetNpcId(), nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_BEGIN_YES" ) == 0 )
	{
		__QuestBeginYes( script.GetPcId(), script.GetNpcId(), nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_BEGIN_NO" ) == 0 )
	{
		__QuestBeginNo( script.GetPcId(), script.GetNpcId(), nQuestId );
		return TRUE;
	}
	else
	if( strcmp( pKey, "QUEST_END_COMPLETE" ) == 0 )
	{
		__QuestEndComplete( script.GetPcId(), script.GetNpcId(), script.m_panGlobalVal[ 15 ], nValue, nQuestId );
		return TRUE;
	}

	LPSCRIPT lpScript = Lookup( pKey );
	if( lpScript && lpScript->m_pScript )
	{
		script.SetProg( lpScript->m_pScript );

		try
		{
			script.Run();
		}
		catch(...)
		{
			Error( "CScriptDialog::Run(%s) - %s ", pKey, m_szFileName );
		}

		if( nResult != NULL )
			*nResult = script.GetReturn();
		return TRUE;
	}
	return FALSE;
}

#endif // #if !defined(__REMOVE_SCIRPT_060712)