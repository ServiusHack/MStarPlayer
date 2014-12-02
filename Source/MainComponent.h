#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "JinglePlayerWindow.h"
#include "PlaylistPlayerWindow.h"
#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "DarkLookAndFeel.h"
#include "AudioConfiguration.h"

class MyMultiDocumentPanel : public MultiDocumentPanel {
public:
    bool tryToCloseDocument(Component *component);
};

//==============================================================================
/** Main component of the Audio Player application.

    It hosts the MDI area for all players and a mixer console with sliders
    for each channel at the bottom.
*/
class MainContentComponent
	: public Component
	, public MenuBarModel
	, public ApplicationCommandTarget
	, public ChangeListener
{
public:

	static LookAndFeel_V3* s_defaultLookAndFeel;
	static DarkLookAndFeel* s_darkLookAndFeel;

	static void initLookAndFeel();
	static void destroyLookAndFeel();

	MainContentComponent(ApplicationCommandManager* commandManager);
	~MainContentComponent();

	virtual void resized() override;

	// MenuBarModel overrides
	virtual StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int menuIndex, const String& menuName) override;
	virtual void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {};

    // ApplicationCommandTarget overrides
    virtual ApplicationCommandTarget* getNextCommandTarget() override;
    virtual void getAllCommands(Array <CommandID>& commands) override;
    virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    virtual bool perform(const InvocationInfo& info) override;
    
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

private:

	int getOutputChannels();

    ApplicationCommandManager* m_commandManager;
	ScopedPointer<MixerComponent> m_mixerComponent;
	ScopedPointer<MyMultiDocumentPanel> m_multiDocumentPanel;
	ScopedPointer<AudioDeviceManager> m_audioDeviceManager;
	ScopedPointer<OutputChannelNames> m_outputChannelNames;
	ScopedPointer<AudioConfigurationWindow> m_audioConfigurationWindow;

    // Commands for menu
    enum CommandIDs
    {
        projectNew= 0x2000,
        projectOpen = 0x2001,
        projectSave = 0x2002,
        projectSaveAs = 0x2003,

        addJinglePlayer = 0x2006,
        addPlaylistPlayer = 0x2007,

		layoutModeFloating = 0x2008,
		layoutModeTabs = 0x2009,

        configureAudio = 0x200A,

		lookAndFeelDefault = 0x200B,
		lookAndFeelDark = 0x200C
    };
    
    // Project file related methods and fields

    void newProject();
    void openProject();
    bool askSaveProject();
    bool saveProject();
    bool saveAsProject();

    void readProjectFile();
	void writeProjectFile();

	File m_projectFile;
	bool m_projectModified;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
