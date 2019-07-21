#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OutputChannelNames.h"
#include "SoloBusComponent.h"

/**
    Window wrapper for the AudioDeviceSelectorComponent.

    This allows the component to be shown in its own window.
*/
class AudioConfigurationWindow
    : public DialogWindow
    , public Button::Listener
{
public:
    AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames,
        SoloBusSettings& soloBusSettings);

    // DialogWindow
    virtual void closeButtonPressed() override;

    // Button::Listener
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationWindow)
};

/**
        Table model for the channel names.
*/
class ChannelNames
    : public TableListBoxModel
    , public TextEditor::Listener
    , public ComboBox::Listener
{
public:
    ChannelNames(OutputChannelNames& outputChannelName);

    std::function<void()> updateCallback;

    // TableListBoxModel
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    virtual Component* refreshComponentForCell(
        int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(TextEditor& textEditor) override;

    // ComboBox::Listener
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    OutputChannelNames& m_outputChannelName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelNames)
};

/**
        The actual component containing the tabs.
*/
class AudioConfigurationComponent
    : public Component
    , public ChangeListener
{
public:
    AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager,
        OutputChannelNames& outputChannelNames, SoloBusSettings& soloBusSettings);
    ~AudioConfigurationComponent();

    // Component
    virtual void resized() override;

    // ChangeListener
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    std::unique_ptr<TabbedComponent> m_tabbedComponent;
    std::unique_ptr<TextButton> m_closeButton;
    std::unique_ptr<ChannelNames> m_channelNames;
    std::unique_ptr<TableListBox> m_tableListBox;
    OutputChannelNames& m_outputChannelName;
    std::unique_ptr<SoloBusComponent> m_soloBusComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioConfigurationComponent)
};
