/*
  ==============================================================================

    ChannelMapping.h
    Created: 25 Oct 2013 8:00:09pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#ifndef CHANNELMAPPING_H_INCLUDED
#define CHANNELMAPPING_H_INCLUDED

#include <vector>
#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"


typedef std::function<void(int, int)> ChangeMappingCallback;

//==============================================================================
/** Show a mapping from audio source channels to output channels.

    The component shows a table with a colum for the source channel and another
    column for the output channel. Each table row corresponds to a source
    channel. In each row the output channel can be changed by the user.

    Usually the output channel indices are zero-based. The exception is within
    this component, where they begin with 1. This is because 0 is a magic value
    for the combo box component.
*/
class ChannelMapping    : public Component,
                          public TableListBoxModel
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
	ChannelMapping(int outputChannels, std::vector<int> mapping, const ChangeMappingCallback callback);

    void resized();

    // TableListBoxModel overwrites
    int getNumRows();
    void paintRowBackground (Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected);
    void paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/);
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate);

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

private:
    TableListBox table;     // the table component itself
    Font font;
    
    int outputChannels;
	std::vector<int> m_mapping;
	const ChangeMappingCallback m_callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelMapping)
};

//==============================================================================
/**
    Wrapper window to show the ChannelMapping component.
*/
class ChannelMappingWindow    : public DialogWindow
{
public:
    /** Creates a window to view and edit the channel mapping.

        See ChannelMapping for parameter explanation.

        @see ChannelMapping
    */
	ChannelMappingWindow(int outputChannels, std::vector<int> mapping, const ChangeMappingCallback callback);

    /** Delete (and thus close) the window when requested by the user.
    */
    void closeButtonPressed();

private:
    ScopedPointer<ChannelMapping> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelMappingWindow)
};



#endif  // CHANNELMAPPING_H_INCLUDED
