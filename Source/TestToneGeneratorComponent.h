/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.0.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "OutputChannelNames.h"
#include "TestToneGeneratorOutputChannelsModel.h"
#include "TestToneGeneratorAudioSource.h"
#include "VolumeSlider.h"
#include "MixerComponent.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class TestToneGeneratorComponent  : public Component,
                                    public ChangeListener,
                                    public OutputChannelNamesListener,
                                    public SliderListener,
                                    public ComboBoxListener,
                                    public ButtonListener
{
public:
    //==============================================================================
    TestToneGeneratorComponent (MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames);
    ~TestToneGeneratorComponent();

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
    virtual void sliderValueChanged (Slider* sliderThatWasMoved) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    MixerComponent* m_mixerComponent;
    TestToneGeneratorAudioSource m_audioSource;
    OutputChannelNames* m_outputChannelNames;
    TestToneGeneratorOutputChannelsModel m_listBoxModel;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ComboBox> comboBox;
    ScopedPointer<ToggleButton> toggleButton;
    ScopedPointer<ListBox> component;
    ScopedPointer<Label> label;
    ScopedPointer<VolumeSlider> slider;
    ScopedPointer<TextButton> textButton;
    ScopedPointer<TextButton> textButton2;
    ScopedPointer<Label> dbLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestToneGeneratorComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
