#pragma once

#include "OutputChannelNames.h"
#include "SoloBusComponent.h"

/**
    Window wrapper for the AudioDeviceSelectorComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow
    : public juce::DialogWindow
    , public juce::Button::Listener
{
public:
    AudioConfigurationWindow(juce::AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames,
        SoloBusSettings& soloBusSettings);

    // DialogWindow
    virtual void closeButtonPressed() override;

    // Button::Listener
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationWindow)
};

/**
        Table model for the channel names.
*/
class ChannelNames
    : public juce::TableListBoxModel
    , public juce::TextEditor::Listener
    , public juce::ComboBox::Listener
{
public:
    ChannelNames(OutputChannelNames& outputChannelName);

    std::function<void()> updateCallback;

    // TableListBoxModel
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(
        juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    virtual juce::Component* refreshComponentForCell(
        int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(juce::TextEditor& textEditor) override;

    // ComboBox::Listener
    virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    OutputChannelNames& m_outputChannelName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelNames)
};

/**
        The actual component containing the tabs.
*/
class AudioConfigurationComponent
    : public juce::Component
    , public juce::ChangeListener
{
public:
    AudioConfigurationComponent(AudioConfigurationWindow* parent, juce::AudioDeviceManager& audioDeviceManager,
        OutputChannelNames& outputChannelNames, SoloBusSettings& soloBusSettings);
    ~AudioConfigurationComponent();

    // Component
    virtual void resized() override;

    // ChangeListener
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    std::unique_ptr<juce::TabbedComponent> m_tabbedComponent;
    std::unique_ptr<juce::TextButton> m_closeButton;
    std::unique_ptr<ChannelNames> m_channelNames;
    std::unique_ptr<juce::TableListBox> m_tableListBox;
    OutputChannelNames& m_outputChannelName;
    std::unique_ptr<SoloBusComponent> m_soloBusComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationComponent)
};
