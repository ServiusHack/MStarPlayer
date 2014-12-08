#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "VolumeSlider.h"
#include "TrackEditDialog.h"
#include "MixerControlable.h"
#include "Track.h"

class TrackUi
	: public Component
	, public ChangeListener
	, public Button::Listener
	, public juce::Slider::Listener
	, public MixerControlableChangeListener
{
public:
	typedef std::function<void(double)> SetPositionCallback;

	TrackUi(Track& track, ApplicationProperties& applicationProperties, SetPositionCallback setPositionCallback);
	~TrackUi();

	void changeListenerCallback(ChangeBroadcaster *source);
	void loadFile();
	void setLongestDuration(double duration);
	void positionChanged(double position);

private:
	void updateIdText();

	Track& m_track;

	ScopedPointer<Label> m_idLabel;
	ScopedPointer<Label> m_descriptionLabel;
	ScopedPointer<ImageButton> m_editButton;
	ScopedPointer<ImageButton> m_openButton;
	ScopedPointer<ImageButton> m_editFileButton;
	ScopedPointer<ImageButton> m_soloButton;
	ScopedPointer<ImageButton> m_muteButton;
	ScopedPointer<VolumeSlider> m_volumeSlider;
	ScopedPointer<Label> m_fileNameLabel;

	ScopedPointer<TrackEditDialogWindow> m_editDialog;

	double m_longestDuration;
	double m_progress; // the progress of the playback

	ApplicationProperties& m_applicationProperties;

	SetPositionCallback m_setPositionCallback;

// Button::Listener
public:
	void buttonClicked(Button* button);

// Component
public:
	virtual void paint(Graphics&) override;
	virtual void resized() override;
	virtual void mouseDown(const MouseEvent & event) override;
	virtual void mouseDrag(const MouseEvent & event) override;

// juce::Slider::Listener
public:
	virtual void sliderValueChanged(Slider *slider) override;

// MixerControlableChangeListener
public:
	virtual void gainChanged(float gain) override;
	virtual void muteChanged(bool /*mute*/) override;
	virtual void soloChanged(bool /*solo*/) override;
	virtual void nameChanged(const String& name) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackUi)
};