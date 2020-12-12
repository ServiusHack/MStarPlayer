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

//[Headers] You can add your own extra header files here...
#include "juce_audio_basics/juce_audio_basics.h"
//[/Headers]

#include "TestToneGeneratorComponent.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
TestToneGeneratorComponent::TestToneGeneratorComponent(
    MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames)
    : m_mixerComponent(mixerComponent)
    , m_outputChannelNames(outputChannelNames)
{
    //[Constructor_pre] You can add your own custom stuff here..
    m_outputChannelNames->addListener(this);
    m_outputChannelNames->addChangeListener(this);
    //[/Constructor_pre]

    comboBox.reset(new juce::ComboBox("new combo box"));
    addAndMakeVisible(comboBox.get());
    comboBox->setEditableText(false);
    comboBox->setJustificationType(juce::Justification::centredLeft);
    comboBox->setTextWhenNothingSelected(juce::String());
    comboBox->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    comboBox->addItem(TRANS("1kHz"), 1);
    comboBox->addItem(TRANS("White"), 2);
    comboBox->addListener(this);

    comboBox->setBounds(64, 16, 112, 24);

    toggleButton.reset(new juce::TextButton("new button"));
    addAndMakeVisible(toggleButton.get());
    toggleButton->setButtonText(TRANS("play"));
    toggleButton->addListener(this);

    toggleButton->setBounds(200, 16, 86, 24);

    component.reset(new juce::ListBox());
    addAndMakeVisible(component.get());
    component->setName("new component");

    label.reset(new juce::Label("new label", TRANS("Tone:")));
    addAndMakeVisible(label.get());
    label->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    label->setJustificationType(juce::Justification::centredLeft);
    label->setEditable(false, false, false);
    label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    label->setBounds(8, 16, 64, 24);

    slider.reset(new VolumeSlider());
    addAndMakeVisible(slider.get());
    slider->setName("new slider");

    slider->setBounds(8, 56, 40, 296);

    textButton.reset(new juce::TextButton("new button"));
    addAndMakeVisible(textButton.get());
    textButton->setButtonText(TRANS("select all"));
    textButton->setConnectedEdges(juce::Button::ConnectedOnRight);
    textButton->addListener(this);

    textButton2.reset(new juce::TextButton("new button"));
    addAndMakeVisible(textButton2.get());
    textButton2->setButtonText(TRANS("select none"));
    textButton2->setConnectedEdges(juce::Button::ConnectedOnLeft);
    textButton2->addListener(this);

    dbLabel.reset(new juce::Label("new label", TRANS("0 dB")));
    addAndMakeVisible(dbLabel.get());
    dbLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    dbLabel->setJustificationType(juce::Justification::centredLeft);
    dbLabel->setEditable(false, false, false);
    dbLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    dbLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    dbLabel->setBounds(8, 360, 280, 24);

    //[UserPreSize]
    toggleButton->setEnabled(false);
    toggleButton->setWantsKeyboardFocus(false);
    toggleButton->setClickingTogglesState(true);
    //[/UserPreSize]

    setSize(296, 400);

    //[Constructor] You can add your own custom stuff here..
    component->setMultipleSelectionEnabled(true);
    component->setModel(&m_listBoxModel);
    m_listBoxModel.resizeTo(m_outputChannelNames->getNumberOfChannels());
    for (int i = 0; i < m_outputChannelNames->getNumberOfChannels(); i++)
    {
        m_listBoxModel.setChannelName(i, m_outputChannelNames->getInternalOutputChannelName(i));
    }
    m_audioSource.setNumChannels(m_outputChannelNames->getNumberOfChannels());
    component->updateContent();

    m_listBoxModel.m_changeCallback = [this](int row, bool enable) {
        component->selectRow(row, true, false);
        m_audioSource.setEnable(row, enable);
    };

    slider->addListener(this);
    slider->setValue(m_audioSource.getVolume());
    //[/Constructor]
}

