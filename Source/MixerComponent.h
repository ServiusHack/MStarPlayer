/*
  ==============================================================================

    MixerComponent.h
    Created: 14 Sep 2013 1:27:01pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#ifndef MIXERCOMPONENT_H_INCLUDED
#define MIXERCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "ChannelVolumeAudioSource.h"
#include "Player.h"

//==============================================================================
/** Shows sliders for output channels to change the individual volumes of them.

	This component is shown at the bottom and allows the user to change the volume
	of each output channel individually. The actual work is done by
	ChannelVolumeAudioSource, which is managed by this class.

	The audio flow looks like this:
	Players -> MixerAudioSource -> ChannelVolumeAudioSource -> AudioSourcePlayer -> AudioDeviceManager
*/
class MixerComponent    : public Component,
						  public SliderListener,
						  public ChangeListener
{
public:
	/** Creates a new MixerComponent.

        @param audioDeviceManager The AudioDeviceManager used throughout the application.
    */
    MixerComponent(AudioDeviceManager *audioDeviceManager);
    ~MixerComponent();

	/** Act accordingly to changes in the AudioDeviceManager. */
	void changeListenerCallback (ChangeBroadcaster * /*source*/);

	/** A slider value has changed. */
    void sliderValueChanged (Slider* sliderThatWasMoved);

    void paint (Graphics&);
    void resized();

	/** Returns the MixerAudioSource into which all Players mix their audio stream. */
	MixerAudioSource& getMixerAudioSource();

	void registerPlayer(Player* player);
	void unregisterPlayer(Player* player);
	
    //==============================================================================
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    void saveToXml(XmlElement* element) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml (const XmlElement& element);
private:

	// ui
	Array< Slider* > channelSliders;
	Array< Slider* > playerSliders;
	Array< Player* > players;

	/** Adds a new slider to the component because not enough are being shown. */
	void addChannelSlider();

	void addPlayerSlider();
	
	// audio output
	MixerAudioSource mixerAudioSource;
	ChannelVolumeAudioSource* channelVolumeAudioSource;
    AudioSourcePlayer audioSourcePlayer;
	AudioDeviceManager *audioDeviceManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerComponent)
};


#endif  // MIXERCOMPONENT_H_INCLUDED
