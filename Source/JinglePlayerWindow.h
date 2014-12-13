#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MixerComponent.h"
#include "ChannelMappingDialog.h"
#include "Utils.h"
#include "PlayerEditDialog.h"
#include "OutputChannelNames.h"
#include "TracksContainer.h"
#include "InterPlayerCommunication.h"

/** A jingle player shown within the application.

    Next to the PlaylistPlayerWindows this is a player that can be added by
    users to playback audio. It is specifically designed to play back short
    audio sequences, i.e. jingles.

*/
class JinglePlayerWindow
	: public Component
	, public Button::Listener
	, public ChangeListener
{
public:
    /** Creates a new JinglePlayer.
    
        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
	JinglePlayerWindow(TracksContainer* tracksContainer, InterPlayerCommunication::ShowEditDialogCallback showEditDialogCallback, InterPlayerCommunication::ConfigureChannelsCallback configureChannelsCallback, InterPlayerCommunication::ChangePlayerTypeCallback changePlayerTypeCallback);

	void setColor(const Colour& color);

	void setUserImage(const File& file);

// Component overrides
public:
    virtual void resized() override;
	virtual void paint(Graphics& g) override;
    /** Show the configuration menu. */
    virtual void mouseDown (const MouseEvent & event) override;

// Button::Listener
public:
    /** Play or stop the audio playback. */
    virtual void buttonClicked(Button * /*button*/) override;

// ChangeListener
public:
	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

private:
    // ui values
    double m_progress; // the progress of the playback
	double m_totalLength;

    // ui components
    ScopedPointer<ProgressBar>  m_progressBar;
	ScopedPointer<Label> m_totalDurationText;
	ScopedPointer<DrawableButton>  m_playButton;
	ScopedPointer<ImageButton>  m_configureButton;
	ScopedPointer<Drawable> m_playImage;
	ScopedPointer<Drawable> m_stopImage;
	OptionalScopedPointer<Drawable> m_userImage;
	ScopedPointer<Label> m_fileNameLabel;
    
    // configuration menu actions
    void loadFile();
    
    // audio output
	OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

	OptionalScopedPointer<PlayerEditDialogWindow> m_PlayerEditDialog;

	bool m_showRemainingTime;

	Colour m_color;
	bool m_blink;
	Colour m_paintColor;
	void updatePointColor();

	TracksContainer* m_tracksContainer;

	InterPlayerCommunication::ShowEditDialogCallback m_showEditDialogCallback;
	InterPlayerCommunication::ConfigureChannelsCallback m_configureChannelsCallback;
	InterPlayerCommunication::ChangePlayerTypeCallback m_changePlayerTypeCallback;

	AudioFormatManager m_formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JinglePlayerWindow)
};
