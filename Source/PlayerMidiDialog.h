#pragma once

#include <functional>

#include "juce_gui_basics/juce_gui_basics.h"

/**
    Window wrapper for the PlayerMidiDialogComponent

    This allows the component to be shown in its own window.
*/
class PlayerMidiDialogWindow : public juce::DialogWindow
{
public:
    using MtcEnabledChangedCallback = std::function<void(bool)>;
    using CloseCallback = std::function<void()>;

    PlayerMidiDialogWindow(bool mtcEnabled, const MtcEnabledChangedCallback& mtcEnabledChangedCallback,
        const CloseCallback& closeCallback);

private:
    CloseCallback m_closeCallback;

    // DialogWindow
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const juce::KeyPress& key) override;
    virtual void focusGained(juce::DialogWindow::FocusChangeType cause) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMidiDialogWindow)
};

/**
        The actual component containing controls to edit a player.
*/
class PlayerMidiDialogComponent
    : public juce::Component
    , public juce::Button::Listener
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
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    juce::ToggleButton m_mtcEnabledButton;
    juce::TextButton m_closeButton;

    PlayerMidiDialogWindow::MtcEnabledChangedCallback m_mtcEnabledChangedCallback;

    PlayerMidiDialogWindow::CloseCallback m_closeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMidiDialogComponent)
};
