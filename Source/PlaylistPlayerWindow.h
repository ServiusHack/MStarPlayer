#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Player.h"
#include "MixerComponent.h"
#include "TracksComponent.h"
#include "ChannelMapping.h"
#include "Utils.h"

/*
*/
class PlaylistPlayerWindow
	: public Player
    , public Button::Listener
{
public:
    /** Creates a new PlaylistPlayer.
    
        @param mixer              Mix the audio into this mixer.

        @param outputChannels     Number of output channels when the player is created.
                                  When this changes later the setOutputChannels method is called.
    */
	PlaylistPlayerWindow(MixerComponent* mixer, int outputChannels, float gain = 1.0f);

    /** Destructor */
    ~PlaylistPlayerWindow();

    void paint(Graphics&);
    void resized();

    void mouseDown(const MouseEvent & event);

    void buttonClicked(Button * /*button*/);

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    void setOutputChannels(int outputChannels) override;
	
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    XmlElement* saveToXml() const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const XmlElement& element);
	
	void setGain(float gain);

	float getGain();

	void configureChannels();

private:
    // audio output
	MixerComponent* m_mixer;
	int m_outputChannels;
	OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    // audio file playback
	TimeSliceThread m_thread;
	
	ScopedPointer<ImageButton> m_playButton;
	ScopedPointer<ImageButton> m_pauseButton;
	ScopedPointer<ImageButton> m_stopButton;
	ScopedPointer<ImageButton> m_seekBackwardButton;
	ScopedPointer<ImageButton> m_seekForwardButton;
	ScopedPointer<ImageButton> m_skipBackwardButton;
	ScopedPointer<ImageButton> m_skipForwardButton;
	ScopedPointer<ImageButton> m_configureButton;
	ScopedPointer<Label>       m_digitalDisplay;
	ScopedPointer<TracksComponent> m_tracks;
	ScopedPointer<Viewport>    m_tracksViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
