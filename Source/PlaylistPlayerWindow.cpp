#include <vector>
#include <algorithm>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaylistPlayerWindow.h"
#include "JinglePlayerWindow.h"

using namespace InterPlayerCommunication;

PlaylistPlayerWindow::PlaylistPlayerWindow(TracksContainer* tracksContainer, OutputChannelNames *outputChannelNames, bool showPlaylist, ShowEditDialogCallback showEditDialogCallback, ConfigureChannelsCallback configureChannelsCallback, ChangePlayerTypeCallback changePlayerTypeCallback, PlaylistModel& playlistModel, ApplicationProperties& applicationProperties)
	: m_color(0xffffffff)
	, m_tracksContainer(tracksContainer)
	, m_outputChannelNames(outputChannelNames)
	, m_showEditDialogCallback(showEditDialogCallback)
	, m_configureChannelsCallback(configureChannelsCallback)
	, m_changePlayerTypeCallback(changePlayerTypeCallback)
{	
	// play button
	m_playButton = new ImageButton("Play");
	Image normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstart_png, BinaryData::mediaplaybackstart_pngSize);
	m_playButton->addListener(this);
	m_playButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_playButton);
	
	// pause button
	m_pauseButton = new ImageButton("Pause");
	m_pauseButton->addListener(this);
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackpause_png, BinaryData::mediaplaybackpause_pngSize);
	m_pauseButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_pauseButton);
	
	// stop button
	m_stopButton = new ImageButton("Stop");
	m_stopButton->addListener(this);
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstop_png, BinaryData::mediaplaybackstop_pngSize);
	m_stopButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_stopButton);
	
	// skip backward button
	m_skipBackwardButton = new ImageButton("Skip Backward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipbackward_png, BinaryData::mediaskipbackward_pngSize);
	m_skipBackwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_skipBackwardButton);
	
	// skip forward button
	m_skipForwardButton = new ImageButton("Skip Forward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipforward_png, BinaryData::mediaskipforward_pngSize);
	m_skipForwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_skipForwardButton);

	// configuration button
	m_configureButton = new ImageButton("Configure");
	normalImage = ImageFileFormat::loadFrom (BinaryData::configure_png, BinaryData::configure_pngSize);
	m_configureButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_configureButton);
	m_configureButton->addMouseListener(this, false);

	// playback time display
	addAndMakeVisible(m_digitalDisplay = new Label(String::empty, "00:00:00"));
	m_digitalDisplay->setFont(Font(Font::getDefaultMonospacedFontName(), 14, Font::plain));
	m_digitalDisplay->setColour(Label::textColourId, Colours::red);
	m_digitalDisplay->setColour(Label::backgroundColourId, m_color.darker());


	addAndMakeVisible(m_tracksViewport = new Viewport());
	m_tracksViewport->setViewedComponent(m_tracks = new TracksComponent(*m_tracksContainer, applicationProperties), false);
	m_tracksViewport->setScrollBarsShown(true, false, false, false);

	// playlist

	PlaylistTable::PlaylistEntryChangedCallback playlistCallback = [&](const std::vector<TrackConfig>& trackConfigs, bool play) {
		std::vector<TrackConfig> oldTrackConfigs = m_tracksContainer->getTrackConfigs();
		m_tracksContainer->setTrackConfigs(trackConfigs);
		if (play)
			m_playButton->triggerClick();
		return oldTrackConfigs;
	};

	addAndMakeVisible(m_tableListBox = new PlaylistTable(playlistCallback, playlistModel));
	m_tableListBox->setColour(ListBox::outlineColourId, Colours::grey);
	m_tableListBox->setOutlineThickness(1);
	m_tableListBox->setVisible(showPlaylist);

	// tracks
	Track::PositionCallback positionCallback = [&](double position, bool finished) {
		m_digitalDisplay->setText(Utils::formatSeconds(position), sendNotification);
		if (finished && m_tableListBox->isVisible())
			m_tableListBox->next();
	};
	m_tracksContainer->addPositionCallback(positionCallback);

	TracksContainer::LongestDurationChangedCallback longestDurationCallback = [&](double duration) {
		m_tableListBox->setCurrentDuration(duration);
	};
	m_tracksContainer->addLongestDurationChangedCallback(longestDurationCallback);
}

