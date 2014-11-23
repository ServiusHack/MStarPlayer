#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ChannelRemappingAudioSourceWithVolume : public ChannelRemappingAudioSource
{
public:
    ChannelRemappingAudioSourceWithVolume(AudioSource* source,
                                 bool deleteSourceWhenDeleted);

	void setNumberOfChannelsToProduce(int requiredNumberOfChannels);

	virtual void getNextAudioBlock(const AudioSourceChannelInfo&) override;

	float getVolume(int channel);

private:
	Array<float> m_volumes;
	CriticalSection lock;
};