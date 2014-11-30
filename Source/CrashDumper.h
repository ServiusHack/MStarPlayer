#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <Windows.h>
#include <Dbghelp.h>

class CrashDumper : public ThreadWithProgressWindow
{
public:
	CrashDumper(EXCEPTION_POINTERS* e);

	virtual void run() override;

	static void init();

private:
	EXCEPTION_POINTERS* e;
};