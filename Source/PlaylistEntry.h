#pragma once

#include "juce_core/juce_core.h"

/**
        Track configuration stored for each playlist entry.
*/
struct TrackConfig
{
    juce::File file;
};

/**
        Entry in the playlist.
*/
class PlaylistEntry
{
public:
    juce::String name;
    double durationInSeconds{0.0};
    bool playNext{false};
    std::vector<TrackConfig> trackConfigs;

    juce::XmlElement* saveToXml(const juce::File& projectDirectory) const;

    static PlaylistEntry createFromXml(const juce::XmlElement& element, const juce::File& projectDirectory);
};
