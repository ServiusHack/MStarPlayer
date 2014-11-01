/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "AudioConfiguration.h"

bool MyMultiDocumentPanel::tryToCloseDocument(Component * /*component*/) {
    return true;
}

//==============================================================================
MainContentComponent::MainContentComponent(ApplicationCommandManager* commandManager_) : commandManager(commandManager_)
{
    projectModified = false;

    // audio setup
    audioDeviceManager = new AudioDeviceManager();
    audioDeviceManager->initialise(64,64, nullptr, false, String::empty, 0);
    
    // mixer control
    mixerComponent = new MixerComponent(audioDeviceManager);
    addAndMakeVisible(mixerComponent);

    // player MDI area
    multiDocumentPanel = new MyMultiDocumentPanel();
    addAndMakeVisible(multiDocumentPanel);
    multiDocumentPanel->setLayoutMode(MultiDocumentPanel::FloatingWindows);

    setSize (700, 600);
}

MainContentComponent::~MainContentComponent()
{
	delete multiDocumentPanel.release();
	delete mixerComponent.release();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeddff));
}

void MainContentComponent::resized()
{
    mixerComponent->setBounds(0,getHeight()-mixerComponent->getHeight(), getWidth(), mixerComponent->getHeight());
    multiDocumentPanel->setBounds(0,0, getWidth(), getHeight() - mixerComponent->getHeight() - 10);
}

StringArray MainContentComponent::getMenuBarNames()
{
    const char* const names[] = { "Project", "Player", "View", "Options", nullptr };

    return StringArray (names);
}

PopupMenu MainContentComponent::getMenuForIndex (int menuIndex, const String& /*menuName*/)
{
    PopupMenu menu;

    if (menuIndex == 0)
    {
        menu.addCommandItem (commandManager, projectNew);
        menu.addCommandItem (commandManager, projectOpen);
        menu.addCommandItem (commandManager, projectSave);
        menu.addCommandItem (commandManager, projectSaveAs);

        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);
    }
    else if (menuIndex == 1)
    {
        menu.addCommandItem (commandManager, addJinglePlayer);
        menu.addCommandItem (commandManager, addPlaylistPlayer);
	}
	else if (menuIndex == 2)
	{
		menu.addCommandItem(commandManager, layoutModeFloating);
		menu.addCommandItem(commandManager, layoutModeTabs);
	}
    else if (menuIndex == 3)
    {
        menu.addCommandItem (commandManager, configureAudio);
    }

    return menu;
}

void MainContentComponent::menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/)
{
}

//==============================================================================
// The following methods implement the ApplicationCommandTarget interface, allowing
// this window to publish a set of actions it can perform, and which can be mapped
// onto menus, keypresses, etc.

ApplicationCommandTarget* MainContentComponent::getNextCommandTarget()
{
    // this will return the next parent component that is an ApplicationCommandTarget (in this
    // case, there probably isn't one, but it's best to use this method in your own apps).
    return findFirstTargetParentComponent();
}

void MainContentComponent::getAllCommands (Array <CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = { projectNew,
                                projectOpen,
                                projectSave,
                                projectSaveAs,
                                addJinglePlayer,
                                addPlaylistPlayer,
								layoutModeFloating,
								layoutModeTabs,
                                configureAudio,
    };

    commands.addArray (ids, numElementsInArray (ids));
}

// This method is used when something needs to find out the details about one of the commands
// that this object can perform..
void MainContentComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    const String projectCategory ("Project");
	const String playerCategory("Player");
	const String viewCategory("View");
    const String optionsCategory ("Options");

    switch (commandID)
    {
    case projectNew:
        result.setInfo ("New", "Create a new project", projectCategory, 0);
        result.addDefaultKeypress ('N', ModifierKeys::commandModifier);
        
        break;

    case projectOpen:
        result.setInfo ("Open", "Open an existing project", projectCategory, 0);
        result.addDefaultKeypress ('O', ModifierKeys::commandModifier);
        break;

    case projectSave:
        result.setInfo ("Save", "Save the current project", projectCategory, 0);
        result.addDefaultKeypress ('S', ModifierKeys::commandModifier);
        break;

    case projectSaveAs:
        result.setInfo ("Save as ...", "Save the current project under ...", projectCategory, 0);
        break;

    case addJinglePlayer:
        result.setInfo ("Add Jingle Player", "Add a player for jingles", playerCategory, 0);
        result.addDefaultKeypress ('J', ModifierKeys::commandModifier);
        break;

    case addPlaylistPlayer:
        result.setInfo ("Add Playlist Player", "Add a player for playlists", playerCategory, 0);
        result.addDefaultKeypress ('P', ModifierKeys::commandModifier);
        break;

	case layoutModeFloating:
		result.setInfo("Windows", "Players are floating windows", viewCategory, 0);
		result.setTicked(multiDocumentPanel->getLayoutMode() == MultiDocumentPanel::FloatingWindows);
		break;

	case layoutModeTabs:
		result.setInfo("Tabs", "Players are tabs", viewCategory, 0);
		result.setTicked(multiDocumentPanel->getLayoutMode() == MultiDocumentPanel::MaximisedWindowsWithTabs);
		break;

    case configureAudio:
        result.setInfo ("Configure Audio", "Configure the audio device to use", optionsCategory, 0);
        break;

    default:
        break;
    };
}

    // this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
