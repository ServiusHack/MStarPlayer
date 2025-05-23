#include "MainComponent.h"

#include <assert.h>

#include "BinaryData.h"

#include "CDPlayer.h"
#include "Player.h"

namespace
{
const int MaxRecentlyUsedFiles = 5;
}

DefaultLookAndFeel* MainContentComponent::s_defaultLookAndFeel;
DarkLookAndFeel* MainContentComponent::s_darkLookAndFeel;
PinkLookAndFeel* MainContentComponent::s_pinkLookAndFeel;

void MainContentComponent::initLookAndFeel()
{
    s_defaultLookAndFeel = new DefaultLookAndFeel();
    s_darkLookAndFeel = new DarkLookAndFeel();
    s_pinkLookAndFeel = new PinkLookAndFeel();
}

void MainContentComponent::destroyLookAndFeel()
{
    delete s_defaultLookAndFeel;
    delete s_darkLookAndFeel;
    delete s_pinkLookAndFeel;
}

void MainContentComponent::switchToDefaultLookAndFeel()
{
    juce::LookAndFeel::setDefaultLookAndFeel(s_defaultLookAndFeel);
    m_multiDocumentPanel->setBackgroundColour(juce::Colours::lightblue);
    static_cast<juce::DocumentWindow*>(getParentComponent())->setBackgroundColour(juce::Colours::lightgrey);
}

void MainContentComponent::switchToDarkLookAndFeel()
{
    juce::LookAndFeel::setDefaultLookAndFeel(s_darkLookAndFeel);
    m_multiDocumentPanel->setBackgroundColour(
        s_darkLookAndFeel->findColour(juce::ResizableWindow::backgroundColourId).darker());
    static_cast<juce::DocumentWindow*>(getParentComponent())
        ->setBackgroundColour(s_darkLookAndFeel->findColour(juce::ResizableWindow::backgroundColourId));
}

void MainContentComponent::switchToPinkLookAndFeel()
{
    juce::LookAndFeel::setDefaultLookAndFeel(s_pinkLookAndFeel);
    m_multiDocumentPanel->setBackgroundColour(juce::Colours::lightpink);
    static_cast<juce::DocumentWindow*>(getParentComponent())
        ->setBackgroundColour(s_pinkLookAndFeel->findColour(juce::ResizableWindow::backgroundColourId));
}

MainContentComponent::MainContentComponent(
    juce::ApplicationProperties& applicationPropeties, juce::ApplicationCommandManager* commandManager)
    : m_timeSliceThread("read ahead")
    , m_commandManager(commandManager)
    , m_projectModified(false)
    , m_audioThumbnailCache(1000)
    , m_applicationProperties(applicationPropeties)
    , m_multiDocumentPanel(std::make_unique<MyMultiDocumentPanel>())
    , m_outputChannelNames(std::make_unique<OutputChannelNames>(m_audioDeviceManager))
    , m_mixerComponent(
          std::make_unique<MixerComponent>(&m_audioDeviceManager, m_outputChannelNames.get(), m_soloBusSettings))
    , m_testToneGenerator(
          std::make_unique<TestToneGeneratorComponent>(m_mixerComponent.get(), m_outputChannelNames.get()))
    , m_soloComponent(
          std::make_unique<SoloBusMixer>(m_soloBusSettings, m_mixerComponent->getChannelVolumeAudioSource()))
    , m_pluginLoader(m_multiDocumentPanel.get())
{
    // audio setup
    m_audioDeviceManager.initialise(0, 64, nullptr, false, {}, 0);

    // mixer control
    addAndMakeVisible(m_mixerComponent.get());

    // solo bus control
    addAndMakeVisible(m_soloComponent.get());

    // player MDI area
    addAndMakeVisible(m_multiDocumentPanel.get());
    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::FloatingWindows);

    m_timeSliceThread.startThread(juce::Thread::Priority::low);

    setSize(700, 600);

    m_soloBusSettings.addListener(this);

    m_recentlyOpenedFiles.setMaxNumberOfItems(MaxRecentlyUsedFiles);
    m_recentlyOpenedFiles.restoreFromString(m_applicationProperties.getUserSettings()->getValue("recentlyUsed"));

    reconfigSnapToGrid();
}

MainContentComponent::~MainContentComponent()
{
    m_soloBusSettings.removeListener(this);
    delete m_testToneGenerator.release();
    delete m_multiDocumentPanel.release();
    delete m_mixerComponent.release();
    m_applicationProperties.closeFiles();
}

