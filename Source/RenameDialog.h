#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class RenameDialogWindow : public DialogWindow
{
public:
	RenameDialogWindow(String playerName);

	void closeButtonPressed();

	String getPlayerName();

	bool keyPressed(const KeyPress &key);

	void focusGained(FocusChangeType cause);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenameDialogWindow)
};

class RenameDialogComponent
	: public Component
	, public ButtonListener
{
	friend class RenameDialogWindow;

public:
	RenameDialogComponent(String playerName, RenameDialogWindow* parent);
	~RenameDialogComponent();

    void resized();
    void buttonClicked(Button* buttonThatWasClicked);

private:
	ScopedPointer<Label> m_label;
	ScopedPointer<TextEditor> m_textEditor;
	ScopedPointer<TextButton> m_saveButton;
	ScopedPointer<TextButton> m_abortButton;

	RenameDialogWindow* m_parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenameDialogComponent)
};
