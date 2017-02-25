#include "MainComponent.h"

#include <assert.h>

#include "CDPlayer.h"
#include "Player.h"

namespace
{
    const int MaxRecentlyUsedFiles = 5;
}

DefaultLookAndFeel* MainContentComponent::s_defaultLookAndFeel;
DarkLookAndFeel* MainContentComponent::s_darkLookAndFeel;

void MainContentComponent::initLookAndFeel()
{
    s_defaultLookAndFeel = new DefaultLookAndFeel();
    s_darkLookAndFeel = new DarkLookAndFeel();
}

void MainContentComponent::destroyLookAndFeel()
{
    delete s_defaultLookAndFeel;
    delete s_darkLookAndFeel;
}

MainContentComponent::MainContentComponent(ApplicationProperties& applicationPropeties, ApplicationCommandManager* commandManager)
    : m_timeSliceThread("read ahead")
    , m_commandManager(commandManager)
    , m_projectModified(false)
    , m_audioThumbnailCache(1000)
    , m_applicationProperties(applicationPropeties)
{
    // audio setup
    m_audioDeviceManager = new AudioDeviceManager();
    m_audioDeviceManager->initialise(64, 64, nullptr, false, String::empty, 0);

    // output channel names
    m_outputChannelNames = new OutputChannelNames(*m_audioDeviceManager);

    // mixer control
    m_mixerComponent = new MixerComponent(m_audioDeviceManager, m_outputChannelNames, m_soloBusSettings);
    addAndMakeVisible(m_mixerComponent);

    // solo bus control
    m_soloComponent = new SoloBusMixer(m_soloBusSettings, m_mixerComponent->getChannelVolumeAudioSource());
    addAndMakeVisible(m_soloComponent);

    // player MDI area
    m_multiDocumentPanel = new MyMultiDocumentPanel();
    addAndMakeVisible(m_multiDocumentPanel);
    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::FloatingWindows);

    m_timeSliceThread.startThread(3);

    setSize(700, 600);

    m_soloBusSettings.addListener(this);

    m_recentlyOpenedFiles.setMaxNumberOfItems(MaxRecentlyUsedFiles);
    m_recentlyOpenedFiles.restoreFromString(m_applicationProperties.getUserSettings()->getValue("recentlyUsed"));
}

MainContentComponent::~MainContentComponent()
{
    m_soloBusSettings.removeListener(this);
    delete m_multiDocumentPanel.release();
    delete m_mixerComponent.release();
    m_applicationProperties.closeFiles();
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

StringArray MainContentComponent::getMenuBarNames()
{
    static const String menuBarNames[] = {TRANS("Project"), TRANS("Player"), TRANS("View"), TRANS("Options")};
    return StringArray(menuBarNames, 4);
}

PopupMenu MainContentComponent::getMenuForIndex(int menuIndex, const String& /*menuName*/)
{
    PopupMenu menu;

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
        menu.addCommandItem(m_commandManager, StandardApplicationCommandIDs::quit);
        break;
    case 1:
        menu.addCommandItem(m_commandManager, addJinglePlayer);
        menu.addCommandItem(m_commandManager, addMultitrackPlayer);
        menu.addCommandItem(m_commandManager, addPlaylistPlayer);
        menu.addCommandItem(m_commandManager, addCDPlayer);
        break;
    case 2:
    {
        menu.addCommandItem(m_commandManager, layoutModeFloating);
        menu.addCommandItem(m_commandManager, layoutModeTabs);

        PopupMenu stylePopupMenu;
        stylePopupMenu.addCommandItem(m_commandManager, lookAndFeelDefault);
        stylePopupMenu.addCommandItem(m_commandManager, lookAndFeelDark);
        menu.addSubMenu(TRANS("Style"), stylePopupMenu);

        menu.addCommandItem(m_commandManager, showMixer);
    }
    break;
    case 3:
        menu.addCommandItem(m_commandManager, configureAudio);
        menu.addCommandItem(m_commandManager, editSettings);
        break;
    }

    return menu;
}

void MainContentComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
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

ApplicationCommandTarget* MainContentComponent::getNextCommandTarget()
{
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void MainContentComponent::getAllCommands(Array<CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        projectNew,
        projectOpen,
        projectSave,
        projectSaveAs,
        addJinglePlayer,
        addMultitrackPlayer,
        addPlaylistPlayer,
        addCDPlayer,
        layoutModeFloating,
        layoutModeTabs,
        showMixer,
        configureAudio,
        editSettings,
        lookAndFeelDefault,
        lookAndFeelDark};

    commands.addArray(ids, numElementsInArray(ids));
}

