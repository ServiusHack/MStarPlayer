#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OutputChannelNames.h"
#include "SoloBusSettings.h"

class SoloBusComponent
    : public Component
    , public ComboBoxListener
{
public:
    SoloBusComponent(OutputChannelNames& outputChannelNames, SoloBusSettings& settings);

    // Component
public:
    void paint(Graphics& g) override;
    void resized() override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    SoloBusSettings& soloBusSettings;

    Label label;
    Label label2;
    ComboBox comboBox;
    ComboBox comboBox2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusComponent)
};
