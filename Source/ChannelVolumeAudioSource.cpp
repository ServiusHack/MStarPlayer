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

void ChannelVolumeAudioSource::setChannelVolume(const int channelIndex, float gain)
{
    const ScopedLock sl(m_lock);

	expandListsTo(channelIndex);

	m_setVolumes.set(channelIndex, gain);

	updateGain(channelIndex);
}

float ChannelVolumeAudioSource::getChannelVolume(const int channelIndex) const
{
    const ScopedLock sl (m_lock);

	if (channelIndex >= 0 && channelIndex < m_setVolumes.size())
		return m_setVolumes.getUnchecked(channelIndex);

    return -1.0;
}

void ChannelVolumeAudioSource::setChannelSolo(int channelIndex, bool solo)
{
	const ScopedLock sl(m_lock);

	expandListsTo(channelIndex);

	m_setSolos.set(channelIndex, solo);

	bool anySolo = std::any_of(m_setSolos.begin(), m_setSolos.end(), [](bool solo) { return solo; });

	if (m_anySolo != anySolo)
	{
		// need to update every channel gain

		m_anySolo = anySolo;
		for (int i = 0; i < m_appliedGains.size(); ++i)
			updateGain(i);
	}
	else
	{
		updateGain(channelIndex);
	}
}

bool ChannelVolumeAudioSource::getChannelSolo(int channelIndex)
{
	const ScopedLock sl(m_lock);

	return m_setSolos.getUnchecked(channelIndex);
}

void ChannelVolumeAudioSource::setChannelMute(int channelIndex, bool mute)
{
	const ScopedLock sl(m_lock);

	expandListsTo(channelIndex);

	m_setMutes.set(channelIndex, mute);

	updateGain(channelIndex);
}

bool ChannelVolumeAudioSource::getChannelMute(int channelIndex)
{
	const ScopedLock sl(m_lock);

	return m_setMutes.getUnchecked(channelIndex);
}

void ChannelVolumeAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_source->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void ChannelVolumeAudioSource::releaseResources()
{
    m_source->releaseResources();
}

void ChannelVolumeAudioSource::expandListsTo(int channelIndex)
{
	while (m_setVolumes.size() <= channelIndex)
		m_setVolumes.add(1.0);

	while (m_setSolos.size() <= channelIndex)
		m_setSolos.add(false);

	while (m_setMutes.size() <= channelIndex)
		m_setMutes.add(false);
}

int ChannelVolumeAudioSource::channelCount()
{
	return m_setVolumes.size();
}

void ChannelVolumeAudioSource::updateGain(int channelIndex)
{
	while (m_appliedGains.size() < channelIndex)
		m_appliedGains.add(1.0);

	bool mute = m_setMutes.getUnchecked(channelIndex);
	bool solo = m_setSolos.getUnchecked(channelIndex);
	float gain = (mute || (m_anySolo && !solo ) ) ? 0.0f : m_setVolumes.getUnchecked(channelIndex);
	m_appliedGains.set(channelIndex, gain);
}

void ChannelVolumeAudioSource::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    const ScopedLock sl (m_lock);

    m_source->getNextAudioBlock(bufferToFill);

    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++)
    {
        float gain = 1.0;
        if (channel < m_appliedGains.size())
			gain = m_appliedGains.getUnchecked(channel);
        bufferToFill.buffer->applyGain(channel, bufferToFill.startSample, bufferToFill.numSamples, gain);
    }
}