// This method is used when something needs to find out the details about one of the commands
// that this object can perform..
void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
    static const String projectCategory("Project");
    static const String playerCategory("Player");
    static const String viewCategory("View");
    static const String optionsCategory("Options");

    switch (commandID)
    {
    case projectNew:
        result.setInfo(TRANS("New"), TRANS("Create a new project"), projectCategory, 0);
        result.addDefaultKeypress('N', ModifierKeys::commandModifier);
        break;

    case projectOpen:
        result.setInfo(TRANS("Open"), TRANS("Open an existing project"), projectCategory, 0);
        result.addDefaultKeypress('O', ModifierKeys::commandModifier);
        break;

    case projectSave:
        result.setInfo(TRANS("Save"), TRANS("Save the current project"), projectCategory, 0);
        result.addDefaultKeypress('S', ModifierKeys::commandModifier);
        break;

    case projectSaveAs:
        result.setInfo(TRANS("Save as ..."), TRANS("Save the current project under ..."), projectCategory, 0);
        break;

    case addJinglePlayer:
        result.setInfo(TRANS("Add Jingle Player"), TRANS("Add a simple player"), playerCategory, 0);
        result.addDefaultKeypress('J', ModifierKeys::commandModifier);
        break;

    case addMultitrackPlayer:
        result.setInfo(TRANS("Add Multitrack Player"), TRANS("Add a player with multiple tracks"), playerCategory, 0);
        result.addDefaultKeypress('M', ModifierKeys::commandModifier);
        break;

    case addPlaylistPlayer:
        result.setInfo(TRANS("Add Playlist Player"), TRANS("Add a player with a playlist"), playerCategory, 0);
        result.addDefaultKeypress('P', ModifierKeys::commandModifier);
        break;

    case addCDPlayer:
        result.setInfo(TRANS("Add CD Player"), TRANS("Add a player for audio CDs"), playerCategory, 0);
        result.addDefaultKeypress('A', ModifierKeys::commandModifier);
        break;

    case layoutModeFloating:
        result.setInfo(TRANS("Windows"), TRANS("Players are floating windows"), viewCategory, 0);
        result.setTicked(m_multiDocumentPanel->getLayoutMode() == MultiDocumentPanel::FloatingWindows);
        break;

    case layoutModeTabs:
        result.setInfo(TRANS("Tabs"), TRANS("Players are tabs"), viewCategory, 0);
        result.setTicked(m_multiDocumentPanel->getLayoutMode() == MultiDocumentPanel::MaximisedWindowsWithTabs);
        break;

    case showMixer:
        result.setInfo(TRANS("Show Mixer"), TRANS("Show the mixer"), viewCategory, 0);
        result.setTicked(m_mixerComponent->isVisible());
        break;

    case configureAudio:
        result.setInfo(TRANS("Configure Audio"), TRANS("Configure the audio device to use"), optionsCategory, 0);
        break;

    case editSettings:
        result.setInfo(TRANS("Edit Settings"), TRANS("Edit the application settings"), optionsCategory, 0);
        break;

    case lookAndFeelDefault:
        result.setInfo(TRANS("Standard"), TRANS("Use the default look and feel"), viewCategory, 0);
        result.setTicked(&LookAndFeel::getDefaultLookAndFeel() == s_defaultLookAndFeel);
        break;

    case lookAndFeelDark:
        result.setInfo(TRANS("Dark"), TRANS("Use a dark look and feel"), viewCategory, 0);
        result.setTicked(&LookAndFeel::getDefaultLookAndFeel() == s_darkLookAndFeel);
        break;
    };
}

