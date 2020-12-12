#include <list>
#include <memory>
#include <stdint.h>

#include "BinaryData.h"

#include "JinglePlayerWindow.h"
#include "Player.h"

using namespace InterPlayerCommunication;

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

bool isImageFile(const juce::String& filePath)
{
    return filePath.endsWithIgnoreCase(".jpg") || filePath.endsWithIgnoreCase(".png");
}
const int ProgressBarHeight = 26;
const int TotalDurationTextWidth = 70;
}

JinglePlayerWindow::JinglePlayerWindow(Player& player, TracksContainer* tracksContainer,
    ShowEditDialogCallback showEditDialogCallback, ConfigureChannelsCallback configureChannelsCallback,
    ConfigureMidiCallback configureMidiCallback, ChangePlayerTypeCallback changePlayerTypeCallback,
    SetUserImageCallback setUserImageCallback)
    : m_player(player)
    , m_tracksContainer(tracksContainer)
    , m_playImage(juce::Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize))
    , m_stopImage(juce::Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize))
    , m_showRemainingTime(false)
    , m_blink(false)
    , m_paintColor(m_color)
    , m_showEditDialogCallback(showEditDialogCallback)
    , m_configureChannelsCallback(configureChannelsCallback)
    , m_configureMidiCallback(configureMidiCallback)
    , m_changePlayerTypeCallback(changePlayerTypeCallback)
    , m_setUserImageCallback(setUserImageCallback)
    , m_totalLength(0)
    , m_progress(0)
    , m_progressBar(m_progress)
    , m_playButton("Play", juce::DrawableButton::ImageFitted)
    , m_configureButton("Configure")
    , m_fileNameLabel("filename label")
    , m_waveform(juce::Colour(0x55000000))
{
    m_formatManager.registerBasicFormats();

    addAndMakeVisible(m_waveform);

    // progress bar
    m_progressBar.setPercentageDisplay(false);
    m_progressBar.setTextToDisplay("00:00:000");
    m_progressBar.addMouseListener(this, false);
    m_progressBar.setColour(juce::ProgressBar::backgroundColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(m_progressBar);

    // total duration text
    addAndMakeVisible(m_totalDurationText);
    m_totalDurationText.setJustificationType(juce::Justification::centredRight);
    m_totalDurationText.setText("00:00:000", juce::sendNotification);

    // play button
    addAndMakeVisible(m_playButton);
    m_playButton.setImages(m_playImage.get());
    m_playButton.addListener(this);
    m_playButton.setEdgeIndent(30);

    // configuration button
    juce::Image normalImage = juce::ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
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

    // filename label
    m_fileNameLabel.setColour(juce::Label::textColourId, m_paintColor.contrasting(1.0f));
    m_fileNameLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(m_fileNameLabel);

    Track::PositionCallback positionCallback = [&](double position, bool finished) {
        const double remainingTime = m_totalLength - position;

        m_progress = finished ? 0.0 : position / m_totalLength;

        m_progressBar.setTextToDisplay(Utils::formatSeconds(m_showRemainingTime ? remainingTime : position));
        m_totalDurationText.setText(Utils::formatSeconds(m_totalLength), juce::sendNotification);

        if (remainingTime < 10)
        {
            const double decimal = remainingTime - static_cast<long>(remainingTime);
            const bool blink = decimal >= 0.5;
            if (blink != m_blink)
            {
                m_blink = blink;
                updatePointColor();
                repaint();
            }
        }

        if (finished)
            m_playButton.setImages(m_playImage.get());
    };
    m_tracksContainer->addPositionCallback(positionCallback);

    TracksContainer::LongestDurationChangedCallback longestDurationCallback = [&](double duration) {
        m_totalDurationText.setText(Utils::formatSeconds(duration), juce::sendNotification);
        m_totalLength = duration;
    };
    m_tracksContainer->addLongestDurationChangedCallback(longestDurationCallback);

    Track::PlayingStateChangedCallback playingStateChangedCallback = [&](bool isPlaying) {
        m_playButton.setImages(isPlaying ? m_stopImage.get() : m_playImage.get());
        m_blink = false;
        updatePointColor();
        repaint();
    };
    m_tracksContainer->addPlayingStateChangedCallback(playingStateChangedCallback);
}

void JinglePlayerWindow::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    repaint();
}

bool JinglePlayerWindow::isInterestedInFileDrag(const juce::StringArray& /*files*/)
{
    return true;
}

