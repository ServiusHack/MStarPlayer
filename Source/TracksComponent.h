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
	TracksComponent(TracksContainer& container);

	virtual void resized() override;

	void addMonoTrack();
	void addStereoTrack();

private:
	void trackAdded(Track& track);

	TracksContainer& m_container;

	std::vector<std::unique_ptr<TrackUi>> m_tracks;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
