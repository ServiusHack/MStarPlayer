#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TracksComponent.h"
#include "ChannelMappingDialog.h"
#include "Utils.h"
#include "PlayerEditDialog.h"
#include "PlaylistTable.h"
#include "InterPlayerCommunication.h"

/**
	User interface with an optional playlist and a list of tracks.
*/
class PlaylistPlayerWindow
	: public Component
    , public Button::Listener
{
public:
    /** Creates a new PlaylistPlayer.
    
        @param mixer              Mix the audio into this mixer.

        @param outputChannels     Number of output channels when the player is created.
                                  When this changes later the setOutputChannels method is called.
    */
	PlaylistPlayerWindow(TracksContainer* tracksContainer, bool showPlaylist, const InterPlayerCommunication::ShowEditDialogCallback& showEditDialogCallback, const InterPlayerCommunication::ConfigureChannelsCallback& configureChannelsCallback, const InterPlayerCommunication::ChangePlayerTypeCallback& changePlayerTypeCallback, PlaylistModel& playlistModel, ApplicationProperties& applicationProperties);

	void setColor(const Colour& color);

	void setShowPlaylist(bool showPlaylist);

	int getSelectedRow() const;

// Component overrides
public:
	virtual void paint(Graphics&) override;
	virtual void resized() override;
	virtual void mouseDown(const MouseEvent & event) override;

// Button::Listener
public:
	virtual void buttonClicked(Button* /*button*/) override;

private:
	ScopedPointer<ImageButton> m_playButton;
	ScopedPointer<ImageButton> m_pauseButton;
	ScopedPointer<ImageButton> m_stopButton;
	ScopedPointer<ImageButton> m_skipBackwardButton;
	ScopedPointer<ImageButton> m_skipForwardButton;
	ScopedPointer<ImageButton> m_configureButton;
	ScopedPointer<Label>       m_digitalDisplay;
	ScopedPointer<TracksComponent> m_tracks;
	ScopedPointer<Viewport>    m_tracksViewport;
	ScopedPointer<PlaylistTable> m_tableListBox;
	ScopedPointer<StretchableLayoutResizerBar> m_resizeBar;

	TracksContainer* m_tracksContainer;

	Colour m_color;

	InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
	InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
	InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;

	StretchableLayoutManager m_layout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
