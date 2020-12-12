#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class CDNamesComboBox : public juce::ComboBox
{
public:
    void selectDrive(juce::String title);

private:
    void showPopup() override;

    void setItems();
};