void MainContentComponent::reconfigSnapToGrid()
{
    const bool snapToGrid = m_applicationProperties.getUserSettings()->getBoolValue("snapToGrid", false);
    const int gridWidth = m_applicationProperties.getUserSettings()->getIntValue("snapToGridWidth", 1);
    const int gridHeight = m_applicationProperties.getUserSettings()->getIntValue("snapToGridHeight", 1);
    m_multiDocumentPanel->reconfigSnapToGrid(snapToGrid, gridWidth, gridHeight);
}

void MainContentComponent::quit()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainContentComponent::resized()
{
    auto bounds = getLocalBounds();

    if (m_mixerComponent->isVisible())
    {
        m_mixerComponent->setBounds(bounds.removeFromBottom(m_mixerComponent->getHeight()));
    }

    if (m_soloBusSettings.isConfigured())
    {
        const int soloComponentWidth = 100;
        m_soloComponent->setBounds(bounds.removeFromRight(soloComponentWidth));
    }
    else
    {
        m_soloComponent->setBounds(0, 0, 0, 0);
    }

    m_multiDocumentPanel->setBounds(bounds);
}

juce::StringArray MainContentComponent::getMenuBarNames()
{
    static const juce::String menuBarNames[] = {TRANS("Project"), TRANS("Player"), TRANS("View"), TRANS("Options")};
    juce::StringArray names(menuBarNames, 4);
    if (m_pluginLoader.count() > 0)
        names.add("Plugins");
    names.add("Info");
    return names;
}

juce::PopupMenu MainContentComponent::getMenuForIndex(int menuIndex, const juce::String& /*menuName*/)
{
    juce::PopupMenu menu;

    switch (menuIndex)
    {
    case 0:
        menu.addCommandItem(m_commandManager, projectNew);
        menu.addCommandItem(m_commandManager, projectOpen);
        menu.addCommandItem(m_commandManager, projectSave);
        menu.addCommandItem(m_commandManager, projectSaveAs);
        if (m_recentlyOpenedFiles.getNumFiles() > 0)
        {
            menu.addSeparator();
            m_recentlyOpenedFiles.createPopupMenuItems(menu, projectRecentlyUsedFiles, false, false);
        }
        menu.addSeparator();
        menu.addCommandItem(m_commandManager, juce::StandardApplicationCommandIDs::quit);
        break;
    case 1:
        menu.addCommandItem(m_commandManager, addJinglePlayer);
        menu.addCommandItem(m_commandManager, addMultitrackPlayer);
        menu.addCommandItem(m_commandManager, addPlaylistPlayer);
        menu.addCommandItem(m_commandManager, addCDPlayer);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager, showTestToneGenerator);
        break;
    case 2:
    {
        menu.addCommandItem(m_commandManager, layoutModeFloating);
        menu.addCommandItem(m_commandManager, layoutModeTabs);

        juce::PopupMenu stylePopupMenu;
        stylePopupMenu.addCommandItem(m_commandManager, lookAndFeelDefault);
        stylePopupMenu.addCommandItem(m_commandManager, lookAndFeelDark);
        stylePopupMenu.addCommandItem(m_commandManager, lookAndFeelPink);
        menu.addSubMenu(TRANS("Style"), stylePopupMenu);

        menu.addCommandItem(m_commandManager, showMixer);
    }
    break;
    case 3:
        menu.addCommandItem(m_commandManager, configureAudio);
        menu.addCommandItem(m_commandManager, configureMidi);
        menu.addCommandItem(m_commandManager, editSettings);
        break;
    }
    if (menuIndex == 4 && m_pluginLoader.count() > 0)
    {
        const size_t numberOfPlugins = m_pluginLoader.count();
        for (size_t i = 0; i < numberOfPlugins; ++i)
            menu.addCommandItem(m_commandManager, basePlugin + i);
    }
    else if (menuIndex == 4 && m_pluginLoader.count() == 0 || menuIndex == 5)
    {
        menu.addCommandItem(m_commandManager, applicationInfo);
    }

    return menu;
}

void MainContentComponent::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/)
{
    if (menuItemID >= projectRecentlyUsedFiles && menuItemID < projectRecentlyUsedFiles + MaxRecentlyUsedFiles)
    {
        m_projectFile = m_recentlyOpenedFiles.getFile(menuItemID - projectRecentlyUsedFiles);
        m_recentlyOpenedFiles.addFile(m_projectFile);
        m_applicationProperties.getUserSettings()->setValue("recentlyUsed", m_recentlyOpenedFiles.toString());
        readProjectFile();
    }
}

