#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

#include "CrashDumper.h"

class AudioPlayerJuceApplication : public JUCEApplication
{
public:
    AudioPlayerJuceApplication() {}

    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }

    void initialise (const String& /*commandLine*/)
	{
#ifdef _WIN32
		CrashDumper::init();
#endif
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
            setMenuBar(nullptr);
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

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(AudioPlayerJuceApplication)
