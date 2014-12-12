#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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