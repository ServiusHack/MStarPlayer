#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "OutputChannelNames.h"

/**
    Window wrapper for the EditSettingsComponent.

    This allows the component to be shown in its own window.
*/
class EditSettingsWindow
    : public juce::DialogWindow
    , public juce::Button::Listener
{
public:
    EditSettingsWindow(
        juce::ApplicationProperties& applicationProperties, const std::function<void()>& snapToGridChanged);

    // DialogWindow overrides
public:
    virtual void closeButtonPressed() override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsWindow)
};

/**
        The actual component.
*/
class EditSettingsComponent
    : public juce::Component
    , public juce::FilenameComponentListener
    , public juce::ComboBox::Listener
    , public juce::ToggleButton::Listener
    , public juce::TextEditor::Listener
{
public:
    EditSettingsComponent(EditSettingsWindow* parent, juce::ApplicationProperties& applicationProperties,
        const std::function<void()>& snapToGridChanged);

private:
    juce::ApplicationProperties& m_applicationProperties;
    juce::Label m_nameLabel;
    juce::FilenameComponent m_audioEditorFilenameComponent;
    juce::Label m_languageLabel;
    juce::ComboBox m_languageComboBox;
    juce::TextButton m_closeButton;

    juce::ToggleButton m_snapToGridButton;
    juce::Label m_snapToGridWidthLabel;
    juce::TextEditor m_snapToGridWidthEditor;
    juce::Label m_snapToGridHeightLabel;
    juce::TextEditor m_snapToGridHeightEditor;
    std::function<void()> m_snapToGridChanged;

    // Component overrides
public:
    virtual void resized() override;

    // FilenameComponentListener
public:
    virtual void filenameComponentChanged(juce::FilenameComponent* fileComponentThatHasChanged) override;

    // ComboBox::Listener
public:
    virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    // ToggleButton::Listener
public:
    virtual void buttonClicked(juce::Button*) override;

    // TextEditor::Listener
public:
    virtual void textEditorTextChanged(juce::TextEditor&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsComponent)
};
