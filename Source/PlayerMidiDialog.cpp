#include "PlayerMidiDialog.h"

PlayerMidiDialogWindow::PlayerMidiDialogWindow(bool mtcEnabled, MTCSender::Position mtcStartPosition,
    const MtcEnabledChangedCallback& mtcEnabledChangedCallback,
    const MtcOffsetChangedCallback& mtcOffsetChangedCallback, const CloseCallback& closeCallback)
    : juce::DialogWindow(TRANS("Configure MIDI for player"), juce::Colours::lightgrey, true, false)
    , m_closeCallback(closeCallback)
{
    PlayerMidiDialogComponent* component = new PlayerMidiDialogComponent(
        mtcEnabled, mtcStartPosition, mtcEnabledChangedCallback, mtcOffsetChangedCallback, closeCallback);
    setContentOwned(component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(false, false);
}

void PlayerMidiDialogWindow::closeButtonPressed()
{
    m_closeCallback();
}

bool PlayerMidiDialogWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        exitModalState(0);
        return true;
    }

    return false;
}

void PlayerMidiDialogWindow::focusGained(juce::DialogWindow::FocusChangeType /*cause*/) {}

PlayerMidiDialogComponent::PlayerMidiDialogComponent(bool mtcEnabled, MTCSender::Position mtcStartPosition,
    const PlayerMidiDialogWindow::MtcEnabledChangedCallback& mtcEnabledChangedCallback,
    const PlayerMidiDialogWindow::MtcOffsetChangedCallback& mtcOffsetChangedCallback,
    const PlayerMidiDialogWindow::CloseCallback& closeCallback)
    : m_mtcEnabledChangedCallback(mtcEnabledChangedCallback)
    , m_mtcOffsetChangedCallback(mtcOffsetChangedCallback)
    , m_closeCallback(closeCallback)
    , m_mtcEnabledButton(TRANS("MTC aktiviert"))
    , m_mtcStartPosition(mtcStartPosition)
    , m_mtcOffsetGroup("", TRANS("MTC Offset"))
    , m_mtcOffsetHourInput()
    , m_mtcOffsetHourLabel("", TRANS("Hour"))
    , m_mtcOffsetMinuteInput()
    , m_mtcOffsetMinuteLabel("", TRANS("Minute"))
    , m_mtcOffsetSecondInput()
    , m_mtcOffsetSecondLabel("", TRANS("Second"))
    , m_numericInputFilter(2, "0123456789")
    , m_closeButton("close")
{
    addAndMakeVisible(m_mtcEnabledButton);
    m_mtcEnabledButton.setToggleState(mtcEnabled, juce::dontSendNotification);
    m_mtcEnabledButton.addListener(this);

    addAndMakeVisible(m_mtcOffsetGroup);

    addAndMakeVisible(m_mtcOffsetHourInput);
    m_mtcOffsetHourInput.setInputFilter(&m_numericInputFilter, false);
    m_mtcOffsetHourInput.addListener(this);
    m_mtcOffsetHourInput.setText(juce::String(m_mtcStartPosition.hour), false);

    addAndMakeVisible(m_mtcOffsetHourLabel);
    m_mtcOffsetHourLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_mtcOffsetMinuteInput);
    m_mtcOffsetMinuteInput.setInputFilter(&m_numericInputFilter, false);
    m_mtcOffsetMinuteInput.addListener(this);
    m_mtcOffsetMinuteInput.setText(juce::String(m_mtcStartPosition.minute), false);

    addAndMakeVisible(m_mtcOffsetMinuteLabel);
    m_mtcOffsetMinuteLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_mtcOffsetSecondInput);
    m_mtcOffsetSecondInput.setInputFilter(&m_numericInputFilter, false);
    m_mtcOffsetSecondInput.addListener(this);
    m_mtcOffsetSecondInput.setText(juce::String(m_mtcStartPosition.second), false);

    addAndMakeVisible(m_mtcOffsetSecondLabel);
    m_mtcOffsetSecondLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_closeButton);
    m_closeButton.setButtonText(TRANS("Close"));
    m_closeButton.addListener(this);
    m_closeButton.setWantsKeyboardFocus(false);

    setWantsKeyboardFocus(false);

    setSize(200, 200);
}

void PlayerMidiDialogComponent::resized()
{
    static const int rowHeight = 24;
    static const int padding = 10;
    static const int separatorWidth = 15;
    static const int inputWidth = 25;
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;

    m_mtcEnabledButton.setBounds(padding, padding, getWidth(), rowHeight);

    m_mtcOffsetGroup.setBounds(padding, 2 * (padding + rowHeight), getWidth() - 2 * padding, 4 * rowHeight);

    const int labelWidth = (getWidth() - 6 * padding) / 3;
    m_mtcOffsetHourLabel.setBounds(2 * padding, 3 * (padding + rowHeight) - padding / 2, labelWidth, rowHeight);
    m_mtcOffsetMinuteLabel.setBounds(
        3 * padding + labelWidth, 3 * (padding + rowHeight) - padding / 2, labelWidth, rowHeight);
    m_mtcOffsetSecondLabel.setBounds(
        4 * padding + 2 * labelWidth, 3 * (padding + rowHeight) - padding / 2, labelWidth, rowHeight);

    m_mtcOffsetHourInput.setBounds(
        2 * padding + labelWidth / 2 - inputWidth / 2, 4 * (padding + rowHeight) - padding, inputWidth, rowHeight);
    m_mtcOffsetMinuteInput.setBounds(3 * padding + labelWidth + labelWidth / 2 - inputWidth / 2,
        4 * (padding + rowHeight) - padding,
        inputWidth,
        rowHeight);
    m_mtcOffsetSecondInput.setBounds(4 * padding + 2 * labelWidth + labelWidth / 2 - inputWidth / 2,
        4 * (padding + rowHeight) - padding,
        inputWidth,
        rowHeight);

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2, getHeight() - 2 * (buttonHeight - padding), buttonWidth, buttonHeight);
}

void PlayerMidiDialogComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &m_closeButton)
        m_closeCallback();
    else if (buttonThatWasClicked == &m_mtcEnabledButton)
    {
        m_mtcEnabledChangedCallback(m_mtcEnabledButton.getToggleState());
    }
}

void PlayerMidiDialogComponent::textEditorTextChanged(juce::TextEditor& editor)
{
    if (&editor == &m_mtcOffsetHourInput)
    {
        m_mtcStartPosition.hour = editor.getText().getIntValue();
    }
    else if (&editor == &m_mtcOffsetMinuteInput)
    {
        m_mtcStartPosition.minute = editor.getText().getIntValue();
        if (m_mtcStartPosition.minute > 59)
        {
            editor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(255, 0, 0));
            return;
        }
        editor.removeColour(juce::TextEditor::backgroundColourId);
    }
    else if (&editor == &m_mtcOffsetSecondInput)
    {
        m_mtcStartPosition.second = editor.getText().getIntValue();
        if (m_mtcStartPosition.second > 59)
        {
            editor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(255, 0, 0));
            return;
        }
        editor.removeColour(juce::TextEditor::backgroundColourId);
    }

    m_mtcOffsetChangedCallback(m_mtcStartPosition);
}
