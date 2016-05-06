#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "VolumeAnalyzer.h"

/**
	Audio source which remaps channels and tracks the current volume of each channel.
*/
class ChannelRemappingAudioSourceWithVolume : public ChannelRemappingAudioSource
{
public:
    ChannelRemappingAudioSourceWithVolume(AudioSource* source, bool deleteSourceWhenDeleted);

// Channel volume
public:
	float getVolume() const;

private:
	std::vector<VolumeAnalyzer> m_volumes;
	CriticalSection lock;
	size_t m_bufferSize;

// ChannelRemappingAudioSource
public:
	void setNumberOfChannelsToProduce(int requiredNumberOfChannels);
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

};