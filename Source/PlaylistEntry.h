#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
	Track configuration stored for each playlist entry.
*/
struct TrackConfig {
	File file;
};

/**
	Entry in the playlist.
*/
class PlaylistEntry {
public:
	String name;
	double durationInSeconds;
	bool playNext;
	std::vector<TrackConfig> trackConfigs;

	XmlElement* saveToXml() const;

	static PlaylistEntry createFromXml(const XmlElement& element);
};