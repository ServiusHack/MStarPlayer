#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "JinglePlayerWindow.h"
#include "PlaylistPlayerWindow.h"
#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "DarkLookAndFeel.h"
#include "AudioConfiguration.h"
#include "DefaultLookAndFeel.h"
#include "EditSettingsDialog.h"
#include "MyMultiDocumentPanel.h"

/** Main component of the Audio Player application.

    It hosts the MDI area for all players and a mixer console with sliders
    for each channel at the bottom.
*/
class MainContentComponent
	: public Component
	, public MenuBarModel
	, public ApplicationCommandTarget
	, public MixerControlableChangeListener
{
public:
	MainContentComponent(ApplicationCommandManager* commandManager);
	~MainContentComponent();

private:
	ApplicationProperties m_applicationProperties;
	AudioThumbnailCache m_audioThumbnailCache;

	TimeSliceThread m_timeSliceThread;

    ApplicationCommandManager* m_commandManager;
	ScopedPointer<MixerComponent> m_mixerComponent;
	ScopedPointer<MyMultiDocumentPanel> m_multiDocumentPanel;
	ScopedPointer<AudioDeviceManager> m_audioDeviceManager;
	ScopedPointer<OutputChannelNames> m_outputChannelNames;
	ScopedPointer<AudioConfigurationWindow> m_audioConfigurationWindow;
	ScopedPointer<EditSettingsWindow> m_editSettingsWindow;

// look and feels
public:
	static DefaultLookAndFeel* s_defaultLookAndFeel;
	static DarkLookAndFeel* s_darkLookAndFeel;

	static void initLookAndFeel();
	static void destroyLookAndFeel();

// Component overrides
public:
	virtual void resized() override;

// MenuBarModel overrides
public:
	virtual StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int menuIndex, const String& menuName) override;
	virtual void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {};

// ApplicationCommandTarget overrides
public:
    virtual ApplicationCommandTarget* getNextCommandTarget() override;
    virtual void getAllCommands(Array <CommandID>& commands) override;
    virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    virtual bool perform(const InvocationInfo& info) override;

// MixerControlableChangeListener overrides
public:
	virtual void soloChanged(bool solo) override;

// Commands for menu
public:
    enum CommandIDs
    {
        projectNew= 0x2000,
        projectOpen = 0x2001,
        projectSave = 0x2002,
        projectSaveAs = 0x2003,

        addJinglePlayer = 0x3000,
        addMultitrackPlayer = 0x3001,
        addPlaylistPlayer = 0x3002,
        addCDPlayer = 0x3003,

		layoutModeFloating = 0x4000,
		layoutModeTabs = 0x4001,

        configureAudio = 0x5000,
		editSettings = 0x5001,

		lookAndFeelDefault = 0x6000,
		lookAndFeelDark = 0x6001
    };
    
// Project file related methods and fields
private:
    void newProject();
    void openProject();
    bool askSaveProject();
    bool saveProject();
    bool saveAsProject();

    void readProjectFile();
	void writeProjectFile();

	File m_projectFile;
	bool m_projectModified;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