// The following methods implement the ApplicationCommandTarget interface, allowing
// this window to publish a set of actions it can perform, and which can be mapped
// onto menus, keypresses, etc.

juce::ApplicationCommandTarget* MainContentComponent::getNextCommandTarget()
{
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void MainContentComponent::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const juce::CommandID ids[] = {projectNew,
        projectOpen,
        projectSave,
        projectSaveAs,
        addJinglePlayer,
        addMultitrackPlayer,
        addPlaylistPlayer,
        addCDPlayer,
        showTestToneGenerator,
        layoutModeFloating,
        layoutModeTabs,
        showMixer,
        configureAudio,
        configureMidi,
        editSettings,
        lookAndFeelDefault,
        lookAndFeelDark,
        lookAndFeelPink};

    commands.addArray(ids, juce::numElementsInArray(ids));

    const size_t numberOfPlugins = m_pluginLoader.count();
    for (size_t i = 0; i < numberOfPlugins; ++i)
        commands.add(basePlugin + i);

    commands.add(applicationInfo);
}

// This method is used when something needs to find out the details about one of the commands
// that this object can perform..
void MainContentComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    static const juce::String projectCategory("Project");
    static const juce::String playerCategory("Player");
    static const juce::String viewCategory("View");
    static const juce::String optionsCategory("Options");
    static const juce::String pluginsCategory("Plugins");
    static const juce::String infoCategory("Info");

    switch (commandID)
    {
    case projectNew:
        result.setInfo(TRANS("New"), TRANS("Create a new project"), projectCategory, 0);
        result.addDefaultKeypress('N', juce::ModifierKeys::commandModifier);
        break;

    case projectOpen:
        result.setInfo(TRANS("Open"), TRANS("Open an existing project"), projectCategory, 0);
        result.addDefaultKeypress('O', juce::ModifierKeys::commandModifier);
        break;

    case projectSave:
        result.setInfo(TRANS("Save"), TRANS("Save the current project"), projectCategory, 0);
        result.addDefaultKeypress('S', juce::ModifierKeys::commandModifier);
        break;

    case projectSaveAs:
        result.setInfo(TRANS("Save as ..."), TRANS("Save the current project under ..."), projectCategory, 0);
        break;

    case addJinglePlayer:
        result.setInfo(TRANS("Add Jingle Player"), TRANS("Add a simple player"), playerCategory, 0);
        result.addDefaultKeypress('J', juce::ModifierKeys::commandModifier);
        break;

    case addMultitrackPlayer:
        result.setInfo(TRANS("Add Multitrack Player"), TRANS("Add a player with multiple tracks"), playerCategory, 0);
        result.addDefaultKeypress('M', juce::ModifierKeys::commandModifier);
        break;

    case addPlaylistPlayer:
        result.setInfo(TRANS("Add Playlist Player"), TRANS("Add a player with a playlist"), playerCategory, 0);
        result.addDefaultKeypress('P', juce::ModifierKeys::commandModifier);
        break;

    case addCDPlayer:
        result.setInfo(TRANS("Add CD Player"), TRANS("Add a player for audio CDs"), playerCategory, 0);
        result.addDefaultKeypress('A', juce::ModifierKeys::commandModifier);
        break;

    case showTestToneGenerator:
        result.setInfo(TRANS("Show Test Tone Generator"),
            TRANS("Show a window from which test tones can be generated on the output channels"),
            playerCategory,
            0);
        result.addDefaultKeypress('T', juce::ModifierKeys::commandModifier);
        break;

    case layoutModeFloating:
        result.setInfo(TRANS("Windows"), TRANS("Players are floating windows"), viewCategory, 0);
        result.setTicked(m_multiDocumentPanel->getLayoutMode() == MyMultiDocumentPanel::FloatingWindows);
        break;

    case layoutModeTabs:
        result.setInfo(TRANS("Tabs"), TRANS("Players are tabs"), viewCategory, 0);
        result.setTicked(m_multiDocumentPanel->getLayoutMode() == MyMultiDocumentPanel::MaximisedWindowsWithTabs);
        break;

    case showMixer:
        result.setInfo(TRANS("Show Mixer"), TRANS("Show the mixer in the lower area"), viewCategory, 0);
        result.setTicked(m_mixerComponent->isVisible());
        break;

    case configureAudio:
        result.setInfo(TRANS("Configure Audio"), TRANS("Configure the audio device to use"), optionsCategory, 0);
        break;

    case configureMidi:
        result.setInfo(TRANS("Configure MIDI"), TRANS("Configure the MIDI device to use"), optionsCategory, 0);
        break;

    case editSettings:
        result.setInfo(TRANS("Edit Settings"), TRANS("Edit the application settings"), optionsCategory, 0);
        break;

    case lookAndFeelDefault:
        result.setInfo(TRANS("Standard"), TRANS("Use the default look and feel"), viewCategory, 0);
        result.setTicked(&juce::LookAndFeel::getDefaultLookAndFeel() == s_defaultLookAndFeel);
        break;

    case lookAndFeelDark:
        result.setInfo(TRANS("Dark"), TRANS("Use a dark look and feel"), viewCategory, 0);
        result.setTicked(&juce::LookAndFeel::getDefaultLookAndFeel() == s_darkLookAndFeel);
        break;

    case lookAndFeelPink:
        result.setInfo(TRANS("Pink"), TRANS("Use a pink look and feel"), viewCategory, 0);
        result.setTicked(&juce::LookAndFeel::getDefaultLookAndFeel() == s_pinkLookAndFeel);
        break;

    case applicationInfo:
        result.setInfo(TRANS("About"), TRANS("Show information about the application"), infoCategory, 0);
        break;
    };

    if (commandID >= basePlugin)
    {
        juce::String pluginName = m_pluginLoader.pluginName(commandID - basePlugin);
        result.setInfo(pluginName, juce::String("Configure ") + pluginName, pluginsCategory, 0);
    }
}

