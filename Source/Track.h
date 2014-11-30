#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrackEditDialog.h"
#include "VolumeSlider.h"
#include "Player.h"
#include "ChannelRemappingAudioSourceWithVolume.h"
#include "PlaylistModel.h"

/*
*/
class Track
	: public Component
	, public ChangeListener
	, public Button::Listener
	, public juce::Slider::Listener
	, private Timer
	, public MixerControlable
{
public:
	typedef std::function<void()> DurationChangedCallback;
	typedef std::function<void(double)> PositionCallback;

	Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback, float gain, bool mute, Player::ChannelCountChangedCallback channelCountChangedCallback);
	~Track();

	/** Play or stop the audio playback. */
	void buttonClicked(Button * /*button*/);

	virtual void paint(Graphics&) override;
	virtual void resized() override;

	virtual void sliderValueChanged(Slider *slider) override;

	virtual void mouseDown(const MouseEvent & event) override;

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
	virtual void setSoloMute(bool mute) override;

	virtual bool getSoloMute() const override;

	virtual bool getSolo() const override;

	void timerCallback();

	void loadFileIntoTransport();

	void setPositionCallback(PositionCallback callback = PositionCallback());

	XmlElement* saveToXml() const;
	void restoreFromXml(const XmlElement& element);

	void setName(String name);

	void setPlayerGain(float gain);

	virtual void setGain(float gain) override;

	void setPlayerMute(bool mute);

	virtual void setMute(bool mute) override;

	virtual float getGain() const override;
	virtual bool getMute() const override;

	virtual void setSolo(bool solo) override;

	virtual void setPan(float) override {};
	virtual float getPan() const override { return 0; };

	virtual float getVolume() const override;

	void loadTrackConfig(const TrackConfig& config);
	TrackConfig getTrackConfig();

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
	ScopedPointer<VolumeSlider> m_volumeSlider;
	ScopedPointer<AudioThumbnail> m_audioThumbnail;
	ScopedPointer<ChannelRemappingAudioSourceWithVolume> m_remappingAudioSource;
	ScopedPointer<Label> m_fileNameLabel;
	AudioThumbnailCache m_audioThumbnailCache;
	AudioFormatManager m_formatManager;

	ScopedPointer<TrackEditDialogWindow> m_editDialog;

	File m_audioFile;
	bool m_stereo;

	int m_trackIndex;

	MixerAudioSource &m_tracksMixer;
	TimeSliceThread m_thread;
	AudioTransportSource m_transportSource;
	AudioFormatReaderSource* m_currentAudioFileSource;

	bool m_playerMute;
	bool m_soloMute;
	double m_duration;
	double m_longestDuration;
	float m_playerGain;
	float m_trackGain;
	int m_outputChannels;

	DurationChangedCallback m_durationChangedCallback;
	DurationChangedCallback m_soloChangedCallback;

	PositionCallback m_positionCallback;

	Player::ChannelCountChangedCallback m_channelCountChangedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
