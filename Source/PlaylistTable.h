#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PlaylistModel.h"

#include <functional>

/**
        The playlist table component.
*/
class PlaylistTable
    : public juce::TableListBox
    , public juce::ChangeListener
    , public juce::DragAndDropTarget
{
public:
    using PlaylistEntryChangedCallback = std::function<void(const std::vector<TrackConfig>& trackConfigs, bool, int)>;

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
    virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // DragAndDropTarget
public:
    virtual bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;
    virtual void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override;

private:
    void forceSelectRow(int rowIndex);

    PlaylistEntryChangedCallback m_callback;
    PlaylistModel& m_model;

    bool m_playNext;
};
