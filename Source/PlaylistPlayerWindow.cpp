#include <algorithm>
#include <memory>
#include <vector>

#include "BinaryData.h"

#include "JinglePlayerWindow.h"
#include "Player.h"
#include "PlaylistPlayerWindow.h"

using namespace InterPlayerCommunication;

PlaylistPlayerWindow::PlaylistPlayerWindow(Player& player, TracksContainer* tracksContainer, bool showPlaylist,
    const ShowEditDialogCallback& showEditDialogCallback, const ConfigureChannelsCallback& configureChannelsCallback,
    const ConfigureMidiCallback& configureMidiCallback, const ChangePlayerTypeCallback& changePlayerTypeCallback,
    PlaylistModel& playlistModel, juce::ApplicationProperties& applicationProperties)
    : m_player(player)
    , m_color(0xffffffff)
    , m_tracksContainer(tracksContainer)
    , m_showEditDialogCallback(showEditDialogCallback)
    , m_configureChannelsCallback(configureChannelsCallback)
    , m_configureMidiCallback(configureMidiCallback)
    , m_changePlayerTypeCallback(changePlayerTypeCallback)
    , m_playButton("Play")
    , m_pauseButton("Pause")
    , m_stopButton("Stop")
    , m_skipBackwardButton("Skip Backward")
    , m_skipForwardButton("Skip Forward")
    , m_configureButton("Configure")
    , m_digitalDisplay({}, "00:00:00")
    , m_tracks(*m_tracksContainer, applicationProperties,
          std::bind(&PlaylistModel::trackHasFiles, &playlistModel, std::placeholders::_1),
          std::bind(&PlaylistModel::removeTrack, &playlistModel, std::placeholders::_1),
          std::bind(&PlaylistPlayerWindow::fileLoaded, this, std::placeholders::_1))
    , m_tableListBox([&](const std::vector<TrackConfig>& trackConfigs, bool play,
                         int index) { m_player.playlistEntryChanged(trackConfigs, play, index); },
          playlistModel)
    , m_resizeBar(&m_layout, 1, false)
{
    // play button
    juce::Image normalImage
        = juce::ImageFileFormat::loadFrom(BinaryData::mediaplaybackstart_png, BinaryData::mediaplaybackstart_pngSize);
    m_playButton.addListener(this);
    m_playButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_playButton);

    // pause button
    m_pauseButton.addListener(this);
    normalImage
        = juce::ImageFileFormat::loadFrom(BinaryData::mediaplaybackpause_png, BinaryData::mediaplaybackpause_pngSize);
    m_pauseButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_pauseButton);

    // stop button
    m_stopButton.addListener(this);
    normalImage
        = juce::ImageFileFormat::loadFrom(BinaryData::mediaplaybackstop_png, BinaryData::mediaplaybackstop_pngSize);
    m_stopButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_stopButton);

    // skip backward button
    m_skipBackwardButton.addListener(this);
    normalImage
        = juce::ImageFileFormat::loadFrom(BinaryData::mediaskipbackward_png, BinaryData::mediaskipbackward_pngSize);
    m_skipBackwardButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_skipBackwardButton);

    // skip forward button
    m_skipForwardButton.addListener(this);
    normalImage
        = juce::ImageFileFormat::loadFrom(BinaryData::mediaskipforward_png, BinaryData::mediaskipforward_pngSize);
    m_skipForwardButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_skipForwardButton);

    // configuration button
    normalImage = juce::ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
    m_configureButton.setImages(true,
        true,
        true,
        normalImage,
        0.7f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::transparentBlack,
        normalImage,
        1.0f,
        juce::Colours::pink.withAlpha(0.8f),
        0.0f);
    addAndMakeVisible(m_configureButton);
    m_configureButton.addMouseListener(this, false);

    // playback time display
    addAndMakeVisible(m_digitalDisplay);

    addAndMakeVisible(m_tracksViewport);
    m_tracksViewport.setViewedComponent(&m_tracks, false);
    m_tracksViewport.setScrollBarsShown(true, false, false, false);

    // playlist
    addAndMakeVisible(m_tableListBox);
    m_tableListBox.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
    m_tableListBox.setOutlineThickness(1);
    m_tableListBox.setVisible(showPlaylist);
    m_tableListBox.selectRow(0);

    // tracks
    Track::PositionCallback positionCallback = [&](double position, bool /*finished*/) {
        m_digitalDisplay.setText(Utils::formatSeconds(position), juce::sendNotification);
    };
    m_tracksContainer->addPositionCallback(positionCallback);

    m_tracksContainer->addLongestDurationChangedCallback(
        std::bind(&PlaylistTable::setCurrentDuration, &m_tableListBox, std::placeholders::_1));

    addAndMakeVisible(m_resizeBar);

    m_layout.setItemLayout(0, 60, -1.0, -0.3); // playlist
    m_layout.setItemLayout(1, 7, 7, 7); // resize bar
    m_layout.setItemLayout(2, 100, -1.0, -0.7); // tracks
}

