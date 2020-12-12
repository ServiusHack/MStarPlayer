#include "CDTracksModel.h"

#include "Utils.h"

CDTracksModel::CDTracksModel(juce::AudioCDReader& reader)
    : m_reader(reader)
{
}

int CDTracksModel::getNumRows()
{
    return m_reader.getNumTracks();
}

void CDTracksModel::paintRowBackground(
    juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
}

void CDTracksModel::paintCell(
    juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
    g.setColour(juce::Colours::black);

    if (rowNumber >= 0 && rowNumber < m_reader.getNumTracks())
    {
        if (columnId == 1)
        {
            g.drawText(juce::String(rowNumber + 1), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
        else if (columnId == 2)
        {
            const juce::String formattedDuration = Utils::formatSeconds(
                (m_reader.getPositionOfTrackStart(rowNumber + 1) - m_reader.getPositionOfTrackStart(rowNumber))
                / m_reader.sampleRate);
            g.drawText(formattedDuration, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
    }

    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}
