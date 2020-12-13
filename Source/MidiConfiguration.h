#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "MTCSender.h"

class MidiConfigurationWindow
    : public juce::DialogWindow
    , public juce::Button::Listener
{
public:
    MidiConfigurationWindow(MTCSender& mtcSender);

    // DialogWindow
    virtual void closeButtonPressed() override;

    // Button::Listener
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationWindow)
};

// This is built after ChannelSelectorListBox of AudioDeviceSelectorComponent
class MidiDeviceSelectorListBox
    : public juce::ListBox
    , private juce::ListBoxModel
{
public:
    MidiDeviceSelectorListBox(MTCSender& mtcSender);
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;
    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override;
    void returnKeyPressed(int row) override;
    void flipEnablement(int row);
    int getTickX();

private:
    juce::Array<juce::MidiDeviceInfo> devices;
    juce::SparseSet<int> indexes;
    MTCSender& mtcSender;
};

class MidiConfigurationComponent : public juce::Component
{
public:
    MidiConfigurationComponent(MidiConfigurationWindow* parent, MTCSender& mtcSender);
    ~MidiConfigurationComponent();

    // Component
    virtual void resized() override;

private:
    MidiDeviceSelectorListBox deviceSelector;
    juce::TextButton m_closeButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiConfigurationComponent)
};
