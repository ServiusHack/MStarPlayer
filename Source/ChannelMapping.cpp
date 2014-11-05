#include "../JuceLibraryCode/JuceHeader.h"

#include <algorithm>

#include "ChannelMapping.h"



//==============================================================================
// This is a custom component containing a combo box, which we're going to put inside
// our table's "output channel" column.
class OutputChannelColumnCustomComponent
	: public Component
	, public ComboBoxListener
{
public:
    OutputChannelColumnCustomComponent(ChannelMapping& owner, int outputChannels)
		: m_owner(owner)
    {
		m_comboBox.setEditableText(true);

        // just put a combo box inside this component
		addAndMakeVisible(&m_comboBox);

        for (int i = 1; i <= outputChannels; i++)
			m_comboBox.addItem(String(i), i);

        // when the combo is changed, we'll get a callback.
		m_comboBox.addListener(this);
		m_comboBox.setWantsKeyboardFocus(false);
    }

    virtual void resized() override
    {
		m_comboBox.setBoundsInset(BorderSize<int>(2));
    }

    void setRowAndColumn(const int newRow, const int newColumn, int outputChannel)
    {
		m_row = newRow;
		m_columnId = newColumn;
		m_comboBox.setSelectedId(outputChannel, dontSendNotification);
    }

    virtual void comboBoxChanged(ComboBox* /*comboBoxThatHasChanged*/) override
    {
		m_owner.setChannelMapping(m_row, m_comboBox.getSelectedId());
    }

private:
    ChannelMapping& m_owner;
	ComboBox m_comboBox;
	int m_row, m_columnId;
};

//==============================================================================
ChannelMapping::ChannelMapping(int outputChannels, std::vector<int> mapping, const ChangeMappingCallback callback)
	: m_outputChannels(outputChannels)
	, m_mapping(mapping)
	, m_callback(callback)
{
}

int ChannelMapping::getNumRows()
{
    return m_mapping.size();
}

void ChannelMapping::paintRowBackground (Graphics& /*g*/, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

void ChannelMapping::paintCell (Graphics& g,
                int rowNumber,
                int columnId,
                int width, int height,
                bool /*rowIsSelected*/)
{
    g.setColour(Colours::black);

    if (columnId == 1) {
        const String text = String(rowNumber + 1);
        g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour(Colours::black.withAlpha (0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

Component* ChannelMapping::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if (columnId == 2) {
		// If it's the output channel column, we'll return our custom component..

        OutputChannelColumnCustomComponent* comboBox = static_cast<OutputChannelColumnCustomComponent*>(existingComponentToUpdate);

        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (comboBox == nullptr)
            comboBox = new OutputChannelColumnCustomComponent(*this, m_outputChannels);

		comboBox->setRowAndColumn(rowNumber, columnId, getOutputChannel(rowNumber));

        return comboBox;
    } else {
        // for any other column, just return 0, as we'll be painting these columns directly.

        jassert (existingComponentToUpdate == 0);
        return 0;
    }
}

int ChannelMapping::getOutputChannel(int row)
{
    return m_mapping[row] + 1;
}

void ChannelMapping::setChannelMapping(int row, int outputChannel)
{
	m_callback(row, outputChannel - 1);
}


//==============================================================================
ChannelMappingWindow::ChannelMappingWindow(int outputChannels, std::vector<int> mapping, const ChangeMappingCallback callback, const CloseCallback closeCallback)
	: DialogWindow("Configure Channels", Colours::lightgrey, true, false)
	, m_outputChannels(outputChannels)
	, m_changeCallback(callback)
	, m_closeCallback(closeCallback)
	, m_tableListBox(new TableListBox())
	, m_channelMapping(new ChannelMapping(m_outputChannels, mapping, m_changeCallback))
{
	m_tableListBox->setModel(m_channelMapping);

	// m_tableListBox the table component a border
	m_tableListBox->setColour(ListBox::outlineColourId, Colours::grey);
	m_tableListBox->setOutlineThickness(1);

	// set the table header columns
	m_tableListBox->getHeader().addColumn("Player Channel", 1, 100, 50, 400, TableHeaderComponent::defaultFlags);
	m_tableListBox->getHeader().addColumn("Output Channel", 2, 100, 50, 400, TableHeaderComponent::defaultFlags);

	setBoundsInset(BorderSize<int>(8));

	setContentOwned(m_tableListBox, false);
    centreWithSize(400, 400);
    setVisible(true);
    setResizable(true, true);
}

void ChannelMappingWindow::closeButtonPressed()
{
	m_closeCallback();
}

void ChannelMappingWindow::setMapping(std::vector<int> mapping)
{
	m_channelMapping = new ChannelMapping(m_outputChannels, mapping, m_changeCallback);
	m_tableListBox->setModel(m_channelMapping);
}