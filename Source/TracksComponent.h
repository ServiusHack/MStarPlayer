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
	typedef std::function<void(int)> TrackRemovedCallback;

	TracksComponent(TracksContainer& container, ApplicationProperties& applicationProperties, TrackUi::TrackHasFilesCallback trackHasFilesCallback, TrackRemovedCallback trackRemovedCallback);

	virtual void resized() override;

	void addMonoTrack();
	void addStereoTrack();

private:
	void trackAdded(Track& track);
	void tracksCleared();
	void trackRemoved(int trackIndex);

	TracksContainer& m_container;

	std::vector<std::unique_ptr<TrackUi>> m_tracks;

	ApplicationProperties& m_applicationProperties;

	TrackUi::TrackHasFilesCallback m_trackHasFilesCallback;
	TrackRemovedCallback m_trackRemovedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
