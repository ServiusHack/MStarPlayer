#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "OutputChannelNames.h"

/**
    Window wrapper for the EditSettingsComponent.

    This allows the component to be shown in its own window.
*/
class EditSettingsWindow
    : public DialogWindow
    , public ButtonListener
{
public:
    EditSettingsWindow(ApplicationProperties& applicationProperties);

// DialogWindow overrides
public:
    virtual void closeButtonPressed() override;

// ButtonListener
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
    , public ComboBoxListener
{
public:
    EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties);

private:
    ScopedPointer<Label> m_nameLabel;
    ScopedPointer<FilenameComponent> m_audioEditorFilenameComponent;
    Label m_languageLabel;
    ComboBox m_languageComboBox;
    ScopedPointer<TextButton> m_closeButton;

    ApplicationProperties& m_applicationProperties;

// Component overrides
public:
    virtual void resized() override;

// FilenameComponentListener
public:
    virtual void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;

// ComboBoxListener
public:
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsComponent)
};