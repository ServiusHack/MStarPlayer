/*
  ==============================================================================

    MidiConfiguration.cpp
    Created: 15 Aug 2017 9:44:04pm
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiConfiguration.h"

MidiConfigurationWindow::MidiConfigurationWindow(MTCSender& mtcSender)
    : DialogWindow(TRANS("Configure MIDI"), Colours::lightgrey, true, true)
    , m_mtcSender(mtcSender)
{
    setContentOwned(new MidiConfigurationComponent(this, m_mtcSender.getDevice()), true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(true, true);
}

void MidiConfigurationWindow::closeButtonPressed()
{
    setVisible(false);
}

void MidiConfigurationWindow::buttonClicked(Button*)
{
    closeButtonPressed();
}

void MidiConfigurationWindow::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    m_mtcSender.setDevice(comboBoxThatHasChanged->getSelectedId() - 2);
}

MidiConfigurationComponent::MidiConfigurationComponent(MidiConfigurationWindow* parent, int selectedDevice)
    : m_outputDevices(std::make_unique<ComboBox>())
    , m_closeButton(std::make_unique<TextButton>("close"))
{
    addAndMakeVisible(m_closeButton.get());
    m_closeButton->setButtonText(TRANS("Close"));
    m_closeButton->addListener(parent);
    m_closeButton->setWantsKeyboardFocus(false);

    m_outputDevices->addListener(parent);
    m_outputDevices->addItem(TRANS("no MIDI output"),1);
    StringArray deviceNames = MidiOutput::getDevices();
    for (int i = 0; i < deviceNames.size(); ++i)
    {
        m_outputDevices->addItem(deviceNames[i],i+2);
    }

    m_outputDevices->setSelectedId(selectedDevice + 2, juce::dontSendNotification);

    addAndMakeVisible(m_outputDevices.get());

    setSize(500, 400);
}

void MidiConfigurationComponent::resized()
{
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;
    const static int padding = 10;

    m_outputDevices->setBounds(padding, padding, getWidth() - 2 * padding, buttonHeight);
    m_closeButton->setBounds(
        (getWidth() - buttonWidth) / 2,
        getHeight() - buttonHeight - padding,
        buttonWidth,
        buttonHeight);
}

MidiConfigurationComponent::~MidiConfigurationComponent()
{
}