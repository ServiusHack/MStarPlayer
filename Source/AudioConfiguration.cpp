#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConfiguration.h"

class ChannelNameTextEditor
    : public TextEditor
{
public:
    ChannelNameTextEditor()
        : m_row(0)
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

class ChannelNameComboBox
    : public ComboBox
{
public:
    ChannelNameComboBox()
        : m_row(0)
    {
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

AudioConfigurationWindow::AudioConfigurationWindow(AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelNames, SoloBusSettings& soloBusSettings)
    : DialogWindow(TRANS("Configure Audio"), Colours::lightgrey, true, true)
{
    setContentOwned(new AudioConfigurationComponent(this, audioDeviceManager, outputChannelNames, soloBusSettings), true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
    setVisible(false);
}

void AudioConfigurationWindow::buttonClicked(Button*)
{
    closeButtonPressed();
}

ChannelNames::ChannelNames(OutputChannelNames& outputChannelName)
    : m_outputChannelName(outputChannelName)
{
}

int ChannelNames::getNumRows()
{
    return m_outputChannelName.getNumberOfChannels();
}

void ChannelNames::paintRowBackground(Graphics& /*g*/, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

void ChannelNames::paintCell(Graphics& g,
                             int rowNumber,
                             int columnId,
                             int width,
                             int height,
                             bool /*rowIsSelected*/)
{
    g.setColour(Colours::black);

    if (columnId == 1)
        g.drawText(m_outputChannelName.getDeviceOutputChannelName(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);

    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

Component* ChannelNames::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        ChannelNameComboBox* editor = static_cast<ChannelNameComboBox*>(existingComponentToUpdate);

        if (editor == nullptr)
        {
            editor = new ChannelNameComboBox();
            editor->addListener(this);
        }

        editor->clear();
        editor->addItem("mono", 1);
        if (rowNumber < m_outputChannelName.getNumberOfChannels() - 1 
            && m_outputChannelName.GetChannelPairing(rowNumber) != PairingMode::Right
            && m_outputChannelName.GetChannelPairing(rowNumber +1) != PairingMode::Left)
            editor->addItem("left of below", 2);
        if (rowNumber > 0 
            && m_outputChannelName.GetChannelPairing(rowNumber) != PairingMode::Left
            && m_outputChannelName.GetChannelPairing(rowNumber - 1) != PairingMode::Right)
            editor->addItem("right of above", 3);

        editor->setSelectedId(static_cast<int>(m_outputChannelName.GetChannelPairing(rowNumber)) + 1, juce::dontSendNotification);
        editor->setRow(rowNumber);

        return editor;
    }
    else if (columnId == 2)
    {
        ChannelNameTextEditor* editor = static_cast<ChannelNameTextEditor*>(existingComponentToUpdate);

        if (editor == nullptr)
        {
            editor = new ChannelNameTextEditor();
            editor->addListener(this);
        }

        editor->setRow(rowNumber);
        editor->setText(m_outputChannelName.getInternalOutputChannelName(rowNumber), dontSendNotification);

        return editor;
    }
    else
    {
        jassert(existingComponentToUpdate == 0);
        return 0;
    }
}

void ChannelNames::textEditorTextChanged(TextEditor& textEditor)
{
    ChannelNameTextEditor& channelNameTextEditor = static_cast<ChannelNameTextEditor&>(textEditor);
    m_outputChannelName.setInternalOutputChannelName(channelNameTextEditor.getRow(), channelNameTextEditor.getText());
}

void ChannelNames::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    ChannelNameComboBox* channelNameComboBox = static_cast<ChannelNameComboBox*>(comboBoxThatHasChanged);
    bool changed = m_outputChannelName.SetChannelPairing(channelNameComboBox->getRow(), static_cast<PairingMode>(channelNameComboBox->getSelectedId() - 1));
    if (changed)
        updateCallback();
}

AudioConfigurationComponent::AudioConfigurationComponent(AudioConfigurationWindow* parent, AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelName, SoloBusSettings& soloBusSettings)
    : m_channelNames(new ChannelNames(outputChannelName))
    , m_outputChannelName(outputChannelName)
    , m_soloBusComponent(new SoloBusComponent(outputChannelName, soloBusSettings))
{
    m_outputChannelName.addChangeListener(this);

    addAndMakeVisible(m_tabbedComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop));

    AudioDeviceSelectorComponent* selector = new AudioDeviceSelectorComponent(audioDeviceManager, 0, 0, 1, 64, false, false, false, false);
    m_tabbedComponent->addTab(TRANS("Audio Device"), Colour(0xffffffff), selector, true);

    m_tableListBox = new TableListBox();
    m_tabbedComponent->addTab(TRANS("Channel Names"), Colour(0xffffffff), m_tableListBox, true);
    m_tableListBox->setColour(ListBox::outlineColourId, Colours::grey);
    m_tableListBox->setOutlineThickness(1);
    m_tableListBox->setModel(m_channelNames);
    m_channelNames->updateCallback = std::bind(&TableListBox::updateContent, m_tableListBox.get());

    // set the table header columns
    m_tableListBox->getHeader().addColumn(TRANS("Device Channel"), 1, 170, 50, 400, TableHeaderComponent::defaultFlags);
    m_tableListBox->getHeader().addColumn(TRANS("Channel Name"), 2, 170, 50, 400, TableHeaderComponent::defaultFlags);
    m_tableListBox->getHeader().addColumn(TRANS("Mode"), 3, 120, 50, 400, TableHeaderComponent::defaultFlags);

    m_tabbedComponent->addTab(TRANS("Solo Bus"), Colour(0xffffffff), m_soloBusComponent, true);

    addAndMakeVisible(m_closeButton = new TextButton("close"));
    m_closeButton->setButtonText(TRANS("Close"));
    m_closeButton->addListener(parent);
    m_closeButton->setWantsKeyboardFocus(false);

    setSize(500, 400);

    changeListenerCallback(&outputChannelName);
}

AudioConfigurationComponent::~AudioConfigurationComponent()
{
    m_outputChannelName.removeChangeListener(this);
}

void AudioConfigurationComponent::resized()
{
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;
    const static int padding = 10;

    m_tabbedComponent->setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
    m_closeButton->setBounds(
        (getWidth() - buttonWidth) / 2,
        getHeight() - buttonHeight - padding,
        buttonWidth,
        buttonHeight);
}

void AudioConfigurationComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    m_tableListBox->updateContent();
}