#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <functional>

#include "Track.h"
#include "MixerComponent.h"

class TracksContainer
{
public:
	typedef std::function<void(double)> LongestDurationChangedCallback;

	typedef std::function<void(Track&)> TrackAddedCallback;
	typedef std::function<void()> TracksClearedCallback;
	typedef std::function<void(int)> TrackRemovedCallback;

	TracksContainer(MixerComponent* mixer, int outputChannels);
	~TracksContainer();

	void play();
	void pause();
	void stop();

	void setPosition(double position);

	size_t size() const;
	Track& operator[](size_t index) const;
	void clear();

	void setOutputChannels(int outputChannels);

	void setGain(float gain);
	float getGain() const;

	void setMute(bool mute);
	bool getMute() const;

	void setTrackConfigs(const std::vector<TrackConfig>& trackConfigs);
	std::vector<TrackConfig> getTrackConfigs() const;

	void addTrack(bool stereo, const XmlElement* element = nullptr);

	void setTrackAddedCallback(TrackAddedCallback callback);
	void setTracksClearedCallback(TracksClearedCallback callback);
	void addLongestDurationChangedCallback(LongestDurationChangedCallback durationCallback);
	void addPositionCallback(Track::PositionCallback positionCallback);
	void addChannelCountChangedCallback(Track::ChannelCountChangedCallback channelCountChangedCallback);
	void addPlayingStateChangedCallback(Track::PlayingStateChangedCallback playingStateChangedCallback);
	void addTrackRemovedCallback(TrackRemovedCallback callback);

	std::vector<std::pair<char, int>> createMapping();

	bool isPlaying() const;

	void removeTrack(Track* track);

protected:
	std::vector<LongestDurationChangedCallback> m_longestDurationChangedCallbacks;
	std::vector<Track::PositionCallback> m_positionCallbacks;
	std::vector<Track::ChannelCountChangedCallback> m_channelCountChangedCallbacks;
	std::vector<Track::PositionCallbackRegistrationToken> m_positionCallbackRegistrationTokens;
	std::vector<Track::PlayingStateChangedCallback> m_playingStateChangedCallback;
	Track* m_longestTrack;

private:
	MixerComponent* m_mixer;
	MixerAudioSource m_tracksMixer;
	std::vector<std::unique_ptr<Track>> m_tracks;
	int m_outputChannels;
	float m_gain;
	bool m_mute;

	TrackAddedCallback m_trackAddedCallback;
	TracksClearedCallback m_tracksClearedCallback;
	std::vector<TrackRemovedCallback> m_trackRemovedCallbacks;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksContainer)
};