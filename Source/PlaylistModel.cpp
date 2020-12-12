#include "PlaylistModel.h"

#include "juce_audio_formats/juce_audio_formats.h"

#include "BinaryData.h"

#include "Utils.h"

class PlayNextButton : public juce::ImageButton
{
public:
    PlayNextButton() = default;
    void setRowNumber(int rowNumber)
    {
        m_rowNumber = rowNumber;
    }
    int getRowNumber()
    {
        return m_rowNumber;
    }

private:
    int m_rowNumber{0};
};

PlaylistModel::PlaylistModel() {}

int PlaylistModel::getNumRows()
{
    return m_playlist.size();
}

void PlaylistModel::paintRowBackground(
    juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
}

void PlaylistModel::paintCell(
    juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
    g.setColour(juce::Colours::black);

    if (rowNumber >= 0 && static_cast<size_t>(rowNumber) < m_playlist.size())
    {
        if (columnId == 1)
        {
            g.drawText(juce::String(rowNumber + 1), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
        else if (columnId == 2)
        {
            g.drawText(m_playlist[rowNumber].name, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }
        else if (columnId == 3)
        {
            g.drawText(Utils::formatSeconds(m_playlist[rowNumber].durationInSeconds),
                2,
                0,
                width - 4,
                height,
                juce::Justification::centredLeft,
                true);
        }
    }

    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRect(width - 1, 0, 1, height);
}

juce::Component* PlaylistModel::refreshComponentForCell(
    int rowNumber, int columnId, bool /*isRowSelected*/, juce::Component* existingComponentToUpdate)
{
    if (columnId == 4)
    {
        // If it's the output channel column, we'll return our custom component..

        PlayNextButton* editor = static_cast<PlayNextButton*>(existingComponentToUpdate);

        // If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (editor == nullptr)
        {
            editor = new PlayNextButton();
            juce::Image normalImage = juce::ImageFileFormat::loadFrom(
                BinaryData::arrowrightdouble_png, BinaryData::arrowrightdouble_pngSize);
            juce::Image desaturedImage(normalImage.createCopy());
            desaturedImage.desaturate();
            editor->setClickingTogglesState(true);
            editor->setImages(true,
                true,
                true,
                desaturedImage,
                0.7f,
                juce::Colours::transparentBlack,
                normalImage,
                1.0f,
                juce::Colours::transparentBlack,
                normalImage,
                1.0f,
                juce::Colours::transparentBlack,
                0.0f);
            editor->addListener(this);
        }

        bool playNext = m_playlist[rowNumber].playNext;
        editor->setToggleState(playNext, juce::dontSendNotification);
        editor->setRowNumber(rowNumber);

        return editor;
    }
    else
    {
        // for any other column, just return 0, as we'll be painting these columns directly.

        jassert(existingComponentToUpdate == 0);
        return 0;
    }
}

void PlaylistModel::buttonClicked(juce::Button* button)
{
    int rowNumber = static_cast<PlayNextButton*>(button)->getRowNumber();

    m_playlist[rowNumber].playNext = button->getToggleState();
}

void PlaylistModel::add(
    juce::String name, double durationInSeconds, bool playNext, const std::vector<TrackConfig>& trackConfigs)
{
    PlaylistEntry entry;
    entry.name = name;
    entry.durationInSeconds = durationInSeconds;
    entry.playNext = playNext;
    entry.trackConfigs = trackConfigs;
    m_playlist.push_back(entry);
    sendChangeMessage();
}

void PlaylistModel::insert(int rowNumber, const juce::String& name, double durationInSeconds, bool playNext,
    const std::vector<TrackConfig>& trackConfigs)
{
    PlaylistEntry entry;
    entry.name = name;
    entry.durationInSeconds = durationInSeconds;
    entry.playNext = playNext;
    entry.trackConfigs = trackConfigs;
    m_playlist.insert(m_playlist.begin() + rowNumber, entry);
    sendChangeMessage();
    m_reloadedCallback();
}

void PlaylistModel::move(int sourceRowNumber, int targetRowNumber)
{
    if (sourceRowNumber < targetRowNumber)
    {
        std::rotate(std::next(m_playlist.begin(), sourceRowNumber),
            std::next(m_playlist.begin(), sourceRowNumber + 1),
            std::next(m_playlist.begin(), targetRowNumber + 1));
    }
    else
    {
        std::rotate(std::next(m_playlist.begin(), targetRowNumber),
            std::next(m_playlist.begin(), sourceRowNumber),
            std::next(m_playlist.begin(), sourceRowNumber + 1));
    }
    sendChangeMessage();
    m_reloadedCallback();
}

void PlaylistModel::cellClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        showPopup(rowNumber, rowNumber >= 0, rowNumber >= 0);
    }
}

void PlaylistModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
    showEditDialog(rowNumber);
}

void PlaylistModel::backgroundClicked(const juce::MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        showPopup(-1, false, false);
    }
}

juce::var PlaylistModel::getDragSourceDescription(const juce::SparseSet<int>& currentlySelectedRows)
{
    jassert(currentlySelectedRows.size() == 1);

    int start = currentlySelectedRows.getRange(0).getStart();

    juce::Array<juce::var> data;

    data.add(start);
    data.add(m_playlist[start].name);
    data.add(m_playlist[start].durationInSeconds);
    data.add(m_playlist[start].playNext);

    for (const TrackConfig& config : m_playlist[start].trackConfigs)
    {
        data.add(config.file.getFullPathName());
    }

    return data;
}

