#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
        Track configuration stored for each playlist entry.
*/
struct TrackConfig
{
    File file;
};

/**
        Entry in the playlist.
*/
class PlaylistEntry
{
public:
    String name;
    double durationInSeconds{0.0};
    bool playNext{false};
    std::vector<TrackConfig> trackConfigs;

    XmlElement* saveToXml(const File& projectDirectory) const;

    static PlaylistEntry createFromXml(const XmlElement& element, const File& projectDirectory);
};