#pragma once

#include "ChannelVolumeAudioSource.h"
#include "MixerFader.h"

/**
        MixerFader for an output channel.
*/
class ChannelMixerFader : public MixerFader
{
public:
    ChannelMixerFader(
        int channel, ChannelVolumeAudioSource* audioSource, const MixerFader::ResizeCallback& resizeCallback);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelMixerFader)
};
