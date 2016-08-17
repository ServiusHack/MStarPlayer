#include "../JuceLibraryCode/JuceHeader.h"

#include <algorithm>

#include "ChannelMappingDialog.h"

// This is a custom component containing a combo box, which we're going to put inside
// our table's "output channel" column.
class OutputChannelColumnCustomComponent : public ComboBox
{
public:
    OutputChannelColumnCustomComponent()
        : m_row(-1)
    {
        setBoundsInset(BorderSize<int>(2));
    }

    void setRow(const int newRow)
    {
        m_row = newRow;
    }

    int getRow() const
    {
        return m_row;
    }

private:
    int m_row;
};

ChannelMapping::ChannelMapping(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings, std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& callback)
    : m_outputChannelNames(outputChannelNames)
    , m_soloBusSettings(soloBusSettings)
    , m_mapping(mapping)
    , m_callback(callback)
{
}

int ChannelMapping::getNumRows()
{
    return m_mapping.size();
}

void ChannelMapping::paintRowBackground(Graphics& /*g*/, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

void ChannelMapping::paintCell(Graphics& g,
                               int rowNumber,
                               int columnId,
                               int width,
                               int height,
                               bool /*rowIsSelected*/)
{
    g.setColour(Colours::black);

    if (columnId == 1)
    {
        String text = String(rowNumber + 1);
        switch (m_mapping[rowNumber].first)
        {
        case 'm':
            text += TRANS(" (mono)");
            break;
        case 'l':
            text += TRANS(" (left)");
            break;
        case 'r':
            text += TRANS(" (right)");
            break;
        }
        g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }

    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

Component* ChannelMapping::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if (columnId == 2)
    {
        // If it's the output channel column, we'll return our custom component..

        OutputChannelColumnCustomComponent* comboBox = static_cast<OutputChannelColumnCustomComponent*>(existingComponentToUpdate);

        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (comboBox == nullptr)
        {
            comboBox = new OutputChannelColumnCustomComponent();
            comboBox->addListener(this);
            comboBox->setWantsKeyboardFocus(false);
        }

        comboBox->clear();
        comboBox->addItemList(m_outputChannelNames->getAllDeviceOutputChannelNames(), 1);

        int leftSoloChannel = m_soloBusSettings.getChannel(SoloBusChannel::Left);
        int rightSoloChannel = m_soloBusSettings.getChannel(SoloBusChannel::Right);

        if (leftSoloChannel != -1)
            comboBox->setItemEnabled(1 + leftSoloChannel, false);

        if (rightSoloChannel != -1)
            comboBox->setItemEnabled(1 + rightSoloChannel, false);

        comboBox->setRow(rowNumber);
        comboBox->setSelectedId(getOutputChannel(rowNumber), dontSendNotification);

        return comboBox;
    }
    else
    {
        // for any other column, just return 0, as we'll be painting these columns directly.

        jassert(existingComponentToUpdate == 0);
        return 0;
    }
}

void ChannelMapping::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    OutputChannelColumnCustomComponent* outputChannelColumn = static_cast<OutputChannelColumnCustomComponent*>(comboBoxThatHasChanged);
    setChannelMapping(outputChannelColumn->getRow(), outputChannelColumn->getSelectedId());
}

int ChannelMapping::getOutputChannel(int row)
{
    return m_mapping[row].second + 1;
}

void ChannelMapping::setChannelMapping(int row, int outputChannel)
{
    m_callback(row, outputChannel - 1);
}

ChannelMappingWindow::ChannelMappingWindow(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings, std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& changeCallback, const CloseCallback& closeCallback)
    : DialogWindow(TRANS("Configure Channels"), Colours::lightgrey, true, false)
    , m_closeCallback(closeCallback)
{
    m_component = new ChannelMappingComponent(outputChannelNames, soloBusSettings, mapping, changeCallback, closeCallback);
    setContentOwned(m_component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
}

void ChannelMappingWindow::closeButtonPressed()
{
    m_closeCallback();
}

void ChannelMappingWindow::setMapping(const std::vector<std::pair<char, int>>& mapping)
{
    m_component->setMapping(mapping);
}

ChannelMappingComponent::ChannelMappingComponent(OutputChannelNames* outputChannelNames, SoloBusSettings& soloBusSettings, std::vector<std::pair<char, int>> mapping, const ChangeMappingCallback& changeCallback, const CloseCallback& closeCallback)
    : m_outputChannelNames(outputChannelNames)
    , m_soloBusSettings(soloBusSettings)
    , m_changeCallback(changeCallback)
    , m_closeCallback(closeCallback)
    , m_tableListBox(new TableListBox())
    , m_channelMapping(new ChannelMapping(m_outputChannelNames, soloBusSettings, mapping, m_changeCallback))
{
    addAndMakeVisible(m_tableListBox);

    m_tableListBox->setModel(m_channelMapping);

    // m_tableListBox the table component a border
    m_tableListBox->setColour(ListBox::outlineColourId, Colours::grey);
    m_tableListBox->setOutlineThickness(1);

    // set the table header columns
    m_tableListBox->getHeader().addColumn(TRANS("Player Channel"), 1, 120, 50, 400, TableHeaderComponent::defaultFlags);
    m_tableListBox->getHeader().addColumn(TRANS("Output Channel"), 2, 220, 50, 400, TableHeaderComponent::defaultFlags);

    addAndMakeVisible(m_closeButton = new TextButton("close"));
    m_closeButton->setButtonText(TRANS("Close"));
    m_closeButton->addListener(this);
    m_closeButton->setWantsKeyboardFocus(false);

    setSize(400, 400);
}

void ChannelMappingComponent::setMapping(const std::vector<std::pair<char, int>>& mapping)
{
    m_channelMapping = new ChannelMapping(m_outputChannelNames, m_soloBusSettings, mapping, m_changeCallback);
    m_tableListBox->setModel(m_channelMapping);
}

void ChannelMappingComponent::buttonClicked(Button* /*buttonThatWasClicked*/)
{
    m_closeCallback();
}

void ChannelMappingComponent::resized()
{
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;
    const static int padding = 10;

    m_tableListBox->setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
    m_closeButton->setBounds(
        (getWidth() - buttonWidth) / 2,
        getHeight() - buttonHeight - padding,
        buttonWidth,
        buttonHeight);
}