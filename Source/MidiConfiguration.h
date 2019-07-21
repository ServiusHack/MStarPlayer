#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MTCSender.h"

class MidiConfigurationWindow
    : public DialogWindow
    , public Button::Listener
    , public ComboBox::Listener
{
public:
    MidiConfigurationWindow(MTCSender& mtcSender);

    // DialogWindow
    virtual void closeButtonPressed() override;

    // Button::Listener
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

    // ComboBox::Listener
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    MTCSender& m_mtcSender;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationWindow)
};

class MidiConfigurationComponent : public Component
{
public:
    MidiConfigurationComponent(MidiConfigurationWindow* parent, int selectedDevice);
    ~MidiConfigurationComponent();

    // Component
    virtual void resized() override;

private:
    std::unique_ptr<ComboBox> m_outputDevices;
    std::unique_ptr<TextButton> m_closeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationComponent)
};
