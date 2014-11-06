#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"

MixerComponent::MixerComponent(AudioDeviceManager *audioDeviceManager) :
  m_audioDeviceManager(audioDeviceManager)
{
    // Get notified when the AudioDeviceManager changes.
	m_audioDeviceManager->addChangeListener(this);
    
    // Add enough sliders for all output channels.
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
	m_audioDeviceManager->getAudioDeviceSetup(deviceSetup);
    for (BigInteger i = 0; i < deviceSetup.outputChannels.countNumberOfSetBits(); i++) {
        addChannelSlider();
    }

    // setup audio playback
	m_audioDeviceManager->addAudioCallback(&m_audioSourcePlayer);
	m_channelVolumeAudioSource = new ChannelVolumeAudioSource(&m_mixerAudioSource);
	m_audioSourcePlayer.setSource(m_channelVolumeAudioSource);

    setSize(100,112);
}

MixerComponent::~MixerComponent()
{
	while (m_channelSliders.size() > 0) {
        delete m_channelSliders.getLast();
        m_channelSliders.removeLast();
    }
    
	m_audioDeviceManager->removeAudioCallback(&m_audioSourcePlayer);
	m_audioSourcePlayer.setSource(nullptr);
	delete m_channelVolumeAudioSource;
}

void MixerComponent::registerPlayer(Player* player) {
	addPlayerSlider(player->getGain());
	m_players.add(player);
	resized();
}

void MixerComponent::unregisterPlayer(Player* player) {
	int playerIndex = m_players.indexOf(player);
	m_players.remove(playerIndex);
	delete m_playerSliders.getUnchecked(playerIndex);
	m_playerSliders.remove(playerIndex);
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

	for (int i = 0; i < m_playerSliders.size(); i++) {
		m_playerSliders.getUnchecked(i)->setBounds(x, 0, sliderWidth, sliderHeight);
		x += sliderWidth + 10;
	}

	x += sliderWidth;

	for (int i = 0; i < m_channelSliders.size(); i++) {
		m_channelSliders.getUnchecked(i)->setBounds(x, 0, sliderWidth, sliderHeight);
        x += sliderWidth + 10;
    }

}

void MixerComponent::addPlayerSlider(float gain)
{
    Slider* slider = new Slider();
    addAndMakeVisible (slider);
    slider->setRange (0, 2, 0.1);
    slider->setValue(gain);
    slider->setSliderStyle (Slider::LinearVertical);
    slider->setTextBoxStyle (Slider::NoTextBox, true,0,0);
    slider->addListener (this);

	m_playerSliders.add(slider);

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

	m_channelSliders.add(slider);
}

void MixerComponent::changeListenerCallback (ChangeBroadcaster * /*source*/)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
	m_audioDeviceManager->getAudioDeviceSetup(deviceSetup);

    // remove sliders until there are not too many
	while (deviceSetup.outputChannels.countNumberOfSetBits() < m_channelSliders.size()) {
		delete m_channelSliders.getLast();
		m_channelSliders.removeLast();
    }

    // add sliders until there are enough
	while (deviceSetup.outputChannels.countNumberOfSetBits() > m_channelSliders.size()) {
        addChannelSlider();
    }

    // correctly size the new sliders
    resized();
}

void MixerComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
	int channel = m_channelSliders.indexOf(sliderThatWasMoved);

	if (channel > -1)
		m_channelVolumeAudioSource->setChannelVolume(channel, static_cast<float>(sliderThatWasMoved->getValue()));
	else {
		int playerIndex = m_playerSliders.indexOf(sliderThatWasMoved);
		m_players.getUnchecked(playerIndex)->setGain(static_cast<float>(sliderThatWasMoved->getValue()));
	}
}

MixerAudioSource& MixerComponent::getMixerAudioSource()
{
	return m_mixerAudioSource;
}

void MixerComponent::saveToXml(XmlElement* element) const
{
	for (int i = 0; i < m_channelSliders.size(); i++) {
		XmlElement* sliderXml = new XmlElement("ChannelSlider");
		sliderXml->addTextElement(String(m_channelSliders.getUnchecked(i)->getValue()));
		element->addChildElement(sliderXml);
	}
}

void MixerComponent::restoreFromXml (const XmlElement& element)
{
	for (int i = 0; i < m_channelSliders.size(); i++) {
		String value = element.getChildElement(i)->getAllSubText().trim();
		m_channelSliders.getUnchecked(i)->setValue(value.getDoubleValue());
	}
}