#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class CDNamesComboBox : public ComboBox
{
public:
    void selectDrive(String title);

private:
    void showPopup() override;

    void setItems();
};