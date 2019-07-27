#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TracksContainer.h"

#include "Track.h"
#include "TrackUi.h"

#include <memory>

/*
        Component containing all tracks.
*/
class TracksComponent
    : public Component
    , public FileDragAndDropTarget

{
public:
    typedef std::function<void(int)> TrackRemovedCallback;
    typedef std::function<void(String)> FileLoadedCallback;

    TracksComponent(TracksContainer& container, ApplicationProperties& applicationProperties,
        TrackUi::TrackHasFilesCallback trackHasFilesCallback, TrackRemovedCallback trackRemovedCallback,
        FileLoadedCallback fileLoadedCallback);

    void addMonoTrack();
    void addStereoTrack();

    // Component overrides
public:
    virtual void resized() override;

    // FileDragAnDropTarget overrides
public:
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:
    void trackAdded(Track& track);
    void tracksCleared();
    void trackRemoved(int trackIndex);

    TracksContainer& m_container;

    std::vector<std::unique_ptr<TrackUi>> m_tracks;

    ApplicationProperties& m_applicationProperties;

    TrackUi::TrackHasFilesCallback m_trackHasFilesCallback;
    TrackRemovedCallback m_trackRemovedCallback;
    FileLoadedCallback m_fileLoadedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
