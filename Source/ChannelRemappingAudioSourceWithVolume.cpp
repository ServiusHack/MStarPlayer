#include "ChannelRemappingAudioSourceWithVolume.h"

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(AudioSource* const source_,
                                                          const bool deleteSourceWhenDeleted)
    : ChannelRemappingAudioSource(source_, deleteSourceWhenDeleted)
{
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
	const ScopedLock sl(lock);
	m_volumes.resize(requiredNumberOfChannels_, 0.0f);
	ChannelRemappingAudioSource::setNumberOfChannelsToProduce(requiredNumberOfChannels_);
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const ScopedLock sl(lock);
	ChannelRemappingAudioSource::getNextAudioBlock(bufferToFill);
	for (size_t i = 0; i < m_volumes.size(); ++i)
		m_volumes[i] = std::max(m_volumes[i], bufferToFill.buffer->getMagnitude(i, bufferToFill.startSample, bufferToFill.numSamples));
}

float ChannelRemappingAudioSourceWithVolume::takeVolume(size_t channel)
{
	const ScopedLock sl(lock);
	jassert(channel < m_volumes.size());
	
	auto volume = m_volumes[channel];
	m_volumes[channel] = 0.0f;
	return volume;
}