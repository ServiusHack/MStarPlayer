/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_A9CBF06D__
#define __MAINCOMPONENT_H_A9CBF06D__

#include "../JuceLibraryCode/JuceHeader.h"
#include "JinglePlayerWindow.h"
#include "PlaylistPlayerWindow.h"
#include "MixerComponent.h"

class MyMultiDocumentPanel : public MultiDocumentPanel {
public:
    bool tryToCloseDocument(Component *component);

};

//==============================================================================
/** Main component of the Audio Player application.

    It hosts the MDI area for all players and a mixer console with sliders
    for each channel at the bottom.
*/
class MainContentComponent :	public Component,
                                public MenuBarModel,
                                public ApplicationCommandTarget,
                                public ChangeListener
{
public:
    //==============================================================================
    MainContentComponent(ApplicationCommandManager* commandManager);
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();

    // MenuBarModel
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex (int menuIndex, const String& /*menuName*/);
    void menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/);

    // ApplicationCommandTarget
    ApplicationCommandTarget* getNextCommandTarget();
    void getAllCommands (Array <CommandID>& commands);
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result);
    bool perform (const InvocationInfo& info);
    
    void changeListenerCallback (ChangeBroadcaster* /*source*/);

private:
    //==============================================================================
    ApplicationCommandManager* commandManager;
    ScopedPointer<MixerComponent> mixerComponent;
    ScopedPointer<MyMultiDocumentPanel> multiDocumentPanel;
    ScopedPointer<AudioDeviceManager> audioDeviceManager;

    //==============================================================================
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
    };
    
    //==============================================================================
    // Project file related fields and methods

    File projectFile;
    bool projectModified;

    void newProject();
    void openProject();
    bool askSaveProject();
    bool saveProject();
    bool saveAsProject();

    void readProjectFile();
    void writeProjectFile();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_A9CBF06D__
