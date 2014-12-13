#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"


typedef std::function<void(String)> PlaylistEntrySettingsChangedCallback;

/**
    Window wrapper for the PlaylistEntryDialogComponent.

    This allows the component to be shown in its own window.
*/
class PlaylistEntryDialogWindow
	: public DialogWindow
	, public ButtonListener
{
public:
	PlaylistEntryDialogWindow(String name, PlaylistEntrySettingsChangedCallback changedCallback);

// DialogWindow
public:
	virtual void closeButtonPressed() override;
	virtual bool keyPressed(const KeyPress &key) override;
	virtual void focusGained(FocusChangeType cause) override;

// ButtonListener
public:
	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogWindow)
};


/**
	The actual component containing controls to edit a playlist entry.
*/
class PlaylistEntryDialogComponent
	: public Component
	, public juce::TextEditor::Listener
{
	friend class PlaylistEntryDialogWindow;

public:
	PlaylistEntryDialogComponent(const String& name, const PlaylistEntrySettingsChangedCallback& changedCallback, PlaylistEntryDialogWindow* parent);
	~PlaylistEntryDialogComponent();

// Component
public:
	virtual void resized() override;

// TextEditor::Listener
public:
	virtual void textEditorTextChanged(TextEditor &) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_closeButton;
	
	PlaylistEntrySettingsChangedCallback m_changedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogComponent)
};
