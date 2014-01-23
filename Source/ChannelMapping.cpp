/*
  ==============================================================================

    ChannelMapping.cpp
    Created: 25 Oct 2013 8:00:09pm
    Author:  Severin Leonhardt

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "ChannelMapping.h"



//==============================================================================
// This is a custom component containing a combo box, which we're going to put inside
// our table's "output channel" column.
class OutputChannelColumnCustomComponent    : public Component,
                                        public ComboBoxListener
{
public:
    OutputChannelColumnCustomComponent (ChannelMapping& owner_, int outputChannels)
        : owner (owner_)
    {
        comboBox.setEditableText(true);

        // just put a combo box inside this component
        addAndMakeVisible (&comboBox);

        for (int i = 1; i <= outputChannels; i++)
            comboBox.addItem (String(i), i);

        // when the combo is changed, we'll get a callback.
        comboBox.addListener (this);
        comboBox.setWantsKeyboardFocus (false);
    }

    void resized()
    {
        comboBox.setBoundsInset (BorderSize<int> (2));
    }

    // Our demo code will call this when we may need to update our contents
    void setRowAndColumn (const int newRow, const int newColumn)
    {
        row = newRow;
        columnId = newColumn;
        comboBox.setSelectedId (owner.getOutputChannel (row), dontSendNotification);
    }

    void comboBoxChanged (ComboBox* /*comboBoxThatHasChanged*/)
    {
        owner.setChannelMapping (row, comboBox.getSelectedId());
    }

private:
    ChannelMapping& owner;
    ComboBox comboBox;
    int row, columnId;
};

//==============================================================================
ChannelMapping::ChannelMapping(int outputChannels_, ChannelRemappingAudioSource& audioSource_, int channels_) :
outputChannels(outputChannels_), audioSource(audioSource_), font (14.0f), channels(channels_)
{
        addAndMakeVisible (&table);
        table.setModel (this);

        // give the table component a border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (1);

        // set the table header columns
        table.getHeader().addColumn ("File Channel", 1, 100, 50, 400, TableHeaderComponent::defaultFlags);
        table.getHeader().addColumn ("Output Channel", 2, 100, 50, 400, TableHeaderComponent::defaultFlags);
        
}

void ChannelMapping::resized()
{
        table.setBoundsInset (BorderSize<int> (8));
}

int ChannelMapping::getNumRows()
{
    return channels;
}

void ChannelMapping::paintRowBackground (Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (Colours::lightblue);
}

void ChannelMapping::paintCell (Graphics& g,
                int rowNumber,
                int columnId,
                int width, int height,
                bool /*rowIsSelected*/)
{
    g.setColour (Colours::black);
    g.setFont (font);

    if (columnId == 1) {
        const String text = String(rowNumber + 1);
        g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour (Colours::black.withAlpha (0.2f));
    g.fillRect (width - 1, 0, 1, height);
}

Component* ChannelMapping::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if (columnId == 2) // If it's the output channel column, we'll return our custom component..
    {
        OutputChannelColumnCustomComponent* comboBox = (OutputChannelColumnCustomComponent*) existingComponentToUpdate;

        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (comboBox == 0) {
            comboBox = new OutputChannelColumnCustomComponent (*this, outputChannels);
        }

        comboBox->setRowAndColumn (rowNumber, columnId);

        return comboBox;
    }
    else
    {
        // for any other column, just return 0, as we'll be painting these columns directly.

        jassert (existingComponentToUpdate == 0);
        return 0;
    }
}

int ChannelMapping::getOutputChannel(int row)
{
    return audioSource.getRemappedOutputChannel(row) + 1;
}

void ChannelMapping::setChannelMapping(int row, int outputChannel)
{
    audioSource.setOutputChannelMapping(row, outputChannel - 1);
}


//==============================================================================
ChannelMappingWindow::ChannelMappingWindow(int outputChannels, ChannelRemappingAudioSource& audioSource_, int channels)  : DialogWindow ("Configure Channels",
                                        Colours::lightgrey,
                                        true,
                                        true)
{
    component = new ChannelMapping(outputChannels, audioSource_, channels);
    component->setSize(400,400);
    
    setContentOwned (component, true);
    centreWithSize (getWidth(), getHeight());
    setVisible (true);
    setResizable(true, true);
}

void ChannelMappingWindow::closeButtonPressed()
{
    delete this;
}