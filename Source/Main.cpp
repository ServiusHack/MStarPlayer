#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"


#include <Windows.h>
#include <Dbghelp.h>

LONG CALLBACK unhandledExceptionFilter(EXCEPTION_POINTERS* e)
{
	HMODULE library = LoadLibrary("dbghelp");
	if (library == nullptr)
		return EXCEPTION_CONTINUE_SEARCH;

	auto address = (decltype(&MiniDumpWriteDump))GetProcAddress(library, "MiniDumpWriteDump");
	if (address == nullptr)
		return EXCEPTION_CONTINUE_SEARCH;

	SYSTEMTIME time;
	GetSystemTime(&time);

	TCHAR path[MAX_PATH];

	wsprintf(path, "CrashDump AudioPlayerJuce %4d-%02d-%02d %02d_%02d_%02d.dmp",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond);

	HANDLE dumpFile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
	exceptionInformation.ThreadId = GetCurrentThreadId();
	exceptionInformation.ExceptionPointers = e;
	exceptionInformation.ClientPointers = FALSE;

	const MINIDUMP_TYPE miniDumpType = static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemory);

	MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		dumpFile,
		miniDumpType,
		&exceptionInformation,
		NULL,
		NULL);

	CloseHandle(dumpFile);

	return EXCEPTION_CONTINUE_SEARCH;
}


class AudioPlayerJuceApplication : public JUCEApplication
{
public:
    AudioPlayerJuceApplication() {}

    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }

    void initialise (const String& /*commandLine*/)
	{
		SetUnhandledExceptionFilter(unhandledExceptionFilter);
		MainContentComponent::initLookAndFeel();
		LookAndFeel::setDefaultLookAndFeel(MainContentComponent::s_defaultLookAndFeel);
		m_mainWindow = new MainWindow();
    }

    void shutdown()
    {
		m_mainWindow = nullptr; // (deletes our window)
		MainContentComponent::destroyLookAndFeel();
    }

    void systemRequestedQuit()
    {
        quit();
    }

    void anotherInstanceStarted(const String& /*commandLine*/)
    {
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow : public DocumentWindow
    {
    public:

        // the command manager object used to dispatch command events
        ApplicationCommandManager commandManager;

        MainWindow() : DocumentWindow ("Audio Player",
                                        Colours::lightgrey,
                                        DocumentWindow::allButtons)
        {
            MainContentComponent* main = new MainContentComponent(&commandManager);
            
            commandManager.registerAllCommandsForTarget(main);
            commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());

            addKeyListener(commandManager.getKeyMappings());

            setContentOwned(main, true);

            setMenuBar(main);

            main->setApplicationCommandManagerToWatch(&commandManager);

            centreWithSize(getWidth(), getHeight());
            setVisible(true);
            setResizable(true, true);
        }

        ~MainWindow()
        {
            setMenuBar (nullptr);
            clearContentComponent();
        }

        void closeButtonPressed()
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
	ScopedPointer<MainWindow> m_mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(AudioPlayerJuceApplication)
