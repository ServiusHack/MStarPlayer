#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
    Table model for CD tracks.
*/
class CDTracksModel
    : public TableListBoxModel
    , public ChangeBroadcaster
{
public:
    typedef std::function<void()> ReloadedCallback;
    CDTracksModel(AudioCDReader& reader);

    // TableListBoxModel
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

private:
    AudioCDReader& m_reader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CDTracksModel)
};