#include "PlayerMidiDialog.h"

PlayerMidiDialogWindow::PlayerMidiDialogWindow(bool mtcEnabled, const MtcEnabledChangedCallback& mtcEnabledChangedCallback, const CloseCallback& closeCallback)
    : DialogWindow(TRANS("Configure MIDI for player"), Colours::lightgrey, true, false)
    , m_closeCallback(closeCallback)
{
    PlayerMidiDialogComponent* component = new PlayerMidiDialogComponent(mtcEnabled, mtcEnabledChangedCallback, closeCallback);
    setContentOwned(component, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
    setResizable(false, false);
}

void PlayerMidiDialogWindow::closeButtonPressed()
{
    m_closeCallback();
}

bool PlayerMidiDialogWindow::keyPressed(const KeyPress& key)
{
    if (key == KeyPress::returnKey)
    {
        exitModalState(0);
        return true;
    }

    return false;
}

void PlayerMidiDialogWindow::focusGained(FocusChangeType /*cause*/)
{
}

PlayerMidiDialogComponent::PlayerMidiDialogComponent(bool mtcEnabled, const PlayerMidiDialogWindow::MtcEnabledChangedCallback& mtcEnabledChangedCallback, const PlayerMidiDialogWindow::CloseCallback& closeCallback)
    : m_mtcEnabledChangedCallback(mtcEnabledChangedCallback)
    , m_closeCallback(closeCallback)
    , m_mtcEnabledButton(TRANS("MTC aktiviert"))
    , m_closeButton("close")
{
    addAndMakeVisible(m_mtcEnabledButton);
    m_mtcEnabledButton.setToggleState(mtcEnabled, juce::dontSendNotification);
    m_mtcEnabledButton.addListener(this);

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
    const static int buttonWidth = 80;
    const static int buttonHeight = 24;

    m_mtcEnabledButton.setBounds(padding, padding, getWidth(), rowHeight);

    m_closeButton.setBounds(
        (getWidth() - buttonWidth) / 2,
        getHeight() - 2 * (buttonHeight - padding),
        buttonWidth,
        buttonHeight);
}

void PlayerMidiDialogComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == &m_closeButton)
        m_closeCallback();
    else if (buttonThatWasClicked == &m_mtcEnabledButton)
    {
        m_mtcEnabledChangedCallback(m_mtcEnabledButton.getToggleState());
    }
}
