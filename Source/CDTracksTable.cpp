#include "CDTracksTable.h"

#include "CDTracksModel.h"

CDTracksTable::CDTracksTable(const TrackChangedCallback& callback)
    : m_callback(callback)
{
    setAutoSizeMenuOptionShown(false);
    getHeader().setPopupMenuActive(false);

    // set the table header columns
    getHeader().addColumn(TRANS("Track"), 1, 25, 25, -1, juce::TableHeaderComponent::notResizableOrSortable);
    getHeader().addColumn(TRANS("Duration"), 2, 100, 25, 100, juce::TableHeaderComponent::notResizableOrSortable);
    getHeader().setStretchToFitActive(true);

    updateContent();
}

void CDTracksTable::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected == -1)
        return;

    m_callback(lastRowSelected);
}

void CDTracksTable::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    updateContent();
    repaint();
}

void CDTracksTable::previous()
{
    if (getSelectedRow() > 0)
        selectRow(getSelectedRow() - 1);
    else
        selectRow(getTableListBoxModel()->getNumRows() - 1);
}

void CDTracksTable::next()
{
    if (getSelectedRow() + 1 < getTableListBoxModel()->getNumRows())
    {
        forceSelectRow(getSelectedRow() + 1);
    }
    else
    {
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
