#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerFader.h"
#include "ChannelVolumeAudioSource.h"

class ChannelMixerFader : public MixerFader
{
public:
	ChannelMixerFader(int channel, ChannelVolumeAudioSource* audioSource, MixerFader::ResizeCallback resizeCallback);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelMixerFader)
};