// this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
bool MainContentComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
    case projectNew:
        requestNewProject();
        break;
    case projectOpen:
        requestOpenProject();
        break;
    case projectSave:
        saveProject();
        break;
    case projectSaveAs:
        saveAsProject();
        break;
    case addJinglePlayer:
    {
        Player* player = new Player(m_mixerComponent.get(),
            m_outputChannelNames.get(),
            m_soloBusSettings,
            InterPlayerCommunication::PlayerType::Jingle,
            m_applicationProperties,
            m_audioThumbnailCache,
            m_timeSliceThread,
            m_mtcSender,
            m_pluginLoader);
        player->setName("Jingle Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, juce::Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addMultitrackPlayer:
    {
        Player* player = new Player(m_mixerComponent.get(),
            m_outputChannelNames.get(),
            m_soloBusSettings,
            InterPlayerCommunication::PlayerType::Multitrack,
            m_applicationProperties,
            m_audioThumbnailCache,
            m_timeSliceThread,
            m_mtcSender,
            m_pluginLoader);
        player->setName("Multitrack Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, juce::Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addPlaylistPlayer:
    {
        Player* player = new Player(m_mixerComponent.get(),
            m_outputChannelNames.get(),
            m_soloBusSettings,
            InterPlayerCommunication::PlayerType::Playlist,
            m_applicationProperties,
            m_audioThumbnailCache,
            m_timeSliceThread,
            m_mtcSender,
            m_pluginLoader);
        player->setName("Playlist Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, juce::Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addCDPlayer:
    {
        CDPlayer* player = new CDPlayer(
            m_mixerComponent.get(), m_outputChannelNames.get(), m_soloBusSettings, m_timeSliceThread, m_pluginLoader);
        player->setName("CD Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, juce::Colours::white, true);
        m_projectModified = true;
    }
    break;
    case showTestToneGenerator:
    {
        if (!m_testToneGeneratorWindow)
        {
            juce::DialogWindow::LaunchOptions options;
            options.dialogTitle = TRANS("Test Tone Generator");
            options.resizable = false;
            options.content.setNonOwned(m_testToneGenerator.get());
            options.componentToCentreAround = this;
            options.useNativeTitleBar = false;
            m_testToneGeneratorWindow.reset(options.create());
        }
        m_testToneGeneratorWindow->setVisible(true);
    }
    break;
    case layoutModeFloating:
        m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::FloatingWindows);
        break;
    case layoutModeTabs:
        m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::MaximisedWindowsWithTabs);
        break;
    case showMixer:
        m_mixerComponent->setVisible(!m_mixerComponent->isVisible());
        resized();
        break;

    case configureAudio:
        m_audioConfigurationWindow = std::make_unique<AudioConfigurationWindow>(
            m_audioDeviceManager, *m_outputChannelNames, m_soloBusSettings);
        break;
    case configureMidi:
        m_midiConfigurationWindow = std::make_unique<MidiConfigurationWindow>(m_mtcSender);
        break;
    case editSettings:
        m_editSettingsWindow
            = std::make_unique<EditSettingsWindow>(m_applicationProperties, [this]() { reconfigSnapToGrid(); });
        break;

    case lookAndFeelDefault:
        switchToDefaultLookAndFeel();
        m_applicationProperties.getUserSettings()->setValue("lookAndFeel", "default");
        break;
    case lookAndFeelDark:
        switchToDarkLookAndFeel();
        m_applicationProperties.getUserSettings()->setValue("lookAndFeel", "dark");
        break;
    case lookAndFeelPink:
        switchToPinkLookAndFeel();
        m_applicationProperties.getUserSettings()->setValue("lookAndFeel", "pink");
        break;
    case applicationInfo:
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
            "M*Player",
            TRANS("Multi-channel, multi-track, multi-player") + "\n" + TRANS("player for audio files") + "\n\n"
                + TRANS("Version: ") + JUCE_APPLICATION_VERSION_STRING);
        break;
    default:
        if (info.commandID >= basePlugin)
        {
            m_pluginLoader.configure(info.commandID - basePlugin);
            return true;
        }
        return false;
    };

    return true;
}

void MainContentComponent::requestNewProject()
{
    askSaveProject(&MainContentComponent::uncheckedNewProject);
}

void MainContentComponent::uncheckedNewProject()
{
    getParentComponent()->setName("M*Player");
    m_multiDocumentPanel->closeAllDocuments(true);
    m_projectModified = false;
}

void MainContentComponent::openProject(juce::File projectFile)
{
    m_projectFile = projectFile;
    m_recentlyOpenedFiles.addFile(m_projectFile);
    m_applicationProperties.getUserSettings()->setValue("recentlyUsed", m_recentlyOpenedFiles.toString());
    readProjectFile();
}

void MainContentComponent::requestOpenProject()
{
    askSaveProject(&MainContentComponent::uncheckedOpenProject);
}

void MainContentComponent::uncheckedOpenProject()
{
    m_currentFileChooser.emplace(TRANS("Please select the project file you want to load ..."), juce::File(), "*.aupp");

    m_currentFileChooser->launchAsync(juce::FileBrowserComponent::openMode,
        [this](const juce::FileChooser& chooser)
        {
            if (chooser.getResult() == juce::File())
                return;

            openProject(chooser.getResult());
        });
}

void MainContentComponent::askSaveProject(ContinueCallback callback)
{
    if (!m_projectModified)
    {
        std::invoke(callback, this);
        return;
    }

    juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                                     .withIconType(juce::AlertWindow::QuestionIcon)
                                     .withTitle(TRANS("Save project?"))
                                     .withMessage(TRANS("Do you want to save the current project?"))
                                     .withButton(TRANS("Yes"))
                                     .withButton(TRANS("No"))
                                     .withButton(TRANS("Cancel"))
                                     .withAssociatedComponent(this),
        [this, callback](int result)
        {
            switch (result)
            {
            case 1:
                saveProject(callback);
                break;
            case 2:
                std::invoke(callback, this);
                break;
            }
        });
}

void MainContentComponent::saveProject(std::optional<ContinueCallback> callback)
{
    if (m_projectFile == juce::File())
    {
        saveAsProject(callback);
        return;
    }

    writeProjectFile();
    if (callback)
        std::invoke(*callback, this);
}

void MainContentComponent::saveAsProject(std::optional<ContinueCallback> callback)
{
    m_currentFileChooser.emplace(TRANS("Please select the project file you want to save ..."), juce::File(), "*.aupp");

    m_currentFileChooser->launchAsync(
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, callback](const juce::FileChooser& chooser)
        {
            if (chooser.getResult() == juce::File())
                return;

            m_projectFile = juce::File(chooser.getResult());
            m_recentlyOpenedFiles.addFile(m_projectFile);
            m_applicationProperties.getUserSettings()->setValue("recentlyUsed", m_recentlyOpenedFiles.toString());
            writeProjectFile();
            if (callback)
                std::invoke(*callback, this);
        });
}

