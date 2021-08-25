#pragma once

#ifdef _WIN32

#include <future>

#include "juce_gui_basics/juce_gui_basics.h"

// clang-format off
#include <Windows.h>
#include <Dbghelp.h>
// clang-format on

/**
        Show a dialog and write a crash dump when the application crashes.
*/
class CrashDumper : public juce::ThreadWithProgressWindow
{
public:
    CrashDumper(std::promise<void>&& promise, EXCEPTION_POINTERS* e);

    static void init();

    // ThreadWithProgressWindow
public:
    virtual void run() override;

private:
    std::promise<void> promise;
    EXCEPTION_POINTERS* e;
};

#endif
