#include "ChannelMixerFader.h"

#include "ChannelMixerControlable.h"

ChannelMixerFader::ChannelMixerFader(int channel, ChannelVolumeAudioSource* audioSource, const MixerFader::ResizeCallback& resizeCallback)
    : MixerFader(new ChannelMixerControlable(channel, audioSource), std::vector<MixerControlable*>(), false, resizeCallback)
{
    m_volumeSlider->setColour(Slider::thumbColourId, Colour(0xffaf0000));
}