void MainContentComponent::readProjectFile()
{
    m_multiDocumentPanel->closeAllDocuments(false);

    getParentComponent()->setName(m_projectFile.getFileNameWithoutExtension());

    juce::StringArray loadWarnings;

    juce::FileInputStream stream(m_projectFile.getSiblingFile(m_projectFile.getFileNameWithoutExtension() + ".aupc"));
    if (stream.failedToOpen())
    {
        loadWarnings.add(TRANS("Unable to open audio thumbnail cache file."));
    }
    m_audioThumbnailCache.readFromStream(stream);
    if (stream.getStatus().failed())
    {
        loadWarnings.add(TRANS("Unable to load audio thumbnails."));
    }

    try
    {
        juce::XmlDocument document(m_projectFile);

        std::unique_ptr<juce::XmlElement> root{document.getDocumentElement()};

        if (!root)
        {
            juce::String error = document.getLastParseError();
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon, TRANS("Failed to open project file"), error);
            return;
        }

        juce::XmlElement* view = root->getChildByName("View");
        if (view == nullptr)
            loadWarnings.add(TRANS("No view settings found, using default."));
        else
        {
            juce::XmlElement* layoutModeElement = view->getChildByName("LayoutMode");
            if (layoutModeElement == nullptr)
                loadWarnings.add(TRANS("No layout mode settings found, using default."));
            else
            {
                juce::String layoutMode = layoutModeElement->getAllSubText().trim();
                if (layoutMode == "windows")
                    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::FloatingWindows);
                else if (layoutMode == "tabs")
                    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::MaximisedWindowsWithTabs);
                else
                    loadWarnings.add(TRANS("Unknown view layout, using default."));
            }

            juce::XmlElement* styleElement = view->getChildByName("Style");
            if (styleElement == nullptr)
                loadWarnings.add(TRANS("No style settings found, using default."));
            else
            {
                juce::String style = styleElement->getAllSubText().trim();
                if (style == "default")
                    perform(juce::ApplicationCommandTarget::InvocationInfo(lookAndFeelDefault));
                else if (style == "dark")
                    perform(juce::ApplicationCommandTarget::InvocationInfo(lookAndFeelDark));
                else if (style == "pink")
                    perform(juce::ApplicationCommandTarget::InvocationInfo(lookAndFeelPink));
                else
                    loadWarnings.add(TRANS("Unknown style, using default."));
            }
        }

        juce::XmlElement* audio = root->getChildByName("Audio");
        if (audio == nullptr)
            loadWarnings.add(TRANS("No audio settings found, using current."));
        else
        {
            if (audio->getNumChildElements() > 0)
            {
                juce::String error = m_audioDeviceManager.initialise(0, 64, audio->getChildElement(0), false, {}, 0);
                if (error != "")
                {
                    loadWarnings.add(error);

                    error = m_audioDeviceManager.initialise(0, 64, nullptr, false, {}, 0);

                    if (error != "")
                        throw std::runtime_error(error.toRawUTF8());
                }

                // AudioDeviceManager only notifies asynchronously but we want to continue loading the project now.
                // So we fake a change broadcast by the AudioDeviceManager so that OutputChannelNames is configured
                // correctly.
                m_outputChannelNames->changeListenerCallback(&m_audioDeviceManager);
            }
        }

        juce::XmlElement* midi = root->getChildByName("MIDI");
        if (midi != nullptr)
        {
            juce::StringArray names;
            if (midi->getNumChildElements() == 1 && midi->getChildElement(0)->isTextElement())
            {
                // XML file is in the old format that stored only one device.
                names.add(midi->getAllSubText().trim());
            }
            else
            {
                for (int i = 0; i < midi->getNumChildElements(); ++i)
                {
                    names.add(midi->getChildElement(i)->getAllSubText().trim());
                }
            }

            juce::Array<juce::MidiDeviceInfo> deviceInfos = juce::MidiOutput::getAvailableDevices();
            deviceInfos.removeIf(
                [&names](const juce::MidiDeviceInfo& deviceInfo) { return !names.contains(deviceInfo.name); });

            m_mtcSender.setDevices(deviceInfos);
        }

        juce::XmlElement* channelNames = root->getChildByName("ChannelNames");
        if (channelNames == nullptr)
            loadWarnings.add(TRANS("No channel names found, using device defaults."));
        else
        {
            m_outputChannelNames->restoreFromXml(*channelNames);
        }

        juce::XmlElement* soloBusSettings = root->getChildByName("SoloBusSettings");
        if (soloBusSettings == nullptr)
            loadWarnings.add(TRANS("No solo bus settings found, using no solo bus."));
        else
        {
            m_soloBusSettings.restoreFromXml(*soloBusSettings);
        }

        juce::XmlElement* soloMixer = root->getChildByName("SoloBusMixer");
        if (soloMixer == nullptr)
            loadWarnings.add(TRANS("No solo mixer settings found, using default volumes."));
        else
        {
            m_soloComponent->restoreFromXml(*soloMixer);
        }

        juce::XmlElement* mixer = root->getChildByName("Mixer");

        if (mixer == nullptr)
            loadWarnings.add(TRANS("No mixer settings found, using current."));
        else
            m_mixerComponent->restoreFromXml(*mixer);

        juce::XmlElement* players = root->getChildByName("Players");

        if (players == nullptr)
            loadWarnings.add(TRANS("No players found. None will be loaded."));
        else
        {
            for (int i = 0; i < players->getNumChildElements(); i++)
            {
                using namespace InterPlayerCommunication;

                juce::XmlElement* player = players->getChildElement(i);
                if (player->getTagName() == "Player")
                {
                    const float gain = static_cast<float>(player->getDoubleAttribute("gain", 1.0));
                    const bool solo = player->getBoolAttribute("solo");
                    const bool mute = player->getBoolAttribute("mute");
                    PlayerType playerType = PlayerType::Playlist;
                    juce::String type = player->getStringAttribute("type", "");
                    if (type == "jingle")
                    {
                        playerType = PlayerType::Jingle;
                    }
                    else if (type == "multitrack")
                    {
                        playerType = PlayerType::Multitrack;
                    }
                    else if (type != "playlist")
                    {
                        loadWarnings.add(juce::String::formatted(TRANS("Unknown player type '%s'."), type));
                        continue;
                    }
                    Player* window = new Player(m_mixerComponent.get(),
                        m_outputChannelNames.get(),
                        m_soloBusSettings,
                        playerType,
                        m_applicationProperties,
                        m_audioThumbnailCache,
                        m_timeSliceThread,
                        m_mtcSender,
                        m_pluginLoader,
                        gain,
                        solo,
                        mute);
                    window->addChangeListener(this);
                    window->setName(" "); // Satisfy JUCE, we set the actual name later.
                    m_multiDocumentPanel->addDocument(window, juce::Colours::white, true);
                    window->restoreFromXml(*player, m_projectFile.getParentDirectory());
                }
                else if (player->getTagName() == "CDPlayer")
                {
                    const float gain = static_cast<float>(player->getDoubleAttribute("gain", 1.0));
                    const bool solo = player->getBoolAttribute("solo");
                    const bool mute = player->getBoolAttribute("mute");
                    CDPlayer* window = new CDPlayer(m_mixerComponent.get(),
                        m_outputChannelNames.get(),
                        m_soloBusSettings,
                        m_timeSliceThread,
                        m_pluginLoader,
                        gain,
                        solo,
                        mute);
                    window->addChangeListener(this);
                    window->setName(" "); // Satisfy JUCE, we set the actual name later.
                    m_multiDocumentPanel->addDocument(window, juce::Colours::white, true);
                    window->restoreFromXml(*player, m_projectFile.getParentDirectory());
                }
                else
                {
                    loadWarnings.add(
                        juce::String::formatted(TRANS("Unknown tag '%s' in players list."), player->getTagName()));
                }
            }

            // update soloMute for all players
            soloChanged(false);
        }

        juce::XmlElement* plugins = root->getChildByName("Plugins");
        if (plugins)
        {
            m_pluginLoader.loadConfigurations(plugins);
        }

        m_projectModified = false;

        if (loadWarnings.size() > 0)
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                TRANS("Problems while opening the project"),
                loadWarnings.joinIntoString("\n"));
    }
    catch (const std::exception& e)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, TRANS("Failed opening the project"), juce::String::fromUTF8(e.what()));
        m_projectModified = false;
        m_multiDocumentPanel->closeAllDocuments(false);
    }
}

