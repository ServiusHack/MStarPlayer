/*
  ==============================================================================

    MidiConfiguration.cpp
    Created: 15 Aug 2017 9:44:04pm
    Author:  User

  ==============================================================================
*/

#include "MidiConfiguration.h"
#include "../JuceLibraryCode/JuceHeader.h"

MidiConfigurationWindow::MidiConfigurationWindow(MTCSender& mtcSender)
    : DialogWindow(TRANS("Configure MIDI"), Colours::lightgrey, true, true)
{
    setContentOwned(new MidiConfigurationComponent(this, mtcSender), true);
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

MidiDeviceSelectorListBox::MidiDeviceSelectorListBox(MTCSender& mtcSender)
    : devices(MidiOutput::getAvailableDevices())
    , mtcSender(mtcSender)
{
    setModel(this);

    for (const MidiDeviceInfo& info : mtcSender.getDevices())
    {
        int index = devices.indexOf(info);
        indexes.addRange(Range(index, index + 1));
    }
}

int MidiDeviceSelectorListBox::getNumRows()
{
    return devices.size();
}

void MidiDeviceSelectorListBox::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    auto enabled = indexes.contains(rowNumber);

    auto x = getTickX();
    auto tickW = (float)height * 0.75f;
    getLookAndFeel().drawTickBox(
        g, *this, (float)x - tickW, ((float)height - tickW) * 0.5f, tickW, tickW, enabled, true, true, false);
    g.setFont((float)height * 0.6f);
    g.setColour(findColour(ListBox::textColourId, true).withMultipliedAlpha(enabled ? 1.0f : 0.6f));
    g.drawText(devices[rowNumber].name, x + 5, 0, width - x - 5, height, Justification::centredLeft, true);
}

void MidiDeviceSelectorListBox::listBoxItemClicked(int row, const MouseEvent& e)
{
    selectRow(row);

    if (e.x < getTickX())
        flipEnablement(row);
}

void MidiDeviceSelectorListBox::listBoxItemDoubleClicked(int row, const MouseEvent&)
{
    flipEnablement(row);
}

void MidiDeviceSelectorListBox::returnKeyPressed(int row)
{
    flipEnablement(row);
}

void MidiDeviceSelectorListBox::flipEnablement(int row)
{
    if (indexes.contains(row))
        indexes.removeRange(Range(row, row + 1));
    else
        indexes.addRange(Range(row, row + 1));

    Array<MidiDeviceInfo> deviceInfos;
    for (int i = 0; i < indexes.size(); ++i)
    {
        deviceInfos.add(devices[indexes[i]]);
    }
    mtcSender.setDevices(deviceInfos);

    repaint();
}

int MidiDeviceSelectorListBox::getTickX()
{
    return getRowHeight();
}

MidiConfigurationComponent::MidiConfigurationComponent(MidiConfigurationWindow* parent, MTCSender& mtcSender)
    : deviceSelector(mtcSender)
    , m_closeButton(std::make_unique<TextButton>("close"))
{
    addAndMakeVisible(m_closeButton.get());
    m_closeButton->setButtonText(TRANS("Close"));
    m_closeButton->addListener(parent);
    m_closeButton->setWantsKeyboardFocus(false);

    addAndMakeVisible(&deviceSelector);

    setSize(500, 400);
}

void MidiConfigurationComponent::resized()
{
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;
    const static int padding = 10;

    deviceSelector.setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
    m_closeButton->setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - buttonHeight - padding, buttonWidth, buttonHeight);
}

MidiConfigurationComponent::~MidiConfigurationComponent() {}
