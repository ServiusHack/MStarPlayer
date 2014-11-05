#include "ChannelVolumeAudioSource.h"


ChannelVolumeAudioSource::ChannelVolumeAudioSource(AudioSource* const source)
   : m_source(source)
{
}

void ChannelVolumeAudioSource::resetAllVolumes()
{
    const ScopedLock sl(m_lock);

    m_volumes.clear();
}

void ChannelVolumeAudioSource::setChannelVolume(const int channelIndex, float gain)
{
    const ScopedLock sl(m_lock);

    while (m_volumes.size() < channelIndex)
        m_volumes.add(1.0);

    m_volumes.set(channelIndex, gain);
}

float ChannelVolumeAudioSource::getChannelVolume(const int channelIndex) const
{
    const ScopedLock sl (m_lock);

    if (channelIndex >= 0 && channelIndex < m_volumes.size())
        return m_volumes.getUnchecked(channelIndex);

    return -1.0;
}

void ChannelVolumeAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_source->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void ChannelVolumeAudioSource::releaseResources()
{
    m_source->releaseResources();
}

void ChannelVolumeAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    const ScopedLock sl (m_lock);

    m_source->getNextAudioBlock(bufferToFill);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
    {
        float gain = 1.0;
        if (channel < m_volumes.size())
            gain = m_volumes.getUnchecked(channel);
        bufferToFill.buffer->applyGain(channel, bufferToFill.startSample, bufferToFill.numSamples, gain);
    }
}