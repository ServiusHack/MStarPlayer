#include "SoloBusMixer.h"

SoloBusMixer::SoloBusMixer(SoloBusSettings& soloBusSettings, ChannelVolumeAudioSource& channelVolumeAudioSource)
    : m_soloBusSettings(soloBusSettings)
    , m_leftMixer(-1, &channelVolumeAudioSource, TRANS("Solo L"))
    , m_rightMixer(-1, &channelVolumeAudioSource, TRANS("Solo R"))
    , m_leftFader(&m_leftMixer, {}, false, MixerFader::ResizeCallback(), false, false)
    , m_rightFader(&m_rightMixer, {}, false, MixerFader::ResizeCallback(), false, false)
{
    m_leftFader.setEnabled(false);
    m_rightFader.setEnabled(false);
    addAndMakeVisible(m_leftFader);
    addAndMakeVisible(m_rightFader);

    soloBusSettings.addListener(this);

    startTimer(1000 / LevelMeter::refreshRate);
}

SoloBusMixer::~SoloBusMixer()
{
    m_soloBusSettings.removeListener(this);
}

void SoloBusMixer::resized()
{
    m_leftFader.setBounds(0, 0, getWidth() / 2, getHeight());
    m_rightFader.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight());
}

void SoloBusMixer::saveToXml(juce::XmlElement* element) const
{
    juce::XmlElement* leftElement = new juce::XmlElement("Left");
    leftElement->addTextElement(juce::String(m_leftFader.getValue()));
    element->addChildElement(leftElement);

    juce::XmlElement* rightElement = new juce::XmlElement("Left");
    rightElement->addTextElement(juce::String(m_rightFader.getValue()));
    element->addChildElement(rightElement);
}

void SoloBusMixer::restoreFromXml(const juce::XmlElement& element)
{
    juce::XmlElement* leftElement = element.getChildByName("Left");
    if (leftElement)
    {
        m_leftFader.setValue(leftElement->getAllSubText().trim().getFloatValue());
    }

    juce::XmlElement* rightElement = element.getChildByName("Left");
    if (rightElement)
    {
        m_rightFader.setValue(rightElement->getAllSubText().trim().getFloatValue());
    }
}

void SoloBusMixer::soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    juce::ignoreUnused(previousOutputChannel);

    switch (channel)
    {
    case SoloBusChannel::Left:
        m_leftMixer.setChannel(outputChannel);
        m_leftFader.setEnabled(outputChannel != -1);
        break;
    case SoloBusChannel::Right:
        m_rightMixer.setChannel(outputChannel);
        m_rightFader.setEnabled(outputChannel != -1);
        break;
    }

    resized();
}

void SoloBusMixer::timerCallback()
{
    m_leftFader.updateLevel();
    m_rightFader.updateLevel();
}
