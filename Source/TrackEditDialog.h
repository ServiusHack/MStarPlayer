#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

typedef std::function<void(String)> TrackSettingsChangedCallback;
typedef std::function<void(float)> VolumeChangedCallback;

class TrackEditDialogWindow : public DialogWindow
{
public:
	TrackEditDialogWindow(String name, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback);

	void closeButtonPressed();

	bool keyPressed(const KeyPress &key);

	void focusGained(FocusChangeType cause);

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEditDialogWindow)
};

class TrackEditDialogComponent
	: public Component
    , public ButtonListener
	, public juce::Slider::Listener
{
	friend class TrackEditDialogWindow;

public:
	TrackEditDialogComponent(String name, TrackSettingsChangedCallback settingsChangedCallback, VolumeChangedCallback volumeChangedCallback, TrackEditDialogWindow* parent);
	~TrackEditDialogComponent();

    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
	virtual void sliderValueChanged(Slider *slider) override;

private:
	ScopedPointer<Label> m_nameLabel;
	ScopedPointer<TextEditor> m_nameEditor;
	ScopedPointer<TextButton> m_saveButton;
	ScopedPointer<TextButton> m_abortButton;
	ScopedPointer<Slider> m_volumeSlider;

	TrackSettingsChangedCallback m_settingsChangedCallback;
	VolumeChangedCallback m_volumeChangedCallback;
	TrackEditDialogWindow* m_parent;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEditDialogComponent)
};