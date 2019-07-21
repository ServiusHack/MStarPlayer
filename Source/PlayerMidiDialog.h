#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

/**
    Window wrapper for the PlayerMidiDialogComponent

    This allows the component to be shown in its own window.
*/
class PlayerMidiDialogWindow : public DialogWindow
{
public:
    typedef std::function<void(bool)> MtcEnabledChangedCallback;
    typedef std::function<void()> CloseCallback;

    PlayerMidiDialogWindow(bool mtcEnabled, const MtcEnabledChangedCallback& mtcEnabledChangedCallback,
        const CloseCallback& closeCallback);

private:
    CloseCallback m_closeCallback;

    // DialogWindow
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const KeyPress& key) override;
    virtual void focusGained(FocusChangeType cause) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMidiDialogWindow)
};

/**
        The actual component containing controls to edit a player.
*/
class PlayerMidiDialogComponent
    : public Component
    , public Button::Listener
{
    friend class PlayerMidiDialogWindow;

public:
    PlayerMidiDialogComponent(bool mtcEnabled,
        const PlayerMidiDialogWindow::MtcEnabledChangedCallback& mtcEnabledChangedCallback,
        const PlayerMidiDialogWindow::CloseCallback& closeCallback);

    // Component overrides
public:
    virtual void resized() override;

    // Button::Listener overrides
public:
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    ToggleButton m_mtcEnabledButton;
    TextButton m_closeButton;

    PlayerMidiDialogWindow::MtcEnabledChangedCallback m_mtcEnabledChangedCallback;

    PlayerMidiDialogWindow::CloseCallback m_closeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMidiDialogComponent)
};
