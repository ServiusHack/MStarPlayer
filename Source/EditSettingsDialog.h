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
    EditSettingsWindow(ApplicationProperties& applicationProperties);

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
{
public:
    EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties);

private:
    ApplicationProperties& m_applicationProperties;
    Label m_nameLabel;
    FilenameComponent m_audioEditorFilenameComponent;
    Label m_languageLabel;
    ComboBox m_languageComboBox;
    TextButton m_closeButton;

    // Component overrides
public:
    virtual void resized() override;

    // FilenameComponentListener
public:
    virtual void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;

    // ComboBox::Listener
public:
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsComponent)
};
