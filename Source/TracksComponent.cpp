/*
  ==============================================================================

    TracksComponent.cpp
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TracksComponent.h"

TracksComponent::TracksComponent(MixerComponent* mixer, int outputChannels, Track::PositionCallback positionCallback, Player::ChannelCountChangedCallback channelCountChanged, LongestDurationChangedCallback durationCallback)
	: m_mixer(mixer)
	, m_outputChannels(outputChannels)
	, m_positionCallback(positionCallback)
	, m_channelCountChanged(channelCountChanged)
	, m_gain(1.0f)
	, m_mute(false)
	, m_longestDurationChangedCallback(durationCallback)
{
	mixer->getMixerAudioSource().addInputSource(&m_tracksMixer, false);
	setBounds(0,0,100,100);
}

TracksComponent::~TracksComponent()
{
	m_mixer->getMixerAudioSource().removeInputSource(&m_tracksMixer);
	m_tracks.clear();
}

void TracksComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	int y = 0;
	int height = 100;
	for (size_t i = 0; i < m_tracks.size(); i++) {
		m_tracks[i]->setBounds(0, y, getWidth(), height);
		y += height;
	}
	setBounds(getX(), getY(), getWidth(), y);
}

void TracksComponent::addMonoTrack()
{
	addTrack(false);
}

void TracksComponent::addStereoTrack()
{
	addTrack(true);
}

void TracksComponent::addTrackFromXml(const XmlElement* element)
{
	addTrack(true, element);
}

void TracksComponent::addTrack(bool stereo, const XmlElement* element)
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
		double longestDuration = 0;
		Track* longestTrack = nullptr;
		for (auto& track : m_tracks) {
			track->setPositionCallback();
			double duration = track->getDuration();
			if (duration > longestDuration) {
				longestDuration = duration;
				longestTrack = track.get();
			}
		}

		if (longestTrack) // might be nullptr if there are only tracks without a file
			longestTrack->setPositionCallback(m_positionCallback);

		for (auto& track : m_tracks)
			track->setLongestDuration(longestDuration);

		m_longestDurationChangedCallback(longestDuration);
	};

	Player::ChannelCountChangedCallback channelCountChanged = [&]() {
		m_channelCountChanged();
	};

	m_tracks.emplace_back(new Track (m_tracksMixer, m_tracks.size() + 1, stereo, m_outputChannels, updateLongestDuration, soloMute, updateSoloMute, m_gain, m_mute, channelCountChanged));
	if (element != nullptr)
		m_tracks.back()->restoreFromXml(*element);
	addAndMakeVisible(*m_tracks.back());
	resized();

	m_channelCountChanged();
}

void TracksComponent::play()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->play();
}

void TracksComponent::pause()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->pause();
}

void TracksComponent::stop()
{
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->stop();
}

int TracksComponent::playerCount()
{
	return m_tracks.size();
}

Track& TracksComponent::player(int index)
{
	return *m_tracks[index].get();
}

void TracksComponent::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setOutputChannels(outputChannels);
}

void TracksComponent::setGain(float gain)
{
	m_gain = gain;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerGain(gain);
}

float TracksComponent::getGain()
{
	return m_gain;
}

void TracksComponent::setMute(bool mute)
{
	m_mute = mute;
	for (size_t i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerMute(mute);
}

bool TracksComponent::getMute()
{
	return m_mute;
}

void TracksComponent::setTrackConfigs(const std::vector<TrackConfig>& trackConfigs)
{
	for (size_t i = 0; i < trackConfigs.size(); ++i) {
		m_tracks[i]->loadTrackConfig(trackConfigs[i]);
	}

	static const TrackConfig emptyTrackConfig;
	for (size_t i = trackConfigs.size(); i < m_tracks.size(); ++i) {
		m_tracks[i]->loadTrackConfig(emptyTrackConfig);
	}
}

std::vector<TrackConfig> TracksComponent::getTrackConfigs()
{
	std::vector<TrackConfig> configs;
	for (size_t i = 0; i < m_tracks.size(); ++i) {
		configs.push_back(m_tracks[i]->getTrackConfig());
	}

	return configs;
}