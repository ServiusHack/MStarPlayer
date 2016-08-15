#include "ChannelRemappingAudioSourceWithVolume.h"

#include <algorithm>

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(AudioSource* const source_,
                                                          const bool deleteSourceWhenDeleted)
   : source (source_, deleteSourceWhenDeleted)
   ,  requiredNumberOfChannels (2)
    , m_bufferSize(1)
{
    remappedInfo.buffer = &buffer;
    remappedInfo.startSample = 0;
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
    const ScopedLock sl(lock);
    requiredNumberOfChannels = requiredNumberOfChannels_;
    m_volumes.resize(requiredNumberOfChannels_, VolumeAnalyzer(m_bufferSize));
}

void ChannelRemappingAudioSourceWithVolume::clearAllMappings()
{
    const ScopedLock sl (lock);

    remappedOutputs.clear();
}

int ChannelRemappingAudioSourceWithVolume::getRemappedOutputChannel (const int outputChannelIndex) const
{
    const ScopedLock sl (lock);

    if (outputChannelIndex >= 0 && outputChannelIndex < remappedOutputs.size())
        return remappedOutputs[outputChannelIndex];

    return -1;
}

void ChannelRemappingAudioSourceWithVolume::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    source->prepareToPlay (samplesPerBlockExpected, sampleRate);

    const ScopedLock sl(lock);
    m_bufferSize = static_cast<size_t>(sampleRate/10);
    auto numberOfChannels = m_volumes.size();
    m_volumes.clear();
    m_volumes.resize(numberOfChannels, VolumeAnalyzer(m_bufferSize));
    m_sampleRate = sampleRate;
}

void ChannelRemappingAudioSourceWithVolume::releaseResources()
{
    source->releaseResources();
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    const ScopedLock sl(lock);

    buffer.setSize (requiredNumberOfChannels, bufferToFill.numSamples, false, false, true);

    const int numChans = bufferToFill.buffer->getNumChannels();

    for (int i = 0; i < buffer.getNumChannels(); ++i)
    {
        buffer.clear (i, 0, bufferToFill.numSamples);
    }

    remappedInfo.numSamples = bufferToFill.numSamples;

    source->getNextAudioBlock (remappedInfo);

    bufferToFill.clearActiveBufferRegion();

    for (int i = 0; i < requiredNumberOfChannels; ++i)
    {
        {
            const int remappedChan = getRemappedOutputChannel (i);

            if (remappedChan >= 0 && remappedChan < numChans)
            {
                bufferToFill.buffer->addFrom (remappedChan, bufferToFill.startSample,
                                              buffer, i, 0, bufferToFill.numSamples);

            }
        }
    }

    for (size_t i = 0; i < m_volumes.size(); ++i) {
        m_volumes[i].update(bufferToFill.buffer->getReadPointer(i) + bufferToFill.startSample, bufferToFill.numSamples);
    }
}

XmlElement* ChannelRemappingAudioSourceWithVolume::createXml() const
{
    XmlElement* e = new XmlElement ("MAPPINGS");
    String outs;

    const ScopedLock sl (lock);

    for (int i = 0; i < remappedOutputs.size(); ++i)
        outs << remappedOutputs[i] << ' ';

    e->setAttribute ("outputs", outs.trimEnd());

    return e;
}

void ChannelRemappingAudioSourceWithVolume::restoreFromXml (const XmlElement& e)
{
    if (e.hasTagName ("MAPPINGS"))
    {
        const ScopedLock sl (lock);

        clearAllMappings();

        StringArray outs;
        outs.addTokens (e.getStringAttribute ("outputs"), false);

        for (int i = 0; i < outs.size(); ++i)
            remappedOutputs.add (outs[i].getIntValue());
    }
}

float ChannelRemappingAudioSourceWithVolume::getVolume() const
{
    const ScopedLock sl(lock);
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
    const ScopedLock sl (lock);    setOutputChannelMappingInternal(sourceChannelIndex, destChannelIndex);
}

void ChannelRemappingAudioSourceWithVolume::setOutputChannelMappingInternal(const int sourceIndex, const int destIndex)
{
    while (remappedOutputs.size() < sourceIndex+1)
        remappedOutputs.add(-1);

    remappedOutputs.set(sourceIndex, destIndex);
}