void PlaylistModel::showPopup(int rowNumber, bool enableInsert, bool enableDelete)
{
    juce::PopupMenu popup;
    popup.addItem(1, TRANS("append"));
    popup.addItem(5, TRANS("append files"));
    popup.addItem(2, TRANS("insert"), enableInsert);
    popup.addItem(3, TRANS("edit"), enableInsert);
    popup.addItem(4, TRANS("delete"), enableDelete);

    switch (popup.show())
    {
    case 1:
        add("", 0);
        break;
    case 2:
        insert(rowNumber, "", 0);
        break;
    case 3:
    {
        showEditDialog(rowNumber);
        break;
    }
    case 4:
        remove(rowNumber);
        break;
    case 5:
    {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        juce::FileChooser myChooser(TRANS("Please select the audio file you want to load ..."),
            juce::File(),
            formatManager.getWildcardForAllFormats());

        if (!myChooser.browseForMultipleFilesToOpen())
            return;

        for (const juce::File& file : myChooser.getResults())
        {
            std::vector<TrackConfig> trackConfigs;
            trackConfigs.push_back({file});
            const std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(trackConfigs[0].file));
            double duration = reader->lengthInSamples / reader->sampleRate;
            add(file.getFileNameWithoutExtension(), duration, false, trackConfigs);
        }
    }
    }
}

void PlaylistModel::showEditDialog(int rowNumber)
{
    PlaylistEntrySettingsChangedCallback callback = [this, rowNumber](juce::String name) {
        m_playlist[rowNumber].name = name;
        m_nameChangedCallback(rowNumber, name);
        sendChangeMessage();
    };
    m_editDialog = std::make_unique<PlaylistEntryDialogWindow>(m_playlist[rowNumber].name, callback);
}

void PlaylistModel::remove(int rowNumber)
{
    m_playlist.erase(m_playlist.begin() + rowNumber);
    sendChangeMessage();
}

const std::vector<TrackConfig>& PlaylistModel::getTrackConfigs(size_t selectedRow)
{
    jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
    return m_playlist[selectedRow].trackConfigs;
}

void PlaylistModel::setTrackConfigs(size_t selectedRow, const std::vector<TrackConfig>& trackConfigs)
{
    jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
    m_playlist[selectedRow].trackConfigs = trackConfigs;
}

void PlaylistModel::setTrackDuration(size_t selectedRow, double duration)
{
    jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
    m_playlist[selectedRow].durationInSeconds = duration;
}

double PlaylistModel::getTrackDuration(size_t selectedRow) const
{
    jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
    return m_playlist[selectedRow].durationInSeconds;
}

void PlaylistModel::setTrackNameIfEmpty(size_t selectedRow, const juce::String& name)
{
    jassert(selectedRow >= 0 && selectedRow < m_playlist.size());
    PlaylistEntry& entry = m_playlist[selectedRow];
    if (entry.name.isEmpty())
    {
        entry.name = name;
        sendChangeMessage();
    }
}

juce::String PlaylistModel::getTrackName(size_t row) const
{
    jassert(row >= 0 && row < m_playlist.size());
    return m_playlist[row].name;
}

void PlaylistModel::clear()
{
    m_playlist.clear();
}

bool PlaylistModel::doPlayNext(int selectedRow)
{
    return m_playlist[selectedRow].playNext;
}

juce::XmlElement* PlaylistModel::saveToXml(const juce::File& projectDirectory) const
{
    juce::XmlElement* playlistXml = new juce::XmlElement("Playlist");
    for (size_t i = 0; i < m_playlist.size(); ++i)
        playlistXml->addChildElement(m_playlist[i].saveToXml(projectDirectory));
    return playlistXml;
}

void PlaylistModel::setReloadedCallback(ReloadedCallback callback)
{
    m_reloadedCallback = callback;
}

void PlaylistModel::setNameChangedCallback(NameChangedCallback callback)
{
    m_nameChangedCallback = callback;
}

void PlaylistModel::restoreFromXml(const juce::XmlElement& element, const juce::File& projectDirectory)
{
    clear();

    for (int i = 0; i < element.getNumChildElements(); ++i)
    {
        m_playlist.push_back(PlaylistEntry::createFromXml(*element.getChildElement(i), projectDirectory));
    }

    m_reloadedCallback();
}

bool PlaylistModel::trackHasFiles(size_t trackIndex) const
{
    trackIndex -= 1; // adjust index to zero-based
    return std::any_of(m_playlist.cbegin(), m_playlist.cend(), [trackIndex](const PlaylistEntry& entry) {
        if (trackIndex < entry.trackConfigs.size())
            return entry.trackConfigs.at(trackIndex).file != juce::File();
        else
            return false;
    });
}

void PlaylistModel::removeTrack(size_t trackIndex)
{
    std::for_each(m_playlist.begin(), m_playlist.end(), [trackIndex](PlaylistEntry& entry) {
        if (trackIndex < entry.trackConfigs.size())
            entry.trackConfigs.erase(std::next(entry.trackConfigs.begin(), trackIndex));
    });
}
