#include "CDTracksModel.h"

#include "Utils.h"


CDTracksModel::CDTracksModel(AudioCDReader& reader)
    : m_reader(reader)
{
}

int CDTracksModel::getNumRows()
{
    return m_reader.getNumTracks();
}

void CDTracksModel::paintRowBackground(Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(Colours::lightblue);
}

void CDTracksModel::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width, int height,
    bool /*rowIsSelected*/)
{
    g.setColour(Colours::black);

    if (rowNumber >= 0 && rowNumber < m_reader.getNumTracks())
    {
        if (columnId == 1) {
            g.drawText(String(rowNumber + 1), 2, 0, width - 4, height, Justification::centredLeft, true);
        }
    }

    g.setColour(Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}