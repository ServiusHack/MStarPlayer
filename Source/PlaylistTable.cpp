#include "PlaylistTable.h"

#include "PlaylistModel.h"

PlaylistTable::PlaylistTable(PlaylistEntryChangedCallback callback)
	: m_callback(callback)
	, m_previousRow(-1)
{
	// set the table header columns
	getHeader().addColumn("#", 1, 50, 50, 100, TableHeaderComponent::defaultFlags);
	getHeader().addColumn("Name", 2, 200, 50, -1, TableHeaderComponent::defaultFlags);
	getHeader().addColumn("Duration", 3, 200, 50, 400, TableHeaderComponent::defaultFlags);

	PlaylistModel* model = new PlaylistModel();
	model->addChangeListener(this);
	setModel(model);

	model->add("", 0);
	updateContent();

	selectRow(0);
}

void PlaylistTable::selectedRowsChanged(int lastRowSelected)
{
	if (lastRowSelected == -1)
		return;

	const Array<TrackConfig>& trackConfigs = static_cast<PlaylistModel*>(getModel())->getTrackConfigs(lastRowSelected);
	Array<TrackConfig> oldTrackConfigs = m_callback(trackConfigs);

	if (m_previousRow != -1)
		static_cast<PlaylistModel*>(getModel())->setTrackConfigs(m_previousRow, oldTrackConfigs);

	m_previousRow = lastRowSelected;
}

void PlaylistTable::changeListenerCallback(ChangeBroadcaster *source)
{
	updateContent();
}

void PlaylistTable::setCurrentDuration(double duration)
{
	static_cast<PlaylistModel*>(getModel())->setTrackDuration(getSelectedRow(), duration);
	repaint();
}

XmlElement* PlaylistTable::saveToXml() const
{
	PlaylistModel* model = static_cast<PlaylistModel*>(getModel());
	XmlElement* playlistXml = new XmlElement("Playlist");
		for (int i = 0; i < model->getNumRows(); ++i)
			playlistXml->addChildElement(model->saveToXml(i));
	return playlistXml;
}

void PlaylistTable::restoreFromXml(const XmlElement& element)
{
	PlaylistModel* model = static_cast<PlaylistModel*>(getModel());
	model->clear();
	m_previousRow = -1;

	for (int i = 0; i < element.getNumChildElements(); ++i) {
		model->addFromXml(*element.getChildElement(i));
	}

	selectedRowsChanged(getSelectedRow());
}