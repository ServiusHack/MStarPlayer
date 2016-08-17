#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

/**
    Window wrapper for the PlayerEditDialogComponent.

    This allows the component to be shown in its own window.
*/
class PlayerEditDialogWindow : public DialogWindow
{
public:
    typedef std::function<void(Colour)> ColourChangedCallback;
    typedef std::function<void(String)> StringChangedCallback;
    typedef std::function<void(juce::File)> ImageChangedCallback;
    typedef std::function<void()> CloseCallback;

    PlayerEditDialogWindow(const String& playerName, const Colour& color, const String& imagePath, const StringChangedCallback& stringCallback, const ColourChangedCallback& colourCallback, const CloseCallback& closeCallback, const ImageChangedCallback& imageCallback = ImageChangedCallback());

    String getPlayerName() const;

private:
    CloseCallback m_closeCallback;

// DialogWindow
public:
    virtual void closeButtonPressed() override;
    virtual bool keyPressed(const KeyPress& key) override;
    virtual void focusGained(FocusChangeType cause) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerEditDialogWindow)
};

/**
	The actual component containing controls to edit a player.
*/
class PlayerEditDialogComponent
    : public Component
    , public ButtonListener
    , public ChangeListener
    , public juce::TextEditor::Listener
{
    friend class PlayerEditDialogWindow;

public:
    PlayerEditDialogComponent(const String& playerName, const Colour& color, const String& imagePath, const PlayerEditDialogWindow::StringChangedCallback& stringCallback, const PlayerEditDialogWindow::ColourChangedCallback& colourCallback, const PlayerEditDialogWindow::CloseCallback& closeCallback, const PlayerEditDialogWindow::ImageChangedCallback& imageCallback);
    ~PlayerEditDialogComponent();

// Component overrides
public:
    virtual void resized() override;

// ButtonListener overrides
public:
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

// ChangeListener overrides
public:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

// TextEditor::Listener overrides
public:
    virtual void textEditorTextChanged(TextEditor&) override;

private:
    ScopedPointer<Label> m_label;
    ScopedPointer<TextEditor> m_textEditor;
    ScopedPointer<TextButton> m_closeButton;

    ScopedPointer<TextButton> m_colorButton;
    OptionalScopedPointer<TextButton> m_imageSelectorButton;
    OptionalScopedPointer<TextButton> m_imageResetButton;

    Colour m_color;
    File m_imageFile;

    PlayerEditDialogWindow::ColourChangedCallback m_colorCallback;
    PlayerEditDialogWindow::StringChangedCallback m_stringCallback;
    PlayerEditDialogWindow::ImageChangedCallback m_imageCallback;

    PlayerEditDialogWindow::CloseCallback m_closeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerEditDialogComponent)
};
