#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PlaylistEntryDialog.h"
#include "PlaylistEntry.h"

/**
	Table model for the channel names.
*/
class PlaylistModel
	: public TableListBoxModel
	, public ChangeBroadcaster
	, public Button::Listener
{
public:
	typedef std::function<void()> ReloadedCallback;
	PlaylistModel();

	void setReloadedCallback(ReloadedCallback);

// Playlist management
public:
	void add(String name, double durationInSeconds);
	void insert(int rowNumber, const String& name, double durationInSeconds);
	void remove(int rowNumber);
	void clear();
	void removeTrack(size_t trackIndex);
	bool doPlayNext(int selectedRow);
	bool trackHasFiles(size_t track) const;
	void setTrackDuration(size_t row, double duration);
	void setTrackNameIfEmpty(size_t row, const String& name);

// Track configs
public:
	const std::vector<TrackConfig>& getTrackConfigs(size_t selectedRow);
	void setTrackConfigs(size_t selectedRow, const std::vector<TrackConfig>& trackConfigs);

// XML serialization
public:
	XmlElement* saveToXml(const File& projectDirectory) const;
	void restoreFromXml(const XmlElement& element, const File& projectDirectory);

// TableListBoxModel
public:
	virtual int getNumRows() override;
	virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;
	virtual void cellClicked(int rowNumber, int columnId, const MouseEvent&) override;
	virtual void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent&) override;
	virtual void backgroundClicked(const MouseEvent&) override;

// Button::Listener
public:
	virtual void buttonClicked(Button*) override;

private:
	void showPopup(int rowNumber, bool enableInsert, bool enableDelete);
	void showEditDialog(int rowNumber);

	std::vector<PlaylistEntry> m_playlist;
	ScopedPointer<PlaylistEntryDialogWindow> m_editDialog;
	ReloadedCallback m_reloadedCallback;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistModel)
};