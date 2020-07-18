/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "MixerComponent.h"
#include "OutputChannelNames.h"
#include "TestToneGeneratorAudioSource.h"
#include "TestToneGeneratorOutputChannelsModel.h"
#include "VolumeSlider.h"
//[/Headers]

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class TestToneGeneratorComponent
    : public Component
    , public ChangeListener
    , public OutputChannelNamesListener
    , public Slider::Listener
    , public juce::ComboBox::Listener
    , public juce::Button::Listener
{
public:
    //==============================================================================
    TestToneGeneratorComponent(MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames);
    ~TestToneGeneratorComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    // ChangeListener
public:
    /** Act accordingly to changes in the AudioDeviceManager. */
    virtual void changeListenerCallback(ChangeBroadcaster* /*source*/) override;

    // OutputChannelNamesListener
public:
    virtual void outputChannelNamesReset() override;
    virtual void outputChannelNameChanged(int activeChannelIndex, const String& text) override;
    virtual void outputChannelPairingModeChanged(int activeChannelIndex, PairingMode mode) override;
    virtual void sliderValueChanged(Slider* sliderThatWasMoved) override;
    //[/UserMethods]

    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    MixerComponent* m_mixerComponent;
    TestToneGeneratorAudioSource m_audioSource;
    OutputChannelNames* m_outputChannelNames;
    TestToneGeneratorOutputChannelsModel m_listBoxModel;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::ComboBox> comboBox;
    std::unique_ptr<juce::TextButton> toggleButton;
    std::unique_ptr<ListBox> component;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<VolumeSlider> slider;
    std::unique_ptr<juce::TextButton> textButton;
    std::unique_ptr<juce::TextButton> textButton2;
    std::unique_ptr<juce::Label> dbLabel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestToneGeneratorComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
