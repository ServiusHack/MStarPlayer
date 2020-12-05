#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MTCSender.h"

class MidiConfigurationWindow
    : public DialogWindow
    , public Button::Listener
{
public:
    MidiConfigurationWindow(MTCSender& mtcSender);

    // DialogWindow
    virtual void closeButtonPressed() override;

    // Button::Listener
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationWindow)
};

// This is built after ChannelSelectorListBox of AudioDeviceSelectorComponent
class MidiDeviceSelectorListBox
    : public ListBox
    , private ListBoxModel
{
public:
    MidiDeviceSelectorListBox(MTCSender& mtcSender);
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const MouseEvent& e) override;
    void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
    void returnKeyPressed(int row) override;
    void flipEnablement(int row);
    int getTickX();

private:
    Array<MidiDeviceInfo> devices;
    SparseSet<int> indexes;
    MTCSender& mtcSender;
};

class MidiConfigurationComponent : public Component
{
public:
    MidiConfigurationComponent(MidiConfigurationWindow* parent, MTCSender& mtcSender);
    ~MidiConfigurationComponent();

    // Component
    virtual void resized() override;

private:
    MidiDeviceSelectorListBox deviceSelector;
    std::unique_ptr<TextButton> m_closeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationComponent)
};
