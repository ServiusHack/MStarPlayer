#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TracksContainer.h"

#include "Track.h"
#include "TrackUi.h"

#include <memory>

/*
*/
class TracksComponent : public Component
{
public:
	TracksComponent(TracksContainer& container, ApplicationProperties& applicationProperties);

	virtual void resized() override;

	void addMonoTrack();
	void addStereoTrack();

private:
	void trackAdded(Track& track);
	void tracksCleared();

	TracksContainer& m_container;

	std::vector<std::unique_ptr<TrackUi>> m_tracks;

	ApplicationProperties& m_applicationProperties;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
