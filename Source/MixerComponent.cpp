#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"

MixerComponent::MixerComponent(AudioDeviceManager *audioDeviceManager, OutputChannelNames *outputChannelNames)
	: m_audioDeviceManager(audioDeviceManager)
	, m_outputChannelNames(outputChannelNames)
	, m_separatorPosition(0.0f)
{
    // Get notified when the AudioDeviceManager changes.
	m_audioDeviceManager->addChangeListener(this);

	m_outputChannelNames->addListener(this);

    // setup audio playback
	m_audioDeviceManager->addAudioCallback(&m_audioSourcePlayer);
	m_channelVolumeAudioSource = new ChannelVolumeAudioSource(&m_mixerAudioSource);
	m_audioSourcePlayer.setSource(m_channelVolumeAudioSource);

	// Add enough sliders for all output channels.
	AudioDeviceManager::AudioDeviceSetup deviceSetup;
	m_audioDeviceManager->getAudioDeviceSetup(deviceSetup);
	for (int i = 0; i < deviceSetup.outputChannels.countNumberOfSetBits(); i++) {
		addChannelSlider();
		m_channelSliders.getUnchecked(i)->setLabel(m_outputChannelNames->getInternalOutputChannelName(i));
	}

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

void MixerComponent::updatePlayerColor(Player* player, Colour color)
{
	int index = m_players.indexOf(player);
	m_playerSliders.getUnchecked(index)->setColor(color);
}

void MixerComponent::paint (Graphics& g)
{
	if (m_playerSliders.size() > 0)
		g.drawLine(m_separatorPosition, 0.0f, m_separatorPosition, static_cast<float>(getHeight()), 1.0f);
}

void MixerComponent::resized()
{
    int x = 0;
    int sliderHeight = getHeight();

	for (int i = 0; i < m_playerSliders.size(); i++) {
		Rectangle<int> bounds = m_playerSliders.getUnchecked(i)->getBounds();
		bounds.setX(x);
		bounds.setHeight(sliderHeight);
		m_playerSliders.getUnchecked(i)->setBounds(bounds);
		x += bounds.getWidth();
	}

	m_separatorPosition = x + 5.0f;
	x += 10;

	for (int i = 0; i < m_channelSliders.size(); i++) {
		Rectangle<int> bounds = m_channelSliders.getUnchecked(i)->getBounds();
		bounds.setX(x);
		bounds.setHeight(sliderHeight);
		m_channelSliders.getUnchecked(i)->setBounds(bounds);
		x += bounds.getWidth();
    }

}

void MixerComponent::addPlayerSlider(Player* player)
{
	MixerFader::ResizeCallback resizeCallback = [this]() {
		resized();
	};

	MixerFader* slider = new MixerFader(player, player->getSubMixerControlables(), false, resizeCallback);
	addAndMakeVisible (slider);
	m_playerSliders.add(slider);



	player->SetChannelCountChangedCallback([slider, player]() {
		std::vector<MixerControlable*> mixSettings = player->getSubMixerControlables();

		slider->setMixSettings(mixSettings);
	});

}

class ChannelMixerControlable : public MixerControlable
{
public:

	ChannelMixerControlable(int channel, ChannelVolumeAudioSource* audioSource)
		: channelNumber(channel)
		, m_channelVolumeAudioSource(audioSource)
	{
	}

	virtual void setGain(float gain) override
	{
		m_channelVolumeAudioSource->setChannelVolume(channelNumber, gain);
	}

	virtual float getGain() const override
	{
		return m_channelVolumeAudioSource->getChannelVolume(channelNumber);
	}

	virtual void setPan(float /*pan*/) override
	{
		jassertfalse
	}

	virtual float getPan() const override
	{
		jassertfalse
		return 0.0f;
	}

	virtual void setSoloMute(bool /*soloMute*/) override
	{
		jassertfalse;
	}

	virtual bool getSoloMute() const override
	{
		jassertfalse
		return false;
	}

	virtual void setSolo(bool solo) override
	{
		m_channelVolumeAudioSource->setChannelSolo(channelNumber, solo);
	}

	virtual bool getSolo() const override
	{
		return m_channelVolumeAudioSource->getChannelSolo(channelNumber);
	}

	virtual void setMute(bool mute) override
	{
		m_channelVolumeAudioSource->setChannelMute(channelNumber, mute);
	}

	virtual bool getMute() const override
	{
		return m_channelVolumeAudioSource->getChannelMute(channelNumber);
	}

	virtual float getVolume() const override
	{
		return m_channelVolumeAudioSource->getActualVolume(channelNumber);
	}

private:
	int channelNumber;
	ChannelVolumeAudioSource* m_channelVolumeAudioSource;
};

void MixerComponent::addChannelSlider()
{
	int channelNumber = m_channelSliders.size();

	ChannelMixerControlable* controlable = new ChannelMixerControlable(channelNumber, m_channelVolumeAudioSource);

	MixerFader::ResizeCallback resizeCallback = [this]() {
		resized();
	};

	MixerFader* slider = new MixerFader(controlable, std::vector<MixerControlable*>(), false, resizeCallback);
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

void MixerComponent::sliderValueChanged (Slider* /*sliderThatWasMoved*/)
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
		m_channelSliders.getUnchecked(i)->setValue(value.getFloatValue());
		m_channelSliders.getUnchecked(i)->setSolo(element.getChildElement(i)->getBoolAttribute("solo"));
		m_channelSliders.getUnchecked(i)->setMute(element.getChildElement(i)->getBoolAttribute("mute"));
	}
}
void MixerComponent::outputChannelNamesReset()
{
	for (int i = 0; i < m_channelSliders.size(); i++) {
		m_channelSliders.getUnchecked(i)->setLabel(m_outputChannelNames->getInternalOutputChannelName(i));
	}
}

void MixerComponent::outputChannelNameChanged(int activeChannelIndex, String text)
{
	m_channelSliders.getUnchecked(activeChannelIndex)->setLabel(text);
}