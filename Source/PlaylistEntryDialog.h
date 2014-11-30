#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"


typedef std::function<void(String)> PlaylistEntrySettingsChangedCallback;

class PlaylistEntryDialogWindow
	: public DialogWindow
	, public ButtonListener
{
public:
	PlaylistEntryDialogWindow(String name, PlaylistEntrySettingsChangedCallback changedCallback);

	virtual void closeButtonPressed() override;

	virtual bool keyPressed(const KeyPress &key) override;

	virtual void focusGained(FocusChangeType cause) override;

	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogWindow)
};

class PlaylistEntryDialogComponent
	: public Component
	, public juce::TextEditor::Listener
{
	friend class PlaylistEntryDialogWindow;

public:
	PlaylistEntryDialogComponent(String name, PlaylistEntrySettingsChangedCallback changedCallback, PlaylistEntryDialogWindow* parent);
	~PlaylistEntryDialogComponent();

	virtual void resized() override;

	virtual void textEditorTextChanged(TextEditor &) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_closeButton;
	
	PlaylistEntrySettingsChangedCallback m_changedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntryDialogComponent)
};
