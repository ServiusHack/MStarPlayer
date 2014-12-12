#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <Windows.h>
#include <Dbghelp.h>

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