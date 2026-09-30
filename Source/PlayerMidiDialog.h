#pragma once

#include <functional>

#include "juce_gui_basics/juce_gui_basics.h"

#include "MTCSender.h"

/**
    Window wrapper for the PlayerMidiDialogComponent

    This allows the component to be shown in its own window.
*/
class PlayerMidiDialogWindow : public juce::DialogWindow
{
public:
    using MtcEnabledChangedCallback = std::function<void(bool)>;
    using MtcOffsetChangedCallback = std::function<void(MTCSender::Position)>;
    using CloseCallback = std::function<void()>;

    PlayerMidiDialogWindow(bool mtcEnabled, MTCSender::Position mtcStartPosition,
        const MtcEnabledChangedCallback& mtcEnabledChangedCallback,
        const MtcOffsetChangedCallback& mtcOffsetChangedCallback, const CloseCallback& closeCallback);

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
    , public juce::TextEditor::Listener
{
    friend class PlayerMidiDialogWindow;

public:
    PlayerMidiDialogComponent(bool mtcEnabled, MTCSender::Position mtcStartPosition,
        const PlayerMidiDialogWindow::MtcEnabledChangedCallback& mtcEnabledChangedCallback,
        const PlayerMidiDialogWindow::MtcOffsetChangedCallback& mtcOffsetChangedCallback,
        const PlayerMidiDialogWindow::CloseCallback& closeCallback);

    // Component overrides
public:
    virtual void resized() override;

    // Button::Listener overrides
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

    // TextEditor::Listener overrides
public:
    virtual void textEditorTextChanged(juce::TextEditor&) override;

private:
    juce::ToggleButton m_mtcEnabledButton;
    juce::Label m_mtcOffsetLabel;

    MTCSender::Position m_mtcStartPosition;
    juce::GroupComponent m_mtcOffsetGroup;
    juce::TextEditor::LengthAndCharacterRestriction m_numericInputFilter;
    juce::TextEditor m_mtcOffsetHourInput;
    juce::Label m_mtcOffsetHourLabel;
    juce::TextEditor m_mtcOffsetMinuteInput;
    juce::Label m_mtcOffsetMinuteLabel;
    juce::TextEditor m_mtcOffsetSecondInput;
    juce::Label m_mtcOffsetSecondLabel;

    juce::TextButton m_closeButton;

    PlayerMidiDialogWindow::MtcEnabledChangedCallback m_mtcEnabledChangedCallback;
    PlayerMidiDialogWindow::MtcOffsetChangedCallback m_mtcOffsetChangedCallback;

    PlayerMidiDialogWindow::CloseCallback m_closeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerMidiDialogComponent)
};
