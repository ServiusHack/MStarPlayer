#include "ChannelRemappingAudioSourceWithVolume.h"

#include <algorithm>

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(
    juce::AudioSource* const source_, SoloBusSettings& soloBusSettings, const bool deleteSourceWhenDeleted)
    : source(source_, deleteSourceWhenDeleted)
    , requiredNumberOfChannels(2)
    , m_decayRate(0)
    , m_soloBusSettings(soloBusSettings)
{
    m_soloBusSettings.addListener(this);
    remappedInfo.buffer = &buffer;
    remappedInfo.startSample = 0;
}

ChannelRemappingAudioSourceWithVolume::~ChannelRemappingAudioSourceWithVolume()
{
    m_soloBusSettings.removeListener(this);
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
    const juce::ScopedLock sl(lock);
    requiredNumberOfChannels = requiredNumberOfChannels_;

    if (requiredNumberOfChannels > m_volumes.size())
        m_volumes.insertMultiple(
            m_volumes.size(), VolumeAnalyzer(m_decayRate), requiredNumberOfChannels_ - m_volumes.size());
}

int ChannelRemappingAudioSourceWithVolume::getRemappedOutputChannel(const int outputChannelIndex) const
{
    const juce::ScopedLock sl(lock);

    if (outputChannelIndex >= 0 && outputChannelIndex < remappedOutputs.size())
        return remappedOutputs[outputChannelIndex].first;

    return -1;
}

void ChannelRemappingAudioSourceWithVolume::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    source->prepareToPlay(samplesPerBlockExpected, sampleRate);

    const juce::ScopedLock sl(lock);
    m_decayRate = static_cast<float>(sampleRate / 10);
    auto numberOfChannels = m_volumes.size();
    m_volumes.clear();
    if (numberOfChannels > m_volumes.size())
        m_volumes.insertMultiple(m_volumes.size(), VolumeAnalyzer(m_decayRate), numberOfChannels - m_volumes.size());
    m_sampleRate = sampleRate;
}

void ChannelRemappingAudioSourceWithVolume::releaseResources()
{
    source->releaseResources();
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    const juce::ScopedLock sl(lock);

    buffer.setSize(requiredNumberOfChannels, bufferToFill.numSamples, false, false, true);

    const int numChans = bufferToFill.buffer->getNumChannels();

    for (int i = 0; i < buffer.getNumChannels(); ++i)
    {
        buffer.clear(i, 0, bufferToFill.numSamples);
    }

    remappedInfo.numSamples = bufferToFill.numSamples;

    source->getNextAudioBlock(remappedInfo);

    bufferToFill.clearActiveBufferRegion();

    for (int i = 0; i < requiredNumberOfChannels; ++i)
    {
        {
            const int remappedChan = getRemappedOutputChannel(i);

            if (remappedChan >= 0 && remappedChan < numChans)
            {
                float gain = 1.0f;
                if (remappedChan < sourceChannelGain.size())
                    gain = sourceChannelGain.getReference(remappedChan);
                bufferToFill.buffer->addFrom(
                    remappedChan, bufferToFill.startSample, buffer, i, 0, bufferToFill.numSamples, gain);
            }
        }

        {
            int remappedChan = -1;
            if (i >= 0 && i < remappedOutputs.size())
                remappedChan = remappedOutputs[i].second;

            if (remappedChan >= 0 && remappedChan < numChans)
            {
                bufferToFill.buffer->addFrom(
                    remappedChan, bufferToFill.startSample, buffer, i, 0, bufferToFill.numSamples);
            }
        }
    }

    for (int i = 0; i < m_volumes.size(); ++i)
    {
        if (i == m_soloLeftChannel || i == m_soloRightChannel)
            continue;
        m_volumes.getReference(i).update(
            bufferToFill.buffer->getReadPointer(i) + bufferToFill.startSample, bufferToFill.numSamples);
    }
}

