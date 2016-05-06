#include "ChannelRemappingAudioSourceWithVolume.h"

#include <algorithm>

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(AudioSource* const source_,
                                                          const bool deleteSourceWhenDeleted)
    : ChannelRemappingAudioSource(source_, deleteSourceWhenDeleted)
	, m_bufferSize(1)
{
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
	const ScopedLock sl(lock);
	m_volumes.resize(requiredNumberOfChannels_, VolumeAnalyzer(m_bufferSize));
	ChannelRemappingAudioSource::setNumberOfChannelsToProduce(requiredNumberOfChannels_);
}

void ChannelRemappingAudioSourceWithVolume::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	ChannelRemappingAudioSource::prepareToPlay(samplesPerBlockExpected, sampleRate);
	const ScopedLock sl(lock);
	m_bufferSize = static_cast<size_t>(sampleRate/10);
	auto numberOfChannels = m_volumes.size();
	m_volumes.clear();
	m_volumes.resize(numberOfChannels, VolumeAnalyzer(m_bufferSize));
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const ScopedLock sl(lock);
	ChannelRemappingAudioSource::getNextAudioBlock(bufferToFill);
	for (size_t i = 0; i < m_volumes.size(); ++i) {
		const float* buffer = bufferToFill.buffer->getReadPointer(i) + bufferToFill.startSample;
		m_volumes[i].update(buffer, bufferToFill.numSamples);
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