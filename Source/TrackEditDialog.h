#pragma once

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

class TrackEditDialogComponent
	: public Component
    , public ButtonListener
{
	friend class TrackEditDialogWindow;

public:
	TrackEditDialogComponent(String name, TrackSettingsChangedCallback settingsChangedCallback, TrackEditDialogWindow* parent);
	~TrackEditDialogComponent();

    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_saveButton;
	ScopedPointer<TextButton> m_abortButton;

	TrackSettingsChangedCallback m_settingsChangedCallback;
	TrackEditDialogWindow* m_parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditDialogComponent)
};
