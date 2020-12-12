#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_gui_basics/juce_gui_basics.h"

/**
        Draw a waveform.
*/
class Waveform
    : public juce::Component
    , public juce::ChangeListener
{
public:
    Waveform(juce::Colour color = juce::Colours::black);

    void setAudioThumbnail(juce::AudioThumbnail* audioThumbnail);

    virtual void paint(juce::Graphics& g) override;

private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    juce::Colour m_color;
    juce::AudioThumbnail* m_audioThumbnail;
};
