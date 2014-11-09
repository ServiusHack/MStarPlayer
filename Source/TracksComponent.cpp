/*
  ==============================================================================

    TracksComponent.cpp
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TracksComponent.h"

TracksComponent::TracksComponent(MixerComponent* mixer, int outputChannels, PositionCallback positionCallback)
	: m_mixer(mixer)
	, m_outputChannels(outputChannels)
	, m_positionCallback(positionCallback)
	, m_gain(1.0f)
	, m_mute(false)
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
	for (int i = 0; i < m_tracks.size(); i++) {
		m_tracks.getUnchecked(i)->setBounds(0, y, getWidth(), height);
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
	bool soloMute = std::any_of(m_tracks.begin(), m_tracks.end(), [](const Track* track) {
		return track->isSolo();
	});

	auto updateSoloMute = [&]() {
		bool soloMute = std::any_of(m_tracks.begin(), m_tracks.end(), [](const Track* track) {
			return track->isSolo();
		});
		for (Track* track : m_tracks)
			track->setSoloMute(soloMute);
	};

	auto updateLongestDuration = [&]() {
		double longestDuration = 0;
		Track* longestTrack = nullptr;
		for (Track* track : m_tracks) {
			track->setPositionCallback();
			double duration = track->getDuration();
			if (duration > longestDuration) {
				longestDuration = duration;
				longestTrack = track;
			}
		}

		longestTrack->setPositionCallback(m_positionCallback);

		for (Track* track : m_tracks)
			track->setLongestDuration(longestDuration);
	};

	Track* track = new Track(m_tracksMixer, m_tracks.size() + 1, stereo, m_outputChannels, updateLongestDuration, soloMute, updateSoloMute, m_gain, m_mute);
	m_tracks.add(track);
	if (element != nullptr)
		track->restoreFromXml(*element);
	addAndMakeVisible(track);
	resized();
}

void TracksComponent::play()
{
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->play();
}

void TracksComponent::pause()
{
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->pause();
}

void TracksComponent::stop()
{
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->stop();
}

int TracksComponent::playerCount()
{
	return m_tracks.size();
}

Track& TracksComponent::player(int index)
{
	return *m_tracks[index];
}

void TracksComponent::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setOutputChannels(outputChannels);
}

void TracksComponent::setGain(float gain)
{
	m_gain = gain;
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerGain(gain);
}

float TracksComponent::getGain()
{
	return m_gain;
}

void TracksComponent::setMute(bool mute)
{
	m_mute = mute;
	for (int i = 0; i < m_tracks.size(); ++i)
		m_tracks[i]->setPlayerMute(mute);
}

bool TracksComponent::getMute()
{
	return m_mute;
}