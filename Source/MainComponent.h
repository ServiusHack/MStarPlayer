#pragma once

#include <optional>

#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"

#include "AudioConfiguration.h"
#include "DarkLookAndFeel.h"
#include "DefaultLookAndFeel.h"
#include "EditSettingsDialog.h"
#include "JinglePlayerWindow.h"
#include "MTCSender.h"
#include "MidiConfiguration.h"
#include "MixerComponent.h"
#include "MyMultiDocumentPanel.h"
#include "OutputChannelNames.h"
#include "PinkLookAndFeel.h"
#include "PlaylistPlayerWindow.h"
#include "PluginLoader.h"
#include "SoloBusMixer.h"
#include "SoloBusSettings.h"
#include "TestToneGeneratorComponent.h"

/** Main component of the Audio Player application.

    It hosts the MDI area for all players and a mixer console with sliders
    for each channel at the bottom.
*/
class MainContentComponent
    : public juce::Component
    , public juce::MenuBarModel
    , public juce::ApplicationCommandTarget
    , public MixerControlableChangeListener
    , public SoloBusSettingsListener
{
public:
    using ContinueCallback = void (MainContentComponent::*)();
    MainContentComponent(
        juce::ApplicationProperties& applicationProperties, juce::ApplicationCommandManager* commandManager);
    ~MainContentComponent();

    void quit();

private:
    juce::ApplicationProperties& m_applicationProperties;
    juce::AudioThumbnailCache m_audioThumbnailCache;

    juce::TimeSliceThread m_timeSliceThread;

    SoloBusSettings m_soloBusSettings;
    juce::ApplicationCommandManager* m_commandManager;
    std::unique_ptr<MyMultiDocumentPanel> m_multiDocumentPanel;
    juce::AudioDeviceManager m_audioDeviceManager;
    std::unique_ptr<OutputChannelNames> m_outputChannelNames;
    std::unique_ptr<MixerComponent> m_mixerComponent;
    std::unique_ptr<TestToneGeneratorComponent> m_testToneGenerator;
    std::unique_ptr<SoloBusMixer> m_soloComponent;
    std::unique_ptr<AudioConfigurationWindow> m_audioConfigurationWindow;
    MTCSender m_mtcSender;
    std::unique_ptr<MidiConfigurationWindow> m_midiConfigurationWindow;
    std::unique_ptr<EditSettingsWindow> m_editSettingsWindow;
    std::unique_ptr<juce::DialogWindow> m_testToneGeneratorWindow;

    void reconfigSnapToGrid();

    // look and feels
public:
    static DefaultLookAndFeel* s_defaultLookAndFeel;
    static DarkLookAndFeel* s_darkLookAndFeel;
    static PinkLookAndFeel* s_pinkLookAndFeel;

    static void initLookAndFeel();
    static void destroyLookAndFeel();

    void switchToDefaultLookAndFeel();
    void switchToDarkLookAndFeel();
    void switchToPinkLookAndFeel();

    // Component overrides
public:
    virtual void resized() override;

    // MenuBarModel overrides
public:
    virtual juce::StringArray getMenuBarNames() override;
    virtual juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& menuName) override;
    virtual void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

    // ApplicationCommandTarget overrides
public:
    virtual juce::ApplicationCommandTarget* getNextCommandTarget() override;
    virtual void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    virtual void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    virtual bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

    // MixerControlableChangeListener overrides
public:
    virtual void soloChanged(bool solo) override;

    // SoloBusSettingsListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

    // Commands for menu
public:
    enum CommandIDs
    {
        projectNew = 0x2000,
        projectOpen = 0x2001,
        projectSave = 0x2002,
        projectSaveAs = 0x2003,
        projectRecentlyUsedFiles = 0x2004,

        addJinglePlayer = 0x3000,
        addMultitrackPlayer = 0x3001,
        addPlaylistPlayer = 0x3002,
        addCDPlayer = 0x3003,
        showTestToneGenerator = 0x3004,

        layoutModeFloating = 0x4000,
        layoutModeTabs = 0x4001,
        showMixer = 0x4002,

        configureAudio = 0x5000,
        configureMidi = 0x5002,
        editSettings = 0x5001,

        lookAndFeelDefault = 0x6000,
        lookAndFeelDark = 0x6001,
        lookAndFeelPink = 0x6002,

        basePlugin = 0x7000,
    };

    // Project file related methods and fields
public:
    void askSaveProject(ContinueCallback callback);
    void openProject(juce::File projectFile);

private:
    void requestNewProject();
    void uncheckedNewProject();

    void requestOpenProject();
    void uncheckedOpenProject();

    void saveProject(std::optional<ContinueCallback> callback = std::nullopt);
    void saveAsProject(std::optional<ContinueCallback> callback = std::nullopt);

    void readProjectFile();
    void writeProjectFile();

    juce::File m_projectFile;
    bool m_projectModified;

    juce::RecentlyOpenedFilesList m_recentlyOpenedFiles;

    std::optional<juce::FileChooser> m_currentFileChooser;

private:
    PluginLoader m_pluginLoader;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
