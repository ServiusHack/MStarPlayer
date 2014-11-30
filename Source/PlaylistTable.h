#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PlaylistModel.h"

#include <functional>

class PlaylistTable
	: public TableListBox
	, public ChangeListener
{
public:

	typedef std::function<Array<TrackConfig>(const Array<TrackConfig>& trackConfigs)> PlaylistEntryChangedCallback;

	PlaylistTable(PlaylistEntryChangedCallback callback);

	virtual void selectedRowsChanged(int lastRowSelected) override;
	virtual void changeListenerCallback(ChangeBroadcaster *source) override;

	void setCurrentDuration(double duration);

	XmlElement* saveToXml() const;

	void restoreFromXml(const XmlElement& element);

	virtual void resized() override;

private:
	PlaylistEntryChangedCallback m_callback;

	int m_previousRow;
};