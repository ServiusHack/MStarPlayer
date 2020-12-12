#pragma once

#include "juce_events/juce_events.h"

#include "PlaylistEntry.h"
#include "PlaylistEntryDialog.h"

/**
        Table model for the channel names.
*/
class PlaylistModel
    : public juce::TableListBoxModel
    , public juce::ChangeBroadcaster
    , public juce::Button::Listener
{
public:
    typedef std::function<void()> ReloadedCallback;
    typedef std::function<void(int rowNumber, const juce::String& name)> NameChangedCallback;
    PlaylistModel();

    void setReloadedCallback(ReloadedCallback);
    void setNameChangedCallback(NameChangedCallback);

    // Playlist management
public:
    void add(juce::String name, double durationInSeconds, bool playNext = false,
        const std::vector<TrackConfig>& trackConfigs = {});
    void insert(int rowNumber, const juce::String& name, double durationInSeconds, bool playNext = false,
        const std::vector<TrackConfig>& trackConfigs = {});
    void move(int sourceRowNumber, int targetRowNumber);
    void remove(int rowNumber);
    void clear();
    void removeTrack(size_t trackIndex);
    bool doPlayNext(int selectedRow);
    bool trackHasFiles(size_t track) const;
    void setTrackDuration(size_t row, double duration);
    double getTrackDuration(size_t row) const;
    void setTrackNameIfEmpty(size_t row, const juce::String& name);
    juce::String getTrackName(size_t row) const;

    // Track configs
public:
    const std::vector<TrackConfig>& getTrackConfigs(size_t selectedRow);
    void setTrackConfigs(size_t selectedRow, const std::vector<TrackConfig>& trackConfigs);

    // XML serialization
public:
    juce::XmlElement* saveToXml(const juce::File& projectDirectory) const;
    void restoreFromXml(const juce::XmlElement& element, const juce::File& projectDirectory);

    // TableListBoxModel
public:
    virtual int getNumRows() override;
    virtual void paintRowBackground(
        juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    virtual void paintCell(
        juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    virtual juce::Component* refreshComponentForCell(
        int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    virtual void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
    virtual void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
    virtual void backgroundClicked(const juce::MouseEvent&) override;
    virtual juce::var getDragSourceDescription(const juce::SparseSet<int>& currentlySelectedRows) override;

    // Button::Listener
public:
    virtual void buttonClicked(juce::Button*) override;

private:
    void showPopup(int rowNumber, bool enableInsert, bool enableDelete);
    void showEditDialog(int rowNumber);

    std::vector<PlaylistEntry> m_playlist;
    std::unique_ptr<PlaylistEntryDialogWindow> m_editDialog;
    ReloadedCallback m_reloadedCallback;
    NameChangedCallback m_nameChangedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistModel)
};
