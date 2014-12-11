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
    
	virtual void closeButtonPressed() override;

	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditSettingsWindow)
};

/**
	The actual component.
*/
class EditSettingsComponent
	: public Component
	, public FilenameComponentListener
{
public:
	EditSettingsComponent(EditSettingsWindow* parent, ApplicationProperties& applicationProperties);

	virtual void resized() override;

	virtual void filenameComponentChanged(FilenameComponent *fileComponentThatHasChanged) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<FilenameComponent> m_audioEditorFilenameComponent;
	ScopedPointer<TextButton> m_closeButton;

	ApplicationProperties& m_applicationProperties;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditSettingsComponent)
};