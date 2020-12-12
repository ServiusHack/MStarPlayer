#pragma once

#include <functional>
#include <vector>

#include "juce_gui_basics/juce_gui_basics.h"

#include "OutputChannelNames.h"
#include "SoloBusSettings.h"

using ChangeMappingCallback = std::function<void(int, int)>;
using CloseCallback = std::function<void()>;

/** Show a mapping from audio source channels to output channels.

    The component shows a table with a colum for the source channel and another
    column for the output channel. Each table row corresponds to a source
    channel. In each row the output channel can be changed by the user.

    Usually the output channel indices are zero-based. The exception is within
    this component, where they begin with 1. This is because 0 is a magic value
    for the combo box component.
*/
class ChannelMapping
    : public juce::TableListBoxModel
    , public juce::ComboBox::Listener
{
public:
    /** A component to view and edit the channel mapping.

        @param outputChannels     Number of output channels currently configured.
                                  This gives the maximum value the user can select as the output channel.

        @param audioSource        The ChannelRemappingAudioSource which implements the mapping.
                                  It is directly modified when the user changes something.

        @param channels           Number of channels the audio source has.
                                  This gives the number of rows to show to the user.
    */
    ChannelMapping(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
        std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& callback);

    /** Get the output channel for a table row.

        This is used by the combo box to preselect the correct output channel.

        @param row                row index (starting with 0)

        @returns                  output channel (starting with 1)
    */
    int getOutputChannel(int row);

    /** Set the output channel for a table row.

        This is used by the combo box to forward a change to the
        ChannelRemappingAudioSource.

        @param row               row index (starting with 0)

        @param outputChannel     newly selected output channel (starting with 1)
    */
    void setChannelMapping(int row, int outputChannel);

    // TableListBoxModel overrides
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(
        juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    virtual juce::Component* refreshComponentForCell(
        int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // ComboBox::Listener overrides
public:
    virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;
    std::vector<std::pair<char, int>> m_mapping;
    const ChangeMappingCallback m_callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelMapping)
};

/**
        The actual component containing controls to change the mapping from playback channels to output channels.
*/
class ChannelMappingComponent
    : public juce::Component
    , public juce::Button::Listener
{
public:
    ChannelMappingComponent(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
        std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& changeCallback,
        const CloseCallback& closeCallback);

    void setMapping(const std::vector<std::pair<char, int>>& mapping);

    // Component overrides
public:
    virtual void resized() override;

    // Button::Listener
public:
    void buttonClicked(juce::Button* buttonThatWasClicked);

private:
    OutputChannelNames* m_outputChannelNames;
    SoloBusSettings& m_soloBusSettings;
    const ChangeMappingCallback m_changeCallback;
    const CloseCallback m_closeCallback;
    juce::TableListBox m_tableListBox;
    std::unique_ptr<ChannelMapping> m_channelMapping;
    juce::TextButton m_closeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelMappingComponent)
};

/**
    Wrapper window to show the ChannelMapping component.
*/
class ChannelMappingWindow : public juce::DialogWindow
{
public:
    /** Creates a window to view and edit the channel mapping.

        See ChannelMapping for parameter explanation.

        @see ChannelMapping
    */
    ChannelMappingWindow(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings,
        std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& callback,
        const CloseCallback& closeCallback);

    void setMapping(const std::vector<std::pair<char, int>>& mapping);

    // DialogWindow
public:
    /** Delete (and thus close) the window when requested by the user.
     */
    virtual void closeButtonPressed() override;

private:
    const CloseCallback m_closeCallback;

    std::unique_ptr<ChannelMappingComponent> m_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelMappingWindow)
};
