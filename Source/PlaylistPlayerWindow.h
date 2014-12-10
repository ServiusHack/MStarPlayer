#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TracksComponent.h"
#include "ChannelMapping.h"
#include "Utils.h"
#include "PlayerEditDialog.h"
#include "PlaylistTable.h"
#include "InterPlayerCommunication.h"

namespace ipc = InterPlayerCommunication;
/*
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
	PlaylistPlayerWindow(TracksContainer* tracksContainer, OutputChannelNames *outputChannelNames, bool showPlaylist, InterPlayerCommunication::ShowEditDialogCallback showEditDialogCallback, InterPlayerCommunication::ConfigureChannelsCallback configureChannelsCallback, InterPlayerCommunication::ChangePlayerTypeCallback changePlayerTypeCallback, PlaylistModel& playlistModel, ApplicationProperties& applicationProperties);

	virtual void paint(Graphics&) override;
	virtual void resized() override;

	virtual void mouseDown(const MouseEvent & event) override;

	virtual void buttonClicked(Button * /*button*/) override;

	void setColor(Colour color);

	void setShowPlaylist(bool showPlaylist);

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

	OutputChannelNames* m_outputChannelNames;

	InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
	InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
	InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;

	StretchableLayoutManager m_layout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
