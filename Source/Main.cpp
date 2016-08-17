#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

#include "CrashDumper.h"
#include "Translations.h"

class MStarPlayerApplication : public JUCEApplication
{
public:
    MStarPlayerApplication() {}
    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }
    void initialise(const String& /*commandLine*/)
    {
#ifdef _WIN32
        CrashDumper::init();
#endif

        PropertiesFile::Options options;
        options.applicationName = "MStarPlayer";
        options.filenameSuffix = ".settings";
        m_applicationProperties.setStorageParameters(options);

        if (m_applicationProperties.getUserSettings()->getValue("language") == "de")
        {
            LocalisedStrings::setCurrentMappings(new LocalisedStrings(Translations::German, false));
        }
        else if (m_applicationProperties.getUserSettings()->getValue("language") == "en")
        {
            // Default is English.
        }
        else if (SystemStats::getUserLanguage() == "de")
        {
            LocalisedStrings::setCurrentMappings(new LocalisedStrings(Translations::German, false));
        }

        MainContentComponent::initLookAndFeel();
        LookAndFeel::setDefaultLookAndFeel(MainContentComponent::s_defaultLookAndFeel);
        m_mainWindow = new MainWindow(m_applicationProperties);
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

        MainWindow(ApplicationProperties& properties)
            : DocumentWindow("M*Player",
                             Colours::lightgrey,
                             DocumentWindow::allButtons)
        {
            MainContentComponent* main = new MainContentComponent(properties, &commandManager);

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
    ApplicationProperties m_applicationProperties;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(MStarPlayerApplication)
