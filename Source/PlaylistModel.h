#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

struct TrackConfig {
	File file;
};
class PlaylistEntry {
public:
	String name;
	double durationInSeconds;
	Array<TrackConfig> trackConfigs;

	XmlElement* saveToXml() const;

	static PlaylistEntry createFromXml(const XmlElement& element);
};

/**
	Table model for the channel names.
*/
class PlaylistModel
	: public TableListBoxModel
	, public ChangeBroadcaster
{
public:
	PlaylistModel();

	virtual int getNumRows() override;
	virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

	void add(String name, double durationInSeconds);

	virtual void cellClicked(int rowNumber, int columnId, const MouseEvent &) override;
	virtual void backgroundClicked(const MouseEvent &) override;

	const Array<TrackConfig>& getTrackConfigs(int selectedRow);
	void setTrackConfigs(int selectedRow, const Array<TrackConfig>& trackConfigs);

	void setTrackDuration(int row, double duration);

	XmlElement* saveToXml(int row) const;

	void addFromXml(const XmlElement& element);

	void clear();

private:
	void showPopup(bool enableInsert, bool enableDelete);

	Array<PlaylistEntry> m_playlist;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistModel)
};