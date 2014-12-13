#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerComponent.h"
#include "ChannelMixerFader.h"

MixerComponent::MixerComponent(AudioDeviceManager *audioDeviceManager, OutputChannelNames *outputChannelNames)
	: m_mixerAudioSource()
	, m_channelVolumeAudioSource(&m_mixerAudioSource)
	, m_audioDeviceManager(audioDeviceManager)
	, m_outputChannelNames(outputChannelNames)
	, m_separatorPosition(0.0f)
{
	m_outputChannelNames->addListener(this);
	m_outputChannelNames->addChangeListener(this);

    // setup audio playback
	m_audioDeviceManager->addAudioCallback(&m_audioSourcePlayer);
	m_audioSourcePlayer.setSource(&m_channelVolumeAudioSource);

	// Add enough sliders for all output channels.
	AudioDeviceManager::AudioDeviceSetup deviceSetup;
	m_audioDeviceManager->getAudioDeviceSetup(deviceSetup);
	for (int i = 0; i < deviceSetup.outputChannels.countNumberOfSetBits(); i++) {
		addChannelSlider();
		m_channelSliders.at(i)->setLabel(m_outputChannelNames->getInternalOutputChannelName(i));
	}

    setSize(100,112);
}

MixerComponent::~MixerComponent()
{
	while (!m_channelSliders.empty()) {
        delete m_channelSliders.back();
        m_channelSliders.pop_back();
    }
    
	m_audioDeviceManager->removeAudioCallback(&m_audioSourcePlayer);
	m_audioSourcePlayer.setSource(nullptr);
}

void MixerComponent::registerPlayer(SubchannelPlayer* player) {
	addPlayerSlider(player);
	resized();
}

void MixerComponent::unregisterPlayer(SubchannelPlayer* player) {
	auto it = std::find_if(m_playerSliders.begin(), m_playerSliders.end(), [player](PlayerMixerFader* probe) { return probe->getPlayer() == player; });

	delete *it;
	m_playerSliders.erase(it);
	resized();
}

void MixerComponent::updatePlayerColor(SubchannelPlayer* player, Colour color)
{
	auto it = std::find_if(m_playerSliders.begin(), m_playerSliders.end(), [player](PlayerMixerFader* probe) { return probe->getPlayer() == player; });
	(*it)->setColor(color);
}

void MixerComponent::paint (Graphics& g)
{
	if (m_playerSliders.size() > 0)
		g.drawLine(m_separatorPosition, 0.0f, m_separatorPosition, static_cast<float>(getHeight()), 1.0f);
}

void MixerComponent::resized()
{
    int x = 0;
    const int sliderHeight = getHeight();

	auto updateBounds = [&x, sliderHeight](MixerFader* fader) {
		Rectangle<int> bounds = fader->getBounds();
		bounds.setX(x);
		bounds.setHeight(sliderHeight);
		fader->setBounds(bounds);
		x += bounds.getWidth();
	};

	std::for_each(m_playerSliders.begin(), m_playerSliders.end(), updateBounds);

	// set place of separation line between player and channel faders
	m_separatorPosition = x + 5.0f;
	x += 10;

	std::for_each(m_channelSliders.begin(), m_channelSliders.end(), updateBounds);
}


void MixerComponent::addPlayerSlider(SubchannelPlayer* player)
{
	PlayerMixerFader* slider = new PlayerMixerFader(player, player->getSubMixerControlables(), false, std::bind(&MixerComponent::resized, this));
	addAndMakeVisible(slider);
	m_playerSliders.push_back(slider);
}

void MixerComponent::addChannelSlider()
{
	int channelNumber = m_channelSliders.size();
	MixerFader* slider = new ChannelMixerFader(channelNumber, &m_channelVolumeAudioSource, std::bind(&MixerComponent::resized, this));
    addAndMakeVisible(slider);
	m_channelSliders.push_back(slider);
}

void MixerComponent::changeListenerCallback (ChangeBroadcaster * /*source*/)
{
    AudioDeviceManager::AudioDeviceSetup deviceSetup;
	m_audioDeviceManager->getAudioDeviceSetup(deviceSetup);

	size_t numberOfChannels = static_cast<size_t>(deviceSetup.outputChannels.countNumberOfSetBits());

    // remove sliders until there are not too many
	while (numberOfChannels < m_channelSliders.size()) {
		delete m_channelSliders.back();
		m_channelSliders.pop_back();
    }

    // add sliders until there are enough
	while (numberOfChannels > m_channelSliders.size()) {
        addChannelSlider();
    }

    // correctly size the new sliders
    resized();
}

MixerAudioSource& MixerComponent::getMixerAudioSource()
{
	return m_mixerAudioSource;
}

void MixerComponent::saveToXml(XmlElement* element) const
{
	for (int i = 0; i < m_channelVolumeAudioSource.channelCount(); i++) {
		XmlElement* sliderXml = new XmlElement("ChannelSlider");
		sliderXml->setAttribute("solo", m_channelVolumeAudioSource.getChannelSolo(i) ? "true" : "false");
		sliderXml->setAttribute("mute", m_channelVolumeAudioSource.getChannelMute(i) ? "true" : "false");
		sliderXml->addTextElement(String(m_channelVolumeAudioSource.getChannelVolume(i)));
		element->addChildElement(sliderXml);
	}
}

void MixerComponent::restoreFromXml (const XmlElement& element)
{
	for (size_t i = 0; i < m_channelSliders.size(); i++) {
		String value = element.getChildElement(i)->getAllSubText().trim();
		m_channelSliders[i]->setValue(value.getFloatValue());
		m_channelSliders[i]->setSolo(element.getChildElement(i)->getBoolAttribute("solo"));
		m_channelSliders[i]->setMute(element.getChildElement(i)->getBoolAttribute("mute"));
	}
}
void MixerComponent::outputChannelNamesReset()
{
	/*for (size_t i = 0; i < m_channelSliders.size(); i++) {
		m_channelSliders[i]->setLabel(m_outputChannelNames->getInternalOutputChannelName(i));
	}*/
}

void MixerComponent::outputChannelNameChanged(int activeChannelIndex, const String& text)
{
	m_channelSliders.at(activeChannelIndex)->setLabel(text);
}