#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Audio source which remaps channels and tracks the current volume of each channel.
*/
class ChannelRemappingAudioSourceWithVolume : public ChannelRemappingAudioSource
{
public:
    ChannelRemappingAudioSourceWithVolume(AudioSource* source, bool deleteSourceWhenDeleted);

// Channel volume
public:
	float getVolume(size_t channel) const;

private:
	std::vector<float> m_volumes;
	CriticalSection lock;

// ChannelRemappingAudioSource
public:
	void setNumberOfChannelsToProduce(int requiredNumberOfChannels);
	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

};