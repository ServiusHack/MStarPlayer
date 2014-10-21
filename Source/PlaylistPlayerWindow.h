/*
  ==============================================================================

    PlaylistPlayerWindow.h
    Created: 29 Oct 2013 8:43:19pm
    Author:  User

  ==============================================================================
*/

#ifndef PLAYLISTPLAYERWINDOW_H_INCLUDED
#define PLAYLISTPLAYERWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Player.h"
#include "MixerComponent.h"
#include "TracksComponent.h"

//==============================================================================
/*
*/
class PlaylistPlayerWindow    : public Component,
                                public Button::Listener,
							    public Player,
                                private Timer
{
public:
    /** Creates a new PlaylistPlayer.
    
        @param mixer              Mix the audio into this mixer.

        @param outputChannels     Number of output channels when the player is created.
                                  When this changes later the setOutputChannels method is called.
    */
    PlaylistPlayerWindow(MixerComponent* mixer, int outputChannels);

    /** Destructor */
    ~PlaylistPlayerWindow();

    void paint (Graphics&);
    void resized();

    void mouseDown (const MouseEvent & event);

    void buttonClicked(Button * /*button*/);

    /** Regularly update the displayed time.

        Called by the Timer from which this class inherits.
    */
    void timerCallback();

    /** Set the number of output channels.

        If the user reconfigures his audio settings the number of output channels
        might change. This method is called to propagate this change to this player.
    */
    void setOutputChannels(int outputChannels);
	
    //==============================================================================
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    void saveToXml(XmlElement* element) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml (const XmlElement& element);
	
	void setGain(float gain);

	void configureChannels();
private:

    // audio output
    MixerComponent* mixer;
    int outputChannels;

    // audio file playback
    TimeSliceThread thread;
	
    ScopedPointer<ImageButton>  playButton;
    ScopedPointer<ImageButton>  pauseButton;
    ScopedPointer<ImageButton>  stopButton;
    ScopedPointer<ImageButton>  seekBackwardButton;
    ScopedPointer<ImageButton>  seekForwardButton;
    ScopedPointer<ImageButton>  skipBackwardButton;
    ScopedPointer<ImageButton>  skipForwardButton;
    ScopedPointer<ImageButton>  configureButton;
	ScopedPointer<Label>        digitalDisplay;
	ScopedPointer<TracksComponent> tracks;
	ScopedPointer<Viewport>     tracksViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistPlayerWindow)
};


#endif  // PLAYLISTPLAYERWINDOW_H_INCLUDED
