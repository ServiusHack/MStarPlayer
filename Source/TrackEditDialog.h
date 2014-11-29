#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

#include "VolumeSlider.h"

typedef std::function<void(String)> TrackSettingsChangedCallback;
typedef std::function<void(float)> VolumeChangedCallback;

class TrackEditDialogWindow
	: public DialogWindow
	, public ButtonListener
{
public:
	TrackEditDialogWindow(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback);

	virtual void closeButtonPressed() override;

	virtual bool keyPressed(const KeyPress &key) override;

	virtual void focusGained(FocusChangeType cause) override;

	virtual void buttonClicked(Button* buttonThatWasClicked) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogWindow)
};

class TrackEditDialogComponent
	: public Component
	, public juce::Slider::Listener
	, public juce::TextEditor::Listener
{
	friend class TrackEditDialogWindow;

public:
	TrackEditDialogComponent(String name, float trackGain, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback, TrackEditDialogWindow* parent);
	~TrackEditDialogComponent();

	virtual void resized() override;
	virtual void sliderValueChanged(Slider *slider) override;

	virtual void textEditorTextChanged(TextEditor &) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_closeButton;
	ScopedPointer<VolumeSlider> m_volumeSlider;

	TrackSettingsChangedCallback m_settingsChangedCallback;
	VolumeChangedCallback m_volumeChangedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditDialogComponent)
};
