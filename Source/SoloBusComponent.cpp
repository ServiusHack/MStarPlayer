#include "SoloBusComponent.h"

SoloBusComponent::SoloBusComponent(OutputChannelNames& outputChannelNames, SoloBusSettings& settings)
    : soloBusSettings(settings)
    , label({}, TRANS("left channel / mono:"))
    , label2({}, TRANS("right channel"))
{
    const juce::FontOptions fontOptions(15.00f, juce::Font::plain);

    addAndMakeVisible(label);
    label.setFont(juce::Font(fontOptions));
    label.setJustificationType(juce::Justification::centredRight);
    label.setEditable(false, false, false);
    label.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    addAndMakeVisible(label2);
    label2.setFont(juce::Font(fontOptions));
    label2.setJustificationType(juce::Justification::centredRight);
    label2.setEditable(false, false, false);
    label2.setColour(juce::TextEditor::textColourId, juce::Colours::black);
    label2.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));

    addAndMakeVisible(comboBox);
    comboBox.setEditableText(false);
    comboBox.setJustificationType(juce::Justification::centredLeft);
    comboBox.setTextWhenNothingSelected(juce::String());
    comboBox.setTextWhenNoChoicesAvailable(TRANS("(no choices)"));

    addAndMakeVisible(comboBox2);
    comboBox2.setEditableText(false);
    comboBox2.setJustificationType(juce::Justification::centredLeft);
    comboBox2.setTextWhenNothingSelected(juce::String());
    comboBox2.setTextWhenNoChoicesAvailable(TRANS("(no choices)"));

    comboBox.addItem(TRANS("none"), 1);
    comboBox.addItemList(outputChannelNames.getAllDeviceOutputChannelNames(), 2);
    comboBox.setSelectedItemIndex(0);
    comboBox2.addItem(TRANS("none"), 1);
    comboBox2.addItemList(outputChannelNames.getAllDeviceOutputChannelNames(), 2);
    comboBox2.setSelectedItemIndex(0);

    comboBox.addListener(this);
    comboBox2.addListener(this);

    comboBox.setSelectedItemIndex(settings.getChannel(SoloBusChannel::Left) + 1, juce::dontSendNotification);
    comboBox2.setSelectedItemIndex(settings.getChannel(SoloBusChannel::Right) + 1, juce::dontSendNotification);
}

//==============================================================================
void SoloBusComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void SoloBusComponent::resized()
{
    label.setBounds(8, 8, 120, 24);
    label2.setBounds(8, 40, 120, 24);
    comboBox.setBounds(136, 8, 150, 24);
    comboBox2.setBounds(136, 40, 150, 24);
}

void SoloBusComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &comboBox)
    {
        soloBusSettings.setChannel(SoloBusChannel::Left, comboBoxThatHasChanged->getSelectedItemIndex() - 1);
    }
    else if (comboBoxThatHasChanged == &comboBox2)
    {
        soloBusSettings.setChannel(SoloBusChannel::Right, comboBoxThatHasChanged->getSelectedItemIndex() - 1);
    }
}