void PlaylistPlayerWindow::paint (Graphics& g)
{
	int buttonWidth = std::min(getWidth() / 11, 32);
	int buttonHeight = buttonWidth;

	g.fillAll(m_color);

	g.drawLine(0.0f, static_cast<float>(buttonHeight), static_cast<float>(getWidth()), static_cast<float>(buttonHeight));
}

void PlaylistPlayerWindow::resized()
{
	int buttonWidth = std::min(getWidth() / 11, 32);
	int buttonHeight = buttonWidth;

#define PLACE_BUTTON(IDX,BTN) BTN->setBounds(IDX * buttonWidth + 3, 3, buttonWidth - 6, buttonHeight - 6);
	PLACE_BUTTON(0, m_playButton);
	PLACE_BUTTON(1, m_pauseButton);
	PLACE_BUTTON(0, m_playButton);
	PLACE_BUTTON(1, m_pauseButton);
	PLACE_BUTTON(2, m_stopButton);
	PLACE_BUTTON(3, m_skipBackwardButton);
	PLACE_BUTTON(4, m_skipForwardButton);
	PLACE_BUTTON(5, m_configureButton);
#undef PLACE_BUTTON
	m_digitalDisplay->setBounds(5 * buttonWidth + 3, 3, buttonWidth * 3, buttonHeight - 6);

	int top = buttonHeight;
	if (m_tableListBox->isVisible()) {
		int playlistHeight = 100;
		m_tableListBox->setBounds(0, top, getWidth(), playlistHeight);
		top += playlistHeight;
	}

	m_tracksViewport->setBounds(0, top, getWidth(), getHeight() - top);
	m_tracks->setBounds(0, 0, m_tracksViewport->getMaximumVisibleWidth(), m_tracks->getHeight());
}

void PlaylistPlayerWindow::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent != m_configureButton)
		return;

	PopupMenu m;
	m.addItem (1, "add stereo track");
	m.addItem (2, "add mono track");
	m.addItem (3, "configure channels");
	m.addItem (4, "rename");
	m.addSeparator();
	m.addItem(5, "Jingle Mode");
	m.addItem(6, "Multitrack Mode", true, !m_tableListBox->isVisible());
	m.addItem(7, "Playlist Mode", true, m_tableListBox->isVisible());
	const int result = m.show();

	switch (result) {
	case 1:
		m_tracks->addStereoTrack();
		repaint();
		parentSizeChanged();
		break;
	case 2:
		m_tracks->addMonoTrack();
		repaint();
		break;
	case 3:
		m_configureChannelsCallback();
		break;
	case 4:
		m_showEditDialogCallback();
		break;
	case 5:
		m_changePlayerTypeCallback(PlayerType::Jingle);
		break;
	case 6:
		m_changePlayerTypeCallback(PlayerType::Multitrack);
		break;
	case 7:
		m_changePlayerTypeCallback(PlayerType::Playlist);
		break;
	}
}

void PlaylistPlayerWindow::buttonClicked(Button * button)
{
	if (button == m_playButton)
		m_tracksContainer->play();
	else if (button == m_pauseButton)
		m_tracksContainer->pause();
	else if (button == m_stopButton)
		m_tracksContainer->stop();
}

void PlaylistPlayerWindow::setColor(Colour color)
{
	m_color = color;
	m_digitalDisplay->setColour(Label::backgroundColourId, m_color.darker());
	repaint();
}

void PlaylistPlayerWindow::setShowPlaylist(bool showPlaylist)
{
	m_tableListBox->setVisible(showPlaylist);
	resized();
	repaint();
}