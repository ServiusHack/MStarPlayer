#include "TracksContainer.h"

TracksContainer::TracksContainer(MixerComponent* mixer, SoloBusSettings& soloBusSettings, int outputChannels,
    const Track::TrackConfigChangedCallback& trackConfigChangedCallback,
    const Track::GainChangedCallback& gainChangedCallback, AudioThumbnailCache& audioThumbnailCache,
    TimeSliceThread& thread, MTCSender& mtcSender)
    : m_mixer(mixer)
    , m_soloBusSettings(soloBusSettings)
    , m_mtcSender(mtcSender)
    , m_outputChannels(outputChannels)
    , m_gain(1.0f)
    , m_mute(false)
    , m_longestTrack(nullptr)
    , m_audioThumbnailCache(audioThumbnailCache)
    , m_trackConfigChangedCallback(trackConfigChangedCallback)
    , m_timeSliceThread(thread)
    , m_gainChangedCallback(gainChangedCallback)
{
    mixer->getMixerAudioSource().addInputSource(&m_tracksMixer, false);
    addTrack(true);
}

void TracksContainer::addLongestDurationChangedCallback(LongestDurationChangedCallback durationCallback)
{
    m_longestDurationChangedCallbacks.push_back(durationCallback);
}

void TracksContainer::addPositionCallback(Track::PositionCallback positionCallback)
{
    m_positionCallbacks.push_back(positionCallback);
}

void TracksContainer::addChannelCountChangedCallback(Track::ChannelCountChangedCallback channelCountChangedCallback)
{
    m_channelCountChangedCallbacks.push_back(channelCountChangedCallback);
}

void TracksContainer::addPlayingStateChangedCallback(Track::PlayingStateChangedCallback playingStateChangedCallback)
{
    m_playingStateChangedCallback.push_back(playingStateChangedCallback);
}

void TracksContainer::addTrackRemovedCallback(TrackRemovedCallback callback)
{
    m_trackRemovedCallbacks.push_back(callback);
}

TracksContainer::~TracksContainer()
{
    m_mixer->getMixerAudioSource().removeInputSource(&m_tracksMixer);
}

void TracksContainer::play()
{
    if (m_mtcEnabled)
    {
        m_mtcSender.start();
    }
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::play, std::placeholders::_1));
}

void TracksContainer::pause()
{
    if (m_mtcEnabled)
    {
        m_mtcSender.pause();
    }
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::pause, std::placeholders::_1));
}

void TracksContainer::stop()
{
    if (m_mtcEnabled)
    {
        m_mtcSender.stop();
    }
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::stop, std::placeholders::_1));
}

void TracksContainer::setPosition(double position)
{
    if (m_mtcEnabled)
    {
        m_mtcSender.setPosition(position);
    }
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::setPosition, std::placeholders::_1, position));
}

size_t TracksContainer::size() const
{
    return m_tracks.size();
}

Track& TracksContainer::operator[](size_t index) const
{
    return *m_tracks[index].get();
}

void TracksContainer::clear()
{
    std::vector<std::unique_ptr<Track>> tracks;
    tracks.swap(m_tracks);

    for (const auto& callback : m_channelCountChangedCallbacks)
        callback();

    m_tracksClearedCallback();
}

void TracksContainer::setOutputChannels(int outputChannels)
{
    m_outputChannels = outputChannels;
    std::for_each(
        m_tracks.begin(), m_tracks.end(), std::bind(&Track::setOutputChannels, std::placeholders::_1, outputChannels));
}

void TracksContainer::setGain(float gain)
{
    m_gain = gain;
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::setPlayerGain, std::placeholders::_1, gain));
}

float TracksContainer::getGain() const
{
    return m_gain;
}

void TracksContainer::setSolo(bool solo)
{
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::setPlayerSolo, std::placeholders::_1, solo));
}

void TracksContainer::setMute(bool mute)
{
    m_mute = mute;
    std::for_each(m_tracks.begin(), m_tracks.end(), std::bind(&Track::setPlayerMute, std::placeholders::_1, mute));
}

bool TracksContainer::getMute() const
{
    return m_mute;
}

void TracksContainer::setTrackConfigs(const std::vector<TrackConfig>& trackConfigs)
{
    for (size_t i = 0; i < trackConfigs.size(); ++i)
    {
        m_tracks[i]->loadTrackConfig(trackConfigs[i]);
    }

    static const TrackConfig emptyTrackConfig;
    for (size_t i = trackConfigs.size(); i < m_tracks.size(); ++i)
    {
        m_tracks[i]->loadTrackConfig(emptyTrackConfig);
    }
}

