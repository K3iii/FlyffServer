////////////////////////////////////////////////////////////////////////
/// \file MiniDumper.h
/// \author excel96
/// \date 2003.11.18
////////////////////////////////////////////////////////////////////////

#ifndef __MINIDUMPER_H__
#define __MINIDUMPER_H__

#ifdef __MINIDUMP


enum DumpLevel
{
    DUMP_LEVEL_0,
    DUMP_LEVEL_1,
    DUMP_LEVEL_2
};

extern void InitDumper( DumpLevel level );
extern LONG WINAPI TopLevelFilter(struct _EXCEPTION_POINTERS* pExPtr);
#endif // __MINIDUMP

#endif //__MINIDUMPER_H__