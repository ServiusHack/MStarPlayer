#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "OutputChannelNames.h"
#include "SoloBusSettings.h"

class SoloBusComponent
    : public juce::Component
    , public juce::ComboBox::Listener
{
public:
    SoloBusComponent(OutputChannelNames& outputChannelNames, SoloBusSettings& settings);

    // Component
public:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    SoloBusSettings& soloBusSettings;

    juce::Label label;
    juce::Label label2;
    juce::ComboBox comboBox;
    juce::ComboBox comboBox2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusComponent)
};
