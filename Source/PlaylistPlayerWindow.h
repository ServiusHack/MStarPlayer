#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "ChannelMappingDialog.h"
#include "InterPlayerCommunication.h"
#include "PlayerEditDialog.h"
#include "PlaylistTable.h"
#include "TracksComponent.h"
#include "Utils.h"

class Player;

/**
        User interface with an optional playlist and a list of tracks.
*/
class PlaylistPlayerWindow
    : public juce::Component
    , public juce::Button::Listener
    , public juce::DragAndDropContainer
{
public:
    /** Creates a new PlaylistPlayer.

        @param mixer              Mix the audio into this mixer.

        @param outputChannels     Number of output channels when the player is created.
                                  When this changes later the setOutputChannels method is called.
    */
    PlaylistPlayerWindow(Player& player, TracksContainer* tracksContainer, bool showPlaylist,
        const InterPlayerCommunication::ShowEditDialogCallback& showEditDialogCallback,
        const InterPlayerCommunication::ConfigureChannelsCallback& configureChannelsCallback,
        const InterPlayerCommunication::ConfigureMidiCallback& configureMidiCallback,
        const InterPlayerCommunication::ChangePlayerTypeCallback& changePlayerTypeCallback,
        PlaylistModel& playlistModel, juce::ApplicationProperties& applicationProperties);

    void setColor(const juce::Colour& color);

    void setShowPlaylist(bool showPlaylist);

    int getSelectedRow() const;

    void nextPlaylistEntry(bool onlyIfEntrySaysSo);

    void previousPlaylistEntry();

    int getResizerBarPosition() const;

    void setResizerBarPosition(int position);

    // Component overrides
public:
    virtual void paint(juce::Graphics&) override;
    virtual void resized() override;
    virtual void mouseDown(const juce::MouseEvent& event) override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* /*button*/) override;

private:
    void fileLoaded(const juce::String& filename);

    Player& m_player;
    TracksContainer* m_tracksContainer;

    juce::ImageButton m_playButton;
    juce::ImageButton m_pauseButton;
    juce::ImageButton m_stopButton;
    juce::ImageButton m_skipBackwardButton;
    juce::ImageButton m_skipForwardButton;
    juce::ImageButton m_configureButton;
    juce::Label m_digitalDisplay;
    TracksComponent m_tracks;
    juce::Viewport m_tracksViewport;
    PlaylistTable m_tableListBox;
    juce::StretchableLayoutResizerBar m_resizeBar;

    juce::Colour m_color;

    InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
    InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
    InterPlayerCommunication::ConfigureMidiCallback m_configureMidiCallback;
    InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;

    juce::StretchableLayoutManager m_layout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
