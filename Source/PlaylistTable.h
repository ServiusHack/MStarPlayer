#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PlaylistModel.h"

#include <functional>

class PlaylistTable
	: public TableListBox
	, public ChangeListener
{
public:
	typedef std::function<std::vector<TrackConfig>(const std::vector<TrackConfig>& trackConfigs,bool)> PlaylistEntryChangedCallback;

	PlaylistTable(PlaylistEntryChangedCallback callback, PlaylistModel& playlistModel);

	virtual void selectedRowsChanged(int lastRowSelected) override;
	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

	void setCurrentDuration(double duration);

	virtual void resized() override;

	void next();

private:
	PlaylistEntryChangedCallback m_callback;
	PlaylistModel& m_model;
	int m_previousRow;

	bool m_playNext;
};