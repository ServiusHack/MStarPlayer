#include "ChannelRemappingAudioSourceWithVolume.h"
#include "Utils.h"

#include <algorithm>

ChannelRemappingAudioSourceWithVolume::ChannelRemappingAudioSourceWithVolume(AudioSource* const source_,
                                                          const bool deleteSourceWhenDeleted)
    : ChannelRemappingAudioSource(source_, deleteSourceWhenDeleted)
	, m_bufferSize(0)
{
}

void ChannelRemappingAudioSourceWithVolume::setNumberOfChannelsToProduce(int requiredNumberOfChannels_)
{
	const ScopedLock sl(lock);
	m_volumes.resize(requiredNumberOfChannels_, boost::circular_buffer<float>(m_bufferSize,0.0f));
	ChannelRemappingAudioSource::setNumberOfChannelsToProduce(requiredNumberOfChannels_);
}

void ChannelRemappingAudioSourceWithVolume::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	ChannelRemappingAudioSource::prepareToPlay(samplesPerBlockExpected, sampleRate);
	const ScopedLock sl(lock);
	m_bufferSize = static_cast<size_t>(sampleRate);
	auto numberOfChannels = m_volumes.size();
	m_volumes.clear();
	m_volumes.resize(numberOfChannels, boost::circular_buffer<float>(m_bufferSize,0.0f));
}

void ChannelRemappingAudioSourceWithVolume::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	const ScopedLock sl(lock);
	ChannelRemappingAudioSource::getNextAudioBlock(bufferToFill);
	for (size_t i = 0; i < m_volumes.size(); ++i) {
		const float* buffer = bufferToFill.buffer->getReadPointer(i) + bufferToFill.startSample;
		std::copy(buffer, buffer + bufferToFill.numSamples, m_volumes[i].begin());
	}
}

float ChannelRemappingAudioSourceWithVolume::getVolume(size_t channel) const
{
	const ScopedLock sl(lock);
	jassert(channel < m_volumes.size());
	
	return std::abs(*std::max_element(m_volumes[channel].begin(), m_volumes[channel].end(), Utils::absoluteCompare<float>));
}