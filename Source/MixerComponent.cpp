#include "MixerComponent.h"

#include "ChannelMixerFader.h"

MixerComponent::MixerComponent(juce::AudioDeviceManager* audioDeviceManager, OutputChannelNames* outputChannelNames,
    SoloBusSettings& soloBusSettings)
    : m_mixerAudioSource()
    , m_channelVolumeAudioSource(&m_mixerAudioSource)
    , m_audioDeviceManager(audioDeviceManager)
    , m_outputChannelNames(outputChannelNames)
    , m_sliderScrollBar(false)
    , m_soloBusSettings(soloBusSettings)
{
    addAndMakeVisible(m_slidersContainer);
    addAndMakeVisible(m_sliderScrollBar);

    m_sliderScrollBar.addListener(this);

    m_outputChannelNames->addListener(this);
    m_outputChannelNames->addChangeListener(this);

    // setup audio playback
    m_audioDeviceManager->addAudioCallback(&m_audioSourcePlayer);
    m_audioSourcePlayer.setSource(&m_channelVolumeAudioSource);

    setSize(100, 112);
    startTimer(1000 / LevelMeter::refreshRate);

    m_soloBusSettings.addListener(this);
}

MixerComponent::~MixerComponent()
{
    while (!m_channelSliders.empty())
    {
        m_channelSliders.pop_back();
    }

    m_outputChannelNames->removeListener(this);
    m_outputChannelNames->removeChangeListener(this);
    m_audioDeviceManager->removeAudioCallback(&m_audioSourcePlayer);
    m_audioSourcePlayer.setSource(nullptr);
    m_soloBusSettings.removeListener(this);
}

void MixerComponent::registerPlayer(SubchannelPlayer* player)
{
    addPlayerSlider(player);
    resized();
}

void MixerComponent::unregisterPlayer(SubchannelPlayer* player)
{
    auto it = std::find_if(m_playerSliders.begin(),
        m_playerSliders.end(),
        [player](const std::unique_ptr<PlayerMixerFader>& probe) { return probe->getPlayer() == player; });

    m_playerSliders.erase(it);
    resized();
}

void MixerComponent::updatePlayerColor(SubchannelPlayer* player, juce::Colour color)
{
    auto it = std::find_if(m_playerSliders.begin(),
        m_playerSliders.end(),
        [player](const std::unique_ptr<PlayerMixerFader>& probe) { return probe->getPlayer() == player; });
    (*it)->setColor(color);
}

void MixerComponent::resized()
{
    const int scrollBarHeight = 18;
    m_sliderScrollBar.setBounds(0, getHeight() - scrollBarHeight, getWidth(), scrollBarHeight);

    // Determine width of all sliders.
    int width = 0;
    auto sumWidths = [&width](const auto& fader) { width += fader->isVisible() ? fader->getBounds().getWidth() : 0; };

    std::for_each(m_playerSliders.begin(), m_playerSliders.end(), sumWidths);
    width += 10;
    std::for_each(m_channelSliders.begin(), m_channelSliders.end(), sumWidths);

    m_sliderScrollBar.setRangeLimits(0, std::max(0, width));
    const int sliderHeight = getHeight() - (width <= getWidth() ? 0 : scrollBarHeight);

    int x = 0;

    auto updateBounds = [&x, sliderHeight](const auto& fader)
    {
        if (!fader->isVisible())
            return;
        juce::Rectangle<int> bounds = fader->getBounds();
        bounds.setX(x);
        bounds.setHeight(sliderHeight);
        fader->setBounds(bounds);
        x += bounds.getWidth();
    };

    std::for_each(m_playerSliders.begin(), m_playerSliders.end(), updateBounds);

    x += 10;

    std::for_each(m_channelSliders.begin(), m_channelSliders.end(), updateBounds);

    m_sliderScrollBar.setCurrentRange(m_slidersContainer.getX(), getWidth());

    {
        juce::Rectangle<int> containerBounds = m_slidersContainer.getBounds();
        containerBounds.setWidth(x);
        containerBounds.setHeight(sliderHeight);
        m_slidersContainer.setBounds(containerBounds);
    }
}

