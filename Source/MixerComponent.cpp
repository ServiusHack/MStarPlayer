/*
  ==============================================================================

    MixerComponent.cpp
    Created: 14 Sep 2013 1:27:01pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"

//==============================================================================
MixerComponent::MixerComponent(AudioDeviceManager *audioDeviceManager_) :
  audioDeviceManager(audioDeviceManager_)
{
    // Get notified when the AudioDeviceManager changes.
    audioDeviceManager->addChangeListener(this);
    
    // Add enough sliders for all output channels.
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    audioDeviceManager->getAudioDeviceSetup(deviceSetup);
    for (BigInteger i = 0; i < deviceSetup.outputChannels.countNumberOfSetBits(); i++) {
        addChannelSlider();
    }

    // setup audio playback
    audioDeviceManager->addAudioCallback (&audioSourcePlayer);
    channelVolumeAudioSource = new ChannelVolumeAudioSource(&mixerAudioSource, false);
    audioSourcePlayer.setSource (channelVolumeAudioSource);

    setSize(100,112);
}

MixerComponent::~MixerComponent()
{
    while (channelSliders.size() > 0) {
        delete channelSliders.getLast();
        channelSliders.removeLast();
    }
    
    audioDeviceManager->removeAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (nullptr);
    delete channelVolumeAudioSource;
}

void MixerComponent::registerPlayer(Player* player) {
	addPlayerSlider();
	players.add(player);
	resized();
}

void MixerComponent::unregisterPlayer(Player* player) {
	int playerIndex = players.indexOf(player);
	players.remove(playerIndex);
	delete playerSliders.getUnchecked(playerIndex);
	playerSliders.remove(playerIndex);
	resized();
}

void MixerComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xffeeddff));
}

void MixerComponent::resized()
{
    int x = 0;
    int sliderWidth = 16;
    int sliderHeight = getHeight();

	for (int i = 0; i < playerSliders.size(); i++) {
		playerSliders.getUnchecked(i)->setBounds(x, 0, sliderWidth, sliderHeight);
		x += sliderWidth + 10;
	}

	x += sliderWidth;

    for (int i = 0; i < channelSliders.size(); i++) {
        channelSliders.getUnchecked(i)->setBounds (x, 0, sliderWidth, sliderHeight);
        x += sliderWidth + 10;
    }

}

void MixerComponent::addPlayerSlider()
{
    Slider* slider = new Slider();
    addAndMakeVisible (slider);
    slider->setRange (0, 2, 0.1);
    slider->setValue(1.0);
    slider->setSliderStyle (Slider::LinearVertical);
    slider->setTextBoxStyle (Slider::NoTextBox, true,0,0);
    slider->addListener (this);

	playerSliders.add(slider);

}

void MixerComponent::addChannelSlider()
{
    Slider* slider = new Slider();
    addAndMakeVisible (slider);
    slider->setRange (0, 2, 0.1);
    slider->setValue(1.0);
    slider->setSliderStyle (Slider::LinearVertical);
    slider->setTextBoxStyle (Slider::NoTextBox, true,0,0);
    slider->addListener (this);

    channelSliders.add(slider);
}

void MixerComponent::changeListenerCallback (ChangeBroadcaster * /*source*/)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
    audioDeviceManager->getAudioDeviceSetup(deviceSetup);

    // remove sliders until there are not too many
    while (deviceSetup.outputChannels.countNumberOfSetBits() < channelSliders.size()) {
        delete channelSliders.getLast();
        channelSliders.removeLast();
    }

    // add sliders until there are enough
    while (deviceSetup.outputChannels.countNumberOfSetBits() > channelSliders.size()) {
        addChannelSlider();
    }

    // correctly size the new sliders
    resized();
}

void MixerComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    int channel = channelSliders.indexOf(sliderThatWasMoved);

	if (channel > -1)
		channelVolumeAudioSource->setChannelVolume(channel, sliderThatWasMoved->getValue());
	else {
		int playerIndex = playerSliders.indexOf(sliderThatWasMoved);
		players.getUnchecked(playerIndex)->setGain(sliderThatWasMoved->getValue());
	}
}

MixerAudioSource& MixerComponent::getMixerAudioSource()
{
    return mixerAudioSource;
}

void MixerComponent::saveToXml(XmlElement* element) const
{
    for (int i = 0; i < channelSliders.size(); i++) {
		XmlElement* sliderXml = new XmlElement("ChannelSlider");
		sliderXml->addTextElement(String(channelSliders.getUnchecked(i)->getValue()));
		element->addChildElement(sliderXml);
	}
}

void MixerComponent::restoreFromXml (const XmlElement& element)
{
    for (int i = 0; i < channelSliders.size(); i++) {
		String value = element.getChildElement(i)->getAllSubText().trim();
		channelSliders.getUnchecked(i)->setValue(value.getDoubleValue());
	}
}