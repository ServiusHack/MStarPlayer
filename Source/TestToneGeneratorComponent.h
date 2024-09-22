#pragma once

#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "TestToneGeneratorAudioSource.h"
#include "TestToneGeneratorOutputChannelsModel.h"
#include "VolumeSlider.h"

class TestToneGeneratorComponent
    : public juce::Component
    , public juce::ChangeListener
    , public OutputChannelNamesListener
    , public juce::Slider::Listener
    , public juce::ComboBox::Listener
    , public juce::Button::Listener
{
public:
    TestToneGeneratorComponent(MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames);
    ~TestToneGeneratorComponent() override;

    // ChangeListener
public:
    /** Act accordingly to changes in the AudioDeviceManager. */
    virtual void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) override;

    // OutputChannelNamesListener
public:
    virtual void outputChannelNamesReset() override;
    virtual void outputChannelNameChanged(int activeChannelIndex, const juce::String& text) override;
    virtual void outputChannelPairingModeChanged(int activeChannelIndex, PairingMode mode) override;
    virtual void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    MixerComponent* m_mixerComponent;
    TestToneGeneratorAudioSource m_audioSource;
    OutputChannelNames* m_outputChannelNames;
    TestToneGeneratorOutputChannelsModel m_listBoxModel;

    std::unique_ptr<juce::ComboBox> comboBox;
    std::unique_ptr<juce::TextButton> toggleButton;
    std::unique_ptr<juce::ListBox> component;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<VolumeSlider> slider;
    std::unique_ptr<juce::TextButton> textButton;
    std::unique_ptr<juce::TextButton> textButton2;
    std::unique_ptr<juce::Label> dbLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestToneGeneratorComponent)
};