bool MainContentComponent::perform (const InvocationInfo& info)
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
            AudioDeviceManager::AudioDeviceSetup deviceSetup;
            audioDeviceManager->getAudioDeviceSetup(deviceSetup);
            int outputChannels = deviceSetup.outputChannels.countNumberOfSetBits();

			JinglePlayerWindow* window = new JinglePlayerWindow(mixerComponent.get(), outputChannels);
            window->setName("Jingle Player");
            multiDocumentPanel->addDocument(window, Colours::white, true);
            projectModified = true;
        }
        break;

    case addPlaylistPlayer:
        {
            AudioDeviceManager::AudioDeviceSetup deviceSetup;
            audioDeviceManager->getAudioDeviceSetup(deviceSetup);
            int outputChannels = deviceSetup.outputChannels.countNumberOfSetBits();

            PlaylistPlayerWindow* window = new PlaylistPlayerWindow(mixerComponent.get(), outputChannels);
            window->setName("Playlist Player");
            multiDocumentPanel->addDocument(window, Colours::white, true);
            projectModified = true;
        }
        break;

	case layoutModeFloating:
		{
			multiDocumentPanel->setLayoutMode(MultiDocumentPanel::FloatingWindows);
		}
		break;

	case layoutModeTabs:
		{
			multiDocumentPanel->setLayoutMode(MultiDocumentPanel::MaximisedWindowsWithTabs);
		}
		break;

    case configureAudio:
        {
            new AudioConfigurationWindow(*audioDeviceManager);
        }
        break;

    default:
        return false;
    };

    return true;
}


void MainContentComponent::newProject() {
    if (!askSaveProject())
        return;

    multiDocumentPanel->closeAllDocuments(true);
    projectModified = false;
}

void MainContentComponent::openProject() {
    if (!askSaveProject())
        return;

    FileChooser myChooser ("Please select the project file you want to load ...",
            File::getSpecialLocation (File::userHomeDirectory),
            "*.aupp");
    if (myChooser.browseForFileToOpen())
    {
        projectFile = File(myChooser.getResult());
        readProjectFile();
    }
}


bool MainContentComponent::askSaveProject()
{
    if (!projectModified)
        return true;

    switch (AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
            "Save project?",
            "Do you want to save the current project?",
            "Yes",
            "No",
            "Cancel",
            this)) {
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
    if (projectFile == File::nonexistent)
        return saveAsProject();

    writeProjectFile();
    return true;
}

bool MainContentComponent::saveAsProject()
{
    FileChooser myChooser ("Please select the project file you want to save ...",
            File::getSpecialLocation (File::userHomeDirectory),
            "*.aupp");
    if (myChooser.browseForFileToSave(true))
    {
        projectFile = File(myChooser.getResult());
        writeProjectFile();
        return true;
    }
    return false;
}

