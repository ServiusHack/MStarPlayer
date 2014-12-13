#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

#include "VolumeSlider.h"

typedef std::function<void(String)> TrackSettingsChangedCallback;
typedef std::function<void(float)> VolumeChangedCallback;

/**
    Window wrapper for the TrackEditDialogComponent.

    This allows the component to be shown in its own window.
*/
class TrackEditDialogWindow
	: public DialogWindow
	, public ButtonListener
{
public:
	TrackEditDialogWindow(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback);

// DialogWindow overrides
public:
	virtual void closeButtonPressed() override;
	virtual bool keyPressed(const KeyPress &key) override;
	virtual void focusGained(FocusChangeType cause) override;

// ButtonListener
public:
	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogWindow)
};

/**
	The actual component containing controls to edit a track.
*/
class TrackEditDialogComponent
	: public Component
	, public juce::Slider::Listener
	, public juce::TextEditor::Listener
{
	friend class TrackEditDialogWindow;

public:
	TrackEditDialogComponent(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback, TrackEditDialogWindow* parent);
	~TrackEditDialogComponent();

// Component overrides
public:
	virtual void resized() override;

// Slider::Listener
public:
	virtual void sliderValueChanged(Slider* slider) override;

// TextEditor::Listener
public:
	virtual void textEditorTextChanged(TextEditor&) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_closeButton;
	ScopedPointer<VolumeSlider> m_volumeSlider;

	TrackSettingsChangedCallback m_settingsChangedCallback;
	VolumeChangedCallback m_volumeChangedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogComponent)
};