void MainContentComponent::writeProjectFile()
{
    getParentComponent()->setName(m_projectFile.getFileNameWithoutExtension());

    std::unique_ptr<juce::XmlElement> root = std::make_unique<juce::XmlElement>("Project");

    juce::XmlElement* view = new juce::XmlElement("View");

    juce::XmlElement* layoutMode = new juce::XmlElement("LayoutMode");
    switch (m_multiDocumentPanel->getLayoutMode())
    {
    case juce::MultiDocumentPanel::FloatingWindows:
        layoutMode->addTextElement("windows");
        break;
    case juce::MultiDocumentPanel::MaximisedWindowsWithTabs:
        layoutMode->addTextElement("tabs");
        break;
    }
    view->addChildElement(layoutMode);

    juce::XmlElement* style = new juce::XmlElement("Style");
    if (&juce::LookAndFeel::getDefaultLookAndFeel() == s_defaultLookAndFeel)
        style->addTextElement("default");
    else if (&juce::LookAndFeel::getDefaultLookAndFeel() == s_darkLookAndFeel)
        style->addTextElement("dark");
    else if (&juce::LookAndFeel::getDefaultLookAndFeel() == s_pinkLookAndFeel)
        style->addTextElement("pink");
    view->addChildElement(style);

    root->addChildElement(view);

    juce::XmlElement* audio = new juce::XmlElement("Audio");
    audio->addChildElement(m_audioDeviceManager.createStateXml().release());
    root->addChildElement(audio);

    if (auto devices = m_mtcSender.getDevices(); !devices.isEmpty())
    {
        juce::XmlElement* midi = new juce::XmlElement("MIDI");
        for (auto& device : devices)
        {
            juce::XmlElement* element = new juce::XmlElement("device");
            element->addTextElement(device.name);
            midi->addChildElement(element);
        }
        root->addChildElement(midi);
    }

    juce::XmlElement* channelNames = new juce::XmlElement("ChannelNames");
    m_outputChannelNames->saveToXml(channelNames);
    root->addChildElement(channelNames);

    juce::XmlElement* soloBusSettings = new juce::XmlElement("SoloBusSettings");
    m_soloBusSettings.saveToXml(soloBusSettings);
    root->addChildElement(soloBusSettings);

    juce::XmlElement* soloBusMixer = new juce::XmlElement("SoloBusMixer");
    m_soloComponent->saveToXml(soloBusMixer);
    root->addChildElement(soloBusMixer);

    juce::XmlElement* mixer = new juce::XmlElement("Mixer");
    m_mixerComponent->saveToXml(mixer);
    root->addChildElement(mixer);

    juce::XmlElement* players = new juce::XmlElement("Players");

    for (int i = 0; i < m_multiDocumentPanel->getNumDocuments(); ++i)
    {
        if (Player* player = dynamic_cast<Player*>(m_multiDocumentPanel->getDocument(i)))
            players->addChildElement(
                player->saveToXml(m_projectFile.getParentDirectory(), m_multiDocumentPanel->getLayoutMode()));
        else if (CDPlayer* cdPlayer = dynamic_cast<CDPlayer*>(m_multiDocumentPanel->getDocument(i)))
            players->addChildElement(
                cdPlayer->saveToXml(m_projectFile.getParentDirectory(), m_multiDocumentPanel->getLayoutMode()));
        else
            assert(false && "Unknown player in multiDocumentPanel");
    }

    root->addChildElement(players);

    if (m_pluginLoader.count() > 0)
    {
        juce::XmlElement* plugins = new juce::XmlElement("Plugins");
        m_pluginLoader.saveConfigurations(plugins);
        root->addChildElement(plugins);
    }

    if (!root->writeTo(m_projectFile))
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            TRANS("Failed to save project file"),
            TRANS("Failed to save project file."));
    else
        m_projectModified = false;

    juce::FileOutputStream stream(m_projectFile.getSiblingFile(m_projectFile.getFileNameWithoutExtension() + ".aupc"));
    if (stream.failedToOpen())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            TRANS("Failed to save project"),
            TRANS("Failed to open audio thumbnail cache file."));
        return;
    }
    stream.setPosition(0);
    stream.truncate();
    m_audioThumbnailCache.writeToStream(stream);
    if (stream.getStatus().failed())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, TRANS("Failed to save project"), TRANS("Failed to save audio thumbnails."));
    }
}

