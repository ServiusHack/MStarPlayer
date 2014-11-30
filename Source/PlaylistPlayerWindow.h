#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Player.h"
#include "MixerComponent.h"
#include "TracksComponent.h"
#include "ChannelMapping.h"
#include "Utils.h"
#include "RenameDialog.h"
#include "PlaylistTable.h"

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
	PlaylistPlayerWindow(MixerComponent* mixer, OutputChannelNames *outputChannelNames, float gain = 1.0f, bool solo = false, bool mute = false, bool showPlaylist = true);

    /** Destructor */
    ~PlaylistPlayerWindow();

	virtual void paint(Graphics&) override;
	virtual void resized() override;

	virtual void mouseDown(const MouseEvent & event) override;

	virtual void buttonClicked(Button * /*button*/) override;

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    virtual void setOutputChannels(int outputChannels) override;
	
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
	virtual XmlElement* saveToXml() const override;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
	void restoreFromXml(const XmlElement& element);
	
	virtual void setGain(float gain) override;

	virtual float getGain() const override;

	virtual void setPan(float pan) override;

	virtual float getPan() const override;

	virtual void setSoloMute(bool soloMute) override;

	virtual bool getSoloMute() const override;

	virtual void setSolo(bool solo) override;

	virtual bool getSolo() const override;

	virtual void setMute(bool mute) override;

	virtual bool getMute() const override;

	virtual float getVolume() const override;

	void updateGain();

	void configureChannels();

	virtual std::vector<MixerControlable*> getSubMixerControlables() override;

	virtual void SetChannelCountChangedCallback(ChannelCountChangedCallback callback) override;

private:
	std::vector<std::pair<char, int>> createMapping();

    // audio output
	MixerComponent* m_mixer;
	OutputChannelNames* m_outputChannelNames;
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
	ScopedPointer<PlaylistTable> m_tableListBox;

	float m_gain;
	bool m_soloMute;
	bool m_solo;
	bool m_mute;

	Colour m_color;
	OptionalScopedPointer<RenameDialogWindow> m_renameDialog;

	ChannelCountChangedCallback m_channelCountChanged;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistPlayerWindow)
};
