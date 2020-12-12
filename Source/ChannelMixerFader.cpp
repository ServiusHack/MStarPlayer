#include "ChannelMixerFader.h"

#include "juce_gui_basics/juce_gui_basics.h"

#include "ChannelMixerControlable.h"

ChannelMixerFader::ChannelMixerFader(
    int channel, ChannelVolumeAudioSource* audioSource, const MixerFader::ResizeCallback& resizeCallback)
    : MixerFader(
        new ChannelMixerControlable(channel, audioSource), std::vector<MixerControlable*>(), false, resizeCallback)
{
    m_volumeSlider->setColour(juce::Slider::thumbColourId, juce::Colour(0xffaf0000));
}
