#pragma once

#include <functional>
#include <list>

#include "juce_audio_formats/juce_audio_formats.h"
#include "juce_audio_utils/juce_audio_utils.h"

#include "ChannelRemappingAudioSourceWithVolume.h"
#include "MixerControlable.h"
#include "PlaylistEntry.h"

/**
        Actual logic for the playback of a track.
*/
class Track
    : public MixerControlable
    , public SoloBusSettingsListener
    , private juce::Timer
{
public:
    using DurationChangedCallback = std::function<void()>;
    using PositionCallback = std::function<void(double, bool)>;
    using PositionCallbackRegistrationToken = std::list<PositionCallback>::const_iterator;
    using ChannelCountChangedCallback = std::function<void()>;
    using FileChangedCallback = std::function<void(const juce::File&, bool)>;
    using PlayingStateChangedCallback = std::function<void(bool)>;
    using TrackConfigChangedCallback = std::function<void()>;
    using GainChangedCallback = std::function<void(const char*, float)>;

    Track(juce::MixerAudioSource& tracksMixer, SoloBusSettings& soloBusSettings, int trackIndex, bool stereo,
        int outputChannels, DurationChangedCallback callback, bool soloMute,
        DurationChangedCallback soloChangedCallback, float gain, bool mute,
        ChannelCountChangedCallback channelCountChangedCallback,
        PlayingStateChangedCallback playingStateChangedCallback, TrackConfigChangedCallback trackConfigChangedCallback,
        GainChangedCallback gainChangedCallback, juce::AudioThumbnailCache& audioThumbnailCache,
        juce::TimeSliceThread& thread);
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

    void timerCallback() override;
    void callPositionCallbacks(double position, bool hasStreamFinished);

    void loadFileIntoTransport(const juce::File& audioFile);
    void reloadFile();
    void unloadFile();

    PositionCallbackRegistrationToken addPositionCallback(PositionCallback callback = PositionCallback());
    void unregisterPositionCallback(PositionCallbackRegistrationToken& token);
    void setFileChangedCallback(FileChangedCallback fileChangedCallback);

    void saveToXml(juce::XmlElement* element) const;
    void restoreFromXml(const juce::XmlElement& element);

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
    void setPlayerSolo(bool solo);

private:
    bool m_playerSolo;

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
    virtual void setPan(float pan) override;
    virtual float getPan() const override;
    virtual float getVolume() const override;

private:
    float m_pan;

    // MixerControlable name
public:
    void setName(juce::String name);
    juce::String getName() const override;

private:
    juce::String m_name;

    // AudioFormat
public:
    juce::AudioFormatManager& getAudioFormatManager();

private:
    juce::AudioFormatManager m_formatManager;

    // stereo
public:
    bool isStereo() const;
    void setStereo(bool stereo);

private:
    bool m_stereo;

    // track ID
public:
    int getTrackIndex() const;
    void setTrackIndex(int index);

private:
    int m_trackIndex;

    // Audio Thumbnail
public:
    juce::AudioThumbnail& getAudioThumbnail();

private:
    juce::AudioThumbnailCache& m_audioThumbnailCache;
    juce::AudioThumbnail m_audioThumbnail;

    // SoloBusListener
public:
    void soloBusChannelChanged(SoloBusChannel channel, int outputChannel, int previousOutputChannel) override;

private:
    void updateGain();

    juce::File m_audioFile;
    juce::MixerAudioSource& m_tracksMixer;
    juce::TimeSliceThread& m_thread;
    std::unique_ptr<juce::AudioFormatReaderSource> m_currentAudioFileSource;
    juce::AudioTransportSource m_transportSource;
    SoloBusSettings& m_soloBusSettings;
    ChannelRemappingAudioSourceWithVolume m_remappingAudioSource;

    double m_duration;

    DurationChangedCallback m_durationChangedCallback;
    DurationChangedCallback m_soloChangedCallback;
    std::list<PositionCallback> m_positionCallbacks;
    ChannelCountChangedCallback m_channelCountChangedCallback;
    FileChangedCallback m_fileChangedCallback;
    PlayingStateChangedCallback m_playingStateChangedCallback;
    TrackConfigChangedCallback m_trackConfigChangedCallback;
    GainChangedCallback m_gainChangedCallback;

    bool m_loadingTrackConfig;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
