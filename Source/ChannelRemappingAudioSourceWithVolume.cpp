#include "ChannelRemappingAudioSourceWithVolume.h"

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(AudioSource* const source_,
                                                          const bool deleteSourceWhenDeleted)
    : ChannelRemappingAudioSource(source_, deleteSourceWhenDeleted)
{
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
	const ScopedLock sl(lock);
	m_volumes.resize(requiredNumberOfChannels_);
	ChannelRemappingAudioSource::setNumberOfChannelsToProduce(requiredNumberOfChannels_);
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const ScopedLock sl(lock);
	ChannelRemappingAudioSource::getNextAudioBlock(bufferToFill);
	for (int i = 0; i < m_volumes.size(); ++i)
		m_volumes.set(i, bufferToFill.buffer->getRMSLevel(i, bufferToFill.startSample, bufferToFill.numSamples));
}

float ChannelRemappingAudioSourceWithVolume::getVolume(int channel)
{
	const ScopedLock sl(lock);
	if (channel < m_volumes.size())
		return m_volumes.getUnchecked(channel);
	else
		return 0.0f;
}