#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ChannelMappingDialog.h"
#include "InterPlayerCommunication.h"
#include "PlayerEditDialog.h"
#include "PlaylistTable.h"
#include "TracksComponent.h"
#include "Utils.h"

/**
	User interface with an optional playlist and a list of tracks.
*/
class PlaylistPlayerWindow
    : public Component
    , public Button::Listener
    , public DragAndDropContainer
{
public:
    /** Creates a new PlaylistPlayer.
    
        @param mixer              Mix the audio into this mixer.

        @param outputChannels     Number of output channels when the player is created.
                                  When this changes later the setOutputChannels method is called.
    */
    PlaylistPlayerWindow(TracksContainer* tracksContainer, bool showPlaylist, const InterPlayerCommunication::ShowEditDialogCallback& showEditDialogCallback, const InterPlayerCommunication::ConfigureChannelsCallback& configureChannelsCallback, const InterPlayerCommunication::ConfigureMidiCallback& configureMidiCallback, const InterPlayerCommunication::ChangePlayerTypeCallback& changePlayerTypeCallback, PlaylistModel& playlistModel, ApplicationProperties& applicationProperties);

    void setColor(const Colour& color);

    void setShowPlaylist(bool showPlaylist);

    int getSelectedRow() const;

    int getResizerBarPosition() const;

    void setResizerBarPosition(int position);

// Component overrides
public:
    virtual void paint(Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const MouseEvent& event) override;

// Button::Listener
public:
    virtual void buttonClicked(Button* /*button*/) override;

private:
    void fileLoaded(const String& filename);

    TracksContainer* m_tracksContainer;

    ImageButton m_playButton;
    ImageButton m_pauseButton;
    ImageButton m_stopButton;
    ImageButton m_skipBackwardButton;
    ImageButton m_skipForwardButton;
    ImageButton m_configureButton;
    Label m_digitalDisplay;
    TracksComponent m_tracks;
    Viewport m_tracksViewport;
    PlaylistTable m_tableListBox;
    StretchableLayoutResizerBar m_resizeBar;

    Colour m_color;

    InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
    InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
    InterPlayerCommunication::ConfigureMidiCallback m_configureMidiCallback;
    InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;

    StretchableLayoutManager m_layout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