// this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
bool MainContentComponent::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
    case projectNew:
        newProject();
        break;
    case projectOpen:
        openProject();
        break;
    case projectSave:
        saveProject();
        break;
    case projectSaveAs:
        saveAsProject();
        break;
    case addJinglePlayer:
    {
        Player* player = new Player(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, InterPlayerCommunication::PlayerType::Jingle, m_applicationProperties, m_audioThumbnailCache, m_timeSliceThread);
        player->setName("Jingle Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addMultitrackPlayer:
    {
        Player* player = new Player(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, InterPlayerCommunication::PlayerType::Multitrack, m_applicationProperties, m_audioThumbnailCache, m_timeSliceThread);
        player->setName("Multitrack Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addPlaylistPlayer:
    {
        Player* player = new Player(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, InterPlayerCommunication::PlayerType::Playlist, m_applicationProperties, m_audioThumbnailCache, m_timeSliceThread);
        player->setName("Playlist Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, Colours::white, true);
        m_projectModified = true;
    }
    break;
    case addCDPlayer:
    {
        CDPlayer* player = new CDPlayer(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, m_timeSliceThread);
        player->setName("CD Player");
        player->addChangeListener(this);
        m_multiDocumentPanel->addDocument(player, Colours::white, true);
        m_projectModified = true;
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
        m_audioConfigurationWindow = new AudioConfigurationWindow(*m_audioDeviceManager, *m_outputChannelNames, m_soloBusSettings);
        break;
    case editSettings:
        m_editSettingsWindow = new EditSettingsWindow(m_applicationProperties);
        break;

    case lookAndFeelDefault:
        LookAndFeel::setDefaultLookAndFeel(s_defaultLookAndFeel);
        m_multiDocumentPanel->setBackgroundColour(Colours::lightblue);
        static_cast<DocumentWindow*>(getParentComponent())->setBackgroundColour(Colours::lightgrey);
        break;
    case lookAndFeelDark:
        LookAndFeel::setDefaultLookAndFeel(s_darkLookAndFeel);
        m_multiDocumentPanel->setBackgroundColour(s_darkLookAndFeel->findColour(ResizableWindow::backgroundColourId).darker());
        static_cast<DocumentWindow*>(getParentComponent())->setBackgroundColour(s_darkLookAndFeel->findColour(ResizableWindow::backgroundColourId));
        break;
    default:
        return false;
    };

    return true;
}

void MainContentComponent::newProject()
{
    if (!askSaveProject())
        return;

    getParentComponent()->setName("M*Player");
    m_multiDocumentPanel->closeAllDocuments(true);
    m_projectModified = false;
}

void MainContentComponent::openProject()
{
    if (!askSaveProject())
        return;

    FileChooser myChooser(TRANS("Please select the project file you want to load ..."),
                          File::nonexistent,
                          "*.aupp");
    if (myChooser.browseForFileToOpen())
    {
        m_projectFile = File(myChooser.getResult());
        m_recentlyOpenedFiles.addFile(m_projectFile);
        m_applicationProperties.getUserSettings()->setValue("recentlyUsed", m_recentlyOpenedFiles.toString());
        readProjectFile();
    }
}

bool MainContentComponent::askSaveProject()
{
    if (!m_projectModified)
        return true;

    switch (AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon,
                                            TRANS("Save project?"),
                                            TRANS("Do you want to save the current project?"),
                                            TRANS("Yes"),
                                            TRANS("No"),
                                            TRANS("Cancel"),
                                            this))
    {
    case 1:
        return saveProject();
    case 2:
        return true;
    default:
        return false;
    }
}

bool MainContentComponent::saveProject()
{
    if (m_projectFile == File::nonexistent)
        return saveAsProject();

    writeProjectFile();
    return true;
}

bool MainContentComponent::saveAsProject()
{
    FileChooser myChooser(TRANS("Please select the project file you want to save ..."),
                          File::nonexistent,
                          "*.aupp");
    if (!myChooser.browseForFileToSave(true))
        return false;

    m_projectFile = File(myChooser.getResult());
    m_recentlyOpenedFiles.addFile(m_projectFile);
    m_applicationProperties.getUserSettings()->setValue("recentlyUsed", m_recentlyOpenedFiles.toString());
    writeProjectFile();
    return true;
}

void MainContentComponent::readProjectFile()
{
    m_multiDocumentPanel->closeAllDocuments(false);

    getParentComponent()->setName(m_projectFile.getFileNameWithoutExtension());

    StringArray loadWarnings;

    FileInputStream stream(m_projectFile.getSiblingFile(m_projectFile.getFileNameWithoutExtension() + ".aupc"));
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
        XmlDocument document(m_projectFile);

        ScopedPointer<XmlElement> root = document.getDocumentElement();

        if (!root)
        {
            String error = document.getLastParseError();
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Failed to open project file"), error);
            return;
        }

        XmlElement* view = root->getChildByName("View");
        if (view == nullptr)
            loadWarnings.add(TRANS("No view settings found, using default."));
        else
        {
            XmlElement* layoutModeElement = view->getChildByName("LayoutMode");
            if (layoutModeElement == nullptr)
                loadWarnings.add(TRANS("No layout mode settings found, using default."));
            else
            {
                String layoutMode = layoutModeElement->getAllSubText().trim();
                if (layoutMode == "windows")
                    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::FloatingWindows);
                else if (layoutMode == "tabs")
                    m_multiDocumentPanel->setLayoutMode(MyMultiDocumentPanel::MaximisedWindowsWithTabs);
                else
                    loadWarnings.add(TRANS("Unknown view layout, using default."));
            }

            XmlElement* styleElement = view->getChildByName("Style");
            if (styleElement == nullptr)
                loadWarnings.add(TRANS("No style settings found, using default."));
            else
            {
                String style = styleElement->getAllSubText().trim();
                if (style == "default")
                    perform(ApplicationCommandTarget::InvocationInfo(lookAndFeelDefault));
                else if (style == "dark")
                    perform(ApplicationCommandTarget::InvocationInfo(lookAndFeelDark));
                else
                    loadWarnings.add(TRANS("Unknown style, using default."));
            }
        }

        XmlElement* audio = root->getChildByName("Audio");
        if (audio == nullptr)
            loadWarnings.add(TRANS("No audio settings found, using current."));
        else
        {
            if (audio->getNumChildElements() > 0)
            {
                String error = m_audioDeviceManager->initialise(64, 64, audio->getChildElement(0), false, String::empty, 0);
                if (error != "")
                {
                    loadWarnings.add(error);

                    error = m_audioDeviceManager->initialise(64, 64, nullptr, false, String::empty, 0);

                    if (error != "")
                        throw std::exception(error.toRawUTF8());
                }

                // AudioDeviceManager only notifies asynchronously but we want to continue loading the project now.
                // So we fake a change broadcast by the AudioDeviceManager so that OutputChannelNames is configured correctly.
                m_outputChannelNames->changeListenerCallback(m_audioDeviceManager);
            }
        }

        XmlElement* channelNames = root->getChildByName("ChannelNames");
        if (channelNames == nullptr)
            loadWarnings.add(TRANS("No channel names found, using device defaults."));
        else
        {
            m_outputChannelNames->restoreFromXml(*channelNames);
        }

        XmlElement* soloBusSettings = root->getChildByName("SoloBusSettings");
        if (soloBusSettings == nullptr)
            loadWarnings.add(TRANS("No solo bus settings found, using no solo bus."));
        else
        {
            m_soloBusSettings.restoreFromXml(*soloBusSettings);
        }

        XmlElement* soloMixer = root->getChildByName("SoloBusMixer");
        if (soloMixer == nullptr)
            loadWarnings.add(TRANS("No solo mixer settings found, using default volumes."));
        else
        {
            m_soloComponent->restoreFromXml(*soloMixer);
        }

        XmlElement* mixer = root->getChildByName("Mixer");

        if (mixer == nullptr)
            loadWarnings.add(TRANS("No mixer settings found, using current."));
        else
            m_mixerComponent->restoreFromXml(*mixer);

        XmlElement* players = root->getChildByName("Players");

        if (players == nullptr)
            loadWarnings.add(TRANS("No players found. None will be loaded."));
        else
        {
            for (int i = 0; i < players->getNumChildElements(); i++)
            {
                using namespace InterPlayerCommunication;

                XmlElement* player = players->getChildElement(i);
                if (player->getTagName() == "Player")
                {
                    const float gain = static_cast<float>(player->getDoubleAttribute("gain", 1.0));
                    const bool solo = player->getBoolAttribute("solo");
                    const bool mute = player->getBoolAttribute("mute");
                    PlayerType playerType = PlayerType::Playlist;
                    String type = player->getStringAttribute("type", "");
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
                        loadWarnings.add(String::formatted(TRANS("Unknown player type '%s'."), type));
                        continue;
                    }
                    Player* window = new Player(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, playerType, m_applicationProperties, m_audioThumbnailCache, m_timeSliceThread, gain, solo, mute);
                    window->addChangeListener(this);
                    window->setName(" "); // Satisfy JUCE, we set the actual name later.
                    m_multiDocumentPanel->addDocument(window, Colours::white, true);
                    window->restoreFromXml(*player, m_projectFile.getParentDirectory());
                }
                else if (player->getTagName() == "CDPlayer")
                {
                    const float gain = static_cast<float>(player->getDoubleAttribute("gain", 1.0));
                    const bool solo = player->getBoolAttribute("solo");
                    const bool mute = player->getBoolAttribute("mute");
                    CDPlayer* window = new CDPlayer(m_mixerComponent.get(), m_outputChannelNames, m_soloBusSettings, m_timeSliceThread, gain, solo, mute);
                    window->addChangeListener(this);
                    window->setName(" "); // Satisfy JUCE, we set the actual name later.
                    m_multiDocumentPanel->addDocument(window, Colours::white, true);
                    window->restoreFromXml(*player, m_projectFile.getParentDirectory());
                }
                else
                {
                    loadWarnings.add(String::formatted(TRANS("Unknown tag '%s' in players list."), player->getTagName()));
                }
            }

            // update soloMute for all players
            soloChanged(false);
        }

        m_projectModified = false;

        if (loadWarnings.size() > 0)
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Problems while opening the project"), loadWarnings.joinIntoString("\n"));
    }
    catch (const std::exception& e)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Failed opening the project"), String::fromUTF8(e.what()));
        m_projectModified = false;
        m_multiDocumentPanel->closeAllDocuments(false);
    }
}

