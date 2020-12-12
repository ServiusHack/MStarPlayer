#pragma once

#include <functional>

#include "juce_gui_basics/juce_gui_basics.h"

typedef std::function<void(juce::String)> PlaylistEntrySettingsChangedCallback;

/**
    Window wrapper for the PlaylistEntryDialogComponent.

    This allows the component to be shown in its own window.
*/
class PlaylistEntryDialogWindow
    : public juce::DialogWindow
    , public juce::Button::Listener
{
public:
    PlaylistEntryDialogWindow(juce::String name, PlaylistEntrySettingsChangedCallback changedCallback);

    // DialogWindow
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const juce::KeyPress& key) override;
    virtual void focusGained(juce::DialogWindow::FocusChangeType cause) override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogWindow)
};

/**
        The actual component containing controls to edit a playlist entry.
*/
class PlaylistEntryDialogComponent
    : public juce::Component
    , public juce::TextEditor::Listener
{
    friend class PlaylistEntryDialogWindow;

public:
    PlaylistEntryDialogComponent(const juce::String& name, const PlaylistEntrySettingsChangedCallback& changedCallback,
        PlaylistEntryDialogWindow* parent);

    // Component
public:
    virtual void resized() override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(juce::TextEditor&) override;

private:
    juce::Label m_nameLabel;
    juce::TextEditor m_nameEditor;
    juce::TextButton m_closeButton;

    PlaylistEntrySettingsChangedCallback m_changedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogComponent)
};
