#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

class AudioDeviceComponent
    : public juce::Component
    , public juce::Button::Listener
{
public:
    AudioDeviceComponent(juce::AudioDeviceManager& audioDeviceManager);

    // Component
    void resized() override;

    // Button::Listener
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    juce::AudioDeviceSelectorComponent selector;
    juce::TextButton selectAllChannelsButton;
    juce::TextButton deselectAllChannelsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDeviceComponent)
};
