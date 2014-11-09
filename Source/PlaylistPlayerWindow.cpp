#include <vector>
#include <algorithm>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaylistPlayerWindow.h"
#include "JinglePlayerWindow.h"

PlaylistPlayerWindow::PlaylistPlayerWindow(MixerComponent* mixer, int outputChannels, float gain, bool solo, bool mute)
	: m_mixer(mixer)
	, m_outputChannels(outputChannels)
	, m_thread("audio file preview")
	, m_gain(1.0f)
	, m_solo(solo)
	, m_soloMute(false)
	, m_mute(mute)
	, m_color(0xffffffff)
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
	
	// seek backward button
	m_seekBackwardButton = new ImageButton("Seek Backward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaseekbackward_png, BinaryData::mediaseekbackward_pngSize);
	m_seekBackwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_seekBackwardButton);
	
	// seek forward button
	m_seekForwardButton = new ImageButton("Seek Forward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaseekforward_png, BinaryData::mediaseekforward_pngSize);
	m_seekForwardButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_seekForwardButton);
	
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
	m_digitalDisplay->setColour(Label::backgroundColourId, Colours::white);

	// tracks
	addAndMakeVisible(m_tracksViewport = new Viewport());
	m_tracksViewport->setViewedComponent(m_tracks = new TracksComponent(mixer, outputChannels, [&](double position) {
		m_digitalDisplay->setText(Utils::formatSeconds(position), sendNotification);
	}), false);
	m_tracksViewport->setScrollBarsShown(true, false, false, false);

	setGain(gain);
	mixer->registerPlayer(this);

	setBounds(0, 0, 600, 300);
}

PlaylistPlayerWindow::~PlaylistPlayerWindow()
{
	m_mixer->unregisterPlayer(this);
}

void PlaylistPlayerWindow::setGain(float gain)
{
	m_tracks->setGain(gain);
}

float PlaylistPlayerWindow::getGain()
{
	return m_tracks->getGain();
}

void PlaylistPlayerWindow::setPan(float pan)
{

}

float PlaylistPlayerWindow::getPan()
{
	return 1.0f;
}

void PlaylistPlayerWindow::setSoloMute(bool soloMute)
{
	m_soloMute = soloMute;
	updateGain();
}

bool PlaylistPlayerWindow::getSoloMute()
{
	return m_soloMute;
}

void PlaylistPlayerWindow::setSolo(bool solo)
{
	m_solo = solo;
	updateGain();
}

bool PlaylistPlayerWindow::getSolo()
{
	return m_solo;
}

void PlaylistPlayerWindow::setMute(bool mute)
{
	m_mute = mute;
	updateGain();
}

bool PlaylistPlayerWindow::getMute()
{
	return m_mute;
}

void PlaylistPlayerWindow::updateGain()
{
	bool mute = m_mute || (m_soloMute && !m_solo);
	m_tracks->setMute(mute);
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
	PLACE_BUTTON(3, m_seekBackwardButton);
	PLACE_BUTTON(4, m_seekForwardButton);
	PLACE_BUTTON(5, m_skipBackwardButton);
	PLACE_BUTTON(6, m_skipForwardButton);
	PLACE_BUTTON(7, m_configureButton);
#undef PLACE_BUTTON
	m_digitalDisplay->setBounds(8 * buttonWidth + 3, 3, buttonWidth * 3, buttonHeight - 6);

	m_tracksViewport->setBounds(0, buttonHeight, getWidth(), getHeight() - buttonHeight);
	m_tracks->setBounds(0, 0, m_tracksViewport->getMaximumVisibleWidth(), m_tracks->getHeight());
}

void PlaylistPlayerWindow::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	m_tracks->setOutputChannels(m_outputChannels);
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
		configureChannels();
		break;
	case 4:
		{
			if (m_renameDialog.get() == nullptr) {
				m_renameDialog.set(new RenameDialogWindow(getName(), m_color, "", [this](String name) {
					setName(name);
				}, [this](Colour color) {
					m_color = color;
					m_mixer->updatePlayerColor(this, m_color);
					repaint();
				}, [&]() {
					// clear is not working
					delete m_renameDialog.release();
				}), true);
			}
			m_renameDialog->addToDesktop();
			m_renameDialog->toFront(true);
		}
	}
}

void PlaylistPlayerWindow::buttonClicked(Button * button)
{
	if (button == m_playButton)
		m_tracks->play();
	else if (button == m_pauseButton)
		m_tracks->pause();
	else if (button == m_stopButton)
		m_tracks->stop();
}

XmlElement* PlaylistPlayerWindow::saveToXml() const
{
    XmlElement* element = new XmlElement("Player");
	element->setAttribute("type", "playlist");
	element->setAttribute("gain", m_tracks->getGain());
	element->setAttribute("mute", m_mute);
	element->setAttribute("solo", m_solo);
	element->setAttribute("color", m_color.toString());

	Rectangle<int> bounds = getParentComponent()->getBounds();

	XmlElement* boundsXml = new XmlElement("Bounds");
	boundsXml->setAttribute("x", bounds.getX());
	boundsXml->setAttribute("y", bounds.getY());
	boundsXml->setAttribute("width", bounds.getWidth());
	boundsXml->setAttribute("height", bounds.getHeight());
	element->addChildElement(boundsXml);

	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);

	XmlElement* tracksXml = new XmlElement("Tracks");
	for (int i = 0; i < m_tracks->playerCount(); ++i)
		tracksXml->addChildElement(m_tracks->player(i).saveToXml());

	element->addChildElement(tracksXml);

	return element;
}

void PlaylistPlayerWindow::restoreFromXml (const XmlElement& element)
{
	m_color = Colour::fromString(element.getStringAttribute("color", "0xffffffff"));
	m_mixer->updatePlayerColor(this, m_color);
	repaint();

	XmlElement* boundsXml = element.getChildByName("Bounds");

	String x = boundsXml->getStringAttribute("x", "0");
	String y = boundsXml->getStringAttribute("y", "0");
	String width = boundsXml->getStringAttribute("width", "150");
	String height = boundsXml->getStringAttribute("height", "150");
	getParentComponent()->setBounds(x.getIntValue(), y.getIntValue(), width.getIntValue(), height.getIntValue());

	XmlElement* nameXml = element.getChildByName("Name");
	setName(nameXml->getAllSubText().trim());

	XmlElement* tracksXml = element.getChildByName("Tracks");
	for (int i = 0; i < tracksXml->getNumChildElements(); ++i)
		m_tracks->addTrackFromXml(tracksXml->getChildElement(i));
}

void PlaylistPlayerWindow::configureChannels()
{
	if (m_channelMappingWindow.get() == nullptr) {
		std::vector<int> mapping;
		for (int i = 0; i < m_tracks->playerCount(); ++i) {
			std::vector<int> playerMapping = m_tracks->player(i).getMapping();
			mapping.insert(mapping.end(), playerMapping.begin(), playerMapping.end());
		}

		m_channelMappingWindow.set(new ChannelMappingWindow(m_outputChannels, mapping, [&](int source, int target) {
			for (int i = 0; i < m_tracks->playerCount(); ++i) {
				if (source - m_tracks->player(i).getNumChannels() < 0) {
					m_tracks->player(i).setOutputChannelMapping(source, target);
					break;
				}
				source -= m_tracks->player(i).getNumChannels();
			}
		}, [&]() {
			// clear is not working
			delete m_channelMappingWindow.release();
		}), true);
	}
	m_channelMappingWindow->addToDesktop();
	m_channelMappingWindow->toFront(true);
}