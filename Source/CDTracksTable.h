#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "CDTracksModel.h"

#include <functional>

/**
    The CD tracks table component.
*/
class CDTracksTable
    : public TableListBox
    , public ChangeListener
{
public:
    typedef std::function<void(int sampleIndex)> TrackChangedCallback;

    CDTracksTable(const TrackChangedCallback& callback);

    void previous();
    void next();

// TableListBox
public:
    virtual void selectedRowsChanged(int lastRowSelected) override;

// ChangeListener
public:
    virtual void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    void forceSelectRow(int rowIndex);

    TrackChangedCallback m_callback;
};