void MainContentComponent::readProjectFile()
{
    multiDocumentPanel->closeAllDocuments(false);

    Array<String> loadErrors;
    
    XmlDocument document(projectFile);

    XmlElement* root = document.getDocumentElement();

    if (root == nullptr)
    {
        String error = document.getLastParseError();
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Failed to open project file", error);
        return;
	}

	XmlElement* view = root->getChildByName("View");
	if (view == nullptr)
	{
		String error = document.getLastParseError();
		loadErrors.add("No view settings found, using default.");
	}
	else
	{
		XmlElement* layoutModeElement = view->getChildByName("LayoutMode");
		if (layoutModeElement == nullptr)
		{
			loadErrors.add("No layout mode settings found, using default.");
		}
		else
		{
			String layoutMode = layoutModeElement->getAllSubText().trim();
			if (layoutMode == "windows")
				multiDocumentPanel->setLayoutMode(MultiDocumentPanel::FloatingWindows);
			else if (layoutMode == "tabs")
				multiDocumentPanel->setLayoutMode(MultiDocumentPanel::MaximisedWindowsWithTabs);
			else
				loadErrors.add("Unknown view layout, using default.");
		}
	}

    XmlElement* audio = root->getChildByName("Audio");

    if (audio == nullptr)
    {
        String error = document.getLastParseError();
        loadErrors.add("No audio settings found, using current.");
    }
    else
    {
        if (audio->getNumChildElements() != 1)
        {
            loadErrors.add("Invalid number of audio settings found, using current.");
        }
        else
        {
            audioDeviceManager->initialise(64,64, audio->getChildElement(0), false, String::empty, 0);
        }
    }

    XmlElement* mixer = root->getChildByName("Mixer");

    if (mixer == nullptr)
    {
        String error = document.getLastParseError();
        loadErrors.add("No mixer settings found, using current.");
    }
    else
    {
        mixerComponent->restoreFromXml(*mixer);
    }

    XmlElement* players = root->getChildByName("Players");

    if (players == nullptr)
    {
        String error = document.getLastParseError();
        loadErrors.add("No players found. None will be loaded.");
    }
    else {

        for (int i = 0; i < players->getNumChildElements(); i++)
        {
            XmlElement* player = players->getChildElement(i);
            if (player->getTagName() != "Player") {
                loadErrors.add("Unknown tag '" + player->getTagName() + "' in players list.");
                continue;
            }

            String type = player->getStringAttribute("type", "");
            if (type == "") {
                loadErrors.add("Unknown player type '" + type + "'.");
                continue;
            }

            else if (type == "jingle")
            {
                AudioDeviceManager::AudioDeviceSetup deviceSetup;
                audioDeviceManager->getAudioDeviceSetup(deviceSetup);
                int outputChannels = deviceSetup.outputChannels.countNumberOfSetBits();

				JinglePlayerWindow* window = new JinglePlayerWindow(mixerComponent.get(), outputChannels);
				multiDocumentPanel->addDocument(window, Colours::white, true);
                window->restoreFromXml(*player);

            }

            else if (type == "playlist")
            {
                AudioDeviceManager::AudioDeviceSetup deviceSetup;
                audioDeviceManager->getAudioDeviceSetup(deviceSetup);
                int outputChannels = deviceSetup.outputChannels.countNumberOfSetBits();

				PlaylistPlayerWindow* window = new PlaylistPlayerWindow(mixerComponent.get(), outputChannels);
				multiDocumentPanel->addDocument(window, Colours::white, true);
                window->restoreFromXml(*player);

            }

        }
    }

    projectModified = false;
}

void MainContentComponent::writeProjectFile()
{
	XmlElement* root = new XmlElement("Project");

	XmlElement* view = new XmlElement("View");
	XmlElement* layoutMode = new XmlElement("LayoutMode");
	switch (multiDocumentPanel->getLayoutMode()) {
	case MultiDocumentPanel::FloatingWindows:
		layoutMode->addTextElement("windows");
		break;
	case MultiDocumentPanel::MaximisedWindowsWithTabs:
		layoutMode->addTextElement("tabs");
		break;
	}
	view->addChildElement(layoutMode);
	root->addChildElement(view);
    
    XmlElement* audio = new XmlElement("Audio");
    audio->addChildElement(audioDeviceManager->createStateXml());
    root->addChildElement(audio);

    XmlElement* mixer = new XmlElement("Mixer");
    mixerComponent->saveToXml(mixer);
    root->addChildElement(mixer);

    XmlElement* players = new XmlElement("Players");

	for (int i = 0; i < multiDocumentPanel->getNumDocuments(); ++i) {
        players->addChildElement(static_cast<Player*>(multiDocumentPanel->getDocument(i))->saveToXml());
	}

    root->addChildElement(players);

    if (!root->writeToFile(projectFile, ""))
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Failed to save project file", "Failed to save project file.");
    else
        projectModified = false;
}

void MainContentComponent::changeListenerCallback (ChangeBroadcaster * /*source*/)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    audioDeviceManager->getAudioDeviceSetup(deviceSetup);

    int outputChannels = deviceSetup.outputChannels.countNumberOfSetBits();
	
	for (int i = 0; i < multiDocumentPanel->getNumChildComponents(); ++i) {
        static_cast<Player*>(multiDocumentPanel->getDocument(i))->setOutputChannels(outputChannels);
    }
}