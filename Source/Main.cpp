#include "MainComponent.h"

#include "juce_gui_basics/juce_gui_basics.h"

#include "BinaryData.h"

#include "CrashDumper.h"

class MStarPlayerApplication : public juce::JUCEApplication
{
public:
    MStarPlayerApplication() {}
    const juce::String getApplicationName()
    {
        return JUCE_APPLICATION_NAME_STRING;
    }
    const juce::String getApplicationVersion()
    {
        return JUCE_APPLICATION_VERSION_STRING;
    }
    bool moreThanOneInstanceAllowed()
    {
        return true;
    }
    void initialise(const juce::String& /*commandLine*/)
    {
#ifdef _WIN32
        CrashDumper::init();
#endif

        juce::PropertiesFile::Options options;
        options.applicationName = "MStarPlayer";
        options.filenameSuffix = ".settings";
#ifdef JUCE_LINUX
        if (const char* path = std::getenv("XDG_CONFIG_HOME"))
        {
            options.folderName = path + juce::String("/MStarPlayer");
        }
        else if (const char* home = std::getenv("HOME"))
        {
            options.folderName = home + juce::String("/.config/MStarPlayer");
        }
#endif
        m_applicationProperties.setStorageParameters(options);

        if (m_applicationProperties.getUserSettings()->getValue("language") == "de")
        {
            juce::LocalisedStrings::setCurrentMappings(
                new juce::LocalisedStrings(juce::CharPointer_UTF8(BinaryData::German_txt), false));
        }
        else if (m_applicationProperties.getUserSettings()->getValue("language") == "en")
        {
            // Default is English.
        }
        else if (juce::SystemStats::getUserLanguage() == "de")
        {
            juce::LocalisedStrings::setCurrentMappings(
                new juce::LocalisedStrings(juce::CharPointer_UTF8(BinaryData::German_txt), false));
        }

        MainContentComponent::initLookAndFeel();
        juce::LookAndFeel::setDefaultLookAndFeel(MainContentComponent::s_defaultLookAndFeel);
        m_mainWindow = std::make_unique<MainWindow>(m_applicationProperties);

        juce::StringArray parameters = juce::JUCEApplication::getCommandLineParameterArray();
        if (!parameters.isEmpty())
        {
            m_mainWindow->openProject(juce::File(parameters[0]));
        }
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

    void anotherInstanceStarted(const juce::String& /*commandLine*/) {}

    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        // the command manager object used to dispatch command events
        juce::ApplicationCommandManager commandManager;

        MainWindow(juce::ApplicationProperties& properties)
            : juce::DocumentWindow("M*Player", juce::Colours::lightgrey, juce::DocumentWindow::allButtons)
        {
            MainContentComponent* main = new MainContentComponent(properties, &commandManager);

            commandManager.registerAllCommandsForTarget(main);
            commandManager.registerAllCommandsForTarget(juce::JUCEApplication::getInstance());

            addKeyListener(commandManager.getKeyMappings());

            setContentOwned(main, true);

            setMenuBar(main);

            main->setApplicationCommandManagerToWatch(&commandManager);

            if (properties.getUserSettings()->getValue("lookAndFeel") == "dark")
            {
                main->switchToDarkLookAndFeel();
            }
            else if (properties.getUserSettings()->getValue("lookAndFeel") == "pink")
            {
                main->switchToPinkLookAndFeel();
            }

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
            static_cast<MainContentComponent*>(getContentComponent())->askSaveProject(&MainContentComponent::quit);
        }

        void openProject(juce::File projectFile)
        {
            static_cast<MainContentComponent*>(getContentComponent())->openProject(projectFile);
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> m_mainWindow;
    juce::ApplicationProperties m_applicationProperties;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(MStarPlayerApplication)
