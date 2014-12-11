/*
  ==============================================================================

    TracksComponent.cpp
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TracksComponent.h"

TracksComponent::TracksComponent(TracksContainer& container, ApplicationProperties& applicationProperties, TrackUi::TrackHasFilesCallback trackHasFilesCallback, TrackRemovedCallback trackRemovedCallback)
	: m_container(container)
	, m_applicationProperties(applicationProperties)
	, m_trackHasFilesCallback(trackHasFilesCallback)
	, m_trackRemovedCallback(trackRemovedCallback)
{
	trackAdded(m_container[0]);
	m_container.setTrackAddedCallback(std::bind(&TracksComponent::trackAdded, this, std::placeholders::_1));
	m_container.addLongestDurationChangedCallback([&](double duration) {
		for (auto& track : m_tracks)
			track->setLongestDuration(duration);
	});
	m_container.setTracksClearedCallback(std::bind(&TracksComponent::tracksCleared, this));

	m_container.addTrackRemovedCallback(std::bind(&TracksComponent::trackRemoved, this, std::placeholders::_1));
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
	m_container.addTrack(false);
}

void TracksComponent::addStereoTrack()
{
	m_container.addTrack(true);
}

void TracksComponent::trackAdded(Track& track)
{
	m_tracks.emplace_back(new TrackUi(track, m_applicationProperties,
		std::bind(&TracksContainer::setPosition, &m_container, std::placeholders::_1),
		std::bind(&TracksContainer::removeTrack, &m_container, std::placeholders::_1),
		m_trackHasFilesCallback));
	addAndMakeVisible(*m_tracks.back());
	resized();
}

void TracksComponent::tracksCleared()
{
	m_tracks.clear();
	removeAllChildren();
	resized();
}

void TracksComponent::trackRemoved(int trackIndex)
{
	auto it = std::next(m_tracks.begin(), trackIndex);
	it = m_tracks.erase(it);
	for (; it != m_tracks.end(); ++it)
		(*it)->updateIdText();
	resized();
	repaint();
	m_trackRemovedCallback(trackIndex);
}
