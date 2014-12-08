#pragma once

#include <functional>
#include <list>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MixerControlable.h"
#include "ChannelRemappingAudioSourceWithVolume.h"
#include "PlaylistEntry.h"

class Track
	: public MixerControlable
	, private Timer
{
public:
	typedef std::function<void()> DurationChangedCallback;
	typedef std::function<void(double, bool)> PositionCallback;
	typedef std::list<PositionCallback>::const_iterator PositionCallbackRegistrationToken;
	typedef std::function<void()> ChannelCountChangedCallback;
	typedef std::function<void(String)> FileChangedCallback;

	Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback, float gain, bool mute, ChannelCountChangedCallback channelCountChangedCallback);
	~Track();

	void play();
	void pause();
	void stop();

	double getDuration();

	void setPosition(double position);

	int getNumChannels();

	void setOutputChannels(int outputChannels);

	void setOutputChannelMapping(int source, int target);
	std::vector<int> getMapping();

	void timerCallback();
	void callPositionCallbacks(double position);

	void loadFileIntoTransport(File audioFile);
	void reloadFile();

	PositionCallbackRegistrationToken addPositionCallback(PositionCallback callback = PositionCallback());
	void unregisterPositionCallback(PositionCallbackRegistrationToken& token);
	void setFileChangedCallback(FileChangedCallback fileChangedCallback);

	XmlElement* saveToXml() const;
	void restoreFromXml(const XmlElement& element);

	void loadTrackConfig(const TrackConfig& config);
	TrackConfig getTrackConfig();

	bool isPlaying();
	
// Solo mute: Track should be muted because other track(s) are in solo mode.
public:
	virtual void setSoloMute(bool mute) override;
	virtual bool getSoloMute() const override;
private:
	bool m_soloMute;

// player gain
public:
	void setPlayerGain(float gain);
private:
	float m_playerGain;

// player mute
public:
	void setPlayerMute(bool mute);
private:
	bool m_playerMute;

// MixerControlable gain
public:
	virtual void setGain(float gain) override;
	virtual float getGain() const override;
private:
	float m_trackGain;

// MixerControlable mute
public:
	virtual void setMute(bool mute) override;
	virtual bool getMute() const override;
private:
	bool m_mute;

// MixerControlable solo
public:
	virtual void setSolo(bool solo) override;
	virtual bool getSolo() const override;
private:
	bool m_solo;

// MixerControlable pan
public:
	virtual void setPan(float) override {};
	virtual float getPan() const override { return 0; };

	virtual float getVolume() const override;

// MixerControlable name
public:
	void setName(String name);
	String getName() const;
private:
	String m_name;

// AudioFormat
public:
	AudioFormatManager& getAudioFormatManager();
private:
	AudioFormatManager m_formatManager;

// stereo
public:
	bool isStereo() const;
	void setStereo(bool stereo);
private:
	bool m_stereo;

// track ID
public:
	int getTrackIndex() const;
private:
	int m_trackIndex;

private:
	void updateGain();

	File m_audioFile;
	MixerAudioSource &m_tracksMixer;
	TimeSliceThread m_thread;
	ScopedPointer<AudioFormatReaderSource> m_currentAudioFileSource;
	AudioTransportSource m_transportSource;
	ChannelRemappingAudioSourceWithVolume m_remappingAudioSource;

	double m_duration;
	int m_outputChannels;

	DurationChangedCallback m_durationChangedCallback;
	DurationChangedCallback m_soloChangedCallback;
	std::list<PositionCallback> m_positionCallbacks;
	ChannelCountChangedCallback m_channelCountChangedCallback;
	FileChangedCallback m_fileChangedCallback;

public:
	AudioThumbnail& getAudioThumbnail();
private:
	AudioThumbnailCache m_audioThumbnailCache;
	AudioThumbnail m_audioThumbnail;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