void MainContentComponent::soloChanged(bool /*solo*/)
{
    if (m_soloBusSettings.isConfigured())
        return;

    bool soloMute = false;
    for (int i = 0; i < m_multiDocumentPanel->getNumDocuments(); ++i)
    {
        if (Player* player = dynamic_cast<Player*>(m_multiDocumentPanel->getDocument(i)))
            soloMute = player->getSolo();
        else if (CDPlayer* cdPlayer = dynamic_cast<CDPlayer*>(m_multiDocumentPanel->getDocument(i)))
            soloMute = cdPlayer->getSolo();
        else
            assert(false && "Unknown player in multiDocumentPanel");

        if (soloMute)
            break;
    }

    for (int i = 0; i < m_multiDocumentPanel->getNumDocuments(); ++i)
    {
        if (Player* player = dynamic_cast<Player*>(m_multiDocumentPanel->getDocument(i)))
            player->setSoloMute(soloMute);
        else if (CDPlayer* cdPlayer = dynamic_cast<CDPlayer*>(m_multiDocumentPanel->getDocument(i)))
            cdPlayer->setSoloMute(soloMute);
        else
            assert(false && "Unknown player in multiDocumentPanel");
    }
}

void MainContentComponent::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    juce::ignoreUnused(channel, outputChannel, previousOutputChannel);

    resized();
}
