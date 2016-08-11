#include "CDTracksTable.h"

#include "CDTracksModel.h"

CDTracksTable::CDTracksTable(const TrackChangedCallback& callback)
    : m_callback(callback)
{
    setAutoSizeMenuOptionShown(false);
    getHeader().setPopupMenuActive(false);

    // set the table header columns
    getHeader().addColumn("Track", 1, 25, 25, -1, TableHeaderComponent::notResizableOrSortable);
    getHeader().setStretchToFitActive(true);

    updateContent();
}

void CDTracksTable::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected == -1)
        return;

    m_callback(lastRowSelected);
}

void CDTracksTable::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    updateContent();
    repaint();
}

void CDTracksTable::previous()
{
    if (getSelectedRow() > 0)
        selectRow(getSelectedRow() - 1);
    else
        selectRow(static_cast<CDTracksModel*>(getModel())->getNumRows() - 1);
}

void CDTracksTable::next()
{
    if (getSelectedRow() + 1 < static_cast<CDTracksModel*>(getModel())->getNumRows()) {
        forceSelectRow(getSelectedRow() + 1);
    }
    else {
        forceSelectRow(0);
    }
}

void CDTracksTable::forceSelectRow(int rowIndex)
{
    if (rowIndex != getSelectedRow())
        selectRow(rowIndex);
    else
        selectedRowsChanged(rowIndex);

}