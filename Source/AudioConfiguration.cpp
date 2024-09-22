#include "AudioConfiguration.h"

#include "juce_audio_utils/juce_audio_utils.h"

#include "AudioDeviceComponent.h"

class ChannelNameTextEditor : public juce::TextEditor
{
public:
    ChannelNameTextEditor()
        : m_row(0)
    {
        setBoundsInset(juce::BorderSize<int>(2));
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

class ChannelNameComboBox : public juce::ComboBox
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

AudioConfigurationWindow::AudioConfigurationWindow(juce::AudioDeviceManager& audioDeviceManager,
    OutputChannelNames& outputChannelNames, SoloBusSettings& soloBusSettings)
    : juce::DialogWindow(TRANS("Configure Audio"), juce::Colours::lightgrey, true, true)
{
    setContentOwned(
        new AudioConfigurationComponent(this, audioDeviceManager, outputChannelNames, soloBusSettings), true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
}

void AudioConfigurationWindow::closeButtonPressed()
{
    setVisible(false);
}

void AudioConfigurationWindow::buttonClicked(juce::Button*)
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

void ChannelNames::paintRowBackground(
    juce::Graphics& /*g*/, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/)
{
}

void ChannelNames::paintCell(
    juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
    g.setColour(juce::Colours::black);

    if (columnId == 1)
        g.drawText(m_outputChannelName.getDeviceOutputChannelName(rowNumber),
            2,
            0,
            width - 4,
            height,
            juce::Justification::centredLeft,
            true);

    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

juce::Component* ChannelNames::refreshComponentForCell(
    int rowNumber, int columnId, bool /*isRowSelected*/, juce::Component* existingComponentToUpdate)
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
            && m_outputChannelName.GetChannelPairing(rowNumber + 1) != PairingMode::Left)
            editor->addItem("left of below", 2);
        if (rowNumber > 0 && m_outputChannelName.GetChannelPairing(rowNumber) != PairingMode::Left
            && m_outputChannelName.GetChannelPairing(rowNumber - 1) != PairingMode::Right)
            editor->addItem("right of above", 3);

        editor->setSelectedId(
            static_cast<int>(m_outputChannelName.GetChannelPairing(rowNumber)) + 1, juce::dontSendNotification);
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
        editor->setText(m_outputChannelName.getInternalOutputChannelName(rowNumber), juce::dontSendNotification);

        return editor;
    }
    else
    {
        jassert(existingComponentToUpdate == 0);
        return 0;
    }
}

void ChannelNames::textEditorTextChanged(juce::TextEditor& textEditor)
{
    const ChannelNameTextEditor& channelNameTextEditor = static_cast<ChannelNameTextEditor&>(textEditor);
    m_outputChannelName.setInternalOutputChannelName(channelNameTextEditor.getRow(), channelNameTextEditor.getText());
}

void ChannelNames::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    const ChannelNameComboBox* channelNameComboBox = static_cast<ChannelNameComboBox*>(comboBoxThatHasChanged);
    const bool changed = m_outputChannelName.SetChannelPairing(
        channelNameComboBox->getRow(), static_cast<PairingMode>(channelNameComboBox->getSelectedId() - 1));
    if (changed)
        updateCallback();
}

AudioConfigurationComponent::AudioConfigurationComponent(AudioConfigurationWindow* parent,
    juce::AudioDeviceManager& audioDeviceManager, OutputChannelNames& outputChannelName,
    SoloBusSettings& soloBusSettings)
    : m_tabbedComponent(juce::TabbedButtonBar::TabsAtTop)
    , m_closeButton("close")
    , m_channelNames(outputChannelName)
    , m_outputChannelName(outputChannelName)
    , m_soloBusComponent(outputChannelName, soloBusSettings)
{
    m_outputChannelName.addChangeListener(this);

    addAndMakeVisible(&m_tabbedComponent);

    AudioDeviceComponent* audioDevice = new AudioDeviceComponent(audioDeviceManager);
    m_tabbedComponent.addTab(TRANS("Audio Device"), juce::Colour(0xffffffff), audioDevice, true);

    m_tabbedComponent.addTab(TRANS("Channel Names"), juce::Colour(0xffffffff), &m_tableListBox, false);
    m_tableListBox.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
    m_tableListBox.setOutlineThickness(1);
    m_tableListBox.setModel(&m_channelNames);
    m_channelNames.updateCallback = std::bind(&juce::TableListBox::updateContent, &m_tableListBox);

    // set the table header columns
    m_tableListBox.getHeader().addColumn(
        TRANS("Device Channel"), 1, 170, 50, 400, juce::TableHeaderComponent::defaultFlags);
    m_tableListBox.getHeader().addColumn(
        TRANS("Channel Name"), 2, 170, 50, 400, juce::TableHeaderComponent::defaultFlags);
    m_tableListBox.getHeader().addColumn(TRANS("Mode"), 3, 120, 50, 400, juce::TableHeaderComponent::defaultFlags);

    m_tabbedComponent.addTab(TRANS("Solo Bus"), juce::Colour(0xffffffff), &m_soloBusComponent, true);

    addAndMakeVisible(&m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(parent);
    m_closeButton.setWantsKeyboardFocus(false);

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

    m_tabbedComponent.setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - buttonHeight - padding, buttonWidth, buttonHeight);
}

void AudioConfigurationComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    m_tableListBox.updateContent();
}
