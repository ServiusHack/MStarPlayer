#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MixerComponent.h"
#include "Player.h"
#include "ChannelMapping.h"
#include "Utils.h"
#include "RenameDialog.h"

//==============================================================================
/** A jingle player shown within the application.

    Next to the PlaylistPlayerWindows this is a player that can be added by
    users to playback audio. It is specifically designed to play back short
    audio sequences, i.e. jingles.

*/
class JinglePlayerWindow
	: public Player
	, public Button::Listener
	, private Timer
{
public:
    /** Creates a new JinglePlayer.
    
        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
	JinglePlayerWindow(MixerComponent* mixer, int outputChannels, float gain = 1.0f, bool solo = false, bool mute = false);

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

	void setGain(float gain);

	float getGain();

	void setPan(float pan);

	float getPan();

	void setSoloMute(bool soloMute);

	bool getSoloMute();

	void setSolo(bool solo);

	bool getSolo();

	void setMute(bool mute);

	bool getMute();

	void updateGain();

private:

	const std::vector<int> createMapping();

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
	String m_userImagePath;
    
    // configuration menu actions
    void loadFile();
    void configureChannels();
	void rename();
    
    // audio output
	MixerComponent* m_mixer;
	int m_outputChannels;
	OptionalScopedPointer<ChannelMappingWindow> m_channelMappingWindow;

    // audio file loading
	File m_audioFile;
	AudioFormatManager m_formatManager;
	ScopedPointer<AudioFormatReaderSource> m_currentAudioFileSource;
    void loadFileIntoTransport();

    // audio file playback
	TimeSliceThread m_thread;
	AudioTransportSource m_transportSource;
	ScopedPointer<ChannelRemappingAudioSource> m_remappingAudioSource;

	OptionalScopedPointer<RenameDialogWindow> m_renameDialog;

	float m_gain;
	bool m_soloMute;
	bool m_solo;
	bool m_mute;

	bool m_showRemainingTime;

	Colour m_color;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JinglePlayerWindow)
};
