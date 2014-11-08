#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrackEditDialog.h"

typedef std::function<void()> DurationChangedCallback;
typedef std::function<void(double)> PositionCallback;

/*
*/
class Track
	: public Component
	, public ChangeListener
	, public Button::Listener
	, public juce::Slider::Listener
	, private Timer
{
public:
	Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback, float gain);
	~Track();

	/** Play or stop the audio playback. */
	void buttonClicked(Button * /*button*/);

	void paint (Graphics&);
	void resized();

	virtual void sliderValueChanged(Slider *slider) override;

	void mouseDown (const MouseEvent & event);

	void play();
	void pause();
	void stop();

	double getDuration();

	std::vector<int> getMapping();
	int getNumChannels();

	void setOutputChannels(int outputChannels);
	void setOutputChannelMapping(int source, int target);

	void changeListenerCallback(ChangeBroadcaster *source);

	void setLongestDuration(double duration);
	
	// Track should be muted because other track(s) are in solo mode.
	void setSoloMute(bool mute);

	bool isSolo() const;

	void timerCallback();

	void loadFileIntoTransport();

	void setPositionCallback(PositionCallback callback = PositionCallback());

	XmlElement* saveToXml() const;
	void restoreFromXml(const XmlElement& element);

	void setName(String name);

	void setPlayerGain(float gain);

	void setTrackGain(float gain);

private:
	void updateIdText();
	void loadFile();
	void updateGain();

	double m_progress; // the progress of the playback

	ScopedPointer<Label> m_idLabel;
	ScopedPointer<Label> m_descriptionLabel;
	ScopedPointer<ImageButton> m_editButton;
	ScopedPointer<ImageButton> m_openButton;
	ScopedPointer<ImageButton> m_soloButton;
	ScopedPointer<ImageButton> m_muteButton;
	ScopedPointer<Slider> m_volumeSlider;
	ScopedPointer<AudioThumbnail> m_audioThumbnail;
	ScopedPointer<ChannelRemappingAudioSource> m_remappingAudioSource;
	AudioThumbnailCache m_audioThumbnailCache;
	AudioFormatManager m_formatManager;

	ScopedPointer<TrackEditDialogWindow> m_editDialog;

	File m_audioFile;
	bool m_stereo;

	int m_trackIndex;

	//AudioFormatManager formatManager;
	MixerAudioSource &m_tracksMixer;
	TimeSliceThread m_thread;
	AudioTransportSource m_transportSource;
	AudioFormatReaderSource* m_currentAudioFileSource;

	bool m_soloMute;
	double m_duration;
	double m_longestDuration;
	float m_playerGain;
	float m_trackGain;

	DurationChangedCallback m_durationChangedCallback;
	DurationChangedCallback m_soloChangedCallback;

	PositionCallback m_positionCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