int PlaylistPlayerWindow::getResizerBarPosition() const
{
    return m_layout.getItemCurrentPosition(1);
}

void PlaylistPlayerWindow::setResizerBarPosition(int position)
{
    m_layout.setItemPosition(1, position);
    resized();
}

void PlaylistPlayerWindow::paint(juce::Graphics& g)
{
    const int buttonWidth = std::min(getWidth() / 11, 32);
    const int buttonHeight = buttonWidth;

    g.fillAll(m_color);

    g.drawLine(
        0.0f, static_cast<float>(buttonHeight), static_cast<float>(getWidth()), static_cast<float>(buttonHeight));
}

void PlaylistPlayerWindow::resized()
{
    int buttonWidth = std::min(getWidth() / 11, 32);
    int buttonHeight = buttonWidth;

#define PLACE_BUTTON(IDX, BTN) BTN.setBounds(IDX* buttonWidth + 3, 3, buttonWidth - 6, buttonHeight - 6);
    PLACE_BUTTON(0, m_playButton);
    PLACE_BUTTON(1, m_pauseButton);
    PLACE_BUTTON(2, m_stopButton);
    PLACE_BUTTON(3, m_skipBackwardButton);
    PLACE_BUTTON(4, m_skipForwardButton);
    PLACE_BUTTON(5, m_configureButton);
#undef PLACE_BUTTON
    m_digitalDisplay.setBounds(6 * buttonWidth + 3, 3, buttonWidth * 3, buttonHeight - 6);

    if (m_tableListBox.isVisible())
    {
        juce::Component* components[] = {&m_tableListBox, &m_resizeBar, &m_tracksViewport};
        m_layout.layOutComponents(components, 3, 0, buttonHeight, getWidth(), getHeight() - buttonHeight, true, true);
    }
    else
    {
        m_tracksViewport.setBounds(0, buttonHeight, getWidth(), getHeight() - buttonHeight);
    }

    m_tracks.setBounds(0, 0, m_tracksViewport.getMaximumVisibleWidth(), m_tracks.getHeight());
}

void PlaylistPlayerWindow::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent != &m_configureButton)
        return;

    juce::PopupMenu m;
    m.addItem(TRANS("add stereo track"), true, false, [this]() {
        m_tracks.addStereoTrack();
        repaint();
        parentSizeChanged();
    });
    m.addItem(TRANS("add mono track"), true, false, [this]() {
        m_tracks.addMonoTrack();
        repaint();
    });
    m.addItem(TRANS("configure channels"), true, false, [this]() { m_configureChannelsCallback(); });
    m.addItem(TRANS("configure appearance"), true, false, [this]() { m_showEditDialogCallback(); });
    m.addItem(TRANS("configure MIDI"), true, false, [this]() { m_configureMidiCallback(); });
    m.addSeparator();
    m.addItem(TRANS("Jingle Mode"), true, false, [this]() { m_changePlayerTypeCallback(PlayerType::Jingle); });
    m.addItem(TRANS("Multitrack Mode"), true, !m_tableListBox.isVisible(), [this]() {
        m_changePlayerTypeCallback(PlayerType::Multitrack);
    });
    m.addItem(TRANS("Playlist Mode"), true, m_tableListBox.isVisible(), [this]() {
        m_changePlayerTypeCallback(PlayerType::Playlist);
    });

    m.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(m_configureButton));
}

void PlaylistPlayerWindow::buttonClicked(juce::Button* button)
{
    if (button == &m_playButton)
        m_player.play();
    else if (button == &m_pauseButton)
        m_player.pause();
    else if (button == &m_stopButton)
        m_player.stop();
    else if (button == &m_skipBackwardButton)
        m_player.previousEntry();
    else if (button == &m_skipForwardButton)
        m_player.nextEntry();
}

void PlaylistPlayerWindow::setColor(const juce::Colour& color)
{
    m_color = color;
    m_tableListBox.setColour(juce::ListBox::backgroundColourId, m_color);
    m_digitalDisplay.setColour(
        juce::Label::textColourId, m_color.isTransparent() ? juce::Colours::black : m_color.contrasting());
    repaint();
}

void PlaylistPlayerWindow::setShowPlaylist(bool showPlaylist)
{
    m_tableListBox.setVisible(showPlaylist);
    m_resizeBar.setVisible(showPlaylist);
    resized();
    repaint();
}

int PlaylistPlayerWindow::getSelectedRow() const
{
    return m_tableListBox.getSelectedRow();
}

void PlaylistPlayerWindow::fileLoaded(const juce::String& filename)
{
    static_cast<PlaylistModel*>(m_tableListBox.getModel())->setTrackNameIfEmpty(getSelectedRow(), filename);
}

void PlaylistPlayerWindow::nextPlaylistEntry(bool onlyIfEntrySaysSo)
{
    m_tableListBox.next(onlyIfEntrySaysSo);
}

void PlaylistPlayerWindow::previousPlaylistEntry()
{
    m_tableListBox.previous();
}
