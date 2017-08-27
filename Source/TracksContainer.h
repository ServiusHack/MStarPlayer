#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <functional>

#include "MixerComponent.h"
#include "Track.h"
#include "MTCSender.h"

/**
	Container to manage all tracks.
*/
class TracksContainer
{
public:
    typedef std::function<void(double)> LongestDurationChangedCallback;
    typedef std::function<void(Track&)> TrackAddedCallback;
    typedef std::function<void()> TracksClearedCallback;
    typedef std::function<void(int)> TrackRemovedCallback;

    TracksContainer(MixerComponent* mixer, SoloBusSettings& soloBusSettings, int outputChannels, const Track::TrackConfigChangedCallback& trackConfigChangedCallback, AudioThumbnailCache& audioThumbnailCache, TimeSliceThread& thread, MTCSender& mtcSender);
    ~TracksContainer();

// Playback
public:
    void play();
    void pause();
    void stop();
    void setPosition(double position);
    bool isPlaying() const;

private:
    MixerComponent* m_mixer;
    MixerAudioSource m_tracksMixer;
    SoloBusSettings& m_soloBusSettings;

// MTC
public:
    void setMtcEnabled(bool enabled);
    bool getMtcEnabled() const;
private:
    bool m_mtcEnabled{false};
    MTCSender& m_mtcSender;

// Tracks
public:
    size_t size() const;
    Track& operator[](size_t index) const;
    void clear();
    void addTrack(bool stereo, const XmlElement* element = nullptr);
    void removeTrack(Track* track);

protected:
    Track* m_longestTrack;

private:
    std::vector<std::unique_ptr<Track>> m_tracks;
    AudioThumbnailCache& m_audioThumbnailCache;

// Track configs
public:
    void setTrackConfigs(const std::vector<TrackConfig>& trackConfigs);
    std::vector<TrackConfig> getTrackConfigs() const;

// Configuration
public:
    void setOutputChannels(int outputChannels);
    void setGain(float gain);
    float getGain() const;
    void setSolo(bool solo);
    void setMute(bool mute);
    bool getMute() const;
    std::vector<std::pair<char, int>> createMapping();

private:
    int m_outputChannels;
    float m_gain;
    bool m_mute;

// Callbacks
public:
    void setTrackAddedCallback(TrackAddedCallback callback);
    void setTracksClearedCallback(TracksClearedCallback callback);
    void addLongestDurationChangedCallback(LongestDurationChangedCallback durationCallback);
    void addPositionCallback(Track::PositionCallback positionCallback);
    void addChannelCountChangedCallback(Track::ChannelCountChangedCallback channelCountChangedCallback);
    void addPlayingStateChangedCallback(Track::PlayingStateChangedCallback playingStateChangedCallback);
    void addTrackRemovedCallback(TrackRemovedCallback callback);

protected:
    std::vector<LongestDurationChangedCallback> m_longestDurationChangedCallbacks;
    std::vector<Track::PositionCallback> m_positionCallbacks;
    std::vector<Track::ChannelCountChangedCallback> m_channelCountChangedCallbacks;
    std::vector<Track::PositionCallbackRegistrationToken> m_positionCallbackRegistrationTokens;
    std::vector<Track::PlayingStateChangedCallback> m_playingStateChangedCallback;

private:
    TrackAddedCallback m_trackAddedCallback;
    TracksClearedCallback m_tracksClearedCallback;
    std::vector<TrackRemovedCallback> m_trackRemovedCallbacks;
    Track::TrackConfigChangedCallback m_trackConfigChangedCallback;

    TimeSliceThread& m_timeSliceThread;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksContainer)
};