std::vector<TrackConfig> TracksContainer::getTrackConfigs() const
{
    std::vector<TrackConfig> configs(m_tracks.size());
    std::transform(m_tracks.cbegin(), m_tracks.cend(), configs.begin(), [](const std::unique_ptr<Track>& track) {
        return track->getTrackConfig();
    });

    return configs;
}
void TracksContainer::addTrack(bool stereo, const XmlElement* element)
{
    bool soloMute = std::any_of(
        m_tracks.begin(), m_tracks.end(), [](const std::unique_ptr<Track>& track) { return track->getSolo(); });

    auto updateSoloMute = [&]() {
        bool soloMute = std::any_of(
            m_tracks.begin(), m_tracks.end(), [](const std::unique_ptr<Track>& track) { return track->getSolo(); });
        std::for_each(
            m_tracks.begin(), m_tracks.end(), std::bind(&Track::setSoloMute, std::placeholders::_1, soloMute));
    };

    auto updateLongestDuration = [&]() {
        if (m_longestTrack)
        {
            std::for_each(m_positionCallbackRegistrationTokens.begin(),
                m_positionCallbackRegistrationTokens.end(),
                std::bind(&Track::unregisterPositionCallback, m_longestTrack, std::placeholders::_1));
            m_positionCallbackRegistrationTokens.clear();
        }

        double longestDuration = 0;
        m_longestTrack = nullptr;

        for (const auto& track : m_tracks)
        {
            const double duration = track->getDuration();
            if (duration > longestDuration)
            {
                longestDuration = duration;
                m_longestTrack = track.get();
            }
        }

        if (m_longestTrack) // might be nullptr if there are only tracks without a file
            for (auto callback : m_positionCallbacks)
                m_positionCallbackRegistrationTokens.push_back(m_longestTrack->addPositionCallback(callback));

        for (const auto& callback : m_longestDurationChangedCallbacks)
            callback(longestDuration);
    };

    Track::ChannelCountChangedCallback channelCountChanged = [&]() {
        for (const auto& callback : m_channelCountChangedCallbacks)
            callback();
    };

    Track::PlayingStateChangedCallback playingStateChangedCallback = [&](bool isPlaying) {
        if (isPlaying)
            for (const auto& callback : m_playingStateChangedCallback)
                callback(true);
        else
        {
            const bool isAnyPlaying
                = std::any_of(m_tracks.cbegin(), m_tracks.cend(), std::bind(&Track::isPlaying, std::placeholders::_1));
            if (!isAnyPlaying)
            {
                m_mtcSender.stop();
                for (const auto& callback : m_playingStateChangedCallback)
                    callback(false);
            }
        }
    };

    m_tracks.emplace_back(new Track(m_tracksMixer,
        m_soloBusSettings,
        m_tracks.size() + 1,
        stereo,
        m_outputChannels,
        updateLongestDuration,
        soloMute,
        updateSoloMute,
        m_gain,
        m_mute,
        channelCountChanged,
        playingStateChangedCallback,
        m_trackConfigChangedCallback,
        m_gainChangedCallback,
        m_audioThumbnailCache,
        m_timeSliceThread));
    if (element != nullptr)
        m_tracks.back()->restoreFromXml(*element);

    if (m_trackAddedCallback)
        m_trackAddedCallback(*m_tracks.back());

    for (const auto& callback : m_channelCountChangedCallbacks)
        callback();
}

void TracksContainer::setTrackAddedCallback(TrackAddedCallback callback)
{
    m_trackAddedCallback = callback;
}

void TracksContainer::setTracksClearedCallback(TracksClearedCallback callback)
{
    m_tracksClearedCallback = callback;
}

std::vector<std::pair<char, int>> TracksContainer::createMapping()
{
    std::vector<std::pair<char, int>> mapping;
    for (size_t i = 0; i < m_tracks.size(); ++i)
    {
        std::vector<int> playerMapping = m_tracks[i]->getMapping();
        for (size_t j = 0; j < playerMapping.size(); ++j)
        {
            if (playerMapping.size() == 1)
                mapping.push_back(std::pair<char, int>('m', playerMapping[j]));
            else if (playerMapping.size() == 2)
                mapping.push_back(std::pair<char, int>(j ? 'r' : 'l', playerMapping[j]));
        }
    }
    return mapping;
}

bool TracksContainer::isPlaying() const
{
    return std::any_of(m_tracks.cbegin(), m_tracks.cend(), std::bind(&Track::isPlaying, std::placeholders::_1));
}

void TracksContainer::setMtcEnabled(bool enabled)
{
    m_mtcEnabled = enabled;
}

bool TracksContainer::getMtcEnabled() const
{
    return m_mtcEnabled;
}

void TracksContainer::removeTrack(Track* track)
{
    auto it = std::find_if(m_tracks.begin(), m_tracks.end(), [track](const std::unique_ptr<Track>& probe) {
        return probe.get() == track;
    });
    jassert(it != m_tracks.cend());

    for (auto it2 = it; it2 != m_tracks.end(); ++it2)
        (*it2)->setTrackIndex((*it2)->getTrackIndex() - 1);

    // keep track alive so unregistering still works
    std::unique_ptr<Track> tmpTrack = std::move(*it);
    m_tracks.erase(it);

    for (const auto& callback : m_trackRemovedCallbacks)
        callback(tmpTrack->getTrackIndex());

    for (const auto& callback : m_channelCountChangedCallbacks)
        callback();
}
