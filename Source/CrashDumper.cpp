#include "CrashDumper.h"

#ifdef _WIN32

LONG CALLBACK unhandledExceptionFilter(EXCEPTION_POINTERS* e)
{
    CrashDumper crashDumper(e);

    crashDumper.runThread();

    return EXCEPTION_CONTINUE_SEARCH;
}

CrashDumper::CrashDumper(EXCEPTION_POINTERS* e)
    : ThreadWithProgressWindow(TRANS("Application crashed.\n\nWriting crash dump ..."), true, false)
    , e(e)
{
    setProgress(-1.0);
}

void CrashDumper::run()
{
    HMODULE library = LoadLibrary("dbghelp");
    if (library == nullptr)
        return;

    const auto address = (decltype(&MiniDumpWriteDump))GetProcAddress(library, "MiniDumpWriteDump");
    if (address == nullptr)
        return;

    SYSTEMTIME time;
    GetSystemTime(&time);

    TCHAR path[MAX_PATH];

    wsprintf(path, "CrashDump MStarPlayer %4d-%02d-%02d %02d_%02d_%02d.dmp",
             time.wYear, time.wMonth, time.wDay,
             time.wHour, time.wMinute, time.wSecond);

    HANDLE dumpFile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
    exceptionInformation.ThreadId = GetCurrentThreadId();
    exceptionInformation.ExceptionPointers = e;
    exceptionInformation.ClientPointers = FALSE;

    MINIDUMP_USER_STREAM_INFORMATION userStreamInfo;
// Include the git revision in dumps. The define is set by the build system.
#ifdef GIT_REVISION
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
    userStreamInfo.UserStreamCount = 1;
    MINIDUMP_USER_STREAM userStreams[1];

    userStreams[0].Type = CommentStreamA;
    userStreams[0].Buffer = TOSTRING(GIT_REVISION);
    userStreams[0].BufferSize = sizeof(TOSTRING(GIT_REVISION));
    userStreamInfo.UserStreamArray = userStreams;
#else
    userStreamInfo.UserStreamCount = 0;
#endif

    const MINIDUMP_TYPE miniDumpType = static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemory);

    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        dumpFile,
        miniDumpType,
        &exceptionInformation,
        &userStreamInfo,
        NULL);

    CloseHandle(dumpFile);
}

void CrashDumper::init()
{
    SetUnhandledExceptionFilter(unhandledExceptionFilter);
}

#endif
