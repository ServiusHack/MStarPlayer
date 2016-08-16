#include "PlaylistTable.h"

#include "PlaylistModel.h"

PlaylistTable::PlaylistTable(const PlaylistEntryChangedCallback& callback, PlaylistModel& playlistModel)
	: m_callback(callback)
	, m_playNext(false)
	, m_model(playlistModel)
{
	setAutoSizeMenuOptionShown(false);
	getHeader().setPopupMenuActive(false);

	// set the table header columns
	getHeader().addColumn(TRANS("#"), 1, 25, 25, 50, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn(TRANS("Name"), 2, 200, 50, -1, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn(TRANS("Duration"), 3, 80, 80, 100, TableHeaderComponent::notResizableOrSortable);
	getHeader().addColumn("", 4, 25, 25, 25, TableHeaderComponent::notResizableOrSortable);

	m_model.addChangeListener(this);
	setModel(&m_model);

	m_model.add("", 0);
	updateContent();

	playlistModel.setReloadedCallback([&]() {
		selectedRowsChanged(getSelectedRow());
	});
}

void PlaylistTable::selectedRowsChanged(int lastRowSelected)
{
	if (lastRowSelected == -1)
		return;

	const std::vector<TrackConfig>& trackConfigs = m_model.getTrackConfigs(lastRowSelected);
	m_callback(trackConfigs, m_playNext);
}

void PlaylistTable::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
	updateContent();
	repaint();
}

void PlaylistTable::setCurrentDuration(double duration)
{
	m_model.setTrackDuration(getSelectedRow(), duration);
	repaint();
}

void PlaylistTable::resized()
{
	TableListBox::resized();

	TableHeaderComponent& header = getHeader();
	header.setColumnWidth(2, getVisibleRowWidth() - header.getColumnWidth(1) - header.getColumnWidth(3) - header.getColumnWidth(4));
}

void PlaylistTable::previous()
{
	if (getSelectedRow() > 0)
		selectRow(getSelectedRow() - 1);
	else
		selectRow(m_model.getNumRows() - 1);
}

void PlaylistTable::next(bool onlyIfEntrySaysSo)
{
	if (getSelectedRow() + 1 < m_model.getNumRows()) {
		m_playNext = onlyIfEntrySaysSo && m_model.doPlayNext(getSelectedRow());
		forceSelectRow(getSelectedRow() + 1);
		m_playNext = false;
	}
	else {
		m_playNext = onlyIfEntrySaysSo && m_model.doPlayNext(getSelectedRow());
		forceSelectRow(0);
		m_playNext = false;
	}
}

void PlaylistTable::forceSelectRow(int rowIndex)
{
	if (rowIndex != getSelectedRow())
		selectRow(rowIndex);
	else
		selectedRowsChanged(rowIndex);

}