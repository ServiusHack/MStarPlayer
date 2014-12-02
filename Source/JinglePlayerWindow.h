#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MixerComponent.h"
#include "Player.h"
#include "ChannelMapping.h"
#include "Utils.h"
#include "PlayerEditDialog.h"
#include "OutputChannelNames.h"
#include "ChannelRemappingAudioSourceWithVolume.h"

//==============================================================================
/** A jingle player shown within the application.

    Next to the PlaylistPlayerWindows this is a player that can be added by
    users to playback audio. It is specifically designed to play back short
    audio sequences, i.e. jingles.

*/
class JinglePlayerWindow
	: public Player
	, public Button::Listener
	, public ChangeListener
	, private Timer
{
public:
    /** Creates a new JinglePlayer.
    
        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
	JinglePlayerWindow(MixerComponent* mixer, OutputChannelNames *outputChannelNames, float gain = 1.0f, bool solo = false, bool mute = false);

    /** Destructor */
    ~JinglePlayerWindow();

    virtual void resized() override;
	virtual void paint(Graphics& g) override;

    /** Show the configuration menu. */
    virtual void mouseDown (const MouseEvent & event) override;

    /** Play or stop the audio playback. */
    virtual void buttonClicked(Button * /*button*/) override;

    /** Regularly update the displayed time.

        Called by the Timer from which this class inherits.
    */
    virtual void timerCallback() override;

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    virtual void setOutputChannels(int outputChannels) override;
	
    //==============================================================================
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    virtual XmlElement* saveToXml() const override;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml (const XmlElement& element);

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

	virtual std::vector<MixerControlable*> getSubMixerControlables() const override;

	virtual void SetChannelCountChangedCallback(ChannelCountChangedCallback callback) override;

	void changeListenerCallback(ChangeBroadcaster *source);

private:

	const std::vector<std::pair<char, int>> createMapping();

    // ui values
    double m_progress; // the progress of the playback

    // ui components
    ScopedPointer<ProgressBar>  m_progressBar;
	ScopedPointer<Label> m_totalDurationText;
	ScopedPointer<DrawableButton>  m_playButton;
	ScopedPointer<ImageButton>  m_configureButton;
	ScopedPointer<Drawable> m_playImage;
	ScopedPointer<Drawable> m_stopImage;
	OptionalScopedPointer<Drawable> m_userImage;
	ScopedPointer<Label> m_fileNameLabel;
	String m_userImagePath;

	AudioThumbnailCache m_audioThumbnailCache;
	ScopedPointer<AudioThumbnail> m_audioThumbnail;
    
    // configuration menu actions
    void loadFile();
    void configureChannels();
	void rename();
    
    // audio output
	MixerComponent* m_mixer;
	OutputChannelNames* m_outputChannelNames;
	OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    // audio file loading
	File m_audioFile;
	AudioFormatManager m_formatManager;
	ScopedPointer<AudioFormatReaderSource> m_currentAudioFileSource;
    void loadFileIntoTransport();

    // audio file playback
	TimeSliceThread m_thread;
	AudioTransportSource m_transportSource;
	ScopedPointer<ChannelRemappingAudioSourceWithVolume> m_remappingAudioSource;

	OptionalScopedPointer<PlayerEditDialogWindow> m_PlayerEditDialog;

	float m_gain;
	bool m_soloMute;
	bool m_solo;
	bool m_mute;
	int m_outputChannels;

	bool m_showRemainingTime;

	Colour m_color;
	bool m_blink;
	Colour m_paintColor;
	void updatePointColor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JinglePlayerWindow)
};
