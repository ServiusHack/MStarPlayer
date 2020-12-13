/*
  ==============================================================================

    MidiConfiguration.cpp
    Created: 15 Aug 2017 9:44:04pm
    Author:  User

  ==============================================================================
*/

#include "MidiConfiguration.h"

MidiConfigurationWindow::MidiConfigurationWindow(MTCSender& mtcSender)
    : juce::DialogWindow(TRANS("Configure MIDI"), juce::Colours::lightgrey, true, true)
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

void MidiConfigurationWindow::buttonClicked(juce::Button*)
{
    closeButtonPressed();
}

MidiDeviceSelectorListBox::MidiDeviceSelectorListBox(MTCSender& mtcSender)
    : devices(juce::MidiOutput::getAvailableDevices())
    , mtcSender(mtcSender)
{
    setModel(this);

    for (const juce::MidiDeviceInfo& info : mtcSender.getDevices())
    {
        int index = devices.indexOf(info);
        indexes.addRange(juce::Range(index, index + 1));
    }
}

int MidiDeviceSelectorListBox::getNumRows()
{
    return devices.size();
}

void MidiDeviceSelectorListBox::paintListBoxItem(
    int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    auto enabled = indexes.contains(rowNumber);

    auto x = getTickX();
    auto tickW = (float)height * 0.75f;
    getLookAndFeel().drawTickBox(
        g, *this, (float)x - tickW, ((float)height - tickW) * 0.5f, tickW, tickW, enabled, true, true, false);
    g.setFont((float)height * 0.6f);
    g.setColour(findColour(juce::ListBox::textColourId, true).withMultipliedAlpha(enabled ? 1.0f : 0.6f));
    g.drawText(devices[rowNumber].name, x + 5, 0, width - x - 5, height, juce::Justification::centredLeft, true);
}

void MidiDeviceSelectorListBox::listBoxItemClicked(int row, const juce::MouseEvent& e)
{
    selectRow(row);

    if (e.x < getTickX())
        flipEnablement(row);
}

void MidiDeviceSelectorListBox::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
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
        indexes.removeRange(juce::Range(row, row + 1));
    else
        indexes.addRange(juce::Range(row, row + 1));

    juce::Array<juce::MidiDeviceInfo> deviceInfos;
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
    , m_closeButton("close")
{
    addAndMakeVisible(&m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(parent);
    m_closeButton.setWantsKeyboardFocus(false);

    addAndMakeVisible(&deviceSelector);

    setSize(500, 400);
}

void MidiConfigurationComponent::resized()
{
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;
    const static int padding = 10;

    deviceSelector.setBounds(padding, padding, getWidth() - 2 * padding, getHeight() - buttonHeight - 3 * padding);
    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - buttonHeight - padding, buttonWidth, buttonHeight);
}

MidiConfigurationComponent::~MidiConfigurationComponent() {}
