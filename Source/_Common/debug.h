#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __CLIENT
extern void DEBUGOUT2(LPCTSTR lpszStr, LPCTSTR szFileName = _T("Error.txt"));
#else // __CLIENT
extern void DEBUGOUT2(LPCTSTR lpszStr, LPCTSTR szFileName = _T("..\\Program\\Logs\\Error.txt"));
#endif // __CLIENT

extern void FILEOUT(LPCTSTR lpszFileName, LPCTSTR lpszFormat, ...);
extern void WriteLog(LPCTSTR lpszFormat, ...);
extern void WriteError(LPCTSTR lpszFormat, ...);

extern void OUTPUTDEBUGSTRING(LPCTSTR lpszFormat, ...);

extern const char* MakeFileNameDate(const char* lpszFileName);
#endif // __DEBUG_H