void JinglePlayerWindow::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{
    auto fileIt = files.begin();

    for (size_t trackIndex = 0; trackIndex < m_tracksContainer->size(); ++trackIndex)
    {
        while (fileIt != files.end() && !isAudioFile(*fileIt))
        {
            ++fileIt;
        }

        if (fileIt == files.end())
            break;

        (*m_tracksContainer)[trackIndex].loadFileIntoTransport(*fileIt);
    }

    fileIt = files.begin();
    while (fileIt != files.end() && !isImageFile(*fileIt))
    {
        ++fileIt;
    }

    if (fileIt != files.end())
    {
        m_setUserImageCallback(*fileIt);
    }
}

void JinglePlayerWindow::resized()
{
    m_configureButton.setBounds(0, getHeight() - ProgressBarHeight, ProgressBarHeight, ProgressBarHeight);
    m_progressBar.setBounds(m_configureButton.getWidth(),
        getHeight() - ProgressBarHeight,
        getWidth() - m_configureButton.getWidth() - TotalDurationTextWidth,
        ProgressBarHeight);
    m_totalDurationText.setBounds(getWidth() - TotalDurationTextWidth,
        getHeight() - ProgressBarHeight,
        TotalDurationTextWidth,
        ProgressBarHeight);
    m_playButton.setBounds(0, 0, getWidth(), getHeight() - ProgressBarHeight);
    m_fileNameLabel.setBounds(0, getHeight() - ProgressBarHeight - ProgressBarHeight, getWidth(), ProgressBarHeight);

    m_waveform.setBounds(m_configureButton.getWidth(),
        getHeight() - ProgressBarHeight,
        getWidth() - m_configureButton.getWidth() - TotalDurationTextWidth,
        ProgressBarHeight);
}

void JinglePlayerWindow::paint(juce::Graphics& g)
{
    g.fillAll(m_paintColor);

    if (m_userImage)
        m_userImage->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);

    m_waveform.setAudioThumbnail(&(*m_tracksContainer)[0].getAudioThumbnail());

    juce::Component::paint(g);
}

void JinglePlayerWindow::mouseDown(const juce::MouseEvent& event)
{
    if (event.eventComponent == &m_progressBar)
    {
        m_showRemainingTime = !m_showRemainingTime;
    }
    if (event.eventComponent == &m_configureButton)
    {
        juce::PopupMenu m;
        m.addItem(1, TRANS("load file"));
        m.addItem(2, TRANS("configure channels"));
        m.addItem(3, TRANS("configure appearance"));
        m.addItem(7, TRANS("configure MIDI"));
        m.addSeparator();
        m.addItem(4, TRANS("Jingle Mode"), true, true);
        m.addItem(5, TRANS("Multitrack Mode"));
        m.addItem(6, TRANS("Playlist Mode"));

        const int result = m.show();

        switch (result)
        {
        case 1:
            loadFile();
            break;
        case 2:
            m_configureChannelsCallback();
            break;
        case 3:
            m_showEditDialogCallback();
            break;
        case 4:
            m_changePlayerTypeCallback(PlayerType::Jingle);
            break;
        case 5:
            m_changePlayerTypeCallback(PlayerType::Multitrack);
            break;
        case 6:
            m_changePlayerTypeCallback(PlayerType::Playlist);
            break;
        case 7:
            m_configureMidiCallback();
            break;
        }
    }
}

void JinglePlayerWindow::loadFile()
{
    juce::FileChooser myChooser(TRANS("Please select the audio file you want to load ..."),
        juce::File(),
        m_formatManager.getWildcardForAllFormats());

    if (!myChooser.browseForFileToOpen())
        return;

    juce::File audioFile = juce::File(myChooser.getResult());
    (*m_tracksContainer)[0].loadFileIntoTransport(audioFile);
}

void JinglePlayerWindow::buttonClicked(juce::Button* /*button*/)
{
    if (m_tracksContainer->isPlaying())
    {
        m_playButton.setImages(m_playImage.get());
        m_player.stop();
    }
    else
    {
        m_playButton.setImages(m_stopImage.get());
        m_player.play();
    }
}

void JinglePlayerWindow::updatePointColor()
{
    m_paintColor = m_blink ? m_color.contrasting(0.5f) : m_color;

    m_fileNameLabel.setColour(juce::Label::textColourId, m_paintColor.contrasting(1.0f));
}

void JinglePlayerWindow::setColor(const juce::Colour& color)
{
    m_color = color;
    updatePointColor();
    repaint();
}

void JinglePlayerWindow::setUserImage(const juce::File& file)
{
    if (file == juce::File())
        m_userImage.reset();
    else
        m_userImage = juce::Drawable::createFromImageFile(file);
    repaint();
}
