#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

struct TrackConfig {
	File file;
};

class PlaylistEntry {
public:
	String name;
	double durationInSeconds;
	bool playNext;
	std::vector<TrackConfig> trackConfigs;

	XmlElement* saveToXml() const;

	static PlaylistEntry createFromXml(const XmlElement& element);
};