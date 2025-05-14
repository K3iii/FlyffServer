////////////////////////////////////////////////////////////////////////
/// \file MiniDumper.cpp
/// \author excel96
/// \date 2003.11.18
////////////////////////////////////////////////////////////////////////
//#include "StdAfx.h"

#include "StdAfx.h"
#include "MiniDumper.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <dbghelp.h>

#ifdef __MINIDUMP

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
    HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
    );


static DumpLevel s_DumpLevel			   = DUMP_LEVEL_0;

static bool		s_bAddTimeStamp            = true;

static TCHAR	s_szAppName[_MAX_PATH]     = {0,};

TCHAR			s_szFaultReason[1024*16]   = {0,};

static LPCTSTR GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs);
LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr);

void filelog( LPVOID, LPCTSTR lpszFormat, ... )
{
	TCHAR szBuffer[1024];

	va_list args;
	va_start(args, lpszFormat);
	int n = _vsntprintf(szBuffer, 1023, lpszFormat, args);
	va_end(args);

	if( n > 0 )
	{
#ifdef __CLIENT
        FILE* fp = fopen(MakeFileNameDate("Error.txt"), "a");
#else
        FILE* fp = fopen(MakeFileNameDate("..\\Program\\Logs\\Error.txt"), "a");
#endif
		if( fp )
		{
			fprintf(fp, szBuffer);
			fclose(fp);
		}
	}
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr)
{
    LONG    retval                   = EXCEPTION_CONTINUE_SEARCH;
    HMODULE hDLL                     = NULL;
    TCHAR   szDbgHelpPath[_MAX_PATH] = {0, };
    TCHAR   szDumpPath[MAX_PATH * 2] = {0,};




    if (::GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH))
    {
        LPTSTR pSlash = ::strrchr(szDbgHelpPath, '\\');
        if (pSlash)
        {
            ::lstrcpy(pSlash + 1, "DBGHELP.DLL");
            hDLL = ::LoadLibrary(szDbgHelpPath);
        }
    }


    if (hDLL == NULL) hDLL = ::LoadLibrary("DBGHELP.DLL");


    if (hDLL == NULL)
    {
        filelog(NULL, "DBGHELP.DLL not found");
        return retval;
    }


    MINIDUMPWRITEDUMP pfnMiniDumpWriteDump =
        (MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");


    if (pfnMiniDumpWriteDump == NULL)
    {
        filelog(NULL, "DBGHELP.DLL too old");
        return retval;
    }

    if (s_bAddTimeStamp)
    {

        SYSTEMTIME t;
        ::GetLocalTime(&t);


        TCHAR szTail[_MAX_PATH];
        _snprintf(szTail, _MAX_PATH-1,
            " %04d-%02d-%02d %02d-%02d-%02d",
            t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);


        ::lstrcat(szDumpPath, s_szAppName);
        ::lstrcat(szDumpPath, szTail);
    }


    ::lstrcat(szDumpPath, ".dmp");


    HANDLE hFile = ::CreateFile(
        szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


    if (hFile == INVALID_HANDLE_VALUE)
    {
        filelog(NULL, "Failed to create dump file '%s'", szDumpPath);
        return retval;
    }

    MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;

    ExceptionParam.ThreadId = ::GetCurrentThreadId();
    ExceptionParam.ExceptionPointers = pExPtr;
    ExceptionParam.ClientPointers = FALSE;


    BOOL bResult = FALSE;
    switch (s_DumpLevel)
    {
        case DUMP_LEVEL_0: // MiniDumpNormal
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(),
                hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
            break;
        case DUMP_LEVEL_1: // MiniDumpWithDataSegs
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(),
                hFile, MiniDumpWithDataSegs, &ExceptionParam, NULL, NULL);
            break;
        case DUMP_LEVEL_2: // MiniDumpWithFullMemory
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(),
                hFile, MiniDumpWithFullMemory, &ExceptionParam, NULL, NULL);
            break;
        default:
            bResult = pfnMiniDumpWriteDump(
                ::GetCurrentProcess(), ::GetCurrentProcessId(),
                hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
            break;
    }


    if (bResult)
    {
        TCHAR szMessage[8192] = {0,};
        lstrcat(szMessage, "Saved dump file to '");
        lstrcat(szMessage, szDumpPath);
        lstrcat(szMessage, "'.\nFault Reason : ");
        lstrcat(szMessage, GetFaultReason(pExPtr));

        filelog(NULL, szMessage);
        retval = EXCEPTION_EXECUTE_HANDLER;
    }
    else
    {
        filelog(NULL, "Failed to save dump file to '%s' (error %d)",
            szDumpPath, ::GetLastError() );
    }

    ::CloseHandle(hFile);

    return retval;
}

LPCTSTR GetFaultReason(struct _EXCEPTION_POINTERS* pExPtrs)
{
    if (::IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS)))
        return "BAD EXCEPTION POINTERS";


    switch (pExPtrs->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_GUARD_PAGE:               return "EXCEPTION_GUARD_PAGE";
        case EXCEPTION_INVALID_HANDLE:           return "EXCEPTION_INVALID_HANDLE";
        case 0xE06D7363:                         return "Microsoft C++ Exception";
        default:
            break;
    }


    lstrcpy(s_szFaultReason, "Unknown");
    ::FormatMessage(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
        ::GetModuleHandle("NTDLL.DLL"),
        pExPtrs->ExceptionRecord->ExceptionCode,
        0,
        s_szFaultReason,
        0,
        NULL);

    return s_szFaultReason;
}

void InitDumper( DumpLevel level )
{
    s_DumpLevel     = level;


    TCHAR szFilename[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilename, _MAX_PATH);


    TCHAR* dot = strrchr(szFilename, '.');
    ::lstrcpyn(s_szAppName, szFilename, (int)(dot - szFilename + 1));


    ::SetUnhandledExceptionFilter(TopLevelFilter);
}

#endif //  __MINIDUMP