TestToneGeneratorComponent::~TestToneGeneratorComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    m_outputChannelNames->removeListener(this);
    m_outputChannelNames->removeChangeListener(this);
    m_mixerComponent->getMixerAudioSource().removeInputSource(&m_audioSource);
    slider->removeListener(this);
    //[/Destructor_pre]

    comboBox = nullptr;
    toggleButton = nullptr;
    component = nullptr;
    label = nullptr;
    slider = nullptr;
    textButton = nullptr;
    textButton2 = nullptr;
    dbLabel = nullptr;

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void TestToneGeneratorComponent::paint(juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll(juce::Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TestToneGeneratorComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds(56, 56, getWidth() - 64, getHeight() - 144);
    textButton->setBounds(56, 320, 112, 24);
    textButton2->setBounds(168, 320, 118, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TestToneGeneratorComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == comboBox.get())
    {
        //[UserComboBoxCode_comboBox] -- add your combo box handling code here..
        if (comboBoxThatHasChanged->isEnabled())
            toggleButton->setEnabled(true);

        if (comboBoxThatHasChanged->getSelectedId() == 1)
            m_audioSource.setMode(TestToneGeneratorAudioSource::Mode::Sine1kHz);
        else if (comboBoxThatHasChanged->getSelectedId() == 2)
            m_audioSource.setMode(TestToneGeneratorAudioSource::Mode::White);
        //[/UserComboBoxCode_comboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void TestToneGeneratorComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == toggleButton.get())
    {
        //[UserButtonCode_toggleButton] -- add your button handler code here..
        if (toggleButton->getToggleState())
        {
            m_mixerComponent->getMixerAudioSource().addInputSource(&m_audioSource, false);
        }
        else
        {
            m_mixerComponent->getMixerAudioSource().removeInputSource(&m_audioSource);
        }
        //[/UserButtonCode_toggleButton]
    }
    else if (buttonThatWasClicked == textButton.get())
    {
        //[UserButtonCode_textButton] -- add your button handler code here..
        m_listBoxModel.selectAll();
        component->updateContent();
        //[/UserButtonCode_textButton]
    }
    else if (buttonThatWasClicked == textButton2.get())
    {
        //[UserButtonCode_textButton2] -- add your button handler code here..
        m_listBoxModel.deselectAll();
        component->updateContent();
        //[/UserButtonCode_textButton2]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void TestToneGeneratorComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    int numberOfChannels = m_outputChannelNames->getNumberOfChannels();
    int oldSize = m_listBoxModel.getNumRows();
    m_listBoxModel.resizeTo(numberOfChannels);
    for (int i = oldSize; i < numberOfChannels; ++i)
    {
        m_listBoxModel.setChannelName(i, m_outputChannelNames->getInternalOutputChannelName(i));
    }
    m_audioSource.setNumChannels(m_outputChannelNames->getNumberOfChannels());
    component->updateContent();
}

void TestToneGeneratorComponent::outputChannelNamesReset() {}

void TestToneGeneratorComponent::outputChannelNameChanged(int activeChannelIndex, const juce::String& text)
{
    m_listBoxModel.setChannelName(activeChannelIndex, text);
    component->updateContent();
}

void TestToneGeneratorComponent::outputChannelPairingModeChanged(int /*activeChannelIndex*/, PairingMode /*mode*/) {}

void TestToneGeneratorComponent::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider.get())
    {
        //[UserSliderCode_slider] -- add your slider handling code here..
        m_audioSource.setVolume(sliderThatWasMoved->getValue());
        dbLabel->setText(juce::Decibels::toString(juce::Decibels::gainToDecibels(sliderThatWasMoved->getValue())),
            juce::sendNotification);
        //[/UserSliderCode_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}
//[/MiscUserCode]

//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TestToneGeneratorComponent"
                 componentName="" parentClasses="public Component, public ChangeListener, public OutputChannelNamesListener, public Slider::Listener"
                 constructorParams="MixerComponent* mixerComponent, OutputChannelNames* outputChannelNames"
                 variableInitialisers="m_mixerComponent(mixerComponent)&#10;m_outputChannelNames(outputChannelNames)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="296" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <COMBOBOX name="new combo box" id="2627f0e5c5151fd2" memberName="comboBox"
            virtualName="" explicitFocusOrder="0" pos="64 16 112 24" editable="0"
            layout="33" items="1kHz&#10;White" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="75a724c5faf4e2b1" memberName="toggleButton"
              virtualName="" explicitFocusOrder="0" pos="200 16 86 24" buttonText="play"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="new component" id="96544753efa6c8c1" memberName="component"
                    virtualName="" explicitFocusOrder="0" pos="56 56 64M 144M" class="ListBox"
                    params=""/>
  <LABEL name="new label" id="fa4fe90b2c777bd9" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 16 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Tone:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="new slider" id="ee7d514938f90c29" memberName="slider" virtualName="VolumeSlider"
                    explicitFocusOrder="0" pos="8 56 40 296" class="Component" params=""/>
  <TEXTBUTTON name="new button" id="710c460e04cff859" memberName="textButton"
              virtualName="" explicitFocusOrder="0" pos="56 320 112 24" posRelativeW="96544753efa6c8c1"
              buttonText="select all" connectedEdges="2" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="45ab63c9bc8a398c" memberName="textButton2"
              virtualName="" explicitFocusOrder="0" pos="168 320 118 24" posRelativeW="96544753efa6c8c1"
              buttonText="select none" connectedEdges="1" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="new label" id="863c5289d7b6c1d0" memberName="dbLabel" virtualName=""
         explicitFocusOrder="0" pos="8 360 280 24" edTextCol="ff000000"
         edBkgCol="0" labelText="0 dB" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//[EndFile] You can add extra defines here...
//[/EndFile]
