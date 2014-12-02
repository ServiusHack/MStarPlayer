#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "ChannelVolumeAudioSource.h"
#include "Player.h"
#include "MixerFader.h"
#include "OutputChannelNames.h"
#include "PlayerMixerFader.h"

#include <memory>

/** Shows sliders for output channels to change the individual volumes of them.

	This component is shown at the bottom and allows the user to change the volume
	of each output channel individually. The actual work is done by
	ChannelVolumeAudioSource, which is managed by this class.

	The audio flow looks like this:
	Players -> MixerAudioSource -> ChannelVolumeAudioSource -> AudioSourcePlayer -> AudioDeviceManager
*/
class MixerComponent
	: public Component
	, public ChangeListener
	, public OutputChannelNamesListener
{
public:
	/** Creates a new MixerComponent.

        @param audioDeviceManager The AudioDeviceManager used throughout the application.
    */
    MixerComponent(AudioDeviceManager *audioDeviceManager, OutputChannelNames *outputChannelNames);
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

	void updatePlayerColor(Player* player, Colour color);
	
    /** Returns an XML object to encapsulate the state of the volumes.
        @see restoreFromXml
    */
    void saveToXml(XmlElement* element) const;

    /** Restores the volumes from an XML object created by createXML().
        @see createXml
    */
    void restoreFromXml(const XmlElement& element);

	virtual void outputChannelNamesReset() override;
	virtual void outputChannelNameChanged(int activeChannelIndex, String text) override;

private:
	// ui
	std::vector<MixerFader*> m_channelSliders;
	std::vector<PlayerMixerFader*> m_playerSliders;

	/** Adds a new slider to the component because not enough are being shown. */
	void addChannelSlider();

	void addPlayerSlider(Player* player);
	
	// audio output
	MixerAudioSource m_mixerAudioSource;
	ChannelVolumeAudioSource m_channelVolumeAudioSource;
    AudioSourcePlayer m_audioSourcePlayer;
	AudioDeviceManager *m_audioDeviceManager;
	OutputChannelNames *m_outputChannelNames;

	float m_separatorPosition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerComponent)
};
