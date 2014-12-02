#include "PlaylistTable.h"

#include "PlaylistModel.h"

PlaylistTable::PlaylistTable(PlaylistEntryChangedCallback callback)
	: m_callback(callback)
	, m_previousRow(-1)
	, m_playNext(false)
{
	setAutoSizeMenuOptionShown(false);
	getHeader().setPopupMenuActive(false);

	// set the table header columns
	getHeader().addColumn("#", 1, 25, 25, 50, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn("Name", 2, 200, 50, -1, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn("Duration", 3, 80, 80, 100, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn("", 4, 25, 25, 25, TableHeaderComponent::notResizableOrSortable);

	m_model.addChangeListener(this);
	setModel(&m_model);

	m_model.add("", 0);
	updateContent();

	selectRow(0);
}

void PlaylistTable::selectedRowsChanged(int lastRowSelected)
{
	if (lastRowSelected == -1)
		return;

	const std::vector<TrackConfig>& trackConfigs = static_cast<PlaylistModel*>(getModel())->getTrackConfigs(lastRowSelected);
	std::vector<TrackConfig> oldTrackConfigs = m_callback(trackConfigs, m_playNext);

	if (m_previousRow != -1)
		static_cast<PlaylistModel*>(getModel())->setTrackConfigs(m_previousRow, oldTrackConfigs);

	m_previousRow = lastRowSelected;
}

void PlaylistTable::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
	updateContent();
	repaint();
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

void PlaylistTable::resized()
{
	TableListBox::resized();

	TableHeaderComponent& header = getHeader();
	header.setColumnWidth(2, getVisibleRowWidth() - header.getColumnWidth(1) - header.getColumnWidth(3) - header.getColumnWidth(4));

}

void PlaylistTable::next()
{
	if (getSelectedRow() + 1 < m_model.getNumRows()) {
		m_playNext = m_model.doPlayNext(getSelectedRow());
		selectRow(getSelectedRow() + 1);
		m_playNext = false;
	}
}