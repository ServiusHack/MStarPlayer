/*
==============================================================================

TrackEditDialog.h
Created: 13 Sep 2013 2:37:51pm
Author:  Severin Leonhardt

==============================================================================
*/

#ifndef TrackEditDIALOG_H_INCLUDED
#define TrackEditDIALOG_H_INCLUDED

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"


typedef std::function<void(String)> TrackSettingsChangedCallback;

class TrackEditDialogWindow : public DialogWindow
{
public:
	TrackEditDialogWindow(String name, TrackSettingsChangedCallback settingsChangedCallback);

	void closeButtonPressed();

	bool keyPressed(const KeyPress &key);

	void focusGained(FocusChangeType cause);

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogWindow)
};

class TrackEditDialogComponent  : public Component,
                      public ButtonListener
{
	friend class TrackEditDialogWindow;

public:
	TrackEditDialogComponent(String name, TrackSettingsChangedCallback settingsChangedCallback, TrackEditDialogWindow* parent);
	~TrackEditDialogComponent();

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

private:
	ScopedPointer<Label> nameLabel;
    ScopedPointer<TextEditor> nameEditor;
	ScopedPointer<TextButton> saveButton;
    ScopedPointer<TextButton> abortButton;

	TrackSettingsChangedCallback settingsChangedCallback;
	TrackEditDialogWindow* parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditDialogComponent)
};

#endif   // TrackEditDIALOG_H_INCLUDED
