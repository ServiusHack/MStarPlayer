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
	addPlayerSlider(player);
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
    int sliderWidth = 70;
    int sliderHeight = getHeight();

	for (int i = 0; i < m_playerSliders.size(); i++) {
		m_playerSliders.getUnchecked(i)->setBounds(x, 0, sliderWidth, sliderHeight);
		x += sliderWidth + 10;
	}

	x += 10;

	for (int i = 0; i < m_channelSliders.size(); i++) {
		m_channelSliders.getUnchecked(i)->setBounds(x, 0, sliderWidth, sliderHeight);
        x += sliderWidth + 10;
    }

}

void MixerComponent::addPlayerSlider(Player* player)
{
	MixerFader::VolumeChangedCallback volumeCallback = [player](float gain) {
		player->setGain(gain);
	};
	MixerFader::PanChangedCallback panCallback = [player](float pan) {
		player->setPan(pan);
	};
	MixerFader::SoloChangedCallback soloCallback = [this, player](bool solo) {
		player->setSolo(solo);

		bool anySolo = std::any_of(m_players.begin(), m_players.end(), [](Player* p) { return p->getSolo(); });
		
		for (int i = 0; i < m_players.size(); ++i) {
			m_players.getUnchecked(i)->setSoloMute(anySolo);
		}
	};
	MixerFader::MuteChangedCallback muteCallback = [player](bool mute) {
		player->setMute(mute);
	};
	MixerFader* slider = new MixerFader(false, player->getGain(), player->getSolo(), player->getMute(), volumeCallback, panCallback, soloCallback, muteCallback);
    addAndMakeVisible (slider);

	m_playerSliders.add(slider);

}

void MixerComponent::addChannelSlider()
{
	int channelNumber = m_channelSliders.size();

	MixerFader::VolumeChangedCallback volumeCallback = [this, channelNumber](float gain) {
		m_channelVolumeAudioSource->setChannelVolume(channelNumber, gain);
	};
	MixerFader::PanChangedCallback panCallback = [this, channelNumber](float pan) {
	};
	MixerFader::SoloChangedCallback soloCallback = [this, channelNumber](bool solo) {
		m_channelVolumeAudioSource->setChannelSolo(channelNumber, solo);
	};
	MixerFader::MuteChangedCallback muteCallback = [this, channelNumber](bool mute) {
		m_channelVolumeAudioSource->setChannelMute(channelNumber, mute);
	};

	MixerFader* slider = new MixerFader(false, 1.0f, false, false, volumeCallback, panCallback, soloCallback, muteCallback);
    addAndMakeVisible (slider);

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
	/*int channel = m_channelSliders.indexOf(sliderThatWasMoved);

	if (channel > -1)
		m_channelVolumeAudioSource->setChannelVolume(channel, static_cast<float>(sliderThatWasMoved->getValue()));
	else {
		int playerIndex = m_playerSliders.indexOf(sliderThatWasMoved);
		m_players.getUnchecked(playerIndex)->setGain(static_cast<float>(sliderThatWasMoved->getValue()));
	}*/
}

MixerAudioSource& MixerComponent::getMixerAudioSource()
{
	return m_mixerAudioSource;
}

void MixerComponent::saveToXml(XmlElement* element) const
{
	for (int i = 0; i < m_channelVolumeAudioSource->channelCount(); i++) {
		XmlElement* sliderXml = new XmlElement("ChannelSlider");
		sliderXml->setAttribute("solo", m_channelVolumeAudioSource->getChannelSolo(i) ? "true" : "false");
		sliderXml->setAttribute("mute", m_channelVolumeAudioSource->getChannelMute(i) ? "true" : "false");
		sliderXml->addTextElement(String(m_channelVolumeAudioSource->getChannelVolume(i)));
		element->addChildElement(sliderXml);
	}
}

void MixerComponent::restoreFromXml (const XmlElement& element)
{
	for (int i = 0; i < m_channelSliders.size(); i++) {
		String value = element.getChildElement(i)->getAllSubText().trim();
		m_channelSliders.getUnchecked(i)->setValue(value.getDoubleValue());
		m_channelSliders.getUnchecked(i)->setSolo(element.getChildElement(i)->getBoolAttribute("solo"));
		m_channelSliders.getUnchecked(i)->setMute(element.getChildElement(i)->getBoolAttribute("mute"));
	}
}