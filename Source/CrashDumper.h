#pragma once

#ifdef _WIN32

#include "../JuceLibraryCode/JuceHeader.h"

#include <Windows.h>
#include <Dbghelp.h>

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
