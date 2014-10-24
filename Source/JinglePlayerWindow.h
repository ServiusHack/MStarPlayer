/*
  ==============================================================================

    JinglePlayerWindow.h
    Created: 9 Sep 2013 4:37:45pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#ifndef JINGLEPLAYERWINDOW_H_INCLUDED
#define JINGLEPLAYERWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MixerComponent.h"
#include "Player.h"
#include "ChannelMapping.h"

//==============================================================================
/** A jingle player shown within the application.

    Next to the PlaylistPlayerWindows this is a player that can be added by
    users to playback audio. It is specifically designed to play back short
    audio sequences, i.e. jingles.

*/
class JinglePlayerWindow    : public Player,
                              public Button::Listener,
                              private Timer
{
public:
    /** Creates a new JinglePlayer.
    
        @param mixer              Mix the jingle audio into this mixer.

        @param outputChannels     Number of output channels when the JinglePlayer is created.
                                  When this changes later the setOutputChannels method is called.
    */
    JinglePlayerWindow(MixerComponent* mixer, int outputChannels);

    /** Destructor */
    ~JinglePlayerWindow();

    void paint (Graphics&);
    void resized();

    /** Show the configuration menu. */
    void mouseDown (const MouseEvent & event);

    /** Play or stop the audio playback. */
    void buttonClicked(Button * /*button*/);

    /** Regularly update the displayed time.

        Called by the Timer from which this class inherits.
    */
    void timerCallback();

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    void setOutputChannels(int outputChannels) override;
	
    //==============================================================================
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    XmlElement* saveToXml() const override;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml (const XmlElement& element);

	void setGain(float gain);

private:

    // ui values
    double progress; // the progress of the playback

    // ui components
    ScopedPointer<ProgressBar>  progressBar;
    ScopedPointer<DrawableText> totalDurationText;
    ScopedPointer<DrawableButton>  playButton;
    ScopedPointer<ImageButton>  configureButton;
	ScopedPointer<Drawable> playImage;
	ScopedPointer<Drawable> stopImage;
    
    // configuration menu actions
    void loadFile();
    void configureChannels();
    
    // audio output
    MixerComponent* mixer;
    int outputChannels;
	OptionalScopedPointer<ChannelMappingWindow> channelMappingWindow;

    // audio file loading
    File audioFile;
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;
    void loadFileIntoTransport();

    // audio file playback
    TimeSliceThread thread;
    AudioTransportSource transportSource;
    ScopedPointer<ChannelRemappingAudioSource> remappingAudioSource;

    // helper methods
    /** Display seconds in the common mm:ss:zzz format.
    */
    String formatSeconds(double seconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JinglePlayerWindow)
};


#endif  // JINGLEPLAYERWINDOW_H_INCLUDED
