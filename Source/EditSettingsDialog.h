#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OutputChannelNames.h"

/**
    Window wrapper for the EditSettingsComponent.

    This allows the component to be shown in its own window.
*/
class EditSettingsWindow
    : public DialogWindow
    , public Button::Listener
{
public:
    EditSettingsWindow(ApplicationProperties& applicationProperties, const std::function<void()>& snapToGridChanged);

    // DialogWindow overrides
public:
    virtual void closeButtonPressed() override;

    // Button::Listener
public:
    virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsWindow)
};

/**
        The actual component.
*/
class EditSettingsComponent
    : public Component
    , public FilenameComponentListener
    , public ComboBox::Listener
    , public ToggleButton::Listener
    , public TextEditor::Listener
{
public:
    EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties,
        const std::function<void()>& snapToGridChanged);

private:
    ApplicationProperties& m_applicationProperties;
    Label m_nameLabel;
    FilenameComponent m_audioEditorFilenameComponent;
    Label m_languageLabel;
    ComboBox m_languageComboBox;
    TextButton m_closeButton;

    ToggleButton m_snapToGridButton;
    Label m_snapToGridWidthLabel;
    TextEditor m_snapToGridWidthEditor;
    Label m_snapToGridHeightLabel;
    TextEditor m_snapToGridHeightEditor;
    std::function<void()> m_snapToGridChanged;

    // Component overrides
public:
    virtual void resized() override;

    // FilenameComponentListener
public:
    virtual void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;

    // ComboBox::Listener
public:
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    // ToggleButton::Listener
public:
    virtual void buttonClicked(Button*) override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(TextEditor&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsComponent)
};
