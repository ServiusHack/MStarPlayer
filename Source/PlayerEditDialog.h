#pragma once

#include <functional>
#include <optional>

#include "juce_gui_basics/juce_gui_basics.h"

/**
    Window wrapper for the PlayerEditDialogComponent.

    This allows the component to be shown in its own window.
*/
class PlayerEditDialogWindow : public juce::DialogWindow
{
public:
    using ColourChangedCallback = std::function<void(juce::Colour)>;
    using StringChangedCallback = std::function<void(juce::String)>;
    using ImageChangedCallback = std::function<void(juce::File)>;
    using CloseCallback = std::function<void()>;

    PlayerEditDialogWindow(const juce::String& playerName, const juce::Colour& color, const juce::String& imagePath,
        const StringChangedCallback& stringCallback, const ColourChangedCallback& colourCallback,
        const CloseCallback& closeCallback, const ImageChangedCallback& imageCallback = ImageChangedCallback());

    juce::String getPlayerName() const;

private:
    CloseCallback m_closeCallback;

    // DialogWindow
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const juce::KeyPress& key) override;
    virtual void focusGained(juce::DialogWindow::FocusChangeType cause) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerEditDialogWindow)
};

/**
        The actual component containing controls to edit a player.
*/
class PlayerEditDialogComponent
    : public juce::Component
    , public juce::Button::Listener
    , public juce::ChangeListener
    , public juce::TextEditor::Listener
{
    friend class PlayerEditDialogWindow;

public:
    PlayerEditDialogComponent(const juce::String& playerName, const juce::Colour& color, const juce::String& imagePath,
        const PlayerEditDialogWindow::StringChangedCallback& stringCallback,
        const PlayerEditDialogWindow::ColourChangedCallback& colourCallback,
        const PlayerEditDialogWindow::CloseCallback& closeCallback,
        const PlayerEditDialogWindow::ImageChangedCallback& imageCallback);

    // Component overrides
public:
    virtual void resized() override;

    // Button::Listener overrides
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

    // ChangeListener overrides
public:
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // TextEditor::Listener overrides
public:
    virtual void textEditorTextChanged(juce::TextEditor&) override;

private:
    juce::Label m_label;
    juce::TextEditor m_textEditor;
    juce::TextButton m_closeButton;

    juce::TextButton m_colorButton;
    std::optional<juce::TextButton> m_imageSelectorButton;
    std::optional<juce::TextButton> m_imageResetButton;

    juce::Colour m_color;
    juce::File m_imageFile;

    PlayerEditDialogWindow::ColourChangedCallback m_colorCallback;
    PlayerEditDialogWindow::StringChangedCallback m_stringCallback;
    PlayerEditDialogWindow::ImageChangedCallback m_imageCallback;

    PlayerEditDialogWindow::CloseCallback m_closeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerEditDialogComponent)
};