void MainContentComponent::writeProjectFile()
{
    getParentComponent()->setName(m_projectFile.getFileNameWithoutExtension());

    ScopedPointer<XmlElement> root = new XmlElement("Project");

    XmlElement* view = new XmlElement("View");

    XmlElement* layoutMode = new XmlElement("LayoutMode");
    switch (m_multiDocumentPanel->getLayoutMode())
    {
    case MultiDocumentPanel::FloatingWindows:
        layoutMode->addTextElement("windows");
        break;
    case MultiDocumentPanel::MaximisedWindowsWithTabs:
        layoutMode->addTextElement("tabs");
        break;
    }
    view->addChildElement(layoutMode);

    XmlElement* style = new XmlElement("Style");
    if (&LookAndFeel::getDefaultLookAndFeel() == s_defaultLookAndFeel)
        style->addTextElement("default");
    else if (&LookAndFeel::getDefaultLookAndFeel() == s_darkLookAndFeel)
        style->addTextElement("dark");
    view->addChildElement(style);

    root->addChildElement(view);

    XmlElement* audio = new XmlElement("Audio");
    audio->addChildElement(m_audioDeviceManager->createStateXml());
    root->addChildElement(audio);

    XmlElement* channelNames = new XmlElement("ChannelNames");
    m_outputChannelNames->saveToXml(channelNames);
    root->addChildElement(channelNames);

    XmlElement* soloBusSettings = new XmlElement("SoloBusSettings");
    m_soloBusSettings.saveToXml(soloBusSettings);
    root->addChildElement(soloBusSettings);

    XmlElement* soloBusMixer = new XmlElement("SoloBusMixer");
    m_soloComponent->saveToXml(soloBusMixer);
    root->addChildElement(soloBusMixer);

    XmlElement* mixer = new XmlElement("Mixer");
    m_mixerComponent->saveToXml(mixer);
    root->addChildElement(mixer);

    XmlElement* players = new XmlElement("Players");

    for (int i = 0; i < m_multiDocumentPanel->getNumDocuments(); ++i)
    {
        if (Player* player = dynamic_cast<Player*>(m_multiDocumentPanel->getDocument(i)))
            players->addChildElement(player->saveToXml(m_projectFile.getParentDirectory(), m_multiDocumentPanel->getLayoutMode()));
        else if (CDPlayer* cdPlayer = dynamic_cast<CDPlayer*>(m_multiDocumentPanel->getDocument(i)))
            players->addChildElement(cdPlayer->saveToXml(m_projectFile.getParentDirectory(), m_multiDocumentPanel->getLayoutMode()));
        else
            assert(false && "Unknown player in multiDocumentPanel");
    }

    root->addChildElement(players);

    if (!root->writeToFile(m_projectFile, ""))
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Failed to save project file"), TRANS("Failed to save project file."));
    else
        m_projectModified = false;

    FileOutputStream stream(m_projectFile.getSiblingFile(m_projectFile.getFileNameWithoutExtension() + ".aupc"));
    if (stream.failedToOpen())
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Failed to save project"), TRANS("Failed to open audio thumbnail cache file."));
        return;
    }
    m_audioThumbnailCache.writeToStream(stream);
    if (stream.getStatus().failed())
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, TRANS("Failed to save project"), TRANS("Failed to save audio thumbnails."));
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
    ignoreUnused(channel, outputChannel, previousOutputChannel);

    resized();
}