#pragma once

#ifdef _WIN32

#include "../JuceLibraryCode/JuceHeader.h"

// clang-format off
#include <Windows.h>
#include <Dbghelp.h>
// clang-format on

/**
        Show a dialog and write a crash dump when the application crashes.
*/
class CrashDumper : public ThreadWithProgressWindow
{
public:
    CrashDumper(EXCEPTION_POINTERS* e);

    static void init();

    // ThreadWithProgressWindow
public:
    virtual void run() override;

private:
    EXCEPTION_POINTERS* e;
};

#endif
