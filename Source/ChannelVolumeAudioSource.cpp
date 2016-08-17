#include "ChannelVolumeAudioSource.h"

ChannelVolumeAudioSource::ChannelVolumeAudioSource(AudioSource* const source)
    : m_source(source)
    , m_anySolo(false)
{
}

void ChannelVolumeAudioSource::resetAllVolumes()
{
    const ScopedLock sl(m_lock);

    m_setVolumes.clear();
    m_appliedGains.clear();
}

void ChannelVolumeAudioSource::setChannelVolume(size_t channelIndex, float gain)
{
    const ScopedLock sl(m_lock);

    m_setVolumes[channelIndex] = gain;

    updateGain(channelIndex);
}

float ChannelVolumeAudioSource::getChannelVolume(size_t channelIndex) const
{
    const ScopedLock sl(m_lock);

    if (channelIndex >= 0 && channelIndex < m_setVolumes.size())
        return m_setVolumes[channelIndex];

    return 1.0;
}

void ChannelVolumeAudioSource::setChannelSolo(size_t channelIndex, bool solo)
{
    const ScopedLock sl(m_lock);

    m_setSolos[channelIndex] = solo;

    bool anySolo = std::any_of(m_setSolos.begin(), m_setSolos.end(), [](bool solo) { return solo; });

    if (m_anySolo != anySolo)
    {
        // need to update every channel gain

        m_anySolo = anySolo;
        for (size_t i = 0; i < m_appliedGains.size(); ++i)
            updateGain(i);
    }
    else
    {
        updateGain(channelIndex);
    }
}

bool ChannelVolumeAudioSource::getChannelSolo(size_t channelIndex) const
{
    const ScopedLock sl(m_lock);

    if (channelIndex >= 0 && channelIndex < m_setSolos.size())
        return m_setSolos[channelIndex];

    return false;
}

void ChannelVolumeAudioSource::setChannelMute(size_t channelIndex, bool mute)
{
    const ScopedLock sl(m_lock);

    m_setMutes[channelIndex] = mute;

    updateGain(channelIndex);
}

bool ChannelVolumeAudioSource::getChannelMute(size_t channelIndex) const
{
    const ScopedLock sl(m_lock);

    if (channelIndex >= 0 && channelIndex < m_setMutes.size())
        return m_setMutes[channelIndex];

    return false;
}

float ChannelVolumeAudioSource::getActualVolume(size_t channelIndex) const
{
    const ScopedLock sl(m_lock);

    if (channelIndex >= 0 && channelIndex < m_actualVolumes.size())
    {
        return m_actualVolumes[channelIndex].getVolume();
    }

    return 0.0f;
}

void ChannelVolumeAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_source->prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_bufferSize = static_cast<size_t>(sampleRate / 10);
    auto numberOfChannels = m_actualVolumes.size();
    m_actualVolumes.clear();
    m_actualVolumes.resize(numberOfChannels, VolumeAnalyzer(m_bufferSize));
}

void ChannelVolumeAudioSource::releaseResources()
{
    m_source->releaseResources();
}

void ChannelVolumeAudioSource::setChannelCount(int channelCount)
{
    channelCount += 1;
    if (channelCount > m_setVolumes.size())
        m_setVolumes.resize(channelCount, 1.0);
    if (channelCount > m_setSolos.size())
        m_setSolos.resize(channelCount, false);
    if (channelCount > m_setMutes.size())
        m_setMutes.resize(channelCount, false);
    if (channelCount > m_actualVolumes.size())
        m_actualVolumes.resize(channelCount, VolumeAnalyzer(m_bufferSize));
    if (channelCount > m_appliedGains.size())
        m_appliedGains.resize(channelCount, 1.0f);
}

int ChannelVolumeAudioSource::channelCount() const
{
    return m_setVolumes.size();
}

void ChannelVolumeAudioSource::updateGain(size_t channelIndex)
{
    bool mute = m_setMutes[channelIndex];
    bool solo = m_setSolos[channelIndex];
    float gain = (mute || (m_anySolo && !solo)) ? 0.0f : m_setVolumes[channelIndex];
    m_appliedGains[channelIndex] = gain;
}

void ChannelVolumeAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    const ScopedLock sl(m_lock);

    m_source->getNextAudioBlock(bufferToFill);

    size_t numberOfChannels = bufferToFill.buffer->getNumChannels();
    for (size_t channel = 0; channel < numberOfChannels; channel++)
    {
        float gain = 1.0f;
        if (channel < m_appliedGains.size())
            gain = m_appliedGains[channel];
        else if (m_anySolo)
            gain = 0.0f; // some channel is in solo but not this one (otherwise it would be in m_appliedGains)
        bufferToFill.buffer->applyGain(channel, bufferToFill.startSample, bufferToFill.numSamples, gain);
        if (channel < m_actualVolumes.size())
        {
            const float* buffer = bufferToFill.buffer->getReadPointer(channel) + bufferToFill.startSample;
            m_actualVolumes[channel].update(buffer, bufferToFill.numSamples);
        }
    }
}