juce::XmlElement* ChannelRemappingAudioSourceWithVolume::createXml() const
{
    juce::XmlElement* e = new juce::XmlElement("MAPPINGS");
    juce::String outs;

    const juce::ScopedLock sl(lock);

    for (int i = 0; i < remappedOutputs.size(); ++i)
        outs << remappedOutputs[i].first << ' ';

    e->setAttribute("outputs", outs.trimEnd());

    e->setAttribute("soloLeft", m_soloLeftChannel);
    e->setAttribute("soloRight", m_soloRightChannel);

    return e;
}

void ChannelRemappingAudioSourceWithVolume::restoreFromXml(const juce::XmlElement& e)
{
    if (e.hasTagName("MAPPINGS"))
    {
        const juce::ScopedLock sl(lock);

        remappedOutputs.clear();

        juce::StringArray outs;
        outs.addTokens(e.getStringAttribute("outputs"), false);

        for (int i = 0; i < outs.size(); ++i)
            remappedOutputs.add(std::pair<int, int>(outs[i].getIntValue(), -1));

        m_soloLeftChannel = e.getIntAttribute("soloLeft", -1);
        m_soloRightChannel = e.getIntAttribute("soloRight", -1);
    }
}

float ChannelRemappingAudioSourceWithVolume::getVolume() const
{
    const juce::ScopedLock sl(lock);
    float maxVolume = 0;
    for (auto&& volume : m_volumes)
        maxVolume = std::max(maxVolume, volume.getVolume());

    return maxVolume;
}

double ChannelRemappingAudioSourceWithVolume::getSampleRate() const
{
    return m_sampleRate;
}

void ChannelRemappingAudioSourceWithVolume::setOutputChannelMapping(int sourceChannelIndex, int destChannelIndex)
{
    const juce::ScopedLock sl(lock);
    setOutputChannelMappingInternal(sourceChannelIndex, destChannelIndex, false);
}

void ChannelRemappingAudioSourceWithVolume::setSourceChannelGain(int sourceIndex, float gain)
{
    while (sourceChannelGain.size() < sourceIndex + 1)
        sourceChannelGain.add(1.0f);

    sourceChannelGain.getReference(sourceIndex) = gain;
}

void ChannelRemappingAudioSourceWithVolume::setOutputChannelMappingInternal(
    const int sourceIndex, const int destIndex, const bool solo)
{
    while (remappedOutputs.size() < sourceIndex + 1)
        remappedOutputs.add(std::pair<int, int>(-1, -1));

    if (solo)
        remappedOutputs.getReference(sourceIndex).second = destIndex;
    else
        remappedOutputs.getReference(sourceIndex).first = destIndex;
}

void ChannelRemappingAudioSourceWithVolume::setSolo(bool solo)
{
    const juce::ScopedLock sl(lock);

    m_solo = solo;
    if (solo)
    {
        setOutputChannelMappingInternal(0, m_soloLeftChannel, true);
        setOutputChannelMappingInternal(1, m_soloRightChannel, true);
    }
    else
    {
        setOutputChannelMappingInternal(0, -1, true);
        setOutputChannelMappingInternal(1, -1, true);
    }
}

void ChannelRemappingAudioSourceWithVolume::soloBusChannelChanged(
    SoloBusChannel channel, int outputChannel, int previousOutputChannel)
{
    juce::ignoreUnused(previousOutputChannel);
    const juce::ScopedLock sl(lock);

    switch (channel)
    {
    case SoloBusChannel::Left:
        m_soloLeftChannel = outputChannel;
        if (m_solo)
        {
            setOutputChannelMappingInternal(0, m_soloLeftChannel, true);
        }
        break;
    case SoloBusChannel::Right:
        m_soloRightChannel = outputChannel;
        if (m_solo)
        {
            setOutputChannelMappingInternal(1, m_soloRightChannel, true);
        }
        break;
    }

    for (auto&& output : remappedOutputs)
    {
        while (output.first == m_soloLeftChannel || output.first == m_soloRightChannel)
        {
            ++output.first;
        }
    }
}