void MixerComponent::addPlayerSlider(SubchannelPlayer* player)
{
    std::unique_ptr<PlayerMixerFader> slider = std::make_unique<PlayerMixerFader>(
        player, player->getSubMixerControlables(), false, std::bind(&MixerComponent::resized, this));
    m_slidersContainer.addAndMakeVisible(slider.get());
    m_playerSliders.push_back(std::move(slider));
}

void MixerComponent::addChannelSlider()
{
    int channelNumber = m_channelSliders.size();
    std::unique_ptr<MixerFader> slider = std::make_unique<ChannelMixerFader>(
        channelNumber, &m_channelVolumeAudioSource, std::bind(&MixerComponent::resized, this));
    m_slidersContainer.addAndMakeVisible(slider.get());
    m_channelSliders.push_back(std::move(slider));
}

void MixerComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    size_t numberOfChannels = m_outputChannelNames->getNumberOfChannels();

    m_channelVolumeAudioSource.setChannelCount(numberOfChannels);

    // remove sliders until there are not too many
    while (numberOfChannels < m_channelSliders.size())
    {
        m_channelSliders.pop_back();
    }

    // add sliders until there are enough
    while (numberOfChannels > m_channelSliders.size())
    {
        addChannelSlider();
    }

    // get names for all sliders
    for (size_t i = 0; i < m_channelSliders.size(); i++)
    {
        m_channelSliders[i]->setLabel(m_outputChannelNames->getInternalOutputChannelName(i));
    }

    // correctly size the new sliders
    resized();
}

juce::MixerAudioSource& MixerComponent::getMixerAudioSource()
{
    return m_mixerAudioSource;
}

ChannelVolumeAudioSource& MixerComponent::getChannelVolumeAudioSource()
{
    return m_channelVolumeAudioSource;
}

void MixerComponent::saveToXml(juce::XmlElement* element) const
{
    for (int i = 0; i < m_channelVolumeAudioSource.channelCount(); i++)
    {
        juce::XmlElement* sliderXml = new juce::XmlElement("ChannelSlider");
        sliderXml->setAttribute("solo", m_channelVolumeAudioSource.getChannelSolo(i) ? "true" : "false");
        sliderXml->setAttribute("mute", m_channelVolumeAudioSource.getChannelMute(i) ? "true" : "false");
        sliderXml->addTextElement(juce::String(m_channelVolumeAudioSource.getChannelVolume(i)));
        element->addChildElement(sliderXml);
    }
}

void MixerComponent::restoreFromXml(const juce::XmlElement& element)
{
    for (size_t i = 0; i < m_channelSliders.size(); i++)
    {
        juce::String value = element.getChildElement(i)->getAllSubText().trim();
        m_channelSliders[i]->setValue(value.getFloatValue());
        m_channelSliders[i]->setSolo(element.getChildElement(i)->getBoolAttribute("solo"));
        m_channelSliders[i]->setMute(element.getChildElement(i)->getBoolAttribute("mute"));
    }
}
void MixerComponent::outputChannelNamesReset() {}

void MixerComponent::outputChannelNameChanged(int activeChannelIndex, const juce::String& text)
{
    m_channelSliders.at(activeChannelIndex)->setLabel(text);
}

void MixerComponent::outputChannelPairingModeChanged(int activeChannelIndex, PairingMode mode)
{
    switch (mode)
    {
    case PairingMode::Mono:
        m_channelSliders[activeChannelIndex]->pairTo(nullptr);
        break;
    case PairingMode::Left:
        m_channelSliders[activeChannelIndex]->pairTo(m_channelSliders[activeChannelIndex + 1].get());
        break;
    case PairingMode::Right:
        m_channelSliders[activeChannelIndex]->pairTo(m_channelSliders[activeChannelIndex - 1].get());
        break;
    }
}

void MixerComponent::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    juce::ignoreUnused(channel);

    if (previousOutputChannel != -1)
        m_channelSliders[previousOutputChannel]->setVisible(true);
    if (outputChannel != -1)
        m_channelSliders[outputChannel]->setVisible(false);
    resized();
}

void MixerComponent::scrollBarMoved(juce::ScrollBar* /*scrollBarThatHasMoved*/, double newRangeStart)
{
    m_slidersContainer.setTopLeftPosition(-static_cast<int>(newRangeStart), 0);
}

void MixerComponent::timerCallback()
{
    for (auto&& slider : m_channelSliders)
        slider->updateLevel();
    for (auto&& slider : m_playerSliders)
        slider->updateLevel();
}
