#include "TracksContainer.h"

TracksContainer::TracksContainer(MixerComponent* mixer, int outputChannels)
	: m_mixer(mixer)
	, m_outputChannels(outputChannels)
	, m_gain(1.0f)
	, m_mute(false)
	, m_longestTrack(nullptr)
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

TracksContainer::~TracksContainer()
{
	m_mixer->getMixerAudioSource().removeInputSource(&m_tracksMixer);
}

void TracksContainer::play()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->play();
}

void TracksContainer::pause()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->pause();
}

void TracksContainer::stop()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->stop();
}

void TracksContainer::setPosition(double position)
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPosition(position);
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

	for (auto const callback: m_channelCountChangedCallbacks)
		callback();
}

void TracksContainer::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setOutputChannels(outputChannels);
}

void TracksContainer::setGain(float gain)
{
	m_gain = gain;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerGain(gain);
}

float TracksContainer::getGain() const
{
	return m_gain;
}

void TracksContainer::setMute(bool mute)
{
	m_mute = mute;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerMute(mute);
}

bool TracksContainer::getMute() const
{
	return m_mute;
}

void TracksContainer::setTrackConfigs(const std::vector<TrackConfig>& trackConfigs)
{
	for (size_t i = 0; i < trackConfigs.size(); ++i) {
		m_tracks[i]->loadTrackConfig(trackConfigs[i]);
	}

	static const TrackConfig emptyTrackConfig;
	for (size_t i = trackConfigs.size(); i < m_tracks.size(); ++i) {
		m_tracks[i]->loadTrackConfig(emptyTrackConfig);
	}
}

std::vector<TrackConfig> TracksContainer::getTrackConfigs() const
{
	std::vector<TrackConfig> configs;
	for (size_t i = 0; i < m_tracks.size(); ++i) {
		configs.push_back(m_tracks[i]->getTrackConfig());
	}

	return configs;
}
void TracksContainer::addTrack(bool stereo, const XmlElement* element)
{
	bool soloMute = std::any_of(m_tracks.begin(), m_tracks.end(), [](const std::unique_ptr<Track>& track) {
		return track->getSolo();
	});

	auto updateSoloMute = [&]() {
		bool soloMute = std::any_of(m_tracks.begin(), m_tracks.end(), [](const std::unique_ptr<Track>& track) {
			return track->getSolo();
		});
		for (auto& track : m_tracks)
			track->setSoloMute(soloMute);
	};

	auto updateLongestDuration = [&]() {

		if (m_longestTrack)
		{
			for (auto token : m_positionCallbackRegistrationTokens)
				m_longestTrack->unregisterPositionCallback(token);
			m_positionCallbackRegistrationTokens.clear();
		}

		double longestDuration = 0;
		m_longestTrack = nullptr;

		for (auto& track : m_tracks) {
			double duration = track->getDuration();
			if (duration > longestDuration) {
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
		for (auto const callback: m_channelCountChangedCallbacks)
			callback();
	};

	m_tracks.emplace_back(new Track (m_tracksMixer, m_tracks.size() + 1, stereo, m_outputChannels, updateLongestDuration, soloMute, updateSoloMute, m_gain, m_mute, channelCountChanged));
	if (element != nullptr)
		m_tracks.back()->restoreFromXml(*element);

	if (m_trackAddedCallback)
		m_trackAddedCallback(*m_tracks.back());

	for (auto const callback: m_channelCountChangedCallbacks)
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
	for (size_t i = 0; i < m_tracks.size(); ++i) {
		std::vector<int> playerMapping = m_tracks[i]->getMapping();
		for (size_t j = 0; j < playerMapping.size(); ++j) {
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
	for (size_t i = 0; i < m_tracks.size(); ++i)
		if (m_tracks[i]->isPlaying())
			return true;
	return false;
}
