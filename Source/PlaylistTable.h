#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PlaylistModel.h"

#include <functional>

/**
        The playlist table component.
*/
class PlaylistTable
    : public TableListBox
    , public ChangeListener
    , public DragAndDropTarget
{
public:
    typedef std::function<void(const std::vector<TrackConfig>& trackConfigs, bool, int)> PlaylistEntryChangedCallback;

    PlaylistTable(const PlaylistEntryChangedCallback& callback, PlaylistModel& playlistModel);

    void setCurrentDuration(double duration);

    void previous();
    void next(bool onlyIfEntrySaysSo = false);

    // TableListBox
public:
    virtual void selectedRowsChanged(int lastRowSelected) override;
    virtual void resized() override;

    // ChangeListener
public:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

    // DragAndDropTarget
public:
    virtual bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    virtual void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
    void forceSelectRow(int rowIndex);

    PlaylistEntryChangedCallback m_callback;
    PlaylistModel& m_model;

    bool m_playNext;
};