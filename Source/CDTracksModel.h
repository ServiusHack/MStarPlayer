#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"

/**
    Table model for CD tracks.
*/
class CDTracksModel
    : public juce::TableListBoxModel
    , public juce::ChangeBroadcaster
{
public:
    using ReloadedCallback = std::function<void()>;
    CDTracksModel(juce::AudioCDReader& reader);

    // TableListBoxModel
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(
        juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

private:
    juce::AudioCDReader& m_reader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CDTracksModel)
};
