#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "TracksContainer.h"

#include "Track.h"
#include "TrackUi.h"

#include <memory>

/*
        Component containing all tracks.
*/
class TracksComponent
    : public juce::Component
    , public juce::FileDragAndDropTarget

{
public:
    typedef std::function<void(int)> TrackRemovedCallback;
    typedef std::function<void(juce::String)> FileLoadedCallback;

    TracksComponent(TracksContainer& container, juce::ApplicationProperties& applicationProperties,
        TrackUi::TrackHasFilesCallback trackHasFilesCallback, TrackRemovedCallback trackRemovedCallback,
        FileLoadedCallback fileLoadedCallback);

    void addMonoTrack();
    void addStereoTrack();

    // Component overrides
public:
    virtual void resized() override;

    // FileDragAnDropTarget overrides
public:
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    void trackAdded(Track& track);
    void tracksCleared();
    void trackRemoved(int trackIndex);

    TracksContainer& m_container;

    std::vector<std::unique_ptr<TrackUi>> m_tracks;

    juce::ApplicationProperties& m_applicationProperties;

    TrackUi::TrackHasFilesCallback m_trackHasFilesCallback;
    TrackRemovedCallback m_trackRemovedCallback;
    FileLoadedCallback m_fileLoadedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
