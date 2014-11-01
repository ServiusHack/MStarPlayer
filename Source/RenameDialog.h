/*
==============================================================================

RenameDialog.h
Created: 13 Sep 2013 2:37:51pm
Author:  Severin Leonhardt

==============================================================================
*/

#ifndef RENAMEDIALOG_H_INCLUDED
#define RENAMEDIALOG_H_INCLUDED

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

class RenameDialogComponent  : public Component,
                      public ButtonListener
{
	friend class RenameDialogWindow;

public:
	RenameDialogComponent(String playerName, RenameDialogWindow* parent);
	~RenameDialogComponent();

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

private:
	ScopedPointer<Label> label;
    ScopedPointer<TextEditor> textEditor;
	ScopedPointer<TextButton> saveButton;
    ScopedPointer<TextButton> abortButton;

	RenameDialogWindow* parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenameDialogComponent)
};

#endif   // RENAMEDIALOG_H_INCLUDED
