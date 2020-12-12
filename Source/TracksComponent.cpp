#include "TracksComponent.h"

namespace
{
bool isAudioFile(const juce::String& filePath)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    for (int i = 0; i < formatManager.getNumKnownFormats(); ++i)
    {
        for (auto&& extension : formatManager.getKnownFormat(i)->getFileExtensions())
        {
            if (filePath.endsWithIgnoreCase(extension))
            {
                return true;
            }
        }
    }

    return false;
}
}

TracksComponent::TracksComponent(TracksContainer& container, juce::ApplicationProperties& applicationProperties,
    TrackUi::TrackHasFilesCallback trackHasFilesCallback, TrackRemovedCallback trackRemovedCallback,
    FileLoadedCallback fileLoadedCallback)
    : m_container(container)
    , m_applicationProperties(applicationProperties)
    , m_trackHasFilesCallback(trackHasFilesCallback)
    , m_trackRemovedCallback(trackRemovedCallback)
    , m_fileLoadedCallback(fileLoadedCallback)
{
    trackAdded(m_container[0]);
    m_container.setTrackAddedCallback(std::bind(&TracksComponent::trackAdded, this, std::placeholders::_1));
    m_container.addLongestDurationChangedCallback([&](double duration) {
        for (const auto& track : m_tracks)
            track->setLongestDuration(duration);
    });
    m_container.setTracksClearedCallback(std::bind(&TracksComponent::tracksCleared, this));

    m_container.addTrackRemovedCallback(std::bind(&TracksComponent::trackRemoved, this, std::placeholders::_1));
}

void TracksComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int y = 0;
    int height = 100;
    for (size_t i = 0; i < m_tracks.size(); i++)
    {
        m_tracks[i]->setBounds(0, y, getWidth(), height);
        y += height;
    }
    setBounds(getX(), getY(), getWidth(), y);
}

bool TracksComponent::isInterestedInFileDrag(const juce::StringArray& /*files*/)
{
    return true;
}

void TracksComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    Component* component = getComponentAt(x, y);

    if (component->getName() != "TrackUi")
        return;

    const TrackUi* trackUi = static_cast<TrackUi*>(component);

    auto trackUiIt = std::find_if(m_tracks.begin(),
        m_tracks.end(),
        [trackUi](const std::unique_ptr<TrackUi>& uniquePointer) { return uniquePointer.get() == trackUi; });
    auto fileIt = files.begin();

    for (; trackUiIt != m_tracks.end(); ++trackUiIt)
    {
        while (fileIt != files.end() && !isAudioFile(*fileIt))
        {
            ++fileIt;
        }

        if (fileIt == files.end())
            break;

        (*trackUiIt)->loadFile(juce::File(*fileIt));
    }
}

void TracksComponent::addMonoTrack()
{
    m_container.addTrack(false);
}

void TracksComponent::addStereoTrack()
{
    m_container.addTrack(true);
}

void TracksComponent::trackAdded(Track& track)
{
    m_tracks.push_back(std::make_unique<TrackUi>(track,
        m_applicationProperties,
        std::bind(&TracksContainer::setPosition, &m_container, std::placeholders::_1),
        std::bind(&TracksContainer::removeTrack, &m_container, std::placeholders::_1),
        m_trackHasFilesCallback,
        m_fileLoadedCallback));
    addAndMakeVisible(*m_tracks.back());
    resized();
}

void TracksComponent::tracksCleared()
{
    m_tracks.clear();
    removeAllChildren();
    resized();
}

void TracksComponent::trackRemoved(int trackIndex)
{
    auto it = std::next(m_tracks.begin(), trackIndex);
    it = m_tracks.erase(it);
    for (; it != m_tracks.end(); ++it)
        (*it)->updateIdText();
    resized();
    repaint();
    m_trackRemovedCallback(trackIndex);
}
