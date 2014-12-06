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

	TracksContainer(MixerComponent* mixer, int outputChannels);
	~TracksContainer();

	void play();
	void pause();
	void stop();

	int playerCount();
	Track& player(int index);
	void clear();

	void setOutputChannels(int outputChannels);

	void setGain(float gain);
	float getGain();

	void setMute(bool mute);
	bool getMute();

	void setTrackConfigs(const std::vector<TrackConfig>& trackConfigs);
	std::vector<TrackConfig> getTrackConfigs();

	void addTrack(bool stereo, const XmlElement* element = nullptr);

	void setTrackAddedCallback(TrackAddedCallback callback);
	void setTracksClearedCallback(TracksClearedCallback callback);
	void addLongestDurationChangedCallback(LongestDurationChangedCallback durationCallback);
	void addPositionCallback(Track::PositionCallback positionCallback);
	void addChannelCountChangedCallback(Track::ChannelCountChangedCallback channelCountChangedCallback);

	std::vector<std::pair<char, int>> createMapping();

	bool isPlaying();

protected:
	std::vector<LongestDurationChangedCallback> m_longestDurationChangedCallbacks;
	std::vector<Track::PositionCallback> m_positionCallbacks;
	std::vector<Track::ChannelCountChangedCallback> m_channelCountChangedCallbacks;
	std::vector<Track::PositionCallbackRegistrationToken> m_positionCallbackRegistrationTokens;
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

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksContainer)
};